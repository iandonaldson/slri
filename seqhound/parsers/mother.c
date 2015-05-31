/* $Id: mother.c,v 1.36 2005/04/26 19:14:46 eburgess Exp $
*******************************************************************************************
  Copyright (C) 2000 Mount Sinai Hospital (MSH)

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of
  the License, or any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR P1URPOSE.

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
  July 1999
  parses *.aso files from GenBank and fills out the CodeBase databases - Seqhound
  database system (ASNDB, NUCPROT, ACCDB, PUBSEQ, TAXGI databases)

    September 1999
    TAXGIFIN taken out of the system
    October 1999
    title of the sequence is now stored in Memo field, it is stored only once for each
    sequence
    December 1999
    added NCBI way of logging the messages, the log file is moved into (division)run
    file at the end of a program
    TAXGI stores each GI even if it is not associated with any taxonomy Id


******************************************************************************************/


#include <intrez_db.h>
#include <seqhound.h>
#include <slri_post.h>
#include <slri_misc.h>
#include <intrez_misc.h>
#include <hist_db.h>
#include <parseasn.h>
#include <slri_rbtree.h>
#include <tofasta.h>
 
#define LOGING "motherlog"

#define NUMARGS 7
Args myargs[NUMARGS] = {
        {"Filename for asn.1 input",NULL,NULL,NULL,FALSE,'i',ARG_FILE_IN,0.0,0,NULL},
        {"Release",NULL,NULL,NULL,FALSE,'r',ARG_STRING,0.0,0,NULL},
        {"Initialize the ASNDB database file","F", NULL, NULL, TRUE, 'n', ARG_BOOLEAN, 0.0, 0, NULL},
        {"Initialize the remaing database files","F", NULL, NULL, TRUE, 'm', ARG_BOOLEAN, 0.0, 0, NULL},
        {"Is this an update","F", NULL, NULL, TRUE, 'u', ARG_BOOLEAN, 0.0, 0, NULL},
        {"Is this a file for human/mouse complete genome","F", NULL, NULL, TRUE, 'c', ARG_BOOLEAN, 0.0, 0, NULL},
        {"Read input file in text mode","F", NULL, NULL, TRUE, 't', ARG_BOOLEAN, 0.0, 0, NULL},
        };

#define MAXINSERTS 10000

typedef struct pubdata{
   Int4 pmid;
   Int4 muid;
} PUBDATA;

static StAsndbPtr pas;
static StPartiPtr ppa;
static StAccdbPtr pac;
static StNucprotPtr pnc;
static StPubseqPtr pps;
static StTaxgiPtr ptg;
static Boolean REFSEQ;
static Boolean UPDATE;

/* globals from sendb parser */
static StSengiPtr psg;
static StSendbPtr psd;

/* ============================================================= 
   Functions used to extract complete genome information.
   ============================================================= */


CharPtr GetGenomeInfoFromSeqDescr( ValNodePtr pSeqDescr,
								StChromPtr pInfo, CharPtr *pDiv );
CharPtr GetGenomeInfoFromBioSource( BioSourcePtr pSource,
								 StChromPtr pInfo, 
								 CharPtr *pDiv);

CharPtr GetGenomeInfo( BioseqPtr pBioseq, ValNodePtr pSeqDescr, 
				    Int4 kloodge, CharPtr *pDivision );

void GetDivisionFromGBBlock( ValNodePtr pDescr, 
							 CharPtr * pDivision );

Int4    GetTaxIdFromOrgRef( OrgRefPtr pOrg );
CharPtr GetGenusAndSpecies( OrgRefPtr pOrg );
CharPtr MakeName( CharPtr firstName, CharPtr lastName );
CharPtr AddChromosomeTypeToName( CharPtr pName, Int4 XmeType );
CharPtr AddSubTypeDetails( CharPtr pName, 
						   SubSourcePtr pSubtype,
						   StChromPtr pRecord );
Int4  AssignChromflagFromSubtype( Int4 SubType );

Int4 GetChromIdFromBioseq( BioseqPtr pBioseq );
Int4 GetChromIdFromBioseqSet( BioseqSetPtr pSet, CharPtr * xmeNumber );


void       FillContigChromid( ValNodePtr pvnContigGis, Int4 topGi,
								 Int4 chromId, Int4 update );
void       FillGiChromosome( ValNodePtr pvnComgenGis, 
							 CharPtr xmeNumber );
void       WriteGichromosome( Int4 gi, CharPtr xmeNumber );

void       FillContigChromosome( ValNodePtr pvnComgenGis, 
								 CharPtr xmeNumber );

/* ============================================================= 
   Structures and functions for extracting the names of 
   biomolecules.
   ============================================================= */
#define YEAST_TAXID 4932

#define SEQENTRY_BIOSEQ    1
#define SEQENTRY_BIOSEQSET 2

/* SeqFeat defines */
#define ANNOT_TABLE 1
#define ANNOT_ALIGN 2
#define ANNOT_GRAPH 3
#define ANNOT_IDS   4
#define ANNOT_LOCS  5

/* Strand Defines */
#define UNKNOWN     0
#define PLUS        1
#define MINUS       2

/* Protein Defines */
#define NOTSET     0
#define PREPROTEIN 1
#define MATURE     2
#define SIGNALPP   3
#define TRANSITPP  4
  
/* Field Defines for REFSEQ records */
#define FLD_UNKNOWN           0
#define FLD_BIOSEQ            1
#define FLD_LOCUS             2
#define FLD_PROT_NAME         3
#define FLD_PROT_DESC         4
#define FLD_BIOSEQ_TITLE      5
#define FLD_BIOSEQ_PDB        6
#define FLD_SEQSET_TITLE      7
#define FLD_SEQSET_PDB        8 
#define FLD_SYNONYM           9  

/* Field Defines for YEAST records */
#define FLD_YEAST_FTYPE       100
#define FLD_YEAST_DESC        101
#define FLD_YEAST_ALIAS       102
#define FLD_YEAST_GENE        103

/* Define for Rules table */
#define RULE_NONE   0
#define RULE_LOCUS  1
#define RULE_SEQSET 2

/* Define for Name types */
#define TYPE_PROTEIN     1
#define TYPE_GENE        2


#define NO_NAME "NO_PRIMARY_LOCUS_NAME"

#define UNCHANGED 0
#define ADDED     1
#define DELETED    2

#define DEFAULTDATE "0000-00-00 00:00:00"

#define TITLE_LENGTH 4096

/** CDRINFO
 *
 *  Structure to hold information found in the CDRegion feature.
 *  Note that functions are provided to create and destroy CDRINFOs.
 */
typedef struct cdrInfo
{
  Int4    gin;    /** nucleotide gi for this coding region */
  Int4    from;   /** start of the coding region */
  Int4    to;     /** end of the coding region */
  Int4    gip;    /** protein gi that this region encodes */
  CharPtr access; /** accession number for this protein */
} CDRINFO;

/** LOCUSINFO
 * *  Structure to hold information found in the Gene-ref feature.
 *  Note that functions are provided to create and destroy 
 *  LOCUSINFOs. Also functions are provided to add synonyms.
 */
typedef struct locusInfo
{
  Int4     gin;          /**  nucleotide gi for this coding region */
  Int4     from;         /** start of the coding region */
  Int4     to;           /** end of the coding region */
  CharPtr  name;         /** locus name */
  CharPtr *pSynonyms;     /** other names from the locus-syn field */
  Int4     synCount; /** number of synonyms found */
} LOCUSINFO;

/** PROTEINNAME
 *
 *  Structure to hold protein names found in a protein bioseq.
 *  The names may come from the title, name or description fields.
 *  Functions are prvided to create and destroy proteinNames.
 */ 
typedef struct proteinName
{
  struct proteinName *next;

  CharPtr name;    /** name */
  Int4    field;   /** field from which this name comes. */
  Int4    nameType; /** Type of name. Either protein or gene. */
  Int4    action;  /** action to take if we are updating. Always
				      starts out as ADDED */
}PROTEINNAME, PNTR ProteinNamePtr;

/** nameList
 *
 * Structure to hold protein names found in a protein bioseq. 
 * Because the names may come from a title, name or description 
 * field, it's necessary to include the access, gi, etc. in the
 * structure Functions are provided to create and destroy the 
 * nameList.
 */
typedef struct nameList
{
  Int4    gi;         /** gi of the protein bioseq containing the names. */
  CharPtr access;     /** Accession of the protein bioseq containing the names. */
  CharPtr db;         /** Database in which the protein is found. */
  CharPtr dateDep;    /** Deprecated data - not used at present */
  CharPtr actionDate; /** Action date - this is the date */

  StBionamePtr pNames;    /** List of name records. */
  StBionamePtr pCurrent;  /** Pointer to the end of the list. */
}NAMELIST, PNTR NameListPtr;

/** CDRLIST
 *
*  RedBlackTree to hold a list of CDRInfos. The nucleotide gi
 *  will be the key.
 */
typedef struct cdrList
{
  int count;          /** number of CDRINFOs in the tree */
  RedBlackTreeNodePtr             cdrRoot; /** root node in the tree. */
  RedBlackTreeFunctionPackagePtr  funPack;  /** functions for the tree. */
} CDRLIST;

/** LOCUSLIST
 *
 *  Structure to hold an array of LOCUSINFOs.
 */
typedef struct locusList

{
  /*LOCUSINFO *pLocusInfos; ** array of LOCUSINFOs 
  Int4     count;       ** number of LOCUSINFOs used 
  Int4     allocated;   ** number of LOCUSINFOs allocated */
  int count;
  RedBlackTreeNodePtr     locusRoot;
  RedBlackTreeFunctionPackagePtr  funPack;
} LOCUSLIST;



/** NAMEINFO
 * Structure that holds lists etc. needed to extract names from
 * bioseqs or bioseqsets.
 */
typedef struct nameInfo
{
  CDRLIST   *pCdrList;    /** List of CD Region information. */
  LOCUSLIST *pLocusList;  /** List of locus information. */
  BioseqSetPtr last_pbss; /** Last bioseqset: sometimes containts 
                              the annotation instead of the bioseq. */ 
  ValNodePtr  pDescr;     /** Description sometimes used to get 
			      the protien name. */
  Boolean    getCdrInfo;  /** Look for the CD region in the bioseq */
} NAMEINFO;
/* -----------------------------------------------------------
   Functions to handle Protein, CDR and Locus info.
   ----------------------------------------------------------- */
CDRINFO   *CreateCdrInfo( Int4 newGin, Int4 newFrom, Int4 newTo, 
                          Int4 newGip, CharPtr newAccess );
void       DestroyCdrInfo( Pointer *pInfo );
LOCUSINFO *CreateLocusInfo( Int4 newGi, Int4 newFrom, 
                            Int4 newTo, CharPtr newName );
void       DestroyLocusInfo( Pointer *pInfo );
Int4       AddNameToLocus( LOCUSINFO *pInfo, CharPtr pName );
Int4       AddSynonymToLocus( LOCUSINFO *pInfo, CharPtr pSynonym );

NameListPtr CreateNameList( CharPtr db, CharPtr access, Int4 gi,
							CharPtr dateDep, CharPtr actionDate);
void           DestroyNameList( NameListPtr pName ); 
void           AddNameToList( NameListPtr * pName, CharPtr name,
							  Int4 nameType, Int4 field, Int4 ruleId);


CDRLIST       *CreateCdrList( );
LOCUSLIST     *CreateLocusList();
void           DestroyCdrList( CDRLIST *pList );
void           DestroyLocusList(LOCUSLIST *pList );
Boolean        AddCdrInfoToList( CDRLIST *pList, CDRINFO *pInfo );
Boolean        AddLocusInfoToList( LOCUSLIST *pList, LOCUSINFO *pInfo );
CDRINFO       *FindCdrInfo( LOCUSINFO *pLocus, CDRLIST *pList );

/* ------------------------------------------------------------
   Functions to parse names from bioseqs and bioseqsets.
   ------------------------------------------------------------ */
Int4 ParseCDRInfo( BioseqSetPtr pbss, BioseqPtr pbs, 
                   CDRLIST *pList, ValNodePtr PNTR ppvnGis );
Int4 ParseCDRInfoFromAnnotation( SeqAnnotPtr psa, Int4 gi, 
								 CDRLIST *pList, ValNodePtr *pGiList );  
Int4 ParseCDRInfoFromFeatureTable( SeqFeatPtr psf, Int4 gi, 
								   CDRLIST *pList );
Int4 GetSourceLocationInfo( SeqLocPtr psl, Int4Ptr pgi, Int4Ptr pfloc, 
                              Int4Ptr ptloc, Boolean transSpliced );
Int4 GetLocationInfoFromSeqInt( SeqLocPtr psl, Int4Ptr pgi,
                                Int4Ptr pfloc, Int4Ptr ptloc );
Int4 GetLocationInfoFromSeqMix( SeqLocPtr psl, Int4Ptr pgi, Int4Ptr pfloc,
								Int4Ptr ptloc, Boolean transSpliced ); 

Int4 GetLocationInfoFromPackedInt( SeqLocPtr psl, Int4Ptr pgi, Int4Ptr pfloc,
                                   Int4Ptr ptloc, Boolean transSpliced );
Boolean ParseNamesFromBioseq( BioseqPtr pbs, 
							  NAMEINFO *pNameINFO,
							  ValNodePtr PNTR ppvnComgenGis );
                              

Int4 ParseLocusInfo(BioseqPtr pbs, BioseqSetPtr pbss, LOCUSLIST *pList);
Int4 ParseLocusInfoFromFeatureTable( SeqFeatPtr psf, Int4 gi,
                                     LOCUSLIST *pList  ); 
Int4  ParseProteinNames( BioseqPtr pbs, NameListPtr *pList );
Int4  ParseNameFromAnnot( SeqAnnotPtr pSeqAnnot, NameListPtr *pList );
Int4  ParseNameFromFeatureTable( SeqFeatPtr psf, NameListPtr *pList );
CharPtr ParseTitleFromSeqDesc( SeqDescPtr pSeqDesc,
							   NameListPtr *pList, Int4 field );

Int4  WriteProteinInfo( CharPtr db, CharPtr access, Int4 gi, 
						NameListPtr pNameList, Int4 bioentityField );
void  WriteProteinNames( NameListPtr pNameList, Int4 bioentityId, 
						 CharPtr db, CharPtr access, 
						 Int4 identifier, Int4 update );
void  UpdateNames( Int4 bioentityId, Int4 type, 
				   StBionamePtr pList );

void    SetNameInfo( NAMEINFO *pInfo, CDRLIST *pCList, 
                     LOCUSLIST *pLList, BioseqSetPtr pbss, 
                     Boolean getInfo );

Boolean CheckName( CharPtr pName );
#ifdef ODBC
void    WriteLocusNames( LOCUSINFO * pLocus, Int4 bioentityId, 
						 CharPtr db, CharPtr access, Int4 Official, 
						 Int4 Deprecated, CharPtr dateDep );

Boolean FillNameDBs( CDRLIST *pCdrList, LOCUSLIST *pLocusList );
Int4    FillBioentityDB( Int4 type, CharPtr dbName, 
                         CharPtr accession,  Int4 indentifier, 
                         Int4 field, Int4 * update );
void    FillBionameDB( Int4 bioentityId, CharPtr pName, Int4 typeid,
					   CharPtr pDb, CharPtr pAccess, Int4 gi,
					   Int4 field, Int4 Official, Int4 Deprecated,
					   CharPtr dateDep, Int4 RuleId, Int4 action,
					   CharPtr actiondate);

void FillBionameRecord( StBionamePtr *pList, Int4 bionameId,
						Int4 bioentityId, CharPtr pName,
						Int4 typeid, CharPtr pDb, CharPtr pAccess,
						Int4 gi, Int4 field, Int4 Official,
						Int4 Deprecated, CharPtr dateDep,
						Int4 RuleId, Int4 action, CharPtr actiondate);
								
#endif /* if ODBC defined */

/* function declarations */
Boolean ToBioseq(SeqEntryPtr, Int4, ValNodePtr, CharPtr, CharPtr,
                 NAMEINFO *pNameInfo);
/* ASNDB, PARTI */
Boolean FillASNDB(BioseqPtr, ValNodePtr, CharPtr, CharPtr);
CharPtr GetType (BioseqPtr);
/* ACCDB */
Boolean FillACCDB(BioseqPtr, ValNodePtr);
Boolean FillACCDBFromTextSeqId(BioseqPtr pbs, SeqIdPtr pSeqId,
							   CharPtr title);
CharPtr GetTitle(BioseqPtr, SeqDescrPtr);
CharPtr GetName(BioseqPtr);
Boolean WriteACCDB(BioseqPtr, CharPtr, TextSeqIdPtr, 
				   PDBSeqIdPtr, DbtagPtr, CharPtr title, Int4 intId );



/* PUBSEQ */
Boolean FillPUBSEQ(BioseqPtr, ValNodePtr);
ValNodePtr GetMuid(ValNodePtr);
/* TAXGI, TAXSP */
Boolean FillTAXGI(BioseqPtr pbs, ValNodePtr pDesc, Int4 chromId);
OrgRefPtr GetOrgRefPtr(ValNodePtr);
Int4 GetTaxId(ValNodePtr);
Boolean WriteTAXGI(BioseqPtr pbs, Int4 taxid, Int4 kloodge );
/* NUCPROT */
Int2 WhichClass(BioseqSetPtr);
Boolean FillNUCPROT(BioseqSetPtr);
Boolean WriteNUCPROT(Int4, Int4);
Int2 WhichType(BioseqPtr);
Int4 GetSegSetGI(BioseqSetPtr, Int4);
ValNodePtr GetProtFromHumanMouse(BioseqPtr pbs);

Boolean FillSOURCEFILE( Int4 gi, CharPtr filename );

/*Boolean FillHistDB( */
/* functions from sendb parser - SENDB, SENGI */
Boolean ToBioseqSEQENTRY(SeqEntryPtr PNTR pse, Int4 callno, 
						 Int4 ibi, CharPtr division);
BioseqPtr MakeDummyBioseq(Int4 gi);

/*variable for counting the number of inserts going into the database*/
Int4 insert_count = 0;
static Int4 seid = 0;
/*----------------------------------------------------------
  Functions and variables used for updates.
   ----------------------------------------------------------*/
/** SEQIDINFO
 *
 * Structure to hold the information relevant to updates found
 * in the SeqIds. Holds gi, version, accession and a list of gis
 * that may be associated with different names. These are used 
 * during updates to delete all records associated with a given
 * accession or name.
 */ 
typedef struct seqidInfo
{
  Int4 gi;
  Int4 version;
  CharPtr access;
  ValNodePtr pvnNames;
  /*  ValNodePtr pvnExtraGis;*/
} SEQIDINFO;

typedef struct dbname
{
  CharPtr pName;
  CharPtr pDb;
} DBNAME;

DBNAME *CreateDBNAME( CharPtr pName, CharPtr pDb );
void    DestroyDBNAME( DBNAME **pDbName );
SEQIDINFO *CreateSEQIDINFO();
void       DestroySEQIDINFO( SEQIDINFO **pInfo );
SEQIDINFO *GetSeqIdInfoFromBioseq( BioseqPtr pbs );

Boolean CompareGis( Int4 newGi, CharPtr pAccess, Int4 newVersion,
					StAccdbPtr pac, CharPtr pFilename );
					
/* --------------------------------------------------
   Functions and variables adopted from update.c
   -------------------------------------------------- */
static Boolean ToBioseqUp(SeqEntryPtr, Int4, CharPtr pFilename);
Boolean IntrezDeleteGI(Int4 gi, Boolean destroy); 
Boolean LogHistory(Int4 gi, Int4 oldgi, CharPtr paccess, 
				   Int2 version, Int2 action, CharPtr pFilename);
StAccdbPtr GetACCDBRecord(CharPtr paccess);
Boolean ProcessTopComgenGi( Int4 topGi, Boolean * pFound );
Boolean ProcessContigGi( Int4 topGi, Boolean * pFound );
Boolean ProcessLowLevelGi( Int4 topGi );

/* global variables*/
ValNodePtr gpvnadd = NULL, gpvnchange = NULL, gpvndelete = NULL;
ValNodePtr gpvnextra = NULL, gpvnextrahead = NULL;
ValNodePtr gpvnaddhead = NULL, gpvnchangehead = NULL, gpvndeletehead = NULL;
CharPtr gpdate = NULL;


/* makes a Bioseq which contains only a GI */
BioseqPtr MakeDummyBioseq(Int4 gi)
{
   BioseqPtr pbs = NULL;
   SeqIdPtr psi = NULL;


   pbs = BioseqNew();
   if (pbs == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0,"MakeDummyBioseq: Cannot allocate memory for new bioseq.");
      return NULL;
    }
   psi = (SeqIdPtr) MemNew(sizeof(ValNode));
   if (psi == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0,"MakeDummyBioseq: Cannot allocate memory for seqentry.");
      return NULL;
    }
   pbs->id = psi;
   psi->choice = 12;
   psi->data.intvalue = gi;
   return pbs;
}

/* visits every Bioseq in given SeqEntry */
Boolean ToBioseqSEQENTRY(SeqEntryPtr PNTR ppse, Int4 callno, Int4 ibi, CharPtr division)
{

   BioseqPtr pbs = NULL, pbsempty = NULL;
   BioseqSetPtr pbss = NULL;
   SeqEntryPtr pseNew = NULL;
   Int4 gival = 0;
   SeqEntryPtr pse = NULL;
   CharPtr gbDivision = NULL;
   /*ValNodePtr pvnGis = NULL;
   Int4 kloodge = 0;*/

   /* SK: NULL ptr check */
   if ( ppse == NULL || *ppse == NULL )
   {
      ErrPostEx(SEV_ERROR,0,0,"ToBioseqSEQENTRY: NULL ppse.");
      return FALSE;
   }

   pse = *ppse;
   if (pse->choice == 1) /* It is a Bioseq */
   {
	 pbs = (BioseqPtr) pse->data.ptrvalue;
	 if (pbs == NULL)
     {
	   ErrPostEx(SEV_ERROR,0,0,"ToBioseqSEQENTRY: Null pointer to Bioseq.");
	   return FALSE;
	 }
	 
	 
	 if ((gival = GetGI(pbs)) <= 0)
	 {
	   ErrPostEx(SEV_ERROR,0,0,"ToBioseqSEQENTRY: Invalid GI.");
	   return FALSE;
	 }

	 gbDivision = GetDivisionFromPARTI( gival );
	  /* append a record to SENGI database */
	 ResetStSENGI(&psg);
	 psg->gi = gival;
	 psg->seid =  ibi;
	 psg->division = IsNullStr( gbDivision );
	 AppendRecordSENGI(psg);
	 insert_count = insert_count + 1;

	  /* make dummy bioseq and replace the current one in the structure saves space*/
	 pbsempty = MakeDummyBioseq(gival);
	 pse->data.ptrvalue = pbsempty;
	 /* free the original bioseq */
	 if(pbs)BioseqFree(pbs);

	 MemFree( gbDivision );

   }
   else if (pse->choice == 2) /* it is nested Bioseq-set call ToBioseqSEQENTRY recursively*/
   {
	 
	 pbss = (BioseqSetPtr) pse->data.ptrvalue;
	 if (pbss == NULL)
	 {
	   ErrPostEx(SEV_ERROR,0,0,"ToBioseqSEQENTRY: Null pointer to Bioseq-Set.");
	   return FALSE;
	 }

	 pseNew = pbss->seq_set;
	 while (pseNew != NULL)
	 {
	   if(!ToBioseqSEQENTRY(&pseNew,++callno, ibi, division))
	   {
		 ErrPostEx(SEV_ERROR,0,0,"ToBioseqSEQENTRY: The recursive call to ToBiseqAs failed");
		 return FALSE;
	   }
	   pseNew = pseNew->next;
	 }
   }
   else
   {
	 ErrPostEx(SEV_ERROR,0,0,"ToBioseqSEQENTRY: Invalid choice field.");
	 return FALSE;
    }
   return TRUE;
}

 /*
retrieves the right GI from segset class for nucprot database
*/
Int4 GetSegSetGI(BioseqSetPtr pbss, Int4 count)
{

   Int4 ssgin = 0;
   Int4 ssgia = 0;
   SeqEntryPtr pse = NULL;
   BioseqPtr pbs = NULL;
   BioseqSetPtr pbssp = NULL;
   BioseqPtr pbsp = NULL;
   SeqEntryPtr psep = NULL;

   /* SK: NULL ptr check */
   if ( pbss == NULL )
   {
      ErrPostEx(SEV_ERROR,0,0,"GetSegSetGI: NULL pbss.");
      return 0;
   }

   if(WhichClass(pbss) != BioseqseqSet_class_segset)
    {
      ErrPostEx(SEV_ERROR,0,0,"GetSegSetGI: Bioseq-set inside nuc-prot class is not segset.");
      return 0;
    }

   pse = pbss->seq_set;
   /* SK: NULL ptr check */
   if ( pse == NULL )
   {
      ErrPostEx(SEV_ERROR,0,0,"GetSegSetGI: NULL pse.");
      return 0;
   }

   if (pse->choice != SEQENTRY_BIOSEQ)
    {
      ErrPostEx(SEV_ERROR,0,0, "GetSegSetGI: First member of NA segset class is not Bioseq.");
      return 0;
    }
   pbs = (BioseqPtr) pse->data.ptrvalue;
   if (pbs == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "GetSegSetGI: Bioseq pointer is NULL.");
      return 0;
    }
   if (count == 1 && ( pbs->mol == Seq_mol_dna ||  
					   pbs->mol == Seq_mol_rna || 
					   pbs->mol == Seq_mol_na))
    {
     if ((ssgin = GetGI(pbs)) == 0)
      {
        ErrPostEx(SEV_ERROR,0,0, "GetSegSetGI: Invalid GIN.");
        return 0;
      }
     return ssgin;
    }
   else if (count > 1 && pbs->mol == Seq_mol_aa)
    {
      pse = pse->next;
      /* SK: NULL ptr check */
      if ( pse == NULL )
      {
         ErrPostEx(SEV_ERROR,0,0,"GetSegSetGI: NULL pse.");
         return 0;
      }
      
      if (pse->choice != SEQENTRY_BIOSEQSET)
       {
        ErrPostEx(SEV_ERROR,0,0, "GetSegSetGI: No Bioseq-set inside protein segset.");
        return 0;
       }
      pbssp = pse->data.ptrvalue;
      if (pbssp == NULL)
       {
         ErrPostEx(SEV_ERROR,0,0, "GetSegSetGI: BioseqSet pointer is NULL.");
         return 0;
       }
      if (WhichClass(pbssp) != BioseqseqSet_class_parts)
       {
         ErrPostEx(SEV_ERROR,0,0, "GetSegSetGI: No class parts inside protein segset.");
         return 0;
       }
      psep = pbssp->seq_set;
      if (psep == NULL)
       {
         ErrPostEx(SEV_ERROR,0,0, "GetSegSetGI: Seq-entry pointer is NULL.");
         return 0;
       }
      if (psep->choice != SEQENTRY_BIOSEQ)
       {
         ErrPostEx(SEV_ERROR,0,0, "GetSegSetGI: First member of protein class parts is not Bioseq.");
         return 0;
       }
      pbsp = psep->data.ptrvalue;
      if (pbsp == NULL)
       {
         ErrPostEx(SEV_ERROR,0,0, "GetSegSetGI: Bioseq pointer is NULL.");
         return 0;
       }
      if (pbs->mol != Seq_mol_aa)
       {
         ErrPostEx(SEV_ERROR,0,0, "GetSegSetGI: First Bioseq in class parts is not protein.");
         return 0;
       }
      if ((ssgia = GetGI(pbsp)) == 0)
       {
         ErrPostEx(SEV_ERROR,0,0, "GetSegSetGI: Invalid GIA.");
         return 0;
       }
      return ssgia;
    }
   else
    {
      ErrPostEx(SEV_ERROR,0,0, "GetSegSetGI: Illegal Bioseq inside segset class.");
      return 0;
    }

}


/*
returns integer which represents molecule type
*/
Int2 WhichType(BioseqPtr pbs)
{

    /* SK: NULL ptr check */
    if ( pbs == NULL )
    {
      ErrPostEx(SEV_ERROR,0,0, "WhichType: NULL pbs.");
      return 0;
    }

    switch (pbs->mol)
       {
        case Seq_mol_dna:
        return 1;
        case Seq_mol_rna:
        return 2;
        case Seq_mol_aa:
        return 3;
        case Seq_mol_na:
        return 4;
        case Seq_mol_other:
        return 255;
        default:
        return 0;
       }
}


Boolean WriteNUCPROT(Int4 gin, Int4 gia)
{

   ResetStNUCPROT(&pnc);
   
   /* SK: NULL ptr check */
   if ( pnc == NULL )
   {
      ErrPostEx(SEV_ERROR,0,0, "WriteNUCPROT: NULL pnc.");
      return FALSE;
   }

   pnc->gin = gin;
   pnc->gia = gia;

   AppendRecordNUCPROT(pnc);
   insert_count = insert_count + 1;
   return TRUE;
}



/*
takes Bioseq-set class nuc-prot, makes calls to extract GIs and stores NA-protein pairs in CodeBase
*/
Boolean FillNUCPROT(BioseqSetPtr pbss)
{

  BioseqSetPtr pbssNew = NULL;
  SeqEntryPtr pse = NULL;
  BioseqPtr pbs = NULL;
  Int4 gin = 0;
  Int4 gia = 0;
  Int4 ssgia = 0;
  Int4 count = 1;

  /* SK: NULL ptr check */
  if ( pbss == NULL )
  {
	ErrPostEx(SEV_ERROR,0,0, "FillNUCPROT: NULL pbss.");
	return FALSE;
  }

  pse = pbss->seq_set;
  while(pse != NULL)
  {
	if (pse->choice == SEQENTRY_BIOSEQSET)  /* another Bioseq-set */
	{
	  pbssNew = pse->data.ptrvalue;
	  if (pbssNew == NULL)
	  {
		ErrPostEx(SEV_ERROR,0,0, "FillNUCPROT: Null pointer to Bioseq-set.");
		return FALSE;
	  }
	  /*  ErrPostEx(SEV_ERROR,0,0, "FillNUCPROT: Bioseq-set inside nuc-prot class."); */
	  if (count == 1)
	  {
		if ((gin = GetSegSetGI(pbssNew, count)) == 0)
		{
		  ErrPostEx(SEV_ERROR,0,0, "FillNUCPROT: Invalid segset GIN.");
		  return FALSE;
		}
             /* ErrPostEx(SEV_ERROR,0,0, "FillNUCPROT: Got segset GIN %ld.", (long) gin); */
	  }
	  else
	  {
		if ((ssgia = GetSegSetGI(pbssNew, count)) == 0)
		{
		  ErrPostEx(SEV_ERROR,0,0, "FillNUCPROT: Invalid segset GIN.");
		  return FALSE;
		}
		if (!WriteNUCPROT(gin, ssgia))
		{
		  ErrPostEx(SEV_ERROR,0,0, "FillNUCPROT: WriteNUCPROT failed.");
		  return FALSE;
		}
	  }
	}
	else if (pse->choice == SEQENTRY_BIOSEQ) /* It is Bioseq */
	{
	  pbs = (BioseqPtr) pse->data.ptrvalue;
	  if (pbs == NULL)
	  {
		ErrPostEx(SEV_ERROR,0,0, "FillNUCPROT: Null pointer to Bioseq.");
		return FALSE;
	  }
	  /* ErrPostEx(SEV_ERROR,0,0, "FillNUCPROT: Got pointer to Bioseq #%d.", count); */

	  if(count == 1 && ( pbs->mol == Seq_mol_dna || 
						 pbs->mol == Seq_mol_rna || 
						 pbs->mol == Seq_mol_na)) /* Is it NA? */
	  {
		if ((gin = GetGI(pbs)) == 0)
		{
		  ErrPostEx(SEV_ERROR,0,0, "FillNUCPROT: Invalid GIN.");
		  return FALSE;
		}
		/* ErrPostEx(SEV_ERROR,0,0, "FillNUCPROT: Got GIN %ld.", (long) gin); */
	  }
	  else if (count > 1 && ( pbs->mol == Seq_mol_aa))   /* Is it protein? */
	  {
		if (gin == 0)
		{
		  ErrPostEx(SEV_ERROR,0,0, "FillNUCPROT: Did not get GIN before banking GIAs.");
		  return FALSE;
		}
		if ((gia = GetGI(pbs)) == 0)
		{
		  ErrPostEx(SEV_ERROR,0,0, "FillNUCPROT: Invalid GIA.", (long) gin);
		  return FALSE;
		}
		if (!WriteNUCPROT(gin, gia))
		{
		  ErrPostEx(SEV_ERROR,0,0, "FillNUCPROT: WriteNUCPROT failed.");
		  return FALSE;
		}
	  }
	  else if (count == 2 && ( pbs->mol == Seq_mol_dna || 
							   pbs->mol == Seq_mol_rna ||  
							   pbs->mol == Seq_mol_na))
	  {
		ErrPostEx(SEV_ERROR,0,0, "FillNUCPROT: Skipping NA in second position of nuc-prot.");
	  }
	  else
	  {
		ErrPostEx(SEV_ERROR,0,0, "FillNUCPROT: ERROR...Unknown type of nuc-prot at %ld.", GetGI(pbs));
		return TRUE;
	  }
	}
	else
	{
	  ErrPostEx(SEV_ERROR,0,0, "FillNUCPROT: Invalid choice field.");
	  return FALSE;
	}
	pse = pse->next;
	count++;
  }
  return TRUE;
}



/*
returns integer which represents class type
*/
Int2 WhichClass(BioseqSetPtr pbss)
{

   /* SK: NULL ptr check */
   if ( pbss == NULL )
   {
      ErrPostEx(SEV_ERROR,0,0, "WhichClass: NULL pbss.");
      return 0;
   }

   switch (pbss ->_class)
    {
      case BioseqseqSet_class_not_set:
      return 0;
      case BioseqseqSet_class_nuc_prot:
      return 1;
      case BioseqseqSet_class_segset:
      return 2;
      case BioseqseqSet_class_conset:
      return 3;
      case BioseqseqSet_class_parts:
      return 4;
      case BioseqseqSet_class_gibb:
      return 5;
      case BioseqseqSet_class_gi:
      return 6;
      case BioseqseqSet_class_genbank:
      return 7;
      case BioseqseqSet_class_pir:
      return 8;
      case BioseqseqSet_class_pub_set:
      return 9;
      case BioseqseqSet_class_equiv:
      return 10;
      case BioseqseqSet_class_swissprot:
      return 11;
      case BioseqseqSet_class_pdb_entry:
      return 12;
      case BioseqseqSet_class_mut_set:
      return 13;
      case BioseqseqSet_class_pop_set:
      return 14;
      case BioseqseqSet_class_phy_set:
      return 15;
      case BioseqseqSet_class_other:
      return 255;
      default:
      return 0;
    }
}


/* Gets a taxonomy identifier from a ValNode list of descriptors */
/* KM 290304 this fnct is a result of complete rewrite; merged the former GetTaxId and GetOrgRefPtr into it*/
Int4 GetTaxId (ValNodePtr pvndes)
{  
  ValNodePtr pList = pvndes;
  SeqDescrPtr pdes = NULL;
  BioSourcePtr pbio = NULL;
  Int4 taxid = 0;

  while(pList != NULL)
  {
	pdes = pList->data.ptrvalue;
	/* ErrPostEx(SEV_ERROR,0,0, "Iteration of GetTaxId.");*/
	while (pdes != NULL)
    {
	  if (pdes->choice == Seq_descr_source) 
      {
		pbio = pdes->data.ptrvalue;
		if (pbio != NULL)
		{
		  taxid = GetTaxIdFromOrgRef( pbio->org );
		}
	  }
	  pdes = pdes->next;
	}  
    pList = pList->next;
  }
  
  return taxid;
}


/* Stores data in CodeBase table TAXGI */
Boolean WriteTAXGI(BioseqPtr pbs, Int4 taxid, Int4 kloodge)
{
   
   Int4 gi = 0;
   CharPtr ptype = NULL;

   /* SK: NULL ptr check */
   if ( pbs == NULL )
   {
      ErrPostEx(SEV_ERROR,0,0, "WriteTAXGI: NULL pbs.");
      return FALSE;
   }

   ResetStTAXGI(&ptg);
   if(taxid <= 0) 
	 ErrPostEx(SEV_ERROR,0,0, "WriteTAXGI: TAXID %ld for GI %ld.", (long) taxid, (long) GetGI(pbs));

   ptg->taxid = taxid;
   if ((gi = GetGI(pbs)) == 0)
    {
      ErrPostEx(SEV_ERROR,0,0, "WriteTAXGI: Invalid GI.");
      return FALSE;
    }
   else
    ptg->gi = gi;

   if ((ptype = GetType(pbs)) == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "WriteTAXGI: GetType failed.");
      return FALSE;
    }
   else
     ptg->type = ptype;

   ptg->kloodge = kloodge;

   AppendRecordTAXGI(ptg);
   insert_count = insert_count + 1;
   return TRUE;
}

Boolean FillSOURCEFILE( Int4 gi, CharPtr filename )
{
  StSourcefilePtr pRecord = NULL;
  Boolean retcode = FALSE;

  if( gi <= 0 || filename == NULL )
  {
	return retcode;
  }

  pRecord = StSourcefileNew();

  if( !pRecord )
  {
	ErrPostEx(SEV_ERROR,0,0, "FillSOURCEFILE: "
			  "Unable to allocate StSourceFilePtr" );
	return retcode;
  }

  pRecord->gi = gi;
  pRecord->filename = StringSave( filename );

  retcode  = AppendRecordSOURCEFILE( pRecord);

  return retcode;
}


/* KM 260304 change of input type; 290304 complete rewrite of supporting functionality for GetTaxId*/
Boolean FillTAXGI(BioseqPtr pbs, ValNodePtr pvndesset, 
				  Int4 kloodge)
{
    /*OrgRefPtr por = NULL;*/
    Int4 taxid = 0;
    ValNodePtr pvndes = NULL;

    /* SK: NULL ptr check */
    if ( pbs == NULL )
    {
       ErrPostEx(SEV_ERROR,0,0, "FillTAXGI: NULL pbs.");
       return FALSE;
    }

    /* KM 260304 wrap the Bioseq descriptor into ValNode to be able to pass it to GetOrgRefPtr */
    /* KM 300304 pass it to GetTaxId as the two merged*/
    pvndes = Slri_ValNodeAddPointer(&pvndes, 0, pbs->descr);
    taxid  = GetTaxId(pvndes);
    if (taxid <= 0)
    {
       taxid  = GetTaxId(pvndesset);
       /*ErrPostEx(SEV_INFO,0,0, "Using ValNode of Bioseq-set descriptors for %ld.", (long) GetGI(pbs));*/
    }
    if(taxid <= 0)
      taxid = 0;  
    
    if (!WriteTAXGI(pbs, taxid, kloodge))
     {
       ErrPostEx(SEV_ERROR,0,0, "FillTAXGI: WriteTAXGI failed.");
       if(pvndes) ValNodeFree(pvndes);
       return FALSE;
     }
    if(pvndes) ValNodeFree(pvndes);
    return TRUE;
}

/* KM 260304 change of input type */
ValNodePtr GetMuid(ValNodePtr pvndes)
{

   SeqDescrPtr pdes = NULL;
   PubdescPtr ppd = NULL;
   ValNodePtr pub = NULL;
   ValNodePtr pvnmuid = NULL;
   
   /* KM 260304 look through ValNode of descriptors */
   while(pvndes != NULL)
   {
      pdes = pvndes->data.ptrvalue;
      if (pdes == NULL)
        continue;   
      while (pdes != NULL)
      {
         if (pdes->choice == Seq_descr_pub) /* 12 */
         {
            ppd = pdes->data.ptrvalue;
            if (ppd != NULL)
            {
               pub  = ppd->pub;
               if (pub != NULL)
               {              
                 while(pub != NULL)
                 {
                   if (pub->choice == 4 && pub->data.intvalue != 0) /* 4 Pub_Muid*/ 
                      ValNodeAddInt(&pvnmuid, 0, pub->data.intvalue);  
                   else if (pub->choice == 13 && pub->data.intvalue != 0) /* 13 Pub_PMid*/
                      ValNodeAddInt(&pvnmuid, 1, pub->data.intvalue);
                   pub = pub->next;
                 }
               }
            }
         }
         pdes = pdes->next;
      }
      pvndes = pvndes->next;
   }
   return pvnmuid;
}

/* KM 260304 change of input type */
Boolean FillPUBSEQ(BioseqPtr pbs, ValNodePtr pvndesset)
{

    SeqDescrPtr pdes = NULL;
    ValNodePtr pvnmuid = NULL, pvnmuidhead = NULL, pvndes = NULL;
    Int4 gi = 0;

    /* SK: NULL ptr check */
    if ( pbs == NULL )
    {
       ErrPostEx(SEV_ERROR,0,0, "FillPUBSEQ: NULL pbs.");
       return FALSE;
    }

    pdes = pbs->descr;
    /* KM 260304 wrap descriptor in ValNode to pass to GetMuid */
    pvndes = Slri_ValNodeAddPointer(&pvndes, 0, pdes);
    pvnmuid = GetMuid(pvndes);
    ValNodeFree(pvndes);
    if (pvnmuid == NULL)
    {
       pvnmuid = GetMuid(pvndesset);
       if (pvnmuid == NULL)
       {
          ValNodeFree(pvnmuid);
          return TRUE;
       }
    }
    if ((gi = GetGI(pbs)) == 0)
    {
       ErrPostEx(SEV_ERROR,0,0, "FillPUBSEQ: Invalid GI.");
       return FALSE;
    }
    pvnmuidhead = pvnmuid;
    while (pvnmuid != NULL)
    {
       ResetStPUBSEQ(&pps);
       /* SK: NULL ptr check */
       if ( pps == NULL )
       {
         ErrPostEx(SEV_ERROR,0,0, "FillPUBSEQ: NULL pps.");
         ValNodeFree(pvnmuidhead);
	    return FALSE;
	  }
       pps->gi = gi;
       if (pvnmuid->data.intvalue == 0)
       {
          ErrPostEx(SEV_ERROR,0,0, "FillPUBSEQ: Invalid MUID.");
          ValNodeFree(pvnmuidhead);
          return FALSE;
       }
	  /* for now, using pps' muid to store muid or pmid, passing
	     in 'choice' to differentiate, choice == 1 -> muid is a PMID,
	     choice == 0 -> muid is a MUID. */
       pps->muid = pvnmuid->data.intvalue;

	  AppendRecordPUBSEQ(pps, pvnmuid->choice);
       insert_count = insert_count + 1;
       pvnmuid = pvnmuid->next;
    } 
    ValNodeFree(pvnmuidhead);
    return TRUE;
}


Boolean WriteACCDB(BioseqPtr pbs,
                   CharPtr datab,
                   TextSeqIdPtr ptsi,
                   PDBSeqIdPtr ppsi,
                   DbtagPtr pdt,
                   CharPtr title, 
				   Int4 intId)


{
   Int4 gi = 0;
   DatePtr dat = NULL;
   Char buf[20];
   Int4 id = 0;
   char idstr[10];
   char chainc[5];
   Char notavail[4];

   ResetStACCDB(&pac);

   StringCpy(notavail, "n/a");


   /* SK: NULL ptr check */
   if ( pbs == NULL )
   {
      ErrPostEx(SEV_ERROR,0,0, "WriteACCDB: NULL pbs.");
      return FALSE;
   }

   if ((gi = GetGI(pbs)) == 0)
    {
      ErrPostEx(SEV_ERROR,0,0, "WriteACCDB: Invalid GI.");
      return FALSE;
    }
   pac->gi = gi;

   if (title != NULL)
        pac->title = title;


   if (datab != NULL)
      pac->db = datab;

   if (ptsi != NULL)
    {
      if (ptsi->name != NULL)
       {
         pac->name = ptsi->name;
         pac->namelow = ptsi->name;
       }
      if (ptsi->accession != NULL)
         pac->access = ptsi->accession;
      if (ptsi->release != NULL)
         pac->release = ptsi->release;
      if (ptsi->version != INT2_MIN)
         pac->version = ptsi->version;
    }
   if (ppsi != NULL)
    {
     if (ppsi->mol != NULL)
      {
         pac->name = ppsi->mol;
         pac->namelow = ppsi->mol;
      }
     if ((ppsi->chain != 0) && (ppsi->chain != 32))
      {
         sprintf(chainc, "%c", (char) ppsi->chain);
         pac->chain = chainc;
      }
     dat = ppsi->rel;
     if (dat != NULL)
      {
       if (DatePrint(dat, buf) == TRUE)
          pac->release = buf;
        else
          ErrPostEx(SEV_ERROR,0,0, "WriteACCDB: DatePrint failed.");
      }
    }
  if (pdt != NULL)
  {
     if (pdt->db != NULL)
        pac->db = pdt->db;

     /* SK: NULL ptr check */
     if (pdt->tag != NULL)
     {
        if (pdt->tag->str != NULL)
        {
           pac->name =  pdt->tag->str;
           pac->namelow = pdt->tag->str;
        }
        else
        {
           id = pdt->tag->id;
           sprintf(idstr, "%ld", (long) id);
           pac->name = idstr;
           pac->namelow = idstr;
        }
     }
  }


  if( intId > 0 )
  {
	sprintf( idstr, "%ld", (long) intId );
	pac->name = idstr;
	pac->namelow = idstr;
  }
   if(pac->db == NULL) pac->db = notavail;
   if(pac->name == NULL) pac->name = notavail;
   if(pac->namelow == NULL) pac->namelow = notavail;
   if(pac->access == NULL) pac->access = notavail;
   if(pac->chain == NULL) pac->chain = notavail;
   if(pac->release == NULL) pac->release =  notavail;

   AppendRecordACCDB(pac);
   insert_count = insert_count + 1;
   return TRUE;
}




/*
Gets name of protein from feature table
*/
CharPtr GetName(BioseqPtr pbs)
{

   SeqAnnotPtr psa = NULL;
   SeqFeatPtr psf = NULL;
   ProtRefPtr ppr = NULL;
   ValNodePtr pvn = NULL;
   CharPtr namestr = NULL;

   /* SK: NULL ptr check */
   if ( pbs == NULL )
   {
      ErrPostEx(SEV_ERROR,0,0, "GetName: NULL pbs.");
      return NULL;
   }

   psa = pbs->annot;
   if (psa == NULL)
   {
      /* ErrPostEx(SEV_ERROR,0,0, "GetName: NULL SeqAnnot pointer."); */
      return NULL;
   }

   while (psa != NULL)
    {
      if (psa->type == 1) /* ftable */
       {
         psf = (SeqFeatPtr) psa->data;
         if (psf == NULL)
          {
            /* ErrPostEx(SEV_ERROR,0,0, "GetName: NULL SeqFeat pointer."); */
            return NULL;
          }
         while (psf != NULL)
          {
           if (psf->data.choice == SEQFEAT_PROT) /* 4 */
            {
              ppr = psf->data.value.ptrvalue;
              if (ppr == NULL)
               {
                /* ErrPostEx(SEV_ERROR,0,0, "GetName: NULL ProtRef pointer."); */
                return NULL;
               }
              pvn = ppr->name;
              if (pvn == NULL)
               {
                 /* ErrPostEx(SEV_ERROR,0,0, "GetName: NULL name valnode pointer."); */
                 return NULL;
               }
              namestr = pvn->data.ptrvalue;
              if (namestr == NULL)
               {
                 /* ErrPostEx(SEV_ERROR,0,0, "GetName: NULL name pointer."); */
                 return NULL;
               }
           }
          psf = psf->next;
         }
       }
      psa = psa->next;
    }
   return namestr;
}


/* KM 260304 change of input type */
CharPtr GetTitle(BioseqPtr pbs, ValNodePtr pvndesset)
{
   SeqDescrPtr pdes = NULL, pdesset = NULL;
   CharPtr title = NULL, name = NULL, temp = NULL;

   /* SK: NULL ptr check */
   if ( pbs == NULL )
   {
       ErrPostEx(SEV_ERROR,0,0, "GetTitle: NULL pbs.");
       return NULL;
   }


   /* look for title from the Bioseq */ 
   pdes = pbs->descr;
   
   while (pdes != NULL && title == NULL)
   {
	 temp = ParseTitleFromSeqDesc( pdes, NULL, 0 );

	 if( temp != NULL )
	   title = temp;

	 pdes = pdes->next;
   } 
  
   if ((title == NULL) && ((name = GetName(pbs)) != NULL)) /* look in feature table in Bioseq */
       title = name;

   temp = NULL;
   if (title == NULL ) /* look in list of Bioseq-set descriptors */
    {
      while(pvndesset != NULL ) /* KM 260304 traverse ValNode of descriptors */
      { 
         pdesset = pvndesset->data.ptrvalue;
         while (pdesset != NULL)
         {
		   temp = ParseTitleFromSeqDesc( pdesset, NULL, 0 );

		   if( temp != NULL )
			 title = temp;

           pdesset = pdesset->next;
         }
         pvndesset = pvndesset->next;
      }
    }

   return title;

}

Boolean FillACCDBFromTextSeqId(BioseqPtr pbs, SeqIdPtr pSeqId,
							   CharPtr title)
{
  CharPtr dbName = GetDbFromSeqId( pSeqId, REFSEQ );
  TextSeqIdPtr pTextSeqId = NULL;
		 
  if( dbName )
  {
	pTextSeqId = pSeqId->data.ptrvalue;

	if (pTextSeqId == NULL)
	{
	  ErrPostEx(SEV_ERROR,0,0, "FillACCDBFromTextSeqId: "
				"NULL TextSeq-id pointer.");
	  return FALSE;
	}
  }

  if (!WriteACCDB(pbs, dbName, pTextSeqId, NULL, NULL, title, 0))
  {
	ErrPostEx(SEV_ERROR,0,0, "FillACCDBFromTextSeqId: WriteACCDB "
			  "failed.");
	return FALSE;
  }

  if( dbName )
	Free( dbName );

  return TRUE;
}
/* KM 260304 change of input type */
Boolean FillACCDB(BioseqPtr pbs, ValNodePtr pvndesset)
{
   Char title[ TITLE_LENGTH ];
   SeqIdPtr psi = NULL;
   SeqIdPtr psiThis = NULL;
   TextSeqIdPtr ptsi = NULL;
   PDBSeqIdPtr ppsi = NULL;
   DbtagPtr pdt = NULL;
   Boolean  bWroteRecord = FALSE;
   /* Int4 gi = 0;*/

   /* SK: NULL ptr check */
   if ( pbs == NULL )
   {
         ErrPostEx(SEV_ERROR,0,0, "FillACCDB: NULL pbs.");
         return FALSE;
   }

   CreateDefLine( NULL, pbs, title, TITLE_LENGTH, 0, NULL, NULL );

      /* process Seq-id valnode and call WriteACCDB with database info */
   psi = pbs->id;
   if (psi == NULL)
   {
	 ErrPostEx(SEV_ERROR,0,0, "FillACCDB: NULL Seq-id pointer.");
	 return FALSE;
   }
   psiThis = psi;
   while (psiThis != NULL)
   {
	 /*---------------------------------------------------
	   SeqIds that are TextSeqIds.
	   --------------------------------------------------*/
	 if ( IsTextSeqId( psiThis ) )
	 {
	   bWroteRecord = FillACCDBFromTextSeqId( pbs, psiThis, title );
	 }
	 
	 if (psiThis->choice == SEQID_PDB)  /* PDB */
	 {
	   ppsi = psiThis->data.ptrvalue;
	   if (ppsi == NULL)
	   {
		 ErrPostEx(SEV_ERROR,0,0, "FillACCDB: NULL PDBSeqId pointer.");
		 return FALSE;
	   }
	   if (!WriteACCDB(pbs, "pdb", NULL, ppsi, NULL, title, 0))
	   {
		 ErrPostEx(SEV_ERROR,0,0, "FillACCDB: WriteACCDB failed.");
		 return FALSE;
	   }
	   bWroteRecord = TRUE;
	 }
	 /*---------------------------------------------------------
	   SeqIds that are integers: Geninfo backbone seqid and
	   GenInfo backbone moltype.
	   --------------------------------------------------------*/
	 if (psiThis->choice == SEQID_GIBBSQ ||
		 psiThis->choice == SEQID_GIBBMT )  
	 {
	   CharPtr dbName = GetDbFromSeqId( psiThis, REFSEQ );
	   
	   if( dbName )
	   {
		 Int4 id = psiThis->data.intvalue;

		 if (!WriteACCDB(pbs, dbName, NULL, NULL, pdt, title, id))
		 {
		   ErrPostEx(SEV_ERROR,0,0, "FillACCDB: WriteACCDB failed.");
		   return FALSE;
		 }
	   }
	   bWroteRecord = TRUE;

	   MemFree( dbName );
	 }
	 if (psiThis->choice == SEQID_GENERAL)  /* General dbtag for other databases */
	 {
	   pdt = psiThis->data.ptrvalue;
	   if (pdt == NULL)
	   {
		 ErrPostEx(SEV_ERROR,0,0, "FillACCDB: NULL Dbtag pointer.");
		 return FALSE;
	   }
	   
	   if (!WriteACCDB(pbs, "other", NULL, NULL, pdt, title, 0))
	   {
		 ErrPostEx(SEV_ERROR,0,0, "FillACCDB: WriteACCDB failed.");
		 return FALSE;
	   }
	   bWroteRecord = TRUE;
	 }
	 psiThis = psiThis->next;
   }

   /* if WriteACCDB was not called yet ...*/
   if (!bWroteRecord )
   {
	 if (!WriteACCDB(pbs, "n/a", NULL, NULL, NULL, title, 0))
	 {
	   ErrPostEx(SEV_ERROR,0,0, "FillACCDB: WriteACCDB failed.");
	   return FALSE;
	 }
   }
   return TRUE;
}



CharPtr GetType (BioseqPtr pbs)
{

   /* SK: NULL ptr check */
   if ( pbs == NULL )
      return NULL;

   switch (pbs->mol)
       {
        case Seq_mol_dna:
          return "DNA";
        case Seq_mol_rna:
           return "RNA";
        case Seq_mol_aa:
            return "protein";
        case Seq_mol_na:
            return "NA";
        case Seq_mol_other:
            return "other";
        default:
           return "not specified";
       }
  /* return NULL;*/
}


Boolean FillASNDB(BioseqPtr pbs, ValNodePtr pvndesset, CharPtr division, CharPtr release)
{

   Int4 gi = 0;
   CharPtr ptype = NULL;

   /* SK: NULL ptr check */
   if ( pbs == NULL )
   {
      ErrPostEx(SEV_ERROR,0,0, "FillASNDB: NULL pbs.");
      return FALSE;
   }

   ResetStASNDB(&pas);
   /* SK: NULL ptr check */
   if ( pas == NULL )
   {
      ErrPostEx(SEV_ERROR,0,0, "FillASNDB: NULL pas.");
      return FALSE;
   }

   ResetStPARTI(&ppa);

   /* SK: NULL ptr check */
   if ( ppa == NULL )
   {
      ErrPostEx(SEV_ERROR,0,0, "FillASNDB: NULL ppa.");
      return FALSE;
   }

   pas->asn1 = pbs;

   if ((gi = GetGI(pbs)) == 0)
    {
       ErrPostEx(SEV_ERROR,0,0, "FillASNDB: GetGI failed.");
       return FALSE;
    }
   else
    {
      pas->gi = gi;
      ppa->gi = gi;
    }


   if ((ptype = GetType(pbs)) == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "FillASNDB: Call to GetType failed.");
      return FALSE;
    }
   else
	 pas->type = ptype;

    pas->division = division;
    pas->release = release;
    ppa->partition = division;

    AppendRecordASNDB(pas);


   insert_count = insert_count + 1;
   AppendRecordPARTI(ppa);
   insert_count = insert_count + 1;
   return TRUE;
}

/* do this for gbhur and gbmor - RNA files for mouse and human
and all the chromosome files for mouse and human (hs_chr*.asn and mm_chr*.asn);
it extracts RNA-protein and DNA-protein pairs for NUCPROT table */
ValNodePtr GetProtFromHumanMouse(BioseqPtr pbs) {

    SeqAnnotPtr psa = NULL;
    SeqIdPtr psid = NULL;
    SeqLocPtr psl = NULL;
    Int4 igi = 0;
    SeqFeatPtr psf = NULL;
    ValNodePtr pvngi = NULL;

    if(pbs == NULL)
      return NULL;
    psa = pbs->annot;
    if (psa == NULL)
    {
       ErrPostEx(SEV_ERROR,3,1,"GetProtFromHumanMouse: NULL pointer to sequence annotation.");
       return NULL;
    }
    while (psa != NULL)
    {
      if (psa->type == 1) /* ftable */
      {
        psf = (SeqFeatPtr) psa->data;
        if (psf == NULL)
        {
	   ErrPostEx(SEV_ERROR,3,1,"GetProtFromHumanMouse: NULL pointer to sequence feature table.");
	   return NULL;
        }
        while(psf != NULL)
        {
          if(psf->data.choice == SEQFEAT_CDREGION)
          {
	    if (psf->product != NULL)
	    {
	       psl = (SeqLocPtr) psf->product;
	       while (psl != NULL)
	       {
		  if(psl->choice == SEQLOC_WHOLE)
		  {
		     psid = psl->data.ptrvalue;
		     while (psid != NULL)
		     {
		       if (psid->choice == SEQID_GI)
		       {
			  igi = psid->data.intvalue;
			  if (igi <= 0)
		          {
			     ErrPostEx(SEV_ERROR,0,0, "GetProtFromHumanMouse: Invalid GI found.");
			     return NULL;
			  }
			  else
			    ValNodeAddInt(&pvngi, 0, igi);
		       }
		       psid = psid->next;
		     }
		  }
		  psl = psl->next;
	       }
	    }
	  }
	  psf = psf->next;
	}
      }
      psa = psa->next;
    }
    return pvngi;
}

/* ============================================================= 
   Complete Genome functions.
   ============================================================= */

/***************************************************************/ 
/** GetGenomeInfoFromSeqDescr
 *
 * Extracts complete genome info from a seq-descr of type 
 * biosource. 

 *
 * @param pDescrList  List of seq-descrs to search.
 *
 * @param pRecord     StChromPtr object to be filled.
 *
 * @return            the chromosome number as a string.
 *
 */
CharPtr GetGenomeInfoFromSeqDescr( ValNodePtr pDescrList,
								   StChromPtr pRecord,
								   CharPtr * pDivision)
{
  CharPtr chromosomeNumber = NULL;
  ValNodePtr pList         = pDescrList;
  SeqDescPtr pDesc         = NULL;
  BioSourcePtr pSource     = NULL;

  while( pList )
  {
	pDesc = pList->data.ptrvalue;
	
	while( pDesc != NULL )
	{
	  if( pDesc->choice == Seq_descr_source )
	  { 	  
		pSource = pDesc->data.ptrvalue;
	  
		chromosomeNumber = 
		  GetGenomeInfoFromBioSource( pSource, pRecord, pDivision );
		
		break;
	  }
	  pDesc = pDesc->next;
	}

	pList = pList->next;
  }

  /*-------------------------------------------------------------
	If we have not found the division, then look for a GBBlockPtr
	from which to extract the division.
	------------------------------------------------------------*/
  if( *pDivision == NULL )
  {
	GetDivisionFromGBBlock( pDescrList, pDivision );
  }

  return chromosomeNumber;
}

/***************************************************************/ 
/** GetGenomeInfoFromBiosource
 *
 * Extracts complete genome info from a biosource
 * If the chromosome is unknown, then the negative value
 * of the taxid is returned. 
 *
 * @param pSource  The biosource to be parsed.
 *
 * @param pRecord  StChromPtr object to be filled.
 *
 * @return         the chromosome number.
 *
 */
CharPtr GetGenomeInfoFromBioSource( BioSourcePtr pSource,
								 StChromPtr pRecord,
								 CharPtr *pDivision )
{
  SubSourcePtr pSubSource = NULL;
  OrgRefPtr    pOrg       = NULL;
  OrgNamePtr   pOrgName   = NULL;

  CharPtr      taxName    = NULL;
  CharPtr      chromosomeNumber = NULL;
  Int4         chromId    = 0;

  if( !pSource || !pRecord ) return chromosomeNumber;
		
  pOrg = pSource->org;

  if( pOrg )
  {
	pRecord->taxid = GetTaxIdFromOrgRef( pOrg );

	pRecord->chromfl = pSource->genome;

	/*-----------------------------------------------------------------
	  Build the name.
	  ---------------------------------------------------------------*/
	if( pOrg->taxname )
    {
	  taxName = StringSave( pOrg->taxname );

	  pSubSource = pSource->subtype;


	  chromosomeNumber = AddSubTypeDetails( taxName, pSubSource, 
											pRecord);
	}

	if( pOrg->orgname )
	{
	  *pDivision = StringSave( IsNullStr( pOrg->orgname->div ) );
	}

  }
  
  return chromosomeNumber;
}

void GetDivisionFromGBBlock( ValNodePtr pDescr, 
							 CharPtr * pDivision )
{
  ValNodePtr pList    = pDescr;
  SeqDescPtr pDesc    = NULL;
  GBBlockPtr pGBBlock = NULL;

  while( pList )
  {
	pDesc = pList->data.ptrvalue;

	while( pDesc != NULL )
	{
	  if( pDesc->choice == Seq_descr_genbank )
	  { 	  
		pGBBlock = pDesc->data.ptrvalue;

		if( pGBBlock )
		{
		  *pDivision = StringSave( IsNullStr( pGBBlock->div ) );
		}

		break;
	  }
	  pDesc = pDesc->next;
	}
	pList = pList->next;
  }

}

/***************************************************************/ 
/** AddSubTypeDetails
 *
 *  Adds infromation from the subsource ptr to the name.
 *
 * @param pName       The name to be modified.
 *
 * @param pSubSource  The SubSource from which to extract the info.
 *
 * @param pRecord     StChromPtr structure containing the chromflag
 *                    and name.
 * 
 * @return            The value of the chromosome name.
 *
 */

CharPtr AddSubTypeDetails( CharPtr pName, 
						   SubSourcePtr pSubSource, 
						   StChromPtr pRecord )
{
  SubSourcePtr pSub = pSubSource;

  CharPtr      pXmeName = NULL;
  CharPtr      pTmpName = NULL;
  CharPtr      pSubName = NULL;
  Int4         xmeType  = pRecord->chromfl;
  Boolean      foundSubtype = FALSE;
  CharPtr      pChromosome = NULL;
  

  if( !pName )
	return NULL;

  /*---------------------------------------------------------------
	If we have no chromosome type, default it to genomic. Later, if
	we're dealing with a segmented subtype, change it.
	---------------------------------------------------------------*/
  if( xmeType == 0 )
  {
	xmeType = SOURCE_GENOMIC;
  }

  /*---------------------------------------------------------------
	Loop through the SubSourcePtrs to find one with genomic info.
	---------------------------------------------------------------*/
  while( pSub )
  {
	if( pSub->subtype == SUBSRC_CHROMOSOME ||
		pSub->subtype == SUBSRC_PLASMID ||
		pSub->subtype == SUBSRC_TRANSPOSON ||
		pSub->subtype == SUBSRC_INSERTION_SEQ ||
		pSub->subtype == SUBSRC_PLASTID ||
		pSub->subtype == SUBSRC_SEGMENT ||
		pSub->subtype == SUBSRC_ENDOGENOUS_VIRUS )

	{
	  foundSubtype = TRUE;

  /*---------------------------------------------------------------
	Add the chromosome type to the name. We need to do this
	seperately from the chromFlag because there is no chromflag
	for SEGMENT.
	---------------------------------------------------------------*/
	  if( pSub->subtype == SUBSRC_SEGMENT )
	  {
		xmeType = SUBSRC_SEGMENT;
	  }

	  /*------------------------------------------------------------
		If we have not found a chromosome type as yet, use the SubSource
		subtype.
		-----------------------------------------------------------*/
	  if( pRecord->chromfl == 0 )
	  {
		pRecord->chromfl 
		  = AssignChromflagFromSubtype( pSub->subtype );

	  }
	  
	  pXmeName = AddChromosomeTypeToName( pName, xmeType );

	  /*---------------------------------------------------------------
		Add the subtype info to the name.
		---------------------------------------------------------------*/
	  if( pSub->name )
	  {
		pChromosome = StringSave( pSub->name );

		pTmpName   = MakeName(pXmeName, " " );

		if( pXmeName )
		{
		  Free( pXmeName );
		  pXmeName = NULL;
		}	  
		pSubName   = MakeName( pTmpName, pSub->name );
	  
		if( pTmpName ) Free( pTmpName );
		break;
	  }
	}
	pSub = pSub->next;
  }

  /*---------------------------------------------------------------
	If no SubSourcePtr was found, add the chromosome type to 
	the name.
	---------------------------------------------------------------*/
  if( !foundSubtype )
  {
	 pXmeName = AddChromosomeTypeToName( pName, xmeType );
  }

  if( pSubName )
	pRecord->name = pSubName;
  else if ( pXmeName )
	pRecord->name = pXmeName;
  else
	pRecord->name = StringSave( pName );

  return pChromosome;
}

Int4  AssignChromflagFromSubtype( Int4 subType )
{
  Int4 chromFlag = 0;

  switch( subType )
  {
    case SUBSRC_CHROMOSOME:
	  chromFlag = SOURCE_GENOMIC;
	  break;
    case SUBSRC_PLASMID:
	  chromFlag = SOURCE_PLASMID;
	  break;
    case SUBSRC_TRANSPOSON:
	  chromFlag = SOURCE_TRANSPOSON;
	  break;
    case SUBSRC_INSERTION_SEQ:
	  chromFlag = SOURCE_INSERTION_SEQ;
	  break;
    case SUBSRC_PLASTID:
	  chromFlag = SOURCE_PLASTID;
	  break;
	  /*--------------------------------------------------
		Note that there is no chromflag for SEGMENT, so
		leave it unknown. 
	  ------------------------------------------------*/
    case SUBSRC_SEGMENT:
	  chromFlag = SOURCE_UNKNOWN;
	  break;
    case SUBSRC_ENDOGENOUS_VIRUS:
	  chromFlag = SOURCE_ENDOGENOUS_VIRUS;
	  break;
  }

  return chromFlag;
}

/***************************************************************/ 
/** AddChromosomeTypeToName
 *
 * @param name    The name to be modified.
 *
 * @param XmeType The chromosome type.
 *
 * @return The modified name.
 *
 */
CharPtr AddChromosomeTypeToName( CharPtr name, Int4 XmeType )
{
  CharPtr typeName    = NULL;
  CharPtr tmpName  = NULL;
  
  tmpName = MakeName( name, " " );

  switch( XmeType )
  {
    case SOURCE_GENOMIC:
	  typeName = MakeName( tmpName, "Chromosome" );
	  break;
    case SOURCE_CHLOROPLAST:
	  typeName = MakeName( tmpName, "Chloroplast" );
	  break;
    case SOURCE_CHROMOPLAST:
	  typeName = MakeName( tmpName, "Chromoplast" );
	  break;
    case SOURCE_KINETOPLAST:
	  typeName = MakeName( tmpName, "Kinetoplast" );
	  break;
    case SOURCE_MITOCHONDRION:
	  typeName = MakeName( tmpName, "Mitochondria" );
	  break;
    case SOURCE_PLASTID:
	  typeName = MakeName( tmpName, "Plastid" );
	  break;
    case SOURCE_MACRONUCLEAR:
	  typeName = MakeName( tmpName, "Macronuclear" );
	  break;
    case SOURCE_EXTRACHROM:
	  typeName = MakeName( tmpName, "Extrachromosome" );
	  break;
    case SOURCE_PLASMID:
	  typeName = MakeName( tmpName, "Plasmid" );
	  break;
    case SOURCE_TRANSPOSON:
	  typeName = MakeName( tmpName, "Transposon" );
	  break;
    case SOURCE_INSERTION_SEQ:
	  typeName = MakeName( tmpName, "Insertion Sequence" );
	  break;
    case SOURCE_CYANELLE:
	  typeName = MakeName( tmpName, "Cyanelle" );
	  break;
    case SOURCE_PROVIRAL:
	  typeName = MakeName( tmpName, "Proviral" );
	  break;
    case SOURCE_VIRION:
	  typeName = MakeName( tmpName, "Virion" );
	  break;
    case SOURCE_NUCLEOMORPH:
	  typeName = MakeName( tmpName, "Nucleomorph" );
	  break;
    case SOURCE_APICOPLAST:
	  typeName = MakeName( tmpName, "Apicoplast" );
	  break;
    case SOURCE_LEUCOPLAST:
	  typeName = MakeName( tmpName, "Leucoplast" );
	  break;
    case SOURCE_PROPLASTID:
	  typeName = MakeName( tmpName, "Proplastid" );
	  break;
    case SOURCE_ENDOGENOUS_VIRUS:
	  typeName = MakeName( tmpName, "Endogenous Virus" );
	  break;
    case SUBSRC_SEGMENT:
	  typeName = MakeName( tmpName, "Segment" );
	  break;
    default:
	  typeName = MakeName( tmpName, " " );
	break;
	  
  }

  MemFree( tmpName );

  return typeName;
}

/***************************************************************/ 
/** GetGenusAndSpecies
 *
 *  Extracts the genus and species names from an OrgRefPtr.
 *
 * @param pOrg     The OrgrefPtr that contains the info.
 *
 * @return         The name.
 *
 */
CharPtr GetGenusAndSpecies( OrgRefPtr pOrg )
{
  CharPtr      fullName    = NULL;

  if(  NULL == pOrg  )
	return NULL;

  if( pOrg->taxname )
  {
	fullName = StringSave( pOrg->taxname );
  }

  return fullName;
}

/***************************************************************/ 
/** MakeName
 *
 * Utility function to concatenate 2 strings.
 * 
 * @param firstName  The first string.
 *
 * @param secondName The second string.
 *
 * @return The concatenated string.
 *
 */
CharPtr MakeName( CharPtr firstName, CharPtr secondName )
{
  int firstLength  = 0;
  int secondLength = 0;
  CharPtr fullName = NULL;

   if( firstName && secondName )
  {
	firstLength = StringLen( firstName );
	secondLength = StringLen( secondName );

	fullName = 
	  ( CharPtr )Malloc( (firstLength + secondLength + 1) *
						 sizeof( Char ) );

	if( !fullName )
	{
	  ErrPostEx(SEV_ERROR,0,0,"MakeName: "
				"Unable to allocate fullName.");
	}
	else
	{
	  StringCpy( fullName, firstName );
	  StringCat( fullName, secondName );
	}
	}

  return fullName;
}

/***************************************************************/ 
/** GetTaxidFromOrgRef
 *
 *  Extracts taxid from an OrgRefPtr.
 *
 * @param pOrg     The OrgrefPtr that contains the info.
 *
 * @return         The taxid.
 *
 */
Int4 GetTaxIdFromOrgRef( OrgRefPtr pOrg )
{
  ValNodePtr  pvnDb  = NULL;
  DbtagPtr    pDbTag = NULL;
  ObjectIdPtr pObjectId = NULL;

  Int4 taxId = 0;

  if( pOrg == NULL )
	return taxId;

  pvnDb = pOrg->db;

  while( pvnDb != NULL )
  {
	pDbTag = pvnDb->data.ptrvalue;

	if( pDbTag && StringStr( pDbTag->db, "taxon" ) != NULL )
	{
	  pObjectId = pDbTag->tag;

	  if( pObjectId )
	  {
		taxId = pObjectId->id;
		break;
	  }
	}
	pvnDb = pvnDb->next;
  }

  return taxId;
}

/***************************************************************/ 
/** GetChromIdFromBioseq
 *
 *  Extracts NCBI's Chrom Id from a bioseq.
 *
 * @param pBioseq     The Bioseq that contains the info.
 *
 * @return            The Chrom Id.
 *
 */
Int4 GetChromIdFromBioseq( BioseqPtr pBioseq )
{
  Int4     chromid = 0;
  SeqIdPtr pSeqId = NULL;
  DbtagPtr pDbTag = NULL;

  if( NULL == pBioseq )
	return chromid;

  pSeqId = pBioseq->id;

  while( pSeqId )
  {
	if( pSeqId->choice == SEQID_GENERAL )
	{
	  pDbTag = pSeqId->data.ptrvalue;

	  if( pDbTag)
	  {
		if( StringCmp( pDbTag->db,"NCBI_GENOMES" ) == 0 )
		{
		  if( pDbTag->tag )
		  {
			chromid = pDbTag->tag->id;
			break;
		  }
		}
	  }
	}
	pSeqId = pSeqId->next;
  }
  return chromid;
}

/***************************************************************/ 
/** GetChromIdFromBioseqSet
 *
 *  Extracts NCBI's Chrom Id from a bioseqSet.
 *
 * @param pBioseqSet  The BioseqSet that contains the info.
 *
 * @return            The Chrom Id.
 *
 */
Int4 GetChromIdFromBioseqSet( BioseqSetPtr pSet, 
							  CharPtr * pXmeNumber )
{
  ValNodePtr pDesc     = NULL;
  UserObjectPtr pUser  = NULL;
  UserFieldPtr  pField = NULL;
  Int4 chromid = 0;

  if( !pSet ) return NULL;

  pDesc = pSet->descr;

  while( pDesc )
  {
	if( pDesc->choice == Seq_descr_user )
	{
	  pUser = pDesc->data.ptrvalue;

	  while( pUser )
	  {
		if( StringCmp( pUser->_class, "Genomes" ) == 0 )
		{
		  pField = pUser->data;

		  if( pField->label->id == 1 )
		  {
			chromid = pField->data.intvalue;
		  }
		  break;
		}
		pUser = pUser->next;
	  }
	}
	else if( pDesc->choice == Seq_descr_source )
	{
	  StChromPtr pRecord = StChromNew();
	  CharPtr pDivision = NULL;


	  if( !pRecord )
	  {
		ErrPostEx(SEV_ERROR,0,0, "GetChromIdFromBioseqSet: "
			  "failed to allocate CHROM record.");
		return chromid;
	  }

	  *pXmeNumber = 
		GetGenomeInfoFromBioSource( pDesc->data.ptrvalue,
									pRecord, &pDivision );
	}

	pDesc = pDesc->next;
  }
  return chromid;
}


/***************************************************************/ 
/** GetGenomeInfo
 *
 * Extracts complete genome info from the bioseq. If the chromid
 * is greater than 0 or the Chromosome name is "Un", the chrom
 * table will be filled 
 *
 * @param pBioseq   The bioseq to be parsed.
 *
 * @param pSeqDescr A list of SeqDescr to be searched.
 *
 * @param chromId   NCBI's chromId if one exists.
 *
 * @returns         The kloodge (our chromID).
 *
 */

CharPtr GetGenomeInfo( BioseqPtr pBioseq, ValNodePtr pSeqDescr, 
				    Int4 chromId, CharPtr * pDivision )
{
  CharPtr xmeNumber = NULL;
  Int4 kloodge = 0;
  StChromPtr pChromRecord = StChromNew();


  if( !pChromRecord )
  {
	ErrPostEx(SEV_ERROR,0,0, "GetGenomeInfo: failed to allocate CHROM record.");
	return xmeNumber;
  }

  xmeNumber = GetGenomeInfoFromSeqDescr( pSeqDescr, 
									   pChromRecord, pDivision );

  pChromRecord->access = Misc_GetAccession( pBioseq );

  if( pChromRecord->access == NULL )
	pChromRecord->access == StringSave( "n/a" );
  if( pChromRecord->name == NULL )
	pChromRecord->name == StringSave( "n/a" );

  if( chromId > 0  )
  {
	pChromRecord->chromid = chromId;
	pChromRecord->chromnum = StringSave( xmeNumber );
	AppendRecordCHROM( pChromRecord );
  }

  if( xmeNumber != NULL )
  {

  	/*----------------------------------------------------------
	  If XmeNumber = Un then write this to CHROM. Note that
	  the access will be the first record that we come across
	  referring to Chromosome Un.
	  ---------------------------------------------------------*/
	if( StringCmp( xmeNumber, "Un" ) == 0 )
	{
	  pChromRecord->chromid = -( pChromRecord->taxid );
	  pChromRecord->chromnum = StringSave( xmeNumber );

	  free( pChromRecord->access );
	  pChromRecord->access = NULL;
	  AppendRecordCHROM( pChromRecord );
	}
  }
  StChromFree( pChromRecord );
  
  return xmeNumber;
}

/***************************************************************/ 
/** FillcontigChromid
 *
 *  Writes all gis in the ValNodePtr to contigkloodge along with
 *  the kloodge.
 *
 * @param pvnContigGis List of contig gis to be written to 
 *                     contigkloodge.
 *
 * @param kloodge      The kloodge to be written to the table.
 *
 */  
void FillContigChromid( ValNodePtr pvnContigGis, Int4 topGi,
						   Int4 chromid, Int4 update )
{
  ValNodePtr pGis = pvnContigGis;
  StContigchromidPtr pRecord = StContigchromidNew();

  if( chromid == 0 )
  { 
	StContigchromidFree(pRecord);
	return;
  }

 if ( pRecord == NULL )
 {
   ErrPostEx(SEV_INFO,0,0, "Fillcontigchromid: "
			 "StContigkloodgeNew failed.\n");
 }
 else
 {
   while( pGis != NULL )
   {
	 pRecord->contiggi = pGis->data.intvalue;
	 pRecord->topgi = topGi;
	 pRecord->chromid = chromid;
	 pRecord->changed = update;

	 if( !ReplaceRecordCONTIGCHROMID(pRecord ) )
	   ErrPostEx(SEV_INFO,0,0, "Fillcontigchromid: "
				 "Failed to insert or replace record for %d.\n",
				 pRecord->contiggi);

	
	 ResetStCONTIGCHROMID( &pRecord );

	 pGis = pGis->next;
   }
   StContigchromidFree(pRecord);
 }
}

void FillGiChromosome( ValNodePtr pvnComgenGis, CharPtr xmeNumber )
{
  ValNodePtr pGis = pvnComgenGis;

  /*------------------------------------------------------------
	Do nothing if the chromId is 0.
	----------------------------------------------------------*/
  if( xmeNumber == NULL ) 
  {
	return;
  }

  while( pGis != NULL )
  {
	WriteGichromosome( pGis->data.intvalue, xmeNumber );
	
	pGis = pGis->next;
  }
}

void WriteGichromosome( Int4 gi, CharPtr xmeNumber )
{
  StGichromPtr pRecord = StGichromNew();

  if( !pRecord )
  {
	ErrPostEx(SEV_INFO,0,0, "WriteGichromosome: "
			  "Unable to allocate memory.\n");
	return;
  }

  if( gi < 0 || NULL == xmeNumber )
  {
	ErrPostEx(SEV_WARNING,0,0, "WriteGichromosome: "
			  "Bad input %d %s.\n", gi, xmeNumber);

	StGichromFree( pRecord );
	return;
  }

  pRecord->gi = gi;
  pRecord->chromnum = StringSave( xmeNumber );

  if( !AppendRecordGICHROMOSOME( pRecord ) )
  {
	ErrPostEx(SEV_INFO,0,0, "WriteGichromosome: "
			  "Failed to append record for %d.\n", pRecord->gi);

  }
  StGichromFree( pRecord );
}

void FillContigChromosome( ValNodePtr pvnComgenGis, 
						   CharPtr xmeNumber )
{
  ValNodePtr pGis = pvnComgenGis;
  StContigchromPtr pRecord = StContigchromNew();

  if( xmeNumber == NULL ) 
  {
	StContigchromFree( pRecord );
	return;
  }
 if ( pRecord == NULL  )
 {
   ErrPostEx(SEV_INFO,0,0, "FillContigchromosome: "
			 "StContigchromoNew failed.\n");
 }
 else
 {
   while( pGis != NULL )
   {
	 pRecord->contiggi = pGis->data.intvalue;
	 pRecord->chromnum = StringSave( xmeNumber );

	 if( !AppendRecordCONTIGCHROMOSOME( pRecord ) )
	 {
	   ErrPostEx(SEV_INFO,0,0, "FillContigchrom: "
				 "Failed to append record for %d.\n",
				 pRecord->contiggi);
	 }

	 ResetStCONTIGCHROM( &pRecord );

	 pGis = pGis->next;
   }
   StContigchromFree(pRecord);
 }
}

/* =============================================================
   Name-related functions.
   ==============================================================*/
/***************************************************************/ 
/** SetNameInfo( *
 * Assign values to the member variables of a NAMEINFO.
 *
 * @param pName   The name info to be filled.
 *
 * @param pCList  The CDRLIST to be added.
 *
 * @param pLList  The LOCUSLIST to be added.
 *
 * @param pbss    The bioseq-set to be added.
 *
 * @param getCdrInfo The getInfo flag.
 */
void    SetNameInfo( NAMEINFO *pName, CDRLIST *pCList, 
                     LOCUSLIST *pLList, BioseqSetPtr pbss,
                      Boolean getInfo )
{
  pName->pCdrList   = pCList;
  pName->pLocusList = pLList;
  pName->last_pbss  = pbss;
  pName->getCdrInfo = getInfo;
}

/***************************************************************/ 
/** CreateCdrInfo()
 *
 *  Creates a CDRINFO and returns a pointer to it. The returned
 *  CDRINFO must be destroyed by the caller.
 * 
 *  @param newGin     nucleotide gi for the new CDRINFO.
 *
 *  @param newFrom    from value for the new CDRINFO.
 *
 *  @param newTo      to value for the new CDRINFO.
 *
 *  @param newAccess  accession value for the new CDRINFO.
 *
 *  @return           returns the newly created CDRINFO.
 */
CDRINFO   *CreateCdrInfo( Int4 newGin, Int4 newFrom, Int4 newTo, 
                          Int4 newGip, CharPtr newAccess )
{
  CDRINFO *pInfo = NULL;

  pInfo = Malloc( sizeof( CDRINFO ) );

  if( pInfo == NULL )
  {
    ErrPostEx(SEV_ERROR,0,0,"CreateCdrInfo: Cannot allocate "
              "memory for new CDRINFO.");
  }
  else
  {
    pInfo->gin = newGin;
    pInfo->from = newFrom;
    pInfo->to = newTo;
    pInfo->gip = newGip;

    pInfo->access = StringSave( newAccess );
  }
  return pInfo;
}

/***************************************************************/ 
/** DestroyCDRInfo
 *
 *  Destroys a CDRINFO.
 *
 *  @param pInfo  CDRINFO to be destroyed
 */
void       DestroyCdrInfo( Pointer *pData )
{
  CDRINFO *pInfo = ( CDRINFO * )pData;

  if( pInfo->access )
    Free ( pInfo->access );
  MemFree( pInfo );
}

/*************************************************************** 
 ** CreateLocusInfo
 *
 *  Creates a LOCUSINFO and returns a pointer to it. The returned
 *  LOCUSINFO must be destroyed by the caller. 
 * 
 *  @param newGin     nucleotide gi for the new LOCUSINFO
 *
 *  @param newFrom    from value for the new LOCUSINFO
 *
 *  @param newTo      to value for the new LOCUSINFO
 *
 *  @return           returns the newly created LOCUSINFO.
 */
LOCUSINFO *CreateLocusInfo( Int4 newGi, Int4 newFrom, Int4 newTo,
                            CharPtr newName )
{
  LOCUSINFO *pInfo = NULL;

  pInfo = Malloc( sizeof( LOCUSINFO ) );

  if( pInfo == NULL )
  {
    ErrPostEx(SEV_ERROR,0,0,"CreateLocusInfo: Cannot allocate "
              "memory for new LOCUSINFO.");
  }
  else
  {
    pInfo->gin = newGi;
    pInfo->from = newFrom;
    pInfo->to = newTo;
    pInfo->name = StringSave( newName );

    pInfo->pSynonyms = NULL;
    pInfo->synCount = 0;
  }

  return pInfo;
}

/***************************************************************/ 
/** AddSynonymToLocus
 *
 * Add a synonym to a given locus info.
 * 
 * @param pInfo    The locus info to which the synynym should be added.
 *
 * @param pSynonym The synynym to add.
 *
 * @returns 0 on success and 1 on failure.
 */
 
Int4 AddSynonymToLocus( LOCUSINFO *pInfo, CharPtr pSynonym )
{
  Int4 synCount = pInfo->synCount;
  Int4 success = 1;

  pInfo->pSynonyms = 
    Realloc( pInfo->pSynonyms, 
	     sizeof( CharPtr * ) * ( synCount + 1 ) );

  if( !pInfo->pSynonyms  )
  {
    ErrPostEx(SEV_ERROR,0,0,"AddSynonymToLocus: Cannot "
                "allocate more synonyms.");
  }
  else
  {
    pInfo->pSynonyms[ synCount ] = StringSave( pSynonym );
    pInfo->synCount++;

    success = 0;
  }
  return success;
}
/***************************************************************/ 
/** DestroyLocusInfo
 *
 *  Destroys a LOCUSINFO.
 *
 *  @param pInfo  LOCUSINFO to be destroyed
 */
void DestroyLocusInfo( Pointer *pLocusInfo )
{
  LOCUSINFO *pInfo = ( LOCUSINFO * )pLocusInfo;
  int i;
  /* -------------------------------------------------------
     Destroy the locus name, if there is one.
     ------------------------------------------------------*/
  if( pInfo->name )
  {
    
    Free( pInfo->name );
    pInfo->name = NULL;
    }
  /* -------------------------------------------------------
     Destroy any synonyms.
     ------------------------------------------------------*/
  if( pInfo->pSynonyms )
  {
    for( i = 0; i < pInfo->synCount; i++ )
    {
      MemFree( pInfo->pSynonyms[ i ] );
      

    } 
   MemFree( pInfo->pSynonyms );
    pInfo->pSynonyms = NULL;
  }

  MemFree( pInfo );
}

NameListPtr CreateNameList(CharPtr newDb, CharPtr newAccess, 
						   Int4 newGi, CharPtr dateDep, CharPtr actionDate)
{
  NameListPtr pList = NULL;

  pList = ( NameListPtr )Malloc( sizeof( NAMELIST ) );

  if( !pList )
  {
	ErrPostEx(SEV_ERROR,0,0,"CreateProteinNameNode: Cannot allocate "
              "memory for new NameList.");
  }

  pList->pNames     = NULL;
  pList->pCurrent   = NULL;
  pList->db         = StringSave(  IsNullStr( newDb ) );
  pList->access     = StringSave(  IsNullStr( newAccess ) );
  pList->gi         = newGi;
  pList->dateDep    = StringSave( IsNullStr( dateDep ) );
  pList->actionDate = StringSave( IsNullStr( actionDate ) );

  return pList;
}

void DestroyNameList( NameListPtr pList )
{
  if( pList->db ) MemFree( pList->db );
  if( pList->access ) MemFree( pList->access );
  if( pList->dateDep ) MemFree( pList->dateDep );
  if( pList->actionDate ) MemFree( pList->actionDate );

  if( pList->pNames )
	StBionameFree( pList->pNames );

 MemFree( pList );
}
 
void AddNameToList( NameListPtr * pNameList, CharPtr name,
					Int4 nameType, Int4 field, Int4 ruleId )
					  
{
  NameListPtr pList = *pNameList;
  StBionamePtr pCurrentName = pList->pNames;
  Boolean found = FALSE;

  /*--------------------------------------------------------------
	Make sure we do not already have this name, nametype, field 
	and ruleid. Used in the case of proteins where a preprocessed 
	protein may have protein names for the mature protein.
	-------------------------------------------------------------*/
  while( pCurrentName )
  {
	if( ( StringCmp( pCurrentName->name, name ) == 0 ) &&
		pCurrentName->nametypeid == nameType &&
		pCurrentName->field == field &&
		pCurrentName->ruleid == ruleId )
	{
	  found = TRUE;
	  break;
	}

	pCurrentName = pCurrentName->next;
  }
  
  if( !found )
  {
	FillBionameRecord( &(pList->pNames), 0, 0, name, nameType, 
					   pList->db, pList->access, pList->gi, field,
					   0, 0,  pList->dateDep, ruleId, ADDED, 
					   pList->actionDate);
  }
}

/*************************************************************** 
 ** CreateProteinNameNode
 *
 *  Creates a PROTEINNAME and returns a pointer to it. The returned
 *  PROTEINNAME must be destroyed by the caller. Note that all
 *  nodes are created with an action of ADDED. This is changed
 *  if we are updating. 
 * 
 *  @param pNewName   The name of the new PROTEINNAME.
 *
 *  @param newField   The field where this name was found.
 */
ProteinNamePtr CreateProteinNameNode( CharPtr pNewName, Int4 type,
									  Int4 newField )
{
  ProteinNamePtr pName = NULL;

  pName = ( ProteinNamePtr )Malloc( sizeof( PROTEINNAME ) );

  if( !pName )
  {
	ErrPostEx(SEV_ERROR,0,0,"CreateProteinNameNode: Cannot allocate "
              "memory for new PROTEINNAME.");
  }
  pName->next     = NULL;
  pName->name     = StringSave( pNewName );
  pName->field    = newField;
  pName->nameType = type;
  pName->action   = ADDED;

  return pName;
}

/***************************************************************/ 
/** DestroyProteinName
 *
 *  Destroys a ProteinName. Frees the name and field members if
 *  neccessary.
 *
 *  @param pInfo  pointer to the PROTEINNAME to be destroyed
 */
void DestroyProteinNameList( ProteinNamePtr pName )
{
  ProteinNamePtr pCurrent = pName;
  ProteinNamePtr pTemp = NULL;

  while( pCurrent )
  {
	pTemp = pCurrent->next;

	if( pCurrent->name )
	  Free( pCurrent->name );

	MemFree( pCurrent );

	pCurrent = pTemp;
  }
}

/***************************************************************/ 
/** CreateCdrList
 *
 *  Creates a new CDRList. Should be freed with DestroyCdrList() when 
 *  finished.
 *
 *  returns a CDRLIST with a null root node.
 */
CDRLIST *CreateCdrList()
{
  CDRLIST *pList = NULL;

  pList = ( CDRLIST * )Malloc( sizeof( CDRLIST ) );

  if( !pList )
  {
     ErrPostEx(SEV_ERROR,0,0,"CreateCdrList: Cannot allocate "
              "memory for new CDRList.");
  }
  pList->count = 0;
  pList->cdrRoot = NULL;
  pList->funPack = 
    RBTree_FunPack_New( (RBTree_CompareFunc)RBTree_CompareIntKeyFunction,
                        (RBTree_KeyFreeFunc)RBTree_IntKeyFree, 
                        (RBTree_DataFreeFunc)DestroyCdrInfo );
  return pList;
}

/***************************************************************/ 
/** CreateLocusList
 *
 *  Creates a new LOCUSLIST. Should be freed with DestroyLocusList()
 *  when finished.
 *
 *  returns a LOCUSLIST.
 */
LOCUSLIST *CreateLocusList()
{
LOCUSLIST *pList = NULL;
  
  pList = Malloc( sizeof( LOCUSLIST ) );

  if( !pList )
  {
     ErrPostEx(SEV_ERROR,0,0,"CreateLocusList: Cannot allocate "
              "memory for new LocusList.");
  }
  pList->count = 0;
  pList->locusRoot = NULL;
  pList->funPack = 
    RBTree_FunPack_New( (RBTree_CompareFunc)RBTree_CompareIntKeyFunction,
                        (RBTree_KeyFreeFunc)RBTree_IntKeyFree, 
                        (RBTree_DataFreeFunc)DestroyLocusInfo );
 
  return pList;
}
/***************************************************************/ 
/** DestroyCdrList
 *
 * @param pList CDRLIST to be destroyed
 */								
void DestroyCdrList( CDRLIST *pList )
{
  RBTree_Free( pList->cdrRoot, pList->funPack );
  RBTree_FunPack_Free( pList->funPack );
  MemFree( pList );
}

/***************************************************************/ 
/** DestroyLocusList
 *
 * @param pList LOCUSLIST to be destroyed.
 *
 */
void DestroyLocusList(LOCUSLIST *pList )
{

  RBTree_Free( pList->locusRoot, pList->funPack );
  RBTree_FunPack_Free( pList->funPack );
  MemFree( pList );
}

/***************************************************************/ 
/** AddCdrInfoToList
 *
 *  Add a CDRINFO to the given CDRLIST.
 *
 *  @param pList  The CDRLIST to which the CDRINFO should be added.
 *
 *  @param pInfo  The CDRINFO to add.
 *
 *  @return returns TRUE(1) on success and FALSE(0) on failure.
 */
Boolean AddCdrInfoToList( CDRLIST *pList, CDRINFO *pInfo )
{
  Boolean added;

  added = RBTree_AddInt( &(pList->cdrRoot), pInfo->gin,
                         ( Pointer ) pInfo, pList->funPack );

  if( added )
  {
    pList->count++;
  }
  else
  {
    ErrPostEx(SEV_ERROR,0,0,"AddCdrInfoToList: Unable to add "
              "new CDRINFO..");
  }
  
  return added;
}

/***************************************************************/ 
/** AddLocusInfoToList
 *
 *  Add a LOCUSINFO to the given LOCUSLIST.
 *
 *  @param pList  The LOCUSLIST to which the LOCUSINFO should be added.
 *
 *  @param pInfo  The LOCUSINFO to add.
 *
 *  @return returns TRUE(1) on success and FALSE(0) on failure.
 */
Boolean AddLocusInfoToList( LOCUSLIST *pList, LOCUSINFO *pInfo )
{
  Boolean added;

  added = RBTree_AddInt( &(pList->locusRoot), pInfo->gin,
                         ( Pointer ) pInfo, pList->funPack );

  if( added )
  {
    pList->count++;
  }
  else
  {
    ErrPostEx(SEV_ERROR,0,0,"AddlocusInfoToList: Unable to add "
              "new LOCUSINFO..");
  }
  
  return added;
}

/***************************************************************/ 
/** ParseCDRInfoFromAnnotation
 *
 *  Extracts info from bioseqset annotation.
 *
 *  @param pSeqAnnot Annotation that contains the CD Regions.
 *
 *  @param bioseqGI  Gi from a bioseq, -1 if the annotaion is
 *                   from a bioseqset.
 *  @param pList     CDRLIST to which the info should be 
 *                   written.
 *  @return          0 on success and 1 on failure.
 */
Int4 ParseCDRInfoFromAnnotation( SeqAnnotPtr pSeqAnnot, Int4 bioseqGI, 
                                 CDRLIST *pList, ValNodePtr *ppvnGiList )
{
  Int4        success   = 1;
  SeqFeatPtr  pSeqFeat  = NULL;
  SeqLocPtr   pSeqLoc   = NULL;
  Int4        gi        = 0;
/* ------------------------------------------------------------
     Look for the feature table while there is still a node in the
     annotation.
     ----------------------------------------------------------- */
  if( pSeqAnnot == NULL )
    return success; 

  if (pSeqAnnot->type == ANNOT_TABLE)
  {
    pSeqFeat = (SeqFeatPtr) pSeqAnnot->data;
    if (pSeqFeat == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "ParseCDRInfoFromAnnotation: "
                "NULL SeqFeat pointer %ld.", bioseqGI);
      return success;
    }

  /* -----------------------------------------------------------------
    While there are still nodes in the feature table, 
    look for the CDRegion data.
    ----------------------------------------------------------------*/      
    while (pSeqFeat != NULL) 
    {
	  GetGisFromSeqFeat( pSeqFeat, ppvnGiList );

/* --------------------------------------------------------------------
       Only parse the CDR Info if it's not a pseudo gene.
    ------------------------------------------------------------------- */	  
      if (pSeqFeat->data.choice == SEQFEAT_CDREGION && 
	  pSeqFeat->pseudo == FALSE )	
      {	
		success = 
		  ParseCDRInfoFromFeatureTable( pSeqFeat, bioseqGI, 
										pList );
      }
      pSeqFeat = pSeqFeat->next;
    }
  }

  return success;
}

Boolean CheckName( CharPtr pName )
{
  Boolean goodName = TRUE;
  CharPtr pCopy = StringSave( pName );
  CharPtr pStart = pCopy;

  StringCpy( pName, pCopy );

  /* -------------------------------------------------------------
	 Convert the string to upcase.
     ------------------------------------------------------------*/
  for(; *(pCopy); pCopy++ )
  {
	*pCopy = toupper( *pName );
  }
  
  /* -------------------------------------------------------------
	 Convert the string to upcase.
     ------------------------------------------------------------*/
  if( StringStr( pCopy, "UNKOWN" ) || /*NOTE: Typo found in RefSeq. */
	  StringStr( pCopy, "UNKNOWN" ) ||
	  StringStr( pCopy, "HYPOTHETICAL" ) ||
	  StringStr( pCopy, "ORF" ) )
	goodName = FALSE;

  MemFree( pStart );
  
  return goodName;
}
/***************************************************************/ 
/** ParseCdrInfo
 *
 *  Looks from for coding region data in the feature table of
 *  either a bioseqset or a bioseq. For each cdr, it gets the
 *  gi of the protein, the from location, the to location and
 *  the gi of the nucleotide that encodes the coding region.
 *
 *  @param pbss   bioseqset to process.  If this is NULL then 
 *                pbs must not be NULL.
 *
 *  @param pbs    bioseq to process. Only processed if pbss is NULL.
 *
 *  @param pList  CDRLIST to which the info should be added.
 *
 *  @returns 0 on success, 1 if the bioseqset does not have
 *           annotation and -1 if there's something really wrong.
 */
Int4 ParseCDRInfo( BioseqSetPtr pbss, BioseqPtr pbs, 
				   CDRLIST *pList, ValNodePtr PNTR ppvnGis)
{
  SeqAnnotPtr pSeqAnnot      = NULL;
  Int4        bioseqGI = -1;
  Int4        success  = 1;

  /* -------------------------------------------------------------
     Usually CDR info will be in the BioseqSet. Check that first.
     ------------------------------------------------------------*/
  if ( pbss )
  {
    pSeqAnnot = pbss->annot;
    /* -------------------------------------------------------
     If there is no annotation here, return 1 so that we can
     set a flag to check the bioseq.
     ------------------------------------------------------ */
    if (pSeqAnnot == NULL)
    {
      return success;
    }
  }
  /*--------------------------------------------------------------
    Look for CDR info in the Bioseq.
    --------------------------------------------------------------*/
  else if( pbs )
  {
    if (pbs->annot == NULL)
    {
       ErrPostEx(SEV_WARNING,0,0, "ParseCDRInfo: NULL SeqAnnot pointer "
                "for both BioseqSet and Bioseq.");
       return -1; 
    }
      
    pSeqAnnot = pbs->annot; 
    bioseqGI = GetGI( pbs );
  }
  else
  {
    ErrPostEx(SEV_WARNING,0,0, "ParseCDRInfo: Both Bioseq and  "
              "BioseqSet have a NULL SeqAnnot pointer " );
    return -1; 
  }

  if( pSeqAnnot )
  {
    success = ParseCDRInfoFromAnnotation( pSeqAnnot, bioseqGI,
										  pList, ppvnGis );
  }
	
  return success;

}

/***************************************************************/ 
/** ParseCDRInfoFromFeatureTable
 *
 *  Finds the protein gi, from and to locations from a CDRegion
 *  SeqFeatPtr. Returns 1 on success and 0 on failure.
 *
 *  @param psf   SeqFeatPtr from which to extract the info.
 *
 *  @param gi    Gi of the BioseqSet or Bioseq from which
 *               this SeqFeatPtr is derived. Only for error
 *               purposes.
 *  @param pList CDRLIST to which the information should be added.
 *
 *  @return      0 on success, -1 on failure.
 *
 */    
Int4 ParseCDRInfoFromFeatureTable( SeqFeatPtr psf, Int4 bioseqGI,
                                   CDRLIST *pList )
{

  SeqFeatPtr pSeqFeat        = psf;
  SeqLocPtr  pSeqLoc         = NULL;
  Int4       gi              = 0;
  Int4       floc            = 0;
  Int4       tloc            = 0;
  Int4       gis             = 0;
  /* ValNodePtr pdbvn           = NULL; */
  Boolean    transSpliced    = FALSE;
  CDRINFO   *pInfo           = NULL;
  Int4       success         = -1;

  /*-----------------------------------------------------------------
    Do nothing if pSeqFeat is not of type CDREGION.
    --------------------------------------------------------------*/
  if (pSeqFeat->data.choice != SEQFEAT_CDREGION )
    return success; 

  /* ------------------------------------------------------------
       Is there trans-splicing going on?
    ------------------------------------------------------------*/
  if( pSeqFeat->excpt && pSeqFeat->excpt == TRUE && pSeqFeat->except_text
      && !strcmp( pSeqFeat->except_text, "trans splicing" ) )
  {
     transSpliced = TRUE;
  }	
  
  /*-----------------------------------------*/
  /*get the gi of the product of the CDregion*/
  /*-----------------------------------------*/
  pSeqLoc = pSeqFeat->product;
  if (pSeqLoc == NULL)/*the product field of the seqfeat struct points to nothing*/
  {
     ErrPostEx(SEV_ERROR,0,0, "ParseCDRInfoFromFeatureTable: "
                  "No product SeqLoc pointer for %ld.", bioseqGI);
  }
  else
  {
    gi = GetGiFromProductLocation( pSeqLoc );
  }
						
  /*--------------------------------------*/
  /*get the location info for the CDregion*/
  /*--------------------------------------*/
  pSeqLoc = pSeqFeat->location;
  if (pSeqLoc == NULL)/*the location field of the seqfeat struct points to nothing*/
  {
    ErrPostEx(SEV_ERROR,0,0, "ParseCDRInfoFromFeatureTable: NULL SeqLoc pointer.");
  }
		     
  GetSourceLocationInfo( pSeqLoc, &gis, &floc, &tloc, transSpliced );

  /*----------------------------------------*/
  /* add the results if non-zero       */
  /* remember 0 is the first base of a DNA sequence */
  /*----------------------------------------*/
  if (gi > 0 && gis > 0 && floc > -1 && tloc > -1)
  {
    pInfo = CreateCdrInfo( gis, floc, tloc, gi, "test" );
    if( pInfo )
    {
      success = AddCdrInfoToList( pList, pInfo );
    }

  }
  else
  {
    if (pSeqFeat->pseudo == FALSE)/*only report missing gi's for cdr's for non-pseudogenes)*/
    {
      ErrPostEx(SEV_ERROR,0,0, "ParseCDRInfoFromFeatureTable: "
		"gi = %ld, location: %ld to %ld, gis = %ld", 
		gis, floc, tloc, gis);
    }
  }

  return success;
}
   


/**************************************************************
      GetLocationInfoFromSeqInt:
      Find the gi, from location, to location from a 
      SeqLoc of type SeqInt.
      

      Parameters: psl - the input SeqLocPtr.
                  pgi - pointer to the gi location to be filled.
                  pfloc - pointer to the from location.
                  ptloc - pointer to the to location.
      
      Returns 0 on success, 1 on failure. 
*******************************************************************/     
Int4 GetLocationInfoFromSeqInt( SeqLocPtr psl, Int4Ptr pgi,
                                Int4Ptr pfloc, Int4Ptr ptloc )
{
    SeqLocPtr pSeqLoc = psl;
    SeqIntPtr pSeqInt =	NULL;
    SeqIdPtr  pSeqID  =	NULL;
    Int4      success = 0;

    /*--------------------------------------------------------
      Do nothing if the SeqLocPtr does not point to a SeqInt.
      --------------------------------------------------------*/
    if (pSeqLoc->choice != SEQLOC_INT) 
        return 1;

    pSeqInt = pSeqLoc->data.ptrvalue; /*a seq-interval struct is of type valnode */
    if (pSeqInt == NULL)/*the pointer to a seq-interval struct for sequence location points to nothing*/
    {
	ErrPostEx(SEV_ERROR,0,0, "GetLocationInfoFromSeqInt: NULL SeqId pointer.");
        success = 1;
    }
    else
    {
	/*get location info*/
	*pfloc = pSeqInt->from; 
	*ptloc = pSeqInt->to;
	      				
	/*get gi*/
	 pSeqID = pSeqInt->id; /*WARNING: Seq-id used to be optional in the Seq-interval struct*/
	 *pgi = GetGiFromSeqId( pSeqID );
    }
    return success;
}

/**************************************************************
      GetLocationInfoFromPackedInt:
      Find the gi, from location, to location from a 
      SeqLoc of type PackedInt.
      

      Parameters: psl - the input SeqLocPtr.
                  pgi - pointer to the gi location to be filled.
                  pfloc - pointer to the from location.
                  ptloc - pointer to the to location.
                  transSpliced - indicates that the product is
                  derived from an mRNA that is trans-spliced.
      
      Returns 0 on success, 1 on failure. 
*******************************************************************/     
Int4 GetLocationInfoFromPackedInt( SeqLocPtr psl, Int4Ptr pgi, Int4Ptr pfloc,
                                  Int4Ptr ptloc, Boolean transSpliced )
{
    SeqLocPtr pSeqLocm       =	psl;
    SeqLocPtr pSeqLocm_first =	NULL;
    SeqLocPtr pSeqLocm_last  =	NULL;
    SeqIntPtr pSeqInt        =	NULL;
    SeqIdPtr  pSeqID         =	NULL;
    Int2      strand         =  0;
    Int4      success        =  0;
    /* Int4      choice         =  psl->choice; */
    /*----------------------------------------------------------
     Do nothing if psl is not of type MIX or PACKED_INT.
     ----------------------------------------------------------*/
    if ( psl->choice != SEQLOC_PACKED_INT) 
        return 1;

    pSeqLocm = psl->data.ptrvalue;
    
    if( pSeqLocm  == NULL ) return 1;
    
    /*get the first location value*/
    pSeqLocm_first = pSeqLocm;
    if (pSeqLocm_first->choice == SEQLOC_INT) /*only if pSeqLocm_from is a seq-int struct*/
      {
	pSeqInt = pSeqLocm_first->data.ptrvalue;
        if (pSeqInt != NULL)
        {
       	    strand = pSeqInt->strand;
	    if (pSeqInt->strand == PLUS || pSeqInt->strand == UNKNOWN) /*treat unknown strand as plus strand*/
	    {
	        *pfloc = pSeqInt->from;/*from location is the from location in the first sequence interval*/
	    }
	    else if (pSeqInt->strand == MINUS) /*minus strand*/
	    {
	        *ptloc = pSeqInt->to; /*to location is the to location in the first sequence interval*/
	    }

	    /*get gi of source DNA*/
	    pSeqID = pSeqInt->id; /*WARNING: Seq-id used to be optional in the Seq-interval struct*/
	    *pgi = GetGiFromSeqId( pSeqID );
	}
    }/*only if pSeqLocm_first is a seq-int struct*/
    else
    {
        ErrPostEx(SEV_WARNING,0,0, "GetLocationInfoFromPackedInt: Non seqint in finding 'from' "
                  "position for gi source: %ld (pSeqLocm_first->choice= %d)", 
                  *pgi, pSeqLocm_first->choice);
        success = 1;
    }					
		   					
    /*get the last location value*/
    pSeqLocm_last = pSeqLocm;
    while (pSeqLocm_last != NULL)
    {
	if (pSeqLocm_last->choice == SEQLOC_INT) /*only if pSeqLocm_last is a seq-int struct*/
	{
	    pSeqInt = pSeqLocm_last->data.ptrvalue;
	    if (pSeqInt != NULL)
	    {
		if (strand == pSeqInt->strand || transSpliced == TRUE)
		{
		    if (pSeqInt->strand == PLUS || pSeqInt->strand == UNKNOWN) /*treat unknown as plus strand*/
			 *ptloc = pSeqInt->to;/*to location is the to location of the last sequence interval*/
		    if (pSeqInt->strand == MINUS) /*minus strand*/
			 *pfloc = pSeqInt->from;/*from location is the from location of the last sequence interval*/
		}
                else
		{
		    ErrPostEx(SEV_WARNING,0,0, "GetLocationInfoFromPackedInt: "
                              "Strandedness of first and last sequence "
                              "intervals do not match. gi source: %ld first strand = %d second strand %d)\n", 
                              *pgi, strand, pSeqInt->strand);
                    success = 1;
		}
	    }
	}/*only if pSeqLocm_last is a seq-int struct*/
	else
	{
	    ErrPostEx(SEV_ERROR,0,0, "GetLocationInfoFromPackedInt: Non seqint in "
                      "finding 'to' position for gi source: %ld (pSeqLocm_last->choice= %d)\n", 
                      *pgi, pSeqLocm_last->choice);
            success = 1;
        }
									
        pSeqLocm_last = pSeqLocm_last->next;
    }/*while pSeqLocm_last != NULL*/

    return success;
}

/**************************************************************
      GetLocationInfoFromSeqMix:
      Find the gi, from location, to location from a 
      SeqLoc of type Mix.

      SeqLocs of type NULL, EMPTY, INT and PNT are handled. 
      Others may be handled later.

      Parameters: psl - the input SeqLocPtr.
                  pgi - pointer to the gi location to be filled.
                  pfloc - pointer to the from location.
                  ptloc - pointer to the to location.
                  transSpliced - indicates that the product is
                  derived from an mRNA that is trans-spliced.
      
      Returns 0 on success, 1 on failure. 
*******************************************************************/     
Int4 GetLocationInfoFromSeqMix( SeqLocPtr psl, Int4Ptr pgi, Int4Ptr pfloc,
                                  Int4Ptr ptloc, Boolean transSpliced )
{
    SeqLocPtr pSeqLocm       =	psl;
    SeqLocPtr pSeqLocm_first =	NULL;
    SeqLocPtr pSeqLocm_last  =	NULL;
    SeqIntPtr pSeqInt        =	NULL;
    SeqIdPtr  pSeqID         =	NULL;
    Int2      strand         =  0;
    Int4      success        =  0;
    /* Int4      choice         =  psl->choice; */
    /*----------------------------------------------------------
     Do nothing if psl is not of type MIX or PACKED_INT.
     ----------------------------------------------------------*/
    if ( psl->choice != SEQLOC_MIX) 
        return 1;

    pSeqLocm = psl->data.ptrvalue;
    
    if( pSeqLocm  == NULL ) return 1;
    
    /*get the first location value*/
    pSeqLocm_first = pSeqLocm;
    
    while ( pSeqLocm_first->choice == SEQLOC_NULL ||
            pSeqLocm_first->choice == SEQLOC_NULL )
      pSeqLocm_first = pSeqLocm->next;
            
    if (pSeqLocm_first->choice == SEQLOC_INT) /*only if pSeqLocm_from is a seq-int struct*/
    {
	pSeqInt = pSeqLocm_first->data.ptrvalue;
        if (pSeqInt != NULL)
        {
       	    strand = pSeqInt->strand;
	    if (pSeqInt->strand == PLUS || pSeqInt->strand == UNKNOWN) /*treat unknown strand as plus strand*/
	    {
	        *pfloc = pSeqInt->from;/*from location is the from location in the first sequence interval*/
	    }
	    else if (pSeqInt->strand == MINUS) /*minus strand*/
	    {
	        *ptloc = pSeqInt->to; /*to location is the to location in the first sequence interval*/
	    }

	    /*get gi of source DNA*/
	    pSeqID = pSeqInt->id; /*WARNING: Seq-id used to be optional in the Seq-interval struct*/
	    *pgi = GetGiFromSeqId( pSeqID );
	}
    }
    else if (pSeqLocm_first->choice == SEQLOC_PNT) /*only if pSeqLocm_from is a seq-int struct*/
    {
        SeqPntPtr pPnt = pSeqLocm_first->data.ptrvalue;
        
        strand = pPnt->strand;
        if( pPnt->strand == PLUS || pPnt->strand == UNKNOWN )
	{
	    *pfloc = pPnt->point;
        }
        else if (pPnt->strand == MINUS) /*minus strand*/
	{ 
	    *ptloc = pPnt->point;
	}
        /*get gi of source DNA*/
	pSeqID = pPnt->id; /*WARNING: Seq-id used to be optional in the Seq-interval struct*/
	*pgi = GetGiFromSeqId( pSeqID );
    }
    else
    {
        ErrPostEx(SEV_ERROR,0,0, "GetLocationInfoFromSeqMix: SeqLoc type wrong in finding 'from' "
                  "position for gi source: %ld (pSeqLocm_first->choice= %d)", 
                  *pgi, pSeqLocm_first->choice);
        success = 1;
    }					
		   					
    /*get the last location value*/
    pSeqLocm_last = pSeqLocm;
    while (pSeqLocm_last != NULL)
    {
	if (pSeqLocm_last->choice == SEQLOC_INT) /*only if pSeqLocm_last is a seq-int struct*/
	{
	    pSeqInt = pSeqLocm_last->data.ptrvalue;
	    if (pSeqInt != NULL)
	    {
		if (strand == pSeqInt->strand || transSpliced == TRUE)
		{
		    if (pSeqInt->strand == PLUS || pSeqInt->strand == UNKNOWN) /*treat unknown as plus strand*/
			 *ptloc = pSeqInt->to;/*to location is the to location of the last sequence interval*/
		    if (pSeqInt->strand == MINUS) /*minus strand*/
			 *pfloc = pSeqInt->from;/*from location is the from location of the last sequence interval*/
		}
                else
		{
		    ErrPostEx(SEV_ERROR,0,0, "GetLocationInfoFromSeqMix: "
                              "Strandedness of first and last sequence "
                              "intervals do not match. gi source: %ld first strand = %d second strand %d)\n", 
                              *pgi, strand, pSeqInt->strand);
                    success = 1;
		}
	    }
	}
        else if (pSeqLocm_last->choice == SEQLOC_PNT) /*only if pSeqLocm_from is a seq-int struct*/
        {
            SeqPntPtr pPnt = pSeqLocm_last->data.ptrvalue;
            
            if( strand == pPnt->strand || transSpliced == TRUE )
	    {
                if( pPnt->strand == PLUS || pPnt->strand == UNKNOWN )
	        {
	            *pfloc = pPnt->point;
                }
                else if (pPnt->strand == MINUS) /*minus strand*/
	        { 
	            *ptloc = pPnt->point;
	        }
            }
            else
	    {
		ErrPostEx(SEV_ERROR,0,0, "GetLocationInfoFromSeqMix: "
                          "Strandedness of first and last sequence "
                          "intervals do not match. gi source: %ld first strand = %d second strand %d)\n", 
                          *pgi, strand, pPnt->strand);
                success = 1;
	    }
        }
        /*--------------------------------------------------------------
          If the seqloc is NULL, or empty, then just move on the the
	    next seqloc. Otherwise, raise a warning.
	    --------------------------------------------------------------*/
	else if ( pSeqLocm_last->choice != SEQLOC_NULL &&
                  pSeqLocm_last->choice != SEQLOC_EMPTY )
	{
	    ErrPostEx(SEV_WARNING,0,0, "GetLocationInfoFromSeqMix: SeqLoc type wrong in "
                      "finding 'to' position for gi source: %ld (pSeqLocm_last->choice= %d)\n", 
                      *pgi, pSeqLocm_last->choice);
            success = 1;
        }
									
        pSeqLocm_last = pSeqLocm_last->next;
    }/*while pSeqLocm_last != NULL*/

    return success;
}

/**************************************************************/
/** GetSourceLocationInfo
 *
 *  Find the source gi, from location, to location from a 
 *  SeqLocPtr. Handles SeqLocs of type SeqInt, Mix and Packed Int.
 *     
 *  @param psl           the input SeqLocPtr
 *
 *  @param pgi           pointer to the gi to be filled
 *
 *  @param pfloc         pointer to the from.
 *
 *  @param ptloc         pointer to the to location.
 *
 *  @param transSpliced  indicates that the product is
 *                       derived from an mRNA that is trans-spliced.
 *
 *  return 0 on success, 1 on failure.
 */
Int4 GetSourceLocationInfo( SeqLocPtr psl, Int4Ptr pgi, Int4Ptr pfloc, 
                              Int4Ptr ptloc, Boolean transSpliced )
{
  SeqLocPtr pSeqLoc = psl;
    
  /* SeqLocPtr pSeqLocm_first = NULL; */
  /* SeqLocPtr pSeqLocm_last  = NULL; */
  /* SeqIntPtr pSeqInt        = NULL; */
  /* SeqIdPtr  pSeqID         = NULL; */
  /* Int2      strand         = 0; */
  Int4      success        = 0;

  while (pSeqLoc != NULL)/*look through the sequence location nodes for a seq-interval- there is probably ony one node*/
  {
    if (pSeqLoc->choice == SEQLOC_INT) /*then pSeqLoc->data.ptrvalue points to a seq-interval struct*/
    {
      success = 
	GetLocationInfoFromSeqInt( pSeqLoc, pgi, 
				   pfloc, ptloc );
    }
    else if ( pSeqLoc->choice == SEQLOC_MIX )
    {
      success = 
	GetLocationInfoFromSeqMix( pSeqLoc, pgi, pfloc,
				   ptloc, transSpliced );
    }
    else if ( psl->choice == SEQLOC_PACKED_INT ) 
    {
      success = 
	GetLocationInfoFromPackedInt( pSeqLoc, pgi, pfloc,
				      ptloc, transSpliced );
    }
    pSeqLoc = pSeqLoc->next;
  }
  return success;
}

/****************************************************************/
/** ParseNamesFromBioseq
 *
 *  Gets names from a bioseq. From a protein bioseq, it gets gi,
 *  accession and name (either title or description). From a
 *  NucProt bioseq, it gets locus names and locus synonyms.
 * 
 *  @param pbs        Bioseq from which to extract the name.
 *
 *  @param pNameInfo  The lists of names.
 * 
 *  returns 0 on success, 1 on failure. 
 */
Boolean ParseNamesFromBioseq( BioseqPtr pbs, NAMEINFO *pNameInfo ,
							  ValNodePtr PNTR ppvnGis)
{

  Int4	         function_report = 0;
  NameListPtr    pNames          = NULL;
  CharPtr        acc_ptr         = NULL;
  Int4	         gi              = 0;
  
  /*---------------------------------------------------------------
    If the gi is bad, move on to the next bioseq.
   --------------------------------------------------------------*/
  if ((gi = GetGI(pbs)) <= 0)
  {
    ErrPostEx(SEV_ERROR,0,0, "ParseNamesFromBioseq: GetGI failed.");
    return 0; 
  }

  /*---------------------------------------------------------------
    It's a protein bioseq.
   --------------------------------------------------------------*/	
  if (pbs->mol == Seq_mol_aa)
  {
	acc_ptr = Misc_GetAccession(pbs);
/*---------------------------------------------------------------
    If the accession is bad, move on to the next bioseq.
   --------------------------------------------------------------*/
    if (acc_ptr == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "ParseNamesFromBioseq: "
		 "GetAccession failed for gi= %ld\n", gi);
       return 0; 
    }
#ifdef ODBC

    /*--------------------------------------------------
     Get the protein names.
     ------------------------------------------------*/
	pNames = CreateNameList( "ref", acc_ptr, gi, 
							 DEFAULTDATE, DEFAULTDATE );

	 ParseProteinNames( pbs, &pNames );

	/*--------------------------------------------------
     Get the title from the bioseq.
     ------------------------------------------------*/
	ParseTitleFromSeqDesc( pbs->descr, &pNames, 
						   FLD_BIOSEQ_TITLE );

	/*--------------------------------------------------
     Get the title from the bioseq set description.
     ------------------------------------------------*/
	ParseTitleFromSeqDesc( pNameInfo->pDescr, &pNames, 
						   FLD_SEQSET_TITLE );

  /*---------------------------------------------------------------		
    Note that all records added to the bioentity table are 
    proteins for now. 
   --------------------------------------------------------------*/

	WriteProteinInfo( "ref", acc_ptr, gi, pNames, FLD_BIOSEQ );


	DestroyNameList( pNames );

#endif /* if ODBC is defined */
  

    MemFree( acc_ptr );
  }
   /*-------------------------------------------------------------
     if the current Bioseq is not a protein get locus name/location 
     info and then move on to the next Bioseq
   --------------------------------------------------------------*/
  else if (pbs->mol != 3) 
  {			
    /*extract locus name and location info from the feature table*/
    function_report = ParseLocusInfo(pbs, pNameInfo->last_pbss, 
                                     pNameInfo->pLocusList);
    if (function_report != 0)
    {
      ErrPostEx(SEV_WARNING, 0, 0, "ParseLocusInfo failed for "
		"gi = %ld: function report= %d\n", gi, 
		function_report);
    }
  }
  
/*---------------------------------------------------------------		
    if coding region data was not found in the BioseqSet, then look 
    for it in the Bioseq.
    ---------------------------------------------------------------*/
  if ( pNameInfo->getCdrInfo)
  {
	ParseCDRInfo(NULL, pbs, pNameInfo->pCdrList, ppvnGis);
  }

  return function_report;   
}

Int4  ParseProteinNames( BioseqPtr pbs, NameListPtr *pNames )
						 
{
  SeqAnnotPtr    pSeqAnnot = NULL;

  pSeqAnnot = pbs->annot;
  if (pSeqAnnot == NULL) 
  {
    ErrPostEx(SEV_ERROR,0,0, "ParseProtName: "
			  "NULL SeqAnnot pointer.\n");
    return -1;
  }
  
  while (pSeqAnnot != NULL) 
  {
    ParseNameFromAnnot( pSeqAnnot, pNames );
    pSeqAnnot = pSeqAnnot->next;
  }

  return 0;
}

Int4  ParseNameFromAnnot( SeqAnnotPtr pSeqAnnot, 
						  NameListPtr *pNames )
{
  SeqFeatPtr     pSeqFeat  = NULL;
  Int4           fieldType = 0;

  /*------------------------------------------------------------
    Do nothing if the annotation is not an annotation table.
    ------------------------------------------------------------*/
  if (pSeqAnnot->type != ANNOT_TABLE)
    return fieldType;
  
  pSeqFeat = (SeqFeatPtr) pSeqAnnot->data;

 /*------------------------------------------------------------
    Do nothing if there is no feature table.
    ------------------------------------------------------------*/
  if (pSeqFeat == NULL)
  {
    ErrPostEx(SEV_ERROR,0,0, "ParseNameFromAnnot: NULL SeqFeat "
	      "pointer.\n");
  }
  else
  {    
   /*----------------------------------------------------------
     Get all the names.
     ---------------------------------------------------------*/
    while ( pSeqFeat != NULL ) 
    {
      if ( pSeqFeat->data.choice == SEQFEAT_PROT ) 
      {
		ParseNameFromFeatureTable( pSeqFeat, pNames );
      }
      pSeqFeat = pSeqFeat->next;
    }
  }

  return fieldType;
}

/*note that ProtRef struct has other goodies like 
		  ec number, ids in other databases and whether or not the 
		  protein is mature or preprocessed*/
Int4  ParseNameFromFeatureTable( SeqFeatPtr pSeqFeat, 
								 NameListPtr * pNames )
{
  ProtRefPtr     pProtRef  = NULL;
  ValNodePtr     pvnName   = NULL;
  Uint1          processed = 0;
  CharPtr        name_ptr  = NULL;
  Int4           fieldType = 0;
  
  /*------------------------------------------------------------
    Do nothing if the feature table is not a prot-ref.
    ------------------------------------------------------------*/
  if (pSeqFeat->data.choice != SEQFEAT_PROT)
    return fieldType;
  
  pProtRef = pSeqFeat->data.value.ptrvalue; 

  /*------------------------------------------------------------
    Do nothing if there is no prot-ref.
    ------------------------------------------------------------*/
  if (pProtRef == NULL)
  {
    ErrPostEx(SEV_ERROR,0,0, "GetProtName: NULL ProtRef pointer.\n");
    return fieldType;
  }

  /*--------------------------------------------------------
    get names only of non-processed or mature proteins
    --------------------------------------------------------*/
  processed = pProtRef->processed;

  if (processed == NOTSET || processed == MATURE)
  {
    pvnName = pProtRef->name;                    
	
    /*-----------------------------------------------------
      sometimes there is a "description" (under desc) 
      instead of a name (See Prot-ref in ASN.1 browser)
     ------------------------------------------------------*/  
    if (pvnName == NULL)
    {
      name_ptr = pProtRef->desc;
      if (name_ptr == NULL)
      {
		ErrPostEx(SEV_WARNING,0,0, "GetProtName: NULL "
				  "name valnode pointer and NULL "
				  "description pointer.");
      }
      else
      {
		AddNameToList( pNames, name_ptr,  TYPE_PROTEIN,
					   FLD_PROT_DESC, RULE_NONE );
      }
    }

    while( pvnName != NULL ) 
    {
      name_ptr = pvnName->data.ptrvalue;    		
      
      if( name_ptr )
      {
		AddNameToList( pNames, name_ptr, TYPE_PROTEIN,
					   FLD_PROT_NAME, RULE_NONE );
      }
      pvnName = pvnName->next;
    }
  }
  else
  {
	  ErrPostEx(SEV_WARNING,0,0, "ParseProtName: "
				"Found a processed protein %ld", processed );
  }
  
  return fieldType;
}

/********************************************************/
/** ParseTitleFromSeqDesc
 *
 * Parses titless from a SeqDesc. If a ProteinNamePtr list is
 * supplied, then the name will be written  to the list with
 * the accompanying field value.
 *
 * Note that PDB names are not written to the list at this time.
 *
 * @param pSeqDesc   SeqDesc from which to extract the title.
 *
 * @param pNames     ProteinNamePtr list to write the names to.
 *                   May be NULL. 
 *
 * @param field      field from which we are obtaining the name.
 *                   May be either FLD_BIOSEQTITLE or FLD_SEQSET_TITLE.
 */
CharPtr ParseTitleFromSeqDesc( SeqDescPtr pSeqDesc, 
							   NameListPtr *pNames, Int4 field )
{
  CharPtr title         = NULL;
  PdbBlockPtr pPdbBlock = NULL;
  SeqDescPtr pDesc      = pSeqDesc;
  /*--------------------------------------------------
	Only parse names if the pSeqDesc is not null.
	------------------------------------------------*/
  while( pDesc != NULL )
  {
	if( pDesc->choice == Seq_descr_title )
	{
	  title = pDesc ->data.ptrvalue;

	  if( pNames )
	  {
		if( field == FLD_BIOSEQ_TITLE )
		  AddNameToList( pNames, title, TYPE_PROTEIN,
						 field, RULE_NONE );
		else if ( field == FLD_SEQSET_TITLE )
		  AddNameToList( pNames, title, TYPE_PROTEIN,
						 field, RULE_SEQSET );
	  }
	}
	else if( pDesc->choice == Seq_descr_pdb )
	{
	  pPdbBlock = pDesc->data.ptrvalue;
	  
	  if( pPdbBlock != NULL && pPdbBlock->compound != NULL )
	  {
		title = pPdbBlock->compound->data.ptrvalue;
		
		if( pNames )
		{
		  if( field == FLD_BIOSEQ_TITLE )
			AddNameToList( pNames, title, TYPE_PROTEIN,
						   FLD_BIOSEQ_PDB, RULE_NONE );
		  else if ( field == FLD_SEQSET_TITLE )
			AddNameToList( pNames, title, TYPE_PROTEIN,
						   FLD_SEQSET_PDB, RULE_SEQSET );
		}
	  }
	}
	pDesc = pDesc->next;
  }

  return title;
}

/********************************************************/
/** WriteProteinInfo
 *
 *  Adds the bioentity and writes the protein names to bioname.
 *
 *  @param db         Name of the database from which this protein
 *                    was derived. Always "ref" for now.
 *
 *  @param access     Accession number for this protein.
 *
 *  @param gi         gi for this protein.
 *
 *  @param pNameList  List of names for this protein.
 *
 *  @param bioentityfield  Field in which we found this protein.
 *                         This is always bioseq for now.
 */  
Int4 WriteProteinInfo( CharPtr db, CharPtr access, Int4 gi, 
					   NameListPtr pNameList, 
					   Int4 bioentityField )
{
  Int4 bioentityId = -1;
  Int4 bSuccess   = -1;
  StBioentityPtr pRecord = StBioentityNew();
  Int4 update= 0;

#ifdef ODBC
  /*----------------------------------------------------------
	Write the bioentity to the database.
    ----------------------------------------------------------*/
  bioentityId = 
	FillBioentityDB( BIND_PROTEIN, db, access, gi, 
					 bioentityField, &update);
#endif
  if( bioentityId )
  {
	
	WriteProteinNames( pNameList, bioentityId, db, access, 
					   gi, update );
	
  }

  StBioentityFree( pRecord );

  return bSuccess;
}

void WriteProteinNames( NameListPtr pNameList, Int4 bioentityId, 
						 CharPtr db, CharPtr access, 
						 Int4 identifier, Int4 update)
{
/*------------------------------------------------------
   Fill in the bioentity id.
    ----------------------------------------------------*/
  StBionamePtr pName = pNameList->pNames;
  while( pName )
  {
	pName->bioentityid = bioentityId;

	pName = pName->next;
  }

  UpdateNames( bioentityId, TYPE_PROTEIN, pNameList->pNames );

#ifdef UPDATE
  if( update )
  {
	UpdateNames( bioentityId, TYPE_PROTEIN, pNameList->pNames );
  }
  else
  {
 /*------------------------------------------------------
   Fill in the bioentity id.
    ----------------------------------------------------*/
    StBionamePtr pName = pNameList->pNames;
	while( pName )
	{
	  pName->bioentityid = bioentityId;

	  pName = pName->next;
	}

#ifdef ODBC
	AppendRecordBioname( pNameList->pNames );
#endif /* if ODBC defined */
  }
#endif /* if UPDATE defined */
}

/**************************************************************/
/** UpdateNames
 *
 * Gets all protein or gene names from the database for a given 
 * bioentityId and compares them with a given list. If a database
 * name is not found in the given list, then it is marked as deleted.
 * Names in pList that are not found in the database are added.
 *
 * @param bioentityId   bioentityId for which names should be found.
 *
 * @param nameType      Type of names to search for. Either gene
 *                      names or protein names.
 *
 * @param pList         List of names to be compared to the database
 *                      names.
 */
 
void  UpdateNames( Int4 bioentityId,  Int4 nameType, 
				   StBionamePtr pList )
{
#ifdef ODBC
  Boolean success          = TRUE;
  StBionamePtr pRecord     = StBionameNew();
  StBionamePtr pDBRecord   = NULL;
  StBionamePtr pUpdate     = NULL;
  StBionamePtr pAdd        = NULL;
  StBionamePtr pWorking    = NULL;
  StBionamePtr pListName   = NULL;

  pRecord->bioentityid = bioentityId;

  success = SearchBionameByBioentityId( &pRecord );

  /*-------------------------------------------------------------
    Compare each record found in the database with the name list.
    -------------------------------------------------------------*/
  if( success )
  {
	pDBRecord = pRecord;
	while( pRecord )
	{
	  Boolean found = FALSE;
	  pListName = pList;

	   /*--------------------------------------------------------
		 If the record is not for the right kind of name, move
		 on to the next record.
        -------------------------------------------------------*/
	  if( nameType != pRecord->nametypeid )
	  {
		pRecord = pRecord->next;
		continue;
	  }

	  while( pListName )
	  {
  /*-------------------------------------------------------------
    If a database name is found in the name list, then mark it as 
	UNCHANGED so that we don't add it to the database again and
	go on to the next name.
    -------------------------------------------------------------*/
  		if( StringCmp( pRecord->name, pListName->name ) == 0 &&
			pRecord->field == pListName->field )
		{
		  found = TRUE;
		  pListName->action = UNCHANGED;

		  if( pRecord->action == DELETED )
		  {
			FillBionameRecord( &pUpdate, pRecord->id, pRecord->bioentityid,
							   pRecord->name, pRecord->nametypeid,
							   pRecord->db, pRecord->access,
							   pRecord->identifier, pRecord->field,
							   pRecord->official, pRecord->deprecated,
							   pRecord->date, pRecord->ruleid,
							   ADDED, DEFAULTDATE );
		  }
		  break;
		}

		pListName = pListName->next;
	  }
 /*---------------------------------------------------------------
    If the database name is not found in the name list, add it to
	the update record so that its action can be changed to deleted.
    --------------------------------------------------------------*/
	  if( found == FALSE && pRecord->action == ADDED )
	  {

		FillBionameRecord( &pUpdate, pRecord->id, pRecord->bioentityid,
						   pRecord->name, pRecord->nametypeid,
						   pRecord->db, pRecord->access,
						   pRecord->identifier, pRecord->field,
						   pRecord->official, pRecord->deprecated,
						   pRecord->date, pRecord->ruleid,
						   DELETED, DEFAULTDATE );
	  }

	  pRecord = pRecord->next;
	}
  }
 /*--------------------------------------------------------
   Update those records that need updating.
     ------------------------------------------------------*/
  EditBionameById( pUpdate );

  /*-----------------------------------------------------
   Loop through the names list to see which names must be added.
   ------------------------------------------------------*/
  pListName = pList;
  pWorking = NULL;
  while( pListName )
  {
	if( pListName->action == ADDED )
	{

	  FillBionameRecord(  &pAdd ,0, bioentityId, pListName->name, 
						  pListName->nametypeid,  pListName->db,
						  pListName->access, pListName->identifier,
						  pListName->field, 0, 0, "00000000",
						  pListName->ruleid, ADDED, 
						  DEFAULTDATE );
	}

	pListName = pListName->next;
  }

  AppendRecordBioname( pAdd );
  
  StBionameFree( pRecord );
  StBionameFree( pUpdate ); 
  StBionameFree( pAdd ); 
#endif

}				

/**************************************************************/
/** FillBionameRecord
 *
 *  Creates and fills an StBioname object. Note that this muse
 *  be freed by the caller.
 *
 *  @param pList        Head of the StBionamePtr list to which the
 *                      new record should be added.
 *
 *  @param bionameId    The Id of the name. May be 0.
 *
 *  @param bioentityId  The bioentity Id.
 *
 *  @param pName        The name.
 *
 *  @param typeId       The type of name. It can be 2 for the protein
 *                      name or 1 for a locus name.
 *
 *  @param pDb          Database from which this record was found.
 *
 *  @param pAccess      Accesssion for this database.
 *
 *  @param gi           Identifier for this database. Gi for ncbi
 *                      databases.
 * 
 *  @param field        Field that this record is based on.
 *
 *  @param official     Is this name official? 0 for no, 1 for yes.
 *
 *  @param deprecated   Is this name deprecated? 0 for no, 1 for yes.
 *
 *  @param datedep      Date as a string that this name was deprecated. 
 *
 *  @param ruleId       Not currently used. Used for the rule used to
 *                      construct this name. 
 *
 *  @param action       Action performed on the name. Either Add or Delete.
 */
void FillBionameRecord( StBionamePtr * pList, Int4 bionameId,
						Int4 bioentityId, CharPtr pName,
						Int4 typeId, CharPtr pDb, CharPtr pAccess,
						Int4 gi, Int4 field, Int4 official,
						Int4 deprecated, CharPtr dateDep,
						Int4 ruleId, Int4 action, CharPtr actionDate )
{
  StBionamePtr pRecord = StBionameNew();

  if( !pRecord )
  {
	ErrPostEx(SEV_ERROR,0,0, "FillBionameRecord: "
			  "Unable to allocate StBionamePtr" );
  }

  pRecord->id = bionameId;
  pRecord->bioentityid = bioentityId;

  pRecord->name = StringSave( IsNullStr(pName) );
  pRecord->nametypeid = typeId;
  pRecord->access = StringSave( IsNullStr(pAccess) );
  pRecord->db = StringSave( IsNullStr(pDb) );
  pRecord->identifier = gi;    
  pRecord->field = field;  
  pRecord->official = official;
  pRecord->deprecated = deprecated;
  pRecord->date = StringSave( IsNullStr(dateDep) );
  pRecord->ruleid = ruleId;
  pRecord->action = action;
  pRecord->actiondate = StringSave( IsNullStr(actionDate) );

  if( !(*pList) )
  {
	*pList = pRecord;
  }
  else
  {
	pRecord ->next = *pList;
	*pList = pRecord;
  }
}
		 
/********************************************************/
/** ParseLocusInfo
 *
 *  Looks for locus info in the feature table of a bioseq. 
 *  For each feature or type gene-ref, this function gets	
 *  locus name, synonyms, from location, to location and 
 *  locus gi. 
 *  
 *  @param pbs       The bioseq from which to extract the 
 *                   locus info.
 *
 *  @param last_pbss Sometimes the annotation is not on 
 *                   the bioseq, but is on the bioseqset.
 *                   
 *  @param pList     The LOCUSLIST to which the information
 *                   should be added.
 *
 *  return 0 on success and 1 on failure.
 */
Int4	ParseLocusInfo(BioseqPtr pbs, BioseqSetPtr last_pbss,
                       LOCUSLIST *pList)
{
  SeqAnnotPtr pSeqAnnot    = NULL;
  SeqFeatPtr  pSeqFeat     = NULL;
  Int4        gi           = 0;
  Int4        success      = 0;

  gi = GetGI( pbs );  
  
  pSeqAnnot = pbs->annot;
  /* -------------------------------------------------------
     If there is no annotation, in rare cases the
     locus info is in the BioseqSet annotation.
     ------------------------------------------------------*/
  if (pSeqAnnot == NULL)
  {

    if (last_pbss == NULL || last_pbss->annot == NULL)
    {
      ErrPostEx(SEV_WARNING,0,0, "ParseLocusInfo: NULL SeqAnnot "
                "pointer for both BioSeq and BioseqSet. %ld",
                 gi);
      return 1;
    }
    else
    {
      pSeqAnnot = last_pbss->annot;

    }
  }

  /* -------------------------------------------------------
     Look for the feature table while there is still a node 
     in the BioseSet annotation.
     ------------------------------------------------------*/
  while (pSeqAnnot != NULL) 
  {
    if (pSeqAnnot->type == ANNOT_TABLE)/*the data value of this annotation node points to a feature table*/
    {
      pSeqFeat = (SeqFeatPtr) pSeqAnnot->data;
      if (pSeqFeat == NULL)
      {
	ErrPostEx(SEV_ERROR,0,0, "ParseLocusInfo: NULL SeqFeat pointer. %ld", gi);
      }

      while (pSeqFeat != NULL) 
      {
	/* ----------------------------------------------------------
	   Only parse the locus info if it's not a pseudogene.
	   ----------------------------------------------------------*/
	if ( pSeqFeat->data.choice == SEQFEAT_GENE && 
	     pSeqFeat->pseudo == FALSE )	
	{
	  ParseLocusInfoFromFeatureTable( pSeqFeat, gi, pList );
	}
				
	pSeqFeat = pSeqFeat->next;
				
      }
			
    }
		
    pSeqAnnot = pSeqAnnot->next;
  }
	
  return success;
	
}

/**************************************************************/
/** ParseLocusInfoFromFeatureTable:
 *
 *  Finds the protein gi, DNA gi, and from and to locations
 *  from a SeqFeatPtr of type GeneRef. Returns 1 if the
 *  SeqFeatTable is not a GeneRef.
 *
 *  @param  psf  The SeqFeatPtr to parse.
 *
 *  @return 0 on success, 1 on failure.
**************************************************************/
Int4 ParseLocusInfoFromFeatureTable( SeqFeatPtr psf, Int4 gi, 
                                     LOCUSLIST *pList )
{
    GeneRefPtr pGeneRef        = NULL;
    Int4       floc            = 0;   /*from location*/
    Int4       tloc            = 0;   /*to location*/
    Int4       gis             = 0;   /*gi of source DNA*/
    ValNodePtr svnp            = NULL;/*valnode pointer to synonym list*/
    CharPtr    syn_ptr         = NULL;/*pointer to synonym string*/
    /* Int4       function_report = 0; */
    /*dbxref stuff*/
    /* ValNodePtr pdbvn           = NULL; */
    CharPtr    locus_ptr      = NULL;/*name of gene locus*/
    SeqLocPtr  psl             = NULL;/**/
    Boolean    transSpliced    = FALSE;
    LOCUSINFO *pLocus           = NULL;

    Int4       success         = 0; 

    /*-----------------------------------------------------------
      Do nothing if the SeqFeatPtr is not a gene-ref.
      ---------------------------------------------------------*/
    if (psf->data.choice != SEQFEAT_GENE)	
    {
        ErrPostEx(SEV_ERROR,0,0, "ParseLocusInfoFromFeatureTable: "
                  "Not a generef for %ld.", gi);
        return 1;
    }


    /* ------------------------------------------------------------
       Is there trans-splicing going on?
       ------------------------------------------------------------*/
    if( psf->excpt && psf->excpt == TRUE && psf->except_text
        && !strcmp( psf->except_text, "trans splicing" ) )
    {
        transSpliced = TRUE;
    }

    /*get a pointer to the generef struct*/
    pGeneRef = psf->data.value.ptrvalue;
    if (pGeneRef == NULL)
    {
	ErrPostEx(SEV_ERROR,0,0, "ParseLocusInfoFromFeatureTable: NULL GeneRef pointer.");
        success = 1;
    }
    else if( pGeneRef->pseudo == FALSE )
    {
        /********************/
	/*get the locus name*/
	/********************/
	locus_ptr = pGeneRef->locus;

	if (locus_ptr == NULL)
	{
            /*sometimes fails here especially for unnamed ORFs from yeast with no known function
	      just ORF names listed under "synonym"*/
	    locus_ptr = NO_NAME;
	}					
			
        /****************************************/
	/*get the location info for the gene	*/
	/****************************************/
	 psl = psf->location;
	 if (psl == NULL)/*the location field of the seqfeat struct points to nothing*/
	 {
	     ErrPostEx(SEV_ERROR,0,0, "ParseLocusInfoFromFeatureTable: "
                       "NULL SeqLoc pointer for %ld.", gi);
	 }
                        
         success = GetSourceLocationInfo( psl, &gis, &floc, &tloc, transSpliced );
							
         /*-------------------------------------------------
	   Add a new locus info to the list.
	   ------------------------------------------------*/  
	 if (gis > 0 && floc > -1 && tloc > -1  )
	 {
      
           pLocus = CreateLocusInfo( gis, floc, tloc,
				     locus_ptr );
            
          /* ------------------------------------------------
	      Add synonyms if there are any.
	      ---------------------------------------------- */
           svnp = pGeneRef->syn;
	   while (svnp != NULL)
	   {
             
	     syn_ptr = svnp->data.ptrvalue;
	     if (syn_ptr != NULL && syn_ptr != locus_ptr)
	     {
	       AddSynonymToLocus( pLocus, syn_ptr );
	     }
	     svnp = svnp->next;
	   }

	   AddLocusInfoToList( pList, pLocus );

	 }
         else
	 {
	   ErrPostEx(SEV_ERROR,0,0, "ParseLocusInfoFromFeatureTable: " 
		     "gis = %ld, floc = %ld, tloc = %ld.", gis, floc, tloc);
         }
				
    }
    return success;					
}
#ifdef ODBC
/**************************************************************/
/** FillNameDBs
 *
 *  Searches the CDRLIST and LOCUSLIST to fill the bioname table.
 *
 *  @param pCdrList   CDRLIST containing info on th
 *                    coding regions.
 *
 *  @param pLocusList LOCUSLIST containing gene names and synonyms.
 *
 *  @return TRUE.
 */
Boolean FillNameDBs( CDRLIST *pCdrList, LOCUSLIST *pLocusList )
{
  Int4           i,j             = 0;
  Int4           bioentityCount  = 0;
  CDRINFO       *pCdr            = NULL;
  LOCUSINFO     *pLocus          = NULL; 
  CharPtr        pDbName         = "ref";
  CharPtr        pField          = "field";
  StBioentityPtr pSearchRecord   = NULL;
  StAccdbPtr     pACC            = NULL;
  CharPtr        pAccess         = NULL;

  RedBlackTreeNodePtr pNode = pLocusList->locusRoot;
  RedBlackTreeNodePtr pFirstNode = NULL, pSecondNode = NULL;

  if( pNode )
  {
	pFirstNode = RBTree_Predecessor( pNode );

	while( pFirstNode )
	{
	  pLocus = ( LOCUSINFO *)pFirstNode->data;
	  pNode = pFirstNode;

	  pFirstNode = RBTree_Predecessor( pNode );
	}
  }

  /*-------------------------------------------------------
	Do nothing if we have not got any names.
	-------------------------------------------------------*/
  if( !pNode )
  {
	return FALSE;
  }

  pSearchRecord   = StBioentityNew();
  pACC            = StAccdbNew();

  pLocus = ( LOCUSINFO * )pNode->data;

  while ( pLocus )
  {
	pCdr = FindCdrInfo( pLocus, pCdrList );
    
	/*-------------------------------------------------------
	  If we've found one, write the info to the tables.
	  Otherwise, raise a warning.
      ------------------------------------------------------*/
	if( pCdr )
    {
	  StAccdbPtr pACCTemp    = NULL;
	  Int4       bioentityId = -1;
	  Int4       retVal     = -1;

	  /*-------------------------------------------------------
		Get the Bioentity ID for this protein gi.
		-------------------------------------------------------*/
	  pSearchRecord->identifier = pCdr->gip;
     
	  retVal = SearchBioentityByGi( &pSearchRecord );

	  if( retVal )
		bioentityId = pSearchRecord->id;

	  if( bioentityId > 0 )
	  {
		/*-------------------------------------------------------
		  Get the Accession for this protein gi.
		  -------------------------------------------------------*/
		pACC->gi = pLocus->gin;
		SearchACCDB( &pACC );

        /*----------------------------------------------------------
		  Make sure that we're getting the refseq access from accdb.
		  --------------------------------------------------------*/
		pACCTemp = pACC;
		while( pACCTemp && !pAccess )
		{ 
		  if( StringCmp( pACCTemp->db, "ref") == 0 )
		  {
			pAccess = pACCTemp->access;
		  }
		  pACCTemp = pACCTemp->next;
		}

		if( !pAccess )
		  pAccess = "n/a";    

	/*-----------------------------------------------------
	  Write the locus names to Bioname.
	  -------------------------------------------------------*/
		WriteLocusNames( pLocus, bioentityId, "ref", pAccess,
						 0, 0, DEFAULTDATE );
	  }
	  else
	  {
		ErrPostEx(SEV_WARNING,0,0, "FillNameDBs: Unable to find "
				  "bioentityId for gi %ld", pCdr->gip );
	  }
	}
	else
	{
	  ErrPostEx(SEV_WARNING,0,0, "FillNameDBs: No protein gi "
				"found for gin= %ld\tfrom= %ld\tto= %ld\tname=%s\n",
				pLocus->gin, pLocus->from, pLocus->to, pLocus->name);
	}
	pNode = RBTree_Successor( pNode );
    
	pLocus = pNode ? ( LOCUSINFO * ) pNode->data : NULL;
  }

  StBioentityFree( pSearchRecord );
  StAccdbFree( pACC );

  return TRUE;
}

void WriteLocusNames( LOCUSINFO * pLocus, Int4 bioentityId, 
                      CharPtr db, CharPtr access, 
					     Int4 official, Int4 deprecated, 
						 CharPtr dateDep )
{
  int iSyn;
  StBionamePtr pRecords = NULL;
  StBionamePtr pWorking = NULL;

  /*-----------------------------------------------------
	Add the locus name, if it's meaningful.
	-------------------------------------------------------*/
  if( StringCmp( pLocus->name, NO_NAME ) != 0 )
  {
	FillBionameRecord( &pRecords, 0, bioentityId, pLocus->name, TYPE_GENE, db, 
					   access, pLocus->gin, FLD_LOCUS,
					   official, deprecated, dateDep, 
					   RULE_LOCUS, ADDED, DEFAULTDATE );
  }

  /*-----------------------------------------------------
	Write the locus synonyms to Bioname.
	-------------------------------------------------------*/    
  if( pLocus->synCount > 0 )
  {
	for( iSyn = 0; iSyn < pLocus->synCount; iSyn++ )
	{
	  FillBionameRecord( &pRecords, 0, bioentityId, pLocus->pSynonyms[ iSyn ],
						 TYPE_GENE, db, access,   pLocus->gin,
						 FLD_SYNONYM, official, deprecated, dateDep, 
						 RULE_LOCUS, ADDED, DEFAULTDATE );
	}
  }

  UpdateNames( bioentityId, TYPE_GENE, pRecords ); 
#ifdef UPDATE
  if( UPDATE )
  {
	UpdateNames( bioentityId, TYPE_GENE, pRecords ); 
  }
  else
  {
#ifdef ODBC
	AppendRecordBioname( pRecords );
#endif /* if ODBC defined */
  }
#endif /* if UPDATE defined */

  StBionameFree( pRecords );

}

/**************************************************************/
/** FillBioentityDB
 *
 *  Adds a record to the bioentity table. 
 *
 *  NOTE: for now all records in this table are proteins.
 * *  @param type       Molecule type.
 *
 *  @param dbName     Database from which this record was found.
 *
 *  @param accession  Accesssion for this database.
 *
 *  @param identifier Identifier for this database. Gi for ncbi
 *                    databases.
 * 
 *  @param field      Field that this record is based on.
 *
 *  @return           returns the bioentity id on success, -1 on
 *                    failure.
 */
Int4 FillBioentityDB( Int4 type, CharPtr dbName, CharPtr accession, 
		      Int4 identifier, Int4 field, Int4 * pUpdate )
{
  StBioentityPtr pRecord = StBioentityNew();
  StBioentityPtr pSearchRecord = StBioentityNew();
  Boolean        found = FALSE;
  Int4           bioentityId = -1;

  /*--------------------------------------------------------------
    If we are updating, then check to see if the accession exists 
	but the gi has changed. In that case, just update the gi.
	--------------------------------------------------------------*/
  *pUpdate = FALSE;

  if( UPDATE )
  {
	pSearchRecord->access = StringSave( IsNullStr(accession) );

	found = SearchBioentityByAccess( &pSearchRecord );

	if( found )
	{
	  bioentityId = pSearchRecord->id;

	  *pUpdate = TRUE;

/*--------------------------------------------------------------
    Update the gi, if it has changed.
	--------------------------------------------------------------*/
	  if( identifier != pSearchRecord->identifier )
	  {
		pSearchRecord->identifier = identifier;

		EditBioentityById( pSearchRecord );
	  }
	}
  }

  /*--------------------------------------------------------------
    If there is no bioentity for this record, then add one.
	---------------------------------------------------------------*/
  if ( bioentityId  <= 0 )
  {
	pRecord->bioentitytypeid = type;
	pRecord->db = StringSave( IsNullStr(dbName) );
	pRecord->access = StringSave( IsNullStr(accession) );    
	pRecord->identifier = identifier;    
	pRecord->field = field;    

	AppendRecordBioentity( pRecord );
	found = SearchBioentityByGi( &pRecord );

	if( found )
	{
	  bioentityId = pRecord->id;
	}
  }

  StBioentityFree( pRecord );
  StBioentityFree( pSearchRecord );

  return bioentityId;
}

/**************************************************************/
/** FillBionameDB
 *
 *  Adds a record to the bioname table.
 *
 *  @param bioentityId  The bioentity id to which this name refers.
 *
 *  @param pName        The name.
 *
 *  @param typeId       The type of name. It can be 2 for the protein
 *                      name or 1 for a locus name.
 *
 *  @param pDb          Database from which this record was found.
 *
 *  @param pAccess      Accesssion for this database.
 *
 *  @param gi           Identifier for this database. Gi for ncbi
 *                      databases.
 * 
 *  @param field        Field that this record is based on.
 *
 *  @param official     Is this name official? 0 for no, 1 for yes.
 *
 *  @param deprecated   Is this name deprecated? 0 for no, 1 for yes.
 *
 *  @param datedep      Date as a string that this name was deprecated. 
 *
 *  @param ruleId       Not currently used. Used for the rule used to
 *                      construct this name. 
 *
 *  @param action       Action performed on the 
 */
void    FillBionameDB( Int4 bioentityId, CharPtr pName, Int4 typeId,
					   CharPtr pDb, CharPtr pAccess, Int4 gi,
					   Int4 field, Int4 official, Int4 deprecated,
					   CharPtr dateDep, Int4 ruleId, Int4 action,
					   CharPtr actionDate)
{
  StBionamePtr pRecord = NULL;

  FillBionameRecord( &pRecord, 0, bioentityId, pName, typeId, pDb, pAccess, gi,
					 field, official, deprecated, dateDep, ruleId,
					 action, actionDate );

  AppendRecordBioname( pRecord ); 

  StBionameFree( pRecord );
}
#endif /* if ODBC defined */

/**************************************************************/
/** FindCDRInfo
 *
 *  Given a LOCUSINFO, find the CDRINFO that corresponds to that
 *  locus info. First find the first CDRINFO with a gin 
 *  corresponding with the LOCUSINFO gi. Then check all CDRINFOS
 *  with that gi until you find the one with the correct to and
 *  from values.
 *
 *  NOTE: If there is only 1 CDRINFO, then the to and from 
 *  values may not agree with the LOCUSINFO values, even
 *  though this is the correct locus.
 *
 *  @param pLocus    The locus info for which a corresponding
 *                   CDRINFO should be found.
 *
 *  @param pCdrList  The CDRLIST to search.
 *
 *  @return the matching CDRINFO.
 */
CDRINFO *FindCdrInfo( LOCUSINFO *pLocus, CDRLIST *pCdrList )
{
  Int4                nodeCount = 0;
  RedBlackTreeNodePtr pNode     = NULL;
  RedBlackTreeNodePtr pFirstNode = NULL;

  Int4      locusGi   = 0;
  Int4      locusFrom = 0;
  Int4      locusTo   = 0;
  /* Int4      cdrGi     = 0; */
  CDRINFO  *pCdr      = NULL;
  CDRINFO  *pFirstCdr = NULL;
  CDRINFO  *pPreCdr = NULL;
  Boolean   found     = FALSE;

  locusGi   = pLocus->gin;
  locusFrom = pLocus->from;
  locusTo   = pLocus->to;
  /* ------------------------------------------------------
     Find a CDRINFO with the locus gi.
     ------------------------------------------------------*/
  pNode = RBTree_FindInt( pCdrList->cdrRoot, locusGi, 
                         pCdrList->funPack ); 
  
  pFirstNode = RBTree_Predecessor( pNode );
  
  if( pFirstNode )
  {
    pPreCdr =  ( CDRINFO * ) pFirstNode->data;
   
    while( pPreCdr && locusGi == pPreCdr->gin )
    {
      pNode = pFirstNode;
      pFirstNode = RBTree_Predecessor( pNode );
      if( pFirstNode )
	pPreCdr = ( CDRINFO * ) pFirstNode->data;
      else 
        pPreCdr = NULL;
    }
  }

  if( pNode )
  {
     pCdr = pFirstCdr = ( CDRINFO * ) pNode->data;

    /*---------------------------------------------------
     Find all nodes with that gi and compare their from
     and to values.
     ----------------------------------------------------*/
     while( pNode && locusGi == pCdr->gin )
     {
	nodeCount++;
        
	if( locusFrom == pCdr->from && locusTo == pCdr->to )
	{
          found = TRUE;
	  break;
        }
        pNode = RBTree_Successor( pNode );
         
        if( pNode )                         
	  pCdr = ( CDRINFO * ) pNode->data;
     }
     
     if( found == FALSE )
     {
       /*---------------------------------------------------
       If there is only locus that matches that gi, then use
       that even if the from/to values do not match. If there
       is only 1 locus, then often the from/to values are given
       (incorrectly) as the entire locus, even though the 
       protein is encoded by a subset of the locus.
       ----------------------------------------------------*/
       if( nodeCount == 1 )
       {
         pCdr = pFirstCdr;
       }
       else
       {
	 pCdr = NULL;
       }
     }
  }
  return pCdr;
}


Boolean ToBioseq(SeqEntryPtr pse, Int4 callno, ValNodePtr pvndesset, 
				 CharPtr filename, CharPtr release, 
				 NAMEINFO *pNameInfo )
{

   BioseqPtr pbs = NULL;
   BioseqSetPtr pbss = NULL;
   SeqEntryPtr pseNew = NULL;
   Int4 igi = 0;
   ValNodePtr pvngi = NULL, pvngihead = NULL, pvnfirst = NULL;
   Boolean bSet = 0;
   static Int4 ncbiChromId = 0;
   Int4 kloodge = 0;
   CharPtr xmeNumber = NULL;

   ValNodePtr pvnComgenGis = NULL;
   CharPtr division = NULL;
   Int4 taxid = 0;

   /* SK: NULL ptr check */
   if ( pse == NULL )
   {
         ErrPostEx(SEV_ERROR,0,0, "ToBioseq: NULL pse.");
         return FALSE;
   }
   /*If this is the first call to the ToBioseq then set the Seid
     to -1 so that we can set it to the first gi that we extract
     from the Seqentry. */
   if(callno == 1)
   {
     seid = -1;
	 ncbiChromId = 0;
   }

   if (pse->choice == SEQENTRY_BIOSEQ) /* It is a Bioseq */
    {
	  Int4    taxId      = 0;
 
      pbs = (BioseqPtr) pse->data.ptrvalue;
      if (pbs == NULL)
       {
         ErrPostEx(SEV_ERROR,0,0, "ToBioseq: Bioseq pointer is NULL.");
         return FALSE;
       }

      igi = GetGI(pbs);

      /*if the seid has not been set yet then set it*/
      if(seid == -1)
	  {
        seid = igi;
	  }

	  /*-----------------------------------------------------------
		See if there is a chromId for this bioseq.
		---------------------------------------------------------*/
		ncbiChromId = GetChromIdFromBioseq( pbs );
/*-----------------------------------------------------------
		If there's a seqdesc on the bioseq, add it to pvndesset.
		---------------------------------------------------------*/
		if( pbs->descr )
		{
		  ValNodePtr pAllDescr = NULL;

		  if( pvndesset && pvndesset->data.ptrvalue )
		  {
			pAllDescr = 

			  Slri_ValNodeAddPointer(&pAllDescr, 1, 
									 pvndesset->data.ptrvalue );
		  }

		  pAllDescr = 
			Slri_ValNodeAddPointer(&pAllDescr, 1, 
								   pbs->descr );

		  xmeNumber = GetGenomeInfo( pbs, pAllDescr, 
									 ncbiChromId, &division );

		  taxid = GetTaxId( pAllDescr );
		}
		else
		{
		  xmeNumber = GetGenomeInfo( pbs, pvndesset, ncbiChromId,
					   &division );
		  
		  taxid = GetTaxId( pvndesset );
		}

	/*-------------------------------------------------------------
	  Get our kloodge so that we can include it in taxgi.
	  ------------------------------------------------------------*/
	  if( ncbiChromId >  0 )
	  {
		kloodge = SearchCHROMByChromId( ncbiChromId );
	  }

	  /*-------------------------------------------------------
		Can we get it from the xmeNumber?
		------------------------------------------------------*/
	  if( kloodge == 0 && xmeNumber != NULL )
	  {
		kloodge = 
		  SearchCHROMByChromNum( xmeNumber, &ncbiChromId, taxid );
	  }

	  /*----------------------------------------------------------
		If we've got a kloodge, we don't need xmeNumber.
		---------------------------------------------------------*/
	  if( kloodge > 0 )
	  {
		MemFree( xmeNumber );
		xmeNumber = NULL;
	  }
	  
	  /*----------------------------------------------------------
		Add the igi to the valnode pointers if we have either kloodge
		or xmeNumber to make sure that it's there.
		---------------------------------------------------------*/
	  if( kloodge != 0 || xmeNumber != NULL  )
	  {
		ValNodeAddInt( &pvnComgenGis, 0, igi );
	  }

      if (!FillASNDB(pbs, pvndesset, division, release))
       {
          ErrPostEx(SEV_ERROR,0,0, "ToBioseq: FillASNDB failed.");
          return FALSE;
       }

      if (!FillACCDB(pbs, pvndesset))
       {
         ErrPostEx(SEV_ERROR,0,0, "ToBioseq: FillACCDB failed");
         return FALSE;
       }

      if (!FillPUBSEQ(pbs, pvndesset))
       {
          ErrPostEx(SEV_ERROR,0,0, "ToBioseq: FillPUBSEQ failed.");
          return FALSE;
       }

      if (!FillTAXGI( pbs, pvndesset, kloodge ))
       {
          ErrPostEx(SEV_ERROR,0,0, "ToBioseq:FillTAXGI failed.");
          return FALSE;
       }

	  if( !UPDATE)
	  {
		SEQIDINFO *pInfo =  GetSeqIdInfoFromBioseq( pbs );

		if( pInfo && pInfo->access )
		{
		  if( !LogHistory(igi, 0, pInfo->access, pInfo->version, 
						ACTION_ADDED, filename) )
		  {
			ErrPostEx(SEV_ERROR,0,0, "ToBioseq:LogHistory failed "
					  "for %ld.", igi);
			return FALSE;
		  }
		}	
		else if( pInfo && pInfo->pvnNames )
		{
		  DBNAME * pDbName = (pInfo->pvnNames)->data.ptrvalue;
		  /*--------------------------------------------------
			Only log the first name if no accession is present.
			---------------------------------------------------*/
			
		  if( !LogHistory(igi, 0, pDbName->pName, pInfo->version, 
						  ACTION_ADDED, filename) )
		  {
			ErrPostEx(SEV_ERROR,0,0, "ToBioseq:LogHistory failed "
					  "for %ld.", igi);
			return FALSE;
		  }
		}
		DestroySEQIDINFO( &pInfo );
	  }

#ifdef ODBC      
    
      pNameInfo->pDescr = pvndesset;
 
	  if( REFSEQ )
	  {
		ValNodePtr pvnContigGis = NULL;

		/*------------------------------------------------------
		  Get the names and any gis for comgen.
		  -----------------------------------------------------*/
		ParseNamesFromBioseq( pbs, pNameInfo, &pvnComgenGis );  

		FillGiChromid( pvnComgenGis, igi, ncbiChromId );
		FillGiChromosome( pvnComgenGis, xmeNumber );

		/*-----------------------------------------------------
		  Parse the contigs and try to fill gikloodge.
		  -----------------------------------------------------*/
		if( pbs->seq_ext_type == DELTA_SEQ && 
			(xmeNumber || ncbiChromId) )
		{

		  pvnContigGis = 
			ParseContigs( pbs->seq_ext );

		  /*------------------------------------------------------------------
			If we found any contig gis, add the bioseq gi to 
			the gi list.
			-----------------------------------------------------------------*/
		  if( pvnContigGis )
		  {
			ValNodeAddInt( &pvnContigGis, 0, igi );
		  }

		  /*------------------------------------------------------
			Write the contig gis to gikloodge or gichromosome. 
			------------------------------------------------------*/
		  FillGiChromid( pvnContigGis, igi, ncbiChromId );
		  FillGiChromosome( pvnContigGis, xmeNumber );

		  /*------------------------------------------------------
			Write the contig gis to contigkloodge or contigchromosome
			so that we can parse out the mRnas and proteins that
			belong to this chromosome later.
			-----------------------------------------------------*/
		  FillContigChromid( pvnContigGis, igi, ncbiChromId, 1 );
		  FillContigChromosome( pvnContigGis, xmeNumber );

		  ValNodeFree( pvnContigGis );
		}

		if( xmeNumber )
		{
		  Free( xmeNumber );
		  xmeNumber = NULL;
		}

		if( pNameInfo->getCdrInfo == TRUE )
		  pNameInfo->getCdrInfo = FALSE;

		if( pvnComgenGis )
		  ValNodeFree( pvnComgenGis );
	  }

      
#endif /* if ODBC defined */
    }
   else if (pse->choice == SEQENTRY_BIOSEQSET) /* it is nested Bioseq-set extract Bioseqs recursively */
    {
         pbss = (BioseqSetPtr) pse->data.ptrvalue;
         if (pbss == NULL)
          {
              ErrPostEx(SEV_ERROR,0,0, "ToBioseq: Bioseq-Set pointer is NULL.");
              return FALSE;
          }
         /* KM 260304 store and pass around all descriptors to BioseqSets to capture annotation missing in Bioseqs */
         if ((pbss->descr) != NULL)
         {
             pvndesset = Slri_ValNodeAddPointer(&pvndesset,
												callno, pbss->descr);
             bSet = 1;
         }  
         
         if(WhichClass(pbss) == BioseqseqSet_class_nuc_prot)
          {
            if (!FillNUCPROT(pbss))
             {
              ErrPostEx(SEV_ERROR,0,0, "ToBioseq: FillNUCPROT failed.");
              return FALSE;
             }
#ifdef ODBC
	      

#endif /* if ODBC defined */
          }
		 if( REFSEQ )
		 {
		   ncbiChromId = 
			 GetChromIdFromBioseqSet( pbss, &xmeNumber );

		   pNameInfo->getCdrInfo = 
			 ( Boolean )ParseCDRInfo( pbss, NULL, 
									  pNameInfo->pCdrList,
									  &pvnComgenGis );

		   if( ncbiChromId )
		   {
			 FillGiChromid( pvnComgenGis, 0, ncbiChromId );
		   }
		   else if( xmeNumber ) 
		   {
			 FillGiChromosome( pvnComgenGis, xmeNumber );
		   }

		   if( xmeNumber )
		   {
			 Free( xmeNumber );
			 xmeNumber = NULL;
		   }
		 }
		 ValNodeFree( pvnComgenGis );

         pseNew = pbss->seq_set;
         while (pseNew != NULL)  /* get all Bioseqs recursively */
            {
             if(!ToBioseq(pseNew,++callno, pvndesset, filename, release, pNameInfo))
               {
                  ErrPostEx(SEV_ERROR,0,0, "ToBioseq: The recursive call to ToBiseq failed");
                  return FALSE;
               }
              pseNew = pseNew->next;
            }
        /* KM 290304 before ending the recursion for this level */
        /* remove first node of Bioseq-set descriptors ValNode and free the node (if added previously) */
        if (bSet && (pvndesset != NULL))
        {
           pvnfirst = pvndesset;
           pvndesset = pvndesset->next;
           pvnfirst->next = NULL;
           pvnfirst = ValNodeFree(pvnfirst);
           bSet = 0;
        }
    }
   else
    {
         ErrPostEx(SEV_ERROR,0,0, "ToBioseq: Invalid choice field.");
         return FALSE;
    }
   return TRUE;
}


/* adopted from update.c, zwang 20041123, 20050202*/
/* locates existing GIs in Intez databases and blanks them, assigns -1 to GI field, and deletes them */
Boolean IntrezDeleteGI(Int4 gi, Boolean destroy) 
{
   Boolean delparti = FALSE;
   Boolean delaccdb = FALSE;
   Boolean delsengi = FALSE;
   Boolean deltaxgi = FALSE;
#ifdef ODBC
   Boolean delsendb = FALSE;
   Boolean delasndb = FALSE;
   Boolean delcomgen = TRUE;

   CharPtr access = NULL;
   Boolean foundContigs = FALSE;

   Boolean success = FALSE;

#endif

  /*-------------------------------------------------------------------
	 NOTE:To delete from the Chrom table, we need the accession. We 
	      MUST get this before we delete from accdb.
	 -------------------------------------------------------------------*/
   if( REFSEQ )
   {
	 access = SHoundFindAccessByGi( gi );
   
	 if( access )
	 {
	   if( !EditTAXGIKloodgeByGi( gi, 0 ) )
	   {
		 ErrPostEx(SEV_INFO,0,0, 
				   "Unable to update taxgi for %s, 0.", 
				   access );
		 /*delcomgen = FALSE;*/
	   }

	 /*---------------------------------------------------------
	   Is this a top complete genome record?
	 --------------------------------------------------------------*/
	   Int4 kloodge = SHoundGetKloodgeFromAccess( access );

	   if( kloodge > 0 )
	   {

	   /*-------------------------------------------------------------
		 Get all contigs that belong to this contig.
		 --------------------------------------------------------*/
		 foundContigs = ProcessTopComgenGi( gi, &foundContigs );
	   
		 if( !DeleteAccessInCHROM( access ) )
		 {
		   ErrPostEx(SEV_INFO,0,0, 
					 "Access %s not deleted in CHROM.", access);
		   /*delcomgen = FALSE;*/
		 } 
		 
	   }
	 }

   /*---------------------------------------------------------
   If the record is not in Chrom, the gi could still be in gikloodge
   or contigkloodge (a lower level contig), so delete those based 
   on gi.
   --------------------------------------------------------------*/
   
	 if( !foundContigs )
	 {
	   /*if( !ProcessContigGi( gi, &foundContigs ) ) 
		 delcomgen = FALSE;*/
	   ProcessContigGi( gi, &foundContigs );
	 }

	 /*if( ! ProcessLowLevelGi( gi ) )
	   delcomgen = FALSE;*/
	 ProcessLowLevelGi( gi );
   }
   if(!DeleteGIinPARTI(gi, destroy))
      ErrPostEx(SEV_INFO,0,0, "GI %ld not deleted in PARTI.", gi);
   else {
      delparti = TRUE;
   }
   if (!DeleteGIinACCDB(gi, destroy))
      ErrPostEx(SEV_INFO,0,0, " GI %ld not deleted in ACCDB.", gi);
   else {
      delaccdb = TRUE;
   }

#ifdef ODBC
   if(!DeleteGIinSENDB("nodiv", gi, destroy))
      ErrPostEx(SEV_INFO,0,0, "   GI %ld not deleted in SENDB.", gi);
   else {
      delsendb = TRUE;
   }
#endif

   if(!DeleteGIinSENGI(gi, destroy))
      ErrPostEx(SEV_INFO,0,0, "   GI %ld not deleted in SENGI.", gi);
   else {
      delsengi = TRUE;
   }

   if(!DeleteGIinTAXGI(gi, destroy))
      ErrPostEx(SEV_INFO,0,0, "    GI %ld not deleted in TAXGI.", gi);
   else{
       deltaxgi = TRUE;
   }

    if(!DeleteGIinPUBSEQ(gi,  destroy))
       ErrPostEx(SEV_INFO,0,0, "    GI %ld not deleted in PUBSEQ.", gi);

   if(!DeleteGIinNUCPROT(gi, destroy))
       ErrPostEx(SEV_INFO,0,0, "    GI %ld not deleted in NUCPROT.", gi);

  if(!(delparti && delaccdb && delsengi && deltaxgi))
       ErrLogPrintf("ERROR, GI %ld not deleted properly.\n", gi);
#ifdef ODBC
   /*because the gi is defined as a primary key in the asndb table
     and sendb table need to delete asndb and sendb record in db2 in order
     to be able to update*/
   if(!DeleteGIinASNDB("nodiv",gi, destroy))
           ErrPostEx(SEV_INFO,0,0, "    GI %ld not deleted in ASNDB.", gi);
   else {
       delasndb = TRUE;
   }

#endif
   success = ( delparti  && delaccdb && delsengi && deltaxgi && 
			delsendb  && delasndb && delcomgen );

   return ( delparti  && delaccdb && delsengi && deltaxgi && 
			delsendb  && delasndb && delcomgen );
}


/* adopted from update.c, zwang 20050202*/
/** ProcessTopComgenGi
 *  
 *  Takes the gi that corresponds to a top-level comgen record, 
 *  gets all of the contig gis from this record and deletes all
 *  gis from GICHROMID.
 *
 * @param topGi gi of the top level record.
 *
 * @return Boolean if contigs are found, FALSE if none are found.
 *
 */
Boolean ProcessTopComgenGi( Int4 topGi, Boolean *foundContigs)
{
  StContigchromidPtr pContigs = NULL, pRecord = NULL;

  Boolean success = FALSE;

  pRecord = pContigs = SearchCONTIGCHROMIDByTopGi( topGi );

  while( pRecord )
  {
	*foundContigs = TRUE;

	success = ProcessContigGi( pRecord->contiggi, foundContigs );

	pRecord = pRecord->next;
  }

  DeleteTopGiInCONTIGCHROMID( topGi );  
  return success;
}


/* adopted from update.c, zwang 20050202*/
Boolean ProcessContigGi( Int4 contigGi, Boolean * foundGis )
{
  StGichromidPtr pGis = NULL, pRecord = NULL;;
  Boolean success = FALSE;

  pRecord = pGis = SearchGICHROMIDByContigGi( contigGi );

  while( pRecord )
  {
	*foundGis = TRUE;
	
	success = ProcessLowLevelGi( pRecord->gi );

	pRecord = pRecord->next;
  }

  DeleteContigGiInGICHROMID( contigGi );
  return success;
}

/* adopted from update.c, zwang 20050202*/
Boolean ProcessLowLevelGi( Int4 gi )
{
  Boolean success = TRUE;
  StGichromidPtr pGi = NULL;
  Int4 contigGi = 0;
  

  success = EditTAXGIKloodgeByGi( gi, 0 );	

  if( !success )
  {
	ErrPostEx(SEV_INFO,0,0, 
			  "ProcessLowLevelGi:Unable to update taxgi for %d.", 
			  gi);
  }
  
  /*------------------------------------------------------------
	Get the contiggi and mark the contig as updated.
	-----------------------------------------------------------*/
  pGi = SearchGICHROMIDByGi( gi );
  
  if( pGi )
  {
	contigGi = pGi->contiggi;

	success = EditRecordCONTIGCHROMIDUpdate( contigGi, 1 );

	if( !success )
	{
	  ErrPostEx(SEV_INFO,0,0, 
				"ProcessLowLevelGi:Unable to update contiggi %d "
				"for gi %d.", contigGi, gi );
	  
	}
  }
  
  success = DeleteGiInGICHROMID( gi );

  if( !success )
  {
	ErrPostEx(SEV_INFO,0,0, 
			  "ProcessLowLevelGi:Unable to delete gi %d.", gi );
			  
  }

  return success;
}

DBNAME *CreateDBNAME( CharPtr name, CharPtr db )
{
  DBNAME * pName = Malloc( sizeof( DBNAME ) );
  
  if( pName == NULL )
  {
	 ErrPostEx(SEV_ERROR,0,0,"CreateDBNAME: Cannot allocate "
              "memory for new DBNAME.");
  }
  else
  {
	pName->pName = StringSave( name );
	pName->pDb   = StringSave( db );
  }

  return pName;
}
void    DestroyDBNAME( DBNAME **pName )
{
  DBNAME *pDbName = *pName;

  MemFree( pDbName->pName );

  
  MemFree( pDbName->pDb );

  MemFree( pDbName );
}

SEQIDINFO *CreateSEQIDINFO()
{
  SEQIDINFO *pInfo =  Malloc( sizeof( SEQIDINFO ) );

  if( pInfo == NULL )
  {
    ErrPostEx(SEV_ERROR,0,0,"CreateSEQIDINFO: Cannot allocate "
              "memory for new SEQIDINFO.");
  }
  else
  {
    pInfo->gi          = 0;
    pInfo->version     = 0;
    pInfo->access      = NULL;
    pInfo->pvnNames    = NULL;
  }
  return pInfo;
}


void DestroySEQIDINFO( SEQIDINFO **pInfo)
{
  SEQIDINFO * pSeqIdInfo = *pInfo;

  MemFree( pSeqIdInfo->access );

  if( pSeqIdInfo->pvnNames )
  {
	ValNodePtr pCurrent = pSeqIdInfo->pvnNames;
	ValNodePtr pTmp     = NULL;

	while( pCurrent )
	{
	  DBNAME * pName = (DBNAME *)pCurrent->data.ptrvalue;
	  DestroyDBNAME( &pName );

	  pTmp = pCurrent->next;
	  pCurrent = pTmp;
	}

	/*SHValNodeFree( &(pSeqIdInfo->pvnNames) );*/
	ValNodeFree( pSeqIdInfo->pvnNames );
  }

  MemFree( pSeqIdInfo );
}

/***************************************************************/ 
/** GetSeqIdInfoFromBioseq
 *
 * Function extracts information needed during the update. It 
 * assumes that there will be only 1 gi, version and accession for
 * a given bioseq. It also returns a list of other gis found 
 * associated with ids (names) from other databases.
 *
 * @param pbs The bioseq from which to extract the seqid info.
 *
 * @return A structure containing all the information.
 *
 */
SEQIDINFO *GetSeqIdInfoFromBioseq( BioseqPtr pbs )
{

  SEQIDINFO   *pInfo    = CreateSEQIDINFO();
  SeqIdPtr     pSeqId   = NULL;
  StAccdbPtr   pRecord  = NULL;
  DBNAME      *pName    = NULL;

	/* --------------------------------------------------------
	   Initial checks.
	   ----------------------------------------------------------*/	  
  if( !pInfo )
	return NULL;
  
  if( pbs == NULL )
  {
	ErrPostEx(SEV_ERROR,0,0, "GetSeqIdInfoFromBioseq: "
			  "Bioseq pointer is NULL.");
	return NULL;
  }

  if( pbs->id == NULL )
  {
	ErrPostEx(SEV_ERROR,0,0, "GetSeqIdInfoFromBioseq: "
			  "SeqId pointer is NULL.");
	return NULL;
  }

  pSeqId = pbs->id;

  while( pSeqId != NULL )
  {

	pRecord = GetInfoFromSeqId( pSeqId, REFSEQ );

	if( pRecord )
	{
	/* --------------------------------------------------------
	   Fill the SEQIDINFO structure 
	----------------------------------------------------------*/
	  if( pRecord->gi )
	  {
		pInfo->gi = pRecord->gi;
	  }
	  if( pRecord->access )
	  {
		pInfo->access = StringSave( pRecord->access );
	  }

	  if( pRecord->version )
	  {
		pInfo->version = pRecord->version;
	  }

	  if( pRecord->name )
	  { 
		if( pRecord->db )
		{
		  pName = CreateDBNAME( pRecord->name, pRecord->db );

		  if( pName )
		  {
			pInfo->pvnNames = 
			  ValNodeAddPointer( &pInfo->pvnNames, 0, pName );
		  }
		}
		else
		{
		  ErrPostEx(SEV_ERROR,0,0, "GetSeqIdInfoFromBioseq: "
				 "no db name for %s\n", pRecord->name);
		}
	  }

	  StAccdbFree( pRecord );
	}
	pSeqId = pSeqId->next;
  }
		
  return pInfo;
}

Boolean CompareGis( Int4 newGi, CharPtr pAccess, Int4 newVersion, StAccdbPtr pRecords,
					CharPtr pFilename )
{
  Boolean result  = TRUE;
  Int4 oldGi      = 0; 
  Int4 firstGi    = 0;
  Int4 oldVersion = 0;
  Int4 giCount    = 0;
  
  /* This means that there is no record for this accession. The gi
	 should be ADDED. */
  if( pRecords == NULL )
  {
	if(!LogHistory(newGi, oldGi, pAccess, newVersion, 
				   ACTION_ADDED, pFilename))
	{
	  ErrPostEx(SEV_ERROR,0,0, "CompareGis: LogHistory failed");
	  return FALSE;
	}
	gpvnadd = Slri_ValNodeAddInt(&gpvnadd, 0, newGi);
  } 
  else
  {
	firstGi = pRecords->gi;
  }

  while(pRecords)
  {
	oldGi      = pRecords->gi;
	oldVersion = pRecords->version;
 		
	/*--------------------------------------------------------
	  Found an extra gi. Log an error and add to the extra list.
	  _------------------------------------------------------*/
	if( firstGi != oldGi )
	{
	  giCount++;

	  if(!LogHistory(newGi, oldGi, pAccess, newVersion, 
					 ACTION_EXTRA, pFilename))
	  {
		ErrPostEx(SEV_ERROR,0,0, "CompareGis: LogHistory failed");
		return FALSE;
	  }
	  ErrPostEx(SEV_ERROR,0,0, "CompareGis: found an extra gi: "
				"%ld %ld for gi %ld", firstGi, oldGi, newGi );
	
	  gpvnextra= Slri_ValNodeAddInt(&gpvnextra, 0, newGi);
	}
/* accession was resubmitted together with old GI - 
   CHANGE action for the old/new GI */
	else if (newGi == oldGi) 
	{
	  if(!LogHistory(oldGi, oldGi, pAccess, oldVersion, 
					 ACTION_CHANGED, pFilename))
	  {
		ErrPostEx(SEV_ERROR,0,0, "CompareGis: LogHistory failed");
		return FALSE;
	  }
	  gpvnchange = Slri_ValNodeAddInt(&gpvnchange, 0, newGi);
	}
	/* accession was resubmitted with a different GI - 
	   DELETE action for the old GI and ADD action for the new GI */
	else if (newGi != oldGi) 
	{
	  if(!LogHistory(oldGi, 0, pAccess, oldVersion, 
					 ACTION_KILLED, pFilename))
	  {
		ErrPostEx(SEV_ERROR,0,0, "CompareGis: LogHistory failed");
		return FALSE;
	  }
	  gpvndelete = Slri_ValNodeAddInt(&gpvndelete, 0, oldGi);
	  if(!LogHistory(newGi, oldGi, pAccess, newVersion,
					 ACTION_ADDED, pFilename))
	  {
		ErrPostEx(SEV_ERROR,0,0, "CompareGis: LogHistory failed");
		return FALSE;
	  }
	  gpvnadd = Slri_ValNodeAddInt(&gpvnadd, 0, newGi);
	}
	pRecords = pRecords->next;
  }
  return TRUE;
}


/* adopted from update.c, zwang 20041123, 20050202*/
/*
This function takes a Seq-entry pointer and extracts all Bioseqs.
*/
static Boolean ToBioseqUp(SeqEntryPtr pse, Int4 callno, 
						  CharPtr pFilename)
{

   BioseqPtr pbs = NULL;
   BioseqSetPtr pbss = NULL;
   SeqEntryPtr pseNew = NULL;
   Int4 newgi = 0, oldgi = 0;
   CharPtr paccess = NULL;
   StAccdbPtr pac = NULL;
   Int2 newversion = 0, oldversion = 0;
   SEQIDINFO *pInfo = NULL;

   Int4 nRecordCount = 0;

   /* SK: NULL ptr check */
   if ( pse == NULL )
   {
      ErrPostEx(SEV_ERROR,0,0, "ToBioseqUp: NULL pse.");
      return FALSE;
   }

   if (pse->choice == 1) /* It is a Bioseq */
   {

	 pbs = (BioseqPtr) pse->data.ptrvalue;
	 if (pbs == NULL)
     {
	   ErrPostEx(SEV_ERROR,0,0, "ToBioseqUp: Bioseq pointer is NULL.");
	   return FALSE;
	 }

	 /*-------------------------------------------------------------------------
	   Loop through the seqids to find the accession and any other names that
	   might be relevant. 
	   ------------------------------------------------------------------------ */
	 pInfo = GetSeqIdInfoFromBioseq( pbs );

	 if( !pInfo || pInfo->gi <= 0 )
	 {
	   ErrPostEx(SEV_ERROR,0,0, "ToBioseqUp: "
				 "No SeqId information or incomplete information.");
	   return FALSE;
	 }
	 newgi      = pInfo->gi;
	 paccess    = pInfo->access;
	 newversion = pInfo->version;

	 if( paccess )
	 {
	   pac = GetACCDBRecord(paccess);
		/* SK: Concatenated three if's in one */

	   if( !CompareGis( newgi, paccess, newversion, pac, pFilename ) )
	   {
		 ErrPostEx(SEV_ERROR,0,0, "ToBioseqUp: Error comparing "
				   "gis for %ld access %s.", newgi,
				   paccess);
		 return FALSE;
	   }
	   
	   StAccdbFree( pac );
	   paccess = MemFree(paccess);
	 }
	 /*-------------------------------------------------------------
	   No access. See if we can use a name.
	   ------------------------------------------------------------*/
	 else
	 {
	   CharPtr name = NULL;
	   CharPtr db   = NULL;
	   Int4    nameCount = 0;
	   
	   ValNodePtr pCurrent = pInfo->pvnNames;
	   while( pCurrent )
	   {
		 DBNAME * pDbName = pCurrent->data.ptrvalue;
		 
		 name = pDbName->pName;
		 db   = pDbName->pDb;

		 StAccdbPtr pNameGis = SearchACCDBByName( db, name );

		 if( pNameGis && nameCount == 0 )
		 {
		   if( !CompareGis( newgi, name, newversion, pNameGis, pFilename ) )
		   {
			 ErrPostEx(SEV_ERROR,0,0, "ToBioseqUp: "
					   "Error comparing gis for %ld name %s.", 
					   newgi, name);
					   
			 return FALSE;
		   
		   }
		 }
		 /* Loop through the other names looking for gis we might 
			have missed. Note that these will not be logged to the
		    *run file, although they will be written to the *gis file*/
		 else if( pNameGis )
		 {
		   while( pNameGis )
		   {
			 Misc_AddInt2UniqueList( &gpvnextra, 0, 
									 pNameGis->gi ); 
									 
			 pNameGis = pNameGis->next;
		   }
		 }
		 StAccdbFree( pNameGis );
		 pCurrent = pCurrent->next;		 
	   }
	   /*-------------------------------------------------------
		 If we have neither name nor accession, log an error, but
		 see if the gi is in the database.
		 --------------------------------------------------------*/
	   if( !pCurrent )
	   {
		 int found = 0;

		 ErrPostEx(SEV_ERROR,0,0, "ToBioseqUp: Bioseq gi "
				   "%ld has neither access or name.", newgi);

		 pac = StAccdbNew();

		 pac->gi = newgi;

		 found = SearchACCDBByGi( &pac );
		 if( found )
		 {
		   if(!LogHistory(newgi, newgi, "n/a", oldversion, 
						  ACTION_EXTRA, pFilename))
		   {
			 ErrPostEx(SEV_ERROR,0,0, "CompareGis: "
					   "LogHistory failed");
			 return FALSE;
		   }
		   gpvnextra = Slri_ValNodeAddInt(&gpvnextra, 0, 
										   newgi);
		 }
		 StAccdbFree( pac );
	   }
	 }
   }
   else if (pse->choice == 2) /* it is nested Bioseq-set extract Bioseqs recursively */
    {

         pbss = (BioseqSetPtr) pse->data.ptrvalue;
         if (pbss == NULL)
           {
             ErrPostEx(SEV_ERROR,0,0, "ToBioseqUp: Bioseq-Set pointer is NULL.");
             return FALSE;
           }
         pseNew = pbss->seq_set;
         while (pseNew != NULL)  /* get all Bioseqs recursively */
            {
             if(!ToBioseqUp(pseNew,++callno, pFilename))
               {
                ErrPostEx(SEV_ERROR,0,0, "ToBioseqUp: The recursive call to ToBiseq failed");
                return FALSE;
               }
              pseNew = pseNew->next;
            }
    }
   else
    {
         ErrPostEx(SEV_ERROR,0,0, "ToBioseqUp: Invalid choice field.");
         return FALSE;
    }

   if( pInfo )
   {
	 DestroySEQIDINFO( &pInfo );
   }

   return TRUE;
}



/* adopted from update.c, zwang 20041123 */
Boolean LogHistory(Int4 gi, Int4 oldgi, CharPtr paccess, 
				   Int2 version, Int2 action, CharPtr pFilename)
{
   StHistdbNodePtr phis = NULL;

   phis = StHistdbNodeNew();
   phis->gi = gi;
   phis->oldgi = oldgi;
   phis->access = StringSave(paccess);
   phis->version = version;
   phis->date = StringSave(gpdate);
   phis->action = action;
   phis->filename = StringSave( pFilename );
   if(!AppendRecordHISTDB(phis))
   {
      ErrPostEx(SEV_ERROR,0,0, "LogHistory: AppendRecordHISTDB failed.");
      StHistdbNodeFree(phis);
      return FALSE;
   }
   StHistdbNodeFree(phis);
   return TRUE;
}

/* adopted from update.c, zwang 20041123 */
StAccdbPtr GetACCDBRecord(CharPtr paccess)
{
  StAccdbPtr pac = NULL;
  Int2 res = 0;

  pac = StAccdbNew();
  pac->access = StringSave(paccess);
  res = SearchACCDBByAccess(&pac);
  if (res == -1)
  {
	ErrPostEx(SEV_ERROR,0,0, "SHoundFindAcc: "
	  "Search ACCDB failed for %s.", paccess);
      StAccdbFree(pac);
      return NULL;
  }
  if (res == 0)
  {
      StAccdbFree(pac);
      return NULL;
  }
  else
    return pac;
}


Int2 Main()
{

   AsnIoPtr aipin = NULL;
   AsnTypePtr atp = NULL;
   AsnTypePtr atp2 = NULL;
   AsnModulePtr amp = NULL;
   SeqEntryPtr sep = NULL;

   /*---------------------------------------------------------------------
	 Log names and file names.
	 -------------------------------------------------------------------*/\
   Char logname[PATH_MAX];   
   Char gisname[PATH_MAX];
   Char shortName[PATH_MAX];
   CharPtr pfilenam = NULL;

   NCBI_DatePtr pncbidate = NULL;
   FILE* pfile = NULL;
   Int2 i = 0;
   Int4 ibi = 0;
   time_t tim1,tim2, tim3;

   CDRLIST   *pCdrList = NULL;
   LOCUSLIST *pLocusList = NULL;
   NAMEINFO  nameInfo;

   tim1=GetSecs();
   /* check command line arguments */
   if (!GetArgs("mother",NUMARGS, myargs))
          return 1;
  


   /* load the sequence alphabets  */
   /* (and sequence parse trees)   */
   if (!SeqEntryLoad())
         ErrShow();

   /*--------------------------------------------------------
	 Note: This points to the position in the input file where
	 the filename starts. Therefore, do not destroy!
	 ----------------------------------------------------------*/
   pfilenam = FileNameFind(myargs[0].strvalue);
   for(i = 0; i<= strlen(pfilenam); i++)
     {
       if (pfilenam[i] == '.') break; 

	shortName[i] = pfilenam[i];
     }
   shortName[i] = '\0';
   sprintf(logname,"%srun", shortName);

   /*---------------------------------------------------------------
	 Are we updating?
	 --------------------------------------------------------------*/
   if (myargs[4].intvalue)
   {
	 UPDATE = TRUE;
   }

   ErrSetOptFlags(EO_LOG_SEVERITY | EO_LOG_CODES | EO_LOG_FILELINE | EO_LOG_USRFILE);
   ErrSetLogfile(logname, ELOG_BANNER);
   ErrSetLogLevel(SEV_INFO);
   ErrSetMessageLevel(SEV_MAX);

   pas = StAsndbNew();
   ppa = StPartiNew();
   pac = StAccdbNew();
   pnc = StNucprotNew();
   pps = StPubseqNew();
   ptg = StTaxgiNew();

   psd = StSendbNew();
   psg = StSengiNew();

   InitCodeBase();
   SetCodeBaseFlagsExclNoidx();
   SetCodeBaseAutoIdxTrue();
   /* decide if this is a refseq record (according to the file of origin and flip a flag so the
   database of origin could be noted (refseq records normally come as other) */
   REFSEQ = FALSE;
   if((strstr(pfilenam, "rscu") != NULL) || (strstr(pfilenam, "rsnc")!= NULL))
    {
      REFSEQ = TRUE;
    }

   if( !InitNAMES() )
   {
     ErrPostEx(SEV_ERROR,0,0, "Main: InitNAMES failed.");
     return 1;
   }

   if( !InitCOMGEN() )
   {
     ErrPostEx(SEV_ERROR,0,0, "Main: InitCOMGEN failed.");
     return 1;
   }

   /* set autocommit off */
   if (!StartTransaction()) {
	ErrPostEx(SEV_ERROR, 0, 0, "Cannot start transaction!");
	return 1;
   }


   if (!OpenASNDB(pfilenam, (Boolean) myargs[2].intvalue))
   {
       ErrPostEx(SEV_ERROR,0,0, "Main: OpenASNDB failed.");
       return 1;
   }

   if (!OpenACCDB("ACCDB", (Boolean)  myargs[3].intvalue))
    {
       ErrPostEx(SEV_ERROR,0,0, "Main: OpenACCDB failed.");
       return 1;
    }
   if (!OpenPUBSEQ("PUBSEQ", (Boolean)  myargs[3].intvalue))
    {
       ErrPostEx(SEV_ERROR,0,0, "Main: OpenPUBSEQ failed.");
       return 1;
    }
   if (!OpenTAXGI("TAXGI", (Boolean)  myargs[3].intvalue))
    {
       ErrPostEx(SEV_ERROR,0,0, "Main: OpenTAXGI failed.");
       return 1;
    }
   if (!OpenPARTI("PARTI", (Boolean)  myargs[3].intvalue))
    {
       ErrPostEx(SEV_ERROR,0,0, "Main: OpenPARTI failed.");
       return 1;
    }
   if (!OpenNUCPROT("NUCPROT", (Boolean)  myargs[3].intvalue))
    {
       ErrPostEx(SEV_ERROR,0,0, "Main: OpenNUCPROT failed.");
       return 1;
    }

   if(!OpenSENDB(pfilenam, (Boolean) myargs[2].intvalue))
        return 1;
   if(!OpenSENGI("SENGI", (Boolean) myargs[3].intvalue))
      return 1;

   if(!OpenHISTDB(FALSE))
   {
	 ErrPostEx(SEV_ERROR,0,0, "Cannot open HISTDB database!");
	 return 1;
   }

   pncbidate = DateCurr();
   gpdate = GetStringDate(pncbidate);

   /*********************************************
    *
    * the code below is adopted from update.c
    * zwang 20041123, 20050202
    *
    *********************************************/
  if (UPDATE) {
   ErrPostEx(SEV_INFO,0,0, "Starting Update");

   

   /* get pointer to all loaded ASN.1 modules */
   amp = AsnAllModPtr();
   if (amp == NULL)
    {
     ErrShow();
     return 1;
    }

   atp = AsnFind("Bioseq-set");    /* get the initial type pointers */
   if (atp == NULL)
    {
     ErrShow();
     return 1;
    }
   atp2 = AsnFind("Bioseq-set.seq-set.E");
   if (atp2 == NULL)
    {
     ErrShow();
     return 1;
    }



   /* open the ASN.1 input file in the right mode */
#if (ASNFORMAT == 1) /* text ASN mode */
   if ((aipin = AsnIoOpen (myargs[0].strvalue, "r")) == NULL)
#else /* binary ASN mode */
   if ((aipin = AsnIoOpen (myargs[0].strvalue, "rb")) == NULL)
#endif
    {
      ErrPostEx(SEV_ERROR,0,0, "Main: Cannot open %s.", IsNullStr(myargs[0].strvalue));
      return 1;
    }

      /* read Seq-entry's from a Bioseq-set */
      while ((atp = AsnReadId(aipin, amp, atp)) != NULL)
        {
          if (atp == atp2)    /* top level Seq-entry */
           {
             sep = SeqEntryAsnRead(aipin, atp);
             if (sep == NULL)
              {
                ErrPostEx(SEV_ERROR,0,0, "Main: Seq-entry pointer is NULL.");
	        return 1;
              }
             /* ErrPostEx(SEV_ERROR,0,0, "MAIN: Got Seq-entry pointer."); */
             if (!ToBioseqUp(sep, 1, pfilenam))
                 ErrPostEx(SEV_ERROR,0,0, "Main: Call to ToBioseqUp,  failed.");

             SeqEntryFree(sep);
           }
          else
            /* don't need to read the data */
            AsnReadVal(aipin, atp, NULL);
        }

   AsnIoClose(aipin);

   /* do all deletions, potentially print out lists of deleted/new/changed GIs for the day */
   gpvnaddhead    = gpvnadd;
   gpvnchangehead = gpvnchange;
   gpvndeletehead = gpvndelete;
   gpvnextrahead  = gpvnextra;

   /* SK: */
   snprintf(gisname, PATH_MAX, "%sgis", shortName);

   pfile = FileOpen(gisname, "w");

   /* delete all changed GIs */
   fprintf(pfile,"\n\nCHANGED\n");
   fflush(pfile);

   while (gpvnchange != NULL)
   {
       if (!IntrezDeleteGI(gpvnchange->data.intvalue, TRUE) )
          ErrPostEx(SEV_ERROR,0,0, "IntrezDeleteGI failed on GI %ld.", gpvnchange->data.intvalue);

     fprintf(pfile, "%ld\n", (long) gpvnchange->data.intvalue);
       fflush(pfile);
       gpvnchange = gpvnchange->next;
   }

   /* delete all obsolete GIs */
   fprintf(pfile,"\n\nDELETED\n");
   fflush(pfile);
   while (gpvndelete != NULL)
   {
       if (!IntrezDeleteGI(gpvndelete->data.intvalue, TRUE))
          ErrPostEx(SEV_ERROR,0,0, "IntrezDeleteGI failed on GI %ld.", gpvndelete->data.intvalue);

       fprintf(pfile, "%ld\n", (long) gpvndelete->data.intvalue);
       fflush(pfile);
       gpvndelete = gpvndelete->next;
   }

   fprintf(pfile,"\n\nADDED\n");
   fflush(pfile);
   while(gpvnadd != NULL)
   {
       fprintf(pfile, "%ld\n", (long) gpvnadd->data.intvalue);
       fflush(pfile);
       gpvnadd = gpvnadd->next;
   }

   fprintf(pfile,"\n\nEXTRAS\n");
   fflush(pfile);
   while(gpvnextra != NULL)
   {

       if (!IntrezDeleteGI(gpvnextra->data.intvalue, TRUE))
          ErrPostEx(SEV_ERROR,0,0, "IntrezDeleteGI failed on GI %ld.", gpvndelete->data.intvalue);

       fprintf(pfile, "%ld\n", (long) gpvnextra->data.intvalue);
       fflush(pfile);
       gpvnextra = gpvnextra->next;
   }

   /* free stuff */
   if(gpvnaddhead) ValNodeFree(gpvnaddhead);
   if(gpvnchangehead) ValNodeFree(gpvnchangehead);
   if(gpvndeletehead) ValNodeFree(gpvndeletehead);
   if(gpvnextrahead) ValNodeFree(gpvnextrahead);
   gpdate = MemFree(gpdate);
   pncbidate = DateFree(pncbidate);

   FileClose(pfile);
  } /* if (UPDATE) */
  tim2=GetSecs();
  if (UPDATE) { 
   ErrPostEx(SEV_WARNING,11,1,"Total elapsed time for updating: %ld sec", (tim2-tim1));
   ErrPostEx(SEV_INFO,0,0, "Update: Done!");
  } /* if (UPDATE) */


  
   /*********************************************
    *
    * the code above is adopted from update.c
    * zwang 20041123, 20050202
    *
    *********************************************/

   /* the log file for mother is *run 
   sprintf(logname,"%srun", shortName);
   ErrSetLogfile(logname, ELOG_BANNER);*/

   ErrPostEx(SEV_INFO,0,0, "Starting Mother.");
   /* get pointer to all loaded ASN.1 modules */
   amp = AsnAllModPtr();
   if (amp == NULL)
    {
     ErrShow();
     return 1;
    }

   atp = AsnFind("Bioseq-set");    /* get the initial type pointers */
   if (atp == NULL)
    {
     ErrShow();
     return 1;
    }
   atp2 = AsnFind("Bioseq-set.seq-set.E");
   if (atp2 == NULL)
    {
     ErrShow();
     return 1;
    }

   /* open the ASN.1 input file in the right mode */
#if (ASNFORMAT == 1) /* text ASN mode by define*/
   aipin = AsnIoOpen (myargs[0].strvalue, "r");
#else
   if(myargs[6].intvalue) /* text or binary ASN read determined by command line param */
     aipin = AsnIoOpen (myargs[0].strvalue, "r");
   else
     aipin = AsnIoOpen (myargs[0].strvalue, "rb");
#endif
    if(aipin == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "Main: Cannot open %s.", IsNullStr(myargs[0].strvalue));
      return 1;
    }

   /*intialize the sendb counter to the last record currently in the table*/
/* comment it out since this is not in use but extremely time-consuming
    ibi = SHoundRecNoSENGI();
      if(ibi == -1) ibi = 1;
*/

   /* read Seq-entry's from a Bioseq-set */
   while ((atp = AsnReadId(aipin, amp, atp)) != NULL)
   {
	 /* tim3 = GetSecs(); */
	 if (atp == atp2)    /* top level Seq-entry */
     {
	   sep = SeqEntryAsnRead(aipin, atp);
	   if (sep == NULL)
	   {
		 ErrPostEx(SEV_ERROR,0,0, "Main: Seq-entry pointer is NULL.");
		 return 1;
	   }

	  /*--------------------------------------------------------
            Set up the lists needed to extract the names.
            ------------------------------------------------------*/
	   pCdrList = CreateCdrList();
	   pLocusList = CreateLocusList();
	   SetNameInfo( &nameInfo, pCdrList, pLocusList, NULL, TRUE );
                       
	   if (!ToBioseq( sep, 1, NULL, pfilenam, myargs[1].strvalue, 
					  &nameInfo ))
	   {
		 ErrPostEx(SEV_ERROR,0,0, "Main: Call to ToBioseq failed.");
		 return 1;
	   }
          /*the seid will be whatever the first gi in the group was*/
	   ibi = seid;

	   if (!ToBioseqSEQENTRY(&sep, 1, ibi, pfilenam))
	   {
		 ErrPostEx(SEV_ERROR,0,0,"Main: Call to ToBioseqSEQENTRY failed.");
		 return 0;
	   }

             /* append the SeqEntry to the SENDB database */
	   ResetStSENDB(&psd);
	   psd->seid = ibi;
	   psd->asn1 = sep;
	   AppendRecordSENDB(psd);
	   insert_count = insert_count + 1;
	   SeqEntryFree(sep);
#ifdef ODBC
	   FillNameDBs( pCdrList, pLocusList );
#endif /*ifdef ODBC*/
	   DestroyCdrList( pCdrList );
	   DestroyLocusList( pLocusList );
	   pCdrList = NULL;
	   pLocusList = NULL;
	   SetNameInfo( &nameInfo, pCdrList, pLocusList, NULL, FALSE );
	   
	 }
	 else
         /* don't need to read the data */

	   AsnReadVal(aipin, atp, NULL);

	/*check to see how many inserts we are up too.  If it is more
	 than 10000 then commit the changes. */
	if (!UPDATE && insert_count >= MAXINSERTS)
	{
	  insert_count = 0;
	  if( !CommitAndRollBack() )
	  {
		ErrPostEx(SEV_ERROR, 0, 0, "Main:Cannot commit transaction!");
		return 1;
	  }
	}
   
	/* tim4 = GetSecs();
	ErrPostEx(SEV_WARNING,11,1,"Total elapsed time for one seq-entry:
%ld sec",(tim4-tim3));*/
   }

	/* commit for the last time:
	* all changes if this is an update,
	* otherwize the last bit of insertion */

   if( !CommitAndRollBack() )
   {
	 ErrPostEx(SEV_ERROR, 0, 0, "Main:Cannot commit transaction!");
	 return 1;
   }
   AsnIoClose(aipin);


   if (!CloseASNDB()) ErrPostEx(SEV_ERROR,0,0, "CloseASNDB failed.");
   if (!CloseACCDB()) ErrPostEx(SEV_ERROR,0,0, "CloseACCDB failed.");
   if (!ClosePUBSEQ()) ErrPostEx(SEV_ERROR,0,0, "ClosePUBSEQ failed.");
   if (!CloseTAXGI())  ErrPostEx(SEV_ERROR,0,0, "CloseTAXGI failed.");
   if (!ClosePARTI())  ErrPostEx(SEV_ERROR,0,0, "ClosePARTI failed.");
   if (!CloseNUCPROT())ErrPostEx(SEV_ERROR,0,0, "CloseNUCPROT failed.");


   CloseCodeBase();
   MemFree(pas);
   MemFree(ppa);
   MemFree(pac);
   MemFree(pnc);
   MemFree(pps);
   MemFree(ptg);

   tim3=GetSecs();

   ErrPostEx(SEV_WARNING,11,1,"Total elapsed time %ld : %ld sec for update,  %ld sec for mother", (tim3-tim1), (tim2-tim1), (tim3-tim2));
   ErrPostEx(SEV_INFO,0,0, "Main: Done!");
   return 0;
}

/*
$Log: mother.c,v $
Revision 1.36  2005/04/26 19:14:46  eburgess
Cleaned up the code and removed some comments.

Revision 1.35  2005/03/29 19:51:48  eburgess
Fixed call to LogHistory() in ToBioseqUp(). If the accession is null, set it
to 'n/a'.

Revision 1.34  2005/03/23 17:15:54  zwang
Fixed the bug in timing.

Revision 1.33  2005/03/17 15:43:56  eburgess
Make sure that "Extra" gis are deleted. Removed a printf statement in
IntrezDeleteGi().

Revision 1.32  2005/03/16 20:34:34  eburgess
Cleaned up the code and made sure everything is deleted properly.

Revision 1.31  2005/03/15 21:15:58  eburgess
Many changes to fix bugs in the new update strategy.

Revision 1.30  2005/03/14 20:33:02  eburgess
If there is no accession, then log a name and gi into the history table
in ToBioseqUp.

Revision 1.29  2005/03/14 16:05:44  eburgess
Major changes to how updates are handled. Look for additional gis associated
with a given accession and check for name if accession is NULL. Bugs 4939 and
5049

Revision 1.28  2005/03/09 17:11:32  eburgess
Changed fields in histdb table. Modified mother accordingly. Also write to
histdb in ToBioseq, when we are not updating.

Revision 1.27  2005/03/02 16:51:53  eburgess
Added transactional support and fixed the log file reporting so that the
update file is not overwritten when the regular mother starts.

Revision 1.26  2005/02/23 19:06:24  eburgess
Made ProcessTopComgenGi and ProcessContigGi more efficient.

Revision 1.25  2005/02/02 22:00:23  zwang
Added ability of building a transactional database.
Combined update.c with it to make daily update of core data in one transaction.
See bug 3441.

Revision 1.24  2005/02/01 16:52:40  eburgess
More changes for comgen and changed FillAccDB to handle more cases of
SeqIds. Bug 4504

Revision 1.23  2005/01/10 19:29:51  eburgess
Many changes for the comgen module.

Revision 1.22  2004/11/10 18:41:52  eburgess
Code to handle parsing of division from the bioseqs. Bug 1193. Also changed
FillACCDB() to get the title using NCBI's CreateDefLineo() instead of trying
to parse it out ourselves. Bug 3464

Revision 1.21  2004/10/19 15:46:18  eburgess
Added include file parseasn.h.

Revision 1.20  2004/10/15 20:03:48  eburgess
Many changes so that complete genome info can be parsed out of refseq
bioseqs. Bug 2952

Revision 1.19  2004/09/22 18:56:13  eburgess
Removed a lot of unused variables etc. to get rid of warnings.

Revision 1.18  2004/09/17 15:35:26  eburgess
Many changes so that mother will parse complete genomes. Bug 2952

Revision 1.17  2004/08/12 16:14:54  eburgess
Commented out changes for bug 2985.

Revision 1.16  2004/08/11 19:52:34  eburgess
Bug 2985: Check for other NCBI internal databases in FillAccdb().

Revision 1.15  2004/08/11 14:27:53  eburgess
Check that the database is not NCBI_GENOMES in FillACCDB, before calling
WriteACCDB().

Revision 1.14  2004/08/03 13:49:39  eburgess
Changes to prevent warnings: replaced ADD by ADDED and DELETE by DELETED and
made WriteProteinNames() returntype void.

Revision 1.13  2004/07/30 12:50:29  eburgess
Changed so that mother always checks to see if names are present because the
refseq release appears to contain some duplicate records. Used #ifdef UPDATE.

Revision 1.12  2004/07/27 12:40:33  eburgess
Fixed bug in FillBionameRecord().

Revision 1.11  2004/07/22 18:41:53  eburgess
Make sure that updates account for names that are deleted and readded.

Revision 1.10  2004/07/09 15:54:37  eburgess
Added functionality to update the names table. Also get all names from
the prot-ref and title fields.

Revision 1.9  2004/06/07 22:10:30  hlieu
minor fixes

Revision 1.8  2004/06/03 16:16:06  eburgess
Added back some variables that Hao removed as they're reqired for odbc.

Revision 1.7  2004/05/31 20:10:03  hlieu
fix some compiler warnings

Revision 1.6  2004/05/03 16:23:02  eburgess
Added many functions to parse names from bioseqs and bioseqset.

Revision 1.5  2004/04/07 16:15:28  vgu
Set CodeBase Exclusvie Read and Write Flag

Revision 1.4  2004/03/30 20:37:36  kmichali
keeps track of all Bioseq-set descriptors and parses them; changes to all dependent functionality - taxgi, pubseq, accdb

Revision 1.3  2004/03/05 18:38:23  kmichali
improved taxonomy identifier parsing

Revision 1.2  2004/01/06 22:32:13  skuznets
Code audit has been done sucessfully.
Possible NULL pointer issues has been eliminated.

Revision 1.1.1.13  2003/07/26 08:41:53  root
Automated import from Sourceforge.net

Revision 1.13  2003/07/25 15:16:10  haocl
added changes to parsing to extract pmid & save into pubseq

Revision 1.12  2002/10/31 22:00:18  kaca
removed printf

Revision 1.11  2002/10/30 21:11:50  kaca
human and mouse complete genome data go into gbchm division

Revision 1.10  2002/10/28 22:08:33  kaca
small changes in  GetProtFromHumanMouse

Revision 1.9  2002/10/25 20:03:02  kaca
changed prototype of GetProtFromHumanMouse

Revision 1.8  2002/10/24 21:34:00  kaca
added option for text ASN, added code which gets rna protein relationship for mouse and human XPs and XMs

Revision 1.7  2002/08/22 13:43:02  mdumonti
added define for building either text or binary ASN mode

Revision 1.6  2002/07/08 17:27:17  kaca
removed unused variables

Revision 1.5  2002/01/17 17:54:52  kaca
fixed boolean on opening of SENGI

Revision 1.4  2001/12/19 23:27:31  kaca
merged mother and sendb functionality

Revision 1.3  2001/10/19 20:02:26  kaca
cosmetic fix on SeqId choice values

Revision 1.2  2001/10/18 15:33:48  kaca
added FileNameFind from the input arg

Revision 1.1.1.2  2001/10/10 21:53:47  kaca
initial import

Revision 1.14  2001/06/08 15:27:52  kaca
moved lower casing of names to the library

Revision 1.13  2001/04/03 19:12:35  kaca
warning fixes

Revision 1.12  2001/03/29 15:47:38  kaca
added an include

Revision 1.11  2001/03/27 21:25:21  kaca
changed database names in ACCDB to reflect Entrez

Revision 1.10  2001/03/26 19:48:05  kaca
changes in include files

Revision 1.9  2001/03/19 21:36:32  kaca
added new command line argument which directs the update files to update database

Revision 1.8  2001/02/23 20:41:16  kaca
license change

Revision 1.7  2001/02/05 20:55:37  kaca
deleted unused variable

Revision 1.6  2001/01/26 19:33:17  kaca
abstracted database calls

Revision 1.1  2000/12/22 20:34:05  kaca
abstracted database calls

Revision 1.5  2000/11/08 18:40:45  kaca
minor change

Revision 1.4  2000/10/13 21:52:18  kaca
changed includes

Revision 1.3  2000/09/20 18:49:59  kaca
split SeqHound headers into private and public one and changed includes

Revision 1.2  2000/09/18 23:17:50  kaca
newline added at the end

Revision 1.1.1.1  2000/09/18 17:41:56  kaca
Initial import



Revision 1.3  2001/10/25 16:44:26  risserlin
check in the latest working parsers.(to parseswissprot still have to change from rb to r)

Revision 1.2  2001/09/06 20:07:20  risserlin
did all aix specific changes for db2

Revision 1.1.1.1  2001/09/06 13:44:25  risserlin
initial import of seqhound code retrieved from slri on Ausust 16 and revised to include all mdsp changes

Revision 1.1.1.1  2001/04/05 15:48:59  ruth
lateset code as of March 30

Revision 1.12  2001/03/29 15:47:38  kaca
added an include

Revision 1.11  2001/03/27 21:25:21  kaca
changed database names in ACCDB to reflect Entrez

Revision 1.10  2001/03/26 19:48:05  kaca
changes in include files

Revision 1.9  2001/03/19 21:36:32  kaca
added new command line argument which directs the update files to update database

Revision 1.8  2001/02/23 20:41:16  kaca
license change

Revision 1.7  2001/02/05 20:55:37  kaca
deleted unused variable

Revision 1.6  2001/01/26 19:33:17  kaca
abstracted database calls

Revision 1.1  2000/12/22 20:34:05  kaca
abstracted database calls

Revision 1.5  2000/11/08 18:40:45  kaca
minor change

Revision 1.4  2000/10/13 21:52:18  kaca
changed includes

Revision 1.3  2000/09/20 18:49:59  kaca
split SeqHound headers into private and public one and changed includes

Revision 1.2  2000/09/18 23:17:50  kaca
newline added at the end

Revision 1.1.1.1  2000/09/18 17:41:56  kaca
Initial import

*/



