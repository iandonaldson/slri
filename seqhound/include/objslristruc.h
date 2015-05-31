#define struct_Bioseq bioseq
#define NLM_GENERATED_CODE_PROTO
#ifndef _objslristruc_ 
#define _objslristruc_ 

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
*    Generated objects for Module INTREZ-databases
*    Generated using ASNCODE Revision: 6.15 at Apr 29, 2005  3:05 PM
*
**************************************************/

NLM_EXTERN Boolean LIBCALL
objslristrucAsnLoad PROTO((void));


/**************************************************
*
*    StRpsdb
*
**************************************************/
typedef struct struct_StRpsNode StRpsdb;
typedef struct struct_StRpsNode PNTR StRpsdbPtr;
#define StRpsdbNew() StRpsNodeNew() 

#ifdef NLM_GENERATED_CODE_PROTO

NLM_EXTERN StRpsdbPtr LIBCALL StRpsdbFree PROTO ((StRpsdbPtr ));
NLM_EXTERN StRpsdbPtr LIBCALL StRpsdbNew PROTO (( void ));
NLM_EXTERN StRpsdbPtr LIBCALL StRpsdbAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL StRpsdbAsnWrite PROTO (( StRpsdbPtr , AsnIoPtr, AsnTypePtr));

#endif /* NLM_GENERATED_CODE_PROTO */



/**************************************************
*
*    SHProteinReportSetDescription
*
**************************************************/
typedef struct struct_SHProteinReportSetDescription {
   CharPtr   title;
   Int4   total_reports;
   Int4   query;
   struct struct_SHProteinReport PNTR   query_report;
   ValNodePtr   redundant_set;
   struct struct_SHProteinReportSet PNTR   redun_reports;
   struct struct_FLink_set PNTR   neighbour_set;
   struct struct_SHProteinReport PNTR   best_report;
   struct struct_SHProteinReportSet PNTR   neighbour_reports;
   FloatHi   cutoff;
   Uint1   acc;
   Uint1   defline;
   Uint1   dna;
   Uint1   taxid;
   Uint1   omim;
   Uint1   muid;
   Uint1   pmid;
   Uint1   mmdb_ids;
   Uint1   go;
   Uint1   llink;
   Uint1   redund;
   Uint1   neighbours;
} SHProteinReportSetDescription, PNTR SHProteinReportSetDescriptionPtr;


NLM_EXTERN SHProteinReportSetDescriptionPtr LIBCALL SHProteinReportSetDescriptionFree PROTO ((SHProteinReportSetDescriptionPtr ));
NLM_EXTERN SHProteinReportSetDescriptionPtr LIBCALL SHProteinReportSetDescriptionNew PROTO (( void ));
NLM_EXTERN SHProteinReportSetDescriptionPtr LIBCALL SHProteinReportSetDescriptionAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL SHProteinReportSetDescriptionAsnWrite PROTO (( SHProteinReportSetDescriptionPtr , AsnIoPtr, AsnTypePtr));



/**************************************************
*
*    SHProteinReportSet
*
**************************************************/
typedef struct struct_SHProteinReportSet {
   Int4   number;
   struct struct_SHProteinReport PNTR   reports;
} SHProteinReportSet, PNTR SHProteinReportSetPtr;


NLM_EXTERN SHProteinReportSetPtr LIBCALL SHProteinReportSetFree PROTO ((SHProteinReportSetPtr ));
NLM_EXTERN SHProteinReportSetPtr LIBCALL SHProteinReportSetNew PROTO (( void ));
NLM_EXTERN SHProteinReportSetPtr LIBCALL SHProteinReportSetAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL SHProteinReportSetAsnWrite PROTO (( SHProteinReportSetPtr , AsnIoPtr, AsnTypePtr));



/**************************************************
*
*    SHProteinReport
*
**************************************************/
typedef struct struct_SHProteinReport {
   struct struct_SHProteinReport PNTR next;
   Int4   gi;
   CharPtr   acc;
   CharPtr   defline;
   Int4   dna;
   ValNodePtr   muids;
   ValNodePtr   pmids;
   Int4   mmdbid;
   Int4   taxid;
   struct struct_SHGoReport PNTR   go;
   ValNodePtr   omim;
   struct struct_SHLocusLinkReport PNTR   llink;
} SHProteinReport, PNTR SHProteinReportPtr;


NLM_EXTERN SHProteinReportPtr LIBCALL SHProteinReportFree PROTO ((SHProteinReportPtr ));
NLM_EXTERN SHProteinReportPtr LIBCALL SHProteinReportNew PROTO (( void ));
NLM_EXTERN SHProteinReportPtr LIBCALL SHProteinReportAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL SHProteinReportAsnWrite PROTO (( SHProteinReportPtr , AsnIoPtr, AsnTypePtr));



/**************************************************
*
*    StAsndbNode
*
**************************************************/
typedef struct struct_StAsndbNode {
   struct struct_StAsndbNode PNTR next;
   Int4   gi;
   struct struct_Bioseq PNTR   asn1;
   CharPtr   division;
   CharPtr   release;
   CharPtr   type;
} StAsndbNode, PNTR StAsndbNodePtr;


NLM_EXTERN StAsndbNodePtr LIBCALL StAsndbNodeFree PROTO ((StAsndbNodePtr ));
NLM_EXTERN StAsndbNodePtr LIBCALL StAsndbNodeNew PROTO (( void ));
NLM_EXTERN StAsndbNodePtr LIBCALL StAsndbNodeAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL StAsndbNodeAsnWrite PROTO (( StAsndbNodePtr , AsnIoPtr, AsnTypePtr));



/**************************************************
*
*    StAsndb
*
**************************************************/
typedef struct struct_StAsndbNode StAsndb;
typedef struct struct_StAsndbNode PNTR StAsndbPtr;
#define StAsndbNew() StAsndbNodeNew() 

#ifdef NLM_GENERATED_CODE_PROTO

NLM_EXTERN StAsndbPtr LIBCALL StAsndbFree PROTO ((StAsndbPtr ));
NLM_EXTERN StAsndbPtr LIBCALL StAsndbNew PROTO (( void ));
NLM_EXTERN StAsndbPtr LIBCALL StAsndbAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL StAsndbAsnWrite PROTO (( StAsndbPtr , AsnIoPtr, AsnTypePtr));

#endif /* NLM_GENERATED_CODE_PROTO */



/**************************************************
*
*    StPartiNode
*
**************************************************/
typedef struct struct_StPartiNode {
   struct struct_StPartiNode PNTR next;
   Int4   gi;
   CharPtr   partition;
} StPartiNode, PNTR StPartiNodePtr;


NLM_EXTERN StPartiNodePtr LIBCALL StPartiNodeFree PROTO ((StPartiNodePtr ));
NLM_EXTERN StPartiNodePtr LIBCALL StPartiNodeNew PROTO (( void ));
NLM_EXTERN StPartiNodePtr LIBCALL StPartiNodeAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL StPartiNodeAsnWrite PROTO (( StPartiNodePtr , AsnIoPtr, AsnTypePtr));



/**************************************************
*
*    StParti
*
**************************************************/
typedef struct struct_StPartiNode StParti;
typedef struct struct_StPartiNode PNTR StPartiPtr;
#define StPartiNew() StPartiNodeNew() 

#ifdef NLM_GENERATED_CODE_PROTO

NLM_EXTERN StPartiPtr LIBCALL StPartiFree PROTO ((StPartiPtr ));
NLM_EXTERN StPartiPtr LIBCALL StPartiNew PROTO (( void ));
NLM_EXTERN StPartiPtr LIBCALL StPartiAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL StPartiAsnWrite PROTO (( StPartiPtr , AsnIoPtr, AsnTypePtr));

#endif /* NLM_GENERATED_CODE_PROTO */



/**************************************************
*
*    StNucprotNode
*
**************************************************/
typedef struct struct_StNucprotNode {
   struct struct_StNucprotNode PNTR next;
   Int4   gin;
   Int4   gia;
} StNucprotNode, PNTR StNucprotNodePtr;


NLM_EXTERN StNucprotNodePtr LIBCALL StNucprotNodeFree PROTO ((StNucprotNodePtr ));
NLM_EXTERN StNucprotNodePtr LIBCALL StNucprotNodeNew PROTO (( void ));
NLM_EXTERN StNucprotNodePtr LIBCALL StNucprotNodeAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL StNucprotNodeAsnWrite PROTO (( StNucprotNodePtr , AsnIoPtr, AsnTypePtr));



/**************************************************
*
*    StNucprot
*
**************************************************/
typedef struct struct_StNucprotNode StNucprot;
typedef struct struct_StNucprotNode PNTR StNucprotPtr;
#define StNucprotNew() StNucprotNodeNew() 

#ifdef NLM_GENERATED_CODE_PROTO

NLM_EXTERN StNucprotPtr LIBCALL StNucprotFree PROTO ((StNucprotPtr ));
NLM_EXTERN StNucprotPtr LIBCALL StNucprotNew PROTO (( void ));
NLM_EXTERN StNucprotPtr LIBCALL StNucprotAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL StNucprotAsnWrite PROTO (( StNucprotPtr , AsnIoPtr, AsnTypePtr));

#endif /* NLM_GENERATED_CODE_PROTO */



/**************************************************
*
*    StAccdbNode
*
**************************************************/
typedef struct struct_StAccdbNode {
   struct struct_StAccdbNode PNTR next;
   Int4   gi;
   CharPtr   db;
   CharPtr   name;
   CharPtr   access;
   CharPtr   chain;
   CharPtr   release;
   Int4   version;
   CharPtr   title;
   CharPtr   namelow;
} StAccdbNode, PNTR StAccdbNodePtr;


NLM_EXTERN StAccdbNodePtr LIBCALL StAccdbNodeFree PROTO ((StAccdbNodePtr ));
NLM_EXTERN StAccdbNodePtr LIBCALL StAccdbNodeNew PROTO (( void ));
NLM_EXTERN StAccdbNodePtr LIBCALL StAccdbNodeAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL StAccdbNodeAsnWrite PROTO (( StAccdbNodePtr , AsnIoPtr, AsnTypePtr));



/**************************************************
*
*    StAccdb
*
**************************************************/
typedef struct struct_StAccdbNode StAccdb;
typedef struct struct_StAccdbNode PNTR StAccdbPtr;
#define StAccdbNew() StAccdbNodeNew() 

#ifdef NLM_GENERATED_CODE_PROTO

NLM_EXTERN StAccdbPtr LIBCALL StAccdbFree PROTO ((StAccdbPtr ));
NLM_EXTERN StAccdbPtr LIBCALL StAccdbNew PROTO (( void ));
NLM_EXTERN StAccdbPtr LIBCALL StAccdbAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL StAccdbAsnWrite PROTO (( StAccdbPtr , AsnIoPtr, AsnTypePtr));

#endif /* NLM_GENERATED_CODE_PROTO */



/**************************************************
*
*    StPubseqNode
*
**************************************************/
typedef struct struct_StPubseqNode {
   struct struct_StPubseqNode PNTR next;
   Int4   gi;
   Int4   muid;
   Int4   pmid;
} StPubseqNode, PNTR StPubseqNodePtr;


NLM_EXTERN StPubseqNodePtr LIBCALL StPubseqNodeFree PROTO ((StPubseqNodePtr ));
NLM_EXTERN StPubseqNodePtr LIBCALL StPubseqNodeNew PROTO (( void ));
NLM_EXTERN StPubseqNodePtr LIBCALL StPubseqNodeAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL StPubseqNodeAsnWrite PROTO (( StPubseqNodePtr , AsnIoPtr, AsnTypePtr));



/**************************************************
*
*    StPubseq
*
**************************************************/
typedef struct struct_StPubseqNode StPubseq;
typedef struct struct_StPubseqNode PNTR StPubseqPtr;
#define StPubseqNew() StPubseqNodeNew() 

#ifdef NLM_GENERATED_CODE_PROTO

NLM_EXTERN StPubseqPtr LIBCALL StPubseqFree PROTO ((StPubseqPtr ));
NLM_EXTERN StPubseqPtr LIBCALL StPubseqNew PROTO (( void ));
NLM_EXTERN StPubseqPtr LIBCALL StPubseqAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL StPubseqAsnWrite PROTO (( StPubseqPtr , AsnIoPtr, AsnTypePtr));

#endif /* NLM_GENERATED_CODE_PROTO */



/**************************************************
*
*    StTaxgiNode
*
**************************************************/
typedef struct struct_StTaxgiNode {
   struct struct_StTaxgiNode PNTR next;
   Int4   gi;
   Int4   taxid;
   Int4   kloodge;
   CharPtr   type;
} StTaxgiNode, PNTR StTaxgiNodePtr;


NLM_EXTERN StTaxgiNodePtr LIBCALL StTaxgiNodeFree PROTO ((StTaxgiNodePtr ));
NLM_EXTERN StTaxgiNodePtr LIBCALL StTaxgiNodeNew PROTO (( void ));
NLM_EXTERN StTaxgiNodePtr LIBCALL StTaxgiNodeAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL StTaxgiNodeAsnWrite PROTO (( StTaxgiNodePtr , AsnIoPtr, AsnTypePtr));



/**************************************************
*
*    StTaxgi
*
**************************************************/
typedef struct struct_StTaxgiNode StTaxgi;
typedef struct struct_StTaxgiNode PNTR StTaxgiPtr;
#define StTaxgiNew() StTaxgiNodeNew() 

#ifdef NLM_GENERATED_CODE_PROTO

NLM_EXTERN StTaxgiPtr LIBCALL StTaxgiFree PROTO ((StTaxgiPtr ));
NLM_EXTERN StTaxgiPtr LIBCALL StTaxgiNew PROTO (( void ));
NLM_EXTERN StTaxgiPtr LIBCALL StTaxgiAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL StTaxgiAsnWrite PROTO (( StTaxgiPtr , AsnIoPtr, AsnTypePtr));

#endif /* NLM_GENERATED_CODE_PROTO */



/**************************************************
*
*    StChromNode
*
**************************************************/
typedef struct struct_StChromNode {
   struct struct_StChromNode PNTR next;
   Int4   kloodge;
   Int4   taxid;
   Int4   chromid;
   Int4   chromfl;
   CharPtr   access;
   CharPtr   name;
   Int4   projectid;
   CharPtr   chromnum;
} StChromNode, PNTR StChromNodePtr;


NLM_EXTERN StChromNodePtr LIBCALL StChromNodeFree PROTO ((StChromNodePtr ));
NLM_EXTERN StChromNodePtr LIBCALL StChromNodeNew PROTO (( void ));
NLM_EXTERN StChromNodePtr LIBCALL StChromNodeAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL StChromNodeAsnWrite PROTO (( StChromNodePtr , AsnIoPtr, AsnTypePtr));



/**************************************************
*
*    StChrom
*
**************************************************/
typedef struct struct_StChromNode StChrom;
typedef struct struct_StChromNode PNTR StChromPtr;
#define StChromNew() StChromNodeNew() 

#ifdef NLM_GENERATED_CODE_PROTO

NLM_EXTERN StChromPtr LIBCALL StChromFree PROTO ((StChromPtr ));
NLM_EXTERN StChromPtr LIBCALL StChromNew PROTO (( void ));
NLM_EXTERN StChromPtr LIBCALL StChromAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL StChromAsnWrite PROTO (( StChromPtr , AsnIoPtr, AsnTypePtr));

#endif /* NLM_GENERATED_CODE_PROTO */



/**************************************************
*
*    StSendbNode
*
**************************************************/
typedef struct struct_StSendbNode {
   struct struct_StSendbNode PNTR next;
   Int4   seid;
   ValNodePtr   asn1;
} StSendbNode, PNTR StSendbNodePtr;


NLM_EXTERN StSendbNodePtr LIBCALL StSendbNodeFree PROTO ((StSendbNodePtr ));
NLM_EXTERN StSendbNodePtr LIBCALL StSendbNodeNew PROTO (( void ));
NLM_EXTERN StSendbNodePtr LIBCALL StSendbNodeAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL StSendbNodeAsnWrite PROTO (( StSendbNodePtr , AsnIoPtr, AsnTypePtr));



/**************************************************
*
*    StSendb
*
**************************************************/
typedef struct struct_StSendbNode StSendb;
typedef struct struct_StSendbNode PNTR StSendbPtr;
#define StSendbNew() StSendbNodeNew() 

#ifdef NLM_GENERATED_CODE_PROTO

NLM_EXTERN StSendbPtr LIBCALL StSendbFree PROTO ((StSendbPtr ));
NLM_EXTERN StSendbPtr LIBCALL StSendbNew PROTO (( void ));
NLM_EXTERN StSendbPtr LIBCALL StSendbAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL StSendbAsnWrite PROTO (( StSendbPtr , AsnIoPtr, AsnTypePtr));

#endif /* NLM_GENERATED_CODE_PROTO */



/**************************************************
*
*    StSengiNode
*
**************************************************/
typedef struct struct_StSengiNode {
   struct struct_StSengiNode PNTR next;
   Int4   seid;
   Int4   gi;
   CharPtr   division;
} StSengiNode, PNTR StSengiNodePtr;


NLM_EXTERN StSengiNodePtr LIBCALL StSengiNodeFree PROTO ((StSengiNodePtr ));
NLM_EXTERN StSengiNodePtr LIBCALL StSengiNodeNew PROTO (( void ));
NLM_EXTERN StSengiNodePtr LIBCALL StSengiNodeAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL StSengiNodeAsnWrite PROTO (( StSengiNodePtr , AsnIoPtr, AsnTypePtr));



/**************************************************
*
*    StSengi
*
**************************************************/
typedef struct struct_StSengiNode StSengi;
typedef struct struct_StSengiNode PNTR StSengiPtr;
#define StSengiNew() StSengiNodeNew() 

#ifdef NLM_GENERATED_CODE_PROTO

NLM_EXTERN StSengiPtr LIBCALL StSengiFree PROTO ((StSengiPtr ));
NLM_EXTERN StSengiPtr LIBCALL StSengiNew PROTO (( void ));
NLM_EXTERN StSengiPtr LIBCALL StSengiAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL StSengiAsnWrite PROTO (( StSengiPtr , AsnIoPtr, AsnTypePtr));

#endif /* NLM_GENERATED_CODE_PROTO */



/**************************************************
*
*    StMmdbNode
*
**************************************************/
typedef struct struct_StMmdbNode {
   struct struct_StMmdbNode PNTR next;
   Int4   mmdbid;
   CharPtr   pdbid;
   struct struct_Biostruc PNTR   asn1;
   Int4   bwhat;
   Int4   models;
   Int4   molecules;
   Int4   size;
   Int4   bzsize;
} StMmdbNode, PNTR StMmdbNodePtr;


NLM_EXTERN StMmdbNodePtr LIBCALL StMmdbNodeFree PROTO ((StMmdbNodePtr ));
NLM_EXTERN StMmdbNodePtr LIBCALL StMmdbNodeNew PROTO (( void ));
NLM_EXTERN StMmdbNodePtr LIBCALL StMmdbNodeAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL StMmdbNodeAsnWrite PROTO (( StMmdbNodePtr , AsnIoPtr, AsnTypePtr));



/**************************************************
*
*    StMmdb
*
**************************************************/
typedef struct struct_StMmdbNode StMmdb;
typedef struct struct_StMmdbNode PNTR StMmdbPtr;
#define StMmdbNew() StMmdbNodeNew() 

#ifdef NLM_GENERATED_CODE_PROTO

NLM_EXTERN StMmdbPtr LIBCALL StMmdbFree PROTO ((StMmdbPtr ));
NLM_EXTERN StMmdbPtr LIBCALL StMmdbNew PROTO (( void ));
NLM_EXTERN StMmdbPtr LIBCALL StMmdbAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL StMmdbAsnWrite PROTO (( StMmdbPtr , AsnIoPtr, AsnTypePtr));

#endif /* NLM_GENERATED_CODE_PROTO */



/**************************************************
*
*    StMmgiNode
*
**************************************************/
typedef struct struct_StMmgiNode {
   struct struct_StMmgiNode PNTR next;
   Int4   mmdbid;
   Int4   gi;
} StMmgiNode, PNTR StMmgiNodePtr;


NLM_EXTERN StMmgiNodePtr LIBCALL StMmgiNodeFree PROTO ((StMmgiNodePtr ));
NLM_EXTERN StMmgiNodePtr LIBCALL StMmgiNodeNew PROTO (( void ));
NLM_EXTERN StMmgiNodePtr LIBCALL StMmgiNodeAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL StMmgiNodeAsnWrite PROTO (( StMmgiNodePtr , AsnIoPtr, AsnTypePtr));



/**************************************************
*
*    StMmgi
*
**************************************************/
typedef struct struct_StMmgiNode StMmgi;
typedef struct struct_StMmgiNode PNTR StMmgiPtr;
#define StMmgiNew() StMmgiNodeNew() 

#ifdef NLM_GENERATED_CODE_PROTO

NLM_EXTERN StMmgiPtr LIBCALL StMmgiFree PROTO ((StMmgiPtr ));
NLM_EXTERN StMmgiPtr LIBCALL StMmgiNew PROTO (( void ));
NLM_EXTERN StMmgiPtr LIBCALL StMmgiAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL StMmgiAsnWrite PROTO (( StMmgiPtr , AsnIoPtr, AsnTypePtr));

#endif /* NLM_GENERATED_CODE_PROTO */



/**************************************************
*
*    StRedundNode
*
**************************************************/
typedef struct struct_StRedundNode {
   struct struct_StRedundNode PNTR next;
   Int4   gi;
   Int4   ordinal;
   Int4   group;
} StRedundNode, PNTR StRedundNodePtr;


NLM_EXTERN StRedundNodePtr LIBCALL StRedundNodeFree PROTO ((StRedundNodePtr ));
NLM_EXTERN StRedundNodePtr LIBCALL StRedundNodeNew PROTO (( void ));
NLM_EXTERN StRedundNodePtr LIBCALL StRedundNodeAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL StRedundNodeAsnWrite PROTO (( StRedundNodePtr , AsnIoPtr, AsnTypePtr));



/**************************************************
*
*    StRedund
*
**************************************************/
typedef struct struct_StRedundNode StRedund;
typedef struct struct_StRedundNode PNTR StRedundPtr;
#define StRedundNew() StRedundNodeNew() 

#ifdef NLM_GENERATED_CODE_PROTO

NLM_EXTERN StRedundPtr LIBCALL StRedundFree PROTO ((StRedundPtr ));
NLM_EXTERN StRedundPtr LIBCALL StRedundNew PROTO (( void ));
NLM_EXTERN StRedundPtr LIBCALL StRedundAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL StRedundAsnWrite PROTO (( StRedundPtr , AsnIoPtr, AsnTypePtr));

#endif /* NLM_GENERATED_CODE_PROTO */



/**************************************************
*
*    StDomdbNode
*
**************************************************/
typedef struct struct_StDomdbNode {
   struct struct_StDomdbNode PNTR next;
   Int4   mmdbid;
   ValNodePtr   asn1;
   CharPtr   pdbid;
   CharPtr   chain;
   Int4   gi;
   Int4   domno;
   Int4   domall;
   Int4   domid;
   Int4   rep;
} StDomdbNode, PNTR StDomdbNodePtr;


NLM_EXTERN StDomdbNodePtr LIBCALL StDomdbNodeFree PROTO ((StDomdbNodePtr ));
NLM_EXTERN StDomdbNodePtr LIBCALL StDomdbNodeNew PROTO (( void ));
NLM_EXTERN StDomdbNodePtr LIBCALL StDomdbNodeAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL StDomdbNodeAsnWrite PROTO (( StDomdbNodePtr , AsnIoPtr, AsnTypePtr));



/**************************************************
*
*    StDomdb
*
**************************************************/
typedef struct struct_StDomdbNode StDomdb;
typedef struct struct_StDomdbNode PNTR StDomdbPtr;
#define StDomdbNew() StDomdbNodeNew() 

#ifdef NLM_GENERATED_CODE_PROTO

NLM_EXTERN StDomdbPtr LIBCALL StDomdbFree PROTO ((StDomdbPtr ));
NLM_EXTERN StDomdbPtr LIBCALL StDomdbNew PROTO (( void ));
NLM_EXTERN StDomdbPtr LIBCALL StDomdbAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL StDomdbAsnWrite PROTO (( StDomdbPtr , AsnIoPtr, AsnTypePtr));

#endif /* NLM_GENERATED_CODE_PROTO */



/**************************************************
*
*    StCddbNode
*
**************************************************/
typedef struct struct_StCddbNode {
   struct struct_StCddbNode PNTR next;
   Int4   gi;
   struct struct_SLRIFasta PNTR   asn1;
} StCddbNode, PNTR StCddbNodePtr;


NLM_EXTERN StCddbNodePtr LIBCALL StCddbNodeFree PROTO ((StCddbNodePtr ));
NLM_EXTERN StCddbNodePtr LIBCALL StCddbNodeNew PROTO (( void ));
NLM_EXTERN StCddbNodePtr LIBCALL StCddbNodeAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL StCddbNodeAsnWrite PROTO (( StCddbNodePtr , AsnIoPtr, AsnTypePtr));



/**************************************************
*
*    StCddb
*
**************************************************/
typedef struct struct_StCddbNode StCddb;
typedef struct struct_StCddbNode PNTR StCddbPtr;
#define StCddbNew() StCddbNodeNew() 

#ifdef NLM_GENERATED_CODE_PROTO

NLM_EXTERN StCddbPtr LIBCALL StCddbFree PROTO ((StCddbPtr ));
NLM_EXTERN StCddbPtr LIBCALL StCddbNew PROTO (( void ));
NLM_EXTERN StCddbPtr LIBCALL StCddbAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL StCddbAsnWrite PROTO (( StCddbPtr , AsnIoPtr, AsnTypePtr));

#endif /* NLM_GENERATED_CODE_PROTO */



/**************************************************
*
*    StRpsNode
*
**************************************************/
typedef struct struct_StRpsNode {
   struct struct_StRpsNode PNTR next;
   Int4   gi;
   CharPtr   dom_id;
   Int4   cdd_id;
   Int4   from;
   Int4   len;
   Int4   score;
   FloatHi   evalue;
   FloatHi   bitscore;
   Int4   n_missing;
   Int4   c_missing;
   Int4   num_of_dom;
} StRpsNode, PNTR StRpsNodePtr;


NLM_EXTERN StRpsNodePtr LIBCALL StRpsNodeFree PROTO ((StRpsNodePtr ));
NLM_EXTERN StRpsNodePtr LIBCALL StRpsNodeNew PROTO (( void ));
NLM_EXTERN StRpsNodePtr LIBCALL StRpsNodeAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL StRpsNodeAsnWrite PROTO (( StRpsNodePtr , AsnIoPtr, AsnTypePtr));



/**************************************************
*
*    StDomNameNode
*
**************************************************/
typedef struct struct_StDomNameNode {
   struct struct_StDomNameNode PNTR next;
   Int4   pssmid;
   CharPtr   accession;
   CharPtr   name;
   CharPtr   description;
   Int4   length;
   CharPtr   pdb_id;
} StDomNameNode, PNTR StDomNameNodePtr;


NLM_EXTERN StDomNameNodePtr LIBCALL StDomNameNodeFree PROTO ((StDomNameNodePtr ));
NLM_EXTERN StDomNameNodePtr LIBCALL StDomNameNodeNew PROTO (( void ));
NLM_EXTERN StDomNameNodePtr LIBCALL StDomNameNodeAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL StDomNameNodeAsnWrite PROTO (( StDomNameNodePtr , AsnIoPtr, AsnTypePtr));



/**************************************************
*
*    StDomNamedb
*
**************************************************/
typedef struct struct_StDomNameNode StDomNamedb;
typedef struct struct_StDomNameNode PNTR StDomNamedbPtr;
#define StDomNamedbNew() StDomNameNodeNew() 

#ifdef NLM_GENERATED_CODE_PROTO

NLM_EXTERN StDomNamedbPtr LIBCALL StDomNamedbFree PROTO ((StDomNamedbPtr ));
NLM_EXTERN StDomNamedbPtr LIBCALL StDomNamedbNew PROTO (( void ));
NLM_EXTERN StDomNamedbPtr LIBCALL StDomNamedbAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL StDomNamedbAsnWrite PROTO (( StDomNamedbPtr , AsnIoPtr, AsnTypePtr));

#endif /* NLM_GENERATED_CODE_PROTO */



/**************************************************
*
*    StHistdbNode
*
**************************************************/
typedef struct struct_StHistdbNode {
   struct struct_StHistdbNode PNTR next;
   Int4   gi;
   Int4   oldgi;
   CharPtr   access;
   Int4   version;
   CharPtr   date;
   Int4   action;
   CharPtr   filename;
} StHistdbNode, PNTR StHistdbNodePtr;


NLM_EXTERN StHistdbNodePtr LIBCALL StHistdbNodeFree PROTO ((StHistdbNodePtr ));
NLM_EXTERN StHistdbNodePtr LIBCALL StHistdbNodeNew PROTO (( void ));
NLM_EXTERN StHistdbNodePtr LIBCALL StHistdbNodeAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL StHistdbNodeAsnWrite PROTO (( StHistdbNodePtr , AsnIoPtr, AsnTypePtr));



/**************************************************
*
*    StHistdb
*
**************************************************/
typedef struct struct_StHistdbNode StHistdb;
typedef struct struct_StHistdbNode PNTR StHistdbPtr;
#define StHistdbNew() StHistdbNodeNew() 

#ifdef NLM_GENERATED_CODE_PROTO

NLM_EXTERN StHistdbPtr LIBCALL StHistdbFree PROTO ((StHistdbPtr ));
NLM_EXTERN StHistdbPtr LIBCALL StHistdbNew PROTO (( void ));
NLM_EXTERN StHistdbPtr LIBCALL StHistdbAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL StHistdbAsnWrite PROTO (( StHistdbPtr , AsnIoPtr, AsnTypePtr));

#endif /* NLM_GENERATED_CODE_PROTO */



/**************************************************
*
*    StGOdbNode
*
**************************************************/
typedef struct struct_StGOdbNode {
   struct struct_StGOdbNode PNTR next;
   Int4   llid;
   Int4   goid;
   Int4   pmid;
   CharPtr   eviCode;
} StGOdbNode, PNTR StGOdbNodePtr;


NLM_EXTERN StGOdbNodePtr LIBCALL StGOdbNodeFree PROTO ((StGOdbNodePtr ));
NLM_EXTERN StGOdbNodePtr LIBCALL StGOdbNodeNew PROTO (( void ));
NLM_EXTERN StGOdbNodePtr LIBCALL StGOdbNodeAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL StGOdbNodeAsnWrite PROTO (( StGOdbNodePtr , AsnIoPtr, AsnTypePtr));



/**************************************************
*
*    StGOdb
*
**************************************************/
typedef struct struct_StGOdbNode StGOdb;
typedef struct struct_StGOdbNode PNTR StGOdbPtr;
#define StGOdbNew() StGOdbNodeNew() 

#ifdef NLM_GENERATED_CODE_PROTO

NLM_EXTERN StGOdbPtr LIBCALL StGOdbFree PROTO ((StGOdbPtr ));
NLM_EXTERN StGOdbPtr LIBCALL StGOdbNew PROTO (( void ));
NLM_EXTERN StGOdbPtr LIBCALL StGOdbAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL StGOdbAsnWrite PROTO (( StGOdbPtr , AsnIoPtr, AsnTypePtr));

#endif /* NLM_GENERATED_CODE_PROTO */



/**************************************************
*
*    StOMIMdbNode
*
**************************************************/
typedef struct struct_StOMIMdbNode {
   struct struct_StOMIMdbNode PNTR next;
   Int4   llid;
   Int4   omimid;
} StOMIMdbNode, PNTR StOMIMdbNodePtr;


NLM_EXTERN StOMIMdbNodePtr LIBCALL StOMIMdbNodeFree PROTO ((StOMIMdbNodePtr ));
NLM_EXTERN StOMIMdbNodePtr LIBCALL StOMIMdbNodeNew PROTO (( void ));
NLM_EXTERN StOMIMdbNodePtr LIBCALL StOMIMdbNodeAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL StOMIMdbNodeAsnWrite PROTO (( StOMIMdbNodePtr , AsnIoPtr, AsnTypePtr));



/**************************************************
*
*    StOMIMdb
*
**************************************************/
typedef struct struct_StOMIMdbNode StOMIMdb;
typedef struct struct_StOMIMdbNode PNTR StOMIMdbPtr;
#define StOMIMdbNew() StOMIMdbNodeNew() 

#ifdef NLM_GENERATED_CODE_PROTO

NLM_EXTERN StOMIMdbPtr LIBCALL StOMIMdbFree PROTO ((StOMIMdbPtr ));
NLM_EXTERN StOMIMdbPtr LIBCALL StOMIMdbNew PROTO (( void ));
NLM_EXTERN StOMIMdbPtr LIBCALL StOMIMdbAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL StOMIMdbAsnWrite PROTO (( StOMIMdbPtr , AsnIoPtr, AsnTypePtr));

#endif /* NLM_GENERATED_CODE_PROTO */



/**************************************************
*
*    StCDDdbNode
*
**************************************************/
typedef struct struct_StCDDdbNode {
   struct struct_StCDDdbNode PNTR next;
   Int4   llid;
   CharPtr   cddid;
   FloatHi   eValue;
} StCDDdbNode, PNTR StCDDdbNodePtr;


NLM_EXTERN StCDDdbNodePtr LIBCALL StCDDdbNodeFree PROTO ((StCDDdbNodePtr ));
NLM_EXTERN StCDDdbNodePtr LIBCALL StCDDdbNodeNew PROTO (( void ));
NLM_EXTERN StCDDdbNodePtr LIBCALL StCDDdbNodeAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL StCDDdbNodeAsnWrite PROTO (( StCDDdbNodePtr , AsnIoPtr, AsnTypePtr));



/**************************************************
*
*    StCDDdb
*
**************************************************/
typedef struct struct_StCDDdbNode StCDDdb;
typedef struct struct_StCDDdbNode PNTR StCDDdbPtr;
#define StCDDdbNew() StCDDdbNodeNew() 

#ifdef NLM_GENERATED_CODE_PROTO

NLM_EXTERN StCDDdbPtr LIBCALL StCDDdbFree PROTO ((StCDDdbPtr ));
NLM_EXTERN StCDDdbPtr LIBCALL StCDDdbNew PROTO (( void ));
NLM_EXTERN StCDDdbPtr LIBCALL StCDDdbAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL StCDDdbAsnWrite PROTO (( StCDDdbPtr , AsnIoPtr, AsnTypePtr));

#endif /* NLM_GENERATED_CODE_PROTO */



/**************************************************
*
*    StLLdbNode
*
**************************************************/
typedef struct struct_StLLdbNode {
   struct struct_StLLdbNode PNTR next;
   Int4   llid;
   Int4   npid;
   CharPtr   map;
} StLLdbNode, PNTR StLLdbNodePtr;


NLM_EXTERN StLLdbNodePtr LIBCALL StLLdbNodeFree PROTO ((StLLdbNodePtr ));
NLM_EXTERN StLLdbNodePtr LIBCALL StLLdbNodeNew PROTO (( void ));
NLM_EXTERN StLLdbNodePtr LIBCALL StLLdbNodeAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL StLLdbNodeAsnWrite PROTO (( StLLdbNodePtr , AsnIoPtr, AsnTypePtr));



/**************************************************
*
*    StLLdb
*
**************************************************/
typedef struct struct_StLLdbNode StLLdb;
typedef struct struct_StLLdbNode PNTR StLLdbPtr;
#define StLLdbNew() StLLdbNodeNew() 

#ifdef NLM_GENERATED_CODE_PROTO

NLM_EXTERN StLLdbPtr LIBCALL StLLdbFree PROTO ((StLLdbPtr ));
NLM_EXTERN StLLdbPtr LIBCALL StLLdbNew PROTO (( void ));
NLM_EXTERN StLLdbPtr LIBCALL StLLdbAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL StLLdbAsnWrite PROTO (( StLLdbPtr , AsnIoPtr, AsnTypePtr));

#endif /* NLM_GENERATED_CODE_PROTO */



/**************************************************
*
*    SHGoReport
*
**************************************************/
typedef struct struct_SHGoReport {
   struct struct_SHGoReport PNTR next;
   Int4   num_mol_fun;
   Int4   num_bio_pro;
   Int4   num_cell_com;
   struct struct_SHGoAnnotNode PNTR   mol_fun;
   struct struct_SHGoAnnotNode PNTR   bio_pro;
   struct struct_SHGoAnnotNode PNTR   cell_com;
} SHGoReport, PNTR SHGoReportPtr;


NLM_EXTERN SHGoReportPtr LIBCALL SHGoReportFree PROTO ((SHGoReportPtr ));
NLM_EXTERN SHGoReportPtr LIBCALL SHGoReportNew PROTO (( void ));
NLM_EXTERN SHGoReportPtr LIBCALL SHGoReportAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL SHGoReportAsnWrite PROTO (( SHGoReportPtr , AsnIoPtr, AsnTypePtr));



/**************************************************
*
*    SHLocusLinkReport
*
**************************************************/
typedef struct struct_SHLocusLinkReport {
   struct struct_SHLocusLinkReport PNTR next;
   Int4   llid;
   CharPtr   locus;
   struct struct_SHLocusProduct PNTR   products;
} SHLocusLinkReport, PNTR SHLocusLinkReportPtr;


NLM_EXTERN SHLocusLinkReportPtr LIBCALL SHLocusLinkReportFree PROTO ((SHLocusLinkReportPtr ));
NLM_EXTERN SHLocusLinkReportPtr LIBCALL SHLocusLinkReportNew PROTO (( void ));
NLM_EXTERN SHLocusLinkReportPtr LIBCALL SHLocusLinkReportAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL SHLocusLinkReportAsnWrite PROTO (( SHLocusLinkReportPtr , AsnIoPtr, AsnTypePtr));



/**************************************************
*
*    SHNeighbour
*
**************************************************/
typedef struct struct_SHNeighbour {
   Int4   gi;
   FloatHi   evalue;
} SHNeighbour, PNTR SHNeighbourPtr;


NLM_EXTERN SHNeighbourPtr LIBCALL SHNeighbourFree PROTO ((SHNeighbourPtr ));
NLM_EXTERN SHNeighbourPtr LIBCALL SHNeighbourNew PROTO (( void ));
NLM_EXTERN SHNeighbourPtr LIBCALL SHNeighbourAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL SHNeighbourAsnWrite PROTO (( SHNeighbourPtr , AsnIoPtr, AsnTypePtr));



/**************************************************
*
*    SHGoAnnot
*
**************************************************/
typedef struct struct_SHGoAnnotNode SHGoAnnot;
typedef struct struct_SHGoAnnotNode PNTR SHGoAnnotPtr;
#define SHGoAnnotNew() SHGoAnnotNodeNew() 

#ifdef NLM_GENERATED_CODE_PROTO

NLM_EXTERN SHGoAnnotPtr LIBCALL SHGoAnnotFree PROTO ((SHGoAnnotPtr ));
NLM_EXTERN SHGoAnnotPtr LIBCALL SHGoAnnotNew PROTO (( void ));
NLM_EXTERN SHGoAnnotPtr LIBCALL SHGoAnnotAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL SHGoAnnotAsnWrite PROTO (( SHGoAnnotPtr , AsnIoPtr, AsnTypePtr));

#endif /* NLM_GENERATED_CODE_PROTO */



/**************************************************
*
*    SHGoAnnotNode
*
**************************************************/
typedef struct struct_SHGoAnnotNode {
   struct struct_SHGoAnnotNode PNTR next;
   Int4   goid;
   CharPtr   ontology;
   CharPtr   goname;
   ValNodePtr   evidence_code;
   ValNodePtr   pmid;
} SHGoAnnotNode, PNTR SHGoAnnotNodePtr;


NLM_EXTERN SHGoAnnotNodePtr LIBCALL SHGoAnnotNodeFree PROTO ((SHGoAnnotNodePtr ));
NLM_EXTERN SHGoAnnotNodePtr LIBCALL SHGoAnnotNodeNew PROTO (( void ));
NLM_EXTERN SHGoAnnotNodePtr LIBCALL SHGoAnnotNodeAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL SHGoAnnotNodeAsnWrite PROTO (( SHGoAnnotNodePtr , AsnIoPtr, AsnTypePtr));



/**************************************************
*
*    SHLocusProduct
*
**************************************************/
typedef struct struct_SHLocusProduct {
   struct struct_SHLocusProduct PNTR next;
   Int4   gi;
   CharPtr   defline;
} SHLocusProduct, PNTR SHLocusProductPtr;


NLM_EXTERN SHLocusProductPtr LIBCALL SHLocusProductFree PROTO ((SHLocusProductPtr ));
NLM_EXTERN SHLocusProductPtr LIBCALL SHLocusProductNew PROTO (( void ));
NLM_EXTERN SHLocusProductPtr LIBCALL SHLocusProductAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL SHLocusProductAsnWrite PROTO (( SHLocusProductPtr , AsnIoPtr, AsnTypePtr));



/**************************************************
*
*    StDbXrefNode
*
**************************************************/
typedef struct struct_StDbXrefNode {
   struct struct_StDbXrefNode PNTR next;
   Int4   id;
   CharPtr   sourcedb;
   Int4   sourceid;
   CharPtr   recordid;
   Int4   parentid;
   Int4   link;
   CharPtr   field;
   Int4   cv;
} StDbXrefNode, PNTR StDbXrefNodePtr;


NLM_EXTERN StDbXrefNodePtr LIBCALL StDbXrefNodeFree PROTO ((StDbXrefNodePtr ));
NLM_EXTERN StDbXrefNodePtr LIBCALL StDbXrefNodeNew PROTO (( void ));
NLM_EXTERN StDbXrefNodePtr LIBCALL StDbXrefNodeAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL StDbXrefNodeAsnWrite PROTO (( StDbXrefNodePtr , AsnIoPtr, AsnTypePtr));



/**************************************************
*
*    StDbXref
*
**************************************************/
typedef struct struct_StDbXrefNode StDbXref;
typedef struct struct_StDbXrefNode PNTR StDbXrefPtr;
#define StDbXrefNew() StDbXrefNodeNew() 

#ifdef NLM_GENERATED_CODE_PROTO

NLM_EXTERN StDbXrefPtr LIBCALL StDbXrefFree PROTO ((StDbXrefPtr ));
NLM_EXTERN StDbXrefPtr LIBCALL StDbXrefNew PROTO (( void ));
NLM_EXTERN StDbXrefPtr LIBCALL StDbXrefAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL StDbXrefAsnWrite PROTO (( StDbXrefPtr , AsnIoPtr, AsnTypePtr));

#endif /* NLM_GENERATED_CODE_PROTO */



/**************************************************
*
*    SHDBXrefNode
*
**************************************************/
typedef struct struct_SHDBXrefNode {
   struct struct_SHDBXrefNode PNTR next;
   CharPtr   sourcedb;
   CharPtr   recordid;
   CharPtr   field;
   Int4   cv;
} SHDBXrefNode, PNTR SHDBXrefNodePtr;


NLM_EXTERN SHDBXrefNodePtr LIBCALL SHDBXrefNodeFree PROTO ((SHDBXrefNodePtr ));
NLM_EXTERN SHDBXrefNodePtr LIBCALL SHDBXrefNodeNew PROTO (( void ));
NLM_EXTERN SHDBXrefNodePtr LIBCALL SHDBXrefNodeAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL SHDBXrefNodeAsnWrite PROTO (( SHDBXrefNodePtr , AsnIoPtr, AsnTypePtr));



/**************************************************
*
*    SHDBXref
*
**************************************************/
typedef struct struct_SHDBXrefNode SHDBXref;
typedef struct struct_SHDBXrefNode PNTR SHDBXrefPtr;
#define SHDBXrefNew() SHDBXrefNodeNew() 

#ifdef NLM_GENERATED_CODE_PROTO

NLM_EXTERN SHDBXrefPtr LIBCALL SHDBXrefFree PROTO ((SHDBXrefPtr ));
NLM_EXTERN SHDBXrefPtr LIBCALL SHDBXrefNew PROTO (( void ));
NLM_EXTERN SHDBXrefPtr LIBCALL SHDBXrefAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL SHDBXrefAsnWrite PROTO (( SHDBXrefPtr , AsnIoPtr, AsnTypePtr));

#endif /* NLM_GENERATED_CODE_PROTO */



/**************************************************
*
*    StBioentityNode
*
**************************************************/
typedef struct struct_StBioentityNode {
   struct struct_StBioentityNode PNTR next;
   Int4   id;
   Int4   bioentitytypeid;
   CharPtr   db;
   CharPtr   access;
   Int4   identifier;
   Int4   field;
} StBioentityNode, PNTR StBioentityNodePtr;


NLM_EXTERN StBioentityNodePtr LIBCALL StBioentityNodeFree PROTO ((StBioentityNodePtr ));
NLM_EXTERN StBioentityNodePtr LIBCALL StBioentityNodeNew PROTO (( void ));
NLM_EXTERN StBioentityNodePtr LIBCALL StBioentityNodeAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL StBioentityNodeAsnWrite PROTO (( StBioentityNodePtr , AsnIoPtr, AsnTypePtr));



/**************************************************
*
*    StBioentity
*
**************************************************/
typedef struct struct_StBioentityNode StBioentity;
typedef struct struct_StBioentityNode PNTR StBioentityPtr;
#define StBioentityNew() StBioentityNodeNew() 

#ifdef NLM_GENERATED_CODE_PROTO

NLM_EXTERN StBioentityPtr LIBCALL StBioentityFree PROTO ((StBioentityPtr ));
NLM_EXTERN StBioentityPtr LIBCALL StBioentityNew PROTO (( void ));
NLM_EXTERN StBioentityPtr LIBCALL StBioentityAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL StBioentityAsnWrite PROTO (( StBioentityPtr , AsnIoPtr, AsnTypePtr));

#endif /* NLM_GENERATED_CODE_PROTO */



/**************************************************
*
*    StBionameNode
*
**************************************************/
typedef struct struct_StBionameNode {
   struct struct_StBionameNode PNTR next;
   Int4   id;
   Int4   bioentityid;
   CharPtr   name;
   Int4   nametypeid;
   CharPtr   db;
   CharPtr   access;
   Int4   identifier;
   Int4   field;
   Int4   official;
   Int4   deprecated;
   CharPtr   date;
   Int4   ruleid;
   Int4   action;
   CharPtr   actiondate;
} StBionameNode, PNTR StBionameNodePtr;


NLM_EXTERN StBionameNodePtr LIBCALL StBionameNodeFree PROTO ((StBionameNodePtr ));
NLM_EXTERN StBionameNodePtr LIBCALL StBionameNodeNew PROTO (( void ));
NLM_EXTERN StBionameNodePtr LIBCALL StBionameNodeAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL StBionameNodeAsnWrite PROTO (( StBionameNodePtr , AsnIoPtr, AsnTypePtr));



/**************************************************
*
*    StBioname
*
**************************************************/
typedef struct struct_StBionameNode StBioname;
typedef struct struct_StBionameNode PNTR StBionamePtr;
#define StBionameNew() StBionameNodeNew() 

#ifdef NLM_GENERATED_CODE_PROTO

NLM_EXTERN StBionamePtr LIBCALL StBionameFree PROTO ((StBionamePtr ));
NLM_EXTERN StBionamePtr LIBCALL StBionameNew PROTO (( void ));
NLM_EXTERN StBionamePtr LIBCALL StBionameAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL StBionameAsnWrite PROTO (( StBionamePtr , AsnIoPtr, AsnTypePtr));

#endif /* NLM_GENERATED_CODE_PROTO */



/**************************************************
*
*    StSecondrefsNode
*
**************************************************/
typedef struct struct_StSecondrefsNode {
   struct struct_StSecondrefsNode PNTR next;
   Int4   id;
   Int4   bionameid;
   CharPtr   db;
   CharPtr   access;
   Int4   field;
} StSecondrefsNode, PNTR StSecondrefsNodePtr;


NLM_EXTERN StSecondrefsNodePtr LIBCALL StSecondrefsNodeFree PROTO ((StSecondrefsNodePtr ));
NLM_EXTERN StSecondrefsNodePtr LIBCALL StSecondrefsNodeNew PROTO (( void ));
NLM_EXTERN StSecondrefsNodePtr LIBCALL StSecondrefsNodeAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL StSecondrefsNodeAsnWrite PROTO (( StSecondrefsNodePtr , AsnIoPtr, AsnTypePtr));



/**************************************************
*
*    StSecondrefs
*
**************************************************/
typedef struct struct_StSecondrefsNode StSecondrefs;
typedef struct struct_StSecondrefsNode PNTR StSecondrefsPtr;
#define StSecondrefsNew() StSecondrefsNodeNew() 

#ifdef NLM_GENERATED_CODE_PROTO

NLM_EXTERN StSecondrefsPtr LIBCALL StSecondrefsFree PROTO ((StSecondrefsPtr ));
NLM_EXTERN StSecondrefsPtr LIBCALL StSecondrefsNew PROTO (( void ));
NLM_EXTERN StSecondrefsPtr LIBCALL StSecondrefsAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL StSecondrefsAsnWrite PROTO (( StSecondrefsPtr , AsnIoPtr, AsnTypePtr));

#endif /* NLM_GENERATED_CODE_PROTO */



/**************************************************
*
*    StGichromidNode
*
**************************************************/
typedef struct struct_StGichromidNode {
   struct struct_StGichromidNode PNTR next;
   Int4   id;
   Int4   gi;
   Int4   contiggi;
   Int4   chromid;
} StGichromidNode, PNTR StGichromidNodePtr;


NLM_EXTERN StGichromidNodePtr LIBCALL StGichromidNodeFree PROTO ((StGichromidNodePtr ));
NLM_EXTERN StGichromidNodePtr LIBCALL StGichromidNodeNew PROTO (( void ));
NLM_EXTERN StGichromidNodePtr LIBCALL StGichromidNodeAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL StGichromidNodeAsnWrite PROTO (( StGichromidNodePtr , AsnIoPtr, AsnTypePtr));



/**************************************************
*
*    StGichromid
*
**************************************************/
typedef struct struct_StGichromidNode StGichromid;
typedef struct struct_StGichromidNode PNTR StGichromidPtr;
#define StGichromidNew() StGichromidNodeNew() 

#ifdef NLM_GENERATED_CODE_PROTO

NLM_EXTERN StGichromidPtr LIBCALL StGichromidFree PROTO ((StGichromidPtr ));
NLM_EXTERN StGichromidPtr LIBCALL StGichromidNew PROTO (( void ));
NLM_EXTERN StGichromidPtr LIBCALL StGichromidAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL StGichromidAsnWrite PROTO (( StGichromidPtr , AsnIoPtr, AsnTypePtr));

#endif /* NLM_GENERATED_CODE_PROTO */



/**************************************************
*
*    StContigchromidNode
*
**************************************************/
typedef struct struct_StContigchromidNode {
   struct struct_StContigchromidNode PNTR next;
   Int4   id;
   Int4   contiggi;
   Int4   topgi;
   Int4   chromid;
   Int4   changed;
} StContigchromidNode, PNTR StContigchromidNodePtr;


NLM_EXTERN StContigchromidNodePtr LIBCALL StContigchromidNodeFree PROTO ((StContigchromidNodePtr ));
NLM_EXTERN StContigchromidNodePtr LIBCALL StContigchromidNodeNew PROTO (( void ));
NLM_EXTERN StContigchromidNodePtr LIBCALL StContigchromidNodeAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL StContigchromidNodeAsnWrite PROTO (( StContigchromidNodePtr , AsnIoPtr, AsnTypePtr));



/**************************************************
*
*    StContigchromid
*
**************************************************/
typedef struct struct_StContigchromidNode StContigchromid;
typedef struct struct_StContigchromidNode PNTR StContigchromidPtr;
#define StContigchromidNew() StContigchromidNodeNew() 

#ifdef NLM_GENERATED_CODE_PROTO

NLM_EXTERN StContigchromidPtr LIBCALL StContigchromidFree PROTO ((StContigchromidPtr ));
NLM_EXTERN StContigchromidPtr LIBCALL StContigchromidNew PROTO (( void ));
NLM_EXTERN StContigchromidPtr LIBCALL StContigchromidAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL StContigchromidAsnWrite PROTO (( StContigchromidPtr , AsnIoPtr, AsnTypePtr));

#endif /* NLM_GENERATED_CODE_PROTO */



/**************************************************
*
*    StGichromNode
*
**************************************************/
typedef struct struct_StGichromNode {
   struct struct_StGichromNode PNTR next;
   Int4   id;
   Int4   gi;
   CharPtr   chromnum;
} StGichromNode, PNTR StGichromNodePtr;


NLM_EXTERN StGichromNodePtr LIBCALL StGichromNodeFree PROTO ((StGichromNodePtr ));
NLM_EXTERN StGichromNodePtr LIBCALL StGichromNodeNew PROTO (( void ));
NLM_EXTERN StGichromNodePtr LIBCALL StGichromNodeAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL StGichromNodeAsnWrite PROTO (( StGichromNodePtr , AsnIoPtr, AsnTypePtr));



/**************************************************
*
*    StGichrom
*
**************************************************/
typedef struct struct_StGichromNode StGichrom;
typedef struct struct_StGichromNode PNTR StGichromPtr;
#define StGichromNew() StGichromNodeNew() 

#ifdef NLM_GENERATED_CODE_PROTO

NLM_EXTERN StGichromPtr LIBCALL StGichromFree PROTO ((StGichromPtr ));
NLM_EXTERN StGichromPtr LIBCALL StGichromNew PROTO (( void ));
NLM_EXTERN StGichromPtr LIBCALL StGichromAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL StGichromAsnWrite PROTO (( StGichromPtr , AsnIoPtr, AsnTypePtr));

#endif /* NLM_GENERATED_CODE_PROTO */



/**************************************************
*
*    StContigchromNode
*
**************************************************/
typedef struct struct_StContigchromNode {
   struct struct_StContigchromNode PNTR next;
   Int4   id;
   Int4   contiggi;
   CharPtr   chromnum;
} StContigchromNode, PNTR StContigchromNodePtr;


NLM_EXTERN StContigchromNodePtr LIBCALL StContigchromNodeFree PROTO ((StContigchromNodePtr ));
NLM_EXTERN StContigchromNodePtr LIBCALL StContigchromNodeNew PROTO (( void ));
NLM_EXTERN StContigchromNodePtr LIBCALL StContigchromNodeAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL StContigchromNodeAsnWrite PROTO (( StContigchromNodePtr , AsnIoPtr, AsnTypePtr));



/**************************************************
*
*    StContigchrom
*
**************************************************/
typedef struct struct_StContigchromNode StContigchrom;
typedef struct struct_StContigchromNode PNTR StContigchromPtr;
#define StContigchromNew() StContigchromNodeNew() 

#ifdef NLM_GENERATED_CODE_PROTO

NLM_EXTERN StContigchromPtr LIBCALL StContigchromFree PROTO ((StContigchromPtr ));
NLM_EXTERN StContigchromPtr LIBCALL StContigchromNew PROTO (( void ));
NLM_EXTERN StContigchromPtr LIBCALL StContigchromAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL StContigchromAsnWrite PROTO (( StContigchromPtr , AsnIoPtr, AsnTypePtr));

#endif /* NLM_GENERATED_CODE_PROTO */



/**************************************************
*
*    StSourcefileNode
*
**************************************************/
typedef struct struct_StSourcefileNode {
   struct struct_StSourcefileNode PNTR next;
   Int4   id;
   Int4   gi;
   CharPtr   filename;
   CharPtr   date;
} StSourcefileNode, PNTR StSourcefileNodePtr;


NLM_EXTERN StSourcefileNodePtr LIBCALL StSourcefileNodeFree PROTO ((StSourcefileNodePtr ));
NLM_EXTERN StSourcefileNodePtr LIBCALL StSourcefileNodeNew PROTO (( void ));
NLM_EXTERN StSourcefileNodePtr LIBCALL StSourcefileNodeAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL StSourcefileNodeAsnWrite PROTO (( StSourcefileNodePtr , AsnIoPtr, AsnTypePtr));



/**************************************************
*
*    StSourcefile
*
**************************************************/
typedef struct struct_StSourcefileNode StSourcefile;
typedef struct struct_StSourcefileNode PNTR StSourcefilePtr;
#define StSourcefileNew() StSourcefileNodeNew() 

#ifdef NLM_GENERATED_CODE_PROTO

NLM_EXTERN StSourcefilePtr LIBCALL StSourcefileFree PROTO ((StSourcefilePtr ));
NLM_EXTERN StSourcefilePtr LIBCALL StSourcefileNew PROTO (( void ));
NLM_EXTERN StSourcefilePtr LIBCALL StSourcefileAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL StSourcefileAsnWrite PROTO (( StSourcefilePtr , AsnIoPtr, AsnTypePtr));

#endif /* NLM_GENERATED_CODE_PROTO */



/**************************************************
*
*    StGeneObjectNode
*
**************************************************/
typedef struct struct_StGeneObjectNode {
   struct struct_StGeneObjectNode PNTR next;
   Int4   id;
   Int4   taxid;
   Int4   geneid;
   CharPtr   status;
} StGeneObjectNode, PNTR StGeneObjectNodePtr;


NLM_EXTERN StGeneObjectNodePtr LIBCALL StGeneObjectNodeFree PROTO ((StGeneObjectNodePtr ));
NLM_EXTERN StGeneObjectNodePtr LIBCALL StGeneObjectNodeNew PROTO (( void ));
NLM_EXTERN StGeneObjectNodePtr LIBCALL StGeneObjectNodeAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL StGeneObjectNodeAsnWrite PROTO (( StGeneObjectNodePtr , AsnIoPtr, AsnTypePtr));



/**************************************************
*
*    StGeneObject
*
**************************************************/
typedef struct struct_StGeneObjectNode StGeneObject;
typedef struct struct_StGeneObjectNode PNTR StGeneObjectPtr;
#define StGeneObjectNew() StGeneObjectNodeNew() 

#ifdef NLM_GENERATED_CODE_PROTO

NLM_EXTERN StGeneObjectPtr LIBCALL StGeneObjectFree PROTO ((StGeneObjectPtr ));
NLM_EXTERN StGeneObjectPtr LIBCALL StGeneObjectNew PROTO (( void ));
NLM_EXTERN StGeneObjectPtr LIBCALL StGeneObjectAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL StGeneObjectAsnWrite PROTO (( StGeneObjectPtr , AsnIoPtr, AsnTypePtr));

#endif /* NLM_GENERATED_CODE_PROTO */



/**************************************************
*
*    StGeneGiNode
*
**************************************************/
typedef struct struct_StGeneGiNode {
   struct struct_StGeneGiNode PNTR next;
   Int4   id;
   Int4   geneobjectid;
   Int4   gi;
   CharPtr   moleculetype;
} StGeneGiNode, PNTR StGeneGiNodePtr;


NLM_EXTERN StGeneGiNodePtr LIBCALL StGeneGiNodeFree PROTO ((StGeneGiNodePtr ));
NLM_EXTERN StGeneGiNodePtr LIBCALL StGeneGiNodeNew PROTO (( void ));
NLM_EXTERN StGeneGiNodePtr LIBCALL StGeneGiNodeAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL StGeneGiNodeAsnWrite PROTO (( StGeneGiNodePtr , AsnIoPtr, AsnTypePtr));



/**************************************************
*
*    StGeneGi
*
**************************************************/
typedef struct struct_StGeneGiNode StGeneGi;
typedef struct struct_StGeneGiNode PNTR StGeneGiPtr;
#define StGeneGiNew() StGeneGiNodeNew() 

#ifdef NLM_GENERATED_CODE_PROTO

NLM_EXTERN StGeneGiPtr LIBCALL StGeneGiFree PROTO ((StGeneGiPtr ));
NLM_EXTERN StGeneGiPtr LIBCALL StGeneGiNew PROTO (( void ));
NLM_EXTERN StGeneGiPtr LIBCALL StGeneGiAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL StGeneGiAsnWrite PROTO (( StGeneGiPtr , AsnIoPtr, AsnTypePtr));

#endif /* NLM_GENERATED_CODE_PROTO */



/**************************************************
*
*    StGeneDbxrefNode
*
**************************************************/
typedef struct struct_StGeneDbxrefNode {
   struct struct_StGeneDbxrefNode PNTR next;
   Int4   id;
   Int4   geneobjectid;
   CharPtr   dbname;
   CharPtr   dbaccess;
} StGeneDbxrefNode, PNTR StGeneDbxrefNodePtr;


NLM_EXTERN StGeneDbxrefNodePtr LIBCALL StGeneDbxrefNodeFree PROTO ((StGeneDbxrefNodePtr ));
NLM_EXTERN StGeneDbxrefNodePtr LIBCALL StGeneDbxrefNodeNew PROTO (( void ));
NLM_EXTERN StGeneDbxrefNodePtr LIBCALL StGeneDbxrefNodeAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL StGeneDbxrefNodeAsnWrite PROTO (( StGeneDbxrefNodePtr , AsnIoPtr, AsnTypePtr));



/**************************************************
*
*    StGeneDbxref
*
**************************************************/
typedef struct struct_StGeneDbxrefNode StGeneDbxref;
typedef struct struct_StGeneDbxrefNode PNTR StGeneDbxrefPtr;
#define StGeneDbxrefNew() StGeneDbxrefNodeNew() 

#ifdef NLM_GENERATED_CODE_PROTO

NLM_EXTERN StGeneDbxrefPtr LIBCALL StGeneDbxrefFree PROTO ((StGeneDbxrefPtr ));
NLM_EXTERN StGeneDbxrefPtr LIBCALL StGeneDbxrefNew PROTO (( void ));
NLM_EXTERN StGeneDbxrefPtr LIBCALL StGeneDbxrefAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL StGeneDbxrefAsnWrite PROTO (( StGeneDbxrefPtr , AsnIoPtr, AsnTypePtr));

#endif /* NLM_GENERATED_CODE_PROTO */

#ifdef __cplusplus
/* { */ }
#endif

#endif /* _objslristruc_ */

#undef NLM_EXTERN
#ifdef NLM_EXPORT
#define NLM_EXTERN NLM_EXPORT
#else
#define NLM_EXTERN
#endif

