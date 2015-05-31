#ifndef _objPBT_ 
#define _objPBT_ 

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
*    Generated objects for Module PBT
*    Generated using ASNCODE Revision: 6.14 at May 31, 2004  3:13 PM
*
**************************************************/

NLM_EXTERN Boolean LIBCALL
objPBTAsnLoad PROTO((void));


/**************************************************
*
*    PBTInteractionSet
*
**************************************************/
typedef struct struct_PBT_Interaction_set {
   ValNodePtr   date;
   CharPtr   desc;
   struct struct_PBT_Interactor PNTR   interactors;
   struct struct_PBT_Interaction PNTR   interactions;
} PBTInteractionSet, PNTR PBTInteractionSetPtr;


NLM_EXTERN PBTInteractionSetPtr LIBCALL PBTInteractionSetFree PROTO ((PBTInteractionSetPtr ));
NLM_EXTERN PBTInteractionSetPtr LIBCALL PBTInteractionSetNew PROTO (( void ));
NLM_EXTERN PBTInteractionSetPtr LIBCALL PBTInteractionSetAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL PBTInteractionSetAsnWrite PROTO (( PBTInteractionSetPtr , AsnIoPtr, AsnTypePtr));



/**************************************************
*
*    PBTInteractor
*
**************************************************/
typedef struct struct_PBT_Interactor {
   struct struct_PBT_Interactor PNTR next;
   Int4   uid;
   struct struct_PBT_Dbref PNTR   source;
   Uint2   type;
   /* following #defines are for enumerated type, not used by object loaders */
#define PBT_Interactor_type_protein 0
#define PBT_Interactor_type_dna 1

   Int4   taxid;
   struct struct_PBT_Name PNTR   names;
   struct struct_PBT_Dbref PNTR   xrefs;
} PBTInteractor, PNTR PBTInteractorPtr;


NLM_EXTERN PBTInteractorPtr LIBCALL PBTInteractorFree PROTO ((PBTInteractorPtr ));
NLM_EXTERN PBTInteractorPtr LIBCALL PBTInteractorNew PROTO (( void ));
NLM_EXTERN PBTInteractorPtr LIBCALL PBTInteractorAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL PBTInteractorAsnWrite PROTO (( PBTInteractorPtr , AsnIoPtr, AsnTypePtr));



/**************************************************
*
*    PBTInteraction
*
**************************************************/
typedef struct struct_PBT_Interaction {
   struct struct_PBT_Interaction PNTR next;
   Int4   uid_a;
   Int4   uid_b;
   struct struct_PBT_Int_Evidence PNTR   evidence;
} PBTInteraction, PNTR PBTInteractionPtr;


NLM_EXTERN PBTInteractionPtr LIBCALL PBTInteractionFree PROTO ((PBTInteractionPtr ));
NLM_EXTERN PBTInteractionPtr LIBCALL PBTInteractionNew PROTO (( void ));
NLM_EXTERN PBTInteractionPtr LIBCALL PBTInteractionAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL PBTInteractionAsnWrite PROTO (( PBTInteractionPtr , AsnIoPtr, AsnTypePtr));



/**************************************************
*
*    PBTIntEvidence
*
**************************************************/
typedef struct struct_PBT_Int_Evidence {
   struct struct_PBT_Int_Evidence PNTR next;
   struct struct_PBT_Dbref PNTR   doc;
   struct struct_PBT_Name_occur PNTR   names_a;
   struct struct_PBT_Name_occur PNTR   names_b;
   struct struct_PBT_Score PNTR   score;
} PBTIntEvidence, PNTR PBTIntEvidencePtr;


NLM_EXTERN PBTIntEvidencePtr LIBCALL PBTIntEvidenceFree PROTO ((PBTIntEvidencePtr ));
NLM_EXTERN PBTIntEvidencePtr LIBCALL PBTIntEvidenceNew PROTO (( void ));
NLM_EXTERN PBTIntEvidencePtr LIBCALL PBTIntEvidenceAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL PBTIntEvidenceAsnWrite PROTO (( PBTIntEvidencePtr , AsnIoPtr, AsnTypePtr));



/**************************************************
*
*    PBTScore
*
**************************************************/
typedef struct struct_PBT_Score {
   CharPtr   hypoth;
   CharPtr   method;
   FloatHi   score;
} PBTScore, PNTR PBTScorePtr;


NLM_EXTERN PBTScorePtr LIBCALL PBTScoreFree PROTO ((PBTScorePtr ));
NLM_EXTERN PBTScorePtr LIBCALL PBTScoreNew PROTO (( void ));
NLM_EXTERN PBTScorePtr LIBCALL PBTScoreAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL PBTScoreAsnWrite PROTO (( PBTScorePtr , AsnIoPtr, AsnTypePtr));



/**************************************************
*
*    PBTNameOccur
*
**************************************************/
typedef struct struct_PBT_Name_occur {
   struct struct_PBT_Name_occur PNTR next;
   CharPtr   name;
   struct struct_PBT_Score PNTR   score;
} PBTNameOccur, PNTR PBTNameOccurPtr;


NLM_EXTERN PBTNameOccurPtr LIBCALL PBTNameOccurFree PROTO ((PBTNameOccurPtr ));
NLM_EXTERN PBTNameOccurPtr LIBCALL PBTNameOccurNew PROTO (( void ));
NLM_EXTERN PBTNameOccurPtr LIBCALL PBTNameOccurAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL PBTNameOccurAsnWrite PROTO (( PBTNameOccurPtr , AsnIoPtr, AsnTypePtr));



/**************************************************
*
*    PBTName
*
**************************************************/
typedef struct struct_PBT_Name {
   struct struct_PBT_Name PNTR next;
   Int4   name_id;
   Int4   objid;
   CharPtr   name;
   CharPtr   name_type;
   struct struct_PBT_Dbref PNTR   source;
   Int4   rule_id;
   Uint1   deprecated;
   ValNodePtr   date_dep;
   ValNodePtr   searched;
   Int4   namered;
   Int4   results;
} PBTName, PNTR PBTNamePtr;


NLM_EXTERN PBTNamePtr LIBCALL PBTNameFree PROTO ((PBTNamePtr ));
NLM_EXTERN PBTNamePtr LIBCALL PBTNameNew PROTO (( void ));
NLM_EXTERN PBTNamePtr LIBCALL PBTNameAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL PBTNameAsnWrite PROTO (( PBTNamePtr , AsnIoPtr, AsnTypePtr));



/**************************************************
*
*    PBTDbref
*
**************************************************/
typedef struct struct_PBT_Dbref {
   struct struct_PBT_Dbref PNTR next;
   CharPtr   dbname;
   CharPtr   acc;
   Int4   id;
   CharPtr   field;
   struct struct_PBT_Score PNTR   score;
} PBTDbref, PNTR PBTDbrefPtr;


NLM_EXTERN PBTDbrefPtr LIBCALL PBTDbrefFree PROTO ((PBTDbrefPtr ));
NLM_EXTERN PBTDbrefPtr LIBCALL PBTDbrefNew PROTO (( void ));
NLM_EXTERN PBTDbrefPtr LIBCALL PBTDbrefAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL PBTDbrefAsnWrite PROTO (( PBTDbrefPtr , AsnIoPtr, AsnTypePtr));

#ifdef __cplusplus
/* { */ }
#endif

#endif /* _objPBT_ */

#undef NLM_EXTERN
#ifdef NLM_EXPORT
#define NLM_EXTERN NLM_EXPORT
#else
#define NLM_EXTERN
#endif

