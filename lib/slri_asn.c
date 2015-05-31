/* $Id: slri_asn.c,v 1.1.1.3 2002/03/02 09:25:26 root Exp $
-- ***************************************************************************
--
--  Samuel Lunenfeld Research Institute (SLRI)
--
--  File: slri_asn.c
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

#include <slri_asn.h>
#include <slri_misc.h>	/*for generic linked list type*/
#include <utilpub.h>

/*************************/
/* ASN.1 read/write mode */
/*************************/

/*private global variables to set read and write mode for functions that support this feature*/
#define ASNREADWRITEMODE_ARRAYSIZE 16 /*array size is arbitrarily chosen*/
Char g_AsnReadMode[ASNREADWRITEMODE_ARRAYSIZE]="rb";
Char g_AsnWriteMode[ASNREADWRITEMODE_ARRAYSIZE]="wb";

/*Set ASN.1 read mode e.g. "rb" or "r"*/
Boolean SetASNReadMode(CharPtr readMode)
{
	if(StringLen(readMode)>ASNREADWRITEMODE_ARRAYSIZE-1) {
		ErrPostEx(SEV_ERROR, 0, 0, "SetASNReadMode failed. Passed read mode is invalid.");
		return FALSE; 
	}

	StringNCpy(g_AsnReadMode, readMode, (size_t) ASNREADWRITEMODE_ARRAYSIZE-1);
	/*make sure string is null terminated*/
	g_AsnReadMode[ASNREADWRITEMODE_ARRAYSIZE-1]='\0';
	g_AsnReadMode[StringLen(g_AsnReadMode)]='\0';

	return(TRUE);
}

/*Set ASN.1 write mode e.g. "wb", "w" or "wx"*/
Boolean SetASNWriteMode(CharPtr writeMode)
{
	if(StringLen(writeMode)>ASNREADWRITEMODE_ARRAYSIZE-1) {
		ErrPostEx(SEV_ERROR, 0, 0, "SetASNWriteMode failed. Passed write mode is invalid.");
		return FALSE; 
	}

	StringNCpy(g_AsnWriteMode, writeMode, (size_t) ASNREADWRITEMODE_ARRAYSIZE-1);
	/*make sure string is null terminated*/
	g_AsnWriteMode[ASNREADWRITEMODE_ARRAYSIZE-1]='\0';
	g_AsnWriteMode[StringLen(g_AsnWriteMode)]='\0';

	return(TRUE);
}

/*Get ASN.1 read mode*/
CharPtr GetASNReadMode(void)
{
	return(g_AsnReadMode);
}

/*Get ASN.1 write mode*/
CharPtr GetASNWriteMode(void)
{
	return(g_AsnWriteMode);
}

/*********/
/* ASN.1 */
/*********/

/*Given a file containing an ASN.1 object, return a pointer to the data.  Supports ASCII (value) or binary ASN.1*/
Pointer ASN_GetFromFileName(CharPtr fileName, AsnReadFunc pAsnRead)
{
	AsnIoPtr aip=NULL;
	Pointer pASNdata=NULL;

	if(fileName == NULL) {
		ErrPostEx(SEV_ERROR, 0, 0, "ASN_GetFromFileName: fileName is NULL.");
		return NULL;
	}
	if(pAsnRead == NULL) {
		ErrPostEx(SEV_ERROR, 0, 0, "ASN_GetFromFileName: pAsnRead is NULL.");
		return NULL;
	}

	/*try binary*/
	aip = AsnIoOpen("rb", fileName);
	if(aip==NULL) {
		ErrPostEx(SEV_ERROR, 0, 0, "ASN_GetFromFileName: Binary AsnIoOpen returned NULL.");
		return(NULL);
	}
	pASNdata = (Pointer) (*pAsnRead) (aip, NULL);
	if(pASNdata==NULL) {
		aip = AsnIoClose(aip);
		/*try ASCII*/
		aip = AsnIoOpen("r", fileName);
		if(aip==NULL) {
			ErrPostEx(SEV_ERROR, 0, 0, "ASN_GetFromFileName: ASCII AsnIoOpen returned NULL.");
			return(NULL);
		}
		pASNdata = (Pointer) (*pAsnRead) (aip, NULL);
		if(pASNdata==NULL) {
			ErrPostEx(SEV_ERROR, 0, 0, "ASN_GetFromFileName: ASN.1 structure is invalid with given read function.");
			aip = AsnIoClose(aip);
			return(NULL);
		}
	}

	aip = AsnIoClose(aip);
	return(pASNdata);
}

/*Given a ByteStore containing an ASN.1 object, return a pointer to the data.  Supports ASCII (value) or binary ASN.1*/
Pointer ASN_GetFromBS(ByteStorePtr bsp, AsnReadFunc pAsnRead)
{
	AsnIoBSPtr aibp=NULL;
	Pointer pASNdata=NULL;

	if(bsp == NULL) {
		ErrPostEx(SEV_ERROR, 0, 0, "ASN_GetFromBS: bsp is NULL.");
		return NULL;
	}
	if(pAsnRead == NULL) {
		ErrPostEx(SEV_ERROR, 0, 0, "ASN_GetFromBS: pAsnRead is NULL.");
		return NULL;
	}

	BSSeek(bsp, 0L, SEEK_SET);

	/*try binary*/
	aibp = AsnIoBSOpen("rb", bsp);
	if(aibp==NULL) {
		ErrPostEx(SEV_ERROR, 0, 0, "ASN_GetFromBS: Binary AsnIoBSOpen returned NULL.");
		return(NULL);
	}
	pASNdata = (Pointer) (*pAsnRead) (aibp->aip, NULL);
	if(pASNdata==NULL) {
		aibp = AsnIoBSClose(aibp);
		/*try ASCII*/
		aibp = AsnIoBSOpen("r", bsp);
		if(aibp==NULL) {
			ErrPostEx(SEV_ERROR, 0, 0, "ASN_GetFromBS: ASCII AsnIoBSOpen returned NULL.");
			return(NULL);
		}
		pASNdata = (Pointer) (*pAsnRead) (aibp->aip, NULL);
		if(pASNdata==NULL) {
			ErrPostEx(SEV_ERROR, 0, 0, "ASN_GetFromBS: ASN.1 structure is invalid with given read function.");
			aibp = AsnIoBSClose(aibp);
			return(NULL);
		}
	}

	aibp = AsnIoBSClose(aibp);
	return(pASNdata);
}

/***************************************************/
/* Generic functions for ASN.1 'SEQUENCE OF' lists */
/***************************************************/

/*copy an ASN.1 generated code SEQUENCE OF list in memory*/
/*The read and write functions correspond to ValNode list elements, count is the returned length of the list*/
/*NOTE: This function only works for lists that have their 'next' pointer at the top of the structure*/
Pointer SLRI_AsnIoMemCopySeqOf(Pointer ptr, Int4Ptr count, AsnReadFunc readfunc, AsnWriteFunc writefunc, AsnOptFreeFunc freefunc)
{
	Pointer pNewList=NULL, pNewListHead=NULL;
	SlriGenericLinkListPtr sgllPtr=NULL, sgllPtrNewList=NULL;

	if(ptr == NULL) {
		ErrPostEx(SEV_ERROR, 0, 0, "SLRI_AsnIoMemCopySeqOf: passed ptr is NULL.");
		return(NULL);
	}
	if(readfunc == NULL) {
		ErrPostEx(SEV_ERROR, 0, 0, "SLRI_AsnIoMemCopySeqOf: passed AsnReadFunc is NULL.");
		return(NULL);
	}
	if(writefunc == NULL) {
		ErrPostEx(SEV_ERROR, 0, 0, "SLRI_AsnIoMemCopySeqOf: passed AsnWriteFunc is NULL.");
		return(NULL);
	}
	if(freefunc == NULL) {
		ErrPostEx(SEV_ERROR, 0, 0, "SLRI_AsnIoMemCopySeqOf: passed AsnOptFreeFunc is NULL.");
		return(NULL);
	}

	sgllPtr = (SlriGenericLinkListPtr) ptr;
	while(sgllPtr!=NULL) {
		if(pNewList!=NULL) {
			/*after first iteration*/
			sgllPtrNewList->next = AsnIoMemCopy(sgllPtr, (AsnReadFunc) readfunc, (AsnWriteFunc) writefunc);
			if(sgllPtrNewList->next==NULL) {
				ErrPostEx(SEV_INFO, 1, 0, "SLRI_AsnIoMemCopySeqOf: AsnIoMemCopy1 failed.");
				/*free and return*/
				AsnGenericUserSeqOfFree(pNewListHead, (AsnOptFreeFunc) freefunc);
				return(NULL);
			}
			sgllPtrNewList = sgllPtrNewList->next;
		}
		else {
			/*first iteration*/
			pNewList = AsnIoMemCopy(sgllPtr, (AsnReadFunc) readfunc, (AsnWriteFunc) writefunc);
			if(pNewList==NULL) {
				ErrPostEx(SEV_INFO, 1, 0, "SLRI_AsnIoMemCopySeqOf: AsnIoMemCopy2 failed.");
				return(NULL);
			}
			pNewListHead = pNewList;
			sgllPtrNewList = (SlriGenericLinkListPtr) pNewList;
		}
		sgllPtr = sgllPtr->next;
		if(count!=NULL) {
			(*count)++;
		}
	}

	return(pNewListHead);
}

/*callback function for vnpHeapSort - just compares ASN.1 objects - only used for vnpunique function*/
AsnWriteFunc g_SLRI_AsnCompareCallBack_AsnWriteFunc;	/*used to pass in the AsnWriteFunc to this function*/
int LIBCALLBACK SLRI_AsnCompareCallBack(Nlm_VoidPtr vp1, Nlm_VoidPtr vp2)
{
	/*This function is written for SLRI_AsnIoMemUniqueSeqOf, if you want a function that sorts a ValNodeList of
	ASN.1 objects, you must define it yourself.*/

	int status;
	ValNodePtr vnp1, vnp2;
	ValNodePtr PNTR vnpp1;
	ValNodePtr PNTR vnpp2;

	Pointer pv1, pv2;

	vnpp1 = (ValNodePtr PNTR) vp1;
	vnpp2 = (ValNodePtr PNTR) vp2;
	vnp1 = *vnpp1;
	vnp2 = *vnpp2;

	pv1 = vnp1->data.ptrvalue;
	pv2 = vnp2->data.ptrvalue;

	if(AsnIoMemComp(pv1, pv2, g_SLRI_AsnCompareCallBack_AsnWriteFunc)) {
		/*pv1 == pv2*/
		status = 0;
	}
	else {
		status = 1;	/*arbitrary*/
	}

	return status;
}

/*remove a vnp from a list
IMPORTANT USAGE EXAMPLE: (Make sure to catch the return value)
head = SLRI_AsnRemoveNode(head, x);*/
static ValNodePtr SLRI_AsnRemoveNode(ValNodePtr head, ValNodePtr x, AsnOptFreeFunc freefunc)
{
	ValNodePtr	v=NULL, p=NULL;

	if (head == NULL) {
		return NULL;
	}
	if (x == head) {
		head = x->next;
		x->next = NULL;
		freefunc(x->data.ptrvalue);
		ValNodeFree(x);
		return head;
	}
	for (v = head; v != NULL && v != x; v = v->next) {
		p = v;
	}
	if (v != NULL) {
		p->next = x->next;
		x->next = NULL;
		freefunc(x->data.ptrvalue);
		ValNodeFree(x);
	}
	return head;
}

/*generically uniques a sequence of ASN.1 objects*/
void SLRI_AsnIoMemUniqueSeqOf(Pointer *list, AsnWriteFunc writefunc, AsnOptFreeFunc freefunc)
{
	ValNodePtr vnpTmp=NULL, vnpHead=NULL, vnpCur=NULL, vnpNext=NULL, vnp=NULL;
	SlriGenericLinkListPtr sgllp=NULL, sgllpTmp=NULL;

	if((list==NULL)||((*list)==NULL)) {
		ErrPostEx(SEV_INFO, 1, 0, "SLRI_AsnIoMemUniqueSeqOf: Invalid type passed.");
		return;
	}

	/*copy all ASN.1 objects into a ValNode list so we are able to use the VnpHeapSort function*/
	/*could use the HeapSort function directly, but you still have to copy it into an array*/
	sgllp=(SlriGenericLinkListPtr)(*list);
	while(sgllp!=NULL) {
		Slri_ValNodeAddPointer(&vnp, 0, sgllp);
		sgllp=sgllp->next;
	}
	sgllp=NULL;

	/*return if only one node in the list*/
	if(vnp->next == NULL) {
		ValNodeFree(vnp);
		return;
	}

	/*sort*/
	g_SLRI_AsnCompareCallBack_AsnWriteFunc = writefunc;
	VnpHeapSort(&vnp, SLRI_AsnCompareCallBack);
	vnpHead = vnp;
	/*unique*/
	vnpCur = vnpHead;
	vnpNext = vnpHead->next;
	while(vnp != NULL) {
		if(SLRI_AsnCompareCallBack(&vnpCur, &vnpNext)==0) {
			vnpTmp = vnp;
			vnpCur = vnp = vnp->next;
			vnpHead = SLRI_AsnRemoveNode(vnpHead, vnpTmp, freefunc);
			if(vnp == NULL) {
				break;
			}
			if(vnp->next != NULL) {
				vnpNext = vnp->next;
			}
			else {
				break;
			}
		}
		else {
			vnp = vnp->next;
			if(vnp != NULL) {
				vnpCur = vnp;
				if(vnp->next != NULL) {
					vnpNext = vnp->next;
				}
				else {
					break;
				}
			}
		}
	}
	g_SLRI_AsnCompareCallBack_AsnWriteFunc = NULL;

	/*Move back into sequence of list format*/
	vnp = vnpHead;
	while(vnp!=NULL) {
		sgllpTmp = (SlriGenericLinkListPtr) vnp->data.ptrvalue;
		sgllpTmp->next=NULL;	/*make sure this is NULL to prevent possible infinite loop*/
		Misc_GenericAddObjectToListStart((Pointer *)(&sgllp), vnp->data.ptrvalue);
		vnp=vnp->next;
	}
	ValNodeFree(vnpHead);

	(*list) = (Pointer) sgllp;

	return;
}

/*
$Log: slri_asn.c,v $
Revision 1.1.1.3  2002/03/02 09:25:26  root
Automated import from Sourceforge.net

Revision 1.3  2002/03/01 23:57:45  gbader
Fixed compiler warnings
Moved generic asn set read/write mode to slri_asn

Revision 1.2  2002/02/19 22:45:36  gbader
Added ASN.1 get from file name function.

Revision 1.1.1.2  2001/05/24 15:08:46  micheld
Update of slrilib

Revision 1.14  2001/04/30 20:44:12  gbader
Updated comments

Revision 1.13  2001/04/05 14:12:02  feldman
Fixed a few compiler warnings

Revision 1.12  2001/04/04 23:16:06  gbader
Fixed small bug in SLRI_AsnIoMemUniqueSeqOf

Revision 1.11  2001/03/30 20:07:45  gbader
Fixed memory leak

Revision 1.10  2001/03/29 22:14:09  gbader
Moved statics to before function prototype

Revision 1.9  2001/03/27 21:11:47  gbader
Fixed bug in unique function

Revision 1.8  2001/03/26 21:58:07  gbader
Added function to unique a SEQUENCE OF asn.1 object

Revision 1.7  2001/03/08 00:13:29  gbader
Moved includes to C files from header files to reduce header dependencies and speed compilation

Revision 1.6  2001/02/07 21:24:48  gbader
Moved remaining CodeBase functions to slri_cb.[ch].  No other lib file should ever include d4all.h

Revision 1.5  2001/01/13 04:04:10  gbader
Added IBM DB2 UDB to license exceptions.

Revision 1.4  2000/12/20 18:12:10  gbader
Fixed small bug - GetASNMemMemo reported error for f4memoLen if memo field was empty

Revision 1.3  2000/09/05 16:48:46  gbader
Changed (CharPtr) cast to (BytePtr) cast in GetASNMemMemo to avoid compiler warnings on certain platforms

Revision 1.2  2000/09/05 13:55:08  gbader
Updated License so that it is now GNU.
Added CVS keywords ID and Log as well as new line at end of files so compilers on some platforms don't complain

*/
