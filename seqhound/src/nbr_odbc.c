/* $Id: nbr_odbc.c,v 1.13 2005/04/26 14:29:33 rcavero Exp $
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


  PROGRAM:  SeqHound Neighbours:  Database Layer (ODBC)

  AUTHORS:  Marc Dumontier (mdumontier@mdsproteomics.com)
            and Christopher W.V. Hogue  (hogue@mshri.on.ca)


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

#include <nbr_odbc.h>

/* henv and hdbc defined in intrez_odbc.h */

static SQLHANDLE hstmt_nblastdb, countNeighboursStmt, maxRowidNeighboursStmt,
				hstmt_nblastdb_gi1, hstmt_nblastdb_gi2, hstmt_nblastdb_rowid,
				hstmt_blastdb_uid;

/* handle error message */
static SQLRETURN                V_OD_erg;                      /* result of functions */

static SQLINTEGER               V_OD_err;
static SQLINTEGER               key1, key2;


SQLCHAR database[50];

/*** for testing **/
static Boolean 			testing = TRUE;



static void InitNPBlastDBFields(void)
{
}

static void InitNPDBFields(void)
{
}

static void InitND3Fields(void)
{
}

Boolean OpenNPBlastDB(Boolean CREATE)
{
	return TRUE;
}

Boolean OpenNPDB(Boolean CREATE)
{
	return TRUE;
}

Boolean OpenN3D(Boolean CREATE)
{
	return TRUE;
}

Boolean OpenAllNbrDBs(Boolean CREATE)
{
	return (OpenCodeBaseNbr(CREATE));
}

void InitCodeBaseNbr(void)
{
}

Boolean OpenCodeBaseNbr(Boolean CREATE)
{
   if (CREATE) {
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
      ErrPostEx(SEV_ERROR, 0, 0, "[OpenCodeBaseNbr]: Could not find username in initialization file.");
      return FALSE;
   }
   if( GetAppParam("intrez", "datab", "password", NULL, (Char*)passwd, sizeof(char) * 10) <= 0 )
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[OpenCodeBaseNbr]: Could not find pasword in initialization file.");
      return FALSE;
   }
   if( GetAppParam("intrez", "datab", "dsn", NULL, (Char*)dsn, sizeof(char) * 10) <= 0 )
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[OpenCodeBaseNbr]: Could not find dsn in initialization file.");
      return FALSE;
   }

   /* database is a global parameter, we will use it when we initialize the sql statements,
      this way, we don't hardcode the database into the source code, allowing us to
      change the instance of the database by specifying it in the .intrezrc file, this
      is illustrated in each of the InitXXX functions below. */
   if( GetAppParam("intrez", "datab", "database", NULL, (Char*)database, sizeof(char) * 10) <= 0 )
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[OpenCodeBaseNbr]: Could not find database in initialization file.");
      return FALSE;
   }
   /* make sure the proper DB2 instance variable is defined--only for aix system */
   /* sprintf(instancebuf,"DB2INSTANCE=db2inst1",instance);
   putenv(instancebuf); */

   /* 1. allocate Environment handle and register version */
   V_OD_erg = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);
   if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO))
   {
      ErrPostEx(SEV_ERROR,0,0, "[OpenCodeBaseNbr]: Unable to allocate environment handle. SQLRETURN=%ld", V_OD_erg);
      exit(1);
   }

   /* 2. Declaring ODBC Version only for 3.x */
   V_OD_erg = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0);
   if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO))
   {
      ErrPostEx(SEV_ERROR,0,0, "[OpenCodeBaseNbr]: Unable to setup ODBC Verison to 3.0. SQLRETURN=%ld", V_OD_erg);
      exit(1);
   }

   /* 3. allocate connection handle. */
   V_OD_erg = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);
   if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO))
   {
      ErrPostEx(SEV_ERROR,0,0, "[OpenCodeBaseNbr]: Cannot allocate connection handle. SQLRETURN=%ld", V_OD_erg);
      SQLFreeHandle(SQL_HANDLE_ENV, henv);
      exit(1);
   }

   /* 4. Set Connection Attributes: Set timeout. set autocommit off */
   V_OD_erg = SQLSetConnectAttr(hdbc, SQL_LOGIN_TIMEOUT, (SQLPOINTER *)5, 0);
   if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO))
   {
      ErrPostEx(SEV_ERROR,0,0, "[OpenCodeBaseNbr]: Cannot set connect attr. SQLRETURN=%ld", V_OD_erg);
      SQLFreeHandle(SQL_HANDLE_ENV, henv);
      exit(1);
   }

   /* 5. Connect to the datasource */
   V_OD_erg = SQLConnect(hdbc, dsn, SQL_NTS, usr, SQL_NTS, passwd, SQL_NTS);
   if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO))
   {
      ErrPostEx(SEV_ERROR,0,0, "[OpenCodeBaseNbr]: Connection to database failed. SQLRETURN=%ld", V_OD_erg);
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
         ErrPostEx(SEV_ERROR,0,0, "[OpenCodeBaseNbr]: failed to set autocommit off. SQLRETURN=%ld", V_OD_erg);
         SQLFreeHandle(SQL_HANDLE_ENV, henv);
         exit(1);
      }
	}
   } /* if(CREATE) */

   /* create SQL handles */
   CharPtr pSQL_stmt = (char *) malloc(sizeof(Char) * 150);
   if (pSQL_stmt == NULL)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[OpenCodeBaseNbr]: Could not allocate space");
     free(pSQL_stmt);
     return FALSE;
   }

   /* table nblastdb */
   sprintf((char *)pSQL_stmt, "SELECT ASNNBR,NUM from %s.nblastdb where gi = ?", database);
   V_OD_erg = CreateHandle(&hstmt_nblastdb,pSQL_stmt,hdbc,&henv);
   if (V_OD_erg != SQL_SUCCESS) {
      ErrPostEx(SEV_ERROR, 0, 0, "Error CreateHandle for nblastdb");
      free(pSQL_stmt);
	  return FALSE;
   }
   V_OD_erg = SQLBindParameter(hstmt_nblastdb, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &key1, 0, NULL);
   if(V_OD_erg != SQL_SUCCESS && V_OD_erg != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "Could not bind parameter");
      free(pSQL_stmt);
	  SQLFreeStmt(hstmt_nblastdb, SQL_DROP);
      return FALSE;
   }

   sprintf((char *)pSQL_stmt, "SELECT SUM(num) FROM %s.nblastdb", database);
   V_OD_erg = CreateHandle(&countNeighboursStmt,pSQL_stmt,hdbc,&henv);
   if (V_OD_erg != SQL_SUCCESS) {
      ErrPostEx(SEV_ERROR, 0, 0, "Error CreateHandle for nblastdb");
      free(pSQL_stmt);
	  return FALSE;
   }

   sprintf((char *)pSQL_stmt, "SELECT max(rowid) FROM %s.nblastdb", database);
   V_OD_erg = CreateHandle(&maxRowidNeighboursStmt,pSQL_stmt,hdbc,&henv);
   if (V_OD_erg != SQL_SUCCESS) {
      ErrPostEx(SEV_ERROR, 0, 0, "Error CreateHandle for nblastdb");
      free(pSQL_stmt);
	  return FALSE;
   }

   /* search in nblastdb for gi1 */
   sprintf((char *)pSQL_stmt, "SELECT ord FROM %s.nblastdb where gi=?", database);
   V_OD_erg = CreateHandle(&hstmt_nblastdb_gi1,pSQL_stmt,hdbc,&henv);
   if (V_OD_erg != SQL_SUCCESS) {
      ErrPostEx(SEV_ERROR, 0, 0, "Error CreateHandle for nblastdb search for gi1");
      free(pSQL_stmt);
	  return FALSE;
   }
   V_OD_erg = SQLBindParameter(hstmt_nblastdb_gi1, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &key1, 0, NULL);
   if(V_OD_erg != SQL_SUCCESS && V_OD_erg != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "Could not bind parameter");
      free(pSQL_stmt);
	  SQLFreeStmt(hstmt_nblastdb_gi1, SQL_DROP);
      return FALSE;
   }

   /* search in nblastdb for gi2 */
   V_OD_erg = CreateHandle(&hstmt_nblastdb_gi2,pSQL_stmt,hdbc,&henv);
   if (V_OD_erg != SQL_SUCCESS) {
      ErrPostEx(SEV_ERROR, 0, 0, "Error CreateHandle for nblastdb search for gi2");
	  return FALSE;
   }
   V_OD_erg = SQLBindParameter(hstmt_nblastdb_gi2, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &key2, 0, NULL);
   if(V_OD_erg!= SQL_SUCCESS && V_OD_erg != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "Could not bind parameter");
      free(pSQL_stmt);
	  SQLFreeStmt(hstmt_nblastdb_gi2, SQL_DROP);
      return FALSE;
   }

   /* search in nblastdb using rowid */
   sprintf((char *)pSQL_stmt, "SELECT gi, num FROM %s.nblastdb where rowid>=? and rowid<?", database);
   V_OD_erg = CreateHandle(&hstmt_nblastdb_rowid,pSQL_stmt,hdbc,&henv);
   if (V_OD_erg != SQL_SUCCESS) {
      ErrPostEx(SEV_ERROR, 0, 0, "Error CreateHandle for nblastdb search using rowid");
	  return FALSE;
   }
   V_OD_erg = SQLBindParameter(hstmt_nblastdb_rowid, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &key1, 0, NULL);
   if(V_OD_erg != SQL_SUCCESS && V_OD_erg != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "Could not bind parameter");
      free(pSQL_stmt);
	  SQLFreeStmt(hstmt_nblastdb_rowid, SQL_DROP);
      return FALSE;
   }
   V_OD_erg = SQLBindParameter(hstmt_nblastdb_rowid, 2, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &key2, 0, NULL);
   if(V_OD_erg != SQL_SUCCESS && V_OD_erg != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "Could not bind parameter");
      free(pSQL_stmt);
	  SQLFreeStmt(hstmt_nblastdb_rowid, SQL_DROP);
      return FALSE;
   }

   /* search in blastdb for uid */
   sprintf((char *)pSQL_stmt, "SELECT asnsa FROM %s.blastdb where uid=?", database);
   V_OD_erg = CreateHandle(&hstmt_blastdb_uid,pSQL_stmt,hdbc,&henv);
   if (V_OD_erg != SQL_SUCCESS) {
      ErrPostEx(SEV_ERROR, 0, 0, "Error CreateHandle for blastdb search for uid");
	  return FALSE;
   }
   V_OD_erg = SQLBindParameter(hstmt_blastdb_uid, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &key1, 0, NULL);
   if(V_OD_erg != SQL_SUCCESS && V_OD_erg != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "Could not bind parameter");
      free(pSQL_stmt);
	  SQLFreeStmt(hstmt_blastdb_uid, SQL_DROP);
      return FALSE;
   }

   free(pSQL_stmt);

   return (TRUE);
}


void CloseCodeBaseNbr(void)
{
}

/* Close the Blast and Neighbour protein databases*/
void CloseAllNbrDBs(Boolean bConnect)
{
   SQLRETURN sqlrc;

   if (SQLFreeHandle(SQL_HANDLE_STMT, hstmt_nblastdb) != SQL_SUCCESS)
       print_err(hstmt_nblastdb);
   if (SQLFreeHandle(SQL_HANDLE_STMT, countNeighboursStmt) != SQL_SUCCESS)
       print_err(countNeighboursStmt);
   if (SQLFreeHandle(SQL_HANDLE_STMT, maxRowidNeighboursStmt) != SQL_SUCCESS)
       print_err(maxRowidNeighboursStmt);
   if (SQLFreeHandle(SQL_HANDLE_STMT, hstmt_nblastdb_gi1) != SQL_SUCCESS)
       print_err(hstmt_nblastdb_gi1);
   if (SQLFreeHandle(SQL_HANDLE_STMT, hstmt_nblastdb_gi2) != SQL_SUCCESS)
       print_err(hstmt_nblastdb_gi2);
   if (SQLFreeHandle(SQL_HANDLE_STMT, hstmt_nblastdb_rowid) != SQL_SUCCESS)
       print_err(hstmt_nblastdb_rowid);
   if (SQLFreeHandle(SQL_HANDLE_STMT, hstmt_blastdb_uid) != SQL_SUCCESS)
       print_err(hstmt_blastdb_uid);

   if (bConnect) {
   /*Free the database handles used to communicate with the database*/
   sqlrc = SQLDisconnect(hdbc);
   if (sqlrc != SQL_SUCCESS && sqlrc != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "Could not disconnect from the database.");
   }

   /*free the connnection handle */
   sqlrc = SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
   if (sqlrc != SQL_SUCCESS && sqlrc != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "could not free the connection handle.");
   }

   /*free envirnment handle */
   sqlrc = SQLFreeHandle(SQL_HANDLE_ENV, henv);
   if (sqlrc != SQL_SUCCESS && sqlrc != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "could not free the environment handle.");
   }
   } /* if(bConnect) */
}

/**************************************************************************
* Database Retrieval Functions                                            *
***************************************************************************
* GetBlastStrucFromDB             Get Blast Result set from BlastDB       *
* GetNBlastStrucFromDB            Get Neighbour set from NBlastDB         *
* GetNumberofNeighboursListFromDB Get the number of neighbours for each GI*
**************************************************************************/
SLRI_ERR ConvertBlastResultToSeqAlign(NBlastResultSetPtr pResultSet, SeqAlignPtr PNTR psap)
{
	NBlastResultPtr pResult = NULL, pResultTemp = NULL;
	DenseSegPtr dsp = NULL;
	SeqAlignPtr sap = NULL;
	ValNodePtr sip = NULL;
	ScorePtr sp1 = NULL, sp2 = NULL;
	ObjectIdPtr obid = NULL;
	Int4 num = 0;


	if(pResultSet == NULL) {
		ErrPostEx(SEV_ERROR,0,0,"Invalid ResultSetPtr");
		return SLRI_FAIL;
	}

	if((pResult = pResultSet->results) == NULL) {
		ErrPostEx(SEV_ERROR,0,0,"Invalid ResultPtr");
		return SLRI_FAIL;
	}

	sap = SeqAlignNew();

	sap->segtype = 2; /* Dense Seg */
	dsp = DenseSegNew();
	/* Results */
	pResultTemp = (NBlastResultPtr) AsnIoMemCopy(pResult,(AsnReadFunc) NBlastResultAsnRead,(AsnWriteFunc) NBlastResultAsnWrite);
	dsp->numseg = pResultTemp->numseg;
	Misc_ValNodeListToInt4Array(pResultTemp->starts,&(dsp->starts),&num); /* size of this should be num (2) * numseg */
	Misc_ValNodeListToInt4Array(pResultTemp->lens,&(dsp->lens),&num);
	dsp->numseg = (Int2) num;


	NBlastResultFree(pResultTemp);

	sap->segs = (Pointer) dsp;

	/* GIs */
	ValNodeAddInt(&sip,SEQID_GI,pResultSet->query_gi);
	ValNodeAddInt(&sip,SEQID_GI,pResultSet->subject_gi);
	dsp->ids = sip;

	/* Scores */
	sp1 = ScoreNew();
	obid = ObjectIdNew();
	obid->str = StringSave("bit_score");
	sp1->id = obid;
	sp1->choice = 1;
	sp1->value.intvalue = pResult->scores->bitscore;
	obid = NULL;
	sp2 = ScoreNew();
	obid = ObjectIdNew();
	obid->str = StringSave("e_value");
	sp2->id = obid;
	sp2->choice = 2;
	sp2->value.realvalue = pResult->scores->evalue;

	sp1->next = sp2;
	dsp->scores = sp1;

	*psap = sap;
	return SLRI_SUCCESS;
}

SLRI_ERR GetUIDfromGIPair(Int4 gi1, Int4 gi2, Int8 PNTR puid)
{

   SQLINTEGER result= 0;				/* result*/
   SQLRETURN retcode;					/* return values for odbc layer */

   Int4 ord1 = 0, ord2 = 0;

   key1 = gi1;
   key2 = gi2;

   /* search for gi1 */
   /* execute the select statement */
   retcode = SQLExecute(hstmt_nblastdb_gi1);
   if (retcode != SQL_SUCCESS)
   {
	  ErrPostEx(SEV_ERROR,0,0, "[GetUIDfromGIPair]: Could not execute select statement.");
	  LogSQLInfo(hstmt_nblastdb_gi1);
      SQLFreeStmt(hstmt_nblastdb_gi1, SQL_CLOSE);
      return SLRI_FAIL;
   }

   /* binding our results from the database */
   retcode = SQLBindCol(hstmt_nblastdb_gi1, 1, SQL_C_SLONG, &result, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[GetUIDfromGIPair]: Could not bind column 1.");
	 SQLFreeStmt(hstmt_nblastdb_gi1, SQL_CLOSE);
     return SLRI_FAIL;
   }

   while((retcode=SQLFetch(hstmt_nblastdb_gi1)) == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
	   ord1 = (Int4) result;
   }

   /* close it in order to re-use */
   if(SQLFreeStmt(hstmt_nblastdb_gi1, SQL_CLOSE) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[GetUIDfromGIPair]: Could not close statement.");
      return SLRI_FAIL;
   }

   /* search for gi2 */
   /* execute the select statement */
   retcode = SQLExecute(hstmt_nblastdb_gi2);
   if (retcode != SQL_SUCCESS)
   {
	  ErrPostEx(SEV_ERROR,0,0, "[GetUIDfromGIPair]: Could not execute select statement.");
	  LogSQLInfo(hstmt_nblastdb_gi2);
      SQLFreeStmt(hstmt_nblastdb_gi2, SQL_CLOSE);
      return SLRI_FAIL;
   }

   /* binding our results from the database */
   retcode = SQLBindCol(hstmt_nblastdb_gi2, 1, SQL_C_SLONG, &result, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[GetUIDfromGIPair]: Could not bind column 1.");
	 SQLFreeStmt(hstmt_nblastdb_gi2, SQL_CLOSE);
     return SLRI_FAIL;
   }

   while((retcode=SQLFetch(hstmt_nblastdb_gi2)) == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
	   ord2 = (Int4) result;
   }

   /* close it in order to re-use */
   if(SQLFreeStmt(hstmt_nblastdb_gi2, SQL_CLOSE) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[GetUIDfromGIPair]: Could not close statement.");
      return SLRI_FAIL;
   }

   if(ord1 < ord2) {
		ORDtoUID(ord1,ord2,&(*puid));
   } else {
		ORDtoUID(ord2,ord1,&(*puid));
   }

   return SLRI_SUCCESS;

}

SLRI_ERR GetBlastSeqAlignFromDB(Int4 gi1, Int4 gi2, SeqAlignPtr PNTR psap)
{

	NBlastResultSetPtr pResultSet = NULL;
	Int8 uid = 0;
	Int4 bloblen = 0, indicator = 0;
	SQLRETURN retcode = SQL_SUCCESS;
	SQLINTEGER blobloc = 0;
	Int4 pbbvalue = 0;

	CharPtr pcASN = NULL;

	GetUIDfromGIPair(gi1,gi2,&uid);
	if(uid <= 0) {
		ErrPostEx(SEV_ERROR,0,0, "[GetBlastSeqAlignFromUID]: Invalid Parameters");
		return SLRI_FAIL;
	}

	key1 = uid;

   /* execute statment */
   retcode = SQLExecute(hstmt_blastdb_uid);
   if (retcode != SQL_SUCCESS)
   {
	  ErrPostEx(SEV_ERROR,0,0, "[GetBlastSeqAlignFromDB]: Unable to execute select statement.");
	  LogSQLInfo(hstmt_blastdb_uid);
	  SQLFreeStmt(hstmt_blastdb_uid, SQL_CLOSE);
	  return SLRI_FAIL;
   }

   retcode = SQLBindCol(hstmt_blastdb_uid, 1, SQL_C_BINARY, &blobloc, 0, (int*)&pbbvalue);
   if((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO))
   {
      ErrPostEx(SEV_ERROR,0,0, "[GetBlastSeqAlignFromDB]: Binding Column 1 failed.");
	  SQLFreeStmt(hstmt_blastdb_uid, SQL_CLOSE);
      return SLRI_FAIL;
   }

   while((retcode=SQLFetch(hstmt_blastdb_uid)) == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
	   if (psap) {
		  /*Get the length of of the blob so that we can read it */
		  bloblen = (Int4)pbbvalue;

		  if(bloblen > 0)
		  {
			  /* allocate a buffer big enough to store the blob. */
			  pcASN = (CharPtr) MemNew((size_t) bloblen);

			  /*read the blob from the database */
			  SQLGetData(hstmt_blastdb_uid, 1, SQL_C_BINARY, (SQLPOINTER)pcASN, bloblen, (int*)&indicator);

			  if(pcASN != NULL)
			  {
			     pResultSet =  (NBlastResultSetPtr) GetASNMemChar(pcASN,(AsnReadFunc)NBlastResultSetAsnRead, bloblen);
			     if(pResultSet) ConvertBlastResultToSeqAlign(pResultSet,&(*psap));
			  }
			  if (pcASN) { MemFree(pcASN); }

		  }
	   }
   }

   SQLFreeStmt(hstmt_blastdb_uid, SQL_CLOSE);

   return SLRI_SUCCESS;
}


SLRI_ERR GetBlastResultsFromDB(Int4 gi1, Int4 gi2, NBlastResultSetPtr PNTR nbrsp)
{
	Int8 uid = 0;
	Int4 bloblen = 0, indicator = 0;
	SQLRETURN retcode = SQL_SUCCESS;
	SQLINTEGER blobloc = 0;
	Int4 pbbvalue = 0;

	CharPtr pcASN = NULL;

	GetUIDfromGIPair(gi1,gi2,&uid);
	if(uid <= 0) {
		ErrPostEx(SEV_ERROR,0,0, "[GetBlastResultsFromDB]: Invalid Parameters");
		return SLRI_FAIL;
	}


	key1 = uid;

   /* execute statment */
   retcode = SQLExecute(hstmt_blastdb_uid);
   if (retcode != SQL_SUCCESS)
   {
	  ErrPostEx(SEV_ERROR,0,0, "[GetBlastResultsFromDB]: Unable to execute select statement.");
	  LogSQLInfo(hstmt_blastdb_uid);
	  SQLFreeStmt(hstmt_blastdb_uid, SQL_CLOSE);
	  return SLRI_FAIL;
   }

   retcode = SQLBindCol(hstmt_blastdb_uid, 1, SQL_C_BINARY, &blobloc, 0, (int*)&pbbvalue);
   if((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO))
   {
      ErrPostEx(SEV_ERROR,0,0, "[GetBlastResultsFromDB]: Binding Column 1 failed.");
	  SQLFreeStmt(hstmt_blastdb_uid, SQL_CLOSE);
      return SLRI_FAIL;
   }

   while((retcode=SQLFetch(hstmt_blastdb_uid)) == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
		if (nbrsp) {
			/*Get the length of of the blob so that we can read it */
			bloblen = (Int4)pbbvalue;

			if(bloblen > 0)
			{
				/* allocate a buffer big enough to store the blob. */
				pcASN = (CharPtr) MemNew((size_t) bloblen);

				/*read the blob from the database */
				SQLGetData(hstmt_blastdb_uid, 1, SQL_C_BINARY, (SQLPOINTER)pcASN, bloblen, (int*)&indicator);

				if(pcASN != NULL)
				{
					*nbrsp =  (NBlastResultSetPtr) GetASNMemChar(pcASN,(AsnReadFunc)NBlastResultSetAsnRead, bloblen);
				}
				if (pcASN) { MemFree(pcASN); }

			}
		}
   }

   SQLFreeStmt(hstmt_blastdb_uid, SQL_CLOSE);

	return SLRI_SUCCESS;
}

SLRI_ERR GetNeighboursFromDB(SQLINTEGER gi, Int4 PNTR pnum, NBlastGiAndEvalSetPtr PNTR nbgesp)
{
	Int4 bloblen = 0, indicator = 0;
	SQLRETURN retcode = SQL_SUCCESS;
	SQLINTEGER blobloc = 0;
	Int4 pbbvalue = 0;
	CharPtr pcASN = NULL;
	SQLINTEGER nblastdb_num=0;

	if(gi < 0) {
		ErrPostEx(SEV_ERROR,0,0, "[GetNeighboursFromDB]: Invalid Parameters");
		return SLRI_FAIL;
	}

	key1 = gi;

   /* execute the select statement */
   retcode = SQLExecute(hstmt_nblastdb);
   if (retcode != SQL_SUCCESS)
   {
	  ErrPostEx(SEV_ERROR,0,0, "[GetNeighboursFromDB]: Could not execute select statement.");
	  LogSQLInfo(hstmt_nblastdb);
      SQLFreeStmt(hstmt_nblastdb, SQL_CLOSE);
      return SLRI_FAIL;
   }

   retcode = SQLBindCol(hstmt_nblastdb, 1, SQL_C_BINARY, &blobloc, 0, (int*)&pbbvalue);
   if((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO))
   {
      ErrPostEx(SEV_ERROR,0,0, "[GetNeighboursFromDB]: Binding Column 1 failed.");
	  SQLFreeStmt(hstmt_nblastdb, SQL_CLOSE);
      return SLRI_FAIL;
   }
   retcode = SQLBindCol(hstmt_nblastdb, 2, SQL_C_SLONG, &nblastdb_num, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[GetNeighboursFromDB]: Could not bind column 2.");
	 SQLFreeStmt(hstmt_nblastdb, SQL_CLOSE);
     return SLRI_FAIL;
   }

   while((retcode=SQLFetch(hstmt_nblastdb)) == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
	   if (nbgesp) {
		  /*Get the length of of the blob so that we can read it */
		  bloblen = (Int4)pbbvalue;

		  if(nblastdb_num > 0 && bloblen > 0)
		  {
			  /* allocate a buffer big enough to store the blob. */
			  pcASN = (CharPtr) MemNew((size_t) bloblen);

			  /*read the blob from the database */
			  SQLGetData(hstmt_nblastdb, 1, SQL_C_BINARY, (SQLPOINTER)pcASN, bloblen, (int*)&indicator);

			  if(pcASN != NULL)
			  {
			     *nbgesp  =  (NBlastGiAndEvalSetPtr) GetASNMemChar(pcASN,(AsnReadFunc)NBlastGiAndEvalSetAsnRead, bloblen);
			  }
			  if (pcASN) { MemFree(pcASN); }

		  }
	   }
	   if (pnum) *pnum = (Int4) nblastdb_num;
   }

   SQLFreeStmt(hstmt_nblastdb, SQL_CLOSE);

   return SLRI_SUCCESS;

}

/* not in use */
SLRI_ERR Get3DNeighboursFromDB(Int4 gi, Int4 PNTR pnum, ILinkSetPtr PNTR pp3DNeighbours)
{
	return SLRI_FAIL;
}

SLRI_ERR GetNumberofNeighboursListFromDB(Int4 PNTR pnum, ILinkSetPtr PNTR ppNumGiList, Int4 iDB)
{
	Int4 recs = 0, i = 0, maxid=0;
	ILinkSetPtr pNumGiList = NULL;
	SQLRETURN sqlrc=SQL_SUCCESS;
	SQLINTEGER gi, num;
	SQLINTEGER result=0, maxRowid=0;
	SQLINTEGER startRowid=1, endRowid=1, stepRowid=10000;


	if(pnum == NULL || ppNumGiList == NULL) {
		ErrPostEx(SEV_ERROR,0,0, "[GetNumberofNeighboursListFromDB]: Invalid Parameters");
		return SLRI_FAIL;
	}

	/* get number of records in table nbalstdb */
	sqlrc = SQLExecute (countNeighboursStmt);
	if (sqlrc != SQL_SUCCESS){
		ErrPostEx(SEV_ERROR,0,0, "[GetNumberofNeighboursListFromDB]:unable to execute select count(*) statement.");
		LogSQLInfo(countNeighboursStmt);
		SQLFreeStmt(countNeighboursStmt, SQL_CLOSE);
		return SLRI_FAIL;
	}
	
	sqlrc = SQLBindCol(countNeighboursStmt, 1, SQL_C_SLONG, &result, 0, NULL);
	if (sqlrc != SQL_SUCCESS && sqlrc != SQL_SUCCESS_WITH_INFO)
	{
		ErrPostEx(SEV_ERROR,0,0, "[GetNumberofNeighboursListFromDB]:unable to BindCol.");
		SQLFreeStmt(countNeighboursStmt, SQL_CLOSE);
		return SLRI_FAIL;
	}
	while ((sqlrc=SQLFetch(countNeighboursStmt))==SQL_SUCCESS || sqlrc == SQL_SUCCESS_WITH_INFO){
		recs = (Int4) result;
	}

	if((pNumGiList = ILinkSetNew2(1)) == NULL) {
		ErrPostEx(SEV_ERROR,0,0, "[GetNumberofNeighboursListFromDB]: Could not allocate memory for array.");
		return SLRI_FAIL;
	}

	/* use a fake gi. its weight is the total number of neighbours in nblastdb. */
	recs = (Int4) result;
	pNumGiList->uids[i] = (Int4) 0;
	pNumGiList->weights[i] = (Int4) recs;

	*ppNumGiList = pNumGiList;

	SQLFreeStmt(countNeighboursStmt,SQL_CLOSE);
	return SLRI_SUCCESS;
}


/* $Log: nbr_odbc.c,v $
/* Revision 1.13  2005/04/26 14:29:33  rcavero
/* Change Copyright Note.
/*
/* Revision 1.12  2004/08/06 14:47:22  mmatan
/* Changed CharPtr database[50] to SQLCHAR database[50]
/*
/* Revision 1.11  2004/08/05 16:20:21  zwang
/* Remove DB2 related code.
/*
/* Revision 1.10  2004/07/23 19:40:09  zwang
/* More change in GetNumberOfNeighboursListFromDB: allocate memory for only one element in the linked list.
/*
/* Revision 1.9  2004/07/23 17:14:53  zwang
/* Change the logic of getting the total number of neighbours from nblastdb id ODBC to speed it up.
/*
/* Revision 1.8  2004/06/10 16:58:03  zwang
/* Add flag to determine if a database connection should be established. See bug #2324.
/*
/* Revision 1.7  2004/05/28 19:25:19  zwang
/* Change in function GetNumberofNeighboursListFromDB: retrieve and fetch 10000 records at a time in order to make it faster and memory usage efficient.
/*
/* Revision 1.6  2004/05/27 19:32:38  zwang
/* Check if field 'num' is 0 in nblastdb, if it is, don't look into field 'asnnbr'
/*
/* Revision 1.5  2004/05/26 19:54:18  zwang
/* Fixed a minor error.
/*
/* Revision 1.4  2004/05/14 16:48:27  zwang
/* Rremove //
/*
/* Revision 1.3  2004/05/14 16:09:29  zwang
/* Implement ODBC layer functions to retrieve data.
/*
/* Revision 1.2  2004/02/27 17:28:54  zwang
/* Got rid of warnings of SQLBindCol.
/*
/* Revision 1.1  2004/01/13 21:25:05  zwang
/* New file for Neighbour in ODBC
/* */


