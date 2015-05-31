/* $Id: slri_dagtree.c,v 1.5 2004/02/26 20:21:21 mjdumont Exp $
-- ***************************************************************************
--
--  Samuel Lunenfeld Research Institute (SLRI)
--
--  File: slri_dagtree.c
--  Description: Directed Acyclic Graph implementation
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
#include <slri_misc.h>
#include "slri_dagtree.h"




/************************/
/* Function definitions */
/************************/

/****************************/
/* DAG Tree functions       */
/****************************/

/*general utility functions*/
ValNodePtr DAGTree_SaveInt(Int4 num)
{
	return(ValNodeAddInt(NULL,0,num));
}
ValNodePtr DAGTree_SaveString(CharPtr str)
{
	return(ValNodeCopyStr(NULL,0,str));
}
ValNodePtr DAGTree_SaveFloatHi(FloatHi num)
{
	return(ValNodeAddFloat(NULL,0,num));
}
ValNodePtr DAGTree_SaveBoolean(Boolean b)
{
	return(ValNodeAddBoolean(NULL,0,b));
}


/*general compare and free functions*/
/*compare integer keys*/
Int4 DAGTree_CompareInt(ValNodePtr key1, ValNodePtr key2)
{
	return((key1->data.intvalue - key2->data.intvalue));
}

/*compare string keys*/
Int4 DAGTree_CompareString(ValNodePtr key1, ValNodePtr key2)
{
	return ((Int4) StringCmp((CharPtr) key1->data.ptrvalue, (CharPtr) key2->data.ptrvalue));
}

/*compare real (FloatHi) keys*/
Int4 DAGTree_CompareFloatHi(ValNodePtr key1, ValNodePtr key2)
{
	FloatHi diff = key1->data.realvalue - key2->data.realvalue;
	if(diff>0) return(1);
	else if(diff<0) return(-1);
	return((Int4) diff);	
}

void DAGTree_PrintInt(ValNodePtr value, FILE *stream)
{
	while(value) {
		fprintf(stream,"%ld", (long) value->data.intvalue);
		value=value->next;
	}
}
void DAGTree_PrintString(ValNodePtr value, FILE *stream)
{
	while(value) {
		fprintf(stream,"%s",IsNullStr((char *) value->data.ptrvalue));
		value = value->next;
	}

}
void DAGTree_PrintFloatHi(ValNodePtr value, FILE *stream)
{
	while(value) {
		fprintf(stream,"%f", (float) value->data.realvalue);
		value=value->next;
	}
}


const DNodePtr DAGTree_GetFirstLeaf(DNodePtr node)
{
	CharPtr method="DAGTree_GetFirstLeaf";
	if(node == NULL) {
		ErrPostEx(SEV_ERROR,0,0,"%s: node is NULL!",method);
		return NULL;
	}
	if(node->c) node = DAGTree_GetFirstLeaf(node->c);
	return node;
}

void DAGTree_GetAllLeafs(ValNodePtr *nodes, DNodePtr node)
{
	CharPtr method="DAGTree_GetAllLeafs";
	DNodePtr cnodes = NULL;

	if(nodes == NULL) {
		ErrPostEx(SEV_ERROR,0,0,"%s: nodes reference pointer is NULL!",method);
		return;
	}
	if(node == NULL) {
		ErrPostEx(SEV_ERROR,0,0,"%s: node is NULL!",method);
		return;
	}
	cnodes = node->c;
	if(cnodes) {
		while(cnodes) {
			DAGTree_GetAllLeafs(&(*nodes), cnodes);
			cnodes=cnodes->next;
		}
	} else {
		ValNodeAddPointer(&(*nodes), 0, node);
	}
}




/*free ValNodeList data*/
ValNodePtr DAGTree_FreeVnp(ValNodePtr data)
{
	ValNodePtr datap = (ValNodePtr) data;
	return(ValNodeFree(datap));
}

/*free ValNodeList data*/
ValNodePtr DAGTree_FreeVnpData(ValNodePtr data)
{
	ValNodePtr datap = (ValNodePtr) data;
	return(ValNodeFreeData(datap));
}

/*Dummy function to not free any data*/
ValNodePtr DAGTree_FreeDummyData(ValNodePtr data)
{
	return(NULL);
}

/*free data using MemFree*/
ValNodePtr DAGTree_MemFreeData(ValNodePtr data)
{
	return(MemFree(data));
}


/*create a function package for the DAG tree*/
DTreeFunctionPackagePtr DAGTree_FunPack_New(DAGTree_KeyCompareFunc keyCompareFunc, 
											  DAGTree_KeyPrintFunc keyPrintFunc,
											  DAGTree_KeyFreeFunc keyFreeFunc,
											  DAGTree_DataPrintFunc dataPrintFunc,
											  DAGTree_DataFreeFunc dataFreeFunc)
{
	DTreeFunctionPackagePtr pFunPack=NULL;

	if(keyCompareFunc==NULL) {
		ErrPostEx(SEV_INFO, 1, 0, "DAGTree_FunPack_New: passed compareFunc is null.");
		return(NULL);
	}
	if(keyFreeFunc==NULL) {
		ErrPostEx(SEV_INFO, 1, 0, "DAGTree_FunPack_New: passed keyFreeFunc is null.");
		return(NULL);
	}
	if(dataFreeFunc==NULL) {
		ErrPostEx(SEV_INFO, 1, 0, "DAGTree_FunPack_New: passed dataFreeFunc is null.");
		return(NULL);
	}

	pFunPack = MemNew((size_t) sizeof(DTreeFunctionPackage));
	if(pFunPack==NULL) {
		ErrPostEx(SEV_INFO, 1, 0, "DAGTree_FunPack_New: MemNew failed.");
		return(NULL);
	}

	pFunPack->keyCompareFunc = keyCompareFunc;
	pFunPack->keyPrintFunc = keyPrintFunc,
	pFunPack->keyFreeFunc = keyFreeFunc;
	pFunPack->dataPrintFunc = dataPrintFunc,
	pFunPack->dataFreeFunc = dataFreeFunc;

	return(pFunPack);
}
/*free a DAGTreeFunctionPackage*/
DTreeFunctionPackagePtr DAGTree_FunPack_Free(DTreeFunctionPackagePtr funPack)
{
	if(funPack==NULL) return(NULL);
	return(MemFree(funPack));
}

/*create a new DAGTree node*/
DNodePtr DAGTree_NodeNew(ValNodePtr key, ValNodePtr data)
{
	DNodePtr newNode=NULL;

	if(key==NULL) {
		ErrPostEx(SEV_INFO, 1, 0, "DAGTree_NewNode: passed key is null.");
		return(NULL);
	}

	newNode = MemNew((size_t)sizeof(DNode));
	if(newNode==NULL) {
		ErrPostEx(SEV_INFO, 1, 0, "DAGTree_NewNode: MemNew failed.");
		return(NULL);
	}

	newNode->next=NULL;
	newNode->key=key;
	newNode->data=data;
	newNode->num = 1;
	newNode->state=DNode_state_none;
	newNode->p=NULL;
	newNode->c=NULL;

	return(newNode);
}

/*free a DAGTree node*/
DNodePtr DAGTree_NodeFree(DNodePtr node, DTreeFunctionPackagePtr funPack)
{
	DNodePtr c,l;
	if(node==NULL) {
		ErrPostEx(SEV_INFO, 1, 0, "DAGTree_NewNode: passed node is null.");
		return(NULL);
	}
	if(funPack==NULL) {
		ErrPostEx(SEV_INFO, 1, 0, "DAGTree_NewNode: passed funPack is null.");
		return(NULL);
	}

	node->key=(*(funPack->keyFreeFunc))(node->key);
	node->data=(*(funPack->dataFreeFunc))(node->data);
	
	/* recursively free children */
	c = node->c;
	while(c) { 
		l = c;
		c = c->next;
		l = DAGTree_NodeFree(l, funPack);
	}
	return MemFree(node);
}




/*find a node in the tree, by given key*/
DNodePtr DAGTree_Find(DNodePtr node, ValNodePtr key, DTreeFunctionPackagePtr funPack)
{
	DNodePtr n=NULL;
	if(key==NULL) {
		ErrPostEx(SEV_INFO, 1, 0, "DAGTree_Find: passed key is null.");
		return(NULL);
	}
	if(node==NULL) {
		ErrPostEx(SEV_INFO, 1, 0, "DAGTree_Find: passed node is null.");
		return(NULL);
	}
	if(funPack==NULL) {
		ErrPostEx(SEV_INFO, 1, 0, "DAGTree_Find: passed funPack is null.");
		return(NULL);
	}

	/* Try this node first */
	if(((*(funPack->keyCompareFunc))(key, node->key))==0) return node;
	else {
		/* Try children*/
		if((n = DAGTree_Find(node->c, key, funPack)) == NULL) {
			/* Try next */
			n = DAGTree_Find(node->next, key, funPack);
		}
	}
	return(n);
}

/** Find the last commmon intersecting node given a keylist */
DNodePtr DAGTree_FindLastIntersect(DNodePtr root, ValNodePtr keylist, DTreeFunctionPackagePtr funPack)
{
	DNodePtr lnode,node;
	ValNodePtr key;
	
	lnode = node = root;
	key = keylist;
	while(node && key) {
		node = DAGTree_Find(node, key, funPack);
		
		if(node != NULL) {lnode = node;}
		else {return lnode;}
		
		key = key->next;
	}
	return lnode;
}



DNodePtr DAGTree_AddVnp(DNodePtr *node, ValNodePtr key, ValNodePtr data, DTreeFunctionPackagePtr funPack)
{
	DNodePtr l = NULL,n = NULL;

	if(key==NULL) {
		ErrPostEx(SEV_INFO, 1, 0, "DAGTree_Add: passed key is null.");
		return(FALSE);
	}
	/*data is optional*/
	if(funPack==NULL) {
		ErrPostEx(SEV_INFO, 1, 0, "DAGTree_Add: passed funPack is null.");
		return(FALSE);
	}
	if(*node == NULL) {
		*node = DAGTree_NodeNew(key,data);
		return *node;
	}
	n = *node;
	n->num++;
	/* First check if this is the node, if so return it */
	if(((*(funPack->keyCompareFunc))(key, n->key))==0) return n;
	else {
		/* Try the nexts */
		l = n;
		n = n->next;
		while(n != NULL) {
			l = n;
			if(((*(funPack->keyCompareFunc))(key, n->key))==0) return n;
			n = n->next;
		}
		/* Otherwise, add at the end of the list */
		n = DAGTree_NodeNew(key,data);
		l->next = n;
	}
	return n;
}



/*This is done by passing a hierarchal list in which successive values will be compared against increase tree deph*/
/*When successive value are found, we follow the tree deeper.  When a successive values is not found, a new node is*/
/*placed in the next field and its children node is filled with the next successive value, recursively */

DNodePtr DAGTree_AddVnpList(DNodePtr *pnode, ValNodePtr keylist, ValNodePtr datalist, DTreeFunctionPackagePtr funPack)
{
	DNodePtr l,node = *pnode;
	ValNodePtr key, data;
	l = node;
	while(keylist && datalist) {
		key = keylist;
		data = datalist;
		keylist = keylist->next;
		datalist = datalist->next;
		key->next = NULL;
		data->next = NULL;

		if(*pnode == NULL) {
			node = DAGTree_NodeNew(key,data);
			l = node;
			*pnode = node;
			node = node->c;
		} 
		else {
			if(node == NULL) {
				node = DAGTree_AddVnp(&(l->c), key, data, funPack);
			} else {
				node = DAGTree_AddVnp(&node, key, data, funPack);
			}
			l = node;
			node = node->c;
		}
	}
	return node;
}




/* Iterate recursively through the tree, printing values */
Int4 DAGTree_Print(DNodePtr node, Int4 level, DTreeFunctionPackagePtr funPack)
{
	Int4 i;
	FILE *stream = NULL;
	stream = FileOpen("stdout","w");

	if(node) {
		if(level == 0 || level == 1 || level == 2 || level == 4 || !node->c) {
			for(i=0;i<level;i++) printf("%s"," ");
			fprintf(stream,"%ld: ", level);
			if(funPack->keyPrintFunc) (*(funPack->keyPrintFunc))(node->key, stream);
			fprintf(stream," - ");
			if(funPack->dataPrintFunc) (*(funPack->dataPrintFunc))(node->data, stream);
			fprintf(stream,"\n");
		}
		if(node->c) DAGTree_Print(node->c, level+1, funPack);
		if(node->next) return DAGTree_Print(node->next, level,  funPack);
	}
	return 0;
}



/*free a DAG tree*/
#define DAGTree_Free DAGTree_NodeFree

/* removes a node and its children from the tree*/
DNodePtr DAGTree_Del(DNodePtr node, ValNodePtr key, DTreeFunctionPackagePtr funPack)
{
	if(node == NULL) {
		ErrPostEx(SEV_INFO, 1, 0, "DAGTree_Del: passed node is null.");
		return(NULL);
	}
	if(key==NULL) {
		ErrPostEx(SEV_INFO, 1, 0, "DAGTree_Del: passed key is null.");
		return(NULL);
	}
	if(funPack==NULL) {
		ErrPostEx(SEV_INFO, 1, 0, "DAGTree_Del: passed funPack is null.");
		return(NULL);
	}

	/*find node & free */
	node = DAGTree_Find(node, key, funPack);
	return DAGTree_Free(node,funPack);
}


/*higher level functions*/
/*Use DTreeRootPtr at top level for increased convenience*/

/*initialize a function package based on key and data type*/
DTreeFunctionPackagePtr DAGTree_FunPack_Init(Uint2 keyType, Uint2 dataType)
{
	DTreeFunctionPackagePtr funPack=NULL;
	DAGTree_KeyCompareFunc keyCompareFunc=NULL;
	DAGTree_KeyPrintFunc keyPrintFunc=NULL;
	DAGTree_KeyFreeFunc keyFreeFunc=NULL;
	DAGTree_DataPrintFunc dataPrintFunc=NULL;
	DAGTree_DataFreeFunc dataFreeFunc=NULL;

	switch(keyType) {
		case DKey_id:
			keyCompareFunc  = DAGTree_CompareInt;
			keyPrintFunc    = DAGTree_PrintInt;
			keyFreeFunc     = DAGTree_FreeVnp;
			break;
		case DKey_str:
			keyCompareFunc  = DAGTree_CompareString;
			keyPrintFunc    = DAGTree_PrintString;
			keyFreeFunc     = DAGTree_FreeVnpData;
			break;
		case DKey_real:
			keyCompareFunc  = DAGTree_CompareFloatHi;
			keyPrintFunc    = DAGTree_PrintFloatHi;
			keyFreeFunc     = DAGTree_FreeVnp;
			break;
		default:
			ErrPostEx(SEV_ERROR, 1, 0, "DAGTree_FunPack_Init: Passed key type (%ld) is not supported.", (long) keyType);
			return(NULL);
	}

	switch(dataType) {
		case DData_id:
		case DData_ids:
			dataPrintFunc = DAGTree_PrintInt;
			dataFreeFunc = DAGTree_FreeVnp;
			break;
		case DData_str:
		case DData_strs:
			dataPrintFunc = DAGTree_PrintString;
			dataFreeFunc = DAGTree_FreeVnpData;
			break;
		case DData_real:
		case DData_reals:
			dataPrintFunc = DAGTree_PrintFloatHi;
			dataFreeFunc = DAGTree_FreeVnp;
			break;
		case DData_data_list:
		case DData_os:
		case DData_oss:
			dataFreeFunc = DAGTree_FreeVnpData;
			break;
		case DData_user_def:
			dataFreeFunc = DAGTree_FreeDummyData;
			break;
		case DData_gen_memfree:
			dataFreeFunc = DAGTree_MemFreeData;
			break;
		default:
			ErrPostEx(SEV_ERROR, 1, 0, "DAGTree_UserInitialize: Passed data type (%ld) is not supported.", (long) dataType);
			return(NULL);
	}

	funPack = DAGTree_FunPack_New((DAGTree_KeyCompareFunc) keyCompareFunc,
	                              (DAGTree_KeyPrintFunc) keyPrintFunc,
								  (DAGTree_KeyFreeFunc) keyFreeFunc, 
								  (DAGTree_DataPrintFunc) dataPrintFunc,
								  (DAGTree_DataFreeFunc) dataFreeFunc);
	funPack->keyType = keyType;
	funPack->dataType = dataType;

	return(funPack);
}

/*Initialize a DAGtree root*/
DTreeRootPtr DAGTree_Init(Uint2 keyType, Uint2 dataType)
{
	DTreeRootPtr DAGtrp=NULL;

	if((DAGtrp = MemNew(sizeof(DTreeRoot))) == NULL) {
		ErrPostEx(SEV_ERROR, 1, 0, "DAGTree_Init: MemNew failed.");
		return(NULL);
	}

	DAGtrp->funPack = DAGTree_FunPack_Init(keyType, dataType);
	return(DAGtrp);
}

/*free DAGtree root*/
DTreeRootPtr DAGTree_End(DTreeRootPtr treeRoot)
{
	treeRoot->treeRoot = DAGTree_Free(treeRoot->treeRoot, treeRoot->funPack);
	treeRoot->funPack = DAGTree_FunPack_Free(treeRoot->funPack);
	return MemFree(treeRoot);
}



/*
 * $Log: slri_dagtree.c,v $
 * Revision 1.5  2004/02/26 20:21:21  mjdumont
 * Added slri_dagtree.  Fixed linux compiler warnings/errors
 *
 * Revision 1.4  2004/02/26 19:44:47  mjdumont
 * Added 2 functions
 *
 * Revision 1.3  2004/02/02 21:28:01  mjdumont
 * Added include for IsNullStr
 *
 * Revision 1.2  2003/10/16 20:59:50  skuznets
 *
 * 1. Checking for NULL pointers when print values
 * 2. Added function Misc_TrimSpacesAroundString
 * 3. Added macro IsNullStr
 *
 * Revision 1.1.1.1  2003/03/20 09:32:34  root
 * Automated import from Sourceforge.net
 *
 * Revision 1.1  2003/03/19 22:15:40  micheld
 * Initial import of DAG tree algorithm
 *
 */

