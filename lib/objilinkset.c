/* $Id: objilinkset.c,v 1.1.1.1 2001/10/12 08:21:54 root Exp $
-- ***************************************************************************
--
--  Samuel Lunenfeld Research Institute (SLRI)
--
--  File: objilinkset.c
--  Description:  Modified LinkSet from NCBI's LinkSet that uses Int4 weights
--                and may be used in a nested list
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


#include <asn.h>

#define NLM_GENERATED_CODE_PROTO

#include <objilinkset.h>

static Boolean loaded = FALSE;

#include <ilinkset.h>

#ifndef NLM_EXTERN_LOADS
#define NLM_EXTERN_LOADS {}
#endif

NLM_EXTERN Boolean LIBCALL
objilinksetAsnLoad(void)
{

   if ( ! loaded) {
      NLM_EXTERN_LOADS

      if ( ! AsnLoad ())
      return FALSE;
      loaded = TRUE;
   }

   return TRUE;
}



/**************************************************
*    Generated object loaders for Module SLRI-ILink-set
*    Generated using ASNCODE Revision: 6.9 at Apr 19, 2001  7:41 PM
*
**************************************************/
/*****************************************************************************
*
*   ILinkSetNew2(Int4 num)
*
*****************************************************************************/
NLM_EXTERN 
ILinkSetPtr LIBCALL
ILinkSetNew2(Int4 num)
{
   ILinkSetPtr ptr = MemNew((size_t) sizeof(ILinkSet));
   Int4Ptr uptr = MemNew((size_t) sizeof(Int4)*num);	
   Int4Ptr wptr = MemNew((size_t) sizeof(Int4)*num);
   
   ptr->num = num;
   ptr->uids = uptr;
   ptr->weights = wptr;

   return ptr;

}

/**************************************************
*
*    ILinkSetNew()
*
**************************************************/
NLM_EXTERN 
ILinkSetPtr LIBCALL
ILinkSetNew(void)
{
   ILinkSetPtr ptr = MemNew((size_t) sizeof(ILinkSet));

   return ptr;

}


/**************************************************
*
*    ILinkSetFree()
*
**************************************************/
NLM_EXTERN 
ILinkSetPtr LIBCALL
ILinkSetFree(ILinkSetPtr ptr)
{

   if(ptr == NULL) {
      return (ILinkSetPtr) NULL;
   }
   ptr->uids = MemFree(ptr->uids);
   ptr->weights = MemFree(ptr->weights);

   return (ILinkSetPtr)MemFree(ptr);
}

/*****************************************************************************
*
*   ILinkSetAsnWrite()
*
*****************************************************************************/
NLM_EXTERN Boolean LIBCALL ILinkSetAsnWrite (ILinkSetPtr lsp, AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	AsnTypePtr atp;
    Int4 num, i;
    Int4Ptr ptr;
	Int4Ptr iptr;
    Boolean retval = FALSE;

	if (! loaded)
	{
		if (! objilinksetAsnLoad())
			return FALSE;
	}

	if (aip == NULL)
		return FALSE;

	atp = AsnLinkType(orig, ILINK_SET);   /* link local tree */
    if (atp == NULL)
        return FALSE;

	if (lsp == NULL) { AsnNullValueMsg(aip, atp); goto erret; }

    if (! AsnOpenStruct(aip, atp, (Pointer)lsp))
        goto erret;
    num = lsp->num;
    av.intvalue = num;
    if (! AsnWrite(aip, ILINK_SET_num, &av))
        goto erret;
    ptr = lsp->uids;
    if (! AsnOpenStruct(aip, ILINK_SET_uids, (Pointer)lsp->uids))
        goto erret;
    for (i = 0; i < num; i++)
    {
        av.intvalue = ptr[i];
        if (! AsnWrite(aip, ILINK_SET_uids_E, &av))
            goto erret;
    }
    if (! AsnCloseStruct(aip, ILINK_SET_uids, (Pointer)lsp->uids))
        goto erret;
    if (lsp->weights != NULL)
    {
        iptr = lsp->weights;
        if (! AsnOpenStruct(aip, ILINK_SET_weights, (Pointer)lsp->weights))
            goto erret;
        for (i = 0; i < num; i++)
        {
            av.intvalue = iptr[i];
            if (! AsnWrite(aip, ILINK_SET_weights_E, &av))
                goto erret;
        }
        if (! AsnCloseStruct(aip, ILINK_SET_weights, (Pointer)lsp->weights))
            goto erret;
    }
    if (! AsnCloseStruct(aip, atp, (Pointer)lsp))
        goto erret;
    retval = TRUE;
erret:
	AsnUnlinkType(orig);       /* unlink local tree */
	return retval;
}
/*****************************************************************************
*
*   ILinkSetAsnRead()
*
*****************************************************************************/
NLM_EXTERN ILinkSetPtr LIBCALL ILinkSetAsnRead (AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	AsnTypePtr atp;
    ILinkSetPtr lsp=NULL;
    Int4 num, i;
    Int4Ptr ptr;
	Int4Ptr iptr;

	if (! loaded)
	{
		if (! objilinksetAsnLoad())
			return lsp;
	}

	if (aip == NULL)
		return lsp;

	if (orig == NULL)           /* LinkSet ::= (self contained) */
		atp = AsnReadId(aip, amp, ILINK_SET);
	else
		atp = AsnLinkType(orig, ILINK_SET);    /* link in local tree */
    if (atp == NULL)
        return lsp;

	lsp = ILinkSetNew();
    if (lsp == NULL)
        goto erret;

	if (AsnReadVal(aip, atp, &av) <= 0)    /* read the start struct */
        goto erret;
	atp = AsnReadId(aip, amp, atp);  /* find the num */
    if (atp == NULL)
        goto erret;
	if (AsnReadVal(aip, atp, &av) <= 0)     /* get the num */
        goto erret;
    num = av.intvalue;
    lsp->num = num;

    atp = AsnReadId(aip, amp, atp);  /* start seq of uids */
    if (atp == NULL)
        goto erret;
    if (AsnReadVal(aip, atp, &av) <= 0)
        goto erret;
    ptr = (Int4Ptr)MemNew((size_t)(sizeof(Int4) * (num + 1)));  /* 0 sentinel at end */
    if (ptr == NULL)
        goto erret;
    lsp->uids = ptr;
    i = 0;
    while ((atp = AsnReadId(aip, amp, atp)) == ILINK_SET_uids_E)
    {
        if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
        ptr[i] = av.intvalue;
        i++;
        if (i > num)
            break;
    }
    if (atp == NULL)
        goto erret;
    if (i != num)
    {
        ErrPost(CTX_NCBIOBJ, 0, "Incorrect number of uids in Link-set. line %ld",
            aip->linenumber);
        goto erret;
    }
    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;   /* end seq of */

    atp = AsnReadId(aip, amp, atp);
    if (atp == NULL)
        goto erret;
    if (atp == ILINK_SET_weights)
    {
        if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
        iptr = (Int4Ptr)MemNew((size_t)(sizeof(Int4) * (num + 1)));  /* 0 sentinel at end */
        if (ptr == NULL)
            goto erret;
        lsp->weights = iptr;
        i = 0;
        while ((atp = AsnReadId(aip, amp, atp)) == ILINK_SET_weights_E)
        {
            if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
            iptr[i] = av.intvalue;
            i++;
            if (i > num)
                break;
        }
        if (atp == NULL)
            goto erret;
        if (i != num)
        {   
            ErrPost(CTX_NCBIOBJ, 0, "Incorrect number of weights in Link-set. line %ld",
                aip->linenumber);
            goto erret;
        }
        if (AsnReadVal(aip, atp, &av) <= 0) goto erret;   /* end seq of */

	    if ((atp = AsnReadId(aip, amp, atp)) == NULL)
	        goto erret;
    }

    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;  /* end struct */
ret:
	AsnUnlinkType(orig);       /* unlink local tree */
	return lsp;
erret:
    lsp = ILinkSetFree(lsp);
    goto ret;
}

/**************************************************
*
*    ILinkSetListNew()
*
**************************************************/
NLM_EXTERN 
ILinkSetListPtr LIBCALL
ILinkSetListNew(void)
{
   ILinkSetListPtr ptr = MemNew((size_t) sizeof(ILinkSetList));

   return ptr;

}


/**************************************************
*
*    ILinkSetListFree()
*
**************************************************/
NLM_EXTERN 
ILinkSetListPtr LIBCALL
ILinkSetListFree(ILinkSetListPtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   AsnGenericUserSeqOfFree(ptr -> set1, (AsnOptFreeFunc) ILinkSetFree);
   return MemFree(ptr);
}


/**************************************************
*
*    ILinkSetListAsnRead()
*
**************************************************/
NLM_EXTERN 
ILinkSetListPtr LIBCALL
ILinkSetListAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   ILinkSetListPtr ptr;

   if (! loaded)
   {
      if (! objilinksetAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* ILinkSetList ::= (self contained) */
      atp = AsnReadId(aip, amp, ILINK_SET_LIST);
   } else {
      atp = AsnLinkType(orig, ILINK_SET_LIST);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   ptr = ILinkSetListNew();
   if (ptr == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
      goto erret;
   }

   atp = AsnReadId(aip,amp, atp);
   func = NULL;

   if (atp == ILINK_SET_LIST_set1) {
      ptr -> set1 = AsnGenericUserSeqOfAsnRead(aip, amp, atp, &isError, (AsnReadFunc) ILinkSetAsnRead, (AsnOptFreeFunc) ILinkSetFree);
      if (isError && ptr -> set1 == NULL) {
         goto erret;
      }
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == ILINK_SET_LIST_id) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> id = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }

   if (AsnReadVal(aip, atp, &av) <= 0) {
      goto erret;
   }
   /* end struct */

ret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return ptr;

erret:
   aip -> io_failure = TRUE;
   ptr = ILinkSetListFree(ptr);
   goto ret;
}



/**************************************************
*
*    ILinkSetListAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
ILinkSetListAsnWrite(ILinkSetListPtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objilinksetAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, ILINK_SET_LIST);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) {
      goto erret;
   }

   AsnGenericUserSeqOfAsnWrite(ptr -> set1, (AsnWriteFunc) ILinkSetAsnWrite, aip, ILINK_SET_LIST_set1, ILINK_SET_LIST_set1_E);
   av.intvalue = ptr -> id;
   retval = AsnWrite(aip, ILINK_SET_LIST_id,  &av);
   if (! AsnCloseStruct(aip, atp, (Pointer)ptr)) {
      goto erret;
   }
   retval = TRUE;

erret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return retval;
}



/**************************************************
*
*    ILinkSetList2New()
*
**************************************************/
NLM_EXTERN 
ILinkSetList2Ptr LIBCALL
ILinkSetList2New(void)
{
   ILinkSetList2Ptr ptr = MemNew((size_t) sizeof(ILinkSetList2));

   return ptr;

}


/**************************************************
*
*    ILinkSetList2Free()
*
**************************************************/
NLM_EXTERN 
ILinkSetList2Ptr LIBCALL
ILinkSetList2Free(ILinkSetList2Ptr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   AsnGenericUserSeqOfFree(ptr -> set2, (AsnOptFreeFunc) ILinkSetListFree);
   return MemFree(ptr);
}


/**************************************************
*
*    ILinkSetList2AsnRead()
*
**************************************************/
NLM_EXTERN 
ILinkSetList2Ptr LIBCALL
ILinkSetList2AsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   ILinkSetList2Ptr ptr;

   if (! loaded)
   {
      if (! objilinksetAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* ILinkSetList2 ::= (self contained) */
      atp = AsnReadId(aip, amp, ILINK_SET_LIST2);
   } else {
      atp = AsnLinkType(orig, ILINK_SET_LIST2);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   ptr = ILinkSetList2New();
   if (ptr == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
      goto erret;
   }

   atp = AsnReadId(aip,amp, atp);
   func = NULL;

   if (atp == ILINK_SET_LIST2_set2) {
      ptr -> set2 = AsnGenericUserSeqOfAsnRead(aip, amp, atp, &isError, (AsnReadFunc) ILinkSetListAsnRead, (AsnOptFreeFunc) ILinkSetListFree);
      if (isError && ptr -> set2 == NULL) {
         goto erret;
      }
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == ILINK_SET_LIST2_id) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> id = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }

   if (AsnReadVal(aip, atp, &av) <= 0) {
      goto erret;
   }
   /* end struct */

ret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return ptr;

erret:
   aip -> io_failure = TRUE;
   ptr = ILinkSetList2Free(ptr);
   goto ret;
}



/**************************************************
*
*    ILinkSetList2AsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
ILinkSetList2AsnWrite(ILinkSetList2Ptr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objilinksetAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, ILINK_SET_LIST2);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) {
      goto erret;
   }

   AsnGenericUserSeqOfAsnWrite(ptr -> set2, (AsnWriteFunc) ILinkSetListAsnWrite, aip, ILINK_SET_LIST2_set2, ILINK_SET_LIST2_set2_E);
   av.intvalue = ptr -> id;
   retval = AsnWrite(aip, ILINK_SET_LIST2_id,  &av);
   if (! AsnCloseStruct(aip, atp, (Pointer)ptr)) {
      goto erret;
   }
   retval = TRUE;

erret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return retval;
}



/**************************************************
*
*    ILinkSetList3New()
*
**************************************************/
NLM_EXTERN 
ILinkSetList3Ptr LIBCALL
ILinkSetList3New(void)
{
   ILinkSetList3Ptr ptr = MemNew((size_t) sizeof(ILinkSetList3));

   return ptr;

}


/**************************************************
*
*    ILinkSetList3Free()
*
**************************************************/
NLM_EXTERN 
ILinkSetList3Ptr LIBCALL
ILinkSetList3Free(ILinkSetList3Ptr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   AsnGenericUserSeqOfFree(ptr -> set3, (AsnOptFreeFunc) ILinkSetList2Free);
   return MemFree(ptr);
}


/**************************************************
*
*    ILinkSetList3AsnRead()
*
**************************************************/
NLM_EXTERN 
ILinkSetList3Ptr LIBCALL
ILinkSetList3AsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   ILinkSetList3Ptr ptr;

   if (! loaded)
   {
      if (! objilinksetAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* ILinkSetList3 ::= (self contained) */
      atp = AsnReadId(aip, amp, ILINK_SET_LIST3);
   } else {
      atp = AsnLinkType(orig, ILINK_SET_LIST3);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   ptr = ILinkSetList3New();
   if (ptr == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
      goto erret;
   }

   atp = AsnReadId(aip,amp, atp);
   func = NULL;

   if (atp == ILINK_SET_LIST3_set3) {
      ptr -> set3 = AsnGenericUserSeqOfAsnRead(aip, amp, atp, &isError, (AsnReadFunc) ILinkSetList2AsnRead, (AsnOptFreeFunc) ILinkSetList2Free);
      if (isError && ptr -> set3 == NULL) {
         goto erret;
      }
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == ILINK_SET_LIST3_id) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> id = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }

   if (AsnReadVal(aip, atp, &av) <= 0) {
      goto erret;
   }
   /* end struct */

ret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return ptr;

erret:
   aip -> io_failure = TRUE;
   ptr = ILinkSetList3Free(ptr);
   goto ret;
}



/**************************************************
*
*    ILinkSetList3AsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
ILinkSetList3AsnWrite(ILinkSetList3Ptr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objilinksetAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, ILINK_SET_LIST3);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) {
      goto erret;
   }

   AsnGenericUserSeqOfAsnWrite(ptr -> set3, (AsnWriteFunc) ILinkSetList2AsnWrite, aip, ILINK_SET_LIST3_set3, ILINK_SET_LIST3_set3_E);
   av.intvalue = ptr -> id;
   retval = AsnWrite(aip, ILINK_SET_LIST3_id,  &av);
   if (! AsnCloseStruct(aip, atp, (Pointer)ptr)) {
      goto erret;
   }
   retval = TRUE;

erret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return retval;
}

/*
$Log: objilinkset.c,v $
Revision 1.1.1.1  2001/10/12 08:21:54  root
Automated import from Sourceforge.net

Revision 1.1.1.1  2001/10/10 22:11:03  kaca
initial import

Revision 1.1  2001/09/11 21:45:10  michel
Moved the ilinkset, flinkset, stack, set and prime files into slrilib

Revision 1.2  2001/04/09 16:00:05  michel
minor change

Revision 1.1  2001/04/05 18:02:16  michel
Removed SLRILinkset from slrilib, added new to slri_math.[c/h]
Made new Generic object library, slriobj, to include FLinkSet and ILinkSet

*/

