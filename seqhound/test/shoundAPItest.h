#ifndef _API_TEST_H_
#define _API_TEST_H_

#include "CuTest.h"

void SetUpInputList ();
void TestSHoundGetNameByGi (CuTest *tc);
CuSuite* ShoundTestGetSuite ();
void TestSHoundFindAcc(CuTest *tc);
void TestSHoundFindAccList(CuTest *tc);
void TestSHoundAccFromGi(CuTest *tc);
void TestSHoundAccFromGiList(CuTest *tc);
void TestSHoundFindName(CuTest *tc);
void TestSHoundFindNameList(CuTest *tc);
void TestSHoundSeqIdFromGi(CuTest *tc);
void TestSHoundSeqIdFromGiList(CuTest *tc);
void TestSHoundIsProtein(CuTest *tc);
void TestSHoundIsNRFirst(CuTest *tc);
void TestSHoundGetBioseq(CuTest *tc);
void TestSHoundGetBioseqList(CuTest *tc);
void TestSHound3DExists(CuTest *tc);
void TestSHound3DbWhat(CuTest *tc);
void TestSHoundGet3D(CuTest *tc);
void TestSHoundGetFasta(CuTest *tc);
void TestSHoundGetFastaList(CuTest *tc);
void TestSHoundGiFrom3D(CuTest *tc);
void TestSHoundGiFrom3DList(CuTest *tc);
void TestSHound3DFromGi(CuTest *tc);
void TestSHound3DFromGiList(CuTest *tc);
void TestSHound3DNeighboursFromGiEx(CuTest *tc);
void TestSHoundCDDScoreFromGi(CuTest *tc);

#endif
