/* $Id: comgen.c,v 1.7 2005/02/01 21:41:22 zwang Exp $
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
  December 1999
  gathers complete genomes records from NCBI using http calls, parses them out and
    1) assigns kloodge IDs in TAXGI database
    2) assigns sequence names in ACCDB database

******************************************************************************************/

#include <intrez_db.h>
#include <seqhound.h>
#include <seqport.h>
#include <intrez_misc.h>
#include <intrez.h>

/*
#include <connect/ncbi_connection.h>
#include <connect/ncbi_connutil.h>
#include <connect/ncbi_http_connector.h>


#define NCBISERVER  "www.ncbi.nlm.nih.gov"
*/

#define LOGING "comgenlog"

#define NUMARGS 3
Args myargs[NUMARGS] = {
   {"Local data processing - all files", "TRUE", NULL, NULL, TRUE, 'l', ARG_BOOLEAN, 0.0, 0, NULL},
   {"Process one file with accession",NULL, NULL, NULL, TRUE, 'a', ARG_STRING, 0.0, 0, NULL},
   {"Path to data files", "./", NULL,NULL,TRUE,'p', ARG_STRING, 0.0, 0, NULL},
	};


/* functions */
Boolean AssignKloodge(Int4 gival, Int4 kloodgeval);
Int2 SLRIGetGIs(Int4 kloodgeval, CharPtr filepath);
ValNodePtr GetCdregionAndRnaGIs(BioseqPtr pbs);

Int4 g_assign, g_found;


Boolean AssignKloodge(Int4 gival, Int4 kloodgeval)
{

    StTaxgiPtr ptg = NULL;

    ptg = StTaxgiNew();
    /* SK: NULL ptr check */
    if ( ptg == NULL )
    {
          ErrPostEx(SEV_INFO,0,0, "AssignKloodge:StTaxgiNew failed.\n");
          return FALSE;
    }

    ptg->gi = gival;
    ptg->kloodge = kloodgeval;

    /*KM 030204 changing logic of reporting failed assignments */
    if(EditRecordTAXGIByGI(ptg, FALSE))
         g_assign = g_assign + 1;

    StTaxgiFree(ptg);
    return TRUE;
}

ValNodePtr GetCdregionAndRnaGIs(BioseqPtr pbs) {

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
       ErrPostEx(SEV_ERROR,3,1,"GetCdregionAndRnaGIs: NULL pointer to sequence annotation.");
       return NULL;
    }
    while (psa != NULL)
    {
      if (psa->type == 1) /* ftable */
      {
        psf = (SeqFeatPtr) psa->data;
        if (psf == NULL)
        {
	   ErrPostEx(SEV_ERROR,3,1,"GetCdregionAndRnaGIs: NULL pointer to sequence feature table.");
	   return NULL;
        }
        while(psf != NULL)
        {
	   /* SEQFEAT_CDREGION holds proteins, SEQFEAT_RNA holds RNAs */
          if((psf->data.choice == SEQFEAT_CDREGION) || (psf->data.choice == SEQFEAT_RNA))
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
			     ErrPostEx(SEV_ERROR,0,0, "GetCdregionAndRnaGIs: Invalid GI found.");
			     return NULL;
			  }
			  else
			  {
			    ValNodeAddInt(&pvngi, 0, igi);
			    /* KM 030204 reporting number of extracted GIs */
			    g_found = g_found + 1;
			  }
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



/* parses out DNA and protein GIS from the Seq-Entry for a complete genome */
Int2 SLRIGetGIs(Int4 kloodgeval, CharPtr filepath) {

    AsnIoPtr aip = NULL;
    SeqEntryPtr pse = NULL, psenew = NULL, psehead = NULL;
    BioseqPtr pbs = NULL, pbsthis = NULL;
    ValNodePtr pvngi = NULL;
    ValNodePtr pvngihead = NULL;
    BioseqSetPtr pbss = NULL;
    Int4 gi = 0;
    CharPtr paccess = NULL;
    ValNodePtr pvngihm = NULL, pvngihmhead = NULL, pvntemp = NULL;
    CharPtr pfilenam = NULL;
    BioseqSetPtr pbsstop = NULL;
    Boolean bhumouse = FALSE;

    g_assign = 0;
    g_found = 0;

    if(kloodgeval == 0)
    {
       ErrPostEx(SEV_ERROR,3,1,"SLRIGetGIs: Invalid kloodge.");
       return 0;
    }

    /* SK: NULL ptr check */
    if(filepath == NULL)
    {
       ErrPostEx(SEV_ERROR,3,1,"SLRIGetGIs: NULL file path.");
       return 0;
    }

    pfilenam = FileNameFind(filepath);
    /* SK: NULL ptr check */
    if ( pfilenam == NULL )
    {
       ErrPostEx(SEV_ERROR,3,1,"SLRIGetGIs: NULL file name.");
       return 0;
    }

    aip = AsnIoOpen(filepath, "r");
    if (aip == NULL)
    {
        ErrPostEx(SEV_ERROR,3,1,"SLRIGetGIs: Unable to open Io.");
        return 0;
    }

    if(strstr(pfilenam, "_chr") != NULL)
    {
       bhumouse = TRUE;
       pbsstop = BioseqSetAsnRead(aip, NULL);
       if(pbsstop == NULL)
       {
           ErrPostEx(SEV_ERROR,3,1,"SLRIGetGIs: BioseqSetAsnRead failed.");
	   return 0;
       }
       pse = SeqEntryNew();
       /* SK: NULL ptr check */
       if ( pse == NULL )
       {
          ErrPostEx(SEV_ERROR,3,1,"SLRIGetGIs: SeqEntryNew failed.");
          return 0;
       }

       pse->data.ptrvalue = pbsstop;
       pse->choice = 2;
       pse->next = NULL;
    }
    else
       pse = SeqEntryAsnRead(aip, NULL);

    if (pse == NULL)
    {
       ErrPostEx(SEV_ERROR,3,1,"SLRIGetGIs: AsnRead failed.");
       return 0;
    }
    psehead = pse;
    while(pse != NULL)
     {
	 if(pse->choice == 1)     /* bioseq; have to get all DNA and protein from this one */
         {
               pbs = pse->data.ptrvalue;
               if(pbs == NULL)
                {
                   ErrPostEx(SEV_ERROR,3,1,"SLRIGetGIs: NULL pointer to bioseq.");
                   return 0;
                }
               /*KM 030204 NCBI changed raw data format
	        DNA GIs moved to feature table; reflecting changes
	       pvngi = GetDNA(pbs);
               pvngi2 = GetProt(pbs);
               ValNodeLink(&pvngi, pvngi2);*/
               pvngi = GetCdregionAndRnaGIs(pbs);
	       gi = GetGI(pbs);
	       if (gi > 0)
	       {
	         ValNodeAddInt(&pvngi, 0, gi);
	         g_found = g_found +1 ;
	       }
	       gi = 0;
	       /* end KM 030204*/

         }
         else if(pse->choice == 2)      /* bioseq-set; go through all bioseqs and fish out GIs or accessions */
         {
              pbss = pse->data.ptrvalue;
              if(pbss == NULL)
               {
                 ErrPostEx(SEV_ERROR,3,1,"SLRIGetGIs: NULL pointer to bioseq-set.");
                 return 0;
               }
              psenew = pbss->seq_set;
              if(psenew == NULL)
               {
                 ErrPostEx(SEV_ERROR,3,1,"SLRIGetGIs: NULL pointer to seq-entry valnode.");
                 return 0;
               }
              while(psenew != NULL)
               {
                 pbsthis = psenew->data.ptrvalue;
                 if ((gi = GetGI(pbsthis)) > 0)
		 {
                     ValNodeAddInt(&pvngi, 0, gi);
                     g_found = g_found + 1;
		 }
                 else
                 {
                     /*ErrPostEx(SEV_ERROR,3,1,"SLRIGetGIs: GetGI returned invalid GI.");*/
                     paccess = Misc_GetAccession(pbsthis);
                     if(paccess != NULL)
                     {
#ifdef ODBC
                        if((gi = SHoundFindGiByAcc(paccess)) > 0)
#else
                        if((gi = SHoundFindAcc(paccess)) > 0)
#endif
                        {
                           ValNodeAddInt(&pvngi, 0, gi);
			   g_found = g_found + 1;
                        }
                        else
                           ErrPostEx(SEV_ERROR,3,1,"SLRIGetGIs: SHoundFindAcc returned invalid GI.");
                        MemFree(paccess);
                     }
                     else
                       ErrPostEx(SEV_ERROR,3,1,"SLRIGetGIs: Misc_GetAccession returned NULL.");
                 }

		 /* in case of human and mouse chromosome hs_chr*.asn and mm_chr*.asn */
		 /* get out all RNA and proteins from each bioseq (the bioseqs hold large parts of chromosomes) */
                 if(bhumouse)
		 {
		    if((pvntemp = GetCdregionAndRnaGIs(pbsthis)) != NULL)
		       ValNodeLink(&pvngihm, pvntemp);
		 }
                 psenew = psenew->next;
               }
         }
         pse = pse->next;
     }

    SeqEntryFree(psehead);
    AsnIoClose(aip);


    /* store all chromosomal IDs */
    pvngihead = pvngi;
    while(pvngi != NULL)
     {
       AssignKloodge(pvngi->data.intvalue, kloodgeval);
       pvngi = pvngi->next;
     }

    pvngihmhead = pvngihm;

    while(pvngihm != NULL)
     {
       AssignKloodge(pvngihm->data.intvalue, kloodgeval);
       pvngihm = pvngihm->next;
     }
    if(pvngihead) ValNodeFree(pvngihead);
    if(pvngihmhead) ValNodeFree(pvngihmhead);

    /* KM 030204 reporting number of extracted GIs */
    if (g_found == g_assign)
    {
       ErrPostEx(SEV_INFO,3,1,"file %s  found %ld  assigned %ld  result OK\n",pfilenam, (long) g_found, (long) g_assign);
    }
    else
    {
       ErrPostEx(SEV_ERROR,3,1,"file %s  found %ld  assigned %ld  result DIFFERENCE\n",pfilenam, (long) g_found, (long) g_assign);
    }

    return 1;
}



Int2 Main()
{
    Char fpath[PATH_MAX];
    Char replypath[PATH_MAX];
    Char replypath0[PATH_MAX];
    Int4 count = 1;
    CharPtr paccess = NULL;
    ValNodePtr pvnaccess = NULL, pvnaccesshead = NULL;
    ValNodePtr pvnkloodge = NULL;

    ErrSetOptFlags(EO_LOG_SEVERITY | EO_LOG_CODES | EO_LOG_FILELINE | EO_LOG_USRFILE);
    ErrSetLogfile(LOGING, ELOG_BANNER);
    ErrSetLogLevel(SEV_INFO);
    ErrSetMessageLevel(SEV_MAX);


    if (!GetArgs("comgen",NUMARGS, myargs))
           return 1;

    InitCodeBase();
    if(!OpenCHROM("CHROM", FALSE))
       {
        ErrPostEx(SEV_ERROR,0,0, "Main: OpenCHROM failed.");
        return 1;
       }
    if(!OpenTAXGI("TAXGI", FALSE))
       {
        ErrPostEx(SEV_ERROR,0,0, "Main: OpenCHROM failed.");
        return 1;
       }
    if(!OpenACCDB("ACCDB", FALSE))
       {
        ErrPostEx(SEV_ERROR,0,0, "Main: OpenACCDB failed.");
        return 1;
       }

     /* starting transaction */
     if (!StartTransaction()) {
        ErrPostEx(SEV_ERROR, 0, 0, "Cannot start transaction!");
        CloseCodeBase();
        return 1;
     }  

    /* assemble a full path to data files; make sure that there is a slash at the end */
    StringCpy(replypath0, myargs[2].strvalue);
    if(replypath0[strlen(replypath0) -1] == DIRDELIMCHR)
       StringCpy(replypath, replypath0);
    else
    {
       StringCat(replypath0, "/");
       StringCpy(replypath, replypath0);
    }


    if (myargs[0].intvalue)  /* process all local files */
    {

	 pvnaccess = SHoundAllGenAccess();
	 pvnkloodge = SHoundAllKloodge();
         if((pvnaccess == NULL) || (pvnkloodge == NULL))
         {
            ErrPostEx(SEV_ERROR,0,0, "Main: SHoundAllGenAccess or SHoundAllKloodge failed.");
            return 1;
         }
         pvnaccesshead = pvnaccess;
         printf("%ld calls.\n", (long) ValNodeLen(pvnaccess));
         while(pvnaccess != NULL)
         {
             paccess = pvnaccess->data.ptrvalue;
             printf("%ld\n", (long) count);
             sprintf(fpath,"%s%s.asn", replypath, IsNullStr(paccess));
             ErrPostEx(SEV_INFO,3,1,"Main: %s started.", fpath);
             if(FileLength(fpath)<=0)
              {
                ErrPostEx(SEV_ERROR,3,1,"Main: Cannot find input file %s.", fpath);
              }
             else
              {
                 if(SLRIGetGIs(SHoundGetKloodgeFromAccess(paccess), fpath) == 0)
                  {
                     ErrPostEx(SEV_ERROR,3,1,"Main:SLRIGetGIs failed.");
                  }

              }
             count++;
             pvnaccess = pvnaccess->next;
         }
    }
    else if (myargs[1].strvalue != NULL)   /* process only one accession */
    {
        sprintf(fpath,"%s%s.asn", replypath, IsNullStr(myargs[1].strvalue));
        ErrPostEx(SEV_INFO,3,1,"Main: %s started.", fpath);
        if(FileLength(fpath)<=0)
        {
           ErrPostEx(SEV_ERROR,3,1,"Main: Cannot find input file %s.", fpath);
	   return 1;
        }
        else
        {
           if(SLRIGetGIs(SHoundGetKloodgeFromAccess(myargs[1].strvalue), fpath) == 0)
           {
              ErrPostEx(SEV_ERROR,3,1,"Main: SHoundGetKloodgeFromAccess failed.");
              return 1;
           }
        }
    }


    /* Committing all changes or rolling back */
    if (!CommitTransaction()) {
       ErrPostEx(SEV_ERROR, 0, 0, "Cannot commit transaction!");
       if (!RollBackTransaction()) {
          ErrPostEx(SEV_ERROR,0,0, "Cannot roll back!");
          CloseCodeBase();
          return 1;
       }
       CloseCodeBase();
       return 1;
    }

    if (!CloseCHROM())
           ErrPostEx(SEV_ERROR,0,0, "Main: CloseCHROM failed.");
    if (!CloseTAXGI())
           ErrPostEx(SEV_ERROR,0,0, "Main: CloseTAXGI failed.");
    if (!CloseACCDB())
           ErrPostEx(SEV_ERROR,0,0, "Main: CloseACCDB failed.");
    CloseCodeBase();
    ValNodeFreeData(pvnaccesshead);
    ErrPostEx(SEV_INFO,0,0, "Main: Done.");
    return 0;
}

/*
$Log: comgen.c,v $
Revision 1.7  2005/02/01 21:41:22  zwang
Added ability of building a transactional database.

Revision 1.6  2004/06/07 22:21:46  hlieu
removed unused variables, bug 2185

Revision 1.5  2004/05/31 20:25:11  hlieu
minor fixes to compiler warnings, bug 2185

Revision 1.4  2004/03/17 20:55:17  zwang
Use database layer function SHoundFindGiByAcc in order to speed up query in a loop for ODBC.

Revision 1.3  2004/02/16 17:22:51  idonalds
Major changes to support data format changes for Drosophila, C. elegans and Arabidopsis

Revision 1.2  2004/01/06 22:32:12  skuznets
Code audit has been done sucessfully.
Possible NULL pointer issues has been eliminated.

Revision 1.1.1.12  2003/01/28 09:32:45  root
Automated import from Sourceforge.net

Revision 1.12  2003/01/27 14:20:35  micheld
removed crosscomp warmings

Revision 1.11  2002/11/06 21:44:15  kaca
the parsing proceeds if one file is not found

Revision 1.10  2002/10/29 21:15:10  kaca
human and mouse chromosome have to be read as Bioseq-sets

Revision 1.8  2002/10/23 23:26:01  kaca
fixed one genome processing option

Revision 1.7  2002/10/17 20:02:13  kaca
rewrote main so it uses files downloaded by genftp.pl script; comgen does not use HTTP calls any more

Revision 1.6  2002/07/08 17:33:36  kaca
removed unused variables

Revision 1.5  2002/07/03 22:27:09  kaca
added path to data argument

Revision 1.4  2002/06/20 16:10:30  kaca
fixed misleading argument info

Revision 1.3  2002/03/25 07:44:31  micheld
include order change and minor fix for windows compile

Revision 1.2  2002/03/21 21:20:32  kaca
added single kloodge fetch

Revision 1.1.1.2  2001/10/10 21:53:46  kaca
initial import

Revision 1.15  2001/09/28 17:42:28  kaca
added accession number processing for genomes which do not have GIs

Revision 1.14  2001/04/18 15:50:35  kaca
more new toolkit connectivity changes

Revision 1.13  2001/04/16 20:43:58  kaca
reflect changes in toolkit connect stuff

Revision 1.12  2001/04/03 19:21:12  kaca
added include

Revision 1.11  2001/03/26 19:47:53  kaca
changes in include files

Revision 1.10  2001/02/23 20:40:38  kaca
license change

Revision 1.9  2001/02/08 19:35:50  kaca
cosmetic change

Revision 1.8  2001/02/07 21:43:29  kaca
removed unused variables

Revision 1.7  2001/02/07 16:48:26  kaca
cosmetic changes

Revision 1.6  2001/02/06 20:23:02  kaca
cosmetic changes

Revision 1.5  2001/02/06 20:13:19  kaca
abstracted database calls

Revision 1.4  2001/01/11 18:32:14  kaca
fixed return values

Revision 1.3  2000/10/13 21:54:22  kaca
changed includes

Revision 1.2  2000/09/20 18:49:59  kaca
split SeqHound headers into private and public one and changed includes

Revision 1.1.1.1  2000/09/18 17:41:56  kaca
Initial import

*/

