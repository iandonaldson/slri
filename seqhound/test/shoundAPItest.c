/* $Id: shoundAPItest.c,v 1.1 2004/03/23 20:27:38 vgu Exp $
*******************************************************************************
$Copyright (C) 2000 Mount Sinai Hospital (MSH)

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
          vgu@blueprint.org

  REVISION/CONTRIBUTION NOTES:

  March 10, 2004 Original Blueprint Revision
                 Samuel Lunenfeld Research
                 Institute, Mount Sinai Hospital
                 http://bioinfo.mshri.on.ca  hogue@mshri.on.ca
                 ftp://ftp.mshri.on.ca/


  PUBLICATION TO CITE:


  DESCRIPTION
    January 1999
    tests SeqHound remote API functions;
    October 2001
    Tests the all functions in current SeqHound but neighbours.
    The default params gave positive results at the time of creating this test.
    If functions fail; it could be caused by a function param which ceased to
    exist.
    Some of the params can be changed through input arguments; some are fixed for
    the sake of simplicity.


*****************************************************************************************/
#include <seqhound.h>
#include <CuTest.h>
#include "shoundAPItest.h"

/************* Parameters for Testing Invalid Input *************/
#define MAXINT 3000000000
#define EMPTYSTR ""
#define MINUSONE -1
/* The following variables are defined as static so that they are only available to the functions in the 
   file. 
*/
   /* Input Data Type and  Values*/ 
   static Int4 gi = 928863, taxId =9, mmdbId = 11, omimId = 103950;
   static Int4 chromId=142, goId = 7612, llId = 9, redundId = 610108, refType = 1, class = 12, retiredGi=0;
   static FloatHi eValue = 10;
   static CharPtr accession = "X88853", reference = "EC", eCode = "ND", cddid = "pfam00664", name="AF060234";

   static ValNodePtr giList = NULL;
   static ValNodePtr accList = NULL;
   static ValNodePtr nameList = NULL;
   static ValNodePtr mmdbList = NULL;
   static ValNodePtr taxidList = NULL;
   static ValNodePtr chromList = NULL;
   static ValNodePtr omimList = NULL;
   static ValNodePtr llidList = NULL;
   static ValNodePtr cddList = NULL;

   /* Return Data Type and Values */
   static Boolean retBool = FALSE;
   static BioseqPtr retBioseq = NULL;
   static BiostrucPtr retBiostruc = NULL;
   static SLRIFastaPtr retSLRIFasta = NULL;
   static Int4 retInt = 0;
   static CharPtr retStr = NULL;
   static ValNodePtr retValNode = NULL;
   static CLinkSetPtr retCLinkSet = NULL;
   static ILinkSetPtr retILinkSet = NULL;
   static ILinkSetListPtr retILinkSetList = NULL;
   static FLinkSetPtr retFLinkSet = NULL;
   static FLinkSetListPtr retFLinkSetList = NULL;
   static FLinkSetList2Ptr retFLinkSetList2 = NULL;
   static StRpsdbPtr retStRpsdb = NULL;
   static SeqAlignPtr retSeqAlign = NULL;
   static SeqIdPtr retSeqId = NULL;
   static NBlastResultSetPtr retNBlastResultSet = NULL;
 
   CuSuite* ShoundTestGetSuite () 
   {
     CuSuite* suite = CuSuiteNew();
     //SUITE_ADD_TEST(suite, TestSHoundGetNameByGi);
     SUITE_ADD_TEST(suite, TestSHoundFindAcc);
     SUITE_ADD_TEST(suite, TestSHoundFindAccList);
     SUITE_ADD_TEST(suite, TestSHoundAccFromGi);
     SUITE_ADD_TEST(suite, TestSHoundAccFromGiList);
     SUITE_ADD_TEST(suite, TestSHoundFindName);
     SUITE_ADD_TEST(suite, TestSHoundFindNameList);
     SUITE_ADD_TEST(suite, TestSHoundSeqIdFromGi);
     SUITE_ADD_TEST(suite, TestSHoundSeqIdFromGiList);
     SUITE_ADD_TEST(suite, TestSHoundIsProtein);
     SUITE_ADD_TEST(suite, TestSHoundIsNRFirst);
     SUITE_ADD_TEST(suite, TestSHoundGetBioseq);
     SUITE_ADD_TEST(suite, TestSHoundGetBioseqList);
     SUITE_ADD_TEST(suite, TestSHound3DExists);
     SUITE_ADD_TEST(suite, TestSHound3DbWhat);
     SUITE_ADD_TEST(suite, TestSHoundGet3D);
     SUITE_ADD_TEST(suite, TestSHoundGetFasta);
     SUITE_ADD_TEST(suite, TestSHoundGetFastaList);
     SUITE_ADD_TEST(suite, TestSHoundGiFrom3D);
     SUITE_ADD_TEST(suite, TestSHoundGiFrom3DList);
     SUITE_ADD_TEST(suite, TestSHound3DFromGi);
     SUITE_ADD_TEST(suite, TestSHound3DFromGiList);
     //SUITE_ADD_TEST(suite, TestSHound3DNeighboursFromGiEx);
     //SUITE_ADD_TEST(suite, TestSHoundCDDScoreFromGi);
     return suite;
   } 
   void TestSHoundGetNameByGi (CuTest *tc)
   {
     if (retStr != NULL) MemFree(retStr);
     retStr = SHoundGetNameByGi(gi);
     CuAssertPtrNotNull (tc, retStr);
   }
   void TestSHoundFindAcc(CuTest *tc)
   {
     retInt = 0;
     retInt = SHoundFindAcc(accession);
     CuAssertIntEquals(tc, gi, retInt);
   }
   void TestSHoundFindAccList(CuTest *tc)
   {
     if (retValNode != NULL) retValNode=ValNodeFree(retValNode);
     retValNode = SHoundFindAccList(accList);
     CuAssertPtrNotNull(tc, retValNode);
   }
   void TestSHoundAccFromGi(CuTest *tc)
   {
     if (retStr != NULL) MemFree(retStr);
     retStr = SHoundAccFromGi(gi);
     CuAssertPtrNotNull(tc, retStr);
   }
   void TestSHoundAccFromGiList(CuTest *tc)
   {
     if (retValNode != NULL) retValNode=ValNodeFree(retValNode);
     retValNode = SHoundAccFromGiList(giList);
     CuAssertPtrNotNull(tc, retValNode);
   }
   void TestSHoundFindName(CuTest *tc)
   {
     Int4 retGi = 0, expGi=4001823;
     retGi = SHoundFindName(name);
     CuAssertIntEquals(tc, expGi, retGi);
   }
   void TestSHoundFindNameList(CuTest *tc)
   {
     if (retValNode != NULL) retValNode=ValNodeFree(retValNode);
     retValNode = SHoundFindNameList(nameList);
     CuAssertPtrNotNull(tc, retValNode);
   }
   void TestSHoundSeqIdFromGi(CuTest *tc)
   {
     if(retSeqId != NULL) retSeqId = FreeSeqId (retSeqId);
     retSeqId = SHoundSeqIdFromGi(gi);
     CuAssertPtrNotNull(tc, retSeqId);
   }
   void TestSHoundSeqIdFromGiList(CuTest *tc)
   {
     if (retValNode != NULL) retValNode=ValNodeFree(retValNode);
     retValNode = SHoundSeqIdFromGiList(giList);
     CuAssertPtrNotNull(tc, retValNode);
   }
   void TestSHoundIsProtein(CuTest *tc)
   {
     Boolean retVal =0;
     Int4 proGi=10;
     retVal = SHoundIsProtein(proGi);
     CuAssertTrue(tc, retVal);
   }
   void TestSHoundIsNRFirst(CuTest *tc)
   {
     Boolean retVal =0;
     Int4 nrGi = 1172650;
     retVal = SHoundIsNRFirst(nrGi);
     CuAssertTrue(tc, retVal);
   }
   void TestSHoundGetBioseq(CuTest *tc)
   {
     if(retBioseq != NULL) retBioseq = BioseqFree (retBioseq);
     retBioseq = SHoundGetBioseq(gi);
     CuAssertPtrNotNull(tc, retBioseq);
   }
   void TestSHoundGetBioseqList(CuTest *tc)
   {
     ValNodePtr retBioseqList = NULL;
     retBioseqList = SHoundGetBioseqList(giList);
     CuAssertPtrNotNull(tc, retBioseqList);
   }
   void TestSHound3DExists(CuTest *tc)
   {
     Boolean retVal =0;
     retVal = SHound3DExists(mmdbId);
     CuAssertTrue(tc, retVal);
   }
   void TestSHound3DbWhat(CuTest *tc)
   {
     Int4 retExp =154, retAct=0;
     retAct = SHound3DbWhat(mmdbId);
     CuAssertIntEquals(tc, retExp, retAct);
   }
   void TestSHoundGet3D(CuTest *tc)
   {
     if(retBiostruc != NULL) retBiostruc = BiostrucFree(retBiostruc);
     retBiostruc = SHoundGet3D(mmdbId);
     CuAssertPtrNotNull(tc, retBiostruc);
   }
   void TestSHoundGetFasta(CuTest *tc)
   {
     if(retSLRIFasta != NULL) 
     {
       free(retSLRIFasta);
       retSLRIFasta = NULL;
     }
     retSLRIFasta = SHoundGetFasta(gi);
     CuAssertPtrNotNull(tc, retSLRIFasta);
   }
   void TestSHoundGetFastaList(CuTest *tc)
   {
     ValNodePtr retVal = NULL;
     retVal = SHoundGetFastaList(giList);
     CuAssertPtrNotNull(tc, retVal);
   }
   void TestSHoundGiFrom3D(CuTest *tc)
   {
     ValNodePtr retVal = NULL;
     retVal = SHoundGiFrom3D(mmdbId, FALSE);
     CuAssertPtrNotNull(tc, retVal);
   }
   void TestSHoundGiFrom3DList(CuTest *tc)
   {
     ValNodePtr retVal = NULL;
     retVal = SHoundGiFrom3DList(mmdbList, FALSE);
     CuAssertPtrNotNull(tc, retVal);
   }
   void TestSHound3DFromGi(CuTest *tc)
   {
     Int4 retVal = 0, expVal = 2964, inGi = 230779;
     retVal = SHound3DFromGi(inGi);
     CuAssertIntEquals(tc, expVal, retVal);
   }
   void TestSHound3DFromGiList(CuTest *tc)
   {
     ValNodePtr retVal = NULL, inGiList= NULL;
     ValNodeAddInt(&inGiList, 0, 230779);
     ValNodeAddInt(&inGiList, 0, 230779);
     ValNodeAddInt(&inGiList, 0, 230779);
     retVal = SHound3DFromGiList(inGiList);
     CuAssertPtrNotNull(tc, retVal);
   }
   void TestSHound3DNeighboursFromGiEx(CuTest *tc)
   {
     Int2 retVal = SHound3DNeighboursFromGiEx(gi, &retILinkSet, eValue);
     CuAssertIntEquals(tc, SLRI_SUCCESS, retVal);
   }
   void TestSHoundCDDScoreFromGi(CuTest *tc)
   {
     Int4 giValue = 5032101;
     Int2 retInt = 1;
     FloatHi retVal = SHoundCDDScoreFromGi(giValue, cddid);
     if(retVal > 0)
     {
       retInt = 0;
     }
     else
     {
       retInt = 1;
     }
     CuAssertIntEquals(tc, SLRI_SUCCESS, retInt);
   }
   void SetUpInputList()
   {
     ValNodeAddInt(&giList, 0, gi);
     ValNodeAddInt(&giList, 0, gi);
     ValNodeAddInt(&giList, 0, gi);
     ValNodeAddStr(&accList, 0, StrSave(accession));
     ValNodeAddStr(&accList, 0, StrSave(accession));
     ValNodeAddStr(&accList, 0, StrSave(accession));
     ValNodeAddStr(&cddList, 0, StrSave(cddid));
     ValNodeAddStr(&cddList, 0, StrSave(cddid));
     ValNodeAddStr(&cddList, 0, StrSave(cddid));
     ValNodeAddStr(&nameList, 0, StrSave(name));
     ValNodeAddStr(&nameList, 0, StrSave(name));
     ValNodeAddStr(&nameList, 0, StrSave(name));
     ValNodeAddInt(&taxidList, 0, taxId);
     ValNodeAddInt(&taxidList, 0, taxId);
     ValNodeAddInt(&taxidList, 0, taxId);
     ValNodeAddInt(&mmdbList, 0, mmdbId);
     ValNodeAddInt(&mmdbList, 0, mmdbId);
     ValNodeAddInt(&mmdbList, 0, mmdbId);
     ValNodeAddInt(&chromList, 0, chromId);
     ValNodeAddInt(&chromList, 0, chromId);
     ValNodeAddInt(&chromList, 0, chromId);
     ValNodeAddInt(&omimList, 0, omimId);
     ValNodeAddInt(&omimList, 0, omimId);
     ValNodeAddInt(&omimList, 0, omimId);
     ValNodeAddInt(&llidList, 0, llId);
     ValNodeAddInt(&llidList, 0, llId);
     ValNodeAddInt(&llidList, 0, llId);
   }
