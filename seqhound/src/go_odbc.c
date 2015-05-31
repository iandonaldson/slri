/* $Id: go_odbc.c,v 1.15 2005/04/26 14:26:46 rcavero Exp $
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
 

  REVISION/CONTRIBUTION NOTES:

  March 1, 2001  Original MSH Revision.
                Hogue Lab - University of Toronto Biochemistry 
                Department and the Samuel Lunenfeld Research 
                Institute, Mount Sinai Hospital
                http://bioinfo.mshri.on.ca  hogue@mshri.on.ca
                ftp://ftp.mshri.on.ca/


  PUBLICATION TO CITE:

  DESCRIPTION:
  Gene Ontology database management but the ODBC layer
  Part of SeqHound

****************************************************************************/

/**
** @file 	go_odbc.c
** 
** @brief 	implementation of go_odbc.h
** @author 	Ruth Isserlin, Hao Lieu, Minzhi Lu
** @version 	2.0
** @date 	May 26, 2003
**
*/


#include <go_odbc.h>


/***************************************************************************************
**	Global variables used for all functions
***************************************************************************************/

/* ODBC variables used in this file */

/* henv and hdbc defined in slri_odbc.h */

static SQLHANDLE V_OD_hstmt, hstmt_goname, hstmt_gosynonym, hstmt_goreference, hstmt_goparent;

/* handle error message */
static SQLRETURN                V_OD_erg;                      /* result of functions */

static SQLINTEGER               V_OD_err;

/*** for testing **/
static Boolean 			testing = TRUE;

/* 
 * global stuct used in SQLBindParameter() 
 *  so these parameters only bind once 
 */

/* for table go_name */
static struct
{
	SQLINTEGER go_id;
	Char go_name[MAXCHARLEN]; 
	SQLINTEGER go_db;
	SQLINTEGER go_level;
}go_name;

static struct
{
	SQLINTEGER go_id;
	SQLINTEGER parent_goid;
}go_parent;

/* for table go_synonym */
static struct
{
	SQLINTEGER go_id;
	Char go_syn[MAXCHARLEN]; 
}go_synonym; 

/* for table go_reference */ 
static struct 
{ 
	SQLINTEGER go_id; 
	Char go_ref[MAXCHARLEN]; 
	Char go_ref_db[50]; 
}go_reference;


/* SeqHound Global Variables */
SQLCHAR database[50];


/**************************************************
 *       PUBLIC DATABASE INITIALIZTION           *
 **************************************************/

/* 
** forms a connection to the database
**
** returns TRUE if connection is successful, FALSE otherwise
*/
Boolean
GO_InitCodeBase(void)
{
 
   SQLCHAR dsn[10];
   SQLCHAR usr[10];
   SQLCHAR passwd[10];
   Char instancebuf[25];
   SQLRETURN retcode;

   /* retrieve these 4 variables from the configuration file
    * .intrezrc in section datab. By doing it this way, we can change
    * the dsn simply by changing the configuration file
    */
   if( GetAppParam("intrez", "datab", "username", NULL, (Char*)usr, sizeof(char) * 10) <= 0 )
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[GO_InitCodeBase]: Could not find username in initialization file.");
      return FALSE;
   }
   if( GetAppParam("intrez", "datab", "password", NULL, (Char*)passwd, sizeof(char) * 10) <= 0 )
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[GO_InitCodeBase]: Could not find pasword in initialization file.");
      return FALSE;
   }
   if( GetAppParam("intrez", "datab", "dsn", NULL, (Char*)dsn, sizeof(char) * 10) <= 0 )
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[GO_InitCodeBase]: Could not find dsn in initialization file.");
      return FALSE;
   }

   /* database is a global parameter, we will use it when we initialize the sql statements,
      this way, we don't hardcode the database into the source code, allowing us to
      change the instance of the database by specifying it in the .intrezrc file, this
      is illustrated in each of the InitXXX functions below. */
   if( GetAppParam("intrez", "datab", "database", NULL, (Char*)database, sizeof(char) * 10) <= 0 )
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[GO_InitCodeBase]: Could not find database in initialization file.");
      return FALSE;
   }
   /* make sure the proper DB2 instance variable is defined--only for aix system */
   /* sprintf(instancebuf,"DB2INSTANCE=db2inst1",instance);
   putenv(instancebuf); */

   /* 1. allocate Environment handle and register version */
   V_OD_erg = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);
   if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO))
   {
      ErrPostEx(SEV_ERROR,0,0, "[GO_InitCodeBase]: Unable to allocate environment handle. SQLRETURN=%ld", V_OD_erg);
      exit(1);
   }

   /* 2. Declaring ODBC Version only for 3.x */
   V_OD_erg = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0);
   if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO))
   {
      ErrPostEx(SEV_ERROR,0,0, "[GO_InitCodeBase]: Unable to setup ODBC Verison to 3.0. SQLRETURN=%ld", V_OD_erg);
      exit(1);
   }

   /* 3. allocate connection handle. */
   V_OD_erg = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);
   if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO))
   {
      ErrPostEx(SEV_ERROR,0,0, "[GO_InitCodeBase]: Cannot allocate connection handle. SQLRETURN=%ld", V_OD_erg);
      SQLFreeHandle(SQL_HANDLE_ENV, henv);
      exit(1);
   }

   /* 4. Set Connection Attributes: Set timeout. set autocommit off */
   V_OD_erg = SQLSetConnectAttr(hdbc, SQL_LOGIN_TIMEOUT, (SQLPOINTER *)5, 0);
   if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO))
   {
      ErrPostEx(SEV_ERROR,0,0, "[GO_InitCodeBase]: Cannot set connect attr. SQLRETURN=%ld", V_OD_erg);
      SQLFreeHandle(SQL_HANDLE_ENV, henv);
      exit(1);
   }

   /* 5. Connect to the datasource */
   V_OD_erg = SQLConnect(hdbc, dsn, SQL_NTS, usr, SQL_NTS, passwd, SQL_NTS);
   if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO))
   {
      ErrPostEx(SEV_ERROR,0,0, "[GO_InitCodeBase]: Connection to database failed. SQLRETURN=%ld", V_OD_erg);
      SQLFreeHandle(SQL_HANDLE_ENV, henv);
      exit(1);
   }

	/* when in testing mode, set the autocommit to off, i
		otherwise, by default, autocommit is on */
   if (testing) {
      V_OD_erg = SQLSetConnectAttr(hdbc, 
					SQL_ATTR_AUTOCOMMIT, 
					(SQLPOINTER *)SQL_AUTOCOMMIT_ON, 
					SQL_NTS);
      if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO))
      {
         ErrPostEx(SEV_ERROR,0,0, "[GO_InitCodeBase]: failed to set autocommit off. SQLRETURN=%ld", V_OD_erg);
         SQLFreeHandle(SQL_HANDLE_ENV, henv);
         exit(1);
      }
	}

   if (InitBindParameterforInsert() != TRUE)
   {
      ErrPostEx(SEV_ERROR,0,0, "[GO_InitCodeBase]: Cannot InitBindParameterforInsert. SQLRETURN=%ld", V_OD_erg);
      exit(1);
   }

	return TRUE;

}

static Boolean
InitBindParameterforInsert()
{
   CharPtr pSQL_stmt = (char *) malloc(sizeof(Char) * 150);
   if (pSQL_stmt == NULL)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[InitBindParameterforInsert]: Could not allocate space");
     free(pSQL_stmt);
     return FALSE;
   }

   /* table go_parent */
   sprintf((char *)pSQL_stmt, "INSERT INTO %s.go_parent(go_id,parent_goid) values(?,?)", database);
   V_OD_erg = CreateHandle(&hstmt_goparent,pSQL_stmt,hdbc,&henv);
   if (V_OD_erg != SQL_SUCCESS) {
      ErrPostEx(SEV_ERROR, 0, 0, "Error CreateHandle for go_parent");
	  return FALSE;
   }

   V_OD_erg = SQLBindParameter(hstmt_goparent, 1, SQL_PARAM_INPUT, 
	           SQL_C_LONG,SQL_INTEGER, 
			   0, 0, &go_parent.go_id, 0, NULL);
   if (V_OD_erg != SQL_SUCCESS) {
      ErrPostEx(SEV_ERROR, 0, 0, "Error: unable to bind parameter 1 for go_parent. go_id=%ld SQLRETURN=%ld", go_parent.go_id, V_OD_erg);
	  return FALSE;
   }
   V_OD_erg = SQLBindParameter(hstmt_goparent, 2, SQL_PARAM_INPUT, 
	            SQL_C_LONG,SQL_INTEGER, 
				0, 0, &go_parent.parent_goid, 0, NULL);
   if (V_OD_erg != SQL_SUCCESS) {
      ErrPostEx(SEV_ERROR, 0, 0, "Error: unable to bind parameter 2 for go_parent. parent_goid=%ld SQLRETURN=%ld", go_parent.parent_goid, V_OD_erg);
	  return FALSE;
   }

   /* table go_reference */
   sprintf((char *)pSQL_stmt, "INSERT INTO %s.go_reference(go_id,go_ref,go_ref_db) values(?,?,?)", database);
   V_OD_erg = CreateHandle(&hstmt_goreference,pSQL_stmt,hdbc,&henv);
   if (V_OD_erg != SQL_SUCCESS) {
      ErrPostEx(SEV_ERROR, 0, 0, "Error CreateHandle for go_reference");
	  return FALSE;
   }

   V_OD_erg = SQLBindParameter(hstmt_goreference, 1, SQL_PARAM_INPUT, 
	             SQL_C_LONG, SQL_INTEGER, 
				 0, 0, &go_reference.go_id, 0, NULL);
   if (V_OD_erg != SQL_SUCCESS) {
      ErrPostEx(SEV_ERROR, 0, 0, "Error: unable to bind parameter 1 for go_reference. go_goid=%ld SQLRETURN=%ld", go_reference.go_id, V_OD_erg);
	  return FALSE;
   }
   V_OD_erg = SQLBindParameter(hstmt_goreference, 2, SQL_PARAM_INPUT, 
	              SQL_C_CHAR, SQL_VARCHAR, 
				  0, 0, go_reference.go_ref, sizeof(go_reference.go_ref), NULL);
   if (V_OD_erg != SQL_SUCCESS) {
      ErrPostEx(SEV_ERROR, 0, 0, "Error: unable to bind parameter 2 for go_reference. go_ref=%s SQLRETURN=%ld", go_reference.go_ref, V_OD_erg);
	  return FALSE;
   }
   V_OD_erg = SQLBindParameter(hstmt_goreference, 3, SQL_PARAM_INPUT, 
	               SQL_C_CHAR, SQL_VARCHAR, 
				   0, 0, go_reference.go_ref_db, sizeof(go_reference.go_ref_db), NULL);
   if (V_OD_erg != SQL_SUCCESS) {
      ErrPostEx(SEV_ERROR, 0, 0, "Error: unable to bind parameter 3 for go_reference. go_ref_db=%s SQLRETURN=%ld", go_reference.go_ref_db, V_OD_erg);
	  return FALSE;
   }

   /* table go_synonym */
   sprintf((char *)pSQL_stmt, "INSERT INTO %s.go_synonym(go_id,go_syn) VALUES(?,?)", database);
   V_OD_erg = CreateHandle(&hstmt_gosynonym, pSQL_stmt, hdbc,&henv);
   if (V_OD_erg != SQL_SUCCESS) {
      ErrPostEx(SEV_ERROR, 0, 0, "Error CreateHandle for go_gosynonym");
	  return FALSE;
   }

   V_OD_erg = SQLBindParameter(hstmt_gosynonym, 1, SQL_PARAM_INPUT, 
	                SQL_C_LONG,SQL_INTEGER, 
					0, 0, &go_synonym.go_id, 0, NULL);
   if (V_OD_erg != SQL_SUCCESS) {
      ErrPostEx(SEV_ERROR, 0, 0, "Error: unable to bind parameter 1 for go_gosynonym. go_id=%ld SQLRETURN=%ld", go_synonym.go_id, V_OD_erg);
	  return FALSE;
   }
   V_OD_erg = SQLBindParameter(hstmt_gosynonym, 2, SQL_PARAM_INPUT, 
					SQL_C_CHAR,SQL_VARCHAR, 
					0, 0, go_synonym.go_syn, sizeof(go_synonym.go_syn), NULL);
   if (V_OD_erg != SQL_SUCCESS) {
      ErrPostEx(SEV_ERROR, 0, 0, "Error: unable to bind parameter 2 for go_gosynonym. go_syn=%s SQLRETURN=%ld", go_synonym.go_syn, V_OD_erg);
	  return FALSE;
   }
   
   /* table go_name */
   sprintf((char *)pSQL_stmt, "INSERT into %s.go_name(go_id,go_name,go_db,go_level) values(?,?,?,?);", database);
   V_OD_erg = CreateHandle(&hstmt_goname,pSQL_stmt,hdbc,&henv);
   if (V_OD_erg != SQL_SUCCESS) {
      ErrPostEx(SEV_ERROR, 0, 0, "Error CreateHandle for go_goname");
	  return FALSE;
   }

   V_OD_erg = SQLBindParameter(hstmt_goname, 1, SQL_PARAM_INPUT,
					SQL_C_LONG,SQL_INTEGER,
					0, 0, &go_name.go_id, 0, NULL);
   if (V_OD_erg != SQL_SUCCESS) {
      ErrPostEx(SEV_ERROR, 0, 0, "Error: unable to bind parameter 1.");
	  return FALSE;
   }
   V_OD_erg = SQLBindParameter(hstmt_goname, 2, SQL_PARAM_INPUT, 
		            SQL_C_CHAR, SQL_VARCHAR, 
					0, 0, go_name.go_name, sizeof(go_name.go_name), NULL);
   if (V_OD_erg != SQL_SUCCESS) {
      ErrPostEx(SEV_ERROR, 0, 0, "Error: unable to bind parameter 2.");
	  return FALSE;
   }
   V_OD_erg = SQLBindParameter(hstmt_goname, 3, SQL_PARAM_INPUT, 
					SQL_C_LONG,SQL_INTEGER, 
					0, 0, &go_name.go_db, 0, NULL);
   if (V_OD_erg != SQL_SUCCESS) {
      ErrPostEx(SEV_ERROR, 0, 0, "Error: unable to bind parameter 3.");
	  return FALSE;
   }

   V_OD_erg = SQLBindParameter(hstmt_goname, 4, SQL_PARAM_INPUT, 
					SQL_C_LONG,SQL_INTEGER, 
					0, 0, &go_name.go_level, 0, NULL);
   if (V_OD_erg != SQL_SUCCESS) {
      ErrPostEx(SEV_ERROR, 0, 0, "Error: unable to bind parameter 4.");
	  return FALSE;
   }
 
   return TRUE;
}

/*
** forms a connection to the database
**
** returns TRUE if connection is successful, FALSE otherwise
**
*/
Boolean
GO_ConnectToDb(void)
{

        return GO_InitCodeBase();
  
}


/*
** closes the databases and free the environment handles. 
**
**  This function must called at the
**   end of your function, if you called GO_InitCodeBase.
**  
** returns TRUE if disconnection is successful, FALSE otherwise
**
*/
Boolean
GO_InitUndoCodeBase(void)
{

	/* free the statment handle */
	SQLFreeStmt(hstmt_goparent, SQL_RESET_PARAMS);
	SQLFreeStmt(hstmt_gosynonym, SQL_RESET_PARAMS);
	SQLFreeStmt(hstmt_goreference, SQL_RESET_PARAMS);
	SQLFreeStmt(hstmt_goname, SQL_RESET_PARAMS);



	/*Free the database handles used to communicate with the database*/
   V_OD_erg = SQLDisconnect(hdbc);
   if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO))
   {
      ErrPostEx(SEV_ERROR,0,0, "[GO_InitUndoCodeBase]: failed tp disconnect. SQLRETURN=%ld", V_OD_erg);
      return FALSE;
   }

	/*free the connnection handle */
   V_OD_erg= SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
   if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO))
   {
      ErrPostEx(SEV_ERROR,0,0, "[GO_InitUndoCodeBase]: failed to free connection handle. SQLRETURN=%ld", V_OD_erg);
      return FALSE;
   }

	/*free envirnment handle */
   V_OD_erg = SQLFreeHandle(SQL_HANDLE_ENV, henv);
   if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO))
   {
      ErrPostEx(SEV_ERROR,0,0, "[GO_InitUndoCodeBase]: failed to free environment handle. SQLRETURN=%ld", V_OD_erg);
      return FALSE;
   }

   return TRUE; 
}

/****************************************************************/
/* Given an SQL handle, set AutoCommit off                      */
/****************************************************************/
Boolean GO_StartTransaction(void)
{
        return SetAutoCommitOff(hdbc);
}

/****************************************************************/
/* Given an SQL handle, commit                                  */
/****************************************************************/
Boolean GO_CommitTransaction(void)
{
        return CommitChanges(hdbc);
}

/****************************************************************/
/* Given an SQL handle, roll back                               */
/****************************************************************/
Boolean GO_RollBackTransaction(void)
{
        return RollBackChanges(hdbc);
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

static ValNodePtr 
StrIntVnp2IntVnp(ValNodePtr vnp)
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
/*
** appends the record with information bs and the parent of bs. 
**
** If the record already exists, the function does not add. This function should be
** called indirectly through the parser program. You should not 
** manually call this yourself. 
**
** param:
** 	bs -
**	bsparent -
**	DB - 	DB is the organization prinicple,
**		 ie if molecular function, DB is 1; if biological process, DB is 2;
**		 if cellular component, DB is 3. 
**	Level -  Level is the hierarchy level that bs is in. 
**
** Returns 0 if successfull, -1 if the GO id
** is NULL or if the name of the gene is NULL.
**
*/
Int4 
GO_AppendRecord(ByteStructure *bs, ByteStructure *bsparent, Int4 DB, Int4 Level)
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

/******************************************************************
* delete all records in the GO tables: 
* go_name, go_parent, go_ref, go_syn
*******************************************************************/
Boolean DeleteAllRecordsInGO(void)
{
   CharPtr apchTables[4] = {"go_name", "go_parent", "go_reference", "go_synonym"};
   SQLHANDLE hstmt;
   Int2 index=0;
   Int2 sql_return = SQL_SUCCESS;

   CharPtr delete_stmt = (char *) malloc(sizeof(Char) * 150);
   if (delete_stmt == NULL)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[DeleteAllRecordsInGO]: Could not allocate space");
     free(delete_stmt);
     return FALSE;
   }

  for (index=0; index<4; index++) { 

     sprintf((char *)delete_stmt, "delete from %s.%s;", database, apchTables[index]);

     if (CreateHandle(&hstmt, delete_stmt, hdbc, &henv) != SQL_SUCCESS)
     {
        ErrPostEx(SEV_ERROR, 0, 0, "[DeleteAllRecordsInGO]: Could not create handle to delete table %s", apchTables[index]);
        free(delete_stmt);
        return FALSE;
     }

     sql_return = SQLExecute(hstmt);
     /* execute statment */
     if (sql_return != SQL_SUCCESS)
     {
        ErrPostEx(SEV_ERROR,0,0, "[DeleteAllRecordsInGO]:unable to execute delete statement for table %s.", apchTables[index]);
        LogSQLInfo(hstmt);
        SQLFreeStmt(hstmt, SQL_DROP);
        return FALSE;
     }

   }

   free(delete_stmt);
   SQLFreeStmt(hstmt, SQL_DROP);
   return TRUE;
}



/******************************************************************
                     PRIVATE RETRIEVAL FUNCTIONS
*******************************************************************/

/*
** This fucntion to me seems to just check if a certain ID is in the table.
** Codebase Code just select the GO-id from the go_name table and
** returned a list of the Go-ids that had this go_id.
**
** param 	ID: go_id
** returns 	a linked list of GO ids of the records with ID as a GO id.
**		(call node->intvalue to get id)
**		NULL if the data is not found
**
** note --  This function is called by GODB_GetRecordByID of query.c.
** You should not call this yourself.
**
** note -- the ValNodePtr's choice = 1
*/
ValNodePtr 
GO_GetRecordByID(Int4 goID)
{
   return getRecord(goID, NULL, "go_id", 1, "go_name", "go_id");
}

/*
 *
 * param 
 *	ref - the value of go_ref_db
 *
 * return 	a linked list of go_id with reference type ref
 *		NULL if data is not found
 *
 * note -- 	this function is called by GODB_GetRecordByReference of query.c 
 * 		you should not call this yourself
 * note --
 *
 */
ValNodePtr
GO_GetRecordByReference(CharPtr ref)
{
   return getRecord(-1, ref, "go_id", 1, "go_reference", "go_ref_db");
}


/*
 * param
 *	goID - the primary key of table go_parent
 *
 * returns a linked list of the children of goID. 
 *
 * note -- This function is called by 
 * 		 GODB_GetChildrenOf of query.c. You should not call this yourself. 
 *
 * note --
 *
 */
ValNodePtr 
GO_GetChildrenOf(Int4 goID)
{
   return getRecord(goID, NULL, "go_id", 1, "go_parent","parent_goid" );
}

/*
 * param
 *	goID - the primary key of table go_parent
 *
 * returns the parents of goID. 
 * 
 * note -- This function is called by
 *	  GODB_GetParentOf of query.c. You should not call this yourself. 
 * note --
 *
 */
ValNodePtr 
GO_GetParentOf(Int4 goID)
{
	return getRecord(goID, NULL, "parent_goid",1,"go_parent", "go_id");
}

/*
 *
 * param
 *	goID - 
 *
 * returns 	the name of goID,
 *		or NULL if the name is not found 
 * 
 * note		this function is called by
 *  		GODB_GetNameByID of query.c. You should not call this yourself. 
 *
 * note 	this function returns only 1 name
 *		it seems that each goID only has 1 unique name
 *
 */
CharPtr
GO_GetNameByID(Int4 goID)
{
	ValNodePtr ptr;
	CharPtr str2return; 
	ptr =  getRecord(goID, NULL, "go_name",2, "go_name", "go_id"); 
	if (ptr != NULL) {
		str2return = StringSave(ptr->data.ptrvalue);
		ValNodeFree(ptr);
		return str2return;

	} else  {
		return NULL;
	}
}


/* 
 * GO databases are organized into 3 principles, 
 * molecular function, biological process and cellular component. 
 *
 * return	This function returns the GO database file
 *		goID was parsed from. If goID was extracted from the 
 *		molecular function database, returns GO_MOL_FUN; 
 *		if from the biological process db, returns GO_BIO_PRO; 
 *		if from the cellular component db, returns GO_CELL_COM.
 *
 * note:	This function is called by GODB_GetClassificationByID 
 *		of query.c. You should not call this yourself.
*/
CharPtr 
GO_GetClassification(Int4 goID)
{
	ValNodePtr node;
	node = getRecord(goID, NULL,  "go_db", 1, "go_name", "go_id");

	if (node != NULL) {
		if( node->data.intvalue == GO_MOLFUNCTION ){
			ValNodeFree(node);
			return GO_MOL_FUN;
		} else if (node->data.intvalue == GO_BIOPROCESS) {
			ValNodeFree(node);
			return GO_BIO_PRO;
		} else if (node->data.intvalue == GO_CELLCOMPONENT) {
			ValNodeFree(node);
			return GO_CELL_COM;
		}
	} else {			/* data not found */
		ValNodeFree(node);
		return NULL;  
	}
}



/* 
 * search the table  
 * this function is used for all the other search functions 
 *
 * param: 
 *	inputID - usually go_id, parent_goid (used in ? part)
 *			if inputID = -1, means the input value is inputRef, a string
 *	inputRef - usually go_ref (used in ? part)
 *			if inputRef = "", means the input value is inputID, an integer
 *
 *	select_name - used in SELECT part, which determine the return value
 *	return_type - an integer to indicate the return type
 *			1 - integer
 *			2 - string
 *	table_name - used in FROM part
 *			the table name, should be: go_name, go_parent, go_synonym, go_reference
 *	where_name - used in WHERE part 
 *
 * return 	ValNodePtr which is a linked list contains the rows of results
 *		if the return type is integer, call node->data.intvalue
 *		if the return type is string, call node->data.ptrvalue
 * 
 * 		NULL if the data is not found  
 *
 */
static ValNodePtr
getRecord(SQLINTEGER inputID, CharPtr inputref, 
		CharPtr select_name, Int4 return_type,  
		CharPtr table_name, CharPtr where_name)
{

	SQLINTEGER outputid;
	Char outputstr[MAXCHARLEN];
	Char ref[MAXCHARLEN];	/* can not get sizeof(inputref), so use this variable instead */
	SQLINTEGER strlen=0;
	/* a ValNodePtr to hold the return value */
	ValNodePtr vnp;

	Char search_str[200];
	sprintf(search_str, "select %s from %s where %s=?", IsNullStr(select_name), IsNullStr(table_name), IsNullStr(where_name));

	if (inputref != NULL)
		sprintf(ref, inputref);
	else {
		if (inputID == -1)
			return NULL;
	}
   
	vnp = NULL;

	V_OD_erg = CreateHandle(&V_OD_hstmt, search_str, hdbc,&henv);
	if (V_OD_erg != SQL_SUCCESS) {
       ErrPostEx(SEV_ERROR, 0, 0, "Error CreateHandle for getRecord");
	   return NULL;
    }


	/* bind input value to ? part */
	if (inputID != -1) {		/* input is an integer */

		V_OD_erg = SQLBindParameter(V_OD_hstmt, 1, SQL_PARAM_INPUT, 
				SQL_C_LONG,SQL_INTEGER, 0, 0, &inputID, 0, NULL);

		if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO))
		{
		   ErrPostEx(SEV_ERROR,0,0, "[getRecord]: unable to bind parameter 1 for table %s column %s value %ld. SQLRETURN=%ld", 
			   table_name, where_name, inputID, V_OD_erg);

		   SQLFreeHandle(SQL_HANDLE_STMT, V_OD_hstmt); 
		   return NULL;
		}
	} else {		/* input is a string */
		V_OD_erg = SQLBindParameter(V_OD_hstmt, 1, SQL_PARAM_INPUT, 
				SQL_C_CHAR,SQL_VARCHAR, 0, 0, ref, sizeof(ref), NULL);

		if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO))
		{
		   ErrPostEx(SEV_ERROR,0,0, "[getRecord]: unable to bind parameter 1 for table %s column %s value %s. SQLRETURN=%ld", 
			   table_name, where_name, ref, V_OD_erg);

		   SQLFreeHandle(SQL_HANDLE_STMT, V_OD_hstmt); 
		   return NULL;
		}
	}

	/* execute statment */
	V_OD_erg = SQLExecute(V_OD_hstmt); 
	if (V_OD_erg != SQL_SUCCESS)
	{
	   ErrPostEx(SEV_ERROR,0,0, "[getRecord]: Unable to execute select statement.");
	   LogSQLInfo(V_OD_hstmt);
	   SQLFreeHandle(SQL_HANDLE_STMT, V_OD_hstmt); 
	   return NULL;
	}

	/* bind the output value */
	if (return_type == 1){	/* return type is an integer */
		V_OD_erg = SQLBindCol(V_OD_hstmt,1,SQL_C_LONG,&outputid,sizeof(outputid), NULL);  
		if (V_OD_erg != SQL_SUCCESS) {
		   ErrPostEx(SEV_ERROR, 0, 0, "Error: unable to bind col 1 for table %s column %s. SQLRETURN=%ld", 
			   table_name, where_name, V_OD_erg);
	       SQLFreeHandle(SQL_HANDLE_STMT, V_OD_hstmt); 
		   return NULL;
		}
	}
	else {	/* return type is a TEXT */
		V_OD_erg = SQLBindCol(V_OD_hstmt,1,SQL_C_CHAR,&outputstr,sizeof(outputstr),(int*)&strlen);  
		if (V_OD_erg != SQL_SUCCESS) {
		   ErrPostEx(SEV_ERROR, 0, 0, "Error: unable to bind col 2 for table %s column %s. SQLRETURN=%ld", 
			   table_name, where_name, V_OD_erg);
	       SQLFreeHandle(SQL_HANDLE_STMT, V_OD_hstmt); 
		   return NULL;
		}
	}

	/* fetch data */
	V_OD_erg = SQLFetch(V_OD_hstmt);
	if (V_OD_erg == SQL_NO_DATA_FOUND) 
	{
		SQLFreeStmt(V_OD_hstmt, SQL_DROP);
		return NULL;
	}
	while (V_OD_erg == SQL_SUCCESS || V_OD_erg == SQL_SUCCESS_WITH_INFO){

		/* add the result as a node at the end of the linked list */
		if (return_type == 1) {		/* return type is an integer */
			Slri_ValNodeAddInt(&vnp, 1, outputid);
		} else {			/* return type is a string */
			Slri_ValNodeAddStr(&vnp, 2, outputstr);
		}

		/* fetch next row */
		V_OD_erg = SQLFetch(V_OD_hstmt);
	}

	/* Free statement handle. */
	SQLFreeHandle(SQL_HANDLE_STMT, V_OD_hstmt); 

	return vnp;

}

 
/* 
 * param
 * 	goID -
 *
 * returns the ancestors of goID.  
 *
 * note - This function is called by
 * 		GODB_GetAllAncestors of query.c. You should not call this yourself.
 *
 *
 */
ValNodePtr 
GO_GetAllAncestors(Int4 goID)
{
	Int4 go = 0, goParent = 0;
	ValNodePtr vnp = NULL, v = NULL, vnp2 = NULL;
	
	vnp = GO_GetParentOf(goID);
	v = vnp;

	while (v != NULL) {

		go = v->data.intvalue;
		vnp2 = GO_GetParentOf(go);
		while (vnp2 != NULL) {
			goParent = vnp2->data.intvalue;

			if (!Misc_IsIntInList(vnp, goParent))
				Slri_ValNodeAddInt(&vnp, 0, goParent);
			vnp2 = vnp2->next;
		}
		v = v->next;
	}
	return vnp;
}
	

/* 
 * param
 *	goID - 
 *
 * returns the children of goID and all their children. 
 *
 * note - This function is called by
 *     GODB_GetAllChildren of query.c. You should not call this yourself. 
 *
 */
ValNodePtr 
GO_GetAllChildren(Int4 goID)
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
				Slri_ValNodeAddInt(&vnp, 0, goChild);
			vnp2 = vnp2->next;
		}
		v = v->next;
	}
	return vnp;
}

/*
 * ********* function disabled since it has wrong logic *********************
 *
 * returns the hierarchy level of goID. If return value is -1
 *  goID is not in the database. 
 *
 * note -- This function is called by 
 * 	 GODB_GetHierarchyLevel of query.c. You should not call this yourself.
 *
 **/
Int4 
GO_GetHierarchyLevel(Int4 goID)
{
	return -1;		
}


/**************************************************************
                 PRIVATE APPENDING FUNCTIONS			       
***************************************************************/
/*
** adds a row of data into table go_parent, if the data is not present 
**
**
** param 
**	goID --
**	parent of GO ID --
**
** return GO_APPEND_SUCCESS TRUE if append if successfully
** return GO_ERROR otherwise
**
*/
Int4
GO_Append_Parent(Int4 goID, Int4 parentID)
{
	
	go_parent.go_id = goID;
	go_parent.parent_goid = parentID;

	/* execute statment */
	V_OD_erg = SQLExecute (hstmt_goparent);
	if (V_OD_erg != SQL_SUCCESS)
	{
	   ErrPostEx(SEV_ERROR,0,0, "[GO_Append_Parent]: cannot insert. go_id=%ld, parent_goid=%5ld. SQLRETURN=%ld", 
                     goID, parentID, V_OD_erg);
	   LogSQLInfo(hstmt_goparent);
	   return GO_ERROR;
	}

	return GO_APPEND_SUCCESS;
}

/*
** adds a row of data into table go_Synonym, if the data is not present 
**
**
** param 
**	goID --
**	synomym -- synomym name
**
** return GO_APPEND_SUCCESS TRUE if append if successfully
** return GO_ERROR otherwise
*/
Int4
GO_Append_Synonym(Int4 goID, CharPtr synonym)
{

	Char syn[MAXCHARLEN];
	CharPtr newSyn=NULL;

	/* synonym must not NULL */
	if (synonym == NULL) {
	   ErrPostEx(SEV_ERROR,0,0, "[GO_Append_Synonym]: Null synonym. id=%ld", goID);
	   return GO_ERROR;
	}
 
	newSyn=(char *) malloc(sizeof(Char) * strlen(synonym) + 1);
	if (newSyn == NULL) {
	   ErrPostEx(SEV_ERROR,0,0, "[GO_Append_Synonym]: Unable to malloc for newSyn. id=%ld", goID);
	   return GO_ERROR;
	}	

	StringNCpy( syn, synonym, MAXCHARLEN );
	syn[ MAXCHARLEN ] = '\0';

	/*	sprintf(syn, "%s", IsNullStr(synonym));*/
	/* to replace '\,' in the file by ',' */
	SubstituteSubString(syn, "\\,", ",", newSyn);
	go_synonym.go_id = goID;
	sprintf(go_synonym.go_syn, "%s", IsNullStr(newSyn)); 
	free(newSyn);

	/* execute statment */
	V_OD_erg = SQLExecute (hstmt_gosynonym);
	if (V_OD_erg != SQL_SUCCESS)
	{
	   ErrPostEx(SEV_ERROR,0,0, "[GO_Append_Synonym]:cannot insert. go_id=%ld, go_syn=%s. SQLRETURN=%ld", 
                     goID, IsNullStr(syn), V_OD_erg);
	   LogSQLInfo(hstmt_gosynonym);
	   return GO_ERROR;
	}

	return GO_APPEND_SUCCESS;
}

static CharPtr
getDB(CharPtr reference)
{
	Int4 size = 0;
	CharPtr cp = NULL;
	size = StringCSpn(reference, ":");
	cp = MemNew((size_t)size * sizeof(Char)+1);
	/* cp = StringNCpy(cp, reference, (size_t)size); */
	cp = strncpy(cp, reference, (size_t)size);

	cp[ size + 1 ] = '\0';

	return cp;
}


/*
** adds a row of data into table go_reference, if the data is not present 
**
**
** param 
**	goID --
**	reference -- cross-reference DB 
**
** return GO_APPEND_SUCCESS TRUE if append if successfully
** return GO_ERROR otherwise
*/
Int4
GO_Append_Reference(Int4 goID, CharPtr reference)
{
	/* guarrente that the max length of reference is MAXCHARLEN */
	Char ref[MAXCHARLEN];
	CharPtr newRef=NULL;
	/* SK: NULL pointer check */
	char* ptr;
	go_reference.go_id = goID;

	if (reference == NULL) {
	   ErrPostEx(SEV_ERROR,0,0, "[GO_Append_Reference]: Null reference. id=%ld", goID);
	   return GO_ERROR;
	}
 
	newRef=(char *) malloc(sizeof(Char) * strlen(reference) + 1);
	if (newRef == NULL) {
	   ErrPostEx(SEV_ERROR,0,0, "[GO_Append_Reference]: Unable to malloc for newRef. id=%ld", goID);
	   return GO_ERROR;
	}	

	StringNCpy( ref, reference, MAXCHARLEN );
	ref[ MAXCHARLEN ] = '\0';

	/* to replace '\,' in the file by ',' */
	SubstituteSubString(ref, "\\,", ",", newRef);
	go_synonym.go_id = goID;
	sprintf(go_reference.go_ref, "%s", IsNullStr(newRef));
	/* SK: NULL pointer check */
	ptr = getDB(reference);
	sprintf(go_reference.go_ref_db, "%s", IsNullStr(ptr)); 
	free(newRef);

	/* execute statment */
	V_OD_erg = SQLExecute (hstmt_goreference);
	if (V_OD_erg != SQL_SUCCESS)
	{
	   ErrPostEx(SEV_ERROR,0,0, "[GO_Append_Reference]: cannot insert. go_id=%ld, go_ref=%s, go_ref_db=%s. SQLRETURN=%ld", 
                     goID, IsNullStr(ref), IsNullStr(ptr), V_OD_erg);
	   LogSQLInfo(hstmt_goreference);
	   return GO_ERROR;
	}

	return GO_APPEND_SUCCESS;
}


/*
** adds a row of data into table go_name, if the data is not present 
**
**
** param 
**	goID --
**	name --
**	DB --
**	level --
**
** return GO_APPEND_SUCCESS TRUE if append if successfully
** return GO_ERROR otherwise
**
*/
Int4
GO_Append_Name(Int4 goID, CharPtr name, Int4 DB, Int4 Level)
{	
	CharPtr newName=NULL;

	/* name must not be NULL */
	if (name == NULL) {
	   ErrPostEx(SEV_ERROR,0,0, "[GO_Append_Name]: Null name. id=%ld", goID);
	   return GO_ERROR;
	}

	newName=(char *) malloc(sizeof(Char) * strlen(name) + 1);
	if (newName == NULL) {
	   ErrPostEx(SEV_ERROR,0,0, "[GO_Append_Name]: Unable to malloc for newName. id=%ld", goID);
	   return GO_ERROR;
	}	

	go_name.go_id = goID;
	/* to replace '\,' in the file by ',' */
	SubstituteSubString(name, "\\,", ",", newName);

	StringNCpy( go_name.go_name, newName, MAXCHARLEN );
	go_name.go_name[ MAXCHARLEN ] = '\0';

	/*	sprintf(go_name.go_name, "%s", IsNullStr(newName));*/
	go_name.go_db = DB;
	go_name.go_level = Level;

	free(newName);

	/* execute statment */
	V_OD_erg = SQLExecute (hstmt_goname);
	if (V_OD_erg != SQL_SUCCESS)
	{
	   ErrPostEx(SEV_ERROR,0,0, "[GO_Append_Name]: cannot insert. go_id=%ld, go_name=%s, go_db=%ld, go_level=%ld. SQLRETURN=%ld",
           goID, IsNullStr(name), DB, Level, V_OD_erg);
	   LogSQLInfo(hstmt_goname);
	   return GO_ERROR;
	}

	return GO_APPEND_SUCCESS;
}

/**********************************************************
 functions used in intrez.c 
 **********************************************************/
void CloseAllGODBs(void) {
}

void GO_PrintRecords(void) {
}
void GO_PrintRecordsInList(ValNodePtr vnp) {
}
 

/* $Log: go_odbc.c,v $
/* Revision 1.15  2005/04/26 14:26:46  rcavero
/* Change Copyright Note.
/*
/* Revision 1.14  2004/11/18 18:55:29  vgu
/* Fix dangling pointer bug number 3966
/*
/* Revision 1.13  2004/10/01 17:30:34  zwang
/* Added transaction-related functions. See bug 3441.
/*
/* Revision 1.12  2004/08/06 14:47:21  mmatan
/* Changed CharPtr database[50] to SQLCHAR database[50]
/*
/* Revision 1.11  2004/08/05 16:20:20  zwang
/* Remove DB2 related code.
/*
/* Revision 1.10  2004/07/16 16:31:53  vgu
/* Bug 2800: When mallocing strings, make sure to add 1 if using strlen(). Also,
/* used StringNCpy, so that we are not overwriting buffers.
/*
/* Revision 1.9  2004/06/24 16:21:51  zwang
/* Use Slri_ValNodeAdd** instead of ValNodeAdd** for better performance.
/*
/* Revision 1.8  2004/06/16 20:43:41  zwang
/* Get rid of '\' in front of ',' for go_name, go_ref and go_syn. See bug #2491.
/*
/* Revision 1.7  2004/02/27 17:31:13  zwang
/* Got rid of warnings of SQLBindCol.
/* In GO_GetNameByID, change from Char[] to CharPtr to be compatible with API.
/*
/* Revision 1.6  2004/02/03 21:24:24  zwang
/* Cosmetic changes in log messages.
/*
/* Revision 1.5  2004/01/21 22:11:10  zwang
/* Removed functions which are defined in go_db.h
/*
/* Revision 1.4  2004/01/19 21:05:18  zwang
/* Changed the return values to be the same as in CodeBase layer.
/*
/* Revision 1.3  2004/01/13 17:30:32  zwang
/* Changed return macros.
/* Use generic database name.
/* Added more functions.
/* */
