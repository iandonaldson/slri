#define struct_Bioseq bioseq 
#define NLM_GENERATED_CODE_PROTO 
#ifndef _objslrilibstruc_ 
#define _objslrilibstruc_ 

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
*    Generated objects for Module SLRILIB-struc
*    Generated using ASNCODE Revision: 6.14 at Feb 11, 2003  2:33 PM
*
**************************************************/

NLM_EXTERN Boolean LIBCALL
objslrilibstrucAsnLoad PROTO((void));


/**************************************************
*
*    SLRISeqId
*
**************************************************/
typedef ValNode SLRISeqId;
typedef ValNodePtr SLRISeqIdPtr;
#define SLRISeqIdNew() ValNodeNew(NULL) 

#ifdef NLM_GENERATED_CODE_PROTO

NLM_EXTERN SLRISeqIdPtr LIBCALL SLRISeqIdFree PROTO ((SLRISeqIdPtr ));
NLM_EXTERN SLRISeqIdPtr LIBCALL SLRISeqIdNew PROTO (( void ));
NLM_EXTERN SLRISeqIdPtr LIBCALL SLRISeqIdAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL SLRISeqIdAsnWrite PROTO (( SLRISeqIdPtr , AsnIoPtr, AsnTypePtr));

#endif /* NLM_GENERATED_CODE_PROTO */



/**************************************************
*
*    SLRIValNode
*
**************************************************/
typedef ValNode SLRIValNode;
typedef ValNodePtr SLRIValNodePtr;
#define SLRIValNodeNew() ValNodeNew(NULL) 

#ifdef NLM_GENERATED_CODE_PROTO

NLM_EXTERN SLRIValNodePtr LIBCALL SLRIValNodeFree PROTO ((SLRIValNodePtr ));
NLM_EXTERN SLRIValNodePtr LIBCALL SLRIValNodeNew PROTO (( void ));
NLM_EXTERN SLRIValNodePtr LIBCALL SLRIValNodeAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL SLRIValNodeAsnWrite PROTO (( SLRIValNodePtr , AsnIoPtr, AsnTypePtr));

#endif /* NLM_GENERATED_CODE_PROTO */



/**************************************************
*
*    SLRIFasta
*
**************************************************/
typedef struct struct_SLRIFasta {
   CharPtr   defline;
   CharPtr   sequence;
} SLRIFasta, PNTR SLRIFastaPtr;


NLM_EXTERN SLRIFastaPtr LIBCALL SLRIFastaFree PROTO ((SLRIFastaPtr ));
NLM_EXTERN SLRIFastaPtr LIBCALL SLRIFastaNew PROTO (( void ));
NLM_EXTERN SLRIFastaPtr LIBCALL SLRIFastaAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL SLRIFastaAsnWrite PROTO (( SLRIFastaPtr , AsnIoPtr, AsnTypePtr));



/**************************************************
*
*    SLRIDomain
*
**************************************************/
typedef struct struct_SLRIDomain {
   Int4   gi;
   Int4   from;
   Int4   to;
} SLRIDomain, PNTR SLRIDomainPtr;


NLM_EXTERN SLRIDomainPtr LIBCALL SLRIDomainFree PROTO ((SLRIDomainPtr ));
NLM_EXTERN SLRIDomainPtr LIBCALL SLRIDomainNew PROTO (( void ));
NLM_EXTERN SLRIDomainPtr LIBCALL SLRIDomainAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL SLRIDomainAsnWrite PROTO (( SLRIDomainPtr , AsnIoPtr, AsnTypePtr));

typedef ValNodePtr SLRINodePtr;
typedef ValNode SLRINode;
#define SLRINode_gi 1
#define SLRINode_accession 2
#define SLRINode_bioseq 3
#define SLRINode_biostruc 4
#define SLRINode_fasta 5
#define SLRINode_seqid 6
#define SLRINode_domain 7
#define SLRINode_seqalign 8
#define SLRINode_bioseqset 9
#define SLRINode_seqentry 10


NLM_EXTERN SLRINodePtr LIBCALL SLRINodeFree PROTO ((SLRINodePtr ));
NLM_EXTERN SLRINodePtr LIBCALL SLRINodeAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL SLRINodeAsnWrite PROTO (( SLRINodePtr , AsnIoPtr, AsnTypePtr));



/**************************************************
*
*    DTree
*
**************************************************/
typedef struct struct_DNode DTree;
typedef struct struct_DNode PNTR DTreePtr;
#define DTreeNew() DNodeNew() 

#ifdef NLM_GENERATED_CODE_PROTO

NLM_EXTERN DTreePtr LIBCALL DTreeFree PROTO ((DTreePtr ));
NLM_EXTERN DTreePtr LIBCALL DTreeNew PROTO (( void ));
NLM_EXTERN DTreePtr LIBCALL DTreeAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL DTreeAsnWrite PROTO (( DTreePtr , AsnIoPtr, AsnTypePtr));

#endif /* NLM_GENERATED_CODE_PROTO */



/**************************************************
*
*    DNode
*
**************************************************/
typedef struct struct_DNode {
   struct struct_DNode PNTR next;
   ValNodePtr   key;
   ValNodePtr   data;
   Int4   num;
   Uint2   state;
   /* following #defines are for enumerated type, not used by object loaders */
#define DNode_state_none 0
#define DNode_state_vis 1

   struct struct_DNode PNTR   p;
   struct struct_DNode PNTR   c;
} DNode, PNTR DNodePtr;


NLM_EXTERN DNodePtr LIBCALL DNodeFree PROTO ((DNodePtr ));
NLM_EXTERN DNodePtr LIBCALL DNodeNew PROTO (( void ));
NLM_EXTERN DNodePtr LIBCALL DNodeAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL DNodeAsnWrite PROTO (( DNodePtr , AsnIoPtr, AsnTypePtr));

typedef ValNodePtr DKeyPtr;
typedef ValNode DKey;
#define DKey_id 1
#define DKey_str 2
#define DKey_real 3


NLM_EXTERN DKeyPtr LIBCALL DKeyFree PROTO ((DKeyPtr ));
NLM_EXTERN DKeyPtr LIBCALL DKeyAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL DKeyAsnWrite PROTO (( DKeyPtr , AsnIoPtr, AsnTypePtr));

typedef ValNodePtr DDataPtr;
typedef ValNode DData;
#define DData_id 1
#define DData_str 2
#define DData_real 3
#define DData_bool 4
#define DData_os 5
#define DData_strs 6
#define DData_ids 7
#define DData_reals 8
#define DData_oss 9
#define DData_gen_memfree 10
#define DData_data_list 11
#define DData_user_def 12


NLM_EXTERN DDataPtr LIBCALL DDataFree PROTO ((DDataPtr ));
NLM_EXTERN DDataPtr LIBCALL DDataAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL DDataAsnWrite PROTO (( DDataPtr , AsnIoPtr, AsnTypePtr));

#ifdef __cplusplus
/* { */ }
#endif

#endif /* _objslrilibstruc_ */

#undef NLM_EXTERN
#ifdef NLM_EXPORT
#define NLM_EXTERN NLM_EXPORT
#else
#define NLM_EXTERN
#endif

