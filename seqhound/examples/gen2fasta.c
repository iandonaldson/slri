/* $Id: gen2fasta.c,v 1.2 2004/01/06 22:32:12 skuznets Exp $
*******************************************************************************************
Mount Sinai Hospital (MSH)

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
           Katerina Michalickova and Christopher W.V. Hogue
           hogue@mshri.on.ca


  REVISION/CONTRIBUTION NOTES:

  June 1, 2000  Original MSH Revision.
                Hogue Lab - University of Toronto Biochemistry
                Department and the Samuel Lunenfeld Research
                Institute, Mount Sinai Hospital
                http://bioinfo.mshri.on.ca  hogue@mshri.on.ca
                ftp://ftp.mshri.on.ca/


  PUBLICATION TO CITE:


  DESCRIPTION
  prints out FASTA protein database for a complete genome;
  input value is a taxonomy ID for the genome

*****************************************************************************************/
#include<readseqs.h>
#include<seqhound.h>

#define LOGING "gen2fastalog"
#define NUMARGS 2
Args myargs[NUMARGS] = {
    {"Taxonomy ID",NULL,NULL,NULL,FALSE,'t',ARG_INT,0.0,0,NULL},
	{"Output file", NULL, NULL, NULL, FALSE, 'o', ARG_FILE_OUT, 0.0,0,NULL},
	};

#define LINELEN 80

Int2 Main()
{
    ValNodePtr pvngi = NULL, pvngihead = NULL, pvnfasta = NULL, pvnfastahead = NULL;
    FILE *pfile = NULL;
    SLRIFastaPtr pfasta = NULL;
    Int4 count = 0;
    Char formatcom[100];
    ReadDBFILEPtr rdbfp=NULL;

    ErrSetOptFlags(EO_LOG_SEVERITY | EO_LOG_CODES | EO_LOG_FILELINE | EO_LOG_USRFILE);
    ErrSetLogfile(LOGING, ELOG_BANNER);
    ErrSetLogLevel(SEV_INFO);
    ErrSetMessageLevel(SEV_MAX);

    if ( ! GetArgs("gen2fasta",NUMARGS, myargs))
           return 1;

    if(!SHoundInit(FALSE, "gen2fasta")) /* initialize the SeqHound system */
     {
      ErrPostEx(SEV_ERROR,2,1,"Unable to Initialize SeqHound.");
      return 1;
     }

    pfile = FileOpen(myargs[1].strvalue, "w");  /* open the output file */
    if (pfile == NULL)
     {
       ErrPostEx(SEV_ERROR,2,1,"Unable to open the file %s.", IsNullStr(myargs[1].strvalue));
       goto getout;
     }

    pvngi = SHoundProteinsFromTaxID(myargs[0].intvalue);  /* get proteins from the genome */
    if(pvngi == NULL)
     {
       ErrPostEx(SEV_ERROR,2,1,"SHoundProteinsFromTaxID returned NULL.");
       goto getout;
     }
    printf("Got proteins.\n");
    pvnfasta = SHoundGetFastaList(pvngi); /* get FASTA sequences */
    if(pvnfasta == NULL)
     {
       ErrPostEx(SEV_ERROR,2,1,"SHoundGetFastaList returned NULL.");
       goto getout;
     }

    PrintSLRIFastaList(pvnfasta, LINELEN, pfile); /* print out the database */

    /* load up into the readseq */
    printf("Printed %ld FASTA sequences.\n", (long) count);

    /* format the FASTA database with format db and print out statistics */
    sprintf(formatcom, "formatdb -t PDB -i %s -o T", IsNullStr(myargs[1].strvalue));
    system(formatcom);
    rdbfp = OpenProteinFastaDB(myargs[1].strvalue);
    if(!WriteDBStatsToStream(stdout, rdbfp))
     {
       ErrPostEx(SEV_ERROR,2,1,"WriteDBStatsToStream failed.");
       goto getout;
     }
    rdbfp=CloseProteinFastaDB(rdbfp);

    /* free everything */
    if(pfile != NULL)FileClose(pfile);
    if(pvngi != NULL)ValNodeFree(pvngi);
    if(pvnfastahead != NULL)FreeFastaList(pvnfastahead);
    SHoundFini();
    return 0;

    getout:
    {
       printf("Aborting, check the log file %s for details.\n", LOGING);
       if(pfile != NULL) FileClose(pfile);
       if(pvngi != NULL) ValNodeFree(pvngi);
       if(pvnfastahead != NULL) FreeFastaList(pvnfastahead);
       if(SHoundIsInited()) SHoundFini();
       return 1;
    }

}

/*
$Log: gen2fasta.c,v $
Revision 1.2  2004/01/06 22:32:12  skuznets
Code audit has been done sucessfully.
Possible NULL pointer issues has been eliminated.

Revision 1.1.1.1  2001/10/11 17:37:13  gbader
Automated import from Sourceforge.net

Revision 1.1.1.2  2001/10/10 21:53:46  kaca
initial import

Revision 1.3  2001/06/11 21:40:36  kaca
cosmetic fix

Revision 1.3  2001/03/26 19:59:59  kaca
changes in include files

Revision 1.2  2001/02/23 21:05:19  kaca
license change

Revision 1.1.1.1  2001/02/23 17:54:40  kaca
moved out of Source Forge  part of SeqHound

*/
