/* $Id: ll_odbc.c,v 1.19 2005/04/26 14:37:26 rcavero Exp $
**************************************************************************
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

****************************************************************************/

/**
** @file	ll_odbc.c
**
** @brief	implementation of ll_odbc.h
** @author	Ruth Isserlin, Hao Lieu, Minzhi Lu
** @version	2.0
** @date	May 26, 2003
**
*/

#include "ll_odbc.h"

/***************************************************************************************
**	Global variables used for all functions
***************************************************************************************/

/* henv and hdbc defined in slri_odbc.h */

/*static SQLHENV henv;
static SQLHDBC hdbc;
*/
static SQLHANDLE V_OD_hstmt, hstmt_llomim, hstmt_llcdd, hstmt_lllink, hstmt_llgo,
				hstmt_lllink_llid, hstmt_lllink_gi,
				hstmt_llgo_llid, hstmt_llgo_goid, hstmt_llgo_pmid;

/* handle error message */
static SQLRETURN                V_OD_erg;                      /* result of functions */

static SQLCHAR                  V_OD_stat[SQL_SQLSTATE_SIZE +1];                 /* Status SQL */
static SQLCHAR                  V_OD_msg[SQL_MAX_MESSAGE_LENGTH + 1];
static SQLINTEGER               V_OD_err;
static SQLSMALLINT              V_OD_mlen;

/*** for testing **/
static Boolean 			testing = TRUE;

/* SeqHound Global Variables */
SQLCHAR database[50];

/*
 * global stuct used in SQLBindParameter()
 *  so these parameters only bind once
 */

/* for table ll_go */
static struct
{
	SQLINTEGER ll_id;
	SQLINTEGER go_id;
	SQLINTEGER pmid;
	Char evidence[50]; /* do not use pointers here, since pointers can't use sizeof() */
}ll_go;

/* for table ll_cdd */
static struct
{
	SQLINTEGER ll_id;
	Char cdd_id[50];
	SQLDOUBLE evalue;
}ll_cdd;

/* for table ll_llink */
static struct
{
	SQLINTEGER ll_id;
	SQLINTEGER gi;
	Char map[MAXCHARLEN];
}ll_link;

/* for table ll_omim */
static struct
{
	SQLINTEGER ll_id;
	SQLINTEGER omim_id;
}ll_omim;

/**************************************************
 *       PUBLIC DATABASE INITIALIZTION           *
 **************************************************/

/*
** forms a connection to the database and set up handles for the
** locus link inserts.
**
** returns TRUE if connection is successful, FALSE otherwise
*/
Boolean
LL_InitCodeBase(Boolean bConnect)
{
   if (bConnect) {
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
      ErrPostEx(SEV_ERROR, 0, 0, "[LL_InitCodeBase]: Could not find username in initialization file.");
      return FALSE;
   }
   if( GetAppParam("intrez", "datab", "password", NULL, (Char*)passwd, sizeof(char) * 10) <= 0 )
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[LL_InitCodeBase]: Could not find pasword in initialization file.");
      return FALSE;
   }
   if( GetAppParam("intrez", "datab", "dsn", NULL, (Char*)dsn, sizeof(char) * 10) <= 0 )
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[LL_InitCodeBase]: Could not find dsn in initialization file.");
      return FALSE;
   }

   /* database is a global parameter, we will use it when we initialize the sql statements,
      this way, we don't hardcode the database into the source code, allowing us to
      change the instance of the database by specifying it in the .intrezrc file, this
      is illustrated in each of the InitXXX functions below. */
   if( GetAppParam("intrez", "datab", "database", NULL, (Char*)database, sizeof(char) * 10) <= 0 )
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[LL_InitCodeBase]: Could not find database in initialization file.");
      return FALSE;
   }
   /* make sure the proper DB2 instance variable is defined--only for aix system */
   /* sprintf(instancebuf,"DB2INSTANCE=db2inst1",instance);
   putenv(instancebuf); */

   /* 1. allocate Environment handle and register version */
   V_OD_erg = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);
   if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO))
   {
      ErrPostEx(SEV_ERROR,0,0, "[LL_InitCodeBase]: Unable to allocate environment handle. SQLRETURN=%ld", V_OD_erg);
      exit(1);
   }

   /* 2. Declaring ODBC Version only for 3.x */
   V_OD_erg = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0);
   if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO))
   {
      ErrPostEx(SEV_ERROR,0,0, "[LL_InitCodeBase]: Unable to setup ODBC Verison to 3.0. SQLRETURN=%ld", V_OD_erg);
      exit(1);
   }

   /* 3. allocate connection handle. */
   V_OD_erg = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);
   if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO))
   {
      ErrPostEx(SEV_ERROR,0,0, "[LL_InitCodeBase]: Cannot allocate connection handle. SQLRETURN=%ld", V_OD_erg);
      SQLFreeHandle(SQL_HANDLE_ENV, henv);
      exit(1);
   }

   /* 4. Set Connection Attributes: Set timeout. set autocommit off */
   V_OD_erg = SQLSetConnectAttr(hdbc, SQL_LOGIN_TIMEOUT, (SQLPOINTER *)5, 0);
   if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO))
   {
      ErrPostEx(SEV_ERROR,0,0, "[LL_InitCodeBase]: Cannot set connect attr. SQLRETURN=%ld", V_OD_erg);
      SQLFreeHandle(SQL_HANDLE_ENV, henv);
      exit(1);
   }

   /* 5. Connect to the datasource */
   V_OD_erg = SQLConnect(hdbc, dsn, SQL_NTS, usr, SQL_NTS, passwd, SQL_NTS);
   if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO))
   {
      ErrPostEx(SEV_ERROR,0,0, "[LL_InitCodeBase]: Connection to database failed. SQLRETURN=%ld", V_OD_erg);
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
         ErrPostEx(SEV_ERROR,0,0, "[LL_InitCodeBase]: failed to set autocommit off. SQLRETURN=%ld", V_OD_erg);
         SQLFreeHandle(SQL_HANDLE_ENV, henv);
         exit(1);
      }
	}
   } /* if (bAlloc) */

   if (InitBindParameter() != TRUE)
   {
      ErrPostEx(SEV_ERROR,0,0, "[LL_InitCodeBase]: Cannot InitBindParameterforInsert. SQLRETURN=%ld", V_OD_erg);
      exit(1);
   }

	return TRUE;
}


static Boolean
InitBindParameter()
{

   CharPtr pSQL_stmt = (char *) malloc(sizeof(Char) * 150);
   if (pSQL_stmt == NULL)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[InitBindParameterforInsert]: Could not allocate space");
     free(pSQL_stmt);
     return FALSE;
   }

   /* table ll_omim */
   sprintf((char *)pSQL_stmt, "INSERT IGNORE into %s.ll_omim(ll_id,omim_id) values(?,?)", database);
   V_OD_erg = CreateHandle(&hstmt_llomim,pSQL_stmt,hdbc,&henv);
   if (V_OD_erg != SQL_SUCCESS) {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitBindParameterforInsert]: Error CreateHandle for ll_omim");
	  return FALSE;
   }

   V_OD_erg = SQLBindParameter(hstmt_llomim, 1, SQL_PARAM_INPUT,
	           SQL_C_LONG,SQL_INTEGER,
			   0, 0, &ll_omim.ll_id, 0, NULL);
   if (V_OD_erg != SQL_SUCCESS) {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitBindParameterforInsert]: Error bind parameter 1 for ll_omim. SQLRETURN=%ld", V_OD_erg);
	  return FALSE;
   }
   V_OD_erg = SQLBindParameter(hstmt_llomim, 2, SQL_PARAM_INPUT,
	            SQL_C_LONG,SQL_INTEGER,
				0, 0, &ll_omim.omim_id, 0, NULL);
   if (V_OD_erg != SQL_SUCCESS) {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitBindParameterforInsert]: Error unable to bind parameter 2 for ll_omim. SQLRETURN=%ld", V_OD_erg);
	  return FALSE;
   }


   /* table ll_link */
   sprintf((char *)pSQL_stmt, "INSERT IGNORE into %s.ll_llink(ll_id, gi, map) values(?,?,?)", database);
   V_OD_erg = CreateHandle(&hstmt_lllink,pSQL_stmt,hdbc,&henv);
   if (V_OD_erg != SQL_SUCCESS) {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitBindParameterforInsert]: Error CreateHandle for ll_link");
	  return FALSE;
   }

	V_OD_erg = SQLBindParameter(hstmt_lllink, 1, SQL_PARAM_INPUT,
					SQL_C_LONG,SQL_INTEGER,
					0, 0, &ll_link.ll_id, 0, NULL);
   if (V_OD_erg != SQL_SUCCESS) {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitBindParameterforInsert]: Error unable to bind parameter 1 for ll_link. SQLRETURN=%ld", V_OD_erg);
	  return FALSE;
   }
	V_OD_erg = SQLBindParameter(hstmt_lllink, 2, SQL_PARAM_INPUT,
					SQL_C_LONG,SQL_INTEGER,
					0, 0, &ll_link.gi, 0, NULL);
   if (V_OD_erg != SQL_SUCCESS) {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitBindParameterforInsert]: Error unable to bind parameter 2 for ll_link. SQLRETURN=%ld", V_OD_erg);
	  return FALSE;
   }
	V_OD_erg = SQLBindParameter(hstmt_lllink, 3, SQL_PARAM_INPUT,
					SQL_C_CHAR,SQL_VARCHAR,
					0, 0, ll_link.map, sizeof(ll_link.map), NULL);
    if (V_OD_erg != SQL_SUCCESS) {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitBindParameterforInsert]: Error unable to bind parameter 3 for ll_link. SQLRETURN=%ld", V_OD_erg);
	  return FALSE;
   }

	/* table ll_cdd */
   sprintf((char *)pSQL_stmt, "INSERT IGNORE INTO %s.ll_cdd(ll_id, cdd_id, evalue) VALUES(?,?,?)", database);
   V_OD_erg = CreateHandle(&hstmt_llcdd,pSQL_stmt,hdbc,&henv);
   if (V_OD_erg != SQL_SUCCESS) {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitBindParameterforInsert]: Error CreateHandle for ll_cdd");
	  return FALSE;
   }

	V_OD_erg = SQLBindParameter(hstmt_llcdd, 1, SQL_PARAM_INPUT,
					SQL_C_LONG,SQL_INTEGER,
					0, 0, &ll_cdd.ll_id, 0, NULL);
    if (V_OD_erg != SQL_SUCCESS) {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitBindParameterforInsert]: Error unable to bind parameter 1 for ll_cdd. SQLRETURN=%ld", V_OD_erg);
	  return FALSE;
   }
	V_OD_erg = SQLBindParameter(hstmt_llcdd, 2, SQL_PARAM_INPUT,
					SQL_C_CHAR,SQL_VARCHAR,
					0, 0, &ll_cdd.cdd_id, sizeof(ll_cdd.cdd_id), NULL);
    if (V_OD_erg != SQL_SUCCESS) {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitBindParameterforInsert]: Error unable to bind parameter 2 for ll_cdd. SQLRETURN=%ld", V_OD_erg);
	  return FALSE;
   }
	V_OD_erg = SQLBindParameter(hstmt_llcdd, 3, SQL_PARAM_INPUT,
					SQL_C_DOUBLE,SQL_DOUBLE,
					0, 0, &ll_cdd.evalue, 0, NULL);
    if (V_OD_erg != SQL_SUCCESS) {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitBindParameterforInsert]: Error unable to bind parameter 3 for ll_cdd. SQLRETURN=%ld", V_OD_erg);
	  return FALSE;
   }

   /* table ll_go */
   sprintf((char *)pSQL_stmt, "INSERT IGNORE into %s.ll_go(ll_id, go_id,pmid, evidence) values(?,?,?,?)", database);
   V_OD_erg = CreateHandle(&hstmt_llgo,pSQL_stmt,hdbc,&henv);
   if (V_OD_erg != SQL_SUCCESS) {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitBindParameterforInsert]: Error CreateHandle for ll_go");
	  return FALSE;
   }

	V_OD_erg = SQLBindParameter(hstmt_llgo, 1, SQL_PARAM_INPUT,
					 SQL_C_LONG,SQL_INTEGER,
					 0, 0, &ll_go.ll_id, 0, NULL);
    if (V_OD_erg != SQL_SUCCESS) {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitBindParameterforInsert]: Error unable to bind parameter 1 for ll_go. SQLRETURN=%ld", V_OD_erg);
	  return FALSE;
    }
	V_OD_erg = SQLBindParameter(hstmt_llgo, 2, SQL_PARAM_INPUT,
					SQL_C_LONG,SQL_INTEGER,
					0, 0, &ll_go.go_id, 0, NULL);
    if (V_OD_erg != SQL_SUCCESS) {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitBindParameterforInsert]: Error unable to bind parameter 2 for ll_go. SQLRETURN=%ld", V_OD_erg);
	  return FALSE;
    }
	V_OD_erg = SQLBindParameter(hstmt_llgo, 3, SQL_PARAM_INPUT,
					SQL_C_LONG,SQL_INTEGER,
					0, 0, &ll_go.pmid, 0, NULL);
    if (V_OD_erg != SQL_SUCCESS) {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitBindParameterforInsert]: Error unable to bind parameter 3 for ll_go. SQLRETURN=%ld", V_OD_erg);
	  return FALSE;
    }
	V_OD_erg = SQLBindParameter(hstmt_llgo, 4, SQL_PARAM_INPUT,
					SQL_C_CHAR,SQL_VARCHAR,
					0, 0, ll_go.evidence, sizeof(ll_go.evidence), NULL);
    if (V_OD_erg != SQL_SUCCESS) {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitBindParameter]: Error unable to bind parameter 4 for ll_go. SQLRETURN=%ld", V_OD_erg);
	  return FALSE;
    }

   /* search table ll_llink on ll_id*/
   sprintf((char *)pSQL_stmt, "select ll_id, gi, map from %s.ll_llink where ll_id = ?", database);
   V_OD_erg = CreateHandle(&hstmt_lllink_llid,pSQL_stmt,hdbc,&henv);
   if (V_OD_erg != SQL_SUCCESS) {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitBindParameter]: Error CreateHandle for hstmt_lllink_llid");
	  return FALSE;
   }

	V_OD_erg = SQLBindParameter(hstmt_lllink_llid, 1, SQL_PARAM_INPUT,
					 SQL_C_LONG,SQL_INTEGER,
					 0, 0, &ll_link.ll_id, 0, NULL);
    if (V_OD_erg != SQL_SUCCESS) {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitBindParameter]: Error unable to bind parameter 1 for hstmt_lllink_llid. SQLRETURN=%ld", V_OD_erg);
	  return FALSE;
    }

   /* search table ll_llink on gi*/
   sprintf((char *)pSQL_stmt, "select ll_id, gi, map from %s.ll_llink where gi = ?", database);
   V_OD_erg = CreateHandle(&hstmt_lllink_gi,pSQL_stmt,hdbc,&henv);
   if (V_OD_erg != SQL_SUCCESS) {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitBindParameter]: Error CreateHandle for hstmt_lllink_gi");
	  return FALSE;
   }

	V_OD_erg = SQLBindParameter(hstmt_lllink_gi, 1, SQL_PARAM_INPUT,
					 SQL_C_LONG,SQL_INTEGER,
					 0, 0, &ll_link.gi, 0, NULL);
    if (V_OD_erg != SQL_SUCCESS) {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitBindParameter]: Error unable to bind parameter 1 for hstmt_lllink_gi. SQLRETURN=%ld", V_OD_erg);
	  return FALSE;
    }

   /* search table ll_go on ll_id*/
   sprintf((char *)pSQL_stmt, "select ll_id,go_id,pmid,evidence from %s.ll_go where ll_id=?",  database);
   V_OD_erg = CreateHandle(&hstmt_llgo_llid,pSQL_stmt,hdbc,&henv);
   if (V_OD_erg != SQL_SUCCESS) {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitBindParameter]: Error CreateHandle for hstmt_llgo_llid");
	  return FALSE;
   }

	V_OD_erg = SQLBindParameter(hstmt_llgo_llid, 1, SQL_PARAM_INPUT,
					 SQL_C_LONG,SQL_INTEGER,
					 0, 0, &ll_go.ll_id, 0, NULL);
    if (V_OD_erg != SQL_SUCCESS) {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitBindParameter]: Error unable to bind parameter 1 for hstmt_llgo_llid. SQLRETURN=%ld", V_OD_erg);
	  return FALSE;
    }

   /* search table ll_go on go_id*/
   sprintf((char *)pSQL_stmt, "select ll_id,go_id,pmid,evidence from %s.ll_go where go_id=?",  database);
   V_OD_erg = CreateHandle(&hstmt_llgo_goid,pSQL_stmt,hdbc,&henv);
   if (V_OD_erg != SQL_SUCCESS) {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitBindParameter]: Error CreateHandle for hstmt_llgo_goid");
	  return FALSE;
   }

	V_OD_erg = SQLBindParameter(hstmt_llgo_goid, 1, SQL_PARAM_INPUT,
					 SQL_C_LONG,SQL_INTEGER,
					 0, 0, &ll_go.go_id, 0, NULL);
    if (V_OD_erg != SQL_SUCCESS) {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitBindParameter]: Error unable to bind parameter 1 for hstmt_llgo_goid. SQLRETURN=%ld", V_OD_erg);
	  return FALSE;
    }

   /* search table ll_go on pmid*/
   sprintf((char *)pSQL_stmt, "select ll_id,go_id,pmid,evidence from %s.ll_go where pmid=?",  database);
   V_OD_erg = CreateHandle(&hstmt_llgo_pmid,pSQL_stmt,hdbc,&henv);
   if (V_OD_erg != SQL_SUCCESS) {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitBindParameter]: Error CreateHandle for hstmt_llgo_pmid");
	  return FALSE;
   }

	V_OD_erg = SQLBindParameter(hstmt_llgo_pmid, 1, SQL_PARAM_INPUT,
					 SQL_C_LONG,SQL_INTEGER,
					 0, 0, &ll_go.pmid, 0, NULL);
    if (V_OD_erg != SQL_SUCCESS) {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitBindParameter]: Error unable to bind parameter 1 for hstmt_llgo_pmid. SQLRETURN=%ld", V_OD_erg);
	  return FALSE;
    }

	free(pSQL_stmt);
	return TRUE;
}

/*
** closes the databases and free the environment handles.
**
**  This function must called at the
**   end of your function, if you called LL_InitCodeBase.
**
** returns TRUE if disconnection is successful, FALSE otherwise
**
*/
Boolean
LL_InitUndoCodeBase(Boolean bConnect)
{

	/* free the statment handle */
	SQLFreeStmt(hstmt_llgo, SQL_RESET_PARAMS);
	SQLFreeStmt(hstmt_lllink, SQL_RESET_PARAMS);
	SQLFreeStmt(hstmt_llomim, SQL_RESET_PARAMS);
	SQLFreeStmt(hstmt_llcdd, SQL_RESET_PARAMS);
	SQLFreeStmt(hstmt_llgo_llid, SQL_RESET_PARAMS);
	SQLFreeStmt(hstmt_llgo_goid, SQL_RESET_PARAMS);
	SQLFreeStmt(hstmt_llgo_pmid, SQL_RESET_PARAMS);
	SQLFreeStmt(hstmt_lllink_llid, SQL_RESET_PARAMS);
	SQLFreeStmt(hstmt_lllink_gi, SQL_RESET_PARAMS);

        if (bConnect) {

	/*Free the database handles used to communicate with the database*/
	V_OD_erg = SQLDisconnect(hdbc);
    if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO))
    {
       ErrPostEx(SEV_ERROR,0,0, "[LL_InitUndoCodeBase]: failed to disconnect. SQLRETURN=%ld", V_OD_erg);
       return FALSE;
    }

	/*free the connnection handle */
	V_OD_erg= SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
    if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO))
    {
       ErrPostEx(SEV_ERROR,0,0, "[LL_InitUndoCodeBase]: failed to free connection handle. SQLRETURN=%ld", V_OD_erg);
       return FALSE;
    }

	/*free envirnment handle */
	V_OD_erg = SQLFreeHandle(SQL_HANDLE_ENV, henv);
    if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO))
    {
       ErrPostEx(SEV_ERROR,0,0, "[LL_InitUndoCodeBase]: failed to free environment handle. SQLRETURN=%ld", V_OD_erg);
       return FALSE;
	}
        } /* if (bConnect) */
	return TRUE;
}


/****************************************************************/
/* Given an SQL handle, set AutoCommit off                      */
/****************************************************************/
Boolean LL_StartTransaction(void)
{
	return SetAutoCommitOff(hdbc);
}

/****************************************************************/
/* Given an SQL handle, commit                                  */
/****************************************************************/
Boolean LL_CommitTransaction(void)
{
	return CommitChanges(hdbc);
}

/****************************************************************/
/* Given an SQL handle, roll back                               */
/****************************************************************/
Boolean LL_RollBackTransaction(void)
{
	return RollBackChanges(hdbc);
}



/********************************************************
             PUBLIC OPEN DATABASE FUNCTIONS
*********************************************************/

/* opens all the databases associated with Locus Link.  This
   function must be called prior to accessing the database.*/
Boolean LL_OpenAllDBs(Boolean Create)
{
  return TRUE;
}

void CloseAllLLDBs(void)
{
  LL_InitUndoCodeBase(FALSE);
}

/***********************************************************
    Stub functions that mirror codebase functions.
  **********************************************************/
Boolean OpenLL_GONoidx(CharPtr name)
{
  return TRUE;
}
Boolean ReindexLL_GO(void)
{
  return TRUE;
}
Boolean CloseLL_GO(void)
{
  return TRUE;
}

/*********************************************************************
                    PRIVATE SEARCH FUNCTIONS
   THESE FUNCTIONS ARE NOT INTENDED TO BY CALLED BY THE END USER.
    SEE QUERY.H/QUERY.C FOR THE PUBLIC SHOUND SEARCH FUNCTIONS
**********************************************************************/
/**
** searches the LL_GODB for a key specified in ppgo.
**
** param	ppgo
** return	If such a key is found, fills in ppgo with the relevant data,
**		and returns LL_SUCCESS.  If an error occurs, return LL_ERROR,
**		if the key is not found returns LL_NOTFOUND.
*/
Int4 LL_SearchGODB(StGOdbPtr PNTR ppgo)
{
	StGOdbPtr pgo = NULL;
  	SQLINTEGER key = 0;
	SQLINTEGER llid=0, goid=0, pmid=0;
	Char evi[MAXCHARLEN];
	SQLINTEGER evilen=0;
	SQLINTEGER fetchCount=0;


  	if(ppgo == NULL){
    		ErrPostEx(SEV_ERROR,0,0, "[LL_SearchGODB]: StGOdbPtr is NULL.");
    		return LL_ERROR;
  	}

  	pgo = *ppgo;

  	/* search on ll_id */
  	if(pgo->llid != 0){
		ll_go.ll_id = pgo->llid;

		/* execute statment */
  		V_OD_erg = SQLExecute(hstmt_llgo_llid);
		if (V_OD_erg != SQL_SUCCESS)
		{
		   ErrPostEx(SEV_ERROR,0,0, "[LL_SearchGODB]: Unable to execute select statement.");
		   LogSQLInfo(hstmt_llgo_llid);
		   SQLFreeStmt(hstmt_llgo_llid, SQL_CLOSE);
		   return LL_ERROR;
		}

		V_OD_erg = SQLBindCol(hstmt_llgo_llid,1,SQL_C_LONG,&llid, 0, NULL);
		if (V_OD_erg != SQL_SUCCESS) {
			ErrPostEx(SEV_ERROR, 0, 0, "[LL_SearchGODB]: unable to bind col 1 for table ll_go. llid=%ld. SQLRETURN=%ld",
				   ll_go.ll_id, V_OD_erg);
			SQLFreeStmt(hstmt_llgo_llid, SQL_CLOSE);
			return LL_ERROR;
		}
		V_OD_erg = SQLBindCol(hstmt_llgo_llid,2,SQL_C_LONG,&goid, 0, NULL);
		if (V_OD_erg != SQL_SUCCESS) {
			ErrPostEx(SEV_ERROR, 0, 0, "[LL_SearchGODB]: unable to bind col 2 for table ll_go. llid=%ld. SQLRETURN=%ld",
				   ll_go.ll_id, V_OD_erg);
			SQLFreeStmt(hstmt_llgo_llid, SQL_CLOSE);
			return LL_ERROR;
		}
		V_OD_erg = SQLBindCol(hstmt_llgo_llid,3,SQL_C_LONG,&pmid, 0, NULL);
		if (V_OD_erg != SQL_SUCCESS) {
			ErrPostEx(SEV_ERROR, 0, 0, "[LL_SearchGODB]: unable to bind col 3 for table ll_go. llid=%ld. SQLRETURN=%ld",
				   ll_go.ll_id, V_OD_erg);
			SQLFreeStmt(hstmt_llgo_llid, SQL_CLOSE);
			return LL_ERROR;
		}
		V_OD_erg = SQLBindCol(hstmt_llgo_llid,4,SQL_C_CHAR, evi, sizeof(evi), (int*)&evilen);
		if (V_OD_erg != SQL_SUCCESS) {
			ErrPostEx(SEV_ERROR, 0, 0, "[LL_SearchGODB]: unable to bind col 4 for table ll_go. llid=%ld. SQLRETURN=%ld",
				   ll_go.ll_id, V_OD_erg);
			SQLFreeStmt(hstmt_llgo_llid, SQL_CLOSE);
			return LL_ERROR;
		}

		while((V_OD_erg=SQLFetch(hstmt_llgo_llid)) == SQL_SUCCESS || V_OD_erg == SQL_SUCCESS_WITH_INFO)
		{
			fetchCount++;
			if (fetchCount>1) {
				pgo->next = StGOdbNodeNew();
				pgo = pgo->next;
			}

			pgo->llid = llid;
			pgo->goid = goid;
			if (pmid == 1000)
				pgo->pmid = -1;
			else
				pgo->pmid = pmid;

			pgo->eviCode = StringSave(evi);
		}

	   /* close the cursor and discard all pending results. */
	   /* must do in order to re-use the handle created in Init */
		SQLFreeStmt(hstmt_llgo_llid, SQL_CLOSE);
  	}


  	/* search on go_id */
  	else if(pgo->goid > 0){
		ll_go.go_id = pgo->goid;

		/* execute statment */
  		V_OD_erg = SQLExecute(hstmt_llgo_goid);
		if (V_OD_erg != SQL_SUCCESS)
		{
		   ErrPostEx(SEV_ERROR,0,0, "[LL_SearchGODB]: Unable to execute select statement.");
		   LogSQLInfo(hstmt_llgo_goid);
		   SQLFreeStmt(hstmt_llgo_goid, SQL_CLOSE);
		   return LL_ERROR;
		}

		V_OD_erg = SQLBindCol(hstmt_llgo_goid,1,SQL_C_LONG,&llid, 0, NULL);
		if (V_OD_erg != SQL_SUCCESS) {
			ErrPostEx(SEV_ERROR, 0, 0, "[LL_SearchGODB]: unable to bind col 1 for table ll_go. goid=%ld. SQLRETURN=%ld",
				   ll_go.go_id, V_OD_erg);
			SQLFreeStmt(hstmt_llgo_goid, SQL_CLOSE);
			return LL_ERROR;
		}
		V_OD_erg = SQLBindCol(hstmt_llgo_goid,2,SQL_C_LONG,&goid, 0, NULL);
		if (V_OD_erg != SQL_SUCCESS) {
			ErrPostEx(SEV_ERROR, 0, 0, "[LL_SearchGODB]: unable to bind col 2 for table ll_go. goid=%ld. SQLRETURN=%ld",
				   ll_go.go_id, V_OD_erg);
			SQLFreeStmt(hstmt_llgo_goid, SQL_CLOSE);
			return LL_ERROR;
		}
		V_OD_erg = SQLBindCol(hstmt_llgo_goid,3,SQL_C_LONG,&pmid, 0, NULL);
		if (V_OD_erg != SQL_SUCCESS) {
			ErrPostEx(SEV_ERROR, 0, 0, "[LL_SearchGODB]: unable to bind col 3 for table ll_go. goid=%ld. SQLRETURN=%ld",
				   ll_go.go_id, V_OD_erg);
			SQLFreeStmt(hstmt_llgo_goid, SQL_CLOSE);
			return LL_ERROR;
		}
		V_OD_erg = SQLBindCol(hstmt_llgo_goid,4,SQL_C_CHAR, evi, sizeof(evi), (int*)&evilen);
		if (V_OD_erg != SQL_SUCCESS) {
			ErrPostEx(SEV_ERROR, 0, 0, "[LL_SearchGODB]: unable to bind col 4 for table ll_go. goid=%ld. SQLRETURN=%ld",
				   ll_go.go_id, V_OD_erg);
			SQLFreeStmt(hstmt_llgo_goid, SQL_CLOSE);
			return LL_ERROR;
		}

		while((V_OD_erg=SQLFetch(hstmt_llgo_goid)) == SQL_SUCCESS || V_OD_erg == SQL_SUCCESS_WITH_INFO)
		{
			fetchCount++;
			if (fetchCount>1) {
				pgo->next = StGOdbNodeNew();
				pgo = pgo->next;
			}

			pgo->llid = llid;
			pgo->goid = goid;
			if (pmid == 1000)
				pgo->pmid = -1;
			else
				pgo->pmid = pmid;
			pgo->eviCode = StringSave(evi);
		}

	   /* close the cursor and discard all pending results. */
	   /* must do in order to re-use the handle created in Init */
		SQLFreeStmt(hstmt_llgo_goid, SQL_CLOSE);
  	}

  	/* search on pmid */
  	else if(pgo->pmid > 0){
		ll_go.pmid = pgo->pmid;
		/* execute statment */
  		V_OD_erg = SQLExecute(hstmt_llgo_pmid);
		if (V_OD_erg != SQL_SUCCESS)
		{
		   ErrPostEx(SEV_ERROR,0,0, "[LL_SearchGODB]: Unable to execute select statement.");
		   LogSQLInfo(hstmt_llgo_pmid);
		   SQLFreeStmt(hstmt_llgo_pmid, SQL_CLOSE);
		   return LL_ERROR;
		}

		V_OD_erg = SQLBindCol(hstmt_llgo_pmid,1,SQL_C_LONG,&llid, 0, NULL);
		if (V_OD_erg != SQL_SUCCESS) {
			ErrPostEx(SEV_ERROR, 0, 0, "[LL_SearchGODB]: unable to bind col 1 for table ll_go. pmid=%ld. SQLRETURN=%ld",
				   ll_go.pmid, V_OD_erg);
			SQLFreeStmt(hstmt_llgo_pmid, SQL_CLOSE);
			return LL_ERROR;
		}
		V_OD_erg = SQLBindCol(hstmt_llgo_pmid,2,SQL_C_LONG,&goid, 0, NULL);
		if (V_OD_erg != SQL_SUCCESS) {
			ErrPostEx(SEV_ERROR, 0, 0, "[LL_SearchGODB]: unable to bind col 2 for table ll_go. pmid=%ld. SQLRETURN=%ld",
				   ll_go.pmid, V_OD_erg);
			SQLFreeStmt(hstmt_llgo_pmid, SQL_CLOSE);
			return LL_ERROR;
		}
		V_OD_erg = SQLBindCol(hstmt_llgo_pmid,3,SQL_C_LONG,&pmid, 0, NULL);
		if (V_OD_erg != SQL_SUCCESS) {
			ErrPostEx(SEV_ERROR, 0, 0, "[LL_SearchGODB]: unable to bind col 3 for table ll_go. pmid=%ld. SQLRETURN=%ld",
				   ll_go.pmid, V_OD_erg);
			SQLFreeStmt(hstmt_llgo_pmid, SQL_CLOSE);
			return LL_ERROR;
		}
		V_OD_erg = SQLBindCol(hstmt_llgo_pmid,4,SQL_C_CHAR, evi, sizeof(evi), (int*)&evilen);
		if (V_OD_erg != SQL_SUCCESS) {
			ErrPostEx(SEV_ERROR, 0, 0, "[LL_SearchGODB]: unable to bind col 4 for table ll_go. pmid=%ld. SQLRETURN=%ld",
				   ll_go.pmid, V_OD_erg);
			SQLFreeStmt(hstmt_llgo_pmid, SQL_CLOSE);
			return LL_ERROR;
		}


		while((V_OD_erg=SQLFetch(hstmt_llgo_pmid)) == SQL_SUCCESS || V_OD_erg == SQL_SUCCESS_WITH_INFO)
		{
			fetchCount++;
			if (fetchCount>1) {
				pgo->next = StGOdbNodeNew();
				pgo = pgo->next;
			}

			pgo->llid = llid;
			pgo->goid = goid;
			if (pmid == 1000)
				pgo->pmid = -1;
			else
				pgo->pmid = pmid;
			pgo->eviCode = StringSave(evi);
		}

	   /* close the cursor and discard all pending results. */
	   /* must do in order to re-use the handle created in Init */
		SQLFreeStmt(hstmt_llgo_pmid, SQL_CLOSE);

	} else {
       ErrPostEx(SEV_ERROR, 0, 0, "[LL_SearchGODB]: No search key set.");
       return LL_ERROR;
	}

   /* should NOT free it */
	/* pgo = StGOdbFree(pgo); */
	if (fetchCount <= 0)
		return LL_NOTFOUND;
  	return LL_SUCCESS;

}

/**
** searches for the key stored in ppomim in the LL_OMIM db.
**
** return   	If an error occurs, return LL_ERROR. If the key is not found in the db,
**   		returns LL_NOTFOUND, otherwise, fills in ppomim with the
**		necessary field data and returns LL_SUCCESS.
*/
Int4
LL_SearchOMIMDB(StOMIMdbPtr PNTR ppomim)
{
  	StOMIMdbPtr pomim = NULL;

	SQLINTEGER llid=0, omimid = 0;

  	SQLINTEGER key = 0;
  	SQLINTEGER fetchCount=0;

  	CharPtr search_stmt = NULL;
	Char ll_stmt[200];
	Char omim_stmt[200];

	sprintf(ll_stmt, "select ll_id,omim_id from %s.ll_omim where ll_id = ?", database);
	sprintf(omim_stmt, "select ll_id,omim_id from %s.ll_omim where omim_id = ?", database);

	/* SK: NULL ptr check */
  	if(ppomim == NULL || *ppomim == NULL)
	{
    		ErrPostEx(SEV_ERROR,0,0, "[LL_SearchOMIMDB]: StOMIMdbPtr is NULL.");
    		return LL_ERROR;
  	}

  	pomim = *ppomim;
  	if(pomim->llid != 0){
    		key = pomim->llid;
    		search_stmt = ll_stmt;
  	} else if(pomim->omimid > 0){
    		key = pomim->omimid;
    		search_stmt = omim_stmt;
  	} else {
       ErrPostEx(SEV_ERROR, 0, 0, "[LL_SearchOMIMDB]: No search key set.");
       return LL_ERROR;
	}


  	/* search omimDB*/
   	V_OD_erg = CreateHandle(&V_OD_hstmt,search_stmt,hdbc,&henv);
 	if (V_OD_erg != SQL_SUCCESS) {
       ErrPostEx(SEV_ERROR, 0, 0, "[LL_SearchOMIMDB]: Error CreateHandle for LL_SearchOMIMDB");
	   return LL_ERROR;
    }

	V_OD_erg = SQLBindParameter(V_OD_hstmt, 1, SQL_PARAM_INPUT,
				SQL_C_LONG,SQL_INTEGER, 0, 0, &key, 0, NULL);
	if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO))
	{
	   ErrPostEx(SEV_ERROR,0,0, "[LL_SearchOMIMDB]: unable to bind parameter 1 for table ll_omim. key=%ld. SQLRETURN=%ld",
			   key, V_OD_erg);

	   SQLFreeHandle(SQL_HANDLE_STMT, V_OD_hstmt);
	   return LL_ERROR;
	}

  	/* execute statment */
  	V_OD_erg = SQLExecute(V_OD_hstmt);
	if (V_OD_erg != SQL_SUCCESS)
	{
	   ErrPostEx(SEV_ERROR,0,0, "[LL_SearchOMIMDB]: Unable to execute select statement.");
	   LogSQLInfo(V_OD_hstmt);
	   SQLFreeHandle(SQL_HANDLE_STMT, V_OD_hstmt);
	   return LL_ERROR;
	}


  	V_OD_erg = SQLBindCol(V_OD_hstmt,1,SQL_C_LONG,&llid, 0, NULL);
	if (V_OD_erg != SQL_SUCCESS) {
		ErrPostEx(SEV_ERROR, 0, 0, "[LL_SearchOMIMDB]: unable to bind col 1 for table ll_omim. key=%ld. SQLRETURN=%ld",
			   key, V_OD_erg);
	    SQLFreeHandle(SQL_HANDLE_STMT, V_OD_hstmt);
		return LL_ERROR;
	}

	SQLBindCol(V_OD_hstmt,2,SQL_C_LONG,&omimid, 0, NULL);
	if (V_OD_erg != SQL_SUCCESS) {
		ErrPostEx(SEV_ERROR, 0, 0, "[LL_SearchOMIMDB]: unable to bind col 2 for table ll_omim. key=%ld. SQLRETURN=%ld",
			   key, V_OD_erg);
	    SQLFreeHandle(SQL_HANDLE_STMT, V_OD_hstmt);
		return LL_ERROR;
	}

  	while((V_OD_erg=SQLFetch(V_OD_hstmt)) == SQL_SUCCESS || V_OD_erg == SQL_SUCCESS_WITH_INFO)
  	{

		fetchCount++;
		if (fetchCount>1)
		{
		   pomim->next = StOMIMdbNodeNew();
		   pomim = pomim->next;
		}
		pomim->llid = llid;
		pomim->omimid = omimid;

  	}

	/* should NOT free */
	/* pomim=StOMIMdbFree(pomim); */
	if (fetchCount <= 0)
		return LL_NOTFOUND;
 	return LL_SUCCESS;

}

/**
** searches for the key stored in ppomim in the LL_OMIM db.
**
** return   	If an error occurs, return LL_ERROR. If the key is not found in the db,
**   		returns LL_NOTFOUND, otherwise, fills in ppomim with the
**   		necessary field data and returns LL_SUCCESS.
*/
Int4
LL_SearchCDDDB(StCDDdbPtr PNTR ppcdd)
{
  	StCDDdbPtr pcdd = NULL;
  	SQLINTEGER key = 0;
  	Char szKey[20];

	SQLINTEGER llid=0;
	FloatHi evalue;
	Char cdd[MAXCHARLEN];
	SQLINTEGER cddlen=0;
	SQLINTEGER fetchCount=0;

  	CharPtr search_stmt = NULL;
	Char ll_stmt[200];
	Char cddid_stmt[200];

	sprintf(ll_stmt, "select ll_id,cdd_id,evalue from %s.ll_cdd where ll_id = ?", database);
	sprintf(cddid_stmt, "select ll_id,cdd_id,evalue from %s.ll_cdd where cdd_id = ?", database);

	/* SK: NULL ptr check */
  	if(ppcdd == NULL || *ppcdd == NULL)
	{
    		ErrPostEx(SEV_ERROR,0,0, "[LL_SearchCDDDB]: StCDDdbPtr is NULL.");
    		return LL_ERROR;
  	}

  	pcdd = *ppcdd;
  	if(pcdd->llid != 0){
    		key = pcdd->llid;
    		search_stmt = ll_stmt;
  	} else if(pcdd->cddid != NULL)
        { 
          if (StrLen(pcdd->cddid) > 0)
          {
    		sprintf(szKey,"%s",IsNullStr(pcdd->cddid));
    		search_stmt = cddid_stmt;
          }
	} else {
       ErrPostEx(SEV_ERROR, 0, 0, "[LL_SearchCDDDB]: No search key set.");
       return LL_ERROR;
	}

	/* search cddDB*/
   	V_OD_erg = CreateHandle(&V_OD_hstmt,search_stmt,hdbc,&henv);
 	if (V_OD_erg != SQL_SUCCESS) {
       ErrPostEx(SEV_ERROR, 0, 0, "[LL_SearchCDDDB]: Error CreateHandle");
	   return LL_ERROR;
    }

	if(pcdd->llid != 0) {
  		V_OD_erg = SQLBindParameter(V_OD_hstmt, 1, SQL_PARAM_INPUT,
 				SQL_C_LONG,SQL_INTEGER, 0, 0, &key, 0, NULL);

		if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO))
		{
	      ErrPostEx(SEV_ERROR,0,0, "[LL_SearchCDDDB]: unable to bind parameter 1 for table ll_cdd. key=%ld. SQLRETURN=%ld",
			   key, V_OD_erg);

	      SQLFreeHandle(SQL_HANDLE_STMT, V_OD_hstmt);
	      return LL_ERROR;
		}

  	} else {
		V_OD_erg = SQLBindParameter(V_OD_hstmt, 1, SQL_PARAM_INPUT,
				SQL_C_CHAR,SQL_VARCHAR, 0, 0, szKey, sizeof(szKey), NULL);
		if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO))
		{
	      ErrPostEx(SEV_ERROR,0,0, "[LL_SearchCDDDB]: unable to bind parameter 1 for table ll_cdd. key=%s. SQLRETURN=%ld",
			   szKey, V_OD_erg);

	      SQLFreeHandle(SQL_HANDLE_STMT, V_OD_hstmt);
	      return LL_ERROR;
		}
  	}

  	/* execute statment */
  	V_OD_erg = SQLExecute(V_OD_hstmt);
	if (V_OD_erg != SQL_SUCCESS)
	{
	   ErrPostEx(SEV_ERROR,0,0, "[LL_SearchCDDDB]: Unable to execute select statement.");
	   LogSQLInfo(V_OD_hstmt);
	   SQLFreeHandle(SQL_HANDLE_STMT, V_OD_hstmt);
	   return LL_ERROR;
	}

  	SQLBindCol(V_OD_hstmt,1,SQL_C_LONG,&llid,0, NULL);
	if (V_OD_erg != SQL_SUCCESS) {
		ErrPostEx(SEV_ERROR, 0, 0, "[LL_SearchCDDDB]: unable to bind col 1 for table ll_cdd. SQLRETURN=%ld",
			   V_OD_erg);
	    SQLFreeHandle(SQL_HANDLE_STMT, V_OD_hstmt);
		return LL_ERROR;
	}

	SQLBindCol(V_OD_hstmt,2,SQL_C_CHAR,cdd, sizeof(cdd), (int*)&cddlen);
	if (V_OD_erg != SQL_SUCCESS) {
		ErrPostEx(SEV_ERROR, 0, 0, "[LL_SearchCDDDB]: unable to bind col 2 for table ll_cdd. SQLRETURN=%ld",
			   V_OD_erg);
	    SQLFreeHandle(SQL_HANDLE_STMT, V_OD_hstmt);
		return LL_ERROR;
	}

	SQLBindCol(V_OD_hstmt,3,SQL_C_DOUBLE,&evalue,0, NULL);
	if (V_OD_erg != SQL_SUCCESS) {
		ErrPostEx(SEV_ERROR, 0, 0, "[LL_SearchCDDDB]: unable to bind col 3 for table ll_cdd. SQLRETURN=%ld",
			   V_OD_erg);
	    SQLFreeHandle(SQL_HANDLE_STMT, V_OD_hstmt);
		return LL_ERROR;
	}

  	while((V_OD_erg=SQLFetch(V_OD_hstmt)) == SQL_SUCCESS || V_OD_erg == SQL_SUCCESS_WITH_INFO)
  	{

		fetchCount++;
		if (fetchCount>1)
		{
		   pcdd->next = StCDDdbNodeNew();
		   pcdd = pcdd->next;
		}
		pcdd->cddid = StringSave(cdd);
		pcdd->llid = llid;
		pcdd->eValue = evalue;
		if(pcdd->eValue < 7){
			SLRI_SafeExp10(&pcdd->eValue);
		} else {
			pcdd->eValue = -1;
		}

  	}

	if (fetchCount <= 0)
		return LL_NOTFOUND;

 	return LL_SUCCESS;

}

/**
** searches for the key stored in ppll in the LL_LINK db.
**
** return   	If an error occurs, return LL_ERROR. If the key is not found in the db,
**   		returns LL_NOTFOUND, otherwise, fills in ppll with the
**   		necessary field data and returns LL_SUCCESS.
*/
Int4
LL_SearchLLDB(StLLdbPtr PNTR ppll)
{
 	StLLdbPtr pll = NULL;
 	StLLdbPtr prev = NULL;
  	SQLINTEGER key = 0;
	SQLINTEGER gi, ll_id;
	Char map[MAXCHARLEN];
	SQLINTEGER maplen=0;
	SQLINTEGER fetchCount=0;

	if(ppll == NULL){
    		ErrPostEx(SEV_ERROR,0,0, "[LL_SearchLLDB]: StLLdbPtr is NULL.");
    		return LL_ERROR;
  	}

  	pll = *ppll;
  	if(pll->llid != 0){
		ll_link.ll_id = pll->llid;

		V_OD_erg = SQLExecute(hstmt_lllink_llid);
		if (V_OD_erg != SQL_SUCCESS)
		{
		   ErrPostEx(SEV_ERROR,0,0, "[LL_SearchLLDB]: Unable to execute select statement.");
		   LogSQLInfo(hstmt_lllink_llid);
		   SQLFreeStmt(hstmt_lllink_llid, SQL_CLOSE);
		   return LL_ERROR;
		}


		V_OD_erg = SQLBindCol(hstmt_lllink_llid,1,SQL_C_LONG,&ll_id, 0, NULL);
		if (V_OD_erg != SQL_SUCCESS) {
			ErrPostEx(SEV_ERROR, 0, 0, "[LL_SearchLLDB]: unable to bind col 1 for table ll_llink. llid=%ld. SQLRETURN=%ld",
				   ll_link.ll_id, V_OD_erg);
			SQLFreeStmt(hstmt_lllink_llid, SQL_CLOSE);
			return LL_ERROR;
		}
		V_OD_erg = SQLBindCol(hstmt_lllink_llid,2,SQL_C_LONG,&gi, 0, NULL);
		if (V_OD_erg != SQL_SUCCESS) {
			ErrPostEx(SEV_ERROR, 0, 0, "[LL_SearchLLDB]: unable to bind col 2 for table ll_llink. llid=%ld. SQLRETURN=%ld",
				   ll_link.ll_id, V_OD_erg);
			SQLFreeStmt(hstmt_lllink_llid, SQL_CLOSE);
			return LL_ERROR;
		}
		V_OD_erg = SQLBindCol(hstmt_lllink_llid,3,SQL_C_CHAR,&map,sizeof(map), (int*)&maplen);
		if (V_OD_erg != SQL_SUCCESS) {
			ErrPostEx(SEV_ERROR, 0, 0, "[LL_SearchLLDB]: unable to bind col 3 for table ll_llink. llid=%ld. SQLRETURN=%ld",
				   ll_link.ll_id, V_OD_erg);
			SQLFreeStmt(hstmt_lllink_llid, SQL_CLOSE);
			return LL_ERROR;
		}


		while((V_OD_erg=SQLFetch(hstmt_lllink_llid)) == SQL_SUCCESS || V_OD_erg == SQL_SUCCESS_WITH_INFO)
		{
			fetchCount++;
			if (fetchCount>1)
			{
			   pll->next = StLLdbNodeNew();
			   pll = pll->next;
			}

			pll->llid = ll_id;
			pll->npid = gi;
			pll->map = StringSave(map);
		}

		/* close the cursor and discard all pending results. */
		/* must do in order to re-use the handle created in Init */
		SQLFreeStmt(hstmt_lllink_llid, SQL_CLOSE);

  	} else if(pll->npid > 0){
    	ll_link.gi = pll->npid;
		V_OD_erg = SQLExecute(hstmt_lllink_gi);
		if (V_OD_erg != SQL_SUCCESS)
		{
		   ErrPostEx(SEV_ERROR,0,0, "[LL_SearchLLDB]: Unable to execute select statement.");
		   LogSQLInfo(hstmt_lllink_gi);
		   SQLFreeStmt(hstmt_lllink_gi, SQL_CLOSE);
		   return LL_ERROR;
		}


		V_OD_erg = SQLBindCol(hstmt_lllink_gi,1,SQL_C_LONG,&ll_id, 0, NULL);
		if (V_OD_erg != SQL_SUCCESS) {
			ErrPostEx(SEV_ERROR, 0, 0, "[LL_SearchLLDB]: unable to bind col 1 for table ll_llink. gi=%ld. SQLRETURN=%ld",
				   ll_link.gi, V_OD_erg);
			SQLFreeStmt(hstmt_lllink_gi, SQL_CLOSE);
			return LL_ERROR;
		}
		V_OD_erg = SQLBindCol(hstmt_lllink_gi,2,SQL_C_LONG,&gi, 0, NULL);
		if (V_OD_erg != SQL_SUCCESS) {
			ErrPostEx(SEV_ERROR, 0, 0, "[LL_SearchLLDB]: unable to bind col 2 for table ll_llink. gi=%ld. SQLRETURN=%ld",
				   ll_link.gi, V_OD_erg);
			SQLFreeStmt(hstmt_lllink_gi, SQL_CLOSE);
			return LL_ERROR;
		}
		V_OD_erg = SQLBindCol(hstmt_lllink_gi,3,SQL_C_CHAR,&map,sizeof(map), (int*)&maplen);
		if (V_OD_erg != SQL_SUCCESS) {
			ErrPostEx(SEV_ERROR, 0, 0, "[LL_SearchLLDB]: unable to bind col 3 for table ll_llink. gi=%ld. SQLRETURN=%ld",
				   ll_link.gi, V_OD_erg);
			SQLFreeStmt(hstmt_lllink_gi, SQL_CLOSE);
			return LL_ERROR;
		}


		while((V_OD_erg=SQLFetch(hstmt_lllink_gi)) == SQL_SUCCESS || V_OD_erg == SQL_SUCCESS_WITH_INFO)
		{

			fetchCount++;
			if (fetchCount>1)
			{
			   pll->next = StLLdbNodeNew();
			   pll = pll->next;
			}

			pll->llid = ll_id;
			pll->npid = gi;
			pll->map = StringSave(map);
		}

		/* close the cursor and discard all pending results. */
		/* must do in order to re-use the handle created in Init */
		SQLFreeStmt(hstmt_lllink_gi, SQL_CLOSE);
  	}

	/*pll= StLLdbFree(pll);
	if (prev!=NULL) prev->next=NULL;*/

	if (fetchCount <= 0)
		return LL_NOTFOUND;

 	return LL_SUCCESS;

}


/**************************************************************
                 PRIVATE APPENDING FUNCTIONS
***************************************************************/
/**
** Adds a entry consisting of ll_id, goID, PMID and eviCode to
** LL_GO
**
** returns LL_SUCCESS if appending is successful.
*/
Int4
LL_Append2GO_DB(Int4 llid, Int4 goID, Int4 PMID, CharPtr eviCode)
{
   	Char evidence_insert[50];
   	Int4 evidencelen= 0;

	/*if(llid <= 0){
    		ErrPostEx(SEV_ERROR, 0, 0,
			"[LL_Append2GO_DB]: Invalid ll_id on record with PMID: %d.\nRecord skipped.",
			PMID);
    		return LL_ERROR;
  	}*/
  	if(goID <= 0){
    		ErrPostEx(SEV_ERROR, 0, 0,
			"[LL_Append2GO_DB]: Invalid go id on record with ll_ID: %d.\nRecord skipped.",
			llid);
    		return LL_ERROR;
  	}
  	/*if(PMID <= 0){
    		ErrPostEx(SEV_ERROR, 0, 0,
			"[LL_Append2GO_DB]: Invalid pmid on record with ll_ID: %d.\nRecord skipped.",
			llid);
    		return LL_ERROR;
  	}*/
  	if(eviCode == NULL){
    		ErrPostEx(SEV_ERROR, 0, 0,
			"[LL_Append2GO_DB]: Invalid E Code on record with ll_ID: %d.\nRecord skipped.",
			llid);
    		return LL_ERROR;
  	}

	ll_go.ll_id = llid;
	ll_go.go_id = goID;
	ll_go.pmid = PMID;
	sprintf(ll_go.evidence, "%s", IsNullStr(eviCode));

	/* execute statment */
  	V_OD_erg = SQLExecute (hstmt_llgo);
	if (V_OD_erg != SQL_SUCCESS)
	{
	   ErrPostEx(SEV_ERROR,0,0, "[LL_Append2GO_DB]: cannot insert into ll_go. ll_id=%ld, go_id=%ld, pmid=%ld, evidence=%s. SQLRETURN=%ld",
                     llid, goID, PMID, IsNullStr(eviCode), V_OD_erg);
	   LogSQLInfo(hstmt_llgo);
	   return LL_ERROR;
	}

  	return LL_APPEND_SUCCESS;
}

/*
** adds an entry consisting of ll_id and OMIM to the LL_OMIM DB
**
** return   LL_APPEND_SUCCESS if appending is successful.
*/
Int4
LL_Append2OMIM_DB(Int4 llid, Int4 OMIM)
{
  	if(llid <= 0){
    		ErrPostEx(SEV_ERROR, 0, 0,
			"[LL_Append2OMIM_DB]: Invalid ll_id on record with OMIM: %d.\nRecord skipped.",
			OMIM);
    		return LL_ERROR;
  	}
  	if(OMIM <= 0){
    		ErrPostEx(SEV_ERROR, 0, 0,
			"[LL_Append2OMIM_DB]: Invalid omim on record with ll_id: %d.\nRecord skipped.",
			llid);
    		return LL_ERROR;
  	}

	ll_omim.ll_id = llid;
	ll_omim.omim_id = OMIM;

  	/* execute statment */
  	V_OD_erg = SQLExecute (hstmt_llomim);
	if (V_OD_erg != SQL_SUCCESS)
	{
	   ErrPostEx(SEV_ERROR,0,0, "[LL_Append2OMIM_DB]: cannot insert into ll_omim. ll_id=%ld omim_id=%ld. SQLRETURN=%ld",
                     llid, OMIM, V_OD_erg);
	   LogSQLInfo(hstmt_llomim);
	   return LL_ERROR;
	}

  	return LL_APPEND_SUCCESS;
}

/**
** adds an entry consisting of ll_id, CDD, evalue to the LL_CDD DB
**
** returns 	LL_APPEND_SUCCESS if appending is successful
*/
Int4
LL_Append2CDD_DB(Int4 llid, CharPtr CDD, FloatHi evalue)
{
  	if(llid <= 0){
    		ErrPostEx(SEV_ERROR, 0, 0,
			"[LL_Append2CDD_DB]: Invalid ll_id on record with CDD: %s.\nRecord skipped.",
			IsNullStr(CDD));
    		return LL_ERROR;
  	}

  	if(CDD == NULL){
   		ErrPostEx(SEV_ERROR, 0, 0,
			"[LL_Append2CDD_DB]: Invalid cdd id on record with ll_ID: %d.\nRecord skipped.",
			llid);
    		return LL_ERROR;
  	}
  	/*if(evalue <= 0){
    		ErrPostEx(SEV_ERROR, 0, 0,
			"[LL_Append2CDD_DB]: Invalid evalue on record with ll_ID: %d.\nRecord skipped.",
			llid);
    		return LL_ERROR;
  	}*/

	ll_cdd.ll_id = llid;
	sprintf(ll_cdd.cdd_id, "%s", IsNullStr(CDD));
	if (evalue>0) {
		SLRI_SafeLog10(&evalue);
		ll_cdd.evalue = evalue;
	}

  	/* execute statment */
  	V_OD_erg = SQLExecute (hstmt_llcdd);
	if (V_OD_erg != SQL_SUCCESS)
	{
	   ErrPostEx(SEV_ERROR,0,0, "[LL_Append2CDD_DB]: cannot insert into ll_cdd. ll_id=%ld, cdd_id=%s, evalue=%ld. SQLRETURN=%ld.",
                     llid, IsNullStr(CDD), ll_cdd.evalue, V_OD_erg);
	   LogSQLInfo(hstmt_llcdd);
	   return LL_ERROR;
	}

  	return LL_APPEND_SUCCESS;
}

/**
** adds an entry consisting of llid, LLid, pMAP
**
** returns 	LL_APPEND_SUCCESS if appending is successful
*/
Int4
LL_Append2LLID_DB(Int4 llid, Int4 NPid, CharPtr pMAP)
{
  	if(NPid <= 0){
    		ErrPostEx(SEV_ERROR, 0, 0,
			"[LL_Append2LLID_DB]: Invalid %s: %d.\nRecord skipped.",
			NPid <= 0 ? "npid" : "llid", NPid < 0 ? NPid : llid );
    		return LL_ERROR;
  	}
  	/*if(pMAP == NULL){
    		ErrPostEx(SEV_ERROR, 0, 0,
			"[LL_Append2LLID_DB]: Invalid map on record with ll_id: %d.\nRecord skipped.",
			llid);
    		return LL_ERROR;
  	}*/

	ll_link.ll_id = llid;
	ll_link.gi = NPid;
	sprintf(ll_link.map, "%s", IsNullStr(pMAP));


  	/* execute statment */
  	V_OD_erg = SQLExecute (hstmt_lllink);
	if (V_OD_erg != SQL_SUCCESS)
	{
	   ErrPostEx(SEV_ERROR,0,0, "[LL_Append2LLID_DB]: cannot insert into ll_llink. ll_id=%ld, gi=%ld, map=%s. SQLRETURN=%ld",
                     llid, NPid, IsNullStr(pMAP), V_OD_erg);
	   LogSQLInfo(hstmt_lllink);
	   return LL_ERROR;
	}

  	return LL_APPEND_SUCCESS;
}


/****************************************************************
                       PUBLIC APPEND FUNCTION
*****************************************************************/
/**
** appends a record stored in ps to the 4 data bases LL_GO, LL_CDD,
** LL_OMIM, LL_LLINK.
**
** note		This program does not check for duplicates. If
**		you are reparsing the locus link database, delete the old files
**   		before reparsing, otherwise you will have doubles of all entries.
** Returns 	-1 if a record is skipped (those that don't have gi). Otherwise
**   		returns 0.
*/
Int4 LL_AppendRecord(LL_ParseStructPtr ps)
{
  ValNodePtr vnp1 = NULL,
             vnp2 = NULL,
             vnp3 = NULL;
  LL_CDDStructPtr cdd_tmp;

  /* adding a name and go id entry for each go id */
  /* SK: NULL ptr check */
  if(ps == NULL /* || ps->cdd_st == NULL */){
    ErrPostEx(SEV_INFO, 0, 0, "LL_AppendRecord: No NP id. Record skipped.");
    /* printf("Record skipped\n"); */
    return -1;
  }
  if ( NULL == ps->cdd_st )
  {
     ps->cdd_st = MemNew ( sizeof ( LL_CDDStruct ) );
     ps->cdd_st->iNPid = 0;
  }

  /* adding to the GO DB */
  if ( ps->vnpGOID != NULL )
  {
     for ( vnp1 = ps->vnpGOID, vnp2 = ps->vnpPMID, vnp3 = ps->vnpE_CODE;
           vnp1 != NULL;
	   vnp1 = vnp1->next, vnp2 = vnp2->next, vnp3 = vnp3->next )
     {
        LL_Append2GO_DB ( ps->iLLID, vnp1->data.intvalue,
			  vnp2->data.intvalue, vnp3->data.ptrvalue );
     }
  }

  if ( ps->vnpOMIM != NULL )
  {
     for ( vnp1 = ps->vnpOMIM; vnp1 != NULL; vnp1 = vnp1->next )
     {
        LL_Append2OMIM_DB ( ps->iLLID, vnp1->data.intvalue );
     }
  }


  for ( cdd_tmp = ps->cdd_st;
	cdd_tmp != NULL ;
	cdd_tmp = cdd_tmp->next )
  {
     if ( ps->cpMAP == NULL )
        LL_Append2LLID_DB ( ps->iLLID, cdd_tmp->iNPid, "NULL" );
     else
        LL_Append2LLID_DB ( ps->iLLID, cdd_tmp->iNPid, ps->cpMAP );

     for ( vnp1 = cdd_tmp->vnpCDD_ID, vnp2 = cdd_tmp->vnpE_VAL;
	   vnp1 != NULL;
	   vnp1 = vnp1->next, vnp2 = vnp2->next )
     {
        LL_Append2CDD_DB ( ps->iLLID, vnp1->data.ptrvalue, vnp2->data.realvalue );
     }
  }

  return 0;
}

/******************************************************************
* LL module is a little bit complicated in updating.
* The data in LL tables are inserted by llparser from file LL_tmpl
* and llgoa from DBXRefGOA tables.
* For updating LL module using llparser, the following data should 
* be deleted:
* - all records in LL_OMIM
* - all records with ll_id > 0 in LL_GO and LL_LLINK 
* - nothing in LL_CDD. CDD information has been omitted from file
* LL_tmpl since Sept. 4, 2004, table LL_CDD is now manually populated 
* using the data in RPSDB. 
*
* NOTE: the function name hence is DeleteRecordsInLL as opposed to 
* DeleteAllRecordInLL
*******************************************************************/
Boolean DeleteRecordsInLL(void)
{  
   CharPtr apchTables[3] = {"ll_omim", "ll_llink", "ll_go"};
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

   for (index=0; index<3; index++) {

     if (index == 0)
        sprintf((char *)delete_stmt, "delete from %s.%s;", database, apchTables[index]);
     else if (index == 1 || index == 2) 
        sprintf((char *)delete_stmt, "delete from %s.%s where ll_id>0;", database, apchTables[index]);


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
* LL module is a little bit complicated in updating.
* The data in LL tables are inserted by llparser from file LL_tmpl
* and llgoa from DBXRefGOA tables.
* For updating LL module using llgoa, the following data should 
* be deleted:
* - all records with ll_id < 0 in LL_GO and LL_LLINK 
*
* NOTE: the function name hence is DeleteRecordsInLL_GOA as opposed to 
* DeleteAllRecordInLL
*******************************************************************/
Boolean DeleteRecordsInLL_GOA(void)
{  
   CharPtr apchTables[2] = {"ll_llink", "ll_go"};
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

   for (index=0; index<2; index++) {

     sprintf((char *)delete_stmt, "delete from %s.%s where ll_id<0;", database, apchTables[index]);


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

/* $Log: ll_odbc.c,v $
/* Revision 1.19  2005/04/26 14:37:26  rcavero
/* Change Copyright Note.
/*
/* Revision 1.18  2005/03/22 15:02:39  vgu
/* Bug 5116 fix: Add check for non-zero locus link IDs and NULL CDD IDs
/*
/* Revision 1.17  2004/12/14 20:03:31  zwang
/* Added transaction related functions.
/*
/* Revision 1.16  2004/08/06 14:47:22  mmatan
/* Changed CharPtr database[50] to SQLCHAR database[50]
/*
/* Revision 1.15  2004/08/05 16:20:20  zwang
/* Remove DB2 related code.
/*
/* Revision 1.14  2004/06/11 15:19:32  zwang
/* Add flag to LL_InitUndoCodeBase to determine if should disconnect along with freeing SQLHandles for search functions. Make CloseAllLLDBs call LL_InitUndoCodeBase. See bug#2324.
/*
/* Revision 1.13  2004/06/10 16:58:03  zwang
/* Add flag to determine if a database connection should be established. See bug #2324.
/*
/* Revision 1.12  2004/06/08 20:54:10  zwang
/* Remove LL_InitUndoCodeBase from CloseAllLLDBs. Fixed typo.
/*
/* Revision 1.11  2004/05/27 17:12:49  zwang
/* Fix minor errors.
/*
/* Revision 1.10  2004/05/26 19:11:41  zwang
/* Changed the way data is fetched to structures.
/*
/* Revision 1.9  2004/05/19 19:20:08  zwang
/* Change schema and code to incorporate the major refactor in llparser.
/*
/* Revision 1.8  2004/03/17 20:47:38  zwang
/* Avoid repeating calls of LL_InitUndoCodeBase in llparser.
/*
/* Revision 1.7  2004/03/17 20:08:06  eburgess
/* Added OpenLL_GONoidx(), ReindexLL_GO() and CloseLL_GO() stubs for compatability
/* with codebase.
/*
/* Revision 1.6  2004/02/27 17:22:35  zwang
/* Got rid of warnings of SQLBindCol.
/*
/* Revision 1.5  2004/02/03 21:23:49  zwang
/* Cosmetic changes in log messages.
/*
/* Revision 1.4  2004/01/19 21:05:18  zwang
/* Changed the return values to be the same as in CodeBase layer.
/*
/* Revision 1.3  2004/01/13 18:39:49  zwang
/* Changed return macros.
/* Use generic database name.
/* Added more functions.
/* */
