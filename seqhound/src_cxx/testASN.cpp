/* $Id: testASN.cpp,v 1.1.1.2 2003/03/07 09:33:10 root Exp $ */ 
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


#include "testASN.hpp"
#include <Seqhound.hpp>
#include <slri_misc.hpp>

USING_NCBI_SCOPE;
using namespace objects;
using namespace slri;
using namespace seqhound;

int testASN::Run(){

  SeqHound shound;

  /* ASN STUFF */              

  ofstream out("testASN_out.txt");
  shound.SHoundInit(true, "testASN");

  list<int> stlMe;
  stlMe.push_back(442547);

  list<int> stlGi;
  stlGi.push_back(2392601);
  stlGi.push_back(442545);
  stlGi.push_back(442547);

  list<int> stlTest;
  stlTest.push_back(84);
  stlTest.push_back(93);


  out << "SHoundAllGenomesWithScientificNames: written to scientific.txt" << endl;
  CCLink_set scientific;
  shound.SHoundAllGenomesWithScientificNames(scientific);
  auto_ptr<CObjectOStream>
    science_out(CObjectOStream::Open("scientific.txt", eSerial_AsnText));
  *science_out << scientific;

  out << "SHoundAllGenomesWithNumProteins: written to proteins.txt" << endl;
  CILink_set proteins;
  shound.SHoundAllGenomesWithNumProteins(proteins);
  auto_ptr<CObjectOStream>
    proteins_out(CObjectOStream::Open("proteins.txt", eSerial_AsnText));
  *proteins_out << proteins;

  /*NcbiCout << "SHoundGetBlastSeqAlign: " << flush;
  CSeq_align *sa = shound.SHoundGetBlastSeqAlign(95900, 95894, sa);
  NcbiCout << endl;*/

  out << "SHoundGetBioseq: written to getbioseq.txt" << endl;
  CBioseq *seq = shound.SHoundGetBioseq(93);
  auto_ptr<CObjectOStream>
    seq_out(CObjectOStream::Open("getbioseq.txt", eSerial_AsnText));
  *seq_out << *seq;
  delete seq;

  out << "SHoundNeighboursFromGiEx: written to nfromgiex.txt" << flush;
  CFLink_set fs;
  shound.SHoundNeighboursFromGiEx(2392601, fs, 1, true);
  out << endl;
  auto_ptr<CObjectOStream>
    txt_out(CObjectOStream::Open("nfromgiex.txt", eSerial_AsnText));
  *txt_out << fs;

  out << "SHoundNeighboursFromGi: written to nfromgi.txt" << endl;
  CFLink_set nfgi;
  shound.SHoundNeighboursFromGi(2392601, nfgi, 1, true);
  auto_ptr<CObjectOStream>
    txt_out2(CObjectOStream::Open("nfromgi.txt", eSerial_AsnText));
  *txt_out2 << nfgi; 

  list<int> stlGi2;
  stlGi2.push_back(2392601);
  stlGi2.push_back(442545);

  out << "SHoundNeighboursFromGiList: written to nfromgilist.txt" << endl;
  CFLink_set_List2 fs2;
  shound.SHoundNeighboursFromGiList(stlGi2, fs2, 1);
  string nfromgilist = "nfromgilist.txt";
  auto_ptr<CObjectOStream>
    txt_out3(CObjectOStream::Open(nfromgilist, eSerial_AsnText));
  *txt_out3 << fs2;

  out << "SHoundNeighboursFromTaxID: written to nfromtaxid.txt" << endl;
  CFLink_set_List2 fs3;
  shound.SHoundNeighboursFromTaxID(56636, fs3, 1);
  string nfromtaxid = "nfromtaxid.txt";
  auto_ptr<CObjectOStream>
    txt_out4(CObjectOStream::Open(nfromtaxid, eSerial_AsnText));
  *txt_out4 << fs3;

  out << "SHoundNeighboursOfNeighbours: written to nofn.txt" << endl;
  CFLink_set_List2 fs4;
  string nofn = "nofn.txt";
  shound.SHoundNeighboursOfNeighbours(2392601, fs4, 1.0);
  auto_ptr<CObjectOStream>
    txt5(CObjectOStream::Open(nofn, eSerial_AsnText));
  *txt5 << fs4;

  out << "SHoundNeighboursOfNeighboursList: written to nofnlist.txt" << endl;
  CFLink_set_List3 lclink;
  shound.SHoundNeighboursOfNeighboursList(stlMe, lclink, 1.0);
  auto_ptr<CObjectOStream>
    txt_out6(CObjectOStream::Open("nofnlist.txt", eSerial_AsnText));
  *txt_out6 << lclink;
 
  out << "SHound3DNeighboursFromGiList: written to 3dngilist.txt" << endl;
  CILink_set_List2 cilist;
  shound.SHound3DNeighboursFromGiList(stlGi, cilist, 1.0);
  auto_ptr<CObjectOStream>
    txt_out7(CObjectOStream::Open("3dngilist.txt", eSerial_AsnText));
  *txt_out7 << cilist;

  CILink_set_List2 cilist2;
  
  out << "SHound3DNeighboursFromTaxID: written to 3dfromtaxid.txt" << endl;
  shound.SHound3DNeighboursFromTaxID(63363, cilist2, 1.0);
  auto_ptr<CObjectOStream>
    d3fromtx(CObjectOStream::Open("3dfromtaxid.txt", eSerial_AsnText));
  *d3fromtx << cilist2;


  out << "SHound3DListFromTaxID: written to 3dlisttaxid.txt" << endl;
  CILink_set ci;
  shound.SHound3DListFromTaxID(63363, ci);
  auto_ptr<CObjectOStream>
    txt_out8(CObjectOStream::Open("3dlisttaxid.txt", eSerial_AsnText));
  *txt_out8 <<ci;

  out << "SHoundGiAndNumNeighboursList: written to ginum.txt" << endl;
  CILink_set ci2;
  shound.SHoundGiAndNumNeighboursList(ci2);
  string file2 = "ginum.txt";
  auto_ptr<CObjectOStream>
    txt(CObjectOStream::Open(file2, eSerial_AsnText));
  *txt << ci2;

  out << "SHoundGetFasta: written to fasta.xml" << endl;
  CSLRIFasta *bs = shound.SHoundGetFasta(82);
  auto_ptr<CObjectOStream>
    outting(CObjectOStream::Open("fasta.xml", eSerial_Xml));
  *outting << *bs;

  out << "SHoundGetBioseqList: written to getbioseqlist.txt" << endl;
  CSLRIValNode *stlBio = shound.SHoundGetBioseqList(stlTest);
  auto_ptr<CObjectOStream>
    bioseq(CObjectOStream::Open("getbioseqlist.txt", eSerial_AsnText));
  *bioseq << *stlBio;
  
  out << "SHoundGetFastaList: written getfasta.txt" << endl;
  CSLRIValNode *stlFasta = shound.SHoundGetFastaList(stlTest);
  auto_ptr<CObjectOStream>
    txt_out9(CObjectOStream::Open("getfasta.txt", eSerial_AsnText));
  *txt_out9 << *stlFasta;

  list<int> stlFun;
  stlFun.push_back(876787);
  stlFun.push_back(876787);

  CSLRIValNode *stlSeqid = shound.SHoundSeqIdFromGiList(stlFun);
  out << "SHoundSeqIdFromGiList: written to seqidfromgilist.txt" << endl;
  auto_ptr<CObjectOStream>
    seqlist(CObjectOStream::Open("seqidfromgilist.txt", eSerial_AsnText));
  *seqlist << *stlSeqid;

  out << "SHoundGetBioseqSetList: written to seqset.txt" << endl; 
  CSLRIValNode *stlBioseqSet = shound.SHoundGetBioseqSetList(stlTest);
  auto_ptr<CObjectOStream>
    getbioseq(CObjectOStream::Open("getbioseqset.txt", eSerial_AsnText));
  *getbioseq << *stlBioseqSet;

  out << "SHoundGetSeqEntryList: written to getseqentry.txt" << endl;
  CSLRIValNode *stlseqEntryList = shound.SHoundGetSeqEntryList(stlTest);
  auto_ptr<CObjectOStream>
    getSeqEntry(CObjectOStream::Open("getseqentry.txt", eSerial_AsnText));
  *getSeqEntry << *stlseqEntryList;

  out << "SHoundNeighboursFromGi: writen to nfromgi.txt" << endl;
  CFLink_set rs;
  shound.SHoundNeighboursFromGi(7228451, rs, 1.0, true);
  auto_ptr<CObjectOStream>
    nfromgi(CObjectOStream::Open("nfromgi.txt", eSerial_AsnText));
  *nfromgi << rs;

  CFLink_set rs2;
  out << "SHoundNeighboursFromGiEx: written to nfromgiEx.txt" << endl;
  shound.SHoundNeighboursFromGiEx(94342, rs2, 1.0, false);
  auto_ptr<CObjectOStream>
    nfromgiEx(CObjectOStream::Open("nfromgiEx.txt", eSerial_AsnText));
  *nfromgiEx << rs2;

  out << "SHoundGet3D: written to get3d.txt" << endl;
  CBiostruc *bs4 = shound.SHoundGet3D(54);
  auto_ptr<CObjectOStream>
    get3d(CObjectOStream::Open("get3d.txt", eSerial_AsnText));
  *get3d << *bs4;
  
  out << "SHoundGetXML3D: written to getxml.xml" << endl;
  shound.SHoundGetXML3D(54, "getxml.xml");
  
  out << "SHoundSeqIdFromGi: written to seqidfromgi.txt" << endl;
  CSLRISeq_id *squid = shound.SHoundSeqIdFromGi(876787);
  auto_ptr<CObjectOStream>
    seqid_out(CObjectOStream::Open("seqidfromgi.txt", eSerial_AsnText));
  *seqid_out << *squid;

  out << "SHoundGetBioseqSet: written to getbioseqset.txt" << endl;
  CBioseq_set *seqset = shound.SHoundGetBioseqSet(93);
  auto_ptr<CObjectOStream>
    seq_set(CObjectOStream::Open("getbioseqset.txt", eSerial_AsnText));
  *seq_set << *seqset;
  delete seqset;

  out << "SHoundGetSeqEntry: written to getseqentry.txt." << endl;
  CSeq_entry *seqentry = shound.SHoundGetSeqEntry(93);
  auto_ptr<CObjectOStream>
    seq_entry(CObjectOStream::Open("getseqentry.txt", eSerial_AsnText));
  *seq_entry << *seqentry;
  delete seqentry;

  out << "SHoundGetXMLBioseq: written to getxmlbioseq.xml" << endl;
  shound.SHoundGetXMLBioseq(93, "getxmlbioseq.xml");
  out << "SHoundGetXMLSeqEntry: written to getxmlseqentry.xml" << endl;
  shound.SHoundGetXMLSeqEntry(93, "getxmlseqentry.xml");
  
  out << "SHoundGet3DfromPdbId: written to 3dfrompdbid.txt" << endl;
  CBiostruc *bs2 = shound.SHoundGet3DfromPdbId("1GIX");
  auto_ptr<CObjectOStream>
    bio_out(CObjectOStream::Open("3dfrompdbid.txt", eSerial_AsnText));
  *bio_out << *bs2;
  delete bs2;

  out << "SHoundGetDomainsFromGi: written to getdomainsfromgi.txt" << endl;
  CStRpsdb *rpsdb = shound.SHoundGetDomainsFromGi(4557225);
  auto_ptr<CObjectOStream>
    rps(CObjectOStream::Open("getdomainsfromgi.txt", eSerial_AsnText));
  *rps << *rpsdb;

  out << "SHoundGetDomainFromGiWithEvalue: written to getdomainsfromgiwithevalue.txt" << endl;
  rpsdb = shound.SHoundGetDomainsFromGiWithEvalue(4557225, 1);
  auto_ptr<CObjectOStream>
    rps2(CObjectOStream::Open("getdomainsfromgiwithevalue.txt", eSerial_AsnText));
  *rps2 << *rpsdb;

  return 0; 
} 

int main(int argc, const char* argv[]){

  testASN theTestApp;
  return theTestApp.AppMain(argc, argv, 0, eDS_Default, 0, "testASN");
}


/* 
 * $Log: testASN.cpp,v $
 * Revision 1.1.1.2  2003/03/07 09:33:10  root
 * Automated import from Sourceforge.net
 *
 * Revision 1.2  2003/03/06 20:18:06  haocl
 * added license
 *
 *
 *
 */
