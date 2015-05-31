/***********************************************************************
*
**
*        Automatic header module from ASNTOOL
*
************************************************************************/

#ifndef _ASNTOOL_
#include <asn.h>
#endif

static char * asnfilename = "slritax.h11";
static AsnValxNode avnx[43] = {
    {20,"none" ,0,0.0,&avnx[1] } ,
    {20,"acronym" ,1,0.0,&avnx[2] } ,
    {20,"anamorph" ,2,0.0,&avnx[3] } ,
    {20,"blast-name" ,3,0.0,&avnx[4] } ,
    {20,"common-name" ,4,0.0,&avnx[5] } ,
    {20,"equivalent-name" ,5,0.0,&avnx[6] } ,
    {20,"includes" ,6,0.0,&avnx[7] } ,
    {20,"in-part" ,7,0.0,&avnx[8] } ,
    {20,"misnomer" ,8,0.0,&avnx[9] } ,
    {20,"misspelling" ,9,0.0,&avnx[10] } ,
    {20,"preferred-acronym" ,10,0.0,&avnx[11] } ,
    {20,"preferred-common-name" ,11,0.0,&avnx[12] } ,
    {20,"scientific-name" ,12,0.0,&avnx[13] } ,
    {20,"synonym" ,13,0.0,&avnx[14] } ,
    {20,"teleomorph" ,14,0.0,&avnx[15] } ,
    {20,"other" ,255,0.0,NULL } ,
    {20,"none" ,0,0.0,&avnx[17] } ,
    {20,"kingdom" ,1,0.0,&avnx[18] } ,
    {20,"phylum" ,2,0.0,&avnx[19] } ,
    {20,"class" ,3,0.0,&avnx[20] } ,
    {20,"cohort" ,4,0.0,&avnx[21] } ,
    {20,"order" ,5,0.0,&avnx[22] } ,
    {20,"family" ,6,0.0,&avnx[23] } ,
    {20,"tribe" ,7,0.0,&avnx[24] } ,
    {20,"genus" ,8,0.0,&avnx[25] } ,
    {20,"section" ,9,0.0,&avnx[26] } ,
    {20,"series" ,10,0.0,&avnx[27] } ,
    {20,"species" ,11,0.0,&avnx[28] } ,
    {20,"varietas" ,12,0.0,&avnx[29] } ,
    {20,"forma" ,13,0.0,&avnx[30] } ,
    {20,"other" ,255,0.0,NULL } ,
    {20,"none" ,0,0.0,&avnx[32] } ,
    {20,"super" ,1,0.0,&avnx[33] } ,
    {20,"sub" ,2,0.0,&avnx[34] } ,
    {20,"infra" ,3,0.0,&avnx[35] } ,
    {20,"parv" ,4,0.0,&avnx[36] } ,
    {20,"other" ,255,0.0,NULL } ,
    {3,NULL,0,0.0,NULL } ,
    {20,"none" ,0,0.0,&avnx[39] } ,
    {20,"subgroup" ,1,0.0,&avnx[40] } ,
    {20,"group" ,2,0.0,&avnx[41] } ,
    {20,"other" ,255,0.0,NULL } ,
    {3,NULL,0,0.0,NULL } };

static AsnType atx[49] = {
  {401, "SLRI-taxon" ,1,0,0,0,0,1,0,0,NULL,&atx[16],&atx[1],0,&atx[27]} ,
  {0, "taxId" ,128,0,0,0,0,0,0,0,NULL,&atx[2],NULL,0,&atx[3]} ,
  {302, "INTEGER" ,0,2,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "parent-taxId" ,128,1,0,0,0,0,0,0,NULL,&atx[2],NULL,0,&atx[4]} ,
  {0, "children-taxId" ,128,2,0,1,0,0,0,0,NULL,&atx[6],&atx[5],0,&atx[7]} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[2],NULL,0,NULL} ,
  {312, "SEQUENCE OF" ,0,16,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "names" ,128,3,0,0,0,0,0,0,NULL,&atx[6],&atx[8],0,&atx[17]} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[9],NULL,0,NULL} ,
  {406, "SLRI-taxon-name" ,1,0,0,0,0,0,0,0,NULL,&atx[16],&atx[10],0,&atx[18]} ,
  {0, "name" ,128,0,0,0,0,0,0,0,NULL,&atx[11],NULL,0,&atx[12]} ,
  {323, "VisibleString" ,0,26,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "unique-name" ,128,1,0,1,0,0,0,0,NULL,&atx[11],NULL,0,&atx[13]} ,
  {0, "name-class" ,128,2,0,0,0,0,0,0,NULL,&atx[14],&avnx[0],0,&atx[15]} ,
  {310, "ENUMERATED" ,0,10,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "other-class" ,128,3,0,1,0,0,0,0,NULL,&atx[11],NULL,0,NULL} ,
  {311, "SEQUENCE" ,0,16,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "rank" ,128,4,0,0,0,0,0,0,NULL,&atx[18],NULL,0,&atx[25]} ,
  {407, "SLRI-taxon-rank" ,1,0,0,0,0,0,0,0,NULL,&atx[16],&atx[19],0,NULL} ,
  {0, "rank" ,128,0,0,0,0,0,0,0,NULL,&atx[14],&avnx[16],0,&atx[20]} ,
  {0, "other-rank" ,128,1,0,1,0,0,0,0,NULL,&atx[11],NULL,0,&atx[21]} ,
  {0, "premod" ,128,2,0,0,1,0,0,0,&avnx[37],&atx[14],&avnx[31],0,&atx[22]} ,
  {0, "other-premod" ,128,3,0,1,0,0,0,0,NULL,&atx[11],NULL,0,&atx[23]} ,
  {0, "postmod" ,128,4,0,0,1,0,0,0,&avnx[42],&atx[14],&avnx[38],0,&atx[24]} ,
  {0, "other-postmod" ,128,5,0,1,0,0,0,0,NULL,&atx[11],NULL,0,NULL} ,
  {0, "embl-code" ,128,5,0,1,0,0,0,0,NULL,&atx[11],NULL,0,&atx[26]} ,
  {0, "division" ,128,6,0,0,0,0,0,0,NULL,&atx[27],NULL,0,&atx[28]} ,
  {402, "SLRI-division-id" ,1,0,0,0,0,1,0,0,NULL,&atx[2],NULL,0,&atx[31]} ,
  {0, "inherited-div" ,128,7,0,0,0,0,0,0,NULL,&atx[29],NULL,0,&atx[30]} ,
  {301, "BOOLEAN" ,0,1,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "gencode" ,128,8,0,0,0,0,0,0,NULL,&atx[31],NULL,0,&atx[32]} ,
  {403, "SLRI-genetic-code-id" ,1,0,0,0,0,1,0,0,NULL,&atx[2],NULL,0,&atx[38]} ,
  {0, "inherited-gencode" ,128,9,0,0,0,0,0,0,NULL,&atx[29],NULL,0,&atx[33]} ,
  {0, "mito-gencode" ,128,10,0,0,0,0,0,0,NULL,&atx[31],NULL,0,&atx[34]} ,
  {0, "inherited-mito-gencode" ,128,11,0,0,0,0,0,0,NULL,&atx[29],NULL,0,&atx[35]} ,
  {0, "genbank-hidden" ,128,12,0,0,0,0,0,0,NULL,&atx[29],NULL,0,&atx[36]} ,
  {0, "hidden-subtree-root" ,128,13,0,0,0,0,0,0,NULL,&atx[29],NULL,0,&atx[37]} ,
  {0, "comments" ,128,14,0,1,0,0,0,0,NULL,&atx[11],NULL,0,NULL} ,
  {404, "SLRI-taxon-div" ,1,0,0,0,0,1,0,0,NULL,&atx[16],&atx[39],0,&atx[43]} ,
  {0, "div-id" ,128,0,0,0,0,0,0,0,NULL,&atx[2],NULL,0,&atx[40]} ,
  {0, "div-code" ,128,1,0,0,0,0,0,0,NULL,&atx[11],NULL,0,&atx[41]} ,
  {0, "div-name" ,128,2,0,0,0,0,0,0,NULL,&atx[11],NULL,0,&atx[42]} ,
  {0, "comments" ,128,3,0,1,0,0,0,0,NULL,&atx[11],NULL,0,NULL} ,
  {405, "SLRI-taxon-gencode" ,1,0,0,0,0,1,0,0,NULL,&atx[16],&atx[44],0,&atx[9]} ,
  {0, "gencode-id" ,128,0,0,0,0,0,0,0,NULL,&atx[2],NULL,0,&atx[45]} ,
  {0, "abbrev" ,128,1,0,1,0,0,0,0,NULL,&atx[11],NULL,0,&atx[46]} ,
  {0, "name" ,128,2,0,0,0,0,0,0,NULL,&atx[11],NULL,0,&atx[47]} ,
  {0, "trans-table" ,128,3,0,1,0,0,0,0,NULL,&atx[11],NULL,0,&atx[48]} ,
  {0, "start-codons" ,128,4,0,1,0,0,0,0,NULL,&atx[11],NULL,0,NULL} };

static AsnModule ampx[1] = {
  { "SLRI-TAXDB" , "slritax.h11",&atx[0],NULL,NULL,0,0} };

static AsnValxNodePtr avn = avnx;
static AsnTypePtr at = atx;
static AsnModulePtr amp = ampx;



/**************************************************
*
*    Defines for Module SLRI-TAXDB
*
**************************************************/

#define SLRI_TAXON &at[0]
#define SLRI_TAXON_taxId &at[1]
#define SLRI_TAXON_parent_taxId &at[3]
#define SLRI_TAXON_children_taxId &at[4]
#define SLRI_TAXON_children_taxId_E &at[5]
#define SLRI_TAXON_names &at[7]
#define SLRI_TAXON_names_E &at[8]
#define SLRI_TAXON_rank &at[17]
#define SLRI_TAXON_embl_code &at[25]
#define SLRI_TAXON_division &at[26]
#define SLRI_TAXON_inherited_div &at[28]
#define SLRI_TAXON_gencode &at[30]
#define SLRI_TAXON_inherited_gencode &at[32]
#define SLRI_TAXON_mito_gencode &at[33]
#define SLRI_TAXON_inherited_mito_gencode &at[34]
#define SLRI_TAXON_genbank_hidden &at[35]
#define SLRI_TAXON_hidden_subtree_root &at[36]
#define SLRI_TAXON_comments &at[37]

#define SLRI_DIVISION_ID &at[27]

#define SLRI_GENETIC_CODE_ID &at[31]

#define SLRI_TAXON_DIV &at[38]
#define SLRI_TAXON_DIV_div_id &at[39]
#define SLRI_TAXON_DIV_div_code &at[40]
#define SLRI_TAXON_DIV_div_name &at[41]
#define SLRI_TAXON_DIV_comments &at[42]

#define SLRI_TAXON_GENCODE &at[43]
#define SLRI_TAXON_GENCODE_gencode_id &at[44]
#define SLRI_TAXON_GENCODE_abbrev &at[45]
#define SLRI_TAXON_GENCODE_name &at[46]
#define SLRI_TAXON_GENCODE_trans_table &at[47]
#define SLRI_TAXON_GENCODE_start_codons &at[48]

#define SLRI_TAXON_NAME &at[9]
#define SLRI_TAXON_NAME_name &at[10]
#define SLRI_TAXON_NAME_unique_name &at[12]
#define SLRI_TAXON_NAME_name_class &at[13]
#define SLRI_TAXON_NAME_other_class &at[15]

#define SLRI_TAXON_RANK &at[18]
#define SLRI_TAXON_RANK_rank &at[19]
#define SLRI_TAXON_RANK_other_rank &at[20]
#define SLRI_TAXON_RANK_premod &at[21]
#define SLRI_TAXON_RANK_other_premod &at[22]
#define SLRI_TAXON_RANK_postmod &at[23]
#define SLRI_TAXON_RANK_other_postmod &at[24]
