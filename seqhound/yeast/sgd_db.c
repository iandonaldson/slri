/* $Id: sgd_db.c,v 1.2 2004/01/06 22:32:18 skuznets Exp $
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
  CodeBase database layer that interacts with the SGD database.
  SGD database holds yeast:
    Accession
    GI
    ORF

  Contains code to append to SGD database, print all the records,

  TODO: Write an API for database 
****************************************************************************/

#include <d4all.h>
#include <sgd_db.h>
#ifdef __cplusplus
extern "C" {
#endif

/* initialize the database */
extern void SGD_InitDataBase(void)
{
  code4init(&sgd_cb);
  sgd_cb.safety = 0;
}

/* close the database */
extern void SGD_CloseDataBase(void)
{
  d4close(pd4sgd);
  code4close(&sgd_cb);
  code4initUndo(&sgd_cb);
}

/* initialize the tags and fields */
static void SGD_InitFieldnTag(void)
{
  pf4acc = d4field(pd4sgd, "YST_ACCESS");
  pf4gi = d4field(pd4sgd, "YST_GI");
  pf4orf = d4field(pd4sgd, "YST_ORF");

  pt4acc = d4tag(pd4sgd, "YST_ACC_TAG");
  pt4gi = d4tag(pd4sgd, "GI_TAG");
  pt4orf = d4tag(pd4sgd, "ORF_TAG");
  return;
}

/* open the database and initialize the fields and 
 * tags. if CREATE is TRUE, the SGD database gets
 * created, else, uses the existing SGD database */
extern Boolean SGD_OpenDB(Boolean CREATE)
{
  Char path[PATH_MAX];
  Boolean res = FALSE;
  CharPtr DBpath;
  /* retrieve field pathyst from section datab of .intrezrc file i
   * and store into the path variable */ 
  GetAppParam("intrez", "datab", "path", NULL, path, (size_t)PATH_MAX * (sizeof(Char)));
  /* pass path and 'SGD' and make /path/SGD */
  DBpath = AllocateAndCreateDBPath(path, "SGD");
  /* initialize the databases and tags and field variables */
  res = OpenDBII(DBpath, &pd4sgd, fieldInfo_sgd, tagInfo_sgd,
		(InitFieldFunc) SGD_InitFieldnTag, &sgd_cb, CREATE);
  MemFree(DBpath);
  return res; 
}


/* append a record into the database */
extern Boolean SGD_AppendRecord(SGD_ST* PNTR_sgd)
{
  if(PNTR_sgd == NULL)
  {
    ErrPostEx(SEV_WARNING, 0, 0, "[SGD]: NULL pointer to SGD_ST *");
    return -1;
  }

  /* assign the data into the fields of the database */
  d4appendStart(pd4sgd, 0);
  f4assign(pf4acc, PNTR_sgd->pAcc);
  f4assignInt(pf4gi, PNTR_sgd->iGI);
  f4assign(pf4orf, PNTR_sgd->pORF);

  /* physically store the data */
  d4append(pd4sgd);
  /* printf("%s %d %s \n", IsNullStr(PNTR_sgd->pAcc), PNTR_sgd->iGI, IsNullStr(PNTR_sgd->pORF)); */
  return TRUE;
}

/* print out the SGD database, one record a line 
 * in alphabetically order based on the accession number */
extern Boolean SGD_PrintDataBase(void)
{
   Int2 rc = 0;
   CharPtr pAcc = malloc(sizeof(char) * ACC_SIZE);
   CharPtr pORF = malloc(sizeof(char) * ORF_SIZE);
   if(pAcc == NULL || pORF == NULL)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[SGD]: Could not allocate char * for printing");
     return -1;
   }

   /* start at the top (alphabetically order) of accession */
   d4tagSelect(pd4sgd, pt4acc);
   for( rc=d4top(pd4sgd); rc == r4success; rc=d4skip(pd4sgd, 1L) )
   {
     StringCpy(pAcc, f4str(d4fieldJ(pd4sgd, 1)));
     StringCpy(pORF, f4str(d4fieldJ(pd4sgd, 3)));

     printf( "%s %d %s\n", IsNullStr(pAcc), f4int(d4fieldJ(pd4sgd, 2)), IsNullStr(pORF) );
   }
   free(pAcc); free(pORF);
   return 1;
}

/**************************************************************************
                              DATABASE API
**************************************************************************/
/* searches the SGD database. One of sgd's members must be set eg:
 * sgd->pAcc = "some Accession here"
 * sgd->pORF = "some ORF" 
 * sgd->iGI = 1234 
 *
 * that member becomes the search key, if found, allocates memory and fills
 * the other twoo members with corresponding data & returns TRUE,
 * otherwise returns FALSE. User must deallocate sgd & members */
extern Boolean SearchSGD(SGD_ST *sgd)
{
   CharPtr key = NULL;    /* search values for strings */
   Int4 KeyNum = 0;       /* search value for int */
   Int2 rc = 0;             
   TAG4 *SearchTag;       /* the index for what we will be searching for */
   CharPtr pAcc = NULL, pORF = NULL;
   
   if(sgd == NULL)
   {
      ErrPostEx(SEV_WARNING, 0, 0, "[SGD]: search parameter is NULL");
      return 0;
   }

   /* Determine what is the search key and make the appropriate 
    * initialization. Is it a char or an int. */
   if(sgd->pAcc != NULL)
   {
      /* the accession is set, key = pAcc */
      key = sgd->pAcc;
      SearchTag = pt4acc;
   }else if(sgd->pORF != NULL){
      /* the ORF is set, key = pORF */
      key = sgd->pORF;
      SearchTag = pt4orf;
   }else if(sgd->iGI > 0){
      /* KeyNum = iGI*/
      KeyNum = sgd->iGI;
      SearchTag = pt4gi;
   }else{
      /* nothing was set */
      ErrPostEx(SEV_WARNING, 0, 0, "[SGD]: Invalid search key");
      free(pAcc); free(pORF);
      return -1;
   }
  
   pAcc = malloc(sizeof(char) * ACC_SIZE);
   pORF = malloc(sizeof(char) * ORF_SIZE);
   if(pAcc == NULL || pORF == NULL)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[SGD]: Could not allocate char * for searching");
     free(pAcc); free(pORF);
     return -1;
   }

   /* Select the tag set above */
   d4tagSelect(pd4sgd, SearchTag);

   if(KeyNum > 0)
   {
      /* searching using the SGDID */
      for( rc = d4seekDouble(pd4sgd, KeyNum); rc == r4success; rc = d4seekNextDouble(pd4sgd, KeyNum) )
      {
	 sgd->pAcc = StringCpy(pAcc, f4str(d4fieldJ(pd4sgd, 1)));
	 sgd->pORF = StringCpy(pORF, f4str(d4fieldJ(pd4sgd, 3)));
	 sgd->iGI = f4int(d4fieldJ(pd4sgd, 2));
         /* printf( "%s %d %s\n", IsNullStr(pAcc), f4int(d4fieldJ(pd4sgd, 2)), IsNullStr(pORF) ); */
      }
   }else{
      /* searching using key */
      for( rc = d4seek(pd4sgd, key); rc == r4success; rc = d4seekNext(pd4sgd, key))
      {
         sgd->pAcc = StringCpy(pAcc, f4str(d4fieldJ(pd4sgd, 1)));
	 sgd->pORF = StringCpy(pORF, f4str(d4fieldJ(pd4sgd, 3)));
	 sgd->iGI = f4int(d4fieldJ(pd4sgd, 2));
	 /* printf( "%s %d %s\n", IsNullStr(pAcc), f4int(d4fieldJ(pd4sgd, 2)), IsNullStr(pORF) ); */
      }
   } 
   return 1;
}

#ifdef __cplusplus
}
#endif


/*
$Log: sgd_db.c,v $
Revision 1.2  2004/01/06 22:32:18  skuznets
Code audit has been done sucessfully.
Possible NULL pointer issues has been eliminated.

Revision 1.1.1.3  2003/06/28 08:45:45  root
Automated import from Sourceforge.net

Revision 1.4  2003/06/27 14:03:53  haocl
fixed path to database

Revision 1.3  2003/06/26 19:22:07  haocl
cleaned up code, added comments and execution instructions

Revision 1.2  2003/06/25 14:26:03  haocl
completed yeast ORF to GI/Accession database

Revision 1.1  2003/06/20 17:55:13  haocl
initial import of SGD database, used by Yeast GO annotation database

*/
