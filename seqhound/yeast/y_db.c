/* $Id: y_db.c,v 1.2 2004/01/06 22:32:18 skuznets Exp $
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

  March 1, 2001  Original MSH Revision.
                Hogue Lab - University of Toronto Biochemistry 
                Department and the Samuel Lunenfeld Research 
                Institute, Mount Sinai Hospital
                http://bioinfo.mshri.on.ca  hogue@mshri.on.ca
                ftp://ftp.mshri.on.ca/


  PUBLICATION TO CITE:

  DESCRIPTION:
   The db layer for the GO Yeast annotation module.
   Adds the GO yeast annotation to seqhound's existing LL_GO database.

****************************************************************************/

#include <d4all.h>
#include <y_db.h>
#include <sgd_db.h>
#include <ll_db.h>

#ifdef __cplusplus
extern "C" {
#endif


/* initializes the SGD and LL_GO database:
 * the SGD we use to look up the ORF/Gene name to get the GI
 * LL_GO is where we store the GO annotation */
extern Boolean Y_InitSGD_n_LL()
{
   SGD_InitDataBase();
   SGD_OpenDB(FALSE);
   LL_InitCodeBase();
   LL_OpenAllDBs(FALSE);
   return TRUE;
}

/* close SGD and LL_GO */
extern Boolean Y_CloseSGD_n_LL()
{
   SGD_CloseDataBase();
   CloseCodeBaseAndLLDBs();
   return TRUE;
}

/* looks up yst's ORF in SGD to find the ORF's gi.
 * if found, the gi, pmid, evidence code and goid is appended
 * to LL_GO, otherwise, does nothing */
extern Boolean Y_Append_LL(YST_STPtr yst)
{
  SGD_ST sgd = {0, NULL, NULL};
  if(yst == NULL)
  {
     ErrPostEx(SEV_WARNING, 0, 0, "[YST]: NULL object cannot be appended");
     return FALSE;
  }
  /* printf("%s %s %s %d %s %d %d\n", IsNullStr(yst->pORF), IsNullStr(yst->pGene), IsNullStr(yst->pGO_term), yst->iGOID, IsNullStr(yst->pEviCode), yst->iPMID, yst->iGi); */

  /* 1st look up the ORF in SGD */
  sgd.pORF = yst->pORF;
  SearchSGD(&sgd);
  if(sgd.iGI == 0)
  {
    /* did not find the ORF, sometimes the ORF is actually 
     * the gene name, so we try with that */
    sgd.pORF = yst->pGene;
    SearchSGD(&sgd);
  }

  /* did not find gene name, so ORF/Gene name not found in RefSeq,
   * so no gi exists for this record, cannnot store in LL_GO */
  if(sgd.iGI == 0)
  {
     ErrPostEx(SEV_WARNING, 0, 0, "[YST]: Could not find this yeast name in RefSeq %s", IsNullStr(yst->pORF));
     return FALSE;
  }

  /* got a Gi, so add record to LL_GO */
  LL_Append2GO_DB(sgd.iGI, yst->iGOID, yst->iPMID, yst->pEviCode);
  /* printf("%d %d %d %s\n", sgd.iGI, yst->iGOID, yst->iPMID, IsNullStr(yst->pEviCode)); */
  /* printf("%s %s %d\n", IsNullStr(sgd.pORF), IsNullStr(sgd.pAcc), sgd.iGI); */
  return TRUE;
}

#ifdef __cplusplus
}
#endif


/*
$Log: y_db.c,v $
Revision 1.2  2004/01/06 22:32:18  skuznets
Code audit has been done sucessfully.
Possible NULL pointer issues has been eliminated.

Revision 1.1.1.2  2003/06/28 08:45:45  root
Automated import from Sourceforge.net

Revision 1.2  2003/06/26 19:22:07  haocl
cleaned up code, added comments and execution instructions

*/
