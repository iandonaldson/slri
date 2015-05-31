/* $Id: shtest.c,v 1.2 2003/10/01 19:57:53 haocl Exp $
*******************************************************************************
$Copyright (C) 2000 Mount Sinai Hospital (MSH)

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
           Katerina Michalickova
	   Hao Lieu
	   and Christopher W.V. Hogue
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
    January 1999
    tests SeqHound remote API functions;
    October 2001
    Tests the all functions in current SeqHound but neighbours.
    The default params gave positive results at the time of creating this test.
    If functions fail; it could be caused by a function param which ceased to
    exist.
    Some of the params can be changed through input arguments; some are fixed for
    the sake of simplicity.


*****************************************************************************************/
#include <seqhound.h>

#define LOGING "shtestlog"

#define NUMARGS 5
Args myargs[NUMARGS] = {
        {"GI","230779",NULL,NULL,TRUE,'g',ARG_INT,0.0,0,NULL},
        {"Accession","P00952",NULL,NULL,TRUE,'a',ARG_STRING,0.0,0,NULL},
        {"MMDBID","11",NULL,NULL,TRUE,'m',ARG_INT,0.0,0,NULL},
        {"Taxonomy ID","56636",NULL,NULL,TRUE,'t',ARG_INT,0.0,0,NULL},
        {"Chromosomal ID","142",NULL,NULL,TRUE,'c',ARG_INT,0.0,0,NULL},
	};


Int2 Main()
{
   BioseqPtr pbs = NULL;
   Int4 gi = 0;
   ValNodePtr pvngi = NULL;
   ValNodePtr pvngi3 = NULL;
    ValNodePtr pvngi4 = NULL;
   ValNodePtr pvnacc = NULL;
   CharPtr pacc = NULL;
   ValNodePtr pvngi2 = NULL;
   SeqIdPtr psi = NULL;
   ValNodePtr pvnseqid = NULL;
   Int4 bwhat = 0;
   ValNodePtr pvnbs = NULL;
   SLRIFastaPtr pfa = NULL;
   ValNodePtr pvnfasta = NULL;
   BiostrucPtr pbio = NULL;
   PDNMS pdnmsModelstruc=NULL;
   ValNodePtr pvnmmdbid = NULL;
   ValNodePtr pvnstrgi = NULL;
   Int4 mmdbid = 0;
   ValNodePtr pvnmmdbid2 = NULL;
   Int4 gin = 0;
   ValNodePtr pvngin = NULL;
   ValNodePtr pvngia = NULL;
   ValNodePtr pvnmuid = NULL;
   Int4 taxid = 0;
   ValNodePtr pvntaxid = NULL;
   ValNodePtr pvnchromid = NULL;
   ValNodePtr pvngen = NULL;
   ValNodePtr pvnchrom = NULL;
   SeqEntryPtr pse = NULL;
   BioseqSetPtr pbss = NULL;
   FILE* pfile = NULL;
   time_t tim1,tim2;
   ValNodePtr vnp2 = NULL, vnp = NULL;
   CharPtr pchar = NULL;
   Int4 i = 0;
   CharPtr pi = NULL;
   ValNodePtr goList = NULL, omimList = NULL;
   ValNodePtr vnpi = NULL, giList = NULL, vnpk = NULL, lllist = NULL;
   ValNodePtr cddidlist = NULL;

	AsnIoPtr aip2 = NULL;
	BiostrucPtr pbiostruc = NULL;
    PDNMS pdnms=NULL;

   ErrSetOptFlags(EO_LOG_SEVERITY | EO_LOG_CODES | EO_LOG_FILELINE | EO_LOG_USRFILE);
   ErrSetLogfile(LOGING, ELOG_BANNER);
   ErrSetLogLevel(SEV_INFO);
   ErrSetMessageLevel(SEV_MAX);

   tim1=GetSecs();
   if (!GetArgs("shtest",NUMARGS, myargs))
       return 1;

	if (OpenMMDBAPI(0,NULL))
       printf("OpenMMDBAPI OK.\n");
    else
    {
       printf("OpenMMDBAPI failed.\n");
       return 2;
    }

    if(SHoundInit(FALSE, "shtest"))
      printf("SHoundInit OK.\n");
    else
      printf("SHoundInit failed.\n");


    if(SHoundIsInited())
      printf("SHoundIsInited OK.\n");
    else
    {
      printf("SHoundIsInited failed.\n");
      return 1;
    }


    if(SHoundExists(myargs[0].intvalue))
      printf("SHoundExists OK.\n");
    else
    {
      printf("SHoundExist failed. Select a GI which exists.\n");
      return 1;
    }


    if (OpenMMDBAPI(0,NULL))
       printf("OpenMMDBAPI OK.\n");
    else
    {
       printf("OpenMMDBAPI failed.\n");
       return 2;
    }

    /*
    if(SHoundNetEntrezInit("shtest"))
       printf("SHoundNetEntrezInit OK.\n");
    else
      printf("SHoundNetEntrezInit failed.\n");


    if(SHoundIsNetEntrezOn())
       printf("SHoundIsNetEntrezOn OK.\n");
    else
       printf("SHoundIsNetEntrezOn failed.\n");
    */

    gi = SHoundFindAcc(myargs[1].strvalue);
    if(gi > 0)
       printf("SHoundFindAcc OK.\n");
    else
       printf("SHoundFindAcc failed.\n");


    ValNodeAddStr(&pvnacc, 0, StrSave(myargs[1].strvalue));
    ValNodeAddStr(&pvnacc, 0, StrSave(myargs[1].strvalue));
    ValNodeAddStr(&pvnacc, 0, StrSave(myargs[1].strvalue));
    pvngi = SHoundFindAccList(pvnacc);
    if (pvngi == NULL)
        printf("SHoundFindAccList failed.\n");
    else
        printf("SHoundFindAccList OK.\n");
    pvngi = ValNodeFree(pvngi);
    pvnacc = ValNodeFreeData(pvnacc);


    pacc = SHoundAccFromGi(myargs[0].intvalue);
    if(pacc == NULL)
       printf("SHoundAccFromGi failed.\n");
    else
       printf("SHoundAccFromGi OK.\n");


    gi = SHoundFindName("BtMIsATn");
    if(gi > 0)
       printf("SHoundFindName OK.\n");
    else
       printf("SHoundFindName failed.\n");


    ValNodeAddStr(&pvnacc, 0, StrSave("BtMIsATn"));
    ValNodeAddStr(&pvnacc, 0, StrSave("ECJ224437"));
    ValNodeAddStr(&pvnacc, 0, StrSave("AF165032"));
    pvngi = SHoundFindNameList(pvnacc);
    if (pvngi == NULL)
        printf("SHoundFindNameList failed.\n");
    else
        printf("SHoundFindNameList OK.\n");
    pvngi = ValNodeFree(pvngi);
    pvnacc = ValNodeFreeData(pvnacc);





    ValNodeAddInt(&pvngi, 0, myargs[0].intvalue);
    ValNodeAddInt(&pvngi, 0, myargs[0].intvalue);
    ValNodeAddInt(&pvngi, 0, myargs[0].intvalue);
    pvnacc = SHoundAccFromGiList(pvngi);
    if(pvnacc == NULL)
       printf("SHoundAccFromGiList failed.\n");
    else
       printf("SHoundAccFromGiList OK.\n");
    pvnacc = ValNodeFreeData(pvnacc);


    psi = SHoundSeqIdFromGi(myargs[0].intvalue);
    if(psi == NULL)
       printf("SHoundSeqIdFromGi failed.\n");
    else
       printf("SHoundSeqIdFromGi OK.\n");
    psi = FreeSeqId(psi);


    pvnseqid = SHoundSeqIdFromGiList(pvngi);
    if(pvnseqid == NULL)
       printf("SHoundSeqIdFromGiList failed.\n");
    else
       printf("SHoundSeqIdFromGiList OK.\n");
    FreeVNSeqIdList(pvnseqid);


    if(SHoundIsProtein(myargs[0].intvalue))
      printf("SHoundIsProtein TRUE.\n");
    else
      printf("SHoundIsProtein FALSE.\n");


    if(SHoundIsNRFirst(1172650))
      printf("SHoundIsNRFirst TRUE.\n");
    else
      printf("SHoundIsNRFirst FALSE.\n");


    pbs = SHoundGetBioseq(myargs[0].intvalue);
    if (pbs == NULL)
      printf("SHoundBioseq failed.\n");
    else
      printf("SHoundGetBioseq OK.\n");
    pbs = BioseqFree(pbs);


    pvnbs = SHoundGetBioseqList(pvngi);
    if (pvnbs == NULL)
      printf("SHoundGetBioseqList failed.\n");
    else
      printf("SHoundGetBioseqList OK.\n");
    pvnbs = FreeBioseqList(pvnbs);


    if(!SHound3DExists(myargs[2].intvalue))
    {
      printf("SHound3DExists FALSE. Select a structure which exists.\n");
      return 1;
    }
    else
    {
        printf("SHound3DExists OK.\n");
        if ((bwhat = SHound3DbWhat(myargs[2].intvalue)) > 0)
            printf("SHound3DbWhat OK.\n");
        else
            printf("SHound3DbWhat failed.\n");

        pbio = SHoundGet3D(myargs[2].intvalue);
        if (pbio == NULL)
           printf("SHound3D failed.\n");
        else
        {
            printf("SHoundGet3D OK.\n");
            pdnmsModelstruc=MakeAModelstruc(pbio);
            if (pdnmsModelstruc == NULL)
            {
               printf("MakeAModelstruc failed");
               BiostrucFree(pbio);
            }
            else
            {
                printf("MakeAModelstruc OK.\n");
                ClearStructures();
            }
        }
    }


    pfa = SHoundGetFasta(myargs[0].intvalue);
    if (pfa == NULL)
      printf("SHoundFasta failed.\n");
    else
      printf("SHoundGetFasta OK.\n");


    pvnfasta = SHoundGetFastaList(pvngi);
    if (pvnfasta == NULL)
      printf("SHoundGetFastaList failed.\n");
    else
      printf("SHoundGetFastaList OK.\n");
    pvnfasta = FreeFastaList(pvnfasta);


    pvnstrgi = SHoundGiFrom3D(myargs[2].intvalue, FALSE);
    if (pvnstrgi == NULL)
      printf("SHoundGiFrom3D failed.\n");
    else
      printf("SHoundGiFrom3D OK.\n");
    pvnstrgi = ValNodeFree(pvnstrgi);


    ValNodeAddInt(&pvnmmdbid, 0, myargs[2].intvalue);
    ValNodeAddInt(&pvnmmdbid, 0, myargs[2].intvalue);
    ValNodeAddInt(&pvnmmdbid, 0, myargs[2].intvalue);
    pvnstrgi = SHoundGiFrom3DList(pvnmmdbid, FALSE);
    if (pvnstrgi == NULL)
      printf("SHoundGiFrom3DList failed.\n");
    else
       printf("SHoundGiFrom3DList OK.\n");
    pvnstrgi = ValNodeFree(pvnstrgi);


    mmdbid = SHound3DFromGi(myargs[0].intvalue);
    if(mmdbid > 0)
      printf("SHound3DFromGi OK.\n");
    else
     printf("SHound3DFromGi failed.\n");


    pvnmmdbid2 = SHound3DFromGiList(pvngi);
    if(pvnmmdbid2 == NULL)
       printf("SHound3DFromGiList failed.\n");
    else
       printf("SHound3DFromGiList OK.\n");
    pvnmmdbid2 = ValNodeFree(pvnmmdbid2);


    ValNodeAddInt(&pvngi3, 0, 2463028);
    ValNodeAddInt(&pvngi3, 0, 2463028);
    ValNodeAddInt(&pvngi3, 0, 2463028);
    pvngia = SHoundProteinFromDNA(2463028);
    if(pvngia == NULL)
       printf("SHoundProteinFromDNA failed.\n");
    else
       printf("SHoundProteinFromDNA OK.\n");
    pvngia = ValNodeFree(pvngia);


    pvngia = SHoundProteinFromDNAList(pvngi3);
    if(pvngia == NULL)
       printf("SHoundProteinFromDNAList failed.\n");
    else
       printf("SHoundProteinFromDNAList OK.\n");


    gin = SHoundDNAFromProtein(8698831);
    if(gin > 0)
      printf("SHoundDNAFromProtein OK.\n");
    else
      printf("SHoundDNAFromProtein failed.\n");


    pvngin = SHoundDNAFromProteinList(pvngia);
    if(pvngin == NULL)
       printf("SHoundDNAFromProteinList failed.\n");
    else
      printf("SHoundDNAFromProteinList OK.\n");
    pvngin = ValNodeFree(pvngin);
    pvngia = ValNodeFree(pvngia);


    pvnmuid = SHoundMuidFromGi(myargs[0].intvalue);
    if(pvnmuid == NULL)
       printf("SHoundMuidFromGi failed.\n");
    else
      printf("SHoundMuidFromGi OK.\n");
    pvnmuid = ValNodeFree(pvnmuid);


    pvnmuid = SHoundMuidFromGiList(pvngi);
    if(pvnmuid == NULL)
       printf("SHoundMuidFromGiList failed.\n");
    else
       printf("SHoundMuidFromGiList OK.\n");
    pvnmuid = ValNodeFree(pvnmuid);


    pvnmuid = SHoundMuidFrom3D(myargs[2].intvalue);
    if(pvnmuid == NULL)
       printf("SHoundMuidFrom3D failed.\n");
    else
       printf("SHoundMuidFrom3D OK.\n");
    pvnmuid = ValNodeFree(pvnmuid);


    pvnmuid = SHoundMuidFrom3DList(pvnmmdbid);
    if(pvnmuid == NULL)
       printf("SHoundMuidFrom3DList failed.\n");
    else
       printf("SHoundMuidFrom3DList OK.\n");
    pvnmuid = ValNodeFree(pvnmuid);


    taxid = SHoundTaxIDFromGi(myargs[0].intvalue);
    if(taxid > 0)
      printf("SHoundTaxIDFromGi OK.\n");
    else
      printf("SHoundTaxIDFromGi failed.\n");


    pvntaxid = SHoundTaxIDFromGiList(pvngi);
    if(pvntaxid == NULL)
       printf("SHoundTaxIDFromGiList failed.\n");
    else
       printf("SHoundTaxIDFromGiList OK.\n");
    pvntaxid = ValNodeFree(pvntaxid);


    pvntaxid = SHoundTaxIDFrom3D(myargs[2].intvalue);
    if(pvntaxid == NULL)
       printf("SHoundTaxIDFrom3D failed.\n");
    else
      printf("SHoundTaxIDFrom3D OK.\n");
    pvntaxid = ValNodeFree(pvntaxid);


    pvntaxid = SHoundTaxIDFrom3DList(pvnmmdbid);
    if(pvntaxid == NULL)
       printf("SHoundTaxIDFrom3DList failed.\n");
    else
      printf("SHoundTaxIDFrom3DList OK.\n");
    pvntaxid = ValNodeFree(pvntaxid);


    pvngia = SHoundProteinsFromOrganism(myargs[3].intvalue, CHROM_ALL);
    if (pvngia == NULL)
       printf("SHoundProteinsFromOrganism failed.\n");
    else
       printf("SHoundProteinsFromOrganism OK.\n");
    pvngia = ValNodeFree(pvngia);


    ValNodeAddInt(&pvntaxid, 0, myargs[3].intvalue);
    ValNodeAddInt(&pvntaxid, 0, myargs[3].intvalue);
    ValNodeAddInt(&pvntaxid, 0, myargs[3].intvalue);
    pvngia = SHoundProteinsFromOrganismList(pvntaxid, CHROM_ALL);
    if (pvngia == NULL)
      printf("SHoundProteinsFromOrganismList failed.\n");
    else
      printf("SHoundProteinsFromOrganismList OK.\n");
    pvngia = ValNodeFree(pvngia);


    pvngia = SHoundProteinsFromChromosome(myargs[4].intvalue);
    if (pvngia == NULL)
       printf("SHoundProteinsFromChromosome failed.\n");
    else
       printf("SHoundProteinsFromChromosome OK.\n");
    pvngia = ValNodeFree(pvngia);


    ValNodeAddInt(&pvnchromid, 0, 13578);
    ValNodeAddInt(&pvnchromid, 0, 13578);
    ValNodeAddInt(&pvnchromid, 0, 13578);
    pvngia = SHoundProteinsFromChromosomeList(pvnchromid);
    if (pvngia == NULL)
       printf("SHoundProteinsFromChromosomeList failed.\n");
    else
       printf("SHoundProteinsFromChromosomeList OK.\n");
    pvngia = ValNodeFree(pvngia);


    pvngen = SHoundAllGenomes();
    if(pvngen == NULL)
     printf("SHoundAllGenomes failed.\n");
    else
      printf("SHoundAllGenomes OK.\n");


    pvnchrom = SHoundChromosomeFromGenome(myargs[3].intvalue, CHROM_ALL);
    if(pvnchrom == NULL)
       printf("SHoundChromosomeFromGenome failed.\n");
    else
       printf("SHoundChromosomeFromGenome OK.\n");
    pvnchrom = ValNodeFree(pvnchrom);


    pvnchrom = SHoundChromosomeFromGenomeList(pvngen, CHROM_ALL);
    if(pvnchrom == NULL)
       printf("SHoundChromosomeFromGenomeList failed.\n");
    else
       printf("SHoundChromosomeFromGenomeList OK.\n");
    pvnchrom = ValNodeFree(pvnchrom);


    pvngin = SHoundDNAFromOrganism(myargs[3].intvalue, CHROM_ALL);
    if (pvngin == NULL)
       printf("SHoundDNAFromOrganism failed.\n");
    else
       printf("SHoundDNAFromOrganism OK.\n");
    pvngin = ValNodeFree(pvngin);


    pvngin = SHoundDNAFromOrganismList(pvntaxid, CHROM_ALL);
    if (pvngin == NULL)
       printf("SHoundDNAFromOrganismList failed.\n");
    else
       printf("SHoundDNAFromOrganismList OK.\n");
    pvngin = ValNodeFree(pvngin);


    pvngin = SHoundDNAFromChromosome(myargs[4].intvalue);
    if (pvngin == NULL)
       printf("SHoundDNAFromChromosome failed.\n");
    else
       printf("SHoundDNAFromChromosome OK.\n");
    pvngin = ValNodeFree(pvngin);



    pvngin = SHoundDNAFromChromosomeList(pvnchromid);
    if (pvngin == NULL)
       printf("SHoundDNAFromChromosomeList failed.\n");
    else
       printf("SHoundDNAFromChromosomeList OK.\n");
    pvngin = ValNodeFree(pvngin);


    pvngi2 = SHoundRedundantGroup(myargs[0].intvalue);
    if(pvngi2 == NULL)
       printf("SHoundRedundantGroup failed.\n");
    else
       printf("SHoundRedundantGroup OK.\n");
    pvngi2 = ValNodeFree(pvngi2);


    pvngi2 = SHoundRedundantGroupList(pvngi);
    if(pvngi2 == NULL)
      printf("SHoundRedundantGroupList failed.\n");
    else
      printf("SHoundRedundantGroupList OK.\n");
    pvngi2 = ValNodeFree(pvngi2);


    pvngi2 = SHoundProteinsFromTaxID(myargs[3].intvalue);
    if(pvngi2 == NULL)
       printf("SHoundProteinsFromTaxID failed.\n");
    else
       printf("SHoundProteinsFromTaxID OK.\n");
    pvngi2 = ValNodeFree(pvngi2);

    pvngi2 = SHoundDNAFromTaxID(myargs[3].intvalue);
    if(pvngi2 == NULL)
      printf("SHoundDNAFromTaxID failed.\n");
    else
      printf("SHoundDNAFromTaxID OK.\n");
    pvngi2 = ValNodeFree(pvngi2);


    pvngi2 = SHoundProteinsFromTaxIDList(pvntaxid);
    if(pvntaxid == NULL)
      printf("SHoundProteinsFromTaxIDList failed.\n");
    else
      printf("SHoundProteinsFromTaxIDList OK.\n");
    pvngi2 = ValNodeFree(pvngi2);


    pvngi2 = SHoundDNAFromTaxIDList(pvntaxid);
    if(pvntaxid == NULL)
       printf("SHoundDNAFromTaxIDList failed.\n");
    else
       printf("SHoundDNAFromTaxIDList OK.\n");
    pvngi2 = ValNodeFree(pvngi2);


    pse = SHoundGetSeqEntry(myargs[0].intvalue);
    if (pse == NULL)
      printf("SHoundGetSeqEntry failed.\n");
    else
      printf("SHoundGetSeqEntry OK.\n");
    pse = SeqEntryFree(pse);


    pvnbs = SHoundGetSeqEntryList(pvngi);
    if (pvnbs == NULL)
      printf("SHoundSeqEntryList failed.\n");
    else
      printf("SHoundGetSeqEntryList OK\n");
    pvnbs = FreeSeqEntryList(pvnbs);

    pbss = SHoundGetBioseqSet(2791836);
    if (pbss == NULL)
      printf("SHoundBioseqSet failed.\n");
    else
      printf("SHoundGetBioseqSet OK.\n");
    pbss = BioseqSetFree(pbss);


    ValNodeAddInt(&pvngi4, 0, 2791836);
    ValNodeAddInt(&pvngi4, 0, 2791836);
    pvnbs = SHoundGetBioseqSetList(pvngi4);
    if (pvnbs == NULL)
      printf("SHoundGetBioseqSetList failed.\n");
    else
      printf("SHoundGetBioseqSetList OK.\n");
    pvnbs = FreeBioseqSetList(pvnbs);


    pfile = FileOpen("flat", "w");
    if(!SHoundGetGenBankff(myargs[0].intvalue, pfile))
      printf("SHoundGetGenBankff failed.\n");
    else
      printf("SHoundGetGenBankff OK.\n");
    FileClose(pfile);

    pfile = FileOpen("flat", "w");
    if(!SHoundGetGenBankffList(pvngi, pfile))
      printf("SHoundGetGenBankffList failed.\n");
    else
      printf("SHoundGetGenBankffList OK.\n");
    FileClose(pfile);


    if(!SHoundGetXMLBioseq(myargs[0].intvalue, "xmlbioseq"))
      printf("SHoundGetXMLBioseq failed.\n");
    else
      printf("SHoundGetXMLBioseq OK.\n");


    if(!SHoundGetXMLSeqEntry(myargs[0].intvalue, "xmlseqentry"))
      printf("SHoundGetXMLSeqEntry failed.\n");
    else
      printf("SHoundGetXMLSeqEntry OK.\n");


	vnp = SHoundGODBGetChildrenOf(7612);
	if (vnp == NULL)
	  printf("SHoundGODBGetChildrenOf failed.\n");
        else
          printf("SHoundGODBGetChildrenOf OK.\n");
	vnp = ValNodeFree(vnp);

	vnp = SHoundGODBGetParentOf(5381);
	if (vnp == NULL)
	  printf("SHoundGODBGetParentOf failed.\n");
        else
          printf("SHoundGODBGetParentOf OK.\n");
	vnp = ValNodeFree(vnp);

	pchar = SHoundGODBGetNameByID(7625);
	if(pchar == NULL)
	 printf("SHoundGODBGetNameByID failed.\n");
	else
          printf("SHoundGODBGetNameByID OK.\n");
	pchar = MemFree(pchar);

	vnp2 = SHoundGODBGetAllAncestors(7627);
	if (vnp2 == NULL)
	  printf("SHoundGODBGetAllAncestors failed.\n");
        else
          printf("SHoundGODBGetAllAncestors OK.\n");
	vnp2 = ValNodeFree(vnp2);

	vnp2 = SHoundGODBGetAllChildren(7627);
	if (vnp2 == NULL)
	  printf("SHoundGODBGetAllChildren failed.\n");
        else
          printf("SHoundGODBGetAllChildren OK.\n");
	vnp2 = ValNodeFree(vnp2);

	vnp2 = SHoundGODBGetRecordByReference("EC");
	if (vnp2 == NULL)
	  printf("SHoundGODBGetRecordByReference failed.\n");
        else
          printf("SHoundGODBGetRecordByReference OK.\n");
	vnp2 = ValNodeFree(vnp2);

	vnp2 = SHoundGODBGetRecordByReference("ISBN");
	if (vnp2 == NULL)
	  printf("SHoundGODBGetRecordByReference failed.\n");
        else
          printf("SHoundGODBGetRecordByReference OK.\n");
	vnp2 = ValNodeFree(vnp2);

	
	pi = SHoundGODBGetClassification(5935);
	if(pi == NULL)
	 printf("SHoundGODBGetClassification failed.\n");
        else
          printf("SHoundGODBGetClassification OK.\n");


	ValNodeAddInt(&lllist, 0, 1);
	ValNodeAddInt(&lllist, 0, 2);
	ValNodeAddInt(&lllist, 0, 13);

	ValNodeAddInt(&goList, 0, 5737);
	ValNodeAddInt(&goList, 0, 8033);
	ValNodeAddInt(&goList, 0, 4867);
	ValNodeAddInt(&goList, 0, 6419);

	ValNodeAddInt(&omimList, 0, 137150);
	ValNodeAddInt(&omimList, 0, 103950);
	ValNodeAddInt(&omimList, 0, 108345);

	ValNodeCopyStr(&cddidlist, 0, "pfam00202");
	ValNodeCopyStr(&cddidlist, 0, "pfam01835");
	ValNodeCopyStr(&cddidlist, 0, "pfam00079");

	vnpi = SHoundGiFromGOID(8320);
	if(vnpi == NULL)
	   printf("SHoundGiFromGOID failed.\n");
	else
          printf("SHoundGiFromGOID OK.\n");
	vnpi = ValNodeFree(vnpi);

	vnpi = SHoundGiFromGOIDList(goList);
	if(vnpi == NULL)
	   printf("SHoundGiFromGOIDList failed.\n");
	else
          printf("SHoundGiFromGOIDList OK.\n");
	vnpi = ValNodeFree(vnpi);

	vnp2 = SHoundGOIDFromGi(4557225);
	if (vnp2 == NULL)
	  printf("SHoundGOIDFromGi failed.\n");
        else
          printf("SHoundGOIDFromGi OK.\n");
	vnp2 = ValNodeFree(vnp2);

	ValNodeAddInt(&giList, 0, 4501841);
	ValNodeAddInt(&giList, 0, 4557225);
	vnpk = SHoundGOIDFromGiList(giList);
	if (vnpk == NULL)
	  printf("SHoundGOIDFromGiList failed.\n");
        else
          printf("SHoundGOIDFromGiList OK.\n");
	vnpk = ValNodeFree(vnpk);

	vnp = SHoundGOECodeFromGiAndGOID(4501841, 3943);
	if (vnp == NULL)
	  printf("SHoundGOECodeFromGi failed.\n");
        else
          printf("SHoundGOECodeFromGi OK.\n");
	vnp = ValNodeFreeData(vnp);

	vnp = SHoundGOPMIDFromGiAndGOID(4501841, 4323);
	if (vnp == NULL)
	  printf("SHoundGOPMIDFromGi failed.\n");
        else
          printf("SHoundGOPMIDFromGi OK.\n");
	vnp = ValNodeFree(vnp);

	vnp = SHoundGiFromOMIM(103950);
	if (vnp == NULL)
	  printf("SHoundGiFromOMIM failed.\n");
        else
          printf("SHoundGiFromOMIM OK.\n");
	vnp = ValNodeFree(vnp);

	vnp = SHoundGiFromOMIMList(omimList);
	if (vnp == NULL)
	  printf("SHoundGiFromOMIMList failed.\n");
        else
          printf("SHoundGiFromOMIMList OK.\n");
	vnp = ValNodeFree(vnp);

	vnp = SHoundOMIMFromGi(4505335);
	if (vnp == NULL)
	  printf("SHoundOMIMFromGi failed.\n");
        else
          printf("SHoundOMIMFromGi OK.\n");
	vnp = ValNodeFree(vnp);

	vnp = SHoundOMIMFromGiList(giList);
	if (vnp == NULL)
	  printf("SHoundOMIMFromGiList failed.\n");
        else
          printf("SHoundOMIMFromGiList OK.\n");
	vnp = ValNodeFree(vnp);

	vnp = SHoundGiFromLLID(9);
	if (vnp == NULL)
	  printf("SHoundGiFromLLID failed.\n");
        else
          printf("SHoundGiFromLLID OK.\n");
	vnp = ValNodeFree(vnp);

	vnp = SHoundGiFromLLIDList(lllist);
	if (vnp == NULL)
	  printf("SHoundGiFromLLIDList failed.\n");
        else
          printf("SHoundGiFromLLIDList OK.\n");
	vnp = ValNodeFree(vnp);

	i = 0;
	i = SHoundLLIDFromGi(4501843);
	if(i == 0)
	  printf("SHoundLLIDFromGi failed.\n");
        else
          printf("SHoundLLIDFromGi OK.\n");

	vnp = SHoundLLIDFromGiList(giList);
	if (vnp == NULL)
	  printf("SHoundLLIDFromGiList failed.\n");
        else
          printf("SHoundLLIDFromGiList OK.\n");
	vnp = ValNodeFree(vnp);

	pchar = SHoundLocusFromGi(4501843);
	if(pchar == NULL)
	  printf("SHoundLocusFromGi failed.\n");
        else
          printf("SHoundLocusFromGi OK.\n");
	pchar = MemFree(pchar);

	vnp = SHoundGiFromCDDID("EPEND");
	if (vnp == NULL)
	  printf("SHoundGiFromCDDID failed.\n");
        else
          printf("SHoundGiFromCDDID OK.\n");
	vnp = ValNodeFree(vnp);

	vnp = SHoundGiFromCDDIDList(cddidlist);
	if (vnp == NULL)
	  printf("SHoundGiFromCDDIDList failed.\n");
        else
          printf("SHoundGiFromCDDIDList OK.\n");
	vnp = ValNodeFree(vnp);

	vnp = SHoundCDDIDFromGi(4557225);
	if (vnp == NULL)
	  printf("SHoundCDDIDFromGi failed.\n");
        else
          printf("SHoundCDDIDFromGi OK.\n");
	vnp = ValNodeFreeData(vnp);

	vnp = SHoundCDDIDFromGiList(giList);
	if (vnp == NULL)
	  printf("SHoundCDDIDFromGiList failed.\n");
        else
          printf("SHoundCDDIDFromGiList OK.\n");
	vnp = ValNodeFreeData(vnp);


    CloseMMDBAPI();
    printf("MMDBAPI closed.\n");


    SHoundFini();
    printf("SHoundFini OK.\n");


    ValNodeFree(pvngi);
    ValNodeFree(pvngi3);
    ValNodeFree(pvnmmdbid);
    ValNodeFree(pvntaxid);
    ValNodeFree(pvnchromid);
    ValNodeFree(lllist);
    ValNodeFree(goList);
    ValNodeFree(omimList);
    ValNodeFree(giList);
    ValNodeFreeData(cddidlist);

    tim2=GetSecs();
    printf("Total elapsed time: %ld sec. ",(tim2-tim1));
    printf("Done.\n");
    return 0;
}

/*
 $Log: shtest.c,v $
 Revision 1.2  2003/10/01 19:57:53  haocl
 changed [SHoundGOECode|SHoundPMID]FromGi to [SHoundGOECode|SHoundPMID]FromGiAndGOID

 Revision 1.1.1.5  2002/05/17 08:27:32  root
 Automated import from Sourceforge.net

 Revision 1.5  2002/05/16 20:29:23  kaca
 removed unwanted test code

 Revision 1.4  2002/05/13 20:11:31  kaca
 changed FTP URL for NCBI

 Revision 1.3  2002/01/15 17:33:28  kaca
 disabled SHoundGODBGetHierarchyLevel due to a wrong logic

 Revision 1.2  2001/11/19 20:23:01  kaca
 fixed printf statement

 Revision 1.1  2001/10/26 21:10:12  kaca
 API test

 Revision 1.2  2001/06/04 19:42:51  kaca
 more functions

 Revision 1.1  2001/04/12 21:06:43  kaca
 initial import

 Revision 1.3  2001/03/26 20:17:13  kaca
 changes in includes

 Revision 1.2  2001/02/23 21:10:08  kaca
 license change

 Revision 1.1.1.1  2001/02/23 17:54:40  kaca
 moved out of Source Forge  part of SeqHound

 Revision 1.1.1.1  2000/09/18 17:41:55  kaca
 Initial import
*/




