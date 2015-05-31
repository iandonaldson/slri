/* $Id: shtest_detail.c,v 1.3 2004/01/06 22:32:14 skuznets Exp $
 *******************************************************************************
 $Copyright(C) 2000 Mount Sinai Hospital (MSH)

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

  /* Gets any config file integer, given a cfg file section/key pair.
    Returns -1 if there is an error.*/
 Int4 GetIntValue_fromCFG(CharPtr section, CharPtr key)
 {
         Int4 value=0;
         CharPtr buf=NULL, tmp=NULL;

         buf = MemNew((size_t) 4*sizeof(Char));
         Mem_Check(buf);
         tmp = MemNew((size_t) 32*sizeof(Char));
         Mem_Check(tmp);

         /*Error - Please make sure that a SHound configuration file exists.*/
         sprintf(buf, "-1");

         /*search the init file for this information*/
         GetAppParam("shtest", section, key, buf, tmp, (size_t) 32 * sizeof(Char));
         if(strlen(tmp) == 0)
           ErrPostEx(SEV_ERROR, 0, 0, "Cfg_GetIntValue: Config file or an entry in a config file missing.");
         /*convert to int*/
         tmp[31]='\0';   /*protect atoi function*/
         value = atoi(tmp);

         buf = MemFree(buf);
         tmp = MemFree(tmp);
         return(value);
 }

 Int2 Main()
 {
    BioseqPtr pbs = NULL;
    Int4 gi = 0;
    ValNodePtr pvngi = NULL, head = NULL;
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
    ValNodePtr pvntaxid = NULL, pvntaxid1 = NULL;
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
    SLRIFastaPtr slrifasta;

    AsnIoPtr aip = NULL;
    Int4 gi_value = 0, taxid_value=0,chromid_value=0,goid_value=0, module=0;
    ErrSetOptFlags(EO_LOG_SEVERITY | EO_LOG_CODES | EO_LOG_FILELINE | EO_LOG_USRFILE);
    ErrSetLogfile(LOGING, ELOG_BANNER);
    ErrSetLogLevel(SEV_INFO);
    ErrSetMessageLevel(SEV_MAX);

    tim1=GetSecs();
    if (!GetArgs("shtest",NUMARGS, myargs))
        return 1;

    /* get all the values from the configu file */
    gi_value = GetIntValue_fromCFG("gis","gi1");
    taxid_value = GetIntValue_fromCFG("taxid","taxid1");
    chromid_value =  GetIntValue_fromCFG("chromid", "chromid1");
    goid_value =  GetIntValue_fromCFG("goid", "goid1");

     ValNodeAddInt(&pvngi, 0, gi_value);
     ValNodeAddInt(&pvngi, 0, GetIntValue_fromCFG("gis","gi2"));
     ValNodeAddInt(&pvngi, 0, GetIntValue_fromCFG("gis","gi3"));

     ValNodeAddStr(&pvnacc, 0, StrSave(myargs[1].strvalue));
     ValNodeAddStr(&pvnacc, 0, StrSave("XP_007702"));
     ValNodeAddStr(&pvnacc, 0, StrSave("NC_002757"));

     ValNodeAddStr(&pvnacc, 0, StrSave("BtMIsATn"));
     ValNodeAddStr(&pvnacc, 0, StrSave("ECJ224437"));
     ValNodeAddStr(&pvnacc, 0, StrSave("AF165032"));

     ValNodeAddInt(&pvngi4, 0, 2791836);
     ValNodeAddInt(&pvngi4, 0, 2791836);

     ValNodeAddInt(&pvnmmdbid, 0, myargs[2].intvalue);
     ValNodeAddInt(&pvnmmdbid, 0, myargs[2].intvalue);
     ValNodeAddInt(&pvnmmdbid, 0, myargs[2].intvalue);

     ValNodeAddInt(&pvntaxid, 0, taxid_value);
     ValNodeAddInt(&pvntaxid, 0,  GetIntValue_fromCFG("taxid","taxid2"));
     ValNodeAddInt(&pvntaxid, 0,  GetIntValue_fromCFG("taxid","taxid3"));

     ValNodeAddInt(&pvnchromid, 0, 13578);
     ValNodeAddInt(&pvnchromid, 0, 13578);
     ValNodeAddInt(&pvnchromid, 0, 13578);

     ValNodeAddInt(&lllist, 0, GetIntValue_fromCFG("ll","ll1"));
     ValNodeAddInt(&lllist, 0, GetIntValue_fromCFG("ll","ll2"));
     ValNodeAddInt(&lllist, 0, GetIntValue_fromCFG("ll","ll3"));

     ValNodeAddInt(&goList, 0, GetIntValue_fromCFG("goid","goid2"));
     ValNodeAddInt(&goList, 0, GetIntValue_fromCFG("goid","goid3"));
     ValNodeAddInt(&goList, 0, GetIntValue_fromCFG("goid","goid4"));
     ValNodeAddInt(&goList, 0, GetIntValue_fromCFG("goid","goid5"));

     ValNodeAddInt(&omimList, 0,  GetIntValue_fromCFG("omim","omim1"));
     ValNodeAddInt(&omimList, 0, GetIntValue_fromCFG("omim","omim2"));
     ValNodeAddInt(&omimList, 0,  GetIntValue_fromCFG("omim","omim3"));

     ValNodeCopyStr(&cddidlist, 0, "pfam00202");
     ValNodeCopyStr(&cddidlist, 0, "pfam01835");
     ValNodeCopyStr(&cddidlist, 0, "pfam00079");
     
     printf("\n\nShoundInit------------------------------\n");

     if(SHoundInit(FALSE, "shtest"))
       printf("SHoundInit OK.\n");
     else
       printf("SHoundInit failed.\n");

     /*If you want to see the Core funtions results you must define that you
      want it to be included in the config file for shtest. */
    if ((module =  GetIntValue_fromCFG("parts", "corefunc")) == 1){

      printf("\n\nShoundInit------------------------------\n");

      if(SHoundInit(FALSE, "shtest"))
       printf("SHoundInit OK.\n");
      else
       printf("SHoundInit failed.\n");


      printf("\n\nShoundIsInit------------------------------\n");

      if(SHoundIsInited())
       printf("SHoundIsInited OK.\n");
      else
      {
       printf("SHoundIsInited failed.\n");
       return 1;
      }

      printf("\n\nShoundExists------------------------------\n");
      printf("Checking to see if GI: %ld exist.\n", gi_value);

      if(SHoundExists(gi_value))
       printf("SHoundExists OK.  Gi exists\n");
      else
      {
       printf("SHoundExist failed. Select a GI which exists.\n");
       return 1;
      }
  
    printf("\n\nOpenMMDBAPI------------------------------\n");

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

      printf("\n\nShoundFindAcc------------------------------\n");
      printf("Getting the GI for Accession: %s. \n", IsNullStr(myargs[1].strvalue));

      gi = SHoundFindAcc(myargs[1].strvalue);
      if(gi > 0){
        printf("SHoundFindAcc OK.\n");
        printf("GI is %ld For the Accession: %s \n", gi, IsNullStr(myargs[1].strvalue));
      }
      else
        printf("SHoundFindAcc failed.\n");


      printf("\n\nShoundFindAccList------------------------------\n");
      pvngi2 = SHoundFindAccList(pvnacc);
      if (pvngi2 == NULL)
         printf("SHoundFindAccList failed.\n");
      else{
         printf("SHoundFindAccList OK.\n");
         printf("The Gis returned for the list of Accessions were:\n");
         printf("Accessions:  ");
             head = pvnacc;
             while(head != NULL){
               printf(" %s, ", IsNullStr(head->data.ptrvalue));
               head = head->next;
                 }
         
         printf("\nGIs:  ");
             head = pvngi2;
             while(head != NULL){
               printf(" %ld, ", head->data.intvalue);
               head = head->next;
                 }
        printf("\n");
      }
      pvngi2 = ValNodeFree(pvngi2);

      printf("\n\nShoundAccFromGi------------------------------\n");
      printf("Checking the Accession for GI : %ld \n", gi_value);
      pacc = SHoundAccFromGi(gi_value);
      if(pacc == NULL)
        printf("SHoundAccFromGi failed.\n");
      else{
        printf("SHoundAccFromGi OK.\n");
        printf("For GI : %ld the Accession returned is: %s\n", gi_value, IsNullStr(pacc));
      }

      printf("\n\nShoundFindName------------------------------\n");
      printf("Checking the GI for the Name : BtMIsATn\n");
      gi = SHoundFindName("BtMIsATn");
      if(gi > 0){
        printf("SHoundFindName OK.\n");
        printf("For Name: BtMIsATn the GI returned is: %ld\n", gi);
      }       
      else
        printf("SHoundFindName failed.\n");



         
     printf("\n\nShoundFindNameList------------------------------\n");
     pvngi2 = SHoundFindNameList(pvnacc);
     if (pvngi2 == NULL)
         printf("SHoundFindNameList failed.\n");
     else{
         printf("SHoundFindNameList OK.\n");
         printf("The Gis returned for the list of Names were:\n");
         printf("Names:  ");
             head = pvnacc;
             while(head != NULL){
              printf(" %s, ", IsNullStr(head->data.ptrvalue));
              head = head->next;
                 }
         
         printf("\nGIs:  ");
             head = pvngi2;
             while(head != NULL){
              printf(" %ld, ", head->data.intvalue);
              head = head->next;
                 }
         printf("\n");
     }
     pvngi2 = ValNodeFree(pvngi2);

     printf("\n\nShoundAccFromGiList------------------------------\n");
     pvnacc = SHoundAccFromGiList(pvngi);
     if(pvnacc == NULL)
        printf("SHoundAccFromGiList failed.\n");
     else{
         printf("SHoundAccFromGiList OK.\n");
         printf("The Accessions returned for the list of Gis were:\n");
         printf("GIs:  ");
             head = pvngi;
             while(head != NULL){
              printf(" %ld, ", head->data.intvalue);
              head = head->next;
                 }
         
         printf("\nAccessions:  ");
             head = pvnacc;
             while(head != NULL){
              printf(" %s, ", IsNullStr(head->data.ptrvalue));
              head = head->next;
                 }
         printf("\n");
     }

     printf("\n\nShoundSeqIdFromGi------------------------------\n");
     printf("Checking the Seq Id for the GI: %ld\n",gi_value);
     psi = SHoundSeqIdFromGi(gi_value);
     if(psi == NULL)
        printf("SHoundSeqIdFromGi failed.\n");
     else{
        printf("SHoundSeqIdFromGi OK.\n");
        printf("For Gi: %ld the returned SeqId is: %ld\n",gi_value, psi->data.intvalue);
     }
     psi = FreeSeqId(psi);

     printf("\n\nShoundSeqIdFromGiList------------------------------\n");
     pvnseqid = SHoundSeqIdFromGiList(pvngi);
     if(pvnseqid == NULL)
        printf("SHoundSeqIdFromGiList failed.\n");
     else{
             printf("SHoundSeqIdFromGiList OK.\n");
         printf("The SeqIds returned for the list of Gis were:\n");
         printf("GIs:  ");
             head = pvngi;
             while(head != NULL){
              printf(" %ld, ", head->data.intvalue);
              head = head->next;
                 }
         
         printf("\nSeqIds:  ");
             head = pvnseqid;
             while(head != NULL){
              printf(" %ld, ", head->data.intvalue);
              head = head->next;
                 }
         printf("\n");
     }  
     FreeVNSeqIdList(pvnseqid);

     printf("\n\nSHoundIsProtein------------------------------\n");
     printf("Check if GI: %ld is a protein\n",gi_value);
     if(SHoundIsProtein(gi_value))
       printf("SHoundIsProtein TRUE.\n");
     else
       printf("SHoundIsProtein FALSE.\n");

     printf("\n\nShoundIsNRFirst------------------------------\n");
     printf("Check if GI 1172650 is the first protein in redund group.\n");
     if(SHoundIsNRFirst(1172650))
       printf("SHoundIsNRFirst TRUE.\n");
     else
       printf("SHoundIsNRFirst FALSE.\n");

    printf("\n\nShoundGetBioseq------------------------------\n");
     printf("Get the Bioseq for GI: %ld\n", gi_value);
     pbs = SHoundGetBioseq(gi_value);
     if (pbs == NULL)
       printf("SHoundBioseq failed.\n");
     else{
       printf("SHoundGetBioseq OK.\n");
       aip = AsnIoNew(ASNIO_TEXT_OUT, stdout, NULL,NULL,NULL);
       if (aip == NULL)
         printf("Aip is null\n");
       if (!BioseqAsnWrite(pbs,aip,NULL))
         printf("the bioseqasnwrite didn't work\n");
       AsnIoFlush(aip);
       AsnIoClose(aip);

     }
     pbs = BioseqFree(pbs);

     printf("\n\nShoundGetBioseqList------------------------------\n");
     pvnbs = SHoundGetBioseqList(pvngi);
     if (pvnbs == NULL)
       printf("SHoundGetBioseqList failed.\n");
     else{
       printf("SHoundGetBioseqList OK.\n");
           printf("The Bioseqs returned for the list of Gis were:\n");
       printf("GIs:  ");
           head = pvngi;
           while(head != NULL){
            printf(" %ld, ", head->data.intvalue);
            head = head->next;
           }
       printf("\nBioseqs:  ");
           head = pvnbs;
           while(head != NULL){
         aip = AsnIoNew(ASNIO_TEXT_OUT, stdout, NULL,NULL,NULL);
         if (aip == NULL)
           printf("Aip is null\n");
         if (!BioseqAsnWrite(pvnbs->data.ptrvalue,aip,NULL))
           printf("the bioseqasnwrite didn't work\n");
         AsnIoFlush(aip);
         AsnIoClose(aip);
             head = head->next;
           }
       printf("\n");
         }
     pvnbs = FreeBioseqList(pvnbs);

     printf("\n\nShoundGetFasta------------------------------\n");
     pfa = SHoundGetFasta(gi_value);
     if (pfa == NULL)
       printf("SHoundFasta failed.\n");
     else{
       printf("SHoundGetFasta OK.\n");
       printf("For GI: %ld the returned FASTA is: %s|%s",gi_value, IsNullStr(pfa->defline), IsNullStr(pfa->sequence));
     }

     printf("\n\nShoundGetFastaList------------------------------\n");
     pvnfasta = SHoundGetFastaList(pvngi);
     if (pvnfasta == NULL)
       printf("SHoundGetFastaList failed.\n");
     else{
       printf("SHoundGetFastaList OK.\n");
       printf("The Fastas returned for the list of Gis were:\n");
       printf("GIs:  ");
           head = pvngi;
           while(head != NULL){
            printf(" %ld, ", head->data.intvalue);
            head = head->next;
           }
         
       printf("\nFasta:  ");
       head = pvnfasta;
         while(head != NULL){
            slrifasta = (SLRIFastaPtr) head->data.ptrvalue;
	    /* SK: NULL ptr check */
	    if ( slrifasta != NULL )
        	printf(" %s|\n%s\n ", IsNullStr(slrifasta->defline),IsNullStr(slrifasta->sequence));
            head = head->next;
         }
       printf("\n");
     }
     pvnfasta = FreeFastaList(pvnfasta);

     printf("\n\nShoundProteinFromDNA------------------------------\n");      
     ValNodeAddInt(&pvngi, 0, 2463028);
     pvngia = SHoundProteinFromDNA(2463028);
     if(pvngia == NULL)
        printf("SHoundProteinFromDNA failed.\n");
     else{
        printf("SHoundProteinFromDNA OK.\n");
        printf("For GI: 2463028 the protein GIs are:\n");
            head = pvngia;
            while(head != NULL){
              printf(" %ld, ", head->data.intvalue);
              head = head->next;
            }
     }
     pvngia = ValNodeFree(pvngia);

     printf("\n\nShoundProteinFromDNAList------------------------------\n"); 
     pvngia = SHoundProteinFromDNAList(pvngi);
     if(pvngia == NULL)
        printf("SHoundProteinFromDNAList failed.\n");
     else{
        printf("SHoundProteinFromDNAList OK.\n");
            printf("The Protein GIs returned for the list of  DNA Gis were:\n");
        printf("DNA GIs:  ");
            head = pvngi;
            while(head != NULL){
              printf(" %ld, ", head->data.intvalue);
              head = head->next;
            }
         
        printf("\nProtein GIs:  ");
            head = pvngia;
            while(head != NULL){
              printf(" %ld, ", head->data.intvalue);
              head = head->next;
            }
        printf("\n");
     }

     printf("\n\nShoundDNAFromProtein------------------------------\n"); 
     gin = SHoundDNAFromProtein(pvngia->data.intvalue);
     if(gin > 0){
       printf("SHoundDNAFromProtein OK.\n");
       printf("For protein Gi: %ld the returned DNA GI is: %ld\n",pvngia->data.intvalue,gin); 
     }
     else
       printf("SHoundDNAFromProtein failed.\n");
    
     printf("\n\nShoundDNAFromProteinList------------------------------\n"); 
     pvngin = SHoundDNAFromProteinList(pvngia);
     if(pvngin == NULL)
        printf("SHoundDNAFromProteinList failed.\n");
     else{
        printf("SHoundDNAFromProteinList OK.\n");
            printf("The Protein GIs returned for the list of  DNA Gis were:\n");
        printf("Protein GIs:  ");
            head = pvngia;
            while(head != NULL){
              printf(" %ld, ", head->data.intvalue);
              head = head->next;
            }
         
        printf("\nDNA GIs:  ");
            head = pvngin;
            while(head != NULL){
              printf(" %ld, ", head->data.intvalue);
              head = head->next;
            }
        printf("\n");
     }
     pvngin = ValNodeFree(pvngin);
     pvngia = ValNodeFree(pvngia);

     printf("\n\nShoundMuidFromGi------------------------------\n"); 
     pvnmuid = SHoundMuidFromGi(gi_value);
     if(pvnmuid == NULL)
        printf("SHoundMuidFromGi failed.\n");
     else{
        printf("SHoundMuidFromGi OK.\n");
            printf("The Medline Ids returned for the GI:%ld were:\n",gi_value);
            head = pvnmuid;
            while(head != NULL){
              printf(" %ld, ", head->data.intvalue);
              head = head->next;
            }
     }
     pvnmuid = ValNodeFree(pvnmuid);  

     printf("\n\nShoundMuidFromGiList------------------------------\n"); 
     pvnmuid = SHoundMuidFromGiList(pvngi);
     if(pvnmuid == NULL)
        printf("SHoundMuidFromGiList failed.\n");
     else{
        printf("SHoundMuidFromGiList OK.\n");
            printf("The Medline Ids returned for the list of Gis were:\n");
        printf("Medline Ids:  ");
            head = pvnmuid;
            while(head != NULL){
              printf(" %ld, ", head->data.intvalue);
              head = head->next;
            }
         
        printf("\nGIs:  ");
            head = pvngi;
            while(head != NULL){
              printf(" %ld, ", head->data.intvalue);
              head = head->next;
            }
        printf("\n");
         }
     pvnmuid = ValNodeFree(pvnmuid);

     printf("\n\nShoundTaxIDFromGi------------------------------\n"); 
     taxid = SHoundTaxIDFromGi(pvngi->data.intvalue);
     if(taxid > 0){
       printf("SHoundTaxIDFromGi OK.\n");
       printf("For Gi: %ld the returned Taxonomy Id is: %ld\n",pvngi->data.intvalue,taxid); 
     }
     else
       printf("SHoundTaxIDFromGi failed.\n");

     printf("\n\nShoundTaxIDFromGiList------------------------------\n"); 
     pvntaxid1 = SHoundTaxIDFromGiList(pvngi);
     if(pvntaxid1 == NULL)
        printf("SHoundTaxIDFromGiList failed.\n");
     else{ 
        printf("SHoundTaxIDFromGiList OK.\n");
            printf("The Taxonomy Ids returned for the list of Gis were:\n");
        printf("Taxonomy Ids:  ");
            head = pvntaxid1;
            while(head != NULL){
              printf(" %ld, ", head->data.intvalue);
              head = head->next;
            }
         
        printf("\nGIs:  ");
            head = pvngi;
            while(head != NULL){
              printf(" %ld, ", head->data.intvalue);
              head = head->next;
            }
        printf("\n");
     }
     pvntaxid1 = ValNodeFree(pvntaxid1);

     printf("\n\nShoundProteinsFromTaxID------------------------------\n"); 
     pvngi2 = SHoundProteinsFromTaxID(taxid_value);
     if(pvngi2 == NULL)
        printf("SHoundProteinsFromTaxID failed.\n");
     else{
        printf("SHoundProteinsFromTaxID OK.\n");
        printf("The Protein Gis returned for the Taxid:%ld were:\n", taxid_value);
        printf("Protein Gis:  ");
            head = pvngi2;
            while(head != NULL){
              printf(" %ld, ", head->data.intvalue);
              head = head->next;
            }
        printf("\n");
     }
     pvngi2 = ValNodeFree(pvngi2);

     printf("\n\nShoundDNAFromTaxID------------------------------\n"); 
     pvngi2 = SHoundDNAFromTaxID(taxid_value);
     if(pvngi2 == NULL)
       printf("SHoundDNAFromTaxID failed.\n");
     else{
       printf("SHoundDNAFromTaxID OK.\n");
       printf("The DNA Gis returned for the Taxid:%ld were:\n", taxid_value);
       printf("DNA Gis:  ");
           head = pvngi2;
           while(head != NULL){
             printf(" %ld, ", head->data.intvalue);
             head = head->next;
           }
       printf("\n");
     }
     pvngi2 = ValNodeFree(pvngi2);

     printf("\n\nShoundProteinsFromTaxIDList------------------------------\n"); 
       printf("\nTaxids:  ");
           head = pvntaxid;
           while(head != NULL){
            printf(" %ld, ", head->data.intvalue);
            head = head->next;
           }
       printf("\n");

     pvngi2 = SHoundProteinsFromTaxIDList(pvntaxid);
     if(pvngi2 == NULL)
       printf("SHoundProteinsFromTaxIDList failed.\n");
     else{
       printf("SHoundProteinsFromTaxIDList OK.\n");
       printf("The Proteins returned for the list of Taxids were:\n");
       printf("Protein Gis:  ");
           head = pvngi2;
           while(head != NULL){
            printf(" %ld, ", head->data.intvalue);
            head = head->next;
           }
         
       printf("\nTaxids:  ");
           head = pvntaxid;
           while(head != NULL){
            printf(" %ld, ", head->data.intvalue);
            head = head->next;
           }
       printf("\n");
     }
     pvngi2 = ValNodeFree(pvngi2);

     printf("\n\nShoundDNAFromTaxIDList------------------------------\n"); 
     pvngi2 = SHoundDNAFromTaxIDList(pvntaxid);
     if(pvngi2 == NULL)
        printf("SHoundDNAFromTaxIDList failed.\n");
     else{
        printf("SHoundDNAFromTaxIDList OK.\n");
        printf("The Dna Gis returned for the list of taxids were:\n");
        printf("DNA Gis:  ");
            head = pvngi2;
            while(head != NULL){
              printf(" %ld, ", head->data.intvalue);
              head = head->next;
            }
         
        printf("\nTaxids:  ");
            head = pvntaxid;
            while(head != NULL){
              printf(" %ld, ", head->data.intvalue);
              head = head->next;
            }
        printf("\n");
     }
     pvngi2 = ValNodeFree(pvngi2);

     printf("\n\nShoundGetSeqEntry------------------------------\n"); 
     pse = SHoundGetSeqEntry(gi_value);
     if (pse == NULL)
       printf("SHoundGetSeqEntry failed.\n");
     else{
       printf("SHoundGetSeqEntry OK.\n");
       printf("no way of printing out this C structure\n");
     }
     pse = SeqEntryFree(pse);

     printf("\n\nShoundGetSeqEntryList------------------------------\n"); 
     pvnbs = SHoundGetSeqEntryList(pvngi);
     if (pvnbs == NULL)
       printf("SHoundSeqEntryList failed.\n");
     else{
       printf("SHoundGetSeqEntryList OK\n");
       printf("No way of printing out this C structure\n");
     }
     pvnbs = FreeSeqEntryList(pvnbs);

     printf("\n\nShoundGetBioseqSet------------------------------\n"); 
     pbss = SHoundGetBioseqSet(2791836);
     if (pbss == NULL)
       printf("SHoundBioseqSet failed.\n");
     else{
       printf("SHoundGetBioseqSet OK.\n");
       printf("No way of printing this C data structure.\n");
     }
     pbss = BioseqSetFree(pbss);
   
     printf("\n\nShoundGetBioseqSetList------------------------------\n"); 
     pvnbs = SHoundGetBioseqSetList(pvngi4);
     if (pvnbs == NULL)
       printf("SHoundGetBioseqSetList failed.\n");
     else
       printf("SHoundGetBioseqSetList OK.\n");
     pvnbs = FreeBioseqSetList(pvnbs);

     printf("\n\nShoundGetGenBankff------------------------------\n"); 
     pfile = FileOpen("stdout", "w");
     if(!SHoundGetGenBankff(gi_value, pfile))
       printf("SHoundGetGenBankff failed.\n");
     else
       printf("SHoundGetGenBankff OK.\n");
     FileClose(pfile);

     printf("\n\nShoundGetGenBankffList------------------------------\n"); 
     pfile = FileOpen("stdout", "w");
     if(!SHoundGetGenBankffList(pvngi, pfile))
       printf("SHoundGetGenBankffList failed.\n");
     else
       printf("SHoundGetGenBankffList OK.\n");
     FileClose(pfile);

     printf("\n\nShoundGetXMLBioseq------------------------------\n"); 
     if(!SHoundGetXMLBioseq(gi_value, "stdout"))
       printf("SHoundGetXMLBioseq failed.\n");
     else
       printf("SHoundGetXMLBioseq OK.\n");

     printf("\n\nShoundGetXMLSeqEntry------------------------------\n"); 
     if(!SHoundGetXMLSeqEntry(gi_value, "stdout"))
       printf("SHoundGetXMLSeqEntry failed.\n");
     else
       printf("SHoundGetXMLSeqEntry OK.\n");


    }/* end of core functionality */

  /*If you want to see the MMdb results you must define that you
      want it to be included in the config file for shtest. */
    if ((module =  GetIntValue_fromCFG("parts", "strucdb")) == 1){
     printf("\n\nShound3DExists------------------------------\n");
     if(!SHound3DExists(myargs[2].intvalue))
       printf("SHound3DExists FALSE. Select a structure which exists.\n");
     else
     {
         printf("SHound3DExists OK.\n");

                 printf("\n\nShound3DbWhat------------------------------\n");
         if ((bwhat = SHound3DbWhat(myargs[2].intvalue)) > 0){
             printf("SHound3DbWhat OK.\n");
                         printf("The bWhat for mmdbid %ld is : %ld.\n", myargs[2].intvalue, bwhat);
                 }
         else
             printf("SHound3DbWhat failed.\n");

         printf("\n\nShoundGet3D------------------------------\n");
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


     printf("\n\nShoundGiFrom3D------------------------------\n");
     pvnstrgi = SHoundGiFrom3D(myargs[2].intvalue, FALSE);
     if (pvnstrgi == NULL)
       printf("SHoundGiFrom3D failed.\n");
     else{
       printf("SHoundGiFrom3D OK.\n");
           printf("The Gi returned for mmdbid: %ld was %s\n", myargs[2].intvalue, IsNullStr(pvnstrgi));
         }
     pvnstrgi = ValNodeFree(pvnstrgi);

     printf("\n\nShoundGiFrom3DList------------------------------\n"); 
     pvnstrgi = SHoundGiFrom3DList(pvnmmdbid, FALSE);
     if (pvnstrgi == NULL)
       printf("SHoundGiFrom3DList failed.\n");
     else
        printf("SHoundGiFrom3DList OK.\n");
     pvnstrgi = ValNodeFree(pvnstrgi);


     mmdbid = SHound3DFromGi(gi_value);
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

    pvntaxid = SHoundTaxIDFrom3D(myargs[2].intvalue);
     if(pvntaxid == NULL)
        printf("SHoundTaxIDFrom3D failed.\n");
     else
       printf("SHoundTaxIDFrom3D OK.\n");
     /*pvntaxid = ValNodeFree(pvntaxid);*/


     pvntaxid = SHoundTaxIDFrom3DList(pvnmmdbid);
     if(pvntaxid == NULL)
        printf("SHoundTaxIDFrom3DList failed.\n");
     else
       printf("SHoundTaxIDFrom3DList OK.\n");
    /* pvntaxid = ValNodeFree(pvntaxid);*/

    }/* end of structure section */

    /*If you want to see the complete genome results you must define that you
      want it to be included in the config file for shtest. */
      if ((module =  GetIntValue_fromCFG("parts", "gendb")) == 1){
        printf("\n\nShoundProteinsFromOrganism------------------------------\n"); 
        pvngia = SHoundProteinsFromOrganism(56636, CHROM_ALL);
        if (pvngia == NULL)
          printf("SHoundProteinsFromOrganism failed.\n");
        else{
         printf("SHoundProteinsFromOrganism OK.\n");
         printf("The Protein Gis returned for the organism Id %ld were:\n",taxid_value);
         printf("Protein GIs:  ");
         head = pvngia;
         while(head != NULL){
            printf(" %ld, ", head->data.intvalue);
            head = head->next;
         }
         printf("\n");
     }
     pvngia = ValNodeFree(pvngia);

    
    
     printf("\n\nShoundProteinsFromOrganismList------------------------------\n"); 
      pvngia = SHoundProteinsFromOrganismList(pvntaxid, CHROM_ALL);
     if (pvngia == NULL)
       printf("SHoundProteinsFromOrganismList failed.\n"); 
     else{ 
         printf("SHoundProteinsFromOrganismList OK.\n");
         printf("The Protein Gis returned for the list of Organism Ids were:\n");
         printf("Protein Gis:  ");
         head = pvngia;
         while(head != NULL){
            printf(" %ld, ", head->data.intvalue);
            head = head->next;
         }
         
         printf("\nOrganism Ids:  ");
         head = pvntaxid;
         while(head != NULL){
            printf(" %ld, ", head->data.intvalue);
            head = head->next;
         }
        printf("\n");
     }
      pvngia = ValNodeFree(pvngia);

     printf("\n\nShoundProteinsFromChromosome------------------------------\n"); 
     pvngia = SHoundProteinsFromChromosome(chromid_value);
     if (pvngia == NULL)
        printf("SHoundProteinsFromChromosome failed.\n");
     else{
        printf("SHoundProteinsFromChromosome OK.\n");
         printf("The Protein Gis returned for the list of Chromosome Id %ld were:\n", chromid_value);
         printf("Protein Gis:  ");
         head = pvngia;
         while(head != NULL){
            printf(" %ld, ", head->data.intvalue);
            head = head->next;
         }
        printf("\n");
     }
     pvngia = ValNodeFree(pvngia);


  
     printf("\n\nShoundProteinsFromChromosomeList------------------------------\n"); 
     pvngia = SHoundProteinsFromChromosomeList(pvnchromid);
     if (pvngia == NULL)
        printf("SHoundProteinsFromChromosomeList failed.\n");
     else{
        printf("SHoundProteinsFromChromosomeList OK.\n");
         printf("The Protein Gis returned for the list of Chromosome Ids were:\n");
         printf("Protein Gis:  ");
         head = pvngia;
         while(head != NULL){
            printf(" %ld, ", head->data.intvalue);
            head = head->next;
         }
         
         printf("\nChromosome Ids:  ");
         head = pvnchromid;
         while(head != NULL){
            printf(" %ld, ", head->data.intvalue);
            head = head->next;
         }
        printf("\n");
     }
     pvngia = ValNodeFree(pvngia);

     printf("\n\nShoundAllGenomes------------------------------\n"); 
     pvngen = SHoundAllGenomes();
     if(pvngen == NULL)
      printf("SHoundAllGenomes failed.\n");
     else{
       printf("SHoundAllGenomes OK.\n");
       printf("The Genomes returned were:\n");
         printf("Complete Genomes:  ");
         head = pvngen;
         while(head != NULL){
            printf(" %ld, ", head->data.intvalue);
            head = head->next;
         }
         printf("\n");
     }

     printf("\n\nShoundChromosomeFromGenome------------------------------\n"); 
     pvnchrom = SHoundChromosomeFromGenome(taxid_value, CHROM_ALL);
     if(pvnchrom == NULL)
        printf("SHoundChromosomeFromGenome failed.\n");
     else{
        printf("SHoundChromosomeFromGenome OK.\n");
       printf("The Chromosomes returned for Genome: %ld were:\n", taxid_value);
         printf("Chromosomes:  ");
         head = pvnchrom;
         while(head != NULL){
            printf(" %ld, ", head->data.intvalue);
            head = head->next;
         }
         printf("\n");
     }
     pvnchrom = ValNodeFree(pvnchrom);

     printf("\n\nShoundChromosomeFromGenomeList------------------------------\n"); 
     pvnchrom = SHoundChromosomeFromGenomeList(pvngen, CHROM_ALL);
     if(pvnchrom == NULL)
        printf("SHoundChromosomeFromGenomeList failed.\n");
     else{
        printf("SHoundChromosomeFromGenomeList OK.\n");
        printf("The Chromosomes returned for the list of Genome Ids were:\n");
         printf("Chromosome Ids:  ");
         head = pvnchrom;
         while(head != NULL){
            printf(" %ld, ", head->data.intvalue);
            head = head->next;
         }
         
         printf("\nGenome Ids:  ");
         head = pvngen;
         while(head != NULL){
            printf(" %ld, ", head->data.intvalue);
            head = head->next;
         }
        printf("\n");
     }
     pvnchrom = ValNodeFree(pvnchrom);

     printf("\n\nShoundDNAFromOrganism------------------------------\n"); 
     pvngin = SHoundDNAFromOrganism(taxid_value, CHROM_ALL);
     if (pvngin == NULL)
        printf("SHoundDNAFromOrganism failed.\n");
     else{
        printf("SHoundDNAFromOrganism OK.\n");
        printf("The DNA Gis returned for the organism Id %ld were:\n",taxid_value);
         printf("DNA Gis:  ");
         head = pvngin;
         while(head != NULL){
            printf(" %ld, ", head->data.intvalue);
            head = head->next;
         }
        printf("\n");
     }
     pvngin = ValNodeFree(pvngin);

    printf("\n\nShoundDNAFromOrganismList------------------------------\n"); 
     pvngin = SHoundDNAFromOrganismList(pvntaxid, CHROM_ALL);
     if (pvngin == NULL)
        printf("SHoundDNAFromOrganismList failed.\n");
     else{
        printf("SHoundDNAFromOrganismList OK.\n");
        printf("The DNA Gis returned for the list of organism Ids were:\n");
         printf("DNA gis:  ");
         head = pvngin;
         while(head != NULL){
            printf(" %ld, ", head->data.intvalue);
            head = head->next;
         }
         
         printf("\nOrganism Ids:  ");
         head = pvntaxid;
         while(head != NULL){
            printf(" %ld, ", head->data.intvalue);
            head = head->next;
         }
        printf("\n");
     }
     pvngin = ValNodeFree(pvngin);

    printf("\n\nShoundDNAFromChromosome------------------------------\n"); 
     pvngin = SHoundDNAFromChromosome(chromid_value);
     if (pvngin == NULL)
        printf("SHoundDNAFromChromosome failed.\n");
     else{
        printf("SHoundDNAFromChromosome OK.\n");
       printf("The DNA Gis returned for the chromosome: %ld were:\n", chromid_value);
         printf("DNA Gis:  ");
         head = pvngin;
         while(head != NULL){
            printf(" %ld, ", head->data.intvalue);
            head = head->next;
         }
         printf("\n");
     }
     pvngin = ValNodeFree(pvngin);


    printf("\n\nShoundDNAFromChromosomeList------------------------------\n"); 
     pvngin = SHoundDNAFromChromosomeList(pvnchromid);
     if (pvngin == NULL)
        printf("SHoundDNAFromChromosomeList failed.\n");
     else{
        printf("SHoundDNAFromChromosomeList OK.\n");
        printf("The DNA Gis returned for the list of chromosome Ids were:\n");
         printf("DNA gis:  ");
         head = pvngin;
         while(head != NULL){
            printf(" %ld, ", head->data.intvalue);
            head = head->next;
         }
         
         printf("\nChromosome Ids:  ");
         head = pvnchromid;
         while(head != NULL){
            printf(" %ld, ", head->data.intvalue);
            head = head->next;
         }
        printf("\n");
     }
     pvngin = ValNodeFree(pvngin);
    } /* end of complete genome section

    /*If you want to see the redund db results you must define that you
      want it to be included in the config file for shtest. */
    if ((module =  GetIntValue_fromCFG("parts", "redundb")) == 1){
     printf("\n\nShoundRedundantGroup------------------------------\n"); 
     pvngi2 = SHoundRedundantGroup(gi_value);
     if(pvngi2 == NULL)
        printf("SHoundRedundantGroup failed.\n");
     else{
        printf("SHoundRedundantGroup OK.\n");
        printf("The Redundant group returned for the GI:%ld were:\n", gi_value);
         printf("Redundant Group:  ");
         head = pvngi2;
         while(head != NULL){
            printf(" %ld, ", head->data.intvalue);
            head = head->next;
         }
         printf("\n");
     }
     pvngi2 = ValNodeFree(pvngi2);

     printf("\n\nShoundRedundantGroupList------------------------------\n"); 
     pvngi2 = SHoundRedundantGroupList(pvngi);
     if(pvngi2 == NULL)
       printf("SHoundRedundantGroupList failed.\n");
     else{
       printf("SHoundRedundantGroupList OK.\n");
       printf("The Redundant groups returned for the list of GIs were:\n");
         printf("Redundant groups:  ");
         head = pvngi2;
         while(head != NULL){
            printf(" %ld, ", head->data.intvalue);
            head = head->next;
         }
         
         printf("\nGis:  ");
         head = pvngi;
         while(head != NULL){
            printf(" %ld, ", head->data.intvalue);
            head = head->next;
         }
        printf("\n");
     }
     pvngi2 = ValNodeFree(pvngi2);
    } /* end of redund section */


    /*If you want to see the GO results you must define that you
      want it to be included in the config file for shtest. */
      if ((module =  GetIntValue_fromCFG("parts", "godb")) == 1){

    /*change the givalue to one that for sure has go references*/
    gi_value = GetIntValue_fromCFG("goid","gi1");

      printf("\n\nShoundGODBGetChildrenOf------------------------------\n"); 
         vnp = SHoundGODBGetChildrenOf(goid_value);
         if (vnp == NULL)
           printf("SHoundGODBGetChildrenOf failed.\n");
         else{
           printf("SHoundGODBGetChildrenOf OK.\n");
           printf("The GO ids for children of Goid:%ld were:\n", goid_value);
           printf("Children:  ");
           head = vnp;
           while(head != NULL){
             printf(" %ld, ", head->data.intvalue);
             head = head->next;
           }
           printf("\n");
         }
         vnp = ValNodeFree(vnp);
      
         printf("\n\nShoundGODBGetParentOf------------------------------\n"); 
         vnp = SHoundGODBGetParentOf(goid_value);
         if (vnp == NULL)
           printf("SHoundGODBGetParentOf failed.\n");
         else{
           printf("SHoundGODBGetParentOf OK.\n");
           printf("The GOids for parents of GOid:%ld were:\n", goid_value);
           printf("Parents:  ");
           head = vnp;
           while(head != NULL){
             printf(" %ld, ", head->data.intvalue);
             head = head->next;
           }
          printf("\n");
         }
         vnp = ValNodeFree(vnp);

         printf("\n\nShoundGetGODBGetNameByID------------------------------\n"); 
         pchar = SHoundGODBGetNameByID(goid_value);
         if(pchar == NULL)
          printf("SHoundGODBGetNameByID failed.\n");
         else{
           printf("SHoundGODBGetNameByID OK.\n");
           printf("The Name for Goid:%ld is %s\n",goid_value, IsNullStr(pchar)); 
         }
         pchar = MemFree(pchar);

         printf("\n\nShoundGODBGetAllAncestors------------------------------\n"); 
         vnp2 = SHoundGODBGetAllAncestors(goid_value);
         if (vnp2 == NULL)
           printf("SHoundGODBGetAllAncestors failed.\n");
         else{
           printf("SHoundGODBGetAllAncestors OK.\n");
           printf("The GOids for all ancestors of GOid:%ld were:\n", goid_value);
           printf("Ancestors:  ");
           head = vnp2;
           while(head != NULL){
             printf(" %ld, ", head->data.intvalue);
             head = head->next;
           }
          printf("\n");
         }
         vnp2 = ValNodeFree(vnp2);

         printf("\n\nShoundGetGODBGetAllChildren------------------------------\n"); 
         vnp2 = SHoundGODBGetAllChildren(goid_value);
         if (vnp2 == NULL)
           printf("SHoundGODBGetAllChildren failed.\n");
         else{
           printf("SHoundGODBGetAllChildren OK.\n");
           printf("The GOids for all children of GOid:%ld were:\n", goid_value);
           printf("All decsendants:  ");
           head = vnp2;
           while(head != NULL){
             printf(" %ld, ", head->data.intvalue);
             head = head->next;
           }
          printf("\n");
         }
         vnp2 = ValNodeFree(vnp2);

          printf("\n\nShoundGetRecordByReference------------------------------\n"); 
         vnp2 = SHoundGODBGetRecordByReference("EC");
         if (vnp2 == NULL)
           printf("SHoundGODBGetRecordByReference failed.\n");
         else{
           printf("SHoundGODBGetRecordByReference OK.\n");
           printf("The GOids by reference: EC were:\n");
           printf("GOIds:  ");
           head = vnp2;
           while(head != NULL){
             printf(" %ld, ", head->data.intvalue);
             head = head->next;
           }
          printf("\n");
         }
         vnp2 = ValNodeFree(vnp2);

           printf("\n\nShoundGetRecordByReference------------------------------\n"); 
         vnp2 = SHoundGODBGetRecordByReference("ISBN");
         if (vnp2 == NULL)
           printf("SHoundGODBGetRecordByReference failed.\n");
         else{
           printf("SHoundGODBGetRecordByReference OK.\n");
           printf("The GOids by reference: ISBN were:\n");
           printf("GOIds:  ");
           head = vnp2;
           while(head != NULL){
             printf(" %ld, ", head->data.intvalue);
             head = head->next;
           }
          printf("\n");
         }
         vnp2 = ValNodeFree(vnp2);

         
         printf("\n\nShoundGetClassification------------------------------\n");
         pi = SHoundGODBGetClassification(goid_value);
         if(pi == NULL)
          printf("SHoundGODBGetClassification failed.\n");
         else{
           printf("SHoundGODBGetClassification OK.\n");
           printf("The Classificaion for GOID : %ld is %s\n", goid_value, IsNullStr(pi));
         }

      } /* end of godb section */

      /*If you want to see the Locus Link  results you must define that you
      want it to be included in the config file for shtest. */
      if ((module =  GetIntValue_fromCFG("parts", "lldb")) == 1){
         
          /*change the givalue to one that for sure has go references*/
         gi_value = GetIntValue_fromCFG("goid","gi1");

         printf("\n\nShoundGiFromGOID------------------------------\n"); 
         vnpi = SHoundGiFromGOID(goid_value);
         if(vnpi == NULL)
            printf("SHoundGiFromGOID failed.\n");
         else{
           printf("SHoundGiFromGOID OK.\n");
           printf("The GIs from the GOid:%ld  were:\n",goid_value);
           printf("GIs:  ");
           head = vnpi;
           while(head != NULL){
             printf(" %ld, ", head->data.intvalue);
             head = head->next;
           }
          printf("\n");
         }
         vnpi = ValNodeFree(vnpi);

         printf("\n\nShoundGiFromGOIDList------------------------------\n"); 
         vnpi = SHoundGiFromGOIDList(goList);
         if(vnpi == NULL)
            printf("SHoundGiFromGOIDList failed.\n");
         else{
           printf("SHoundGiFromGOIDList OK.\n");
           printf("The Gis returned for the list of goids were:\n");
           printf("Gis:  ");
           head = vnpi;
           while(head != NULL){
            printf(" %ld, ", head->data.intvalue);
            head = head->next;
           }
         
           printf("\nGoids:  ");
           head = goList;
           while(head != NULL){
            printf(" %ld, ", head->data.intvalue);
            head = head->next;
          }
          printf("\n");
         }
         vnpi = ValNodeFree(vnpi);

         printf("\n\nShoundGOIDfromGI------------------------------\n"); 
         vnp2 = SHoundGOIDFromGi(gi_value);
         if (vnp2 == NULL)
           printf("SHoundGOIDFromGi failed.\n");
         else{
           printf("SHoundGOIDFromGi OK.\n");
           printf("The Goids from the Gi:%ld  were:\n",gi_value);
           printf("Goids:  ");
           head = vnp2;
           while(head != NULL){
             printf(" %ld, ", head->data.intvalue);
             head = head->next;
           }
          printf("\n");
         }
         vnp2 = ValNodeFree(vnp2);

         ValNodeAddInt(&giList, 0, GetIntValue_fromCFG("goid","gi1"));
         ValNodeAddInt(&giList, 0, GetIntValue_fromCFG("goid","gi2"));
         printf("\n\nShoundGOIDfromGIList------------------------------\n"); 
         vnpk = SHoundGOIDFromGiList(giList);
         if (vnpk == NULL)
           printf("SHoundGOIDFromGiList failed.\n");
         else{
           printf("SHoundGOIDFromGiList OK.\n");
           printf("The Goids returned for the list of gis were:\n");
           printf("Goids:  ");
           head = vnpk;
           while(head != NULL){
            printf(" %ld, ", head->data.intvalue);
            head = head->next;
           }
         
           printf("\nGis:  ");
           head = giList;
           while(head != NULL){
            printf(" %ld, ", head->data.intvalue);
            head = head->next;
          }
          printf("\n");
         }
         vnpk = ValNodeFree(vnpk);

         printf("\n\nShoundGOECodefromGiAndGOID-----------------------\n"); 
         vnp = SHoundGOECodeFromGiAndGOID(gi_value, goid_value);
         if (vnp == NULL)
           printf("SHoundGOECodeFromGiAndGOID failed.\n");
         else{
           printf("SHoundGOECodeFromGiAndGOID OK.\n");
           printf("The GOECode returned for the gi: %ld were:\n", gi_value);
           printf("GOECode:  ");
           head = vnp;
           while(head != NULL){
            printf(" %ld, ", head->data.intvalue);
            head = head->next;
           }
          printf("\n");
         }
         vnp = ValNodeFreeData(vnp);

         printf("\n\nShoundGOPMIDfromGiAndGOID------------------------------\n"); 
         vnp = SHoundGOPMIDFromGiAndGOID(gi_value, goid_value);
         if (vnp == NULL)
           printf("SHoundGOPMIDFromGiAndGOID failed.\n");
         else{
           printf("SHoundGOPMIDFromGiAndGOID OK.\n");
           printf("The GOPMID returned for the gi: %ld were:\n", gi_value);
           printf("GOPMID:  ");
           head = vnp;
           while(head != NULL){
            printf(" %ld, ", head->data.intvalue);
            head = head->next;
           }
          printf("\n");
         }
         vnp = ValNodeFree(vnp);

         printf("\n\nShoundGiFromOMIM------------------------------\n"); 
	 /* SK: NULL ptr check */
	 if ( omimList !- NULL )
	 {
            vnp = SHoundGiFromOMIM(omimList->data.intvalue);
            if (vnp == NULL)
        	printf("SHoundGiFromOMIM failed.\n");
            else{
        	printf("SHoundGiFromOMIM OK.\n");
        	printf("The GI returned for the omimid: %ld were:\n", omimList->data.intvalue);
        	printf("GI:  ");
        	head = vnp;
        	while(head != NULL){
        	    printf(" %ld, ", head->data.intvalue);
        	    head = head->next;
        	}
        	printf("\n");
            }
            vnp = ValNodeFree(vnp);
	 }

         printf("\n\nShoundGifromOMIMList------------------------------\n"); 
         vnp = SHoundGiFromOMIMList(omimList);
         if (vnp == NULL)
           printf("SHoundGiFromOMIMList failed.\n");
         else{
           printf("SHoundGiFromOMIMList OK.\n");
           printf("The GIs returned for the list of omim ids were:\n");
           printf("GIs:  ");
           head = vnp;
           while(head != NULL){
            printf(" %ld, ", head->data.intvalue);
            head = head->next;
           }

           printf("\nOmim Ids:  ");
           head = omimList;
           while(head != NULL){
            printf(" %ld, ", head->data.intvalue);
            head = head->next;
          }
          printf("\n");
         }
         vnp = ValNodeFree(vnp);

        printf("\n\nShoundOMIMfromGI------------------------------\n"); 
         vnp = SHoundOMIMFromGi(gi_value);
         if (vnp == NULL)
           printf("SHoundOMIMFromGi failed.\n");
         else{
           printf("SHoundOMIMFromGi OK.\n");
           printf("The Omim Id returned for the gi: %ld were:\n", gi_value);
           printf("Omim Id:  ");
           head = vnp;
           while(head != NULL){
            printf(" %ld, ", head->data.intvalue);
            head = head->next;
           }
          printf("\n");
         }
         vnp = ValNodeFree(vnp);

         printf("\n\nShoundOMIMfromGIList------------------------------\n"); 
         vnp = SHoundOMIMFromGiList(giList);
         if (vnp == NULL)
           printf("SHoundOMIMFromGiList failed.\n");
         else{
           printf("SHoundOMIMFromGiList OK.\n");
           printf("The Omim idss returned for the list of gis were:\n");
           printf("Omim Ids:  ");
           head = vnp;
           while(head != NULL){
            printf(" %ld, ", head->data.intvalue);
            head = head->next;
           }
         
           printf("\nGis:  ");
           head = giList;
           while(head != NULL){
            printf(" %ld, ", head->data.intvalue);
            head = head->next;
          }
          printf("\n");
         }
         vnp = ValNodeFree(vnp);

         printf("\n\nShoundGifromLLID------------------------------\n"); 
	 /* SK: NULL ptr check */
	 if ( lllist != NULL )
	 {
            vnp = SHoundGiFromLLID(lllist->data.intvalue);
            if (vnp == NULL)
        	printf("SHoundGiFromLLID failed.\n");
            else{
        	printf("SHoundGiFromLLID OK.\n");
        	printf("The GI returned for the LLid:%ld were:\n", lllist->data.intvalue);
        	printf("GI:  ");
        	head = vnp;
        	while(head != NULL){
        	    printf(" %ld, ", head->data.intvalue);
        	    head = head->next;
        	}
        	printf("\n");
            }
            vnp = ValNodeFree(vnp);
	 }
    
        printf("\n\nShoundGifromLLIDList------------------------------\n"); 
         vnp = SHoundGiFromLLIDList(lllist);
         if (vnp == NULL)
           printf("SHoundGiFromLLIDList failed.\n");
         else{
           printf("SHoundGiFromLLIDList OK.\n");
           printf("The Gis returned for the list of LL ids were:\n");
           printf("Gis:  ");
           head = vnp;
           while(head != NULL){
            printf(" %ld, ", head->data.intvalue);
            head = head->next;
           }
         
           printf("\nLocus Link ids:  ");
           head = lllist;
           while(head != NULL){
            printf(" %ld, ", head->data.intvalue);
            head = head->next;
          }
          printf("\n");
         }
         vnp = ValNodeFree(vnp);
         
         printf("\n\nShoundLLIDfromGi------------------------------\n"); 
         i = 0;
         i = SHoundLLIDFromGi(gi_value);
         if(i == 0)
           printf("SHoundLLIDFromGi failed.\n");
         else{
           printf("SHoundLLIDFromGi OK.\n");
           printf("The Locus Link Id returned for the gi: %ld was: %ld\n", gi_value, i);
	 }

         printf("\n\nShoundLLIDFromGiList------------------------------\n"); 
         vnp = SHoundLLIDFromGiList(giList);
         if (vnp == NULL)
           printf("SHoundLLIDFromGiList failed.\n");
         else{
           printf("SHoundLLIDFromGiList OK.\n");
           printf("The LL ids returned for the list of gis were:\n");
           printf("LL ids:  ");
           head = vnp;
           while(head != NULL){
            printf(" %ld, ", head->data.intvalue);
            head = head->next;
           }
         
           printf("\nGis:  ");
           head = giList;
           while(head != NULL){
            printf(" %ld, ", head->data.intvalue);
            head = head->next;
          }
          printf("\n");
         }
         vnp = ValNodeFree(vnp);

         printf("\n\nShoundLocusFromGi------------------------------\n"); 
         pchar = SHoundLocusFromGi(gi_value);
         if(pchar == NULL)
           printf("SHoundLocusFromGi failed.\n");
         else{
           printf("SHoundLocusFromGi OK.\n");
           printf("The Locus returned for the gi: %ld was: %s\n", gi_value, IsNullStr(pchar));
         }
         pchar = MemFree(pchar);
         
         printf("\n\nShoundGifromCDDID------------------------------\n"); 
         vnp = SHoundGiFromCDDID("EPEND");
         if (vnp == NULL)
           printf("SHoundGiFromCDDID failed.\n");
         else{
           printf("SHoundGiFromCDDID OK.\n");
           printf("The GI list returned for the the CDDID: EPEND  were:\n");
           printf("GIS:  ");
           head = vnp;
           while(head != NULL){
            printf(" %ld, ", head->data.intvalue);
            head = head->next;
           }
           printf("\n");
	 }
         vnp = ValNodeFree(vnp);

         vnp = SHoundGiFromCDDIDList(cddidlist);
         if (vnp == NULL)
           printf("SHoundGiFromCDDIDList failed.\n");
         else{
           printf("SHoundGiFromCDDIDList OK.\n");
           printf("The Gis returned for the list of CDD ids were:\n");
           printf("Gis:  ");
           head = vnp;
           while(head != NULL){
            printf(" %ld, ", head->data.intvalue);
            head = head->next;
           }
         
           printf("\nCDD ids:  ");
           head = cddidlist;
           while(head != NULL){
            printf(" %ld, ", head->data.intvalue);
            head = head->next;
          }
          printf("\n");
         }
         vnp = ValNodeFree(vnp);

        printf("\n\nShoundCDDIDfromGi------------------------------\n"); 
         vnp = SHoundCDDIDFromGi(4557225);
         if (vnp == NULL)
           printf("SHoundCDDIDFromGi failed.\n");
         else
           printf("SHoundCDDIDFromGi OK.\n");
         vnp = ValNodeFreeData(vnp);

         vnp = SHoundCDDIDFromGiList(giList);
         if (vnp == NULL)
           printf("SHoundCDDIDFromGiList failed.\n");
         else{
           printf("SHoundCDDIDFromGiList OK.\n");
           printf("The CDD ids returned for the list of gis were:\n");
           printf("CDD ids:  ");
           head = vnp;
           while(head != NULL){
            printf(" %ld, ", head->data.intvalue);
            head = head->next;
           }
         
           printf("\nGis:  ");
           head = giList;
           while(head != NULL){
            printf(" %ld, ", head->data.intvalue);
            head = head->next;
          }
          printf("\n");
         }
         vnp = ValNodeFreeData(vnp);
     } /* end of locus link section */

     CloseMMDBAPI();
     printf("MMDBAPI closed.\n");


     SHoundFini();
     printf("SHoundFini OK.\n");

     pvnacc = ValNodeFreeData(pvnacc);
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
  $Log: shtest_detail.c,v $
  Revision 1.3  2004/01/06 22:32:14  skuznets
  Code audit has been done sucessfully.
  Possible NULL pointer issues has been eliminated.

  Revision 1.2  2003/12/02 16:47:06  vgu
  change api name from SHoundGOGetEcodeFromGi to SHoundGOGetEcodeFromGiAndGOID
  same modification to SHoundGOPMIDFromGi

  Revision 1.1.1.2  2002/05/16 08:27:50  root
  Automated import from Sourceforge.net

  Revision 1.2  2002/05/15 17:12:01  risserlin
  fixed errors in redund section

  Revision 1.1  2002/05/14 18:51:16  risserlin
  modified shtest so that it prints out results based on modules that you want to test

 */

