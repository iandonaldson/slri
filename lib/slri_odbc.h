/* $Id: slri_odbc.h,v 1.5 2004/10/01 19:19:02 zwang Exp $
*******************************************************************************
  Copyright (C) 2000 Mount Sinai Hospital (MSH)

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
           taken from NCBI's cdentrez.c


  REVISION/CONTRIBUTION NOTES:

  June 1, 2000  Original MSH Revision.
                Hogue Lab - University of Toronto Biochemistry
                Department and the Samuel Lunenfeld Research
                Institute, Mount Sinai Hospital
                http://bioinfo.mshri.on.ca  hogue@mshri.on.ca
                ftp://ftp.mshri.on.ca/


  PUBLICATION TO CITE:


  DESCRIPTION



*******************************************************************************/
#ifndef _SLRI_ODBC_
#define _SLRI_ODBC_
#include <sql.h>
#include <sqlext.h>
#include <slri_asn.h>
#include <slri_misc.h>



#ifdef __cplusplus
extern "C" { /* } */
#endif

#define SQLINTEGER long
#define MAXCHARLEN 1024

SQLHENV henv; /* ODBC environment handle */
SQLHDBC hdbc; /* ODBC connection handle */

/* defined type to store both the asn binary blob and the len
of the blob just written to the buffer. */
typedef struct ASNBufLen {
    CharPtr buf;
    Int4 len;
} ABL, PNTR pABL;


/* defined type to store both the bzipped asn binary blob and the len
of the blob originally and length of the blob zipped.  Used with large
data such as MMDB blobs.*/
typedef struct MMDBBufLen {
	CharPtr buf;
	Int4 bslen;
	Int4 bzlen;
} MBL, PNTR pMBL;

/******************************************************************/
/*  Given : a  pointer to the binary info from the database stored*/
/* in a character string, pointer to the asn read function and the*/
/* length of the buffer this method extracts the asn from the     */
/* buffer and returns the asn1 structure.                         */
/******************************************************************/
Pointer GetASNMemChar(CharPtr pcASN1, AsnReadFunc pAsnRead, Int4 length);

/******************************************************************/
/*  Given : a  pointer to the binary info from the database stored*/
/* in a character string but it is bzipeed compressed,            */
/* pointer to the asn read function, the length of the bzipped    */
/* version of the buffer and the length of the non zipped version */
/* of the buffer this method extracts the asn from the            */
/* buffer and returns the asn1 structure.                         */
/******************************************************************/
Pointer GetASNBZMemChar(CharPtr pcASNbz, AsnReadFunc pAsnRead, Int4 iSizebz, Int4 iSize);

/***********************************************************************/
/* Stores binary blobs into a byte store so that it can be loaded into db2*/
/*  Given : the pointer to the asn1 data and a pointer to the function */
/* that writes this asn1 pointer to memory.                            */
/*  Returns :  a structure that contains the asn buffer and the        */
/* length of the byteStorePtr that stored the asn1 structure.     */
/*  IT IS THE CALLER OF THIS FUNCTION THAT IS RESPONSIBLE FOR FREEING  */
/*  THE STRUCTURE THAT IS RETURNED.                                    */
/***********************************************************************/
pABL AssignASNMemChar(Pointer pASNdata, AsnWriteFunc pAsnWrite);

/***********************************************************/
/* ASN.1 blob storing function which uses bzip compression */
/* Given :  pointer to the ASN.1 data, the asn write func  */
/*  to be used.                                            */
/*  Returns : CharPtr representation of the asn pointer in */
/*  binary compressed version ready to be put into the database*/
/***********************************************************/
pMBL AssignASNMemBZChar(Pointer pASNdata, AsnWriteFunc pAsnWrite);


void FreeABL(pABL ABLtmp);
void FreeMBL(pMBL MBLtmp);


Boolean CreateHandle(SQLHANDLE *hstmt, CharPtr sqlstatement, SQLHDBC *hdbc, SQLHENV *henv);
int print_err(SQLHANDLE odbc_hstmt);

Boolean execute_sql(SQLCHAR * sqlstatement, SQLHANDLE * hdbc, SQLHANDLE * henv);

/****************************************************************/
/* Given an SQL handle, write the SQL information to log        */
/****************************************************************/
void LogSQLInfo(SQLHANDLE hstmt);
void LogSQLConnectInfo(SQLHDBC hdbc);


/****************************************************************/
/* transaction related functions                                */
/****************************************************************/
Boolean SetAutoCommitOff(SQLHDBC * hdbc);
Boolean CommitChanges(SQLHDBC * hdbc);
Boolean RollBackChanges(SQLHDBC * hdbc);



#ifdef __cplusplus
/* { */ }
#endif

#endif    /* _SLRI_ODBC_  */

/* $Log: slri_odbc.h,v $
/* Revision 1.5  2004/10/01 19:19:02  zwang
/* Added transaction-related functions.
/*
/* Revision 1.4.2.1  2004/10/01 16:18:23  zwang
/* Added transaction-related functions.
/*
/* Revision 1.4  2004/03/23 22:24:52  rcavero
/* Implement function LogSQLConnectInfo used by intrez_odbc.c ConnectToDb()
/*
/* Revision 1.3  2004/01/12 19:02:33  zwang
/* Added generic functions for ODBC layer.
/* */  
