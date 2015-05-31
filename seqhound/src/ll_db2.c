/* $Id: ll_db2.c,v 1.2 2004/01/06 22:32:16 skuznets Exp $
**************************************************************************
  Copyright (C) 2001 MDS Proteomics

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
  Locus Link database management
  Part of SeqHound

****************************************************************************/

#include <slri_db2.h>
#include <intrez_misc.h>
#include <slri_math.h>
#include <sqlcli1.h>
#include <ll_db.h>

/**************************************************
 *       PUBLIC CODE BASE INITIALIZTION           *
 **************************************************/
/* two variables used in every method to communicate with 
the db2 database */

static SQLHANDLE henv; /*envirnment handle */
static SQLHANDLE hdbc; /* connection handle */

static SQLHANDLE hstmt_cdddb, hstmt_omimdb, hstmt_lldb, hstmt_godb;

/* connect to the db2 database and set up handles for the locus
link inserts.*/
void LL_InitCodeBase(void)
{
  SQLCHAR db2alias[10];
  SQLCHAR usr[10];
  SQLCHAR passwd[10];
  
  GetAppParam("intrez", "datab", "username", NULL, (Char*)usr, (size_t)10 * (sizeof(Char)));
  GetAppParam("intrez", "datab", "password", NULL, (Char*)passwd, (size_t)10 * (sizeof(Char)));
  GetAppParam("intrez", "datab", "db2alias", NULL, (Char*)db2alias, (size_t)10 * (sizeof(Char)));

   /* allocate an envirnment handle */
   if (SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv) != SQL_SUCCESS){
     print_err(henv);	
     ErrPostEx(SEV_ERROR,0,0, "Unable to allocate envirnment handle.");
   }

   /* allocate a connection handle */
   if (SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc) == SQL_ERROR){
	print_err(henv);
	ErrPostEx(SEV_ERROR,0,0, "Can not allocate a connection handle");
   }

   if(SQLConnect( hdbc, db2alias, SQL_NTS, usr, SQL_NTS, passwd, SQL_NTS) != SQL_SUCCESS){
	ErrPostEx(SEV_ERROR,0,0, "could not connect to the database. ");
   }

   CreateHandle(&hstmt_omimdb,"INSERT into seqhound.omimdb(gi,omim_id) values(?,?)", &hdbc,&henv);
   CreateHandle(&hstmt_lldb,"INSERT into seqhound.lldb(gi,locuslink_id,map) values(?,?,?)",&hdbc,&henv);
   CreateHandle(&hstmt_cdddb,"INSERT INTO seqhound.cdddb(gi,cdd_id,evalue) values(?,?,?)",&hdbc,&henv);
   CreateHandle(&hstmt_godb,"INSERT into seqhound.godb(gi,go_id,pmid,evidence) values(?,?,?,?)",&hdbc,&henv);

}

/* closes the databases.  This function must called at the
   end of your function, if you called LL_InitCodeBase.*/
void LL_InitUndoCodeBase(void)
{
     Int2 sqlrc = SQL_SUCCESS;

 /* commit all changes */
  SQLTransact(henv, hdbc, SQL_COMMIT);

   /*Free the database handles used to communicate with the database*/
   sqlrc = SQLDisconnect( hdbc);
   if (sqlrc != SQL_SUCCESS){
	ErrPostEx(SEV_INFO, 0, 0, "Could not disconnect from the database.\n");
   }

   /*free the connnection handle */
   sqlrc = SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
   if (sqlrc != SQL_SUCCESS) {
	ErrPostEx(SEV_INFO, 0, 0, "could not free the connection handle. \n");
   }

   /*free envirnment handle */
   sqlrc = SQLFreeHandle(SQL_HANDLE_ENV, henv);
   if (sqlrc != SQL_SUCCESS) {
	ErrPostEx(SEV_INFO, 0, 0, "could not free the envirnment handle. \n");	
   }
}

void CloseAllLLDBs(void){
}

/**********************************************************
       PRIVATE CODE BASE INITIALIZATION FUNCTIONS
***********************************************************/

/* initializes the fields and tags of the file
   of the LL_GO DB file */
static void LL_Init_GO_db_Fields (void)
{

}

/* initializes the fields and tags of the files
   of the LL_OMIM DB file */
static void LL_Init_OMIM_db_Fields(void)
{

}

/* initializes the fields and tags of the files
   of the LL_CDD DB files */
static void LL_Init_CDD_db_Fields(void)
{

}

/* initializes the fields and tags of the files
   of the LL_LLINK DB file */
static void LL_Init_LLID_db_Fields(void)
{

}

/***************************************************
           PRIVATE OPEN DATABASE FUNCTIONS
****************************************************/
/* opens and sets the fields and tags of LL_GO code base files */
static void LL_OpenGO_db(CharPtr path)
{

}

/* opens and sets the fields and tags of LL_OMIM code base files */
static void LL_OpenOMIM_db(CharPtr path)
{
 
}

/* open and sets the fields and tags of LL_CDD code base files */
static void LL_OpenCDD_db(CharPtr path)
{

}

/* opens and sets the fields and tags of LL_LINK code base files */
static void LL_OpenLLID_db(CharPtr path)
{
 
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

/******************************************************************
		    PRIVATE HELPER FUNCTION
*******************************************************************/
/* retrieves a string stored in a memo field. It is the responsibility
   of the calling function to free the string. */
static CharPtr GetMemoField(/*FIELD4 *field*/)
{
/*   Int4 iSize = 0;
   CharPtr ptitle = NULL, title = NULL;
   CharPtr nospaces = NULL, retVal = NULL;

   iSize = f4memoLen(field);

   title = (CharPtr) f4memoStr(field);
   ptitle = StringSave(title);
   nospaces = TrimSpace(ptitle);
   iSize = StringLen(nospaces);
   retVal = (CharPtr) MemNew( (size_t)(iSize+1)*sizeof(Char) );
   StringCpy(retVal, nospaces);
   MemFree(ptitle);
   return retVal;*/
}

/*********************************************************************
                    PRIVATE SEARCH FUNCTIONS
   THESE FUNCTIONS ARE NOT INTENDED TO BY CALLED BY THE END USER.
    SEE QUERY.H/QUERY.C FOR THE PUBLIC SHOUND SEARCH FUNCTIONS
**********************************************************************/

/* searches the LL_GODB for a key specified in ppgo. If such a key is found, 
   fills in ppgo with the relevant data, and returns LL_SUCCESS. 
   If an error occurs, return LL_ERROR, if the key is not found returns LL_NOTFOUND.
*/
Int4 LL_SearchGODB(StGOdbPtr PNTR ppgo)
{

  StGOdbPtr pgo = NULL;
  Int4 key = 0;
  CharPtr search_godb = NULL;
  CharPtr pmid_stmt = "select gi,go_id,pmid,evidence from seqhound.godb where pmid = ?";
  CharPtr go_stmt = "select gi,go_id,pmid,evidence from seqhound.godb where go_id = ?";
  CharPtr np_stmt = "select gi,go_id,pmid,evidence from seqhound.godb where gi=?";
  SQLHANDLE hstmt;
  Int2 sqlrc = SQL_SUCCESS;
  struct{
     SQLINTEGER len;
     SQLINTEGER val;
   }gi, goid, pmid;
  struct{
     SQLINTEGER len;
     Char val[50];
   }evidence;
   
  /* SK: NULL ptr check */
  if(ppgo == NULL || *ppgo == NULL)
  {
    ErrPostEx(SEV_ERROR,0,0, "LL_SearchGODB: StGOdbPtr is NULL.");
    return LL_ERROR;
  }
    
  LL_InitCodeBase();

  pgo = *ppgo;
  if(pgo->npid > 0){
    key = pgo->npid;
    /* tag select the npid */
    search_godb = np_stmt;
  }
  if(pgo->goid > 0){
    key = pgo->goid;
    /* tag select the goid */
    search_godb = go_stmt;
  }
  if(pgo->pmid > 0){
    key = pgo->pmid;
    /* tag select the pmid */
    search_godb = pmid_stmt;
  }
 /* search GODB*/
   CreateHandle(&hstmt,search_godb,&henv,&hdbc);
  
  SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_LONG,SQL_INTEGER, 0, 0, &key, 0, NULL);

/* set auto commit on */
  SQLSetConnectAttr( hdbc, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_ON, SQL_NTS);
      
  /* execute statment */
  if (SQLExecute(hstmt) != SQL_SUCCESS){
    ErrPostEx(SEV_ERROR,0,0,"SEARCH_GODB:unable to execute select statement.");
    print_err(hstmt);
    SQLFreeStmt(hstmt, SQL_DROP);
    return FALSE;
  }
      
  SQLBindCol(hstmt,1,SQL_C_LONG,&gi.val,0,&gi.len);
  SQLBindCol(hstmt,2,SQL_C_LONG,&goid.val,0,&goid.len);
  SQLBindCol(hstmt,3,SQL_C_LONG,&pmid.val,0,&pmid.len);
  SQLBindCol(hstmt,4,SQL_C_CHAR,evidence.val,50,&evidence.len);
     
  sqlrc = SQLFetch(hstmt);
  if(sqlrc == SQL_NO_DATA_FOUND){
    SQLFreeStmt(hstmt, SQL_DROP);
    ErrPostEx(SEV_INFO, 0, 0, "No data found for key : %ld", key);
    
    return LL_NOTFOUND;
  }
    
  while(sqlrc == SQL_SUCCESS || sqlrc == SQL_SUCCESS_WITH_INFO){
    pgo->npid = gi.val;
    pgo->goid = goid.val;
    pgo->pmid = pmid.val;
    pgo->eviCode = StringSave(evidence.val);
    if(pgo->pmid == 1000) {
      pgo->pmid = -1;
      }
    /*Get next record, if there is one add a node to the valnode.*/
    sqlrc = SQLFetch(hstmt);
    if(sqlrc == SQL_SUCCESS || sqlrc == SQL_SUCCESS_WITH_INFO){
      pgo->next = StGOdbNodeNew();
      pgo = pgo->next;
    }
  }
  
  LL_InitUndoCodeBase();  
  return LL_SUCCESS;
 
}

/* searches for the key stored in ppomim in the LL_OMIM db.
   If an error occurs, return LL_ERROR. If the key is not found in the db, 
   returns LL_NOTFOUND, otherwise, fills in ppomim with the
   necessary field data and returns LL_SUCCESS. */
Int4 LL_SearchOMIMDB(StOMIMdbPtr PNTR ppomim)
{
  StOMIMdbPtr pomim = NULL;
  Int4 key = 0;
  CharPtr search_omim = NULL;
  CharPtr gi_stmt = "select gi,omim_id from seqhound.omimdb where gi = ?";
  CharPtr omim_stmt = "select gi,omim_id from seqhound.omimdb where omim_id = ?";
  SQLHANDLE hstmt;  
 Int2 sqlrc = SQL_SUCCESS;
  struct{
     SQLINTEGER len;
     SQLINTEGER val;
   }gi, omim; 

  LL_InitCodeBase();

  /* SK: NULL ptr check */
  if(ppomim == NULL || *ppomim == NULL)
  {
    ErrPostEx(SEV_ERROR,0,0, "LL_SearchOMIMDB: StOMIMdbPtr is NULL.");
    return LL_ERROR;
  }
      
  pomim = *ppomim;
  if(pomim->npid > 0){
    key = pomim->npid;
    /* tag select the npid */
    search_omim = gi_stmt;
  }
  if(pomim->omimid > 0){
    key = pomim->omimid;
    /* tag select the goid */
    search_omim = omim_stmt;
  }  

 /* search omimDB*/
   CreateHandle(&hstmt,search_omim,&henv,&hdbc);
    
  SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_LONG,SQL_INTEGER, 0, 0, &key, 0, NULL);
   
/* set auto commit on */
  SQLSetConnectAttr( hdbc, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_ON, SQL_NTS);
    
  /* execute statment */ 
  if (SQLExecute(hstmt) != SQL_SUCCESS){
    ErrPostEx(SEV_ERROR,0,0,"SEARCH_OMIM:unable to execute select statement.");
    print_err(hstmt);
    SQLFreeStmt(hstmt, SQL_DROP);
    return FALSE;
  }
    
  SQLBindCol(hstmt,1,SQL_C_LONG,&gi.val,0,&gi.len);
  SQLBindCol(hstmt,2,SQL_C_LONG,&omim.val,0,&omim.len);

 sqlrc = SQLFetch(hstmt);
  if(sqlrc == SQL_NO_DATA_FOUND){
    SQLFreeStmt(hstmt, SQL_DROP);
    ErrPostEx(SEV_INFO, 0, 0, "Search_OMIM : No data found for key : %ld", key);
  
    LL_InitUndoCodeBase();
    return LL_NOTFOUND;
  }
  
  while(sqlrc == SQL_SUCCESS || sqlrc == SQL_SUCCESS_WITH_INFO){
      pomim->npid = gi.val;
      pomim->omimid = omim.val;

    /*Get next record, if there is one add a node to the valnode.*/
    sqlrc = SQLFetch(hstmt);
    if(sqlrc == SQL_SUCCESS || sqlrc == SQL_SUCCESS_WITH_INFO){
      pomim->next = StOMIMdbNodeNew();
      pomim = pomim->next;

    }
  }

 LL_InitUndoCodeBase();
 return LL_SUCCESS;
 
}

/* searches for the key stored in ppomim in the LL_OMIM db. 
   If an error occurs, return LL_ERROR. If the key is not found in the db, 
   returns LL_NOTFOUND, otherwise, fills in ppomim with the
   necessary field data and returns LL_SUCCESS. */
Int4 LL_SearchCDDDB(StCDDdbPtr PNTR ppcdd)
{
  StCDDdbPtr pcdd = NULL;
  Int4 key = 0;
  CharPtr szKey = NULL;
  Int4 szKeyLen = 0;
  Int2 sqlrc = SQL_SUCCESS;
  CharPtr search_cdddb = NULL;

  CharPtr gi_stmt = "select gi,cdd_id,evalue from seqhound.cdddb where gi = ?";
  CharPtr cddid_stmt = "select gi,cdd_id,evalue from seqhound.cdddb where cdd_id = ?";

  SQLHANDLE hstmt;

  struct{
     SQLINTEGER len;
     SQLINTEGER val;
   }gi;

  struct{
     SQLINTEGER len;
     Char val[20];
   }cdd;   

  struct{
     SQLINTEGER len;
     SQLDECIMAL val;
   }eval;

   LL_InitCodeBase();

  /* SK: NULL ptr check */
  if(ppcdd == NULL || *ppcdd == NULL)
  {
    ErrPostEx(SEV_ERROR,0,0, "LL_SearchCDDDB: StCDDdbPtr is NULL.");
    return LL_ERROR;
  }
    

  pcdd = *ppcdd;
  if(pcdd->npid > 0){
    key = pcdd->npid;
    /* tag select the npid */
    search_cdddb = gi_stmt;
  }
  
if(StrLen(pcdd->cddid) > 0){
    szKey = MemNew(20*sizeof(Char));
    sprintf(szKey,"%s",IsNullStr(pcdd->cddid));
    szKeyLen = StrLen(szKey);
    /* tag select the cdd id */
    search_cdddb = cddid_stmt;
  }

/* search cddDB*/
   CreateHandle(&hstmt,search_cdddb,&henv,&hdbc);
   
  if(pcdd->npid > 0) {
  	SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_LONG,SQL_INTEGER, 0, 0, &key, 0, NULL);
  }
  else {
	SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_DEFAULT,SQL_VARCHAR, 20, 0, szKey, 0, (long*)&szKeyLen);
  }


/* set auto commit on */   
  SQLSetConnectAttr( hdbc, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_ON, SQL_NTS);
 
  /* execute statment */
  if (SQLExecute(hstmt) != SQL_SUCCESS){
    ErrPostEx(SEV_ERROR,0,0,"SEARCH_CDDDB:unable to execute select statement.");
    print_err(hstmt);
    SQLFreeStmt(hstmt, SQL_DROP);
    LL_InitUndoCodeBase();
    return FALSE;
  }  
   
  SQLBindCol(hstmt,1,SQL_C_LONG,&gi.val,0,&gi.len);
  SQLBindCol(hstmt,2,SQL_C_CHAR,&cdd.val,20,&cdd.len);
  SQLBindCol(hstmt,3,SQL_DECIMAL,&eval.val,0,&eval.len);
   
  
 sqlrc = SQLFetch(hstmt);
  if(sqlrc == SQL_NO_DATA_FOUND){
    SQLFreeStmt(hstmt, SQL_DROP);
    if(pcdd->npid > 0) {
	    ErrPostEx(SEV_INFO, 0, 0, "Search_LLDB : No data found for key : %ld", key);
    }
    else {
	    ErrPostEx(SEV_INFO, 0, 0, "Search_LLDB : No data found for key : %s", IsNullStr(szKey));
    }
    return LL_NOTFOUND;
  }

  while(sqlrc == SQL_SUCCESS || sqlrc == SQL_SUCCESS_WITH_INFO){
      pcdd->npid = gi.val;
      pcdd->cddid = StringSave(cdd.val);

      if(eval.val < 7){
        SLRI_SafeExp10(&eval.val);
        pcdd->eValue = eval.val;
      }else {
        pcdd->eValue = -1;
      }
 
    /*Get next record, if there is one add a node to the valnode.*/
    sqlrc = SQLFetch(hstmt);
    if(sqlrc == SQL_SUCCESS || sqlrc == SQL_SUCCESS_WITH_INFO){
      pcdd->next = StCDDdbNodeNew();
      pcdd = pcdd->next;

    }
  }

 LL_InitUndoCodeBase();

  if(szKey)
	MemFree(szKey);

 return LL_SUCCESS;

}

/* searches for the key stored in ppll in the LL_LINK db. 
   If an error occurs, return LL_ERROR. If the key is not found in the db, 
   returns LL_NOTFOUND, otherwise, fills in ppll with the
   necessary field data and returns LL_SUCCESS. */
Int4 LL_SearchLLDB(StLLdbPtr PNTR ppll)
{
 StLLdbPtr pll = NULL;
  Int4 key = 0;
  CharPtr search_lldb = NULL;
  CharPtr gi_stmt = "select gi,locuslink_id, map from seqhound.lldb where gi = ?";
  CharPtr ll_stmt = "select gi,locuslink_id, map from seqhound.lldb where locuslink_id = ?";
  SQLHANDLE hstmt;
   Int2 sqlrc = SQL_SUCCESS;
  struct{
     SQLINTEGER len;
     SQLINTEGER val;
   }gi, ll;
  struct{
     SQLINTEGER len;
     Char val[1024];
   }map;   

   LL_InitCodeBase();

  /* SK: NULL ptr check */
  if(ppll == NULL || *ppll == NULL)
  {
    ErrPostEx(SEV_ERROR,0,0, "LL_SearchLLDB: StLLdbPtr is NULL.");
    return LL_ERROR;
  }
    
  pll = *ppll;
  if(pll->npid > 0){
    key = pll->npid;
    /* tag select the npid */
    search_lldb = gi_stmt;
  }
  if(pll->llid > 0){
    key = pll->llid;
    /* tag select the goid */
    search_lldb = ll_stmt;
  }

/* search omimDB*/
   CreateHandle(&hstmt,search_lldb,&henv,&hdbc);
   
  SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_LONG,SQL_INTEGER, 0, 0, &key, 0, NULL);
  
/* set auto commit on */   
  SQLSetConnectAttr( hdbc, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_ON, SQL_NTS);
 
  /* execute statment */
  if (SQLExecute(hstmt) != SQL_SUCCESS){
    ErrPostEx(SEV_ERROR,0,0,"SEARCH_OMIM:unable to execute select statement.");
    print_err(hstmt);
    SQLFreeStmt(hstmt, SQL_DROP);
    LL_InitUndoCodeBase();
    return FALSE;
  }  
   
  SQLBindCol(hstmt,1,SQL_C_LONG,&gi.val,0,&gi.len);
  SQLBindCol(hstmt,2,SQL_C_LONG,&ll.val,0,&ll.len);
  SQLBindCol(hstmt,3,SQL_C_CHAR,map.val,1024,&map.len);

 sqlrc = SQLFetch(hstmt);
  if(sqlrc == SQL_NO_DATA_FOUND){
    SQLFreeStmt(hstmt, SQL_DROP);
    ErrPostEx(SEV_INFO, 0, 0, "Search_LLDB : No data found for key : %ld", key);
    return LL_NOTFOUND;
  }

  while(sqlrc == SQL_SUCCESS || sqlrc == SQL_SUCCESS_WITH_INFO){
      pll->npid = gi.val;
      pll->llid = ll.val;
      pll->map = StringSave(map.val);
 
    /*Get next record, if there is one add a node to the valnode.*/
    sqlrc = SQLFetch(hstmt);
    if(sqlrc == SQL_SUCCESS || sqlrc == SQL_SUCCESS_WITH_INFO){
      pll->next = StLLdbNodeNew();
      pll = pll->next;

    }
  }

 LL_InitUndoCodeBase();
 return LL_SUCCESS;
 
}


/**************************************************************
                 PRIVATE APPENDING FUNCTIONS			       
***************************************************************/
/* Adds a entry consisting of npid, goID, PMID and eviCode to
   LL_GO and then returns LL_APPEND_SUCCESS. */
static Int4 LL_Append2GO_DB(Int4 NPid, Int4 goID, Int4 PMID, CharPtr eviCode)
{
   Char evidence_insert[50];
   Int4 evidencelen= 0;
	
  if(NPid <= 0){
    ErrPostEx(SEV_ERROR, 0, 0, "LL_Append2GO: Invalid NP id on record with PMID: %d.\nRecord skipped.", PMID);
    return LL_ERROR;
  }
  if(goID <= 0){
    ErrPostEx(SEV_ERROR, 0, 0, "LL_Append2GO: Invalid go id on record with NPID: %d.\nRecord skipped.", NPid);
    return LL_ERROR;
  }
  if(PMID <= 0){
    ErrPostEx(SEV_ERROR, 0, 0, "LL_Append2GO: Invalid pmid on record with NPID: %d.\nRecord skipped.", NPid);
    return LL_ERROR;
  }
  if(eviCode == NULL){
    ErrPostEx(SEV_ERROR, 0, 0, "LL_Append2GO: Invalid E Code on record with NPID: %d.\nRecord skipped.", NPid);
    return LL_ERROR;
  }

    /*set up the input parameters */
   SQLBindParameter(hstmt_godb, 1, SQL_PARAM_INPUT, SQL_C_LONG,SQL_INTEGER, 0, 0, &NPid, 0, NULL);
   SQLBindParameter(hstmt_godb, 2, SQL_PARAM_INPUT, SQL_C_LONG,SQL_INTEGER, 0, 0, &goID, 0, NULL);
   SQLBindParameter(hstmt_godb, 3, SQL_PARAM_INPUT, SQL_C_LONG,SQL_INTEGER, 0, 0, &PMID, 0, NULL);
   SQLBindParameter(hstmt_godb, 4, SQL_PARAM_INPUT, SQL_C_DEFAULT,SQL_VARCHAR, 50, 0, evidence_insert, 0, (long*)&evidencelen);
  
   sprintf(evidence_insert,"%s",IsNullStr(eviCode));
   evidencelen = strlen(evidence_insert);

  /* execute statment */
  if (SQLExecute (hstmt_godb) != SQL_SUCCESS){
	ErrPostEx(SEV_ERROR,0,0, "LL_Append2GO_DB: unable to execute Godb insert statement. go_id = %ld ", goID);
	print_err(hstmt_godb);
	return LL_ERROR;
  }

  return LL_APPEND_SUCCESS;
}

/* adds an entry consisting of NPid and OMIM to the LL_OMIM DB and 
   LL_APPEND_SUCCESS. */
Int4 LL_Append2OMIM_DB(Int4 NPid, Int4 OMIM)
{
  if(NPid <= 0){
    ErrPostEx(SEV_ERROR, 0, 0, "LL_Append2OMIM: Invalid npid on record with OMIM: %d.\nRecord skipped.", OMIM);
    return LL_ERROR;
  }
  if(OMIM <= 0){
    ErrPostEx(SEV_ERROR, 0, 0, "LL_Append2OMIM: Invalid omim on record with NP id: %d.\nRecord skipped.", NPid);
    return LL_ERROR;
  }
  /*set up the input parameters */
   SQLBindParameter(hstmt_omimdb, 1, SQL_PARAM_INPUT, SQL_C_LONG,SQL_INTEGER, 0, 0, &NPid, 0, NULL);
   SQLBindParameter(hstmt_omimdb, 2, SQL_PARAM_INPUT, SQL_C_LONG,SQL_INTEGER, 0, 0, &OMIM, 0, NULL);

  /* execute statment */
  if (SQLExecute (hstmt_omimdb) != SQL_SUCCESS){
	ErrPostEx(SEV_ERROR,0,0, "LL_Append2Omim: unable to execute Omimdb insert statement. gi = %ld ", NPid);
	print_err(hstmt_omimdb);
	return LL_ERROR;
  }

  return LL_APPEND_SUCCESS;
}

/* adds an entry consisting of NPid, CDD, evalue to the LL_CDD DB 
   and returns LL_APPEND_SUCCESS */
Int4 LL_Append2CDD_DB(Int4 NPid, CharPtr CDD, FloatHi evalue)
{
   Char cdd_insert[50];
   Int4 cddlen= 0;

  if(NPid <= 0){
    ErrPostEx(SEV_ERROR, 0, 0, "LL_Append2CDD: Invalid npid on record with CDD: %s.\nRecord skipped.", IsNullStr(CDD));
    return LL_ERROR;
  }
  if(CDD == NULL){
    ErrPostEx(SEV_ERROR, 0, 0, "LL_Append2CDD: Invalid cdd id on record with NPID: %d.\nRecord skipped.", NPid);
    return LL_ERROR;
  }
  if(evalue <= 0){
    ErrPostEx(SEV_ERROR, 0, 0, "LL_Append2CDD: Invalid evalue on record with NPID: %d.\nRecord skipped.", NPid);
    return LL_ERROR;
  }
 /*set up the input parameters */
   SQLBindParameter(hstmt_cdddb, 1, SQL_PARAM_INPUT, SQL_C_LONG,SQL_INTEGER, 0, 0, &NPid, 0, NULL);
   SQLBindParameter(hstmt_cdddb, 2, SQL_PARAM_INPUT, SQL_C_DEFAULT,SQL_VARCHAR, 50, 0, cdd_insert, 0, (long*)&cddlen);
   SQLBindParameter(hstmt_cdddb, 3, SQL_PARAM_INPUT, SQL_C_FLOAT,SQL_DECIMAL, 20, 10, &evalue, 0, NULL);

   sprintf(cdd_insert,"%s",IsNullStr(CDD));
   cddlen = strlen(cdd_insert);

  /* execute statment */
  if (SQLExecute (hstmt_cdddb) != SQL_SUCCESS){
	ErrPostEx(SEV_ERROR,0,0, "LL_Append2CDD_DB: unable to execute cdddb insert statement. go_id = %ld ", NPid);
	print_err(hstmt_cdddb);
	return LL_ERROR;
  }

  return LL_APPEND_SUCCESS;
}

/* adds an entry consisting of NPid, LLid, pMAP and then 
   returns GO_APPEND_SUCCESS */
Int4 LL_Append2LLID_DB(Int4 NPid, Int4 LLid, CharPtr pMAP)
{
   Char map_insert[1024];
   Int4 maplen= 0;

  if(NPid <= 0){
    ErrPostEx(SEV_ERROR, 0, 0, "LL_Append2LLID: Invalid npid on record with LL ID: %d.\nRecord skipped.", LLid);
    return LL_ERROR;
  }
  if(LLid <= 0){
    ErrPostEx(SEV_ERROR, 0, 0, "LL_Append2LLID: Invalid ll id on record with NPID: %d.\nRecord skipped.", NPid);
    return LL_ERROR;
  }
  if(pMAP == NULL){
    ErrPostEx(SEV_ERROR, 0, 0, "LL_Append2LLID: Invalid map on record with NPID: %d.\nRecord skipped.", NPid);
    return LL_ERROR;
  }
  /*set up the input parameters */
   SQLBindParameter(hstmt_lldb, 1, SQL_PARAM_INPUT, SQL_C_LONG,SQL_INTEGER, 0, 0, &NPid, 0, NULL);
   SQLBindParameter(hstmt_lldb, 2, SQL_PARAM_INPUT, SQL_C_LONG,SQL_INTEGER, 0, 0, &LLid, 0, NULL);
   SQLBindParameter(hstmt_lldb, 3, SQL_PARAM_INPUT, SQL_C_DEFAULT,SQL_VARCHAR, 1024, 0, map_insert, 0, (long*)&maplen);
  
   sprintf(map_insert,"%s",IsNullStr(pMAP));
   maplen = strlen(map_insert);

  /* execute statment */
  if (SQLExecute (hstmt_lldb) != SQL_SUCCESS){
	ErrPostEx(SEV_ERROR,0,0, "LL_Append2LLID_DB: unable to execute lldb insert statement. gi = %ld ", NPid);
	print_err(hstmt_lldb);
	return LL_ERROR;
  }

  return LL_APPEND_SUCCESS;;
}


/****************************************************************
                       PUBLIC APPEND FUNCTION
*****************************************************************/
/* appends a record stored in ps to the 4 data bases LL_GO, LL_CDD,
   LL_OMIM, LL_LLINK. This program does not check for duplicates. If
   you are reparsing the locus link database, delete the old files 
   before reparsing, otherwise you will have doubles of all entries.
   Returns -1 if a record is skipped (those that don't have gi). Otherwise 
   returns 0.
*/
Int4 LL_AppendRecord(LL_ParseStructPtr ps)
{
  ValNodePtr vnp1 = NULL,
             vnp2 = NULL,
             vnp3 = NULL;
  LL_CDDStructPtr cdd_tmp;
  
  /* adding a name and go id entry for each go id */
  /* SK: NULL ptr check */
  if(ps == NULL || ps->cdd_st == NULL)
  {
    ErrPostEx(SEV_INFO, 0, 0, "LL_AppendRecord: No NP id. Record skipped.");
    /* printf("Record skipped\n"); */
    return -1;
  }
  
  /* adding to the GO DB */
  if(ps->vnpGOID != NULL){
    for(cdd_tmp = ps->cdd_st; cdd_tmp != NULL; cdd_tmp = cdd_tmp->next){
      for(vnp1 = ps->vnpGOID, vnp2 = ps->vnpPMID, vnp3 = ps->vnpE_CODE; 
	  vnp1 != NULL; vnp1 = vnp1->next, vnp2 = vnp2->next, vnp3 = vnp3->next){
	LL_Append2GO_DB(cdd_tmp->iNPid, vnp1->data.intvalue, 
			vnp2->data.intvalue, vnp3->data.ptrvalue);
      }
    }
  }
  
  /* add to the OMIM DB */
  if (ps->vnpOMIM != NULL){
    for(cdd_tmp = ps->cdd_st; cdd_tmp != NULL; cdd_tmp = cdd_tmp->next){
      for(vnp1 = ps->vnpOMIM; vnp1 != NULL; vnp1 = vnp1->next){
	LL_Append2OMIM_DB(cdd_tmp->iNPid, vnp1->data.intvalue);
      }
    }
  }
  
  /* add to the CDD DB */
  for(cdd_tmp = ps->cdd_st; cdd_tmp != NULL; cdd_tmp = cdd_tmp->next){
    for(vnp1 = cdd_tmp->vnpCDD_ID, vnp2 = cdd_tmp->vnpE_VAL; vnp1 != NULL; 
	vnp1 = vnp1->next, vnp2 = vnp2->next){
      LL_Append2CDD_DB(cdd_tmp->iNPid, vnp1->data.ptrvalue, vnp2->data.realvalue);
    }
  }
  
  /* add to the LL DB */
  for(cdd_tmp = ps->cdd_st; cdd_tmp != NULL; cdd_tmp = cdd_tmp->next){
    if(ps->cpMAP == NULL){
      LL_Append2LLID_DB(cdd_tmp->iNPid, ps->iLLID, "NULL");
    }
    else{
      LL_Append2LLID_DB(cdd_tmp->iNPid, ps->iLLID, ps->cpMAP);
    }
  }
  return 0;
}
