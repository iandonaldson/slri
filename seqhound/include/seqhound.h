/* $Id: seqhound.h,v 1.36 2005/04/29 19:56:03 rcavero Exp $
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
  January 2000
  header file for SeqHound API light-weight version

******************************************************************************************/

#ifndef _SEQHOUND_
#define _SEQHOUND_

#include <asn.h>
#include <ncbi.h>
#include <mmdbapi.h>
#include <objslristruc.h>
#include <objNBlastasn.h>
#include <objslrilinkset.h>
#include <slri_misc.h>
#include <rpsdbapi.h>

#ifdef __cplusplus
extern "C" { /* } */
#endif

/* sets the SeqHound server as local or remote */
typedef enum {
	local = 0, 
	remote = 1
} SH_Server_Type;

/* Global Info Pointer */
/* NOT YET USED  */
typedef struct sh_info_ptr {
	SH_Server_Type server_type;  /* Local or Remote                        */
	CharPtr pcServerName;        /* Server name                            */
	CharPtr pcCGI_Path;          /* CGI path                               */
	CharPtr pcServerCGI_Path;    /* Server and CGI path                    */
	Boolean bIsIgnited;          /* is SeqHound initialized?               */
	Boolean bIsNetEntrez;        /* is NetEntrez on?                       */
	Int4 idtimeout;              /* default timeout                        */
	Int4 iutimeout;              /* user set timeout                       */
} SHInfoPtr, PNTR SHInfo;

/* molecule type as defined in the BIND spec. */
#define BIND_NOT_SPEC  0
#define BIND_PROTEIN   1
#define BIND_DNA       2
#define BIND_RNA       3
#define BIND_SMALL_MOL 4
#define BIND_COMPLEX   5
#define BIND_GENE      6

/* ============================================================= 
   Defines used to extract complete genome information.
   ============================================================= */
/*----------------------------------------------------------------
  Defines for biosource type. These correspond to NCBI defines in
  ncbi/api/subutil.h.
  ----------------------------------------------------------------*/
#define SOURCE_UNKNOWN          0
#define SOURCE_GENOMIC          1
#define SOURCE_CHLOROPLAST      2
#define SOURCE_CHROMOPLAST      3
#define SOURCE_KINETOPLAST      4
#define SOURCE_MITOCHONDRION    5
#define SOURCE_PLASTID          6
#define SOURCE_MACRONUCLEAR     7
#define SOURCE_EXTRACHROM       8
#define SOURCE_PLASMID          9
#define SOURCE_TRANSPOSON       10
#define SOURCE_INSERTION_SEQ    11
#define SOURCE_CYANELLE         12
#define SOURCE_PROVIRAL         13
#define SOURCE_VIRION           14
#define SOURCE_NUCLEOMORPH      15
#define SOURCE_APICOPLAST       16
#define SOURCE_LEUCOPLAST       17
#define SOURCE_PROPLASTID       18
#define SOURCE_ENDOGENOUS_VIRUS 19


/*----------------------------------------------------------------
  Defines for subsource type. These correspond to NCBI defines in
  ncbi/api/subutil.h. Only types that identify chromosome types
  or names are defined here.
  ----------------------------------------------------------------*/
#define SUBSRC_CHROMOSOME       1
#define SUBSRC_PLASMID          19
#define SUBSRC_TRANSPOSON       20
#define SUBSRC_INSERTION_SEQ    21
#define SUBSRC_PLASTID          22
#define SUBSRC_SEGMENT          24
#define SUBSRC_ENDOGENOUS_VIRUS 25

#define ORG_NAME_BINOMIAL 1
/*
#define CHROM      0x80
*/
#define CHROM_PHAGE   0x40
#define CHROM_PLASTID 0x20
#define CHROM_ECE     0x10
#define CHROM_PLMD    0x08
#define CHROM_CHLO    0x04
#define CHROM_MITO    0x02
#define CHROM_CHROM   0x01
#define CHROM_NR      0x80
#define CHROM_ALL     0xFF
 
#define PROTEIN "protein"
#define DNA "dna"
#define RNA "rna"

/* SeqFeat defines */
#define ANNOT_TABLE 1
#define ANNOT_ALIGN 2
#define ANNOT_GRAPH 3
#define ANNOT_IDS   4
#define ANNOT_LOCS  5

/* the bit flags for REPORT_FLAGS */
#define BYTE_LENGTH 8
#define REPORT_ACC    0x0001            /* the accession flag */
#define REPORT_DEF    0x0002            /* the defline flag   */
#define REPORT_DNA    0x0004            /* the dna flag       */
#define REPORT_MUID   0x0008            /* the muid flag      */
#define REPORT_MMDB   0x0010            /* the mmdb flag      */
#define REPORT_GO     0x0020            /* the GO flag        */
#define REPORT_OMIM   0x0040            /* the omim flag      */
#define REPORT_LL     0x0080            /* the locuslink report */
#define REPORT_TAX    0x0100            /* the taxid flag     */
#define REPORT_CDD    0x0200            /* the cdd flag       */
#define REPORT_PMID   0x0400            /* the pmid flag      */
#define REPORT_REDUND 0x0800            /* the redundant-set flag */
#define REPORT_NBR    0x1000            /* the neighbour-set flag */
#define REPORT_ALL    0x1FFF            /* the all flag       */


/* DBLayer return codes */
#define SH_SUCCESS 1
#define SH_ERROR 0
#define SH_NODATA -1


/************************************************/
/* SeqHound Initialization, Finishing and Testing */
/************************************************/
 Boolean    LIBCALL SHoundInit(Boolean NetEntrezOnToo, CharPtr appname);
 Boolean    LIBCALL SHoundInitReadOnly(Boolean NetEntrezOnToo, CharPtr appname);
 Boolean    LIBCALL SHoundInitEx(Boolean NetEntrezOnToo, CharPtr appname, Boolean ReadOnly);
/* if NetEntrezOnToo is set it does an EntrezInit() */
 Boolean    LIBCALL SHoundIsNetEntrezOn(void);
 Boolean    LIBCALL SHoundNetEntrezInit(CharPtr appname);
 void       LIBCALL SHoundFini(void);
 Boolean    LIBCALL SHoundIsInited(void);
 CharPtr    LIBCALL SHoundGetCurrentServer(void);
 Boolean    LIBCALL SHoundStartTransaction(void);
 Boolean    LIBCALL SHoundEndTransaction(void);

/* User functions to get and change the connection timeout setting */
/* useful only with remote api - local api for GetTimeout returns 0 */
 void SHoundUserSetTimeout(Int4 sec);
 Int4 SHoundUserGetTimeout(void);

 
/*******************************************************
 *              SHoundProteinReport 
 *******************************************************/
 Boolean    LIBCALL SHoundProteinReportSetDescription(SHProteinReportSetDescriptionPtr pprsd, Uint2 flags);
 Boolean    LIBCALL SHoundGetLocusLinkReport(SHLocusLinkReportPtr plr, Int4 Gi);
 Boolean    LIBCALL SHoundGetProteinReport(SHProteinReportPtr pprs, Uint2 REPORT_FLAGS);
 Boolean    LIBCALL SHoundGetLocusProduct(SHLocusProductPtr plp, const Int4 LLID); 
 Boolean    LIBCALL SHoundGetGOReport(SHGoReportPtr pgr, Int4 Gi);
 Boolean    LIBCALL ProteinReportSetNeighboursReport(SHProteinReportSetDescriptionPtr pprsd, Uint2 flags);
 Boolean    LIBCALL ProteinReportSetRedundantGroup(SHProteinReportSetDescriptionPtr pprsd, Uint2 flags);
 Int2 AddGOAnnotation(SHGoAnnotPtr, Int4); 
/******************************************************/
/* SeqId to/from Gi, Accession to/from Gi Conversions */
/******************************************************/
 Int4       LIBCALL SHoundFindAcc(CharPtr pcAcc);
 ValNodePtr LIBCALL SHoundFindAccList(ValNodePtr pvnAcc);
 Int4       LIBCALL SHoundFindName(CharPtr pName);
 ValNodePtr LIBCALL SHoundFindNameList(ValNodePtr pvnName);
 CharPtr    LIBCALL SHoundAccFromGi(Int4 gi);
 ValNodePtr LIBCALL SHoundAccFromGiList(ValNodePtr pvnGi);
 CharPtr    LIBCALL SHoundTitleFromGi(Int4 gi);
 ValNodePtr LIBCALL SHoundTitleFromGiList(ValNodePtr pvnGi);
 SeqIdPtr   LIBCALL SHoundSeqIdFromGi(Int4 gi);
 ValNodePtr LIBCALL SHoundSeqIdFromGiList(ValNodePtr pnvGi);
 SeqIdPtr   LIBCALL FreeSeqId(SeqIdPtr psi);
 ValNodePtr LIBCALL FreeVNSeqIdList(ValNodePtr pvnSeqId);
 CharPtr	LIBCALL SHoundGetNameByGi(Int4 gi);

/******************************************************/
/*  Definition Line                                   */
/******************************************************/
CharPtr		LIBCALL SHoundGetDefline(Int4  Gi);
ValNodePtr	LIBCALL SHoundGetDeflineList(ValNodePtr pvnGi);

/******************************************************/
/*  Sequence & Structure Checks                       */
/******************************************************/
 Boolean    LIBCALL SHoundIsProtein(Int4 Gi);
 Boolean    LIBCALL SHoundIsNRFirst(Int4 Gi);
 Boolean    LIBCALL SHoundExists(Int4 Gi);
 Int4       LIBCALL SHound3DbWhat(Int4 mmdbid);
 Boolean    LIBCALL SHound3DExists(Int4 mmdbid);
 CharPtr    LIBCALL SHoundPDBFromMMDB(Int4 mmdbid);
 Int4       LIBCALL SHoundGetLargestMMDBID(void);

/******************************************************/
/*  Sequence fetch - Bioseq                           */
/******************************************************/
 BioseqPtr  LIBCALL SHoundGetBioseq(Int4 Gi);
 ValNodePtr LIBCALL SHoundGetBioseqList(ValNodePtr pvnGi);
 ValNodePtr LIBCALL FreeBioseqList(ValNodePtr pvnBioseq);


/******************************************************/
/*  Sequence fetch - SeqEntry                           */
/******************************************************/
 SeqEntryPtr  LIBCALL SHoundGetSeqEntry(Int4 Gi);
 ValNodePtr   LIBCALL SHoundGetSeqEntryList(ValNodePtr pvnGi);
 ValNodePtr   LIBCALL FreeSeqEntryList(ValNodePtr pvnSeqEntry);


/***********************************************/
/*  Sequence fetch - BioseqSet                 */
/***********************************************/
 BioseqSetPtr  LIBCALL SHoundGetBioseqSet(Int4 Gi);
 ValNodePtr LIBCALL SHoundGetBioseqSetList(ValNodePtr pvnGi);
 ValNodePtr LIBCALL FreeBioseqSetList(ValNodePtr pvnBioseqSet);


/************************************************/
/* Sequence fetch - Fasta                       */
/************************************************/
 SLRIFastaPtr LIBCALL SHoundGetFasta(Int4 Gi);
 ValNodePtr   LIBCALL SHoundGetFastaList(ValNodePtr pvnGi);
 ValNodePtr   LIBCALL FreeFastaList(ValNodePtr pvnFasta);
 Boolean      LIBCALL PrintSLRIFasta(SLRIFastaPtr pfa, Int4 linelen, FILE *pfile);
 Boolean      LIBCALL PrintSLRIFastaList (ValNodePtr pvnfa, Int4 linelen, FILE *pfile);	
 Boolean      LIBCALL PrintSLRIFastaSequence(CharPtr pseq, Int4 linelen, FILE *pfile);


/******************************************************/
/*  Sequence fetch - GenBank flat file                */
/* writes into a file                                 */
/******************************************************/
 Boolean LIBCALL SHoundGetGenBankff(Int4 gi, FILE *pfile);
 Boolean LIBCALL SHoundGetGenBankffList(ValNodePtr pvnGi, FILE *pfile);

/************************************************/
/* Sequence fetch - XML                         */
/************************************************/
 Boolean LIBCALL SHoundGetXMLBioseq(Int4 gi, CharPtr file_name);
 Boolean LIBCALL SHoundGetXMLSeqEntry(Int4 gi, CharPtr file_name);

/************************************************/
/* Structure fetch - Biostruc                   */
/************************************************/
 BiostrucPtr  LIBCALL SHoundGet3D(Int4 mmdbid);
 BiostrucPtr  LIBCALL SHoundGetFixed3D(Int4 mmdbid, Int4 mdlLvl, Int4 maxModels);
 BiostrucPtr  LIBCALL SHoundGet3DEx(Int4 mmdbid, Int4 mdlLvl, Int4 maxModels);
 BiostrucPtr  LIBCALL SHoundGet3DfromPdbId(CharPtr pdbid);
 BiostrucPtr  LIBCALL SHoundGet3DfromPdbIdEx(CharPtr pdbid ,Int4 mdlLvl, Int4 maxModels);
 Boolean      LIBCALL SHoundGetXML3D(Int4 mmdbid, CharPtr file_name);
 Boolean      LIBCALL SHoundGetPDB3D(Int4 mmdbid, CharPtr file_name);

/************************************************/
/* SeqHound links                               */
/************************************************/
 ValNodePtr  LIBCALL SHoundGiFrom3D(Int4 mmdbid, Boolean ProteinOnly);
 ValNodePtr  LIBCALL SHoundGiFrom3DList(ValNodePtr pvnMmdbid, Boolean ProteinOnly);
 Int4        LIBCALL SHoundGiFromPDBchain(CharPtr pdbcode, CharPtr chain);
 Int4        LIBCALL SHound3DFromGi(Int4 Gi);
 ValNodePtr  LIBCALL SHound3DFromGiList(ValNodePtr pvnGi);
 Int4        LIBCALL SHoundDNAFromProtein(Int4 Gi);
 ValNodePtr  LIBCALL SHoundDNAFromProteinList(ValNodePtr pvnGi);
 ValNodePtr  LIBCALL SHoundProteinFromDNA(Int4 Gi);
 ValNodePtr  LIBCALL SHoundProteinFromDNAList(ValNodePtr pvnGi);
 ValNodePtr  LIBCALL SHoundMuidFromGi(Int4 Gi);
 ValNodePtr  LIBCALL SHoundMuidFromGiList(ValNodePtr pvnGi);
 ValNodePtr  LIBCALL SHoundMuidFrom3D(Int4 mmdbid);
 ValNodePtr  LIBCALL SHoundMuidFrom3DList(ValNodePtr pvnMmdbid);
 Int4        LIBCALL SHoundTaxIDFromGi(Int4 Gi);
 ValNodePtr  LIBCALL SHoundTaxIDFromGiList(ValNodePtr pvnGi);
 ValNodePtr  LIBCALL SHoundTaxIDFrom3D(Int4 mmdbid);
 ValNodePtr  LIBCALL SHoundTaxIDFrom3DList(ValNodePtr pvnMmdbid);
 ValNodePtr  LIBCALL SHoundProteinsFromTaxID(Int4 taxid);
 ValNodePtr  LIBCALL SHoundProteinsFromTaxIDIII(Int4 taxid);
 ValNodePtr  LIBCALL SHoundProteinsFromTaxIDList(ValNodePtr pvntaxid);
 ValNodePtr  LIBCALL SHoundDNAFromTaxID(Int4 taxid);
 ValNodePtr  LIBCALL SHoundDNAFromTaxIDIII(Int4 taxid);
 ValNodePtr  LIBCALL SHoundDNAFromTaxIDList(ValNodePtr pvntaxid);

/************************************************/
/* Blast Result Queries                         */
/************************************************/
SLRI_ERR LIBCALL SHoundGetBlastResult (Int4 gi1, Int4 gi2, NBlastResultSetPtr PNTR ppResult);
SLRI_ERR LIBCALL SHoundGetBlastSeqAlign (Int4 gi1, Int4 gi2, SeqAlignPtr PNTR psap);

/************************************************/
/* Neighbours                                   */
/************************************************/
/* Sequence Neighbours                          */
/************************************************/
 Boolean  LIBCALL HasNeighbours(Int4 gi); 
 SLRI_ERR LIBCALL SHoundNeighboursFromGiEx (Int4 gi, FLinkSetPtr PNTR pNeighbours, FloatHi evalue, Boolean bLimit);
 SLRI_ERR LIBCALL SHoundNeighboursFromGi (Int4 gi, FLinkSetPtr PNTR pNeighbours, FloatHi evalue, Boolean bLimit);
 SLRI_ERR LIBCALL SHoundNeighboursFromGiList (ValNodePtr gilist, FLinkSetListPtr PNTR pNeighboursList, FloatHi evalue);
 SLRI_ERR LIBCALL SHoundNeighboursFromTaxID (Int4 taxid, FLinkSetListPtr PNTR pNeighboursList, FloatHi evalue); 
 SLRI_ERR LIBCALL SHoundNeighboursOfNeighbours (Int4 gi, FLinkSetListPtr PNTR pGINeighboursList, FloatHi evalue);
 SLRI_ERR LIBCALL SHoundNeighboursOfNeighboursList (ValNodePtr pvngilist, FLinkSetList2Ptr PNTR pAllNeighboursList, FloatHi evalue);
 SLRI_ERR LIBCALL SHoundNeighboursOfNeighboursFromTaxID(Int4 taxid, FLinkSetList2Ptr PNTR ppAllNeighboursList, FloatHi evalue);

 SLRI_ERR LIBCALL SHoundGiAndNumNeighboursList (ILinkSetPtr PNTR ilsp);
 SLRI_ERR LIBCALL SHoundNumNeighboursInDB (Int4 PNTR pnum);

/************************************************/
/* Neighbours with Structures                   */
/************************************************/
 Boolean  LIBCALL Has3DNeighbours(Int4 gi);
 SLRI_ERR LIBCALL SHound3DNeighboursFromGiEx (Int4 gi, ILinkSetPtr PNTR p3DNeighbours, FloatHi evalue);
 SLRI_ERR LIBCALL SHound3DNeighboursFromGi (Int4 gi, ILinkSetPtr PNTR p3DNeighbours, FloatHi evalue);
 SLRI_ERR LIBCALL SHound3DNeighboursFromGiList (ValNodePtr pvngilist, ILinkSetListPtr PNTR p3DNeighboursList, FloatHi evalue);
 SLRI_ERR LIBCALL SHound3DNeighboursFromTaxID (Int4 taxid, ILinkSetListPtr PNTR p3DNeighboursList, FloatHi evalue);

 SLRI_ERR LIBCALL SHoundGiAndNumNeighboursList (ILinkSetPtr PNTR ilsp); 
 SLRI_ERR LIBCALL SHoundNum3DNeighboursInDB (Int4 PNTR pnum);

 SLRI_ERR LIBCALL SHound3DListFromTaxID (Int4 taxid, ILinkSetPtr PNTR p3DList);


/************************************************/
/* Genome iterators                             */
/************************************************/
 ValNodePtr   LIBCALL SHoundProteinsFromOrganism(Int4 taxid, Byte chromflags);
 ValNodePtr   LIBCALL SHoundProteinsFromOrganismList(ValNodePtr pvnTax, Byte chromflags);
 ValNodePtr   LIBCALL SHoundProteinsFromChromosome(Int4 chromid);
 ValNodePtr   LIBCALL SHoundProteinsFromChromosomeList(ValNodePtr pvnChrom);
 ValNodePtr   LIBCALL SHoundAllGenomes(void);
 ValNodePtr   LIBCALL SHoundChromosomeFromGenome(Int4 taxid, Byte chromflags);
 ValNodePtr   LIBCALL SHoundChromosomeFromGenomeList(ValNodePtr pvnGenom, Byte chromflags);
 ValNodePtr   LIBCALL SHoundDNAFromOrganism(Int4 taxid, Byte chromflags);
 ValNodePtr   LIBCALL SHoundDNAFromOrganismList(ValNodePtr pvnTax, Byte chromflags);
 ValNodePtr   LIBCALL SHoundDNAFromChromosome(Int4 chromid);
 ValNodePtr   LIBCALL SHoundDNAFromChromosomeList(ValNodePtr pvnChrom);
 ValNodePtr   LIBCALL SHoundDNAFromUnknownChromosome( Int4 taxid );
 ValNodePtr   LIBCALL SHoundProteinsFromUnknownChromosome( Int4 taxid );
 ValNodePtr   LIBCALL SHoundRNAFromChromosome(Int4 chromid);
 ValNodePtr   LIBCALL SHoundRNAFromOrganism(Int4 taxid, Byte chromflags);
 ValNodePtr   LIBCALL SHoundRNAFromUnknownChromosome( Int4 taxid );
 ValNodePtr   LIBCALL SHoundRNAFromOrganismList(Int4 taxid, Byte chromflags);
/*****************************************************/
/* Iterator Functions - callback upon each iteration */
/*****************************************************/
/* User functions must be typed as follows: */
 typedef SLRI_ERR (*SHoundIteratorFunc) PROTO((Int4 iterationnum, Int4 invalue, Pointer inpointer, Pointer PNTR poutpointer));

 SLRI_ERR SHoundTaxonIterator(Pointer inpointer, SHoundIteratorFunc iteratorFunc, Pointer PNTR ppointer);
 SLRI_ERR SHoundProteinIterator(Int4 itaxid, Pointer inpointer, SHoundIteratorFunc iteratorFunc, Pointer PNTR ppointer);
 SLRI_ERR SHoundDNAIterator(Int4 itaxid, Pointer inpointer, SHoundIteratorFunc iteratorFunc, Pointer PNTR ppointer);
 SLRI_ERR SHoundRedundGroupIterator(Int4 gi, Pointer inpointer, SHoundIteratorFunc iteratorFunc, Pointer PNTR ppointer);

/*****************************************************/
/* Genome (callback) Implementations                 */
/*****************************************************/
 SLRI_ERR LIBCALL SHoundAllGenomesWithScientificNames(CLinkSetPtr PNTR pAllGenomes);
 SLRI_ERR LIBCALL SHoundAllGenomesWithNumProteins(ILinkSetPtr PNTR pAllGenomes);

/************************************************/
/* Redundancy                                   */
/************************************************/
 ValNodePtr LIBCALL SHoundRedundantGroup(Int4 Gi);
 ValNodePtr LIBCALL SHoundRedundantGroupList(ValNodePtr pvnGi);
 Int4       LIBCALL SHoundRedundantGroupIDFromGI(Int4 Gi);
 ValNodePtr LIBCALL SHoundRedundantGroupIDFromGIList(ValNodePtr pvnGi);
 ValNodePtr LIBCALL SHoundRedundantGroupFromID(Int4 group);
 ValNodePtr LIBCALL SHoundRedundantGroupFromIDList(ValNodePtr pvngroup);
 Int4       LIBCALL SHoundFirstOfRedundantGroupFromID(Int4 group);
 ValNodePtr LIBCALL SHoundFirstOfRedundantGroupFromIDList(ValNodePtr pvngroup);

/*************************************************/
/* Gene Ontology                                 */
/*************************************************/
 ValNodePtr  LIBCALL SHoundGODBGetRecordByReference(CharPtr refType);
 ValNodePtr  LIBCALL SHoundGODBGetChildrenOf(Int4 goid);
 ValNodePtr  LIBCALL SHoundGODBGetParentOf(Int4 goid);
 ValNodePtr  LIBCALL SHoundGODBGetAllAncestors(Int4 goid);
 ValNodePtr  LIBCALL SHoundGODBGetAllChildren(Int4 goid);
 CharPtr     LIBCALL SHoundGODBGetNameByID(Int4 goid);
 CharPtr  LIBCALL SHoundGODBGetClassification(Int4 goid);
 /*Int4        LIBCALL SHoundGODBGetHierarchyLevel(Int4 goid);*/

/**************************************************/
/* Locus Link                                     */
/**************************************************/
 ValNodePtr  LIBCALL SHoundGiFromGOIDListAndECode(ValNodePtr vnpgo, CharPtr ECode);
 ValNodePtr  LIBCALL SHoundGiFromGOIDAndECode(Int4 GOID, CharPtr ECode);
 ValNodePtr  LIBCALL SHoundGiFromGOID(Int4 GOID);
 ValNodePtr  LIBCALL SHoundGiFromGOIDList(ValNodePtr vnpgo);
 ValNodePtr  LIBCALL SHoundGOIDFromGi(Int4 Gi);
 ValNodePtr  LIBCALL SHoundGOIDFromRedundantGi(Int4 Gi);
 ValNodePtr  LIBCALL SHoundGOIDFromGiList(ValNodePtr vnpgi);
 ValNodePtr  LIBCALL SHoundGOIDFromRedundantGiList(ValNodePtr vnpgi);
 ValNodePtr  LIBCALL SHoundGOECodeFromGiAndGOID(Int4 Gi, Int4 goid);
 ValNodePtr  LIBCALL SHoundGOPMIDFromGiAndGOID(Int4 Gi, Int4 goid);
 ValNodePtr  LIBCALL SHoundGiFromCDDID(CharPtr cddid);
 ValNodePtr  LIBCALL SHoundGiFromOMIM(Int4 omimid);
 ValNodePtr  LIBCALL SHoundGiFromOMIMList(ValNodePtr vnpomim);
 ValNodePtr  LIBCALL SHoundOMIMFromGi(Int4 Gi);
 ValNodePtr  LIBCALL SHoundOMIMFromGiList(ValNodePtr vnpgi);
 ValNodePtr  LIBCALL SHoundGiFromCDDIDList(ValNodePtr vnpcdd);
 ValNodePtr  LIBCALL SHoundCDDIDFromGi(Int4 Gi);
 ValNodePtr  LIBCALL SHoundCDDIDFromGiList(ValNodePtr vnpgi);
 FloatHi     LIBCALL SHoundCDDScoreFromGi(Int4 Gi, CharPtr cddid);
 ValNodePtr  LIBCALL SHoundGiFromLLID(Int4 llid);
 ValNodePtr  LIBCALL SHoundGiFromLLIDList(ValNodePtr vnpllid);
 Int4        LIBCALL SHoundLLIDFromGi(Int4 Gi);
 ValNodePtr  LIBCALL SHoundLLIDFromGiList(ValNodePtr vnpgi);
 CharPtr     LIBCALL SHoundLocusFromGi(Int4 Gi);

 ValNodePtr  LIBCALL SHoundGOIDFromLLID ( Int4 llid );
 Int4  LIBCALL SHoundOMIMFromLLID ( Int4 llid );
 ValNodePtr  LIBCALL SHoundCDDIDFromLLID ( Int4 llid );
 ValNodePtr  LIBCALL SHoundLLIDFromGOIDAndECode ( Int4 goid, CharPtr evidence );
 ValNodePtr  LIBCALL SHoundLLIDFromOMIM (Int4 omim);
 ValNodePtr  LIBCALL SHoundLLIDFromCDDID (CharPtr cddid);
  /**************************************
   RPS Functions 
  ***************************************/
  /* RPS Functions are declared in separate header file rpsapi.h
   */

/***********************************************
 * HISTDB Functions
 ***********************************************/
 CharPtr    LIBCALL SHoundGetAccFromRetiredGi(Int4 Gi);
 

/***********************************************/
/* Taxonomy                                    */
/***********************************************/
 ValNodePtr LIBCALL SHoundGetTaxChildNodes(Int4 taxid);
 ValNodePtr LIBCALL SHoundGetTaxChildNodesList(ValNodePtr vnptaxidlist);
 ValNodePtr LIBCALL SHoundGetAllTaxProgeny(Int4 taxid);
 Int4       LIBCALL SHoundGetTaxParent(Int4 taxid);
 ValNodePtr LIBCALL SHoundGetAllTaxAncestors(Int4 taxid);
 Boolean    LIBCALL SHoundIsTaxMerged(Int4 taxid);
 Boolean    LIBCALL SHoundIsTaxDeleted(Int4 taxid);
 CharPtr    LIBCALL SHoundGetTaxNameFromTaxIDByClass(Int4 taxid, Uint2 nameClass);
 CharPtr    LIBCALL SHoundGetTaxNameFromTaxID(Int4 taxid);
 
 CharPtr    LIBCALL SHoundGetTaxLineageFromTaxID(Int4 taxid, Uint2 type);



 Int4       LIBCALL SHoundSequenceLength(Int4 Gi);
 CharPtr    LIBCALL SHoundMoleculeType(Int4 Gi);
 ValNodePtr LIBCALL SHoundGetReferenceIDFromGi(Int4 Gi, Int2 ReferenceType);
 ValNodePtr LIBCALL SHoundGetReferenceIDFromGiList(ValNodePtr GiList, Int2 ReferenceType);
 ValNodePtr LIBCALL SHoundGiFromReferenceID(Int4 ReferenceID, Int2 ReferenceType);
 ValNodePtr LIBCALL SHoundGiFromReferenceList(ValNodePtr ReferenceList, Int2 ReferenceType);

/***********************************************/
/* DBXref                                      */
/***********************************************/

/********************************************************/
/* Given a Database name, Record Id (Cross Ref. Record),*/ 
/*  Field and a Control Vocabulary.                     */
/*  This function retrieves a ValNodePtr Link List of   */
/*  all possible Source_Records for that Record ID.     */
/*                                                      */
/*  Parameters:                                         */
/*      DBName:   Database name for the SourceRecord.   */
/*      RecordID: Cross Ref. Recird ID.                 */
/*      Field:    Field description, Optional  NULL     */
/*      CV:       Control vocabulary, Optional -1       */
/*                                                      */
/*  Returns: NULL if not values where found.            */
/*           ValNodePtr Link List of Source Records.    */
/********************************************************/
SHDBXrefPtr LIBCALL SHoundDBXREFGetSourceRecordListByDBXref(CharPtr DBName, CharPtr RecordID, CharPtr Field, Int4 CV);


/********************************************************/
/* Given a Database name, Record Id(Source Record),     */
/*  Field and a Control Vocabulary.                     */
/*  This function retrieves a ValNodePtr Link List of   */
/*  all possible DBXref (Database Cross Referencies)    */
/*  for that Source Record.                             */
/*                                                      */
/*  Parameters:                                         */
/*      DBName:   Database name for the SourceRecord.   */
/*      RecordID: SourceRecord (Source Record ID).      */
/*      Field:    Field description, Optional  NULL.    */
/*      CV:       Control vocabulary, Optional -1.      */
/*                                                      */
/*  Returns: NULL if not values where found.            */
/*           ValNodePtr Link List of DBXrefs.           */
/********************************************************/
SHDBXrefPtr LIBCALL SHoundDBXREFGetDBXrefListBySourceRecord(CharPtr DBName, CharPtr RecordID, CharPtr Field, Int4 CV);


/********************************************************/
/* Given a flag that represent Source Record or         */
/*  Record ID retrive a list of unique Database Names   */
/*  from DBXref.                                        */
/*                                                      */
/*  Parameters:                                         */
/*      recordType: int flag 1 or 0          .          */
/*      where 0 = Source Record.                        */
/*            1 = Record ID.                            */
/*            2 = All Database Names.                   */
/*                                                      */
/*  Returns: TRUE if successful and FALSE otherwise.    */
/*           ValNode LinkList with all SourceRecords.   */
/********************************************************/
ValNodePtr LIBCALL SHoundDBXREFGetDBNamesListInDBXREF();


/********************************************************/
/* Return a list of all DataBase names that are         */
/*  Source Databases into DBXref.                       */
/*  A Source Database record is the one that field      */
/*  parent_id = 0                                       */
/*  Parameters:                                         */
/*                                                      */
/*  Returns: NULL if not DataBase was found.       .    */
/*           ValNode LinkList with all Source Database  */ 
/*           names.                                     */
/********************************************************/
ValNodePtr LIBCALL SHoundDBXREFGetListOfSourceDBNames();

/********************************************************/
/* Return a list of all DataBase names that are         */
/*  Xref Databases into DBXref.                         */
/*  A Xref Database record is the one that field        */
/*  parent_id <> 0                                      */
/*                                                      */
/*  Parameters:                                         */
/*                                                      */
/*  Returns: NULL if not DataBase was found.       .    */
/*           ValNode LinkList with all Xref Database    */
/*           names.                                     */
/********************************************************/
ValNodePtr LIBCALL SHoundDBXREFGetListOfXrefDBNames();

/********************************************************/
/* Return a list of all DataBase names into DBXref.     */
/*  All Database record are the one that field          */
/*  parent_id <> 0 and parent_id = 0                    */
/*                                                      */
/*  Parameters:                                         */
/*                                                      */
/*  Returns: NULL if not DataBase was found.       .    */
/*           ValNode LinkList with all Database names.  */
/********************************************************/
ValNodePtr LIBCALL SHoundDBXREFGetListOfAllDBNames();

/********************************************************/
/* Return a unique list of all RecordIDs that belong to */
/*  a Database Name and are Source Records              */
/*                                                      */
/*  Parameters:                                         */
/*     DBName: Name of a Database                       */
/*     ei: SPTR, GB, FB, INTERPRO                       */
/*                                                      */
/*  Returns: NULL if no DBName was found.               */
/*           ValNode LinkList of RecordIDs.             */
/*                                                      */
/*  Obs: Be Carrefoul in using this function because    */
/*       can retrieve a huge Dataset > 1Gbyte           */
/********************************************************/
ValNodePtr LIBCALL SHoundDBXREFGetSourceRecordIDListByDBName(CharPtr DBName);

/********************************************************/
/* Return a unique list of all RecordIDs that belong to */
/*  a Database Name and are Xref. Records               */
/*                                                      */
/*  Parameters:                                         */
/*     DBName: Name of a Database                       */
/*     ei: SPTR, GB, FB, INTERPRO                       */
/*                                                      */
/*  Returns: NULL if no DBName was found.               */
/*           ValNode LinkList of RecordIDs.             */
/*                                                      */
/*  Obs: Be Carrefoul in using this function because    */
/*       can retrieve a huge Dataset > 1Gbyte           */
/********************************************************/
ValNodePtr LIBCALL SHoundDBXREFGetXrefRecordIDListByDBName(CharPtr DBName);

/********************************************************/
/* Return a list of CV Control Vocabulary.              */
/*  0Record ID retrive al list of unique Database Names */
/*  from DBXref.                                        */
/*                                                      */
/*  CV:   0 Not CV defined                              */
/*        1 Nucleotide                                  */
/*        2 Protein                                     */
/*        3 Domain                                      */
/*      100 Swissprot                                   */
/*      101 Trembl                                      */
/*                                                      */
/*  Returns: NULL if no CV were found.                  */
/*           ValNode LinkList with all CV.              */
/********************************************************/
ValNodePtr LIBCALL SHoundDBXREFGetCVList();


/********************************************************/
/* Return a list of Field names.                        */
/*                                                      */
/* Field: is a tag name from a 3rd party record where   */
/*        the information was extracted.                */
/*                                                      */
/*  Returns: NULL if no Field were found.               */
/*           ValNode LinkList with all Field Names.     */
/********************************************************/
ValNodePtr LIBCALL SHoundDBXREFGetXrefSourceFieldName();


/**************************************************/
/* GOA MODULE                                      */
/**************************************************/
ValNodePtr LIBCALL SHoundGOIDFromGiII(Int4 gi);
ValNodePtr LIBCALL SHoundGOIDFromRedundantGiII(Int4 gi);


/*List API functions*/
ValNodePtr   LIBCALL SHoundGetFastaKeyedList(ValNodePtr pvnGi);
ValNodePtr LIBCALL SHoundRedundantGroupKeyedList(ValNodePtr pvnGi);
/*process an array of string in order to return a valnode list stored the
 *query-return pair. The choice field of the ValNodeList indicates query or
 *return, a FastaPtr is stored in the valnode's data.ptrvalue
 *array is a pointer to an array of CharPtr
 *count is a pointer to an int value which is the non-empty entries of the array
 */ 
ValNodePtr StrArray2FastaValNodeList(CharPtr *array, Int4* count);
/*Given a Redundant groupID, return a fasta for any of gi in this groupID*/
SLRIFastaPtr LIBCALL SHoundGetFastaFromRedundantGroupID(Int4 groupID);
/*Given a list of Redundant group ids, return a list of fasta structures*/
ValNodePtr LIBCALL SHoundGetFastaFromRedundantGroupIDKeyedList(ValNodePtr
pvnGroupID);

#define SHoundGetTaxLineage_FULL 10
#define SHoundGetTaxLineage_GENBANK 20
#define SHoundGetTaxLineage_TAXDBSHORT 30


/* LEGACY function names - DO NOT USE !!! */
#define TaxDB_GetChildNodes            SHoundGetTaxChildNodes
#define TaxDB_GetChildNodesList        SHoundGetTaxChildNodesList
#define TaxDB_GetAllProgeny            SHoundGetAllTaxProgeny
#define TaxDB_GetParent                SHoundGetTaxParent
#define TaxDB_GetAllAncestors          SHoundGetAllTaxAncestors
#define TaxDB_IsMerged                 SHoundIsTaxMerged
#define TaxDB_IsDeleted                SHoundIsTaxDeleted
#define TaxDB_GetNameFromTaxIDByClass  SHoundGetTaxNameFromTaxIDByClass
#define TaxDB_GetNameFromTaxID         SHoundGetTaxNameFromTaxID
#define TaxDB_GetLineageFromTaxId      SHoundGetTaxLineargeFromTaxID
#define TAXDB_GetLineage_FULL          SHoundGetTaxLineage_FULL
#define TAXDB_GetLineage_GENBANK       SHoundGetTaxLineage_GENBANK
#define TAXDB_GetLineage_TAXDBSHORT    SHoundGetTaxLineage_TAXDBSHORT

#ifdef __cplusplus
/* { */ }
#endif


#endif

/*
$Log: seqhound.h,v $
Revision 1.36  2005/04/29 19:56:03  rcavero
New Module GOA will replace LL

Revision 1.35  2005/03/02 16:47:24  eburgess
Added API calls SHoundRNAFromChromosome, SHoundRNAFromOrganism,
SHoundRNAFromUnknownChromosome, and SHoundRNAFromOrganismList. Bug 4914

Revision 1.34  2005/02/01 16:45:40  eburgess
Added a new define for comgen: CHROM_PLASTID. Use the unused byteflag 0x20.

Revision 1.33  2005/01/06 19:18:24  zwang
Added two new APIs - SHoundStartTransaction and SHoundEndTransaction - for users to put their process into a transaction.

Revision 1.32  2004/11/22 14:56:59  mmatan

Modified shoundgetfixed3d so that is takes the same parameters as shoundget3dex.
See bug 3930 for details.

Revision 1.31  2004/11/18 19:05:45  mmatan
Adding SHoundGetFixed3D to the interface.  See bug 3930.

Revision 1.30  2004/09/17 15:27:48  eburgess
Added a number of defines for various NCBI structures, like BioSource.

Revision 1.29  2004/08/12 17:55:29  zwang
New API added to get title by gi from ACCDB

Revision 1.28  2004/07/09 15:46:37  eburgess
Added defines for molecule type accoding to the BIND specification. Defines
are of the type BIND_XXXXXX.

Revision 1.27  2004/05/17 19:36:42  hlieu
fix prototype

Revision 1.26  2004/05/14 15:24:08  hlieu
fixed prototype error

Revision 1.25  2004/05/13 20:48:13  hfeldman
Fixed incorrect header

Revision 1.24  2004/05/13 17:39:13  hlieu
major refactor of locus link module. Primary key changed from gi to
locus id. ASN source code also required update.

Revision 1.23  2004/04/08 12:49:34  ryao
Added SHoundGetFastaFromRedundantGroupID SHoundGetFastaFromRedundantGroupIDKeyedList.

Revision 1.22  2004/04/05 19:06:50  ryao
Added SHoundGetFastaKeyedList SHoundRedundantGroupKeyedList StrArray2FastaValNodeList

Revision 1.21  2004/01/13 21:49:56  rcavero
nblast_odbc.h it's a draf version for nblast's ODBC port.

Revision 1.20  2004/01/07 20:45:14  hlieu
Added SHoundGetLargestMMDBID

Revision 1.19  2003/12/08 17:04:04  hlieu
Added function declarations to silence warning of implicit declaration

Revision 1.18  2003/11/18 16:02:25  renan
Add Macro definitions for DBLayer's return code. (SH_SUCCESS, SH_ERROR, SH_NODATA)

Revision 1.17  2003/11/13 19:09:23  renan
Update Header files for DBXref Module

Revision 1.16  2003/11/11 22:47:03  feldman
Fixed missing const in header

Revision 1.15  2003/11/03 20:40:09  ryao
added SHoundPDBFromMMDB allow user to query pdbid from given mmdbid

Revision 1.14  2003/10/31 22:36:17  haocl
changed prototype of protein report set description

Revision 1.13  2003/10/24 17:53:12  feldman
Added SHoundGet3DEx function to allow user to choose model level and num models when fetching biostruc

Revision 1.12  2003/10/01 19:32:28  renan
DBXref Module Database layer Header File.

Revision 1.11  2003/09/29 21:20:32  haocl
merged [obj]ll.h, [obj]shproteinreport.h into [obj]slristruc.h,
changed [obj]NBlastasn.h, ll_db.h, [obj]NBlastasn.h, [obj]slritax.h, seqhound.h
to reflect deleted file

Revision 1.10  2003/09/16 21:09:33  haocl
changed protein report asn, added bit flags in seqhound.h

Revision 1.9  2003/09/03 19:45:56  volodya
added prototype for SHoundGetNameByGi

Revision 1.8  2003/09/03 17:51:16  ryao
changed prototype to SHoundMoleculeType

Revision 1.7  2003/08/26 20:54:57  haocl
changed GOPMIDFromGi to GiAndGOID

Revision 1.6  2003/08/26 20:31:45  haocl
changed SHoundGOECodeFromGi to GiAndGOID

Revision 1.5  2003/08/26 19:14:16  haocl
updated size of PMID

Revision 1.4  2003/08/25 19:53:43  haocl
fixed SHoundGetAccFromRetiredGi

Revision 1.3  2003/08/22 15:23:37  haocl
changed GetClassification back to char *

Revision 1.2  2003/08/22 13:34:10  haocl
added SearchHISTDB and SHoundGetAccFromRetiredGi

Revision 1.1.1.16  2003/08/21 08:48:09  root
Automated import from Sourceforge.net

Revision 1.18  2003/08/20 17:12:36  haocl
updating GetClassification to return a ValNodePtr

Revision 1.17  2003/08/13 17:16:07  haocl
added SHoundSequenceLength

Revision 1.16  2003/07/23 16:22:58  haocl
redefined PROTEIN, DNA, RNA to match the web interface

Revision 1.15  2003/07/22 20:26:14  haocl
defines and prototype for SHoundMoleculeType

Revision 1.14  2002/09/27 16:48:29  kaca
added read only mode for SHound initialization

Revision 1.13  2002/09/03 22:10:10  micheld
removed slri.h include from includes (executable header include)

Revision 1.12  2002/05/16 14:48:42  kaca
fixed rps include name

Revision 1.11  2002/05/13 18:01:47  haocl
added rpsapi.h to file

Revision 1.10  2002/03/25 07:51:32  micheld
New genome functions. changed includes to new slrilinkset.h

Revision 1.9  2002/02/12 22:25:57  micheld
fixed compile bugs
NOTE: you now must have DB2DIR as an exported environment variable!

Revision 1.8  2002/02/06 23:56:45  micheld
Changed TaxDB_ Functions to SHound*Tax* function call, legacy preserved for C api but *NOT* PERL

Revision 1.7  2002/01/15 17:26:40  kaca
disabled SHoundGODBGetHierarchyLevel due to wrong logic

Revision 1.6  2002/01/15 16:08:04  kaca
added declaration of PrintSLRIFastaSequence

Revision 1.5  2001/12/21 22:35:04  micheld
Added taxon, protein, dna, redund Iterator functions with user callback

Revision 1.4  2001/12/19 23:24:40  gbader
Fixed SHoundGODBGetClassification comments

Revision 1.3  2001/12/05 00:52:07  micheld
Added access to BLAST pairwise results... returns either NBLASTDB format or seqalign

Revision 1.2  2001/11/13 21:47:35  kaca
new defline and goid annotation functions

Revision 1.1.1.2  2001/10/10 21:53:47  kaca
initial import

Revision 1.24  2001/09/21 17:40:39  kaca
removed obsolete GODBGetRecordByID

Revision 1.23  2001/09/04 21:33:57  michel
Added local equivalent of the set and get timeout functions for local API (which do nothing)
Removed compiler warning in ll_db.h

Revision 1.22  2001/09/04 05:36:51  michel
Added user functions for user to change connnection settings for seqhound remote api

Revision 1.21  2001/08/30 19:53:26  haocl
added comments for remote gene ontology and locus link functions

Revision 1.20  2001/08/30 18:19:31  haocl
changed implemenation of SHoundGODBGetClassification to return CharPtr

Revision 1.19  2001/08/29 16:36:02  haocl
renamed SHoundECodeFromGi

Revision 1.18  2001/08/27 20:34:49  haocl
added prototypes to locuslink

Revision 1.17  2001/08/27 19:18:23  haocl
added gene ontology prototypes

Revision 1.16  2001/08/24 20:59:44  michel
removed SEQHOUNDNBRS compile flag from library
added new functions for 3DNeighbours database retrieval
temporary fix for redundant neighbours coming from NBLAST

Revision 1.15  2001/08/15 18:11:47  michel
Added pathnbr for config file to find neighbour database
Added additional functions and error checking

Revision 1.14  2001/08/07 18:54:49  michel
Tested SHoundNeighboursFromGiEx
Added the rest of the remote API and CGI calls (untested)

Revision 1.13  2001/08/02 18:24:25  michel
Added #ifdef SEQHOUNDNBRS to exlude neighbour database and calls until
testing is complete and databases are fully available
Added path info for remotelibraries

Revision 1.12  2001/08/02 17:24:26  michel
fixed invalid name for linux include

Revision 1.11  2001/08/02 16:13:32  michel
Added Neighbour Retrieval to SeqHound local

Revision 1.10  2001/06/21 16:15:21  kaca
all functions tested, removed comments

Revision 1.9  2001/06/07 19:13:02  kaca
added Get*FromTaxIDIII declaration

Revision 1.8  2001/05/28 21:33:46  kaca
parameter name change

Revision 1.7  2001/04/27 23:11:37  kaca
added SHoundFindName and SHoundFindNameList functions

Revision 1.6  2001/04/27 22:02:17  kaca
bug fix

Revision 1.5  2001/04/12 21:01:42  kaca
moved function declaration from intrez.h to seqhound.h

Revision 1.4  2001/04/06 22:42:52  kaca
added SHoundGiFromPDBchain

Revision 1.3  2001/03/27 21:23:26  kaca
removed function declaration duplicates

Revision 1.2  2001/03/26 22:33:13  kaca
created intrez_misc file and moved some API functions into it

Revision 1.1  2001/03/26 19:46:02  kaca
reorganized headers to support a single header for the local and remote APIs

*/
