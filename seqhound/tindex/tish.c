/* $Id: tish.c,v 1.2 2004/01/06 22:32:18 skuznets Exp $
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

******************************************************************************************/
#include <slri_cb.h>
#include <netdb.h>
#include <slri_tim.h>
#include <intrez_misc.h>
#include <slri_misc.h>
#include <slri_ti_index.h>
#include <slri_te.h>



#define LOGING "tishlog"
#define ALLMEM 500000

#define NUMARGS 7
Args myargs[NUMARGS] = {
	     {"Filename for asn.1 input",NULL,NULL,NULL,FALSE,'i',ARG_FILE_IN,0.0,0,NULL},
      {"Name of output database","TI",NULL,NULL,TRUE,'d',ARG_STRING,0.0,0,NULL},
      {"Word instance limit per table","10000000",NULL,NULL,TRUE,'n',ARG_INT,0.0,0,NULL},
      {"Gunzip the input file","F", NULL, NULL, TRUE, 'z', ARG_BOOLEAN, 0.0, 0, NULL},
      {"Delete intermediate tables","F", NULL, NULL, TRUE, 'r', ARG_BOOLEAN, 0.0, 0, NULL},
      {"Make temporary file for cluster implementation","F", NULL, NULL, TRUE, 't', ARG_BOOLEAN, 0.0, 0, NULL},
      {"Path to temporary file","./",NULL,NULL,TRUE,'p',ARG_STRING,0.0,0,NULL},
      };

/* functions */
Boolean ToBioseq(SeqEntryPtr PNTR pse);
BioseqPtr MakeDummyBioseq(Int4 gi);
Boolean BioseqTextIndex(BioseqPtr pbs, Int4 gi);
Boolean SeqEntryTextIndex(SeqEntryPtr pse);
CharPtr CreateTIDatabPath(CharPtr basename, Int4 tablecount);

/* globals */
ValNodePtr pvntigi = NULL;
Int4 g_tablecount = 1;
CharPtr g_basename = NULL;
Int4 g_wordsdone = 0;
ValNodePtr g_pvnnames = NULL;
Int4 g_wordlim = 0;


Boolean BioseqTextIndex(BioseqPtr pbs, Int4 gi)
{
   AsnModulePtr amp = NULL;
   AsnTypePtr atp = NULL;
   TextobjSetPtr ptos = NULL;


   /* load parse tree */
   if(!BioseqLoad())
   {
     ErrPostEx(SEV_ERROR, 0, 0, "SeqEntryLoad failed.");
     return FALSE;
   }

   /* get pointer to the modules in the memory */
   amp = AsnAllModPtr();
   if (amp == NULL)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "AsnAllModPtr failed.");
     return FALSE;
   }

   /* get pointer to the begining of the module in question */
   atp = AsnFind("Bioseq");
   if (atp == NULL)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "AsnFind failed.");
     return FALSE;
   }

   ptos = TI_TextExtractMem(atp, (Pointer) pbs, (AsnWriteFunc) BioseqAsnWrite);
   if(ptos == NULL)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "TE_TextExtractMem failed on %ld.", (long) gi);
     return FALSE;
   }
   /* index text from Bioseq */
   if(TI_IndexFieldTextSet(ptos, gi,1) != 0)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "TI_IndexFieldTextSet failed on %ld.", (long) gi);
     return FALSE;
   }
   ptos = TextobjSetFree(ptos);

   return TRUE;
}



Boolean SeqEntryTextIndex(SeqEntryPtr pse)
{
   AsnModulePtr amp = NULL;
   AsnTypePtr atp = NULL;
   TextobjSetPtr ptos = NULL;

   /* load parse tree */
   if(!SeqEntryLoad())
   {
     ErrPostEx(SEV_ERROR, 0, 0, "SeqEntryLoad failed.");
     return FALSE;
   }

   /* get pointer to the modules in the memory */
   amp = AsnAllModPtr();
   if (amp == NULL)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "AsnAllModPtr failed.");
     return FALSE;
   }

   /* get pointer to the begining of the module in question */
   atp = AsnFind("Seq-entry");
   if (atp == NULL)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "AsnFind failed.");
     return FALSE;
   }
   ptos = TI_TextExtractMem(atp, (Pointer) pse, (AsnWriteFunc) SeqEntryAsnWrite);
   if(ptos == NULL)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "TE_TextExtractMem failed.");
     return FALSE;
   }
   if(TI_IndexFieldTextSetList(ptos, pvntigi, 2) != 0)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "TI_IndexFieldTextSetList failed.");
      ptos = TextobjSetFree(ptos);
      return FALSE;
   }
   ptos = TextobjSetFree(ptos);
   return TRUE;
}



/* makes a Bioseq which contains only a GI */
BioseqPtr MakeDummyBioseq(Int4 gi)
{
   BioseqPtr pbs = NULL;
   SeqIdPtr psi = NULL;


   if(gi <= 0)
   {
      ErrPostEx(SEV_ERROR,0,0,"MakeDummyBioseq: Invalid parameter.");
      return NULL;
   }

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


Boolean RemoveAccessionAndName(BioseqPtr PNTR ppbs)
{
   BioseqPtr pbs = NULL;
   SeqIdPtr psi = NULL;
   SeqIdPtr psiThis = NULL;
   TextSeqIdPtr ptsi = NULL;
   PDBSeqIdPtr ppsi = NULL;
   DbtagPtr pdt = NULL;

   /* SK: NULL ptr check */
   if (ppbs == NULL || *ppbs == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0,"RemoveAccessionAndName: Invalid parameter. ");
      return FALSE;
   }

   pbs = *ppbs;
   psi = pbs->id;
   if (psi == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, "RemoveAccessionAndName: NULL Seq-id pointer.");
      return FALSE;
   }
   psiThis = psi;
   while (psiThis != NULL)
   {
      if (psiThis->choice == SEQID_GENBANK)  /* GenBank */
       {
         ptsi = psiThis->data.ptrvalue;
         if (ptsi == NULL)
          {
            ErrPostEx(SEV_ERROR,0,0, "RemoveAccessionAndName: NULL TextSeq-id pointer.");
            return FALSE;
          }
         if (ptsi->name != NULL)
             ptsi->name = MemFree(ptsi->name);
         if (ptsi->accession != NULL)
             ptsi->accession = MemFree(ptsi->accession);
       }
      if (psiThis->choice == SEQID_EMBL)  /* EMBL */
       {
         ptsi = psiThis->data.ptrvalue;
         if (ptsi == NULL)
          {
            ErrPostEx(SEV_ERROR,0,0, "RemoveAccessionAndName: NULL TextSeq-id pointer.");
            return FALSE;
          }
         if (ptsi->name != NULL)
             ptsi->name = MemFree(ptsi->name);
         if (ptsi->accession != NULL)
             ptsi->accession = MemFree(ptsi->accession);
       }
      if (psiThis->choice == SEQID_PIR)  /* PIR */
       {
         ptsi = psiThis->data.ptrvalue;
         if (ptsi == NULL)
          {
            ErrPostEx(SEV_ERROR,0,0, "RemoveAccessionAndName: NULL TextSeq-id pointer.");
            return FALSE;
          }
         if (ptsi->name != NULL)
             ptsi->name = MemFree(ptsi->name);
         if (ptsi->accession != NULL)
             ptsi->accession = MemFree(ptsi->accession);
       }
      if (psiThis->choice == SEQID_SWISSPROT)  /* SWISS-PROT */
       {
          ptsi = psiThis->data.ptrvalue;
          if (ptsi == NULL)
          {
            ErrPostEx(SEV_ERROR,0,0, "RemoveAccessionAndName: NULL TextSeq-id pointer.");
            return FALSE;
          }
         if (ptsi->name != NULL)
             ptsi->name = MemFree(ptsi->name);
         if (ptsi->accession != NULL)
             ptsi->accession = MemFree(ptsi->accession);
       }
      if (psiThis->choice == SEQID_OTHER)  /* other */
       {
          ptsi = psiThis->data.ptrvalue;
          if (ptsi == NULL)
          {
            ErrPostEx(SEV_ERROR,0,0, "RemoveAccessionAndName: NULL TextSeq-id pointer.");
            return FALSE;
          }
         if (ptsi->name != NULL)
             ptsi->name = MemFree(ptsi->name);
         if (ptsi->accession != NULL)
             ptsi->accession = MemFree(ptsi->accession);
       }
      if (psiThis->choice == SEQID_DDBJ)  /* DDBJ */
       {
          ptsi = psiThis->data.ptrvalue;
          if (ptsi == NULL)
          {
            ErrPostEx(SEV_ERROR,0,0, "RemoveAccessionAndName: NULL TextSeq-id pointer.");
            return FALSE;
          }
         if (ptsi->name != NULL)
             ptsi->name = MemFree(ptsi->name);
         if (ptsi->accession != NULL)
             ptsi->accession = MemFree(ptsi->accession);
       }
      if (psiThis->choice == SEQID_PRF)  /* PRF */
       {
          ptsi = psiThis->data.ptrvalue;
          if (ptsi == NULL)
          {
            ErrPostEx(SEV_ERROR,0,0, "RemoveAccessionAndName: NULL TextSeq-id pointer.");
            return FALSE;
          }
         if (ptsi->name != NULL)
             ptsi->name = MemFree(ptsi->name);
         if (ptsi->accession != NULL)
             ptsi->accession = MemFree(ptsi->accession);
       }
      if (psiThis->choice == SEQID_GENERAL)  /* General dbtag for other databases */
       {
          pdt = psiThis->data.ptrvalue;
          if (pdt == NULL)
          {
            ErrPostEx(SEV_ERROR,0,0, "RemoveAccessionAndName: NULL Dbtag pointer.");
            return FALSE;
          }
          if (pdt->tag->str != NULL)
            pdt->tag->str = MemFree(pdt->tag->str);
       }
      psiThis = psiThis->next;
   }
   return TRUE;

}


/* visits every Bioseq in given SeqEntry */
Boolean ToBioseq(SeqEntryPtr PNTR ppse)
{

   BioseqPtr pbs = NULL, pbsempty = NULL;
   BioseqSetPtr pbss = NULL;
   SeqEntryPtr pseNew = NULL;
   Int4 gival = 0;
   SeqEntryPtr pse = NULL;

   /* SK: NULL ptr check */
   if ( ppse == NULL || *ppse == NULL )
   {
       ErrPostEx(SEV_ERROR,0,0,"ToBioseq: NULL ppse.");
       return FALSE;
   }

   pse = *ppse;
   if (pse->choice == 1) /* It is a Bioseq */
    {
        pbs = (BioseqPtr) pse->data.ptrvalue;
        if (pbs == NULL)
         {
           ErrPostEx(SEV_ERROR,0,0,"ToBioseq: Null pointer to Bioseq.");
           return FALSE;
         }
        if ((gival = GetGI(pbs)) <= 0)
         {
           ErrPostEx(SEV_ERROR,0,0,"ToBioseq: Invalid GI.");
           return FALSE;
         }
        /* remove accessions and names - spare yourself millions of records in the text index */
        if(!RemoveAccessionAndName(&pbs))
        {
           ErrPostEx(SEV_INFO,0,0,"ToBioseq: RemoveAccessionAndName failed.");
        }

        /* index the bioseq*/
        if(!BioseqTextIndex(pbs, gival))
        {
           ErrPostEx(SEV_ERROR,0,0,"ToBioseq: BioseqTextIndex failed.");
           return FALSE;
        }
        if(pvntigi == NULL)
          pvntigi = Slri_ValNodeAddInt(&pvntigi, 0, gival);

        /* make dummy bioseq and replace the current one in the structure */
        pbsempty = MakeDummyBioseq(gival);
        pse->data.ptrvalue = pbsempty;
        /* free the original bioseq */
        if(pbs)BioseqFree(pbs);

    }
   else if (pse->choice == 2) /* it is nested Bioseq-set call ToBioseq recursively*/
    {

        pbss = (BioseqSetPtr) pse->data.ptrvalue;
        if (pbss == NULL)
        {
            ErrPostEx(SEV_ERROR,0,0,"ToBioseq: Null pointer to Bioseq-Set.");
            return FALSE;
        }

        pseNew = pbss->seq_set;
        while (pseNew != NULL)
        {
            if(!ToBioseq(&pseNew))
              {
               ErrPostEx(SEV_ERROR,0,0,"ToBioseq: The recursive call to ToBiseqAs failed");
               return FALSE;
              }
             pseNew = pseNew->next;
        }
    }
   else
    {
        ErrPostEx(SEV_ERROR,0,0,"ToBioseq: Invalid choice field.");
        return FALSE;
    }
   return TRUE;
}

/* makes temporary file containing hostname and final table */
/* the "pseudo-unique" file name consists of hostname and final table concatenated together */
static void MakeTempFile(CharPtr path, CharPtr ptable)
{
      FILE* pfile = NULL;
      char* phost = NULL;
      Char pfname[PATH_MAX];
      struct hostent *hostinfo=NULL;
      char* ip = NULL;
      /* SK: Modification for IsNullStr */
      char* nameptr = NULL;

      phost = MemNew((size_t)80*sizeof(Char));
      gethostname(phost,(size_t) 80);
      if(!phost)
         ErrPostEx(SEV_ERROR, 0, 0, "Unable to get hostname.");
      hostinfo = (struct hostent *) gethostbyname(phost);
      if(!hostinfo)
         ErrPostEx(SEV_ERROR, 0, 0, "Unable to get hostinfo.");
      ip = *(hostinfo->h_addr_list);

      /* SK: */
      snprintf(pfname, PATH_MAX, "%s/%s%s",IsNullStr(path), IsNullStr(phost), IsNullStr(ptable));
      pfile = FileOpen(pfname, "w");
      /* SK: Modification for IsNullStr */
      nameptr = inet_ntoa(*(struct in_addr*)ip);
      fprintf(pfile, "%s\t%s\n",IsNullPtr(nameptr), IsNullStr(ptable));
      FileClose(pfile);
      MemFree(phost);
      MemFree(ip);
}


static Boolean GunzipFile(void)
{
   CharPtr pcommand = NULL;
   /* SK: */
   size_t  pcommandLen = 0;

   printf("Decompressing.\n");
   /* SK: */
   pcommandLen = (size_t)(StringLen(myargs[0].strvalue)+20)*sizeof(Char);
   pcommand = MemNew(pcommandLen);
   /* SK: */
   snprintf(pcommand, pcommandLen, "gunzip %s.gz\n", IsNullStr(myargs[0].strvalue));
   if(system(pcommand) != 0)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "Failed to gunzip the source file; check your path and available disk space.");
      MemFree(pcommand);
      return FALSE;
   }
   pcommand = MemFree(pcommand);
   return TRUE;
}

static Boolean GzipFile(void)
{
   CharPtr pcommand = NULL;
   /* SK: */
   size_t pcommandLen = 0;

   printf("Compressing.\n");
   /* SK: */
   pcommandLen = (size_t)(StringLen(myargs[0].strvalue)+20)*sizeof(Char);
   pcommand = MemNew(pcommandLen);
   /* SK: */
   snprintf(pcommand, pcommandLen, "gzip %s\n", IsNullStr(myargs[0].strvalue));
   if(system(pcommand) != 0)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "Failed to gzip the source file; check your path and available disk space.");
      MemFree(pcommand);
      return FALSE;
   }
   pcommand = MemFree(pcommand);
   return TRUE;
}

static void DeleteIntermediateFiles(ValNodePtr pvntempo)
{
      CharPtr pcommand = NULL;
      /* SK: */
      size_t  pcommandLen = 0;

      printf("Deleting.\n");
      while(pvntempo != NULL)
      {
         /* SK: */
         pcomandLen = (size_t)(StringLen(pvntempo->data.ptrvalue)+20)*sizeof(Char);
         pcommand = MemNew(pcommandLen);
         snprintf(pcommand, pcommandLen, "rm -f %s*", IsNullStr(pvntempo->data.ptrvalue));
         system(pcommand);
         pcommand = MemFree(pcommand);
         pvntempo = pvntempo->next;
      }
}


Int2 Main()
{
   AsnIoPtr aipin = NULL;
   AsnTypePtr atp = NULL;
   AsnTypePtr atp2 = NULL;
   AsnModulePtr amp = NULL;
   SeqEntryPtr sep = NULL;
   time_t tim1=0,tim2=0,tim3=0,tim4=0,tim5=0,time_indexing=0,time_merging=0,time_total=0;
   Char filenam[PATH_MAX];
   CharPtr pfilenam = NULL;
   Int2 i = 0;
   Char division[PATH_MAX];
   ValNodePtr pvntempo = NULL;
   CharPtr dbnam = NULL;


   tim1=GetSecs();
   /* check command line arguments */
   if ( ! GetArgs("sendb",NUMARGS, myargs))
          return 1;


   /* load the parse trees */
   if (! SeqEntryLoad())
         ErrShow();

   /* set up a log file name */
   pfilenam = myargs[0].strvalue;
   for(i = 0; i<= strlen(pfilenam); i++)
     {
        if(pfilenam[i] == '.')
            break;
        division[i] = pfilenam[i];
     }
   division[i] = '\0';
   /* SK: */
   snprintf(filenam, PATH_MAX, "%stirun", IsNullStr(division));
   g_basename = division;
   g_wordlim = myargs[2].intvalue;

   ErrSetOptFlags(EO_LOG_SEVERITY | EO_LOG_CODES | EO_LOG_FILELINE | EO_LOG_USRFILE);
   ErrSetLogfile(filenam, ELOG_BANNER);
   ErrSetLogLevel(SEV_ERROR);
   ErrSetMessageLevel(SEV_MAX);

   /* gunzip the file if needed */
   if(myargs[3].intvalue)
     if(!GunzipFile())
     {
       ErrPostEx(SEV_ERROR, 0, 0, "Cannot decompress %s", IsNullStr(myargs[0].strvalue));
       return(1);
     }

   printf("Indexing.\n");
   dbnam = CreateTIDatabPath(g_basename, g_tablecount);
   if (!TI_OpenIndex(dbnam))
   {
     ErrPostEx(SEV_ERROR, 0, 0, "TI_OpenIndex failed.");
     return(1);
   }
   ValNodeCopyStr(&g_pvnnames, 0, dbnam);
   MemFree(dbnam);

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

   /* open the ASN.1 input file */
   if ((aipin = AsnIoOpen (myargs[0].strvalue, "rb")) == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "Can't open %s", IsNullStr(myargs[0].strvalue));
      ErrShow();
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
              ErrPostEx(SEV_ERROR,0,0,"Main: Seq-entry pointer is NULL.");
              return 0;
            }
           if (!ToBioseq(&sep))
            {
               ErrPostEx(SEV_ERROR,0,0,"Main: Call to ToBioseq failed.");
               return 0;
            }
           /* remember all GIs in previous SeqEntry and index it in a loop with SeqEntry index */
           if(!SeqEntryTextIndex(sep))
           {
                ErrPostEx(SEV_ERROR,0,0,"Main: Call to SeqEntryTextIndex failed.");
                return 0;
           }
           pvntigi = ValNodeFree(pvntigi);
           SeqEntryFree(sep);
        }
       else
         /* don't need to read the data */
         AsnReadVal(aipin, atp, NULL);
    }

   /* save the last text index table */
   printf("Writing TI last database.\n");
   TI_SaveIndexChanges();
   TI_CloseIndex();
   tim2 = GetSecs();
   time_indexing = tim2-tim1;
   AsnIoClose(aipin);
   if(pvntigi) pvntigi = ValNodeFree(pvntigi);


   pvntempo = g_pvnnames;
   printf("List of names:\n");
   while(pvntempo != NULL)
   {
     printf("%s\n", IsNullStr(pvntempo->data.ptrvalue));
     pvntempo = pvntempo->next;
   }

   /* merge if the process produced more than one table */
   tim3 = GetSecs();
   if(g_tablecount > 1)
   {
     printf("Merging.\n");
     dbnam = CreateTIDatabPath(myargs[1].strvalue, 0);
     if(TI_MergeAllDb(dbnam, g_pvnnames) != 0)
     {
         ErrPostEx(SEV_ERROR,0,0,"Main: Call to TI_MergeAllDb failed.");
         return 0;
     }
     MemFree(dbnam);
   }
   tim4 = GetSecs();
   time_merging = tim4 - tim3;

   tim5=GetSecs();
   time_total = tim5-tim1;

   /* delete all intermediate tables; leave the table if there is only one */
   if((myargs[4].intvalue) && (g_tablecount > 1))
      DeleteIntermediateFiles(g_pvnnames);

   /* gzip the file if needed */
   if(myargs[3].intvalue)
     if(!GzipFile())
     {
       ErrPostEx(SEV_ERROR, 0, 0, "Cannot compress %s.", IsNullStr(myargs[0].strvalue));
       return(1);
     }

   /* make a temp file if needed*/
   if(myargs[5].intvalue)
   {
       if (g_tablecount > 1)
          MakeTempFile( myargs[6].strvalue, myargs[1].strvalue);
       else
          MakeTempFile( myargs[6].strvalue, g_pvnnames->data.ptrvalue);
   }

   /* free the ValNode of table names */
   ValNodeFreeData(g_pvnnames);

   printf("Elapsed time for indexing %ld, merging %ld; total %ld min.\n",time_indexing/60, time_merging/60,time_total/60);
   ErrPostEx(SEV_ERROR,11,1,"Elapsed time for indexing %ld, merging %ld; total %ld min.\n", time_indexing/60, time_merging/60,time_total/60);
   ErrPostEx(SEV_ERROR,0,0, "Main: Done!");

   return 0;

}

/*
$Log: tish.c,v $
Revision 1.2  2004/01/06 22:32:18  skuznets
Code audit has been done sucessfully.
Possible NULL pointer issues has been eliminated.

Revision 1.1.1.8  2001/12/11 09:22:39  root
Automated import from Sourceforge.net

Revision 1.8  2001/12/10 23:09:55  kaca
changed implementation for SeqEntries indexing

Revision 1.7  2001/12/04 00:28:40  kaca
memory management moved to text index library

Revision 1.6  2001/11/22 22:34:38  kaca
added log file functionality

Revision 1.5  2001/11/22 02:41:23  kaca
temp file now stores IP address not host name - from Marc's code

Revision 1.4  2001/11/19 22:37:42  kaca
added memory management, temporary file for cluster implementation

Revision 1.2  2001/10/19 20:04:22  kaca
remove accessions and sequence names from bioseqs

Revision 1.1.1.1  2001/10/10 21:53:48  kaca
initial import

Revision 1.4  2001/10/10 16:09:50  kaca
indexing list of IDs at the same time with TI_IndexFieldTextSetList

Revision 1.3  2001/09/18 21:39:11  kaca
switched includes for WIN

Revision 1.2  2001/09/18 21:27:50  kaca
added merging step

*/




