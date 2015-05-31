#include <asn.h>

#define NLM_GENERATED_CODE_PROTO

#include </export/software/64/ncbi/include/objgen.h>
#include <objPBT.h>

static Boolean loaded = FALSE;

#include <prebind_transfer.h>

#ifndef NLM_EXTERN_LOADS
#define NLM_EXTERN_LOADS {}
#endif

NLM_EXTERN Boolean LIBCALL
objPBTAsnLoad(void)
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
*    Generated object loaders for Module PBT
*    Generated using ASNCODE Revision: 6.14 at May 31, 2004  3:13 PM
*
**************************************************/


/**************************************************
*
*    PBTInteractionSetNew()
*
**************************************************/
NLM_EXTERN 
PBTInteractionSetPtr LIBCALL
PBTInteractionSetNew(void)
{
   PBTInteractionSetPtr ptr = MemNew((size_t) sizeof(PBTInteractionSet));

   return ptr;

}


/**************************************************
*
*    PBTInteractionSetFree()
*
**************************************************/
NLM_EXTERN 
PBTInteractionSetPtr LIBCALL
PBTInteractionSetFree(PBTInteractionSetPtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   DateFree(ptr -> date);
   MemFree(ptr -> desc);
   AsnGenericUserSeqOfFree(ptr -> interactors, (AsnOptFreeFunc) PBTInteractorFree);
   AsnGenericUserSeqOfFree(ptr -> interactions, (AsnOptFreeFunc) PBTInteractionFree);
   return MemFree(ptr);
}


/**************************************************
*
*    PBTInteractionSetAsnRead()
*
**************************************************/
NLM_EXTERN 
PBTInteractionSetPtr LIBCALL
PBTInteractionSetAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   PBTInteractionSetPtr ptr;

   if (! loaded)
   {
      if (! objPBTAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* PBTInteractionSet ::= (self contained) */
      atp = AsnReadId(aip, amp, PBT_INTERACTION_SET);
   } else {
      atp = AsnLinkType(orig, PBT_INTERACTION_SET);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   ptr = PBTInteractionSetNew();
   if (ptr == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
      goto erret;
   }

   atp = AsnReadId(aip,amp, atp);
   func = NULL;

   if (atp == PBT_INTERACTION_SET_date) {
      ptr -> date = DateAsnRead(aip, atp);
      if (aip -> io_failure) {
         goto erret;
      }
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == PBT_INTERACTION_SET_desc) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> desc = av.ptrvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == PBT_INTERACTION_SET_interactors) {
      ptr -> interactors = AsnGenericUserSeqOfAsnRead(aip, amp, atp, &isError, (AsnReadFunc) PBTInteractorAsnRead, (AsnOptFreeFunc) PBTInteractorFree);
      if (isError && ptr -> interactors == NULL) {
         goto erret;
      }
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == PBT_INTERACTION_SET_interactions) {
      ptr -> interactions = AsnGenericUserSeqOfAsnRead(aip, amp, atp, &isError, (AsnReadFunc) PBTInteractionAsnRead, (AsnOptFreeFunc) PBTInteractionFree);
      if (isError && ptr -> interactions == NULL) {
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
   ptr = PBTInteractionSetFree(ptr);
   goto ret;
}



/**************************************************
*
*    PBTInteractionSetAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
PBTInteractionSetAsnWrite(PBTInteractionSetPtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objPBTAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, PBT_INTERACTION_SET);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) {
      goto erret;
   }

   if (ptr -> date != NULL) {
      if ( ! DateAsnWrite(ptr -> date, aip, PBT_INTERACTION_SET_date)) {
         goto erret;
      }
   }
   if (ptr -> desc != NULL) {
      av.ptrvalue = ptr -> desc;
      retval = AsnWrite(aip, PBT_INTERACTION_SET_desc,  &av);
   }
   AsnGenericUserSeqOfAsnWrite(ptr -> interactors, (AsnWriteFunc) PBTInteractorAsnWrite, aip, PBT_INTERACTION_SET_interactors, PBT_INTERACTION_SET_interactors_E);
   AsnGenericUserSeqOfAsnWrite(ptr -> interactions, (AsnWriteFunc) PBTInteractionAsnWrite, aip, PBT_INTERACTION_SET_interactions, PBT_INTERACTION_SET_interactions_E);
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
*    PBTInteractorNew()
*
**************************************************/
NLM_EXTERN 
PBTInteractorPtr LIBCALL
PBTInteractorNew(void)
{
   PBTInteractorPtr ptr = MemNew((size_t) sizeof(PBTInteractor));

   return ptr;

}


/**************************************************
*
*    PBTInteractorFree()
*
**************************************************/
NLM_EXTERN 
PBTInteractorPtr LIBCALL
PBTInteractorFree(PBTInteractorPtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   PBTDbrefFree(ptr -> source);
   AsnGenericUserSeqOfFree(ptr -> names, (AsnOptFreeFunc) PBTNameFree);
   AsnGenericUserSeqOfFree(ptr -> xrefs, (AsnOptFreeFunc) PBTDbrefFree);
   return MemFree(ptr);
}


/**************************************************
*
*    PBTInteractorAsnRead()
*
**************************************************/
NLM_EXTERN 
PBTInteractorPtr LIBCALL
PBTInteractorAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   PBTInteractorPtr ptr;

   if (! loaded)
   {
      if (! objPBTAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* PBTInteractor ::= (self contained) */
      atp = AsnReadId(aip, amp, PBT_INTERACTOR);
   } else {
      atp = AsnLinkType(orig, PBT_INTERACTOR);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   ptr = PBTInteractorNew();
   if (ptr == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
      goto erret;
   }

   atp = AsnReadId(aip,amp, atp);
   func = NULL;

   if (atp == PBT_INTERACTOR_uid) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> uid = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == PBT_INTERACTOR_source) {
      ptr -> source = PBTDbrefAsnRead(aip, atp);
      if (aip -> io_failure) {
         goto erret;
      }
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == PBT_INTERACTOR_type) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> type = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == PBT_INTERACTOR_taxid) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> taxid = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == PBT_INTERACTOR_names) {
      ptr -> names = AsnGenericUserSeqOfAsnRead(aip, amp, atp, &isError, (AsnReadFunc) PBTNameAsnRead, (AsnOptFreeFunc) PBTNameFree);
      if (isError && ptr -> names == NULL) {
         goto erret;
      }
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == PBT_INTERACTOR_xrefs) {
      ptr -> xrefs = AsnGenericUserSeqOfAsnRead(aip, amp, atp, &isError, (AsnReadFunc) PBTDbrefAsnRead, (AsnOptFreeFunc) PBTDbrefFree);
      if (isError && ptr -> xrefs == NULL) {
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
   ptr = PBTInteractorFree(ptr);
   goto ret;
}



/**************************************************
*
*    PBTInteractorAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
PBTInteractorAsnWrite(PBTInteractorPtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objPBTAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, PBT_INTERACTOR);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) {
      goto erret;
   }

   av.intvalue = ptr -> uid;
   retval = AsnWrite(aip, PBT_INTERACTOR_uid,  &av);
   if (ptr -> source != NULL) {
      if ( ! PBTDbrefAsnWrite(ptr -> source, aip, PBT_INTERACTOR_source)) {
         goto erret;
      }
   }
   av.intvalue = ptr -> type;
   retval = AsnWrite(aip, PBT_INTERACTOR_type,  &av);
   av.intvalue = ptr -> taxid;
   retval = AsnWrite(aip, PBT_INTERACTOR_taxid,  &av);
   AsnGenericUserSeqOfAsnWrite(ptr -> names, (AsnWriteFunc) PBTNameAsnWrite, aip, PBT_INTERACTOR_names, PBT_INTERACTOR_names_E);
   AsnGenericUserSeqOfAsnWrite(ptr -> xrefs, (AsnWriteFunc) PBTDbrefAsnWrite, aip, PBT_INTERACTOR_xrefs, PBT_INTERACTOR_xrefs_E);
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
*    PBTInteractionNew()
*
**************************************************/
NLM_EXTERN 
PBTInteractionPtr LIBCALL
PBTInteractionNew(void)
{
   PBTInteractionPtr ptr = MemNew((size_t) sizeof(PBTInteraction));

   return ptr;

}


/**************************************************
*
*    PBTInteractionFree()
*
**************************************************/
NLM_EXTERN 
PBTInteractionPtr LIBCALL
PBTInteractionFree(PBTInteractionPtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   AsnGenericUserSeqOfFree(ptr -> evidence, (AsnOptFreeFunc) PBTIntEvidenceFree);
   return MemFree(ptr);
}


/**************************************************
*
*    PBTInteractionAsnRead()
*
**************************************************/
NLM_EXTERN 
PBTInteractionPtr LIBCALL
PBTInteractionAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   PBTInteractionPtr ptr;

   if (! loaded)
   {
      if (! objPBTAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* PBTInteraction ::= (self contained) */
      atp = AsnReadId(aip, amp, PBT_INTERACTION);
   } else {
      atp = AsnLinkType(orig, PBT_INTERACTION);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   ptr = PBTInteractionNew();
   if (ptr == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
      goto erret;
   }

   atp = AsnReadId(aip,amp, atp);
   func = NULL;

   if (atp == PBT_INTERACTION_uid_a) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> uid_a = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == PBT_INTERACTION_uid_b) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> uid_b = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == PBT_INTERACTION_evidence) {
      ptr -> evidence = AsnGenericUserSeqOfAsnRead(aip, amp, atp, &isError, (AsnReadFunc) PBTIntEvidenceAsnRead, (AsnOptFreeFunc) PBTIntEvidenceFree);
      if (isError && ptr -> evidence == NULL) {
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
   ptr = PBTInteractionFree(ptr);
   goto ret;
}



/**************************************************
*
*    PBTInteractionAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
PBTInteractionAsnWrite(PBTInteractionPtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objPBTAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, PBT_INTERACTION);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) {
      goto erret;
   }

   av.intvalue = ptr -> uid_a;
   retval = AsnWrite(aip, PBT_INTERACTION_uid_a,  &av);
   av.intvalue = ptr -> uid_b;
   retval = AsnWrite(aip, PBT_INTERACTION_uid_b,  &av);
   AsnGenericUserSeqOfAsnWrite(ptr -> evidence, (AsnWriteFunc) PBTIntEvidenceAsnWrite, aip, PBT_INTERACTION_evidence, PBT_INTERACTION_evidence_E);
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
*    PBTIntEvidenceNew()
*
**************************************************/
NLM_EXTERN 
PBTIntEvidencePtr LIBCALL
PBTIntEvidenceNew(void)
{
   PBTIntEvidencePtr ptr = MemNew((size_t) sizeof(PBTIntEvidence));

   return ptr;

}


/**************************************************
*
*    PBTIntEvidenceFree()
*
**************************************************/
NLM_EXTERN 
PBTIntEvidencePtr LIBCALL
PBTIntEvidenceFree(PBTIntEvidencePtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   PBTDbrefFree(ptr -> doc);
   AsnGenericUserSeqOfFree(ptr -> names_a, (AsnOptFreeFunc) PBTNameOccurFree);
   AsnGenericUserSeqOfFree(ptr -> names_b, (AsnOptFreeFunc) PBTNameOccurFree);
   PBTScoreFree(ptr -> score);
   return MemFree(ptr);
}


/**************************************************
*
*    PBTIntEvidenceAsnRead()
*
**************************************************/
NLM_EXTERN 
PBTIntEvidencePtr LIBCALL
PBTIntEvidenceAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   PBTIntEvidencePtr ptr;

   if (! loaded)
   {
      if (! objPBTAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* PBTIntEvidence ::= (self contained) */
      atp = AsnReadId(aip, amp, PBT_INT_EVIDENCE);
   } else {
      atp = AsnLinkType(orig, PBT_INT_EVIDENCE);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   ptr = PBTIntEvidenceNew();
   if (ptr == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
      goto erret;
   }

   atp = AsnReadId(aip,amp, atp);
   func = NULL;

   if (atp == PBT_INT_EVIDENCE_doc) {
      ptr -> doc = PBTDbrefAsnRead(aip, atp);
      if (aip -> io_failure) {
         goto erret;
      }
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == PBT_INT_EVIDENCE_names_a) {
      ptr -> names_a = AsnGenericUserSeqOfAsnRead(aip, amp, atp, &isError, (AsnReadFunc) PBTNameOccurAsnRead, (AsnOptFreeFunc) PBTNameOccurFree);
      if (isError && ptr -> names_a == NULL) {
         goto erret;
      }
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == PBT_INT_EVIDENCE_names_b) {
      ptr -> names_b = AsnGenericUserSeqOfAsnRead(aip, amp, atp, &isError, (AsnReadFunc) PBTNameOccurAsnRead, (AsnOptFreeFunc) PBTNameOccurFree);
      if (isError && ptr -> names_b == NULL) {
         goto erret;
      }
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == PBT_INT_EVIDENCE_score) {
      ptr -> score = PBTScoreAsnRead(aip, atp);
      if (aip -> io_failure) {
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
   ptr = PBTIntEvidenceFree(ptr);
   goto ret;
}



/**************************************************
*
*    PBTIntEvidenceAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
PBTIntEvidenceAsnWrite(PBTIntEvidencePtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objPBTAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, PBT_INT_EVIDENCE);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) {
      goto erret;
   }

   if (ptr -> doc != NULL) {
      if ( ! PBTDbrefAsnWrite(ptr -> doc, aip, PBT_INT_EVIDENCE_doc)) {
         goto erret;
      }
   }
   AsnGenericUserSeqOfAsnWrite(ptr -> names_a, (AsnWriteFunc) PBTNameOccurAsnWrite, aip, PBT_INT_EVIDENCE_names_a, PBT_INT_EVIDENCE_names_a_E);
   AsnGenericUserSeqOfAsnWrite(ptr -> names_b, (AsnWriteFunc) PBTNameOccurAsnWrite, aip, PBT_INT_EVIDENCE_names_b, PBT_INT_EVIDENCE_names_b_E);
   if (ptr -> score != NULL) {
      if ( ! PBTScoreAsnWrite(ptr -> score, aip, PBT_INT_EVIDENCE_score)) {
         goto erret;
      }
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
*    PBTScoreNew()
*
**************************************************/
NLM_EXTERN 
PBTScorePtr LIBCALL
PBTScoreNew(void)
{
   PBTScorePtr ptr = MemNew((size_t) sizeof(PBTScore));

   return ptr;

}


/**************************************************
*
*    PBTScoreFree()
*
**************************************************/
NLM_EXTERN 
PBTScorePtr LIBCALL
PBTScoreFree(PBTScorePtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   MemFree(ptr -> hypoth);
   MemFree(ptr -> method);
   return MemFree(ptr);
}


/**************************************************
*
*    PBTScoreAsnRead()
*
**************************************************/
NLM_EXTERN 
PBTScorePtr LIBCALL
PBTScoreAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   PBTScorePtr ptr;

   if (! loaded)
   {
      if (! objPBTAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* PBTScore ::= (self contained) */
      atp = AsnReadId(aip, amp, PBT_SCORE);
   } else {
      atp = AsnLinkType(orig, PBT_SCORE);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   ptr = PBTScoreNew();
   if (ptr == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
      goto erret;
   }

   atp = AsnReadId(aip,amp, atp);
   func = NULL;

   if (atp == PBT_SCORE_hypoth) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> hypoth = av.ptrvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == PBT_SCORE_method) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> method = av.ptrvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == PBT_SCORE_score) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> score = av.realvalue;
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
   ptr = PBTScoreFree(ptr);
   goto ret;
}



/**************************************************
*
*    PBTScoreAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
PBTScoreAsnWrite(PBTScorePtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objPBTAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, PBT_SCORE);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) {
      goto erret;
   }

   if (ptr -> hypoth != NULL) {
      av.ptrvalue = ptr -> hypoth;
      retval = AsnWrite(aip, PBT_SCORE_hypoth,  &av);
   }
   if (ptr -> method != NULL) {
      av.ptrvalue = ptr -> method;
      retval = AsnWrite(aip, PBT_SCORE_method,  &av);
   }
   av.realvalue = ptr -> score;
   retval = AsnWrite(aip, PBT_SCORE_score,  &av);
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
*    PBTNameOccurNew()
*
**************************************************/
NLM_EXTERN 
PBTNameOccurPtr LIBCALL
PBTNameOccurNew(void)
{
   PBTNameOccurPtr ptr = MemNew((size_t) sizeof(PBTNameOccur));

   return ptr;

}


/**************************************************
*
*    PBTNameOccurFree()
*
**************************************************/
NLM_EXTERN 
PBTNameOccurPtr LIBCALL
PBTNameOccurFree(PBTNameOccurPtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   MemFree(ptr -> name);
   PBTScoreFree(ptr -> score);
   return MemFree(ptr);
}


/**************************************************
*
*    PBTNameOccurAsnRead()
*
**************************************************/
NLM_EXTERN 
PBTNameOccurPtr LIBCALL
PBTNameOccurAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   PBTNameOccurPtr ptr;

   if (! loaded)
   {
      if (! objPBTAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* PBTNameOccur ::= (self contained) */
      atp = AsnReadId(aip, amp, PBT_NAME_OCCUR);
   } else {
      atp = AsnLinkType(orig, PBT_NAME_OCCUR);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   ptr = PBTNameOccurNew();
   if (ptr == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
      goto erret;
   }

   atp = AsnReadId(aip,amp, atp);
   func = NULL;

   if (atp == PBT_NAME_OCCUR_name) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> name = av.ptrvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == PBT_NAME_OCCUR_score) {
      ptr -> score = PBTScoreAsnRead(aip, atp);
      if (aip -> io_failure) {
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
   ptr = PBTNameOccurFree(ptr);
   goto ret;
}



/**************************************************
*
*    PBTNameOccurAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
PBTNameOccurAsnWrite(PBTNameOccurPtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objPBTAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, PBT_NAME_OCCUR);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) {
      goto erret;
   }

   if (ptr -> name != NULL) {
      av.ptrvalue = ptr -> name;
      retval = AsnWrite(aip, PBT_NAME_OCCUR_name,  &av);
   }
   if (ptr -> score != NULL) {
      if ( ! PBTScoreAsnWrite(ptr -> score, aip, PBT_NAME_OCCUR_score)) {
         goto erret;
      }
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
*    PBTNameNew()
*
**************************************************/
NLM_EXTERN 
PBTNamePtr LIBCALL
PBTNameNew(void)
{
   PBTNamePtr ptr = MemNew((size_t) sizeof(PBTName));

   return ptr;

}


/**************************************************
*
*    PBTNameFree()
*
**************************************************/
NLM_EXTERN 
PBTNamePtr LIBCALL
PBTNameFree(PBTNamePtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   MemFree(ptr -> name);
   MemFree(ptr -> name_type);
   PBTDbrefFree(ptr -> source);
   DateFree(ptr -> date_dep);
   DateFree(ptr -> searched);
   return MemFree(ptr);
}


/**************************************************
*
*    PBTNameAsnRead()
*
**************************************************/
NLM_EXTERN 
PBTNamePtr LIBCALL
PBTNameAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   PBTNamePtr ptr;

   if (! loaded)
   {
      if (! objPBTAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* PBTName ::= (self contained) */
      atp = AsnReadId(aip, amp, PBT_NAME);
   } else {
      atp = AsnLinkType(orig, PBT_NAME);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   ptr = PBTNameNew();
   if (ptr == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
      goto erret;
   }

   atp = AsnReadId(aip,amp, atp);
   func = NULL;

   if (atp == PBT_NAME_name_id) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> name_id = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == PBT_NAME_objid) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> objid = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == PBT_NAME_name) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> name = av.ptrvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == PBT_NAME_name_type) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> name_type = av.ptrvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == PBT_NAME_source) {
      ptr -> source = PBTDbrefAsnRead(aip, atp);
      if (aip -> io_failure) {
         goto erret;
      }
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == PBT_NAME_rule_id) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> rule_id = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == PBT_NAME_deprecated) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> deprecated = av.boolvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == PBT_NAME_date_dep) {
      ptr -> date_dep = DateAsnRead(aip, atp);
      if (aip -> io_failure) {
         goto erret;
      }
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == PBT_NAME_searched) {
      ptr -> searched = DateAsnRead(aip, atp);
      if (aip -> io_failure) {
         goto erret;
      }
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == PBT_NAME_namered) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> namered = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == PBT_NAME_results) {
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
   ptr = PBTNameFree(ptr);
   goto ret;
}



/**************************************************
*
*    PBTNameAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
PBTNameAsnWrite(PBTNamePtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objPBTAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, PBT_NAME);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) {
      goto erret;
   }

   av.intvalue = ptr -> name_id;
   retval = AsnWrite(aip, PBT_NAME_name_id,  &av);
   av.intvalue = ptr -> objid;
   retval = AsnWrite(aip, PBT_NAME_objid,  &av);
   if (ptr -> name != NULL) {
      av.ptrvalue = ptr -> name;
      retval = AsnWrite(aip, PBT_NAME_name,  &av);
   }
   if (ptr -> name_type != NULL) {
      av.ptrvalue = ptr -> name_type;
      retval = AsnWrite(aip, PBT_NAME_name_type,  &av);
   }
   if (ptr -> source != NULL) {
      if ( ! PBTDbrefAsnWrite(ptr -> source, aip, PBT_NAME_source)) {
         goto erret;
      }
   }
   av.intvalue = ptr -> rule_id;
   retval = AsnWrite(aip, PBT_NAME_rule_id,  &av);
   av.boolvalue = ptr -> deprecated;
   retval = AsnWrite(aip, PBT_NAME_deprecated,  &av);
   if (ptr -> date_dep != NULL) {
      if ( ! DateAsnWrite(ptr -> date_dep, aip, PBT_NAME_date_dep)) {
         goto erret;
      }
   }
   if (ptr -> searched != NULL) {
      if ( ! DateAsnWrite(ptr -> searched, aip, PBT_NAME_searched)) {
         goto erret;
      }
   }
   av.intvalue = ptr -> namered;
   retval = AsnWrite(aip, PBT_NAME_namered,  &av);
   av.intvalue = ptr -> results;
   retval = AsnWrite(aip, PBT_NAME_results,  &av);
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
*    PBTDbrefNew()
*
**************************************************/
NLM_EXTERN 
PBTDbrefPtr LIBCALL
PBTDbrefNew(void)
{
   PBTDbrefPtr ptr = MemNew((size_t) sizeof(PBTDbref));

   return ptr;

}


/**************************************************
*
*    PBTDbrefFree()
*
**************************************************/
NLM_EXTERN 
PBTDbrefPtr LIBCALL
PBTDbrefFree(PBTDbrefPtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   MemFree(ptr -> dbname);
   MemFree(ptr -> acc);
   MemFree(ptr -> field);
   PBTScoreFree(ptr -> score);
   return MemFree(ptr);
}


/**************************************************
*
*    PBTDbrefAsnRead()
*
**************************************************/
NLM_EXTERN 
PBTDbrefPtr LIBCALL
PBTDbrefAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   PBTDbrefPtr ptr;

   if (! loaded)
   {
      if (! objPBTAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* PBTDbref ::= (self contained) */
      atp = AsnReadId(aip, amp, PBT_DBREF);
   } else {
      atp = AsnLinkType(orig, PBT_DBREF);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   ptr = PBTDbrefNew();
   if (ptr == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
      goto erret;
   }

   atp = AsnReadId(aip,amp, atp);
   func = NULL;

   if (atp == PBT_DBREF_dbname) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> dbname = av.ptrvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == PBT_DBREF_acc) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> acc = av.ptrvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == PBT_DBREF_id) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> id = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == PBT_DBREF_field) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> field = av.ptrvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == PBT_DBREF_score) {
      ptr -> score = PBTScoreAsnRead(aip, atp);
      if (aip -> io_failure) {
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
   ptr = PBTDbrefFree(ptr);
   goto ret;
}



/**************************************************
*
*    PBTDbrefAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
PBTDbrefAsnWrite(PBTDbrefPtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objPBTAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, PBT_DBREF);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) {
      goto erret;
   }

   if (ptr -> dbname != NULL) {
      av.ptrvalue = ptr -> dbname;
      retval = AsnWrite(aip, PBT_DBREF_dbname,  &av);
   }
   if (ptr -> acc != NULL) {
      av.ptrvalue = ptr -> acc;
      retval = AsnWrite(aip, PBT_DBREF_acc,  &av);
   }
   av.intvalue = ptr -> id;
   retval = AsnWrite(aip, PBT_DBREF_id,  &av);
   if (ptr -> field != NULL) {
      av.ptrvalue = ptr -> field;
      retval = AsnWrite(aip, PBT_DBREF_field,  &av);
   }
   if (ptr -> score != NULL) {
      if ( ! PBTScoreAsnWrite(ptr -> score, aip, PBT_DBREF_score)) {
         goto erret;
      }
   }
   if (! AsnCloseStruct(aip, atp, (Pointer)ptr)) {
      goto erret;
   }
   retval = TRUE;

erret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return retval;
}

