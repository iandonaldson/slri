/* $Id: slri_rbtree.c,v 1.1.1.3 2002/01/16 09:23:28 root Exp $
-- ***************************************************************************
--
--  Samuel Lunenfeld Research Institute (SLRI)
--
--  File: slri_rbtree.c
--  Description: Red black tree implementation
--
--  Author: Gary Bader
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

#include <slri_rbtree.h>

/************************/
/* Function definitions */
/************************/

/****************************/
/* Red-Black Tree functions */
/****************************/

/*red-black tree functions for node number to BIND ID map functions*/
/*See any good computer science algorithms book for explanation of how this works*/

/*create a function package for the Red-Black tree*/
RedBlackTreeFunctionPackagePtr RBTree_FunPack_New(RBTree_CompareFunc compareFunc, RBTree_KeyFreeFunc keyFreeFunc,
										  RBTree_DataFreeFunc dataFreeFunc)
{
	RedBlackTreeFunctionPackagePtr pFunPack=NULL;

	if(compareFunc==NULL) {
		ErrPostEx(SEV_INFO, 1, 0, "RBTree_FunPack_New: passed compareFunc is null.");
		return(NULL);
	}
	if(keyFreeFunc==NULL) {
		ErrPostEx(SEV_INFO, 1, 0, "RBTree_FunPack_New: passed keyFreeFunc is null.");
		return(NULL);
	}
	if(dataFreeFunc==NULL) {
		ErrPostEx(SEV_INFO, 1, 0, "RBTree_FunPack_New: passed dataFreeFunc is null.");
		return(NULL);
	}

	pFunPack = MemNew((size_t) sizeof(RedBlackTreeFunctionPackage));
	if(pFunPack==NULL) {
		ErrPostEx(SEV_INFO, 1, 0, "RBTree_FunPack_New: MemNew failed.");
		return(NULL);
	}

	pFunPack->compareFunc = compareFunc;
	pFunPack->keyFreeFunc = keyFreeFunc;
	pFunPack->dataFreeFunc = dataFreeFunc;

	return(pFunPack);
}

/*general utility functions*/

/*copy an integer into newly allocated memory*/
Int4Ptr RBTree_IntSave(Int4 num)
{
	Int4Ptr pNumCopy=NULL;

	pNumCopy = MemNew((size_t) sizeof(Int4));
	if(pNumCopy==NULL) {
		ErrPostEx(SEV_INFO, 1, 0, "RBTree_IntSave: MemNew failed.");
		return(FALSE);
	}
	*pNumCopy = num;

	return(pNumCopy);
}

/*general compare and free functions*/

/*compare integer keys*/
Int4 RBTree_CompareIntKeyFunction(Pointer key1, Pointer key2)
{
	Int4Ptr keyp1, keyp2;
	Int4 num1, num2;

	keyp1 = (Int4Ptr) key1;
	keyp2 = (Int4Ptr) key2;
	num1 = *keyp1;
	num2 = *keyp2;

	return((Int4)(num1 - num2));
}

/*compare string keys*/
Int4 RBTree_CompareStringKeyFunction(Pointer key1, Pointer key2)
{
	CharPtr pc1, pc2;

	pc1 = (CharPtr) key1;
	pc2 = (CharPtr) key2;

	return((Int4) StringCmp(pc1, pc2));
}

/*compare real (FloatHi) keys*/
Int4 RBTree_CompareFloatHiKeyFunction(Pointer key1, Pointer key2)
{
	FloatHiPtr keyp1, keyp2;
	FloatHi num1, num2, diff;

	keyp1 = (FloatHiPtr) key1;
	keyp2 = (FloatHiPtr) key2;
	num1 = *keyp1;
	num2 = *keyp2;

	diff = num1 - num2;

	if(diff>0) {
		return(1);
	}
	else if(diff<0) {
		return(-1);
	}

	return((Int4) diff);	/*diff is zero*/
}

/*free integer key*/
Pointer RBTree_IntKeyFree(Pointer key)
{
	Int4Ptr keyp;

	keyp = (Int4Ptr) key;

	return(MemFree(keyp));
}

/*free string key*/
Pointer RBTree_StringKeyFree(Pointer key)
{
	CharPtr pc;

	pc = (CharPtr) key;

	return(MemFree(pc));
}

/*free FloatHi key*/
Pointer RBTree_FloatHiKeyFree(Pointer key)
{
	FloatHiPtr keyp;

	keyp = (FloatHiPtr) key;

	return(MemFree(keyp));
}

/*free integer data*/
Pointer RBTree_IntDataFree(Pointer data)
{
	Int4Ptr datap;

	datap = (Int4Ptr) data;

	return(MemFree(datap));
}

/*free string data*/
Pointer RBTree_StringDataFree(Pointer data)
{
	CharPtr datap;

	datap = (CharPtr) data;

	return(MemFree(datap));
}

/*free ValNodeList data*/
Pointer RBTree_VnpDataFree(Pointer data)
{
	ValNodePtr datap;

	datap = (ValNodePtr) data;

	return(ValNodeFree(datap));
}

/*free FloatHi data*/
Pointer RBTree_FloatHiDataFree(Pointer data)
{
	FloatHiPtr datap;

	datap = (FloatHiPtr) data;

	return(MemFree(datap));
}

/*Dummy function to not free any data*/
Pointer RBTree_DummyDataFree(Pointer data)
{
	return(NULL);
}

/*free data using MemFree*/
Pointer RBTree_DataMemFree(Pointer data)
{
	return(MemFree(data));
}

/*free a RedBlackTreeFunctionPackage*/
RedBlackTreeFunctionPackagePtr RBTree_FunPack_Free(RedBlackTreeFunctionPackagePtr funPack)
{
	if(funPack==NULL) {
		return(NULL);
	}

	return(MemFree(funPack));
}

/*create a new RBTree node*/
RedBlackTreeNodePtr RBTree_NodeNew(Pointer key, Pointer data)
{
	RedBlackTreeNodePtr newNode=NULL;

	if(key==NULL) {
		ErrPostEx(SEV_INFO, 1, 0, "RBTree_NewNode: passed key is null.");
		return(NULL);
	}

	newNode = MemNew((size_t)sizeof(RedBlackTreeNode));
	if(newNode==NULL) {
		ErrPostEx(SEV_INFO, 1, 0, "RBTree_NewNode: MemNew failed.");
		return(NULL);
	}

	newNode->key=key;
	newNode->data=data;
	newNode->colour=RBTreeRED;	/*all node start off red*/
	newNode->parent=NULL;
	newNode->leftChild=NULL;
	newNode->rightChild=NULL;

	return(newNode);
}

/*free a RBTree node*/
RedBlackTreeNodePtr RBTree_NodeFree(RedBlackTreeNodePtr node, RedBlackTreeFunctionPackagePtr funPack)
{
	if(node==NULL) {
		ErrPostEx(SEV_INFO, 1, 0, "RBTree_NewNode: passed node is null.");
		return(NULL);
	}
	if(funPack==NULL) {
		ErrPostEx(SEV_INFO, 1, 0, "RBTree_NewNode: passed funPack is null.");
		return(NULL);
	}

	node->key=(*(funPack->keyFreeFunc))(node->key);
	node->data=(*(funPack->dataFreeFunc))(node->data);
	node=MemFree(node);

	return(NULL);
}

/*find the node containing the minimum key in the tree*/
RedBlackTreeNodePtr RBTree_MinKeyNode(RedBlackTreeNodePtr treeRoot)
{
	RedBlackTreeNodePtr nodeTmp=NULL;

	if(treeRoot == NULL) {
		ErrPostEx(SEV_ERROR, 1, 0, "RBTree_MinKeyNode: passed tree root is null.");
		return(NULL);
	}

	nodeTmp=treeRoot;
	while((nodeTmp!=NULL)&&(nodeTmp->leftChild != NULL)) {
		nodeTmp = nodeTmp->leftChild;
	}

	return(nodeTmp);
}

/*find the node containing the maximum key in the tree*/
RedBlackTreeNodePtr RBTree_MaxKeyNode(RedBlackTreeNodePtr treeRoot)
{
	RedBlackTreeNodePtr nodeTmp=NULL;

	if(treeRoot == NULL) {
		ErrPostEx(SEV_ERROR, 1, 0, "RBTree_MaxKeyNode: passed tree root is null.");
		return(NULL);
	}

	nodeTmp=treeRoot;
	while((nodeTmp!=NULL)&&(nodeTmp->rightChild != NULL)) {
		nodeTmp = nodeTmp->rightChild;
	}

	return(nodeTmp);
}

/*iterator to find node containing next lowest key in tree from key in node x*/
RedBlackTreeNodePtr RBTree_Predecessor(RedBlackTreeNodePtr x)
{
	RedBlackTreeNodePtr nodeTmp=NULL;

	if(x == NULL) {
		ErrPostEx(SEV_ERROR, 1, 0, "RBTree_Predecessor: passed tree node is null.");
		return(NULL);
	}

	if(x->leftChild != NULL) {
		return(RBTree_MaxKeyNode(x->leftChild));
	}
	nodeTmp=x->parent;
	while((nodeTmp!=NULL)&&(x==nodeTmp->leftChild)) {
		x=nodeTmp;
		nodeTmp=nodeTmp->parent;
	}

	return(nodeTmp);
}

/*iterator to find node containing next highest key in tree from key in node x*/
RedBlackTreeNodePtr RBTree_Successor(RedBlackTreeNodePtr x)
{
	RedBlackTreeNodePtr nodeTmp=NULL;

	if(x == NULL) {
		ErrPostEx(SEV_ERROR, 1, 0, "RBTree_Successor: passed tree node is null.");
		return(NULL);
	}

	if(x->rightChild != NULL) {
		return(RBTree_MinKeyNode(x->rightChild));
	}
	nodeTmp=x->parent;
	while((nodeTmp!=NULL)&&(x==nodeTmp->rightChild)) {
		x=nodeTmp;
		nodeTmp=nodeTmp->parent;
	}

	return(nodeTmp);
}

/*rotate a tree node (x) left*/
Boolean RBTree_RotateLeft(RedBlackTreeNodePtr *treeRoot, RedBlackTreeNodePtr x)
{
	RedBlackTreeNodePtr y=NULL;

	if((treeRoot == NULL)||(*treeRoot == NULL)) {
		ErrPostEx(SEV_ERROR, 1, 0, "RBTree_RotateLeft: passed tree root is null.");
		return(FALSE);
	}
	if(x->rightChild == NULL) {
		ErrPostEx(SEV_ERROR, 1, 0, "RBTree_RotateLeft: passed right child of x is null.");
		return(FALSE);
	}

	/*set y*/
	y=x->rightChild;
	/*turn y's left subtree into x's right subtree*/
	x->rightChild = y->leftChild;
	if(y->leftChild != NULL)
		y->leftChild->parent = x;
	y->parent = x->parent;
	if(x->parent==NULL) {
		(*treeRoot)=y;
	}
	else if(x==x->parent->leftChild) {
		x->parent->leftChild = y;
	}
	else {
		x->parent->rightChild = y;
	}
	y->leftChild = x;
	x->parent = y;

	return(TRUE);
}

/*roatate a tree node (y) right*/
Boolean RBTree_RotateRight(RedBlackTreeNodePtr *treeRoot, RedBlackTreeNodePtr y)
{
	RedBlackTreeNodePtr x=NULL;

	if((treeRoot == NULL)||(*treeRoot == NULL)) {
		ErrPostEx(SEV_ERROR, 1, 0, "RBTree_RotateRight: passed tree root is null.");
		return(FALSE);
	}
	if(y->leftChild == NULL) {
		ErrPostEx(SEV_ERROR, 1, 0, "RBTree_RotateRight: passed left child of y is null.");
		return(FALSE);
	}

	/*set x*/
	x=y->leftChild;
	/*turn x's left subtree into y's right subtree*/
	y->leftChild = x->rightChild;
	if(x->rightChild != NULL) {
		x->rightChild->parent = y;
	}
	x->parent = y->parent;
	if(y->parent==NULL) {
		(*treeRoot)=x;
	}
	else if(y==y->parent->leftChild) {
		y->parent->leftChild = x;
	}
	else {
		y->parent->rightChild = x;
	}
	x->rightChild = y;
	y->parent = x;

	return(TRUE);
}

/*find a node in the tree, by given key*/
RedBlackTreeNodePtr RBTree_Find(RedBlackTreeNodePtr treeRoot, Pointer key, RedBlackTreeFunctionPackagePtr funPack)
{
	RedBlackTreeNodePtr nodeTmp=NULL;

	if(key==NULL) {
		ErrPostEx(SEV_INFO, 1, 0, "RBTree_Find: passed key is null.");
		return(NULL);
	}
	if(treeRoot==NULL) {
		ErrPostEx(SEV_INFO, 1, 0, "RBTree_Find: passed treeRoot is null.");
		return(NULL);
	}
	if(funPack==NULL) {
		ErrPostEx(SEV_INFO, 1, 0, "RBTree_Find: passed funPack is null.");
		return(NULL);
	}

	nodeTmp = treeRoot;
	while((nodeTmp != NULL)&&((*(funPack->compareFunc))(key, nodeTmp->key)!=0)) {
		if((*(funPack->compareFunc))(key, nodeTmp->key)<0) {
			nodeTmp = nodeTmp->leftChild;
		}
		else {
			nodeTmp = nodeTmp->rightChild;
		}
	}

	return(nodeTmp);
}

/*add a node to the tree*/
Boolean RBTree_Add(RedBlackTreeNodePtr *treeRoot, Pointer key, Pointer data, RedBlackTreeFunctionPackagePtr funPack)
{
	RedBlackTreeNodePtr x=NULL, y=NULL, newNode=NULL;

	if(key==NULL) {
		ErrPostEx(SEV_INFO, 1, 0, "RBTree_Add: passed key is null.");
		return(FALSE);
	}
	/*data is optional*/
	if(funPack==NULL) {
		ErrPostEx(SEV_INFO, 1, 0, "RBTree_Add: passed funPack is null.");
		return(FALSE);
	}

	/*insert the node in the binary tree*/
	x=(*treeRoot);
	while(x != NULL) {
		y=x;
		if((*(funPack->compareFunc))(key, x->key)<0) {
			x = x->leftChild;
		}
		else {
			x = x->rightChild;
		}
	}
	/*y is now a leaf - can be added to*/
	newNode = RBTree_NodeNew(key, data);
	newNode->parent = y;
	/*deal with treeRoot==NULL and treeRoot is only node cases*/
	if(y==NULL) {
		*treeRoot=newNode;
	}
	else if((*(funPack->compareFunc))(key, y->key)<0) {
		y->leftChild=newNode;
	}
	else {
		y->rightChild=newNode;
	}
	/*fix up modified tree*/
	while((newNode != (*treeRoot))&&(newNode->parent->parent != NULL)&&(newNode->parent->colour==RBTreeRED)) {
		if(newNode->parent == newNode->parent->parent->leftChild) {
			y=newNode->parent->parent->rightChild;
			if((y!=NULL)&&(y->colour==RBTreeRED)) {
				/*case 1*/
				newNode->parent->colour = RBTreeBLACK;
				y->colour = RBTreeBLACK;
				newNode->parent->parent->colour = RBTreeRED;
				newNode = newNode->parent->parent;
			}
			else {
				if(newNode==newNode->parent->rightChild) {
					/*transform so case 3 applies*/
					newNode=newNode->parent;
					RBTree_RotateLeft(treeRoot, newNode);
				}
				/*case 3*/
				newNode->parent->colour=RBTreeBLACK;
				if(newNode->parent->parent != NULL) {
					newNode->parent->parent->colour = RBTreeRED;
					RBTree_RotateRight(treeRoot, newNode->parent->parent);
				}
				/*the while loop will terminate on the next iteration*/
			}
		}
		else {
			/*right child - the following is symmetric to the above cases*/
			y=newNode->parent->parent->leftChild;
			if((y!=NULL)&&(y->colour==RBTreeRED)) {
				newNode->parent->colour = RBTreeBLACK;
				y->colour = RBTreeBLACK;
				newNode->parent->parent->colour = RBTreeRED;
				newNode = newNode->parent->parent;
			}
			else {
				if(newNode==newNode->parent->leftChild) {
					newNode = newNode->parent;
					RBTree_RotateRight(treeRoot, newNode);
				}
				newNode->parent->colour=RBTreeBLACK;
				if(newNode->parent->parent != NULL) {
					newNode->parent->parent->colour = RBTreeRED;
					RBTree_RotateLeft(treeRoot, newNode->parent->parent);
				}
			}
		}
	}

	(*treeRoot)->colour = RBTreeBLACK;
	return(TRUE);
}

/*delete fixup*/
Boolean RBTree_DelFixup(RedBlackTreeNodePtr *treeRoot, RedBlackTreeNodePtr x, RedBlackTreeNodePtr p)
{
	RedBlackTreeNodePtr w=NULL;

	if((treeRoot == NULL)||(*treeRoot == NULL)) {
		ErrPostEx(SEV_INFO, 1, 0, "RBTree_DelFixup: passed treeRoot is null.");
		return(FALSE);
	}
	if(x==NULL) {
		ErrPostEx(SEV_INFO, 1, 0, "RBTree_DelFixup: passed x is null.");
		return(FALSE);
	}
	if((p==NULL)&&(p!=(*treeRoot))) {
		ErrPostEx(SEV_INFO, 1, 0, "RBTree_DelFixup: passed p is null and is not the root.");
		return(FALSE);
	}

	while((x != (*treeRoot))&&(x==NULL || x->colour==RBTreeBLACK)) {
		if(x==p->leftChild) {
			w=p->rightChild;
			if(w==NULL) {
				return(TRUE);
			}
			if(w->colour==RBTreeRED) {
				w->colour=RBTreeBLACK;
				p->colour=RBTreeRED;
				RBTree_RotateLeft(treeRoot, p);
				w=p->rightChild;
				if(w==NULL) {
					return(TRUE);
				}
			}
			if(((w->leftChild==NULL)||(w->leftChild->colour==RBTreeBLACK))&&
				((w->rightChild==NULL)||(w->rightChild->colour==RBTreeBLACK))) {
				w->colour=RBTreeRED;
				x=p;
				/*maintain p as x->parent*/
				p=p->parent;
				continue;
			}
			else if((w->rightChild==NULL)||(w->rightChild->colour==RBTreeBLACK)) {
				w->leftChild->colour=RBTreeBLACK;
				w->colour = RBTreeRED;
				RBTree_RotateRight(treeRoot, w);
				w=p->rightChild;
				if(w==NULL) {
					return(TRUE);
				}
			}
			w->colour=p->colour;
			p->colour=RBTreeBLACK;
			w->rightChild->colour=RBTreeBLACK;
			RBTree_RotateLeft(treeRoot, p);
			x=(*treeRoot);
		}
		else {
			if(p==NULL) {
				return(TRUE);
			}
			w=p->leftChild;
			if(w==NULL) {
				return(TRUE);
			}
			if(w->colour==RBTreeRED) {
				w->colour = RBTreeBLACK;
				p->colour = RBTreeRED;
				RBTree_RotateRight(treeRoot, p);
				w=p->leftChild;
				if(w==NULL) {
					return(TRUE);
				}
			}
			if( ((w->rightChild==NULL)||(w->rightChild->colour==RBTreeBLACK)) &&
				((w->leftChild==NULL)||(w->leftChild->colour==RBTreeBLACK)) ) {
				w->colour=RBTreeRED;
				x=p;
				/*maintain p as x->parent*/
				p=p->parent;
				continue;
			}
			else if((w->leftChild==NULL)||(w->leftChild->colour==RBTreeBLACK)) {
				w->rightChild->colour=RBTreeBLACK;
				w->colour = RBTreeRED;
				RBTree_RotateLeft(treeRoot, w);
				w=p->leftChild;
				if(w==NULL) {
					return(TRUE);
				}
			}
			w->colour=p->colour;
			p->colour=RBTreeBLACK;
			w->leftChild->colour=RBTreeBLACK;
			RBTree_RotateRight(treeRoot, p);
			x=(*treeRoot);
		}
	}
	if(x!=NULL) {
		x->colour=RBTreeBLACK;
	}

	return(TRUE);
}

/*remove a node from the tree*/
Boolean RBTree_Del(RedBlackTreeNodePtr *treeRoot, Pointer key, RedBlackTreeFunctionPackagePtr funPack)
{
	RedBlackTreeNodePtr x=NULL, y=NULL, z=NULL;

	if((treeRoot == NULL)||(*treeRoot == NULL)) {
		ErrPostEx(SEV_INFO, 1, 0, "RBTree_Del: passed treeRoot is null.");
		return(TRUE);
	}
	if(key==NULL) {
		ErrPostEx(SEV_INFO, 1, 0, "RBTree_Del: passed key is null.");
		return(FALSE);
	}
	if(funPack==NULL) {
		ErrPostEx(SEV_INFO, 1, 0, "RBTree_Del: passed funPack is null.");
		return(FALSE);
	}

	/*find node*/
	z = RBTree_Find(*treeRoot, key, funPack);
	if(z==NULL) {
		return(TRUE);	/*key is not present in tree*/
	}

	if((z->leftChild==NULL)||(z->rightChild==NULL)) {
		y=z;
	}
	else {
		y = RBTree_Successor(z);
	}

	if(y->leftChild != NULL) {
		x=y->leftChild;
	}
	else {
		x=y->rightChild;
	}

	if(x!=NULL) {
		x->parent=y->parent;
	}

	/*splice out x*/
	if(y->parent != NULL) {
		if(y==y->parent->leftChild) {
			y->parent->leftChild=x;
		}
		else {
			y->parent->rightChild=x;
		}
	}
	else {
		(*treeRoot)=x;
	}

	if(y!=z) {
		/*move all values over to z*/
		z->key=y->key;
		y->key=NULL;
		z->data=y->data;
		y->data=NULL;
	}

	if(y->colour==RBTreeBLACK) {
		RBTree_DelFixup(treeRoot, x, y->parent);
	}

	y=RBTree_NodeFree(y, funPack);

	return(TRUE);
}

/*free a Red-black tree*/
RedBlackTreeNodePtr RBTree_Free(RedBlackTreeNodePtr treeRoot, RedBlackTreeFunctionPackagePtr funPack)
{
	if(treeRoot==NULL) {
		return(NULL);
	}
	if(funPack==NULL) {
		ErrPostEx(SEV_INFO, 1, 0, "RBTree_Free: passed funPack is null.");
		return(NULL);
	}

	/*post order traversal and free*/
	treeRoot->leftChild = RBTree_Free(treeRoot->leftChild, funPack);
	treeRoot->rightChild = RBTree_Free(treeRoot->rightChild, funPack);
	treeRoot=RBTree_NodeFree(treeRoot, funPack);

	return(NULL);
}

/*higher level functions*/

/*Use RedBlackTreeRootPtr at top level for increased convenience*/

/*Initialize a rbtree root*/
RedBlackTreeRootPtr RBTree_Root_Init(Uint2 keyType, Uint2 dataType)
{
	RedBlackTreeRootPtr rbtrp=NULL;

	rbtrp = MemNew(sizeof(RedBlackTreeRoot));
	if(rbtrp==NULL) {
		ErrPostEx(SEV_ERROR, 1, 0, "RBTree_Root_Init: MemNew failed.");
		return(NULL);
	}

	rbtrp->funPack = RBTree_FunPack_Init(keyType, dataType);

	return(rbtrp);
}

/*free rbtree root*/
RedBlackTreeRootPtr RBTree_Root_Free(RedBlackTreeRootPtr rbtrp)
{
	rbtrp->treeRoot = RBTree_Free(rbtrp->treeRoot, rbtrp->funPack);
	rbtrp->funPack = RBTree_FunPack_Free(rbtrp->funPack);
	rbtrp = MemFree(rbtrp);

	return(NULL);
}

/*Helper functions*/

/*Copies an integer to new memory - useful when your data type is an Int4*/
Int4Ptr RBTree_CopyInt4(Int4 data)
{
	Int4Ptr intCopy;

	intCopy = MemNew((size_t) sizeof(Int4));
	if(intCopy==NULL) {
		ErrPostEx(SEV_ERROR, 1, 0, "RBTree_CopyInt4: MemNew failed.");
		return(NULL);
	}
	*intCopy = data;

	return(intCopy);
}

/*integer key functions*/

/*find a node by integer key*/
RedBlackTreeNodePtr RBTree_Root_FindInt(RedBlackTreeRootPtr treeRoot, Int4 key)
{
	return(RBTree_FindInt(treeRoot->treeRoot, key, treeRoot->funPack));
}

/*add a node given an integer key and a pointer to data that will be linked in the tree*/
/*The data that you allocate becomes part of the tree, so don't free it after you add to the tree.*/
Boolean RBTree_Root_AddInt(RedBlackTreeRootPtr treeRoot, Int4 key, Pointer data)
{
	return(RBTree_AddInt(&(treeRoot->treeRoot), key, data, treeRoot->funPack));
}

/*delete a node given an integer key*/
Boolean RBTree_Root_DelInt(RedBlackTreeRootPtr treeRoot, Int4 key)
{
	return(RBTree_DelInt(&(treeRoot->treeRoot), key, treeRoot->funPack));
}

/*string key functions*/

/*find a node by string key*/
RedBlackTreeNodePtr RBTree_Root_FindString(RedBlackTreeRootPtr treeRoot, CharPtr key)
{
	return(RBTree_FindString(treeRoot->treeRoot, key, treeRoot->funPack));
}

/*add a node given a string key and a pointer to data that will be linked in the tree*/
/*The data that you allocate becomes part of the tree, so don't free it after you add to the tree.*/
Boolean RBTree_Root_AddString(RedBlackTreeRootPtr treeRoot, CharPtr key, Pointer data)
{
	return(RBTree_AddString(&(treeRoot->treeRoot), key, data, treeRoot->funPack));
}

/*delete a node given a string key*/
Boolean RBTree_Root_DelString(RedBlackTreeRootPtr treeRoot, CharPtr key)
{
	return(RBTree_DelString(&(treeRoot->treeRoot), key, treeRoot->funPack));
}

/*FloatHi key functions*/

/*find a node by FloatHi key*/
RedBlackTreeNodePtr RBTree_Root_FindFloatHi(RedBlackTreeRootPtr treeRoot, FloatHi key)
{
	return(RBTree_Find(treeRoot->treeRoot, (Pointer) &key, treeRoot->funPack));
}

/*add a node given a FloatHi key and a pointer to data that will be linked in the tree*/
/*The data that you allocate becomes part of the tree, so don't free it after you add to the tree.*/
Boolean RBTree_Root_AddFloatHi(RedBlackTreeRootPtr treeRoot, FloatHi key, Pointer data)
{
	FloatHiPtr pKeyCopy=NULL;

	/*copy key to add to tree*/
	pKeyCopy = MemNew((size_t) sizeof(FloatHi));
	if(pKeyCopy==NULL) {
		ErrPostEx(SEV_ERROR, 1, 0, "RBTree_Root_AddFloatHi: MemNew pKeyCopy failed.");
		return(FALSE);
	}
	*pKeyCopy = key;

	return(RBTree_Add(&(treeRoot->treeRoot), (Pointer) pKeyCopy, data, treeRoot->funPack));
}

/*delete a node given a FloatHi key*/
Boolean RBTree_Root_DelFloatHi(RedBlackTreeRootPtr treeRoot, FloatHi key)
{
	return(RBTree_Del(&(treeRoot->treeRoot), &key, treeRoot->funPack));
}


/*-----------Below are old functions, please use above-------------*/

/*initialize a function package based on key and data type*/
RedBlackTreeFunctionPackagePtr RBTree_FunPack_Init(Uint2 keyType, Uint2 dataType)
{
	RedBlackTreeFunctionPackagePtr funPack=NULL;
	RBTree_CompareFunc compareFunc=NULL;
	RBTree_KeyFreeFunc keyFreeFunc=NULL;
	RBTree_DataFreeFunc dataFreeFunc=NULL;

	switch(keyType) {
	case RBTreeKeyInt:
		compareFunc = RBTree_CompareIntKeyFunction;
		keyFreeFunc = RBTree_IntKeyFree;
		break;
	case RBTreeKeyString:
		compareFunc = RBTree_CompareStringKeyFunction;
		keyFreeFunc = RBTree_StringKeyFree;
		break;
	case RBTreeKeyFloatHi:
		compareFunc = RBTree_CompareFloatHiKeyFunction;
		keyFreeFunc = RBTree_FloatHiKeyFree;
		break;
	default:
		ErrPostEx(SEV_ERROR, 1, 0, "RBTree_UserInitialize: Passed key type (%ld) is not supported.", (long) keyType);
		return(NULL);
	}

	switch(dataType) {
	case RBTreeDataInt:
		dataFreeFunc = RBTree_IntDataFree;
		break;
	case RBTreeDataString:
		dataFreeFunc = RBTree_StringDataFree;
		break;
	case RBTreeDataValNodeList:
		dataFreeFunc = RBTree_VnpDataFree;
		break;
	case RBTreeDataFloatHi:
		dataFreeFunc = RBTree_FloatHiDataFree;
		break;
	case RBTreeDataUserDefinedNoFree:
		dataFreeFunc = RBTree_DummyDataFree;
		break;
	case RBTreeDataMemFree:
		dataFreeFunc = RBTree_DataMemFree;
		break;
	default:
		ErrPostEx(SEV_ERROR, 1, 0, "RBTree_UserInitialize: Passed data type (%ld) is not supported.", (long) dataType);
		return(NULL);
	}

	funPack = RBTree_FunPack_New((RBTree_CompareFunc) compareFunc,
		(RBTree_KeyFreeFunc) keyFreeFunc, (RBTree_DataFreeFunc) dataFreeFunc);
	funPack->keyType = keyType;
	funPack->dataType = dataType;

	return(funPack);
}

/*integer key functions*/

/*find a node by integer key*/
RedBlackTreeNodePtr RBTree_FindInt(RedBlackTreeNodePtr treeRoot, Int4 key, RedBlackTreeFunctionPackagePtr funPack)
{
	return(RBTree_Find(treeRoot, (Pointer) &key, funPack));
}

/*add a node given an integer key and a pointer to data that will be linked in the tree*/
/*The data that you allocate becomes part of the tree, so don't free it after you add to the tree.*/
Boolean RBTree_AddInt(RedBlackTreeNodePtr *treeRoot, Int4 key, Pointer data, RedBlackTreeFunctionPackagePtr funPack)
{
	Int4Ptr pKeyCopy=NULL;

	/*copy key to add to tree*/
	pKeyCopy = MemNew((size_t) sizeof(Int4));
	if(pKeyCopy==NULL) {
		ErrPostEx(SEV_ERROR, 1, 0, "RBTree_AddInt: MemNew pKeyCopy failed.");
		return(FALSE);
	}
	*pKeyCopy = key;

	return(RBTree_Add(treeRoot, (Pointer) pKeyCopy, data, funPack));
}

/*delete a node given an integer key*/
Boolean RBTree_DelInt(RedBlackTreeNodePtr *treeRoot, Int4 key, RedBlackTreeFunctionPackagePtr funPack)
{
	return(RBTree_Del(treeRoot, &key, funPack));
}

/*string key functions*/

/*find a node by string key*/
RedBlackTreeNodePtr RBTree_FindString(RedBlackTreeNodePtr treeRoot, CharPtr key, RedBlackTreeFunctionPackagePtr funPack)
{
	if(key==NULL) {
		ErrPostEx(SEV_ERROR, 1, 0, "RBTree_FindString: Passed key was null.");
		return(NULL);
	}

	return(RBTree_Find(treeRoot, (Pointer) key, funPack));
}

/*add a node given a string key and a pointer to data that will be linked in the tree*/
/*The data that you allocate becomes part of the tree, so don't free it after you add to the tree.*/
Boolean RBTree_AddString(RedBlackTreeNodePtr *treeRoot, CharPtr key, Pointer data, RedBlackTreeFunctionPackagePtr funPack)
{
	return(RBTree_Add(treeRoot, StringSave(key), data, funPack));
}

/*delete a node given a string key*/
Boolean RBTree_DelString(RedBlackTreeNodePtr *treeRoot, CharPtr key, RedBlackTreeFunctionPackagePtr funPack)
{
	return(RBTree_Del(treeRoot, key, funPack));
}



/*Implementation test area*/

/*check that the red black tree properties hold true*/
Boolean RBTree_CheckInvariant(RedBlackTreeNodePtr root, RedBlackTreeFunctionPackagePtr funPack)
{
	static Int4 blackHeight;
	Int4 bh=0;
	RedBlackTreeNodePtr nodeTmp=NULL;

	if(root->parent==NULL) {
		/*initialize black height when we see the real root node*/
		blackHeight = -1;
	}

	/*check binary tree properties*/ 
	if(root->leftChild != NULL) {
		if((*(funPack->compareFunc))(root->key, root->leftChild->key)<0) {
			return(FALSE);
		}
	}
	if(root->rightChild != NULL) {
		if((*(funPack->compareFunc))(root->rightChild->key, root->key)<0) {
			return(FALSE);
		}
	}

	/*check red-black tree properties*/
	/*if a node is red, both children must be black - NULL nodes are black*/
	if(root->colour==RBTreeRED) {
		if(((root->leftChild!=NULL)&&(root->leftChild->colour != RBTreeBLACK)) ||
			((root->rightChild!=NULL)&&(root->rightChild->colour != RBTreeBLACK))) {
			return(FALSE);
		}
	}

	/*the black heights of all leaf node are equal*/
	if((root->leftChild==NULL)&&(root->rightChild==NULL)) {
		/*compute black height of node*/
		for(nodeTmp=root;nodeTmp!=NULL;nodeTmp=nodeTmp->parent) {
			if(nodeTmp->colour==RBTreeBLACK) {
				bh++;
			}
		}

		if(blackHeight == -1) {
			blackHeight = bh;
		}
		else {
			if(bh != blackHeight) {
				return(FALSE);
			}
		}
	}

	if((root->leftChild!=NULL)&&(!RBTree_CheckInvariant(root->leftChild, funPack))) {
		return(FALSE);
	}
	if((root->rightChild!=NULL)&&(!RBTree_CheckInvariant(root->rightChild, funPack))) {
		return(FALSE);
	}

	return(TRUE);
}

/*test the Red-Black tree implementation*/
void RBTree_Test(void)
{
	RedBlackTreeFunctionPackagePtr funPack=NULL;
	RedBlackTreeNodePtr treeRoot=NULL, nodeTmp=NULL;
	Boolean ret=FALSE;
	Int4 i=0, key=0, nodesToAdd=100000;
	Int4Ptr keyp=NULL, datap=NULL;
	clock_t start, finish;

#define RBTree_DEBUG_Print	/*define to output print statements*/

	/*test red-black tree*/
	funPack = RBTree_FunPack_New((RBTree_CompareFunc) RBTree_CompareIntKeyFunction,
		(RBTree_KeyFreeFunc) RBTree_IntKeyFree, (RBTree_DataFreeFunc) RBTree_IntDataFree);

#ifdef RBTree_DEBUG_Print
	printf("Testing Red-Black tree...\n");
#endif

	/*add some things*/
#ifdef RBTree_DEBUG_Print
	printf("Adding keys 0 to 14, data equals key*key\n");
#endif
	for(i=0;i<15;i++) {
		keyp = MemNew(sizeof(Int4));
		*keyp = i;
		datap = MemNew(sizeof(Int4));
		*datap = i*i;
		ret = RBTree_Add(&treeRoot, (Pointer) keyp, (Pointer) datap, funPack);
#ifdef RBTree_DEBUG_Print
		printf("RBTree_Add: key=%ld, data=%ld, ret=%ld\n", (long) *keyp, (long) *datap, (long) ret);
#endif
	}

	key=14;
	nodeTmp = RBTree_Find(treeRoot, &key, funPack);
#ifdef RBTree_DEBUG_Print
	printf("RBTree_Find: tried to find %ld, found %ld\n", (long) key, (long) *((Int4Ptr)nodeTmp->key));
#endif

	/*print out what's in tree*/
#ifdef RBTree_DEBUG_Print
	printf("Printing out what's in the tree forwards.\n");
#endif
	nodeTmp = RBTree_MinKeyNode(treeRoot);
#ifdef RBTree_DEBUG_Print
	printf("RBTree_MinKeyNode: min key=%ld\n", (long) *((Int4Ptr)nodeTmp->key));
	printf("RBTree_MinKeyNode: min key data=%ld\n", (long) *((Int4Ptr)nodeTmp->data));
#endif
	while((nodeTmp=RBTree_Successor(nodeTmp)) != NULL) {
#ifdef RBTree_DEBUG_Print
		printf("RBTree_Successor: next key=%ld\n", (long) *((Int4Ptr)nodeTmp->key));
		printf("RBTree_Successor: next key data=%ld\n", (long) *((Int4Ptr)nodeTmp->data));
#endif
	}

	/*print out tree backwards*/
#ifdef RBTree_DEBUG_Print
	printf("Printing out what's in the tree backwards.\n");
#endif
	nodeTmp = RBTree_MaxKeyNode(treeRoot);
#ifdef RBTree_DEBUG_Print
	printf("RBTree_MaxKeyNode: max key=%ld\n", (long) *((Int4Ptr)nodeTmp->key));
	printf("RBTree_MaxKeyNode: max key data=%ld\n", (long) *((Int4Ptr)nodeTmp->data));
#endif
	while((nodeTmp=RBTree_Predecessor(nodeTmp)) != NULL) {
#ifdef RBTree_DEBUG_Print
		printf("RBTree_Predecessor: previous key=%ld\n", (long) *((Int4Ptr)nodeTmp->key));
		printf("RBTree_Predecessor: previous key data=%ld\n", (long) *((Int4Ptr)nodeTmp->data));
#endif
	}

	/*delete some things*/
#ifdef RBTree_DEBUG_Print
	printf("Testing deletion...\n");
#endif
	key=3;
	ret=RBTree_Del(&treeRoot, (Pointer) &key, funPack);
#ifdef RBTree_DEBUG_Print
	printf("RBTree_Del: del key=%ld, ret=%ld\n", (long) key, (long) ret);
#endif
	key=7;
	ret=RBTree_Del(&treeRoot, (Pointer) &key, funPack);
#ifdef RBTree_DEBUG_Print
	printf("RBTree_Del: del key=%ld, ret=%ld\n", (long) key, (long) ret);
#endif
	key=12;
	ret=RBTree_Del(&treeRoot, (Pointer) &key, funPack);
#ifdef RBTree_DEBUG_Print
	printf("RBTree_Del: del key=%ld, ret=%ld\n", (long) key, (long) ret);
#endif
	key=25;
	ret=RBTree_Del(&treeRoot, (Pointer) &key, funPack);
#ifdef RBTree_DEBUG_Print
	printf("RBTree_Del: del key=%ld, ret=%ld\n", (long) key, (long) ret);
#endif

	/*print out what's in tree*/
	nodeTmp = RBTree_MinKeyNode(treeRoot);
#ifdef RBTree_DEBUG_Print
	printf("RBTree_MinKeyNode: min key=%ld\n", (long) *((Int4Ptr)nodeTmp->key));
	printf("RBTree_MinKeyNode: min key data=%ld\n", (long) *((Int4Ptr)nodeTmp->data));
#endif
	while((nodeTmp=RBTree_Successor(nodeTmp)) != NULL) {
#ifdef RBTree_DEBUG_Print
		printf("RBTree_Successor: next key=%ld\n", (long) *((Int4Ptr)nodeTmp->key));
		printf("RBTree_Successor: next key data=%ld\n", (long) *((Int4Ptr)nodeTmp->data));
#endif
	}

	/*add some things back to the tree*/
#ifdef RBTree_DEBUG_Print
	printf("Testing add after deletion...\n");
#endif
	keyp = MemNew(sizeof(Int4));
	*keyp = 3;
	datap = MemNew(sizeof(Int4));
	*datap = 3*3;
	ret = RBTree_Add(&treeRoot, (Pointer) keyp, (Pointer) datap, funPack);
#ifdef RBTree_DEBUG_Print
	printf("RBTree_Add: key=%ld, data=%ld, ret=%ld\n", (long) *keyp, (long) *datap, (long) ret);
#endif

	/*print out what's in tree*/
	nodeTmp = RBTree_MinKeyNode(treeRoot);
#ifdef RBTree_DEBUG_Print
	printf("RBTree_MinKeyNode: min key=%ld\n", (long) *((Int4Ptr)nodeTmp->key));
	printf("RBTree_MinKeyNode: min key data=%ld\n", (long) *((Int4Ptr)nodeTmp->data));
#endif
	while((nodeTmp=RBTree_Successor(nodeTmp)) != NULL) {
#ifdef RBTree_DEBUG_Print
		printf("RBTree_Successor: next key=%ld\n", (long) *((Int4Ptr)nodeTmp->key));
		printf("RBTree_Successor: next key data=%ld\n", (long) *((Int4Ptr)nodeTmp->data));
#endif
	}

	treeRoot = RBTree_Free(treeRoot, funPack);

	/*test for memory leaks and speed*/
#ifdef RBTree_DEBUG_Print
	printf("Testing speed and memory leaks...\n");
	printf("Adding %ld nodes to the tree sequentially.\n", (long) nodesToAdd);
	start = clock();
#endif
	for(i=0;i<nodesToAdd;i++) {
		keyp = MemNew(sizeof(Int4));
		*keyp = i;
		datap = MemNew(sizeof(Int4));
		*datap = i*i;
		ret = RBTree_Add(&treeRoot, (Pointer) keyp, (Pointer) datap, funPack);
	}
#ifdef RBTree_DEBUG_Print
	finish = clock();
	printf( "Adding %ld nodes took %f seconds.\n", (long) nodesToAdd, (double)(finish-start)/CLOCKS_PER_SEC);
#endif

	/*test sequential delete*/
#ifdef RBTree_DEBUG_Print
	printf("Deleting %ld nodes from the tree sequentially.\n", (long) nodesToAdd);
	start = clock();
#endif
	for(i=(nodesToAdd-1);i>=0;i--) {
		ret=RBTree_Del(&treeRoot, (Pointer) &i, funPack);
	}
#ifdef RBTree_DEBUG_Print
	finish = clock();
	printf( "Deleting %ld nodes took %f seconds.\n", (long) nodesToAdd, (double)(finish-start)/CLOCKS_PER_SEC);
#endif

	treeRoot = RBTree_Free(treeRoot, funPack);
	funPack = RBTree_FunPack_Free(funPack);

#ifdef RBTree_DEBUG_Print
	printf("Reb-Black tree testing done.\n");
#endif

#ifdef RBTree_DEBUG_Print
#undef RBTree_DEBUG_Print
#endif

	return;
}

/*
$Log: slri_rbtree.c,v $
Revision 1.1.1.3  2002/01/16 09:23:28  root
Automated import from Sourceforge.net

Revision 1.3  2002/01/16 03:31:05  gbader
Added more data type options for top level RB tree functions

Revision 1.2  2002/01/12 17:11:57  gbader
Added option ot save data and not free it on top level tree functions.

Revision 1.1.1.2  2001/10/10 22:11:04  kaca
initial import

Revision 1.8  2001/08/02 03:24:09  gbader
Added functionality for FloatHi data types in higher level tree functions

Revision 1.7  2001/07/03 18:54:17  gbader
Added more higher level rbtree functions

Revision 1.6  2001/01/13 04:04:10  gbader
Added IBM DB2 UDB to license exceptions.

Revision 1.5  2000/09/22 21:30:55  gbader
Code clean up from -Wall compiler flag on Linux

Revision 1.4  2000/09/09 17:46:39  gbader
Added (long) type casts for all Int4 variables used with ANSI format string functions (e.g. printf) to avoid -Wall warnings on Linux

Revision 1.3  2000/09/05 13:55:08  gbader
Updated License so that it is now GNU.
Added CVS keywords ID and Log as well as new line at end of files so compilers on some platforms don't complain

*/
