/***********************************************************************
*
**
*        Automatic header module from ASNTOOL
*
************************************************************************/

#ifndef _ASNTOOL_
#include <asn.h>
#endif

static char * asnfilename = "NBlastasn.h";
static AsnType atx[27] = {
  {401, "NBlast-Result-Set" ,1,0,0,0,0,1,0,0,NULL,&atx[19],&atx[1],0,&atx[6]} ,
  {0, "query-gi" ,128,0,0,1,0,0,0,0,NULL,&atx[2],NULL,0,&atx[3]} ,
  {302, "INTEGER" ,0,2,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "subject-gi" ,128,1,0,1,0,0,0,0,NULL,&atx[2],NULL,0,&atx[4]} ,
  {0, "results" ,128,2,0,0,0,0,0,0,NULL,&atx[10],&atx[5],0,NULL} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[6],NULL,0,NULL} ,
  {402, "NBlast-Result" ,1,0,0,0,0,1,0,0,NULL,&atx[19],&atx[7],0,&atx[15]} ,
  {0, "numseg" ,128,0,0,0,0,0,0,0,NULL,&atx[2],NULL,0,&atx[8]} ,
  {0, "starts" ,128,1,0,0,0,0,0,0,NULL,&atx[10],&atx[9],0,&atx[11]} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[2],NULL,0,NULL} ,
  {312, "SEQUENCE OF" ,0,16,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "lens" ,128,2,0,0,0,0,0,0,NULL,&atx[10],&atx[12],0,&atx[13]} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[2],NULL,0,NULL} ,
  {0, "scores" ,128,3,0,0,0,0,0,0,NULL,&atx[10],&atx[14],0,NULL} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[15],NULL,0,NULL} ,
  {403, "NBlast-Scores" ,1,0,0,0,0,1,0,0,NULL,&atx[19],&atx[16],0,&atx[20]} ,
  {0, "bitscore" ,128,0,0,0,0,0,0,0,NULL,&atx[17],NULL,0,&atx[18]} ,
  {309, "REAL" ,0,9,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "evalue" ,128,1,0,0,0,0,0,0,NULL,&atx[17],NULL,0,NULL} ,
  {311, "SEQUENCE" ,0,16,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {404, "NBlast-GiAndEval" ,1,0,0,0,0,1,0,0,NULL,&atx[19],&atx[21],0,&atx[23]} ,
  {0, "subject-gi" ,128,0,0,0,0,0,0,0,NULL,&atx[2],NULL,0,&atx[22]} ,
  {0, "evalue" ,128,1,0,0,0,0,0,0,NULL,&atx[17],NULL,0,NULL} ,
  {405, "NBlast-GiAndEval-set" ,1,0,0,0,0,1,0,0,NULL,&atx[19],&atx[24],0,NULL} ,
  {0, "query-gi" ,128,0,0,1,0,0,0,0,NULL,&atx[2],NULL,0,&atx[25]} ,
  {0, "giandevals" ,128,1,0,0,0,0,0,0,NULL,&atx[10],&atx[26],0,NULL} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[20],NULL,0,NULL} };

static AsnModule ampx[1] = {
  { "NBlast" , "NBlastasn.h",&atx[0],NULL,NULL,0,0} };

static AsnValxNodePtr avn = NULL;
static AsnTypePtr at = atx;
static AsnModulePtr amp = ampx;



/**************************************************
*
*    Defines for Module NBlast
*
**************************************************/

#define NBLAST_RESULT_SET &at[0]
#define NBLAST_RESULT_SET_query_gi &at[1]
#define NBLAST_RESULT_SET_subject_gi &at[3]
#define NBLAST_RESULT_SET_results &at[4]
#define NBLAST_RESULT_SET_results_E &at[5]

#define NBLAST_RESULT &at[6]
#define NBLAST_RESULT_numseg &at[7]
#define NBLAST_RESULT_starts &at[8]
#define NBLAST_RESULT_starts_E &at[9]
#define NBLAST_RESULT_lens &at[11]
#define NBLAST_RESULT_lens_E &at[12]
#define NBLAST_RESULT_scores &at[13]
#define NBLAST_RESULT_scores_E &at[14]

#define NBLAST_SCORES &at[15]
#define NBLAST_SCORES_bitscore &at[16]
#define NBLAST_SCORES_evalue &at[18]

#define NBLAST_GIANDEVAL &at[20]
#define NBLAST_GIANDEVAL_subject_gi &at[21]
#define NBLAST_GIANDEVAL_evalue &at[22]

#define NBLAST_GIANDEVAL_SET &at[23]
#define NBLAST_GIANDEVAL_SET_query_gi &at[24]
#define NBLAST_GIANDEVAL_SET_giandevals &at[25]
#define NBLAST_GIANDEVAL_SET_giandevals_E &at[26]
