#ifndef __YEAST_DB_H__
#define __YEAST_DB_H__

#include <slri_cb.h>
#include <slri_misc.h>
#include <ncbi.h>
#include <d4all.h>
#ifdef __cplusplus
extern "C" {
#endif

#define NP_ID_SIZE 8
#define GO_ID_SIZE 6
#define PMID_SIZE 8
#define MEMO_SIZE 10

  typedef struct yeast_st
  {
    Int4 iGi;
    Int4 iGOID;
    Int4 iPMID;
    CharPtr pEviCode;
  }YEAST_ST, *YEAST_STPtr;

	
extern CODE4 cb;

  static DATA4 *pD4LL_GO = NULL;
  static FIELD4 *pF4ll_goid;
  static FIELD4 *pF4ll_pmid;
  static FIELD4 *pF4ll_evicode;
  static FIELD4 *pF4ll_go;

  static TAG4 *pT4ll_goid;
  static TAG4 *pT4ll_pmid;
  static TAG4 *pT4ll_go;

  static FIELD4INFO fieldInfo_go[] =
  {
    {"NP_GO", r4num, NP_ID_SIZE, 0},
    {"GO_ID", r4num, GO_ID_SIZE, 0},
    {"PMID", r4num, PMID_SIZE, 0},
    {"EVICODE", r4memo, MEMO_SIZE, 0},
    {0, 0, 0, 0, 0}
  };

  static TAG4INFO tagInfo_go[] =
  {
    {"NP_TAG_GO", "NP_GO", 0, 0, 0},
    {"GOID_TAG", "GO_ID", 0, 0, 0},
    {"PMID_TAG", "PMID", 0, 0, 0},
    {0, 0, 0, 0, 0}
  };
  extern void YEAST_InitCodeBase(void);
  extern Boolean OpenYEASTDB(Boolean CREATE);
  
#ifdef __cplusplus
}
#endif
#endif
