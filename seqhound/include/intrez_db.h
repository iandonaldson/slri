/* $Id: intrez_db.h,v 1.23 2005/03/14 15:55:25 eburgess Exp $
*******************************************************************************************
  Copyright (C) 2000 Mount Sinai Hospital (MSH)

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of
  the License, or any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
xxxx
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
  header file for SeqHound direct CodeBase calls


******************************************************************************************/

#ifndef _INTREZ_DB_
#define _INTREZ_DB_


#include <bzlib.h>
#include <slri_post.h>
#include <ncbi.h>
#include <mmdbapi.h>
#include <objslristruc.h>
#include <objslrilibstruc.h>
#include <objcdd.h>

#ifdef __cplusplus
extern "C" { /* } */
#endif

/* Functions */
Boolean LIBCALL InitCodeBase(void);
Boolean LIBCALL InitCodeBaseReadOnly(void);
Boolean LIBCALL SetCodeBaseFlagsExclNoidx(void);
Boolean LIBCALL SetCodeBaseAutoIdxTrue(void);
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
Boolean LIBCALL OpenREDUNDNoidx(CharPtr);
Boolean LIBCALL ReindexREDUND(void);
Boolean LIBCALL OpenDOMDB(CharPtr name, Boolean Create);
Boolean LIBCALL OpenVASTDB(CharPtr name, Boolean Create);
Boolean LIBCALL OpenNRBIOSEQ(CharPtr name, Boolean Create);
Boolean LIBCALL OpenCDDB(CharPtr name, Boolean Create);
Boolean LIBCALL OpenRPSDB(CharPtr name, Boolean Create);
Boolean LIBCALL OpenDOMNAME(CharPtr name, Boolean Create);

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
Boolean LIBCALL PrintCDDB(void);
Boolean LIBCALL PrintRPSDB(void);
Boolean LIBCALL PrintDOMNAME(void);

Boolean LIBCALL AppendRecordASNDB(StAsndbPtr pas);
Boolean LIBCALL AppendRecordPARTI(StPartiPtr ppa);
Boolean LIBCALL AppendRecordNUCPROT(StNucprotPtr pnc);
Boolean LIBCALL AppendRecordACCDB(StAccdbPtr pac);
Boolean LIBCALL AppendRecordPUBSEQ(StPubseqPtr pps, Int2 choice);
Boolean LIBCALL AppendRecordTAXGI(StTaxgiPtr ptg);
Boolean LIBCALL AppendRecordSOURCEFILE( StSourcefilePtr pRecord );
Boolean LIBCALL AppendRecordSENDB(StSendbPtr psd);
Boolean LIBCALL AppendRecordSENGI(StSengiPtr psg);
Boolean LIBCALL AppendRecordMMDB(StMmdbPtr pmd);
Boolean LIBCALL AppendRecordMMGI(StMmgiPtr pmg);
Boolean LIBCALL AppendRecordREDUND(StRedundPtr prd);
Boolean LIBCALL AppendRecordDOMDB(StDomdbPtr pdd);
/*Boolean LIBCALL AppendRecordVASTDB(StVastdbPtr pvd);*/
Boolean LIBCALL AppendRecordCDDB(StCddbPtr  pcd);
Boolean LIBCALL AppendRecordRPSDB(StRpsdbPtr prs);
Boolean LIBCALL AppendRecordDOMNAME(StDomNamedbPtr dnp);

Boolean LIBCALL EditRecordACCDBByGI(StAccdbPtr pac, Boolean overwrite);
Boolean LIBCALL EditRecordTAXGIByGI(StTaxgiPtr ptg, Boolean overwrite);
Boolean LIBCALL EditRecordTAXGIByKloodge( Int4 oldKloodge, Int4 newKloodge); /* Set all records with oldKloodge to newKloodge */
Boolean LIBCALL EditTAXGIKloodgeByGi( Int4 gi, Int4 kloodge ); /* Set the kloodge for a given gi. Do not search on gi first. */
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
Boolean LIBCALL DeleteGIinCDDB( Int4 gi, Boolean destroy);
Boolean LIBCALL DeleteGIinRPSDB(Int4 gi, Boolean destroy);
Boolean LIBCALL DeleteAccinDOMNAME(CharPtr, Boolean destroy);
Boolean LIBCALL DeleteAllRecordsInREDUND();
Boolean LIBCALL DeleteAllRecordsInDOMDB();

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
Boolean LIBCALL CloseCDDB(void);
Boolean LIBCALL CloseNRBIOSEQ(void);
Boolean LIBCALL CloseCodeBase(void);
Boolean LIBCALL CloseRPSDB(void);
Boolean LIBCALL CloseDOMNAME(void);


/* Database search functions */
Int2        LIBCALL SearchASNDB(StAsndbPtr PNTR);
Int2        LIBCALL SearchPARTI (StPartiPtr PNTR);
Int2        LIBCALL SearchNUCPROT (StNucprotPtr PNTR);
Int2        LIBCALL SearchACCDB (StAccdbPtr PNTR);
Int2        LIBCALL SearchACCDBByAccess (StAccdbPtr PNTR);
Int2        LIBCALL SearchACCDBByGi (StAccdbPtr PNTR);
StAccdbPtr  LIBCALL SearchACCDBByName( CharPtr db, CharPtr pName );
Int2        LIBCALL SearchPUBSEQ (StPubseqPtr PNTR);
Int2        LIBCALL SearchTAXGI (StTaxgiPtr PNTR);
Int2        LIBCALL SearchSENDB (StSendbPtr PNTR);
Int2        LIBCALL SearchSENGI (StSengiPtr PNTR);
Int2        LIBCALL SearchSENDBforGI(StSendbPtr PNTR, Int4);
Int2        LIBCALL SearchMMDB (StMmdbPtr PNTR);
Int2        LIBCALL SearchMMDBEx (StMmdbPtr PNTR ,Int4, Int4);
Int2        LIBCALL SearchMMGI (StMmgiPtr PNTR);
Int2        LIBCALL SearchDOMDB(StDomdbPtr PNTR);
Int2        LIBCALL SearchREDUND (StRedundPtr PNTR);
/*Int2         LIBCALL SearchVASTDB(StVastdbPtr PNTR);*/
Int2 	    LIBCALL SearchCDDB(StCddbPtr PNTR ppcd);
Int2		LIBCALL SearchRPSDB(StRpsdbPtr PNTR pprs);
Int2		LIBCALL SearchDOMNAME(StDomNamedbPtr PNTR ppdm);
ValNodePtr 	LIBCALL SearchTAXGIProt(Int4 taxid);
ValNodePtr 	LIBCALL SearchTAXGIDNA(Int4 taxid);
ValNodePtr  LIBCALL SearchTAXGIByKloodgeForNA(Int4 kloodge);
ValNodePtr  LIBCALL SearchTAXGIByKloodgeForProt(Int4 kloodge);

Int4        LIBCALL SHoundFindGiByAcc(CharPtr pcAcc);
CharPtr     LIBCALL SHoundFindAccessByGi(Int4 gi);

CharPtr LIBCALL GetDivisionFromPARTI( Int4 gi );


/* reset the database asntool generated structures */
void LIBCALL ResetStASNDB(StAsndbPtr PNTR ppas);
void LIBCALL ResetStPARTI(StPartiPtr PNTR pppa);
void LIBCALL ResetStNUCPROT(StNucprotPtr PNTR ppnc);
void LIBCALL ResetStACCDB(StAccdbPtr PNTR ppac);
void LIBCALL ResetStPUBSEQ(StPubseqPtr PNTR ppps);
void LIBCALL ResetStTAXGI(StTaxgiPtr PNTR pptg);
void LIBCALL ResetStSENDB(StSendbPtr PNTR ppsd);
void LIBCALL ResetStSENGI(StSengiPtr PNTR ppsg);
void LIBCALL ResetStMMDB(StMmdbPtr PNTR ppmd);
void LIBCALL ResetStMMGI(StMmgiPtr PNTR ppmg);
void LIBCALL ResetStREDUND(StRedundPtr PNTR pprd);
void LIBCALL ResetStDOMDB(StDomdbPtr PNTR ppdd);
void LIBCALL ResetStCDDB (StCddbPtr PNTR pcdp);
void LIBCALL ResetStRPSDB (StRpsdbPtr PNTR pprs);
void LIBCALL ResetStDOMNAME (StDomNamedbPtr PNTR ppdn);

/*void LIBCALL ResetStVASTDB(StVastdbPtr PNTR ppvd);*/

/* ASN.1 storage into MMDB using bzip compression */
Boolean AssignASNMemBZMemo(Pointer pASNdata, AsnWriteFunc pAsnWrite);

/* ASN.1 retrieval */
BiostrucPtr    LIBCALL GetBiostrucfromMMDB(Int4 mdlLvl, Int4 maxModels);
BioseqPtr      LIBCALL GetBioseqfromASNDB(void);
SeqEntryPtr    LIBCALL GetSeqEntryfromSENDB(void);
CharPtr        LIBCALL GetTitlefromACCDB(void);
SLRIValNodePtr LIBCALL GetDomainfromDOMDB(void);
/*BiostrucFeaturePtr LIBCALL GetFeaturefromVASTDB(void);*/
SLRIFastaPtr   LIBCALL GetSLRIFastaCDFromCDDB(void);
CddPtr		LIBCALL GetCddFROMDOMNAME(void);
/* rather specialized stuff for parsers: fastadom, redund, sendb, precompute, text indexer */
ValNodePtr     LIBCALL SHoundAllMMDBID(void);
Int4           LIBCALL SHoundRecNoDOMDB(void);
Int4           LIBCALL SHoundRecNoREDUND(void);
Int4           LIBCALL SHoundRecNoSENGI(void);
StDomdbPtr     LIBCALL SHoundGetRecDOMDB(Int4 recno);
StRedundPtr    LIBCALL SHoundGetRecREDUND(Int4 recno);
Int4           LIBCALL GetRecNoACCDB(void);


/* lightweight versions of SHound*FromTaxID */
ValNodePtr LIBCALL SHoundProteinsFromTaxIDII(Int4 taxid);
ValNodePtr LIBCALL SHoundDNAFromTaxIDII(Int4 taxid);


/* specialized SeqEntry retrieval for Text Index */
/*static Boolean LIBCALL ExpandSeqEntryforTindex(SeqEntryPtr PNTR ppse, Int4 Gi);*/
SeqEntryPtr    LIBCALL GetSeqEntryfromSENDBforTindex(Int4 Gi);
Int2           LIBCALL SearchSENDBforGIforTindex(StSendbPtr PNTR ppsd, Int4 gi);

/*********************************************************
  Names functions.
*********************************************************/
Boolean LIBCALL InitNAMES(); 
Boolean LIBCALL CloseNAMES();

Boolean LIBCALL InitBioentity();
Boolean LIBCALL AppendRecordBioentity( StBioentityPtr pRecord );
Boolean LIBCALL SearchBioentityByGi( StBioentityPtr PNTR pRecord );
Boolean LIBCALL SearchBioentityByAccess( StBioentityPtr PNTR pRecord );				      
Boolean LIBCALL DeleteGIinBioentity(Int4  gi, Boolean destroy);
Int4    LIBCALL GetLargestBioentityId();
Boolean LIBCALL EditBioentityById( StBioentityPtr pRecord );

Boolean LIBCALL InitBioname();
Boolean LIBCALL AppendRecordBioname( StBionamePtr pRecord );
Boolean LIBCALL SearchBionameByBioentityId ( StBionamePtr PNTR ppREcord );
Boolean LIBCALL EditBionameById( StBionamePtr pRecord );
Boolean LIBCALL DeleteGIinBioname( Int4 gi, Boolean destroy);


/*********************************************************
  COMGEN functions.
*********************************************************/
Boolean LIBCALL InitCOMGEN(); 
Boolean LIBCALL CloseCOMGEN();

Boolean LIBCALL AppendRecordCHROM(StChromPtr pch);
Boolean LIBCALL AppendRecordGICHROMID( StGichromidPtr pRecord );
Boolean LIBCALL ReplaceRecordGICHROMID( StGichromidPtr pRecord );
Boolean LIBCALL AppendRecordCONTIGCHROMOSOME( StContigchromPtr pRecord );
Boolean LIBCALL AppendRecordGICHROMOSOME( StGichromPtr pRecord);
Boolean LIBCALL ReplaceRecordCONTIGCHROMID( StContigchromidPtr pRecord );

Boolean LIBCALL EditRecordCONTIGCHROMIDUpdate( Int4 gi, Int4 update);
Boolean LIBCALL EditRecordCONTIGCHROMIDUpdateAll();

Int2               LIBCALL SearchCHROM (StChromPtr PNTR);
Int4               LIBCALL SearchCHROMByChromId(Int4 chromid);
Int4               LIBCALL SearchCHROMByChromNum(CharPtr xmeNum, Int4* chromId, Int4 taxId); 
StGichromidPtr     LIBCALL SearchGICHROMIDByGi( Int4 gi );
StGichromidPtr     LIBCALL SearchGICHROMIDByContigGi( Int4 gi );
StContigchromidPtr LIBCALL SearchCONTIGCHROMIDByTopGi( Int4 gi );

Boolean LIBCALL DeleteAccessInCHROM( CharPtr access );
Boolean LIBCALL DeleteChromidInGICHROMID( Int4 chromid );
Boolean LIBCALL DeleteGiInGICHROMID( Int4 gi);
Boolean LIBCALL DeleteContigGiInGICHROMID( Int4 gi);
Boolean LIBCALL DeleteTopGiInCONTIGCHROMID( Int4 gi);
Boolean LIBCALL DeleteGiInGichromosome( Int4 gi );
Boolean LIBCALL DeleteChromidInContigchromid( Int4 chromid );
Boolean LIBCALL DeleteGiInContigchromid( Int4 gi );
Boolean LIBCALL DeleteAllCONTIGCHROMOSOME();
Boolean LIBCALL DeleteAllGICHROMOSOME();
Boolean LIBCALL DeleteAllRecordsInCHROM();

/* Functions to remove genbank records from comgen tables */
Boolean LIBCALL RemoveGenBankRecordsFromGICHROMID();
Boolean LIBCALL RemoveGenBankRecordsFromCONTIGCHROMID();
Boolean LIBCALL RemoveGenBankRecordsFromGICHROMOSOME();
Boolean LIBCALL RemoveGenBankRecordsFromCONTIGCHROMOSOME();

StGichromidPtr     LIBCALL GetAllRecordsFromGICHROMID();
StContigchromidPtr LIBCALL GetAllRecordsFromCONTIGCHROMID();
StGichromidPtr     LIBCALL GetAllRecordsFromGICHROMOSOME();
StContigchromidPtr LIBCALL GetAllRecordsFromCONTIGCHROMOSOME();

Boolean LIBCALL AddChromIdsToGICHROMID();
Boolean LIBCALL AddChromIdsToCONTIGCHROMID();

void LIBCALL ResetStCHROM(StChromPtr PNTR ppch);
void LIBCALL ResetStGICHROMID(StGichromidPtr PNTR pRecord);
void LIBCALL ResetStCONTIGCHROMID(StContigchromidPtr PNTR pRecord);
void LIBCALL ResetStGICHROM(StGichromPtr PNTR ppch);
void LIBCALL ResetStCONTIGCHROM(StContigchromPtr PNTR ppch);

CharPtr        LIBCALL GetNameFromCHROM(void);
ValNodePtr     LIBCALL SHoundAllGenomes(void);
ValNodePtr     LIBCALL SHoundAllGenAccess(void);
ValNodePtr     LIBCALL SHoundAllKloodge(void);
Int4           LIBCALL SHoundGetKloodgeFromAccess(CharPtr paccess);
Int4           LIBCALL GetKloodgeByAccess(CharPtr paccess);

/*********************************************************/
/* Database Layer Functions that are not cross-supported */
/*********************************************************/

/* CODEBASE specific functions */
ValNodePtr     LIBCALL SHoundAllGI(Int4 from, Int4 to);
ValNodePtr     LIBCALL SHoundAllPartition(CharPtr ppartition);
Int4           LIBCALL getTaxInfo(Int4 taxid, PostingPtr* psp, CharPtr type);
Boolean        LIBCALL IsThisDivision(Int4 gi, CharPtr division);
Int4           LIBCALL GetLargestTaxid(void);
Int4           LIBCALL getTaxInfoII(Int4 taxid, PostingPtr* psp_all,  PostingPtr* psp_na, PostingPtr* psp_prot, Int4 *num_all, Int4 *num_na, Int4 *num_prot);
Int4           LIBCALL getTaxInfoIII(Int4 taxid, PostingPtr* psp_all,  PostingPtr* psp_na, PostingPtr* psp_prot, Int4 *num_all, Int4 *num_na, Int4 *num_prot);
Int4           LIBCALL SHoundGICount(void);


/*Db2 specific functions*/
Boolean LIBCALL Commit_inserts(void);

/*ODBC specific functions*/
Boolean StartTransaction();
Boolean EndTransaction();
Boolean CommitTransaction();
Boolean RollBackTransaction();
Boolean CommitAndRollBack();
Boolean IsAutoCommitOn();

/* the file that holds the largest mmdb id */
#define MMDBID_LARGEST "MMDB.txt"

#ifdef __cplusplus
/* { */ }
#endif


#endif  /* _INTREZ_DB */


/*
 $Log: intrez_db.h,v $
 Revision 1.23  2005/03/14 15:55:25  eburgess
 Added SearchAccdbByName() so that we can find gis associated with a name if
 we do not have an accession during updates. Bug 4939

 Revision 1.22  2005/03/02 16:44:47  eburgess
 Added CommitAndRollBack();

 Revision 1.21  2005/02/01 16:40:08  eburgess
 Added some new comgen functions and changed the names of others.

 Revision 1.20  2005/01/10 19:20:28  eburgess
 Added functions to handle new comgen tables.

 Revision 1.19  2005/01/07 22:38:13  zwang
 Added definition of EndTransaction

 Revision 1.18  2004/12/14 19:56:38  zwang
 Added functions to delete all records from tables CHROM and DOMDB.

 Revision 1.17  2004/11/10 18:33:43  eburgess
 Added code for sourcefile functions and other functions to handle the
 division. Bug 1193

 Revision 1.16  2004/10/27 18:32:07  eburgess
 Added SearchTAXGIByKloodgeForNA() and SearchTAXGIByKloodgeForProt() for
 comgen apis. Bug 2952

 Revision 1.15  2004/10/19 15:02:14  eburgess
 Fixed typo in ResetStGIKLOODGE.

 Revision 1.14  2004/10/15 19:51:43  eburgess
 Added a number of functions for complete genomes, bug 2952.

 Revision 1.13  2004/10/04 14:10:58  zwang
 Added transaction related functions.

 Revision 1.12  2004/09/22 18:19:36  eburgess
 Added EditRecordTAXGIByKloodge() and SHoundFindAccessByGi() for complete
 genomes.

 Revision 1.11  2004/09/17 15:29:06  eburgess
 Added various functions for secondrefs ins names and gikloodge in Comgen.

 Revision 1.10  2004/07/09 15:35:50  eburgess
 Added functions for updating names tables.

 Revision 1.9  2004/06/02 21:16:32  zwang
 Added prototype SHoundFindGiByAcc.

 Revision 1.8  2004/05/03 16:16:14  eburgess
 Added prototypes for names functions.

 Revision 1.7  2004/04/19 14:30:55  zwang
 Move prototype of transaction related functions from intrez_odbc.h to intrez_db.h

 Revision 1.6  2004/03/17 20:02:08  eburgess
 Victor: Added SetCodeBaseAutoIdxTrue() to set the codebase autoOpen flag.

 Revision 1.5  2004/03/09 23:25:05  hfeldman
 changed reindexing of redund

 Revision 1.4  2004/03/09 21:17:27  hfeldman
 added functions for opening redun with no index

 Revision 1.3  2004/01/07 19:19:53  hlieu
 added file support and code for saving largest mmdb id

 Revision 1.2  2003/10/24 17:53:12  feldman
 Added SHoundGet3DEx function to allow user to choose model level and num models when fetching biostruc

 Revision 1.1.1.11  2003/07/26 08:41:52  root
 Automated import from Sourceforge.net

 Revision 1.11  2003/07/25 15:23:29  haocl
 changed prototype of AppendRecordPUBSEQ to allow storing of pmid

 Revision 1.10  2002/11/04 21:56:57  kaca
 added SHoundAllPartition

 Revision 1.9  2002/10/17 19:45:49  kaca
 overhaul of complete genomes; added CHROM table retrieval fncts

 Revision 1.8  2002/09/27 16:48:29  kaca
 added read only mode for SHound initialization

 Revision 1.7  2002/02/28 17:22:59  micheld
 New MSVC project files to build cb/db2/rem apps and libraries
 Moved include order to accomodate msvc, removed #ifdef [CODEBASE|DB2]

 Revision 1.6  2002/02/21 22:07:41  betel
 Addition of RPS results and Cdd tables

 Revision 1.5  2002/02/11 19:35:37  micheld
 Moved SLRI asns from slristruc.asn to slrilib slrilibstruc.asn

 Revision 1.4  2002/02/10 19:55:06  mdumontier
 abstracted internals of SHoundProteinsFromTaxID and SHoundDNAFromTaxID to database layer:wq

 Revision 1.3  2002/02/07 00:26:18  mdumontier
 declarations for db2 specific optimization functions

 Revision 1.2  2001/12/19 23:22:59  kaca
 added DB2 layer code

 Revision 1.1.1.2  2001/10/10 21:53:47  kaca
 initial import

 Revision 1.10  2001/09/03 20:31:08  michel
 Mainly fixed compiler warnings and changed CODE4 reference for GO and LL

 Revision 1.9  2001/07/27 17:58:50  kaca
 modified SHoundAllGI

 Revision 1.8  2001/07/24 19:55:36  kaca
 added specialized SeqEntry retrieval for text index

 Revision 1.7  2001/04/06 22:42:27  kaca
 moved in some functions from precompute

 Revision 1.6  2001/04/03 19:19:06  kaca
 moved CodeBase globals to the library code

 Revision 1.5  2001/03/27 21:09:10  doron
 Addition of CDDB function decl. and struc

 Revision 1.4  2001/03/27 15:44:36  kaca
 added newline at EOF

 Revision 1.3  2001/03/27 15:30:04  kaca
 removed molecule type field from REDUND

 Revision 1.2  2001/03/26 22:33:13  kaca
 created intrez_misc file and moved some API functions into it

 Revision 1.1  2001/03/26 19:46:02  kaca
 reorganized headers to support a single header for the local and remote APIs

*/


