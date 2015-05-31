/* $Id: sendb.c,v 1.2 2004/01/06 22:32:14 skuznets Exp $
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
  July 1999
  stores entries of type Seq-entry in CodeBase databases SENDB and SENGI

******************************************************************************************/
#include <intrez_db.h>
#include <intrez_misc.h>
#include <seqhound.h>

#define LOGING "sendblog"

#define NUMARGS 4
Args myargs[NUMARGS] = {
	    {"Filename for asn.1 input",NULL,NULL,NULL,FALSE,'i',ARG_FILE_IN,0.0,0,NULL},
        {"Initialize Seq-Entry Database File","F", NULL, NULL, TRUE, 'n', ARG_BOOLEAN, 0.0, 0, NULL},
        {"Initialize GI/Seq-Entry Database File","F", NULL, NULL, TRUE, 'm', ARG_BOOLEAN, 0.0, 0, NULL},
        {"Is this an update","F", NULL, NULL, TRUE, 'u', ARG_BOOLEAN, 0.0, 0, NULL},
        };


/* globals */
StSengiPtr psg;
StSendbPtr psd;

/* functions */
Boolean ToBioseq(SeqEntryPtr PNTR pse, Int4 callno, Int4 ibi, CharPtr division);
BioseqPtr MakeDummyBioseq(Int4 gi);

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
Boolean ToBioseq(SeqEntryPtr PNTR ppse, Int4 callno, Int4 ibi, CharPtr division)
{

   BioseqPtr pbs = NULL, pbsempty = NULL;
   BioseqSetPtr pbss = NULL;
   SeqEntryPtr pseNew = NULL;
   Int4 gival = 0;
   SeqEntryPtr pse = NULL;

   /* SK: NULL ptr check */
   if ( psse == NULL || *psse == NULL )
   {
      ErrPostEx(SEV_ERROR,0,0,"ToBioseq: Null pointer to Bioseq Entry.");
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

   /* append a record to SENGI database */
   ResetStSENGI(&psg);
   psg->gi = gival;
   psg->seid =  ibi;
   psg->division = division;
   AppendRecordSENGI(psg);


   /* make dummy bioseq and replace the current one in the structure saves space*/
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
             if(!ToBioseq(&pseNew,++callno, ibi, division))
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



Int2 Main()
{
   AsnIoPtr aipin = NULL;
   AsnTypePtr atp = NULL;
   AsnTypePtr atp2 = NULL;
   AsnModulePtr amp = NULL;
   SeqEntryPtr sep = NULL;
   Int4 ibi = 0;
   Char filenam[PATH_MAX];
   Char division[PATH_MAX];
   CharPtr pfilenam = NULL;
   Int2 i = 0;
   time_t tim1,tim2;

   tim1=GetSecs();
   /* check command line arguments */
   if ( ! GetArgs("sendb",NUMARGS, myargs))
          return 1;


   /* load the sequence alphabets  */
   /* (and sequence parse trees)   */
   if (! SeqEntryLoad())
         ErrShow();

   /* set up a log file name */
   pfilenam = FileNameFind(myargs[0].strvalue);
   for(i = 0; i<= strlen(pfilenam); i++)
     {
        if(pfilenam[i] == '.')
            break;
        division[i] = pfilenam[i];
     }
   division[i] = 's';
   division[i+1] = '\0';
   sprintf(filenam,"%srun", division);


   ErrSetOptFlags(EO_LOG_SEVERITY | EO_LOG_CODES | EO_LOG_FILELINE | EO_LOG_USRFILE);
   ErrSetLogfile(filenam, ELOG_BANNER);
   ErrSetLogLevel(SEV_INFO);
   ErrSetMessageLevel(SEV_MAX);


   /* set up the division name (division + s) */
   pfilenam = myargs[0].strvalue;
   for(i = 0; i<= strlen(pfilenam); i++)
     {
        if(pfilenam[i] == '.')
            break;
        division[i] = pfilenam[i];
     }
   division[i] = 's';
   division[i+1] = '\0';

   psd = StSendbNew();
   psg = StSengiNew();


   /* Initialize CodeBase and open the database */
   InitCodeBase();

   /* open databases */
   if(myargs[3].intvalue)
   {
     sprintf(division, "gbupds");
     if(!OpenSENDB(division, (Boolean) myargs[1].intvalue))
      return 1;
   }
   else
   {
     if(!OpenSENDB(division, (Boolean) myargs[1].intvalue))
        return 1;
   }
   if(!OpenSENGI("SENGI", (Boolean) myargs[2].intvalue))
      return 1;
   
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

             /* figure out how to assign Seq-Entry identifiers - ibi, this is not perfect the BI numbers are not continuous, some SeqEntries have an Id block */
             ibi = SHoundRecNoSENGI();
             if(ibi == -1) ibi = 1;
             else ibi = ibi + 1;

             if (!ToBioseq(&sep, 1, ibi, division))
              {
                 ErrPostEx(SEV_ERROR,0,0,"Main: Call to ToBioseqAs failed.");
                 return 0;
              }

             /* append the SeqEntry to the SENDB database */
             ResetStSENDB(&psd);
             psd->seid = ibi;
             psd->asn1 = sep;
             AppendRecordSENDB(psd);

             SeqEntryFree(sep);
           }
          else
            /* don't need to read the data */
            AsnReadVal(aipin, atp, NULL);
        }
    

   AsnIoClose(aipin);


   CloseSENDB();
   CloseSENGI();
   CloseCodeBase();

   MemFree(psd);
   MemFree(psg);

   tim2=GetSecs();
   ErrPostEx(SEV_WARNING,11,1,"Total elapsed time: %ld sec",(tim2-tim1));
   ErrPostEx(SEV_INFO,0,0, "Main: Done!");


   return 0;
   
}

/*
$Log: sendb.c,v $
Revision 1.2  2004/01/06 22:32:14  skuznets
Code audit has been done sucessfully.
Possible NULL pointer issues has been eliminated.

Revision 1.1.1.2  2001/10/19 08:21:53  root
Automated import from Sourceforge.net

Revision 1.2  2001/10/18 15:34:01  kaca
added FileNameFind from the input arg

Revision 1.1.1.1  2001/05/25 21:36:21  kaca
initial import

Revision 1.12  2001/05/02 17:21:37  kaca
update procedure fix

Revision 1.11  2001/04/03 19:12:35  kaca
warning fixes

Revision 1.10  2001/03/26 19:48:05  kaca
changes in include files

Revision 1.9  2001/03/19 21:36:32  kaca
added new command line argument which directs the update files to update database

Revision 1.8  2001/02/23 20:41:16  kaca
license change

Revision 1.7  2001/01/27 19:51:10  kaca
slim SeqEntries and abstracted database calls

Revision 1.6  2001/01/26 19:42:47  kaca
slim Seq-Entries implemented

Revision 1.1  2000/10/23 18:47:34  kaca
initial import stores SeqEntries without complete Bioseqs - saves space

Revision 1.5  2000/10/13 21:52:18  kaca
changed includes

Revision 1.4  2000/09/20 18:54:36  kaca
added newline at EOF

Revision 1.3  2000/09/20 18:49:59  kaca
split SeqHound headers into private and public one and changed includes

Revision 1.2  2000/09/19 21:45:45  kaca
minor change

Revision 1.1.1.1  2000/09/18 17:41:56  kaca
Initial import

*/




