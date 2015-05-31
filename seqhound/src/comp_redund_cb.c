/* $Id: comp_redund_cb.c,v 1.4 2004/05/31 20:58:36 hlieu Exp $
**************************************************************************
  Copyright (C) 2001 Mount Sinai Hospital (MSH)

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
            Hao Lieu


  REVISION/CONTRIBUTION NOTES:

  March 1, 2001  Original MSH Revision.
                Hogue Lab - University of Toronto Biochemistry 
                Department and the Samuel Lunenfeld Research 
                Institute, Mount Sinai Hospital
                http://bioinfo.mshri.on.ca  hogue@mshri.on.ca
                ftp://ftp.mshri.on.ca/


  PUBLICATION TO CITE:

  DESCRIPTION:
  Part of SeqHound

****************************************************************************/
#include <comp_redund_db.h>
#include <comp_redund_db_p.h>
#include <slri_cb.h>
#include <slri_misc.h>
#include <objslristruc.h>
#include <d4all.h>     /* move to end to avoid _LARGE_FILE_SOURCE redefinition warnings */

/**************************************************
 *       PUBLIC CODE BASE INITIALIZTION           *
 **************************************************/

/* these functions initialize the codebase tags and field
   information.  Alter these only if you know what you are doing */

/* initializes the CODE4 structure and sets various flags.
   This function must be called in any application that uses 
   this database, prior to doing anything to the database.*/
void REDUND_InitCodeBase(void)
{
  code4init(&cb);
  cb.errOpen = 0;
  cb.safety = 0;
}

/* closes the databases.  This function must called at the
   end of your function, if you called LL_InitCodeBase.*/
void REDUND_InitUndoCodeBase(void)
{
  code4close(&cb);
  code4initUndo(&cb);
}

/***************************************************
           PRIVATE OPEN DATABASE FUNCTIONS
****************************************************/
/* opens and sets the fields and tags of HISTDB code base files */
static Boolean OpenRedund(CharPtr path, Boolean CREATE, RedundDBPtr pRed)
{
  CharPtr DBpath;

  /* SK: NULL ptr check */
  if ( path == NULL )
    return FALSE;
  /* SK: NULL ptr check */
  if ( pRed == NULL )
  {
    ErrPostEx(SEV_ERROR,0,0, "Error: NULL RedundDBPtr\n");
    return FALSE;
  }
  
  DBpath = AllocateAndCreateDBPath(path, "REDUND");

  /* SK: NULL ptr check */
  if ( DBpath == NULL )
    return FALSE;

  if(CREATE == FALSE){

    pRed->pd4redund = d4open(&cb, DBpath);
    if(pRed->pd4redund == NULL){
      MemFree(path);
      return FALSE;
    }
  } else{
    pRed->pd4redund = d4create(&cb, DBpath, fieldInfo_Redund, tagInfo_Redund);
    if(cb.errorCode){
      MemFree(path);
      return FALSE;
    }
  }
  pRed->pf4rdGi = d4field(pRed->pd4redund, "GI");
  pRed->pf4rdOrdinal = d4field(pRed->pd4redund, "ORDINAL");
  pRed->pf4rdGroup = d4field(pRed->pd4redund, "GROUP");

  pRed->pt4rdGi = d4tag(pRed->pd4redund, "GI");
  pRed->pt4rdOrdinal = d4tag(pRed->pd4redund, "ORDINAL");
  pRed->pt4rdGroup = d4tag(pRed->pd4redund, "GROUP");
  return TRUE;
}

/********************************************************
             PUBLIC OPEN DATABASE FUNCTIONS
*********************************************************/

/* opens all the databases associated with Histdb.  This
   function must be called prior to accessing the database. */
Boolean REDUND_OpenAllDBs(Boolean CREATE, RedundDBPtr pRed, CharPtr path_old, CharPtr path_new)
{
  Boolean res1 = FALSE;
  Boolean res2 = FALSE;

  /* SK: NULL ptr check */
  if ( pRed == NULL )
  {
    ErrPostEx(SEV_ERROR,0,0, "Error: NULL RedundDBPtr\n");
    return FALSE;
  }

  res1 = OpenRedund(path_new, CREATE, pRed);
  res2 = OpenRedund(path_old, CREATE, pRed->next);

  if(res1 && res2) return TRUE;
  else return FALSE;
}

/********************************************************
             PUBLIC CLOSE DATABASE FUNCTIONS
*********************************************************/
void CloseCodeBaseAndDBs(RedundDBPtr pRed)
{

  /* SK: NULL ptr check */
  if ( pRed == NULL )
  {
    ErrPostEx(SEV_ERROR,0,0, "Error: NULL RedundDBPtr\n");
    return;
  }

  /* explicity flush all DBs */
  if(pRed->pd4redund)   d4flush(pRed->pd4redund);
  d4close(pRed->pd4redund);
  /* close CodeBase */
  code4close(&cb);
  code4initUndo(&cb);
}
                  
Boolean CheckGI(Int4 gi, RedundDBPtr pRed){

  Int4 rc;
  if(pRed == NULL){

    ErrPostEx(SEV_ERROR,0,0, "Error: NULL RedundDBPtr\n");
    return FALSE;
  }
  d4tagSelect(pRed->pd4redund, pRed->pt4rdGi);
  d4top(pRed->pd4redund);
  rc = d4seekDouble(pRed->pd4redund, gi);
  if( rc == r4success){
    return TRUE;
  }
  return FALSE;
}


/* parses from the new REDUND table and extracts the GI,
   checks that it exists in the old REDUND table. Prints
   the GI of those that do not exists in the old table to file
*/
Boolean RedundantCheck(RedundDBPtr pRed, RedundDBPtr pRedCheck, FILE *fp)
{
  int rc;
  Int4 iGi;
  Int4 igroup = 0, igroupold = 0;
  ValNodePtr pvnlist = NULL, pvnlisttemp = NULL;
  Boolean bwrite = FALSE;

  /* SK: NULL ptr check */
  if ( pRed == NULL )
  {
    ErrPostEx(SEV_ERROR,0,0, "Error: NULL RedundDBPtr\n");
    return FALSE;
  }

  d4tagSelect(pRed->pd4redund, pRed->pt4rdGroup);
  for (rc = d4top(pRed->pd4redund); rc == r4success; rc = d4skip(pRed->pd4redund, (long) 1))
  {
      iGi = f4long(pRed->pf4rdGi);
      igroup = f4long(pRed->pf4rdGroup);
      if(d4recNo(pRed->pd4redund) == 1)
         igroupold = f4long(pRed->pf4rdGroup);

      if(f4long(pRed->pf4rdOrdinal) == 1)
      {
         if(!CheckGI(iGi, pRedCheck))
            bwrite = TRUE;
      }

      if(igroup == igroupold)
        ValNodeAddInt(&pvnlist, f4long(pRed->pf4rdOrdinal), iGi);
      else
      {
        if(bwrite)
	{
           pvnlisttemp = pvnlist;
           while(pvnlisttemp != NULL)
	   {
              fprintf(fp, "%ld", (long) pvnlisttemp->data.intvalue);
	      if(pvnlisttemp->next != NULL)
	         fprintf(fp, ",");
	      pvnlisttemp = pvnlisttemp->next;
	   }
	   fprintf(fp, "\n");
           bwrite = FALSE;
        }
	pvnlist = ValNodeFree(pvnlist);
        ValNodeAddInt(&pvnlist, f4long(pRed->pf4rdOrdinal), iGi);
      }
      igroupold = igroup;
  }

  return TRUE;
}

/*
$Log: comp_redund_cb.c,v $
Revision 1.4  2004/05/31 20:58:36  hlieu
minor compiler warning fixes, bug 2185

Revision 1.3  2004/01/06 22:32:14  skuznets
Code audit has been done sucessfully.
Possible NULL pointer issues has been eliminated.

Revision 1.2  2003/11/03 15:50:27  haocl
changed order of includes to avoid _LARGE_FILE_SOURCE redefinition warnings

Revision 1.1.1.4  2002/10/17 08:34:32  root
Automated import from Sourceforge.net

Revision 1.5  2002/10/15 20:44:24  kaca
lists to blast and to delete now contain full groups instead of the first representative only

Revision 1.4  2002/09/25 18:31:20  kaca
produces a list to blast and list to delete

Revision 1.3  2002/08/22 18:35:22  haocl
modified REDUND_OpenAllDB to take path_old and path_new

Revision 1.2  2002/08/22 17:34:56  kaca
changed include from redund_db(_p).h comp_redund_db(_p).h

Revision 1.1  2002/08/21 17:45:02  haocl
renaming file redund_cb.c to comp_redund.c

Revision 1.3  2002/08/19 17:38:29  haocl
modified to take path to redund_old/redund_new from command line

Revision 1.2  2002/08/19 15:28:40  haocl
renamed redund_cb.h to redund_db.h

Revision 1.1  2002/08/19 15:12:57  haocl
initial import of redund_cb.c


*/
