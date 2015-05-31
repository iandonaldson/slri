/* $Id: go_db2.c,v 1.3 2005/04/26 14:24:34 rcavero Exp $
***************************************************************************
  Copyright (C) 2005 Mount Sinai Hospital (MSH)

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
            Ruth Isserlin, Hao Lieu


  REVISION/CONTRIBUTION NOTES:

  March 1, 2001  Original MSH Revision.
                Hogue Lab - University of Toronto Biochemistry 
                Department and the Samuel Lunenfeld Research 
                Institute, Mount Sinai Hospital
                http://bioinfo.mshri.on.ca  hogue@mshri.on.ca
                ftp://ftp.mshri.on.ca/


  PUBLICATION TO CITE:

  DESCRIPTION:
  Gene Ontology database management but the DB2 layer
  Part of SeqHound

****************************************************************************/
#include <sqlcli1.h>
#include <go_db.h>
#include <slri_db2.h>
#include <slri_misc.h>
#include <go_parser.h>

/* two variables used in every method to communicate with 
the db2 database */

static SQLHANDLE henv_GO; /*envirnment handle */
static SQLHANDLE hdbc_GO; /* connection handle */

static SQLHANDLE hstmt_goname, hstmt_gosynonym, hstmt_goreference, hstmt_goparent;

/**************************************************
 *       PUBLIC DB2 INITIALIZTION           *
 **************************************************/

/* In db2 layer the Initcodebase function forms a connection to 
the db2 database.*/
void GO_InitCodeBase(void)
{
  SQLCHAR db2alias[10];
  SQLCHAR usr[10];
  SQLCHAR passwd[10];
  
  GetAppParam("intrez", "datab", "username", NULL, (Char*)usr, (size_t)10 * (sizeof(Char)));
  GetAppParam("intrez", "datab", "password", NULL, (Char*)passwd, (size_t)10 * (sizeof(Char)));
  GetAppParam("intrez", "datab", "db2alias", NULL, (Char*)db2alias, (size_t)10 * (sizeof(Char)));

   /* allocate an envirnment handle */
   if (SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv_GO) != SQL_SUCCESS){
     print_err(henv_GO);	
     ErrPostEx(SEV_ERROR,0,0, "Unable to allocate environment handle.");
   }

   /* allocate a connection handle */
   if (SQLAllocHandle(SQL_HANDLE_DBC, henv_GO, &hdbc_GO) == SQL_ERROR){
	print_err(henv_GO);
	ErrPostEx(SEV_ERROR,0,0, "Unable to allocate connection handle");
   }

   if(SQLConnect( hdbc_GO, db2alias, SQL_NTS, usr, SQL_NTS, passwd, SQL_NTS) != SQL_SUCCESS){
	ErrPostEx(SEV_ERROR,0,0, "unable to connect to the database. ");
   }

   CreateHandle(&hstmt_goparent,"INSERT into seqhound.go_parent(go_id,parent_goid) values(?,?)", &hdbc_GO,&henv_GO);
   CreateHandle(&hstmt_goreference,"INSERT into seqhound.go_reference(go_id,go_ref,go_ref_db) values(?,?,?)",&hdbc_GO,&henv_GO);
   CreateHandle(&hstmt_gosynonym,"INSERT INTO seqhound.go_synonym(go_id,go_syn) VALUES(?,?)",&hdbc_GO,&henv_GO);
   CreateHandle(&hstmt_goname,"INSERT into seqhound.go_name(go_id,go_name,go_db,go_level) values(?,?,?,?)",&hdbc_GO,&henv_GO);

}

void Go_ConnectToDb(void){
  SQLCHAR db2alias[10];
  SQLCHAR usr[10];
  SQLCHAR passwd[10];

  GetAppParam("intrez", "datab", "username", NULL, (Char*)usr, (size_t)10 * (sizeof(Char)));
  GetAppParam("intrez", "datab", "password", NULL, (Char*)passwd, (size_t)10 * (sizeof(Char)));
  GetAppParam("intrez", "datab", "db2alias", NULL, (Char*)db2alias, (size_t)10 * (sizeof(Char)));

   /* allocate an envirnment handle */
   if (SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv_GO) != SQL_SUCCESS){
     print_err(henv_GO);
     ErrPostEx(SEV_ERROR,0,0, "Unable to allocate environment handle.");
   }

   /* allocate a connection handle */
   if (SQLAllocHandle(SQL_HANDLE_DBC, henv_GO, &hdbc_GO) == SQL_ERROR){ 
        print_err(henv_GO);
        ErrPostEx(SEV_ERROR,0,0, "Unable to allocate connection handle");
   }
  
   if(SQLConnect( hdbc_GO, db2alias, SQL_NTS, usr, SQL_NTS, passwd, SQL_NTS) != SQL_SUCCESS){
        ErrPostEx(SEV_ERROR,0,0, "Unable to connect to database");
   }
  
}

/* closes the databases.  This function must called at the
   end of your function, if you called GO_InitCodeBase.*/
void GO_InitUndoCodeBase(void)
{
     Int2 sqlrc = SQL_SUCCESS;

 /* commit all changes */
  SQLTransact(henv_GO, hdbc_GO, SQL_COMMIT);

   /*Free the database handles used to communicate with the database*/
   sqlrc = SQLDisconnect( hdbc_GO);
   if (sqlrc != SQL_SUCCESS){
	ErrPostEx(SEV_INFO, 0, 0, "Could not disconnect from the database.");
   }

   /*free the connnection handle */
   sqlrc = SQLFreeHandle(SQL_HANDLE_DBC, hdbc_GO);
   if (sqlrc != SQL_SUCCESS) {
	ErrPostEx(SEV_INFO, 0, 0, "could not free the connection handle.");
   }

   /*free envirnment handle */
   sqlrc = SQLFreeHandle(SQL_HANDLE_ENV, henv_GO);
   if (sqlrc != SQL_SUCCESS) {
	ErrPostEx(SEV_INFO, 0, 0, "could not free the environment handle.");	
   }
 
}

void CloseAllGODBs(void){

}

/**********************************************************
       PRIVATE CODE BASE INITIALIZATION FUNCTIONS
***********************************************************/

/* initializes the fields and tags of the file */
static void GO_InitNameDBFields (void)
{
 
}

static void GO_InitParentDBFields(void)
{

}

static void GO_InitSynonymDBFields(void)
{
 
}

static void GO_InitReferenceDBFields(void)
{
 
}

/***************************************************
           PRIVATE OPEN DATABASE FUNCTIONS
		   Do not need to explicitly open tables
		   in the db2 database layer.  These functions
		   are not used.
****************************************************/
static void GO_OpenNameDB(CharPtr path)
{
 
}

static void GO_OpenParentDB(CharPtr path)
{

}

static void GO_OpenSynonymDB(CharPtr path)
{
 
}

static void GO_OpenReferenceDB(CharPtr path)
{
 
}

/********************************************************
             PUBLIC OPEN DATABASE FUNCTIONS
*********************************************************/

/* opens all the databases associated with the GO.  This
   function must be called prior to accessing the database.*/
Boolean GO_OpenAllDBs(Boolean CREATE)
{
 return TRUE;
}
/************************************************************
                     PRIVATE PRINT FUNCTIONS
*************************************************************/
/* prints the parents of goid */
static void PrintParents(Int4 goID)
{
/*  Int4 rc = 0;
  printf("\tParents\n");
  printf("\t-------\n");

  d4tagSelect(pD4GO_P, pT4goID_P);
  rc = d4seekDouble(pD4GO_P, goID);

  for (; rc == r4success; rc = d4seekNextDouble(pD4GO_P, goID))
  {
    printf("\t%d\n", f4int(pF4go_P));
  }
  printf("\n");
*/
}

/* prints the children of goid */
static void PrintChildren(Int4 goID)
{
/*  Int4 rc = 0;
  printf("\tChildren\n");
  printf("\t--------\n");

  d4tagSelect(pD4GO_P, pT4p);
  rc = d4seekDouble(pD4GO_P, goID);

  for (; rc == r4success; rc = d4seekNextDouble(pD4GO_P, goID))
  {
    printf("\t%d\n", f4int(pF4goID_P));
  }
  printf("\n");
*/
}

/* prints the synonyms of goid */
static void PrintSynonym(Int4 goID)
{
/*  Int4 rc = 0;
  printf("\tSynonym\n");
  printf("\t-------\n");

  d4tagSelect(pD4GO_S, pT4goID_S);
  rc = d4seekDouble(pD4GO_S, goID);

  for (; rc == r4success; rc = d4seekNextDouble(pD4GO_S, goID))
  {
    printf("\t%s\n", f4memoStr(pF4go_S));
  }
  printf("\n");
*/
}

/* prints the references of goID */
static void PrintReferences(Int4 goID)
{
/*  Int4 rc = 0;
  printf("\tReference\n");
  printf("\t---------\n");

  d4tagSelect(pD4GO_R, pT4goID_R);
  rc = d4seekDouble(pD4GO_R, goID);

  for (; rc == r4success; rc = d4seekNextDouble(pD4GO_R, goID))
  {
    printf("\t%s\n", f4memoStr(pF4go_R));
  }
  printf("\n");
*/
}

/* prints the parents, children synonym and references of gene with go_id
*/
static void PrintAllInfo(Int4 go_id)
{
/*  PrintParents(go_id);
  PrintChildren(go_id);
  PrintSynonym(go_id);
  PrintReferences(go_id);
  */
}

/* called by GODB_PrintAllRecords to print out the records */
void GO_PrintRecords(void)
{
/*  Int4 	rc = 0;
  Int4 go;

  d4tagSelect(pD4GO_N, pT4goID_N);
  for(rc = d4top(pD4GO_N); rc == r4success; rc = d4skip(pD4GO_N, 1L)) 
  {
    go = f4int(pF4goID_N);
    printf("GO:%d", go);
    printf(" %s\n", f4memoStr(pF4go_N));
    PrintAllInfo(go);
    printf("\n");
  }
  */
}

/* prints the records of vnp. This function is called by GODB_PrintRecordsInList
   of query.c. You should not call this yourself. */
void GO_PrintRecordsInList(ValNodePtr vnp)
{
/*  Int4 go = 0;
  ValNodePtr v = NULL;
  d4tagSelect(pD4GO_N, pT4goID_N);
  for(v = vnp; v != NULL; v = v->next){
    go = v->data.intvalue;
    d4seekDouble(pD4GO_N, go);
    printf("GO:%d ", go);
    printf("%s\n", f4memoStr(pF4go_N));
    PrintAllInfo(go);
    printf("\n");
  }
  */
}
/******************************************************************
                     PRIVATE RETRIEVAL FUNCTIONS
*******************************************************************/

/* returns a linked list of GO ids of the records with ID as a GO id.
   This function is called by GODB_GetRecordByID of query.c. You should not call this yourself.
This fucntion to me seems to just check if a certain ID is in the table.  Codebase
Code just select the GO-id from the go_name table and returned a list of the Go-ids that 
had this go_id.
*/
ValNodePtr GO_GetRecordByID(Int4 ID)
{

   CharPtr search_goname = "select go_id from seqhound.go_name where go_id=?";
   SQLHANDLE hstmt;
   Int2 sqlrc=SQL_SUCCESS;
   struct{
     SQLINTEGER len;
     SQLINTEGER val;
   }goid;
   ValNodePtr vnp = NULL;
   
    Go_ConnectToDb();

   CreateHandle(&hstmt, search_goname,&henv_GO,&hdbc_GO);
   SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_LONG,SQL_INTEGER, 0, 0, &ID, 0, NULL);

  /* set auto commit on */
   SQLSetConnectAttr( hdbc_GO, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_ON, SQL_NTS);  

  /* execute statment */
  if (SQLExecute(hstmt) != SQL_SUCCESS){
	ErrPostEx(SEV_ERROR,0,0,"GO_GetRecordByID:unable to execute select statement.");
	print_err(hstmt);
	SQLFreeStmt(hstmt, SQL_DROP);
	return NULL;
  }

   SQLBindCol(hstmt,1,SQL_C_LONG,&goid.val,0,&goid.len);  
  
  sqlrc = SQLFetch(hstmt);
  if(sqlrc == SQL_NO_DATA_FOUND){
      ErrPostEx(SEV_INFO, 0, 0, "No data found for goid : %ld",ID);
        SQLFreeStmt(hstmt, SQL_DROP);
      return NULL;
  }
  while(sqlrc == SQL_SUCCESS || sqlrc == SQL_SUCCESS_WITH_INFO){
   ValNodeAddInt(&vnp, 0, goid.val);
   sqlrc = SQLFetch(hstmt);
  }

   /* Free statement handle. */
   if (SQLFreeHandle(SQL_HANDLE_STMT, hstmt) != SQL_SUCCESS){
	print_err(hstmt);
	return NULL;
   }
  GO_InitUndoCodeBase();
  return vnp;
}

/* returns a linked list of all the records with reference type ref.
   This function is called by GODB_GetRecordByReference of query.c
   You should not call this yourself.
*/
ValNodePtr GO_GetRecordByReference(CharPtr ref)
{
    CharPtr search_goref = "select go_id from seqhound.go_reference where go_ref_db=?";
   SQLHANDLE hstmt;
   Int2 sqlrc=SQL_SUCCESS;
   struct{
     SQLINTEGER len;
     SQLINTEGER val;
   }goid;
   ValNodePtr vnp = NULL;
   Char ref_select[1024];
   Int4 reflen=0;

    Go_ConnectToDb();

   CreateHandle(&hstmt, search_goref,&henv_GO,&hdbc_GO);
   SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_DEFAULT,SQL_VARCHAR, 1024, 0, ref_select, 0, (long*)&reflen);

   sprintf(ref_select,"%s",IsNullStr(ref));
   reflen = strlen(ref_select);

  /* set auto commit on */
   SQLSetConnectAttr( hdbc_GO, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_ON, SQL_NTS);  

  /* execute statment */
  if (SQLExecute(hstmt) != SQL_SUCCESS){
	ErrPostEx(SEV_ERROR,0,0,"GO_GetRecordByReference:unable to execute select statement.");
	print_err(hstmt);
	SQLFreeStmt(hstmt, SQL_DROP);
	return NULL;
  }

   SQLBindCol(hstmt,1,SQL_C_LONG,&goid.val,0,&goid.len);  
  
  sqlrc = SQLFetch(hstmt);
  if(sqlrc == SQL_NO_DATA_FOUND){
      ErrPostEx(SEV_INFO, 0, 0, "No data found for ref : %s",IsNullStr(ref));
        SQLFreeStmt(hstmt, SQL_DROP);
      return NULL;
  }
  while(sqlrc == SQL_SUCCESS || sqlrc == SQL_SUCCESS_WITH_INFO){
   ValNodeAddInt(&vnp, 0, goid.val);
   sqlrc = SQLFetch(hstmt);

  }

   /* Free statement handle. */
   if (SQLFreeHandle(SQL_HANDLE_STMT, hstmt) != SQL_SUCCESS){
	print_err(hstmt);
	return NULL;
   }

   GO_InitUndoCodeBase();
  return vnp;
  
}

/* returns the children of goID. This function is called by 
   GODB_GetChildrenOf of query.c. You should not call this yourself. */
ValNodePtr GO_GetChildrenOf(Int4 goID)
{
   CharPtr search_goparent = "select go_id from seqhound.go_parent where parent_goid=?";
   SQLHANDLE hstmt;
   Int2 sqlrc=SQL_SUCCESS;
   struct{
     SQLINTEGER len;
     SQLINTEGER val;
   }goid;
   ValNodePtr vnp = NULL;

  Go_ConnectToDb();

   CreateHandle(&hstmt, search_goparent,&henv_GO,&hdbc_GO);
   SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_LONG,SQL_INTEGER, 0, 0, &goID, 0, NULL);

  /* set auto commit on */
   SQLSetConnectAttr( hdbc_GO, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_ON, SQL_NTS);  

  /* execute statment */
  if (SQLExecute(hstmt) != SQL_SUCCESS){
	ErrPostEx(SEV_ERROR,0,0,"GO_GetChildrenOf:unable to execute select statement.");
	print_err(hstmt);
	SQLFreeStmt(hstmt, SQL_DROP);
	return NULL;
  }

   SQLBindCol(hstmt,1,SQL_C_LONG,&goid.val,0,&goid.len);  
  
  sqlrc = SQLFetch(hstmt);
  if(sqlrc == SQL_NO_DATA_FOUND){
      ErrPostEx(SEV_INFO, 0, 0, "No data found for goid : %ld",goID);
        SQLFreeStmt(hstmt, SQL_DROP);
      return NULL;
  }
  while(sqlrc == SQL_SUCCESS || sqlrc == SQL_SUCCESS_WITH_INFO){
   ValNodeAddInt(&vnp, 0, goid.val);
   sqlrc = SQLFetch(hstmt);

  }

   /* Free statement handle. */
   if (SQLFreeHandle(SQL_HANDLE_STMT, hstmt) != SQL_SUCCESS){
	print_err(hstmt);
	return NULL;
   }

   GO_InitUndoCodeBase();
  return vnp;
}

/* returns the parents of goID. This function is called by
   GODB_GetParentOf of query.c. You should not call this yourself. */
ValNodePtr GO_GetParentOf(Int4 goID)
{
   CharPtr search_goparent = "select parent_goid from seqhound.go_parent where go_id=?";
   SQLHANDLE hstmt;
   Int2 sqlrc=SQL_SUCCESS;
   struct{
     SQLINTEGER len;
     SQLINTEGER val;
   }goid;
   ValNodePtr vnp = NULL;

    Go_ConnectToDb();

   CreateHandle(&hstmt, search_goparent,&henv_GO,&hdbc_GO);
   SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_LONG,SQL_INTEGER, 0, 0, &goID, 0, NULL);

  /* set auto commit on */
   SQLSetConnectAttr( hdbc_GO, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_ON, SQL_NTS);  

  /* execute statment */
  if (SQLExecute(hstmt) != SQL_SUCCESS){
	ErrPostEx(SEV_ERROR,0,0,"GO_GetParentOf:unable to execute select statement.");
	print_err(hstmt);
	SQLFreeStmt(hstmt, SQL_DROP);
	return NULL;
  }

   SQLBindCol(hstmt,1,SQL_C_LONG,&goid.val,0,&goid.len);  
  
  sqlrc = SQLFetch(hstmt);
  if(sqlrc == SQL_NO_DATA_FOUND){
      ErrPostEx(SEV_INFO, 0, 0, "No data found for goid : %ld",goID);
        SQLFreeStmt(hstmt, SQL_DROP);
      return NULL;
  }
  while(sqlrc == SQL_SUCCESS || sqlrc == SQL_SUCCESS_WITH_INFO){
   ValNodeAddInt(&vnp, 0, goid.val);
   sqlrc = SQLFetch(hstmt);
  }

   /* Free statement handle. */
   if (SQLFreeHandle(SQL_HANDLE_STMT, hstmt) != SQL_SUCCESS){
	print_err(hstmt);
	return NULL;
   }

   GO_InitUndoCodeBase();
  return vnp;
}

/* returns the ancestors of goID.  This function is called by
   GODB_GetAllAncestorsOf of query.c. You should not call this yourself. */
ValNodePtr GO_GetAllAncestors(Int4 goID)
{
  
  Int4 go = 0, goParent = 0;
  ValNodePtr vnp = NULL, v = NULL, vnp2= NULL;
  
  vnp = GO_GetParentOf(goID);
  v = vnp;
  
  while(v != NULL) {
    
    go = v->data.intvalue;
	vnp2 = GO_GetParentOf(go);
    while(vnp2 != NULL){
		goParent = vnp2->data.intvalue;
        if(!Misc_IsIntInList(vnp, goParent))
	      ValNodeAddInt(&vnp, 0, goParent);
		vnp2 = vnp2->next;
	}
	v = v->next;
  }
  return vnp;
}

/* returns the children of goID and all their children. This function is called by
   GODB_GetAllChildren of query.c. You should not call this yourself. */
/* I think he means get all descendants of a certain goid */
ValNodePtr GO_GetAllChildren(Int4 goID)
{
  Int4 go = 0, goChild = 0;
  ValNodePtr vnp = NULL, v = NULL, vnp2 = NULL;
  
  vnp = GO_GetChildrenOf(goID);
  v = vnp;
  
  while(v != NULL) {
    
    go = v->data.intvalue;
   vnp2 = GO_GetChildrenOf(go);
    while(vnp2 != NULL){
		goChild = vnp2->data.intvalue;
        if(!Misc_IsIntInList(vnp, goChild))
	      ValNodeAddInt(&vnp, 0, goChild);
		vnp2 = vnp2->next;
	}
	v = v->next;
  }
  return vnp;
}

/* returns the name of goID, this function is called by
   GODB_GetNameByID of query.c. You should not call this yourself. */
CharPtr GO_GetNameByID(Int4 goID)
{
   CharPtr search_goname = "select go_name from seqhound.go_name where go_id=?";
   SQLHANDLE hstmt;
   Int2 sqlrc=SQL_SUCCESS;
   CharPtr return_val = NULL;
   struct{
     SQLINTEGER len;
     Char val[1024];
   }goname;
	
   Go_ConnectToDb();
   CreateHandle(&hstmt, search_goname,&henv_GO,&hdbc_GO);
   SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_LONG,SQL_INTEGER, 0, 0, &goID, 0, NULL);
  /* set auto commit on */
   SQLSetConnectAttr( hdbc_GO, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_ON, SQL_NTS);  
  /* execute statment */
  if (SQLExecute(hstmt) != SQL_SUCCESS){
	ErrPostEx(SEV_ERROR,0,0,"GO_GetNameByID:unable to execute select statement.");
	print_err(hstmt);
	SQLFreeStmt(hstmt, SQL_DROP);
	return NULL;
  }
    SQLBindCol(hstmt,1,SQL_C_CHAR,goname.val,1024,&goname.len);    
  sqlrc = SQLFetch(hstmt);
  if(sqlrc == SQL_NO_DATA_FOUND){
      ErrPostEx(SEV_INFO, 0, 0, "No data found for goid : %ld",goID);
        SQLFreeStmt(hstmt, SQL_DROP);
      return NULL;
  }
  else if(sqlrc == SQL_SUCCESS || sqlrc == SQL_SUCCESS_WITH_INFO){
   return_val = StringSave(goname.val);
  }

   /* Free statement handle. */
   if (SQLFreeHandle(SQL_HANDLE_STMT, hstmt) != SQL_SUCCESS){
	print_err(hstmt);
	return NULL;
   }
   GO_InitUndoCodeBase();
  return return_val;
}

/* GO databases are organized into 3 principles, molecular function, biological process
   and cellular component. This function returns the GO database file
   goID was parsed from. If goID was extracted from the 
   molecular function database, returns 1; if from the biological process db, 
   returns 2; if from the cellular component db, returns 3.
   This function is called by GODB_GetClassificationByID of query.c. You 
   should not call this yourself.
*/
CharPtr GO_GetClassification(Int4 goID)
{
   CharPtr search_goname = "select go_db  from seqhound.go_name where go_id=?";
   SQLHANDLE hstmt;
   Int4 return_val = -1;
   Int2 sqlrc=SQL_SUCCESS;
   struct{
     SQLINTEGER len;
     SQLINTEGER val;
   }classification;

   Go_ConnectToDb();

   CreateHandle(&hstmt, search_goname,&henv_GO,&hdbc_GO);
   SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_LONG,SQL_INTEGER, 0, 0, &goID, 0, NULL);

  /* set auto commit on */
   SQLSetConnectAttr( hdbc_GO, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_ON, SQL_NTS);  

  /* execute statment */
  if (SQLExecute(hstmt) != SQL_SUCCESS){
	ErrPostEx(SEV_ERROR,0,0,"GO_GetClassification:unable to execute select statement.");
	print_err(hstmt);
	SQLFreeStmt(hstmt, SQL_DROP);
	return NULL;
  }

   SQLBindCol(hstmt,1,SQL_C_LONG,&classification.val,0,&classification.len);  
  
  sqlrc = SQLFetch(hstmt);
  if(sqlrc == SQL_NO_DATA_FOUND){
      ErrPostEx(SEV_INFO, 0, 0, "No data found for goid : %ld",goID);
        SQLFreeStmt(hstmt, SQL_DROP);
      return NULL;
  }
  else if(sqlrc == SQL_SUCCESS || sqlrc == SQL_SUCCESS_WITH_INFO){
   return_val = classification.val;
  }

   /* Free statement handle. */
   if (SQLFreeHandle(SQL_HANDLE_STMT, hstmt) != SQL_SUCCESS){
	print_err(hstmt);
	return NULL;
   }
   
   GO_InitUndoCodeBase();
  
   if( return_val == GO_MOLFUNCTION ){
    return GO_MOL_FUN;
  } else if (return_val == GO_BIOPROCESS) {
    return GO_BIO_PRO;
  } else if (return_val == GO_CELLCOMPONENT) {
    return GO_CELL_COM;
  }
  return NULL;  

}

/* returns the hierarchy level of goID. If return value is -1
   goID is not in the database. This function is called by 
   GODB_GetHierarchyLevel of query.c. You should not call this yourself.
*/
Int4 GO_GetHierarchyLevel(Int4 goID)
{
  CharPtr search_goname = "select go_level  from seqhound.go_name where go_id=?";
   SQLHANDLE hstmt;
   Int4 return_val = -1;
   Int2 sqlrc=SQL_SUCCESS;
   struct{
     SQLINTEGER len;
     SQLINTEGER val;
   }level;

   Go_ConnectToDb();

   CreateHandle(&hstmt, search_goname,&henv_GO,&hdbc_GO);
   SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_LONG,SQL_INTEGER, 0, 0, &goID, 0, NULL);

  /* set auto commit on */
   SQLSetConnectAttr( hdbc_GO, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_ON, SQL_NTS);  

  /* execute statment */
  if (SQLExecute(hstmt) != SQL_SUCCESS){
	ErrPostEx(SEV_ERROR,0,0,"GO_GetHierarchyLevel:unable to execute select statement.");
	print_err(hstmt);
	SQLFreeStmt(hstmt, SQL_DROP);
	return -1;
  }

   SQLBindCol(hstmt,1,SQL_C_LONG,&level.val,0,&level.len);  
  
  sqlrc = SQLFetch(hstmt);
  if(sqlrc == SQL_NO_DATA_FOUND){
      ErrPostEx(SEV_INFO, 0, 0, "No data found for goid : %ld",goID);
        SQLFreeStmt(hstmt, SQL_DROP);
      return -1;
  }
  else if(sqlrc == SQL_SUCCESS || sqlrc == SQL_SUCCESS_WITH_INFO){
   return_val = level.val;
  }

   /* Free statement handle. */
   if (SQLFreeHandle(SQL_HANDLE_STMT, hstmt) != SQL_SUCCESS){
	print_err(hstmt);
	return -1;
   }

   GO_InitUndoCodeBase();
  return return_val;
}
/**************************************************************
                 PRIVATE APPENDING FUNCTIONS			       
***************************************************************/
/* if entry does not exist, adds a goID and name entry 
   to the name db, if the entry already exists, returns
   GO_APPEND_ALREADY_EXIST, else returns GO_APPEND_SUCCESS. */
/* adds a name to the database, if the name is not present */
static Int4 GO_Append_Name(Int4 goID, CharPtr name, Int4 DB, Int4 Level)
{
   Char name_insert[1024];
   Int4 namelen= 0;

 printf("NAME: GoId : %ld, Name : %s, DB: %ld , Level: %ld\n", goID,IsNullStr(name),DB,Level);

    /*set up the input parameters */
   SQLBindParameter(hstmt_goname, 1, SQL_PARAM_INPUT, SQL_C_LONG,SQL_INTEGER, 0, 0, &goID, 0, NULL);
   SQLBindParameter(hstmt_goname, 2, SQL_PARAM_INPUT, SQL_C_DEFAULT,SQL_VARCHAR, 1024, 0, name_insert, 0, (long*)&namelen);
   SQLBindParameter(hstmt_goname, 3, SQL_PARAM_INPUT, SQL_C_LONG,SQL_INTEGER, 0, 0, &DB, 0, NULL);
   SQLBindParameter(hstmt_goname, 4, SQL_PARAM_INPUT, SQL_C_LONG,SQL_INTEGER, 0, 0, &Level, 0, NULL);
  
   sprintf(name_insert,"%s",IsNullStr(name));
   namelen = strlen(name_insert);

  /* execute statment */
  if (SQLExecute (hstmt_goname) != SQL_SUCCESS){
	ErrPostEx(SEV_ERROR,0,0, "GO_Append_name: unable to execute GO_Name insert statement. go_id = %ld ", goID);
	print_err(hstmt_goname);
	return GO_APPEND_ALREADY_EXIST;
  }

  return GO_APPEND_SUCCESS;
}

/* if entry does not exist, adds a goID and parent entry 
   to the parent db, if the entry already exists, returns
   GO_APPEND_ALREADY_EXIST, else returns GO_APPEND_SUCCESS. */
static Int4 GO_Append_Parent(Int4 goID, Int4 parentID)
{
 printf("Parent: GoId : %ld, ParentID : %ld\n", goID,parentID);

    /*set up the input parameters */
   SQLBindParameter(hstmt_goparent, 1, SQL_PARAM_INPUT, SQL_C_LONG,SQL_INTEGER, 0, 0, &goID, 0, NULL);
   SQLBindParameter(hstmt_goparent, 2, SQL_PARAM_INPUT, SQL_C_LONG,SQL_INTEGER, 0, 0, &parentID, 0, NULL);
  
  /* execute statment */
  if (SQLExecute (hstmt_goparent) != SQL_SUCCESS){
	ErrPostEx(SEV_ERROR,0,0, "GO_Append_Parent: unable to execute GO_Parent insert statement. ");
	print_err(hstmt_goparent);
	return GO_APPEND_ALREADY_EXIST;
  }

  return GO_APPEND_SUCCESS;
}

/* if entry does not exist, adds a goID and synonym entry 
   to the Synonym db, if the entry already exists, returns
   GO_APPEND_ALREADY_EXIST, else returns GO_APPEND_SUCCESS. */
static Int4 GO_Append_Synonym(Int4 goID, CharPtr synonym)
{
   Char synonym_insert[1024];
   Int4 synonymlen= 0;

   printf("SYN: GoId : %ld, Synonym : %s\n", goID,IsNullStr(synonym));

    /*set up the input parameters */
   SQLBindParameter(hstmt_gosynonym, 1, SQL_PARAM_INPUT, SQL_C_LONG,SQL_INTEGER, 0, 0, &goID, 0, NULL);
   SQLBindParameter(hstmt_gosynonym, 2, SQL_PARAM_INPUT, SQL_C_DEFAULT,SQL_VARCHAR, 1024, 0, synonym_insert, 0, (long*)&synonymlen);
  
   sprintf(synonym_insert,"%s",IsNullStr(synonym));
   synonymlen = strlen(synonym_insert);

  /* execute statment */
  if (SQLExecute (hstmt_gosynonym) != SQL_SUCCESS){
	ErrPostEx(SEV_ERROR,0,0, "GO_Append_synonym: unable to execute GO_Synonym insert statement. ");
	print_err(hstmt_gosynonym);
	return GO_APPEND_ALREADY_EXIST;
  }

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
   CharPtr refType = NULL;
   Char reference_insert[1024], type[10];
   Int4 referencelen= 0,typelen=0;

   printf("REF : Go_ID : %ld, ref: %s\n",goID,IsNullStr(reference));

    /* storing the reference type into a charptr */
   refType = StoreRefType(reference);

    /*set up the input parameters */
   SQLBindParameter(hstmt_goreference, 1, SQL_PARAM_INPUT, SQL_C_LONG,SQL_INTEGER, 0, 0, &goID, 0, NULL);
   SQLBindParameter(hstmt_goreference, 2, SQL_PARAM_INPUT, SQL_C_DEFAULT,SQL_VARCHAR, 1024, 0, reference_insert, 0, (long*)&referencelen);
   SQLBindParameter(hstmt_goreference, 3, SQL_PARAM_INPUT, SQL_C_DEFAULT,SQL_VARCHAR, 10, 0, type, 0, (long*)&typelen);

   sprintf(reference_insert,"%s",IsNullStr(reference));
   referencelen = strlen(reference_insert);
   sprintf(type,"%s",IsNullStr(refType));
   typelen = strlen(type);

  /* execute statment */
  if (SQLExecute (hstmt_goreference) != SQL_SUCCESS){
	ErrPostEx(SEV_ERROR,0,0, "GO_Append_reference: unable to execute GO_Ref insert statement. go Id %ld ", goID);
	print_err(hstmt_goreference);
	return GO_APPEND_ALREADY_EXIST;
  }

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
  }

  MemFree(pch);
  
  /* adding a go id and parent entry for each possible combination */
  if (bsparent->pvnGO_id != NULL) {
    
    /* save the GOID strings in bsparent as Valnode list of ints */
    vnpGOIDparent = StrIntVnp2IntVnp(bsparent->pvnGO_id);
    
    for(v = vnpGOID; v != NULL; v = v->next){
      for(pv = vnpGOIDparent; pv != NULL; pv = pv->next){
	GO_Append_Parent(v->data.intvalue, pv->data.intvalue);
      }
    }

    ValNodeFree(vnpGOIDparent);
  }

  /* adding a go id and synonym entry for each possible combination */
  if(bs->pvnGO_synonym != NULL) {
    for(v = vnpGOID; v != NULL; v = v->next){
      for(pv = bs->pvnGO_synonym; pv != NULL; pv = pv->next){
	GO_Append_Synonym(v->data.intvalue, (CharPtr)pv->data.ptrvalue);
      }
    }
  }
  
  /* adding a go id and reference entry for each possible combination */
  if(bs->pvnGO_reference != NULL) {
    for(v = vnpGOID; v != NULL; v = v->next){
      for(pv = bs->pvnGO_reference; pv != NULL; pv = pv->next){
	GO_Append_Reference(v->data.intvalue, (CharPtr)pv->data.ptrvalue);
      }
    }
  }

  ValNodeFree(vnpGOID);
  return 0;
}

