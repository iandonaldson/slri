/* $Id: hist_odbc.c,v 1.11 2005/03/15 21:22:42 eburgess Exp $
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


  PROGRAM:


  AUTHORS:
            Zhe Wang


  REVISION/CONTRIBUTION NOTES:

  March 1, 2001  Original MSH Revision.
                Hogue Lab - University of Toronto Biochemistry
                Department and the Samuel Lunenfeld Research
                Institute, Mount Sinai Hospital
                http://bioinfo.mshri.on.ca  hogue@mshri.on.ca
                ftp://ftp.mshri.on.ca/


  PUBLICATION TO CITE:

  DESCRIPTION:
  Part of SeqHound

****************************************************************************/
#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>

#include <intrez_odbc.h>
#include <hist_odbc.h>
#include <slri_odbc.h>
#include <slri_misc.h>
#include <objslristruc.h>

/***************************************************************************************
**	Global variables used for all functions
***************************************************************************************/

/* ODBC variables used in this file */

/* henv and hdbc defined in slri_odbc.h */

static SQLHANDLE HNDLHistdb;

/* handle error message */
static SQLRETURN                V_OD_erg;                      /* result of functions */

static SQLCHAR                  V_OD_stat[SQL_SQLSTATE_SIZE +1];                 /* Status SQL */
static SQLCHAR                  V_OD_msg[SQL_MAX_MESSAGE_LENGTH + 1];
static SQLINTEGER               V_OD_err;
static SQLSMALLINT              V_OD_mlen;

/*
 * global stuct used in SQLBindParameter()
 *  so these parameters only bind once
 */

/* for table histdb */
static struct
{
	SQLINTEGER gi;
    SQLINTEGER oldgi;
	SQLINTEGER version;
	SQLINTEGER action;
}histdb;

static struct
{
	Char val[20];
	Int4 len;
}histdb_access;

static struct
{
	Char val[20];
	Int4 len;
}histdb_date;

static struct
{
	Char val[80];
	Int4 len;
}histdb_filename;

/* SeqHound Global Variables */
SQLCHAR database[50];

/**************************************************
 *       PUBLIC CODE BASE INITIALIZTION           *
 **************************************************/

/* these functions initialize the codebase tags and field
   information.  Alter these only if you know what you are doing */

/* initializes database.
   This function must be called in any application that uses
   this database, prior to doing anything to the database.*/
void HIST_InitCodeBase(Boolean bConnect)
{
	HIST_ConnectToDB(bConnect);
}

/* closes the databases.  This function must called at the
   end of your function, if you called LL_InitCodeBase.*/
void HIST_InitUndoCodeBase(void)
{
	HIST_DisconnectDB(TRUE);
}

/*
** forms a connection to the database and set up handles for the
** rps inserts.
**
** returns TRUE if connection is successful, FALSE otherwise
*/
Boolean
HIST_ConnectToDB(Boolean bConnect)
{

   if (bConnect) {
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
      ErrPostEx(SEV_ERROR, 0, 0, "[HIST_ConnectToDB]: Could not find username in initialization file.");
      exit(1);
   }
   if( GetAppParam("intrez", "datab", "password", NULL, (Char*)passwd, sizeof(char) * 10) <= 0 )
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[HIST_ConnectToDB]: Could not find pasword in initialization file.");
      exit(1);
   }
   if( GetAppParam("intrez", "datab", "dsn", NULL, (Char*)dsn, sizeof(char) * 10) <= 0 )
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[HIST_ConnectToDB]: Could not find dsn in initialization file.");
      exit(1);
   }

   /* database is a global parameter, we will use it when we initialize the sql statements,
      this way, we don't hardcode the database into the source code, allowing us to
      change the instance of the database by specifying it in the .intrezrc file, this
      is illustrated in each of the InitXXX functions below. */
   if( GetAppParam("intrez", "datab", "database", NULL, (Char*)database, sizeof(char) * 10) <= 0 )
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[HIST_ConnectToDb]: Could not find database in initialization file.");
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
   } /* if (bConnect) */

    /* get here then connection to database successful */
	/* set up all handles for inserting */
   if (InitBindParameterforInsert() != TRUE)
   {
      ErrPostEx(SEV_ERROR,0,0, "[HIST_ConnectToDB]: Cannot InitBindParameterforInsert. SQLRETURN=%ld", V_OD_erg);
      exit(1);
   }
    return TRUE;

}


static Boolean InitBindParameterforInsert()
{

   CharPtr pSQL_stmt = (char *) malloc(sizeof(Char) * 150);
   if (pSQL_stmt == NULL)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[InitBindParameterforInsert]: Could not allocate space");
     free(pSQL_stmt);
     return FALSE;
   }

   /* table RPSDB */
   sprintf((char *)pSQL_stmt, "INSERT into %s.histdb(gi, "
		   "oldgi, access, version, "
		   "date, action, filename) values(?,?,?,?,?,?,?)",
	        database);

   V_OD_erg = CreateHandle(&HNDLHistdb, pSQL_stmt,hdbc,&henv);
   if (V_OD_erg != SQL_SUCCESS) {
      ErrPostEx(SEV_ERROR, 0, 0, "Error CreateHandle for histdb");
      free(pSQL_stmt);
	  return FALSE;
   }
   free(pSQL_stmt);

  	V_OD_erg = SQLBindParameter(HNDLHistdb, 1, SQL_PARAM_INPUT,
		SQL_C_LONG,SQL_INTEGER, 0, 0, &histdb.gi, 0, NULL);
   if (V_OD_erg != SQL_SUCCESS) {
      ErrPostEx(SEV_ERROR, 0, 0, "Error: unable to bind parameter 1 "
				"for histdb.");
	  return FALSE;
   }

   V_OD_erg = SQLBindParameter(HNDLHistdb, 2, SQL_PARAM_INPUT,
		SQL_C_LONG,SQL_INTEGER, 0, 0, &histdb.oldgi, 0, NULL);
   if (V_OD_erg != SQL_SUCCESS) {
      ErrPostEx(SEV_ERROR, 0, 0, "Error: unable to bind parameter 2 "
				"for histdb.");
	  return FALSE;
   }

  	V_OD_erg = SQLBindParameter(HNDLHistdb, 3, SQL_PARAM_INPUT,
		SQL_C_CHAR,SQL_VARCHAR, 0, 0, histdb_access.val, 
								sizeof(histdb_access.val), NULL);
   if (V_OD_erg != SQL_SUCCESS) {
      ErrPostEx(SEV_ERROR, 0, 0, "Error: unable to bind parameter 3 "
				"for histdb.");
	  return FALSE;
   }

  	V_OD_erg = SQLBindParameter(HNDLHistdb, 4, SQL_PARAM_INPUT,
		SQL_C_LONG,SQL_INTEGER, 0, 0, &histdb.version, 0, NULL);
   if (V_OD_erg != SQL_SUCCESS) {
      ErrPostEx(SEV_ERROR, 0, 0, "Error: unable to bind parameter 4 "
				"for histdb.");
	  return FALSE;
   }

  	V_OD_erg = SQLBindParameter(HNDLHistdb, 5, SQL_PARAM_INPUT,
		SQL_C_CHAR,SQL_VARCHAR, 0, 0, histdb_date.val, 
								sizeof(histdb_date.val), NULL);
   if (V_OD_erg != SQL_SUCCESS) {
      ErrPostEx(SEV_ERROR, 0, 0, "Error: unable to bind parameter 5 "
				"for histdb.");
	  return FALSE;
   }

  	V_OD_erg = SQLBindParameter(HNDLHistdb, 6, SQL_PARAM_INPUT,
		SQL_C_LONG,SQL_INTEGER, 0, 0, &histdb.action, 0, NULL);
   if (V_OD_erg != SQL_SUCCESS) {
      ErrPostEx(SEV_ERROR, 0, 0, "Error: unable to bind parameter 6 "
				"for histdb.");
	  return FALSE;
   }

   V_OD_erg = SQLBindParameter(HNDLHistdb, 7, SQL_PARAM_INPUT,
		SQL_C_CHAR,SQL_VARCHAR, 0, 0, histdb_filename.val, 
							   sizeof(histdb_filename.val), NULL);
   if (V_OD_erg != SQL_SUCCESS) {
      ErrPostEx(SEV_ERROR, 0, 0, "Error: unable to bind parameter 7 "
				"for histdb.");
	  return FALSE;
   }
   return TRUE;
}


/*
** closes the databases and free the environment handles.
**
**  This function must called at the
**   end of your function, if you called HIST_InitCodeBase.
**
** returns TRUE if disconnection is successful, FALSE otherwise
**
*/
Boolean
HIST_DisconnectDB(Boolean bConnect)
{
	/* free the statment handle */
	SQLFreeStmt(HNDLHistdb, SQL_RESET_PARAMS);

   if (bConnect) {
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
   } /* if (bConnect) */
	return TRUE;
}

/***************************************************
           PRIVATE OPEN DATABASE FUNCTIONS
****************************************************/
/* opens and sets the fields and tags of HISTDB code base files */
static Boolean OpenHISTdb(CharPtr path, Boolean CREATE)
{
  return TRUE;
}


/********************************************************
             PUBLIC OPEN DATABASE FUNCTIONS
*********************************************************/

/* opens all the databases associated with Histdb.  This
   function must be called prior to accessing the database. */
Boolean HIST_OpenAllDBs(Boolean CREATE)
{
	HIST_InitCodeBase(CREATE);
	return TRUE;
}

Boolean OpenHISTDB(Boolean CREATE)
{
  HIST_InitCodeBase(CREATE);
	return TRUE;
}


/********************************************************
             PUBLIC CLOSE DATABASE FUNCTIONS
*********************************************************/
void CloseCodeBaseAndHISTDBs(void)
{
	HIST_DisconnectDB(TRUE);
}

void CloseAllHISTDBs(void)
{
	HIST_DisconnectDB(FALSE);
}

/* find the number of records in table HISTDB */
static Int4 GetRecNoHISTDB(void) {

   CharPtr search_histdb = (char *) malloc(sizeof(Char) * 150);
   if (search_histdb == NULL)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[GetRecNoHISTDB]: Could not allocate space");
     free(search_histdb);
     return 0;
   }
   sprintf((char *)search_histdb, "SELECT count(*) as count from %s.histdb", database);

   SQLHANDLE hstmt;
    Int2 sqlrc=SQL_SUCCESS;
    struct
    {
       SQLINTEGER len;
       SQLINTEGER val;
    }count;

   /* 0 returned for count(*) if the table is empty */
   /* initialize them to avoid a weird number */
	count.val=0; count.len=0;

    sqlrc = CreateHandle(&hstmt, search_histdb, hdbc, &henv);
   if (sqlrc != SQL_SUCCESS)
   {
	  ErrPostEx(SEV_ERROR,0,0, "[GetRecNoHISTDB]:unable to create handle.");
      free(search_histdb);
      return 0;
   }
   free(search_histdb);


   /* set auto commit on */
   SQLSetConnectAttr( hdbc, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_ON, SQL_NTS);

   /* execute statment */
   sqlrc = SQLExecute(hstmt);
   if (sqlrc != SQL_SUCCESS)
   {
	  ErrPostEx(SEV_ERROR,0,0, "[GetRecNoHISTDB]:unable to execute select statement.");
	  LogSQLInfo(hstmt);
      SQLFreeStmt(hstmt, SQL_DROP);
      return 0;
   }

   sqlrc = SQLBindCol(hstmt,1,SQL_C_SLONG,&count.val,0,(int*)&count.len);
   if (sqlrc != SQL_SUCCESS)
   {
	  ErrPostEx(SEV_ERROR,0,0, "[GetRecNoHISTDB]:unable to bind col 1.");
      SQLFreeStmt(hstmt, SQL_DROP);
      return 0;
   }

   sqlrc = SQLFetch(hstmt);
   if(sqlrc == SQL_NO_DATA_FOUND){
      ErrPostEx(SEV_INFO, 0, 0, "[GetRecNoHISTDB]: No data found");
      SQLFreeStmt(hstmt, SQL_DROP);
      return 0;
   }

   if(sqlrc == SQL_SUCCESS || sqlrc == SQL_SUCCESS_WITH_INFO)
   {
      SQLFreeStmt(hstmt, SQL_DROP);
      return count.val;
   }

    SQLFreeStmt(hstmt, SQL_DROP);
    return -1;
}



/* parses from ACCDB and extracts the GI, accesion & version
   and fills the HISTDB.
*/
Boolean LIBCALL Extract_ACCDB(void)
{
  Int4 ginum=0;
  StHistdbNodePtr pHis = NULL;
  NCBI_DatePtr pDate = NULL;
  Int4 recnum_histdb = 0;
  Int4 recnum_accdb = 0;
  Int4 target = 1;

  /* count the number of records in ACCDB */
  recnum_accdb = GetRecNoACCDB();

  /* count the number of records in HISTDB */
  recnum_histdb = GetRecNoHISTDB();
  if (recnum_histdb >= 1)
  {

	  /* HISTDB has more records than ACCDB, so upto date */
	  if (recnum_histdb > recnum_accdb) {
			ErrPostEx(SEV_ERROR,0,0, "[Extract_ACCDB]: Number of records in ACCDB is lower than in HISTDB.");
			return FALSE;
	  }
	  /* upto date */
	  else if (recnum_histdb == recnum_accdb)
	  {
			ErrPostEx(SEV_ERROR,0,0, "[Extract_ACCDB]: Nothing to do. Number of records in ACCDB is the same as in HISTDB.");
			return FALSE;
	  }
	  else
			target = recnum_histdb+1;

  }

  pDate = DateCurr();

  ginum=target;
  while (ginum<=recnum_accdb) {
        StAccdbPtr pAccdb=NULL;

        pHis = StHistdbNodeNew();
       
       /* get the date from the system using an NCBI function
       and then convert it to a string format YYYMMDD */
        pHis->date = GetStringDate(pDate);

        pHis->action = ACTION_ADDED;

	pAccdb = NULL;
	pAccdb = GetRecACCDB(ginum);
	ginum++;
	if (pAccdb == NULL) {
	  ErrPostEx(SEV_ERROR,0,0, "[Extract_ACCDB]: NULL for %ldth record in ACCDB.", ginum);
	  continue;
	}
	pHis->gi = pAccdb->gi;
	pHis->oldgi = 0;
	pHis->version = pAccdb->version;
	pHis->access = pAccdb->access;
	pHis->filename = "ACCDB";

	AppendRecordHISTDB(pHis);
	StHistdbNodeFree(pHis);
	StAccdbFree(pAccdb);
  }

  DateFree(pDate);
  return TRUE;

}

/* converts a string of format YYYYMMDD into a NCIB_DatePtr object.
   The calling function must free the object.
*/
NCBI_DatePtr LIBCALL RetrieveDate(CharPtr date){
  char year[5] = {0};
  char month[3] = {0};
  char day[3] = {0};
  char *p = date;
  NCBI_DatePtr pdate = DateNew();

  strncpy(year, date, 4 * sizeof(char)); p+= 4;
  strncpy(month, p, 2 * sizeof(char)); p += 2;
  strncpy(day, p, 2 * sizeof(char)); p += 2;
  DateWrite(pdate, atoi(year), atoi(month), atoi(day), NULL);
  return pdate;
}


/* adds a record to HISTDB */
Boolean LIBCALL AppendRecordHISTDB(StHistdbPtr pHis)
{
  SQLRETURN retcode;
   if(pHis == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, "[AppendRecordHISTDB]: "
				"NULL parameter.");
      return FALSE;
   }

  if(strstr(pHis->access, "n/a") != NULL)
    return TRUE;

   histdb.gi = pHis->gi;
   histdb.oldgi = pHis->oldgi;
   sprintf((char *)histdb_access.val,"%s", pHis->access);
   histdb.version = pHis->version;
   sprintf((char *)histdb_date.val,"%s", pHis->date);
   sprintf( (char *)histdb_filename.val, "%s", pHis->filename);
   histdb.action = pHis->action;

  /* execute statment */
   retcode = SQLExecute(HNDLHistdb);
   if ( (retcode != SQL_SUCCESS) && 
	    (retcode != SQL_SUCCESS_WITH_INFO) )
   {
	  ErrPostEx(SEV_ERROR,0,0, "[AppendRecordHISTDB]: "
				"unable to execute histdb insert statement. (gi=%ld)",histdb.gi);
	  LogSQLInfo(HNDLHistdb);
      return FALSE;
   }

  return TRUE;
}

/* searches for all records with the action
   between (inclusive) the 2 dates.
   date_from must preceed date_to. Caller has to free both dates.
*/
StHistdbNodePtr GetAll(Int4 ACTION, NCBI_DatePtr date_from, NCBI_DatePtr date_to)
{
  Int2 i;
  SQLHANDLE hstmt;               /* select statement handle */
  SQLINTEGER retcode;
  StHistdbNodePtr pHis = NULL;
  StHistdbNodePtr pPrev = NULL, pCur = NULL;
  CharPtr key = (CharPtr) MemNew(25*sizeof(Char));
  Int4 rc = 0;
  NCBI_DatePtr pdate = NULL;

  /* select all records with action==ACTION */
   CharPtr pSQL_stmt = (char*) malloc(sizeof(char) * 150);
   if(pSQL_stmt == NULL)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[GetAll]: Could not allocate space");
      return NULL;
   }
   sprintf((char*)pSQL_stmt, "select gi, acc, version, date, action from %s.histdb where action=?;", database);

   /* create handle to statement, once handle exists we can free char* */
   if(CreateHandle(&hstmt, pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[GetAll]: Could not create handle.");
      free(pSQL_stmt);
      return NULL;
   }
   free(pSQL_stmt);

   SQLINTEGER iKey=ACTION;
   retcode = SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &iKey, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[GetAll]: Could not bind parameter");
	  SQLFreeStmt(hstmt, SQL_DROP);
      return NULL;
   }

   /* execute the select statement */
   retcode = SQLExecute(hstmt);
   if (retcode != SQL_SUCCESS)
   {
	  ErrPostEx(SEV_ERROR,0,0, "[GetAll]: Could not execute select statement.");
	  LogSQLInfo(hstmt);
      SQLFreeStmt(hstmt, SQL_DROP);
      return NULL;
   }

   /* bind return colums */
   retcode = SQLBindCol(hstmt, 1, SQL_C_SLONG, &histdb.gi, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[GetAll]: Could not bind column 1.");
	 SQLFreeStmt(hstmt, SQL_DROP);
     return NULL;
   }
   retcode = SQLBindCol(hstmt, 2, SQL_C_CHAR, histdb_access.val, 
						20, (int*)&histdb_access.len);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[GetAll]: Could not bind column 2.");
	 SQLFreeStmt(hstmt, SQL_DROP);
     return NULL;
   }
   retcode = SQLBindCol(hstmt, 3, SQL_C_SLONG, &histdb.version, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[GetAll]: Could not bind column 3.");
	 SQLFreeStmt(hstmt, SQL_DROP);
     return NULL;
   }
   retcode = SQLBindCol(hstmt, 4, SQL_C_CHAR, histdb_date.val, 8, (int*)&histdb_date.len);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[GetAll]: Could not bind column 4.");
	 SQLFreeStmt(hstmt, SQL_DROP);
     return NULL;
   }
   retcode = SQLBindCol(hstmt, 5, SQL_C_SLONG, &histdb.action, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[GetAll]: Could not bind column 5.");
	 SQLFreeStmt(hstmt, SQL_DROP);
     return NULL;
   }


  if(date_from == NULL)
  {
    date_from = DateCurr();
  }
  if(date_to == NULL)
  {
    date_to = DateCurr();
  }
  i = DateMatch(date_from, date_to, FALSE);
  if( i == -1 || i == 0 )
  {
     /* fetch results and save in your linked list */
     SQLINTEGER fetchCount = 0;
     /* fetch result and save in the linked list */
     while((retcode=SQLFetch(hstmt)) == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
	 {
		 pdate = RetrieveDate((char*)histdb_date.val);
         /* get the record's date and compare with date_from */
		 i = DateMatch(date_from, pdate, FALSE);
		 if (i == -1 || i == 0)
		 {
             /* the date_from preceeds or is the same date as the records
	         so compare with date_to */
			 i = DateMatch(pdate, date_to, FALSE);
			 if (i == -1 || i == 0)
			 {
				 fetchCount++;
				 pCur = StHistdbNodeNew();
				 pCur->access = (CharPtr) MemNew(25*sizeof(Char));
				 pCur->gi = histdb.gi;
				 strcpy(pCur->access, (char*)histdb_access.val);
				 pCur->version = histdb.version;
				 DatePrint(pdate, key);
				 pCur->date = (CharPtr) MemNew(25*sizeof(Char));
				 strcpy(pCur->date, key);
				 if(pHis == NULL) pHis = pCur;
				 else pPrev->next = pCur;
				 pPrev = pCur;

			 }

		 }
	 }
  }


  DateFree(pdate);
  MemFree(key);
  return pHis;
}

void LIBCALL PrintStHist(StHistdbNodePtr pHis){
  StHistdbNodePtr pCur = NULL;
  for(pCur = pHis; pCur != NULL; pCur = pCur->next){
    printf("%d\t%s\t%d\t%s\n", pCur->gi, pCur->access, pCur->version, pCur->date);
  }
}

Boolean LIBCALL SearchHISTDB(StHistdbNodePtr pHis)
{
	SQLRETURN retcode;
    SQLHANDLE hstmt;
	SQLINTEGER iKey;

   StHistdbNodePtr pTmp;
   if(pHis == NULL)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[SearchHISTDB]: Null StHistdbNodePtr.");
      return FALSE;
   }

   pTmp = pHis;

   if(pHis->gi < 1)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[SearchHISTDB]: Invalid keys.");
      return FALSE;
   }

  /* allocate space for the select statement */
   CharPtr  pSQL_stmt = (char*) malloc(sizeof(char) * 150);
   if(pSQL_stmt == NULL)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[SearchHISTDB]: Could not allocate space");
      return FALSE;
   }


   /* fill in the select statement based on the search key passed in.
      database is a global variable initialized in ConnectToDb. The
      value of database is set in the ini file. eg database = seqhound. */
   if (pHis->gi > 0)
   {
     iKey = pHis->gi;
     sprintf((char*)pSQL_stmt, "select gi, acc, version, date, action from %s.histdb where gi=?;", database);
   } else{
      ErrPostEx(SEV_ERROR, 0, 0, "[SearchHISTDB]: No search key found.");
      free(pSQL_stmt);
      return FALSE;
   }

   /* create handle to statement, once handle exists we can free char* */
   if(CreateHandle(&hstmt, pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[SearchHISTDB]: Could not create handle.");
      free(pSQL_stmt);
      return FALSE;
   }
   free(pSQL_stmt);

   retcode = SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &iKey, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[SearchHISTDB]: Could not bind parameter 1");
	  SQLFreeStmt(hstmt, SQL_DROP);
      return FALSE;
   }

   /* execute the select statement */
   retcode = SQLExecute(hstmt);
   if (retcode != SQL_SUCCESS)
   {
	  ErrPostEx(SEV_ERROR,0,0, "[SearchHISTDB]: Could not execute select statement.");
	  LogSQLInfo(hstmt);
      SQLFreeStmt(hstmt, SQL_DROP);
      return FALSE;
   }

   /* bind return colums */
   retcode = SQLBindCol(hstmt, 1, SQL_C_SLONG, &histdb.gi, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[SearchHISTDB]: Could not bind column 1.");
	 SQLFreeStmt(hstmt, SQL_DROP);
     return FALSE;
   }
   retcode = SQLBindCol(hstmt, 2, SQL_C_CHAR, histdb_access.val, 20, (int*)&histdb_access.len);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[SearchHISTDB]: Could not bind column 2.");
	 SQLFreeStmt(hstmt, SQL_DROP);
     return FALSE;
   }
   retcode = SQLBindCol(hstmt, 3, SQL_C_SLONG, &histdb.version, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[SearchHISTDB]: Could not bind column 3.");
	 SQLFreeStmt(hstmt, SQL_DROP);
     return FALSE;
   }
   retcode = SQLBindCol(hstmt, 4, SQL_C_CHAR, histdb_date.val, 8, (int*)&histdb_date.len);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[SearchHISTDB]: Could not bind column 4.");
	 SQLFreeStmt(hstmt, SQL_DROP);
     return FALSE;
   }
   retcode = SQLBindCol(hstmt, 5, SQL_C_SLONG, &histdb.action, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[SearchHISTDB]: Could not bind column 5.");
	 SQLFreeStmt(hstmt, SQL_DROP);
     return FALSE;
   }

   SQLINTEGER fetchCount = 0;
   /* fetch result and save in the linked list */
   while((retcode=SQLFetch(hstmt)) == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
   {
	   pTmp = pHis;

      if(strstr((char*)(histdb_access.val), "n/a") != NULL) continue;

      pTmp->access = (char *)malloc((strlen((char*)histdb_access.val)+1) * sizeof(char));
      strcpy(pTmp->access, (char*)histdb_access.val);
      pTmp->gi = histdb.gi;
      pTmp->version = histdb.version;
      pTmp->action = histdb.action;
      pTmp->date = (char*) malloc((strlen((char*)histdb_date.val)+1) * sizeof(char));
      strcpy(pTmp->date, (char*)histdb_date.val);
      /* printf("%s %d %d %d %s\n", pHis->access, pHis->gi, pHis->version, pHis->action, pHis->date); */
      return TRUE;
   }

   /* CloseAllHISTDBs(); */
   return FALSE;
}


/*
$Log: hist_odbc.c,v $
Revision 1.11  2005/03/15 21:22:42  eburgess
Removed printf statement from InitBindParameterForInsert().

Revision 1.10  2005/03/09 17:00:00  eburgess
Added oldgi and filename to histdb. Changed the field acc to access in
histdb. Modified AppendRecordHISTDB and other functions accordingly. Bug 5049

Revision 1.9  2004/08/06 14:47:21  mmatan
Changed CharPtr database[50] to SQLCHAR database[50]

Revision 1.8  2004/08/05 16:20:20  zwang
Remove DB2 related code.

Revision 1.7  2004/07/22 20:53:14  zwang
Added flag to specify if a connection to database should be established.

Revision 1.6  2004/03/31 19:26:44  zwang
Create and free structure in the loop to avoid memory leak.

Revision 1.5  2004/03/16 19:24:40  zwang
Fixed bugs in Char and CharPtr.

Revision 1.4  2004/02/27 17:23:27  zwang
Changed return value.

Revision 1.3  2004/02/11 21:34:08  eburgess
Added HIST_InitCodeBase() to OpenHISTDB(), which is called in update.c to
open histdb.

Revision 1.2  2004/02/11 19:08:05  zwang
Fixed bugs in extracting data from ACCDB. Got rid of warning regarding SQL int in SQLBindCol.

Revision 1.1  2004/01/13 21:26:45  zwang
New file for Update in ODBC

*/
