/***********************************************************************
*
**
*        Automatic header module from ASNTOOL
*
************************************************************************/

#ifndef _ASNTOOL_
#include <asn.h>
#endif

static char * asnfilename = "prebind_spec.h";
static AsnValxNode avnx[4] = {
    {20,"not-approved" ,0,0.0,&avnx[1] } ,
    {20,"approved" ,1,0.0,&avnx[2] } ,
    {20,"search-queued" ,2,0.0,&avnx[3] } ,
    {20,"search-completed" ,3,0.0,NULL } };

static AsnType atx[90] = {
  {401, "PREBIND-Acc" ,1,0,0,0,0,1,0,0,NULL,&atx[10],&atx[1],0,&atx[11]} ,
  {0, "acc" ,128,0,0,0,0,0,0,0,NULL,&atx[2],NULL,0,&atx[3]} ,
  {323, "VisibleString" ,0,26,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "gi" ,128,1,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[5]} ,
  {302, "INTEGER" ,0,2,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "db" ,128,2,0,0,0,0,0,0,NULL,&atx[2],NULL,0,&atx[6]} ,
  {0, "locus" ,128,3,0,0,0,0,0,0,NULL,&atx[2],NULL,0,&atx[7]} ,
  {0, "pprod" ,128,4,0,0,0,0,0,0,NULL,&atx[2],NULL,0,&atx[8]} ,
  {0, "tax" ,128,5,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[9]} ,
  {0, "update" ,128,6,0,0,0,0,0,0,NULL,&atx[2],NULL,0,NULL} ,
  {311, "SEQUENCE" ,0,16,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {402, "PREBIND-Acc-set" ,1,0,0,0,0,1,0,0,NULL,&atx[10],&atx[12],0,&atx[15]} ,
  {0, "records" ,128,0,0,0,0,0,0,0,NULL,&atx[14],&atx[13],0,NULL} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[0],NULL,0,NULL} ,
  {312, "SEQUENCE OF" ,0,16,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {403, "PREBIND-Search" ,1,0,0,0,0,1,0,0,NULL,&atx[10],&atx[16],0,&atx[25]} ,
  {0, "gi" ,128,0,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[17]} ,
  {0, "name" ,128,1,0,0,0,0,0,0,NULL,&atx[2],NULL,0,&atx[18]} ,
  {0, "source" ,128,2,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[19]} ,
  {0, "taxid" ,128,3,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[20]} ,
  {0, "searched" ,128,4,0,0,0,0,0,0,NULL,&atx[2],NULL,0,&atx[21]} ,
  {0, "status" ,128,5,0,0,0,0,0,0,NULL,&atx[22],&avnx[0],0,&atx[23]} ,
  {310, "ENUMERATED" ,0,10,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "namered" ,128,6,0,1,0,0,0,0,NULL,&atx[4],NULL,0,&atx[24]} ,
  {0, "results" ,128,7,0,1,0,0,0,0,NULL,&atx[4],NULL,0,NULL} ,
  {404, "PREBIND-Search-set" ,1,0,0,0,0,1,0,0,NULL,&atx[10],&atx[26],0,&atx[28]} ,
  {0, "records" ,128,0,0,0,0,0,0,0,NULL,&atx[14],&atx[27],0,NULL} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[15],NULL,0,NULL} ,
  {405, "PREBIND-Result" ,1,0,0,0,0,1,0,0,NULL,&atx[10],&atx[29],0,&atx[38]} ,
  {0, "gi" ,128,0,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[30]} ,
  {0, "name" ,128,1,0,0,0,0,0,0,NULL,&atx[2],NULL,0,&atx[31]} ,
  {0, "pmid" ,128,2,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[32]} ,
  {0, "iscore" ,128,3,0,0,0,0,0,0,NULL,&atx[33],NULL,0,&atx[34]} ,
  {309, "REAL" ,0,9,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "iscore2" ,128,4,0,0,0,0,0,0,NULL,&atx[33],NULL,0,&atx[35]} ,
  {0, "itruth" ,128,5,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[36]} ,
  {0, "opno" ,128,6,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[37]} ,
  {0, "opyes" ,128,7,0,0,0,0,0,0,NULL,&atx[4],NULL,0,NULL} ,
  {406, "PREBIND-Result-set" ,1,0,0,0,0,1,0,0,NULL,&atx[10],&atx[39],0,&atx[41]} ,
  {0, "records" ,128,0,0,0,0,0,0,0,NULL,&atx[14],&atx[40],0,NULL} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[28],NULL,0,NULL} ,
  {407, "PREBIND-Isvm" ,1,0,0,0,0,1,0,0,NULL,&atx[10],&atx[42],0,&atx[49]} ,
  {0, "pmid" ,128,0,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[43]} ,
  {0, "muid" ,128,1,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[44]} ,
  {0, "iscore" ,128,2,0,0,0,0,0,0,NULL,&atx[33],NULL,0,&atx[45]} ,
  {0, "iscore2" ,128,3,0,0,0,0,0,0,NULL,&atx[33],NULL,0,&atx[46]} ,
  {0, "itruth" ,128,4,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[47]} ,
  {0, "opno" ,128,5,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[48]} ,
  {0, "opyes" ,128,6,0,0,0,0,0,0,NULL,&atx[4],NULL,0,NULL} ,
  {408, "PREBIND-Isvm-set" ,1,0,0,0,0,1,0,0,NULL,&atx[10],&atx[50],0,&atx[52]} ,
  {0, "records" ,128,0,0,0,0,0,0,0,NULL,&atx[14],&atx[51],0,NULL} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[41],NULL,0,NULL} ,
  {409, "PREBIND-Point" ,1,0,0,0,0,1,0,0,NULL,&atx[10],&atx[53],0,&atx[72]} ,
  {0, "pmid" ,128,0,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[54]} ,
  {0, "muid" ,128,1,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[55]} ,
  {0, "gia" ,128,2,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[56]} ,
  {0, "nama" ,128,3,0,0,0,0,0,0,NULL,&atx[2],NULL,0,&atx[57]} ,
  {0, "atype" ,128,4,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[58]} ,
  {0, "adgi" ,128,5,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[59]} ,
  {0, "adf" ,128,6,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[60]} ,
  {0, "adt" ,128,7,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[61]} ,
  {0, "gib" ,128,8,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[62]} ,
  {0, "namb" ,128,9,0,0,0,0,0,0,NULL,&atx[2],NULL,0,&atx[63]} ,
  {0, "btype" ,128,10,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[64]} ,
  {0, "bdgi" ,128,11,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[65]} ,
  {0, "bdf" ,128,12,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[66]} ,
  {0, "bdt" ,128,13,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[67]} ,
  {0, "opno" ,128,14,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[68]} ,
  {0, "opyes" ,128,15,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[69]} ,
  {0, "piscore" ,128,16,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[70]} ,
  {0, "piscore2" ,128,17,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[71]} ,
  {0, "pitruth" ,128,18,0,0,0,0,0,0,NULL,&atx[4],NULL,0,NULL} ,
  {410, "PREBIND-Point-set" ,1,0,0,0,0,1,0,0,NULL,&atx[10],&atx[73],0,&atx[75]} ,
  {0, "records" ,128,0,0,0,0,0,0,0,NULL,&atx[14],&atx[74],0,NULL} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[52],NULL,0,NULL} ,
  {411, "PREBIND-Interactor" ,1,0,0,0,0,1,0,0,NULL,&atx[10],&atx[76],0,&atx[86]} ,
  {0, "gi" ,128,0,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[77]} ,
  {0, "pitruth" ,128,1,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[78]} ,
  {0, "opyes" ,128,2,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[79]} ,
  {0, "opno" ,128,3,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[80]} ,
  {0, "piscore" ,128,4,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[81]} ,
  {0, "piscore2" ,128,5,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[82]} ,
  {0, "num-pmids" ,128,6,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[83]} ,
  {0, "known-pmids" ,128,7,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[84]} ,
  {0, "opyes-pmids" ,128,8,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[85]} ,
  {0, "prob-pmids" ,128,9,0,0,0,0,0,0,NULL,&atx[4],NULL,0,NULL} ,
  {412, "PREBIND-Interactor-set" ,1,0,0,0,0,1,0,0,NULL,&atx[10],&atx[87],0,NULL} ,
  {0, "num" ,128,0,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[88]} ,
  {0, "records" ,128,1,0,0,0,0,0,0,NULL,&atx[14],&atx[89],0,NULL} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[75],NULL,0,NULL} };

static AsnModule ampx[1] = {
  { "PREBIND" , "prebind_spec.h",&atx[0],NULL,NULL,0,0} };

static AsnValxNodePtr avn = avnx;
static AsnTypePtr at = atx;
static AsnModulePtr amp = ampx;



/**************************************************
*
*    Defines for Module PREBIND
*
**************************************************/

#define PREBIND_ACC &at[0]
#define PREBIND_ACC_acc &at[1]
#define PREBIND_ACC_gi &at[3]
#define PREBIND_ACC_db &at[5]
#define PREBIND_ACC_locus &at[6]
#define PREBIND_ACC_pprod &at[7]
#define PREBIND_ACC_tax &at[8]
#define PREBIND_ACC_update &at[9]

#define PREBIND_ACC_SET &at[11]
#define PREBIND_ACC_SET_records &at[12]
#define PREBIND_ACC_SET_records_E &at[13]

#define PREBIND_SEARCH &at[15]
#define PREBIND_SEARCH_gi &at[16]
#define PREBIND_SEARCH_name &at[17]
#define PREBIND_SEARCH_source &at[18]
#define PREBIND_SEARCH_taxid &at[19]
#define PREBIND_SEARCH_searched &at[20]
#define PREBIND_SEARCH_status &at[21]
#define PREBIND_SEARCH_namered &at[23]
#define PREBIND_SEARCH_results &at[24]

#define PREBIND_SEARCH_SET &at[25]
#define PREBIND_SEARCH_SET_records &at[26]
#define PREBIND_SEARCH_SET_records_E &at[27]

#define PREBIND_RESULT &at[28]
#define PREBIND_RESULT_gi &at[29]
#define PREBIND_RESULT_name &at[30]
#define PREBIND_RESULT_pmid &at[31]
#define PREBIND_RESULT_iscore &at[32]
#define PREBIND_RESULT_iscore2 &at[34]
#define PREBIND_RESULT_itruth &at[35]
#define PREBIND_RESULT_opno &at[36]
#define PREBIND_RESULT_opyes &at[37]

#define PREBIND_RESULT_SET &at[38]
#define PREBIND_RESULT_SET_records &at[39]
#define PREBIND_RESULT_SET_records_E &at[40]

#define PREBIND_ISVM &at[41]
#define PREBIND_ISVM_pmid &at[42]
#define PREBIND_ISVM_muid &at[43]
#define PREBIND_ISVM_iscore &at[44]
#define PREBIND_ISVM_iscore2 &at[45]
#define PREBIND_ISVM_itruth &at[46]
#define PREBIND_ISVM_opno &at[47]
#define PREBIND_ISVM_opyes &at[48]

#define PREBIND_ISVM_SET &at[49]
#define PREBIND_ISVM_SET_records &at[50]
#define PREBIND_ISVM_SET_records_E &at[51]

#define PREBIND_POINT &at[52]
#define PREBIND_POINT_pmid &at[53]
#define PREBIND_POINT_muid &at[54]
#define PREBIND_POINT_gia &at[55]
#define PREBIND_POINT_nama &at[56]
#define PREBIND_POINT_atype &at[57]
#define PREBIND_POINT_adgi &at[58]
#define PREBIND_POINT_adf &at[59]
#define PREBIND_POINT_adt &at[60]
#define PREBIND_POINT_gib &at[61]
#define PREBIND_POINT_namb &at[62]
#define PREBIND_POINT_btype &at[63]
#define PREBIND_POINT_bdgi &at[64]
#define PREBIND_POINT_bdf &at[65]
#define PREBIND_POINT_bdt &at[66]
#define PREBIND_POINT_opno &at[67]
#define PREBIND_POINT_opyes &at[68]
#define PREBIND_POINT_piscore &at[69]
#define PREBIND_POINT_piscore2 &at[70]
#define PREBIND_POINT_pitruth &at[71]

#define PREBIND_POINT_SET &at[72]
#define PREBIND_POINT_SET_records &at[73]
#define PREBIND_POINT_SET_records_E &at[74]

#define PREBIND_INTERACTOR &at[75]
#define PREBIND_INTERACTOR_gi &at[76]
#define PREBIND_INTERACTOR_pitruth &at[77]
#define PREBIND_INTERACTOR_opyes &at[78]
#define PREBIND_INTERACTOR_opno &at[79]
#define PREBIND_INTERACTOR_piscore &at[80]
#define PREBIND_INTERACTOR_piscore2 &at[81]
#define PREBIND_INTERACTOR_num_pmids &at[82]
#define PREBIND_INTERACTOR_known_pmids &at[83]
#define PREBIND_INTERACTOR_opyes_pmids &at[84]
#define PREBIND_INTERACTOR_prob_pmids &at[85]

#define PREBIND_INTERACTOR_SET &at[86]
#define PREBIND_INTERACTOR_SET_num &at[87]
#define PREBIND_INTERACTOR_SET_records &at[88]
#define PREBIND_INTERACTOR_SET_records_E &at[89]
