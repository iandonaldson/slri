/* $Id: fastadom.c,v 1.2 2004/01/06 22:32:11 skuznets Exp $
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
  January 2000
  makes Fasta database from the database of domains (DOMDB)
  -database is made with a specified tolerance (i.e. tolerance of 2
  will include two unresolved residues inside the domain; any pockets
  of unresolved residues longer than two will be denoted with "X" )
  -makes the different databases using non-redundant sets from NCBI nrtable



******************************************************************************************/

#include <intrez_db.h>
#include <intrez.h>

#define LOGING "fastadomlog"
#define MINILEN 15

#define NUMARGS 3
Args myargs[NUMARGS] = {
        {"tolerance",NULL,NULL,NULL,FALSE,'t',ARG_INT,0.0,0,NULL},
        {"Cut-off BLAST E value 1=10e-7 2=10e-40 3=10e-80 4=non-identical 0=all",0,NULL,NULL,FALSE,'e',ARG_INT,0.0,0,NULL},
        {"Output file for FASTA sequences",NULL,NULL,NULL,FALSE,'o',ARG_FILE_OUT,0.0,0,NULL}
        };

StDomdbPtr pdd;


Int2 Main()
{
   SLRIFastaPtr pdom = NULL;
   FILE *pOut = NULL;
   Int4 count = 0;
   Byte set = 0;
   Int4 totalrec = 0;
   Int4 i = 0;

   ErrSetOptFlags(EO_LOG_SEVERITY | EO_LOG_CODES | EO_LOG_FILELINE | EO_LOG_USRFILE);
   ErrSetLogfile(LOGING, ELOG_BANNER);
   ErrSetLogLevel(SEV_INFO);
   ErrSetMessageLevel(SEV_MAX);

   /* check command line arguments */
   if ( ! GetArgs("makedom",NUMARGS, myargs))
          return 1;

   if(myargs[1].intvalue == 1) set = REP_MIN_SEVEN;
   else if(myargs[1].intvalue == 2) set = REP_MIN_FORTY;
   else if(myargs[1].intvalue == 3) set = REP_MIN_EIGHTY;
   else if(myargs[1].intvalue == 4) set = REP_NONREDUND;
   else if(myargs[1].intvalue == 0) set = 0;


   if(!SHoundInit(FALSE, "makedom"))  /* initialize the SeqHound system */
    {
      ErrPostEx(SEV_ERROR,0,0, "SHoundInit failed.\n");
      return 1;
    }
   pOut = FileOpen(myargs[2].strvalue, "w"); /* open the output file */
   if(pOut == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "Failed to open %s.\n", IsNullStr(myargs[2].strvalue));
      goto getout;
    }


   totalrec = SHoundRecNoDOMDB(); /* get number of records in the domain database */
   if (totalrec == 0)
    {
      ErrPostEx(SEV_ERROR,0,0, "SHoundRecNoDOMDB returned 0.\n");
      goto getout;
    }

   pdd = NULL;

   for (i = 1; i <= totalrec; i++)  /* fetch each record */
   {
      pdd = SHoundGetRecDOMDB(i);
      if(pdd == NULL)
       {
         ErrPostEx(SEV_ERROR,0,0, "SHoundGetRecDOMDB returned NULL.\n");
         goto getout;
       }
      if ((set == 0)|| ((Byte) pdd->rep & set)) /* the record belong to a chosen non-redundant set */
       {
            pdom = SHoundMakeFastaDomain(pdd, myargs[0].intvalue, MINILEN);  /* get FASTA */
            if (pdom == NULL)
             {
               ErrPostEx(SEV_WARNING,0,0, "SHoundMakeFastaDomain failed on mmdbid %ld.", (long) pdd->mmdbid);
               printf("SHoundMakeFastaDomain failed on mmdbid %ld. Check the log file %s for details.\n", (long) pdd->mmdbid, LOGING);
               /* print out a warning but proceed to the end of the database,
                this error is usually caused by some GIs missing in the databases due to an update */
             }
            else
             {
               fprintf(pOut, "%s\n", IsNullStr(pdom->defline)); /* format and print the FASTA sequence into a file */
               if(!PrintSLRIFastaSequence(pdom->sequence, 80, pOut))
                {
                  ErrPostEx(SEV_ERROR,2,1,"PrintSLRIFastaSequence failed.");
                  goto getout;
                }
               count ++;
             }
            /*printf("%ld\n", (long) count);*/ /*to monitor progress*/
            if (pdom != NULL) pdom = SLRIFastaFree(pdom);
       }
      ResetStDOMDB(&pdd);
   }
   printf("Wrote %ld FASTA sequences.\n", (long) count);
   /* free and close everything */
   StDomdbFree(pdd);
   FileClose(pOut);
   SHoundFini();
   ErrPostEx(SEV_INFO,0,0, "Done!");
   return 0;

   getout:
    {
      printf("Aborting, check the log file %s for details.\n", LOGING);
      if(pOut != NULL) FileClose(pOut);
      if(pdd != NULL) StDomdbFree(pdd);
      if(SHoundIsInited()) SHoundFini();
      if (pdom != NULL) SLRIFastaFree(pdom);
      return 1;
    }
}

/*
$Log: fastadom.c,v $
Revision 1.2  2004/01/06 22:32:11  skuznets
Code audit has been done sucessfully.
Possible NULL pointer issues has been eliminated.

Revision 1.1.1.2  2002/01/16 09:23:29  root
Automated import from Sourceforge.net

Revision 1.3  2002/01/15 16:41:55  kaca
removed unused variable

Revision 1.2  2002/01/15 16:11:09  kaca
renamed PrintFastaSequence into PrintSLRIFastaSequence

Revision 1.1.1.1  2001/05/25 21:36:18  kaca
initial import

Revision 1.11  2001/04/27 21:56:54  kaca
added minimal domain length check

Revision 1.10  2001/03/26 19:47:41  kaca
changes in include files

Revision 1.9  2001/02/23 20:40:06  kaca
license change

Revision 1.8  2001/02/21 14:31:55  kaca
cosmetic change

Revision 1.6  2001/02/16 19:22:46  kaca
abstracted database calls

Revision 1.5  2000/11/06 22:19:35  kaca
added functionality around different E value BLAST sets of domains

Revision 1.4  2000/10/13 22:01:02  kaca
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
