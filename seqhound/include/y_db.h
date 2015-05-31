#ifndef __YEAST_DB_H__
#define __YEAST_DB_H__

#include <slri_cb.h>
#include <slri_misc.h>
#include <ncbi.h>
#include <d4all.h>
#ifdef __cplusplus
extern "C" {
#endif

  /* holds information about the yeast go annotation */
  typedef struct yeast_st
  {
    CharPtr pORF;        /* ORF name */
    CharPtr pGene;       /* sometimes, the gene name is used instead of the ORF */
    Int4 iGOID;          /* GOID */
    CharPtr pEviCode;    /* evidence code, see below */
    Int4 iPMID;          /* Pubmed ID: the article where the GO annotation was curated from */
  }YST_ST, *YST_STPtr;

  /* the evidence code standardizes the way the annotation of the ORF was
   * obtained: includes:
   *     IC:        inferred by curator
   *     IDA:       inferred by directy assay 
   *     IEA:       inferred from electronic annotation
   *     IEP:	    inferred from expression pattern
   *     IGI:       inferred from genetic interaction
   *     IMP:       inferred from mutant phenotype
   *     IPI:       inferred from physical interaction
   *     ISS:	    inferred from sequence/structure similarity
   *     NAS:       non-traceable author statement
   *     ND:        no biological data available
   *     TAS:       traceable author statement
   *     NR:        not recorded */

  /* looks up yst's ORF in SGD to try to find the gi,
   * if found, stores gi, pmid, goid and evidence code
   * into LL_GO */
  extern Boolean Y_Append_LL(YST_STPtr yst);


  /* initializes the SGD and LL_GO database:
   * the SGD we use to look up the ORF/Gene name to get the GI
   * LL_GO is where we store the GO annotation */
  extern Boolean Y_InitSGD_n_LL(void); 

  
  /* close SGD and LL_GO */
  extern Boolean Y_CloseSGD_n_LL(void);
#ifdef __cplusplus
}
#endif
#endif
