/* $Id: slri_db2.c,v 1.3 2005/04/26 14:33:12 rcavero Exp $
-- ***************************************************************************
--
--  File: slri_db2.c
--  Description: Generic CodeBase database functions
--
--  Author: Ruth Isserlin, Katerina Michalickova, Chris Hogue, Gary Bader
--  Date: January 2 2002
--
--  Copyright Notice:
--
--  Copyright ©2005 Mount Sinai Hospital (MSH)
--
--  This program is free software; you can redistribute it and/or
--  modify it under the terms of the GNU General Public License as
--  published by the Free Software Foundation; either version 2 of
--  the License, or any later version.
--
--  This program is distributed in the hope that it will be useful,
--  but WITHOUT ANY WARRANTY; without even the implied warranty of
--  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
--
--  See the GNU General Public License for more details.
--
--  You should have received a copy of the GNU General Public License
--  along with this program; if not, write to the
--         Free Software Foundation, Inc.,
--         59 Temple Place, Suite 330, Boston, MA
--         02111-1307  USA
--  or visit http://www.gnu.org/copyleft/gpl.html
--
--  SPECIAL EXCEPTIONS
--  
--  As a special exception, Mount Sinai Hospital gives permission to
--  link this program with the following non-GPL programs or libraries,
--  and distribute the resulting executable, without including the source
--  code for these in the source distribution:
--  
--  a) CodeBase 6.5 or greater from Sequiter Software Inc.
--  
--  b) The C or C++ interface to Oracle 8.x or greater, from
--     Oracle Corporation or IBM DB2 UDB.
--
-- ***************************************************************************
*/


/*****************************************************************/
/*      slri_db2 file that stores generic fucntions that can be  */
/*  used with any db2 applications.                              */
/*****************************************************************/

#include <slri_db2.h>
#include <slri_misc.h>
#include <slri_asn.h>
#include <bzlib.h>

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
		ErrPostEx(SEV_ERROR, 0, 0, "GetASNMemChar: Out of memory.");
		BSFree(bsp);
		return NULL;
	}
	rtn = BSRead(bsp, buf, bsLength);
	buffsize = (unsigned long int) ((FloatLo)bsLength * 1.01 + 50.0);
	bzbuf =  (CharPtr)MemNew(buffsize);
	if(bzbuf == NULL) {
		ErrPostEx(SEV_ERROR, 0, 0, "GetASNMemChar: Out of memory.");
		BSFree(bsp);
        MemFree(buf);
		return NULL;
	}
	if (BZ2_bzBuffToBuffCompress(bzbuf, &buffsize, buf, bsLength, 4, 0, 50)!=BZ_OK) {
		ErrPostEx(SEV_ERROR, 0, 0, "GetASNMemMemo: Bzip compression fatal error occurred.");
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
	    ErrPostEx(SEV_ERROR,0,0, "GetBioseqfromASNDB: Bioseq pointer is NULL.");
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

	if (pcASN1 == NULL) {
		ErrPostEx(SEV_ERROR, 0, 0, "GetASNMemMemo: pf4Field is NULL.");
		return NULL;
	}
	if (pAsnRead == NULL) {
		ErrPostEx(SEV_ERROR, 0, 0, "GetASNMemMemo: pAsnRead is NULL.");
		return NULL;
	}

	/*get length of the memo field*/
	if(length<=0) {
	  ErrPostEx(SEV_ERROR, 0, 0, "GetASNMemMemo: Length must be a positive number.");
	  return(NULL);
	}

	/*open the asn io stream*/
	aimp = AsnIoMemOpen(GetASNReadMode(), (BytePtr) pcASN1, length);
	if (aimp == NULL) {
		ErrPostEx(SEV_ERROR, 0, 0, "GetASNMemMemo: AsnIoMemPtr is NULL.");
		return NULL;
	}

	/*read the asn.1 into a data structure*/
	pASNdata = (Pointer) (*pAsnRead) (aimp->aip, NULL);
	if (pASNdata == NULL)
	  {
	    ErrPostEx(SEV_ERROR,0,0, "GetBioseqfromASNDB: Bioseq pointer is NULL.");
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

/***********************************************************************/
/* Stores binary blobs into a byte store so that it can be loaded into db2*/
/*  Given : the pointer to the asn1 data and a pointer to the function */
/* that writes this asn1 pointer to memory.                            */
/*  Returns :  a structure that contains the asn buffer and the        */
/* length of the byteStorePtr that stored the asn1 structure.     */
/*  IT IS THE CALLER OF THIS FUNCTION THAT IS RESPONSIBLE FOR FREEING  */
/*  THE STRUCTURE THAT IS RETURNED.                                    */
/***********************************************************************/
pABL AssignASNMemChar(Pointer pASNdata, AsnWriteFunc pAsnWrite){

	AsnIoBSPtr aibp=NULL;
	ByteStorePtr bsp=NULL;
	Int4 size=5000;	/*start size of ByteStore*/
	CharPtr pcASN=NULL;
	Int4 bsLength=0;
	pABL pABLtmp=NULL;

	if (pASNdata == NULL) {
		ErrPostEx(SEV_ERROR, 0, 0, "AssignASNMemChar: pASNdata is NULL.");
		return NULL; 
	}
   
	if (pAsnWrite == NULL) { 
		ErrPostEx(SEV_ERROR, 0, 0, "AssignASNMemChar: pAsnWrite is NULL.");
		return NULL;
	}     


	pABLtmp = MemNew((size_t) sizeof (ABL));

	bsp = BSNew(size);
	BSSeek(bsp, 0L, SEEK_SET);
	aibp = AsnIoBSOpen(GetASNWriteMode(), bsp);
	if (aibp == NULL) {
		ErrPostEx(SEV_ERROR, 0, 0, "AssignASNMemChar: AsnIoBSPtr is NULL.");
		return NULL;
	}
	if( ! (*pAsnWrite) (pASNdata, aibp->aip, NULL)) {
		AsnIoBSClose(aibp);
		ErrPostEx(SEV_ERROR, 0, 0, "AssignASNMemChar: Asn ByteStore mapped write function failed.");
		bsp = BSFree(bsp);
		return NULL;
	}
	aibp = AsnIoBSClose(aibp);
	
    if ( bsp != NULL){ 
	  BSSeek(bsp, 0L, SEEK_SET);
	  bsLength = BSLen(bsp);
	  pcASN = MemNew((size_t) bsLength);
	  pABLtmp->len = bsLength;

	  if(pcASN == NULL) {
		ErrPostEx(SEV_ERROR, 0, 0, "AssignASNMemChar: Out of memory.");
		bsp = BSFree(bsp);
		return FALSE;
	  }
	  if(BSRead(bsp, pcASN, bsLength) != bsLength) {
		ErrPostEx(SEV_ERROR, 0, 0, "AssignASNMemChar: BSRead failed.");
		bsp = BSFree(bsp);
		pcASN = MemFree(pcASN);
		return FALSE;
	  }
    }
    else{
	  ErrPostEx(SEV_ERROR,0,0, "AssignASNMemChar: bsp pointer returned is NULL");
      bsp = BSFree(bsp);
	  return FALSE;
	}
	pABLtmp->buf = pcASN;
    bsp = BSFree(bsp);
	return pABLtmp;
}

/***********************************************/
/*  Given a Sql statement handle and the sql   */
/*  statements you want to associate with that */
/*  handle this method allocates the statement */
/*  handle and the statement as well.          */
/*  Returns : True is successful and False     */
/*   otherwise.                                */
/***********************************************/
Boolean CreateHandle(SQLHANDLE * hstmt,CharPtr sqlstate, SQLHANDLE * hdbc,SQLHANDLE * henv){

  if (SQLAllocStmt(*hdbc, hstmt) != SQL_SUCCESS){
    ErrPostEx(SEV_ERROR,0,0, "CreateHandle: counld not allocate statement");
    print_err(*hstmt);
  }

  if (SQLPrepare( *hstmt, (SQLCHAR *)sqlstate, SQL_NTS) != SQL_SUCCESS){
	ErrPostEx(SEV_ERROR,0,0, "creatHandle: could not prepare the stament for %s. ",IsNullStr(sqlstate));
        print_err(*hstmt);
   }

   return TRUE;
}

/***********************************************************/
/*  Print a db2 error.  Given the statement handle         */
/*    print out the error                                  */
/*    associated with it at the present time.              */
/***********************************************************/
Int4 print_err(SQLHSTMT hstmt){

   SQLCHAR buffer[1024];
   SQLCHAR sqlstate[1024];
   SQLINTEGER sqlcode;
   SQLSMALLINT length;
   
   SQLError(SQL_NULL_HENV, SQL_NULL_HDBC, hstmt, sqlstate, &sqlcode, buffer, SQL_MAX_MESSAGE_LENGTH +1, &length);
   ErrPostEx(SEV_ERROR, 0, 0,"SQLSTATE : %s Native Error Code : %ld", IsNullStr(sqlstate), sqlcode);
   ErrPostEx(SEV_ERROR, 0, 0, "%s", IsNullStr(buffer));
   ErrPostEx(SEV_ERROR, 0, 0, "----------------------------------------");
   return(SQL_ERROR);

}

/***********************************************************/
/*  Given : a string representing a sql statement that has */
/*  to be run on the database.  Execute statement.         */
/*  Statement can not contain any varaibles so this function*/
/*  is only good for static sql.                           */
/*  Return : True if the statement is successful and False */
/*  if it is not successful.                               */
/*  (At present used only for the create statements        */
/***********************************************************/
Boolean execute_sql(SQLCHAR * sqlstatement,SQLHANDLE * hdbc,SQLHANDLE * henv)
{
   SQLHANDLE hstmt;

   if (SQLAllocStmt(*hdbc, &hstmt) != SQL_SUCCESS){
	print_err(hstmt);
	return FALSE;
   }

   /* set auto commit on */
   /*   SQLSetConnectAttr( hdbc, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_ON, SQL_NTS);*/

  if (SQLPrepare( hstmt, sqlstatement, SQL_NTS) != SQL_SUCCESS){
	ErrPostEx(SEV_ERROR,0,0, "execute_sql: could not prepare the stament %s ",IsNullStr(sqlstatement));
	SQLFreeStmt(hstmt, SQL_DROP);
	return FALSE;
   }

  /* execute statment */
  if (SQLExecute (hstmt) != SQL_SUCCESS){
	ErrPostEx(SEV_ERROR,0,0, "execute_sql: unable to execute statement. %s ", IsNullStr(sqlstatement));
	print_err(hstmt);
	SQLFreeStmt(hstmt, SQL_DROP);
	return FALSE;
  }

  if(SQLFreeStmt(hstmt, SQL_DROP) != SQL_SUCCESS){
     print_err(hstmt);
     return FALSE;
   }

   return TRUE;
}

void FreeABL(pABL pABLtmp){

	MemFree(pABLtmp);

}


void FreeMBL(pMBL pABLtmp){

	MemFree(pABLtmp);

}

/* 
$Log: slri_db2.c,v $
Revision 1.3  2005/04/26 14:33:12  rcavero
Change Copyright Note.

Revision 1.2  2003/10/16 20:59:50  skuznets

1. Checking for NULL pointers when print values
2. Added function Misc_TrimSpacesAroundString
3. Added macro IsNullStr

Revision 1.1.1.4  2002/07/27 08:28:44  root
Automated import from Sourceforge.net

Revision 1.9  2002/07/26 15:57:37  hfeldman
removed printf

Revision 1.8  2002/03/03 23:10:32  gbader
Fixed MSVC compiler warnings.

Revision 1.7  2002/03/01 23:57:45  gbader
Fixed compiler warnings
Moved generic asn set read/write mode to slri_asn

Revision 1.6  2002/01/03 20:50:47  risserlin
got rid of commented out code

Revision 1.5  2002/01/03 20:25:00  risserlin
fixed cvs id and log tag X3

*/

