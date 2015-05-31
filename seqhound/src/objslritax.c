#include <asn.h>

#define NLM_GENERATED_CODE_PROTO

#include <objslritax.h>

static Boolean loaded = FALSE;

#include <slritax.h>

#ifndef NLM_EXTERN_LOADS
#define NLM_EXTERN_LOADS {}
#endif

NLM_EXTERN Boolean LIBCALL
objslritaxAsnLoad(void)
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
*    Generated object loaders for Module SLRI-TAXDB
*    Generated using ASNCODE Revision: 6.15 at Apr 11, 2005  5:01 PM
*
**************************************************/


/**************************************************
*
*    SLRITaxonNew()
*
**************************************************/
NLM_EXTERN 
SLRITaxonPtr LIBCALL
SLRITaxonNew(void)
{
   SLRITaxonPtr ptr = MemNew((size_t) sizeof(SLRITaxon));

   return ptr;

}


/**************************************************
*
*    SLRITaxonFree()
*
**************************************************/
NLM_EXTERN 
SLRITaxonPtr LIBCALL
SLRITaxonFree(SLRITaxonPtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   AsnGenericBaseSeqOfFree(ptr -> children_taxId ,ASNCODE_INTVAL_SLOT);
   AsnGenericUserSeqOfFree(ptr -> names, (AsnOptFreeFunc) SLRITaxonNameFree);
   SLRITaxonRankFree(ptr -> rank);
   MemFree(ptr -> embl_code);
   MemFree(ptr -> comments);
   return MemFree(ptr);
}


/**************************************************
*
*    SLRITaxonAsnRead()
*
**************************************************/
NLM_EXTERN 
SLRITaxonPtr LIBCALL
SLRITaxonAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   SLRITaxonPtr ptr;

   if (! loaded)
   {
      if (! objslritaxAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* SLRITaxon ::= (self contained) */
      atp = AsnReadId(aip, amp, SLRI_TAXON);
   } else {
      atp = AsnLinkType(orig, SLRI_TAXON);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   ptr = SLRITaxonNew();
   if (ptr == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
      goto erret;
   }

   atp = AsnReadId(aip,amp, atp);
   func = NULL;

   if (atp == SLRI_TAXON_taxId) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> taxId = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == SLRI_TAXON_parent_taxId) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> parent_taxId = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == SLRI_TAXON_children_taxId) {
      ptr -> children_taxId = AsnGenericBaseSeqOfAsnRead(aip, amp, atp, ASNCODE_INTVAL_SLOT, &isError);
      if (isError && ptr -> children_taxId == NULL) {
         goto erret;
      }
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == SLRI_TAXON_names) {
      ptr -> names = AsnGenericUserSeqOfAsnRead(aip, amp, atp, &isError, (AsnReadFunc) SLRITaxonNameAsnRead, (AsnOptFreeFunc) SLRITaxonNameFree);
      if (isError && ptr -> names == NULL) {
         goto erret;
      }
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == SLRI_TAXON_rank) {
      ptr -> rank = SLRITaxonRankAsnRead(aip, atp);
      if (aip -> io_failure) {
         goto erret;
      }
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == SLRI_TAXON_embl_code) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> embl_code = av.ptrvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == SLRI_TAXON_division) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> division = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == SLRI_TAXON_inherited_div) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> inherited_div = av.boolvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == SLRI_TAXON_gencode) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> gencode = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == SLRI_TAXON_inherited_gencode) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> inherited_gencode = av.boolvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == SLRI_TAXON_mito_gencode) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> mito_gencode = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == SLRI_TAXON_inherited_mito_gencode) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> inherited_mito_gencode = av.boolvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == SLRI_TAXON_genbank_hidden) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> genbank_hidden = av.boolvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == SLRI_TAXON_hidden_subtree_root) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> hidden_subtree_root = av.boolvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == SLRI_TAXON_comments) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> comments = av.ptrvalue;
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
   ptr = SLRITaxonFree(ptr);
   goto ret;
}



/**************************************************
*
*    SLRITaxonAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
SLRITaxonAsnWrite(SLRITaxonPtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objslritaxAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, SLRI_TAXON);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) {
      goto erret;
   }

   av.intvalue = ptr -> taxId;
   retval = AsnWrite(aip, SLRI_TAXON_taxId,  &av);
   av.intvalue = ptr -> parent_taxId;
   retval = AsnWrite(aip, SLRI_TAXON_parent_taxId,  &av);
   retval = AsnGenericBaseSeqOfAsnWrite(ptr -> children_taxId ,ASNCODE_INTVAL_SLOT, aip, SLRI_TAXON_children_taxId, SLRI_TAXON_children_taxId_E);
   AsnGenericUserSeqOfAsnWrite(ptr -> names, (AsnWriteFunc) SLRITaxonNameAsnWrite, aip, SLRI_TAXON_names, SLRI_TAXON_names_E);
   if (ptr -> rank != NULL) {
      if ( ! SLRITaxonRankAsnWrite(ptr -> rank, aip, SLRI_TAXON_rank)) {
         goto erret;
      }
   }
   if (ptr -> embl_code != NULL) {
      av.ptrvalue = ptr -> embl_code;
      retval = AsnWrite(aip, SLRI_TAXON_embl_code,  &av);
   }
   av.intvalue = ptr -> division;
   retval = AsnWrite(aip, SLRI_TAXON_division,  &av);
   av.boolvalue = ptr -> inherited_div;
   retval = AsnWrite(aip, SLRI_TAXON_inherited_div,  &av);
   av.intvalue = ptr -> gencode;
   retval = AsnWrite(aip, SLRI_TAXON_gencode,  &av);
   av.boolvalue = ptr -> inherited_gencode;
   retval = AsnWrite(aip, SLRI_TAXON_inherited_gencode,  &av);
   av.intvalue = ptr -> mito_gencode;
   retval = AsnWrite(aip, SLRI_TAXON_mito_gencode,  &av);
   av.boolvalue = ptr -> inherited_mito_gencode;
   retval = AsnWrite(aip, SLRI_TAXON_inherited_mito_gencode,  &av);
   av.boolvalue = ptr -> genbank_hidden;
   retval = AsnWrite(aip, SLRI_TAXON_genbank_hidden,  &av);
   av.boolvalue = ptr -> hidden_subtree_root;
   retval = AsnWrite(aip, SLRI_TAXON_hidden_subtree_root,  &av);
   if (ptr -> comments != NULL) {
      av.ptrvalue = ptr -> comments;
      retval = AsnWrite(aip, SLRI_TAXON_comments,  &av);
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
*    SLRITaxonDivNew()
*
**************************************************/
NLM_EXTERN 
SLRITaxonDivPtr LIBCALL
SLRITaxonDivNew(void)
{
   SLRITaxonDivPtr ptr = MemNew((size_t) sizeof(SLRITaxonDiv));

   return ptr;

}


/**************************************************
*
*    SLRITaxonDivFree()
*
**************************************************/
NLM_EXTERN 
SLRITaxonDivPtr LIBCALL
SLRITaxonDivFree(SLRITaxonDivPtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   MemFree(ptr -> div_code);
   MemFree(ptr -> div_name);
   MemFree(ptr -> comments);
   return MemFree(ptr);
}


/**************************************************
*
*    SLRITaxonDivAsnRead()
*
**************************************************/
NLM_EXTERN 
SLRITaxonDivPtr LIBCALL
SLRITaxonDivAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   SLRITaxonDivPtr ptr;

   if (! loaded)
   {
      if (! objslritaxAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* SLRITaxonDiv ::= (self contained) */
      atp = AsnReadId(aip, amp, SLRI_TAXON_DIV);
   } else {
      atp = AsnLinkType(orig, SLRI_TAXON_DIV);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   ptr = SLRITaxonDivNew();
   if (ptr == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
      goto erret;
   }

   atp = AsnReadId(aip,amp, atp);
   func = NULL;

   if (atp == SLRI_TAXON_DIV_div_id) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> div_id = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == SLRI_TAXON_DIV_div_code) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> div_code = av.ptrvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == SLRI_TAXON_DIV_div_name) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> div_name = av.ptrvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == SLRI_TAXON_DIV_comments) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> comments = av.ptrvalue;
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
   ptr = SLRITaxonDivFree(ptr);
   goto ret;
}



/**************************************************
*
*    SLRITaxonDivAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
SLRITaxonDivAsnWrite(SLRITaxonDivPtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objslritaxAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, SLRI_TAXON_DIV);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) {
      goto erret;
   }

   av.intvalue = ptr -> div_id;
   retval = AsnWrite(aip, SLRI_TAXON_DIV_div_id,  &av);
   if (ptr -> div_code != NULL) {
      av.ptrvalue = ptr -> div_code;
      retval = AsnWrite(aip, SLRI_TAXON_DIV_div_code,  &av);
   }
   if (ptr -> div_name != NULL) {
      av.ptrvalue = ptr -> div_name;
      retval = AsnWrite(aip, SLRI_TAXON_DIV_div_name,  &av);
   }
   if (ptr -> comments != NULL) {
      av.ptrvalue = ptr -> comments;
      retval = AsnWrite(aip, SLRI_TAXON_DIV_comments,  &av);
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
*    SLRITaxonGencodeNew()
*
**************************************************/
NLM_EXTERN 
SLRITaxonGencodePtr LIBCALL
SLRITaxonGencodeNew(void)
{
   SLRITaxonGencodePtr ptr = MemNew((size_t) sizeof(SLRITaxonGencode));

   return ptr;

}


/**************************************************
*
*    SLRITaxonGencodeFree()
*
**************************************************/
NLM_EXTERN 
SLRITaxonGencodePtr LIBCALL
SLRITaxonGencodeFree(SLRITaxonGencodePtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   MemFree(ptr -> abbrev);
   MemFree(ptr -> name);
   MemFree(ptr -> trans_table);
   MemFree(ptr -> start_codons);
   return MemFree(ptr);
}


/**************************************************
*
*    SLRITaxonGencodeAsnRead()
*
**************************************************/
NLM_EXTERN 
SLRITaxonGencodePtr LIBCALL
SLRITaxonGencodeAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   SLRITaxonGencodePtr ptr;

   if (! loaded)
   {
      if (! objslritaxAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* SLRITaxonGencode ::= (self contained) */
      atp = AsnReadId(aip, amp, SLRI_TAXON_GENCODE);
   } else {
      atp = AsnLinkType(orig, SLRI_TAXON_GENCODE);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   ptr = SLRITaxonGencodeNew();
   if (ptr == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
      goto erret;
   }

   atp = AsnReadId(aip,amp, atp);
   func = NULL;

   if (atp == SLRI_TAXON_GENCODE_gencode_id) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> gencode_id = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == SLRI_TAXON_GENCODE_abbrev) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> abbrev = av.ptrvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == SLRI_TAXON_GENCODE_name) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> name = av.ptrvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == SLRI_TAXON_GENCODE_trans_table) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> trans_table = av.ptrvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == SLRI_TAXON_GENCODE_start_codons) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> start_codons = av.ptrvalue;
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
   ptr = SLRITaxonGencodeFree(ptr);
   goto ret;
}



/**************************************************
*
*    SLRITaxonGencodeAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
SLRITaxonGencodeAsnWrite(SLRITaxonGencodePtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objslritaxAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, SLRI_TAXON_GENCODE);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) {
      goto erret;
   }

   av.intvalue = ptr -> gencode_id;
   retval = AsnWrite(aip, SLRI_TAXON_GENCODE_gencode_id,  &av);
   if (ptr -> abbrev != NULL) {
      av.ptrvalue = ptr -> abbrev;
      retval = AsnWrite(aip, SLRI_TAXON_GENCODE_abbrev,  &av);
   }
   if (ptr -> name != NULL) {
      av.ptrvalue = ptr -> name;
      retval = AsnWrite(aip, SLRI_TAXON_GENCODE_name,  &av);
   }
   if (ptr -> trans_table != NULL) {
      av.ptrvalue = ptr -> trans_table;
      retval = AsnWrite(aip, SLRI_TAXON_GENCODE_trans_table,  &av);
   }
   if (ptr -> start_codons != NULL) {
      av.ptrvalue = ptr -> start_codons;
      retval = AsnWrite(aip, SLRI_TAXON_GENCODE_start_codons,  &av);
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
*    SLRITaxonNameNew()
*
**************************************************/
NLM_EXTERN 
SLRITaxonNamePtr LIBCALL
SLRITaxonNameNew(void)
{
   SLRITaxonNamePtr ptr = MemNew((size_t) sizeof(SLRITaxonName));

   return ptr;

}


/**************************************************
*
*    SLRITaxonNameFree()
*
**************************************************/
NLM_EXTERN 
SLRITaxonNamePtr LIBCALL
SLRITaxonNameFree(SLRITaxonNamePtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   MemFree(ptr -> name);
   MemFree(ptr -> unique_name);
   MemFree(ptr -> other_class);
   return MemFree(ptr);
}


/**************************************************
*
*    SLRITaxonNameAsnRead()
*
**************************************************/
NLM_EXTERN 
SLRITaxonNamePtr LIBCALL
SLRITaxonNameAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   SLRITaxonNamePtr ptr;

   if (! loaded)
   {
      if (! objslritaxAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* SLRITaxonName ::= (self contained) */
      atp = AsnReadId(aip, amp, SLRI_TAXON_NAME);
   } else {
      atp = AsnLinkType(orig, SLRI_TAXON_NAME);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   ptr = SLRITaxonNameNew();
   if (ptr == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
      goto erret;
   }

   atp = AsnReadId(aip,amp, atp);
   func = NULL;

   if (atp == SLRI_TAXON_NAME_name) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> name = av.ptrvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == SLRI_TAXON_NAME_unique_name) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> unique_name = av.ptrvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == SLRI_TAXON_NAME_name_class) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> name_class = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == SLRI_TAXON_NAME_other_class) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> other_class = av.ptrvalue;
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
   ptr = SLRITaxonNameFree(ptr);
   goto ret;
}



/**************************************************
*
*    SLRITaxonNameAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
SLRITaxonNameAsnWrite(SLRITaxonNamePtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objslritaxAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, SLRI_TAXON_NAME);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) {
      goto erret;
   }

   if (ptr -> name != NULL) {
      av.ptrvalue = ptr -> name;
      retval = AsnWrite(aip, SLRI_TAXON_NAME_name,  &av);
   }
   if (ptr -> unique_name != NULL) {
      av.ptrvalue = ptr -> unique_name;
      retval = AsnWrite(aip, SLRI_TAXON_NAME_unique_name,  &av);
   }
   av.intvalue = ptr -> name_class;
   retval = AsnWrite(aip, SLRI_TAXON_NAME_name_class,  &av);
   if (ptr -> other_class != NULL) {
      av.ptrvalue = ptr -> other_class;
      retval = AsnWrite(aip, SLRI_TAXON_NAME_other_class,  &av);
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
*    SLRITaxonRankNew()
*
**************************************************/
NLM_EXTERN 
SLRITaxonRankPtr LIBCALL
SLRITaxonRankNew(void)
{
   SLRITaxonRankPtr ptr = MemNew((size_t) sizeof(SLRITaxonRank));

   ptr -> premod = 0;
   ptr -> postmod = 0;
   return ptr;

}


/**************************************************
*
*    SLRITaxonRankFree()
*
**************************************************/
NLM_EXTERN 
SLRITaxonRankPtr LIBCALL
SLRITaxonRankFree(SLRITaxonRankPtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   MemFree(ptr -> other_rank);
   MemFree(ptr -> other_premod);
   MemFree(ptr -> other_postmod);
   return MemFree(ptr);
}


/**************************************************
*
*    SLRITaxonRankAsnRead()
*
**************************************************/
NLM_EXTERN 
SLRITaxonRankPtr LIBCALL
SLRITaxonRankAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   SLRITaxonRankPtr ptr;

   if (! loaded)
   {
      if (! objslritaxAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* SLRITaxonRank ::= (self contained) */
      atp = AsnReadId(aip, amp, SLRI_TAXON_RANK);
   } else {
      atp = AsnLinkType(orig, SLRI_TAXON_RANK);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   ptr = SLRITaxonRankNew();
   if (ptr == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
      goto erret;
   }

   atp = AsnReadId(aip,amp, atp);
   func = NULL;

   if (atp == SLRI_TAXON_RANK_rank) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> rank = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == SLRI_TAXON_RANK_other_rank) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> other_rank = av.ptrvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == SLRI_TAXON_RANK_premod) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> premod = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == SLRI_TAXON_RANK_other_premod) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> other_premod = av.ptrvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == SLRI_TAXON_RANK_postmod) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> postmod = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == SLRI_TAXON_RANK_other_postmod) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> other_postmod = av.ptrvalue;
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
   ptr = SLRITaxonRankFree(ptr);
   goto ret;
}



/**************************************************
*
*    SLRITaxonRankAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
SLRITaxonRankAsnWrite(SLRITaxonRankPtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objslritaxAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, SLRI_TAXON_RANK);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) {
      goto erret;
   }

   av.intvalue = ptr -> rank;
   retval = AsnWrite(aip, SLRI_TAXON_RANK_rank,  &av);
   if (ptr -> other_rank != NULL) {
      av.ptrvalue = ptr -> other_rank;
      retval = AsnWrite(aip, SLRI_TAXON_RANK_other_rank,  &av);
   }
   av.intvalue = ptr -> premod;
   retval = AsnWrite(aip, SLRI_TAXON_RANK_premod,  &av);
   if (ptr -> other_premod != NULL) {
      av.ptrvalue = ptr -> other_premod;
      retval = AsnWrite(aip, SLRI_TAXON_RANK_other_premod,  &av);
   }
   av.intvalue = ptr -> postmod;
   retval = AsnWrite(aip, SLRI_TAXON_RANK_postmod,  &av);
   if (ptr -> other_postmod != NULL) {
      av.ptrvalue = ptr -> other_postmod;
      retval = AsnWrite(aip, SLRI_TAXON_RANK_other_postmod,  &av);
   }
   if (! AsnCloseStruct(aip, atp, (Pointer)ptr)) {
      goto erret;
   }
   retval = TRUE;

erret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return retval;
}

