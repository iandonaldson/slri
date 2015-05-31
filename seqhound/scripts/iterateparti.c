/* $Id: iterateparti.c,v 1.6 2005/02/01 21:52:29 zwang Exp $
*******************************************************************************************
  Copyright (C) 2000 Mount Sinai Hospital (MSH)

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of
  the License, or any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

  See the GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the
         Free Software Foundation, Inc.,
         59 Temple Place, Suite 330, Boston, MA
         02111-1307  USA
  or visit http://www.gnu.org/copyleft/gpl.html

  SPECIAL EXCEPTIONS

  As a special exception, Mount Sinai Hospital gives permission to
  link this program with the following non-GPL programs or libraries,
  and distribute the resulting executable, without including the source
  code for these in the source distribution:

  a) CodeBase 6.5 or greater from Sequiter Software Inc.

  b) The C or C++ interface to Oracle 8.x or greater, from
     Oracle Corporation or IBM DB2 UDB.


  PROGRAM:


  AUTHORS:
           Katerina Michalickova and Christopher W.V. Hogue hogue@mshri.on.ca


  REVISION/CONTRIBUTION NOTES:

  June 1, 2000  Original MSH Revision.
                Hogue Lab - University of Toronto Biochemistry
                Department and the Samuel Lunenfeld Research
                Institute, Mount Sinai Hospital
                http://bioinfo.mshri.on.ca  hogue@mshri.on.ca
                ftp://ftp.mshri.on.ca/


  PUBLICATION TO CITE:


  DESCRIPTION
  Iterates through a partition (defined by user input) and finds all GI's
  that are a part of that division.  If the user has specified that all GI's are 
  to be deleted for that division, then all records in TAXGI, PARTI, ACCDB, SENGI,
  PUBSEQ and NUCPROT tables with GI's from the specified division are removed.
******************************************************************************************/

#include <intrez_db.h>
#include <seqhound.h>


#define LOGING "iteratepartilog"

#define NUMARGS 2
Args myargs[NUMARGS] = {
       {"Partition to iterate", NULL, NULL, NULL, FALSE, 'p', ARG_STRING, 0.0, 0, NULL},
       {"Delete entries", "FALSE", NULL, NULL, TRUE, 'd', ARG_BOOLEAN, 0.0, 0, NULL}
	};

Int4 count=0;

static Boolean InternalDeleteGI(Int4 gi, CharPtr div, Boolean destroy)
{
   /*assume that all deletions will be successful*/
   Boolean delparti = TRUE;
   Boolean delaccdb = TRUE;
   Boolean delsengi = TRUE;
   Boolean deltaxgi = TRUE;

#ifdef ODBC
   /* delete from ASNDB and SENDB. 
      They are handled in function DeleteGIinPARTI and DeleteGIinSENGI 
      respectively in CodeBase version */
   if(!DeleteGIinASNDB(div, gi, destroy))
   {
      deltaxgi = FALSE;
      ErrLogPrintf("ERROR, GI %ld DIV %s not deleted properly in ASNDB.\n", gi, div);
   }
   if(!DeleteGIinSENDB(div, gi, destroy))
   {
      deltaxgi = FALSE;
      ErrLogPrintf("ERROR, GI %ld not deleted properly in SENDB.\n", gi);
   }
#endif

   /*delete gi from PARTI, ACCDB, SENGI, TAXGI, PUBSEQ, and NUCPROT*/
   if(!DeleteGIinPARTI(gi, destroy))
   {
      delparti = FALSE;
      ErrLogPrintf("ERROR, GI %ld not deleted properly in PARTI.\n", gi);
   }
   if (!DeleteGIinACCDB(gi, destroy))
   {
      delaccdb = FALSE;
      ErrLogPrintf("ERROR, GI %ld not deleted properly in ACCDB.\n", gi);
   }
   if(!DeleteGIinSENGI(gi, destroy))
   {
      delsengi = FALSE;
      ErrLogPrintf("ERROR, GI %ld not deleted properly in SENGI.\n", gi);
   }
   if(!DeleteGIinTAXGI(gi, destroy))
   {
      deltaxgi = FALSE;
      ErrLogPrintf("ERROR, GI %ld not deleted properly in TAXGI.\n", gi);
   }

   DeleteGIinPUBSEQ(gi,  destroy);
   DeleteGIinNUCPROT(gi, destroy);

   count++;

   if(!(delparti && delaccdb && delsengi && deltaxgi))
   {
       return FALSE;
   }

  return TRUE;
}


Int2 Main()
{
   ValNodePtr pvngi = NULL, pvngihead = NULL;

    ErrSetOptFlags(EO_LOG_SEVERITY | EO_LOG_CODES | EO_LOG_FILELINE | EO_LOG_USRFILE);
    ErrSetLogfile(LOGING, ELOG_BANNER);
    ErrSetLogLevel(SEV_INFO);
    ErrSetMessageLevel(SEV_MAX);

    if ( ! GetArgs("iterateparti",NUMARGS, myargs))
           return 1;


    if(!SHoundInit(FALSE, "iterateparti"))
    {
      ErrPostEx(SEV_ERROR,0,0, "SHoundInit failed.\n");
      return 1;
    }

    pvngihead = pvngi = SHoundAllPartition(myargs[0].strvalue);
    if(pvngi == NULL)
      ErrPostEx(SEV_INFO,0,0, "SHoundAllPartition returned NULL.\n");

   /* set autocommit off if this is an update */
   if (!StartTransaction()) {
       ErrPostEx(SEV_ERROR, 0, 0, "Cannot start transaction!");
       return 1;
   }

    while(pvngi != NULL)
    {
      if(myargs[1].intvalue)
      {
         if(!InternalDeleteGI(pvngi->data.intvalue,  myargs[0].strvalue, TRUE))
              ErrPostEx(SEV_ERROR,0,0, "InternalDeleteGI failed on %ld.\n", pvngi->data.intvalue);
      }

      if (count >= 1000){
	   count = 0;
	   if (!CommitTransaction()) {
              ErrPostEx(SEV_ERROR, 0, 0, "Cannot commit transaction!");
              if (!RollBackTransaction()) {
                 ErrPostEx(SEV_ERROR,0,0, "Cannot roll back!");
                 return 1;
              }
           }
      }
      pvngi = pvngi->next;
    }

    if (!CommitTransaction()) {
       ErrPostEx(SEV_ERROR, 0, 0, "Cannot commit transaction!");
       if (!RollBackTransaction()) {
          ErrPostEx(SEV_ERROR,0,0, "Cannot roll back!");
          return 1;
       }
    } 

    if(pvngihead != NULL) ValNodeFree(pvngihead);
    SHoundFini();
    ErrPostEx(SEV_INFO,0,0, "Done.\n");
    return 0;
}

/*
$Log: iterateparti.c,v $
Revision 1.6  2005/02/01 21:52:29  zwang
Added ability of building a transactional database.
See bug 3441.

Revision 1.5  2004/11/12 17:28:09  vgu
Add transaction support

Revision 1.4  2004/11/10 15:05:05  vgu
remove header file intrez_cb.h to deprecate codebase support

Revision 1.3  2004/07/15 22:12:37  zwang
Explicitly delete records from ASNDB and SEND in odbc.

Revision 1.2  2004/02/04 03:11:17  idonalds
Corrected logical error in error reporting

Revision 1.1.1.2  2002/11/11 09:35:07  root
Automated import from Sourceforge.net

Revision 1.2  2002/11/11 01:19:48  kaca
made it not exit if SHoundAllPartition returns NULL

Revision 1.1  2002/11/01 19:33:50  kaca
initial import; it deletes a partition from the system; use with caution

*/

