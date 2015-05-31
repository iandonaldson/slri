/***********************************************************************
*
**
*        Automatic header module from ASNTOOL
*
************************************************************************/

#ifndef _ASNTOOL_
#include <asn.h>
#endif

static char * asnfilename = "prebind_transfer.h";
static AsnValxNode avnx[2] = {
    {20,"protein" ,0,0.0,&avnx[1] } ,
    {20,"dna" ,1,0.0,NULL } };

static AsnType atx[61] = {
  {401, "PBT-Interaction-set" ,1,0,0,0,0,1,0,0,NULL,&atx[22],&atx[1],0,&atx[7]} ,
  {0, "date" ,128,0,0,1,0,0,0,0,NULL,&atx[2],NULL,0,&atx[3]} ,
  {409, "Date" ,1,0,0,0,0,0,1,0,NULL,NULL,NULL,0,NULL} ,
  {0, "desc" ,128,1,0,1,0,0,0,0,NULL,&atx[4],NULL,0,&atx[5]} ,
  {323, "VisibleString" ,0,26,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "interactors" ,128,2,0,0,0,0,0,0,NULL,&atx[41],&atx[6],0,&atx[44]} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[7],NULL,0,NULL} ,
  {402, "PBT-Interactor" ,1,0,0,0,0,1,0,0,NULL,&atx[22],&atx[8],0,&atx[46]} ,
  {0, "uid" ,128,0,0,0,0,0,0,0,NULL,&atx[9],NULL,0,&atx[10]} ,
  {302, "INTEGER" ,0,2,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "source" ,128,1,0,0,0,0,0,0,NULL,&atx[11],NULL,0,&atx[23]} ,
  {408, "PBT-Dbref" ,1,0,0,0,0,1,0,0,NULL,&atx[22],&atx[12],0,&atx[2]} ,
  {0, "dbname" ,128,0,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[13]} ,
  {0, "acc" ,128,1,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[14]} ,
  {0, "id" ,128,2,0,0,0,0,0,0,NULL,&atx[9],NULL,0,&atx[15]} ,
  {0, "field" ,128,3,0,1,0,0,0,0,NULL,&atx[4],NULL,0,&atx[16]} ,
  {0, "score" ,128,4,0,1,0,0,0,0,NULL,&atx[17],NULL,0,NULL} ,
  {405, "PBT-Score" ,1,0,0,0,0,1,0,0,NULL,&atx[22],&atx[18],0,&atx[55]} ,
  {0, "hypoth" ,128,0,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[19]} ,
  {0, "method" ,128,1,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[20]} ,
  {0, "score" ,128,2,0,0,0,0,0,0,NULL,&atx[21],NULL,0,NULL} ,
  {309, "REAL" ,0,9,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {311, "SEQUENCE" ,0,16,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "type" ,128,2,0,0,0,0,0,0,NULL,&atx[24],&avnx[0],0,&atx[25]} ,
  {310, "ENUMERATED" ,0,10,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "taxid" ,128,3,0,1,0,0,0,0,NULL,&atx[9],NULL,0,&atx[26]} ,
  {0, "names" ,128,4,0,0,0,0,0,0,NULL,&atx[41],&atx[27],0,&atx[42]} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[28],NULL,0,NULL} ,
  {407, "PBT-Name" ,1,0,0,0,0,1,0,0,NULL,&atx[22],&atx[29],0,&atx[11]} ,
  {0, "name-id" ,128,0,0,0,0,0,0,0,NULL,&atx[9],NULL,0,&atx[30]} ,
  {0, "objid" ,128,1,0,0,0,0,0,0,NULL,&atx[9],NULL,0,&atx[31]} ,
  {0, "name" ,128,2,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[32]} ,
  {0, "name-type" ,128,3,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[33]} ,
  {0, "source" ,128,4,0,0,0,0,0,0,NULL,&atx[11],NULL,0,&atx[34]} ,
  {0, "rule-id" ,128,5,0,0,0,0,0,0,NULL,&atx[9],NULL,0,&atx[35]} ,
  {0, "deprecated" ,128,6,0,0,0,0,0,0,NULL,&atx[36],NULL,0,&atx[37]} ,
  {301, "BOOLEAN" ,0,1,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "date-dep" ,128,7,0,0,0,0,0,0,NULL,&atx[2],NULL,0,&atx[38]} ,
  {0, "searched" ,128,8,0,0,0,0,0,0,NULL,&atx[2],NULL,0,&atx[39]} ,
  {0, "namered" ,128,9,0,0,0,0,0,0,NULL,&atx[9],NULL,0,&atx[40]} ,
  {0, "results" ,128,10,0,0,0,0,0,0,NULL,&atx[9],NULL,0,NULL} ,
  {312, "SEQUENCE OF" ,0,16,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "xrefs" ,128,5,0,1,0,0,0,0,NULL,&atx[41],&atx[43],0,NULL} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[11],NULL,0,NULL} ,
  {0, "interactions" ,128,3,0,0,0,0,0,0,NULL,&atx[41],&atx[45],0,NULL} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[46],NULL,0,NULL} ,
  {403, "PBT-Interaction" ,1,0,0,0,0,1,0,0,NULL,&atx[22],&atx[47],0,&atx[51]} ,
  {0, "uid-a" ,128,0,0,0,0,0,0,0,NULL,&atx[9],NULL,0,&atx[48]} ,
  {0, "uid-b" ,128,1,0,0,0,0,0,0,NULL,&atx[9],NULL,0,&atx[49]} ,
  {0, "evidence" ,128,2,0,0,0,0,0,0,NULL,&atx[41],&atx[50],0,NULL} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[51],NULL,0,NULL} ,
  {404, "PBT-Int-Evidence" ,1,0,0,0,0,1,0,0,NULL,&atx[22],&atx[52],0,&atx[17]} ,
  {0, "doc" ,128,0,0,0,0,0,0,0,NULL,&atx[11],NULL,0,&atx[53]} ,
  {0, "names-a" ,128,1,0,0,0,0,0,0,NULL,&atx[41],&atx[54],0,&atx[58]} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[55],NULL,0,NULL} ,
  {406, "PBT-Name-occur" ,1,0,0,0,0,1,0,0,NULL,&atx[22],&atx[56],0,&atx[28]} ,
  {0, "name" ,128,0,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[57]} ,
  {0, "score" ,128,1,0,1,0,0,0,0,NULL,&atx[17],NULL,0,NULL} ,
  {0, "names-b" ,128,2,0,0,0,0,0,0,NULL,&atx[41],&atx[59],0,&atx[60]} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[55],NULL,0,NULL} ,
  {0, "score" ,128,3,0,0,0,0,0,0,NULL,&atx[17],NULL,0,NULL} };

static AsnModule ampx[1] = {
  { "PBT" , "prebind_transfer.h",&atx[0],NULL,NULL,0,0} };

static AsnValxNodePtr avn = avnx;
static AsnTypePtr at = atx;
static AsnModulePtr amp = ampx;



/**************************************************
*
*    Defines for Module PBT
*
**************************************************/

#define PBT_INTERACTION_SET &at[0]
#define PBT_INTERACTION_SET_date &at[1]
#define PBT_INTERACTION_SET_desc &at[3]
#define PBT_INTERACTION_SET_interactors &at[5]
#define PBT_INTERACTION_SET_interactors_E &at[6]
#define PBT_INTERACTION_SET_interactions &at[44]
#define PBT_INTERACTION_SET_interactions_E &at[45]

#define PBT_INTERACTOR &at[7]
#define PBT_INTERACTOR_uid &at[8]
#define PBT_INTERACTOR_source &at[10]
#define PBT_INTERACTOR_type &at[23]
#define PBT_INTERACTOR_taxid &at[25]
#define PBT_INTERACTOR_names &at[26]
#define PBT_INTERACTOR_names_E &at[27]
#define PBT_INTERACTOR_xrefs &at[42]
#define PBT_INTERACTOR_xrefs_E &at[43]

#define PBT_INTERACTION &at[46]
#define PBT_INTERACTION_uid_a &at[47]
#define PBT_INTERACTION_uid_b &at[48]
#define PBT_INTERACTION_evidence &at[49]
#define PBT_INTERACTION_evidence_E &at[50]

#define PBT_INT_EVIDENCE &at[51]
#define PBT_INT_EVIDENCE_doc &at[52]
#define PBT_INT_EVIDENCE_names_a &at[53]
#define PBT_INT_EVIDENCE_names_a_E &at[54]
#define PBT_INT_EVIDENCE_names_b &at[58]
#define PBT_INT_EVIDENCE_names_b_E &at[59]
#define PBT_INT_EVIDENCE_score &at[60]

#define PBT_SCORE &at[17]
#define PBT_SCORE_hypoth &at[18]
#define PBT_SCORE_method &at[19]
#define PBT_SCORE_score &at[20]

#define PBT_NAME_OCCUR &at[55]
#define PBT_NAME_OCCUR_name &at[56]
#define PBT_NAME_OCCUR_score &at[57]

#define PBT_NAME &at[28]
#define PBT_NAME_name_id &at[29]
#define PBT_NAME_objid &at[30]
#define PBT_NAME_name &at[31]
#define PBT_NAME_name_type &at[32]
#define PBT_NAME_source &at[33]
#define PBT_NAME_rule_id &at[34]
#define PBT_NAME_deprecated &at[35]
#define PBT_NAME_date_dep &at[37]
#define PBT_NAME_searched &at[38]
#define PBT_NAME_namered &at[39]
#define PBT_NAME_results &at[40]

#define PBT_DBREF &at[11]
#define PBT_DBREF_dbname &at[12]
#define PBT_DBREF_acc &at[13]
#define PBT_DBREF_id &at[14]
#define PBT_DBREF_field &at[15]
#define PBT_DBREF_score &at[16]
