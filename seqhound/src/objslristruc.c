#include <objsset.h>
#include <mmdbapi.h>
#include <objslrilibstruc.h>
#include <objslrilinkset.h>
#include <objcdd.h>
#include <asn.h>

#define NLM_GENERATED_CODE_PROTO

#include <objslristruc.h>

static Boolean loaded = FALSE;

#include <slristruc.h>

#ifndef NLM_EXTERN_LOADS
#define NLM_EXTERN_LOADS {}
#endif

NLM_EXTERN Boolean LIBCALL
objslristrucAsnLoad(void)
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
*    Generated object loaders for Module INTREZ-databases
*    Generated using ASNCODE Revision: 6.15 at Apr 29, 2005  3:05 PM
*
**************************************************/


/**************************************************
*
*    StRpsdbFree()
*
**************************************************/
NLM_EXTERN 
StRpsdbPtr LIBCALL
StRpsdbFree(StRpsdbPtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   AsnGenericUserSeqOfFree(ptr,  (AsnOptFreeFunc) StRpsNodeFree);
   return NULL;
}


/**************************************************
*
*    StRpsdbAsnRead()
*
**************************************************/
NLM_EXTERN 
StRpsdbPtr LIBCALL
StRpsdbAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   StRpsdbPtr ptr;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* StRpsdb ::= (self contained) */
      atp = AsnReadId(aip, amp, STRPSDB);
   } else {
      atp = AsnLinkType(orig, STRPSDB);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   func = NULL;

   ptr  = AsnGenericUserSeqOfAsnRead(aip, amp, atp, &isError, (AsnReadFunc) StRpsNodeAsnRead, (AsnOptFreeFunc) StRpsNodeFree);
   if (isError && ptr  == NULL) {
      goto erret;
   }



ret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return ptr;

erret:
   aip -> io_failure = TRUE;
   ptr = StRpsdbFree(ptr);
   goto ret;
}



/**************************************************
*
*    StRpsdbAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
StRpsdbAsnWrite(StRpsdbPtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, STRPSDB);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   retval = AsnGenericUserSeqOfAsnWrite(ptr , (AsnWriteFunc) StRpsNodeAsnWrite, aip, atp, STRPSDB_E);
   retval = TRUE;

erret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return retval;
}



/**************************************************
*
*    SHProteinReportSetDescriptionNew()
*
**************************************************/
NLM_EXTERN 
SHProteinReportSetDescriptionPtr LIBCALL
SHProteinReportSetDescriptionNew(void)
{
   SHProteinReportSetDescriptionPtr ptr = MemNew((size_t) sizeof(SHProteinReportSetDescription));

   return ptr;

}


/**************************************************
*
*    SHProteinReportSetDescriptionFree()
*
**************************************************/
NLM_EXTERN 
SHProteinReportSetDescriptionPtr LIBCALL
SHProteinReportSetDescriptionFree(SHProteinReportSetDescriptionPtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   MemFree(ptr -> title);
   SHProteinReportFree(ptr -> query_report);
   AsnGenericBaseSeqOfFree(ptr -> redundant_set ,ASNCODE_INTVAL_SLOT);
   SHProteinReportSetFree(ptr -> redun_reports);
   FLinkSetFree(ptr -> neighbour_set);
   SHProteinReportFree(ptr -> best_report);
   SHProteinReportSetFree(ptr -> neighbour_reports);
   return MemFree(ptr);
}


/**************************************************
*
*    SHProteinReportSetDescriptionAsnRead()
*
**************************************************/
NLM_EXTERN 
SHProteinReportSetDescriptionPtr LIBCALL
SHProteinReportSetDescriptionAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   SHProteinReportSetDescriptionPtr ptr;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* SHProteinReportSetDescription ::= (self contained) */
      atp = AsnReadId(aip, amp, SHPROTEINREPORTSETDESCRIPTION);
   } else {
      atp = AsnLinkType(orig, SHPROTEINREPORTSETDESCRIPTION);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   ptr = SHProteinReportSetDescriptionNew();
   if (ptr == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
      goto erret;
   }

   atp = AsnReadId(aip,amp, atp);
   func = NULL;

   if (atp == SHPROTEINREPORTSETDESCRIPTION_title) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> title = av.ptrvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == SHPROTEINREPORTSETDESCRIPTION_total_reports) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> total_reports = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == SHPROTEINREPORTSETDESCRIPTION_query) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> query = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == SHPROTEINREPORTSETDESCRIPTION_query_report) {
      ptr -> query_report = SHProteinReportAsnRead(aip, atp);
      if (aip -> io_failure) {
         goto erret;
      }
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == SHPROTEINREPORTSETDESCRIPTION_redundant_set) {
      ptr -> redundant_set = AsnGenericBaseSeqOfAsnRead(aip, amp, atp, ASNCODE_INTVAL_SLOT, &isError);
      if (isError && ptr -> redundant_set == NULL) {
         goto erret;
      }
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == SHPROTEINREPORTSETDESCRIPTION_redun_reports) {
      ptr -> redun_reports = SHProteinReportSetAsnRead(aip, atp);
      if (aip -> io_failure) {
         goto erret;
      }
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == SHPROTEINREPORTSETDESCRIPTION_neighbour_set) {
      ptr -> neighbour_set = FLinkSetAsnRead(aip, atp);
      if (aip -> io_failure) {
         goto erret;
      }
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == SHPROTEINREPORTSETDESCRIPTION_best_report) {
      ptr -> best_report = SHProteinReportAsnRead(aip, atp);
      if (aip -> io_failure) {
         goto erret;
      }
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == SHPROTEINREPORTSETDESCRIPTION_neighbour_reports) {
      ptr -> neighbour_reports = SHProteinReportSetAsnRead(aip, atp);
      if (aip -> io_failure) {
         goto erret;
      }
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == SHPROTEINREPORTSETDESCRIPTION_cutoff) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> cutoff = av.realvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == SHPROTEINREPORTSETDESCRIPTION_acc) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> acc = av.boolvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == SHPROTEINREPORTSETDESCRIPTION_defline) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> defline = av.boolvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == SHPROTEINREPORTSETDESCRIPTION_dna) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> dna = av.boolvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == SHPROTEINREPORTSETDESCRIPTION_taxid) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> taxid = av.boolvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == SHPROTEINREPORTSETDESCRIPTION_omim) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> omim = av.boolvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == SHPROTEINREPORTSETDESCRIPTION_muid) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> muid = av.boolvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == SHPROTEINREPORTSETDESCRIPTION_pmid) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> pmid = av.boolvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == SHPROTEINREPORTSETDESCRIPTION_mmdb_ids) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> mmdb_ids = av.boolvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == SHPROTEINREPORTSETDESCRIPTION_go) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> go = av.boolvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == SHPROTEINREPORTSETDESCRIPTION_llink) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> llink = av.boolvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == SHPROTEINREPORTSETDESCRIPTION_redund) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> redund = av.boolvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == SHPROTEINREPORTSETDESCRIPTION_neighbours) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> neighbours = av.boolvalue;
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
   ptr = SHProteinReportSetDescriptionFree(ptr);
   goto ret;
}



/**************************************************
*
*    SHProteinReportSetDescriptionAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
SHProteinReportSetDescriptionAsnWrite(SHProteinReportSetDescriptionPtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, SHPROTEINREPORTSETDESCRIPTION);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) {
      goto erret;
   }

   if (ptr -> title != NULL) {
      av.ptrvalue = ptr -> title;
      retval = AsnWrite(aip, SHPROTEINREPORTSETDESCRIPTION_title,  &av);
   }
   av.intvalue = ptr -> total_reports;
   retval = AsnWrite(aip, SHPROTEINREPORTSETDESCRIPTION_total_reports,  &av);
   av.intvalue = ptr -> query;
   retval = AsnWrite(aip, SHPROTEINREPORTSETDESCRIPTION_query,  &av);
   if (ptr -> query_report != NULL) {
      if ( ! SHProteinReportAsnWrite(ptr -> query_report, aip, SHPROTEINREPORTSETDESCRIPTION_query_report)) {
         goto erret;
      }
   }
   retval = AsnGenericBaseSeqOfAsnWrite(ptr -> redundant_set ,ASNCODE_INTVAL_SLOT, aip, SHPROTEINREPORTSETDESCRIPTION_redundant_set, SHPROTEINREPORTSETDESCRIPTION_redundant_set_E);
   if (ptr -> redun_reports != NULL) {
      if ( ! SHProteinReportSetAsnWrite(ptr -> redun_reports, aip, SHPROTEINREPORTSETDESCRIPTION_redun_reports)) {
         goto erret;
      }
   }
   if (ptr -> neighbour_set != NULL) {
      if ( ! FLinkSetAsnWrite(ptr -> neighbour_set, aip, SHPROTEINREPORTSETDESCRIPTION_neighbour_set)) {
         goto erret;
      }
   }
   if (ptr -> best_report != NULL) {
      if ( ! SHProteinReportAsnWrite(ptr -> best_report, aip, SHPROTEINREPORTSETDESCRIPTION_best_report)) {
         goto erret;
      }
   }
   if (ptr -> neighbour_reports != NULL) {
      if ( ! SHProteinReportSetAsnWrite(ptr -> neighbour_reports, aip, SHPROTEINREPORTSETDESCRIPTION_neighbour_reports)) {
         goto erret;
      }
   }
   av.realvalue = ptr -> cutoff;
   retval = AsnWrite(aip, SHPROTEINREPORTSETDESCRIPTION_cutoff,  &av);
   av.boolvalue = ptr -> acc;
   retval = AsnWrite(aip, SHPROTEINREPORTSETDESCRIPTION_acc,  &av);
   av.boolvalue = ptr -> defline;
   retval = AsnWrite(aip, SHPROTEINREPORTSETDESCRIPTION_defline,  &av);
   av.boolvalue = ptr -> dna;
   retval = AsnWrite(aip, SHPROTEINREPORTSETDESCRIPTION_dna,  &av);
   av.boolvalue = ptr -> taxid;
   retval = AsnWrite(aip, SHPROTEINREPORTSETDESCRIPTION_taxid,  &av);
   av.boolvalue = ptr -> omim;
   retval = AsnWrite(aip, SHPROTEINREPORTSETDESCRIPTION_omim,  &av);
   av.boolvalue = ptr -> muid;
   retval = AsnWrite(aip, SHPROTEINREPORTSETDESCRIPTION_muid,  &av);
   av.boolvalue = ptr -> pmid;
   retval = AsnWrite(aip, SHPROTEINREPORTSETDESCRIPTION_pmid,  &av);
   av.boolvalue = ptr -> mmdb_ids;
   retval = AsnWrite(aip, SHPROTEINREPORTSETDESCRIPTION_mmdb_ids,  &av);
   av.boolvalue = ptr -> go;
   retval = AsnWrite(aip, SHPROTEINREPORTSETDESCRIPTION_go,  &av);
   av.boolvalue = ptr -> llink;
   retval = AsnWrite(aip, SHPROTEINREPORTSETDESCRIPTION_llink,  &av);
   av.boolvalue = ptr -> redund;
   retval = AsnWrite(aip, SHPROTEINREPORTSETDESCRIPTION_redund,  &av);
   av.boolvalue = ptr -> neighbours;
   retval = AsnWrite(aip, SHPROTEINREPORTSETDESCRIPTION_neighbours,  &av);
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
*    SHProteinReportSetNew()
*
**************************************************/
NLM_EXTERN 
SHProteinReportSetPtr LIBCALL
SHProteinReportSetNew(void)
{
   SHProteinReportSetPtr ptr = MemNew((size_t) sizeof(SHProteinReportSet));

   return ptr;

}


/**************************************************
*
*    SHProteinReportSetFree()
*
**************************************************/
NLM_EXTERN 
SHProteinReportSetPtr LIBCALL
SHProteinReportSetFree(SHProteinReportSetPtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   AsnGenericUserSeqOfFree(ptr -> reports, (AsnOptFreeFunc) SHProteinReportFree);
   return MemFree(ptr);
}


/**************************************************
*
*    SHProteinReportSetAsnRead()
*
**************************************************/
NLM_EXTERN 
SHProteinReportSetPtr LIBCALL
SHProteinReportSetAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   SHProteinReportSetPtr ptr;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* SHProteinReportSet ::= (self contained) */
      atp = AsnReadId(aip, amp, SHPROTEINREPORTSET);
   } else {
      atp = AsnLinkType(orig, SHPROTEINREPORTSET);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   ptr = SHProteinReportSetNew();
   if (ptr == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
      goto erret;
   }

   atp = AsnReadId(aip,amp, atp);
   func = NULL;

   if (atp == SHPROTEINREPORTSET_number) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> number = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == SHPROTEINREPORTSET_reports) {
      ptr -> reports = AsnGenericUserSeqOfAsnRead(aip, amp, atp, &isError, (AsnReadFunc) SHProteinReportAsnRead, (AsnOptFreeFunc) SHProteinReportFree);
      if (isError && ptr -> reports == NULL) {
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
   ptr = SHProteinReportSetFree(ptr);
   goto ret;
}



/**************************************************
*
*    SHProteinReportSetAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
SHProteinReportSetAsnWrite(SHProteinReportSetPtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, SHPROTEINREPORTSET);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) {
      goto erret;
   }

   av.intvalue = ptr -> number;
   retval = AsnWrite(aip, SHPROTEINREPORTSET_number,  &av);
   AsnGenericUserSeqOfAsnWrite(ptr -> reports, (AsnWriteFunc) SHProteinReportAsnWrite, aip, SHPROTEINREPORTSET_reports, SHPROTEINREPORTSET_reports_E);
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
*    SHProteinReportNew()
*
**************************************************/
NLM_EXTERN 
SHProteinReportPtr LIBCALL
SHProteinReportNew(void)
{
   SHProteinReportPtr ptr = MemNew((size_t) sizeof(SHProteinReport));

   return ptr;

}


/**************************************************
*
*    SHProteinReportFree()
*
**************************************************/
NLM_EXTERN 
SHProteinReportPtr LIBCALL
SHProteinReportFree(SHProteinReportPtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   MemFree(ptr -> acc);
   MemFree(ptr -> defline);
   AsnGenericBaseSeqOfFree(ptr -> muids ,ASNCODE_INTVAL_SLOT);
   AsnGenericBaseSeqOfFree(ptr -> pmids ,ASNCODE_INTVAL_SLOT);
   AsnGenericUserSeqOfFree(ptr -> go, (AsnOptFreeFunc) SHGoReportFree);
   AsnGenericBaseSeqOfFree(ptr -> omim ,ASNCODE_INTVAL_SLOT);
   AsnGenericUserSeqOfFree(ptr -> llink, (AsnOptFreeFunc) SHLocusLinkReportFree);
   return MemFree(ptr);
}


/**************************************************
*
*    SHProteinReportAsnRead()
*
**************************************************/
NLM_EXTERN 
SHProteinReportPtr LIBCALL
SHProteinReportAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   SHProteinReportPtr ptr;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* SHProteinReport ::= (self contained) */
      atp = AsnReadId(aip, amp, SHPROTEINREPORT);
   } else {
      atp = AsnLinkType(orig, SHPROTEINREPORT);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   ptr = SHProteinReportNew();
   if (ptr == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
      goto erret;
   }

   atp = AsnReadId(aip,amp, atp);
   func = NULL;

   if (atp == SHPROTEINREPORT_gi) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> gi = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == SHPROTEINREPORT_acc) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> acc = av.ptrvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == SHPROTEINREPORT_defline) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> defline = av.ptrvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == SHPROTEINREPORT_dna) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> dna = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == SHPROTEINREPORT_muids) {
      ptr -> muids = AsnGenericBaseSeqOfAsnRead(aip, amp, atp, ASNCODE_INTVAL_SLOT, &isError);
      if (isError && ptr -> muids == NULL) {
         goto erret;
      }
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == SHPROTEINREPORT_pmids) {
      ptr -> pmids = AsnGenericBaseSeqOfAsnRead(aip, amp, atp, ASNCODE_INTVAL_SLOT, &isError);
      if (isError && ptr -> pmids == NULL) {
         goto erret;
      }
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == SHPROTEINREPORT_mmdbid) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> mmdbid = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == SHPROTEINREPORT_taxid) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> taxid = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == SHPROTEINREPORT_go) {
      ptr -> go = AsnGenericUserSeqOfAsnRead(aip, amp, atp, &isError, (AsnReadFunc) SHGoReportAsnRead, (AsnOptFreeFunc) SHGoReportFree);
      if (isError && ptr -> go == NULL) {
         goto erret;
      }
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == SHPROTEINREPORT_omim) {
      ptr -> omim = AsnGenericBaseSeqOfAsnRead(aip, amp, atp, ASNCODE_INTVAL_SLOT, &isError);
      if (isError && ptr -> omim == NULL) {
         goto erret;
      }
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == SHPROTEINREPORT_llink) {
      ptr -> llink = AsnGenericUserSeqOfAsnRead(aip, amp, atp, &isError, (AsnReadFunc) SHLocusLinkReportAsnRead, (AsnOptFreeFunc) SHLocusLinkReportFree);
      if (isError && ptr -> llink == NULL) {
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
   ptr = SHProteinReportFree(ptr);
   goto ret;
}



/**************************************************
*
*    SHProteinReportAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
SHProteinReportAsnWrite(SHProteinReportPtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, SHPROTEINREPORT);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) {
      goto erret;
   }

   av.intvalue = ptr -> gi;
   retval = AsnWrite(aip, SHPROTEINREPORT_gi,  &av);
   if (ptr -> acc != NULL) {
      av.ptrvalue = ptr -> acc;
      retval = AsnWrite(aip, SHPROTEINREPORT_acc,  &av);
   }
   if (ptr -> defline != NULL) {
      av.ptrvalue = ptr -> defline;
      retval = AsnWrite(aip, SHPROTEINREPORT_defline,  &av);
   }
   av.intvalue = ptr -> dna;
   retval = AsnWrite(aip, SHPROTEINREPORT_dna,  &av);
   retval = AsnGenericBaseSeqOfAsnWrite(ptr -> muids ,ASNCODE_INTVAL_SLOT, aip, SHPROTEINREPORT_muids, SHPROTEINREPORT_muids_E);
   retval = AsnGenericBaseSeqOfAsnWrite(ptr -> pmids ,ASNCODE_INTVAL_SLOT, aip, SHPROTEINREPORT_pmids, SHPROTEINREPORT_pmids_E);
   av.intvalue = ptr -> mmdbid;
   retval = AsnWrite(aip, SHPROTEINREPORT_mmdbid,  &av);
   av.intvalue = ptr -> taxid;
   retval = AsnWrite(aip, SHPROTEINREPORT_taxid,  &av);
   AsnGenericUserSeqOfAsnWrite(ptr -> go, (AsnWriteFunc) SHGoReportAsnWrite, aip, SHPROTEINREPORT_go, SHPROTEINREPORT_go_E);
   retval = AsnGenericBaseSeqOfAsnWrite(ptr -> omim ,ASNCODE_INTVAL_SLOT, aip, SHPROTEINREPORT_omim, SHPROTEINREPORT_omim_E);
   AsnGenericUserSeqOfAsnWrite(ptr -> llink, (AsnWriteFunc) SHLocusLinkReportAsnWrite, aip, SHPROTEINREPORT_llink, SHPROTEINREPORT_llink_E);
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
*    StAsndbNodeNew()
*
**************************************************/
NLM_EXTERN 
StAsndbNodePtr LIBCALL
StAsndbNodeNew(void)
{
   StAsndbNodePtr ptr = MemNew((size_t) sizeof(StAsndbNode));

   return ptr;

}


/**************************************************
*
*    StAsndbNodeFree()
*
**************************************************/
NLM_EXTERN 
StAsndbNodePtr LIBCALL
StAsndbNodeFree(StAsndbNodePtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   BioseqFree(ptr -> asn1);
   MemFree(ptr -> division);
   MemFree(ptr -> release);
   MemFree(ptr -> type);
   return MemFree(ptr);
}


/**************************************************
*
*    StAsndbNodeAsnRead()
*
**************************************************/
NLM_EXTERN 
StAsndbNodePtr LIBCALL
StAsndbNodeAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   StAsndbNodePtr ptr;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* StAsndbNode ::= (self contained) */
      atp = AsnReadId(aip, amp, STASNDBNODE);
   } else {
      atp = AsnLinkType(orig, STASNDBNODE);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   ptr = StAsndbNodeNew();
   if (ptr == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
      goto erret;
   }

   atp = AsnReadId(aip,amp, atp);
   func = NULL;

   if (atp == STASNDBNODE_gi) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> gi = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STASNDBNODE_asn1) {
      ptr -> asn1 = BioseqAsnRead(aip, atp);
      if (aip -> io_failure) {
         goto erret;
      }
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STASNDBNODE_division) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> division = av.ptrvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STASNDBNODE_release) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> release = av.ptrvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STASNDBNODE_type) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> type = av.ptrvalue;
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
   ptr = StAsndbNodeFree(ptr);
   goto ret;
}



/**************************************************
*
*    StAsndbNodeAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
StAsndbNodeAsnWrite(StAsndbNodePtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, STASNDBNODE);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) {
      goto erret;
   }

   av.intvalue = ptr -> gi;
   retval = AsnWrite(aip, STASNDBNODE_gi,  &av);
   if (ptr -> asn1 != NULL) {
      if ( ! BioseqAsnWrite(ptr -> asn1, aip, STASNDBNODE_asn1)) {
         goto erret;
      }
   }
   if (ptr -> division != NULL) {
      av.ptrvalue = ptr -> division;
      retval = AsnWrite(aip, STASNDBNODE_division,  &av);
   }
   if (ptr -> release != NULL) {
      av.ptrvalue = ptr -> release;
      retval = AsnWrite(aip, STASNDBNODE_release,  &av);
   }
   if (ptr -> type != NULL) {
      av.ptrvalue = ptr -> type;
      retval = AsnWrite(aip, STASNDBNODE_type,  &av);
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
*    StAsndbFree()
*
**************************************************/
NLM_EXTERN 
StAsndbPtr LIBCALL
StAsndbFree(StAsndbPtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   AsnGenericUserSeqOfFree(ptr,  (AsnOptFreeFunc) StAsndbNodeFree);
   return NULL;
}


/**************************************************
*
*    StAsndbAsnRead()
*
**************************************************/
NLM_EXTERN 
StAsndbPtr LIBCALL
StAsndbAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   StAsndbPtr ptr;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* StAsndb ::= (self contained) */
      atp = AsnReadId(aip, amp, STASNDB);
   } else {
      atp = AsnLinkType(orig, STASNDB);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   func = NULL;

   ptr  = AsnGenericUserSeqOfAsnRead(aip, amp, atp, &isError, (AsnReadFunc) StAsndbNodeAsnRead, (AsnOptFreeFunc) StAsndbNodeFree);
   if (isError && ptr  == NULL) {
      goto erret;
   }



ret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return ptr;

erret:
   aip -> io_failure = TRUE;
   ptr = StAsndbFree(ptr);
   goto ret;
}



/**************************************************
*
*    StAsndbAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
StAsndbAsnWrite(StAsndbPtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, STASNDB);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   retval = AsnGenericUserSeqOfAsnWrite(ptr , (AsnWriteFunc) StAsndbNodeAsnWrite, aip, atp, STASNDB_E);
   retval = TRUE;

erret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return retval;
}



/**************************************************
*
*    StPartiNodeNew()
*
**************************************************/
NLM_EXTERN 
StPartiNodePtr LIBCALL
StPartiNodeNew(void)
{
   StPartiNodePtr ptr = MemNew((size_t) sizeof(StPartiNode));

   return ptr;

}


/**************************************************
*
*    StPartiNodeFree()
*
**************************************************/
NLM_EXTERN 
StPartiNodePtr LIBCALL
StPartiNodeFree(StPartiNodePtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   MemFree(ptr -> partition);
   return MemFree(ptr);
}


/**************************************************
*
*    StPartiNodeAsnRead()
*
**************************************************/
NLM_EXTERN 
StPartiNodePtr LIBCALL
StPartiNodeAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   StPartiNodePtr ptr;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* StPartiNode ::= (self contained) */
      atp = AsnReadId(aip, amp, STPARTINODE);
   } else {
      atp = AsnLinkType(orig, STPARTINODE);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   ptr = StPartiNodeNew();
   if (ptr == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
      goto erret;
   }

   atp = AsnReadId(aip,amp, atp);
   func = NULL;

   if (atp == STPARTINODE_gi) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> gi = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STPARTINODE_partition) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> partition = av.ptrvalue;
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
   ptr = StPartiNodeFree(ptr);
   goto ret;
}



/**************************************************
*
*    StPartiNodeAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
StPartiNodeAsnWrite(StPartiNodePtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, STPARTINODE);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) {
      goto erret;
   }

   av.intvalue = ptr -> gi;
   retval = AsnWrite(aip, STPARTINODE_gi,  &av);
   if (ptr -> partition != NULL) {
      av.ptrvalue = ptr -> partition;
      retval = AsnWrite(aip, STPARTINODE_partition,  &av);
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
*    StPartiFree()
*
**************************************************/
NLM_EXTERN 
StPartiPtr LIBCALL
StPartiFree(StPartiPtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   AsnGenericUserSeqOfFree(ptr,  (AsnOptFreeFunc) StPartiNodeFree);
   return NULL;
}


/**************************************************
*
*    StPartiAsnRead()
*
**************************************************/
NLM_EXTERN 
StPartiPtr LIBCALL
StPartiAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   StPartiPtr ptr;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* StParti ::= (self contained) */
      atp = AsnReadId(aip, amp, STPARTI);
   } else {
      atp = AsnLinkType(orig, STPARTI);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   func = NULL;

   ptr  = AsnGenericUserSeqOfAsnRead(aip, amp, atp, &isError, (AsnReadFunc) StPartiNodeAsnRead, (AsnOptFreeFunc) StPartiNodeFree);
   if (isError && ptr  == NULL) {
      goto erret;
   }



ret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return ptr;

erret:
   aip -> io_failure = TRUE;
   ptr = StPartiFree(ptr);
   goto ret;
}



/**************************************************
*
*    StPartiAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
StPartiAsnWrite(StPartiPtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, STPARTI);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   retval = AsnGenericUserSeqOfAsnWrite(ptr , (AsnWriteFunc) StPartiNodeAsnWrite, aip, atp, STPARTI_E);
   retval = TRUE;

erret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return retval;
}



/**************************************************
*
*    StNucprotNodeNew()
*
**************************************************/
NLM_EXTERN 
StNucprotNodePtr LIBCALL
StNucprotNodeNew(void)
{
   StNucprotNodePtr ptr = MemNew((size_t) sizeof(StNucprotNode));

   return ptr;

}


/**************************************************
*
*    StNucprotNodeFree()
*
**************************************************/
NLM_EXTERN 
StNucprotNodePtr LIBCALL
StNucprotNodeFree(StNucprotNodePtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   return MemFree(ptr);
}


/**************************************************
*
*    StNucprotNodeAsnRead()
*
**************************************************/
NLM_EXTERN 
StNucprotNodePtr LIBCALL
StNucprotNodeAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   StNucprotNodePtr ptr;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* StNucprotNode ::= (self contained) */
      atp = AsnReadId(aip, amp, STNUCPROTNODE);
   } else {
      atp = AsnLinkType(orig, STNUCPROTNODE);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   ptr = StNucprotNodeNew();
   if (ptr == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
      goto erret;
   }

   atp = AsnReadId(aip,amp, atp);
   func = NULL;

   if (atp == STNUCPROTNODE_gin) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> gin = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STNUCPROTNODE_gia) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> gia = av.intvalue;
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
   ptr = StNucprotNodeFree(ptr);
   goto ret;
}



/**************************************************
*
*    StNucprotNodeAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
StNucprotNodeAsnWrite(StNucprotNodePtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, STNUCPROTNODE);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) {
      goto erret;
   }

   av.intvalue = ptr -> gin;
   retval = AsnWrite(aip, STNUCPROTNODE_gin,  &av);
   av.intvalue = ptr -> gia;
   retval = AsnWrite(aip, STNUCPROTNODE_gia,  &av);
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
*    StNucprotFree()
*
**************************************************/
NLM_EXTERN 
StNucprotPtr LIBCALL
StNucprotFree(StNucprotPtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   AsnGenericUserSeqOfFree(ptr,  (AsnOptFreeFunc) StNucprotNodeFree);
   return NULL;
}


/**************************************************
*
*    StNucprotAsnRead()
*
**************************************************/
NLM_EXTERN 
StNucprotPtr LIBCALL
StNucprotAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   StNucprotPtr ptr;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* StNucprot ::= (self contained) */
      atp = AsnReadId(aip, amp, STNUCPROT);
   } else {
      atp = AsnLinkType(orig, STNUCPROT);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   func = NULL;

   ptr  = AsnGenericUserSeqOfAsnRead(aip, amp, atp, &isError, (AsnReadFunc) StNucprotNodeAsnRead, (AsnOptFreeFunc) StNucprotNodeFree);
   if (isError && ptr  == NULL) {
      goto erret;
   }



ret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return ptr;

erret:
   aip -> io_failure = TRUE;
   ptr = StNucprotFree(ptr);
   goto ret;
}



/**************************************************
*
*    StNucprotAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
StNucprotAsnWrite(StNucprotPtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, STNUCPROT);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   retval = AsnGenericUserSeqOfAsnWrite(ptr , (AsnWriteFunc) StNucprotNodeAsnWrite, aip, atp, STNUCPROT_E);
   retval = TRUE;

erret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return retval;
}



/**************************************************
*
*    StAccdbNodeNew()
*
**************************************************/
NLM_EXTERN 
StAccdbNodePtr LIBCALL
StAccdbNodeNew(void)
{
   StAccdbNodePtr ptr = MemNew((size_t) sizeof(StAccdbNode));

   return ptr;

}


/**************************************************
*
*    StAccdbNodeFree()
*
**************************************************/
NLM_EXTERN 
StAccdbNodePtr LIBCALL
StAccdbNodeFree(StAccdbNodePtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   MemFree(ptr -> db);
   MemFree(ptr -> name);
   MemFree(ptr -> access);
   MemFree(ptr -> chain);
   MemFree(ptr -> release);
   MemFree(ptr -> title);
   MemFree(ptr -> namelow);
   return MemFree(ptr);
}


/**************************************************
*
*    StAccdbNodeAsnRead()
*
**************************************************/
NLM_EXTERN 
StAccdbNodePtr LIBCALL
StAccdbNodeAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   StAccdbNodePtr ptr;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* StAccdbNode ::= (self contained) */
      atp = AsnReadId(aip, amp, STACCDBNODE);
   } else {
      atp = AsnLinkType(orig, STACCDBNODE);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   ptr = StAccdbNodeNew();
   if (ptr == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
      goto erret;
   }

   atp = AsnReadId(aip,amp, atp);
   func = NULL;

   if (atp == STACCDBNODE_gi) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> gi = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STACCDBNODE_db) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> db = av.ptrvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STACCDBNODE_name) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> name = av.ptrvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STACCDBNODE_access) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> access = av.ptrvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STACCDBNODE_chain) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> chain = av.ptrvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STACCDBNODE_release) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> release = av.ptrvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STACCDBNODE_version) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> version = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STACCDBNODE_title) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> title = av.ptrvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STACCDBNODE_namelow) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> namelow = av.ptrvalue;
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
   ptr = StAccdbNodeFree(ptr);
   goto ret;
}



/**************************************************
*
*    StAccdbNodeAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
StAccdbNodeAsnWrite(StAccdbNodePtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, STACCDBNODE);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) {
      goto erret;
   }

   av.intvalue = ptr -> gi;
   retval = AsnWrite(aip, STACCDBNODE_gi,  &av);
   if (ptr -> db != NULL) {
      av.ptrvalue = ptr -> db;
      retval = AsnWrite(aip, STACCDBNODE_db,  &av);
   }
   if (ptr -> name != NULL) {
      av.ptrvalue = ptr -> name;
      retval = AsnWrite(aip, STACCDBNODE_name,  &av);
   }
   if (ptr -> access != NULL) {
      av.ptrvalue = ptr -> access;
      retval = AsnWrite(aip, STACCDBNODE_access,  &av);
   }
   if (ptr -> chain != NULL) {
      av.ptrvalue = ptr -> chain;
      retval = AsnWrite(aip, STACCDBNODE_chain,  &av);
   }
   if (ptr -> release != NULL) {
      av.ptrvalue = ptr -> release;
      retval = AsnWrite(aip, STACCDBNODE_release,  &av);
   }
   av.intvalue = ptr -> version;
   retval = AsnWrite(aip, STACCDBNODE_version,  &av);
   if (ptr -> title != NULL) {
      av.ptrvalue = ptr -> title;
      retval = AsnWrite(aip, STACCDBNODE_title,  &av);
   }
   if (ptr -> namelow != NULL) {
      av.ptrvalue = ptr -> namelow;
      retval = AsnWrite(aip, STACCDBNODE_namelow,  &av);
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
*    StAccdbFree()
*
**************************************************/
NLM_EXTERN 
StAccdbPtr LIBCALL
StAccdbFree(StAccdbPtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   AsnGenericUserSeqOfFree(ptr,  (AsnOptFreeFunc) StAccdbNodeFree);
   return NULL;
}


/**************************************************
*
*    StAccdbAsnRead()
*
**************************************************/
NLM_EXTERN 
StAccdbPtr LIBCALL
StAccdbAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   StAccdbPtr ptr;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* StAccdb ::= (self contained) */
      atp = AsnReadId(aip, amp, STACCDB);
   } else {
      atp = AsnLinkType(orig, STACCDB);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   func = NULL;

   ptr  = AsnGenericUserSeqOfAsnRead(aip, amp, atp, &isError, (AsnReadFunc) StAccdbNodeAsnRead, (AsnOptFreeFunc) StAccdbNodeFree);
   if (isError && ptr  == NULL) {
      goto erret;
   }



ret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return ptr;

erret:
   aip -> io_failure = TRUE;
   ptr = StAccdbFree(ptr);
   goto ret;
}



/**************************************************
*
*    StAccdbAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
StAccdbAsnWrite(StAccdbPtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, STACCDB);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   retval = AsnGenericUserSeqOfAsnWrite(ptr , (AsnWriteFunc) StAccdbNodeAsnWrite, aip, atp, STACCDB_E);
   retval = TRUE;

erret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return retval;
}



/**************************************************
*
*    StPubseqNodeNew()
*
**************************************************/
NLM_EXTERN 
StPubseqNodePtr LIBCALL
StPubseqNodeNew(void)
{
   StPubseqNodePtr ptr = MemNew((size_t) sizeof(StPubseqNode));

   return ptr;

}


/**************************************************
*
*    StPubseqNodeFree()
*
**************************************************/
NLM_EXTERN 
StPubseqNodePtr LIBCALL
StPubseqNodeFree(StPubseqNodePtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   return MemFree(ptr);
}


/**************************************************
*
*    StPubseqNodeAsnRead()
*
**************************************************/
NLM_EXTERN 
StPubseqNodePtr LIBCALL
StPubseqNodeAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   StPubseqNodePtr ptr;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* StPubseqNode ::= (self contained) */
      atp = AsnReadId(aip, amp, STPUBSEQNODE);
   } else {
      atp = AsnLinkType(orig, STPUBSEQNODE);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   ptr = StPubseqNodeNew();
   if (ptr == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
      goto erret;
   }

   atp = AsnReadId(aip,amp, atp);
   func = NULL;

   if (atp == STPUBSEQNODE_gi) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> gi = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STPUBSEQNODE_muid) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> muid = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STPUBSEQNODE_pmid) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> pmid = av.intvalue;
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
   ptr = StPubseqNodeFree(ptr);
   goto ret;
}



/**************************************************
*
*    StPubseqNodeAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
StPubseqNodeAsnWrite(StPubseqNodePtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, STPUBSEQNODE);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) {
      goto erret;
   }

   av.intvalue = ptr -> gi;
   retval = AsnWrite(aip, STPUBSEQNODE_gi,  &av);
   av.intvalue = ptr -> muid;
   retval = AsnWrite(aip, STPUBSEQNODE_muid,  &av);
   av.intvalue = ptr -> pmid;
   retval = AsnWrite(aip, STPUBSEQNODE_pmid,  &av);
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
*    StPubseqFree()
*
**************************************************/
NLM_EXTERN 
StPubseqPtr LIBCALL
StPubseqFree(StPubseqPtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   AsnGenericUserSeqOfFree(ptr,  (AsnOptFreeFunc) StPubseqNodeFree);
   return NULL;
}


/**************************************************
*
*    StPubseqAsnRead()
*
**************************************************/
NLM_EXTERN 
StPubseqPtr LIBCALL
StPubseqAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   StPubseqPtr ptr;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* StPubseq ::= (self contained) */
      atp = AsnReadId(aip, amp, STPUBSEQ);
   } else {
      atp = AsnLinkType(orig, STPUBSEQ);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   func = NULL;

   ptr  = AsnGenericUserSeqOfAsnRead(aip, amp, atp, &isError, (AsnReadFunc) StPubseqNodeAsnRead, (AsnOptFreeFunc) StPubseqNodeFree);
   if (isError && ptr  == NULL) {
      goto erret;
   }



ret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return ptr;

erret:
   aip -> io_failure = TRUE;
   ptr = StPubseqFree(ptr);
   goto ret;
}



/**************************************************
*
*    StPubseqAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
StPubseqAsnWrite(StPubseqPtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, STPUBSEQ);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   retval = AsnGenericUserSeqOfAsnWrite(ptr , (AsnWriteFunc) StPubseqNodeAsnWrite, aip, atp, STPUBSEQ_E);
   retval = TRUE;

erret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return retval;
}



/**************************************************
*
*    StTaxgiNodeNew()
*
**************************************************/
NLM_EXTERN 
StTaxgiNodePtr LIBCALL
StTaxgiNodeNew(void)
{
   StTaxgiNodePtr ptr = MemNew((size_t) sizeof(StTaxgiNode));

   return ptr;

}


/**************************************************
*
*    StTaxgiNodeFree()
*
**************************************************/
NLM_EXTERN 
StTaxgiNodePtr LIBCALL
StTaxgiNodeFree(StTaxgiNodePtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   MemFree(ptr -> type);
   return MemFree(ptr);
}


/**************************************************
*
*    StTaxgiNodeAsnRead()
*
**************************************************/
NLM_EXTERN 
StTaxgiNodePtr LIBCALL
StTaxgiNodeAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   StTaxgiNodePtr ptr;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* StTaxgiNode ::= (self contained) */
      atp = AsnReadId(aip, amp, STTAXGINODE);
   } else {
      atp = AsnLinkType(orig, STTAXGINODE);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   ptr = StTaxgiNodeNew();
   if (ptr == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
      goto erret;
   }

   atp = AsnReadId(aip,amp, atp);
   func = NULL;

   if (atp == STTAXGINODE_gi) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> gi = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STTAXGINODE_taxid) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> taxid = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STTAXGINODE_kloodge) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> kloodge = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STTAXGINODE_type) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> type = av.ptrvalue;
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
   ptr = StTaxgiNodeFree(ptr);
   goto ret;
}



/**************************************************
*
*    StTaxgiNodeAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
StTaxgiNodeAsnWrite(StTaxgiNodePtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, STTAXGINODE);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) {
      goto erret;
   }

   av.intvalue = ptr -> gi;
   retval = AsnWrite(aip, STTAXGINODE_gi,  &av);
   av.intvalue = ptr -> taxid;
   retval = AsnWrite(aip, STTAXGINODE_taxid,  &av);
   av.intvalue = ptr -> kloodge;
   retval = AsnWrite(aip, STTAXGINODE_kloodge,  &av);
   if (ptr -> type != NULL) {
      av.ptrvalue = ptr -> type;
      retval = AsnWrite(aip, STTAXGINODE_type,  &av);
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
*    StTaxgiFree()
*
**************************************************/
NLM_EXTERN 
StTaxgiPtr LIBCALL
StTaxgiFree(StTaxgiPtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   AsnGenericUserSeqOfFree(ptr,  (AsnOptFreeFunc) StTaxgiNodeFree);
   return NULL;
}


/**************************************************
*
*    StTaxgiAsnRead()
*
**************************************************/
NLM_EXTERN 
StTaxgiPtr LIBCALL
StTaxgiAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   StTaxgiPtr ptr;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* StTaxgi ::= (self contained) */
      atp = AsnReadId(aip, amp, STTAXGI);
   } else {
      atp = AsnLinkType(orig, STTAXGI);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   func = NULL;

   ptr  = AsnGenericUserSeqOfAsnRead(aip, amp, atp, &isError, (AsnReadFunc) StTaxgiNodeAsnRead, (AsnOptFreeFunc) StTaxgiNodeFree);
   if (isError && ptr  == NULL) {
      goto erret;
   }



ret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return ptr;

erret:
   aip -> io_failure = TRUE;
   ptr = StTaxgiFree(ptr);
   goto ret;
}



/**************************************************
*
*    StTaxgiAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
StTaxgiAsnWrite(StTaxgiPtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, STTAXGI);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   retval = AsnGenericUserSeqOfAsnWrite(ptr , (AsnWriteFunc) StTaxgiNodeAsnWrite, aip, atp, STTAXGI_E);
   retval = TRUE;

erret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return retval;
}



/**************************************************
*
*    StChromNodeNew()
*
**************************************************/
NLM_EXTERN 
StChromNodePtr LIBCALL
StChromNodeNew(void)
{
   StChromNodePtr ptr = MemNew((size_t) sizeof(StChromNode));

   return ptr;

}


/**************************************************
*
*    StChromNodeFree()
*
**************************************************/
NLM_EXTERN 
StChromNodePtr LIBCALL
StChromNodeFree(StChromNodePtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   MemFree(ptr -> access);
   MemFree(ptr -> name);
   MemFree(ptr -> chromnum);
   return MemFree(ptr);
}


/**************************************************
*
*    StChromNodeAsnRead()
*
**************************************************/
NLM_EXTERN 
StChromNodePtr LIBCALL
StChromNodeAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   StChromNodePtr ptr;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* StChromNode ::= (self contained) */
      atp = AsnReadId(aip, amp, STCHROMNODE);
   } else {
      atp = AsnLinkType(orig, STCHROMNODE);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   ptr = StChromNodeNew();
   if (ptr == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
      goto erret;
   }

   atp = AsnReadId(aip,amp, atp);
   func = NULL;

   if (atp == STCHROMNODE_kloodge) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> kloodge = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STCHROMNODE_taxid) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> taxid = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STCHROMNODE_chromid) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> chromid = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STCHROMNODE_chromfl) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> chromfl = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STCHROMNODE_access) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> access = av.ptrvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STCHROMNODE_name) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> name = av.ptrvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STCHROMNODE_projectid) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> projectid = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STCHROMNODE_chromnum) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> chromnum = av.ptrvalue;
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
   ptr = StChromNodeFree(ptr);
   goto ret;
}



/**************************************************
*
*    StChromNodeAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
StChromNodeAsnWrite(StChromNodePtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, STCHROMNODE);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) {
      goto erret;
   }

   av.intvalue = ptr -> kloodge;
   retval = AsnWrite(aip, STCHROMNODE_kloodge,  &av);
   av.intvalue = ptr -> taxid;
   retval = AsnWrite(aip, STCHROMNODE_taxid,  &av);
   av.intvalue = ptr -> chromid;
   retval = AsnWrite(aip, STCHROMNODE_chromid,  &av);
   av.intvalue = ptr -> chromfl;
   retval = AsnWrite(aip, STCHROMNODE_chromfl,  &av);
   if (ptr -> access != NULL) {
      av.ptrvalue = ptr -> access;
      retval = AsnWrite(aip, STCHROMNODE_access,  &av);
   }
   if (ptr -> name != NULL) {
      av.ptrvalue = ptr -> name;
      retval = AsnWrite(aip, STCHROMNODE_name,  &av);
   }
   av.intvalue = ptr -> projectid;
   retval = AsnWrite(aip, STCHROMNODE_projectid,  &av);
   if (ptr -> chromnum != NULL) {
      av.ptrvalue = ptr -> chromnum;
      retval = AsnWrite(aip, STCHROMNODE_chromnum,  &av);
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
*    StChromFree()
*
**************************************************/
NLM_EXTERN 
StChromPtr LIBCALL
StChromFree(StChromPtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   AsnGenericUserSeqOfFree(ptr,  (AsnOptFreeFunc) StChromNodeFree);
   return NULL;
}


/**************************************************
*
*    StChromAsnRead()
*
**************************************************/
NLM_EXTERN 
StChromPtr LIBCALL
StChromAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   StChromPtr ptr;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* StChrom ::= (self contained) */
      atp = AsnReadId(aip, amp, STCHROM);
   } else {
      atp = AsnLinkType(orig, STCHROM);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   func = NULL;

   ptr  = AsnGenericUserSeqOfAsnRead(aip, amp, atp, &isError, (AsnReadFunc) StChromNodeAsnRead, (AsnOptFreeFunc) StChromNodeFree);
   if (isError && ptr  == NULL) {
      goto erret;
   }



ret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return ptr;

erret:
   aip -> io_failure = TRUE;
   ptr = StChromFree(ptr);
   goto ret;
}



/**************************************************
*
*    StChromAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
StChromAsnWrite(StChromPtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, STCHROM);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   retval = AsnGenericUserSeqOfAsnWrite(ptr , (AsnWriteFunc) StChromNodeAsnWrite, aip, atp, STCHROM_E);
   retval = TRUE;

erret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return retval;
}



/**************************************************
*
*    StSendbNodeNew()
*
**************************************************/
NLM_EXTERN 
StSendbNodePtr LIBCALL
StSendbNodeNew(void)
{
   StSendbNodePtr ptr = MemNew((size_t) sizeof(StSendbNode));

   return ptr;

}


/**************************************************
*
*    StSendbNodeFree()
*
**************************************************/
NLM_EXTERN 
StSendbNodePtr LIBCALL
StSendbNodeFree(StSendbNodePtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   SeqEntryFree(ptr -> asn1);
   return MemFree(ptr);
}


/**************************************************
*
*    StSendbNodeAsnRead()
*
**************************************************/
NLM_EXTERN 
StSendbNodePtr LIBCALL
StSendbNodeAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   StSendbNodePtr ptr;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* StSendbNode ::= (self contained) */
      atp = AsnReadId(aip, amp, STSENDBNODE);
   } else {
      atp = AsnLinkType(orig, STSENDBNODE);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   ptr = StSendbNodeNew();
   if (ptr == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
      goto erret;
   }

   atp = AsnReadId(aip,amp, atp);
   func = NULL;

   if (atp == STSENDBNODE_seid) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> seid = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STSENDBNODE_asn1) {
      ptr -> asn1 = SeqEntryAsnRead(aip, atp);
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
   ptr = StSendbNodeFree(ptr);
   goto ret;
}



/**************************************************
*
*    StSendbNodeAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
StSendbNodeAsnWrite(StSendbNodePtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, STSENDBNODE);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) {
      goto erret;
   }

   av.intvalue = ptr -> seid;
   retval = AsnWrite(aip, STSENDBNODE_seid,  &av);
   if (ptr -> asn1 != NULL) {
      if ( ! SeqEntryAsnWrite(ptr -> asn1, aip, STSENDBNODE_asn1)) {
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
*    StSendbFree()
*
**************************************************/
NLM_EXTERN 
StSendbPtr LIBCALL
StSendbFree(StSendbPtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   AsnGenericUserSeqOfFree(ptr,  (AsnOptFreeFunc) StSendbNodeFree);
   return NULL;
}


/**************************************************
*
*    StSendbAsnRead()
*
**************************************************/
NLM_EXTERN 
StSendbPtr LIBCALL
StSendbAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   StSendbPtr ptr;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* StSendb ::= (self contained) */
      atp = AsnReadId(aip, amp, STSENDB);
   } else {
      atp = AsnLinkType(orig, STSENDB);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   func = NULL;

   ptr  = AsnGenericUserSeqOfAsnRead(aip, amp, atp, &isError, (AsnReadFunc) StSendbNodeAsnRead, (AsnOptFreeFunc) StSendbNodeFree);
   if (isError && ptr  == NULL) {
      goto erret;
   }



ret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return ptr;

erret:
   aip -> io_failure = TRUE;
   ptr = StSendbFree(ptr);
   goto ret;
}



/**************************************************
*
*    StSendbAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
StSendbAsnWrite(StSendbPtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, STSENDB);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   retval = AsnGenericUserSeqOfAsnWrite(ptr , (AsnWriteFunc) StSendbNodeAsnWrite, aip, atp, STSENDB_E);
   retval = TRUE;

erret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return retval;
}



/**************************************************
*
*    StSengiNodeNew()
*
**************************************************/
NLM_EXTERN 
StSengiNodePtr LIBCALL
StSengiNodeNew(void)
{
   StSengiNodePtr ptr = MemNew((size_t) sizeof(StSengiNode));

   return ptr;

}


/**************************************************
*
*    StSengiNodeFree()
*
**************************************************/
NLM_EXTERN 
StSengiNodePtr LIBCALL
StSengiNodeFree(StSengiNodePtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   MemFree(ptr -> division);
   return MemFree(ptr);
}


/**************************************************
*
*    StSengiNodeAsnRead()
*
**************************************************/
NLM_EXTERN 
StSengiNodePtr LIBCALL
StSengiNodeAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   StSengiNodePtr ptr;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* StSengiNode ::= (self contained) */
      atp = AsnReadId(aip, amp, STSENGINODE);
   } else {
      atp = AsnLinkType(orig, STSENGINODE);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   ptr = StSengiNodeNew();
   if (ptr == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
      goto erret;
   }

   atp = AsnReadId(aip,amp, atp);
   func = NULL;

   if (atp == STSENGINODE_seid) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> seid = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STSENGINODE_gi) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> gi = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STSENGINODE_division) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> division = av.ptrvalue;
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
   ptr = StSengiNodeFree(ptr);
   goto ret;
}



/**************************************************
*
*    StSengiNodeAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
StSengiNodeAsnWrite(StSengiNodePtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, STSENGINODE);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) {
      goto erret;
   }

   av.intvalue = ptr -> seid;
   retval = AsnWrite(aip, STSENGINODE_seid,  &av);
   av.intvalue = ptr -> gi;
   retval = AsnWrite(aip, STSENGINODE_gi,  &av);
   if (ptr -> division != NULL) {
      av.ptrvalue = ptr -> division;
      retval = AsnWrite(aip, STSENGINODE_division,  &av);
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
*    StSengiFree()
*
**************************************************/
NLM_EXTERN 
StSengiPtr LIBCALL
StSengiFree(StSengiPtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   AsnGenericUserSeqOfFree(ptr,  (AsnOptFreeFunc) StSengiNodeFree);
   return NULL;
}


/**************************************************
*
*    StSengiAsnRead()
*
**************************************************/
NLM_EXTERN 
StSengiPtr LIBCALL
StSengiAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   StSengiPtr ptr;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* StSengi ::= (self contained) */
      atp = AsnReadId(aip, amp, STSENGI);
   } else {
      atp = AsnLinkType(orig, STSENGI);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   func = NULL;

   ptr  = AsnGenericUserSeqOfAsnRead(aip, amp, atp, &isError, (AsnReadFunc) StSengiNodeAsnRead, (AsnOptFreeFunc) StSengiNodeFree);
   if (isError && ptr  == NULL) {
      goto erret;
   }



ret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return ptr;

erret:
   aip -> io_failure = TRUE;
   ptr = StSengiFree(ptr);
   goto ret;
}



/**************************************************
*
*    StSengiAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
StSengiAsnWrite(StSengiPtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, STSENGI);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   retval = AsnGenericUserSeqOfAsnWrite(ptr , (AsnWriteFunc) StSengiNodeAsnWrite, aip, atp, STSENGI_E);
   retval = TRUE;

erret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return retval;
}



/**************************************************
*
*    StMmdbNodeNew()
*
**************************************************/
NLM_EXTERN 
StMmdbNodePtr LIBCALL
StMmdbNodeNew(void)
{
   StMmdbNodePtr ptr = MemNew((size_t) sizeof(StMmdbNode));

   return ptr;

}


/**************************************************
*
*    StMmdbNodeFree()
*
**************************************************/
NLM_EXTERN 
StMmdbNodePtr LIBCALL
StMmdbNodeFree(StMmdbNodePtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   MemFree(ptr -> pdbid);
   BiostrucFree(ptr -> asn1);
   return MemFree(ptr);
}


/**************************************************
*
*    StMmdbNodeAsnRead()
*
**************************************************/
NLM_EXTERN 
StMmdbNodePtr LIBCALL
StMmdbNodeAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   StMmdbNodePtr ptr;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* StMmdbNode ::= (self contained) */
      atp = AsnReadId(aip, amp, STMMDBNODE);
   } else {
      atp = AsnLinkType(orig, STMMDBNODE);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   ptr = StMmdbNodeNew();
   if (ptr == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
      goto erret;
   }

   atp = AsnReadId(aip,amp, atp);
   func = NULL;

   if (atp == STMMDBNODE_mmdbid) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> mmdbid = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STMMDBNODE_pdbid) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> pdbid = av.ptrvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STMMDBNODE_asn1) {
      ptr -> asn1 = BiostrucAsnRead(aip, atp);
      if (aip -> io_failure) {
         goto erret;
      }
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STMMDBNODE_bwhat) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> bwhat = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STMMDBNODE_models) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> models = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STMMDBNODE_molecules) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> molecules = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STMMDBNODE_size) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> size = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STMMDBNODE_bzsize) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> bzsize = av.intvalue;
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
   ptr = StMmdbNodeFree(ptr);
   goto ret;
}



/**************************************************
*
*    StMmdbNodeAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
StMmdbNodeAsnWrite(StMmdbNodePtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, STMMDBNODE);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) {
      goto erret;
   }

   av.intvalue = ptr -> mmdbid;
   retval = AsnWrite(aip, STMMDBNODE_mmdbid,  &av);
   if (ptr -> pdbid != NULL) {
      av.ptrvalue = ptr -> pdbid;
      retval = AsnWrite(aip, STMMDBNODE_pdbid,  &av);
   }
   if (ptr -> asn1 != NULL) {
      if ( ! BiostrucAsnWrite(ptr -> asn1, aip, STMMDBNODE_asn1)) {
         goto erret;
      }
   }
   av.intvalue = ptr -> bwhat;
   retval = AsnWrite(aip, STMMDBNODE_bwhat,  &av);
   av.intvalue = ptr -> models;
   retval = AsnWrite(aip, STMMDBNODE_models,  &av);
   av.intvalue = ptr -> molecules;
   retval = AsnWrite(aip, STMMDBNODE_molecules,  &av);
   av.intvalue = ptr -> size;
   retval = AsnWrite(aip, STMMDBNODE_size,  &av);
   av.intvalue = ptr -> bzsize;
   retval = AsnWrite(aip, STMMDBNODE_bzsize,  &av);
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
*    StMmdbFree()
*
**************************************************/
NLM_EXTERN 
StMmdbPtr LIBCALL
StMmdbFree(StMmdbPtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   AsnGenericUserSeqOfFree(ptr,  (AsnOptFreeFunc) StMmdbNodeFree);
   return NULL;
}


/**************************************************
*
*    StMmdbAsnRead()
*
**************************************************/
NLM_EXTERN 
StMmdbPtr LIBCALL
StMmdbAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   StMmdbPtr ptr;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* StMmdb ::= (self contained) */
      atp = AsnReadId(aip, amp, STMMDB);
   } else {
      atp = AsnLinkType(orig, STMMDB);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   func = NULL;

   ptr  = AsnGenericUserSeqOfAsnRead(aip, amp, atp, &isError, (AsnReadFunc) StMmdbNodeAsnRead, (AsnOptFreeFunc) StMmdbNodeFree);
   if (isError && ptr  == NULL) {
      goto erret;
   }



ret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return ptr;

erret:
   aip -> io_failure = TRUE;
   ptr = StMmdbFree(ptr);
   goto ret;
}



/**************************************************
*
*    StMmdbAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
StMmdbAsnWrite(StMmdbPtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, STMMDB);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   retval = AsnGenericUserSeqOfAsnWrite(ptr , (AsnWriteFunc) StMmdbNodeAsnWrite, aip, atp, STMMDB_E);
   retval = TRUE;

erret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return retval;
}



/**************************************************
*
*    StMmgiNodeNew()
*
**************************************************/
NLM_EXTERN 
StMmgiNodePtr LIBCALL
StMmgiNodeNew(void)
{
   StMmgiNodePtr ptr = MemNew((size_t) sizeof(StMmgiNode));

   return ptr;

}


/**************************************************
*
*    StMmgiNodeFree()
*
**************************************************/
NLM_EXTERN 
StMmgiNodePtr LIBCALL
StMmgiNodeFree(StMmgiNodePtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   return MemFree(ptr);
}


/**************************************************
*
*    StMmgiNodeAsnRead()
*
**************************************************/
NLM_EXTERN 
StMmgiNodePtr LIBCALL
StMmgiNodeAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   StMmgiNodePtr ptr;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* StMmgiNode ::= (self contained) */
      atp = AsnReadId(aip, amp, STMMGINODE);
   } else {
      atp = AsnLinkType(orig, STMMGINODE);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   ptr = StMmgiNodeNew();
   if (ptr == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
      goto erret;
   }

   atp = AsnReadId(aip,amp, atp);
   func = NULL;

   if (atp == STMMGINODE_mmdbid) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> mmdbid = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STMMGINODE_gi) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> gi = av.intvalue;
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
   ptr = StMmgiNodeFree(ptr);
   goto ret;
}



/**************************************************
*
*    StMmgiNodeAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
StMmgiNodeAsnWrite(StMmgiNodePtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, STMMGINODE);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) {
      goto erret;
   }

   av.intvalue = ptr -> mmdbid;
   retval = AsnWrite(aip, STMMGINODE_mmdbid,  &av);
   av.intvalue = ptr -> gi;
   retval = AsnWrite(aip, STMMGINODE_gi,  &av);
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
*    StMmgiFree()
*
**************************************************/
NLM_EXTERN 
StMmgiPtr LIBCALL
StMmgiFree(StMmgiPtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   AsnGenericUserSeqOfFree(ptr,  (AsnOptFreeFunc) StMmgiNodeFree);
   return NULL;
}


/**************************************************
*
*    StMmgiAsnRead()
*
**************************************************/
NLM_EXTERN 
StMmgiPtr LIBCALL
StMmgiAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   StMmgiPtr ptr;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* StMmgi ::= (self contained) */
      atp = AsnReadId(aip, amp, STMMGI);
   } else {
      atp = AsnLinkType(orig, STMMGI);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   func = NULL;

   ptr  = AsnGenericUserSeqOfAsnRead(aip, amp, atp, &isError, (AsnReadFunc) StMmgiNodeAsnRead, (AsnOptFreeFunc) StMmgiNodeFree);
   if (isError && ptr  == NULL) {
      goto erret;
   }



ret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return ptr;

erret:
   aip -> io_failure = TRUE;
   ptr = StMmgiFree(ptr);
   goto ret;
}



/**************************************************
*
*    StMmgiAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
StMmgiAsnWrite(StMmgiPtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, STMMGI);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   retval = AsnGenericUserSeqOfAsnWrite(ptr , (AsnWriteFunc) StMmgiNodeAsnWrite, aip, atp, STMMGI_E);
   retval = TRUE;

erret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return retval;
}



/**************************************************
*
*    StRedundNodeNew()
*
**************************************************/
NLM_EXTERN 
StRedundNodePtr LIBCALL
StRedundNodeNew(void)
{
   StRedundNodePtr ptr = MemNew((size_t) sizeof(StRedundNode));

   return ptr;

}


/**************************************************
*
*    StRedundNodeFree()
*
**************************************************/
NLM_EXTERN 
StRedundNodePtr LIBCALL
StRedundNodeFree(StRedundNodePtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   return MemFree(ptr);
}


/**************************************************
*
*    StRedundNodeAsnRead()
*
**************************************************/
NLM_EXTERN 
StRedundNodePtr LIBCALL
StRedundNodeAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   StRedundNodePtr ptr;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* StRedundNode ::= (self contained) */
      atp = AsnReadId(aip, amp, STREDUNDNODE);
   } else {
      atp = AsnLinkType(orig, STREDUNDNODE);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   ptr = StRedundNodeNew();
   if (ptr == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
      goto erret;
   }

   atp = AsnReadId(aip,amp, atp);
   func = NULL;

   if (atp == STREDUNDNODE_gi) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> gi = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STREDUNDNODE_ordinal) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> ordinal = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STREDUNDNODE_group) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> group = av.intvalue;
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
   ptr = StRedundNodeFree(ptr);
   goto ret;
}



/**************************************************
*
*    StRedundNodeAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
StRedundNodeAsnWrite(StRedundNodePtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, STREDUNDNODE);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) {
      goto erret;
   }

   av.intvalue = ptr -> gi;
   retval = AsnWrite(aip, STREDUNDNODE_gi,  &av);
   av.intvalue = ptr -> ordinal;
   retval = AsnWrite(aip, STREDUNDNODE_ordinal,  &av);
   av.intvalue = ptr -> group;
   retval = AsnWrite(aip, STREDUNDNODE_group,  &av);
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
*    StRedundFree()
*
**************************************************/
NLM_EXTERN 
StRedundPtr LIBCALL
StRedundFree(StRedundPtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   AsnGenericUserSeqOfFree(ptr,  (AsnOptFreeFunc) StRedundNodeFree);
   return NULL;
}


/**************************************************
*
*    StRedundAsnRead()
*
**************************************************/
NLM_EXTERN 
StRedundPtr LIBCALL
StRedundAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   StRedundPtr ptr;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* StRedund ::= (self contained) */
      atp = AsnReadId(aip, amp, STREDUND);
   } else {
      atp = AsnLinkType(orig, STREDUND);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   func = NULL;

   ptr  = AsnGenericUserSeqOfAsnRead(aip, amp, atp, &isError, (AsnReadFunc) StRedundNodeAsnRead, (AsnOptFreeFunc) StRedundNodeFree);
   if (isError && ptr  == NULL) {
      goto erret;
   }



ret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return ptr;

erret:
   aip -> io_failure = TRUE;
   ptr = StRedundFree(ptr);
   goto ret;
}



/**************************************************
*
*    StRedundAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
StRedundAsnWrite(StRedundPtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, STREDUND);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   retval = AsnGenericUserSeqOfAsnWrite(ptr , (AsnWriteFunc) StRedundNodeAsnWrite, aip, atp, STREDUND_E);
   retval = TRUE;

erret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return retval;
}



/**************************************************
*
*    StDomdbNodeNew()
*
**************************************************/
NLM_EXTERN 
StDomdbNodePtr LIBCALL
StDomdbNodeNew(void)
{
   StDomdbNodePtr ptr = MemNew((size_t) sizeof(StDomdbNode));

   return ptr;

}


/**************************************************
*
*    StDomdbNodeFree()
*
**************************************************/
NLM_EXTERN 
StDomdbNodePtr LIBCALL
StDomdbNodeFree(StDomdbNodePtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   SLRIValNodeFree(ptr -> asn1);
   MemFree(ptr -> pdbid);
   MemFree(ptr -> chain);
   return MemFree(ptr);
}


/**************************************************
*
*    StDomdbNodeAsnRead()
*
**************************************************/
NLM_EXTERN 
StDomdbNodePtr LIBCALL
StDomdbNodeAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   StDomdbNodePtr ptr;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* StDomdbNode ::= (self contained) */
      atp = AsnReadId(aip, amp, STDOMDBNODE);
   } else {
      atp = AsnLinkType(orig, STDOMDBNODE);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   ptr = StDomdbNodeNew();
   if (ptr == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
      goto erret;
   }

   atp = AsnReadId(aip,amp, atp);
   func = NULL;

   if (atp == STDOMDBNODE_mmdbid) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> mmdbid = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STDOMDBNODE_asn1) {
      ptr -> asn1 = SLRIValNodeAsnRead(aip, atp);
      if (aip -> io_failure) {
         goto erret;
      }
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STDOMDBNODE_pdbid) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> pdbid = av.ptrvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STDOMDBNODE_chain) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> chain = av.ptrvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STDOMDBNODE_gi) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> gi = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STDOMDBNODE_domno) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> domno = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STDOMDBNODE_domall) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> domall = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STDOMDBNODE_domid) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> domid = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STDOMDBNODE_rep) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> rep = av.intvalue;
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
   ptr = StDomdbNodeFree(ptr);
   goto ret;
}



/**************************************************
*
*    StDomdbNodeAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
StDomdbNodeAsnWrite(StDomdbNodePtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, STDOMDBNODE);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) {
      goto erret;
   }

   av.intvalue = ptr -> mmdbid;
   retval = AsnWrite(aip, STDOMDBNODE_mmdbid,  &av);
   if (ptr -> asn1 != NULL) {
      if ( ! SLRIValNodeAsnWrite(ptr -> asn1, aip, STDOMDBNODE_asn1)) {
         goto erret;
      }
   }
   if (ptr -> pdbid != NULL) {
      av.ptrvalue = ptr -> pdbid;
      retval = AsnWrite(aip, STDOMDBNODE_pdbid,  &av);
   }
   if (ptr -> chain != NULL) {
      av.ptrvalue = ptr -> chain;
      retval = AsnWrite(aip, STDOMDBNODE_chain,  &av);
   }
   av.intvalue = ptr -> gi;
   retval = AsnWrite(aip, STDOMDBNODE_gi,  &av);
   av.intvalue = ptr -> domno;
   retval = AsnWrite(aip, STDOMDBNODE_domno,  &av);
   av.intvalue = ptr -> domall;
   retval = AsnWrite(aip, STDOMDBNODE_domall,  &av);
   av.intvalue = ptr -> domid;
   retval = AsnWrite(aip, STDOMDBNODE_domid,  &av);
   av.intvalue = ptr -> rep;
   retval = AsnWrite(aip, STDOMDBNODE_rep,  &av);
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
*    StDomdbFree()
*
**************************************************/
NLM_EXTERN 
StDomdbPtr LIBCALL
StDomdbFree(StDomdbPtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   AsnGenericUserSeqOfFree(ptr,  (AsnOptFreeFunc) StDomdbNodeFree);
   return NULL;
}


/**************************************************
*
*    StDomdbAsnRead()
*
**************************************************/
NLM_EXTERN 
StDomdbPtr LIBCALL
StDomdbAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   StDomdbPtr ptr;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* StDomdb ::= (self contained) */
      atp = AsnReadId(aip, amp, STDOMDB);
   } else {
      atp = AsnLinkType(orig, STDOMDB);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   func = NULL;

   ptr  = AsnGenericUserSeqOfAsnRead(aip, amp, atp, &isError, (AsnReadFunc) StDomdbNodeAsnRead, (AsnOptFreeFunc) StDomdbNodeFree);
   if (isError && ptr  == NULL) {
      goto erret;
   }



ret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return ptr;

erret:
   aip -> io_failure = TRUE;
   ptr = StDomdbFree(ptr);
   goto ret;
}



/**************************************************
*
*    StDomdbAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
StDomdbAsnWrite(StDomdbPtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, STDOMDB);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   retval = AsnGenericUserSeqOfAsnWrite(ptr , (AsnWriteFunc) StDomdbNodeAsnWrite, aip, atp, STDOMDB_E);
   retval = TRUE;

erret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return retval;
}



/**************************************************
*
*    StCddbNodeNew()
*
**************************************************/
NLM_EXTERN 
StCddbNodePtr LIBCALL
StCddbNodeNew(void)
{
   StCddbNodePtr ptr = MemNew((size_t) sizeof(StCddbNode));

   return ptr;

}


/**************************************************
*
*    StCddbNodeFree()
*
**************************************************/
NLM_EXTERN 
StCddbNodePtr LIBCALL
StCddbNodeFree(StCddbNodePtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   SLRIFastaFree(ptr -> asn1);
   return MemFree(ptr);
}


/**************************************************
*
*    StCddbNodeAsnRead()
*
**************************************************/
NLM_EXTERN 
StCddbNodePtr LIBCALL
StCddbNodeAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   StCddbNodePtr ptr;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* StCddbNode ::= (self contained) */
      atp = AsnReadId(aip, amp, STCDDBNODE);
   } else {
      atp = AsnLinkType(orig, STCDDBNODE);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   ptr = StCddbNodeNew();
   if (ptr == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
      goto erret;
   }

   atp = AsnReadId(aip,amp, atp);
   func = NULL;

   if (atp == STCDDBNODE_gi) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> gi = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STCDDBNODE_asn1) {
      ptr -> asn1 = SLRIFastaAsnRead(aip, atp);
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
   ptr = StCddbNodeFree(ptr);
   goto ret;
}



/**************************************************
*
*    StCddbNodeAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
StCddbNodeAsnWrite(StCddbNodePtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, STCDDBNODE);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) {
      goto erret;
   }

   av.intvalue = ptr -> gi;
   retval = AsnWrite(aip, STCDDBNODE_gi,  &av);
   if (ptr -> asn1 != NULL) {
      if ( ! SLRIFastaAsnWrite(ptr -> asn1, aip, STCDDBNODE_asn1)) {
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
*    StCddbFree()
*
**************************************************/
NLM_EXTERN 
StCddbPtr LIBCALL
StCddbFree(StCddbPtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   AsnGenericUserSeqOfFree(ptr,  (AsnOptFreeFunc) StCddbNodeFree);
   return NULL;
}


/**************************************************
*
*    StCddbAsnRead()
*
**************************************************/
NLM_EXTERN 
StCddbPtr LIBCALL
StCddbAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   StCddbPtr ptr;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* StCddb ::= (self contained) */
      atp = AsnReadId(aip, amp, STCDDB);
   } else {
      atp = AsnLinkType(orig, STCDDB);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   func = NULL;

   ptr  = AsnGenericUserSeqOfAsnRead(aip, amp, atp, &isError, (AsnReadFunc) StCddbNodeAsnRead, (AsnOptFreeFunc) StCddbNodeFree);
   if (isError && ptr  == NULL) {
      goto erret;
   }



ret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return ptr;

erret:
   aip -> io_failure = TRUE;
   ptr = StCddbFree(ptr);
   goto ret;
}



/**************************************************
*
*    StCddbAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
StCddbAsnWrite(StCddbPtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, STCDDB);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   retval = AsnGenericUserSeqOfAsnWrite(ptr , (AsnWriteFunc) StCddbNodeAsnWrite, aip, atp, STCDDB_E);
   retval = TRUE;

erret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return retval;
}



/**************************************************
*
*    StRpsNodeNew()
*
**************************************************/
NLM_EXTERN 
StRpsNodePtr LIBCALL
StRpsNodeNew(void)
{
   StRpsNodePtr ptr = MemNew((size_t) sizeof(StRpsNode));

   return ptr;

}


/**************************************************
*
*    StRpsNodeFree()
*
**************************************************/
NLM_EXTERN 
StRpsNodePtr LIBCALL
StRpsNodeFree(StRpsNodePtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   MemFree(ptr -> dom_id);
   return MemFree(ptr);
}


/**************************************************
*
*    StRpsNodeAsnRead()
*
**************************************************/
NLM_EXTERN 
StRpsNodePtr LIBCALL
StRpsNodeAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   StRpsNodePtr ptr;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* StRpsNode ::= (self contained) */
      atp = AsnReadId(aip, amp, STRPSNODE);
   } else {
      atp = AsnLinkType(orig, STRPSNODE);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   ptr = StRpsNodeNew();
   if (ptr == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
      goto erret;
   }

   atp = AsnReadId(aip,amp, atp);
   func = NULL;

   if (atp == STRPSNODE_gi) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> gi = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STRPSNODE_dom_id) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> dom_id = av.ptrvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STRPSNODE_cdd_id) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> cdd_id = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STRPSNODE_from) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> from = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STRPSNODE_len) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> len = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STRPSNODE_score) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> score = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STRPSNODE_evalue) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> evalue = av.realvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STRPSNODE_bitscore) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> bitscore = av.realvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STRPSNODE_n_missing) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> n_missing = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STRPSNODE_c_missing) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> c_missing = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STRPSNODE_num_of_dom) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> num_of_dom = av.intvalue;
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
   ptr = StRpsNodeFree(ptr);
   goto ret;
}



/**************************************************
*
*    StRpsNodeAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
StRpsNodeAsnWrite(StRpsNodePtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, STRPSNODE);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) {
      goto erret;
   }

   av.intvalue = ptr -> gi;
   retval = AsnWrite(aip, STRPSNODE_gi,  &av);
   if (ptr -> dom_id != NULL) {
      av.ptrvalue = ptr -> dom_id;
      retval = AsnWrite(aip, STRPSNODE_dom_id,  &av);
   }
   av.intvalue = ptr -> cdd_id;
   retval = AsnWrite(aip, STRPSNODE_cdd_id,  &av);
   av.intvalue = ptr -> from;
   retval = AsnWrite(aip, STRPSNODE_from,  &av);
   av.intvalue = ptr -> len;
   retval = AsnWrite(aip, STRPSNODE_len,  &av);
   av.intvalue = ptr -> score;
   retval = AsnWrite(aip, STRPSNODE_score,  &av);
   av.realvalue = ptr -> evalue;
   retval = AsnWrite(aip, STRPSNODE_evalue,  &av);
   av.realvalue = ptr -> bitscore;
   retval = AsnWrite(aip, STRPSNODE_bitscore,  &av);
   av.intvalue = ptr -> n_missing;
   retval = AsnWrite(aip, STRPSNODE_n_missing,  &av);
   av.intvalue = ptr -> c_missing;
   retval = AsnWrite(aip, STRPSNODE_c_missing,  &av);
   av.intvalue = ptr -> num_of_dom;
   retval = AsnWrite(aip, STRPSNODE_num_of_dom,  &av);
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
*    StDomNameNodeNew()
*
**************************************************/
NLM_EXTERN 
StDomNameNodePtr LIBCALL
StDomNameNodeNew(void)
{
   StDomNameNodePtr ptr = MemNew((size_t) sizeof(StDomNameNode));

   return ptr;

}


/**************************************************
*
*    StDomNameNodeFree()
*
**************************************************/
NLM_EXTERN 
StDomNameNodePtr LIBCALL
StDomNameNodeFree(StDomNameNodePtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   MemFree(ptr -> accession);
   MemFree(ptr -> name);
   MemFree(ptr -> description);
   MemFree(ptr -> pdb_id);
   return MemFree(ptr);
}


/**************************************************
*
*    StDomNameNodeAsnRead()
*
**************************************************/
NLM_EXTERN 
StDomNameNodePtr LIBCALL
StDomNameNodeAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   StDomNameNodePtr ptr;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* StDomNameNode ::= (self contained) */
      atp = AsnReadId(aip, amp, STDOMNAMENODE);
   } else {
      atp = AsnLinkType(orig, STDOMNAMENODE);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   ptr = StDomNameNodeNew();
   if (ptr == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
      goto erret;
   }

   atp = AsnReadId(aip,amp, atp);
   func = NULL;

   if (atp == STDOMNAMENODE_pssmid) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> pssmid = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STDOMNAMENODE_accession) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> accession = av.ptrvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STDOMNAMENODE_name) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> name = av.ptrvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STDOMNAMENODE_description) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> description = av.ptrvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STDOMNAMENODE_length) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> length = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STDOMNAMENODE_pdb_id) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> pdb_id = av.ptrvalue;
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
   ptr = StDomNameNodeFree(ptr);
   goto ret;
}



/**************************************************
*
*    StDomNameNodeAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
StDomNameNodeAsnWrite(StDomNameNodePtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, STDOMNAMENODE);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) {
      goto erret;
   }

   av.intvalue = ptr -> pssmid;
   retval = AsnWrite(aip, STDOMNAMENODE_pssmid,  &av);
   if (ptr -> accession != NULL) {
      av.ptrvalue = ptr -> accession;
      retval = AsnWrite(aip, STDOMNAMENODE_accession,  &av);
   }
   if (ptr -> name != NULL) {
      av.ptrvalue = ptr -> name;
      retval = AsnWrite(aip, STDOMNAMENODE_name,  &av);
   }
   if (ptr -> description != NULL) {
      av.ptrvalue = ptr -> description;
      retval = AsnWrite(aip, STDOMNAMENODE_description,  &av);
   }
   av.intvalue = ptr -> length;
   retval = AsnWrite(aip, STDOMNAMENODE_length,  &av);
   if (ptr -> pdb_id != NULL) {
      av.ptrvalue = ptr -> pdb_id;
      retval = AsnWrite(aip, STDOMNAMENODE_pdb_id,  &av);
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
*    StDomNamedbFree()
*
**************************************************/
NLM_EXTERN 
StDomNamedbPtr LIBCALL
StDomNamedbFree(StDomNamedbPtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   AsnGenericUserSeqOfFree(ptr,  (AsnOptFreeFunc) StDomNameNodeFree);
   return NULL;
}


/**************************************************
*
*    StDomNamedbAsnRead()
*
**************************************************/
NLM_EXTERN 
StDomNamedbPtr LIBCALL
StDomNamedbAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   StDomNamedbPtr ptr;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* StDomNamedb ::= (self contained) */
      atp = AsnReadId(aip, amp, STDOMNAMEDB);
   } else {
      atp = AsnLinkType(orig, STDOMNAMEDB);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   func = NULL;

   ptr  = AsnGenericUserSeqOfAsnRead(aip, amp, atp, &isError, (AsnReadFunc) StDomNameNodeAsnRead, (AsnOptFreeFunc) StDomNameNodeFree);
   if (isError && ptr  == NULL) {
      goto erret;
   }



ret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return ptr;

erret:
   aip -> io_failure = TRUE;
   ptr = StDomNamedbFree(ptr);
   goto ret;
}



/**************************************************
*
*    StDomNamedbAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
StDomNamedbAsnWrite(StDomNamedbPtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, STDOMNAMEDB);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   retval = AsnGenericUserSeqOfAsnWrite(ptr , (AsnWriteFunc) StDomNameNodeAsnWrite, aip, atp, STDOMNAMEDB_E);
   retval = TRUE;

erret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return retval;
}



/**************************************************
*
*    StHistdbNodeNew()
*
**************************************************/
NLM_EXTERN 
StHistdbNodePtr LIBCALL
StHistdbNodeNew(void)
{
   StHistdbNodePtr ptr = MemNew((size_t) sizeof(StHistdbNode));

   return ptr;

}


/**************************************************
*
*    StHistdbNodeFree()
*
**************************************************/
NLM_EXTERN 
StHistdbNodePtr LIBCALL
StHistdbNodeFree(StHistdbNodePtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   MemFree(ptr -> access);
   MemFree(ptr -> date);
   MemFree(ptr -> filename);
   return MemFree(ptr);
}


/**************************************************
*
*    StHistdbNodeAsnRead()
*
**************************************************/
NLM_EXTERN 
StHistdbNodePtr LIBCALL
StHistdbNodeAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   StHistdbNodePtr ptr;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* StHistdbNode ::= (self contained) */
      atp = AsnReadId(aip, amp, STHISTDBNODE);
   } else {
      atp = AsnLinkType(orig, STHISTDBNODE);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   ptr = StHistdbNodeNew();
   if (ptr == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
      goto erret;
   }

   atp = AsnReadId(aip,amp, atp);
   func = NULL;

   if (atp == STHISTDBNODE_gi) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> gi = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STHISTDBNODE_oldgi) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> oldgi = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STHISTDBNODE_access) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> access = av.ptrvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STHISTDBNODE_version) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> version = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STHISTDBNODE_date) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> date = av.ptrvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STHISTDBNODE_action) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> action = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STHISTDBNODE_filename) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> filename = av.ptrvalue;
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
   ptr = StHistdbNodeFree(ptr);
   goto ret;
}



/**************************************************
*
*    StHistdbNodeAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
StHistdbNodeAsnWrite(StHistdbNodePtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, STHISTDBNODE);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) {
      goto erret;
   }

   av.intvalue = ptr -> gi;
   retval = AsnWrite(aip, STHISTDBNODE_gi,  &av);
   av.intvalue = ptr -> oldgi;
   retval = AsnWrite(aip, STHISTDBNODE_oldgi,  &av);
   if (ptr -> access != NULL) {
      av.ptrvalue = ptr -> access;
      retval = AsnWrite(aip, STHISTDBNODE_access,  &av);
   }
   av.intvalue = ptr -> version;
   retval = AsnWrite(aip, STHISTDBNODE_version,  &av);
   if (ptr -> date != NULL) {
      av.ptrvalue = ptr -> date;
      retval = AsnWrite(aip, STHISTDBNODE_date,  &av);
   }
   av.intvalue = ptr -> action;
   retval = AsnWrite(aip, STHISTDBNODE_action,  &av);
   if (ptr -> filename != NULL) {
      av.ptrvalue = ptr -> filename;
      retval = AsnWrite(aip, STHISTDBNODE_filename,  &av);
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
*    StHistdbFree()
*
**************************************************/
NLM_EXTERN 
StHistdbPtr LIBCALL
StHistdbFree(StHistdbPtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   AsnGenericUserSeqOfFree(ptr,  (AsnOptFreeFunc) StHistdbNodeFree);
   return NULL;
}


/**************************************************
*
*    StHistdbAsnRead()
*
**************************************************/
NLM_EXTERN 
StHistdbPtr LIBCALL
StHistdbAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   StHistdbPtr ptr;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* StHistdb ::= (self contained) */
      atp = AsnReadId(aip, amp, STHISTDB);
   } else {
      atp = AsnLinkType(orig, STHISTDB);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   func = NULL;

   ptr  = AsnGenericUserSeqOfAsnRead(aip, amp, atp, &isError, (AsnReadFunc) StHistdbNodeAsnRead, (AsnOptFreeFunc) StHistdbNodeFree);
   if (isError && ptr  == NULL) {
      goto erret;
   }



ret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return ptr;

erret:
   aip -> io_failure = TRUE;
   ptr = StHistdbFree(ptr);
   goto ret;
}



/**************************************************
*
*    StHistdbAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
StHistdbAsnWrite(StHistdbPtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, STHISTDB);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   retval = AsnGenericUserSeqOfAsnWrite(ptr , (AsnWriteFunc) StHistdbNodeAsnWrite, aip, atp, STHISTDB_E);
   retval = TRUE;

erret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return retval;
}



/**************************************************
*
*    StGOdbNodeNew()
*
**************************************************/
NLM_EXTERN 
StGOdbNodePtr LIBCALL
StGOdbNodeNew(void)
{
   StGOdbNodePtr ptr = MemNew((size_t) sizeof(StGOdbNode));

   return ptr;

}


/**************************************************
*
*    StGOdbNodeFree()
*
**************************************************/
NLM_EXTERN 
StGOdbNodePtr LIBCALL
StGOdbNodeFree(StGOdbNodePtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   MemFree(ptr -> eviCode);
   return MemFree(ptr);
}


/**************************************************
*
*    StGOdbNodeAsnRead()
*
**************************************************/
NLM_EXTERN 
StGOdbNodePtr LIBCALL
StGOdbNodeAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   StGOdbNodePtr ptr;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* StGOdbNode ::= (self contained) */
      atp = AsnReadId(aip, amp, STGODBNODE);
   } else {
      atp = AsnLinkType(orig, STGODBNODE);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   ptr = StGOdbNodeNew();
   if (ptr == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
      goto erret;
   }

   atp = AsnReadId(aip,amp, atp);
   func = NULL;

   if (atp == STGODBNODE_llid) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> llid = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STGODBNODE_goid) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> goid = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STGODBNODE_pmid) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> pmid = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STGODBNODE_eviCode) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> eviCode = av.ptrvalue;
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
   ptr = StGOdbNodeFree(ptr);
   goto ret;
}



/**************************************************
*
*    StGOdbNodeAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
StGOdbNodeAsnWrite(StGOdbNodePtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, STGODBNODE);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) {
      goto erret;
   }

   av.intvalue = ptr -> llid;
   retval = AsnWrite(aip, STGODBNODE_llid,  &av);
   av.intvalue = ptr -> goid;
   retval = AsnWrite(aip, STGODBNODE_goid,  &av);
   av.intvalue = ptr -> pmid;
   retval = AsnWrite(aip, STGODBNODE_pmid,  &av);
   if (ptr -> eviCode != NULL) {
      av.ptrvalue = ptr -> eviCode;
      retval = AsnWrite(aip, STGODBNODE_eviCode,  &av);
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
*    StGOdbFree()
*
**************************************************/
NLM_EXTERN 
StGOdbPtr LIBCALL
StGOdbFree(StGOdbPtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   AsnGenericUserSeqOfFree(ptr,  (AsnOptFreeFunc) StGOdbNodeFree);
   return NULL;
}


/**************************************************
*
*    StGOdbAsnRead()
*
**************************************************/
NLM_EXTERN 
StGOdbPtr LIBCALL
StGOdbAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   StGOdbPtr ptr;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* StGOdb ::= (self contained) */
      atp = AsnReadId(aip, amp, STGODB);
   } else {
      atp = AsnLinkType(orig, STGODB);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   func = NULL;

   ptr  = AsnGenericUserSeqOfAsnRead(aip, amp, atp, &isError, (AsnReadFunc) StGOdbNodeAsnRead, (AsnOptFreeFunc) StGOdbNodeFree);
   if (isError && ptr  == NULL) {
      goto erret;
   }



ret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return ptr;

erret:
   aip -> io_failure = TRUE;
   ptr = StGOdbFree(ptr);
   goto ret;
}



/**************************************************
*
*    StGOdbAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
StGOdbAsnWrite(StGOdbPtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, STGODB);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   retval = AsnGenericUserSeqOfAsnWrite(ptr , (AsnWriteFunc) StGOdbNodeAsnWrite, aip, atp, STGODB_E);
   retval = TRUE;

erret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return retval;
}



/**************************************************
*
*    StOMIMdbNodeNew()
*
**************************************************/
NLM_EXTERN 
StOMIMdbNodePtr LIBCALL
StOMIMdbNodeNew(void)
{
   StOMIMdbNodePtr ptr = MemNew((size_t) sizeof(StOMIMdbNode));

   return ptr;

}


/**************************************************
*
*    StOMIMdbNodeFree()
*
**************************************************/
NLM_EXTERN 
StOMIMdbNodePtr LIBCALL
StOMIMdbNodeFree(StOMIMdbNodePtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   return MemFree(ptr);
}


/**************************************************
*
*    StOMIMdbNodeAsnRead()
*
**************************************************/
NLM_EXTERN 
StOMIMdbNodePtr LIBCALL
StOMIMdbNodeAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   StOMIMdbNodePtr ptr;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* StOMIMdbNode ::= (self contained) */
      atp = AsnReadId(aip, amp, STOMIMDBNODE);
   } else {
      atp = AsnLinkType(orig, STOMIMDBNODE);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   ptr = StOMIMdbNodeNew();
   if (ptr == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
      goto erret;
   }

   atp = AsnReadId(aip,amp, atp);
   func = NULL;

   if (atp == STOMIMDBNODE_llid) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> llid = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STOMIMDBNODE_omimid) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> omimid = av.intvalue;
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
   ptr = StOMIMdbNodeFree(ptr);
   goto ret;
}



/**************************************************
*
*    StOMIMdbNodeAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
StOMIMdbNodeAsnWrite(StOMIMdbNodePtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, STOMIMDBNODE);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) {
      goto erret;
   }

   av.intvalue = ptr -> llid;
   retval = AsnWrite(aip, STOMIMDBNODE_llid,  &av);
   av.intvalue = ptr -> omimid;
   retval = AsnWrite(aip, STOMIMDBNODE_omimid,  &av);
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
*    StOMIMdbFree()
*
**************************************************/
NLM_EXTERN 
StOMIMdbPtr LIBCALL
StOMIMdbFree(StOMIMdbPtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   AsnGenericUserSeqOfFree(ptr,  (AsnOptFreeFunc) StOMIMdbNodeFree);
   return NULL;
}


/**************************************************
*
*    StOMIMdbAsnRead()
*
**************************************************/
NLM_EXTERN 
StOMIMdbPtr LIBCALL
StOMIMdbAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   StOMIMdbPtr ptr;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* StOMIMdb ::= (self contained) */
      atp = AsnReadId(aip, amp, STOMIMDB);
   } else {
      atp = AsnLinkType(orig, STOMIMDB);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   func = NULL;

   ptr  = AsnGenericUserSeqOfAsnRead(aip, amp, atp, &isError, (AsnReadFunc) StOMIMdbNodeAsnRead, (AsnOptFreeFunc) StOMIMdbNodeFree);
   if (isError && ptr  == NULL) {
      goto erret;
   }



ret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return ptr;

erret:
   aip -> io_failure = TRUE;
   ptr = StOMIMdbFree(ptr);
   goto ret;
}



/**************************************************
*
*    StOMIMdbAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
StOMIMdbAsnWrite(StOMIMdbPtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, STOMIMDB);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   retval = AsnGenericUserSeqOfAsnWrite(ptr , (AsnWriteFunc) StOMIMdbNodeAsnWrite, aip, atp, STOMIMDB_E);
   retval = TRUE;

erret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return retval;
}



/**************************************************
*
*    StCDDdbNodeNew()
*
**************************************************/
NLM_EXTERN 
StCDDdbNodePtr LIBCALL
StCDDdbNodeNew(void)
{
   StCDDdbNodePtr ptr = MemNew((size_t) sizeof(StCDDdbNode));

   return ptr;

}


/**************************************************
*
*    StCDDdbNodeFree()
*
**************************************************/
NLM_EXTERN 
StCDDdbNodePtr LIBCALL
StCDDdbNodeFree(StCDDdbNodePtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   MemFree(ptr -> cddid);
   return MemFree(ptr);
}


/**************************************************
*
*    StCDDdbNodeAsnRead()
*
**************************************************/
NLM_EXTERN 
StCDDdbNodePtr LIBCALL
StCDDdbNodeAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   StCDDdbNodePtr ptr;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* StCDDdbNode ::= (self contained) */
      atp = AsnReadId(aip, amp, STCDDDBNODE);
   } else {
      atp = AsnLinkType(orig, STCDDDBNODE);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   ptr = StCDDdbNodeNew();
   if (ptr == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
      goto erret;
   }

   atp = AsnReadId(aip,amp, atp);
   func = NULL;

   if (atp == STCDDDBNODE_llid) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> llid = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STCDDDBNODE_cddid) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> cddid = av.ptrvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STCDDDBNODE_eValue) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> eValue = av.realvalue;
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
   ptr = StCDDdbNodeFree(ptr);
   goto ret;
}



/**************************************************
*
*    StCDDdbNodeAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
StCDDdbNodeAsnWrite(StCDDdbNodePtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, STCDDDBNODE);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) {
      goto erret;
   }

   av.intvalue = ptr -> llid;
   retval = AsnWrite(aip, STCDDDBNODE_llid,  &av);
   if (ptr -> cddid != NULL) {
      av.ptrvalue = ptr -> cddid;
      retval = AsnWrite(aip, STCDDDBNODE_cddid,  &av);
   }
   av.realvalue = ptr -> eValue;
   retval = AsnWrite(aip, STCDDDBNODE_eValue,  &av);
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
*    StCDDdbFree()
*
**************************************************/
NLM_EXTERN 
StCDDdbPtr LIBCALL
StCDDdbFree(StCDDdbPtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   AsnGenericUserSeqOfFree(ptr,  (AsnOptFreeFunc) StCDDdbNodeFree);
   return NULL;
}


/**************************************************
*
*    StCDDdbAsnRead()
*
**************************************************/
NLM_EXTERN 
StCDDdbPtr LIBCALL
StCDDdbAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   StCDDdbPtr ptr;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* StCDDdb ::= (self contained) */
      atp = AsnReadId(aip, amp, STCDDDB);
   } else {
      atp = AsnLinkType(orig, STCDDDB);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   func = NULL;

   ptr  = AsnGenericUserSeqOfAsnRead(aip, amp, atp, &isError, (AsnReadFunc) StCDDdbNodeAsnRead, (AsnOptFreeFunc) StCDDdbNodeFree);
   if (isError && ptr  == NULL) {
      goto erret;
   }



ret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return ptr;

erret:
   aip -> io_failure = TRUE;
   ptr = StCDDdbFree(ptr);
   goto ret;
}



/**************************************************
*
*    StCDDdbAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
StCDDdbAsnWrite(StCDDdbPtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, STCDDDB);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   retval = AsnGenericUserSeqOfAsnWrite(ptr , (AsnWriteFunc) StCDDdbNodeAsnWrite, aip, atp, STCDDDB_E);
   retval = TRUE;

erret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return retval;
}



/**************************************************
*
*    StLLdbNodeNew()
*
**************************************************/
NLM_EXTERN 
StLLdbNodePtr LIBCALL
StLLdbNodeNew(void)
{
   StLLdbNodePtr ptr = MemNew((size_t) sizeof(StLLdbNode));

   return ptr;

}


/**************************************************
*
*    StLLdbNodeFree()
*
**************************************************/
NLM_EXTERN 
StLLdbNodePtr LIBCALL
StLLdbNodeFree(StLLdbNodePtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   MemFree(ptr -> map);
   return MemFree(ptr);
}


/**************************************************
*
*    StLLdbNodeAsnRead()
*
**************************************************/
NLM_EXTERN 
StLLdbNodePtr LIBCALL
StLLdbNodeAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   StLLdbNodePtr ptr;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* StLLdbNode ::= (self contained) */
      atp = AsnReadId(aip, amp, STLLDBNODE);
   } else {
      atp = AsnLinkType(orig, STLLDBNODE);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   ptr = StLLdbNodeNew();
   if (ptr == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
      goto erret;
   }

   atp = AsnReadId(aip,amp, atp);
   func = NULL;

   if (atp == STLLDBNODE_llid) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> llid = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STLLDBNODE_npid) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> npid = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STLLDBNODE_map) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> map = av.ptrvalue;
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
   ptr = StLLdbNodeFree(ptr);
   goto ret;
}



/**************************************************
*
*    StLLdbNodeAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
StLLdbNodeAsnWrite(StLLdbNodePtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, STLLDBNODE);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) {
      goto erret;
   }

   av.intvalue = ptr -> llid;
   retval = AsnWrite(aip, STLLDBNODE_llid,  &av);
   av.intvalue = ptr -> npid;
   retval = AsnWrite(aip, STLLDBNODE_npid,  &av);
   if (ptr -> map != NULL) {
      av.ptrvalue = ptr -> map;
      retval = AsnWrite(aip, STLLDBNODE_map,  &av);
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
*    StLLdbFree()
*
**************************************************/
NLM_EXTERN 
StLLdbPtr LIBCALL
StLLdbFree(StLLdbPtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   AsnGenericUserSeqOfFree(ptr,  (AsnOptFreeFunc) StLLdbNodeFree);
   return NULL;
}


/**************************************************
*
*    StLLdbAsnRead()
*
**************************************************/
NLM_EXTERN 
StLLdbPtr LIBCALL
StLLdbAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   StLLdbPtr ptr;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* StLLdb ::= (self contained) */
      atp = AsnReadId(aip, amp, STLLDB);
   } else {
      atp = AsnLinkType(orig, STLLDB);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   func = NULL;

   ptr  = AsnGenericUserSeqOfAsnRead(aip, amp, atp, &isError, (AsnReadFunc) StLLdbNodeAsnRead, (AsnOptFreeFunc) StLLdbNodeFree);
   if (isError && ptr  == NULL) {
      goto erret;
   }



ret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return ptr;

erret:
   aip -> io_failure = TRUE;
   ptr = StLLdbFree(ptr);
   goto ret;
}



/**************************************************
*
*    StLLdbAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
StLLdbAsnWrite(StLLdbPtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, STLLDB);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   retval = AsnGenericUserSeqOfAsnWrite(ptr , (AsnWriteFunc) StLLdbNodeAsnWrite, aip, atp, STLLDB_E);
   retval = TRUE;

erret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return retval;
}



/**************************************************
*
*    SHGoReportNew()
*
**************************************************/
NLM_EXTERN 
SHGoReportPtr LIBCALL
SHGoReportNew(void)
{
   SHGoReportPtr ptr = MemNew((size_t) sizeof(SHGoReport));

   return ptr;

}


/**************************************************
*
*    SHGoReportFree()
*
**************************************************/
NLM_EXTERN 
SHGoReportPtr LIBCALL
SHGoReportFree(SHGoReportPtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   SHGoAnnotFree(ptr -> mol_fun);
   SHGoAnnotFree(ptr -> bio_pro);
   SHGoAnnotFree(ptr -> cell_com);
   return MemFree(ptr);
}


/**************************************************
*
*    SHGoReportAsnRead()
*
**************************************************/
NLM_EXTERN 
SHGoReportPtr LIBCALL
SHGoReportAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   SHGoReportPtr ptr;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* SHGoReport ::= (self contained) */
      atp = AsnReadId(aip, amp, SHGOREPORT);
   } else {
      atp = AsnLinkType(orig, SHGOREPORT);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   ptr = SHGoReportNew();
   if (ptr == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
      goto erret;
   }

   atp = AsnReadId(aip,amp, atp);
   func = NULL;

   if (atp == SHGOREPORT_num_mol_fun) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> num_mol_fun = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == SHGOREPORT_num_bio_pro) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> num_bio_pro = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == SHGOREPORT_num_cell_com) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> num_cell_com = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == SHGOREPORT_mol_fun) {
      ptr -> mol_fun = SHGoAnnotAsnRead(aip, atp);
      if (aip -> io_failure) {
         goto erret;
      }
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == SHGOREPORT_bio_pro) {
      ptr -> bio_pro = SHGoAnnotAsnRead(aip, atp);
      if (aip -> io_failure) {
         goto erret;
      }
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == SHGOREPORT_cell_com) {
      ptr -> cell_com = SHGoAnnotAsnRead(aip, atp);
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
   ptr = SHGoReportFree(ptr);
   goto ret;
}



/**************************************************
*
*    SHGoReportAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
SHGoReportAsnWrite(SHGoReportPtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, SHGOREPORT);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) {
      goto erret;
   }

   av.intvalue = ptr -> num_mol_fun;
   retval = AsnWrite(aip, SHGOREPORT_num_mol_fun,  &av);
   av.intvalue = ptr -> num_bio_pro;
   retval = AsnWrite(aip, SHGOREPORT_num_bio_pro,  &av);
   av.intvalue = ptr -> num_cell_com;
   retval = AsnWrite(aip, SHGOREPORT_num_cell_com,  &av);
   if (ptr -> mol_fun != NULL) {
      if ( ! SHGoAnnotAsnWrite(ptr -> mol_fun, aip, SHGOREPORT_mol_fun)) {
         goto erret;
      }
   }
   if (ptr -> bio_pro != NULL) {
      if ( ! SHGoAnnotAsnWrite(ptr -> bio_pro, aip, SHGOREPORT_bio_pro)) {
         goto erret;
      }
   }
   if (ptr -> cell_com != NULL) {
      if ( ! SHGoAnnotAsnWrite(ptr -> cell_com, aip, SHGOREPORT_cell_com)) {
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
*    SHLocusLinkReportNew()
*
**************************************************/
NLM_EXTERN 
SHLocusLinkReportPtr LIBCALL
SHLocusLinkReportNew(void)
{
   SHLocusLinkReportPtr ptr = MemNew((size_t) sizeof(SHLocusLinkReport));

   return ptr;

}


/**************************************************
*
*    SHLocusLinkReportFree()
*
**************************************************/
NLM_EXTERN 
SHLocusLinkReportPtr LIBCALL
SHLocusLinkReportFree(SHLocusLinkReportPtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   MemFree(ptr -> locus);
   AsnGenericUserSeqOfFree(ptr -> products, (AsnOptFreeFunc) SHLocusProductFree);
   return MemFree(ptr);
}


/**************************************************
*
*    SHLocusLinkReportAsnRead()
*
**************************************************/
NLM_EXTERN 
SHLocusLinkReportPtr LIBCALL
SHLocusLinkReportAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   SHLocusLinkReportPtr ptr;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* SHLocusLinkReport ::= (self contained) */
      atp = AsnReadId(aip, amp, SHLOCUSLINKREPORT);
   } else {
      atp = AsnLinkType(orig, SHLOCUSLINKREPORT);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   ptr = SHLocusLinkReportNew();
   if (ptr == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
      goto erret;
   }

   atp = AsnReadId(aip,amp, atp);
   func = NULL;

   if (atp == SHLOCUSLINKREPORT_llid) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> llid = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == SHLOCUSLINKREPORT_locus) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> locus = av.ptrvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == SHLOCUSLINKREPORT_products) {
      ptr -> products = AsnGenericUserSeqOfAsnRead(aip, amp, atp, &isError, (AsnReadFunc) SHLocusProductAsnRead, (AsnOptFreeFunc) SHLocusProductFree);
      if (isError && ptr -> products == NULL) {
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
   ptr = SHLocusLinkReportFree(ptr);
   goto ret;
}



/**************************************************
*
*    SHLocusLinkReportAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
SHLocusLinkReportAsnWrite(SHLocusLinkReportPtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, SHLOCUSLINKREPORT);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) {
      goto erret;
   }

   av.intvalue = ptr -> llid;
   retval = AsnWrite(aip, SHLOCUSLINKREPORT_llid,  &av);
   if (ptr -> locus != NULL) {
      av.ptrvalue = ptr -> locus;
      retval = AsnWrite(aip, SHLOCUSLINKREPORT_locus,  &av);
   }
   AsnGenericUserSeqOfAsnWrite(ptr -> products, (AsnWriteFunc) SHLocusProductAsnWrite, aip, SHLOCUSLINKREPORT_products, SHLOCUSLINKREPORT_products_E);
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
*    SHNeighbourNew()
*
**************************************************/
NLM_EXTERN 
SHNeighbourPtr LIBCALL
SHNeighbourNew(void)
{
   SHNeighbourPtr ptr = MemNew((size_t) sizeof(SHNeighbour));

   return ptr;

}


/**************************************************
*
*    SHNeighbourFree()
*
**************************************************/
NLM_EXTERN 
SHNeighbourPtr LIBCALL
SHNeighbourFree(SHNeighbourPtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   return MemFree(ptr);
}


/**************************************************
*
*    SHNeighbourAsnRead()
*
**************************************************/
NLM_EXTERN 
SHNeighbourPtr LIBCALL
SHNeighbourAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   SHNeighbourPtr ptr;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* SHNeighbour ::= (self contained) */
      atp = AsnReadId(aip, amp, SHNEIGHBOUR);
   } else {
      atp = AsnLinkType(orig, SHNEIGHBOUR);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   ptr = SHNeighbourNew();
   if (ptr == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
      goto erret;
   }

   atp = AsnReadId(aip,amp, atp);
   func = NULL;

   if (atp == SHNEIGHBOUR_gi) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> gi = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == SHNEIGHBOUR_evalue) {
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
   ptr = SHNeighbourFree(ptr);
   goto ret;
}



/**************************************************
*
*    SHNeighbourAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
SHNeighbourAsnWrite(SHNeighbourPtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, SHNEIGHBOUR);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) {
      goto erret;
   }

   av.intvalue = ptr -> gi;
   retval = AsnWrite(aip, SHNEIGHBOUR_gi,  &av);
   av.realvalue = ptr -> evalue;
   retval = AsnWrite(aip, SHNEIGHBOUR_evalue,  &av);
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
*    SHGoAnnotFree()
*
**************************************************/
NLM_EXTERN 
SHGoAnnotPtr LIBCALL
SHGoAnnotFree(SHGoAnnotPtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   AsnGenericUserSeqOfFree(ptr,  (AsnOptFreeFunc) SHGoAnnotNodeFree);
   return NULL;
}


/**************************************************
*
*    SHGoAnnotAsnRead()
*
**************************************************/
NLM_EXTERN 
SHGoAnnotPtr LIBCALL
SHGoAnnotAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   SHGoAnnotPtr ptr;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* SHGoAnnot ::= (self contained) */
      atp = AsnReadId(aip, amp, SHGOANNOT);
   } else {
      atp = AsnLinkType(orig, SHGOANNOT);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   func = NULL;

   ptr  = AsnGenericUserSeqOfAsnRead(aip, amp, atp, &isError, (AsnReadFunc) SHGoAnnotNodeAsnRead, (AsnOptFreeFunc) SHGoAnnotNodeFree);
   if (isError && ptr  == NULL) {
      goto erret;
   }



ret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return ptr;

erret:
   aip -> io_failure = TRUE;
   ptr = SHGoAnnotFree(ptr);
   goto ret;
}



/**************************************************
*
*    SHGoAnnotAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
SHGoAnnotAsnWrite(SHGoAnnotPtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, SHGOANNOT);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   retval = AsnGenericUserSeqOfAsnWrite(ptr , (AsnWriteFunc) SHGoAnnotNodeAsnWrite, aip, atp, SHGOANNOT_E);
   retval = TRUE;

erret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return retval;
}



/**************************************************
*
*    SHGoAnnotNodeNew()
*
**************************************************/
NLM_EXTERN 
SHGoAnnotNodePtr LIBCALL
SHGoAnnotNodeNew(void)
{
   SHGoAnnotNodePtr ptr = MemNew((size_t) sizeof(SHGoAnnotNode));

   return ptr;

}


/**************************************************
*
*    SHGoAnnotNodeFree()
*
**************************************************/
NLM_EXTERN 
SHGoAnnotNodePtr LIBCALL
SHGoAnnotNodeFree(SHGoAnnotNodePtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   MemFree(ptr -> ontology);
   MemFree(ptr -> goname);
   AsnGenericBaseSeqOfFree(ptr -> evidence_code ,ASNCODE_PTRVAL_SLOT);
   AsnGenericBaseSeqOfFree(ptr -> pmid ,ASNCODE_INTVAL_SLOT);
   return MemFree(ptr);
}


/**************************************************
*
*    SHGoAnnotNodeAsnRead()
*
**************************************************/
NLM_EXTERN 
SHGoAnnotNodePtr LIBCALL
SHGoAnnotNodeAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   SHGoAnnotNodePtr ptr;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* SHGoAnnotNode ::= (self contained) */
      atp = AsnReadId(aip, amp, SHGOANNOTNODE);
   } else {
      atp = AsnLinkType(orig, SHGOANNOTNODE);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   ptr = SHGoAnnotNodeNew();
   if (ptr == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
      goto erret;
   }

   atp = AsnReadId(aip,amp, atp);
   func = NULL;

   if (atp == SHGOANNOTNODE_goid) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> goid = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == SHGOANNOTNODE_ontology) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> ontology = av.ptrvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == SHGOANNOTNODE_goname) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> goname = av.ptrvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == SHGOANNOTNODE_evidence_code) {
      ptr -> evidence_code = AsnGenericBaseSeqOfAsnRead(aip, amp, atp, ASNCODE_PTRVAL_SLOT, &isError);
      if (isError && ptr -> evidence_code == NULL) {
         goto erret;
      }
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == SHGOANNOTNODE_pmid) {
      ptr -> pmid = AsnGenericBaseSeqOfAsnRead(aip, amp, atp, ASNCODE_INTVAL_SLOT, &isError);
      if (isError && ptr -> pmid == NULL) {
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
   ptr = SHGoAnnotNodeFree(ptr);
   goto ret;
}



/**************************************************
*
*    SHGoAnnotNodeAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
SHGoAnnotNodeAsnWrite(SHGoAnnotNodePtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, SHGOANNOTNODE);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) {
      goto erret;
   }

   av.intvalue = ptr -> goid;
   retval = AsnWrite(aip, SHGOANNOTNODE_goid,  &av);
   if (ptr -> ontology != NULL) {
      av.ptrvalue = ptr -> ontology;
      retval = AsnWrite(aip, SHGOANNOTNODE_ontology,  &av);
   }
   if (ptr -> goname != NULL) {
      av.ptrvalue = ptr -> goname;
      retval = AsnWrite(aip, SHGOANNOTNODE_goname,  &av);
   }
   retval = AsnGenericBaseSeqOfAsnWrite(ptr -> evidence_code ,ASNCODE_PTRVAL_SLOT, aip, SHGOANNOTNODE_evidence_code, SHGOANNOTNODE_evidence_code_E);
   retval = AsnGenericBaseSeqOfAsnWrite(ptr -> pmid ,ASNCODE_INTVAL_SLOT, aip, SHGOANNOTNODE_pmid, SHGOANNOTNODE_pmid_E);
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
*    SHLocusProductNew()
*
**************************************************/
NLM_EXTERN 
SHLocusProductPtr LIBCALL
SHLocusProductNew(void)
{
   SHLocusProductPtr ptr = MemNew((size_t) sizeof(SHLocusProduct));

   return ptr;

}


/**************************************************
*
*    SHLocusProductFree()
*
**************************************************/
NLM_EXTERN 
SHLocusProductPtr LIBCALL
SHLocusProductFree(SHLocusProductPtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   MemFree(ptr -> defline);
   return MemFree(ptr);
}


/**************************************************
*
*    SHLocusProductAsnRead()
*
**************************************************/
NLM_EXTERN 
SHLocusProductPtr LIBCALL
SHLocusProductAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   SHLocusProductPtr ptr;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* SHLocusProduct ::= (self contained) */
      atp = AsnReadId(aip, amp, SHLOCUSPRODUCT);
   } else {
      atp = AsnLinkType(orig, SHLOCUSPRODUCT);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   ptr = SHLocusProductNew();
   if (ptr == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
      goto erret;
   }

   atp = AsnReadId(aip,amp, atp);
   func = NULL;

   if (atp == SHLOCUSPRODUCT_gi) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> gi = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == SHLOCUSPRODUCT_defline) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> defline = av.ptrvalue;
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
   ptr = SHLocusProductFree(ptr);
   goto ret;
}



/**************************************************
*
*    SHLocusProductAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
SHLocusProductAsnWrite(SHLocusProductPtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, SHLOCUSPRODUCT);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) {
      goto erret;
   }

   av.intvalue = ptr -> gi;
   retval = AsnWrite(aip, SHLOCUSPRODUCT_gi,  &av);
   if (ptr -> defline != NULL) {
      av.ptrvalue = ptr -> defline;
      retval = AsnWrite(aip, SHLOCUSPRODUCT_defline,  &av);
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
*    StDbXrefNodeNew()
*
**************************************************/
NLM_EXTERN 
StDbXrefNodePtr LIBCALL
StDbXrefNodeNew(void)
{
   StDbXrefNodePtr ptr = MemNew((size_t) sizeof(StDbXrefNode));

   return ptr;

}


/**************************************************
*
*    StDbXrefNodeFree()
*
**************************************************/
NLM_EXTERN 
StDbXrefNodePtr LIBCALL
StDbXrefNodeFree(StDbXrefNodePtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   MemFree(ptr -> sourcedb);
   MemFree(ptr -> recordid);
   MemFree(ptr -> field);
   return MemFree(ptr);
}


/**************************************************
*
*    StDbXrefNodeAsnRead()
*
**************************************************/
NLM_EXTERN 
StDbXrefNodePtr LIBCALL
StDbXrefNodeAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   StDbXrefNodePtr ptr;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* StDbXrefNode ::= (self contained) */
      atp = AsnReadId(aip, amp, STDBXREFNODE);
   } else {
      atp = AsnLinkType(orig, STDBXREFNODE);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   ptr = StDbXrefNodeNew();
   if (ptr == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
      goto erret;
   }

   atp = AsnReadId(aip,amp, atp);
   func = NULL;

   if (atp == STDBXREFNODE_id) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> id = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STDBXREFNODE_sourcedb) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> sourcedb = av.ptrvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STDBXREFNODE_sourceid) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> sourceid = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STDBXREFNODE_recordid) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> recordid = av.ptrvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STDBXREFNODE_parentid) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> parentid = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STDBXREFNODE_link) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> link = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STDBXREFNODE_field) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> field = av.ptrvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STDBXREFNODE_cv) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> cv = av.intvalue;
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
   ptr = StDbXrefNodeFree(ptr);
   goto ret;
}



/**************************************************
*
*    StDbXrefNodeAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
StDbXrefNodeAsnWrite(StDbXrefNodePtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, STDBXREFNODE);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) {
      goto erret;
   }

   av.intvalue = ptr -> id;
   retval = AsnWrite(aip, STDBXREFNODE_id,  &av);
   if (ptr -> sourcedb != NULL) {
      av.ptrvalue = ptr -> sourcedb;
      retval = AsnWrite(aip, STDBXREFNODE_sourcedb,  &av);
   }
   av.intvalue = ptr -> sourceid;
   retval = AsnWrite(aip, STDBXREFNODE_sourceid,  &av);
   if (ptr -> recordid != NULL) {
      av.ptrvalue = ptr -> recordid;
      retval = AsnWrite(aip, STDBXREFNODE_recordid,  &av);
   }
   av.intvalue = ptr -> parentid;
   retval = AsnWrite(aip, STDBXREFNODE_parentid,  &av);
   av.intvalue = ptr -> link;
   retval = AsnWrite(aip, STDBXREFNODE_link,  &av);
   if (ptr -> field != NULL) {
      av.ptrvalue = ptr -> field;
      retval = AsnWrite(aip, STDBXREFNODE_field,  &av);
   }
   av.intvalue = ptr -> cv;
   retval = AsnWrite(aip, STDBXREFNODE_cv,  &av);
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
*    StDbXrefFree()
*
**************************************************/
NLM_EXTERN 
StDbXrefPtr LIBCALL
StDbXrefFree(StDbXrefPtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   AsnGenericUserSeqOfFree(ptr,  (AsnOptFreeFunc) StDbXrefNodeFree);
   return NULL;
}


/**************************************************
*
*    StDbXrefAsnRead()
*
**************************************************/
NLM_EXTERN 
StDbXrefPtr LIBCALL
StDbXrefAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   StDbXrefPtr ptr;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* StDbXref ::= (self contained) */
      atp = AsnReadId(aip, amp, STDBXREF);
   } else {
      atp = AsnLinkType(orig, STDBXREF);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   func = NULL;

   ptr  = AsnGenericUserSeqOfAsnRead(aip, amp, atp, &isError, (AsnReadFunc) StDbXrefNodeAsnRead, (AsnOptFreeFunc) StDbXrefNodeFree);
   if (isError && ptr  == NULL) {
      goto erret;
   }



ret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return ptr;

erret:
   aip -> io_failure = TRUE;
   ptr = StDbXrefFree(ptr);
   goto ret;
}



/**************************************************
*
*    StDbXrefAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
StDbXrefAsnWrite(StDbXrefPtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, STDBXREF);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   retval = AsnGenericUserSeqOfAsnWrite(ptr , (AsnWriteFunc) StDbXrefNodeAsnWrite, aip, atp, STDBXREF_E);
   retval = TRUE;

erret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return retval;
}



/**************************************************
*
*    SHDBXrefNodeNew()
*
**************************************************/
NLM_EXTERN 
SHDBXrefNodePtr LIBCALL
SHDBXrefNodeNew(void)
{
   SHDBXrefNodePtr ptr = MemNew((size_t) sizeof(SHDBXrefNode));

   return ptr;

}


/**************************************************
*
*    SHDBXrefNodeFree()
*
**************************************************/
NLM_EXTERN 
SHDBXrefNodePtr LIBCALL
SHDBXrefNodeFree(SHDBXrefNodePtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   MemFree(ptr -> sourcedb);
   MemFree(ptr -> recordid);
   MemFree(ptr -> field);
   return MemFree(ptr);
}


/**************************************************
*
*    SHDBXrefNodeAsnRead()
*
**************************************************/
NLM_EXTERN 
SHDBXrefNodePtr LIBCALL
SHDBXrefNodeAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   SHDBXrefNodePtr ptr;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* SHDBXrefNode ::= (self contained) */
      atp = AsnReadId(aip, amp, SHDBXREFNODE);
   } else {
      atp = AsnLinkType(orig, SHDBXREFNODE);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   ptr = SHDBXrefNodeNew();
   if (ptr == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
      goto erret;
   }

   atp = AsnReadId(aip,amp, atp);
   func = NULL;

   if (atp == SHDBXREFNODE_sourcedb) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> sourcedb = av.ptrvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == SHDBXREFNODE_recordid) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> recordid = av.ptrvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == SHDBXREFNODE_field) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> field = av.ptrvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == SHDBXREFNODE_cv) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> cv = av.intvalue;
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
   ptr = SHDBXrefNodeFree(ptr);
   goto ret;
}



/**************************************************
*
*    SHDBXrefNodeAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
SHDBXrefNodeAsnWrite(SHDBXrefNodePtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, SHDBXREFNODE);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) {
      goto erret;
   }

   if (ptr -> sourcedb != NULL) {
      av.ptrvalue = ptr -> sourcedb;
      retval = AsnWrite(aip, SHDBXREFNODE_sourcedb,  &av);
   }
   if (ptr -> recordid != NULL) {
      av.ptrvalue = ptr -> recordid;
      retval = AsnWrite(aip, SHDBXREFNODE_recordid,  &av);
   }
   if (ptr -> field != NULL) {
      av.ptrvalue = ptr -> field;
      retval = AsnWrite(aip, SHDBXREFNODE_field,  &av);
   }
   av.intvalue = ptr -> cv;
   retval = AsnWrite(aip, SHDBXREFNODE_cv,  &av);
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
*    SHDBXrefFree()
*
**************************************************/
NLM_EXTERN 
SHDBXrefPtr LIBCALL
SHDBXrefFree(SHDBXrefPtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   AsnGenericUserSeqOfFree(ptr,  (AsnOptFreeFunc) SHDBXrefNodeFree);
   return NULL;
}


/**************************************************
*
*    SHDBXrefAsnRead()
*
**************************************************/
NLM_EXTERN 
SHDBXrefPtr LIBCALL
SHDBXrefAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   SHDBXrefPtr ptr;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* SHDBXref ::= (self contained) */
      atp = AsnReadId(aip, amp, SHDBXREF);
   } else {
      atp = AsnLinkType(orig, SHDBXREF);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   func = NULL;

   ptr  = AsnGenericUserSeqOfAsnRead(aip, amp, atp, &isError, (AsnReadFunc) SHDBXrefNodeAsnRead, (AsnOptFreeFunc) SHDBXrefNodeFree);
   if (isError && ptr  == NULL) {
      goto erret;
   }



ret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return ptr;

erret:
   aip -> io_failure = TRUE;
   ptr = SHDBXrefFree(ptr);
   goto ret;
}



/**************************************************
*
*    SHDBXrefAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
SHDBXrefAsnWrite(SHDBXrefPtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, SHDBXREF);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   retval = AsnGenericUserSeqOfAsnWrite(ptr , (AsnWriteFunc) SHDBXrefNodeAsnWrite, aip, atp, SHDBXREF_E);
   retval = TRUE;

erret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return retval;
}



/**************************************************
*
*    StBioentityNodeNew()
*
**************************************************/
NLM_EXTERN 
StBioentityNodePtr LIBCALL
StBioentityNodeNew(void)
{
   StBioentityNodePtr ptr = MemNew((size_t) sizeof(StBioentityNode));

   return ptr;

}


/**************************************************
*
*    StBioentityNodeFree()
*
**************************************************/
NLM_EXTERN 
StBioentityNodePtr LIBCALL
StBioentityNodeFree(StBioentityNodePtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   MemFree(ptr -> db);
   MemFree(ptr -> access);
   return MemFree(ptr);
}


/**************************************************
*
*    StBioentityNodeAsnRead()
*
**************************************************/
NLM_EXTERN 
StBioentityNodePtr LIBCALL
StBioentityNodeAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   StBioentityNodePtr ptr;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* StBioentityNode ::= (self contained) */
      atp = AsnReadId(aip, amp, STBIOENTITYNODE);
   } else {
      atp = AsnLinkType(orig, STBIOENTITYNODE);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   ptr = StBioentityNodeNew();
   if (ptr == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
      goto erret;
   }

   atp = AsnReadId(aip,amp, atp);
   func = NULL;

   if (atp == STBIOENTITYNODE_id) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> id = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STBIOENTITYNODE_bioentitytypeid) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> bioentitytypeid = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STBIOENTITYNODE_db) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> db = av.ptrvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STBIOENTITYNODE_access) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> access = av.ptrvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STBIOENTITYNODE_identifier) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> identifier = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STBIOENTITYNODE_field) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> field = av.intvalue;
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
   ptr = StBioentityNodeFree(ptr);
   goto ret;
}



/**************************************************
*
*    StBioentityNodeAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
StBioentityNodeAsnWrite(StBioentityNodePtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, STBIOENTITYNODE);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) {
      goto erret;
   }

   av.intvalue = ptr -> id;
   retval = AsnWrite(aip, STBIOENTITYNODE_id,  &av);
   av.intvalue = ptr -> bioentitytypeid;
   retval = AsnWrite(aip, STBIOENTITYNODE_bioentitytypeid,  &av);
   if (ptr -> db != NULL) {
      av.ptrvalue = ptr -> db;
      retval = AsnWrite(aip, STBIOENTITYNODE_db,  &av);
   }
   if (ptr -> access != NULL) {
      av.ptrvalue = ptr -> access;
      retval = AsnWrite(aip, STBIOENTITYNODE_access,  &av);
   }
   av.intvalue = ptr -> identifier;
   retval = AsnWrite(aip, STBIOENTITYNODE_identifier,  &av);
   av.intvalue = ptr -> field;
   retval = AsnWrite(aip, STBIOENTITYNODE_field,  &av);
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
*    StBioentityFree()
*
**************************************************/
NLM_EXTERN 
StBioentityPtr LIBCALL
StBioentityFree(StBioentityPtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   AsnGenericUserSeqOfFree(ptr,  (AsnOptFreeFunc) StBioentityNodeFree);
   return NULL;
}


/**************************************************
*
*    StBioentityAsnRead()
*
**************************************************/
NLM_EXTERN 
StBioentityPtr LIBCALL
StBioentityAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   StBioentityPtr ptr;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* StBioentity ::= (self contained) */
      atp = AsnReadId(aip, amp, STBIOENTITY);
   } else {
      atp = AsnLinkType(orig, STBIOENTITY);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   func = NULL;

   ptr  = AsnGenericUserSeqOfAsnRead(aip, amp, atp, &isError, (AsnReadFunc) StBioentityNodeAsnRead, (AsnOptFreeFunc) StBioentityNodeFree);
   if (isError && ptr  == NULL) {
      goto erret;
   }



ret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return ptr;

erret:
   aip -> io_failure = TRUE;
   ptr = StBioentityFree(ptr);
   goto ret;
}



/**************************************************
*
*    StBioentityAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
StBioentityAsnWrite(StBioentityPtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, STBIOENTITY);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   retval = AsnGenericUserSeqOfAsnWrite(ptr , (AsnWriteFunc) StBioentityNodeAsnWrite, aip, atp, STBIOENTITY_E);
   retval = TRUE;

erret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return retval;
}



/**************************************************
*
*    StBionameNodeNew()
*
**************************************************/
NLM_EXTERN 
StBionameNodePtr LIBCALL
StBionameNodeNew(void)
{
   StBionameNodePtr ptr = MemNew((size_t) sizeof(StBionameNode));

   return ptr;

}


/**************************************************
*
*    StBionameNodeFree()
*
**************************************************/
NLM_EXTERN 
StBionameNodePtr LIBCALL
StBionameNodeFree(StBionameNodePtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   MemFree(ptr -> name);
   MemFree(ptr -> db);
   MemFree(ptr -> access);
   MemFree(ptr -> date);
   MemFree(ptr -> actiondate);
   return MemFree(ptr);
}


/**************************************************
*
*    StBionameNodeAsnRead()
*
**************************************************/
NLM_EXTERN 
StBionameNodePtr LIBCALL
StBionameNodeAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   StBionameNodePtr ptr;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* StBionameNode ::= (self contained) */
      atp = AsnReadId(aip, amp, STBIONAMENODE);
   } else {
      atp = AsnLinkType(orig, STBIONAMENODE);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   ptr = StBionameNodeNew();
   if (ptr == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
      goto erret;
   }

   atp = AsnReadId(aip,amp, atp);
   func = NULL;

   if (atp == STBIONAMENODE_id) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> id = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STBIONAMENODE_bioentityid) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> bioentityid = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STBIONAMENODE_name) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> name = av.ptrvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STBIONAMENODE_nametypeid) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> nametypeid = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STBIONAMENODE_db) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> db = av.ptrvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STBIONAMENODE_access) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> access = av.ptrvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STBIONAMENODE_identifier) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> identifier = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STBIONAMENODE_field) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> field = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STBIONAMENODE_official) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> official = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STBIONAMENODE_deprecated) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> deprecated = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STBIONAMENODE_date) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> date = av.ptrvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STBIONAMENODE_ruleid) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> ruleid = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STBIONAMENODE_action) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> action = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STBIONAMENODE_actiondate) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> actiondate = av.ptrvalue;
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
   ptr = StBionameNodeFree(ptr);
   goto ret;
}



/**************************************************
*
*    StBionameNodeAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
StBionameNodeAsnWrite(StBionameNodePtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, STBIONAMENODE);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) {
      goto erret;
   }

   av.intvalue = ptr -> id;
   retval = AsnWrite(aip, STBIONAMENODE_id,  &av);
   av.intvalue = ptr -> bioentityid;
   retval = AsnWrite(aip, STBIONAMENODE_bioentityid,  &av);
   if (ptr -> name != NULL) {
      av.ptrvalue = ptr -> name;
      retval = AsnWrite(aip, STBIONAMENODE_name,  &av);
   }
   av.intvalue = ptr -> nametypeid;
   retval = AsnWrite(aip, STBIONAMENODE_nametypeid,  &av);
   if (ptr -> db != NULL) {
      av.ptrvalue = ptr -> db;
      retval = AsnWrite(aip, STBIONAMENODE_db,  &av);
   }
   if (ptr -> access != NULL) {
      av.ptrvalue = ptr -> access;
      retval = AsnWrite(aip, STBIONAMENODE_access,  &av);
   }
   av.intvalue = ptr -> identifier;
   retval = AsnWrite(aip, STBIONAMENODE_identifier,  &av);
   av.intvalue = ptr -> field;
   retval = AsnWrite(aip, STBIONAMENODE_field,  &av);
   av.intvalue = ptr -> official;
   retval = AsnWrite(aip, STBIONAMENODE_official,  &av);
   av.intvalue = ptr -> deprecated;
   retval = AsnWrite(aip, STBIONAMENODE_deprecated,  &av);
   if (ptr -> date != NULL) {
      av.ptrvalue = ptr -> date;
      retval = AsnWrite(aip, STBIONAMENODE_date,  &av);
   }
   av.intvalue = ptr -> ruleid;
   retval = AsnWrite(aip, STBIONAMENODE_ruleid,  &av);
   av.intvalue = ptr -> action;
   retval = AsnWrite(aip, STBIONAMENODE_action,  &av);
   if (ptr -> actiondate != NULL) {
      av.ptrvalue = ptr -> actiondate;
      retval = AsnWrite(aip, STBIONAMENODE_actiondate,  &av);
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
*    StBionameFree()
*
**************************************************/
NLM_EXTERN 
StBionamePtr LIBCALL
StBionameFree(StBionamePtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   AsnGenericUserSeqOfFree(ptr,  (AsnOptFreeFunc) StBionameNodeFree);
   return NULL;
}


/**************************************************
*
*    StBionameAsnRead()
*
**************************************************/
NLM_EXTERN 
StBionamePtr LIBCALL
StBionameAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   StBionamePtr ptr;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* StBioname ::= (self contained) */
      atp = AsnReadId(aip, amp, STBIONAME);
   } else {
      atp = AsnLinkType(orig, STBIONAME);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   func = NULL;

   ptr  = AsnGenericUserSeqOfAsnRead(aip, amp, atp, &isError, (AsnReadFunc) StBionameNodeAsnRead, (AsnOptFreeFunc) StBionameNodeFree);
   if (isError && ptr  == NULL) {
      goto erret;
   }



ret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return ptr;

erret:
   aip -> io_failure = TRUE;
   ptr = StBionameFree(ptr);
   goto ret;
}



/**************************************************
*
*    StBionameAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
StBionameAsnWrite(StBionamePtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, STBIONAME);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   retval = AsnGenericUserSeqOfAsnWrite(ptr , (AsnWriteFunc) StBionameNodeAsnWrite, aip, atp, STBIONAME_E);
   retval = TRUE;

erret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return retval;
}



/**************************************************
*
*    StSecondrefsNodeNew()
*
**************************************************/
NLM_EXTERN 
StSecondrefsNodePtr LIBCALL
StSecondrefsNodeNew(void)
{
   StSecondrefsNodePtr ptr = MemNew((size_t) sizeof(StSecondrefsNode));

   return ptr;

}


/**************************************************
*
*    StSecondrefsNodeFree()
*
**************************************************/
NLM_EXTERN 
StSecondrefsNodePtr LIBCALL
StSecondrefsNodeFree(StSecondrefsNodePtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   MemFree(ptr -> db);
   MemFree(ptr -> access);
   return MemFree(ptr);
}


/**************************************************
*
*    StSecondrefsNodeAsnRead()
*
**************************************************/
NLM_EXTERN 
StSecondrefsNodePtr LIBCALL
StSecondrefsNodeAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   StSecondrefsNodePtr ptr;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* StSecondrefsNode ::= (self contained) */
      atp = AsnReadId(aip, amp, STSECONDREFSNODE);
   } else {
      atp = AsnLinkType(orig, STSECONDREFSNODE);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   ptr = StSecondrefsNodeNew();
   if (ptr == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
      goto erret;
   }

   atp = AsnReadId(aip,amp, atp);
   func = NULL;

   if (atp == STSECONDREFSNODE_id) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> id = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STSECONDREFSNODE_bionameid) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> bionameid = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STSECONDREFSNODE_db) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> db = av.ptrvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STSECONDREFSNODE_access) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> access = av.ptrvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STSECONDREFSNODE_field) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> field = av.intvalue;
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
   ptr = StSecondrefsNodeFree(ptr);
   goto ret;
}



/**************************************************
*
*    StSecondrefsNodeAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
StSecondrefsNodeAsnWrite(StSecondrefsNodePtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, STSECONDREFSNODE);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) {
      goto erret;
   }

   av.intvalue = ptr -> id;
   retval = AsnWrite(aip, STSECONDREFSNODE_id,  &av);
   av.intvalue = ptr -> bionameid;
   retval = AsnWrite(aip, STSECONDREFSNODE_bionameid,  &av);
   if (ptr -> db != NULL) {
      av.ptrvalue = ptr -> db;
      retval = AsnWrite(aip, STSECONDREFSNODE_db,  &av);
   }
   if (ptr -> access != NULL) {
      av.ptrvalue = ptr -> access;
      retval = AsnWrite(aip, STSECONDREFSNODE_access,  &av);
   }
   av.intvalue = ptr -> field;
   retval = AsnWrite(aip, STSECONDREFSNODE_field,  &av);
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
*    StSecondrefsFree()
*
**************************************************/
NLM_EXTERN 
StSecondrefsPtr LIBCALL
StSecondrefsFree(StSecondrefsPtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   AsnGenericUserSeqOfFree(ptr,  (AsnOptFreeFunc) StSecondrefsNodeFree);
   return NULL;
}


/**************************************************
*
*    StSecondrefsAsnRead()
*
**************************************************/
NLM_EXTERN 
StSecondrefsPtr LIBCALL
StSecondrefsAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   StSecondrefsPtr ptr;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* StSecondrefs ::= (self contained) */
      atp = AsnReadId(aip, amp, STSECONDREFS);
   } else {
      atp = AsnLinkType(orig, STSECONDREFS);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   func = NULL;

   ptr  = AsnGenericUserSeqOfAsnRead(aip, amp, atp, &isError, (AsnReadFunc) StSecondrefsNodeAsnRead, (AsnOptFreeFunc) StSecondrefsNodeFree);
   if (isError && ptr  == NULL) {
      goto erret;
   }



ret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return ptr;

erret:
   aip -> io_failure = TRUE;
   ptr = StSecondrefsFree(ptr);
   goto ret;
}



/**************************************************
*
*    StSecondrefsAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
StSecondrefsAsnWrite(StSecondrefsPtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, STSECONDREFS);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   retval = AsnGenericUserSeqOfAsnWrite(ptr , (AsnWriteFunc) StSecondrefsNodeAsnWrite, aip, atp, STSECONDREFS_E);
   retval = TRUE;

erret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return retval;
}



/**************************************************
*
*    StGichromidNodeNew()
*
**************************************************/
NLM_EXTERN 
StGichromidNodePtr LIBCALL
StGichromidNodeNew(void)
{
   StGichromidNodePtr ptr = MemNew((size_t) sizeof(StGichromidNode));

   return ptr;

}


/**************************************************
*
*    StGichromidNodeFree()
*
**************************************************/
NLM_EXTERN 
StGichromidNodePtr LIBCALL
StGichromidNodeFree(StGichromidNodePtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   return MemFree(ptr);
}


/**************************************************
*
*    StGichromidNodeAsnRead()
*
**************************************************/
NLM_EXTERN 
StGichromidNodePtr LIBCALL
StGichromidNodeAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   StGichromidNodePtr ptr;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* StGichromidNode ::= (self contained) */
      atp = AsnReadId(aip, amp, STGICHROMIDNODE);
   } else {
      atp = AsnLinkType(orig, STGICHROMIDNODE);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   ptr = StGichromidNodeNew();
   if (ptr == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
      goto erret;
   }

   atp = AsnReadId(aip,amp, atp);
   func = NULL;

   if (atp == STGICHROMIDNODE_id) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> id = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STGICHROMIDNODE_gi) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> gi = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STGICHROMIDNODE_contiggi) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> contiggi = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STGICHROMIDNODE_chromid) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> chromid = av.intvalue;
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
   ptr = StGichromidNodeFree(ptr);
   goto ret;
}



/**************************************************
*
*    StGichromidNodeAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
StGichromidNodeAsnWrite(StGichromidNodePtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, STGICHROMIDNODE);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) {
      goto erret;
   }

   av.intvalue = ptr -> id;
   retval = AsnWrite(aip, STGICHROMIDNODE_id,  &av);
   av.intvalue = ptr -> gi;
   retval = AsnWrite(aip, STGICHROMIDNODE_gi,  &av);
   av.intvalue = ptr -> contiggi;
   retval = AsnWrite(aip, STGICHROMIDNODE_contiggi,  &av);
   av.intvalue = ptr -> chromid;
   retval = AsnWrite(aip, STGICHROMIDNODE_chromid,  &av);
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
*    StGichromidFree()
*
**************************************************/
NLM_EXTERN 
StGichromidPtr LIBCALL
StGichromidFree(StGichromidPtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   AsnGenericUserSeqOfFree(ptr,  (AsnOptFreeFunc) StGichromidNodeFree);
   return NULL;
}


/**************************************************
*
*    StGichromidAsnRead()
*
**************************************************/
NLM_EXTERN 
StGichromidPtr LIBCALL
StGichromidAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   StGichromidPtr ptr;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* StGichromid ::= (self contained) */
      atp = AsnReadId(aip, amp, STGICHROMID);
   } else {
      atp = AsnLinkType(orig, STGICHROMID);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   func = NULL;

   ptr  = AsnGenericUserSeqOfAsnRead(aip, amp, atp, &isError, (AsnReadFunc) StGichromidNodeAsnRead, (AsnOptFreeFunc) StGichromidNodeFree);
   if (isError && ptr  == NULL) {
      goto erret;
   }



ret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return ptr;

erret:
   aip -> io_failure = TRUE;
   ptr = StGichromidFree(ptr);
   goto ret;
}



/**************************************************
*
*    StGichromidAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
StGichromidAsnWrite(StGichromidPtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, STGICHROMID);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   retval = AsnGenericUserSeqOfAsnWrite(ptr , (AsnWriteFunc) StGichromidNodeAsnWrite, aip, atp, STGICHROMID_E);
   retval = TRUE;

erret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return retval;
}



/**************************************************
*
*    StContigchromidNodeNew()
*
**************************************************/
NLM_EXTERN 
StContigchromidNodePtr LIBCALL
StContigchromidNodeNew(void)
{
   StContigchromidNodePtr ptr = MemNew((size_t) sizeof(StContigchromidNode));

   return ptr;

}


/**************************************************
*
*    StContigchromidNodeFree()
*
**************************************************/
NLM_EXTERN 
StContigchromidNodePtr LIBCALL
StContigchromidNodeFree(StContigchromidNodePtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   return MemFree(ptr);
}


/**************************************************
*
*    StContigchromidNodeAsnRead()
*
**************************************************/
NLM_EXTERN 
StContigchromidNodePtr LIBCALL
StContigchromidNodeAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   StContigchromidNodePtr ptr;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* StContigchromidNode ::= (self contained) */
      atp = AsnReadId(aip, amp, STCONTIGCHROMIDNODE);
   } else {
      atp = AsnLinkType(orig, STCONTIGCHROMIDNODE);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   ptr = StContigchromidNodeNew();
   if (ptr == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
      goto erret;
   }

   atp = AsnReadId(aip,amp, atp);
   func = NULL;

   if (atp == STCONTIGCHROMIDNODE_id) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> id = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STCONTIGCHROMIDNODE_contiggi) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> contiggi = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STCONTIGCHROMIDNODE_topgi) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> topgi = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STCONTIGCHROMIDNODE_chromid) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> chromid = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STCONTIGCHROMIDNODE_changed) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> changed = av.intvalue;
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
   ptr = StContigchromidNodeFree(ptr);
   goto ret;
}



/**************************************************
*
*    StContigchromidNodeAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
StContigchromidNodeAsnWrite(StContigchromidNodePtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, STCONTIGCHROMIDNODE);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) {
      goto erret;
   }

   av.intvalue = ptr -> id;
   retval = AsnWrite(aip, STCONTIGCHROMIDNODE_id,  &av);
   av.intvalue = ptr -> contiggi;
   retval = AsnWrite(aip, STCONTIGCHROMIDNODE_contiggi,  &av);
   av.intvalue = ptr -> topgi;
   retval = AsnWrite(aip, STCONTIGCHROMIDNODE_topgi,  &av);
   av.intvalue = ptr -> chromid;
   retval = AsnWrite(aip, STCONTIGCHROMIDNODE_chromid,  &av);
   av.intvalue = ptr -> changed;
   retval = AsnWrite(aip, STCONTIGCHROMIDNODE_changed,  &av);
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
*    StContigchromidFree()
*
**************************************************/
NLM_EXTERN 
StContigchromidPtr LIBCALL
StContigchromidFree(StContigchromidPtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   AsnGenericUserSeqOfFree(ptr,  (AsnOptFreeFunc) StContigchromidNodeFree);
   return NULL;
}


/**************************************************
*
*    StContigchromidAsnRead()
*
**************************************************/
NLM_EXTERN 
StContigchromidPtr LIBCALL
StContigchromidAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   StContigchromidPtr ptr;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* StContigchromid ::= (self contained) */
      atp = AsnReadId(aip, amp, STCONTIGCHROMID);
   } else {
      atp = AsnLinkType(orig, STCONTIGCHROMID);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   func = NULL;

   ptr  = AsnGenericUserSeqOfAsnRead(aip, amp, atp, &isError, (AsnReadFunc) StContigchromidNodeAsnRead, (AsnOptFreeFunc) StContigchromidNodeFree);
   if (isError && ptr  == NULL) {
      goto erret;
   }



ret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return ptr;

erret:
   aip -> io_failure = TRUE;
   ptr = StContigchromidFree(ptr);
   goto ret;
}



/**************************************************
*
*    StContigchromidAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
StContigchromidAsnWrite(StContigchromidPtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, STCONTIGCHROMID);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   retval = AsnGenericUserSeqOfAsnWrite(ptr , (AsnWriteFunc) StContigchromidNodeAsnWrite, aip, atp, STCONTIGCHROMID_E);
   retval = TRUE;

erret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return retval;
}



/**************************************************
*
*    StGichromNodeNew()
*
**************************************************/
NLM_EXTERN 
StGichromNodePtr LIBCALL
StGichromNodeNew(void)
{
   StGichromNodePtr ptr = MemNew((size_t) sizeof(StGichromNode));

   return ptr;

}


/**************************************************
*
*    StGichromNodeFree()
*
**************************************************/
NLM_EXTERN 
StGichromNodePtr LIBCALL
StGichromNodeFree(StGichromNodePtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   MemFree(ptr -> chromnum);
   return MemFree(ptr);
}


/**************************************************
*
*    StGichromNodeAsnRead()
*
**************************************************/
NLM_EXTERN 
StGichromNodePtr LIBCALL
StGichromNodeAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   StGichromNodePtr ptr;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* StGichromNode ::= (self contained) */
      atp = AsnReadId(aip, amp, STGICHROMNODE);
   } else {
      atp = AsnLinkType(orig, STGICHROMNODE);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   ptr = StGichromNodeNew();
   if (ptr == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
      goto erret;
   }

   atp = AsnReadId(aip,amp, atp);
   func = NULL;

   if (atp == STGICHROMNODE_id) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> id = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STGICHROMNODE_gi) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> gi = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STGICHROMNODE_chromnum) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> chromnum = av.ptrvalue;
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
   ptr = StGichromNodeFree(ptr);
   goto ret;
}



/**************************************************
*
*    StGichromNodeAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
StGichromNodeAsnWrite(StGichromNodePtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, STGICHROMNODE);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) {
      goto erret;
   }

   av.intvalue = ptr -> id;
   retval = AsnWrite(aip, STGICHROMNODE_id,  &av);
   av.intvalue = ptr -> gi;
   retval = AsnWrite(aip, STGICHROMNODE_gi,  &av);
   if (ptr -> chromnum != NULL) {
      av.ptrvalue = ptr -> chromnum;
      retval = AsnWrite(aip, STGICHROMNODE_chromnum,  &av);
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
*    StGichromFree()
*
**************************************************/
NLM_EXTERN 
StGichromPtr LIBCALL
StGichromFree(StGichromPtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   AsnGenericUserSeqOfFree(ptr,  (AsnOptFreeFunc) StGichromNodeFree);
   return NULL;
}


/**************************************************
*
*    StGichromAsnRead()
*
**************************************************/
NLM_EXTERN 
StGichromPtr LIBCALL
StGichromAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   StGichromPtr ptr;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* StGichrom ::= (self contained) */
      atp = AsnReadId(aip, amp, STGICHROM);
   } else {
      atp = AsnLinkType(orig, STGICHROM);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   func = NULL;

   ptr  = AsnGenericUserSeqOfAsnRead(aip, amp, atp, &isError, (AsnReadFunc) StGichromNodeAsnRead, (AsnOptFreeFunc) StGichromNodeFree);
   if (isError && ptr  == NULL) {
      goto erret;
   }



ret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return ptr;

erret:
   aip -> io_failure = TRUE;
   ptr = StGichromFree(ptr);
   goto ret;
}



/**************************************************
*
*    StGichromAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
StGichromAsnWrite(StGichromPtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, STGICHROM);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   retval = AsnGenericUserSeqOfAsnWrite(ptr , (AsnWriteFunc) StGichromNodeAsnWrite, aip, atp, STGICHROM_E);
   retval = TRUE;

erret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return retval;
}



/**************************************************
*
*    StContigchromNodeNew()
*
**************************************************/
NLM_EXTERN 
StContigchromNodePtr LIBCALL
StContigchromNodeNew(void)
{
   StContigchromNodePtr ptr = MemNew((size_t) sizeof(StContigchromNode));

   return ptr;

}


/**************************************************
*
*    StContigchromNodeFree()
*
**************************************************/
NLM_EXTERN 
StContigchromNodePtr LIBCALL
StContigchromNodeFree(StContigchromNodePtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   MemFree(ptr -> chromnum);
   return MemFree(ptr);
}


/**************************************************
*
*    StContigchromNodeAsnRead()
*
**************************************************/
NLM_EXTERN 
StContigchromNodePtr LIBCALL
StContigchromNodeAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   StContigchromNodePtr ptr;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* StContigchromNode ::= (self contained) */
      atp = AsnReadId(aip, amp, STCONTIGCHROMNODE);
   } else {
      atp = AsnLinkType(orig, STCONTIGCHROMNODE);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   ptr = StContigchromNodeNew();
   if (ptr == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
      goto erret;
   }

   atp = AsnReadId(aip,amp, atp);
   func = NULL;

   if (atp == STCONTIGCHROMNODE_id) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> id = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STCONTIGCHROMNODE_contiggi) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> contiggi = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STCONTIGCHROMNODE_chromnum) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> chromnum = av.ptrvalue;
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
   ptr = StContigchromNodeFree(ptr);
   goto ret;
}



/**************************************************
*
*    StContigchromNodeAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
StContigchromNodeAsnWrite(StContigchromNodePtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, STCONTIGCHROMNODE);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) {
      goto erret;
   }

   av.intvalue = ptr -> id;
   retval = AsnWrite(aip, STCONTIGCHROMNODE_id,  &av);
   av.intvalue = ptr -> contiggi;
   retval = AsnWrite(aip, STCONTIGCHROMNODE_contiggi,  &av);
   if (ptr -> chromnum != NULL) {
      av.ptrvalue = ptr -> chromnum;
      retval = AsnWrite(aip, STCONTIGCHROMNODE_chromnum,  &av);
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
*    StContigchromFree()
*
**************************************************/
NLM_EXTERN 
StContigchromPtr LIBCALL
StContigchromFree(StContigchromPtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   AsnGenericUserSeqOfFree(ptr,  (AsnOptFreeFunc) StContigchromNodeFree);
   return NULL;
}


/**************************************************
*
*    StContigchromAsnRead()
*
**************************************************/
NLM_EXTERN 
StContigchromPtr LIBCALL
StContigchromAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   StContigchromPtr ptr;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* StContigchrom ::= (self contained) */
      atp = AsnReadId(aip, amp, STCONTIGCHROM);
   } else {
      atp = AsnLinkType(orig, STCONTIGCHROM);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   func = NULL;

   ptr  = AsnGenericUserSeqOfAsnRead(aip, amp, atp, &isError, (AsnReadFunc) StContigchromNodeAsnRead, (AsnOptFreeFunc) StContigchromNodeFree);
   if (isError && ptr  == NULL) {
      goto erret;
   }



ret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return ptr;

erret:
   aip -> io_failure = TRUE;
   ptr = StContigchromFree(ptr);
   goto ret;
}



/**************************************************
*
*    StContigchromAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
StContigchromAsnWrite(StContigchromPtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, STCONTIGCHROM);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   retval = AsnGenericUserSeqOfAsnWrite(ptr , (AsnWriteFunc) StContigchromNodeAsnWrite, aip, atp, STCONTIGCHROM_E);
   retval = TRUE;

erret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return retval;
}



/**************************************************
*
*    StSourcefileNodeNew()
*
**************************************************/
NLM_EXTERN 
StSourcefileNodePtr LIBCALL
StSourcefileNodeNew(void)
{
   StSourcefileNodePtr ptr = MemNew((size_t) sizeof(StSourcefileNode));

   return ptr;

}


/**************************************************
*
*    StSourcefileNodeFree()
*
**************************************************/
NLM_EXTERN 
StSourcefileNodePtr LIBCALL
StSourcefileNodeFree(StSourcefileNodePtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   MemFree(ptr -> filename);
   MemFree(ptr -> date);
   return MemFree(ptr);
}


/**************************************************
*
*    StSourcefileNodeAsnRead()
*
**************************************************/
NLM_EXTERN 
StSourcefileNodePtr LIBCALL
StSourcefileNodeAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   StSourcefileNodePtr ptr;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* StSourcefileNode ::= (self contained) */
      atp = AsnReadId(aip, amp, STSOURCEFILENODE);
   } else {
      atp = AsnLinkType(orig, STSOURCEFILENODE);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   ptr = StSourcefileNodeNew();
   if (ptr == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
      goto erret;
   }

   atp = AsnReadId(aip,amp, atp);
   func = NULL;

   if (atp == STSOURCEFILENODE_id) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> id = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STSOURCEFILENODE_gi) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> gi = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STSOURCEFILENODE_filename) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> filename = av.ptrvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STSOURCEFILENODE_date) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> date = av.ptrvalue;
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
   ptr = StSourcefileNodeFree(ptr);
   goto ret;
}



/**************************************************
*
*    StSourcefileNodeAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
StSourcefileNodeAsnWrite(StSourcefileNodePtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, STSOURCEFILENODE);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) {
      goto erret;
   }

   av.intvalue = ptr -> id;
   retval = AsnWrite(aip, STSOURCEFILENODE_id,  &av);
   av.intvalue = ptr -> gi;
   retval = AsnWrite(aip, STSOURCEFILENODE_gi,  &av);
   if (ptr -> filename != NULL) {
      av.ptrvalue = ptr -> filename;
      retval = AsnWrite(aip, STSOURCEFILENODE_filename,  &av);
   }
   if (ptr -> date != NULL) {
      av.ptrvalue = ptr -> date;
      retval = AsnWrite(aip, STSOURCEFILENODE_date,  &av);
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
*    StSourcefileFree()
*
**************************************************/
NLM_EXTERN 
StSourcefilePtr LIBCALL
StSourcefileFree(StSourcefilePtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   AsnGenericUserSeqOfFree(ptr,  (AsnOptFreeFunc) StSourcefileNodeFree);
   return NULL;
}


/**************************************************
*
*    StSourcefileAsnRead()
*
**************************************************/
NLM_EXTERN 
StSourcefilePtr LIBCALL
StSourcefileAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   StSourcefilePtr ptr;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* StSourcefile ::= (self contained) */
      atp = AsnReadId(aip, amp, STSOURCEFILE);
   } else {
      atp = AsnLinkType(orig, STSOURCEFILE);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   func = NULL;

   ptr  = AsnGenericUserSeqOfAsnRead(aip, amp, atp, &isError, (AsnReadFunc) StSourcefileNodeAsnRead, (AsnOptFreeFunc) StSourcefileNodeFree);
   if (isError && ptr  == NULL) {
      goto erret;
   }



ret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return ptr;

erret:
   aip -> io_failure = TRUE;
   ptr = StSourcefileFree(ptr);
   goto ret;
}



/**************************************************
*
*    StSourcefileAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
StSourcefileAsnWrite(StSourcefilePtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, STSOURCEFILE);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   retval = AsnGenericUserSeqOfAsnWrite(ptr , (AsnWriteFunc) StSourcefileNodeAsnWrite, aip, atp, STSOURCEFILE_E);
   retval = TRUE;

erret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return retval;
}



/**************************************************
*
*    StGeneObjectNodeNew()
*
**************************************************/
NLM_EXTERN 
StGeneObjectNodePtr LIBCALL
StGeneObjectNodeNew(void)
{
   StGeneObjectNodePtr ptr = MemNew((size_t) sizeof(StGeneObjectNode));

   return ptr;

}


/**************************************************
*
*    StGeneObjectNodeFree()
*
**************************************************/
NLM_EXTERN 
StGeneObjectNodePtr LIBCALL
StGeneObjectNodeFree(StGeneObjectNodePtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   MemFree(ptr -> status);
   return MemFree(ptr);
}


/**************************************************
*
*    StGeneObjectNodeAsnRead()
*
**************************************************/
NLM_EXTERN 
StGeneObjectNodePtr LIBCALL
StGeneObjectNodeAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   StGeneObjectNodePtr ptr;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* StGeneObjectNode ::= (self contained) */
      atp = AsnReadId(aip, amp, STGENEOBJECTNODE);
   } else {
      atp = AsnLinkType(orig, STGENEOBJECTNODE);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   ptr = StGeneObjectNodeNew();
   if (ptr == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
      goto erret;
   }

   atp = AsnReadId(aip,amp, atp);
   func = NULL;

   if (atp == STGENEOBJECTNODE_id) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> id = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STGENEOBJECTNODE_taxid) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> taxid = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STGENEOBJECTNODE_geneid) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> geneid = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STGENEOBJECTNODE_status) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> status = av.ptrvalue;
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
   ptr = StGeneObjectNodeFree(ptr);
   goto ret;
}



/**************************************************
*
*    StGeneObjectNodeAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
StGeneObjectNodeAsnWrite(StGeneObjectNodePtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, STGENEOBJECTNODE);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) {
      goto erret;
   }

   av.intvalue = ptr -> id;
   retval = AsnWrite(aip, STGENEOBJECTNODE_id,  &av);
   av.intvalue = ptr -> taxid;
   retval = AsnWrite(aip, STGENEOBJECTNODE_taxid,  &av);
   av.intvalue = ptr -> geneid;
   retval = AsnWrite(aip, STGENEOBJECTNODE_geneid,  &av);
   if (ptr -> status != NULL) {
      av.ptrvalue = ptr -> status;
      retval = AsnWrite(aip, STGENEOBJECTNODE_status,  &av);
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
*    StGeneObjectFree()
*
**************************************************/
NLM_EXTERN 
StGeneObjectPtr LIBCALL
StGeneObjectFree(StGeneObjectPtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   AsnGenericUserSeqOfFree(ptr,  (AsnOptFreeFunc) StGeneObjectNodeFree);
   return NULL;
}


/**************************************************
*
*    StGeneObjectAsnRead()
*
**************************************************/
NLM_EXTERN 
StGeneObjectPtr LIBCALL
StGeneObjectAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   StGeneObjectPtr ptr;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* StGeneObject ::= (self contained) */
      atp = AsnReadId(aip, amp, STGENEOBJECT);
   } else {
      atp = AsnLinkType(orig, STGENEOBJECT);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   func = NULL;

   ptr  = AsnGenericUserSeqOfAsnRead(aip, amp, atp, &isError, (AsnReadFunc) StGeneObjectNodeAsnRead, (AsnOptFreeFunc) StGeneObjectNodeFree);
   if (isError && ptr  == NULL) {
      goto erret;
   }



ret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return ptr;

erret:
   aip -> io_failure = TRUE;
   ptr = StGeneObjectFree(ptr);
   goto ret;
}



/**************************************************
*
*    StGeneObjectAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
StGeneObjectAsnWrite(StGeneObjectPtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, STGENEOBJECT);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   retval = AsnGenericUserSeqOfAsnWrite(ptr , (AsnWriteFunc) StGeneObjectNodeAsnWrite, aip, atp, STGENEOBJECT_E);
   retval = TRUE;

erret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return retval;
}



/**************************************************
*
*    StGeneGiNodeNew()
*
**************************************************/
NLM_EXTERN 
StGeneGiNodePtr LIBCALL
StGeneGiNodeNew(void)
{
   StGeneGiNodePtr ptr = MemNew((size_t) sizeof(StGeneGiNode));

   return ptr;

}


/**************************************************
*
*    StGeneGiNodeFree()
*
**************************************************/
NLM_EXTERN 
StGeneGiNodePtr LIBCALL
StGeneGiNodeFree(StGeneGiNodePtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   MemFree(ptr -> moleculetype);
   return MemFree(ptr);
}


/**************************************************
*
*    StGeneGiNodeAsnRead()
*
**************************************************/
NLM_EXTERN 
StGeneGiNodePtr LIBCALL
StGeneGiNodeAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   StGeneGiNodePtr ptr;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* StGeneGiNode ::= (self contained) */
      atp = AsnReadId(aip, amp, STGENEGINODE);
   } else {
      atp = AsnLinkType(orig, STGENEGINODE);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   ptr = StGeneGiNodeNew();
   if (ptr == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
      goto erret;
   }

   atp = AsnReadId(aip,amp, atp);
   func = NULL;

   if (atp == STGENEGINODE_id) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> id = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STGENEGINODE_geneobjectid) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> geneobjectid = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STGENEGINODE_gi) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> gi = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STGENEGINODE_moleculetype) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> moleculetype = av.ptrvalue;
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
   ptr = StGeneGiNodeFree(ptr);
   goto ret;
}



/**************************************************
*
*    StGeneGiNodeAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
StGeneGiNodeAsnWrite(StGeneGiNodePtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, STGENEGINODE);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) {
      goto erret;
   }

   av.intvalue = ptr -> id;
   retval = AsnWrite(aip, STGENEGINODE_id,  &av);
   av.intvalue = ptr -> geneobjectid;
   retval = AsnWrite(aip, STGENEGINODE_geneobjectid,  &av);
   av.intvalue = ptr -> gi;
   retval = AsnWrite(aip, STGENEGINODE_gi,  &av);
   if (ptr -> moleculetype != NULL) {
      av.ptrvalue = ptr -> moleculetype;
      retval = AsnWrite(aip, STGENEGINODE_moleculetype,  &av);
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
*    StGeneGiFree()
*
**************************************************/
NLM_EXTERN 
StGeneGiPtr LIBCALL
StGeneGiFree(StGeneGiPtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   AsnGenericUserSeqOfFree(ptr,  (AsnOptFreeFunc) StGeneGiNodeFree);
   return NULL;
}


/**************************************************
*
*    StGeneGiAsnRead()
*
**************************************************/
NLM_EXTERN 
StGeneGiPtr LIBCALL
StGeneGiAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   StGeneGiPtr ptr;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* StGeneGi ::= (self contained) */
      atp = AsnReadId(aip, amp, STGENEGI);
   } else {
      atp = AsnLinkType(orig, STGENEGI);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   func = NULL;

   ptr  = AsnGenericUserSeqOfAsnRead(aip, amp, atp, &isError, (AsnReadFunc) StGeneGiNodeAsnRead, (AsnOptFreeFunc) StGeneGiNodeFree);
   if (isError && ptr  == NULL) {
      goto erret;
   }



ret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return ptr;

erret:
   aip -> io_failure = TRUE;
   ptr = StGeneGiFree(ptr);
   goto ret;
}



/**************************************************
*
*    StGeneGiAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
StGeneGiAsnWrite(StGeneGiPtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, STGENEGI);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   retval = AsnGenericUserSeqOfAsnWrite(ptr , (AsnWriteFunc) StGeneGiNodeAsnWrite, aip, atp, STGENEGI_E);
   retval = TRUE;

erret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return retval;
}



/**************************************************
*
*    StGeneDbxrefNodeNew()
*
**************************************************/
NLM_EXTERN 
StGeneDbxrefNodePtr LIBCALL
StGeneDbxrefNodeNew(void)
{
   StGeneDbxrefNodePtr ptr = MemNew((size_t) sizeof(StGeneDbxrefNode));

   return ptr;

}


/**************************************************
*
*    StGeneDbxrefNodeFree()
*
**************************************************/
NLM_EXTERN 
StGeneDbxrefNodePtr LIBCALL
StGeneDbxrefNodeFree(StGeneDbxrefNodePtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   MemFree(ptr -> dbname);
   MemFree(ptr -> dbaccess);
   return MemFree(ptr);
}


/**************************************************
*
*    StGeneDbxrefNodeAsnRead()
*
**************************************************/
NLM_EXTERN 
StGeneDbxrefNodePtr LIBCALL
StGeneDbxrefNodeAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   StGeneDbxrefNodePtr ptr;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* StGeneDbxrefNode ::= (self contained) */
      atp = AsnReadId(aip, amp, STGENEDBXREFNODE);
   } else {
      atp = AsnLinkType(orig, STGENEDBXREFNODE);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   ptr = StGeneDbxrefNodeNew();
   if (ptr == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
      goto erret;
   }

   atp = AsnReadId(aip,amp, atp);
   func = NULL;

   if (atp == STGENEDBXREFNODE_id) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> id = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STGENEDBXREFNODE_geneobjectid) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> geneobjectid = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STGENEDBXREFNODE_dbname) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> dbname = av.ptrvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == STGENEDBXREFNODE_dbaccess) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> dbaccess = av.ptrvalue;
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
   ptr = StGeneDbxrefNodeFree(ptr);
   goto ret;
}



/**************************************************
*
*    StGeneDbxrefNodeAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
StGeneDbxrefNodeAsnWrite(StGeneDbxrefNodePtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, STGENEDBXREFNODE);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) {
      goto erret;
   }

   av.intvalue = ptr -> id;
   retval = AsnWrite(aip, STGENEDBXREFNODE_id,  &av);
   av.intvalue = ptr -> geneobjectid;
   retval = AsnWrite(aip, STGENEDBXREFNODE_geneobjectid,  &av);
   if (ptr -> dbname != NULL) {
      av.ptrvalue = ptr -> dbname;
      retval = AsnWrite(aip, STGENEDBXREFNODE_dbname,  &av);
   }
   if (ptr -> dbaccess != NULL) {
      av.ptrvalue = ptr -> dbaccess;
      retval = AsnWrite(aip, STGENEDBXREFNODE_dbaccess,  &av);
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
*    StGeneDbxrefFree()
*
**************************************************/
NLM_EXTERN 
StGeneDbxrefPtr LIBCALL
StGeneDbxrefFree(StGeneDbxrefPtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   AsnGenericUserSeqOfFree(ptr,  (AsnOptFreeFunc) StGeneDbxrefNodeFree);
   return NULL;
}


/**************************************************
*
*    StGeneDbxrefAsnRead()
*
**************************************************/
NLM_EXTERN 
StGeneDbxrefPtr LIBCALL
StGeneDbxrefAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   StGeneDbxrefPtr ptr;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* StGeneDbxref ::= (self contained) */
      atp = AsnReadId(aip, amp, STGENEDBXREF);
   } else {
      atp = AsnLinkType(orig, STGENEDBXREF);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   func = NULL;

   ptr  = AsnGenericUserSeqOfAsnRead(aip, amp, atp, &isError, (AsnReadFunc) StGeneDbxrefNodeAsnRead, (AsnOptFreeFunc) StGeneDbxrefNodeFree);
   if (isError && ptr  == NULL) {
      goto erret;
   }



ret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return ptr;

erret:
   aip -> io_failure = TRUE;
   ptr = StGeneDbxrefFree(ptr);
   goto ret;
}



/**************************************************
*
*    StGeneDbxrefAsnWrite()
*
**************************************************/
NLM_EXTERN Boolean LIBCALL 
StGeneDbxrefAsnWrite(StGeneDbxrefPtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objslristrucAsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, STGENEDBXREF);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   retval = AsnGenericUserSeqOfAsnWrite(ptr , (AsnWriteFunc) StGeneDbxrefNodeAsnWrite, aip, atp, STGENEDBXREF_E);
   retval = TRUE;

erret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return retval;
}

