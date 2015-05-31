/* $Id: slri_dagtree.h,v 1.3 2004/02/26 20:21:21 mjdumont Exp $
-- ***************************************************************************
--
--  Samuel Lunenfeld Research Institute (SLRI)
--
--  File: slri_dagtree.h
--  Description: DAG tree implementation header.
--               Directed Acyclic Graph (DAG) search tree 
--
--  Author: Michel Dumontier
--  Date: January 2003
--
--  Hogue Lab - University of Toronto Biochemistry Department
--  Samuel Lunenfeld Research Institute, Mount Sinai Hospital
--
--  Copyright Notice:
--
--  Copyright ©2003 Mount Sinai Hospital (MSH)
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

#ifndef _SLRI_DAGTREE_
#define _SLRI_DAGTREE_

#include <ncbi.h>
#include <asn.h>
#include <objslrilibstruc.h>

#ifdef __cplusplus
extern "C" {
#endif



/****************************************/
/** DAG tree structures                **/
/****************************************/

/** Callback function pointer for compare operation
 *  Returns negative if key1 < key2
 *        zero if key1 == key2
 *        positive if key1 > key2  
 */
typedef Int4 (*DAGTree_KeyCompareFunc) PROTO((ValNodePtr key1, ValNodePtr key2));

/** Function pointer for printing/freeing key memory*/
typedef void (*DAGTree_KeyPrintFunc) PROTO((ValNodePtr key, FILE *stream));
typedef ValNodePtr (*DAGTree_KeyFreeFunc) PROTO((ValNodePtr key));

/** Function pointer type for printing/freeing data memory*/
typedef void (*DAGTree_DataPrintFunc) PROTO((ValNodePtr data, FILE *stream));
typedef ValNodePtr (*DAGTree_DataFreeFunc) PROTO((ValNodePtr data));

/** Function pointer package */
typedef struct struct_DTreeFunctionPackage {
	DAGTree_KeyCompareFunc   keyCompareFunc;
	DAGTree_KeyPrintFunc  keyPrintFunc;
	DAGTree_KeyFreeFunc   keyFreeFunc;
	DAGTree_DataPrintFunc dataPrintFunc;
	DAGTree_DataFreeFunc  dataFreeFunc;
	Uint2 keyType;			
	Uint2 dataType;
} DTreeFunctionPackage, PNTR DTreeFunctionPackagePtr;

/** DAG tree root package*/
typedef struct struct_DAGTreeRoot {
	DTreePtr treeRoot;
	DTreeFunctionPackagePtr funPack;
} DTreeRoot, PNTR DTreeRootPtr;


/************************/
/* Function definitions */
/************************/

/** General utility functions*/
/** Copy an Int4/FloatHi into newly allocated memory*/
extern ValNodePtr DAGTree_SaveInt    (Int4 num);
extern ValNodePtr DAGTree_SaveString (CharPtr string);
extern ValNodePtr DAGTree_SaveFloatHi(FloatHi num);


/* General compare, print and free functions*/
/* Compare value functions */
extern Int4 DAGTree_CompareInt    (ValNodePtr val1, ValNodePtr val2);
extern Int4 DAGTree_CompareString (ValNodePtr val1, ValNodePtr val2);
extern Int4 DAGTree_CompareFloatHi(ValNodePtr val1, ValNodePtr val2);

/** Print value functions */
extern void DAGTree_PrintInt    (ValNodePtr val, FILE *stream);
extern void DAGTree_PrintString (ValNodePtr val, FILE *stream);
extern void DAGTree_PrintFloatHi(ValNodePtr val, FILE *stream);

/** Free value functions */
ValNodePtr DAGTree_FreeVnp      (ValNodePtr data);
ValNodePtr DAGTree_FreeVnpData  (ValNodePtr data);
ValNodePtr DAGTree_FreeDummyData(ValNodePtr data);



/** DAGTreeFunctionPackage */
extern DTreeFunctionPackagePtr DAGTree_FunPack_New(DAGTree_KeyCompareFunc  keyCompareFunc,
													 DAGTree_KeyPrintFunc    keyPrintFunc,
													 DAGTree_KeyFreeFunc     keyFreeFunc,
													 DAGTree_DataPrintFunc   dataPrintFunc,
													 DAGTree_DataFreeFunc    dataFreeFunc);
/** DAGTreeFunctionPackage Free */
extern DTreeFunctionPackagePtr DAGTree_FunPack_Free(DTreeFunctionPackagePtr funPack);

/** Allocate/Free a node */
extern DNodePtr DAGTree_NodeNew(ValNodePtr key, ValNodePtr data);                    
extern DNodePtr DAGTree_NodeFree(DNodePtr node, DTreeFunctionPackagePtr funPack);

/** Find/Add/Del/Free a node from the given node */
extern DNodePtr DAGTree_Find(DNodePtr  node, ValNodePtr key, DTreeFunctionPackagePtr funPack);                               
extern DNodePtr DAGTree_Add (DNodePtr *node, ValNodePtr key, Pointer ValNodePtr, DTreeFunctionPackagePtr funPack);
extern DNodePtr DAGTree_Del (DNodePtr node, ValNodePtr key, DTreeFunctionPackagePtr funPack);
extern DNodePtr DAGTree_Free(DNodePtr *node, DTreeFunctionPackagePtr funPack);

/** Add a hierarchical ValNodeList of keys and data to the DAGTree */
extern DNodePtr DAGTree_AddVnpList(DNodePtr *pnode, ValNodePtr keylist, ValNodePtr datalist, DTreeFunctionPackagePtr funPack);
/** Find the last commmon intersecting node given a keylist */
extern DNodePtr DAGTree_FindLastIntersect(DNodePtr root, ValNodePtr keylist, DTreeFunctionPackagePtr funPack);
/** Generically print a DAGTree */
extern Int4 DAGTree_Print(DNodePtr node, Int4 level, DTreeFunctionPackagePtr funPack);

extern const DNodePtr DAGTree_GetFirstLeaf(DNodePtr node);
extern void DAGTree_GetAllLeafs(ValNodePtr *nodes, DNodePtr node);

/********************************************/
/* Higher level functions for specific data */
/********************************************/

/*HOW TO USE:*/

/*
If your data type is supported by the high-level functions (e.g. Int4, CharPtr, FloatHi, ...),
you just need to call DAGTree_Init with the appropriate key and data type defines to create the tree.


If your data type is not supported by high-level functions:
Create a DTreeFunctionPackagePtr, use tree functions, free tree.
The DTreeFunctionPackagePtr contains compare and free functions used by
the tree implementation.  If you are not storing integers or strings, you must write
custom functions (Follow the examples).
NOTE: The 'add' functions do not copy the memory that you have allocated and passed. That
memory becomes part of the tree, so don't free it.  When DAGTree_Free is called, it will
free all of the memory (if the custom free functions have been correctly written).
*/


/*Initialize and free a DAGtree root*/
extern DTreeRootPtr DAGTree_Init(Uint2 keyType, Uint2 dataType);
extern DTreeRootPtr DAGTree_End (DTreeRootPtr treeroot);


/*Select integer or string key or data*/
extern DTreeFunctionPackagePtr DAGTree_FunPack_Init(Uint2 keyType, Uint2 dataType);


#ifdef __cplusplus
}
#endif
#endif		/* _SLRI_DAGTREE_ */

/*
$Log: slri_dagtree.h,v $
Revision 1.3  2004/02/26 20:21:21  mjdumont
Added slri_dagtree.  Fixed linux compiler warnings/errors

Revision 1.2  2004/02/26 19:44:47  mjdumont
Added 2 functions

Revision 1.1.1.2  2003/03/22 09:31:41  root
Automated import from Sourceforge.net

Revision 1.2  2003/03/22 00:03:02  micheld
Added Print function to header

Revision 1.1  2003/03/19 22:15:43  micheld
Initial import of DAG tree algorithm

*/

