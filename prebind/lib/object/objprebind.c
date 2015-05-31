#include <asn.h>

#define NLM_GENERATED_CODE_PROTO

#include <objgen.h>
#include <objprebind.h>

static Boolean loaded = FALSE;

#include <prebind_spec.h>

#ifndef NLM_EXTERN_LOADS
#define NLM_EXTERN_LOADS {}
#endif

NLM_EXTERN Boolean LIBCALL
objprebindAsnLoad(void)
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
*    Generated object loaders for Module PREBIND
*    Generated using ASNCODE Revision: 6.14 at Feb 20, 2003  1:29 PM
*
**************************************************/


/**************************************************
*
*    PREBINDAccNew()
*
**************************************************/
NLM_EXTERN 
PREBINDAccPtr LIBCALL
PREBINDAccNew(void)
{
   PREBINDAccPtr ptr = MemNew((size_t) sizeof(PREBINDAcc));

   return ptr;

}


/**************************************************
*
*    PREBINDAccFree()
*
**************************************************/
NLM_EXTERN 
PREBINDAccPtr LIBCALL
PREBINDAccFree(PREBINDAccPtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   MemFree(ptr -> acc);
   MemFree(ptr -> db);
   MemFree(ptr -> locus);
   MemFree(ptr -> pprod);
   MemFree(ptr -> update);
   return MemFree(ptr);
}


/**************************************************
*
*    PREBINDAccAsnRead()
*
**************************************************/
NLM_EXTERN 
PREBINDAccPtr LIBCALL
PREBINDAccAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   PREBINDAccPtr ptr;

   if (! loaded)
   {
      if (! objprebindAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* PREBINDAcc ::= (self contained) */
      atp = AsnReadId(aip, amp, PREBIND_ACC);
   } else {
      atp = AsnLinkType(orig, PREBIND_ACC);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   ptr = PREBINDAccNew();
   if (ptr == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
      goto erret;
   }

   atp = AsnReadId(aip,amp, atp);
   func = NULL;

   if (atp == PREBIND_ACC_acc) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> acc = av.ptrvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == PREBIND_ACC_gi) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> gi = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == PREBIND_ACC_db) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> db = av.ptrvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == PREBIND_ACC_locus) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> locus = av.ptrvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == PREBIND_ACC_pprod) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> pprod = av.ptrvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == PREBIND_ACC_tax) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> tax = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == PREBIND_ACC_update) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> update = av.ptrvalue;
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
   ptr = PREBINDAccFree(ptr);
   goto ret;
}



/**************************************************
*
*    PREBINDAccAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
PREBINDAccAsnWrite(PREBINDAccPtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objprebindAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, PREBIND_ACC);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) {
      goto erret;
   }

   if (ptr -> acc != NULL) {
      av.ptrvalue = ptr -> acc;
      retval = AsnWrite(aip, PREBIND_ACC_acc,  &av);
   }
   av.intvalue = ptr -> gi;
   retval = AsnWrite(aip, PREBIND_ACC_gi,  &av);
   if (ptr -> db != NULL) {
      av.ptrvalue = ptr -> db;
      retval = AsnWrite(aip, PREBIND_ACC_db,  &av);
   }
   if (ptr -> locus != NULL) {
      av.ptrvalue = ptr -> locus;
      retval = AsnWrite(aip, PREBIND_ACC_locus,  &av);
   }
   if (ptr -> pprod != NULL) {
      av.ptrvalue = ptr -> pprod;
      retval = AsnWrite(aip, PREBIND_ACC_pprod,  &av);
   }
   av.intvalue = ptr -> tax;
   retval = AsnWrite(aip, PREBIND_ACC_tax,  &av);
   if (ptr -> update != NULL) {
      av.ptrvalue = ptr -> update;
      retval = AsnWrite(aip, PREBIND_ACC_update,  &av);
   }
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
*    PREBINDAccSetNew()
*
**************************************************/
NLM_EXTERN 
PREBINDAccSetPtr LIBCALL
PREBINDAccSetNew(void)
{
   PREBINDAccSetPtr ptr = MemNew((size_t) sizeof(PREBINDAccSet));

   return ptr;

}


/**************************************************
*
*    PREBINDAccSetFree()
*
**************************************************/
NLM_EXTERN 
PREBINDAccSetPtr LIBCALL
PREBINDAccSetFree(PREBINDAccSetPtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   AsnGenericUserSeqOfFree(ptr -> records, (AsnOptFreeFunc) PREBINDAccFree);
   return MemFree(ptr);
}


/**************************************************
*
*    PREBINDAccSetAsnRead()
*
**************************************************/
NLM_EXTERN 
PREBINDAccSetPtr LIBCALL
PREBINDAccSetAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   PREBINDAccSetPtr ptr;

   if (! loaded)
   {
      if (! objprebindAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* PREBINDAccSet ::= (self contained) */
      atp = AsnReadId(aip, amp, PREBIND_ACC_SET);
   } else {
      atp = AsnLinkType(orig, PREBIND_ACC_SET);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   ptr = PREBINDAccSetNew();
   if (ptr == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
      goto erret;
   }

   atp = AsnReadId(aip,amp, atp);
   func = NULL;

   if (atp == PREBIND_ACC_SET_records) {
      ptr -> records = AsnGenericUserSeqOfAsnRead(aip, amp, atp, &isError, (AsnReadFunc) PREBINDAccAsnRead, (AsnOptFreeFunc) PREBINDAccFree);
      if (isError && ptr -> records == NULL) {
         goto erret;
      }
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
   ptr = PREBINDAccSetFree(ptr);
   goto ret;
}



/**************************************************
*
*    PREBINDAccSetAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
PREBINDAccSetAsnWrite(PREBINDAccSetPtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objprebindAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, PREBIND_ACC_SET);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) {
      goto erret;
   }

   AsnGenericUserSeqOfAsnWrite(ptr -> records, (AsnWriteFunc) PREBINDAccAsnWrite, aip, PREBIND_ACC_SET_records, PREBIND_ACC_SET_records_E);
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
*    PREBINDSearchNew()
*
**************************************************/
NLM_EXTERN 
PREBINDSearchPtr LIBCALL
PREBINDSearchNew(void)
{
   PREBINDSearchPtr ptr = MemNew((size_t) sizeof(PREBINDSearch));

   return ptr;

}


/**************************************************
*
*    PREBINDSearchFree()
*
**************************************************/
NLM_EXTERN 
PREBINDSearchPtr LIBCALL
PREBINDSearchFree(PREBINDSearchPtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   MemFree(ptr -> name);
   MemFree(ptr -> searched);
   return MemFree(ptr);
}


/**************************************************
*
*    PREBINDSearchAsnRead()
*
**************************************************/
NLM_EXTERN 
PREBINDSearchPtr LIBCALL
PREBINDSearchAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   PREBINDSearchPtr ptr;

   if (! loaded)
   {
      if (! objprebindAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* PREBINDSearch ::= (self contained) */
      atp = AsnReadId(aip, amp, PREBIND_SEARCH);
   } else {
      atp = AsnLinkType(orig, PREBIND_SEARCH);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   ptr = PREBINDSearchNew();
   if (ptr == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
      goto erret;
   }

   atp = AsnReadId(aip,amp, atp);
   func = NULL;

   if (atp == PREBIND_SEARCH_gi) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> gi = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == PREBIND_SEARCH_name) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> name = av.ptrvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == PREBIND_SEARCH_source) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> source = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == PREBIND_SEARCH_taxid) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> taxid = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == PREBIND_SEARCH_searched) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> searched = av.ptrvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == PREBIND_SEARCH_status) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> status = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == PREBIND_SEARCH_namered) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> namered = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == PREBIND_SEARCH_results) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> results = av.intvalue;
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
   ptr = PREBINDSearchFree(ptr);
   goto ret;
}



/**************************************************
*
*    PREBINDSearchAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
PREBINDSearchAsnWrite(PREBINDSearchPtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objprebindAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, PREBIND_SEARCH);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) {
      goto erret;
   }

   av.intvalue = ptr -> gi;
   retval = AsnWrite(aip, PREBIND_SEARCH_gi,  &av);
   if (ptr -> name != NULL) {
      av.ptrvalue = ptr -> name;
      retval = AsnWrite(aip, PREBIND_SEARCH_name,  &av);
   }
   av.intvalue = ptr -> source;
   retval = AsnWrite(aip, PREBIND_SEARCH_source,  &av);
   av.intvalue = ptr -> taxid;
   retval = AsnWrite(aip, PREBIND_SEARCH_taxid,  &av);
   if (ptr -> searched != NULL) {
      av.ptrvalue = ptr -> searched;
      retval = AsnWrite(aip, PREBIND_SEARCH_searched,  &av);
   }
   av.intvalue = ptr -> status;
   retval = AsnWrite(aip, PREBIND_SEARCH_status,  &av);
   av.intvalue = ptr -> namered;
   retval = AsnWrite(aip, PREBIND_SEARCH_namered,  &av);
   av.intvalue = ptr -> results;
   retval = AsnWrite(aip, PREBIND_SEARCH_results,  &av);
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
*    PREBINDSearchSetNew()
*
**************************************************/
NLM_EXTERN 
PREBINDSearchSetPtr LIBCALL
PREBINDSearchSetNew(void)
{
   PREBINDSearchSetPtr ptr = MemNew((size_t) sizeof(PREBINDSearchSet));

   return ptr;

}


/**************************************************
*
*    PREBINDSearchSetFree()
*
**************************************************/
NLM_EXTERN 
PREBINDSearchSetPtr LIBCALL
PREBINDSearchSetFree(PREBINDSearchSetPtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   AsnGenericUserSeqOfFree(ptr -> records, (AsnOptFreeFunc) PREBINDSearchFree);
   return MemFree(ptr);
}


/**************************************************
*
*    PREBINDSearchSetAsnRead()
*
**************************************************/
NLM_EXTERN 
PREBINDSearchSetPtr LIBCALL
PREBINDSearchSetAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   PREBINDSearchSetPtr ptr;

   if (! loaded)
   {
      if (! objprebindAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* PREBINDSearchSet ::= (self contained) */
      atp = AsnReadId(aip, amp, PREBIND_SEARCH_SET);
   } else {
      atp = AsnLinkType(orig, PREBIND_SEARCH_SET);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   ptr = PREBINDSearchSetNew();
   if (ptr == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
      goto erret;
   }

   atp = AsnReadId(aip,amp, atp);
   func = NULL;

   if (atp == PREBIND_SEARCH_SET_records) {
      ptr -> records = AsnGenericUserSeqOfAsnRead(aip, amp, atp, &isError, (AsnReadFunc) PREBINDSearchAsnRead, (AsnOptFreeFunc) PREBINDSearchFree);
      if (isError && ptr -> records == NULL) {
         goto erret;
      }
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
   ptr = PREBINDSearchSetFree(ptr);
   goto ret;
}



/**************************************************
*
*    PREBINDSearchSetAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
PREBINDSearchSetAsnWrite(PREBINDSearchSetPtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objprebindAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, PREBIND_SEARCH_SET);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) {
      goto erret;
   }

   AsnGenericUserSeqOfAsnWrite(ptr -> records, (AsnWriteFunc) PREBINDSearchAsnWrite, aip, PREBIND_SEARCH_SET_records, PREBIND_SEARCH_SET_records_E);
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
*    PREBINDResultNew()
*
**************************************************/
NLM_EXTERN 
PREBINDResultPtr LIBCALL
PREBINDResultNew(void)
{
   PREBINDResultPtr ptr = MemNew((size_t) sizeof(PREBINDResult));

   return ptr;

}


/**************************************************
*
*    PREBINDResultFree()
*
**************************************************/
NLM_EXTERN 
PREBINDResultPtr LIBCALL
PREBINDResultFree(PREBINDResultPtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   MemFree(ptr -> name);
   return MemFree(ptr);
}


/**************************************************
*
*    PREBINDResultAsnRead()
*
**************************************************/
NLM_EXTERN 
PREBINDResultPtr LIBCALL
PREBINDResultAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   PREBINDResultPtr ptr;

   if (! loaded)
   {
      if (! objprebindAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* PREBINDResult ::= (self contained) */
      atp = AsnReadId(aip, amp, PREBIND_RESULT);
   } else {
      atp = AsnLinkType(orig, PREBIND_RESULT);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   ptr = PREBINDResultNew();
   if (ptr == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
      goto erret;
   }

   atp = AsnReadId(aip,amp, atp);
   func = NULL;

   if (atp == PREBIND_RESULT_gi) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> gi = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == PREBIND_RESULT_name) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> name = av.ptrvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == PREBIND_RESULT_pmid) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> pmid = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == PREBIND_RESULT_iscore) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> iscore = av.realvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == PREBIND_RESULT_iscore2) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> iscore2 = av.realvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == PREBIND_RESULT_itruth) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> itruth = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == PREBIND_RESULT_opno) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> opno = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == PREBIND_RESULT_opyes) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> opyes = av.intvalue;
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
   ptr = PREBINDResultFree(ptr);
   goto ret;
}



/**************************************************
*
*    PREBINDResultAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
PREBINDResultAsnWrite(PREBINDResultPtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objprebindAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, PREBIND_RESULT);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) {
      goto erret;
   }

   av.intvalue = ptr -> gi;
   retval = AsnWrite(aip, PREBIND_RESULT_gi,  &av);
   if (ptr -> name != NULL) {
      av.ptrvalue = ptr -> name;
      retval = AsnWrite(aip, PREBIND_RESULT_name,  &av);
   }
   av.intvalue = ptr -> pmid;
   retval = AsnWrite(aip, PREBIND_RESULT_pmid,  &av);
   av.realvalue = ptr -> iscore;
   retval = AsnWrite(aip, PREBIND_RESULT_iscore,  &av);
   av.realvalue = ptr -> iscore2;
   retval = AsnWrite(aip, PREBIND_RESULT_iscore2,  &av);
   av.intvalue = ptr -> itruth;
   retval = AsnWrite(aip, PREBIND_RESULT_itruth,  &av);
   av.intvalue = ptr -> opno;
   retval = AsnWrite(aip, PREBIND_RESULT_opno,  &av);
   av.intvalue = ptr -> opyes;
   retval = AsnWrite(aip, PREBIND_RESULT_opyes,  &av);
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
*    PREBINDResultSetNew()
*
**************************************************/
NLM_EXTERN 
PREBINDResultSetPtr LIBCALL
PREBINDResultSetNew(void)
{
   PREBINDResultSetPtr ptr = MemNew((size_t) sizeof(PREBINDResultSet));

   return ptr;

}


/**************************************************
*
*    PREBINDResultSetFree()
*
**************************************************/
NLM_EXTERN 
PREBINDResultSetPtr LIBCALL
PREBINDResultSetFree(PREBINDResultSetPtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   AsnGenericUserSeqOfFree(ptr -> records, (AsnOptFreeFunc) PREBINDResultFree);
   return MemFree(ptr);
}


/**************************************************
*
*    PREBINDResultSetAsnRead()
*
**************************************************/
NLM_EXTERN 
PREBINDResultSetPtr LIBCALL
PREBINDResultSetAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   PREBINDResultSetPtr ptr;

   if (! loaded)
   {
      if (! objprebindAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* PREBINDResultSet ::= (self contained) */
      atp = AsnReadId(aip, amp, PREBIND_RESULT_SET);
   } else {
      atp = AsnLinkType(orig, PREBIND_RESULT_SET);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   ptr = PREBINDResultSetNew();
   if (ptr == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
      goto erret;
   }

   atp = AsnReadId(aip,amp, atp);
   func = NULL;

   if (atp == PREBIND_RESULT_SET_records) {
      ptr -> records = AsnGenericUserSeqOfAsnRead(aip, amp, atp, &isError, (AsnReadFunc) PREBINDResultAsnRead, (AsnOptFreeFunc) PREBINDResultFree);
      if (isError && ptr -> records == NULL) {
         goto erret;
      }
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
   ptr = PREBINDResultSetFree(ptr);
   goto ret;
}



/**************************************************
*
*    PREBINDResultSetAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
PREBINDResultSetAsnWrite(PREBINDResultSetPtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objprebindAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, PREBIND_RESULT_SET);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) {
      goto erret;
   }

   AsnGenericUserSeqOfAsnWrite(ptr -> records, (AsnWriteFunc) PREBINDResultAsnWrite, aip, PREBIND_RESULT_SET_records, PREBIND_RESULT_SET_records_E);
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
*    PREBINDIsvmNew()
*
**************************************************/
NLM_EXTERN 
PREBINDIsvmPtr LIBCALL
PREBINDIsvmNew(void)
{
   PREBINDIsvmPtr ptr = MemNew((size_t) sizeof(PREBINDIsvm));

   return ptr;

}


/**************************************************
*
*    PREBINDIsvmFree()
*
**************************************************/
NLM_EXTERN 
PREBINDIsvmPtr LIBCALL
PREBINDIsvmFree(PREBINDIsvmPtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   return MemFree(ptr);
}


/**************************************************
*
*    PREBINDIsvmAsnRead()
*
**************************************************/
NLM_EXTERN 
PREBINDIsvmPtr LIBCALL
PREBINDIsvmAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   PREBINDIsvmPtr ptr;

   if (! loaded)
   {
      if (! objprebindAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* PREBINDIsvm ::= (self contained) */
      atp = AsnReadId(aip, amp, PREBIND_ISVM);
   } else {
      atp = AsnLinkType(orig, PREBIND_ISVM);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   ptr = PREBINDIsvmNew();
   if (ptr == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
      goto erret;
   }

   atp = AsnReadId(aip,amp, atp);
   func = NULL;

   if (atp == PREBIND_ISVM_pmid) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> pmid = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == PREBIND_ISVM_muid) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> muid = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == PREBIND_ISVM_iscore) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> iscore = av.realvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == PREBIND_ISVM_iscore2) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> iscore2 = av.realvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == PREBIND_ISVM_itruth) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> itruth = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == PREBIND_ISVM_opno) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> opno = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == PREBIND_ISVM_opyes) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> opyes = av.intvalue;
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
   ptr = PREBINDIsvmFree(ptr);
   goto ret;
}



/**************************************************
*
*    PREBINDIsvmAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
PREBINDIsvmAsnWrite(PREBINDIsvmPtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objprebindAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, PREBIND_ISVM);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) {
      goto erret;
   }

   av.intvalue = ptr -> pmid;
   retval = AsnWrite(aip, PREBIND_ISVM_pmid,  &av);
   av.intvalue = ptr -> muid;
   retval = AsnWrite(aip, PREBIND_ISVM_muid,  &av);
   av.realvalue = ptr -> iscore;
   retval = AsnWrite(aip, PREBIND_ISVM_iscore,  &av);
   av.realvalue = ptr -> iscore2;
   retval = AsnWrite(aip, PREBIND_ISVM_iscore2,  &av);
   av.intvalue = ptr -> itruth;
   retval = AsnWrite(aip, PREBIND_ISVM_itruth,  &av);
   av.intvalue = ptr -> opno;
   retval = AsnWrite(aip, PREBIND_ISVM_opno,  &av);
   av.intvalue = ptr -> opyes;
   retval = AsnWrite(aip, PREBIND_ISVM_opyes,  &av);
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
*    PREBINDIsvmSetNew()
*
**************************************************/
NLM_EXTERN 
PREBINDIsvmSetPtr LIBCALL
PREBINDIsvmSetNew(void)
{
   PREBINDIsvmSetPtr ptr = MemNew((size_t) sizeof(PREBINDIsvmSet));

   return ptr;

}


/**************************************************
*
*    PREBINDIsvmSetFree()
*
**************************************************/
NLM_EXTERN 
PREBINDIsvmSetPtr LIBCALL
PREBINDIsvmSetFree(PREBINDIsvmSetPtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   AsnGenericUserSeqOfFree(ptr -> records, (AsnOptFreeFunc) PREBINDIsvmFree);
   return MemFree(ptr);
}


/**************************************************
*
*    PREBINDIsvmSetAsnRead()
*
**************************************************/
NLM_EXTERN 
PREBINDIsvmSetPtr LIBCALL
PREBINDIsvmSetAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   PREBINDIsvmSetPtr ptr;

   if (! loaded)
   {
      if (! objprebindAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* PREBINDIsvmSet ::= (self contained) */
      atp = AsnReadId(aip, amp, PREBIND_ISVM_SET);
   } else {
      atp = AsnLinkType(orig, PREBIND_ISVM_SET);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   ptr = PREBINDIsvmSetNew();
   if (ptr == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
      goto erret;
   }

   atp = AsnReadId(aip,amp, atp);
   func = NULL;

   if (atp == PREBIND_ISVM_SET_records) {
      ptr -> records = AsnGenericUserSeqOfAsnRead(aip, amp, atp, &isError, (AsnReadFunc) PREBINDIsvmAsnRead, (AsnOptFreeFunc) PREBINDIsvmFree);
      if (isError && ptr -> records == NULL) {
         goto erret;
      }
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
   ptr = PREBINDIsvmSetFree(ptr);
   goto ret;
}



/**************************************************
*
*    PREBINDIsvmSetAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
PREBINDIsvmSetAsnWrite(PREBINDIsvmSetPtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objprebindAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, PREBIND_ISVM_SET);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) {
      goto erret;
   }

   AsnGenericUserSeqOfAsnWrite(ptr -> records, (AsnWriteFunc) PREBINDIsvmAsnWrite, aip, PREBIND_ISVM_SET_records, PREBIND_ISVM_SET_records_E);
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
*    PREBINDPointNew()
*
**************************************************/
NLM_EXTERN 
PREBINDPointPtr LIBCALL
PREBINDPointNew(void)
{
   PREBINDPointPtr ptr = MemNew((size_t) sizeof(PREBINDPoint));

   return ptr;

}


/**************************************************
*
*    PREBINDPointFree()
*
**************************************************/
NLM_EXTERN 
PREBINDPointPtr LIBCALL
PREBINDPointFree(PREBINDPointPtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   MemFree(ptr -> nama);
   MemFree(ptr -> namb);
   return MemFree(ptr);
}


/**************************************************
*
*    PREBINDPointAsnRead()
*
**************************************************/
NLM_EXTERN 
PREBINDPointPtr LIBCALL
PREBINDPointAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   PREBINDPointPtr ptr;

   if (! loaded)
   {
      if (! objprebindAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* PREBINDPoint ::= (self contained) */
      atp = AsnReadId(aip, amp, PREBIND_POINT);
   } else {
      atp = AsnLinkType(orig, PREBIND_POINT);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   ptr = PREBINDPointNew();
   if (ptr == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
      goto erret;
   }

   atp = AsnReadId(aip,amp, atp);
   func = NULL;

   if (atp == PREBIND_POINT_pmid) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> pmid = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == PREBIND_POINT_muid) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> muid = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == PREBIND_POINT_gia) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> gia = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == PREBIND_POINT_nama) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> nama = av.ptrvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == PREBIND_POINT_atype) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> atype = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == PREBIND_POINT_adgi) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> adgi = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == PREBIND_POINT_adf) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> adf = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == PREBIND_POINT_adt) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> adt = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == PREBIND_POINT_gib) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> gib = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == PREBIND_POINT_namb) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> namb = av.ptrvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == PREBIND_POINT_btype) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> btype = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == PREBIND_POINT_bdgi) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> bdgi = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == PREBIND_POINT_bdf) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> bdf = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == PREBIND_POINT_bdt) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> bdt = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == PREBIND_POINT_opno) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> opno = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == PREBIND_POINT_opyes) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> opyes = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == PREBIND_POINT_piscore) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> piscore = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == PREBIND_POINT_piscore2) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> piscore2 = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == PREBIND_POINT_pitruth) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> pitruth = av.intvalue;
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
   ptr = PREBINDPointFree(ptr);
   goto ret;
}



/**************************************************
*
*    PREBINDPointAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
PREBINDPointAsnWrite(PREBINDPointPtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objprebindAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, PREBIND_POINT);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) {
      goto erret;
   }

   av.intvalue = ptr -> pmid;
   retval = AsnWrite(aip, PREBIND_POINT_pmid,  &av);
   av.intvalue = ptr -> muid;
   retval = AsnWrite(aip, PREBIND_POINT_muid,  &av);
   av.intvalue = ptr -> gia;
   retval = AsnWrite(aip, PREBIND_POINT_gia,  &av);
   if (ptr -> nama != NULL) {
      av.ptrvalue = ptr -> nama;
      retval = AsnWrite(aip, PREBIND_POINT_nama,  &av);
   }
   av.intvalue = ptr -> atype;
   retval = AsnWrite(aip, PREBIND_POINT_atype,  &av);
   av.intvalue = ptr -> adgi;
   retval = AsnWrite(aip, PREBIND_POINT_adgi,  &av);
   av.intvalue = ptr -> adf;
   retval = AsnWrite(aip, PREBIND_POINT_adf,  &av);
   av.intvalue = ptr -> adt;
   retval = AsnWrite(aip, PREBIND_POINT_adt,  &av);
   av.intvalue = ptr -> gib;
   retval = AsnWrite(aip, PREBIND_POINT_gib,  &av);
   if (ptr -> namb != NULL) {
      av.ptrvalue = ptr -> namb;
      retval = AsnWrite(aip, PREBIND_POINT_namb,  &av);
   }
   av.intvalue = ptr -> btype;
   retval = AsnWrite(aip, PREBIND_POINT_btype,  &av);
   av.intvalue = ptr -> bdgi;
   retval = AsnWrite(aip, PREBIND_POINT_bdgi,  &av);
   av.intvalue = ptr -> bdf;
   retval = AsnWrite(aip, PREBIND_POINT_bdf,  &av);
   av.intvalue = ptr -> bdt;
   retval = AsnWrite(aip, PREBIND_POINT_bdt,  &av);
   av.intvalue = ptr -> opno;
   retval = AsnWrite(aip, PREBIND_POINT_opno,  &av);
   av.intvalue = ptr -> opyes;
   retval = AsnWrite(aip, PREBIND_POINT_opyes,  &av);
   av.intvalue = ptr -> piscore;
   retval = AsnWrite(aip, PREBIND_POINT_piscore,  &av);
   av.intvalue = ptr -> piscore2;
   retval = AsnWrite(aip, PREBIND_POINT_piscore2,  &av);
   av.intvalue = ptr -> pitruth;
   retval = AsnWrite(aip, PREBIND_POINT_pitruth,  &av);
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
*    PREBINDPointSetNew()
*
**************************************************/
NLM_EXTERN 
PREBINDPointSetPtr LIBCALL
PREBINDPointSetNew(void)
{
   PREBINDPointSetPtr ptr = MemNew((size_t) sizeof(PREBINDPointSet));

   return ptr;

}


/**************************************************
*
*    PREBINDPointSetFree()
*
**************************************************/
NLM_EXTERN 
PREBINDPointSetPtr LIBCALL
PREBINDPointSetFree(PREBINDPointSetPtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   AsnGenericUserSeqOfFree(ptr -> records, (AsnOptFreeFunc) PREBINDPointFree);
   return MemFree(ptr);
}


/**************************************************
*
*    PREBINDPointSetAsnRead()
*
**************************************************/
NLM_EXTERN 
PREBINDPointSetPtr LIBCALL
PREBINDPointSetAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   PREBINDPointSetPtr ptr;

   if (! loaded)
   {
      if (! objprebindAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* PREBINDPointSet ::= (self contained) */
      atp = AsnReadId(aip, amp, PREBIND_POINT_SET);
   } else {
      atp = AsnLinkType(orig, PREBIND_POINT_SET);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   ptr = PREBINDPointSetNew();
   if (ptr == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
      goto erret;
   }

   atp = AsnReadId(aip,amp, atp);
   func = NULL;

   if (atp == PREBIND_POINT_SET_records) {
      ptr -> records = AsnGenericUserSeqOfAsnRead(aip, amp, atp, &isError, (AsnReadFunc) PREBINDPointAsnRead, (AsnOptFreeFunc) PREBINDPointFree);
      if (isError && ptr -> records == NULL) {
         goto erret;
      }
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
   ptr = PREBINDPointSetFree(ptr);
   goto ret;
}



/**************************************************
*
*    PREBINDPointSetAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
PREBINDPointSetAsnWrite(PREBINDPointSetPtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objprebindAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, PREBIND_POINT_SET);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) {
      goto erret;
   }

   AsnGenericUserSeqOfAsnWrite(ptr -> records, (AsnWriteFunc) PREBINDPointAsnWrite, aip, PREBIND_POINT_SET_records, PREBIND_POINT_SET_records_E);
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
*    PREBINDInteractorNew()
*
**************************************************/
NLM_EXTERN 
PREBINDInteractorPtr LIBCALL
PREBINDInteractorNew(void)
{
   PREBINDInteractorPtr ptr = MemNew((size_t) sizeof(PREBINDInteractor));

   return ptr;

}


/**************************************************
*
*    PREBINDInteractorFree()
*
**************************************************/
NLM_EXTERN 
PREBINDInteractorPtr LIBCALL
PREBINDInteractorFree(PREBINDInteractorPtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   return MemFree(ptr);
}


/**************************************************
*
*    PREBINDInteractorAsnRead()
*
**************************************************/
NLM_EXTERN 
PREBINDInteractorPtr LIBCALL
PREBINDInteractorAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   PREBINDInteractorPtr ptr;

   if (! loaded)
   {
      if (! objprebindAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* PREBINDInteractor ::= (self contained) */
      atp = AsnReadId(aip, amp, PREBIND_INTERACTOR);
   } else {
      atp = AsnLinkType(orig, PREBIND_INTERACTOR);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   ptr = PREBINDInteractorNew();
   if (ptr == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
      goto erret;
   }

   atp = AsnReadId(aip,amp, atp);
   func = NULL;

   if (atp == PREBIND_INTERACTOR_gi) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> gi = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == PREBIND_INTERACTOR_pitruth) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> pitruth = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == PREBIND_INTERACTOR_opyes) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> opyes = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == PREBIND_INTERACTOR_opno) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> opno = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == PREBIND_INTERACTOR_piscore) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> piscore = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == PREBIND_INTERACTOR_piscore2) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> piscore2 = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == PREBIND_INTERACTOR_num_pmids) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> num_pmids = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == PREBIND_INTERACTOR_known_pmids) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> known_pmids = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == PREBIND_INTERACTOR_opyes_pmids) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> opyes_pmids = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == PREBIND_INTERACTOR_prob_pmids) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> prob_pmids = av.intvalue;
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
   ptr = PREBINDInteractorFree(ptr);
   goto ret;
}



/**************************************************
*
*    PREBINDInteractorAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
PREBINDInteractorAsnWrite(PREBINDInteractorPtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objprebindAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, PREBIND_INTERACTOR);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) {
      goto erret;
   }

   av.intvalue = ptr -> gi;
   retval = AsnWrite(aip, PREBIND_INTERACTOR_gi,  &av);
   av.intvalue = ptr -> pitruth;
   retval = AsnWrite(aip, PREBIND_INTERACTOR_pitruth,  &av);
   av.intvalue = ptr -> opyes;
   retval = AsnWrite(aip, PREBIND_INTERACTOR_opyes,  &av);
   av.intvalue = ptr -> opno;
   retval = AsnWrite(aip, PREBIND_INTERACTOR_opno,  &av);
   av.intvalue = ptr -> piscore;
   retval = AsnWrite(aip, PREBIND_INTERACTOR_piscore,  &av);
   av.intvalue = ptr -> piscore2;
   retval = AsnWrite(aip, PREBIND_INTERACTOR_piscore2,  &av);
   av.intvalue = ptr -> num_pmids;
   retval = AsnWrite(aip, PREBIND_INTERACTOR_num_pmids,  &av);
   av.intvalue = ptr -> known_pmids;
   retval = AsnWrite(aip, PREBIND_INTERACTOR_known_pmids,  &av);
   av.intvalue = ptr -> opyes_pmids;
   retval = AsnWrite(aip, PREBIND_INTERACTOR_opyes_pmids,  &av);
   av.intvalue = ptr -> prob_pmids;
   retval = AsnWrite(aip, PREBIND_INTERACTOR_prob_pmids,  &av);
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
*    PREBINDInteractorSetNew()
*
**************************************************/
NLM_EXTERN 
PREBINDInteractorSetPtr LIBCALL
PREBINDInteractorSetNew(void)
{
   PREBINDInteractorSetPtr ptr = MemNew((size_t) sizeof(PREBINDInteractorSet));

   return ptr;

}


/**************************************************
*
*    PREBINDInteractorSetFree()
*
**************************************************/
NLM_EXTERN 
PREBINDInteractorSetPtr LIBCALL
PREBINDInteractorSetFree(PREBINDInteractorSetPtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   AsnGenericUserSeqOfFree(ptr -> records, (AsnOptFreeFunc) PREBINDInteractorFree);
   return MemFree(ptr);
}


/**************************************************
*
*    PREBINDInteractorSetAsnRead()
*
**************************************************/
NLM_EXTERN 
PREBINDInteractorSetPtr LIBCALL
PREBINDInteractorSetAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   PREBINDInteractorSetPtr ptr;

   if (! loaded)
   {
      if (! objprebindAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* PREBINDInteractorSet ::= (self contained) */
      atp = AsnReadId(aip, amp, PREBIND_INTERACTOR_SET);
   } else {
      atp = AsnLinkType(orig, PREBIND_INTERACTOR_SET);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   ptr = PREBINDInteractorSetNew();
   if (ptr == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
      goto erret;
   }

   atp = AsnReadId(aip,amp, atp);
   func = NULL;

   if (atp == PREBIND_INTERACTOR_SET_num) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> num = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == PREBIND_INTERACTOR_SET_records) {
      ptr -> records = AsnGenericUserSeqOfAsnRead(aip, amp, atp, &isError, (AsnReadFunc) PREBINDInteractorAsnRead, (AsnOptFreeFunc) PREBINDInteractorFree);
      if (isError && ptr -> records == NULL) {
         goto erret;
      }
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
   ptr = PREBINDInteractorSetFree(ptr);
   goto ret;
}



/**************************************************
*
*    PREBINDInteractorSetAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
PREBINDInteractorSetAsnWrite(PREBINDInteractorSetPtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objprebindAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, PREBIND_INTERACTOR_SET);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) {
      goto erret;
   }

   av.intvalue = ptr -> num;
   retval = AsnWrite(aip, PREBIND_INTERACTOR_SET_num,  &av);
   AsnGenericUserSeqOfAsnWrite(ptr -> records, (AsnWriteFunc) PREBINDInteractorAsnWrite, aip, PREBIND_INTERACTOR_SET_records, PREBIND_INTERACTOR_SET_records_E);
   if (! AsnCloseStruct(aip, atp, (Pointer)ptr)) {
      goto erret;
   }
   retval = TRUE;

erret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return retval;
}

