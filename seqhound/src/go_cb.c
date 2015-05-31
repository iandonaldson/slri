/* $Id: go_cb.c,v 1.4 2004/01/06 22:32:15 skuznets Exp $
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
  Gene Ontology database management
  Part of SeqHound

****************************************************************************/
#include <ncbi.h>
#include <slri_cb.h>
#include <slri_misc.h>
#include <go_db_p.h>
#include <go_db.h>
#include <go_parser.h>
#include <d4all.h>   /* move to end to avoid _LARGE_FILE_SOURCE redefinition warnings */

/**************************************************
 *       PUBLIC CODE BASE INITIALIZTION           *
 **************************************************/

/* these functions initialize the codebase tags and field
   information.  Alter these only if you know what you are doing */

/* initializes the CODE4 structure and sets various flags.
   This function must be called in any application that uses 
   this database, prior to doing anything to the database.*/
void GO_InitCodeBase(void)
{
  code4init(&cb);
  cb.errOpen = 0;
  cb.safety = 1;
}

/* closes the databases.  This function must called at the
   end of your function, if you called GO_InitCodeBase.*/
void GO_InitUndoCodeBase(void)
{
  code4close(&cb);
  code4initUndo(&cb);
}

/**********************************************************
       PRIVATE CODE BASE INITIALIZATION FUNCTIONS
***********************************************************/

/* initializes the fields and tags of the file */
static void GO_InitNameDBFields (void)
{
  pF4goID_N = d4field(pD4GO_N, "GO_ID_NAME");
  pF4go_N = d4field(pD4GO_N, "GO_NAME");
  pF4go_F = d4field(pD4GO_N, "GO_DB");
  pF4go_L = d4field(pD4GO_N, "GO_LEVEL");

  pT4goID_N = d4tag(pD4GO_N, "ID_TAG_NAME");
  pT4f = d4tag(pD4GO_N, "DB_TAG");
  pT4L = d4tag(pD4GO_N, "DB_LEVEL");
}

static void GO_InitParentDBFields(void)
{
  pF4goID_P = d4field(pD4GO_P, "GO_ID_PAR");
  pF4go_P = d4field(pD4GO_P, "GO_PAR");

  pT4goID_P = d4tag(pD4GO_P, "ID_TAG_PAR");
  pT4p = d4tag(pD4GO_P, "PAR_TAG");
}

static void GO_InitSynonymDBFields(void)
{
  pF4goID_S = d4field(pD4GO_S, "GO_ID_SYN");
  pF4go_S = d4field(pD4GO_S, "GO_SYN");

  pT4goID_S = d4tag(pD4GO_S, "ID_TAG_SYN");
}

static void GO_InitReferenceDBFields(void)
{
  pF4goID_R = d4field(pD4GO_R, "GO_ID_REF");
  pF4go_R = d4field(pD4GO_R, "GO_REF");
  pF4go_I = d4field(pD4GO_R, "GO_REF_DB");

  pT4goID_R = d4tag(pD4GO_R, "ID_TAG_REF");
  pT4i = d4tag(pD4GO_R, "DB_TAG");
}

/***************************************************
           PRIVATE OPEN DATABASE FUNCTIONS
****************************************************/
static Boolean GO_OpenNameDB(CharPtr path, Boolean CREATE)
{
  CharPtr DBpath;
  Boolean res =FALSE;

  DBpath = AllocateAndCreateDBPath(path, "GO_NAME");
  res = OpenDBII(DBpath, &pD4GO_N, fieldInfo_Name, tagInfo_Name,
		  (InitFieldFunc) GO_InitNameDBFields, &cb, CREATE);
  MemFree(DBpath);
  return res;
}

static Boolean GO_OpenParentDB(CharPtr path, Boolean CREATE)
{
  CharPtr DBpath;
  Boolean res =FALSE;

  DBpath = AllocateAndCreateDBPath(path, "GO_PARENT");
  res = OpenDBII(DBpath, &pD4GO_P, fieldInfo_Parent, tagInfo_Parent,
		  (InitFieldFunc) GO_InitParentDBFields, &cb, CREATE);
  MemFree(DBpath);
  return res;
}

static Boolean GO_OpenSynonymDB(CharPtr path, Boolean CREATE)
{
  CharPtr DBpath;
  Boolean res =FALSE;

  DBpath = AllocateAndCreateDBPath(path, "GO_SYN");
  res = OpenDBII(DBpath, &pD4GO_S, fieldInfo_Synonym, tagInfo_Synonym,
		  (InitFieldFunc) GO_InitSynonymDBFields, &cb, CREATE);
  MemFree(DBpath);
  return res;
}

static Boolean GO_OpenReferenceDB(CharPtr path, Boolean CREATE)
{
  CharPtr DBpath;
  Boolean res =FALSE;

  DBpath = AllocateAndCreateDBPath(path, "GO_REF");
  res = OpenDBII(DBpath, &pD4GO_R, fieldInfo_Reference, tagInfo_Reference,
		  (InitFieldFunc) GO_InitReferenceDBFields, &cb, CREATE);
  MemFree(DBpath);
  return res;
}


/********************************************************
             PUBLIC OPEN DATABASE FUNCTIONS
*********************************************************/

/* opens all the databases associated with the GO.  This
   function must be called prior to accessing the database.*/
Boolean GO_OpenAllDBs(Boolean CREATE)
{
  Char path[PATH_MAX];
  Boolean res1 = FALSE, res2 = FALSE, res3 = FALSE, res4 = FALSE;

  GetAppParam("intrez", "datab", "path", NULL, path, (size_t)PATH_MAX * (sizeof(Char)));
  res1 = GO_OpenNameDB(path, CREATE);
  res2 = GO_OpenParentDB(path, CREATE);
  res3 = GO_OpenSynonymDB(path, CREATE);
  res4 = GO_OpenReferenceDB(path, CREATE);

  if(res1 && res2 && res3 && res4) return TRUE;
  else return FALSE;

}


/********************************************************
             PUBLIC CLOSE DATABASE FUNCTIONS
*********************************************************/
void CloseCodeBaseAndGODBs(void)
{
	/* explicity flush all DBs */
	if(pD4GO_N) d4flush(pD4GO_N);
	if(pD4GO_P) d4flush(pD4GO_P);
	if(pD4GO_S) d4flush(pD4GO_S);
	if(pD4GO_R) d4flush(pD4GO_R);
	/* close CodeBase */
	code4close(&cb);
	code4initUndo(&cb);
}
	

void CloseAllGODBs(void)
{
	if(pD4GO_N) d4close(pD4GO_N);
	if(pD4GO_P) d4close(pD4GO_P);
	if(pD4GO_S) d4close(pD4GO_S);
	if(pD4GO_R) d4close(pD4GO_R);
}


/************************************************************
                     PRIVATE PRINT FUNCTIONS
*************************************************************/
/* prints the parents of goid */
static void PrintParents(Int4 goID)
{
  Int4 rc = 0;
  printf("\tParents\n");
  printf("\t-------\n");

  d4tagSelect(pD4GO_P, pT4goID_P);
  rc = d4seekDouble(pD4GO_P, goID);

  for (; rc == r4success; rc = d4seekNextDouble(pD4GO_P, goID))
  {
    printf("\t%d\n", f4int(pF4go_P));
  }
  printf("\n");
}

/* prints the children of goid */
static void PrintChildren(Int4 goID)
{
  Int4 rc = 0;
  printf("\tChildren\n");
  printf("\t--------\n");

  d4tagSelect(pD4GO_P, pT4p);
  rc = d4seekDouble(pD4GO_P, goID);

  for (; rc == r4success; rc = d4seekNextDouble(pD4GO_P, goID))
  {
    printf("\t%d\n", f4int(pF4goID_P));
  }
  printf("\n");
}

/* prints the synonyms of goid */
static void PrintSynonym(Int4 goID)
{
  Int4 rc = 0;
  /* SK: NULL pointer check */
  char* ptr;

  printf("\tSynonym\n");
  printf("\t-------\n");

  d4tagSelect(pD4GO_S, pT4goID_S);
  rc = d4seekDouble(pD4GO_S, goID);

  for (; rc == r4success; rc = d4seekNextDouble(pD4GO_S, goID))
  {
    /* SK: NULL pointer check */
    ptr = (char *)f4memoStr(pF4go_S);
    printf("\t%s\n", IsNullStr(ptr));
  }
  printf("\n");
}

/* prints the references of goID */
static void PrintReferences(Int4 goID)
{
  Int4 rc = 0;
  /* SK: NULL pointer check */
  char* ptr;
  printf("\tReference\n");
  printf("\t---------\n");

  d4tagSelect(pD4GO_R, pT4goID_R);
  rc = d4seekDouble(pD4GO_R, goID);

  for (; rc == r4success; rc = d4seekNextDouble(pD4GO_R, goID))
  {
    /* SK: NULL pointer check */
    ptr = (char *)f4memoStr(pF4go_R);
    printf("\t%s\n", IsNullStr(ptr));
  }
  printf("\n");
}

/* prints the parents, children synonym and references of gene with go_id
*/
static void PrintAllInfo(Int4 go_id)
{
  PrintParents(go_id);
  PrintChildren(go_id);
  PrintSynonym(go_id);
  PrintReferences(go_id);
}

/* called by GODB_PrintAllRecords to print out the records */
void GO_PrintRecords(void)
{
  Int4 	rc = 0;
  Int4 go;
  /* SK: NULL pointer check */
  char* ptr;

  d4tagSelect(pD4GO_N, pT4goID_N);
  for(rc = d4top(pD4GO_N); rc == r4success; rc = d4skip(pD4GO_N, 1L)) 
  {
    go = f4int(pF4goID_N);
    printf("GO:%d", go);
    /* SK: NULL pointer check */
    ptr = (char *)f4memoStr(pF4go_N);
    printf(" %s\n", IsNullStr(ptr));
    PrintAllInfo(go);
    printf("\n");
  }
}

/* prints the records of vnp. This function is called by GODB_PrintRecordsInList
   of query.c. You should not call this yourself. */
void GO_PrintRecordsInList(ValNodePtr vnp)
{
  Int4 go = 0;
  ValNodePtr v = NULL;
  /* SK: NULL pointer check */
  char* ptr;

  d4tagSelect(pD4GO_N, pT4goID_N);
  for(v = vnp; v != NULL; v = v->next){
    go = v->data.intvalue;
    d4seekDouble(pD4GO_N, go);
    printf("GO:%d ", go);
    /* SK: NULL pointer check */
    ptr = (char *)f4memoStr(pF4go_N);
    printf("%s\n",IsNullStr(ptr));
    PrintAllInfo(go);
    printf("\n");
  }
}
/******************************************************************
                     PRIVATE RETRIEVAL FUNCTIONS
*******************************************************************/

/* returns a linked list of GO ids of the records with ID as a GO id.
   This function is called by GODB_GetRecordByID of query.c. You should not call this yourself.
*/
ValNodePtr GO_GetRecordByID(Int4 ID)
{
  Int4 go = 0;
  Int4 rc = 0;
  ValNodePtr vnp = NULL;
  
  d4tagSelect(pD4GO_N, pT4goID_N);

  for(rc = d4seekDouble(pD4GO_N, ID); rc == r4success; rc = d4seekNextDouble(pD4GO_N, ID))
  {
    go = f4int(pF4goID_N);
    ValNodeAddInt(&vnp, 0, go);
  }
  return vnp;
}

/* returns a linked list of all the records with reference type ref.
   This function is called by GODB_GetRecordByReference of query.c
   You should not call this yourself.
*/
ValNodePtr GO_GetRecordByReference(CharPtr ref)
{
  Int4 go = 0;
  ValNodePtr vnp = NULL;

  d4tagSelect(pD4GO_R, pT4goID_R);

  for(  d4top(pD4GO_R); !d4eof(pD4GO_R) ; d4skip(pD4GO_R, 1L) ){

    if( StringNCmp(ref, f4str(pF4go_I), (size_t)StringLen(ref)) == 0 ){
      go = f4int(pF4goID_R);
      ValNodeAddInt(&vnp, 0, go);
    }
  }
  return vnp;
}

/* returns the children of goID. This function is called by
   GODB_GetChildrenOf of query.c. You should not call this yourself. */
ValNodePtr GO_GetChildrenOf(Int4 goID)
{
  Int4 go = 0;
  ValNodePtr vnp = NULL;
  Int4 rc = 0;

  d4tagSelect(pD4GO_P, pT4p);

  for(rc = d4seekDouble(pD4GO_P, goID); rc == r4success; rc = d4seekNextDouble(pD4GO_P, goID)){
    go = f4int(pF4goID_P);
    ValNodeAddInt(&vnp, 0, go);
  }
  Misc_IntVnpUnique(&vnp);
  return vnp;
}


/* returns the parents of goID. This function is called by
   GODB_GetParentOf of query.c. You should not call this yourself. */
ValNodePtr GO_GetParentOf(Int4 goID)
{
  Int4 go = 0;
  ValNodePtr vnp = NULL;
  Int4 rc = 0;

  d4tagSelect(pD4GO_P, pT4goID_P);

  for(rc = d4seekDouble(pD4GO_P, goID); rc == r4success; rc = d4seekNextDouble(pD4GO_P, goID)){
    go = f4int(pF4go_P);
    ValNodeAddInt(&vnp, 0, go);
  }
  Misc_IntVnpUnique(&vnp);
  return vnp;
}


/* original Hao stuff gets only two levels
   returns the ancestors of goID.  This function is called by
   GODB_GetAllAncestorsOf of query.c. You should not call this yourself. */
/*ValNodePtr GO_GetAllAncestors(Int4 goID)
{

  Int4 go = 0, goParent = 0;
  ValNodePtr vnp = NULL, v = NULL;
  Int4 rc = 0;

  vnp = GO_GetParentOf(goID);

  d4tagSelect(pD4GO_P, pT4goID_P);
  d4top(pD4GO_P);
  v = vnp;

  while(v != NULL) {

    go = v->data.intvalue;
    for(rc = d4seekDouble(pD4GO_P, go); rc == r4success; rc = d4seekNextDouble(pD4GO_P, go)){

      goParent = f4int(pF4go_P);
      if(!Misc_IsIntInList(vnp, goParent)){
	ValNodeAddInt(&vnp, 0, goParent);
      }
    }
    v = v->next;
  }
  return vnp;
} */


/* recursive not tested
ValNodePtr GO_GetAllAncestorsInternal(ValNodePtr pvnid)
{
     ValNodePtr pvnres = NULL;

     pvnres = GO_GetAllAncestorsInternal(pvnid);
     if(pvnres != NULL)
       ValNodeLink(&pvnres, pvnid);
       return GO_GetAllAncestorsInternal(pvnres);
     else
       return pvnid;
}*/


static ValNodePtr GO_GetParentOfList(ValNodePtr pvnid)
{
  ValNodePtr pvnshort = NULL, pvnlong = NULL, pvntemp = NULL;

  if(pvnid == NULL)
     return NULL;

  while(pvnid != NULL)
  {
    if((pvnshort = GO_GetParentOf(pvnid->data.intvalue)) != NULL)
    {
      /* check if the list contains the query value to avoid circular references */
      pvntemp = pvnshort;
      while(pvntemp != NULL)
      {
        if(pvntemp->data.intvalue == pvnid->data.intvalue)
           pvnshort = Misc_RemoveNode(pvnshort,pvntemp);
        pvntemp = pvntemp->next;
      }
      ValNodeLink(&pvnlong, pvnshort);
    }
    pvnid = pvnid->next;
  }
  return pvnlong;
}


static ValNodePtr GO_GetAllAncestorsInternal(ValNodePtr pvnid)
{
  ValNodePtr pvnres = NULL, pvntemp = NULL;

  while((pvntemp = GO_GetParentOfList(pvnid)) != NULL)
  {
    /*Misc_IntVnpUnique(&pvntemp);*/
    ValNodeLink(&pvnres, pvntemp);
    pvnid = pvntemp;
  }
  return pvnres;
}


ValNodePtr GO_GetAllAncestors(Int4 goID)
{
  ValNodePtr pvn = NULL, pvnres = NULL;
  if(goID <= 0)
    return NULL;

  ValNodeAddInt(&pvn, 0, goID);
  pvnres = GO_GetAllAncestorsInternal(pvn);
  Misc_IntVnpUnique(&pvnres);
  return pvnres;
}



/* original Hao stuff gets only two levels
   returns the children of goID and all their children. This function is called by
   GODB_GetAllChildren of query.c. You should not call this yourself. */
/*ValNodePtr GO_GetAllChildren(Int4 goID)
{
  Int4 go = 0, goChild = 0;
  ValNodePtr vnp = NULL, v = NULL;
  Int4 rc = 0;

  vnp = GO_GetChildrenOf(goID);

  d4tagSelect(pD4GO_P, pT4p);
  d4top(pD4GO_P);
  v = vnp;

  while(v != NULL) {
    
    go = v->data.intvalue;
    for(rc = d4seekDouble(pD4GO_P, go); rc == r4success; rc = d4seekNextDouble(pD4GO_P, go)){
      
      goChild = f4int(pF4goID_P);
      
      if(!Misc_IsIntInList(vnp, goChild)){
	ValNodeAddInt(&vnp, 0, goChild);
      }   
    }
    v = v->next;
  }
  return vnp;
}*/



/*  recursive not tested
ValNodePtr GO_GetAllChildrenInternal(ValNodePtr pvnid)
{
     ValNodePtr pvnres = NULL;

     pvnres = GO_GetAllChildrenInternal(pvnid);
     if(pvnres != NULL)
       ValNodeLink(&pvnres, pvnid);
       return GO_GetAllChildrenInternal(pvnres);
     else
       return pvnid;
}*/

static ValNodePtr GO_GetChildrenOfList(ValNodePtr pvnid)
{
  ValNodePtr pvnshort = NULL, pvnlong = NULL, pvntemp = NULL;

  if(pvnid == NULL)
    return NULL;

  while(pvnid != NULL)
  {
    if((pvnshort = GO_GetChildrenOf(pvnid->data.intvalue)) != NULL)
    {
      /* check if the list contains the query value to avoid circular references */
      pvntemp = pvnshort;
      while(pvntemp != NULL)
      {
        if(pvntemp->data.intvalue == pvnid->data.intvalue)
           pvnshort = Misc_RemoveNode(pvnshort,pvntemp);
        pvntemp = pvntemp->next;
      }
      ValNodeLink(&pvnlong, pvnshort);
    }
    pvnid = pvnid->next;
  }
  return pvnlong;
}


static ValNodePtr GO_GetAllChildrenInternal(ValNodePtr pvnid)
{
     ValNodePtr pvnres = NULL, pvntemp = NULL;

     while((pvntemp = GO_GetChildrenOfList(pvnid)) != NULL)
     {
       ValNodeLink(&pvnres, pvntemp);
       pvnid = pvntemp;
     }
     return pvnres;
}


ValNodePtr GO_GetAllChildren(Int4 goID)
{
  ValNodePtr pvn = NULL, pvnres = NULL;
  if(goID <= 0)
    return NULL;

  ValNodeAddInt(&pvn, 0, goID);
  pvnres = GO_GetAllChildrenInternal(pvn);
  Misc_IntVnpUnique(&pvnres);
  return pvnres;
}


/* returns the name of goID, this function is called by
   GODB_GetNameByID of query.c. You should not call this yourself. */
CharPtr GO_GetNameByID(Int4 goID)
{
  Int4 rc = 0;
  CharPtr pchar = NULL;
  d4tagSelect(pD4GO_N, pT4goID_N);
  rc = d4seekDouble(pD4GO_N, goID);
  if(rc == r4after){
    return NULL;
  }
  pchar = MemNew ( (f4memoLen(pF4go_N)+1) * sizeof(Char));
  f4memoNcpy(pF4go_N, pchar, f4memoLen(pF4go_N) + 1);
  return pchar;
}

/* GO databases are organized into 3 principles, molecular function, biological process
   and cellular component. This function returns the GO database file
   goID was parsed from.  This function is called by GODB_GetClassificationByID of query.c. 
   You should not call this yourself.
*/
CharPtr GO_GetClassification(Int4 goID)
{
  Int4 go = 0;
  Int4 rc = 0;

  d4tagSelect(pD4GO_N, pT4goID_N);
  rc = d4seekDouble(pD4GO_N, goID);
  if(rc == r4after)
     return NULL;
  
  go = f4int(pF4go_F);
  if( go == GO_MOLFUNCTION ){
     return GO_MOL_FUN;
  } else if (go == GO_BIOPROCESS) {
     return GO_BIO_PRO;
  } else if (go == GO_CELLCOMPONENT) {
     return GO_CELL_COM;
  }
  return NULL; 
}

/* returns the hierarchy level of goID. If return value is -1
   goID is not in the database. This function is called by 
   GODB_GetHierarchyLevel of query.c. You should not call this yourself.
*/
/* function disabled since it has a wrong logic
Int4 GO_GetHierarchyLevel(Int4 goID)
{
  Int4 go = 0;
  Int4 rc = 0;

  d4tagSelect(pD4GO_N, pT4goID_N);
  rc = d4seekDouble(pD4GO_N, goID);
  if(rc == r4after){
    return -1;
  }
  go = f4int(pF4go_L);
  return go;
}*/

/**************************************************************
                 PRIVATE APPENDING FUNCTIONS
***************************************************************/
/* if entry does not exist, adds a goID and name entry
   to the name db, if the entry already exists, returns
   GO_APPEND_ALREADY_EXIST, else returns GO_APPEND_SUCCESS. */
/* adds a name to the database, if the name is not present */
static Int4 GO_Append_Name(Int4 goID, CharPtr name, Int4 DB, Int4 Level)
{
  Int4 rc;

  d4tagSelect(pD4GO_N, pT4goID_N);

  /* first check if the same record exists by looking for goid */
  for(rc = d4seekDouble(pD4GO_N, goID); rc == r4success; rc = d4seekNextDouble(pD4GO_N, goID))
  {
     /* then compare the gene name and the go annotation.
      * GO Annotation can be molecular_function, biological_process
      * or cellular location. */
     if( (strcmp(name, f4memoStr(pF4go_N)) == 0) && (f4int(pF4go_F) == DB) )
     {
	/* printf("Exact match found\n"); */
        return GO_APPEND_ALREADY_EXIST;
     }
  }
  /* reach here, we have a new record, so appending */
  d4appendStart(pD4GO_N, 0);
  f4assignInt(pF4goID_N, goID);
  f4memoAssign(pF4go_N, name);
  f4assignInt(pF4go_F, DB);
  f4assignInt(pF4go_L, Level);

  d4append(pD4GO_N);
  return GO_APPEND_SUCCESS;
}

/* if entry does not exist, adds a goID and parent entry
   to the parent db, if the entry already exists, returns
   GO_APPEND_ALREADY_EXIST, else returns GO_APPEND_SUCCESS. */
static Int4 GO_Append_Parent(Int4 goID, Int4 parentID)
{
  Int4 rc;

  d4tagSelect(pD4GO_P, pT4goID_P);

  /* search to see if the goid already exists in db */
  for(rc = d4seekDouble(pD4GO_P, goID); rc == r4success; rc = d4seekNextDouble(pD4GO_P, goID))
  {
    /* then compare parent id, if same, then the record already exists */
    if(parentID == f4int(pF4go_P)){
      return GO_APPEND_ALREADY_EXIST;
    }
  }
  /* reach here, we have a new record, so appending */
  d4appendStart(pD4GO_P, 0);
  f4assignInt(pF4goID_P, goID);
  f4assignInt(pF4go_P, parentID);

  d4append(pD4GO_P);
  return GO_APPEND_SUCCESS;
}

/* if entry does not exist, adds a goID and synonym entry
   to the Synonym db, if the entry already exists, returns
   GO_APPEND_ALREADY_EXIST, else returns GO_APPEND_SUCCESS. */
static Int4 GO_Append_Synonym(Int4 goID, CharPtr synonym)
{
  Int4 rc;

  d4tagSelect(pD4GO_S, pT4goID_S);

  /* search for duplicate record */
  for(rc = d4seekDouble(pD4GO_S, goID); rc == r4success; rc = d4seekNextDouble(pD4GO_S, goID))
  {
    if(StringCmp(synonym, f4memoStr(pF4go_S)) == 0){
      /* exact match found, so not appending */
      return GO_APPEND_ALREADY_EXIST;
    }
  }
  /* reach here, new record so append */
  d4appendStart(pD4GO_S, 0);
  f4assignInt(pF4goID_S, goID);
  f4memoAssign(pF4go_S, synonym);
  d4append(pD4GO_S);
  return GO_APPEND_SUCCESS;
}

static CharPtr StoreRefType(CharPtr reference)
{
  Int4 size = 0;
  CharPtr cp = NULL;

  size = StringCSpn(reference, ":");
  cp = MemNew((size_t)size * sizeof(Char)+1);
  cp = StringNCpy(cp, reference, (size_t)size);
  return cp;
}  

/* if entry does not exist, add a go id and reference entry 
   to the reference db. if the entry exists, returns GO_APPEND_ALREADY_EXIST
   else returns GO_APPEND_SUCCESS */
static Int4 GO_Append_Reference(Int4 goID, CharPtr reference)
{
  Int4 rc;
  CharPtr refType = NULL;

  d4tagSelect(pD4GO_R, pT4goID_R);

  for(rc = d4seekDouble(pD4GO_R, goID); rc == r4success; rc = d4seekNextDouble(pD4GO_R, goID))
  {
    if( StringCmp(reference, f4memoStr(pF4go_R) ) == 0 ){
      return GO_APPEND_ALREADY_EXIST;
    }
  }

  d4appendStart(pD4GO_R, 0);
  f4assignInt(pF4goID_R, goID);
  f4memoAssign(pF4go_R, reference);
  /* storing the reference type into a charptr */
  refType = StoreRefType(reference);
  f4assign(pF4go_I, refType);
  MemFree(refType);
  d4append(pD4GO_R);
  return GO_APPEND_SUCCESS;
}

static ValNodePtr StrIntVnp2IntVnp(ValNodePtr vnp)
{
  CharPtr cp = NULL;
  ValNodePtr v = NULL;

  cp = Misc_SaveStrValNodeListAsString(vnp);
  v = Misc_CommaIntString2ValNodeListOrdered(cp, ",");
  MemFree(cp);
  return v;
}

/****************************************************************
                       PUBLIC APPEND FUNCTION
*****************************************************************/
/* appends the record with information bs and the parent of bs. 
   DB is the organization prinicple, ie if molecular function, DB is 1; if 
   biological process, DB is 2; if cellular component, DB is 3. 
   Level is the hierarchy level that bs is in. If the record
   already exists, the function does not add. This function should be
   called indirectly through the parser program. You should not 
   manually call this yourself. Returns 1 if successfull, -1 if the GO id
   is NULL or if the name of the gene is NULL.
*/
Int4 GO_AppendRecord(ByteStructure *bs, ByteStructure *bsparent, Int4 DB, Int4 Level)
{
  CharPtr pch = NULL;
  ValNodePtr v = NULL,
             pv = NULL;
  ValNodePtr vnpGOID = NULL,
             vnpGOIDparent = NULL;

  /* SK: NULL ptr check */
  if ( bs == NULL )
  {
    ErrPostEx(SEV_WARNING, 0, 0, "GODB_AppendRecord: NULL bs.");
    return -1;
  }

  /* SK: NULL ptr check */
  if ( bsparent == NULL )
  {
    ErrPostEx(SEV_WARNING, 0, 0, "GODB_AppendRecord: NULL bsparent.");
    return -1;
  }


  /* adding a name and go id entry for each go id */
  if(bs->pvnGO_id == NULL){
    ErrPostEx(SEV_WARNING, 0, 0, "GODB_AppendRecord: NULL GO id.");
    return -1;
  }
  if(bs->pGO_name == NULL){
    ErrPostEx(SEV_WARNING, 0, 0, "GODB_AppendRecord: NULL GO name.");
    return -1;
  }

  /* save the name into a CharPtr */
  pch = BSStringSave(bs->pGO_name);

  /* save the GOID strings in bs as ValNode list of ints */
  vnpGOID = StrIntVnp2IntVnp(bs->pvnGO_id);

  for(v = vnpGOID; v != NULL; v = v->next){
    GO_Append_Name(v->data.intvalue, pch, DB, Level);
    /* printf("NAME %ld %s %ld %ld\n", (long) v->data.intvalue, IsNullStr(pch),  (long) DB, (long) Level); */
  }

  MemFree(pch);

  /* adding a go id and parent entry for each possible combination */
  if (bsparent->pvnGO_id != NULL) {

    /* save the GOID strings in bsparent as Valnode list of ints */
    vnpGOIDparent = StrIntVnp2IntVnp(bsparent->pvnGO_id);

    for(v = vnpGOID; v != NULL; v = v->next){
      for(pv = vnpGOIDparent; pv != NULL; pv = pv->next){
	GO_Append_Parent(v->data.intvalue, pv->data.intvalue);
        /* printf("PARENT %ld %ld\n", (long)v->data.intvalue, (long) pv->data.intvalue); */
      }
    }

    ValNodeFree(vnpGOIDparent);
  }

  /* adding a go id and synonym entry for each possible combination */
  if(bs->pvnGO_synonym != NULL) {
    for(v = vnpGOID; v != NULL; v = v->next){
      for(pv = bs->pvnGO_synonym; pv != NULL; pv = pv->next){
	GO_Append_Synonym(v->data.intvalue, (CharPtr)pv->data.ptrvalue);
        /* printf("SYNONYM %ld %s\n", (long) v->data.intvalue, IsNullStr(pv->data.ptrvalue)); */
      }
    }
  }
  
  /* adding a go id and reference entry for each possible combination */
  if(bs->pvnGO_reference != NULL) {
    for(v = vnpGOID; v != NULL; v = v->next){
      for(pv = bs->pvnGO_reference; pv != NULL; pv = pv->next){
	GO_Append_Reference(v->data.intvalue, (CharPtr)pv->data.ptrvalue);
        /* printf("REFERENCE %ld %s\n", (long) v->data.intvalue, IsNullStr(pv->data.ptrvalue)); */
      }
    }
  }

  ValNodeFree(vnpGOID);
  return 0;
}


/*
$Log: go_cb.c,v $
Revision 1.4  2004/01/06 22:32:15  skuznets
Code audit has been done sucessfully.
Possible NULL pointer issues has been eliminated.

Revision 1.3  2003/11/03 15:50:27  haocl
changed order of includes to avoid _LARGE_FILE_SOURCE redefinition warnings

Revision 1.2  2003/08/22 15:23:11  haocl
changed GetClassification back to char*, only one classification/goid

Revision 1.1.1.6  2003/08/21 08:48:18  root
Automated import from Sourceforge.net

Revision 1.7  2003/08/20 19:13:36  haocl
change so duplicate records not appended

Revision 1.6  2003/08/20 17:16:07  haocl
changed return value for GetClassification

Revision 1.5  2002/07/25 15:11:53  kaca
reviewed ValNode sorting upon return

Revision 1.4  2002/07/08 17:25:34  kaca
removed unused variables

Revision 1.3  2002/03/26 16:13:20  kaca
removed searching for the record upon appending

Revision 1.2  2002/01/15 17:28:17  kaca
disabled SHoundGODBGetHierarchyLevel due to a wrong logic

Revision 1.1  2001/12/19 23:59:52  kaca
renamed to support database layer naming convention

Revision 1.4  2001/12/11 19:01:51  kaca
added Log keyword

*/













