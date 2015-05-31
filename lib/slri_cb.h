/* $Id: slri_cb.h,v 1.1.1.4 2002/03/02 09:25:26 root Exp $
-- ***************************************************************************
--
--  Samuel Lunenfeld Research Institute (SLRI)
--
--  File: slri_cb.h
--  Description: Generic CodeBase database functions
--
--  Author: Katerina Michalickova, Chris Hogue, Gary Bader
--  Date: October 1998
--
--  Hogue Lab - University of Toronto Biochemistry Department
--  Samuel Lunenfeld Research Institute, Mount Sinai Hospital
--
--  Copyright Notice:
--
--  Copyright ©2000 Mount Sinai Hospital (MSH)
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

#ifndef _SLRI_CB_
#define _SLRI_CB_

#include <d4all.h>
#include <asn.h>

#ifdef __cplusplus
extern "C" {
#endif

/*********************/
/* Function Pointers */
/*********************/

/*function pointer type for CodeBase database field initialization functions*/
typedef void (LIBCALL *InitFieldFunc) PROTO((void));

/************************/
/* Function definitions */
/************************/

/*********************/
/* Utility functions */
/*********************/

/*allocates and creates a full database name given a path and a database name in a platform
independent manner*/
extern CharPtr AllocateAndCreateDBPath(CharPtr dbPath, CharPtr dbName);

/*generically open a CodeBase database*/
extern Boolean OpenDB(CharPtr pathname, DATA4 **pd4db, FIELD4INFO *f4i,
					TAG4INFO *t4i, InitFieldFunc initFieldFunc, CODE4 *codebase);

/*generically open a CodeBase database, directs if database is to be opened or created*/
extern Boolean OpenDBII(CharPtr pathname, DATA4 **pd4db, FIELD4INFO *f4i,
					TAG4INFO *t4i, InitFieldFunc initFieldFunc, CODE4 *codebase, Boolean CREATE);

/*deletes the database files - useful for a refresh function*/
extern Boolean DeleteDB(DATA4 *d4datap, CharPtr dbPath);

/********************/
/*ASN.1 <-> CodeBase*/
/********************/

/* Stores binary blobs into CodeBase Memo field */
extern Boolean AssignASNMemMemo(Pointer pASNdata, AsnWriteFunc pAsnWrite, FIELD4 *pf4Field);

/* Generically gets the ASN.1 from the CodeBase ASN.1 field given */
extern Pointer GetASNMemMemo(FIELD4 *pf4Field, AsnReadFunc pAsnRead);

/***********************************************/
/*Comma separated lists <-> CodeBase Memo Field*/
/***********************************************/

/*Adds a string to a string list in a memo field - optionally keep the list unique*/
extern Boolean AddString2ListMemo(CharPtr string, FIELD4 *pf4MemoField, Boolean unique);

/*Gets a string from a string list in a memo field*/
extern CharPtr GetStringFromListMemo(FIELD4 *pf4MemoField);

/*Adds an integer to an integer list in a memo field - optionally keep the list sorted or sorted&unique*/
extern Boolean AddInt2ListMemo(Int4 integer, FIELD4 *pf4MemoField, Boolean sorted, Boolean unique);

#ifdef __cplusplus
}
#endif
#endif		/* _SLRI_CB_ */

/*
$Log: slri_cb.h,v $
Revision 1.1.1.4  2002/03/02 09:25:26  root
Automated import from Sourceforge.net

Revision 1.4  2002/03/01 23:58:14  gbader
Moved generic asn.1 read/write mode function to slri_asn

Revision 1.3  2001/11/29 04:26:33  gbader
Header rearrangement

Revision 1.2  2001/11/21 23:55:23  kaca
added OpenDBII accepts Boolean flag for creating a database

Revision 1.1.1.3  2001/10/10 22:11:04  kaca
initial import

Revision 1.10  2001/05/25 19:04:45  gbader
User can now select database ASN.1 read/write mode.

Revision 1.9  2001/04/05 22:57:03  gbader
Added GetStringFromListMemo and fixed ListMemo to encode important characters

Revision 1.8  2001/03/28 21:45:51  gbader
Moved externs to proper place before function return type

Revision 1.7  2001/03/28 01:19:46  gbader
Added option of keeping memo lists unique

Revision 1.6  2001/03/15 21:09:46  gbader
Moved extern "C" statements to proper place in header files (after includes)

Revision 1.5  2001/03/08 00:09:55  gbader
Moved extraneous header files inside the C files to make reduce header dependencies and make compiling much more efficient.

Revision 1.4  2001/02/07 21:24:49  gbader
Moved remaining CodeBase functions to slri_cb.[ch].  No other lib file should ever include d4all.h

Revision 1.3  2001/01/13 04:04:10  gbader
Added IBM DB2 UDB to license exceptions.

Revision 1.2  2000/09/05 13:55:08  gbader
Updated License so that it is now GNU.
Added CVS keywords ID and Log as well as new line at end of files so compilers on some platforms don't complain

*/

