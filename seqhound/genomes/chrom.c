/* $Id: chrom.c,v 1.3 2005/02/01 21:40:43 zwang Exp $
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
  May 1999
  makes CodeBase database (CHROM) with data about complete genomes (needs an input flat file with
  taxonomy info of a format - taxonomy id, kloodge id, type of DNA (byteflags defined in intrez.h),
  organism name - all separated by white space)


******************************************************************************************/

#include <intrez_db.h>
#include <intrez_misc.h>
#include <intrez.h>


#define LOGING "chromlog"

#define NUMARGS 2
Args myargs[NUMARGS] = {
          {"Input file", NULL, NULL, NULL, FALSE, 'i', ARG_FILE_IN, 0.0,0,NULL},
          {"Is this an update", NULL, NULL, NULL, TRUE, 'i', ARG_BOOLEAN, 0.0,0,NULL}
          };

StChromPtr pch;

Int2 Main()
{
   Char pcBuf[100];
   CharPtr pcTest = NULL;
   CharPtr ptok = NULL;
   Char test[3];
   FILE *pFile = NULL;
   Int4 someid = 0;
   Boolean isUpdate = FALSE;


   /* check command line arguments */
   if ( ! GetArgs("chrom",NUMARGS, myargs))
          return 1;


   ErrSetOptFlags(EO_LOG_SEVERITY | EO_LOG_CODES | EO_LOG_FILELINE | EO_LOG_USRFILE);
   ErrSetLogfile(LOGING, ELOG_BANNER);
   ErrSetLogLevel(SEV_INFO);
   ErrSetMessageLevel(SEV_MAX);	


   if ((pFile = FileOpen(myargs[0].strvalue, "r")) == NULL)
    {
        ErrPostEx(SEV_ERROR,0,0, "Cannot open %s", IsNullStr(myargs[0].strvalue));
        return 1;
    }

   /* Initialize CodeBase and open databases */
   InitCodeBase();

   /* starting transaction */
   if (!StartTransaction()) {
     ErrPostEx(SEV_ERROR, 0, 0, "Cannot start transaction!");
     CloseCodeBase();
     return 1;
   }

   /* deleting all records in table CHROM for update */
   if (isUpdate) {
     if (!DeleteAllRecordsInCHROM()) {
        ErrPostEx(SEV_ERROR,0,0, "Cannot delete all records from CHROM table!");
        if (!RollBackTransaction()) {
           ErrPostEx(SEV_ERROR,0,0, "Cannot roll back!");
           CloseCodeBase();
           return 1;
        }
        CloseCodeBase();
        return 1;
     }
   }


   if(!OpenCHROM("CHROM", TRUE))
    {
       ErrPostEx(SEV_ERROR,0,0, "Cannot open CHROM");
       return 1; 
    }

   pch = StChromNew();
   /* SK: NULL ptr check */
   if ( pch == NULL )
   {
       ErrPostEx(SEV_ERROR,0,0, "CHROM Pointer is NULL");
       return 1; 
   }

   test[0] = '\t';
   test[1] = '\n';
   test[2] = '\0';

   do {
       pcTest = fgets(pcBuf, (size_t)100,  pFile);
       if (pcTest)
       {
            ResetStCHROM(&pch);
            ptok = strtok(pcTest, test);
            if (StrToInt4 (ptok, &someid))
	    {
	         pch->taxid = someid;
                 ErrPostEx(SEV_INFO,0,0, "Assigned TaxId %ld.",  (long) someid);
                 someid = 0;
            }
            ptok = strtok(NULL, test);
            if (StrToInt4 (ptok, &someid))
	    {
	         pch->kloodge = someid;
                 ErrPostEx(SEV_INFO,0,0, "Assigned Kloodge %ld.",  (long) someid);
                 someid = 0;
            }
            ptok = strtok(NULL, test);
            if (StrToInt4 (ptok, &someid))
	    {
	         pch->chromfl = someid;
                 ErrPostEx(SEV_INFO,0,0, "Assigned Chromfl %ld.", (long) someid);
                 someid = 0;
            }
            ptok = strtok(NULL, test);
	    pch->access = StrSave(ptok);
            ErrPostEx(SEV_INFO,0,0, "Assigned Access %s.", IsNullStr(ptok));
            ptok = strtok(NULL, test);
	    pch->name = StrSave(ptok);
            ErrPostEx(SEV_INFO,0,0, "Assigned Name %s.\n", IsNullStr(ptok));
            AppendRecordCHROM(pch);
	    MemFree(pch->access);
            MemFree(pch->name);
       }
      } while (pcTest);

   /* Committing all changes or rolling back */
   if (!CommitTransaction()) {
      ErrPostEx(SEV_ERROR, 0, 0, "Cannot commit transaction!");
      if (!RollBackTransaction()) {
         ErrPostEx(SEV_ERROR,0,0, "Cannot roll back!");
         CloseCodeBase();
         return 1;
      }
      CloseCodeBase();
      return 1;
   }


   /* close the databases*/
   CloseCHROM();
   CloseCodeBase();
   FileClose(pFile);


   ErrPostEx(SEV_INFO,0,0, "Done.");
   return 0;
}

/*
$Log: chrom.c,v $
Revision 1.3  2005/02/01 21:40:43  zwang
Added ability of building a transactional database.
Added input parameter to specify if it's an update.
See bug 3441.

Revision 1.2  2004/01/06 22:32:12  skuznets
Code audit has been done sucessfully.
Possible NULL pointer issues has been eliminated.

Revision 1.1.1.3  2002/10/31 09:34:18  root
Automated import from Sourceforge.net

Revision 1.3  2002/10/29 18:55:32  kaca
modified kloodge assignment

Revision 1.2  2002/10/17 20:01:24  kaca
parser assignes two more fields to CHROM table - accession and name of chromosome

Revision 1.1.1.1  2001/05/25 21:36:19  kaca
initial import

Revision 1.9  2001/03/29 16:32:48  kaca
added intrez_misc include

Revision 1.8  2001/03/26 19:47:53  kaca
changes in include files

Revision 1.7  2001/02/23 20:40:38  kaca
license change

Revision 1.6  2001/02/07 16:48:30  kaca
cosmetic changes

Revision 1.5  2001/02/05 18:14:20  kaca
abstracted database calls

Revision 1.4  2000/10/22 19:40:36  kaca
minor change

Revision 1.3  2000/10/13 21:54:22  kaca
changed includes

Revision 1.2  2000/09/20 18:49:59  kaca
split SeqHound headers into private and public one and changed includes

Revision 1.1.1.1  2000/09/18 17:41:56  kaca
Initial import

Revision 1.1.1.1  2000/07/06 20:02:32  kaca
Initial import

*/






