/* $Id: objflinkset.c,v 1.1.1.1 2001/10/12 08:21:54 root Exp $
-- ***************************************************************************
--
--  Samuel Lunenfeld Research Institute (SLRI)
--
--  File: objflinkset.c
--  Description:  Modified LinkSet from NCBI's LinkSet that uses FloatHi weights
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

#include <objflinkset.h>

static Boolean loaded = FALSE;

#include <flinkset.h>

#ifndef NLM_EXTERN_LOADS
#define NLM_EXTERN_LOADS {}
#endif

NLM_EXTERN Boolean LIBCALL
objflinksetAsnLoad(void)
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
*    Generated object loaders for Module SLRI-FLink-set
*    Generated using ASNCODE Revision: 6.9 at Apr 19, 2001  7:41 PM
*
**************************************************/
/*****************************************************************************
*
*   SLRILinkSetNew2(Int4 num)
*
*****************************************************************************/
NLM_EXTERN 
FLinkSetPtr LIBCALL
FLinkSetNew2(Int4 num)
{
   FLinkSetPtr ptr = MemNew((size_t) sizeof(FLinkSet));
   Int4Ptr iptr = MemNew((size_t) sizeof(Int4)*num);	
   FloatHiPtr fptr = MemNew((size_t) sizeof(FloatHi)*num);
   
   ptr->num = num;   
   ptr->uids = iptr;
   ptr->weights = fptr;

   return ptr;

}

/**************************************************
*
*    FLinkSetNew()
*
**************************************************/
NLM_EXTERN 
FLinkSetPtr LIBCALL
FLinkSetNew(void)
{
   FLinkSetPtr ptr = MemNew((size_t) sizeof(FLinkSet));

   return ptr;

}


/**************************************************
*
*    FLinkSetFree()
*
**************************************************/
NLM_EXTERN 
FLinkSetPtr LIBCALL
FLinkSetFree(FLinkSetPtr ptr)
{

   if(ptr == NULL) {
      return (FLinkSetPtr) NULL;
   }
   ptr->uids = MemFree(ptr->uids);
   ptr->weights = MemFree(ptr->weights);

   return (FLinkSetPtr)MemFree(ptr);
}


/**************************************************
*
*    FLinkSetAsnRead()
*
**************************************************/
NLM_EXTERN FLinkSetPtr LIBCALL FLinkSetAsnRead (AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	AsnTypePtr atp;
    FLinkSetPtr lsp=NULL;
    Int4 num, i;
    Int4Ptr ptr;
	FloatHiPtr fptr;

	if (! loaded)
	{
		if (! objflinksetAsnLoad())
			return lsp;
	}

	if (aip == NULL)
		return lsp;

	if (orig == NULL)           /* LinkSet ::= (self contained) */
		atp = AsnReadId(aip, amp, FLINK_SET);
	else
		atp = AsnLinkType(orig, FLINK_SET);    /* link in local tree */
    if (atp == NULL)
        return lsp;

	lsp = FLinkSetNew();
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
    while ((atp = AsnReadId(aip, amp, atp)) == FLINK_SET_uids_E)
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
    if (atp == FLINK_SET_weights)
    {
        if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
        fptr = (FloatHiPtr)MemNew((size_t)(sizeof(FloatHi) * (num + 1)));  /* 0 sentinel at end */
        if (fptr == NULL)
            goto erret;
        lsp->weights = fptr;
        i = 0;
        while ((atp = AsnReadId(aip, amp, atp)) == FLINK_SET_weights_E)
        {
            if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
            fptr[i] = av.realvalue;
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
    lsp = FLinkSetFree(lsp);
    goto ret;
}


/**************************************************
*
*    FLinkSetAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL FLinkSetAsnWrite (FLinkSetPtr lsp, AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	AsnTypePtr atp;
    Int4 num, i;
    Int4Ptr ptr;
	FloatHiPtr fptr;
    Boolean retval = FALSE;

	if (! loaded)
	{
		if (! objflinksetAsnLoad())
			return FALSE;
	}

	if (aip == NULL)
		return FALSE;

	atp = AsnLinkType(orig, FLINK_SET);   /* link local tree */
    if (atp == NULL)
        return FALSE;

	if (lsp == NULL) { AsnNullValueMsg(aip, atp); goto erret; }

    if (! AsnOpenStruct(aip, atp, (Pointer)lsp))
        goto erret;
    num = lsp->num;
    av.intvalue = num;
    if (! AsnWrite(aip, FLINK_SET_num, &av))
        goto erret;
    ptr = lsp->uids;
    if (! AsnOpenStruct(aip, FLINK_SET_uids, (Pointer)lsp->uids))
        goto erret;
    for (i = 0; i < num; i++)
    {
        av.intvalue = ptr[i];
        if (! AsnWrite(aip, FLINK_SET_uids_E, &av))
            goto erret;
    }
    if (! AsnCloseStruct(aip, FLINK_SET_uids, (Pointer)lsp->uids))
        goto erret;
    if (lsp->weights != NULL)
    {
        fptr = lsp->weights;
        if (! AsnOpenStruct(aip, FLINK_SET_weights, (Pointer)lsp->weights))
            goto erret;
        for (i = 0; i < num; i++)
        {
            av.realvalue = fptr[i];
            if (! AsnWrite(aip, FLINK_SET_weights_E, &av))
                goto erret;
        }
        if (! AsnCloseStruct(aip, FLINK_SET_weights, (Pointer)lsp->weights))
            goto erret;
    }
    if (! AsnCloseStruct(aip, atp, (Pointer)lsp))
        goto erret;
    retval = TRUE;
erret:
	AsnUnlinkType(orig);       /* unlink local tree */
	return retval;
}


/**************************************************
*
*    FLinkSetListNew()
*
**************************************************/
NLM_EXTERN 
FLinkSetListPtr LIBCALL
FLinkSetListNew(void)
{
   FLinkSetListPtr ptr = MemNew((size_t) sizeof(FLinkSetList));

   return ptr;

}


/**************************************************
*
*    FLinkSetListFree()
*
**************************************************/
NLM_EXTERN 
FLinkSetListPtr LIBCALL
FLinkSetListFree(FLinkSetListPtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   AsnGenericUserSeqOfFree(ptr -> set1, (AsnOptFreeFunc) FLinkSetFree);
   return MemFree(ptr);
}


/**************************************************
*
*    FLinkSetListAsnRead()
*
**************************************************/
NLM_EXTERN 
FLinkSetListPtr LIBCALL
FLinkSetListAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   FLinkSetListPtr ptr;

   if (! loaded)
   {
      if (! objflinksetAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* FLinkSetList ::= (self contained) */
      atp = AsnReadId(aip, amp, FLINK_SET_LIST);
   } else {
      atp = AsnLinkType(orig, FLINK_SET_LIST);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   ptr = FLinkSetListNew();
   if (ptr == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
      goto erret;
   }

   atp = AsnReadId(aip,amp, atp);
   func = NULL;

   if (atp == FLINK_SET_LIST_set1) {
      ptr -> set1 = AsnGenericUserSeqOfAsnRead(aip, amp, atp, &isError, (AsnReadFunc) FLinkSetAsnRead, (AsnOptFreeFunc) FLinkSetFree);
      if (isError && ptr -> set1 == NULL) {
         goto erret;
      }
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == FLINK_SET_LIST_id) {
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
   ptr = FLinkSetListFree(ptr);
   goto ret;
}



/**************************************************
*
*    FLinkSetListAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
FLinkSetListAsnWrite(FLinkSetListPtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objflinksetAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, FLINK_SET_LIST);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) {
      goto erret;
   }

   AsnGenericUserSeqOfAsnWrite(ptr -> set1, (AsnWriteFunc) FLinkSetAsnWrite, aip, FLINK_SET_LIST_set1, FLINK_SET_LIST_set1_E);
   av.intvalue = ptr -> id;
   retval = AsnWrite(aip, FLINK_SET_LIST_id,  &av);
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
*    FLinkSetList2New()
*
**************************************************/
NLM_EXTERN 
FLinkSetList2Ptr LIBCALL
FLinkSetList2New(void)
{
   FLinkSetList2Ptr ptr = MemNew((size_t) sizeof(FLinkSetList2));

   return ptr;

}


/**************************************************
*
*    FLinkSetList2Free()
*
**************************************************/
NLM_EXTERN 
FLinkSetList2Ptr LIBCALL
FLinkSetList2Free(FLinkSetList2Ptr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   AsnGenericUserSeqOfFree(ptr -> set2, (AsnOptFreeFunc) FLinkSetListFree);
   return MemFree(ptr);
}


/**************************************************
*
*    FLinkSetList2AsnRead()
*
**************************************************/
NLM_EXTERN 
FLinkSetList2Ptr LIBCALL
FLinkSetList2AsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   FLinkSetList2Ptr ptr;

   if (! loaded)
   {
      if (! objflinksetAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* FLinkSetList2 ::= (self contained) */
      atp = AsnReadId(aip, amp, FLINK_SET_LIST2);
   } else {
      atp = AsnLinkType(orig, FLINK_SET_LIST2);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   ptr = FLinkSetList2New();
   if (ptr == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
      goto erret;
   }

   atp = AsnReadId(aip,amp, atp);
   func = NULL;

   if (atp == FLINK_SET_LIST2_set2) {
      ptr -> set2 = AsnGenericUserSeqOfAsnRead(aip, amp, atp, &isError, (AsnReadFunc) FLinkSetListAsnRead, (AsnOptFreeFunc) FLinkSetListFree);
      if (isError && ptr -> set2 == NULL) {
         goto erret;
      }
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == FLINK_SET_LIST2_id) {
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
   ptr = FLinkSetList2Free(ptr);
   goto ret;
}



/**************************************************
*
*    FLinkSetList2AsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
FLinkSetList2AsnWrite(FLinkSetList2Ptr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objflinksetAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, FLINK_SET_LIST2);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) {
      goto erret;
   }

   AsnGenericUserSeqOfAsnWrite(ptr -> set2, (AsnWriteFunc) FLinkSetListAsnWrite, aip, FLINK_SET_LIST2_set2, FLINK_SET_LIST2_set2_E);
   av.intvalue = ptr -> id;
   retval = AsnWrite(aip, FLINK_SET_LIST2_id,  &av);
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
*    FLinkSetList3New()
*
**************************************************/
NLM_EXTERN 
FLinkSetList3Ptr LIBCALL
FLinkSetList3New(void)
{
   FLinkSetList3Ptr ptr = MemNew((size_t) sizeof(FLinkSetList3));

   return ptr;

}


/**************************************************
*
*    FLinkSetList3Free()
*
**************************************************/
NLM_EXTERN 
FLinkSetList3Ptr LIBCALL
FLinkSetList3Free(FLinkSetList3Ptr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   AsnGenericUserSeqOfFree(ptr -> set3, (AsnOptFreeFunc) FLinkSetList2Free);
   return MemFree(ptr);
}


/**************************************************
*
*    FLinkSetList3AsnRead()
*
**************************************************/
NLM_EXTERN 
FLinkSetList3Ptr LIBCALL
FLinkSetList3AsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   FLinkSetList3Ptr ptr;

   if (! loaded)
   {
      if (! objflinksetAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* FLinkSetList3 ::= (self contained) */
      atp = AsnReadId(aip, amp, FLINK_SET_LIST3);
   } else {
      atp = AsnLinkType(orig, FLINK_SET_LIST3);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   ptr = FLinkSetList3New();
   if (ptr == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
      goto erret;
   }

   atp = AsnReadId(aip,amp, atp);
   func = NULL;

   if (atp == FLINK_SET_LIST3_set3) {
      ptr -> set3 = AsnGenericUserSeqOfAsnRead(aip, amp, atp, &isError, (AsnReadFunc) FLinkSetList2AsnRead, (AsnOptFreeFunc) FLinkSetList2Free);
      if (isError && ptr -> set3 == NULL) {
         goto erret;
      }
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == FLINK_SET_LIST3_id) {
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
   ptr = FLinkSetList3Free(ptr);
   goto ret;
}



/**************************************************
*
*    FLinkSetList3AsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
FLinkSetList3AsnWrite(FLinkSetList3Ptr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objflinksetAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, FLINK_SET_LIST3);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) {
      goto erret;
   }

   AsnGenericUserSeqOfAsnWrite(ptr -> set3, (AsnWriteFunc) FLinkSetList2AsnWrite, aip, FLINK_SET_LIST3_set3, FLINK_SET_LIST3_set3_E);
   av.intvalue = ptr -> id;
   retval = AsnWrite(aip, FLINK_SET_LIST3_id,  &av);
   if (! AsnCloseStruct(aip, atp, (Pointer)ptr)) {
      goto erret;
   }
   retval = TRUE;

erret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return retval;
}

/*
$Log: objflinkset.c,v $
Revision 1.1.1.1  2001/10/12 08:21:54  root
Automated import from Sourceforge.net

Revision 1.1.1.1  2001/10/10 22:11:03  kaca
initial import

Revision 1.1  2001/09/11 21:45:10  michel
Moved the ilinkset, flinkset, stack, set and prime files into slrilib

Revision 1.4  2001/08/11 18:10:33  michel
minor change

Revision 1.3  2001/04/10 20:04:47  michel
minor fix

Revision 1.2  2001/04/09 16:00:05  michel
minor change

Revision 1.1  2001/04/05 18:02:16  michel
Removed SLRILinkset from slrilib, added new to slri_math.[c/h]
Made new Generic object library, slriobj, to include FLinkSet and ILinkSet

*/


