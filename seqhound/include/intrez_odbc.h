/* $Id: intrez_odbc.h,v 1.33 2005/03/14 15:55:01 eburgess Exp $
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
 

  REVISION/CONTRIBUTION NOTES:

  March 1, 2001  Original MSH Revision.
                Hogue Lab - University of Toronto Biochemistry 
                Department and the Samuel Lunenfeld Research 
                Institute, Mount Sinai Hospital
                http://bioinfo.mshri.on.ca  hogue@mshri.on.ca
                ftp://ftp.mshri.on.ca/


  PUBLICATION TO CITE:

****************************************************************************/

#ifndef _INTREZ_ODBC_
#define _INTREZ_ODBC_

#include <slri_odbc.h>
#include <slri_post.h>
#include <ncbi.h>
#include <asn.h>
#include <objslristruc.h>
#include <objslrilibstruc.h>

#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>

#include <bzlib.h>

#include <mmdbapi.h>  /* for BiostrucPtr */

/* for functions SHound... */
#include <intrez.h>

#ifdef __cplusplus
extern "C" { /* } */
#endif


/* ************************************************************ */

 /* SeqHound defined structures: Used for ODBC functions that
    required binding to the parameters or columns of the
    result set. See SQLBindParameter & SQLBindCol
  */

#ifndef SH_IntParam
typedef struct SH_SQLIParam {
  SQLINTEGER val;
} SH_IntParam;
#endif

#ifndef SH_CharParam
typedef struct SH_SQLCParam {
   SQLINTEGER len;
   SQLCHAR val[50];
} SH_CharParam;
#endif

/**********************************************************************/
/* Define Macros for DBLayer ODBC                                     */
#define SH_SUCCESS 1
#define SH_ERROR 0
#define SH_NODATA -1

/**********************************************************************/
/* Size of a TEXT field.       */
#define TEXT_LENGTH 4096

/**********************************************************************/



/* ******************************************************************* */

 /* Global variables used in the SQL statements to bind
    to the SQL statement handles and return values.
    SH_IntParam & SHCharParam defined above.
  */ 

/* row id */
static SH_IntParam rowid;

/* REDUND variables */
static SH_IntParam redund_gi;
static SH_IntParam redund_rordinal;
static SH_IntParam redund_rgroup;

/* PARTI variables */
static SH_IntParam parti_gi;
static SH_CharParam parti_div;
 
/* SENGI variables */
static SH_IntParam sengi_gi;
static SH_IntParam sengi_seid;
static SH_CharParam sengi_div;

/* ACCDB variables */
static SH_IntParam accdb_gi;
static SH_IntParam accdb_version;
static SH_CharParam accdb_db;
static SH_CharParam accdb_access;
static SH_CharParam accdb_chain;
static SH_CharParam accdb_release;
static SH_CharParam accdb_name;
static SH_CharParam accdb_namelow;

/* DO NOT USE THIS FOR NOW, not sure of size constraints */
/* static SH_CharParam accdb_title; */

/* TAXGI variables */
static SH_IntParam taxgi_gi;
static SH_IntParam taxgi_kloodge;
static SH_IntParam taxgi_kloodge2; /* use to set kloodge to a new value */
static SH_IntParam taxgi_taxid;
static SH_CharParam taxgi_type;

/* ASNDB variables */
static SH_IntParam asndb_gi;
static SH_CharParam asndb_release;
static SH_CharParam asndb_div;
static SH_CharParam asndb_type;

/* DO NOT USE THIS FOR NOW, not sure of size constraints */
/* static SH_CharParam asndb_asn; */

/* PUBSEQ variables */
static SH_IntParam pubseq_gi;
static SH_IntParam pubseq_muid;
static SH_IntParam pubseq_pmid;

/* SENDB variables */
static SH_IntParam sendb_seid;

/* DO NOT USE THIS FOR NOW, not sure of size constraints */
/* static SH_CharParam sendb_asn; */

/* NUCPROT variables */
static SH_IntParam nucprot_gin;
static SH_IntParam nucprot_gia;

/* SOURCEFILE varibles */
static SH_IntParam  sourcefile_id;
static SH_IntParam  sourcefile_gi;
static SH_CharParam sourcefile_filename;
static SH_CharParam sourcefile_date;

/* MMDB variables */
static SH_IntParam mmdb_mmdbid;
static SH_CharParam mmdb_pdbid;
static SH_IntParam mmdb_bwhat;
static SH_IntParam mmdb_models;
static SH_IntParam mmdb_molecules;
static SH_IntParam mmdb_size;
static SH_IntParam mmdb_bzsize;

/* MMGI variables */
static SH_IntParam mmgi_mmdbid;
static SH_IntParam mmgi_gi;

/* DOMDB variables */
static SH_IntParam domdb_mmdbid;
static SH_CharParam domdb_pdbid;
static SH_CharParam domdb_chain;
static SH_IntParam domdb_gi;
static SH_IntParam domdb_domno;
static SH_IntParam domdb_domall;
static SH_IntParam domdb_domid;
static SH_IntParam domdb_rep;

/* CDDB variables */
static SH_IntParam cddb_gi;

/* ****************************************************** */

/* handles for each of the databases. */

/* insert */
static SQLHANDLE HNDLAsndb,
          HNDLParti,
          HNDLNucprot,
          HNDLAccdb,
          HNDLSendb,
          HNDLSengi,
          HNDLRedund,
          HNDLTaxgi,
          HNDLMmdb,
          HNDLMmgi,
          HNDLDomdb,

          HNDLCddb;

/* delete */
static SQLHANDLE HNDLAsndbDelete,
          HNDLPartiDelete,
          HNDLNucprotDeleteGin,
          HNDLNucprotDeleteGia,
          HNDLAccdbDelete,
          HNDLSendbDelete,
          HNDLSengiDelete,
          HNDLTaxgiDelete,
          HNDLPubseqDelete;

/* PUBSEQ is special */
static SQLHANDLE HNDLPubseqInsertGMP, /* insert all 3 fields: gi, muid, pmid */
                 HNDLPubseqUpdateM,   /* update muid */
                 HNDLPubseqUpdateP,   /* update pmid */
                 HNDLPubseqSearchGi;  /* search on gi */

/* SENGI is special */
static SQLHANDLE HNDLSengiSearchGi;     /* search SENGI by gi */

static SQLHANDLE HNDLPartiSearchGi; /* search PARTI by gi for division */

/* sourcefile */

  static SQLHANDLE HNDLSourcefileInsert; /* Add a record into sourcefile */


/* Edit functions are special */
static SQLHANDLE HNDLAccdbSearchGi,     /* search ACCDB by gi alone, used in EditRecordACCDBByGI */
                 HNDLAccdbUpdateAll,    /* update all fields */
                 HNDLAccdbUpdateDb,     /* update DB  */
                 HNDLAccdbUpdateName,   /* update NAME */
                 HNDLAccdbUpdateNamelow,/* update NAMELOW */
                 HNDLAccdbUpdateAccess, /* update ACCESS */
                 HNDLAccdbUpdateRelease,/* update RELEASE */
                 HNDLAccdbUpdateVersion,/* update VERSION */
                 HNDLAccdbUpdateChain;  /* update CHAIN */

static SQLHANDLE HNDLAccdbSearchAccess; /* search ACCDB gi by access */
static SQLHANDLE HNDLAccdbGetAccessByGi;/* search ACCDB access by gi */
static SQLHANDLE HNDLAccdbGetGiByName;  /* search ACCDB access by name and db */
                
static SQLHANDLE HNDLTaxgiSearchGi,     /* search TAXGI by gi alone, used in EditRecordTAXGIByGI */
                 HNDLTaxgiUpdateAll,    /* update all fields */
                 HNDLTaxgiUpdateTaxid,  /* update TAXID */
                 HNDLTaxgiUpdateKloodge,/* update KLOODGE */
                 HNDLTaxgiUpdateKloodgeByKloodge,/* update KLOODGE using kloodge as key*/
                 HNDLTaxgiUpdateType;   /* update TYPE */
static SQLHANDLE HNDLTaxgiSearchKloodgeNA; /*Search TAXGI by kloodge and type (DNA or RNA)*/
static SQLHANDLE HNDLTaxgiSearchKloodgeAA; /*Search TAXGI by kloodge and type (protein)*/
                 

static SQLHANDLE HNDLDomdbSearchDomid,  /* search DOMDB by domid alone, used in EditRecordDOMDBByDOMID */
                 HNDLDomdbUpdateAll,    /* update all fields */
                 HNDLDomdbUpdatePdbid,  /* update pdbid */
                 HNDLDomdbUpdateChain,  /* update chain */
                 HNDLDomdbUpdateMmdbid, /* update mmdbid */
                 HNDLDomdbUpdateGi,     /* update gi*/
                 HNDLDomdbUpdateDomno,  /* update domno */
                 HNDLDomdbUpdateDomall, /* update domall */
                 HNDLDomdbUpdateRep;    /* update rep */

/* GetRec by recno are special */
static SQLHANDLE HNDLAccdbSelectCount, /* select count(*) in ACCDB */
                 HNDLAccdbSearchRowid; /* select ACCDB by rowid */

static SQLHANDLE HNDLDomdbSelectCount, /* select count(*) in DOMDB */
                 HNDLDomdbSearchRowid; /* select DOMDB by rowid */

/* other SQLHANDLE */
static SQLHANDLE HNDLSendbSearchGi,
                 HNDLAsndbSearchGi;

/* SQLHENV henv; */ /* ODBC environment handle . Moved to slri_odbc.h*/ 
/* SQLHDBC hdbc; */ /* ODBC connection handle . Moved to slri_odbc.h*/

/* ******************************************************* */

 /* Function prototypes */

Boolean ConnectToDb(void);
Boolean InitASNDB(void);
Boolean InitPARTI(void);
Boolean InitTAXGI(void);
Boolean InitNUCPROT(void);
Boolean InitACCDB(void);
Boolean InitPUBSEQ(void);
Boolean InitSENGI(void);
Boolean InitSENDB(void);
Boolean InitSOURCEFILE(void);
Boolean InitMMDB(void);
Boolean InitMMGI(void);
Boolean InitDOMDB(void);
Boolean InitCDDB(void);
Boolean LIBCALL InitCodeBase(void);
Boolean LIBCALL InitCodeBaseReadOnly(void);
Boolean LIBCALL SetCodeBaseFlagsExclNoidx(void);
Boolean LIBCALL SetCodeBaseAutoIdxTrue(void);
Boolean CloseCodeBase(void);
Boolean StartTransaction();
Boolean EndTransaction();
Boolean CommitTransaction();
Boolean RollBackTransaction();
Boolean CommitAndRollBack();
Boolean IsAutoCommitOn();

Boolean CheckDBConnection(void);

Boolean OpenPARTI(CharPtr name, Boolean Create);
Boolean LIBCALL AppendRecordPARTI(StPartiPtr ppa);
Int2 LIBCALL SearchPARTI (StPartiPtr PNTR pppa);
Boolean LIBCALL IsThisDivision(Int4 gi, CharPtr division);
ValNodePtr LIBCALL SHoundAllPartition(CharPtr ppartition);
Boolean LIBCALL DeleteGIinPARTI(SQLINTEGER gi, Boolean destroy);
CharPtr LIBCALL GetDivisionFromPARTI( Int4 gi );

Boolean LIBCALL OpenREDUND(CharPtr name, Boolean Create);
Boolean AppendRecordREDUND(StRedundPtr prd);
Int2 SearchREDUND(StRedundPtr PNTR pprd);
Boolean DeleteGIinREDUND(SQLINTEGER gi, Boolean destroy);
Int4 LIBCALL SHoundRecNoREDUND(void);
StRedundPtr LIBCALL SHoundGetRecREDUND(Int4 recno);
void LIBCALL ResetStREDUND(StRedundPtr PNTR pprd);
Boolean LIBCALL OpenREDUNDNoidx(CharPtr);
Boolean LIBCALL ReindexREDUND(void);

Boolean LIBCALL OpenSENGI(CharPtr name, Boolean Create);
Boolean LIBCALL AppendRecordSENGI(StSengiPtr psg);
Int2  LIBCALL SearchSENGI (StSengiPtr PNTR ppsg);
Boolean LIBCALL DeleteGIinSENGI(SQLINTEGER gi, Boolean destroy);
Int4 LIBCALL SHoundRecNoSENGI(void);

Boolean LIBCALL OpenACCDB(CharPtr name, Boolean Create);
Boolean LIBCALL AppendRecordACCDB(StAccdbPtr pac);
Int2 LIBCALL SearchACCDB (StAccdbPtr PNTR ppac);
StAccdbPtr LIBCALL SearchACCDBByName( CharPtr db, CharPtr pName );
Int2 LIBCALL SearchACCDBByAccess (StAccdbPtr PNTR ppac);
Int2 LIBCALL SearchACCDBByGi (StAccdbPtr PNTR ppac);
Boolean LIBCALL DeleteGIinACCDB(SQLINTEGER gi, Boolean destroy);
CharPtr LIBCALL  GetTitlefromACCDB(void);
Boolean LIBCALL EditRecordACCDBByGI(StAccdbPtr pac, Boolean overwrite);
Int4 LIBCALL GetRecNoACCDB(void);
StAccdbPtr LIBCALL GetRecACCDB(Int4 recno);
Int4 LIBCALL SHoundFindGiByAcc(CharPtr pcAcc);
CharPtr LIBCALL SHoundFindAccessByGi(Int4 gi);

Boolean LIBCALL OpenTAXGI(CharPtr name, Boolean Create);
Boolean LIBCALL AppendRecordTAXGI(StTaxgiPtr ptg);
Int2 LIBCALL SearchTAXGI (StTaxgiPtr PNTR pptg);
ValNodePtr LIBCALL SearchTAXGIDNA(SQLINTEGER taxid);
ValNodePtr LIBCALL SearchTAXGIProt(SQLINTEGER taxid);
Boolean LIBCALL DeleteGIinTAXGI(SQLINTEGER gi, Boolean destroy);
Boolean LIBCALL EditRecordTAXGIByGI(StTaxgiPtr ptg, Boolean overwrite);
Boolean LIBCALL EditRecordTAXGIByKloodge(Int4 oldKloodge, 
										 Int4 newKloodge); /* Set all records with oldKloodge to newKloodge */
Boolean LIBCALL EditTAXGIKloodgeByGi( Int4 gi, Int4 kloodge ); /* Set the kloodge for a given gi. Do not search on gi first. */
ValNodePtr LIBCALL SearchTAXGIByKloodgeForNA(Int4 kloodge);
ValNodePtr LIBCALL SearchTAXGIByKloodgeForProt(Int4 kloodge);

Int4 LIBCALL GetLargestTaxid(void);
Int4 LIBCALL getTaxInfoIII(Int4 taxid, PostingPtr* psp_all,  PostingPtr* psp_na, PostingPtr* psp_prot, Int4 *num_all, Int4 *num_na, Int4 *num_prot);

Boolean LIBCALL OpenASNDB(CharPtr name, Boolean Create);
Boolean LIBCALL AppendRecordASNDB(StAsndbPtr pas);
Int2 LIBCALL SearchASNDB(StAsndbPtr PNTR ppas);
Boolean LIBCALL DeleteGIinASNDB(CharPtr div, SQLINTEGER gi, Boolean destroy);

Boolean LIBCALL OpenPUBSEQ(CharPtr name, Boolean Create);
Boolean LIBCALL AppendRecordPUBSEQ(StPubseqPtr pps, Int2 choice);
Int2 LIBCALL SearchPUBSEQ (StPubseqPtr PNTR ppps);
Boolean LIBCALL DeleteGIinPUBSEQ(SQLINTEGER gi, Boolean destroy);
static Boolean AddNewRecordPUBSEQ(StPubseqPtr pps, Int2 choice);
static Boolean UpdatePUBSEQ (StPubseqPtr pps, Int2 choice);

Boolean LIBCALL OpenSENDB(CharPtr name, Boolean Create);
Boolean LIBCALL AppendRecordSENDB(StSendbPtr psd);
Int2 LIBCALL SearchSENDB(StSendbPtr PNTR ppsd);
Boolean LIBCALL DeleteGIinSENDB(CharPtr div, SQLINTEGER gi, Boolean destroy);
Int2 LIBCALL SearchSENDBforGIforTindex(StSendbPtr PNTR ppsd, Int4 gi);
Int2 LIBCALL SearchSENDBforGI(StSendbPtr PNTR ppsd, SQLINTEGER gi);

Boolean LIBCALL OpenNUCPROT(CharPtr name, Boolean Create);
Boolean LIBCALL AppendRecordNUCPROT(StNucprotPtr pnc);
Int2 LIBCALL SearchNUCPROT (StNucprotPtr PNTR ppnc);
Boolean LIBCALL DeleteGIinNUCPROT(SQLINTEGER gi, Boolean destroy);

Boolean LIBCALL DeleteAllRecordsInDOMDB();

Boolean LIBCALL AppendRecordSOURCEFILE( StSourcefilePtr pRecord );

Boolean LIBCALL OpenMMDB(CharPtr name, Boolean Create);
Boolean LIBCALL AppendRecordMMDB(StMmdbPtr pmd);
Int2 LIBCALL SearchMMDB(StMmdbPtr PNTR ppmd);
void LIBCALL ResetStMMDB(StMmdbPtr PNTR ppmd);
ValNodePtr LIBCALL SHoundAllMMDBID(void);
Int4 LIBCALL GetLargestMmdbid(void);


Boolean LIBCALL OpenMMGI(CharPtr name, Boolean Create);
Boolean LIBCALL AppendRecordMMGI(StMmgiPtr pmg);
Int2 LIBCALL SearchMMGI(StMmgiPtr PNTR ppmg);
void LIBCALL ResetStMMGI(StMmgiPtr PNTR ppmg);


Boolean LIBCALL OpenDOMDB(CharPtr name, Boolean Create);
Boolean LIBCALL AppendRecordDOMDB(StDomdbPtr pdd);
Int2 LIBCALL SearchDOMDB(StDomdbPtr PNTR ppdd);
Boolean LIBCALL EditRecordDOMDBByDOMID(StDomdbPtr pdd, Boolean overwrite);
Int4 LIBCALL SHoundRecNoDOMDB(void);
StDomdbPtr LIBCALL SHoundGetRecDOMDB(Int4 recno);
void LIBCALL ResetStDOMDB(StDomdbPtr PNTR ppdd);

Boolean LIBCALL OpenCDDB(CharPtr name, Boolean Create);
Boolean LIBCALL AppendRecordCDDB(StCddbPtr pcd);
Boolean LIBCALL DeleteGIinCDDB(SQLINTEGER gi, Boolean destroy);
Int2 LIBCALL SearchCDDB(StCddbPtr PNTR ppcd);
void LIBCALL ResetStCDDB(StCddbPtr PNTR ppcd);

Boolean LIBCALL CloseASNDB(void);
Boolean LIBCALL ClosePARTI(void);
Boolean LIBCALL CloseNUCPROT(void);
Boolean LIBCALL CloseACCDB(void);
Boolean LIBCALL ClosePUBSEQ(void);
Boolean LIBCALL CloseTAXGI(void);
Boolean LIBCALL CloseREDUND(void);
Boolean LIBCALL CloseSENDB(void);
Boolean LIBCALL CloseSENGI(void);
Boolean LIBCALL CloseMMDB(void);
Boolean LIBCALL CloseMMGI(void);
Boolean LIBCALL CloseDOMDB(void);
Boolean LIBCALL CloseCDDB(void);
Boolean LIBCALL CloseVASTDB(void);

static Boolean LIBCALL ExpandSeqEntry(SeqEntryPtr PNTR ppse);
SeqEntryPtr LIBCALL GetSeqEntryFromSENDB(CharPtr pcASN, Int4 iSize);

void LIBCALL ResetStSENGI(StSengiPtr PNTR ppsg);
void LIBCALL ResetStNUCPROT(StNucprotPtr PNTR ppnc);
void LIBCALL ResetStTAXGI(StTaxgiPtr PNTR pptg);
void LIBCALL ResetStPUBSEQ(StPubseqPtr PNTR ppps);
void LIBCALL ResetStACCDB(StAccdbPtr PNTR ppac);
void LIBCALL ResetStASNDB(StAsndbPtr PNTR ppas);
void LIBCALL ResetStPARTI(StPartiPtr PNTR pppa);
void LIBCALL ResetStSENDB(StSendbPtr PNTR ppsd);


ValNodePtr LIBCALL SearchTAXGIRNA(Int4 taxid);

/******************************************
functions used in intrez.c
******************************************/
ValNodePtr LIBCALL SHoundProteinsFromTaxIDII(Int4 taxid);
Int2 SearchMMDBEx(StMmdbPtr PNTR ppmd,Int4 mdlLvl, Int4 maxModels);
ValNodePtr LIBCALL SHoundDNAFromTaxIDII(Int4 taxid);




#ifdef __cplusplus
/* { */ }
#endif


#endif  /* _INTREZ_ODBC */

/* $Log: intrez_odbc.h,v $
/* Revision 1.33  2005/03/14 15:55:01  eburgess
/* Added SearchAccdbByName() so that we can find gis associated with a name if
/* we do not have an accession during updates. Bug 4939
/*
/* Revision 1.32  2005/03/02 16:43:45  eburgess
/* Added CommitAndRollBack().
/*
/* Revision 1.31  2005/02/01 16:38:32  eburgess
/* Made many changes for comgen. Moved many comgen functions to comgen_odbc.c.
/*
/* Revision 1.30  2005/01/10 19:21:20  eburgess
/* Added functions to handle new comgen tables.
/*
/* Revision 1.29  2005/01/07 22:56:23  zwang
/* Added definition of transaction related functions.
/*
/* Revision 1.28  2004/12/14 19:55:27  zwang
/* Added functions to delete all records from tables CHROM and DOMDB.
/*
/* Revision 1.27  2004/11/10 18:33:12  eburgess
/* Added code for sourcefile functions and other functions to handle the
/* division. Bug 1193
/*
/* Revision 1.26  2004/10/27 18:31:25  eburgess
/* Added SearchTAXGIByKloodgeForNA() and SearchTAXGIByKloodgeForProt() for
/* comgen apis. Bug 2952
/*
/* Revision 1.25  2004/10/15 19:53:14  eburgess
/* Added a number of functions, variables and handles for complete genomes,
/* bug 2952.
/*
/* Revision 1.24  2004/09/22 18:20:02  eburgess
/* Added EditRecordTAXGIByKloodge() and SHoundFindAccessByGi() for complete
/* genomes.
/*
/* Revision 1.23  2004/09/17 15:29:28  eburgess
/* Added various functions for secondrefs ins names and gikloodge in Comgen.
/*
/* Revision 1.22  2004/08/05 15:51:58  zwang
/* Added more SQLHANDLE
/*
/* Revision 1.21  2004/07/20 15:22:50  rcavero
/* Remove C++ Comments
/*
/* Revision 1.20  2004/07/09 15:38:33  eburgess
/* Added a define for the size of a TEXT field: TEXT_LENGTH 4096.
/*
/* Revision 1.19  2004/06/02 21:16:49  zwang
/* Added prototype InitCodeBaseReadOnly.
/*
/* Revision 1.18  2004/04/19 14:30:55  zwang
/* Move prototype of transaction related functions from intrez_odbc.h to intrez_db.h
/*
/* Revision 1.17  2004/04/15 21:18:40  zwang
/* Added more SQLHandle. Added functions for SQL Transaction.
/*
/* Revision 1.16  2004/03/17 20:04:55  eburgess
/* Added SetCodeBaseAutoIdxTrue() prototype for compatability to codebase.
/*
/* Revision 1.15  2004/03/15 19:47:51  eburgess
/* Added functions OpenREDUNDNoIdx(), ReindexREDUND() and
/* SetCodeBaseFlagsExclNoids() to mirror Codebase functions.
/*
/* Revision 1.14  2004/03/15 17:46:37  zwang
/* Added function to get the largest mmdbid
/*
/* Revision 1.13  2004/02/27 17:35:16  zwang
/* Added more SQLHandle.
/*
/* Revision 1.12  2004/02/18 17:17:30  zwang
/* Added more handles for delete functions.
/*
/* Revision 1.11  2004/02/12 19:54:14  zwang
/* More SQLHANDLE for ACCDB and DOMDB.
/*
/* Revision 1.10  2004/02/03 22:50:59  zwang
/* Added more SQLHANDLE.
/*
/* Revision 1.9  2004/01/21 22:09:18  zwang
/* Removed functions copied from intrez.c
/* Include intrez.h
/* Added functions called in intrez.c (they simple call some existing functions.)
/*
/* Revision 1.8  2004/01/15 19:27:07  zwang
/* Speed up inserting records by moving handle creations in AppendRecordPUBSEQ to InitPUBSEQ and changing the code accordingly.
/*
/* Revision 1.7  2004/01/14 16:03:28  zwang
/* Added namelow for accdb. It was missing before.
/*
/* Revision 1.6  2004/01/13 22:27:13  zwang
/* Added more functions from intrez.c used in addgoid.c
/*
/* Revision 1.5  2004/01/13 21:49:55  rcavero
/* nblast_odbc.h it's a draf version for nblast's ODBC port.
/*
/* Revision 1.4  2004/01/12 22:28:58  zwang
/* Added more functions for ODBC layer.
/* Changed return types.
/* Copy some function from API layer:intrez.c
/* */

