/*$Id: rpsdb_odbc.c,v 1.13 2005/04/25 15:43:55 ryao Exp $*/
/*
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

  REVISION/CONTRIBUTION NOTES:

  March 1, 2001  Original MSH Revision.
                Hogue Lab - University of Toronto Biochemistry
                Department and the Samuel Lunenfeld Research
                Institute, Mount Sinai Hospital
                http://bioinfo.mshri.on.ca  hogue@mshri.on.ca
                ftp://ftp.mshri.on.ca/


  PUBLICATION TO CITE:



***************************************************************************
*/

/**
** @file 	rpsdb_odbc.c
** 
** @brief 	implementation of rps_odbc.h
** @authors:    Doron Betel           betel@mshri.on.ca
**            	Christopher W.V. Hogue hogue@mshri.on.ca
** 		Minzhi Lu
** @version 	2.0
** @date 	July 17, 2003
**
*/



#include <intrez_misc.h>
#include <slri_math.h>

#include "rpsdb_odbc.h"

/***************************************************************************************
**	Global variables used for all functions
***************************************************************************************/

/* ODBC variables used in this file */

/* henv and hdbc defined in slri_odbc.h */

static SQLHANDLE 		hstmt_rpsdb, hstmt_domname;

/* handle error message */
static SQLRETURN                V_OD_erg;                      /* result of functions */

static SQLCHAR                  V_OD_stat[SQL_SQLSTATE_SIZE +1];                 /* Status SQL */
static SQLCHAR                  V_OD_msg[SQL_MAX_MESSAGE_LENGTH + 1];
static SQLINTEGER               V_OD_err;
static SQLSMALLINT              V_OD_mlen;

/*** for testing **/
static Boolean 			testing = TRUE;

/* 
 * global stuct used in SQLBindParameter() 
 *  so these parameters only bind once 
 */

/* for table rpsdb */
static struct
{
	SQLINTEGER gi;
	SQLINTEGER cddid;
	Char domid[20];
	SQLINTEGER start;
	SQLINTEGER align_len;
	SQLINTEGER score;
	SQLDOUBLE evalue;
	SQLDOUBLE bitscore;
	SQLINTEGER missing_n;
	SQLINTEGER missing_c;
	SQLINTEGER numdom;
}rpsdb;

/* for table domname */
static struct
{
	SQLINTEGER pssmid;
	Char accession[50];
	Char name[255];
	Char description[1024];
	SQLINTEGER length;
	Char pdbid[15];
}domname;

static struct
{
	Int4 len;
	CharPtr val;
}domname_asn;

/* SeqHound Global Variables */
SQLCHAR database[50];


/************************************************************************/
/* 		rps database init functions 				*/
/************************************************************************/

Boolean LIBCALL RPS_InitCodeBase(void) {
	return RPS_ConnectToDB();
}
Boolean LIBCALL RPS_CloseCodeBase(void) {
	return RPS_DisconnectDB();
}

/* 
** forms a connection to the database and set up handles for the
** rps inserts.
**
** returns TRUE if connection is successful, FALSE otherwise
*/
Boolean
RPS_ConnectToDB(void)
{
 
   SQLCHAR dsn[10];
   SQLCHAR usr[10];
   SQLCHAR passwd[10];
   Char instancebuf[25];

   /* retrieve these 4 variables from the configuration file
    * .intrezrc in section datab. By doing it this way, we can change
    * the dsn simply by changing the configuration file
    */
   if( GetAppParam("intrez", "datab", "username", NULL, (Char*)usr, sizeof(char) * 10) <= 0 )
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[RPS_ConnectToDB]: Could not find username in initialization file.");
      exit(1);
   }
   if( GetAppParam("intrez", "datab", "password", NULL, (Char*)passwd, sizeof(char) * 10) <= 0 )
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[RPS_ConnectToDB]: Could not find pasword in initialization file.");
      exit(1);
   }
   if( GetAppParam("intrez", "datab", "dsn", NULL, (Char*)dsn, sizeof(char) * 10) <= 0 )
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[RPS_ConnectToDB]: Could not find dsn in initialization file.");
      exit(1);
   }

   /* database is a global parameter, we will use it when we initialize the sql statements,
      this way, we don't hardcode the database into the source code, allowing us to
      change the instance of the database by specifying it in the .intrezrc file, this
      is illustrated in each of the InitXXX functions below. */
   if( GetAppParam("intrez", "datab", "database", NULL, (Char*)database, sizeof(char) * 10) <= 0 )
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[RPS_ConnectToDB]: Could not find database in initialization file.");
      return FALSE;
   }

	/* 1. allocate Environment handle and register version */
        V_OD_erg = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);
        if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO))
        {
                ErrPostEx(SEV_ERROR, 0, 0, "Error AllocHandle");
	            exit(1);
        }

        /* 2. Declaring ODBC Version only for 3.x */
        V_OD_erg = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0);
        if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO))
        {
                ErrPostEx(SEV_ERROR, 0, 0, "Error SetEnv");
                SQLFreeHandle(SQL_HANDLE_ENV, henv);
                exit(1);
        }

        /* 3. allocate connection handle. */
        V_OD_erg = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);
        if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO))
        {
                ErrPostEx(SEV_ERROR, 0, 0, "Error AllocHDB");
                SQLFreeHandle(SQL_HANDLE_ENV, henv);
                exit(1);
        }

	/* 4. Set Connection Attributes: Set timeout. set autocommit off */
        SQLSetConnectAttr(hdbc, SQL_LOGIN_TIMEOUT, (SQLPOINTER *)5, 0);

        /* 5. Connect to the datasource */
        V_OD_erg = SQLConnect(hdbc, dsn, SQL_NTS, usr, SQL_NTS, passwd, SQL_NTS);
        if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO))
        {
                ErrPostEx(SEV_ERROR, 0, 0, "Error SQLConnect");
                SQLFreeHandle(SQL_HANDLE_ENV, henv);
                exit(1);
        }

        ErrPostEx(SEV_INFO, 0, 0, "Connected !\n");

	/* when in testing mode, set the autocommit to off, i
		otherwise, by default, autocommit is on */
	if (testing) {
	        V_OD_erg = SQLSetConnectAttr(hdbc, 
					SQL_ATTR_AUTOCOMMIT, 
					(SQLPOINTER *)SQL_AUTOCOMMIT_ON, 
					SQL_NTS);
			if (V_OD_erg != SQL_SUCCESS) {
                ErrPostEx(SEV_ERROR, 0, 0, "failed to set autocommit off");
				return FALSE;
			}
	}

    /* get here then connection to database successful */
	/* set up all handles for inserting */
   if (InitBindParameterforInsert() != TRUE)
   {
      ErrPostEx(SEV_ERROR,0,0, "[RPS_ConnectToDB]: Cannot InitBindParameterforInsert. SQLRETURN=%ld", V_OD_erg);
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

   /* table RPSDB */
   sprintf((char *)pSQL_stmt, "INSERT into %s.rpsdb(gi, cddid, domid, rfrom, align_len, score, evalue, bitscore, missing_n, missing_c, numdom) values(?,?,?,?,?,?,?,?,?,?,?)",
	        database);

   V_OD_erg = CreateHandle(&hstmt_rpsdb, pSQL_stmt,hdbc,&henv);
   if (V_OD_erg != SQL_SUCCESS) {
      ErrPostEx(SEV_ERROR, 0, 0, "Error CreateHandle for rpsdb");
	  return FALSE;
   }

  	V_OD_erg = SQLBindParameter(hstmt_rpsdb, 1, SQL_PARAM_INPUT, 
		SQL_C_LONG,SQL_INTEGER, 0, 0, &rpsdb.gi, 0, NULL);
   if (V_OD_erg != SQL_SUCCESS) {
      ErrPostEx(SEV_ERROR, 0, 0, "Error: unable to bind parameter 1 for rpsdb.");
	  return FALSE;
   }

  	V_OD_erg = SQLBindParameter(hstmt_rpsdb, 2, SQL_PARAM_INPUT, 
		SQL_C_LONG,SQL_INTEGER, 0, 0, &rpsdb.cddid, 0, NULL);
   if (V_OD_erg != SQL_SUCCESS) {
      ErrPostEx(SEV_ERROR, 0, 0, "Error: unable to bind parameter 2 for rpsdb.");
	  return FALSE;
   }

  	V_OD_erg = SQLBindParameter(hstmt_rpsdb, 3, SQL_PARAM_INPUT, 
		SQL_C_CHAR,SQL_VARCHAR, 0, 0, rpsdb.domid, sizeof(rpsdb.domid), NULL);
   if (V_OD_erg != SQL_SUCCESS) {
      ErrPostEx(SEV_ERROR, 0, 0, "Error: unable to bind parameter 3 for rpsdb.");
	  return FALSE;
   }

  	V_OD_erg = SQLBindParameter(hstmt_rpsdb, 4, SQL_PARAM_INPUT, 
		SQL_C_LONG,SQL_INTEGER, 0, 0, &rpsdb.start, 0, NULL);
   if (V_OD_erg != SQL_SUCCESS) {
      ErrPostEx(SEV_ERROR, 0, 0, "Error: unable to bind parameter 4 for rpsdb.");
	  return FALSE;
   }

  	V_OD_erg = SQLBindParameter(hstmt_rpsdb, 5, SQL_PARAM_INPUT, 
		SQL_C_LONG,SQL_INTEGER, 0, 0, &rpsdb.align_len, 0, NULL);
   if (V_OD_erg != SQL_SUCCESS) {
      ErrPostEx(SEV_ERROR, 0, 0, "Error: unable to bind parameter 5 for rpsdb.");
	  return FALSE;
   }

  	V_OD_erg = SQLBindParameter(hstmt_rpsdb, 6, SQL_PARAM_INPUT, 
		SQL_C_LONG,SQL_INTEGER, 0, 0, &rpsdb.score, 0, NULL);
   if (V_OD_erg != SQL_SUCCESS) {
      ErrPostEx(SEV_ERROR, 0, 0, "Error: unable to bind parameter 6 for rpsdb.");
	  return FALSE;
   }

  	V_OD_erg = SQLBindParameter(hstmt_rpsdb, 7, SQL_PARAM_INPUT, 
		SQL_C_DOUBLE,SQL_DOUBLE, 0, 0, &rpsdb.evalue, 0, NULL); 
   if (V_OD_erg != SQL_SUCCESS) {
      ErrPostEx(SEV_ERROR, 0, 0, "Error: unable to bind parameter 7 for rpsdb.");
	  return FALSE;
   }

  	V_OD_erg = SQLBindParameter(hstmt_rpsdb, 8, SQL_PARAM_INPUT, 
		SQL_C_DOUBLE,SQL_DOUBLE, 0, 0, &rpsdb.bitscore, 0, NULL); 
   if (V_OD_erg != SQL_SUCCESS) {
      ErrPostEx(SEV_ERROR, 0, 0, "Error: unable to bind parameter 8 for rpsdb.");
	  return FALSE;
   }

  	V_OD_erg = SQLBindParameter(hstmt_rpsdb, 9, SQL_PARAM_INPUT, 
		SQL_C_LONG,SQL_INTEGER, 0, 0, &rpsdb.missing_n, 0, NULL);
   if (V_OD_erg != SQL_SUCCESS) {
      ErrPostEx(SEV_ERROR, 0, 0, "Error: unable to bind parameter 9 for rpsdb.");
	  return FALSE;
   }

  	V_OD_erg = SQLBindParameter(hstmt_rpsdb, 10, SQL_PARAM_INPUT, 
		SQL_C_LONG,SQL_INTEGER, 0, 0, &rpsdb.missing_c, 0, NULL);
   if (V_OD_erg != SQL_SUCCESS) {
      ErrPostEx(SEV_ERROR, 0, 0, "Error: unable to bind parameter 10 for rpsdb.");
	  return FALSE;
   }

  	V_OD_erg = SQLBindParameter(hstmt_rpsdb, 11, SQL_PARAM_INPUT, 
		SQL_C_LONG,SQL_INTEGER, 0, 0, &rpsdb.numdom, 0, NULL);
   if (V_OD_erg != SQL_SUCCESS) {
      ErrPostEx(SEV_ERROR, 0, 0, "Error: unable to bind parameter 11 for rpsdb.");
	  return FALSE;
   }


   /* table DOMNAME */
   sprintf((char *)pSQL_stmt, "INSERT into %s.domname(pssmid, accession, name, description, length, pdbid) values(?,?,?,?,?,?)", database);
	V_OD_erg = CreateHandle(&hstmt_domname, pSQL_stmt, hdbc,&henv);
   if (V_OD_erg != SQL_SUCCESS) {
      ErrPostEx(SEV_ERROR, 0, 0, "Error CreateHandle for domname");
	  return FALSE;
   }

  	V_OD_erg = SQLBindParameter(hstmt_domname, 1, SQL_PARAM_INPUT, 
		SQL_C_LONG,SQL_INTEGER, 0, 0, &domname.pssmid, 0, NULL);
   if (V_OD_erg != SQL_SUCCESS) {
      ErrPostEx(SEV_ERROR, 0, 0, "Error: unable to bind parameter 1 for domname.");
	  return FALSE;
   }
  	V_OD_erg = SQLBindParameter(hstmt_domname, 2, SQL_PARAM_INPUT, 
		SQL_C_CHAR,SQL_VARCHAR, 0, 0, domname.accession, sizeof(domname.accession), NULL);
   if (V_OD_erg != SQL_SUCCESS) {
      ErrPostEx(SEV_ERROR, 0, 0, "Error: unable to bind parameter 2 for domname.");
	  return FALSE;
   }

  	V_OD_erg = SQLBindParameter(hstmt_domname, 3, SQL_PARAM_INPUT, 
		SQL_C_CHAR,SQL_VARCHAR, 0, 0, domname.name, sizeof(domname.name), NULL);
   if (V_OD_erg != SQL_SUCCESS) {
      ErrPostEx(SEV_ERROR, 0, 0, "Error: unable to bind parameter 3 for domname.");
	  return FALSE;
   }

  	V_OD_erg = SQLBindParameter(hstmt_domname, 4, SQL_PARAM_INPUT, 
		SQL_C_CHAR,SQL_VARCHAR, 0, 0, domname.description, sizeof(domname.description), NULL);
   if (V_OD_erg != SQL_SUCCESS) {
      ErrPostEx(SEV_ERROR, 0, 0, "Error: unable to bind parameter 4 for domname.");
	  return FALSE;
   }

  	V_OD_erg = SQLBindParameter(hstmt_domname, 5, SQL_PARAM_INPUT, 
		SQL_C_LONG,SQL_INTEGER, 0, 0, &domname.length, 0, NULL);
   if (V_OD_erg != SQL_SUCCESS) {
      ErrPostEx(SEV_ERROR, 0, 0, "Error: unable to bind parameter 5 for domname.");
	  return FALSE;
   }

  	V_OD_erg = SQLBindParameter(hstmt_domname, 6, SQL_PARAM_INPUT, 
		SQL_C_CHAR,SQL_VARCHAR, 0, 0, domname.pdbid, sizeof(domname.pdbid), NULL);
   if (V_OD_erg != SQL_SUCCESS) {
      ErrPostEx(SEV_ERROR, 0, 0, "Error: unable to bind parameter 6 for domname.");
	  return FALSE;
   }

   return TRUE;
}
 

/*
** closes the databases and free the environment handles. 
**
**  This function must called at the
**   end of your function, if you called RPS_InitCodeBase.
**  
** returns TRUE if disconnection is successful, FALSE otherwise
**
*/
Boolean
RPS_DisconnectDB(void)
{
	/* free the statment handle */
	SQLFreeStmt(hstmt_rpsdb, SQL_RESET_PARAMS);
	SQLFreeStmt(hstmt_domname, SQL_RESET_PARAMS);


	/*Free the database handles used to communicate with the database*/
	V_OD_erg = SQLDisconnect(hdbc);
    if (V_OD_erg != SQL_SUCCESS) {
       ErrPostEx(SEV_ERROR, 0, 0, "Error: failed to disconnect");
	   return FALSE;
    }

	/*free the connnection handle */
	V_OD_erg = SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
    if (V_OD_erg != SQL_SUCCESS) {
       ErrPostEx(SEV_ERROR, 0, 0, "failed to free connection handle");
	   return FALSE;
    }

	/*free envirnment handle */
	V_OD_erg = SQLFreeHandle(SQL_HANDLE_ENV, henv);
    if (V_OD_erg != SQL_SUCCESS) {
       ErrPostEx(SEV_ERROR, 0, 0, "failed to free environment handle");
	   return FALSE;
    }

	return TRUE; 
}

/****************************************************************/
/* Given an SQL handle, set AutoCommit off                      */
/****************************************************************/
Boolean RPS_StartTransaction(void)
{
	return SetAutoCommitOff(hdbc);
}

/****************************************************************/
/* Given an SQL handle, commit                                  */
/****************************************************************/
Boolean RPS_CommitTransaction(void)
{
	return CommitChanges(hdbc);
}

/****************************************************************/
/* Given an SQL handle, roll back                               */
/****************************************************************/
Boolean RPS_RollBackTransaction(void)
{
	return RollBackChanges(hdbc);
}


/**************************************************************
                 APPENDING FUNCTIONS			       
***************************************************************/


/*
** Append record to RPS Database
** @param	StRpsdbPtr
**
** @return  	TRUE if append successfully
**		FALSE otherwise
*/
Boolean
AppendRecordRPSDB(StRpsdbPtr record)
{
	if(record == NULL)
	{
		ErrPostEx(SEV_ERROR, 0, 0, 
			"AppendRecordRPSDB: missing information.");
		return FALSE;
	}

	if (record->evalue > 0)
		SLRI_SafeLog10(&(record->evalue));
	if (record->bitscore > 0)
	SLRI_SafeLog10(&(record->bitscore));

	rpsdb.gi = record->gi;
	rpsdb.cddid = record->cdd_id;
	sprintf(rpsdb.domid, "%s", IsNullStr(record->dom_id));
	rpsdb.start = record->from;
	rpsdb.align_len = record->len;
	rpsdb.score = record->score;
	rpsdb.evalue = record->evalue;
	rpsdb.bitscore = record->bitscore;
	rpsdb.missing_n = record->n_missing;
	rpsdb.missing_c = record->c_missing;
	rpsdb.numdom = record->num_of_dom;

	/* execute statment */
  	V_OD_erg = SQLExecute (hstmt_rpsdb);
    if (V_OD_erg != SQL_SUCCESS)
    {
	  ErrPostEx(SEV_ERROR,0,0, "[AppendRecordRPSDB]: unable to execute insert statment");
	  LogSQLInfo(hstmt_rpsdb);

       return FALSE;
    }

  	return TRUE;
}

/* Append to DOMNAME table */
Boolean
AppendRecordDOMNAME(StDomNamedbPtr dnp)
{

	if( dnp == NULL)
	{
		ErrPostEx(SEV_ERROR, 0, 0, " AppendRecordDOMNAME: NULL parameter.");
		return FALSE;
	}

	domname.pssmid = dnp->pssmid;
	sprintf(domname.accession, "%s", IsNullStr(dnp->accession));
	sprintf(domname.name, "%s", IsNullStr(dnp->name));
	sprintf(domname.description, "%s", IsNullStr(dnp->description));
	domname.length = dnp->length;
fprintf(stdout, "length is %ld\n", domname.length);
	if (dnp->pdb_id == NULL)
fprintf(stdout, "pdbid is NULL\n");
	//	sprintf(domname.pdbid, "n/a");
	else
fprintf(stdout, "pdbid is not NULL\n");
	//	sprintf(domname.pdbid, "%s", dnp->pdb_id);

fprintf(stdout, "pdbid is %s\n", domname.pdbid);

	/* execute statment */
  	V_OD_erg = SQLExecute (hstmt_domname);
    if (V_OD_erg != SQL_SUCCESS)
    {
	  ErrPostEx(SEV_ERROR,0,0, "[AppendRecordDOMNAME]: unable to execute insert statment");
	  LogSQLInfo(hstmt_domname);
       return FALSE;
    }

	return TRUE;

}

/**************************************************************
                 DELETEING FUNCTIONS			 
	note: not implemented in ODBC      
***************************************************************/


/*Delete record from RPS table*/
Boolean DeleteGIinRPSDB(SQLINTEGER gi, Boolean destroy)
{
   CharPtr rpsdb_delete = (char *) malloc(sizeof(Char) * 150);
   if (rpsdb_delete == NULL)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[DeleteGIinRPSDB]: Could not allocate space");
     free(rpsdb_delete);
     return FALSE;
   }
   sprintf((char *)rpsdb_delete, "delete from %s.rpsdb where gi=?", database);

   SQLHANDLE hstmt;
   Int2 sql_return = TRUE;

   CreateHandle(&hstmt, rpsdb_delete, hdbc, &henv);
   free(rpsdb_delete);


   sql_return = SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_SLONG,SQL_INTEGER, 0, 0, &gi, 0, NULL);
   if(sql_return!= SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[DeleteGIinRPSDB]: Could not bind parameter 1");
      return FALSE;
   }

   /* set auto commit off */
      SQLSetConnectAttr( hdbc, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_OFF, SQL_NTS);


  /* execute statment */
  sql_return = SQLExecute(hstmt);
   if (sql_return != SQL_SUCCESS)
   {
	  ErrPostEx(SEV_ERROR,0,0, "[DeleteGIinRPSDB]:unable to execute delete statement.");
	  LogSQLInfo(hstmt);
      return FALSE;
   }

   SQLFreeStmt(hstmt, SQL_DROP);

    return TRUE;
}

/*Delete record from DomName table*/
Boolean DeleteAccinDOMNAME(CharPtr accession, Boolean destroy)
{
   CharPtr domname_delete = (char *) malloc(sizeof(Char) * 150);
   if (domname_delete == NULL)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[DeleteGIinDOMNAME]: Could not allocate space");
     free(domname_delete);
     return FALSE;
   }
   sprintf((char *)domname_delete, "delete from %s.domname where accession=?", database);

   SQLHANDLE hstmt;
   Int2 sql_return = TRUE;

   CreateHandle(&hstmt, domname_delete, hdbc, &henv);
   free(domname_delete);

   sql_return = SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR,SQL_VARCHAR, 0, 0, accession, sizeof(accession), NULL);
   if(sql_return!= SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[DeleteAccinDOMNAME]: Could not bind parameter 1");
      return FALSE;
   }

   /* set auto commit off */
      SQLSetConnectAttr( hdbc, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_OFF, SQL_NTS);


  /* execute statment */
  sql_return = SQLExecute(hstmt);
   if (sql_return != SQL_SUCCESS)
   {
	  ErrPostEx(SEV_ERROR,0,0, "[DeleteGIinDOMNAME]:unable to execute delete statement.");
	  LogSQLInfo(hstmt);
      return FALSE;
   }

   SQLFreeStmt(hstmt, SQL_DROP);

    return TRUE;
}

/**************************************************************
                 SEARCHING FUNCTIONS			       
***************************************************************/
static ValNodePtr 
getRecord(CharPtr select_name, CharPtr table_name, CharPtr where_name, SQLINTEGER inputID, CharPtr inputStr)
{

	SQLHANDLE V_OD_hstmt;
	SQLINTEGER outputid;
	Char outputstr[MAXCHARLEN];
        SQLINTEGER outputstrlen=0;
	Char str[MAXCHARLEN];		
	/* a ValNodePtr to hold the return value */
	ValNodePtr vnp;

	Char search_str[200];
	sprintf(search_str, "select %s from %s where %s=?", IsNullStr(select_name), IsNullStr(table_name), IsNullStr(where_name));

	if (inputStr != NULL)
		sprintf(str, inputStr);
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
		if ((V_OD_erg != SQL_SUCCESS))
		{
		   ErrPostEx(SEV_ERROR,0,0, "[getRecord]: unable to bind parameter 1 for table %s column %s value %ld. SQLRETURN=%ld", 
			   table_name, where_name, inputID, V_OD_erg);

		   SQLFreeHandle(SQL_HANDLE_STMT, V_OD_hstmt); 
		   return NULL;
		}

	} else {		/* input is a string */
		V_OD_erg = SQLBindParameter(V_OD_hstmt, 1, SQL_PARAM_INPUT, 
				SQL_C_CHAR,SQL_VARCHAR, 0, 0, str, sizeof(str), NULL);
		if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO))
		{
		   ErrPostEx(SEV_ERROR,0,0, "[getRecord]: unable to bind parameter 1 for table %s column %s value %s. SQLRETURN=%ld", 
			   table_name, where_name, str, V_OD_erg);

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
	if (inputID != -1){	/* return type is an integer */
		V_OD_erg = SQLBindCol(V_OD_hstmt,1,SQL_C_LONG,&outputid,0, NULL);  
	} else {		/* return type is a string */
		SQLBindCol(V_OD_hstmt,1,SQL_C_CHAR,&outputstr,sizeof(outputstr), (int*)&outputstrlen);  
	}
	
	if (V_OD_erg != SQL_SUCCESS) {
	  ErrPostEx(SEV_ERROR, 0, 0, "Error: unable to bind col 2 for table %s column %s. SQLRETURN=%ld", 
		   table_name, where_name, V_OD_erg);
	  SQLFreeHandle(SQL_HANDLE_STMT, V_OD_hstmt); 
      return NULL;
	}

	/* fetch data */
	V_OD_erg = SQLFetch(V_OD_hstmt);
	if (V_OD_erg == SQL_NO_DATA_FOUND) 
	{
		SQLFreeStmt(V_OD_hstmt, SQL_DROP);
		return NULL;
	}
	while (V_OD_erg == SQL_SUCCESS || V_OD_erg == SQL_SUCCESS_WITH_INFO){

		if (inputID != -1) {		/* return type is an integer */
			ValNodeAddInt(&vnp, 1, outputid);
		} else {			/* return type is a string */
			ValNodeAddStr(&vnp, 2, outputstr);
		}

		/* fetch next row */
		V_OD_erg = SQLFetch(V_OD_hstmt);
	}

	/* Free statement handle. */
	SQLFreeHandle(SQL_HANDLE_STMT, V_OD_hstmt); 

	return vnp;

}



/*Check if GI is in RPSDB */
Boolean IsExistInRPSDB(Int4 gi)
{
	ValNodePtr vpr = getRecord("gi", "rpsdb", "gi", gi, NULL);

	/* gi not found */
	if (vpr == NULL) return FALSE;

	if (vpr->data.intvalue == -1) /* cannot find the gi */
		return FALSE;
	else		/* found */
		return TRUE;
}

/*Check if domain accession exist in DOMNAME */
Boolean IsExistInDOMNAME(CharPtr accession)
{
	ValNodePtr vpr = getRecord("accession", "domname", "accession", -1, accession);

	/* not found */
	if (vpr == NULL) return FALSE;

	if (vpr->data.ptrvalue == NULL) /*can not find */
		return FALSE;
	else
		return TRUE;
}

/****************************
* Search Functions
*****************************/

/*Return record from RPSDb
* searches can be done using any of the
* tags defined for this table. Returns all
* matching records. Note that Evalue must be set to some value*/
Int2 
SearchRPSDB(StRpsdbPtr PNTR pprs)
{
	SQLHANDLE V_OD_hstmt;
	Char search_str[200];
	Char where_name[10];

	Char outputStr[MAXCHARLEN];
	SQLINTEGER gi, cdd_id, from, len, score, n_missing, c_missing, num_of_dom, outputStrlen=0;
	FloatHi evalue=0.0, bitscore=0.0;
	FloatHi evalueCutOff=0.0;

	StRpsdbPtr rpsdbp = NULL;
	StRpsdbPtr prev= NULL;
	SQLINTEGER key=0;
	CharPtr pKey = NULL;

	/* SK: NULL ptr check */
	if ( pprs == NULL || *pprs == NULL )
	{
	    ErrPostEx(SEV_ERROR, 0, 0,"SearchRPSDB: NULL pprs");
	    return 0;
	}

	rpsdbp=*pprs;
	/*check if any of the tags are set*/
	if( ( ((rpsdbp)->gi<=0) && ((rpsdbp)->dom_id==NULL) &&
		((rpsdbp)->num_of_dom==0) && ((rpsdbp)->len==0) && ((rpsdbp)->cdd_id==0) ) ){
		ErrPostEx(SEV_ERROR, 0, 0,"SearchRPSDB: no valid search key");
		return -1;
	}

	evalueCutOff = rpsdbp->evalue;

	/*searches based on gi,number of domains, or length or cddid*/
	if((rpsdbp)->gi>0 ||(rpsdbp)->num_of_dom>0 || (rpsdbp)->len>0 || (rpsdbp)->cdd_id>0 )
	{
		if((rpsdbp)->gi>0){
			key=(rpsdbp)->gi;
			sprintf(where_name, "%s", "gi");
		}
		if((rpsdbp)->num_of_dom>0){
			key=(rpsdbp)->num_of_dom;
			sprintf(where_name, "%s", "numdom");
		}
		if((rpsdbp)->len>0){
			key=(rpsdbp)->len;
			sprintf(where_name, "%s", "align_len");
		}
		if((rpsdbp)->cdd_id>0){
			key=(rpsdbp)->cdd_id;
			sprintf(where_name, "%s",  "cddid");
		}
	}
	/* search based on domid */
	else if (rpsdbp->dom_id != NULL) {
		pKey = rpsdbp->dom_id; 
		sprintf(where_name, "%s",  "domid");
	} 
	sprintf(search_str, "select gi, cddid, domid, rfrom, align_len, score, evalue, bitscore, missing_n, missing_c, numdom from %s.rpsdb where %s=?", database, IsNullStr(where_name));

	V_OD_erg = CreateHandle(&V_OD_hstmt, search_str, hdbc,&henv);
	if (V_OD_erg != SQL_SUCCESS) {
       ErrPostEx(SEV_ERROR, 0, 0, "Error CreateHandle for SearchRPSDB");
	   return -1;
    }

	/* bind input */
	if((rpsdbp)->gi>0 ||(rpsdbp)->num_of_dom>0 || (rpsdbp)->len>0 || (rpsdbp)->cdd_id>0 ) {
		V_OD_erg = SQLBindParameter(V_OD_hstmt, 1, SQL_PARAM_INPUT, 
				SQL_C_LONG,SQL_INTEGER, 0, 0, &key, 0, NULL);
	}
	else if (rpsdbp->dom_id != NULL) {
		V_OD_erg = SQLBindParameter(V_OD_hstmt, 1, SQL_PARAM_INPUT, 
				SQL_C_DEFAULT,SQL_VARCHAR, 12, 0, pKey, 0, NULL);
	}

	if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO))
	{
	  ErrPostEx(SEV_ERROR,0,0, "[SearchRPSDB]: unable to bind parameter 1. SQLRETURN=%ld", V_OD_erg);

	  SQLFreeHandle(SQL_HANDLE_STMT, V_OD_hstmt); 
	  return -1;
	}

	/* execute statment */
	V_OD_erg = SQLExecute(V_OD_hstmt); 
	if (V_OD_erg != SQL_SUCCESS)
	{
	   ErrPostEx(SEV_ERROR,0,0, "[SearchRPSDB]: Unable to execute select statement.");
	   LogSQLInfo(V_OD_hstmt);
	   SQLFreeHandle(SQL_HANDLE_STMT, V_OD_hstmt); 
	   return -1;
	}

	/* bind the output value */
	V_OD_erg = SQLBindCol(V_OD_hstmt,1,SQL_C_LONG,&gi,0, NULL);
    if (V_OD_erg != SQL_SUCCESS) {
       ErrPostEx(SEV_ERROR, 0, 0, "Error: unable to bind col 1. SQLRETURN=%ld",V_OD_erg);
       SQLFreeHandle(SQL_HANDLE_STMT, V_OD_hstmt); 
       return -1;
	}
	V_OD_erg = SQLBindCol(V_OD_hstmt,2,SQL_C_LONG,&cdd_id,0, NULL);  
    if (V_OD_erg != SQL_SUCCESS) {
       ErrPostEx(SEV_ERROR, 0, 0, "Error: unable to bind col 2. SQLRETURN=%ld",V_OD_erg);
       SQLFreeHandle(SQL_HANDLE_STMT, V_OD_hstmt); 
       return -1;
	}
	V_OD_erg = SQLBindCol(V_OD_hstmt,3,SQL_C_CHAR,&outputStr,sizeof(outputStr), (int*)&outputStrlen);  
    if (V_OD_erg != SQL_SUCCESS) {
       ErrPostEx(SEV_ERROR, 0, 0, "Error: unable to bind col 3. SQLRETURN=%ld",V_OD_erg);
       SQLFreeHandle(SQL_HANDLE_STMT, V_OD_hstmt); 
       return -1;
	}
	V_OD_erg = SQLBindCol(V_OD_hstmt,4,SQL_C_LONG,&from,0, NULL);  
    if (V_OD_erg != SQL_SUCCESS) {
       ErrPostEx(SEV_ERROR, 0, 0, "Error: unable to bind col 4. SQLRETURN=%ld",V_OD_erg);
       SQLFreeHandle(SQL_HANDLE_STMT, V_OD_hstmt); 
       return -1;
	}
	V_OD_erg = SQLBindCol(V_OD_hstmt,5,SQL_C_LONG,&len,0, NULL);  
    if (V_OD_erg != SQL_SUCCESS) {
       ErrPostEx(SEV_ERROR, 0, 0, "Error: unable to bind col 5. SQLRETURN=%ld",V_OD_erg);
       SQLFreeHandle(SQL_HANDLE_STMT, V_OD_hstmt); 
       return -1;
	}
	V_OD_erg = SQLBindCol(V_OD_hstmt,6,SQL_C_LONG,&score,0, NULL);  
    if (V_OD_erg != SQL_SUCCESS) {
       ErrPostEx(SEV_ERROR, 0, 0, "Error: unable to bind col 6. SQLRETURN=%ld",V_OD_erg);
       SQLFreeHandle(SQL_HANDLE_STMT, V_OD_hstmt); 
       return -1;
	}
	V_OD_erg = SQLBindCol(V_OD_hstmt,7,SQL_C_DOUBLE,&evalue,0, NULL);  
    if (V_OD_erg != SQL_SUCCESS) {
       ErrPostEx(SEV_ERROR, 0, 0, "Error: unable to bind col 7. SQLRETURN=%ld",V_OD_erg);
       SQLFreeHandle(SQL_HANDLE_STMT, V_OD_hstmt); 
       return -1;
	}
	V_OD_erg = SQLBindCol(V_OD_hstmt,8,SQL_C_DOUBLE,&bitscore,0, NULL);  
    if (V_OD_erg != SQL_SUCCESS) {
       ErrPostEx(SEV_ERROR, 0, 0, "Error: unable to bind col 8. SQLRETURN=%ld",V_OD_erg);
       SQLFreeHandle(SQL_HANDLE_STMT, V_OD_hstmt); 
       return -1;
	}
	V_OD_erg = SQLBindCol(V_OD_hstmt,9,SQL_C_LONG,&n_missing,0, NULL);  
    if (V_OD_erg != SQL_SUCCESS) {
       ErrPostEx(SEV_ERROR, 0, 0, "Error: unable to bind col 9. SQLRETURN=%ld",V_OD_erg);
       SQLFreeHandle(SQL_HANDLE_STMT, V_OD_hstmt); 
       return -1;
	}
	V_OD_erg = SQLBindCol(V_OD_hstmt,10,SQL_C_LONG,&c_missing,0, NULL);  
    if (V_OD_erg != SQL_SUCCESS) {
       ErrPostEx(SEV_ERROR, 0, 0, "Error: unable to bind col 10. SQLRETURN=%ld",V_OD_erg);
       SQLFreeHandle(SQL_HANDLE_STMT, V_OD_hstmt); 
       return -1;
	}
	V_OD_erg = SQLBindCol(V_OD_hstmt,11,SQL_C_LONG,&num_of_dom,0, NULL);  
    if (V_OD_erg != SQL_SUCCESS) {
       ErrPostEx(SEV_ERROR, 0, 0, "Error: unable to bind col 11. SQLRETURN=%ld",V_OD_erg);
       SQLFreeHandle(SQL_HANDLE_STMT, V_OD_hstmt); 
       return -1;
	}
	/* fetch data */
	V_OD_erg = SQLFetch(V_OD_hstmt);
	if (V_OD_erg == SQL_NO_DATA_FOUND) 
	{
		SQLFreeStmt(V_OD_hstmt, SQL_DROP);
		/* pprs will be freed in the caller program, fix bug [5425] */
		/* *pprs=NULL; */ 
		return -1;
	}
	while (V_OD_erg == SQL_SUCCESS || V_OD_erg == SQL_SUCCESS_WITH_INFO){
		/* get domain id lower or equal to evalueCutOff */
		if (evalue <= evalueCutOff || evalue == 0) {
		   rpsdbp->gi = gi;
		   rpsdbp->cdd_id = cdd_id;
		   rpsdbp->dom_id = StringSave(outputStr);
		   rpsdbp->from = from;
		   rpsdbp->len = len;
		   rpsdbp->score = score;
	   	   rpsdbp->evalue = evalue;
		   rpsdbp->bitscore = bitscore;
		   rpsdbp->n_missing = n_missing;
		   rpsdbp->c_missing = c_missing;
		   rpsdbp->num_of_dom = num_of_dom;

		   rpsdbp->next= (StRpsdbPtr)StRpsdbNew();
		   prev=rpsdbp;
		   rpsdbp=rpsdbp->next;
		}
		
		/* fetch next row */
		V_OD_erg = SQLFetch(V_OD_hstmt);
	}
	/* last node is deleted */
	if((*pprs)->num_of_dom==0)
		*pprs=NULL;
	rpsdbp=StRpsdbFree(rpsdbp);
	if(prev!=NULL)
		prev->next=NULL;

	/* Free statement handle. */
	SQLFreeHandle(SQL_HANDLE_STMT, V_OD_hstmt); 
	return 1;
}

/* This function is currently not in use!!
* Query times are too long to be practical.
*-----------------------------------------
* Search RPSDB using a combination of any
* two tags from RPSDB and an Evalue (gi, dom_id, len, numdom)
* Can search any combination of two tags with evalue or any single tag with evalue.
* Precondition- No more than two tags can be set. Evalue must be set even if just default value
* Note that evalues should be in log form.
*/
Int2 
SearchRPSDBByTagsAndEvalue(StRpsdbPtr PNTR pprs)
{
	/*This function is currently not in use!!
 Query times are too long to be practical.*/
	return 0;
}


/* search DOMNAME table based on domain accession,
** domain name, or mmdb-id 
**
** @param:	ppdn: StDomnamedbPtr
** @param:	noasn: FALSE if do not want to retrieve asn1 data
**			TRUE retrieve asn1 data
*/
Int2 
SearchDOMNAME(StDomNamedbPtr PNTR ppdn, Boolean noasn)
{
	SQLHANDLE V_OD_hstmt;
	Char key[20];
	Char search_str[MAXCHARLEN];
	CharPtr where_name;
	Char outputStr1[MAXCHARLEN];
	Char outputStr2[MAXCHARLEN];
	Char outputStr3[MAXCHARLEN];
	Char outputStr4[MAXCHARLEN];
	SQLINTEGER pssmid=0, length=0;
        SQLINTEGER outputStr1len=0, outputStr2len=0, outputStr3len=0, outputStr4len=0;

	SQLINTEGER fetchCount=0;

	StDomNamedbPtr domnamep= NULL;
	int count = 0;

	/*check if any of the tags are set*/
	/* SK: NULL ptr check */
	if((ppdn==NULL) || (*ppdn == NULL) || ((*ppdn)->pdb_id==NULL) && ((*ppdn)->accession==NULL) &&
			((*ppdn)->name==NULL)){
		ErrPostEx(SEV_ERROR, 0, 0,"SearchDOMNAME: no valid search key");
		return 0;
	}

	domnamep = *ppdn;
	if (domnamep->pdb_id != NULL) {
		sprintf(key, "%s", IsNullStr(domnamep->pdb_id));
		where_name = "pdbid";
	}

	if (domnamep->name != NULL) {
		sprintf(key, "%s", IsNullStr(domnamep->name));
		where_name = "name";
	}

	if (domnamep->accession != NULL) {
		sprintf(key, "%s", IsNullStr(domnamep->accession));
		where_name = "accession";
	}

	sprintf(search_str, 
		"select pssmid, accession, name, description, length, pdbid from %s.domname where %s=?", 
		database, IsNullStr(where_name));

	V_OD_erg = CreateHandle(&V_OD_hstmt, search_str, hdbc,&henv);
	if (V_OD_erg != SQL_SUCCESS) {
		ErrPostEx(SEV_ERROR, 0, 0, "Error CreateHandle for SearchDOMNAME");
		return -1;
	}

	/* bind input */
	V_OD_erg = SQLBindParameter(V_OD_hstmt, 1, SQL_PARAM_INPUT, 
				SQL_C_CHAR,SQL_VARCHAR, 0, 0, key, sizeof(key), NULL);
	if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO))
	{
		ErrPostEx(SEV_ERROR,0,0, "[SearchDOMNAME]: unable to bind parameter 1 for. SQLRETURN=%ld", V_OD_erg);
		SQLFreeHandle(SQL_HANDLE_STMT, V_OD_hstmt); 
		return -1;
	}


	/* execute statment */
	V_OD_erg =  SQLExecute(V_OD_hstmt); 
	if (V_OD_erg != SQL_SUCCESS)
	{
	   ErrPostEx(SEV_ERROR,0,0, "[SearchDOMNAME]: Unable to execute select statement.");
	   LogSQLInfo(V_OD_hstmt);
	   SQLFreeHandle(SQL_HANDLE_STMT, V_OD_hstmt); 
	   return -1;
	}

	/* bind the output value */
	V_OD_erg = SQLBindCol(V_OD_hstmt,1,SQL_C_LONG,&pssmid,0, NULL);  
	if (V_OD_erg != SQL_SUCCESS) {
		ErrPostEx(SEV_ERROR, 0, 0, "Error: unable to bind col 1. SQLRETURN=%ld",V_OD_erg);
		SQLFreeHandle(SQL_HANDLE_STMT, V_OD_hstmt); 
		return -1;
	}

	V_OD_erg = SQLBindCol(V_OD_hstmt,2,SQL_C_CHAR,outputStr1,sizeof(outputStr1), (int*)&outputStr1len);  
	if (V_OD_erg != SQL_SUCCESS) {
		ErrPostEx(SEV_ERROR, 0, 0, "Error: unable to bind col 2. SQLRETURN=%ld", V_OD_erg);
		SQLFreeHandle(SQL_HANDLE_STMT, V_OD_hstmt); 
		return -1;
	}
	V_OD_erg = SQLBindCol(V_OD_hstmt,3,SQL_C_CHAR,outputStr2,sizeof(outputStr2), (int*)&outputStr2len);  
	if (V_OD_erg != SQL_SUCCESS) {
		ErrPostEx(SEV_ERROR, 0, 0, "Error: unable to bind col 3. SQLRETURN=%ld", V_OD_erg);
		SQLFreeHandle(SQL_HANDLE_STMT, V_OD_hstmt); 
		return -1;
	}
	V_OD_erg = SQLBindCol(V_OD_hstmt,4,SQL_C_CHAR,outputStr3,sizeof(outputStr3), (int*)&outputStr3len);  
	if (V_OD_erg != SQL_SUCCESS) {
		ErrPostEx(SEV_ERROR, 0, 0, "Error: unable to bind col 4. SQLRETURN=%ld", V_OD_erg);
		SQLFreeHandle(SQL_HANDLE_STMT, V_OD_hstmt); 
		return -1;
	}
	V_OD_erg = SQLBindCol(V_OD_hstmt,5,SQL_C_LONG,&length,0, NULL);  
	if (V_OD_erg != SQL_SUCCESS) {
		ErrPostEx(SEV_ERROR, 0, 0, "Error: unable to bind col 5. SQLRETURN=%ld",V_OD_erg);
		SQLFreeHandle(SQL_HANDLE_STMT, V_OD_hstmt); 
		return -1;
	}
	V_OD_erg = SQLBindCol(V_OD_hstmt,6,SQL_C_CHAR,outputStr4,sizeof(outputStr4), (int*)&outputStr4len);  
	if (V_OD_erg != SQL_SUCCESS) {
		ErrPostEx(SEV_ERROR, 0, 0, "Error: unable to bind col 6. SQLRETURN=%ld", V_OD_erg);
		SQLFreeHandle(SQL_HANDLE_STMT, V_OD_hstmt); 
		return -1;
	}
		
	/* fetch data */
	while ((V_OD_erg = SQLFetch(V_OD_hstmt)) == SQL_SUCCESS || V_OD_erg == SQL_SUCCESS_WITH_INFO){
		fetchCount++;
		if (fetchCount>1) {
			domnamep->next = StDomNamedbNew();
			domnamep = domnamep->next;
		}

		domnamep->pssmid = pssmid;
		domnamep->accession = StringSave(outputStr1);
		domnamep->name = StringSave(outputStr2);
		domnamep->description = StringSave(outputStr3);
		domnamep->length = length;
		if (outputStr4len > 0)
			domnamep->pdb_id = StringSave(outputStr4);
		else
			domnamep->pdb_id = NULL;
	}

	/* Free statement handle. */
	SQLFreeStmt(V_OD_hstmt, SQL_DROP); 

	if (fetchCount == 0)
		return -1;

	return 1;
}

/******************
* delete all records from table DOMNAME
*******************/
Boolean DeleteAllRecordsInDOMNAME()
{
	SQLHANDLE hstmt;
	Int2 sql_return = SQL_SUCCESS;

	CharPtr delete_stmt = (char *) malloc(sizeof(Char) * 150);
	if (delete_stmt == NULL)
	{
		ErrPostEx(SEV_ERROR, 0, 0, "[DeleteAllRecordsInDOMNAME]: Could not allocate space");
		free(delete_stmt);
	return FALSE;
	}
	sprintf((char *)delete_stmt, "delete from %s.domname", database);

	if (CreateHandle(&hstmt, delete_stmt, hdbc, &henv) != SQL_SUCCESS)
	{
	ErrPostEx(SEV_ERROR, 0, 0, "[DeleteAllRecordsInDOMDB]: Could not create handle");
	free(delete_stmt);
	return FALSE;
	}
	free(delete_stmt);

	sql_return = SQLExecute(hstmt);
	/* execute statment */
	if (sql_return != SQL_SUCCESS)
	{
		ErrPostEx(SEV_ERROR,0,0, "[DeleteAllRecordsInDOMNAME]:unable to execute delete statement.");
		LogSQLInfo(hstmt);
		SQLFreeStmt(hstmt, SQL_DROP);
		return FALSE;
	}

	SQLFreeStmt(hstmt, SQL_DROP);
	return TRUE;
}




/******************
* Reset functions
* note: not needed in ODBC
******************/

/*Reset RPSDB record */
void ResetStRPSDB (StRpsdbPtr PNTR pprs)
{
	if((*pprs)==NULL)
	{
		ErrPostEx(SEV_ERROR, 0,0,"ResetStRPSDB: structure is null");
		return;
	}
	(*pprs)->gi=0;
	(*pprs)->dom_id=NULL;
	(*pprs)->from=0;
	(*pprs)->evalue=0;
	(*pprs)->n_missing=0;
	(*pprs)->c_missing=0;
	(*pprs)->len=0;
	(*pprs)->num_of_dom=0;
	(*pprs)->cdd_id=0;
	(*pprs)->score=0;
	(*pprs)->bitscore=0.0;
	(*pprs)->next=NULL;
	return;
}

/*Reset DOMNAME record*/
void ResetStDOMNAME (StDomNamedbPtr PNTR ppdn)
{
	if((*ppdn)==NULL)
	{
		ErrPostEx(SEV_ERROR, 0,0,"ResetStRPSDB: structure is null");
		return;
	}
	(*ppdn)->pssmid=0;
	(*ppdn)->accession=NULL;
	(*ppdn)->name=NULL;
	(*ppdn)->description=NULL;
	(*ppdn)->length=0;
	(*ppdn)->pdb_id=NULL;
	(*ppdn)->next=NULL;
	return;
}

/*************************************
* Codebase open and close calls
* note: not needed in ODBC
**************************************/
/*Close RPS database*/

Boolean CloseRPSDB(void)
{
	return TRUE;
}

/*Close DOMNAME database*/
Boolean CloseDOMNAME(void)
{
	return TRUE;
}

/*Open RPS database*/
Boolean OpenRPSDB( CharPtr name, Boolean create)
{
	return TRUE;
}

/*Open domain name database*/
Boolean OpenDOMNAME(CharPtr name, Boolean create)
{
	return TRUE;
}


/***********************
*Print Functions
***********************/

/*Print RPSDB */
Boolean PrintRPSDB(void)
{
	return TRUE;
}

/*Print Domain Name table*/
Boolean PrintDOMNAME(void)
{
	return TRUE;
}

/***********************
* Utilities functions
***********************/

/* Utility function to read Cdd asn1*/
CddPtr GetCddFROMRDOMNAME(void)
{
	/* not needed in ODBC */
	return NULL;
}

void LIBCALL PackRPSDB(void) {
	return;
}

/*
 $Log: rpsdb_odbc.c,v $
 Revision 1.13  2005/04/25 15:43:55  ryao
 fixed memory free error in SearchRPSDB. bug [5425] related

 Revision 1.12  2005/04/11 21:02:50  zwang
 Change according to schema. See bug 3974.

 Revision 1.11  2004/12/14 20:03:31  zwang
 Added transaction related functions.

 Revision 1.10  2004/08/19 20:35:18  zwang
 Rewrite SearchDOMNAME. Use CddAsnWrite in AppendRecordDOMNAME.

 Revision 1.9  2004/08/06 14:47:22  mmatan
 Changed CharPtr database[50] to SQLCHAR database[50]

 Revision 1.8  2004/08/05 16:20:21  zwang
 Remove DB2 related code.

 Revision 1.7  2004/05/13 17:32:53  zwang
 Added: search rpsdb on domid.

 Revision 1.6  2004/05/13 15:29:35  zwang
 Make select on explicit field names (an extra column 'rowid' is added to table).

 Revision 1.5  2004/02/27 17:21:24  zwang
 Got rid of warnings of SQLBindCol.

 Revision 1.4  2004/01/22 16:01:13  zwang
 Remove SHoundIsInited since it is intrez.h/c

*/
