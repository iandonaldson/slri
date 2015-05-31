/* $Id: slri_db2.h,v 1.2 2005/04/26 14:34:31 rcavero Exp $
-- ***************************************************************************
--
--  File: slri_db2.h
--  Description: Generic Db2 database functions
--
--  Author: Ruth Isserlin, Katerina Michalickova, Chris Hogue, Gary Bader
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


#ifndef _SLRI_DB2_
#define _SLRI_DB2_

#include <sqlcli1.h>
#include <asn.h>
    
#ifdef __cplusplus
extern "C" {
#endif

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

/***********************/
/* function definitions*/
/***********************/
          
/******************************************************************/
/*  Given : a  pointer to the binary info from the database stored*/
/* in a character string, pointer to the asn read function and the*/
/* length of the buffer this method extracts the asn from the     */
/* buffer and returns the asn1 structure.                         */
/******************************************************************/
extern Pointer GetASNMemChar(CharPtr pcASN1, AsnReadFunc pAsnRead, Int4 length);
        
/***********************************************************/
/* ASN.1 blob storing function which uses bzip compression */
/* Given :  pointer to the ASN.1 data, the asn write func  */
/*  to be used.                                            */
/*  Returns : CharPtr representation of the asn pointer in */
/*  binary compressed version ready to be put into the database*/
/***********************************************************/
extern pMBL AssignASNMemBZChar(Pointer pASNdata, AsnWriteFunc pAsnWrite);
       
/******************************************************************/
/*  Given : a  pointer to the binary info from the database stored*/
/* in a character string but it is bzipeed compressed,            */
/* pointer to the asn read function, the length of the bzipped    */
/* version of the buffer and the length of the non zipped version */
/* of the buffer this method extracts the asn from the            */
/* buffer and returns the asn1 structure.                         */
/******************************************************************/
extern Pointer GetASNBZMemChar(CharPtr pcASNbz, AsnReadFunc pAsnRead, Int4 iSizebz, Int4 iSize);

/***********************************************************************/
/* Stores binary blobs into a byte store so that it can be loaded into db2*/
/*  Given : the pointer to the asn1 data and a pointer to the function */
/* that writes this asn1 pointer to memory.                            */
/*  Returns :  a structure that contains the asn buffer and the        */
/* length of the byteStorePtr that stored the asn1 structure.     */
/*  IT IS THE CALLER OF THIS FUNCTION THAT IS RESPONSIBLE FOR FREEING  */
/*  THE STRUCTURE THAT IS RETURNED.                                    */
/***********************************************************************/
extern pABL AssignASNMemChar(Pointer pASNdata, AsnWriteFunc pAsnWrite);

/***********************************************/
/*  Given a Sql statement handle and the sql   */
/*  statements you want to associate with that */
/*  handle this method allocates the statement */
/*  handle and the statement as well.          */
/*  Returns : True is successful and False     */
/*   otherwise.                                */
/***********************************************/
extern Boolean CreateHandle(SQLHANDLE * hstmt,CharPtr sqlstate, SQLHANDLE * hdbc, SQLHANDLE * henv);

/***********************************************************/
/*  Print a db2 error.  Given the statement handle         */
/*    print out the error                                  */
/*    associated with it at the present time.              */
/***********************************************************/
extern Int4 print_err(SQLHSTMT hstmt);

/***********************************************************/
/*  Given : a string representing a sql statement that has */
/*  to be run on the database.  Execute statement.         */
/*  Statement can not contain any varaibles so this function*/
/*  is only good for static sql.                           */
/*  Return : True if the statement is successful and False */
/*  if it is not successful.                               */
/*  (At present used only for the create statements        */
/***********************************************************/
extern Boolean execute_sql(SQLCHAR * sqlstatement,SQLHANDLE * hdbc, SQLHANDLE * henv);

/*free asn buf length structure*/
extern void FreeABL(pABL pABLtmp);

/*free mmdb asn buf length structure */
extern void FreeMBL(pMBL pABLtmp);

#ifdef __cplusplus
}
#endif

#endif /* _SLRI_DB2_ */

/* 
$Log: slri_db2.h,v $
Revision 1.2  2005/04/26 14:34:31  rcavero
Change Copyright Note.

Revision 1.1.1.2  2002/03/02 09:25:26  root
Automated import from Sourceforge.net

Revision 1.6  2002/03/01 23:58:14  gbader
Moved generic asn.1 read/write mode function to slri_asn

Revision 1.5  2002/01/03 20:24:32  risserlin
fixed cvs id and log tag X3

*/

