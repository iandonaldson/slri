/* $Id: y_parser.c,v 1.2 2004/01/06 22:32:18 skuznets Exp $
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

  March 1, 2001  Original MSH Revision.
                Hogue Lab - University of Toronto Biochemistry 
                Department and the Samuel Lunenfeld Research 
                Institute, Mount Sinai Hospital
                http://bioinfo.mshri.on.ca  hogue@mshri.on.ca
                ftp://ftp.mshri.on.ca/


  PUBLICATION TO CITE:

  DESCRIPTION:
  Parser for orf_geneontology.all.tab which can be downloaded at 
  ftp://genome-ftp.stanford.edu/pub/yeast/data_download/literature_curation

****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#include <y_parser.h>
#include <y_db.h>
#include <slri_misc.h>

/* Actually parsing of orf_geneontology occurs here. The ORF, gene name,
 * PMID, evidence code & goid are extracted and appended to LL_GO database.
 * prior to appending the data to LL_GO, the db layer (y_db.c) will
 * look up the ORF/gene name in the SGD database to get a gi,
 * then the gi, pmid, evidence code and goid get inserted into LL_GO.
 * If no gi found, not inserted.  */
static Int2 Y_ReadFile(FILE *fp)
{
  ssize_t read = 0;
  size_t len = 0;
  CharPtr pline = NULL;
  YST_ST yst = {NULL, NULL, 0, NULL, 0};
  CharPtr pORF = malloc(sizeof(char) * 7);
  CharPtr pGene = malloc(sizeof(char) * 5);
  CharPtr pEVI = malloc(sizeof(char) * 4);
  CharPtr p = NULL;  
 
  if(fp == NULL)
  {
    ErrPostEx(SEV_ERROR, 0, 0, "[YST]: No file handle");
    return -1;
  }
  if(pORF == NULL || pGene == NULL || pEVI == NULL)
  {
    ErrPostEx(SEV_ERROR, 0, 0, "[YST]: Could not allocate char *");
    return -1;
  }
  
  /* read in line by line, store content into pline, len is size of pline,
   * read returns -1 on EOF */ 
  while( (read = getline(&pline, &len, fp)) != -1 ){
    /* all entries have an ORF, so we save it */
    yst.pORF = StringCpy(pORF, strtok(pline, "\t"));

    /* this may be the gene name or the SGDID */
    yst.pGene = StringCpy(pGene, strtok(NULL, "\t"));
    
    /* do a comparison to check */
    if(StringNCmp(pGene, "S000", 4) != 0) 
    {  /* not an SGDID, so that means we have the gene name, so
	  have to advance to the next token which is the SGDID */
       p = strtok(NULL, "\t");
    } else {  /* got an SGDID, which means there is no gene name
	       * so I can store a junk value in it */
       yst.pGene = StringCpy(pGene, "GENNAME");
    }

    /* grab the ontology classification: can be C, P or F.
     * A few rare cases where no classification is provided
     * in which case it is the go term p points to */
    p = strtok(NULL, "\t");
    if( StringCmp(p, "C") == 0 || StringCmp(p, "F") == 0 || StringCmp(p, "P") == 0 )
    {  /* got the ontology classification, which can be ignored.
	* then next grab the GO term, but we don't save GO term, we save GOID */
       strtok(NULL, "\t");
    } /* else, p is the GO term, but not saving GO terms, so can ignored */

    /* next token; according to specs, this should be the goid, but
     * the specs are not always followed, so have to check */
    p = strtok(NULL, "\t");
    if(Misc_IsNumeric(p))  /* check that it is numeric */
    {
        /* numeric means conforms to the spec, so it is the goid */
	yst.iGOID = atoi(p);
    }else{
	/* does not conform to spec, so the next token is goid 
	 * so we get that and store it */
	yst.iGOID = atoi(strtok(NULL, "\t"));
    }

    /* the Is_not term, can ignore this */
    strtok(NULL, "\t");
    /* next token is the evidence code, so save this */
    yst.pEviCode = StringCpy(pEVI, strtok(NULL, "\t"));
    /* the reference number; ignore this */
    strtok(NULL, "\t");
    /* the pubmed id, save it as an int */
    yst.iPMID = atoi(strtok(NULL, "\t"));
    /* use for debugging */
    /* printf("%s %s %d %s %d\n", IsNullStr(yst.pORF), IsNullStr(yst.pGene), yst.iGOID, IsNullStr(yst.pEviCode), yst.iPMID); */
    Y_Append_LL(&yst);
  }
  free(pORF); free(pGene); free(pEVI); free(pline);
  return 1;
}

/* the 'file' you want to parse */
extern Int2 Y_ParseFile(CharPtr file)
{
  FILE *fp;

  fp = FileOpen(file, "r");
  if(fp == NULL){
     ErrPostEx(SEV_FATAL, 0, 0, "[YST]: File not found");
     return -1;
  }

  /* initialize the SGD and LL_GO database. Need SGD to convert 
   * the ORF (provided by file we are parsing) into 
   * gi, which is what is needed in the LL_GO database */
  if(!Y_InitSGD_n_LL())
  {
    ErrPostEx(SEV_ERROR, 0, 0, "[YST]: Could not open SGD");
    return -1;
  }
  Y_ReadFile(fp);
  FileClose(fp);
  Y_CloseSGD_n_LL();
  return 0;
}

#ifdef __cplusplus
}
#endif


	
/*
$Log: y_parser.c,v $
Revision 1.2  2004/01/06 22:32:18  skuznets
Code audit has been done sucessfully.
Possible NULL pointer issues has been eliminated.

Revision 1.1.1.2  2003/06/28 08:45:45  root
Automated import from Sourceforge.net

Revision 1.3  2003/06/26 19:22:07  haocl
cleaned up code, added comments and execution instructions

*/
