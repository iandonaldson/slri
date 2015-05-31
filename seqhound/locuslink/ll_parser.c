/* $Id: ll_parser.c,v 1.10 2004/12/14 19:43:51 zwang Exp $
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
 

  PROGRAM
 

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
  Locus Link flat file parser
  Part of SeqHound

****************************************************************************/
#include <ll_db.h>
#include <ll_parser.h>
#include <ll_db_misc.h>
#include <slri_misc.h>


#define LL_FLOAT_CONST 1000
/*************************************************************************
                       MISCELLANEOUS FUNCTIONS
**************************************************************************/

/* appends cddnew to the CDD linked list in ps.
   Updates the link list as necessary */
static void LL_Append_CDDStruct(LL_ParseStructPtr ps, LL_CDDStructPtr cddnew)
{
  if(ps == NULL){
    ErrPostEx(SEV_ERROR, 0, 0, "LL_Append_CDDStruct: NULL ParseStructPtr.");
    return;
  }
  if(cddnew == NULL){
    ErrPostEx(SEV_ERROR, 0, 0, "LL_Append_CDDStruct: NULL CDDStructPtr.");
    return;
  }

  if(ps->cdd_st == NULL){
    ps->cdd_st = cddnew;
  }else{
    ps->last->next = cddnew;
  }
  ps->last = cddnew;
}

/**********************************************************************
                        TOKENIZER FUNCTION
***********************************************************************/

/* bsp contains a record where the fields are separated by tokenizer. returns
   a the token at position tokenPosition. It is the calling functions responsibility 
   to store and free this string returned. bsp is not altered. The fields of bsp 
   are strings of characters that are alphanumeric in nature, ie 0-9 and a-zA-Z. */
static CharPtr LL_LineTokenizer(const ByteStorePtr bsp, CharPtr tokenizer, Int4 tokenPosition)
{
  ByteStorePtr bsDup;
  CharPtr cpline, p;
  CharPtr retToken;
  long ltmpLen = 0;
  Int4 i = 1;

  if(bsp == NULL){
    ErrPostEx(SEV_ERROR, 0, 0, "LL_LineTokenizer: NULL ByteStorePtr.");
    return NULL;
  }

  if(tokenizer == NULL){
    ErrPostEx(SEV_ERROR, 0, 0, "LL_LineTokenizer: NULL tokenizer string.");
    return NULL;
  }

  if(tokenPosition <= 0){
    ErrPostEx(SEV_ERROR, 0, 0, "LL_LineTokenizer: Invalid token position %d.", tokenPosition);
    return NULL;
  }

  /* copies into cpbar the string starting at the first tokenizer */
  cpline = BSStringSave(bsp);
  p = StringStr(cpline, tokenizer);
  bsDup = BSDup(bsp);
  ltmpLen = BSLen(bsp);
  if ( StringLen(p) == BSLen(bsp) ){
    /* There is a tokenizer to start the string off. */
    BSRemoveLeadingChar(bsDup, 1L);   /* remove the token */
    /* assumes that there is a token to end the string also */
    BSSeek(bsDup, -1L, SEEK_END);
    BSDelete(bsDup, 1L);
    BSSeek(bsDup, 0L, SEEK_SET);
  }
  
  while(i < tokenPosition){
    /* search for the first alpha-numeric char */
    p = StringPBrk(p, ALPHA_STRING);
    ltmpLen = StringLen(p);
    bsDup = BSFree(bsDup); bsDup = BSNew(0L);
    BSWrite(bsDup, p, ltmpLen);
    /* get the tokenizer string */
    p = StringStr(p, tokenizer);
    i++;
  }
  if(p != NULL){
    ltmpLen = ltmpLen - StringLen(p);
  }
  retToken = MemNew( (ltmpLen+1) * sizeof(Char) );
  BSSeek(bsDup, 0L, SEEK_SET);

  /* copy our token */
  BSRead(bsDup, retToken, ltmpLen);
  
  BSFree(bsDup);
  MemFree(cpline);
  return retToken;
}

/***********************************************************
                  PRIVATE PARSING FUNCTIONS
************************************************************/
/* These functions are private helper functions called by LL_LineParser
   or one of LL_LineParser's helper functions.  These should not
   be called directly by the end user. */

/* stores the locus link Id contained in bsp into ps. bsp must have
   been parsed from the locus link data file format, ie <<n where n is
   the locus link id. 
*/
static void LL_StoreLocusID(LL_ParseStructPtr ps, ByteStorePtr bsp)
{
  CharPtr cpline = NULL;

  if(ps == NULL){
    ErrPostEx(SEV_ERROR, 0, 0, "LL_StoreLocusID: NULL ParseStructPtr.");
    return;
  }

  if(bsp == NULL){
    ErrPostEx(SEV_ERROR, 0, 0, "LL_StoreLocusID: NULL ByteStorePtr.");
    return;
  }

  BSRemoveLeadingChar(bsp, 2L);  /* removing the "<<" */
  cpline = BSStringSave(bsp);
  ps->iLLID = atoi(cpline);
  MemFree(cpline);
}

/* stores the MAP information into ps. 
   bsp must have been parsed from the locus link data file format. 
*/
static void LL_ParseMAP(LL_ParseStructPtr ps, ByteStorePtr bsp, FILE *fp)
{
  if(ps == NULL){
    ErrPostEx(SEV_ERROR, 0, 0, "LL_ParseMAP: NULL ParseStructPtr.");
    return;
  }
  if(bsp == NULL){
    ErrPostEx(SEV_ERROR, 0, 0, "LL_ParseMAP: NULL ByteStorePtr.");
    return;
  }
  if(fp == NULL){
    ErrPostEx(SEV_ERROR, 0, 0, "LL_ParseMAP: NULL FILE POINTER.");
    return;
  }

  BSRemoveLeadingChar(bsp, 5L);     /* remove the "MAP: " */
  ps->cpMAP = LL_LineTokenizer(bsp, "|", 1);
}

/* stores the OMIM information into ps. bsp must been parsered from
   the locus link data file.
*/
static void LL_ParseOMIM(LL_ParseStructPtr ps, ByteStorePtr bsp)
{
  CharPtr pline;

  if(ps == NULL){
    ErrPostEx(SEV_ERROR, 0, 0, "LL_ParseOMIM: NULL Parse Struct.");
    return;
  }
  if(bsp == NULL){
    ErrPostEx(SEV_ERROR, 0, 0, "LL_ParseOMIM: NULL ByteStorePtr.");
    return;
  }

  BSRemoveLeadingChar(bsp, 6L);   /* removing the "OMIM: " */
  pline = BSStringSave(bsp);
  ValNodeAddInt(&ps->vnpOMIM, 0, atoi(pline));
  MemFree(pline);
}

/* stores NP and the following CDD information into cddnew. bsp must have
   been parsed from a locus link data file format.
*/
static Int4 LL_ParseNPUnit(LL_CDDStructPtr cddnew, ByteStorePtr bsp, FILE *fp)
{
  CharPtr token = NULL;
  CharPtr pline = NULL;
  Int4 i;

  if ( NULL == cddnew || NULL == bsp || NULL == fp )
  {
    ErrPostEx(SEV_ERROR, 0, 0, "[LL_ParseNPUnit]: Invalid parameter.");
    return -1;
  }

  BSRemoveLeadingChar(bsp, 4L);   /* removing the "NP: " */
  
  /* retrieving the NPid */
  token = LL_LineTokenizer(bsp, "|", 2);
  cddnew->iNPid = atoi(token);
  MemFree(token);

  i = BSGetLineFromFile(bsp, fp);
  
  while(i != EOF){
    pline = BSStringSave(bsp);

    if( StringStr(pline, "CDD: ") != NULL )
    {
      MemFree(pline);
      BSRemoveLeadingChar(bsp, 5L);     /* removing the "CDD: " */

      /* storing the cddid */
      token = LL_LineTokenizer(bsp, ":", 1);
      ValNodeAddStr(&cddnew->vnpCDD_ID, 0, token);

      /* storing the evalue */
      token = LL_LineTokenizer(bsp, "|", 4);
      if(StringCmp(token, "na") == 0){
	/* token is not a float */
	ValNodeAddFloat(&cddnew->vnpE_VAL, 0, LL_EVAL_CONST);
      }else{
	/* token is a float */	
	ValNodeAddFloat(&cddnew->vnpE_VAL, 0, atof(token));
      }
      MemFree(token);
    } else {
      break;
    }
    i = BSGetLineFromFile(bsp, fp);
  }
  MemFree(pline);
  return i;
}

/* parses the GO information of fp. stores the info into ps. */
static Int4 LL_ParseGOUnit(LL_ParseStructPtr ps, ByteStorePtr bsp, FILE *fp)
{
  CharPtr pline = NULL, 
          token = NULL, 
	   goID = NULL;
  Int4 i = 0;

  while( i != EOF ){

    pline = BSStringSave(bsp);

    if( StringStr(pline, "GO: ") != NULL ){
      MemFree(pline);

      BSRemoveLeadingChar(bsp, 4L);    /* remove the "GO: " */

      /* retrieving the GO id */
      token = LL_LineTokenizer(bsp, "|", 4);
      goID = StringPBrk(token, "0123456789");
      if(goID != NULL){
	ValNodeAddInt(&ps->vnpGOID, 0, atoi(goID));
      } else {
	ValNodeAddInt(&ps->vnpGOID, 0, 0);
      }
      MemFree(token);

      token = LL_LineTokenizer(bsp, "|", 3);
      ValNodeAddStr(&ps->vnpE_CODE, 0, token);

      token = LL_LineTokenizer(bsp, "|", 6);
      if( StringCmp(token, "na") == 0){            
	/* not a integer so we store a fixed constant 1000 */
	ValNodeAddInt(&ps->vnpPMID, 0, LL_FLOAT_CONST);
      }else {                                      /* an integer */
	ValNodeAddInt(&ps->vnpPMID, 0, atoi(token));
      }
      MemFree(token);
    }else{
      break;
    }
    i = BSGetLineFromFile(bsp, fp);
  }
  MemFree(pline);
  return i;
}  

/* stores XP and the following CDD information into cddnew. bsp must have
   been parsed from a locus link data file format.
*/
static Int4 LL_ParseXPUnit ( LL_CDDStructPtr cddnew, 
		ByteStorePtr bsp, FILE *fp )
{
  CharPtr token = NULL;
  CharPtr pline = NULL;
  Int4 i;

  if( NULL == cddnew || NULL == bsp || NULL == fp ){
    ErrPostEx(SEV_ERROR, 0, 0, "[LL_ParseXPUnit]: Parameter error.");
    return -1;
  }

  BSRemoveLeadingChar(bsp, 4L);   /* removing the "NP: " */
  
  /* retrieving the NPid */
  token = LL_LineTokenizer(bsp, "|", 2);
  cddnew->iNPid = atoi(token);
  MemFree(token);

  i = BSGetLineFromFile(bsp, fp);
  
  while(i != EOF)
  {
    pline = BSStringSave(bsp);

    if( StringStr(pline, "CDD: ") != NULL )
    {
      MemFree(pline);
      BSRemoveLeadingChar(bsp, 5L);     /* removing the "CDD: " */

      /* storing the cddid */
      token = LL_LineTokenizer(bsp, "|", 2);
      ValNodeAddStr(&cddnew->vnpCDD_ID, 0, token);

      /* storing the evalue */
      token = LL_LineTokenizer(bsp, "|", 4);
      if(StringCmp(token, "na") == 0){
	/* token is not a float */
	ValNodeAddFloat(&cddnew->vnpE_VAL, 0, LL_EVAL_CONST);
      }else{
	/* token is a float */	
	ValNodeAddFloat(&cddnew->vnpE_VAL, 0, atof(token));
      }
      MemFree(token);
    } else {
      break;
    }
    i = BSGetLineFromFile(bsp, fp);
  }
  MemFree(pline);
  return i;
}

/* parses the contents of the locus link data file pointed to by fp.
   Just call this function once. 
*/
static Int4 LL_LineParser(FILE *fp)
{
  Int4 i                    = 0;
  CharPtr pline             = NULL;
  ByteStorePtr bsp          = BSNew(0L);
  LL_ParseStruct *ps        = MemNew(sizeof(LL_ParseStruct));
  Boolean np_set            = FALSE;
  LL_CDDStruct *cddnew;

  if(fp == NULL || !bsp )
  {
    ErrPostEx(SEV_ERROR, 0, 0, "[LL_LineParser]: Parameter error.");
    return -1;
  }

  BSGetLineFromFile(bsp, fp);
  LL_StoreLocusID(ps, bsp);

  i = BSGetLineFromFile(bsp, fp);

  {
  CharPtr str = NULL;
  while( i != EOF )
  {
    str = NULL;
    pline = BSStringSave(bsp);
    /* printf ( "new line: %s\n", pline ); */
 
    if( (NULL != ( str = StringStr(pline, ">>") ))  && 
        ( StringLen (pline) == StringLen (str) )   )
    {
      /* append record & mem free */
      LL_AppendRecord(ps);
      UndoInitLL_ParseStruct(ps);
      MemFree(ps);

      ps = MemNew ( sizeof(LL_ParseStruct) );
      LL_StoreLocusID(ps, bsp);
      np_set = FALSE;
      if ( ps->iLLID == 0 ) break;
    }
    else if( (NULL != StringStr(pline, "NP: ") ) || 
             (NULL != StringStr(pline, "XP: ")) )
    {
      if ( NULL != StringStr(pline, "NP: ") )
      {
         /* NP gi set */
	 np_set = TRUE;
         /* printf ( "Saving NP " ); */
         cddnew = MemNew ( sizeof ( LL_CDDStruct ) );
	 i = LL_ParseNPUnit ( cddnew, bsp, fp );
         /* printf("\tNP %d\n", cddnew->iNPid); */
      } else if (NULL != StringStr(pline, "XP: ") && ! np_set ) {

	 /* NP gi not set, so use XP */
         /* printf ( "Saving XP " ); */
	 cddnew = MemNew ( sizeof ( LL_CDDStruct ) );
	 i = LL_ParseXPUnit ( cddnew, bsp, fp );
         /* printf("\tNP %d\n", cddnew->iNPid); */
      } else {

	 /* NP gi already set, so not use XP */
         /* printf("Skipping XP\n"); */
	 i = BSGetLineFromFile ( bsp, fp );
         MemFree(pline);
         continue;
      }

      /* since ParseNPUnit reads in lines, 
       * which may be start of new record, have to go to top */
      LL_Append_CDDStruct(ps, cddnew);
      MemFree(pline);
      continue;
    }
    else if( StringStr(pline, "OMIM: ") != NULL){ 
      /* printf("Saving OMIM\n"); */
      LL_ParseOMIM(ps, bsp);
    }
    else if( StringStr(pline, "MAP: ") != NULL){ 
      /* have to store MAP info and LLid */
      /* printf("Saving MAP "); */
      LL_ParseMAP(ps, bsp, fp);
      /* printf("MAP %s\n", IsNullStr(ps->cpMAP)); */
    }
    else if( StringStr(pline, "GO: ") != NULL){ 
      /* printf("Saving GO "); */
      i = LL_ParseGOUnit(ps, bsp, fp);
      
      /* since ParseGOUnit reads in lines, which 
       * may be start of new record, have to go to top */
      MemFree(pline);
      continue;
    }
    else if (i == EOF){
      break;
    }
    MemFree(pline);
    /* printf ( "get new line\n" ); */
    i = BSGetLineFromFile(bsp, fp);
  }}

  BSFree(bsp);
  MemFree(pline);
  UndoInitLL_ParseStruct(ps);
  MemFree(ps);
  return 0;
}

/*************************************************************
                       PUBLIC FUNCTION
**************************************************************/

/* 
   This is the one public function.  Pass it the path to the file
   to be parsed and the path where you want the DB files to be stored.
   The contents of the parser file must be in the format of the 
   locus link datafiles
*/
Int4 LL_ParseFile(CharPtr ParsePath, Int2 IsUpdate)
{
  FILE *fp;

  if(ParsePath == NULL){
    ErrPostEx(SEV_FATAL, 0, 0, "LL_ParseFile: NULL path for data file.");
    return -1;
  }

  /* opening the GO file to be parsed */
  fp = FileOpen(ParsePath, "r");
  if(fp == NULL) {
    ErrPostEx(SEV_ERROR, 0, 0, "LL_ParseFile: Error opening the file.");
    return -1;
  }

  /* initialize the codebase files and the ByteStructure */
  LL_InitCodeBase(TRUE);

  /* opening the database files */
  LL_OpenAllDBs(TRUE);

  /* starting transaction */
  if (!LL_StartTransaction()) {
    ErrPostEx(SEV_ERROR, 0, 0, "Cannot start transaction!");
    LL_InitUndoCodeBase();
    return -1;
  }  

  /* deleting the following existing records for update */
  /* - all in LL_OMIM
     - all with ll_id>0 in LL_GO and LL_LLINK 
     - nothing in LL_CDD */ 
  if (IsUpdate) {
    if (!DeleteRecordsInLL()) {
       ErrPostEx(SEV_ERROR,0,0, "Cannot delete all records from LL tables!");
       if (!LL_RollBackTransaction()) {
          ErrPostEx(SEV_ERROR,0,0, "Cannot roll back!");
          LL_InitUndoCodeBase();
          return -1;
       }
       LL_InitUndoCodeBase();
       return -1;
    }
  }


  /*printf("Starting Parsing\n");*/

  /* the parsing function, it loops until the end of the file */
  LL_LineParser(fp);

  /* get here, the parsing is done, uninitialize the
     codebase and ByteStructure */
  FileClose(fp);

  /* Committing all changes or rolling back */
  if (!LL_CommitTransaction()) {
     ErrPostEx(SEV_ERROR, 0, 0, "Cannot commit transaction!");
     if (!LL_RollBackTransaction()) {
        ErrPostEx(SEV_ERROR,0,0, "Cannot roll back!");
        LL_InitUndoCodeBase();
        return -1;
     }
     LL_InitUndoCodeBase();
     return -1;
  }

  LL_InitUndoCodeBase();

  /*printf("Parsing Completed\n"); */
  return 1;
}


/*
$Log: ll_parser.c,v $
Revision 1.10  2004/12/14 19:43:51  zwang
Added ability of building a transactional database:
Made LL_ParseFile take one more parameters IsUpdate.

Revision 1.9  2004/06/10 17:01:00  zwang
Add flag to determine if a database connection should be established. See bug #2324.

Revision 1.8  2004/06/07 22:52:55  hlieu
initialize varaible, bug 2185, cross comp warnings

Revision 1.7  2004/05/13 17:37:09  hlieu
major refactor of locuslink module. Locus ID now become the
'primary' key for the module, it used to be Gi's.

Revision 1.6  2004/04/16 18:23:42  vgu
Rollback changes in version 1.4

Revision 1.5  2004/04/15 20:34:52  hlieu
fix for bug 2081

Revision 1.4  2004/04/14 14:17:15  vgu
Parser will open tables with read/write deny on and reindexing to improve performance

Revision 1.3  2004/03/11 20:45:18  hlieu
fixed changed in format of LL_tmpl

Revision 1.2  2004/01/06 22:32:13  skuznets
Code audit has been done sucessfully.
Possible NULL pointer issues has been eliminated.

Revision 1.1.1.7  2002/12/18 09:31:07  root
Automated import from Sourceforge.net

Revision 1.7  2002/12/17 16:44:02  haocl
minor program termination bug fix

Revision 1.6  2002/09/10 20:45:19  kaca
removed printf statements, they confuse cron job

Revision 1.5  2002/07/08 18:01:11  kaca
fixed compiler varnings

Revision 1.4  2001/12/19 23:26:37  kaca
added newline at the EOF

Revision 1.3  2001/12/11 20:19:51  kaca
added Log and Id keywords

*/



