/* $Id: go_parser.c,v 1.5 2004/06/14 13:00:35 eburgess Exp $
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
  Gene Ontology flat file parser
  Part of SeqHound

****************************************************************************/
#include <go_db.h>
#include <go_parser.h>
#include <go_db_misc.h>
#include <slri_misc.h>
/****************************************************************
		   MISCELLANEOUS PARSING NEEDS
*****************************************************************/

/* removes a blank space from the front and the back of bsp.
   If these are not blank spaces, it is not removed */
static void GO_BSRemoveTerminalSpaces(ByteStorePtr bsp) 
{
  Int2 ch = 0;
 
  if(bsp == NULL){
    ErrPostEx(SEV_INFO, 0, 0, "GO_BSRemoveTerminalSpaces: NULL POINTER.");
    return;
  }

  /* removing tail spaces */
  BSSeek(bsp, -1L, SEEK_CUR);
  if( (Char)BSGetByte(bsp) == ' ') {
    BSSeek(bsp, -1L, SEEK_CUR);
    BSDelete(bsp, 1L);
  }

  /* removing header spaces */
  BSSeek(bsp, 0L, SEEK_SET);  
  if( (ch=(Char)BSGetByte(bsp)) == ' ') {
    BSSeek(bsp, -1L, SEEK_CUR);
    BSDelete(bsp, 1L);
  }

  BSSeek(bsp, 0L, SEEK_SET);
}

/* skips the rest of the current line in fp, then
   repositions itself at the end of current line */
static FILE *GO_SkipLine(FILE *fp)
{
  Int2 ch = 0;

  /* SK: NULL ptr check */
  if ( fp == NULL )
     return NULL;

  ch=(Int2)(fgetc(fp));

  while(ch != '\n' && ch !=EOF){
    ch=(Int2)(fgetc(fp)); 
  }

  if(ch=='\n'){
    fseek(fp, -1, SEEK_CUR);
  }

  return fp;
}

/* returns the type of field bsp is, if bsp is empty returns -1,
   if bsp is a synonym (has "synonym:") returns SYN, 
   if has "GO:" returns GOID, otherwise returns REF */
static Int4 GO_DetermineFieldType(ByteStorePtr bsp)
{  
  CharPtr pchar = NULL;
  Int4 type = 0;


  /* SK: NULL ptr check */
  if ( bsp == NULL )
     return -1;

  if (!BSIsEmpty(bsp)){
    pchar = BSStringSave(bsp);
    /* SK: NULL ptr check */
    if ( pchar == NULL )
       return -1;
    
    if(StringStr(pchar, "synonym:") != NULL){
      type = SYN;
    }else if(StringStr(pchar, "GO:") != NULL){
      type = GOID;
    }else{
      type = REF;
    }
    MemFree(pchar);
    return type;
  }
  
  /* if bsp is empty */
  return -1;
}

/* counts and returns the number of leading spaces on this line, then 
   repositions itself back to the previous position. */
static Int4 GO_CountLeadingSpaces(FILE *fp)
{
  Int2 i=0, j=0;
  Int2 ch = 0;

  /* SK: NULL ptr check */
  if ( fp == NULL )
     return 0;

  while((ch=(Int2)(fgetc(fp))) == ' '){
    i++;
  }

  j = -i-1;

  if(ch=='%'||ch=='<'){
    fseek(fp, j, SEEK_CUR);
  }

  return i;
}

/***********************************************************
                      PARSING FUNCTIONS
************************************************************/

/* if pvn is NULL,  a new Linked List is created and returned, if pvn is 
   non NULL, a new ValNode is appended to pvn, and the head of the new list is 
   returned. Leading and tailing spaces in bsp are also removed. TYPE 
   is either GOID or SYN; if GOID, the leading "GO:" in bsp is removed, if 
   SYN the leading "synonym:" is removed from bsp. */
ValNodePtr GO_SetInNode(ValNodePtr pvn, ByteStorePtr bsp, Int4 TYPE)
{
  CharPtr pchar = NULL;

  if(!BSIsEmpty(bsp)){

    GO_BSRemoveTerminalSpaces(bsp);
 
    if(TYPE == SYN){
      BSRemoveLeadingChar(bsp, 8L);      /* 8L removes the leading "synonym:" in front */
    }else if(TYPE == GOID){
      BSRemoveLeadingChar(bsp, 3L);      /* 3L removes the leading "GO:" in front */
    }
    
    pchar = BSStringSave(bsp);
    ValNodeAddStr(&pvn, TYPE, pchar);
  }
  return pvn;
}
  
/* stores the GO Id into pvn, if there are multiple GO id separated by 
   comma, pvn becomes a linked list of all the IDs.
   Return the list holding the id(s). 
*/
static ValNodePtr GO_GetGOID(ValNodePtr pvn, FILE *fp)
{
  Int2 ch = 0;
  ByteStorePtr bsp = NULL;
  
  bsp = BSNew(0L);
  /* SK: NULL ptr check */
  if ( bsp == NULL || fp == NULL )
     return NULL;

  ch=(Int2)(fgetc(fp));

  /* fields in GO are separated by semicolons */  
  while(ch != '\n' && ch !='%' && ch != '<' && ch != EOF){

    if(ch==';'){
      /* no more IDs, so we stop */
      pvn = GO_SetInNode(pvn, bsp, GOID);
      break;
    }
    if(ch==','){
      /* more IDs to come */
      pvn = GO_SetInNode(pvn, bsp, GOID);
      BSClear(bsp);
      ch=(Int2)(fgetc(fp));
      continue;
    }
    BSPutByte(bsp, ch);
    ch=(Int2)(fgetc(fp));
  }

  /* reached a newline or EOF or a new record, 
     save the recent field, then terminate */

  if(ch=='\n'){
    pvn = GO_SetInNode(pvn, bsp, GOID); 
    fseek(fp, -1, SEEK_CUR);
  } else if(ch=='%'||ch=='<'){
    pvn = GO_SetInNode(pvn, bsp, GOID);
    fp = GO_SkipLine(fp);
  }else if(ch==EOF){
    pvn = GO_SetInNode(pvn, bsp, GOID);
  }
  BSFree(bsp);
  return pvn;
}

/* read in field from fp and stores into ByteStorePtr */
static ByteStorePtr GO_BSGetField(FILE *fp)
{
  Int2 ch = 0;
  ByteStorePtr bsp = NULL;
  
  bsp = BSNew(0L);
  /* SK: NULL ptr check */
  if ( bsp == NULL || fp == NULL )
     return NULL;

  ch=(Int2)(fgetc(fp));

  /* fields in GO are separated by semicolons, so we stop if hit it */  
  while(ch != '\n' && ch !='%' && ch != '<' && ch != EOF){
    if(ch==';'){
      break;
    }
    BSPutByte(bsp, ch);
    ch=(Int2)(fgetc(fp));
  }
  
  if(ch=='\n'){
    fseek(fp, -1, SEEK_CUR);
  }else if(ch=='%' || ch=='<'){
    fp = GO_SkipLine(fp);
  }
  return bsp;
}

/* breaks a line from the GO db into separate fields and stores them
   into the components of bsDest */
static void GO_BStGetFieldsFromFile(ByteStructure *bsDest, FILE *fp)
{
  Int4 type = 0;
  ByteStorePtr bsp = NULL;

  /* SK: NULL ptr check */
  if ( bsDest == NULL || fp == NULL )
     return;
  
  /* getting the name */
  bsDest->pGO_name = GO_BSGetField(fp);
  GO_BSRemoveTerminalSpaces(bsDest->pGO_name);
  
  /* then the GO id (list) */
  bsDest->pvnGO_id = GO_GetGOID(bsDest->pvnGO_id, fp);
  
  /* then next field is optional... */
  bsp = GO_BSGetField(fp);
  type = GO_DetermineFieldType(bsp);
  
  /* ...it could be references ...*/
  while(type == REF){   
    /* in which case we store them and then get the next item */
    bsDest->pvnGO_reference = GO_SetInNode(bsDest->pvnGO_reference, bsp, type);
    BSFree(bsp);
    bsp = GO_BSGetField(fp);
    type = GO_DetermineFieldType(bsp);
  }

  /* or synonyms, or references followed by synonyms. */
  while(type == SYN){
    /* storing the synonyms */
    bsDest->pvnGO_synonym = GO_SetInNode(bsDest->pvnGO_synonym, bsp, type);
    BSFree(bsp);
    bsp = GO_BSGetField(fp);
    type = GO_DetermineFieldType(bsp);
  }

  BSFree(bsp);
  /* get the newline */
  (Int2)(fgetc(fp));
}

/* Retrieves a record from the file fp and stores it into bsDest,
   returns the number of spaces in front of the line, which expresses
   the relationship of the record 
*/
static Int4 GO_BStGetRecordFromFile(ByteStructure *bsDest, FILE *fp)
{
  Int2 ch = 0;
  Int2 i = 0;
  if(fp==NULL) {
    ErrPostEx(SEV_ERROR, 0, 0, "GO_BStGetRecordFromFile: Passed file pointer is NULL.");
    return -1;
  }

  /* SK: NULL ptr check */
  if(bsDest==NULL) {
    ErrPostEx(SEV_ERROR, 0, 0, "GO_BStGetRecordFromFile: Passed Byte Store  pointer is NULL.");
    return -1;
  }
  
  /* counting the spaces */  
  while((ch=(Int2)(fgetc(fp))) == ' '){
    i++;
  }
  
  /* reading record */
  if(ch=='%'||ch=='<') {
    GO_BStGetFieldsFromFile(bsDest, fp);
  }

  /* record is now in the ByteStructure */
  if(ch==EOF) {
    printf("REACHED END OF FILE\n");
    return(EOF);
  }

  return i;
}

/* parses line by line the opened file fp which points to a GO db. 
   Returns the number of leading spaces on next line.
   If fp is in the format of GO, this function needs to be called only once,
   after which, recursive calls will be made based on the relationships 
   between entries.
*/
static Int4 GO_LineParser(ByteStructure *ppbs, FILE *fp, Int4 DB)
{
  Int4 ns = 0,              /* number of leading spaces on current line */ 
    nsl = 0;             /* number of leading spaces on next line */

  /* this structure will hold the current record information */
  ByteStructure bs = {NULL, NULL, NULL, NULL, NULL};

  /* SK: NULL ptr check */
  if(ppbs == NULL){
    ErrPostEx(SEV_FATAL, 0, 0, "GO_LineParser: ppbs is NULL.");
  }

  if(fp == NULL){
    ErrPostEx(SEV_FATAL, 0, 0, "GO_LineParser: Error opening file.");
  }

  /* parsing the current record pointed to by fp */
  ns = GO_BStGetRecordFromFile(&bs, fp);
  if(ns == EOF){
    return ns;
  }

  /* adding all the record information into the database files.
     bs is the current record, ppbs is the parent of bs */
  GO_AppendRecord(&bs, ppbs, DB, ns);

  /* counting the leading spaces in the next line.  
     The level of indentation is the primary determinant
     of the relationships between records. If the 
     indentation is the same, they are siblings. If line k
     has fewer indentations then line k+1, k is k+1's parent. 
     If k+1 has fewer indentations than k, a new family begins with k. */
  nsl = GO_CountLeadingSpaces(fp);
  if(ns == nsl){

    /* sibling relationship */
    UndoInitBStructure(&bs);
    nsl = GO_LineParser(ppbs, fp, DB);
  }else if(nsl > ns) {

    /* parent child relationship */
    nsl = GO_LineParser(&bs, fp, DB);
    UndoInitBStructure(&bs);
    if (nsl < ns){
      return nsl; 
    }
    else {
      nsl = GO_LineParser(ppbs, fp, DB);
    }
  }else{

    /* a new lineage */
    UndoInitBStructure(&bs);
    return nsl;
  }
  return nsl;
}

static Int4 GO_DetermineDB(FILE *fp)
{
  Int2 ch;
  CharPtr pchar;
  Int4 i = 0, retval;
  ByteStorePtr bsp = BSNew(0L);;

  /* SK: NULL ptr check */
  if ( bsp == NULL )
  {
    ErrPostEx(SEV_FATAL, 0, 0, "GO_DetermineDB: bsp pointer is NULL.");
    return -1;
  }

  /* SK: NULL ptr check */
  if(fp == NULL)
  {
    ErrPostEx(SEV_FATAL, 0, 0, "GO_DetermineDB: fp pointer is NULL.");
    return -1;
  }
  
  while((ch=(Int2)(fgetc(fp))) == ' '){
    i++;
  }  
  ch = (Int2)fgetc(fp);

  while(ch != '\n' && ch !='%' && ch != '<' && ch != EOF){
    i++;
    if(ch==';'){
      break;
    }
    BSPutByte(bsp, ch);
    ch=(Int2)(fgetc(fp));
  }
  
  fseek(fp, -i-1, SEEK_CUR);
  GO_BSRemoveTerminalSpaces(bsp);
  pchar = BSStringSave(bsp);
  /* SK: NULL ptr check */
  if ( pchar == NULL )
  {
    ErrPostEx(SEV_FATAL, 0, 0, "GO_DetermineDB: pchar pointer is NULL.");
    return -1;
  }
  BSFree(bsp);

  if (StringCmp(pchar, GO_MOL_FUN) == 0){
    retval = GO_MOLFUNCTION;
  }else if (StringCmp(pchar, GO_BIO_PRO) == 0) {
    retval = GO_BIOPROCESS;
  }else if (StringCmp(pchar, GO_CELL_COM) == 0){
    retval = GO_CELLCOMPONENT;
  }else{
    ErrPostEx(SEV_ERROR, 0, 0, "GO_DetermineDB: GO Database not identifiable.");
    retval =  -1;
  }
  
  MemFree(pchar);
  return retval;
}

/*************************************************************
                       PUBLIC FUNCTION
**************************************************************/

/* This is the one public function.  Pass it the path to the file
   to be parsed and the path where you want the DB files to be stored.
   The contents of the parser file must be in the format of the GO datafiles
*/
void GODB_ParseFile(CharPtr ParsePath)
{
  FILE *fp;
  ByteStructure bs = {NULL, NULL, NULL, NULL, NULL};
  Int4 ch= 0;
  Int4 DB; /* the DB identifier */

  /* SK: NULL ptr check */
  if ( ParsePath == NULL )
  {
    ErrPostEx(SEV_FATAL, 0, 0, "GODB_ParserFile: ParsePath is NULL.");
    return;
  }

  /* opening the GO file to be parsed */
  fp = FileOpen(ParsePath, "r");
  if(fp == NULL) {
    ErrPostEx(SEV_FATAL, 0, 0, "GODB_ParserFile: Error opening the file.");
    /* SK: Missed return call */
    return;
  }


  ch = (Int2)(fgetc(fp));

  /* this while loop scans the top lines of GO db 
     and skips the comments which start with '!' or '$' */
  while( ch == '!' || ch == '$' ){
    fp = GO_SkipLine(fp);
    if(ch == '!'){
      ch = (Int2)(fgetc(fp));
      ch = (Int2)(fgetc(fp));
    }
    else if(ch == '$'){
      ch = (Int2)(fgetc(fp));
      }
  }

  /* determine which DB file it is: if fp is the molecular function, DB = 1, 
     if fp is the biological process, DB = 2, if fp is the cellular component, 
     DB = 3; else DB = -1 and an error is issued.
  */
  DB = GO_DetermineDB(fp);
  
  /* the parsing starts, this function needs to be called only
     once, after which it will recursively call itself as
     necessary, provided the file is in the format of a GO database */
  GO_LineParser(&bs, fp, DB);

  /* get here, the parsing is done, uninitialize the
     codebase and ByteStructure */
  UndoInitBStructure(&bs);
  FileClose(fp);

  /*printf("THE END\n");*/
}

/*
$Log: go_parser.c,v $
Revision 1.5  2004/06/14 13:00:35  eburgess
Fixed compile error in GO_BSRemoveTerminalSpaces. Added defined ch.

Revision 1.4  2004/06/07 22:23:01  hlieu
removed unused variables, bug 2185

Revision 1.3  2004/01/14 18:56:13  vgu
fix goparser error noted in goparserlog report on step9b in bug 1114

Revision 1.2  2004/01/06 22:32:12  skuznets
Code audit has been done sucessfully.
Possible NULL pointer issues has been eliminated.

Revision 1.1.1.4  2002/03/27 09:27:02  root
Automated import from Sourceforge.net

Revision 1.4  2002/03/26 16:14:39  kaca
moved table opening into goparser; it is done only once

Revision 1.3  2001/12/11 20:12:38  kaca
added Id and Log keywords

*/
