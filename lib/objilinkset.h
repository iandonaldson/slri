/* $Id: objilinkset.h,v 1.1.1.1 2001/10/12 08:21:54 root Exp $
-- ***************************************************************************
--
--  Samuel Lunenfeld Research Institute (SLRI)
--
--  File: objilinkset.h
--  Description:  Modified LinkSet from NCBI's LinkSet that uses Int4 weights
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

#ifndef _objilinkset_ 
#define _objilinkset_ 

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
*    Generated objects for Module SLRI-ILink-set
*    Generated using ASNCODE Revision: 6.9 at Apr 19, 2001  7:41 PM
*
**************************************************/

NLM_EXTERN Boolean LIBCALL
objilinksetAsnLoad PROTO((void));


/**************************************************
*
*    ILinkSet
*
**************************************************/
typedef struct struct_ILink_set {
   struct struct_ILink_set PNTR next;
   Int4   num;
   Int4Ptr   uids;
   Int4Ptr   weights;
} ILinkSet, PNTR ILinkSetPtr;

NLM_EXTERN ILinkSetPtr LIBCALL ILinkSetNew2 PROTO ((Int4 num));

NLM_EXTERN ILinkSetPtr LIBCALL ILinkSetNew PROTO (( void ));
NLM_EXTERN ILinkSetPtr LIBCALL ILinkSetFree PROTO ((ILinkSetPtr ));
NLM_EXTERN ILinkSetPtr LIBCALL ILinkSetAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL ILinkSetAsnWrite PROTO (( ILinkSetPtr , AsnIoPtr, AsnTypePtr));



/**************************************************
*
*    ILinkSetList
*
**************************************************/
typedef struct struct_ILink_set_List {
   struct struct_ILink_set_List PNTR next;
   struct struct_ILink_set PNTR   set1;
   Int4   id;
} ILinkSetList, PNTR ILinkSetListPtr;


NLM_EXTERN ILinkSetListPtr LIBCALL ILinkSetListFree PROTO ((ILinkSetListPtr ));
NLM_EXTERN ILinkSetListPtr LIBCALL ILinkSetListNew PROTO (( void ));
NLM_EXTERN ILinkSetListPtr LIBCALL ILinkSetListAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL ILinkSetListAsnWrite PROTO (( ILinkSetListPtr , AsnIoPtr, AsnTypePtr));



/**************************************************
*
*    ILinkSetList2
*
**************************************************/
typedef struct struct_ILink_set_List2 {
   struct struct_ILink_set_List2 PNTR next;
   struct struct_ILink_set_List PNTR   set2;
   Int4   id;
} ILinkSetList2, PNTR ILinkSetList2Ptr;


NLM_EXTERN ILinkSetList2Ptr LIBCALL ILinkSetList2Free PROTO ((ILinkSetList2Ptr ));
NLM_EXTERN ILinkSetList2Ptr LIBCALL ILinkSetList2New PROTO (( void ));
NLM_EXTERN ILinkSetList2Ptr LIBCALL ILinkSetList2AsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL ILinkSetList2AsnWrite PROTO (( ILinkSetList2Ptr , AsnIoPtr, AsnTypePtr));



/**************************************************
*
*    ILinkSetList3
*
**************************************************/
typedef struct struct_ILink_set_List3 {
   struct struct_ILink_set_List2 PNTR   set3;
   Int4   id;
} ILinkSetList3, PNTR ILinkSetList3Ptr;


NLM_EXTERN ILinkSetList3Ptr LIBCALL ILinkSetList3Free PROTO ((ILinkSetList3Ptr ));
NLM_EXTERN ILinkSetList3Ptr LIBCALL ILinkSetList3New PROTO (( void ));
NLM_EXTERN ILinkSetList3Ptr LIBCALL ILinkSetList3AsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL ILinkSetList3AsnWrite PROTO (( ILinkSetList3Ptr , AsnIoPtr, AsnTypePtr));

#ifdef __cplusplus
/* { */ }
#endif

#endif /* _objilinkset_ */

#undef NLM_EXTERN
#ifdef NLM_EXPORT
#define NLM_EXTERN NLM_EXPORT
#else
#define NLM_EXTERN
#endif

/*
$log
*/
