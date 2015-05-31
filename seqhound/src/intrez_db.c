/* $Id: intrez_db.c,v 1.3 2004/01/06 22:32:15 skuznets Exp $
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
  SeqHound system API

    July 1999
    added a new field to the ACCDB database: version of accession to reflect the
    NCBI versoning system
    September 1999
    added the REDUND database and took redundant info out of TAXGIFIN and TAXGI
    TAXGIFIN taken out of the system
    October 1999
    title in ACCDB now stored in Memo field
    June 2000
    removed all remote functions, they live in ambiseqhoundapi.c only the lighweight
    (no local obtion) version is maintained

******************************************************************************************/
#include <intrez_db.h>
#include <intrez_misc.h>
#include <intrez.h>

CODE4 cb;

DATA4
     *pd4Asndb,
     *pd4Partition,
     *pd4Nucprot,
     *pd4Accdb,
     *pd4Pubseq,
     *pd4Taxgi,
     *pd4Chrom,
     *pd4Sendb,
     *pd4Sengi,
     *pd4Mmdb,
     *pd4Mmgi,
     *pd4Redund,
     *pd4Domdb,
     *pd4Vastdb,
     *pd4Nrbioseq,
     *pd4Cddb;	

FIELD4
      /* ASNDB */
      *pf4asGi,
      *pf4asAsn1,
      *pf4asType,
      *pf4asDivision,
      *pf4asRelease,
      /* PARTI */
      *pf4paGi,
      *pf4paParti,
      /* NUCPROT */
      *pf4ncGin,
      *pf4ncGia,
      /* ACCDB */
      *pf4acGi,
      *pf4acDb,
      *pf4acName,
      *pf4acAccess,
      *pf4acChain,
      *pf4acRelease,
      *pf4acVersion,
      *pf4acTitle,
      *pf4acNamelow,
      /* PUBSEQ */
      *pf4psGi,
      *pf4psMuid,
      /* TAXGI */
      *pf4tgGi,
      *pf4tgTaxId,
      *pf4tgKloodge,
      *pf4tgType,
      /* CHROM */
      *pf4chTaxId,
      *pf4chKloodge,
      *pf4chChromfl,
      /* SENDB */
      *pf4sdSeid,
      *pf4sdAsn1,
      /* SENGI */
      *pf4sgSeid,
      *pf4sgGi,
      *pf4sgDivision,
       /* MMDB */
      *pf4mdAsn1,
      *pf4mdMmdbId,
      *pf4mdPdbId,
      *pf4mdbWhat,
      *pf4mdModels,
      *pf4mdMolecules,
      *pf4mdSize,
      *pf4mdBzSize,
      *pf4mdRep,
      /* MMGI */
      *pf4mgMmdbId,
      *pf4mgGi,
      /* REDUND */
      *pf4rdGi,
      *pf4rdOrdinal,
      *pf4rdGroup,
       /* DOMDB */
       *pf4ddMmdbId,
       *pf4ddAsn1,
       *pf4ddPdbId,
       *pf4ddChain,
       *pf4ddGi,
       *pf4ddDomno,
       *pf4ddDomall,
       *pf4ddDomId,
       *pf4ddRep,
        /* VASTDB */
       *pf4vdMmdbId,
       *pf4vdPdbId,
       *pf4vdChain,
       *pf4vdAsn1,
       *pf4vdVastId1,
       *pf4vdVastId2,
       *pf4vdScore,
       *pf4vdPval,
       *pf4vdRes,
       *pf4vdIdent,
       *pf4vdRmsd,
       *pf4vdScale,
       /* NRBIOSEQ */
       *pf4nbGi,
       *pf4nbAsn1,
	/*CDDB*/
       *pf4cdGi,
       *pf4cdAsn1;	

TAG4
      /* ASNDB */
      *tagasGi,
      /* PARTI */
      *tagpaGi,
      /* NUCPROT */
      *tagncGi,
      *tagncGin,
      *tagncGia,
      /* ACCDB */
      *tagacGi,
      *tagacAcc,
      *tagacName,
      *tagacNamelow,
      /* PUBSEQ */
      *tagpsGi,
      *tagpsMuid,
      /* TAXGI */
      *tagtgGi,
      *tagtgTaxId,
      *tagtgKloodge,
      /* CHROM */
      *tagchTaxId,
      *tagchKloodge,
      *tagchChromfl,
      /* SENDB */
      *tagsdSeid,
      /* SENGI */
      *tagsgGi,
      *tagsgSeid,
      /* MMDB */
      *tagmdMmdbId,
      *tagmdPdbId,
      *tagmdRep,
      /* MMGI */
      *tagmgMmdbId,
      *tagmgGi,
      /* REDUND */
      *tagrdGi,
      *tagrdOrdinal,
      *tagrdGroup,
       /* DOMDB */
       *tagddMmdbId,
       *tagddPdbId,
       *tagddGi,
       *tagddDomall,
       *tagddDomId,
       /* VASTDB */
       *tagvdMmdbId,
       *tagvdPdbId,
       *tagvdVastId1,
       /* NRBIOSEQ */
       *tagnbGi,
    	/*CDDB*/
       *tagcdGi;


/* CodeBase FIELD4INFO  and TAG4INFO structures */

static FIELD4INFO f4iAsndb[ ] =
{
    { "GI",       'N', 10, 0 },    /* Gi*/
    { "ASN1",     'M', 10, 0 },    /* The corresponding ASN.1 */
    { "DIVISION", 'C', 10, 0 },    /* GenBank division */
    { "RELEASE",  'C', 10, 0 },    /* Release */
    { "TYPE",     'C', 15, 0 },    /* Nucleic acid or Protein */
    { 0,0,0,0 }
};


static TAG4INFO t4iAsndb[ ] =
{
  { "GI", "GI", 0, 0, 0 },
  { 0, 0, 0, 0, 0 }
};


static FIELD4INFO f4iPartition[ ] ={
    { "GI",       'N', 10, 0 },    /* Gi*/
    { "PARTITION",'C', 10, 0 },    /* Which database is GI stored in */
    { 0,0,0,0 }
};


static TAG4INFO t4iPartition[ ] =
{
  { "GI", "GI", 0, 0, 0 },
  { 0, 0, 0, 0, 0 }
};


static FIELD4INFO f4iNucprot[ ] =
{
    { "GIN",  'N', 10, 0 },  /* DNA GI*/
    { "GIA",  'N', 10, 0 },  /* protein GI  */
    { 0,0,0,0 }
};


static TAG4INFO t4iNucprot[ ] =
{
  { "GIN", "GIN", 0, 0, 0 },
  { "GIA", "GIA", 0, 0, 0 },
  { 0, 0, 0, 0, 0 }
};



static FIELD4INFO f4iAccdb[ ] =
{
    { "GI",      'N', 10,  0 },    /* Gi*/
    { "DB",      'C', 15,  0 },    /* database name */
    { "NAME",    'C', 30,  0 },    /* sequence name */
    { "NAMELOW", 'C', 30,  0 },    /* sequence name in lower case */
    { "ACCESS",  'C', 20,  0 },    /* GenBank accession */
    { "CHAIN",   'C', 10,  0 },    /* PDB chain */
    { "RELEASE", 'C', 10,  0 },    /* release in other database */
    { "VERSION", 'N', 10,  0 },    /* accession version */
    { "TITLE",   'M', 10,  0 },    /* title in seqdescr */
    { 0,0,0,0 }
};


static TAG4INFO t4iAccdb[ ] =
{
  { "GI", "GI", 0, 0, 0 },
  { "ACCESS", "ACCESS", 0, 0, 0 },
  { "NAME", "NAME", 0, 0, 0 },
  { "NAMELOW", "NAMELOW", 0, 0, 0 },
  { 0, 0, 0, 0, 0 }
};


static FIELD4INFO f4iPubseq[ ] =
{
    { "GI",      'N', 10,  0 },    /* Gi*/
    { "MUID",    'N', 10,  0 },    /* medline Id */
    { 0,0,0,0 }
};


static TAG4INFO t4iPubseq[ ] =
{
  { "GI", "GI", 0, 0, 0 },
  { "MUID", "MUID", 0, 0, 0 },
  { 0, 0, 0, 0, 0 }
};


static FIELD4INFO f4iTaxgi[ ] =
{
    { "GI",      'N', 10,  0 },    /* Gi*/
    { "TAXID",   'N', 10,  0 },    /* NCBI taxonomy Id */
    { "KLOODGE", 'N', 10,  0 },    /* NCBI Kloodge Id */
    { "TYPE",    'C', 15,  0 },    /* type of molecule */
    { 0,0,0,0 }
};


static TAG4INFO t4iTaxgi[ ] =
{
  { "GI", "GI", 0, 0, 0 },
  { "TAXID", "TAXID", 0, 0, 0 },
  { "KLOODGE", "KLOODGE", 0, 0, 0 },
  { 0, 0, 0, 0, 0 }
};


static FIELD4INFO f4iChrom[ ] ={
    { "TAXID",   'N', 10,  0 },     /* NCBI TaxId */
    { "KLOODGE", 'N', 10,  0 },     /* NCBI Kloodge ID */
    { "CHROMFL", 'N', 10,  0 },     /* chromosome, plasmid, mitochondria.. in a byte flag*/
    { 0,0,0,0 }
};

static TAG4INFO t4iChrom[ ] =
{
  { "TAXID",   "TAXID",  0, 0, 0 },
  { "KLOODGE",  "KLOODGE", 0, 0, 0 },
  { "CHROMFL", "CHROMFL",0, 0, 0 },
  { 0, 0, 0, 0, 0 }
};


static FIELD4INFO f4iSendb[ ] ={
    { "SEID",  'N', 10,  0 },  /* Seq-entry Id */
    { "ASN1",  'M', 10,  0 },   /* Seq-entry Asn1 */
    { 0,0,0,0 }
};


static TAG4INFO t4iSendb[ ] =
{
  { "SEID", "SEID", 0, r4unique, 0 },
  { 0, 0, 0, 0, 0 }
};

static FIELD4INFO f4iSengi[ ] ={
    { "GI",       'N', 10,  0 },   /* GI */
    { "SEID",     'N', 10,  0 },   /* Seq-entry Id */
    { "DIVISION", 'C', 10, 0 },    /* GenBank division */
    { 0,0,0,0 }
};


static TAG4INFO t4iSengi[ ] =
{
  { "SEID", "SEID", 0, 0, 0 },
  { "GI",   "GI",   0, 0, 0 },
  { 0, 0, 0, 0, 0 }
};

static FIELD4INFO f4iMmdb[ ] ={
    { "MMDBID",   'N', 10,  0 },   /* Mmdb id*/
    { "ASN1",     'M', 10,  0 },   /* Biostruc */
    { "PDBID",    'C', 20,  0 },   /* PDB id*/
    { "BWHAT",    'N', 10,  0 },   /* types of molecules in the biostruc */
    { "MODELS",   'N', 10,  0 },   /* number of models */
    { "MOLECULES",'N', 10,  0 },   /* number of molecules */
    { "SIZE",     'N', 10,  0 },   /* size of uncompressed biostruc */
    { "BZSIZE",   'N', 10,  0 },   /* size of compressed biostruc */
    { 0,0,0,0 }
};

static TAG4INFO t4iMmdb[ ] =
{
  { "MMDBID",  "MMDBID", 0, 0, 0 },
  { "PDBID",   "PDBID",  0, 0, 0 },
  { 0, 0, 0, 0, 0 }
};


static FIELD4INFO f4iMmgi[ ] ={
    { "MMDBID",   'N', 10,  0 },   /* Mmdb id*/
    { "GI",       'N', 10,  0 },   /* GI */
    { 0,0,0,0 }
};

static TAG4INFO t4iMmgi[ ] =
{
  { "MMDBID",  "MMDBID", 0, 0, 0 },
  { "GI",      "GI",     0, 0, 0 },
  { 0, 0, 0, 0, 0 }
};


static FIELD4INFO f4iRedund[ ] ={
    { "GI",      'N', 10,  0 },    /* Gi*/
    { "ORDINAL", 'N', 10,  0 },    /* position in redundant set */
    { "GROUP",   'N', 10,  0 },    /* group # of redundant proteins */
    { 0,0,0,0 }
};


static TAG4INFO t4iRedund[ ] =
{
  { "GI",     "GI",     0, 0, 0 },
  { "ORDINAL",  "ORDINAL",  0, 0, 0 },
  { "GROUP", "GROUP", 0, 0, 0 },
  { 0, 0, 0, 0, 0 }
};

static FIELD4INFO f4iDomdb[ ] ={
    { "MMDBID",   'N', 10,  0 },   /* Mmdb id*/
    { "ASN1",     'M', 10,  0 },   /* SLRIValNode */
    { "PDBID",    'C', 20,  0 },   /* PDB id*/
    { "CHAIN",    'C', 10,  0 },   /* PDB chain*/
    { "GI",       'N', 10,  0 },   /* GI */
    { "DOMNO",    'N', 10,  0 },   /* domain number */
    { "DOMALL",   'N', 10,  0 },   /* number of domains in the whole structure */
    { "DOMID",    'N', 10,  0 },   /* vast domain id */
    { "REP",      'N', 10,  0 },   /* representative of blast set */
    { 0,0,0,0 }
};

static TAG4INFO t4iDomdb[ ] =
{
  { "MMDBID", "MMDBID", 0, 0, 0 },
  { "PDBID",  "PDBID",  0, 0, 0 },
  { "GI",     "GI",     0, 0, 0 },
  { "DOMALL", "DOMALL", 0, 0, 0 },
  { "DOMID",  "DOMID",  0, 0, 0 },
  { 0, 0, 0, 0, 0 }
};


static FIELD4INFO f4iVastdb[ ] ={
    { "MMDBID",   'N', 10,  0 },   /* Mmdb id*/
    { "ASN1",     'M', 10,  0 },   /* BiostrucFeature */
    { "PDBID",    'C', 20,  0 },   /* PDB id*/
    { "CHAIN",    'C', 10,  0 },   /* PDB chain*/
    { "VASTID1",  'N', 10,  0 },   /* vast ID */
    { "VASTID2",  'N', 10,  0 },   /* vast ID */
    { "SCORE",    'N', 10,  0 },   /* vast score*/
    { "PVAL",     'N', 10,  0 },   /* vast mlog P*/
    { "RES",      'N', 10,  0 },   /* residues aligned*/
    { "IDENT",    'N', 10,  0 },   /* identity between vastid1 and vastid2*/
    { "RMSD",     'N', 10,  0 },   /* rmsd between vastid1 and vastid2*/
    { "SCALE",    'N', 10,  0 },   /* scale factor */
    { 0,0,0,0 }
};

static TAG4INFO t4iVastdb[ ] =
{
  { "MMDBID", "MMDBID", 0, 0, 0 },
  { "PDBID",  "PDBID",  0, 0, 0 },
  { "VASTID1","VASTID1",0, 0, 0 },
  { 0, 0, 0, 0, 0 }
};

static FIELD4INFO f4iNrbioseq[ ] =
{
    { "GI",       'N', 10, 0 },    /* Gi*/
    { "ASN1",     'M', 10, 0 },    /* The corresponding ASN.1 type Bioseq*/
    { 0,0,0,0 }
};


static TAG4INFO t4iNrbioseq[ ] =
{
  { "GI", "GI", 0, 0, 0 },
  { 0, 0, 0, 0, 0 }
};


/*March 2001, Doron- Coding region database */
static FIELD4INFO f4Cddb[ ] =
{
	{ "GI",   'N', 10, 0 },    /*Gi*/
	{ "ASN1", 'M', 10, 0 },   /* the ASN.1 SLRIfasta coding region */
         {0,0,0,0 }
};

static TAG4INFO t4Cddb[ ]=
{
	{"GI", "GI" , 0 , 0 , 0 },
	{0, 0, 0, 0, 0}
};




/********************************************/
/*  Open databases                          */
/********************************************/
Boolean LIBCALL InitCodeBase(void)
{
   code4init(&cb);
   cb.safety = 1;
   /*cb.readOnly = 1;
   cb.accessMode = OPEN4DENY_WRITE;*/
   error4exitTest(&cb);
   code4largeOn(&cb);
   cb.errDefaultUnique = r4unique;

   return TRUE;
}


Boolean LIBCALL OpenASNDB(CharPtr name, Boolean Create)
{
   Char pathdatab[PATH_MAX];
   CharPtr path = NULL;

   pathdatab[0] = '\0';
   GetAppParam("intrez", "datab", "path", NULL, pathdatab, (size_t)PATH_MAX * (sizeof(Char)));
   path = AllocateAndCreateDBPath(pathdatab, name);
   if(Create == FALSE)
    {
       pd4Asndb = d4open(&cb, path);
       if (pd4Asndb == NULL)
       {
		   MemFree(path);
           return FALSE;
       }
    }
   else
    {
       pd4Asndb = d4create( &cb, path, f4iAsndb, t4iAsndb);
       if (cb.errorCode)
       {
	      MemFree(path);
          return FALSE;
	   }
    }

   pf4asGi = d4field(pd4Asndb,"GI");
   pf4asAsn1 = d4field(pd4Asndb,"ASN1");
   pf4asDivision = d4field(pd4Asndb,"DIVISION");
   pf4asRelease = d4field(pd4Asndb,"RELEASE");
   pf4asType = d4field(pd4Asndb,"TYPE");
   tagasGi = d4tag(pd4Asndb, "GI");
   MemFree(path);

   return TRUE;
}


Boolean LIBCALL OpenPARTI(CharPtr name, Boolean Create)
{
   Char pathdatab[PATH_MAX];
   CharPtr path = NULL;


   pathdatab[0] = '\0';
   GetAppParam("intrez", "datab", "path", "", pathdatab, (size_t)PATH_MAX * (sizeof(char)));
   path = AllocateAndCreateDBPath(pathdatab, name);
   if(Create == FALSE)
    {
       pd4Partition = d4open(&cb, path);
       if (pd4Partition == NULL)
	   {
          MemFree(path);
          return FALSE;
	   }
    }
   else
    {
       pd4Partition = d4create( &cb, path, f4iPartition, t4iPartition);
       if (cb.errorCode)
       {
          MemFree(path);
          return FALSE;
	   }
    }
          

   pf4paGi = d4field(pd4Partition,"GI");
   pf4paParti = d4field(pd4Partition,"PARTITION");
   tagpaGi = d4tag(pd4Partition, "GI");
   MemFree(path);

   return TRUE;
}


Boolean LIBCALL OpenNUCPROT(CharPtr name, Boolean Create)
{
   Char pathdatab[PATH_MAX];
   CharPtr path = NULL;


   pathdatab[0] = '\0';
   GetAppParam("intrez", "datab", "path", "", pathdatab, (size_t)PATH_MAX * (sizeof(char)));
   path = AllocateAndCreateDBPath(path, name);
   if(Create == FALSE)
    {
       pd4Nucprot = d4open(&cb, path);
       if (pd4Nucprot == NULL)
       {
          MemFree(path);
          return FALSE;
	   }    
    }
   else
    {
       pd4Nucprot = d4create( &cb, path, f4iNucprot, t4iNucprot);
       if (cb.errorCode)
       {
          MemFree(path);
          return FALSE;
	   }    
    }

   pf4ncGin = d4field(pd4Nucprot,"GIN");
   pf4ncGia = d4field(pd4Nucprot,"GIA");
   tagncGin = d4tag(pd4Nucprot, "GIN");
   tagncGia = d4tag(pd4Nucprot, "GIA");
   MemFree(path);

   return TRUE;
}



Boolean LIBCALL OpenACCDB(CharPtr name, Boolean Create)
{
   Char pathdatab[PATH_MAX];
   CharPtr path = NULL;

   pathdatab[0] = '\0';
   GetAppParam("intrez", "datab", "path", "", pathdatab, (size_t)PATH_MAX * (sizeof(char)));
   path = AllocateAndCreateDBPath(pathdatab, name);
   if(Create == FALSE)
    {
       pd4Accdb = d4open(&cb, path);
       if (pd4Accdb == NULL)
       {
          MemFree(path);
          return FALSE;
	   } 
    }
   else
    {
       pd4Accdb = d4create( &cb, path, f4iAccdb, t4iAccdb);
       if (cb.errorCode)
       {
          MemFree(path);
          return FALSE;
	   } 
    }

   pf4acGi = d4field(pd4Accdb,"GI");
   pf4acDb = d4field(pd4Accdb, "DB");
   pf4acName = d4field(pd4Accdb, "NAME");
   pf4acAccess = d4field(pd4Accdb, "ACCESS");
   pf4acChain = d4field(pd4Accdb, "CHAIN");
   pf4acRelease = d4field(pd4Accdb, "RELEASE");
   pf4acVersion = d4field(pd4Accdb, "VERSION");
   pf4acTitle = d4field(pd4Accdb, "TITLE");
   pf4acNamelow = d4field(pd4Accdb, "NAMELOW");
   tagacGi = d4tag(pd4Accdb, "GI");
   tagacAcc = d4tag(pd4Accdb, "ACCESS");
   tagacName = d4tag(pd4Accdb, "NAME");
   tagacNamelow = d4tag(pd4Accdb, "NAMELOW");
   MemFree(path); 

   return TRUE;
}


Boolean LIBCALL OpenPUBSEQ(CharPtr name, Boolean Create)
{
   Char pathdatab[PATH_MAX];
   CharPtr path = NULL;

   pathdatab[0] = '\0';
   GetAppParam("intrez", "datab", "path", "", pathdatab, (size_t)PATH_MAX * (sizeof(char)));
   path = AllocateAndCreateDBPath(pathdatab, name);
   if(Create == FALSE)
    {
       pd4Pubseq = d4open(&cb, path);
       if (pd4Pubseq == NULL)
       {
          MemFree(path);
          return FALSE;
	   } 
    }
   else
    {
       pd4Pubseq = d4create( &cb, path, f4iPubseq, t4iPubseq);
       if (cb.errorCode)
       {
          MemFree(path);
          return FALSE;
	   } 
    }

   pf4psGi = d4field(pd4Pubseq,"GI");
   pf4psMuid = d4field(pd4Pubseq, "MUID");
   tagpsGi = d4tag(pd4Pubseq, "GI");
   tagpsMuid = d4tag(pd4Pubseq, "MUID");
   MemFree(path);

   return TRUE;
}


Boolean LIBCALL OpenTAXGI(CharPtr name, Boolean Create)
{
   Char pathdatab[PATH_MAX];
   CharPtr path = NULL;

   pathdatab[0] = '\0';
   GetAppParam("intrez", "datab", "path", "", pathdatab, (size_t)PATH_MAX * (sizeof(char)));
   path = AllocateAndCreateDBPath(pathdatab, name);
   if(Create == FALSE)
    {
       pd4Taxgi = d4open(&cb, path);
       if (pd4Taxgi == NULL)
       {
          MemFree(path);
          return FALSE;
	   } 
    }
   else
    {
       pd4Taxgi = d4create( &cb, path, f4iTaxgi, t4iTaxgi);
       if (cb.errorCode)
       {
          MemFree(path);
          return FALSE;
	   } 
    }

   pf4tgGi = d4field(pd4Taxgi,"GI");
   pf4tgTaxId = d4field(pd4Taxgi, "TAXID");
   pf4tgType = d4field(pd4Taxgi, "TYPE");
   pf4tgKloodge = d4field(pd4Taxgi, "KLOODGE");

   tagtgGi = d4tag(pd4Taxgi, "GI");
   tagtgTaxId = d4tag(pd4Taxgi, "TAXID");
   tagtgKloodge = d4tag(pd4Taxgi, "KLOODGE");
   MemFree(path);

   return TRUE;
}




Boolean LIBCALL OpenCHROM(CharPtr name, Boolean Create)
{
   Char pathdatab[PATH_MAX];
   CharPtr path = NULL;

   pathdatab[0] = '\0';
   GetAppParam("intrez", "datab", "path", "", pathdatab, (size_t)PATH_MAX * (sizeof(char)));
   path = AllocateAndCreateDBPath(pathdatab, name);
   if(Create == FALSE)
    {
       pd4Chrom = d4open(&cb, path);
       if (pd4Chrom == NULL)
       {
          MemFree(path);
          return FALSE;
	   } 
    }
   else
    {
       pd4Chrom = d4create( &cb, path, f4iChrom, t4iChrom);
       if (cb.errorCode)
       {
          MemFree(path);
          return FALSE;
	   } 
    }


   pf4chTaxId = d4field(pd4Chrom, "TAXID");
   pf4chKloodge = d4field(pd4Chrom, "KLOODGE");
   pf4chChromfl = d4field(pd4Chrom, "CHROMFL");
   tagchTaxId = d4tag(pd4Chrom, "TAXID");
   tagchKloodge = d4tag(pd4Chrom, "KLOODGE");
   tagchChromfl = d4tag(pd4Chrom, "CHROMFL");
   MemFree(path);

   return TRUE;
}



Boolean LIBCALL OpenSENDB(CharPtr name, Boolean Create)
{
   Char pathdatab[PATH_MAX];
   CharPtr path = NULL;

   pathdatab[0] = '\0';
   GetAppParam("intrez", "datab", "pathsen", "", pathdatab, (size_t)PATH_MAX * (sizeof(char)));
   path = AllocateAndCreateDBPath(pathdatab, name);
   if(Create == FALSE)
    {
       pd4Sendb = d4open(&cb, path);
       if (pd4Sendb == NULL)
       {
          MemFree(path);
          return FALSE;
	   } 
    }
   else
    {
       pd4Sendb = d4create( &cb, path, f4iSendb, t4iSendb);
       if (cb.errorCode)
       {
          MemFree(path);
          return FALSE;
	   } 
    }


   pf4sdSeid = d4field(pd4Sendb,"SEID");
   pf4sdAsn1 = d4field(pd4Sendb,"ASN1");
   tagsdSeid = d4tag(pd4Sendb, "SEID");
   MemFree(path);

   return TRUE;
}

Boolean LIBCALL OpenSENGI(CharPtr name, Boolean Create)
{
   Char pathdatab[PATH_MAX];
   CharPtr path = NULL;

   pathdatab[0] = '\0';
   GetAppParam("intrez", "datab", "pathsen", "", pathdatab, (size_t)PATH_MAX * (sizeof(char)));
   path = AllocateAndCreateDBPath(pathdatab, name);
   if(Create == FALSE)
    {
       pd4Sengi = d4open(&cb, path);
       if (pd4Sengi == NULL)
       {
          MemFree(path);
          return FALSE;
	   } 
    }
   else
    {
       pd4Sengi = d4create( &cb, path, f4iSengi, t4iSengi);
       if (cb.errorCode)
       {
          MemFree(path);
          return FALSE;
	   } 
    }


   pf4sgSeid = d4field(pd4Sengi,"SEID");
   pf4sgGi = d4field(pd4Sengi,"GI");
   pf4sgDivision = d4field(pd4Sengi,"DIVISION");
   tagsgGi = d4tag(pd4Sengi, "GI");
   tagsgSeid = d4tag(pd4Sengi, "SEID");
   MemFree(path);

   return TRUE;
}


Boolean LIBCALL OpenMMDB(CharPtr name, Boolean Create)
{
   Char pathdatab[PATH_MAX];
   CharPtr path = NULL;

   pathdatab[0] = '\0';
   GetAppParam("intrez", "datab", "pathmm", "", pathdatab, (size_t)PATH_MAX * (sizeof(char)));
   path = AllocateAndCreateDBPath(pathdatab, name);
   if(Create == FALSE)
    {
       pd4Mmdb = d4open(&cb, path);
       if (pd4Mmdb == NULL)
       {
          MemFree(path);
          return FALSE;
	   } 
    }
   else
    {
       pd4Mmdb = d4create( &cb, path, f4iMmdb, t4iMmdb);
       if (cb.errorCode)
       {
          MemFree(path);
          return FALSE;
	   } 
    }


   pf4mdMmdbId = d4field(pd4Mmdb,"MMDBID");
   pf4mdAsn1 = d4field(pd4Mmdb,"ASN1");
   pf4mdPdbId = d4field(pd4Mmdb,"PDBID");
   pf4mdbWhat = d4field(pd4Mmdb,"BWHAT");
   pf4mdModels = d4field(pd4Mmdb,"MODELS");
   pf4mdMolecules = d4field(pd4Mmdb,"MOLECULES");
   pf4mdSize =  d4field(pd4Mmdb,"SIZE");
   pf4mdBzSize =  d4field(pd4Mmdb,"BZSIZE");
   tagmdMmdbId = d4tag(pd4Mmdb, "MMDBID");
   tagmdPdbId = d4tag(pd4Mmdb, "PDBID");
   MemFree(path);

   return TRUE;
}



Boolean LIBCALL OpenMMGI(CharPtr name, Boolean Create)
{
   Char pathdatab[PATH_MAX];
   CharPtr path = NULL;

   pathdatab[0] = '\0';
   GetAppParam("intrez", "datab", "pathmm", "", pathdatab, (size_t)PATH_MAX * (sizeof(char)));
   path = AllocateAndCreateDBPath(pathdatab, name);
   if(Create == FALSE)
    {
       pd4Mmgi = d4open(&cb, path);
       if (pd4Mmgi == NULL)
       {
          MemFree(path);
          return FALSE;
	   } 
    }
   else
    {
       pd4Mmgi = d4create( &cb, path, f4iMmgi, t4iMmgi);
       if (cb.errorCode)
       {
          MemFree(path);
          return FALSE;
	   } 
    }


   pf4mgMmdbId = d4field(pd4Mmgi,"MMDBID");
   pf4mgGi = d4field(pd4Mmgi,"GI");
   tagmgMmdbId = d4tag(pd4Mmgi, "MMDBID");
   tagmgGi = d4tag(pd4Mmgi, "GI");
   MemFree(path);

   return TRUE;
}


Boolean LIBCALL OpenREDUND(CharPtr name, Boolean Create)
{
   Char pathdatab[PATH_MAX];
   CharPtr path = NULL;

   pathdatab[0] = '\0';
   GetAppParam("intrez", "datab", "path", "", pathdatab, (size_t)PATH_MAX * (sizeof(char)));
   path = AllocateAndCreateDBPath(pathdatab, name);
   if(Create == FALSE)
    {
       pd4Redund = d4open(&cb, path);
       if (pd4Redund == NULL)
       {
          MemFree(path);
          return FALSE;
	   } 
    }
   else
    {
       pd4Redund = d4create( &cb, path, f4iRedund, t4iRedund);
       if (cb.errorCode)
       {
          MemFree(path);
          return FALSE;
	   } 
    }


   pf4rdGi = d4field(pd4Redund,"GI");
   pf4rdOrdinal = d4field(pd4Redund,"ORDINAL");
   pf4rdGroup = d4field(pd4Redund,"GROUP");
   tagrdGi = d4tag(pd4Redund, "GI");
   tagrdOrdinal = d4tag(pd4Redund, "ORDINAL");
   tagrdGroup = d4tag(pd4Redund, "GROUP");
   MemFree(path);

   return TRUE;
}



Boolean LIBCALL OpenDOMDB(CharPtr name, Boolean Create)
{
   Char pathdatab[PATH_MAX];
   CharPtr path = NULL;

   pathdatab[0] = '\0';
   GetAppParam("intrez", "datab", "pathmm", "", pathdatab, (size_t)PATH_MAX * (sizeof(char)));
   path = AllocateAndCreateDBPath(pathdatab, name);
   if(Create == FALSE)
    {
       pd4Domdb = d4open(&cb, path);
       if (pd4Domdb == NULL)
       {
          MemFree(path);
          return FALSE;
	   } 
    }
   else
    {
       pd4Domdb = d4create( &cb, path, f4iDomdb, t4iDomdb);
       if (cb.errorCode)
       {
          MemFree(path);
          return FALSE;
	   } 
    }


   pf4ddMmdbId = d4field(pd4Domdb,"MMDBID");
   pf4ddDomId =  d4field(pd4Domdb,"DOMID");
   pf4ddAsn1 = d4field(pd4Domdb,"ASN1");
   pf4ddPdbId = d4field(pd4Domdb,"PDBID");
   pf4ddChain = d4field(pd4Domdb,"CHAIN");
   pf4ddGi = d4field(pd4Domdb,"GI");
   pf4ddDomno = d4field(pd4Domdb,"DOMNO");
   pf4ddDomall = d4field(pd4Domdb,"DOMALL");
   pf4ddRep = d4field(pd4Domdb,"REP");

   tagddMmdbId = d4tag(pd4Domdb, "MMDBID");
   tagddPdbId = d4tag(pd4Domdb, "PDBID");
   tagddGi = d4tag(pd4Domdb, "GI");
   tagddDomall = d4tag(pd4Domdb, "DOMALL");
   tagddDomId = d4tag(pd4Domdb, "DOMID");
   MemFree(path);

   return TRUE;
}



Boolean LIBCALL OpenVASTDB(CharPtr name, Boolean Create)
{
   Char pathdatab[PATH_MAX];
   CharPtr path = NULL;

   pathdatab[0] = '\0';
   GetAppParam("intrez", "datab", "pathmm", "", pathdatab, (size_t)PATH_MAX * (sizeof(char)));
   path = AllocateAndCreateDBPath(pathdatab, name);
   if(Create == FALSE)
    {
       pd4Vastdb = d4open(&cb, path);
       if (pd4Vastdb == NULL)
       {
          MemFree(path);
          return FALSE;
	   } 
    }
   else
    {
       pd4Vastdb = d4create( &cb, path, f4iVastdb, t4iVastdb);
       if (cb.errorCode)
       {
          MemFree(path);
          return FALSE;
	   }       
    }


   pf4vdMmdbId = d4field(pd4Vastdb,"MMDBID");
   pf4vdAsn1 = d4field(pd4Vastdb,"ASN1");
   pf4vdPdbId = d4field(pd4Vastdb,"PDBID");
   pf4vdChain = d4field(pd4Vastdb,"CHAIN");
   pf4vdVastId1 = d4field(pd4Vastdb, "VASTID1");
   pf4vdVastId2 = d4field(pd4Vastdb, "VASTID2");
   pf4vdScore = d4field(pd4Vastdb, "SCORE");
   pf4vdPval = d4field(pd4Vastdb, "PVAL");
   pf4vdRes = d4field(pd4Vastdb, "RES");
   pf4vdIdent = d4field(pd4Vastdb, "IDENT");
   pf4vdRmsd = d4field(pd4Vastdb, "RMSD");
   pf4vdScale = d4field(pd4Vastdb, "SCALE");


   tagvdMmdbId = d4tag(pd4Vastdb, "MMDBID");
   tagvdPdbId = d4tag(pd4Vastdb, "PDBID");
   tagvdVastId1 = d4tag(pd4Vastdb, "VASTID1");
   MemFree(path);

   return TRUE;
}


Boolean LIBCALL OpenNRBIOSEQ(CharPtr name, Boolean Create)
{
   Char pathdatab[PATH_MAX];
   CharPtr path = NULL;

   pathdatab[0] = '\0';
   GetAppParam("intrez", "datab", "pathmobl", NULL, pathdatab, (size_t)PATH_MAX * (sizeof(Char)));
   path = AllocateAndCreateDBPath(pathdatab, name);
   if(Create == FALSE)
    {
       pd4Nrbioseq = d4open(&cb, path);
       if (pd4Nrbioseq == NULL)
       {
          MemFree(path);
          return FALSE;
	   } 
    }
   else
    {
       pd4Nrbioseq = d4create( &cb, path, f4iNrbioseq, t4iNrbioseq);
       if (cb.errorCode)
       {
          MemFree(path);
          return FALSE;
	   } 
    }

   pf4nbGi = d4field(pd4Nrbioseq,"GI");
   pf4nbAsn1 = d4field(pd4Nrbioseq,"ASN1");
   MemFree(path);

   return TRUE;
}

/*March 2001, Doron- Open Coding Region database*/
Boolean LIBCALL OpenCDDB(CharPtr name, Boolean Create)
{
	Char pathdatab[PATH_MAX];
	CharPtr path = NULL;

	pathdatab[0]='\0';
	/*filling in the missing param*/
	GetAppParam("intrez", "datab", "pathcd", "", pathdatab, (size_t)PATH_MAX*(sizeof(char)));
	path = AllocateAndCreateDBPath(pathdatab, name);
	 /*open database*/
	 if(Create == FALSE)
	 {
	 	pd4Cddb = d4open(&cb, path);
	 	if(pd4Cddb == NULL)
	    {
          MemFree(path);
          return FALSE;
		} 
	 }
	 /*create database*/
	 else{
	 	pd4Cddb= d4create(&cb, path, f4Cddb, t4Cddb);
		if (cb.errorCode)
		{
          MemFree(path);
          return FALSE;
		} 
	}
	pf4cdGi = d4field(pd4Cddb, "GI");
	pf4cdAsn1 = d4field(pd4Cddb, "ASN1");
	tagcdGi = d4tag(pd4Cddb, "GI");
	MemFree(path); 

	return TRUE;
}	




/*************************************/
/*   Print databases                 */
/*************************************/

Boolean LIBCALL PrintASNDB(void)
{
   int rc;
   Int4 gival = 0;
   Int4 ginum = 0;
   char typestr[15];
   char divisionstr[15];
   char releasestr[15];


   for (rc = d4top(pd4Asndb); rc == r4success; rc = d4skip(pd4Asndb, (long) 1))
     {
       ginum++;
       gival = f4long(pf4asGi);
       u4ncpy(typestr, f4str(pf4asType), sizeof(typestr));
       u4ncpy(divisionstr, f4str(pf4asDivision), sizeof(divisionstr));
       u4ncpy(releasestr, f4str(pf4asRelease), sizeof(releasestr));

       printf("%ld\t%s\t%s\t%s\n", (long) gival, IsNullStr(typestr), IsNullStr(divisionstr), IsNullStr(releasestr));
     }
   printf("Number of records: %ld\n", (long) ginum);
   return TRUE;
}


Boolean LIBCALL PrintPARTI(void)
{
   int rc;
   Int4 gival = 0;
   Int4 ginum = 0;
   char partitstr[15];


   for (rc = d4top(pd4Partition); rc == r4success; rc = d4skip(pd4Partition, (long) 1))
     {
       ginum++;
       gival = f4long(pf4paGi);
       u4ncpy(partitstr, f4str(pf4paParti), sizeof(partitstr));
       printf("%ld\t%s\n", (long) gival, IsNullStr(partitstr));
     }
   printf("Number of records: %ld\n", (long) ginum);
   return TRUE;
}


Boolean LIBCALL PrintNUCPROT(void)
{
   int rc;
   Int4 ginval = 0;
   Int4 ginum = 0;
   Int4 giaval = 0;


   for (rc = d4top(pd4Nucprot); rc == r4success; rc = d4skip(pd4Nucprot, (long) 1))
     {
       ginum++;
       ginval = f4long(pf4ncGin);
       giaval = f4long(pf4ncGia);
       printf("%ld   %ld\n", (long) ginval, (long) giaval);
     }
   printf("Number of records: %ld\n", (long) ginum);
   return TRUE;
}


Boolean LIBCALL PrintACCDB(void)
{
   int rc;
   Int4 gival = 0;
   Int2 verval = 0;
   Int4 ginum = 0;
   char db[15];
   char nam[20];
   char namlow[20];
   char acc[20];
   char rel[20];
   char chain[10];
   CharPtr titl = NULL;


   for (rc = d4top(pd4Accdb); rc == r4success; rc = d4skip(pd4Accdb, (long) 1))
     {
       ginum++;
       gival = f4long(pf4acGi);
       verval = f4int(pf4acVersion);
       u4ncpy(db, f4str(pf4acDb), sizeof(db));
       u4ncpy(nam, f4str(pf4acName), sizeof(nam));
       u4ncpy(namlow, f4str(pf4acNamelow), sizeof(namlow));
       u4ncpy(acc, f4str(pf4acAccess), sizeof(acc));
       u4ncpy(chain, f4str(pf4acChain), sizeof(chain));
       u4ncpy(rel, f4str(pf4acRelease), sizeof(rel));
       titl = GetTitlefromACCDB();
       if (strstr(acc, "n/a") != NULL)
          printf("%ld|%s|%s|%s|%s\n", (long) gival, IsNullStr(TrimSpace(db)), IsNullStr(TrimSpace(acc)), IsNullStr(TrimSpace(nam)), IsNullStr(titl));
       else
          printf("%ld|%s|%s.%d|%s|%s\n", (long) gival, IsNullStr(TrimSpace(db)), IsNullStr(TrimSpace(acc)), (int) verval, IsNullStr(TrimSpace(nam)), IsNullStr(titl));
       MemFree(titl);

     }
   printf("Number of records: %ld\n", (long) ginum);
   return TRUE;
}


Boolean LIBCALL PrintPUBSEQ(void)
{
   int rc;
   Int4 gival = 0;
   Int4 ginum = 0;
   Int4 muid=0;


   for (rc = d4top(pd4Pubseq); rc == r4success; rc = d4skip(pd4Pubseq, (long) 1))
     {
       ginum++;
       gival = f4long(pf4psGi);
       muid = f4long(pf4psMuid);
       printf("%ld\t%ld\n", (long) gival, (long) muid);
     }
   printf("Number of records: %ld\n", (long) ginum);
   return TRUE;
}

Boolean LIBCALL PrintTAXGI(void)
{
   int rc;
   Int4 gival = 0;
   Int4 ginum = 0;
   Int4 taxval = 0;
   Int4 klval = 0;
   char type[15];



   for (rc = d4top(pd4Taxgi); rc == r4success; rc = d4skip(pd4Taxgi, (long) 1))
     {
       ginum++;
       gival = f4long(pf4tgGi);
       taxval = f4long(pf4tgTaxId);
       klval = f4long(pf4tgKloodge);
       u4ncpy(type, f4str(pf4tgType), sizeof(type));

       printf("%ld\t%ld\t%ld %s\n", (long) gival, (long) taxval, (long) klval, IsNullStr(type));


     }
   printf("Number of records: %ld\n", (long) ginum);
   return TRUE;
}



Boolean LIBCALL PrintCHROM(void)
{
   Int4 ginum = 0;
   int rc;
   Int4 klval=0;
   Int4 taxval=0;
   Int4 chromval=0;


   for (rc = d4top(pd4Chrom); rc == r4success; rc = d4skip(pd4Chrom, (long) 1))
     {
       ginum++;
       taxval = f4long(pf4chTaxId);
       klval = f4long(pf4chKloodge);
       chromval = f4long(pf4chChromfl);
       printf("%ld\t%ld\t%ld\n", (long) taxval, (long) klval, (long) chromval);
     }
   printf("Number of records: %ld\n", (long) ginum);
   return TRUE;
}



Boolean LIBCALL PrintSENDB(void)
{
   int rc;
   Int4 seidval = 0;
   Int4 ginum = 0;



   for (rc = d4top(pd4Sendb); rc == r4success; rc = d4skip(pd4Sendb, (long) 1))
     {
       ginum++;
       seidval = f4long(pf4sdSeid);
       printf("%ld\n", (long) seidval);
     }
   printf("Number of records: %ld\n", (long) ginum);
   return TRUE;
}

Boolean LIBCALL PrintSENGI(void)
{
   int rc;
   Int4 seidval = 0;
   Int4 gival = 0;
   Int4 ginum = 0;
   char division[10];

   for (rc = d4top(pd4Sengi); rc == r4success; rc = d4skip(pd4Sengi, (long) 1))
     {
       ginum++;
       gival = f4long(pf4sgGi);
       seidval = f4long(pf4sgSeid);
       u4ncpy(division, f4str(pf4sgDivision), sizeof(division));
       printf("%ld\t%ld\t%s\n", (long) gival, (long) seidval, IsNullStr(division));
     }
   printf("Number of records: %ld\n", (long) ginum);
   return TRUE;
}


Boolean LIBCALL PrintMMDB(void)
{
   int rc;
   Int4 ginum = 0;
   Int4 mmdbidval = 0;
   Int4 molval = 0;
   Int4 modval = 0;
   Int4 bwhatval = 0;
   Int4 sizeval = 0;
   Int4 bzsizeval = 0;
   char pdbid[10];



   for (rc = d4top(pd4Mmdb); rc == r4success; rc = d4skip(pd4Mmdb, (long) 1))
     {
       ginum++;
       mmdbidval = f4long(pf4mdMmdbId);
       u4ncpy(pdbid, f4str(pf4mdPdbId), sizeof(pdbid));
       bwhatval = f4long(pf4mdbWhat);
       modval =  f4long(pf4mdModels);
       molval =  f4long(pf4mdMolecules);
       sizeval = f4long(pf4mdSize);
       bzsizeval =  f4long(pf4mdBzSize);
       printf("%ld\t%s\t%ld\t%ld\t%ld\t%ld\t%ld\n", (long) mmdbidval, IsNullStr(pdbid), (long) bwhatval, (long) modval, (long) molval, (long) sizeval, (long) bzsizeval);
     }
   printf("Number of records: %ld\n", (long) ginum);
   return TRUE;
}


Boolean LIBCALL PrintMMGI(void)
{
   int rc;
   Int4 ginum = 0;
   Int4 mmdbidval = 0;
   Int4 gival = 0;


   for (rc = d4top(pd4Mmgi); rc == r4success; rc = d4skip(pd4Mmgi, (long) 1))
     {
       ginum++;
       mmdbidval = f4long(pf4mgMmdbId);
       gival =  f4long(pf4mgGi);
       printf("%ld\t%ld\n", (long) mmdbidval, (long) gival);
     }
   printf("Number of records: %ld\n", (long) ginum);
   return TRUE;
}


Boolean LIBCALL PrintREDUND(void)
{
   int rc;
   Int4 ginum = 0;
   Int4 ordval = 0;
   Int4 groupval = 0;
   Int4 gival = 0;


   for (rc = d4top(pd4Redund); rc == r4success; rc = d4skip(pd4Redund, (long) 1))
     {
       ginum++;
       ordval = f4long(pf4rdOrdinal);
       gival =  f4long(pf4rdGi);
       groupval = f4long(pf4rdGroup);
       printf("%ld\t%ld\t%ld\n", (long) gival, (long) ordval, (long) groupval);
     }
   printf("Number of records: %ld\n", (long) ginum);
   return TRUE;
}


Boolean LIBCALL PrintDOMDB(Byte set)
{
   int rc;
   Int4 ginum = 0;
   Int4 mmdbidval = 0;
   Int4 gival = 0;
   Int4 domnoval = 0;
   char pdbid[10];
   char chain[5];
   Int4 domid = 0;
   Int2 repval = 0;
   Int4 domallval = 0;


   for (rc = d4top(pd4Domdb); rc == r4success; rc = d4skip(pd4Domdb, (long) 1))
     {
       repval = f4int(pf4ddRep);
       if ((set == 0) || ((Byte) repval & set))
        {
          mmdbidval = f4long(pf4ddMmdbId);
          u4ncpy(pdbid, f4str(pf4ddPdbId), sizeof(pdbid));
          u4ncpy(chain, f4str(pf4ddChain), sizeof(chain));
          gival = f4long(pf4ddGi);
          domid = f4long(pf4ddDomId);
          domnoval =  f4long(pf4ddDomno);
          domallval = f4int(pf4ddDomall);
          ginum++;
          printf("%ld\t%s %s\t%ld\t%ld\t%ld\t%d\t%ld\n", (long) mmdbidval, IsNullStr(pdbid), IsNullStr(chain), (long) gival, (long) domnoval, (long) domallval, repval, (long) domid);
        }
     }
   printf("Number of records: %ld\n", (long) ginum);
   return TRUE;
}



Boolean LIBCALL PrintVASTDB(void)
{
   int rc;
   Int4 ginum = 0;
   Int4 mmdbidval = 0;
   char pdbid[10];
   char chain[5];
   Int4 idval1 = 0;
   Int4 idval2 = 0;
   Int4 scoreval = 0;
   Int4 pval = 0;
   Int4 resval = 0;
   Int4 identval = 0;
   Int4 rmsdval = 0;
   Int4 scaleval = 0;

   for (rc = d4top(pd4Vastdb); rc == r4success; rc = d4skip(pd4Vastdb, (long) 1))
     {
       ginum++;
       mmdbidval = f4long(pf4vdMmdbId);
       u4ncpy(pdbid, f4str(pf4vdPdbId), sizeof(pdbid));
       u4ncpy(chain, f4str(pf4vdChain), sizeof(chain));
       idval1 = f4long(pf4vdVastId1);
       idval2 = f4long(pf4vdVastId2);
       scoreval = f4long(pf4vdScore);
       pval = f4long(pf4vdPval);
       resval =  f4long(pf4vdRes);
       identval = f4long(pf4vdIdent);
       rmsdval = f4long(pf4vdRmsd);
       scaleval= f4long(pf4vdScale);
       printf("%ld\t%s %s\t%ld\t%ld\t%ld\t%ld\t%ld\t%ld\t%ld\t%ld\n", (long) mmdbidval, IsNullStr(pdbid), IsNullStr(chain), (long) idval1, (long) idval2, (long) scoreval, (long) pval, (long) resval, (long) identval, (long) rmsdval, (long) scaleval);
     }
   printf("Number of records: %ld\n", (long) ginum);
   return TRUE;
}



Boolean LIBCALL PrintNRBIOSEQ(void)
{
   int rc;
   Int4 ginum = 0;
   Int4 gival =0;

   for (rc = d4top(pd4Nrbioseq); rc == r4success; rc = d4skip(pd4Nrbioseq, (long) 1))
     {
       ginum++;
       gival = f4long(pf4nbGi);

       printf("%ld: %ld\n", (long) ginum, (long) gival);
     }
   printf("Number of records: %ld\n", (long) ginum);
   return TRUE;
}

/*March 2001, Doron- print giCD*/
Boolean LIBCALL PrintCDDB(void)
{
	int rc;
	Int4 ginum = 0;
	Int4 gival = 0;
	for (rc= d4top(pd4Cddb); rc == r4success; rc = d4skip(pd4Cddb, (long)1))
	{
		ginum++;
		gival = f4long(pf4cdGi);
		printf("%ld: %ld\n", (long) ginum, (long) gival);
	}
	printf("Number of records: %ld\n", (long) ginum);
	return TRUE;
}

/***************************************/
/*     Append records to databases     */
/***************************************/

Boolean LIBCALL AppendRecordASNDB(StAsndbPtr pas)
{
   if(pas == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "AppendRecordASNDB: NULL parameter.");
      return FALSE;
    }

   d4appendStart(pd4Asndb, 0);
   d4blank(pd4Asndb);

   if(pas->asn1)
    {
      if (!AssignASNMemMemo((Pointer) pas->asn1, (AsnWriteFunc) BioseqAsnWrite, pf4asAsn1))
        {
          ErrPostEx(SEV_ERROR,0,0, "AppendRecordASNDB: Call to AssignASNMemMemo failed.");
          return FALSE;
         }
    }
   if(pas->gi) f4assignLong(pf4asGi, (long) pas->gi);
   if(pas->type) f4assign(pf4asType, pas->type);
   if(pas->division) f4assign(pf4asDivision, pas->division);
   if(pas->release) f4assign (pf4asRelease, pas->release);

   d4append(pd4Asndb);

   return TRUE;
}


Boolean LIBCALL AppendRecordPARTI(StPartiPtr ppa)
{
   if(ppa == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "AppendRecordPARTI: NULL parameter.");
      return FALSE;
    }
   d4appendStart(pd4Partition, 0);
   d4blank(pd4Partition);

   if(ppa->gi) f4assignLong(pf4paGi, (long) ppa->gi);
   if(ppa->partition) f4assign (pf4paParti, ppa->partition);

   d4append(pd4Partition);
   return TRUE;

}



Boolean LIBCALL AppendRecordNUCPROT(StNucprotPtr pnc)
{
   if(pnc == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "AppendRecordNUCPROT: NULL parameter.");
      return FALSE;
    }

   d4appendStart(pd4Nucprot, 0);
   d4blank(pd4Nucprot);

   if(pnc->gin) f4assignLong(pf4ncGin, (long) pnc->gin);
   if(pnc->gia) f4assignLong(pf4ncGia, (long) pnc->gia);

   d4append(pd4Nucprot);
   return TRUE;
}



Boolean LIBCALL AppendRecordACCDB(StAccdbPtr pac)
{
   Int2 rtn = 0;

   if(pac == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "AppendRecordACCDB: NULL parameter.");
      return FALSE;
    }
   d4appendStart(pd4Accdb, 0);
   d4blank(pd4Accdb);
   if (pac->title != NULL)
    {

        rtn = f4memoAssignN(pf4acTitle, pac->title, (unsigned) strlen(pac->title)+1);
        if (rtn < 0)
         {
            ErrPostEx(SEV_ERROR, 0, 0, "AppendRecordACCDB: f4memoAssign failed.");
            return FALSE;
         }
    }
   if(pac->gi) f4assignLong(pf4acGi, (long) pac->gi);
   if(pac->db) f4assign(pf4acDb, pac->db);
   if(pac->name) f4assign(pf4acName, pac->name);
   if(pac->namelow) f4assign(pf4acNamelow, StrLower(pac->namelow));
   if(pac->access) f4assign(pf4acAccess, pac->access);
   if(pac->release) f4assign(pf4acRelease, pac->release);
   if(pac->version) f4assignLong(pf4acVersion, (long) pac->version);
   if(pac->chain) f4assign(pf4acChain, pac->chain);

   d4append(pd4Accdb);
   return TRUE;
}



Boolean LIBCALL AppendRecordPUBSEQ(StPubseqPtr pps)
{
   if(pps == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "AppendRecordPUBSEQ: NULL parameter.");
      return FALSE;
    }
   d4appendStart(pd4Pubseq, 0);
   d4blank(pd4Pubseq);

   if(pps->gi)f4assignLong(pf4psGi, (long) pps->gi);
   if(pps->muid)f4assignLong(pf4psMuid, (long) pps->muid);

   d4append(pd4Pubseq);
   return TRUE;
}


Boolean LIBCALL AppendRecordTAXGI(StTaxgiPtr ptg)
{
   if(ptg == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "AppendRecordTAXGI: NULL parameter.");
      return FALSE;
    }
   d4appendStart(pd4Taxgi, 0);
   d4blank(pd4Taxgi);

   if(ptg->taxid) f4assignLong(pf4tgTaxId, (long) ptg->taxid);
   if(ptg->gi) f4assignLong(pf4tgGi, (long) ptg->gi);
   if(ptg->type) f4assign(pf4tgType, ptg->type);
   if(ptg->kloodge) f4assignLong(pf4tgKloodge, ptg->kloodge);

   d4append(pd4Taxgi);
   return TRUE;
}


Boolean LIBCALL AppendRecordCHROM(StChromPtr pch)
{
   if(pch == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "AppendRecordCHROM: NULL parameter.");
      return FALSE;
    }
   d4appendStart(pd4Chrom, 0);
   d4blank(pd4Chrom);

   if(pch->taxid) f4assignLong(pf4chTaxId, (long) pch->taxid);
   if(pch->kloodge)f4assignLong(pf4chKloodge, (long) pch->kloodge);
   if(pch->chromfl) f4assignLong(pf4chChromfl, (long) pch->chromfl);

   d4append(pd4Chrom);
   return TRUE;
}


Boolean LIBCALL AppendRecordSENDB(StSendbPtr psd)
{
   if(psd == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "AppendRecordSENDB: NULL parameter.");
      return FALSE;
    }
   d4appendStart(pd4Sendb, 0);
   d4blank(pd4Sendb);

   if(psd->asn1 != NULL)
    {
       if (!AssignASNMemMemo((Pointer) psd->asn1, (AsnWriteFunc) SeqEntryAsnWrite, pf4sdAsn1))
        {
          ErrPostEx(SEV_ERROR,0,0,"ToBioseq: Call to AssignASNMemMemo failed.");
          return FALSE;
        }
    }
   if(psd->seid) f4assignLong(pf4sdSeid, (long) psd->seid);

   d4append(pd4Sendb);
   return TRUE;
}


Boolean LIBCALL AppendRecordSENGI(StSengiPtr psg)
{
   if(psg == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "AppendRecordSENGI: NULL parameter.");
      return FALSE;
    }
   d4appendStart(pd4Sengi, 0);
   d4blank(pd4Sengi);

   if(psg->seid) f4assignLong(pf4sgSeid, (long) psg->seid);
   if(psg->gi) f4assignLong(pf4sgGi, (long) psg->gi);
   if(psg->division) f4assign(pf4sgDivision, psg->division);

   d4append(pd4Sengi);
   return TRUE;
}


Boolean LIBCALL AppendRecordMMDB(StMmdbPtr pmd)
{
   if(pmd == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "AppendRecordMMDB: NULL parameter.");
      return FALSE;
    }
   d4appendStart(pd4Mmdb, 0);
   d4blank(pd4Mmdb);

  if(pmd->asn1 != NULL)
    {
      if (!AssignASNMemBZMemo((Pointer) pmd->asn1, (AsnWriteFunc) BiostrucAsnWrite))
       {
           ErrPostEx(SEV_ERROR,5,1,"AssignASNMemMem failed");
           ClearStructures();
           return FALSE;
       }
    }
   if(pmd->bwhat) f4assignLong(pf4mdbWhat, (long) pmd->bwhat);
   if(pmd->mmdbid) f4assignLong(pf4mdMmdbId, (long) pmd->mmdbid);
   if(pmd->pdbid) f4assign(pf4mdPdbId, pmd->pdbid);
   if(pmd->models) f4assignLong(pf4mdModels, (long) pmd->models);
   if(pmd->molecules) f4assignLong(pf4mdMolecules, (long) pmd->molecules);

   d4append(pd4Mmdb);
   return TRUE;
}


Boolean LIBCALL AppendRecordMMGI(StMmgiPtr pmg)
{
   if(pmg == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "AppendRecordMMGI: NULL parameter.");
      return FALSE;
    }
   d4appendStart(pd4Mmgi, 0);
   d4blank(pd4Mmgi);

   if(pmg->mmdbid) f4assignLong(pf4mgMmdbId, (long) pmg->mmdbid);
   if(pmg->gi) f4assignLong(pf4mgGi, (long) pmg->gi);

   d4append(pd4Mmgi);
   return TRUE;
}


Boolean LIBCALL AppendRecordREDUND(StRedundPtr prd)
{
   if(prd == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "AppendRecordREDUND: NULL parameter.");
      return FALSE;
    }
   d4appendStart(pd4Redund, 0);
   d4blank(pd4Redund);

   if(prd->gi) f4assignLong(pf4rdGi, prd->gi);
   if(prd->ordinal) f4assignLong(pf4rdOrdinal, prd->ordinal);
   if(prd->group) f4assignLong(pf4rdGroup, prd->group);

   d4append(pd4Redund);
   return TRUE;
}


Boolean LIBCALL AppendRecordDOMDB(StDomdbPtr pdd)
{
   if(pdd == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "AppendRecordDOMDB: NULL parameter.");
      return FALSE;
    }
   d4appendStart(pd4Domdb, 0);
   d4blank(pd4Domdb);

   if(pdd->asn1 != NULL)
    {
       if (!AssignASNMemMemo((Pointer) pdd->asn1, (AsnWriteFunc) SLRIValNodeAsnWrite, pf4ddAsn1))
        {
          ErrPostEx(SEV_FATAL,0,0, "Call to AssignASNMemMemo failed.");
          return FALSE;
        }
    }
   if(pdd->mmdbid) f4assignLong(pf4ddMmdbId, (long) pdd->mmdbid);
   if(pdd->domid) f4assignLong(pf4ddDomId, (long) pdd-> domid);
   if(pdd->gi) f4assignLong(pf4ddGi, (long) pdd->gi);
   if(pdd->domno) f4assignLong(pf4ddDomno, (long) pdd->domno);
   if(pdd->domall) f4assignLong(pf4ddDomall, (long) pdd->domall);
   if(pdd->pdbid) f4assign(pf4ddPdbId, pdd->pdbid);
   if(pdd->chain) f4assign(pf4ddChain, pdd->chain);
   if(pdd->rep) f4assignLong(pf4ddRep, (long) pdd->rep);

   d4append(pd4Domdb);
   return TRUE;


}

/*
Boolean LIBCALL AppendRecordVASTDB(StVastdbPtr pvd)
{

   if(pvd == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "AppendRecordVASTDB: NULL parameter.");
      return FALSE;
    }
   d4appendStart(pd4Vastdb, 0);
   d4blank(pd4Vastdb);

   if(pvd->asn1 != NULL)
    {
      if (!AssignASNMemMemo((Pointer) pvd->asn1, (AsnWriteFunc) BiostrucFeatureAsnWrite, pf4vdAsn1))
       {
         ErrPostEx(SEV_FATAL,0,0, "Call to AssignASNMemMemo failed.");
         return FALSE;
       }
    }
   if(pvd->mmdbid) f4assignLong(pf4vdMmdbId, (long) pvd->mmdbid);
   if(pvd->pdbid) f4assign(pf4vdPdbId, pvd->pdbid);
   if(pvd->chain) f4assign(pf4vdChain, pvd->chain);
   if(pvd->vastid1) f4assignLong(pf4vdVastId1, (long) pvd->vastid1);
   if(pvd->vastid2) f4assignLong(pf4vdVastId2, (long) pvd->vastid2);*/  /* has the 1 at the end */
   /*if(pvd->score) f4assignLong(pf4vdScore, (long) pvd->score);
   if(pvd->pval) f4assignLong(pf4vdPval, (long) pvd->pval);
   if(pvd->res) f4assignLong(pf4vdRes, (long) pvd->res);
   if(pvd->ident) f4assignLong(pf4vdIdent, (long) pvd->ident);
   if(pvd->rmsd) f4assignLong(pf4vdRmsd, (long) pvd->rmsd);
   if(pvd->scale) f4assignLong(pf4vdScale, (long) pvd->scale);

   d4append(pd4Vastdb);
   return TRUE;
}
*/

/*March 2001, Doron*
* Append am StCddb record  */

Boolean LIBCALL AppendRecordCDDB(StCddbPtr  pcd)
{
	if( pcd == NULL)
	{
		ErrPostEx(SEV_ERROR, 0, 0, " AppendRecordCDDB: NULL parameter.");
		return FALSE;
	}
	d4appendStart(pd4Cddb, 0);
	d4blank(pd4Cddb);
	
	if( pcd->asn1 !=NULL)
	{
		if(!AssignASNMemMemo((Pointer) pcd->asn1, (AsnWriteFunc) SLRIFastaAsnWrite, pf4cdAsn1))
		{
			ErrPostEx(SEV_ERROR,0,0,"AppendRecordCDDB: Call to AssignASNMemMemo failed.");
			return FALSE;
		}
	}
	if(pcd->gi)
		f4assignLong(pf4cdGi, (long) pcd->gi);
	d4append(pd4Cddb);
	return TRUE;
}



/***************************************/
/*     Delete records in databases     */
/***************************************/
/* if Boolean variable "destroy" is TRUE the record will be blanked out
and GI substituted with -1 if it is FALSE record will be simply deleted */

Boolean LIBCALL DeleteGIinPARTI(Int4 gi, Boolean destroy)
{

   int rc;

   Char div[10];
   Boolean delparti = FALSE;


   d4tagSelect(pd4Partition, tagpaGi);
   d4top(pd4Partition);
   rc = d4seekDouble(pd4Partition, gi);
   if (rc == r4success)
    {

      u4ncpy(div, f4str(pf4paParti), sizeof(div));
      if (destroy)
       {
         d4blank(pd4Partition);
         f4assignLong(pf4paGi, (long) -1);
       }
      d4delete(pd4Partition);
      delparti = TRUE;

      if (!DeleteGIinASNDB(div, gi, destroy))
       {
         /* ErrPostEx(SEV_ERROR, 0,0, "ERROR, DeleteGIinASNDB failed on %ld.", gi); */
         delparti = FALSE;
       }
      for(rc = d4seekNextDouble(pd4Partition, gi); rc == r4success; rc = d4seekNextDouble(pd4Partition, gi))
        {

          u4ncpy(div, f4str(pf4paParti), sizeof(div));
          if (destroy)
           {
             d4blank(pd4Partition);
             f4assignLong(pf4paGi, (long) -1);
           }
          d4delete(pd4Partition);
          delparti = TRUE;

          if (!DeleteGIinASNDB(div, gi, destroy))
           {
             /* ErrPostEx(SEV_ERROR, 0,0, "ERROR, DeleteGIinASNDB failed on %ld.", gi); */
             delparti = FALSE;
           }
        }
     }

    return delparti;
}



Boolean LIBCALL DeleteGIinASNDB(CharPtr div, Int4 gi, Boolean destroy)
{
    int rc;

    Boolean delasndb = FALSE;

    /* SK: NULL ptr check */
    if ( div == NULL )
    {
       ErrPostEx(SEV_ERROR,0,0, "DeleteGIinASNDB: NULL div.");
       return FALSE;
    }

    if (!OpenASNDB(div, FALSE))
     {
       /* SK: missed file name */
       ErrPostEx(SEV_ERROR,0,0, "Cannot open %s.", IsNullStr(div));
       return FALSE;
     }

    d4tagSelect(pd4Asndb, tagasGi);
    d4top(pd4Asndb);
    rc = d4seekDouble(pd4Asndb, gi);
    if (rc == r4success)
     {

       if (destroy)
        {
          d4blank(pd4Asndb);
          f4assignLong(pf4asGi, (long) -1);
        }
       d4delete(pd4Asndb);
       delasndb = TRUE;

       for(rc = d4seekNextDouble(pd4Asndb, gi); rc == r4success; rc = d4seekNextDouble(pd4Asndb, gi))
        {

          if (destroy)
           {
             d4blank(pd4Asndb);
             f4assignLong(pf4asGi, (long) -1);
           }
          d4delete(pd4Asndb);
          delasndb = TRUE;

        }
     }

    CloseASNDB();
    return delasndb;
}


Boolean LIBCALL DeleteGIinACCDB(Int4 gi, Boolean destroy)
{
   int rc;

   Boolean delaccdb = FALSE;


   d4tagSelect(pd4Accdb, tagacGi);
   d4top(pd4Accdb);
   rc = d4seekDouble(pd4Accdb, gi);
   if (rc == r4success)
   {

      if (destroy)
       {
         d4blank(pd4Accdb);
         f4assignLong(pf4acGi, (long) -1);
         f4assign(pf4acAccess, "n/a");
       }
      d4delete(pd4Accdb);
      delaccdb = TRUE;

      for(rc = d4seekNextDouble(pd4Accdb, gi); rc == r4success; rc = d4seekNextDouble(pd4Accdb, gi))
        {

          if (destroy)
           {
             d4blank(pd4Accdb);
             f4assignLong(pf4acGi, (long) -1);
             f4assign(pf4acAccess, "n/a");
           }
          d4delete(pd4Accdb);
          delaccdb = TRUE;

        }
    }

   return delaccdb;
}


Boolean LIBCALL DeleteGIinTAXGI(Int4 gi, Boolean destroy)
{
   int rc;

   Boolean deltaxgi = FALSE;


   d4tagSelect(pd4Taxgi, tagtgGi);
   d4top(pd4Taxgi);
   rc = d4seekDouble(pd4Taxgi, gi);
   if (rc == r4success)
    {

      if (destroy)
       {
         d4blank(pd4Taxgi);
         f4assignLong(pf4tgGi, (long) -1);
       }
      d4delete(pd4Taxgi);
      deltaxgi = TRUE;

      for(rc = d4seekNextDouble(pd4Taxgi, gi); rc == r4success; rc = d4seekNextDouble(pd4Taxgi, gi))
        {

          if (destroy)
           {
             d4blank(pd4Taxgi);
             f4assignLong(pf4tgGi, (long) -1);
           }
          d4delete(pd4Taxgi);
          deltaxgi = TRUE;

        }
    }

   return deltaxgi;
}


Boolean LIBCALL DeleteGIinPUBSEQ(Int4 gi, Boolean destroy)
{
   int rc;

   Boolean delpubseq = FALSE;



   d4tagSelect(pd4Pubseq, tagpsGi);
   d4top(pd4Pubseq);
   rc = d4seekDouble(pd4Pubseq, gi);
   if (rc == r4success)
    {

      if (destroy)
       {
         d4blank(pd4Pubseq);
         f4assignLong(pf4psGi, (long) -1);
       }
      d4delete(pd4Pubseq);
      delpubseq = TRUE;

      for(rc = d4seekNextDouble(pd4Pubseq, gi); rc == r4success; rc = d4seekNextDouble(pd4Pubseq, gi))
        {

          if (destroy)
           {
             d4blank(pd4Pubseq);
             f4assignLong(pf4psGi, (long) -1);
           }
          d4delete(pd4Pubseq);
          delpubseq = TRUE;

        }
    }

   return delpubseq;
}


Boolean LIBCALL DeleteGIinNUCPROT(Int4 gi, Boolean destroy)
{
   int rc;

   Boolean delnucprot = FALSE;


   /* for NUCPROT one has to look through all gins and gias */

   d4tagSelect(pd4Nucprot, tagncGin);
   d4top(pd4Nucprot);
   rc = d4seekDouble(pd4Nucprot, gi);
   if (rc == r4success)
    {

      if (destroy)
       {
         d4blank(pd4Nucprot);
         f4assignLong(pf4ncGin, (long) -1);
       }
      d4delete(pd4Nucprot);
      delnucprot = TRUE;

      for(rc = d4seekNextDouble(pd4Nucprot, gi); rc == r4success; rc = d4seekNextDouble(pd4Nucprot, gi))
        {

          if (destroy)
           {
             d4blank(pd4Nucprot);
             f4assignLong(pf4ncGin, (long) -1);
           }
          d4delete(pd4Nucprot);
          delnucprot = TRUE;

        }
    }



   d4tagSelect(pd4Nucprot, tagncGia);
   d4top(pd4Nucprot);
   rc = d4seekDouble(pd4Nucprot, gi);
   if (rc == r4success)
    {

      if (destroy)
       {
         d4blank(pd4Nucprot);
         f4assignLong(pf4ncGia, (long) -1);
       }
      d4delete(pd4Nucprot);
      delnucprot = TRUE;

      for(rc = d4seekNextDouble(pd4Nucprot, gi); rc == r4success; rc = d4seekNextDouble(pd4Nucprot, gi))
        {

          if (destroy)
           {
             d4blank(pd4Nucprot);
             f4assignLong(pf4ncGia, (long) -1);
           }
          d4delete(pd4Nucprot);
          delnucprot = TRUE;

        }
    }

   return delnucprot;
}


Boolean LIBCALL DeleteGIinSENGI(Int4 gi, Boolean destroy)
{
   int rc;

   Boolean delsengi = FALSE;
   Char divi[11];
   Int4 seidval = 0;


   d4tagSelect(pd4Sengi, tagsgGi);
   d4top(pd4Sengi);
   rc = d4seekDouble(pd4Sengi, gi);
   if (rc == r4success)
    {

      u4ncpy(divi, f4str(pf4sgDivision), sizeof(divi));
      seidval = f4long(pf4sgSeid);
      if (destroy)
       {
         d4blank(pd4Sengi);
         f4assignLong(pf4sgGi, (long) -1);
       }
      d4delete(pd4Sengi);
      delsengi = TRUE;

      if (!DeleteGIinSENDB(divi, seidval, destroy))
       {
         /* ErrPostEx(SEV_ERROR, 0,0, "ERROR, DeleteGIinSENDB failed on %ld.", gi); */
         /* delsengi = FALSE; */
       }
      for(rc = d4seekNextDouble(pd4Sengi, gi); rc == r4success; rc = d4seekNextDouble(pd4Sengi, gi))
        {

          u4ncpy(divi, f4str(pf4sgDivision), sizeof(divi));
          seidval = f4long(pf4sgSeid);
          if (destroy)
           {
             d4blank(pd4Sengi);
             f4assignLong(pf4sgGi, (long) -1);
           }
          d4delete(pd4Sengi);
          delsengi = TRUE;

          if (!DeleteGIinSENDB(divi, seidval, destroy))
           {
             /* ErrPostEx(SEV_ERROR, 0,0, "ERROR, DeleteGIinSENDB failed on %ld.", gi); */
             /* delsengi = FALSE; */
           }
        }
    }

   return delsengi;
}



Boolean LIBCALL DeleteGIinSENDB(CharPtr div, Int4 seid, Boolean destroy)
{
   int rc;

   Boolean delsendb = FALSE;


   if(!OpenSENDB(div, FALSE))
    {
      /* SK: missed file name */
      ErrPostEx(SEV_ERROR,0,0, "Cannot open SENDB database %s.",IsNullStr(div));
      return 0;
    }

   d4tagSelect(pd4Sendb, tagsdSeid);
   d4top(pd4Sendb);
   rc = d4seekDouble(pd4Sendb, seid);
   if (rc == r4success)
    {

      if (destroy)
       {
          d4blank(pd4Sendb);
          f4assignLong(pf4sdSeid, (long) -1);
       }
      d4delete(pd4Sendb);
      delsendb = TRUE;

      for(rc = d4seekNextDouble(pd4Sendb, seid); rc == r4success; rc = d4seekNextDouble(pd4Sendb, seid))
        {

          if (destroy)
           {
             d4blank(pd4Sendb);
             f4assignLong(pf4sdSeid, (long) -1);
           }
          d4delete(pd4Sendb);
          delsendb = TRUE;

        }
    }

   CloseSENDB();
   return delsendb;
}



Boolean LIBCALL DeleteGIinREDUND(Int4 gi, Boolean destroy)
{
   int rc;

   Boolean delredund = FALSE;



   d4tagSelect(pd4Redund, tagrdGi);
   d4top(pd4Redund);
   rc = d4seekDouble(pd4Redund, gi);
   if (rc == r4success)
    {

      if (destroy)
       {
         d4blank(pd4Redund);
         f4assignLong(pf4rdGi, (long) -1);
       }
      d4delete(pd4Redund);
      delredund = TRUE;

      for(rc = d4seekNextDouble(pd4Redund, gi); rc == r4success; rc = d4seekNextDouble(pd4Redund, gi))
        {

           if (destroy)
            {
              d4blank(pd4Redund);
              f4assignLong(pf4rdGi, (long) -1);
            }
           d4delete(pd4Redund);
           delredund = TRUE;

        }
    }

   return delredund;
}


/*March 2001, Doron
* Delete call for removing a CDDB record*/
Boolean LIBCALL DeleteGIinCDDB( Int4 gi, Boolean destroy)
{
	int rc;
	
	Boolean delcd = FALSE;
	
	d4tagSelect( pd4Cddb ,tagcdGi);
	d4top(pd4Cddb);
	rc = d4seekDouble(pd4Cddb, gi);
	if ( rc== r4success)
	{
		if (destroy)
		{
			d4blank(pd4Cddb);
			f4assignLong(pf4cdGi, (long)-1);
		}
		d4delete(pd4Cddb);
		delcd =TRUE;
	}
	return delcd;
}

/***************************************/
/*     Close databases                 */
/***************************************/

Boolean LIBCALL CloseASNDB(void)
{
   if (d4close(pd4Asndb) == r4success)
     return TRUE;
   else
     return FALSE;
}


Boolean LIBCALL ClosePARTI(void)
{
   if (d4close(pd4Partition) == r4success)
     return TRUE;
   else
     return FALSE;
}


Boolean LIBCALL CloseNUCPROT(void)
{
  if (d4close(pd4Nucprot) == r4success)
    return TRUE;
  else
     return FALSE;
}


Boolean LIBCALL CloseACCDB(void)
{
   if (d4close(pd4Accdb) == r4success)
     return TRUE;
   else
     return FALSE;
}


Boolean LIBCALL ClosePUBSEQ(void)
{
   if (d4close(pd4Pubseq) == r4success)
     return TRUE;
   else
     return FALSE;
}


Boolean LIBCALL CloseTAXGI(void)
{
   if (d4close(pd4Taxgi) == r4success)
     return TRUE;
   else
     return FALSE;
}


Boolean LIBCALL CloseCHROM(void)
{
   if (d4close(pd4Chrom) == r4success)
     return TRUE;
   else
     return FALSE;
}


Boolean LIBCALL CloseSENDB(void)
{
   if (d4close(pd4Sendb) == r4success)
     return TRUE;
   else
     return FALSE;
}


Boolean LIBCALL CloseSENGI(void)
{
   if (d4close(pd4Sengi) == r4success)
     return TRUE;
   else
     return FALSE;
}


Boolean LIBCALL CloseMMDB(void)
{
   if (d4close(pd4Mmdb) == r4success)
     return TRUE;
   else
     return FALSE;
}

Boolean LIBCALL CloseMMGI(void)
{
   if (d4close(pd4Mmgi) == r4success)
     return TRUE;
   else
     return FALSE;
}

Boolean LIBCALL CloseREDUND(void)
{
   if (d4close(pd4Redund) == r4success)
     return TRUE;
   else
     return FALSE;
}


Boolean LIBCALL CloseDOMDB(void)
{
   if (d4close(pd4Domdb) == r4success)
     return TRUE;
   else
     return FALSE;
}

Boolean LIBCALL CloseVASTDB(void)
{
   if (d4close(pd4Vastdb) == r4success)
     return TRUE;
   else
     return FALSE;
}


Boolean LIBCALL CloseNRBIOSEQ(void)
{
   if (d4close(pd4Nrbioseq) == r4success)
     return TRUE;
   else
     return FALSE;
}

/*March 2001, Doron- closing CDDB*/
 Boolean LIBCALL CloseCDDB(void)
{
	if(d4close(pd4Cddb) == r4success)
		return TRUE;
	else
		return FALSE;
}

Boolean LIBCALL CloseCodeBase(void)
{
   if ((code4close(&cb) == r4success) && (code4initUndo(&cb) == r4success))
      return TRUE;
   else
      return FALSE;
}


/*************************************/
/* SHound databases search functions */
/*************************************/
Int2 LIBCALL SearchASNDB(StAsndbPtr PNTR ppas)
{
   StAsndbPtr pas = NULL;
   Int4 keynum = 0;
   int rc;
   BioseqPtr pbs = NULL;
   StPartiPtr ppa = NULL;

   /* SK: NULL ptr check */
   if (ppas == NULL || *ppas == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "SearchASNDB: StAsndbPtr is NULL.");
       return 0;
    }
   pas = *ppas;
   if (pas->gi <= 0)
    {
       ErrPostEx(SEV_ERROR,0,0, "SearchASNDB: No allowed search key (GI).");
       return 0;
    }

   keynum = pas->gi;


   /* have to search PARTI and open the right ASNDB database */
   ppa = StPartiNew();
   ppa->gi = keynum;
   if (SearchPARTI(&ppa) == -1)
     return -1;
   if(!OpenASNDB(ppa->partition, FALSE))
    {
      ErrPostEx(SEV_ERROR,0,0, "SearchASNDB: Cannot open %s ASNDB database", IsNullStr(ppa->partition));
      return 0;
    }
   StPartiFree(ppa);

   /* search ASNDB */

   d4tagSelect(pd4Asndb, tagasGi);
   d4top(pd4Asndb);
   rc = d4seekDouble(pd4Asndb, keynum);
   if (rc == r4success)
   {
      /* fill out the StAsndb structure */
      pas->gi = 0;
      pas->gi = f4long(pf4asGi);
      /* get bioseq */
      pbs = GetBioseqfromASNDB();

      if (pbs == NULL)
       {
         ErrPostEx(SEV_ERROR,0,0, "SearchASNDB: Bioseq pointer is NULL.");
         CloseASNDB();
         return 0;
       }

      pas->asn1 = pbs;
      pas->type = TrimSpace(StrSave(f4str(pf4asType)));
      pas->division = TrimSpace(StrSave(f4str(pf4asDivision)));
      pas->release = TrimSpace(StrSave(f4str(pf4asRelease)));
      for(rc = d4seekNextDouble(pd4Asndb, keynum); rc == r4success; rc = d4seekNextDouble(pd4Asndb, keynum))
        {
          /* add and fill out the next StAsndb structure */
          pas->next = StAsndbNew();
          pas = pas->next;
          pas->gi = f4long(pf4asGi);
          pbs = GetBioseqfromASNDB();

          if (pbs == NULL)
           {
             ErrPostEx(SEV_ERROR,0,0, "SearchASNDB: Bioseq pointer is NULL.");
             CloseASNDB();
             return 0;
           }

          pas->asn1 =  pbs;
          pas->type = TrimSpace(StrSave(f4str(pf4asType)));
          pas->division = TrimSpace(StrSave(f4str(pf4asDivision)));
          pas->release = TrimSpace(StrSave(f4str(pf4asRelease)));
        }
      CloseASNDB();
      return 1;
    }
   else
      CloseASNDB();
      return -1;
}


/* Gets ASNDB Asn1 field pointer and returns a bioseq pointer */
BioseqPtr LIBCALL GetBioseqfromASNDB(void)
{
  int iSize = 0;
  CharPtr pcASN = NULL;

  BioseqPtr pbsq = NULL;
  AsnIoMemPtr aimp;


   /* dump out memo field into a file and read it up into memory with BioseqAsnRead */
   iSize = f4memoLen(pf4asAsn1);
   pcASN = (CharPtr) MemNew((size_t) iSize+1);
   if (pcASN == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "GetBioseqfromASNDB: Failed to allocate memory.");
      if (pcASN) MemFree(pcASN);
      return NULL;
    }
   f4memoNcpy(pf4asAsn1, pcASN, (unsigned) iSize);
   aimp = AsnIoMemOpen("rb", (BytePtr) pcASN, iSize);
   if (aimp == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "GetBioseqfromASNDB: Failed to get AsnIo.");
      if (pcASN) MemFree(pcASN);
      return NULL;
    }
   pbsq = BioseqAsnRead(aimp->aip, NULL);
   if (pbsq == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "GetBioseqfromASNDB: Bioseq pointer is NULL.");
      if (pcASN) MemFree(pcASN);
      AsnIoMemClose(aimp);
      return NULL;
    }


   AsnIoFlush(aimp->aip);
   AsnIoMemClose(aimp);
   if (pcASN) MemFree(pcASN);
   return pbsq;
}


Int2 LIBCALL SearchPARTI (StPartiPtr PNTR pppa)
{
   StPartiPtr ppa = NULL;
   Int4 keynum = 0;
   int rc;

   /* SK: NULL ptr check */
   if (pppa == NULL || *pppa == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "SearchPARTI: StPartiPtr is NULL.");
       return 0;
    }
   ppa = *pppa;
   if (ppa->gi <= 0)
    {
       ErrPostEx(SEV_ERROR,0,0, "SearchPARTI: No allowed search key (GI).");
       return 0;
    }

   keynum = ppa->gi;

   d4tagSelect(pd4Partition, tagpaGi);
   d4top(pd4Partition);
   rc = d4seekDouble(pd4Partition, keynum);
   if (rc == r4success)
    {
      ppa->gi = f4long(pf4paGi);
      ppa->partition = TrimSpace(StrSave(f4str(pf4paParti)));
      for(rc = d4seekNextDouble(pd4Partition, keynum); rc == r4success; rc = d4seekNextDouble(pd4Partition, keynum))
        {
          ppa->next = StPartiNew();
          ppa = ppa->next;
          ppa->gi = f4long(pf4paGi);
          ppa->partition = TrimSpace(StrSave(f4str(pf4paParti)));
        }
      return 1;
    }
   else
      return -1;
}

Int2 LIBCALL SearchNUCPROT (StNucprotPtr PNTR ppnc)
{
   StNucprotPtr pnc = NULL;
   Int4 keynum = 0;
   int rc;

   /* SK: NULL ptr check */
   if (ppnc == NULL || *ppnc == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "SearchNUCPROT: StNucprotPtr is NULL.");
       return 0;
    }
   pnc = *ppnc;
   if (pnc->gin > 0)
    {
      keynum = pnc->gin;
      d4tagSelect(pd4Nucprot, tagncGin);
    }
   if (pnc->gia > 0)
    {
      keynum = pnc->gia;
      d4tagSelect(pd4Nucprot, tagncGia);
    }
   if (keynum == 0)
    {
       ErrPostEx(SEV_ERROR,0,0, "SearchNUCPROT: No allowed search key (GI).");
       return 0;
    }
   d4top(pd4Nucprot);
   rc = d4seekDouble(pd4Nucprot, keynum);
   if (rc == r4success)
    {
      pnc->gin = f4long(pf4ncGin);
      pnc->gia = f4long(pf4ncGia);
      for(rc = d4seekNextDouble(pd4Nucprot, keynum); rc == r4success; rc = d4seekNextDouble(pd4Nucprot, keynum))
        {
          pnc->next = StNucprotNew();
          pnc = pnc->next;
          pnc->gin = f4long(pf4ncGin);
          pnc->gia = f4long(pf4ncGia);
        }
      return 1;
    }
   else
      return -1;
}


Int2 LIBCALL SearchACCDB (StAccdbPtr PNTR ppac)
{
   StAccdbPtr pac = NULL;
   Int4 keynum = 0;
   int rc;
   CharPtr key0 = NULL;
   CharPtr key = NULL;

   /* SK: NULL ptr check */
   if (ppac == NULL || *ppac == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "SearchACCDB: StAccdbPtr is NULL.");
       return 0;
    }
   pac = *ppac;
   if ((pac->gi <= 0) && (pac->access == NULL) && (pac->name == NULL) && (pac->namelow == NULL))
    {
       ErrPostEx(SEV_ERROR,0,0, "SearchACCDB: No allowed search key.");
       return 0;
    }

   if (pac->gi > 0)
    {
      keynum = pac->gi;
      d4tagSelect(pd4Accdb, tagacGi);
      d4top(pd4Accdb);
      rc = d4seekDouble(pd4Accdb, keynum);
      if (rc == r4success)
       {
         pac->gi = f4long(pf4acGi);
         pac->version =  f4int(pf4acVersion);
         pac->db = TrimSpace(StrSave(f4str(pf4acDb)));
         pac->name = TrimSpace(StrSave(f4str(pf4acName)));
         pac->namelow = TrimSpace(StrSave(f4str(pf4acNamelow)));
         pac->access = TrimSpace(StrSave(f4str(pf4acAccess)));
         pac->chain = TrimSpace(StrSave(f4str(pf4acChain)));
         pac->release = TrimSpace(StrSave(f4str(pf4acRelease)));
         pac->title = GetTitlefromACCDB();
         for(rc = d4seekNextDouble(pd4Accdb, keynum); rc == r4success; rc = d4seekNextDouble(pd4Accdb, keynum))
           {
             pac->next = StAccdbNew();
             pac = pac->next;
             pac->gi = f4long(pf4acGi);
             pac->version =  f4int(pf4acVersion);
             pac->db = TrimSpace(StrSave(f4str(pf4acDb)));
             pac->name = TrimSpace(StrSave(f4str(pf4acName)));
             pac->namelow = TrimSpace(StrSave(f4str(pf4acNamelow)));
             pac->access = TrimSpace(StrSave(f4str(pf4acAccess)));
             pac->chain = TrimSpace(StrSave(f4str(pf4acChain)));
             pac->release = TrimSpace(StrSave(f4str(pf4acRelease)));
             pac->title = GetTitlefromACCDB();
           }
         return 1;
       }
      else
         return -1;
    }

   if (pac->access != NULL)
    {
      key0 = pac->access;
      d4tagSelect(pd4Accdb, tagacAcc);
     }
   if (pac->name != NULL)
    {
      key0 = pac->name;
      d4tagSelect(pd4Accdb, tagacName);
    }
   if (pac->namelow != NULL)
    {
      key0 = pac->namelow;
      d4tagSelect(pd4Accdb, tagacNamelow);
    }
   if (key0 == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "SearchACCDB: No allowed search key.");
       return 0;
    }

   d4top(pd4Accdb);
   key = PadString(key0);
   rc = d4seekN(pd4Accdb, key, (int) strlen(key));
   if (rc == r4success)
    {
      pac->gi = f4long(pf4acGi);
      pac->version =  f4int(pf4acVersion);
      pac->db = TrimSpace(StrSave(f4str(pf4acDb)));
      pac->name = TrimSpace(StrSave(f4str(pf4acName)));
      pac->namelow = TrimSpace(StrSave(f4str(pf4acNamelow)));
      pac->access = TrimSpace(StrSave(f4str(pf4acAccess)));
      pac->chain = TrimSpace(StrSave(f4str(pf4acChain)));
      pac->release = TrimSpace(StrSave(f4str(pf4acRelease)));
      pac->title = GetTitlefromACCDB();
      for(rc = d4seekNextN(pd4Accdb, key, (int) strlen(key)); rc == r4success; rc = d4seekNextN(pd4Accdb, key, (int) strlen(key)))
        {
          pac->next = StAccdbNew();
          pac = pac->next;
          pac->gi = f4long(pf4acGi);
          pac->version =  f4int(pf4acVersion);
          pac->db = TrimSpace(StrSave(f4str(pf4acDb)));
          pac->name = TrimSpace(StrSave(f4str(pf4acName)));
          pac->namelow = TrimSpace(StrSave(f4str(pf4acNamelow)));
          pac->access = TrimSpace(StrSave(f4str(pf4acAccess)));
          pac->chain = TrimSpace(StrSave(f4str(pf4acChain)));
          pac->release = TrimSpace(StrSave(f4str(pf4acRelease)));
          pac->title = GetTitlefromACCDB();
        }
      MemFree(key);
      return 1;
    }
   else
    {
      MemFree(key);
      return -1;
    }
}

CharPtr LIBCALL  GetTitlefromACCDB(void)
{

   int iSize = 0;
   CharPtr ptitle = NULL, title = NULL;


   iSize = f4memoLen(pf4acTitle);
   ptitle = (CharPtr) MemNew((size_t) iSize+1);
   if (ptitle == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "GetTitlefromACCDB: Failed to allocate memory.");
      if (ptitle) MemFree(ptitle);
      return NULL;
    }
    title = (CharPtr) f4memoStr(pf4acTitle);
   /*
   if(rtn < 0)
    {
      ErrPostEx(SEV_ERROR,0,0, "GetTitlefromACCDB: f4memoNcpy failed.");
      if (ptitle) MemFree(ptitle);
      return NULL;
    }
   */
   ptitle = StringSave(title);
   return ptitle;
}




Int2  LIBCALL SearchPUBSEQ (StPubseqPtr PNTR ppps)
{

   StPubseqPtr pps= NULL;
   Int4 keynum = 0;
   int rc;

   /* SK: NULL ptr check */
   if (ppps == NULL || *ppps == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "SearchPUBSEQ: StPubseqPtr is NULL.");
       return 0;
    }
   pps = *ppps;
   if (pps->gi > 0)
    {
      keynum = pps->gi;
      d4tagSelect(pd4Pubseq, tagpsGi);
    }
   if (pps->muid > 0)
    {
      keynum = pps->muid;
      d4tagSelect(pd4Pubseq, tagpsMuid);
    }
   if (keynum == 0)
    {
       ErrPostEx(SEV_ERROR,0,0, "SearchPUBSEQ: No allowed search key.");
       return 0;
    }

   d4top(pd4Pubseq);
   rc = d4seekDouble(pd4Pubseq, keynum);
   if (rc == r4success)
    {
      pps->gi = f4long(pf4psGi);
      pps->muid = f4long(pf4psMuid);
      for(rc = d4seekNextDouble(pd4Pubseq, keynum); rc == r4success; rc = d4seekNextDouble(pd4Pubseq, keynum))
        {
          pps->next = StPubseqNew();
          pps = pps->next;
          pps->gi = f4long(pf4psGi);
          pps->muid = f4long(pf4psMuid);
        }
      return 1;
    }
   else
      return -1;
}


Int2 LIBCALL SearchTAXGI (StTaxgiPtr PNTR pptg)
{
   StTaxgiPtr ptg = NULL;
   Int4 keynum = 0;
   int rc;

   /* SK: NULL ptr check */
   if (pptg == NULL || *pptg == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "SearchTAXGI: StTaxgiPtr is NULL.");
       return 0;
    }
   ptg = *pptg;
   if ((ptg->gi <= 0) && (ptg->kloodge <= 0) && (ptg->taxid <= 0))
    {
       ErrPostEx(SEV_ERROR,0,0, "SearchTAXGI: No allowed search key.");
       return 0;
    }
   if (ptg->gi > 0)
    {
      keynum = ptg->gi;
      d4tagSelect(pd4Taxgi, tagtgGi);
    }
   if (ptg->kloodge > 0)
    {
      keynum = ptg->kloodge;
      d4tagSelect(pd4Taxgi, tagtgKloodge);
    }
   if (ptg->taxid > 0)
    {
      keynum = ptg->taxid;
      d4tagSelect(pd4Taxgi, tagtgTaxId);
    }
   if (keynum == 0)
    {
       ErrPostEx(SEV_ERROR,0,0, "SearchTAXGI: No allowed search key.");
       return 0;
    }

   d4top(pd4Taxgi);
   rc = d4seekDouble(pd4Taxgi, keynum);
   if (rc == r4success)
    {
      ptg->gi = f4long(pf4tgGi);
      ptg->kloodge = f4long(pf4tgKloodge);
      ptg->taxid = f4long(pf4tgTaxId);
      ptg->type = TrimSpace(StrSave(f4str(pf4tgType)));

      for(rc = d4seekNextDouble(pd4Taxgi, keynum); rc == r4success; rc = d4seekNextDouble(pd4Taxgi, keynum))
        {
          ptg->next = StTaxgiNew();
          ptg = ptg->next;
          ptg->gi = f4long(pf4tgGi);
          ptg->kloodge = f4long(pf4tgKloodge);
          ptg->taxid = f4long(pf4tgTaxId);
          ptg->type = TrimSpace(StrSave(f4str(pf4tgType)));

        }
      return 1;
    }
   else
     return -1;
}


Int2 LIBCALL SearchCHROM (StChromPtr PNTR ppch)
{
   StChromPtr pch = NULL;

   Int4 keynum = 0;
   int rc;

   /* SK: NULL ptr check */
   if (ppch == NULL || *ppch == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "SearchCHROM: StChromPtr is NULL.");
       return 0;
    }

   pch = *ppch;
   if ((pch->chromfl <= 0) && (pch->kloodge <= 0) && (pch->taxid <= 0))
    {
       ErrPostEx(SEV_ERROR,0,0, "SearchCHROM: No allowed search key.");
       return 0;
    }

   if (pch->taxid > 0)
    {
      keynum = pch->taxid;
      d4tagSelect(pd4Chrom, tagchTaxId);
    }
   if (pch->kloodge > 0)
    {
      keynum = pch->kloodge;
      d4tagSelect(pd4Chrom, tagchKloodge);
    }
   if (pch->chromfl > 0)
    {
      keynum = pch->chromfl;
      d4tagSelect(pd4Chrom, tagchChromfl);
    }
   if (keynum == 0)
    {
       ErrPostEx(SEV_ERROR,0,0, "SearchCHROM: No allowed search key.");
       return 0;
    }

   d4top(pd4Chrom);
   rc = d4seekDouble(pd4Chrom, keynum);
   if (rc == r4success)
    {
      pch->kloodge = f4long(pf4chKloodge);
      pch->taxid = f4long(pf4chTaxId);
      pch->chromfl = f4long(pf4chChromfl);
      for(rc = d4seekNextDouble(pd4Chrom, keynum); rc == r4success; rc = d4seekNextDouble(pd4Chrom, keynum))
        {
          pch->next = StChromNew();
          pch = pch->next;
          pch->kloodge = f4long(pf4chKloodge);
          pch->taxid = f4long(pf4chTaxId);
          pch->chromfl = f4long(pf4chChromfl);
        }
      return 1;
    }
   else
      return -1;
}

static Boolean LIBCALL ExpandSeqEntry(SeqEntryPtr PNTR ppse)
{
   BioseqSetPtr pbss = NULL;
   SeqEntryPtr pseNew = NULL, pse = NULL;
   BioseqPtr pbsfull = NULL, pbs = NULL;
   Int4 gival = 0;

   /* SK: NULL ptr check */
  if (ppse == NULL || *ppse == NULL)
  {
     ErrPostEx(SEV_ERROR,0,0,"ExpandSeqEntry: Null pointer to ppse.");
     return FALSE;
  }

   pse = *ppse;
   if (pse->choice == 1) /* It is a Bioseq */
    {
      pbs = (BioseqPtr) pse->data.ptrvalue;
      if (pbs == NULL)
       {
         ErrPostEx(SEV_ERROR,0,0,"ExpandSeqEntry: Null pointer to Bioseq.");
         return FALSE;
       }

      if ((gival = GetGI(pbs)) <= 0)
       {
         ErrPostEx(SEV_ERROR,0,0,"ExpandSeqEntry: Invalid GI.");
         return FALSE;
       }
      /* get the full bioseq */
      pbsfull = SHoundGetBioseq(gival);
      pse->data.ptrvalue = pbsfull;
      /* free the original bioseq*/
      if(pbs)BioseqFree(pbs);

    }
   else if (pse->choice == 2) /* it is nested Bioseq-set */
    {

         pbss = (BioseqSetPtr) pse->data.ptrvalue;
         if (pbss == NULL)
           {
             ErrPostEx(SEV_ERROR,0,0,"ExpandSeqEntry: Null pointer to Bioseq-Set.");
             return FALSE;
           }

         pseNew = pbss->seq_set;
         while (pseNew != NULL)
            {
             if(!ExpandSeqEntry(&pseNew))
               {
                ErrPostEx(SEV_ERROR,0,0,"ExpandSeqEntry: The recursive call to ToBiseqAs failed");
                return FALSE;
               }
              pseNew = pseNew->next;
            }
    }
   else
    {
         ErrPostEx(SEV_ERROR,0,0,"ExpandSeqEntry: Invalid choice field.");
         return FALSE;
    }
   return TRUE;
}




SeqEntryPtr LIBCALL GetSeqEntryfromSENDB(void)
{
   int iSize = 0;
   CharPtr pcASN = NULL;
   int rtn = 0;
   AsnIoMemPtr aimp;
   SeqEntryPtr pse = NULL;

   iSize = f4memoLen(pf4sdAsn1);
    if(iSize<=0){
   	/*post error message*/
   	ErrPostEx(SEV_ERROR, 0,0, "GetSeqEntryfromSENDB: f4memolen returned an empty asn.1 record.");
   	return NULL;
   }

   pcASN = (CharPtr) MemNew((size_t) iSize+1);
   if (pcASN == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "GetSeqEntryfromSENDB: Failed to allocate memory.");
      return NULL;
    }
   rtn = f4memoNcpy(pf4sdAsn1, pcASN, (unsigned) iSize);
   if(rtn<=0){
   	/*post error message*/
   	ErrPostEx(SEV_ERROR,0,0, "GetSeqEntryfromSENDB: f4memoNcopy failed");
   	return NULL;
   }

   aimp = AsnIoMemOpen("rb", (BytePtr) pcASN, iSize);
   if (aimp == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "GetSeqEntryfromSENDB: Failed to get AsnIo.");
      if (pcASN) MemFree(pcASN);
      return NULL;
    }
   pse = SeqEntryAsnRead(aimp->aip, NULL);
   if (pse == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "GetSeqEntryfromSENDB: Bioseq pointer is NULL.");
      if (pcASN) MemFree(pcASN);
      AsnIoMemClose(aimp);
      return NULL;
    }

   AsnIoFlush(aimp->aip);
   AsnIoMemClose(aimp);
   if (pcASN) MemFree(pcASN);
   if(!ExpandSeqEntry(&pse))
    {
      ErrPostEx(SEV_ERROR,0,0, "GetSeqEntryfromSENDB: ExpandSeqEntry failed.");
      return NULL;
    }
   return pse;
}




Int2 LIBCALL SearchSENDB(StSendbPtr PNTR ppsd)
{
   StSendbPtr psd = NULL;
   Int4 keynum = 0;
   int rc;
   SeqEntryPtr pse = NULL;
   StSengiPtr psg = NULL;


   /* SK: NULL ptr check */
   if (ppsd == NULL || *ppsd == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "SearchSENDB: StSendbPtr is NULL.");
       return 0;
    }
   psd = *ppsd;
   if (psd->seid <= 0)
    {
       ErrPostEx(SEV_ERROR,0,0, "SearchSENDB: No allowed search key (SEID).");
       return 0;
    }

   keynum = psd->seid;


   /* have to search SENGI and open the right SENDB database */
   psg = StSengiNew();
   psg->seid = keynum;
   if (SearchSENGI(&psg) == -1)
     return -1;
   if(!OpenSENDB(psg->division, FALSE))
    {
      ErrPostEx(SEV_ERROR,0,0, "SearchSENDB: Cannot open %s SENDB database", IsNullStr(psg->division));
      return 0;
    }
   StSengiFree(psg);

   /* search SENDB */
   d4tagSelect(pd4Sendb, tagsdSeid);
   d4top(pd4Sendb);
   rc = d4seekDouble(pd4Sendb, keynum);
   if (rc == r4success)
   {
      /* fill out the StSendb structure */
      psd->seid = f4long(pf4sdSeid);
      /* get Seq-entry */
      pse = GetSeqEntryfromSENDB();
      if (pse == NULL)
       {
         ErrPostEx(SEV_ERROR,0,0, "SearchSENDB: SeqEntry pointer is NULL.");
         CloseSENDB();
         return 0;
       }
      psd->asn1 = pse;
      for(rc = d4seekNextDouble(pd4Sendb, keynum); rc == r4success; rc = d4seekNextDouble(pd4Sendb, keynum))
        {
          /* add and fill out the next StSendb structure */
          psd->next = StSendbNew();
          psd = psd->next;
          psd->seid = f4long(pf4sdSeid);
          pse = GetSeqEntryfromSENDB();
          if (pse == NULL)
           {
             ErrPostEx(SEV_ERROR,0,0, "SearchSENDB: SeqEntry pointer is NULL.");
             CloseSENDB();
             return 0;
           }
          psd->asn1 = pse;
        }
      CloseSENDB();
      return 1;
    }
   else
      CloseSENDB();
      return -1;
}




Int2  LIBCALL SearchSENGI (StSengiPtr PNTR ppsg)
{

   StSengiPtr psg= NULL;
   Int4 keynum = 0;
   int rc;

   /* SK: NULL ptr check */
   if (ppsg == NULL || *ppsg == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "SearchSENGI: StSengiPtr is NULL.");
       return 0;
    }
   psg = *ppsg;
   if (psg->gi > 0)
    {
      keynum = psg->gi;
      d4tagSelect(pd4Sengi, tagsgGi);
    }
   if (psg->seid > 0)
    {
      keynum = psg->seid;
      d4tagSelect(pd4Sengi, tagsgSeid);
    }
   if (keynum == 0)
    {
       ErrPostEx(SEV_ERROR,0,0, "SearchSENGI: No allowed search key.");
       return 0;
    }

   d4top(pd4Sengi);
   rc = d4seekDouble(pd4Sengi, keynum);
   if (rc == r4success)
    {
      psg->gi = f4long(pf4sgGi);
      psg->seid = f4long(pf4sgSeid);
      psg->division = TrimSpace(StrSave(f4str(pf4sgDivision)));
      for(rc = d4seekNextDouble(pd4Sengi, keynum); rc == r4success; rc = d4seekNextDouble(pd4Sengi, keynum))
        {
          psg->next = StSengiNew();
          psg = psg->next;
          psg->gi = f4long(pf4sgGi);
          psg->seid = f4long(pf4sgSeid);
          psg->division = TrimSpace(StrSave(f4str(pf4sgDivision)));
        }
      return 1;
    }
   else
      return -1;
}


Int2 LIBCALL SearchSENDBforGI(StSendbPtr PNTR ppsd, Int4 gi)
{
   StSendbPtr psd = NULL;
   int rc;
   SeqEntryPtr pse = NULL;
   StSengiPtr psg = NULL;
   Int2 res = 0;
   Int4 keynum = 0;

   /* SK: NULL ptr check */
   if (ppsd == NULL || *ppsd == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "SearchSENDB: StSendbPtr is NULL.");
       return 0;
    }
   psd = *ppsd;

   /* have to search SENGI and open the right SENDB database */
   psg = StSengiNew();
   psg->gi = gi;
   if ((res = SearchSENGI(&psg)) == -1)
     return -1;
   else if (res == 0)
     return 0;
   keynum = psg->seid;
   if(!OpenSENDB(psg->division, FALSE))
    {
      ErrPostEx(SEV_ERROR,0,0, "SearchSENDB: Cannot open %s SENDB database", IsNullStr(psg->division));
      return 0;
    }
   StSengiFree(psg);

   /* search SENDB */
   d4tagSelect(pd4Sendb, tagsdSeid);
   d4top(pd4Sendb);
   rc = d4seekDouble(pd4Sendb, keynum);
   if (rc == r4success)
   {
      /* fill out the StSendb structure */
      psd->seid = f4long(pf4sdSeid);
      /* get Seq-entry */
      pse = GetSeqEntryfromSENDB();
      if (pse == NULL)
       {
         ErrPostEx(SEV_ERROR,0,0, "SearchSENDB: SeqEntry pointer is NULL.");
         CloseSENDB();
         return 0;
       }
      psd->asn1 = pse;
      for(rc = d4seekNextDouble(pd4Sendb, keynum); rc == r4success; rc = d4seekNextDouble(pd4Sendb, keynum))
        {
          /* add and fill out the next StSendb structure */
          psd->next = StSendbNew();
          psd = psd->next;
          psd->seid = f4long(pf4sdSeid);
          pse = GetSeqEntryfromSENDB();
          if (pse == NULL)
           {
             ErrPostEx(SEV_ERROR,0,0, "SearchSENDB: SeqEntry pointer is NULL.");
             CloseSENDB();
             return 0;
           }
          psd->asn1 = pse;
        }
      CloseSENDB();
      return 1;
    }
   else
      CloseSENDB();
      return -1;
}


/* no compression version of the function */
/*
BiostrucPtr  LIBCALL GetBiostrucfromMMDB(void)
{
  Uint4 iSize = 0;
  Int4 iSizebz = 0;
  CharPtr pcASN = NULL;
  CharPtr pcASNbz = NULL;
  int rtn = 0;
  BiostrucPtr pbstr = NULL;
  AsnIoMemPtr aimp;
  Int2 err = 0;


   iSize = f4memoLen(pf4mdAsn1);
   pcASN = (CharPtr) MemNew((size_t) iSize);
   rtn = f4memoNcpy(pf4mdAsn1, pcASN, (unsigned) iSize);
   aimp = AsnIoMemOpen("rb",(BytePtr) pcASN, iSize);
   if (aimp == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "SHoundGet3D: Failed to get AsnIoMem.");
      if (pcASN) MemFree(pcASN);
      return NULL;
    }

   pbstr = BiostrucAsnRead(aimp->aip, NULL);
   if (pbstr == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "SHoundGet3D: Bioseq pointer is NULL.");
      if (pcASN) MemFree(pcASN);
      AsnIoMemClose(aimp);
      return NULL;
    }

   AsnIoFlush(aimp->aip);
   AsnIoMemClose(aimp);
   if (pcASN) MemFree(pcASN);
   return pbstr;
}
*/



BiostrucPtr  LIBCALL GetBiostrucfromMMDB(Int4 mdlLvl, Int4 maxModels)
{
  unsigned int iSize = 0;
  unsigned int iSizebz = 0;
  CharPtr pcASN = NULL;
  BiostrucPtr pbstr = NULL;
  AsnIoMemPtr aimp;



   iSizebz = f4memoLen(pf4mdAsn1);
   iSize =  f4long(pf4mdSize);
   pcASN = (CharPtr) MemNew((size_t) iSize + 1);
   if (pcASN == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "GetBiostrucfromMMDB: Failed to allocate memory.");
      return NULL;
    }
   if (BZ2_bzBuffToBuffDecompress(pcASN, &iSize, f4memoPtr(pf4mdAsn1),iSizebz,0,0)!=BZ_OK)
    {
       ErrPostEx(SEV_ERROR,0,0, "GetBiostrucfromMMDB: bzBuffToBuffDecompress failed.");
       if (pcASN) MemFree(pcASN);
       return NULL;
    }
   aimp = AsnIoMemOpen("rb",(BytePtr) pcASN, iSize);
   if (aimp == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "GetBiostrucfromMMDB: Failed to get AsnIoMem.");
      if (pcASN) MemFree(pcASN);
      return NULL;
    }


   pbstr = BiostrucAsnGet(aimp->aip, NULL, mdlLvl, maxModels);
   if (pbstr == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "GetBiostrucfromMMDB: Biostruc pointer is NULL.");
      if (pcASN) MemFree(pcASN);
      AsnIoMemClose(aimp);
      return NULL;
    }

   AsnIoFlush(aimp->aip);
   AsnIoMemClose(aimp);
   if (pcASN) MemFree(pcASN);
   return pbstr;
}


Int2 LIBCALL SearchMMDB (StMmdbPtr PNTR ppmd)
{
   StMmdbPtr pmd= NULL;
   Int4 keynum = 0;
   int rc;
   CharPtr key0 = NULL, key = NULL;

   /* SK: NULL ptr check */
   if (ppmd == NULL || *ppmd == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "SearchMMDB: StMmdbPtr is NULL.");
       return 0;
    }
   pmd = *ppmd;
   if (pmd->mmdbid > 0)
    {
      keynum = pmd->mmdbid;
      d4tagSelect(pd4Mmdb, tagmdMmdbId);
    }
   if (pmd->pdbid != NULL)
    {
      key0 = pmd->pdbid;
      d4tagSelect(pd4Mmdb, tagmdPdbId);
    }
   if ((keynum == 0) && (key0 == NULL))
    {
       ErrPostEx(SEV_ERROR,0,0, "SearchMMDB: No allowed search key.");
       return 0;
    }
   if (keynum > 0)
    {
      d4top(pd4Mmdb);
      rc = d4seekDouble(pd4Mmdb, keynum);
      if (rc == r4success)
       {
         pmd->mmdbid = f4long(pf4mdMmdbId);
         pmd->asn1 = GetBiostrucfromMMDB(ALLMDL, 100);
         if (pmd->asn1 == NULL)
          {
             ErrPostEx(SEV_ERROR,0,0, "SearchMMDB: GetBiostrucfromMMDB failed.");
             return 0;
          }
         pmd->pdbid = TrimSpace(StrSave(f4str(pf4mdPdbId)));
         pmd->bwhat =  f4long(pf4mdbWhat);
         pmd->models =  f4long(pf4mdModels);
         pmd->molecules = f4long(pf4mdMolecules);
         pmd->size = f4long(pf4mdSize);
         pmd->bzsize  = f4long(pf4mdBzSize);
         for(rc = d4seekNextDouble(pd4Mmdb, keynum); rc == r4success; rc = d4seekNextDouble(pd4Mmdb, keynum))
           {
             pmd->next = StMmdbNew();
             pmd = pmd->next;
             pmd->mmdbid = f4long(pf4mdMmdbId);
             pmd->asn1 = GetBiostrucfromMMDB(ALLMDL, 100);
             if (pmd->asn1 == NULL)
              {
                ErrPostEx(SEV_ERROR,0,0, "SearchMMDB: GetBiostrucfromMMDB failed.");
                return 0;
              }
             pmd->pdbid = TrimSpace(StrSave(f4str(pf4mdPdbId)));
             pmd->bwhat =  f4long(pf4mdbWhat);
             pmd->models =  f4long(pf4mdModels);
             pmd->molecules = f4long(pf4mdMolecules);
             pmd->size = f4long(pf4mdSize);
             pmd->bzsize  = f4long(pf4mdBzSize);
           }
        return 1;
       }
      else
         return -1;

    }
   if (key0 != NULL)
    {
      key = PadString(key0);
      d4top(pd4Mmdb);
      rc = d4seekN(pd4Mmdb, key, (int) strlen(key));
      if (rc == r4success)
       {
         pmd->mmdbid = f4long(pf4mdMmdbId);
         pmd->asn1 = GetBiostrucfromMMDB(ALLMDL, 100);
         if (pmd->asn1 == NULL)
          {
             ErrPostEx(SEV_ERROR,0,0, "SearchMMDB: GetBiostrucfromMMDB failed.");
             return 0;
          }
         pmd->pdbid = TrimSpace(StrSave(f4str(pf4mdPdbId)));
         pmd->bwhat =  f4long(pf4mdbWhat);
         pmd->models =  f4long(pf4mdModels);
         pmd->molecules = f4long(pf4mdMolecules);
         pmd->size = f4long(pf4mdSize);
         pmd->bzsize  = f4long(pf4mdBzSize);
         for(rc = d4seekNextN(pd4Mmdb, key, (int) strlen(key)); rc == r4success; rc = d4seekNextN(pd4Mmdb, key, (int) strlen(key)))
           {
             pmd->next = StMmdbNew();
             pmd = pmd->next;
             pmd->mmdbid = f4long(pf4mdMmdbId);
             pmd->asn1 = GetBiostrucfromMMDB(ALLMDL, 100);
             if (pmd->asn1 == NULL)
              {
                ErrPostEx(SEV_ERROR,0,0, "SearchMMDB: GetBiostrucfromMMDB failed.");
                return 0;
              }
             pmd->pdbid = TrimSpace(StrSave(f4str(pf4mdPdbId)));
             pmd->bwhat =  f4long(pf4mdbWhat);
             pmd->models =  f4long(pf4mdModels);
             pmd->molecules = f4long(pf4mdMolecules);
             pmd->size = f4long(pf4mdSize);
             pmd->bzsize  = f4long(pf4mdBzSize);
           }
         MemFree(key);
         return 1;
       }
      else
       {
         MemFree(key);
         return -1;
       }
    }
  return 0;
}


Int2 LIBCALL SearchMMGI (StMmgiPtr PNTR ppmg)
{
   StMmgiPtr pmg= NULL;
   Int4 keynum = 0;
   int rc;

   /* SK: NULL ptr check */
   if (ppmg == NULL || *ppmg == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "SearchMMGI: StMmgiPtr is NULL.");
       return 0;
    }
   pmg = *ppmg;
   if (pmg->mmdbid > 0)
    {
      keynum = pmg->mmdbid;
      d4tagSelect(pd4Mmgi, tagmgMmdbId);
    }
   if (pmg->gi > 0)
    {
      keynum = pmg->gi;
      d4tagSelect(pd4Mmgi, tagmgGi);
    }
   if (keynum == 0)
    {
       ErrPostEx(SEV_ERROR,0,0, "SearchMMGI: No allowed search key.");
       return 0;
    }

   d4top(pd4Mmgi);
   rc = d4seekDouble(pd4Mmgi, keynum);
   if (rc == r4success)
    {
      pmg->mmdbid = f4long(pf4mgMmdbId);
      pmg->gi = f4long(pf4mgGi);
      for(rc = d4seekNextDouble(pd4Mmgi, keynum); rc == r4success; rc = d4seekNextDouble(pd4Mmgi, keynum))
       {
         pmg->next = StMmgiNew();
         pmg = pmg->next;
         pmg->mmdbid = f4long(pf4mgMmdbId);
         pmg->gi = f4long(pf4mgGi);
       }
      return 1;
    }
   else
      return -1;
}



Int2 LIBCALL SearchDOMDB (StDomdbPtr PNTR ppdd)
{
   StDomdbPtr pdd= NULL;
   Int4 keynum = 0;
   int rc;
   CharPtr key0 = NULL, key = NULL;

   /* SK: NULL ptr check */
   if (ppdd == NULL || *ppdd == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "SearchDOMDB: StDomdbPtr is NULL.");
       return 0;
    }
   pdd = *ppdd;
   if (pdd->domall > 0)
    {
      keynum = pdd->domall;
      d4tagSelect(pd4Domdb, tagddDomall);
    }
   if (pdd->mmdbid > 0)
    {
      keynum = pdd->mmdbid;
      d4tagSelect(pd4Domdb, tagddMmdbId);
    }
   if (pdd->gi > 0)
    {
      keynum = pdd->gi;
      d4tagSelect(pd4Domdb, tagddGi);
    }
   if (pdd->domid > 0)
    {
      keynum = pdd->domid;
      d4tagSelect(pd4Domdb, tagddDomId);
    }


   if (pdd->pdbid != NULL)
    {
      key0 = pdd->pdbid;
      d4tagSelect(pd4Domdb, tagddPdbId);
    }
   if ((keynum == 0) && (key0 == NULL))
    {
       ErrPostEx(SEV_ERROR,0,0, "SearchDOMDB: No allowed search key.");
       return 0;
    }
   if (keynum > 0)
    {
      d4top(pd4Domdb);
      rc = d4seekDouble(pd4Domdb, keynum);
      if (rc == r4success)
       {
         pdd->mmdbid = f4long(pf4ddMmdbId);
         pdd->asn1 = GetDomainfromDOMDB();
         if (pdd->asn1 == NULL)
          {
             ErrPostEx(SEV_ERROR,0,0, "SearchDOMDB: GetDomainfromDOMDB failed.");
             return 0;
          }
         pdd->pdbid = TrimSpace(StrSave(f4str(pf4ddPdbId)));
         pdd->chain = TrimSpace(StrSave(f4str(pf4ddChain)));
         pdd->gi = f4long(pf4ddGi);
         pdd->domno = f4long(pf4ddDomno);
         pdd->domall = f4long(pf4ddDomall);
         pdd->domid =  f4long(pf4ddDomId);
         pdd->rep = f4int(pf4ddRep);
         for(rc = d4seekNextDouble(pd4Domdb, keynum); rc == r4success; rc = d4seekNextDouble(pd4Domdb, keynum))
           {
             pdd->next = StDomdbNew();
             pdd = pdd->next;
             pdd->mmdbid = f4long(pf4ddMmdbId);
             pdd->asn1 = GetDomainfromDOMDB();
             if (pdd->asn1 == NULL)
              {
                 ErrPostEx(SEV_ERROR,0,0, "SearchDOMDB: GetDomainfromDOMDB failed.");
                 return 0;
              }
             pdd->pdbid = TrimSpace(StrSave(f4str(pf4ddPdbId)));
             pdd->chain = TrimSpace(StrSave(f4str(pf4ddChain)));
             pdd->gi = f4long(pf4ddGi);
             pdd->domno = f4long(pf4ddDomno);
             pdd->domall = f4long(pf4ddDomall);
             pdd->domid =  f4long(pf4ddDomId);
             pdd->rep = f4int(pf4ddRep);
           }
        return 1;
       }
      else
         return -1;

    }
   if (key0 != NULL)
    {
      key = PadString(key0);
      d4top(pd4Domdb);
      rc = d4seekN(pd4Domdb, key, (int) strlen(key));
      if (rc == r4success)
       {
         pdd->mmdbid = f4long(pf4ddMmdbId);
         pdd->asn1 = GetDomainfromDOMDB();
         if (pdd->asn1 == NULL)
          {
             ErrPostEx(SEV_ERROR,0,0, "SearchDOMDB: GetDomainfromDOMDB failed.");
             return 0;
          }
         pdd->pdbid = TrimSpace(StrSave(f4str(pf4ddPdbId)));
         pdd->chain = TrimSpace(StrSave(f4str(pf4ddChain)));
         pdd->gi = f4long(pf4ddGi);
         pdd->domno = f4long(pf4ddDomno);
         pdd->domall = f4long(pf4ddDomall);
         pdd->domid =  f4long(pf4ddDomId);
         pdd->rep = f4int(pf4ddRep);
         for(rc = d4seekNextN(pd4Domdb, key, (int) strlen(key)); rc == r4success; rc = d4seekNextN(pd4Domdb, key, (int) strlen(key)))
           {
             pdd->next = StDomdbNew();
             pdd = pdd->next;
             pdd->mmdbid = f4long(pf4ddMmdbId);
             pdd->asn1 = GetDomainfromDOMDB();
             if (pdd->asn1 == NULL)
              {
                 ErrPostEx(SEV_ERROR,0,0, "SearchDOMDB: GetDomainfromDOMDB failed.");
                 return 0;
              }
             pdd->pdbid = TrimSpace(StrSave(f4str(pf4ddPdbId)));
             pdd->chain = TrimSpace(StrSave(f4str(pf4ddChain)));
             pdd->gi = f4long(pf4ddGi);
             pdd->domno = f4long(pf4ddDomno);
             pdd->domall = f4long(pf4ddDomall);
             pdd->domid =  f4long(pf4ddDomId);
             pdd->rep = f4int(pf4ddRep);
           }
         MemFree(key);
         return 1;
       }
      else
       {
         MemFree(key);
         return -1;
       }
    }
  return 0;
}


SLRIValNodePtr LIBCALL GetDomainfromDOMDB(void)
{
  int iSize = 0;
  CharPtr pcASN = NULL;

  SLRIValNodePtr pvndom = NULL;
  AsnIoMemPtr aimp;


   /* dump out memo field into a file and read it up into memory with BioseqAsnRead */
   iSize = f4memoLen(pf4ddAsn1);
   pcASN = (CharPtr) MemNew((size_t) iSize+1);
   if (pcASN == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "GetDomainfromDOMDB: Failed to allocate memory.");
      if (pcASN) MemFree(pcASN);
      return NULL;
    }
   f4memoNcpy(pf4ddAsn1, pcASN, (unsigned) iSize);
   aimp = AsnIoMemOpen("rb", (BytePtr) pcASN, iSize);
   if (aimp == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "GetDomainfromDOMDB: Failed to get AsnIo.");
      if (pcASN) MemFree(pcASN);
      return NULL;
    }
   pvndom = SLRIValNodeAsnRead(aimp->aip, NULL);
   if (pvndom == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "GetDomainfromDOMDB: Bioseq pointer is NULL.");
      if (pcASN) MemFree(pcASN);
      AsnIoMemClose(aimp);
      return NULL;
    }


   AsnIoFlush(aimp->aip);
   AsnIoMemClose(aimp);
   if (pcASN) MemFree(pcASN);
   return pvndom;
}


Int2 LIBCALL SearchREDUND (StRedundPtr PNTR pprd)
{
   StRedundPtr prd= NULL;
   Int4 keynum = 0;
   int rc;

   /* SK: NULL ptr check */
   if (pprd == NULL || *pprd == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "SearchREDUND: StRedundPtr is NULL.");
       return 0;
    }
   prd = *pprd;
   if (prd->gi > 0)
    {
      keynum = prd->gi;
      d4tagSelect(pd4Redund, tagrdGi);
    }
   if (prd->ordinal > 0)
    {
      keynum = prd->ordinal;
      d4tagSelect(pd4Redund, tagrdOrdinal);
    }
   if (prd->group > 0)
    {
      keynum = prd->group;
      d4tagSelect(pd4Redund, tagrdGroup);
    }
   if (keynum == 0)
    {
       ErrPostEx(SEV_ERROR,0,0, "SearchREDUND: No allowed search key.");
       return 0;
    }

   d4top(pd4Redund);
   rc = d4seekDouble(pd4Redund, keynum);
   if (rc == r4success)
    {
      prd->gi = f4long(pf4rdGi);
      prd->ordinal = f4long(pf4rdOrdinal);
      prd->group = f4long(pf4rdGroup);
      for(rc = d4seekNextDouble(pd4Redund, keynum); rc == r4success; rc = d4seekNextDouble(pd4Redund, keynum))
       {
          prd->next = StRedundNew();
          prd = prd->next;
          prd->gi = f4long(pf4rdGi);
          prd->ordinal = f4long(pf4rdOrdinal);
          prd->group = f4long(pf4rdGroup);
       }
      return 1;
    }
   else
      return -1;
}



/*
Int2 LIBCALL SearchVASTDB(StVastdbPtr PNTR ppvd)
{
   StVastdbPtr pvd = NULL;
   Int4 keynum = 0;
   int rc;
   CharPtr key0 = NULL, key = NULL;

   /* SK: NULL ptr check */
   if (ppvd == NULL || *ppvd == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "SearchDOMDB: StDomdbPtr is NULL.");
       return 0;
    }
   pvd = *ppvd;
   if (pvd->mmdbid > 0)
    {
      keynum = pvd->mmdbid;
      d4tagSelect(pd4Vastdb, tagvdMmdbId);
    }
   if (pvd->vastid1 > 0)
    {
      keynum = pvd->vastid1;
      d4tagSelect(pd4Vastdb, tagvdVastId1);
    }


   if (pvd->pdbid != NULL)
    {
      key0 = pvd->pdbid;
      d4tagSelect(pd4Vastdb, tagvdPdbId);
    }
   if ((keynum == 0) && (key0 == NULL))
    {
       ErrPostEx(SEV_ERROR,0,0, "SearchDOMDB: No allowed search key.");
       return 0;
    }
   if (keynum > 0)
    {
      d4top(pd4Vastdb);
      rc = d4seekDouble(pd4Vastdb, keynum);
      if (rc == r4success)
       {
         pvd->mmdbid = f4long(pf4vdMmdbId);
         pvd->asn1 = GetFeaturefromVASTDB();
         if (pvd->asn1 == NULL)
          {
             ErrPostEx(SEV_ERROR,0,0, "SearchVASTDB: GetFeaturefromVASTDB failed.");
             return 0;
          }
         pvd->pdbid = TrimSpace(StrSave(f4str(pf4vdPdbId)));
         pvd->chain = TrimSpace(StrSave(f4str(pf4vdChain)));
         pvd->vastid1 = f4long(pf4vdVastId1);
         pvd->vastid2 = f4long(pf4vdVastId2);
         pvd->score = f4long(pf4vdScore);
         pvd->pval = f4long(pf4vdPval);
         pvd->res =  f4long(pf4vdRes);
         pvd->ident = f4long(pf4vdIdent);
         pvd->rmsd = f4long(pf4vdRmsd);
         pvd->scale = f4long(pf4vdScale);


         for(rc = d4seekNextDouble(pd4Vastdb, keynum); rc == r4success; rc = d4seekNextDouble(pd4Vastdb, keynum))
           {
             pvd->next = StVastdbNew();
             pvd->mmdbid = f4long(pf4vdMmdbId);
             pvd->asn1 = GetFeaturefromVASTDB();
             if (pvd->asn1 == NULL)
              {
                 ErrPostEx(SEV_ERROR,0,0, "SearchVASTDB: GetFeaturefromVASTDB failed.");
                 return 0;
              }
             pvd->pdbid = TrimSpace(StrSave(f4str(pf4vdPdbId)));
             pvd->chain = TrimSpace(StrSave(f4str(pf4vdChain)));
             pvd->vastid1 = f4long(pf4vdVastId1);
             pvd->vastid2 = f4long(pf4vdVastId2);
             pvd->score = f4long(pf4vdScore);
             pvd->pval = f4long(pf4vdPval);
             pvd->res =  f4long(pf4vdRes);
             pvd->ident = f4long(pf4vdIdent);
             pvd->rmsd = f4long(pf4vdRmsd);
             pvd->scale = f4long(pf4vdScale);
           }
        return 1;
       }
      else
         return -1;

    }

   if (key0 != NULL)
    {
      key = PadString(key0);
      d4top(pd4Vastdb);
      rc = d4seekN(pd4Vastdb, key, (int) strlen(key));
      if (rc == r4success)
       {
         pvd->mmdbid = f4long(pf4vdMmdbId);
         pvd->asn1 = GetFeaturefromVASTDB();
         if (pvd->asn1 == NULL)
          {
             ErrPostEx(SEV_ERROR,0,0, "SearchVASTDB: GetFeaturefromVASTDB failed.");
             return 0;
          }
         pvd->pdbid = TrimSpace(StrSave(f4str(pf4vdPdbId)));
         pvd->chain = TrimSpace(StrSave(f4str(pf4vdChain)));
         pvd->vastid1 = f4long(pf4vdVastId1);
         pvd->vastid2 = f4long(pf4vdVastId2);
         pvd->score = f4long(pf4vdScore);
         pvd->pval = f4long(pf4vdPval);
         pvd->res =  f4long(pf4vdRes);
         pvd->ident = f4long(pf4vdIdent);
         pvd->rmsd = f4long(pf4vdRmsd);
         pvd->scale = f4long(pf4vdScale);

         for(rc = d4seekNextN(pd4Vastdb, key, (int) strlen(key)); rc == r4success; rc = d4seekNextN(pd4Vastdb, key, (int) strlen(key)))
           {
             pvd->next = StVastdbNew();
             pvd->mmdbid = f4long(pf4vdMmdbId);
             pvd->asn1 = GetFeaturefromVASTDB();
             if (pvd->asn1 == NULL)
              {
                 ErrPostEx(SEV_ERROR,0,0, "SearchVASTDB: GetFeaturefromVASTDB failed.");
                 return 0;
              }
             pvd->pdbid = TrimSpace(StrSave(f4str(pf4vdPdbId)));
             pvd->chain = TrimSpace(StrSave(f4str(pf4vdChain)));
             pvd->vastid1 = f4long(pf4vdVastId1);
             pvd->vastid2 = f4long(pf4vdVastId2);
             pvd->score = f4long(pf4vdScore);
             pvd->pval = f4long(pf4vdPval);
             pvd->res =  f4long(pf4vdRes);
             pvd->ident = f4long(pf4vdIdent);
             pvd->rmsd = f4long(pf4vdRmsd);
             pvd->scale = f4long(pf4vdScale);
           }
         MemFree(key);
         return 1;
       }
      else
       {
         MemFree(key);
         return -1;
       }
    }
  return 0;
}


BiostrucFeaturePtr LIBCALL GetFeaturefromVASTDB(void)
{
  int iSize = 0;
  CharPtr pcASN = NULL;

  BiostrucFeaturePtr pbf = NULL;
  AsnIoMemPtr aimp;



   iSize = f4memoLen(pf4vdAsn1);
   pcASN = (CharPtr) MemNew((size_t) iSize+1);
   if (pcASN == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "GetFeaturefromVASTDB: Failed to allocate memory.");
      if (pcASN) MemFree(pcASN);
      return NULL;
    }
   f4memoNcpy(pf4vdAsn1, pcASN, (unsigned) iSize);
   aimp = AsnIoMemOpen("rb", (BytePtr) pcASN, iSize);
   if (aimp == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "GetFeaturefromVASTDB: Failed to get AsnIo.");
      if (pcASN) MemFree(pcASN);
      return NULL;
    }
   pbf = BiostrucFeatureAsnRead(aimp->aip, NULL);
   if (pbf == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "GetFeaturefromVASTDB: Bioseq pointer is NULL.");
      if (pcASN) MemFree(pcASN);
      AsnIoMemClose(aimp);
      return NULL;
    }


   AsnIoFlush(aimp->aip);
   AsnIoMemClose(aimp);
   if (pcASN) MemFree(pcASN);
   return pbf;
}*/


/*March 2001, Doron Get SLRIFasta coding region*/
SLRIFastaPtr LIBCALL GetSLRIFastaCDFromCDDB( void)
{
   int iSize = 0;
  CharPtr pcASN = NULL;

  SLRIFastaPtr pfasta = NULL;
  AsnIoMemPtr aimp;


   /* dump out memo field into a file and read it up into memory with BioseqAsnRead */
   iSize = f4memoLen(pf4cdAsn1);
   pcASN = (CharPtr) MemNew((size_t) iSize+1);
   if (pcASN == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "GetSLRIFastaCD: Failed to allocate memory.");
      if (pcASN) MemFree(pcASN);
      return NULL;
    }
   f4memoNcpy(pf4cdAsn1, pcASN, (unsigned) iSize);
   aimp = AsnIoMemOpen("rb", (BytePtr) pcASN, iSize);
   if (aimp == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "GetSLRIFastaCD: Failed to get AsnIo.");
      if (pcASN) MemFree(pcASN);
      return NULL;
    }
   pfasta = SLRIFastaAsnRead(aimp->aip, NULL);
   if (pfasta == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "GetSLRIFastaCD: Bioseq pointer is NULL.");
      if (pcASN) MemFree(pcASN);
      AsnIoMemClose(aimp);
      return NULL;
    }


   AsnIoFlush(aimp->aip);
   AsnIoMemClose(aimp);
   if (pcASN) MemFree(pcASN);
   return pfasta;
}

Int2 LIBCALL SearchCDDB(StCddbPtr PNTR ppcd)
{
   StCddbPtr pcd = NULL;
   Int4 keynum = 0;
   int rc;
   SLRIFastaPtr sfp;

   /* SK: NULL ptr check */
   if (ppcd == NULL || *ppcd == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "SearchCDDB: StCddbPtr is NULL.");
       return 0;
    }
   pcd = *ppcd;
   if (pcd->gi <= 0)
    {
       ErrPostEx(SEV_ERROR,0,0, "SearchCDDB: No allowed search key (GI).");
       return 0;
    }

   keynum = pcd->gi;

   d4tagSelect(pd4Cddb, tagcdGi);
   d4top(pd4Cddb);
   rc = d4seekDouble(pd4Cddb, keynum);
   if (rc == r4success)
   {
      /* fill out the StSendb structure */
      pcd->gi = f4long(pf4cdGi);
      /* get Seq-entry */
      sfp = GetSLRIFastaCDFromCDDB();
      if (sfp == NULL)
       {
         ErrPostEx(SEV_ERROR,0,0, "SearchCDDB: SLRIFasta pointer is NULL.");
         return 0;
       }
      pcd->asn1 = sfp;
      for(rc = d4seekNextDouble(pd4Cddb, keynum); rc == r4success; rc = d4seekNextDouble(pd4Cddb, keynum))
        {
          /* add and fill out the next StSendb structure */
          pcd->next = StCddbNew();
          pcd = pcd->next;
          pcd->gi = f4long(pf4cdGi);
          sfp = GetSLRIFastaCDFromCDDB();
	if (sfp == NULL)
       	{
           ErrPostEx(SEV_ERROR,0,0, "SearchCDDB: SLRIFasta pointer is NULL.");
           return 0;
         }
          pcd->asn1 = sfp;
        }
      return 1;
    }
   else
      return -1;
}

/***********************************************************/
/* ASN.1 blob storing function which uses bzip compression */
/* usable only for storing biostrucs in the MMDB database -*/
/* it writes Biostruc sizes directly into the MMDB         */
/***********************************************************/

Boolean AssignASNMemBZMemo(Pointer pASNdata, AsnWriteFunc pAsnWrite)
{
	AsnIoBSPtr aibp=NULL;
	ByteStorePtr bsp=NULL;
	Int4 size=5000;	
	CharPtr buf = NULL;
        CharPtr bzbuf = NULL;
        unsigned int bsLength = 0;
        unsigned int buffsize = 0;
        Int4 totalwritten = 0;
	int rtn = 0;

	if (pASNdata == NULL) {
		ErrPostEx(SEV_ERROR, 0, 0, "AssignASNMemBZMemo: pASNdata is NULL.");
		return FALSE;
	}
	if (pf4mdAsn1 == NULL) {
		ErrPostEx(SEV_ERROR, 0, 0, "AssignASNMemBZMemo: pf4Field is NULL.");
		return FALSE;
	}
	if (pAsnWrite == NULL) {
		ErrPostEx(SEV_ERROR, 0, 0, "AssignASNMemBZMemo: pAsnWrite is NULL.");
		return FALSE;
	}

	bsp = BSNew(size);
	BSSeek(bsp, 0L, SEEK_SET);
	aibp = AsnIoBSOpen("wb", bsp);
	if (aibp == NULL) {
		ErrPostEx(SEV_ERROR, 0, 0, "AssignASNMemBZMemo: AsnIoBSPtr is NULL.");
		return FALSE;
	}
	if( ! (*pAsnWrite) (pASNdata, aibp->aip, NULL)) {
		AsnIoBSClose(aibp);
		ErrPostEx(SEV_ERROR, 0, 0, "AssignASNMemBZMemo: Asn ByteStore mapped write function failed.");
		BSFree(bsp);
		return FALSE;
	}
	aibp = AsnIoBSClose(aibp);

	BSSeek(bsp, 0L, SEEK_SET);
	bsLength = (unsigned long int) BSLen(bsp);
	buf = (CharPtr)MemNew((size_t) bsLength);
	if(buf == NULL) {
		ErrPostEx(SEV_ERROR, 0, 0, "GetASNMemMemo: Out of memory.");
		BSFree(bsp);
		return FALSE;
	}
	rtn = BSRead(bsp, buf, bsLength);
	buffsize = (unsigned long int) ((FloatLo)bsLength * 1.01 + 50.0);
	bzbuf =  (CharPtr)MemNew(buffsize);
	if(bzbuf == NULL) {
		ErrPostEx(SEV_ERROR, 0, 0, "GetASNMemMemo: Out of memory.");
		BSFree(bsp);
                MemFree(buf);
		return FALSE;
	}
	if (BZ2_bzBuffToBuffCompress(bzbuf, &buffsize, buf, bsLength, 4, 0, 50)!=BZ_OK) {
		ErrPostEx(SEV_ERROR, 0, 0, "GetASNMemMemo: Bzip compression fatal error occurred.");
                BSFree(bsp);
                MemFree(buf);
                MemFree(bzbuf);
		return FALSE;
        }
        totalwritten = buffsize;
	rtn = f4memoAssignN(pf4mdAsn1, bzbuf, totalwritten);
        f4assignLong(pf4mdSize, (long) bsLength);
        f4assignLong(pf4mdBzSize, (long) totalwritten );
	if (rtn < 0) {
		ErrPostEx(SEV_ERROR, 0, 0, "AssignASNMemBZMemo: f4memoAssignN failed.");
		BSFree(bsp);
		MemFree(buf);
                MemFree(bzbuf);
		return FALSE;
	}
	else {
		BSFree(bsp);
		MemFree(buf);
                MemFree(bzbuf);
		return TRUE;
	}
}


/* no compression version of the above function */
/*
Boolean AssignASNMemMemo(Pointer pASNdata, AsnWriteFunc pAsnWrite)
{
	AsnIoBSPtr aibp=NULL;
	ByteStorePtr bsp=NULL;
	Int4 size=5000;	
	CharPtr buf = NULL;
        CharPtr bzbuf = NULL;
	Uint4 bsLength = 0;
        Uint4 buffsize = 0;
        Int4 totalwritten = 0;
	int rtn = 0;
        Int2 err = 0;

	if (pASNdata == NULL) {
		ErrPostEx(SEV_ERROR, 0, 0, "AssignASNMemMemo: pASNdata is NULL.");
		return FALSE;
	}
	if (pf4mdAsn1 == NULL) {
		ErrPostEx(SEV_ERROR, 0, 0, "AssignASNMemMemo: pf4Field is NULL.");
		return FALSE;
	}
	if (pAsnWrite == NULL) {
		ErrPostEx(SEV_ERROR, 0, 0, "AssignASNMemMemo: pAsnWrite is NULL.");
		return FALSE;
	}

	bsp = BSNew(size);
	BSSeek(bsp, 0L, SEEK_SET);
	aibp = AsnIoBSOpen("wb", bsp);
	if (aibp == NULL) {
		ErrPostEx(SEV_ERROR, 0, 0, "AssignASNMemMemo: AsnIoBSPtr is NULL.");
		return FALSE;
	}
	if( ! (*pAsnWrite) (pASNdata, aibp->aip, NULL)) {
		AsnIoBSClose(aibp);
		ErrPostEx(SEV_ERROR, 0, 0, "AssignASNMemMemo: Asn ByteStore mapped write function failed.");
		BSFree(bsp);
		return FALSE;
	}
	aibp = AsnIoBSClose(aibp);

	BSSeek(bsp, 0L, SEEK_SET);
	bsLength = BSLen(bsp);
	buf = (CharPtr)MemNew((size_t) bsLength);
	if(buf == NULL) {
		ErrPostEx(SEV_ERROR, 0, 0, "GetASNMemMemo: Out of memory.");
		BSFree(bsp);
		return FALSE;
	}
	rtn = BSRead(bsp, buf, bsLength);
	
	f4memoAssignN(pf4mdAsn1, buf, bsLength);
        f4assignLong(pf4mdSize, (long) bsLength);

	if (rtn < 0) {
		ErrPostEx(SEV_ERROR, 0, 0, "AssignASNMemMemo: f4memoAssignN failed.");
		BSFree(bsp);
		MemFree(buf);

		return FALSE;
	}
	else {
		BSFree(bsp);
		MemFree(buf);

		return TRUE;
	}
}
*/




/*************************************/
/* Edit record functions - needed for*/
/* some parsers (comgen and pdbrep)  */
/* search key is always in the name  */
/* of the function                   */
/*************************************/

Boolean LIBCALL EditRecordACCDBByGI(StAccdbPtr pac, Boolean overwrite)
{
    int rc;
    StAccdbPtr pactemp = NULL;

    /* SK: NULL ptr check */
    if (pac == NULL || pac->gi <= 0)
     {
       ErrPostEx(SEV_ERROR,0,0, "EditRecordACCDBByGI: Invalid parameters.");
       return FALSE;
     }
    pactemp = StAccdbNew();
    d4tagSelect(pd4Accdb, tagacGi);
    d4top(pd4Accdb);
    rc = d4seekDouble(pd4Accdb, pac->gi);
    if (rc == r4success)
     {
         /* read all stuff in a temporary structure */
         pactemp->version =  f4int(pf4acVersion);
         pactemp->db = TrimSpace(StrSave(f4str(pf4acDb)));
         pactemp->name = TrimSpace(StrSave(f4str(pf4acName)));
         pactemp->namelow = TrimSpace(StrSave(f4str(pf4acNamelow)));
         pactemp->access = TrimSpace(StrSave(f4str(pf4acAccess)));
         pactemp->chain = TrimSpace(StrSave(f4str(pf4acChain)));
         pactemp->release = TrimSpace(StrSave(f4str(pf4acRelease)));
         pactemp->title = GetTitlefromACCDB();

         if(overwrite)  /* replace all values except GI but only it the replacement values are valid*/
          {
             if(pac->db != NULL) f4assign(pf4acDb, pac->db);
             if(pac->name != NULL) f4assign(pf4acName, pac->name);
             if(pac->namelow != NULL) f4assign(pf4acNamelow, pac->namelow);
             if(pac->access != NULL) f4assign(pf4acAccess, pac->access);
             if(pac->release != NULL) f4assign(pf4acRelease, pac->release);
             if(pac->version != 0) f4assignLong(pf4acVersion, (long) pac->version);
             if(pac->chain != NULL) f4assign(pf4acChain, pac->chain);
          }
         else  /* replace only if no value in the database*/
          {
             if((strstr(pactemp->db, "n/a") != NULL) && (pac->db != NULL)) f4assign(pf4acDb, pac->db);
             if((strstr(pactemp->name, "n/a") != NULL) && (pac->name != NULL)) f4assign(pf4acName, pac->name);
             if((strstr(pactemp->namelow, "n/a") != NULL) && (pac->namelow != NULL)) f4assign(pf4acNamelow, pac->namelow);
             if((strstr(pactemp->access, "n/a") != NULL) && (pac->access != NULL)) f4assign(pf4acAccess, pac->access);
             if((strstr(pactemp->release, "n/a") != NULL) && (pac->release != NULL)) f4assign(pf4acRelease, pac->release);
             if((pactemp->version == 0) && (pac->version != 0)) f4assignLong(pf4acVersion, (long) pac->version);
             if((strstr(pactemp->chain, "n/a") != NULL) && (pac->chain != NULL)) f4assign(pf4acChain, pac->chain);
          }

         for (rc = d4seekNextDouble(pd4Accdb, pac->gi); rc == r4success; rc = d4seekNextDouble(pd4Accdb, pac->gi))
           {
                /* free and reallocate */
                pactemp = StAccdbFree(pactemp);
                pactemp = StAccdbNew();

                /* read all stuff in a temporary structure */
                pactemp->version =  f4int(pf4acVersion);
                pactemp->db = TrimSpace(StrSave(f4str(pf4acDb)));
                pactemp->name = TrimSpace(StrSave(f4str(pf4acName)));
                pactemp->namelow = TrimSpace(StrSave(f4str(pf4acNamelow)));
                pactemp->access = TrimSpace(StrSave(f4str(pf4acAccess)));
                pactemp->chain = TrimSpace(StrSave(f4str(pf4acChain)));
                pactemp->release = TrimSpace(StrSave(f4str(pf4acRelease)));
                pactemp->title = GetTitlefromACCDB();

                if(overwrite)
                 {
                    if(pac->db) f4assign(pf4acDb, pac->db);
                    if(pac->name) f4assign(pf4acName, pac->name);
                    if(pac->namelow) f4assign(pf4acNamelow, pac->namelow);
                    if(pac->access) f4assign(pf4acAccess, pac->access);
                    if(pac->release) f4assign(pf4acRelease, pac->release);
                    if(pac->version) f4assignLong(pf4acVersion, (long) pac->version);
                    if(pac->chain) f4assign(pf4acChain, pac->chain);
                 }
                else
                 {
                   if((strstr(pactemp->db, "n/a") != NULL) && (pac->db != NULL)) f4assign(pf4acDb, pac->db);
                   if((strstr(pactemp->name, "n/a") != NULL) && (pac->name != NULL)) f4assign(pf4acName, pac->name);
                   if((strstr(pactemp->namelow, "n/a") != NULL) && (pac->namelow != NULL)) f4assign(pf4acNamelow, pac->namelow);
                   if((strstr(pactemp->access, "n/a") != NULL) && (pac->access != NULL)) f4assign(pf4acAccess, pac->access);
                   if((strstr(pactemp->release, "n/a") != NULL) && (pac->release != NULL)) f4assign(pf4acRelease, pac->release);
                   if((pactemp->version == 0) && (pac->version != 0)) f4assignLong(pf4acVersion, (long) pac->version);
                   if((strstr(pactemp->chain, "n/a") != NULL) && (pac->chain != NULL)) f4assign(pf4acChain, pac->chain);
                 }
           }
     }
    else
       ErrPostEx(SEV_INFO,0,0, "EditRecordACCDBByGI: GI %ld not found in ACCDB database.", (long)pac->gi);


    StAccdbFree(pactemp);
    return TRUE;
}


Boolean LIBCALL EditRecordTAXGIByGI(StTaxgiPtr ptg, Boolean overwrite)
{
    int rc;
    StTaxgiPtr ptgtemp = NULL;

    /* SK: NULL ptr check */
    if(ptg == NULL || ptg->gi == 0)
     {
       ErrPostEx(SEV_ERROR,0,0, "EditRecordTAXGIByGI: Invalid parameters.");
       return FALSE;
     }
    ptgtemp = StTaxgiNew();
    d4tagSelect(pd4Taxgi, tagtgGi);
    d4top(pd4Taxgi);
    rc = d4seekDouble(pd4Taxgi, ptg->gi);
    if (rc == r4success)
     {
       /* read all stuff in a temporary structure */
       ptgtemp->kloodge = f4long(pf4tgKloodge);
       ptgtemp->taxid = f4long(pf4tgTaxId);
       ptgtemp->type = TrimSpace(StrSave(f4str(pf4tgType)));

       if(overwrite)  /* replace all values except GI but only it the replacement values are valid*/
          {
             if(ptg->taxid != 0) f4assignLong(pf4tgTaxId, (long) ptg->taxid);
             if(ptg->kloodge != 0) f4assignLong(pf4tgKloodge, (long) ptg->kloodge);
             if(ptg->type != NULL) f4assign(pf4tgType, ptg->type);
          }
         else  /* replace only if no value in the database*/
          {
             if((ptgtemp->taxid == 0) && (ptg->taxid != 0)) f4assignLong(pf4tgTaxId, (long) ptg->taxid);
             if((ptgtemp->kloodge == 0) && (ptg->kloodge != 0)) f4assignLong(pf4tgKloodge, (long) ptg->kloodge);
             if((ptgtemp->type == NULL) && (ptg->type != NULL)) f4assign(pf4tgType, ptg->type);
          }

       for (rc = d4seekNextDouble(pd4Taxgi, ptg->gi); rc == r4success; rc = d4seekNextDouble(pd4Taxgi, ptg->gi))
        {
          /* free and reallocate */
          ptgtemp = StTaxgiFree(ptgtemp);
          ptgtemp = StTaxgiNew();

          /* read all stuff in a temporary structure */
          ptgtemp->kloodge = f4long(pf4tgKloodge);
          ptgtemp->taxid = f4long(pf4tgTaxId);
          ptgtemp->type = TrimSpace(StrSave(f4str(pf4tgType)));

          if(overwrite)  /* replace all values except GI but only it the replacement values are valid*/
             {
                if(ptg->taxid != 0) f4assignLong(pf4tgTaxId, (long) ptg->taxid);
                if(ptg->kloodge != 0) f4assignLong(pf4tgKloodge, (long) ptg->kloodge);
                if(ptg->type != NULL) f4assign(pf4tgType, ptg->type);
             }
            else  /* replace only if no value in the database*/
             {
                if((ptgtemp->taxid == 0) && (ptg->taxid != 0)) f4assignLong(pf4tgTaxId, (long) ptg->taxid);
                if((ptgtemp->kloodge == 0) && (ptg->kloodge != 0)) f4assignLong(pf4tgKloodge, (long) ptg->kloodge);
                if((ptgtemp->type == NULL) && (ptg->type != NULL)) f4assign(pf4tgType, ptg->type);
             }
        }
     }
    else
      ErrPostEx(SEV_INFO,0,0, "EditRecordTAXGIByGI: GI %ld not found in ACCDB database.", (long) ptg->gi);

    StTaxgiFree(ptgtemp);
    return TRUE;
}

Boolean LIBCALL EditRecordDOMDBByDOMID(StDomdbPtr pdd, Boolean overwrite)
{
  int rc;
  StDomdbPtr pddtemp = NULL;

  /* SK: NULL ptr check */
  if(pdd == NULL || pdd->domid == 0)
   {
      ErrPostEx(SEV_ERROR,0,0, "EditRecordDOMDBByDOMID: Invalid parameters.");
      return FALSE;
   }
  pddtemp = StDomdbNew();
  d4tagSelect(pd4Domdb, tagddDomId);
  d4top(pd4Domdb);
  rc = d4seekDouble(pd4Domdb, pdd->domid);
  if (rc == r4success)
   {
     /* read to temporary structure */
     pddtemp->mmdbid = f4long(pf4ddMmdbId);
     pddtemp->asn1 = GetDomainfromDOMDB();
     if (pddtemp->asn1 == NULL)
      {
         ErrPostEx(SEV_ERROR,0,0, "EditRecordDOMDBByDOMID: GetDomainfromDOMDB failed.");
         return FALSE;
      }
     pddtemp->pdbid = TrimSpace(StrSave(f4str(pf4ddPdbId)));
     pddtemp->chain = TrimSpace(StrSave(f4str(pf4ddChain)));
     pddtemp->gi = f4long(pf4ddGi);
     pddtemp->domno = f4long(pf4ddDomno);
     pddtemp->domall = f4long(pf4ddDomall);
     pddtemp->rep = f4int(pf4ddRep);

     if(overwrite) /* replace all values except DOMDID but only it the replacement values are valid*/
      {
        if(pdd->asn1 != NULL)
         {
            if (!AssignASNMemMemo((Pointer) pdd->asn1, (AsnWriteFunc) SLRIValNodeAsnWrite, pf4ddAsn1))
             {
               ErrPostEx(SEV_FATAL,0,0, "Call to AssignASNMemMemo failed.");
               return FALSE;
             }
         }
        if(pdd->mmdbid != 0) f4assignLong(pf4ddMmdbId, (long) pdd->mmdbid);
        if(pdd->gi != 0) f4assignLong(pf4ddGi, (long) pdd->gi);
        if(pdd->domno != 0) f4assignLong(pf4ddDomno, (long) pdd->domno);
        if(pdd->domall != 0) f4assignLong(pf4ddDomall, (long) pdd->domall);
        if(pdd->pdbid != NULL) f4assign(pf4ddPdbId, pdd->pdbid);
        if(pdd->chain != NULL) f4assign(pf4ddChain, pdd->chain);
        if(pdd->rep != 0) f4assignLong(pf4ddRep, (long) pdd->rep);
      }
     else /* replace only if no value in the database*/
      {
        if((pddtemp == NULL) && (pdd->asn1 != NULL))
         {
            if (!AssignASNMemMemo((Pointer) pdd->asn1, (AsnWriteFunc) SLRIValNodeAsnWrite, pf4ddAsn1))
             {
               ErrPostEx(SEV_FATAL,0,0, "Call to AssignASNMemMemo failed.");
               return FALSE;
             }
         }
        if((pddtemp->mmdbid == 0) && (pdd->mmdbid != 0)) f4assignLong(pf4ddMmdbId, (long) pdd->mmdbid);
        if((pddtemp->gi == 0) && (pdd->gi != 0)) f4assignLong(pf4ddGi, (long) pdd->gi);
        if((pddtemp->domno == 0) && (pdd->domno != 0)) f4assignLong(pf4ddDomno, (long) pdd->domno);
        if((pddtemp->domall == 0) && (pdd->domall != 0)) f4assignLong(pf4ddDomall, (long) pdd->domall);
        if((pddtemp->pdbid == NULL) && (pdd->pdbid != NULL)) f4assign(pf4ddPdbId, pdd->pdbid);
        if((pddtemp->chain == NULL) && (pdd->chain != NULL)) f4assign(pf4ddChain, pdd->chain);
        if((pddtemp->rep == 0) && (pdd->rep != 0)) f4assignLong(pf4ddRep, (long) pdd->rep);
      }


     for(rc = d4seekNextDouble(pd4Domdb, pdd->domid); rc == r4success; rc = d4seekNextDouble(pd4Domdb, pdd->domid))
       {
          /* free and reallocate */
          pddtemp = StDomdbFree(pddtemp);
          pddtemp = StDomdbNew();

          /*read to temporary structure */
          pddtemp->mmdbid = f4long(pf4ddMmdbId);
          pddtemp->asn1 = GetDomainfromDOMDB();
          if (pddtemp->asn1 == NULL)
           {
              ErrPostEx(SEV_ERROR,0,0, "EditRecordDOMDBByDOMID: GetDomainfromDOMDB failed.");
              return FALSE;
           }
          pddtemp->pdbid = TrimSpace(StrSave(f4str(pf4ddPdbId)));
          pddtemp->chain = TrimSpace(StrSave(f4str(pf4ddChain)));
          pddtemp->gi = f4long(pf4ddGi);
          pddtemp->domno = f4long(pf4ddDomno);
          pddtemp->domall = f4long(pf4ddDomall);
          pddtemp->domid =  f4long(pf4ddDomId);
          pddtemp->rep = f4int(pf4ddRep);

          if(overwrite) /* replace all values except DOMDID but only it the replacement values are valid*/
           {
             if(pdd->asn1 != NULL)
              {
                 if (!AssignASNMemMemo((Pointer) pdd->asn1, (AsnWriteFunc) SLRIValNodeAsnWrite, pf4ddAsn1))
                  {
                    ErrPostEx(SEV_FATAL,0,0, "EditRecordDOMDBByDOMID:Call to AssignASNMemMemo failed.");
                    return FALSE;
                  }
              }
             if(pdd->mmdbid != 0) f4assignLong(pf4ddMmdbId, (long) pdd->mmdbid);
             if(pdd->gi != 0) f4assignLong(pf4ddGi, (long) pdd->gi);
             if(pdd->domno != 0) f4assignLong(pf4ddDomno, (long) pdd->domno);
             if(pdd->domall != 0) f4assignLong(pf4ddDomall, (long) pdd->domall);
             if(pdd->pdbid != NULL) f4assign(pf4ddPdbId, pdd->pdbid);
             if(pdd->chain != NULL) f4assign(pf4ddChain, pdd->chain);
             if(pdd->rep != 0) f4assignLong(pf4ddRep, (long) pdd->rep);

           }
          else /* replace only if no value in the database*/
           {
             if((pddtemp == NULL) && (pdd->asn1 != NULL))
              {
                 if (!AssignASNMemMemo((Pointer) pdd->asn1, (AsnWriteFunc) SLRIValNodeAsnWrite, pf4ddAsn1))
                  {
                    ErrPostEx(SEV_FATAL,0,0, "EditRecordDOMDBByDOMID:Call to AssignASNMemMemo failed.");
                    return FALSE;
                  }
              }
             if((pddtemp->mmdbid == 0) && (pdd->mmdbid != 0)) f4assignLong(pf4ddMmdbId, (long) pdd->mmdbid);
             if((pddtemp->gi == 0) && (pdd->gi != 0)) f4assignLong(pf4ddGi, (long) pdd->gi);
             if((pddtemp->domno == 0) && (pdd->domno != 0)) f4assignLong(pf4ddDomno, (long) pdd->domno);
             if((pddtemp->domall == 0) && (pdd->domall != 0)) f4assignLong(pf4ddDomall, (long) pdd->domall);
             if((pddtemp->pdbid == NULL) && (pdd->pdbid != NULL)) f4assign(pf4ddPdbId, pdd->pdbid);
             if((pddtemp->chain == NULL) && (pdd->chain != NULL)) f4assign(pf4ddChain, pdd->chain);
             if((pddtemp->rep == 0) && (pdd->rep != 0)) f4assignLong(pf4ddRep, (long) pdd->rep);
           }
       }
   }
  else
      ErrPostEx(SEV_INFO,0,0, "EditRecordDOMDBByDOMID: MMDB ID %ld not found in DOMDB database.", (long)pdd->mmdbid);

   StDomdbFree(pddtemp);
   return TRUE;
}


/*************************************/
/* Specialized database API functions*/
/* used in fastadom, redund, sendb   */
/* and precompute                    */
/*************************************/

/* returns record number */
Int4 LIBCALL SHoundRecNoDOMDB(void)
{

  return d4recCount(pd4Domdb);

}

/* returns record number */
Int4 LIBCALL SHoundRecNoREDUND(void)
{

  return d4recCount(pd4Redund);

}


/* returns record number */
Int4 LIBCALL SHoundRecNoSENGI(void)
{

  return d4recCount(pd4Sengi);

}


StRedundPtr LIBCALL SHoundGetRecREDUND(Int4 recno)
{

  StRedundPtr prd = NULL;

  if((recno == 0) || (recno > SHoundRecNoREDUND()))
    {
       ErrPostEx(SEV_ERROR,0,0, "SHoundGetRecDOMDB: Invalid parameters.");
       return 0;
    }

  prd = StRedundNew();
  d4go(pd4Redund, recno);
  prd->gi = f4long(pf4rdGi);
  prd->ordinal = f4long(pf4rdOrdinal);

  return prd;
}


/* returns a record based on record number */
StDomdbPtr LIBCALL SHoundGetRecDOMDB(Int4 recno)
{
   StDomdbPtr pdd = NULL;

   if((recno == 0) || (recno > SHoundRecNoDOMDB()))
    {
       ErrPostEx(SEV_ERROR,0,0, "SHoundGetRecDOMDB: Invalid parameters.");
       return 0;
    }

   pdd = StDomdbNew();

   d4go(pd4Domdb, recno);
   pdd->mmdbid = f4long(pf4ddMmdbId);
   pdd->asn1 = GetDomainfromDOMDB();
   if (pdd->asn1 == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "SHoundGetRecDOMDB: GetDomainfromDOMDB failed.");
       return 0;
    }
   pdd->pdbid = TrimSpace(StrSave(f4str(pf4ddPdbId)));
   pdd->chain = TrimSpace(StrSave(f4str(pf4ddChain)));
   pdd->gi = f4long(pf4ddGi);
   pdd->domno = f4long(pf4ddDomno);
   pdd->domall = f4long(pf4ddDomall);
   pdd->domid =  f4long(pf4ddDomId);
   pdd->rep = f4int(pf4ddRep);

   return pdd;

}

/* returns TRUE if the GI belongs to the specified division -
will not work for updates (is only used for primates in precomputed)*/
Boolean LIBCALL IsThisDivision(Int4 gi, CharPtr division)
{
   Int4 rc = 0;

   if((gi <= 0) || (division == NULL))
   {
     ErrPostEx(SEV_ERROR,0,0, "IsThisDivision: Invalid parameter.");
     return FALSE;
   }
   d4tagSelect(pd4Partition, tagpaGi);
   rc = d4seekDouble(pd4Partition, gi);
   if (rc == r4success)
   {
	  if(strstr(f4ptr(pf4paParti), division) != NULL)		
	    return TRUE;
	  else
            return FALSE;
   }
   else
      return FALSE;
}


/*find the absolute largest taxid in the database */
Int4 LIBCALL GetLargestTaxid(void)
{
   /* jump to the bottom and find out what id it is*/
    d4tagSelect(pd4Taxgi, tagtgTaxId);
	d4bottom(pd4Taxgi);
	return (f4long(pf4tgTaxId));
}	


/*Given the taxid we are interested in and a Posting
*  to store
*  Return : the number of gis placed into the
*  Posting and fill in all the gis associated with that
*  taxid into the posting.
*/
Int4 LIBCALL getTaxInfo(Int4 taxid, PostingPtr* psp, CharPtr type)
{
	char expr[20];
	Int2 rc = 0;
	Int4 gival = 0, count = 0;
	char tax_type[15];
	RELATE4 * relation = NULL;
	
	if((relation = relate4init(pd4Taxgi)) == NULL){
		ErrPostEx(SEV_ERROR,0,0, "No info for %ld id.", (long) taxid);
		return 0;
	}

	sprintf(expr, "taxid = %ld", (long) taxid);
	relate4querySet(relation, expr);

	relate4optimizeable(relation);

	for(rc = relate4top(relation); rc != r4eof; rc=relate4skip(relation, 1L)){
		gival = f4long(pf4tgGi);
		u4ncpy(tax_type, f4str(pf4tgType), sizeof(tax_type));
		if(strstr(tax_type, type) != NULL){
			WriteItem(*psp,gival);
			count++;
		}
	}

	FlushItems(*psp);

	relate4free(relation, 0);
	return count;
}

Int4 LIBCALL getTaxInfoII(Int4 taxid, PostingPtr* psp_all,  PostingPtr* psp_na, PostingPtr* psp_prot, Int4 *num_all, Int4 *num_na, Int4 *num_prot)
{
	char expr[20];
	Int2 rc = 0;
	Int4 gival = 0, count = 0;
	RELATE4 * relation = NULL;
	
	if((relation = relate4init(pd4Taxgi)) == NULL){
		ErrPostEx(SEV_ERROR,0,0, "No info for %ld id.", (long) taxid);
		return 0;
	}

	
	sprintf(expr, "taxid = %ld", (long) taxid);
	relate4querySet(relation, expr);

	relate4optimizeable(relation);

	for(rc = relate4top(relation); rc != r4eof; rc=relate4skip(relation, 1L))
	{
		gival = f4long(pf4tgGi);
		WriteItem(*psp_all,gival);
		count++;
		(*num_all)++;
		if(strstr(f4ptr(pf4tgType), "protein") != NULL)
		{
			WriteItem(*psp_prot,gival);
			(*num_prot)++;
		}	
			
		else if(strstr(f4ptr(pf4tgType), "NA") != NULL)
		{
			WriteItem(*psp_na,gival);
			(*num_na)++;
		}	
	}

	FlushItems(*psp_all);
    FlushItems(*psp_na);
    FlushItems(*psp_prot);
	
	relate4free(relation, 0);
	return count;
}


Int4 LIBCALL getTaxInfoIII(Int4 taxid, PostingPtr* psp_all,  PostingPtr* psp_na, PostingPtr* psp_prot, Int4 *num_all, Int4 *num_na, Int4 *num_prot)
{
	Int2 rc = 0;
	Int4 gival = 0, count = 0;
	
	d4tagSelect(pd4Taxgi, tagtgTaxId);
    d4top(pd4Taxgi);
    rc = d4seekDouble(pd4Taxgi, taxid);
	if (rc == r4success)
    {
        gival = f4long(pf4tgGi);
		WriteItem(*psp_all,gival);
		count++;
		(*num_all)++;
		if(strstr(f4ptr(pf4tgType), "protein") != NULL)
		{
			WriteItem(*psp_prot,gival);
			(*num_prot)++;
		}	
			
		else if(strstr(f4ptr(pf4tgType), "NA") != NULL)
		{
			WriteItem(*psp_na,gival);
			(*num_na)++;
		}	

    	for(rc = d4seekNextDouble(pd4Taxgi, taxid); rc == r4success; rc = d4seekNextDouble(pd4Taxgi, taxid))
    	{
    		gival = f4long(pf4tgGi);
    		WriteItem(*psp_all,gival);
    		count++;
    		(*num_all)++;
    		if(strstr(f4ptr(pf4tgType), "protein") != NULL)
    		{
    			WriteItem(*psp_prot,gival);
    			(*num_prot)++;
    		}	
    			
    		else if(strstr(f4ptr(pf4tgType), "NA") != NULL)
    		{
    			WriteItem(*psp_na,gival);
    			(*num_na)++;
    		}	
    	}
    }
	FlushItems(*psp_all);
    FlushItems(*psp_na);
    FlushItems(*psp_prot);
	
	return count;
}
/*************************************/
/* Specialized database API functions*/
/* have to be touched some more      */
/*************************************/

Boolean LIBCALL SHoundIsInited(void)
{
   if ((pd4Partition != NULL) &&
      (pd4Nucprot != NULL) &&
      (pd4Accdb != NULL) &&
      (pd4Pubseq != NULL) &&
      (pd4Taxgi != NULL) &&
      (pd4Sengi != NULL) &&
      (pd4Mmgi != NULL) &&
      (pd4Mmdb != NULL) &&
      (pd4Redund != NULL) &&
      (pd4Domdb != NULL))
        return TRUE;
   else
        return FALSE;
}


/* returns a ValNode list with data.intvalue = taxid of each org with a completed genome */
ValNodePtr LIBCALL SHoundAllGenomes(void)
{
   ValNodePtr pvnTax = 0;
   Int2 rc = 0;

   for (rc = d4top(pd4Chrom); rc == r4success; rc = d4skip(pd4Chrom, (long) 1))
     {
        if(!ValNodeIsIn(f4long(pf4chTaxId), pvnTax))
           pvnTax = Slri_ValNodeAddInt(&pvnTax, 0, f4long(pf4chTaxId));
     }
   return pvnTax;
}

/* returns a ValNode list with data.intvalue = kloodge of each DNA unit of completed genomes */
ValNodePtr LIBCALL SHoundAllKloodge(void)
{
   ValNodePtr pvnTax = 0;
   Int2 rc = 0;

   for (rc = d4top(pd4Chrom); rc == r4success; rc = d4skip(pd4Chrom, (long) 1))
     {
        if(!ValNodeIsIn(f4long(pf4chKloodge), pvnTax))
           ValNodeAddInt(&pvnTax, 0, f4long(pf4chKloodge));
     }
   return pvnTax;
}

/* returns all MMDB IDs in the MMDB database */
ValNodePtr LIBCALL SHoundAllMMDBID(void)
{
   ValNodePtr pvnmmdbid = 0;
   Int2 rc = 0;

   for (rc = d4top(pd4Mmdb); rc == r4success; rc = d4skip(pd4Mmdb, (long) 1))
     {
           pvnmmdbid = Slri_ValNodeAddInt(&pvnmmdbid, 0, f4long(pf4mdMmdbId));
     }
   return pvnmmdbid;
}

/* returns all GIs in SeqHound */
ValNodePtr LIBCALL SHoundAllGI(Int4 from, Int4 to)
{
   ValNodePtr pvngi = NULL;
   Int2 rc = 0;
   Int4 count = 0;

   if(to > SHoundGICount())
   {
      ErrPostEx(SEV_ERROR,0,0, "SHoundAllGI: The record number requested is out of range.");
	  to = SHoundGICount();
   }
	  
   for (rc = d4go(pd4Partition, from); rc == r4success; rc = d4skip(pd4Partition, (long) 1))
     {
           pvngi = Slri_ValNodeAddInt(&pvngi, 0, f4long(pf4paGi));
		   count++;
		   if(count > (to - from))
			   break;
     }
   return pvngi;
}


Int4 LIBCALL SHoundGICount(void)
{
	return (Int4) d4recCount(pd4Partition);
}


/**************************************/
/* Specialized database API functions */
/* no need to rewrite                 */
/**************************************/

/* lighweight version of SHoundProteinsFromTaxID which does not fill out linked list of structures from TAXGI */
ValNodePtr LIBCALL SHoundProteinsFromTaxIDII(Int4 taxid)
{

   ValNodePtr pvngi = NULL;
   Int2 rc = 0;
   Char type[10];
   Int4 gi = 0;

   if ((taxid <= 0) && (taxid != -9606))
     {
        ErrPostEx(SEV_ERROR,0,0, " SHoundProteinsFromTaxIDII: Invalid parameter");
        return NULL;
     }

   d4tagSelect(pd4Taxgi, tagtgTaxId);
   d4top(pd4Taxgi);
   rc = d4seekDouble(pd4Taxgi, taxid);
   if (rc == r4success)
    {
      u4ncpy(type, f4str(pf4tgType), sizeof(type));

      if (strstr(type, "protein") != NULL)
       {
             gi = f4long(pf4tgGi);
             pvngi = Slri_ValNodeAddInt(&pvngi, 0, gi);
       }
      for(rc = d4seekNextDouble(pd4Taxgi, taxid); rc == r4success; rc = d4seekNextDouble(pd4Taxgi, taxid))
        {
          u4ncpy(type, f4str(pf4tgType), sizeof(type));
          if (strstr(type, "protein") != NULL)
           {
             gi = f4long(pf4tgGi);
             pvngi = Slri_ValNodeAddInt(&pvngi, 0, gi);
           }
        }
    }
   return pvngi;
}


/* lighweight version of SHoundProteinsFromTaxID which does not fill out linked list of structures from TAXGI */
ValNodePtr LIBCALL SHoundDNAFromTaxIDII(Int4 taxid)
{

   ValNodePtr pvngi = NULL;
   Int2 rc = 0;
   Char type[10];
   Int4 gi = 0;

   if ((taxid <= 0) && (taxid != -9606))
   {
        ErrPostEx(SEV_ERROR,0,0, " SHoundDNAFromTaxIDII: Invalid parameter");
        return NULL;
   }
	

   d4tagSelect(pd4Taxgi, tagtgTaxId);
   d4top(pd4Taxgi);
   rc = d4seekDouble(pd4Taxgi, taxid);
   if (rc == r4success)
    {
      u4ncpy(type, f4str(pf4tgType), sizeof(type));

      if (strstr(type, "DNA") != NULL)
       {
             gi = f4long(pf4tgGi);
             pvngi = Slri_ValNodeAddInt(&pvngi, 0, gi);
       }
      for(rc = d4seekNextDouble(pd4Taxgi, taxid); rc == r4success; rc = d4seekNextDouble(pd4Taxgi, taxid))
        {
          u4ncpy(type, f4str(pf4tgType), sizeof(type));
          if (strstr(type, "DNA") != NULL)
           {
             gi = f4long(pf4tgGi);
             pvngi = Slri_ValNodeAddInt(&pvngi, 0, gi);

           }
        }
    }
   return pvngi;
}


/*******************************************/
/*     Reset structures representing       */
/*     database fields (structs declared   */
/*     in objslristruc.h)                  */
/* none of these actually free memory !!   */
/*******************************************/


void LIBCALL ResetStASNDB(StAsndbPtr PNTR ppas)
{
   StAsndbPtr pas = NULL;

   /* SK: NULL ptr check */
   if(ppas == NULL || *ppas == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "ResetStASNDB: NULL parameter.");
      return;
    }

   pas = *ppas;
   pas->asn1 = NULL;
   pas->gi = 0;
   pas->type = NULL;
   pas->division = NULL;
   pas->release = NULL;

   return;
}


void LIBCALL ResetStPARTI(StPartiPtr PNTR pppa)
{
   StPartiPtr ppa = NULL;

   /* SK: NULL ptr check */
   if(pppa == NULL || *pppa == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "ResetStPARTI: NULL parameter.");
      return;
    }

   ppa = *pppa;
   ppa->gi = 0;
   ppa->partition = NULL;

   return;
}



void LIBCALL ResetStNUCPROT(StNucprotPtr PNTR ppnc)
{
   StNucprotPtr pnc = NULL;

   /* SK: NULL ptr check */
   if(ppnc == NULL || *ppnc == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "ResetStNUCPROT: NULL parameter.");
      return;
    }

   pnc = *ppnc;
   pnc->gin = 0;
   pnc->gia = 0;

   return;
}



void LIBCALL ResetStACCDB(StAccdbPtr PNTR ppac)
{
   StAccdbPtr pac = NULL;

   /* SK: NULL ptr check */
   if(ppac == NULL || *ppac == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "ResetStACCDB: NULL parameter.");
      return;
    }

   pac = *ppac;
   pac->title = NULL;
   pac->gi = 0;
   pac->db = NULL;
   pac->name = NULL;
   pac->namelow = NULL;
   pac->access = NULL;
   pac->release = NULL;
   pac->version = 0;
   pac->chain = NULL;

   return;
}



void LIBCALL ResetStPUBSEQ(StPubseqPtr PNTR ppps)
{
   StPubseqPtr pps = NULL;

   /* SK: NULL ptr check */
   if(ppps == NULL || *ppps == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "ResetStPUBSEQ: NULL parameter.");
      return;
    }

   pps = *ppps;
   pps->gi = 0;
   pps->muid = 0;

   return;
}

void LIBCALL ResetStTAXGI(StTaxgiPtr PNTR pptg)
{
   StTaxgiPtr ptg = NULL;

   /* SK: NULL ptr check */
   if(pptg == NULL || *pptg == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "ResetStTAXGI: NULL parameter.");
      return;
    }

   ptg = *pptg;
   ptg->taxid = 0;
   ptg->gi = 0;
   ptg->type = NULL;

   return;
}


void LIBCALL ResetStCHROM(StChromPtr PNTR ppch)
{
   StChromPtr pch = NULL;

   /* SK: NULL ptr check */
   if(ppch == NULL || *ppch == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "ResetStCHROM: NULL parameter.");
      return;
    }

   pch = *ppch;
   pch->taxid = 0;
   pch->kloodge = 0;
   pch->chromfl = 0;

   return;
}


void LIBCALL ResetStSENDB(StSendbPtr PNTR ppsd)
{
   StSendbPtr psd = NULL;

   /* SK: NULL ptr check */
   if(ppsd == NULL || *ppsd == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "ResetStSENDB: NULL parameter.");
      return;
    }

   psd = *ppsd;
   psd->asn1 = NULL;
   psd->seid = 0;

   return;
}


void LIBCALL ResetStSENGI(StSengiPtr PNTR ppsg)
{
   StSengiPtr psg = NULL;

   /* SK: NULL ptr check */
   if(ppsg == NULL || *ppsg == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "ResetStSENGI: NULL parameter.");
      return;
    }

   psg = *ppsg;
   psg->seid = 0;
   psg->gi = 0;
   psg->division = NULL;

   return;
}


void LIBCALL ResetStMMDB(StMmdbPtr PNTR ppmd)
{
   StMmdbPtr pmd = NULL;

   /* SK: NULL ptr check */
   if(ppmd == NULL || *ppmd == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "ResetStMMDB: NULL parameter.");
      return;
    }

   pmd = *ppmd;
   pmd->asn1 = NULL;
   pmd->bwhat = 0;
   pmd->mmdbid = 0;
   pmd->pdbid = NULL;
   pmd->models = 0;
   pmd->molecules =0;

   return;
}


void LIBCALL ResetStMMGI(StMmgiPtr PNTR ppmg)
{
   StMmgiPtr pmg = NULL;

   /* SK: NULL ptr check */
   if(pmg == NULL || *ppmg == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "ResetStMMGI: NULL parameter.");
      return;
    }

   pmg = *ppmg;
   pmg->mmdbid = 0;
   pmg->gi = 0;

   return;
}


void LIBCALL ResetStREDUND(StRedundPtr PNTR pprd)
{
   StRedundPtr prd = NULL;

   /* SK: NULL ptr check */
   if(pprd == NULL || *pprd == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "ResetStREDUND: NULL parameter.");
      return;
    }

   prd = *pprd;
   prd->gi = 0;
   prd->ordinal = 0;
   prd->group = 0;

   return;
}



void LIBCALL ResetStDOMDB(StDomdbPtr PNTR ppdd)
{
   StDomdbPtr pdd = NULL;

   /* SK: NULL ptr check */
   if(ppdd == NULL || *ppdd == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "ResetStDOMDB: NULL parameter.");
      return;
    }

   pdd = *ppdd;
   pdd->asn1 = NULL;
   pdd->mmdbid = 0;
   pdd->domid = 0;
   pdd->gi = 0;
   pdd->domall = 0;
   pdd->pdbid = NULL;
   pdd->chain = NULL;
   pdd->rep = 0;

   return;


}

/*
void LIBCALL ResetStVASTDB(StVastdbPtr PNTR ppvd)
{
   StVastdbPtr pvd = NULL;

   /* SK: NULL ptr check */
   if(ppvd == NULL || *ppvd == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "ResetStVASTDB: NULL parameter.");
      return;
    }

   pvd = *ppvd;
   pvd->asn1 = NULL;
   pvd->mmdbid = 0;
   pvd->pdbid = NULL;
   pvd->chain = NULL;
   pvd->vastid1 = 0;
   pvd->vastid2 = 0;
   pvd->score = 0;
   pvd->pval = 0;
   pvd->res = 0;
   pvd->ident = 0;
   pvd->rmsd = 0;
   pvd->scale = 0;

   return;
}
*/

/*March 2001, Doron*/    
/* Reset StCDDB pointer*/
void LIBCALL ResetStCDDB (StCddbPtr PNTR pcdp)
{
        StCddbPtr cdp=NULL;
	
	/* SK: NULL ptr check */
        if(pcdp == NULL || *pcdb == NULL)
        {
                ErrPostEx (SEV_ERROR,0 ,0 , "ReserStCDDB : Null parameter.");
       		 return;  
        }
        cdp = *pcdp;
        cdp->gi=0;
        cdp->asn1=NULL;
        return;

}


/*******************************************/
/*      Text indexer                       */
/*      retrieves one SeqEntry with only   */
/*      one Bioseq in it                   */
/*******************************************/

static Boolean LIBCALL ExpandSeqEntryforTindex(SeqEntryPtr PNTR ppse, Int4 Gi)
{
   BioseqSetPtr pbss = NULL;
   SeqEntryPtr pseNew = NULL, pse = NULL;
   BioseqPtr pbsfull = NULL, pbs = NULL;
   Int4 gival = 0;

   /* SK: NULL ptr check */
   if (ppse == NULL || *ppse == NULL)
   {
       ErrPostEx(SEV_ERROR,0,0,"ExpandSeqEntry: Null pointer to Bioseq.");
       return FALSE;
   }

   pse = *ppse;
   if (pse->choice == 1) /* It is a Bioseq */
    {
      pbs = (BioseqPtr) pse->data.ptrvalue;
      if (pbs == NULL)
       {
         ErrPostEx(SEV_ERROR,0,0,"ExpandSeqEntry: Null pointer to Bioseq.");
         return FALSE;
       }

      if ((gival = GetGI(pbs)) <= 0)
       {
         ErrPostEx(SEV_ERROR,0,0,"ExpandSeqEntry: Invalid GI.");
         return FALSE;
       }
      if(gival == Gi)
       {
         /* get the full bioseq */
         pbsfull = SHoundGetBioseq(gival);
         pse->data.ptrvalue = pbsfull;
         /* free the original bioseq*/
         if(pbs)BioseqFree(pbs);
       }
    }
   else if (pse->choice == 2) /* it is nested Bioseq-set */
    {

         pbss = (BioseqSetPtr) pse->data.ptrvalue;
         if (pbss == NULL)
           {
             ErrPostEx(SEV_ERROR,0,0,"ExpandSeqEntry: Null pointer to Bioseq-Set.");
             return FALSE;
           }

         pseNew = pbss->seq_set;
         while (pseNew != NULL)
            {
             if(!ExpandSeqEntry(&pseNew))
               {
                ErrPostEx(SEV_ERROR,0,0,"ExpandSeqEntry: The recursive call to ToBiseqAs failed");
                return FALSE;
               }
              pseNew = pseNew->next;
            }
    }
   else
    {
         ErrPostEx(SEV_ERROR,0,0,"ExpandSeqEntry: Invalid choice field.");
         return FALSE;
    }
   return TRUE;
}


SeqEntryPtr LIBCALL GetSeqEntryfromSENDBforTindex(Int4 Gi)
{
   int iSize = 0;
   CharPtr pcASN = NULL;
   int rtn = 0;
   AsnIoMemPtr aimp;
   SeqEntryPtr pse = NULL;

   iSize = f4memoLen(pf4sdAsn1);
    if(iSize<=0){
   	/*post error message*/
   	ErrPostEx(SEV_ERROR, 0,0, "GetSeqEntryfromSENDB: f4memolen returned an empty asn.1 record.");
   	return NULL;
   }

   pcASN = (CharPtr) MemNew((size_t) iSize+1);
   if (pcASN == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "GetSeqEntryfromSENDB: Failed to allocate memory.");
      return NULL;
    }
   rtn = f4memoNcpy(pf4sdAsn1, pcASN, (unsigned) iSize);
   if(rtn<=0){
   	/*post error message*/
   	ErrPostEx(SEV_ERROR,0,0, "GetSeqEntryfromSENDB: f4memoNcopy failed");
   	return NULL;
   }

   aimp = AsnIoMemOpen("rb", (BytePtr) pcASN, iSize);
   if (aimp == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "GetSeqEntryfromSENDB: Failed to get AsnIo.");
      if (pcASN) MemFree(pcASN);
      return NULL;
    }
   pse = SeqEntryAsnRead(aimp->aip, NULL);
   if (pse == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "GetSeqEntryfromSENDB: Bioseq pointer is NULL.");
      if (pcASN) MemFree(pcASN);
      AsnIoMemClose(aimp);
      return NULL;
    }

   AsnIoFlush(aimp->aip);
   AsnIoMemClose(aimp);
   if (pcASN) MemFree(pcASN);
   if(!ExpandSeqEntryforTindex(&pse, Gi))
    {
      ErrPostEx(SEV_ERROR,0,0, "GetSeqEntryfromSENDB: ExpandSeqEntry failed.");
      return NULL;
    }
   return pse;
}



Int2 LIBCALL SearchSENDBforGIforTindex(StSendbPtr PNTR ppsd, Int4 gi)
{
   StSendbPtr psd = NULL;
   int rc;
   SeqEntryPtr pse = NULL;
   StSengiPtr psg = NULL;
   Int2 res = 0;
   Int4 keynum = 0;

   /* SK: NULL ptr check */
   if (ppsd == NULL || *ppsd == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "SearchSENDB: StSendbPtr is NULL.");
       return 0;
    }
   psd = *ppsd;

   /* have to search SENGI and open the right SENDB database */
   psg = StSengiNew();
   psg->gi = gi;
   if ((res = SearchSENGI(&psg)) == -1)
     return -1;
   else if (res == 0)
     return 0;
   keynum = psg->seid;
   if(!OpenSENDB(psg->division, FALSE))
    {
      ErrPostEx(SEV_ERROR,0,0, "SearchSENDB: Cannot open %s SENDB database", IsNullStr(psg->division));
      return 0;
    }
   StSengiFree(psg);

   /* search SENDB */
   d4tagSelect(pd4Sendb, tagsdSeid);
   d4top(pd4Sendb);
   rc = d4seekDouble(pd4Sendb, keynum);
   if (rc == r4success)
   {
      /* fill out the StSendb structure */
      psd->seid = f4long(pf4sdSeid);
      /* get Seq-entry */
      pse = GetSeqEntryfromSENDBforTindex(gi);
      if (pse == NULL)
       {
         ErrPostEx(SEV_ERROR,0,0, "SearchSENDB: SeqEntry pointer is NULL.");
         CloseSENDB();
         return 0;
       }
      psd->asn1 = pse;
      for(rc = d4seekNextDouble(pd4Sendb, keynum); rc == r4success; rc = d4seekNextDouble(pd4Sendb, keynum))
        {
          /* add and fill out the next StSendb structure */
          psd->next = StSendbNew();
          psd = psd->next;
          psd->seid = f4long(pf4sdSeid);
          pse = GetSeqEntryfromSENDBforTindex(gi);
          if (pse == NULL)
           {
             ErrPostEx(SEV_ERROR,0,0, "SearchSENDB: SeqEntry pointer is NULL.");
             CloseSENDB();
             return 0;
           }
          psd->asn1 = pse;
        }
      CloseSENDB();
      return 1;
    }
   else
      CloseSENDB();
      return -1;
}


/*
 $Log: intrez_db.c,v $
 Revision 1.3  2004/01/06 22:32:15  skuznets
 Code audit has been done sucessfully.
 Possible NULL pointer issues has been eliminated.

 Revision 1.2  2003/11/27 19:49:13  idonalds
 code4largeOn required to access files larger than 2GB

 Revision 1.1.1.2  2001/12/12 09:22:28  root
 Automated import from Sourceforge.net

 Revision 1.2  2001/12/11 20:41:00  kaca
 added AllocateAndCreateDBPath to database opening functions

 Revision 1.1.1.2  2001/10/10 21:53:48  kaca
 initial import

 Revision 1.29  2001/07/27 17:57:30  kaca
 modified SHoundAllGI

 Revision 1.28  2001/07/26 22:16:35  kaca
 fixed indentation for ifdef line

 Revision 1.27  2001/07/24 19:50:04  kaca
 added text index specific retrieval of SeqEntries

 Revision 1.26  2001/06/20 17:32:33  kaca
 code4largeOn needed on windows but does not work on unix

 Revision 1.25  2001/06/19 22:28:08  kaca
 large file on for windows

 Revision 1.24  2001/06/08 15:28:36  kaca
 lower casing of sequence names

 Revision 1.23  2001/05/25 19:53:36  kaca
 removed database opening from IsThisDivision

 Revision 1.22  2001/04/27 23:12:28  kaca
 added SHoundFindName and SHoundFindNameList functions

 Revision 1.21  2001/04/27 21:58:24  kaca
 made all numerical fields 10 bytes since use only Int4

 Revision 1.20  2001/04/16 17:09:57  kaca
 fix in IsThisDivision

 Revision 1.19  2001/04/12 20:59:49  kaca
 fixed bug in assigning Kloodge IDs

 Revision 1.18  2001/04/06 22:46:49  kaca
 moved in some functions from precompute

 Revision 1.17  2001/04/03 19:14:08  kaca
 moved all CodeBase globals in; any un-abstracted piece of code will have to include intrez_cb.h

 Revision 1.16  2001/03/29 15:44:40  kaca
 moved ExpandSrqEntry from intrez.c to intrez_db.c

 Revision 1.15  2001/03/28 22:32:41  gbader
 Changed order of includes - d4all.h must be first include on windows

 Revision 1.14  2001/03/27 21:10:41  doron
 Addition of CDDB (coding region database) functions

 Revision 1.13  2001/03/27 15:30:56  kaca
 removed molecule type field from REDUND

 Revision 1.12  2001/03/26 22:34:06  kaca
 created slri+misc and moved some API functions into it

*/
