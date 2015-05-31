/* $Id: objflinkset.h,v 1.1.1.1 2001/10/12 08:21:54 root Exp $
-- ***************************************************************************
--
--  Samuel Lunenfeld Research Institute (SLRI)
--
--  File: objflinkset.h
--  Description:  Modified LinkSet from NCBI's LinkSet that uses FloatHi weights
--                and can be used in a nested list
--                **** Do not use ASNTOOL ****
--                Hard-coded LinkSet component
--
--  Author: Michel Dumontier
--  Date: April 2001
--
--  Hogue Lab - University of Toronto Biochemistry Department
--  Samuel Lunenfeld Research Institute, Mount Sinai Hospital
--
--  Copyright Notice:
--
--  Copyright ©2001 Mount Sinai Hospital (MSH)
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

#ifndef _objflinkset_ 
#define _objflinkset_ 

#undef NLM_EXTERN
#ifdef NLM_IMPORT
#define NLM_EXTERN NLM_IMPORT
#else
#define NLM_EXTERN extern
#endif


#ifdef __cplusplus
extern "C" { /* } */
#endif


/**************************************************
*
*    Generated objects for Module SLRI-FLink-set
*    Generated using ASNCODE Revision: 6.9 at Apr 19, 2001  7:41 PM
*
**************************************************/

NLM_EXTERN Boolean LIBCALL
objflinksetAsnLoad PROTO((void));


/**************************************************
*
*    FLinkSet
*
**************************************************/
typedef struct struct_FLink_set {
   struct struct_FLink_set PNTR next;
   Int4   num;
   Int4Ptr   uids;
   FloatHiPtr   weights;
} FLinkSet, PNTR FLinkSetPtr;

NLM_EXTERN FLinkSetPtr LIBCALL FLinkSetNew2 PROTO ((Int4 num));
NLM_EXTERN FLinkSetPtr LIBCALL FLinkSetNew PROTO (( void ));

NLM_EXTERN FLinkSetPtr LIBCALL FLinkSetFree PROTO ((FLinkSetPtr ));
NLM_EXTERN FLinkSetPtr LIBCALL FLinkSetAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL FLinkSetAsnWrite PROTO (( FLinkSetPtr , AsnIoPtr, AsnTypePtr));



/**************************************************
*
*    FLinkSetList
*
**************************************************/
typedef struct struct_FLink_set_List {
   struct struct_FLink_set_List PNTR next;
   struct struct_FLink_set PNTR   set1;
   Int4   id;
} FLinkSetList, PNTR FLinkSetListPtr;


NLM_EXTERN FLinkSetListPtr LIBCALL FLinkSetListFree PROTO ((FLinkSetListPtr ));
NLM_EXTERN FLinkSetListPtr LIBCALL FLinkSetListNew PROTO (( void ));
NLM_EXTERN FLinkSetListPtr LIBCALL FLinkSetListAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL FLinkSetListAsnWrite PROTO (( FLinkSetListPtr , AsnIoPtr, AsnTypePtr));



/**************************************************
*
*    FLinkSetList2
*
**************************************************/
typedef struct struct_FLink_set_List2 {
   struct struct_FLink_set_List2 PNTR next;
   struct struct_FLink_set_List PNTR   set2;
   Int4   id;
} FLinkSetList2, PNTR FLinkSetList2Ptr;


NLM_EXTERN FLinkSetList2Ptr LIBCALL FLinkSetList2Free PROTO ((FLinkSetList2Ptr ));
NLM_EXTERN FLinkSetList2Ptr LIBCALL FLinkSetList2New PROTO (( void ));
NLM_EXTERN FLinkSetList2Ptr LIBCALL FLinkSetList2AsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL FLinkSetList2AsnWrite PROTO (( FLinkSetList2Ptr , AsnIoPtr, AsnTypePtr));



/**************************************************
*
*    FLinkSetList3
*
**************************************************/
typedef struct struct_FLink_set_List3 {
   struct struct_FLink_set_List2 PNTR   set3;
   Int4   id;
} FLinkSetList3, PNTR FLinkSetList3Ptr;


NLM_EXTERN FLinkSetList3Ptr LIBCALL FLinkSetList3Free PROTO ((FLinkSetList3Ptr ));
NLM_EXTERN FLinkSetList3Ptr LIBCALL FLinkSetList3New PROTO (( void ));
NLM_EXTERN FLinkSetList3Ptr LIBCALL FLinkSetList3AsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL FLinkSetList3AsnWrite PROTO (( FLinkSetList3Ptr , AsnIoPtr, AsnTypePtr));

#ifdef __cplusplus
/* { */ }
#endif

#endif /* _objflinkset_ */

#undef NLM_EXTERN
#ifdef NLM_EXPORT
#define NLM_EXTERN NLM_EXPORT
#else
#define NLM_EXTERN
#endif

/*
$Log: objflinkset.h,v $
Revision 1.1.1.1  2001/10/12 08:21:54  root
Automated import from Sourceforge.net

Revision 1.1.1.1  2001/10/10 22:11:03  kaca
initial import

Revision 1.1  2001/09/11 21:45:10  michel
Moved the ilinkset, flinkset, stack, set and prime files into slrilib

Revision 1.2  2001/04/06 14:33:14  michel
Initial import of makefile and general header

*/

