/* $Id: slri_asn.h,v 1.1.1.3 2002/03/02 09:25:26 root Exp $
-- ***************************************************************************
--
--  Samuel Lunenfeld Research Institute (SLRI)
--
--  File: slri_asn.h
--  Description: General ASN.1 functions
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

#ifndef _SLRI_ASN_
#define _SLRI_ASN_

#include <asn.h>

#ifdef __cplusplus
extern "C" {
#endif

/************************/
/* Function definitions */
/************************/

/*************************/
/* ASN.1 read/write mode */
/*************************/

/*For setting read/write mode in modules that support this feature*/

/*Set ASN.1 read mode e.g. "rb" or "r"*/
extern Boolean SetASNReadMode(CharPtr readMode);

/*Set ASN.1 write mode e.g. "wb", "w" or "wx"*/
extern Boolean SetASNWriteMode(CharPtr writeMode);

/*Get ASN.1 read mode*/
extern CharPtr GetASNReadMode(void);

/*Get ASN.1 write mode*/
extern CharPtr GetASNWriteMode(void);

/*********/
/* ASN.1 */
/*********/

/*Given a file containing an ASN.1 object, return a pointer to the data.  Supports ASCII (value) or binary ASN.1*/
extern Pointer ASN_GetFromFileName(CharPtr fileName, AsnReadFunc pAsnRead);

/*Given a ByteStore containing an ASN.1 object, return a pointer to the data.  Supports ASCII (value) or binary ASN.1*/
extern Pointer ASN_GetFromBS(ByteStorePtr bsp, AsnReadFunc pAsnRead);

/***************************************************/
/* Generic functions for ASN.1 'SEQUENCE OF' lists */
/***************************************************/

/*copy an ASN.1 generated code SEQUENCE OF list in memory*/
extern Pointer SLRI_AsnIoMemCopySeqOf(Pointer ptr, Int4Ptr count, AsnReadFunc readfunc, AsnWriteFunc writefunc, AsnOptFreeFunc freefunc);

/*generically uniques a sequence of ASN.1 objects*/
extern void SLRI_AsnIoMemUniqueSeqOf(Pointer *list, AsnWriteFunc writefunc, AsnOptFreeFunc freefunc);

#ifdef __cplusplus
}
#endif
#endif		/* _SLRI_ASN_ */

/*
$Log: slri_asn.h,v $
Revision 1.1.1.3  2002/03/02 09:25:26  root
Automated import from Sourceforge.net

Revision 1.3  2002/03/01 23:58:14  gbader
Moved generic asn.1 read/write mode function to slri_asn

Revision 1.2  2002/02/19 22:45:36  gbader
Added ASN.1 get from file name function.

Revision 1.1.1.2  2001/05/24 15:08:46  micheld
Update of slrilib

Revision 1.9  2001/03/28 21:45:51  gbader
Moved externs to proper place before function return type

Revision 1.8  2001/03/26 21:58:07  gbader
Added function to unique a SEQUENCE OF asn.1 object

Revision 1.7  2001/03/15 21:09:46  gbader
Moved extern "C" statements to proper place in header files (after includes)

Revision 1.6  2001/03/08 00:09:55  gbader
Moved extraneous header files inside the C files to make reduce header dependencies and make compiling much more efficient.

Revision 1.5  2001/02/07 21:24:48  gbader
Moved remaining CodeBase functions to slri_cb.[ch].  No other lib file should ever include d4all.h

Revision 1.4  2001/01/13 04:04:10  gbader
Added IBM DB2 UDB to license exceptions.

Revision 1.3  2000/09/05 13:55:08  gbader
Updated License so that it is now GNU.
Added CVS keywords ID and Log as well as new line at end of files so compilers on some platforms don't complain

*/

