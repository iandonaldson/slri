/* $Id: Seqhound.hpp,v 1.12 2005/01/21 21:47:48 vgu Exp $ */
/***************************************************************************
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


  PROGRAM: SLRI C++ Seqhound remote functions.


  AUTHORS: Hao Lieu (lieu@mshri.on.ca)


  REVISION/CONTRIBUTION NOTES:

  March 1, 2001  Original MSH Revision.
                Hogue Lab - University of Toronto Biochemistry
                Department and the Samuel Lunenfeld Research
                Institute, Mount Sinai Hospital
                http://bioinfo.mshri.on.ca  hogue@mshri.on.ca
                ftp://ftp.mshri.on.ca/


  PUBLICATION TO CITE:

***************************************************************************
*/

#ifndef SEQHOUND_HPP
#define SEQHOUND_HPP

#include <slri_misc.hpp>
#include <corelib/ncbireg.hpp>
#include <connect/ncbi_conn_stream.hpp>
#include <nblast.hpp>
#include <slristruc.hpp>



/*RY: type definition for API list functions*/
typedef list<int> TIList;
typedef list<string> TSList;
typedef map<int, list<int> > TIMap;


BEGIN_SCOPE(seqhound)

USING_NCBI_SCOPE;
using namespace objects;
using namespace slri;

class SeqHound {

public:

  SeqHound();
  int ParseSeqHoundError(const string& pline);
  int SHoundWWWGetFile(string host, string url, string& SHoundReply);

  bool SHoundIsServerAlive(string configFileKey, string fPath);
  bool SHoundInit(bool NetEntrezOnToo, string appname);
  bool SHoundIsInited(void);
  bool SHoundIsNetEntrezOn(void);
  bool SHoundNetEntrezInit(const string appname);
  void SHoundFini(void);
  string SHoundGetCurrentServer(void);
  int SHoundFindAcc(string account);
  list<int> *SHoundFindAccList(list<string>& stlAcc);
  int SHoundFindName(string name); 
  list<int> *SHoundFindNameList(list<string>& stlAcc);
  string SHoundAccFromGi(int Gi);
  list<string> *SHoundAccFromGiList(list<int>& stlGi);
  CSLRISeq_id *SHoundSeqIdFromGi(int Gi);
  CSLRIValNode *SHoundSeqIdFromGiList(list<int>& stlGi);
  string SHoundGetDefline(int Gi);
  list<string> *SHoundGetDeflineList(list<int>& stlGi);
  string SHoundMoleculeType(int Gi);
  bool SHoundIsProtein(int Gi);
  bool SHoundIsNRFirst(int Gi);
  bool SHoundExists(int Gi);
  int SHound3DbWhat(int mmdbid);
  int SHoundGetLargestMMDBID(void);
  bool SHound3DExists(int mmdbid);
  CBioseq *SHoundGetBioseq(int Gi);
  CSLRIValNode *SHoundGetBioseqList(list<int>& sltGi);
  CBioseq_set *SHoundGetBioseqSet(int Gi);
  CSLRIValNode *SHoundGetBioseqSetList(list<int>& stlGi);
  CSeq_entry *SHoundGetSeqEntry(int Gi);
  CSLRIValNode *SHoundGetSeqEntryList(list<int>& stlGi);
  CSLRIFasta *SHoundGetFasta(int Gi);
  CSLRIFasta *SHoundGetFastaFromRedundantGroupID(int groupid);
  CSLRIValNode *SHoundGetFastaList(list<int>& stlGi);
  bool SHoundGetGenBankff(int Gi, ofstream& pfile);
  bool SHoundGetGenBankffList(list<int>& stlGi, ofstream& pfile);
  bool SHoundGetXMLBioseq(int Gi, string file_name);
  bool SHoundGetXMLSeqEntry(int Gi, string file_name);
  CBiostruc *SHoundGet3D(int mmdbid);
  CBiostruc *SHoundGetFixed3D(int mmdbid,int mdlLvl, int maxModels);
  CBiostruc *SHoundGet3DfromPdbId(string pdbid);
  bool SHoundGetXML3D(int mmdbid, string file);
  bool SHoundGetPDB3D(int mmdbid, string& file);
  list<int> *SHoundGiFrom3D(int mmdbid, bool ProteinOnly);
  list<int> *SHoundGiFrom3DList(list<int>& stlMmdbid, bool ProteinOnly);
  int SHoundGiFromPDBchain(string pdbcode, string chain);
  int SHound3DFromGi(int Gi);
  list<int> *SHound3DFromGiList(list<int>& stlGi);
  int SHoundDNAFromProtein(int Gi);
  list<int> *SHoundDNAFromProteinList(list<int>& stlGi);
  list<int> *SHoundProteinFromDNAList(list<int>& stlGi);
  list<int> *SHoundProteinFromDNA(int Gi);
  list<int> *SHoundMuidFromGi(int Gi);
  list<int> *SHoundGetReferenceIDFromGi(int Gi, int reftype);
  list<int> *SHoundGetReferenceIDFromGiList(list<int>& stlGi, int reftype);
  list<int> *SHoundMuidFromGiList(list<int>& stlGi);
  list<int> *SHoundGiFromReferenceID(int refid, int reftype);
  list<int> *SHoundGiFromReferenceList(list<int>& stlRef, int reftype);
  list<int> *SHoundMuidFrom3D(int mmdbid);
  list<int> *SHoundMuidFrom3DList(list<int>& stlmmdbid);
  int SHoundTaxIDFromGi(int Gi);
  list<int> *SHoundTaxIDFromGiList(list<int>& stlGi);
  list<int> *SHoundTaxIDFrom3D(int mmdbid);
  list<int> *SHoundTaxIDFrom3DList(list<int>& stlmmdb);
  list<int> *SHoundProteinsFromTaxID(int taxid);
  list<int> *SHoundProteinsFromTaxIDIII(int taxid);
  list<int> *SHoundProteinsFromTaxIDList(list<int>& stltaxid);
  list<int> *SHoundDNAFromTaxID(int taxid);
  list<int> *SHoundDNAFromTaxIDIII(int taxid);
  list<int> *SHoundDNAFromTaxIDList(list<int>& stltaxid);
  list<int> *SHoundProteinsFromOrganism(int taxid, unsigned char chromflags);
  list<int> *SHoundProteinsFromOrganismList(list<int>& stltaxid, unsigned char chromflags);
  list<int> *SHoundProteinsFromChromosome(int chromid);
  list<int> *SHoundProteinsFromChromosomeList(list<int>& stlchrom);
  list<int> *SHoundAllGenomes(void);
  list<int> *SHoundChromosomeFromGenome(int taxid, unsigned char chromflags); 
  list<int> *SHoundChromosomeFromGenomeList(list<int>& stlGenome, unsigned char chromflags);
  list<int> *SHoundDNAFromOrganism(int taxid, unsigned char chromflags); 
  list<int> *SHoundDNAFromOrganismList(list<int>& sltTax, unsigned char chromflags);
  list<int> *SHoundDNAFromChromosome(int chromid);
  list<int> *SHoundDNAFromChromosomeList(list<int>& stlchrom);
  list<int> *SHoundRedundantGroup(int Gi);
  list<int> *SHoundRedundantGroupList(list<int>& stlGi);
  int SHoundRedundantGroupIDFromGI(int Gi);
  list<int> *SHoundRedundantGroupIDFromGIList(list<int>& stlGi);
  list<int> *SHoundRedundantGroupFromID(int group);
  list<int> *SHoundRedundantGroupFromIDList(list<int>& stlgroup);
  int SHoundFirstOfRedundantGroupFromID(int group);
  list<int> *SHoundFirstOfRedundantGroupFromIDList(list<int>& stlgroup);
  list<int> *SHoundGetTaxChildNodes(int taxid);
  list<int> *SHoundGetTaxChildNodesList(list<int>& sltTaxid);
  list<int> *SHoundGetAllTaxProgeny(int taxid);
  int SHoundGetTaxParent(int taxid);
  int SHoundSequenceLength(int gi);
  list<int> *SHoundGetAllTaxAncestors(int taxid);
  bool SHoundIsTaxMerged(int taxid);
  bool SHoundIsTaxDeleted(int taxid);
  string SHoundGetTaxNameFromTaxID(int taxid);
  string SHoundGetTaxNameFromTaxIDByClass(int taxid, unsigned short nameClass);
  string SHoundGetTaxLineageFromTaxID(int taxid, int type);
  /* not tested  */
  int SHoundGetBlastResult(int gi1, int gi2, CNBlast_Result_Set& pResult);

  /* not tested */
  int SHoundGetBlastSeqAlign(int gi1, int gi2, CSeq_align& psap);
  int SHoundNeighboursFromGiEx(int gi, CFLink_set& pNeighbours, float evalue, bool bLimit);
  int SHoundNeighboursFromGi(int gi, CFLink_set& pNeighbours, float evalue, bool bLimit);
  int SHoundNeighboursFromGiList(list<int>& stlGi, CFLink_set_List2& pNeighbours, float evalue);
  int SHoundNeighboursFromTaxID(int taxid, CFLink_set_List2& pNeighboursList, float evalue);
  int SHoundNeighboursOfNeighbours(int Gi, CFLink_set_List2& pNeighboursList, float evalue);
  int SHoundNeighboursOfNeighboursList(list<int>& stlGi, CFLink_set_List3&, float evalue);
  int SHoundNeighboursOfNeighboursFromTaxID(int taxid, CFLink_set_List3&, float evalue);
  int SHound3DNeighboursFromGiEx(int gi, CILink_set& p3DNeighbours, float evalue);
  int SHound3DNeighboursFromGi(int gi, CILink_set& p3DNeighbours, float evalue);
  int SHound3DNeighboursFromGiList(list<int> stlGi, CILink_set_List2& pNeighbours, float evalue);
  int SHound3DNeighboursFromTaxID(int taxid, CILink_set_List2& p3DNeighbours, float evalue);
  int SHound3DListFromTaxID(int taxid, CILink_set& p3DNeighbours);
  int SHoundGiAndNumNeighboursList(CILink_set& pilsp);
  int SHoundNumNeighboursInDB(int& pnum);
  list<int> *SHoundGOIDFromLLID(int llid);
  int SHoundOMIMFromLLID(int llid);
  list<string> *SHoundCDDIDFromLLID(int llid);
  list<int> *SHoundLLIDFromGOIDAndECode(int goid, string ecode);
  list<int> *SHoundLLIDFromOMIM(int omimid);
  list<int> *SHoundLLIDFromCDDID(string stlcdd);
  list<int> *SHoundGiFromGOID(int GOID);
  list<int> *SHoundGiFromGOIDList(list<int>& stlGo);
  list<int> *SHoundGiFromGOIDAndECode(int goid, string ecode);
  list<int> *SHoundGiFromGOIDListAndECode(list<int>& stlGo, string ecode); 
  list<int> *SHoundGOIDFromGi(int Gi);
  list<int> *SHoundGOIDFromGiList(list<int>& stlGi);
  list<int> *SHoundGOIDFromRedundantGi(int Gi);
  list<int> *SHoundGOIDFromRedundantGiList(list<int>& stlGi);
  list<string> *SHoundGOECodeFromGiAndGOID(int Gi, int goid);
  list<int>  *SHoundGOPMIDFromGiAndGOID(int Gi, int goid);
  list<int> *SHoundGiFromOMIM(int omimid);
  list<int> *SHoundGiFromOMIMList(list<int>& stlomim);
  list<int> *SHoundOMIMFromGi(int Gi);
  list<int> *SHoundOMIMFromGiList(list<int>& stlGi);
  list<int> *SHoundGiFromCDDID(string& cddid);
  list<int> *SHoundGiFromCDDIDList(list<string>& stlcdd);
  list<string> *SHoundCDDIDFromGi(int Gi);
  list<string> *SHoundCDDIDFromGiList(list<int>& stlGi);
  float SHoundCDDScoreFromGi(int Gi, string cddid);
  list<int> *SHoundGiFromLLID(int llid);
  list<int> *SHoundGiFromLLIDList(list<int>& stllid);
  int SHoundLLIDFromGi(int Gi);
  list<int> *SHoundLLIDFromGiList(list<int>& stlGi);
  string SHoundLocusFromGi(int Gi);
  list<int> *SHoundGODBGetRecordByReference(string refType);
  list<int> *SHoundGODBGetChildrenOf(int goid);
  list<int> *SHoundGODBGetParentOf(int goid);
  list<int> *SHoundGODBGetAllAncestors(int goid);
  list<int> *SHoundGODBGetAllChildren(int goid);
  string SHoundGODBGetNameByID(int goid);
  string SHoundGODBGetClassification(int goid);
  int SHoundAllGenomesWithScientificNames(CCLink_set& pAllGenomes);
  int SHoundAllGenomesWithNumProteins(CILink_set& pAllGenomes);
  list<int> *SHoundGetGisByNumberOfDomains(int numdom);
  list<string> *SHoundGetDomainIdFromLabel(string label);
  list<int> *SHoundGetGisByDomainId(string domain);
  string SHoundGetDomainLabelFromDomainId(string domain);
  list<int> *SHoundGetGisByDomainIdAndEvalue(string domain, float cutoff);
  CStRpsdb *SHoundGetDomainsFromGi(int Gi);
  CStRpsdb *SHoundGetDomainsFromGiWithEvalue(int Gi, float cutoff);
  string SHoundGetAccFromRetiredGi(int Gi);
  
  /*RY API list functions*/
  map<int, list<int>* > *SHoundRedundantGroupKeyedList(list<int>& stlGi);
  map<int, CSLRIFasta*> *SHoundGetFastaKeyedList(list<int>& stlGi);
  map<int, CSLRIFasta*> *SHoundGetFastaFromRedundantGroupIDKeyedList(list<int>& stlGroupID); 
#define CHROM_PHAGE 0x40
#define CHROM_NR    0x20
#define CHROM_ECE   0x10
#define CHROM_PLMD  0x08
#define CHROM_CHLO  0x04
#define CHROM_MITO  0x02
#define CHROM_CHROM 0x01
#define CHROM_ALL   0xFF

#define SLRI_taxon_name_name_class_none 0
#define SLRI_taxon_name_name_class_acronym 1
#define SLRI_taxon_name_name_class_anamorph 2
#define SLRI_taxon_name_name_class_blast_name 3
#define SLRI_taxon_name_name_class_common_name 4
#define SLRI_taxon_name_name_class_equivalent_name 5
#define SLRI_taxon_name_name_class_includes 6
#define SLRI_taxon_name_name_class_in_part 7
#define SLRI_taxon_name_name_class_misnomer 8
#define SLRI_taxon_name_name_class_misspelling 9
#define SLRI_taxon_name_name_class_preferred_acronym 10
#define SLRI_taxon_name_name_class_preferred_common_name 11
#define SLRI_taxon_name_name_class_scientific_name 12
#define SLRI_taxon_name_name_class_synonym 13
#define SLRI_taxon_name_name_class_teleomorph 14
#define SLRI_taxon_name_name_class_other 255

#define SHoundGetTaxLineage_FULL 10
#define SHoundGetTaxLineage_GENBANK 20
#define SHoundGetTaxLineage_TAXDBSHORT 30

private:
  /* private macros */
  
  /* Timeout settings for network connection */
#define DEFAULTTO 180
#define USERTO 120

  /* Error values */
#define WWWGETFILE_ERROR 0
#define SEQHOUND_BUFFER_ERROR -4
#define SEQHOUND_ERROR -1
#define SEQHOUND_NULL -2
#define SEQHOUND_OK 1
#define SEQHOUND_ABNORMAL -3

#define MAXQ 700

  bool isInited;
  int m_error;
  string slri_sername;
  string slri_cgipath;
  string SHoundReply;
  char buffer[80];

  /* Overriding default of 30 secs */
  int gsec;
  int gUsersec;

  /* private functions */
  void SetError(int ERR);
  bool IsErrorSet(void);
  void ErrorReset(void);
  int GetError(void);
  void* pvoid;

  /* Allow the user to override default of 30 */
  void SHoundSetTimeOut(int sec);

  /* Override the userdefault */
  void SHoundUserSetTimeOut(int sec);
  int SHoundUserGetTimeout(void);
  int SHoundGetTimeOut();

  /*template < class T >
  void MaxSizeList(list <T>& stList, list<T>& stlRet){
    int count = 0;
    while(!stList.empty()){
      stlRet.push_back( stList.front() );
      stList.pop_front();
      count++;
      if( count == MAXQ ) break;
    }
    return;
    }*/

  template < class List >
  void MaxSizeList(List& stlList, List& stlRet, typename List::iterator& i){

    for( int count = 0; i != stlList.end(); ){
      stlRet.push_back( *i );
      count++; i++;
      if( count == MAXQ ) break;
    }
    
  }


};

END_SCOPE(seqhound)

#endif

/* 
$Log: Seqhound.hpp,v $
Revision 1.12  2005/01/21 21:47:48  vgu
code change related to bug 4254: change default time from 120 to 180 seconds to process large mmdb objects

Revision 1.11  2004/11/22 14:57:00  mmatan

Modified shoundgetfixed3d so that is takes the same parameters as shoundget3dex.
See bug 3930 for details.

Revision 1.10  2004/11/18 20:15:56  hfeldman
added new f'n header

Revision 1.9  2004/06/15 18:16:11  ryao
Removed unused ProcessRetStr functions; Amend last version: alsochanged prototype of SHoundRedundantGroupKeyedList

Revision 1.8  2004/06/15 18:03:14  ryao
Added six new locus link functions and SHoundGetFastaFromRedundantGroupID(KeyedList).

Revision 1.7  2004/04/05 18:30:46  ryao
Added SHoundGetFastaKeyedList SHoundRedundantGroupKeyedList

Revision 1.6  2004/01/12 22:19:24  ryao
added SHoundGetLargestMMDBID

Revision 1.5  2003/11/28 16:30:30  ryao
Added SHoundGetReferenceIDFromGiList SHoundGiFromReferenceList SHoundGiFromGOIDListAndECode

Revision 1.4  2003/11/07 21:04:59  ryao
added SHoundGetAccFromRetiredGi SHoundGiFromGOIDAndECode

Revision 1.3  2003/11/06 22:50:22  ryao
added SHoundGetReferenceIDFromGi SHoundGiFromReferenceID

Revision 1.28  2003/08/14 19:13:06  haocl
added SHoundSequenceLength

Revision 1.27  2003/03/06 20:04:28  haocl
added license, fixed #ifndef directives

Revision 1.26  2003/02/27 22:13:44  micheld
Modified project files for combined asn files
changed ERROR to private member m_error, and functions that use it - incompatibility with windows defined ERROR

Revision 1.25  2003/02/06 15:46:16  haocl
new nblast and slristruc files and modifications to reflect new files

Revision 1.23  2002/10/18 20:31:36  haocl
changed syntax of namespace declaration

Revision 1.22  2002/10/17 19:46:46  haocl
changed seqhound scoping to work with MSCV++

Revision 1.21  2002/10/11 19:19:16  haocl
fixed MaxSizeList to work in MSC++ also

Revision 1.20  2002/10/07 20:18:10  haocl
initial import of readme for c++ seqhound

Revision 1.19  2002/09/26 21:27:56  haocl
added 2 include statements

Revision 1.18  2002/08/22 21:16:52  haocl
removed comments so that MAXQ is set to 700

Revision 1.17  2002/07/16 16:38:14  haocl
changed MaxSizeList to work with iterators to minimize list copying

Revision 1.16  2002/07/15 19:43:50  haocl
changed methods that return objects to object pointers

Revision 1.15  2002/07/11 20:42:46  haocl
changed interface of SHoundGetPDB3D

Revision 1.14  2002/07/11 19:21:59  haocl
fixed ProteinFromDNA to return a list

Revision 1.13  2002/07/10 19:37:45  haocl
rename GetDomainLabelFromAccession and GetDomainAccessionFromLabel

Revision 1.12  2002/07/10 16:18:17  haocl
added constructor and isInited functionality

Revision 1.11  2002/07/09 21:55:41  haocl
changed the linkset type in couple functions

Revision 1.10  2002/07/08 17:16:55  haocl
changes to reflect namespace seqhound

Revision 1.8  2002/07/05 18:19:22  haocl
added Sehound to library

Revision 1.7  2002/07/05 14:17:03  haocl
fixed functions some functions to return CSLRIValNode strucs

Revision 1.6  2002/07/04 21:52:25  haocl
removed extraneous libraries

Revision 1.5  2002/07/04 19:54:46  haocl
got ride of excessive include statements in slri_misc.hpp

Revision 1.4  2002/07/04 14:44:27  haocl
fixed a small warning in NBlast and small changes in Seqhound

Revision 1.3  2002/07/03 21:54:11  haocl
moved MaxSizeList here

Revision 1.2  2002/07/03 21:18:34  haocl
changes to reflect the inclusion of slri_misc class

Revision 1.1  2002/07/02 21:42:35  haocl
import slristruc and nblast lib and seqhound include files

Revision 1.8  2002/06/28 22:06:43  haocl
converted all auxiliary files into libraries, made corresponding changes in Seqhound and makefile

Revision 1.7  2002/06/28 21:05:41  haocl
all ASN functions working

Revision 1.6  2002/06/28 19:45:16  haocl
more ASN functionality added

Revision 1.5  2002/06/25 21:54:06  haocl
fixed more ASN.1 stuff

Revision 1.4  2002/06/25 16:09:00  haocl
added Id and Log tags

*/
