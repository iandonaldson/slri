/* $Id: cbmmdb.c,v 1.9 2004/12/14 16:46:29 zwang Exp $
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
           Katerina Michalickova, Howard Feldman and Christopher W.V. Hogue hogue@mshri.on.ca


  REVISION/CONTRIBUTION NOTES:

  June 1, 2000  Original MSH Revision.
                Hogue Lab - University of Toronto Biochemistry
                Department and the Samuel Lunenfeld Research
                Institute, Mount Sinai Hospital
                http://bioinfo.mshri.on.ca  hogue@mshri.on.ca
                ftp://ftp.mshri.on.ca/


  PUBLICATION TO CITE:


  DESCRIPTION
  August 1999
  parser which fills CodeBase databases (MMDB and MMGI) with MMDB data


******************************************************************************************/
#include <intrez_db.h>
#include <slri_mmdblocl.h>
#include <seqhound.h>

#define LOGING "cbmmdblog"
#define ARR_LEN 1024
#define NUMARGS 1
Args myargs[NUMARGS] = {
        {"Is this an update", "F", NULL, NULL, TRUE, 'u', ARG_BOOLEAN, 0.0, 0, NULL}
        };

StMmdbPtr pmd;
StMmgiPtr pmg;


Int2 Main()
{
   BiostrucPtr bspBiostruc=NULL;
   PMSD pmsdRoot = NULL;
   PDNMM pdnmmhead = NULL, pdnmmthis = NULL;
   PDNMS pdnmsModelstruc=NULL;
   Int4 id;
   time_t tim1,tim2;
   /* the largest mmdbid */
   Int4 max_mmdbid = 0;
   Boolean isUpdate = FALSE;
   Int4 count = 0;

   ErrSetOptFlags(EO_SHOW_SEVERITY|EO_SHOW_CODES|EO_LOG_FILELINE|EO_SHOW_USERSTR|EO_SHOW_ERRTEXT|EO_BEEP|EO_WAIT_KEY|EO_LOGTO_USRFILE);
   ErrSetLogfile(LOGING, ELOG_BANNER);
   ErrSetMessageLevel(SEV_MAX);
   ErrSetLogLevel(SEV_INFO);

   pmd = StMmdbNew();
   pmg = StMmgiNew();

   if ( ! GetArgs("cbmmdb",NUMARGS, myargs))
       return 1;

   InitCodeBase();

   if (myargs[0].intvalue)
       isUpdate = TRUE;

   if (!StartTransaction()) {
       ErrPostEx(SEV_ERROR, 0, 0, "Cannot start transaction!");
       return 1;
   }

   if(!OpenMMDB("MMDB", TRUE))
       return 1;
   if(!OpenMMGI("MMGI", TRUE))
       return 1;

   if (!OpenMMDBAPI(0,NULL))
     {
	ErrPostEx(SEV_FATAL,2,1,"Unable to open MMDBAPI");
	return 2;
     }
   if (!MMDBInit())
     {
        ErrPostEx(SEV_FATAL,3,1,"Unable to open local MMDB service");
        return 3;
     }
   tim1=GetSecs();

   /* load an ASN.1 Biostruc */
   ResetMMDBIterate();
   do {
         ResetStMMDB(&pmd);
         id=0;
         id=GetMMDBIdCurrent();	

         bspBiostruc=MMDBBiostrucGet((DocUid) id, ALLMDL,100);
         if (bspBiostruc==NULL)
          {
             ErrPostEx(SEV_ERROR,4,1,"Error opening MMDB id %ld",id);
             ClearStructures();
             continue;
           }

         count++;

         /* copy the ASN.1 for Biostruc in the memory because the MakeAModelstruc call will modify it */
	 /* now load in fresh from disk, full Biostruc this time, using -1 for model level */
         pmd->asn1 = MMDBBiostrucGet((DocUid) id, -1,100);
	 /*(BiostrucPtr) AsnIoCopy(bspBiostruc, (AsnReadFunc) BiostrucAsnRead, (AsnWriteFunc) BiostrucAsnWrite);*/
         pdnmsModelstruc=MakeAModelstruc(bspBiostruc);
         if (pdnmsModelstruc==NULL)
          {
            ErrPostEx(SEV_ERROR,5,1,"Error parsing MMDB id %ld ",id);
            ClearStructures();
            continue;
          }
         pmsdRoot=(PMSD)(pdnmsModelstruc->data.ptrvalue);
         if (pmsdRoot==NULL)
          {
             ErrPostEx(SEV_ERROR,6,1,"No structures in MMDB id %ld",id);
             ClearStructures();
             continue;
          }

         if (pmsdRoot->bWhat <= 0)
          {
	     ErrPostEx(SEV_INFO,6,1,"Invalid bWhat in %ld", id);
	     ClearStructures();
	     continue;
           }
          else
           {
              pmd->bwhat = (Int4) pmsdRoot->bWhat;
           }

          if (pmsdRoot->iMMDBid <= 0)
           {
             ErrPostEx(SEV_ERROR,6,1,"Invalid MMDB Id in %ld", id);
	     ClearStructures();
	     continue;
           }
          else
           {
              pmd->mmdbid = pmsdRoot->iMMDBid;
           }
          if (pmsdRoot->pcPDBName == NULL)
           {
             ErrPostEx(SEV_ERROR,6,1,"Invalid PDB Id in %ld", id);
	     ClearStructures();
	     continue;
           }
          else
           {
              pmd->pdbid = pmsdRoot->pcPDBName;
           }

          if (pmsdRoot->iModels < 1)
           {
             ErrPostEx(SEV_INFO,6,1,"No models in %ld", id);
	     ClearStructures();
	     continue;
           }
          else
           {
             pmd->models = pmsdRoot->iModels;
           }
         		
          if (pmsdRoot->iMolCount < 1)
           {
             ErrPostEx(SEV_ERROR,6,1,"No molecules in %ld", id);
	     ClearStructures();
	     continue;
           }
          else
           {
             pmd->molecules = pmsdRoot->iMolCount;
           }

          pdnmmhead = pmsdRoot->pdnmmHead;
          if (pdnmmhead == NULL)
           {
             ErrPostEx(SEV_ERROR,6,1,"No molecules in MMDB id %ld",id);
             ClearStructures();
             continue;
           }
          pdnmmthis = pdnmmhead;
	  while(pdnmmthis != NULL)
           {
             if (((PMMD) pdnmmthis->data.ptrvalue)->iGi > 0)
              {
                 ResetStMMGI(&pmg);
                 pmg->mmdbid = pmsdRoot->iMMDBid;
		 if(pmg->mmdbid > max_mmdbid) max_mmdbid = pmg->mmdbid;
                 pmg->gi = ((PMMD)pdnmmthis->data.ptrvalue)->iGi;
                 AppendRecordMMGI(pmg);

              }
             pdnmmthis = pdnmmthis->next;
           }

          AppendRecordMMDB(pmd);
          BiostrucFree(pmd->asn1);

          ClearStructures();

          if (!isUpdate && count >= 100) {
                count = 0;
                /* commit */
                if (!CommitTransaction()) {
                   ErrPostEx(SEV_ERROR, 0, 0, "Cannot commit transaction!");
                   if (!RollBackTransaction()) {
                      ErrPostEx(SEV_ERROR,0,0, "Cannot roll back!");
                      MemFree(pmd);
                      MemFree(pmg);
                      CloseMMDBAPI();
                      MMDBFini();
                      CloseCodeBase();
                      return 1;
                   }
                   MemFree(pmd);
                   MemFree(pmg);
                   CloseMMDBAPI();
                   MMDBFini();
                   CloseCodeBase();
                   return 1;
                } 
          }


   } while (IterateMMDBId());

   /* commit for the last time:
    * all changes if this is an update,
    * otherwize the last bit of insertion */
   if (!CommitTransaction()) {
      ErrPostEx(SEV_ERROR, 0, 0, "Cannot commit transaction!");
      if (!RollBackTransaction()) {
         ErrPostEx(SEV_ERROR, 0, 0, "Cannot roll back!");
         MemFree(pmd);
         MemFree(pmg);
         CloseMMDBAPI();
         MMDBFini();
         CloseCodeBase();
         return 1;
      }
      MemFree(pmd);
      MemFree(pmg);
      CloseMMDBAPI();
      MMDBFini();
      CloseCodeBase();
      return 1;
   }

   /* Free the Modelstruc (and its enclosed Biostruc) */	
   /* FreeAModelstruc(PDNMS pdnmsThis); not necessary */
   /* This can be done individually - but all Modelstrucs */
   /* remaining are freed in CloseMMDB-API() */

   MemFree(pmd);
   MemFree(pmg);

   tim2=GetSecs();

   /* Shut Down MMDB-API */	
   CloseMMDBAPI();	
   MMDBFini();
   ErrPostEx(SEV_WARNING,11,1,"Total elapsed time: %ld seconds",tim2-tim1);

   CloseMMGI();	
   CloseMMDB();
   CloseCodeBase();
   ErrPostEx(SEV_INFO,0,0, "Main: Done!");
   
#ifndef ODBC
   /* new block for creating the "datafile" for largest MMDB ID.
    * The datafile is a text file with one value, the largest mmdb id.
    */
   {{
      Char pathmm[ARR_LEN] = {'\0'};  /* ARR_LEN defined at top */
      CharPtr path = NULL;
      FILE *fp = NULL;

      GetAppParam("intrez", "datab", "pathmm", "./", pathmm, ARR_LEN - 1);

      /*  MMDBID_LARGEST defined in intrez_db.h  */
      path = StringCat(pathmm, MMDBID_LARGEST);
      fp = fopen(path, "w");
      if(!fp) ErrPostEx(SEV_WARNING, 11, 1, "Could not open MMDBID_LARGEST");
      fprintf(fp, "%lu\n", (long) max_mmdbid);
      fflush(fp);
      fclose(fp);
    }} /* end block */
#endif
   return 0;

}

/*
$Log: cbmmdb.c,v $
Revision 1.9  2004/12/14 16:46:29  zwang
Added ability of building a transactional database.
Added input parameter to specify if it's an update.
Removed unused parameters -n and -m.
See bug 3441.

Revision 1.8  2004/06/07 21:48:26  hlieu
cast to long, bug 2185, getting rid of warnings in cross comp

Revision 1.7  2004/03/24 19:59:57  zwang
Removed unnecessary printout.

Revision 1.6  2004/03/15 16:59:42  zwang
Don't create MMDB.txt for ODBC.

Revision 1.5  2004/01/15 15:04:38  hlieu
added error statement on failure of largest MMDBID

Revision 1.4  2004/01/07 19:19:53  hlieu
added file support and code for saving largest mmdb id

Revision 1.3  2004/01/06 22:32:13  skuznets
Code audit has been done sucessfully.
Possible NULL pointer issues has been eliminated.

Revision 1.2  2003/10/22 17:14:17  feldman
Store full biostruc in MMDB table now

Revision 1.1.1.2  2002/03/25 09:27:23  root
Automated import from Sourceforge.net

Revision 1.2  2002/03/25 06:55:15  micheld
changed include order

Revision 1.1.1.1  2001/05/25 21:36:20  kaca
initial import

Revision 1.10  2001/03/26 19:48:05  kaca
changes in include files

Revision 1.9  2001/02/23 20:41:16  kaca
license change

Revision 1.8  2001/01/27 22:15:06  kaca
abstracted database calls

Revision 1.7  2001/01/26 19:19:48  kaca
removed unused variables

Revision 1.6  2001/01/25 15:12:15  kaca
bzip error handling change

Revision 1.5  2001/01/24 16:51:37  kaca
bzip new library

Revision 1.3  2000/10/13 21:52:18  kaca
changed includes

Revision 1.2  2000/09/20 18:49:59  kaca
split SeqHound headers into private and public one and changed includes

Revision 1.1.1.1  2000/09/18 17:41:56  kaca
Initial import

Revision 1.1.1.1  2000/07/06 20:02:32  kaca
Initial import

*/
