#ifndef _objprebind_ 
#define _objprebind_ 

#undef NLM_EXTERN
#ifdef NLM_IMPORT
#define NLM_EXTERN NLM_IMPORT
#else
#define NLM_EXTERN extern
#endif


#ifdef __cplusplus
extern "C" { /* } */
#endif


/**************************************************
*
*    Generated objects for Module PREBIND
*    Generated using ASNCODE Revision: 6.14 at Feb 20, 2003  1:29 PM
*
**************************************************/

NLM_EXTERN Boolean LIBCALL
objprebindAsnLoad PROTO((void));


/**************************************************
*
*    PREBINDAcc
*
**************************************************/
typedef struct struct_PREBIND_Acc {
   struct struct_PREBIND_Acc PNTR next;
   CharPtr   acc;
   Int4   gi;
   CharPtr   db;
   CharPtr   locus;
   CharPtr   pprod;
   Int4   tax;
   CharPtr   update;
} PREBINDAcc, PNTR PREBINDAccPtr;


NLM_EXTERN PREBINDAccPtr LIBCALL PREBINDAccFree PROTO ((PREBINDAccPtr ));
NLM_EXTERN PREBINDAccPtr LIBCALL PREBINDAccNew PROTO (( void ));
NLM_EXTERN PREBINDAccPtr LIBCALL PREBINDAccAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL PREBINDAccAsnWrite PROTO (( PREBINDAccPtr , AsnIoPtr, AsnTypePtr));



/**************************************************
*
*    PREBINDAccSet
*
**************************************************/
typedef struct struct_PREBIND_Acc_set {
   struct struct_PREBIND_Acc PNTR   records;
} PREBINDAccSet, PNTR PREBINDAccSetPtr;


NLM_EXTERN PREBINDAccSetPtr LIBCALL PREBINDAccSetFree PROTO ((PREBINDAccSetPtr ));
NLM_EXTERN PREBINDAccSetPtr LIBCALL PREBINDAccSetNew PROTO (( void ));
NLM_EXTERN PREBINDAccSetPtr LIBCALL PREBINDAccSetAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL PREBINDAccSetAsnWrite PROTO (( PREBINDAccSetPtr , AsnIoPtr, AsnTypePtr));



/**************************************************
*
*    PREBINDSearch
*
**************************************************/
typedef struct struct_PREBIND_Search {
   struct struct_PREBIND_Search PNTR next;
   Int4   gi;
   CharPtr   name;
   Int4   source;
   Int4   taxid;
   CharPtr   searched;
   Uint2   status;
   /* following #defines are for enumerated type, not used by object loaders */
#define PREBIND_Search_status_not_approved 0
#define PREBIND_Search_status_approved 1
#define PREBIND_Search_status_search_queued 2
#define PREBIND_Search_status_search_completed 3

   Int4   namered;
   Int4   results;
} PREBINDSearch, PNTR PREBINDSearchPtr;


NLM_EXTERN PREBINDSearchPtr LIBCALL PREBINDSearchFree PROTO ((PREBINDSearchPtr ));
NLM_EXTERN PREBINDSearchPtr LIBCALL PREBINDSearchNew PROTO (( void ));
NLM_EXTERN PREBINDSearchPtr LIBCALL PREBINDSearchAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL PREBINDSearchAsnWrite PROTO (( PREBINDSearchPtr , AsnIoPtr, AsnTypePtr));



/**************************************************
*
*    PREBINDSearchSet
*
**************************************************/
typedef struct struct_PREBIND_Search_set {
   struct struct_PREBIND_Search PNTR   records;
} PREBINDSearchSet, PNTR PREBINDSearchSetPtr;


NLM_EXTERN PREBINDSearchSetPtr LIBCALL PREBINDSearchSetFree PROTO ((PREBINDSearchSetPtr ));
NLM_EXTERN PREBINDSearchSetPtr LIBCALL PREBINDSearchSetNew PROTO (( void ));
NLM_EXTERN PREBINDSearchSetPtr LIBCALL PREBINDSearchSetAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL PREBINDSearchSetAsnWrite PROTO (( PREBINDSearchSetPtr , AsnIoPtr, AsnTypePtr));



/**************************************************
*
*    PREBINDResult
*
**************************************************/
typedef struct struct_PREBIND_Result {
   struct struct_PREBIND_Result PNTR next;
   Int4   gi;
   CharPtr   name;
   Int4   pmid;
   FloatHi   iscore;
   FloatHi   iscore2;
   Int4   itruth;
   Int4   opno;
   Int4   opyes;
} PREBINDResult, PNTR PREBINDResultPtr;


NLM_EXTERN PREBINDResultPtr LIBCALL PREBINDResultFree PROTO ((PREBINDResultPtr ));
NLM_EXTERN PREBINDResultPtr LIBCALL PREBINDResultNew PROTO (( void ));
NLM_EXTERN PREBINDResultPtr LIBCALL PREBINDResultAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL PREBINDResultAsnWrite PROTO (( PREBINDResultPtr , AsnIoPtr, AsnTypePtr));



/**************************************************
*
*    PREBINDResultSet
*
**************************************************/
typedef struct struct_PREBIND_Result_set {
   struct struct_PREBIND_Result PNTR   records;
} PREBINDResultSet, PNTR PREBINDResultSetPtr;


NLM_EXTERN PREBINDResultSetPtr LIBCALL PREBINDResultSetFree PROTO ((PREBINDResultSetPtr ));
NLM_EXTERN PREBINDResultSetPtr LIBCALL PREBINDResultSetNew PROTO (( void ));
NLM_EXTERN PREBINDResultSetPtr LIBCALL PREBINDResultSetAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL PREBINDResultSetAsnWrite PROTO (( PREBINDResultSetPtr , AsnIoPtr, AsnTypePtr));



/**************************************************
*
*    PREBINDIsvm
*
**************************************************/
typedef struct struct_PREBIND_Isvm {
   struct struct_PREBIND_Isvm PNTR next;
   Int4   pmid;
   Int4   muid;
   FloatHi   iscore;
   FloatHi   iscore2;
   Int4   itruth;
   Int4   opno;
   Int4   opyes;
} PREBINDIsvm, PNTR PREBINDIsvmPtr;


NLM_EXTERN PREBINDIsvmPtr LIBCALL PREBINDIsvmFree PROTO ((PREBINDIsvmPtr ));
NLM_EXTERN PREBINDIsvmPtr LIBCALL PREBINDIsvmNew PROTO (( void ));
NLM_EXTERN PREBINDIsvmPtr LIBCALL PREBINDIsvmAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL PREBINDIsvmAsnWrite PROTO (( PREBINDIsvmPtr , AsnIoPtr, AsnTypePtr));



/**************************************************
*
*    PREBINDIsvmSet
*
**************************************************/
typedef struct struct_PREBIND_Isvm_set {
   struct struct_PREBIND_Isvm PNTR   records;
} PREBINDIsvmSet, PNTR PREBINDIsvmSetPtr;


NLM_EXTERN PREBINDIsvmSetPtr LIBCALL PREBINDIsvmSetFree PROTO ((PREBINDIsvmSetPtr ));
NLM_EXTERN PREBINDIsvmSetPtr LIBCALL PREBINDIsvmSetNew PROTO (( void ));
NLM_EXTERN PREBINDIsvmSetPtr LIBCALL PREBINDIsvmSetAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL PREBINDIsvmSetAsnWrite PROTO (( PREBINDIsvmSetPtr , AsnIoPtr, AsnTypePtr));



/**************************************************
*
*    PREBINDPoint
*
**************************************************/
typedef struct struct_PREBIND_Point {
   struct struct_PREBIND_Point PNTR next;
   Int4   pmid;
   Int4   muid;
   Int4   gia;
   CharPtr   nama;
   Int4   atype;
   Int4   adgi;
   Int4   adf;
   Int4   adt;
   Int4   gib;
   CharPtr   namb;
   Int4   btype;
   Int4   bdgi;
   Int4   bdf;
   Int4   bdt;
   Int4   opno;
   Int4   opyes;
   Int4   piscore;
   Int4   piscore2;
   Int4   pitruth;
} PREBINDPoint, PNTR PREBINDPointPtr;


NLM_EXTERN PREBINDPointPtr LIBCALL PREBINDPointFree PROTO ((PREBINDPointPtr ));
NLM_EXTERN PREBINDPointPtr LIBCALL PREBINDPointNew PROTO (( void ));
NLM_EXTERN PREBINDPointPtr LIBCALL PREBINDPointAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL PREBINDPointAsnWrite PROTO (( PREBINDPointPtr , AsnIoPtr, AsnTypePtr));



/**************************************************
*
*    PREBINDPointSet
*
**************************************************/
typedef struct struct_PREBIND_Point_set {
   struct struct_PREBIND_Point PNTR   records;
} PREBINDPointSet, PNTR PREBINDPointSetPtr;


NLM_EXTERN PREBINDPointSetPtr LIBCALL PREBINDPointSetFree PROTO ((PREBINDPointSetPtr ));
NLM_EXTERN PREBINDPointSetPtr LIBCALL PREBINDPointSetNew PROTO (( void ));
NLM_EXTERN PREBINDPointSetPtr LIBCALL PREBINDPointSetAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL PREBINDPointSetAsnWrite PROTO (( PREBINDPointSetPtr , AsnIoPtr, AsnTypePtr));



/**************************************************
*
*    PREBINDInteractor
*
**************************************************/
typedef struct struct_PREBIND_Interactor {
   struct struct_PREBIND_Interactor PNTR next;
   Int4   gi;
   Int4   pitruth;
   Int4   opyes;
   Int4   opno;
   Int4   piscore;
   Int4   piscore2;
   Int4   num_pmids;
   Int4   known_pmids;
   Int4   opyes_pmids;
   Int4   prob_pmids;
} PREBINDInteractor, PNTR PREBINDInteractorPtr;


NLM_EXTERN PREBINDInteractorPtr LIBCALL PREBINDInteractorFree PROTO ((PREBINDInteractorPtr ));
NLM_EXTERN PREBINDInteractorPtr LIBCALL PREBINDInteractorNew PROTO (( void ));
NLM_EXTERN PREBINDInteractorPtr LIBCALL PREBINDInteractorAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL PREBINDInteractorAsnWrite PROTO (( PREBINDInteractorPtr , AsnIoPtr, AsnTypePtr));



/**************************************************
*
*    PREBINDInteractorSet
*
**************************************************/
typedef struct struct_PREBIND_Interactor_set {
   Int4   num;
   struct struct_PREBIND_Interactor PNTR   records;
} PREBINDInteractorSet, PNTR PREBINDInteractorSetPtr;


NLM_EXTERN PREBINDInteractorSetPtr LIBCALL PREBINDInteractorSetFree PROTO ((PREBINDInteractorSetPtr ));
NLM_EXTERN PREBINDInteractorSetPtr LIBCALL PREBINDInteractorSetNew PROTO (( void ));
NLM_EXTERN PREBINDInteractorSetPtr LIBCALL PREBINDInteractorSetAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL PREBINDInteractorSetAsnWrite PROTO (( PREBINDInteractorSetPtr , AsnIoPtr, AsnTypePtr));

#ifdef __cplusplus
/* { */ }
#endif

#endif /* _objprebind_ */

#undef NLM_EXTERN
#ifdef NLM_EXPORT
#define NLM_EXTERN NLM_EXPORT
#else
#define NLM_EXTERN
#endif

