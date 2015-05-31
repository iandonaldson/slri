/* $Id: addgoid.c,v 1.11 2004/06/10 17:01:00 zwang Exp $
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
            Katerina Michalickova
            and Christopher Hogue hogue@mshri.on.ca


  REVISION/CONTRIBUTION NOTES:

  June 1, 2001  Original MSH Revision.
                Hogue Lab - University of Toronto Biochemistry
                Department and the Samuel Lunenfeld Research
                Institute, Mount Sinai Hospital
                http://bioinfo.mshri.on.ca  hogue@mshri.on.ca
                ftp://ftp.mshri.on.ca/


  PUBLICATION TO CITE:

  DESCRIPTION
  Gene Ontology flat file parser which supplements LL_GO table contents
  parses gene_association.compugen.Genbank.xls and gene_association.compugen.Swissprot.xls files from
  www.geneontology.org
  note these files do not provide PMID information
  program directly dependent on SeqHound API

****************************************************************************/
#include<ncbi.h>
#include<asn.h>
#include<objslristruc.h>
#include<ll_db.h>
#include<intrez_db.h>
#include<intrez_misc.h>
#include<seqhound.h>

#define LOGING "addgoidlog"
#define NUMARGS 1
Args myargs[NUMARGS] = {
        {"Input file",NULL,NULL,NULL,FALSE,'i',ARG_FILE_IN,0.0,0,NULL}
  };
/* this is used only internally, it does not have a header */
Int4 LL_Append2GO_DB(Int4 NPid, Int4 goID, Int4 PMID, CharPtr eviCode);

Boolean IsInGODB(Int4 gi)
{
   StGOdbPtr pgo = NULL;
   Int2 retVal = 0;

   if(gi <= 0)
     return FALSE;

   pgo = StGOdbNew();
   /* SK: NULL ptr check */
   if ( pgo == NULL )
   {
     ErrPostEx(SEV_ERROR,0,0, "LL_Append2GO_DB: pgo is NULL.");
     StGOdbFree(pgo);
     return FALSE;
   }

   /* HCL: take this out to prevent compiler error, because Locuslink
    *      module has changed, and addgoid is no longer used. */
   /* pgo->npid = gi; */

   retVal = LL_SearchGODB(&pgo);
   if (retVal == -1)
   {
     ErrPostEx(SEV_ERROR,0,0, "LL_SearchGODB failed.");
     StGOdbFree(pgo);
     return FALSE;
   }
   if (retVal == 0)
   {
    StGOdbFree(pgo);
    return FALSE;
   }
   if (retVal == 1)
   {
     StGOdbFree(pgo);
     return TRUE;
   }
   return FALSE;
}

Int4 ProcessGOID(CharPtr pgoid)
{
   CharPtr ptemp = NULL;

   if(pgoid == NULL)
     return 0;

   ptemp = pgoid;
   ptemp = ptemp+3;
   while(ptemp[0] == '0')
     ptemp = ptemp+1;

   return atoi(ptemp);
}

CharPtr ProcessAltName(CharPtr paltname, Int2 offset)
{
   CharPtr ptemp = NULL;
   if(paltname == NULL)
     return 0;

   ptemp = paltname;
   
   /* SK: It's better to check if we have that much data */
   ptemp = ptemp+offset;

   return ptemp;
}

Int2 Main(void)
{
   FILE *pFile;
   Char div[4];
   Char pcBuf[500];
   CharPtr pcTest = NULL;
   CharPtr pstr = NULL;
   Int4 count = 0;
   CharPtr pname0 = NULL, pname = NULL, pgoid = NULL, pcode = NULL, paltname0 = NULL, paltname = NULL;
   Int4 gi = 0, gi0 = 0, goid = 0;
   ValNodePtr pvngi = NULL;

   ErrSetOptFlags(EO_LOG_SEVERITY | EO_LOG_CODES | EO_LOG_FILELINE | EO_LOG_USRFILE);
   ErrSetLogfile(LOGING, ELOG_BANNER);
   ErrSetLogLevel(SEV_INFO);
   ErrSetMessageLevel(SEV_MAX);

   if (! GetArgs("pdbrep",NUMARGS,myargs))
        return 1;

   if ((pFile = FileOpen(myargs[0].strvalue, "r")) == NULL)
        return 1;

   div[0] = ' ';
   div[1] = '\t';
   div[2] = '\n';
   div[3] = '\0';

   if (!InitCodeBase())
   {
      ErrPostEx(SEV_ERROR,0,0, " InitCodeBase failed.");
      return 1;
   }
   if (!OpenPARTI("PARTI", FALSE))
   {
      ErrPostEx(SEV_ERROR,0,0, "OpenPARTI failed.");
      return 1;
   }
   if(!OpenACCDB("ACCDB", FALSE))
   {
      ErrPostEx(SEV_ERROR,0,0, "OpenACCDB failed.");
      return 1;
   }
   if(!OpenNUCPROT("NUCPROT", FALSE))
   {
      ErrPostEx(SEV_ERROR,0,0, "OpenNUCPROT failed.");
      return 1;
   }
   if(!OpenTAXGI("TAXGI", FALSE))
   {
      ErrPostEx(SEV_ERROR,0,0, "OpenTAXGI failed.");
      return 1;
   }
   
   /* set codebase access flag to deny write */
   SetCodeBaseFlagsExclNoidx();
   /* Open LL_GO without index */
   if (!OpenLL_GONoidx("LL_GO"))
   {
      ErrPostEx(SEV_ERROR,0,0, "OpenLL_GO failed.");
      return 1;
   }

   /* After opening the table LL_GO, set autoopen index to true */
   SetCodeBaseAutoIdxTrue();
/*
   if(!LL_OpenAllDBs(FALSE))
   {
     ErrPostEx(SEV_ERROR,0,0, "LL_OpenAllDBs failed.");
     return 1;
   }
*/
#ifdef ODBC
   LL_InitCodeBase(TRUE);
#endif
   do {
         pcTest = fgets(pcBuf, (size_t)500,  pFile);
         if ((pcTest != NULL) && (pcBuf[0] != '!'))
         {
            pstr = strtok(pcTest, div);
            count++;
            while(pstr)
            {
               pstr = strtok(NULL, div);
               count++;
               if(count == 3) pname0 = StrSave(pstr);
               if(count == 4) pgoid = StrSave(pstr);
               if(count == 6) pcode = StrSave(pstr);
	       if(count == 9) paltname0 = StrSave(pstr);
            }
	
            /* if name starts with GI, chop GI off */
            if (pname0[0]=='G' && pname0[1]=='I')
            {
               pname = ProcessAltName(pname0, 2);
            }

            /* RY: else, initialize pname = pname0 to gurantee pname is not null*/
	    else
	       pname = pname0;
	       
            if(Misc_IsNumeric(pname))
	    {
	       if(!StrToInt4 (pname, &gi0))
                   ErrPostEx(SEV_ERROR,0,0, "Conversion to string failed.", pname);

	    }
            else
	    {
                gi0 = SHoundFindName(pname);
                if(gi0 <= 0)
                {
                   gi0 = SHoundFindAcc(pname);
                }
	    }
            if(gi0 <= 0)  /* try out if the name of the protein was hidden after the taxon; as is in Swissprot file from July 2002 */
	    {
	       paltname = ProcessAltName(paltname0,6);
               if(!Misc_IsNumeric(paltname))
	       {
                 gi0 = SHoundFindName(paltname);
                 if(gi0 <= 0)
                   gi0 = SHoundFindAcc(paltname);
               }
	    }

            if(gi0 <= 0)
                 ErrPostEx(SEV_ERROR,0,0, "No GI from %s.", IsNullStr(pname));
            else
            {
                /*ErrPostEx(SEV_INFO,0,0, "GI from %s %ld.", IsNullStr(pname), (long) gi0);*/
                if(!SHoundIsProtein(gi0))
                {
                  pvngi = SHoundProteinFromDNA(gi0);
                  if(ValNodeLen(pvngi) > 1)
                     ErrPostEx(SEV_ERROR,0,0, "Too many protein GIs from %ld %s.", (long) gi0, IsNullStr(pname));
                  else
                  {
                      if(pvngi != NULL)
                      {
                         if((gi = pvngi->data.intvalue) <= 0)
                           ErrPostEx(SEV_ERROR,0,0, "Invalid protein GI from %ld %s.", (long) gi0, IsNullStr(pname));
                      }
                  }
                }
                else
                  gi = gi0;

                if(gi > 0)
                {
                   goid = ProcessGOID(pgoid);
                   if(goid <=0)
                   {
                      ErrPostEx(SEV_ERROR,0,0, "Invalid goid %ld from %ld %s.", (long) goid, (long) gi, IsNullStr(pname));
                      return 1;
                   }
                   /*printf("Appending from %s: %ld\t%ld\t%s\n", IsNullStr(pname), gi, goid, pcode);*/
                   LL_Append2GO_DB(gi, goid, 0, pcode);
                }
            }

            pname0 = MemFree(pname0);
            pgoid = MemFree(pgoid);
            pcode = MemFree(pcode);
	    paltname0 = MemFree(paltname0);
            count = 0;
         }
      } while (pcTest);
   /* Reindex the table LL_GO */
   if(!ReindexLL_GO())
   {
      ErrPostEx(SEV_ERROR,0,0, "Reindex table LL_GO failed.");
      return 1;
   }
   ClosePARTI();
   CloseNUCPROT();
   CloseACCDB();
   if(!CloseLL_GO())
   {
      ErrPostEx(SEV_ERROR,0,0, "CloseLL_GO failed.");
      return 1;
   }
   /* CloseAllLLDBs(); */
   FileClose(pFile);
#ifdef ODBC
   LL_InitUndoCodeBase();
   CloseCodeBase();
#endif
   ErrPostEx(SEV_INFO,0,0, "Main: Done!");
   return 0;
}



/*
$Log: addgoid.c,v $
Revision 1.11  2004/06/10 17:01:00  zwang
Add flag to determine if a database connection should be established. See bug #2324.

Revision 1.10  2004/05/31 21:46:27  hlieu
got rid of // comments, for bug 2185, error on mork

Revision 1.9  2004/05/31 20:28:10  hlieu
fix minor warnings, bug 2185

Revision 1.8  2004/03/19 19:34:14  vgu
Further change to use SHoundIsProtein for searching molecule type from the table TAXGI see bug 1854 for details on performance tuning of addgoid

Revision 1.7  2004/03/17 20:10:29  eburgess
Victor: Improve performance by opening the LL_GODB table without an index
and then adding the index later.

Revision 1.6  2004/02/18 21:08:14  ryao
to avoid name is a NULL pointer if it does not start with 'GI'

Revision 1.5  2004/02/05 15:47:26  zwang
Checks if the third column read in starts with 'GI', if it does, chop off 'GI'. This is to adapt the new format of input file. See bug #1533.

Revision 1.4  2004/01/22 20:51:32  vgu
code change re bug 420

Revision 1.3  2004/01/06 22:32:13  skuznets
Code audit has been done sucessfully.
Possible NULL pointer issues has been eliminated.

Revision 1.2  2003/10/01 19:48:28  haocl
changed objll to objslristruc

Revision 1.1.1.8  2002/11/10 09:39:22  root
Automated import from Sourceforge.net

Revision 1.9  2002/11/09 13:40:13  kaca
removed excessive log printing

Revision 1.8  2002/08/07 21:53:11  kaca
added an include and function declaration

Revision 1.7  2002/08/07 17:12:25  kaca
removed debug comment

Revision 1.6  2002/08/06 19:07:15  kaca
added code to deal with association file format changes

Revision 1.5  2002/08/02 21:20:43  kaca
fixes reflecting the source file changes in format

Revision 1.4  2002/06/07 16:43:49  kaca
fixed return values

Revision 1.3  2002/01/22 23:39:27  kaca
removed debug printf

Revision 1.2  2001/11/13 22:03:06  kaca
changes of logic; append all instances of GI and GOID

Revision 1.1  2001/11/09 20:40:54  kaca
parser for flat files from www.geneontology.org

*/


