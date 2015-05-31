/**************************************************************************
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
** @file	goa_odbc.c
**
** @brief	implementation of goa_odbc.h
** @author	Renan A. CAVERO
** @version	2.0
** @date	Apr. 21, 2005
**
*/


#include "goa_odbc.h"

extern SQLHDBC hdbc;
extern SQLHENV henv;

/* SeqHound Global Variables */
extern SQLCHAR database[50];


Boolean InitGOA(void)
{
    SQLRETURN retcode;
    Char *pSQL_stmt; 
    Char *auxSQL;


    /* GOA_SearchGOIDFromGi allocate space for the select statement */

    auxSQL = "SELECT DISTINCT go FROM %s.goa_gigo WHERE gi=?";
    pSQL_stmt = (char*) malloc(sizeof(char) * (strlen(auxSQL) + strlen(database)));
 
    if(pSQL_stmt == NULL)
    {
	ErrPostEx(SEV_ERROR, 0, 0, "[InitGOA]: Could not allocate space for hstmtGoaGOIDFromGi.");
	return FALSE;
    }
    sprintf((char*)pSQL_stmt, auxSQL, database);
    /* Create Handle Statement, once handle exists we can free char* */
    retcode = CreateHandle(&hstmtGoaGOIDFromGi, pSQL_stmt, hdbc, &henv);
    if(retcode != SQL_SUCCESS)
    {
	ErrPostEx(SEV_ERROR, 0, 0, "[InitGOA]: Could not create stmt handle for hstmtGoaGOIDFromGi.");
	free(pSQL_stmt);
	return FALSE;
    }
    free(pSQL_stmt);
    /* Bind Parameters for search */
    retcode = SQLBindParameter(hstmtGoaGOIDFromGi, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &goa_gi, sizeof(goa_gi), NULL);
    if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
    {
	SQLFreeStmt(hstmtGoaGOIDFromGi, SQL_DROP);
	ErrPostEx(SEV_ERROR, 0, 0, "[InitGOA]: Could not bind parameter 1 for hstmtGoaGOIDFromGi.");
	return FALSE;
    }


    /* GOA_SearchGOIDFromRedundantGi allocate space for the select statement */

    auxSQL = "SELECT distinct e.go FROM goa_gigo e, %s.redund a, %s.redund b, %s.taxgi c, %s.taxgi d WHERE b.gi=e.gi AND b.rgroup = a.rgroup AND a.gi=c.gi AND b.gi=d.gi AND c.taxid=d.taxid AND a.gi =? order by e.go";
    pSQL_stmt = (char*) malloc(sizeof(char) * (strlen(auxSQL) + (strlen(database)*4)));
    if(pSQL_stmt == NULL)
    {
	ErrPostEx(SEV_ERROR, 0, 0, "[InitGOA]: Could not allocate space for hstmtGoaGOIDFromRedundantGi.");
	return FALSE;
    }
    sprintf((char*)pSQL_stmt, auxSQL, database, database, database, database);
    /* Create Handle Statement, once handle exists we can free char* */
    retcode = CreateHandle(&hstmtGoaGOIDFromRedundantGi, pSQL_stmt, hdbc, &henv);
    if(retcode != SQL_SUCCESS)
    {
	ErrPostEx(SEV_ERROR, 0, 0, "[InitGOA]: Could not create stmt handle for hstmtGoaGOIDFromRedundantGi.");
	free(pSQL_stmt);
	return FALSE;
    }
    free(pSQL_stmt);
    /* Bind Parameters for search */
    retcode = SQLBindParameter(hstmtGoaGOIDFromRedundantGi, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &goa_gi, sizeof(goa_gi), NULL);
    if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
    {
	SQLFreeStmt(hstmtGoaGOIDFromRedundantGi, SQL_DROP);
	ErrPostEx(SEV_ERROR, 0, 0, "[InitGOA]: Could not bind parameter 1 for hstmtGoaGOIDFromRedundantGi");
	return FALSE;
    }


   /* GOA_SearchGiFromGOID allocate space for the select statement */

    auxSQL = "SELECT distinct gi FROM %s.goa_gigo WHERE go=?";
    pSQL_stmt = (char*) malloc(sizeof(char) * ( strlen(auxSQL) + strlen(database)));
    if(pSQL_stmt == NULL)
    {
	ErrPostEx(SEV_ERROR, 0, 0, "[InitGOA]: Could not allocate space for hstmtGoaGiFromGOID.");
	return FALSE;
    }
    sprintf((char*)pSQL_stmt, auxSQL, database);
    /* Create Handle Statement, once handle exists we can free char* */
    retcode = CreateHandle(&hstmtGoaGiFromGOID, pSQL_stmt, hdbc, &henv);
    if(retcode != SQL_SUCCESS)
    {
	ErrPostEx(SEV_ERROR, 0, 0, "[InitGOA]: Could not create stmt handle for hstmtGoaGiFromGOID.");
	free(pSQL_stmt);
	return FALSE;
    }
    free(pSQL_stmt);
    /* Bind Parameters for search */
    retcode = SQLBindParameter(hstmtGoaGiFromGOID, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &goa_go, sizeof(goa_go), NULL);
    if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
    {
	SQLFreeStmt(hstmtGoaGiFromGOID, SQL_DROP);
	ErrPostEx(SEV_ERROR, 0, 0, "[InitGOA]: Could not bind parameter 1 for hstmtGoaGiFromGOID.");
	return FALSE;
    }


    /* GOA_SearchGiFromGOIDAndECode allocate space for the select statement */

    auxSQL = "SELECT DISTINCT gi FROM %s.goa_gigo WHERE go=? AND code=?";
    pSQL_stmt = (char*) malloc(sizeof(char) * (strlen(auxSQL) + strlen(database)));
    if(pSQL_stmt == NULL)
    {
	ErrPostEx(SEV_ERROR, 0, 0, "[InitGOA]: Could not allocate space for hstmtGoaGiFromGOIDAndECode.");
	return FALSE;
    }
    sprintf((char*)pSQL_stmt, auxSQL, database);    
    /* Create Handle Statement, once handle exists we can free */
    retcode = CreateHandle(&hstmtGoaGiFromGOIDAndECode, pSQL_stmt, hdbc, &henv);
    if(retcode != SQL_SUCCESS)
    {
	ErrPostEx(SEV_ERROR, 0, 0, "[InitGOA]: Could not create stmt handle for hstmtGoaGiFromGOIDAndECode.");
	free(pSQL_stmt);
	return FALSE;
    }
    free(pSQL_stmt); 
    /* Bind Parameters for search */
    retcode = SQLBindParameter(hstmtGoaGiFromGOIDAndECode, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &goa_go, 0, NULL);
    if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
    {
 	SQLFreeStmt(hstmtGoaGiFromGOIDAndECode, SQL_DROP);
	ErrPostEx(SEV_ERROR, 0, 0, "[InitGOA]: Could not bind parameter 1 for hstmtGoaGiFromGOIDAndECode.");
	return FALSE;
    }
    retcode = SQLBindParameter(hstmtGoaGiFromGOIDAndECode, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 0, 0, goa_ecode, strlen(goa_ecode), NULL);
    if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
    {
	SQLFreeStmt(hstmtGoaGiFromGOIDAndECode, SQL_DROP);
	ErrPostEx(SEV_ERROR, 0, 0, "[InitGOA]: Could not bind parameter 2 for hstmtGoaGiFromGOIDAndECode.");
	return FALSE;
    }


    /* GOA_SearchGOECodeFromGiAndGOID allocate space for the select statement */

    auxSQL = "SELECT code FROM %s.goa_gigo WHERE gi=? AND go=?";
    pSQL_stmt = (char*) malloc(sizeof(char) * ( strlen(auxSQL) + strlen(database)));
    if(pSQL_stmt == NULL)
    {
	ErrPostEx(SEV_ERROR, 0, 0, "[InitGOA]: Could not allocate space for hstmtGoaGOECodeFromGiAndGOID.");
	return FALSE;
    }
    sprintf((char*)pSQL_stmt, auxSQL , database);

    /* Create Handle Statement, once handle exists we can free char */
    retcode = CreateHandle(&hstmtGoaGOECodeFromGiAndGOID, pSQL_stmt, hdbc, &henv);
    if(retcode != SQL_SUCCESS)
    {
	ErrPostEx(SEV_ERROR, 0, 0, "[InitGOA]: Could not create stmt handle for hstmtGoaGOECodeFromGiAndGOID.");
	free(pSQL_stmt);
	return FALSE;
    }
    free(pSQL_stmt);  
    /* Bind Parameters for search */
    retcode = SQLBindParameter(hstmtGoaGOECodeFromGiAndGOID, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &goa_gi, 0, NULL);
    if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
    {
	SQLFreeStmt(hstmtGoaGOECodeFromGiAndGOID, SQL_DROP);
	ErrPostEx(SEV_ERROR, 0, 0, "[InitGOA]: Could not bind parameter 1.");
	return FALSE;
    }
    retcode = SQLBindParameter(hstmtGoaGOECodeFromGiAndGOID, 2, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &goa_go, 0, NULL);
    if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
    {	
	SQLFreeStmt(hstmtGoaGOECodeFromGiAndGOID, SQL_DROP);
	ErrPostEx(SEV_ERROR, 0, 0, "[InitGOA]: Could not bind parameter 2.");
	return SH_ERROR;
    }
  

    /* GOA_SearchGOIDFromGENEIDallocate space for the select statement */

    auxSQL = "SELECT distinct b.go FROM %s.goa_xdb a, %s.goa_gigo b WHERE a.id=b.xdb_id AND a.abbreviation='GENE' AND b.xref_key=?";
    pSQL_stmt = (char*) malloc(sizeof(char) * (strlen(auxSQL) + (strlen(database)*2) + 30 )); /* 30 = goa_seq_dbxref.xref_key */
    sprintf((char*)pSQL_stmt, auxSQL, database, database, goa_gene);
    /* Create Handle Statement, once handle exists we can free char* */
    retcode = CreateHandle(&hstmtGoaGOIDFromGENEID, pSQL_stmt, hdbc, &henv);
    if(retcode != SQL_SUCCESS)
    {
	ErrPostEx(SEV_ERROR, 0, 0, "[InitGOA]: Could not create stmt handle for hstmtGoaGOIDFromGENEID.");
	free(pSQL_stmt);
	return SH_ERROR;
    }
    free(pSQL_stmt);
    /* Bind Parameters for search */
    retcode = SQLBindParameter(hstmtGoaGOIDFromGENEID, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 0, 0, goa_gene, strlen(goa_gene), NULL);
    if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
    {
	SQLFreeStmt(hstmtGoaGiFromGOIDAndECode, SQL_DROP);
	ErrPostEx(SEV_ERROR, 0, 0, "[InitGOA]: Could not bind parameter 2 for hstmtGoaGiFromGOIDAndECode.");
	return FALSE;
    }


    /* GOA_SearchPMIDFromGiAndGOID allocate space for the select statement */

    auxSQL = "SELECT distinct h.xref_key "
	"FROM %s.goa_gigo e, %s.redund a, %s.redund b, %s.taxgi c, %s.taxgi d, "
	"%s.goa_seq_dbxref f, %s.goa_association g, %s.goa_reference h, %s.goa_xdb i "
	"WHERE b.gi=e.gi AND b.rgroup = a.rgroup AND a.gi=c.gi AND b.gi=d.gi "
	"AND c.taxid=d.taxid AND a.gi=? AND e.xref_key=f.xref_key "
	"AND e.xdb_id=f.goa_xdb_id AND f.id=g.goa_seq_dbxref_id "
	"AND g.id=h.goa_association_id AND g.go_id=? AND h.goa_xdb_id=i.id "
	"AND (i.abbreviation='PMID' or i.abbreviation='PUBMED');";
    pSQL_stmt = (char*) malloc(sizeof(char) * (strlen(auxSQL) + (strlen(database)*9)));
    if(pSQL_stmt == NULL)
    {
	ErrPostEx(SEV_ERROR, 0, 0, "[InitGOA]: Could not allocate space for hstmtPMIDFromGiAndGOID.");
	return FALSE;
    }
    sprintf((char*)pSQL_stmt, auxSQL, database, database, database, database, database, database, database, database, database);
    /* Create Handle Statement, once handle exists we can free char* */
    retcode = CreateHandle(&hstmtPMIDFromGiAndGOID, pSQL_stmt, hdbc, &henv);
    if(retcode != SQL_SUCCESS)
    {
	ErrPostEx(SEV_ERROR, 0, 0, "[InitGOA]: Could not create stmt handle for hstmtPMIDFromGiAndGOID.");
	free(pSQL_stmt);
	return FALSE;
    }
    free(pSQL_stmt);
    /* Bind Parameters for search */
    retcode = SQLBindParameter(hstmtPMIDFromGiAndGOID, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &goa_gi, 0, NULL);
    if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
    {
	SQLFreeStmt(hstmtPMIDFromGiAndGOID, SQL_DROP);
	ErrPostEx(SEV_ERROR, 0, 0, "[InitGOA]: Could not bind parameter 1");
	return FALSE;
    }
    retcode = SQLBindParameter(hstmtPMIDFromGiAndGOID, 2, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &goa_go, 0, NULL);
    if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
    {	

	SQLFreeStmt(hstmtPMIDFromGiAndGOID, SQL_DROP);
	ErrPostEx(SEV_ERROR, 0, 0, "[InitGOA]: Could not bind parameter 2");
	return FALSE;
    }
    return TRUE;
}


Int4 GOA_SearchGOIDFromGi(Int4 gi, ValNodePtr PNTR vnp)
{
    ValNodePtr vnpGOList = NULL;
    Int4 goid = 0;
    SQLRETURN retcode;                    /* return values for odbc layer      */

    if(vnp == NULL)
    {
	ErrPostEx(SEV_ERROR,0,0, "[GOA_SearchGOIDFromGi]: ValNodePtr is NULL.");
	return SH_ERROR;
    }
    if (gi <= 0) 
    {
	ErrPostEx(SEV_ERROR,0,0, "[GOA_SearchGOIDFromGi]: Invalid GI.");
	return SH_ERROR;
    }

    goa_gi = gi;

    /* execute the select statement */
    if( (retcode = SQLExecute(hstmtGoaGOIDFromGi)) != SQL_SUCCESS )
    {
	ErrPostEx(SEV_ERROR, 0, 0, "[GOA_SearchGOIDFromGi]: Could not execute select statement.");
	LogSQLInfo(hstmtGoaGOIDFromGi);
	SQLFreeStmt(hstmtGoaGOIDFromGi, SQL_CLOSE);
	return SH_ERROR;
    }

    /*   Bind Return Parameters from the search   */
    retcode = SQLBindCol(hstmtGoaGOIDFromGi, 1, SQL_C_SLONG, &goa_go, sizeof(goa_go), NULL);
    if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
    {
	SQLFreeStmt(hstmtGoaGOIDFromGi, SQL_CLOSE);
	ErrPostEx(SEV_ERROR, 0, 0, "[GOA_SearchGOIDFromGi]: Could not bind return parameter");
	return SH_ERROR;
    }

    /* fetch results and save in your linked list */
    SQLINTEGER fetchCount = 0;
    while( (retcode = SQLFetch(hstmtGoaGOIDFromGi) ==  SQL_SUCCESS)  || (retcode == SQL_SUCCESS_WITH_INFO) )
    {
	fetchCount++;

	if(retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
	{
	    ValNodeAddInt ( &vnpGOList, 0, goa_go);
	}
    }

    SQLFreeStmt(hstmtGoaGOIDFromGi, SQL_CLOSE);
    if (fetchCount == 0)
    {
	return SH_NODATA;
    }

    *vnp = vnpGOList;
    return SH_SUCCESS; 
}


Int4 GOA_SearchGOIDFromGiList(ValNodePtr giList, ValNodePtr PNTR vnp)
{
    ValNodePtr vnpGOList = NULL;
    Int4 goid = 0;
    SQLRETURN retcode;                    /* return values for odbc layer      */
    static SQLHANDLE hstmtGoaGOIDFromGiList;
    Char *pSQL_stmt;
    Char *auxSQL;
    CharPtr pstr = NULL;

    pstr = Misc_SaveValNodeListAsString(giList);
  
    if(vnp == NULL)
    {
	ErrPostEx(SEV_ERROR,0,0, "[GOA_SearchGOIDFromGiList]: ValNodePtr is NULL.");
	return SH_ERROR;
    }

    /* allocate space for the select statement */
    auxSQL = "select distinct go from %s.goa_gigo where gi IN(%s)";
    pSQL_stmt = (char*) malloc(sizeof(char) * (strlen(auxSQL) + strlen(database) + strlen(pstr) ));
 
    if(pSQL_stmt == NULL)
    {
	ErrPostEx(SEV_ERROR, 0, 0, "[GOA_SearchGOIDFromGiList]: Could not allocate space for hstmtGoaGOIDFromGi.");
	return SH_ERROR;
    }

    sprintf((char*)pSQL_stmt, auxSQL, database, pstr);
    /* Create Handle Statement, once handle exists we can free char* */
    retcode = CreateHandle(&hstmtGoaGOIDFromGiList, pSQL_stmt, hdbc, &henv);
    if(retcode != SQL_SUCCESS)
    {
	ErrPostEx(SEV_ERROR, 0, 0, "[GOA_SearchGOIDFromGiList]: Could not create stmt handle for hstmtGoaGOIDFromGi.");
	free(pSQL_stmt);
	return SH_ERROR;
    }
    free(pSQL_stmt);
    free(pstr);
    /* execute the select statement */
    if( (retcode = SQLExecute(hstmtGoaGOIDFromGiList)) != SQL_SUCCESS )
    {
	ErrPostEx(SEV_ERROR, 0, 0, "[GOA_SearchGOIDFromGiList]: Could not execute select statement.");
	LogSQLInfo(hstmtGoaGOIDFromGiList);
	SQLFreeStmt(hstmtGoaGOIDFromGiList, SQL_DROP);
	return SH_ERROR;
    }

    /*   Bind Return Parameters from the search   */
    retcode = SQLBindCol(hstmtGoaGOIDFromGiList, 1, SQL_C_SLONG, &goid, sizeof(goid), NULL);
    if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
    {
	SQLFreeStmt(hstmtGoaGOIDFromGiList, SQL_DROP);
	ErrPostEx(SEV_ERROR, 0, 0, "[GOA_SearchGOIDFromGiList]: Could not bind return parameter");
	return SH_ERROR;
    }

    /* fetch results and save in your linked list */
    SQLINTEGER fetchCount = 0;
    while( (retcode = SQLFetch(hstmtGoaGOIDFromGiList) ==  SQL_SUCCESS)  || (retcode == SQL_SUCCESS_WITH_INFO) )
    {
	fetchCount++;
	if(retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
	{
	    //printf("go: %d\t", goid);
	    ValNodeAddInt ( &vnpGOList, 0, goid);
	}
    }
   
    if (fetchCount == 0)
    {	
	SQLFreeStmt(hstmtGoaGOIDFromGiList, SQL_DROP);
	return SH_NODATA;
    }
    if(SQLFreeStmt(hstmtGoaGOIDFromGiList, SQL_DROP) != SQL_SUCCESS)
    {
	ErrPostEx(SEV_ERROR, 0, 0, "[GOA_SearchGOIDFromGiList]: Could not free statement.");
	return SH_ERROR;
    } 
    *vnp = vnpGOList;
    return SH_SUCCESS; 
}


Int4 GOA_SearchGOIDFromRedundantGi(Int4 gi, ValNodePtr PNTR vnp)
{
    ValNodePtr vnpGOList = NULL;
    SQLRETURN retcode;                    /* return values for odbc layer      */

    if(vnp == NULL)
    {
	ErrPostEx(SEV_ERROR,0,0, "[GOA_SearchGOIDFromRedundantGi]: ValNodePtr is NULL.");
	return SH_ERROR;
    }

    if (gi <= 0)
    {	
	ErrPostEx(SEV_ERROR,0,0, "[GOA_SearchGOIDFromGi]: Invalid GI.");
	return SH_ERROR;
    }

    goa_gi = gi;

    /* execute the select statement */
    if( (retcode = SQLExecute(hstmtGoaGOIDFromRedundantGi)) != SQL_SUCCESS )
    {
	ErrPostEx(SEV_ERROR, 0, 0, "[GOA_SearchGOIDFromRedundantGi]: Could not execute select statement.");
	LogSQLInfo(hstmtGoaGOIDFromRedundantGi);
	SQLFreeStmt(hstmtGoaGOIDFromRedundantGi, SQL_CLOSE);
	return SH_ERROR;
    }

    /*   Bind Return Parameters from the search   */
    retcode = SQLBindCol(hstmtGoaGOIDFromRedundantGi, 1, SQL_C_SLONG, &goa_go, sizeof(goa_go), NULL);
    if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
    {
	SQLFreeStmt(hstmtGoaGOIDFromRedundantGi, SQL_CLOSE);
	ErrPostEx(SEV_ERROR, 0, 0, "[GOA_SearchGOIDFromRedundantGi]: Could not bind return parameter");
	return SH_ERROR;
    }

    /* fetch results and save in your linked list */
    SQLINTEGER fetchCount = 0;

    while( (retcode = SQLFetch(hstmtGoaGOIDFromRedundantGi) ==  SQL_SUCCESS)  || (retcode == SQL_SUCCESS_WITH_INFO) )
    {
	    ValNodeAddInt ( &vnpGOList, 0, goa_go);
	    fetchCount++;
    }

    SQLFreeStmt(hstmtGoaGOIDFromRedundantGi, SQL_CLOSE);

    if (fetchCount == 0)
    {
	return SH_NODATA;
    }

    *vnp = vnpGOList;
    return SH_SUCCESS;  
}


Int4 GOA_SearchGOIDFromRedundantGiList(ValNodePtr giList, ValNodePtr PNTR vnp)
{
    ValNodePtr vnpGOList = NULL;
    Int4 goid = 0;
    SQLRETURN retcode;                    /* return values for odbc layer      */
    static SQLHANDLE hstmtGoaGOIDFromRedundantGiList;
    Char *pSQL_stmt; 
    Char *auxSQL;
    CharPtr pstr = NULL;

    pstr = Misc_SaveValNodeListAsString(giList);
  
    if(vnp == NULL)
    {
	ErrPostEx(SEV_ERROR,0,0, "[GOA_SearchGOIDFromRedundantGiList]: ValNodePtr is NULL.");
	return SH_ERROR;
    }

    /* allocate space for the select statement */
    auxSQL = "SELECT distinct e.go FROM goa_gigo e, %s.redund a, %s.redund b, %s.taxgi c, %s.taxgi d WHERE b.gi=e.gi AND b.rgroup = a.rgroup AND a.gi=c.gi AND b.gi=d.gi AND c.taxid=d.taxid AND a.gi IN(%s) order by e.go";
    pSQL_stmt = (char*) malloc((sizeof(char) * (strlen(auxSQL) + (strlen(database)*4) + strlen(pstr))));
 
    if(pSQL_stmt == NULL)
    {
	ErrPostEx(SEV_ERROR, 0, 0, "[GOA_SearchGOIDFromRedundantGiList]: Could not allocate space for hstmtGoaGOIDFromRedundantGiList.");
	return SH_ERROR;
    }
    
    sprintf((char*)pSQL_stmt, auxSQL, database, database, database, database, pstr);

    /* Create Handle Statement, once handle exists we can free char* */
    retcode = CreateHandle(&hstmtGoaGOIDFromRedundantGiList, pSQL_stmt, hdbc, &henv);
    if(retcode != SQL_SUCCESS)
    {
	ErrPostEx(SEV_ERROR, 0, 0, "[GOA_SearchGOIDFromRedundantGiList]: Could not create stmt handle for hstmtGoaGOIDFromGi.");
	free(pSQL_stmt);
	return SH_ERROR;
    }
    free(pSQL_stmt);
    free(pstr);

    /* execute the select statement */
    if( (retcode = SQLExecute(hstmtGoaGOIDFromRedundantGiList)) != SQL_SUCCESS )
    {
	ErrPostEx(SEV_ERROR, 0, 0, "[GOA_SearchGOIDFromRedundantGiList]: Could not execute select statement.");
	LogSQLInfo(hstmtGoaGOIDFromRedundantGiList);
	SQLFreeStmt(hstmtGoaGOIDFromRedundantGiList, SQL_DROP);
	return SH_ERROR;
    }

    /*   Bind Return Parameters from the search   */
    retcode = SQLBindCol(hstmtGoaGOIDFromRedundantGiList, 1, SQL_C_SLONG, &goid, sizeof(goid), NULL);
    if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
    {
	SQLFreeStmt(hstmtGoaGOIDFromRedundantGiList, SQL_DROP);
	ErrPostEx(SEV_ERROR, 0, 0, "[GOA_SearchGOIDFromRedundantGiList]: Could not bind return parameter");
	return SH_ERROR;
    }

    /* fetch results and save in your linked list */
    SQLINTEGER fetchCount = 0;
    while( (retcode = SQLFetch(hstmtGoaGOIDFromRedundantGiList) ==  SQL_SUCCESS)  || (retcode == SQL_SUCCESS_WITH_INFO) )
    {
	fetchCount++;
	if(retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
	{
	    ValNodeAddInt ( &vnpGOList, 0, goid);
	}
    }
   
    if (fetchCount == 0)
    {	
	SQLFreeStmt(hstmtGoaGOIDFromRedundantGiList, SQL_DROP);
	return SH_NODATA;
    }
    if(SQLFreeStmt(hstmtGoaGOIDFromRedundantGiList, SQL_DROP) != SQL_SUCCESS)
    {
	ErrPostEx(SEV_ERROR, 0, 0, "[GOA_SearchGOIDFromRedundantGiList]: Could not free statement.");
	return SH_ERROR;
    } 
    *vnp = vnpGOList;
    return SH_SUCCESS; 
}


Int4 GOA_SearchGiFromGOID(Int4 go, ValNodePtr PNTR vnp)
{
    ValNodePtr vnpGiList = NULL;
    SQLRETURN retcode;                    /* return values for odbc layer      */

    if(vnp == NULL)
    {
	ErrPostEx(SEV_ERROR,0,0, "[GOA_SearchGiFromGOID]: ValNodePtr is NULL.");
	return SH_ERROR;
    }

    if (go <= 0)
    {
	ErrPostEx(SEV_ERROR,0,0, "[GOA_SearchGiFromGOID]: Invalid GO.");
	return SH_ERROR;
    }
    goa_go = go;

    /* execute the select statement */
    if( (retcode = SQLExecute(hstmtGoaGiFromGOID)) != SQL_SUCCESS )
    {
	ErrPostEx(SEV_ERROR, 0, 0, "[GOA_SearchGiFromGOID]: Could not execute select statement.");
	LogSQLInfo(hstmtGoaGiFromGOID);
	SQLFreeStmt(hstmtGoaGiFromGOID, SQL_CLOSE);
	return SH_ERROR;
    }

    /*   Bind Return Parameters from the search   */
    retcode = SQLBindCol(hstmtGoaGiFromGOID, 1, SQL_C_SLONG, &goa_gi, sizeof(goa_gi), NULL);
    if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
    {
	SQLFreeStmt(hstmtGoaGiFromGOID, SQL_CLOSE);
	ErrPostEx(SEV_ERROR, 0, 0, "[GOA_SearchGiFromGOID]: Could not bind return parameter");
	return SH_ERROR;
    }

    /* fetch results and save in your linked list */
    SQLINTEGER fetchCount = 0;

    while( (retcode = SQLFetch(hstmtGoaGiFromGOID) ==  SQL_SUCCESS)  || (retcode == SQL_SUCCESS_WITH_INFO) )
    {
	    ValNodeAddInt ( &vnpGiList, 0, goa_gi);
	    fetchCount++;
    }
    SQLFreeStmt(hstmtGoaGiFromGOID, SQL_CLOSE);
    if (fetchCount == 0)
    {
	return SH_NODATA;
    }

    *vnp = vnpGiList;
    return SH_SUCCESS;  
}


Int4 GOA_SearchGiFromGOIDList(ValNodePtr goList, ValNodePtr PNTR vnp)
{
    ValNodePtr vnpGiList = NULL;
    Int4 gi = 0;
    SQLRETURN retcode;                    /* return values for odbc layer      */
    static SQLHANDLE hstmtGoaGiFromGOIDList;
    Char *pSQL_stmt; 
    CharPtr pstr = NULL;
  
    if(vnp == NULL)
    {
	ErrPostEx(SEV_ERROR,0,0, "[GOA_SearchGiFromGOIDList]: ValNodePtr is NULL.");
	return SH_ERROR;  
    }

    pstr = Misc_SaveValNodeListAsString(goList);

    /* allocate space for the select statement */
    pSQL_stmt = (char*) malloc((sizeof(char) * 50) + strlen(database) + strlen(pstr) );
 
    if(pSQL_stmt == NULL)
    {
	ErrPostEx(SEV_ERROR, 0, 0, "[GOA_SearchGiFromGOIDList]: Could not allocate space for hstmt.");
	return SH_ERROR;
    }
    sprintf((char*)pSQL_stmt, "SELECT DISTINCT gi FROM %s.goa_gigo WHERE go IN(%s)", database, pstr);

    /* Create Handle Statement, once handle exists we can free char* */
    retcode = CreateHandle(&hstmtGoaGiFromGOIDList, pSQL_stmt, hdbc, &henv);
    if(retcode != SQL_SUCCESS)
    {
	ErrPostEx(SEV_ERROR, 0, 0, "[GOA_SearchGiFromGOIDList]: Could not create stmt handle for hstmt.");
	free(pSQL_stmt);
	return SH_ERROR;
    }
    free(pSQL_stmt);
    free(pstr);
    /* execute the select statement */
    if( (retcode = SQLExecute(hstmtGoaGiFromGOIDList)) != SQL_SUCCESS )
    {
	ErrPostEx(SEV_ERROR, 0, 0, "[GOA_SearchGiFromGOIDList]: Could not execute select statement.");
	LogSQLInfo(hstmtGoaGiFromGOIDList);
	SQLFreeStmt(hstmtGoaGiFromGOIDList, SQL_DROP);
	return SH_ERROR;
    }

    /*   Bind Return Parameters from the search   */
    retcode = SQLBindCol(hstmtGoaGiFromGOIDList, 1, SQL_C_SLONG, &gi, sizeof(gi), NULL);
    if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
    {
	SQLFreeStmt(hstmtGoaGiFromGOIDList, SQL_DROP);
	ErrPostEx(SEV_ERROR, 0, 0, "[GOA_SearchGiFromGOIDList]: Could not bind return parameter");
	return SH_ERROR;
    }

    /* fetch results and save in your linked list */
    SQLINTEGER fetchCount = 0;
    while( (retcode = SQLFetch(hstmtGoaGiFromGOIDList) ==  SQL_SUCCESS)  || (retcode == SQL_SUCCESS_WITH_INFO) )
    {
	ValNodeAddInt ( &vnpGiList, 0, gi);
	fetchCount++;
    }
   
    if (fetchCount == 0)
    {	
	SQLFreeStmt(hstmtGoaGiFromGOIDList, SQL_DROP);
	return SH_NODATA;
    }

    if(SQLFreeStmt(hstmtGoaGiFromGOIDList, SQL_DROP) != SQL_SUCCESS)
    {
	ErrPostEx(SEV_ERROR, 0, 0, "[GOA_SearchGiFromGOIDList]: Could not free statement.");
	return SH_ERROR;
    }
    *vnp = vnpGiList;
    return SH_SUCCESS; 
}


Int4 GOA_SearchGiFromGOIDAndECode(Int4 go, CharPtr pECode, ValNodePtr PNTR vnp)
{
    ValNodePtr vnpGiList = NULL;
    SQLRETURN retcode;                    /* return values for odbc layer      */

    if(vnp == NULL)
    {
	ErrPostEx(SEV_ERROR,0,0, "[GOA_SearchGiFromGOIDAndECode]: ValNodePtr is NULL.");
	return SH_ERROR;
    }
    if (go <= 0)
    {	
	ErrPostEx(SEV_ERROR,0,0, "[GOA_SearchGiFromGOIDAndECode]: Invalid GO.");
	return SH_ERROR;
    }
    goa_go = go;

    if (pECode == NULL)
    {
	ErrPostEx(SEV_ERROR,0,0, "[GOA_SearchGiFromGOIDAndECode]: Invalid ECode.");
	return SH_ERROR;
    }                        
    sprintf((char*)goa_ecode, "%s", IsNullStr(pECode));

    /* execute the select statement */
    retcode = SQLExecute(hstmtGoaGiFromGOIDAndECode);
    if( retcode != SQL_SUCCESS) 
    {
	ErrPostEx(SEV_ERROR, 0, 0, "[GOA_SearchGiFromGOIDAndECode]: Could not execute select statement.");
	LogSQLInfo(hstmtGoaGiFromGOIDAndECode);
	SQLFreeStmt(hstmtGoaGiFromGOIDAndECode, SQL_CLOSE);
	return SH_ERROR;
    }

    /*   Bind Return Parameters from the search   */
    retcode = SQLBindCol(hstmtGoaGiFromGOIDAndECode, 1, SQL_C_SLONG, &goa_gi, sizeof(goa_gi), NULL);
    if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
    {
	ErrPostEx(SEV_ERROR, 0, 0, "[GOA_SearchGiFromGOIDAndECode]: Could not bind return parameter");
	SQLFreeStmt(hstmtGoaGiFromGOIDAndECode, SQL_CLOSE);
	return SH_ERROR;
    }

    /* fetch results and save in your linked list */
    SQLINTEGER fetchCount = 0;
    while( (retcode = SQLFetch(hstmtGoaGiFromGOIDAndECode) ==  SQL_SUCCESS)  || (retcode == SQL_SUCCESS_WITH_INFO) )
    {
	fetchCount++;
	ValNodeAddInt ( &vnpGiList, 0, goa_gi);
    }

    SQLFreeStmt(hstmtGoaGiFromGOIDAndECode, SQL_CLOSE);

    if (fetchCount == 0)
    {
	return SH_NODATA;
    }
    *vnp = vnpGiList;
    return SH_SUCCESS;  
}


Int4 GOA_SearchGiFromGOIDListAndECode(ValNodePtr goList, CharPtr ECode, ValNodePtr PNTR vnp)
{
    ValNodePtr vnpGiList = NULL;
    Int4 gi = 0;
    SQLRETURN retcode;                    /* return values for odbc layer      */
    static SQLHANDLE hstmtGoaGiListFromGOIDListAndEcode;
    Char *pSQL_stmt;
    CharPtr pstr = NULL;

    if(vnp == NULL)
    {
	ErrPostEx(SEV_ERROR,0,0, "[GOA_SearchGiFromGOIDListAndECode]: ValNodePtr is NULL.");
	return SH_ERROR;
    }

    pstr = Misc_SaveValNodeListAsString(goList);

   /* allocate space for the select statement */
    
    pSQL_stmt = (char*) malloc((sizeof(char) * 70) + strlen(database) + strlen(pstr) );
 
    if(pSQL_stmt == NULL)
    {
	ErrPostEx(SEV_ERROR, 0, 0, "[GOA_SearchGiFromGOIDListAndECode]: Could not allocate space for hstmt.");
	return SH_ERROR;
    }
    sprintf((char*)pSQL_stmt, "SELECT DISTINCT gi FROM %s.goa_gigo WHERE go IN(%s) AND code='%s'", database, pstr, ECode);

    /* Create Handle Statement, once handle exists we can free char* */
    retcode = CreateHandle(&hstmtGoaGiListFromGOIDListAndEcode, pSQL_stmt, hdbc, &henv);
    if(retcode != SQL_SUCCESS)
    {
	ErrPostEx(SEV_ERROR, 0, 0, "[GOA_SearchGiFromGOIDListAndECode]: Could not create stmt handle for hstmt.");
	free(pSQL_stmt);
	return SH_ERROR;
    }
    free(pSQL_stmt);
    free(pstr);


    /* execute the select statement */
    if( (retcode = SQLExecute(hstmtGoaGiListFromGOIDListAndEcode)) != SQL_SUCCESS )
    {
	ErrPostEx(SEV_ERROR, 0, 0, "[GOA_SearchGiFromGOIDListAndECode]: Could not execute select statement.");
	LogSQLInfo(hstmtGoaGiListFromGOIDListAndEcode);
	SQLFreeStmt(hstmtGoaGiListFromGOIDListAndEcode, SQL_DROP);
	return SH_ERROR;
    }

    /*   Bind Return Parameters from the search   */
    retcode = SQLBindCol(hstmtGoaGiListFromGOIDListAndEcode, 1, SQL_C_SLONG, &gi, sizeof(gi), NULL);
    if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
    {
	SQLFreeStmt(hstmtGoaGiListFromGOIDListAndEcode, SQL_DROP);
	ErrPostEx(SEV_ERROR, 0, 0, "[GOA_SearchGiFromGOIDListAndECode]: Could not bind return parameter");
	return SH_ERROR;
    }

    /* fetch results and save in your linked list */
    SQLINTEGER fetchCount = 0;
    
    while( (retcode = SQLFetch(hstmtGoaGiListFromGOIDListAndEcode) ==  SQL_SUCCESS)  || (retcode == SQL_SUCCESS_WITH_INFO) )
    {
	fetchCount++;
	ValNodeAddInt ( &vnpGiList, 0, gi);
    }

    if (fetchCount == 0)
    {
	SQLFreeStmt(hstmtGoaGiListFromGOIDListAndEcode, SQL_DROP);
	return SH_NODATA;
    }

    if(SQLFreeStmt(hstmtGoaGiListFromGOIDListAndEcode, SQL_DROP) != SQL_SUCCESS)
    {
	ErrPostEx(SEV_ERROR, 0, 0, "[GOA_SearchGiFromGOIDListAndECode]: Could not free statement.");
	return SH_ERROR;
    } 
    
    *vnp = vnpGiList;
    return SH_SUCCESS;  
}


Int4 GOA_SearchGOECodeFromGiAndGOID(Int4 gi, Int4 go, ValNodePtr PNTR vnp)
{
    ValNodePtr vnpECodeList = NULL;
    Char ECode[4];                        /* The size of goa_gigo.code */
    SQLINTEGER ECodeLen;
    SQLRETURN retcode;                    /* return values for odbc layer      */

    if(vnp == NULL)
    {
	ErrPostEx(SEV_ERROR,0,0, "[GOA_SearchGOECodeFromGiAndGOID]: ValNodePtr is NULL.");
	return SH_ERROR;
    }
    if (gi <= 0)
    {
	ErrPostEx(SEV_ERROR,0,0, "[GOA_SearchGOECodeFromGiAndGOID]: Invalid GI.");
	return SH_ERROR;
    }
    goa_gi = gi;
    if (go <= 0)
    {
	ErrPostEx(SEV_ERROR,0,0, "[GOA_SearchGOECodeFromGiAndGOID]: Invalid GO.");
	return SH_ERROR;
    }
    goa_go = go;
  
    /* execute the select statement */
    if( (retcode = SQLExecute(hstmtGoaGOECodeFromGiAndGOID)) != SQL_SUCCESS )
    {
	ErrPostEx(SEV_ERROR, 0, 0, "[GOA_SearchGOECodeFromGiAndGOID]: Could not execute select statement.");
	LogSQLInfo(hstmtGoaGOECodeFromGiAndGOID);
	SQLFreeStmt(hstmtGoaGOECodeFromGiAndGOID, SQL_CLOSE);
	return SH_ERROR;
    }

    /*   Bind Return Parameters from the search   */
    retcode = SQLBindCol(hstmtGoaGOECodeFromGiAndGOID, 1, SQL_C_CHAR, ECode, sizeof(ECode), (int*)(&ECodeLen));
    if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
    {
	SQLFreeStmt(hstmtGoaGOECodeFromGiAndGOID, SQL_CLOSE);
	ErrPostEx(SEV_ERROR, 0, 0, "[GOA_SearchGOECodeFromGiAndGOID]: Could not bind return parameter.");
	return SH_ERROR;
    }

    /* fetch results and save in your linked list */
    SQLINTEGER fetchCount = 0;
    while( (retcode = SQLFetch(hstmtGoaGOECodeFromGiAndGOID) ==  SQL_SUCCESS)  || (retcode == SQL_SUCCESS_WITH_INFO) )
    {
	    ValNodeAddStr ( &vnpECodeList, 0, StringSave(ECode));
	    fetchCount++;
    }

    SQLFreeStmt(hstmtGoaGOECodeFromGiAndGOID, SQL_CLOSE);
    if (fetchCount == 0)
    {
	return SH_NODATA;
    }
    *vnp = vnpECodeList;
    return SH_SUCCESS;
}


Int4 GOA_SearchGOIDFromGENEID(Int4 gene, ValNodePtr PNTR vnp)
{
    ValNodePtr vnpGOList = NULL;
    SQLRETURN retcode;                    /* return values for odbc layer      */
    Char *pSQL_stmt; 
    Char *auxSQL;
    if(vnp == NULL)
    {
	ErrPostEx(SEV_ERROR,0,0, "[GOA_SearchGOIDFromGENEID]: ValNodePtr is NULL.");
	return SH_ERROR;
    }

    if (gene <= 0)
    {
	ErrPostEx(SEV_ERROR,0,0, "[GOA_SearchGOIDFromGENEID]: Invalid GENE Id.");
	return SH_ERROR;
    }
    sprintf(goa_gene, "%ld", gene);

    /* execute the select statement */
    if( (retcode = SQLExecute(hstmtGoaGOIDFromGENEID)) != SQL_SUCCESS )
    {
	ErrPostEx(SEV_ERROR, 0, 0, "[GOA_SearchGOIDFromGENEID]: Could not execute select statement.");
	LogSQLInfo(hstmtGoaGOIDFromGENEID);
	SQLFreeStmt(hstmtGoaGOIDFromGENEID, SQL_CLOSE);
	return SH_ERROR;
    }

    /*   Bind Return Parameters from the search   */
    retcode = SQLBindCol(hstmtGoaGOIDFromGENEID, 1, SQL_C_SLONG, &goa_go, sizeof(goa_go), NULL);
    if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
    {
	SQLFreeStmt(hstmtGoaGOIDFromGENEID, SQL_CLOSE);
	ErrPostEx(SEV_ERROR, 0, 0, "[GOA_SearchGOIDFromGENEID]: Could not bind return parameter");
	return SH_ERROR;
    }

    /* fetch results and save in your linked list */
    SQLINTEGER fetchCount = 0;
    while( (retcode = SQLFetch(hstmtGoaGOIDFromGENEID) ==  SQL_SUCCESS)  || (retcode == SQL_SUCCESS_WITH_INFO) )
    {
	fetchCount++;
	ValNodeAddInt ( &vnpGOList, 0, goa_go);
    }
 
    if (fetchCount == 0)
    {
	SQLFreeStmt(hstmtGoaGOIDFromGENEID, SQL_CLOSE);
	return SH_NODATA;
    }
    SQLFreeStmt(hstmtGoaGOIDFromGENEID, SQL_CLOSE);

    *vnp = vnpGOList;
    return SH_SUCCESS; 
}

Int4 GOA_SearchPMIDFromGiAndGOID(Int4 gi, Int4 go, ValNodePtr PNTR vnp)
{
    ValNodePtr vnpPMIDList = NULL;
    Char PMID[20];                        /* The size of goa_reference.xref_key */
    SQLINTEGER PMIDLen;
    SQLRETURN retcode;                    /* return values for odbc layer      */

    if(vnp == NULL)
    {
	ErrPostEx(SEV_ERROR,0,0, "[GOA_SearchPMIDFromGiAndGOID]: ValNodePtr is NULL.");
	return SH_ERROR;
    }

    /* Bind Parameters for search */
    if (gi <= 0)
    {
	ErrPostEx(SEV_ERROR,0,0, "[GOA_SearchPMIDFromGiAndGOID]: Invalid GI.");
	return SH_ERROR;
    }
    goa_gi = gi;
    if (go <= 0)
    {
	ErrPostEx(SEV_ERROR,0,0, "[GOA_SearchPMIDFromGiAndGOID]: Invalid GO.");
	return SH_ERROR;
    }
    goa_go = go;

    /* execute the select statement */
    if( (retcode = SQLExecute(hstmtPMIDFromGiAndGOID)) != SQL_SUCCESS )
    {
	ErrPostEx(SEV_ERROR, 0, 0, "[GOA_SearchPMIDFromGiAndGOID]: Could not execute select statement.");
	LogSQLInfo(hstmtPMIDFromGiAndGOID);
	SQLFreeStmt(hstmtPMIDFromGiAndGOID, SQL_CLOSE);
	return SH_ERROR;
    }

    /*   Bind Return Parameters from the search   */
    retcode = SQLBindCol(hstmtPMIDFromGiAndGOID, 1, SQL_C_CHAR, PMID, sizeof(PMID), (int*)(&PMIDLen));
    if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
    {
	SQLFreeStmt(hstmtPMIDFromGiAndGOID, SQL_CLOSE);
	ErrPostEx(SEV_ERROR, 0, 0, "[GOA_SearchPMIDFromGiAndGOID]: Could not bind return parameter");
	return SH_ERROR;
    }

    /* fetch results and save in your linked list */
    SQLINTEGER fetchCount = 0;

    while( (retcode = SQLFetch(hstmtPMIDFromGiAndGOID) ==  SQL_SUCCESS)  || (retcode == SQL_SUCCESS_WITH_INFO) )
    {
	ValNodeAddInt( &vnpPMIDList, 0, atol(PMID)); 
	fetchCount++;
    } 
    SQLFreeStmt(hstmtPMIDFromGiAndGOID, SQL_CLOSE);
    if (fetchCount == 0)
    {
	return SH_NODATA;
    }
   
    *vnp = vnpPMIDList;
    return SH_SUCCESS;  
}


Boolean CloseGOA(void)
{
    if(SQLFreeStmt(hstmtGoaGOIDFromGi, SQL_DROP) != SQL_SUCCESS)
    {
	ErrPostEx(SEV_ERROR, 0, 0, "[CloseGOA]: Could not free statement hstmtGoaGOIDFromGi.");
	return FALSE;
    } 
    if(SQLFreeStmt(hstmtGoaGOIDFromRedundantGi, SQL_DROP) != SQL_SUCCESS)
    {
	ErrPostEx(SEV_ERROR, 0, 0, "[CloseGOA]: Could not free statement hstmtGoaGOIDFromRedundantGi.");
	return FALSE;
    }
    if(SQLFreeStmt(hstmtGoaGiFromGOID, SQL_DROP) != SQL_SUCCESS)
    {
	ErrPostEx(SEV_ERROR, 0, 0, "[CloseGOA]: Could not free statement hstmtGoaGiFromGOID.");
	return FALSE;
    } 
    if(SQLFreeStmt(hstmtGoaGiFromGOIDAndECode, SQL_DROP) != SQL_SUCCESS)
    {
	ErrPostEx(SEV_ERROR, 0, 0, "[CloseGOA]: Could not free statement hstmtGoaGiFromGOIDAndECode.");
	return FALSE;
    }
    if(SQLFreeStmt(hstmtGoaGOECodeFromGiAndGOID, SQL_DROP) != SQL_SUCCESS)
    {
	ErrPostEx(SEV_ERROR, 0, 0, "[CloseGOA]: Could not free statement hstmtGoaGOECodeFromGiAndGOID.");
	return FALSE;
    } 
    if(SQLFreeStmt(hstmtGoaGOIDFromGENEID, SQL_DROP) != SQL_SUCCESS)
    {
	ErrPostEx(SEV_ERROR, 0, 0, "[CloseGOA]: Could not free statement hstmtGoaGOIDFromGENEID.");
	return FALSE;
    }
    if(SQLFreeStmt(hstmtPMIDFromGiAndGOID, SQL_DROP) != SQL_SUCCESS)
    {
	ErrPostEx(SEV_ERROR, 0, 0, "[CloseGOA]: Could not free statement hstmtPMIDFromGiAndGOID.");
	return FALSE;
    }
    return TRUE;
}


