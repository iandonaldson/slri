#ifndef _objslritax_ 
#define _objslritax_ 

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
*    Generated objects for Module SLRI-TAXDB
*    Generated using ASNCODE Revision: 6.15 at Apr 11, 2005  5:01 PM
*
**************************************************/

NLM_EXTERN Boolean LIBCALL
objslritaxAsnLoad PROTO((void));


/**************************************************
*
*    SLRITaxon
*
**************************************************/
typedef struct struct_SLRI_taxon {
   Int4   taxId;
   Int4   parent_taxId;
   ValNodePtr   children_taxId;
   struct struct_SLRI_taxon_name PNTR   names;
   struct struct_SLRI_taxon_rank PNTR   rank;
   CharPtr   embl_code;
   Int4   division;
   Uint1   inherited_div;
   Int4   gencode;
   Uint1   inherited_gencode;
   Int4   mito_gencode;
   Uint1   inherited_mito_gencode;
   Uint1   genbank_hidden;
   Uint1   hidden_subtree_root;
   CharPtr   comments;
} SLRITaxon, PNTR SLRITaxonPtr;


NLM_EXTERN SLRITaxonPtr LIBCALL SLRITaxonFree PROTO ((SLRITaxonPtr ));
NLM_EXTERN SLRITaxonPtr LIBCALL SLRITaxonNew PROTO (( void ));
NLM_EXTERN SLRITaxonPtr LIBCALL SLRITaxonAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL SLRITaxonAsnWrite PROTO (( SLRITaxonPtr , AsnIoPtr, AsnTypePtr));



/**************************************************
*
*    SLRITaxonDiv
*
**************************************************/
typedef struct struct_SLRI_taxon_div {
   Int4   div_id;
   CharPtr   div_code;
   CharPtr   div_name;
   CharPtr   comments;
} SLRITaxonDiv, PNTR SLRITaxonDivPtr;


NLM_EXTERN SLRITaxonDivPtr LIBCALL SLRITaxonDivFree PROTO ((SLRITaxonDivPtr ));
NLM_EXTERN SLRITaxonDivPtr LIBCALL SLRITaxonDivNew PROTO (( void ));
NLM_EXTERN SLRITaxonDivPtr LIBCALL SLRITaxonDivAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL SLRITaxonDivAsnWrite PROTO (( SLRITaxonDivPtr , AsnIoPtr, AsnTypePtr));



/**************************************************
*
*    SLRITaxonGencode
*
**************************************************/
typedef struct struct_SLRI_taxon_gencode {
   Int4   gencode_id;
   CharPtr   abbrev;
   CharPtr   name;
   CharPtr   trans_table;
   CharPtr   start_codons;
} SLRITaxonGencode, PNTR SLRITaxonGencodePtr;


NLM_EXTERN SLRITaxonGencodePtr LIBCALL SLRITaxonGencodeFree PROTO ((SLRITaxonGencodePtr ));
NLM_EXTERN SLRITaxonGencodePtr LIBCALL SLRITaxonGencodeNew PROTO (( void ));
NLM_EXTERN SLRITaxonGencodePtr LIBCALL SLRITaxonGencodeAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL SLRITaxonGencodeAsnWrite PROTO (( SLRITaxonGencodePtr , AsnIoPtr, AsnTypePtr));



/**************************************************
*
*    SLRITaxonName
*
**************************************************/
typedef struct struct_SLRI_taxon_name {
   struct struct_SLRI_taxon_name PNTR next;
   CharPtr   name;
   CharPtr   unique_name;
   Uint2   name_class;
   /* following #defines are for enumerated type, not used by object loaders */
#define SLRI_taxon_name_name_class_none 0
#define SLRI_taxon_name_name_class_acronym 1
#define SLRI_taxon_name_name_class_anamorph 2
#define SLRI_taxon_name_name_class_blast_name 3
#define SLRI_taxon_name_name_class_common_name 4
#define SLRI_taxon_name_name_class_equivalent_name 5
#define SLRI_taxon_name_name_class_includes 6
#define SLRI_taxon_name_name_class_in_part 7
#define SLRI_taxon_name_name_class_misnomer 8
#define SLRI_taxon_name_name_class_misspelling 9
#define SLRI_taxon_name_name_class_preferred_acronym 10
#define SLRI_taxon_name_name_class_preferred_common_name 11
#define SLRI_taxon_name_name_class_scientific_name 12
#define SLRI_taxon_name_name_class_synonym 13
#define SLRI_taxon_name_name_class_teleomorph 14
#define SLRI_taxon_name_name_class_other 255

   CharPtr   other_class;
} SLRITaxonName, PNTR SLRITaxonNamePtr;


NLM_EXTERN SLRITaxonNamePtr LIBCALL SLRITaxonNameFree PROTO ((SLRITaxonNamePtr ));
NLM_EXTERN SLRITaxonNamePtr LIBCALL SLRITaxonNameNew PROTO (( void ));
NLM_EXTERN SLRITaxonNamePtr LIBCALL SLRITaxonNameAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL SLRITaxonNameAsnWrite PROTO (( SLRITaxonNamePtr , AsnIoPtr, AsnTypePtr));



/**************************************************
*
*    SLRITaxonRank
*
**************************************************/
typedef struct struct_SLRI_taxon_rank {
   Uint2   rank;
   /* following #defines are for enumerated type, not used by object loaders */
#define SLRI_taxon_rank_rank_none 0
#define SLRI_taxon_rank_rank_kingdom 1
#define SLRI_taxon_rank_rank_phylum 2
#define SLRI_taxon_rank_rank_class__ 3
#define SLRI_taxon_rank_rank_cohort 4
#define SLRI_taxon_rank_rank_order 5
#define SLRI_taxon_rank_rank_family 6
#define SLRI_taxon_rank_rank_tribe 7
#define SLRI_taxon_rank_rank_genus 8
#define SLRI_taxon_rank_rank_section 9
#define SLRI_taxon_rank_rank_series 10
#define SLRI_taxon_rank_rank_species 11
#define SLRI_taxon_rank_rank_varietas 12
#define SLRI_taxon_rank_rank_forma 13
#define SLRI_taxon_rank_rank_other 255

   CharPtr   other_rank;
   Uint2   premod;
   /* following #defines are for enumerated type, not used by object loaders */
#define SLRI_taxon_rank_premod_none 0
#define SLRI_taxon_rank_premod_super 1
#define SLRI_taxon_rank_premod_sub 2
#define SLRI_taxon_rank_premod_infra 3
#define SLRI_taxon_rank_premod_parv 4
#define SLRI_taxon_rank_premod_other 255

   CharPtr   other_premod;
   Uint2   postmod;
   /* following #defines are for enumerated type, not used by object loaders */
#define SLRI_taxon_rank_postmod_none 0
#define SLRI_taxon_rank_postmod_subgroup 1
#define SLRI_taxon_rank_postmod_group 2
#define SLRI_taxon_rank_postmod_other 255

   CharPtr   other_postmod;
} SLRITaxonRank, PNTR SLRITaxonRankPtr;


NLM_EXTERN SLRITaxonRankPtr LIBCALL SLRITaxonRankFree PROTO ((SLRITaxonRankPtr ));
NLM_EXTERN SLRITaxonRankPtr LIBCALL SLRITaxonRankNew PROTO (( void ));
NLM_EXTERN SLRITaxonRankPtr LIBCALL SLRITaxonRankAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
NLM_EXTERN Boolean LIBCALL SLRITaxonRankAsnWrite PROTO (( SLRITaxonRankPtr , AsnIoPtr, AsnTypePtr));

#ifdef __cplusplus
/* { */ }
#endif

#endif /* _objslritax_ */

#undef NLM_EXTERN
#ifdef NLM_EXPORT
#define NLM_EXTERN NLM_EXPORT
#else
#define NLM_EXTERN
#endif

