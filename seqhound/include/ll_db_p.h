/* $Id: ll_db_p.h,v 1.4 2004/05/13 17:39:13 hlieu Exp $
***************************************************************************
  Copyright (C) 2001 Mount Sinai Hospital (MSH)

  This program is free software; you can redistribute it and/or 
  modify it under the terms of the GNU General Public License as 
  published by the Free Software Foundation; either version 2 of 
  the License, or any later version.

  This program is distributed in the hope that it will be useful, 
  but WITHOUT ANY WARRANTY; without even the implied warranty of 
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 

  See the GNU General Public License for more details.

  You should have received a copy of the GNU General Public License 
  along with this program; if not, write to the
         Free Software Foundation, Inc., 
         59 Temple Place, Suite 330, Boston, MA  
         02111-1307  USA 
  or visit http://www.gnu.org/copyleft/gpl.html

  SPECIAL EXCEPTIONS

  As a special exception, Mount Sinai Hospital gives permission to 
  link this program with the following non-GPL programs or libraries, 
  and distribute the resulting executable, without including the source 
  code for these in the source distribution:

  a) CodeBase 6.5 or greater from Sequiter Software Inc.

  b) The C or C++ interface to Oracle 8.x or greater, from
     Oracle Corporation or IBM DB2 UDB.
 

  PROGRAM:
 

  AUTHORS:  
            Hao Lieu


  REVISION/CONTRIBUTION NOTES:

  June 1, 2001  Original MSH Revision.
                Hogue Lab - University of Toronto Biochemistry 
                Department and the Samuel Lunenfeld Research 
                Institute, Mount Sinai Hospital
                http://bioinfo.mshri.on.ca  hogue@mshri.on.ca
                ftp://ftp.mshri.on.ca/


  PUBLICATION TO CITE:

  DESCRIPTION
  Locus Link code base privates
  Part of SeqHound

****************************************************************************/

#ifndef __LL_DB_P_H__
#define __LL_DB_P_H__

#include <d4all.h>

#ifdef __cplusplus
extern "C" {
#endif
#define NP_ID_SIZE 8
#define GO_ID_SIZE 7
#define PMID_SIZE 10
#define MEMO_SIZE 10
#define OMIM_SIZE 6
#define CDDID_SIZE 15
#define E_VAL_SIZE 15
#define ECODE_SIZE 4
extern CODE4 cb;

  static DATA4  *pD4LL_GO = NULL;      /* the LLID-GOID relation db */
  static DATA4  *pD4LL_OMIM = NULL;    /* the LLID-OMIM relation db */
  static DATA4  *pD4LL_CDD = NULL;     /* the LLID-CDD relation db */    
  static DATA4  *pD4LL_LLID = NULL;    /* the LLID-GI relation db */

  static FIELD4 *pF4LL_LL_GO;      /* the LLID field in the GOID DB */
  static FIELD4 *pF4LL_LL_OMIM;    /* the LLID field in the OMIM DB */
  static FIELD4 *pF4LL_LL_CDD;     /* the LLID field in the CDD DB */
  static FIELD4 *pF4LL_LL_LLID;    /* the LLID field in the LLID DB */

  static FIELD4 *pF4LL_GOID;       /* the GOID of the gene loci in GOID DB */
  static FIELD4 *pF4LL_PMID;       /* the PMID of the gene loci in GOID DB */
  static FIELD4 *pF4LL_EVICODE;    /* the evidence code in the GOID DB */
  static FIELD4 *pF4LL_OMIM;       /* the OMIM code in the OMIM DB */
  static FIELD4 *pF4LL_CDD;        /* the CDD_ID code in the CDD DB */
  static FIELD4 *pF4LL_E_VAL;      /* the e-value in the CDD DB */
  static FIELD4 *pF4LL_GI;         /* the GI in the LLID DB */
  static FIELD4 *pF4LL_MAP;        /* the loci of the sequence */

  static TAG4 *pT4LL_LL_GO;    /* the LLID tag in the GOID DB */
  static TAG4 *pT4LL_LL_OMIM;  /* the LLID tag in the OMIM DB */
  static TAG4 *pT4LL_LL_CDD;   /* the LLID tag in the CDD DB */
  static TAG4 *pT4LL_LL_LLID;  /* the LLID tag in the LLID DB */

  static TAG4 *pT4LL_GOID;     /* the GOID tag in the GOID DB */
  static TAG4 *pT4LL_PMID;     /* the PMID tag in the GOID DB */  
  static TAG4 *pT4LL_OMIM;     /* the OMIM tag in the OMIM DB */
  static TAG4 *pT4LL_CDD;      /* the CDD tag in the CDD DB */
  static TAG4 *pT4LL_GI;     /* the GI tag in the LLID DB */

  static FIELD4INFO fieldInfo_GO[] =
  {
    {"LL_GO", r4num, NP_ID_SIZE, 0},
    {"GO_ID", r4num, GO_ID_SIZE, 0},
    {"PMID", r4num, PMID_SIZE, 0},
    {"EVICODE", r4str, ECODE_SIZE, 0},
    {0, 0, 0, 0, 0}
  };

  static FIELD4INFO fieldInfo_OMIM[] =
  {
    {"LL_OMIM", r4num, NP_ID_SIZE, 0},
    {"OMIM", r4num, OMIM_SIZE, 0},
    {0, 0, 0, 0, 0}
  };

  static FIELD4INFO fieldInfo_CDD[] = 
  {
    {"LL_CDD", r4num, NP_ID_SIZE, 0},
    {"CDD_ID", r4str, CDDID_SIZE, 0},
    {"E_VALUE", r4float, E_VAL_SIZE, 10},
    {0, 0, 0, 0, 0}
  };

  static FIELD4INFO fieldInfo_LL[] =
  {
    {"LL_LLID", r4num, NP_ID_SIZE , 0},
    {"GI_ID", r4num, NP_ID_SIZE, 0},
    {"LL_MAP", r4memo, MEMO_SIZE, 0},
    {0, 0, 0, 0, 0}
  };

  static TAG4INFO tagInfo_GO[] =
  {
    {"LL_TAG_GO", "LL_GO", 0, 0, 0},
    {"GOID_TAG", "GO_ID", 0, 0, 0},
    {"PMID_TAG", "PMID", 0, 0, 0},
    {0, 0, 0, 0, 0}
  };
  
  static TAG4INFO tagInfo_OMIM[] =
  {
    {"LL_TAG_OMIM", "LL_OMIM", 0, 0, 0},
    {"OMIM_TAG", "OMIM", 0, 0, 0},
    {0, 0, 0, 0, 0}
  };
  
  static TAG4INFO tagInfo_CDD[] =
  {
    {"LL_TAG_CDD", "LL_CDD", 0, 0, 0},
    {"CDD_TAG", "CDD_ID", 0, 0, 0},
    {0, 0, 0, 0, 0}
  };
  
  static TAG4INFO tagInfo_LL[] =
  {
    {"LL_TAG_LL", "LL_LLID", 0, 0, 0},
    {"GI_TAG", "GI_ID", 0, 0, 0},
    {0, 0, 0, 0, 0}
  };

#ifdef __cplusplus
}
#endif
#endif

/*
$Log: ll_db_p.h,v $
Revision 1.4  2004/05/13 17:39:13  hlieu
major refactor of locus link module. Primary key changed from gi to
locus id. ASN source code also required update.

Revision 1.3  2004/02/27 17:35:43  zwang
Changed CDDID_SIZE from 12 to 15.

Revision 1.2  2003/08/26 19:14:16  haocl
updated size of PMID

Revision 1.1.1.2  2001/12/12 09:22:27  root
Automated import from Sourceforge.net

Revision 1.2  2001/12/11 19:08:43  kaca
added Id and Log keywords

*/
