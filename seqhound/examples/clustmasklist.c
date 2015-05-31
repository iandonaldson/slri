/* clustmask.c
* ===========================================================================
*
*                         COPYRIGHT NOTICE
*                Samuel Lunenfeld Research Institute
*
*  This software/database is copyrighted. 
*
*  Although all reasonable efforts have been taken to ensure the accuracy
*  and reliability of the software and data, the Samuel Lunenfeld 
*  Research Institute does not and cannot warrant the performance or 
*  results that may be obtained by using this software or data. The 
*  The Samuel Lunenfeld Research Institutes disclaims all warranties, express 
*  or implied, including warranties of performance, merchantability or fitness 
*  for any particular purpose.
*
* ===========================================================================
*
* File Name:  slapamol.c
*
* Author:  Christopher Hogue
*
* Version Creation Date:   04/23/98
*
* $Revision: 1.2 $
*
* File Description: Clustal-style Masked Structure Alignments 
*
* Modifications:  
* --------------------------------------------------------------------------
* Date     Name        Description of modification
* -------  ----------  -----------------------------------------------------
*
*
*
* $Log: clustmasklist.c,v $
* Revision 1.2  2004/01/06 22:32:11  skuznets
* Code audit has been done sucessfully.
* Possible NULL pointer issues has been eliminated.
*
* Revision 1.1.1.2  2002/02/15 09:25:33  root
* Automated import from Sourceforge.net
*
* Revision 1.2  2002/02/15 06:42:23  micheld
* Modified makefile targets to include dependency building.
*
* Revision 1.1.1.1  2001/05/25 21:36:19  kaca
* initial import
*
* Revision 1.1.1.1  2001/05/25 20:43:00  kaca
* code examples for SeqHound
*
* Revision 1.2  2001/05/10 18:15:31  kaca
* moved printfastafromgi here from the lib
*
* Revision 1.1  2001/03/28 18:16:42  kaca
* changed clustmask to reflect the paper example
*
* Revision 1.3  2001/03/26 19:59:59  kaca
* changes in include files
*
* Revision 1.2  2001/02/26 15:40:44  kaca
* all Intrez calls changed to SHound calls
*
* Revision 1.1.1.1  2001/02/23 17:54:40  kaca
* moved out of Source Forge  part of SeqHound
*
* Revision 1.1.1.1  2000/06/09 18:14:05  feldman
* TraDES project
*
*
* ==========================================================================
*/

#include <csan.h>
#include <seqhound.h>

#define LOGING "clustmasklog"

#define BIGGEROF(bach1,bach2) (((bach1) > (bach2)) ? (bach1) : (bach2))


#define NUMARGS 1
Args myargs[NUMARGS] = {
        {"Input File with Sequence GI & Structure GI",NULL,NULL,NULL,FALSE,'i',ARG_FILE_IN,0.0,0,NULL}
  };


Boolean WriteFASTAfromGI(Int4 iGiSeq, FILE *pOut)
{

    SLRIFastaPtr pfa = NULL;

    pfa = SHoundGetFasta(iGiSeq);
    if(pfa == NULL)
     {
       ErrPostEx(SEV_ERROR, 0, 0,"WriteFASTAfromGI: SHoundGetFasta failed.");
       return FALSE;
     }
    if(!PrintSLRIFasta(pfa, 80, pOut))
     {
       ErrPostEx(SEV_ERROR, 0, 0,"WriteFASTAfromGI: PrintSLRIFasta failed.");
       return FALSE;
     }
    return TRUE;

}


Nlm_Int2 Main ()
{
    
   FILE *pFile;
   FILE *pOut;
   Char pcBuf[100];
   Char path[PATH_MAX];
   CharPtr pcTest;
   long gi1, gi2;
   Int4 iGi1, iGi2, iGiStr, iGiSeq;
   PCSAN pcsanMask = NULL;
   Int4 iMMDBid = 0;

   if (! GetArgs("clustmask",NUMARGS,myargs))
         return 0;



   ErrSetOptFlags(EO_SHOW_SEVERITY|EO_SHOW_CODES|EO_LOG_FILELINE|EO_SHOW_USERSTR|EO_SHOW_ERRTEXT|EO_BEEP|EO_WAIT_KEY|EO_LOGTO_USRFILE);
   ErrSetLogfile(LOGING, ELOG_BANNER);
   ErrSetMessageLevel(SEV_MAX);
   ErrSetLogLevel(SEV_INFO);


   if (!SHoundInit(FALSE, "clustmask")) /* initialize the SeqHound system */
      {
          ErrPostEx(SEV_ERROR, 0, 0,"Cannot open SHound.");
          return 3;
      }

   if (!OpenMMDBAPI(0, NULL))  /* initialize MMDB interface */
     {
        ErrPostEx(SEV_ERROR, 0, 0,"Unable to Initialize MMDBAPI\n");
        return 3;
     }

   StringCpy(path,myargs[0].strvalue);   /* open the input file */
   if ((pFile = FileOpen(path, "r")) == NULL)
      {
         ErrPostEx(SEV_ERROR, 0, 0,"No input file found: %s\n", IsNullStr(path));
         goto getout;
      }


   do {
        pcBuf[0] = '\0';
        path[0] = '\0';
        pcTest = fgets(pcBuf, (size_t)100,  pFile);
        gi1 = 0;
        gi2 = 0;
        iGi1 = 0;
        iGi2 = 0;
        iMMDBid = 0;
        if (pcTest)
          {
              sscanf(pcBuf, "%ld %ld", &gi1, &gi2);  /* figure out which GI has a structure */
              if ((gi1 != 0) && (gi2 != 0))
                {
                   iGi1 = (Int4) gi1;
                   iGi2 = (Int4) gi2;
                   iMMDBid = 0;

                   iMMDBid = SHound3DFromGi(iGi1);
                   if (!iMMDBid)
                     {
                       iMMDBid = SHound3DFromGi(iGi2);
                       iGiStr = iGi2;
                       iGiSeq = iGi1;
                     }
                   else
                     {
                       iGiStr = iGi1;
                       iGiSeq = iGi2;
                     }
                   if (!iMMDBid)
                     {
                       ErrPostEx(SEV_ERROR, 0, 0,"No structure GI found: %s\n", IsNullStr(path));
                       goto getout;
                     }
                   printf("Sequence [%ld] is the structure %ld \n",(long) iGiStr, (long) iMMDBid);

                   /* dump the sequence as a FASTA file named "xxxxx.faa" where xxxx is the Gi */
                   sprintf(path,"%ld.faa",(long) iGiSeq);
                   if ((pOut = FileOpen(path, "w")) == NULL)
                     {
                       ErrPostEx(SEV_ERROR, 0, 0,"Cannot open output file %s.\n", IsNullStr(path));
                       goto getout;
                     }
                   if(!WriteFASTAfromGI(iGiSeq, pOut))
                    {
                      ErrPostEx(SEV_ERROR, 0, 0,"WriteFASTAfromGI failed.");
                      goto getout;
                    }
                   fflush(pOut);
                   FileClose(pOut);
                   printf("Wrote FASTA for GI %ld\n", (long)iGiSeq);
                   /* make the mask clustal data structure */
                   pcsanMask = CSANStructMask(iMMDBid, iGiStr);
                   /* write out clustal file xxxxx.aln */
                   sprintf(path,"%ld.aln",(long) iGiStr);
                   if ((pOut = FileOpen(path, "w")) == NULL)
                     {
                       ErrPostEx(SEV_ERROR, 0, 0,"Cannot open output file %s.\n", IsNullStr(path));
                       goto getout;
                     }
                   WriteCSAN(pcsanMask, pOut);
                   fflush(pOut);
                   FileClose(pOut);
                   FreeCSAN(pcsanMask);
                }
          }
     } while (pcTest);
   SHoundFini();
   CloseMMDBAPI();
   ErrPostEx(SEV_INFO,0,0, "Done!");
   return 0;

   getout:
        {
          printf("Aborting, check the log file %s for details.\n", LOGING);
          if(pFile != NULL) FileClose(pFile);
          if(pOut != NULL) FileClose(pOut);
          CloseMMDBAPI();
          if(SHoundIsInited()) SHoundFini();
          return 1;

        }

}
/*
$Log: clustmasklist.c,v $
Revision 1.2  2004/01/06 22:32:11  skuznets
Code audit has been done sucessfully.
Possible NULL pointer issues has been eliminated.

Revision 1.1.1.2  2002/02/15 09:25:33  root
Automated import from Sourceforge.net

Revision 1.2  2002/02/15 06:42:23  micheld
Modified makefile targets to include dependency building.

Revision 1.1.1.1  2001/05/25 21:36:19  kaca
initial import

Revision 1.1.1.1  2001/05/25 20:43:00  kaca
code examples for SeqHound

Revision 1.2  2001/05/10 18:15:31  kaca
moved printfastafromgi here from the lib

Revision 1.1  2001/03/28 18:16:42  kaca
changed clustmask to reflect the paper example

Revision 1.3  2001/03/26 19:59:59  kaca
changes in include files

Revision 1.2  2001/02/26 15:40:44  kaca
all Intrez calls changed to SHound calls

Revision 1.1.1.1  2001/02/23 17:54:40  kaca
moved out of Source Forge  part of SeqHound

*/

