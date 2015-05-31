/***********************************************************************
*
**
*        Automatic header module from ASNTOOL
*
************************************************************************/

#ifndef _ASNTOOL_
#include <asn.h>
#endif

static char * asnfilename = "slrilibstruc.h";
static AsnValxNode avnx[3] = {
    {20,"none" ,0,0.0,&avnx[1] } ,
    {20,"vis" ,1,0.0,NULL } ,
    {3,NULL,0,0.0,NULL } };

static AsnType atx[71] = {
  {401, "SLRISeq-id" ,1,0,0,0,0,1,0,0,NULL,&atx[3],&atx[1],0,&atx[4]} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[2],NULL,0,NULL} ,
  {407, "Seq-id" ,1,0,0,0,0,0,1,0,NULL,NULL,NULL,0,&atx[27]} ,
  {312, "SEQUENCE OF" ,0,16,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {402, "SLRIValNode" ,1,0,0,0,0,1,0,0,NULL,&atx[3],&atx[5],0,&atx[16]} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[6],NULL,0,NULL} ,
  {411, "SLRINode" ,1,0,0,0,0,0,0,0,NULL,&atx[32],&atx[7],0,&atx[33]} ,
  {0, "gi" ,128,0,0,0,0,0,0,0,NULL,&atx[8],NULL,0,&atx[9]} ,
  {302, "INTEGER" ,0,2,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "accession" ,128,1,0,0,0,0,0,0,NULL,&atx[10],NULL,0,&atx[11]} ,
  {323, "VisibleString" ,0,26,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "bioseq" ,128,2,0,0,0,0,0,0,NULL,&atx[12],NULL,0,&atx[13]} ,
  {405, "Bioseq" ,1,0,0,0,0,0,1,0,NULL,NULL,NULL,0,&atx[14]} ,
  {0, "biostruc" ,128,3,0,0,0,0,0,0,NULL,&atx[14],NULL,0,&atx[15]} ,
  {406, "Biostruc" ,1,0,0,0,0,0,1,0,NULL,NULL,NULL,0,&atx[2]} ,
  {0, "fasta" ,128,4,0,0,0,0,0,0,NULL,&atx[16],NULL,0,&atx[20]} ,
  {403, "SLRIFasta" ,1,0,0,0,0,1,0,0,NULL,&atx[19],&atx[17],0,&atx[22]} ,
  {0, "defline" ,128,0,0,0,0,0,0,0,NULL,&atx[10],NULL,0,&atx[18]} ,
  {0, "sequence" ,128,1,0,0,0,0,0,0,NULL,&atx[10],NULL,0,NULL} ,
  {311, "SEQUENCE" ,0,16,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "seqid" ,128,5,0,0,0,0,0,0,NULL,&atx[0],NULL,0,&atx[21]} ,
  {0, "domain" ,128,6,0,0,0,0,0,0,NULL,&atx[22],NULL,0,&atx[26]} ,
  {404, "SLRIDomain" ,1,0,0,0,0,1,0,0,NULL,&atx[19],&atx[23],0,&atx[12]} ,
  {0, "gi" ,128,0,0,0,0,0,0,0,NULL,&atx[8],NULL,0,&atx[24]} ,
  {0, "from" ,128,1,0,0,0,0,0,0,NULL,&atx[8],NULL,0,&atx[25]} ,
  {0, "to" ,128,2,0,0,0,0,0,0,NULL,&atx[8],NULL,0,NULL} ,
  {0, "seqalign" ,128,7,0,0,0,0,0,0,NULL,&atx[27],NULL,0,&atx[28]} ,
  {408, "Seq-align" ,1,0,0,0,0,0,1,0,NULL,NULL,NULL,0,&atx[29]} ,
  {0, "bioseqset" ,128,8,0,0,0,0,0,0,NULL,&atx[29],NULL,0,&atx[30]} ,
  {409, "Bioseq-set" ,1,0,0,0,0,0,1,0,NULL,NULL,NULL,0,&atx[31]} ,
  {0, "seqentry" ,128,9,0,0,0,0,0,0,NULL,&atx[31],NULL,0,NULL} ,
  {410, "Seq-entry" ,1,0,0,0,0,0,1,0,NULL,NULL,NULL,0,&atx[6]} ,
  {315, "CHOICE" ,0,-1,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {412, "DTree" ,1,0,0,0,0,0,0,0,NULL,&atx[3],&atx[34],0,&atx[35]} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[35],NULL,0,NULL} ,
  {413, "DNode" ,1,0,0,0,0,0,0,0,NULL,&atx[19],&atx[36],0,&atx[37]} ,
  {0, "key" ,128,0,0,0,0,0,0,0,NULL,&atx[37],NULL,0,&atx[42]} ,
  {414, "DKey" ,1,0,0,0,0,0,0,0,NULL,&atx[32],&atx[38],0,&atx[43]} ,
  {0, "id" ,128,0,0,0,0,0,0,0,NULL,&atx[8],NULL,0,&atx[39]} ,
  {0, "str" ,128,1,0,0,0,0,0,0,NULL,&atx[10],NULL,0,&atx[40]} ,
  {0, "real" ,128,2,0,0,0,0,0,0,NULL,&atx[41],NULL,0,NULL} ,
  {309, "REAL" ,0,9,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "data" ,128,1,0,1,0,0,0,0,NULL,&atx[43],NULL,0,&atx[64]} ,
  {415, "DData" ,1,0,0,0,0,0,0,0,NULL,&atx[32],&atx[44],0,NULL} ,
  {0, "id" ,128,0,0,0,0,0,0,0,NULL,&atx[8],NULL,0,&atx[45]} ,
  {0, "str" ,128,1,0,0,0,0,0,0,NULL,&atx[10],NULL,0,&atx[46]} ,
  {0, "real" ,128,2,0,0,0,0,0,0,NULL,&atx[41],NULL,0,&atx[47]} ,
  {0, "bool" ,128,3,0,0,0,0,0,0,NULL,&atx[48],NULL,0,&atx[49]} ,
  {301, "BOOLEAN" ,0,1,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "os" ,128,4,0,0,0,0,0,0,NULL,&atx[50],NULL,0,&atx[51]} ,
  {304, "OCTET STRING" ,0,4,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "strs" ,128,5,0,0,0,0,0,0,NULL,&atx[3],&atx[52],0,&atx[53]} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[10],NULL,0,NULL} ,
  {0, "ids" ,128,6,0,0,0,0,0,0,NULL,&atx[3],&atx[54],0,&atx[55]} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[8],NULL,0,NULL} ,
  {0, "reals" ,128,7,0,0,0,0,0,0,NULL,&atx[3],&atx[56],0,&atx[57]} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[41],NULL,0,NULL} ,
  {0, "oss" ,128,8,0,0,0,0,0,0,NULL,&atx[3],&atx[58],0,&atx[59]} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[50],NULL,0,NULL} ,
  {0, "gen-memfree" ,128,9,0,0,0,0,0,0,NULL,&atx[10],NULL,0,&atx[60]} ,
  {0, "data-list" ,128,10,0,0,0,0,0,0,NULL,&atx[3],&atx[61],0,&atx[62]} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[43],NULL,0,NULL} ,
  {0, "user-def" ,128,11,0,0,0,0,0,0,NULL,&atx[63],NULL,0,NULL} ,
  {305, "NULL" ,0,5,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "num" ,128,2,0,1,0,0,0,0,NULL,&atx[8],NULL,0,&atx[65]} ,
  {0, "state" ,128,3,0,0,1,0,0,0,&avnx[2],&atx[66],&avnx[0],0,&atx[67]} ,
  {310, "ENUMERATED" ,0,10,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "p" ,128,4,0,1,0,0,0,0,NULL,&atx[3],&atx[68],0,&atx[69]} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[35],NULL,0,NULL} ,
  {0, "c" ,128,5,0,1,0,0,0,0,NULL,&atx[3],&atx[70],0,NULL} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[35],NULL,0,NULL} };

static AsnModule ampx[1] = {
  { "SLRILIB-struc" , "slrilibstruc.h",&atx[0],NULL,NULL,0,0} };

static AsnValxNodePtr avn = avnx;
static AsnTypePtr at = atx;
static AsnModulePtr amp = ampx;



/**************************************************
*
*    Defines for Module SLRILIB-struc
*
**************************************************/

#define SLRISEQ_ID &at[0]
#define SLRISEQ_ID_E &at[1]

#define SLRIVALNODE &at[4]
#define SLRIVALNODE_E &at[5]

#define SLRIFASTA &at[16]
#define SLRIFASTA_defline &at[17]
#define SLRIFASTA_sequence &at[18]

#define SLRIDOMAIN &at[22]
#define SLRIDOMAIN_gi &at[23]
#define SLRIDOMAIN_from &at[24]
#define SLRIDOMAIN_to &at[25]

#define SLRINODE &at[6]
#define SLRINODE_gi &at[7]
#define SLRINODE_accession &at[9]
#define SLRINODE_bioseq &at[11]
#define SLRINODE_biostruc &at[13]
#define SLRINODE_fasta &at[15]
#define SLRINODE_seqid &at[20]
#define SLRINODE_domain &at[21]
#define SLRINODE_seqalign &at[26]
#define SLRINODE_bioseqset &at[28]
#define SLRINODE_seqentry &at[30]

#define DTREE &at[33]
#define DTREE_E &at[34]

#define DNODE &at[35]
#define DNODE_key &at[36]
#define DNODE_data &at[42]
#define DNODE_num &at[64]
#define DNODE_state &at[65]
#define DNODE_p &at[67]
#define DNODE_p_E &at[68]
#define DNODE_c &at[69]
#define DNODE_c_E &at[70]

#define DKEY &at[37]
#define DKEY_id &at[38]
#define DKEY_str &at[39]
#define DKEY_real &at[40]

#define DDATA &at[43]
#define DDATA_id &at[44]
#define DDATA_str &at[45]
#define DDATA_real &at[46]
#define DDATA_bool &at[47]
#define DDATA_os &at[49]
#define DDATA_strs &at[51]
#define DDATA_strs_E &at[52]
#define DDATA_ids &at[53]
#define DDATA_ids_E &at[54]
#define DDATA_reals &at[55]
#define DDATA_reals_E &at[56]
#define DDATA_oss &at[57]
#define DDATA_oss_E &at[58]
#define DDATA_gen_memfree &at[59]
#define DDATA_data_list &at[60]
#define DDATA_data_list_E &at[61]
#define DDATA_user_def &at[62]
