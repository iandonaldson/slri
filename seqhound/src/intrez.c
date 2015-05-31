/* $Id: intrez.c,v 1.84 2005/04/14 18:21:26 vgu Exp $
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


#ifdef ODBC 
#include<intrez_odbc.h>
#include<ll_odbc.h>
#include<go_odbc.h>
#include<rpsdb_db.h>
#include<comgen_odbc.h>
#else
#include<intrez_db.h>
#include<ll_db.h>
#include<go_db.h>
/* include <asn2ffg.h> */
/* include <asn2ff.h> */
#endif

#include<intrez_misc.h>
#include<intrez_cfg.h>
#include<intrez.h>


#include<go_parser.h>
#ifndef _ODBC
#include<hist_db.h>
#endif
#include<objslristruc.h>

#include<polymerfix.h>


#define ARR_LEN 1024

/* global variable to keep track of SeqHound state */
static Boolean SHOUNDISON = FALSE;


/******************************/
/* SHound interface functions */
/******************************/

Boolean LIBCALL SHoundInit(Boolean NetEntrezOnToo, CharPtr appname)
{
  return SHoundInitEx(NetEntrezOnToo, appname, FALSE);
}


Boolean LIBCALL SHoundInitReadOnly(Boolean NetEntrezOnToo, CharPtr appname)
{
  return SHoundInitEx(NetEntrezOnToo, appname, TRUE);
}


Boolean  LIBCALL SHoundInitEx(Boolean NetEntrezOnToo, CharPtr appname, Boolean ReadOnly)
{
   if (!SHoundIsInited())
   {
     if(!Cfg_exists()) Cfg_shound_new();
     if(!ReadOnly)
     {
        if (!InitCodeBase())
        {
          ErrPostEx(SEV_ERROR,0,0, " InitCodeBase failed.");
          return FALSE;
        }

        if (!InitCOMGEN())
        {
          ErrPostEx(SEV_ERROR,0,0, " InitCOMGEN failed.");
          return FALSE;
        }
     }
     else
     {
        if (!InitCodeBaseReadOnly())
        {
          ErrPostEx(SEV_ERROR,0,0, " InitCodeBase failed.");
          return FALSE;
        }
		if (!InitCOMGEN())
        {
          ErrPostEx(SEV_ERROR,0,0, " InitCOMGEN failed.");
          return FALSE;
        }
     }

#ifdef ODBC
     /* call functions which create SQLHandles for DB layer functions SearchXXXXX */
     if (SHoundModule("lldb")) 
     {
        if (!LL_InitCodeBase(FALSE))
        {
          ErrPostEx(SEV_ERROR,0,0, " LL_InitCodeBase failed.");
          return FALSE;
        }
     }
     if (SHoundModule("neigdb")) 
     {
        if (!OpenCodeBaseNbr(FALSE))
        {
          ErrPostEx(SEV_ERROR,0,0, " OpenCodeBaseNbr failed.");
          return FALSE;
        }
     }
#endif

     if (!OpenPARTI("PARTI", FALSE))
      {
        ErrPostEx(SEV_ERROR,0,0, "OpenPARTI failed.");
        return FALSE;
      }
     if(!OpenNUCPROT("NUCPROT", FALSE))
      {
        ErrPostEx(SEV_ERROR,0,0, "OpenNUCPROT failed.");
        return FALSE;
      }
     if(!OpenACCDB("ACCDB", FALSE))
      {
        ErrPostEx(SEV_ERROR,0,0, "OpenACCDB failed.");
        return FALSE;
      }
     if(!OpenPUBSEQ("PUBSEQ", FALSE))
      {
        ErrPostEx(SEV_ERROR,0,0, "OpenPUBSEQ failed.");
        return FALSE;
      }
     if(!OpenTAXGI("TAXGI", FALSE))
      {
        ErrPostEx(SEV_ERROR,0,0, "OpenTAXGI failed.");
        return FALSE;
      }
     if(!OpenCHROM("CHROM", FALSE))
      {
       ErrPostEx(SEV_ERROR,0,0, "OpenCHROM failed.");
       return FALSE;
      }
     if(!OpenSENGI("SENGI", FALSE))
      {
        ErrPostEx(SEV_ERROR,0,0, "OpenSENGI failed.");
        return FALSE;
      }
     if(SHoundModule("strucdb"))
     {
        if(!OpenMMGI("MMGI", FALSE))
        {
           ErrPostEx(SEV_ERROR,0,0, "OpenMMGI failed.");
           return FALSE;
        }
        if(!OpenMMDB("MMDB", FALSE))
        {
          ErrPostEx(SEV_ERROR,0,0, "OpenMMDB failed.");
          return FALSE;
        }
        if(!OpenDOMDB("DOMDB", FALSE))
        {
          ErrPostEx(SEV_ERROR,0,0, "OpenDOMDB failed.");
          return FALSE;
        }
     }
     if(SHoundModule("redundb"))
     {
        if(!OpenREDUND("REDUND", FALSE))
        {
          ErrPostEx(SEV_ERROR,0,0, "OpenREDUND failed.");
          return FALSE;
        }
     }
     if(SHoundModule("taxdb"))
     {
        if(!OpenAllTaxDBs(FALSE))
        {
          ErrPostEx(SEV_ERROR,0,0, "OpenAllTaxDBs failed.");
          return FALSE;
        }
     }
     if(SHoundModule("neigdb"))
     {
        if(!OpenAllNbrDBs(FALSE))
        {
          ErrPostEx(SEV_ERROR,0,0, "OpenAllNbrDBs failed.");
          return FALSE;
        }
     }
     if(SHoundModule("godb"))
     {
	 if(!GO_OpenAllDBs(FALSE))
	 {
	     ErrPostEx(SEV_ERROR,0,0, "LL_OpenAllDBs failed.");
	     return FALSE;
        }
     }

     if(SHoundModule("lldb"))
     {
	 if(!LL_OpenAllDBs(FALSE))
	 {
	     ErrPostEx(SEV_ERROR,0,0, "LL_OpenAllDBs failed.");
	     return FALSE;
	 }
     }
     if(SHoundModule("rpsdb"))
     {
	 if(!OpenRPSDB("MASTER", FALSE))
	 {
	     ErrPostEx(SEV_ERROR,0,0, "OpenRPSDB failed.");
	     return FALSE;
	 }
	 if(!OpenDOMNAME("DOMNAME", FALSE))
	 {
	     ErrPostEx(SEV_ERROR,0,0, "OpenDOMNAME failed.");
	     return FALSE;
	 }
     }
     if(SHoundModule("dbxref"))
     {
	 /* Nothing to do here ODBC don't need to open any file */
     }


     if(!OpenHISTDB(FALSE))
     {
        ErrPostEx(SEV_ERROR, 0, 0, "OpenHISTDB failed.");
	return FALSE;
     }

   }
   SHOUNDISON = TRUE;

/* 
 * HL: NetEntrez is by default turned off,
 * unless compiled with the -DINTREZ_NET flag
 */
#ifdef INTREZ_NET
   if((NetEntrezOnToo == TRUE) && (!SHoundIsNetEntrezOn()))
   {
      if (!EntrezInit(appname, FALSE, NULL))
      {
           ErrPostEx(SEV_FATAL,0,0, "EntrezInit failed.");
           return FALSE;
      }
   }
#endif
   return TRUE;
}


Boolean LIBCALL SHoundIsInited(void)
{
   if(SHOUNDISON)
     return TRUE;
   else
     return FALSE;
}

Boolean  LIBCALL SHoundStartTransaction()
{
   if (!SHoundIsInited())
   {
     ErrPostEx(SEV_ERROR,0,0, "Must first initialize database connection by calling SHoundInit.");
     return FALSE;
   }

#ifdef ODBC
   if (!StartTransaction())
   {
     ErrPostEx(SEV_ERROR,0,0, "Unable to start transaction.");
     return FALSE;
   }
#endif

   return TRUE;
}

/* 
 * HL: NetEntrez is by default turned off,
 * unless compiled with the -DINTREZ_NET flag
 */
#ifdef INTREZ_NET
Boolean    LIBCALL SHoundIsNetEntrezOn(void)
{
   if (EntrezIsInited())
     return TRUE;
   else
     return FALSE;
}
#endif

Boolean LIBCALL SHoundNetEntrezInit(CharPtr appname)
{
   if (SHoundInit(TRUE, appname))
     return TRUE;
   else
     return FALSE;
}


void  LIBCALL SHoundFini(void)
{

   if (SHoundIsInited())
    {
      if (!ClosePARTI())
          ErrPostEx(SEV_ERROR,0,0, "ClosePARTI failed.");
      if (!CloseNUCPROT())
          ErrPostEx(SEV_ERROR,0,0, "CloseNUCPROT failed.");
      if (!CloseACCDB())
          ErrPostEx(SEV_ERROR,0,0, "CloseACCDB failed.");
      if(!ClosePUBSEQ())
          ErrPostEx(SEV_ERROR,0,0, "ClosePUBSEQ failed.");
      if (!CloseTAXGI())
          ErrPostEx(SEV_ERROR,0,0, "CloseTAXGI failed.");
      if (!CloseCHROM())
          ErrPostEx(SEV_ERROR,0,0, "CloseCHROM failed.");
      if (!CloseSENGI())
          ErrPostEx(SEV_ERROR,0,0, "CloseSENGI failed.");
      if(SHoundModule("redundb"))
         if (!CloseREDUND())
            ErrPostEx(SEV_ERROR,0,0, "CloseREDUND failed.");
      if(SHoundModule("strucdb"))
	{
         if (!CloseMMGI())
            ErrPostEx(SEV_ERROR,0,0, "CloseMMGI failed.");
         if (!CloseMMDB())
            ErrPostEx(SEV_ERROR,0,0, "CloseMMDB failed.");
         if (!CloseDOMDB())
            ErrPostEx(SEV_ERROR,0,0, "CloseDOMDB failed.");
	}
      if(SHoundModule("taxdb"))
         CloseAllTaxDBs();
      if(SHoundModule("neigdb"))
         CloseAllNbrDBs(FALSE);
      if(SHoundModule("godb"))
         CloseAllGODBs();
      if(SHoundModule("lldb"))
         CloseAllLLDBs();
      CloseAllHISTDBs();
      if (!CloseCodeBase())
          ErrPostEx(SEV_ERROR,0,0, "CloseCodeBase failed.");

      if(Cfg_exists()) Cfg_FreeMem();
      SHOUNDISON = FALSE;
    }
/*
 * HL: NetEntrez is by default turned off.
 * To turn on, compile with -DINTREZ_NET flag.
 */
#ifdef INTREZ_NET
   if(SHoundIsNetEntrezOn())
     EntrezFini();
#endif
}

Boolean  LIBCALL SHoundEndTransaction()
{
   if (!SHoundIsInited())
   {
     ErrPostEx(SEV_ERROR,0,0, "Must call this function before calling SHoundFini.");
     return FALSE;
   }

#ifdef ODBC
   if (!EndTransaction())
   {
     ErrPostEx(SEV_ERROR,0,0, "Unable to end transaction.");
     return FALSE;
   }
#endif

   return TRUE;
}

/*Allocates memory - free returned value*/
CharPtr LIBCALL SHoundGetCurrentServer(void)
{
    Char dbPath[PATH_MAX], retVal[PATH_MAX+128];

	/*local API - always local host - return database path*/
    GetAppParam("intrez", "datab", "path", NULL, dbPath, (size_t)PATH_MAX * (sizeof(Char)));
	sprintf(retVal, "local database: %s", IsNullStr(dbPath));

	return(StringSave(retVal));
}

/* Sets timeout for remote api - not used in local api */
void SHoundUserSetTimeout(Int4 sec) {}
Int4 SHoundUserGetTimeout(void) {return 0;}

/******************************************************
 * ProteinReportSetRedundantGroup
 * 
 * Description: retrieves ProteinReports for the redundant
 *   set. pprsd->gi is set to a key whose redundant set
 *   is desired as protein reports.
 *
 * flags can be set as specified in seqhound.h (under REPORT_FLAGS).
 *   to turn on the flags.
 *   Uint2 flags = 0; flags |= REPORT_ALL;
 *     
 *   to set a subset of the flags, use:
 *      flags |= REPORT_DNA | REPORT_MMDB | REPORT_TAX;
 *
 ******************************************************/
Boolean ProteinReportSetRedundantGroup(SHProteinReportSetDescriptionPtr pprsd, Uint2 flags)
{
   SHProteinReportPtr pcurr   = NULL,    /* a tmp pointer */
                      pBRPPR  = NULL;    /* protein report for best annotation */
   ValNodePtr          VNPRed = NULL,    /* redundant set */
                            v = NULL;    /* tmp pointer */
   Int4              iGroupID = 0,       /* redundant group id */
                     iFirstOf = 0;       /* best annotation id */

   /* SK: NULL ptr check */
   if ( pprsd == NULL )
   {
      ErrPostEx(SEV_INFO,0,0, "[ProteinReportSetRedundantGroup]: NULL pprsd.");
      return FALSE;
   }

   /* 
    * get the best annotation report: start by getting the
    * redundant group 
    */
   if( (iGroupID = SHoundRedundantGroupIDFromGI(pprsd->query)) > 0 )
   {
     /* then get the best annotation of the group */
     iFirstOf = SHoundFirstOfRedundantGroupFromID(iGroupID); 
     pBRPPR = SHProteinReportNew();
     pBRPPR->gi = iFirstOf;
     
     /* get the protein report of the best annotation */
     if(SHoundGetProteinReport(pBRPPR, flags))
     {
        pprsd->best_report = pBRPPR;
        pprsd->total_reports += 1;
     }else{  SHProteinReportFree(pBRPPR); }
   
   } /* best annotation protein report */


   /* 
    * get the redundant gi's and fill in their reports based on 'flags' 
    * */
   if( (VNPRed = SHoundRedundantGroup(pprsd->query)) != NULL )
   {
      /* we have redundant gi, get their protein reports */
      pprsd->redundant_set = VNPRed;
      pprsd->redun_reports = SHProteinReportSetNew();
      pprsd->redun_reports->reports = SHProteinReportNew();
      pcurr = pprsd->redun_reports->reports; 

      /* have a list of the redundant gi's, now filling in the 
         individual reports */
      for(v = pprsd->redundant_set; v != NULL; v = v->next)
      {
         pcurr->gi = v->data.intvalue;
         if( SHoundGetProteinReport(pcurr, flags) ) 
         {
            pprsd->redun_reports->number++;        /* keeping track of number of reports */
         } else {
            /* free to prevent leaks */
            pcurr = (SHProteinReportPtr) SHProteinReportFree(pcurr);
         }
         if(v->next != NULL) pcurr->next = SHProteinReportNew();
         pcurr = pcurr->next;

      }  /* end for: got all the redundant gi protein reports */

      /* update the count of protein reports */
      pprsd->total_reports += pprsd->redun_reports->number;
   }  /* end if: redundant gi protein reports filled in, counter updated */

   return TRUE;
}

/*************************************************************
 * ProteinReportSetNeighboursReport
 *
 * Description: retrieves protein reports for the neighbours of a gi.
 *   the gi is set in pprsd->gi && pprsd->cutoff must also be
 *   specified.
 *
 * flags can be set as specified in seqhound.h (under REPORT_FLAGS). 
 *   to turn on all flags:
 *   Uint2 flags = 0; flags |= REPORT_ALL;
 *
 *   to set a subset of the flags, use:
 *      flags |= REPORT_DNA | REPORT_MMDB | REPORT_TAX;
 *
 *************************************************************/
Boolean ProteinReportSetNeighboursReport(SHProteinReportSetDescriptionPtr pprsd, Uint2 flags)
{
   SHProteinReportPtr pcurr = NULL;      /* protein report for the neighbour set */
   FLinkSetPtr          pfs = NULL;      /* link list used in neighbour functions */
   Int4                   i = 0;         /* loop counter */
   Int4Ptr v = NULL;                     /* variable for holding the gi of each neighbour */

   /* SK: NULL ptr check */
   if ( pprsd == NULL )
   {
      ErrPostEx(SEV_INFO,0,0, "[ProteinReportSetRedundantGroup]: NULL pprsd.");
      return FALSE;
   }

   /* 
    * get the neighbours of pprsd->query, and save list into 'pfs'
    * using cutoff specified in pprsd 
    */
   if(SHoundNeighboursFromGi(pprsd->query, &pfs, pprsd->cutoff, FALSE) == SLRI_SUCCESS && (pfs))
   {
      pprsd->neighbour_set = pfs;
      pprsd->neighbour_reports = SHProteinReportSetNew();
      pprsd->neighbour_reports->reports = SHProteinReportNew();
      pcurr = pprsd->neighbour_reports->reports;

      /* 
       * get the protein reports of each neighbour. 
       * fill in report based on 'flags' 
       */
      for(v = pprsd->neighbour_set->uids; i < pprsd->neighbour_set->num; i++, v++)
      {
         pcurr->gi = *v; 
         if( SHoundGetProteinReport(pcurr, flags) )
         {
            pprsd->neighbour_reports->number++;
         } else {
            pcurr = (SHProteinReportPtr) SHProteinReportFree(pcurr);
         }
         if(i+1 < pprsd->neighbour_set->num) pcurr->next = SHProteinReportNew();
         pcurr = pcurr->next;
      } /* end of retrieval of neighbours protein reports */

      /* update the count of protein reports */
      pprsd->total_reports += pprsd->neighbour_reports->number;
   }

   return TRUE;
}

/***********************************************************************
 * There are 14 flags (settings) that can be turned on:
 * See seqhound.h (under REPORT_FLAGS) for all 
 *    the different flags that can be turned on.
 *
 * 
 * To turn on all the flags:
 *     Uint2 report_flags = 0;
 *     report_flags |= REPORT_ALL;
 *
 * To set only a subset of the flags, use the bitwise or operator (|). 
 * Eg, to turn on the flags REPORT_ACC, REPORT_DNA and REPORT_TAXID, use the syntax:
 *     Uint2 report_flags = 0;
 *     report_flags |= REPORT_ACC | REPORT_DNA | REPORT_TAXID;
 ***************************************************************************/
Boolean LIBCALL SHoundProteinReportSetDescription
   (SHProteinReportSetDescriptionPtr pprsd, Uint2 flags)
{
   SHProteinReportPtr      ppr = NULL;     /* protein report for the query protein */

   if( pprsd == NULL || pprsd->query <= 0 )
   {
      ErrPostEx(SEV_INFO, 0, 0, "[SHoundProteinReportSetDescription]: Null input");
      goto end;
   }

   /* get the protein report for the query gi */
   ppr = SHProteinReportNew();
   ppr->gi = pprsd->query;
   if(SHoundGetProteinReport(ppr, flags)) 
   { 
      pprsd->query_report = ppr; 
      pprsd->total_reports += 1;      /* update count of reports */
   } /* protein report of query gi */

   /* This if statement first checks if REPORT_REDUND is set in 'flags'. If set,
      'pprsd->redund' is initialized to 'true' and ProteinReportSetRedundantGroup 
      is called. If not set, then 'pprsd->redund' is set to false.

      ProteinReportSetRedundantGroup gets the protein reports for all the 
      redundant gi's and their protein reports.
    */
   if(  ( pprsd->redund = ((flags & REPORT_REDUND) >> BYTE_LENGTH) )  )
      { ProteinReportSetRedundantGroup(pprsd, flags); }

   /* This if statement first checks if REPORT_NBR is set in 'flags'. If set, 
      'pprsd->neighbours' initialized to true and ProteinReportSetNeighboursReport 
      is called. If not set, then 'pprsd->neighbours' set to false. 
  
      ProteinReportSetNeighboursReport will get all the neighbours of 'pprsd->query'
      and their protein reports.
    */
   if(  ( pprsd->neighbours = ((flags & REPORT_NBR) >> BYTE_LENGTH) )  )
      { ProteinReportSetNeighboursReport(pprsd, flags); }

   /* 
    * the 'pprsd->x' in the lines below (and above) are booleans 
      that indicate whether those fields are filled out in the
      protein report, as determined by 'flags'. 
      
      syntax: '>> BYTE_LENGTH' used to shift bits to fit Uint2 into Uint1. 
    */
   pprsd->defline = flags & REPORT_DEF;
   pprsd->acc = flags & REPORT_ACC;
   pprsd->dna = flags & REPORT_DNA;
   pprsd->muid = flags & REPORT_MUID;
   pprsd->pmid = ((flags & REPORT_PMID) >> BYTE_LENGTH);
   pprsd->mmdb_ids = flags & REPORT_MMDB;
   pprsd->taxid = ((flags & REPORT_TAX) >> BYTE_LENGTH);
   pprsd->go = flags & REPORT_GO;
   pprsd->omim = flags & REPORT_OMIM;
   pprsd->llink = flags & REPORT_LL;
  
   return TRUE;
end:
   return FALSE;
}

/***************************************************************************
 *
 * SHoundGetProteinReport
 *   
 * Description: retrieves a protein report (ppr) for a specified gi based
 *      on flags set in REPORT_FLAGS.
 *    The gi is specified in ppr, ie ppr->gi = 123456, and the
 *      report returned to the user is ppr.
 *
 *    REPORT_FLAGS is 9 flags that can be turned on, see
 *    seqhound.h (under REPORT_FLAGS) for info on the flags.
 * 
 *       REPORT_LL, REPORT_OMIM, REPORT_GO, REPORT_TAX, REPORT_MMDB, 
 *       REPORT_DNA, REPORT_MUID, REPORT_DEF, REPORT_ACC
 *
 *    To turn on all the flags:
 *      Uint2 flags = 0;
 *      flags |= REPORT_ALL
 *
 *    To turn on only a subset of the flags, use the bitwise OR operator (|). 
 *    eg. to turn on the REPORT_LL and REPORT_GO flags.
 *      flags |= REPORT_LL | REPORT_GO;
 *
 *****************************************************************************/
Boolean LIBCALL SHoundGetProteinReport(SHProteinReportPtr ppr, Uint2 REPORT_FLAGS)
{
   SHLocusLinkReportPtr   pllr = NULL;    /* locuslink report, a component of a protein report */
   SHGoReportPtr           pgr = NULL;    /* go report, a component of the protein report */
   Int4                   iTmp = 0;       /* temp integer variable */      
   CharPtr                 acc = NULL;    /* accession of protein */
   CharPtr             defline = NULL;    /* defline */
   
   if( ppr == NULL || ppr->gi <= 0 )
   {
      ErrPostEx(SEV_INFO, 0, 0, "[SHoundGetProteinReport]: Invalid input.");
      goto end;
   }

   pllr = SHLocusLinkReportNew();
  
   /* 
    * each flag is checked to see if they are turned on,
    *   if turned on, then the structure is retrieved and initialized. 
    */
   
   /* checks if REPORT_LL is turned on, if so, retrieve the locus link report */
   if((REPORT_FLAGS & REPORT_LL) && SHoundGetLocusLinkReport(pllr, ppr->gi))
   {
      ppr->llink = pllr;
   }else{ SHLocusLinkReportFree(pllr); }
  
   /* retrieve the omim associated with the gi */
   if(REPORT_FLAGS & REPORT_OMIM) 
     { ppr->omim = SHoundOMIMFromGi(ppr->gi); }
 
   /* retrieve the gene ontology reports */ 
   pgr = SHGoReportNew();
   if( (REPORT_FLAGS & REPORT_GO) && SHoundGetGOReport(pgr, ppr->gi))
   {
      ppr->go = pgr;
   }else { SHGoReportFree(pgr); }

   /* taxid, mmdbid, dna id, muid */
   if( (REPORT_FLAGS & REPORT_TAX) && (iTmp = SHoundTaxIDFromGi(ppr->gi)) > 0 ) 
      { ppr->taxid = iTmp; }
   if( (REPORT_FLAGS & REPORT_MMDB) && (iTmp = SHound3DFromGi(ppr->gi)) > 0 ) 
      { ppr->mmdbid = iTmp; }
   if( (REPORT_FLAGS & REPORT_DNA) && (iTmp = SHoundDNAFromProtein(ppr->gi)) > 0) 
      { ppr->dna = iTmp; }
   if( (REPORT_FLAGS & REPORT_MUID) )
      { ppr->muids = SHoundGetReferenceIDFromGi(ppr->gi, 2); }

/* if 0 */
   if( (REPORT_FLAGS & REPORT_PMID) )
      { ppr->pmids = SHoundGetReferenceIDFromGi(ppr->gi, 1); }
/* endif */

   /* defline, accession id */
   if( (REPORT_FLAGS & REPORT_DEF) && (defline = SHoundGetDefline(ppr->gi)) != NULL )
      { ppr->defline = defline; }

   if( (REPORT_FLAGS & REPORT_ACC) 
          && (acc = SHoundGetNameByGi(ppr->gi)) != NULL 
          && (strstr(acc, "n/a") == NULL) )
      { ppr->acc = acc; }

   
   return TRUE; 
end:
   ErrPostEx(SEV_INFO, 0, 0, "[SHoundGetProteinReport]: Error in SHoundGetProteinReport");
   return FALSE;
}

/*****************************************************************
 * SHoundGetGOReport
 *
 * Description: retrieves the GOReport for a gi.
 ****************************************************************/
Boolean LIBCALL SHoundGetGOReport(SHGoReportPtr pgr, Int4 Gi)
{
   ValNodePtr vnp = NULL;       /* the goids of this gi */
   
   if(Gi <= 0 || pgr == NULL)
   {
      ErrPostEx(SEV_INFO, 0, 0, "[SHoundGetGOReport]: Invalid Inputs %d", Gi);
      goto end;
   }

   /* get all the goid associated with this gi */ 
   if( (vnp = SHoundGOIDFromGi(Gi)) != NULL)
   {
      ValNodePtr          v = NULL;   /* list iterator */
      SHGoAnnotPtr        p = NULL;   /* holds go annotation for each goid */
      Int2                i = 0;      /* loop counter */

      /* 
       * each goid has a name, a classification (whether it is from
       * the molecular_function ontology, biological_process ontoloty,
       * or the cellular_component ontology), 0 or more PMID, and 0 or 
       * more evidence code. These data get stored into a GoAnnotNode structure. 
       */
      for(v = vnp; v != NULL; v = v->next)
      {
	 /* create the GoAnnotNode structure to hold each annotation */
	 p = SHGoAnnotNodeNew();
	 
	 /*
	  * save the annotation data: each goreport has 3 annotation
	  * node, 1 for each of the ontology. AddGOAnnotation will
	  * save the data of each goid (and Gi) into a GoAnnot Node
	  * and link it to the FRONT of the appropriate list of the GoReport. 
	  */
	 p->goid = v->data.intvalue;
	 if( (i = AddGOAnnotation(p, Gi)) == GO_MOLFUNCTION) {	 
	    p->next = pgr->mol_fun; pgr->mol_fun = p;
	    pgr->num_mol_fun++;
	 } else if( i == GO_BIOPROCESS ) { 
	    p->next = pgr->bio_pro; pgr->bio_pro = p; 
	    pgr->num_bio_pro++;
	 } else { 
	    p->next = pgr->cell_com; pgr->cell_com = p;
	    pgr->num_cell_com++;
	 }
      }  /* the go annotation has been stored */

   }else{
      /* did not find goid associated with this gi */ 
      return FALSE; 
   } 

   return TRUE;
end:
   return FALSE;
}

/******************************************************************
 * AddGOAnnotation:
 *  
 * Description: Adds the go annotation for a gi
 *  -annotation information are: gene ontology(go) evidence code,
 *      go pmid, go name, ontology classfication
 *      
 ******************************************************************/
Int2 AddGOAnnotation(SHGoAnnotNodePtr pga, Int4 gi)
{
   if(gi <= 0 || pga == NULL)
   {
      ErrPostEx(SEV_INFO, 0, 0, "[AddGoAnnotation]: Invalid input.");
      return FALSE;
   }
   /* fill the ecodes and pmid and name */
   pga->evidence_code = SHoundGOECodeFromGiAndGOID(gi, pga->goid); 
   pga->pmid = SHoundGOPMIDFromGiAndGOID(gi, pga->goid); 
   pga->goname = SHoundGODBGetNameByID(pga->goid);

   /* get the classification */
   pga->ontology = SHoundGODBGetClassification(pga->goid);
   if(StringCmp(GO_MOL_FUN, pga->ontology) == 0){
      return GO_MOLFUNCTION;
   }else if(StringCmp(GO_BIO_PRO, pga->ontology) == 0){
      return GO_BIOPROCESS;
   }else if(StringCmp(GO_CELL_COM, pga->ontology) == 0){
      return GO_CELLCOMPONENT;
   }
   return -1;
}

/**********************************************************************
 * SHoundGetLocusLinkReport
 *
 * Description: retrieves the locuslink report for a gi
 * 
 * returns the locuslink report associated with the gi 
 *
 **********************************************************************/
Boolean LIBCALL SHoundGetLocusLinkReport(SHLocusLinkReportPtr plr, Int4 Gi)
{
   Int4                  iTmp = 0;       /* the locuslink id */
   CharPtr             pLocus = NULL;    /* the locus */
   SHLocusProductPtr      plp = NULL;    /* asn locus product */

   if( plr == NULL || Gi <= 0 )
   {
      ErrPostEx(SEV_INFO, 0, 0, "[SHoundGetLocusLinkReport]: Invalid input");
      goto end;
   }
   
   if( (iTmp = SHoundLLIDFromGi(Gi)) > 0 )
   {
      plr->llid = iTmp;
  
     if( (pLocus = SHoundLocusFromGi(Gi)) != NULL)
        { plr->locus = pLocus; }
     
     plp = SHLocusProductNew();
     if(SHoundGetLocusProduct(plp, plr->llid))
        { plr->products = plp;}
     else 
        { plr->products = NULL; }

   }else { 
     /* could not find the LLID from this GI, so not in locuslink */
     return FALSE; 
   }

   return TRUE; 
   
end:
   return FALSE;
}

/* retrieves the locusproduct associated with the locus link id. */
Boolean LIBCALL SHoundGetLocusProduct(SHLocusProductPtr plp, const Int4 LLID)
{
  ValNodePtr vnp = NULL;
  SHLocusProductPtr plpp = NULL;
  if(plp == NULL)
  {
    ErrPostEx(SEV_INFO, 0, 0, "[SHoundGetLocusProduct]: Null object");
    goto end;
  }
  if(LLID <= 0)
  {
     ErrPostEx(SEV_INFO, 0, 0, "[SHoundGetLocusProduct]: Invalid LLID %d", LLID);
     goto end;
  } 
  
  /* get all the gi's associated with this LLID */
  if( (vnp = SHoundGiFromLLID(LLID)) != NULL )
  {
     CharPtr p = NULL;
     ValNodePtr v = NULL;
     for(plpp = plp, v = vnp; v != NULL; v = v->next, plpp = plpp->next)
     {
	/* save the gi's, and get the deflines of the gi's */
        plpp->gi = v->data.intvalue;
	if( (p = SHoundGetDefline(plpp->gi)) != NULL )
	{
	   plpp->defline = p;
	}

	/* advance the next locusproduct */
	if(v->next != NULL)
	   plpp->next = SHLocusProductNew();
     } /* end for */
  } /* end if */

  ValNodeFree(vnp);
  return TRUE;

end:
  return FALSE;
}

/******************************************************/
/* SeqId to/from Gi, Accession to/from Gi Conversions */
/******************************************************/

Int4 LIBCALL SHoundFindSeqId(SeqIdPtr psi)
{
     
     TextSeqIdPtr  ptsi = NULL;
     StAccdbPtr pac = NULL, pachead = NULL;
     PDBSeqIdPtr ppsi = NULL;  
     DbtagPtr pdt = NULL;
     Int4 id = 0, gi = 0;
     Char idstr[10];
     Int2 res = 0;
    
     if (psi == NULL)
       {
         ErrPostEx(SEV_ERROR,0,0, "SHoundFindSeqId: Seq-id pointer is NULL.");
         return 0;
       }

     pac = StAccdbNew();
     pachead = pac;
     
     if ((psi->choice == 5)  /* GenBank */   ||
        (psi->choice == 6)  /* EMBL */       ||
        (psi->choice == 7)  /* PIR */        ||
        (psi->choice == 8)  /* SWISS-PROT */ ||
        (psi->choice == 10) /* other */      ||
        (psi->choice == 13)  /* DDBJ */      ||
        (psi->choice == 14)  /* PRF */)    
      {
         ptsi = psi->data.ptrvalue;
         if (ptsi == NULL)
          {
            ErrPostEx(SEV_ERROR,0,0, "SHoundFindSeqId: NULL TextSeq-id pointer.");
            StAccdbFree(pachead);
            return  0;     
          }
         if (ptsi->accession != NULL)
            pac->access = StrSave(ptsi->accession);
         if (ptsi->name != NULL)
            pac->name = StrSave(ptsi->name);    
      }
         
   if (psi->choice == 15)  /* PDB */
    {
       ppsi = psi->data.ptrvalue;
       if (ppsi == NULL)
        {
          ErrPostEx(SEV_ERROR,0,0, "SHoundFindSeqId: NULL PDBSeqId pointer.");
          StAccdbFree(pachead); 
          return 0;     
        }
       if (ppsi->mol != NULL)
          pac->name = StrSave(ppsi->mol);    
    }
   
   if (psi->choice == 11)  /* General dbtag for other databases */ 
    {
      pdt = psi->data.ptrvalue;
      if (pdt == NULL)
       {
          ErrPostEx(SEV_ERROR,0,0, "SHoundFindSeqId: NULL Dbtag pointer.");
          StAccdbFree(pachead); 
          return 0;     
       }
      /* SK: NULL ptr check */
      if ( pdt->tag == NULL )
      {
        ErrPostEx(SEV_ERROR,0,0, "SHoundFindSeqId: NULL pdt->tag.");
        StAccdbFree(pachead);
        return 0;
      }

      if (pdt->tag->str != NULL)
         pac->name = StrSave(pdt->tag->str);
      else
       {
        id = pdt->tag->id;
        sprintf(idstr, "%ld", (long) id);
        pac->name = idstr;
       }
    }     
   
   
   if ((pac->name == NULL) && (pac->access == NULL))
    {
      ErrPostEx(SEV_ERROR,0,0, "SHoundFindSeqId: No allowed search key in the StAccdb structure.");
      StAccdbFree(pachead); 
      return 0;
    }
   
   res = SearchACCDB(&pac);
   if (res == 0)
    { 
       ErrPostEx(SEV_ERROR,0,0, "SHoundFindSeqId: Search ACCDB failed.");
       StAccdbFree(pachead); 
       return 0;
    }
   if (res == -1)
    {
      /*  ErrPostEx(SEV_ERROR, 0,0, "Accession not found in ACCDB."); */
       StAccdbFree(pachead); 
       return -1;   
    }
   if (res == 1)
      while(pac != NULL)
       {
         /* ErrPostEx(SEV_ERROR, 0,0, "Accession found in ACCDB."); */
         gi = pac->gi;
         pac = pac->next;
       } 
    
   StAccdbFree(pachead); 
   return gi;
}

ValNodePtr LIBCALL SHoundFindSeqIdList(ValNodePtr pvnSeqId)
{
   ValNodePtr pvngi = NULL;
   Int4 gi = 0;
     
   while(pvnSeqId != NULL)
    {
       gi = SHoundFindSeqId(pvnSeqId->data.ptrvalue);
       if (gi > 0)
         pvngi = Slri_ValNodeAddInt(&pvngi, 0, gi);
       pvnSeqId = pvnSeqId->next;
    }

   Misc_IntVnpUnique(&pvngi);
   return pvngi;
}


Int4  LIBCALL SHoundFindAcc(CharPtr pcAcc)
{
   Int4 gi = 0;
   StAccdbPtr pac = NULL, pachead = NULL;
   Int2 res = 0;
   
   if ((pcAcc == NULL) || (strcmp(pcAcc, "n/a") == 0))
     {
       ErrPostEx(SEV_ERROR,0,0, "SHoundFindAcc: No accession.");
       return 0;
     }

   pac = StAccdbNew();
   pachead = pac;
   pac->access = StrSave(pcAcc);
   res = SearchACCDB(&pac);
   if (res == 0)
    { 
       ErrPostEx(SEV_ERROR,0,0, "SHoundFindAcc: Search ACCDB failed.");
       StAccdbFree(pachead);
       return 0;
    }
   if (res == -1)
    {
      /*  ErrPostEx(SEV_ERROR,0,0, "Accession not found in ACCDB."); */
       StAccdbFree(pachead);
       return -1;   
    }
   if (res == 1)
      while(pac != NULL)
       {
	 /*  ErrPostEx(SEV_ERROR,0,0, "Accession found in ACCDB."); */
         gi = pac->gi;
         pac = pac->next;
       } 
   
   StAccdbFree(pachead);
   return gi;
}

CharPtr LIBCALL SHoundGetNameByGi(Int4 gi)
{
	StAccdbPtr pac = NULL;

	if (gi <= 0)
	{
		ErrPostEx(SEV_ERROR,0,0, "SHoundAccFromGi: Invalid GI.");
		return NULL;
	}

	pac = StAccdbNew();
	pac->gi = gi;	/*since gi is PRIMARY key it will be only one corresponding name in the hole acc-db*/
	if(SearchACCDB(&pac) <= 0)
	{
		StAccdbFree(pac);
		return NULL;
	}
	
	return StringSave(pac->name);
}



ValNodePtr LIBCALL SHoundFindAccList(ValNodePtr pvnAcc)
{
   Int4 gi = 0;
   ValNodePtr pvngi = NULL;
   
   while(pvnAcc != NULL)
    {
       gi = SHoundFindAcc(pvnAcc->data.ptrvalue);
       if (gi > 0)
         pvngi = Slri_ValNodeAddInt(&pvngi, 0, gi);
       pvnAcc = pvnAcc->next;
    }
   Misc_IntVnpUnique(&pvngi);
   return pvngi;
}


Int4  LIBCALL SHoundFindName(CharPtr pName)
{
   Int4 gi = 0;
   StAccdbPtr pac = NULL, pachead = NULL;
   Int2 res = 0;
   CharPtr plow = NULL;

   if ((pName == NULL) || (strcmp(pName, "n/a") == 0) || (strcmp(pName, "")==0))
     {
       ErrPostEx(SEV_ERROR,0,0, "SHoundFindAcc: No accession.");
       return 0;
     }

   pac = StAccdbNew();
   pachead = pac;
   plow = StrSave(pName);
   pac->namelow = StrLower(plow);
   res = SearchACCDB(&pac);
   if (res == 0)
    {
       ErrPostEx(SEV_ERROR,0,0, "SHoundFindAcc: Search ACCDB failed.");
       StAccdbFree(pachead);
       return 0;
    }
   if (res == -1)
    {
      /*  ErrPostEx(SEV_ERROR,0,0, "Accession not found in ACCDB."); */
       StAccdbFree(pachead);
       return -1;
    }
   if (res == 1)
      while(pac != NULL)
       {
	 /*  ErrPostEx(SEV_ERROR,0,0, "Accession found in ACCDB."); */
         gi = pac->gi;
         pac = pac->next;
       }

   StAccdbFree(pachead);
   return gi;
}


ValNodePtr LIBCALL SHoundFindNameList(ValNodePtr pvnName)
{
   Int4 gi = 0;
   ValNodePtr pvngi = NULL;


   while(pvnName != NULL)
    {
       gi = SHoundFindName(pvnName->data.ptrvalue);
       if (gi > 0)
         pvngi = Slri_ValNodeAddInt(&pvngi, 0, gi);
       pvnName = pvnName->next;
    }
   Misc_IntVnpUnique(&pvngi);
   return pvngi;
}





CharPtr LIBCALL SHoundAccFromGi(Int4 gi)   /* do not forget to free the return string */
{
   StAccdbPtr pac = NULL, pachead = NULL;
   CharPtr acc = NULL;
   Int2 res = 0;
   
   if (gi <= 0)
     {
       ErrPostEx(SEV_ERROR,0,0, "SHoundAccFromGi: Invalid GI.");
       return NULL;
     }
   pac = StAccdbNew();
   pachead = pac;
   pac->gi = gi;
   res = SearchACCDB(&pac);
   if (res == 0)
    {
       ErrPostEx(SEV_ERROR,0,0, "SHoundAccFromGi: SearchACCDB failed.");
       StAccdbFree(pachead); 
       return NULL;
    }  
   if (res == -1)
    {
      /*  ErrPostEx(SEV_ERROR,0,0, "GI not found in ACCDB."); */
       StAccdbFree(pachead);
       return NULL;
    }  
   if (res == 1){
	   /*this will fix the problem when multiple records match one gi,
		 returns the non "n/a" accession as possible, see bug [4005]*/
	     while (pac!=NULL){
			 acc = StrSave(pac->access);
			 if (acc!=NULL && 0!=StrCmp("n/a", acc)){
				return acc;
			}
			 else {
				 pac=pac->next;
			}
		}
	}
   /* ErrPostEx(SEV_ERROR,0,0, "GI found in ACCDB."); */
   StAccdbFree(pachead);
   return acc;
}


ValNodePtr LIBCALL SHoundAccFromGiList(ValNodePtr pvnGi)
{
   ValNodePtr pvnacc = NULL;
   CharPtr acc = NULL;
   
   while(pvnGi != NULL)
    {
       acc = SHoundAccFromGi(pvnGi->data.intvalue);
       if (acc != NULL)
           pvnacc = Slri_ValNodeAddStr(&pvnacc, 0, acc);
       pvnGi = pvnGi->next;
    }
   Misc_StringVnpUnique(&pvnacc);
   return pvnacc;
}

CharPtr LIBCALL SHoundTitleFromGi(Int4 gi)   /* do not forget to free the return string */
{
   StAccdbPtr pac = NULL, pachead = NULL;
   CharPtr title = NULL;
   Int2 res = 0;
   
   if (gi <= 0)
     {
       ErrPostEx(SEV_ERROR,0,0, "SHoundTitleFromGi: Invalid GI.");
       return NULL;
     }
   pac = StAccdbNew();
   pachead = pac;
   pac->gi = gi;
   res = SearchACCDB(&pac);
   if (res == 0)
    {
       ErrPostEx(SEV_ERROR,0,0, "SHoundTitleFromGi: SearchACCDB failed.");
       StAccdbFree(pachead); 
       return NULL;
    }  
   if (res == -1)
    {
      /*  ErrPostEx(SEV_ERROR,0,0, "GI not found in ACCDB."); */
       StAccdbFree(pachead);
       return NULL;
    }  
   if (res == 1)
         title = StrSave(pac->title);
   /* ErrPostEx(SEV_ERROR,0,0, "GI found in ACCDB."); */
   StAccdbFree(pachead);
   return title;
}


ValNodePtr LIBCALL SHoundTitleFromGiList(ValNodePtr pvnGi)
{
   ValNodePtr pvntitle = NULL;
   CharPtr title = NULL;
   
   while(pvnGi != NULL)
    {
       title = SHoundTitleFromGi(pvnGi->data.intvalue);
       if (title != NULL)
           pvntitle = Slri_ValNodeAddStr(&pvntitle, 0, title);
       pvnGi = pvnGi->next;
    }
   Misc_StringVnpUnique(&pvntitle);
   return pvntitle;
}

SeqIdPtr LIBCALL SHoundSeqIdFromGi(Int4 gi)
{
   StAsndbPtr pas = NULL, pashead = NULL;
   BioseqPtr pbs = NULL; 
   SeqIdPtr psi = NULL, psiNew = NULL, psihead = NULL;
   Int2 res = 0;

   
   if (gi <= 0)
     {
        ErrPostEx(SEV_ERROR,0,0, "SHoundSeqIdFromGi: Invalid GI.");
        return NULL;
     }   
   pas = StAsndbNew();
   pashead = pas;
   pas->gi = gi;
   res = SearchASNDB(&pas);
   if (res == 0)
    {
       ErrPostEx(SEV_ERROR,0,0, "SHoundSeqIdFromGi: SearchASNDB failed.");
       StAsndbFree(pashead);
       return NULL;
    }
   if (res == -1)
    {
      /*  ErrPostEx(SEV_ERROR,0,0, "GI not found in ASNDB."); */
       StAsndbFree(pashead);
       return NULL;
    }
   if (res == 1)
       while(pas != NULL)
        {
	  /* Take the last Bioseq*/
          pbs = pas->asn1;
          pas = pas->next;
        }
 
   /* get SeqIdPtr from the Bioseq and return it */   
   psi = pbs->id;
   if (psi == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "SHoundSeqIdFromGi: NULL Seq-id pointer.");
      StAsndbFree(pashead);
      return NULL;
    }
   psiNew = (SeqIdPtr) AsnIoMemCopy(psi, (AsnReadFunc) SeqIdAsnRead, (AsnWriteFunc) SeqIdAsnWrite);
   if (psiNew == NULL)
     {
       ErrPostEx(SEV_ERROR,0,0, "SHoundSeqIdFromGi: NULL Seq-id pointer. AsnIoMemCopy failed.");
       StAsndbFree(pashead); 
       return NULL;
     }
   psihead = psiNew;
   psi = psi->next;
   while(psi != NULL)
    {
       psiNew->next = (SeqIdPtr) AsnIoMemCopy(psi, (AsnReadFunc) SeqIdAsnRead, (AsnWriteFunc) SeqIdAsnWrite);
       if (psiNew->next == NULL)
         {
           ErrPostEx(SEV_ERROR,0,0, "SHoundSeqIdFromGi: NULL Seq-id pointer. AsnIoMemCopy failed.");
           StAsndbFree(pashead); FreeSeqId(psihead);
           return NULL;
         }
       psiNew = psiNew->next;
       psi = psi->next;
    }
   StAsndbFree(pashead);
   return psihead;
}

ValNodePtr LIBCALL SHoundSeqIdFromGiList(ValNodePtr pvnGi)
{
   ValNodePtr pvnseqid = NULL;
   SeqIdPtr psi = NULL; 
   
   while(pvnGi != NULL)
    {
       psi = SHoundSeqIdFromGi(pvnGi->data.intvalue);
       if (psi != NULL)
           pvnseqid = Slri_ValNodeAddPointer(&pvnseqid, SLRINode_seqid, psi);
       pvnGi = pvnGi->next;
    }
   return pvnseqid;
}


SeqIdPtr  LIBCALL FreeSeqId(SeqIdPtr psi)
{
  SeqIdPtr psiNew = NULL;
   
   while (psi != NULL)
    {
      psiNew = psi->next;
      SeqIdFree(psi);   
      psi = psiNew;
    }
   return (SeqIdPtr) NULL;
}


ValNodePtr LIBCALL FreeVNSeqIdList(ValNodePtr pvnSeqId)
{
   ValNodePtr pvnSeqIdNew = NULL;
   SeqIdPtr psi = NULL;     
      
   while (pvnSeqId != NULL)
    {
       pvnSeqIdNew = pvnSeqId->next;
       psi = (SeqIdPtr) pvnSeqId->data.ptrvalue;
       FreeSeqId(psi);
       pvnSeqId = pvnSeqIdNew;
    }
   return (ValNodePtr) NULL;
}

/******************************************************/
/*  Definition Line                                   */
/******************************************************/


CharPtr LIBCALL SHoundGetDefline(Int4  Gi)
{
  StAccdbPtr pac = NULL, pachead = NULL;
  CharPtr pdefline = NULL, pdb = NULL, pname = NULL, paccess = NULL, pchain = NULL, ptitle = NULL;
  Int2 res = 0;

   if (Gi <= 0)
     {
       ErrPostEx(SEV_ERROR,0,0, "SHoundGetDefline: Invalid GI.");
       return NULL;
     }
   pac = StAccdbNew();
   pachead = pac;
   pac->gi = Gi;
   res = SearchACCDB(&pac);
   if (res == 0)
   {
       ErrPostEx(SEV_ERROR,0,0, "SHoundGetDefline: SearchACCDB failed.");
       StAccdbFree(pachead);
       return NULL;
   }
   if (res == -1)
   {
       StAccdbFree(pachead);
       return NULL;
   }
   if (res == 1)
   {
      if(strcmp(pac->db,"n/a") == 0)
         pdb = "\0";
      else
         pdb = pac->db;
      if(strcmp(pac->name,"n/a") == 0)
         pname = "\0";
      else
         pname = pac->name;
      if(strcmp(pac->access,"n/a") == 0)
         paccess = "\0";
      else
         paccess = pac->access;
      if(strcmp(pac->chain,"n/a") == 0)
         pchain = "\0";
      else
         pchain = pac->chain;
      if(strcmp(IsNullStr(pac->title),"n/a") == 0)
         ptitle = "\0";
      else
         ptitle = pac->title;

      ptitle = CleanCommas(ptitle);
      pdefline = MemNew((size_t)(StringLen(pdb)+StringLen(pname)+StringLen(paccess)+StringLen(pchain)+StringLen(ptitle)+200)*sizeof(Char));

      if(strcmp(pac->access,"n/a") == 0)
        sprintf(pdefline, "%ld|%s| |%s|%s %s", (long) Gi, IsNullStr(pdb), IsNullStr(pname), IsNullStr(pchain), IsNullStr(ptitle));
      else
        sprintf(pdefline, "%ld|%s|%s.%ld|%s|%s %s", (long) Gi, IsNullStr(pdb), IsNullStr(paccess), (long) pac->version, IsNullStr(pname), IsNullStr(pchain), IsNullStr(ptitle));
   }

   StAccdbFree(pachead);
   return pdefline;
}

ValNodePtr LIBCALL SHoundGetDeflineList(ValNodePtr pvnGi)
{
   ValNodePtr pvndef = NULL;
   CharPtr pdef = NULL;

   while(pvnGi != NULL)
    {
       pdef = SHoundGetDefline(pvnGi->data.intvalue);
       if (pdef != NULL)
           pvndef = Slri_ValNodeAddStr(&pvndef, 0, pdef);
       pvnGi = pvnGi->next;
    }
   Misc_StringVnpUnique(&pvndef);
   return pvndef;
}







/******************************************************/
/*  Sequence & Structure Checks                       */
/******************************************************/

/* SHoundMoleculeType takes a gi and returns the type of molecule the gi is.
 * Returns:
 *  0 if Gi is a protein, 1 if Gi is DNA, 2 if Gi is RNA, 
 *  else returns -1.
 *
 *  This function deprecates SHoundIsProtein
 */
CharPtr LIBCALL SHoundMoleculeType(Int4 Gi)
{
   StTaxgiPtr ptg = NULL, ptghead = NULL;
   Int2 res = 0;
   CharPtr ret=NULL;

   if (Gi <= 0)
     {
        ErrPostEx(SEV_ERROR,0,0, " SHoundMoleculeType: Invalid GI %d",(int)Gi);
        goto end;
     }

   ptg = StTaxgiNew();
   ptghead = ptg;
   ptg->gi = Gi;
   res = SearchTAXGI(&ptg);
   /* Return value 0 or -1 means unsuccessful search */
   if (res == 0 || res == -1)
    {
       ErrPostEx(SEV_ERROR,0,0, "[SHoundMoleculeType]: SearchTAXGI failed %d.", ret);
       goto end;
    }
    else 
    {
      if(ptg->type == NULL) 
      { 
         ErrPostEx(SEV_ERROR, 0, 0, "[SHoundMoleculeType]: Molecule type missing.");
         goto end; 
      }
      if(strstr(ptg->type, "protein") != NULL){ ret = PROTEIN; }  /* protein == 0 */
      else if(strstr(ptg->type, "DNA") != NULL) { ret = DNA; }         /* DNA == 1     */
      else if(strstr(ptg->type, "RNA") != NULL) { ret = RNA; }        /* RNA  == 2    */
      else { 
         ErrPostEx(SEV_WARNING, 0, 0, "[SHoundMoleculeType]: Molecule not a protein, DNA or RNA. %s.", IsNullStr(ptg->type));
       goto end; 
      }

    }
  StTaxgiFree(ptghead);
  return ret; 
  
end:
  ErrPostEx(SEV_ERROR, 0, 0, "[SHoundMoleculeType]: Error on gi = %d", Gi);
  StTaxgiFree(ptghead);
  return NULL;
}

/*The code inside this function has been completed revamped to
  Speed up search. Instead of searching the table PARTI and ASNDB
  for molecule type, the API now get molecule type from the table
  TAXGI. */
Boolean LIBCALL SHoundIsProtein(Int4 Gi)

{
   StTaxgiPtr ptg = NULL, ptghead = NULL;
   Int2 res = 0;

   if (Gi <= 0)
     {
        ErrPostEx(SEV_ERROR,0,0, " SHoundIsProtein: Invalid GI %d",(int)Gi);
        return FALSE;
     }

   ptg = StTaxgiNew();
   ptghead = ptg;
   ptg->gi = Gi;
   res = SearchTAXGI(&ptg);
   if (res == 0)
    {
       ErrPostEx(SEV_ERROR,0,0, " SHoundIsProtein: SearchTAXGI failed.");
       StTaxgiFree(ptghead);
       return FALSE;
    }
   if (res == -1)
    {
       StTaxgiFree(ptghead);
       return FALSE;
    }
   if (res == 1)
    {
      if(strstr(ptg->type, "protein") != NULL)
       {
         StTaxgiFree(ptghead);
         return TRUE;
       } 
      else
      {
             
         StTaxgiFree(ptghead);
         return FALSE;
      }
    }
   StTaxgiFree(ptghead);
   return FALSE;
}



Boolean  LIBCALL SHoundIsNRFirst(Int4 Gi)
{
   StRedundPtr prd = NULL;
   Int2 res = 0;

   if(!SHoundModule("redundb"))
   {
     ErrPostEx(SEV_ERROR,0,0, "SHoundIsNRFirst: Missing required SeqHound module.");
     return FALSE;
   }


   if (Gi <= 0)
     {
       ErrPostEx(SEV_ERROR,0,0, "SHoundExists: Invalid GI.");
       return FALSE;
     }

   prd = StRedundNew();
   prd->gi = Gi;
   res = SearchREDUND(&prd);
   if (res == 0)
     {
       ErrPostEx(SEV_ERROR,0,0, "SearchREDUND failed.");
       StRedundFree(prd);
       return FALSE;
     }
    if (res == -1)
     {
       StRedundFree(prd);
       return FALSE;
     }
    if (res == 1)
     {
       if (prd->ordinal == 1)
        {
          StRedundFree(prd);
          return TRUE;
        }
       else
        {
          StRedundFree(prd);
          return FALSE;
        }
     }
  StRedundFree(prd);
  return FALSE;
}
/* returns whether a Gi is the preferred NR version of a number of identical seqs */



Boolean  LIBCALL SHoundExists(Int4 Gi)
{
   StAsndbPtr pas = NULL, pashead = NULL;
   Int2 res = 0;

   if (Gi <= 0)
     {
       ErrPostEx(SEV_ERROR,0,0, "SHoundExists: Invalid GI.");
       return FALSE;
     }

   pas = StAsndbNew();
   pashead = pas;
   pas->gi = Gi;
   res = SearchASNDB (&pas);
    if (res == 0)
     {
       ErrPostEx(SEV_ERROR,0,0, "SHoundExists: SearchASNDB failed.");
       StAsndbFree(pashead); 
       return FALSE;
     }
    if (res == -1)
     {  
       StAsndbFree(pashead); 
       return FALSE;
     } 
    if (res == 1)
     {
       StAsndbFree(pashead); 
       return TRUE;           
     }
    StAsndbFree(pashead);
    return FALSE;
}
 
#ifndef ODBC
Int4 SHoundGetLargestMMDBID(void)
{
   FILE *fp = NULL;
   Char path[ARR_LEN] = {'\0'};
   CharPtr data = NULL;
   Int4 mmdbid, size = ARR_LEN;

   mmdbid = 0 ;
   if(!GetAppParam("intrez", "datab", "pathmm", NULL, path, ARR_LEN - 1))
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[SHoundGetLargestMMDBID]: Cannot read config file.");
      goto end;
   }
   StringCat(path, MMDBID_LARGEST);
   data = (char*) malloc((ARR_LEN - 1) * sizeof(Char));
   if( !data || !(fp = FileOpen(path, "r")) || !(data = fgets(data, size, fp)) )
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[SHoundGetLargestMMDBID]: Cannot get mmdbid from %s.", path);
      goto end;
   }
   mmdbid = atoi(data);
end:
   free(data);
   fclose(fp);
   return mmdbid;
}
#else
Int4 SHoundGetLargestMMDBID(void)
{
   return GetLargestMmdbid();
}
#endif

Int4 LIBCALL SHound3DbWhat(Int4 mmdbid)
{
   StMmdbPtr pmd = NULL, pmdhead = NULL;
   Int2 res = 0;
   Int4 what = 0;

   if(!SHoundModule("strucdb"))
   {
     ErrPostEx(SEV_ERROR,0,0, "SHound3DbWhat: Missing required SeqHound module.");
     return 0;
   }

   if(mmdbid <= 0)
    {
       ErrPostEx(SEV_ERROR,0,0, "SHound3DbWhat: Invalid MmdbId.");
       return 0;
    }
   pmd = StMmdbNew();
   pmdhead = pmd;
   pmd->mmdbid = mmdbid;
   res = SearchMMDB (&pmd);
    if (res == 0)
     {
       ErrPostEx(SEV_ERROR,0,0, "SHound3DbWhat: SeekMMDB failed.");
       StMmdbFree(pmdhead);
       return 0;
     }
    if (res == -1)
     {
       StMmdbFree(pmdhead);
       return 0;
     }
    if (res == 1)
     {
       what = pmd->bwhat;
       StMmdbFree(pmdhead);
       return what;
     }
   StMmdbFree(pmdhead);
   return 0;
}
/*  Returns the bWhat byte of a 3-D structure - see mmdbapi1.h for value masks */
/*  This byte indicates the contents of a 3-D structure (DNA,RNA,PROT,HET,Ion, Solvent) */


Boolean LIBCALL SHound3DExists(Int4 mmdbid)
{
   StMmgiPtr pmg = NULL, pmghead = NULL;
   Int2 res = 0;

   if(!SHoundModule("strucdb"))
   {
     ErrPostEx(SEV_ERROR,0,0, "SHound3DExists: Missing required SeqHound module.");
     return FALSE;
   }

   if(mmdbid <= 0)
    {
      ErrPostEx(SEV_ERROR,0,0, "SHound3DExists: Invalid MmdbId.");
      return FALSE;
    }

   pmg = StMmgiNew();
   pmghead = pmg;
   pmg->mmdbid = mmdbid;
   res = SearchMMGI(&pmg);
    if (res == 0)
     {
       ErrPostEx(SEV_ERROR,0,0, "SHound3DExists: SeekMMGI failed.");
       StMmgiFree(pmghead);
       return FALSE;
     }
    if (res == -1)
     {
       StMmgiFree(pmghead);
       return FALSE;
     }
    if (res == 1)
     {
       StMmgiFree(pmghead);
       return TRUE;
     }
   StMmgiFree(pmghead);
   return FALSE;
}


/*
Boolean LIBCALL SHound3DExists(Int4 mmdbid)
{
   StMmdbPtr pmd = NULL, pmdhead = NULL;
   Int2 res = 0;

   if(mmdbid <= 0)
     ErrPostEx(SEV_ERROR,0,0, "SHound3DExists: Invalid MmdbId.");

   pmd = StMmdbNew();
   pmdhead = pmd;
   pmd->mmdbid = mmdbid;
   res = SearchMMDB (&pmd);
    if (res == 0)
     {
       ErrPostEx(SEV_ERROR,0,0, "SeekMMDB failed.");
       StMmdbFree(pmdhead);
       return FALSE;
     }
    if (res == -1)
     {
       StMmdbFree(pmdhead);
       return FALSE;
     }
    if (res == 1)
     {
       StMmdbFree(pmdhead);
       return TRUE;
     }
}
*/
/* TRUE if the mmdbid is in SHound */


/* returns the length of the sequence */
Int4 LIBCALL SHoundSequenceLength(Int4 Gi)
{
  BioseqPtr pbs = NULL;
  Int4 iLength = -1;
  CharPtr pSeq = NULL;
  if(Gi <= 0)
  {
    ErrPostEx(SEV_ERROR, 0, 0, "[SHoundSequenceLength]: Invalid Gi.");
    goto end;
  }
  /* use the bioseq */
  pbs = SHoundGetBioseq(Gi);
  if(pbs == NULL)
  {
     ErrPostEx(SEV_INFO, 0, 0, "[SHoundSequenceLength]: NULL Bioseq.");
     goto end;
  }else{
     /* got a bioseq */
     /* length is optional, and may not be set */
     if( (iLength = pbs->length) == -1 )  
     {  /* length not set, so get the sequence and
	   calculate the length ourself */ 
        pSeq = BSStringSave(pbs->seq_data);
	iLength = StringLen(pSeq);
        goto end;	
     }else {
	/* length is set, so wrapping up */
        goto end;
     }
  }

end:
  if(pSeq) MemFree(pSeq);
  if(pbs)  BioseqFree(pbs);
  return iLength;
}


/******************************************************/
/*  Sequence Fetch  - Bioseq                          */
/******************************************************/

BioseqPtr  LIBCALL SHoundGetBioseq(Int4 Gi)
{
   StAsndbPtr pas = NULL, pashead = NULL;
   BioseqPtr pbs = NULL;
   Int2 res = 0;


   if (Gi <= 0)
     {
       ErrPostEx(SEV_ERROR,0,0, "SHoundGetBioseq: Invalid GI.");
       return NULL;
     }

   pas = StAsndbNew();
   pashead = pas;
   pas->gi = Gi;
   res = SearchASNDB (&pas);
    if (res == 0)
     {
       ErrPostEx(SEV_ERROR,0,0, "SHoundGetBioseq: SearchASNDB failed.");
       StAsndbFree(pashead); 
       return NULL;
     }
    if (res == -1)
     {  
       /*  ErrPostEx(SEV_ERROR,0,0, "GI not found in ASNDB."); */
       StAsndbFree(pashead); 
       return NULL;
     } 
    if (res == 1)
     {
       if (pas->asn1 == NULL)
	 {
           ErrPostEx(SEV_ERROR,0,0, "SHoundGetBioseq: SearchASNDB failed; Bioseq is NULL.");
           StAsndbFree(pashead); 
           return NULL;
         }
       else
	{
          pbs = (BioseqPtr) AsnIoMemCopy (pas->asn1, (AsnReadFunc) BioseqAsnRead,  (AsnWriteFunc) BioseqAsnWrite);
          if (pbs == NULL)
	   {
              ErrPostEx(SEV_ERROR,0,0, "SHoundGetBioseq: SearchASNDB failed. AsnIoMemCopy failed.");
              StAsndbFree(pashead);     
              return NULL;
           }
          StAsndbFree(pashead); 
          return pbs;
        }
      }
   StAsndbFree(pashead);
   return NULL;
}  


ValNodePtr LIBCALL SHoundGetBioseqList(ValNodePtr pvnGi)
{
    ValNodePtr pvnBioseq = NULL;
    BioseqPtr pbs = NULL;

    while (pvnGi != NULL)
     {
       pbs = SHoundGetBioseq(pvnGi->data.intvalue);
       if (pbs != NULL)
           pvnBioseq = Slri_ValNodeAddPointer(&pvnBioseq, SLRINode_bioseq, pbs);
       pvnGi = pvnGi->next;
     }
    return pvnBioseq;
}


ValNodePtr LIBCALL FreeBioseqList(ValNodePtr pvnBioseq)
{
   ValNodePtr pvnBioseqNew = NULL;
   ValNodePtr pvnhead = NULL;

   pvnhead = pvnBioseq;
   while (pvnBioseq != NULL)
     {
       pvnBioseqNew = pvnBioseq->next;
       BioseqFree(pvnBioseq->data.ptrvalue);
       pvnBioseq = pvnBioseqNew;
     }
   ValNodeFree(pvnhead);
   return (ValNodePtr) NULL;
}



/******************************************************/
/*  Sequence Fetch  - BioseqSet                          */
/******************************************************/

BioseqSetPtr  LIBCALL SHoundGetBioseqSet(Int4 Gi)
{
   StSendbPtr psd = NULL, psdhead = NULL;
   Int2 res = 0;
   SeqEntryPtr pse = NULL;

   if (Gi <= 0)
     {
       ErrPostEx(SEV_ERROR,0,0, "SHoundGetBioseq: Invalid GI.");
       return NULL;
     }

   psd = StSendbNew();
   psdhead = psd;
   res = SearchSENDBforGI(&psd, Gi);
   if (res == 0)
     {
       ErrPostEx(SEV_ERROR,0,0, "SHoundGetBioseq: SearchASNDB failed.");
       StSendbFree(psdhead);
       return NULL;
     }
    if (res == -1)
     {
       StSendbFree(psdhead);
       return NULL;
     }
    if (res == 1)
     {
       if (psd->asn1 == NULL)
	   {
           ErrPostEx(SEV_ERROR,0,0, "SHoundGetBioseq: SearchASNDB failed; Bioseq is NULL.");
           StSendbFree(psdhead);
           return NULL;
       }
       else
	   {
		 if (psd->asn1 != NULL)
		   pse = (SeqEntryPtr) AsnIoMemCopy (psd->asn1, (AsnReadFunc) SeqEntryAsnRead,  (AsnWriteFunc) SeqEntryAsnWrite);
		 if (pse == NULL)
		 {
		   ErrPostEx(SEV_ERROR,0,0, "SHoundGetBioseq: SearchASNDB failed. AsnIoMemCopy failed.");
		   StSendbFree(psdhead);
		   return NULL;
		 }

		 if (pse->choice == 2)
		 {
		   StSendbFree(psdhead);
		   return (BioseqSetPtr) pse->data.ptrvalue;
		 }
		 else{
		   ErrPostEx(SEV_ERROR,0,0, "No bioseqset associated with the GI.");
		   SeqEntryFree(pse);
		 }
        }
      }
   StSendbFree(psdhead);
   return NULL;
}


ValNodePtr LIBCALL SHoundGetBioseqSetList(ValNodePtr pvnGi)
{
    ValNodePtr pvnBioseqSet = NULL;
    BioseqSetPtr pbss = NULL;

    while (pvnGi != NULL)
     {
       pbss = SHoundGetBioseqSet(pvnGi->data.intvalue);
       if (pbss != NULL)
           pvnBioseqSet = Slri_ValNodeAddPointer(&pvnBioseqSet, SLRINode_bioseqset, pbss);
       pvnGi = pvnGi->next;
     }
    return pvnBioseqSet;
}

ValNodePtr LIBCALL FreeBioseqSetList(ValNodePtr pvnBioseqSet)
{
   ValNodePtr pvnBioseqSetNew = NULL;
   ValNodePtr pvnhead = NULL;

   pvnhead = pvnBioseqSet;
   while (pvnBioseqSet != NULL)
     {
       pvnBioseqSetNew = pvnBioseqSet->next;
       BioseqSetFree(pvnBioseqSet->data.ptrvalue);
       pvnBioseqSet = pvnBioseqSetNew;
     }
   ValNodeFree(pvnhead);
   return (ValNodePtr) NULL;
}



/******************************************************/
/*  Sequence Fetch  - SeqEntry                        */
/******************************************************/

SeqEntryPtr  LIBCALL SHoundGetSeqEntry ( Int4 Gi )
{
   StSendbPtr      psd = NULL, 
               psdhead = NULL;
   SeqEntryPtr     pse = NULL;
   Int2 res            = 0;

   if ( Gi <= 0 )
   {
       ErrPostEx ( SEV_ERROR, 0, 0, "[SHoundGetSeqEntry]: Invalid GI." );
       return NULL;
   }

   psd = StSendbNew();
   psdhead = psd;

   res = SearchSENDBforGI ( &psd, Gi );
   if ( 0 == res )
   {
       ErrPostEx( SEV_ERROR, 0, 0, "[SHoundGetSeqEntry]: SearchASNDB failed." );
       StSendbFree(psdhead);
       return NULL;
   }

   else if ( -1 == res )
   {
       StSendbFree(psdhead);
       return NULL;
   }

   else if ( 1 == res )
   {
       if ( NULL == psd->asn1 )  /* seqentry null */
       {
           ErrPostEx ( SEV_ERROR,0,0, "[SHoundGetSeqEntry]: SearchASNDB failed. Bioseq is NULL." );
           StSendbFree(psdhead);
           return NULL;

       } else {  /* seqentry filled in */

          pse = ( SeqEntryPtr ) AsnIoMemCopy ( psd->asn1, 
                                               (AsnReadFunc) SeqEntryAsnRead,
                                               (AsnWriteFunc) SeqEntryAsnWrite );
	  if ( NULL == pse ) 
	  {
             ErrPostEx ( SEV_ERROR, 0, 0, "[SHoundGetSeqEntry]: AsnIoMemCopy failed." );
             StSendbFree ( psdhead );
             return NULL;
	  }
	  StSendbFree ( psdhead );
	  return pse;
      }
   }
   StSendbFree(psdhead);
   return NULL;
}


SeqEntryPtr  LIBCALL SHoundGetSeqEntryforTindex(Int4 Gi)
{
   StSendbPtr psd = NULL, psdhead = NULL;
   SeqEntryPtr pse = NULL;
   Int2 res = 0;

   if (Gi <= 0)
     {
       ErrPostEx(SEV_ERROR,0,0, "SHoundGetBioseq: Invalid GI.");
       return NULL;
     }

   psd = StSendbNew();
   psdhead = psd;
   res = SearchSENDBforGIforTindex(&psd, Gi);
   if (res == 0)
     {
       ErrPostEx(SEV_ERROR,0,0, "SHoundGetBioseq: SearchASNDB failed.");
       StSendbFree(psdhead);
       return NULL;
     }
    if (res == -1)
     {
       /*ErrPostEx(SEV_ERROR,0,0, "GI not found in ASNDB.");*/
       StSendbFree(psdhead);
       return NULL;
     }
    if (res == 1)
     {
       if (psd->asn1 == NULL)
	 {
           ErrPostEx(SEV_ERROR,0,0, "SHoundGetBioseq: SearchASNDB failed; Bioseq is NULL.");
           StSendbFree(psdhead);
           return NULL;
         }
       else
	{
	  if (psd->asn1 != NULL)
            /*pse = (SeqEntryPtr) AsnIoMemCopy (psd->asn1, (AsnReadFunc) SeqEntryAsnRead,  (AsnWriteFunc) SeqEntryAsnWrite);*/
          {
             pse = psd->asn1;
             psd->asn1 = NULL;
          }
          if (pse == NULL)
	   {
              ErrPostEx(SEV_ERROR,0,0, "SHoundGetBioseq: SearchASNDB failed. AsnIoMemCopy failed.");
              StSendbFree(psdhead);
              return NULL;
           }
          StSendbFree(psdhead);
          return pse;

        }
      }
   StSendbFree(psdhead);
   return NULL;
}


ValNodePtr LIBCALL SHoundGetSeqEntryList(ValNodePtr pvnGi)
{
    ValNodePtr pvnSeqEntry = NULL;
    SeqEntryPtr pse = NULL;

    while (pvnGi != NULL)
     {
       pse = SHoundGetSeqEntry(pvnGi->data.intvalue);
       if (pse != NULL)
           pvnSeqEntry = Slri_ValNodeAddPointer(&pvnSeqEntry, SLRINode_seqentry, pse);
       pvnGi = pvnGi->next;
     }
    return pvnSeqEntry;
}


ValNodePtr LIBCALL FreeSeqEntryList(ValNodePtr pvnSeqEntry)
{
   ValNodePtr pvnSeqEntryNew = NULL;
   ValNodePtr pvnhead = NULL;

   pvnhead = pvnSeqEntry;
   while (pvnSeqEntry != NULL)
     {
       pvnSeqEntryNew = pvnSeqEntry->next;
       SeqEntryFree(pvnSeqEntry->data.ptrvalue);
       pvnSeqEntry = pvnSeqEntryNew;
     }
   ValNodeFree(pvnhead);
   return (ValNodePtr) NULL;
}




/******************************************************/
/*  Sequence Fetch  - FASTA                           */
/******************************************************/


/**************************************
* Doron - June 7, 2000 19:40
* The following block (SLRIBioseqToFasta, SLRIBioseqToFastaX and FastaMemFunc
* were added so the function SHoundGetFasta will not read fasta structures from disk
* as the standard NCBI functions do.
* added na_order and aa_order. copied from tofasta.c  NCBI file  (not defined in tofasta.h file)
* for compiling with the new modefied SLRIBioseqToFastaX function.
**************************************/


#define PATENT_ORDER 110
#define FASTA_BUFFER_LEN 25000

static Uint1 na_order[NUM_SEQID] = {   /* order of nucleic acid deflines */
        255, /* 0 = not set */
        230, /* 1 = local Object-id */
        30,  /* 2 = gibbsq */
        30,  /* 3 = gibbmt */
        255, /* 4 = giim Giimport-id */
        20, /* 5 = genbank */
        20, /* 6 = embl */
        255, /* 7 = pir */
        255, /* 8 = swissprot */
        40,  /* 9 = patent */
        15, /* 10 = other TextSeqId (RefGene) */
        50, /* 11 = general Dbtag */
        120,  /* 12 = gi */
        20, /* 13 = ddbj */
        255, /* 14 = prf */
        30  /* 15 = pdb */
    };

static Uint1 aa_order[NUM_SEQID] = {   /* order of nucleic acid deflines */
        255, /* 0 = not set */
        230, /* 1 = local Object-id */
        40,  /* 2 = gibbsq */
        40,  /* 3 = gibbmt */
        255, /* 4 = giim Giimport-id */
        60, /* 5 = genbank */
        60, /* 6 = embl */
        30, /* 7 = pir */
        20, /* 8 = swissprot */
        80,  /* 9 = patent */
        15, /* 10 = other TextSeqId (RefGene) */
        90, /* 11 = general Dbtag */
        120,  /* 12 = gi */
        60, /* 13 = ddbj */
        70, /* 14 = prf */
        50  /* 15 = pdb */
    };


static Boolean LIBCALL SLRIBioseqToFasta (BioseqPtr bsp, SLRIFastaPtr fasta_ptr, Boolean is_na);
static Boolean LIBCALL SLRIBioseqToFastaX (BioseqPtr bsp, MyFsaPtr mfp, Boolean is_na);
static Boolean LIBCALL FastaMemFunc (BioseqPtr bsp, Int2 key, CharPtr buf,
                                  Uint4 buflen, ByteStorePtr data);

/***************************************
*   Boolean BioseqToFasta(bsp, fp, is_na)
* Initializes a MyFsa struct and calls
* SLRIbioseqToFastaX to get the fasta record.
***************************************/
static Boolean LIBCALL SLRIBioseqToFasta (BioseqPtr bsp, SLRIFastaPtr fasta_ptr, Boolean is_na)

{
	MyFsa mfa;
	Char buf[FASTA_BUFFER_LEN+1];

    if (bsp == NULL)
        return FALSE;

	mfa.buf = buf;
	mfa.buflen = FASTA_BUFFER_LEN;
	mfa.seqlen = 80;
	mfa.mydata = (Pointer)fasta_ptr;
	mfa.myfunc = NULL; /*change this*/
	mfa.bad_asn1 = FALSE;
	mfa.order = 0;
	mfa.accession = NULL;
	mfa.organism = NULL;
	mfa.do_virtual = FALSE;
	mfa.tech = 0;
        mfa.no_sequence = FALSE;
    	mfa.formatdb	= FALSE;

	return SLRIBioseqToFastaX(bsp, &mfa, is_na);
}

/***********************************************
*
*   Boolean SLRIBioseqToFastaX(bsp, mfp, is_na)
*
**********************************************/
static Boolean LIBCALL SLRIBioseqToFastaX (BioseqPtr bsp, MyFsaPtr mfp, Boolean is_na)

{
    SeqPortPtr spp;
	/*Uint1 repr;*/
    Uint1 code;
    Char buf[41];
    SeqIdPtr sip;
    Uint1 order = 255;
    Boolean is_patent = FALSE, is_genbank = FALSE;
    Uint1Ptr order_array;
    int i;
    CharPtr organism = NULL;
    SLRIFastaPtr slri_fasta=NULL;

    /*ByteStore variables*/
    static Int2 charsPerLine=60;
    ByteStorePtr bsp_defline=NULL;
    ByteStorePtr bsp_sequence=NULL;

    if ((bsp == NULL) || (mfp == NULL))
        return FALSE;

    /*repr = Bioseq_repr(bsp);*/

    if (ISA_na(bsp->mol))
    {
        if (! is_na)
            return FALSE;
		order_array = na_order;
    }
    else if (ISA_aa(bsp->mol))
    {
		if (is_na)
			return FALSE;
		order_array = aa_order;
		if (mfp->accession != NULL)		   /* translated genbank */
		{
			order = order_array[SEQID_GENBANK];
			is_genbank = TRUE;
			organism = mfp->organism;
		}
    }
    else
    {
	   buf[0] = '\0';
		SeqIdWrite(bsp->id, buf, PRINTID_FASTA_LONG, 40);
	ErrPostEx(SEV_ERROR,0,0,"ToFasta: [%s] Unrecognized bsp->mol = %d",
		  IsNullStr(buf), (int)(bsp->mol));
	mfp->bad_asn1 = TRUE;
	return FALSE;
    }

	mfp->bsp = bsp;

	for (sip = bsp->id; sip != NULL; sip = sip->next)
	{
		i=(int)(sip->choice);
		if (! is_genbank)    /* don't change order for translated genbank */
		{
			if (order_array[i] < order)
				order = order_array[i];
		}
		if (i == (int)SEQID_PATENT)
			is_patent = TRUE;
		else if (i == (int)SEQID_PRF)
			organism = mfp->organism;
	}

	if (is_patent)
		order = PATENT_ORDER;

	mfp->order = order;

	switch (mfp->tech)
	{
		case MI_TECH_est:
		case MI_TECH_sts:
		case MI_TECH_survey:
		case MI_TECH_htgs_1:
		case MI_TECH_htgs_2:
		case MI_TECH_htgs_3:
			organism = mfp->organism;
			break;
		default:
			break;
	}
         /*Initialize the ByteStore pointers
         	* Instead of writing out to file writes into a bytestore*/
	bsp_defline=BSNew(charsPerLine);
	bsp_sequence=BSNew(charsPerLine);
	BSSeek(bsp_defline, 0L, SEEK_SET);
	BSSeek(bsp_sequence, 0L, SEEK_SET);
	
	if (! FastaId(bsp, mfp->buf, mfp->buflen))

        {
                BSFree(bsp_defline);
                BSFree(bsp_sequence);
		return FALSE;
        }

               FastaMemFunc(bsp,FASTA_ID, mfp->buf, StringLen(mfp->buf), bsp_defline);
   	if (! CreateDefLine(NULL, bsp, mfp->buf, mfp->buflen, mfp->tech, mfp->accession, organism))
        {
                BSFree(bsp_defline);
                BSFree(bsp_sequence);
		return FALSE;
        }

            FastaMemFunc(bsp, FASTA_DEFLINE, mfp->buf, StringLen(mfp->buf), bsp_defline);
        if (mfp->formatdb && is_na) {
            FastaMemFunc(bsp, FASTA_FORMATDB_AMB, mfp->buf, StringLen(mfp->buf), bsp_defline);
        }
        else if(!mfp->no_sequence) {
	    if (!mfp->formatdb) {
		if (is_na)
		    code = Seq_code_iupacna;
		else
		    code = Seq_code_ncbieaa;
	    } else {
		code = mfp->code;
	    }

            spp = FastaSeqPort(bsp, is_na, mfp->do_virtual, code);
            if (spp == NULL) return FALSE;
            
            while (FastaSeqLine(spp, mfp->buf, mfp->seqlen, is_na))
                FastaMemFunc(bsp, FASTA_SEQLINE, mfp->buf, StringLen(mfp->buf),
                                 bsp_sequence);

            SeqPortFree(spp);

            FastaMemFunc(bsp, FASTA_EOS, mfp->buf, StringLen(mfp->buf),
                             bsp_sequence);
        }
        /*Finally, fill in SLRIfasta structure*/
        slri_fasta=(SLRIFastaPtr)mfp->mydata;
        slri_fasta->defline=(CharPtr)BSStringSave(bsp_defline);
        BSFree(bsp_defline);
        slri_fasta->sequence=(CharPtr)BSStringSave(bsp_sequence);
        BSFree(bsp_sequence);
        return TRUE;
}

 /*****************************************************************************
*
*   FastaMemFunc(key, buf, data)
*   	 callback function that will fill up an SLRIFasta structure.
*
*****************************************************************************/
static Boolean LIBCALL FastaMemFunc (BioseqPtr bsp, Int2 key, CharPtr buf,
                                  Uint4 buflen, ByteStorePtr data)
{
	char space[1]=" ";
	
	switch (key)
	{
		case FASTA_ID:
			{
				if((BSWrite(data, buf, (long)buflen))<(long)buflen)
				   {
					  ErrPostEx(SEV_ERROR,2,1,"FastaMemFunc: BSWrite failed.");
					  return FALSE;
					}
				BSWrite(data, space, sizeof(space));
			}
			break;
		
		case FASTA_DEFLINE:
			
			if((BSWrite(data, buf, (long)buflen))<(long)buflen)
			 {
			   ErrPostEx(SEV_ERROR,2,1,"FastaMemFunc: BSWrite failed.");
			   return FALSE;
			 }	
			break;
		
		case FASTA_SEQLINE:
			
			if((BSWrite(data, buf, (long)buflen))<(long)buflen)
			 {
			   ErrPostEx(SEV_ERROR,2,1,"FastaMemFunc: BSWrite failed.");
			   return FALSE;
			 }
			break;
		
		case FASTA_EOS:   /* end of sequence */
			break;
		default:
			break;
	}
	return TRUE;
}


ValNodePtr LIBCALL FreeFastaList(ValNodePtr pvnFasta)
{

   ValNodePtr pvnFastaNew = NULL;
   ValNodePtr pvnhead = NULL;

   pvnhead = pvnFasta;
   while (pvnFasta != NULL)
     {
       pvnFastaNew = pvnFasta->next;
       SLRIFastaFree(pvnFasta->data.ptrvalue);
       pvnFasta = pvnFastaNew;
     }
   ValNodeFree(pvnhead);
   return (ValNodePtr) NULL;
}
/* returns a SLRIFastaPtr, or a ValNode list of SLRIFastaPtrs */
/* client server implementation requests ASN.1 and            */
/* the client converts these to SLRIFastaPtr                  */


 SLRIFastaPtr LIBCALL SHoundGetFasta(Int4 Gi)
{
    StAsndbPtr pas = NULL, pashead = NULL;
    Int2 res = 0;
    BioseqPtr pbs = NULL;
    SLRIFastaPtr pfa = NULL;


   if (Gi <= 0)
     {
       ErrPostEx(SEV_ERROR,0,0, "SHoundGetFasta: Invalid GI.");
       return NULL;
     }

    pfa = SLRIFastaNew();
    pas = StAsndbNew();
    pashead = pas;
    pas->gi = Gi;
    res = SearchASNDB(&pas);
    if (res == 0)
     {
       ErrPostEx(SEV_ERROR,0,0, " SHoundGetFasta: SearchASNDB failed.");
       StAsndbFree(pashead); 
       SLRIFastaFree(pfa);
       return NULL;
     }
    if (res == -1)
     {
       StAsndbFree(pashead); 
       SLRIFastaFree(pfa);
       return NULL;
     }

    pbs = pas->asn1;
    if (pbs == NULL)
      {
         ErrPostEx(SEV_ERROR,0,0, "SHoundGetFasta: BioseqPtr is NULL.");
         StAsndbFree(pashead);
	 SLRIFastaFree(pfa);
         return NULL;
      }

    if (strstr(pas->type, "protein") != NULL)
     {
       /*Doron-June 8, 2000
     	* changed BioseqToFasta to SLRIBioseqToFasta call
     	****/
       if (!SLRIBioseqToFasta(pbs, pfa, FALSE))
        {
          ErrPostEx(SEV_ERROR,0,0, "SHoundGetFasta: BioseqToFasta failed.");
          StAsndbFree(pashead);
	  SLRIFastaFree(pfa);
          return NULL;
        }
     }
    else if (strstr(pas->type, "NA") != NULL)
     {
        /*Doron-June 8, 2000
     	* changed BioseqToFasta to SLRIBioseqToFasta call
     	****/
       if (!SLRIBioseqToFasta(pbs, pfa, TRUE))
        {
          ErrPostEx(SEV_ERROR,0,0, "SHoundGetFasta: BioseqToFasta failed.");
          StAsndbFree(pashead);
	  SLRIFastaFree(pfa);
          return NULL;
        }
     }


    /************************************
    * Doron- July 19, 2000
    * Opening the file and filling SLRIfasta
    * structure was removed from here
    * All this is done by SLRIBioseqToFasta
    *************************************/
    StAsndbFree(pashead);
    return pfa;
}


ValNodePtr LIBCALL SHoundGetFastaList(ValNodePtr pvnGi)
{
   ValNodePtr pvnFasta = NULL;
   SLRIFastaPtr pfa = NULL;

   while (pvnGi != NULL)
     {
        pfa = SHoundGetFasta(pvnGi->data.intvalue);
        if (pfa != NULL)
           pvnFasta = Slri_ValNodeAddPointer(&pvnFasta, SLRINode_fasta, pfa);
        pvnGi = pvnGi->next;
     }
   return pvnFasta;
}





/* chops up a sequence string into lines of length linelen and writes it out in a file pfile */	
Boolean LIBCALL PrintSLRIFastaSequence(CharPtr pseq, Int4 linelen, FILE *pfile) 
{
  CharPtr ptemp = NULL;
  Int4 count = 0;
  
  
  if(pseq == NULL)
    {
      ErrPostEx(SEV_ERROR,2,1,"PrintSLRIFastaSequence:Invalid parameter.");
      return FALSE;
    }
  ptemp = pseq;
  
  while(ptemp[0] != '\0')
    {
      fprintf(pfile, "%c", ptemp[0]);
      count++;
      if(count == linelen)
        {
          fprintf(pfile, "\n");
          count = 0;
        }
      ptemp++;
    }
  fprintf(pfile, "\n");
  
  
  return TRUE;
}


Boolean LIBCALL PrintSLRIFasta(SLRIFastaPtr pfa, Int4 linelen, FILE *pfile)
{

  if((pfa == NULL)|| (pfile == NULL))
   {
      ErrPostEx(SEV_ERROR,2,1,"PrintSLRIFasta:Invalid parameter.");
      return FALSE;
   }
  if(linelen == 0) linelen = 80;

  if((pfa->defline != NULL) && (pfa->sequence == NULL))
   {
     fprintf(pfile, ">%s\n", IsNullStr(pfa->defline));
   }
  if((pfa->defline != NULL) && (pfa->sequence != NULL))
   {
     fprintf(pfile, ">%s\n", IsNullStr(pfa->defline));
     if(!PrintSLRIFastaSequence(pfa->sequence, linelen, pfile))
      {
         ErrPostEx(SEV_ERROR,2,1,"PrintSLRIFasta:PrintSLRIFastaSequence failed.");
         return FALSE;
      }
   }
  return TRUE;
}


Boolean LIBCALL PrintSLRIFastaList (ValNodePtr pvnfa, Int4 linelen, FILE *pfile)	
{

  SLRIFastaPtr pfa = NULL;

  if((pvnfa == NULL)|| (pfile == NULL))
   {
      ErrPostEx(SEV_ERROR,2,1,"PrintSLRIFasta:Invalid parameter.");
      return FALSE;
   }
  if(linelen == 0) linelen = 80;


  while(pvnfa != NULL)
  {
      pfa = (SLRIFastaPtr) pvnfa->data.ptrvalue;
      PrintSLRIFasta(pfa, linelen, pfile);
      pvnfa = pvnfa->next;
   }
  return TRUE;

}
	

/******************************************************/
/*  Sequence fetch - GenBank flat file                */
/******************************************************/
/* New implementation of SHoundGetGenBankff.
 * This function implements new NCBI function SeqEntryToGnbk()
 */
Boolean LIBCALL SHoundGetGenBankff(Int4 gi, FILE *pfile)
{
    SeqEntryPtr  pse    = NULL;
    BioseqSetPtr pbss   = NULL;       /*this is a pointer to a BioseqSet*/
    SeqEntryPtr  pseNew = NULL;       /*this is a pointer to a new set of SeqEntrys inside a BioseqSet*/
    BioseqPtr    pbsAux = NULL;
    SeqIdPtr     psi    = NULL;
    SeqIdPtr     psiAux = NULL;
    SeqEntryPtr  pseFnd = NULL;
    SeqEntryPtr  pseAux = NULL; 
    SeqLocPtr    slp    = NULL;

    int flagdna= 0;

    if (gi <= 0)
    {
        ErrPostEx(SEV_ERROR,0,0, "SHoundGetGenBankff: Invalid GI.");
        return FALSE;
    }
    if (pfile == NULL)
    {
        ErrPostEx(SEV_ERROR,0,0, "SHoundGetGenBankff: Invalid file pointer.");
        return FALSE;
    }
     
    
    if(SHoundIsProtein(gi)) 
    {
	    pse = SHoundGetSeqEntry(gi);  
		if(pse == NULL)
		{
		    ErrPostEx(SEV_ERROR,0,0, "[SHoundGetGenBankff]: SHoundGetSeqEntry returned NULL.");
			return FALSE;
		}
	
		slp = (SeqLocPtr) ValNodeNew(slp);
		slp->choice=3;
		slp->data.ptrvalue = (SeqIdPtr) ValNodeNew(slp->data.ptrvalue);
	        psi = slp->data.ptrvalue;
		psi->choice=12;
		psi->data.intvalue=gi;			

		ObjMgrRegister(OBJ_SEQENTRY, pse);
		if (! SeqEntryToGnbk(pse,
							 slp,
							 GENPEPT_FMT,    /* 3 GENPEPT_FMT; GENBANK_FMT: for amino acids. */
							 ENTREZ_MODE,   /* 1 RELEASE_MODE, ENTREZ_MODE. */
							 NORMAL_STYLE,   /* 1 STYLE                      */
							 8,              /* FLAGS 1 HTML, 8 TXT.         */
							 4,              /* LOCKS                        */
							 0,              /* CUSTOM 1                     */  
							 NULL, 
							 pfile) )
		{	   
		    ErrPostEx(SEV_ERROR,0,0, "SHoundGetGenBankff: SeqEntryToFlat failed for NA gi %ld.", (long) gi);
			SeqEntryFree(pse); 
			return FALSE;
		}
    }
    else {
		ErrPostEx(SEV_ERROR,0,0, "\tSHoundGetGenBankff: Processing DNA or RNA.");
		pse = SHoundGetSeqEntry(gi);
		if(pse == NULL)
		{
		    ErrPostEx(SEV_ERROR,0,0, "[SHoundGetGenBankff]: SHoundGetSeqEntry returned NULL.");
			return FALSE;
		}

		slp = (SeqLocPtr) ValNodeNew(slp);
		slp->choice=3;
		slp->data.ptrvalue = (SeqIdPtr) ValNodeNew(slp->data.ptrvalue);
	        psi = slp->data.ptrvalue;
		psi->choice=12;
		psi->data.intvalue=gi;				

		
		ObjMgrRegister(OBJ_SEQENTRY, pse);
		
		if (! SeqEntryToGnbk(pse, 
							 slp, 
							 GENBANK_FMT,    /* 1 GENBANK_FMT: for amino acids. */
							 ENTREZ_MODE,   /* 1 RELEASE_MODE, ENTREZ_MODE. */
							 NORMAL_STYLE,   /* 1 STYLE                      */
							 8,              /* FLAGS 1 HTML, 8 TXT.         */
							 4,              /* LOCKS                        */
							 0,              /* CUSTOM 1                     */
							 NULL, 
							 pfile) )

        		{
			    ErrPostEx(SEV_ERROR,0,0, "SHoundGetGenBankff: SeqEntryToFlat failed for NA gi %ld.", (long) gi);
				SeqEntryFree(pse); 
				return FALSE;
        		}
    	}
	
	SeqEntryFree(pse);
	ValNodeFree(psi);
	ValNodeFree(slp);
    return TRUE;
}


/* This functions it's removed and a new SHoundGetGenBankff 
 * was implemented because NCBI Depricate SeqEntryToFlatEx()
 * The new SHoundGetGenBankff will implement SeqEntryToGnbk()
 */
#if 0 
/* THE OLD BROKEN GETGENBANKFF */
Boolean LIBCALL SHoundGetGenBankff(Int4 gi, FILE *pfile)
{
   SeqEntryPtr pse = NULL;
 /* BioseqPtr pbs = NULL; */
 /* BioseqSetPtr pbss = NULL; */
 SeqIdPtr psi = NULL; 
 AsnIoBSPtr aibp=NULL;
 AsnIoBSPtr aibp2=NULL;
 ByteStorePtr bsp = NULL;
 SeqEntryPtr pse_single = NULL;
 
 if (gi <= 0)
 {
    ErrPostEx(SEV_ERROR,0,0, "SHoundGetGenBankff: Invalid GI.");
    return FALSE;
 }
 if (pfile == NULL)
 {
    ErrPostEx(SEV_ERROR,0,0, "SHoundGetGenBankff: Invalid file pointer.");
    return FALSE;
 }
 
 pse = SHoundGetSeqEntry(gi);

 if(pse == NULL)
 {
   ErrPostEx(SEV_ERROR,0,0, "SHoundGetGenBankff: SHoundGetSeqEntry returned NULL.");
   return FALSE;
 }
  
 
 if(SHoundIsProtein(gi))
 {
  if (!SeqEntryLoad())
  {
     ErrPostEx(SEV_ERROR,0,0, "SHoundGetGenBankff: SeqEntryLoad failed.");
     return FALSE;
  } 
  psi = ValNodeNew(NULL);
  psi->choice = SEQID_GI;
  psi->data.intvalue = gi;

  bsp = BSNew(5000);
  aibp = AsnIoBSOpen("wb", bsp);
  if (aibp == NULL)
  {
     SeqEntryFree(pse);
     ErrPostEx(SEV_ERROR,0,0, "SHoundGetGenBankff: AsnIoBSOpen failed.");
     return FALSE;
  }
  SeqEntryAsnWrite(pse, aibp->aip, NULL);
  AsnIoBSClose(aibp);

  aibp2 = AsnIoBSOpen("rb", bsp);
  if (aibp2 == NULL)
  {
    SeqEntryFree(pse);
    ErrPostEx(SEV_ERROR,0,0, "SHoundGetGenBankff: AsnIoBSOpen failed.");
    return FALSE;
  }
  pse_single = SeqEntryAsnGet(aibp2->aip, NULL, psi, SEQENTRY_READ_BIOSEQ);
  if(pse_single == NULL)
  {
    SeqEntryFree(pse);
    SeqEntryFree(pse_single);
    ErrPostEx(SEV_ERROR,0,0, "SHoundGetGenBankff: SeqEntryAsnGet failed.");
    return FALSE;
  }
  AsnIoBSClose(aibp2);
  BSFree(bsp);
      
  ObjMgrRegister(OBJ_SEQENTRY, pse_single);
  if (! SeqEntryToFlatEx(pse_single, pfile, GENPEPT_FMT, RELEASE_MODE, NULL,FF_REGULAR))
  {
     ErrPostEx(SEV_ERROR,0,0, "SHoundGetGenBankff: SeqEntryToFlat failed for protein gi %ld.", (long) gi);
     SeqEntryFree(pse);
     return FALSE;
  }
 }
  else
  {
    ObjMgrRegister(OBJ_SEQENTRY, pse);
    if (! SeqEntryToFlatEx(pse, pfile, GENBANK_FMT, RELEASE_MODE, NULL, FF_REGULAR))
     {
        ErrPostEx(SEV_ERROR,0,0, "SHoundGetGenBankff: SeqEntryToFlat failed for NA gi %ld.", (long) gi);
        SeqEntryFree(pse); 
        return FALSE;
      }
  }
  SeqEntryFree(pse);
  return TRUE;
}
#endif

Boolean LIBCALL SHoundGetGenBankffList(ValNodePtr pvngi, FILE *pfile)
{
    if (pvngi == NULL) 
	{
        ErrPostEx(SEV_ERROR,0,0, "SHoundGetGenBankffList: NULL list.");
		return FALSE;
	}
	while(pvngi != NULL)
    {
	    if(!SHoundGetGenBankff(pvngi->data.intvalue, pfile))
		{
		    ErrPostEx(SEV_ERROR,0,0, "SHoundGetGenBankffList: SHoundGetGenBank failed on %ld.", (long) pvngi->data.intvalue);
			fprintf(pfile,"Could not generate GenBank Flat File for GI %d.  Skipping this GI\n//\n",pvngi->data.intvalue);
		}
		pvngi = pvngi->next;
    }
	return TRUE;
}



/******************************************************/
/*  Sequence fetch - XML                              */
/******************************************************/

Boolean LIBCALL SHoundGetXMLBioseq(Int4 gi, CharPtr file_name)
{
   BioseqPtr pbs = NULL;
   AsnIoPtr aip = NULL;


   if (gi <= 0)
    {
       ErrPostEx(SEV_ERROR,0,0, "SHoundGetXMLBioseq: Invalid GI.");
       return FALSE;
    }
   if (file_name == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "SHoundGetXMLBioseq: Invalid file pointer.");
       return FALSE;
    }

   aip = AsnIoOpen(file_name, "wx");
   if(aip == NULL)
   {
     ErrPostEx(SEV_ERROR,0,0, "SHoundGetXMLBioseq: Cannot open %s.\n", IsNullStr(file_name));
     return FALSE;
   }
   pbs = SHoundGetBioseq(gi);
   if(pbs == NULL)
   {
     ErrPostEx(SEV_ERROR,0,0, "SHoundGetXMLBioseq: Cannot get bioseq.\n");
     return FALSE;
   }
   if(!BioseqAsnWrite(pbs, aip, NULL))
   {
     ErrPostEx(SEV_ERROR,0,0, "SHoundGetXMLBioseq: BioseqAsnWrite failed.\n");
     AsnIoClose(aip);
     BioseqFree(pbs);
     return FALSE;
   }
   AsnIoClose(aip);
   BioseqFree(pbs);
   return TRUE;
}


Boolean LIBCALL SHoundGetXMLSeqEntry(Int4 gi, CharPtr file_name)
{
    AsnIoPtr aip = NULL;
    SeqEntryPtr pse = NULL;

    if (gi <= 0)
    {
       ErrPostEx(SEV_ERROR,0,0, "SHoundGetXMLSeqEntry: Invalid GI.");
       return FALSE;
    }
   if (file_name == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "SHoundGetXMLSeqEntry: Invalid file pointer.");
       return FALSE;
    }

   aip = AsnIoOpen(file_name, "wx");
   if(aip == NULL)
   {
     ErrPostEx(SEV_ERROR,0,0, "SHoundGetXMLSeqEntry: Cannot open %s.\n", IsNullStr(file_name));
     return FALSE;
   }
   pse = SHoundGetSeqEntry(gi);
   if(pse == NULL)
   {
     ErrPostEx(SEV_ERROR,0,0, "SHoundGetXMLSeqEntry: Cannot get SeqEntry.\n");
     AsnIoClose(aip);
     return FALSE;
   }
   if(!SeqEntryAsnWrite(pse, aip, NULL))
    {
     ErrPostEx(SEV_ERROR,0,0, "SHoundGetXMLSeqEntry: BioseqAsnWrite failed.\n", file_name);
     AsnIoClose(aip); 
     SeqEntryFree(pse);
     return FALSE;
   }

   AsnIoClose(aip);
   SeqEntryFree(pse);
   return TRUE;
}



/******************************************************/
/*  SHound Links                                      */
/******************************************************/

ValNodePtr  LIBCALL SHoundGiFrom3D(Int4 mmdbid, Boolean ProteinOnly)
{
   StMmgiPtr pmg = NULL, pmghead = NULL;
   Int2 res = 0;
   ValNodePtr pvngi = NULL;


   if(!SHoundModule("strucdb"))
   {
     ErrPostEx(SEV_ERROR,0,0, "SHoundGiFrom3D: Missing required SeqHound module.");
     return NULL;
   }

   if (mmdbid <= 0)
    {
      ErrPostEx(SEV_ERROR,0,0, "SHoundGiFrom3D: Invalid MmdbId.");
      return NULL;
    }

   pmg = StMmgiNew();
   pmghead = pmg;
   pmg->mmdbid = mmdbid;
   res = SearchMMGI(&pmg);
    if (res == 0)
     {
       ErrPostEx(SEV_ERROR,0,0, "SeekMMGI failed.");
       StMmgiFree(pmghead);
       return NULL;
     }
    if (res == -1)
     {
       StMmgiFree(pmghead);
       return NULL;
     }
    if (res == 1)
     {
       while(pmg != NULL)
        {
          if (!ProteinOnly)
              pvngi = Slri_ValNodeAddInt(&pvngi, 0, pmg->gi);
          else if ((ProteinOnly) && SHoundIsProtein(pmg->gi))
              pvngi = Slri_ValNodeAddInt(&pvngi, 0, pmg->gi);
          pmg = pmg->next;
        }

       StMmgiFree(pmghead);
       Misc_IntVnpUnique(&pvngi);
       return pvngi;
     }
   StMmgiFree(pmghead);
   return NULL;
}


ValNodePtr LIBCALL SHoundGiFrom3DList(ValNodePtr pvnMmdbid, Boolean ProteinOnly)
{
   ValNodePtr pvngilong = NULL;
   ValNodePtr pvngi = NULL;

   if(!SHoundModule("strucdb"))
   {
     ErrPostEx(SEV_ERROR,0,0, "SHoundGiFrom3DList: Missing required SeqHound module.");
     return NULL;
   }

   while(pvnMmdbid != NULL)
     {
        pvngi = SHoundGiFrom3D(pvnMmdbid->data.intvalue, ProteinOnly);
        if (pvngi != NULL)
	  {
            ValNodeLink(&pvngi, pvngilong);
            pvngilong = pvngi;
            pvngi = NULL;
	  }
        pvnMmdbid = pvnMmdbid->next;
     }

   Misc_IntVnpUnique(&pvngilong);
   return pvngilong;
}


Int4  LIBCALL SHoundGiFromPDBchain(CharPtr pdbcode, CharPtr chain)
{
   CharPtr upper_pdbcode=NULL, upper_chain=NULL;
   StDomdbPtr pdd = NULL, pddhead = NULL;
   Int2 res = 0, i=0;
   Int4 gival = 0;

   if(!SHoundModule("strucdb"))
   {
     ErrPostEx(SEV_ERROR,0,0, "SHoundGiFromPDBchain: Missing required SeqHound module.");
     return 0;
   }
   if (pdbcode == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "SHoundGiFromPDBchain: Invalid pdbcode.");
      return 0;
    }

   pdd = StDomdbNew();
   pddhead = pdd;
   
   
   /*check input pdbcode, converts to upper case if applies*/
   upper_pdbcode = (char *)malloc(strlen(pdbcode)+1);
   for (i=0; i<strlen(pdbcode);i++){
       if (!isdigit(pdbcode[i]) && islower(pdbcode[i]))
           upper_pdbcode[i]=toupper(pdbcode[i]);
       else
	   upper_pdbcode[i]=pdbcode[i];
   }
   upper_pdbcode[strlen(pdbcode)]='\0';
   /*check input chain, converts to upper case if applies*/
   if (chain != NULL) {
       upper_chain = (char *)malloc(strlen(chain)+1);
       if (islower(chain[0]))
	   upper_chain[0]=toupper(chain[0]);
       else
	   upper_chain[0]=chain[0];
       upper_chain[strlen(chain)]='\0';
   }
   pdd->pdbid = StringSave(upper_pdbcode);
   res = SearchDOMDB(&pdd);
   if (res == 0)
   {
       ErrPostEx(SEV_ERROR,0,0, "SHoundGiFromPDBchain: Search DOMDB failed.");
       StDomdbFree(pddhead);
       if (upper_pdbcode)MemFree(upper_pdbcode); 
       if (upper_chain) MemFree(upper_chain);
       return 0;
     }
    if (res == -1)
     {
       StDomdbFree(pddhead);
       if (upper_pdbcode) MemFree(upper_pdbcode); 
       if (upper_chain) MemFree(upper_chain);
       return -1;
     }
    if (res == 1)
     {
       if(chain == NULL)
          gival = pdd->gi;
       else
       {
         while(pdd != NULL)
          {
            if(pdd->chain[0] == upper_chain[0])
	    {
               gival = pdd->gi;
	    }
            pdd = pdd->next;
          }
	  
       }
       StDomdbFree(pddhead);
       if (upper_pdbcode) MemFree(upper_pdbcode);
       if (upper_chain) MemFree(upper_chain);
       return gival;
     }
   StDomdbFree(pddhead);
   MemFree(upper_pdbcode); 
   if (upper_chain) MemFree(upper_chain);
   return 0;
}




Int4 LIBCALL SHound3DFromGi(Int4 Gi)
{
   StMmgiPtr pmg = NULL, pmghead = NULL;
   Int2 res = 0;
   Int4 mmdbid = 0;

   if(!SHoundModule("strucdb"))
   {
     ErrPostEx(SEV_ERROR,0,0, "SHound3DFromGi: Missing required SeqHound module.");
     return 0;
   }

   if (Gi <= 0)
   {
       ErrPostEx(SEV_ERROR,0,0, "SHound3DFromGi: Invalid GI.");
       return 0;
   }

    pmg = StMmgiNew();
    pmghead = pmg;
    pmg->gi = Gi;
    res = SearchMMGI(&pmg);
    if (res == 0)
     {
       ErrPostEx(SEV_ERROR,0,0, "SeekMMGI failed.");
       StMmgiFree(pmghead);
       return 0;
     }
    if (res == -1)
     {
       StMmgiFree(pmghead);
       return -1;
     }
    if (res == 1)
     {
       mmdbid = pmg->mmdbid;
       StMmgiFree(pmghead);
       return mmdbid;
     }
    StMmgiFree(pmghead);
    return 0;
}


ValNodePtr  LIBCALL SHound3DFromGiList(ValNodePtr pvnGi)
{
    ValNodePtr pvnmmdbid = NULL;
    Int4 mmdbid = 0;

   if(!SHoundModule("strucdb"))
   {
     ErrPostEx(SEV_ERROR,0,0, "SHound3DFromGiList: Missing required SeqHound module.");
     return NULL;
   }

   while (pvnGi != NULL)
    {
       mmdbid = SHound3DFromGi(pvnGi->data.intvalue);
       if (mmdbid > 0)
         pvnmmdbid = Slri_ValNodeAddInt(&pvnmmdbid, 0, mmdbid);
       pvnGi = pvnGi->next;
    }
   Misc_IntVnpUnique(&pvnmmdbid);
   return pvnmmdbid;
}




Int4 LIBCALL SHoundDNAFromProtein(Int4 Gia)
{
   StNucprotPtr pnc = NULL, pnchead = NULL;
   Int4 Gin = 0;
   Int2 res = 0;


   if (Gia <= 0)
     {
        ErrPostEx(SEV_ERROR,0,0, "SHoundDNAFromProtein: Invalid GI.");
        return 0;
     }

   pnc = StNucprotNew();
   pnchead = pnc;
   pnc->gia = Gia;
   res = SearchNUCPROT(&pnc);
   if (res == 0)
     {
       ErrPostEx(SEV_ERROR,0,0, "SHoundDNAFromProtein: SearchNUCPROT failed.");
       StNucprotFree(pnchead);
       return 0;
     }
   if (res == -1)
    {
      StNucprotFree(pnchead);
      return -1; 
    }
   if (res == 1)
    {
       if (pnc->gin > 0)
        {
          Gin = pnc->gin;
          StNucprotFree(pnchead);
          return Gin;
        }
       else
        {
           ErrPostEx(SEV_ERROR,0,0, "SHoundDNAFromProtein: Invalid Gi.");
           StNucprotFree(pnchead);
           return 0;
        }
    }
   StNucprotFree(pnchead);
   return 0;
}


ValNodePtr  LIBCALL SHoundDNAFromProteinList(ValNodePtr pvnGia)
{
   ValNodePtr pvnGin = NULL;
   Int4 Gin = 0;
   
   while (pvnGia != NULL)
    {
       Gin = SHoundDNAFromProtein(pvnGia->data.intvalue);
       if (Gin > 0)
         pvnGin = Slri_ValNodeAddInt(&pvnGin, 0, Gin);
       pvnGia = pvnGia->next;
    }

   Misc_IntVnpUnique(&pvnGin);
   return pvnGin;
}


ValNodePtr LIBCALL SHoundProteinFromDNA(Int4 Gin)
{
   StNucprotPtr pnc = NULL, pnchead = NULL;    
   ValNodePtr pvnGia = NULL;
   Int2 res = 0;

   if (Gin <= 0)
     {
       ErrPostEx(SEV_ERROR,0,0, "SHoundProteinFromDNA: Invalid GI.");
       return NULL; 
     }
   pnc = StNucprotNew();
   pnchead = pnc;
   pnc->gin = Gin;
   res = SearchNUCPROT(&pnc);
   if (res == 0)
     {
       ErrPostEx(SEV_ERROR,0,0, "SHoundProteinFromDNA: SearchNUCPROT failed.");
       StNucprotFree(pnchead);
       return NULL;
     }
   if (res == -1)
    {
      StNucprotFree(pnchead);
      return NULL; 
    }
   if (res == 1)
   while(pnc != NULL)
    {
       if (pnc->gia > 0)
          pvnGia = Slri_ValNodeAddInt(&pvnGia, 0, pnc->gia);
       else
        {
           ErrPostEx(SEV_ERROR,0,0, "SHoundProteinFromDNA: Invalid GI.");
           StNucprotFree(pnchead); 
           return NULL;
        }            
       pnc = pnc->next;
    }
   StNucprotFree(pnchead);      
   return pvnGia;
}

ValNodePtr  LIBCALL SHoundProteinFromDNAList(ValNodePtr pvnGin)
{
   ValNodePtr pvnGia = NULL;
   ValNodePtr pvnGialong = NULL;

   while (pvnGin != NULL)
     {
        pvnGia = SHoundProteinFromDNA(pvnGin->data.intvalue);
        if (pvnGia != NULL)
	  {
             ValNodeLink(&pvnGia, pvnGialong);
             pvnGialong = pvnGia;
             pvnGia = NULL;
          }
        pvnGin = pvnGin->next;
     }

   Misc_IntVnpUnique(&pvnGialong);
   return pvnGialong;
}

/* Retrieves the pmids or muids given a Gi and the reference id you want 
 * back. If you want PubMed ID, set 'ReferenceType' == 1, if you want
 * MedLine ID, set 'ReferenceType' == 2. 
 *
 * This function deprecates SHoundMuidFromGi.
 * */
ValNodePtr LIBCALL SHoundGetReferenceIDFromGi(Int4 Gi, Int2 ReferenceType)
{
  StPubseqPtr pps = NULL, ppshead = NULL;
  ValNodePtr VNPRef = NULL;
  Int2 res = 0;
  
  if(Gi <= 0)
  {
    ErrPostEx(SEV_ERROR, 0, 0, "[SHoundGetReferenceIDFromGi]: Invalid Gi %ld.", Gi);
    goto end;
  }
  /* make sure that only requesting PMID or MUID */
  if(ReferenceType != 1 && ReferenceType != 2) 
  {
    ErrPostEx(SEV_ERROR, 0, 0, "[SHoundGetReferenceIDFromGi]: Invalid reference type %d.", ReferenceType);
    goto end;
  }

  pps = StPubseqNew();
  ppshead = pps;
  pps->gi = Gi;

  /* SearchPUBSEQ will retrieve both pmid and muid */
  res = SearchPUBSEQ(&pps);
  if(res == -1)
  {
    /* didn't find the record */
    /* ErrPostEx(SEV_ERROR, 0, 0, "[SHoundGetReferenceIDFromGi]: SearchPUBSEQ failed %d.", res); */
    goto end;
  } 
  else if(res == 0) { goto end; } 
  else {
    /* SearchPUBSEQ got some records */
    while(pps != NULL)
    {
      if(ReferenceType == 1)      /* requesting PubMed */
      {
        if(pps->pmid > 0) ValNodeAddInt(&VNPRef, 0, pps->pmid);
      } else {                    /* requesting MedLine ID */
        if(pps->muid > 0) ValNodeAddInt(&VNPRef, 0, pps->muid);
      }

      pps = pps->next;
    }
  }
  StPubseqFree(ppshead);
  Misc_IntVnpUnique(&VNPRef);
  return VNPRef;

  end:
    /* got an error */
    ErrPostEx(SEV_INFO, 0, 0, "[SHoundGetReferenceIDFromGi]: Error on Gi = %ld. No %s.", 
           Gi, ReferenceType == 1 ? "PMID" : "MUID");
    StPubseqFree(ppshead);
    return NULL;
}

/* Retrieves the pmids or muids given a gi list and the type of 
 * reference identifiers you want. Set ReferenceType to 1 if you want
 * PubMed ID, set ReferenceType to 2 if you want MedLine ID.
 *
 * The return value will not contain duplicates.
 *
 * This function deprecates SHoundMuidFromGiList */
ValNodePtr LIBCALL SHoundGetReferenceIDFromGiList(ValNodePtr GiList, Int2 ReferenceType)
{
  ValNodePtr VNPRef = NULL;
  ValNodePtr VNPTemp = NULL;
   
  /* make sure that only requesting PMID or MUID */
  if(ReferenceType != 1 && ReferenceType != 2) 
  {
    ErrPostEx(SEV_ERROR, 0, 0, "[SHoundGetReferenceIDFromGiList]: Invalid reference type %d.", ReferenceType);
    goto end;
  }

  while(GiList != NULL)
  {
     VNPTemp = SHoundGetReferenceIDFromGi(GiList->data.intvalue, ReferenceType);
     if(VNPTemp != NULL)
     {
       ValNodeLink(&VNPRef, VNPTemp);
       VNPTemp = NULL;
     }
     GiList = GiList->next;
  }
  Misc_IntVnpUnique(&VNPRef);
  return VNPRef;

  end:
    /* got an error */
    ErrPostEx(SEV_ERROR, 0, 0, "[SHoundGetReferenceIDFromGiList]: Error.");
    return NULL;
}

/* Retrieves the gis associated with a publication id 'ReferenceID'.
 * If 'ReferenceID' is a pubmed publication, set 'ReferenceType' == 1,
 * if 'ReferenceID' is a medline publication, set 'ReferenceType' == 2, 
 *
 * A publication may contain many gis, eg a high throughput publication.
 *
 * The return list may contain duplicates.
 */
ValNodePtr LIBCALL SHoundGiFromReferenceID(Int4 ReferenceID, Int2 ReferenceType)
{
  StPubseqPtr pps = NULL, ppshead = NULL;
  ValNodePtr VNPGi = NULL;
  Int2 res = 0;
  
  if(ReferenceID <= 0)
  {
    ErrPostEx(SEV_ERROR, 0, 0, "[SHoundGiFromReferenceID]: Invalid Reference ID %ld.", ReferenceID);
    goto end;
  }
  if(ReferenceType != 1 && ReferenceType != 2)
  {
    ErrPostEx(SEV_ERROR, 0, 0, "[SHoundGiFromReferenceID]: Invalid Reference Type %d.", ReferenceType);
    goto end;
  }

  pps = StPubseqNew();
  ppshead = pps;
  if(ReferenceType == 1)
    pps->pmid = ReferenceID;
  else
    pps->muid = ReferenceID;
  
  /* SearchPUBSEQ will retrieve both pmid and muid */
  res = SearchPUBSEQ(&pps);
  if(res == 0 || res == -1)
  {
    /* didn't find the record */
    ErrPostEx(SEV_ERROR, 0, 0, "[SHoundGiFromReferenceID]: SearchPUBSEQ failed %d.", res); 
    goto end;
  } else {
    /* SearchPUBSEQ got some records */
    while(pps != NULL)
    {
      if(pps->gi > 0) ValNodeAddInt(&VNPGi, 0, pps->gi);
      pps = pps->next;
    }
  }
  StPubseqFree(ppshead);
  return VNPGi;

  end:
    /* got an error */
    ErrPostEx(SEV_ERROR, 0, 0, "[SHoundGiFromReferenceID]: Error on RefID = %ld.", ReferenceID);
    StPubseqFree(ppshead);
    return NULL;
}

/* Retrieves all the gi's associated with the references found in 'ReferenceList' of
 * 'ReferenceType', where 'ReferenceType' == 1 if a PubMed publication, or 
 * 'ReferenceType' == 2 if a Medline publication.
 *
 * Return value may contain duplicates.
 *
 * Return value may be very long for some publication, eg for a high through put publication.
 * */
ValNodePtr LIBCALL SHoundGiFromReferenceList(ValNodePtr ReferenceList, Int2 ReferenceType)
{
  ValNodePtr VNPGi = NULL;
  ValNodePtr VNPTemp = NULL;
   
  /* make sure that only requesting PMID or MUID */
  if(ReferenceType != 1 && ReferenceType != 2) 
  {
    ErrPostEx(SEV_ERROR, 0, 0, "[SHoundGetReferenceIDFromGiList]: Invalid reference type %d.", ReferenceType);
    goto end;
  }

  while(ReferenceList != NULL)
  {
     VNPTemp = SHoundGiFromReferenceID(ReferenceList->data.intvalue, ReferenceType);
     if(VNPTemp != NULL)
     {
       ValNodeLink(&VNPGi, VNPTemp);
       VNPTemp = NULL;
     }
     ReferenceList = ReferenceList->next;
  }
  return VNPGi;

  end:
    /* got an error */
    ErrPostEx(SEV_ERROR, 0, 0, "[SHoundGiFromReferenceList]: Error.");
    return NULL;
}

ValNodePtr  LIBCALL SHoundMuidFromGi(Int4 Gi)
{
   StPubseqPtr pps = NULL, ppshead = NULL;
   ValNodePtr pvnMuid = NULL;
   Int2 res = 0;

   if (Gi <= 0)
     {
       ErrPostEx(SEV_ERROR,0,0, "SHoundMuidFromGi: Invalid GI.");
       return NULL;
     }

   pps = StPubseqNew();
   ppshead = pps;
   pps->gi = Gi;
   res = SearchPUBSEQ(&pps);
   if (res == 0)
     {
       ErrPostEx(SEV_ERROR,0,0, "SHoundMuidFromGi: SearchPUBSEQ failed.");
       StPubseqFree(ppshead);
       return NULL;
     }
   if (res == -1)
     {
       StPubseqFree(ppshead); 
       return NULL;
     }
   if (res == 1)
       while(pps != NULL)
        {   
          if (pps->muid <= 0)
	    {
              ErrPostEx(SEV_ERROR,0,0, "SHoundMuidFromGi: Invalid MUID.");
              StPubseqFree(ppshead);  
              return NULL;
	    }
          else
             pvnMuid = Slri_ValNodeAddInt(&pvnMuid, 0, pps->muid);
          pps = pps->next;
        }
   StPubseqFree(ppshead);  
   return pvnMuid;
}




ValNodePtr  LIBCALL SHoundMuidFromGiList(ValNodePtr pvnGi)
{
   ValNodePtr pvnMuidlong = NULL;
   ValNodePtr pvnMuid = NULL;

   while(pvnGi != NULL)
     {
        pvnMuid = SHoundMuidFromGi(pvnGi->data.intvalue);
        if (pvnMuid != NULL)
	  {
            ValNodeLink(&pvnMuid, pvnMuidlong);
            pvnMuidlong = pvnMuid;
            pvnMuid = NULL;
	  }
        pvnGi = pvnGi->next;
     }

   Misc_IntVnpUnique(&pvnMuidlong);
   return pvnMuidlong;
}


ValNodePtr  LIBCALL SHoundMuidFrom3D(Int4 mmdbid)
{
   ValNodePtr pvngi = NULL, pvnmuid = NULL;

   if(mmdbid <= 0)
    {
      ErrPostEx(SEV_ERROR,0,0, "SHoundMuidFrom3D: Invalid MmdbId.");
      return NULL;
    }

   pvngi = SHoundGiFrom3D(mmdbid, FALSE);
   if (pvngi == NULL)
       return NULL;
   else
    {
       pvnmuid = SHoundMuidFromGiList(pvngi);
       ValNodeFree(pvngi);
       return pvnmuid;
    }
}


ValNodePtr  LIBCALL SHoundMuidFrom3DList(ValNodePtr pvnMmdbid)
{

   ValNodePtr pvnmuid = NULL;
   ValNodePtr pvnmuidlong = NULL;

   while (pvnMmdbid != NULL)
     {
        pvnmuid =  SHoundMuidFrom3D(pvnMmdbid->data.intvalue);
        if (pvnmuid != NULL)
	  {
             ValNodeLink(&pvnmuid, pvnmuidlong);
             pvnmuidlong = pvnmuid;
             pvnmuid = NULL;
          }
        pvnMmdbid = pvnMmdbid->next;
     }

   Misc_IntVnpUnique(&pvnmuidlong);
   return pvnmuidlong;
}



Int4  LIBCALL SHoundTaxIDFromGi(Int4 Gi)
{
   StTaxgiPtr      ptg = NULL, 
               ptghead = NULL;
   Int4          TaxId = 0;
   Int2            res = 0;

   if (Gi <= 0)
   {
      ErrPostEx( SEV_INFO, 0, 0, "[SHoundTaxIDFromGi]: Invalid GI %d", (int)Gi );
      return 0;
   }

   if ( NULL == (ptg = StTaxgiNew()) )
   {
      ErrPostEx( SEV_WARNING, 0, 0, "[SHoundTaxIDFromGi]: Cannot create StTaxgi object" );
      return 0;
   }

   ptghead = ptg;
   ptg->gi = Gi;
   res = SearchTAXGI(&ptg);
   if (res == 0)
   {
      ErrPostEx(SEV_ERROR,0,0, "[SHoundTaxIDFromGi]: SearchTAXGI failed.");
      StTaxgiFree(ptghead);
      return 0;
   }
   if (res == -1)
   {
      StTaxgiFree(ptghead);
      return -1;
   }
   if (res == 1)
   {
      if(ptg->taxid <= 0)
      {
        ErrPostEx ( SEV_ERROR,0,0, "[SHoundTaxIDFromGi]: Invalid TaxID %d for GI %d.",
	            ptg->taxid,ptg->gi );
        StTaxgiFree(ptghead);
        return 0;
      } 
      else     
        TaxId = ptg->taxid;
   }
   StTaxgiFree(ptghead);
   return TaxId; 
}


ValNodePtr  LIBCALL SHoundTaxIDFromGiList(ValNodePtr pvnGi)
{
   ValNodePtr pvntaxid = NULL;
   Int4 TaxId = 0;

   while(pvnGi != NULL)
   {
       TaxId = SHoundTaxIDFromGi(pvnGi->data.intvalue);
       if (TaxId > 0)
           pvntaxid = Slri_ValNodeAddInt(&pvntaxid, 0, TaxId);
       pvnGi = pvnGi->next;
   }
   Misc_IntVnpUnique(&pvntaxid);
   return pvntaxid;
}


ValNodePtr LIBCALL SHoundTaxIDFrom3D(Int4 mmdbid)
{
   ValNodePtr       pvngi = NULL, 
                 pvntaxid = NULL;

   if (mmdbid <= 0)
   {
      ErrPostEx(SEV_INFO,0,0, "[SHoundTaxIDFrom3D]: Invalid parameter.");
      return 0;
   }

   if ( NULL == ( pvngi = SHoundGiFrom3D(mmdbid, FALSE)) )
      { return NULL; }
   else
   {
       pvntaxid = SHoundTaxIDFromGiList(pvngi);
       ValNodeFree(pvngi);
       return pvntaxid;
   }
}



ValNodePtr LIBCALL SHoundTaxIDFrom3DList(ValNodePtr pvnMmdbid)
{
   ValNodePtr pvntaxid = NULL;
   ValNodePtr pvntaxidlong = NULL;

   while (pvnMmdbid != NULL)
     {
        pvntaxid =  SHoundTaxIDFrom3D(pvnMmdbid->data.intvalue);
        if (pvntaxid != NULL)
	  {
             ValNodeLink(&pvntaxid, pvntaxidlong);
             pvntaxidlong = pvntaxid;
             pvntaxid = NULL;
          }
        pvnMmdbid = pvnMmdbid->next;
     }

   Misc_IntVnpUnique(&pvntaxidlong);
   return pvntaxidlong;
}


ValNodePtr LIBCALL SHoundProteinsFromTaxID(Int4 taxid)
{
   return SearchTAXGIProt(taxid);
}

/* uses precomputed index when possible */
ValNodePtr LIBCALL SHoundProteinsFromTaxIDIII(Int4 taxid)
{

   ValNodePtr pvngi = NULL;
   Int4 * gis=NULL;
   Int4 size =0, count =0;


   if((taxid <= 0) && (taxid != -9606))
     {
        ErrPostEx(SEV_ERROR,0,0, " SHoundProteinsFromTaxIDII: Invalid parameter");
        return NULL;
     }
   size = Check_precomputed_index(&gis,taxid,"protein");
   /*put the gis into the posting*/
   while(count < size){
   	pvngi = Slri_ValNodeAddInt(&pvngi, 0, gis[count]);
   	count++;
   }
   /*since allocated memory for gis in Check_precomputed_index, after
	 use it should free memory */
   if (gis) MemFree(gis);

   if(count != 0)
   	return pvngi;
   
   pvngi = SHoundProteinsFromTaxID(taxid);
   return pvngi;
}


/*  Given:  an Int pointer, a taxid and the the taxtype this method
*  checks the precomputed index file to see if the taxid is a precomputed
*  search.  If it is it files an array of corresponding size with the
*  the appropriate gis corresponding to that taxid and taxtype.
*  Returns:  the number of gis corresponding to that taxid in the array.
*  Returns zero if there is no precomputed search.
*/
Int4 Check_precomputed_index(Int4 ** gis, Int4 taxid, CharPtr TaxType){

	struct Tax_index * index;
	FILE * fptr=NULL, * pre_compfptr=NULL;
	Int4 querycounter =0, size =0;
	Char * precompname=NULL;
	/* SK: NULL pointer check */
	Char * ptr=NULL;

	/*open the index file*/
	index = Malloc(Cfg_GetQueryCount() * sizeof(struct Tax_index));
	/* SK: NULL pointer check if memory run out */
	Mem_Check(index);
	fptr = FileOpen(Cfg_GetIndexFile(), "rb");
	if(fptr == NULL)
	{
		ErrPostEx(SEV_ERROR,0,0,"Couldn't open the index file of precomputed.\n");
		MemFree(index);
		return 0;
	}
	/*read out all the contents of the index file*/
	if(fread(index,sizeof(struct Tax_index), (unsigned)Cfg_GetQueryCount(), fptr)==0)
	{
		ErrPostEx(SEV_ERROR,0,0,"Nothing read from the index file of precomputed.\n");
		MemFree(index);
                FileClose(fptr);
				return 0;
	}
	for(querycounter = 0; querycounter<Cfg_GetQueryCount(); querycounter++)
	{
		/*find out if our tax_id is in the index*/
		if(taxid == index[querycounter].tax_id)
		{
			/*open the corresponding file*/
        		precompname = Malloc(256 * sizeof(Char));
			Mem_Check(precompname);

                        /* SK: NULLpointer check */
                        ptr = Cfg_GetPrecompuPath();
        		sprintf(precompname , "%s%ld%s", IsNullStr(ptr),(long) index[querycounter].tax_id, IsNullStr(TaxType));
        		pre_compfptr = FileOpen(precompname, "rb");
        		if(pre_compfptr == NULL)
        		{
        			ErrPostEx(SEV_ERROR,0,0, "Couldn't open %s.\n",IsNullStr(precompname));
        			MemFree(index);
        			MemFree(precompname);
                                FileClose(fptr);
					return 0;
        		}
        		/*get the appropriate size of the information, depends on type*/
        		if(strstr("NA", TaxType) != NULL)
        			size = index[querycounter].num_DNA_gis;
        		else if(strstr("protein", TaxType) != NULL) 
        			size = index[querycounter].num_proteins_gis;
        		else 
        			size = index[querycounter].num_gis;
        		*gis = Malloc(size * sizeof(Int4));
        		if(fread(*gis, sizeof(Int4),size, pre_compfptr)==0)
        		{
        			ErrPostEx(SEV_ERROR,0,0, "Nothing read from file %s \n",IsNullStr(precompname));
        			MemFree(index);
        			MemFree(precompname);
                                FileClose(fptr);
                                FileClose(pre_compfptr);
					return 0;
        		}
        		fflush(pre_compfptr);
				FileClose(pre_compfptr);
        		MemFree(precompname);
		}
	}
	if (index) MemFree(index);
	FileClose(fptr);
	return size;
}



ValNodePtr LIBCALL SHoundProteinsFromTaxIDList(ValNodePtr pvntaxid)
{
   ValNodePtr pvngilong = NULL;
   ValNodePtr pvngi = NULL;


   while(pvntaxid != NULL)
     {
        pvngi = SHoundProteinsFromTaxIDII(pvntaxid->data.intvalue);
        if (pvngi != NULL)
	  {
            ValNodeLink(&pvngi, pvngilong);
            pvngilong = pvngi;
            pvngi = NULL;
	  }
        pvntaxid = pvntaxid->next;
     }

   Misc_IntVnpUnique(&pvngilong);
   return pvngilong;
}



ValNodePtr LIBCALL SHoundDNAFromTaxID(Int4 taxid)
{
   return SearchTAXGIDNA(taxid);
}


/* uses precomputed index when possible */
ValNodePtr LIBCALL SHoundDNAFromTaxIDIII(Int4 taxid)
{

   ValNodePtr pvngi = NULL;
   Int4 * gis = NULL;
   Int4 size =0, count =0;

   if((taxid <= 0) && (taxid != -9606))
     {
        ErrPostEx(SEV_ERROR,0,0, " SHoundDNAFromTaxIDII: Invalid parameter");
        return NULL;
     }

	size = Check_precomputed_index(&gis,taxid,"NA");

	/*put the gis into the valnode struct*/
	while(count < size){
		pvngi = Slri_ValNodeAddInt(&pvngi, 0, gis[count]);
		count++;
	}
    
	/*since allocate memory for gis in Check_precomputed_index,
	  after use it, should free memory*/
	if (gis) MemFree(gis);

	if(count != 0)
		return pvngi;

   pvngi =	SHoundDNAFromTaxID(taxid);
   return pvngi;
}



ValNodePtr LIBCALL SHoundDNAFromTaxIDList(ValNodePtr pvntaxid)
{
   ValNodePtr pvngilong = NULL;
   ValNodePtr pvngi = NULL;


   while(pvntaxid != NULL)
     {
        pvngi = SHoundDNAFromTaxIDII(pvntaxid->data.intvalue);
        if (pvngi != NULL)
	  {
            ValNodeLink(&pvngi, pvngilong);
            pvngilong = pvngi;
            pvngi = NULL;
	  }
        pvntaxid = pvntaxid->next;
     }

   Misc_IntVnpUnique(&pvngilong);
   return pvngilong;
}




/********************************************************/
/* Genome Iterators                                     */
/********************************************************/

ValNodePtr   LIBCALL SHoundProteinsFromOrganism(Int4 taxid, Byte chromflags)
{
   ValNodePtr pvnchrom = NULL;
   ValNodePtr pvnGi = NULL;
   ValNodePtr pvnUnknownGis = NULL;


   /*   if(!SHoundModule("gendb"))
   {
     ErrPostEx(SEV_ERROR,0,0, "SHoundProteinsFromOrganism: Missing required SeqHound module.");
     return NULL;
	 }*/

   if (taxid <= 0)
   {
      ErrPostEx(SEV_INFO,0,0, "SHoundProteinsFromOrganism: Invalid taxonomy ID.");
      return NULL;
   }
   if (chromflags <= 0)
   {
       ErrPostEx(SEV_ERROR,0,0, "SHoundProteinsFromOrganism: Invalid chromflags.");
       return NULL;
   }

   pvnchrom = SHoundChromosomeFromGenome(taxid, chromflags);
   pvnGi = SHoundProteinsFromChromosomeList(pvnchrom);

   /*--------------------------------------------------------------
	 Get the proteins that have not been assigned to a specific 
	 chromosome.
	 -------------------------------------------------------------*/
   pvnUnknownGis = SHoundProteinsFromUnknownChromosome( taxid );

   if( pvnGi && pvnUnknownGis )
   {
	 ValNodeLink( &pvnGi, pvnUnknownGis );
	 pvnGi = pvnUnknownGis;
   }
   else if( pvnUnknownGis && !pvnGi )
   {
	 pvnGi = pvnUnknownGis;
   }

   ValNodeFree(pvnchrom);
   return pvnGi;
}

ValNodePtr LIBCALL SHoundProteinsFromOrganismList(ValNodePtr pvnTax, Byte chromflags)
{
   ValNodePtr pvnGi = NULL, pvnGilong = NULL;

   /*   if(!SHoundModule("gendb"))
   {
     ErrPostEx(SEV_ERROR,0,0, "SHoundProteinsFromOrganismList: Missing required SeqHound module.");
     return NULL;
	 }*/
   while(pvnTax != NULL)
     {
       pvnGi = SHoundProteinsFromOrganism(pvnTax->data.intvalue, chromflags);
       if (pvnGi != NULL)
        {
          ValNodeLink(&pvnGi, pvnGilong);
          pvnGilong = pvnGi;
          pvnGi = NULL;
        }
       pvnTax = pvnTax->next;
     }

   Misc_IntVnpUnique(&pvnGilong);
   return pvnGilong;
}

/***************************************************************/ 
/** SHoundProteinsFromChromosome
 *
 * Retrieves all protein gis from a given chromosome. Note
 * that this function has been changed to accept NCBI's Chrom Id.
 *
 * @param chromid  NCBI's identifier for the desired chromosome.
 *
 * @return A valnode list of the protein gis.
 *
 */
ValNodePtr LIBCALL SHoundProteinsFromChromosome(Int4 chromid)
{
  Int4 kloodge = 0;
  ValNodePtr pvnGi = NULL;
  Int2 res = 0;

  if (chromid <= 0)
  {
	ErrPostEx(SEV_ERROR,0,0, "SHoundProteinsFromChromosome: "
			  "Invalid chromosome ID.");
	return NULL; 
  }

  pvnGi = GetMoleculeFromChromosome( chromid, "protein" );

  return pvnGi;
}

ValNodePtr   LIBCALL SHoundProteinsFromChromosomeList(ValNodePtr pvnChrom)
{
   ValNodePtr pvnGi = NULL, pvnGilong = NULL;
 
   while (pvnChrom != NULL)
     {
       pvnGi = SHoundProteinsFromChromosome(pvnChrom->data.intvalue);
       if (pvnGi != NULL)
        {
          ValNodeLink(&pvnGi, pvnGilong);
          pvnGilong = pvnGi;
          pvnGi = NULL;
        }
       pvnChrom =  pvnChrom->next;
     }

  Misc_IntVnpUnique(&pvnGilong);
  return pvnGilong;

}


/* returns NCBI chrom id for a given genome */
ValNodePtr LIBCALL SHoundChromosomeFromGenome(Int4 taxid, Byte chromflags)
{
   StChromPtr pch = NULL, pchhead = NULL;
   Int2 res = 0;
   ValNodePtr pvnchrom = NULL, pvnHead = NULL;

   if (taxid <= 0)
     {
       ErrPostEx(SEV_ERROR,0,0, "SHoundChromosomeFromGenome: Invalid taxonomy ID.");
       return NULL;
     }
   if(chromflags <= 0)
     {
       ErrPostEx(SEV_ERROR,0,0, "SHoundChromosomeFromGenome: Invalid chromflag.");
       return NULL;
     }

   pch = StChromNew();
   pchhead = pch;
   pch->taxid = taxid;
   res = SearchCHROM(&pch);
   if (res ==0)
    {
      ErrPostEx(SEV_ERROR,0,0, "SHoundChromosomeFromGenome: SearchCHROM failed.");
      StChromFree(pchhead);
      return NULL;
    }
   if (res == -1)
    {
      StChromFree(pchhead);
      return NULL;
    }
   if (res ==1)
   {
	 Int4 count = 0;
	 while (pch != NULL)
	 {
	   
	   if( pch->chromid >= 0 )
	   {
		 /*--------------------------------------------------------------
		   Translate the flag stored in the database to the appropriate
		   byteflag.
		   ------------------------------------------------------------*/
		 Int4 flag = TranslateChromFlag( pch->chromfl );

		 if( flag & chromflags )
		 {
		    pvnchrom = Slri_ValNodeAddInt(&pvnchrom, 0, pch->chromid);
		 }
		 count++;
	   }
	   pch = pch->next;
	 }
   }
   StChromFree(pchhead);
   return pvnchrom;
}

ValNodePtr LIBCALL SHoundChromosomeFromGenomeList(ValNodePtr pvnGenom, Byte chromflags)
{
   ValNodePtr pvnchrom = NULL, pvnchromlong = NULL;

   while (pvnGenom != NULL)
     {
        pvnchrom =  SHoundChromosomeFromGenome(pvnGenom->data.intvalue, chromflags);
        if (pvnchrom != NULL)
	 {
            ValNodeLink (&pvnchrom, pvnchromlong);
            pvnchromlong = pvnchrom;
            pvnchrom = NULL;
	 }
        pvnGenom = pvnGenom->next;
     }

   Misc_IntVnpUnique(&pvnchromlong);
   return pvnchromlong;
}


ValNodePtr LIBCALL SHoundDNAFromOrganism(Int4 taxid, Byte chromflags)
{
   ValNodePtr pvnchrom = NULL;
   ValNodePtr pvnGi = NULL;
   ValNodePtr pvnUnknownGis = NULL;

   if (taxid <= 0)
     {
         ErrPostEx(SEV_ERROR,0,0, "SHoundDNAFromOrganism: Invalid taxonomy ID.");
         return NULL;
     }
   if (chromflags <= 0)
     {
         ErrPostEx(SEV_ERROR,0,0, "SHoundDNAFromOrganism: Invalid chromflags.");
         return NULL;
     }
   pvnchrom = SHoundChromosomeFromGenome(taxid, chromflags);
   pvnGi = SHoundDNAFromChromosomeList(pvnchrom);

 /*--------------------------------------------------------------
	 Get the NA gis that have not been assigned to a specific
	 chromosome.
	 -------------------------------------------------------------*/
   pvnUnknownGis = SHoundDNAFromUnknownChromosome( taxid );

   if( pvnGi && pvnUnknownGis )
   {
	  pvnGi = ValNodeLink( &pvnGi, pvnUnknownGis );
   }
   else if( pvnUnknownGis && !pvnGi )
   {
	 pvnGi = pvnUnknownGis;
   }
   ValNodeFree(pvnchrom);
   return pvnGi;
}

ValNodePtr   LIBCALL SHoundDNAFromOrganismList(ValNodePtr pvnTax, Byte chromflags)
{
   ValNodePtr pvnGi = NULL, pvnGilong = NULL;

   while(pvnTax != NULL)
     {
       pvnGi = SHoundDNAFromOrganism(pvnTax->data.intvalue, chromflags);
       if (pvnGi != NULL)
        {
          ValNodeLink(&pvnGi, pvnGilong);
          pvnGilong = pvnGi;
          pvnGi = NULL;
        }
       pvnTax = pvnTax->next;
     }

    Misc_IntVnpUnique(&pvnGilong);
    return pvnGilong;

}

/***************************************************************/ 
/** SHoundDNAFromChromosome
 *
 * Retrieves all DNA and RNA gis from a given chromosome. Note
 * that this function has been changed to accept NCBI's Chrom Id.
 *
 * @param chromid  NCBI's identifier for the desired chromosome.
 *
 * @return A valnode list of DNA and RNA gis.
 *
 */
ValNodePtr   LIBCALL SHoundDNAFromChromosome(Int4 chromid)
{
  Int4 kloodge = 0;
  ValNodePtr pvnDNAGis = NULL;
  Int2 res = 0;

  if (chromid == 0)
  {
	ErrPostEx(SEV_ERROR,0,0, "SHoundDNAFromChromosome: "
			  "Invalid chromosome ID.");
	return NULL; 
  }

  pvnDNAGis = GetMoleculeFromChromosome( chromid, "DNA" );

  return pvnDNAGis;
}



ValNodePtr   LIBCALL SHoundDNAFromChromosomeList(ValNodePtr pvnChrom)
{
   ValNodePtr pvnGi = NULL, pvnGilong = NULL;

   while (pvnChrom != NULL)
    {
       pvnGi = SHoundDNAFromChromosome(pvnChrom->data.intvalue);
       if (pvnGi != NULL)
        {
          ValNodeLink(&pvnGi, pvnGilong);
          pvnGilong = pvnGi;
          pvnGi = NULL;
        }
       pvnChrom =  pvnChrom->next;
    }

   Misc_IntVnpUnique(&pvnGilong);
   return pvnGilong;
}

ValNodePtr   LIBCALL SHoundRNAFromChromosomeList(ValNodePtr pvnChrom)
{
   ValNodePtr pvnGi = NULL, pvnGilong = NULL;

   while (pvnChrom != NULL)
    {
       pvnGi = SHoundRNAFromChromosome(pvnChrom->data.intvalue);
       if (pvnGi != NULL)
        {
          ValNodeLink(&pvnGi, pvnGilong);
          pvnGilong = pvnGi;
          pvnGi = NULL;
        }
       pvnChrom =  pvnChrom->next;
    }

   Misc_IntVnpUnique(&pvnGilong);
   return pvnGilong;
}

/* returns gi lists of all DNA from an organism or list of organisms */

ValNodePtr SHoundDNAFromUnknownChromosome( Int4 taxid )
{
  ValNodePtr pvnDNAGis = NULL;
  Int4 chromid = 0;

  if( taxid < 0 )
  {
	ErrPostEx(SEV_ERROR,0,0, "SHoundDNAFromUnknownChromosome: "
			  "Bad taxid");
     return NULL;
  }
  
  chromid = 0 - taxid;
  
  pvnDNAGis = GetMoleculeFromChromosome( chromid, "DNA" );

  return pvnDNAGis;
}

ValNodePtr SHoundProteinsFromUnknownChromosome( Int4 taxid )
{
  ValNodePtr pvnGi = NULL;
  Int4 chromid = 0;

  if( taxid < 0 )
  {
	ErrPostEx(SEV_ERROR,0,0, "SHoundDNAFromUnknownChromosome: "
			  "Bad taxid");
     return NULL;
  }
  
  chromid = 0 - taxid;

  pvnGi = GetMoleculeFromChromosome( chromid, "protein" );

  return pvnGi;
}

ValNodePtr   LIBCALL SHoundRNAFromChromosome(Int4 chromid)
{
  Int4 kloodge = 0;
  ValNodePtr pvnRNAGis = NULL;
  Int2 res = 0;

  if (chromid == 0)
  {
	ErrPostEx(SEV_ERROR,0,0, "SHoundRNAFromChromosome: "
			  "Invalid chromosome ID.");
	return NULL; 
  }

  pvnRNAGis = GetMoleculeFromChromosome( chromid, "RNA" );

  return pvnRNAGis;
}

ValNodePtr   LIBCALL SHoundRNAFromOrganism(Int4 taxid, Byte chromflags)
{
  ValNodePtr pvnchrom = NULL;
  ValNodePtr pvnGi = NULL;
  ValNodePtr pvnUnknownGis = NULL;

  if (taxid <= 0)
  {
	ErrPostEx(SEV_ERROR,0,0, "SHoundRNAFromOrganism: Invalid taxonomy ID.");
	return NULL;
  }
  if (chromflags <= 0)
  {
	ErrPostEx(SEV_ERROR,0,0, "SHoundRNAFromOrganism: Invalid chromflags.");
	return NULL;
  }
  pvnchrom = SHoundChromosomeFromGenome(taxid, chromflags);
  pvnGi = SHoundRNAFromChromosomeList(pvnchrom);

 /*--------------------------------------------------------------
	 Get the NA gis that have not been assigned to a specific
	 chromosome.
	 -------------------------------------------------------------*/
  pvnUnknownGis = SHoundRNAFromUnknownChromosome( taxid );

  if( pvnGi && pvnUnknownGis )
  {
	pvnGi = ValNodeLink( &pvnGi, pvnUnknownGis );
  }
  else if( pvnUnknownGis && !pvnGi )
  {
	pvnGi = pvnUnknownGis;
  }
  ValNodeFree(pvnchrom);
  return pvnGi; 
}
ValNodePtr   LIBCALL SHoundRNAFromUnknownChromosome( Int4 taxid )
{
  ValNodePtr pvnRNAGis = NULL;
  Int4 chromid = 0;

  if( taxid < 0 )
  {
	ErrPostEx(SEV_ERROR,0,0, "SHoundRNAFromUnknownChromosome: "
			  "Bad taxid");
     return NULL;
  }
  
  chromid = 0 - taxid;
  
  pvnRNAGis = GetMoleculeFromChromosome( chromid, "RNA" );

  return pvnRNAGis;
}

/******************************************************/
/*  Structure Fetch  - Biostruc                       */
/******************************************************/

BiostrucPtr LIBCALL SLRI_MMDBBiostrucGet(DocUid uid, Int4 mdlLvl, Int4 maxModels)
{ return SHoundGet3DEx((Int4)uid,mdlLvl,maxModels);}

BiostrucPtr  LIBCALL SHoundGet3DEx(Int4 mmdbid, Int4 mdlLvl, Int4 maxModels)
{
   StMmdbPtr pmd = NULL, pmdhead = NULL;
   BiostrucPtr pbstr = NULL;
   Int2 res = 0;

   if(!SHoundModule("strucdb"))
   {
     ErrPostEx(SEV_ERROR,0,0, "SHoundGet3D: Missing required SeqHound module.");
     return NULL;
   }

   if (mmdbid <= 0)
   {
      ErrPostEx(SEV_ERROR,0,0, "SHoundGet3D: Invalid parameter.");
      return NULL;
   }

   pmd = StMmdbNew();
   pmdhead = pmd;
   pmd->mmdbid = mmdbid;
   res = SearchMMDBEx (&pmd,mdlLvl,maxModels);
   if (res == 0)
    {
      ErrPostEx(SEV_ERROR,0,0, "SHoundGet3: DSeekMMDB failed.");
      StMmdbFree(pmdhead);
      return NULL;
    }
   if (res == -1)
    {
	/*  ErrPostEx(SEV_ERROR,0,0, "GI not found in ASNDB."); */
      StMmdbFree(pmdhead);
      return NULL;
    }
   if (res == 1)
    {
      if (pmd->asn1 == NULL)
        {
          ErrPostEx(SEV_ERROR,0,0, "SHoundGet3D: SeekMMDB failed; Biostruc is NULL .");
          StMmdbFree(pmdhead);
          return NULL;
        }
      else
       {
         pbstr = (BiostrucPtr) AsnIoMemCopy (pmd->asn1, (AsnReadFunc) BiostrucAsnRead,  (AsnWriteFunc) BiostrucAsnWrite);
         if (pbstr == NULL)
          {
             ErrPostEx(SEV_ERROR,0,0, "SHoundGet3D: AsnIoMemCopy failed.");
             StMmdbFree(pmdhead);
             return NULL;
          }
         StMmdbFree(pmdhead);
         return pbstr;
       }
    }
   StMmdbFree(pmdhead);
   return NULL;
}

BiostrucPtr  LIBCALL SHoundGet3D(Int4 mmdbid)
{
	return SHoundGet3DEx(mmdbid, ALLMDL, 100);
}

BiostrucPtr  LIBCALL SHoundGetFixed3D(Int4 mmdbid, Int4 mdlLvl, Int4 maxModels)
{
	return BiostrucPolymerFix(SHoundGet3DEx(mmdbid, mdlLvl,maxModels),1);
}
/* put in mmdblocl routines...  */

BiostrucPtr  LIBCALL SHoundGet3DfromPdbIdEx(CharPtr pdbid, Int4 mdlLvl, Int4 maxModels)
{
  
   Int2 res = 0, i=0;
   CharPtr upper_pdbid;
   StMmdbPtr pmd = NULL, pmdhead = NULL;
   BiostrucPtr pbstr = NULL;
  

   if(!SHoundModule("strucdb"))
   {
     ErrPostEx(SEV_ERROR,0,0, "SHoundGet3DfromPdbId: Missing required SeqHound module.");
     return NULL;
   }

   if (pdbid == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "SHoundGet3DfromPdbId: Invalid parameter.");
      return NULL;
    }
   
   pmd = StMmdbNew();
   pmdhead = pmd;
   /*check input pdbid, converts its lower case letter to upper case*/
   upper_pdbid = (char *)malloc(strlen(pdbid)+1);
   for (i=0; i<StrLen(pdbid);i++){
       if (!isdigit(pdbid[i]) && islower(pdbid[i]))
           upper_pdbid[i]=toupper(pdbid[i]);
	   else
		   upper_pdbid[i]=pdbid[i];
   }
   upper_pdbid[StrLen(pdbid)]='\0';
   pmd->pdbid = StringSave(upper_pdbid);
   res = SearchMMDBEx (&pmd,mdlLvl,maxModels);
   
   if (res == 0)
    {
      ErrPostEx(SEV_ERROR,0,0, "SHoundGet3DfromPdbId: SeekMMDB failed.");
      StMmdbFree(pmdhead);
	  MemFree(upper_pdbid);
      return NULL;
    }
   if (res == -1)
    {
	/*  ErrPostEx(SEV_ERROR,0,0, "GI not found in ASNDB.");  */
      StMmdbFree(pmdhead);
	  MemFree(upper_pdbid);
      return NULL;
    }
   if (res == 1)
    {
      if (pmd->asn1 == NULL)
        {
          ErrPostEx(SEV_ERROR,0,0, "SHoundGet3DfromPdbId: SeekMMDB failed; Biostruc is NULL .");
          StMmdbFree(pmdhead);
		  MemFree(upper_pdbid);
          return NULL;
        }
      else
       {
         pbstr = (BiostrucPtr) AsnIoMemCopy (pmd->asn1, (AsnReadFunc) BiostrucAsnRead,  (AsnWriteFunc) BiostrucAsnWrite);
         if (pbstr == NULL)
          {
             ErrPostEx(SEV_ERROR,0,0, "SHoundGet3DfromPdbId: AsnIoMemCopy failed.");
             StMmdbFree(pmdhead); BiostrucFree(pbstr); MemFree(upper_pdbid);
             return NULL;
          }
         StMmdbFree(pmdhead);
		 MemFree(upper_pdbid);
         return pbstr;
       }
    }
   StMmdbFree(pmdhead);
   MemFree(upper_pdbid);
   return NULL;
}

BiostrucPtr  LIBCALL SHoundGet3DfromPdbId(CharPtr pdbid)
{
	return SHoundGet3DfromPdbIdEx(pdbid, ALLMDL, 100);
}

Boolean LIBCALL SHoundGetXML3D(Int4 mmdbid, CharPtr file_name)
{
    AsnIoPtr aip = NULL;
    BiostrucPtr pstruc = NULL;


   if(!SHoundModule("strucdb"))
   {
     ErrPostEx(SEV_ERROR,0,0, "SHoundGetXML3D: Missing required SeqHound module.");
     return FALSE;
   }

   if (mmdbid <= 0)
    {
       ErrPostEx(SEV_ERROR,0,0, "SHoundGetXML3D: Invalid GI.");
       return FALSE;
    }
   if (file_name == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "SHoundGetXML3D: Invalid file pointer.");
       return FALSE;
    }

   aip = AsnIoOpen(file_name, "wx");
   if(aip == NULL)
   {
     ErrPostEx(SEV_ERROR,0,0, "SHoundGetXML3D: Cannot open %s.\n", IsNullStr(file_name));
     return FALSE;
   }
   pstruc = SHoundGet3D(mmdbid);
   if(!BiostrucAsnWrite(pstruc, aip, NULL))
    {
     ErrPostEx(SEV_ERROR,0,0, "SHoundGetXML3D: BioseqAsnWrite failed.\n", file_name);
     AsnIoClose(aip); BiostrucFree(pstruc);
     return FALSE;
   }
   BiostrucFree(pstruc);
   AsnIoClose(aip);
   return TRUE;
}


Boolean LIBCALL SHoundGetPDB3D(Int4 mmdbid, CharPtr file_name)
{
   BiostrucPtr pstruc = NULL;
   PDNMS pmodelstruc=NULL;
   FILE *pfile = NULL;
   Int2 result = 0;

   if(!SHoundModule("strucdb"))
   {
     ErrPostEx(SEV_ERROR,0,0, "SHoundGetPDB3D: Missing required SeqHound module.");
     return FALSE;
   }
   if (mmdbid <= 0)
    {
       ErrPostEx(SEV_ERROR,0,0, "SHoundGetPDB3D: Invalid GI.");
       return FALSE;
    }
   if (file_name == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "SHoundGetPDB3D: Invalid file pointer.");
       return FALSE;
    }

   pfile = FileOpen(file_name, "w");
   if (pfile == NULL)
    {
      ErrPostEx(SEV_ERROR,2,1,"SHoundGetPDB3D: Unable to open the file %s.", IsNullStr(file_name));
      return FALSE;
    }


   pstruc = SHoundGet3D(mmdbid);
   if(pstruc == NULL)
    {
      ErrPostEx(SEV_ERROR,2,1,"SHoundGetPDB3D: SHoundGet3D failed.");
      FileClose(pfile);
      return FALSE;
    }
   pmodelstruc=MakeAModelstruc(pstruc);
   if(pmodelstruc == NULL)
    {
      ErrPostEx(SEV_ERROR,2,1,"SHoundGetPDB3D: MakeAModelstruc failed.");
      FileClose(pfile); 
      BiostrucFree(pstruc);
      return FALSE;
    }

   result = WritePDBAllModel(pmodelstruc, pfile);
   if(result != 1)
    {
      ErrPostEx(SEV_ERROR,2,1,"SHoundGetPDB3D: WritePDBAllModel failed.");
      FileClose(pfile);
      BiostrucFree(pstruc);
      ClearStructures();
      return FALSE;
    }
   ClearStructures();
   BiostrucFree(pstruc);
   FileClose(pfile);
   return TRUE;
}

/* given mmdbid returns PDBid  */
CharPtr  LIBCALL SHoundPDBFromMMDB(Int4 mmdbid)
{
   StMmdbPtr pmd = NULL, pmdhead = NULL;
   CharPtr   pstr = NULL;
   Int2 res = 0;

   if(!SHoundModule("strucdb"))
   {
     ErrPostEx(SEV_ERROR,0,0, "SHoundPDBFromMMDB: Missing required SeqHound module.");
     return NULL;
   }

   if (mmdbid <=0)
    {
      ErrPostEx(SEV_ERROR,0,0, "SHoundPDBFromMMDB: Invalid parameter.");
      return NULL;
    }

   pmd = StMmdbNew();
   pmdhead = pmd;
   pmd->mmdbid = mmdbid;
   res = SearchMMDB(&pmd);
   if (res == 0)
    {
      ErrPostEx(SEV_ERROR,0,0, "SHoundPDBFromMMDB: SeekMMDB failed.");
      StMmdbFree(pmdhead);
      return NULL;
    }
   if (res == -1)
    {
      StMmdbFree(pmdhead);
      return NULL;
    }
   if (res == 1)
    {
      pstr = StringSave(pmd->pdbid);
      if (pstr == NULL)
      {
        ErrPostEx(SEV_ERROR,0,0, "SHoundPDBFromMMDB: failed to find pdbid.");
        StMmdbFree(pmdhead); MemFree(pstr);
        return NULL;
      }
      StMmdbFree(pmdhead);
      return pstr;
    }
   StMmdbFree(pmdhead);
   return NULL;
}



/*************************************************************************************
*
*   Redundancy
*
*
**************************************************************************************/
/* returns a list of GIs from the same redundant group as the input GI;
if there is only one member of the group it returns just the input GI*/
ValNodePtr LIBCALL SHoundRedundantGroup(Int4 Gi)
{
   StRedundPtr prd = NULL, prd2 = NULL, prdnew = NULL;
   Int2 res = 0;
   Int4 count = 0;
   ValNodePtr pvngi = NULL, pvngihead = NULL;

   if(!SHoundModule("redundb"))
   {
     ErrPostEx(SEV_ERROR,0,0, "SHoundRedundantGroup: Missing required SeqHound module.");
     return NULL;
   }

   if (Gi <= 0)
    {
      ErrPostEx(SEV_ERROR,0,0, "SHoundRedundantGroup: Invalid parameter.");
      return NULL;
    }

   prd = StRedundNew();
   prd->gi = Gi;
   res = SearchREDUND(&prd);
   if (res == 0)
     {
       ErrPostEx(SEV_ERROR,0,0, "SearchREDUND failed.");
       StRedundFree(prd);
       return NULL;
     }
    if (res == -1)
     {
       StRedundFree(prd);
       return NULL;
     }
    if (res == 1)
     {
       if (prd->group == 0)
        {
          ErrPostEx(SEV_ERROR,0,0, "SearchREDUND failed, group is zero.");
          StRedundFree(prd);
          return NULL;
        }
       prd2 = StRedundNew();
       prd2->group = prd->group;
       StRedundFree(prd);
       res = SearchREDUND(&prd2);
       if (res == 0)
        {
          ErrPostEx(SEV_ERROR,0,0, "SearchREDUND failed.");
          StRedundFree(prd2);
          return NULL;
        }
       if (res == -1)
        {
          StRedundFree(prd2);
          return NULL;
        }
       if (res == 1)
        {
          prdnew = prd2;
          while (prdnew != NULL)
           {
             if (prdnew->gi != 0)
              {
                pvngi = ValNodeAddInt(&pvngi, 0, prdnew->gi);
                count++;
                if(count==1)
                   pvngihead = pvngi;
              }
             prdnew = prdnew->next;
           }
          StRedundFree(prd2);
          return pvngihead;
        }
     }
  StRedundFree(prd);
  return NULL;
}


/* returns a list of GIs from all redundant groups for the input GI list;
if there is only one member of the group it returns just the input GI*/
ValNodePtr LIBCALL SHoundRedundantGroupList(ValNodePtr pvnGi)
{
  ValNodePtr pvngilong = NULL;
  ValNodePtr pvngi = NULL;

  if(!SHoundModule("redundb"))
   {
     ErrPostEx(SEV_ERROR,0,0, "SHoundRedundantGroupList: Missing required SeqHound module.");
     return NULL;
   }

  while(pvnGi != NULL)
     {
        pvngi = SHoundRedundantGroup(pvnGi->data.intvalue);
        if (pvngi != NULL)
	  {
            ValNodeLink(&pvngi, pvngilong);
            pvngilong = pvngi;
            pvngi = NULL;
	  }
        pvnGi = pvnGi->next;
     }
   Misc_IntVnpUnique(&pvngilong);
   return pvngilong;
}




/* gets an ID of a redundant group from a GI */
Int4 LIBCALL SHoundRedundantGroupIDFromGI(Int4 Gi)
{
   StRedundPtr prd = NULL;
   Int2 res = 0;
   Int4 group = 0;

   if(!SHoundModule("redundb"))
   {
     ErrPostEx(SEV_ERROR,0,0, "SHoundRedundantGroupIDFromGI: Missing required SeqHound module.");
     return 0;
   }

   if (Gi <= 0)
    {
      ErrPostEx(SEV_ERROR,0,0, "SHoundRedundantGroup: Invalid parameter.");
      return 0;
    }

   prd = StRedundNew();
   prd->gi = Gi;
   res = SearchREDUND(&prd);
   if (res == 0)
     {
       ErrPostEx(SEV_ERROR,0,0, "SearchREDUND failed.");
       StRedundFree(prd);
       return 0;
     }
    if (res == -1)
     {
       StRedundFree(prd);
       return -1;
     }
    if (res == 1)
     {
        if (prd->group > 0)
           group = prd->group;
     }
  StRedundFree(prd);
  return group;
}


/* gets a redundant group ID list from a GI list*/
ValNodePtr LIBCALL SHoundRedundantGroupIDFromGIList(ValNodePtr pvnGi)
{
   ValNodePtr pvngroup = NULL;
   Int4 group = 0;

   if(!SHoundModule("redundb"))
   {
     ErrPostEx(SEV_ERROR,0,0, "SHoundRedundantGroupIDFromGIList: Missing required SeqHound module.");
     return NULL;
   }

   while(pvnGi != NULL)
    {
       group = SHoundRedundantGroupIDFromGI(pvnGi->data.intvalue);
       if (group > 0)
           pvngroup = Slri_ValNodeAddInt(&pvngroup, 0, group);
       pvnGi = pvnGi->next;
    }
   Misc_IntVnpUnique(&pvngroup);
   return pvngroup;
}


/* gets a list of GIs belonging to a group ID */
ValNodePtr LIBCALL SHoundRedundantGroupFromID(Int4 group)
{
   StRedundPtr prd = NULL, prdhead = NULL;
   Int2 res = 0;
   ValNodePtr pvngi = NULL;

   if(!SHoundModule("redundb"))
   {
     ErrPostEx(SEV_ERROR,0,0, "SHoundRedundantGroupFromID: Missing required SeqHound module.");
     return NULL;
   }

   if (group <= 0)
    {
      ErrPostEx(SEV_ERROR,0,0, "SHoundRedundantGroupFromID: Invalid parameter.");
      return NULL;
    }

   prd = StRedundNew();
   prdhead = prd;
   prd->group = group;
   res = SearchREDUND(&prd);
   if (res == 0)
     {
       ErrPostEx(SEV_ERROR,0,0, "SHoundRedundantGroupFromID: SearchREDUND failed.");
       StRedundFree(prdhead);
       return NULL;
     }
    if (res == -1)
     {
       StRedundFree(prdhead);
       return NULL;
     }
    if (res == 1)
     {
       while (prd != NULL)
        {
          if (prd->gi != 0)
             pvngi = Slri_ValNodeAddInt(&pvngi, 0, prd->gi);
          prd = prd->next;
        }
       StRedundFree(prdhead);
       return pvngi;
     }
  StRedundFree(prdhead);
  return NULL;
}


/* gets a list of GIs belonging to a group ID list*/
ValNodePtr LIBCALL SHoundRedundantGroupFromIDList(ValNodePtr pvngroup)
{
  ValNodePtr pvngilong = NULL;
  ValNodePtr pvngi = NULL;

   if(!SHoundModule("redundb"))
   {
     ErrPostEx(SEV_ERROR,0,0, "SHoundRedundantGroupFromIDList: Missing required SeqHound module.");
     return NULL;
   }

   while(pvngroup != NULL)
     {
        pvngi = SHoundRedundantGroupFromID(pvngroup->data.intvalue);
        if (pvngi != NULL)
	  {
            ValNodeLink(&pvngi, pvngilong);
            pvngilong = pvngi;
            pvngi = NULL;
	  }
        pvngroup = pvngroup->next;
     }

   Misc_IntVnpUnique(&pvngilong);
   return pvngilong;
}

/* returns a GI of the best annotated representative from a redundant group ID*/
Int4 LIBCALL SHoundFirstOfRedundantGroupFromID(Int4 group)
{
   StRedundPtr prd = NULL, prdhead = NULL;
   Int2 res = 0;
   Int4 gival = 0;

   if(!SHoundModule("redundb"))
   {
     ErrPostEx(SEV_ERROR,0,0, "SHoundRedundantGroupFromID: Missing required SeqHound module.");
     return 0;
   }

   if (group <= 0)
    {
      ErrPostEx(SEV_ERROR,0,0, "SHoundRedundantGroupFromID: Invalid parameter.");
      return 0;
    }

   prd = StRedundNew();
   prdhead = prd;
   prd->group = group;
   res = SearchREDUND(&prd);
   if (res == 0)
     {
       ErrPostEx(SEV_ERROR,0,0, "SHoundRedundantGroupFromID: SearchREDUND failed.");
       StRedundFree(prdhead);
       return 0;
     }
    if (res == -1)
     {
       StRedundFree(prdhead);
       return -1;
     }
    if (res == 1)
     {
       while (prd != NULL)
        {
          if ((prd->ordinal == 1) && (prd->gi != 0))
           {
             gival = prd->gi;
             StRedundFree(prdhead);
             return gival;
           }
          prd = prd->next;
        }
       StRedundFree(prdhead);
     }
  StRedundFree(prdhead);
  return 0;
}

/* returns a GI list of the best annotated representatives from a redundant group ID list*/
ValNodePtr LIBCALL SHoundFirstOfRedundantGroupFromIDList(ValNodePtr pvngroup)
{
  ValNodePtr pvngi = NULL;
  Int4 gival = 0;

   if(!SHoundModule("redundb"))
   {
     ErrPostEx(SEV_ERROR,0,0, "SHoundRedundantGroupFromIDList: Missing required SeqHound module.");
     return NULL;
   }
   while(pvngroup != NULL)
     {
        gival = SHoundFirstOfRedundantGroupFromID(pvngroup->data.intvalue);
        if (gival != 0)
           pvngi = Slri_ValNodeAddInt(&pvngi, 0, gival);
        pvngroup = pvngroup->next;
     }

   Misc_IntVnpUnique(&pvngi);
   return pvngi;
}



/*************************************************************************************
*
*   Domains
*
*
**************************************************************************************/

/***********************************************/
/* Domains; remote versions not implemented    */
/***********************************************/

static CharPtr        LIBCALL GetSLRIDomainDefline(StDomdbPtr pdd);
static CharPtr        LIBCALL SLRIGetCompound(Int4 gi);
static Boolean        LIBCALL WriteSLRIDomainSequence(CharPtr pseq, CharPtr PNTR ppseqfin, Int4 from, Int4 to, Boolean Xout);

SLRIValNodePtr LIBCALL SHoundGetDomain(Int4 gi, Int2 domno)
{
   StDomdbPtr pdd = NULL, pddhead = NULL;
   SLRIValNodePtr pvndom = NULL;
   Int2 res = 0;

   if(!SHoundModule("strucdb"))
   {
     ErrPostEx(SEV_ERROR,0,0, "SHoundGetDomain: Missing required SeqHound module.");
     return NULL;
   }

   if(gi <= 0)
   {
     ErrPostEx(SEV_ERROR,0,0, "SHoundGetDomain: Invalid param.\n");
     return NULL;
   }

   pdd = StDomdbNew();
   pddhead = pdd;
   pdd->gi = gi;
   res = SearchDOMDB(&pdd);
   if (res == 0)
       ErrPostEx(SEV_ERROR,0,0, "SHoundGetDomain: SearchDOMDB failed.\n");
   if (res == -1)
       ErrPostEx(SEV_ERROR,0,0, "SHoundGetDomain: GI not found in DOMDB.\n");
   if (res == 1)
    {
       while(pdd != NULL)
        {
          if ((pdd->domno == domno) && (pdd->asn1 != NULL))
           {
              pvndom = (SLRIValNodePtr) AsnIoMemCopy (pdd->asn1, (AsnReadFunc) SLRIValNodeAsnRead,  (AsnWriteFunc) SLRIValNodeAsnWrite);
              if (pvndom == NULL)
               {
                 ErrPostEx(SEV_ERROR,0,0, "SHoundGetDomain: AsnIoMemCopy failed.\n");
                 StDomdbFree(pddhead);
                 return NULL;
               }
           }
          pdd = pdd->next;
        }
     }
   StDomdbFree(pddhead);
   return pvndom;
}

SLRIFastaPtr LIBCALL SHoundGetFastaDomain(Int4 domid, Int4 tolerance)
{
   SLRIFastaPtr pfa = NULL;
   StDomdbPtr pdd = NULL;
   Int4 res = 0;

   if(!SHoundModule("strucdb"))
   {
     ErrPostEx(SEV_ERROR,0,0, "SHoundGetFastaDomain: Missing required SeqHound module.");
     return NULL;
   }

   if(domid <= 0)
   {
     ErrPostEx(SEV_ERROR,0,0, "SHoundGetFastaDomain: Invalid param.\n");
     return NULL;
   }

   pdd = StDomdbNew();
   pdd->domid = domid;
   res = SearchDOMDB(&pdd);
   if(res == 0)
    {
      ErrPostEx(SEV_ERROR,0,0, "SHoundGetFastaDomain: SearchDOMDB failed.");
      StDomdbFree(pdd);      
      return NULL;
    }
   if (res == -1)
    {
      ErrPostEx(SEV_ERROR,0,0, "SHoundGetFastaDomain: Domid %ld not found in DOMDB database.", (long) domid);
      StDomdbFree(pdd);
      return NULL;
    }
   if (res == 1)
    {
      pfa = SHoundMakeFastaDomain(pdd, tolerance, 0);
      if(pfa == NULL)
       {
         ErrPostEx(SEV_ERROR,0,0, "SHoundGetFastaDomain: SHoundMakeFastaDomain failed on domid %ld.", (long) domid);
	 StDomdbFree(pdd);
         return NULL;
       }
    }

   StDomdbFree(pdd);
   return pfa;
}



/* returns the slrifasta structure including domain sequence and defline, tolerance window
 specifies a maximum number of internal Xs in the sequence to include into the domain
(for not completely resolved structures) */
SLRIFastaPtr LIBCALL SHoundMakeFastaDomain(StDomdbPtr pdd, Int4 tolerance, Int4 minilen)
{

   SLRIFastaPtr pfa = NULL, pfafin = NULL;
   SLRIValNodePtr pvndom = NULL, pvndomhead = NULL, pvndomlen = NULL;
   CharPtr pseq = NULL, pseqfin = NULL;
   Int4 last = 0, first = 0;
   SLRIDomainPtr pval = NULL, pvalnext;

   if(pdd == NULL)
   {
     ErrPostEx(SEV_ERROR,0,0, "SHoundMakeFastaDomain: Invalid param.\n");
     return NULL;
   }
   if(minilen == 0)
   {
     ErrPostEx(SEV_ERROR,0,0, "SHoundMakeFastaDomain: Invalid param.\n");
     return NULL;
   }

   /* get domain-describing valnode from DOMDB */
   pvndom = pdd->asn1;
   if (pvndom == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "SHoundMakeFastaDomain: SHoundGetDomain failed.");
      return NULL;
    }

   pvndomhead = pvndom;
   pvndomlen = pvndom;

   /* get sequence for given gi */
   pfa = SHoundGetFasta(pdd->gi);
   if (pfa == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "SHoundMakeFastaDomain: SHoundGetFasta failed.");
      return NULL;
    }
   pseq = pfa->sequence;
   if (pseq == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "SHoundMakeFastaDomain: The sequence is NULL.");
      SLRIFastaFree(pfa);
      return NULL;
    }

    pval = pvndomlen->data.ptrvalue;
    /* SK: NULL ptr check */
    if ( pval != NULL )
	first = pval->from;

   /* check if the specified residue numbers are in range of the sequence length */
   while (pvndomlen != NULL)
    {
      pval = pvndomlen->data.ptrvalue;
      /* SK: NULL ptr check */
      if ( pval != NULL )
          last = pval->to;
      pvndomlen = pvndomlen->next;
    }
   if (last > (Int4) strlen(pseq))
    {
      ErrPostEx(SEV_ERROR,0,0, "SHoundMakeFastaDomain: The sequence is shorter then specified domain.");
      SLRIFastaFree(pfa);
      return NULL;
    }
   if ((last-first)< minilen)
    {
      ErrPostEx(SEV_ERROR,0,0, "SHoundMakeFastaDomain: The domain is shorter then specified minimal domain legth. last: %ld, first %ld, domid: %ld", (long) last, (long) first, (long)pdd->domid);
      SLRIFastaFree(pfa);
      return NULL;
    }

   /* allocate memory for the new domain sequence string */
   pseqfin = MemNew(strlen(pseq) + 1);

   /* make domain sequence */
   /* fill the begining with X */
   pval = pvndom->data.ptrvalue;
   /* SK: NULL ptr check */
   if ( pval != NULL )
   {
     if (pval->from > 1)   /*check the tolerance*/
     {
       if (pval->from <= tolerance+1)
         WriteSLRIDomainSequence(pseq, &pseqfin, 0, pval->from-2, FALSE);
       else
         WriteSLRIDomainSequence(pseq, &pseqfin, 0, pval->from-2, TRUE);
     }
   }


   /* write the body of the domain */
   while (pvndom != NULL)
    {
      pval = pvndom->data.ptrvalue;
      /* SK: NULL ptr check */
      if ( pval != NULL )
      {
         WriteSLRIDomainSequence(pseq, &pseqfin, pval->from-1, pval->to-1, FALSE);
         /* if the node is not last, process the tolerance level value */
         if (pval->to != last)
         {
            pvalnext = (pvndom->next)->data.ptrvalue;
            if (pvalnext->from - pval->to <= tolerance+1)
               WriteSLRIDomainSequence(pseq, &pseqfin, pval->to, pvalnext->from-2, FALSE);  /* write the sequence in the gap */
            else
               WriteSLRIDomainSequence(pseq, &pseqfin, pval->to, pvalnext->from-2, TRUE);   /* write 'X's in the sequence */
         }
      }
      pvndom = pvndom->next;
    }
   /* SK: NULL ptr check */
   if ( pval != NULL )
   {
       /* finish writing all X to the end */
       if (pval->to < (Int4) strlen(pseq))
       {
           if ((Int4)strlen(pseq) - pval->to <= tolerance)      /*check the tolerance*/
               WriteSLRIDomainSequence(pseq, &pseqfin, pval->to, (unsigned)strlen(pseq)-1, FALSE);
           else
               WriteSLRIDomainSequence(pseq, &pseqfin, pval->to, (unsigned) strlen(pseq)-1, TRUE);
       }
   }

  /* allocate a new SLRIFasta structure and fill it */
   pfafin = SLRIFastaNew();
   pfafin->sequence = pseqfin;
   pfafin->defline = GetSLRIDomainDefline(pdd);   /* fetches defline */

   /* free all the structures */
   SLRIValNodeFree(pvndomhead);
   SLRIFastaFree(pfa);

   return pfafin;
}


/* makes definition line for domains */
static CharPtr LIBCALL GetSLRIDomainDefline(StDomdbPtr pdd)
{

  /* SLRIValNodePtr pvndom = NULL;*/

   CharPtr comp = NULL;
   CharPtr defline = NULL;
   Char pdomid[15];
   Char pgi[15];
   Char pdomno[5];
   Int4 deflinelen = 0;
   CharPtr pdbid;
   CharPtr chain;

   if(pdd == NULL)
   {
     ErrPostEx(SEV_ERROR,0,0, "GetSLRIDomainDefline: Invalid param.\n");
     return NULL;
   }

   if ((comp = SLRIGetCompound(pdd->gi)) == NULL)
        ErrPostEx(SEV_ERROR,0,0, "GetSLRIDomainDefline: SLRIGetCompound returned NULL.\n");
   sprintf(pgi, "%ld", (long) pdd->gi);
   sprintf(pdomid, "%ld", (long) pdd->domid);
   sprintf(pdomno, "%ld", (long) pdd->domno);
   pdbid = pdd->pdbid;
   chain = pdd->chain;
   deflinelen = strlen(pgi) + strlen(pdomid) + strlen(pdbid) + strlen(pdomno) +  strlen(comp)+ strlen(chain) + 35;
   defline = MemNew( (unsigned) deflinelen);
   strcat(defline, ">gi|");
   strcat(defline, pgi);
   strcat(defline, "|gnl|vast|");
   strcat(defline, pdomid);
   strcat(defline, "|pdb|");
   strcat(defline, pdbid);
   strcat(defline, "|");
   strcat(defline, chain);
   strcat(defline, " Domain ");
   strcat(defline, pdomno);
   strcat(defline, " ");
   strcat(defline, comp);

   MemFree(comp);
   return defline;
}


/* fetches sequence title from ACCDB database */
static CharPtr LIBCALL SLRIGetCompound(Int4 gi)
{
   StAccdbPtr pac = NULL, pachead = NULL;
   CharPtr comp = NULL;
   Int2 res = 0;

   if (gi <= 0)
     {
       ErrPostEx(SEV_ERROR,0,0, "SLRIGetCompound: Invalid GI.");
       return NULL;
     }
   pac = StAccdbNew();
   pachead = pac;
   pac->gi = gi;
   res = SearchACCDB(&pac);
   if (res == 0)
    {
       ErrPostEx(SEV_ERROR,0,0, "SLRIGetCompound: SearchACCDB failed.");
       StAccdbFree(pachead);
       return NULL;
    }
   if (res == -1)
    {
       StAccdbFree(pachead);
       return NULL;
    }
   if (res == 1)
         comp = StringSave(pac->title);
   StAccdbFree(pachead);
   return comp;
}


/* writes sequence into a string in interval from-to, if Xout is TRUE it fills the interval with Xs */
static Boolean LIBCALL WriteSLRIDomainSequence(CharPtr pseq, CharPtr PNTR ppseqfin, Int4 from, Int4 to, Boolean Xout)
{
   CharPtr pseqfin = NULL;
   Int4 i = 0;


   if (pseq == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "WriteSLRIDomainSequence: Invalid parameter.");
      return FALSE;
    }
   pseqfin = *ppseqfin;

   for (i = from; i<= to; i++)
    {
      if (Xout == FALSE)
          pseqfin[i] = pseq[i];
      else
          pseqfin[i] = 'X';
    }

   return TRUE;
}





/*************************************************
*
*   Gary - taxonomy
*
*
**************************************************/


/*Get direct children tax ID of a given tax ID*/
ValNodePtr LIBCALL SHoundGetTaxChildNodes(Int4 taxid)
{
	ValNodePtr vnpRet=NULL;
	SLRITaxonPtr stp=NULL;

	if(!SHoundModule("taxdb"))
	{
		ErrPostEx(SEV_ERROR,0,0, "SHoundGetTaxChildNodes: Missing required SeqHound module.");
		return NULL;
	}

	if(taxid<0) {
		ErrPostEx(SEV_INFO, 0, 0, "SHoundGetTaxChildNodes: Passed taxid was < 0.");
		return(NULL);
	}

	stp = DB_GetTaxRec(taxid);
	if(stp==NULL) {
		ErrPostEx(SEV_INFO, 0, 0, "SHoundGetTaxChildNodes: DB_GetTaxRec failed for taxid %ld.", taxid);
		return(NULL);
	}

	if(stp->children_taxId != NULL) {
		vnpRet = Misc_ValNodeIntegerCopy(stp->children_taxId);
	}

	stp = SLRITaxonFree(stp);
	return(vnpRet);
}


/*Get direct children tax IDs of a given list of tax ID*/
ValNodePtr LIBCALL SHoundGetTaxChildNodesList(ValNodePtr taxidlist)
{
	ValNodePtr vnpRet=NULL, vnpTmp=NULL;
	Int4 taxid=0;

	if(!SHoundModule("taxdb")) {
		ErrPostEx(SEV_ERROR,0,0, "TaxDB_GetChildNodes: Missing required SeqHound module.");
		return NULL;
	}

	if(taxidlist==NULL) {
		ErrPostEx(SEV_INFO, 0, 0, "TaxDB_GetChildNodesList: Passed taxIdList was NULL.");
		return(NULL);
	}

	while(taxidlist!=NULL) {
		taxid = taxidlist->data.intvalue;
		vnpTmp=SHoundGetTaxChildNodes(taxid);
		while(vnpTmp!=NULL) {
			Misc_AddInt2SortedList(&vnpRet, vnpTmp->data.intvalue, 0, TRUE);
			vnpTmp = vnpTmp->next;
		}
		taxidlist = taxidlist->next;
	}

	return(vnpRet);
}


/*Get all children recursively of a given tax ID (get everything lower in the tree) internal function*/
Int2 SHoundGetAllTaxProgenyInternal(Int4 taxid, ValNodePtr *vnp)
{
	ValNodePtr vnpTmp=NULL;


	if(taxid<0) {
		ErrPostEx(SEV_INFO, 0, 0, "SHoundGetAllTaxProgenyInternal: Passed taxid was < 0.");
		return(-1);
	}

	vnpTmp=SHoundGetTaxChildNodes(taxid);
	while(vnpTmp!=NULL) {
		SHoundGetAllTaxProgenyInternal(vnpTmp->data.intvalue, vnp);
		Misc_AddInt2SortedList(vnp, vnpTmp->data.intvalue, 0, TRUE);
		vnpTmp = vnpTmp->next;
	}

	return(0);
}

/*Get all children recursively of a given tax ID (get everything lower in the tree)*/
/*Warning: calling this function with high taxid nodes, like 1 will take a very long time.  If you want
that many taxIDs returned, call the function once, save the results and use them instead of calling the function
repeatedly*/
ValNodePtr LIBCALL SHoundGetAllTaxProgeny(Int4 taxid)
{
	ValNodePtr vnpRet=NULL;

	if(!SHoundModule("taxdb")) {
		ErrPostEx(SEV_ERROR,0,0, "SHoundGetAllTaxProgeny: Missing required SeqHound module.");
		return NULL;
	}

	if(taxid<0) {
		ErrPostEx(SEV_INFO, 0, 0, "SHoundGetAllTaxProgeny: Passed taxid was < 0.");
		return(NULL);
	}

	SHoundGetAllTaxProgenyInternal(taxid, &vnpRet);

	return(vnpRet);
}

/*Get parent tax ID of a given tax ID*/
Int4 LIBCALL SHoundGetTaxParent(Int4 taxid)
{
	SLRITaxonPtr stp=NULL;
	Int4 parentId=0;

        if(!SHoundModule("taxdb"))
	{
		ErrPostEx(SEV_ERROR,0,0, "SHoundGetTaxParent: Missing required SeqHound module.");
		return (-1);
	}

        if(taxid<0) {
		ErrPostEx(SEV_INFO, 0, 0, "SHoundGetTaxParent: Passed taxid was < 0.");
		return(-1);
	}

	stp = DB_GetTaxRec(taxid);
	if(stp==NULL) {
		ErrPostEx(SEV_INFO, 0, 0, "SHoundGetTaxParent: DB_GetTaxRec failed for taxid %ld.", taxid);
		return(-1);
	}

	parentId = stp->parent_taxId;
	stp = SLRITaxonFree(stp);

	return(parentId);
}

/*Get all ancestors of a given tax ID - internal function*/
Int2 SHoundGetAllTaxAncestorsInternal(Int4 taxid, ValNodePtr *vnp)
{
	Int4 parentTaxId=0;
	ValNodePtr vnpTmp=NULL;

	if(taxid<0) {
		ErrPostEx(SEV_INFO, 0, 0, "SHoundGetAllTaxAncestorsInternal: Passed taxid was < 0.");
		return(-1);
	}

	if(taxid==1) {
		/*taxid 1 is the root node which has itself as a parent*/
		return(0);
	}

	parentTaxId=TaxDB_GetParent(taxid);
	if(parentTaxId<0) {
		return(-1);
	}
	if(parentTaxId!=1) {
		/*don't add taxid 1 to the list since it is redundant*/
		/*add numbers to head of list*/
		if(*vnp!=NULL) {
			ValNodeAddInt(&vnpTmp, 0, parentTaxId);
			/*add to head of list*/
			vnpTmp->next = *vnp;
			*vnp = vnpTmp;
		}
		else {
			ValNodeAddInt(vnp, 0, parentTaxId);
		}
	}
	SHoundGetAllTaxAncestorsInternal(parentTaxId, vnp);

	return(0);
}

/*Get all ancestors of a given tax ID*/
ValNodePtr LIBCALL SHoundGetAllTaxAncestors(Int4 taxid)
{
	ValNodePtr vnpRet=NULL;

	if(!SHoundModule("taxdb")) {
		ErrPostEx(SEV_ERROR,0,0, "SHoundGetAllTaxAncestors: Missing required SeqHound module.");
		return NULL;
	}

	if(taxid<0) {
		ErrPostEx(SEV_INFO, 0, 0, "SHoundGetAllTaxAncestors: Passed taxid was < 0.");
		return(NULL);
	}

	SHoundGetAllTaxAncestorsInternal(taxid, &vnpRet);

	return(vnpRet);
}

/*finds if a given old tax ID has been merged into a new one*/
Boolean LIBCALL SHoundIsTaxMerged(Int4 taxid)
{
	if(!SHoundModule("taxdb")) {
		ErrPostEx(SEV_ERROR,0,0, "SHoundIsTaxMerged: Missing required SeqHound module.");
		return FALSE;
	}

	if(DB_GetTaxMergedNodeRecNewTIDByOldTID(taxid) <= 0) {
		return(FALSE);
	}

	return(TRUE);
}

/*returns a new taxID for a possible given merged old taxID
  if input taxID is not merged or does not exist, returns -1 */
Int4 LIBCALL SHoundGetNewTaxIDFromMergedTaxID(Int4 taxid)
{
    Int4 newTaxID = 0;
	
    if(taxid<0) {
		ErrPostEx(SEV_INFO, 0, 0, "SHoundGetNewTaxIDFromMergedTaxID: Passed taxid was < 0.");
		return(-1);
	}
	if (!SHoundModule("taxdb"))
	{
	        ErrPostEx(SEV_ERROR,0,0, "SHoundGetNewTaxIDFromMergedTaxID: Missing required SeqHound module.");
		return (-1);
	}
	newTaxID = DB_GetTaxMergedNodeRecNewTIDByOldTID(taxid);
	if (newTaxID <= 0) {
		return -1;
	}
	return newTaxID;
}

/*finds if a given tax ID has been deleted*/
Boolean LIBCALL SHoundIsTaxDeleted(Int4 taxid)
{
	if(!SHoundModule("taxdb")) {
		ErrPostEx(SEV_ERROR,0,0, "SHoundIsTaxDeleted: Missing required SeqHound module.");
		return FALSE;
	}

	if(DB_GetTaxDelNodeRec(taxid) <= 0) {
		return(FALSE);
	}

	return(TRUE);
}

/*Get taxonomy name from tax ID by name class (see objslritax.h for ASN.1 enumeration defines)*/
/*Returns NULL if name or tax ID not found.  Allocates memory for name - caller must free returned string.*/
CharPtr LIBCALL SHoundGetTaxNameFromTaxIDByClass(Int4 taxid, Uint2 nameClass)
{
	SLRITaxonPtr stp=NULL;
	SLRITaxonNamePtr stnp=NULL;
	CharPtr name=NULL;

	if(!SHoundModule("taxdb")) {
		ErrPostEx(SEV_ERROR,0,0, "SHoundGetTaxNameFromTaxIDByClass: Missing required SeqHound module.");
		return NULL;
	}
	if(taxid<0) {
		ErrPostEx(SEV_INFO, 0, 0, "SHoundGetTaxNameFromTaxIDByClass: Passed taxid was < 0.");
		return(NULL);
	}

	stp = DB_GetTaxRec(taxid);
	if(stp==NULL) {
		ErrPostEx(SEV_INFO, 0, 0, "SHoundGetTaxNameFromTaxIDByClass: DB_GetTaxRec failed for taxid %ld.", taxid);
		return(NULL);
	}

	stnp = stp->names;
	while(stnp!=NULL) {
		if(stnp->name_class == nameClass) {
			name=StringSave(stnp->name);
			if(name==NULL) {
				ErrPostEx(SEV_INFO, 0, 0, "SHoundGetTaxNameFromTaxIDByClass: StringSave failed for taxid %ld.", taxid);
				SLRITaxonFree(stp);
				return(NULL);
			}
			break;
		}
		stnp=stnp->next;
	}

	stp = SLRITaxonFree(stp);

	return(name);
}

/*Get taxonomy name from tax ID (scientific name)*/
/*if given taxid is deleted, returns NULL, if given taxid is merged, find the new taxid and return the name*/
/*Returns NULL if tax name not found.  Allocates memory for name - caller must free returned string.*/
CharPtr LIBCALL SHoundGetTaxNameFromTaxID(Int4 taxid)
{
	CharPtr name = NULL;
	Int4 newTaxID =0;
	if(!SHoundModule("taxdb")) {
		ErrPostEx(SEV_ERROR,0,0, "SHoundGetTaxNameFromTaxID: Missing required SeqHound module.");
		return NULL;
	}

	if(taxid<0) {
		ErrPostEx(SEV_INFO, 0, 0, "SHoundGetTaxNameFromTaxID: Passed taxid was < 0.");
		return(NULL);
	}
    /*bug 3474. added Sept 24, 2004*/
	if(SHoundIsTaxDeleted(taxid)){
		ErrPostEx(SEV_INFO, 0, 0, "SHoundGetTaxNameFromTaxID: Passed taxid was deleted from taxonomy tree.");
		return NULL;
	}
    name = SHoundGetTaxNameFromTaxIDByClass(taxid, SLRI_taxon_name_name_class_scientific_name);
	if (name == NULL){
		newTaxID = SHoundGetNewTaxIDFromMergedTaxID(taxid);
		if (newTaxID >0)
			return (SHoundGetTaxNameFromTaxIDByClass(newTaxID, SLRI_taxon_name_name_class_scientific_name));
	}
    return name;
}

/*Get lineage from tax ID*/
/*Type: use type defines:
TAXDB_GetLineage_FULL - gets full lineage
TAXDB_GetLineage_GENBANK - gets lineage as it would appear in a genbank record
TAXDB_GetLineage_TAXDBSHORT - gets lineage as it would appear on the taxonomy database homepage (tax IDs with no rank are filtered)*/
/*Returns NULL if tax ID not found.  Allocates memory for lineage - caller must free returned string.*/
CharPtr LIBCALL SHoundGetTaxLineageFromTaxID(Int4 taxid, Uint2 type)
{
	CharPtr lineage=NULL, name=NULL;
	ValNodePtr lineTaxIds=NULL, lineTaxIdsHead=NULL;
	ByteStorePtr bsp=NULL;
	SLRITaxonPtr stp=NULL;
	SLRITaxonNamePtr stnp=NULL;
	SLRITaxonRankPtr strp=NULL;
	Boolean start=TRUE;

	if(!SHoundModule("taxdb")) {
		ErrPostEx(SEV_ERROR,0,0, "SHoundGetTaxLineageFromTaxID: Missing required SeqHound module.");
		return NULL;
	}

	if(taxid<0) {
		ErrPostEx(SEV_INFO, 0, 0, "SHoundGetTaxLineageFromTaxID: Passed taxid was < 0.");
		return(NULL);
	}

	/*initialize bytestore*/
	bsp = BSNew(50);
	if(bsp==NULL) {
		ErrPostEx(SEV_INFO, 0, 0, "SHoundGetTaxLineageFromTaxID: BSNew failed on taxid %ld.", taxid);
		return(NULL);
	}

	lineTaxIds = SHoundGetAllTaxAncestors(taxid);
	if(lineTaxIds==NULL) {
		ErrPostEx(SEV_INFO, 0, 0, "SHoundGetTaxLineageFromTaxID: SHoundGetAllTaxAncestors failed on taxid %ld.", taxid);
		BSFree(bsp);
		return(NULL);
	}
	lineTaxIdsHead = lineTaxIds;
	while(lineTaxIds != NULL) {
		stp = DB_GetTaxRec(lineTaxIds->data.intvalue);
		if(stp==NULL) {
			ErrPostEx(SEV_INFO, 0, 0, "SHoundGetTaxLineageFromTaxID: DB_GetTaxRec failed for taxid %ld.", lineTaxIds->data.intvalue);
			BSFree(bsp);
			return(NULL);
		}
		/*only add to normal lineage if genbank hidden flag is not set*/
		/* SK: NULL ptr check */
		if(((type==SHoundGetTaxLineage_GENBANK) && (!stp->genbank_hidden)) || (type==SHoundGetTaxLineage_FULL)
			|| ((type==SHoundGetTaxLineage_TAXDBSHORT)&&(!stp->genbank_hidden)&&(stp->rank != NULL)&&(stp->rank->rank != SLRI_taxon_rank_rank_none))) {
			/*find scientific name*/
			stnp = stp->names;
			while(stnp!=NULL) {
				if(stnp->name_class == SLRI_taxon_name_name_class_scientific_name) {
					name=stnp->name;
					break;
				}
				stnp=stnp->next;
			}
			/*return a lineage without the final species name, if the taxid refers to a species*/
			strp = stp->rank;
			/* SK: NULL ptr check */
			if((strp != NULL) && !((strp->rank == SLRI_taxon_rank_rank_species)&&(strp->premod == SLRI_taxon_rank_premod_none)
				&&(strp->postmod == SLRI_taxon_rank_postmod_none))) {
				if(start) {
					BSprintf(bsp, "%s", IsNullStr(name));
					start=FALSE;
				}
				else {
					BSprintf(bsp, "; %s", IsNullStr(name));
				}
			}
		}
		lineTaxIds = lineTaxIds->next;
	}
	lineTaxIdsHead = ValNodeFree(lineTaxIdsHead);

	stp = SLRITaxonFree(stp);
	lineage = BSStringSave(bsp);
	bsp = BSFree(bsp);

	return(lineage);
}


/***************************************
* Doron - Coding Region SLRIFasta fetch
*
***************************************/
SLRIFastaPtr LIBCALL IntrezGetCDSLRIFasta(Int4 Gi)
{
        StCddbPtr pcd=NULL, pcdhead=NULL;
        SLRIFastaPtr fsp=NULL;
        Int2 res=0;
      
        if(Gi<=0)
        {
                ErrPostEx(SEV_ERROR,0,0,"IntrezGetCDSLRIFasta: Invalid GI");
                 return NULL; 
        }
        pcd = StCddbNew();
        pcdhead = pcd;
        pcd->gi = Gi;
        res = SearchCDDB (&pcd);
        if (res == 0)
        {
                ErrPostEx(SEV_ERROR,0,0,"IntrezGetCDSLRIFasta: SearchCDDB failed.");
                 StCddbFree(pcdhead);
                return NULL;
         }

        if (res == -1)
        {
        /*  ErrPostEx(SEV_ERROR,0,0, "GI not found in CDDB."); */
                StCddbFree(pcdhead);
                return NULL;
        }
        if (res == 1)
        {
                if (pcd->asn1 == NULL)
                {
                        ErrPostEx(SEV_ERROR,0,0,"IntrezGetCDSLRIFasta: SearchCDDB failed");
                        StCddbFree(pcdhead);
                        return NULL;
                 }
                else  
                {
                        fsp =(SLRIFastaPtr)AsnIoMemCopy(pcd->asn1,(AsnReadFunc)SLRIFastaAsnRead,(AsnWriteFunc)SLRIFastaAsnWrite);
                          if (fsp == NULL)
                        {
                                
				ErrPostEx(SEV_ERROR,0,0,"IntrezGetCDSLRIFasta: searchCDSLRIFasta failed.AsnIoMemCopy failed");
			         StCddbFree(pcdhead);
                                return NULL;
                        }
                        StCddbFree(pcdhead);
                        return fsp;

		}
	}
	StCddbFree(pcdhead);
	return NULL;
} 


SLRI_ERR LIBCALL SHoundAllGenomesWithScientificNames(CLinkSetPtr PNTR pAllGenomes) 
{
	ValNodePtr pvnGenomes = NULL, pvnGenomesHead = NULL;
	Int4 i = 0, num = 0, itaxid = 0;
	CharPtr pctaxname = NULL;
	CLinkSetPtr cptr = NULL;

	/* check to see that the modules are here */
	if(!SHoundModule("taxdb")) {
		ErrPostEx(SEV_ERROR,0,0, "SHoundAllGenomesWithScientificNames: Missing required SeqHound module.");
		return FALSE;
	}


	if((pvnGenomes = pvnGenomesHead = SHoundAllGenomes()) == NULL) {
		ErrPostEx(SEV_ERROR,0,0,"SHoundAllGenomesWithScientificNames: No Complete Genomes Found!!!!");
		return SLRI_FAIL;
	}
	
	num = ValNodeLen(pvnGenomes);
	if((cptr = CLinkSetNew2(num)) == NULL) {
		ErrPostEx(SEV_ERROR,0,0,"Not enough memory");
		ValNodeFree(pvnGenomes);
		return SLRI_FAIL;
	}

	for(i = num-1; i >= 0; i--) {
		itaxid = pvnGenomes->data.intvalue;
		pctaxname = SHoundGetTaxNameFromTaxID(itaxid);
		if(pctaxname == NULL) pctaxname = StringSave("unknown");
		
		cptr->uids[i] = itaxid;
		cptr->weights[i] = pctaxname;

		pvnGenomes = pvnGenomes->next;
	}
	pvnGenomes = pvnGenomesHead = ValNodeFree(pvnGenomesHead);
	
	*pAllGenomes = cptr;
	return SLRI_SUCCESS;
}

SLRI_ERR LIBCALL SHoundAllGenomesWithNumProteins(ILinkSetPtr PNTR pAllGenomes)
{
	ValNodePtr pvnGenomes = NULL, pvnGenomesHead = NULL;
	Int4 i = 0, num = 0, itaxid = 0, num_prot = 0;
	ILinkSetPtr iptr = NULL;
	ValNodePtr vnp = NULL;

	/* check to see that the modules are here */
/*	if(!SHoundModule("taxdb")) {
		ErrPostEx(SEV_ERROR,0,0, "SHoundAllGenomesWithNames: Missing required SeqHound module.");
		return FALSE;
	}
*/

	if((pvnGenomes = pvnGenomesHead = SHoundAllGenomes()) == NULL) {
		ErrPostEx(SEV_ERROR,0,0,"SHoundAllGenomesWithNumProteins: No Complete Genomes Found!!!!");
		return SLRI_FAIL;
	}
	
	num = ValNodeLen(pvnGenomes);
	if((iptr = ILinkSetNew2(num)) == NULL) {
		ErrPostEx(SEV_ERROR,0,0,"Not enough memory");
		ValNodeFree(pvnGenomes);
		return SLRI_FAIL;
	}

	for(i = num-1; i >= 0; i--) {
		itaxid = pvnGenomes->data.intvalue;
		vnp = SHoundProteinsFromOrganism(itaxid, CHROM_ALL);
		num_prot = ValNodeLen(vnp);
		ValNodeFree(vnp);
		
		iptr->uids[i] = itaxid;
		iptr->weights[i] = num_prot;

		pvnGenomes = pvnGenomes->next;
	}
	pvnGenomes = pvnGenomesHead = ValNodeFree(pvnGenomesHead);
	
	*pAllGenomes = iptr;
	return SLRI_SUCCESS;
}

/***************************************************
 * HISTDB Functions
 ***************************************************/
CharPtr LIBCALL SHoundGetAccFromRetiredGi(Int4 Gi)
{
   StHistdbNodePtr pHis = NULL;
   CharPtr pAccession = NULL;
   if(Gi < 1)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[SHoundGetAccFromRetiredGi]: Invalid Gi");
     goto end;
   }
   pHis = StHistdbNodeNew();
   pHis->gi = Gi;

   if(SearchHISTDB(pHis)) 
   {
     pAccession = (char*)malloc( (strlen(pHis->access)+1) * sizeof(char) );
     strcpy(pAccession, pHis->access);
     TrimSpacesAroundString(pAccession);
   } 
   StHistdbNodeFree(pHis);
   return pAccession;
end:
   return NULL;
}


/*********************
 * RY: March, 2004
 *retrieve a list of Fasta formatted data corresponding to the input list of Gis
 *param: a list of Gis
 *return: a ValNode list wiht query-return sequences
 *        if it is a query node, set choice==0, data == int gi
 *        if it is a return value node, set choice == 1, data = SLRIFasta structure (can be NULL)
 **********************/
ValNodePtr LIBCALL SHoundGetFastaKeyedList(ValNodePtr pvnGi)
{
   Int4 gi = 0;
   ValNodePtr pvnFasta = NULL;
   SLRIFastaPtr pfa = NULL;

   while (pvnGi != NULL)
   {
        gi = pvnGi->data.intvalue;
	/*add query node, set choice==0, data=int gi*/  
        ValNodeAddInt(&pvnFasta, 0, gi); 
	   
	pfa = SHoundGetFasta(gi);
	
        /*add return value node, set choice==1, data= SLRIFasta structure*/
	pvnFasta = Slri_ValNodeAddPointerAtEnd(&pvnFasta, 1, pfa);
	
        pvnGi = pvnGi->next;
   }
   return pvnFasta;
}


/*******************
 *RY: March, 2004
 *retrieve a list of groups of redundant gis associated with the input gi list
 *param: a list of Gis
 *return: a ValNode list wiht query-return sequences
 *        if it is a query node, set choice==0, data == int gi
 *        if it is a return value node, set choice == 1, data = ptrvalue to a ValNodeList (can be NULL)
 *        this ValNodeList is a list of redudant gis associated the previous query gi value
 **********************/
ValNodePtr LIBCALL SHoundRedundantGroupKeyedList(ValNodePtr pvnGi)
{
  Int4 gi=0;
  ValNodePtr pvngilong = NULL;
  ValNodePtr pvngi = NULL;

  if(!SHoundModule("redundb"))
  {
     ErrPostEx(SEV_ERROR,0,0, "SHoundRedundantGroupKeyedList: Missing required SeqHound module.");
     return NULL;
  }

  while(pvnGi != NULL)
  {
        gi = pvnGi->data.intvalue;
	
	/*add query node, set choice==0, data=int gi*/  
        ValNodeAddInt(&pvngilong, 0, gi); 
        pvngi = SHoundRedundantGroup(pvnGi->data.intvalue);
	
	/*add return value node, set choice==1, data= ValNodePtr structure*/
	pvngilong = Slri_ValNodeAddPointerAtEnd(&pvngilong, 1, pvngi);
	
	pvnGi = pvnGi->next;
   }
      
   return pvngilong;
}


/*Given a Redundant groupID, return a fasta for any of gi in this groupID
 *param : a redundant group id
 *return: a pointer to a SLRIFasta structure
 */
SLRIFastaPtr LIBCALL SHoundGetFastaFromRedundantGroupID(Int4 groupID)
{
    Int4 gi = 0;
    SLRIFastaPtr pfa = NULL;
    ValNodePtr pgi = NULL, tmp=NULL;


   if (groupID <= 0)
   {
       ErrPostEx(SEV_ERROR,0,0, "[SHoundGetFastaFromRedundantGroupID]: Invalid groupID.");
       return NULL;
   }
   /*retrieves the first of the redundant group for the given a group id*/
   gi = SHoundFirstOfRedundantGroupFromID (groupID );
   /*if hava a valid gi as the first of the redundant group, then use this gi to retrieve fasta structure*/
   if (gi > 0)
   {
       pfa = SHoundGetFasta(gi);
   }
   /*if miss the first of redundant group, or first gi of the redundant group retrieves a NULL fasta structure,
     then retrieve a redundant group for the given group id*/
   if ( gi <= 0 || pfa == NULL)
   {
       pgi = SHoundRedundantGroupFromID(groupID);
       tmp = pgi;
       while ( tmp != NULL ) {
           gi = tmp->data.intvalue;
	   if (gi > 0)
	   {
	        pfa = SHoundGetFasta(gi);
	        if ( pfa != NULL ) 
	        break;
	   }
	   tmp =tmp->next;
	}
	if (pgi) ValNodeFree(pgi);
    }     
    return pfa;
}

/*Given a list of Redundant groupID, return a list of fasta for corresponding gi in the list of groupID
 *param : a list of redundant group ids
 *return: a ValNode list wiht query-return sequences
 *        if it is a query node, set choice==0, data == int redundant group id
 *        if it is a return value node, set choice == 1, data = ptrvalue to a SLRIFasta (can be NULL)
 */
ValNodePtr LIBCALL SHoundGetFastaFromRedundantGroupIDKeyedList(ValNodePtr pvnGroupID)
{
   Int4 groupID=0;
   ValNodePtr pvnFasta = NULL;
   SLRIFastaPtr pfa = NULL;

   while (pvnGroupID != NULL)
   {
        groupID = pvnGroupID->data.intvalue;
	/*add query node, set choice==0, data=int groupid*/  
        ValNodeAddInt(&pvnFasta, 0, groupID); 
	   
	pfa = SHoundGetFastaFromRedundantGroupID(groupID);
	
        /*add return value node, set choice==1, data= SLRIFasta structure*/
	pvnFasta = Slri_ValNodeAddPointerAtEnd(&pvnFasta, 1, pfa);
	
        pvnGroupID = pvnGroupID->next;
   }
   return pvnFasta;
}




/*
$Log: intrez.c,v $
Revision 1.84  2005/04/14 18:21:26  vgu
Bug 5354 fix: Initialize integer pointer gis in function SHoundDNAFromTaxIDIII

Revision 1.83  2005/03/10 19:29:28  ryao
Modified SHoundProteinsFromTaxIDIII and SHoundDNAFromTaxIDIII to free allocated memory int* gis; Modified Check_precomputed_index() to fix core dump. bug [5050] and [4803] related

Revision 1.82  2005/03/09 17:00:50  eburgess
Changed acc to access in histdb. Modified SHoundGetAccFromRetiredGi()
accordingly.

Revision 1.81  2005/03/02 16:48:59  eburgess
Added API calls SHoundRNAFromChromosome, SHoundRNAFromOrganism,
SHoundRNAFromUnknownChromosome, and SHoundRNAFromOrganismList. Bug 4914

Revision 1.80  2005/03/01 22:16:53  ryao
minor change on SHoundGetNewTaxIDFromMergedTaxID

Revision 1.79  2005/02/15 21:05:45  eburgess
Added InitCOMGEN() where InitCodebase() is called.

Revision 1.78  2005/02/01 16:47:37  eburgess
Changed comgen apis to work with the new comgen code.

Revision 1.77  2005/01/24 21:54:08  vgu
Bug 4254 fix: change memory leak code

Revision 1.76  2005/01/06 19:17:02  zwang
Added two new APIs - SHoundStartTransaction and SHoundEndTransaction - for users to put their process into a transaction.

Revision 1.75  2004/12/02 22:55:37  mmatan
in function getGenBankffList, the function will now tolerate gi's which don't exist in seqhound being in the list.  In these cases, a placeholder error message will be output to the gbff list, and the function will continue through the rest of the list, rather than simply quitting half way through the list.  This fixes bug 4068.

Revision 1.74  2004/11/24 22:14:08  ryao
fix bug in SHoundAccFromGi, when multiple records match one gi, returns the non 'n/a' accession as possible. see bug [4005]

Revision 1.73  2004/11/22 14:56:58  mmatan

Modified shoundgetfixed3d so that is takes the same parameters as shoundget3dex.
See bug 3930 for details.

Revision 1.72  2004/11/18 19:04:51  mmatan
Added new function SHoundGetFixed3D which functions like SHoundGet3D, but
"fixes" the biostruc before returning it.  See bug 3930.

Revision 1.71  2004/11/15 19:14:27  mmatan
Altered function SHoundGetGenBankff so that it uses the built in functionality
of the the SeqEntryToGbnk to determine which bioseq to generate the flat file
for, rather that hacking at the seqentry input to do so.  This fixes bug 3906.

Revision 1.70  2004/11/10 16:44:02  mmatan
Changed parameters passed to SeqEntryToGenBank so that genbank flat files
generated are identical to NCBI's version.  This resolve bug 3199.

Revision 1.69  2004/10/27 18:38:22  eburgess
Changed SHoundDNAFromChromosome() and SHoundProteinsFromChromosome() to work
with the new comgen modules. Minor changes to other comgen functions. Bug
2952

Revision 1.68  2004/09/30 17:46:57  ryao
Added a new function to retrieve merged taxid: SHoundGetNewTaxIDFromMergedTaxID; Modified SHoundGetTaxNameFromTaxID: to retrieve a valid tax name from taxid even this taxid is merged. bug [3474]

Revision 1.67  2004/09/14 17:23:05  mmatan
Moved phy-set specific code into the if-then conditional block executed when
phy-sets are encountered.  This fixes bug 3370.

Revision 1.66  2004/09/14 15:26:17  mmatan
Added test to prevent null pointer error, related to bug 3370.

Revision 1.65  2004/09/10 15:52:39  mmatan
Changed format of defline so that if accession is null, it will insert a blank
space in between the '|' delimiters, rather than ommitting that field
altogether.  This is to resolve bug 3239.

Revision 1.64  2004/08/31 16:04:52  mmatan
Modified SHoundGetGenbankff to deal with phy-set type seq-entries in a logical manner.  This goes towards fixing bug 3087.

Revision 1.63  2004/08/20 14:57:17  eburgess
Fixed bug in SHoundGetBioseqSet() where StSendbFree(psdhead) was being
called twice.

Revision 1.62  2004/08/12 17:55:49  zwang
New API added to get title by gi from ACCDB

Revision 1.61  2004/06/30 15:05:11  hlieu
add unique to referenceidbygi

Revision 1.60  2004/06/18 22:18:53  hlieu
changes to include for bug 1162, new GenBankff format

Revision 1.59  2004/06/16 17:59:22  ryao
Modified SHoundGetFastaFromRedundantGroupID to retrieve fasta structure even if the first in the redundant group is missing.

Revision 1.58  2004/06/16 15:01:24  hlieu
returning fix to past version

Revision 1.57  2004/06/16 14:58:58  hlieu
fix small error

Revision 1.56  2004/06/10 16:59:32  zwang
Don't establish database connection at LL_InitCodeBase and OpenCodeBaseNbr, which are used to create SQLHandle for search functions only. See bug #2324.

Revision 1.55  2004/06/08 21:10:08  zwang
Include rpsdb_db.h for ODBC only. Remove LL_InitUndoCodeBase from SHoundFini to let ODBC connection freed by CloseCodeBase.

Revision 1.54  2004/06/07 21:57:12  hlieu
removed unnecessary variables

Revision 1.53  2004/06/04 15:13:57  hlieu
fix to prevent redefintion, bug 2185

Revision 1.52  2004/06/02 14:29:47  zwang
Add #ifdef ODBC to call LL_InitCodeBase and OpenCodeBaseNbr in order to create SQLHandle for soem search functions. See bug #2324.
Include rpsdb_db.h.

Revision 1.51  2004/05/31 22:07:54  hlieu
fix warning, bug 2185

Revision 1.50  2004/05/31 21:45:33  hlieu
got rid of // comments, bug 2185, compiler error on mork

Revision 1.49  2004/05/31 21:11:26  hlieu
compiler warning fixes, bug 2185

Revision 1.48  2004/05/31 20:36:17  hlieu
minor compiler warning fixes for bug 2185

Revision 1.47  2004/05/20 18:26:02  vgu
add conditional compile for GenBankff for ODBC vs CODEBASe

Revision 1.46  2004/05/17 19:56:51  hlieu
fix cross comp error

Revision 1.45  2004/05/10 15:57:22  ryao
removed unused variables according to crosscompileError

Revision 1.44  2004/05/07 16:20:36  rcavero
Re-implement function SHoundGetGenBankff() because NCBI depricate function
SeqEntryToFlatEx().
This new implementation use SeqEntryToGnbk().

Revision 1.43  2004/05/03 13:27:38  eburgess
Fix cross-compile errors: do not indent #include statements.

Revision 1.42  2004/04/29 17:05:17  mmatan
Included nullstring check of pac->title in SHoundGetDefline, which was causing
crash on Solaris.  This partially resolves bug 2166.

Revision 1.41  2004/04/22 15:46:09  hlieu
fix inadvertent code check in

Revision 1.40  2004/04/21 21:07:05  hlieu
fix bug 1759, info instead of error on taxid 0

Revision 1.39  2004/04/07 21:06:49  ryao
Added SHoundGetFastaFromRedundantGroupID SHoundGetFastaFromRedundantGroupIDKeyedList

Revision 1.38  2004/04/05 16:27:24  ryao
added SHoundGetFastaKeyedList SHoundRedundantGroupKeyedList

Revision 1.37  2004/03/19 20:51:33  vgu
Modify SHoundMoleculeType to search GI from TAXGI instead of ASNDB

Revision 1.36  2004/03/19 19:39:40  vgu
Minor change in updating:w error log message

Revision 1.35  2004/03/19 19:31:25  vgu
Modifed SHoundIsProtein to search the table TAXGI for molecule type re:bug1854 addgoid parser performance tuning

Revision 1.34  2004/03/15 17:48:03  zwang
For ODBC, get max(mmdbid) from database directly.

Revision 1.33  2004/03/11 16:31:27  hlieu
turned off NetEntrez access

Revision 1.32  2004/03/05 15:35:13  hlieu
changed protein report error log to info log

Revision 1.31  2004/02/28 23:53:10  hfeldman
improved error message

Revision 1.30  2004/02/27 19:34:11  ryao
fix bug in SHoundGiFromPDBchain. Allow the function to accept both upper case and lower case pdbcode and chain.

Revision 1.29  2004/02/19 20:22:34  zwang
Fixed bug in SHoundIsTaxDeleted and SHoundIsTaxMerged. See bug #1663

Revision 1.28  2004/02/19 17:17:15  zwang
Check if list is null in SHoundGetGenBankffList.

Revision 1.27  2004/02/10 20:19:17  rcavero
SHoundModule("dbxref") ODBC don't need to open any file

Revision 1.26  2004/02/05 20:30:39  ryao
allow SHoundFindName to check if input as an empty string, fix bug 1483

Revision 1.25  2004/01/22 16:07:21  zwang
Include _odbc.h when ODBC is defined, otherwise include _db.h

Revision 1.24  2004/01/13 21:39:38  rcavero
nblast_odbc it's a draf version for nblast's ODBC port.

Revision 1.23  2004/01/12 22:20:51  ryao
minor change

Revision 1.22  2004/01/12 21:50:17  hlieu
fixed bug in string comparisons in SHoundMoleculeType

Revision 1.21  2004/01/09 22:11:35  ryao
SHoundGetLargestMMDBID reinialize mmdbid=0

Revision 1.20  2004/01/08 19:29:25  hlieu
removed getline not compatiable with Solaris OS. replace with gets

Revision 1.19  2004/01/07 20:45:13  hlieu
Added SHoundGetLargestMMDBID

Revision 1.18  2004/01/06 22:32:15  skuznets
Code audit has been done sucessfully.
Possible NULL pointer issues has been eliminated.

Revision 1.17  2003/11/25 15:51:06  ryao
fixed bug in SHoundSequenceLength

Revision 1.16  2003/11/03 22:52:14  haocl
minor modifications

Revision 1.15  2003/11/03 20:41:00  ryao
added SHoundPDBFromMMDB allow user to query pdbid from given mmdbid

Revision 1.14  2003/10/31 22:51:18  haocl
comments on protein report

Revision 1.13  2003/10/31 22:35:36  haocl
better commenting of protein reports, removed unused variables,

Revision 1.12  2003/10/24 17:53:11  feldman
Added SHoundGet3DEx function to allow user to choose model level and num models when fetching biostruc

Revision 1.11  2003/10/02 21:56:43  volodya
fixed SHoundInit

Revision 1.10  2003/10/02 21:33:13  haocl
added HISTDB to SeqHoundInit and SeqHoundFini

Revision 1.9  2003/09/29 21:33:39  haocl
merged objll.c, objshproteinreport.c into objslristruc.c,
intrez, ll_query reflect merging, NBlastasn, objslristruc and objslritax
newly generated

Revision 1.8  2003/09/16 20:59:37  haocl
fixed proteinreport functions, set the booleans in the descriptionset

Revision 1.7  2003/09/03 19:26:21  volodya
added function SHoundGetNameByGi

Revision 1.6  2003/09/03 17:49:03  ryao
changed prototype of SHoundMoleculeType

Revision 1.5  2003/09/02 15:37:15  haocl
changed ecode to evidence_code

Revision 1.4  2003/08/28 21:07:38  haocl
changed some fields of GoAnnotNode

Revision 1.3  2003/08/26 15:23:57  haocl
trimed spaces around retiredGi

Revision 1.2  2003/08/22 13:35:10  haocl
added SHoundGetAccFromRetiredGi and SearchHISTDB

Revision 1.1.1.25  2003/08/21 08:48:18  root
Automated import from Sourceforge.net

Revision 1.30  2003/08/20 19:43:11  haocl
added code for protein report

Revision 1.29  2003/08/13 17:16:28  haocl
added SHoundSequenceLength

Revision 1.28  2003/07/25 19:53:39  haocl
added new functionality associated with the new PUBSEQ database.
SHoundGetReferenceIDFromGi[List] && SHoundGiFromReferenceID[List]

Revision 1.27  2003/07/23 16:22:09  haocl
changed SHoundMoleculeType, protein = 0, dna = 1, rna = 2
to match the webinterface

Revision 1.26  2003/07/22 20:25:38  haocl
added SHoundMoleculeType

Revision 1.25  2003/05/31 01:31:49  kaca
flat file fix, has to be optimized

Revision 1.24  2003/05/23 02:18:52  kaca
fixed flat file fetch; shows only one sequence per gi

Revision 1.23  2003/01/09 16:50:25  haocl
fixed SHoundFindSeqId memory freeing and added some more memory freeing

Revision 1.22  2003/01/06 15:30:17  micheld
Fixed typo

Revision 1.21  2003/01/03 22:36:40  haocl
added memory freeing

Revision 1.20  2002/11/03 23:23:40  betel
Fixed bug in SHoundFini

Revision 1.19  2002/09/27 16:44:38  kaca
added read only mode for SHound initialization

Revision 1.18  2002/07/08 16:58:02  kaca
renamed two RPS-BLAST functions Accession is DomainId

Revision 1.17  2002/04/02 23:42:03  kaca
changed RPSDB name in SHoundInit

Revision 1.16  2002/04/02 23:07:38  kaca
added RPS tables initialization to SHoundInit

Revision 1.15  2002/03/25 07:51:32  micheld
New genome functions. changed includes to new slrilinkset.h

Revision 1.14  2002/02/10 19:54:20  mdumontier
abstracted internals of SHoundProteinsFromTaxID and SHoundDNAFromTaxID to database layer

Revision 1.13  2002/02/06 23:56:45  micheld
Changed TaxDB_ Functions to SHound*Tax* function call, legacy preserved for C api but *NOT* PERL

Revision 1.12  2002/01/23 23:24:55  kaca
changed SHoundIsInited function and moved it from intrez_cb.c to intrez.c

Revision 1.11  2002/01/15 16:41:12  kaca
changed scope of SHoundMakeFastaDomain

Revision 1.10  2002/01/15 16:09:42  kaca
renamed PrintFastaSequence into PrintSLRIFastaSequence

Revision 1.9  2001/12/19 23:32:38  kaca
changed logic of SHoundProteins/DNAFromTaxID

Revision 1.8  2001/11/26 16:12:05  kaca
removed unused variables

Revision 1.7  2001/11/21 23:51:16  kaca
database opening controled by CREATE Boolean

Revision 1.6  2001/11/19 21:20:51  kaca
declared some domains functions static

Revision 1.5  2001/11/13 23:21:14  kaca
checking definition lines for commas in GetDefline they would break remote API

Revision 1.4  2001/11/13 22:01:24  kaca
implemented SeqHound modules; check if your config is up to date

Revision 1.3  2001/11/08 22:18:37  kaca
removed comments intended for internal use only

Revision 1.2  2001/11/08 22:08:50  kaca
changed SHoundFindName string handling

Revision 1.1.1.2  2001/10/10 21:53:47  kaca
initial import

Revision 1.64  2001/09/10 16:49:38  michel
added SLRI_MMDBBiostrucGet to call MMDBGetBiostrucGet (for clustal library)

Revision 1.63  2001/09/06 17:48:20  kaca
changes to accomodate new locus link parser

Revision 1.62  2001/09/06 16:52:38  kaca
small changes to accomodate new GO parser executable

Revision 1.61  2001/09/04 21:33:58  michel
Added local equivalent of the set and get timeout functions for local API (which do nothing)
Removed compiler warning in ll_db.h

Revision 1.60  2001/09/04 21:25:44  michel
Moved mmdblocl.c out of seqhound local library so that it can be directly compiled with the cbmmdb parser
This allows clustlib to use MMDBBiostrucGet with SeqHound (which calls SHoundGet3D)
or without SeqHound (in which case mmdblocl.c must be compiled with application)
SeqHound's MMDBBiostrucGet in mmdblocl.c has been renamed to MMDBBiostrucGetHere

Revision 1.59  2001/09/03 20:31:08  michel
Mainly fixed compiler warnings and changed CODE4 reference for GO and LL

Revision 1.58  2001/08/24 20:59:44  michel
removed SEQHOUNDNBRS compile flag from library
added new functions for 3DNeighbours database retrieval
temporary fix for redundant neighbours coming from NBLAST

Revision 1.57  2001/08/15 22:27:52  kaca
added includes

Revision 1.56  2001/08/15 21:17:01  kaca
added database closing for GO and locus link

Revision 1.55  2001/08/15 21:14:13  kaca
added datab open for GO databases

Revision 1.53  2001/08/15 18:11:47  michel
Added pathnbr for config file to find neighbour database
Added additional functions and error checking

Revision 1.52  2001/08/02 18:24:25  michel
Added #ifdef SEQHOUNDNBRS to exlude neighbour database and calls until
testing is complete and databases are fully available
Added path info for remotelibraries

Revision 1.51  2001/08/02 16:13:32  michel
Added Neighbour Retrieval to SeqHound local

Revision 1.50  2001/07/24 19:50:04  kaca
added text index specific retrieval of SeqEntries

Revision 1.49  2001/05/28 21:32:06  kaca
more error testing and memory freeing

Revision 1.48  2001/04/27 23:12:28  kaca
added SHoundFindName and SHoundFindNameList functions

Revision 1.47  2001/04/27 21:57:39  kaca
added minimal domain length check to makefastadomain

Revision 1.46  2001/04/19 21:50:10  kaca
change in fopen mode in Check_precomputed

Revision 1.45  2001/04/12 20:59:22  kaca
cosmetic change

Revision 1.44  2001/04/06 22:48:12  kaca
memory management in Check_precomputed; while loop into for loop

Revision 1.43  2001/04/05 14:12:08  doron
Removed  within comment

Revision 1.42  2001/03/29 15:59:12  gbader
Change include order

Revision 1.41  2001/03/29 15:44:39  kaca
moved ExpandSrqEntry from intrez.c to intrez_db.c

Revision 1.40  2001/03/28 22:45:31  gbader
Added required LIBCALLs to functions to ones that were missing (Tax lib function)

Revision 1.39  2001/03/27 21:10:41  doron
Addition of CDDB (coding region database) functions

Revision 1.38  2001/03/26 22:49:45  kaca
removed unused variables

Revision 1.37  2001/03/26 22:34:06  kaca
created slri+misc and moved some API functions into it

Revision 1.36  2001/03/26 19:58:33  kaca
changes in include files

Revision 1.35  2001/03/19 22:50:30  kaca
moved some Get*FromTaxIdIII from intrez_db.c to intrez.c

Revision 1.34  2001/03/15 18:03:58  gbader
Added SHoundGetCurrentServer function

Revision 1.33  2001/03/15 00:07:53  kaca
XML functions for sequences and structures

Revision 1.32  2001/03/12 22:52:48  kaca
removed unused variables

Revision 1.31  2001/03/09 21:40:37  kaca
small fix

Revision 1.30  2001/03/09 20:09:25  kaca
freeing functions now return input param type


Revision 1.27  2001/03/09 16:41:38  gbader
Updated PrintSLRIFastaList to use PrintSLRIFasta

Revision 1.26  2001/03/09 16:29:13  kaca
added PrintSLRIFastaList function

Revision 1.25  2001/03/08 22:35:49  kaca
small fix

Revision 1.24  2001/03/08 15:42:52  kaca
removed valnode/string functions which migratedinto slrilib

Revision 1.23  2001/02/23 22:40:13  kaca
new functions

Revision 1.22  2001/02/23 20:43:25  kaca
license change

Revision 1.21  2001/02/21 22:41:14  kaca
fasta bug fixed

Revision 1.20  2001/02/20 23:22:09  kaca
new fasta domain related functions

Revision 1.19  2001/02/08 21:29:15  kaca
removed vastdb struc definition

Revision 1.18  2001/02/06 20:15:21  kaca
minor change

Revision 1.17  2001/02/05 14:18:40  kaca
minor change

Revision 1.15  2001/01/26 19:20:05  kaca
removed unused variables

Revision 1.14  2001/01/24 16:33:26  kaca
bzip new library

Revision 1.13  2000/12/22 20:38:50  kaca
removed hand written database structures, added functions for storing records

Revision 1.12  2000/11/06 22:18:16  kaca
removed obsolete variable initialization

Revision 1.10  2000/11/02 20:44:13  kaca
speed up on ValNodeLink use

Revision 1.9  2000/10/24 21:02:25  kaca
minor change

Revision 1.8  2000/10/24 17:37:34  kaca
error checking with expanding seqentries

Revision 1.6  2000/10/16 20:38:21  doron
precompute changed DNA to NA

Revision 1.5  2000/10/16 20:16:11  kaca
removed define of generated code

Revision 1.4  2000/10/13 21:54:22  kaca
changed includes

Revision 1.3  2000/09/20 18:49:59  kaca
split SeqHound headers into private and public one and changed includes

Revision 1.2  2000/09/19 22:14:54  kaca
BytePtr cast for AsnIoMemOpen

Revision 1.1.1.1  2000/09/18 17:41:56  kaca
Initial import

Revision 1.8  2000/08/31 14:23:11  ruth
added precomputed searches to DNAfromTaxid and proteinfortaxid.  Added precomputed index functions

Revision 1.7  2000/08/30 15:58:09  ruth
added config function definitions

Revision 1.6  2000/08/28 14:24:49  doron
seqhound/intrez.c

Revision 1.5  2000/08/21 21:07:31  kaca
removed TAXSP and parts of TAXGI

Revision 1.4  2000/07/20 20:14:58  doron
modified IntrezBioseqToFasta added SLRIBioseqToFasta

Revision 1.3  2000/07/12 17:30:19  kaca
ValNodeAdd* changed to faster Slri_ValNodeAdd

*/
