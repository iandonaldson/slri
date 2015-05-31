#include <objsset.h> 
#include <mmdbapi.h> 
#include <asn.h>

#define NLM_GENERATED_CODE_PROTO

#include <objslrilibstruc.h>

static Boolean loaded = FALSE;

#include <slrilibstruc.h>

#ifndef NLM_EXTERN_LOADS
#define NLM_EXTERN_LOADS {}
#endif

NLM_EXTERN Boolean LIBCALL
objslrilibstrucAsnLoad(void)
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
*    Generated object loaders for Module SLRILIB-struc
*    Generated using ASNCODE Revision: 6.14 at Feb 11, 2003  2:33 PM
*
**************************************************/


/**************************************************
*
*    SLRISeqIdFree()
*
**************************************************/
NLM_EXTERN 
SLRISeqIdPtr LIBCALL
SLRISeqIdFree(SLRISeqIdPtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   AsnGenericChoiceSeqOfFree(ptr, (AsnOptFreeFunc) SeqIdFree);
   return NULL;
}


/**************************************************
*
*    SLRISeqIdAsnRead()
*
**************************************************/
NLM_EXTERN 
SLRISeqIdPtr LIBCALL
SLRISeqIdAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   SLRISeqIdPtr ptr;

   if (! loaded)
   {
      if (! objslrilibstrucAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* SLRISeqId ::= (self contained) */
      atp = AsnReadId(aip, amp, SLRISEQ_ID);
   } else {
      atp = AsnLinkType(orig, SLRISEQ_ID);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   func = NULL;

   ptr  = AsnGenericChoiceSeqOfAsnRead(aip, amp, atp, &isError, (AsnReadFunc) SeqIdAsnRead, (AsnOptFreeFunc) SeqIdFree);
   if (isError && ptr  == NULL) {
      goto erret;
   }



ret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return ptr;

erret:
   aip -> io_failure = TRUE;
   ptr = SLRISeqIdFree(ptr);
   goto ret;
}



/**************************************************
*
*    SLRISeqIdAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
SLRISeqIdAsnWrite(SLRISeqIdPtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objslrilibstrucAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, SLRISEQ_ID);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   retval = AsnGenericChoiceSeqOfAsnWrite(ptr , (AsnWriteFunc) SeqIdAsnWrite, aip, atp, SLRISEQ_ID_E);
   retval = TRUE;

erret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return retval;
}



/**************************************************
*
*    SLRIValNodeFree()
*
**************************************************/
NLM_EXTERN 
SLRIValNodePtr LIBCALL
SLRIValNodeFree(SLRIValNodePtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   AsnGenericChoiceSeqOfFree(ptr, (AsnOptFreeFunc) SLRINodeFree);
   return NULL;
}


/**************************************************
*
*    SLRIValNodeAsnRead()
*
**************************************************/
NLM_EXTERN 
SLRIValNodePtr LIBCALL
SLRIValNodeAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   SLRIValNodePtr ptr;

   if (! loaded)
   {
      if (! objslrilibstrucAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* SLRIValNode ::= (self contained) */
      atp = AsnReadId(aip, amp, SLRIVALNODE);
   } else {
      atp = AsnLinkType(orig, SLRIVALNODE);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   func = NULL;

   ptr  = AsnGenericChoiceSeqOfAsnRead(aip, amp, atp, &isError, (AsnReadFunc) SLRINodeAsnRead, (AsnOptFreeFunc) SLRINodeFree);
   if (isError && ptr  == NULL) {
      goto erret;
   }



ret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return ptr;

erret:
   aip -> io_failure = TRUE;
   ptr = SLRIValNodeFree(ptr);
   goto ret;
}



/**************************************************
*
*    SLRIValNodeAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
SLRIValNodeAsnWrite(SLRIValNodePtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objslrilibstrucAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, SLRIVALNODE);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   retval = AsnGenericChoiceSeqOfAsnWrite(ptr , (AsnWriteFunc) SLRINodeAsnWrite, aip, atp, SLRIVALNODE_E);
   retval = TRUE;

erret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return retval;
}



/**************************************************
*
*    SLRIFastaNew()
*
**************************************************/
NLM_EXTERN 
SLRIFastaPtr LIBCALL
SLRIFastaNew(void)
{
   SLRIFastaPtr ptr = MemNew((size_t) sizeof(SLRIFasta));

   return ptr;

}


/**************************************************
*
*    SLRIFastaFree()
*
**************************************************/
NLM_EXTERN 
SLRIFastaPtr LIBCALL
SLRIFastaFree(SLRIFastaPtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   MemFree(ptr -> defline);
   MemFree(ptr -> sequence);
   return MemFree(ptr);
}


/**************************************************
*
*    SLRIFastaAsnRead()
*
**************************************************/
NLM_EXTERN 
SLRIFastaPtr LIBCALL
SLRIFastaAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   SLRIFastaPtr ptr;

   if (! loaded)
   {
      if (! objslrilibstrucAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* SLRIFasta ::= (self contained) */
      atp = AsnReadId(aip, amp, SLRIFASTA);
   } else {
      atp = AsnLinkType(orig, SLRIFASTA);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   ptr = SLRIFastaNew();
   if (ptr == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
      goto erret;
   }

   atp = AsnReadId(aip,amp, atp);
   func = NULL;

   if (atp == SLRIFASTA_defline) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> defline = av.ptrvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == SLRIFASTA_sequence) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> sequence = av.ptrvalue;
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
   ptr = SLRIFastaFree(ptr);
   goto ret;
}



/**************************************************
*
*    SLRIFastaAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
SLRIFastaAsnWrite(SLRIFastaPtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objslrilibstrucAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, SLRIFASTA);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) {
      goto erret;
   }

   if (ptr -> defline != NULL) {
      av.ptrvalue = ptr -> defline;
      retval = AsnWrite(aip, SLRIFASTA_defline,  &av);
   }
   if (ptr -> sequence != NULL) {
      av.ptrvalue = ptr -> sequence;
      retval = AsnWrite(aip, SLRIFASTA_sequence,  &av);
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
*    SLRIDomainNew()
*
**************************************************/
NLM_EXTERN 
SLRIDomainPtr LIBCALL
SLRIDomainNew(void)
{
   SLRIDomainPtr ptr = MemNew((size_t) sizeof(SLRIDomain));

   return ptr;

}


/**************************************************
*
*    SLRIDomainFree()
*
**************************************************/
NLM_EXTERN 
SLRIDomainPtr LIBCALL
SLRIDomainFree(SLRIDomainPtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   return MemFree(ptr);
}


/**************************************************
*
*    SLRIDomainAsnRead()
*
**************************************************/
NLM_EXTERN 
SLRIDomainPtr LIBCALL
SLRIDomainAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   SLRIDomainPtr ptr;

   if (! loaded)
   {
      if (! objslrilibstrucAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* SLRIDomain ::= (self contained) */
      atp = AsnReadId(aip, amp, SLRIDOMAIN);
   } else {
      atp = AsnLinkType(orig, SLRIDOMAIN);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   ptr = SLRIDomainNew();
   if (ptr == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
      goto erret;
   }

   atp = AsnReadId(aip,amp, atp);
   func = NULL;

   if (atp == SLRIDOMAIN_gi) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> gi = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == SLRIDOMAIN_from) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> from = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == SLRIDOMAIN_to) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> to = av.intvalue;
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
   ptr = SLRIDomainFree(ptr);
   goto ret;
}



/**************************************************
*
*    SLRIDomainAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
SLRIDomainAsnWrite(SLRIDomainPtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objslrilibstrucAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, SLRIDOMAIN);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) {
      goto erret;
   }

   av.intvalue = ptr -> gi;
   retval = AsnWrite(aip, SLRIDOMAIN_gi,  &av);
   av.intvalue = ptr -> from;
   retval = AsnWrite(aip, SLRIDOMAIN_from,  &av);
   av.intvalue = ptr -> to;
   retval = AsnWrite(aip, SLRIDOMAIN_to,  &av);
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
*    SLRINodeFree()
*
**************************************************/
NLM_EXTERN 
SLRINodePtr LIBCALL
SLRINodeFree(ValNodePtr anp)
{
   Pointer pnt;

   if (anp == NULL) {
      return NULL;
   }

   pnt = anp->data.ptrvalue;
   switch (anp->choice)
   {
   default:
      break;
   case SLRINode_accession:
      MemFree(anp -> data.ptrvalue);
      break;
   case SLRINode_bioseq:
      BioseqFree(anp -> data.ptrvalue);
      break;
   case SLRINode_biostruc:
      BiostrucFree(anp -> data.ptrvalue);
      break;
   case SLRINode_fasta:
      SLRIFastaFree(anp -> data.ptrvalue);
      break;
   case SLRINode_seqid:
      SLRISeqIdFree(anp -> data.ptrvalue);
      break;
   case SLRINode_domain:
      SLRIDomainFree(anp -> data.ptrvalue);
      break;
   case SLRINode_seqalign:
      SeqAlignFree(anp -> data.ptrvalue);
      break;
   case SLRINode_bioseqset:
      BioseqSetFree(anp -> data.ptrvalue);
      break;
   case SLRINode_seqentry:
      SeqEntryFree(anp -> data.ptrvalue);
      break;
   }
   return MemFree(anp);
}


/**************************************************
*
*    SLRINodeAsnRead()
*
**************************************************/
NLM_EXTERN 
SLRINodePtr LIBCALL
SLRINodeAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   ValNodePtr anp;
   Uint1 choice;
   Boolean isError = FALSE;
   Boolean nullIsError = FALSE;
   AsnReadFunc func;

   if (! loaded)
   {
      if (! objslrilibstrucAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* SLRINode ::= (self contained) */
      atp = AsnReadId(aip, amp, SLRINODE);
   } else {
      atp = AsnLinkType(orig, SLRINODE);    /* link in local tree */
   }
   if (atp == NULL) {
      return NULL;
   }

   anp = ValNodeNew(NULL);
   if (anp == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the CHOICE or OpenStruct value (nothing) */
      goto erret;
   }

   func = NULL;

   atp = AsnReadId(aip, amp, atp);  /* find the choice */
   if (atp == NULL) {
      goto erret;
   }
   if (atp == SLRINODE_gi) {
      choice = SLRINode_gi;
      if (AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      anp->data.intvalue = av.intvalue;
   }
   else if (atp == SLRINODE_accession) {
      choice = SLRINode_accession;
      if (AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      anp->data.ptrvalue = av.ptrvalue;
   }
   else if (atp == SLRINODE_bioseq) {
      choice = SLRINode_bioseq;
      func = (AsnReadFunc) BioseqAsnRead;
   }
   else if (atp == SLRINODE_biostruc) {
      choice = SLRINode_biostruc;
      func = (AsnReadFunc) BiostrucAsnRead;
   }
   else if (atp == SLRINODE_fasta) {
      choice = SLRINode_fasta;
      func = (AsnReadFunc) SLRIFastaAsnRead;
   }
   else if (atp == SLRINODE_seqid) {
      choice = SLRINode_seqid;
      func = (AsnReadFunc) SLRISeqIdAsnRead;
   }
   else if (atp == SLRINODE_domain) {
      choice = SLRINode_domain;
      func = (AsnReadFunc) SLRIDomainAsnRead;
   }
   else if (atp == SLRINODE_seqalign) {
      choice = SLRINode_seqalign;
      func = (AsnReadFunc) SeqAlignAsnRead;
   }
   else if (atp == SLRINODE_bioseqset) {
      choice = SLRINode_bioseqset;
      func = (AsnReadFunc) BioseqSetAsnRead;
   }
   else if (atp == SLRINODE_seqentry) {
      choice = SLRINode_seqentry;
      func = (AsnReadFunc) SeqEntryAsnRead;
   }
   anp->choice = choice;
   if (func != NULL)
   {
      anp->data.ptrvalue = (* func)(aip, atp);
      if (aip -> io_failure) goto erret;

      if (nullIsError && anp->data.ptrvalue == NULL) {
         goto erret;
      }
   }

ret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return anp;

erret:
   anp = MemFree(anp);
   aip -> io_failure = TRUE;
   goto ret;
}


/**************************************************
*
*    SLRINodeAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
SLRINodeAsnWrite(SLRINodePtr anp, AsnIoPtr aip, AsnTypePtr orig)

{
   DataVal av;
   AsnTypePtr atp, writetype = NULL;
   Pointer pnt;
   AsnWriteFunc func = NULL;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objslrilibstrucAsnLoad())
      return FALSE;
   }

   if (aip == NULL)
   return FALSE;

   atp = AsnLinkType(orig, SLRINODE);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (anp == NULL) { AsnNullValueMsg(aip, atp); goto erret; }

   av.ptrvalue = (Pointer)anp;
   if (! AsnWriteChoice(aip, atp, (Int2)anp->choice, &av)) {
      goto erret;
   }

   pnt = anp->data.ptrvalue;
   switch (anp->choice)
   {
   case SLRINode_gi:
      av.intvalue = anp->data.intvalue;
      retval = AsnWrite(aip, SLRINODE_gi, &av);
      break;
   case SLRINode_accession:
      av.ptrvalue = anp->data.ptrvalue;
      retval = AsnWrite(aip, SLRINODE_accession, &av);
      break;
   case SLRINode_bioseq:
      writetype = SLRINODE_bioseq;
      func = (AsnWriteFunc) BioseqAsnWrite;
      break;
   case SLRINode_biostruc:
      writetype = SLRINODE_biostruc;
      func = (AsnWriteFunc) BiostrucAsnWrite;
      break;
   case SLRINode_fasta:
      writetype = SLRINODE_fasta;
      func = (AsnWriteFunc) SLRIFastaAsnWrite;
      break;
   case SLRINode_seqid:
      writetype = SLRINODE_seqid;
      func = (AsnWriteFunc) SLRISeqIdAsnWrite;
      break;
   case SLRINode_domain:
      writetype = SLRINODE_domain;
      func = (AsnWriteFunc) SLRIDomainAsnWrite;
      break;
   case SLRINode_seqalign:
      writetype = SLRINODE_seqalign;
      func = (AsnWriteFunc) SeqAlignAsnWrite;
      break;
   case SLRINode_bioseqset:
      writetype = SLRINODE_bioseqset;
      func = (AsnWriteFunc) BioseqSetAsnWrite;
      break;
   case SLRINode_seqentry:
      writetype = SLRINODE_seqentry;
      func = (AsnWriteFunc) SeqEntryAsnWrite;
      break;
   }
   if (writetype != NULL) {
      retval = (* func)(pnt, aip, writetype);   /* write it out */
   }
   if (!retval) {
      goto erret;
   }
   retval = TRUE;

erret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return retval;
}


/**************************************************
*
*    DTreeFree()
*
**************************************************/
NLM_EXTERN 
DTreePtr LIBCALL
DTreeFree(DTreePtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   AsnGenericUserSeqOfFree(ptr,  (AsnOptFreeFunc) DNodeFree);
   return NULL;
}


/**************************************************
*
*    DTreeAsnRead()
*
**************************************************/
NLM_EXTERN 
DTreePtr LIBCALL
DTreeAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   DTreePtr ptr;

   if (! loaded)
   {
      if (! objslrilibstrucAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* DTree ::= (self contained) */
      atp = AsnReadId(aip, amp, DTREE);
   } else {
      atp = AsnLinkType(orig, DTREE);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   func = NULL;

   ptr  = AsnGenericUserSeqOfAsnRead(aip, amp, atp, &isError, (AsnReadFunc) DNodeAsnRead, (AsnOptFreeFunc) DNodeFree);
   if (isError && ptr  == NULL) {
      goto erret;
   }



ret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return ptr;

erret:
   aip -> io_failure = TRUE;
   ptr = DTreeFree(ptr);
   goto ret;
}



/**************************************************
*
*    DTreeAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
DTreeAsnWrite(DTreePtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objslrilibstrucAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, DTREE);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   retval = AsnGenericUserSeqOfAsnWrite(ptr , (AsnWriteFunc) DNodeAsnWrite, aip, atp, DTREE_E);
   retval = TRUE;

erret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return retval;
}



/**************************************************
*
*    DNodeNew()
*
**************************************************/
NLM_EXTERN 
DNodePtr LIBCALL
DNodeNew(void)
{
   DNodePtr ptr = MemNew((size_t) sizeof(DNode));

   ptr -> state = 0;
   return ptr;

}


/**************************************************
*
*    DNodeFree()
*
**************************************************/
NLM_EXTERN 
DNodePtr LIBCALL
DNodeFree(DNodePtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   DKeyFree(ptr -> key);
   DDataFree(ptr -> data);
   AsnGenericUserSeqOfFree(ptr -> p, (AsnOptFreeFunc) DNodeFree);
   AsnGenericUserSeqOfFree(ptr -> c, (AsnOptFreeFunc) DNodeFree);
   return MemFree(ptr);
}


/**************************************************
*
*    DNodeAsnRead()
*
**************************************************/
NLM_EXTERN 
DNodePtr LIBCALL
DNodeAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   DNodePtr ptr;

   if (! loaded)
   {
      if (! objslrilibstrucAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* DNode ::= (self contained) */
      atp = AsnReadId(aip, amp, DNODE);
   } else {
      atp = AsnLinkType(orig, DNODE);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   ptr = DNodeNew();
   if (ptr == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
      goto erret;
   }

   atp = AsnReadId(aip,amp, atp);
   func = NULL;

   if (atp == DNODE_key) {
      ptr -> key = DKeyAsnRead(aip, atp);
      if (aip -> io_failure) {
         goto erret;
      }
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == DNODE_data) {
      ptr -> data = DDataAsnRead(aip, atp);
      if (aip -> io_failure) {
         goto erret;
      }
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == DNODE_num) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> num = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == DNODE_state) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> state = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == DNODE_p) {
      ptr -> p = AsnGenericUserSeqOfAsnRead(aip, amp, atp, &isError, (AsnReadFunc) DNodeAsnRead, (AsnOptFreeFunc) DNodeFree);
      if (isError && ptr -> p == NULL) {
         goto erret;
      }
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == DNODE_c) {
      ptr -> c = AsnGenericUserSeqOfAsnRead(aip, amp, atp, &isError, (AsnReadFunc) DNodeAsnRead, (AsnOptFreeFunc) DNodeFree);
      if (isError && ptr -> c == NULL) {
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
   ptr = DNodeFree(ptr);
   goto ret;
}



/**************************************************
*
*    DNodeAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
DNodeAsnWrite(DNodePtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objslrilibstrucAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, DNODE);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) {
      goto erret;
   }

   if (ptr -> key != NULL) {
      if ( ! DKeyAsnWrite(ptr -> key, aip, DNODE_key)) {
         goto erret;
      }
   }
   if (ptr -> data != NULL) {
      if ( ! DDataAsnWrite(ptr -> data, aip, DNODE_data)) {
         goto erret;
      }
   }
   av.intvalue = ptr -> num;
   retval = AsnWrite(aip, DNODE_num,  &av);
   av.intvalue = ptr -> state;
   retval = AsnWrite(aip, DNODE_state,  &av);
   AsnGenericUserSeqOfAsnWrite(ptr -> p, (AsnWriteFunc) DNodeAsnWrite, aip, DNODE_p, DNODE_p_E);
   AsnGenericUserSeqOfAsnWrite(ptr -> c, (AsnWriteFunc) DNodeAsnWrite, aip, DNODE_c, DNODE_c_E);
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
*    DKeyFree()
*
**************************************************/
NLM_EXTERN 
DKeyPtr LIBCALL
DKeyFree(ValNodePtr anp)
{
   Pointer pnt;

   if (anp == NULL) {
      return NULL;
   }

   pnt = anp->data.ptrvalue;
   switch (anp->choice)
   {
   default:
      break;
   case DKey_str:
      MemFree(anp -> data.ptrvalue);
      break;
   }
   return MemFree(anp);
}


/**************************************************
*
*    DKeyAsnRead()
*
**************************************************/
NLM_EXTERN 
DKeyPtr LIBCALL
DKeyAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   ValNodePtr anp;
   Uint1 choice;
   Boolean isError = FALSE;
   Boolean nullIsError = FALSE;
   AsnReadFunc func;

   if (! loaded)
   {
      if (! objslrilibstrucAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* DKey ::= (self contained) */
      atp = AsnReadId(aip, amp, DKEY);
   } else {
      atp = AsnLinkType(orig, DKEY);    /* link in local tree */
   }
   if (atp == NULL) {
      return NULL;
   }

   anp = ValNodeNew(NULL);
   if (anp == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the CHOICE or OpenStruct value (nothing) */
      goto erret;
   }

   func = NULL;

   atp = AsnReadId(aip, amp, atp);  /* find the choice */
   if (atp == NULL) {
      goto erret;
   }
   if (atp == DKEY_id) {
      choice = DKey_id;
      if (AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      anp->data.intvalue = av.intvalue;
   }
   else if (atp == DKEY_str) {
      choice = DKey_str;
      if (AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      anp->data.ptrvalue = av.ptrvalue;
   }
   else if (atp == DKEY_real) {
      choice = DKey_real;
      if (AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      anp->data.realvalue = av.realvalue;
   }
   anp->choice = choice;
   if (func != NULL)
   {
      anp->data.ptrvalue = (* func)(aip, atp);
      if (aip -> io_failure) goto erret;

      if (nullIsError && anp->data.ptrvalue == NULL) {
         goto erret;
      }
   }

ret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return anp;

erret:
   anp = MemFree(anp);
   aip -> io_failure = TRUE;
   goto ret;
}


/**************************************************
*
*    DKeyAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
DKeyAsnWrite(DKeyPtr anp, AsnIoPtr aip, AsnTypePtr orig)

{
   DataVal av;
   AsnTypePtr atp, writetype = NULL;
   Pointer pnt;
   AsnWriteFunc func = NULL;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objslrilibstrucAsnLoad())
      return FALSE;
   }

   if (aip == NULL)
   return FALSE;

   atp = AsnLinkType(orig, DKEY);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (anp == NULL) { AsnNullValueMsg(aip, atp); goto erret; }

   av.ptrvalue = (Pointer)anp;
   if (! AsnWriteChoice(aip, atp, (Int2)anp->choice, &av)) {
      goto erret;
   }

   pnt = anp->data.ptrvalue;
   switch (anp->choice)
   {
   case DKey_id:
      av.intvalue = anp->data.intvalue;
      retval = AsnWrite(aip, DKEY_id, &av);
      break;
   case DKey_str:
      av.ptrvalue = anp->data.ptrvalue;
      retval = AsnWrite(aip, DKEY_str, &av);
      break;
   case DKey_real:
      av.realvalue = anp->data.realvalue;
      retval = AsnWrite(aip, DKEY_real, &av);
      break;
   }
   if (writetype != NULL) {
      retval = (* func)(pnt, aip, writetype);   /* write it out */
   }
   if (!retval) {
      goto erret;
   }
   retval = TRUE;

erret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return retval;
}


/**************************************************
*
*    DDataFree()
*
**************************************************/
NLM_EXTERN 
DDataPtr LIBCALL
DDataFree(ValNodePtr anp)
{
   Pointer pnt;

   if (anp == NULL) {
      return NULL;
   }

   pnt = anp->data.ptrvalue;
   switch (anp->choice)
   {
   default:
      break;
   case DData_str:
      MemFree(anp -> data.ptrvalue);
      break;
   case DData_os:
      BSFree(anp -> data.ptrvalue);
      break;
   case DData_strs:
      AsnGenericBaseSeqOfFree((ValNodePtr) pnt,ASNCODE_PTRVAL_SLOT);
      break;
   case DData_ids:
      AsnGenericBaseSeqOfFree((ValNodePtr) pnt,ASNCODE_INTVAL_SLOT);
      break;
   case DData_reals:
      AsnGenericBaseSeqOfFree((ValNodePtr) pnt,ASNCODE_REALVAL_SLOT);
      break;
   case DData_oss:
      AsnGenericBaseSeqOfFree((ValNodePtr) pnt,ASNCODE_BYTEVAL_SLOT);
      break;
   case DData_gen_memfree:
      MemFree(anp -> data.ptrvalue);
      break;
   case DData_data_list:
      AsnGenericChoiceSeqOfFree((Pointer) pnt, (AsnOptFreeFunc) DDataFree);
      break;
   }
   return MemFree(anp);
}


/**************************************************
*
*    DDataAsnRead()
*
**************************************************/
NLM_EXTERN 
DDataPtr LIBCALL
DDataAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   ValNodePtr anp;
   Uint1 choice;
   Boolean isError = FALSE;
   Boolean nullIsError = FALSE;
   AsnReadFunc func;

   if (! loaded)
   {
      if (! objslrilibstrucAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* DData ::= (self contained) */
      atp = AsnReadId(aip, amp, DDATA);
   } else {
      atp = AsnLinkType(orig, DDATA);    /* link in local tree */
   }
   if (atp == NULL) {
      return NULL;
   }

   anp = ValNodeNew(NULL);
   if (anp == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the CHOICE or OpenStruct value (nothing) */
      goto erret;
   }

   func = NULL;

   atp = AsnReadId(aip, amp, atp);  /* find the choice */
   if (atp == NULL) {
      goto erret;
   }
   if (atp == DDATA_id) {
      choice = DData_id;
      if (AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      anp->data.intvalue = av.intvalue;
   }
   else if (atp == DDATA_str) {
      choice = DData_str;
      if (AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      anp->data.ptrvalue = av.ptrvalue;
   }
   else if (atp == DDATA_real) {
      choice = DData_real;
      if (AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      anp->data.realvalue = av.realvalue;
   }
   else if (atp == DDATA_bool) {
      choice = DData_bool;
      if (AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      anp->data.boolvalue = av.boolvalue;
   }
   else if (atp == DDATA_os) {
      choice = DData_os;
      if (AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      anp->data.ptrvalue = av.ptrvalue;
   }
   else if (atp == DDATA_strs) {
      choice = DData_strs;
      anp -> data.ptrvalue = 
      AsnGenericBaseSeqOfAsnRead(aip, amp, atp, ASNCODE_PTRVAL_SLOT, &isError);
      if (isError && anp -> data.ptrvalue == NULL) {
         goto erret;
      }
   }
   else if (atp == DDATA_ids) {
      choice = DData_ids;
      anp -> data.ptrvalue = 
      AsnGenericBaseSeqOfAsnRead(aip, amp, atp, ASNCODE_INTVAL_SLOT, &isError);
      if (isError && anp -> data.ptrvalue == NULL) {
         goto erret;
      }
   }
   else if (atp == DDATA_reals) {
      choice = DData_reals;
      anp -> data.ptrvalue = 
      AsnGenericBaseSeqOfAsnRead(aip, amp, atp, ASNCODE_REALVAL_SLOT, &isError);
      if (isError && anp -> data.ptrvalue == NULL) {
         goto erret;
      }
   }
   else if (atp == DDATA_oss) {
      choice = DData_oss;
      anp -> data.ptrvalue = 
      AsnGenericBaseSeqOfAsnRead(aip, amp, atp, ASNCODE_BYTEVAL_SLOT, &isError);
      if (isError && anp -> data.ptrvalue == NULL) {
         goto erret;
      }
   }
   else if (atp == DDATA_gen_memfree) {
      choice = DData_gen_memfree;
      if (AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      anp->data.ptrvalue = av.ptrvalue;
   }
   else if (atp == DDATA_data_list) {
      choice = DData_data_list;
      anp -> data.ptrvalue =
      AsnGenericChoiceSeqOfAsnRead(aip, amp, atp, &isError, (AsnReadFunc) DDataAsnRead,             (AsnOptFreeFunc) DDataFree);
      if (isError && anp -> data.ptrvalue == NULL) {
         goto erret;
      }
   }
   else if (atp == DDATA_user_def) {
      choice = DData_user_def;
      if (AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      anp->data.boolvalue = av.boolvalue;
   }
   anp->choice = choice;
   if (func != NULL)
   {
      anp->data.ptrvalue = (* func)(aip, atp);
      if (aip -> io_failure) goto erret;

      if (nullIsError && anp->data.ptrvalue == NULL) {
         goto erret;
      }
   }

ret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return anp;

erret:
   anp = MemFree(anp);
   aip -> io_failure = TRUE;
   goto ret;
}


/**************************************************
*
*    DDataAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
DDataAsnWrite(DDataPtr anp, AsnIoPtr aip, AsnTypePtr orig)

{
   DataVal av;
   AsnTypePtr atp, writetype = NULL;
   Pointer pnt;
   AsnWriteFunc func = NULL;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objslrilibstrucAsnLoad())
      return FALSE;
   }

   if (aip == NULL)
   return FALSE;

   atp = AsnLinkType(orig, DDATA);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (anp == NULL) { AsnNullValueMsg(aip, atp); goto erret; }

   av.ptrvalue = (Pointer)anp;
   if (! AsnWriteChoice(aip, atp, (Int2)anp->choice, &av)) {
      goto erret;
   }

   pnt = anp->data.ptrvalue;
   switch (anp->choice)
   {
   case DData_id:
      av.intvalue = anp->data.intvalue;
      retval = AsnWrite(aip, DDATA_id, &av);
      break;
   case DData_str:
      av.ptrvalue = anp->data.ptrvalue;
      retval = AsnWrite(aip, DDATA_str, &av);
      break;
   case DData_real:
      av.realvalue = anp->data.realvalue;
      retval = AsnWrite(aip, DDATA_real, &av);
      break;
   case DData_bool:
      av.boolvalue = anp->data.boolvalue;
      retval = AsnWrite(aip, DDATA_bool, &av);
      break;
   case DData_os:
      av.ptrvalue = anp->data.ptrvalue;
      retval = AsnWrite(aip, DDATA_os, &av);
      break;
   case DData_strs:
      retval = AsnGenericBaseSeqOfAsnWrite((Pointer) pnt,ASNCODE_PTRVAL_SLOT, aip, DDATA_strs, DDATA_strs_E);            break;
   case DData_ids:
      retval = AsnGenericBaseSeqOfAsnWrite((Pointer) pnt,ASNCODE_INTVAL_SLOT, aip, DDATA_ids, DDATA_ids_E);            break;
   case DData_reals:
      retval = AsnGenericBaseSeqOfAsnWrite((Pointer) pnt,ASNCODE_REALVAL_SLOT, aip, DDATA_reals, DDATA_reals_E);            break;
   case DData_oss:
      retval = AsnGenericBaseSeqOfAsnWrite((Pointer) pnt,ASNCODE_BYTEVAL_SLOT, aip, DDATA_oss, DDATA_oss_E);            break;
   case DData_gen_memfree:
      av.ptrvalue = anp->data.ptrvalue;
      retval = AsnWrite(aip, DDATA_gen_memfree, &av);
      break;
   case DData_data_list:
      retval = AsnGenericChoiceSeqOfAsnWrite((Pointer) pnt, (AsnWriteFunc) DDataAsnWrite, aip, DDATA_data_list, DDATA_data_list_E);
      break;
   case DData_user_def:
      av.boolvalue = anp->data.boolvalue;
      retval = AsnWrite(aip, DDATA_user_def, &av);
      break;
   }
   if (writetype != NULL) {
      retval = (* func)(pnt, aip, writetype);   /* write it out */
   }
   if (!retval) {
      goto erret;
   }
   retval = TRUE;

erret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return retval;
}
