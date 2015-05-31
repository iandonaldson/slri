/* $Id: sgd_parser.c,v 1.2 2004/01/06 22:32:18 skuznets Exp $
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

  Parser for *.faa files found at NCBI refseq yeast
  ftp://ftp.ncbi.nih.gov/refseq/S_cerevisiae/ChrXX/*.faa
  
  format of file:
    Accession Number
    GI
    S. cerevisiae ORF name
  where fields are obtained from the defline of a fasta output

****************************************************************************/


#ifdef __cplusplus
extern "C" {
#endif

#include <sgd_parser.h>
#include <sgd_db.h>

/* prints the entire database, one record per line */
extern Int2 SGD_PrintDB(void)
{
  /* this function wraps the PrintDataBase function located
   * in the db layer. */
  SGD_InitDataBase();
  if(SGD_OpenDB(FALSE))
    SGD_PrintDataBase();
  else {
    ErrPostEx(SEV_ERROR, 0, 0, "[SGD]: Could not open databases");
    return 0;
  }
  SGD_CloseDataBase();
  return 1;
}
	

/* takes the file pointed to 'fp' and parses the contents line-by-line 
 * and then appends them into the SGD database. 
 * Format of the file : 
 *   Accession number
 *   GI
 *   Standard S. cerevisiae ORF Name 
 *   
 *   fields  obtained from defline of fasta format */
static Int2 SGD_ReadFile(FILE *fp)
{
  /* the sgd structure so that I can pass around 1 parameter 
   * instead of 4; 3 are char * and 1 is an int */
  SGD_ST sgd = {0, NULL, NULL};
  /* line read in from file */
  CharPtr pline = NULL;
  /* temp char * for tokenizing line */
  CharPtr p = NULL;  
  /* how big each line is, -1 when reaches EOF */
  ssize_t read = 0;
  /* length of the buffer pline */
  size_t len = 0;

  char *c = NULL;
  char *orf = NULL;
  /* variables to store the strings that will be tokenized */ 
  CharPtr pAcc = (CharPtr) malloc(sizeof(char) * ACC_SIZE);
  CharPtr pORF = (CharPtr) malloc(sizeof(char) * ORF_SIZE);
  CharPtr pGI = (CharPtr) malloc(sizeof(char) * GI_SIZE);
  if(pAcc == NULL || pORF == NULL || pGI == NULL)
  {
    ErrPostEx(SEV_ERROR, 0, 0, "[SGD]: Could not allocated char *"); 
    return -1;
  }

  /* get line from the input file, 'read' is size of line 
   * read in 'pline' buffer stores line, 'len' is size of 
   * pline, gets initialized by getline */
  while( (read = getline(&pline, &len,fp)) != -1 )
  {
    /* we only want the defline */
    if( (c = strpbrk(pline, ">")) != NULL )
    {
      int len1, len2;
      /* parse the defline to get the gi ... */
      p = strtok(pline, ">gi|");
      /* convert to an int */
      sgd.iGI = atoi(/*strtok(NULL, "|")*/ p);
      p = strtok(NULL, "|");

      /* refseq accession */
      sgd.pAcc = strncpy(pAcc, strtok(NULL, "|"), 9);

      /* ORF is the last token on line, so search for \n */
      p = strtok(NULL, "\n");

      /* the ORF is either in the 
       *    (1)last token, or
       *    (2)the token after the last ';' */
      orf = strrchr(p, ';');   /* assuming (1) */
      if(orf == NULL) orf = p; /* this deals with (2) */
 
      /* orf now contains the string with the ORF in it, but
       * is will contain other stuff that we do not want, 
       * such as '; ' or ' ', so we look for the 1st letter */
      orf = strpbrk(orf, "ABCDEFGHIJKLMNOPQRSTUVWXYZ"); 
      
      /* now orf is : ORF [Saccharomyces cerevisiae]; we only 
       * want the ORF so we find the substring [S ...]
       * and calculate the lengths to get the right substring */
      len1 = strlen(orf);
      /* sometimes [Sac..] is missing, ... */
      if( (p = strstr(orf, " [Saccharomyces cerevisiae]")) == NULL)
      {
	 /* ... so the length of [Sacc ...] is 0 */
         len2 = 0;
      }else{ /* ... otherwise get length of [S ..] */
         len2 = strlen(strstr(orf, " [Saccharomyces cerevisiae]"));
      }
      /* save only ORF portion and add \0 to end (also
       * remove the trailing p because this not useful) */
      strncpy(pORF, orf, len1-len2); 
      pORF[len1-len2-1] = '\0';
      sgd.pORF = StringUpper(pORF);

      /* use this printf for debugging */
      /* printf("%d %s %s\n", IsNullStr(sgd.iGI), sgd.pAcc, IsNullStr(sgd.pORF)); */
      SGD_AppendRecord(&sgd);
    }
  }
  free(pORF); free(pAcc); free(pGI);
  if(pline) free(pline);
  return 1;
}

/* 'file' to be parsed. 
 * Format of the file : 
 *   Accession number
 *   GI
 *   Standard S. cerevisiae ORF Name 
 *   
 *   fields from defline of fasta format
 *   CREATE set to true if want to create SGD database, otherwise,
 *   set to FALSE to append to existing SGD database. */
extern Int2 SGD_ParseFile(CharPtr file, Boolean CREATE)
{
  FILE *fp;

  /* open the file */
  fp = FileOpen(file, "r");
  if(fp == NULL){
     ErrPostEx(SEV_FATAL, 0, 0, "[SGD] File not found");
     return -1;
  }
  
  SGD_InitDataBase();

  /* create the database files and initialize the fields & tags */
  SGD_OpenDB(CREATE);
  
  /* parsing occurs here */
  SGD_ReadFile(fp);

  SGD_CloseDataBase();
  FileClose(fp);
  return 0;
}

#ifdef __cplusplus
}
#endif

/*
$Log: sgd_parser.c,v $
Revision 1.2  2004/01/06 22:32:18  skuznets
Code audit has been done sucessfully.
Possible NULL pointer issues has been eliminated.

Revision 1.1.1.3  2003/06/28 08:45:45  root
Automated import from Sourceforge.net

Revision 1.3  2003/06/26 19:22:07  haocl
cleaned up code, added comments and execution instructions

Revision 1.2  2003/06/25 14:26:03  haocl
completed yeast ORF to GI/Accession database

Revision 1.1  2003/06/20 17:55:13  haocl
initial import of SGD database, used by Yeast GO annotation database

*/
