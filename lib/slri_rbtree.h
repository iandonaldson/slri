/* $Id: slri_rbtree.h,v 1.1.1.3 2002/01/16 09:23:28 root Exp $
-- ***************************************************************************
--
--  Samuel Lunenfeld Research Institute (SLRI)
--
--  File: slri_rbtree.h
--  Description: Red black tree implementation header.
--               Balanced binary search tree that guarantees O[lg(n)]
--               performance for all operations. Worst case is [ceiling of
--               2*lg(n)]+1.
--
--  Author: Gary Bader
--  Date: April 2000
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

#ifndef _SLRI_RBTREE_
#define _SLRI_RBTREE_

#include <ncbi.h>

#ifdef __cplusplus
extern "C" {
#endif

/****************************************/
/* Generalized Red-Black tree structure */
/****************************************/

#define RBTreeRED 1
#define RBTreeBLACK 2

typedef struct struct_RedBlackTreeNode {
   Pointer key;
   Pointer data;
   Uint1   colour;
   struct struct_RedBlackTreeNode PNTR   parent;
   struct struct_RedBlackTreeNode PNTR   leftChild;
   struct struct_RedBlackTreeNode PNTR   rightChild;
} RedBlackTreeNode, PNTR RedBlackTreeNodePtr;

/*function pointer type for compare operation*/
/*Returns negative if key1 < key2
          zero if key1 == key2
          positive if key1 > key2  */
typedef Int4 (*RBTree_CompareFunc) PROTO((Pointer key1, Pointer key2));

/*function pointer type for freeing key memory*/
typedef Pointer (*RBTree_KeyFreeFunc) PROTO((Pointer key));

/*function pointer type for freeing data memory*/
typedef Pointer (*RBTree_DataFreeFunc) PROTO((Pointer data));

/*package of above function pointers*/
typedef struct struct_RedBlackTreeFunctionPackage {
	RBTree_CompareFunc  compareFunc;
	RBTree_KeyFreeFunc keyFreeFunc;
	RBTree_DataFreeFunc dataFreeFunc;
	Uint2 keyType;			/*Used by top level RBTree functions*/
	Uint2 dataType;
} RedBlackTreeFunctionPackage, PNTR RedBlackTreeFunctionPackagePtr;

/*neater package of a red black tree*/
typedef struct struct_RedBlackTreeRoot {
	RedBlackTreeNodePtr treeRoot;
	RedBlackTreeFunctionPackagePtr funPack;
} RedBlackTreeRoot, PNTR RedBlackTreeRootPtr;

/************************/
/* Function definitions */
/************************/

/****************************/
/* Red-Black Tree functions */
/****************************/

/*See any good computer science algorithms book for explanation of how this works*/

/*general utility functions*/

/*copy an integer into newly allocated memory*/
extern Int4Ptr RBTree_IntSave(Int4 num);

/*general compare and free functions*/

/*compare integer keys*/
extern Int4 RBTree_CompareIntKeyFunction(Pointer key1, Pointer key2);

/*compare string keys*/
extern Int4 RBTree_CompareStringKeyFunction(Pointer key1, Pointer key2);

/*compare real (FloatHi) keys*/
extern Int4 RBTree_CompareFloatHiKeyFunction(Pointer key1, Pointer key2);

/*free integer key*/
extern Pointer RBTree_IntKeyFree(Pointer key);

/*free string key*/
extern Pointer RBTree_StringKeyFree(Pointer key);

/*free FloatHi key*/
extern Pointer RBTree_FloatHiKeyFree(Pointer key);

/*free integer data*/
extern Pointer RBTree_IntDataFree(Pointer data);

/*free string data*/
extern Pointer RBTree_StringDataFree(Pointer data);

/*free ValNodeList data*/
extern Pointer RBTree_VnpDataFree(Pointer data);

/*free FloatHi data*/
extern Pointer RBTree_FloatHiDataFree(Pointer data);

/*create a function package for the Red-Black tree*/
extern RedBlackTreeFunctionPackagePtr RBTree_FunPack_New(RBTree_CompareFunc compareFunc, RBTree_KeyFreeFunc keyFreeFunc,
										  RBTree_DataFreeFunc dataFreeFunc);

/*free a RedBlackTreeFunctionPackage*/
extern RedBlackTreeFunctionPackagePtr RBTree_FunPack_Free(RedBlackTreeFunctionPackagePtr funPack);

/*create a new RBTree node*/
extern RedBlackTreeNodePtr RBTree_NodeNew(Pointer key, Pointer data);

/*free a RBTree node*/
extern RedBlackTreeNodePtr RBTree_NodeFree(RedBlackTreeNodePtr node, RedBlackTreeFunctionPackagePtr funPack);

/*find the node containing the minimum key in the tree*/
extern RedBlackTreeNodePtr RBTree_MinKeyNode(RedBlackTreeNodePtr treeRoot);

/*find the node containing the maximum key in the tree*/
extern RedBlackTreeNodePtr RBTree_MaxKeyNode(RedBlackTreeNodePtr treeRoot);

/*iterator to find node containing next lowest key in tree from key in node x*/
extern RedBlackTreeNodePtr RBTree_Predecessor(RedBlackTreeNodePtr x);

/*iterator to find node containing next highest key in tree from key in node x*/
extern RedBlackTreeNodePtr RBTree_Successor(RedBlackTreeNodePtr x);

/*rotate a tree node (x); left*/
extern Boolean RBTree_RotateLeft(RedBlackTreeNodePtr *treeRoot, RedBlackTreeNodePtr x);

/*roatate a tree node (y); right*/
extern Boolean RBTree_RotateRight(RedBlackTreeNodePtr *treeRoot, RedBlackTreeNodePtr y);

/*find a node in the tree, by given key*/
extern RedBlackTreeNodePtr RBTree_Find(RedBlackTreeNodePtr treeRoot, Pointer key, RedBlackTreeFunctionPackagePtr funPack);

/*add a node to the tree*/
extern Boolean RBTree_Add(RedBlackTreeNodePtr *treeRoot, Pointer key, Pointer data, RedBlackTreeFunctionPackagePtr funPack);

/*remove a node from the tree*/
extern Boolean RBTree_Del(RedBlackTreeNodePtr *treeRoot, Pointer key, RedBlackTreeFunctionPackagePtr funPack);

/*free a Red-black tree*/
extern RedBlackTreeNodePtr RBTree_Free(RedBlackTreeNodePtr treeRoot, RedBlackTreeFunctionPackagePtr funPack);

/********************************************/
/* Higher level functions for specific data */
/********************************************/

/*HOW TO USE:*/

/*
If your data type is supported by the high-level functions (e.g. Int4, CharPtr, FloatHi, ...),
you just need to call RBTree_Root_Init with the appropriate key and data type defines to create the tree.
*/

/*
If your data type is not supported by high-level functions:
Create a RedBlackTreeFunctionPackagePtr, use tree functions, free tree.
The RedBlackTreeFunctionPackagePtr contains compare and free functions used by
the tree implementation.  If you are not storing integers or strings, you must write
custom functions (Follow the examples).
NOTE: The 'add' functions do not copy the memory that you have allocated and passed. That
memory becomes part of the tree, so don't free it.  When RBTree_Free is called, it will
free all of the memory (if the custom free functions have been correctly written).
*/

/*currently supported key and data type selections*/
#define RBTreeKeyInt 1
#define RBTreeKeyString 2
#define RBTreeKeyFloatHi 3
#define RBTreeDataInt 100
#define RBTreeDataString 101
#define RBTreeDataValNodeList 102		/*Uses ValNodeFree*/
#define RBTreeDataFloatHi 103
#define RBTreeDataUserDefinedNoFree 104	/*Does not free the data, user must free seperately*/
#define RBTreeDataMemFree 105			/*general MemFree*/

/*Initialize a rbtree root*/
extern RedBlackTreeRootPtr RBTree_Root_Init(Uint2 keyType, Uint2 dataType);

/*free rbtree root*/
extern RedBlackTreeRootPtr RBTree_Root_Free(RedBlackTreeRootPtr rbtrp);

/*Helper functions*/

/*Copies an integer to new memory - useful when your data type is an Int4*/
extern Int4Ptr RBTree_CopyInt4(Int4 data);

/*integer key functions*/

/*find a node by integer key*/
extern RedBlackTreeNodePtr RBTree_Root_FindInt(RedBlackTreeRootPtr treeRoot, Int4 key);

/*add a node given an integer key and a pointer to data that will be linked in the tree*/
/*The data that you allocate becomes part of the tree, so don't free it after you add to the tree.*/
extern Boolean RBTree_Root_AddInt(RedBlackTreeRootPtr treeRoot, Int4 key, Pointer data);

/*delete a node given an integer key*/
extern Boolean RBTree_Root_DelInt(RedBlackTreeRootPtr treeRoot, Int4 key);

/*string key functions*/

/*find a node by string key*/
extern RedBlackTreeNodePtr RBTree_Root_FindString(RedBlackTreeRootPtr treeRoot, CharPtr key);

/*add a node given a string key and a pointer to data that will be linked in the tree*/
/*The data that you allocate becomes part of the tree, so don't free it after you add to the tree.*/
extern Boolean RBTree_Root_AddString(RedBlackTreeRootPtr treeRoot, CharPtr key, Pointer data);

/*delete a node given a string key*/
extern Boolean RBTree_Root_DelString(RedBlackTreeRootPtr treeRoot, CharPtr key);

/*FloatHi key functions*/

/*find a node by FloatHi key*/
extern RedBlackTreeNodePtr RBTree_Root_FindFloatHi(RedBlackTreeRootPtr treeRoot, FloatHi key);

/*add a node given a FloatHi key and a pointer to data that will be linked in the tree*/
/*The data that you allocate becomes part of the tree, so don't free it after you add to the tree.*/
extern Boolean RBTree_Root_AddFloatHi(RedBlackTreeRootPtr treeRoot, FloatHi key, Pointer data);

/*delete a node given a FloatHi key*/
extern Boolean RBTree_Root_DelFloatHi(RedBlackTreeRootPtr treeRoot, FloatHi key);


/*-----------Below are old functions, please use above-------------*/

/*Select integer or string key or data*/
extern RedBlackTreeFunctionPackagePtr RBTree_FunPack_Init(Uint2 keyType, Uint2 dataType);

/*integer key functions*/

/*find a node by integer key*/
extern RedBlackTreeNodePtr RBTree_FindInt(RedBlackTreeNodePtr treeRoot, Int4 key, RedBlackTreeFunctionPackagePtr funPack);

/*add a node given an integer key and a pointer to data that will be linked in the tree*/
/*The data that you allocate becomes part of the tree, so don't free it after you add to the tree.*/
extern Boolean RBTree_AddInt(RedBlackTreeNodePtr *treeRoot, Int4 key, Pointer data, RedBlackTreeFunctionPackagePtr funPack);

/*delete a node given an integer key*/
extern Boolean RBTree_DelInt(RedBlackTreeNodePtr *treeRoot, Int4 key, RedBlackTreeFunctionPackagePtr funPack);

/*string key functions*/

/*find a node by string key*/
extern RedBlackTreeNodePtr RBTree_FindString(RedBlackTreeNodePtr treeRoot, CharPtr key, RedBlackTreeFunctionPackagePtr funPack);

/*add a node given a string key and a pointer to data that will be linked in the tree*/
/*The data that you allocate becomes part of the tree, so don't free it after you add to the tree.*/
extern Boolean RBTree_AddString(RedBlackTreeNodePtr *treeRoot, CharPtr key, Pointer data, RedBlackTreeFunctionPackagePtr funPack);

/*delete a node given a string key*/
extern Boolean RBTree_DelString(RedBlackTreeNodePtr *treeRoot, CharPtr key, RedBlackTreeFunctionPackagePtr funPack);

/*************/
/* Test Area */
/*************/

/*check that the red black tree properties hold true*/
extern Boolean RBTree_CheckInvariant(RedBlackTreeNodePtr root, RedBlackTreeFunctionPackagePtr funPack);

/*test the Red-Black tree implementation*/
extern void RBTree_Test(void);


#ifdef __cplusplus
}
#endif
#endif		/* _SLRI_RBTREE_ */

/*
$Log: slri_rbtree.h,v $
Revision 1.1.1.3  2002/01/16 09:23:28  root
Automated import from Sourceforge.net

Revision 1.3  2002/01/16 03:31:05  gbader
Added more data type options for top level RB tree functions

Revision 1.2  2002/01/12 17:11:57  gbader
Added option ot save data and not free it on top level tree functions.

Revision 1.1.1.3  2001/10/10 22:11:04  kaca
initial import

Revision 1.10  2001/08/02 03:24:09  gbader
Added functionality for FloatHi data types in higher level tree functions

Revision 1.9  2001/07/03 18:54:17  gbader
Added more higher level rbtree functions

Revision 1.8  2001/06/26 04:27:14  gbader
Added higher level wrapper for Red-Black tree.

Revision 1.7  2001/04/12 15:08:11  gbader
Added comments

Revision 1.6  2001/03/28 21:45:51  gbader
Moved externs to proper place before function return type

Revision 1.5  2001/03/15 21:09:46  gbader
Moved extern "C" statements to proper place in header files (after includes)

Revision 1.4  2001/01/13 04:04:10  gbader
Added IBM DB2 UDB to license exceptions.

Revision 1.3  2000/09/05 13:55:08  gbader
Updated License so that it is now GNU.
Added CVS keywords ID and Log as well as new line at end of files so compilers on some platforms don't complain

*/

