/* $Id: vastblst.c,v 1.5 2004/12/14 16:48:51 zwang Exp $
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
  March 2000
  makes CodeBase database (DOMDB) from MMDB structures and outputs fasta file for
  structural domains


******************************************************************************************/
#include <intrez_db.h>
#include <intrez.h>

#define LOGING "vastblstlog"

#define NUMARGS 1
Args myargs[NUMARGS] = {
        {"Is this an update", "F", NULL, NULL, FALSE, 'u', ARG_BOOLEAN, 0.0, 0, NULL}
        };

#define SPLINK "mmdbdom.fas"

StDomdbPtr pdd;
ValNodePtr pvnres = NULL;
Int4 rowCount=0;

SLRIValNodePtr LIBCALL MakeDomain(Int4 gi, FILE *pFile);
static void LIBCALLBACK DoDomainSeq(PFB pfbThis, Int4 iModel, Int4 iIndex, Pointer ptr);
void LIBCALL WriteFASTAByDomain(PDNMM pdnmmThis, FILE *File, Int4 LineLen);
void LIBCALL WriteFASTADomain(PDNMS pdnmsThis,  FILE *pFile);
SLRIValNodePtr  SLRISeqLocFree(SLRIValNodePtr pvnres);

Int4 chaincount;

SLRIValNodePtr LIBCALL MakeDomain(Int4 gi, FILE *pFile)
{
  Int4 from = 0;
  Int4 to = 0;
  ValNodePtr pvnresthis = NULL;
  SLRIDomainPtr pdom = NULL;
  SLRIValNodePtr pvnfin = NULL;


  if (gi <= 0)
  {
      ErrPostEx(SEV_ERROR, 0, 0, "MakeDomain: Invalid paremeter.");
      return NULL;
  }

  if(pFile == NULL)
  {
      ErrPostEx(SEV_ERROR, 0, 0, "MakeDomain: Invalid paremeter.");
      return NULL;
  }

  /* SK: NULL ptr check */
  if(pvnres == NULL)
  {
      ErrPostEx(SEV_ERROR, 0, 0, "MakeDomain: pvnres is NULL.");
      return NULL;
  }

  pvnresthis = pvnres;
  fprintf(pFile, "\n");
  while (pvnresthis->next != NULL)
   {
     if ((pvnresthis->choice == 0) && (pvnresthis->next->choice == 1))
        from = pvnresthis->next->data.intvalue;
     if ((pvnresthis->choice == 1) && (pvnresthis->next->choice == 0))
      {
         to = pvnresthis->data.intvalue;
         if(!SHoundExists(gi))
          {
            ErrPostEx(SEV_ERROR, 0, 0,"GI %ld not in the system.", gi);
            /* fprintf(pFile, "%ld\n", gi); */
          }
         pdom = SLRIDomainNew();
	 /* SK: NULL ptr check */
	 if ( pdom == NULL )
         {
            ErrPostEx(SEV_ERROR, 0, 0,"GI %ld not in the system.", gi);
	    return NULL;
         }

         pdom->gi = gi;
         pdom->from = from;
         pdom->to  = to;
         ValNodeAddPointer(&pvnfin, SLRINode_domain, pdom);
         /*fprintf(pFile, "%ld-%ld\n", from, to);*/
      }
     pvnresthis = pvnresthis->next;
   }
  return (SLRIValNodePtr) pvnfin;
}


static Boolean ResidueHasLoc(PMGD pmgdThis)
{
  PVNMA pvnmaThis;
  PMAD pmadThis;

  Boolean bFound;

  if (!pmgdThis)
   {
      ErrPostEx(SEV_ERROR, 0, 0,"ResidueHasLoc: invalid argument\n");
      return FALSE;
   }
  bFound = FALSE;
  pvnmaThis = pmgdThis->pvnmaAHead;
  while (pvnmaThis != NULL)
  {
      pmadThis = (PMAD) pvnmaThis->data.ptrvalue;
      /* SK: NULL ptr check
      if (pmadThis->pvnalLocate != NULL)
      */
      if (pmadThis != NULL && pmadThis->pvnalLocate != NULL)
      {
         bFound = TRUE;
      }
      pvnmaThis = pvnmaThis->next;
  }
  return bFound;
}


static void LIBCALLBACK DoDomainSeq(PFB pfbThis, Int4 iModel, Int4 iIndex, Pointer ptr)
{
  FILE *pFile = NULL;
  PMGD pmgdRes = NULL;


  if (!pfbThis) return;
  if (ptr) pFile = (FILE *) ptr;
    else return;
  
    if (pfbThis->bMe == AM_MGD)
      {
          /* cast to the correct PMGD pointer type */
          pmgdRes = (PMGD) pfbThis;
          /* prints 1 letter code  */
          /* for absentee residues in structure, always Xed out...*/
	  /* SK: NULL ptr check */
          if (pmgdRes != NULL) {
           if (!ResidueHasLoc(pmgdRes))
           {
                fprintf(pFile, "X");
                ValNodeAddInt(&pvnres, 0, pmgdRes->pdnmgLink->choice);
           }
           /* for the current domain number */
           else
           {
               if ((pmgdRes->pcIUPAC) && (pmgdRes->iDomain == (Int2) iModel))
                {
                  fprintf(pFile, "%c", pmgdRes->pcIUPAC[0]);
		  /* SK: NULL ptr check */
		  if ( pmgdRes->pdnmgLink != NULL )
                     ValNodeAddInt(&pvnres, 1, pmgdRes->pdnmgLink->choice);
                }
               else
                {
                  fprintf(pFile, "X");
                  ValNodeAddInt(&pvnres, 0, pmgdRes->pdnmgLink->choice);
                }

           }
         }

          /* this version should always masking off residues without 
             coordinates, even when NCBI domain splitter leaves them
             with domain numbering, as it did with 3TS1... */
	  /* SK: NULL ptr check */
          if ((pmgdRes->pdnmgLink != NULL) && (pmgdRes->pdnmgLink->choice % iIndex == 0))
               fprintf(pFile,"\n");

      }
     return;
}


void LIBCALL WriteFASTAByDomain(PDNMM pdnmmThis, FILE *File, Int4 LineLen)
{
   PMMD pmmdThis = NULL; 
   PMSD pmsdThis = NULL;
   ValNodePtr pvnDom = NULL;
   ValNodePtr pvnDomHead = NULL;
   PDNMG pdnmgThis = NULL;
   PMGD pmgdThis = NULL;
   Int4 iDomain = 0;
   Int4 iHashVal = 0;
   Int4 count = 0;
   SLRIValNodePtr pvnintervals = NULL;

   
   if (!pdnmmThis) return;
   if (!File) return;
   
   if (LineLen <= 0)
     LineLen = 60;
   pmmdThis = (PMMD) pdnmmThis->data.ptrvalue;
   /* SK: NULL ptr check */
   if ( pmmdThis == NULL)
      return;
   pmsdThis = ToMSDParent((PFB) pmmdThis);
   /* SK: NULL ptr check */
   if ( pmsdThis == NULL)
      return;
   pdnmgThis = pmmdThis->pdnmgHead;
   while (pdnmgThis != NULL)
       {  
         pmgdThis = (PMGD) pdnmgThis->data.ptrvalue;
	 /* SK: NULL ptr check */
	 if ( pmgdThis != NULL )
	 {
            pvnDom =  NULL;
            pvnDom = ValNodeFindNext(pvnDomHead, NULL, pmgdThis->iDomain);
            if (!pvnDom)
            {
               /*printf("%ld\n", (long) pmgdThis->iDomain);*/
               ValNodeAddInt(&pvnDomHead, pmgdThis->iDomain, pmgdThis->iDomain);
               count++;
             }
	 }
	 pdnmgThis = pdnmgThis->next;
       }
   /*printf("total %ld\n", (long) count);*/
   pvnDom = pvnDomHead;
   if ((pmmdThis->iResCount > 1) && (pvnDom != NULL)) 
   while (pvnDom)
     {
        iDomain = pvnDom->data.intvalue;
        iHashVal = 0;
        iHashVal = ((Int4) pmsdThis->iMMDBid * 10000);
        iHashVal = iHashVal + (pdnmmThis->choice * 100);
        iHashVal = iHashVal + (Int4) iDomain;	
        fprintf(File, ">gi|%ld|gnl|vast|%ld|pdb|%s|%c Domain %ld - %s, %s\n",
                      (long) pmmdThis->iGi,
                      (long) iHashVal,
                      IsNullStr(pmsdThis->pcPDBName), (char) pmmdThis->pcMolName[0],	
                      (long) iDomain,
                      pmsdThis->pcChemName ? pmsdThis->pcChemName : "No name",
                      pmsdThis->pcPdbSource ? pmsdThis->pcPdbSource : "Unknown source");
        fflush(File);

        ValNodeAddInt(&pvnres, 0, 0);
        TraverseGraphs(pmmdThis->pdnmgHead,iDomain,
                                 LineLen,File,
                                 (pNodeFunc)(DoDomainSeq));
        ValNodeAddInt(&pvnres, 0, 0);
        /* the TraverseGraphs produces a global ValNode
        pvnres (pvnres has a value which indicates the
        residue number and a choice which is 0 for not
        a part of the domain and 1 for a part of the
        domain) pvnres has mock first and last elements
        (all values set to zero) which make its processing
        much easier */

        pvnintervals = MakeDomain(pmmdThis->iGi, File);
        if(pvnintervals == NULL)
         {
            ErrPostEx(SEV_INFO, 0, 0,"NULL domain valnode at %ld.", pmmdThis->iGi);
         }
        else
         {

             ResetStDOMDB(&pdd);
	     /* SK: NULL ptr check */
	     if ( pdd == NULL )
	        return;

             pdd->asn1 = pvnintervals;
             pdd->mmdbid = pmsdThis->iMMDBid;
             pdd->domid = iHashVal;
             pdd->gi = pmmdThis->iGi;
             pdd->domno = iDomain;
             pdd->domall = count;
             pdd->pdbid = pmsdThis->pcPDBName;
             pdd->chain = pmmdThis->pcMolName;

             AppendRecordDOMDB(pdd);
             rowCount++;

             /*
             d4appendStart(pd4Domdb, 0);
             d4blank(pd4Domdb);

             if (!AssignASNMemMemo((Pointer) pvnintervals, (AsnWriteFunc) SLRIValNodeAsnWrite, pf4ddAsn1))
              {
                ErrPostEx(SEV_FATAL,0,0, "Call to AssignASNMemMemo failed.");
              }
             f4assignLong(pf4ddMmdbId, (long) pmsdThis->iMMDBid);
             f4assignLong(pf4ddDomId, (long) iHashVal);
             f4assignLong(pf4ddGi, (long) pmmdThis->iGi);
             f4assignLong(pf4ddDomno, (long) iDomain);
             f4assignLong(pf4ddDomall, (long) count);
             f4assign(pf4ddPdbId, pmsdThis->pcPDBName);
             f4assign(pf4ddChain, pmmdThis->pcMolName);
             d4append(pd4Domdb);
             */
         }

        /* free valnodes */
        if (pvnintervals != NULL) pvnintervals = SLRIValNodeFree(pvnintervals);
        if (pvnres != NULL) pvnres = ValNodeFree(pvnres);
        fflush(File);


        pvnDom = pvnDom->next;
    }
   if (pvnDomHead) ValNodeFree(pvnDomHead);
   return;
}


void LIBCALL WriteFASTADomain(PDNMS pdnmsThis,  FILE *pFile)
{  /* writes out the sequences and heterogen names   */

   PMSD pmsdThis = NULL;
   PDNMM pdnmmThis = NULL;
   PMMD pmmdThis = NULL;

   /* SK: NULL ptr check */
   if ( pdnmsThis == NULL )
      return;

   pmsdThis = (PMSD) pdnmsThis->data.ptrvalue;
   /*printf("\n\n%s\n", IsNullStr(pmsdThis->pcPDBName));*/
   pdnmmThis = pmsdThis->pdnmmHead;    /* the top of the molecule list */
   while (pdnmmThis)                   /* walk the molecules individually */
     {
       pmmdThis = (PMMD) pdnmmThis->data.ptrvalue;
       if (pmmdThis != NULL && (int)pmmdThis->bWhat == AM_PROT)
         {
  	   chaincount ++;
  	   WriteFASTAByDomain(pdnmmThis, pFile, 70);
           fflush(pFile);
         }
        pdnmmThis = pdnmmThis->next;
     } /* while pdnmmThis */
   return ;
}

Nlm_Int2 Main ()
{
    BiostrucPtr bsp = NULL;
    PDNMS pdnmsThis = NULL;
    Int4 count = 0;
    /*Int4 rowCount = 0;   *//* to keep track of how many records should be committed */
    FILE  *sp;
    ValNodePtr pvnmmdbid = NULL, pvnhead = NULL;
    Boolean isUpdate = FALSE;


    ErrSetOptFlags(EO_SHOW_SEVERITY|EO_SHOW_CODES|EO_LOG_FILELINE|EO_SHOW_USERSTR|EO_SHOW_ERRTEXT|EO_BEEP|EO_WAIT_KEY|EO_LOGTO_USRFILE);
    ErrSetLogfile(LOGING, ELOG_BANNER);
    ErrSetMessageLevel(SEV_MAX);
    ErrSetLogLevel(SEV_INFO);


    if ( ! GetArgs("vastblst",NUMARGS, myargs))
       return 1;

    if (myargs[0].intvalue)
       isUpdate = TRUE;


    if (! OpenMMDBAPI(0, NULL) )
     {
        ErrPostEx(SEV_ERROR, 0, 0,"Have not opened mmdbapi.");
        return 2;	
     }
    sp = FileOpen(SPLINK, "w");
    if(sp == NULL)
     {
        ErrPostEx(SEV_ERROR, 0, 0,"Cannot open output file.");
        return 2;	
     }

    pdd = StDomdbNew();
    /* SK: NULL ptr check */
    if ( pdd == NULL )
    {
       ErrPostEx(SEV_ERROR,0,0, "pdd is NULL.");
       return 3;
    }
    InitCodeBase();
    if(!OpenDOMDB("DOMDB", TRUE))
     {
       ErrPostEx(SEV_ERROR,0,0, "OpenDOMDB failed.");
       return 3;
     }

    CloseDOMDB();
    CloseCodeBase();

    chaincount = 0;

    if (!SHoundInit(FALSE, "vastblst"))
     {
         ErrPostEx(SEV_ERROR, 0, 0,"Have not opened SHound.");
         return 3;
     }

    if (!StartTransaction()) {
       ErrPostEx(SEV_ERROR, 0, 0, "Cannot start transaction!");
       return 3;
    }

    /* deleting all records from table DOMDB */
    if (isUpdate) {
       if (!DeleteAllRecordsInDOMDB()) {
          ErrPostEx(SEV_ERROR, 0, 0, "Cannot delete all records from DOMDB!");
          if (!RollBackTransaction()) {
             ErrPostEx(SEV_ERROR, 0, 0, "Cannot roll back!");
             SHoundFini();
             CloseMMDBAPI();   	
             MemFree(pdd);
             return 3;
          }
          SHoundFini();
          CloseMMDBAPI();   	
          MemFree(pdd);
          return 3;
       }
    }

    pvnmmdbid = SHoundAllMMDBID();
    pvnhead = pvnmmdbid;
    while (pvnmmdbid != NULL)
     {
       if (SHound3DbWhat(pvnmmdbid->data.intvalue) & (Byte) AM_PROT)
        {
              bsp = SHoundGet3D(pvnmmdbid->data.intvalue);
              pdnmsThis = MakeAModelstruc(bsp);
              WriteFASTADomain(pdnmsThis, sp);
              ClearStructures();
              count++;
              /*rowCount++;*/
        }

       if (!isUpdate && rowCount >= 500) {
              rowCount = 0;
              /* commit */
              if (!CommitTransaction()) {
                 ErrPostEx(SEV_ERROR, 0, 0, "Cannot commit transaction!");
                 if (!RollBackTransaction()) {
                    ErrPostEx(SEV_ERROR,0,0, "Cannot roll back!");
                    SHoundFini();
                    CloseMMDBAPI();
                    MemFree(pdd);
                    return 1;
                 }
                 SHoundFini();
                 CloseMMDBAPI();
                 MemFree(pdd);
                 return 1;
              }
       }

       pvnmmdbid = pvnmmdbid->next;
     }

    /* commit for the last time:
     * all changes if this is an update,
     * otherwize the last bit of insertion */
    if (!CommitTransaction()) {
              ErrPostEx(SEV_ERROR, 0, 0, "Cannot commit transaction!");
              if (!RollBackTransaction()) {
                 ErrPostEx(SEV_ERROR,0,0, "Cannot roll back!");
                 MemFree(pdd);
                 SHoundFini();
                 CloseMMDBAPI();
                 return 1;
              }
              MemFree(pdd);
              SHoundFini();
              CloseMMDBAPI();
              return 1;
     }

    ValNodeFree(pvnhead);
    SHoundFini();
    CloseMMDBAPI();   	
    FileClose(sp);
    MemFree(pdd);
    /*printf("structures %ld\n", (long) count);
    printf("chains %ld\n", (long) chaincount);*/
    ErrPostEx(SEV_INFO,0,0, "Main: Done!");
    return 0;
}

/*
$Log: vastblst.c,v $
Revision 1.5  2004/12/14 16:48:51  zwang
Added ability of building a transactional database.
Added input parameter to specify if it's an update.
Removed unused patameters -n.
See bug 3441.

Revision 1.4  2004/08/23 16:15:07  zwang
Fixed errorneous NULL pointer checking. See bug #3070.

Revision 1.3  2004/06/07 22:28:29  hlieu
minor unused varaible;, bug 2185

Revision 1.2  2004/01/06 22:32:11  skuznets
Code audit has been done sucessfully.
Possible NULL pointer issues has been eliminated.

Revision 1.1.1.3  2002/07/09 08:28:28  root
Automated import from Sourceforge.net

Revision 1.3  2002/07/08 18:05:14  kaca
removed unused variables

Revision 1.2  2002/06/06 18:09:22  kaca
removed Intrez calls

Revision 1.1.1.1  2001/05/25 21:36:19  kaca
initial import

Revision 1.7  2001/03/26 19:47:41  kaca
changes in include files

Revision 1.6  2001/02/23 20:40:06  kaca
license change

Revision 1.5  2001/02/09 21:42:35  kaca
abstracted database calls

Revision 1.4  2000/10/13 21:58:51  kaca
minor changes

Revision 1.3  2000/10/13 21:54:22  kaca
changed includes

Revision 1.2  2000/09/20 18:49:58  kaca
split SeqHound headers into private and public one and changed includes

Revision 1.1.1.1  2000/09/18 17:41:56  kaca
Initial import

Revision 1.1.1.1  2000/07/06 20:02:32  kaca
Initial import

*/
