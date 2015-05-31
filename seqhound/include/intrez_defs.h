/* $Id: intrez_defs.h,v 1.6 2004/08/12 17:55:28 zwang Exp $
*******************************************************************************************
  Copyright (C) 2000 Mount Sinai Hospital (MSH)

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
           Katerina Michalickova and Christopher W.V. Hogue hogue@mshri.on.ca


  REVISION/CONTRIBUTION NOTES:

  June 1, 2000  Original MSH Revision.
                Hogue Lab - University of Toronto Biochemistry
                Department and the Samuel Lunenfeld Research
                Institute, Mount Sinai Hospital
                http://bioinfo.mshri.on.ca  hogue@mshri.on.ca
                ftp://ftp.mshri.on.ca/


  PUBLICATION TO CITE:


  DESCRIPTION
  November 1998
  header file for SeqHound database parsers and cgi apps,
  and API

******************************************************************************************/

#ifndef _INTREZ_DEFS_
#define _INTREZ_DEFS_


#ifdef __cplusplus
extern "C" { /* } */
#endif

#include <bzlib.h>
#include <d4all.h>
#include <asn.h>
#include <ncbi.h>
#include <objsset.h>
#ifdef ODBC
#include <asn2gnbk.h>
#else
#include <asn2gnbk.h>
/* include <asn2ff.h> */
#endif
#include <mmdbapi.h>
#include <objmmdb3.h>
#include <vastlocl.h>
#include <objentr.h>
#include <mmdblocl.h>
#include <mmdbdata.h>
#include <tofasta.h>
#include <objslristruc.h>
#include <blast.h>
#include <blastdef.h>
#include <utilpub.h>
#include <taxdb_db.h>
#include <objslritax.h>
#include <slri_asn.h>
#include <slri_cb.h>
#include <slri_misc.h>
#include <slri_www.h>



/*
#define CHROM      0x80 */
#define CHROM_PHAGE 0x40
#define CHROM_NR    0x20
#define CHROM_ECE   0x10
#define CHROM_PLMD  0x08
#define CHROM_CHLO  0x04
#define CHROM_MITO  0x02
#define CHROM_CHROM 0x01
#define CHROM_ALL   0xFF

/*
#define REP            0x80
#define REP            0x40
#define REP            0x20
#define REP            0x10 */
#define REP_NONREDUND  0x08
#define REP_MIN_EIGHTY 0x04
#define REP_MIN_FORTY  0x02
#define REP_MIN_SEVEN  0x01
#define REP_ALL        0x0F



/* redefine Intrez functions to SHound equivalents to compile and link remote code to local API*/
#define SHoundInit			IntrezInit
#define SHoundIsNetEntrezOn		IntrezIsNetEntrezOn
#define SHoundNetEntrezInit		IntrezNetEntrezInit
#define SHoundFini			IntrezFini
#define SHoundIsInited			IntrezIsInited
#define SHoundFindSeqId			IntrezFindSeqId
#define SHoundFindSeqIdList		IntrezFindSeqIdList
#define SHoundFindAcc			IntrezFindAcc
#define SHoundFindAccList		IntrezFindAccList
#define SHoundAccFromGi			IntrezAccFromGi
#define SHoundAccFromGiList		IntrezAccFromGiList
#define SHoundTitleFromGi		IntrezTitleFromGi
#define SHoundTitleFromGiList		IntrezTitleFromGiList
#define SHoundSeqIdFromGi		IntrezSeqIdFromGi
#define SHoundSeqIdFromGiList		IntrezSeqIdFromGiList
#define SHoundIsProtein			IntrezIsProtein
#define SHoundIsNRFirst			IntrezIsNRFirst
#define SHoundExists			IntrezExists
#define SHound3DbWhat			Intrez3DbWhat
#define SHound3DExists			Intrez3DExists
#define SHoundGetBioseq			IntrezGetBioseq
#define SHoundGetBioseqList		IntrezGetBioseqList
#define SHoundGetBioseqSet		IntrezGetBioseqSet
#define SHoundGetBioseqSetList		IntrezGetBioseqSetList
#define SHoundGetSeqEntry		IntrezGetSeqEntry
#define SHoundGetSeqEntryList		IntrezGetSeqEntryList
#define SHoundGetFasta			IntrezGetFasta
#define SHoundGetFastaList		IntrezGetFastaList
#define SHoundGetGenBankff		IntrezGetGenBankff
#define SHoundGetGenBankffList		IntrezGetGenBankffList
#define SHoundGetXMLBioseq		IntrezGetXMLBioseq
#define SHoundGetXMLSeqEntry	IntrezGetXMLSeqEntry
#define SHoundGetXML3D	IntrezGetXML3D
#define SHoundGiFrom3D			IntrezGiFrom3D
#define SHoundGiFrom3DList		IntrezGiFrom3DList
#define SHound3DFromGi			Intrez3DFromGi
#define SHound3DFromGiList		Intrez3DFromGiList
#define SHoundDNAFromProtein		IntrezDNAFromProtein
#define SHoundDNAFromProteinList	IntrezDNAFromProteinList
#define SHoundProteinFromDNA		IntrezProteinFromDNA
#define SHoundProteinFromDNAList	IntrezProteinFromDNAList
#define SHoundMuidFromGi		IntrezMuidFromGi
#define SHoundMuidFromGiList		IntrezMuidFromGiList
#define SHoundMuidFrom3D		IntrezMuidFrom3D
#define SHoundMuidFrom3DList		IntrezMuidFrom3DList
#define SHoundTaxIDFromGi		IntrezTaxIDFromGi
#define SHoundTaxIDFromGiList		IntrezTaxIDFromGiList
#define SHoundTaxIDFrom3D		IntrezTaxIDFrom3D
#define SHoundTaxIDFrom3DList		IntrezTaxIDFrom3DList
#define SHoundProteinsFromTaxID		IntrezProteinsFromTaxID
#define SHoundProteinsFromTaxIDII	IntrezProteinsFromTaxIDII
#define SHoundProteinsFromTaxIDList	IntrezProteinsFromTaxIDList
#define SHoundDNAFromTaxID		IntrezDNAFromTaxID
#define SHoundDNAFromTaxIDII		IntrezDNAFromTaxIDII
#define SHoundDNAFromTaxIDList		IntrezDNAFromTaxIDList
#define SHoundDNAFromTaxIDIII		IntrezDNAFromTaxIDIII
#define SHoundProteinsFromTaxIDIII	IntrezProteinsFromTaxIDIII
#define SHoundRedundantGroup		IntrezRedundantGroup
#define SHoundRedundantGroupList	IntrezRedundantGroupList
#define SHoundRedundantGroupIDFromGI    IntrezRedundantGroupIDFromGI
#define SHoundRedundantGroupIDFromGIList	IntrezRedundantGroupIDFromGIList
#define SHoundRedundantGroupFromID      IntrezRedundantGroupFromID
#define SHoundRedundantGroupFromIDList  IntrezRedundantGroupFromIDList
#define SHoundFirstOfRedundantGroupFromID	IntrezFirstOfRedundantGroupFromID
#define SHoundFirstOfRedundantGroupFromIDList	IntrezFirstOfRedundantGroupFromIDList
#define SHoundProteinsFromOrganism	IntrezProteinsFromOrganism
#define SHoundProteinsFromOrganismList	IntrezProteinsFromOrganismList
#define SHoundProteinsFromChromosome	IntrezProteinsFromChromosome
#define SHoundProteinsFromChromosomeList	IntrezProteinsFromChromosomeList
#define SHoundAllGenomes		IntrezAllGenomes
#define SHoundAllKllodge		IntrezAllKloodge
#define SHoundAllMMDBID			IntrezAllMMDBID
#define SHoundChromosomeFromGenome	IntrezChromosomeFromGenome
#define SHoundChromosomeFromGenomeList	IntrezChromosomeFromGenomeList
#define SHoundDNAFromOrganism		IntrezDNAFromOrganism
#define SHoundDNAFromOrganismList	IntrezDNAFromOrganismList
#define SHoundDNAFromChromosome		IntrezDNAFromChromosome
#define SHoundDNAFromChromosomeList	IntrezDNAFromChromosomeList
#define SHoundGet3D			IntrezGet3D
#define SHoundGet3DfromPdbId		IntrezGet3DfromPdbId
#define SHoundGetPDB3D			IntrezGetPDB3D
#define SHoundGetDomain			IntrezGetDomain
/*March, 2001, Doron Added SHoundGetCDSLRIFasta*/
#define SHoundGetCDSLRIFasta		IntrezGetCDSLRIFasta





/* Intrez databases datastructures */
/*
typedef struct asndb {
    Int4 gi;
    BioseqPtr asn1;
    CharPtr division;
    CharPtr release;
    CharPtr type;
    struct asndb PNTR next;
} StAsndb, PNTR StAsndbPtr;



typedef struct parti {
    Int4 gi;
    CharPtr partition;
    struct parti PNTR next;
} StParti, PNTR StPartiPtr;


typedef struct nucprot {
    Int4 gin;
    Int4 gia;
    struct nucprot PNTR next;
} StNucprot, PNTR StNucprotPtr;


typedef struct accdb {
    Int4 gi;
    CharPtr db;
    CharPtr name;
    CharPtr access;
    CharPtr chain;
    CharPtr release;
    Int2 version;
    CharPtr title;
    CharPtr namelow;
    struct accdb PNTR next;
} StAccdb, PNTR StAccdbPtr;

typedef struct pubseq {
    Int4 gi;
    Int4 muid;
    struct pubseq PNTR next;
} StPubseq, PNTR StPubseqPtr;


typedef struct taxgi {
     Int4 gi;
     Int4 taxid;
     Int4 kloodge;
     CharPtr type;
     struct taxgi PNTR next;
} StTaxgi, PNTR StTaxgiPtr;


typedef struct chrom {
     Int4 taxid;
     Int4 kloodge;
     Int4 chromfl;
     struct chrom PNTR next;
} StChrom, PNTR StChromPtr;


typedef struct sendb {
    Int4 seid;
    SeqEntryPtr asn1;
    struct sendb PNTR next;
} StSendb, PNTR StSendbPtr;


typedef struct sengi {
    Int4 seid;
    Int4 gi;
    CharPtr division;
    struct sengi PNTR next;
} StSengi, PNTR StSengiPtr;


typedef struct mmdb {
    Int4 mmdbid;
    CharPtr pdbid;
    BiostrucPtr asn1;
    Int4 bwhat;
    Int4 models;
    Int4 molecules;
    Int4 size;
    Int4 bzsize;
    struct mmdb PNTR next;
} StMmdb, PNTR StMmdbPtr;


typedef struct mmgi {
    Int4 mmdbid;
    Int4 gi;
    struct mmgi PNTR next;
} StMmgi, PNTR StMmgiPtr;


typedef struct redund {
     Int4 gi;
     Int4 ordinal;
     Int4 group;
     CharPtr type;
     struct redund PNTR next;
} StRedund, PNTR StRedundPtr;


typedef struct domdb {
    Int4 mmdbid;
    SLRIValNodePtr asn1;
    CharPtr pdbid;
    CharPtr chain;
    Int4 gi;
    Int4 domno;
    Int4 domall;
    Int4 domid;
    Int4 rep;
    struct domdb PNTR next;
} StDomdb, PNTR StDomdbPtr;


typedef struct vastdb {
    Int4 mmdbid;
    BiostrucFeaturePtr asn1;
    CharPtr pdbid;
    CharPtr chain;
    Int4 vastid1;
    Int4 vastid2;
    Int4 score;
    Int4 pval;
    Int4 res;
    Int4 ident;
    Int4 rmsd;
    Int4 scale;
    struct vastdb PNTR next;
} StVastdb, PNTR StVastdbPtr;
*/

/* function declarations */

/* CodeBase functions */
Boolean LIBCALL InitCodeBase(void);
Boolean LIBCALL OpenASNDB(CharPtr, Boolean);
Boolean LIBCALL OpenPARTI(CharPtr, Boolean);
Boolean LIBCALL OpenNUCPROT(CharPtr, Boolean);
Boolean LIBCALL OpenACCDB(CharPtr, Boolean);
Boolean LIBCALL OpenPUBSEQ(CharPtr, Boolean);
Boolean LIBCALL OpenTAXGI(CharPtr, Boolean);
Boolean LIBCALL OpenCHROM(CharPtr, Boolean);
Boolean LIBCALL OpenSENDB(CharPtr, Boolean);
Boolean LIBCALL OpenSENGI(CharPtr, Boolean);
Boolean LIBCALL OpenMMDB(CharPtr, Boolean);
Boolean LIBCALL OpenMMGI(CharPtr, Boolean);
Boolean LIBCALL OpenREDUND(CharPtr, Boolean);
Boolean LIBCALL OpenDOMDB(CharPtr name, Boolean Create);
Boolean LIBCALL OpenVASTDB(CharPtr name, Boolean Create);
Boolean LIBCALL OpenNRBIOSEQ(CharPtr name, Boolean Create);
Boolean LIBCALL OpenCDDB(CharPtr name, Boolean Create);/*Doron, March 2001*/
Boolean LIBCALL PrintASNDB(void);
Boolean LIBCALL PrintPARTI(void);
Boolean LIBCALL PrintNUCPROT(void);
Boolean LIBCALL PrintACCDB(void);
Boolean LIBCALL PrintPUBSEQ(void);
Boolean LIBCALL PrintTAXGI(void);
Boolean LIBCALL PrintCHROM(void);
Boolean LIBCALL PrintSENDB(void);
Boolean LIBCALL PrintSENGI(void);
Boolean LIBCALL PrintMMDB(void);
Boolean LIBCALL PrintMMGI(void);
Boolean LIBCALL PrintREDUND(void);
Boolean LIBCALL PrintDOMDB(Byte set);
Boolean LIBCALL PrintVASTDB(void);
Boolean LIBCALL PrintNRBIOSEQ(void);
Boolean LIBCALL PrintCDDB(void); /*Doron, March 2001*/
Boolean LIBCALL AppendRecordASNDB(StAsndbPtr pas);
Boolean LIBCALL AppendRecordPARTI(StPartiPtr ppa);
Boolean LIBCALL AppendRecordNUCPROT(StNucprotPtr pnc);
Boolean LIBCALL AppendRecordACCDB(StAccdbPtr pac);
Boolean LIBCALL AppendRecordPUBSEQ(StPubseqPtr pps, Int2 choice);
Boolean LIBCALL AppendRecordTAXGI(StTaxgiPtr ptg);
Boolean LIBCALL AppendRecordCHROM(StChromPtr pch);
Boolean LIBCALL AppendRecordSENDB(StSendbPtr psd);
Boolean LIBCALL AppendRecordSENGI(StSengiPtr psg);
Boolean LIBCALL AppendRecordMMDB(StMmdbPtr pmd);
Boolean LIBCALL AppendRecordMMGI(StMmgiPtr pmg);
Boolean LIBCALL AppendRecordREDUND(StRedundPtr prd);
Boolean LIBCALL AppendRecordDOMDB(StDomdbPtr pdd);
/*Boolean LIBCALL AppendRecordVASTDB(StVastdbPtr pvd);*/
Boolean LIBCALL EditRecordACCDBByGI(StAccdbPtr pac, Boolean overwrite);
Boolean LIBCALL EditRecordTAXGIByGI(StTaxgiPtr ptg, Boolean overwrite);
Boolean LIBCALL EditRecordDOMDBByDOMID(StDomdbPtr pdd, Boolean overwrite);
Boolean LIBCALL DeleteGIinPARTI(Int4, Boolean);
Boolean LIBCALL DeleteGIinASNDB(CharPtr, Int4, Boolean);
Boolean LIBCALL DeleteGIinACCDB(Int4, Boolean);
Boolean LIBCALL DeleteGIinTAXGI(Int4, Boolean);
Boolean LIBCALL DeleteGIinPUBSEQ(Int4, Boolean);
Boolean LIBCALL DeleteGIinNUCPROT(Int4, Boolean);
Boolean LIBCALL DeleteGIinSENGI(Int4, Boolean);
Boolean LIBCALL DeleteGIinSENDB(CharPtr, Int4, Boolean);
Boolean LIBCALL DeleteGIinREDUND(Int4, Boolean);
Boolean LIBCALL DeleteGIinCDDB( Int4 gi, Boolean destroy); /*March 2001, Doron*/
Boolean LIBCALL CloseASNDB(void);
Boolean LIBCALL ClosePARTI(void);
Boolean LIBCALL CloseNUCPROT(void);
Boolean LIBCALL CloseACCDB(void);
Boolean LIBCALL ClosePUBSEQ(void);
Boolean LIBCALL CloseTAXGI(void);
Boolean LIBCALL CloseCHROM(void);
Boolean LIBCALL CloseSENDB(void);
Boolean LIBCALL CloseSENGI(void);
Boolean LIBCALL CloseMMDB(void);
Boolean LIBCALL CloseMMGI(void);
Boolean LIBCALL CloseREDUND(void);
Boolean LIBCALL CloseDOMDB(void);
Boolean LIBCALL CloseVASTDB(void);
Boolean LIBCALL CloseNRBIOSEQ(void);
Boolean LIBCALL CloseCodeBase(void);
 Boolean LIBCALL CloseCDDB(void);  /*March 2001, Doron*/

/* ASN.1 storage into MMDB using bzip compression */
Boolean AssignASNMemBZMemo(Pointer pASNdata, AsnWriteFunc pAsnWrite);

/* Database search functions */
Int2           LIBCALL SearchASNDB(StAsndbPtr PNTR);
Int2           LIBCALL SearchPARTI (StPartiPtr PNTR);
Int2           LIBCALL SearchNUCPROT (StNucprotPtr PNTR);
Int2           LIBCALL SearchACCDB (StAccdbPtr PNTR);
Int2           LIBCALL SearchPUBSEQ (StPubseqPtr PNTR);
Int2           LIBCALL SearchTAXGI (StTaxgiPtr PNTR);
Int2           LIBCALL SearchCHROM (StChromPtr PNTR);
Int2           LIBCALL SearchSENDB (StSendbPtr PNTR);
Int2           LIBCALL SearchSENGI (StSengiPtr PNTR);
Int2           LIBCALL SearchSENDBforGI(StSendbPtr PNTR, Int4);
Int2           LIBCALL SearchMMDB (StMmdbPtr PNTR);
Int2           LIBCALL SearchMMDBEx (StMmdbPtr PNTR, Int4, Int4);
Int2           LIBCALL SearchMMGI (StMmgiPtr PNTR);
Int2           LIBCALL SearchDOMDB(StDomdbPtr PNTR);
Int2           LIBCALL SearchREDUND (StRedundPtr PNTR);
/*Int2           LIBCALL SearchVASTDB(StVastdbPtr PNTR);*/
BiostrucPtr    LIBCALL GetBiostrucfromMMDB(Int4 mdlLvl, Int4 maxModels);
BioseqPtr      LIBCALL GetBioseqfromASNDB(void);
SeqEntryPtr    LIBCALL GetSeqEntryfromSENDB(void);
CharPtr        LIBCALL GetTitlefromACCDB(void);
SLRIValNodePtr LIBCALL GetDomainfromDOMDB(void);
BiostrucFeaturePtr LIBCALL GetFeaturefromVASTDB(void);
ValNodePtr     LIBCALL IntrezAllMMDBID(void);
ValNodePtr     LIBCALL IntrezAllGenomes(void);
ValNodePtr     LIBCALL IntrezAllKloodge(void);
Int4           LIBCALL SHoundRecNoDOMDB(void);
StDomdbPtr     LIBCALL SHoundGetRecDOMDB(Int4 recno);
/* end of CodeBase related functions */


/* reset the database asntool generated structures */
void LIBCALL ResetStASNDB(StAsndbPtr PNTR ppas);
void LIBCALL ResetStPARTI(StPartiPtr PNTR pppa);
void LIBCALL ResetStNUCPROT(StNucprotPtr PNTR ppnc);
void LIBCALL ResetStACCDB(StAccdbPtr PNTR ppac);
void LIBCALL ResetStPUBSEQ(StPubseqPtr PNTR ppps);
void LIBCALL ResetStTAXGI(StTaxgiPtr PNTR pptg);
void LIBCALL ResetStCHROM(StChromPtr PNTR ppch);
void LIBCALL ResetStSENDB(StSendbPtr PNTR ppsd);
void LIBCALL ResetStSENGI(StSengiPtr PNTR ppsg);
void LIBCALL ResetStMMDB(StMmdbPtr PNTR ppmd);
void LIBCALL ResetStMMGI(StMmgiPtr PNTR ppmg);
void LIBCALL ResetStREDUND(StRedundPtr PNTR pprd);
void LIBCALL ResetStDOMDB(StDomdbPtr PNTR ppdd);
/*void LIBCALL ResetStVASTDB(StVastdbPtr PNTR ppvd);*/

/* miscelaneous */
void        LIBCALL PrintValNodeOfInt(ValNodePtr pvn);
void        LIBCALL PrintValNodeOfStr(ValNodePtr pvn);
Boolean     LIBCALL StrToInt4 (CharPtr str, Int4Ptr longval);
Boolean     LIBCALL IsDivisionEST(SeqDescrPtr);
CharPtr     LIBCALL TrimSpace(CharPtr);   /* cuts off spaces at the end of a string */
CharPtr     LIBCALL PadString(CharPtr);   /* adds one space at the end of a string */
Int4        LIBCALL GetGI(BioseqPtr);
Boolean     LIBCALL ValNodeIsIn(Int4 num, ValNodePtr pvnval);
Int2        LIBCALL Misc_IswwStringTrueOrFalse(CharPtr);
ValNodePtr  LIBCALL ValNodeUnique(ValNodePtr);
CharPtr     LIBCALL ValNodeToCharPtr(ValNodePtr);
/*ValNodePtr  LIBCALL Misc_CommaStrString2ValNodeList(CharPtr, CharPtr);*/
/*CharPtr     LIBCALL Misc_SaveStrValNodeListAsString(ValNodePtr);*/


/* Intrez database structures handlers - obsolete asntool generates these
StAsndbPtr    LIBCALL StAsndbNew (void);
StAsndbPtr    LIBCALL StAsndbFree (StAsndbPtr);
StPartiPtr    LIBCALL StPartiNew (void);
StPartiPtr    LIBCALL StPartiFree (StPartiPtr);
StNucprotPtr  LIBCALL StNucprotNew (void);
StNucprotPtr  LIBCALL StNucprotFree (StNucprotPtr);
StAccdbPtr    LIBCALL StAccdbNew (void);
StAccdbPtr    LIBCALL StAccdbFree (StAccdbPtr);
StPubseqPtr   LIBCALL StPubseqNew (void);
StPubseqPtr   LIBCALL StPubseqFree (StPubseqPtr);
StTaxgiPtr    LIBCALL StTaxgiNew (void);
StTaxgiPtr    LIBCALL StTaxgiFree (StTaxgiPtr);
StChromPtr    LIBCALL StChromNew (void);
StChromPtr    LIBCALL StChromFree (StChromPtr);
StSendbPtr    LIBCALL StSendbNew (void);
StSendbPtr    LIBCALL StSendbFree (StSendbPtr);
StSengiPtr    LIBCALL StSengiNew (void);
StSengiPtr    LIBCALL StSengiFree (StSengiPtr);
StMmdbPtr     LIBCALL StMmdbNew (void);
StMmdbPtr     LIBCALL StMmdbFree (StMmdbPtr);
StMmgiPtr     LIBCALL StMmgiNew (void);
StMmgiPtr     LIBCALL StMmgiFree (StMmgiPtr);
StDomdbPtr    LIBCALL StDomdbNew(void);
StDomdbPtr    LIBCALL StDomdbFree(StDomdbPtr);
StRedundPtr   LIBCALL StRedundNew (void);
StRedundPtr   LIBCALL StRedundFree (StRedundPtr);
StVastdbPtr   LIBCALL StVastdbNew(void);
StVastdbPtr   LIBCALL StVastdbFree(StVastdbPtr);
*/


/* Intrez opening, testing and closing */
Boolean  LIBCALL IntrezInit(Boolean, CharPtr);
Boolean  LIBCALL IntrezInitII(Boolean, CharPtr, Boolean);
Boolean  LIBCALL IntrezIsNetEntrezOn(void);
Boolean  LIBCALL IntrezNetEntrezInit(CharPtr);
void     LIBCALL IntrezFini(void);
Boolean  LIBCALL IntrezIsInited(void);
CharPtr  LIBCALL SHoundGetCurrentServer(void);

/* SeqId to/from Gi, Accession to/from Gi Conversions */
Int4        LIBCALL IntrezFindSeqId(SeqIdPtr);
ValNodePtr  LIBCALL IntrezFindSeqIdList(ValNodePtr);
Int4        LIBCALL IntrezFindAcc(CharPtr);
ValNodePtr  LIBCALL IntrezFindAccList(ValNodePtr);
CharPtr     LIBCALL IntrezAccFromGi(Int4);
ValNodePtr  LIBCALL IntrezAccFromGiList(ValNodePtr);
CharPtr     LIBCALL IntrezTitleFromGi(Int4);
ValNodePtr  LIBCALL IntrezTitleFromGiList(ValNodePtr);
SeqIdPtr    LIBCALL IntrezSeqIdFromGi(Int4);
ValNodePtr  LIBCALL IntrezSeqIdFromGiList(ValNodePtr);
SeqIdPtr    LIBCALL FreeSeqId(SeqIdPtr);
ValNodePtr  LIBCALL FreeVNSeqIdList(ValNodePtr);

/* Sequence &  structure checks */
Boolean    LIBCALL IntrezIsProtein(Int4);
Boolean    LIBCALL IntrezIsNRFirst(Int4);
Boolean    LIBCALL IntrezExists(Int4);
Int4       LIBCALL Intrez3DbWhat(Int4);
Boolean    LIBCALL Intrez3DExists(Int4);

/* Sequence fetch - Bioseq */
BioseqPtr  LIBCALL IntrezGetBioseq(Int4);
ValNodePtr LIBCALL IntrezGetBioseqList(ValNodePtr);
ValNodePtr LIBCALL FreeBioseqList(ValNodePtr);

/* Sequence fetch - BioseqSet */
BioseqSetPtr LIBCALL IntrezGetBioseqSet(Int4);
ValNodePtr   LIBCALL IntrezGetBioseqSetList(ValNodePtr);
ValNodePtr   LIBCALL FreeBioseqSetList(ValNodePtr);

/* Sequence fetch - SeqEntry */
SeqEntryPtr  LIBCALL IntrezGetSeqEntry(Int4);
ValNodePtr   LIBCALL IntrezGetSeqEntryList(ValNodePtr);
ValNodePtr   LIBCALL FreeSeqEntryList(ValNodePtr);
Boolean      LIBCALL ExpandSeqEntry(SeqEntryPtr PNTR ppse);


/* Sequence fetch - FastA */
SLRIFastaPtr  LIBCALL IntrezGetFasta(Int4);
ValNodePtr    LIBCALL IntrezGetFastaList(ValNodePtr);
ValNodePtr    LIBCALL FreeFastaList(ValNodePtr);
Boolean       LIBCALL PrintSLRIFastaSequence(CharPtr pseq, Int4 linelen, FILE *pfile);	
Boolean       LIBCALL PrintSLRIFasta(SLRIFastaPtr pfa, Int4 linelen, FILE *pfile);
Boolean       LIBCALL PrintSLRIFastaList(ValNodePtr pvnfa, Int4 linelen, FILE *pfile);


/* Sequence fetch - GenBank flat file */
Boolean    LIBCALL IntrezGetGenBankff(Int4 gi, FILE *pfile);
Boolean    LIBCALL IntrezGetGenBankffList(ValNodePtr pvngi, FILE *pfile);

/* Sequence fetch - XML */
Boolean LIBCALL IntrezGetXMLBioseq(Int4 gi, CharPtr file_name);  /* not tested, not remote */
Boolean LIBCALL IntrezGetXMLSeqEntry(Int4 gi, CharPtr file_name); /* not tested, not remote */


/*  Links */
ValNodePtr  LIBCALL IntrezGiFrom3D(Int4, Boolean);
ValNodePtr  LIBCALL IntrezGiFrom3DList(ValNodePtr, Boolean);
Int4        LIBCALL Intrez3DFromGi(Int4);
ValNodePtr  LIBCALL Intrez3DFromGiList(ValNodePtr);
Int4        LIBCALL IntrezDNAFromProtein(Int4 Gia);
ValNodePtr  LIBCALL IntrezDNAFromProteinList(ValNodePtr pvnGia);
ValNodePtr  LIBCALL IntrezProteinFromDNA(Int4 Gin);
ValNodePtr  LIBCALL IntrezProteinFromDNAList(ValNodePtr pvnGin);
ValNodePtr  LIBCALL IntrezMuidFromGi(Int4 Gi);
ValNodePtr  LIBCALL IntrezMuidFromGiList(ValNodePtr pvnGi);
ValNodePtr  LIBCALL IntrezMuidFrom3D(Int4);
ValNodePtr  LIBCALL IntrezMuidFrom3DList(ValNodePtr);
Int4        LIBCALL IntrezTaxIDFromGi(Int4 Gi);
ValNodePtr  LIBCALL IntrezTaxIDFromGiList(ValNodePtr pvnGi);
ValNodePtr  LIBCALL IntrezTaxIDFrom3D(Int4);
ValNodePtr  LIBCALL IntrezTaxIDFrom3DList(ValNodePtr);
ValNodePtr  LIBCALL IntrezProteinsFromTaxID(Int4 taxid);
ValNodePtr  LIBCALL IntrezProteinsFromTaxIDII(Int4 taxid);
ValNodePtr  LIBCALL IntrezProteinsFromTaxIDList(ValNodePtr pvntaxid);
ValNodePtr  LIBCALL IntrezDNAFromTaxID(Int4 taxid);
ValNodePtr  LIBCALL IntrezDNAFromTaxIDII(Int4 taxid);
ValNodePtr  LIBCALL IntrezDNAFromTaxIDList(ValNodePtr pvntaxid);

ValNodePtr LIBCALL IntrezDNAFromTaxIDIII(Int4 taxid);
ValNodePtr LIBCALL IntrezProteinsFromTaxIDIII(Int4 taxid);

/*redundancy */
ValNodePtr LIBCALL IntrezRedundantGroup(Int4 Gi);
ValNodePtr LIBCALL IntrezRedundantGroupList(ValNodePtr pvnGi);
Int4       LIBCALL IntrezRedundantGroupIDFromGI(Int4 Gi);
ValNodePtr LIBCALL IntrezRedundantGroupIDFromGIList(ValNodePtr pvnGi);
ValNodePtr LIBCALL IntrezRedundantGroupFromID(Int4 group);
ValNodePtr LIBCALL IntrezRedundantGroupFromIDList(ValNodePtr pvngroup);
Int4       LIBCALL IntrezFirstOfRedundantGroupFromID(Int4 Gi);
ValNodePtr LIBCALL IntrezFirstOfRedundantGroupFromIDList(ValNodePtr pvnGi);



/* Genome Iterators */
ValNodePtr  LIBCALL IntrezProteinsFromOrganism(Int4 taxid, Byte chromflags);
ValNodePtr  LIBCALL IntrezProteinsFromOrganismList(ValNodePtr pvnTax, Byte chromflags);
ValNodePtr  LIBCALL IntrezProteinsFromChromosome(Int4 chromid);
ValNodePtr  LIBCALL IntrezProteinsFromChromosomeList(ValNodePtr pvnChrom);
ValNodePtr  LIBCALL IntrezChromosomeFromGenome(Int4 taxid, Byte chromflags);
ValNodePtr  LIBCALL IntrezChromosomeFromGenomeList(ValNodePtr pvnGenom, Byte chromflags);
ValNodePtr  LIBCALL IntrezDNAFromOrganism(Int4 taxid, Byte chromflags);
ValNodePtr  LIBCALL IntrezDNAFromOrganismList(ValNodePtr pvnTax, Byte chromflags);
ValNodePtr  LIBCALL IntrezDNAFromChromosome(Int4 chromid);
ValNodePtr  LIBCALL IntrezDNAFromChromosomeList(ValNodePtr pvnChrom);

/* Structure Fetch - Biostruc */
BiostrucPtr  LIBCALL IntrezGet3D(Int4);
BiostrucPtr  LIBCALL IntrezGet3DfromPdbId(CharPtr pdbid);
Boolean      LIBCALL IntrezGetXML3D(Int4 mmdbid, CharPtr file_name); /* not tested, not remote */
Boolean      LIBCALL IntrezGetPDB3D(Int4 mmdbid, CharPtr file_name); /* not tested, not remote */

/* domains */
SLRIValNodePtr LIBCALL IntrezGetDomain(Int4 gi, Int2 domno);
SLRIFastaPtr   LIBCALL SHoundGetFastaDomain(Int4 domid, Int4 tolerance);
SLRIFastaPtr   LIBCALL SHoundMakeFastaDomain(StDomdbPtr pdd, Int4 tolerance);
CharPtr        LIBCALL GetDefline(StDomdbPtr pdd);
CharPtr        LIBCALL GetCompound(Int4 gi);
Boolean        LIBCALL WriteSequence(CharPtr pseq, CharPtr PNTR ppseqfin, Int4 from, Int4 to, Boolean Xout);


/* Taxonomy */
/*Get direct children tax ID of a given tax ID*/
ValNodePtr TaxDB_GetChildNodes(Int4 taxId);
/*Get direct children tax IDs of a given list of tax ID*/
ValNodePtr TaxDB_GetChildNodesList(ValNodePtr taxIdList);
/*Get all children recursively of a given tax ID (get everything lower in the tree)*/
/*Warning: calling this function with high taxId nodes, like 1 will take a very long time.  If you want
that many taxIDs returned, call the function once, save the results and use them instead of calling the function
repeatedly*/
ValNodePtr TaxDB_GetAllProgeny(Int4 taxId);
/*Get parent tax ID of a given tax ID*/
Int4 TaxDB_GetParent(Int4 taxId);
/*Get all ancestors of a given tax ID*/
ValNodePtr TaxDB_GetAllAncestors(Int4 taxId);
/*finds if a given tax ID has been merged into a new one*/
Boolean TaxDB_IsMerged(Int4 taxId);
/*finds if a given tax ID has been deleted*/
Boolean TaxDB_IsDeleted(Int4 taxId);
/*Get taxonomy name from tax ID by name class (see objslritax.h for ASN.1 enumeration defines)*/
/*Returns NULL if name or tax ID not found.  Allocates memory for name - caller must free returned string.*/
CharPtr TaxDB_GetNameFromTaxIDByClass(Int4 taxId, Uint2 nameClass);
/*Get taxonomy name from tax ID (scientific name)*/
/*Returns NULL if tax ID not found.  Allocates memory for name - caller must free returned string.*/
CharPtr TaxDB_GetNameFromTaxID(Int4 taxId);
#define TAXDB_GetLineage_FULL 10
#define TAXDB_GetLineage_GENBANK 20
#define TAXDB_GetLineage_TAXDBSHORT 30
/*Get lineage from tax ID*/
/*Type: use type defines:
TAXDB_GetLineage_FULL - gets full lineage
TAXDB_GetLineage_GENBANK - gets lineage as it would appear in a genbank record
TAXDB_GetLineage_TAXDBSHORT - gets lineage as it would appear on the taxonomy database homepage (tax IDs with no rank are filtered)*/
/*Returns NULL if tax ID not found.  Allocates memory for lineage - caller must free returned string.*/
CharPtr TaxDB_GetLineageFromTaxId(Int4 taxId, Uint2 type);
/*TODO: get tax ids from any text (needs the text indexer)*/


/*config file function calls*/
void Cfg_intrez_new(void);
void Cfg_FreeMem(void);
CharPtr Cfg_GetCgiPath(void);
Int4 Cfg_GetMaxQueryTime(void);
CharPtr Cfg_GetPrecompuPath(void);
Int4 Cfg_GetIntValue(CharPtr section, CharPtr key);
Int4 Cfg_GetMaxQuery(void);
CharPtr Cfg_GetIndexFile(void);
void Cfg_IncrementQueryCount(void);
Int4 Cfg_GetQueryCount(void);
void Cfg_IncrementMaxQuery(void);

typedef struct Intrez_Config_Info {
	CharPtr Cgi_path;
	CharPtr precompu_path;
	Int4 MaxQueries;
	Int4 MaxQueryTime;
	Int4 QueryCount;
	CharPtr indexFile;
} IntrezCfg, PNTR IntrezCfgPtr;

/*structure of the precomputed query index file*/
typedef struct Tax_index{
	Int4 tax_id;
	Int4 num_gis;
	Int4 num_DNA_gis;
	Int4 num_proteins_gis;
}Tax_index;

/*mehtods that retrieve information from the index file*/
/*checks to see a taxid is precomputed.  if it is then
if fills up an array of all the gis and returns the number of
gis it just copied over.  Must hand the function a pointer to an
Int4 and remember to free it once done.*/
Int4 Check_precomputed_index(Int4 ** gis, Int4 taxid, CharPtr TaxType);


/***********************************************
* Fasta functionality
***********************************************/
Boolean LIBCALL SLRIBioseqToFasta (BioseqPtr bsp, SLRIFastaPtr fasta_ptr, Boolean is_na);
Boolean LIBCALL SLRIBioseqToFastaX (BioseqPtr bsp, MyFsaPtr mfp, Boolean is_na);
Boolean LIBCALL FastaMemFunc (BioseqPtr bsp, Int2 key, CharPtr buf,
                                  Uint4 buflen, ByteStorePtr data);


/***********************************************
* function declarations for mmdblocl stuff
***********************************************/
Int4        LIBCALL GetMMDBSize(void);
void        LIBCALL SetMMDBIterateStart(Int4 MMDBid);
Int4        LIBCALL GetMMDBIdCurrent(void);
Boolean     LIBCALL IterateMMDBId(void);
CharPtr     LIBCALL GetPDBIdCurrent(void);
DocUid      LIBCALL MMDBEvalPDB (CharPtr str);
BiostrucPtr LIBCALL MMDBBiostrucGet (DocUid uid, Int4 mdlLvl, Int4 maxModels);
Boolean     LIBCALL MMDBInit (void);
void        LIBCALL MMDBFini (void);
void        LIBCALL ResetMMDBIterate(void);

/********************************************
* function for CDDB
********************************************/
SLRIFastaPtr LIBCALL IntrezGetCDSLRIFasta(Int4 Gi);

#ifdef __cplusplus
/* { */ }
#endif



#endif  /* _INTREZ_DEFS */

/*
$Log: intrez_defs.h,v $
Revision 1.6  2004/08/12 17:55:28  zwang
New API added to get title by gi from ACCDB

Revision 1.5  2004/06/18 22:19:24  hlieu
change includes for new GenBankff, bug 1162

Revision 1.4  2004/05/20 18:18:07  vgu
added conditional compilation for GenBankff on ODBC and CB

Revision 1.3  2004/05/07 17:18:28  rcavero
Replace asn2ff.h with asn2gnbk.h to be able to use
NCBI's function SeqEntryToGnbk

Revision 1.2  2003/10/24 17:53:12  feldman
Added SHoundGet3DEx function to allow user to choose model level and num models when fetching biostruc

Revision 1.1.1.2  2003/07/26 08:41:52  root
Automated import from Sourceforge.net

Revision 1.2  2003/07/25 15:23:29  haocl
changed prototype of AppendRecordPUBSEQ to allow storing of pmid

Revision 1.1.1.1  2001/05/25 21:36:20  kaca
initial import

Revision 1.31  2001/03/27 21:09:10  doron
Addition of CDDB function decl. and struc

Revision 1.30  2001/03/26 19:46:02  kaca
reorganized headers to support a single header for the local and remote APIs

Revision 1.29  2001/03/20 16:17:59  kaca
removed extern from declarations for taxonomy functions

Revision 1.28  2001/03/15 18:03:58  gbader
Added SHoundGetCurrentServer function

Revision 1.27  2001/03/15 00:08:02  kaca
XML functions for sequences and structures

Revision 1.26  2001/03/09 20:10:04  kaca
freeing functions now return input param type

Revision 1.25  2001/03/09 19:18:24  kaca
added defines

Revision 1.23  2001/03/08 15:43:07  kaca
removed valnode/string functions which migratedinto slrilib

Revision 1.22  2001/02/23 22:40:21  kaca
new functions

Revision 1.21  2001/02/23 21:03:16  kaca
license change

Revision 1.20  2001/02/21 22:41:46  kaca
new fasta function

Revision 1.19  2001/02/20 23:22:28  kaca
new fasta domain related functions

Revision 1.18  2001/02/19 20:11:31  kaca
more functions to support pdbrep.c

Revision 1.17  2001/02/16 19:22:20  kaca
new functions

Revision 1.16  2001/02/09 21:45:22  kaca
added SHoundAllMMDBID function

Revision 1.15  2001/02/08 21:27:30  kaca
removed vastdb struc definition

Revision 1.14  2001/02/06 20:22:36  kaca
added define

Revision 1.13  2001/02/06 20:16:13  kaca
added function declarations

Revision 1.12  2001/02/05 21:06:41  kaca
added function declaration

Revision 1.11  2001/01/27 21:54:48  kaca
moved one function from cbmmdb in

Revision 1.10  2001/01/26 20:46:28  kaca
minor changes

Revision 1.9  2000/12/22 20:38:33  kaca
removed hand written database structures, added functions for storing records

Revision 1.8  2000/11/06 22:17:33  kaca
removed obsolete variable

Revision 1.7  2000/11/01 23:28:40  kaca
redundancy and taxonomy functions added to remote API

Revision 1.6  2000/10/24 14:39:16  kaca
added a function for expanding seqentries

Revision 1.5  2000/10/19 19:55:09  kaca
added defines of Intrez to SHound functions which enable linking of remote API code to local library

Revision 1.4  2000/10/17 18:27:58  kaca
added mmdblocl function declarations

Revision 1.3  2000/10/16 20:26:27  kaca
added header for mmdblocl

Revision 1.2  2000/10/16 20:11:54  kaca
minor changes

Revision 1.1  2000/10/13 21:47:30  kaca
split headers into private, public and definitions

*/


