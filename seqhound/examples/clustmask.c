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
* $Log: clustmask.c,v $
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
* Revision 1.5  2001/04/06 22:56:21  kaca
* changed input param and added new funct
*
* Revision 1.4  2001/03/28 18:16:42  kaca
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


#define NUMARGS 2
Args myargs[NUMARGS] = {
  {"PDB code",NULL,NULL,NULL,FALSE,'p',ARG_STRING,0.0,0,NULL},
  {"chain","none",NULL,NULL,TRUE,'c',ARG_STRING,0.0,0,NULL},
  };


Nlm_Int2 Main ()
{

   FILE *pOut;
   Char path[PATH_MAX];
   PCSAN pcsanMask = NULL;
   Int4 iMMDBid = 0;
   Int4 gival = 0;

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

   if(strcmp(myargs[1].strvalue, "none") == 0)
   {
       gival = SHoundGiFromPDBchain(myargs[0].strvalue, NULL);
       myargs[1].strvalue = NULL;
   }
   else
       gival = SHoundGiFromPDBchain(myargs[0].strvalue, myargs[1].strvalue);

   if(!gival)
     {
       ErrPostEx(SEV_ERROR, 0, 0,"No sequence identifier found for %s%s.\n", IsNullStr(myargs[0].strvalue), IsNullStr(myargs[1].strvalue));
       goto getout;
     }
   iMMDBid = SHound3DFromGi(gival);
   if (!iMMDBid)
     {
       ErrPostEx(SEV_ERROR, 0, 0,"No mmdb identifier found for %ld.\n", (long) gival);
       goto getout;
     }
   /* make the mask clustal data structure */
   pcsanMask = CSANStructMask(iMMDBid, gival);
   /* write out clustal file xxxxx.aln */
   sprintf(path,"%ld.aln",(long) gival);
   if ((pOut = FileOpen(path, "w")) == NULL)
     {
       ErrPostEx(SEV_ERROR, 0, 0,"Cannot open output file %s.\n", IsNullStr(path));
       goto getout;
     }
   WriteCSAN(pcsanMask, pOut);
   fflush(pOut);
   FileClose(pOut);
   FreeCSAN(pcsanMask);

   SHoundFini();
   CloseMMDBAPI();
   ErrPostEx(SEV_INFO,0,0, "Done!");
   return 0;

   getout:
        {
          printf("Aborting, check the log file %s for details.\n", LOGING);
          if(pOut != NULL) FileClose(pOut);
          CloseMMDBAPI(); 
          if(SHoundIsInited()) SHoundFini();
          return 1;

        }

}
/*
$Log: clustmask.c,v $
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

Revision 1.5  2001/04/06 22:56:21  kaca
changed input param and added new funct

Revision 1.4  2001/03/28 18:16:42  kaca
changed clustmask to reflect the paper example

Revision 1.3  2001/03/26 19:59:59  kaca
changes in include files

Revision 1.2  2001/02/26 15:40:44  kaca
all Intrez calls changed to SHound calls

Revision 1.1.1.1  2001/02/23 17:54:40  kaca
moved out of Source Forge  part of SeqHound

*/

