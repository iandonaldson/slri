#include <asn.h>

#define NLM_GENERATED_CODE_PROTO

#include <objNBlastasn.h>

static Boolean loaded = FALSE;

#include <NBlastasn.h>

#ifndef NLM_EXTERN_LOADS
#define NLM_EXTERN_LOADS {}
#endif

NLM_EXTERN Boolean LIBCALL
objNBlastasnAsnLoad(void)
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
*    Generated object loaders for Module NBlast
*    Generated using ASNCODE Revision: 6.15 at Apr 11, 2005  5:01 PM
*
**************************************************/


/**************************************************
*
*    NBlastResultSetNew()
*
**************************************************/
NLM_EXTERN 
NBlastResultSetPtr LIBCALL
NBlastResultSetNew(void)
{
   NBlastResultSetPtr ptr = MemNew((size_t) sizeof(NBlastResultSet));

   return ptr;

}


/**************************************************
*
*    NBlastResultSetFree()
*
**************************************************/
NLM_EXTERN 
NBlastResultSetPtr LIBCALL
NBlastResultSetFree(NBlastResultSetPtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   AsnGenericUserSeqOfFree(ptr -> results, (AsnOptFreeFunc) NBlastResultFree);
   return MemFree(ptr);
}


/**************************************************
*
*    NBlastResultSetAsnRead()
*
**************************************************/
NLM_EXTERN 
NBlastResultSetPtr LIBCALL
NBlastResultSetAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   NBlastResultSetPtr ptr;

   if (! loaded)
   {
      if (! objNBlastasnAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* NBlastResultSet ::= (self contained) */
      atp = AsnReadId(aip, amp, NBLAST_RESULT_SET);
   } else {
      atp = AsnLinkType(orig, NBLAST_RESULT_SET);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   ptr = NBlastResultSetNew();
   if (ptr == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
      goto erret;
   }

   atp = AsnReadId(aip,amp, atp);
   func = NULL;

   if (atp == NBLAST_RESULT_SET_query_gi) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> query_gi = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == NBLAST_RESULT_SET_subject_gi) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> subject_gi = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == NBLAST_RESULT_SET_results) {
      ptr -> results = AsnGenericUserSeqOfAsnRead(aip, amp, atp, &isError, (AsnReadFunc) NBlastResultAsnRead, (AsnOptFreeFunc) NBlastResultFree);
      if (isError && ptr -> results == NULL) {
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
   ptr = NBlastResultSetFree(ptr);
   goto ret;
}



/**************************************************
*
*    NBlastResultSetAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
NBlastResultSetAsnWrite(NBlastResultSetPtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objNBlastasnAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, NBLAST_RESULT_SET);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) {
      goto erret;
   }

   av.intvalue = ptr -> query_gi;
   retval = AsnWrite(aip, NBLAST_RESULT_SET_query_gi,  &av);
   av.intvalue = ptr -> subject_gi;
   retval = AsnWrite(aip, NBLAST_RESULT_SET_subject_gi,  &av);
   AsnGenericUserSeqOfAsnWrite(ptr -> results, (AsnWriteFunc) NBlastResultAsnWrite, aip, NBLAST_RESULT_SET_results, NBLAST_RESULT_SET_results_E);
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
*    NBlastResultNew()
*
**************************************************/
NLM_EXTERN 
NBlastResultPtr LIBCALL
NBlastResultNew(void)
{
   NBlastResultPtr ptr = MemNew((size_t) sizeof(NBlastResult));

   return ptr;

}


/**************************************************
*
*    NBlastResultFree()
*
**************************************************/
NLM_EXTERN 
NBlastResultPtr LIBCALL
NBlastResultFree(NBlastResultPtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   AsnGenericBaseSeqOfFree(ptr -> starts ,ASNCODE_INTVAL_SLOT);
   AsnGenericBaseSeqOfFree(ptr -> lens ,ASNCODE_INTVAL_SLOT);
   AsnGenericUserSeqOfFree(ptr -> scores, (AsnOptFreeFunc) NBlastScoresFree);
   return MemFree(ptr);
}


/**************************************************
*
*    NBlastResultAsnRead()
*
**************************************************/
NLM_EXTERN 
NBlastResultPtr LIBCALL
NBlastResultAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   NBlastResultPtr ptr;

   if (! loaded)
   {
      if (! objNBlastasnAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* NBlastResult ::= (self contained) */
      atp = AsnReadId(aip, amp, NBLAST_RESULT);
   } else {
      atp = AsnLinkType(orig, NBLAST_RESULT);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   ptr = NBlastResultNew();
   if (ptr == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
      goto erret;
   }

   atp = AsnReadId(aip,amp, atp);
   func = NULL;

   if (atp == NBLAST_RESULT_numseg) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> numseg = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == NBLAST_RESULT_starts) {
      ptr -> starts = AsnGenericBaseSeqOfAsnRead(aip, amp, atp, ASNCODE_INTVAL_SLOT, &isError);
      if (isError && ptr -> starts == NULL) {
         goto erret;
      }
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == NBLAST_RESULT_lens) {
      ptr -> lens = AsnGenericBaseSeqOfAsnRead(aip, amp, atp, ASNCODE_INTVAL_SLOT, &isError);
      if (isError && ptr -> lens == NULL) {
         goto erret;
      }
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == NBLAST_RESULT_scores) {
      ptr -> scores = AsnGenericUserSeqOfAsnRead(aip, amp, atp, &isError, (AsnReadFunc) NBlastScoresAsnRead, (AsnOptFreeFunc) NBlastScoresFree);
      if (isError && ptr -> scores == NULL) {
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
   ptr = NBlastResultFree(ptr);
   goto ret;
}



/**************************************************
*
*    NBlastResultAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
NBlastResultAsnWrite(NBlastResultPtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objNBlastasnAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, NBLAST_RESULT);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) {
      goto erret;
   }

   av.intvalue = ptr -> numseg;
   retval = AsnWrite(aip, NBLAST_RESULT_numseg,  &av);
   retval = AsnGenericBaseSeqOfAsnWrite(ptr -> starts ,ASNCODE_INTVAL_SLOT, aip, NBLAST_RESULT_starts, NBLAST_RESULT_starts_E);
   retval = AsnGenericBaseSeqOfAsnWrite(ptr -> lens ,ASNCODE_INTVAL_SLOT, aip, NBLAST_RESULT_lens, NBLAST_RESULT_lens_E);
   AsnGenericUserSeqOfAsnWrite(ptr -> scores, (AsnWriteFunc) NBlastScoresAsnWrite, aip, NBLAST_RESULT_scores, NBLAST_RESULT_scores_E);
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
*    NBlastScoresNew()
*
**************************************************/
NLM_EXTERN 
NBlastScoresPtr LIBCALL
NBlastScoresNew(void)
{
   NBlastScoresPtr ptr = MemNew((size_t) sizeof(NBlastScores));

   return ptr;

}


/**************************************************
*
*    NBlastScoresFree()
*
**************************************************/
NLM_EXTERN 
NBlastScoresPtr LIBCALL
NBlastScoresFree(NBlastScoresPtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   return MemFree(ptr);
}


/**************************************************
*
*    NBlastScoresAsnRead()
*
**************************************************/
NLM_EXTERN 
NBlastScoresPtr LIBCALL
NBlastScoresAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   NBlastScoresPtr ptr;

   if (! loaded)
   {
      if (! objNBlastasnAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* NBlastScores ::= (self contained) */
      atp = AsnReadId(aip, amp, NBLAST_SCORES);
   } else {
      atp = AsnLinkType(orig, NBLAST_SCORES);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   ptr = NBlastScoresNew();
   if (ptr == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
      goto erret;
   }

   atp = AsnReadId(aip,amp, atp);
   func = NULL;

   if (atp == NBLAST_SCORES_bitscore) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> bitscore = av.realvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == NBLAST_SCORES_evalue) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> evalue = av.realvalue;
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
   ptr = NBlastScoresFree(ptr);
   goto ret;
}



/**************************************************
*
*    NBlastScoresAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
NBlastScoresAsnWrite(NBlastScoresPtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objNBlastasnAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, NBLAST_SCORES);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) {
      goto erret;
   }

   av.realvalue = ptr -> bitscore;
   retval = AsnWrite(aip, NBLAST_SCORES_bitscore,  &av);
   av.realvalue = ptr -> evalue;
   retval = AsnWrite(aip, NBLAST_SCORES_evalue,  &av);
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
*    NBlastGiAndEvalNew()
*
**************************************************/
NLM_EXTERN 
NBlastGiAndEvalPtr LIBCALL
NBlastGiAndEvalNew(void)
{
   NBlastGiAndEvalPtr ptr = MemNew((size_t) sizeof(NBlastGiAndEval));

   return ptr;

}


/**************************************************
*
*    NBlastGiAndEvalFree()
*
**************************************************/
NLM_EXTERN 
NBlastGiAndEvalPtr LIBCALL
NBlastGiAndEvalFree(NBlastGiAndEvalPtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   return MemFree(ptr);
}


/**************************************************
*
*    NBlastGiAndEvalAsnRead()
*
**************************************************/
NLM_EXTERN 
NBlastGiAndEvalPtr LIBCALL
NBlastGiAndEvalAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   NBlastGiAndEvalPtr ptr;

   if (! loaded)
   {
      if (! objNBlastasnAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* NBlastGiAndEval ::= (self contained) */
      atp = AsnReadId(aip, amp, NBLAST_GIANDEVAL);
   } else {
      atp = AsnLinkType(orig, NBLAST_GIANDEVAL);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   ptr = NBlastGiAndEvalNew();
   if (ptr == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
      goto erret;
   }

   atp = AsnReadId(aip,amp, atp);
   func = NULL;

   if (atp == NBLAST_GIANDEVAL_subject_gi) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> subject_gi = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == NBLAST_GIANDEVAL_evalue) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> evalue = av.realvalue;
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
   ptr = NBlastGiAndEvalFree(ptr);
   goto ret;
}



/**************************************************
*
*    NBlastGiAndEvalAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
NBlastGiAndEvalAsnWrite(NBlastGiAndEvalPtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objNBlastasnAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, NBLAST_GIANDEVAL);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) {
      goto erret;
   }

   av.intvalue = ptr -> subject_gi;
   retval = AsnWrite(aip, NBLAST_GIANDEVAL_subject_gi,  &av);
   av.realvalue = ptr -> evalue;
   retval = AsnWrite(aip, NBLAST_GIANDEVAL_evalue,  &av);
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
*    NBlastGiAndEvalSetNew()
*
**************************************************/
NLM_EXTERN 
NBlastGiAndEvalSetPtr LIBCALL
NBlastGiAndEvalSetNew(void)
{
   NBlastGiAndEvalSetPtr ptr = MemNew((size_t) sizeof(NBlastGiAndEvalSet));

   return ptr;

}


/**************************************************
*
*    NBlastGiAndEvalSetFree()
*
**************************************************/
NLM_EXTERN 
NBlastGiAndEvalSetPtr LIBCALL
NBlastGiAndEvalSetFree(NBlastGiAndEvalSetPtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   AsnGenericUserSeqOfFree(ptr -> giandevals, (AsnOptFreeFunc) NBlastGiAndEvalFree);
   return MemFree(ptr);
}


/**************************************************
*
*    NBlastGiAndEvalSetAsnRead()
*
**************************************************/
NLM_EXTERN 
NBlastGiAndEvalSetPtr LIBCALL
NBlastGiAndEvalSetAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   NBlastGiAndEvalSetPtr ptr;

   if (! loaded)
   {
      if (! objNBlastasnAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* NBlastGiAndEvalSet ::= (self contained) */
      atp = AsnReadId(aip, amp, NBLAST_GIANDEVAL_SET);
   } else {
      atp = AsnLinkType(orig, NBLAST_GIANDEVAL_SET);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   ptr = NBlastGiAndEvalSetNew();
   if (ptr == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
      goto erret;
   }

   atp = AsnReadId(aip,amp, atp);
   func = NULL;

   if (atp == NBLAST_GIANDEVAL_SET_query_gi) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> query_gi = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == NBLAST_GIANDEVAL_SET_giandevals) {
      ptr -> giandevals = AsnGenericUserSeqOfAsnRead(aip, amp, atp, &isError, (AsnReadFunc) NBlastGiAndEvalAsnRead, (AsnOptFreeFunc) NBlastGiAndEvalFree);
      if (isError && ptr -> giandevals == NULL) {
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
   ptr = NBlastGiAndEvalSetFree(ptr);
   goto ret;
}



/**************************************************
*
*    NBlastGiAndEvalSetAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
NBlastGiAndEvalSetAsnWrite(NBlastGiAndEvalSetPtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objNBlastasnAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, NBLAST_GIANDEVAL_SET);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) {
      goto erret;
   }

   av.intvalue = ptr -> query_gi;
   retval = AsnWrite(aip, NBLAST_GIANDEVAL_SET_query_gi,  &av);
   AsnGenericUserSeqOfAsnWrite(ptr -> giandevals, (AsnWriteFunc) NBlastGiAndEvalAsnWrite, aip, NBLAST_GIANDEVAL_SET_giandevals, NBLAST_GIANDEVAL_SET_giandevals_E);
   if (! AsnCloseStruct(aip, atp, (Pointer)ptr)) {
      goto erret;
   }
   retval = TRUE;

erret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return retval;
}

