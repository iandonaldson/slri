/* $Id: objslrilinkset.h,v 1.5 2004/02/19 23:47:26 mjdumont Exp $
-- ***************************************************************************
--
--  Samuel Lunenfeld Research Institute (SLRI)
--
--  File: objslrilinkset.h
--  Description:  Modified LinkSet from NCBI's LinkSet that uses different weights
--                and can be used in a nested list
--                **** Do not use ASNTOOL to regenerate ****
--                Hard-coded LinkSet component
--
--  Author: Michel Dumontier
--  Date: April 2001 (updated Feb 2002)
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
#ifndef _objslrilinkset_ 
#define _objslrilinkset_ 

#undef NLM_EXTERN
#ifdef NLM_IMPORT
#define NLM_EXTERN NLM_IMPORT
#else
#define NLM_EXTERN extern
#endif


#ifdef __cplusplus
extern "C" { /* } */
#endif

/* AsnLoaders */

NLM_EXTERN Boolean LIBCALL objslrilinksetAsnLoad PROTO((void));


/**************************************************
*
*    CLinkSet
*
**************************************************/
typedef struct struct_CLink_set {
   struct struct_CLink_set PNTR next;
   Int4   num;
   Int4Ptr   uids;
   CharPtr * weights;
} CLinkSet, PNTR CLinkSetPtr;


NLM_EXTERN CLinkSetPtr LIBCALL CLinkSetNew2 PROTO ((Int4 num));
NLM_EXTERN CLinkSetPtr LIBCALL CLinkSetFree PROTO ((CLinkSetPtr ));
NLM_EXTERN CLinkSetPtr LIBCALL CLinkSetNew PROTO (( void ));
NLM_EXTERN CLinkSetPtr LIBCALL CLinkSetAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL CLinkSetAsnWrite PROTO (( CLinkSetPtr , AsnIoPtr, AsnTypePtr));



/**************************************************
*
*    CLinkSetList
*
**************************************************/
typedef struct struct_CLink_set_List {
   struct struct_CLink_set_List PNTR next;
   struct struct_CLink_set PNTR   set1;
   Int4   id;
} CLinkSetList, PNTR CLinkSetListPtr;


NLM_EXTERN CLinkSetListPtr LIBCALL CLinkSetListFree PROTO ((CLinkSetListPtr ));
NLM_EXTERN CLinkSetListPtr LIBCALL CLinkSetListNew PROTO (( void ));
NLM_EXTERN CLinkSetListPtr LIBCALL CLinkSetListAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL CLinkSetListAsnWrite PROTO (( CLinkSetListPtr , AsnIoPtr, AsnTypePtr));



/**************************************************
*
*    CLinkSetList2
*
**************************************************/
typedef struct struct_CLink_set_List2 {
   struct struct_CLink_set_List2 PNTR next;
   struct struct_CLink_set_List PNTR   set2;
   Int4   id;
} CLinkSetList2, PNTR CLinkSetList2Ptr;


NLM_EXTERN CLinkSetList2Ptr LIBCALL CLinkSetList2Free PROTO ((CLinkSetList2Ptr ));
NLM_EXTERN CLinkSetList2Ptr LIBCALL CLinkSetList2New PROTO (( void ));
NLM_EXTERN CLinkSetList2Ptr LIBCALL CLinkSetList2AsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL CLinkSetList2AsnWrite PROTO (( CLinkSetList2Ptr , AsnIoPtr, AsnTypePtr));



/**************************************************
*
*    CLinkSetList3
*
**************************************************/
typedef struct struct_CLink_set_List3 {
   struct struct_CLink_set_List2 PNTR   set3;
   Int4   id;
} CLinkSetList3, PNTR CLinkSetList3Ptr;


NLM_EXTERN CLinkSetList3Ptr LIBCALL CLinkSetList3Free PROTO ((CLinkSetList3Ptr ));
NLM_EXTERN CLinkSetList3Ptr LIBCALL CLinkSetList3New PROTO (( void ));
NLM_EXTERN CLinkSetList3Ptr LIBCALL CLinkSetList3AsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL CLinkSetList3AsnWrite PROTO (( CLinkSetList3Ptr , AsnIoPtr, AsnTypePtr));



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


/**************************************************
*
*    BLinkSet
*
**************************************************/
typedef struct struct_BLink_set {
   struct struct_BLink_set PNTR next;
   Int4   num;
   Int4Ptr   uids;
   Boolean * weights;
} BLinkSet, PNTR BLinkSetPtr;

NLM_EXTERN BLinkSetPtr LIBCALL BLinkSetNew2 PROTO ((Int4 num));

NLM_EXTERN BLinkSetPtr LIBCALL BLinkSetNew PROTO (( void ));
NLM_EXTERN BLinkSetPtr LIBCALL BLinkSetFree PROTO ((BLinkSetPtr ));
NLM_EXTERN BLinkSetPtr LIBCALL BLinkSetAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL BLinkSetAsnWrite PROTO (( BLinkSetPtr , AsnIoPtr, AsnTypePtr));



/**************************************************
*
*    BLinkSetList
*
**************************************************/
typedef struct struct_BLink_set_List {
   struct struct_BLink_set_List PNTR next;
   struct struct_BLink_set PNTR   set1;
   Int4   id;
} BLinkSetList, PNTR BLinkSetListPtr;


NLM_EXTERN BLinkSetListPtr LIBCALL BLinkSetListFree PROTO ((BLinkSetListPtr ));
NLM_EXTERN BLinkSetListPtr LIBCALL BLinkSetListNew PROTO (( void ));
NLM_EXTERN BLinkSetListPtr LIBCALL BLinkSetListAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL BLinkSetListAsnWrite PROTO (( BLinkSetListPtr , AsnIoPtr, AsnTypePtr));



/**************************************************
*
*    BLinkSetList2
*
**************************************************/
typedef struct struct_BLink_set_List2 {
   struct struct_BLink_set_List2 PNTR next;
   struct struct_BLink_set_List PNTR   set2;
   Int4   id;
} BLinkSetList2, PNTR BLinkSetList2Ptr;


NLM_EXTERN BLinkSetList2Ptr LIBCALL BLinkSetList2Free PROTO ((BLinkSetList2Ptr ));
NLM_EXTERN BLinkSetList2Ptr LIBCALL BLinkSetList2New PROTO (( void ));
NLM_EXTERN BLinkSetList2Ptr LIBCALL BLinkSetList2AsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL BLinkSetList2AsnWrite PROTO (( BLinkSetList2Ptr , AsnIoPtr, AsnTypePtr));



/**************************************************
*
*    BLinkSetList3
*
**************************************************/
typedef struct struct_BLink_set_List3 {
   struct struct_BLink_set_List2 PNTR   set3;
   Int4   id;
} BLinkSetList3, PNTR BLinkSetList3Ptr;


NLM_EXTERN BLinkSetList3Ptr LIBCALL BLinkSetList3Free PROTO ((BLinkSetList3Ptr ));
NLM_EXTERN BLinkSetList3Ptr LIBCALL BLinkSetList3New PROTO (( void ));
NLM_EXTERN BLinkSetList3Ptr LIBCALL BLinkSetList3AsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL BLinkSetList3AsnWrite PROTO (( BLinkSetList3Ptr , AsnIoPtr, AsnTypePtr));

/**************************************************
*
*    LLinkSet
*
**************************************************/
typedef struct struct_LLink_set {
   struct struct_LLink_set PNTR next;
   Int4   num;
   Int4Ptr   uids;
   Int8Ptr   weights;
} LLinkSet, PNTR LLinkSetPtr;

NLM_EXTERN LLinkSetPtr LIBCALL LLinkSetNew2 PROTO ((Int4 num));

NLM_EXTERN LLinkSetPtr LIBCALL LLinkSetNew PROTO (( void ));
NLM_EXTERN LLinkSetPtr LIBCALL LLinkSetFree PROTO ((LLinkSetPtr ));
NLM_EXTERN LLinkSetPtr LIBCALL LLinkSetAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL LLinkSetAsnWrite PROTO (( LLinkSetPtr , AsnIoPtr, AsnTypePtr));



/**************************************************
*
*    LLinkSetList
*
**************************************************/
typedef struct struct_LLink_set_List {
   struct struct_LLink_set_List PNTR next;
   struct struct_LLink_set PNTR   set1;
   Int4   id;
} LLinkSetList, PNTR LLinkSetListPtr;


NLM_EXTERN LLinkSetListPtr LIBCALL LLinkSetListFree PROTO ((LLinkSetListPtr ));
NLM_EXTERN LLinkSetListPtr LIBCALL LLinkSetListNew PROTO (( void ));
NLM_EXTERN LLinkSetListPtr LIBCALL LLinkSetListAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL LLinkSetListAsnWrite PROTO (( LLinkSetListPtr , AsnIoPtr, AsnTypePtr));



/**************************************************
*
*    LLinkSetList2
*
**************************************************/
typedef struct struct_LLink_set_List2 {
   struct struct_LLink_set_List2 PNTR next;
   struct struct_LLink_set_List PNTR   set2;
   Int4   id;
} LLinkSetList2, PNTR LLinkSetList2Ptr;


NLM_EXTERN LLinkSetList2Ptr LIBCALL LLinkSetList2Free PROTO ((LLinkSetList2Ptr ));
NLM_EXTERN LLinkSetList2Ptr LIBCALL LLinkSetList2New PROTO (( void ));
NLM_EXTERN LLinkSetList2Ptr LIBCALL LLinkSetList2AsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL LLinkSetList2AsnWrite PROTO (( LLinkSetList2Ptr , AsnIoPtr, AsnTypePtr));



/**************************************************
*
*    LLinkSetList3
*
**************************************************/
typedef struct struct_LLink_set_List3 {
   struct struct_LLink_set_List2 PNTR   set3;
   Int4   id;
} LLinkSetList3, PNTR LLinkSetList3Ptr;


NLM_EXTERN LLinkSetList3Ptr LIBCALL LLinkSetList3Free PROTO ((LLinkSetList3Ptr ));
NLM_EXTERN LLinkSetList3Ptr LIBCALL LLinkSetList3New PROTO (( void ));
NLM_EXTERN LLinkSetList3Ptr LIBCALL LLinkSetList3AsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL LLinkSetList3AsnWrite PROTO (( LLinkSetList3Ptr , AsnIoPtr, AsnTypePtr));


/**************************************************
*
*    YLinkSet
*
**************************************************/
typedef struct struct_YLink_set {
   struct struct_YLink_set PNTR next;
   Int4   num;
   Int4Ptr   uids;
   Byte *    weights;
} YLinkSet, PNTR YLinkSetPtr;

NLM_EXTERN YLinkSetPtr LIBCALL YLinkSetNew2 PROTO ((Int4 num));

NLM_EXTERN YLinkSetPtr LIBCALL YLinkSetNew PROTO (( void ));
NLM_EXTERN YLinkSetPtr LIBCALL YLinkSetFree PROTO ((YLinkSetPtr ));
NLM_EXTERN YLinkSetPtr LIBCALL YLinkSetAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL YLinkSetAsnWrite PROTO (( YLinkSetPtr , AsnIoPtr, AsnTypePtr));



/**************************************************
*
*    YLinkSetList
*
**************************************************/
typedef struct struct_YLink_set_List {
   struct struct_YLink_set_List PNTR next;
   struct struct_YLink_set PNTR   set1;
   Int4   id;
} YLinkSetList, PNTR YLinkSetListPtr;


NLM_EXTERN YLinkSetListPtr LIBCALL YLinkSetListFree PROTO ((YLinkSetListPtr ));
NLM_EXTERN YLinkSetListPtr LIBCALL YLinkSetListNew PROTO (( void ));
NLM_EXTERN YLinkSetListPtr LIBCALL YLinkSetListAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL YLinkSetListAsnWrite PROTO (( YLinkSetListPtr , AsnIoPtr, AsnTypePtr));



/**************************************************
*
*    YLinkSetList2
*
**************************************************/
typedef struct struct_YLink_set_List2 {
   struct struct_YLink_set_List2 PNTR next;
   struct struct_YLink_set_List PNTR   set2;
   Int4   id;
} YLinkSetList2, PNTR YLinkSetList2Ptr;


NLM_EXTERN YLinkSetList2Ptr LIBCALL YLinkSetList2Free PROTO ((YLinkSetList2Ptr ));
NLM_EXTERN YLinkSetList2Ptr LIBCALL YLinkSetList2New PROTO (( void ));
NLM_EXTERN YLinkSetList2Ptr LIBCALL YLinkSetList2AsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL YLinkSetList2AsnWrite PROTO (( YLinkSetList2Ptr , AsnIoPtr, AsnTypePtr));



/**************************************************
*
*    YLinkSetList3
*
**************************************************/
typedef struct struct_YLink_set_List3 {
   struct struct_YLink_set_List2 PNTR   set3;
   Int4   id;
} YLinkSetList3, PNTR YLinkSetList3Ptr;


NLM_EXTERN YLinkSetList3Ptr LIBCALL YLinkSetList3Free PROTO ((YLinkSetList3Ptr ));
NLM_EXTERN YLinkSetList3Ptr LIBCALL YLinkSetList3New PROTO (( void ));
NLM_EXTERN YLinkSetList3Ptr LIBCALL YLinkSetList3AsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL YLinkSetList3AsnWrite PROTO (( YLinkSetList3Ptr , AsnIoPtr, AsnTypePtr));




/**************************************************
*
*    IVector
*
**************************************************/
typedef struct struct_IVector {
	struct struct_IVector PNTR next;
   Int4   i;
   Int4Ptr   v;
} IVector, PNTR IVectorPtr;

NLM_EXTERN IVectorPtr LIBCALL IVectorNewEx(void);
NLM_EXTERN IVectorPtr LIBCALL IVectorFree PROTO ((IVectorPtr ));
NLM_EXTERN IVectorPtr LIBCALL IVectorNew PROTO (( Int4 ));
NLM_EXTERN IVectorPtr LIBCALL IVectorAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL IVectorAsnWrite PROTO (( IVectorPtr , AsnIoPtr, AsnTypePtr));


/**************************************************
*
*    IVectorList
*
**************************************************/
typedef struct struct_IVectorList {
   Int4   id;
   struct struct_IVector PNTR   iV;
} IVectorList, PNTR IVectorListPtr;


NLM_EXTERN IVectorListPtr LIBCALL IVectorListFree PROTO ((IVectorListPtr ));
NLM_EXTERN IVectorListPtr LIBCALL IVectorListNew PROTO (( void ));
NLM_EXTERN IVectorListPtr LIBCALL IVectorListAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL IVectorListAsnWrite PROTO (( IVectorListPtr , AsnIoPtr, AsnTypePtr));

/**************************************************
*
*    IMatrix
*
**************************************************/
typedef struct struct_IMatrix {
	struct struct_IMatrix PNTR next;
   Int4   i;
   Int4   j;
   Int4Ptr * m;
} IMatrix, PNTR IMatrixPtr;


NLM_EXTERN IMatrixPtr LIBCALL IMatrixFree PROTO ((IMatrixPtr ));
NLM_EXTERN IMatrixPtr LIBCALL IMatrixNew PROTO (( Int4, Int4 ));
NLM_EXTERN IMatrixPtr LIBCALL IMatrixAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL IMatrixAsnWrite PROTO (( IMatrixPtr , AsnIoPtr, AsnTypePtr));


/**************************************************
*
*    IMatrixList
*
**************************************************/
typedef struct struct_IMatrixList {
   struct struct_IMatrixList PNTR next;
   Int4   id;
   struct struct_IMatrix PNTR   iM;
} IMatrixList, PNTR IMatrixListPtr;


NLM_EXTERN IMatrixListPtr LIBCALL IMatrixListFree PROTO ((IMatrixListPtr ));
NLM_EXTERN IMatrixListPtr LIBCALL IMatrixListNew PROTO (( void ));
NLM_EXTERN IMatrixListPtr LIBCALL IMatrixListAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL IMatrixListAsnWrite PROTO (( IMatrixListPtr , AsnIoPtr, AsnTypePtr));


/**************************************************
*
*    IMatrixListSet
*
**************************************************/
typedef struct struct_IMatrixListSet {
   Int4   id;
   struct struct_IMatrixList PNTR   iML;
} IMatrixListSet, PNTR IMatrixListSetPtr;


NLM_EXTERN IMatrixListSetPtr LIBCALL IMatrixListSetFree PROTO ((IMatrixListSetPtr ));
NLM_EXTERN IMatrixListSetPtr LIBCALL IMatrixListSetNew PROTO (( void ));
NLM_EXTERN IMatrixListSetPtr LIBCALL IMatrixListSetAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL IMatrixListSetAsnWrite PROTO (( IMatrixListSetPtr , AsnIoPtr, AsnTypePtr));


/**************************************************
*
*    FHiVector
*
**************************************************/
typedef struct struct_FHiVector {
	struct struct_FHiVector PNTR next;
   Int4   i;
   FloatHiPtr   v;
} FHiVector, PNTR FHiVectorPtr;

NLM_EXTERN FHiVectorPtr LIBCALL FHiVectorNewEx(void);
NLM_EXTERN FHiVectorPtr LIBCALL FHiVectorFree PROTO ((FHiVectorPtr ));
NLM_EXTERN FHiVectorPtr LIBCALL FHiVectorNew PROTO (( Int4 ));
NLM_EXTERN FHiVectorPtr LIBCALL FHiVectorAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL FHiVectorAsnWrite PROTO (( FHiVectorPtr , AsnIoPtr, AsnTypePtr));


/**************************************************
*
*    FHiVectorList
*
**************************************************/
typedef struct struct_FHiVectorList {
   Int4   id;
   struct struct_FHiVector PNTR   iV;
} FHiVectorList, PNTR FHiVectorListPtr;


NLM_EXTERN FHiVectorListPtr LIBCALL FHiVectorListFree PROTO ((FHiVectorListPtr ));
NLM_EXTERN FHiVectorListPtr LIBCALL FHiVectorListNew PROTO (( void ));
NLM_EXTERN FHiVectorListPtr LIBCALL FHiVectorListAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL FHiVectorListAsnWrite PROTO (( FHiVectorListPtr , AsnIoPtr, AsnTypePtr));

/**************************************************
*
*    FHiMatrix
*
**************************************************/
typedef struct struct_FHiMatrix {
	struct struct_FHiMatrix PNTR next;
   Int4   i;
   Int4   j;
   FloatHiPtr * m;
} FHiMatrix, PNTR FHiMatrixPtr;


NLM_EXTERN FHiMatrixPtr LIBCALL FHiMatrixFree PROTO ((FHiMatrixPtr ));
NLM_EXTERN FHiMatrixPtr LIBCALL FHiMatrixNew PROTO (( Int4, Int4 ));
NLM_EXTERN FHiMatrixPtr LIBCALL FHiMatrixAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL FHiMatrixAsnWrite PROTO (( FHiMatrixPtr , AsnIoPtr, AsnTypePtr));


/**************************************************
*
*    FHiMatrixList
*
**************************************************/
typedef struct struct_FHiMatrixList {
   struct struct_FHiMatrixList PNTR next;
   Int4   id;
   struct struct_FHiMatrix PNTR   iM;
} FHiMatrixList, PNTR FHiMatrixListPtr;


NLM_EXTERN FHiMatrixListPtr LIBCALL FHiMatrixListFree PROTO ((FHiMatrixListPtr ));
NLM_EXTERN FHiMatrixListPtr LIBCALL FHiMatrixListNew PROTO (( void ));
NLM_EXTERN FHiMatrixListPtr LIBCALL FHiMatrixListAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL FHiMatrixListAsnWrite PROTO (( FHiMatrixListPtr , AsnIoPtr, AsnTypePtr));


/**************************************************
*
*    FHiMatrixListSet
*
**************************************************/
typedef struct struct_FHiMatrixListSet {
   Int4   id;
   struct struct_FHiMatrixList PNTR   iML;
} FHiMatrixListSet, PNTR FHiMatrixListSetPtr;


NLM_EXTERN FHiMatrixListSetPtr LIBCALL FHiMatrixListSetFree PROTO ((FHiMatrixListSetPtr ));
NLM_EXTERN FHiMatrixListSetPtr LIBCALL FHiMatrixListSetNew PROTO (( void ));
NLM_EXTERN FHiMatrixListSetPtr LIBCALL FHiMatrixListSetAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL FHiMatrixListSetAsnWrite PROTO (( FHiMatrixListSetPtr , AsnIoPtr, AsnTypePtr));

/**************************************************
*
*    FLoVector
*
**************************************************/
typedef struct struct_FLoVector {
	struct struct_FLoVector PNTR next;
   Int4   i;
   FloatLoPtr   v;
} FLoVector, PNTR FLoVectorPtr;

NLM_EXTERN FLoVectorPtr LIBCALL FLoVectorNewEx(void);
NLM_EXTERN FLoVectorPtr LIBCALL FLoVectorFree PROTO ((FLoVectorPtr ));
NLM_EXTERN FLoVectorPtr LIBCALL FLoVectorNew PROTO (( Int4 ));
NLM_EXTERN FLoVectorPtr LIBCALL FLoVectorAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL FLoVectorAsnWrite PROTO (( FLoVectorPtr , AsnIoPtr, AsnTypePtr));


/**************************************************
*
*    FLoVectorList
*
**************************************************/
typedef struct struct_FLoVectorList {
   Int4   id;
   struct struct_FLoVector PNTR   iV;
} FLoVectorList, PNTR FLoVectorListPtr;


NLM_EXTERN FLoVectorListPtr LIBCALL FLoVectorListFree PROTO ((FLoVectorListPtr ));
NLM_EXTERN FLoVectorListPtr LIBCALL FLoVectorListNew PROTO (( void ));
NLM_EXTERN FLoVectorListPtr LIBCALL FLoVectorListAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL FLoVectorListAsnWrite PROTO (( FLoVectorListPtr , AsnIoPtr, AsnTypePtr));




/**************************************************
*
*    CVector
*
**************************************************/
typedef struct struct_CVector {
	struct struct_CVector PNTR next;
   Int4   i;
   CharPtr *v;
} CVector, PNTR CVectorPtr;

NLM_EXTERN CVectorPtr LIBCALL CVectorNewEx(void);
NLM_EXTERN CVectorPtr LIBCALL CVectorFree PROTO ((CVectorPtr ));
NLM_EXTERN CVectorPtr LIBCALL CVectorNew PROTO (( Int4 ));
NLM_EXTERN CVectorPtr LIBCALL CVectorAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL CVectorAsnWrite PROTO (( CVectorPtr , AsnIoPtr, AsnTypePtr));


/**************************************************
*
*    CVectorList
*
**************************************************/
typedef struct struct_CVectorList {
   Int4   id;
   struct struct_CVector PNTR   iV;
} CVectorList, PNTR CVectorListPtr;


NLM_EXTERN CVectorListPtr LIBCALL CVectorListFree PROTO ((CVectorListPtr ));
NLM_EXTERN CVectorListPtr LIBCALL CVectorListNew PROTO (( void ));
NLM_EXTERN CVectorListPtr LIBCALL CVectorListAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL CVectorListAsnWrite PROTO (( CVectorListPtr , AsnIoPtr, AsnTypePtr));



#ifdef __cplusplus
/* { */ }
#endif

#endif /* _objclinkset_ */

#undef NLM_EXTERN
#ifdef NLM_EXPORT
#define NLM_EXTERN NLM_EXPORT
#else
#define NLM_EXTERN
#endif

/*
$Log: objslrilinkset.h,v $
Revision 1.5  2004/02/19 23:47:26  mjdumont
Added FHiMatrix and lists

Revision 1.4  2004/02/02 20:52:45  mjdumont
added CVector

Revision 1.3  2003/11/05 18:41:07  michel
Changed FVector to FHiVector to avoid conflict with OS X's Carbon

Revision 1.2  2003/10/20 18:38:51  michel
Added a FloatLo Vector ASN.1 type

Revision 1.1.1.4  2003/08/21 08:48:06  root
Automated import from Sourceforge.net

Revision 1.4  2003/08/20 14:41:30  micheld
Added FHiVector - FloatHi Vector.

Revision 1.3  2003/02/14 15:17:39  micheld
Added IMatrixListSet to add NEXT field to IMatrixList

Revision 1.2  2003/02/13 16:44:24  micheld
Added basic integer array vector(1D) and matrix(2D) types

Revision 1.1  2002/03/25 06:44:22  micheld
Added comprehensive suite of linksets (only clinkset, flinkset and ilinkset tested)

*/
