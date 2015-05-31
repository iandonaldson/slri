/* $Id: intrez_odbc.c,v 1.55 2005/03/14 16:02:39 eburgess Exp $ */

#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>
#include <asn.h>

#include <intrez_odbc.h>
#include <intrez_cfg.h>
#include <intrez_misc.h>
#include <objslristruc.h>
#include <slri_odbc.h>
#include <stdlib.h>

/* henv and hdbc defined in slri_odbc.h */

/* ACCDB fields used in SQLBindParamter */
struct
{
  SQLCHAR val[1024];
  /*CharPtr val;*/
   SQLINTEGER len;
}accdb_title;


/* ASNDB fields in SQLBindParameter*/
struct
{
   /*SQLINTEGER len;*/
   Int4 len;
   CharPtr val;
}asndb_asn;

/* SENDB fields in SQLBindParameter */
struct
{
   Int4 len;
   CharPtr val;
}sendb_asn;


/* MMDB fields in SQLBindParameter*/
struct
{
   Int4 len;
   CharPtr val;
}mmdb_asn;
/* DOMDB fields in SQLBindParameter*/
struct
{
   Int4 len;
   CharPtr val;
}domdb_asn;

/* CDDB fields in SQLBindParameter*/
struct
{
   Int4 len;
   CharPtr val;
}cddb_asn;

/* SeqHound Global Variables */
int connected;
SQLCHAR database[50];

/********************************************/
/*  Open databases                          */
/********************************************/
Boolean ConnectToDb(void)
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
      ErrPostEx(SEV_ERROR, 0, 0, "[ConnectToDb]: Could not find username in initialization file.");
      return FALSE;
   }
   if( GetAppParam("intrez", "datab", "password", NULL, (Char*)passwd, sizeof(char) * 10) <= 0 )
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[ConnectToDb]: Could not find pasword in initialization file.");
      return FALSE;
   }
   if( GetAppParam("intrez", "datab", "dsn", NULL, (Char*)dsn, sizeof(char) * 10) <= 0 )
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[ConnectToDb]: Could not find dsn in initialization file.");
      return FALSE;
   }

   /* database is a global parameter, we will use it when we initialize the sql statements,
      this way, we don't hardcode the database into the source code, allowing us to
      change the instance of the database by specifying it in the .intrezrc file, this
      is illustrated in each of the InitXXX functions below. */
   if( GetAppParam("intrez", "datab", "database", NULL, (Char*)database, sizeof(char) * 10) <= 0 )
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[ConnectToDb]: Could not find database in initialization file.");
      return FALSE;
   }
   /* make sure the proper DB2 instance variable is defined--only for aix system */
   /* sprintf(instancebuf,"DB2INSTANCE=db2inst1",instance);
   putenv(instancebuf); */

    /* allocate an envirnment handle */
    retcode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);
    if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO))
    {
	ErrPostEx(SEV_ERROR,0,0, "[ConnectToDb]: Unable to allocate environment handle.");
	return FALSE;
    }

    /* Set up ODBC Version 3.0 */
    retcode = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0);
    if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO))
    {
      ErrPostEx(SEV_ERROR,0,0, "[ConnectToDb]: Unable to setup ODBC Verison to 3.0.");
      SQLFreeHandle(SQL_HANDLE_ENV, henv);
      return FALSE;
    }

    /* allocate a connection handle */
    retcode = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);
    if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO))
    {
      ErrPostEx(SEV_ERROR,0,0, "[ConnectToDb]: Cannot allocate connection handle.");
      SQLFreeHandle(SQL_HANDLE_ENV, henv);
      return FALSE;
    }

    /* Attempt to connect to DB */
    retcode = SQLConnect(hdbc, dsn, SQL_NTS, usr, SQL_NTS, passwd,
						 SQL_NTS);
    if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO))
    {
       ErrPostEx(SEV_ERROR,0,0, "[ConnectToDb]: Connection to database failed");
       LogSQLConnectInfo(hdbc);
       SQLFreeHandle(SQL_HANDLE_ENV, henv);
       return FALSE;
    }

    /* get here then connection to database successful */
    connected = TRUE;
    return TRUE;
}


/* initialization of ASNDB for insertion of records:
 *  1) Creates a handle for the insertion command and
 *  2) binds the parameters.
 *  This function is called by InitCodeBase. To insert into
 * the ASNDB database, use AppendRecordASNDB */
Boolean InitASNDB(void)
{
   SQLRETURN retcode;
   CharPtr pSQL_stmt = (char*) malloc(sizeof(Char) * 150);
   if(pSQL_stmt == NULL)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[InitASNDB]: Could not allocate space");
     return FALSE;
   }

   /******************************************
    insert
	******************************************/
   sprintf((char *)pSQL_stmt, "insert into %s.asndb(gi, asn1, division, release, type) values(?, ?, ?, ?, ?);", database);
   /* create the global insert handles.
    * paramter 1 = gi, 2 = asn1, 3 = division ... 5 = type */
   retcode = CreateHandle(&HNDLAsndb, pSQL_stmt, hdbc, &henv);
   if(retcode != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitASNDB]: Could not create handle");
      free(pSQL_stmt);
      return FALSE;
   }

   /* bind parameters 1, 3-5, did not bind parameter 2 because it is an asn blob
    * whose size is not fixed, and as such it must be bound each time it
    * is inserted, so the SQLBindParameter for parameter 2 is done in
    * function AppendRecordASNDB. */
   retcode = SQLBindParameter(HNDLAsndb, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &asndb_gi.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitASNDB]: Could not bind parameter 1");
      free(pSQL_stmt);
      return FALSE;
   }

   retcode = SQLBindParameter(HNDLAsndb, 3, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 0, 0, asndb_div.val, sizeof(asndb_div.val), NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitASNDB]: Could not bind parameter 3");
      free(pSQL_stmt);
      return FALSE;
   }

   retcode = SQLBindParameter(HNDLAsndb, 4, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 0, 0, asndb_release.val, sizeof(asndb_release.val), NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitASNDB]: Could not bind parameter 4");
      free(pSQL_stmt);
      return FALSE;
   }

   retcode = SQLBindParameter(HNDLAsndb, 5, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 0, 0, asndb_type.val, sizeof(asndb_type.val), NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitASNDB]: Could not bind parameter 5");
      free(pSQL_stmt);
      return FALSE;
   }
        /* cannot bind the ASN blob here: once a variable is bound, its length may not change.
         * SQLINTEGER variables have a fixed length determined by the OS platform,
         * SQLCHAR variables have a fixed length based on the size of the array declared,
         * BLOBS have lengths that change, so they must be individually bound each time
         * a blob is appended. The binding of the blob parameter occurs in
         * AppendRecordASNDB. Leaving this commented out for sake of CLARITY.
         */ /* retcode = SQLBindParameter(HNDLAsndb, 2, SQL_PARAM_INPUT, SQL_C_DEFAULT,
               SQL_LONGVARBINARY, 0, 0, asndb_asn.val, 0, (long*)&asndb_asn.len);
               if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
               ErrPostEx(SEV_ERROR, 0, 0, "[InitASNDB]: Could not bind parameter 2");
             */


	/******************************************
    delete
	******************************************/
   sprintf((char *)pSQL_stmt, "delete from %s.asndb where gi=?", database);
   retcode = CreateHandle(&HNDLAsndbDelete, pSQL_stmt, hdbc, &henv);
   if(retcode != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitASNDB]: Could not create handle");
      free(pSQL_stmt);
      return FALSE;
   }
   retcode = SQLBindParameter(HNDLAsndbDelete, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &asndb_gi.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitASNDB]: Could not bind parameter 1");
      free(pSQL_stmt);
      return FALSE;
   }

	/******************************************
    search by Gi
	******************************************/
   sprintf((char *)pSQL_stmt, "select division, release, type, asn1 from %s.asndb where gi=?", database);
   retcode = CreateHandle(&HNDLAsndbSearchGi, pSQL_stmt, hdbc, &henv);
   if(retcode != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitASNDB]: Could not create handle HNDLAsndbSearchGi");
      free(pSQL_stmt);
      return FALSE;
   }
   retcode = SQLBindParameter(HNDLAsndbSearchGi, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &asndb_gi.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitASNDB]: Could not bind parameter 1 for HNDLAsndbSearchGi");
      free(pSQL_stmt);
      return FALSE;
   }

   free(pSQL_stmt);
   return TRUE;
}


/* initialization of PARTI for insertion of records:
 *  1) Creates a handle for the insertion command and
 *  2) binds the parameters.
 *  This function is called by InitCodeBase. To insert into
 * the PARTI database, use AppendRecordPARTI */
Boolean InitPARTI(void)
{
   SQLRETURN retcode;
   CharPtr pSQL_stmt = (Char*) malloc(sizeof(Char) * 150);
   if(pSQL_stmt == NULL)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[InitPARTI]: Could not allocated space");
     return FALSE;
   }

   /******************************************
    insert
	******************************************/
   /* create the handle to the insert statement, 2 input fields: gi and division */
   sprintf((char*)pSQL_stmt, "insert into %s.parti(gi, division) values(?,?);", database);
   retcode = CreateHandle(&HNDLParti, pSQL_stmt, hdbc, &henv);
   if(retcode != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitPARTI]: Could not create handle");
      free(pSQL_stmt);
      return FALSE;
   }

   /* bind the 2 parameters gi and division here so that we only have to do it once.*/
   retcode = SQLBindParameter(HNDLParti, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &parti_gi.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitPARTI]: Could not bind parameter 1");
      free(pSQL_stmt);
      return FALSE;
   }
   retcode = SQLBindParameter(HNDLParti, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 0, 0, parti_div.val, sizeof(parti_div.val), NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitPARTI]: Could not bind parameter 2");
      free(pSQL_stmt);
      return FALSE;
   }

   /******************************************
    delete
	******************************************/
   sprintf((char*)pSQL_stmt, "delete from %s.parti where gi=?", database);
   retcode = CreateHandle(&HNDLPartiDelete, pSQL_stmt, hdbc, &henv);
   if(retcode != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitPARTI]: Could not create handle");
      free(pSQL_stmt);
      return FALSE;
   }
   retcode = SQLBindParameter(HNDLPartiDelete, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &parti_gi.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitPARTI]: Could not bind parameter 1");
      free(pSQL_stmt);
      return FALSE;
   }


   /******************************************
    Search Parti by gi to find division.
	******************************************/

   sprintf((char*)pSQL_stmt, "select division from %s.parti where gi=?", database);
   retcode = CreateHandle(&HNDLPartiSearchGi, pSQL_stmt, hdbc, &henv);
   if(retcode != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitPARTI]: Could not create handle");
      free(pSQL_stmt);
      return FALSE;
   }
   retcode = SQLBindParameter(HNDLPartiSearchGi, 1, SQL_PARAM_INPUT, 
							  SQL_C_SLONG, SQL_INTEGER, 0, 0, 
							  &parti_gi.val, 0, NULL);

   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitPARTI]: Could not bind parameter 1");
      free(pSQL_stmt);
      return FALSE;
   }


   free(pSQL_stmt);
   return TRUE;
}

/* initialization of REDUND for insertion of records:
 *  1) Creates a handle for the insertion command and
 *  2) binds the parameters.
 *  This function is called by InitCodeBase. To insert into
 * the REDUND database, use AppendRecordREDUND */
Boolean InitREDUND(void)
{
   SQLRETURN retcode;
   CharPtr pSQL_stmt = (Char*) malloc(sizeof(Char) * 150);
   if(pSQL_stmt == NULL)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[InitREDUND]: Could not allocated space");
     return FALSE;
   }

   /****************************************
    insert into REDUND
	****************************************/
   /* create the handle to the insert statement, 3 input fields: gi, rordinal & rgroup */
   sprintf((char*)pSQL_stmt, "insert into %s.redund(gi, rordinal, rgroup) values(?,?,?);", database);
   if(CreateHandle(&HNDLRedund, pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitREDUND]: Could not create handle for HNDLRedund");
      free(pSQL_stmt);
      return FALSE;
   }
   free(pSQL_stmt);

   /* bind parameters here so that we only have to do it once.*/
   retcode = SQLBindParameter(HNDLRedund, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &redund_gi.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitREDUND]: Could not bind parameter 1");
      return FALSE;
   }
   retcode = SQLBindParameter(HNDLRedund, 2, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_VARCHAR, 0, 0, &redund_rordinal.val, sizeof(redund_rordinal.val), NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitREDUND]: Could not bind parameter 2");
      return FALSE;
   }
   retcode = SQLBindParameter(HNDLRedund, 3, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_VARCHAR, 0, 0, &redund_rgroup.val, sizeof(redund_rgroup.val), NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitREDUND]: Could not bind parameter 3");
      return FALSE;
   }

   return TRUE;
}

/* initialization of TAXGI for insertion of records:
 *  1) Creates a handle for the insertion command and
 *  2) binds the parameters.
 *  This function is called by InitCodeBase. To insert into
 * the TAXGI database, use AppendRecordTAXGI */
Boolean InitTAXGI(void)
{
   SQLRETURN retcode;
   CharPtr pSQL_stmt = (Char *) malloc(sizeof(Char) * 150);
   if(pSQL_stmt == NULL)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[InitTAXGI]: Could not allocate space");
     return FALSE;
   }

   /******************************************
    insert
	******************************************/
   /* create the insert handle to TAXGI DB, 4 insert fields: gi, taxid, kloodge, type.
      database is a global that holds the value of the database, which gets read in
      from the .intrezrc file in ConnectToDb. typicall, database = 'seqhound'  */
   sprintf((char *) pSQL_stmt, "insert into %s.taxgi(gi, taxid, kloodge, type) values(?, ?, ?, ?);", database);
   if(CreateHandle(&HNDLTaxgi, pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitTAXGI]: Could not create handle.");
      free(pSQL_stmt);
      return FALSE;
   }

   /* do all the binding here since all the fields have fixed lengths,
    * this will reduce the time of insertion. */
   /* binding field 1 */
   retcode = SQLBindParameter(HNDLTaxgi, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &taxgi_gi.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitTAXGI]: Could not bind parameter 1");
      return FALSE;
   }
   /* binding field 2 */
   retcode = SQLBindParameter(HNDLTaxgi, 2, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &taxgi_taxid.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitTAXGI]: Could not bind parameter 2");
      return FALSE;
   }
   /* binding fields 3 */
   retcode = SQLBindParameter(HNDLTaxgi, 3, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &taxgi_kloodge.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitTAXGI]: Could not bind parameter 3");
      return FALSE;
   }
   /* binding field 4 */
   retcode = SQLBindParameter(HNDLTaxgi, 4, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 
							  0, 0, taxgi_type.val, sizeof(taxgi_type.val), NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitTAXGI]: Could not bind parameter 4");
      return FALSE;
   }

   /******************************************
    delete
	******************************************/
   sprintf((char *) pSQL_stmt, "delete from %s.taxgi where gi=?", database);
   if(CreateHandle(&HNDLTaxgiDelete, pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitTAXGI]: Could not create handle.");
      free(pSQL_stmt);
      return FALSE;
   }
   retcode = SQLBindParameter(HNDLTaxgiDelete, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &taxgi_gi.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitTAXGI]: Could not bind parameter 1");
      return FALSE;
   }

   /******************************************
    for search in function EditRecordTAXGIByGI
	******************************************/
   sprintf(pSQL_stmt, "select gi, taxid, kloodge, type from %s.taxgi where gi=?;", database);
   if(CreateHandle(&HNDLTaxgiSearchGi,pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitTAXGI]: Could not create handle HNDLTaxgiSearchGi");
      free(pSQL_stmt);
      return FALSE;
   }

   retcode = SQLBindParameter(HNDLTaxgiSearchGi, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &taxgi_gi.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitTAXGI]: Could not bind parameter 1 for HNDLTaxgiSearchGi");
      return FALSE;
   }


   /******************************************
    for update in function EditRecordTAXGIByGI and EditTaxgiKloodgeByGi
	******************************************/

   /* update all */
   sprintf(pSQL_stmt, "update %s.taxgi set taxid = ?, kloodge = ?, type = ? where gi = ?", database);
   if(CreateHandle(&HNDLTaxgiUpdateAll,pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitTAXGI]: Could not create handle HNDLTaxgiUpdateAll");
      free(pSQL_stmt);
      return FALSE;
   }
   retcode = SQLBindParameter(HNDLTaxgiUpdateAll, 1, SQL_PARAM_INPUT, SQL_C_SLONG, 
							  SQL_INTEGER, 0, 0, &taxgi_taxid.val, 0, NULL);

   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitTAXGI]: Could not bind parameter 1 for HNDLTaxgiUpdateAll");
      return FALSE;
   }
   retcode = SQLBindParameter(HNDLTaxgiUpdateAll, 2, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &taxgi_kloodge.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitTAXGI]: Could not bind parameter 2 for HNDLTaxgiUpdateAll");
      return FALSE;
   }
   retcode = SQLBindParameter(HNDLTaxgiUpdateAll, 3, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 0, 0, taxgi_type.val, sizeof(taxgi_type.val), NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitTAXGI]: Could not bind parameter 3 for HNDLTaxgiUpdateAll");
      return FALSE;
   }
   retcode = SQLBindParameter(HNDLTaxgiUpdateAll, 4, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &taxgi_gi.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitTAXGI]: Could not bind parameter 4 for HNDLTaxgiUpdateAll");
      return FALSE;
   }

   /* update taxid */
   sprintf(pSQL_stmt, "update %s.taxgi set taxid=? where gi = ?", database);
   if(CreateHandle(&HNDLTaxgiUpdateTaxid,pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitTAXGI]: Could not create handle HNDLTaxgiUpdateTaxid");
      free(pSQL_stmt);
      return FALSE;
   }
   retcode = SQLBindParameter(HNDLTaxgiUpdateTaxid, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &taxgi_taxid.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitTAXGI]: Could not bind parameter 1 for HNDLTaxgiUpdateTaxid");
      return FALSE;
   }
   retcode = SQLBindParameter(HNDLTaxgiUpdateTaxid, 2, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &taxgi_gi.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitTAXGI]: Could not bind parameter 2 for HNDLTaxgiUpdateTaxid");
      return FALSE;
   }

   /* update kloodge */
   sprintf(pSQL_stmt, "update %s.taxgi set kloodge=? where gi = ?", database);
   if(CreateHandle(&HNDLTaxgiUpdateKloodge,pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitTAXGI]: Could not create handle HNDLTaxgiUpdateKloodge");
      free(pSQL_stmt);
      return FALSE;
   }
   retcode = SQLBindParameter(HNDLTaxgiUpdateKloodge, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &taxgi_kloodge.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitTAXGI]: Could not bind parameter 1 for HNDLTaxgiUpdateKloodge");
      return FALSE;
   }
   retcode = SQLBindParameter(HNDLTaxgiUpdateKloodge, 2, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &taxgi_gi.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitTAXGI]: Could not bind parameter 2 for HNDLTaxgiUpdateKloodge");
      return FALSE;
   }

/* update kloodge by kloodge - used in update.c for comgen records*/
   sprintf(pSQL_stmt, "update %s.taxgi set kloodge=? where kloodge = ?", database);
   if(CreateHandle(&HNDLTaxgiUpdateKloodgeByKloodge,pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitTAXGI]: Could not create handle HNDLTaxgiUpdateKloodgeByKloodge");
      free(pSQL_stmt);
      return FALSE;
   }
   retcode = SQLBindParameter(HNDLTaxgiUpdateKloodgeByKloodge, 1, SQL_PARAM_INPUT, 
							  SQL_C_SLONG, SQL_INTEGER, 0, 0, 
							  &taxgi_kloodge.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitTAXGI]: Could not bind parameter 1 for HNDLTaxgiUpdateKloodgeByKloodge");
      return FALSE;
   }
   retcode = SQLBindParameter(HNDLTaxgiUpdateKloodgeByKloodge, 2, SQL_PARAM_INPUT, 
							  SQL_C_SLONG, SQL_INTEGER, 0, 0, 
							  &taxgi_kloodge2.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitTAXGI]: Could not bind parameter 2 for HNDLTaxgiUpdateKloodgeByKloodge");
      return FALSE;
   }

   /* update type */
   sprintf(pSQL_stmt, "update %s.taxgi set type=? where gi = ?", database);
   if(CreateHandle(&HNDLTaxgiUpdateType,pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitTAXGI]: Could not create handle HNDLTaxgiUpdateType");
      free(pSQL_stmt);
      return FALSE;
   }
   retcode = SQLBindParameter(HNDLTaxgiUpdateType, 1, SQL_PARAM_INPUT,
							  SQL_C_CHAR, SQL_VARCHAR, 0, 0, taxgi_type.val, sizeof(taxgi_type.val), NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitTAXGI]: Could not bind parameter 1 for HNDLTaxgiUpdateType");
      return FALSE;
   }
   retcode = SQLBindParameter(HNDLTaxgiUpdateType, 2, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &taxgi_gi.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitTAXGI]: Could not bind parameter 2 for HNDLTaxgiUpdateType");
      return FALSE;
   }

 /******************************************
    Searching for protein records by kloodge.
	******************************************/
   sprintf( pSQL_stmt, "select gi from %s.taxgi where kloodge = ? "
			"and type = 'protein'", database );

    if(CreateHandle(&HNDLTaxgiSearchKloodgeAA,pSQL_stmt, hdbc, &henv) != 
	   SQL_SUCCESS)
	{
      ErrPostEx(SEV_ERROR, 0, 0, "[InitTAXGI]: "
				"Could not create handle HNDLTaxgiSearchKloodgeAA");
      free(pSQL_stmt);
      return FALSE;
	}

	retcode = SQLBindParameter(HNDLTaxgiSearchKloodgeAA, 1, SQL_PARAM_INPUT, 
							   SQL_C_SLONG, SQL_INTEGER, 0, 0, 
							   &taxgi_kloodge.val, 0, NULL);

	if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
	{
      ErrPostEx(SEV_ERROR, 0, 0, "[InitTAXGI]: Could not bind parameter 1"
				"for HNDLTaxgiSearchKloodgeAA");
      return FALSE;
	}

	/******************************************
    Searching for Nucleic acid records by kloodge.
	******************************************/
    sprintf( pSQL_stmt, "select gi from %s.taxgi where kloodge = ? and "
			"type in ('DNA', 'RNA')", database );

    if(CreateHandle(&HNDLTaxgiSearchKloodgeNA,pSQL_stmt, hdbc, &henv) != 
	   SQL_SUCCESS)
	{
      ErrPostEx(SEV_ERROR, 0, 0, "[InitTAXGI]: "
				"Could not create handle HNDLTaxgiSearchKloodgeNA");
      free(pSQL_stmt);
      return FALSE;
	}

	retcode = SQLBindParameter(HNDLTaxgiSearchKloodgeNA, 1, SQL_PARAM_INPUT, 
							   SQL_C_SLONG, SQL_INTEGER, 0, 0, 
							   &taxgi_kloodge.val, 0, NULL);

	if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
	{
      ErrPostEx(SEV_ERROR, 0, 0, "[InitTAXGI]: Could not bind parameter 1"
				"for HNDLTaxgiSearchKloodgeNA");
	  free(pSQL_stmt);
      return FALSE;
	}

	free(pSQL_stmt);

	return TRUE;
}

/* initialization of NUCPROT for insertion of records:
 *  1) Creates a handle for the insertion command and
 *  2) binds the parameters.
 *  This function is called by InitCodeBase. To insert into
 * the NUCPROT database, use AppendRecordNUCPROT */
Boolean InitNUCPROT(void)
{
   SQLRETURN retcode;
   CharPtr pSQL_stmt = (Char *) malloc (sizeof(Char) * 150);
   if(pSQL_stmt == NULL)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[InitNUCPROT]: Could not allocate space.");
     return FALSE;
   }

   /******************************************
    insert
	******************************************/
   /* create handle */
   sprintf(pSQL_stmt, "insert into %s.nucprot(gin, gia) values(?,?)", database);
   if(CreateHandle(&HNDLNucprot, pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitNUCPROT]: Could not create handle to insert statement.");
      free(pSQL_stmt);
      return FALSE;
   }

   /* binding paramters 1 & 2 */
   retcode = SQLBindParameter(HNDLNucprot, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &nucprot_gin.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitNUCPROT]: Could not bind parameter 1.");
      free(pSQL_stmt);
      return FALSE;
   }

   retcode = SQLBindParameter(HNDLNucprot, 2, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &nucprot_gia.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitNUCPROT]: Could not bind parameter 2.");
      free(pSQL_stmt);
      return FALSE;
   }

   /******************************************
    delete
    delete from nucprot where gin=? or gia=? is slow because of "or" condition
    break this query into two
	******************************************/
   /* create handle */
   sprintf(pSQL_stmt, "delete from %s.nucprot where gin=?", database);
   if(CreateHandle(&HNDLNucprotDeleteGin, pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitNUCPROT]: Could not create handle to delete statement.");
      free(pSQL_stmt);
      return FALSE;
   }

   retcode = SQLBindParameter(HNDLNucprotDeleteGin, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &nucprot_gin.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitNUCPROT]: Could not bind parameter 1.");
      free(pSQL_stmt);
      return FALSE;
   }

   sprintf(pSQL_stmt, "delete from %s.nucprot where gia=?", database);
   if(CreateHandle(&HNDLNucprotDeleteGia, pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitNUCPROT]: Could not create handle to delete statement.");
      free(pSQL_stmt);
      return FALSE;
   }

   retcode = SQLBindParameter(HNDLNucprotDeleteGia, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &nucprot_gia.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitNUCPROT]: Could not bind parameter 1.");
      free(pSQL_stmt);
      return FALSE;
   }

   free(pSQL_stmt);
   return TRUE;
}

/* initialization of ACCDB for insertion of records:
 *  1) Creates a handle for the insertion command and
 *  2) binds the parameters.
 *  This function is called by InitCodeBase. To insert into
 * the ACCDB database, use AppendRecordACCDB */
Boolean InitACCDB(void)
{
   SQLRETURN retcode;
   CharPtr pSQL_stmt = (Char *) malloc (sizeof(Char) * 150);
   if(pSQL_stmt == NULL)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[InitACCDB]: Could not allocate space.");
     return FALSE;
   }

   /******************************************
    insert
	******************************************/
   /* create handle to accdb: variable database is global, and
      is retrieved from .intrezrc in ConnectToDb function. It
      contains the database name, eg: seqhound */
   sprintf(pSQL_stmt, "insert into %s.accdb(gi, db, name, access, chain, release, version, title, namelow) values(?,?,?,?,?,?,?,?,?)", database);
   if(CreateHandle(&HNDLAccdb,pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitACCDB]: Could not create handle to ACCDB");
      free(pSQL_stmt);
      return FALSE;
   }

   /* bind parameters 1 - 8 */
   retcode = SQLBindParameter(HNDLAccdb, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &accdb_gi.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitACCDB]: Could not bind parameter 1");
      return FALSE;
   }

   retcode = SQLBindParameter(HNDLAccdb, 2, SQL_PARAM_INPUT, SQL_C_CHAR, 
							  SQL_VARCHAR, 0, 0, accdb_db.val, 
							  sizeof(accdb_db.val), NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitACCDB]: Could not bind parameter 2");
      return FALSE;
   }

   retcode = SQLBindParameter(HNDLAccdb, 3, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 
							  0, 0, accdb_name.val, sizeof(accdb_name.val), NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitACCDB]: Could not bind parameter 3");
      return FALSE;
   }
   retcode = SQLBindParameter(HNDLAccdb, 4, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 0, 0, accdb_access.val, sizeof(accdb_access.val), NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitACCDB]: Could not bind parameter 4");
      return FALSE;
   }
   retcode = SQLBindParameter(HNDLAccdb, 5, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 0, 0, accdb_chain.val, sizeof(accdb_chain.val), NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitACCDB]: Could not bind parameter 5");
      return FALSE;
   }
   retcode = SQLBindParameter(HNDLAccdb, 6, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 0, 0, accdb_release.val, sizeof(accdb_release.val), NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitACCDB]: Could not bind parameter 6");
      return FALSE;
   }
   retcode = SQLBindParameter(HNDLAccdb, 7, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &accdb_version.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitACCDB]: Could not bind parameter 7");
      return FALSE;
   }

   /* this may be a problem because of the size of the field, see MyODBC documentation */
   /*bind in in Append function in case the length of title exceeds 1024 */
   /*retcode = SQLBindParameter(HNDLAccdb, 8, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 1024, 0, accdb_title.val, sizeof(accdb_title.val), NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitACCDB]: Could not bind parameter 8");
      return FALSE;
   }
   */

   /* bind parameter 9 */
   retcode = SQLBindParameter(HNDLAccdb, 9, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 0, 0, accdb_namelow.val, sizeof(accdb_namelow.val), NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitACCDB]: Could not bind parameter 9");
      return FALSE;
   }

   /******************************************
    select count
	******************************************/
   sprintf(pSQL_stmt, "select count(*) as count from %s.accdb;", database);
   if(CreateHandle(&HNDLAccdbSelectCount,pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitACCDB]: Could not create handle HNDLAccdbSelectCount");
      free(pSQL_stmt);
      return FALSE;
   }

   /******************************************
    for search in function EditRecordACCDBByGI
	******************************************/
   sprintf(pSQL_stmt, "select gi, db, name, access, chain, release, version, title, namelow from %s.accdb where gi=?;", database);
   if(CreateHandle(&HNDLAccdbSearchGi,pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitACCDB]: Could not create handle HNDLAccdbSearchGi");
      free(pSQL_stmt);
      return FALSE;
   }

   retcode = SQLBindParameter(HNDLAccdbSearchGi, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &accdb_gi.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitACCDB]: Could not bind parameter 1 for HNDLAccdbSearchGi");
      return FALSE;
   }

/******************************************
    for search in function EditRecordACCDBByGI
	******************************************/
   sprintf(pSQL_stmt, "select gi, db, name, access, chain, release, version, title, namelow from %s.accdb where gi=?;", database);
   if(CreateHandle(&HNDLAccdbSearchGi,pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitACCDB]: Could not create handle HNDLAccdbSearchGi");
      free(pSQL_stmt);
      return FALSE;
   }

   retcode = SQLBindParameter(HNDLAccdbSearchGi, 1, SQL_PARAM_INPUT, 
							  SQL_C_SLONG, SQL_INTEGER, 0, 0, &accdb_gi.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitACCDB]: Could not bind parameter 1 for HNDLAccdbSearchGi");
      return FALSE;
   }

   /******************************************
    for search in function GetRecACCDB
	******************************************/
   sprintf(pSQL_stmt, "select gi, db, name, access, chain, release, version, title, namelow from %s.accdb where rowid=?;", database);
   if(CreateHandle(&HNDLAccdbSearchRowid,pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitACCDB]: Could not create handle HNDLAccdbSearchRowid");
      free(pSQL_stmt);
      return FALSE;
   }

   retcode = SQLBindParameter(HNDLAccdbSearchRowid, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &rowid.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitACCDB]: Could not bind parameter 1 for HNDLAccdbSearchRowid");
      return FALSE;
   }

   /******************************************
    for search in function SHoundFindGiByAcc
	******************************************/
   sprintf(pSQL_stmt, "select gi, version from %s.accdb where access=?;", database);
   if(CreateHandle(&HNDLAccdbSearchAccess,pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitACCDB]: Could not create handle HNDLAccdbSearchAccess");
      free(pSQL_stmt);
      return FALSE;
   }

   retcode = SQLBindParameter(HNDLAccdbSearchAccess, 1, 
							  SQL_PARAM_INPUT, SQL_C_CHAR, 
							  SQL_VARCHAR, 0, 0, accdb_access.val, 
							  sizeof(accdb_access.val), NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitACCDB]: Could not bind parameter 1 for HNDLAccdbSearchAccess");
      return FALSE;
   }

   /******************************************
    for search in function SHoundFindAccessByGi
	******************************************/
   sprintf(pSQL_stmt, "select access, db from %s.accdb where gi =?;", database);

   if(CreateHandle(&HNDLAccdbGetAccessByGi,pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitACCDB]: Could not create handle "
				"HNDLAccdbGetGiAccess");
      free(pSQL_stmt);
      return FALSE;
   }

   retcode = SQLBindParameter(HNDLAccdbGetAccessByGi, 1,
							 SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 
							  0, 0, &accdb_gi.val, 0, NULL);

   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitACCDB]: Could not bind parameter 1 for "
				"HNDLAccdbGetAccessByGi");
      return FALSE;
   }

   /******************************************
    for search in function SearchAccdbByName
	******************************************/
sprintf(pSQL_stmt, "select gi from %s.accdb where name =? and "
		"db = ?;", database);

   if(CreateHandle(&HNDLAccdbGetGiByName ,pSQL_stmt, hdbc, 
				   &henv) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitACCDB]: Could not create handle "
				"HNDLAccdbGetGiByName");
      free(pSQL_stmt);
      return FALSE;
   }

   retcode = SQLBindParameter(HNDLAccdbGetGiByName, 1,
							 SQL_PARAM_INPUT, SQL_C_CHAR, 
							  SQL_VARCHAR,  0, 0, accdb_name.val,
							  sizeof( accdb_name.val), NULL);

   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitACCDB]: Could not bind"
				"parameter 1 for HNDLAccdbGetGiByName");
				
      return FALSE;
   }

   retcode = SQLBindParameter(HNDLAccdbGetGiByName, 2,
							  SQL_PARAM_INPUT, SQL_C_CHAR, 
							  SQL_VARCHAR,  0, 0, accdb_db.val,
							  sizeof( accdb_db.val), NULL);

   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitACCDB]: Could not bind"
				"parameter 2 for HNDLAccdbGetGiByName");
				
      return FALSE;
   }
   /******************************************
    delete
	******************************************/
   sprintf(pSQL_stmt, "delete from %s.accdb where gi=?", database);
   if(CreateHandle(&HNDLAccdbDelete,pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitACCDB]: Could not create handle to ACCDB");
      free(pSQL_stmt);
      return FALSE;
   }
   retcode = SQLBindParameter(HNDLAccdbDelete, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &accdb_gi.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitACCDB]: Could not bind parameter 1");
      return FALSE;
   }


   /******************************************
    for update in function EditRecordACCDBByGI
	******************************************/

   /* update all */
   /* no update for title */
   sprintf(pSQL_stmt, "update %s.accdb set db=?, name=?, access=?, chain=?, release=?, version =?, namelow =? where gi = ?", database);
   if(CreateHandle(&HNDLAccdbUpdateAll,pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitACCDB]: Could not create handle HNDLAccdbUpdateAll");
      free(pSQL_stmt);
      return FALSE;
   }
   retcode = SQLBindParameter(HNDLAccdbUpdateAll, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 0, 0, accdb_db.val, sizeof(accdb_db.val), NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitACCDB]: Could not bind parameter 1 for HNDLAccdbUpdateAll");
      return FALSE;
   }
   retcode = SQLBindParameter(HNDLAccdbUpdateAll, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 0, 0, accdb_name.val, sizeof(accdb_name.val), NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitACCDB]: Could not bind parameter 2 for HNDLAccdbUpdateAll");
      return FALSE;
   }
   retcode = SQLBindParameter(HNDLAccdbUpdateAll, 3, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 0, 0, accdb_access.val, sizeof(accdb_access.val), NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitACCDB]: Could not bind parameter 3 for HNDLAccdbUpdateAll");
      return FALSE;
   }
   retcode = SQLBindParameter(HNDLAccdbUpdateAll, 4, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 0, 0, accdb_chain.val, sizeof(accdb_chain.val), NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitACCDB]: Could not bind parameter 4 for HNDLAccdbUpdateAll");
      return FALSE;
   }
   retcode = SQLBindParameter(HNDLAccdbUpdateAll, 5, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 0, 0, accdb_release.val, sizeof(accdb_release.val), NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitACCDB]: Could not bind parameter for 5 HNDLAccdbUpdateAll");
      return FALSE;
   }
   retcode = SQLBindParameter(HNDLAccdbUpdateAll, 6, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &accdb_version.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitACCDB]: Could not bind parameter 6 for HNDLAccdbUpdateAll");
      return FALSE;
   }
   retcode = SQLBindParameter(HNDLAccdbUpdateAll, 7, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 0, 0, accdb_namelow.val, sizeof(accdb_namelow.val), NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitACCDB]: Could not bind parameter 7 for HNDLAccdbUpdateAll");
      return FALSE;
   }
   retcode = SQLBindParameter(HNDLAccdbUpdateAll, 8, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &accdb_gi.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitACCDB]: Could not bind parameter 8 for HNDLAccdbUpdateAll");
      return FALSE;
   }

   /* update db */
   sprintf(pSQL_stmt, "update %s.accdb set db=? where gi = ?", database);
   if(CreateHandle(&HNDLAccdbUpdateDb,pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitACCDB]: Could not create handle HNDLAccdbUpdateDb");
      free(pSQL_stmt);
      return FALSE;
   }
   retcode = SQLBindParameter(HNDLAccdbUpdateDb, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 0, 0, accdb_db.val, sizeof(accdb_db.val), NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitACCDB]: Could not bind parameter 1 for HNDLAccdbUpdateDb");
      return FALSE;
   }
   retcode = SQLBindParameter(HNDLAccdbUpdateDb, 2, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &accdb_gi.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitACCDB]: Could not bind parameter 2 for HNDLAccdbUpdateDb");
      return FALSE;
   }

   /* update name */
   sprintf(pSQL_stmt, "update %s.accdb set name=? where gi = ?", database);
   if(CreateHandle(&HNDLAccdbUpdateName,pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitACCDB]: Could not create handle HNDLAccdbUpdateName");
      free(pSQL_stmt);
      return FALSE;
   }
   retcode = SQLBindParameter(HNDLAccdbUpdateName, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 0, 0, accdb_name.val, sizeof(accdb_name.val), NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitACCDB]: Could not bind parameter 1 for HNDLAccdbUpdateName");
      return FALSE;
   }
   retcode = SQLBindParameter(HNDLAccdbUpdateName, 2, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &accdb_gi.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitACCDB]: Could not bind parameter 2 for HNDLAccdbUpdateName");
      return FALSE;
   }

   /* update namelow */
   sprintf(pSQL_stmt, "update %s.accdb set namelow=? where gi = ?", database);
   if(CreateHandle(&HNDLAccdbUpdateNamelow,pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitACCDB]: Could not create handle HNDLAccdbUpdateNamelow");
      free(pSQL_stmt);
      return FALSE;
   }
   retcode = SQLBindParameter(HNDLAccdbUpdateNamelow, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 0, 0, accdb_namelow.val, sizeof(accdb_namelow.val), NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitACCDB]: Could not bind parameter 1 for HNDLAccdbUpdateNamelow");
      return FALSE;
   }
   retcode = SQLBindParameter(HNDLAccdbUpdateNamelow, 2, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &accdb_gi.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitACCDB]: Could not bind parameter 2 for HNDLAccdbUpdateNamelow");
      return FALSE;
   }

   /* update access */
   sprintf(pSQL_stmt, "update %s.accdb set access=? where gi = ?", database);
   if(CreateHandle(&HNDLAccdbUpdateAccess,pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitACCDB]: Could not create handle HNDLAccdbUpdateAccess");
      free(pSQL_stmt);
      return FALSE;
   }
   retcode = SQLBindParameter(HNDLAccdbUpdateAccess, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 0, 0, accdb_access.val, sizeof(accdb_access.val), NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitACCDB]: Could not bind parameter 1 for HNDLAccdbUpdateAccess");
      return FALSE;
   }
   retcode = SQLBindParameter(HNDLAccdbUpdateAccess, 2, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &accdb_gi.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitACCDB]: Could not bind parameter 2 for HNDLAccdbUpdateAccess");
      return FALSE;
   }

   /* update release */
   sprintf(pSQL_stmt, "update %s.accdb set release=? where gi = ?", database);
   if(CreateHandle(&HNDLAccdbUpdateRelease,pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitACCDB]: Could not create handle HNDLAccdbUpdateRelease");
      free(pSQL_stmt);
      return FALSE;
   }
   retcode = SQLBindParameter(HNDLAccdbUpdateRelease, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 0, 0, accdb_release.val, sizeof(accdb_release.val), NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitACCDB]: Could not bind parameter 1 for HNDLAccdbUpdateRelease");
      return FALSE;
   }
   retcode = SQLBindParameter(HNDLAccdbUpdateRelease, 2, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &accdb_gi.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitACCDB]: Could not bind parameter 2 for HNDLAccdbUpdateRelease");
      return FALSE;
   }

   /* update version */
   sprintf(pSQL_stmt, "update %s.accdb set version=? where gi = ?", database);
   if(CreateHandle(&HNDLAccdbUpdateVersion,pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitACCDB]: Could not create handle HNDLAccdbUpdateVersion");
      free(pSQL_stmt);
      return FALSE;
   }
   retcode = SQLBindParameter(HNDLAccdbUpdateVersion, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &accdb_version.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitACCDB]: Could not bind parameter 1 for HNDLAccdbUpdateVersion");
      return FALSE;
   }
   retcode = SQLBindParameter(HNDLAccdbUpdateVersion, 2, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &accdb_gi.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitACCDB]: Could not bind parameter 2 for HNDLAccdbUpdateVersion");
      return FALSE;
   }

   /* update chain */
   sprintf(pSQL_stmt, "update %s.accdb set chain=? where gi = ?", database);
   if(CreateHandle(&HNDLAccdbUpdateChain,pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitACCDB]: Could not create handle HNDLAccdbUpdateChain");
      free(pSQL_stmt);
      return FALSE;
   }
   retcode = SQLBindParameter(HNDLAccdbUpdateChain, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 0, 0, accdb_chain.val, sizeof(accdb_release.val), NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitACCDB]: Could not bind parameter 1 for HNDLAccdbUpdateChain");
      return FALSE;
   }
   retcode = SQLBindParameter(HNDLAccdbUpdateChain, 2, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &accdb_gi.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitACCDB]: Could not bind parameter 2 for HNDLAccdbUpdateChain");
      return FALSE;
   }

   free(pSQL_stmt);
   return TRUE;
}

/* initialization of PUBSEQ for insertion of records:
 *  1) Creates a handle for the insertion command and
 *  2) binds the parameters.
 *  This function is called by InitCodeBase. To insert into
 * the PUBSEQ database, use AppendRecordPUBSEQ */
/* no longer used by AppendRecordPUBSEQ. should be removed. */
Boolean InitPUBSEQ(void)
{
   SQLRETURN retcode;
   CharPtr pSQL_stmt = (Char *) malloc (sizeof(Char) * 150);

   if(pSQL_stmt == NULL)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[InitPUBSEQ]: Could not allocate space.");
     return FALSE;
   }

   /***********************************************
   ** for inserting (gi, muid, pmid)
   ************************************************/
   /* create handle for insertion statement */
   sprintf(pSQL_stmt, "insert into %s.pubseq(gi, muid, pmid) values(?,?,?);", database);
   if(CreateHandle(&HNDLPubseqInsertGMP, pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitPUBSEQ]: Could not create handle HNDLPubseqInsertGMP");
      free(pSQL_stmt);
      return FALSE;
   }
   /* bind parameters 1 - 3 */
   retcode = SQLBindParameter(HNDLPubseqInsertGMP, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &pubseq_gi.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitPUBSEQ]: Could not bind parameter 1 for InsertGMP");
      free(pSQL_stmt);
      return FALSE;
   }
   retcode = SQLBindParameter(HNDLPubseqInsertGMP, 2, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &pubseq_muid.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitPUBSEQ]: Could not bind parameter 2 for InsertGMP");
      free(pSQL_stmt);
      return FALSE;
   }
   retcode = SQLBindParameter(HNDLPubseqInsertGMP, 3, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &pubseq_pmid.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitPUBSEQ]: Could not bind parameter 3 for InsertGMP");
      free(pSQL_stmt);
      return FALSE;
   }


   /***********************************************
   ** for updating (muid)
   ************************************************/
   /* create handle for insertion statement */
   /* must be a record with muid not filled in yet. */
   sprintf(pSQL_stmt, "update %s.pubseq set muid = ? where gi = ? and muid=0;", database);
   if(CreateHandle(&HNDLPubseqUpdateM, pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitPUBSEQ]: Could not create handle HNDLPubseqUpdateM");
      free(pSQL_stmt);
      return FALSE;
   }
   /* bind parameters 1 & 2 */
   retcode = SQLBindParameter(HNDLPubseqUpdateM, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &pubseq_muid.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitPUBSEQ]: Could not bind parameter 1 for UpdateM");
      free(pSQL_stmt);
      return FALSE;
   }
   retcode = SQLBindParameter(HNDLPubseqUpdateM, 2, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &pubseq_gi.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitPUBSEQ]: Could not bind parameter 2 for UpdateM");
      free(pSQL_stmt);
      return FALSE;
   }

   /***********************************************
   ** for updating (pmid)
   ************************************************/
   /* create handle for insertion statement */
   /* must be a record with pmid not filled in yet. */
   sprintf(pSQL_stmt, "update %s.pubseq set pmid = ? where gi = ? and pmid=0;", database);
   if(CreateHandle(&HNDLPubseqUpdateP, pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitPUBSEQ]: Could not create handle HNDLPubseqUpdateP");
      free(pSQL_stmt);
      return FALSE;
   }
   /* bind parameters 1 & 2 */
   retcode = SQLBindParameter(HNDLPubseqUpdateP, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &pubseq_pmid.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitPUBSEQ]: Could not bind parameter 1 for UpdateP");
      free(pSQL_stmt);
      return FALSE;
   }
   retcode = SQLBindParameter(HNDLPubseqUpdateP, 2, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &pubseq_gi.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitPUBSEQ]: Could not bind parameter 2 for UpdateP");
      free(pSQL_stmt);
      return FALSE;
   }

   /***********************************************
   ** delete
   ************************************************/
   sprintf(pSQL_stmt, "delete from %s.pubseq where gi=?", database);
   if(CreateHandle(&HNDLPubseqDelete, pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitPUBSEQ]: Could not create handle HNDLPubseqDelete");
      free(pSQL_stmt);
      return FALSE;
   }
   retcode = SQLBindParameter(HNDLPubseqDelete, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &pubseq_gi.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitPUBSEQ]: Could not bind parameter 1 for HNDLPubseqDelete");
      free(pSQL_stmt);
      return FALSE;
   }

   /***********************************************
   ** for searching on gi
   ************************************************/
   /* create handle for insertion statement */
   /* must be a record with either muid or pmid or both not filled in yet. */
   /* use this query in order to avoid presence of records such as
      gi     muid     pmid
	  8903   3412     7309
	  8903   3412     0
	  NOTE: muid and pmid are one-to-one related */
   sprintf(pSQL_stmt, "select gi from %s.pubseq where gi=? and ((muid=? and pmid<>0) or (pmid=? and muid<>0) or muid=0 or pmid=0);", database);
   if(CreateHandle(&HNDLPubseqSearchGi, pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitPUBSEQ]: Could not create handle HNDLPubseqSearchGi");
      free(pSQL_stmt);
      return FALSE;
   }
   /* bind parameters 1 */
   retcode = SQLBindParameter(HNDLPubseqSearchGi, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &pubseq_gi.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitPUBSEQ]: Could not bind parameter 1 for SearchGi");
      free(pSQL_stmt);
      return FALSE;
   }
   /* bind parameters 2 */
   retcode = SQLBindParameter(HNDLPubseqSearchGi, 2, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &pubseq_muid.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitPUBSEQ]: Could not bind parameter 2 for SearchGi");
      free(pSQL_stmt);
      return FALSE;
   }
   /* bind parameters 3 */
   retcode = SQLBindParameter(HNDLPubseqSearchGi, 3, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &pubseq_pmid.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitPUBSEQ]: Could not bind parameter 3 for SearchGi");
      free(pSQL_stmt);
      return FALSE;
   }

   free(pSQL_stmt);

   return TRUE;
}

/* initialization of SENGI for insertion of records:
 *  1) Creates a handle for the insertion command and
 *  2) binds the parameters.
 *  This function is called by InitCodeBase. To insert into
 * the SENGI database, use AppendRecordSENGI */
Boolean InitSENGI(void)
{
   SQLRETURN retcode;
   CharPtr pSQL_stmt = (Char *) malloc(sizeof(Char) * 150);
   if(pSQL_stmt == NULL)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitSENGI]: Could not allocate space");
      return FALSE;
   }

   /***********************************************
   ** insert
   ************************************************/
   /* create handle */
   sprintf((char*) pSQL_stmt, "insert into %s.sengi(gi, seid, division) values(?,?,?);", database);
   if(CreateHandle(&HNDLSengi, pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitSENGI]: Could not create handle");
      free(pSQL_stmt);
      return FALSE;
   }

   /* bind parameters 1 - 3*/
   retcode = SQLBindParameter(HNDLSengi, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &sengi_gi.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitSENGI]: Could not bind parameter 1");
      free(pSQL_stmt);
      return FALSE;
   }
   retcode = SQLBindParameter(HNDLSengi, 2, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &sengi_seid.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitSENGI]: Could not bind parameter 2");
      free(pSQL_stmt);
      return FALSE;
   }

   retcode = SQLBindParameter(HNDLSengi, 3, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 0, 0, sengi_div.val, sizeof(sengi_div.val), NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitSENGI]: Could not bind parameter 3");
      free(pSQL_stmt);
      return FALSE;
   }

   /***********************************************
   ** search by gi
   ************************************************/
   sprintf((char*) pSQL_stmt, "select seid from %s.sengi where gi=?;", database);
   if(CreateHandle(&HNDLSengiSearchGi, pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitSENGI]: Could not create handle");
      free(pSQL_stmt);
      return FALSE;
   }

   retcode = SQLBindParameter(HNDLSengiSearchGi, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &sengi_gi.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitSENGI]: Could not bind parameter 1");
      free(pSQL_stmt);
      return FALSE;
   }

   /***********************************************
   ** delete
   ************************************************/
   sprintf((char*) pSQL_stmt, "delete from %s.sengi where gi=?", database);
   if(CreateHandle(&HNDLSengiDelete, pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitSENGI]: Could not create handle");
      free(pSQL_stmt);
      return FALSE;
   }

   retcode = SQLBindParameter(HNDLSengiDelete, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &sengi_gi.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitSENGI]: Could not bind parameter 1");
      free(pSQL_stmt);
      return FALSE;
   }

   free(pSQL_stmt);
   return TRUE;
}

/* initialization of SENDB for insertion of records:
 *  1) Creates a handle for the insertion command and
 *  2) binds the parameters.
 *  This function is called by InitCodeBase. To insert into
 * the SENDB database, use AppendRecordSENDB */
Boolean InitSENDB(void)
{
   SQLRETURN retcode;
   CharPtr pSQL_stmt = (char *) malloc (sizeof(Char) * 150);
   if(pSQL_stmt == NULL)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitSENDB]: Could not allocate space");
      return FALSE;
   }

   /***********************************************
   ** insert
   ************************************************/
   /* create the handle */
   sprintf(pSQL_stmt, "insert into %s.sendb(seid, asn1) values(?, ?);", database);
   if(CreateHandle(&HNDLSendb, pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitSENDB]: Could not create handle");
      free(pSQL_stmt);
      return FALSE;
   }

   /* bind parameter 1 */
   retcode = SQLBindParameter(HNDLSendb, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &sendb_seid.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitSENDB]: Could not bind parameter 1");
      free(pSQL_stmt);
      return FALSE;
   }

   /* A blob object. Cannot bind parameter here because blob sizes change
    * all the time, instead bind parameter in Append function: see AppendRecordSENDB. */
   /* SQLBindParameter(HNDLSendb, 2, SQL_PARAM_INPUT, SQL_C_DEFAULT, SQL_LONGVARBINARY, 0, 0, sendb_asn.val, 0, (long*)&sendb_asn.len); */


   /***********************************************
   ** delete
   ************************************************/
   /* create the handle */
   sprintf(pSQL_stmt, "delete from %s.sendb where seid=?", database);
   if(CreateHandle(&HNDLSendbDelete, pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitSENDB]: Could not create handle");
      free(pSQL_stmt);
      return FALSE;
   }

   /* bind parameter 1 */
   retcode = SQLBindParameter(HNDLSendbDelete, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &sendb_seid.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitSENDB]: Could not bind parameter 1");
      free(pSQL_stmt);
      return FALSE;
   }

   /***********************************************
   ** search * for Gi, used in SearchSENDBforGI
   ************************************************/
   /* create the handle */
   sprintf(pSQL_stmt, "select s.seid, s.asn1 from %s.sendb as s, %s.sengi as g where s.seid = g.seid and g.gi = ?", database, database);
   if(CreateHandle(&HNDLSendbSearchGi, pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitSENDB]: Could not create handle HNDLSendbSearchGi");
      free(pSQL_stmt);
      return FALSE;
   }

   /* bind parameter 1 */
   retcode = SQLBindParameter(HNDLSendbSearchGi, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &sengi_gi.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitSENDB]: Could not bind parameter 1 for HNDLSendbSearchGi");
      free(pSQL_stmt);
      return FALSE;
   }

   free(pSQL_stmt);
   return TRUE;
}

Boolean InitSOURCEFILE(void)
{
   SQLRETURN retcode;
   CharPtr pSQL_stmt = (Char *) malloc(sizeof(Char) * 150);
   if(pSQL_stmt == NULL)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[InitSOURCEFILE]: Could not allocate space");
     return FALSE;
   }

   /***********************************************
   ** insert
   ************************************************/
   sprintf((char *) pSQL_stmt, "insert ignore into %s.sourcefile(gi, "
		   "filename, date ) values(?, ?, NOW() );", database);
   if(CreateHandle(&HNDLSourcefileInsert, pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitSOURCEFILE]: Could not create handle.");
      free(pSQL_stmt);
      return FALSE;
   }

   retcode = SQLBindParameter(HNDLSourcefileInsert, 1, SQL_PARAM_INPUT, 
							  SQL_C_SLONG, SQL_INTEGER, 0, 0,
							  &sourcefile_gi.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitSOURCEFILE]: Could not bind "
				"parameter 1, gi");
      free(pSQL_stmt);
	  SQLFreeHandle(SQL_HANDLE_STMT, HNDLSourcefileInsert);
      return FALSE;
   }

   retcode = SQLBindParameter(HNDLSourcefileInsert, 2, SQL_PARAM_INPUT, 
							  SQL_C_CHAR, SQL_VARCHAR, 0, 0,
							  sourcefile_filename.val, 
							  sizeof(sourcefile_filename.val), 
							  (int *)sourcefile_filename.len);

   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitSOURCEFILE]: Could not bind "
				"parameter 2");
      free(pSQL_stmt);
	  SQLFreeHandle(SQL_HANDLE_STMT, HNDLSourcefileInsert);
      return FALSE;
   }

   return TRUE;
}
 

/* initialization of MMDB for insertion of records:
 *  1) Creates a handle for the insertion command and
 *  2) binds the parameters.
 *  This function is called by InitCodeBase. To insert into
 * the MMDB database, use AppendRecordMMDB*/
Boolean InitMMDB(void)
{
   SQLRETURN retcode;
   CharPtr pSQL_stmt = (char *) malloc (sizeof(Char) * 150);
   if(pSQL_stmt == NULL)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitMMDB]: Could not allocate space");
      return FALSE;
   }

   /* create the handle */
   sprintf(pSQL_stmt, "insert into %s.mmdb(mmdbid,asn1,pdbid,bwhat,models,molecules,size,bzsize) values(?,?,?,?,?,?,?,?);", database);
   if(CreateHandle(&HNDLMmdb, pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitMMDB]: Could not create handle");
      free(pSQL_stmt);
      return FALSE;
   }
   free(pSQL_stmt);

   /* bind parameter 1 */
   retcode = SQLBindParameter(HNDLMmdb, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &mmdb_mmdbid.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitMMDB]: Could not bind parameter 1");
      return FALSE;
   }

   /* A blob object. Cannot bind parameter here because blob sizes change
    * all the time, instead bind parameter in Append function: see AppendRecordMMDB. */
   /* SQLBindParameter(HNDLMmdb, 2, SQL_PARAM_INPUT, SQL_C_DEFAULT, SQL_LONGVARBINARY, 0, 0, mmdb_asn.val, 0, (long*)&mmdb_asn.len); */


    /* bind parameter 3 */
   retcode = SQLBindParameter(HNDLMmdb, 3, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 0, 0, mmdb_pdbid.val, sizeof(mmdb_pdbid.val), NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitMMDB]: Could not bind parameter 3");
      return FALSE;
   }

   /* bind parameter 4 */
   retcode = SQLBindParameter(HNDLMmdb, 4, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &mmdb_bwhat.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitMMDB]: Could not bind parameter 4");
      return FALSE;
   }

   /* bind parameter 5 */
   retcode = SQLBindParameter(HNDLMmdb, 5, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &mmdb_models.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitMMDB]: Could not bind parameter 5");
      return FALSE;
   }

   /* bind parameter 6 */
   retcode = SQLBindParameter(HNDLMmdb, 6, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &mmdb_molecules.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitMMDB]: Could not bind parameter 6");
      return FALSE;
   }

   /* bind parameter 7 */
   retcode = SQLBindParameter(HNDLMmdb, 7, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &mmdb_size.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitMMDB]: Could not bind parameter 7");
      return FALSE;
   }

   /* bind parameter 8 */
   retcode = SQLBindParameter(HNDLMmdb, 8, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &mmdb_bzsize.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitMMDB]: Could not bind parameter 8");
      return FALSE;
   }

   return TRUE;
}

/* initialization of MMGI for insertion of records:
 *  1) Creates a handle for the insertion command and
 *  2) binds the parameters.
 *  This function is called by InitCodeBase. To insert into
 * the MMGI database, use AppendRecordMMGI */
Boolean InitMMGI(void)
{
   SQLRETURN retcode;
   CharPtr pSQL_stmt = (char *) malloc (sizeof(Char) * 150);
   if(pSQL_stmt == NULL)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitMMGI]: Could not allocate space");
      return FALSE;
   }

   /* create the handle */
   sprintf(pSQL_stmt, "insert into %s.mmgi(mmdbid,gi) values(?,?);", database);
   if(CreateHandle(&HNDLMmgi, pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitMMGI]: Could not create handle");
      free(pSQL_stmt);
      return FALSE;
   }
   free(pSQL_stmt);

   /* bind parameter 1 */
   retcode = SQLBindParameter(HNDLMmgi, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &mmgi_mmdbid.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitMMGI]: Could not bind parameter 1");
      return FALSE;
   }

   /* bind parameter 2 */
   retcode = SQLBindParameter(HNDLMmgi, 2, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &mmgi_gi.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitMMGI]: Could not bind parameter 2");
      return FALSE;
   }


   return TRUE;
}

/* initialization of DOMDB for insertion of records:
 *  1) Creates a handle for the insertion command and
 *  2) binds the parameters.
 *  This function is called by InitCodeBase. To insert into
 * the DOMDB database, use AppendRecordDOMDB */
Boolean InitDOMDB(void)
{
   SQLRETURN retcode;
   CharPtr pSQL_stmt = (char *) malloc (sizeof(Char) * 150);
   if(pSQL_stmt == NULL)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitDOMDB]: Could not allocate space");
      return FALSE;
   }

   /******************************************
    insert
	******************************************/
   /* create the handle */
   sprintf(pSQL_stmt, "insert into %s.domdb(mmdbid,asn1,pdbid,pdbchain,gi,domno,domall,domid,rep) values(?,?,?,?,?,?,?,?,?);", database);
   if(CreateHandle(&HNDLDomdb, pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitDOMDB]: Could not create handle");
      free(pSQL_stmt);
      return FALSE;
   }

   /* bind parameter 1 */
   retcode = SQLBindParameter(HNDLDomdb, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &domdb_mmdbid.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitDOMDB]: Could not bind parameter 1");
      return FALSE;
   }

   /* A blob object. Cannot bind parameter here because blob sizes change
    * all the time, instead bind parameter in Append function: see AppendRecordMMDB. */
   /* SQLBindParameter(HNDLDomdb, 2, SQL_PARAM_INPUT, SQL_C_DEFAULT, SQL_LONGVARBINARY, 0, 0, domdb_asn.val, 0, (long*)&domdb_asn.len); */


    /* bind parameter 3 */
   retcode = SQLBindParameter(HNDLDomdb, 3, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 0, 0, domdb_pdbid.val, sizeof(domdb_pdbid.val), NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitDOMDB]: Could not bind parameter 3");
      return FALSE;
   }

   /* bind parameter 4 */
   retcode = SQLBindParameter(HNDLDomdb, 4, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 0, 0, domdb_chain.val, sizeof(domdb_chain.val), NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitDOMDB]: Could not bind parameter 4");
      return FALSE;
   }

   /* bind parameter 5 */
   retcode = SQLBindParameter(HNDLDomdb, 5, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &domdb_gi.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitDOMDB]: Could not bind parameter 5");
      return FALSE;
   }

   /* bind parameter 6 */
   retcode = SQLBindParameter(HNDLDomdb, 6, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &domdb_domno.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitDOMDB]: Could not bind parameter 6");
      return FALSE;
   }

   /* bind parameter 7 */
   retcode = SQLBindParameter(HNDLDomdb, 7, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &domdb_domall.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitDOMDB]: Could not bind parameter 7");
      return FALSE;
   }

   /* bind parameter 8 */
   retcode = SQLBindParameter(HNDLDomdb, 8, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &domdb_domid.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitDOMDB]: Could not bind parameter 8");
      return FALSE;
   }

   /* bind parameter 9 */
   retcode = SQLBindParameter(HNDLDomdb, 9, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &domdb_rep.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitDOMDB]: Could not bind parameter 9");
      return FALSE;
   }

   /******************************************
    select count
	******************************************/
   sprintf(pSQL_stmt, "select count(*) as count from %s.domdb;", database);
   if(CreateHandle(&HNDLDomdbSelectCount,pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitDOMDB]: Could not create handle HNDLDomdbSelectCount");
      free(pSQL_stmt);
      return FALSE;
   }

   /*********************************************
    for search in function EditRecordDOMDBByDOMID
	*********************************************/
   sprintf(pSQL_stmt, "select mmdbid, pdbid, pdbchain, gi, domno, domall, domid, rep from %s.domdb where domid=?;", database);
   if(CreateHandle(&HNDLDomdbSearchDomid,pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitDOMDB]: Could not create handle HNDLDomdbSearchDomid");
      free(pSQL_stmt);
      return FALSE;
   }

   retcode = SQLBindParameter(HNDLDomdbSearchDomid, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &domdb_domid.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitDOMDB]: Could not bind parameter 1 for HNDLDomdbSearchDomid");
      return FALSE;
   }

   /******************************************
    for search in function GetRecDOMDB
	******************************************/
   sprintf(pSQL_stmt, "select mmdbid, pdbid, pdbchain, gi, domno, domall, domid, rep from %s.domdb where rowid=?;", database);
   if(CreateHandle(&HNDLDomdbSearchRowid,pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitDOMDB]: Could not create handle HNDLDomdbSearchRowid");
      free(pSQL_stmt);
      return FALSE;
   }

   retcode = SQLBindParameter(HNDLDomdbSearchRowid, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &rowid.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitDOMDB]: Could not bind parameter 1 for HNDLDomdbSearchRowid");
      return FALSE;
   }

   /*********************************************
    for update in function EditRecordDOMDBByDOMID
	*********************************************/

   /* update all */
   /* no update for asn.1 */
   sprintf(pSQL_stmt, "update %s.domdb set mmdbid=?, pdbid=?, pdbchain=?, gi=?, domno=?, domall=?, rep=? where domid=?", database);
   if(CreateHandle(&HNDLDomdbUpdateAll,pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitDOMDB]: Could not create handle HNDLDomdbUpdateAll");
      free(pSQL_stmt);
      return FALSE;
   }
   retcode = SQLBindParameter(HNDLDomdbUpdateAll, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &domdb_mmdbid.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitDOMDB]: Could not bind parameter 1 for HNDLDomdbUpdateAll");
      return FALSE;
   }
   retcode = SQLBindParameter(HNDLDomdbUpdateAll, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 0, 0, domdb_pdbid.val, sizeof(domdb_pdbid.val), NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitDOMDB]: Could not bind parameter 2 for HNDLDomdbUpdateAll");
      return FALSE;
   }
   retcode = SQLBindParameter(HNDLDomdbUpdateAll, 3, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 0, 0, domdb_chain.val, sizeof(domdb_chain.val), NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitDOMDB]: Could not bind parameter 3 for HNDLDomdbUpdateAll");
      return FALSE;
   }
   retcode = SQLBindParameter(HNDLDomdbUpdateAll, 4, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &domdb_gi.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitDOMDB]: Could not bind parameter 4for HNDLDomdbUpdateAll");
      return FALSE;
   }
   retcode = SQLBindParameter(HNDLDomdbUpdateAll, 5, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &domdb_domno.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitDOMDB]: Could not bind parameter 5 for HNDLDomdbUpdateAll");
      return FALSE;
   }
   retcode = SQLBindParameter(HNDLDomdbUpdateAll, 6, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &domdb_domall.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitDOMDB]: Could not bind parameter 6 for HNDLDomdbUpdateAll");
      return FALSE;
   }
   retcode = SQLBindParameter(HNDLDomdbUpdateAll, 7, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &domdb_rep.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitDOMDB]: Could not bind parameter 7 for HNDLDomdbUpdateAll");
      return FALSE;
   }
   retcode = SQLBindParameter(HNDLDomdbUpdateAll, 8, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &domdb_domid.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitDOMDB]: Could not bind parameter 8 for HNDLDomdbUpdateAll");
      return FALSE;
   }

   /* update mmdbid */
   sprintf(pSQL_stmt, "update %s.domdb set mmdbid=? where domid = ?", database);
   if(CreateHandle(&HNDLDomdbUpdateMmdbid,pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitDOMDB]: Could not create handle HNDLDomdbUpdateMmdbid");
      free(pSQL_stmt);
      return FALSE;
   }
   retcode = SQLBindParameter(HNDLDomdbUpdateMmdbid, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &domdb_mmdbid.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitDOMDB]: Could not bind parameter 1 for HNDLDomdbUpdateMmdbid");
      return FALSE;
   }
   retcode = SQLBindParameter(HNDLDomdbUpdateMmdbid, 2, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &domdb_domid.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitDOMDB]: Could not bind parameter 2 for HNDLDomdbUpdateMmdbid");
      return FALSE;
   }

   /* update pdbid */
   sprintf(pSQL_stmt, "update %s.domdb set pdbid=? where domid = ?", database);
   if(CreateHandle(&HNDLDomdbUpdatePdbid,pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitDOMDB]: Could not create handle HNDLDomdbUpdatePdbid");
      free(pSQL_stmt);
      return FALSE;
   }
   retcode = SQLBindParameter(HNDLDomdbUpdatePdbid, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 0, 0, domdb_pdbid.val, sizeof(domdb_pdbid.val), NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitDOMDB]: Could not bind parameter 1 for HNDLDomdbUpdatePdbid");
      return FALSE;
   }
   retcode = SQLBindParameter(HNDLDomdbUpdatePdbid, 2, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &domdb_domid.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitDOMDB]: Could not bind parameter 2 for HNDLDomdbUpdatePdbid");
      return FALSE;
   }

   /* update chain */
   sprintf(pSQL_stmt, "update %s.domdb set pdbchain=? where domid = ?", database);
   if(CreateHandle(&HNDLDomdbUpdateChain,pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitDOMDB]: Could not create handle HNDLDomdbUpdateChain");
      free(pSQL_stmt);
      return FALSE;
   }
   retcode = SQLBindParameter(HNDLDomdbUpdateChain, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 0, 0, domdb_chain.val, sizeof(domdb_chain.val), NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitDOMDB]: Could not bind parameter 1 for HNDLDomdbUpdateChain");
      return FALSE;
   }
   retcode = SQLBindParameter(HNDLDomdbUpdateChain, 2, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &domdb_domid.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitDOMDB]: Could not bind parameter 2 for HNDLDomdbUpdateChain");
      return FALSE;
   }

   /* update gi */
   sprintf(pSQL_stmt, "update %s.domdb set gi=? where domid = ?", database);
   if(CreateHandle(&HNDLDomdbUpdateGi,pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitDOMDB]: Could not create handle HNDLDomdbUpdateGi");
      free(pSQL_stmt);
      return FALSE;
   }
   retcode = SQLBindParameter(HNDLDomdbUpdateGi, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &domdb_gi.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitDOMDB]: Could not bind parameter 1 for HNDLDomdbUpdateGi");
      return FALSE;
   }
   retcode = SQLBindParameter(HNDLDomdbUpdateGi, 2, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &domdb_domid.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitDOMDB]: Could not bind parameter 2 for HNDLDomdbUpdateGi");
      return FALSE;
   }

   /* update domno */
   sprintf(pSQL_stmt, "update %s.domdb set domno=? where domid = ?", database);
   if(CreateHandle(&HNDLDomdbUpdateDomno,pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitDOMDB]: Could not create handle HNDLDomdbUpdateDomno");
      free(pSQL_stmt);
      return FALSE;
   }
   retcode = SQLBindParameter(HNDLDomdbUpdateDomno, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &domdb_domno.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitDOMDB]: Could not bind parameter 1 for HNDLDomdbUpdateDomno");
      return FALSE;
   }
   retcode = SQLBindParameter(HNDLDomdbUpdateDomno, 2, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &domdb_domid.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitDOMDB]: Could not bind parameter 2 for HNDLDomdbUpdateDomno");
      return FALSE;
   }

   /* update domall */
   sprintf(pSQL_stmt, "update %s.domdb set domall=? where domid = ?", database);
   if(CreateHandle(&HNDLDomdbUpdateDomall,pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitDOMDB]: Could not create handle HNDLDomdbUpdateDomall");
      free(pSQL_stmt);
      return FALSE;
   }
   retcode = SQLBindParameter(HNDLDomdbUpdateDomall, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &domdb_domall.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitDOMDB]: Could not bind parameter 1 for HNDLDomdbUpdateDomall");
      return FALSE;
   }
   retcode = SQLBindParameter(HNDLDomdbUpdateDomall, 2, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &domdb_domid.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitDOMDB]: Could not bind parameter 2 for HNDLDomdbUpdateDomall");
      return FALSE;
   }

   /* update rep */
   sprintf(pSQL_stmt, "update %s.domdb set rep=? where domid = ?", database);
   if(CreateHandle(&HNDLDomdbUpdateRep,pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitDOMDB]: Could not create handle HNDLDomdbUpdateRep");
      free(pSQL_stmt);
      return FALSE;
   }
   retcode = SQLBindParameter(HNDLDomdbUpdateRep, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &domdb_rep.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitDOMDB]: Could not bind parameter 1 for HNDLDomdbUpdateRep");
      return FALSE;
   }
   retcode = SQLBindParameter(HNDLDomdbUpdateRep, 2, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &domdb_domid.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitDOMDB]: Could not bind parameter 2 for HNDLDomdbUpdateRep");
      return FALSE;
   }


   free(pSQL_stmt);
   return TRUE;
}

/* initialization of CDDB for insertion of records:
 *  1) Creates a handle for the insertion command and
 *  2) binds the parameters.
 *  This function is called by InitCodeBase. To insert into
 * the CDDB database, use AppendRecordCDDB */
Boolean InitCDDB(void)
{
   SQLRETURN retcode;
   CharPtr pSQL_stmt = (char *) malloc (sizeof(Char) * 150);
   if(pSQL_stmt == NULL)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitCDDB]: Could not allocate space");
      return FALSE;
   }

   /* create the handle */
   sprintf(pSQL_stmt, "insert into %s.cddb(gi,asn1) values(?,?);", database);
   if(CreateHandle(&HNDLCddb, pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitCDDB]: Could not create handle");
      free(pSQL_stmt);
      return FALSE;
   }
   free(pSQL_stmt);

   /* bind parameter 1 */
   retcode = SQLBindParameter(HNDLCddb, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &cddb_gi.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitCDDB]: Could not bind parameter 1");
      return FALSE;
   }

   /* A blob object. Cannot bind parameter here because blob sizes change
    * all the time, instead bind parameter in Append function: see AppendRecordMMDB. */
   /* SQLBindParameter(HNDLCddb, 2, SQL_PARAM_INPUT, SQL_C_DEFAULT, SQL_LONGVARBINARY, 0, 0, cddb_asn.val, 0, (long*)&cddb_asn.len); */

   return TRUE;
}

/*originally intializing codebase involves setting up variable
that deal with the codebase environment.  Now the intializaion
involves connecting to the database using the cli interface.
And setting up global varables. (insert globals should only
be intialized depending on a flag --> TO DO*/
/* RAC I don't think that Global variables need to be initialized here
   Why don't initialize inside the function that use it?
   Create Handle need to handle Errors.
*/
Boolean LIBCALL InitCodeBase(void)
{
   SQLINTEGER retcode;

   if(!ConnectToDb())
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitCodeBase]: Could not Connect to database.");
      return FALSE;
   }

   /* turn autocommit on: optimization for the intial Load of data.
      Might consider making a function that stores all the load specific
      parameters and is activated by a flag when calling init*/
   /*  = SQLSetConnectAttr(hdbc, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_ON, SQL_NTS);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitCodeBase]: Could not set connection attribute.");
      return FALSE;
	  }*/

   if(!InitASNDB()){
      ErrPostEx(SEV_ERROR, 0, 0, "[InitCodeBase]: Could not initialize ASNDB");
      return FALSE;
   }
   if(!InitPARTI())
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitCodeBase]: Could not initialize PARTI");
      return FALSE;
   }
   if(!InitTAXGI())
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitCodeBase]: Could not initialize TAXGI");
      return FALSE;
   }
   if(!InitNUCPROT())
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitCodeBase]: Could not initialize NUCPROT");
      return FALSE;
   }
   if(!InitACCDB())
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitCodeBase]: Could not initialize ACCDB");
      return FALSE;
   }
   if(!InitPUBSEQ())
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitCodeBase]: Could not initialize PUBSEQ");
      return FALSE;
   }
   if(!InitSENGI())
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitCodeBase]: Could not initialize SENGI");
      return FALSE;
   }
   if(!InitSENDB())
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitCodeBase]: Could not initialize SENDB");
      return FALSE;
   }
   if(!InitREDUND())
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitCodeBase]: Could not initialize REDUND");
      return FALSE;
   }

  
   if(!InitSOURCEFILE())
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitCodeBase]: Could not initialize SOURCEFILE");
      return FALSE;
   }
   
   if(!InitMMDB())
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitCodeBase]: Could not initialize MMDB");
      return FALSE;
   }

   if(!InitMMGI())
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitCodeBase]: Could not initialize MMGI");
      return FALSE;
   }

   if(!InitDOMDB())
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitCodeBase]: Could not initialize DOMDB");
      return FALSE;
   }

   if(!InitCDDB())
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitCodeBase]: Could not initialize CDDB");
      return FALSE;
   }

   return TRUE;
}


Boolean CloseCodeBase(void)
{
   Int2 sqlrc = SQL_SUCCESS;

   /* commit all changes */
   SQLTransact(henv, hdbc, SQL_COMMIT);

   /* Free statement handles. */
   if (SQLFreeHandle(SQL_HANDLE_STMT, HNDLAsndb) != SQL_SUCCESS)
       print_err(&HNDLAsndb);
   if (SQLFreeHandle(SQL_HANDLE_STMT, HNDLAsndbSearchGi) != SQL_SUCCESS)
       print_err(&HNDLAsndbSearchGi);
   if (SQLFreeHandle(SQL_HANDLE_STMT, HNDLParti) != SQL_SUCCESS)
       print_err(&HNDLParti);
   if (SQLFreeHandle(SQL_HANDLE_STMT, HNDLPartiSearchGi) != SQL_SUCCESS)
       print_err(&HNDLPartiSearchGi);
   if (SQLFreeHandle(SQL_HANDLE_STMT, HNDLNucprot) != SQL_SUCCESS)
       print_err(&HNDLNucprot);
   if (SQLFreeHandle(SQL_HANDLE_STMT, HNDLAccdb) != SQL_SUCCESS)
       print_err(HNDLAccdb);
   if (SQLFreeHandle(SQL_HANDLE_STMT, HNDLPubseqInsertGMP) != SQL_SUCCESS)
       print_err(HNDLPubseqInsertGMP);
   if (SQLFreeHandle(SQL_HANDLE_STMT, HNDLPubseqUpdateM) != SQL_SUCCESS)
       print_err(HNDLPubseqUpdateM);
   if (SQLFreeHandle(SQL_HANDLE_STMT, HNDLPubseqUpdateP) != SQL_SUCCESS)
       print_err(HNDLPubseqUpdateP);
   if (SQLFreeHandle(SQL_HANDLE_STMT, HNDLPubseqSearchGi) != SQL_SUCCESS)
       print_err(HNDLPubseqSearchGi);
   if (SQLFreeHandle(SQL_HANDLE_STMT, HNDLTaxgi) != SQL_SUCCESS)
       print_err(HNDLTaxgi);
   if (SQLFreeHandle(SQL_HANDLE_STMT, HNDLSendb) != SQL_SUCCESS)
       print_err(HNDLSendb);
   if (SQLFreeHandle(SQL_HANDLE_STMT, HNDLSendbSearchGi) != SQL_SUCCESS)
       print_err(HNDLSendbSearchGi);
   if (SQLFreeHandle(SQL_HANDLE_STMT, HNDLSengi) != SQL_SUCCESS)
       print_err(HNDLSengi);
   if (SQLFreeHandle(SQL_HANDLE_STMT, HNDLRedund) != SQL_SUCCESS)
       print_err(HNDLRedund);

   if (SQLFreeHandle(SQL_HANDLE_STMT, HNDLAsndbDelete) != SQL_SUCCESS)
       print_err(&HNDLAsndbDelete);
   if (SQLFreeHandle(SQL_HANDLE_STMT, HNDLPartiDelete) != SQL_SUCCESS)
       print_err(&HNDLPartiDelete);
   if (SQLFreeHandle(SQL_HANDLE_STMT, HNDLNucprotDeleteGin) != SQL_SUCCESS)
       print_err(&HNDLNucprotDeleteGin);
   if (SQLFreeHandle(SQL_HANDLE_STMT, HNDLNucprotDeleteGia) != SQL_SUCCESS)
       print_err(&HNDLNucprotDeleteGia);
   if (SQLFreeHandle(SQL_HANDLE_STMT, HNDLAccdbDelete) != SQL_SUCCESS)
       print_err(HNDLAccdbDelete);
   if (SQLFreeHandle(SQL_HANDLE_STMT, HNDLPubseqDelete) != SQL_SUCCESS)
       print_err(HNDLPubseqDelete);
   if (SQLFreeHandle(SQL_HANDLE_STMT, HNDLTaxgiDelete) != SQL_SUCCESS)
       print_err(HNDLTaxgiDelete);
   if (SQLFreeHandle(SQL_HANDLE_STMT, HNDLSendbDelete) != SQL_SUCCESS)
       print_err(HNDLSendbDelete);
   if (SQLFreeHandle(SQL_HANDLE_STMT, HNDLSengiDelete) != SQL_SUCCESS)
       print_err(HNDLSengiDelete);
   if (SQLFreeHandle(SQL_HANDLE_STMT, HNDLSengiSearchGi) != SQL_SUCCESS)
       print_err(HNDLSengiSearchGi);

  
   if (SQLFreeHandle(SQL_HANDLE_STMT, HNDLSourcefileInsert) != SQL_SUCCESS)
	 print_err(HNDLSourcefileInsert);

   if (SQLFreeHandle(SQL_HANDLE_STMT, HNDLMmdb) != SQL_SUCCESS)
       print_err(HNDLMmdb);
   if (SQLFreeHandle(SQL_HANDLE_STMT, HNDLMmgi) != SQL_SUCCESS)
       print_err(HNDLMmgi);
   if (SQLFreeHandle(SQL_HANDLE_STMT, HNDLDomdb) != SQL_SUCCESS)
       print_err(HNDLDomdb);
   if (SQLFreeHandle(SQL_HANDLE_STMT, HNDLDomdbSelectCount) != SQL_SUCCESS)
       print_err(HNDLDomdbSelectCount);
   if (SQLFreeHandle(SQL_HANDLE_STMT, HNDLDomdbSearchRowid) != SQL_SUCCESS)
       print_err(HNDLDomdbSearchRowid);
   if (SQLFreeHandle(SQL_HANDLE_STMT, HNDLCddb) != SQL_SUCCESS)
       print_err(HNDLCddb);

   if (SQLFreeHandle(SQL_HANDLE_STMT, HNDLAccdbSelectCount) != SQL_SUCCESS)
       print_err(HNDLAccdbSelectCount);
   if (SQLFreeHandle(SQL_HANDLE_STMT, HNDLAccdbSearchRowid) != SQL_SUCCESS)
       print_err(HNDLAccdbSearchRowid);
   if (SQLFreeHandle(SQL_HANDLE_STMT, HNDLAccdbSearchGi) != SQL_SUCCESS)
       print_err(HNDLAccdbSearchGi);
   if (SQLFreeHandle(SQL_HANDLE_STMT, HNDLAccdbUpdateAll) != SQL_SUCCESS)
       print_err(HNDLAccdbUpdateAll);
   if (SQLFreeHandle(SQL_HANDLE_STMT, HNDLAccdbUpdateDb) != SQL_SUCCESS)
       print_err(HNDLAccdbUpdateDb);
   if (SQLFreeHandle(SQL_HANDLE_STMT, HNDLAccdbUpdateName) != SQL_SUCCESS)
       print_err(HNDLAccdbUpdateName);
   if (SQLFreeHandle(SQL_HANDLE_STMT, HNDLAccdbUpdateNamelow) != SQL_SUCCESS)
       print_err(HNDLAccdbUpdateNamelow);
   if (SQLFreeHandle(SQL_HANDLE_STMT, HNDLAccdbUpdateAccess) != SQL_SUCCESS)
       print_err(HNDLAccdbUpdateAccess);
   if (SQLFreeHandle(SQL_HANDLE_STMT, HNDLAccdbUpdateRelease) != SQL_SUCCESS)
       print_err(HNDLAccdbUpdateRelease);
   if (SQLFreeHandle(SQL_HANDLE_STMT, HNDLAccdbUpdateVersion) != SQL_SUCCESS)
       print_err(HNDLAccdbUpdateVersion);
   if (SQLFreeHandle(SQL_HANDLE_STMT, HNDLAccdbUpdateChain) != SQL_SUCCESS)
       print_err(HNDLAccdbUpdateChain);
   if (SQLFreeHandle(SQL_HANDLE_STMT, HNDLAccdbSearchAccess) != SQL_SUCCESS)
       print_err(HNDLAccdbSearchAccess);
   if (SQLFreeHandle(SQL_HANDLE_STMT, HNDLAccdbGetAccessByGi) != SQL_SUCCESS)
	 print_err(HNDLAccdbGetAccessByGi);

   if (SQLFreeHandle(SQL_HANDLE_STMT, HNDLAccdbGetGiByName) != SQL_SUCCESS)
	 print_err(HNDLAccdbGetGiByName);

   if (SQLFreeHandle(SQL_HANDLE_STMT, HNDLTaxgiSearchGi) != SQL_SUCCESS)
       print_err(HNDLTaxgiSearchGi);
   if (SQLFreeHandle(SQL_HANDLE_STMT, HNDLTaxgiUpdateAll) != SQL_SUCCESS)
       print_err(HNDLTaxgiUpdateAll);
   if (SQLFreeHandle(SQL_HANDLE_STMT, HNDLTaxgiUpdateTaxid) != SQL_SUCCESS)
       print_err(HNDLTaxgiUpdateTaxid);
   if (SQLFreeHandle(SQL_HANDLE_STMT, HNDLTaxgiUpdateKloodge) != SQL_SUCCESS)
       print_err(HNDLTaxgiUpdateKloodge);
   if (SQLFreeHandle(SQL_HANDLE_STMT, HNDLTaxgiUpdateType) != SQL_SUCCESS)
       print_err(HNDLTaxgiUpdateType);
   if (SQLFreeHandle(SQL_HANDLE_STMT, HNDLTaxgiUpdateKloodgeByKloodge) != SQL_SUCCESS)
       print_err(HNDLTaxgiUpdateKloodgeByKloodge);
   if (SQLFreeHandle(SQL_HANDLE_STMT, HNDLTaxgiSearchKloodgeNA) != SQL_SUCCESS)
	 print_err(HNDLTaxgiSearchKloodgeNA);
   if (SQLFreeHandle(SQL_HANDLE_STMT, HNDLTaxgiSearchKloodgeAA) != SQL_SUCCESS)
	 print_err(HNDLTaxgiSearchKloodgeAA);

   if (SQLFreeHandle(SQL_HANDLE_STMT, HNDLDomdbSearchDomid) != SQL_SUCCESS)
       print_err(HNDLDomdbSearchDomid);
   if (SQLFreeHandle(SQL_HANDLE_STMT, HNDLDomdbUpdateAll) != SQL_SUCCESS)
       print_err(HNDLDomdbUpdateAll);
   if (SQLFreeHandle(SQL_HANDLE_STMT, HNDLDomdbUpdatePdbid) != SQL_SUCCESS)
       print_err(HNDLDomdbUpdatePdbid);
   if (SQLFreeHandle(SQL_HANDLE_STMT, HNDLDomdbUpdateChain) != SQL_SUCCESS)
       print_err(HNDLDomdbUpdateChain);
   if (SQLFreeHandle(SQL_HANDLE_STMT, HNDLDomdbUpdateMmdbid) != SQL_SUCCESS)
       print_err(HNDLDomdbUpdateMmdbid);
   if (SQLFreeHandle(SQL_HANDLE_STMT, HNDLDomdbUpdateGi) != SQL_SUCCESS)
       print_err(HNDLDomdbUpdateGi);
   if (SQLFreeHandle(SQL_HANDLE_STMT, HNDLDomdbUpdateDomno) != SQL_SUCCESS)
       print_err(HNDLDomdbUpdateDomno);
   if (SQLFreeHandle(SQL_HANDLE_STMT, HNDLDomdbUpdateDomall) != SQL_SUCCESS)
       print_err(HNDLDomdbUpdateDomall);
   if (SQLFreeHandle(SQL_HANDLE_STMT, HNDLDomdbUpdateRep) != SQL_SUCCESS)
       print_err(HNDLDomdbUpdateRep);


   /* RAC Do I need to return FALSE if something go Wrong??? */
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

   return TRUE;
}

/*question the presence of this table in a ODBC database becasue
  theoretically we should not need to divide the database up into
  partitions because the ODBC database should be able to make
  a table that is very large.  For now this will remain but
  it is possible to depending on which backend one is using
  to have different tables as well as different database
  system.
  RAC Variable name is not used.
*/
Boolean OpenPARTI(CharPtr name, Boolean Create)
{
  /*check to see if table is supposed to be created.if yes then create it.*/
  if(Create)
    {
      SQLCHAR * parti_create = (SQLCHAR *) "create table seqhound.parti ("
	"gi INTEGER NOT NULL Primary KEY,"
	"division VARCHAR(10) NOT NULL)";

      if(!execute_sql(parti_create, &hdbc, &henv))
	{
	  return (FALSE);
	}
    }
   return (TRUE);
}


Boolean LIBCALL AppendRecordPARTI(StPartiPtr ppa)
{
   SQLRETURN retcode;
   if(ppa == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, "[AppendRecordPARTI]: NULL parameter.");
      return FALSE;
   }

   parti_gi.val = ppa->gi;
   sprintf((char *)parti_div.val, "%s", IsNullStr(ppa->partition));

   /* execute statment */
   retcode = SQLExecute(HNDLParti);
   if (retcode != SQL_SUCCESS)
   {
	  ErrPostEx(SEV_ERROR,0,0, "[AppendRecordPARTI]: unable to execute PARTI insert statement. (Gi=%ld)",parti_gi.val);
	  LogSQLInfo(HNDLParti);
      return FALSE;
   }

   return TRUE;
}

/*******************************************************/
/*Given a pointer to the asn structure that represents */
/*  a record in the parti table this function pulls out*/
/*  The gi from the asn structure that we are interested*/
/*  in finding out more information and pulls it out of*/
/*  the database and populates the rest of the         */
/*  asn parti structure.                               */
/*Returns : TRUE if successful and FALSE otherwise.    */
/*******************************************************/
Int2 LIBCALL SearchPARTI (StPartiPtr PNTR pppa)
{
   StPartiPtr ppa = NULL;         /* linked list for return value */
   SQLHANDLE hstmt;               /* select statement handle */
   SQLRETURN retcode;             /* return value for error detection */
   Char *pSQL_stmt;               /* the select statement */
   SQLINTEGER key = 0;                  /* search key if int */
   Char * pKey = NULL;            /* search key if string */
   SQLINTEGER bbvalue = 0;
   if (pppa == NULL)
   {
       ErrPostEx(SEV_ERROR, 0, 0, "[SearchPARTI]: StPartiPtr is NULL.");
       return 0;
   }
   ppa = *pppa;

   /* allocate select statement & initialize: using global variable
      database that gets initialized in ConnectToDb. its value is
      set in init file. eg database = seqhound */
   pSQL_stmt = (char *) malloc(sizeof(char) * 150);
   if(pSQL_stmt == NULL)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[SearchPARTI]: Could not allocate space.");
     return 0;
   }

   /* set the search key based on whether gi or partition is set.
      create the select statement */
   if (ppa->gi > 0)
   {
       key = ppa->gi;
       sprintf((char*) pSQL_stmt, "select gi, division from %s.parti where gi = ?;", database);
   }else if(ppa->partition != NULL){
       pKey = ppa->partition;
       bbvalue = strlen(ppa->partition);
       sprintf((char*) pSQL_stmt, "select gi, division from %s.parti where division = ?;", database);
   }else{
       /* neither is set, error */
       ErrPostEx(SEV_ERROR, 0, 0, "[SearchPARTI]: No search key set.");
       return 0;
   }

   /* create the handle to the select statement, once handle exists, free char* */
   if(CreateHandle(&hstmt, pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[SearchPARTI]: Could not create handle");
      return 0;
   }
   free(pSQL_stmt);

   /* bind search key to select handle */
   if(key > 0)
      retcode = SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_SLONG,SQL_INTEGER, 0, 0, &key, 0, NULL);
   else
     retcode = SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_DEFAULT, SQL_VARCHAR, 10, 0, pKey, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[SearchPARTI]: Could not bind parameter");
      SQLFreeStmt(hstmt, SQL_DROP);
      return 0;
   }

   /* execute statment */
   retcode = SQLExecute(hstmt);
   if (retcode != SQL_SUCCESS)
   {
	  ErrPostEx(SEV_ERROR,0,0, "SearchPARTI: Unable to execute select statement.");
	  LogSQLInfo(hstmt);
      SQLFreeStmt(hstmt, SQL_DROP);
      return 0;
   }

   /* bind the columns of the return value from the database */
   retcode = SQLBindCol(hstmt, 1, SQL_C_SLONG, &parti_gi.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[SearchPARTI]: Can not bind column 1.");
      SQLFreeStmt(hstmt, SQL_DROP);
      return 0;
   }
   retcode = SQLBindCol(hstmt, 2, SQL_C_CHAR, parti_div.val, 10, (int*)&bbvalue);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[SearchPARTI]: Can not bind column 2.");
      SQLFreeStmt(hstmt, SQL_DROP);
      return 0;
   }

   /* fetch the data in columns iteratively */
   SQLINTEGER fetchCount = 0;
   /* fetch result and save in the linked list */
   while((retcode=SQLFetch(hstmt)) == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
   {
      fetchCount++;
      if (fetchCount>1)
      {
	    ppa->next = StPartiNew();
	    ppa = ppa->next;
	  }

	  ppa->partition = StringSave((Char*)parti_div.val);
      ppa->gi = parti_gi.val;
   }


   /* Free statement handle. */
   retcode = SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
   if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[SearchPARTI]: Could not free handle.");
      return 0;
   }

   if (fetchCount == 0)
	  return -1;

   return 1;
}


/* returns TRUE if the GI belongs to the specified division -
will not work for updates (is only used for primates in precomputed)*/
Boolean LIBCALL IsThisDivision(Int4 gi, CharPtr division)
{
   if((gi <= 0) || (division == NULL))
   {
     ErrPostEx(SEV_ERROR,0,0, "IsThisDivision: Invalid parameter.");
     return FALSE;
   }

   StPartiPtr ppa=StPartiNew();
   ppa->gi = gi;

   Int2 rc = 0;
   rc = SearchPARTI(&ppa);

   if (rc == 0)
   {
       ErrPostEx(SEV_ERROR, 0, 0, "[IsThisDivision]: Error.");
       return FALSE;
   }
   if (rc == -1)
   {
       ErrPostEx(SEV_ERROR, 0, 0, "[IsThisDivision]: No data found.");
       return FALSE;
   }
   if (rc == 1)
   {
	   while (ppa != NULL) {
		   if (!StrCmp(ppa->partition, division)) {
			   StPartiFree(ppa);
			   return TRUE;
		   }
		   ppa = ppa->next;
	   }
   }

	StPartiFree(ppa);
   return FALSE;
}

/* Given: a partition
 * Return: all gis in this partition */
ValNodePtr LIBCALL SHoundAllPartition(CharPtr ppartition)
{
	ValNodePtr pvn = NULL;

   SQLHANDLE hstmt;               /* select statement handle */
   SQLRETURN retcode;             /* return value for error detection */
   Char *pSQL_stmt;               /* the select statement */
   Char * pKey = NULL;            /* search key if string */
   SQLINTEGER bbvalue = 0;

   struct{
     SQLINTEGER len;
     SQLINTEGER val;
   }gi;

   if (ppartition == NULL)
   {
       ErrPostEx(SEV_ERROR, 0, 0, "[SHoundAllPartition]: paritition is NULL.");
       return NULL;
   }

   /* allocate select statement & initialize: using global variable
      database that gets initialized in ConnectToDb. its value is
      set in init file. eg database = seqhound */
   pSQL_stmt = (char *) malloc(sizeof(char) * 150);
   if(pSQL_stmt == NULL)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[SHoundAllPartition]: Could not allocate space.");
     return NULL;
   }
   pKey = ppartition;
   bbvalue = strlen(ppartition);
   sprintf((char*) pSQL_stmt, "select gi from %s.parti where division = ?;", database);


   /* create the handle to the select statement, once handle exists, free char* */
   if(CreateHandle(&hstmt, pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[SHoundAllPartition]: Could not create handle");
      return NULL;
   }
   free(pSQL_stmt);

   /* bind search key to select handle */
   retcode = SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_DEFAULT, SQL_VARCHAR, 10, 0, pKey, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[SHoundAllPartition]: Could not bind parameter");
      SQLFreeStmt(hstmt, SQL_DROP);
      return NULL;
   }

   /* execute statment */
   retcode = SQLExecute(hstmt);
   if (retcode != SQL_SUCCESS)
   {
	  ErrPostEx(SEV_ERROR,0,0, "SHoundAllPartition: Unable to execute select statement.");
	  LogSQLInfo(hstmt);
      SQLFreeStmt(hstmt, SQL_DROP);
      return NULL;
   }

   /* bind the columns of the return value from the database */
   retcode = SQLBindCol(hstmt, 1, SQL_C_SLONG, &gi.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[SHoundAllPartition]: Can not bind column 1.");
      SQLFreeStmt(hstmt, SQL_DROP);
      return NULL;
   }

   /* fetch the data in columns iteratively */
   while((retcode=SQLFetch(hstmt)) == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
   {
       pvn = Slri_ValNodeAddInt(&pvn, 0, gi.val);
   }


   /* Free statement handle. */
   retcode = SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
   if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[SHoundAllPartition]: Could not free handle.");
      return NULL;
   }

   return pvn;

}

CharPtr LIBCALL GetDivisionFromPARTI( Int4 gi )
{
  Int2     retcode    = 0;
  Int2     fetchcount = 0;
  CharPtr  division   = NULL;

  if( gi <= 0 )
  {
	 ErrPostEx(SEV_ERROR, 0, 0, "[GetDivisionFromParti]: "
			   "Bad gi %d", gi );
	 return division;
  }

  parti_gi.val = gi;

  retcode = SQLExecute( HNDLPartiSearchGi );

  if (retcode != SQL_SUCCESS)
  {
	ErrPostEx(SEV_ERROR,0,0, "[GetDivisionFromPARTI]: "
			  "Could not execute select statement. gi = %d", gi);
	LogSQLInfo(HNDLPartiSearchGi);
	return division;
  }

  retcode = SQLBindCol(HNDLPartiSearchGi, 1, SQL_C_CHAR, 
						parti_div.val, 10, (int*)&parti_div.len);
  if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
  {
	ErrPostEx(SEV_ERROR, 0, 0, "[GetDivisionFromPARTI]: "
			  "Could not bind column 1.");
	return division;
  }

  while((retcode=SQLFetch(HNDLPartiSearchGi)) == SQL_SUCCESS || 
		retcode == SQL_SUCCESS_WITH_INFO)
  {
	fetchcount++;

	division = StringSave( (const char *) parti_div.val );
  }

  if( fetchcount <= 0 )
  {
	ErrPostEx(SEV_WARNING, 0, 0, "[GetDivisionFromPARTI]: "
			  "No PARTI record for gi %d.", gi);
  }

  retcode  = SQLFreeStmt( HNDLPartiSearchGi, SQL_CLOSE );

  if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[GetDivisionFromPARTI]: Error in closing cursor.");
      return FALSE;
   }
  return division;
}
/***************************************/
/*     Delete records in databases     */
/***************************************/
/* if Boolean variable "destroy" is TRUE the record will be blanked out
and GI substituted with -1 if it is FALSE record will be simply deleted */
/*  In ODBC there is no such thing as a logical delete unless a flag has
been defined in the table so the delete functions in this ODBC layer simply
delete the record.  */
Boolean LIBCALL DeleteGIinPARTI(SQLINTEGER gi, Boolean destroy)
{
   Int2 sql_return = SQL_SUCCESS;

   parti_gi.val = gi;

  sql_return = SQLExecute(HNDLPartiDelete);
  /* execute statment */
  if (sql_return != SQL_SUCCESS)
  {
     ErrPostEx(SEV_ERROR,0,0, "[DeleteGIinParti]:unable to execute delete statement.");
	 LogSQLInfo(HNDLPartiDelete);
	 return FALSE;
  }

  return TRUE;
}


Boolean AppendRecordREDUND(StRedundPtr prd)
{
   SQLRETURN retcode;

   if(prd == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, "AppendRecordREDUND: NULL parameter.");
      return FALSE;
   }

   /* assign values to the parameters according to what was passed into the method. */
   redund_gi.val = prd->gi;
   redund_rordinal.val = prd->ordinal;
   redund_rgroup.val = prd->group;

   /* execute statment */
   retcode = SQLExecute(HNDLRedund);
   if (retcode != SQL_SUCCESS)
   {
	  ErrPostEx(SEV_ERROR,0,0, "[AppendRecordREDUND]: unable to execute insert statement. (gi=%ld)",redund_gi.val);
	  LogSQLInfo(HNDLRedund);
      return FALSE;
   }

   return TRUE;
}

/* Search for records in REDUND database. The search key is set in pprd. eg:
    pprd->gi = 1       will get all records in REDUND with gi = 1
    pprd->ordinal = 2  will get all records in REDUND with ordinal = 2
    pprd->group = 3    will get all records in REDUND with group = 3
   The return value is pprd. Iterate through pprd as a linked list */
Int2 SearchREDUND(StRedundPtr PNTR pprd)
{
   StRedundPtr prd;   /* linked list for return value */
   SQLINTEGER keynum;       /* the search value */
   CharPtr pSQL_stmt;  /* the select statement */
   SQLHANDLE hstmt;   /* handle to select statement */
   SQLINTEGER retcode; /* odbc return values */

   if(pprd == NULL)
   {
     ErrPostEx(SEV_ERROR,0,0, "[SearchREDUND]: StRedundPtr is NULL.");
     return 0;
   }
   prd = *pprd;

   /* allocate the select statement and initialize it: use global variable
      'database' which is initialized in ConnectToDb:
      its value is set in initialization file. eg database = seqhound */
   pSQL_stmt = (char*) malloc(sizeof(char) * 150);
   if(pSQL_stmt == NULL)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[SearchREDUND]: Could not allocate space.");
      return FALSE;
   }
   if (prd->gi > 0)
   {
      keynum = prd->gi;
      sprintf((char*) pSQL_stmt, "select gi, rordinal, rgroup from %s.redund where gi = ?;", database);
   } else if (prd->ordinal > 0) {
      keynum = prd->ordinal;
      sprintf((char*) pSQL_stmt, "select gi, rordinal, rgroup from %s.redund where rordinal = ?;", database);
   } else if (prd->group > 0) {
      keynum = prd->group;
      sprintf((char*) pSQL_stmt, "select gi, rordinal, rgroup from %s.redund where rgroup = ?;", database);
   } else {
      ErrPostEx(SEV_ERROR,0,0, "SearchREDUND: No allowed search key.");
      free(pSQL_stmt);
      return 0;
   }

   /* allocate a statement handle to our select statement. */
   if(CreateHandle(&hstmt, pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
   {
      SQLFreeStmt(hstmt, SQL_DROP);
      ErrPostEx(SEV_ERROR,0,0, "[SearchREDUND]: Could not create statement handle.");
      free(pSQL_stmt);
	  return 0;
   }
   /* once handle is created I can free the char* */
   free(pSQL_stmt);

   /* Bind SQL Arguments. */
   retcode = SQLBindParameter (hstmt, 1 ,SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &keynum, 0, NULL);
   if((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO))
   {
      SQLFreeStmt(hstmt, SQL_DROP);
      ErrPostEx(SEV_ERROR, 0, 0, "[SearchREDUND]: SQLBindParameter fail");
      free(pSQL_stmt);
      return 0;
   }

   /* Execute the statement using Direct Execute but hire I'm using SQLExecute() */
   retcode = SQLExecute(hstmt);
   if (retcode != SQL_SUCCESS)
   {
	  ErrPostEx(SEV_ERROR,0,0, "[SearchREDUND]: Unable to execute select statement.");
	  LogSQLInfo(hstmt);
      SQLFreeStmt(hstmt, SQL_DROP);
      free(pSQL_stmt);
      return 0;
   }

   /* bind the statement handle to a variable to retrieve the data. */
   retcode = SQLBindCol(hstmt, 1, SQL_C_ULONG, &redund_gi.val, 0, NULL);
   if((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO))
   {
      ErrPostEx(SEV_ERROR,0,0, "[SearchREDUND]: Binding Column 1 failed.");
      return 0;
   }
   retcode = SQLBindCol(hstmt, 2, SQL_C_ULONG, &redund_rordinal.val, 0, NULL);
   if((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO))
   {
      ErrPostEx(SEV_ERROR,0,0, "[SearchREDUND]: Binding Column 2 failed.");
      return 0;
   }
   retcode = SQLBindCol(hstmt, 3, SQL_C_ULONG, &redund_rgroup.val, 0, NULL);
   if((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO))
   {
      ErrPostEx(SEV_ERROR,0,0, "[SearchREDUND]: Binding Column 3 failed.");
      return 0;
   }

   /* used SQLFetchScroll for rowset instead of result set
      rowset is buffer in application which is faster that
      result set which is maintained by SQL driver on server side */
   retcode = SQLFetchScroll(hstmt, SQL_FETCH_NEXT, 1);
   if (retcode == SQL_NO_DATA_FOUND) {
      ErrPostEx(SEV_INFO, 0, 0, "No data found");
      SQLFreeStmt(hstmt, SQL_DROP);
      return -1;
   }

   while (retcode != SQL_NO_DATA_FOUND)
   {
      /* assign the fetched values to the return value */
      prd->gi      = redund_gi.val;
      prd->ordinal = redund_rordinal.val;
      prd->group   = redund_rgroup.val;
      retcode = SQLFetchScroll(hstmt, SQL_FETCH_NEXT, 1);
      if(retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
      {
	prd->next = StRedundNew();
	prd = prd->next;
      }
   }

   if(SQLFreeHandle(SQL_HANDLE_STMT, hstmt) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[SearchREDUND]: Could not free handle");
      return 0;
   }
   SQLFreeStmt(hstmt, SQL_CLOSE);
   return 1;
}

Boolean DeleteGIinREDUND(SQLINTEGER gi, Boolean destroy)
{
   CharPtr redund_delete = (char *) malloc(sizeof(Char) * 150);
   if (redund_delete == NULL)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[DeleteGIinREDUND]: Could not allocate space");
     free(redund_delete);
     return FALSE;
   }
   sprintf((char *)redund_delete, "delete from %s.redund where gi=?", database);

   SQLHANDLE hstmt;
   Int2 sql_return = SQL_SUCCESS;

   sql_return = CreateHandle(&hstmt, redund_delete, hdbc, &henv);

   sql_return = SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &gi, 0, NULL);

   sql_return = SQLExecute(hstmt);
   /* execute statment */
   if (sql_return != SQL_SUCCESS)
   {
	  ErrPostEx(SEV_ERROR,0,0, "[DeleteGIinREDUND]:unable to execute delete statement.");
	  LogSQLInfo(hstmt);
      SQLFreeStmt(hstmt, SQL_DROP);
	  free(redund_delete);
      return FALSE;
   }
   free(redund_delete); 

   SQLFreeStmt(hstmt, SQL_DROP);
   return TRUE;
}

Boolean DeleteAllRecordsInREDUND()
{
   SQLHANDLE hstmt;
   Int2 sql_return = SQL_SUCCESS;

   CharPtr delete_stmt = (char *) malloc(sizeof(Char) * 150);
   if (delete_stmt == NULL)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[DeleteAllRecordsInREDUND]: Could not allocate space");
     free(delete_stmt);
     return FALSE;
   }
   sprintf((char *)delete_stmt, "delete from %s.redund", database);

   if (CreateHandle(&hstmt, delete_stmt, hdbc, &henv) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[DeleteAllRecordsInREDUND]: Could not create handle");
      free(delete_stmt);
      return FALSE;
   }
   free(delete_stmt);

   sql_return = SQLExecute(hstmt);
   /* execute statment */
   if (sql_return != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR,0,0, "[DeleteAllRecordsInREDUND]:unable to execute delete statement.");
      LogSQLInfo(hstmt);
      SQLFreeStmt(hstmt, SQL_DROP);
      return FALSE;
   }

   SQLFreeStmt(hstmt, SQL_DROP);
   return TRUE;
}

/* returns record number */
/* Purpose of this method is to get the last entered record */
/* so if you are building redund and stop in the middle you */
/* can start the building off from where you left off.      */
/* RETURNS : the record in redund that has the largest group*/
/*  returns zero if there is nothing in the table.          */
Int4 LIBCALL SHoundRecNoREDUND(void)
{
	SQLRETURN sqlrc;
	SQLINTEGER max=0;
	SQLHANDLE hstmt;

   CharPtr pSQL_stmt = (Char*) malloc(sizeof(Char) * 150);
   if(pSQL_stmt == NULL)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[SHoundRecNoREDUND]: Could not allocated space");
     return 0;
   }

   sprintf((char*)pSQL_stmt, "select count(*) as count from %s.redund;", database);
   if(CreateHandle(&hstmt, pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[SHoundRecNoREDUND]: Could not create handle");
      free(pSQL_stmt);
      return 0;
   }
   free(pSQL_stmt);

   /* execute statment */
   sqlrc = SQLExecute(hstmt);
   if (sqlrc != SQL_SUCCESS)
   {
	  ErrPostEx(SEV_ERROR,0,0, "[SHoundRecNoRedund]:unable to execute select statement.");
	  LogSQLInfo(hstmt);
      return 0;
   }

   sqlrc = SQLBindCol(hstmt,1,SQL_C_SLONG,&max,0,NULL);
   if (sqlrc != SQL_SUCCESS)
   {
	  ErrPostEx(SEV_ERROR,0,0, "[SHoundRecNoRedund]:unable to bind col 1.");
      return 0;
   }

   sqlrc = SQLFetch(hstmt);
   if(sqlrc == SQL_NO_DATA_FOUND){
      ErrPostEx(SEV_INFO, 0, 0, "[SHoundRecNoRedund]:No data found");
      return 0;
   }

   if(sqlrc == SQL_SUCCESS || sqlrc == SQL_SUCCESS_WITH_INFO)
   {
      sqlrc = SQLFreeStmt(hstmt, SQL_DROP);
      return max;
   }

    return -1;
}


StRedundPtr LIBCALL SHoundGetRecREDUND(Int4 recno)
{
   SQLHANDLE hstmt;
   SQLRETURN retcode;
   StRedundPtr prd = NULL;

   if (recno < 1 || recno > SHoundRecNoREDUND())
   {
      ErrPostEx(SEV_ERROR,0,0, "[SHoundGetRecREDUND]: Invalid parameters.");
      return NULL;
    }

   prd = StRedundNew();
   rowid.val = recno;

   CharPtr pSQL_stmt = (Char*) malloc(sizeof(Char) * 150);
   if(pSQL_stmt == NULL)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[SHoundGetRecREDUND]: Could not allocated space");
     return NULL;
   }

   sprintf((char*)pSQL_stmt, "select gi, rordinal, rgroup from %s.redund where rowid=?;", database);
   if(CreateHandle(&hstmt, pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[SHoundGetRecREDUND]: Could not create handle");
      free(pSQL_stmt);
      return NULL;
   }
   free(pSQL_stmt);

   retcode = SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &rowid.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[SHoundGetRecREDUND]: Could not bind parameter 1");
      return NULL;
   }

   /* execute statment */
   retcode = SQLExecute(hstmt);
   if (retcode != SQL_SUCCESS)
   {
	  ErrPostEx(SEV_ERROR,0,0, "[SHoundGetRecREDUND]:unable to execute select statement.");
	  LogSQLInfo(hstmt);
      return NULL;
   }

   /* bind the statement handle to a variable to retrieve the data. */
   retcode = SQLBindCol(hstmt, 1, SQL_C_ULONG, &redund_gi.val, 0, NULL);
   if((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO))
   {
      ErrPostEx(SEV_ERROR,0,0, "[SHoundGetRecREDUND]: Binding Column 1 failed.");
      return NULL;
   }
   retcode = SQLBindCol(hstmt, 2, SQL_C_ULONG, &redund_rordinal.val, 0, NULL);
   if((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO))
   {
      ErrPostEx(SEV_ERROR,0,0, "[SHoundGetRecREDUND]: Binding Column 2 failed.");
      return NULL;
   }
   retcode = SQLBindCol(hstmt, 3, SQL_C_ULONG, &redund_rgroup.val, 0, NULL);
   if((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO))
   {
      ErrPostEx(SEV_ERROR,0,0, "[SHoundGetRecREDUND]: Binding Column 3 failed.");
      return NULL;
   }

   /* used SQLFetchScroll for rowset instead of result set
      rowset is buffer in application which is faster that
      result set which is maintained by SQL driver on server side */
   retcode = SQLFetchScroll(hstmt, SQL_FETCH_NEXT, 1);
   if (retcode == SQL_NO_DATA_FOUND) {
      ErrPostEx(SEV_INFO, 0, 0, "[SHoundGetRecREDUND]: No data found");
      SQLFreeStmt(hstmt, SQL_DROP);
      return NULL;
   }

   while (retcode != SQL_NO_DATA_FOUND)
   {
      /* assign the fetched values to the return value */
      prd->gi      = redund_gi.val;
      prd->ordinal = redund_rordinal.val;
      prd->group   = redund_rgroup.val;
      retcode = SQLFetchScroll(hstmt, SQL_FETCH_NEXT, 1);
      if(retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
      {
	     prd->next = StRedundNew();
	     prd = prd->next;
      }
   }

   if(SQLFreeHandle(SQL_HANDLE_STMT, hstmt) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[SHoundGetRecREDUND]: Could not free handle");
      return NULL;
   }

   SQLFreeStmt(hstmt, SQL_DROP);
   return prd;
}


void LIBCALL ResetStREDUND(StRedundPtr PNTR pprd)
{
   StRedundPtr prd = NULL;

   prd = *pprd;
   if(prd == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, "ResetStREDUND: NULL parameter.");
      return;
   }

   prd->gi = 0;
   prd->ordinal = 0;
   prd->group = 0;
   return;
}

Boolean LIBCALL OpenSENGI(CharPtr name, Boolean Create)
{
   /*check to see if table is supposed to be created.if yes then create it.*/
   if(Create)
   {
     SQLCHAR * sengi_create = (SQLCHAR *) "create table seqhound.sengi ("
       "gi INTEGER NOT NULL PRIMARY KEY, "
       "seid INTEGER NOT NULL,"
       "division VARCHAR(10) NOT NULL)";

      if(!execute_sql(sengi_create, &hdbc, &henv))
      {
         return FALSE;
      }
   }
   return TRUE;
}


Boolean LIBCALL AppendRecordSENGI(StSengiPtr psg)
{
   SQLRETURN retcode;
   if(psg == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, "AppendRecordSENGI: NULL parameter.");
      return FALSE;
   }

   sengi_gi.val = psg->gi;
   sengi_seid.val = psg->seid;

   if(strlen(psg->division) > 15)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "AppendRecordSENGI: division length > 15. %ld", strlen(psg->division));
      return FALSE;
   }

   sprintf((char *)sengi_div.val, "%s", IsNullStr(psg->division));

   /* execute statment */
   retcode = SQLExecute(HNDLSengi);
   if (retcode != SQL_SUCCESS)
   {
	  ErrPostEx(SEV_ERROR,0,0, "[AppendRecordSENGI]: unable to execute SENGI insert statement. (gi=%ld)", sengi_gi.val);
	  LogSQLInfo(HNDLSengi);
      return FALSE;
   }
   return TRUE;
}

Int2  LIBCALL SearchSENGI (StSengiPtr PNTR ppsg)
{
   StSengiPtr pps = NULL;
   SQLRETURN retcode;
   SQLHANDLE hstmt;	/* the handle to the select statement */
   Char *pSQL_stmt;
   SQLINTEGER iKey;

   /* check that our input is valid */
   if (ppsg == NULL)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[SearchSENGI]: StSengiPtr is NULL.");
      return 0;
   }
   pps = *ppsg;

   pSQL_stmt = (char*) malloc (sizeof(char) * 150);
   if(pSQL_stmt == NULL)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[SearchSENGI]: Could not allocate space.");
      return 0;
   }
   if (pps->gi > 0)
   {
      iKey = pps->gi;
      sprintf((char*)pSQL_stmt, "select gi, seid, division from %s.sengi where gi = ?;", database);
   }else if(pps->seid > 0){
      iKey = pps->seid;
      sprintf((char*)pSQL_stmt, "select gi, seid, division from %s.sengi where seid = ?;", database);
   }else { /* no search key was found */
      ErrPostEx(SEV_ERROR,0,0, "[SearchSENGI]: No search key was set.");
      return 0;
   }

   if(CreateHandle(&hstmt, pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[SearchSENGI]: Could not create handle to select statement");
      return 0;
   }
   free(pSQL_stmt);

   /* bind the search key to the handle */
   retcode = SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &iKey, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[SearchSENGI]: Could not bind parameter");
      SQLFreeStmt(hstmt, SQL_DROP);
      return 0;
   }

   /* execute select statment created above */
   retcode = SQLExecute(hstmt);
   if (retcode != SQL_SUCCESS)
   {
	  ErrPostEx(SEV_ERROR,0,0, "[SearchSENGI]: Unable to execute select statement.");
	  LogSQLInfo(hstmt);
      SQLFreeStmt(hstmt, SQL_DROP);
      return 0;
   }

   /* the select statement execution was successful, if our search key is a gi
    * then we bind the result set from the select statement to seqentry */
   retcode = SQLBindCol(hstmt, 1, SQL_C_SLONG, &sengi_gi.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "SearchSENGI: Could not bind result set");
      SQLFreeStmt(hstmt, SQL_DROP);
      return 0;
   }
   retcode = SQLBindCol(hstmt, 2, SQL_C_SLONG, &sengi_seid.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "SearchSENGI: Could not bind result set");
      SQLFreeStmt(hstmt, SQL_DROP);
      return 0;
   }
   retcode = SQLBindCol(hstmt, 3, SQL_C_CHAR, sengi_div.val, 15, (int*)&sengi_div.len);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "SearchSENGI: Could not bind result set");
      SQLFreeStmt(hstmt, SQL_DROP);
      return 0;
   }

   /* fetch the first row of the result set */
   SQLINTEGER fetchCount = 0;
   /* fetch result and save in the linked list */
   while((retcode=SQLFetch(hstmt)) == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
   {
      fetchCount++;
      if (fetchCount>1)
      {
         pps->next = StSengiNew();
         pps = pps->next;
	  }

	  pps->gi = sengi_gi.val;
      pps->seid = sengi_seid.val;
      pps->division = StringSave((char*)sengi_div.val);
   }

   /* refresh the handle by freeing the select statement */
   retcode = SQLFreeStmt(hstmt, SQL_DROP);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "SearchSENGI: Could not drop statement");
     return 0;
   }

   if (fetchCount == 0)
      return -1;

   return 1;
}


Boolean LIBCALL DeleteGIinSENGI(SQLINTEGER gi, Boolean destroy)
{
   Int2 sql_return = SQL_SUCCESS;

   sengi_gi.val = gi;

   sql_return = SQLExecute(HNDLSengiDelete);
   /* execute statment */
   if (sql_return != SQL_SUCCESS)
   {
	  ErrPostEx(SEV_ERROR,0,0, "[DeleteGIinSENGI]:unable to execute delete statement.");
	  LogSQLInfo(HNDLSengiDelete);
      return FALSE;
   }

   return TRUE;
}

void LIBCALL ResetStSENGI(StSengiPtr PNTR ppsg)
{
   StSengiPtr psg = NULL;

   psg = *ppsg;
   if(psg == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "ResetStSENGI: NULL parameter.");
      return;
    }

   psg->seid = 0;
   psg->gi = 0;
   psg->division = NULL;

   return;
}


void LIBCALL ResetStNUCPROT(StNucprotPtr PNTR ppnc)
{
   StNucprotPtr pnc = NULL;

   pnc = *ppnc;
   if(pnc == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "ResetStNUCPROT: NULL parameter.");
      return;
    }

   pnc->gin = 0;
   pnc->gia = 0;

   return;
}

void LIBCALL ResetStTAXGI(StTaxgiPtr PNTR pptg)
{
   StTaxgiPtr ptg = NULL;

   ptg = *pptg;
   if(ptg == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "ResetStTAXGI: NULL parameter.");
      return;
    }

   ptg->taxid = 0;
   ptg->gi = 0;
   ptg->type = NULL;

   return;
}

void LIBCALL ResetStPUBSEQ(StPubseqPtr PNTR ppps)
{
   StPubseqPtr pps = NULL;

   pps = *ppps;
   if(pps == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "ResetStPUBSEQ: NULL parameter.");
      return;
    }

   pps->gi = 0;
   pps->muid = 0;

   return;
}

void LIBCALL ResetStACCDB(StAccdbPtr PNTR ppac)
{
   StAccdbPtr pac = NULL;

   pac = *ppac;
   if(pac == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "ResetStACCDB: NULL parameter.");
      return;
    }

   pac->title = NULL;

   pac->gi = 0;
   pac->db = NULL;
   pac->name = NULL;
   pac->namelow = NULL;
   pac->access = NULL;
   pac->release = NULL;
   pac->version = 0;
   pac->chain = NULL;

   return;
}

void LIBCALL ResetStMMDB(StMmdbPtr PNTR ppmd)
{
   StMmdbPtr pmd = NULL;

   pmd = *ppmd;
   if(pmd == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "ResetStMMDB: NULL parameter.");
      return;
    }

   pmd->mmdbid = 0;
   pmd->pdbid = NULL;
   pmd->asn1 = NULL;
   pmd->bwhat = 0;
   pmd->models = 0;
   pmd->size = 0;
   pmd->bzsize = 0;

   return;
}

void LIBCALL ResetStMMGI(StMmgiPtr PNTR ppmg)
{
   StMmgiPtr pmg = NULL;

   pmg = *ppmg;
   if(pmg == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "ResetStMMGI: NULL parameter.");
      return;
    }

   pmg->mmdbid = 0;
   pmg->gi = 0;

   return;
}

void LIBCALL ResetStDOMDB(StDomdbPtr PNTR ppdd)
{
   StDomdbPtr pdd = NULL;

   pdd = *ppdd;
   if(pdd == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "ResetStDOMDB: NULL parameter.");
      return;
    }

   pdd->asn1 = NULL;
   pdd->mmdbid = 0;
   pdd->domid = 0;
   pdd->gi = 0;
   pdd->domall = 0;
   pdd->pdbid = NULL;
   pdd->chain = NULL;
   pdd->rep = 0;

   return;
}

void LIBCALL ResetStCDDB(StCddbPtr PNTR ppcd)
{
   StCddbPtr pcd = NULL;

   pcd = *ppcd;
   if(pcd == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "ResetStCDDB: NULL parameter.");
      return;
    }

   pcd->gi = 0;
   pcd->asn1 = NULL;

   return;
}
/* returns record number */
Int4 LIBCALL SHoundRecNoSENGI(void)
{
  SQLHANDLE hstmt_count;
  SQLINTEGER recordcount=-1;
  SQLRETURN retcode;

   CharPtr stmt = (char *) malloc(sizeof(Char) * 150);
   if (stmt == NULL)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[SHoundRecNoSENGI]: Could not allocate space");
     free(stmt);
     return (Int4)recordcount;
   }
   sprintf((char *)stmt, "SELECT count(*) as count from %s.sengi", database);

  retcode = CreateHandle(&hstmt_count, (CharPtr)stmt, hdbc, &henv);
  if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO){
    ErrPostEx(SEV_ERROR,0,0, "[SHoundRecNoSENGI]: unable to create handle");
    return (Int4)recordcount;
  }

  if(SQLBindCol(hstmt_count,1,SQL_C_SLONG,&recordcount,0,0)!=SQL_SUCCESS){
  	ErrPostEx(SEV_ERROR,0,0, "[SHoundRecNoSENGI]: could not bind record count.");
        print_err(hstmt_count);
    return (Int4)recordcount;
   }

  retcode = SQLExecute(hstmt_count);
   if (retcode != SQL_SUCCESS)
   {
	  ErrPostEx(SEV_ERROR,0,0, "[SHoundRecNoSENGI]: could not execute statment.");
	  LogSQLInfo(hstmt_count);
      return (Int4)recordcount;
   }

  if(SQLFetch(hstmt_count)!=SQL_SUCCESS){
	ErrPostEx(SEV_ERROR,0,0, "SHoundRecNoSENGI: could not fetch result.");
        print_err(hstmt_count);
    return (Int4)recordcount;
   }

  SQLCloseCursor(hstmt_count);
  SQLFreeHandle(SQL_HANDLE_STMT,hstmt_count);

  return (Int4)recordcount;

}


void LIBCALL ResetStASNDB(StAsndbPtr PNTR ppas)
{
   StAsndbPtr pas = NULL;

   pas = *ppas;
   if(pas == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "ResetStASNDB: NULL parameter.");
      return;
    }

   pas->asn1 = NULL;
   pas->gi = 0;
   pas->type = NULL;
   pas->division = NULL;
   pas->release = NULL;

   return;
}


void LIBCALL ResetStPARTI(StPartiPtr PNTR pppa)
{
   StPartiPtr ppa = NULL;

   ppa = *pppa;
   if(ppa == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "ResetStPARTI: NULL parameter.");
      return;
    }

   ppa->gi = 0;
   ppa->partition = NULL;

   return;
}

void LIBCALL ResetStSENDB(StSendbPtr PNTR ppsd)
{
   StSendbPtr psd = NULL;

   psd = *ppsd;
   if(psd == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "ResetStSENDB: NULL parameter.");
      return;
    }

   psd->asn1 = NULL;
   psd->seid = 0;

   return;
}



Boolean LIBCALL OpenACCDB(CharPtr name, Boolean Create)
{
  /*check to see if table is supposed to be created.if yes then create it.*/
  if(Create)
  {
     SQLCHAR * accdb_create = (SQLCHAR *) "create table seqhound.accdb ("
       "gi INTEGER NOT NULL,"
       "db VARCHAR(15) NOT NULL,"
       "name VARCHAR(30),"
       "accession VARCHAR(20) NOT NULL,"
       "chain VARCHAR(20),"
       "release VARCHAR(20),"
       "version INTEGER,"
       "title TEXT,"
       "Primary KEY(gi,db,accession))";
     if(!execute_sql(accdb_create, &hdbc, &henv))
     {
       return FALSE;
     }
  }
  return TRUE;
}


/* RAC: This function is missing to check the size of the variables recivied in *pac.
   if for some reason the size is biger than the storage, will generate a segmentation
   fail.
 */
Boolean LIBCALL AppendRecordACCDB(StAccdbPtr pac)
{
   SQLRETURN retcode;
   if(pac == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, "AppendRecordACCDB: NULL parameter.");
      return FALSE;
   }

   accdb_gi.val = pac->gi;
   sprintf((char *)accdb_db.val,"%s", IsNullStr(pac->db));
   sprintf((char *)accdb_name.val,"%s", IsNullStr(pac->name));
   sprintf((char *)accdb_namelow.val,"%s", IsNullStr(pac->namelow));
   sprintf((char *)accdb_access.val, "%s", IsNullStr(pac->access));
   sprintf((char *)accdb_chain.val,"%s",IsNullStr(pac->chain));
   sprintf((char *)accdb_release.val,"%s", IsNullStr(pac->release));
   accdb_version.val = pac->version;

   /* use this way in case the length of title exceeds 1024 */
   if (pac->title != NULL) {
      retcode = SQLBindParameter(HNDLAccdb, 8, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, strlen(pac->title), 0, IsNullStr(pac->title), 0, NULL);
   } else {
      retcode = SQLBindParameter(HNDLAccdb, 8, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, strlen(""), 0, "", 0, NULL);
   }
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[AppendRecordACCDB]: Could not bind parameter 8");
      return FALSE;
	}

   /*
   if (pac->title != NULL)
       sprintf((char *)accdb_title.val,"%s",pac->title);
   else
       StrCpy(accdb_title.val, "");
   */

  /* execute statment */
   retcode = SQLExecute(HNDLAccdb);
   if (retcode != SQL_SUCCESS)
   {
	  ErrPostEx(SEV_ERROR,0,0, "[AppendRecordACCDB]: cannot insert (gi=%ld, db=%s, name=%s)",
                    accdb_gi.val, accdb_db.val, accdb_name.val);
	  LogSQLInfo(HNDLAccdb);
      return FALSE;
   }

   return TRUE;
}

Boolean LIBCALL DeleteGIinACCDB(SQLINTEGER gi, Boolean destroy)
{
   Int2 sql_return = SQL_SUCCESS;

   accdb_gi.val = gi;

   sql_return = SQLExecute(HNDLAccdbDelete);
   /* execute statment */
   if (sql_return != SQL_SUCCESS)
   {
	  ErrPostEx(SEV_ERROR,0,0, "[DeleteGIinACCDB]:unable to execute delete statement.");
	  LogSQLInfo(HNDLAccdbDelete);
      return FALSE;
   }

   return TRUE;
}

/***********************************************************
* Given an SQL statement, create SQL handle then execute it.
* for statement which doesn't need bind parameters
************************************************************/
static Boolean executeSQL(Char * sqlStmt) {
   SQLHANDLE hstmt;
   SQLRETURN sql_return;

   if (CreateHandle(&hstmt, sqlStmt,hdbc, &henv) != SQL_SUCCESS) {
      ErrPostEx(SEV_ERROR,0,0, "[executeSQL]: unable to create handle");
      return FALSE;
   }

   sql_return = SQLExecute(hstmt);
   if (sql_return != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR,0,0, "[executeSQL]: unable to execute update statement");
	  LogSQLInfo(hstmt);
      return FALSE;
   }

    if (SQLFreeHandle(SQL_HANDLE_STMT, hstmt) != SQL_SUCCESS) {

      ErrPostEx(SEV_ERROR,0,0, "[executeSQL]: unable to free statement");
      return FALSE;
   }

	return TRUE;
}


/*************************************/
/* Edit record functions - needed for*/
/* some parsers (genomes/comgen)     */
/* search key is always in the name  */
/* of the function                   */
/*************************************/
/* only updating name (FALSE) is in use */
Boolean LIBCALL EditRecordACCDBByGI(StAccdbPtr pac, Boolean overwrite)
{
    StAccdbPtr pactemp = NULL;
	SQLRETURN retcode;
	SQLINTEGER fetchCount=0;

	if (pac->gi <= 0)
     {
       ErrPostEx(SEV_ERROR,0,0, "[EditRecordACCDBByGI]: Invalid parameters.");
       return FALSE;
     }

	pactemp = StAccdbNew();
	accdb_gi.val = pac->gi;

    /* search the record in database */
	/* don't call SearchACCDB because it is slow if this Edit function is called many times in parser */
	retcode = SQLExecute(HNDLAccdbSearchGi);
   if (retcode != SQL_SUCCESS)
   {
	  ErrPostEx(SEV_ERROR,0,0, "[EditRecordACCDBByGI]: unable to execute search statement. (gi=%ld)",accdb_gi.val);
	  LogSQLInfo(HNDLAccdbSearchGi);
      return FALSE;
   }

   retcode = SQLBindCol(HNDLAccdbSearchGi, 1, SQL_C_SLONG, &accdb_gi.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[EditRecordACCDBByGI]: Could not bind column 1.");
     return FALSE;
   }
   retcode = SQLBindCol(HNDLAccdbSearchGi, 2, SQL_C_CHAR, accdb_db.val, 15, (int*)&accdb_db.len);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[EditRecordACCDBByGI]: Could not bind column 2.");
     return FALSE;
   }
   retcode = SQLBindCol(HNDLAccdbSearchGi, 3, SQL_C_CHAR, accdb_name.val, 30, (int*)&accdb_name.len);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[EditRecordACCDBByGI]: Could not bind column 3.");
     return FALSE;
   }
   retcode = SQLBindCol(HNDLAccdbSearchGi, 4, SQL_C_CHAR, accdb_access.val, 20, (int*)&accdb_access.len);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[EditRecordACCDBByGI]: Could not bind column 4.");
     return FALSE;
   }
   retcode = SQLBindCol(HNDLAccdbSearchGi, 5, SQL_C_CHAR, accdb_chain.val, 10, (int*)&accdb_chain.len);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[EditRecordACCDBByGI]: Could not bind column 5.");
     return FALSE;
   }
   retcode = SQLBindCol(HNDLAccdbSearchGi, 6, SQL_C_CHAR, accdb_release.val, 20, (int*)&accdb_release.len);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[EditRecordACCDBByGI]: Could not bind column 6.");
     return FALSE;
   }
   retcode = SQLBindCol(HNDLAccdbSearchGi, 7, SQL_C_SLONG, &accdb_version.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[EditRecordACCDBByGI]: Could not bind column 7.");
     return FALSE;
   }
   retcode = SQLBindCol(HNDLAccdbSearchGi, 8, SQL_C_CHAR, accdb_title.val, TEXT_LENGTH, (int*)&accdb_title.len);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[EditRecordACCDBByGI]: Could not bind column 8.");
     return FALSE;
   }
   retcode = SQLBindCol(HNDLAccdbSearchGi, 9, SQL_C_CHAR, accdb_namelow.val, 30, (int*)&accdb_namelow.len);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[EditRecordACCDBByGI]: Could not bind column 9.");
     return FALSE;
   }

   /* update every record that has gi matched */
   while((retcode=SQLFetch(HNDLAccdbSearchGi)) == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
   {
      fetchCount++;
      if (fetchCount>1)
      {
         pactemp->next = StAccdbNew();
         pactemp = pactemp->next;
      }

      pactemp->gi = accdb_gi.val;
      pactemp->db = StringSave((char*)accdb_db.val);
      pactemp->name = StringSave((char*)accdb_name.val);
      pactemp->namelow = StringSave((char*)accdb_namelow.val);
      pactemp->access = StringSave((char*)accdb_access.val);
      pactemp->chain = StringSave((char*)accdb_chain.val);
      pactemp->release = StringSave((char*)accdb_release.val);
      pactemp->version = accdb_version.val;
      if(accdb_title.len >0 ){
         pactemp->title = StringSave((char*)accdb_title.val);
      } else{
         pactemp->title = NULL;
      }

		if(overwrite == TRUE)  /* replace all values except GI but only if the replacement values are valid*/
		 {
			if (pac->db != NULL) sprintf((char *)accdb_db.val,"%s", IsNullStr(pac->db));
			else sprintf((char *)accdb_db.val,"%s", IsNullStr(pactemp->db));

			if (pac->name != NULL) sprintf((char *)accdb_name.val,"%s", IsNullStr(pac->name));
			else sprintf((char *)accdb_name.val,"%s", IsNullStr(pactemp->name));

			if (pac->namelow != NULL) sprintf((char *)accdb_namelow.val,"%s", IsNullStr(pac->namelow));
			else sprintf((char *)accdb_namelow.val,"%s", IsNullStr(pactemp->namelow));

			if (pac->access != NULL) sprintf((char *)accdb_access.val, "%s", IsNullStr(pac->access));
			else sprintf((char *)accdb_access.val, "%s", IsNullStr(pactemp->access));

			if (pac->chain != NULL) sprintf((char *)accdb_chain.val,"%s",IsNullStr(pac->chain));
			else sprintf((char *)accdb_chain.val,"%s",IsNullStr(pactemp->chain));

			if (pac->release != NULL) sprintf((char *)accdb_release.val,"%s", IsNullStr(pac->release));
			else sprintf((char *)accdb_release.val,"%s", IsNullStr(pactemp->release));

			if (pac->version !=0) accdb_version.val = pac->version;
			else accdb_version.val = pactemp->version;

			/*no update for title */

			  /* execute update statment */
			retcode = SQLExecute (HNDLAccdbUpdateAll);
			if (retcode != SQL_SUCCESS){
				ErrPostEx(SEV_ERROR,0,0, "[EditRecordACCDBByGi]: unable to execute ACCDB update all statement. gi=%ld", accdb_gi.val);
				LogSQLInfo(HNDLAccdbUpdateAll);
				return FALSE;
			}

		 }
		 else  /* replace only if no value in the database*/
		 {
			 if((strstr(pactemp->db, "n/a") != NULL) && (pac->db != NULL)) {
				sprintf((char *)accdb_db.val,"%s", IsNullStr(pac->db));
				retcode = SQLExecute (HNDLAccdbUpdateDb);
				if (retcode != SQL_SUCCESS) {
					ErrPostEx(SEV_ERROR,0,0, "[EditRecordACCDBByGI]: Error in updating db  gi=%ld", accdb_gi.val);
					LogSQLInfo(HNDLAccdbUpdateDb);
					return FALSE;
				}
			 }
			 if((strstr(pactemp->name, "n/a") != NULL) && (pac->name != NULL)) {
				sprintf((char *)accdb_name.val,"%s", IsNullStr(pac->name));
				retcode = SQLExecute (HNDLAccdbUpdateName);
				if (retcode != SQL_SUCCESS) {
					ErrPostEx(SEV_ERROR,0,0, "[EditRecordACCDBByGI]: Error in updating name  gi=%ld", accdb_gi.val);
					LogSQLInfo(HNDLAccdbUpdateName);
					return FALSE;
				}
			 }
			 if((strstr(pactemp->namelow, "n/a") != NULL) && (pac->namelow != NULL)) {
				sprintf((char *)accdb_namelow.val,"%s", IsNullStr(pac->namelow));
				retcode = SQLExecute (HNDLAccdbUpdateNamelow);
				if (retcode != SQL_SUCCESS) {
					ErrPostEx(SEV_ERROR,0,0, "[EditRecordACCDBByGI]: Error in updating namelow  gi=%ld", accdb_gi.val);
					LogSQLInfo(HNDLAccdbUpdateNamelow);
					return FALSE;
				}
			 }
			 if((strstr(pactemp->access, "n/a") != NULL) && (pac->access != NULL)){
				sprintf((char *)accdb_access.val,"%s", IsNullStr(pac->access));
				retcode = SQLExecute (HNDLAccdbUpdateAccess);
				if (retcode != SQL_SUCCESS) {
					ErrPostEx(SEV_ERROR,0,0, "[EditRecordACCDBByGI]: Error in updating access  gi=%ld", accdb_gi.val);
					LogSQLInfo(HNDLAccdbUpdateAccess);
					return FALSE;
				}
			 }
			 if((strstr(pactemp->release, "n/a") != NULL) && (pac->release != NULL)) {
				sprintf((char *)accdb_release.val,"%s", IsNullStr(pac->release));
				retcode = SQLExecute (HNDLAccdbUpdateRelease);
				if (retcode != SQL_SUCCESS) {
					ErrPostEx(SEV_ERROR,0,0, "[EditRecordACCDBByGI]: Error in updating release  gi=%ld", accdb_gi.val);
					LogSQLInfo(HNDLAccdbUpdateRelease);
					return FALSE;
				}
			 }
			 if((pactemp->version == 0) && (pac->version != 0)) {
				accdb_version.val = pac->version;
				retcode = SQLExecute (HNDLAccdbUpdateVersion);
				if (retcode != SQL_SUCCESS) {
					ErrPostEx(SEV_ERROR,0,0, "[EditRecordACCDBByGI]: Error in updating version  gi=%ld", accdb_gi.val);
					LogSQLInfo(HNDLAccdbUpdateVersion);
					return FALSE;
				}
			 }
			 if((strstr(pactemp->chain, "n/a") != NULL) && (pac->chain != NULL)) {
				sprintf((char *)accdb_chain.val,"%s", IsNullStr(pac->chain));
				retcode = SQLExecute (HNDLAccdbUpdateChain);
				if (retcode != SQL_SUCCESS) {
					ErrPostEx(SEV_ERROR,0,0, "[EditRecordACCDBByGI]: Error in updating chain  gi=%ld", accdb_gi.val);
					LogSQLInfo(HNDLAccdbUpdateChain);
					return FALSE;
				}
			 }
			 /* no update for title */
		 }

   } /* while fetch */


   /* close the cursor and discard all pending results. */
   /* must do in order to re-use the handle created in InitACCDB */
   retcode = SQLFreeStmt(HNDLAccdbSearchGi, SQL_CLOSE);
   if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[EditRecordACCDBByGI]: Error in closing cursor.");
      return FALSE;
   }

    StAccdbFree(pactemp);

    return TRUE;
}

/* find the number of records in table HISTDB */
Int4 LIBCALL GetRecNoACCDB(void) {

    Int2 sqlrc=SQL_SUCCESS;
    struct
    {
       SQLINTEGER len;
       SQLINTEGER val;
    }count;

   /* 0 returned for count(*) if the table is empty */
   /* initialize them to avoid a weird number */
	count.val=0; count.len=0;

   /* execute statment */
   sqlrc = SQLExecute(HNDLAccdbSelectCount);
   if (sqlrc != SQL_SUCCESS)
   {
	  ErrPostEx(SEV_ERROR,0,0, "[GetRecNoACCDB]:unable to execute select statement.");
	  LogSQLInfo(HNDLAccdbSelectCount);
      return 0;
   }

   sqlrc = SQLBindCol(HNDLAccdbSelectCount,1,SQL_C_SLONG,&count.val,0,(int*)&count.len);
   if (sqlrc != SQL_SUCCESS)
   {
	  ErrPostEx(SEV_ERROR,0,0, "[GetRecNoACCDB]:unable to bind col 1.");
      return 0;
   }

   sqlrc = SQLFetch(HNDLAccdbSelectCount);
   if(sqlrc == SQL_NO_DATA_FOUND){
      ErrPostEx(SEV_INFO, 0, 0, "[GetRecNoACCDB]: No data found");
      return 0;
   }

   if(sqlrc == SQL_SUCCESS || sqlrc == SQL_SUCCESS_WITH_INFO)
   {
      /* close the cursor and discard all pending results. */
      /* must do in order to re-use the handle created in InitACCDB */
      sqlrc = SQLFreeStmt(HNDLAccdbSelectCount, SQL_CLOSE);
      if (sqlrc != SQL_SUCCESS && sqlrc != SQL_SUCCESS_WITH_INFO)
	  {
         ErrPostEx(SEV_ERROR, 0, 0, "[GetRecNoACCDB]: Error in closing cursor.");
         return 0;
	  }
      return count.val;
   }

    return -1;
}


Boolean LIBCALL OpenTAXGI(CharPtr name, Boolean Create)
{
   /*check to see if table is supposed to be created.if yes then create it.*/
   if(Create)
   {
      SQLCHAR * taxgi_create = (SQLCHAR *) "create table seqhound.taxgi ("
	"gi INTEGER NOT NULL Primary KEY,"
	"taxid INTEGER NOT NULL,"
	"kloodge INTEGER,"
	"type VARCHAR(50))"; 
      if(!execute_sql(taxgi_create, &hdbc, &henv))
      {
         return FALSE;
      }
   }
   return TRUE;
}

Boolean LIBCALL AppendRecordTAXGI(StTaxgiPtr ptg)
{
  SQLRETURN retcode;

  if(ptg == NULL)
  {
     ErrPostEx(SEV_ERROR,0,0, "AppendRecordTAXGI: NULL parameter.");
     return FALSE;
  }

  taxgi_gi.val = ptg->gi;
  sprintf((char *)taxgi_type.val,"%s",IsNullStr(ptg->type));
  taxgi_kloodge.val = ptg->kloodge;
  taxgi_taxid.val = ptg->taxid;

  /* execute statment */
  retcode = SQLExecute(HNDLTaxgi);
   if (retcode != SQL_SUCCESS)
   {
	  ErrPostEx(SEV_ERROR,0,0, "[AppendRecordTAXGI]: unable to execute TAXGI insert statement. (gi=%ld)", taxgi_gi.val);
	  LogSQLInfo(HNDLTaxgi);
      return FALSE;
   }

   return TRUE;
}

/*************************************/
/* Edit record functions - needed for*/
/* some parsers (genomes/comgen)     */
/* search key is always in the name  */
/* of the function                   */
/*************************************/
/* only updating kloodge (FALSE) is in use */
Boolean LIBCALL EditRecordTAXGIByGI(StTaxgiPtr ptg, Boolean overwrite) {
    StTaxgiPtr ptgtemp = NULL;
	SQLINTEGER fetchCount=0;
	SQLRETURN retcode;

   if(ptg->gi <= 0)
     {
       ErrPostEx(SEV_ERROR,0,0, "[EditRecordTAXGIByGI]: Invalid parameters.");
       return FALSE;
     }

    ptgtemp = StTaxgiNew();
	taxgi_gi.val = ptg->gi;

    /* search the record in database */
	/* don't call SearchTAXGI because it is slow if this Edit function is called many times in parser */
	retcode = SQLExecute(HNDLTaxgiSearchGi);
   if (retcode != SQL_SUCCESS)
   {
	  ErrPostEx(SEV_ERROR,0,0, "[EditRecordTAXGIByGI]: unable to execute search statement. (gi=%ld)",taxgi_gi.val);
	  LogSQLInfo(HNDLTaxgiSearchGi);
      return FALSE;
   }

   retcode = SQLBindCol(HNDLTaxgiSearchGi, 1, SQL_C_SLONG, &taxgi_gi.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[EditRecordTAXGIByGI]: Could not bind column 1.");
     return FALSE;
   }
   retcode = SQLBindCol(HNDLTaxgiSearchGi, 2, SQL_C_SLONG, &taxgi_taxid.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[EditRecordTAXGIByGI]: Could not bind column 2.");
     return FALSE;
   }
   retcode = SQLBindCol(HNDLTaxgiSearchGi, 3, SQL_C_SLONG, &taxgi_kloodge.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[EditRecordTAXGIByGI]: Could not bind column 3.");
     return FALSE;
   }
   retcode = SQLBindCol(HNDLTaxgiSearchGi, 4, SQL_C_CHAR, taxgi_type.val, 50, (int*)&taxgi_type.len);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[EditRecordTAXGIByGI]: Could not bind column 4.");
     return FALSE;
   }

   /* update every record that has gi matched */
   while((retcode=SQLFetch(HNDLTaxgiSearchGi)) == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
   {
      fetchCount++;
      if (fetchCount>1)
      {
         ptgtemp->next = StTaxgiNew();
         ptgtemp = ptgtemp->next;
      }

      ptgtemp->gi = taxgi_gi.val;
      ptgtemp->taxid = taxgi_taxid.val;
      ptgtemp->kloodge = taxgi_kloodge.val;
	  if (taxgi_type.len>0)
		  ptgtemp->type = StringSave((char*)taxgi_type.val);
	  else
		  ptgtemp->type = NULL;

		if(overwrite == TRUE)  /* replace all values except GI but only if the replacement values are valid*/
		 {
			if (ptg->taxid != 0) taxgi_taxid.val = ptg->taxid;
			else taxgi_taxid.val = ptgtemp->taxid;

			if (ptg->kloodge != 0) taxgi_kloodge.val = ptg->kloodge;
			else taxgi_kloodge.val = ptgtemp->kloodge;

			if (ptg->type != NULL) sprintf((char *)taxgi_type.val,"%s", IsNullStr(ptg->type));
			else sprintf((char *)taxgi_type.val,"%s", IsNullStr(ptgtemp->type));

			  /* execute update statment */
			retcode = SQLExecute (HNDLTaxgiUpdateAll);
			if (retcode != SQL_SUCCESS){
				ErrPostEx(SEV_ERROR,0,0, "[EditRecordTAXGIByGi]: unable to execute TAXGI update all statement. gi=%ld", taxgi_gi.val);
				LogSQLInfo(HNDLTaxgiUpdateAll);
				return FALSE;
			}

		 }
		 else  /* replace only if no value in the database*/
		 {
			 if((ptgtemp->taxid==0) && (ptg->taxid != 0)) {
				taxgi_taxid.val = ptg->taxid;
				retcode = SQLExecute (HNDLTaxgiUpdateTaxid);
				if (retcode != SQL_SUCCESS) {
					ErrPostEx(SEV_ERROR,0,0, "[EditRecordTAXGIByGI]: Error in updating taxid. gi=%ld", taxgi_gi.val);
					LogSQLInfo(HNDLTaxgiUpdateTaxid);
					return FALSE;
				}
			 }
			 if((ptgtemp->kloodge==0) && (ptg->kloodge != 0)) {
				taxgi_kloodge.val = ptg->kloodge;
				retcode = SQLExecute (HNDLTaxgiUpdateKloodge);
				if (retcode != SQL_SUCCESS) {
					ErrPostEx(SEV_ERROR,0,0, "[EditRecordTAXGIByGI]: Error in updating kloodge. gi=%ld", taxgi_gi.val);
					LogSQLInfo(HNDLTaxgiUpdateKloodge);
					return FALSE;
				}
			 }
			 if((ptgtemp->type == NULL) && (ptg->type != NULL)) {
				sprintf((char *)taxgi_type.val,"%s", IsNullStr(ptg->type));
				retcode = SQLExecute (HNDLTaxgiUpdateType);
				if (retcode != SQL_SUCCESS) {
					ErrPostEx(SEV_ERROR,0,0, "[EditRecordTAXGIByGI]: Error in updating type");
					LogSQLInfo(HNDLTaxgiUpdateType);
					return FALSE;
				}
			 }
		 }

   } /* while fetch */


   /* close the cursor and discard all pending results. */
   /* must do in order to re-use the handle created in InitTAXGI */
   retcode = SQLFreeStmt(HNDLTaxgiSearchGi, SQL_CLOSE);
   if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[EditRecordTAXGIByGI]: Error in closing cursor.");
      return FALSE;
   }

    StTaxgiFree(ptgtemp);

	return TRUE;
}

/* Edit the kloodge for a given gi. Do not search on gi first, 
  unlike EditRecordTAXGIByGI() as this is slow.
   Used in postcomgen.c UpdateTaxgiWithKloodge(). */
Boolean LIBCALL EditTAXGIKloodgeByGi( Int4 gi, Int4 kloodge )
{
  Int4 retcode;

  if( gi <= 0 || kloodge < 0 )
  {
	ErrPostEx(SEV_ERROR,0,0, "[EditTAXGIKloodgeByGi]: "
			  "Invalid parameters.");
	return FALSE;
  }

  taxgi_gi.val  = gi;
  taxgi_kloodge.val  = kloodge;

  retcode = SQLExecute( HNDLTaxgiUpdateKloodge );

  if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
  {
	ErrPostEx(SEV_ERROR,0,0, "EditTAXGIKloodgeByGi: "
			  "Could not execute update statement for %d.",
			  taxgi_gi.val );

	LogSQLInfo(HNDLTaxgiUpdateKloodge);
	
	return FALSE;
  }

  return TRUE;
}

/* Set all records with oldKloodge to newKloodge */
Boolean LIBCALL EditRecordTAXGIByKloodge(Int4 oldKloodge, 
										 Int4 newKloodge)
{
  Int4 retcode;

  /*---------------------------------------------------------------------
	Remember, kloodge is our kloodge (i.e. auto-increment column from
	chrom and should never be < 0.
	---------------------------------------------------------------------*/
  if( oldKloodge < 0 || newKloodge < 0 )
  {
	ErrPostEx(SEV_ERROR,0,0, "[EditRecordTAXGIByKloodge]: Invalid parameters.");
	return FALSE;
  }


  taxgi_kloodge.val  = newKloodge;
  taxgi_kloodge2.val = oldKloodge;
	
  retcode = SQLExecute(HNDLTaxgiUpdateKloodgeByKloodge);

  if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
  {
	ErrPostEx(SEV_ERROR,0,0, "EditRecordTAXGIByKloodge: "
			  "Could not execute update statement for %d.",
			  taxgi_kloodge2.val );

	LogSQLInfo(HNDLTaxgiUpdateKloodgeByKloodge);
	
	return FALSE;
  }

  return TRUE;
  
} 
/*******************************************************/
/*Given a pointer to the asn structure that represents */
/*  a taxgi record this function pulls out the         */
/*  taxid, kloodge, or gi from the asn structure and   */
/*  retrieves all the records associated with that key */
/*Returns : TRUE if successful and FALSE otherwise.    */
/*******************************************************/

Int2 LIBCALL SearchTAXGI (StTaxgiPtr PNTR pptg)
{
   StTaxgiPtr ptg = NULL;
   SQLRETURN retcode;
   SQLINTEGER iKey;
   Char *pSQL_stmt;
   SQLHANDLE hstmt;
   /* note there are 4 results the select statement asks for: gi, taxid, kloodge & type */

   if (pptg == NULL)
   {
       ErrPostEx(SEV_ERROR,0,0, "SearchTAXGI: StTaxgiPtr is NULL.");
       return 0;
   }
   ptg = *pptg;

   pSQL_stmt = (char*) malloc(sizeof(char) * 150);
   if(pSQL_stmt == NULL)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[SearchTAXGI]: Could not allocate space.");
      return 0;
   }
   /* search key in ptg was set, so now we set up our local search keys */
   if (ptg->gi > 0)
   {
      iKey = ptg->gi;
      sprintf((char*) pSQL_stmt, "select gi, taxid, kloodge, type from %s.taxgi where gi = ?;", database);
   } else if (ptg->kloodge > 0) {
      iKey = ptg->kloodge;
      sprintf((char*) pSQL_stmt, "select gi, taxid, kloodge, type from %s.taxgi where kloodge = ?;", database);
   } else if (ptg->taxid > 0){
      iKey = ptg->taxid;
      sprintf((char*) pSQL_stmt, "select gi, taxid, kloodge, type from %s.taxgi where taxid = ?;", database);
   } else{
      ErrPostEx(SEV_ERROR, 0, 0, "[SearchTAXGI]: Invalid search key.");
      free(pSQL_stmt);
      return 0;
   }

   /* create the handle to the select statement */
   if(CreateHandle(&hstmt, pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[SearchTAXGI]: Could not create handle.");
      SQLFreeStmt(hstmt, SQL_DROP);
      return 0;
   }
   /* bind the parameter 1 to keynum */
   retcode = SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &iKey, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[SearchTAXGI]: Could not bind parameter.");
      SQLFreeStmt(hstmt, SQL_DROP);
      return 0;
   }

   /* execute statment */
   retcode = SQLExecute(hstmt);
   if (retcode != SQL_SUCCESS)
   {
	  ErrPostEx(SEV_ERROR,0,0, "[SearchTAXGI]: Unable to execute select statement.");
	  LogSQLInfo(hstmt);
      SQLFreeStmt(hstmt, SQL_DROP);
      return 0;
   }

   /* bind the 4 columns from the select statement */
   SQLBindCol(hstmt, 1, SQL_C_SLONG, &taxgi_gi.val, 0, NULL);
   SQLBindCol(hstmt, 2, SQL_C_SLONG, &taxgi_taxid.val, 0, NULL);
   SQLBindCol(hstmt, 3, SQL_C_SLONG, &taxgi_kloodge.val, 0, NULL);
   SQLBindCol(hstmt, 4, SQL_C_CHAR, taxgi_type.val, 15, (int*)&taxgi_type.len);

   /* retrieve the column data */
   SQLINTEGER fetchCount = 0;
   /* fetch result and save in the linked list */
   while((retcode=SQLFetch(hstmt)) == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
   {
      fetchCount++;
      if (fetchCount>1)
      {
	     ptg->next = StTaxgiNew();
	     ptg = ptg->next;
      }


	  ptg->gi = taxgi_gi.val;
      ptg->taxid = taxgi_taxid.val;
      ptg->kloodge = taxgi_kloodge.val;
      ptg->type = StringSave((CharPtr) taxgi_type.val);
   }

   /* free everything */
   retcode = SQLFreeStmt(hstmt, SQL_DROP);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_INFO, 0 ,0, "SearchTAXGI: Unable to free statement.");
      return 0;
   }

   if (fetchCount == 0)
	  return -1;

   return 1;
}


Boolean LIBCALL DeleteGIinTAXGI(SQLINTEGER gi, Boolean destroy)
{
   Int2 sql_return = SQL_SUCCESS;

   taxgi_gi.val = gi;

   sql_return = SQLExecute(HNDLTaxgiDelete);
   /* execute statment */
   if (sql_return != SQL_SUCCESS)
   {
	  ErrPostEx(SEV_ERROR,0,0, "DeleteGIinTAXGI:unable to execute delete statement.");
	  LogSQLInfo(HNDLTaxgiDelete);
      return FALSE;
   }

   return TRUE;
}

/*find the absolute largest taxid in the database */
Int4 LIBCALL GetLargestTaxid(void)
{
   CharPtr search_taxgi = (char *) malloc(sizeof(Char) * 150);
   if (search_taxgi == NULL)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[GetLargestTaxid]: Could not allocate space");
     free(search_taxgi);
     return 0;
   }
   sprintf((char *)search_taxgi, "SELECT MAX(taxid) as Max from %s.taxgi", database);

   SQLHANDLE hstmt;
    Int2 sqlrc=SQL_SUCCESS;
    struct
    {
       SQLINTEGER len;
       SQLINTEGER val;
    }max;

   /* NULL returned for max(taxid) if the table is empty */
   /* initialize them to avoid a weird number */
	max.val=0; max.len=0;

    CreateHandle(&hstmt, search_taxgi, hdbc, &henv);

   /* execute statment */
   sqlrc = SQLExecute(hstmt);
   if (sqlrc != SQL_SUCCESS)
   {
	  ErrPostEx(SEV_ERROR,0,0, "GetLargestTaxid:unable to execute select statement.");
	  LogSQLInfo(hstmt);
      SQLFreeStmt(hstmt, SQL_DROP);
      return 0;
   }

   SQLBindCol(hstmt,1,SQL_C_SLONG,&max.val,0,(int*)&max.len);

   sqlrc = SQLFetch(hstmt);
   if(sqlrc == SQL_NO_DATA_FOUND){
      ErrPostEx(SEV_INFO, 0, 0, "No data found");
      SQLFreeStmt(hstmt, SQL_DROP);
      return 0;
   }

   if(sqlrc == SQL_SUCCESS || sqlrc == SQL_SUCCESS_WITH_INFO)
   {
      SQLFreeStmt(hstmt, SQL_DROP);
      return max.val;
   }

    SQLFreeStmt(hstmt, SQL_DROP);
    return -1;
}


/* Given a taxid, retrieve all records for it,
*  put the records into a list of DNA/RNA and a list of protein
*  returns the number of records */
Int4 LIBCALL getTaxInfoIII(Int4 taxid, PostingPtr* psp_all,  PostingPtr* psp_na, PostingPtr* psp_prot, Int4 *num_all, Int4 *num_na, Int4 *num_prot)
{

   SQLHANDLE hstmt;
   Int2 sqlrc=SQL_SUCCESS;
   SQLINTEGER iKey = taxid;
   Int4 count=0;

   /* protein */
   CharPtr search_taxgi = (char *) malloc(sizeof(Char) * 150);
   if (search_taxgi == NULL)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[getTaxInfoIII]: Could not allocate space");
     free(search_taxgi);
     return 0;
   }
   sprintf((char *)search_taxgi, "SELECT gi, type from %s.taxgi where taxid = ?", database);

   struct{
     SQLINTEGER len;
     SQLINTEGER val;
   }gi;

   struct{
     SQLINTEGER len;
     SQLCHAR val[50];
   }type;

  sqlrc = CreateHandle(&hstmt,search_taxgi, hdbc, &henv);
  if (sqlrc != SQL_SUCCESS && sqlrc != SQL_SUCCESS_WITH_INFO)
  {
    ErrPostEx(SEV_ERROR,0,0,"getTaxInfoIII: Could not create handle.");
    SQLFreeStmt(hstmt, SQL_DROP);
    return 0;
  }
  free(search_taxgi);

  sqlrc = SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &iKey, 0, NULL);
   if((sqlrc != SQL_SUCCESS) && (sqlrc != SQL_SUCCESS_WITH_INFO))
   {
      SQLFreeStmt(hstmt, SQL_DROP);
      ErrPostEx(SEV_ERROR, 0, 0, "getTaxInfoIII: SQLBindParameter fail");
      return 0;
   }

  sqlrc = SQLExecute(hstmt);
   if (sqlrc != SQL_SUCCESS)
   {
	  ErrPostEx(SEV_ERROR,0,0, "getTaxInfoIII: unable to execute select statement.");
	  LogSQLInfo(hstmt);
      SQLFreeStmt(hstmt, SQL_DROP);
      return 0;
   }

  sqlrc = SQLBindCol(hstmt,1,SQL_C_SLONG,&gi.val,0,(int*)&gi.len);
  if (sqlrc != SQL_SUCCESS && sqlrc != SQL_SUCCESS_WITH_INFO)
  {
    ErrPostEx(SEV_ERROR,0,0,"getTaxInfoIII: unable to execute bind column 1.");
    SQLFreeStmt(hstmt, SQL_DROP);
    return 0;
  }
  sqlrc = SQLBindCol(hstmt,2,SQL_C_CHAR,type.val,50,(int*)&type.len);
  if (sqlrc != SQL_SUCCESS && sqlrc != SQL_SUCCESS_WITH_INFO)
  {
    ErrPostEx(SEV_ERROR,0,0,"getTaxInfoIII: unable to execute bind column 2.");
    SQLFreeStmt(hstmt, SQL_DROP);
    return 0;
  }

  /* fetch result and save in the linked list */
   while((sqlrc=SQLFetch(hstmt)) == SQL_SUCCESS || sqlrc == SQL_SUCCESS_WITH_INFO)
   {
	   if (strstr((char*)type.val, "protein") != NULL) {
		   WriteItem(*psp_all, gi.val);
		   WriteItem(*psp_prot, gi.val);
		   count++;
		   (*num_all)++;
		   (*num_prot)++;
	   }
	   else if (strstr((char*)type.val, "NA") != NULL) {
		   WriteItem(*psp_all, gi.val);
		   WriteItem(*psp_na, gi.val);
		   count++;
		   (*num_all)++;
		   (*num_na)++;
	   }
   }

	FlushItems(*psp_all);
    FlushItems(*psp_na);
    FlushItems(*psp_prot);

    SQLFreeStmt(hstmt, SQL_DROP);

	return count;
}

/************************************************************/
/*  Given : the name of the table to be created and whether */
/*  the table is to be created (both are remenants          */
/*  from codebase)*/
/*  This method opens the table in codebase but in DB2      */
/*  there is no need to open the table.  This function      */
/*  creates the table in ODBC if create is true and nothing  */
/*  otherwise.                                              */
/*  Returns : trueif successful and False otherwise         */
/*  ABOVE COMMECT APPLIES FOR ALL OF THE OPENXXXXX FUNCTIONS*/
/************************************************************/
/*
 *RAC : Crating a Table in this ODBC Layer doesn't make sence, because
 *      different databases have different ways to create tables.
 */
Boolean LIBCALL OpenASNDB(CharPtr name, Boolean Create)
{

  /*check to see if table is supposed to be created.if yes then create it.*/
  if(Create)
  {
     SQLCHAR * asndb_create = (SQLCHAR *) "create table seqhound.asndb ("
       "gi INTEGER NOT NULL PRIMARY KEY,"
       "asn1 MEDIUMBLOB NOT LOGGED ,"
       "division VARCHAR(10) NOT NULL,"
       "release VARCHAR(10) NOT NULL,"
       "type VARCHAR(15) NOT NULL)";
     if(!execute_sql(asndb_create, &hdbc, &henv))
     {
	 return FALSE;
     }
  }
  return TRUE;
}

Boolean LIBCALL AppendRecordASNDB(StAsndbPtr pas)
{
   SQLRETURN retcode;
   pABL pABLtmp = NULL;

   if(pas == NULL)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[AppendRecordASNDB]: NULL parameter.");
      return FALSE;
   }
   if(pas->gi < 1)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[AppendRecordASNDB]: Invalid gi.");
      return FALSE;
   }

   /* get the asn1 bioseq into a bytestore pointer so that we can put it into a buffer. */
   if (pas->asn1 != NULL) {
      pABLtmp = (pABL)AssignASNMemChar((Pointer) pas->asn1, (AsnWriteFunc) BioseqAsnWrite);
	  if (pABLtmp == NULL) {
		  ErrPostEx(SEV_ERROR, 0, 0, "[AppendRecordASNDB]: AssignASNMemChar failed.");
		  ClearStructures();
		  return FALSE;
	  }
      asndb_asn.val = pABLtmp->buf;
      asndb_asn.len = pABLtmp->len;
   }
   else {
	   asndb_asn.val = NULL;
	   asndb_asn.len = 0;
   }

   /* set up the input parameter. We bind the blob here because its size
    * changes for each blob, so we must individually bind it here. */
   retcode = SQLBindParameter(HNDLAsndb, 2, SQL_PARAM_INPUT, SQL_C_DEFAULT, SQL_LONGVARBINARY, 0, 0, asndb_asn.val, 0, (int*)&asndb_asn.len);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO) {
     ErrPostEx(SEV_ERROR, 0, 0, "ASNDB: Could not bind parameter 2");
     return FALSE;
   }

   asndb_gi.val = pas->gi;
   sprintf((char *)asndb_type.val,"%s",IsNullStr(pas->type));
   sprintf((char *)asndb_div.val,"%s",IsNullStr(pas->division));
   sprintf((char *)asndb_release.val,"%s",IsNullStr(pas->release));

   /* execute statment */
   retcode = SQLExecute(HNDLAsndb);
   if (retcode != SQL_SUCCESS)
   {
	  ErrPostEx(SEV_ERROR,0,0, "[AppendRecordASNDB]: cannot insert (Gi:%ld, div=%s, type=%s)",
                    asndb_gi.val, asndb_div.val, asndb_type.val);
	  LogSQLInfo(HNDLAsndb);

       free(asndb_asn.val);
       FreeABL(pABLtmp);
      return FALSE;
   }

   /*free all the memory that we have allocated*/     /* RAC What happend wiht asn1 if SQLExecute Fail. */
   free(asndb_asn.val);

   FreeABL(pABLtmp);

   return TRUE;
}


/* RAC I have same problems with the pointer PNTR
     and with the assignment: pas = ppass; insted of pas = *ppass */
Int2 LIBCALL SearchASNDB(StAsndbPtr PNTR ppas)
{
   if (ppas == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "[SearchASNDB]: StAsndbPtr is NULL.");
       return 0;
    }

   StAsndbPtr pas = NULL;
   pas = *ppas;
   if (pas->gi <= 0)
    {
       ErrPostEx(SEV_ERROR,0,0, "SearchASNDB: Invalid gi as search key.");
       return 0;
    }
   asndb_gi.val = pas->gi;

   Int4 bloblen = 0, indicator = 0;
   SQLHANDLE LocUse;
   SQLRETURN retcode = SQL_SUCCESS;
   SQLINTEGER blobloc = 0;
   Int4 pbbvalue = 0;

   CharPtr pcASN = NULL;
   struct
   {
     Int4 len;
     Char val[15];
   }division, release, type;

   /* execute statment */
   retcode = SQLExecute(HNDLAsndbSearchGi);
   if (retcode != SQL_SUCCESS)
   {
	  ErrPostEx(SEV_ERROR,0,0, "[SearchASNDB]: Unable to execute select statement.");
	  LogSQLInfo(HNDLAsndbSearchGi);
	  SQLFreeStmt(HNDLAsndbSearchGi, SQL_CLOSE);
	  return 0;
   }

   retcode = SQLBindCol(HNDLAsndbSearchGi, 1, SQL_C_CHAR, division.val, 10, (int*)&division.len);
   if((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO))
   {
      ErrPostEx(SEV_ERROR,0,0, "[SearchASNDB]: Binding Column 1 failed.");
	  SQLFreeStmt(HNDLAsndbSearchGi, SQL_CLOSE);
      return 0;
   }
   retcode = SQLBindCol(HNDLAsndbSearchGi, 2, SQL_C_CHAR, release.val, 10, (int*)&release.len);
   if((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO))
   {
      ErrPostEx(SEV_ERROR,0,0, "[SearchASNDB]: Binding Column 2 failed.");
	  SQLFreeStmt(HNDLAsndbSearchGi, SQL_CLOSE);
      return 0;
   }
   retcode = SQLBindCol(HNDLAsndbSearchGi, 3, SQL_C_CHAR, type.val, 15, (int*)&type.len);
   if((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO))
   {
      ErrPostEx(SEV_ERROR,0,0, "[SearchASNDB]: Binding Column 3 failed.");
	  SQLFreeStmt(HNDLAsndbSearchGi, SQL_CLOSE);
      return 0;
   }
   retcode = SQLBindCol(HNDLAsndbSearchGi, 4, SQL_C_BINARY, &blobloc, 0, (int*)&pbbvalue);
   if((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO))
   {
      ErrPostEx(SEV_ERROR,0,0, "[SearchASNDB]: Binding Column 4 failed.");
	  SQLFreeStmt(HNDLAsndbSearchGi, SQL_CLOSE);
      return 0;
   }

   /* RAC: unixODBC SQL_C_BINARY replace SQL_C_BLOB_LOCATOR  ??? I'm not shure.
     SQLBindCol(hstmt, 4, SQL_C_BLOB_LOCATOR, &blobloc, 0, (long*)&pbbvalue); */
   SQLINTEGER fetchCount = 0;
   /* fetch result and save in the linked list */
   while((retcode=SQLFetch(HNDLAsndbSearchGi)) == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
   {
      fetchCount++;
      if (fetchCount>1)
      {
         pas->next = StAsndbNew();
		 pas = pas->next;
      }

	  pas->type = StringSave(type.val);
	  pas->division = StringSave(division.val);
	  pas->release = StringSave(release.val);

	  /*Get the length of of the blob so that we can read it */
	  SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &LocUse);
	  bloblen = (Int4)pbbvalue;

	  if(bloblen > 0)
	  {
	      /* allocate a buffer big enough to store the blob. */
	      pcASN = (CharPtr) MemNew((size_t) bloblen);

	      /*read the blob from the database */
	      /* RAC put an if Statement here if (SQLGetData(...) != SQL_SUCCESS) --->> print_err(hstmt) */
	      SQLGetData(HNDLAsndbSearchGi, 4, SQL_C_BINARY, (SQLPOINTER)pcASN, bloblen, (int*)&indicator);

	      if(pcASN != NULL)
	      {
		  pas->asn1 =  (BioseqPtr) GetASNMemChar(pcASN,(AsnReadFunc)BioseqAsnRead, bloblen);
	      }
	      if (pcASN) { MemFree(pcASN); }

	  }
	  else
	  {
	      ErrPostEx(SEV_INFO, 0, 0, "SearchASNDB: ASN1 Blob retrieved has length = 0. gi=%ld", asndb_gi.val);
	      if(pas->type) MemFree(pas->type);
	      if(pas->division) MemFree(pas->division);
	      if(pas->release) MemFree(pas->release);
	  }

	  if (SQLFreeHandle(SQL_HANDLE_STMT, LocUse) != SQL_SUCCESS)
	  {
	      ErrPostEx(SEV_INFO, 0, 0, "SearchASNDB: Unable to free handle");
	      SQLFreeStmt(HNDLAsndbSearchGi, SQL_CLOSE);
	      return 0;
	  }

   }


   retcode = SQLFreeStmt(HNDLAsndbSearchGi, SQL_CLOSE);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_INFO, 0 ,0, "SearchASNDB: Unable to close statement.");
      return 0;
   }

   if (fetchCount == 0)
	  return -1;

   return 1;

}

/* both the div and destroy parameters are ignored in ODBC version.*/
Boolean LIBCALL DeleteGIinASNDB(CharPtr div, SQLINTEGER gi, Boolean destroy)
{
   Int2 sql_return = SQL_SUCCESS;

   asndb_gi.val = gi;

   sql_return = SQLExecute(HNDLAsndbDelete);
  /* execute statment */
   if (sql_return != SQL_SUCCESS)
   {
	  ErrPostEx(SEV_ERROR,0,0, "[DeleteGIinASNDB]:unable to execute delete statement.");
	  LogSQLInfo(HNDLAsndbDelete);
      return FALSE;
   }

  return TRUE;
}


Boolean LIBCALL OpenPUBSEQ(CharPtr name, Boolean Create)
{
  /*check to see if table is supposed to be created.if yes then create it.*/
  if(Create)
  {
     SQLCHAR * pubseq_create = (SQLCHAR *) "create table seqhound.pubseq ("
        "gi INTEGER NOT NULL,"
        "muid INTEGER NOT NULL, "
        "Primary KEY (gi, muid)  )"; 
        if(!execute_sql(pubseq_create, hdbc, &henv))
	{
           return FALSE;
        }
  }
  return TRUE;
}

/*******************************************************/
/*Given a pointer to the asn structure that represents */
/*  an pubseq record this function appends it into     */
/*  database.                                          */
/*  If the gi exists, update either muid or pmid,      */
/*  otherwise, add as a complete new record            */
/*Returns : TRUE if successful and FALSE otherwise.    */
/*******************************************************/

Boolean LIBCALL AppendRecordPUBSEQ(StPubseqPtr pps, Int2 choice)
{
   Boolean bExists = FALSE;               /* if gi exists already */
   SQLRETURN retcode;

   if(pps == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, "AppendRecordPUBSEQ: NULL parameter.");
      return FALSE;
   }

   /* values for both muid and pmid are in muid */
   pubseq_gi.val = pps->gi;
   if (choice == 0) {
      pubseq_muid.val = pps->muid;
      pubseq_pmid.val = 0;
   }
   if (choice == 1) {
      pubseq_muid.val = 0;
      pubseq_pmid.val = pps->muid;
   }

   /* check if the record already exists in database */
   retcode = SQLExecute(HNDLPubseqSearchGi);
   if (retcode != SQL_SUCCESS)
   {
	  ErrPostEx(SEV_ERROR,0,0, "[AppendRecordPUBSEQ]: unable to execute search statement. (gi=%ld)",pubseq_gi.val);
	  LogSQLInfo(HNDLPubseqSearchGi);
      return FALSE;
   }
   retcode = SQLBindCol(HNDLPubseqSearchGi, 1, SQL_C_SLONG, &pubseq_gi.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[AppendRecordPUBSEQ]: Could not bind column 1.");
      return FALSE;
   }
   retcode = SQLFetch(HNDLPubseqSearchGi);
   if (retcode == SQL_NO_DATA_FOUND)
	   bExists = FALSE;
   else if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
	   bExists = TRUE;
   else {
      ErrPostEx(SEV_ERROR, 0, 0, "[AppendRecordPUBSEQ]: Error in fetching.");
      return FALSE;
   }
   /* close the cursor and discard all pending results. */
   /* must do in order to re-use the handle created in InitPUBSEQ */
   retcode = SQLFreeStmt(HNDLPubseqSearchGi, SQL_CLOSE);
   if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[AppendRecordPUBSEQ]: Error in closing cursor.");
      return FALSE;
   }

   if (bExists == TRUE) {
	   /* update record */
	   if (choice == 0) {
		   /* update muid */
		   retcode = SQLExecute(HNDLPubseqUpdateM);
		   if (retcode != SQL_SUCCESS)
		   {
	          ErrPostEx(SEV_ERROR,0,0, "[AppendRecordPUBSEQ]: unable to execute update muid statement. (gi=%ld)",pubseq_gi.val);
	          LogSQLInfo(HNDLPubseqUpdateM);
              return FALSE;
		   }
	   }
	   else if (choice == 1) {
  		   /* update pmid */
		   retcode = SQLExecute(HNDLPubseqUpdateP);
		   if (retcode != SQL_SUCCESS)
		   {
	          ErrPostEx(SEV_ERROR,0,0, "[AppendRecordPUBSEQ]: unable to execute update pmid statement. (gi=%ld)",pubseq_gi.val);
	          LogSQLInfo(HNDLPubseqUpdateP);
              return FALSE;
		   }

	   }
	   else {
	       ErrPostEx(SEV_ERROR,0,0, "[AppendRecordPUBSEQ]: Unexpected choice %ld. (gi=%ld)", choice, pubseq_gi.val);
           return FALSE;
	   }
   }
   else if (bExists == FALSE) {
	   /* add as a new record */
	   retcode = SQLExecute(HNDLPubseqInsertGMP);
	   if (retcode != SQL_SUCCESS)
	   {
	       ErrPostEx(SEV_ERROR,0,0, "[AppendRecordPUBSEQ]: unable to execute insert statement. (gi=%ld)",pubseq_gi.val);
	       LogSQLInfo(HNDLPubseqInsertGMP);
           return FALSE;
	   }
   }

   return TRUE;
}


/*******************************************************/
/*Given a pointer to the asn structure that represents */
/*  an pubseq record this function pulls out the       */
/*  gi, or muid from the asn structure and             */
/*  retrieves all the records associated with that key */
/*Returns : 1 if found, -1 if not found, 0 if error.   */
/*******************************************************/

Int2  LIBCALL SearchPUBSEQ (StPubseqPtr PNTR ppps)
{
   StPubseqPtr pps= NULL;
   SQLHANDLE hstmt;
   Char *pSQL_stmt;
   SQLRETURN retcode;
   SQLINTEGER iKey = 0;

   if (ppps == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, "[SearchPUBSEQ]: StPubseqPtr is NULL.");
      return 0;
   }
   pps = *ppps;

   pSQL_stmt = (char*)malloc(sizeof(char) * 150);
   if (pps->gi > 0)
   {
      iKey = pps->gi;
      sprintf((char*)pSQL_stmt, "select gi, muid, pmid from %s.pubseq where gi = ?;", database);
   } else if(pps->muid > 0){
      iKey = pps->muid;
      sprintf((char*) pSQL_stmt, "select gi, muid, pmid from %s.pubseq where muid = ?;", database);
   } else if(pps->pmid > 0){
      iKey = pps->pmid;
      sprintf((char*) pSQL_stmt, "select gi, muid, pmid from %s.pubseq where pmid = ?;", database);
   } else{
      ErrPostEx(SEV_ERROR, 0, 0, "[SearchPUBSEQ]: Invalid search key.");
      free(pSQL_stmt);
      return 0;
   }

   if(CreateHandle(&hstmt, pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
   {
      SQLFreeStmt(hstmt, SQL_DROP);
      ErrPostEx(SEV_ERROR, 0, 0, "[SearchPUBSEQ]: Could not allocate handle.");
      free(pSQL_stmt);
      return 0;
   }
   free(pSQL_stmt);

   /* Bind SQL Arguments. */
   retcode = SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &iKey, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      SQLFreeStmt(hstmt, SQL_DROP);
      ErrPostEx(SEV_ERROR, 0, 0, "[SearchPUBSEQ]: Could not bind parameter 1.");
      return 0;
   }

   /* execute statment */
   retcode = SQLExecute(hstmt);
   if (retcode != SQL_SUCCESS)
   {
	  ErrPostEx(SEV_ERROR,0,0, "[SearchPUBSEQ]: Unable to execute select statement.");
	  LogSQLInfo(hstmt);
      SQLFreeStmt(hstmt, SQL_DROP);
      return 0;
   }

   retcode = SQLBindCol(hstmt, 1, SQL_C_SLONG, &pubseq_gi.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[SearchPUBSEQ]: Could not bind column 1.");
      SQLFreeStmt(hstmt, SQL_DROP);
      return 0;
   }
   retcode = SQLBindCol(hstmt, 2, SQL_C_SLONG, &pubseq_muid.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[SearchPUBSEQ]: Could not bind column 2.");
      SQLFreeStmt(hstmt, SQL_DROP);
      return 0;
   }
   retcode = SQLBindCol(hstmt, 3, SQL_C_SLONG, &pubseq_pmid.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[SearchPUBSEQ]: Could not bind column 3.");
      SQLFreeStmt(hstmt, SQL_DROP);
      return 0;
   }

   SQLINTEGER fetchCount = 0;
  /* fetch result and save in the linked list */
   while((retcode=SQLFetch(hstmt)) == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
   {
      fetchCount++;
      if (fetchCount>1)
      {
         pps->next = StPubseqNew();
         pps = pps->next;
      }

      pps->gi = pubseq_gi.val;
      pps->muid = pubseq_muid.val;
      pps->pmid = pubseq_pmid.val;
   }

   if(SQLFreeStmt(hstmt, SQL_DROP) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[SearchPUBSEQ]: Could not free statement.");
      return 0;
   }

   if (fetchCount == 0)
	  return -1;

    return 1;
}

/*******************************************************/
/*Given a pointer to the asn structure that represents */
/*  an pubseq record this function inserts it into     */
/*  database as a completely new record                */
/*Returns : TRUE if successful and FALSE otherwise.    */
/*******************************************************/
/* not in use */
static Boolean AddNewRecordPUBSEQ(StPubseqPtr pps, Int2 choice)
{
   SQLRETURN retcode;
   SQLHANDLE hstmt;               /* insert statement handle */
   Char *pSQL_stmt;               /* the select statement */

   pubseq_gi.val = pps->gi;
   pubseq_muid.val = pps->muid;

   /* allocate insert statement & initialize: using global variable
      database that gets initialized in ConnectToDb. its value is
      set in init file. eg database = seqhound */
   pSQL_stmt = (char *) malloc (sizeof(Char) * 150);
   if(pSQL_stmt == NULL)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[AddNewRecordPUBSEQ]: Could not allocate space.");
     return FALSE;
   }

   /* create handle for insertion statement */
   if (choice == 0 && pubseq_gi.val)
      sprintf((char*) pSQL_stmt, "insert into %s.pubseq(gi, muid) values(?,?);", database);
   else if (choice == 1 && pubseq_muid.val)
      sprintf((char*) pSQL_stmt, "insert into %s.pubseq(gi, pmid) values(?,?);", database);
   else
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[AddNewRecordPUBSEQ]: Invalid value for choice %ld or muid %ld or pmid %ld. (gi=%ld)",
      		choice, pubseq_muid, pubseq_pmid.val, pubseq_gi.val);
      return FALSE;
   }

   if(CreateHandle(&hstmt, pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[AppendRecordPUBSEQ]: Could not create handle");
      return FALSE;
   }
   /* bind parameters 1 & 2 */
   retcode = SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &pubseq_gi.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[AppendRecordPUBSEQ]: Could not bind parameter 1");
      return FALSE;
   }

   /* for now, the value for muid/pmid is stored in pps->muid */
   if (pps->muid)
      retcode = SQLBindParameter(hstmt, 2, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &pubseq_muid.val, 0, NULL);

   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[AppendRecordPUBSEQ]: Could not bind parameter 2");
      return FALSE;
   }

   /* execute statment */
   retcode = SQLExecute(hstmt);
   if (retcode != SQL_SUCCESS)
   {
	  ErrPostEx(SEV_ERROR,0,0, "[AppendRecordPUBSEQ]: unable to execute PUBSEQ insert statement. (gi=%ld)", pubseq_gi.val);
	  LogSQLInfo(hstmt);
      return FALSE;
   }

   return TRUE;
}

/* not in use */
static Boolean UpdatePUBSEQ(StPubseqPtr pps, Int2 choice)
{
   SQLHANDLE hstmt;
   Char *pSQL_stmt;
   SQLRETURN retcode;

   pubseq_gi.val = pps->gi;
   pubseq_muid.val = pps->muid;

   pSQL_stmt = (char*)malloc(sizeof(char) * 150);

   if (choice == 0) {
      sprintf((char*)pSQL_stmt, "update %s.pubseq set muid = ? where gi = ?;", database);
   } else if(choice == 1) {
      sprintf((char*)pSQL_stmt, "update %s.pubseq set pmid = ? where gi = ?;", database);
   } else{
      ErrPostEx(SEV_ERROR, 0, 0, "[UpdatePUBSEQ]: Invalid choice %ld.", choice);
      free(pSQL_stmt);
      return FALSE;
   }

   if(CreateHandle(&hstmt, pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[UpdatePUBSEQ]: Could not allocate handle.");
      free(pSQL_stmt);
      return FALSE;
   }
   free(pSQL_stmt);

   /* for now, the value for muid/pmid is stored in pps->muid */
   retcode = SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &pubseq_muid.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[UpdatePUBSEQ]: Could not bind parameter 1.");
      return FALSE;
   }

   retcode = SQLBindParameter(hstmt, 2, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &pubseq_gi.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[UpdatePUBSEQ]: Could not bind parameter 2.");
      return FALSE;
   }

   /* execute statment */
   retcode = SQLExecute(hstmt);
   if (retcode != SQL_SUCCESS)
   {
	  ErrPostEx(SEV_ERROR,0,0, "[UpdatePUBSEQ]: Unable to execute update statement.");
	  LogSQLInfo(hstmt);
      SQLFreeStmt(hstmt, SQL_DROP);
      return FALSE;
   }

   if(SQLFreeStmt(hstmt, SQL_DROP) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[UpdatePUBSEQ]: Could not free statement.");
      return FALSE;
   }

   return TRUE;
}


/* RAC: When DeleteGIinPUBSEQ execute and the GI dosen't exist the function
      does not return FALSE, allways return TRUE. */
Boolean LIBCALL DeleteGIinPUBSEQ(SQLINTEGER gi, Boolean destroy)
{
   Int2 sql_return = SQL_SUCCESS;

   pubseq_gi.val = gi;

  sql_return = SQLExecute(HNDLPubseqDelete);
  /* execute statment */
   if (sql_return != SQL_SUCCESS)
   {
	  ErrPostEx(SEV_ERROR,0,0, "[DeleteGIinPUBSEQ]:unable to execute delete statement.");
	  LogSQLInfo(HNDLPubseqDelete);
      return FALSE;
   }

    return TRUE;
}


Boolean LIBCALL OpenSENDB(CharPtr name, Boolean Create)
{
   /*check to see if table is supposed to be created.if yes then create it.*/
  if(Create)
  {
      SQLCHAR * sendb_create = (SQLCHAR *) "create table seqhound.sendb ("
	"seid INTEGER NOT NULL PRIMARY KEY,"
	"asn1 BLOB(1M) NOT NULL NOT LOGGED)";

      if(!execute_sql(sendb_create, hdbc, &henv))
	{
	  return FALSE;
	}

  }
  return TRUE;
}

Boolean LIBCALL AppendRecordSENDB(StSendbPtr psd)
{
   SQLRETURN retcode;
   pABL pABLtmp = NULL;

   if(psd == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, "AppendRecordSENDB: NULL parameter.");
      return FALSE;
   }
   /*get the asn1 bioseq into a bytestore pointer so that we can put it into a buffer.*/
   if (psd->asn1 != NULL) {
      pABLtmp = (pABL)AssignASNMemChar((Pointer) psd->asn1, (AsnWriteFunc) SeqEntryAsnWrite);
	  if (pABLtmp == NULL) {
		  ErrPostEx(SEV_ERROR, 0, 0, "[AppendRecordSENDB]: AssignASNMemChar failed.");
		  ClearStructures();
		  return FALSE;
	  }
      sendb_asn.val = pABLtmp->buf;
      sendb_asn.len = pABLtmp->len;
   }
   else {
      sendb_asn.val = NULL;
      sendb_asn.len = 0;
   }

   /*set up the input parameters */
   retcode = SQLBindParameter(HNDLSendb, 2, SQL_PARAM_INPUT, SQL_C_DEFAULT,  SQL_LONGVARBINARY, 0, 0, sendb_asn.val, 0, (int*)&sendb_asn.len);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO) {
     ErrPostEx(SEV_ERROR, 0, 0, "SENDB: Could not bind parameter 2");
     return FALSE;
   }
   sendb_seid.val = psd->seid;

   /* execute statment */
   retcode = SQLExecute (HNDLSendb);
   if (retcode != SQL_SUCCESS)
   {
	  ErrPostEx(SEV_ERROR,0,0, "[AppendRecordSENDB]: unable to execute SENDB insert statement. (seid=%ld)", sendb_seid.val);
	  LogSQLInfo(HNDLSendb);

      free(sendb_asn.val);
      FreeABL(pABLtmp);

      return FALSE;
   }

   free(sendb_asn.val);
   FreeABL(pABLtmp);
   return TRUE;
}

static Boolean LIBCALL ExpandSeqEntry(SeqEntryPtr PNTR ppse)
{
   BioseqSetPtr pbss = NULL;
   SeqEntryPtr pseNew = NULL, pse = NULL;
   BioseqPtr pbsfull = NULL, pbs = NULL;
   Int4 gival = 0;

   if (ppse == NULL || *ppse == NULL) {
	   ErrPostEx(SEV_ERROR, 0, 0, "ExpandSeqEntry: NULL ppse.");
	   return FALSE;
   }

   pse = *ppse;
   if (pse->choice == 1) /*  It is a Bioseq  */
    {
      pbs = (BioseqPtr) pse->data.ptrvalue;
      if (pbs == NULL)
       {
         ErrPostEx(SEV_ERROR,0,0,"ExpandSeqEntry: Null pointer to Bioseq.");
         return FALSE;
       }

      if ((gival = GetGI(pbs)) <= 0)
       {
         ErrPostEx(SEV_ERROR,0,0,"ExpandSeqEntry: Invalid GI.");
         return FALSE;
       }

      /*  get the full bioseq from ASNDB */
      pbsfull = SHoundGetBioseq(gival);
      pse->data.ptrvalue = pbsfull;

      /*  free the original bioseq  */
      if(pbs)BioseqFree(pbs);
    }
   else if (pse->choice == 2) /*  it is nested Bioseq-set  */
    {

         pbss = (BioseqSetPtr) pse->data.ptrvalue;
         if (pbss == NULL)
           {
             ErrPostEx(SEV_ERROR,0,0,"ExpandSeqEntry: Null pointer to Bioseq-Set.");
             return FALSE;
           }

         pseNew = pbss->seq_set;
         while (pseNew != NULL)
            {
             if(!ExpandSeqEntry(&pseNew))
               {
                ErrPostEx(SEV_ERROR,0,0,"ExpandSeqEntry: The recursive call to ToBiseqAs failed");
                return FALSE;
               }
              pseNew = pseNew->next;
            }
    }
   else
    {
         ErrPostEx(SEV_ERROR,0,0,"ExpandSeqEntry: Invalid choice field.");
         return FALSE;
    }
   return TRUE;
}


SeqEntryPtr LIBCALL GetSeqEntryFromSENDB(CharPtr pcASN, Int4 iSize)
{
   AsnIoMemPtr aimp;
   SeqEntryPtr pse = NULL;
   AsnIoPtr aip;

   if(pcASN == NULL)
     return NULL;

   if (pcASN == NULL)
	   return NULL;

   if(iSize<=0){
      /* post error message */
      ErrPostEx(SEV_ERROR, 0,0, "GetSeqEntryFromSENDB:returned an empty asn.1 record.");
      return NULL;
   }

   aimp = AsnIoMemOpen("rb", (BytePtr) pcASN, iSize);
   if (aimp == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "GetSeqEntryFromSENDB: Failed to get AsnIo.");
      if (pcASN) MemFree(pcASN);
      return NULL;
    }
   pse = SeqEntryAsnRead(aimp->aip, NULL);
   if (pse == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, "GetSeqEntryFromSENDB: SeqEntry is NULL.");
      if (pcASN) MemFree(pcASN);
      AsnIoMemClose(aimp);
      return NULL;
   }

   AsnIoFlush(aimp->aip);
   AsnIoMemClose(aimp);

   if(!ExpandSeqEntry(&pse))
   {
      ErrPostEx(SEV_ERROR,0,0, "GetSeqEntryFromSENDB: ExpandSeqEntry failed.");
      return NULL;
   }

   return pse;
}

Int2 LIBCALL SearchSENDB(StSendbPtr PNTR ppsd)
{
   StSendbPtr psd = NULL;
   SQLHANDLE hstmt;
   SeqEntryPtr seq;
   Int4 bloblen, indicator, pbbvalue = 0;
   SQLRETURN retcode;
   SQLINTEGER blobloc = 0;
   SQLHANDLE LocUse=0;
   CharPtr pASN = NULL;
   SQLINTEGER iKey = 0;

   if (ppsd == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, "SearchSENDB: StSendbPtr is NULL.");
      return 0;
   }
   if ((*ppsd)->seid <= 0)
   {
       ErrPostEx(SEV_ERROR,0,0, "SearchSENDB: Search key is %d.", (*ppsd)->seid);
       return 0;
   }
   /* create the select statement: using global variable database which holds the
      database name that we will be search, it is read in from initialization
      file in function ConnectToDb. eg database = 'seqhound' */
   CharPtr pSrch_stmt = malloc(sizeof(char) * 150);
   if(pSrch_stmt == NULL)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[SearchSENDB]: Could not allocate space.");
     return 0;
   }
   sprintf((char*) pSrch_stmt, "select seid, asn1 from %s.sendb where seid = ?;", database);

   psd = *ppsd;
   iKey = psd->seid;

   if(CreateHandle(&hstmt, pSrch_stmt, hdbc, &henv) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[SearchSENDB]: Could not create search statement.");
      return 0;
   }
   /* once we've created the handle, we can free the char*. */
   free (pSrch_stmt);

   /* bind the search parameter to the select statment's handle. */
   retcode = SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &iKey, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[SearchSENDB]: Could not bind parameter 1.");
     return 0;
   }

   /*  execute select statment */
   retcode = SQLExecute(hstmt);
   if (retcode != SQL_SUCCESS)
   {
	  ErrPostEx(SEV_ERROR,0,0, "[SearchSENDB]: Unable to execute select statement.");
	  LogSQLInfo(hstmt);
      SQLFreeStmt(hstmt, SQL_DROP);
      return 0;
   }

   /* bind the return values so we can extract it */
   if( (( retcode = SQLBindCol(hstmt, 1, SQL_C_SLONG, &sendb_seid.val, 0, NULL) ) != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO) )
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[SearchSENDB]: Could not bind column 1.");
      return 0;
   }
   if( (( retcode = SQLBindCol(hstmt, 2, SQL_C_BINARY, &blobloc, 0, (int*)&pbbvalue) ) != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO) )
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[SearchSENDB]: Could not bind column 2.");
      return 0;
   }

   /* fetch the data */
   /* retrieve the column data */
   SQLINTEGER fetchCount = 0;
   /* fetch result and save in the linked list */
   while((retcode=SQLFetch(hstmt)) == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
   {
      fetchCount++;
      if (fetchCount>1)
	  {
		  psd->next = StSendbNew();
		  psd = psd->next;
	  }

	  /*Get the length of of the blob so that we can read it */
      SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &LocUse);

      bloblen = (Int4) pbbvalue;

      if(bloblen > 0)
      {
         /* allocate a buffer big enough to store the blob. */
         pASN = (CharPtr) MemNew((size_t) bloblen);
         /*read the blob from the database */
         retcode = SQLGetData(hstmt, 2, SQL_C_BINARY, (SQLPOINTER)pASN, bloblen, (int*)&indicator);
	     if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
         {
           ErrPostEx(SEV_ERROR, 0, 0, "[SearchSENDB]: Could not get data.");
           return 0;
         }

         if(pASN != NULL)
         {
            psd->asn1 = GetSeqEntryFromSENDB(pASN, bloblen);
         }
		 if (pASN) { MemFree(pASN); }

      }
	  else
	  {
	      ErrPostEx(SEV_INFO, 0, 0, "SearchSENDB: ASN1 Blob retrieved has length = 0. seid=%ld", iKey);
		  return 0;
	  }

	  if (SQLFreeHandle(SQL_HANDLE_STMT, LocUse) != SQL_SUCCESS)
	  {
	      ErrPostEx(SEV_INFO, 0, 0, "SearchSENDB: Unable to free handle");
	      SQLFreeStmt(hstmt, SQL_DROP);
	      return 0;
	  }

   }

   if(SQLFreeStmt(hstmt, SQL_DROP) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_INFO, 0 ,0, "SearchSENDB: Unable to free statement.");
      return 0;
   }

   if (fetchCount == 0)
	  return -1;

   return 1;
}

Int2 LIBCALL SearchSENDBforGI(StSendbPtr PNTR ppsd, SQLINTEGER gi) {
   StSendbPtr psd = NULL;
   SQLHANDLE LocUse=0;
   Int4 bloblen=0,indicator=0, pbbvalue=0;
   Int2 sqlrc=SQL_SUCCESS;
   SQLINTEGER blobloc=0;
   CharPtr pASN = NULL;

   if (ppsd == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "SearchSENDBforGI: StSendbPtr is NULL.");
       return 0;
    }
   psd = *ppsd;

  struct{
     SQLINTEGER len;
     SQLINTEGER val;
   }seid;

  sengi_gi.val = gi;

  /* execute statment */
  if (SQLExecute(HNDLSendbSearchGi) != SQL_SUCCESS){
    ErrPostEx(SEV_ERROR,0,0,"SearchSENDBforGI: unable to execute select statement.");
	LogSQLInfo(HNDLSendbSearchGi);
    SQLFreeStmt(HNDLSendbSearchGi, SQL_CLOSE);
    return 0;
  }

  sqlrc = SQLBindCol(HNDLSendbSearchGi,1,SQL_C_LONG,&seid.val,0,NULL);
  if (sqlrc != SQL_SUCCESS && sqlrc != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[SearchSENDBforGI]: Could not bind column 1.");
      SQLFreeStmt(HNDLSendbSearchGi, SQL_CLOSE);
      return 0;
   }
  sqlrc = SQLBindCol(HNDLSendbSearchGi,2, SQL_C_BINARY,&blobloc,0,(int*)&pbbvalue);
  if (sqlrc != SQL_SUCCESS && sqlrc != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[SearchSENDBforGI]: Could not bind column 2.");
      SQLFreeStmt(HNDLSendbSearchGi, SQL_CLOSE);
      return 0;
   }

   SQLINTEGER fetchCount = 0;
   /* fetch result and save in the linked list */
   while((sqlrc=SQLFetch(HNDLSendbSearchGi)) == SQL_SUCCESS || sqlrc == SQL_SUCCESS_WITH_INFO)
   {
      fetchCount++;
      if (fetchCount>1)
	  {
		  psd->next = StSendbNew();
		  psd = psd->next;
	  }

	  psd->seid = seid.val;

	  /*Get the length of of the blob so that we can read it */
      SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &LocUse);

      bloblen = (Int4) pbbvalue;

      if(bloblen > 0)
      {
         /* allocate a buffer big enough to store the blob. */
         pASN = (CharPtr) MemNew((size_t) bloblen);
         /*read the blob from the database */
         sqlrc = SQLGetData(HNDLSendbSearchGi, 2, SQL_C_BINARY, (SQLPOINTER)pASN, bloblen, (int*)&indicator);
	     if(sqlrc != SQL_SUCCESS && sqlrc != SQL_SUCCESS_WITH_INFO)
         {
           ErrPostEx(SEV_ERROR, 0, 0, "[SearchSENDBforGI]: Could not get data.");
           SQLFreeStmt(HNDLSendbSearchGi, SQL_CLOSE);
           return 0;
         }

         if(pASN != NULL)
         {
            psd->asn1 = GetSeqEntryFromSENDB(pASN, bloblen);
         }
		 if (pASN) { MemFree(pASN); }

      }
	  else
	  {
	      ErrPostEx(SEV_INFO, 0, 0, "SearchSENDBforGI: ASN1 Blob retrieved has length = 0. gi=%ld", gi);
              SQLFreeStmt(HNDLSendbSearchGi, SQL_CLOSE);
		  return 0;
	  }

	  if (SQLFreeHandle(SQL_HANDLE_STMT, LocUse) != SQL_SUCCESS)
	  {
	      ErrPostEx(SEV_INFO, 0, 0, "SearchSENDBforGI: Unable to free handle");
	      SQLFreeStmt(HNDLSendbSearchGi, SQL_CLOSE);
	      return 0;
	  }

   }

   if(SQLFreeStmt(HNDLSendbSearchGi, SQL_CLOSE) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_INFO, 0 ,0, "SearchSENDBforGI: Unable to close statement.");
      return 0;
   }

   if (fetchCount == 0)
	  return -1;

   return 1;

}

Int2 LIBCALL SearchSENDBforGIforTindex(StSendbPtr PNTR ppsd, Int4 gi) {
	return SearchSENDBforGI(ppsd, gi);
}


Boolean LIBCALL DeleteGIinSENDB(CharPtr div, SQLINTEGER gi, Boolean destroy)
{
   Int2 sql_return = SQL_SUCCESS;
   SQLINTEGER seidVal=0;

   sengi_gi.val = gi;

   /* search for seid in SENGI */
   sql_return = SQLExecute(HNDLSengiSearchGi);
   /* execute statment */
   if (sql_return != SQL_SUCCESS)
   {
	  ErrPostEx(SEV_ERROR,0,0, "[DeleteGIinSENDB]:unable to execute search statement.");
	  LogSQLInfo(HNDLSengiSearchGi);
      return FALSE;
   }

   sql_return = SQLBindCol(HNDLSengiSearchGi, 1, SQL_C_SLONG, &seidVal, 0, NULL);
   if(sql_return != SQL_SUCCESS && sql_return != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[DeleteGIinSENDB]: Could not bind column 1.");
     return FALSE;
   }

   while((sql_return=SQLFetch(HNDLSengiSearchGi)) == SQL_SUCCESS || sql_return == SQL_SUCCESS_WITH_INFO)
   {
      sendb_seid.val = seidVal;
   }

   /* close the cursor and discard all pending results. */
   /* must do in order to re-use the handle created in InitACCDB */
   sql_return = SQLFreeStmt(HNDLSengiSearchGi, SQL_CLOSE);
   if (sql_return != SQL_SUCCESS && sql_return != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[DeleteGIinSENDB]: Error in closing cursor.");
      return FALSE;
   }

   sql_return = SQLExecute(HNDLSendbDelete);
   /* execute statment */
   if (sql_return != SQL_SUCCESS)
   {
	  ErrPostEx(SEV_ERROR,0,0, "[DeleteGIinSENDB]:unable to execute delete statement.");
	  LogSQLInfo(HNDLSendbDelete);
      return FALSE;
   }

  return TRUE;
}



/***************************************/
/*     Close databases                 */
/***************************************/

Boolean LIBCALL CloseASNDB(void)
{
   return TRUE;
}


Boolean LIBCALL ClosePARTI(void)
{
    return TRUE;
}


Boolean LIBCALL CloseNUCPROT(void)
{
   return TRUE;
}


Boolean LIBCALL CloseACCDB(void)
{
   return TRUE;
}


Boolean LIBCALL ClosePUBSEQ(void)
{
   return TRUE;
}


Boolean LIBCALL CloseTAXGI(void)
{
   return TRUE;
}




Boolean LIBCALL CloseREDUND(void)
{
  return TRUE;
}


Boolean LIBCALL CloseSENDB(void)
{
     return TRUE;
}


Boolean LIBCALL CloseSENGI(void)
{
     return TRUE;
}


Boolean LIBCALL CloseMMDB(void)
{
     return TRUE;
}

Boolean LIBCALL CloseMMGI(void)
{
     return TRUE;
}


Boolean LIBCALL CloseDOMDB(void)
{
     return TRUE;
}

Boolean LIBCALL CloseCDDB(void)
{
     return TRUE;
}

Boolean LIBCALL CloseVASTDB(void)
{
     return TRUE;
}

/********************************************************************************
 *   Functions already not ported to ODBC                                       *
 ********************************************************************************/
Boolean LIBCALL OpenNUCPROT(CharPtr name, Boolean Create)
{
 if(Create)
 {
   SQLCHAR * nucprot_create = (SQLCHAR *) malloc (sizeof(SQLCHAR) * 500);
   if(nucprot_create == NULL)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "Could not allocate create statement");
     return FALSE;
   }
   sprintf((char *) nucprot_create, "create table %s.nucprot (gia integer not null primary key, gin integer not null);", database);
   /* SQLCHAR * nucprot_create = (SQLCHAR *) "create table seqhound.nucprot ("
	"gia INTEGER NOT NULL Primary KEY,"
	"gin INTEGER NOT NULL)";
   */

   /*check to see if table is supposed to be created.if yes then create it.*/
   if(!execute_sql(nucprot_create, &hdbc, &henv))
   {
     return FALSE;
   }
 }
 return TRUE;
}


Boolean LIBCALL AppendRecordNUCPROT(StNucprotPtr pnc)
{
   SQLRETURN retcode;
   if(pnc == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, "AppendRecordNUCPROT: NULL parameter.");
      return FALSE;
   }

   /* assign values to the parameters according to what was passed into the method. */
   nucprot_gia.val = pnc->gia;
   nucprot_gin.val = pnc->gin;

   /* execute statment */
   retcode = SQLExecute(HNDLNucprot);
   if (retcode != SQL_SUCCESS)
   {
	  ErrPostEx(SEV_ERROR,0,0, "[AppendRecordNUCPROT]: Unable to execute NUCPROT insert statement.");
	  LogSQLInfo(HNDLNucprot);
      return FALSE;
   }

   return TRUE;
}

Boolean LIBCALL DeleteGIinNUCPROT(SQLINTEGER gi, Boolean destroy)
{
   Int2 sql_return = SQL_SUCCESS;

   nucprot_gin.val = gi;
   nucprot_gia.val = gi;

  sql_return = SQLExecute(HNDLNucprotDeleteGin);
  /* execute statment */
   if (sql_return != SQL_SUCCESS)
   {
	  ErrPostEx(SEV_ERROR,0,0, "[DeleteGIinNUCPROT]:unable to execute delete statement.");
	  LogSQLInfo(HNDLNucprotDeleteGin);
      return FALSE;
   }

  sql_return = SQLExecute(HNDLNucprotDeleteGia);
  /* execute statment */
   if (sql_return != SQL_SUCCESS)
   {
	  ErrPostEx(SEV_ERROR,0,0, "[DeleteGIinNUCPROT]:unable to execute delete statement.");
	  LogSQLInfo(HNDLNucprotDeleteGia);
      return FALSE;
   }

    return TRUE;
}


/***********************************************************/
/*  DB2 version.  No longer need to open the table before  */
/*  it is accessed.  Method is given the name of the table */
/*  and whether or not it needs to be created.  True if the*/
/*  table needs to be created and false otherwise.         */
/***********************************************************/
Boolean LIBCALL OpenREDUND(CharPtr name, Boolean Create)
{
  if(Create)
  {
     SQLCHAR * redund_create = (SQLCHAR *) "create table seqhound.redund ("
        "gi INTEGER NOT NULL PRIMARY KEY,"
        "ordinal INTEGER NOT NULL,"
        "group INTEGER NOT NULL)";

/*check to see if table is supposed to be created.if yes then create it.*/
	if(!execute_sql(redund_create,&hdbc,&henv)){
		return FALSE;
	}
   }
   return TRUE;
}


/* this is to mirror CodeBase function which initializes tables with read only permissions */
Boolean LIBCALL InitCodeBaseReadOnly(void)
{
   return InitCodeBase();
}

/*===============================================================
  Functions that mirror CodebaseFunctions.
  =============================================================*/
Boolean LIBCALL SetCodeBaseFlagsExclNoidx(void)
{
  return TRUE;
}

Boolean LIBCALL SetCodeBaseAutoIdxTrue(void)
{
  return TRUE;
}

Boolean LIBCALL OpenREDUNDNoidx(CharPtr name)
{
  return OpenREDUND( name, TRUE );
}

Boolean LIBCALL ReindexREDUND(void)
{
  return TRUE;
}

/*******************************************************/
/*Given a pointer to the asn structure that represents */
/*  an accdb record this function pulls out the        */
/*  accession, name, or gi from the asn structure and  */
/*  retrieves all the records associated with that key */
/*Returns : TRUE if successful and FALSE otherwise.    */
/*******************************************************/

Int2 LIBCALL SearchACCDB (StAccdbPtr PNTR ppac)
{
   StAccdbPtr pac = NULL;                /* linked list to hold return values */
   SQLINTEGER iKey = 0, iLength = 0;           /* search key and length of key */
   CharPtr pKey = NULL;                  /* search key for strings */
   Char *pSQL_stmt;                      /* SQL select statement */
   SQLHANDLE hstmt;                      /* handle to select statement */
   SQLRETURN retcode;                    /* return values for odbc layer */

   if (ppac == NULL)
   {
       ErrPostEx(SEV_ERROR,0,0, "[SearchACCDB]: StAccdbPtr is NULL.");
       return 0;
   }
   pac = *ppac;

  /* allocate space for the select statement */
   pSQL_stmt = (Char*) malloc(sizeof(Char) * 150);
   if(pSQL_stmt == NULL)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[SearchACCDB]: Could not allocate space");
      return 0;
   }


   /* fill in the select statement based on the search key passed in.
      database is a global variable initialized in ConnectToDb. The
      value of database is set in the ini file. eg database = seqhound. */
   if (pac->gi > 0)
   {
     iKey = pac->gi;
      sprintf((char*)pSQL_stmt, "select gi, db, name, access, chain, release, version, title, namelow from %s.accdb where gi=?;", database);
   } else if (pac->access != NULL) {
      pKey = pac->access;
      iLength = strlen(pKey);
      sprintf((char*)pSQL_stmt, "select gi, db, name, access, chain, release, version, title, namelow from %s.accdb where access = ?;", database);
   }else if (pac->name != NULL){
      pKey = pac->name;
      iLength = strlen(pKey);
      sprintf((char*)pSQL_stmt, "select gi, db, name, access, chain, release, version, title, namelow from %s.accdb where name = ?;", database);
   }else if (pac->namelow != NULL){
      pKey = pac->namelow;
      iLength = strlen(pKey);
      sprintf((char*)pSQL_stmt, "select gi, db, name, access, chain, release, version, title, namelow from %s.accdb where namelow = ?;", database);
   } else{
      ErrPostEx(SEV_ERROR, 0, 0, "[SearchACCDB]: No search key found.");
      free(pSQL_stmt);
      return 0;
   }

   /* create handle to statement, once handle exists we can free char* */
   if(CreateHandle(&hstmt, pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[SearchACCDB]: Could not create handle.");
      free(pSQL_stmt);
      return 0;
   }
/*   free(pSQL_stmt);
*/
   /* bind the search key */
   if(iKey > 0)
      retcode = SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &iKey, 0, NULL);
   else
      retcode = SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_DEFAULT, SQL_VARCHAR, 10, 0, pKey, 0, NULL);
      /*retcode = SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_DEFAULT, SQL_VARCHAR, 10, 0, pKey, 0, (long*)&iLength);*/
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[SearchACCDB]: Could not bind parameter");
	  SQLFreeStmt(hstmt, SQL_DROP);
      return 0;
   }

   /* execute the select statement */
   retcode = SQLExecute(hstmt);
   if (retcode != SQL_SUCCESS)
   {
	  ErrPostEx(SEV_ERROR,0,0, "[SearchACCDB]: Could not execute select statement.");
ErrPostEx(SEV_INFO,0,0, "%s %s", pSQL_stmt, pac->access);
	  LogSQLInfo(hstmt);
      SQLFreeStmt(hstmt, SQL_DROP);
      return 0;
   }
   free(pSQL_stmt);

   /* binding our results from the database */

   retcode = SQLBindCol(hstmt, 1, SQL_C_SLONG, &accdb_gi.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[SearchACCDB]: Could not bind column 1.");
	 SQLFreeStmt(hstmt, SQL_DROP);
     return 0;
   }
   retcode = SQLBindCol(hstmt, 2, SQL_C_CHAR, accdb_db.val, 15, (int*)&accdb_db.len);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[SearchACCDB]: Could not bind column 2.");
	 SQLFreeStmt(hstmt, SQL_DROP);
     return 0;
   }
   retcode = SQLBindCol(hstmt, 3, SQL_C_CHAR, accdb_name.val, 30, (int*)&accdb_name.len);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[SearchACCDB]: Could not bind column 3.");
	 SQLFreeStmt(hstmt, SQL_DROP);
     return 0;
   }
   retcode = SQLBindCol(hstmt, 4, SQL_C_CHAR, accdb_access.val, 20, (int*)&accdb_access.len);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[SearchACCDB]: Could not bind column 4.");
	 SQLFreeStmt(hstmt, SQL_DROP);
     return 0;
   }
   retcode = SQLBindCol(hstmt, 5, SQL_C_CHAR, accdb_chain.val, 10, (int*)&accdb_chain.len);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[SearchACCDB]: Could not bind column 5.");
	 SQLFreeStmt(hstmt, SQL_DROP);
     return 0;
   }
   retcode = SQLBindCol(hstmt, 6, SQL_C_CHAR, accdb_release.val, 20, (int*)&accdb_release.len);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[SearchACCDB]: Could not bind column 6.");
	 SQLFreeStmt(hstmt, SQL_DROP);
     return 0;
   }
   retcode = SQLBindCol(hstmt, 7, SQL_C_SLONG, &accdb_version.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[SearchACCDB]: Could not bind column 7.");
	 SQLFreeStmt(hstmt, SQL_DROP);
     return 0;
   }
   retcode = SQLBindCol(hstmt, 8, SQL_C_CHAR, accdb_title.val, TEXT_LENGTH, (int*)&accdb_title.len);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[SearchACCDB]: Could not bind column 8.");
	 SQLFreeStmt(hstmt, SQL_DROP);
     return 0;
   }

   retcode = SQLBindCol(hstmt, 9, SQL_C_CHAR, accdb_namelow.val, 30, (int*)&accdb_namelow.len);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[SearchACCDB]: Could not bind column 9.");
	 SQLFreeStmt(hstmt, SQL_DROP);
     return 0;
   }

   /* fetch results and save in your linked list */
   SQLINTEGER fetchCount = 0;
   /* fetch result and save in the linked list */
   while((retcode=SQLFetch(hstmt)) == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
   {
      fetchCount++;
      if (fetchCount>1)
      {
         pac->next = StAccdbNew();
         pac = pac->next;
      }

      pac->gi = accdb_gi.val;
      pac->db = StringSave((char*)accdb_db.val);
      pac->name = StringSave((char*)accdb_name.val);
      pac->namelow = StringSave((char*)accdb_namelow.val);
      pac->access = StringSave((char*)accdb_access.val);
      pac->chain = StringSave((char*)accdb_chain.val);
      pac->release = StringSave((char*)accdb_release.val);
      pac->version = accdb_version.val;
      if(accdb_title.len >0 ){
         pac->title = StringSave((char*)accdb_title.val);
      } else{
         pac->title = StringSave("n/a");
      }

   }

   if(SQLFreeStmt(hstmt, SQL_DROP) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[SearchACCDB]: Could not free statement.");
      return 0;
   }

   if (fetchCount == 0)
	  return -1;

   return 1;
}

/* search for a record in ACCDB by accession
   Currently only retrieves gi and version. */
Int2 LIBCALL SearchACCDBByAccess (StAccdbPtr PNTR ppac)
{
  StAccdbPtr pac     = NULL;
  SQLINTEGER retcode = 0;
  Int2       fetchCount = 0;

  if( ppac == NULL || *ppac == NULL || 
	  (*ppac)->access == NULL )
  {

      ErrPostEx(SEV_ERROR,0,0, "SearchACCDBByAccess: Bad accession.");
      return -1;
  }

  pac = *ppac;

  sprintf( (char *) accdb_access.val, "%s", pac->access );

  /* execute the select statement */
  retcode = SQLExecute(HNDLAccdbSearchAccess);

  if (retcode != SQL_SUCCESS)
  {
	ErrPostEx(SEV_ERROR,0,0, "[SearchACCDBByAccess]: "
			  "Could not execute select statement. access=%s", pac->access);
	LogSQLInfo(HNDLAccdbSearchAccess);
	return -1;
  }
  
  retcode = SQLBindCol(HNDLAccdbSearchAccess, 1, SQL_C_SLONG, 
					   &accdb_gi.val, 0, NULL);
  if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
  {
	ErrPostEx(SEV_ERROR, 0, 0, "[SearchACCDBByAccess]: "
			  "Could not bind column 1. access=%s", pac->access);
	return -1;
  }

  retcode = SQLBindCol(HNDLAccdbSearchAccess, 2, SQL_C_SLONG, 
					   &accdb_version.val, 0, NULL);
  if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
  {
	ErrPostEx(SEV_ERROR, 0, 0, "[SearchACCDBByAccess]: "
			  "Could not bind column 2. access=%s", pac->access);
	return -1;
  }  

   while((retcode=SQLFetch(HNDLAccdbSearchAccess)) == SQL_SUCCESS ||
		 retcode == SQL_SUCCESS_WITH_INFO)
   {
	 fetchCount++;

	 if( fetchCount > 1 )
	 {
	   pac->next = StAccdbNew();
	   pac = pac->next;
	 }
	 pac->gi = accdb_gi.val;
	 pac->version = accdb_version.val;
   }

   /* close the cursor and discard all pending results. */
   /* must do in order to re-use the handle created in InitACCDB */
   retcode = SQLFreeStmt(HNDLAccdbSearchAccess, SQL_CLOSE);
   if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[SearchAccdbByAccess]: Error in closing cursor.");
      return -1;
   }

   if( fetchCount == 0 )
	 return 0;

   return 1;
}

/* returns a list of StAccdb records found for that name and db. 
   Note that only the gi is retrieved. */
StAccdbPtr LIBCALL SearchACCDBByName( CharPtr pDb, CharPtr pName )
{

  SQLINTEGER retcode = 0;
  Int2       fetchCount = 0;
  StAccdbPtr pGis = StAccdbNew();

  if( pDb == NULL || pName == NULL )
  {
      ErrPostEx(SEV_ERROR,0,0, "SearchACCDBByName: Bad input.");
      return pGis;
  }

  if( !pGis )
  {
	ErrPostEx(SEV_ERROR,0,0, "SearchACCDBByName: Unable to "
			  "allocate StAccdb structure for %s, %s.", pDb,
			  pName );

	return pGis;
  }
  sprintf( (char *) accdb_name.val, "%s", pName );
  sprintf( (char *) accdb_db.val, "%s", pDb );

  /* execute the select statement */
  retcode = SQLExecute(HNDLAccdbGetGiByName);

  if (retcode != SQL_SUCCESS)
  {
	ErrPostEx(SEV_ERROR,0,0, "[SearchACCDBByName]: "
			  "Could not execute select statement. db=%s name=%s",
			  pDb, pName);
	LogSQLInfo(HNDLAccdbGetGiByName);
	return pGis;
  }
  
  retcode = SQLBindCol(HNDLAccdbGetGiByName, 1, SQL_C_SLONG, 
					   &accdb_gi.val, 0, NULL);
  if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
  {
	ErrPostEx(SEV_ERROR, 0, 0, "[SearchACCDBByName]: "
			  "Could not bind column 1. db=%s access=%s",
			  pDb, pName);
	return pGis;
  }

   while((retcode=SQLFetch(HNDLAccdbGetGiByName)) == SQL_SUCCESS ||
		 retcode == SQL_SUCCESS_WITH_INFO)
   {
	 fetchCount++;

	 if( fetchCount > 1 )
	 {
	   pGis->next = StAccdbNew();

	   if( !pGis->next )
	   {
		 ErrPostEx(SEV_ERROR,0,0, "SearchACCDBByName: Unable to "
				   "allocate StAccdb structure for %s, %s.", pDb,
				   pName );
		 return NULL;
	   }	   

	   pGis = pGis->next;
	 }
	 pGis->gi = accdb_gi.val;
   }

   /* close the cursor and discard all pending results. */
   /* must do in order to re-use the handle created in InitACCDB */
   retcode = SQLFreeStmt(HNDLAccdbGetGiByName, SQL_CLOSE);
   if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[SearchACCDBByName]: "
				"Error in closing cursor.");
      return NULL;
   }

   return pGis;

}
/* search for a record in ACCDB by gi */
Int2 LIBCALL SearchACCDBByGi (StAccdbPtr PNTR ppac)
{
  StAccdbPtr pac     = NULL;
  SQLINTEGER retcode = 0;
  Int2       fetchCount = 0;

  if( ppac == NULL || *ppac == NULL || 
	  (*ppac)->gi == NULL )
  {

      ErrPostEx(SEV_ERROR,0,0, "SearchACCDBByGi: Bad gi.");
      return retcode;
  }

  pac = *ppac;

  accdb_gi.val = pac->gi;

  /* execute the select statement */
  retcode = SQLExecute(HNDLAccdbSearchGi);

  if (retcode != SQL_SUCCESS)
  {
	ErrPostEx(SEV_ERROR,0,0, "[SearchACCDBByGi]: "
			  "Could not execute select statement. gi=%d", pac->gi);
	LogSQLInfo(HNDLAccdbSearchGi);
	return retcode;
  }
  
  retcode = SQLBindCol(HNDLAccdbSearchGi, 1, SQL_C_SLONG, 
					   &accdb_gi.val, 0, NULL);
  if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
  {
	ErrPostEx(SEV_ERROR, 0, 0, "[SearchACCDBByGi]: "
			  "Could not bind column 1. gi=%d", pac->gi);
	return 0;
  }

   retcode = SQLBindCol(HNDLAccdbSearchGi, 2, SQL_C_CHAR, accdb_db.val, 15, (int*)&accdb_db.len);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[SearchACCDBByGi]: Could not bind column 2.");
     return FALSE;
   }
   retcode = SQLBindCol(HNDLAccdbSearchGi, 3, SQL_C_CHAR, accdb_name.val, 30, (int*)&accdb_name.len);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[SearchACCDBByGi]: Could not bind column 3.");
     return FALSE;
   }
   retcode = SQLBindCol(HNDLAccdbSearchGi, 4, SQL_C_CHAR, accdb_access.val, 20, (int*)&accdb_access.len);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[SearchACCDBByGi]: Could not bind column 4.");
     return FALSE;
   }
   retcode = SQLBindCol(HNDLAccdbSearchGi, 5, SQL_C_CHAR, accdb_chain.val, 10, (int*)&accdb_chain.len);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[SearchACCDBByGi]: Could not bind column 5.");
     return FALSE;
   }
   retcode = SQLBindCol(HNDLAccdbSearchGi, 6, SQL_C_CHAR, accdb_release.val, 20, (int*)&accdb_release.len);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[SearchACCDBByGi]: Could not bind column 6.");
     return FALSE;
   }
   retcode = SQLBindCol(HNDLAccdbSearchGi, 7, SQL_C_SLONG, &accdb_version.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[SearchACCDBByGi]: Could not bind column 7.");
     return FALSE;
   }
   retcode = SQLBindCol(HNDLAccdbSearchGi, 8, SQL_C_CHAR, accdb_title.val, TEXT_LENGTH, (int*)&accdb_title.len);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[SearchACCDBByGi]: Could not bind column 8.");
     return FALSE;
   }
   retcode = SQLBindCol(HNDLAccdbSearchGi, 9, SQL_C_CHAR, accdb_namelow.val, 30, (int*)&accdb_namelow.len);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[SearchACCDBByGi]: Could not bind column 9.");
     return FALSE;
   }


   while((retcode=SQLFetch(HNDLAccdbSearchGi)) == SQL_SUCCESS ||
		 retcode == SQL_SUCCESS_WITH_INFO)
   {
	 fetchCount++;

	 if( fetchCount > 1 )
	 {
	   pac->next = StAccdbNew();
	   pac = pac->next;
	 }
	 pac->gi = accdb_gi.val;
	 pac->version = accdb_version.val;
	 pac->access = StringSave( ( const char * ) accdb_access.val );
   }

   /* close the cursor and discard all pending results. */
   /* must do in order to re-use the handle created in InitACCDB */
   retcode = SQLFreeStmt(HNDLAccdbSearchGi, SQL_CLOSE);
   if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[SearchACCDBByGi]: Error in closing cursor.");
      return 0;
   }

   if( fetchCount == 0 )
	 return 0;

   return 1;
}


/* search gi in ACCDB for access */
/* similar to SHoundFindAcc in intrez.c */
/* rewrite here in order to speed up the call in a loop in comgen.c */
Int4  LIBCALL SHoundFindGiByAcc(CharPtr pcAcc)
{
   Int4 gi = 0;
   SQLINTEGER retcode = 0;

   if ((pcAcc == NULL) || (strcmp(pcAcc, "n/a") == 0))
     {
       ErrPostEx(SEV_ERROR,0,0, "[SHoundFindGiByAcc]: No accession.");
       return 0;
     }

   sprintf((char*)accdb_access.val, "%s", IsNullStr(pcAcc));

   /* execute the select statement */
   retcode = SQLExecute(HNDLAccdbSearchAccess);
   if (retcode != SQL_SUCCESS)
   {
	  ErrPostEx(SEV_ERROR,0,0, "[SHoundFindGiByAcc]: Could not execute select statement. access=%s", pcAcc);
	  LogSQLInfo(HNDLAccdbSearchAccess);
      return 0;
   }

   retcode = SQLBindCol(HNDLAccdbSearchAccess, 1, SQL_C_SLONG, &accdb_gi.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[SHoundFindGiByAcc]: Could not bind column 1. access=%s", pcAcc);
     return 0;
   }

   while((retcode=SQLFetch(HNDLAccdbSearchAccess)) == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
   {
	   gi = accdb_gi.val;
   }

   /* close the cursor and discard all pending results. */
   /* must do in order to re-use the handle created in InitACCDB */
   retcode = SQLFreeStmt(HNDLAccdbSearchAccess, SQL_CLOSE);
   if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[HNDLAccdbSearchAccess]: Error in closing cursor.");
      return 0;
   }

   return gi;
}
/* SHoundFindAccessByGi - searches Accdb by gi and returns the accession. 
   Only returns accessions for the following databases: genbank, refseq 
   swiss-prot, dbj. These are the only ones that have accessions in accdb. */
CharPtr  LIBCALL SHoundFindAccessByGi( Int4 gi)
{
   CharPtr access = NULL;
   SQLINTEGER retcode = 0;

   if ( gi <= 0)
   {
	 ErrPostEx(SEV_ERROR,0,0, "[SHoundFindAccessByGi]: No accession.");
	 return access;
   }

   accdb_gi.val = gi;

   /* execute the select statement */
   retcode = SQLExecute(HNDLAccdbGetAccessByGi);
   if (retcode != SQL_SUCCESS)
   {
	  ErrPostEx(SEV_ERROR,0,0, "[SHoundFindAccessByGi]: "
				"Could not execute select statement. gi = %d", gi);
	  LogSQLInfo(HNDLAccdbGetAccessByGi);
      return access;
   }

   retcode = SQLBindCol(HNDLAccdbGetAccessByGi, 1, SQL_C_CHAR, 
						accdb_access.val, 20, (int*)&accdb_access.len);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[SHoundFindAccessByGi]: "
			   "Could not bind column 1.");
     return access;
   }

   retcode = SQLBindCol(HNDLAccdbGetAccessByGi, 2, SQL_C_CHAR, 
						accdb_db.val, 15, (int*)&accdb_db.len);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[SHoundFindAccessByGi]: "
			   "Could not bind column 2.");
     return access;
   }

   while((retcode=SQLFetch(HNDLAccdbGetAccessByGi)) == SQL_SUCCESS || 
		 retcode == SQL_SUCCESS_WITH_INFO)
   {
	 if( strcmp( (const char *) accdb_db.val, "gb" ) == 0 ||
		 strcmp( (const char *) accdb_db.val, "ref" ) == 0 ||
		 strcmp( (const char *) accdb_db.val, "sp" ) == 0 ||
		 strcmp( (const char *) accdb_db.val, "dbj" ) == 0 )

	 {
	   access = StringSave( (const char *)accdb_access.val );
	   break;
	 }
   }

   /* close the cursor and discard all pending results. */
   /* must do in order to re-use the handle created in InitACCDB */
   retcode = SQLFreeStmt(HNDLAccdbGetAccessByGi, SQL_CLOSE);
   if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[HNDLAccdbGetAccessByGi]: Error in closing cursor.");
      return NULL;
   }

   return access;
}


/* returns a record based on record number */
StAccdbPtr LIBCALL GetRecACCDB(Int4 recno)
{
   StAccdbPtr pac = StAccdbNew();        /* linked list to hold return values */
   SQLRETURN retcode;                    /* return values for odbc layer */

   if (recno < 1 || recno > GetRecNoACCDB()) {
      ErrPostEx(SEV_ERROR, 0, 0, "[GetRecACCDB]: Invalid record number %ld", recno);
      return NULL;
   }

   rowid.val = recno;

   /* execute the select statement */
   retcode = SQLExecute(HNDLAccdbSearchRowid);
   if (retcode != SQL_SUCCESS)
   {
	  ErrPostEx(SEV_ERROR,0,0, "[GetRecACCDB]: Could not execute select statement.");
	  LogSQLInfo(HNDLAccdbSearchRowid);
      return NULL;
   }

   /* binding our results from the database */

   retcode = SQLBindCol(HNDLAccdbSearchRowid, 1, SQL_C_SLONG, &accdb_gi.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[GetRecACCDB]: Could not bind column 1.");
     return NULL;
   }
   retcode = SQLBindCol(HNDLAccdbSearchRowid, 2, SQL_C_CHAR, accdb_db.val, 15, (int*)&accdb_db.len);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[GetRecACCDB]: Could not bind column 2.");
     return NULL;
   }
   retcode = SQLBindCol(HNDLAccdbSearchRowid, 3, SQL_C_CHAR, accdb_name.val, 30, (int*)&accdb_name.len);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[GetRecACCDB]: Could not bind column 3.");
     return NULL;
   }
   retcode = SQLBindCol(HNDLAccdbSearchRowid, 4, SQL_C_CHAR, accdb_access.val, 20, (int*)&accdb_access.len);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[GetRecACCDB]: Could not bind column 4.");
     return NULL;
   }
   retcode = SQLBindCol(HNDLAccdbSearchRowid, 5, SQL_C_CHAR, accdb_chain.val, 10, (int*)&accdb_chain.len);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[GetRecACCDB]: Could not bind column 5.");
     return NULL;
   }
   retcode = SQLBindCol(HNDLAccdbSearchRowid, 6, SQL_C_CHAR, accdb_release.val, 20, (int*)&accdb_release.len);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[GetRecACCDB]: Could not bind column 6.");
     return NULL;
   }
   retcode = SQLBindCol(HNDLAccdbSearchRowid, 7, SQL_C_SLONG, &accdb_version.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[GetRecACCDB]: Could not bind column 7.");
     return NULL;
   }
   retcode = SQLBindCol(HNDLAccdbSearchRowid, 8, SQL_C_CHAR, accdb_title.val, TEXT_LENGTH, (int*)&accdb_title.len);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[GetRecACCDB]: Could not bind column 8.");
     return NULL;
   }
   retcode = SQLBindCol(HNDLAccdbSearchRowid, 9, SQL_C_CHAR, accdb_namelow.val, 30, (int*)&accdb_namelow.len);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[GetRecACCDB]: Could not bind column 9.");
     return NULL;
   }

   /* fetch results and save in your linked list */
   SQLINTEGER fetchCount = 0;
   /* fetch result and save in the linked list */
   while((retcode=SQLFetch(HNDLAccdbSearchRowid)) == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
   {
      fetchCount++;
      if (fetchCount>1)
      {
         pac->next = StAccdbNew();
         pac = pac->next;
      }

      pac->gi = accdb_gi.val;
      pac->db = StringSave((char*)accdb_db.val);
      pac->name = StringSave((char*)accdb_name.val);
      pac->namelow = StringSave((char*)accdb_namelow.val);
      pac->access = StringSave((char*)accdb_access.val);
      pac->chain = StringSave((char*)accdb_chain.val);
      pac->release = StringSave((char*)accdb_release.val);
      pac->version = accdb_version.val;
      if(accdb_title.len >0 ){
         pac->title = StringSave((char*)accdb_title.val);
      } else{
         pac->title = StringSave("n/a");
      }

   }

   /* close the cursor and discard all pending results. */
   /* must do in order to re-use the handle created in InitACCDB */
   retcode = SQLFreeStmt(HNDLAccdbSearchRowid, SQL_CLOSE);
   if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[GetRecACCDB]: Error in closing cursor.");
      return NULL;
   }

   if (fetchCount == 0)
	  return NULL;

   return pac;
}


/*******************************************************/
/*Given a pointer to the asn structure that represents */
/*  a nucprot record this function pulls out either the*/
/*  nucleotide gi or the amino acid gi depending on    */
/*  which one is populated.  This function then goes   */
/*  into the database and pulls out all the nucleotide */
/*  protein relations for that gi.                     */
/*Returns : a linked list of the nucprot records that  */
/*  were retrieved from the database.                  */
/*******************************************************/
Int2 LIBCALL SearchNUCPROT (StNucprotPtr PNTR ppnc)
{
   StNucprotPtr pnc = NULL;
   SQLHANDLE hstmt;
   SQLRETURN retcode;
   Char *pSQL_stmt;
   SQLINTEGER iKey = 0;

   if (ppnc == NULL)
   {
     ErrPostEx(SEV_ERROR,0,0, "[SearchNUCPROT]: StNucprotPtr is NULL.");
     return 0;
   }
   pnc = *ppnc;

   pSQL_stmt = (char*)malloc(sizeof(char) * 150);
   if(pSQL_stmt == NULL)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[SearchNUCPROT]: Could not allocate space.");
      return 0;
   }

   if(pnc->gin > 0)
   {
      iKey = pnc->gin;
      sprintf((char*)pSQL_stmt, "select gia, gin from %s.nucprot where gin = ?;", database);
   }else if (pnc->gia > 0) {
      iKey = pnc->gia;
      sprintf((char*)pSQL_stmt, "select gia, gin from %s.nucprot where gia = ?;", database);
   }else{
      ErrPostEx(SEV_ERROR, 0, 0, "[SearchNUCPROT]: Invalid search key.");
      free(pSQL_stmt);
      return 0;
   }

   if(CreateHandle(&hstmt, pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[SearchNUCPROT]: Could not allocate handle.");
      free(pSQL_stmt);
      return 0;
   }
   free(pSQL_stmt);

   retcode = SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &iKey, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[SearchNUCPROT]: Could not bind parameter.");
      SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
      return 0;
   }

   /* execute statment */
   retcode = SQLExecute(hstmt);
   if (retcode != SQL_SUCCESS)
   {
	  ErrPostEx(SEV_ERROR,0,0, "[SearchNUCPROT]: Unable to execute select statement.");
	  LogSQLInfo(hstmt);
      SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
      return 0;
   }

   retcode = SQLBindCol(hstmt, 1, SQL_C_SLONG, &nucprot_gia.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[SearchNUCPROT]: Could not bind column 1.");
      SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
      return 0;
   }
   retcode = SQLBindCol(hstmt, 2, SQL_C_SLONG, &nucprot_gin.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[SearchNUCPROT]: Could not bind column 2.");
      SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
      return 0;
   }

   SQLINTEGER fetchCount = 0;
   while((retcode=SQLFetch(hstmt)) == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
   {
      fetchCount++;

      if(fetchCount>1)
      {
         pnc->next = StNucprotNew();
         pnc = pnc->next;
      }

      pnc->gin = nucprot_gin.val;
      pnc->gia = nucprot_gia.val;
   }

   /* Free statement handle. */
   if (SQLFreeHandle(SQL_HANDLE_STMT, hstmt) != SQL_SUCCESS){
     ErrPostEx(SEV_ERROR, 0, 0, "[SearchNUCPROT]: Could not free statement.");
     return 0;
   }

   if (fetchCount == 0)
	  return -1;

   return 1;

}

ValNodePtr LIBCALL SearchTAXGIDNA(SQLINTEGER taxid)
{
   SQLHANDLE hstmt;
   Int2 sqlrc=SQL_SUCCESS;
   ValNodePtr vnp=NULL;

   CharPtr search_taxgi = (char *) malloc(sizeof(Char) * 150);
   if (search_taxgi == NULL)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[SearchTAXGIDNA]: Could not allocate space");
     free(search_taxgi);
     return FALSE;
   }
   sprintf((char *)search_taxgi, "SELECT gi from %s.taxgi where taxid=? and (type=\'DNA\' or type=\'RNA\')", database);

   struct{
     SQLINTEGER len;
     SQLINTEGER val;
   }gi;

   if (taxid <= 0)
    {
       ErrPostEx(SEV_ERROR,0,0, "SearchTAXGIDNA: No allowed search key.");
       return NULL;
    }

  if(CreateHandle(&hstmt,search_taxgi, hdbc, &henv) != SQL_SUCCESS)
  {
    ErrPostEx(SEV_ERROR, 0, 0, "[SearchTAXGIDNA]: Could not create handle.");
    return NULL;
  }

  sqlrc=SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_SLONG,SQL_INTEGER, 0, 0, &taxid, 0, NULL);
  if (sqlrc != SQL_SUCCESS && sqlrc != SQL_SUCCESS_WITH_INFO)
  {
    ErrPostEx(SEV_ERROR,0,0,"SearchTAXGIDNA: unable to execute bind parameter 1.");
    SQLFreeStmt(hstmt, SQL_DROP);
    return NULL;
  }

  /* execute statment */
  sqlrc=SQLExecute(hstmt);
   if (sqlrc != SQL_SUCCESS)
   {
	  ErrPostEx(SEV_ERROR,0,0, "SearchTAXGIDNA: unable to execute select statement.");
	  LogSQLInfo(hstmt);
      SQLFreeStmt(hstmt, SQL_DROP);
      return NULL;
   }

  sqlrc=SQLBindCol(hstmt,1,SQL_C_SLONG,&gi.val,0,(int*)&gi.len);
  if (sqlrc != SQL_SUCCESS && sqlrc != SQL_SUCCESS_WITH_INFO)
  {
    ErrPostEx(SEV_ERROR,0,0,"SearchTAXGIDNA: unable to execute bind column 1.");
    SQLFreeStmt(hstmt, SQL_DROP);
    return NULL;
  }

   /* fetch the data in columns iteratively */
   SQLINTEGER fetchCount = 0;
   /* fetch result and save in the linked list */
   while((sqlrc=SQLFetch(hstmt)) == SQL_SUCCESS || sqlrc == SQL_SUCCESS_WITH_INFO)
   {
      fetchCount++;
      Slri_ValNodeAddInt(&vnp,1,gi.val);
   }

  if(SQLFreeStmt(hstmt, SQL_DROP) != SQL_SUCCESS)
  {
     ErrPostEx(SEV_INFO, 0 ,0, "SearchTAXGIDNA : unable to free statement.");
     print_err(hstmt);
     return NULL;
   }

  if (fetchCount == 0) {
    ErrPostEx(SEV_INFO, 0, 0, "SearchTAXGIDNA No data found for taxid : %ld",taxid);
	return NULL;
  }

   return vnp;
}

ValNodePtr LIBCALL SearchTAXGIProt(SQLINTEGER taxid)
{
   SQLHANDLE hstmt;
   Int2 sqlrc=SQL_SUCCESS;
   ValNodePtr vnp=NULL;

   CharPtr search_taxgi = (char *) malloc(sizeof(Char) * 150);
   if (search_taxgi == NULL)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[SearchTAXGIProt]: Could not allocate space");
     free(search_taxgi);
     return FALSE;
   }
   sprintf((char *)search_taxgi, "SELECT gi from %s.taxgi where taxid=? and type=\'protein\'", database);

   struct{
     SQLINTEGER len;
     SQLINTEGER val;
   }gi;

   if (taxid <= 0)
    {
       ErrPostEx(SEV_ERROR,0,0, "SearchTAXGIProt: No allowed search key.");
       return NULL;
    }

  sqlrc = CreateHandle(&hstmt,search_taxgi, hdbc, &henv);
  if (sqlrc != SQL_SUCCESS && sqlrc != SQL_SUCCESS_WITH_INFO)
  {
    ErrPostEx(SEV_ERROR,0,0,"SearchTAXGIProt: Could not create handle.");
    SQLFreeStmt(hstmt, SQL_DROP);
    return NULL;
  }

  sqlrc = SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_SLONG,SQL_INTEGER, 0, 0, &taxid, 0, NULL);
  if (sqlrc != SQL_SUCCESS && sqlrc != SQL_SUCCESS_WITH_INFO)
  {
    ErrPostEx(SEV_ERROR,0,0,"SearchTAXGIProt: unable to execute bind parameter 1.");
    SQLFreeStmt(hstmt, SQL_DROP);
    return NULL;
  }

  /* execute statment */
  sqlrc = SQLExecute(hstmt);
   if (sqlrc != SQL_SUCCESS)
   {
	  ErrPostEx(SEV_ERROR,0,0, "SearchTAXGIProt: unable to execute select statement.");
	  LogSQLInfo(hstmt);
      SQLFreeStmt(hstmt, SQL_DROP);
      return NULL;
   }

  sqlrc = SQLBindCol(hstmt,1,SQL_C_SLONG,&gi.val,0,(int*)&gi.len);
  if (sqlrc != SQL_SUCCESS && sqlrc != SQL_SUCCESS_WITH_INFO)
  {
    ErrPostEx(SEV_ERROR,0,0,"SearchTAXGIProt: unable to execute bind column 1.");
    SQLFreeStmt(hstmt, SQL_DROP);
    return NULL;
  }

   /* fetch the data in columns iteratively */
   SQLINTEGER fetchCount = 0;
   /* fetch result and save in the linked list */
   while((sqlrc=SQLFetch(hstmt)) == SQL_SUCCESS || sqlrc == SQL_SUCCESS_WITH_INFO)
   {
      fetchCount++;
      Slri_ValNodeAddInt(&vnp,1,gi.val);
   }

  if(SQLFreeStmt(hstmt, SQL_DROP) != SQL_SUCCESS)
  {
     ErrPostEx(SEV_INFO, 0 ,0, "SearchTAXGIProt : unable to free statement.");
     print_err(hstmt);
     return NULL;
   }

  if (fetchCount == 0) {
    ErrPostEx(SEV_INFO, 0, 0, "SearchTAXGIProt No data found for taxid : %ld",taxid);
	return NULL;
  }

   return vnp;
}

ValNodePtr LIBCALL SearchTAXGIByKloodgeForNA(Int4 kloodge)
{
  Int2       retcode    = SQL_SUCCESS;
  ValNodePtr pvnGis     = NULL;
  Int4       fetchCount = 0;

  if( kloodge == 0 )
  {
	ErrPostEx(SEV_ERROR,0,0, "SearchTAXGIByKloodgeForNA: "
			  "Bad kloodge %d", kloodge);
	return NULL;
  }

  taxgi_kloodge.val = kloodge;

  retcode = SQLExecute(HNDLTaxgiSearchKloodgeNA);

  if (retcode != SQL_SUCCESS  && retcode != SQL_SUCCESS_WITH_INFO)
  {
	ErrPostEx(SEV_ERROR,0,0, "[SearchTaxgiByKloodgeForNA]: "
			  "unable to execute search statement. (kloodge=%d)", kloodge);
	LogSQLInfo(HNDLTaxgiSearchKloodgeNA);
	return NULL;
  }

  retcode = SQLBindCol(HNDLTaxgiSearchKloodgeNA, 1, SQL_C_SLONG, 
					   &taxgi_gi.val, 0, NULL);

  if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
  {
	ErrPostEx(SEV_ERROR, 0, 0, "[SearchTaxgiByKloodgeForNA]: "
			  "Could not bind column 1.");
	return NULL;
  }

  while((retcode=SQLFetch(HNDLTaxgiSearchKloodgeNA)) == SQL_SUCCESS || 
		 retcode == SQL_SUCCESS_WITH_INFO)
  {
	fetchCount++;

	pvnGis = Slri_ValNodeAddInt( &pvnGis, 1, taxgi_gi.val );
  }

   /* close the cursor and discard all pending results. */
   /* must do in order to re-use the handle created in InitTaxgi */
  retcode = SQLFreeStmt(HNDLTaxgiSearchKloodgeNA, SQL_CLOSE);
  if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
  {
	ErrPostEx(SEV_ERROR, 0, 0, "[SearchTaxgiByKloodgeForNA]: "
			  "Error in closing cursor.");
	return NULL;
  }

  if (fetchCount == 0)
  {
    ErrPostEx(SEV_INFO, 0, 0, "SearchTAXGIByKloodgeForNA: "
			  "No data found for kloodge: %ld", kloodge);
	return NULL;
  }

  return pvnGis;

  
}

ValNodePtr LIBCALL SearchTAXGIByKloodgeForProt(Int4 kloodge)
{
  Int2       retcode    = SQL_SUCCESS;
  ValNodePtr pvnGis     = NULL;
  Int4       fetchCount = 0;

  if( kloodge == 0 )
  {
	ErrPostEx(SEV_ERROR,0,0, "SearchTAXGIByKloodgeForProt: "
			  "Bad kloodge %d", kloodge);
	return NULL;
  }

  taxgi_kloodge.val = kloodge;

  retcode = SQLExecute(HNDLTaxgiSearchKloodgeAA);

  if (retcode != SQL_SUCCESS  && retcode != SQL_SUCCESS_WITH_INFO)
  {
	ErrPostEx(SEV_ERROR,0,0, "[SearchTaxgiByKloodgeForProt]: "
			  "unable to execute search statement. (kloodge=%d)", kloodge);
	LogSQLInfo(HNDLTaxgiSearchKloodgeAA);
	return NULL;
  }

  retcode = SQLBindCol(HNDLTaxgiSearchKloodgeAA, 1, SQL_C_SLONG, 
					   &taxgi_gi.val, 0, NULL);

  if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
  {
	ErrPostEx(SEV_ERROR, 0, 0, "[SearchTaxgiByKloodgeForProt]: "
			  "Could not bind column 1.");
	return NULL;
  }

  while((retcode=SQLFetch(HNDLTaxgiSearchKloodgeAA)) == SQL_SUCCESS || 
		 retcode == SQL_SUCCESS_WITH_INFO)
  {
	fetchCount++;

	pvnGis = Slri_ValNodeAddInt( &pvnGis, 1, taxgi_gi.val );
  }

   /* close the cursor and discard all pending results. */
   /* must do in order to re-use the handle created in InitTaxgi */
  retcode = SQLFreeStmt(HNDLTaxgiSearchKloodgeAA, SQL_CLOSE);

  if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
  {
	ErrPostEx(SEV_ERROR, 0, 0, "[SearchTaxgiByKloodgeForProt]: "
			  "Error in closing cursor.");
	return NULL;
  }

  if (fetchCount == 0)
  {
    ErrPostEx(SEV_INFO, 0, 0, "SearchTAXGIByKloodgeForProt: "
			  "No data found for kloodge: %ld", kloodge);
	return NULL;
  }

  return pvnGis;

}

Boolean LIBCALL AppendRecordSOURCEFILE( StSourcefilePtr pRecord )
{
  SQLRETURN retcode;
   
  if(pRecord == NULL)
  {
	ErrPostEx(SEV_ERROR,0,0, "[AppendRecordSOURCEFILE]: NULL parameter.");
	return FALSE;
  }

  sourcefile_gi.val      = pRecord->gi;
  sprintf( (char *)sourcefile_filename.val, "%s",
		   IsNullStr(pRecord->filename) );


  /* execute statment */
  retcode = SQLExecute(HNDLSourcefileInsert);
  if (retcode != SQL_SUCCESS)
  {
	ErrPostEx(SEV_ERROR,0,0, "[AppendRecordSOURCEFILE]: "
			  "unable to execute insert statement. (gi=%ld)",
			  sourcefile_gi.val);
	LogSQLInfo(HNDLSourcefileInsert);
	return FALSE;
  }

  return TRUE;
}

Boolean LIBCALL OpenMMDB(CharPtr name, Boolean Create){ return TRUE;}


Boolean LIBCALL AppendRecordMMDB(StMmdbPtr pmd) {

   if(pmd == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "[AppendRecordMMDB]: NULL parameter.");
      return FALSE;
    }

   SQLRETURN retcode;
   pMBL pMBLtmp = NULL;

   mmdb_mmdbid.val = pmd->mmdbid;
   mmdb_bwhat.val = pmd->bwhat;
   mmdb_models.val = pmd->models;
   mmdb_molecules.val = pmd->molecules;
   sprintf((char *)mmdb_pdbid.val,"%s", IsNullStr(pmd->pdbid));


   /* get the asn1 bioseq into a bytestore pointer so that we can put it into a buffer. */
   if (pmd->asn1 != NULL) {
      pMBLtmp = (pMBL)AssignASNMemBZChar((Pointer) pmd->asn1, (AsnWriteFunc) BiostrucAsnWrite);
	  if (pMBLtmp == NULL) {
		  ErrPostEx(SEV_ERROR, 0, 0, "AssignASNMemBZChar failed for mmdbid=%ld.", mmdb_mmdbid.val);
		  ClearStructures();
		  return FALSE;
	  }
      mmdb_asn.val = pMBLtmp->buf;
      mmdb_asn.len = pMBLtmp->bzlen;    /* asn1 stores compressed info. */
	  mmdb_size.val = pMBLtmp->bslen;
	  mmdb_bzsize.val = pMBLtmp->bzlen;
   } else {
      mmdb_asn.val = NULL;
      mmdb_asn.len = 0;
	  mmdb_size.val = 0;
	  mmdb_bzsize.val = 0;
   }

   /* set up the input parameter. We bind the blob here because its size
    * changes for each blob, so we must individually bind it here. */
   retcode = SQLBindParameter(HNDLMmdb, 2, SQL_PARAM_INPUT, SQL_C_DEFAULT, SQL_LONGVARBINARY, 0, 0, mmdb_asn.val, 0, (int*)&mmdb_asn.len);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO) {
     ErrPostEx(SEV_ERROR, 0, 0, "MMDB: Could not bind parameter 2");
     MemFree(pMBLtmp->buf);
     pMBLtmp->buf = NULL;
     return FALSE;
   }

  /* execute statment */
   retcode = SQLExecute(HNDLMmdb);
   if (retcode != SQL_SUCCESS)
   {
	  ErrPostEx(SEV_ERROR,0,0, "[AppendRecordMMDB]: unable to execute MMDB insert statement. (mmid=%ld)",mmdb_mmdbid.val);
	  LogSQLInfo(HNDLMmdb);
	  MemFree(pMBLtmp->buf);
     pMBLtmp->buf = NULL;
      return FALSE;
   }

   /*----------------------------------------------------------
     Free either mmdb_asn.val or pMBLtmp->buf, but not both as
     they point to the same thing. Bug 2801.
     ----------------------------------------------------------*/
   MemFree(pMBLtmp->buf);
     pMBLtmp->buf = NULL;

	return TRUE;
}


BiostrucPtr GetBiostrucfromMMDB(CharPtr pcASNbz, Int4 iSizebz, Int4 iSize, Int4 mdlLvl, Int4 maxModels)
{
	AsnIoMemPtr aimp = NULL;
	BiostrucPtr pASNdata=NULL;
	CharPtr pcASN1=NULL;

	if (pcASNbz == NULL) {
		ErrPostEx(SEV_ERROR, 0, 0, "GetASNBZMemChar: pf4Field is NULL.");
		return NULL;
	}

	/*get length of the memo field*/
	if((iSize<=0) || (iSizebz <=0)) {
	  ErrPostEx(SEV_ERROR, 0, 0, "GetASNBZMemChar: Length must be a positive number.");
	  return(NULL);
	}

   pcASN1 = (CharPtr) MemNew((size_t) iSize + 1);

   if (BZ2_bzBuffToBuffDecompress(pcASN1, (uint*)&iSize, pcASNbz,iSizebz,0,0)!=BZ_OK)
    {
       ErrPostEx(SEV_ERROR,0,0, "GetASNBZMemChar: bzBuffToBuffDecompress failed.");
       if (pcASN1) MemFree(pcASNbz);
       return NULL;
    }


	/*open the asn io stream*/
	aimp = AsnIoMemOpen(GetASNReadMode(), (BytePtr) pcASN1, iSize);
	if (aimp == NULL) {
		ErrPostEx(SEV_ERROR, 0, 0, "GetASNMemMemo: AsnIoMemPtr is NULL.");
		return NULL;
	}

	/*read the asn.1 into a data structure*/
	pASNdata = BiostrucAsnGet(aimp->aip, NULL, mdlLvl, maxModels);
	if (pASNdata == NULL)
	  {
	    ErrPostEx(SEV_ERROR,0,0, "GetASNBZMemChar: pASNdata pointer is NULL.");
	    if (pASNdata) MemFree(pASNdata);
	    AsnIoMemClose(aimp);
	    return NULL;
	  }

	AsnIoFlush(aimp->aip);

	/*clean up*/
    AsnIoMemClose(aimp);
    if (pcASN1) MemFree(pcASN1);

	/*return a pointer to the data structure*/
	return(pASNdata);
}

/*******************************************************/
/*Given a pointer to the asn structure that represents */
/*  a mmdb record this function pulls out the          */
/*  taxid from the asn structure and                   */
/*  retrieves all the records associated with that key */
/*Returns : 1 if successful, 0 if fails, -1 if no data */
/*******************************************************/

Int2 LIBCALL SearchMMDBEx (StMmdbPtr PNTR ppmd, Int4 mdlLvl, Int4 maxModels)
{
   StMmdbPtr pmd = NULL;                /* linked list to hold return values */
   SQLINTEGER iKey = 0, iLength = 0;           /* search key and length of key */
   CharPtr pKey = NULL;                  /* search key for strings */
   Char *pSQL_stmt;                      /* SQL select statement */
   SQLHANDLE hstmt, LocUse;              /* handle to select statement */
   SQLRETURN retcode;                    /* return values for odbc layer */

   Int4 bloblen = 0, indicator = 0, pbbvalue = 0;
   SQLINTEGER blobloc = 0;
   CharPtr pcASN = NULL;

   if (ppmd == NULL)
   {
       ErrPostEx(SEV_ERROR,0,0, "[SearchMMDB]: StMmdbPtr is NULL.");
       return 0;
   }
   pmd = *ppmd;

   /* allocate space for the select statement */
   pSQL_stmt = (char*) malloc(sizeof(char) * 150);
   if(pSQL_stmt == NULL)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[SearchMMDB]: Could not allocate space");
      return 0;
   }
   /* fill in the select statement based on the search key passed in.
      database is a global variable initialized in ConnectToDb. The
      value of database is set in the ini file. eg database = seqhound. */
   if (pmd->mmdbid > 0)
   {
      iKey = pmd->mmdbid;
      sprintf((char*)pSQL_stmt, "select mmdbid, asn1, pdbid, bwhat, models, molecules, size, bzsize from %s.mmdb where mmdbid=?;", database);
   }
   else if (pmd->pdbid != NULL) {
	  pKey = pmd->pdbid;
      sprintf((char*)pSQL_stmt, "select mmdbid, asn1, pdbid, bwhat, models, molecules, size, bzsize from %s.mmdb where pdbid=?;", database);
   }
   else{
      ErrPostEx(SEV_ERROR, 0, 0, "[SearchMMDB]: No search key found.");
      free(pSQL_stmt);
      return 0;
   }

   /* create handle to statement, once handle exists we can free char* */
   if(CreateHandle(&hstmt, pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[SearchMMDB]: Could not create handle.");
      free(pSQL_stmt);
      return 0;
   }
   free(pSQL_stmt);


   /* bind the search key */
   if (iKey > 0)
      retcode = SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &iKey, 0, NULL);
   else
      retcode = SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_DEFAULT, SQL_VARCHAR, 20, 0, pKey, 0, NULL);

   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[SearchMMDB]: Could not bind parameter");
	  SQLFreeStmt(hstmt, SQL_DROP);
      return 0;
   }
   /* execute the select statement */
   retcode = SQLExecute(hstmt);
   if (retcode != SQL_SUCCESS)
   {
	  ErrPostEx(SEV_ERROR,0,0, "[SearchMMDB]: Could not execute select statement.");
	  LogSQLInfo(hstmt);
      SQLFreeStmt(hstmt, SQL_DROP);
      return 0;
   }

   /* binding our results from the database */

   retcode = SQLBindCol(hstmt, 1, SQL_C_SLONG, &mmdb_mmdbid.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[SearchMMDB]: Could not bind column 1.");
	 SQLFreeStmt(hstmt, SQL_DROP);
     return 0;
   }
   retcode = SQLBindCol(hstmt, 2, SQL_C_BINARY, &blobloc, 0, (int*)&pbbvalue);
   if((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO))
   {
      ErrPostEx(SEV_ERROR,0,0, "[SearchMMDB]: Binding Column 2 failed.");
	  SQLFreeStmt(hstmt, SQL_DROP);
      return 0;
   }
   retcode = SQLBindCol(hstmt, 3, SQL_C_CHAR, mmdb_pdbid.val, 20, (int*)&mmdb_pdbid.len);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[SearchMMDB]: Could not bind column 3.");
	 SQLFreeStmt(hstmt, SQL_DROP);
     return 0;
   }
   retcode = SQLBindCol(hstmt, 4, SQL_C_SLONG, &mmdb_bwhat.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[SearchMMDB]: Could not bind column 4.");
	 SQLFreeStmt(hstmt, SQL_DROP);
     return 0;
   }
   retcode = SQLBindCol(hstmt, 5, SQL_C_SLONG, &mmdb_models.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[SearchMMDB]: Could not bind column 5.");
	 SQLFreeStmt(hstmt, SQL_DROP);
     return 0;
   }
   retcode = SQLBindCol(hstmt, 6, SQL_C_SLONG, &mmdb_molecules.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[SearchMMDB]: Could not bind column 6.");
	 SQLFreeStmt(hstmt, SQL_DROP);
     return 0;
   }
   retcode = SQLBindCol(hstmt, 7, SQL_C_SLONG, &mmdb_size.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[SearchMMDB]: Could not bind column 7.");
	 SQLFreeStmt(hstmt, SQL_DROP);
     return 0;
   }
   retcode = SQLBindCol(hstmt, 8, SQL_C_SLONG, &mmdb_bzsize.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[SearchMMDB]: Could not bind column 8.");
	 SQLFreeStmt(hstmt, SQL_DROP);
     return 0;
   }

   /* fetch results and save in your linked list */
   SQLINTEGER fetchCount = 0;
   /* fetch result and save in the linked list */
   while((retcode=SQLFetch(hstmt)) == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
   {
      fetchCount++;
      if (fetchCount>1)
      {
         pmd->next = StMmdbNew();
         pmd = pmd->next;
      }

      pmd->mmdbid = mmdb_mmdbid.val;
      pmd->bwhat = mmdb_bwhat.val;
      pmd->models = mmdb_models.val;
      pmd->molecules = mmdb_molecules.val;
      pmd->size = mmdb_size.val;
      pmd->bzsize = mmdb_bzsize.val;

      if(mmdb_pdbid.len >0 ){
         pmd->pdbid = StringSave((char*)mmdb_pdbid.val);
      } else{
         pmd->pdbid = NULL;
      }


	  /*Get the length of of the blob so that we can read it */
	  SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &LocUse);
	  bloblen = (Int4)pbbvalue;

	  if(bloblen > 0)
	  {
	      /* allocate a buffer big enough to store the blob. */
	      pcASN = (CharPtr) MemNew((size_t) bloblen);

	      /*read the blob from the database */
	      /* RAC put an if Statement here if (SQLGetData(...) != SQL_SUCCESS) --->> print_err(hstmt) */
	      SQLGetData(hstmt, 2, SQL_C_BINARY, (SQLPOINTER)pcASN, bloblen, (int*)&indicator);

	      if(pcASN != NULL)
	      {
			pmd->asn1 = GetBiostrucfromMMDB(pcASN,pmd->bzsize,pmd->size, mdlLvl, maxModels);
	      }
	      if (pcASN) { MemFree(pcASN); }

	  }
	  else
	  {
	      ErrPostEx(SEV_INFO, 0, 0, "[SearchMMDB] ASN1 Blob retrieved has length = 0. gi=%ld", iKey);
	  }

	  if (SQLFreeHandle(SQL_HANDLE_STMT, LocUse) != SQL_SUCCESS)
	  {
	      ErrPostEx(SEV_INFO, 0, 0, "[SearchMMDB]: Unable to free handle");
	      SQLFreeStmt(hstmt, SQL_DROP);
	      return 0;
	  }

   }

   if(SQLFreeStmt(hstmt, SQL_DROP) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[SearchMMDB]: Could not free statement.");
      return 0;
   }

   if (fetchCount == 0)
	  return -1;

   return 1;
}

Int2 LIBCALL SearchMMDB (StMmdbPtr PNTR ppmd)
{
	return SearchMMDBEx(ppmd, ALLMDL, 100);
}

ValNodePtr LIBCALL SHoundAllMMDBID(void)
{
   ValNodePtr pvnMd = NULL;
   SQLRETURN sqlrc=SQL_SUCCESS;
   SQLHANDLE hstmt;

   CharPtr search_mmdb = (char *) malloc(sizeof(Char) * 150);
   if (search_mmdb == NULL)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[SHoundAllMMDBID]: Could not allocate space");
     free(search_mmdb);
     return FALSE;
   }
   sprintf((char *)search_mmdb, "SELECT mmdbid from %s.mmdb", database);

   struct{
     SQLINTEGER len;
     SQLINTEGER val;
   }mmdbid;

  if(CreateHandle(&hstmt,search_mmdb, hdbc, &henv) != SQL_SUCCESS)
  {
    ErrPostEx(SEV_ERROR, 0, 0, "[SHoundAllMMDBID]: Could not create handle.");
    free(search_mmdb);
    return NULL;
  }
  free(search_mmdb);

  /* execute statment */
  sqlrc=SQLExecute(hstmt);
   if (sqlrc != SQL_SUCCESS)
   {
	  ErrPostEx(SEV_ERROR,0,0, "[SHoundAllMMDBID]: unable to execute select statement.");
	  LogSQLInfo(hstmt);
      SQLFreeStmt(hstmt, SQL_DROP);
      return NULL;
   }

  sqlrc=SQLBindCol(hstmt,1,SQL_C_SLONG,&mmdbid.val,0,(int*)&mmdbid.len);
  if (sqlrc != SQL_SUCCESS && sqlrc != SQL_SUCCESS_WITH_INFO)
  {
    ErrPostEx(SEV_ERROR,0,0,"SHoundAllMMDBID]: unable to execute bind column 1.");
    SQLFreeStmt(hstmt, SQL_DROP);
    return NULL;
  }

   /* fetch the data in columns iteratively */
   SQLINTEGER fetchCount = 0;
   /* fetch result and save in the linked list */
   while((sqlrc=SQLFetch(hstmt)) == SQL_SUCCESS || sqlrc == SQL_SUCCESS_WITH_INFO)
   {
      fetchCount++;
	  if (!ValNodeIsIn(mmdbid.val, pvnMd))
           pvnMd = Slri_ValNodeAddInt(&pvnMd, 0, mmdbid.val);
   }

  if(SQLFreeStmt(hstmt, SQL_DROP) != SQL_SUCCESS)
  {
     ErrPostEx(SEV_INFO, 0 ,0, "[SHoundAllMMDBID] : unable to free statement.");
     print_err(hstmt);
     return NULL;
   }

  if (fetchCount == 0) {
    ErrPostEx(SEV_INFO, 0, 0, "[SHoundAllMMDBID] No data found");
	return NULL;
  }

  return pvnMd;

}

Int4 LIBCALL GetLargestMmdbid(void)
{
   SQLRETURN sqlrc=SQL_SUCCESS;
   SQLHANDLE hstmt;
   Int4 maxid=0;

   CharPtr search_mmdb = (char *) malloc(sizeof(Char) * 150);
   if (search_mmdb == NULL)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[GetLargestMmdbid]: Could not allocate space");
     free(search_mmdb);
     return 0;
   }
   sprintf((char *)search_mmdb, "SELECT max(mmdbid) as max from %s.mmdb", database);

   struct{
     SQLINTEGER len;
     SQLINTEGER val;
   }max;

   /* NULL returned for max if the table is empty */
   /* initialize to avoid a weird number */
   max.val=0; max.len=0;

  if(CreateHandle(&hstmt,search_mmdb, hdbc, &henv) != SQL_SUCCESS)
  {
    ErrPostEx(SEV_ERROR, 0, 0, "[GetLargestMmdbid]: Could not create handle.");
    free(search_mmdb);
    return 0;
  }
  free(search_mmdb);

  /* execute statment */
  sqlrc=SQLExecute(hstmt);
   if (sqlrc != SQL_SUCCESS)
   {
	  ErrPostEx(SEV_ERROR,0,0, "[GetLargestMmdbid]: unable to execute select statement.");
	  LogSQLInfo(hstmt);
      SQLFreeStmt(hstmt, SQL_DROP);
      return 0;
   }

  sqlrc=SQLBindCol(hstmt,1,SQL_C_SLONG,&max.val,0,(int*)&max.len);
  if (sqlrc != SQL_SUCCESS && sqlrc != SQL_SUCCESS_WITH_INFO)
  {
    ErrPostEx(SEV_ERROR,0,0,"[GetLargestMmdbid]: unable to execute bind column 1.");
    SQLFreeStmt(hstmt, SQL_DROP);
    return 0;
  }

   while((sqlrc=SQLFetch(hstmt)) == SQL_SUCCESS || sqlrc == SQL_SUCCESS_WITH_INFO)
   {
	   maxid = max.val;
   }

  if(SQLFreeStmt(hstmt, SQL_DROP) != SQL_SUCCESS)
  {
     ErrPostEx(SEV_INFO, 0 ,0, "[GetLargestMmdbid] : unable to free statement.");
     print_err(hstmt);
     return 0;
   }

  return maxid;

}

Boolean LIBCALL OpenMMGI(CharPtr name, Boolean Create){ return TRUE;}


Boolean LIBCALL AppendRecordMMGI(StMmgiPtr pmg) {

   if(pmg == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "[AppendRecordMMGI]: NULL parameter.");
      return FALSE;
    }

   SQLRETURN retcode;

   mmgi_mmdbid.val = pmg->mmdbid;
   mmgi_gi.val = pmg->gi;

  /* execute statment */
   retcode = SQLExecute(HNDLMmgi);
   if (retcode != SQL_SUCCESS)
   {
	  ErrPostEx(SEV_ERROR,0,0, "[AppendRecordMMGI]: unable to execute MMGI insert statement. (mmid=%ld)",mmgi_mmdbid.val);
	  LogSQLInfo(HNDLMmgi);
      return FALSE;
   }

	return TRUE;
}

/*******************************************************/
/*Given a pointer to the asn structure that represents */
/*  an mmgi record this function pulls out the         */
/*  taxid from the asn structure and                   */
/*  retrieves all the records associated with that key */
/*Returns : 1 if successful, 0 if fails, -1 if no data */
/*******************************************************/

Int2 LIBCALL SearchMMGI (StMmgiPtr PNTR ppmg)
{
   StMmgiPtr pmg = NULL;                /* linked list to hold return values */
   SQLINTEGER iKey = 0;                  /* search key*/
   Char *pSQL_stmt;                      /* SQL select statement */
   SQLHANDLE hstmt;                      /* handle to select statement */
   SQLRETURN retcode;                    /* return values for odbc layer */

   if (ppmg == NULL)
   {
       ErrPostEx(SEV_ERROR,0,0, "[SearchMMGI]: StMmgiPtr is NULL.");
       return 0;
   }
   pmg = *ppmg;

   /* allocate space for the select statement */
   pSQL_stmt = (char*) malloc(sizeof(char) * 150);
   if(pSQL_stmt == NULL)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[SearchMMGI]: Could not allocate space");
      return 0;
   }
   /* fill in the select statement based on the search key passed in.
      database is a global variable initialized in ConnectToDb. The
      value of database is set in the ini file. eg database = seqhound. */
   if (pmg->gi > 0)
   {
      iKey = pmg->gi;
      sprintf((char*)pSQL_stmt, "select mmdbid, gi from %s.mmgi where gi=?;", database);
   }
   else if (pmg->mmdbid >0 ) {
	  iKey = pmg->mmdbid;
      sprintf((char*)pSQL_stmt, "select mmdbid, gi from %s.mmgi where mmdbid=?;", database);
   }
   else{
      ErrPostEx(SEV_ERROR, 0, 0, "[SearchMMGI]: No search key found.");
      free(pSQL_stmt);
      return 0;
   }

   /* create handle to statement, once handle exists we can free char* */
   if(CreateHandle(&hstmt, pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[SearchMMGI]: Could not create handle.");
      free(pSQL_stmt);
      return 0;
   }
   free(pSQL_stmt);


   /* bind the search key */
   retcode = SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &iKey, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[SearchMMGI]: Could not bind parameter");
	  SQLFreeStmt(hstmt, SQL_DROP);
      return 0;
   }


   /* execute the select statement */
   retcode = SQLExecute(hstmt);
   if (retcode != SQL_SUCCESS)
   {
	  ErrPostEx(SEV_ERROR,0,0, "[SearchMMGI]: Could not execute select statement.");
	  LogSQLInfo(hstmt);
      SQLFreeStmt(hstmt, SQL_DROP);
      return 0;
   }

   /* binding our results from the database */

   retcode = SQLBindCol(hstmt, 1, SQL_C_SLONG, &mmgi_mmdbid.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[SearchMMGI]: Could not bind column 1.");
	 SQLFreeStmt(hstmt, SQL_DROP);
     return 0;
   }
   retcode = SQLBindCol(hstmt, 2, SQL_C_SLONG, &mmgi_gi.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[SearchMMGI]: Could not bind column 2.");
	 SQLFreeStmt(hstmt, SQL_DROP);
     return 0;
   }

   /* fetch results and save in your linked list */
   SQLINTEGER fetchCount = 0;
   /* fetch result and save in the linked list */
   while((retcode=SQLFetch(hstmt)) == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
   {
      fetchCount++;
      if (fetchCount>1)
      {
         pmg->next = StMmgiNew();
         pmg = pmg->next;
      }

      pmg->mmdbid = mmgi_mmdbid.val;
      pmg->gi = mmgi_gi.val;

   }

   if(SQLFreeStmt(hstmt, SQL_DROP) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[SearchMMGI]: Could not free statement.");
      return 0;
   }

   if (fetchCount == 0)
	  return -1;

   return 1;
}

Boolean LIBCALL OpenDOMDB(CharPtr name, Boolean Create){ return TRUE;}


Boolean LIBCALL AppendRecordDOMDB(StDomdbPtr pdd) {

   if(pdd == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "[AppendRecordDOMDB]: NULL parameter.");
      return FALSE;
    }

   SQLRETURN retcode;
   pABL pABLtmp = NULL;

   domdb_mmdbid.val = pdd->mmdbid;
   domdb_gi.val = pdd->gi;
   domdb_domno.val = pdd->domno;
   domdb_domall.val = pdd->domall;
   domdb_domid.val = pdd->domid;
   domdb_rep.val = pdd->rep;
   sprintf((char *)domdb_pdbid.val,"%s", IsNullStr(pdd->pdbid));
   sprintf((char *)domdb_chain.val,"%s", IsNullStr(pdd->chain));


   /* get the asn1 bioseq into a bytestore pointer so that we can put it into a buffer. */
   if (pdd->asn1 != NULL) {
      pABLtmp = (pABL)AssignASNMemChar((Pointer) pdd->asn1, (AsnWriteFunc) SLRIValNodeAsnWrite);
	  if (pABLtmp == NULL) {
		  ErrPostEx(SEV_ERROR, 0, 0, "AssignASNMemChar failed.");
		  ClearStructures();
		  return FALSE;
	  }
      domdb_asn.val = pABLtmp->buf;
      domdb_asn.len = pABLtmp->len;
   } else {
      domdb_asn.val = NULL;
      domdb_asn.len = 0;
   }

   /* set up the input parameter. We bind the blob here because its domall
    * changes for each blob, so we must individually bind it here. */
   retcode = SQLBindParameter(HNDLDomdb, 2, SQL_PARAM_INPUT, SQL_C_DEFAULT, SQL_LONGVARBINARY, 0, 0, domdb_asn.val, 0, (int*)&domdb_asn.len);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO) {
     ErrPostEx(SEV_ERROR, 0, 0, "DOMDB: Could not bind parameter 2");
     return FALSE;
   }

  /* execute statment */
   retcode = SQLExecute(HNDLDomdb);
   if (retcode != SQL_SUCCESS)
   {
	  ErrPostEx(SEV_ERROR,0,0, "[AppendRecordDOMDB]: unable to execute DOMDB insert statement. (mmid=%ld)",domdb_mmdbid.val);
	  LogSQLInfo(HNDLDomdb);
      return FALSE;
   }

   MemFree(domdb_asn.val);
   FreeABL(pABLtmp);

	return TRUE;
}

/*******************************************************/
/*Given a pointer to the asn structure that represents */
/*  a domdb record this function pulls out the         */
/*  mmdbid from the asn structure and                  */
/*  retrieves all the records associated with that key */
/*Returns : 1 if successful, 0 if fails, -1 if no data */
/*******************************************************/

Int2 LIBCALL SearchDOMDB (StDomdbPtr PNTR ppdd)
{
   StDomdbPtr pdd = NULL;                /* linked list to hold return values */
   SQLINTEGER iKey = 0, iLength = 0;           /* search key and length of key */
   CharPtr pKey = NULL;                  /* search key for strings */
   Char *pSQL_stmt;                      /* SQL select statement */
   SQLHANDLE hstmt, LocUse;              /* handle to select statement */
   SQLRETURN retcode;                    /* return values for odbc layer */

   Int4 bloblen = 0, indicator = 0, pbbvalue = 0;
   SQLINTEGER blobloc = 0;
   CharPtr pcASN = NULL;

   if (ppdd == NULL)
   {
       ErrPostEx(SEV_ERROR,0,0, "[SearchDOMDB]: StDomdbPtr is NULL.");
       return 0;
   }
   pdd = *ppdd;
   /* allocate space for the select statement */
   pSQL_stmt = (char*) malloc(sizeof(char) * 150);
   if(pSQL_stmt == NULL)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[SearchDOMDB]: Could not allocate space");
      return 0;
   }
   /* fill in the select statement based on the search key passed in.
      database is a global variable initialized in ConnectToDb. The
      value of database is set in the ini file. eg database = seqhound. */
   if (pdd->domid > 0)
   {
      iKey = pdd->domid;
      sprintf((char*)pSQL_stmt, "select mmdbid, asn1, pdbid, pdbchain, gi, domno, domall, domid, rep from %s.domdb where domid=?;", database);
   }
   else if (pdd->gi > 0)
   {
      iKey = pdd->gi;
      sprintf((char*)pSQL_stmt, "select mmdbid, asn1, pdbid, pdbchain, gi, domno, domall, domid, rep from %s.domdb where gi=?;", database);
   }
   else if (pdd->mmdbid > 0)
   {
      iKey = pdd->mmdbid;
      sprintf((char*)pSQL_stmt, "select mmdbid, asn1, pdbid, pdbchain, gi, domno, domall, domid, rep from %s.domdb where mmdbid=?;", database);
   }
   else if (pdd->domall > 0)
   {
      iKey = pdd->domall;
      sprintf((char*)pSQL_stmt, "select mmdbid, asn1, pdbid, pdbchain, gi, domno, domall, domid, rep from %s.domdb where domall=?;", database);
   }
   else if (pdd->pdbid != NULL) {
	  pKey = pdd->pdbid;
      sprintf((char*)pSQL_stmt, "select mmdbid, asn1, pdbid, pdbchain, gi, domno, domall, domid, rep from %s.domdb where pdbid=?;", database);
   }
   else{
      ErrPostEx(SEV_ERROR, 0, 0, "[SearchDOMDB]: No search key found.");
      free(pSQL_stmt);
      return 0;
   }

   /* create handle to statement, once handle exists we can free char* */
   if(CreateHandle(&hstmt, pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[SearchDOMDB]: Could not create handle.");
      free(pSQL_stmt);
      return 0;
   }
   free(pSQL_stmt);


   /* bind the search key */
   if (iKey > 0)
      retcode = SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &iKey, 0, NULL);
   else
      retcode = SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_DEFAULT, SQL_VARCHAR, 20, 0, pKey, 0, NULL);

   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[SearchDOMDB]: Could not bind parameter");
	  SQLFreeStmt(hstmt, SQL_DROP);
      return 0;
   }

   /* execute the select statement */
   retcode = SQLExecute(hstmt);
   if (retcode != SQL_SUCCESS)
   {
	  ErrPostEx(SEV_ERROR,0,0, "[SearchDOMDB]: Could not execute select statement.");
	  LogSQLInfo(hstmt);
      SQLFreeStmt(hstmt, SQL_DROP);
      return 0;
   }

   /* binding our results from the database */

   retcode = SQLBindCol(hstmt, 1, SQL_C_SLONG, &domdb_mmdbid.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[SearchDOMDB]: Could not bind column 1.");
	 SQLFreeStmt(hstmt, SQL_DROP);
     return 0;
   }
   retcode = SQLBindCol(hstmt, 2, SQL_C_BINARY, &blobloc, 0, (int*)&pbbvalue);
   if((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO))
   {
      ErrPostEx(SEV_ERROR,0,0, "[SearchDOMDB]: Binding Column 2 failed.");
	  SQLFreeStmt(hstmt, SQL_DROP);
      return 0;
   }
   retcode = SQLBindCol(hstmt, 3, SQL_C_CHAR, domdb_pdbid.val, 20, (int*)&domdb_pdbid.len);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[SearchDOMDB]: Could not bind column 3.");
	 SQLFreeStmt(hstmt, SQL_DROP);
     return 0;
   }
   retcode = SQLBindCol(hstmt, 4, SQL_C_CHAR, domdb_chain.val, 20, (int*)&domdb_chain.len);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[SearchDOMDB]: Could not bind column 4.");
	 SQLFreeStmt(hstmt, SQL_DROP);
     return 0;
   }
   retcode = SQLBindCol(hstmt, 5, SQL_C_SLONG, &domdb_gi.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[SearchDOMDB]: Could not bind column 5.");
	 SQLFreeStmt(hstmt, SQL_DROP);
     return 0;
   }
   retcode = SQLBindCol(hstmt, 6, SQL_C_SLONG, &domdb_domno.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[SearchDOMDB]: Could not bind column 6.");
	 SQLFreeStmt(hstmt, SQL_DROP);
     return 0;
   }
   retcode = SQLBindCol(hstmt, 7, SQL_C_SLONG, &domdb_domall.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[SearchDOMDB]: Could not bind column 7.");
	 SQLFreeStmt(hstmt, SQL_DROP);
     return 0;
   }
   retcode = SQLBindCol(hstmt, 8, SQL_C_SLONG, &domdb_domid.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[SearchDOMDB]: Could not bind column 8.");
	 SQLFreeStmt(hstmt, SQL_DROP);
     return 0;
   }
   retcode = SQLBindCol(hstmt, 9, SQL_C_SLONG, &domdb_rep.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[SearchDOMDB]: Could not bind column 9.");
	 SQLFreeStmt(hstmt, SQL_DROP);
     return 0;
   }

   /* fetch results and save in your linked list */
   SQLINTEGER fetchCount = 0;
   /* fetch result and save in the linked list */
   while((retcode=SQLFetch(hstmt)) == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
   {
      fetchCount++;
      if (fetchCount>1)
      {
         pdd->next = StDomdbNew();
         pdd = pdd->next;
      }

      pdd->mmdbid = domdb_mmdbid.val;
      pdd->gi = domdb_gi.val;
      pdd->domno = domdb_domno.val;
      pdd->domall = domdb_domall.val;
      pdd->domid = domdb_domid.val;
      pdd->rep = domdb_rep.val;

      if(domdb_pdbid.len >0 ){
         pdd->pdbid = StringSave((char*)domdb_pdbid.val);
      } else{
         pdd->pdbid = StringSave("n/a");
      }

      if(domdb_chain.len >0 ){
         pdd->chain = StringSave((char*)domdb_chain.val);
      } else{
         pdd->chain = StringSave("n/a");
      }


	  /*Get the length of of the blob so that we can read it */
	  SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &LocUse);
	  bloblen = (Int4)pbbvalue;

	  if(bloblen > 0)
	  {
	      /* allocate a buffer big enough to store the blob. */
	      pcASN = (CharPtr) MemNew((size_t) bloblen);

	      /*read the blob from the database */
	      /* RAC put an if Statement here if (SQLGetData(...) != SQL_SUCCESS) --->> print_err(hstmt) */
	      SQLGetData(hstmt, 2, SQL_C_BINARY, (SQLPOINTER)pcASN, bloblen, (int*)&indicator);

	      if(pcASN != NULL)
	      {
			pdd->asn1 = (SLRIValNodePtr)GetASNMemChar(pcASN,(AsnReadFunc)SLRIValNodeAsnRead,bloblen);
	      }
	      if (pcASN) { MemFree(pcASN); }

	  }
	  else
	  {
	      ErrPostEx(SEV_INFO, 0, 0, "[SearchDOMDB] ASN1 Blob retrieved has length = 0");
	  }

	  if (SQLFreeHandle(SQL_HANDLE_STMT, LocUse) != SQL_SUCCESS)
	  {
	      ErrPostEx(SEV_INFO, 0, 0, "[SearchDOMDB]: Unable to free handle");
	      SQLFreeStmt(hstmt, SQL_DROP);
	      return 0;
	  }

   }

   if(SQLFreeStmt(hstmt, SQL_DROP) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[SearchDOMDB]: Could not free statement.");
      return 0;
   }

   if (fetchCount == 0)
	  return -1;

   return 1;
}

Boolean DeleteAllRecordsInDOMDB()
{
   SQLHANDLE hstmt;
   Int2 sql_return = SQL_SUCCESS;

   CharPtr delete_stmt = (char *) malloc(sizeof(Char) * 150);
   if (delete_stmt == NULL)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[DeleteAllRecordsInDOMDB]: Could not allocate space");
     free(delete_stmt);
     return FALSE;
   }
   sprintf((char *)delete_stmt, "delete from %s.domdb", database);

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
      ErrPostEx(SEV_ERROR,0,0, "[DeleteAllRecordsInDOMDB]:unable to execute delete statement.");
      LogSQLInfo(hstmt);
      SQLFreeStmt(hstmt, SQL_DROP);
      return FALSE;
   }

   SQLFreeStmt(hstmt, SQL_DROP);
   return TRUE;
}

/*************************************/
/* Edit record functions - needed for*/
/* some parsers (domains/pdbrep)     */
/* search key is always in the name  */
/* of the function                   */
/*************************************/
/* note: asn.1 is not updated */
/* only updating rep (TRUE) is in use */
Boolean LIBCALL EditRecordDOMDBByDOMID(StDomdbPtr pdd, Boolean overwrite) {
	StDomdbPtr pddtemp = NULL;
	SQLRETURN retcode;
	SQLINTEGER fetchCount=0;

	if(pdd->domid <= 0)
	{
		ErrPostEx(SEV_ERROR,0,0, "EditRecordDOMDBByDOMID: Invalid parameters.");
		return FALSE;
	}

    pddtemp = StDomdbNew();
	domdb_domid.val = pdd->domid;

    /* search the record in database */
	/* don't call SearchDOMDB because it is slow if this Edit function is called many times in parser */
	retcode = SQLExecute(HNDLDomdbSearchDomid);
   if (retcode != SQL_SUCCESS)
   {
	  ErrPostEx(SEV_ERROR,0,0, "[EditRecordDOMDBByDOMID]: unable to execute search statement. (domid=%ld)",domdb_domid.val);
	  LogSQLInfo(HNDLDomdbSearchDomid);
      return FALSE;
   }

   retcode = SQLBindCol(HNDLDomdbSearchDomid, 1, SQL_C_SLONG, &domdb_mmdbid.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[EditRecordDOMDBByDOMID]: Could not bind column 1.");
     return FALSE;
   }
   retcode = SQLBindCol(HNDLDomdbSearchDomid, 2, SQL_C_CHAR, domdb_pdbid.val, 20, (int*)&domdb_pdbid.len);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[EditRecordDOMDBByDOMID]: Could not bind column 2.");
     return FALSE;
   }
   retcode = SQLBindCol(HNDLDomdbSearchDomid, 3, SQL_C_CHAR, domdb_chain.val, 20, (int*)&domdb_chain.len);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[EditRecordDOMDBByDOMID]: Could not bind column 3.");
     return FALSE;
   }
   retcode = SQLBindCol(HNDLDomdbSearchDomid, 4, SQL_C_SLONG, &domdb_gi.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[EditRecordDOMDBByDOMID]: Could not bind column 4.");
     return FALSE;
   }
   retcode = SQLBindCol(HNDLDomdbSearchDomid, 5, SQL_C_SLONG, &domdb_domno.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[EditRecordDOMDBByDOMID]: Could not bind column 5.");
     return FALSE;
   }
   retcode = SQLBindCol(HNDLDomdbSearchDomid, 6, SQL_C_SLONG, &domdb_domall.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[EditRecordDOMDBByDOMID]: Could not bind column 6.");
     return FALSE;
   }
   retcode = SQLBindCol(HNDLDomdbSearchDomid, 7, SQL_C_SLONG, &domdb_domid.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[EditRecordDOMDBByDOMID]: Could not bind column 7.");
     return FALSE;
   }
   retcode = SQLBindCol(HNDLDomdbSearchDomid, 8, SQL_C_SLONG, &domdb_rep.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[EditRecordDOMDBByDOMID]: Could not bind column 8.");
     return FALSE;
   }

   /* update every record that has gi matched */
   while((retcode=SQLFetch(HNDLDomdbSearchDomid)) == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
   {
      fetchCount++;
      if (fetchCount>1)
      {
         pddtemp->next = StDomdbNew();
         pddtemp = pddtemp->next;
      }

      pddtemp->mmdbid = domdb_mmdbid.val;
      pddtemp->gi = domdb_gi.val;
      pddtemp->domno = domdb_domno.val;
      pddtemp->domall = domdb_domall.val;
      pddtemp->domid = domdb_domid.val;
      pddtemp->rep = domdb_rep.val;

      if(domdb_pdbid.len >0 ){
         pddtemp->pdbid = StringSave((char*)domdb_pdbid.val);
      } else{
         pddtemp->pdbid = NULL;
      }

      if(domdb_pdbid.len >0 ){
         pddtemp->chain = StringSave((char*)domdb_chain.val);
      } else{
         pddtemp->chain = NULL;
      }

		if(overwrite == TRUE)  /* replace all values except DOMID but only if the replacement values are valid*/
		 {
			if (pdd->mmdbid !=0) domdb_mmdbid.val = pdd->mmdbid;
			else domdb_mmdbid.val = pddtemp->mmdbid;

			if (pdd->pdbid != NULL) sprintf((char *)domdb_pdbid.val,"%s", IsNullStr(pdd->pdbid));
			else sprintf((char *)domdb_pdbid.val,"%s", IsNullStr(pddtemp->pdbid));

			if (pdd->chain != NULL) sprintf((char *)domdb_chain.val,"%s", IsNullStr(pdd->chain));
			else sprintf((char *)domdb_chain.val,"%s", IsNullStr(pddtemp->chain));

			if (pdd->gi !=0) domdb_gi.val = pdd->gi;
			else domdb_gi.val = pddtemp->gi;

			if (pdd->domno !=0) domdb_domno.val = pdd->domno;
			else domdb_domno.val = pddtemp->domno;

			if (pdd->domall !=0) domdb_domall.val = pdd->domall;
			else domdb_domall.val = pddtemp->domall;

			if (pdd->rep !=0) domdb_rep.val = pdd->rep;
			else domdb_rep.val = pddtemp->rep;


			/*no update for asn.1 */

			  /* execute update statment */
			retcode = SQLExecute (HNDLDomdbUpdateAll);
			if (retcode != SQL_SUCCESS){
				ErrPostEx(SEV_ERROR,0,0, "[EditRecordDOMDBByDomid]: unable to execute DOMDB update all statement. domid=%ld", domdb_domid.val);
				LogSQLInfo(HNDLDomdbUpdateAll);
				return FALSE;
			}

		 }
		 else  /* replace only if no value in the database*/
		 {
			 if((pddtemp->mmdbid == 0) && (pdd->mmdbid != 0)) {
				domdb_mmdbid.val = pdd->mmdbid;
				retcode = SQLExecute (HNDLDomdbUpdateMmdbid);
				if (retcode != SQL_SUCCESS) {
					ErrPostEx(SEV_ERROR,0,0, "[EditRecordDOMDBByDOMID]: Error in updating mmdbid.  domid=%ld", domdb_domid.val);
					LogSQLInfo(HNDLDomdbUpdateMmdbid);
					return FALSE;
				}
			 }
			 if((pddtemp->pdbid == NULL) && (pdd->pdbid != NULL)) {
				sprintf((char *)domdb_pdbid.val,"%s", IsNullStr(pdd->pdbid));
				retcode = SQLExecute (HNDLDomdbUpdatePdbid);
				if (retcode != SQL_SUCCESS) {
					ErrPostEx(SEV_ERROR,0,0, "[EditRecordDOMDBByDOMID]: Error in updating pdbid  domid=%ld", domdb_domid.val);
					LogSQLInfo(HNDLDomdbUpdatePdbid);
					return FALSE;
				}
			 }
			 if((pddtemp->chain == NULL) && (pdd->chain != NULL)) {
				sprintf((char *)domdb_chain.val,"%s", IsNullStr(pdd->chain));
				retcode = SQLExecute (HNDLDomdbUpdateChain);
				if (retcode != SQL_SUCCESS) {
					ErrPostEx(SEV_ERROR,0,0, "[EditRecordDOMDBByDOMID]: Error in updating pdbchain.  domid=%ld", domdb_domid.val);
					LogSQLInfo(HNDLDomdbUpdateChain);
					return FALSE;
				}
			 }
			 if((pddtemp->gi == 0) && (pdd->gi != 0)) {
				domdb_gi.val = pdd->gi;
				retcode = SQLExecute (HNDLDomdbUpdateGi);
				if (retcode != SQL_SUCCESS) {
					ErrPostEx(SEV_ERROR,0,0, "[EditRecordDOMDBByDOMID]: Error in updating gi.  domid=%ld", domdb_domid.val);
					LogSQLInfo(HNDLDomdbUpdateGi);
					return FALSE;
				}
			 }
			 if((pddtemp->domno == 0) && (pdd->domno != 0)) {
				domdb_domno.val = pdd->domno;
				retcode = SQLExecute (HNDLDomdbUpdateDomno);
				if (retcode != SQL_SUCCESS) {
					ErrPostEx(SEV_ERROR,0,0, "[EditRecordDOMDBByDOMID]: Error in updating domno.  domid=%ld", domdb_domid.val);
					LogSQLInfo(HNDLDomdbUpdateDomno);
					return FALSE;
				}
			 }
			 if((pddtemp->domall == 0) && (pdd->domall != 0)) {
				domdb_domall.val = pdd->domall;
				retcode = SQLExecute (HNDLDomdbUpdateDomall);
				if (retcode != SQL_SUCCESS) {
					ErrPostEx(SEV_ERROR,0,0, "[EditRecordDOMDBByDOMID]: Error in updating domall.  domid=%ld", domdb_domid.val);
					LogSQLInfo(HNDLDomdbUpdateDomall);
					return FALSE;
				}
			 }
			 if((pddtemp->rep == 0) && (pdd->rep != 0)) {
				domdb_rep.val = pdd->rep;
				retcode = SQLExecute (HNDLDomdbUpdateRep);
				if (retcode != SQL_SUCCESS) {
					ErrPostEx(SEV_ERROR,0,0, "[EditRecordDOMDBByDOMID]: Error in updating rep.  domid=%ld", domdb_domid.val);
					LogSQLInfo(HNDLDomdbUpdateRep);
					return FALSE;
				}
			 }
			 /* no update for asn.1 */
		 }

   } /* while fetch */


   /* close the cursor and discard all pending results. */
   /* must do in order to re-use the handle created in InitDOMDB */
   retcode = SQLFreeStmt(HNDLDomdbSearchDomid, SQL_CLOSE);
   if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[EditRecordDOMDBByDOMID]: Error in closing cursor.");
      return FALSE;
   }

    StDomdbFree(pddtemp);
	return TRUE;
}


Int4 LIBCALL SHoundRecNoDOMDB(void) {
	SQLRETURN retcode;

	struct
    {
       SQLINTEGER len;
       SQLINTEGER val;
    }count;
	count.val=0; count.len=0;


   retcode = SQLExecute(HNDLDomdbSelectCount);
   if (retcode != SQL_SUCCESS)
   {
	  ErrPostEx(SEV_ERROR,0,0, "[SHoundRecNoDOMDB]: Could not execute select count statement.");
	  LogSQLInfo(HNDLDomdbSelectCount);
      return 0;
   }

   retcode = SQLBindCol(HNDLDomdbSelectCount, 1, SQL_C_SLONG, &count.val, 0, (int*)&count.len);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
		ErrPostEx(SEV_ERROR, 0, 0, "[SHoundRecNoDOMDB]: Could not bind column 1.");
		return 0;
   }

   retcode = SQLFetch(HNDLDomdbSelectCount);
   if(retcode == SQL_NO_DATA_FOUND){
      ErrPostEx(SEV_INFO, 0, 0, "[SHoundRecNoDOMDB]: No data found");
      return 0;
   }

   if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
      /* close the cursor and discard all pending results. */
      /* must do in order to re-use the handle created in InitACCDB */
	   if(SQLFreeStmt(HNDLDomdbSelectCount, SQL_CLOSE) != SQL_SUCCESS)
	   {
		  ErrPostEx(SEV_ERROR, 0, 0, "[SHoundRecNoDOMDB]]: Could not free statement.");
		  return 0;
	   }
	   return count.val;
   }

   return 0;

}


/* returns a record based on record number */
StDomdbPtr LIBCALL SHoundGetRecDOMDB(Int4 recno) {
   StDomdbPtr pdd = StDomdbNew();                /* linked list to hold return values */
   SQLRETURN retcode;                    /* return values for odbc layer */

   Int4 bloblen = 0, indicator = 0, pbbvalue = 0;
   SQLINTEGER blobloc = 0;
   CharPtr pcASN = NULL;

   if (recno < 1 || recno > SHoundRecNoDOMDB()) {
      ErrPostEx(SEV_ERROR, 0, 0, "[SHoundGetRecDOMDB]: Invalid record number %ld", recno);
      return NULL;
   }

   rowid.val = recno;

   /* execute the select statement */
   retcode = SQLExecute(HNDLDomdbSearchRowid);
   if (retcode != SQL_SUCCESS)
   {
	  ErrPostEx(SEV_ERROR,0,0, "[SHoundGetRecDOMDB]: Could not execute select statement.");
	  LogSQLInfo(HNDLAccdbSearchRowid);
      return NULL;
   }

   /* binding our results from the database */

   retcode = SQLBindCol(HNDLAccdbSearchRowid, 1, SQL_C_SLONG, &domdb_mmdbid.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[SHoundGetRecDOMDB]: Could not bind column 1.");
     return NULL;
   }
   retcode = SQLBindCol(HNDLAccdbSearchRowid, 2, SQL_C_BINARY, &blobloc, 0, (int*)&pbbvalue);
   if((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO))
   {
      ErrPostEx(SEV_ERROR,0,0, "[SHoundGetRecDOMDB]: Binding Column 2 failed.");
      return NULL;
   }
   retcode = SQLBindCol(HNDLAccdbSearchRowid, 3, SQL_C_CHAR, domdb_pdbid.val, 20, (int*)&domdb_pdbid.len);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[SHoundGetRecDOMDB]: Could not bind column 3.");
     return NULL;
   }
   retcode = SQLBindCol(HNDLAccdbSearchRowid, 4, SQL_C_CHAR, domdb_chain.val, 20, (int*)&domdb_chain.len);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[SHoundGetRecDOMDB]: Could not bind column 4.");
     return NULL;
   }
   retcode = SQLBindCol(HNDLAccdbSearchRowid, 5, SQL_C_SLONG, &domdb_gi.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[SHoundGetRecDOMDB]: Could not bind column 5.");
     return NULL;
   }
   retcode = SQLBindCol(HNDLAccdbSearchRowid, 6, SQL_C_SLONG, &domdb_domno.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[SHoundGetRecDOMDB]: Could not bind column 6.");
     return NULL;
   }
   retcode = SQLBindCol(HNDLAccdbSearchRowid, 7, SQL_C_SLONG, &domdb_domall.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[SHoundGetRecDOMDB]: Could not bind column 7.");
     return NULL;
   }
   retcode = SQLBindCol(HNDLAccdbSearchRowid, 8, SQL_C_SLONG, &domdb_domid.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[SHoundGetRecDOMDB]: Could not bind column 8.");
     return NULL;
   }
   retcode = SQLBindCol(HNDLAccdbSearchRowid, 9, SQL_C_SLONG, &domdb_rep.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[SHoundGetRecDOMDB]: Could not bind column 9.");
     return NULL;
   }

   /* fetch results and save in your linked list */
   SQLINTEGER fetchCount = 0;
   /* fetch result and save in the linked list */
   while((retcode=SQLFetch(HNDLAccdbSearchRowid)) == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
   {
      fetchCount++;
      if (fetchCount>1)
      {
         pdd->next = StDomdbNew();
         pdd = pdd->next;
      }

      pdd->mmdbid = domdb_mmdbid.val;
      pdd->gi = domdb_gi.val;
      pdd->domno = domdb_domno.val;
      pdd->domall = domdb_domall.val;
      pdd->domid = domdb_domid.val;
      pdd->rep = domdb_rep.val;

      if(domdb_pdbid.len >0 ){
         pdd->pdbid = StringSave((char*)domdb_pdbid.val);
      } else{
         pdd->pdbid = NULL;
      }

      if(domdb_pdbid.len >0 ){
         pdd->chain = StringSave((char*)domdb_chain.val);
      } else{
         pdd->chain = NULL;
      }

	  /*Get the length of of the blob so that we can read it */
	  bloblen = (Int4)pbbvalue;

	  if(bloblen > 0)
	  {
	      /* allocate a buffer big enough to store the blob. */
	      pcASN = (CharPtr) MemNew((size_t) bloblen);

	      /*read the blob from the database */
	      /* RAC put an if Statement here if (SQLGetData(...) != SQL_SUCCESS) --->> print_err(hstmt) */
	      SQLGetData(HNDLAccdbSearchRowid, 2, SQL_C_BINARY, (SQLPOINTER)pcASN, bloblen, (int*)&indicator);

	      if(pcASN != NULL)
	      {
			pdd->asn1 = (SLRIValNodePtr)GetASNMemChar(pcASN,(AsnReadFunc)SLRIValNodeAsnRead,bloblen);
	      }
	      if (pcASN) { MemFree(pcASN); }

	  }
	  else
	  {
	      ErrPostEx(SEV_INFO, 0, 0, "[SHoundGetRecDOMDB] ASN1 Blob retrieved has length = 0");
	  }

   }

   /* close the cursor and discard all pending results. */
   /* must do in order to re-use the handle created in InitDOMDB */
   retcode = SQLFreeStmt(HNDLDomdbSearchRowid, SQL_CLOSE);
   if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[SHoundGetRecDOMDB]: Error in closing cursor.");
      return NULL;
   }

   if (fetchCount == 0)
	  return NULL;

   return pdd;

}


Boolean LIBCALL OpenCDDB(CharPtr name, Boolean Create){ return TRUE;}


Boolean LIBCALL AppendRecordCDDB(StCddbPtr pcd) {

   if(pcd == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "[AppendRecordCDDB]: NULL parameter.");
      return FALSE;
    }

   SQLRETURN retcode;
   pABL pABLtmp = NULL;

   cddb_gi.val = pcd->gi;

   /* get the asn1 SLRIFasta into a bytestore pointer so that we can put it into a buffer. */
   if (pcd->asn1 != NULL) {
      pABLtmp = (pABL)AssignASNMemChar((Pointer) pcd->asn1, (AsnWriteFunc) SLRIFastaAsnWrite);
	  if (pABLtmp == NULL) {
		  ErrPostEx(SEV_ERROR, 0, 0, "AssignASNMemChar failed.");
		  ClearStructures();
		  return FALSE;
	  }
      cddb_asn.val = pABLtmp->buf;
      cddb_asn.len = pABLtmp->len;
   } else {
      cddb_asn.val = NULL;
      cddb_asn.len = 0;
   }

   /* set up the input parameter. We bind the blob here because its domall
    * changes for each blob, so we must individually bind it here. */
   retcode = SQLBindParameter(HNDLCddb, 2, SQL_PARAM_INPUT, SQL_C_DEFAULT, SQL_LONGVARBINARY, 0, 0, cddb_asn.val, 0, (int*)&cddb_asn.len);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO) {
     ErrPostEx(SEV_ERROR, 0, 0, "[AppendRecordCDDB]: Could not bind parameter 2");
     return FALSE;
   }

  /* execute statment */
   retcode = SQLExecute(HNDLCddb);
   if (retcode != SQL_SUCCESS)
   {
	  ErrPostEx(SEV_ERROR,0,0, "[AppendRecordCDDB]: unable to execute CDDB insert statement. (gi=%ld)",cddb_gi.val);
	  LogSQLInfo(HNDLCddb);
      return FALSE;
   }

   MemFree(cddb_asn.val);
   FreeABL(pABLtmp);

	return TRUE;
}

/*******************************************************/
/*Given a pointer to the asn structure that represents */
/*  a cddb record this function pulls out the         */
/*  mmdbid from the asn structure and                  */
/*  retrieves all the records associated with that key */
/*Returns : 1 if successful, 0 if fails, -1 if no data */
/*******************************************************/

Int2 LIBCALL SearchCDDB (StCddbPtr PNTR ppcd)
{
   StCddbPtr pcd = NULL;                /* linked list to hold return values */
   SQLINTEGER iKey = 0;
   Char *pSQL_stmt;                      /* SQL select statement */
   SQLHANDLE hstmt, LocUse;              /* handle to select statement */
   SQLRETURN retcode;                    /* return values for odbc layer */

   Int4 bloblen = 0, indicator = 0, pbbvalue = 0;
   SQLINTEGER blobloc = 0;
   CharPtr pcASN = NULL;

   if (ppcd == NULL)
   {
       ErrPostEx(SEV_ERROR,0,0, "[SearchCDDB]: StCddbPtr is NULL.");
       return 0;
   }
   pcd = *ppcd;

   /* allocate space for the select statement */
   pSQL_stmt = (char*) malloc(sizeof(char) * 150);
   if(pSQL_stmt == NULL)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[SearchCDDB]: Could not allocate space");
      return 0;
   }

   if (pcd->gi > 0)
   {
      iKey = pcd->gi;
      sprintf((char*)pSQL_stmt, "select gi, asn1 from %s.cddb where gi=?;", database);
   }
   else{
      ErrPostEx(SEV_ERROR, 0, 0, "[SearchCDDB]: Invalid search key found %ld", pcd->gi);
      free(pSQL_stmt);
      return 0;
   }

   /* create handle to statement, once handle exists we can free char* */
   if(CreateHandle(&hstmt, pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[SearchCDDB]: Could not create handle.");
      free(pSQL_stmt);
      return 0;
   }
   free(pSQL_stmt);


   /* bind the search key */
   retcode = SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &iKey, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[SearchCDDB]: Could not bind parameter");
	  SQLFreeStmt(hstmt, SQL_DROP);
      return 0;
   }

   /* execute the select statement */
   retcode = SQLExecute(hstmt);
   if (retcode != SQL_SUCCESS)
   {
	  ErrPostEx(SEV_ERROR,0,0, "[SearchCDDB]: Could not execute select statement.");
	  LogSQLInfo(hstmt);
      SQLFreeStmt(hstmt, SQL_DROP);
      return 0;
   }

   /* binding our results from the database */
   retcode = SQLBindCol(hstmt, 1, SQL_C_SLONG, &cddb_gi.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[SearchCDDB]: Could not bind column 1.");
	 SQLFreeStmt(hstmt, SQL_DROP);
     return 0;
   }
   retcode = SQLBindCol(hstmt, 2, SQL_C_BINARY, &blobloc, 0, (int*)&pbbvalue);
   if((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO))
   {
      ErrPostEx(SEV_ERROR,0,0, "[SearchCDDB]: Binding Column 2 failed.");
	  SQLFreeStmt(hstmt, SQL_DROP);
      return 0;
   }

   /* fetch results and save in your linked list */
   SQLINTEGER fetchCount = 0;
   /* fetch result and save in the linked list */
   while((retcode=SQLFetch(hstmt)) == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
   {
      fetchCount++;
      if (fetchCount>1)
      {
         pcd->next = StCddbNew();
         pcd = pcd->next;
      }

      pcd->gi = cddb_gi.val;

	  /*Get the length of of the blob so that we can read it */
	  SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &LocUse);
	  bloblen = (Int4)pbbvalue;

	  if(bloblen > 0)
	  {
	      /* allocate a buffer big enough to store the blob. */
	      pcASN = (CharPtr) MemNew((size_t) bloblen);

	      /*read the blob from the database */
	      /* RAC put an if Statement here if (SQLGetData(...) != SQL_SUCCESS) --->> print_err(hstmt) */
	      SQLGetData(hstmt, 2, SQL_C_BINARY, (SQLPOINTER)pcASN, bloblen, (int*)&indicator);

	      if(pcASN != NULL)
	      {
			pcd->asn1 = (SLRIFastaPtr)GetASNMemChar(pcASN,(AsnReadFunc)SLRIFastaAsnRead,bloblen);
	      }
	      if (pcASN) { MemFree(pcASN); }

	  }
	  else
	  {
	      ErrPostEx(SEV_INFO, 0, 0, "[SearchCDDB] ASN1 Blob retrieved has length = 0");
	  }

	  if (SQLFreeHandle(SQL_HANDLE_STMT, LocUse) != SQL_SUCCESS)
	  {
	      ErrPostEx(SEV_INFO, 0, 0, "[SearchCDDB]: Unable to free handle");
	      SQLFreeStmt(hstmt, SQL_DROP);
	      return 0;
	  }

   }

   if(SQLFreeStmt(hstmt, SQL_DROP) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[SearchCDDB]: Could not free statement.");
      return 0;
   }

   if (fetchCount == 0)
	  return -1;

   return 1;
}

Boolean LIBCALL DeleteGIinCDDB(SQLINTEGER gi, Boolean destroy) {
   CharPtr cddb_delete = (char *) malloc(sizeof(Char) * 150);
   if (cddb_delete == NULL)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[DeleteGIinCDDB]: Could not allocate space");
     free(cddb_delete);
     return FALSE;
   }
   sprintf((char *)cddb_delete, "delete from %s.cddb where gi=?", database);

   SQLHANDLE hstmt;
   Int2 sql_return = SQL_SUCCESS;

   CreateHandle(&hstmt, cddb_delete, hdbc, &henv );
   free(cddb_delete);

   SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_SLONG,SQL_INTEGER, 0, 0, &gi, 0, NULL);

  sql_return = SQLExecute(hstmt);
  /* execute statment */
  if (sql_return != SQL_SUCCESS)
  {
     ErrPostEx(SEV_ERROR,0,0, "DeleteGIinCDDB:unable to execute delete statement.");
	 LogSQLInfo(hstmt);
	 SQLFreeStmt(hstmt, SQL_DROP);
	 return FALSE;
  }

  SQLFreeStmt(hstmt, SQL_DROP);

	return TRUE;
}

/****************************************************************/
/* Given an SQL handle, set AutoCommit off                      */
/****************************************************************/
Boolean StartTransaction(void)
{
        return SetAutoCommitOff(hdbc);
}

/****************************************************************/
/* Given an SQL handle, end transaction                         */
/****************************************************************/
Boolean EndTransaction(void)
{
	/* the name is misleading.
           RollBackChanges calles SQLEndTran which takes in
           with "commit" or "rollback" as the parameter,
           we choose tp use RollBack since this function is
           called by read-only processes */
        return RollBackChanges(hdbc);
}

/****************************************************************/
/* Given an SQL handle, commit                                  */
/****************************************************************/
Boolean CommitTransaction(void)
{
        return CommitChanges(hdbc);
}

/****************************************************************/
/* Given an SQL handle, roll back                               */
/****************************************************************/
Boolean RollBackTransaction(void)
{
        return RollBackChanges(hdbc);
}

Boolean CommitAndRollBack()
{
  Boolean success = TRUE;

  if( !CommitTransaction() )
  {
	/*----------------------------------------------------------
	  If we cannot commit, try to rollback and return FALSE.
	  --------------------------------------------------------*/
	success = FALSE;

	ErrPostEx( SEV_ERROR, 0, 0, 
			   "CommitAndRollBack:Cannot commit transaction!" );
	if( !RollBackTransaction() )
	{
	  ErrPostEx( SEV_ERROR, 0, 0, "Cannot Roll Back!" );
	}
  }
  return success;
}
/****************************************************************/
/* Given an SQL handle, check if AutoCommit is on               */
/****************************************************************/
Boolean IsAutoCommitOn(void)
{
        SQLRETURN retcode;

        SQLINTEGER val;

        retcode = SQLGetConnectAttr(hdbc, SQL_ATTR_AUTOCOMMIT, &val, SQL_IS_INTEGER, 0);
        if (retcode != SQL_SUCCESS)
        {
                ErrPostEx(SEV_ERROR,0,0, "[IsAutoCommitOn]:unable to get ConnectAttr.");
                LogSQLConnectInfo(hdbc);
                return FALSE;
        }

        ErrPostEx(SEV_ERROR,0,0, "[IsAutoCommitOn]: val is %ld.", val);

        if (val == SQL_AUTOCOMMIT_ON)
                return TRUE;
        else if (val == SQL_AUTOCOMMIT_OFF)
                return FALSE;
        else {
                ErrPostEx(SEV_ERROR,0,0, "[IsAutoCommitOn]:unable to see Value.");
                return FALSE;
        }
}



ValNodePtr LIBCALL SearchTAXGIRNA(Int4 taxid){}


/*************************************************************************
  functions implemented in _cb or _db or _db2, but not used in API layer
  ************************************************************************/
BioseqPtr LIBCALL GetBioseqfromASNDB(void) {return NULL;}

CharPtr LIBCALL GetTitleFromACCDB(void) {return NULL;}

SeqEntryPtr LIBCALL GetSeqEntryfromSENDB(void) {return NULL;}

/*************************************************************************
 functions used in intrez.c
 *************************************************************************/
/* lighweight version of SHoundProteinsFromTaxID which does not fill out linked list of structures from TAXGI */
ValNodePtr LIBCALL SHoundProteinsFromTaxIDII(Int4 taxid)
{
	return SHoundProteinsFromTaxID(taxid);
}

ValNodePtr LIBCALL SHoundDNAFromTaxIDII(Int4 taxid)
{
	return SHoundDNAFromTaxID(taxid);
}


/* $Log: intrez_odbc.c,v $
/* Revision 1.55  2005/03/14 16:02:39  eburgess
/* Added SearchAccdbByName(). Bug 4939 and 5049
/*
/* Revision 1.54  2005/03/09 17:04:38  eburgess
/* Changed return values for SearchACCDBByAccess to make them more informative.
/*
/* Revision 1.53  2005/03/02 16:49:39  eburgess
/* Added CommitAndRollBack();
/*
/* Revision 1.52  2005/02/01 16:48:30  eburgess
/* Moved most comgen functions to comgen_odbc.c and changed other comgen api
/* fucntions to work with the new module.
/*
/* Revision 1.51  2005/01/24 22:10:28  vgu
/* bug 4254 fix: change memory leaking code
/*
/* Revision 1.50  2005/01/10 19:27:00  eburgess
/* Added functions to access contigchromosome and many other changes for comgen.
/*
/* Revision 1.49  2005/01/07 23:06:28  zwang
/* Added EndTransaction.
/*
/* Revision 1.48  2004/12/14 20:06:18  zwang
/* Added functions to delete all records from tables CHROM and DOMDB.
/*
/* Revision 1.47  2004/11/10 18:36:31  eburgess
/* Added code for sourcefile functions and other functions to handle the
/* division. Bug 1193
/*
/* Revision 1.46  2004/11/05 19:22:59  hfeldman
/* Fixed duplicate line
/*
/* Revision 1.45  2004/10/27 18:32:45  eburgess
/* Added SearchTAXGIByKloodgeForNA() and SearchTAXGIByKloodgeForProt() for
/* comgen apis. Bug 2952
/*
/* Revision 1.44  2004/10/21 18:44:56  eburgess
/* Fixed typos in DeleteGikloodgeGi etc.
/*
/* Revision 1.43  2004/10/15 19:59:42  eburgess
/* Added in many functions to deal with contigkloodge and other complete
/* genome issues. Bug 2952
/*
/* Revision 1.42  2004/10/01 17:30:33  zwang
/* Added transaction-related functions. See bug 3441.
/*
/* Revision 1.41  2004/09/22 18:17:40  eburgess
/* Added SHoundFindAccessByGi() and EditREcordTAXGIByKloodge() for complete
/* genomes.
/*
/* Revision 1.40  2004/09/17 15:31:43  eburgess
/* Added functions for secondrefs in names and gikloodge in comgen.
/*
/* Revision 1.39  2004/09/02 15:51:36  zwang
/* Fixed bug in SearchPUBSEQ for Bug #3113.
/* In SearchDOMDB, assign 'n/a' to pdbid and chain if result is NULL.
/*
/* Revision 1.38  2004/08/17 18:49:11  zwang
/* Move SQLHandle create out from AppendRecordREDUND to InitCodeBase.
/*
/* Revision 1.37  2004/08/09 18:49:20  vgu
/* Fix core dump error re bug 1964
/*
/* Revision 1.36  2004/08/06 14:47:21  mmatan
/* Changed CharPtr database[50] to SQLCHAR database[50]
/*
/* Revision 1.35  2004/08/05 16:24:03  zwang
/* Correct typo.
/*
/* Revision 1.34  2004/08/05 16:20:20  zwang
/* Remove DB2 related code.
/*
/* Revision 1.33  2004/08/05 15:40:09  zwang
/* Move SQLHandle creations out from SearchASNDB and SearchSENDBforGI to InitCodeBase
/*
/* Revision 1.32  2004/07/28 20:57:24  eburgess
/* In SearchAccdb(), set pac->title to "n/a" instead of NULL to prevent
/* crashes in wwwseekgi.c.
/*
/* Revision 1.31  2004/07/23 20:10:00  vgu
/* remove debugging code
/*
/* Revision 1.30  2004/07/19 21:42:58  zwang
/* Remove C++ style comments
/*
/* Revision 1.29  2004/07/16 18:52:16  vgu
/* In AppendRecordMMDB(), free either  mmdb_asn.val or pMBLtmp->buf, but not both as they point to the same thing. Bug 2801
/*
/* Revision 1.28  2004/07/09 15:48:51  eburgess
/* Use TEXT_LENGTH define when setting the length of text fields.
/*
/* Revision 1.27  2004/06/24 16:21:51  zwang
/* Use Slri_ValNodeAdd** instead of ValNodeAdd** for better performance.
/*
/* Revision 1.26  2004/06/09 18:55:42  zwang
/* Fix the bug in getting asn1 from MMDB. See bug#2442.
/*
/* Revision 1.25  2004/06/02 21:39:58  zwang
/* MemFree CharPtr in pMBLtmp.
/*
/* Revision 1.24  2004/05/14 20:23:01  zwang
/* Make division length to 15.
/* Free SQLHANDLE in getTaxInfoIII.
/*
/* Revision 1.23  2004/05/03 16:12:07  eburgess
/* Use C-style comments in InitCodebase() to comment out autocommit and comment
/* out the check for success as well.
/*
/* Revision 1.22  2004/04/15 21:17:32  zwang
/* In order to speed up the update parser: (see bug #2085):
/* 1. remove setting AUTO_COMMIT_ON in all delete functions.
/* 2. when deleting from NUCPROT, delete records with matched gin or matched gia in two separated queries instead of using 'OR' condition in one query. (This is the key to speed up.)
/* 3. add functions for SQL Transaction.
/*
/* Revision 1.21  2004/03/17 20:04:24  eburgess
/* Added SetCodeBaseAutoIdxTrue() stub for compatability to codebase.
/*
/* Revision 1.20  2004/03/15 19:48:38  eburgess
/* Added functions OpenREDUNDNoIdx(), ReindexREDUND() and
/* SetCodeBaseFlagsExclNoids() to mirror Codebase functions.
/*
/* Revision 1.19  2004/03/15 17:45:57  zwang
/* Added function to get the largest mmdbid
/*
/* Revision 1.18  2004/02/27 17:27:33  zwang
/* Sped up SHoundGetKloodgeFromAccess.
/* Added function SHoundFindGiByAcc (similar to SHoundFindAcc in intrez.c) to speed up the call in comgen.c
/*
/* Revision 1.17  2004/02/18 21:12:38  zwang
/* Fixed bug in DeleteGIinSENDB. Set auto-commit on for all delete functions.
/*
/* Revision 1.16  2004/02/18 17:19:50  zwang
/* Move handle creation for deleting from Delete functions to Init functions.
/*
/* Revision 1.15  2004/02/12 18:46:59  zwang
/* Speed up getting records by record number in ACCDB, REDUND and DOMDB.
/*
/* Revision 1.14  2004/02/11 20:29:20  zwang
/* Added DeleteGIinNUCPROT.
/* Got rid of warnings of SQLBindCol.
/*
/* Revision 1.13  2004/02/06 15:11:28  zwang
/* Fixed wrong free(pSQL_stmt).
/*
/* Revision 1.12  2004/02/03 22:12:21  zwang
/* Fixed bugs in functions EditRecordACCDBByGI, EditRecordTAXGIByGI and EditRecordDOMDBByDOMID:
/*  When flag overwrite is TRUE, overwrite data fields in DB only if supplied values are valid.
/* For the above 3 functions, move handler creations to corresponding Init functions for performance concern.
/*
/* Revision 1.11  2004/01/27 21:19:16  zwang
/* 1. Fixed the bug where extra record is inserted. For example:
/* gi       muid      pmid
/* 385254   93152451  8427800
/* 385254   93152451        0
/* The second record should not be in the database. See bug report 1357.
/* 2. Added more info in log when appending to ACCDB and ASNDB.
/*
/* Revision 1.10  2004/01/21 22:09:24  zwang
/* Removed functions copied from intrez.c
/* Include intrez.h
/* Added functions called in intrez.c (they simple call some existing functions.)
/*
/* Revision 1.9  2004/01/15 19:28:04  zwang
/* Speed up inserting records by moving handle creations in AppendRecordPUBSEQ to InitPUBSEQ and changing the code accordingly.
/*
/* Revision 1.8  2004/01/14 16:03:55  zwang
/* Added namelow for accdb. It was missing before.
/*
/* Revision 1.7  2004/01/13 22:28:48  zwang
/* Added more functions from intrez.c used in addgoid.c
/*
/* Revision 1.6  2004/01/13 20:35:57  zwang
/* Changed return values to be the same as in CodeBase version.
/* Added more functions from CodeBase version and intrez.c
/* Use generic database name.
/* */





