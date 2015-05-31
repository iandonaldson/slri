/*  $Id: taxdb_odbc.c,v 1.9 2005/04/26 14:32:02 rcavero Exp $
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


  PROGRAM:


  REVISION/CONTRIBUTION NOTES:

  July 10, 2001  Original MSH Revision.
                Hogue Lab - University of Toronto Biochemistry
                Department and the Samuel Lunenfeld Research
                Institute, Mount Sinai Hospital
                http://bioinfo.mshri.on.ca  hogue@mshri.on.ca
                ftp://ftp.mshri.on.ca/


  PUBLICATION TO CITE:


******************************************************************************************/

/**
** @file	taxdb_odbc.c
** 
** @brief	implementation of taxdb_odbc.h
** @authors	Ruth Isserlin, Gary Bader, Christopher W.V. Hogue Minzhi Lu
** @version	2.0
** @date	Jun 04, 2003
**
*/


#include "taxdb_odbc.h"

/***************************************************************************************
**	Global variables used for all functions
***************************************************************************************/

/* ODBC variables used in this file */

/* henv and hdbc defined in slri_odbc.h */

static SQLHANDLE 		hstmt_tax, hstmt_div, hstmt_gcode, hstmt_del, hstmt_merge;

/* handle error message */
static SQLRETURN                V_OD_erg;                      /* result of functions */

static SQLCHAR                  V_OD_stat[SQL_SQLSTATE_SIZE +1];                 /* Status SQL */
static SQLCHAR                  V_OD_msg[SQL_MAX_MESSAGE_LENGTH + 1];
static SQLINTEGER               V_OD_err;
static SQLSMALLINT              V_OD_mlen;

/*** for testing **/
static Boolean 			testing = TRUE;


/* global stuct used in SQLBindParameter */
/* for table taxdb */
static struct
{
	SQLINTEGER id;
}taxdb;
struct {
	Int4 len;
	CharPtr val;
}taxdb_asn;


/* for table divdb */
static struct
{
	SQLINTEGER id;
}divdb;

/* for table gcodedb */
static struct
{
	SQLINTEGER id;
}gcodedb;

/* for table deldb */
static struct
{
	SQLINTEGER id;
}deldb;

/* for table mergedb */
static struct
{
	SQLINTEGER oldid;
	SQLINTEGER newid;
}mergedb;


/* SeqHound Global Variables */
SQLCHAR database[50];


/************************************************************************/
/* 		Taxonomy database functions 				*/
/************************************************************************/

/*Opens all taxonomy database - returns TRUE if any database was created*/
Boolean OpenAllTaxDBs(Boolean Create)
{
  return TRUE;
}

/* 
** forms a connection to the database and set up handles for the
** locus link inserts.
**
** returns TRUE if connection is successful, FALSE otherwise
*/
Boolean
InitCodeBaseTax(void)
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
      ErrPostEx(SEV_ERROR, 0, 0, "[InitCodeBaseTax]: Could not find username in initialization file.");
      exit(1);
   }
   if( GetAppParam("intrez", "datab", "password", NULL, (Char*)passwd, sizeof(char) * 10) <= 0 )
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitCodeBaseTax]: Could not find pasword in initialization file.");
      exit(1);
   }
   if( GetAppParam("intrez", "datab", "dsn", NULL, (Char*)dsn, sizeof(char) * 10) <= 0 )
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitCodeBaseTax]: Could not find dsn in initialization file.");
      exit(1);
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
      ErrPostEx(SEV_ERROR,0,0, "[TAX_InitCodeBase]: Cannot InitBindParameterforInsert. SQLRETURN=%ld", V_OD_erg);
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

   /* table TAXDB */
   sprintf((char *)pSQL_stmt, "INSERT into %s.taxdb(tid, asn) values(?,?)", database);
   V_OD_erg = CreateHandle(&hstmt_tax,pSQL_stmt,hdbc,&henv);
   if (V_OD_erg != SQL_SUCCESS) {
      ErrPostEx(SEV_ERROR, 0, 0, "Error CreateHandle for taxdb");
	  return FALSE;
   }
   V_OD_erg = SQLBindParameter(hstmt_tax, 1, SQL_PARAM_INPUT, SQL_C_LONG,SQL_INTEGER, 0, 0, &taxdb.id, 0, NULL);
   if (V_OD_erg != SQL_SUCCESS) {
      ErrPostEx(SEV_ERROR, 0, 0, "Error: unable to bind parameter 1 for taxdb.");
	  return FALSE;
   }
   /* NOTE: value of asn1 bound in later function. */


   /* table DIV */
   sprintf((char *)pSQL_stmt, "INSERT into %s.divdb(did, asn) values(?,?)", database);
   V_OD_erg = CreateHandle(&hstmt_div, pSQL_stmt, hdbc,&henv);
   if (V_OD_erg != SQL_SUCCESS) {
      ErrPostEx(SEV_ERROR, 0, 0, "Error CreateHandle for div");
	  return FALSE;
   }

   V_OD_erg = SQLBindParameter(hstmt_div, 1, SQL_PARAM_INPUT, 
		SQL_C_LONG,SQL_INTEGER, 0, 0, &divdb.id, 0, NULL);
   if (V_OD_erg != SQL_SUCCESS) {
      ErrPostEx(SEV_ERROR, 0, 0, "Error: unable to bind parameter 1 for divdb.");
	  return FALSE;
   }
   /* NOTE: value of asn1 bound in later function. */


   /* table GCODEDB */
   sprintf((char *)pSQL_stmt, "INSERT INTO %s.gcodedb(gcid, asn) VALUES(?,?)", database);
   V_OD_erg = CreateHandle(&hstmt_gcode, pSQL_stmt, hdbc,&henv);
   if (V_OD_erg != SQL_SUCCESS) {
      ErrPostEx(SEV_ERROR, 0, 0, "Error CreateHandle for gcodedb");
	  return FALSE;
   }

   V_OD_erg = SQLBindParameter(hstmt_gcode, 1, SQL_PARAM_INPUT, 
		SQL_C_LONG,SQL_INTEGER, 0, 0, &gcodedb.id, 0, NULL);
   if (V_OD_erg != SQL_SUCCESS) {
      ErrPostEx(SEV_ERROR, 0, 0, "Error: unable to bind parameter 1 for gcodedb.");
	  return FALSE;
   }
   /* NOTE: value of asn1 bound in later function. */

   /* table DEL */
   sprintf((char *)pSQL_stmt, "INSERT into %s.del(tid) values(?)", database);
   V_OD_erg = CreateHandle(&hstmt_del, pSQL_stmt, hdbc,&henv);
   if (V_OD_erg != SQL_SUCCESS) {
      ErrPostEx(SEV_ERROR, 0, 0, "Error CreateHandle for del");
	  return FALSE;
   }

  	V_OD_erg = SQLBindParameter(hstmt_del, 1, SQL_PARAM_INPUT, 
		SQL_C_LONG,SQL_INTEGER, 0, 0, &deldb.id, 0, NULL);
   if (V_OD_erg != SQL_SUCCESS) {
      ErrPostEx(SEV_ERROR, 0, 0, "Error: unable to bind parameter 1 for del.");
	  return FALSE;
   }

   /* table MERGE */
   sprintf((char *)pSQL_stmt, "INSERT into %s.merge(otid, ntid) values(?,?)", database);
   V_OD_erg = CreateHandle(&hstmt_merge, pSQL_stmt, hdbc,&henv);
   if (V_OD_erg != SQL_SUCCESS) {
      ErrPostEx(SEV_ERROR, 0, 0, "Error CreateHandle for merge");
	  return FALSE;
   }
	
  	V_OD_erg = SQLBindParameter(hstmt_merge, 1, SQL_PARAM_INPUT, 
		SQL_C_LONG,SQL_INTEGER, 0, 0, &mergedb.oldid, 0, NULL);
   if (V_OD_erg != SQL_SUCCESS) {
      ErrPostEx(SEV_ERROR, 0, 0, "Error: unable to bind parameter 1 for merge.");
	  return FALSE;
   }
  	V_OD_erg = SQLBindParameter(hstmt_merge, 2, SQL_PARAM_INPUT, 
		SQL_C_LONG,SQL_INTEGER, 0, 0, &mergedb.newid, 0, NULL);
   if (V_OD_erg != SQL_SUCCESS) {
      ErrPostEx(SEV_ERROR, 0, 0, "Error: unable to bind parameter 2 for merge.");
	  return FALSE;
   }

   return TRUE;
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
CloseCodeBaseTax(void)
{

	/* free the statment handle */
	SQLFreeStmt(hstmt_tax, SQL_RESET_PARAMS);
	SQLFreeStmt(hstmt_div, SQL_RESET_PARAMS);
	SQLFreeStmt(hstmt_gcode, SQL_RESET_PARAMS);
	SQLFreeStmt(hstmt_del, SQL_RESET_PARAMS);
	SQLFreeStmt(hstmt_merge, SQL_RESET_PARAMS);


	/*Free the database handles used to communicate with the database*/
	V_OD_erg = SQLDisconnect(hdbc);
    if (V_OD_erg != SQL_SUCCESS) {
       ErrPostEx(SEV_ERROR, 0, 0, "Error: failed to disconnect");
	   return FALSE;
    }

	/*free the connnection handle */
	V_OD_erg= SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
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
Boolean StartTransactionTax(void)
{
        return SetAutoCommitOff(hdbc);
}

/****************************************************************/
/* Given an SQL handle, commit                                  */
/****************************************************************/
Boolean CommitTransactionTax(void)
{
        return CommitChanges(hdbc);
}

/****************************************************************/
/* Given an SQL handle, roll back                               */
/****************************************************************/
Boolean RollBackTransactionTax(void)
{
        return RollBackChanges(hdbc);
}

/*Close all taxonomy databases*/
void CloseAllTaxDBs(void)
{

}

/*Open the taxonomy database*/
Boolean 
OpenTaxDB(Boolean create)
{
	return InitCodeBaseTax();
}

/*Close the taxonomy database*/
Boolean CloseTaxDB(void)
{
	return CloseCodeBaseTax();
}

/*Pack the TAX DB*/
void DB_PackTAXDB()
{

}

/*Pack the DIV DB*/
void DB_PackDIVDB()
{

}

/*Pack the GCODE DB*/
void DB_PackGCODEDB()
{

}

/*Pack the DEL DB*/
void DB_PackDELDB()
{

}

/*Pack the MERGE DB*/
void DB_PackMERGEDB()
{

}

/* Pack all of the taxonomy databases */
void DB_PackAllTaxDB(void)
{

}

static Int2 
deleteRec(SQLINTEGER id)
{

	SQLHANDLE V_OD_hstmt;

    CharPtr pSQLstmt = (char *) malloc(sizeof(Char) * 150);
    if (pSQLstmt == NULL)
    {
      ErrPostEx(SEV_ERROR, 0, 0, "[DB_ReplaceTaxRec]: Could not allocate space");
      free(pSQLstmt);
      return -1;
    }
   sprintf((char *)pSQLstmt, "delete from %s.taxdb where tid = ?", database);

   V_OD_erg =	CreateHandle(&V_OD_hstmt,pSQLstmt,hdbc,&henv);
   if (V_OD_erg != SQL_SUCCESS) {
      ErrPostEx(SEV_ERROR, 0, 0, "Error CreateHandle for taxdb");
	  return -1;
   }
   free(pSQLstmt);

  	V_OD_erg = SQLBindParameter(V_OD_hstmt, 1, SQL_PARAM_INPUT, 
		SQL_C_LONG,SQL_INTEGER, 0, 0, &id, 0, NULL);
   if (V_OD_erg != SQL_SUCCESS) {
      ErrPostEx(SEV_ERROR, 0, 0, "Error: unable to bind parameter 2 for taxdb.");
	  return -1;
   }

   /* execute statment */
  	V_OD_erg = SQLExecute (V_OD_hstmt);
    if (V_OD_erg != SQL_SUCCESS)
    {
	  ErrPostEx(SEV_ERROR,0,0, "[deleteRec]: unable to execute delete statment. SQLRETURN=%ld", V_OD_erg);
	  LogSQLInfo(V_OD_hstmt);

       return -1;
    }

  	V_OD_erg = SQLFreeStmt(V_OD_hstmt, SQL_DROP); 

	return TAX_SUCCESS;
}


/************************************************************************/
/* 		APPEND RECORDS INTO DATABASE 				*/
/************************************************************************/
static Int2 
appendRec(SQLHANDLE hstmt, pABL Asnbuflen, Int2 mode)
{
	/* SK: NULL ptr check */
	if (mode==1 && Asnbuflen == NULL ) {
       ErrPostEx(SEV_ERROR, 0, 0, "Error: Null asn");
	   return -1;
	}

   	/*set up the input parameter */
	if (mode == 1) {		/* have asn1 field */

  		V_OD_erg = SQLBindParameter(hstmt, 2, SQL_PARAM_INPUT, 
		    	SQL_C_DEFAULT, SQL_LONGVARBINARY, 0, 0, 
			    Asnbuflen->buf, 0, (int*)&(Asnbuflen->len)); 
        if (V_OD_erg != SQL_SUCCESS) {
          ErrPostEx(SEV_ERROR, 0, 0, "Error: unable to bind parameter 2 as asn");
	      return -1;
		}

	} 

	/* execute statment */
  	V_OD_erg = SQLExecute (hstmt);
    if (V_OD_erg != SQL_SUCCESS)
    {
	  ErrPostEx(SEV_ERROR,0,0, "[appendRec]: unable to execute insert statment. SQLRETURN=%ld", V_OD_erg);
	  LogSQLInfo(hstmt);

       return -1;
    }


	return TAX_SUCCESS;
}

 

/**
** Append an SLRI-taxon record to the taxdb table 
**
** @param 	stp
** @return 	TAX_APPEND_SUCCESS if append is successful, 
**		-1 if primary key is already exist
**		-2 otherwise
*/
Int2 
DB_WriteTaxRec(SLRITaxonPtr stp)
{
   	pABL Asnbuflen=NULL;
	Int2 result = -2;
   
   	if(stp == NULL) {
     		ErrPostEx(SEV_INFO, 3, 0, 
			"DB_WriteTaxRec: Passed ASN.1 pointer is null.");
     		return(-2);
    	}


   	/*get the asn1 bioseq into a bytestore pointer so that 
	we can put it into a buffer.*/
  	Asnbuflen = (pABL)AssignASNMemChar((Pointer)stp, 
			(AsnWriteFunc) SLRITaxonAsnWrite);
	/* SK: NULL ptr check */
   	if(Asnbuflen == NULL) {
     		ErrPostEx(SEV_INFO, 3, 0, 
			"DB_WriteTaxRec: NULL Asnbuflen.");
     		return(-2);
    	}

	taxdb.id = stp->taxId;
	result = appendRec(hstmt_tax, Asnbuflen, 1);

  	FreeABL(Asnbuflen);
   	return result;

}

/**
** Append an SLRITaxonDiv record to the divdb table
**
** @param 	stdp
** @retur	TAX_APPEND_SUCCESS if append is successful, 
**		-1 if primary key is already exist
**		-2 otherwise
*/
Int2 
DB_WriteTaxDivRec(SLRITaxonDivPtr stdp)
{
    	pABL Asnbuflen=NULL;
	Int2 result;

    	if(stdp == NULL) {
     		ErrPostEx(SEV_INFO, 3, 0, 
			"DB_WriteTaxDivRec: Passed ASN.1 pointer is null.");
     		return(-2);
    	}

   	/*get the asn1 bioseq into a bytestore pointer 
		so that we can put it into a buffer.*/
  	Asnbuflen = (pABL)AssignASNMemChar((Pointer) stdp, 
				(AsnWriteFunc) SLRITaxonDivAsnWrite);
	/* SK: NULL ptr check */
   	if(Asnbuflen == NULL) {
     		ErrPostEx(SEV_INFO, 3, 0, 
			"DB_WriteTaxDivRec: NULL Asnbuflen.");
     		return(-2);
    	}

	divdb.id = stdp->div_id;
  	result = appendRec(hstmt_div, Asnbuflen, 1);  
	
  	FreeABL(Asnbuflen); 
   	return result;

}

/**
** Append an SLRITaxonGencode record to the gcodedb table
**
** @param	stgp
** @return	TAX_APPEND_SUCCESS if append is successful, 
**		-1 if primary key is already exist
**		-2 otherwise
*/
Int2 
DB_WriteTaxGenCodeRec(SLRITaxonGencodePtr stgp)
{
    	pABL Asnbuflen=NULL;
	Int2 result;

  	if (stgp == NULL) {
		ErrPostEx(SEV_INFO, 3, 0, 
			"DB_WriteTaxGenCodeRec: Passed ASN.1 pointer is null.");
		return(-2);
	}

  	Asnbuflen = (pABL)AssignASNMemChar((Pointer) stgp, 
		(AsnWriteFunc) SLRITaxonGencodeAsnWrite);
	/* SK: NULL ptr check */
   	if(Asnbuflen == NULL) {
     		ErrPostEx(SEV_INFO, 3, 0, 
			"DB_WriteTaxGenCodeRec: NULL Asnbuflen.");
     		return(-2);
    	}

	gcodedb.id = stgp->gencode_id; 
	result = appendRec(hstmt_gcode, Asnbuflen, 1); 
  	FreeABL(Asnbuflen); 
   	return result;

}


/**
** Append a deleted node record to the del table
**
** @param	taxId taxid
** @return	TAX_APPEND_SUCCESS if append is successful, 
**		-1 if primary key is already exist
**		-2 otherwise
*/
Int2 
DB_WriteTaxDelNodeRec(Int4 taxId)
{
	if(taxId < 0) {
		ErrPostEx(SEV_INFO, 3, 0, "DB_WriteTaxDelNodeRec: Passed taxId  < 0.");
		return(-1);
	}

	deldb.id = taxId; 
	return appendRec(hstmt_del, NULL, 2); 
}


/**
** Append a merged node record to the merge table
**
** @param	oldTaxId
** @param	newTaxId
** @return	TAX_APPEND_SUCCESS if append is successful, 
**		-1 if primary key is already exist
**		-2 otherwise
*/
Int2 
DB_WriteTaxMergedNodeRec(Int4 oldTaxId, Int4 newTaxId)
{

	if(oldTaxId < 0) {
		ErrPostEx(SEV_INFO, 3, 0, 
			"DB_WriteTaxMergedNodeRec: Passed oldTaxId  < 0.");
		return(-1);
	}
	if(newTaxId < 0) {
		ErrPostEx(SEV_INFO, 3, 0, 
			"DB_WriteTaxMergedNodeRec: Passed newTaxId  < 0.");
		return(-1);
	}

	mergedb.oldid = oldTaxId;
	mergedb.newid = newTaxId;
	return appendRec(hstmt_merge, NULL, 3); 
}



/************************************************************************/
/* 		UPDATE RECORDS INTO DATABASE 				*/
/************************************************************************/


/**
** update an SLRI-Taxon record in the taxdb table
**
** @param 	stp
** @return	TAX_UPDATE_SUCCESS if updating is successful, 
**		-1 if input data is NULL
**		-2 otherwise 
** 
*/
Int2 
DB_ReplaceTaxRec(SLRITaxonPtr stp)
{

	/************************************************
	 * problem of using "update". 
	 * delete the record then add back with new asn 
	 ************************************************/
	SQLINTEGER key= (SQLINTEGER) (stp->taxId);

	V_OD_erg = deleteRec(key);
	if ( V_OD_erg!= TAX_SUCCESS) {
      ErrPostEx(SEV_ERROR, 0, 0, "[DB_ReplaceTaxRec]: Could not delete old record");
      return -1;
	}

   	/*get the asn1 bioseq into a bytestore pointer 
		so that we can put it into a buffer.*/
   	pABL Asnbuflen=NULL;
  	Asnbuflen = (pABL)AssignASNMemChar((Pointer) stp, 
		(AsnWriteFunc) SLRITaxonAsnWrite);

    taxdb.id=stp->taxId;


	V_OD_erg = appendRec(hstmt_tax, Asnbuflen, 1);
	if (V_OD_erg != TAX_SUCCESS) {
      ErrPostEx(SEV_ERROR, 0, 0, "[DB_ReplaceTaxRec]: Could not append new record");
      return -1;
	}

	return TAX_SUCCESS;
	
	/**** old way of doing it. use "update" directly. error ****/

/*	SQLHANDLE V_OD_hstmt;
   	pABL Asnbuflen=NULL;

    CharPtr pSQLstmt = (char *) malloc(sizeof(Char) * 150);
    if (pSQLstmt == NULL)
    {
      ErrPostEx(SEV_ERROR, 0, 0, "[DB_ReplaceTaxRec]: Could not allocate space");
      free(pSQLstmt);
      return -1;
    }
   sprintf((char *)pSQLstmt, "update %s.taxdb set asn=? where tid = ?", database);

   if(stp == NULL) {
     		ErrPostEx(SEV_INFO, 3, 0, 
			"DB_WriteTaxRec: Passed ASN.1 pointer is null.");
     		return(-1);
    	}
   
   V_OD_erg =	CreateHandle(&V_OD_hstmt,pSQLstmt,hdbc,&henv);
   if (V_OD_erg != SQL_SUCCESS) {
      ErrPostEx(SEV_ERROR, 0, 0, "Error CreateHandle for taxdb");
	  return -1;
   }
   free(pSQLstmt);

   	//get the asn1 bioseq into a bytestore pointer 
	//	so that we can put it into a buffer.
  	Asnbuflen = (pABL)AssignASNMemChar((Pointer) stp, 
		(AsnWriteFunc) SLRITaxonAsnWrite);
  
	// SK: NULL ptr check 
   	if(Asnbuflen == NULL) {
     		ErrPostEx(SEV_INFO, 3, 0, 
			"DB_ReplaceTaxRec: NULL Asnbuflen.");
     		return(-2);
    	}
   	
  	V_OD_erg = SQLBindParameter(V_OD_hstmt, 1, SQL_PARAM_INPUT, 
		SQL_C_DEFAULT,SQL_LONGVARBINARY, 0, 0, 
		Asnbuflen->buf, 0, (long*)&(Asnbuflen->len)); 
   if (V_OD_erg != SQL_SUCCESS) {
      ErrPostEx(SEV_ERROR, 0, 0, "Error: unable to bind parameter 1 for taxdb.");
	  return -1;
   }
  	V_OD_erg = SQLBindParameter(V_OD_hstmt, 2, SQL_PARAM_INPUT, 
		SQL_C_LONG,SQL_INTEGER, 0, 0, &stp->taxId, 0, NULL);
   if (V_OD_erg != SQL_SUCCESS) {
      ErrPostEx(SEV_ERROR, 0, 0, "Error: unable to bind parameter 2 for taxdb.");
	  return -1;
   }

  	//execute statment 
   V_OD_erg = SQLExecute(V_OD_hstmt);
    if (V_OD_erg != SQL_SUCCESS)
    {
	  ErrPostEx(SEV_ERROR,0,0, "[DB_ReplaceTaxRec]: unable to execute update statment. taxid=%ld", stp->taxId);
	  LogSQLInfo(V_OD_erg);

       return -1;
    }
  	FreeABL(Asnbuflen);

  	V_OD_erg = SQLFreeStmt(V_OD_hstmt, SQL_DROP); 

   	return TAX_SUCCESS;

  */

}


/************************************************************************/
/* 		SEARCH RECORDS FROM DATABASE 				*/
/************************************************************************/
/* update current node's parent node
** return 	0 if updating is successful
** 		-1 if parent Taxid = current node's taxid
**		-2 if cannot find current node/parent node in taxdb
*/	
static Int4
updateTaxdb(CharPtr pcASN, Int4 bloblen) {
	SLRITaxonPtr stp = NULL, stpParent = NULL;
	Int4 parentTaxId = 0;

	
      	stp = (SLRITaxonPtr) GetASNMemChar(pcASN, (AsnReadFunc) SLRITaxonAsnRead,bloblen);

	if(stp==NULL) {
		ErrPostEx(SEV_INFO, 3, 0, 
			"updateTaxdb: Call to GetASNMemMemo failed for record after Taxonomy ID %ld.", parentTaxId);
		return(-2);
	}
	parentTaxId = stp->parent_taxId;
	if(parentTaxId==stp->taxId) {
		stp = SLRITaxonFree(stp);
		return -1;	/*don't allow any circles in the tree*/
	}
	stpParent = DB_GetTaxRec(parentTaxId);
	if(stpParent==NULL) {
		ErrPostEx(SEV_INFO, 0, 0, 
			"updateTaxdb: DB_GetTaxRec failed for Tax ID %ld.", parentTaxId);
		stp = SLRITaxonFree(stp);
		return(-2);
	}
	Misc_AddInt2SortedList(&(stpParent->children_taxId), stp->taxId, 0, TRUE);
	DB_ReplaceTaxRec(stpParent);
	stp = SLRITaxonFree(stp);
	stpParent = SLRITaxonFree(stpParent);
	
	return 0;
}

/** 
** search the record
** param:
**	mode: 	0, 1, 3 - search for ASN1 data, while mode 3 include updating taxdb
**	mode: 	2 - search for integer data
**
*/			
static Int4 
searchRec(CharPtr selectPart, CharPtr fromPart, CharPtr wherePart, SQLINTEGER inputID, Int2 mode, CharPtr* asnData)
{
	SQLHANDLE V_OD_hstmt;
   	ValNodePtr vnp=NULL;
   	Char search_stmt[MAXCHARLEN];

	SQLINTEGER outputID; 
   	CharPtr pcASN = NULL;
   	Int4 pbbvalue=0;
   	SQLINTEGER blobloc=0;
   	Int4 bloblen=0,indicator=0;

	if (mode == 3){	/* update taxdb */
		sprintf(search_stmt, "select asn from %s.taxdb", database); 
	} else {
		sprintf(search_stmt, "select %s from %s.%s where %s = ?", 
			IsNullStr(selectPart), database, IsNullStr(fromPart), IsNullStr(wherePart));
	}

   	CreateHandle(&V_OD_hstmt,search_stmt,hdbc,&henv);

	if (mode != 3) { /* search by ID */
		/* bind input parameter, we all search by ID */
  		V_OD_erg = SQLBindParameter(V_OD_hstmt, 1, SQL_PARAM_INPUT, SQL_C_LONG,
				SQL_INTEGER, 0, 0, &inputID, 0, NULL);
       if (V_OD_erg != SQL_SUCCESS) {
          ErrPostEx(SEV_ERROR, 0, 0, "Error: unable to bind parameter 1");
	      return -1;
	   }
	}

	/* execute */
  	V_OD_erg = SQLExecute(V_OD_hstmt);
    if (V_OD_erg != SQL_SUCCESS)
    {
	  ErrPostEx(SEV_ERROR,0,0, "[searchRec]: unable to execute select statment");
	  LogSQLInfo(V_OD_hstmt);

       return -1;
    }

	/* bind output value */
	if (mode == 2) {			/* output is integer */
  		V_OD_erg = SQLBindCol(V_OD_hstmt,1,SQL_C_LONG,
				&outputID, 0, NULL); 
       if (V_OD_erg != SQL_SUCCESS) {
          ErrPostEx(SEV_ERROR, 0, 0, "Error: unable to bind col 1");
	      return -1;
	   }
	} else {				/* output is ASN1 */
  		V_OD_erg = SQLBindCol(V_OD_hstmt,1, SQL_C_BINARY, 
				&blobloc,0,(int*)&pbbvalue); 
       if (V_OD_erg != SQL_SUCCESS) {
          ErrPostEx(SEV_ERROR, 0, 0, "Error: unable to bind col 1");
	      return -1;
	   }
	} 					
  
  	V_OD_erg = SQLFetch(V_OD_hstmt);
  	if(V_OD_erg == SQL_NO_DATA_FOUND){
		if (mode == 1) { 	/* taxdb */
      			/*try looking in merged database before giving up*/
			Int4 newTaxId = DB_GetTaxMergedNodeRecNewTIDByOldTID(inputID);
			if(newTaxId<0) {
				ErrPostEx(SEV_INFO, 0, 0, 
				"Taxonomy ID: %ld not found in database.", inputID);
				return -1;
			} else{
	        		ErrPostEx(SEV_INFO, 0, 0, 
				"Taxonomy ID:  found with another id : %ld.", newTaxId);
				return -1;
			}
		}
 
    		ErrPostEx(SEV_INFO, 0, 0, " ID: %ld not found.", inputID);
		return -1;

  	} 

	/*CHECKERR(V_OD_hstmt, "SQLFetch failed"); */
	while (V_OD_erg == SQL_SUCCESS || V_OD_erg == SQL_SUCCESS_WITH_INFO){

		if (mode == 0 || mode == 1 || mode ==3) {	/* fetch ASN1 */
			bloblen = (Int4) pbbvalue;

			if (bloblen > 0) {

      				/* allocate a buffer big enough to store the blob. */ 
      				pcASN = (CharPtr) MemNew((size_t) bloblen);
      				/*read the blob from the database */
      				V_OD_erg = SQLGetData(V_OD_hstmt,1,SQL_C_BINARY,
					(SQLPOINTER)pcASN, bloblen,(int*)&indicator);
                   if (V_OD_erg != SQL_SUCCESS)
				   {
	                  ErrPostEx(SEV_ERROR,0,0, "[searchRec]: unable to execute SQLGetData statment");
	                  LogSQLInfo(V_OD_hstmt);

                      return -1;
				   }

				if (mode == 3) {
					/* update the taxdb */
					int ret = updateTaxdb(pcASN, bloblen);
					if (ret==-1 || ret == 0) {
						V_OD_erg = SQLFetch(V_OD_hstmt);
					} else { /* error occur */
						return -2;
					}
				} else { 
					break;
				}
			}
			else {
				return 0;
			}
		} else {		/* fetch integer */
			break;
		}
   
	} /* end of while */
 

  	V_OD_erg = SQLFreeStmt(V_OD_hstmt, SQL_DROP);
	if (mode == 0 || mode == 1) {	/* return ASN1 */
		/* return ASN1 data */
		*asnData= pcASN;
		return bloblen;
	} else if (mode == 3) {
		return 0;
	} else {
		return outputID;
	}
}

/**
** search an SLRI-taxon record from the taxdb table
**
** @param 	taxId taxid
**
** @return 	a struct that holds the record
** 		or NULL if not found
*/
SLRITaxonPtr 
DB_GetTaxRec(Int4 taxId)
{
   	SLRITaxonPtr stp=NULL;
	CharPtr ptr = NULL;
	Int4 length = searchRec("asn", "taxdb", "tid", (SQLINTEGER) taxId, 1, &ptr);

	if (length > 0) {
		stp = (SLRITaxonPtr) GetASNMemChar(ptr, (AsnReadFunc) 
				SLRITaxonAsnRead,length);
		if(stp==NULL) {
			ErrPostEx(SEV_INFO, 3, 0, 
			"DB_GetTaxRec: Call to GetASNMemMemo failed Taxonomy ID %ld.", taxId);
			return(NULL);
		}
      		if (ptr) MemFree(ptr);
   		return(stp);
	} else
		return NULL;

}

/**
** Search an SLRITaxonDiv record from the divdb table
**
** @param 	divId divid
** @return	a struct that holds the record
**		NULL if not found
*/
SLRITaxonDivPtr 
DB_GetTaxDivRec(Int4 divId)
{
   	SLRITaxonDivPtr stdp=NULL;
	CharPtr ptr = NULL;
	Int4 length = searchRec("asn", "divdb", "did", (SQLINTEGER) divId, 0, &ptr);
	
	if (length > 0) {
      		stdp = (SLRITaxonDivPtr) GetASNMemChar(ptr, 
			(AsnReadFunc) SLRITaxonDivAsnRead,length);
		if(stdp==NULL) {
			ErrPostEx(SEV_INFO, 3, 0, 
			"DB_GetTaxDivRec: GetASNMemMemo failed Taxonomy Division ID %ld.", divId);
			return(NULL);
		}
      		if (ptr) MemFree(ptr);
   		return(stdp);
	} else
		return NULL;
}


/**
** Search an SLRITaxonGencode record from the gcodedb table
**
** @param	genCodeId gcodeid
** @return	a struct that holds the record
**		NULL if not found
*/
SLRITaxonGencodePtr 
DB_GetTaxGenCodeRec(Int4 genCodeId)
{
	SLRITaxonGencodePtr stgp=NULL;
	CharPtr ptr = NULL;
	Int4 length = searchRec("asn", "gcodedb", "gcid", (SQLINTEGER) genCodeId, 0, &ptr);

	if (length < 0) return NULL;

	stgp = (SLRITaxonGencodePtr) GetASNMemChar(ptr, 
			(AsnReadFunc) SLRITaxonGencodeAsnRead,length);
	if(stgp==NULL) {
		ErrPostEx(SEV_INFO, 3, 0, 
		"DB_GetTaxGenCodeRec: GetASNMemCahr failed Taxonomy Genetic Code ID %ld.", genCodeId);
      		if (ptr) MemFree(ptr);
		return(NULL);
	}

      	if (ptr) MemFree(ptr);
   	return(stgp);

}


/**
** Search a deleted node record from the del table
** 
** @param	taxId taxid
** @return	the deleted taxid if searching is successful
**		-1 if can not find
*/
Int4 
DB_GetTaxDelNodeRec(Int4 taxId)
{
	return(searchRec("tid", "del", "tid", (SQLINTEGER) taxId, 2, NULL));
 
}


/**
** Search an old Tax ID given a new Tax ID from the merged node database 
**
** @param	newTaxId
** @return	the old tax id
**		-1 if not found
*/
/* this function is not used in API */
Int4 
DB_GetTaxMergedNodeRecOldTIDByNewTID(Int4 newTaxId)
{
	return(searchRec("otid", "merge", "ntid", (SQLINTEGER) newTaxId, 2, NULL));

}


/**
** Search a new Tax ID given an old Tax ID from the merged node database 
** 
** @param	oldTaxId
** @return	a new tax id
*/
Int4 
DB_GetTaxMergedNodeRecNewTIDByOldTID(Int4 oldTaxId)
{
	return(searchRec("ntid", "merge", "otid", (SQLINTEGER) oldTaxId, 2, NULL));
}


/* Function for usein importtaxdb
** This function takes each node in the taxdb database.  It gets its parent node
** and then the current node is added to the list of children of the parent node  
** that was retrieved.  This updated parent node is then replaced in the database.
** return 	0 if updating successful
**		-1 if parent node's taxid = current node's taxid
**		-2 if cannot find parent node/current node in taxdb 
*/
/* this function is not in use ifdef PARSE_IN_MEMORY. 
10 times faster if pasring in memory.*/
Int2 TaxIDDisk_BuildChildrenNodeList(void)
{
	return (0);
}


/************************************************************************/
/* 		DELETE RECORDS FROM DATABASE 				*/
/************************************************************************/
/******************************************************************
* delete all records in the TAX tables:
* taxdb, gcode, merge, del, div 
*******************************************************************/
Boolean DeleteAllRecordsInTAX(void)
{
   CharPtr apchTables[5] = {"taxdb", "gcodedb", "merge", "del", "divdb"};
   SQLHANDLE hstmt;
   Int2 index=0;
   Int2 sql_return = SQL_SUCCESS;

   CharPtr delete_stmt = (char *) malloc(sizeof(Char) * 150);
   if (delete_stmt == NULL)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[DeleteAllRecordsInTAX]: Could not allocate space");
     free(delete_stmt);
     return FALSE;
   }

  for (index=0; index<5; index++) {

     sprintf((char *)delete_stmt, "delete from %s.%s;", database, apchTables[index]);

     if (CreateHandle(&hstmt, delete_stmt, hdbc, &henv) != SQL_SUCCESS)
     {
        ErrPostEx(SEV_ERROR, 0, 0, "[DeleteAllRecordsInTAX]: Could not create handle to delete table %s", apchTables[index]);
        free(delete_stmt);
        return FALSE;
     }

     sql_return = SQLExecute(hstmt);
     /* execute statment */
     if (sql_return != SQL_SUCCESS)
     {
        ErrPostEx(SEV_ERROR,0,0, "[DeleteAllRecordsInTAX]:unable to execute delete statement for table %s.", apchTables[index]);
        LogSQLInfo(hstmt);
        SQLFreeStmt(hstmt, SQL_DROP);
        return FALSE;
     }

   }

   free(delete_stmt);
   SQLFreeStmt(hstmt, SQL_DROP);
   return TRUE;
}


/************************************************************************/
/* not in use                                 */
/**********************************************/

/* Delete an SLRI-taxon record from the database */
Int4 DB_DelTaxRec(Int4 taxId)
{
    return (0);
}

/* Undelete an SLRI-taxon record from the database */
Int4 DB_UnDelTaxRec(Int4 taxId)
{

	return(0);
}


/* Delete an SLRITaxonDiv record from the database */
Int4 DB_DelTaxDivRec(Int4 divId)
{
	return(0);
}

/* Undelete an SLRITaxonDiv record from the database */
Int4 DB_UnDelTaxDivRec(Int4 divId)
{

	return(0);
}

/* Delete an SLRITaxonGencode record from the database */
Int4 DB_DelTaxGenCodeRec(Int4 genCodeId)
{

	return(0);
}

/* Undelete an SLRITaxonGencode record from the database */
Int4 DB_UnDelTaxGenCodeRec(Int4 genCodeId)
{

	return(0);
}


/* Delete a deleted node record from the database */
Int4 DB_DelTaxDelNodeRec(Int4 taxId)
{

	return(0);
}

/* Undelete a deleted node record from the database */
Int4 DB_UnDelTaxDelNodeRec(Int4 taxId)
{

	return(0);
}


/* Delete a merged node record from the database by new Tax ID*/
Int4 DB_DelTaxMergedNodeRec(Int4 newTaxId)
{
	return(0);
}

/* Undelete a merged node record from the database by New Tax ID*/
Int4 DB_UnDelTaxMergedNodeRec(Int4 newTaxId)
{
	return(0);
}

