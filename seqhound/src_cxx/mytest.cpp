/* $Id: mytest.cpp,v 1.1.1.8 2003/05/24 08:36:15 root Exp $ */ 
/***************************************************************************
  Copyright (C) 2001 Mount Sinai Hospital (MSH)
    
  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public License
  as published by the Free Software Foundation; either version 2.1 of
  the License, or any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

  See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with this program; if not, write to the
         Free Software Foundation, Inc.,
         59 Temple Place, Suite 330, Boston, MA
         02111-1307  USA
  or visit http://www.gnu.org/licenses/lgpl.txt

  SPECIAL EXCEPTIONS

  As a special exception, Mount Sinai Hospital gives permission to
  link this program with the following non-GPL programs or libraries,
  and distribute the resulting executable, without including the source
  code for these in the source distribution:

  a) CodeBase 6.5 or greater from Sequiter Software Inc.

  b) The C or C++ interface to Oracle 8.x or greater, from
     Oracle Corporation or IBM DB2 UDB.


  PROGRAM: SLRI C++ Seqhound test functions.


  AUTHORS: Hao Lieu


  REVISION/CONTRIBUTION NOTES:

  September 1, 2002  Original MSH Revision.
                Hogue Lab - University of Toronto Biochemistry
                Department and the Samuel Lunenfeld Research
                Institute, Mount Sinai Hospital
                http://bioinfo.mshri.on.ca  hogue@mshri.on.ca
                ftp://ftp.mshri.on.ca/


  PUBLICATION TO CITE:

***********************************************************************
*/
#include "mytest.hpp"
#include <Seqhound.hpp>
#include <slri_misc.hpp>

USING_NCBI_SCOPE;
using namespace objects;
using namespace slri;
using namespace seqhound;

int Mytest::Run() {

  ofstream out("sample_test.txt2");

  list<int> stlOMIM;
  stlOMIM.push_back(137150);
  stlOMIM.push_back(103950);
  stlOMIM.push_back(108345);

  list<int> stlGi;
  stlGi.push_back(2392601);
  stlGi.push_back(442545);
  stlGi.push_back(442547);

  list<int> stlmuid;
  stlmuid.push_back(87226186);
  stlmuid.push_back(93287299);
  stlmuid.push_back(89306659);
  
  list<int> stl3D;
  stl3D.push_back(11);
  stl3D.push_back(13);
  stl3D.push_back(12);

  list<int> stlpro;
  stlpro.push_back(94);
  stlpro.push_back(99);

  list<int> stldna;
  stldna.push_back(93);
  stldna.push_back(94);

  list<int> stlNeig;
  stlNeig.push_back(94342);
  stlNeig.push_back(7228451);
  stlNeig.push_back(98764);

  list<int> stlTest;
  stlTest.push_back(84);
  stlTest.push_back(93);

  list<int> stlLLID;
  stlLLID.push_back(2);

  list<int> stlMe;
  stlMe.push_back(442547);

  list<string> stlName;
  stlName.push_back("BtMIsATn");
  stlName.push_back("ECJ224437");
  stlName.push_back("AF165032");

  list<int> stlGOID;
  stlGOID.push_back(5737);
  stlGOID.push_back(8033);
  stlGOID.push_back(4867);
  stlGOID.push_back(6419);

  list<int> stlGI;
  stlGI.push_back(4557225);
  stlGI.push_back(4559587);

  list<string> stlCDDID;
  stlCDDID.push_back("pfam00202");
  stlCDDID.push_back("pfam01835");
  stlCDDID.push_back("pfam00079");

  list<int> stlChrom;
  stlChrom.push_back(139);
  stlChrom.push_back(160);

  list<int> stlID;
  stlID.push_back(34);
  stlID.push_back(35);

  list <int> stlMmdbid;
  stlMmdbid.push_back(11);
  stlMmdbid.push_back(12);
  stlMmdbid.push_back(13);

  list<string> pValue;
  pValue.push_back("P00953");
  pValue.push_back("P00951");
  pValue.push_back("P00952");

  list<int> stlTax;
  stlTax.push_back(56636);

  SeqHound shound;

  out << "SHoundInit: " << shound.SHoundInit(true, "test") << endl;
  /* out << "SHoundIsNetEntrezOn: " << shound.SHoundIsNetEntrezOn() << endl;
  out << "SHoundNetEntrezInit: " << shound.SHoundNetEntrezInit("test") << endl;

  out << "SHoundGetCurrentServer: " << shound.SHoundGetCurrentServer() << endl;
  out << "SHoundFindAcc: " << shound.SHoundFindAcc("P00951") << endl;
  out << "SHoundAccFromGi: " << shound.SHoundAccFromGi(938754) << endl;
  
  out << "SHoundGetDefline: " << shound.SHoundGetDefline(93) << endl;
  out << "SHoundIsProtein: " << shound.SHoundIsProtein(93) << endl;
  out << "SHoundIsNRFirst: " << shound.SHoundIsNRFirst(93) << endl;
  */
  /* out << "SHoundGetPDB3D: written to pdb3d.txt" << endl;
  string pdb = "pdb3d.txt";
  shound.SHoundGetPDB3D(11, pdb);
  


  list<int> *stlRAcc1 = shound.SHoundFindAccList(pValue);
  out << "SHoundFindAccList: " << flush;
  SLRIMisc::PrintList(stlRAcc1, out);
  out << "done." <<endl;

  out << "SHoundFindName: " << shound.SHoundFindName("BtMIsATn") << endl;

  list<int> *stlRGi = shound.SHoundFindNameList(stlName);
  out << "SHoundFindNameList: " << flush;
  SLRIMisc::PrintList(stlRGi, out);
  out << "done" << endl;
  
  out << "SHoundAccFromGiList: " << flush;
  list<string> *stlRAcc2 = shound.SHoundAccFromGiList(stlGi);
  SLRIMisc::PrintList(stlRAcc2, out);
  out << endl;

  out << "SHoundGetDefline: " << shound.SHoundGetDefline(74539) << endl;

  out << "SHoundGetDeflineList: " << flush;
  list<string> *stlRDef = shound.SHoundGetDeflineList(stlGi);
  SLRIMisc::PrintList(stlRDef, out);
  out << endl;

  out << "SHound3DbWhat: " << shound.SHound3DbWhat(93) << endl;

  string file = "GetGenBankff.txt";
  ofstream ofile(file.c_str());
  out << "SHoundGetGenBankff: written to GetGenBankff.txt" << flush;
  shound.SHoundGetGenBankff(555, ofile);
  ofile << endl;
  shound.SHoundGetGenBankff(3319926, ofile);
  ofile << endl;
  shound.SHoundGetGenBankff(6906990, ofile);
  ofile << endl;
  out << endl;

  out << "SHoundGetGenBankffList: written to GetGenBankff.txt" << flush;
  shound.SHoundGetGenBankffList(stlGi, ofile);
  ofile << endl;
  out << endl;
  
  out << "SHoundGiFrom3D: " << flush;
  stlRGi = shound.SHoundGiFrom3D(11, false);
  SLRIMisc::PrintList(stlRGi, out);
  out << endl;

  out << "SHoundGiFrom3DList: " << flush;
  stlRGi = shound.SHoundGiFrom3DList(stlMmdbid, false);
  SLRIMisc::PrintList(stlRGi, out);
  out << endl;
  
  out << "SHoundGiFromPDBchain: " << shound.SHoundGiFromPDBchain("1VII", "") << endl;
  out << "SHound3DFromGi: " << shound.SHound3DFromGi(442545) << endl;

  out << "SHound3DFromGiList: " << flush;
  list<int> *stlR3D = shound.SHound3DFromGiList(stlGi);
  SLRIMisc::PrintList(stlR3D, out);
  out << endl;

  out << "SHoundDNAFromProtein: " << shound.SHoundDNAFromProtein(94) << endl;

  out << "SHoundDNAFromProteinList: " << flush;
  list<int> *stlRdna = shound.SHoundDNAFromProteinList(stlpro);
  SLRIMisc::PrintList(stlRdna, out);
  out << endl;

  out << "SHoundProteinFromDNA: " << flush;
  stlRdna = shound.SHoundProteinFromDNA(93);
  SLRIMisc::PrintList(stlRdna, out);
  out << endl;
  
  out << "SHoundProteinFromDNAList: " << flush;
  list<int> *stlRpro = shound.SHoundProteinFromDNAList(stldna);
  SLRIMisc::PrintList(stlRpro, out);
  out << endl;

  list<int> *stlRMu = shound.SHoundMuidFromGi(93);
  out << "SHoundMuidFromGi: " << flush;
  SLRIMisc::PrintList(stlRMu, out);
  out << endl;

  out << "SHoundMuidFromGiList: " << flush;
  stlRMu = shound.SHoundMuidFromGiList(stlGi);
  SLRIMisc::PrintList(stlRMu, out);
  out << endl;

  out << "SHoundMuidFrom3D: " << flush;
  stlRMu = shound.SHoundMuidFrom3D(13);
  SLRIMisc::PrintList(stlRMu, out);
  out << endl;

  out << "SHoundMuidFrom3DList: " << flush;
  stlRMu = shound.SHoundMuidFrom3DList(stl3D);
  SLRIMisc::PrintList(stlRMu, out);
  out << endl;

  out << "SHoundTaxIDFromGi: " << shound.SHoundTaxIDFromGi(93) << endl;

  out << "SHoundTaxIDFromGiList: " << flush;
  list<int> *stlRTax = shound.SHoundTaxIDFromGiList(stlGi);
  SLRIMisc::PrintList(stlRTax, out);
  out << endl;


    
  out << "SHoundExists: " << shound.SHoundExists(93) << endl;
  out << "SHound3DExists: " << shound.SHound3DExists(93) << endl;
  out << "SHound3DbWhat: " << shound.SHound3DbWhat(93) << endl;
   
  list<int> *stlRTaxid = shound.SHoundTaxIDFrom3D(11);
  out << "SHoundTaxIDFrom3D: " << flush;
  SLRIMisc::PrintList(stlRTaxid, out);
  out << endl;

  stlRTaxid = shound.SHoundTaxIDFrom3DList(stlMmdbid);
  out << "SHoundTaxIDFrom3DList: " << flush;
  SLRIMisc::PrintList(stlRTaxid, out);
  out << endl;

  out << "SHoundProteinsFromTaxID: " << flush;
  list<int> *stlRpro2 = shound.SHoundProteinsFromTaxID(56636);
  SLRIMisc::PrintList(stlRpro2, out); out << endl;

  out << "SHoundProteinsFromTaxIDIII: " << flush;
  stlRpro2 = shound.SHoundProteinsFromTaxIDIII(56636);
  SLRIMisc::PrintList(stlRpro2, out); out << endl;

  out << "SHoundProteinsFromTaxIDList: " << flush;
  stlRpro2 = shound.SHoundProteinsFromTaxIDList(stlTax);
  SLRIMisc::PrintList(stlRpro2, out); out << endl;

  list<int> *stlRDNA = shound.SHoundDNAFromTaxID(56636);
  out << "SHoundDNAFromTaxID: " << flush;
  SLRIMisc::PrintList(stlRDNA, out);out<< endl;

  stlRDNA = shound.SHoundDNAFromTaxIDIII(56636);
  out << "SHoundDNAFromTaxIDIII: " << flush;
  SLRIMisc::PrintList(stlRDNA, out); out<< endl;

  stlRDNA = shound.SHoundDNAFromTaxIDList(stlTax);
  out << "SHoundDNAFromTaxIDList: " << flush;
  SLRIMisc::PrintList(stlRDNA, out); out<< endl;


  list<int> *stlRPRO = shound.SHoundProteinsFromChromosome(139);
  out << "SHoundProteinsFromChromosome: " << flush;
  SLRIMisc::PrintList(stlRPRO, out); out << endl;

  stlRPRO = shound.SHoundProteinsFromChromosomeList(stlChrom);
  out << "SHoundProteinsFromChromosomeList: " << flush;
  SLRIMisc::PrintList(stlRPRO, out); out << endl;
  */
  list<int> *stlRPRO = shound.SHoundAllGenomes();
  out << "SHoundAllGenomes: " << flush;
  SLRIMisc::PrintList(stlRPRO, out);
  out << endl;

  out << "SHoundProteinsFromOrganismList(all Genomes): " << flush;
  list<int> *stlRpro = shound.SHoundProteinsFromOrganismList(*stlRPRO, CHROM_ALL);
  SLRIMisc::PrintList(stlRpro, out);
  out << endl;

  out << "SHoundProteinsFromOrganism:(S. cere). " <<flush;
  stlRpro = shound.SHoundProteinsFromOrganism(4932, CHROM_ALL);
  SLRIMisc::PrintList(stlRpro, out);
  out << endl;
  
  out << "SHoundProteinsFromOrganism:(E. coli). " <<flush;
  stlRpro = shound.SHoundProteinsFromOrganism(562, CHROM_ALL);
  SLRIMisc::PrintList(stlRpro, out);
  out << endl;
  /* 
  list<int> *stlRChrom = shound.SHoundChromosomeFromGenome(56636, CHROM_ALL);
  out << "SHoundChromosomeFromGenome: " << flush;
  SLRIMisc::PrintList(stlRChrom, out); out << endl;

  stlRChrom = shound.SHoundChromosomeFromGenomeList(stlTax, CHROM_ALL);
  out << "SHoundChromosomeFromGenomeList: " << flush;
  SLRIMisc::PrintList(stlRChrom, out); out << endl;

  stlRChrom = shound.SHoundDNAFromOrganism(56636, CHROM_ALL);
  out << "SHoundDNAFromOrganism: " << flush;
  SLRIMisc::PrintList(stlRChrom, out); out << endl;

  stlRChrom = shound.SHoundDNAFromOrganismList(stlTax, CHROM_ALL);
  out << "SHoundDNAFromOrganismList: " << flush;
  SLRIMisc::PrintList(stlRChrom, out); out << endl;

  stlRChrom = shound.SHoundRedundantGroup(56336);
  out << "SHoundRedundantGroup: " << flush;
  SLRIMisc::PrintList(stlRChrom, out); out << endl;

  stlRChrom = shound.SHoundRedundantGroupList(stlGi);
  out << "SHoundRedundantGroupList: " << flush;
  SLRIMisc::PrintList(stlRChrom, out); out<< endl;

  out << "SHoundRedundantGroupIDFromGI: " << shound.SHoundRedundantGroupIDFromGI(2392601) << endl;
  
  stlRChrom = shound.SHoundRedundantGroupIDFromGIList(stlGi);
  out << "SHoundRedundantGroupIDFromGIList: " << flush;
  SLRIMisc::PrintList(stlRChrom, out); out<< endl;

  stlRChrom = shound.SHoundRedundantGroupFromID(34);
  out << "SHoundRedundantGroupFromID: " << flush;
  SLRIMisc::PrintList(stlRChrom, out); out<< endl;

  stlRChrom = shound.SHoundRedundantGroupFromIDList(stlID);
  out << "SHoundRedundantGroupFromIDList: " << flush;
  SLRIMisc::PrintList(stlRChrom, out);
  out<< endl;

  out << "SHoundFirstOfRedundantGroupFromID: " << shound.SHoundFirstOfRedundantGroupFromID(34) << endl;

  out << "SHoundFirstOfRedundantGroupFromIDList: " << flush;
  stlRChrom = shound.SHoundFirstOfRedundantGroupFromIDList(stlID);
  SLRIMisc::PrintList(stlRChrom, out); out << endl;

  stlRChrom = shound.SHoundGetTaxChildNodes(56635);
  out << "SHoundGetTaxChildNodes: " << flush;
  SLRIMisc::PrintList(stlRChrom, out); out<< endl;

  stlTax.push_back(56635);
  stlRChrom = shound.SHoundGetTaxChildNodesList(stlTax);
  out << "SHoundGetTaxChildNodesList: " << flush;
  SLRIMisc::PrintList(stlRChrom, out); out<< endl;

  stlRChrom = shound.SHoundGetAllTaxProgeny(56635);
  out << "SHoundGetAllTaxProgeny: " << flush;
  SLRIMisc::PrintList(stlRChrom, out); out<< endl;

  out << "SHoundGetTaxParent: " << shound.SHoundGetTaxParent(56636) << endl;

  stlRChrom = shound.SHoundGetAllTaxAncestors(56636);
  out<< "SHoundGetAllTaxAncestors: " << flush;
  SLRIMisc::PrintList(stlRChrom, out); out<< endl;
  
  out << "SHoundIsTaxMerged: " << shound.SHoundIsTaxMerged(56635) << endl;
  out << "SHoundIsTaxDeleted: " << shound.SHoundIsTaxDeleted(56635) << endl;
  out << "SHoundGetTaxNameFromTaxIDByClass: " << shound.SHoundGetTaxNameFromTaxIDByClass(56636, 12) << endl; */
	  
  out << "SHoundGetTaxNameFromTaxID: " << shound.SHoundGetTaxNameFromTaxID(212717) << endl;
 
  /* out << "SHoundGetTaxLineageFromTaxID: " << shound.SHoundGetTaxLineageFromTaxID(56636, 10) << endl;

  out << "SHoundNumNeighboursInDB: " << flush;
  int num = 0;
  shound.SHoundNumNeighboursInDB(num);
  out << num << endl;

  out << "SHoundGiFromGOID: " << flush;
  list<int> *stlRGO = shound.SHoundGiFromGOID(5737);
  SLRIMisc::PrintList(stlRGO, out); out << endl;

  out << "SHoundGiFromGOIDList: " << flush;
  stlRGO = shound.SHoundGiFromGOIDList(stlGOID);
  SLRIMisc::PrintList(stlRGO, out); out << endl;

  stlRGO = shound.SHoundGOIDFromGi(4557225);
  out << "SHoundGOIDFromGi: " << flush;
  SLRIMisc::PrintList(stlRGO, out); out << endl;

  stlRGO = shound.SHoundGOIDFromGiList(stlGI);
  out << "SHoundGOIDFromGiList: " << flush;
  SLRIMisc::PrintList(stlRGO, out); out << endl;

  stlRGO = shound.SHoundGOIDFromRedundantGi(4557225);
  out << "SHoundGOIDFromRedundantGi: " << flush;
  SLRIMisc::PrintList(stlRGO, out); out << endl;

  stlRGO = shound.SHoundGOIDFromRedundantGiList(stlGI);
  out << "SHoundGOIDFromRedundantGiList: "<< flush;
  SLRIMisc::PrintList(stlRGO, out); out << endl;

  list<string> *stlRECode = shound.SHoundGOECodeFromGi(4501841);
  out << "ShoundGOECodeFromGi: " << flush;
  SLRIMisc::PrintList(stlRECode, out); out << endl;

  list<int> *stlRPMID = shound.SHoundGOPMIDFromGi(4501841);
  out << "SHoundGOPMIDFromGi: " << flush;
  SLRIMisc::PrintList(stlRPMID, out); out << endl;

  stlRPMID = shound.SHoundGiFromOMIM(103950);
  out << "SHoundGiFromOMIM: " << flush;
  SLRIMisc::PrintList(stlRPMID, out); out << endl;
    
  stlRPMID = shound.SHoundGiFromOMIMList(stlOMIM);
  out << "SHoundGiFromOMIMList: " << flush;
  SLRIMisc::PrintList(stlRPMID, out); out << endl;

  list<int> *stlROMIM = shound.SHoundOMIMFromGi(4501841);
  out << "SHoundOMIMFromGi: " << flush;
  SLRIMisc::PrintList(stlROMIM, out); out << endl;

  stlROMIM = shound.SHoundOMIMFromGiList(stlGI);
  out << "SHoundOMIMFromGiList: " << flush;
  SLRIMisc::PrintList(stlROMIM, out); out << endl;

  string pfam202 = "pfam00202";
  stlROMIM = shound.SHoundGiFromCDDID(pfam202);
  out << "SHoundGiFromCDDID: " << flush;
  SLRIMisc::PrintList(stlROMIM, out); out << endl;

  stlROMIM = shound.SHoundGiFromCDDIDList(stlCDDID);
  out << "SHoundGiFromCDDIDList: " << flush;
  SLRIMisc::PrintList(stlROMIM, out); out << endl;

  list<string> *stlRCDDID = shound.SHoundCDDIDFromGi(4557225);
  out << "SHoundCDDIDFromGi: " << flush;
  SLRIMisc::PrintList(stlRCDDID, out); out << endl;

  stlRCDDID = shound.SHoundCDDIDFromGiList(stlGI);
  out << "SHoundCDDIDFromGiList: " << flush;
  SLRIMisc::PrintList(stlRCDDID, out); out << endl;

  string s2 = "EPEND";
  out << "SHoundCDDScoreFromGi: " << shound.SHoundCDDScoreFromGi(4557225, s2) << endl;

  out << "SHoundGiFromLLID: " << flush;
  list<int> *stlRGI2 = shound.SHoundGiFromLLID(2);
  SLRIMisc::PrintList(stlRGI2, out); out << endl;

  //list<int> stlRGI2;
  out << "SHoundGiFromLLID: " << flush;
  stlRGI2 = shound.SHoundGiFromLLIDList(stlLLID);
  SLRIMisc::PrintList(stlRGI2, out); out << endl;

  out << "SHoundLLIDFromGi: " << shound.SHoundLLIDFromGi(4557225) << endl;
  
  out << "SHoundLLIDFromGiList: " << flush;
  stlRGI2 = shound.SHoundLLIDFromGiList(stlGI);
  SLRIMisc::PrintList(stlRGI2, out); out<< endl;
  
  out << "SHoundLocusFromGi: " << shound.SHoundLocusFromGi(4557225) << endl;
  string ref = "EC";
  stlRGI2 = shound.SHoundGODBGetRecordByReference(ref);
  out << "SHoundGODBGetRecordByReference: " << flush;
  SLRIMisc::PrintList(stlRGI2, out); out << endl;

  stlRGI2 = shound.SHoundGODBGetChildrenOf(6754);
  out << "SHoundGODBGetChildrenOf: " << flush;
  SLRIMisc::PrintList(stlRGI2, out); out << endl;

  stlRGI2 = shound.SHoundGODBGetParentOf(6754);
  out << "SHoundGODBGetParentOf: " << flush;
  SLRIMisc::PrintList(stlRGI2, out); out << endl;

  stlRGI2 = shound.SHoundGODBGetAllAncestors(6754);
  out << "SHoundGODBGetAllAncestors: " << flush;
  SLRIMisc::PrintList(stlRGI2, out); out << endl;

  stlRGI2 = shound.SHoundGODBGetAllChildren(6754);
  out << "SHoundGODBGetAllChildren: " << flush;
  SLRIMisc::PrintList(stlRGI2, out); out << endl;

  out << "SHoundGODBGetNameByID: " << shound.SHoundGODBGetNameByID(6754) << endl;
  out << "SHoundGODBGetClassification: " << shound.SHoundGODBGetClassification(6754) << endl;
  
  out << "SHoundGetGisByNumberOfDomains: " << flush;
  stlRGI2 = shound.SHoundGetGisByNumberOfDomains(2);
  SLRIMisc::PrintList(stlRGI2, out); out << endl;

  out << "SHoundGetDomainIdFromLabel: " << flush;
  string st = "SH3";
  list<string> *stlRStr = shound.SHoundGetDomainIdFromLabel(st);
  SLRIMisc::PrintList(stlRStr, out); out << endl;

  out << "SHoundGetGisByDomainId: " << flush;
  string st2 = "smart00038";
  stlRGI2 = shound.SHoundGetGisByDomainId(st2);
  SLRIMisc::PrintList(stlRGI2, out); out << endl;

  out << "SHoundGetDomainLabelFromDomainId: " << shound.SHoundGetDomainLabelFromDomainId(st2) << endl;
  out << "SHoundGetGisByDomainIdAndEvalue: " << flush;
  stlRGI2 = shound.SHoundGetGisByDomainIdAndEvalue(st2, 1.0);
  SLRIMisc::PrintList(stlRGI2, out); out << endl;

  out << "SHoundDNAFromChromosome: " << flush;
  list <int> *stlint = shound.SHoundDNAFromChromosome(13578);
  SLRIMisc::PrintList(stlint, out); out << endl;

  list<int> stlInt;
  stlInt.push_back(13578);

  out << "SHoundDNAFromChromosomeList: " << flush;
  stlint = shound.SHoundDNAFromChromosomeList(stlInt);
  SLRIMisc::PrintList(stlint, out); out << endl;
  */

  return 0; 
} 

int main(int argc, const char* argv[]){

  Mytest theTestApp;
  return theTestApp.AppMain(argc, argv, 0, eDS_Default, 0, "mytest");
}

/* 
 * $Log: mytest.cpp,v $
 * Revision 1.1.1.8  2003/05/24 08:36:15  root
 * Automated import from Sourceforge.net
 *
 * Revision 1.12  2003/05/23 20:38:54  haocl
 * changed config file to be Windows compatible
 *
 * Revision 1.11  2002/10/02 19:48:05  haocl
 * added GNU Lesser GPL
 *
 */
