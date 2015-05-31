/* $Id: gen2struc.c,v 1.2 2004/01/06 22:32:12 skuznets Exp $
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
  fetches all structures for a genome and writes them out in PDB format

*****************************************************************************************/
#include<seqhound.h>

#define LOGING "gen2struclog"
#define NUMARGS 2
Args myargs[NUMARGS] = {
        {"Taxonomy ID",NULL,NULL,NULL,FALSE,'t',ARG_INT,0.0,0,NULL},
        {"Output file", NULL, NULL, NULL, FALSE, 'o', ARG_FILE_OUT, 0.0,0,NULL},
	};


Int2 Main()
{
    ValNodePtr pvngi = NULL, pvngihead = NULL;
    SeqEntryPtr pse = NULL;
    FILE *pfile = NULL;
    ValNodePtr pvngiall = NULL;
    BiostrucPtr pstruc = NULL;
    PDNMS pmodelstruc=NULL;
    Int2 result = 0;
    ValNodePtr pvnmmdb = NULL, pvnmmdbhead = NULL;
    Int4 count = 0;
    ValNodePtr pvngroup = NULL;


    ErrSetOptFlags(EO_LOG_SEVERITY | EO_LOG_CODES | EO_LOG_FILELINE | EO_LOG_USRFILE);
    ErrSetLogfile(LOGING, ELOG_BANNER);
    ErrSetLogLevel(SEV_INFO);
    ErrSetMessageLevel(SEV_MAX);

    if ( ! GetArgs("gen2struc",NUMARGS, myargs))
           return 1;

    if(!SHoundInit(FALSE, "gen2struc"))  /* initialize the SeqHound system */
     {
      ErrPostEx(SEV_ERROR,2,1,"Unable to Initialize SeqHound.");
      return 1;
     }

    if (!OpenMMDBAPI(0,NULL)) /* initialize MMDB interface */
     {
       ErrPostEx(SEV_ERROR,2,1,"Unable to open MMDBAPI.");
       goto getout;
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
       printf("Could not retrieve any proteins for the taxonomy ID %ld.\n", (long) myargs[0].intvalue);
       goto getout;
     }
    printf("Got proteins.\n");
    pvngroup =  SHoundRedundantGroupIDFromGIList(pvngi);  /* get IDs for all redundant groups */
    if(pvngroup == NULL)
     {
       ErrPostEx(SEV_ERROR,2,1,"SHoundRedundantGroupIDFromGIList returned NULL.");
       goto getout;
     }
    printf("Got groups.\n");
    pvngiall = SHoundRedundantGroupFromIDList(pvngroup); /* get all redundant proteins for the genome */
    if(pvngiall == NULL)
     {
       ErrPostEx(SEV_ERROR,2,1,"SHoundRedundantGroupFromIDList returned NULL.");
       goto getout;
     }
    printf("Got redundant proteins.\n");
    pvnmmdb = SHound3DFromGiList(pvngiall);  /* get all structures */
    if(pvnmmdb == NULL)
     {
       ErrPostEx(SEV_ERROR,2,1,"SHound3DFromGiList returned NULL.");
       printf("Could not retrieve any structures for the taxonomy ID %ld.\n", (long) myargs[0].intvalue);
       goto getout;
     }
    pvnmmdbhead = pvnmmdb;
    printf("Got structures.\n");
    while(pvnmmdb != NULL)     /* print out all structures in PDB format */
     {
       if(SHound3DExists(pvnmmdb->data.intvalue))
        {
            pstruc = SHoundGet3D(pvnmmdb->data.intvalue);
            if(pstruc == NULL)
             {
               ErrPostEx(SEV_ERROR,2,1,"SHoundGet3D failed.");
               goto getout;
             }
            pmodelstruc=MakeAModelstruc(pstruc);
            if(pmodelstruc == NULL)
             {
               ErrPostEx(SEV_ERROR,2,1,"MakeAModelstruc failed.");
               goto getout;
             }

            result = WritePDBAllModel(pmodelstruc, pfile);
            if(result != 1)
             {
               ErrPostEx(SEV_ERROR,2,1,"WritePDBAllModel failed.");
               ClearStructures();
               goto getout;
             }
            ClearStructures();
            count++;
        }
       pvnmmdb = pvnmmdb->next;
     }

    printf("Wrote %ld structures.\n", (long) count);


    /* free everything */
    if(pvngi != NULL) ValNodeFree(pvngi);
    if(pvngiall != NULL) ValNodeFree(pvngiall);
    if(pvngroup != NULL) ValNodeFree(pvngroup);
    if(pvnmmdbhead != NULL) ValNodeFree(pvnmmdbhead);
    /* close MMDBAPI and SeqHound */
    if(pfile != NULL) FileClose(pfile);
    CloseMMDBAPI();
    SHoundFini();
    ErrPostEx(SEV_INFO,2,1,"Done.");
    return 0;

    getout:
    {
       printf("Aborting, check the log file %s for details.\n", LOGING);
       if(pfile != NULL) FileClose(pfile);
       if(pvngi != NULL) ValNodeFree(pvngi);
       if(pvngiall != NULL) ValNodeFree(pvngiall);
       if(pvngroup != NULL) ValNodeFree(pvngroup);
       if(pvnmmdbhead != NULL) ValNodeFree(pvnmmdbhead);
       CloseMMDBAPI();
       if(SHoundIsInited()) SHoundFini();
       return 1;
    }
}

/*
$Log: gen2struc.c,v $
Revision 1.2  2004/01/06 22:32:12  skuznets
Code audit has been done sucessfully.
Possible NULL pointer issues has been eliminated.

Revision 1.1.1.1  2002/09/21 08:44:46  root
Automated import from Sourceforge.net

Revision 1.2  2002/02/15 06:42:23  micheld
Modified makefile targets to include dependency building.

Revision 1.1.1.1  2001/05/25 21:36:19  kaca
initial import

Revision 1.1.1.1  2001/05/25 20:43:00  kaca
code examples for SeqHound

Revision 1.4  2001/03/26 19:59:59  kaca
changes in include files

Revision 1.3  2001/02/26 15:40:44  kaca
all Intrez calls changed to SHound calls

Revision 1.2  2001/02/23 21:05:19  kaca
license change

Revision 1.1.1.1  2001/02/23 17:54:40  kaca
moved out of Source Forge  part of SeqHound

*/
