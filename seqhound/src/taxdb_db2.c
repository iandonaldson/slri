/*  $Id: taxdb_db2.c,v 1.3 2005/04/26 14:31:19 rcavero Exp $
-- ***************************************************************************
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
           Ruth Isserlin, Gary Bader and Christopher W.V. Hogue hogue@mshri.on.ca


  REVISION/CONTRIBUTION NOTES:

  July 10, 2001  Original MSH Revision.
                Hogue Lab - University of Toronto Biochemistry
                Department and the Samuel Lunenfeld Research
                Institute, Mount Sinai Hospital
                http://bioinfo.mshri.on.ca  hogue@mshri.on.ca
                ftp://ftp.mshri.on.ca/


  PUBLICATION TO CITE:


  DESCRIPTION
  taxonomy database part of SeqHound coded in db2


******************************************************************************************/

#include <sqlcli1.h>
#include <taxdb_db.h>
#include <slri_misc.h>

#include <slri_db2.h>

/*******************************/
/* Taxonomy database functions */
/*******************************/
/* two variables used in every method to communicate with 
the db2 database */

SQLHANDLE henv; /*envirnment handle */
SQLHANDLE hdbc; /* connection handle */
SQLHANDLE hstmt_tax, hstmt_div,hstmt_gcode, hstmt_del,hstmt_merge;


/*field initializers - must call these before any DB access*/
static void InittTAXdbFields(void)
{

}

static void InittDIVdbFields(void)
{

}

static void InittGCODEdbFields(void)
{

}

static void InittDELdbFields(void)
{

}

static void InittMERGEdbFields(void)
{

}

Boolean OpentTAXDB(Boolean Create)
{
  CreateHandle(&hstmt_tax,"INSERT into seqhound.taxdb(taxid,asn1) values(?,?)",&henv,&hdbc);
  return TRUE;
}

Boolean OpentDIVDB(Boolean Create)
{
 CreateHandle(&hstmt_div,"INSERT into seqhound.divdb(divid,asn1) values(?,?)",&henv,&hdbc);
  return TRUE;
}

Boolean OpentGCODEDB(Boolean Create)
{
 CreateHandle(&hstmt_gcode,"INSERT into seqhound.gcodedb(gcodeid,asn1) values(?,?)",&henv,&hdbc);
  return TRUE;
}

Boolean OpentDELDB(Boolean Create)
{
  CreateHandle(&hstmt_del, "insert into seqhound.deltaxdb(del_taxid) values(?)",&henv,&hdbc);
  return TRUE;
}

Boolean OpentMERGEDB(Boolean Create)
{
  CreateHandle(&hstmt_merge, "insert into seqhound.mergedtaxdb(old_taxid,new_taxid) values(?,?)",&henv,&hdbc);
  return TRUE;
}

/*Opens all taxonomy database - returns TRUE if any database was created*/
Boolean OpenAllTaxDBs(Boolean Create)
{
  InitCodeBaseTax();
  OpentTAXDB(Create);
  OpentDIVDB(Create);
  OpentGCODEDB(Create);
  OpentDELDB(Create);
  OpentMERGEDB(Create);
  return TRUE;
}

/*Initialize codebase for the taxonomy database*/
void InitCodeBaseTax(void)
{
 /*CHANGE : so the user name and database comes out of the seqhound 
    config files.*/
  SQLCHAR db2alias[10];
  SQLCHAR usr[10];
  SQLCHAR passwd[10];
  Char instance[10];
  Char instancebuf[25];

 GetAppParam("intrez", "datab", "username", NULL,(Char*) usr, (size_t)10 * (sizeof(Char)));
 GetAppParam("intrez", "datab", "password", NULL, (Char*)passwd, (size_t)10 * (sizeof(Char)));
 GetAppParam("intrez", "datab", "db2alias", NULL, (Char*)db2alias, (size_t)10 * (sizeof(Char)));
 GetAppParam("intrez", "datab", "instance", NULL, (Char*)instance, (size_t)10 * (sizeof(Char)));
         
 /*make sure the the dbeinst3 variable is defined--only for aix system*/
  sprintf(instancebuf,"DB2INSTANCE=%s",IsNullStr(instance));
  putenv(instancebuf);

   /* allocate an envirnment handle */
   if (SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv) != SQL_SUCCESS){
     print_err(henv);	
     ErrPostEx(SEV_ERROR,0,0, "Unable to allocate envirnment handle.");
   }

   /* allocate a connection handle */
   if (SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc) == SQL_ERROR){
	print_err(henv);
	ErrPostEx(SEV_ERROR,0,0, "An error has occurred!  Bastards!");
   }

   if(SQLConnect( hdbc, db2alias, SQL_NTS, usr, SQL_NTS, passwd, SQL_NTS) != SQL_SUCCESS){
	ErrPostEx(SEV_ERROR,0,0, "could not connect to the database. ");
   }


}

/*Close codebase for the taxonomy database*/
void CloseCodeBaseTax(void)
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

/*Close all taxonomy databases*/
void CloseAllTaxDBs(void)
{

}

/*Open the taxonomy database*/
Int2 OpenTaxDB(Boolean create)
{
	InitCodeBaseTax();
	OpenAllTaxDBs(FALSE);
	return(0);
}

/*Close the taxonomy database*/
Int2 CloseTaxDB(void)
{
	CloseCodeBaseTax();
	return(0);
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

/***************************/
/*Data Management Functions*/
/***************************/

/*main taxonomy database*/

/* Write an SLRI-taxon record to the database */
Int2 DB_WriteTaxRec(SLRITaxonPtr stp)
{
   pABL Asnbuflen=NULL;
   Int4 taxid, bsLength;
   CharPtr asn1;  
   
   if(stp == NULL) {
     ErrPostEx(SEV_INFO, 3, 0, "DB_WriteTaxRec: Passed ASN.1 pointer is null.");
     return(-1);
    }

   /*get the asn1 bioseq into a bytestore pointer so that we can put it into a buffer.*/
  Asnbuflen = AssignASNMemChar((Pointer) stp, (AsnWriteFunc) SLRITaxonAsnWrite);
  /* SK: NULL ptr check */
  if(Asnbuflen == NULL) {
     ErrPostEx(SEV_INFO, 3, 0, "DB_WriteTaxRec: NULL Asnbuflen.");
     return(-1);
  }
  
  asn1 = Asnbuflen->buf;
  bsLength = Asnbuflen->len;     

   /*set up the input parameters */
  SQLBindParameter(hstmt_tax, 1, SQL_PARAM_INPUT, SQL_C_LONG,SQL_INTEGER, 0, 0, &taxid, 0, NULL);
  SQLBindParameter(hstmt_tax, 2, SQL_PARAM_INPUT, SQL_C_DEFAULT,SQL_BLOB, 0, 0, asn1, 0, (long*)&bsLength);

 taxid = stp->taxId;
	
  /* execute statment */
  if (SQLExecute (hstmt_tax) != SQL_SUCCESS){
    ErrPostEx(SEV_ERROR,0,0, "DB_WriteTax: unable to execute insert statement. ");
    print_err(hstmt_tax);
    return FALSE;
  }

  asn1 = MemFree(asn1);
/*  pABLtmp->buf = MemFree(pABLtmp->buf);*/
  FreeABL(Asnbuflen);
   return TRUE;

}

/* Replace an SLRI-Taxon record in the database */
Int2 DB_ReplaceTaxRec(SLRITaxonPtr stp)
{
   Int4 bsLength=0;
   pABL Asnbuflen=NULL;
   Int4 taxid;
   CharPtr asn1;  
   SQLHANDLE hstmt;
   CharPtr update = "update seqhound.taxdb set asn1=? where taxid = ?";

   if(stp == NULL) {
     ErrPostEx(SEV_INFO, 3, 0, "DB_WriteTaxRec: Passed ASN.1 pointer is null.");
     return(-1);
    }
   
   CreateHandle(&hstmt,update,&henv,&hdbc);

   /*get the asn1 bioseq into a bytestore pointer so that we can put it into a buffer.*/
  Asnbuflen = AssignASNMemChar((Pointer) stp, (AsnWriteFunc) SLRITaxonAsnWrite);
  /* SK: NULL ptr check */
  if(Asnbuflen == NULL) {
     ErrPostEx(SEV_INFO, 3, 0, "DB_ReplaceTaxRec: NULL Asnbuflen.");
     return(-1);
  }
   asn1 = Asnbuflen->buf;
  bsLength = Asnbuflen->len;     
  
   /*set up the input parameters */
  SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_DEFAULT,SQL_BLOB, 0, 0, asn1, 0, (long*)&bsLength);
  SQLBindParameter(hstmt, 2, SQL_PARAM_INPUT, SQL_C_LONG,SQL_INTEGER, 0, 0, &taxid, 0, NULL);

 taxid = stp->taxId;
	
  /* execute statment */
  if (SQLExecute (hstmt) != SQL_SUCCESS){
    ErrPostEx(SEV_ERROR,0,0, "DB_ReplaceTax: unable to execute insert statement. ");
    print_err(hstmt);
    return FALSE;
  }

  asn1 = MemFree(asn1);
/*  pABLtmp->buf = MemFree(pABLtmp->buf);*/
  FreeABL(Asnbuflen);

  if(SQLFreeStmt(hstmt, SQL_DROP) != SQL_SUCCESS){
     print_err(hstmt);
     return FALSE;
   }

   return TRUE;
}

/* Get an SLRI-taxon record from the database */
SLRITaxonPtr DB_GetTaxRec(Int4 taxId)
{
   Int4 bloblen,indicator;
    SQLINTEGER blobloc, pbbvalue;
   SQLHANDLE LocUse;
   CharPtr pcASN = NULL;
   CharPtr search_taxdb = "select taxid,asn1 from seqhound.taxdb where taxid=?";
   SQLHANDLE hstmt;
   Int2 sqlrc=SQL_SUCCESS;
   SLRITaxonPtr stp=NULL;
   Int4 newTaxId=0;
   struct{
     Int4 len;
     Int4 val;
   }taxid;

 
   /* search TAXDB */
   CreateHandle(&hstmt,search_taxdb,&henv,&hdbc);
  
  SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_LONG,SQL_INTEGER, 0, 0, &taxId, 0, NULL);

  /* set auto commit on */
   SQLSetConnectAttr( hdbc, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_ON, SQL_NTS);  

  /* execute statment */
  if (SQLExecute(hstmt) != SQL_SUCCESS){
	ErrPostEx(SEV_ERROR,0,0,"Get_TaxRec:unable to execute select statement.");
	print_err(hstmt);
	SQLFreeStmt(hstmt, SQL_DROP);
	return FALSE;
  }

  SQLBindCol(hstmt,1,SQL_C_LONG,&taxid.val,10,(long*)&taxid.len); 
  SQLBindCol(hstmt,2, SQL_C_BLOB_LOCATOR,&blobloc,0,&pbbvalue);
  
  sqlrc = SQLFetch(hstmt);
  if(sqlrc == SQL_NO_DATA_FOUND){
      /*try looking in merged database before giving up*/
		newTaxId = DB_GetTaxMergedNodeRecNewTIDByOldTID(taxId);
		if(newTaxId<0) {
			ErrPostEx(SEV_INFO, 0, 0, "Taxonomy ID: %ld not found in database.", taxId);
			return(NULL);
		}
		else{
	        ErrPostEx(SEV_INFO, 0, 0, "Taxonomy ID:  found with another id : %ld.", newTaxId);
			return(NULL);
		}
  }
  else if(sqlrc == SQL_SUCCESS || sqlrc == SQL_SUCCESS_WITH_INFO){
      /*Get the length of of the blob so that we can read it */
      SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &LocUse);
      SQLGetLength(LocUse, SQL_C_BLOB_LOCATOR, blobloc, (long*)&bloblen, (long*)&indicator);

      /* allocate a buffer big enough to store the blob. */ 
      pcASN = (CharPtr) MemNew((size_t) bloblen);
      /*read the blob from the database */
      SQLGetData(hstmt,2,SQL_C_BINARY,(SQLPOINTER)pcASN,bloblen,(long*)&indicator);

      stp = (SLRITaxonPtr) GetASNMemChar(pcASN, (AsnReadFunc) SLRITaxonAsnRead,bloblen);
	  if(stp==NULL) {
		ErrPostEx(SEV_INFO, 3, 0, "DB_GetTaxRec: Call to GetASNMemMemo failed Taxonomy ID %ld.", taxId);
		return(NULL);
	}
      if (pcASN) MemFree(pcASN);
   
  }
  else{
    print_err(hstmt);
    return NULL;
  }

  if(SQLFreeStmt(hstmt, SQL_DROP) != SQL_SUCCESS){
     print_err(hstmt);
     return FALSE;
   }

   return(stp);

}

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

/*taxonomy division database*/

/* Write an SLRITaxonDiv record to the database */
Int2 DB_WriteTaxDivRec(SLRITaxonDivPtr stdp)
{
   Int4 bsLength=0;
   Int4 div_id;
   CharPtr asn1;
    pABL Asnbuflen=NULL;

    if(stdp == NULL) {
     ErrPostEx(SEV_INFO, 3, 0, "DB_WriteTaxDivRec: Passed ASN.1 pointer is null.");
     return(-1);
    }

   /*get the asn1 bioseq into a bytestore pointer so that we can put it into a buffer.*/
  Asnbuflen = AssignASNMemChar((Pointer) stdp, (AsnWriteFunc) SLRITaxonDivAsnWrite);
  /* SK: NULL ptr check */
  if(Asnbuflen == NULL) {
     ErrPostEx(SEV_INFO, 3, 0, "DB_WriteTaxDivRec: NULL Asnbuflen.");
     return(-1);
  }

  asn1 = Asnbuflen->buf;
  bsLength = Asnbuflen->len;     
   
   /*set up the input parameters */
  SQLBindParameter(hstmt_div, 1, SQL_PARAM_INPUT, SQL_C_LONG,SQL_INTEGER, 0, 0, &div_id, 0, NULL);
  SQLBindParameter(hstmt_div, 2, SQL_PARAM_INPUT, SQL_C_DEFAULT,SQL_BLOB, 0, 0, asn1, 0, (long*)&bsLength);

 div_id = stdp->div_id;
	
  /* execute statment */
  if (SQLExecute (hstmt_div) != SQL_SUCCESS){
    ErrPostEx(SEV_ERROR,0,0, "DB_WriteDiv: unable to execute insert statement. ");
    print_err(hstmt_div);
    return FALSE;
  }

  asn1 = MemFree(asn1);
  /*  pABLtmp->buf = MemFree(pABLtmp->buf);*/
  FreeABL(Asnbuflen); 
   return TRUE;

}

/* Get an SLRITaxonDiv record from the database */
SLRITaxonDivPtr DB_GetTaxDivRec(Int4 divId)
{
   SLRITaxonDivPtr stdp=NULL;
   Int4 bloblen,indicator;
   CharPtr search_divdb = "select divid,asn1 from seqhound.divdb where divid=?";
   SQLHANDLE hstmt;
   Int2 sqlrc=SQL_SUCCESS;
   SQLINTEGER blobloc, pbbvalue;
   SQLHANDLE LocUse;
   CharPtr pcASN = NULL;
   struct{
     Int4 len;
     Int4 val;
   }divid;
 
   /* search DIVDB */
   CreateHandle(&hstmt,search_divdb,&henv,&hdbc);

  SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_LONG,SQL_INTEGER, 0, 0, &divId, 0, NULL);

  /* set auto commit on */
   SQLSetConnectAttr( hdbc, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_ON, SQL_NTS);  

  /* execute statment */
  if (SQLExecute(hstmt) != SQL_SUCCESS){
	ErrPostEx(SEV_ERROR,0,0,"SEARCH_DIVDB:unable to execute select statement.");
	print_err(hstmt);
	SQLFreeStmt(hstmt, SQL_DROP);
	return FALSE;
  }

  SQLBindCol(hstmt,1,SQL_C_LONG,&divid.val,10,(long*)&divid.len); 
  SQLBindCol(hstmt,2, SQL_C_BLOB_LOCATOR,&blobloc,0,(long*)&pbbvalue);
  
  sqlrc = SQLFetch(hstmt);
  if(sqlrc == SQL_NO_DATA_FOUND){
    ErrPostEx(SEV_INFO, 0, 0, "Div ID: %ld not found in DIVDB.", divId);
	return(NULL);

  }
  else if(sqlrc == SQL_SUCCESS || sqlrc == SQL_SUCCESS_WITH_INFO){
      /*Get the length of of the blob so that we can read it */
      SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &LocUse);
      SQLGetLength(LocUse, SQL_C_BLOB_LOCATOR, blobloc, (long*)&bloblen, (long*)&indicator);

      /* allocate a buffer big enough to store the blob. */ 
      pcASN = (CharPtr) MemNew((size_t) bloblen);
      /*read the blob from the database */
      SQLGetData(hstmt,2,SQL_C_BINARY,(SQLPOINTER)pcASN,bloblen,(long*)&indicator);

      stdp = (SLRITaxonDivPtr) GetASNMemChar(pcASN, (AsnReadFunc) SLRITaxonDivAsnRead,bloblen);
	  if(stdp==NULL) {
		ErrPostEx(SEV_INFO, 3, 0, "DB_GetTaxDivRec: Call to GetASNMemMemo failed Taxonomy Division ID %ld.", divId);
		return(NULL);
	  }

      if (pcASN) MemFree(pcASN);
   
  }
  else{
    print_err(hstmt);
    return NULL;
  }

  if(SQLFreeStmt(hstmt, SQL_DROP) != SQL_SUCCESS){
     print_err(hstmt);
     return FALSE;
   }
   return(stdp);

	
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

/*Genetic code database*/

/* Write an SLRITaxonGencode record to the database */
Int2 DB_WriteTaxGenCodeRec(SLRITaxonGencodePtr stgp)
{
   Int4 bsLength=0;
   Int4 gencode_id;
   CharPtr asn1; 
    pABL Asnbuflen=NULL;

  	if(stgp == NULL) {
		ErrPostEx(SEV_INFO, 3, 0, "DB_WriteTaxGenCodeRec: Passed ASN.1 pointer is null.");
		return(-1);
	}

   /*get the asn1 bioseq into a bytestore pointer so that we can put it into a buffer.*/
  Asnbuflen = AssignASNMemChar((Pointer) stgp, (AsnWriteFunc) SLRITaxonGencodeAsnWrite);
  /* SK: NULL ptr check */
  if(Asnbuflen == NULL)
  {
    ErrPostEx(SEV_INFO, 3, 0, "DB_WriteTaxGenCodeRec: NULL Asnbuflen.");
    return(-1);
  }

  asn1 = Asnbuflen->buf;
  bsLength = Asnbuflen->len;     
   
   /*set up the input parameters */
  SQLBindParameter(hstmt_gcode, 1, SQL_PARAM_INPUT, SQL_C_LONG,SQL_INTEGER, 0, 0, &gencode_id, 0, NULL);
  SQLBindParameter(hstmt_gcode, 2, SQL_PARAM_INPUT, SQL_C_DEFAULT,SQL_BLOB, 0, 0, asn1, 0, (long*)&bsLength);

 gencode_id = stgp->gencode_id;
	
  /* execute statment */
  if (SQLExecute (hstmt_gcode) != SQL_SUCCESS){
    ErrPostEx(SEV_ERROR,0,0, "DB_WriteTaxGenCodeRec: unable to execute insert statement. ");
    print_err(hstmt_gcode);
    return FALSE;
  }

  asn1 = MemFree(asn1);
  /*  pABLtmp->buf = MemFree(pABLtmp->buf);*/
  FreeABL(Asnbuflen); 
   return TRUE;

}

/* Get an SLRITaxonGencode record from the database */
SLRITaxonGencodePtr DB_GetTaxGenCodeRec(Int4 genCodeId)
{
	SLRITaxonGencodePtr stgp=NULL;
   Int4 bloblen,indicator;
   CharPtr search_gcodedb = "select gcodeid,asn1 from seqhound.gcodedb where gcodeid=?";
   SQLHANDLE hstmt;
   Int2 sqlrc=SQL_SUCCESS;
   SQLINTEGER blobloc, pbbvalue;
   SQLHANDLE LocUse;
   CharPtr pcASN = NULL;
   struct{
     Int4 len;
     Int4 val;
   }gcodeid;
 
   /* search GCodeDB */
   CreateHandle(&hstmt,search_gcodedb,&henv,&hdbc);

  SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_LONG,SQL_INTEGER, 0, 0, &genCodeId, 0, NULL);

  /* set auto commit on */
   SQLSetConnectAttr( hdbc, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_ON, SQL_NTS);  

  /* execute statment */
  if (SQLExecute(hstmt) != SQL_SUCCESS){
	ErrPostEx(SEV_ERROR,0,0,"SEARCH_GCODEDB:unable to execute select statement.");
	print_err(hstmt);
	SQLFreeStmt(hstmt, SQL_DROP);
	return FALSE;
  }

  SQLBindCol(hstmt,1,SQL_C_LONG,&gcodeid.val,10,(long*)&gcodeid.len); 
  SQLBindCol(hstmt,2, SQL_C_BLOB_LOCATOR,&blobloc,0,&pbbvalue);
  
  sqlrc = SQLFetch(hstmt);
  if(sqlrc == SQL_NO_DATA_FOUND){
    ErrPostEx(SEV_INFO, 0, 0, "GenCode ID: %ld not found in GCODEDB.", genCodeId);
	return(NULL);

  }
  else if(sqlrc == SQL_SUCCESS || sqlrc == SQL_SUCCESS_WITH_INFO){
      /*Get the length of of the blob so that we can read it */
      SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &LocUse);
      SQLGetLength(LocUse, SQL_C_BLOB_LOCATOR, blobloc, (long*)&bloblen, (long*)&indicator);

      /* allocate a buffer big enough to store the blob. */ 
      pcASN = (CharPtr) MemNew((size_t) bloblen);
      /*read the blob from the database */
      SQLGetData(hstmt,2,SQL_C_BINARY,(SQLPOINTER)pcASN,bloblen,(long*)&indicator);

 	  stgp = (SLRITaxonGencodePtr) GetASNMemChar(pcASN, (AsnReadFunc) SLRITaxonGencodeAsnRead,bloblen);
	  if(stgp==NULL) {
		ErrPostEx(SEV_INFO, 3, 0, "DB_GetTaxGenCodeRec: Call to GetASNMemCahr failed Taxonomy Genetic Code ID %ld.", genCodeId);
		return(NULL);
	}

      if (pcASN) MemFree(pcASN);
   
  }
  else{
    print_err(hstmt);
    return NULL;
  }

   /* Free statement handle. */
   if(SQLFreeStmt(hstmt, SQL_DROP) != SQL_SUCCESS){
     print_err(hstmt);
     return FALSE;
   }
   return(stgp);

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

/*Deleted nodes history database*/

/* Write a deleted node record to the database */
Int2 DB_WriteTaxDelNodeRec(Int4 taxId)
{
	if(taxId < 0) {
		ErrPostEx(SEV_INFO, 3, 0, "DB_WriteTaxDelNodeRec: Passed taxId  < 0.");
		return(-1);
	}
   
   /*set up the input parameters */
   SQLBindParameter(hstmt_del, 1, SQL_PARAM_INPUT, SQL_C_LONG,SQL_INTEGER, 0, 0, &taxId, 0, NULL);

   /* set auto commit on */
   SQLSetConnectAttr( hdbc, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_ON, SQL_NTS);  

  /* execute statment */
  if (SQLExecute (hstmt_del) != SQL_SUCCESS){
    ErrPostEx(SEV_ERROR,0,0, "DB_WriteTaxDelRec: unable to execute insert statement. ");
    print_err(hstmt_del);
    return FALSE;
  }

   return TRUE;

}

/* Get a deleted node record from the database */
Int4 DB_GetTaxDelNodeRec(Int4 taxId)
{
   CharPtr search_deldb = "select del_taxid from seqhound.deltaxdb where del_taxid=?";
   SQLHANDLE hstmt;
   Int4 del_value = -1;
   Int2 sqlrc=SQL_SUCCESS;
   struct{
     Int4 len;
     Int4 val;
   }del_id;
 
   /* search GCodeDB */
   CreateHandle(&hstmt,search_deldb,&henv,&hdbc);

  SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_LONG,SQL_INTEGER, 0, 0, &taxId, 0, NULL);

  /* set auto commit on */
   SQLSetConnectAttr( hdbc, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_ON, SQL_NTS);  

  /* execute statment */
  if (SQLExecute(hstmt) != SQL_SUCCESS){
	ErrPostEx(SEV_ERROR,0,0,"DB_GetTaxDelNodeRec:unable to execute select statement.");
	print_err(hstmt);
	SQLFreeStmt(hstmt, SQL_DROP);
	return FALSE;
  }

  SQLBindCol(hstmt,1,SQL_C_LONG,&del_id.val,10,(long*)&del_id.len); 
 
  sqlrc = SQLFetch(hstmt);
  if(sqlrc == SQL_NO_DATA_FOUND){
    ErrPostEx(SEV_INFO, 0, 0, "Del Tax ID: %ld not found in del_taxdb.", taxId);
	return(-1);
  }
  else if(sqlrc == SQL_SUCCESS || sqlrc == SQL_SUCCESS_WITH_INFO){
    del_value = del_id.val; 
	}
  else{
    print_err(hstmt);
    return (-1);
  }

   /* Free statement handle. */
     if(SQLFreeStmt(hstmt, SQL_DROP) != SQL_SUCCESS){
     print_err(hstmt);
     return (-1);
   }
   return(del_value);

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

/*Merged nodes history database*/

/* Write a merged node record to the database */
Int2 DB_WriteTaxMergedNodeRec(Int4 oldTaxId, Int4 newTaxId)
{

	if(oldTaxId < 0) {
		ErrPostEx(SEV_INFO, 3, 0, "DB_WriteTaxMergedNodeRec: Passed oldTaxId  < 0.");
		return(-1);
	}
	if(newTaxId < 0) {
		ErrPostEx(SEV_INFO, 3, 0, "DB_WriteTaxMergedNodeRec: Passed newTaxId  < 0.");
		return(-1);
	}

   
   /*set up the input parameters */
   SQLBindParameter(hstmt_merge, 1, SQL_PARAM_INPUT, SQL_C_LONG,SQL_INTEGER, 0, 0, &oldTaxId, 0, NULL);
   SQLBindParameter(hstmt_merge, 2, SQL_PARAM_INPUT, SQL_C_LONG,SQL_INTEGER, 0, 0, &newTaxId, 0, NULL);

   /* set auto commit on */
   SQLSetConnectAttr( hdbc, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_ON, SQL_NTS);  

  /* execute statment */
  if (SQLExecute (hstmt_merge) != SQL_SUCCESS){
    ErrPostEx(SEV_ERROR,0,0, "DB_WriteTaxMergedRec: unable to execute insert statement. ");
    print_err(hstmt_merge);
    return FALSE;
  }

   return TRUE;

}

/* Get an old Tax ID given a new Tax ID from the merged node database */
Int4 DB_GetTaxMergedNodeRecOldTIDByNewTID(Int4 newTaxId)
{
  CharPtr search_mergeddb = "select old_taxid from seqhound.mergedtaxdb where new_taxid=?";
   SQLHANDLE hstmt;
   Int4 old_value = -1;
   Int2 sqlrc=SQL_SUCCESS;
   struct{
     Int4 len;
     Int4 val;
   }old_id;
 
   /* search GCodeDB */
   CreateHandle(&hstmt,search_mergeddb,&henv,&hdbc);

  SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_LONG,SQL_INTEGER, 0, 0, &newTaxId, 0, NULL);

  /* set auto commit on */
   SQLSetConnectAttr( hdbc, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_ON, SQL_NTS);  

  /* execute statment */
  if (SQLExecute(hstmt) != SQL_SUCCESS){
	ErrPostEx(SEV_ERROR,0,0,"DB_GetTaxMergedNodeRecOldTIDByNewTID:unable to execute select statement.");
	print_err(hstmt);
	SQLFreeStmt(hstmt, SQL_DROP);
	return FALSE;
  }

  SQLBindCol(hstmt,1,SQL_C_LONG,&old_id.val,10,(long*)&old_id.len); 
 
  sqlrc = SQLFetch(hstmt);
  if(sqlrc == SQL_NO_DATA_FOUND){
    ErrPostEx(SEV_INFO, 0, 0, "new Tax ID: %ld not found in Mergedtaxdb.", newTaxId);
	return(-1);
  }
  else if(sqlrc == SQL_SUCCESS || sqlrc == SQL_SUCCESS_WITH_INFO){
    old_value = old_id.val; 
	}
  else{
    print_err(hstmt);
    return (-1);
  }

   /* Free statement handle. */
   if(SQLFreeStmt(hstmt, SQL_DROP) != SQL_SUCCESS){
     print_err(hstmt);
     return (-1);
   }
   return(old_value);

}

/* Get a new Tax ID given an old Tax ID from the merged node database */
Int4 DB_GetTaxMergedNodeRecNewTIDByOldTID(Int4 oldTaxId)
{
CharPtr search_mergeddb = "select new_taxid from seqhound.mergedtaxdb where old_taxid=?";
   SQLHANDLE hstmt;
   Int4 new_value = -1;
   Int2 sqlrc=SQL_SUCCESS;
   struct{
     Int4 len;
     Int4 val;
   }new_id;
 
   /* search merged db*/
   CreateHandle(&hstmt,search_mergeddb,&henv, &hdbc);

  SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_LONG,SQL_INTEGER, 0, 0, &oldTaxId, 0, NULL);

  /* set auto commit on */
   SQLSetConnectAttr( hdbc, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_ON, SQL_NTS);  

  /* execute statment */
  if (SQLExecute(hstmt) != SQL_SUCCESS){
	ErrPostEx(SEV_ERROR,0,0,"DB_GetTaxMergedNodeRecNewTIDByOldTID:unable to execute select statement.");
	print_err(hstmt);
	SQLFreeStmt(hstmt, SQL_DROP);
	return FALSE;
  }

  SQLBindCol(hstmt,1,SQL_C_LONG,&new_id.val,10,(long*)&new_id.len); 
 
  sqlrc = SQLFetch(hstmt);
  if(sqlrc == SQL_NO_DATA_FOUND){
    ErrPostEx(SEV_INFO, 0, 0, "Old Tax ID: %ld not found in Mergedtaxdb.", oldTaxId);
	return(-1);
  }
  else if(sqlrc == SQL_SUCCESS || sqlrc == SQL_SUCCESS_WITH_INFO){
    new_value = new_id.val; 
	}
  else{
    print_err(hstmt);
    return (-1);
  }

   /* Free statement handle. */
   if(SQLFreeStmt(hstmt, SQL_DROP) != SQL_SUCCESS){
     print_err(hstmt);
     return (-1);
   }
   return(new_value);

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

/*Function for usein importtaxdb*/
/* This function takes each node in the taxdb database.  It gets its parent node
  and then the current node is added to the list of children of the parent node  
  that was retrieved.  This updated parent node is then replaced in the database.*/
Int2 TaxIDDisk_BuildChildrenNodeList(void)
{
	Int4 bloblen,indicator;
    SQLINTEGER blobloc, pbbvalue;
    SQLHANDLE LocUse;
    CharPtr pcASN = NULL;
	SLRITaxonPtr stp=NULL, stpParent=NULL;
	Int4 parentTaxId=0;
	CharPtr search_taxdb = "select * from seqhound.taxdb";
    SQLHANDLE hstmt;
    Int2 sqlrc=SQL_SUCCESS;
    struct{
     Int4 len;
     Int4 val;
    }tax_id;
 
   /* search tax db*/
   CreateHandle(&hstmt,search_taxdb, &henv,&hdbc);

  /* set auto commit on */
   SQLSetConnectAttr( hdbc, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_ON, SQL_NTS);  

  /* execute statment */
  if (SQLExecute(hstmt) != SQL_SUCCESS){
	ErrPostEx(SEV_ERROR,0,0,"TaxIDDisk_BuildChildrenNodeList:unable to execute select statement.");
	print_err(hstmt);
	SQLFreeStmt(hstmt, SQL_DROP);
	return FALSE;
  }

  SQLBindCol(hstmt,1,SQL_C_LONG,&tax_id.val,10,(long*)&tax_id.len); 
  SQLBindCol(hstmt,2, SQL_C_BLOB_LOCATOR,&blobloc,0,&pbbvalue); 
 
  sqlrc = SQLFetch(hstmt);
  if(sqlrc == SQL_NO_DATA_FOUND){
    ErrPostEx(SEV_INFO, 0, 0, "Nothing in the Taxonomy database");
	return(-1);
  }
  while(sqlrc == SQL_SUCCESS || sqlrc == SQL_SUCCESS_WITH_INFO){
	/*Get the length of of the blob so that we can read it */
      SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &LocUse);
      SQLGetLength(LocUse, SQL_C_BLOB_LOCATOR, blobloc, (long*)&bloblen, (long*)&indicator);

      /* allocate a buffer big enough to store the blob. */ 
      pcASN = (CharPtr) MemNew((size_t) bloblen);
      /*read the blob from the database */
      SQLGetData(hstmt,2,SQL_C_BINARY,(SQLPOINTER)pcASN,bloblen,(long*)&indicator);

      stp = (SLRITaxonPtr) GetASNMemChar(pcASN, (AsnReadFunc) SLRITaxonAsnRead,bloblen);

	if(stp==NULL) {
		ErrPostEx(SEV_INFO, 3, 0, "DB_GetTaxRec: Call to GetASNMemMemo failed for record after Taxonomy ID %ld.", parentTaxId);
		return(1);
	}
	parentTaxId = stp->parent_taxId;
	if(parentTaxId==stp->taxId) {
		stp = SLRITaxonFree(stp);
		continue;	/*don't allow any circles in the tree*/
	}
	stpParent = DB_GetTaxRec(parentTaxId);
	if(stpParent==NULL) {
		ErrPostEx(SEV_INFO, 0, 0, "TaxIDDisk_BuildChildrenNodeList: DB_GetTaxRec failed for Tax ID %ld.", parentTaxId);
		stp = SLRITaxonFree(stp);
		return(1);
	}
	Misc_AddInt2SortedList(&(stpParent->children_taxId), stp->taxId, 0, TRUE);
	DB_ReplaceTaxRec(stpParent);
	/*d4seekDouble(pd4tTAXdb, stp->taxId);*/	/*reposition database for d4skip call*/
	stp = SLRITaxonFree(stp);
	stpParent = SLRITaxonFree(stpParent);

	sqlrc = SQLFetch(hstmt);
	}

 /* Free statement handle. */
   if(SQLFreeStmt(hstmt, SQL_DROP) != SQL_SUCCESS){
     print_err(hstmt);
     return (-1);
   }

	return(0);
}

/* 
   $Log: taxdb_db2.c,v $
   Revision 1.3  2005/04/26 14:31:19  rcavero
   Change Copyright Note.

   Revision 1.2  2004/01/06 22:32:17  skuznets
   Code audit has been done sucessfully.
   Possible NULL pointer issues has been eliminated.

   Revision 1.1.1.4  2002/03/04 09:25:30  root
   Automated import from Sourceforge.net

   Revision 1.4  2002/03/03 23:02:52  gbader
   Fixed MSVC compiler warnings - mostly unused variables and uncasted type changes.

   Revision 1.3  2002/02/28 17:23:02  micheld
   New MSVC project files to build cb/db2/rem apps and libraries
   Moved include order to accomodate msvc, removed #ifdef [CODEBASE|DB2]

   Revision 1.2  2002/02/12 16:21:23  mdumontier
   fixed warnings

   Revision 1.1  2002/01/03 20:44:28  risserlin
   added additional db2 layer files

   Revision 1.4  2001/12/11 19:35:33  risserlin
   merged with the latest version of slri on dec 3 and then got working on fern

   Revision 1.2  2001/09/06 20:07:20  risserlin
   did all aix specific changes for db2

   Revision 1.1.1.1  2001/09/06 13:44:30  risserlin
   initial import of seqhound code retrieved from slri on Ausust 16 and revised to include all mdsp changes

   Revision 1.1.1.1  2001/04/05 15:48:59  ruth
   lateset code as of March 30

   Revision 1.2  2001/03/29 15:34:07  gbader
   Added slri_misc.h include

   Revision 1.1  2001/03/20 16:22:28  kaca
   migrated from ../taxon directory; library shoundtaxlib merged with shoundlocllib

   Revision 1.5  2001/02/23 21:20:11  kaca
   license change

   Revision 1.4  2000/09/20 21:38:47  kaca
   fixed Log keyword

   Revision 1.3  2000/09/19 23:44:42  kaca
   added CVS Id and Log keywords
*/



