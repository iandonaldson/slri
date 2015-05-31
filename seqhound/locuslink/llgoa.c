/* $Id: llgoa.c,v 1.7 2004/12/14 19:45:09 zwang Exp $
***************************************************************************
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

  June 1, 2001  Original MSH Revision.
                Hogue Lab - University of Toronto Biochemistry
                Department and the Samuel Lunenfeld Research
                Institute, Mount Sinai Hospital
                http://bioinfo.mshri.on.ca  hogue@mshri.on.ca
                ftp://ftp.mshri.on.ca/


  PUBLICATION TO CITE:

  DESCRIPTION
 ' Gene Ontology flat file parser
  Part of SeqHound

****************************************************************************/
#include <ll_db.h>
#include <ll_parser.h>
#include <slri_misc.h>

#define LOGING "llgoalog"
#define NUMARGS 2

Args myargs[NUMARGS] = {
        {"Input file","gigo_pdb.txt",NULL,NULL,TRUE,'i',ARG_FILE_IN,0.0,0,NULL},
        {"Is this an update","F",NULL,NULL,TRUE,'u',ARG_BOOLEAN,0.0,0,NULL}
  };


Boolean appendData ( Int4 gi, Int4 goid, CharPtr evidence,
		     Int4 pmid, Int4 record, Int4 locus )
{
   Int4 i;
   StLLdbPtr pLLHead = NULL;
   StGOdbPtr pGO = NULL,
         pGOHead = NULL;
   Boolean found = FALSE;

   if ( gi <= 0 || goid <= 0 )
   {
      ErrPostEx ( SEV_ERROR, 0, 0, "[appendData]: Invalid %s on Record %d with %s %d. Record skipped.",
                  gi <= 0 ? "gi" : "goid", record, gi <= 0 ? "goid" : "gi", gi <= 0 ? goid : gi );
      return FALSE;
   }

   pLLHead = StLLdbNew();
   pLLHead->npid = gi;


   /* get the llid from the gi */
   if ( 1 == (i = LL_SearchLLDB(&pLLHead)) )
   {
#ifdef ODBC
      LL_Append2GO_DB ( pLLHead->llid, goid, pmid <= 0 ? 1000 : pmid, evidence );
#else
      /* llid exists, so use that to try to find if the goid, evidence code exists */
      /* fprintf ( stderr, "Found gi %d locus %d\n", gi, pLLHead->llid ); */
      pGOHead = StGOdbNew();
      pGOHead->llid = pLLHead->llid;

      /* using llid, see if it has goid with same evidence code assigned */
      if ( 1 == (i = LL_SearchGODB(&pGOHead)) )
      {
         /* fprintf ( stderr, "Search %d %d %d %s\n", gi, goid, pmid, evidence ); */
	 for ( pGO = pGOHead; NULL != pGO; pGO = pGO->next )
	 {
            /* fprintf ( stderr, "Found %d %d %d %s\n", gi, pGO->goid, pGO->pmid, pGO->eviCode ); */
            if ( pGO->goid == goid && 0 == StringCmp(pGO->eviCode, evidence) )
	       { found = TRUE; }
	 }
	 if ( FALSE == found )
	 {
           /* insert using existing llid: goid, pmid, evidence code
            fprintf ( stderr, "Found %d %d %d %s\n", gi, goid, pmid, evidence ); */
	   LL_Append2GO_DB ( pGOHead->llid, goid, pmid <= 0 ? 1000 : pmid, evidence );
	 }
      }
#endif
   } else {
      /* llid does not exist, so add to LL_LLINK with negative llid and
       * add to LL_GO */
      /* fprintf ( stderr, "Insert into LL_LLINK -llid and insert into LL_GO\n" ); */
      LL_Append2LLID_DB ( locus, gi, "NULL" );
      LL_Append2GO_DB ( locus, goid, pmid <= 0 ? 1000 : pmid, evidence );
   }
   return TRUE;
}


Boolean importData ( CharPtr file )
{
   FILE     *fp            = NULL;   /* file stream */
   char     line[500];              /* line buffer */
   char     *tokens[1]     = {0};   /* array of pointers, my tokens */
   int      itokens[3]     = {0};
   int      i              = 0,
            j              = 0;

   if ( NULL == (fp = FileOpen ( file, "r" )) )
   {
      ErrPostEx ( SEV_ERROR, 0, 0,
		  "Cannot open file %s", file );
      return FALSE;
   }

   for ( i = 1, j = -1; NULL != fgets (line, 500, fp ); i++, j-- )
   {
      itokens[0] = atoi(strtok ( line, "\t" ));
      itokens[1] = atoi(strtok ( NULL, "\t" ));
      tokens[0] = strtok ( NULL, "\t" );
      itokens[2] = atoi(strtok ( NULL, "\n" ));
      appendData ( itokens[0], itokens[1], tokens[0], itokens[2], i, j );
      //fprintf ( stdout, "%d\t%d\t%s\t%d\n", itokens[0], itokens[1], tokens[0], itokens[2] );
   }
   FileClose ( fp );
   return TRUE;
}


Int2 Main(void)
{
   Boolean isUpdate = FALSE;

   ErrSetOptFlags(EO_LOG_SEVERITY | EO_LOG_CODES | EO_LOG_FILELINE | EO_LOG_USRFILE);
   ErrSetLogfile(LOGING, ELOG_BANNER);
   ErrSetLogLevel(SEV_INFO);
   ErrSetMessageLevel(SEV_MAX);

   if (! GetArgs("llgoa",NUMARGS,myargs))
        return 1;

   if(FileLength(myargs[0].strvalue) == 0)
   {
     ErrPostEx(SEV_ERROR,0,0,"Main: Cannot find %s file.", IsNullStr(myargs[0].strvalue));
     fprintf ( stderr, "Error while running program\nSee log file %s.\n", LOGING);
     return 1;
   }

   if (myargs[1].intvalue)
     isUpdate = TRUE;

   LL_InitCodeBase(TRUE);
   LL_OpenAllDBs (FALSE);

   if (!LL_StartTransaction()) {
     ErrPostEx(SEV_ERROR, 0, 0, "Cannot start transaction!");
     LL_InitUndoCodeBase();
     return 1;
   }

   if (isUpdate) {
     /* delete all existing records originated from DBXRefGOA. 
        they are records in LL_GO and LL_LLINK with ll_id<0 */
     if (!DeleteRecordsInLL_GOA()) {
       ErrPostEx(SEV_ERROR,0,0, "Cannot delete GOA records from LL tables!");
       if (!LL_RollBackTransaction()) {
         ErrPostEx(SEV_ERROR,0,0, "Cannot roll back!");
         LL_InitUndoCodeBase();
         return 1;
       }
       LL_InitUndoCodeBase();
       return 1;
     }
   }

   if( FALSE == importData(myargs[0].strvalue) )
   {
     ErrPostEx(SEV_ERROR,0,0,"Main: LL_ParseFile failed.");
     return 1;
   }

   if (!LL_CommitTransaction()) {
     ErrPostEx(SEV_ERROR, 0, 0, "Cannot commit transaction!");
     if (!LL_RollBackTransaction()) {
       ErrPostEx(SEV_ERROR,0,0, "Cannot roll back!");
       LL_InitUndoCodeBase();
       return 1;
     }
     LL_InitUndoCodeBase();
     return 1;
   }

   LL_InitUndoCodeBase();
   fprintf( stderr, "\n\nDone! Check %s for errors.\n\n\n", LOGING);
   return 0;
}

/*
$Log: llgoa.c,v $
Revision 1.7  2004/12/14 19:45:09  zwang
Added ability of building a transactional database.
Added input parameter to specify if it's an update.
See bug 3441.

Revision 1.6  2004/06/10 17:01:00  zwang
Add flag to determine if a database connection should be established. See bug #2324.

Revision 1.5  2004/05/31 20:30:08  hlieu
minor compiler warning fixes, bug 2185

Revision 1.4  2004/05/31 20:28:10  hlieu
fix minor warnings, bug 2185

Revision 1.3  2004/05/26 17:45:36  zwang
In ODBC, No need to check if go_id exists ll_go. 'insert ignore' in LL_Append2GO_DB will skip duplicates.

Revision 1.2  2004/05/17 18:09:12  hlieu
remove stale logs

Revision 1.1  2004/05/17 18:05:16  hlieu
import of locus goa importer

*/

