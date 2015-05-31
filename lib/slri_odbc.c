/* $Id: slri_odbc.c,v 1.7 2004/10/01 19:19:02 zwang Exp $ */
#include <sql.h>
#include <slri_odbc.h>
#include <bzlib.h>
#include <slri_asn.h>

/******************************************************************/
/*  Given : a  pointer to the binary info from the database stored*/
/* in a character string, pointer to the asn read function and the*/
/* length of the buffer this method extracts the asn from the     */
/* buffer and returns the asn1 structure.                         */
/******************************************************************/
Pointer GetASNMemChar(CharPtr pcASN1, AsnReadFunc pAsnRead, Int4 length)
{
    AsnIoMemPtr aimp = NULL;
    Pointer pASNdata=NULL;

    if (pcASN1 == NULL) 
    {
	 ErrPostEx(SEV_ERROR, 0, 0, "GetASNMemMemo: pf4Field is NULL.");
	 return NULL;
    }
    if (pAsnRead == NULL) 
    {
	 ErrPostEx(SEV_ERROR, 0, 0, "GetASNMemMemo: pAsnRead is NULL.");
	 return NULL;
    }

    /*  get length of the memo field  */
    if(length<=0) 
    {
	ErrPostEx(SEV_ERROR, 0, 0, "GetASNMemMemo: Length must be a positive number.");
        return(NULL);
    }

    /*  open the asn io stream  */
    aimp = AsnIoMemOpen(GetASNReadMode(), (BytePtr) pcASN1, length);
    if (aimp == NULL) 
    {
	ErrPostEx(SEV_ERROR, 0, 0, "GetASNMemMemo: AsnIoMemPtr is NULL.");
	return NULL;
    }

    /*  read the asn.1 into a data structure  */
    pASNdata = (Pointer) (*pAsnRead) (aimp->aip, NULL);
    if (pASNdata == NULL)
    {
	ErrPostEx(SEV_ERROR,0,0, "GetBioseqfromASNDB: Bioseq pointer is NULL.");
        if (pASNdata) MemFree(pASNdata);
	AsnIoMemClose(aimp);
	return NULL;
    }

    AsnIoFlush(aimp->aip);

    /*  clean up  */
    AsnIoMemClose(aimp);

    /*  return a pointer to the data structure  */
    return(pASNdata);
}


/***********************************************************************/
/* Stores binary blobs into a byte store so that it can be loaded into db2*/
/*  Given : the pointer to the asn1 data and a pointer to the function */
/* that writes this asn1 pointer to memory.                            */
/*  Returns :  a structure that contains the asn buffer and the        */
/* length of the byteStorePtr that stored the asn1 structure.     */
/*  IT IS THE CALLER OF THIS FUNCTION THAT IS RESPONSIBLE FOR FREEING  */
/*  THE STRUCTURE THAT IS RETURNED.                                    */
/***********************************************************************/
pABL AssignASNMemChar(Pointer pASNdata, AsnWriteFunc pAsnWrite)
{
   AsnIoBSPtr aibp=NULL;
   ByteStorePtr bsp=NULL;
   Int4 size=5000;	         /*  start size of ByteStore  */
   CharPtr pcASN=NULL;
   Int4 bsLength=0;
   pABL pABLtmp=NULL;

   if (pASNdata == NULL) 
   {
       ErrPostEx(SEV_ERROR, 0, 0, "AssignASNMemChar: pASNdata is NULL.");
       return NULL; 
   }
   
   if (pAsnWrite == NULL) 
   { 
       ErrPostEx(SEV_ERROR, 0, 0, "AssignASNMemChar: pAsnWrite is NULL.");
       return NULL;
   }     

   pABLtmp = MemNew((size_t) sizeof (ABL));

   bsp = BSNew(size);
   BSSeek(bsp, 0L, SEEK_SET);
   aibp = AsnIoBSOpen(GetASNWriteMode(), bsp);
   if (aibp == NULL) 
   {
       ErrPostEx(SEV_ERROR, 0, 0, "AssignASNMemChar: AsnIoBSPtr is NULL.");
       return NULL;
   }
   if( ! (*pAsnWrite) (pASNdata, aibp->aip, NULL)) 
   {
       AsnIoBSClose(aibp);
       ErrPostEx(SEV_ERROR, 0, 0, "AssignASNMemChar: Asn ByteStore mapped write function failed.");
       bsp = BSFree(bsp);
       return NULL;
   }
   aibp = AsnIoBSClose(aibp);
	
   if ( bsp != NULL)
   { 
        BSSeek(bsp, 0L, SEEK_SET);
	bsLength = BSLen(bsp);
	pcASN = MemNew((size_t) bsLength);
	pABLtmp->len = bsLength;

	if(pcASN == NULL) 
        {
	    ErrPostEx(SEV_ERROR, 0, 0, "AssignASNMemChar: Out of memory.");
	    bsp = BSFree(bsp);
	    return FALSE;
	}
	if(BSRead(bsp, pcASN, bsLength) != bsLength) 
        {
	    ErrPostEx(SEV_ERROR, 0, 0, "AssignASNMemChar: BSRead failed.");
	    bsp = BSFree(bsp);
	    pcASN = MemFree(pcASN);
	    return FALSE;
	}
    } else {
	ErrPostEx(SEV_ERROR,0,0, "AssignASNMemChar: bsp pointer returned is NULL");
        bsp = BSFree(bsp);
	return FALSE;
    }

    pABLtmp->buf = pcASN;
    bsp = BSFree(bsp);
    return pABLtmp;
}


/******************************************************************/
/*  Given : a  pointer to the binary info from the database stored*/
/* in a character string but it is bzipeed compressed,            */
/* pointer to the asn read function, the length of the bzipped    */
/* version of the buffer and the length of the non zipped version */
/* of the buffer this method extracts the asn from the            */
/* buffer and returns the asn1 structure.                         */
/******************************************************************/
Pointer GetASNBZMemChar(CharPtr pcASNbz, AsnReadFunc pAsnRead, Int4 iSizebz, Int4 iSize)
{
	AsnIoMemPtr aimp = NULL;
	Pointer pASNdata=NULL;
	CharPtr pcASN1=NULL;

	if (pcASNbz == NULL) {
		ErrPostEx(SEV_ERROR, 0, 0, "GetASNBZMemChar: pf4Field is NULL.");
		return NULL;
	}
	if (pAsnRead == NULL) {
		ErrPostEx(SEV_ERROR, 0, 0, "GetASNBZMemChar: pAsnRead is NULL.");
		return NULL;
	}
	/*get length of the memo field*/
	if((iSize<=0) || (iSizebz <=0)) {
	  ErrPostEx(SEV_ERROR, 0, 0, "GetASNBZMemChar: Length must be a positive number.");
	  return(NULL);
	}

   pcASN1 = (CharPtr) MemNew((size_t) iSize + 1);

   if (BZ2_bzBuffToBuffDecompress(pcASN1, &iSize, pcASNbz,iSizebz,0,0)!=BZ_OK)
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
	pASNdata = (Pointer) (*pAsnRead) (aimp->aip, NULL);
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

	/*return a pointer to the data structure*/
	return(pASNdata);
}

/************************************************************/
/* Create an SQLHandle for an SQLStmt                       */
/************************************************************/
Boolean CreateHandle(SQLHANDLE *hstmt, CharPtr sqlstatement, SQLHDBC *hdbc, SQLHENV *henv)
{
   SQLRETURN retcode;

   retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, hstmt); 
   if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "CreateHandle: Could not allocate statement");
      print_err(*hstmt);
      return SQL_ERROR; 
   }

   retcode = SQLPrepare(*hstmt, (SQLCHAR *) sqlstatement, SQL_NTS);
   if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR,0,0, "CreatHandle: Could not prepare stament %s.",IsNullStr(sqlstatement));
      print_err(*hstmt);
      return SQL_ERROR;
   }

   return SQL_SUCCESS;
}

/***********************************************************/
/* ASN.1 blob storing function which uses bzip compression */
/* Given :  pointer to the ASN.1 data, the asn write func  */
/*  to be used.                                            */
/*  Returns : CharPtr representation of the asn pointer in */
/*  binary compressed version ready to be put into the database*/
/***********************************************************/
pMBL AssignASNMemBZChar(Pointer pASNdata, AsnWriteFunc pAsnWrite)
{
	AsnIoBSPtr aibp=NULL;
	ByteStorePtr bsp=NULL;
	Int4 size=5000;	
	CharPtr buf = NULL;
    CharPtr bzbuf = NULL;
    unsigned int bsLength = 0;
    unsigned int buffsize = 0;
    Int4 totalwritten = 0;
	int rtn = 0;
	pMBL MMDBAsnBuflen = NULL;

	if (pASNdata == NULL) {
		ErrPostEx(SEV_ERROR, 0, 0, "AssignASNMemBZChar: pASNdata is NULL.");
		return NULL;
	}
	if (pAsnWrite == NULL) {
		ErrPostEx(SEV_ERROR, 0, 0, "AssignASNMemBZChar: pAsnWrite is NULL.");
		return NULL;
	}

    MMDBAsnBuflen = MemNew((size_t) sizeof (MBL));

	bsp = BSNew(size);
	BSSeek(bsp, 0L, SEEK_SET);
	aibp = AsnIoBSOpen(GetASNWriteMode(), bsp);
	if (aibp == NULL) {
		ErrPostEx(SEV_ERROR, 0, 0, "AssignASNMemBZChar: AsnIoBSPtr is NULL.");
		return NULL;
	}
	if( ! (*pAsnWrite) (pASNdata, aibp->aip, NULL)) {
		AsnIoBSClose(aibp);
		ErrPostEx(SEV_ERROR, 0, 0, "AssignASNMemBZChar: Asn ByteStore mapped write function failed.");
		BSFree(bsp);
		return NULL;
	}
	aibp = AsnIoBSClose(aibp);

	BSSeek(bsp, 0L, SEEK_SET);
	bsLength = (unsigned long int) BSLen(bsp);
	buf = (CharPtr)MemNew((size_t) bsLength);
	if(buf == NULL) {
		ErrPostEx(SEV_ERROR, 0, 0, "AssignASNMemBZChar: Out of memory.");
		BSFree(bsp);
		return NULL;
	}
	rtn = BSRead(bsp, buf, bsLength);
	buffsize = (unsigned long int) ((FloatLo)bsLength * 1.01 + 50.0);
	bzbuf =  (CharPtr)MemNew(buffsize);
	if(bzbuf == NULL) {
		ErrPostEx(SEV_ERROR, 0, 0, "AssignASNMemBZChar: Out of memory.");
		BSFree(bsp);
        MemFree(buf);
		return NULL;
	}
	if (BZ2_bzBuffToBuffCompress(bzbuf, &buffsize, buf, bsLength, 4, 0, 50)!=BZ_OK) {
		ErrPostEx(SEV_ERROR, 0, 0, "AssignASNMemBZChar: Bzip compression fatal error occurred.");
        BSFree(bsp);
        MemFree(buf);
        MemFree(bzbuf);
		return NULL;
    }
    totalwritten = buffsize;
	MMDBAsnBuflen->buf = bzbuf;
	MMDBAsnBuflen->bslen = bsLength;
	MMDBAsnBuflen->bzlen=totalwritten;

	return MMDBAsnBuflen;

}




/***********************************************************/
/*  Print a db2 error.  Given the statement handle         */
/*    print out the error                                  */
/*    associated with it at the present time.              */
/***********************************************************/
int print_err(SQLHANDLE odbc_hstmt)
{
   SQLCHAR SQL_buffer[1024];
   SQLCHAR SQL_state[6];
   SQLINTEGER SQL_error;
   SQLSMALLINT SQL_Len;
   SQLRETURN retcode;
   
   /*SQLError(SQL_NULL_HENV, SQL_NULL_HDBC, odbc_hstmt, sqlstate, &sqlcode, buffer, SQL_MAX_MESSAGE_LENGTH +1, &length); */
   retcode = SQLGetDiagRec(SQL_HANDLE_DESC, odbc_hstmt, 1, SQL_state, &SQL_error, SQL_buffer, sizeof(SQL_buffer), &SQL_Len);
   if(retcode != SQL_NO_DATA)
   {
      ErrPostEx(SEV_ERROR, 0, 0,"SQLSTATE : %s Native Error Code : %ld", IsNullStr((CharPtr)SQL_state), SQL_error);
      ErrPostEx(SEV_ERROR, 0, 0, "SQLBUFFER: %s", IsNullStr((CharPtr)SQL_buffer));
      ErrPostEx(SEV_ERROR, 0, 0, "----------------------------------------");
      return TRUE;
   }
   return FALSE;
}

void FreeABL(pABL pABLtmp)
{
  MemFree(pABLtmp);
}
void FreeMBL(pMBL pMBLtmp)
{
  MemFree(pMBLtmp);
}


/* Boolean execute_sql(SQLCHAR * sqlstatement, SQLHANDLE * odbc_hdbc, SQLHANDLE * odbc_henv) */
Boolean execute_sql(SQLCHAR * sqlstatement, SQLHANDLE * hdbc, SQLHANDLE * henv)
{
   SQLHANDLE hstmt;

   if (SQLAllocStmt(*hdbc, &hstmt) != SQL_SUCCESS)
   {
      print_err(hstmt);
      return FALSE;
   }

   /* set auto commit on */
   /*   SQLSetConnectAttr( hdbc, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_ON, SQL_NTS);*/
  
   if (SQLPrepare( hstmt, sqlstatement, SQL_NTS) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR,0,0, "execute_sql: could not prepare the stament %s ",IsNullStr((CharPtr)sqlstatement));
      SQLFreeStmt(hstmt, SQL_DROP);
      return FALSE;
   }
  
  /* execute statment */
  if (SQLExecute (hstmt) != SQL_SUCCESS)
  {
      ErrPostEx(SEV_ERROR,0,0, "execute_sql: unable to execute statement. %s ", IsNullStr((CharPtr)sqlstatement));
      print_err(hstmt);
      SQLFreeStmt(hstmt, SQL_DROP);
      return FALSE;
  }

  if(SQLFreeStmt(hstmt, SQL_DROP) != SQL_SUCCESS)
  {
      print_err(hstmt);
      return FALSE;
  }

  return TRUE;
}


/****************************************************************/
/* Given an SQL handle, write the SQL information to log        */
/****************************************************************/
void LogSQLInfo(SQLHANDLE hstmt)
{
  SQLRETURN retcode;
  SQLCHAR sqlState[6];
  SQLINTEGER nativeErrPtr;
  SQLCHAR errMsg[SQL_MAX_MESSAGE_LENGTH];
  SQLSMALLINT msgLen;

  retcode = SQLGetDiagRec(SQL_HANDLE_STMT, hstmt, 1, sqlState, 
			  &nativeErrPtr, errMsg, sizeof(errMsg), 
			  &msgLen);
   if(retcode != SQL_NO_DATA) 
   { 
	 ErrPostEx(SEV_ERROR, 0, 0, "SQLSTATE:%s. Native Err:%d. Err Msg:%s.", 
		   IsNullStr((CharPtr)sqlState),                      /* unixODBC Error #  */
		   nativeErrPtr,                  /* Data Base Error # */
		   IsNullStr((CharPtr)errMsg) );                      /* Error Message     */ 
   } 
   else
   {
       ErrPostEx(SEV_ERROR, 0, 0, "[LogSQLInfo]: Could not display errMsg");
   }
}


/****************************************************************/
/* Given an SQL handle, write the connection info to log        */
/****************************************************************/
void LogSQLConnectInfo(SQLHDBC hdbc)
{
  SQLRETURN retcode;
  SQLCHAR sqlState[6];
  SQLINTEGER nativeErrPtr;
  SQLCHAR errMsg[SQL_MAX_MESSAGE_LENGTH];
  SQLSMALLINT msgLen;

  retcode = SQLGetDiagRec(SQL_HANDLE_DBC, hdbc, 1, sqlState, 
			  &nativeErrPtr, errMsg, sizeof(errMsg), 
			  &msgLen);
   if(retcode != SQL_NO_DATA) 
   { 
	 ErrPostEx(SEV_ERROR, 0, 0, "SQLSTATE:%s. Native Err:%d. Err Msg:%s.", 
		   IsNullStr((CharPtr)sqlState),                      /* unixODBC Error #  */
		   nativeErrPtr,                  /* Data Base Error # */
		   IsNullStr((CharPtr)errMsg) );                      /* Error Message     */ 
   } 
   else
   {
       ErrPostEx(SEV_ERROR, 0, 0, "[LogSQLConnectInfo]: Could not display errMsg");
   }
}


/************************************************************/
/* Given a SQLHandle, set AutoCommit off                    */
/************************************************************/
Boolean SetAutoCommitOff(SQLHDBC *hdbc)
{
   SQLRETURN retcode;

   retcode = SQLSetConnectAttr(hdbc, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_OFF, SQL_NTS);; 
   if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[SetAutoCommitOff]:unable to set SQL_AUTOCOMMIT.");
      LogSQLConnectInfo(hdbc);
      return FALSE; 
   }

   return TRUE;
}

/************************************************************/
/* Given a SQLHandle, commit                                */
/************************************************************/
Boolean CommitChanges(SQLHDBC *hdbc)
{
   SQLRETURN retcode;

   retcode = SQLEndTran(SQL_HANDLE_DBC, hdbc, SQL_COMMIT); 
   if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[CommitChanges]:unable to commit changes.");
      LogSQLConnectInfo(hdbc);
      return FALSE; 
   }

   return TRUE;
}

/************************************************************/
/* Given a SQLHandle, rollback                              */
/************************************************************/
Boolean RollBackChanges(SQLHDBC *hdbc)
{
   SQLRETURN retcode;

   retcode = SQLEndTran(SQL_HANDLE_DBC, hdbc, SQL_ROLLBACK); 
   if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[RollBackChanges]: unable to roll back.");
      LogSQLConnectInfo(hdbc);
      return FALSE; 
   }

   return TRUE;
}

/* $Log: slri_odbc.c,v $
/* Revision 1.7  2004/10/01 19:19:02  zwang
/* Added transaction-related functions.
/*
/* Revision 1.6.2.1  2004/10/01 16:18:23  zwang
/* Added transaction-related functions.
/*
/* Revision 1.6  2004/03/23 22:24:38  rcavero
/* Implement function LogSQLConnectInfo used by intrez_odbc.c ConnectToDb()
/*
/* Revision 1.5  2004/01/12 20:58:59  zwang
/* Fixed bug in calling IsNullStr.
/*
/* Revision 1.4  2004/01/12 19:02:33  zwang
/* Added generic functions for ODBC layer.
/* */
