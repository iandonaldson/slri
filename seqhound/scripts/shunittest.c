/* $Id: shunittest.c,v 1.35 2005/04/14 18:23:34 vgu Exp $
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

  December 1, 2003
                Victor Gu. The Blueprint Initiative, Samuel Lunenfeld Research Institute,
                Mount Sinai Hospital. 
                Added reporting functionality to log the input and
                output values of SeqHound APIs. A summary log file reports if an API has been
                passed or failed while a report log will list the input/output values. The 
                test driver with the new functionality is
                named shunittest.c.

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

#define LOGING "shunittestlog"
#define NUMARGS 7

/************* Parameters for Testing Invalid Input *************/
#define MAXINT 3000000000
#define EMPTYSTR ""
#define MINUSONE -1
/************ Utility Function for printing results to log file ***/
void logWrite (FILE *fp, char *str1, char *str2, char *str3, char *str4, char *str5);

Args myargs[NUMARGS] = {
        {"GI","230779",NULL,NULL,FALSE,'g',ARG_INT,0.0,0,NULL},
        {"Accession","X88853",NULL,NULL,TRUE,'a',ARG_STRING,0.0,0,NULL},
        {"MMDBID","11",NULL,NULL,TRUE,'m',ARG_INT,0.0,0,NULL},
        {"Taxonomy ID","224915",NULL,NULL,TRUE,'t',ARG_INT,0.0,0,NULL},
        {"Chromosomal ID","200",NULL,NULL,TRUE,'c',ARG_INT,0.0,0,NULL},
        {"Redundant ID","610108",NULL,NULL,TRUE,'c',ARG_INT,0.0,0,NULL},
        {"GO ID","8320",NULL,NULL,TRUE,'c',ARG_INT,0.0,0,NULL},
	};


Int2 Main()
{
   BioseqPtr pbs = NULL;
   Int4 gi = 0, taxid =0, mmdbid = 0, gin = 0, bwhat = 0, i = 0, returnInt = 0, inTaxid=56636;
   Int4 precomTaxid = 3055, NumNeighbours=0;
   CharPtr pacc = NULL, ptitle = NULL, pchar = NULL, pi = NULL, cddid = NULL, server = NULL, pname=NULL;
   CharPtr pdbcode=NULL, chain=NULL;
   CharPtr file_name = "pdb3D";
   FloatHi score = 0;
   ValNodePtr pvngi = NULL;
   ValNodePtr pvngi3 = NULL;
   ValNodePtr pvngi4 = NULL;
   ValNodePtr pvngi5 = NULL;
   ValNodePtr pvnacc = NULL;
   ValNodePtr pvntitle = NULL;
   ValNodePtr pvngi2 = NULL;
   SeqIdPtr psi = NULL;
   ValNodePtr pvnseqid = NULL;
   ValNodePtr pvnbs = NULL;
   SLRIFastaPtr pfa = NULL;
   ValNodePtr pvnfasta = NULL;
   BiostrucPtr pbio = NULL;
   PDNMS pdnmsModelstruc=NULL;
   ValNodePtr pvnmmdbid = NULL;
   ValNodePtr pvnstrgi = NULL;
   ValNodePtr pvnmmdbid2 = NULL;
   ValNodePtr pvngin = NULL;
   ValNodePtr pvngia = NULL;
   ValNodePtr pvnmuid = NULL;
   ValNodePtr pvntaxid = NULL;
   ValNodePtr pvnchromid = NULL;
   ValNodePtr pvngen = NULL;
   ValNodePtr pvnchrom = NULL;
   ValNodePtr pvnredund = NULL, pvnInvalRedund = NULL;
   SeqEntryPtr pse = NULL;
   BioseqSetPtr pbss = NULL;
   FILE* pfile = NULL;
   time_t tim1,tim2;
   ValNodePtr vnp2 = NULL, vnp = NULL;
   ValNodePtr goList = NULL, omimList = NULL;
   ValNodePtr vnpi = NULL, giList = NULL, vnpk = NULL, lllist = NULL;
   ValNodePtr cddidlist = NULL;
   ValNodePtr pvnret = NULL;
   /********************** Pointers to Additional Data Structures mostly SLRI specific **/
   AsnIoPtr aip2 = NULL;
   BiostrucPtr pbiostruc = NULL;
   PDNMS pdnms=NULL;
   ILinkSetPtr p3DList = NULL, ilsp = NULL;
   ILinkSetListPtr ilslp = NULL;
   FLinkSetPtr flsp = NULL;
   FLinkSetListPtr flslp = NULL;
   FLinkSetList2Ptr flsl2p = NULL;
   CLinkSetPtr clsp = NULL;
   NBlastResultSetPtr nbrsp = NULL;
   SeqAlignPtr seqAlignPtr = NULL;
   StRpsdbPtr srpsp = NULL;

   /*************** Variables Specific to Regression and Reporting Log ************/
   ValNodePtr nullValNodePtr = NULL, knownPvnTaxidList = NULL;
   ValNodePtr valNodeRefList = NULL, knownGoList = NULL;
   CharPtr nullString = NULL; 
   Int4 intValue = 0, intValue2 =0, ret = SLRI_FAIL, testFlag = 0;
   Uint2 taxClass = 11, typeInt = 12;
   char itos1[25], itos2[25], strMinusone[5] = "-1";
   /**************** End of Variables Specific to Regression Testing and Reporting Log ****/

/********* Set up the log files ******************/
   	FILE *reportLog, *summaryLog; 
	char logName[25];
	char summaryName[25];
	char *prefix = "shreport";
	char *summaryPrefix = "summary";
	int timeStringSize = 13;
	char timeString[timeStringSize];
	time_t *tp = NULL, time_now = 0;
	struct tm *timeStrucPtr;

	/* Get current time from system for stamping log file */ 
	time_now = time(tp);
	timeStrucPtr = localtime(&time_now);
	strftime(timeString, timeStringSize, "%m%d%y%H%M%S", timeStrucPtr);
	/* Set up report log file */
	strcpy(logName, prefix);
	strcat(logName, timeString);
	reportLog = fopen(logName, "w");
	if (reportLog == NULL)
	{
	  printf(" ERROR opening report log file\n");
	  return 1;
	}
	logWrite(reportLog, "API_NAME", "INPUT_TYPE", "INPUT_VALUE", "OUTPUT_TYPE", "OUTPUT_VALUE");
	logWrite(reportLog, "________", "___________", "__________", "___________", "____________");
	logWrite(reportLog, "        ", "           ", "          ", "           ", "            ");
      fflush(reportLog);
	/* Set up summary log file */
	strcpy (summaryName, summaryPrefix);
	strcat (summaryName, timeString);
	summaryLog = fopen(summaryName, "w");
	if (summaryLog == NULL)
	{
	  printf(" ERROR opening report log file\n");
	  fclose(reportLog);
	  return 1;
	}
       	logWrite(summaryLog, "API_NAME", "TEST_RESULT", "", "", "");
       	logWrite(summaryLog, "________", "___________", "", "", "");
       	logWrite(summaryLog, "        ", "           ", "          ", "           ", "            ");
      fflush(summaryLog);

/*************** End setting up report log *****/
   tim1=GetSecs();
   if (!GetArgs("shunittest",NUMARGS, myargs))
   {
	printf("No command line arguments provided. Check variable myargs in the test driver\n");
	fclose(reportLog);
	fclose(summaryLog);
	return 1;
   }
   /* setting up error log flags */ 
   ErrSetOptFlags(EO_LOG_SEVERITY | EO_LOG_CODES | EO_LOG_FILELINE | EO_LOG_USRFILE);
   ErrSetLogfile(LOGING, ELOG_BANNER);
   ErrSetLogLevel(SEV_INFO);
   ErrSetMessageLevel(SEV_MAX);
   if (OpenMMDBAPI(0,NULL))
   {
       printf("OpenMMDBAPI OK.\n");
       	 logWrite(summaryLog, "openMMDBAPI", "OK", "", "", "");
   }
    else
    {
       printf("OpenMMDBAPI failed.\n");
       	 logWrite(summaryLog, "openMMDBAPI", "FAIL", "", "", "");
	 fclose(reportLog);
	 fclose(summaryLog);
       return 2;
    }

    if(SHoundInitReadOnly(FALSE, "shunittest"))
    {
      printf("SHoundInit OK.\n");
       	logWrite(summaryLog, "SHoundInit", "OK", "", "", "");
    }
    else
    {
      printf("SHoundInit failed.\n");
       	logWrite(summaryLog, "SHoundInit", "FAIL", "", "", "");
    }


    if(SHoundIsInited())
    {
      printf("SHoundIsInited OK.\n");
       	logWrite(summaryLog, "SHoundIsInited", "OK", "", "", "");
    }
    else
    {
      printf("SHoundIsInited failed.\n");
       	logWrite(summaryLog, "SHoundIsInited", "FAIL", "", "", "");
	fclose(reportLog);
	fclose(summaryLog);
      return 1;
    }
    
    server = SHoundGetCurrentServer();
    printf ("Server is %s\n", server);
    if(server)
    {
        printf("SHoundGetCurrentServer OK.\n");
       	logWrite(summaryLog, "SHoundGetCurrentServer", "OK", "", "", "");
	MemFree(server);
    }
	
    else
    {
      printf("SHoundGetCurrentServer failed.\n");
       	logWrite(summaryLog, "SHoundGetCurrentServer", "FAIL", "", "", "");
	fclose(reportLog);
	fclose(summaryLog);
      return 1;
    }
    
    
#if 0

    if(SHoundNetEntrezInit("shunittest"))
    {
       printf("SHoundNetEntrezInit OK.\n");
       	 logWrite(summaryLog, "SHoundNetEntrezInit", "OK", "", "", "");
    }
    else
    {
       printf("SHoundNetEntrezInit failed.\n");
       	 logWrite(summaryLog, "SHoundNetEntrezInit", "FAIL", "", "", "");
    }


    if(SHoundIsNetEntrezOn())
    {
       printf("SHoundIsNetEntrezOn OK.\n");
       	 logWrite(summaryLog, "SHoundIsNetEntrezOn", "OK", "", "", "");
    }
    else
    {
       printf("SHoundIsNetEntrezOn failed.\n");
       	 logWrite(summaryLog, "SHoundIsNetEntrezOn", "FAIL", "", "", "");
    }
#endif
    sprintf(itos1, "%ld", myargs[0].intvalue);
    /* assuming the GI from command line is a valid one */
    if(SHoundExists(myargs[0].intvalue))
    {
      printf("SHoundExists OK.\n");
      logWrite(reportLog, "SHoundExists",  "GI", itos1, "BOOLEAN", "TRUE");
      testFlag = 1;
    }
    else
    {
      printf("SHoundExist failed. Select a GI which exists.\n");
      logWrite(reportLog, "SHoundExists",  "GI", itos1, "BOOLEAN", "FALSE");
      testFlag = 0;
      /* Do not let the program return as we need to test it against invalid parameters */ 
      /* return 1; */ 
    }
    if(SHoundExists(MAXINT))
    {
      printf("SHoundExists OK.\n");
      logWrite(reportLog, "SHoundExists",  "GI", "MAXINT", "BOOLEAN", "TRUE");
      testFlag = 0;
    }
    else
    {
      printf("SHoundExist failed. Select a GI which exists.\n");
      /* Do not let the program return as we need to test it against invalid parameters */ 
      /* return 1; */ 
    }
    if(SHoundExists(MINUSONE))
    {
      printf("SHoundExists OK.\n");
      logWrite(reportLog, "SHoundExists",  "GI", "-1", "BOOLEAN", "TRUE");
      testFlag = 0;
    }
    else
    {
      printf("SHoundExist failed. Select a GI which exists.\n");
      logWrite(reportLog, "SHoundExists",  "GI", "-1", "BOOLEAN", "FALSE");
      /* Do not let the program return as we need to test it against invalid parameters */ 
      /* return 1; */ 
    }
     if ( testFlag = 1)
     {
           logWrite(summaryLog, "SHoundExists", "PASS", "", "", "");
     }
     else
     {
           logWrite(summaryLog, "SHoundExists", "FAIL", "", "", "");
     }

    /*** Valid Accesion number as input ***/
    gi = 0;
    gi = SHoundFindAcc(myargs[1].strvalue);
    sprintf(itos1, "%ld", gi);
    logWrite(reportLog, "SHoundFindAcc",  "Accession", myargs[1].strvalue, "GI", itos1);
    if(gi > 0)
    {
       printf("SHoundFindAcc OK.\n");
	 testFlag = 1;
    }
    else
    {
       printf("SHoundFindAcc failed.\n");
	 testFlag = 0;
    }

    /* Invalid Accesion number as input */
    gi = 0;
    gi = SHoundFindAcc(EMPTYSTR);
    sprintf(itos1, "%ld", gi);
    logWrite(reportLog, "SHoundFindAcc",  "Accession", "EMPTYSTRING", "GI", itos1);
    if(gi > 0)
    {
	 testFlag = 0;
    }
    gi = 0;
    gi = SHoundFindAcc(nullString);
    if(gi > 0)
    {
	 testFlag = 0;
    }
    sprintf(itos1, "%ld", gi);
    logWrite(reportLog, "SHoundFindAcc",  "Accession", "NULLSTRING", "GI", itos1);
     if ( testFlag = 1)
     {
           logWrite(summaryLog, "SHoundFindAcc", "PASS", "", "", "");

     }
     else
     {
           logWrite(summaryLog, "SHoundFindAcc", "FAIL", "", "", "");

     }

    ValNodeAddStr(&pvnacc, 0, StrSave(myargs[1].strvalue));
    ValNodeAddStr(&pvnacc, 0, StrSave(myargs[1].strvalue));
    ValNodeAddStr(&pvnacc, 0, StrSave(myargs[1].strvalue));
    /* valid value node call */
    pvngi = SHoundFindAccList(pvnacc);
    if (pvngi == NULL)
    {
      logWrite(reportLog, "SHoundFindAccList", "ValNodePtr", "NOTNULL", "ValNodePtr", "NULL");
      printf("SHoundFindAccList failed.\n");
	testFlag = 0;
    }
    else
    {
      printf("SHoundFindAccList OK.\n");
	testFlag = 1;
	logWrite(reportLog, "SHoundFindAccList", "ValNodePtr", "NOTNULL", "ValNodePtr", "NOTNULL");
    }
    pvngi = ValNodeFree(pvngi);
    pvnacc = ValNodeFreeData(pvnacc);
    /***** Invalid value node input ****/
    pvngi = NULL; 
    pvngi = SHoundFindAccList(nullValNodePtr);
    if (pvngi == NULL)
    {
        logWrite(reportLog, "SHoundFindAccList", "ValNodePtr", "NULL", "ValNodePtr", "NULL");
        printf("SHoundFindAccList failed.\n");
    }
    else
    {
      printf("SHoundFindAccList OK.\n");
	logWrite(reportLog, "SHoundFindAccList", "ValNodePtr", "NOTNULL", "ValNodePtr", "NOTNULL");
      pvngi = ValNodeFree(pvngi); 
	testFlag = 0;
    }
     if ( testFlag = 1)
     {
           logWrite(summaryLog, "SHoundFindAccList", "PASS", "", "", "");

     }
     else
     {
           logWrite(summaryLog, "SHoundFindAccList", "FAIL", "", "", "");

     }

    /* Valid Call */
    pacc = SHoundAccFromGi(4389899);
    if(pacc == NULL)
    {
       sprintf(itos1, "%ld", 4389899);
       logWrite(reportLog, "SHoundAccFromGi",  "GI", itos1, "ValNodePtr", "NULL");
       printf("SHoundAccFromGi failed.\n");
	 testFlag = 0;
    }
    else
    {
       logWrite(reportLog, "SHoundAccFromGi",  "GI", itos1, "ValNodePtr", pacc);
	 testFlag = 1;
       printf("SHoundAccFromGi OK.\n");
       MemFree (pacc);
    }
    /**** Invalid GI input ****/
    pacc = SHoundAccFromGi(MAXINT);
    if(pacc == NULL)
    {
       logWrite(reportLog, "SHoundAccFromGi",  "GI", "MAXINT", "ValNodePtr", "NULL");
       printf("SHoundAccFromGi failed.\n");
    }
    else
    {
       logWrite(reportLog, "SHoundAccFromGi",  "GI", "MAXINT", "ValNodePtr", "NOTNULL");
       printf("SHoundAccFromGi OK.\n");
	 testFlag = 0;
       MemFree(pacc);
    }
    /**** GI = -1 as input ****/
    pacc = NULL;
    pacc = SHoundAccFromGi(MINUSONE);
    sprintf(itos1, "%ld", MINUSONE);
    if(pacc == NULL)
    {
       logWrite(reportLog, "SHoundAccFromGi",  "GI", itos1, "ValNodePtr", "NULL");
       printf("SHoundAccFromGi failed.\n");
    }
    else
    {
       logWrite(reportLog, "SHoundAccFromGi",  "GI", itos1, "ValNodePtr", "NOTNULL");
       printf("SHoundAccFromGi OK.\n");
	 testFlag = 0;
       MemFree(pacc);
    }
     if ( testFlag = 1)
     {
           logWrite(summaryLog, "SHoundAccFromGi", "PASS", "", "", "");

     }
     else
     {
           logWrite(summaryLog, "SHoundAccFromGi", "FAIL", "", "", "");

     }

    /* Valid Call */
    ptitle = SHoundTitleFromGi(myargs[0].intvalue);
    if(ptitle == NULL)
    {
       sprintf(itos1, "%ld", myargs[0].intvalue);
       logWrite(reportLog, "SHoundTitleFromGi",  "GI", itos1, "ValNodePtr", "NULL");
       printf("SHoundTitleFromGi failed.\n");
	 testFlag = 0;
    }
    else
    {
       logWrite(reportLog, "SHoundTitleFromGi",  "GI", itos1, "ValNodePtr", "NOTNULL");
	 testFlag = 1;
       printf("SHoundTitleFromGi OK.\n");
       MemFree (ptitle);
    }
    /**** Invalid GI input ****/
    ptitle = SHoundTitleFromGi(MAXINT);
    if(ptitle == NULL)
    {
       logWrite(reportLog, "SHoundTitleFromGi",  "GI", "MAXINT", "ValNodePtr", "NULL");
       printf("SHoundTitleFromGi failed.\n");
    }
    else
    {
       logWrite(reportLog, "SHoundTitleFromGi",  "GI", "MAXINT", "ValNodePtr", "NOTNULL");
       printf("SHoundTitleFromGi OK.\n");
	 testFlag = 0;
       MemFree(ptitle);
    }
    /**** GI = -1 as input ****/
    ptitle = NULL;
    ptitle = SHoundTitleFromGi(MINUSONE);
    sprintf(itos1, "%ld", MINUSONE);
    if(ptitle == NULL)
    {
       logWrite(reportLog, "SHoundTitleFromGi",  "GI", itos1, "ValNodePtr", "NULL");
       printf("SHoundTitleFromGi failed.\n");
    }
    else
    {
       logWrite(reportLog, "SHoundTitleFromGi",  "GI", itos1, "ValNodePtr", "NOTNULL");
       printf("SHoundTitleFromGi OK.\n");
	 testFlag = 0;
       MemFree(ptitle);
    }
     if ( testFlag = 1)
     {
           logWrite(summaryLog, "SHoundTitleFromGi", "PASS", "", "", "");

     }
     else
     {
           logWrite(summaryLog, "SHoundTitleFromGi", "FAIL", "", "", "");

     }

    /* valid call using Accesion number */
    gi= 0;
    /* Commented out as the number X88853 does not have a matching name in the current Seqhound database */
    /*gi = SHoundFindName("X88853"); */
    gi = SHoundFindName("AF060234");  
    sprintf(itos1, "%ld", gi);
    logWrite(reportLog, "SHoundFindName",  "SEQIDENTIFIER", "AF060234", "GI", itos1);
    if(gi > 0)
    {
	 testFlag = 1;
       printf("SHoundFindName OK.\n");
    }
    else
    {
       printf("SHoundFindName failed.\n");
	 testFlag = 0;
    }
    /* Invalid call using empty string */ 
    gi= 0;
    gi = SHoundFindName(EMPTYSTR);
    if(gi > 0)
    {
	 testFlag = 0;
    }
    sprintf(itos1, "%ld", gi);
    logWrite(reportLog, "SHoundFindName",  "SEQIDENTIFIER", "EMPTYSTRING", "GI", itos1);
    /* Invalid call using Null String */
    gi= 0;
    gi = SHoundFindName(nullString);
    if(gi > 0)
    {
	 testFlag = 0;
    }
    sprintf(itos1, "%ld", gi);
    logWrite(reportLog, "SHoundFindName",  "SEQIDENTIFIER", "NULLSTRING", "GI", itos1);
     if ( testFlag = 1)
     {
           logWrite(summaryLog, "SHoundFindName", "PASS", "", "", "");

     }
     else
     {
           logWrite(summaryLog, "SHoundFindName", "FAIL", "", "", "");

     }


    ValNodeAddStr(&pvnacc, 0, StrSave("X88853"));
    ValNodeAddStr(&pvnacc, 0, StrSave("ECJ224437"));
    ValNodeAddStr(&pvnacc, 0, StrSave("AF165032"));
    pvngi = SHoundFindNameList(pvnacc);
    
    if (pvngi == NULL)
    {
	logWrite(reportLog, "SHoundFindNameList",  "ValNodePtr", "NOTNULL", "Pointer", "NULL");
      printf("SHoundFindNameList failed.\n");
      testFlag = 0;
    }
    else
    {
	logWrite(reportLog, "SHoundFindNameList",  "ValNodePtr", "NOTNULL", "Pointer", "NOTNULL");
      printf("SHoundFindNameList OK.\n");
      testFlag = 1;
    }
    pvngi = ValNodeFree(pvngi);
    pvnacc = ValNodeFreeData(pvnacc);
    /* Make calls using null valunodePtr*/ 
    pvngi = NULL;
    pvngi = SHoundFindNameList(nullValNodePtr);
    if (pvngi == NULL)
    {
       logWrite(reportLog, "SHoundFindNameList",  "ValNodePtr", "NULL", "Pointer", "NULL");
    }
    else
    {
       logWrite(reportLog, "SHoundFindNameList",  "ValNodePtr", "NULL", "Pointer", "NOTNULL");
      testFlag = 0;
    }
     if ( testFlag = 1)
     {
           logWrite(summaryLog, "SHoundFindNameList", "PASS", "", "", "");

     }
     else
     {
           logWrite(summaryLog, "SHoundFindNameList", "FAIL", "", "", "");

     }

    ValNodeAddInt(&pvngi, 0, 4389899);
    ValNodeAddInt(&pvngi, 0, 4389899);
    ValNodeAddInt(&pvngi, 0, 4389899);
    pvnacc = SHoundAccFromGiList(pvngi);
    if(pvnacc == NULL)
    {
       logWrite(reportLog, "SHoundAccFromGiList",  "ValNodePtr", "NOTNULL", "Pointer", "NULL");
       printf("SHoundAccFromGiList failed.\n");
       testFlag = 0;
    }
    else
    {
       printf("SHoundAccFromGiList OK.\n");
       logWrite(reportLog, "SHoundAccFromGiList",  "ValNodePtr", "NOTNULL", "ValNodePtr", pvnacc->data.ptrvalue);
       testFlag = 1;
		
    }
    pvnacc = ValNodeFreeData(pvnacc);
    /*** Testing call with invalid null value node pointer */
    pvnacc = SHoundAccFromGiList(nullValNodePtr);
    if(pvnacc == NULL)
    {
       logWrite(reportLog, "SHoundAccFromGiList",  "ValNodePtr", "NULL", "Pointer", "NULL");
       printf("SHoundAccFromGiList failed.\n");
    }
    else
    {
       printf("SHoundAccFromGiList OK.\n");
       logWrite(reportLog, "SHoundAccFromGiList",  "ValNodePtr", "NULL", "Pointer", "NOTNULL");
       testFlag = 0;
    }
     if ( testFlag = 1)
     {
           logWrite(summaryLog, "SHoundAccFromGiList", "PASS", "", "", "");

     }
     else
     {
           logWrite(summaryLog, "SHoundAccFromGiList", "FAIL", "", "", "");

     }


    ValNodeAddInt(&pvngi, 0, myargs[0].intvalue);
    ValNodeAddInt(&pvngi, 0, myargs[0].intvalue);
    ValNodeAddInt(&pvngi, 0, myargs[0].intvalue);
    pvntitle = SHoundTitleFromGiList(pvngi);
    if(pvntitle == NULL)
    {
       logWrite(reportLog, "SHoundTitleFromGiList",  "ValNodePtr", "NOTNULL", "Pointer", "NULL");
       printf("SHoundTitleFromGiList failed.\n");
       testFlag = 0;
    }
    else
    {
       printf("SHoundTitleFromGiList OK.\n");
       logWrite(reportLog, "SHoundTitleFromGiList",  "ValNodePtr", "NOTNULL", "Pointer", "NOTNULL");
       testFlag = 1;
		
    }
    pvntitle = ValNodeFreeData(pvntitle);
    /*** Testing call with invalid null value node pointer */
    pvntitle = SHoundTitleFromGiList(nullValNodePtr);
    if(pvntitle == NULL)
    {
       logWrite(reportLog, "SHoundTitleFromGiList",  "ValNodePtr", "NULL", "Pointer", "NULL");
       printf("SHoundTitleFromGiList failed.\n");
    }
    else
    {
       printf("SHoundTitleFromGiList OK.\n");
       logWrite(reportLog, "SHoundTitleFromGiList",  "ValNodePtr", "NULL", "Pointer", "NOTNULL");
       testFlag = 0;
    }


    /* Valid Param */
    psi = SHoundSeqIdFromGi(myargs[0].intvalue);
    sprintf(itos1, "%ld", myargs[0].intvalue);
    if(psi == NULL)
    {
       printf("SHoundSeqIdFromGi failed.\n");
       logWrite(reportLog, "SHoundSeqIdFromGi",  "GI", itos1, "Pointer", "NULL");
       testFlag = 0;
    }
    else
    {
       printf("SHoundSeqIdFromGi OK.\n");
       logWrite(reportLog, "SHoundSeqIdFromGi",  "GI", itos1, "Pointer", "NOTNULL");
       testFlag = 1;
    }
    psi = FreeSeqId(psi);
    /* Call using MAX Integer */
    psi = SHoundSeqIdFromGi(MAXINT);
    if(psi == NULL)
    {
       printf("SHoundSeqIdFromGi failed.\n");
       logWrite(reportLog, "SHoundSeqIdFromGi",  "GI", "MAXINT", "Pointer", "NULL");
    }
    else
    {
       printf("SHoundSeqIdFromGi OK.\n");
       testFlag = 0;
       logWrite(reportLog, "SHoundSeqIdFromGi",  "GI", "MAXINT", "Pointer", "NOTNULL");
    }
    psi = FreeSeqId(psi);
    /* GI=-1 */
    psi = SHoundSeqIdFromGi(MINUSONE);
    sprintf(itos1, "%ld", MINUSONE);
    if(psi == NULL)
    {
       printf("SHoundSeqIdFromGi failed.\n");
       logWrite(reportLog, "SHoundSeqIdFromGi",  "GI", itos1, "Pointer", "NULL");
    }
    else
    {
       printf("SHoundSeqIdFromGi OK.\n");
       testFlag = 0;
       logWrite(reportLog, "SHoundSeqIdFromGi",  "GI", itos1, "Pointer", "NOTNULL");
    }
     if ( testFlag = 1)
     {
           logWrite(summaryLog, "SHoundSeqIdFromGi", "PASS", "", "", "");

     }
     else
     {
           logWrite(summaryLog, "SHoundSeqIdFromGi", "FAIL", "", "", "");

     }
    psi = FreeSeqId(psi);

    /* Valid Param */
    pvnseqid = SHoundSeqIdFromGiList(pvngi);
    if(pvnseqid == NULL)
    {
       printf("SHoundSeqIdFromGiList failed.\n");
       testFlag = 0;
       logWrite(reportLog, "SHoundSeqIdFromGiList",  "ValNodePtr", "NOTNULL", "Pointer", "NULL");
    }
    else
    {
       printf("SHoundSeqIdFromGiList OK.\n");
       testFlag = 1;
       logWrite(reportLog, "SHoundSeqIdFromGiList",  "ValNodePtr", "NOTNULL", "Pointer", "NOTNULL");
    }
    FreeVNSeqIdList(pvnseqid);
    /* Invalid Param */
    pvnseqid = SHoundSeqIdFromGiList(nullValNodePtr);
    if(pvnseqid == NULL)
    {
       printf("SHoundSeqIdFromGiList failed.\n");
       logWrite(reportLog, "SHoundSeqIdFromGiList",  "ValNodePtr", "NULL", "Pointer", "NULL");
    }
    else
    {
       printf("SHoundSeqIdFromGiList OK.\n");
       testFlag = 0;
       logWrite(reportLog, "SHoundSeqIdFromGiList",  "ValNodePtr", "NULL", "Pointer", "NOTNULL");
    }
    FreeVNSeqIdList(pvnseqid);
     if ( testFlag = 1)
     {
           logWrite(summaryLog, "SHoundSeqIdFromGiList", "PASS", "", "", "");

     }
     else
     {
           logWrite(summaryLog, "SHoundSeqIdFromGiList", "FAIL", "", "", "");

     }

    /* Valid Param */
    sprintf(itos1, "%ld", myargs[0].intvalue);
    if(SHoundIsProtein(myargs[0].intvalue))
    {
      logWrite(reportLog, "SHoundIsProtein",  "GI", itos1, "BOOLEAN", "TRUE");
      printf("SHoundIsProtein TRUE.\n");
      testFlag = 1;
    }
    else
    {
      printf("SHoundIsProtein FALSE.\n"); 
      logWrite(reportLog, "SHoundIsProtein",  "GI", itos1, "BOOLEAN", "FALSE");
      testFlag = 0;
    }
    /* GI = MAXINT */
    if(SHoundIsProtein(MAXINT))
    {
      logWrite(reportLog, "SHoundIsProtein",  "GI", "MAXINT", "BOOLEAN", "TRUE");
      printf("SHoundIsProtein TRUE.\n");
      testFlag = 0;
    }
    else
    {
      printf("SHoundIsProtein FALSE.\n"); 
      logWrite(reportLog, "SHoundIsProtein",  "GI", "MAXINT", "BOOLEAN", "FALSE");
    }
    /* GI = -1 */
    sprintf(itos1, "%ld", MINUSONE);
    if(SHoundIsProtein(MINUSONE))           
    {
      logWrite(reportLog, "SHoundIsProtein",  "GI", itos1, "BOOLEAN", "TRUE");
      printf("SHoundIsProtein TRUE.\n");
      testFlag = 0;
    }
    else
    {
      printf("SHoundIsProtein FALSE.\n"); 
      logWrite(reportLog, "SHoundIsProtein",  "GI", itos1, "BOOLEAN", "FALSE");
    }

    /* Valid call GI = 4557225 */
    gi = 4557225;
    sprintf(itos1, "%ld", gi);
    if(SHoundIsNRFirst(gi))
    {
      printf("SHoundIsNRFirst TRUE.\n");
      testFlag = 1;
      logWrite(reportLog, "SHoundIsNRFirst",  "GI", itos1, "BOOLEAN", "TRUE");
    }
    else
    {
      printf("SHoundIsNRFirst FALSE.\n");
      testFlag = 0;
      logWrite(reportLog, "SHoundIsNRFirst",  "GI", itos1, "BOOLEAN", "FALSE");
    }
    /* Invalid param GI = MAX INT */
    if(SHoundIsNRFirst(MAXINT))
    {
      printf("SHoundIsNRFirst TRUE.\n");
      testFlag = 0;
      logWrite(reportLog, "SHoundIsNRFirst",  "GI", "MAXINT", "BOOLEAN", "TRUE");
    }
    else
    {
      printf("SHoundIsNRFirst FALSE.\n");
      logWrite(reportLog, "SHoundIsNRFirst",  "GI", "MAXINT", "BOOLEAN", "FALSE");
    }
    /* GI = -1 */
    sprintf(itos1, "%ld", MINUSONE);
    if(SHoundIsNRFirst(MINUSONE))
    {
      printf("SHoundIsNRFirst TRUE.\n");
      testFlag = 0;
      logWrite(reportLog, "SHoundIsNRFirst",  "GI", itos1, "BOOLEAN", "TRUE");
    }
    else
    {
      printf("SHoundIsNRFirst FALSE.\n");
      logWrite(reportLog, "SHoundIsNRFirst",  "GI", itos1, "BOOLEAN", "FALSE");
    }
     if ( testFlag = 1)
     {
           logWrite(summaryLog, "SHoundIsNRFirst", "PASS", "", "", "");

     }
     else
     {
           logWrite(summaryLog, "SHoundIsNRFirst", "FAIL", "", "", "");

     }

    /* Valid Param */
    sprintf(itos1, "%ld", myargs[0].intvalue);
    pbs = SHoundGetBioseq(myargs[0].intvalue);
    if (pbs == NULL)
    {
      printf("SHoundGetBioseq failed.\n");
      testFlag = 0;
      logWrite(reportLog, "SHoundGetBioSeq",  "GI", itos1, "Pointer", "NULL");
    }
    else
    {
      printf("SHoundGetBioseq OK.\n");
      testFlag = 1;
      logWrite(reportLog, "SHoundGetBioSeq",  "GI", itos1, "Pointer", "NOTNULL");
    }
    pbs = BioseqFree(pbs);

    /* GI = MAXINT */
    pbs = SHoundGetBioseq(MAXINT);
    if (pbs == NULL)
    {
      printf("SHoundGetBioseq failed.\n");
      logWrite(reportLog, "SHoundGetBioSeq",  "GI", "MAXINT", "Pointer", "NULL");
    }
    else
    {
      printf("SHoundGetBioseq OK.\n");
      testFlag = 0;
      logWrite(reportLog, "SHoundGetBioSeq",  "GI", "MAXINT", "Pointer", "NOTNULL");
    }
    pbs = BioseqFree(pbs);

    /* GI = -1 */
    sprintf(itos1, "%ld", myargs[0].intvalue);
    pbs = SHoundGetBioseq(MINUSONE);
    if (pbs == NULL)
    {
      printf("SHoundGetBioseq failed.\n");
      logWrite(reportLog, "SHoundGetBioSeq",  "GI", itos1, "Pointer", "NULL");
    }
    else
    {
      printf("SHoundGetBioseq OK.\n");
      testFlag = 0;
      logWrite(reportLog, "SHoundGetBioSeq",  "GI", itos1, "Pointer", "NOTNULL");
    }
     if ( testFlag = 1)
     {
           logWrite(summaryLog, "SHoundGetBioSeq", "PASS", "", "", "");

     }
     else
     {
           logWrite(summaryLog, "SHoundGetBioSeq", "FAIL", "", "", "");

     }
    pbs = BioseqFree(pbs);
    
    /* Valid Param */
    pvnbs = SHoundGetBioseqList(pvngi);
    if (pvnbs == NULL)
    {
      printf("SHoundGetBioseqList failed.\n");
      testFlag = 0;
      logWrite(reportLog, "SHoundGetBioseqList",  "ValNodePtr", "NOTNULL", "Pointer", "NULL");
    }
    else
    {
      printf("SHoundGetBioseqList OK.\n");
      testFlag = 1;
      logWrite(reportLog, "SHoundGetBioseqList",  "ValNodePtr", "NOTNULL", "Pointer", "NOTNULL");
    }
    pvnbs = FreeBioseqList(pvnbs);
   
    /* Invalid Param */ 
    pvnbs = SHoundGetBioseqList(nullValNodePtr);
    if (pvnbs == NULL)
    {
      printf("SHoundGetBioseqList failed.\n");
      logWrite(reportLog, "SHoundGetBioseqList",  "ValNodePtr", "NOTNULL", "Pointer", "NULL");
    }
    else
    {
      printf("SHoundGetBioseqList OK.\n");
      testFlag = 0;
      logWrite(reportLog, "SHoundGetBioseqList",  "ValNodePtr", "NOTNULL", "Pointer", "NOTNULL");
    }
     if ( testFlag = 1)
     {
           logWrite(summaryLog, "SHoundGetBioSeqList", "PASS", "", "", "");

     }
     else
     {
           logWrite(summaryLog, "SHoundGetBioSeqList", "FAIL", "", "", "");

     }
    pvnbs = FreeBioseqList(pvnbs);

    /* Valid Param */
    sprintf(itos1, "%ld", myargs[2].intvalue);
    if(!SHound3DExists(myargs[2].intvalue))
    {
      printf("SHound3DExists FALSE. Select a structure which exists.\n");
      logWrite(reportLog, "SHound3DExists",  "MMDBID", itos1, "BOOLEAN", "FALSE");
      testFlag = 0;
      /*return 1;  commented out for testing purpose*/
    }
    else
    {
	logWrite(reportLog, "SHound3DExists",  "MMDBID", itos1, "BOOLEAN", "TRUE");
      testFlag = 1;
    }
    /* MMDB ID = MAXINT */
    if(!SHound3DExists(MAXINT))
    {
      printf("SHound3DExists FALSE. Select a structure which exists.\n");
      logWrite(reportLog, "SHound3DExists",  "MMDBID", "MAXINT", "BOOLEAN", "FALSE");
      /*return 1;  commented out for testing purpose*/
    }
    else
    {
	logWrite(reportLog, "SHound3DExists",  "MMDBID", "MAXINT", "BOOLEAN", "TRUE");
      testFlag = 0;
    }
    /* MMDB ID = -1 */
    sprintf(itos1, "%ld", MINUSONE);
    if(!SHound3DExists(MINUSONE))
    {
      printf("SHound3DExists FALSE. Select a structure which exists.\n");
      logWrite(reportLog, "SHound3DExists",  "MMDBID", itos1, "BOOLEAN", "FALSE");
      /*return 1;  commented out for testing purpose*/
    }
    else
    {
	logWrite(reportLog, "SHound3DExists",  "MMDBID", itos1, "BOOLEAN", "TRUE");
      testFlag = 0;
    }
     if ( testFlag = 1)
     {
           logWrite(summaryLog, "SHound3DExists", "PASS", "", "", "");

     }
     else
     {
           logWrite(summaryLog, "SHound3DExists", "FAIL", "", "", "");

     }
    /*  The else clause is commented out for testing purpose. It is more appropriate to use if here */
    /*  else */
    if(SHound3DExists(myargs[2].intvalue))
    {
      printf("SHound3DExists OK.\n");
	sprintf(itos1, "%ld", myargs[2].intvalue);
      bwhat = SHound3DbWhat(myargs[2].intvalue);
	sprintf(itos2, "%ld", bwhat);
	logWrite(reportLog, "SHound3DbWhat", "MMDBID", itos1, "MOLECULEID", itos2 );
      if (bwhat > 0)
      {
          printf("SHound3DbWhat OK.\n");
          testFlag = 1;
      }
      else
      {
          printf("SHound3DbWhat failed.\n");
          testFlag = 0;
      }
      /* Invalid param MMDBID = MAXINT */
      bwhat = 0;
      bwhat = SHound3DbWhat(MAXINT);
      if (bwhat > 0)
      {
          testFlag = 0;
      }
	sprintf(itos2, "%ld", bwhat);
	logWrite(reportLog, "SHound3DbWhat", "MMDBID", "MAXINT", "MOLECULEID", itos2);

      /* Invalid param MMDBID = MINUSONE */
      bwhat = 0;
      bwhat = SHound3DbWhat(MINUSONE);
      if (bwhat > 0)
      {
          testFlag = 0;
      }
	sprintf(itos2, "%ld", bwhat);
	sprintf(itos1, "%ld", MINUSONE);
	logWrite(reportLog, "SHound3DbWhat", "MMDBID", itos1, "MOLECULEID", itos2);
     if ( testFlag = 1)
     {
             logWrite(summaryLog, "SHound3DbWhat", "PASS", "", "", "");

     }
     else
     {
             logWrite(summaryLog, "SHound3DbWhat", "FAIL", "", "", "");

     }

        /*---------------------
	  SHoundGet3D
	  ---------------------*/
      pbio = SHoundGet3D(myargs[2].intvalue);
	sprintf(itos1, "%ld", myargs[2].intvalue);
      if (pbio == NULL)
      {
	   logWrite(reportLog, "SHoundGet3D",  "MMDBID", itos1, "Pointer", "NULL");
         printf("SHoundGet3D failed.\n");
          testFlag = 0;
      }
      else
      {
          printf("SHoundGet3D OK.\n");
          testFlag = 1;
	    logWrite(reportLog, "SHoundGet3D",  "MMDBID", itos1, "Pointer", "NOTNULL");
            /********* code below commented out as the call ClearStructures () causes dcore dump ****/
            /*
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
            */
      }
      /* Invalid param */
      if(pbio != NULL) BiostrucFree(pbio);
      pbio = SHoundGet3D(MAXINT);
      if (pbio == NULL)
      {
	   logWrite(reportLog, "SHoundGet3D",  "MMDBID", "MAXINT", "Pointer", "NULL");
         printf("SHoundGet3D failed.\n");
      }
      else
      {
	   logWrite(reportLog, "SHoundGet3D",  "MMDBID", "MAXINT", "Pointer", "NOTNULL");
         testFlag = 0;
      }
      /* MMDBID = -1 */
      if(pbio != NULL) BiostrucFree(pbio);
      pbio = SHoundGet3D(MINUSONE);
	sprintf(itos1, "%ld", MINUSONE);
      if (pbio == NULL)
      {
	   logWrite(reportLog, "SHoundGet3D",  "MMDBID", itos1, "Pointer", "NULL");
         printf("SHoundGet3D failed.\n");
      }
      else
      {
	   logWrite(reportLog, "SHoundGet3D",  "MMDBID", itos1, "Pointer", "NOTNULL");
         testFlag = 0;
      }
     if ( testFlag = 1)
     {
             logWrite(summaryLog, "SHoundGet3D", "PASS", "", "", "");

     }
     else
     {
             logWrite(summaryLog, "SHoundGet3D", "FAIL", "", "", "");

     }
    }
    pfa = SHoundGetFasta(myargs[0].intvalue);
    sprintf(itos1, "%ld", myargs[0].intvalue);
    if (pfa == NULL)
    {
      printf("SHoundGetFasta failed.\n");     
      testFlag = 0;
      logWrite(reportLog, "SHoundGetFasta",  "GI", itos1, "Pointer", "NULL");
    }
    else
    {
      printf("SHoundGetFasta OK.\n"); 
      testFlag = 1;
      logWrite(reportLog, "SHoundGetFasta",  "GI", itos1, "Pointer", "NOTNULL");
    }
    /* GI = MAXINT */    
    if (pfa != NULL) free (pfa);
    pfa = SHoundGetFasta(MAXINT);
    if (pfa == NULL)
    {
      printf("SHoundGetFasta failed.\n");
      logWrite(reportLog, "SHoundGetFasta",  "GI", "MAXINT", "Pointer", "NULL");
    }
    else
    {
      printf("SHoundGetFasta OK.\n");
      testFlag = 0;
      logWrite(reportLog, "SHoundGetFasta",  "GI", "MAXINT", "Pointer", "NOTNULL");
    }
    /* GI = -1 */
    if (pfa != NULL) free (pfa);
    pfa = SHoundGetFasta(MINUSONE);
    sprintf(itos1, "%ld", MINUSONE);
    if (pfa == NULL)
    {
      printf("SHoundGetFasta failed.\n");
      logWrite(reportLog, "SHoundGetFasta",  "GI", itos1, "Pointer", "NULL");
    }
    else
    {
      printf("SHoundGetFasta OK.\n");
      testFlag = 0;
      logWrite(reportLog, "SHoundGetFasta",  "GI", itos1, "Pointer", "NOTNULL");
    }
     if ( testFlag = 1)
     {
           logWrite(summaryLog, "SHoundGetFasta", "PASS", "", "", "");

     }
     else
     {
           logWrite(summaryLog, "SHoundGetFasta", "FAIL", "", "", "");

     }

    pvnfasta = SHoundGetFastaList(pvngi);
    if (pvnfasta == NULL)
    {
      printf("SHoundGetFastaList failed.\n");
      testFlag = 0;
      logWrite(reportLog, "SHoundGetFastaList", "ValNodePtr", "NOTNULL", "Pointer","NULL");
    }
    else
    {
      printf("SHoundGetFastaList OK.\n");
      testFlag = 1;
      logWrite(reportLog, "SHoundGetFastaList", "ValNodePtr", "NOTNULL", "Pointer","NOTNULL");
    }
    pvnfasta = FreeFastaList(pvnfasta);

    /* Invalid Param */
    pvnfasta = SHoundGetFastaList(nullValNodePtr);
    if (pvnfasta == NULL)
    {
      printf("SHoundGetFastaList failed.\n");
      logWrite(reportLog, "SHoundGetFastaList", "ValNodePtr", "NULL", "Pointer","NULL");
    }
    else
    {
      printf("SHoundGetFastaList OK.\n");
      testFlag = 0;
      logWrite(reportLog, "SHoundGetFastaList", "ValNodePtr", "NULL", "Pointer","NOTNULL");
    }
     if ( testFlag = 1)
     {
           logWrite(summaryLog, "SHoundGetFastaList", "PASS", "", "", "");

     }
     else
     {
           logWrite(summaryLog, "SHoundGetFastaList", "FAIL", "", "", "");

     }
    pvnfasta = FreeFastaList(pvnfasta);


    pvnstrgi = SHoundGiFrom3D(myargs[2].intvalue, FALSE);
    sprintf(itos1, "%ld/%s", myargs[2].intvalue,"FALSE");
    if (pvnstrgi == NULL)
    {
      printf("SHoundGiFrom3D failed.\n");
      testFlag = 0;
      logWrite(reportLog, "SHoundGiFrom3D",  "GI/BOOLEAN", itos1, "Pointer", "NULL");
    }
    else
    {
      printf("SHoundGiFrom3D OK.\n");
      testFlag = 1;
      logWrite(reportLog, "SHoundGiFrom3D",  "GI/BOOLEAN", itos1, "Pointer", "NOTNULL");
    }
    pvnstrgi = ValNodeFree(pvnstrgi);

    /* Invalid GI = MAXINT */
    pvnstrgi = SHoundGiFrom3D(MAXINT, FALSE);
    sprintf(itos1, "%s", "MAXINT/FALSE");
    if (pvnstrgi == NULL)
    {
      printf("SHoundGiFrom3D failed.\n");
      logWrite(reportLog, "SHoundGiFrom3D",  "GI/BOOLEAN", itos1, "Pointer", "NULL");
    }
    else
    {
      printf("SHoundGiFrom3D OK.\n");
      testFlag = 0;
      logWrite(reportLog, "SHoundGiFrom3D",  "GI/BOOLEAN", itos1, "Pointer", "NOTNULL");
    }
    pvnstrgi = ValNodeFree(pvnstrgi);

    /* GI = -1 */
    sprintf(itos1, "%s", "MINUSONE/FALSE");
    pvnstrgi = SHoundGiFrom3D(MINUSONE, FALSE);
    if (pvnstrgi == NULL)
    {
      printf("SHoundGiFrom3D failed.\n");
      logWrite(reportLog, "SHoundGiFrom3D",  "GI/BOOLEAN", itos1, "Pointer", "NULL");
    }
    else
    {
      printf("SHoundGiFrom3D OK.\n");
      testFlag = 0;
      logWrite(reportLog, "SHoundGiFrom3D",  "GI/BOOLEAN", itos1, "Pointer", "NOTNULL");
    }
     if ( testFlag = 1)
     {
           logWrite(summaryLog, "SHoundGiFrom3D", "PASS", "", "", "");

     }
     else
     {
           logWrite(summaryLog, "SHoundGiFrom3D", "FAIL", "", "", "");

     }
    pvnstrgi = ValNodeFree(pvnstrgi);


    ValNodeAddInt(&pvnmmdbid, 0, myargs[2].intvalue);
    ValNodeAddInt(&pvnmmdbid, 0, myargs[2].intvalue);
    ValNodeAddInt(&pvnmmdbid, 0, myargs[2].intvalue);
    pvnstrgi = SHoundGiFrom3DList(pvnmmdbid, FALSE);
    if (pvnstrgi == NULL)
    {
      printf("SHoundGiFrom3DList failed.\n");
      testFlag = 0;
      logWrite(reportLog, "SHoundGiFrom3DList",  "ValNodePtr/BOOLEAN", "NOTNULL/FALSE", "Pointer", "NULL");
    }
    else
    {
       printf("SHoundGiFrom3DList OK.\n");
      testFlag = 1;
       logWrite(reportLog, "SHoundGiFrom3DList",  "ValNodePtr/BOOLEAN", "NOTNULL/FAKSE", "Pointer", "NOTNULL");
    }
    pvnstrgi = ValNodeFree(pvnstrgi);

    /* Invalid Param Null pointer */ 
    pvnstrgi = SHoundGiFrom3DList(nullValNodePtr, FALSE);
    if (pvnstrgi == NULL)
    {
      printf("SHoundGiFrom3DList failed.\n");
      logWrite(reportLog, "SHoundGiFrom3DList",  "ValNodePtr/BOOLEAN", "NULL/FALSE", "Pointer", "NULL");
    }
    else
    {
       printf("SHoundGiFrom3DList OK.\n");
       testFlag = 0;
       logWrite(reportLog, "SHoundGiFrom3DList",  "ValNodePtr/BOOLEAN", "NULL/FALSE", "Pointer", "NOTNULL");
    }
     if ( testFlag = 1)
     {
           logWrite(summaryLog, "SHoundGiFrom3DList", "PASS", "", "", "");

     }
     else
     {
           logWrite(summaryLog, "SHoundGiFrom3DList", "FAIL", "", "", "");

     }
    pvnstrgi = ValNodeFree(pvnstrgi);


    mmdbid = SHound3DFromGi(myargs[0].intvalue);
    sprintf(itos1, "%ld", myargs[0].intvalue );
    sprintf(itos2, "%ld", mmdbid);
    logWrite(reportLog, "SHound3DFromGi",  "GI", itos1, "MMDBID", itos2);
    if(mmdbid > 0)
    {
      printf("SHound3DFromGi OK.\n");
      testFlag = 1;
    }
    else
    {
      printf("SHound3DFromGi failed.\n");
       testFlag = 0;
    }

    /* GI = MAXINT */
    mmdbid = 0;
    mmdbid = SHound3DFromGi(MAXINT);
    if(mmdbid > 0)
    {
      testFlag = 0;
    }
    sprintf(itos2, "%ld", mmdbid);
    logWrite(reportLog, "SHound3DFromGi",  "GI", "MAXINT", "MMDBID", itos2);

    /* GI = -1 */
    mmdbid = 0;
    mmdbid = SHound3DFromGi(MINUSONE);
    if(mmdbid > 0)
    {
      testFlag = 0;
    }
    sprintf(itos1, "%ld", MINUSONE);
    sprintf(itos2, "%ld", mmdbid);
    logWrite(reportLog, "SHound3DFromGi",  "GI", itos1, "MMDBID", itos2);
     if ( testFlag = 1)
     {
           logWrite(summaryLog, "SHound3DFromGi", "PASS", "", "", "");

     }
     else
     {
           logWrite(summaryLog, "SHound3DFromGi", "FAIL", "", "", "");

     }


    pvnmmdbid2 = SHound3DFromGiList(pvngi);
    if(pvnmmdbid2 == NULL)
    {
       printf("SHound3DFromGiList failed.\n");
       testFlag = 0;
       logWrite(reportLog, "SHound3DFromGiList", "ValNodePtr", "NOTNULL", "Pointer","NULL");
    }
    else
    {
       printf("SHound3DFromGiList OK.\n");
       testFlag = 1;
       logWrite(reportLog, "SHound3DFromGiList", "ValNodePtr", "NOTNULL", "Pointer","NOTNULL");
    }
    pvnmmdbid2 = ValNodeFree(pvnmmdbid2);

    /* Invalid Param */
    pvnmmdbid2 = SHound3DFromGiList(nullValNodePtr);
    if(pvnmmdbid2 == NULL)
    {
       printf("SHound3DFromGiList failed.\n");
       logWrite(reportLog, "SHound3DFromGiList", "ValNodePtr", "NULL", "Pointer","NULL");
    }
    else
    {
       printf("SHound3DFromGiList OK.\n");
       testFlag = 0;
       logWrite(reportLog, "SHound3DFromGiList", "ValNodePtr", "NULL", "Pointer","NOTNULL");
    }
     if ( testFlag = 1)
     {
           logWrite(summaryLog, "SHound3DFromGiList", "PASS", "", "", "");

     }
     else
     {
           logWrite(summaryLog, "SHound3DFromGiList", "FAIL", "", "", "");

     }
    pvnmmdbid2 = ValNodeFree(pvnmmdbid2);


    ValNodeAddInt(&pvngi3, 0, 2463028);
    ValNodeAddInt(&pvngi3, 0, 2463028);
    ValNodeAddInt(&pvngi3, 0, 2463028);

    intValue = 2463028;
    sprintf(itos1, "%ld", intValue);
    pvngia = SHoundProteinFromDNA(intValue);
    if(pvngia == NULL)
    {
       printf("SHoundProteinFromDNA failed.\n");
       testFlag = 0;
       logWrite(reportLog, "SHoundProteinFromDNA", "GI", itos1, "Pointer", "NULL");
    }
    else
    {
       printf("SHoundProteinFromDNA OK.\n");
       testFlag = 1;
       logWrite(reportLog, "SHoundProteinFromDNA", "GI", itos1, "Pointer", "NOTNULL");
    }
    pvngia = ValNodeFree(pvngia);
    
    /* Invalid param GI = MAXINT */
    pvngia = SHoundProteinFromDNA(MAXINT);
    if(pvngia == NULL)
    {
       printf("SHoundProteinFromDNA failed.\n");
       logWrite(reportLog, "SHoundProteinFromDNA", "GI", "MAXINT", "Pointer", "NULL");
    }
    else
    {
       printf("SHoundProteinFromDNA OK.\n");
       testFlag = 0;
       logWrite(reportLog, "SHoundProteinFromDNA", "GI", "MAXINT", "Pointer", "NOTNULL");
    }
    pvngia = ValNodeFree(pvngia);

    pvngia = SHoundProteinFromDNA(MINUSONE);
    sprintf(itos1, "%ld", MINUSONE);
    if(pvngia == NULL)
    {
       printf("SHoundProteinFromDNA failed.\n");
       logWrite(reportLog, "SHoundProteinFromDNA", "GI", itos1, "Pointer", "NULL");
    }
    else
    {
       printf("SHoundProteinFromDNA OK.\n");
       testFlag = 0;
       logWrite(reportLog, "SHoundProteinFromDNA", "GI", itos1, "Pointer", "NOTNULL");
    }
     if ( testFlag = 1)
     {
           logWrite(summaryLog, "SHoundProteinFromDNA", "PASS", "", "", "");

     }
     else
     {
           logWrite(summaryLog, "SHoundProteinFromDNA", "FAIL", "", "", "");

     }
    pvngia = ValNodeFree(pvngia);


    pvngia = SHoundProteinFromDNAList(pvngi3);
    if(pvngia == NULL)
    {
       printf("SHoundProteinFromDNAList failed.\n");
       testFlag = 0;
       logWrite(reportLog, "SHoundProteinFromDNAList", "ValNodePtr", "NOTNULL", "Pointer","NULL");
    }
    else
    {
       printf("SHoundProteinFromDNAList OK.\n");
       testFlag = 1;
       logWrite(reportLog, "SHoundProteinFromDNAList", "ValNodePtr", "NOTNULL", "Pointer","NOTNULL");
    }
    pvngia = ValNodeFree(pvngia);
    
    /* Invalid Param */
    pvngia = SHoundProteinFromDNAList(nullValNodePtr);
    if(pvngia == NULL)
    {
       printf("SHoundProteinFromDNAList failed.\n");
       logWrite(reportLog, "SHoundProteinFromDNAList", "ValNodePtr", "NULL", "Pointer","NULL");
    }
    else
    {
       printf("SHoundProteinFromDNAList OK.\n");
       testFlag = 0;
       logWrite(reportLog, "SHoundProteinFromDNAList", "ValNodePtr", "NULL", "Pointer","NOTNULL");
    }
     if ( testFlag = 1)
     {
           logWrite(summaryLog, "SHoundProteinFromDNAList", "PASS", "", "", "");

     }
     else
     {
           logWrite(summaryLog, "SHoundProteinFromDNAList", "FAIL", "", "", "");

     }
    pvngia = ValNodeFree(pvngia);
  
    intValue = 8698831;
    sprintf(itos1, "%ld", intValue);     
    gin = SHoundDNAFromProtein(intValue);
    sprintf(itos2, "%ld", gin);
    logWrite(reportLog, "SHoundDNAFromProtein", "GI", itos1, "GENEID", itos2);
    if(gin > 0)
    {
      printf("SHoundDNAFromProtein OK.\n");
      testFlag = 1;
    }
    else
    { 
      printf("SHoundDNAFromProtein failed.\n");
      testFlag = 0;
    }
    
    gin = 0; 
    gin = SHoundDNAFromProtein(MAXINT);
    sprintf(itos2, "%ld", gin);
    logWrite(reportLog, "SHoundDNAFromProtein", "GI", "MAXINT", "GENEID", itos2);
    if(gin > 0)
    {
      printf("SHoundDNAFromProtein OK.\n");
      testFlag = 0;
    }
    else
      printf("SHoundDNAFromProtein failed.\n");

    gin = 0; 
    gin = SHoundDNAFromProtein(MINUSONE);
    sprintf(itos1, "%ld", MINUSONE);
    sprintf(itos2, "%ld", gin);
    logWrite(reportLog, "SHoundDNAFromProtein", "GI", itos1, "GENEID", itos2);
    if(gin > 0)
    {
      printf("SHoundDNAFromProtein OK.\n");
      testFlag = 0;
    }
    else
      printf("SHoundDNAFromProtein failed.\n");
     if ( testFlag = 1)
     {
           logWrite(summaryLog, "SHoundDNAFromProtein", "PASS", "", "", "");

     }
     else
     {
           logWrite(summaryLog, "SHoundDNAFromProtein", "FAIL", "", "", "");

     }


    pvngia = SHoundProteinFromDNA(2463028);
    pvngin = SHoundDNAFromProteinList(pvngia);
    if(pvngin == NULL)
    {
       printf("SHoundDNAFromProteinList failed.\n");
       testFlag = 0;
       logWrite(reportLog, "SHoundDNAFromProteinList", "ValNodePtr", "NOTNULL", "Pointer","NULL");
    }
    else
    {
      printf("SHoundDNAFromProteinList OK.\n");
      testFlag = 1;
      logWrite(reportLog, "SHoundDNAFromProteinList", "ValNodePtr", "NOTNULL", "Pointer","NOTNULL");
    }
    pvngin = ValNodeFree(pvngin);
    pvngia = ValNodeFree(pvngia);

    /* Invalid Param Null pointer passed in */
    pvngin = SHoundDNAFromProteinList(nullValNodePtr);
    if(pvngin == NULL)
    {
       printf("SHoundDNAFromProteinList failed.\n");
       logWrite(reportLog, "SHoundDNAFromProteinList", "ValNodePtr", "NULL", "Pointer","NULL");
    }
    else
    {
      printf("SHoundDNAFromProteinList OK.\n");
      testFlag = 0;
      logWrite(reportLog, "SHoundDNAFromProteinList", "ValNodePtr", "NULL", "Pointer","NOTNULL");
    }
     if ( testFlag = 1)
     {
           logWrite(summaryLog, "SHoundDNAFromProteinList", "PASS", "", "", "");

     }
     else
     {
           logWrite(summaryLog, "SHoundDNAFromProteinList", "FAIL", "", "", "");

     }
    pvngin = ValNodeFree(pvngin);

    pvnmuid = SHoundMuidFromGi(myargs[0].intvalue);
    sprintf(itos1, "%ld", myargs[0].intvalue);
    if(pvnmuid == NULL)
    {
       printf("SHoundMuidFromGi failed.\n");
       testFlag = 0;
       logWrite(reportLog, "SHoundMuidFromGi", "GI", itos1, "Pointer", "NULL");
    }
    else
    {
      printf("SHoundMuidFromGi OK.\n");
      testFlag = 1;
      logWrite(reportLog, "SHoundMuidFromGi", "GI", itos1, "Pointer", "NOTNULL");
    }
    pvnmuid = ValNodeFree(pvnmuid);
    /* GI = MAXINT */
    pvnmuid = SHoundMuidFromGi(MAXINT);
    if(pvnmuid == NULL)
    {
       printf("SHoundMuidFromGi failed.\n");
       logWrite(reportLog, "SHoundMuidFromGi", "GI", "MAXINT", "Pointer", "NULL");
    }
    else
    {
      printf("SHoundMuidFromGi OK.\n");
      testFlag = 0;
      logWrite(reportLog, "SHoundMuidFromGi", "GI", "MAXINT", "Pointer", "NOTNULL");
    }
    pvnmuid = ValNodeFree(pvnmuid);
    /* GI = -1 */
    pvnmuid = SHoundMuidFromGi(MINUSONE);
    sprintf(itos1, "%ld", MINUSONE);
    if(pvnmuid == NULL)
    {
       printf("SHoundMuidFromGi failed.\n");
       logWrite(reportLog, "SHoundMuidFromGi", "GI", "-1", "Pointer", "NULL");
    }
    else
    {
      printf("SHoundMuidFromGi OK.\n"); 
      testFlag = 0;
      logWrite(reportLog, "SHoundMuidFromGi", "GI", "-1", "Pointer", "NOTNULL");
    }
     if ( testFlag = 1)
     {
           logWrite(summaryLog, "SHoundMuidFromGi", "PASS", "", "", "");

     }
     else
     {
           logWrite(summaryLog, "SHoundMuidFromGi", "FAIL", "", "", "");

     }
    pvnmuid = ValNodeFree(pvnmuid);


    pvnmuid = SHoundMuidFromGiList(pvngi);
    if(pvnmuid == NULL)
    {
       printf("SHoundMuidFromGiList failed.\n");
       testFlag = 0;
       logWrite(reportLog, "SHoundMuidFromGiList", "ValNodePtr", "NOTNULL", "Pointer","NULL");
    }
    else
    {
       printf("SHoundMuidFromGiList OK.\n");
       testFlag = 1;
       logWrite(reportLog, "SHoundMuidFromGiList", "ValNodePtr", "NOTNULL", "Pointer","NOTNULL");
    }
    pvnmuid = ValNodeFree(pvnmuid);
    /* Invalid Param */
    pvnmuid = SHoundMuidFromGiList(nullValNodePtr);
    if(pvnmuid == NULL)
    {
       printf("SHoundMuidFromGiList failed.\n");
       logWrite(reportLog, "SHoundMuidFromGiList", "ValNodePtr", "NULL", "Pointer","NULL");
    }
    else
    {
       printf("SHoundMuidFromGiList OK.\n");
       testFlag = 0;
       logWrite(reportLog, "SHoundMuidFromGiList", "ValNodePtr", "NULL", "Pointer","NOTNULL");
    }
     if ( testFlag = 1)
     {
           logWrite(summaryLog, "SHoundMuidFromGiList", "PASS", "", "", "");

     }
     else
     {
           logWrite(summaryLog, "SHoundMuidFromGiList", "FAIL", "", "", "");

     }
    pvnmuid = ValNodeFree(pvnmuid);


    pvnmuid = SHoundMuidFrom3D(myargs[2].intvalue);
    sprintf(itos1,"%ld", myargs[2].intvalue);
    if(pvnmuid == NULL)
    {
       printf("SHoundMuidFrom3D failed.\n");
       testFlag = 0;
       logWrite(reportLog, "SHoundMuidFrom3D",  "MMDBID", itos1, "Pointer", "NULL");
    }
    else
    {
       printf("SHoundMuidFrom3D OK.\n");
       testFlag = 1;
       logWrite(reportLog, "SHoundMuidFrom3D",  "MMDBID", itos1, "Pointer", "NOTNULL");
    }
    pvnmuid = ValNodeFree(pvnmuid);
    /* Invalid Param GI = MAXINT */
    pvnmuid = SHoundMuidFrom3D(MAXINT);
    if(pvnmuid == NULL)
    {
       printf("SHoundMuidFrom3D failed.\n");
       logWrite(reportLog, "SHoundMuidFrom3D",  "MMDBID", "MAXINT", "Pointer", "NULL");
    }
    else
    {
       printf("SHoundMuidFrom3D OK.\n");
       testFlag = 0;
       logWrite(reportLog, "SHoundMuidFrom3D",  "MMDBID", "MAXINT", "Pointer", "NOTNULL");
    }
    pvnmuid = ValNodeFree(pvnmuid);
    
    /* Invalid Param GI = -1 */
    pvnmuid = SHoundMuidFrom3D(MINUSONE);
    if(pvnmuid == NULL)
    {
       printf("SHoundMuidFrom3D failed.\n");
       logWrite(reportLog, "SHoundMuidFrom3D",  "MMDBID", "-1", "Pointer", "NULL");
    }
    else
    {
       printf("SHoundMuidFrom3D OK.\n");
       testFlag = 0;
       logWrite(reportLog, "SHoundMuidFrom3D",  "MMDBID", "-1", "Pointer", "NOTNULL");
    }
     if ( testFlag = 1)
     {
           logWrite(summaryLog, "SHoundMuidFrom3D", "PASS", "", "", "");

     }
     else
     {
           logWrite(summaryLog, "SHoundMuidFrom3D", "FAIL", "", "", "");

     }
    pvnmuid = ValNodeFree(pvnmuid);

    pvnmuid = SHoundMuidFrom3DList(pvnmmdbid);
    if(pvnmuid == NULL)
    {
       printf("SHoundMuidFrom3DList failed.\n");
       testFlag = 0;
       logWrite(reportLog, "SHoundMuidFrom3DList",  "ValNodePtr", "NOTNULL", "Pointer", "NULL");

    }
    else
    {
       printf("SHoundMuidFrom3DList OK.\n");
       testFlag = 1;
       logWrite(reportLog, "SHoundMuidFrom3DList",  "ValNodePtr", "NOTNULL", "Pointer", "NOTNULL");
    }
    pvnmuid = ValNodeFree(pvnmuid);
 
    /* Invalid Param */
    pvnmuid = SHoundMuidFrom3DList(nullValNodePtr);
    if(pvnmuid == NULL)
    {
       printf("SHoundMuidFrom3DList failed.\n");
       logWrite(reportLog, "SHoundMuidFrom3DList",  "ValNodePtr", "NULL", "Pointer", "NULL");
    }
    else
    {
       printf("SHoundMuidFrom3DList OK.\n");
       testFlag = 0;
       logWrite(reportLog, "SHoundMuidFrom3DList",  "ValNodePtr", "NULL", "Pointer", "NOTNULL");
    }
     if ( testFlag = 1)
     {
           logWrite(summaryLog, "SHoundMuidFrom3DList", "PASS", "", "", "");

     }
     else
     {
           logWrite(summaryLog, "SHoundMuidFrom3DList", "FAIL", "", "", "");

     }
    pvnmuid = ValNodeFree(pvnmuid);


    taxid = SHoundTaxIDFromGi(myargs[0].intvalue);
    sprintf(itos2, "%ld", taxid);
    sprintf(itos1, "%ld", myargs[0].intvalue);
    logWrite(reportLog, "SHoundTaxIDFromGi", "GI", itos1, "TAXID", itos2);
    if(taxid > 0)
    {
      printf("SHoundTaxIDFromGi OK.\n");
      testFlag = 1;
    }
    else
    {
      printf("SHoundTaxIDFromGi failed.\n"); 
      testFlag = 0;
    }
 
    /* Invalid Param GI = MAXINT */
    taxid = 0;
    taxid = SHoundTaxIDFromGi(MAXINT);
    sprintf(itos2, "%ld", taxid);
    if(taxid > 0)
    {
      testFlag = 0;
    }
    logWrite(reportLog, "SHoundTaxIDFromGi", "GI", "MAXINT", "TAXID", itos2);
    /* Invalid Param GI = MINUSONE */
    taxid = 0;
    taxid = SHoundTaxIDFromGi(MINUSONE);
    if(taxid > 0)
    {
      testFlag = 0;
    }
    sprintf(itos2, "%ld", taxid);
    logWrite(reportLog, "SHoundTaxIDFromGi", "GI", "-1", "TAXID", itos2);
     if ( testFlag = 1)
     {
           logWrite(summaryLog, "SHoundTaxIDFromGi", "PASS", "", "", "");

     }
     else
     {
           logWrite(summaryLog, "SHoundTaxIDFromGi", "FAIL", "", "", "");

     }
   
    pvntaxid = SHoundTaxIDFromGiList(pvngi);
    if(pvntaxid == NULL)
    {
       printf("SHoundTaxIDFromGiList failed.\n");
       testFlag = 0;
       logWrite(reportLog, "SHoundTaxIDFromGiList",  "ValNodePtr", "NOTNULL", "Pointer", "NULL");
    }
    else
    {
       printf("SHoundTaxIDFromGiList OK.\n");
       testFlag = 1;
       logWrite(reportLog, "SHoundTaxIDFromGiList",  "ValNodePtr", "NOTNULL", "Pointer", "NOTNULL");
    }
    pvntaxid = ValNodeFree(pvntaxid);

    /* Invalid Param */
    pvntaxid = SHoundTaxIDFromGiList(nullValNodePtr);
    if(pvntaxid == NULL)
    {
       printf("SHoundTaxIDFromGiList failed.\n");
       logWrite(reportLog, "SHoundTaxIDFromGiList",  "ValNodePtr", "NULL", "Pointer", "NULL");
    }
    else
    {
       printf("SHoundTaxIDFromGiList OK.\n");
       testFlag = 0;
       logWrite(reportLog, "SHoundTaxIDFromGiList",  "ValNodePtr", "NULL", "Pointer", "NOTNULL");
    }
     if ( testFlag = 1)
     {
           logWrite(summaryLog, "SHoundTaxIDFromGiList", "PASS", "", "", "");

     }
     else
     {
           logWrite(summaryLog, "SHoundTaxIDFromGiList", "FAIL", "", "", "");

     }
    pvntaxid = ValNodeFree(pvntaxid);



    pvntaxid = SHoundTaxIDFrom3D(myargs[2].intvalue);
    sprintf(itos1, "%ld", myargs[2].intvalue);
    
    if(pvntaxid == NULL)
      {
       printf("SHoundTaxIDFrom3D failed.\n");
       testFlag = 0;
       logWrite(reportLog, "SHoundTaxIDFrom3D", "MMDBID", itos1, "ValNodePtr", "NULL");
      }
    else
      {
      printf("SHoundTaxIDFrom3D OK.\n");
      testFlag = 1;
      logWrite(reportLog, "SHoundTaxIDFrom3D", "MMDBID", itos1, "ValNodePtr", "NOTNULL");
      }
    pvntaxid = ValNodeFree(pvntaxid);
    /* Invalid Param GI = MAXINT */
    pvntaxid = SHoundTaxIDFrom3D(MAXINT);
    if (pvntaxid == NULL)
      {
	logWrite(reportLog, "SHoundTaxIDFrom3D", "MMDBID", "MAXINT", "ValNodePtr", "NULL");
      }
    else
      {
      testFlag = 0;
	logWrite(reportLog, "SHoundTaxIDFrom3D", "MMDBID", "MAXINT", "ValNodePtr", "NOTNULL");
      }
    pvntaxid = ValNodeFree(pvntaxid);

    /* Invalid Param GI = -1 */
    pvntaxid = SHoundTaxIDFrom3D(MINUSONE);
    if (pvntaxid == NULL)
      {
	logWrite(reportLog, "SHoundTaxIDFrom3D", "MMDBID", "-1", "ValNodePtr", "NULL");
      }
    else
      {
      testFlag = 0;
	logWrite(reportLog, "SHoundTaxIDFrom3D", "MMDBID", "-1", "ValNodePtr", "NOTNULL");
      }
     if ( testFlag = 1)
     {
           logWrite(summaryLog, "SHoundTaxIDFrom3D", "PASS", "", "", "");

     }
     else
     {
           logWrite(summaryLog, "SHoundTaxIDFrom3D", "FAIL", "", "", "");

     }
    pvntaxid = ValNodeFree(pvntaxid);


    pvntaxid = SHoundTaxIDFrom3DList(pvnmmdbid);
    if(pvntaxid == NULL)
    {
       printf("SHoundTaxIDFrom3DList failed.\n");
      testFlag = 0;
       logWrite(reportLog, "SHoundTaxIDFrom3DList", "ValNodePtr", "NOTNULL", "Pointer","NULL");
    }
    else
    {
      printf("SHoundTaxIDFrom3DList OK.\n");
      testFlag = 1;
      logWrite(reportLog, "SHoundTaxIDFrom3DList", "ValNodePtr", "NOTNULL", "Pointer","NOTNULL");
    }
    pvntaxid = ValNodeFree(pvntaxid);

    /* Invalid Param */
    pvntaxid = SHoundTaxIDFrom3DList(nullValNodePtr);
    if(pvntaxid == NULL)
    {
       printf("SHoundTaxIDFrom3DList failed.\n");
       logWrite(reportLog, "SHoundTaxIDFrom3DList", "ValNodePtr", "NULL", "Pointer","NULL");
    }
    else
    {
      printf("SHoundTaxIDFrom3DList OK.\n");
      testFlag = 0;
      logWrite(reportLog, "SHoundTaxIDFrom3DList", "ValNodePtr", "NULL", "Pointer","NOTNULL");
    }
     if ( testFlag = 1)
     {
           logWrite(summaryLog, "SHoundTaxIDFrom3DList", "PASS", "", "", "");

     }
     else
     {
           logWrite(summaryLog, "SHoundTaxIDFrom3DList", "FAIL", "", "", "");

     }
    pvntaxid = ValNodeFree(pvntaxid);
    

	/*-------------------------------
	  SHoundProteinsFromOrganism
	  -------------------------------*/
	intValue = 9606;
    pvngia = SHoundProteinsFromOrganism(intValue, CHROM_ALL);
    sprintf(itos1, "%ld%s", intValue, "CHROM_ALL");
    if (pvngia == NULL)
    {
       printf("SHoundProteinsFromOrganism failed.\n");
       testFlag = 0;
       logWrite(reportLog, "SHoundProteinsFromOrganism",  "TAXID/CHROMFLAG", itos1, "Pointer", "NULL");
    }
    else
    {
       printf("SHoundProteinsFromOrganism OK.\n");
       testFlag = 1;
       logWrite(reportLog, "SHoundProteinsFromOrganism",  "TAXID/CHROMFLAG", itos1, "Pointer", "NOTNULL");
    }
    pvngia = ValNodeFree(pvngia);
    /* Invalid Param TAXID = MAXINT */
    pvngia = SHoundProteinsFromOrganism(MAXINT, CHROM_ALL);
    if (pvngia == NULL)
    {
       printf("SHoundProteinsFromOrganism failed.\n");
       logWrite(reportLog, "SHoundProteinsFromOrganism",  "TAXID/CHROMFLAG", "MAXINT/CHROM_ALL", "Pointer", "NULL");
    }
    else
    {
       printf("SHoundProteinsFromOrganism OK.\n");
       testFlag = 0;
       logWrite(reportLog, "SHoundProteinsFromOrganism",  "TAXID/CHROMFLAG", "MAXINT/CHROM_ALL", "Pointer", "NOTNULL");
    }
    pvngia = ValNodeFree(pvngia);
    /* Invalid Param TAXID = MINUSONE */
    pvngia = SHoundProteinsFromOrganism(MINUSONE, CHROM_ALL);
    sprintf(itos1, "%d%s", MINUSONE, "/CHROM_ALL");
    if (pvngia == NULL)
    {
       printf("SHoundProteinsFromOrganism failed.\n");
       logWrite(reportLog, "SHoundProteinsFromOrganism",  "TAXID/CHROMFLAG", itos1, "Pointer", "NULL");
    }
    else
    {
       printf("SHoundProteinsFromOrganism OK.\n");
       testFlag = 0;
       logWrite(reportLog, "SHoundProteinsFromOrganism",  "TAXID/CHROMFLAG", itos1, "Pointer", "NOTNULL");
    }
     if ( testFlag = 1)
     {
           logWrite(summaryLog, "SHoundProteinsFromOrganism", "PASS", "", "", "");

     }
     else
     {
           logWrite(summaryLog, "SHoundProteinsFromOrganism", "FAIL", "", "", "");

     }
    pvngia = ValNodeFree(pvngia);


    ValNodeAddInt(&pvntaxid, 0, 9606);
    ValNodeAddInt(&pvntaxid, 0, 56636);
    ValNodeAddInt(&pvntaxid, 0, 6239);
    pvngia = SHoundProteinsFromOrganismList(pvntaxid, CHROM_ALL);
    if (pvngia == NULL)
    {
      printf("SHoundProteinsFromOrganismList failed.\n");
       testFlag = 0;
      logWrite(reportLog, "SHoundProteinsFromOrganismList",  "ValNodePtr/CHROMFLAG", "NOTNULL/CHROM_ALL", "Pointer", "NULL");
    }
    else
    {
      printf("SHoundProteinsFromOrganismList OK.\n");
       testFlag = 1;
      logWrite(reportLog, "SHoundProteinsFromOrganismList",  "ValNodePtr/CHROMFLAG", "NOTNULL/CHROM_ALL", "Pointer", "NOTNULL");
    }
    pvngia = ValNodeFree(pvngia);
    /*Invalid param */
    pvngia = SHoundProteinsFromOrganismList(nullValNodePtr, CHROM_ALL);
    if (pvngia == NULL)
    {
      printf("SHoundProteinsFromOrganismList failed.\n");
      logWrite(reportLog, "SHoundProteinsFromOrganismList",  "ValNodePtr/CHROMFLAG", "NOTNULL/CHROM_ALL", "Pointer", "NULL");
    }
    else
    {
      printf("SHoundProteinsFromOrganismList OK.\n");
       testFlag = 0;
      logWrite(reportLog, "SHoundProteinsFromOrganismList",  "ValNodePtr/CHROMFLAG", "NOTNULL/CHROM_ALL", "Pointer", "NOTNULL");
    }
    pvngia = ValNodeFree(pvngia);
     if ( testFlag = 1)
     {
           logWrite(summaryLog, "SHoundProteinsFromOrganismList", "PASS", "", "", "");

     }
     else
     {
           logWrite(summaryLog, "SHoundProteinsFromOrganismList", "FAIL", "", "", "");

     }

    /*-----------------------------
	  SHoundProteinsFromChromosome
	  -----------------------------*/
	intValue = 321;
    pvngia = SHoundProteinsFromChromosome(intValue);
    sprintf(itos1, "%ld", intValue);
    if (pvngia == NULL)
    {
       printf("SHoundProteinsFromChromosome failed.\n");
       testFlag = 0;
       logWrite(reportLog, "SHoundProteinsFromChromosome", "CHROMID", itos1, "Pointer", "NULL");
    }
    else
    {
       printf("SHoundProteinsFromChromosome OK.\n");
       testFlag = 1;
       logWrite(reportLog, "SHoundProteinsFromChromosome", "CHROMID", itos1, "Pointer", "NOTNULL");
    }
    pvngia = ValNodeFree(pvngia);

    /* Invalid Param CHROMSOME ID = MAXINT */
    pvngia = SHoundProteinsFromChromosome(MAXINT);
    if (pvngia == NULL)
    {
       printf("SHoundProteinsFromChromosome failed.\n");
       logWrite(reportLog, "SHoundProteinsFromChromosome", "CHROMID", "MAXINT", "Pointer", "NULL");
    }
    else
    {
       printf("SHoundProteinsFromChromosome OK.\n");
       testFlag = 0;
       logWrite(reportLog, "SHoundProteinsFromChromosome", "CHROMID", "MAXINT", "Pointer", "NOTNULL");
    }
    pvngia = ValNodeFree(pvngia);

    /* Invalid Param CHROMSOME ID = MINUSONE */
    pvngia = SHoundProteinsFromChromosome(MINUSONE);
    if (pvngia == NULL)
    {
       printf("SHoundProteinsFromChromosome failed.\n");
       logWrite(reportLog, "SHoundProteinsFromChromosome", "CHROMID", "-1", "Pointer", "NULL");
    }
    else
    {
       printf("SHoundProteinsFromChromosome OK.\n");
       testFlag = 0;
       logWrite(reportLog, "SHoundProteinsFromChromosome", "CHROMID", "-1", "Pointer", "NOTNULL");
    }
     if ( testFlag = 1)
     {
           logWrite(summaryLog, "SHoundProteinsFromChromosome", "PASS", "", "", "");

     }
     else
     {
           logWrite(summaryLog, "SHoundProteinsFromChromosome", "FAIL", "", "", "");

     }
    pvngia = ValNodeFree(pvngia);

	/*---------------------------------
	  SHoundProteinsFromChromosomeList
	  --------------------------------*/
    ValNodeAddInt(&pvnchromid, 0, 321);
    ValNodeAddInt(&pvnchromid, 0, 10);
    ValNodeAddInt(&pvnchromid, 0, 12);
    pvngia = SHoundProteinsFromChromosomeList(pvnchromid);
    if (pvngia == NULL)
    {
       printf("SHoundProteinsFromChromosomeList failed.\n");
       testFlag = 0;
       logWrite(reportLog, "SHoundProteinsFromChromosomeList", "ValNodePtr", "NOTNULL", "Pointer", "NULL");
    }
    else
    {
       printf("SHoundProteinsFromChromosomeList OK.\n");
       testFlag = 1;
       logWrite(reportLog, "SHoundProteinsFromChromosomeList", "ValNodePtr", "NOTNULL", "Pointer", "NOTNULL");
    }
    pvngia = ValNodeFree(pvngia);

    /* Invalid Param */
    pvngia = SHoundProteinsFromChromosomeList(nullValNodePtr);
    if (pvngia == NULL)
    {
       printf("SHoundProteinsFromChromosomeList failed.\n");
       logWrite(reportLog, "SHoundProteinsFromChromosomeList", "ValNodePtr", "NULL", "Pointer", "NULL");
    }
    else
    {
       printf("SHoundProteinsFromChromosomeList OK.\n");
       testFlag = 0;
       logWrite(reportLog, "SHoundProteinsFromChromosomeList", "ValNodePtr", "NULL", "Pointer", "NOTNULL");
    }
     if ( testFlag = 1)
     {
           logWrite(summaryLog, "SHoundProteinsFromChromosomeList", "PASS", "", "", "");

     }
     else
     {
           logWrite(summaryLog, "SHoundProteinsFromChromosomeList", "FAIL", "", "", "");

     }
    pvngia = ValNodeFree(pvngia);


    pvngen = SHoundAllGenomes();
    if(pvngen == NULL)
    {
      printf("SHoundAllGenomes failed.\n");
       testFlag = 0;
      logWrite(reportLog, "SHoundAllGenomes", "NONE", "NONE", "Pointer", "NULL");
    }
    else
    {
      printf("SHoundAllGenomes OK.\n");
      logWrite(reportLog, "SHoundAllGenomes", "NONE", "NONE", "Pointer", "NOTNULL");
      testFlag = 1;
    }
     if ( testFlag = 1)
     {
           logWrite(summaryLog, "SHoundAllGenomes", "PASS", "", "", "");

     }
     else
     {
           logWrite(summaryLog, "SHoundAllGenomes", "FAIL", "", "", "");

     }


    pvnchrom = SHoundChromosomeFromGenome(myargs[3].intvalue, CHROM_ALL);
    sprintf(itos1, "%ld/%s", myargs[3].intvalue, "CHROM_ALL");
    if(pvnchrom == NULL)
    {
       printf("SHoundChromosomeFromGenome failed.\n");
       testFlag = 0;
       logWrite(reportLog, "SHoundChromosomeFromGenome",  "TAXID/CHROMFLAG", itos1, "Pointer", "NULL");
    }
    else
    {
       printf("SHoundChromosomeFromGenome OK.\n");
       testFlag = 1;
       logWrite(reportLog, "SHoundChromosomeFromGenome",  "TAXID/CHROMFLAG", itos1, "Pointer", "NOTNULL");
    }
    pvnchrom = ValNodeFree(pvnchrom);
    /* Invalid Param TAXID = MAXINT */
    pvnchrom = SHoundChromosomeFromGenome(MAXINT, CHROM_ALL);
    if(pvnchrom == NULL)
    {
       printf("SHoundChromosomeFromGenome failed.\n");
       logWrite(reportLog, "SHoundChromosomeFromGenome",  "TAXID/CHROMFLAG", "MAXINT/CHROMALL", "Pointer", "NULL");
    }
    else
    {
       printf("SHoundChromosomeFromGenome OK.\n");
       testFlag = 0;
       logWrite(reportLog, "SHoundChromosomeFromGenome",  "TAXID/CHROMFLAG", "MAXINT/CHROMALL", "Pointer", "NOTNULL");
    }
    pvnchrom = ValNodeFree(pvnchrom);
    /* Invalid Param TAXID = MINUSONE */
    pvnchrom = SHoundChromosomeFromGenome(MINUSONE, CHROM_ALL);
    sprintf(itos1, "%ld%s", MINUSONE, "/CHROM_ALL");
    if(pvnchrom == NULL)
    {
       printf("SHoundChromosomeFromGenome failed.\n");
       logWrite(reportLog, "SHoundChromosomeFromGenome",  "TAXID/CHROMFLAG", itos1, "Pointer", "NULL");
    }
    else
    {
       printf("SHoundChromosomeFromGenome OK.\n");
       testFlag = 0;
       logWrite(reportLog, "SHoundChromosomeFromGenome",  "TAXID/CHROMFLAG", itos1, "Pointer", "NOTNULL");
    }
    pvnchrom = ValNodeFree(pvnchrom);
     if ( testFlag = 1)
     {
           logWrite(summaryLog, "SHoundChromosomeFromGenome", "PASS", "", "", "");

     }
     else
     {
           logWrite(summaryLog, "SHoundChromosomeFromGenome", "FAIL", "", "", "");

     }


    pvnchrom = SHoundChromosomeFromGenomeList(pvngen, CHROM_ALL);
    if(pvnchrom == NULL)
    {
       printf("SHoundChromosomeFromGenomeList failed.\n");
       testFlag = 0;
       logWrite(reportLog, "SHoundChromosomeFromGenomeList","VALNODE/CHROMFLAG","NOTNULL/CHROM_ALL", "Pointer", "NULL");
    }
    else
    {
       printf("SHoundChromosomeFromGenomeList OK.\n");
       testFlag = 1;
       logWrite(reportLog, "SHoundChromosomeFromGenomeList","VALNODE/CHROMFLAG","NOTNULL/CHROM_ALL", "Pointer", "NOTNULL");
    }
    pvnchrom = ValNodeFree(pvnchrom);

    /* Invalid Param */
    pvnchrom = SHoundChromosomeFromGenomeList(nullValNodePtr, CHROM_ALL);
    if(pvnchrom == NULL)
    {
       printf("SHoundChromosomeFromGenomeList failed.\n");
       logWrite(reportLog, "SHoundChromosomeFromGenomeList","VALNODE/CHROMFLAG","NULL/CHROM_ALL", "Pointer", "NULL");
    }
    else
    {
       printf("SHoundChromosomeFromGenomeList OK.\n");
       testFlag = 0;
       logWrite(reportLog, "SHoundChromosomeFromGenomeList","VALNODE/CHROMFLAG","NULL/CHROM_ALL", "Pointer", "NOTNULL");
    }
    pvnchrom = ValNodeFree(pvnchrom);
     if ( testFlag = 1)
     {
           logWrite(summaryLog, "SHoundChromosomeFromGenomeList", "PASS", "", "", "");

     }
     else
     {
           logWrite(summaryLog, "SHoundChromosomeFromGenomeList", "FAIL", "", "", "");

     }

    /*-----------------------------
	  SHoundDNAFromOrganism
	  -----------------------------*/
	intValue = 9606;
    pvngin = SHoundDNAFromOrganism(intValue, CHROM_ALL);
    sprintf(itos1, "%ld%s", intValue, "CHROM_ALL");
    if (pvngin == NULL)
    {
       printf("SHoundDNAFromOrganism failed.\n");
       testFlag = 0;
       logWrite(reportLog, "SHoundDNAFromOrganism",  "TAXID/CHROMFLAG", itos1, "Pointer", "NULL");
    }
    else
    {
       printf("SHoundDNAFromOrganism OK.\n");
       testFlag = 1;
       logWrite(reportLog, "SHoundDNAFromOrganism",  "TAXID/CHROMFLAG", itos1, "Pointer", "NOTNULL");
    }
    pvngin = ValNodeFree(pvngin);
    /* Invalid Param TAXID = MAXINT*/
    pvngin = SHoundDNAFromOrganism(MAXINT, CHROM_ALL);
    if (pvngin == NULL)
    {
       printf("SHoundDNAFromOrganism failed.\n");
       logWrite(reportLog, "SHoundDNAFromOrganism",  "TAXID/CHROMFLAG", "MAXINT/CHROM_ALL", "Pointer", "NULL");
    }
    else
    {
       printf("SHoundDNAFromOrganism OK.\n");
       testFlag = 0;
       logWrite(reportLog, "SHoundDNAFromOrganism",  "TAXID/CHROMFLAG", "MAXINT/CHROM_ALL", "Pointer", "NOTNULL");
    }
    pvngin = ValNodeFree(pvngin);
    /* Invalid Param TAXID = MINUSONE*/
    pvngin = SHoundDNAFromOrganism(MINUSONE, CHROM_ALL);
    sprintf(itos1, "%ld%s", MINUSONE, "/CHROM_ALL");
    if (pvngin == NULL)
    {
       printf("SHoundDNAFromOrganism failed.\n");
       logWrite(reportLog, "SHoundDNAFromOrganism",  "TAXID/CHROMFLAG", itos1, "Pointer", "NULL");
    }
    else
    {
       printf("SHoundDNAFromOrganism OK.\n");
       testFlag = 0;
       logWrite(reportLog, "SHoundDNAFromOrganism",  "TAXID/CHROMFLAG", itos1, "Pointer", "NOTNULL");
    }
     if ( testFlag = 1)
     {
           logWrite(summaryLog, "SHoundDNAFromOrganism", "PASS", "", "", "");

     }
     else
     {
           logWrite(summaryLog, "SHoundDNAFromOrganism", "FAIL", "", "", "");

     }
    pvngin = ValNodeFree(pvngin);


    pvngin = SHoundDNAFromOrganismList(pvntaxid, CHROM_ALL);
    if (pvngin == NULL)
    {
       printf("SHoundDNAFromOrganismList failed.\n");
       testFlag = 0;
       logWrite(reportLog, "SHoundDNAFromOrganismList",  "ValNodePtr/CHROMFLAG", "NOTNULL/CHROM_ALL", "Pointer", "NULL");
    }
    else
    {
       printf("SHoundDNAFromOrganismList OK.\n");
       testFlag = 1;
       logWrite(reportLog, "SHoundDNAFromOrganismList",  "ValNodePtr/CHROMFLAG", "NOTNULL/CHROM_ALL", "Pointer", "NOTNULL");
    }
    pvngin = ValNodeFree(pvngin);
  
    /* Invalid Param */
    pvngin = SHoundDNAFromOrganismList(nullValNodePtr, CHROM_ALL);
    if (pvngin == NULL)
    {
       printf("SHoundDNAFromOrganismList failed.\n");
       logWrite(reportLog, "SHoundDNAFromOrganismList",  "ValNodePtr/CHROMFLAG", "NULL/CHROM_ALL", "Pointer", "NULL");
    }
    else
    {
       printf("SHoundDNAFromOrganismList OK.\n");
       testFlag = 0;
       logWrite(reportLog, "SHoundDNAFromOrganismList",  "ValNodePtr/CHROMFLAG", "NULL/CHROM_ALL", "Pointer", "NOTNULL");
    }
     if ( testFlag = 1)
     {
           logWrite(summaryLog, "SHoundDNAFromOrganismList", "PASS", "", "", "");

     }
     else
     {
           logWrite(summaryLog, "SHoundDNAFromOrganismList", "FAIL", "", "", "");

     }
    pvngin = ValNodeFree(pvngin);

	
    /*-------------------------------
	  SHoundDNAFromChromosome
	  -------------------------------*/
	intValue = 321;
    pvngin = SHoundDNAFromChromosome(intValue);
    sprintf(itos1, "%ld", intValue);
    if (pvngin == NULL)
    {
       printf("SHoundDNAFromChromosome failed.\n");
       testFlag = 0;
       logWrite(reportLog, "SHoundDNAFromChromosome", "CHROMID", itos1, "Pointer", "NULL");
    }
    else
    {
       printf("SHoundDNAFromChromosome OK.\n");
       testFlag = 1;
       logWrite(reportLog, "SHoundDNAFromChromosome", "CHROMID", itos1, "Pointer", "NOTNULL");
    }
    pvngin = ValNodeFree(pvngin);
    /* Invalid Param CHROMID = MAXINT */
    pvngin = SHoundDNAFromChromosome(MAXINT);
    if (pvngin == NULL)
    {
       printf("SHoundDNAFromChromosome failed.\n");
       logWrite(reportLog, "SHoundDNAFromChromosome", "CHROMID", "MAXINT", "Pointer", "NULL");
    }
    else
    {
       printf("SHoundDNAFromChromosome OK.\n");
       testFlag = 0;
       logWrite(reportLog, "SHoundDNAFromChromosome", "CHROMID", "MAXINT", "Pointer", "NOTNULL");
    }
    pvngin = ValNodeFree(pvngin);
    /* Invalid Param CHROMID = MINUSONE */
    pvngin = SHoundDNAFromChromosome(MINUSONE);
    sprintf(itos1, "%ld", MINUSONE);
    if (pvngin == NULL)
    {
       printf("SHoundDNAFromChromosome failed.\n");
       logWrite(reportLog, "SHoundDNAFromChromosome", "CHROMID", itos1, "Pointer", "NULL");
    }
    else
    {
       printf("SHoundDNAFromChromosome OK.\n");
       testFlag = 0;
       logWrite(reportLog, "SHoundDNAFromChromosome", "CHROMID", itos1, "Pointer", "NOTNULL");
    }
     if ( testFlag = 1)
     {
           logWrite(summaryLog, "SHoundDNAFromChromosome", "PASS", "", "", "");

     }
     else
     {
           logWrite(summaryLog, "SHoundDNAFromChromosome", "FAIL", "", "", "");

     }
    pvngin = ValNodeFree(pvngin);


    /*-------------------------------
	  SHoundDNAFromChromosomeList
	  -------------------------------*/
    pvngin = SHoundDNAFromChromosomeList(pvnchromid);
    if (pvngin == NULL)
    {
       printf("SHoundDNAFromChromosomeList failed.\n");
       testFlag = 0;
       logWrite(reportLog, "SHoundDNAFromChromosomeList", "ValNodePtr", "NOTNULL", "Pointer", "NULL");
    }
    else
    {
       printf("SHoundDNAFromChromosomeList OK.\n");
       testFlag = 1;
       logWrite(reportLog, "SHoundDNAFromChromosomeList", "ValNodePtr", "NOTNULL", "Pointer", "NOTNULL");
    }
    pvngin = ValNodeFree(pvngin);

    /* invalid Param */
    pvngin = SHoundDNAFromChromosomeList(nullValNodePtr);
    if (pvngin == NULL)
    {
       printf("SHoundDNAFromChromosomeList failed.\n");
       logWrite(reportLog, "SHoundDNAFromChromosomeList", "ValNodePtr", "NULL", "Pointer", "NULL");
    }
    else
    {
       printf("SHoundDNAFromChromosomeList OK.\n");
       testFlag = 0;
       logWrite(reportLog, "SHoundDNAFromChromosomeList", "ValNodePtr", "NULL", "Pointer", "NOTNULL");
    }
     if ( testFlag = 1)
     {
           logWrite(summaryLog, "SHoundDNAFromChromosomeList", "PASS", "", "", "");

     }
     else
     {
           logWrite(summaryLog, "SHoundDNAFromChromosomeList", "FAIL", "", "", "");

     }
    pvngin = ValNodeFree(pvngin);


    pvngi2 = SHoundRedundantGroup(myargs[0].intvalue);
    sprintf(itos1,"%ld", myargs[0].intvalue);
    if(pvngi2 == NULL)
    {
       printf("SHoundRedundantGroup failed.\n");
       testFlag = 0;
       logWrite(reportLog, "SHoundRedundantGroup", "GI", itos1,"Pointer", "NULL");
    }
    else
    {
       printf("SHoundRedundantGroup OK.\n");
       testFlag = 1;
       logWrite(reportLog, "SHoundRedundantGroup", "GI", itos1,"Pointer", "NOTNULL");
    }
    pvngi2 = ValNodeFree(pvngi2);

    /* Invalid param GI = MAXINT */
    pvngi2 = SHoundRedundantGroup(MAXINT);
    if(pvngi2 == NULL)
    {
       printf("SHoundRedundantGroup failed.\n");
       logWrite(reportLog, "SHoundRedundantGroup", "GI", "MAXINT","Pointer", "NULL");
    }
    else
    {
       printf("SHoundRedundantGroup OK.\n");
       testFlag = 0;
       logWrite(reportLog, "SHoundRedundantGroup", "GI", "MAXINT","Pointer", "NOTNULL");
    }
    pvngi2 = ValNodeFree(pvngi2);
    /* Invalid param GI = MINUSONE */
    pvngi2 = SHoundRedundantGroup(MINUSONE);
    if(pvngi2 == NULL)
    {
       printf("SHoundRedundantGroup failed.\n");
       logWrite(reportLog, "SHoundRedundantGroup", "GI", "-1","Pointer", "NULL");
    }
    else
    {
       printf("SHoundRedundantGroup OK.\n");
       testFlag = 0;
       logWrite(reportLog, "SHoundRedundantGroup", "GI", "-1","Pointer", "NOTNULL");
    }
     if ( testFlag = 1)
     {
           logWrite(summaryLog, "SHoundRedundantGroup", "PASS", "", "", "");

     }
     else
     {
           logWrite(summaryLog, "SHoundRedundantGroup", "FAIL", "", "", "");

     }
    pvngi2 = ValNodeFree(pvngi2);


    pvngi2 = SHoundRedundantGroupList(pvngi);
    if(pvngi2 == NULL)
    {
      printf("SHoundRedundantGroupList failed.\n");
       testFlag = 0;
      logWrite(reportLog, "SHoundRedundantGroupList", "ValNodePtr", "NOTNULL", "Pointer", "NULL");
    }
    else
    {
      printf("SHoundRedundantGroupList OK.\n");
       testFlag = 1;
      logWrite(reportLog, "SHoundRedundantGroupList", "ValNodePtr", "NOTNULL", "Pointer", "NOTNULL");
    }
    pvngi2 = ValNodeFree(pvngi2);
  
    /* Invalid param */
    pvngi2 = SHoundRedundantGroupList(nullValNodePtr);
    if(pvngi2 == NULL)
    {
      printf("SHoundRedundantGroupList failed.\n");
      logWrite(reportLog, "SHoundRedundantGroupList", "ValNodePtr", "NULL", "Pointer", "NULL");
    }
    else
    {
      printf("SHoundRedundantGroupList OK.\n");
       testFlag = 0;
      logWrite(reportLog, "SHoundRedundantGroupList", "ValNodePtr", "NULL", "Pointer", "NOTNULL");
    }
     if ( testFlag = 1)
     {
           logWrite(summaryLog, "SHoundRedundantGroupList", "PASS", "", "", "");

     }
     else
     {
           logWrite(summaryLog, "SHoundRedundantGroupList", "FAIL", "", "", "");

     }
    pvngi2 = ValNodeFree(pvngi2);

    pvngi2 = SHoundProteinsFromTaxID(myargs[3].intvalue);
    sprintf(itos1, "%ld", myargs[3].intvalue);
    if(pvngi2 == NULL)
    {
       printf("SHoundProteinsFromTaxID failed.\n");
       testFlag = 0;
       logWrite(reportLog, "SHoundProteinsFromTAXID", "TAXID", itos1, "Pointer", "NULL");
    }
    else
    {
       printf("SHoundProteinsFromTaxID OK.\n");
       testFlag = 1;
       logWrite(reportLog, "SHoundProteinsFromTAXID", "TAXID", itos1, "Pointer", "NOTNULL");
    }
    pvngi2 = ValNodeFree(pvngi2);
    /* Invalid Param  TAXID = MAXINT */
    pvngi2 = SHoundProteinsFromTaxID(MAXINT);
    if(pvngi2 == NULL)
    {
       printf("SHoundProteinsFromTaxID failed.\n");
       logWrite(reportLog, "SHoundProteinsFromTAXID", "TAXID", "MAXINT", "Pointer", "NULL");
    }
    else
    {
       printf("SHoundProteinsFromTaxID OK.\n");
       testFlag = 0;
       logWrite(reportLog, "SHoundProteinsFromTAXID", "TAXID", "MAXINT", "Pointer", "NOTNULL");
    }
    pvngi2 = ValNodeFree(pvngi2);
    /* Invalid Param  TAXID = MINUSONE */
    pvngi2 = SHoundProteinsFromTaxID(MINUSONE);
    if(pvngi2 == NULL)
    {
       printf("SHoundProteinsFromTaxID failed.\n");
       logWrite(reportLog, "SHoundProteinsFromTAXID", "TAXID", "-1", "Pointer", "NULL");
    }
    else
    {
       printf("SHoundProteinsFromTaxID OK.\n");
       testFlag = 0;
       logWrite(reportLog, "SHoundProteinsFromTAXID", "TAXID", "-1", "Pointer", "NOTNULL");
    }
     if ( testFlag = 1)
     {
           logWrite(summaryLog, "SHoundProteinsFromTAXID", "PASS", "", "", "");

     }
     else
     {
           logWrite(summaryLog, "SHoundProteinsFromTAXID", "FAIL", "", "", "");

     }
    pvngi2 = ValNodeFree(pvngi2);
    /* Test for precomputed data using SHoundProteinsFromTaXIDIII */
    pvngi2 = SHoundProteinsFromTaxIDIII(precomTaxid);
    sprintf(itos1, "%ld", precomTaxid);
    if(pvngi2 == NULL)
    {
       printf("SHoundProteinsFromTaxIDIII failed.\n");
       testFlag = 0;
       logWrite(reportLog, "SHoundProteinsFromTAXIDIII", "TAXID", itos1, "Pointer", "NULL");
    }
    else
    {
       printf("SHoundProteinsFromTaxIDIII OK.\n");
       testFlag = 1;
       logWrite(reportLog, "SHoundProteinsFromTAXIDIII", "TAXID", itos1, "Pointer", "NOTNULL");
    }
    pvngi2 = ValNodeFree(pvngi2);
    /* Invalid Param  TAXID = MAXINT */
    pvngi2 = SHoundProteinsFromTaxIDIII(MAXINT);
    if(pvngi2 == NULL)
    {
       printf("SHoundProteinsFromTaxIDIII failed.\n");
       logWrite(reportLog, "SHoundProteinsFromTAXIDIII", "TAXID", "MAXINT", "Pointer", "NULL");
    }
    else
    {
       printf("SHoundProteinsFromTaxIDIII OK.\n");
       testFlag = 0;
       logWrite(reportLog, "SHoundProteinsFromTAXIDIII", "TAXID", "MAXINT", "Pointer", "NOTNULL");
    }
    pvngi2 = ValNodeFree(pvngi2);
    /* Invalid Param  TAXID = MINUSONE */
    pvngi2 = SHoundProteinsFromTaxIDIII(MINUSONE);
    if(pvngi2 == NULL)
    {
       printf("SHoundProteinsFromTaxIDIII failed.\n");
       logWrite(reportLog, "SHoundProteinsFromTAXIDIII", "TAXID", "-1", "Pointer", "NULL");
    }
    else
    {
       printf("SHoundProteinsFromTaxIDIII OK.\n");
       testFlag = 0;
       logWrite(reportLog, "SHoundProteinsFromTAXIDIII", "TAXID", "-1", "Pointer", "NOTNULL");
    }
     if ( testFlag = 1)
     {
           logWrite(summaryLog, "SHoundProteinsFromTAXIDIII", "PASS", "", "", "");

     }
     else
     {
           logWrite(summaryLog, "SHoundProteinsFromTAXIDIII", "FAIL", "", "", "");

     }
    pvngi2 = ValNodeFree(pvngi2);

      /*----------------------
        SHoundDNAFromTaxID
	----------------------*/
    pvngi2 = SHoundDNAFromTaxID(myargs[3].intvalue);
    sprintf(itos1, "%ld", myargs[3].intvalue);
    if(pvngi2 == NULL)
    {
      printf("SHoundDNAFromTaxID failed.\n");
       testFlag = 0;
      logWrite(reportLog, "SHoundDNAFromTaxID", "TAXID", itos1, "Pointer", "NULL");
    }
    else
    {
      printf("SHoundDNAFromTaxID OK.\n");
       testFlag = 1;
      logWrite(reportLog, "SHoundDNAFromTaxID", "TAXID", itos1, "Pointer", "NOTNULL");
    }
    pvngi2 = ValNodeFree(pvngi2);

    /* Invalid Param TAXID = MAXINT */
    pvngi2 = SHoundDNAFromTaxID(MAXINT);
    if(pvngi2 == NULL)
    {
      printf("SHoundDNAFromTaxID failed.\n");
      logWrite(reportLog, "SHoundDNAFromTaxID", "TAXID", "MAXINT", "Pointer", "NULL");
    }
    else
    {
      printf("SHoundDNAFromTaxID OK.\n");
       testFlag = 0;
      logWrite(reportLog, "SHoundDNAFromTaxID", "TAXID", "MAXINT", "Pointer", "NOTNULL");
    }
    pvngi2 = ValNodeFree(pvngi2);
    /* Invalid Param TAXID = MINUSONE */
    pvngi2 = SHoundDNAFromTaxID(MINUSONE);
    if(pvngi2 == NULL)
    {
      printf("SHoundDNAFromTaxID failed.\n");
      logWrite(reportLog, "SHoundDNAFromTaxID", "TAXID", "-1", "Pointer", "NULL");
    }
    else
    {
      printf("SHoundDNAFromTaxID OK.\n");
       testFlag = 0;
      logWrite(reportLog, "SHoundDNAFromTaxID", "TAXID", "-1", "Pointer", "NOTNULL");
    }
     if ( testFlag = 1)
     {
           logWrite(summaryLog, "SHoundDNAFromTaxID", "PASS", "", "", "");

     }
     else
     {
           logWrite(summaryLog, "SHoundDNAFromTaxID", "FAIL", "", "", "");

     }
    pvngi2 = ValNodeFree(pvngi2);
    
    
      /*------------------------
        SHoundDNAFromTaxIDIII
	------------------------*/
    pvngi2 = SHoundDNAFromTaxIDIII(precomTaxid);
    sprintf(itos1, "%ld", precomTaxid);
    if(pvngi2 == NULL)
    {
      printf("SHoundDNAFromTaxIDIII failed.\n");
      testFlag = 0;
      logWrite(reportLog, "SHoundDNAFromTaxIDIII", "TAXID", itos1, "Pointer", "NULL");
    }
    else
    {
      printf("SHoundDNAFromTaxIDIII OK.\n");
      testFlag = 1;
      logWrite(reportLog, "SHoundDNAFromTaxIDIII", "TAXID", itos1, "Pointer", "NOTNULL");
    }
    pvngi2 = ValNodeFree(pvngi2);

    /* Invalid Param TAXID = MAXINT */
    pvngi2 = SHoundDNAFromTaxIDIII(MAXINT);
    if(pvngi2 == NULL)
    {
      printf("SHoundDNAFromTaxIDIII failed.\n");
      logWrite(reportLog, "SHoundDNAFromTaxIDIII", "TAXID", "MAXINT", "Pointer", "NULL");
    }
    else
    {
      printf("SHoundDNAFromTaxIDIII OK.\n");
       testFlag = 0;
      logWrite(reportLog, "SHoundDNAFromTaxIDIII", "TAXID", "MAXINT", "Pointer", "NOTNULL");
    }
    pvngi2 = ValNodeFree(pvngi2);
    /* Invalid Param TAXID = MINUSONE */
    pvngi2 = SHoundDNAFromTaxIDIII(MINUSONE);
    if(pvngi2 == NULL)
    {
      printf("SHoundDNAFromTaxIDIII failed.\n");
      logWrite(reportLog, "SHoundDNAFromTaxIDIII", "TAXID", "-1", "Pointer", "NULL");
    }
    else
    {
      printf("SHoundDNAFromTaxIDIII OK.\n");
      testFlag = 0;
      logWrite(reportLog, "SHoundDNAFromTaxIDIII", "TAXID", "-1", "Pointer", "NOTNULL");
    }
     if ( testFlag = 1)
     {
           logWrite(summaryLog, "SHoundDNAFromTaxIDIII", "PASS", "", "", "");

     }
     else
     {
           logWrite(summaryLog, "SHoundDNAFromTaxIDIII", "FAIL", "", "", "");

     }
    pvngi2 = ValNodeFree(pvngi2);
    
    
/*
    if (pvntaxid != NULL)
    {
      pvntaxid = ValNodeFree(pvntaxid);
    }
*/
    /* make sure pvntaxid has valid contents */
    ValNodeAddInt(&pvngi5, 0, 13557);
    ValNodeAddInt(&pvngi5, 0, 13557);
    ValNodeAddInt(&pvngi5, 0, 13557);
    pvntaxid = SHoundTaxIDFromGiList(pvngi5);
    pvngi2 = SHoundProteinsFromTaxIDList(pvntaxid);
    if(pvngi2 == NULL)
    {
      printf("SHoundProteinsFromTaxIDList failed.\n");
       testFlag = 0;
      logWrite(reportLog, "SHoundProteinsFromTaxIDList", "ValNodePtr", "NOTNULL", "Pointer", "NULL");
    }
    else
    {
      printf("SHoundProteinsFromTaxIDList OK.\n");
       testFlag = 1;
      logWrite(reportLog, "SHoundProteinsFromTaxIDList", "ValNodePtr", "NOTNULL", "Pointer", "NOTNULL");
    }
    pvngi2 = ValNodeFree(pvngi2);
    pvngi5 = ValNodeFree(pvngi5);

    /*INvalid Param */
    pvngi2 = SHoundProteinsFromTaxIDList(nullValNodePtr);
    if(pvngi2 == NULL)
    {
      printf("SHoundProteinsFromTaxIDList failed.\n");
      logWrite(reportLog, "SHoundProteinsFromTaxIDList", "ValNodePtr", "NULL", "Pointer", "NULL");
    }
    else
    {
      printf("SHoundProteinsFromTaxIDList OK.\n");
       testFlag = 0;
      logWrite(reportLog, "SHoundProteinsFromTaxIDList", "ValNodePtr", "NULL", "Pointer", "NOTNULL");
    }
     if ( testFlag = 1)
     {
           logWrite(summaryLog, "SHoundProteinsFromTaxIDList", "PASS", "", "", "");

     }
     else
     {
           logWrite(summaryLog, "SHoundProteinsFromTaxIDList", "FAIL", "", "", "");

     }
    pvngi2 = ValNodeFree(pvngi2);


    pvngi2 = SHoundDNAFromTaxIDList(pvntaxid);
    if(pvngi2 == NULL)
    {
       printf("SHoundDNAFromTaxIDList failed.\n");
       testFlag = 0;
       logWrite(reportLog, "SHoundDNAFromTaxIDList", "ValNodePtr", "NOTNULL", "Pointer", "NULL");
    }
    else
    {
       printf("SHoundDNAFromTaxIDList OK.\n");
       testFlag = 1;
       logWrite(reportLog, "SHoundDNAFromTaxIDList", "ValNodePtr", "NOTNULL", "Pointer", "NOTNULL");
    }
    pvngi2 = ValNodeFree(pvngi2);
  
    /* Invalid Param */
    pvngi2 = SHoundDNAFromTaxIDList(nullValNodePtr);
    if(pvngi2 == NULL)
    {
       printf("SHoundDNAFromTaxIDList failed.\n");
       logWrite(reportLog, "SHoundDNAFromTaxIDList", "ValNodePtr", "NULL", "Pointer", "NULL");
    }
    else
    {
       printf("SHoundDNAFromTaxIDList OK.\n");
       testFlag = 0;
       logWrite(reportLog, "SHoundDNAFromTaxIDList", "ValNodePtr", "NOTNULL", "Pointer", "NOTNULL");
    }
     if ( testFlag = 1)
     {
           logWrite(summaryLog, "SHoundDNAFromTaxIDList", "PASS", "", "", "");

     }
     else
     {
           logWrite(summaryLog, "SHoundDNAFromTaxIDList", "FAIL", "", "", "");

     }
    pvngi2 = ValNodeFree(pvngi2);


    pse = SHoundGetSeqEntry(myargs[0].intvalue);
    sprintf(itos1, "%ld", myargs[0].intvalue);
    if (pse == NULL)
    {
      printf("SHoundGetSeqEntry failed.\n");
       testFlag = 0;
      logWrite(reportLog, "SHoundGetSeqEntry", "GI", itos1, "Pointer", "NULL");
    }
    else
    {
      printf("SHoundGetSeqEntry OK.\n");
       testFlag = 1;
      logWrite(reportLog, "SHoundGetSeqEntry", "GI", itos1, "Pointer", "NOTNULL");
    }
    pse = SeqEntryFree(pse);

    /* Invalid Param GI = MAXINT */
    pse = SHoundGetSeqEntry(MAXINT);
    if (pse == NULL)
    {
      printf("SHoundGetSeqEntry failed.\n");
      logWrite(reportLog, "SHoundGetSeqEntry", "GI", "MAXINT", "Pointer", "NULL");
    }
    else
    {
      printf("SHoundGetSeqEntry OK.\n");
       testFlag = 0;
      logWrite(reportLog, "SHoundGetSeqEntry", "GI", "MAXINT", "Pointer", "NOTNULL");
    }
    pse = SeqEntryFree(pse);
    /* Invalid Param GI = MINUSONE */
    pse = SHoundGetSeqEntry(MINUSONE);
    if (pse == NULL)
    {
      printf("SHoundGetSeqEntry failed.\n");
      logWrite(reportLog, "SHoundGetSeqEntry", "GI", "-1", "Pointer", "NULL");
    }
    else
    {
      printf("SHoundGetSeqEntry OK.\n");
       testFlag = 0;
      logWrite(reportLog, "SHoundGetSeqEntry", "GI", "-1", "Pointer", "NOTNULL");
    }
     if ( testFlag = 1)
     {
           logWrite(summaryLog, "SHoundGetSeqEntry", "PASS", "", "", "");

     }
     else
     {
           logWrite(summaryLog, "SHoundGetSeqEntry", "FAIL", "", "", "");

     }
    pse = SeqEntryFree(pse);


    pvnbs = SHoundGetSeqEntryList(pvngi);
    if (pvnbs == NULL)
    {
      printf("SHoundSeqEntryList failed.\n");
      testFlag = 0;
      logWrite(reportLog, "SHoundGetSeqEntryList", "ValNodePtr", "NOTNULL", "Pointer", "NULL");
    }
    else
    {
      printf("SHoundGetSeqEntryList OK\n");
       testFlag = 1;
      logWrite(reportLog, "SHoundGetSeqEntryList", "ValNodePtr", "NOTNULL", "Pointer", "NOTNULL");
    }
    pvnbs = FreeSeqEntryList(pvnbs);

    /* Invalid  param */
     pvnbs = SHoundGetSeqEntryList(nullValNodePtr);
    if (pvnbs == NULL)
    {
      printf("SHoundSeqEntryList failed.\n");
      logWrite(reportLog, "SHoundGetSeqEntryList", "ValNodePtr", "NULL", "Pointer", "NULL");
    }
    else
    {
      printf("SHoundGetSeqEntryList OK\n");
       testFlag = 0;
      logWrite(reportLog, "SHoundGetSeqEntryList", "ValNodePtr", "NULL", "Pointer", "NOTNULL");
    }
     if ( testFlag = 1)
     {
           logWrite(summaryLog, "SHoundGetSeqEntryList", "PASS", "", "", "");

     }
     else
     {
           logWrite(summaryLog, "SHoundGetSeqEntryList", "FAIL", "", "", "");

     }
    pvnbs = FreeSeqEntryList(pvnbs);
    
    gi = 2791836; 
    pbss = SHoundGetBioseqSet(gi);
    sprintf(itos1, "%ld", gi);
    if (pbss == NULL)
    {
      printf("SHoundBioseqSet failed.\n");
       testFlag = 0;
      logWrite(reportLog, "SHoundGetBioseqSet", "GI", itos1, "Pointer", "NULL");
    }
    else
    {
      printf("SHoundGetBioseqSet OK.\n");
       testFlag = 1;
      logWrite(reportLog, "SHoundGetBioseqSet", "GI", itos1, "Pointer", "NOTNULL");
    }
    pbss = BioseqSetFree(pbss);

    /* Invalid Param GI = MAXINT */
    pbss = SHoundGetBioseqSet(MAXINT);
    if (pbss == NULL)
    {
      printf("SHoundBioseqSet failed.\n");
      logWrite(reportLog, "SHoundGetBioseqSet", "GI", "MAXINT", "Pointer", "NULL");
    }
    else
    {
      printf("SHoundGetBioseqSet OK.\n");
       testFlag = 0;
      logWrite(reportLog, "SHoundGetBioseqSet", "GI", "MAXINT", "Pointer", "NOTNULL");
    }
    pbss = BioseqSetFree(pbss);
    
    /* Invalid Param GI = MINUSONE */
    pbss = SHoundGetBioseqSet(MINUSONE);
    if (pbss == NULL)
    {
      printf("SHoundBioseqSet failed.\n");
      logWrite(reportLog, "SHoundGetBioseqSet", "GI", "-1", "Pointer", "NULL");
    }
    else
    {
      printf("SHoundGetBioseqSet OK.\n");
       testFlag = 0;
      logWrite(reportLog, "SHoundGetBioseqSet", "GI", "-1", "Pointer", "NULL");
    }
     if ( testFlag = 1)
     {
           logWrite(summaryLog, "SHoundGetBioseqSet", "PASS", "", "", "");

     }
     else
     {
           logWrite(summaryLog, "SHoundGetBioseqSet", "FAIL", "", "", "");

     }
    pbss = BioseqSetFree(pbss);


    ValNodeAddInt(&pvngi4, 0, 2791836);
    ValNodeAddInt(&pvngi4, 0, 2791836);
    pvnbs = SHoundGetBioseqSetList(pvngi4);
    if (pvnbs == NULL)
    {
      printf("SHoundGetBioseqSetList failed.\n");
       testFlag = 0;
      logWrite(reportLog, "SHoundGetBioseqSetList", "ValNodePtr", "NOTNULL", "Pointer", "NULL");
    }
    else
    {
      printf("SHoundGetBioseqSetList OK.\n");
       testFlag = 1;
      logWrite(reportLog, "SHoundGetBioseqSetList", "ValNodePtr", "NOTNULL", "Pointer", "NOTNULL");
    }
    pvnbs = FreeBioseqSetList(pvnbs);

    /* Invalid param */
    pvnbs = SHoundGetBioseqSetList(nullValNodePtr);
    if (pvnbs == NULL)
    {
      printf("SHoundGetBioseqSetList failed.\n");
      logWrite(reportLog, "SHoundGetBioseqSetList", "ValNodePtr", "NULL", "Pointer", "NULL");
    }
    else
    {
      printf("SHoundGetBioseqSetList OK.\n");
       testFlag = 0;
      logWrite(reportLog, "SHoundGetBioseqSetList", "ValNodePtr", "NULL", "Pointer", "NOTNULL");
    }
     if ( testFlag = 1)
     {
           logWrite(summaryLog, "SHoundGetBioseqSetList", "PASS", "", "", "");

     }
     else
     {
           logWrite(summaryLog, "SHoundGetBioseqSetList", "FAIL", "", "", "");

     }
    pvnbs = FreeBioseqSetList(pvnbs);


    pfile = FileOpen("flat", "w");
    sprintf(itos1, "%ld", myargs[0].intvalue);
    if(!SHoundGetGenBankff(myargs[0].intvalue, pfile))
    {
      printf("SHoundGetGenBankff failed.\n");
       testFlag = 0;
      logWrite(reportLog, "SHoundGetGenBankff", "GI", itos1, "BOOLEAN", "FALSE");
    }
    else
    {
      printf("SHoundGetGenBankff OK.\n");
       testFlag = 1;
      logWrite(reportLog, "SHoundGetGenBankff", "GI", itos1, "BOOLEAN", "TRUE");
    }
    FileClose(pfile);

    /* Invalid param GI = MAXINT */
    pfile = FileOpen("flat", "w");
    if(!SHoundGetGenBankff(MAXINT, pfile))
    {
      printf("SHoundGetGenBankff failed.\n");
      logWrite(reportLog, "SHoundGetGenBankff", "GI", "MAXINT", "BOOLEAN", "FALSE");
    }
    else
    {
      printf("SHoundGetGenBankff OK.\n");
       testFlag = 0;
      logWrite(reportLog, "SHoundGetGenBankff", "GI", "MAXINT", "BOOLEAN", "TRUE");
    }
    FileClose(pfile);

    /* Invalid param GI = MINUSONE */
    pfile = FileOpen("flat", "w");
    if(!SHoundGetGenBankff(MINUSONE, pfile))
    {
      printf("SHoundGetGenBankff failed.\n");
      logWrite(reportLog, "SHoundGetGenBankff", "GI", "-1", "BOOLEAN", "FALSE");
    }
    else
    {
      printf("SHoundGetGenBankff OK.\n");
       testFlag = 0;
      logWrite(reportLog, "SHoundGetGenBankff", "GI", "-1", "BOOLEAN", "TRUE");
    }
     if ( testFlag = 1)
     {
           logWrite(summaryLog, "SHoundGetGenBankff", "PASS", "", "", "");

     }
     else
     {
           logWrite(summaryLog, "SHoundGetGenBankff", "FAIL", "", "", "");

     }
    FileClose(pfile);

    pfile = FileOpen("flat", "w");
    if(!SHoundGetGenBankffList(pvngi, pfile))
    {
      printf("SHoundGetGenBankffList failed.\n");
       testFlag = 0;
      logWrite(reportLog, "SHoundGetGenBankffList", "ValNodePtr", "NOTNULL", "BOOLEAN", "FALSE");
    }
    else
    {
      printf("SHoundGetGenBankffList OK.\n");
       testFlag = 1;
      logWrite(reportLog, "SHoundGetGenBankffList", "ValNodePtr", "NOTNULL", "BOOLEAN", "TRUE");
    }
    FileClose(pfile);

    /* Invalid param */
    pfile = FileOpen("flat", "w");
    if(!SHoundGetGenBankffList(nullValNodePtr, pfile))
    {
      printf("SHoundGetGenBankffList failed.\n");
      logWrite(reportLog, "SHoundGetGenBankffList", "ValNodePtr", "NULL", "BOOLEAN", "FALSE");
    }
    else
    {
      printf("SHoundGetGenBankffList OK.\n");
       testFlag = 0;
      logWrite(reportLog, "SHoundGetGenBankffList", "ValNodePtr", "NULL", "BOOLEAN", "TRUE");
    }
     if ( testFlag = 1)
     {
           logWrite(summaryLog, "SHoundGetGenBankffList", "PASS", "", "", "");

     }
     else
     {
           logWrite(summaryLog, "SHoundGetGenBankffList", "FAIL", "", "", "");

     }
    FileClose(pfile);


    sprintf(itos1, "%ld", myargs[0].intvalue);
    if(!SHoundGetXMLBioseq(myargs[0].intvalue, "xmlbioseq"))
    {
      printf("SHoundGetXMLBioseq failed.\n");
       testFlag = 0;
      logWrite(reportLog, "SHoundGetXMLBioseq", "GI", itos1, "BOOLEAN", "FALSE");
    }
    else
    {
      printf("SHoundGetXMLBioseq OK.\n");
       testFlag = 1;
      logWrite(reportLog, "SHoundGetXMLBioseq", "GI", itos1, "BOOLEAN", "TRUE");
      
    }
 
    /* Invalid Param GI = MAXINT */
    if(!SHoundGetXMLBioseq(MAXINT, "xmlbioseq"))
    {
      printf("SHoundGetXMLBioseq failed.\n");
      logWrite(reportLog, "SHoundGetXMLBioseq", "GI", "MAXINT", "BOOLEAN", "FALSE");
    }
       
    else
    {
      printf("SHoundGetXMLBioseq OK.\n");
       testFlag = 0;
      logWrite(reportLog, "SHoundGetXMLBioseq", "GI", "MAXINT", "BOOLEAN", "TRUE");
    }
    /* Invalid Param GI = MINUSONE */
    if(!SHoundGetXMLBioseq(MINUSONE, "xmlbioseq"))
    {
      printf("SHoundGetXMLBioseq failed.\n");
      logWrite(reportLog, "SHoundGetXMLBioseq", "GI", "-1", "BOOLEAN", "FALSE");
    }
    else
    {
      printf("SHoundGetXMLBioseq OK.\n");
       testFlag = 0;
      logWrite(reportLog, "SHoundGetXMLBioseq", "GI", "-1", "BOOLEAN", "TRUE");
    }
     if ( testFlag = 1)
     {
           logWrite(summaryLog, "SHoundGetXMLBioseq", "PASS", "", "", "");

     }
     else
     {
           logWrite(summaryLog, "SHoundGetXMLBioseq", "FAIL", "", "", "");

     }

    sprintf(itos1, "%ld", myargs[0].intvalue);
    if(!SHoundGetXMLSeqEntry(myargs[0].intvalue, "xmlseqentry"))
    {
      printf("SHoundGetXMLSeqEntry failed.\n");
       testFlag = 0;
      logWrite(reportLog, "SHoundGetXMLSeqEntry", "GI", itos1, "BOOLEAN", "FALSE");
    }
    else
    {
      printf("SHoundGetXMLSeqEntry OK.\n");
       testFlag = 1;
      logWrite(reportLog, "SHoundGetXMLSeqEntry", "GI", itos1, "BOOLEAN", "TRUE");
    }

    /* Invalid Param GI = MAXINT */
    if(!SHoundGetXMLSeqEntry(MAXINT, "xmlseqentry"))
    {
      printf("SHoundGetXMLSeqEntry failed.\n");
      logWrite(reportLog, "SHoundGetXMLSeqEntry", "GI", "MAXINT", "BOOLEAN", "FALSE");
    }
    else
    {
      printf("SHoundGetXMLSeqEntry OK.\n");
       testFlag = 0;
      logWrite(reportLog, "SHoundGetXMLSeqEntry", "GI", "MAXINT", "BOOLEAN", "TRUE");
    }
    /* Invalid Param GI = MINUSONE */
    if(!SHoundGetXMLSeqEntry(MINUSONE, "xmlseqentry"))
    {
      printf("SHoundGetXMLSeqEntry failed.\n");
      logWrite(reportLog, "SHoundGetXMLSeqEntry", "GI", "-1", "BOOLEAN", "FALSE");
    }
    else
    {
      printf("SHoundGetXMLSeqEntry OK.\n");
       testFlag = 0;
      logWrite(reportLog, "SHoundGetXMLSeqEntry", "GI", "-1", "BOOLEAN", "TRUE");
    }
     if ( testFlag = 1)
     {
           logWrite(summaryLog, "SHoundGetXMLSeqEntry", "PASS", "", "", "");

     }
     else
     {
           logWrite(summaryLog, "SHoundGetXMLSeqEntry", "FAIL", "", "", "");

     }

      intValue = 7612; /* valid GO ID */ 
      sprintf(itos1, "%ld", intValue);
      vnp = SHoundGODBGetChildrenOf(intValue);
      if (vnp == NULL)
      {
	  printf("SHoundGODBGetChildrenOf failed.\n");
        testFlag = 0;
        logWrite(reportLog, "SHoundGODBGetChildrenOf", "GOID", itos1, "Pointer", "NULL");
      }
      else
      {
        printf("SHoundGODBGetChildrenOf OK.\n");
        testFlag = 1;
	  logWrite(reportLog, "SHoundGODBGetChildrenOf", "GOID", itos1, "Pointer", "NOTNULL");
      }
	vnp = ValNodeFree(vnp);
      /* invalid GO ID = MAXINT */ 
	vnp = SHoundGODBGetChildrenOf(MAXINT);
	if (vnp == NULL)
      {
	  printf("SHoundGODBGetChildrenOf failed.\n");
	  logWrite(reportLog, "SHoundGODBGetChildrenOf", "GOID", "MAXINT", "Pointer", "NULL");
      }
      else
      {
        printf("SHoundGODBGetChildrenOf OK.\n");
        testFlag = 0;
	  logWrite(reportLog, "SHoundGODBGetChildrenOf", "GOID", "MAXINT", "Pointer", "NOTNULL");
      }
	vnp = ValNodeFree(vnp);
      /* invalid GO ID = MINUSONE */ 
	vnp = SHoundGODBGetChildrenOf(MINUSONE);
	if (vnp == NULL)
      {
	  printf("SHoundGODBGetChildrenOf failed.\n");
	  logWrite(reportLog, "SHoundGODBGetChildrenOf", "GOID", "-1", "Pointer", "NULL");
      }
      else
      {
        printf("SHoundGODBGetChildrenOf OK.\n");
        testFlag = 0;
	  logWrite(reportLog, "SHoundGODBGetChildrenOf", "GOID", "-1", "Pointer", "NOTNULL");
      }
     if ( testFlag = 1)
     {
             logWrite(summaryLog, "SHoundGODBGetChildrenOf", "PASS", "", "", "");

     }
     else
     {
             logWrite(summaryLog, "SHoundGODBGetChildrenOf", "FAIL", "", "", "");

     }
	vnp = ValNodeFree(vnp);

      intValue = 5381; /* valid GO ID = 5381 */
      sprintf(itos1, "%ld", intValue);
      vnp = SHoundGODBGetParentOf(intValue);
      if (vnp == NULL)
      {
	  printf("SHoundGODBGetParentOf failed.\n");
        testFlag = 0;
        logWrite(reportLog, "SHoundGODBGetParentOf", "GOID", itos1, "Pointer", "NULL");
      }
      else
      {
        printf("SHoundGODBGetParentOf OK.\n");
        testFlag = 1;
	  logWrite(reportLog, "SHoundGODBGetParentOf", "GOID", itos1, "Pointer", "NOTNULL");
      }
	vnp = ValNodeFree(vnp);

      /* invalid GO ID = MAXINT */
	vnp = SHoundGODBGetParentOf(MAXINT);
	if (vnp == NULL)
      {
	  printf("SHoundGODBGetParentOf failed.\n");
	  logWrite(reportLog, "SHoundGODBGetParentOf", "GOID", "MAXINT", "Pointer", "NULL");
      }
      else
      {
        printf("SHoundGODBGetParentOf OK.\n");
        testFlag = 0;
	  logWrite(reportLog, "SHoundGODBGetParentOf", "GOID", itos1, "Pointer", "NOTNULL");
      }
	vnp = ValNodeFree(vnp);
      /* invalid GO ID = MAXINT */
	vnp = SHoundGODBGetParentOf(MINUSONE);
	if (vnp == NULL)
      {
	  printf("SHoundGODBGetParentOf failed.\n");
	  logWrite(reportLog, "SHoundGODBGetParentOf", "GOID", "-1", "Pointer", "NULL");
      }
      else
      {
        printf("SHoundGODBGetParentOf OK.\n");
        testFlag = 0;
	  logWrite(reportLog, "SHoundGODBGetParentOf", "GOID", "-1", "Pointer", "NOTNULL");
      }
     if ( testFlag = 1)
     {
             logWrite(summaryLog, "SHoundGODBGetParentOf", "PASS", "", "", "");

     }
     else
     {
             logWrite(summaryLog, "SHoundGODBGetParentOf", "FAIL", "", "", "");

     }
	vnp = ValNodeFree(vnp);
	intValue = 7625; /* valid GO ID */
	sprintf(itos1, "%ld", intValue);
	pchar = SHoundGODBGetNameByID(intValue);
	if(pchar == NULL)
	{
        printf("SHoundGODBGetNameByID failed.\n");
        testFlag = 0;
	  logWrite(reportLog, "SHoundGODBGetNameByID", "GOID", itos1, "Pointer", "NULL");
	}
	else
	{
        printf("SHoundGODBGetNameByID OK.\n");
        testFlag = 1;
	  logWrite(reportLog, "SHoundGODBGetNameByID", "GOID", itos1, "Pointer", "NOTNULL");
	}
	pchar = MemFree(pchar);
	/* invalid GO ID= MAXINT */
	pchar = SHoundGODBGetNameByID(MAXINT);
	if(pchar == NULL)
	{
        printf("SHoundGODBGetNameByID failed.\n");
	  logWrite(reportLog, "SHoundGODBGetNameByID", "GOID", "MAXINT", "Pointer", "NULL");
	}
	else
	{
        printf("SHoundGODBGetNameByID OK.\n");
        testFlag = 0;
	  logWrite(reportLog, "SHoundGODBGetNameByID", "GOID", "MAXINT", "Pointer", "NOTNULL");
	}
	pchar = MemFree(pchar);
	/* invalid GO ID= MINUSONE */
	pchar = SHoundGODBGetNameByID(MINUSONE);
	if(pchar == NULL)
	{
        printf("SHoundGODBGetNameByID failed.\n");
	  logWrite(reportLog, "SHoundGODBGetNameByID", "GOID", "-1", "Pointer", "NULL");
	}
	else
	{
        printf("SHoundGODBGetNameByID OK.\n");
        testFlag = 0;
	  logWrite(reportLog, "SHoundGODBGetNameByID", "GOID", "-1", "Pointer", "NOTNULL");
	}
     if ( testFlag = 1)
     {
             logWrite(summaryLog, "SHoundGODBGetNameByID", "PASS", "", "", "");

     }
     else
     {
             logWrite(summaryLog, "SHoundGODBGetNameByID", "FAIL", "", "", "");

     }
	pchar = MemFree(pchar);

 	intValue = 7627; /* valid GO ID */
	sprintf(itos1, "%ld", intValue);
	vnp2 = SHoundGODBGetAllAncestors(intValue);
	if (vnp2 == NULL)
	{
	  printf("SHoundGODBGetAllAncestors failed.\n");
        testFlag = 0;
	  logWrite(reportLog, "SHoundGODBGetAllAncestors", "GOID", itos1, "Pointer", "NULL");
	}
        else
	{
	  printf("SHoundGODBGetAllAncestors OK.\n");
        testFlag = 1;
	  logWrite(reportLog, "SHoundGODBGetAllAncestors", "GOID", itos1, "Pointer", "NOTNULL");
	}
	vnp2 = ValNodeFree(vnp2);
 	/* invalid GO ID = MAXINT */
	vnp2 = SHoundGODBGetAllAncestors(MAXINT);
	if (vnp2 == NULL)
	{
	  printf("SHoundGODBGetAllAncestors failed.\n");
	  logWrite(reportLog, "SHoundGODBGetAllAncestors", "GOID", "MAXINT", "Pointer", "NULL");
	}
        else
	{
	  printf("SHoundGODBGetAllAncestors OK.\n");
        testFlag = 0;
	  logWrite(reportLog, "SHoundGODBGetAllAncestors", "GOID", "MAXINT", "Pointer", "NOTNULL");
	}
	vnp2 = ValNodeFree(vnp2);
 	/* invalid GO ID = MINUSONE */
	vnp2 = SHoundGODBGetAllAncestors(MINUSONE);
	if (vnp2 == NULL)
	{
	  printf("SHoundGODBGetAllAncestors failed.\n");
	  logWrite(reportLog, "SHoundGODBGetAllAncestors", "GOID", "-1", "Pointer", "NULL");
	}
        else
	{
	  printf("SHoundGODBGetAllAncestors OK.\n");
        testFlag = 0;
	  logWrite(reportLog, "SHoundGODBGetAllAncestors", "GOID", "-1", "Pointer", "NOTNULL");
	}
     if ( testFlag = 1)
     {
             logWrite(summaryLog, "SHoundGODBGetAllAncestors", "PASS", "", "", "");

     }
     else
     {
             logWrite(summaryLog, "SHoundGODBGetAllAncestors", "FAIL", "", "", "");

     }
	vnp2 = ValNodeFree(vnp2);
	intValue = 4867; /* valid GO ID */ 
	sprintf(itos1, "%ld", intValue);
	vnp2 = SHoundGODBGetAllChildren(intValue);
	if (vnp2 == NULL)
      {
	  printf("SHoundGODBGetAllChildren failed.\n");
        testFlag = 0;
	  logWrite(reportLog, "SHoundGODBGetAllChildren", "GOID", itos1, "Pointer", "NULL");
      }
      else
      {
         printf("SHoundGODBGetAllChildren OK.\n");
        testFlag = 1;
	   logWrite(reportLog, "SHoundGODBGetAllChildren", "GOID", itos1, "Pointer", "NOTNULL");
      }
	vnp2 = ValNodeFree(vnp2);
	/* invalid GO ID = MAXINT */ 
	vnp2 = SHoundGODBGetAllChildren(MAXINT);
	if (vnp2 == NULL)
      {
	  printf("SHoundGODBGetAllChildren failed.\n");
	  logWrite(reportLog, "SHoundGODBGetAllChildren", "GOID", "MAXINT", "Pointer", "NULL");
      }
      else
      {
         printf("SHoundGODBGetAllChildren OK.\n");
        testFlag = 0;
	   logWrite(reportLog, "SHoundGODBGetAllChildren", "GOID", "MAXINT", "Pointer", "NOTNULL");
      }
	vnp2 = ValNodeFree(vnp2);
	/* invalid GO ID = MINUSONE */ 
	vnp2 = SHoundGODBGetAllChildren(MINUSONE);
	if (vnp2 == NULL)
      {
	  printf("SHoundGODBGetAllChildren failed.\n");
	  logWrite(reportLog, "SHoundGODBGetAllChildren", "GOID", "-1", "Pointer", "NULL");
      }
      else
      {
         printf("SHoundGODBGetAllChildren OK.\n");
        testFlag = 0;
	   logWrite(reportLog, "SHoundGODBGetAllChildren", "GOID", "-1", "Pointer", "NOTNULL");
      }
     if ( testFlag = 1)
     {
             logWrite(summaryLog, "SHoundGODBGetAllChildren", "PASS", "", "", "");

     }
     else
     {
             logWrite(summaryLog, "SHoundGODBGetAllChildren", "FAIL", "", "", "");

     }
      vnp2 = ValNodeFree(vnp2);

      vnp2 = SHoundGODBGetRecordByReference("EC");
      if (vnp2 == NULL)
      {
	  printf("SHoundGODBGetRecordByReference failed.\n");
        testFlag = 0;
    	  logWrite(reportLog, "SHoundGODBGetRecordByReference", "Reference", "EC", "Pointer", "NULL");
      }
      else
      {
	  printf("SHoundGODBGetRecordByReference OK.\n");
        testFlag = 1;
	  logWrite(reportLog, "SHoundGODBGetRecordByReference", "Reference", "EC", "Pointer", "NOTNULL");
      }
	vnp2 = ValNodeFree(vnp2);
      /* Invalid Param */ 
	vnp2 = SHoundGODBGetRecordByReference(EMPTYSTR);
	if (vnp2 == NULL)
      {
	  printf("SHoundGODBGetRecordByReference failed.\n");
	  logWrite(reportLog, "SHoundGODBGetRecordByReference", "Reference", "EMPTYSTRING", "Pointer", "NULL");
      }
      else
      {
        printf("SHoundGODBGetRecordByReference OK.\n");
        testFlag = 0;
	  logWrite(reportLog, "SHoundGODBGetRecordByReference", "Reference", "EMPTYSTRING", "Pointer", "NOTNULL");
      }
	vnp2 = ValNodeFree(vnp2);
      /* Invalid Param */ 
	vnp2 = SHoundGODBGetRecordByReference(nullString);
	if (vnp2 == NULL)
      {
	  printf("SHoundGODBGetRecordByReference failed.\n");
	  logWrite(reportLog, "SHoundGODBGetRecordByReference", "Reference", "NULLSTRING", "Pointer", "NULL");
      }
      else
      {
        printf("SHoundGODBGetRecordByReference OK.\n");
        testFlag = 0;
	   logWrite(reportLog, "SHoundGODBGetRecordByReference", "Reference", "NULLSTRING", "Pointer", "NOTNULL");
      }
     if ( testFlag = 1)
     {
             logWrite(summaryLog, "SHoundGODBGetRecordByReference", "PASS", "", "", "");

     }
     else
     {
             logWrite(summaryLog, "SHoundGODBGetRecordByReference", "FAIL", "", "", "");

     }
	vnp2 = ValNodeFree(vnp2);

	vnp2 = SHoundGODBGetRecordByReference("ISBN");
	if (vnp2 == NULL)
	  printf("SHoundGODBGetRecordByReference failed.\n");
        else
          printf("SHoundGODBGetRecordByReference OK.\n");
	vnp2 = ValNodeFree(vnp2);

      intValue = 5935; /* valid GO ID */ 
      sprintf(itos1, "%ld", intValue);
      pi = SHoundGODBGetClassification(intValue);
      if(pi == NULL)
      {
	  printf("SHoundGODBGetClassification failed.\n");
        testFlag = 0;
    	  logWrite(reportLog, "SHoundGODBGetClassification", "GOID", itos1, "Pointer", "NULL");
      }
      else
      {
        printf("SHoundGODBGetClassification OK.\n");
        testFlag = 1;
	  logWrite(reportLog, "SHoundGODBGetClassification", "GOID", itos1, "Pointer", "NOTNULL");
        MemFree (pi);
        pi = NULL;
      }
 	/* invalid GO ID = MAXINT */ 	
	pi = SHoundGODBGetClassification(MAXINT);
	if(pi == NULL)
      {
	  printf("SHoundGODBGetClassification failed.\n");
	  logWrite(reportLog, "SHoundGODBGetClassification", "GOID", "MAXINT", "Pointer", "NULL");
      }
      else
      {
        printf("SHoundGODBGetClassification OK.\n");
        testFlag = 0;
	  logWrite(reportLog, "SHoundGODBGetClassification", "GOID", "MAXINT", "Pointer", "NOTNULL");
	  MemFree (pi);
	  pi = NULL;
      }

 	/* invalid GO ID = MINUSONE */ 	
	pi = SHoundGODBGetClassification(MINUSONE);
	if(pi == NULL)
      {
	  printf("SHoundGODBGetClassification failed.\n");
	  logWrite(reportLog, "SHoundGODBGetClassification", "GOID", "-1", "Pointer", "NULL");
      }
      else
      {
        printf("SHoundGODBGetClassification OK.\n");
        testFlag = 0;
	  logWrite(reportLog, "SHoundGODBGetClassification", "GOID", "-1", "Pointer", "NOTNULL");
        MemFree (pi);
	  pi = NULL;
      }
     if ( testFlag = 1)
     {
             logWrite(summaryLog, "SHoundGODBGetClassification", "PASS", "", "", "");

     }
     else
     {
             logWrite(summaryLog, "SHoundGODBGetClassification", "FAIL", "", "", "");

     }

	ValNodeAddInt(&lllist, 0, 1);
	ValNodeAddInt(&lllist, 0, 2);
	ValNodeAddInt(&lllist, 0, 13);

	ValNodeAddInt(&goList, 0, 8033);
	ValNodeAddInt(&goList, 0, 4867);
	ValNodeAddInt(&goList, 0, 6419);

	ValNodeAddInt(&omimList, 0, 137150);
	ValNodeAddInt(&omimList, 0, 103950);
	ValNodeAddInt(&omimList, 0, 108345);

	ValNodeCopyStr(&cddidlist, 0, "pfam00202");
	ValNodeCopyStr(&cddidlist, 0, "pfam01835");
	ValNodeCopyStr(&cddidlist, 0, "pfam00079");

	/* ---------------------- 
	   test SHoundGOIDFromLLID 
	   ----------------------*/
        intValue = 1; /*a valid locus link id*/
        sprintf(itos1, "%ld", intValue);
        vnpi = SHoundGOIDFromLLID(intValue);
        if(vnpi == NULL)
        {
	    printf("SHoundGOIDFromLLID failed.\n");
            testFlag = 0;
    	    logWrite(reportLog, "SHoundGOIDFromLLID", "LLID", itos1, "Pointer", "NULL");
        }
	else
        {
            printf("SHoundGOIDFromLLID OK.\n");
            testFlag = 1;
	    logWrite(reportLog, "SHoundGOIDFromLLID", "LLID", itos1, "Pointer", "NOTNULL");
        } 
        vnpi = ValNodeFree(vnpi);

        intValue = MAXINT; /* invalid param */ 
        vnpi = SHoundGOIDFromLLID(intValue);
	if(vnpi == NULL)
        {
	    printf("SHoundGOIDFromLLID gets the expected result.\n");
	    logWrite(reportLog, "SHoundGOIDFromLLID", "LLID", "MAXINT", "Pointer", "NULL");
        }
	else
        {
            printf("SHoundGOIDFromLLID fails to get the expected result.\n");
            testFlag = 0;
	    logWrite(reportLog, "SHoundGOIDFromLLID", "LLID", "MAXINT", "Pointer", "NOTNULL");
        } 
	vnpi = ValNodeFree(vnpi);
        
	/* since From LLID to GOID crosses LL-LLink and LL-GO Table, allow llid to be negative with the cureent implementation 
        intValue = MINUSONE; 
	vnpi = SHoundGOIDFromLLID(intValue);
	if(vnpi == NULL)
        {
	    printf("SHoundGOIDFromLLID failed.\n");
	    logWrite(reportLog, "SHoundGOIDFromLLID", "LLID", "-1", "Pointer", "NULL");
        }
	else
        {
            printf("SHoundGOIDFromLLID OK.\n");
            testFlag = 0;
	    logWrite(reportLog, "SHoundGOIDFromLLID", "LLID", "-1", "Pointer", "NOTNULL");
        } */
     if ( testFlag = 1)
     {
               logWrite(summaryLog, "SHoundGOIDFromLLID", "PASS", "", "", "");

     }
     else
     {
               logWrite(summaryLog, "SHoundGOIDFromLLID", "FAIL", "", "", "");

     }
	/*vnpi = ValNodeFree(vnpi);*/
        

        /* -----------------------
	   test SHoundOMIMFromLLID
	   -----------------------*/
        intValue = 1; /*a valid locus link id*/
	returnInt = 0;
        sprintf(itos1, "%ld", intValue);
        returnInt = SHoundOMIMFromLLID(intValue);
	sprintf(itos2, "%ld", returnInt);
	logWrite(reportLog, "SHoundOMIMFromLLID", "LLID", itos1, "OMIM", itos2);
        if(returnInt > 0 )
        {
	    printf("SHoundOMIMFromLLID OK.\n");
            testFlag = 1;
        }
	else
        {
            printf("SHoundOMIMFromLLID failed.\n");
            testFlag = 0;
        } 
        
        
	returnInt = 0;
        intValue = MAXINT; /* invalid param */ 
        returnInt = SHoundOMIMFromLLID(intValue);
	sprintf(itos2, "%ld", returnInt);
	logWrite(reportLog, "SHoundOMIMFromLLID", "LLID", "MAXINT", "OMIM", itos2);
	if(returnInt > 0 )
        {
	    printf("SHoundOMIMFromLLID fails to get the expected result\n");
            testFlag = 0;
        }
	else
        {
            printf("SHoundOMIMFromLLID gets the expected result\n");
        } 
      
        returnInt = 0;
        intValue = MINUSONE; /* invalid param */ 
	returnInt = SHoundOMIMFromLLID(intValue);
	sprintf(itos2, "%ld", returnInt);
	logWrite(reportLog, "SHoundOMIMFromLLID", "LLID", "-1", "OMIM", itos2);
	if(returnInt > 0 )
        {
	    printf("SHoundOMIMFromLLID fails to get the expected result\n");
            testFlag = 0;
        }
	else
        {
            printf("SHoundOMIMFromLLID gets the expected result.\n");
        } 
     if ( testFlag = 1)
     {
               logWrite(summaryLog, "SHoundOMIMFromLLID", "PASS", "", "", "");

     }
     else
     {
               logWrite(summaryLog, "SHoundOMIMFromLLID", "FAIL", "", "", "");

     }
	
	
	/* ------------------------
	   test SHoundCDDIDFromLLID 
	   ------------------------*/
	intValue = 1; /*a valid locus link id*/
        sprintf(itos1, "%ld", intValue);
        vnpi = SHoundCDDIDFromLLID(intValue);
        if(vnpi == NULL)
        {
	    printf("SHoundCDDIDFromLLID failed.\n");
            testFlag = 0;
    	    logWrite(reportLog, "SHoundCDDIDFromLLID", "LLID", itos1, "Pointer", "NULL");
        }
	else
        {
            printf("SHoundCDDIDFromLLID OK.\n");
            testFlag = 1;
	    logWrite(reportLog, "SHoundCDDIDFromLLID", "LLID", itos1, "Pointer", "NOTNULL");
        } 
        vnpi = ValNodeFreeData(vnpi);

        intValue = MAXINT; /* invalid param */ 
        vnpi = SHoundCDDIDFromLLID(intValue);
	if(vnpi == NULL)
        {
	    printf("SHoundCDDIDFromLLID gets the expected result.\n");
	    logWrite(reportLog, "SHoundCDDIDFromLLID", "LLID", "MAXINT", "Pointer", "NULL");
        }
	else
        {
            printf("SHoundCDDIDFromLLID fails to get the expected result.\n");
            testFlag = 0;
	    logWrite(reportLog, "SHoundCDDIDFromLLID", "LLID", "MAXINT", "Pointer", "NOTNULL");
        } 
	vnpi = ValNodeFreeData(vnpi);
        
	
        intValue = MINUSONE; 
	vnpi = SHoundCDDIDFromLLID(intValue);
	if(vnpi == NULL)
        {
	    printf("SHoundCDDIDFromLLID gets the expected result. \n");
	    logWrite(reportLog, "SHoundCDDIDFromLLID", "LLID", "-1", "Pointer", "NULL");
        }
	else
        {
            printf("SHoundCDDIDFromLLID fails to get the expected result.\n");
            testFlag = 0;
	    logWrite(reportLog, "SHoundCDDIDFromLLID", "LLID", "-1", "Pointer", "NOTNULL");
        } 
     if ( testFlag = 1)
     {
               logWrite(summaryLog, "SHoundCDDIDFromLLID", "PASS", "", "", "");

     }
     else
     {
               logWrite(summaryLog, "SHoundCDDIDFromLLID", "FAIL", "", "", "");

     }
	vnpi = ValNodeFreeData(vnpi);
        
	/* -------------------------------
	   test SHoundLLIDFromGOIDAndECode 
	   -------------------------------*/
	intValue = 6810;
        if (vnp != NULL) vnp = ValNodeFree(vnp);
        sprintf(itos1, "%ld", intValue);
        vnp = SHoundLLIDFromGOIDAndECode(intValue, "IEA");
        if (vnp == NULL)
        {
	    printf("SHoundLLIDFromGOIDAndECode failed.\n");
            testFlag = 0;
    	    logWrite(reportLog, "SHoundLLIDFromGOIDAndECode", "GOID/Ecode", itos1, "ValNodePtr", "NULL");
        }
        else
        {
	    printf("SHoundLLIDFromGOIDAndECode OK.\n");
            testFlag = 1;
	    logWrite(reportLog, "SHoundLLIDFromGOIDAndECode", "GOID/Ecode", itos1, "ValNodePtr", "NOTNULL");
        }
	vnp = ValNodeFree(vnp);
       
	vnp = SHoundLLIDFromGOIDAndECode(MAXINT,"ND");
	if (vnp == NULL)
        {
	    printf("SHoundLLIDFromGOIDAndECode gets the expected result.\n");
	    logWrite(reportLog, "SHoundLLIDFromGOIDAndECode", "GOID/Ecode", "MAXINT", "ValNodePtr", "NULL");
        }
        else
        {
            printf("SHoundLLIDFromGOIDAndECode fails to get the expected result.\n");
            testFlag = 0;
	    logWrite(reportLog, "SHoundLLIDFromGOIDAndECode", "GOID/Ecode", "MAXINT", "ValNodePtr", "NOTNULL");
        }
	vnp = ValNodeFree(vnp);
        
	vnp = SHoundLLIDFromGOIDAndECode(MINUSONE,"ND");
	if (vnp == NULL)
        {
	    printf("SHoundLLIDFromGOIDAndECode gets the expected result.\n");
	    logWrite(reportLog, "SHoundLLIDFromGOIDAndECode", "GOID/Ecode", "-1", "ValNodePtr", "NULL");
        }
        else
        {
            printf("SHoundLLIDFromGOIDAndECode fails to get the expected result.\n");
            testFlag = 0;
	   logWrite(reportLog, "SHoundLLIDFromGOIDAndECode", "GOID/Ecode", "-1", "ValNodePtr", "NOTNULL");
        }
     if ( testFlag = 1)
     {
               logWrite(summaryLog, "SHoundLLIDFromGOIDAndECode", "PASS", "", "", "");

     }
     else
     {
               logWrite(summaryLog, "SHoundLLIDFromGOIDAndECode", "FAIL", "", "", "");

     }
	vnp = ValNodeFree(vnp);

	
	/* ------------------------
	   test SHoundLLIDFromOMIM
	   ------------------------*/
	intValue = 103950; /*a valid omim id*/
        sprintf(itos1, "%ld", intValue);
        vnpi = SHoundLLIDFromOMIM(intValue);
        if(vnpi == NULL)
        {
	    printf("SHoundLLIDFromOMIM failed.\n");
            testFlag = 0;
    	    logWrite(reportLog, "SHoundLLIDFromOMIM", "OMIM", itos1, "Pointer", "NULL");
        }
	else
        {
            printf("SHoundLLIDFromOMIM OK.\n");
            testFlag = 1;
	    logWrite(reportLog, "SHoundLLIDFromOMIM", "OMIM", itos1, "Pointer", "NOTNULL");
        } 
        vnpi = ValNodeFree(vnpi);

        intValue = MAXINT; /* invalid param */ 
        vnpi = SHoundLLIDFromOMIM(intValue);
	if(vnpi == NULL)
        {
	    printf("SHoundLLIDFromOMIM gets the expected result.\n");
	    logWrite(reportLog, "SHoundLLIDFromOMIM", "OMIM", "MAXINT", "Pointer", "NULL");
        }
	else
        {
            printf("SHoundLLIDFromOMIM fails to get the expected result.\n");
            testFlag = 0;
	    logWrite(reportLog, "SHoundLLIDFromOMIM", "OMIM", "MAXINT", "Pointer", "NOTNULL");
        } 
	vnpi = ValNodeFree(vnpi);
        
	
        intValue = MINUSONE; 
	vnpi = SHoundLLIDFromOMIM(intValue);
	if(vnpi == NULL)
        {
	    printf("SHoundLLIDFromOMIM gets the expected result.\n");
	    logWrite(reportLog, "SHoundLLIDFromOMIM", "OMIM", "-1", "Pointer", "NULL");
        }
	else
        {
            printf("SHoundLLIDFromOMIM fails to get the expected result.\n");
            testFlag = 0;
	    logWrite(reportLog, "SHoundLLIDFromOMIM", "OMIM", "-1", "Pointer", "NOTNULL");
        } 
     if ( testFlag = 1)
     {
               logWrite(summaryLog, "SHoundLLIDFromOMIM", "PASS", "", "", "");

     }
     else
     {
               logWrite(summaryLog, "SHoundLLIDFromOMIM", "FAIL", "", "", "");

     }
	vnpi = ValNodeFree(vnpi);
	
	/* ------------------------
	   test SHoundLLIDFromCDDID
	   ------------------------*/
	vnp = ValNodeFree(vnp);
	vnp = SHoundLLIDFromCDDID("smart00408");

	if (vnp == NULL)
	{
	    printf("SHoundLLIDFromCDDID failed.\n");
            testFlag = 0;
            logWrite(reportLog, "SHoundLLIDFromCDDID", "CDDID", "STRING", "Pointer", "NULL");
	}
        else
	{
            printf("SHoundLLIDFromCDDID OK.\n");
            testFlag = 1;
	    logWrite(reportLog, "SHoundLLIFromCDDID", "CDDID", "STRING", "Pointer", "NOTNULL");
	}
	vnp = ValNodeFree(vnp);

	vnp = SHoundLLIDFromCDDID(nullString);
	if (vnp == NULL)
	{
	    printf("SHoundLLIDFromCDDID gets the expected result.\n");
            logWrite(reportLog, "SHoundLLIDFromCDDID", "CDDID", "NULLSTRING", "Pointer", "NULL");
	}
        else
	{
            printf("SHoundLLIDFromCDDID fails to get the expected result.\n");
            testFlag = 0;
	    logWrite(reportLog, "SHoundLLIDFromCDDID", "CDDID", "NULLSTRING", "Pointer", "NOTNULL");
	}
	vnp = ValNodeFree(vnp);
	
	/* Invalid Param */
	vnp = SHoundLLIDFromCDDID(EMPTYSTR);
	if (vnp == NULL)
	{
	    printf("SHoundLLIDFromCDDID gets the expected result.\n");
            logWrite(reportLog, "SHoundLLIDFromCDDID", "CDDID", "EMPTYSTRING", "Pointer", "NULL");
	}
        else
	{
            printf("SHoundLLIDFromCDDID fails to get the expected result.\n");
            testFlag = 0;
	    logWrite(reportLog, "SHoundLLIDFromCDDID", "CDDID", "EMPTYSTRING", "Pointer", "NOTNULL");
	}
     if ( testFlag = 1)
     {
               logWrite(summaryLog, "SHoundLLIDFromCDDID", "PASS", "", "", "");

     }
     else
     {
               logWrite(summaryLog, "SHoundLLIDFromCDDID", "FAIL", "", "", "");

     }
	vnp = ValNodeFree(vnp);

      
       /*-----------------------
         SHoundCDDScoreFromGi
	 -----------------------*/
        score = -1;
        gi = 34870013;
        cddid = "COG0606";
        score = SHoundCDDScoreFromGi (gi, cddid);
        sprintf(itos2, "%.8g", score);
        logWrite(reportLog, "SHoundCDDScoreFromGi", "GI & CDDID", "34870013 & COG0606", "SCORE", itos2);
        if(score == -1)
        {
	    printf("SHoundOMIMFromLLID fails to get the expected result\n");
            testFlag = 0;
        }
	else
        {
            printf("SHoundOMIMFromLLID gets the expected result\n");
	    testFlag = 1;
        } 
	
	score = -1;
        intValue = MAXINT; /* invalid param */ 
	score = SHoundCDDScoreFromGi(intValue, cddid);
	sprintf(itos2, "%.8g", score);
	logWrite(reportLog, "SHoundCDDScoreFromGi", "GI & CDDID", "MAXINT", "SCORE", itos2);
	if(score <= 0 )
        {
	    printf("SHoundCDDScoreFromGi gets the expected result\n");
           
        }
	else
        {
            printf("SHoundCDDScoreFromGi fails to get the expected result.\n");
	    testFlag = 0;
        } 
      
        score = -1;
        intValue = MINUSONE; /* invalid param */ 
	score = SHoundCDDScoreFromGi(intValue, cddid);
	sprintf(itos2, "%.8g", score);
	logWrite(reportLog, "SHoundCDDScoreFromGi", "GI & CDDID", "-1", "SCORE", itos2);
	if(score <=0 )
        {
	    printf("SHoundCDDScoreFromGi gets the expected result\n");
        }
	else
        {
            printf("SHoundCDDScoreFromGi fails to get the expected result.\n");
	    testFlag = 0;
        } 
      
	score = -1;
	score = SHoundCDDScoreFromGi(gi, nullString);
	sprintf(itos2, "%.8g", score);
	logWrite(reportLog, "SHoundCDDScoreFromGi", "GI & CDDID", "NULLSTRING", "SCORE", itos2 );
	if(score <= 0 )
        {
	    printf("SHoundCDDScoreFromGi gets the expected result\n");
        }
	else
        {
            printf("SHoundCDDScoreFromGi fails to get the expected result.\n");
	    testFlag = 0;
        } 
	
     if ( testFlag = 1)
     {
               logWrite(summaryLog, "SHoundCDDScoreFromGi", "PASS", "", "", "");

     }
     else
     {
               logWrite(summaryLog, "SHoundCDDScoreFromGi", "FAIL", "", "", "");

     }


	
      intValue = 8320; /* valid GO ID */ 
      sprintf(itos1, "%ld", intValue);
	vnpi = SHoundGiFromGOID(intValue);
	if(vnpi == NULL)
      {
	   printf("SHoundGiFromGOID failed.\n");
        testFlag = 0;
    	   logWrite(reportLog, "SHoundGiFromGOID", "GOID", itos1, "Pointer", "NULL");
      }
	else
      {
         printf("SHoundGiFromGOID OK.\n");
        testFlag = 1;
	   logWrite(reportLog, "SHoundGiFromGOID", "GOID", itos1, "Pointer", "NOTNULL");
      } 
	vnpi = ValNodeFree(vnpi);

      intValue = MAXINT; /* invalid param */ 
	vnpi = SHoundGiFromGOID(intValue);
	if(vnpi == NULL)
      {
	   printf("SHoundGiFromGOID failed.\n");
	   logWrite(reportLog, "SHoundGiFromGOID", "GOID", "MAXINT", "Pointer", "NULL");
      }
	else
      {
         printf("SHoundGiFromGOID OK.\n");
        testFlag = 0;
	   logWrite(reportLog, "SHoundGiFromGOID", "GOID", "MAXINT", "Pointer", "NOTNULL");
      } 
	vnpi = ValNodeFree(vnpi);
      
      intValue = MINUSONE; /* invalid param */ 
	vnpi = SHoundGiFromGOID(intValue);
	if(vnpi == NULL)
      {
	   printf("SHoundGiFromGOID failed.\n");
	   logWrite(reportLog, "SHoundGiFromGOID", "GOID", "-1", "Pointer", "NULL");
      }
	else
      {
         printf("SHoundGiFromGOID OK.\n");
        testFlag = 0;
	   logWrite(reportLog, "SHoundGiFromGOID", "GOID", "-1", "Pointer", "NOTNULL");
      } 
     if ( testFlag = 1)
     {
             logWrite(summaryLog, "SHoundGiFromGOID", "PASS", "", "", "");

     }
     else
     {
             logWrite(summaryLog, "SHoundGiFromGOID", "FAIL", "", "", "");

     }
	vnpi = ValNodeFree(vnpi);

	vnpi = SHoundGiFromGOIDList(goList);
	if(vnpi == NULL)
      {
	   printf("SHoundGiFromGOIDList failed.\n");
        testFlag = 0;
    	   logWrite(reportLog, "SHoundGiFromGOIDList", "ValNodePtr", "NOTNULL", "Pointer", "NULL");
      }
	else
      {
          printf("SHoundGiFromGOIDList OK.\n");
          testFlag = 1;
	    logWrite(reportLog, "SHoundGiFromGOIDList", "ValNodePtr", "NOTNULL", "Pointer", "NOTNULL");
      }
	vnpi = ValNodeFree(vnpi);
  
      /* Invlid Param */
	vnpi = SHoundGiFromGOIDList(nullValNodePtr);
	if(vnpi == NULL)
      {
	   printf("SHoundGiFromGOIDList failed.\n");
	   logWrite(reportLog, "SHoundGiFromGOIDList", "ValNodePtr", "NULL", "Pointer", "NULL");
      }
	else
      {
          printf("SHoundGiFromGOIDList OK.\n");
          testFlag = 0;
	    logWrite(reportLog, "SHoundGiFromGOIDList", "ValNodePtr", "NULL", "Pointer", "NOTNULL");
      }
     if ( testFlag = 1)
     {
             logWrite(summaryLog, "SHoundGiFromGOIDList", "PASS", "", "", "");

     }
     else
     {
             logWrite(summaryLog, "SHoundGiFromGOIDList", "FAIL", "", "", "");

     }
	vnpi = ValNodeFree(vnpi);
      
      intValue = 4557225; /* valid GI */
      sprintf(itos1, "%ld", intValue);
	vnp2 = SHoundGOIDFromGi(intValue);
	if (vnp2 == NULL)
      {
	  printf("SHoundGOIDFromGi failed.\n");
        testFlag = 0;
    	  logWrite(reportLog, "SHoundGOIDFromGi", "GI", itos1, "Pointer", "NULL");
      }
      else
      {
        printf("SHoundGOIDFromGi OK.\n");
        testFlag = 1;
	  logWrite(reportLog, "SHoundGOIDFromGi", "GI", itos1, "Pointer", "NOTNULL");
      }
	vnp2 = ValNodeFree(vnp2);
      
      intValue = MAXINT; /* valid GI */
	vnp2 = SHoundGOIDFromGi(intValue);
	if (vnp2 == NULL)
      {
	  printf("SHoundGOIDFromGi failed.\n");
	  logWrite(reportLog, "SHoundGOIDFromGi", "GI", "MAXINT", "Pointer", "NULL");
      }
      else
      {
        printf("SHoundGOIDFromGi OK.\n");
        testFlag = 0;
	  logWrite(reportLog, "SHoundGOIDFromGi", "GI", "MAXINT", "Pointer", "NOTNULL");
      }
	vnp2 = ValNodeFree(vnp2);
 
      intValue = MINUSONE; /* valid GI */
	vnp2 = SHoundGOIDFromGi(intValue);
	if (vnp2 == NULL)
      {
	  printf("SHoundGOIDFromGi failed.\n");
	  logWrite(reportLog, "SHoundGOIDFromGi", "GI", "-1", "Pointer", "NULL");
      }
      else
      {
        printf("SHoundGOIDFromGi OK.\n");
        testFlag = 0;
	  logWrite(reportLog, "SHoundGOIDFromGi", "GI", "-1", "Pointer", "NULL");
      }
     if ( testFlag = 1)
     {
             logWrite(summaryLog, "SHoundGOIDFromGi", "PASS", "", "", "");

     }
     else
     {
             logWrite(summaryLog, "SHoundGOIDFromGi", "FAIL", "", "", "");

     }
	vnp2 = ValNodeFree(vnp2);

	ValNodeAddInt(&giList, 0, 4501841);
	ValNodeAddInt(&giList, 0, 4557225);
	vnpk = SHoundGOIDFromGiList(giList);
	if (vnpk == NULL)
      {
	  printf("SHoundGOIDFromGiList failed.\n");
        testFlag = 0;
    	  logWrite(reportLog, "SHoundGOIDFromGiList", "ValNodePtr", "NOTNULL", "Pointer", "NULL");
      }
      else
      {
        printf("SHoundGOIDFromGiList OK.\n");
        testFlag = 1;
	  logWrite(reportLog, "SHoundGOIDFromGiList", "ValNodePtr", "NOTNULL", "Pointer", "NOTNULL");
      }
   	vnpk = ValNodeFree(vnpk);

      /* Invalid Param */
	vnpk = SHoundGOIDFromGiList(nullValNodePtr);
	if (vnpk == NULL)
      {
	  printf("SHoundGOIDFromGiList failed.\n");
	  logWrite(reportLog, "SHoundGOIDFromGiList", "ValNodePtr", "NULL", "Pointer", "NULL");
      }
      else
      {
        printf("SHoundGOIDFromGiList OK.\n");
        testFlag = 0;
	  logWrite(reportLog, "SHoundGOIDFromGiList", "ValNodePtr", "NULL", "Pointer", "NOTNULL");
      }
     if ( testFlag = 1)
     {
             logWrite(summaryLog, "SHoundGOIDFromGiList", "PASS", "", "", "");

     }
     else
     {
             logWrite(summaryLog, "SHoundGOIDFromGiList", "FAIL", "", "", "");

     }
   	vnpk = ValNodeFree(vnpk);
	intValue = 4557225;  /* valid GI ID */
 	intValue2 = 4867;   /* valid GOID */ 
	sprintf(itos1, "%ld/%ld", intValue, intValue2);
	vnp = SHoundGOECodeFromGiAndGOID(intValue, intValue2);
	if (vnp == NULL)
      {
	  printf("SHoundGOECodeFromGiAndGOID failed.\n");
        testFlag = 0;
    	  logWrite(reportLog, "SHoundGOEcodeFromGiAndGOID", "GI/GOID",itos1, "Pointer", "NULL");
      }
      else
 	{
        printf("SHoundGOECodeFromGiAndGOID OK.\n");
        testFlag = 1;
	  logWrite(reportLog, "SHoundGOEcodeFromGiAndGOID", "GI/GOID",itos1, "Pointer", "NOTNULL");
	}
	vnp = ValNodeFreeData(vnp);

	intValue = MAXINT;  /* invalid GI ID */
 	intValue2 = MAXINT;   /* invalid GOID */ 
	vnp = SHoundGOECodeFromGiAndGOID(intValue, intValue2);
	if (vnp == NULL)
      {
	  printf("SHoundGOECodeFromGiAndGOID failed.\n");
	  logWrite(reportLog, "SHoundGOEcodeFromGiAndGOID", "GI/GOID","MAXINT", "Pointer", "NULL");
      }
      else
 	{
        printf("SHoundGOECodeFromGiAndGOID OK.\n");
        testFlag = 0;
	  logWrite(reportLog, "SHoundGOEcodeFromGiAndGOID", "GI/GOID",itos1, "Pointer", "NOTNULL");
	}
	vnp = ValNodeFreeData(vnp);

	intValue = MINUSONE;  /* invalid GI ID */
 	intValue2 = MINUSONE;   /* invalid GOID */ 
	vnp = SHoundGOECodeFromGiAndGOID(intValue, intValue2);
	if (vnp == NULL)
      {
	  printf("SHoundGOECodeFromGiAndGOID failed.\n");
	  logWrite(reportLog, "SHoundGOEcodeFromGiAndGOID", "GI/GOID","-1/-1", "Pointer", "NULL");
      }
      else
 	{
        printf("SHoundGOECodeFromGiAndGOID OK.\n");
        testFlag = 0;
	  logWrite(reportLog, "SHoundGOEcodeFromGiAndGOID", "GI/GOID", "-1/-1", "Pointer", "NULL");
	}
     if ( testFlag = 1)
     {
             logWrite(summaryLog, "SHoundGOEcodeFromGiAndGOID", "PASS", "", "", "");

     }
     else
     {
             logWrite(summaryLog, "SHoundGOEcodeFromGiAndGOID", "FAIL", "", "", "");

     }
	vnp = ValNodeFreeData(vnp);

	intValue = 21071030;  /* valid GI ID */
 	intValue2 = 5554;   /* valid GOID */ 
	sprintf(itos1, "%ld/%ld", intValue, intValue2);
	vnp = SHoundGOPMIDFromGiAndGOID(intValue, intValue2);
	if (vnp == NULL)
      {
	  printf("SSHoundGOPMIDFromGiAndGOID failed.\n");
        testFlag = 0;
	  logWrite(reportLog, "SHoundGOPMIDFromGiAndGOID", "GI/GOID",itos1, "Pointer", "NULL");
	}
      else
	{
        printf("SHoundGOPMIDFromGiAndGOID OK.\n");
        testFlag = 1;
	  logWrite(reportLog, "SHoundGOPMIDFromGiAndGOID", "GI/GOID",itos1, "Pointer", "NOTNULL");
	}
	vnp = ValNodeFree(vnp);

	intValue = MAXINT;  /* invalid GI ID */
 	intValue2 = MAXINT;   /* invalid GOID */ 
	vnp = SHoundGOPMIDFromGiAndGOID(intValue, intValue2);
	if (vnp == NULL)
      {
	  printf("SSHoundGOPMIDFromGiAndGOID failed.\n");
	  logWrite(reportLog, "SHoundGOPMIDFromGiAndGOID", "GI/GOID","MAXINT", "Pointer", "NULL");
	}
      else
	{
        printf("SHoundGOPMIDFromGiAndGOID OK.\n");
        testFlag = 0;
	  logWrite(reportLog, "SHoundGOPMIDFromGiAndGOID", "GI/GOID","MAXINT", "Pointer", "NOTNULL");
	}
	vnp = ValNodeFree(vnp);

	intValue = MINUSONE;  /* invalid GI ID */
 	intValue2 = MINUSONE;   /* invalid GOID */ 
	vnp = SHoundGOPMIDFromGiAndGOID(intValue, intValue2);
	if (vnp == NULL)
      {
	  printf("SSHoundGOPMIDFromGiAndGOID failed.\n");
	  logWrite(reportLog, "SHoundGOPMIDFromGiAndGOID", "GI/GOID","-1/-1", "Pointer", "NULL");
	}
      else
	{
        printf("SHoundGOPMIDFromGiAndGOID OK.\n");
        testFlag = 0;
	  logWrite(reportLog, "SHoundGOPMIDFromGiAndGOID", "GI/GOID","-1/-1", "Pointer", "NOTNULL");
	}
     if ( testFlag = 1)
     {
             logWrite(summaryLog, "SHoundGOPMIDFromGiAndGOID", "PASS", "", "", "");

     }
     else
     {
             logWrite(summaryLog, "SHoundGOPMIDFromGiAndGOID", "FAIL", "", "", "");

     }
	vnp = ValNodeFree(vnp);

        /*---------------------------
	  SHoundGOIDFromRedundantGi
	  ---------------------------*/
	intValue = 4557225;
	sprintf(itos1, "%ld", intValue);
	vnp = SHoundGOIDFromRedundantGi(intValue);
	if (vnp == NULL)
	{
	  printf("SHoundGOIDFromRedundantGi failed.\n");
          testFlag = 0;
    	  logWrite(reportLog, "SHoundGOIDFromRedundantGi", "GI", itos1, "Pointer", "NULL");
	}
        else
	{
          printf("SHoundGOIDFromRedundantGi OK.\n");
          testFlag = 1;
	  logWrite(reportLog, "SHoundGOIDFromRedundantGi", "GI", itos1, "Pointer", "NOTNULL");
	}
	vnp = ValNodeFree(vnp);
	intValue = MAXINT; 
	vnp = SHoundGOIDFromRedundantGi(intValue);
	if (vnp == NULL)
	{
	  printf("SHoundGOIDFromRedundantGi failed.\n");
	  logWrite(reportLog, "SHoundGOIDFromRedundantGi", "GI", "MAXINT", "Pointer", "NULL");
	}
        else
	{
          printf("SHoundGOIDFromRedundantGi OK.\n");
          testFlag = 0;
	  logWrite(reportLog, "SHoundGOIDFromRedundantGi", "GI", "MAXINT", "Pointer", "NOTNULL");
	}
	vnp = ValNodeFree(vnp);
	intValue = MINUSONE; 
	vnp = SHoundGOIDFromRedundantGi(intValue);
	if (vnp == NULL)
	{
	  printf("SHoundGOIDFromRedundantGi failed.\n");
	  logWrite(reportLog, "SHoundGOIDFromRedundantGi", "GI", "-1", "Pointer", "NULL");
	}
        else
	{
          printf("SHoundGOIDFromRedundantGi OK.\n");
          testFlag = 0;
	  logWrite(reportLog, "SHoundGOIDFromRedundantGi", "GI", "-1", "Pointer", "NOTNULL");
	}
     if ( testFlag = 1)
     {
               logWrite(summaryLog, "SHoundGOIDFromRedundantGi", "PASS", "", "", "");

     }
     else
     {
               logWrite(summaryLog, "SHoundGOIDFromRedundantGi", "FAIL", "", "", "");

     }
	vnp = ValNodeFree(vnp);
       
       
        /*-------------------------------
	  SHoundGOIDFromRedundantGiList
	  -------------------------------*/
	pvngi = ValNodeFree(pvngi);
	ValNodeAddInt (&pvngi, 0, 4557225);
	ValNodeAddInt (&pvngi, 0, 123);
	
	vnp = SHoundGOIDFromRedundantGiList(pvngi);
	
	if (vnp == NULL)
	{
	  printf("SHoundGOIDFromRedundantGiList failed.\n");
          testFlag = 0;
    	  logWrite(reportLog, "SHoundGOIDFromRedundantGiList", "ValNodePtr", "NOTNULL", "Pointer", "NULL");
	}
        else
	{
          printf("SHoundGOIDFromRedundantGiList OK.\n");
          testFlag = 1;
	  logWrite(reportLog, "SHoundGOIDFromRedundantGiList", "ValNodePtr", "NOTNULL", "Pointer", "NOTNULL");
	}
	vnp = ValNodeFree(vnp);
     
	vnp = SHoundGOIDFromRedundantGiList(nullValNodePtr);
	if (vnp == NULL)
	{
	  printf("SHoundGOIDFromRedundantGiList failed.\n");
	  logWrite(reportLog, "SHoundGOIDFromRedundantGiList", "ValNodePtr", "NULL", "Pointer", "NULL");
	}
        else
	{
          printf("SHoundGOIDFromRedundantGiList OK.\n");
          testFlag = 0;
	  logWrite(reportLog, "SHoundGOIDFromRedundantGiList", "ValNodePtr", "NULL", "Pointer", "NULL");
	}
     if ( testFlag = 1)
     {
               logWrite(summaryLog, "SHoundGOIDFromRedundantGiList", "PASS", "", "", "");

     }
     else
     {
               logWrite(summaryLog, "SHoundGOIDFromRedundantGiList", "FAIL", "", "", "");

     }
	vnp = ValNodeFree(vnp);
        
	/*-----------------------------
	  SHoundGiFromOMIM
	  -----------------------------*/

	intValue = 103950; /*valid OMIM ID */
	sprintf(itos1, "%ld", intValue);
	vnp = SHoundGiFromOMIM(intValue);
	if (vnp == NULL)
	{
	  printf("SHoundGiFromOMIM failed.\n");
        testFlag = 0;
    	  logWrite(reportLog, "SHoundGiFromOMIM", "OMIM", itos1, "Pointer", "NULL");
	}
      else
	{
        printf("SHoundGiFromOMIM OK.\n");
        testFlag = 1;
	  logWrite(reportLog, "SHoundGiFromOMIM", "OMIM", itos1, "Pointer", "NOTNULL");
	}
	vnp = ValNodeFree(vnp);
	intValue = MAXINT; /*invalid OMIM ID */
	vnp = SHoundGiFromOMIM(intValue);
	if (vnp == NULL)
	{
	  printf("SHoundGiFromOMIM failed.\n");
	  logWrite(reportLog, "SHoundGiFromOMIM", "OMIM", "MAXINT", "Pointer", "NULL");
	}
      else
	{
        printf("SHoundGiFromOMIM OK.\n");
        testFlag = 0;
	  logWrite(reportLog, "SHoundGiFromOMIM", "OMIM", "MAXINT", "Pointer", "NOTNULL");
	}
	vnp = ValNodeFree(vnp);
	intValue = MINUSONE; /*invalid OMIM ID */
	vnp = SHoundGiFromOMIM(intValue);
	if (vnp == NULL)
	{
	  printf("SHoundGiFromOMIM failed.\n");
	  logWrite(reportLog, "SHoundGiFromOMIM", "OMIM", "-1", "Pointer", "NULL");
	}
      else
	{
        printf("SHoundGiFromOMIM OK.\n");
        testFlag = 0;
	  logWrite(reportLog, "SHoundGiFromOMIM", "OMIM", "-1", "Pointer", "NOTNULL");
	}
     if ( testFlag = 1)
     {
             logWrite(summaryLog, "SHoundGiFromOMIM", "PASS", "", "", "");

     }
     else
     {
             logWrite(summaryLog, "SHoundGiFromOMIM", "FAIL", "", "", "");

     }
	vnp = ValNodeFree(vnp);

        /*-----------------------------
	  SHoundGiFromOMIMList
	  -----------------------------*/
	vnp = SHoundGiFromOMIMList(omimList);
	if (vnp == NULL)
	{
	  printf("SHoundGiFromOMIMList failed.\n");
        testFlag = 0;
    	  logWrite(reportLog, "SHoundGiFromOMIMList", "ValNodePtr", "NOTNULL", "Pointer", "NULL");
	}
      else
	{
        printf("SHoundGiFromOMIMList OK.\n");
        testFlag = 1;
	  logWrite(reportLog, "SHoundGiFromOMIMList", "ValNodePtr", "NOTNULL", "Pointer", "NOTNULL");
	}
	vnp = ValNodeFree(vnp);
      /* invalid param */
	vnp = SHoundGiFromOMIMList(nullValNodePtr);
	if (vnp == NULL)
	{
	  printf("SHoundGiFromOMIMList failed.\n");
	  logWrite(reportLog, "SHoundGiFromOMIMList", "ValNodePtr", "NULL", "Pointer", "NULL");
	}
      else
	{
        printf("SHoundGiFromOMIMList OK.\n");
        testFlag = 0;
	  logWrite(reportLog, "SHoundGiFromOMIMList", "ValNodePtr", "NULL", "Pointer", "NULL");
	}
     if ( testFlag = 1)
     {
             logWrite(summaryLog, "SHoundGiFromOMIMList", "PASS", "", "", "");

     }
     else
     {
             logWrite(summaryLog, "SHoundGiFromOMIMList", "FAIL", "", "", "");

     }
	vnp = ValNodeFree(vnp);

	intValue = 4505355; /* valid param GI */
	sprintf(itos1, "%ld", intValue);
	vnp = SHoundOMIMFromGi(intValue);
	if (vnp == NULL)
	{
	  printf("SHoundOMIMFromGi failed.\n");
        testFlag = 0;
    	  logWrite(reportLog, "SHoundOMIMFromGi", "GI", itos1, "Pointer", "NULL");
	}
      else
	{
        printf("SHoundOMIMFromGi OK.\n");
        testFlag = 1;
	  logWrite(reportLog, "SHoundOMIMFromGi", "GI", itos1, "Pointer", "NOTNULL");
	}
	vnp = ValNodeFree(vnp);

	intValue = MAXINT; /* invalid param GI */
	vnp = SHoundOMIMFromGi(intValue);
	if (vnp == NULL)
	{
	  printf("SHoundOMIMFromGi failed.\n");
	  logWrite(reportLog, "SHoundOMIMFromGi", "GI", "MAXINT", "Pointer", "NULL");
	}
      else
	{
        printf("SHoundOMIMFromGi OK.\n");
        testFlag = 0;
	  logWrite(reportLog, "SHoundOMIMFromGi", "GI", "MAXINT", "Pointer", "NOTNULL");
	}
	vnp = ValNodeFree(vnp);
	intValue = MINUSONE; /* invalid param GI */
	vnp = SHoundOMIMFromGi(intValue);
	if (vnp == NULL)
	{
	  printf("SHoundOMIMFromGi failed.\n");
	  logWrite(reportLog, "SHoundOMIMFromGi", "GI", "-1", "Pointer", "NULL");
	}
      else
	{
        printf("SHoundOMIMFromGi OK.\n");
        testFlag = 0;
	  logWrite(reportLog, "SHoundOMIMFromGi", "GI", "-1", "Pointer", "NOTNULL");
	}
     if ( testFlag = 1)
     {
             logWrite(summaryLog, "SHoundOMIMFromGi", "PASS", "", "", "");

     }
     else
     {
             logWrite(summaryLog, "SHoundOMIMFromGi", "FAIL", "", "", "");

     }
	vnp = ValNodeFree(vnp);

	vnp = SHoundOMIMFromGiList(giList);
	if (vnp == NULL)
   	{
	  printf("SHoundOMIMFromGiList failed.\n");
        testFlag = 0;
	  logWrite(reportLog, "SHoundOMIMFromGiList", "ValNodePtr", "NOTNULL", "Pointer", "NULL");
	}
      else
	{
        printf("SHoundOMIMFromGiList OK.\n");
        testFlag = 1;
	  logWrite(reportLog, "SHoundOMIMFromGiList", "ValNodePtr", "NOTNULL", "Pointer", "NOTNULL");
	}
	vnp = ValNodeFree(vnp);

	/* invalid param */
	vnp = SHoundOMIMFromGiList(nullValNodePtr);
	if (vnp == NULL)
   	{
	  printf("SHoundOMIMFromGiList failed.\n");
	  logWrite(reportLog, "SHoundOMIMFromGiList", "ValNodePtr", "NULL", "Pointer", "NULL");
	}
      else
	{
        printf("SHoundOMIMFromGiList OK.\n");
        testFlag = 0;
	  logWrite(reportLog, "SHoundOMIMFromGiList", "ValNodePtr", "NULL", "Pointer", "NOTNULL");
	}
     if ( testFlag = 1)
     {
             logWrite(summaryLog, "SHoundOMIMFromGiList", "PASS", "", "", "");

     }
     else
     {
             logWrite(summaryLog, "SHoundOMIMFromGiList", "FAIL", "", "", "");

     }
	vnp = ValNodeFree(vnp);
	intValue = 9; /* valid param locus link ID */
	sprintf(itos1, "%ld", intValue);
	vnp = SHoundGiFromLLID(intValue);
	if (vnp == NULL)
	{
	  printf("SHoundGiFromLLID failed.\n");
        testFlag = 0;
	  logWrite(reportLog, "SHoundGiFromLLID", "LLID", itos1, "Pointer", "NULL");
	}
      else
	{
        printf("SHoundGiFromLLID OK.\n");
        testFlag = 1;
	  logWrite(reportLog, "SHoundGiFromLLID", "LLID", itos1, "Pointer", "NOTNULL");
	}
	vnp = ValNodeFree(vnp);
	intValue = MAXINT; /* invalid param locus link ID */
	vnp = SHoundGiFromLLID(intValue);
	if (vnp == NULL)
	{
	  printf("SHoundGiFromLLID failed.\n");
	  logWrite(reportLog, "SHoundGiFromLLID", "LLID", "MAXINT", "Pointer", "NULL");
	}
      else
	{
        printf("SHoundGiFromLLID OK.\n");
        testFlag = 0;
	  logWrite(reportLog, "SHoundGiFromLLID", "LLID", "MAXINT", "Pointer", "NOTNULL");
	}
	vnp = ValNodeFree(vnp);
	intValue = MINUSONE; /* invalid param locus link ID */
	vnp = SHoundGiFromLLID(intValue);
	if (vnp == NULL)
	{
	  printf("SHoundGiFromLLID failed.\n");
	  logWrite(reportLog, "SHoundGiFromLLID", "LLID", "-1", "Pointer", "NULL");
	}
      else
	{
        printf("SHoundGiFromLLID OK.\n");
        testFlag = 0;
	  logWrite(reportLog, "SHoundGiFromLLID", "LLID", "-1", "Pointer", "NOTNULL");
	}
     if ( testFlag = 1)
     {
             logWrite(summaryLog, "SHoundGiFromLLID", "PASS", "", "", "");

     }
     else
     {
             logWrite(summaryLog, "SHoundGiFromLLID", "FAIL", "", "", "");

     }
	vnp = ValNodeFree(vnp);

	vnp = SHoundGiFromLLIDList(lllist);
	if (vnp == NULL)
	{
	  printf("SHoundGiFromLLIDList failed.\n");
        testFlag = 0;
	  logWrite(reportLog, "SHoundGiFromLLIDList", "ValNodePtr", "NOTNULL", "Pointer", "NULL");
	}
      else
	{
        printf("SHoundGiFromLLIDList OK.\n");
        testFlag = 1;
	  logWrite(reportLog, "SHoundGiFromLLIDList", "ValNodePtr", "NOTNULL", "Pointer", "NOTNULL");
	}
	vnp = ValNodeFree(vnp);
	/* invalid param */
	vnp = SHoundGiFromLLIDList(nullValNodePtr);
	if (vnp == NULL)
	{
	  printf("SHoundGiFromLLIDList failed.\n");
	  logWrite(reportLog, "SHoundGiFromLLIDList", "ValNodePtr", "NULL", "Pointer", "NULL");
	}
      else
	{
        printf("SHoundGiFromLLIDList OK.\n");
        testFlag = 0;
	  logWrite(reportLog, "SHoundGiFromLLIDList", "ValNodePtr", "NULL", "Pointer", "NOTNULL");
	}
     if ( testFlag = 1)
     {
             logWrite(summaryLog, "SHoundGiFromLLIDList", "PASS", "", "", "");

     }
     else
     {
             logWrite(summaryLog, "SHoundGiFromLLIDList", "FAIL", "", "", "");

     }
	vnp = ValNodeFree(vnp);

	intValue = 0;  /*set return value to 0 */
    	intValue2 = 4557225; /* valid GI */
	sprintf(itos1, "%ld", intValue2);	
	intValue = SHoundLLIDFromGi(intValue2);
	sprintf(itos2, "%ld", intValue);
	logWrite(reportLog, "SHoundLLIDFromGi", "GI", itos1, "LLID", itos2);
	if(intValue > 0)
	{
	  printf("SHoundLLIDFromGi OK.\n");
        testFlag = 1;
    	  
	}
      else
	{
        printf("SHoundLLIDFromGi failed.\n");
        testFlag = 0;
	}
	intValue = 0;  /*set return value to 0 */
    	intValue2 = MAXINT; /* invalid GI */
	intValue = SHoundLLIDFromGi(intValue2);
	sprintf(itos2, "%ld", intValue);
	logWrite(reportLog, "SHoundLLIDFromGi", "GI", "MAXINT", "LLID", itos2);
	if(intValue > 0)
	{
	  printf("SHoundLLIDFromGi failed.\n");
        testFlag = 0;
	}
      else
	{
        printf("SHoundLLIDFromGi OK.\n");
	}
	intValue = 0;  /*set return value to 0 */
    	intValue2 = MINUSONE; /* invalid GI */
	intValue = SHoundLLIDFromGi(intValue2);
	sprintf(itos2, "%ld", intValue);
	logWrite(reportLog, "SHoundLLIDFromGi", "GI", "-1", "LLID", itos2);
	if(intValue > 0)
	{
	  printf("SHoundLLIDFromGi failed.\n");
        testFlag = 0;
	}
      else
	{
        printf("SHoundLLIDFromGi OK.\n");
	}
     if ( testFlag = 1)
     {
             logWrite(summaryLog, "SHoundLLIDFromGi", "PASS", "", "", "");

     }
     else
     {
             logWrite(summaryLog, "SHoundLLIDFromGi", "FAIL", "", "", "");

     }

	vnp = SHoundLLIDFromGiList(giList);
	if (vnp == NULL)
	{
	  printf("SHoundLLIDFromGiList failed.\n");
        testFlag = 0;
    	  logWrite(reportLog, "SHoundLLIDFromGiList", "ValNodePtr", "NOTNULL", "Pointer", "NULL");
	}
      else
	{
          printf("SHoundLLIDFromGiList OK.\n");
          testFlag = 1;
	    logWrite(reportLog, "SHoundLLIDFromGiList", "ValNodePtr", "NOTNULL", "Pointer", "NOTNULL");
	}
	vnp = ValNodeFree(vnp);
	/* invalid param */
	vnp = SHoundLLIDFromGiList(nullValNodePtr);
	if (vnp == NULL)
	{
	  printf("SHoundLLIDFromGiList failed.\n");
	  logWrite(reportLog, "SHoundLLIDFromGiList", "ValNodePtr", "NULL", "Pointer", "NULL");
	}
      else
	{
          printf("SHoundLLIDFromGiList OK.\n");
          testFlag = 0;
	    logWrite(reportLog, "SHoundLLIDFromGiList", "ValNodePtr", "NULL", "Pointer", "NOTNULL");
	}
     if ( testFlag = 1)
     {
             logWrite(summaryLog, "SHoundLLIDFromGiList", "PASS", "", "", "");

     }
     else
     {
             logWrite(summaryLog, "SHoundLLIDFromGiList", "FAIL", "", "", "");

     }
	vnp = ValNodeFree(vnp);

	intValue = 21071030; /* valid param GI */
	pchar = SHoundLocusFromGi(intValue);
	sprintf(itos1, "%ld", intValue);
	if(pchar == NULL)
	{
	  printf("SHoundLocusFromGi failed.\n");
        testFlag = 0;
    	  logWrite(reportLog, "SHoundLocusFromGi", "GI", itos1, "Pointer","NULL");
	}
      else
	{
        printf("SHoundLocusFromGi OK.\n");
        testFlag = 1;
	  logWrite(reportLog, "SHoundLocusFromGi", "GI", itos1, "Pointer","NOTNULL");
	}
	pchar = MemFree(pchar);

	intValue = MAXINT; /* invalid param GI */
	pchar = SHoundLocusFromGi(intValue);
	if(pchar == NULL)
	{
	  printf("SHoundLocusFromGi failed.\n");
    	  logWrite(reportLog, "SHoundLocusFromGi", "GI", "MAXINT", "Pointer","NULL");
	}
      else
	{
        printf("SHoundLocusFromGi OK.\n");
        testFlag = 0;
	  logWrite(reportLog, "SHoundLocusFromGi", "GI", "MAXINT", "Pointer","NOTNULL");
	}
	pchar = MemFree(pchar);

	intValue = MINUSONE; /* invalid param GI */
	pchar = SHoundLocusFromGi(intValue);
	if(pchar == NULL)
	{
	  printf("SHoundLocusFromGi failed.\n");
    	  logWrite(reportLog, "SHoundLocusFromGi", "GI", "-1", "Pointer","NULL");
	}
      else
	{
        printf("SHoundLocusFromGi OK.\n");
        testFlag = 0;
	  logWrite(reportLog, "SHoundLocusFromGi", "GI", "-1", "Pointer","NOTNULL");
	}
     if ( testFlag = 1)
     {
             logWrite(summaryLog, "SHoundLocusFromGi", "PASS", "", "", "");

     }
     else
     {
             logWrite(summaryLog, "SHoundLocusFromGi", "FAIL", "", "", "");

     }
	pchar = MemFree(pchar);


	vnp = SHoundGiFromCDDID("pfam00664");
/*vnp = SHoundGiFromCDDID("pfam03028");*/
	if (vnp == NULL)
	{
	  printf("SHoundGiFromCDDID failed.\n");
        testFlag = 0;
        logWrite(reportLog, "SHoundGiFromCDDID", "CDDID", "STRING", "Pointer", "NULL");
	}
      else
	{
        printf("SHoundGiFromCDDID OK.\n");
        testFlag = 1;
	  logWrite(reportLog, "SHoundGiFromCDDID", "CDDID", "STRING", "Pointer", "NOTNULL");
	}
	vnp = ValNodeFree(vnp);

	vnp = SHoundGiFromCDDID(nullString);
	if (vnp == NULL)
	{
	  printf("SHoundGiFromCDDID failed.\n");
        logWrite(reportLog, "SHoundGiFromCDDID", "CDDID", "NULLSTRING", "Pointer", "NULL");
	}
      else
	{
        printf("SHoundGiFromCDDID OK.\n");
        testFlag = 0;
	  logWrite(reportLog, "SHoundGiFromCDDID", "CDDID", "NULLSTRING", "Pointer", "NOTNULL");
	}
	vnp = ValNodeFree(vnp);
	
	/* Invalid Param */
	vnp = SHoundGiFromCDDID(EMPTYSTR);
	if (vnp == NULL)
	{
	  printf("SHoundGiFromCDDID failed.\n");
        logWrite(reportLog, "SHoundGiFromCDDID", "CDDID", "EMPTYSTRING", "Pointer", "NULL");
	}
      else
	{
        printf("SHoundGiFromCDDID OK.\n");
        testFlag = 0;
	  logWrite(reportLog, "SHoundGiFromCDDID", "CDDID", "EMPTYSTRING", "Pointer", "NOTNULL");
	}
     if ( testFlag = 1)
     {
             logWrite(summaryLog, "SHoundGiFromCDDID", "PASS", "", "", "");

     }
     else
     {
             logWrite(summaryLog, "SHoundGiFromCDDID", "FAIL", "", "", "");

     }
	vnp = ValNodeFree(vnp);

	vnp = SHoundGiFromCDDIDList(cddidlist);
	if (vnp == NULL)
	{
	  printf("SHoundGiFromCDDIDList failed.\n");
        testFlag = 0;
        logWrite(reportLog, "SHoundGiFromCDDIDList", "ValNodePtr", "NOTNULL", "Pointer", "NULL");
	}
      else
	{
        printf("SHoundGiFromCDDIDList OK.\n");
        testFlag = 1;
	  logWrite(reportLog, "SHoundGiFromCDDIDList", "ValNodePtr", "NOTNULL", "Pointer", "NOTNULL");
	}
	vnp = ValNodeFree(vnp);
	/* invalid param */
	vnp = SHoundGiFromCDDIDList(nullValNodePtr);
	if (vnp == NULL)
	{
	  printf("SHoundGiFromCDDIDList failed.\n");
	  logWrite(reportLog, "SHoundGiFromCDDIDList", "ValNodePtr", "NULL", "Pointer", "NULL");
	}
      else
	{
        printf("SHoundGiFromCDDIDList OK.\n");
        testFlag = 0;
	  logWrite(reportLog, "SHoundGiFromCDDIDList", "ValNodePtr", "NULL", "Pointer", "NOTNULL");
	}
     if ( testFlag = 1)
     {
             logWrite(summaryLog, "SHoundGiFromCDDIDList", "PASS", "", "", "");

     }
     else
     {
             logWrite(summaryLog, "SHoundGiFromCDDIDList", "FAIL", "", "", "");

     }
	vnp = ValNodeFree(vnp);

	intValue = 4557225; /* valid param GI */
	sprintf(itos1, "%ld", intValue);
	vnp = SHoundCDDIDFromGi(intValue);
	if (vnp == NULL)
	{
	  printf("SHoundCDDIDFromGi failed.\n");
        testFlag = 0;
    	  logWrite(reportLog, "SHoundCDDIDFromGi", "GI", itos1, "Pointer", "NULL");
	}
      else
	{
        printf("SHoundCDDIDFromGi OK.\n");
        testFlag = 1;
	  logWrite(reportLog, "SHoundCDDIDFromGi", "GI", itos1, "Pointer", "NOTNULL");
	}
	vnp = ValNodeFreeData(vnp);

	intValue = MAXINT; /* invalid param GI */
	vnp = SHoundCDDIDFromGi(intValue);
	if (vnp == NULL)
	{
	  printf("SHoundCDDIDFromGi failed.\n");
	  logWrite(reportLog, "SHoundCDDIDFromGi", "GI", "MAXINT", "Pointer", "NULL");
	}
      else
	{
        printf("SHoundCDDIDFromGi OK.\n");
        testFlag = 0;
	  logWrite(reportLog, "SHoundCDDIDFromGi", "GI", "MAXINT", "Pointer", "NOTNULL");
	}
	vnp = ValNodeFreeData(vnp);
	intValue = MINUSONE; /* invalid param GI */
	vnp = SHoundCDDIDFromGi(intValue);
	if (vnp == NULL)
	{
	  printf("SHoundCDDIDFromGi failed.\n");
	  logWrite(reportLog, "SHoundCDDIDFromGi", "GI", "-1", "Pointer", "NULL");
	}
      else
	{
        printf("SHoundCDDIDFromGi OK.\n");
        testFlag = 0;
	  logWrite(reportLog, "SHoundCDDIDFromGi", "GI", "-1", "Pointer", "NOTNULL");
	}
     if ( testFlag = 1)
     {
             logWrite(summaryLog, "SHoundCDDIDFromGi", "PASS", "", "", "");

     }
     else
     {
             logWrite(summaryLog, "SHoundCDDIDFromGi", "FAIL", "", "", "");

     }
	vnp = ValNodeFreeData(vnp);

	vnp = SHoundCDDIDFromGiList(giList);
	if (vnp == NULL)
	{
	  printf("SHoundCDDIDFromGiList failed.\n");
        testFlag = 0;
    	  logWrite(reportLog, "SHoundCDDIDFromGiList", "VALNODE", "NOTNULL", "Pointer", "NULL");
	}
      else
	{
        printf("SHoundCDDIDFromGiList OK.\n");
        testFlag = 1;
	  logWrite(reportLog, "SHoundCDDIDFromGiList", "VALNODE", "NOTNULL", "Pointer", "NOTNULL");
	}
	vnp = ValNodeFreeData(vnp);
	/* Invalid Param */
	vnp = SHoundCDDIDFromGiList(nullValNodePtr);
	if (vnp == NULL)
	{
	  printf("SHoundCDDIDFromGiList failed.\n");
	  logWrite(reportLog, "SHoundCDDIDFromGiList", "VALNODE", "NULL", "Pointer", "NULL");
	}
      else
	{
        printf("SHoundCDDIDFromGiList OK.\n");
        testFlag = 0;
	  logWrite(reportLog, "SHoundCDDIDFromGiList", "VALNODE", "NULL", "Pointer", "NOTNULL");
	}
     if ( testFlag = 1)
     {
             logWrite(summaryLog, "SHoundCDDIDFromGiList", "PASS", "", "", "");

     }
     else
     {
             logWrite(summaryLog, "SHoundCDDIDFromGiList", "FAIL", "", "", "");

     }
	vnp = ValNodeFreeData(vnp);


   
    returnInt = -1;
    returnInt= SHound3DListFromTaxID( inTaxid, &p3DList );
    sprintf(itos1, "%ld", myargs[3].intvalue);
    sprintf(itos2, "%d", returnInt);
    logWrite(reportLog, "SHound3DListFromTaxID", "TAXID", itos1, "INT", itos2);
    if( returnInt == SLRI_SUCCESS )
    {
      testFlag = 1;
      printf( "SHound3DListFromTaxID OK.\n" );
    }
    else
    {
      testFlag = 0;
      printf( "SHound3DListFromTaxID failed.\n" );
    }
    returnInt = 0;
    returnInt= SHound3DListFromTaxID( MAXINT, &p3DList );
    sprintf(itos2, "%d", returnInt);
    logWrite(reportLog, "SHound3DListFromTaxID", "TAXID", "MAXINT", "INT", itos2);
    if( returnInt == SLRI_SUCCESS )
    {
      testFlag = 0;
    }
    returnInt = 0;
    returnInt= SHound3DListFromTaxID( MINUSONE, &p3DList );
    sprintf(itos2, "%d", returnInt);
    logWrite(reportLog, "SHound3DListFromTaxID", "TAXID", "-1", "INT", itos2);
    if( returnInt == SLRI_SUCCESS )
    {
      testFlag = 0;
    }
     if ( testFlag = 1)
     {
           logWrite(summaryLog, "SHound3DListFromTaxID", "PASS", "","","");

     }
     else
     {
           logWrite(summaryLog, "SHound3DListFromTaxID", "FAIL", "","","");

     }

   /*-----------------------------------------------------------
     SHoundFirstOfRedundantGroupFromIDList
     -----------------------------------------------------------*/

    ValNodeAddInt (&pvnredund, 0, myargs[0].intvalue);
    ValNodeAddInt (&pvnredund, 0, myargs[0].intvalue);
    ValNodeAddInt (&pvnredund, 0, myargs[0].intvalue);
    
    if (pvngi2 != NULL) pvngi2 = ValNodeFree(pvngi2);
    pvngi2 = SHoundFirstOfRedundantGroupFromIDList(pvnredund);
    sprintf(itos1, "%ld", myargs[0].intvalue);
    if(pvngi2 == NULL)
    {
      testFlag = 0;
      logWrite(reportLog, "SHoundFirstOfRedundantGroupFromIDList", "GILIST", itos1, "ValNodePtr", "NULL");
    }
    else
    {
      testFlag = 1;
      logWrite(reportLog, "SHoundFirstOfRedundantGroupFromIDList", "GILIST", itos1, "ValNodePtr", "NOTNULL");
    }

    ValNodeAddInt (&pvnInvalRedund, 0, MAXINT);
    ValNodeAddInt (&pvnInvalRedund, 0, MAXINT);
    ValNodeAddInt (&pvnInvalRedund, 0, MAXINT);

    if (pvngi2 != NULL) pvngi2 = ValNodeFree(pvngi2);
    pvngi2 = SHoundFirstOfRedundantGroupFromIDList(pvnInvalRedund);
    if(pvngi2 == NULL)
    {
      logWrite(reportLog, "SHoundFirstOfRedundantGroupFromIDList", "GILIST", "MAXINT", "ValNodePtr", "NULL");
    }
    else
    {
      testFlag = 0;
      logWrite(reportLog, "SHoundFirstOfRedundantGroupFromIDList", "GILIST", "MAXINT", "ValNodePtr", "NOTNULL");
    }

    pvnInvalRedund = ValNodeFree (pvnInvalRedund);
    ValNodeAddInt (&pvnInvalRedund, 0, MINUSONE);
    ValNodeAddInt (&pvnInvalRedund, 0, MINUSONE);
    ValNodeAddInt (&pvnInvalRedund, 0, MINUSONE);

    if (pvngi2 != NULL) pvngi2 = ValNodeFree(pvngi2);
    pvngi2 = SHoundFirstOfRedundantGroupFromIDList(pvnInvalRedund);
    if(pvngi2 == NULL)
    {
      logWrite(reportLog, "SHoundFirstOfRedundantGroupFromIDList", "GILIST", "-1", "ValNodePtr", "NULL");
    }
    else
    {
      testFlag = 0;
      logWrite(reportLog, "SHoundFirstOfRedundantGroupFromIDList", "GILIST", "-1", "ValNodePtr", "NOTNULL");
    }
     if ( testFlag = 1)
     {
           logWrite(summaryLog, "SHoundFirstOfRedundantGroupFromIDList", "PASS", "","","");

     }
     else
     {
           logWrite(summaryLog, "SHoundFirstOfRedundantGroupFromIDList", "FAIL", "","","");

     }

    pvngi2 = ValNodeFree(pvngi2);
/*-----------------------------------------------------------
     SHoundFirstOfRedundantGroupFromID
     -----------------------------------------------------------*/
    gin = SHoundFirstOfRedundantGroupFromID(myargs[5].intvalue);
    sprintf(itos1, "%ld", myargs[5].intvalue);
    sprintf(itos2, "%d", gin);
    logWrite(reportLog, "SHoundFirstOfRedundantGroupFromID", "RedundantID", itos1, "INT", itos2);

    if( gin <= 0 )
    {
      testFlag = 0;
    }
    else
    {
      testFlag = 1;
    }
    
    gin = SHoundFirstOfRedundantGroupFromID(MAXINT);
    sprintf(itos2, "%d", gin);
    logWrite(reportLog, "SHoundFirstOfRedundantGroupFromID", "RedundantID", "MAXINT", "INT", itos2);

    if( gin > 0 )
    {
      testFlag = 0;
    }

    gin = SHoundFirstOfRedundantGroupFromID(MINUSONE);
    sprintf(itos2, "%d", gin);
    logWrite(reportLog, "SHoundFirstOfRedundantGroupFromID", "RedundantID", "-1", "INT", itos2);

    if( gin > 0 )
    {
      testFlag = 0;
    }
     if ( testFlag = 1)
     {
           logWrite(summaryLog, "SHoundFirstOfRedundantGroupFromID", "PASS", "","","");

     }
     else
     {
           logWrite(summaryLog, "SHoundFirstOfRedundantGroupFromID", "FAIL", "","","");

     }
    
 /*-----------------------------------------------------------
     SHoundGet3DfromPdbId
   -----------------------------------------------------------*/

    pbio = SHoundGet3DfromPdbId("3INS");
    logWrite(reportLog, "SHoundGet3DfromPdbId",  "STRING", "3INS", "BioStrucPtr", itos1);
    if(pbio != NULL)
    {
       printf("SHoundGet3DfromPdbId OK.\n");
       logWrite(reportLog, "SHoundGet3DfromPdbId",  "STRING", "3INS", "BioStrucPtr", "NOTNULL");
	 testFlag = 1;
    }
    else
    {
       printf("SHoundGet3DfromPdbId failed.\n");
       logWrite(reportLog, "SHoundGet3DfromPdbId",  "STRING", "3INS", "BioStrucPtr", "NULL");
	 testFlag = 0;
    }
   
    BiostrucFree(pbio);
   
    pbio = SHoundGet3DfromPdbId(EMPTYSTR);
    if(pbio !=NULL)
    {
	 testFlag = 0;
       logWrite(reportLog, "SHoundGet3DfromPdbId",  "STRING", "EMPTYSTRING", "BioStrucPtr", "NOTNULL");
    }
    else
    {
       logWrite(reportLog, "SHoundGet3DfromPdbId",  "STRING", "EMPTYSTRING", "BioStrucPtr", "NULL");
    }
    BiostrucFree(pbio);
   
    pbio = SHoundGet3DfromPdbId(nullString);
    if( pbio!=NULL)
    {
	 testFlag = 0;
       logWrite(reportLog, "SHoundGet3DfromPdbId",  "STRING", "NULLSTRING", "BioStrucPtr", "NOTNULL");
    }
    else
    {
       logWrite(reportLog, "SHoundGet3DfromPdbId",  "STRING", "NULLSTRING", "BioStrucPtr", "NULL");
    }
     if ( testFlag = 1)
     {
           logWrite(summaryLog, "SHoundGet3DfromPdbId", "PASS", "", "", "");

     }
     else
     {
           logWrite(summaryLog, "SHoundGet3DfromPdbId", "FAIL", "", "", "");

     }
    BiostrucFree(pbio);
 
   /*-----------------------------------------------------------
     SHoundGetPDB3D
     this function meets segmentation fault
     -----------------------------------------------------------*/
    
    /*intValue =11;
    sprintf(itos2, "%ld", intValue);
    if (! SHoundGetPDB3D(intValue, file_name)){
    	logWrite(reportLog, "SHoundGetPDB3D", "MMDBID & FILENAME", itos2, "Boolean", "FALSE");
	testFlag = 0;  
    }
    
    else
    {
        logWrite(reportLog, "SHoundGetPDB3D", "MMDBID & FILENAME", itos2, "Boolean", "TRUE");
        testFlag = 1;
    }
    
   
    if ( SHoundGetPDB3D(MAXINT, file_name)){
    	logWrite(reportLog, "SHoundGetPDB3D", "MMDBID & FILENAME", "MAXINT", "Boolean", "TRUE");
	testFlag = 0;  
    }
    else
    {
        logWrite(reportLog, "SHoundGetPDB3D", "MMDBID & FILENAME", "MAXINT", "Boolean", "FALSE");
    }
    
    if (SHoundGetPDB3D(MINUSONE, file_name)){
    	logWrite(reportLog, "SHoundGetPDB3D", "MMDBID & FILENAME", "MINUSONE", "Boolean", "TRUE");
	testFlag = 0;  
    }
    else
    {
        logWrite(reportLog, "SHoundGetPDB3D", "MMDBID & FILENAME", "MINUSONE", "Boolean", "FALSE");
    }
     if ( testFlag = 1)
     {
           logWrite(summaryLog, "SHoundGetPDB3D", "PASS", "","",""); 

     }
     else
     {
           logWrite(summaryLog, "SHoundGetPDB3D", "FAIL", "","",""); 

     }
   */

   /*-----------------------------------------------------------
     SHoundGiFromPDBChain
     -----------------------------------------------------------*/
    pdbcode = "9XIM";
    chain = "A";
    gi = SHoundGiFromPDBchain (pdbcode, chain);
    sprintf(itos2, "%d", gi);
    if (gi <= 0){
        logWrite(reportLog, "SHoundGiFromPDBchain ", "PDBCODE & CHAIN", "9XIM & A", "INT", itos2 );
        testFlag = 0;  
    }
    else
    {
        logWrite(reportLog, "SHoundGiFromPDBchain ", "PDBCODE & CHAIN", "9XIM & A", "INT", itos2 );
        testFlag = 1;
    } 

    gi = SHoundGiFromPDBchain (nullString, nullString);
    sprintf(itos2, "%d", gi);
    if (gi <= 0){
        logWrite(reportLog, "SHoundGiFromPDBchain ", "PDBCODE & CHAIN", "NULLSTRING", "INT", itos2 ); 
    }
    else
    {
        logWrite(reportLog, "SHoundGiFromPDBchain ", "PDBCODE & CHAIN", "NULLSTRING", "INT", itos2 );
        testFlag = 0;
    } 
    
    gi = SHoundGiFromPDBchain (EMPTYSTR, EMPTYSTR);
    sprintf(itos2, "%d", gi);
    if (gi <= 0){
        logWrite(reportLog, "SHoundGiFromPDBchain ", "PDBCODE & CHAIN", "EMPTYSTR", "INT", itos2 ); 
    }
    else
    {
        logWrite(reportLog, "SHoundGiFromPDBchain ", "PDBCODE & CHAIN", "EMPTYSTR", "INT", itos2 );
        testFlag = 0;
    } 
     if ( testFlag = 1)
     {
            logWrite(summaryLog, "SHoundGiFromPDBchain", "PASS", "","","");

     }
     else
     {
            logWrite(summaryLog, "SHoundGiFromPDBchain", "FAIL", "","","");

     }
    
   /*-----------------------------------------------------------
     SHoundGetAllTaxAncestors
   -----------------------------------------------------------*/
    if (pvngia != NULL) pvngia = ValNodeFree (pvngia);
    pvngia = SHoundGetAllTaxAncestors(myargs[3].intvalue);
    sprintf(itos1, "%ld", myargs[3].intvalue);

    if( pvngia == NULL )
    {
      testFlag = 0;
      logWrite(reportLog, "SHoundGetAllTaxAncestors", "TaxID", itos1, "ValNodePtr", "NULL");
    }
    else
    {
      testFlag = 1;
      logWrite(reportLog, "SHoundGetAllTaxAncestors", "TaxID", itos1, "ValNodePtr", "NOTNULL");
    }
    
    if (pvngia != NULL) pvngia = ValNodeFree (pvngia);
    pvngia = SHoundGetAllTaxAncestors(MAXINT);
    if( pvngia == NULL )
    {
      logWrite(reportLog, "SHoundGetAllTaxAncestors", "TaxID", "MAXINT", "ValNodePtr", "NULL");
    }
    else
    {
      testFlag = 0;
      logWrite(reportLog, "SHoundGetAllTaxAncestors", "TaxID", "MAXINT", "ValNodePtr", "NOTNULL");
    }

    if (pvngia != NULL) pvngia = ValNodeFree (pvngia);
    pvngia = SHoundGetAllTaxAncestors(MINUSONE);
    logWrite(reportLog, "SHoundGetAllTaxAncestors", "TaxID", "-1", "ValNodePtr", "NULL");

    if( pvngia == NULL  )
    {
      logWrite(reportLog, "SHoundGetAllTaxAncestors", "TaxID", "-1", "ValNodePtr", "NULL");
    }
    else
    {
      testFlag = 0;
      logWrite(reportLog, "SHoundGetAllTaxAncestors", "TaxID", "-1", "ValNodePtr", "NOTNULL");
    }
     if ( testFlag = 1)
     {
           logWrite(summaryLog, "SHoundGetAllTaxAncestors", "PASS", "","","");

     }
     else
     {
           logWrite(summaryLog, "SHoundGetAllTaxAncestors", "FAIL", "","","");

     }
    if (pvngia != NULL) pvngia = ValNodeFree (pvngia);
    
   /*-----------------------------------------------------------
     SHoundGetAllTaxProgeny
   -----------------------------------------------------------*/
    if (pvngia != NULL) pvngia = ValNodeFree (pvngia);
/*pvngia = SHoundGetAllTaxProgeny(myargs[3].intvalue);*/
/*sprintf(itos1, "%ld", myargs[3].intvalue);*/
/* Use TAX ID 9 */
    intValue = 9;
    pvngia = SHoundGetAllTaxProgeny(intValue);
    sprintf(itos1, "%ld", intValue);

    if( pvngia == NULL )
    {
      testFlag = 0;
      logWrite(reportLog, "SHoundGetAllTaxProgeny", "TaxID", itos1, "ValNodePtr", "NULL");
    }
    else
    {
      testFlag = 1;
      logWrite(reportLog, "SHoundGetAllTaxProgeny", "TaxID", itos1, "ValNodePtr", "NOTNULL");
    }
    
    if (pvngia != NULL) pvngia = ValNodeFree (pvngia);
    pvngia = SHoundGetAllTaxProgeny(MAXINT);
    if( pvngia == NULL )
    {
      logWrite(reportLog, "SHoundGetAllTaxProgeny", "TaxID", "MAXINT", "ValNodePtr", "NULL");
    }
    else
    {
      testFlag = 0;
      logWrite(reportLog, "SHoundGetAllTaxProgeny", "TaxID", "MAXINT", "ValNodePtr", "NOTNULL");
    }

    if (pvngia != NULL) pvngia = ValNodeFree (pvngia);
    pvngia = SHoundGetAllTaxProgeny(MINUSONE);
    logWrite(reportLog, "SHoundGetAllTaxProgeny", "TaxID", "-1", "ValNodePtr", "NULL");

    if( pvngia == NULL  )
    {
      logWrite(reportLog, "SHoundGetAllTaxProgeny", "TaxID", "-1", "ValNodePtr", "NULL");
    }
    else
    {
      testFlag = 0;
      logWrite(reportLog, "SHoundGetAllTaxProgeny", "TaxID", "-1", "ValNodePtr", "NOTNULL");
    }
     if ( testFlag = 1)
     {
           logWrite(summaryLog, "SHoundGetAllTaxProgeny", "PASS", "","","");

     }
     else
     {
           logWrite(summaryLog, "SHoundGetAllTaxProgeny", "FAIL", "","","");

     }
    if (pvngia != NULL) pvngia = ValNodeFree (pvngia);
    
 /*-----------------------------------------------------------
     SHoundGetDefline
   -----------------------------------------------------------*/

    sprintf(itos1, "%ld", myargs[0].intvalue);
    if(!SHoundGetDefline(myargs[0].intvalue))
    {
      testFlag = 0;
      logWrite(reportLog, "SHoundGetDefline", "GI", itos1, "BOOLEAN", "FALSE");
    }
    else
    {
      testFlag = 1;
      logWrite(reportLog, "SHoundGetDefline", "GI", itos1, "BOOLEAN", "TRUE");
    }
    
    if(!SHoundGetDefline(MAXINT))
    {
      logWrite(reportLog, "SHoundGetDefline", "GI", "MAXINT", "BOOLEAN", "FALSE");
    }
    else
    {
      testFlag = 0;
      logWrite(reportLog, "SHoundGetDefline", "GI", "MAXINT", "BOOLEAN", "TRUE");
    }

    if(!SHoundGetDefline(MINUSONE))
    {
      logWrite(reportLog, "SHoundGetDefline", "GI", "-1", "BOOLEAN", "FALSE");
    }
    else
    {
      testFlag = 0;
      logWrite(reportLog, "SHoundGetDefline", "GI", "-1", "BOOLEAN", "TRUE");
    }
     if ( testFlag = 1)
     {
           logWrite(summaryLog, "SHoundGetDefline", "PASS", "","","");

     }
     else
     {
           logWrite(summaryLog, "SHoundGetDefline", "FAIL", "","","");

     }
    
 /*-----------------------------------------------------------
     SHoundGetDeflineList
   -----------------------------------------------------------*/

    if (pvngia != NULL) pvngia = ValNodeFree (pvngia);
    pvngia = SHoundGetDeflineList(pvngi);
    if( pvngia == NULL )
    {
      testFlag = 0;
      logWrite(reportLog, "SHoundGetDeflineList", "ValNodePtr", "NOTNULL", "ValNodePtr", "NULL");
    }
    else
    {
      testFlag = 1;
      logWrite(reportLog, "SHoundGetDeflineList", "ValNodePtr", "NOTNULL", "ValNodePtr", "NOTNULL");
    }
    
    if (pvngia != NULL) pvngia = ValNodeFree (pvngia);
    pvngia = SHoundGetDeflineList(nullValNodePtr);
    if( pvngia == NULL )
    {
      logWrite(reportLog, "SHoundGetDeflineList", "ValNodePtr", "NULL", "ValNodePtr", "NULL");
    }
    else
    {
      testFlag = 0;
      logWrite(reportLog, "SHoundGetDeflineList", "ValNodePtr", "NULL", "ValNodePtr", "NOTNULL");
    }
     if ( testFlag = 1)
     {
           logWrite(summaryLog, "SHoundGetDeflineList", "PASS", "","","");

     }
     else
     {
           logWrite(summaryLog, "SHoundGetDeflineList", "FAIL", "","","");

     }

   /*-----------------------------------------------------------
     SHoundGetTaxChildNodes
   -----------------------------------------------------------*/

/*sprintf(itos1, "%ld", myargs[3].intvalue);*/
/* use TAX ID 9*/
    intValue = 9; 
    sprintf(itos1, "%ld", intValue);
    if (pvngia != NULL) pvngia = ValNodeFree (pvngia);
/*pvngia = SHoundGetTaxChildNodes(myargs[3].intvalue);*/
    pvngia = SHoundGetTaxChildNodes(intValue);
    if (pvngia == NULL)
    {
      testFlag = 0;
      logWrite(reportLog, "SHoundGetTaxChildNodes", "TaxID", itos1, "ValNodePtr", "NULL");
    }
    else
    {
      testFlag = 1;
      logWrite(reportLog, "SHoundGetTaxChildNodes", "TaxID", itos1, "ValNodePtr", "NOTNULL");
    }
    
    if (pvngia != NULL) pvngia = ValNodeFree (pvngia);
    pvngia = SHoundGetTaxChildNodes(MAXINT);
    if (pvngia == NULL)
    {
      logWrite(reportLog, "SHoundGetTaxChildNodes", "TaxID", "MAXINT", "ValNodePtr", "NULL");
    }
    else
    {
      testFlag = 0;
      logWrite(reportLog, "SHoundGetTaxChildNodes", "TaxID", "MAXINT", "ValNodePtr", "NOTNULL");
    }

    if (pvngia != NULL) pvngia = ValNodeFree (pvngia);
    pvngia = SHoundGetTaxChildNodes(MINUSONE);
    if (pvngia == NULL)
    {
      logWrite(reportLog, "SHoundGetTaxChildNodes", "TaxID", "-1", "ValNodePtr", "NULL");
    }
    else
    {
      testFlag = 0;
      logWrite(reportLog, "SHoundGetTaxChildNodes", "TaxID", "-1", "ValNodePtr", "NOTNULL");
    }
     if ( testFlag = 1)
     {
           logWrite(summaryLog, "SHoundGetTaxChildNodes", "PASS", "","","");

     }
     else
     {
           logWrite(summaryLog, "SHoundGetTaxChildNodes", "FAIL", "","","");

     }
    
   /*-----------------------------------------------------------
     SHoundGetTaxChildNodesList
   -----------------------------------------------------------*/

    if (pvngia != NULL) pvngia = ValNodeFree (pvngia);
/* use TAXIDs that are known to exist in the DB*/
/*pvngia = SHoundGetTaxChildNodesList(pvntaxid);*/
/*ValNodePtr knownPvnTaxidList = NULL;*/

    ValNodeAddInt(&knownPvnTaxidList, 0, 9);
    ValNodeAddInt(&knownPvnTaxidList, 0, 10);
    ValNodeAddInt(&knownPvnTaxidList, 0, 18);
    pvngia = SHoundGetTaxChildNodesList(knownPvnTaxidList);
    if( pvngia == NULL )
    {
      testFlag = 0;
      logWrite(reportLog, "SHoundGetTaxChildNodesList", "ValNodePtr", "NOTNULL", "ValNodePtr", "NULL");
    }
    else
    {
      testFlag = 1;
      logWrite(reportLog, "SHoundGetTaxChildNodesList", "ValNodePtr", "NOTNULL", "ValNodePtr", "NOTNULL");
    }
    
    if (pvngia != NULL) pvngia = ValNodeFree (pvngia);
    pvngia = SHoundGetTaxChildNodesList(nullValNodePtr);
    if( pvngia == NULL )
    {
      logWrite(reportLog, "SHoundGetTaxChildNodesList", "ValNodePtr", "NULL", "ValNodePtr", "NULL");
    }
    else
    {
      testFlag = 0;
      logWrite(reportLog, "SHoundGetTaxChildNodesList", "ValNodePtr", "NULL", "ValNodePtr", "NOTNULL");
    }
     if ( testFlag = 1)
     {
           logWrite(summaryLog, "SHoundGetTaxChildNodesList", "PASS", "","","");

     }
     else
     {
           logWrite(summaryLog, "SHoundGetTaxChildNodesList", "FAIL", "","","");

     }
    if (pvngia != NULL) pvngia = ValNodeFree (pvngia);

 /*-----------------------------------------------------------
     SHoundGetTaxLineageFromTaxID
   -----------------------------------------------------------*/

    sprintf(itos1, "%ld", myargs[3].intvalue);
    if (pchar != NULL) pchar = MemFree (pchar);
    pchar = SHoundGetTaxLineageFromTaxID(myargs[3].intvalue, SHoundGetTaxLineage_FULL);
    if (pchar == NULL)
    {
      testFlag = 0;
      logWrite(reportLog, "SHoundGetTaxLineageFromTaxID", "TaxID", itos1, "CharPtr", "NULL");
    }
    else
    {
      testFlag = 1;
      logWrite(reportLog, "SHoundGetTaxLineageFromTaxID", "TaxID", itos1, "CharPtr", "NOTNULL");
    }
    
    if (pchar != NULL) pchar = MemFree (pchar);
    pchar = SHoundGetTaxLineageFromTaxID(MAXINT, SHoundGetTaxLineage_FULL);
    if (pchar == NULL)
    {
      logWrite(reportLog, "SHoundGetTaxLineageFromTaxID", "TaxID", "MAXINT", "CharPtr", "NULL");
    }
    else
    {
      testFlag = 0;
      logWrite(reportLog, "SHoundGetTaxLineageFromTaxID", "TaxID", "MAXINT", "CharPtr", "NOTNULL");
    }

    if (pchar != NULL) pchar = MemFree (pchar);
    pchar = SHoundGetTaxLineageFromTaxID(MINUSONE, SHoundGetTaxLineage_FULL);
    if (pchar == NULL)
    {
      logWrite(reportLog, "SHoundGetTaxLineageFromTaxID", "TaxID", "-1", "CharPtr", "NULL");
    }
    else
    {
      testFlag = 0;
      logWrite(reportLog, "SHoundGetTaxLineageFromTaxID", "TaxID", "-1", "CharPtr", "NOTNULL");
    }
     if ( testFlag = 1)
     {
           logWrite(summaryLog, "SHoundGetTaxLineageFromTaxID", "PASS", "","","");

     }
     else
     {
           logWrite(summaryLog, "SHoundGetTaxLineageFromTaxID", "FAIL", "","","");

     }
  
  
  /*-----------------------------------------------------------
     SHoundGetNewTaxIDFromMergedTaxID
   -----------------------------------------------------------*/

   /* intValue = 395; 
    sprintf(itos1, "%ld", intValue);
    returnInt =0;
    returnInt = SHoundGetNewTaxIDFromMergedTaxID(intValue);
    sprintf(itos2, "%ld", returnInt);
    logWrite(reportLog, "SHoundGetNewTaxIDFromMergedTaxID", "TaxID", itos1, "NewTaxID", itos2);
    if(returnInt > 0 )
    {
	printf(" SHoundGetNewTaxIDFromMergedTaxID OK.\n");
        testFlag = 1;
    }
    else
    {
        printf(" SHoundGetNewTaxIDFromMergedTaxID failed.\n");
        testFlag = 0;
    } 
    returnInt=0;
    intValue = MAXINT; 
    returnInt = SHoundGetNewTaxIDFromMergedTaxID(intValue);
    sprintf(itos2, "%ld", returnInt);
    logWrite(reportLog, "SHoundGetNewTaxIDFromMergedTaxID", "TaxID", "MAXINT", "NewTaxID", itos2);
    if(returnInt > 0 )
    {
	printf("GetNewTaxIDFromMergedTaxID fails to get the expected result\n");
        testFlag = 0;
    }
    else
    {
        printf("SHoundGetNewTaxIDFromMergedTaxID gets the expected result\n");
    } 

    returnInt = 0;
    intValue = MINUSONE; 
    returnInt = SHoundGetNewTaxIDFromMergedTaxID(intValue);
    sprintf(itos2, "%ld", returnInt);
    logWrite(reportLog, "SHoundGetNewTaxIDFromMergedTaxID", "TaxID", "-1", "NewTaxID", itos2);
    if(returnInt > 0 )
    {
	printf("SHoundGetNewTaxIDFromMergedTaxID fails to get the expected result\n");
        testFlag = 0;
    }
    else
    {
        printf("SHoundGetNewTaxIDFromMergedTaxID gets the expected result.\n");
    } 
     if ( testFlag = 1)
     {
           logWrite(summaryLog, "SHoundGetNewTaxIDFromMergedTaxID", "PASS", "", "", "");

     }
     else
     {
           logWrite(summaryLog, "SHoundGetNewTaxIDFromMergedTaxID", "FAIL", "", "", "");

     }
  */
    
 /*-----------------------------------------------------------
     SHoundGetTaxNameFromTaxID
   -----------------------------------------------------------*/

    sprintf(itos1, "%ld", myargs[3].intvalue);
    if (pchar != NULL) pchar = MemFree (pchar);
    pchar = SHoundGetTaxNameFromTaxID(myargs[3].intvalue);
    if (pchar == NULL)
    {
      testFlag = 0;
      logWrite(reportLog, "SHoundGetTaxNameFromTaxID", "TaxID", itos1, "CharPtr", "NULL");
    }
    else
    {
      testFlag = 1;
      logWrite(reportLog, "SHoundGetTaxNameFromTaxID", "TaxID", itos1, "CharPtr", "NOTNULL");
    }
    
    if (pchar != NULL) pchar = MemFree (pchar);
    pchar = SHoundGetTaxNameFromTaxID(MAXINT);
    if (pchar == NULL)
    {
      logWrite(reportLog, "SHoundGetTaxNameFromTaxID", "TaxID", "MAXINT", "CharPtr", "NULL");
    }
    else
    {
      testFlag = 0;
      logWrite(reportLog, "SHoundGetTaxNameFromTaxID", "TaxID", "MAXINT", "CharPtr", "NOTNULL");
    }

    if (pchar != NULL) pchar = MemFree (pchar);
    pchar = SHoundGetTaxNameFromTaxID(MINUSONE);
    if (pchar == NULL)
    {
      logWrite(reportLog, "SHoundGetTaxNameFromTaxID", "TaxID", "-1", "CharPtr", "NULL");
    }
    else
    {
      testFlag = 0;
      logWrite(reportLog, "SHoundGetTaxNameFromTaxID", "TaxID", "-1", "CharPtr", "NOTNULL");
    }
     if ( testFlag = 1)
     {
           logWrite(summaryLog, "SHoundGetTaxNameFromTaxID", "PASS", "","","");

     }
     else
     {
           logWrite(summaryLog, "SHoundGetTaxNameFromTaxID", "FAIL", "","","");

     }
    
 /*-----------------------------------------------------------
     SHoundGetTaxNameFromTaxIDByClass
   -----------------------------------------------------------*/

/*sprintf(itos1, "%ld", myargs[3].intvalue);*/
/* use TAX ID known to exist in the DB*/
    intValue = 9;
/*Uint2 typeInt = 12; */
    sprintf(itos1, "%ld", intValue);
    if (pchar != NULL) pchar = MemFree (pchar);
    pchar = SHoundGetTaxNameFromTaxIDByClass(intValue, typeInt);
    if (pchar == NULL)
    {
      testFlag = 0;
      logWrite(reportLog, "SHoundGetTaxNameFromTaxIDByClass", "TaxID", itos1, "CharPtr", "NULL");
    }
    else
    {
      testFlag = 1;
      logWrite(reportLog, "SHoundGetTaxNameFromTaxIDByClass", "TaxID", itos1, "CharPtr", "NOTNULL");
    }
    
    if (pchar != NULL) pchar = MemFree (pchar);
    pchar = SHoundGetTaxNameFromTaxIDByClass(MAXINT, 12);
    if (pchar == NULL)
    {
      logWrite(reportLog, "SHoundGetTaxNameFromTaxIDByClass", "TaxID", "MAXINT", "CharPtr", "NULL");
    }
    else
    {
      testFlag = 0;
      logWrite(reportLog, "SHoundGetTaxNameFromTaxIDByClass", "TaxID", "MAXINT", "CharPtr", "NOTNULL");
    }

    if (pchar != NULL) pchar = MemFree (pchar);
    pchar = SHoundGetTaxNameFromTaxIDByClass(MINUSONE, 12);
    if (pchar == NULL)
    {
      logWrite(reportLog, "SHoundGetTaxNameFromTaxIDByClass", "TaxID", "-1", "CharPtr", "NULL");
    }
    else
    {
      testFlag = 0;
      logWrite(reportLog, "SHoundGetTaxNameFromTaxIDByClass", "TaxID", "-1", "CharPtr", "NOTNULL");
    }
     if ( testFlag = 1)
     {
           logWrite(summaryLog, "SHoundGetTaxNameFromTaxIDByClass", "PASS", "","","");

     }
     else
     {
           logWrite(summaryLog, "SHoundGetTaxNameFromTaxIDByClass", "FAIL", "","","");

     }
    
   /*-----------------------------------------------------------
     SHoundGetTaxParent
   -----------------------------------------------------------*/

    sprintf(itos1, "%ld", myargs[3].intvalue);
    intValue = 0;
    intValue = SHoundGetTaxParent(myargs[3].intvalue);
    sprintf(itos2, "%ld", intValue);
    if (intValue <= 0)
    {
      testFlag = 0;
      logWrite(reportLog, "SHoundGetTaxParent", "TaxID", itos1, "INT", itos2);
    }
    else
    {
      testFlag = 1;
      logWrite(reportLog, "SHoundGetTaxParent", "TaxID", itos1, "INT", itos2);
    }
    
    intValue = 0;
    intValue = SHoundGetTaxParent(MAXINT);
    sprintf(itos2, "%ld", intValue);
    if (intValue <= 0)
    {
      logWrite(reportLog, "SHoundGetTaxParent", "TaxID", "MAXINT", "INT", itos2);
    }
    else
    {
      testFlag = 0;
      logWrite(reportLog, "SHoundGetTaxParent", "TaxID", "MAXINT", "INT", itos2);
    }

    intValue = 0;
    intValue = SHoundGetTaxParent(MINUSONE);
    sprintf(itos2, "%ld", intValue);
    if (intValue <= 0)
    {
      logWrite(reportLog, "SHoundGetTaxParent", "TaxID", "-1", "INT", itos2);
    }
    else
    {
      testFlag = 0;
      logWrite(reportLog, "SHoundGetTaxParent", "TaxID", "-1", "INT", itos2);
    }
     if ( testFlag = 1)
     {
           logWrite(summaryLog, "SHoundGetTaxParent", "PASS", "","","");

     }
     else
     {
           logWrite(summaryLog, "SHoundGetTaxParent", "FAIL", "","","");

     }
    
    taxid = SHoundGetTaxParent(myargs[3].intvalue);
     
 /*-----------------------------------------------------------
     SHoundGetXML3D
   -----------------------------------------------------------*/

    sprintf(itos1, "%ld", myargs[2].intvalue);
    if(!SHoundGetXML3D(myargs[2].intvalue, "xml3D"))
    {
      testFlag = 0;
      logWrite(reportLog, "SHoundGetXML3D", "MMDBID", itos1, "BOOLEAN", "FALSE");
    }
    else
    {
      testFlag = 1;
      logWrite(reportLog, "SHoundGetXML3D", "MMDBID", itos1, "BOOLEAN", "TRUE");
    }
    
    if(!SHoundGetXML3D(MAXINT, "xml3D"))
    {
      logWrite(reportLog, "SHoundGetXML3D", "MMDBID", "MAXINT", "BOOLEAN", "FALSE");
    }
    else
    {
      testFlag = 0;
      logWrite(reportLog, "SHoundGetXML3D", "MMDBID", "MAXINT", "BOOLEAN", "TRUE");
    }

    if(!SHoundGetXML3D(MINUSONE, "xml3D"))
    {
      logWrite(reportLog, "SHoundGetXML3D", "MMDBID", "-1", "BOOLEAN", "FALSE");
    }
    else
    {
      testFlag = 0;
      logWrite(reportLog, "SHoundGetXML3D", "MMDBID", "-1", "BOOLEAN", "TRUE");
    }
     if ( testFlag = 1)
     {
           logWrite(summaryLog, "SHoundGetXML3D", "PASS", "","","");

     }
     else
     {
           logWrite(summaryLog, "SHoundGetXML3D", "FAIL", "","","");

     }
    
 /*-----------------------------------------------------------
     SHoundIsTaxDeleted
   -----------------------------------------------------------*/

    sprintf(itos1, "%ld", 56629);
    if(!SHoundIsTaxDeleted(56629))
    {
      testFlag = 0;
      logWrite(reportLog, "SHoundIsTaxDeleted", "TaxID", itos1, "BOOLEAN", "FALSE");
    }
    else
    {
      testFlag = 1;
      logWrite(reportLog, "SHoundIsTaxDeleted", "TaxID", itos1, "BOOLEAN", "TRUE");
    }
    
    sprintf(itos1, "%ld", myargs[3].intvalue);
    if(!SHoundIsTaxDeleted(myargs[3].intvalue))
    {
      logWrite(reportLog, "SHoundIsTaxDeleted", "TaxID", itos1, "BOOLEAN", "FALSE");
    }
    else
    {
      testFlag = 0;
      logWrite(reportLog, "SHoundIsTaxDeleted", "TaxID", itos1, "BOOLEAN", "TRUE");
    }

    if(!SHoundIsTaxDeleted(MAXINT))
    {
      logWrite(reportLog, "SHoundIsTaxDeleted", "TaxID", "MAXINT", "BOOLEAN", "FALSE");
    }
    else
    {
      testFlag = 0;
      logWrite(reportLog, "SHoundIsTaxDeleted", "TaxID", "MAXINT", "BOOLEAN", "TRUE");
    }

    if(!SHoundIsTaxDeleted(MINUSONE))
    {
      logWrite(reportLog, "SHoundIsTaxDeleted", "TaxID", "-1", "BOOLEAN", "FALSE");
    }
    else
    {
      testFlag = 0;
      logWrite(reportLog, "SHoundIsTaxDeleted", "TaxID", "-1", "BOOLEAN", "TRUE");
    }
     if ( testFlag = 1)
     {
           logWrite(summaryLog, "SHoundIsTaxDeleted", "PASS", "","","");

     }
     else
     {
           logWrite(summaryLog, "SHoundIsTaxDeleted", "FAIL", "","","");

     }
    
   /*-----------------------------------------------------------
     SHoundIsTaxMerged
   -----------------------------------------------------------*/

    sprintf(itos1, "%ld", 56529);
    if(!SHoundIsTaxMerged(56529))
    {
      testFlag = 0;
      logWrite(reportLog, "SHoundIsTaxMerged", "TaxID", itos1, "BOOLEAN", "FALSE");
    }
    else
    {
      testFlag = 1;
      logWrite(reportLog, "SHoundIsTaxMerged", "TaxID", itos1, "BOOLEAN", "TRUE");
    }
    
    sprintf(itos1, "%ld", 56642);
    if(!SHoundIsTaxMerged(56642))
    {
      logWrite(reportLog, "SHoundIsTaxMerged", "TaxID", itos1, "BOOLEAN", "FALSE");
    }
    else
    {
      testFlag = 0;
      logWrite(reportLog, "SHoundIsTaxMerged", "TaxID", itos1, "BOOLEAN", "TRUE");
    }
    
    if(!SHoundIsTaxMerged(MAXINT))
    {
      logWrite(reportLog, "SHoundIsTaxMerged", "TaxID", "MAXINT", "BOOLEAN", "FALSE");
    }
    else
    {
      testFlag = 0;
      logWrite(reportLog, "SHoundIsTaxMerged", "TaxID", "MAXINT", "BOOLEAN", "TRUE");
    }

    if(!SHoundIsTaxMerged(MINUSONE))
    {
      logWrite(reportLog, "SHoundIsTaxMerged", "TaxID", "-1", "BOOLEAN", "FALSE");
    }
    else
    {
      testFlag = 0;
      logWrite(reportLog, "SHoundIsTaxMerged", "TaxID", "-1", "BOOLEAN", "TRUE");
    }
     if ( testFlag = 1)
     {
           logWrite(summaryLog, "SHoundIsTaxMerged", "PASS", "","","");

     }
     else
     {
           logWrite(summaryLog, "SHoundIsTaxMerged", "FAIL", "","","");

     }

   /*-----------------------------------------------------------
     SHoundRedundantGroupFromID
     -----------------------------------------------------------*/

    sprintf(itos1, "%ld", myargs[5].intvalue);
    if (pvngia != NULL) pvngia = ValNodeFree (pvngia);
    pvngia = SHoundRedundantGroupFromID(myargs[5].intvalue);
    if (pvngia == NULL)
    {
      testFlag = 0;
      logWrite(reportLog, "SHoundRedundantGroupFromID", "RedundantGroupID", itos1, "ValNodePtr", "NULL");
    }
    else
    {
      testFlag = 1;
      logWrite(reportLog, "SHoundRedundantGroupFromID", "RedundantGroupID", itos1, "ValNodePtr", "NOTNULL");
    }
    
    if (pvngia != NULL) pvngia = ValNodeFree (pvngia);
    pvngia = SHoundRedundantGroupFromID(MAXINT);
    if (pvngia == NULL)
    {
      logWrite(reportLog, "SHoundRedundantGroupFromID", "RedundantGroupID", "MAXINT", "ValNodePtr", "NULL");
    }
    else
    {
      testFlag = 0;
      logWrite(reportLog, "SHoundRedundantGroupFromID", "RedundantGroupID", "MAXINT", "ValNodePtr", "NOTNULL");
    }

    if (pvngia != NULL) pvngia = ValNodeFree (pvngia);
    pvngia = SHoundRedundantGroupFromID(MINUSONE);
    if (pvngia == NULL)
    {
      logWrite(reportLog, "SHoundRedundantGroupFromID", "RedundantGroupID", "-1", "ValNodePtr", "NULL");
    }
    else
    {
      testFlag = 0;
      logWrite(reportLog, "SHoundRedundantGroupFromID", "RedundantGroupID", "-1", "ValNodePtr", "NOTNULL");
    }
     if ( testFlag = 1)
     {
           logWrite(summaryLog, "SHoundRedundantGroupFromID", "PASS", "","","");

     }
     else
     {
           logWrite(summaryLog, "SHoundRedundantGroupFromID", "FAIL", "","","");

     }

   /*-----------------------------------------------------------
     SHoundRedundantGroupFromIDList
     -----------------------------------------------------------*/

    ValNodeAddInt(&pvnredund, 0, myargs[0].intvalue);
    ValNodeAddInt(&pvnredund, 0, myargs[0].intvalue);
    ValNodeAddInt(&pvnredund, 0, myargs[0].intvalue);

    if (pvngia != NULL) pvngia = ValNodeFree (pvngia);
    pvngia = SHoundRedundantGroupFromIDList(pvnredund);
    if( pvngia == NULL )
    {
      testFlag = 0;
      logWrite(reportLog, "SHoundRedundantGroupFromIDList", "ValNodePtr", "NOTNULL", "ValNodePtr", "NULL");
    }
    else
    {
      testFlag = 1;
      logWrite(reportLog, "SHoundRedundantGroupFromIDList", "ValNodePtr", "NOTNULL", "ValNodePtr", "NOTNULL");
    }
    
    if (pvngia != NULL) pvngia = ValNodeFree (pvngia);
    pvngia = SHoundRedundantGroupFromIDList(nullValNodePtr);
    if( pvngia == NULL )
    {
      logWrite(reportLog, "SHoundRedundantGroupFromIDList", "ValNodePtr", "NULL", "ValNodePtr", "NULL");
    }
    else
    {
      testFlag = 0;
      logWrite(reportLog, "SHoundRedundantGroupFromIDList", "ValNodePtr", "NULL", "ValNodePtr", "NOTNULL");
    }
     if ( testFlag = 1)
     {
           logWrite(summaryLog, "SHoundRedundantGroupFromIDList", "PASS", "","","");

     }
     else
     {
           logWrite(summaryLog, "SHoundRedundantGroupFromIDList", "FAIL", "","","");

     }

 /*-----------------------------------------------------------
     SHoundRedundantGroupIDFromGI
     -----------------------------------------------------------*/

    sprintf(itos1, "%ld", myargs[0].intvalue);
    intValue = 0;
    intValue = SHoundRedundantGroupIDFromGI(myargs[0].intvalue);
    sprintf(itos2, "%ld", intValue);
    if (intValue <= 0)
    {
      testFlag = 0;
      logWrite(reportLog, "SHoundRedundantGroupIDFromGI", "GI", itos1, "RedundantGroupID", itos2);
    }
    else
    {
      testFlag = 1;
      logWrite(reportLog, "SHoundRedundantGroupIDFromGI", "GI", itos1, "RedundantGroupID", itos2);
    }
    
    intValue = 0;
    intValue = SHoundRedundantGroupIDFromGI(MAXINT);
    sprintf(itos2, "%ld", intValue);
    if (intValue <= 0)
    {
      logWrite(reportLog, "SHoundRedundantGroupIDFromGI", "GI", "MAXINT", "RedundantGroupID", itos2);
    }
    else
    {
      testFlag = 0;
      logWrite(reportLog, "SHoundRedundantGroupIDFromGI", "GI", "MAXINT", "RedundantGroupID", itos2);
    }

    intValue = 0;
    intValue = SHoundRedundantGroupIDFromGI(MINUSONE);
    sprintf(itos2, "%ld", intValue);
    if (intValue <= 0)
    {
      logWrite(reportLog, "SHoundRedundantGroupIDFromGI", "GI", "-1", "RedundantGroupID", itos2);
    }
    else
    {
      testFlag = 0;
      logWrite(reportLog, "SHoundRedundantGroupIDFromGI", "GI", "-1", "RedundantGroupID", itos2);
    }
     if ( testFlag = 1)
     {
           logWrite(summaryLog, "SHoundRedundantGroupIDFromGI", "PASS", "","","");

     }
     else
     {
           logWrite(summaryLog, "SHoundRedundantGroupIDFromGI", "FAIL", "","","");

     }
    
 /*-----------------------------------------------------------
     SHoundRedundantGroupIDFromGIList
     -----------------------------------------------------------*/

    if (pvngia != NULL) pvngia = ValNodeFree (pvngia);
    pvngia = SHoundRedundantGroupIDFromGIList(pvngi);
    if( pvngia == NULL )
    {
      testFlag = 0;
      logWrite(reportLog, "SHoundRedundantGroupIDFromGIList", "ValNodePtr", "NOTNULL", "ValNodePtr", "NULL");
    }
    else
    {
      testFlag = 1;
      logWrite(reportLog, "SHoundRedundantGroupIDFromGIList", "ValNodePtr", "NOTNULL", "ValNodePtr", "NOTNULL");
    }
    
    if (pvngia != NULL) pvngia = ValNodeFree (pvngia);
    pvngia = SHoundRedundantGroupIDFromGIList(nullValNodePtr);
    if( pvngia == NULL )
    {
      logWrite(reportLog, "SHoundRedundantGroupIDFromGIList", "ValNodePtr", "NULL", "ValNodePtr", "NULL");
    }
    else
    {
      testFlag = 0;
      logWrite(reportLog, "SHoundRedundantGroupIDFromGIList", "ValNodePtr", "NULL", "ValNodePtr", "NOTNULL");
    }
     if ( testFlag = 1)
     {
           logWrite(summaryLog, "SHoundRedundantGroupIDFromGIList", "PASS", "","","");

     }
     else
     {
           logWrite(summaryLog, "SHoundRedundantGroupIDFromGIList", "FAIL", "","","");

     }
    if (pvngia != NULL) pvngia = ValNodeFree (pvngia);

     /*--------------------------------------
       SHound3DNeighboursFromGi
      -------------------------------------- */ 

    sprintf(itos1, "%ld", myargs[0].intvalue);
    if (ilsp != NULL) ilsp = ILinkSetFree (ilsp);
    SHound3DNeighboursFromGi(myargs[0].intvalue, &ilsp, 1);
    if (ilsp == NULL)
    {
      testFlag = 0;
      logWrite(reportLog, "SHound3DNeighboursFromGi", "GI", itos1, "ValNodePtr", "NULL");
    }
    else
    {
      testFlag = 1;
      logWrite(reportLog, "SHound3DNeighboursFromGi", "GI", itos1, "ValNodePtr", "NOTNULL");
    }
    
    if (ilsp != NULL) ilsp = ILinkSetFree (ilsp);
    SHound3DNeighboursFromGi(MAXINT, &ilsp, 1);
    if (ilsp == NULL)
    {
      logWrite(reportLog, "SHound3DNeighboursFromGi", "GI", "MAXINT", "ValNodePtr", "NULL");
    }
    else
    {
      testFlag = 0;
      logWrite(reportLog, "SHound3DNeighboursFromGi", "GI", "MAXINT", "ValNodePtr", "NOTNULL");
    }

    if (ilsp != NULL) ilsp = ILinkSetFree (ilsp);
    SHound3DNeighboursFromGi(MINUSONE, &ilsp, 1);
    if (ilsp == NULL)
    {
      logWrite(reportLog, "SHound3DNeighboursFromGi", "GI", "-1", "ValNodePtr", "NULL");
    }
    else
    {
      testFlag = 0;
      logWrite(reportLog, "SHound3DNeighboursFromGi", "GI", "-1", "ValNodePtr", "NOTNULL");
    }
     if ( testFlag = 1)
     {
           logWrite(summaryLog, "SHound3DNeighboursFromGi", "PASS", "","","");

     }
     else
     {
           logWrite(summaryLog, "SHound3DNeighboursFromGi", "FAIL", "","","");

     }
    if (ilsp != NULL) ilsp = ILinkSetFree (ilsp);


    /*----------------------------------
      SHoundGiAndNumNeighboursList
      ----------------------------------*/
    if (ilsp != NULL) ilsp = ILinkSetFree (ilsp);
    SHoundGiAndNumNeighboursList(&ilsp);
    if (ilsp == NULL)
    {
      testFlag = 0;
      logWrite(reportLog, "SHoundGiAndNumNeighboursList", "ILINKSetPtr", "Pointer", "ValNodePtr", "NULL");
    }
    else
    {
      testFlag = 1;
      logWrite(reportLog, "SHoundGiAndNumNeighboursList", "ILINKSetPtr", "Pointer", "ValNodePtr", "NULL");
    }
    
     if ( testFlag = 1)
     {
           logWrite(summaryLog, "SHoundGiAndNumNeighboursList", "PASS", "","","");

     }
     else
     {
           logWrite(summaryLog, "SHoundGiAndNumNeighboursList", "FAIL", "","","");

     }
    if (ilsp != NULL) ilsp = ILinkSetFree (ilsp);


	/* SHound3DNeighboursFromGiList */ 
      if ( pvngi == NULL)
      {
	  ValNodeAddInt(&pvngi, 0, myargs[0].intvalue);
	  ValNodeAddInt(&pvngi, 0, myargs[0].intvalue);
	  ValNodeAddInt(&pvngi, 0, myargs[0].intvalue);
      }

    if (ilslp != NULL) ilslp = ILinkSetListFree (ilslp);
    SHound3DNeighboursFromGiList(pvngi, &ilslp, 100);
    if (ilslp == NULL)
    {
      testFlag = 0;
      logWrite(reportLog, "SHound3DNeighboursFromGiList", "ILinkSetListPtr", "NOTNULL", "ILinkSetListPtr", "NULL");
    }
    else
    {
      testFlag = 1;
      logWrite(reportLog, "SHound3DNeighboursFromGiList", "ILinkSetListPtr", "NOTNULL", "ILinkSetListPtr", "NOTNULL");
    }
    
    if (ilslp != NULL) ilslp = ILinkSetListFree (ilslp);
    SHound3DNeighboursFromGiList(nullValNodePtr, &ilslp, 100);
    if (ilslp == NULL)
    {
      logWrite(reportLog, "SHound3DNeighboursFromGiList", "ILinkSetListPtr", "NULL", "ILinkSetListPtr", "NULL");
    }
    else
    {
      testFlag = 0;
      logWrite(reportLog, "SHound3DNeighboursFromGiList", "ILinkSetListPtr", "NULL", "ILinkSetListPtr", "NOTNULL");
    }
     if ( testFlag = 1)
     {
           logWrite(summaryLog, "SHound3DNeighboursFromGiList", "PASS", "","","");

     }
     else
     {
           logWrite(summaryLog, "SHound3DNeighboursFromGiList", "FAIL", "","","");

     }
    if (ilslp != NULL) ilslp = ILinkSetListFree (ilslp);
    
	/* SHound3DNeighboursFromTaxID */ 
    myargs[3].intvalue=224915;
    sprintf(itos1, "%ld", myargs[3].intvalue);
    if (ilslp != NULL) ilslp = ILinkSetListFree (ilslp);
    SHound3DNeighboursFromTaxID(myargs[3].intvalue, &ilslp, 1);
    if (ilslp == NULL)
    {
      testFlag = 0;
      logWrite(reportLog, "SHound3DNeighboursFromTaxID", "TaxID", itos1, "ILinkSetListPtr", "NULL");
    }
    else
    {
      testFlag = 1;
      logWrite(reportLog, "SHound3DNeighboursFromTaxID", "TaxID", itos1, "ILinkSetListPtr", "NOTNULL");
    }
    
    if (ilslp != NULL) ilslp = ILinkSetListFree (ilslp);
    SHound3DNeighboursFromTaxID(MAXINT, &ilslp, 1);
    if (ilslp == NULL)
    {
      logWrite(reportLog, "SHound3DNeighboursFromTaxID", "TaxID", "MAXINT", "ILinkSetListPtr", "NULL");
    }
    else
    {
      testFlag = 0;
      logWrite(reportLog, "SHound3DNeighboursFromTaxID", "TaxID", "MAXINT", "ILinkSetListPtr", "NOTNULL");
    }

    if (ilslp != NULL) ilslp = ILinkSetListFree (ilslp);
    SHound3DNeighboursFromTaxID(MINUSONE, &ilslp, 1);
    if (ilslp == NULL)
    {
      logWrite(reportLog, "SHound3DNeighboursFromTaxID", "TaxID", "-1", "ILinkSetListPtr", "NULL");
    }
    else
    {
      testFlag = 0;
      logWrite(reportLog, "SHound3DNeighboursFromTaxID", "TaxID", "-1", "ILinkSetListPtr", "NOTNULL");
    }
     if ( testFlag = 1)
     {
           logWrite(summaryLog, "SHound3DNeighboursFromTaxID", "PASS", "","","");

     }
     else
     {
           logWrite(summaryLog, "SHound3DNeighboursFromTaxID", "FAIL", "","","");

     }
    if (ilslp != NULL) ilslp = ILinkSetListFree (ilslp);
    
        /* SHoundAllGenomesWithNumProteins */
         SHoundAllGenomesWithNumProteins(&ilsp);
         if (ilsp != NULL)
         {
           testFlag = 1;
           logWrite(reportLog, "SHoundAllGenomesWithNumProteins", "NONE", "NONE", "ILinkSetPtr", "NOTNULL");
         }
         else
         {
           testFlag = 0;
           logWrite(reportLog, "SHoundAllGenomesWithNumProteins", "NONE", "NONE", "ILinkSetPtr", "NULL");
         }
     if ( testFlag = 1)
     {
                logWrite(summaryLog, "SHoundAllGenomesWithNumProteins", "PASS", "","","");

     }
     else
     {
                logWrite(summaryLog, "SHoundAllGenomesWithNumProteins", "FAIL", "","","");

     }

         if (ilsp != NULL) ilsp = ILinkSetFree(ilsp);
	/* SHoundAllGenomesWithScientificNames */ 
         if (clsp != NULL) CLinkSetFree (clsp);
         SHoundAllGenomesWithScientificNames(&clsp);
         if (clsp != NULL)
         {
           testFlag = 1;
           logWrite(reportLog, "SHoundAllGenomesWithScientificNames", "NONE", "NONE", "CLinkSetPtr", "NOTNULL");
         }
         else
         {
           testFlag = 0;
           logWrite(reportLog, "SHoundAllGenomesWithScientificNames", "NONE", "NONE", "CLinkSetPtr", "NULL");
         }
     if ( testFlag = 1)
     {
                logWrite(summaryLog, "SHoundAllGenomesWithScientificNames", "PASS", "","","");

     }
     else
     {
                logWrite(summaryLog, "SHoundAllGenomesWithScientificNames", "FAIL", "","","");

     }
         if (clsp != NULL) CLinkSetFree (clsp);

	/* SHoundGetBlastResult */

/*sprintf(itos1, "%ld", myargs[0].intvalue);*/
/*if (nbrsp != NULL) nbrsp = NBlastResultSetFree (nbrsp);*/
/*SHoundGetBlastResult(myargs[0].intvalue, myargs[0].intvalue, &nbrsp);*/
    intValue = 49456613;
    intValue2 = 5453876;
/*Int4 ret = SLRI_FAIL;*/
    sprintf(itos1, "%ld/%ld", intValue, intValue2);
/*if (nbrsp != NULL) nbrsp = NBlastResultSetFree (nbrsp);*/
    ret = SHoundGetBlastResult(intValue, intValue2, &nbrsp);
    if (nbrsp == NULL)
    {
      testFlag = 0;
      logWrite(reportLog, "SHoundGetBlastResult", "GIPair", itos1, "NBlastResultSetPtr", "NULL");
    }
    else
    {
      testFlag = 1;
      logWrite(reportLog, "SHoundGetBlastResult", "GIPair", itos1, "NBlastResultSetPtr", "NOTNULL");
    }
    
    if (nbrsp != NULL) nbrsp = NBlastResultSetFree (nbrsp);
    ret = SHoundGetBlastResult(MAXINT, MAXINT, &nbrsp);
    if (nbrsp == NULL)
    {
      logWrite(reportLog, "SHoundGetBlastResult", "GIPair", "MAXINT", "NBlastResultSetPtr", "NULL");
    }
    else
    {
      testFlag = 0;
      logWrite(reportLog, "SHoundGetBlastResult", "GIPair", "MAXINT", "NBlastResultSetPtr", "NOTNULL");
    }

    if (nbrsp != NULL) nbrsp = NBlastResultSetFree (nbrsp);
    SHoundGetBlastResult(MINUSONE, MINUSONE, &nbrsp);
    if (nbrsp == NULL)
    {
      logWrite(reportLog, "SHoundGetBlastResult", "GIPair", "-1", "NBlastResultSetPtr", "NULL");
    }
    else
    {
      testFlag = 0;
      logWrite(reportLog, "SHoundGetBlastResult", "GIPair", "-1", "NBlastResultSetPtr", "NOTNULL");
    }
     if ( testFlag = 1)
     {
           logWrite(summaryLog, "SHoundGetBlastResult", "PASS", "","","");

     }
     else
     {
           logWrite(summaryLog, "SHoundGetBlastResult", "FAIL", "","","");

     }
    if (nbrsp != NULL) nbrsp = NBlastResultSetFree (nbrsp);
    
	/* SHoundGetBlastSeqAlign */

/*sprintf(itos1, "%ld", myargs[0].intvalue);*/
    if (seqAlignPtr != NULL) seqAlignPtr = SeqAlignFree (seqAlignPtr);
    ret = SHoundGetBlastSeqAlign(intValue, intValue2, &seqAlignPtr);
    if (seqAlignPtr == NULL)
    {
      testFlag = 0;
      logWrite(reportLog, "SHoundGetBlastSeqAlign", "GIPair", itos1, "SeqAlignPtr", "NULL");
    }
    else
    {
      testFlag = 1;
      logWrite(reportLog, "SHoundGetBlastSeqAlign", "GIPair", itos1, "SeqAlignPtr", "NOTNULL");
    }
    
    if (seqAlignPtr != NULL) seqAlignPtr = SeqAlignFree (seqAlignPtr);
    ret = SHoundGetBlastSeqAlign(MAXINT, MAXINT, &seqAlignPtr);
    if (seqAlignPtr == NULL)
    {
      logWrite(reportLog, "SHoundGetBlastSeqAlign", "GIPair", "MAXINT", "SeqAlignPtr", "NULL");
    }
    else
    {
      testFlag = 0;
      logWrite(reportLog, "SHoundGetBlastSeqAlign", "GIPair", "MAXINT", "SeqAlignPtr", "NOTNULL");
    }

    if (seqAlignPtr != NULL) seqAlignPtr = SeqAlignFree (seqAlignPtr);
    ret = SHoundGetBlastSeqAlign(MINUSONE, MINUSONE, &seqAlignPtr);
    if (seqAlignPtr == NULL)
    {
      logWrite(reportLog, "SHoundGetBlastSeqAlign", "GIPair", "-1", "SeqAlignPtr", "NULL");
    }
    else
    {
      testFlag = 0;
      logWrite(reportLog, "SHoundGetBlastSeqAlign", "GIPair", "-1", "SeqAlignPtr", "NOTNULL");
    }
     if ( testFlag = 1)
     {
           logWrite(summaryLog, "SHoundGetBlastSeqAlign", "PASS", "","","");

     }
     else
     {
           logWrite(summaryLog, "SHoundGetBlastSeqAlign", "FAIL", "","","");

     }
    if (seqAlignPtr != NULL) seqAlignPtr = SeqAlignFree (seqAlignPtr);
    

        /* SHoundGetDomainIdFromLabel */

      if (vnp2 != NULL) vnp2 = ValNodeFree(vnp2);
      vnp2 = SHoundGetDomainIdFromLabel("COLFI");
      if (vnp2 == NULL)
      {
	  printf("SHoundGetDomainIdFromLabel failed.\n");
        testFlag = 0;
    	  logWrite(reportLog, "SHoundGetDomainIdFromLabel", "Reference", "COLFI", "ValNodePtr", "NULL");
      }
      else
      {
	  printf("SHoundGetDomainIdFromLabel OK.\n");
        testFlag = 1;
	  logWrite(reportLog, "SHoundGetDomainIdFromLabel", "Reference", "COLFI", "ValNodePtr", "NOTNULL");
      }
	vnp2 = ValNodeFree(vnp2);
      /* Invalid Param */ 
	vnp2 = SHoundGetDomainIdFromLabel(EMPTYSTR);
	if (vnp2 == NULL)
      {
	  printf("SHoundGetDomainIdFromLabel failed.\n");
	  logWrite(reportLog, "SHoundGetDomainIdFromLabel", "Reference", "EMPTYSTRING", "ValNodePtr", "NULL");
      }
      else
      {
        printf("SHoundGetDomainIdFromLabel OK.\n");
        testFlag = 0;
	  logWrite(reportLog, "SHoundGetDomainIdFromLabel", "Reference", "EMPTYSTRING", "ValNodePtr", "NOTNULL");
      }
	vnp2 = ValNodeFree(vnp2);
      /* Invalid Param */ 
	vnp2 = SHoundGetDomainIdFromLabel(nullString);
	if (vnp2 == NULL)
      {
	  printf("SHoundGetDomainIdFromLabel failed.\n");
	  logWrite(reportLog, "SHoundGetDomainIdFromLabel", "Reference", "NULLSTRING", "ValNodePtr", "NULL");
      }
      else
      {
        printf("SHoundGetDomainIdFromLabel OK.\n");
        testFlag = 0;
	   logWrite(reportLog, "SHoundGetDomainIdFromLabel", "Reference", "NULLSTRING", "ValNodePtr", "NOTNULL");
      }
     if ( testFlag = 1)
     {
             logWrite(summaryLog, "SHoundGetDomainIdFromLabel", "PASS", "", "", "");

     }
     else
     {
             logWrite(summaryLog, "SHoundGetDomainIdFromLabel", "FAIL", "", "", "");

     }
	vnp2 = ValNodeFree(vnp2);

	/* SHoundGetDomainLabelFromDomainId */ 

      if (pchar != NULL) pchar = MemFree(pchar);
      pchar = SHoundGetDomainLabelFromDomainId("smart00038");
      if (pchar == NULL)
      {
	  printf("SHoundGetDomainLabelFromDomainId failed.\n");
        testFlag = 0;
    	  logWrite(reportLog, "SHoundGetDomainLabelFromDomainId", "Reference", "smart00038", "CharPtr", "NULL");
      }
      else
      {
	  printf("SHoundGetDomainLabelFromDomainId OK.\n");
        testFlag = 1;
	  logWrite(reportLog, "SHoundGetDomainLabelFromDomainId", "Reference", "smart00038", "CharPtr", "NOTNULL");
      }
	pchar = MemFree(pchar);
      /* Invalid Param */ 
	pchar = SHoundGetDomainLabelFromDomainId(EMPTYSTR);
	if (pchar == NULL)
      {
	  printf("SHoundGetDomainLabelFromDomainId failed.\n");
	  logWrite(reportLog, "SHoundGetDomainLabelFromDomainId", "Reference", "EMPTYSTRING", "CharPtr", "NULL");
      }
      else
      {
        printf("SHoundGetDomainLabelFromDomainId OK.\n");
        testFlag = 0;
	  logWrite(reportLog, "SHoundGetDomainLabelFromDomainId", "Reference", "EMPTYSTRING", "CharPtr", "NOTNULL");
      }
	pchar = MemFree(pchar);
      /* Invalid Param */ 
	pchar = SHoundGetDomainLabelFromDomainId(nullString);
	if (pchar == NULL)
      {
	  printf("SHoundGetDomainLabelFromDomainId failed.\n");
	  logWrite(reportLog, "SHoundGetDomainLabelFromDomainId", "Reference", "NULLSTRING", "CharPtr", "NULL");
      }
      else
      {
        printf("SHoundGetDomainLabelFromDomainId OK.\n");
        testFlag = 0;
	   logWrite(reportLog, "SHoundGetDomainLabelFromDomainId", "Reference", "NULLSTRING", "CharPtr", "NOTNULL");
      }
     if ( testFlag = 1)
     {
             logWrite(summaryLog, "SHoundGetDomainLabelFromDomainId", "PASS", "", "", "");

     }
     else
     {
             logWrite(summaryLog, "SHoundGetDomainLabelFromDomainId", "FAIL", "", "", "");

     }
	pchar = MemFree(pchar);
	
	/* SHoundGetDomainsFromGi */ 

/*sprintf(itos1, "%ld", myargs[0].intvalue);*/
/*if (srpsp != NULL) srpsp = StRpsdbFree (srpsp);*/
/*srpsp = SHoundGetDomainsFromGi(myargs[0].intvalue);*/
    intValue = 6322015;
    sprintf(itos1, "%ld", intValue);
    if (srpsp != NULL) srpsp = StRpsdbFree (srpsp);
    srpsp = SHoundGetDomainsFromGi(intValue);
    if (srpsp == NULL)
    {
      testFlag = 0;
      logWrite(reportLog, "SHoundGetDomainsFromGi", "GI", itos1, "StRpsdbPtr", "NULL");
    }
    else
    {
      testFlag = 1;
      logWrite(reportLog, "SHoundGetDomainsFromGi", "GI", itos1, "StRpsdbPtr", "NOTNULL");
    }
    
    if (srpsp != NULL) srpsp = StRpsdbFree (srpsp);
    srpsp = SHoundGetDomainsFromGi(MAXINT);
    if (srpsp == NULL)
    {
      logWrite(reportLog, "SHoundGetDomainsFromGi", "GI", "MAXINT", "StRpsdbPtr", "NULL");
    }
    else
    {
      testFlag = 0;
      logWrite(reportLog, "SHoundGetDomainsFromGi", "GI", "MAXINT", "StRpsdbPtr", "NOTNULL");
    }

    if (srpsp != NULL) srpsp = StRpsdbFree (srpsp);
    srpsp = SHoundGetDomainsFromGi(MINUSONE);
    if (srpsp == NULL)
    {
      logWrite(reportLog, "SHoundGetDomainsFromGi", "GI", "-1", "StRpsdbPtr", "NULL");
    }
    else
    {
      testFlag = 0;
      logWrite(reportLog, "SHoundGetDomainsFromGi", "GI", "-1", "StRpsdbPtr", "NOTNULL");
    }
     if ( testFlag = 1)
     {
           logWrite(summaryLog, "SHoundGetDomainsFromGi", "PASS", "","","");

     }
     else
     {
           logWrite(summaryLog, "SHoundGetDomainsFromGi", "FAIL", "","","");

     }
    if (srpsp != NULL) srpsp = StRpsdbFree (srpsp);

	/* SHoundGetDomainsFromGiWithEvalue */ 

/*sprintf(itos1, "%ld", myargs[0].intvalue);*/
    if (srpsp != NULL) srpsp = StRpsdbFree (srpsp);
    srpsp = SHoundGetDomainsFromGiWithEvalue(intValue, 10);
    if (srpsp == NULL)
    {
      testFlag = 0;
      logWrite(reportLog, "SHoundGetDomainsFromGiWithEvalue", "GI", itos1, "StRpsdbPtr", "NULL");
    }
    else
    {
      testFlag = 1;
      logWrite(reportLog, "SHoundGetDomainsFromGiWithEvalue", "GI", itos1, "StRpsdbPtr", "NOTNULL");
    }
    
    if (srpsp != NULL) srpsp = StRpsdbFree (srpsp);
    srpsp = SHoundGetDomainsFromGiWithEvalue(MAXINT, 10);
    if (srpsp == NULL)
    {
      logWrite(reportLog, "SHoundGetDomainsFromGiWithEvalue", "GI", "MAXINT", "StRpsdbPtr", "NULL");
    }
    else
    {
      testFlag = 0;
      logWrite(reportLog, "SHoundGetDomainsFromGiWithEvalue", "GI", "MAXINT", "StRpsdbPtr", "NOTNULL");
    }

    if (srpsp != NULL) srpsp = StRpsdbFree (srpsp);
    srpsp = SHoundGetDomainsFromGiWithEvalue(MINUSONE, 10);
    if (srpsp == NULL)
    {
      logWrite(reportLog, "SHoundGetDomainsFromGiWithEvalue", "GI", "-1", "StRpsdbPtr", "NULL");
    }
    else
    {
      testFlag = 0;
      logWrite(reportLog, "SHoundGetDomainsFromGiWithEvalue", "GI", "-1", "StRpsdbPtr", "NOTNULL");
    }
     if ( testFlag = 1)
     {
           logWrite(summaryLog, "SHoundGetDomainsFromGiWithEvalue", "PASS", "","","");

     }
     else
     {
           logWrite(summaryLog, "SHoundGetDomainsFromGiWithEvalue", "FAIL", "","","");

     }
    if (srpsp != NULL) srpsp = StRpsdbFree (srpsp);
    
        /* SHoundGetDomainsFromGiListWithEvalue */
    pvngi =  ValNodeFree(pvngi);
    if (srpsp != NULL) srpsp = StRpsdbFree (srpsp);
    intValue = 6322015;
    ValNodeAddInt(&pvngi, 0, intValue);
    ValNodeAddInt(&pvngi, 0, intValue);
    
    srpsp = SHoundGetDomainsFromGiListWithEvalue(pvngi, 10);
    if (srpsp == NULL)
    {
      testFlag = 0;
      printf("SHoundGetDomainsFromGiListWithEvalue failed.\n");
      logWrite(reportLog, "SHoundGetDomainsFromGiListWithEvalue", "GILIST", itos1, "StRpsdbPtr", "NULL");
    }
    else
    {
      testFlag = 1;
      printf("SHoundGetDomainsFromGiListWithEvalue OK\n");
      logWrite(reportLog, "SHoundGetDomainsFromGiListWithEvalue", "GILIST", itos1, "StRpsdbPtr", "NOTNULL");
    }
    
    if (srpsp != NULL) srpsp = StRpsdbFree (srpsp);
    srpsp = SHoundGetDomainsFromGiListWithEvalue(nullValNodePtr, 10);
    if (srpsp == NULL)
    {
      printf("SHoundGetDomainsFromGiListWithEvalue failed.\n");
      logWrite(reportLog, "SHoundGetDomainsFromGiListWithEvalue", "GILIST", "NULLLIST", "StRpsdbPtr", "NULL");
    }
    else
    {
      testFlag = 0;
      printf("SHoundGetDomainsFromGiListWithEvalue OK.\n");
      logWrite(reportLog, "SHoundGetDomainsFromGiListWithEvalue", "GILIST", "NULLLIST", "StRpsdbPtr", "NOTNULL");
    }

     if ( testFlag = 1)
     {
           logWrite(summaryLog, "SHoundGetDomainsFromGiListWithEvalue", "PASS", "","","");

     }
     else
     {
           logWrite(summaryLog, "SHoundGetDomainsFromGiListWithEvalue", "FAIL", "","","");

     }
    if (srpsp != NULL) srpsp = StRpsdbFree (srpsp);
    

	/* SHoundGetGisByDomainId */ 

      if (vnp != NULL) vnp = ValNodeFree(vnp);
      vnp = SHoundGetGisByDomainId("smart00038");
      if (vnp == NULL)
      {
	  printf("SHoundGetGisByDomainId failed.\n");
        testFlag = 0;
    	  logWrite(reportLog, "SHoundGetGisByDomainId", "CDDID", "smart00038", "ValNodePtr", "NULL");
      }
      else
      {
	  printf("SHoundGetGisByDomainId OK.\n");
        testFlag = 1;
	  logWrite(reportLog, "SHoundGetGisByDomainId", "CDDID", "smart00038", "ValNodePtr", "NOTNULL");
      }
	vnp = ValNodeFree(vnp);
      /* Invalid Param */ 
	vnp = SHoundGetGisByDomainId(EMPTYSTR);
	if (vnp == NULL)
      {
	  printf("SHoundGetGisByDomainId failed.\n");
	  logWrite(reportLog, "SHoundGetGisByDomainId", "CDDID", "EMPTYSTRING", "ValNodePtr", "NULL");
      }
      else
      {
        printf("SHoundGetGisByDomainId OK.\n");
        testFlag = 0;
	  logWrite(reportLog, "SHoundGetGisByDomainId", "CDDID", "EMPTYSTRING", "ValNodePtr", "NOTNULL");
      }
	vnp = ValNodeFree(vnp);
      /* Invalid Param */ 
	vnp = SHoundGetGisByDomainId(nullString);
	if (vnp == NULL)
      {
	  printf("SHoundGetGisByDomainId failed.\n");
	  logWrite(reportLog, "SHoundGetGisByDomainId", "CDDID", "NULLSTRING", "ValNodePtr", "NULL");
      }
      else
      {
        printf("SHoundGetGisByDomainId OK.\n");
        testFlag = 0;
	   logWrite(reportLog, "SHoundGetGisByDomainId", "CDDID", "NULLSTRING", "ValNodePtr", "NOTNULL");
      }
     if ( testFlag = 1)
     {
             logWrite(summaryLog, "SHoundGetGisByDomainId", "PASS", "", "", "");

     }
     else
     {
             logWrite(summaryLog, "SHoundGetGisByDomainId", "FAIL", "", "", "");

     }
	vnp = ValNodeFree(vnp);
	
       /* SHoundGetGisByDomainIdAndEvalue */ 

      if (vnp != NULL) vnp = ValNodeFree(vnp);
      vnp = SHoundGetGisByDomainIdAndEvalue("smart00038", 100);
      if (vnp == NULL)
      {
	  printf("SHoundGetGisByDomainIdAndEvalue failed.\n");
        testFlag = 0;
    	  logWrite(reportLog, "SHoundGetGisByDomainIdAndEvalue", "CDDID", "smart00038", "ValNodePtr", "NULL");
      }
      else
      {
	  printf("SHoundGetGisByDomainIdAndEvalue OK.\n");
        testFlag = 1;
	  logWrite(reportLog, "SHoundGetGisByDomainIdAndEvalue", "CDDID", "smart00038", "ValNodePtr", "NOTNULL");
      }
	vnp = ValNodeFree(vnp);
      /* Invalid Param */ 
	vnp = SHoundGetGisByDomainIdAndEvalue(EMPTYSTR, 100);
	if (vnp == NULL)
      {
	  printf("SHoundGetGisByDomainIdAndEvalue failed.\n");
	  logWrite(reportLog, "SHoundGetGisByDomainIdAndEvalue", "CDDID", "EMPTYSTRING", "ValNodePtr", "NULL");
      }
      else
      {
        printf("SHoundGetGisByDomainIdAndEvalue OK.\n");
        testFlag = 0;
	  logWrite(reportLog, "SHoundGetGisByDomainIdAndEvalue", "CDDID", "EMPTYSTRING", "ValNodePtr", "NOTNULL");
      }
	vnp = ValNodeFree(vnp);
      /* Invalid Param */ 
	vnp = SHoundGetGisByDomainIdAndEvalue(nullString, 100);
	if (vnp == NULL)
      {
	  printf("SHoundGetGisByDomainIdAndEvalue failed.\n");
	  logWrite(reportLog, "SHoundGetGisByDomainIdAndEvalue", "CDDID", "NULLSTRING", "ValNodePtr", "NULL");
      }
      else
      {
        printf("SHoundGetGisByDomainIdAndEvalue OK.\n");
        testFlag = 0;
	   logWrite(reportLog, "SHoundGetGisByDomainIdAndEvalue", "CDDID", "NULLSTRING", "ValNodePtr", "NOTNULL");
      }
     if ( testFlag = 1)
     {
             logWrite(summaryLog, "SHoundGetGisByDomainIdAndEvalue", "PASS", "", "", "");

     }
     else
     {
             logWrite(summaryLog, "SHoundGetGisByDomainIdAndEvalue", "FAIL", "", "", "");

     }
	vnp = ValNodeFree(vnp);

	/* SHoundGetGisByNumberOfDomains */ 
    intValue = 40; 
    sprintf(itos1, "%ld", intValue);
    if (vnp != NULL) vnp = ValNodeFree (vnp);
    vnp = SHoundGetGisByNumberOfDomains(intValue);
    if (vnp == NULL)
    {
      testFlag = 0;
      logWrite(reportLog, "SHoundGetGisByNumberOfDomains", "INT", itos1, "ValNodePtr", "NULL");
    }
    else
    {
      testFlag = 1;
      logWrite(reportLog, "SHoundGetGisByNumberOfDomains", "INT", itos1, "ValNodePtr", "NOTNULL");
    }
    
    if (vnp != NULL) vnp = ValNodeFree (vnp);
    vnp = SHoundGetGisByNumberOfDomains(MAXINT);
    if (vnp == NULL)
    {
      logWrite(reportLog, "SHoundGetGisByNumberOfDomains", "INT", "MAXINT", "ValNodePtr", "NULL");
    }
    else
    {
      testFlag = 0;
      logWrite(reportLog, "SHoundGetGisByNumberOfDomains", "INT", "MAXINT", "ValNodePtr", "NOTNULL");
    }

    if (vnp != NULL) vnp = ValNodeFree (vnp);
    vnp = SHoundGetGisByNumberOfDomains(MINUSONE);
    if (vnp == NULL)
    {
      logWrite(reportLog, "SHoundGetGisByNumberOfDomains", "INT", "-1", "ValNodePtr", "NULL");
    }
    else
    {
      testFlag = 0;
      logWrite(reportLog, "SHoundGetGisByNumberOfDomains", "INT", "-1", "ValNodePtr", "NOTNULL");
    }
     if ( testFlag = 1)
     {
           logWrite(summaryLog, "SHoundGetGisByNumberOfDomains", "PASS", "","","");

     }
     else
     {
           logWrite(summaryLog, "SHoundGetGisByNumberOfDomains", "FAIL", "","","");

     }
    if (vnp != NULL) vnp = ValNodeFree (vnp);
    intValue = 0;
    
	/* SHoundNeighboursFromGiEx */ 
    intValue = 6322015;
    sprintf(itos1, "%ld", intValue);
    if (flsp != NULL) flsp = FLinkSetFree (flsp);
    SHoundNeighboursFromGiEx(intValue, &flsp, 100, 1);
    if (flsp == NULL)
    {
      testFlag = 0;
      logWrite(reportLog, "SHoundNeighboursFromGiEx", "GI", itos1, "FLinkSetPtr", "NULL");
    }
    else
    {
      testFlag = 1;
      logWrite(reportLog, "SHoundNeighboursFromGiEx", "GI", itos1, "FLinkSetPtr", "NOTNULL");
    }
    
    if (flsp != NULL) flsp = FLinkSetFree (flsp);
    SHoundNeighboursFromGiEx(MAXINT, &flsp, 100, 1);
    if (flsp == NULL)
    {
      logWrite(reportLog, "SHoundNeighboursFromGiEx", "GI", "MAXINT", "FLinkSetPtr", "NULL");
    }
    else
    {
      testFlag = 0;
      logWrite(reportLog, "SHoundNeighboursFromGiEx", "GI", "MAXINT", "FLinkSetPtr", "NOTNULL");
    }

    if (flsp != NULL) flsp = FLinkSetFree (flsp);
    SHoundNeighboursFromGiEx(MINUSONE,&flsp, 100, 1);
    if (flsp == NULL)
    {
      logWrite(reportLog, "SHoundNeighboursFromGiEx", "GI", "-1", "FLinkSetPtr", "NULL");
    }
    else
    {
      testFlag = 0;
      logWrite(reportLog, "SHoundNeighboursFromGiEx", "GI", "-1", "FLinkSetPtr", "NOTNULL");
    }
     if ( testFlag = 1)
     {
           logWrite(summaryLog, "SHoundNeighboursFromGiEx", "PASS", "","","");

     }
     else
     {
           logWrite(summaryLog, "SHoundNeighboursFromGiEx", "FAIL", "","","");

     }
    if (flsp != NULL) flsp = FLinkSetFree (flsp);
    
	/* SHoundNeighboursFromGi */
    sprintf(itos1, "%ld", myargs[0].intvalue);
    if (flsp != NULL) flsp = FLinkSetFree (flsp);
    SHoundNeighboursFromGi(myargs[0].intvalue,&flsp, 100, 1);
    if (flsp == NULL)
    {
      testFlag = 0;
      logWrite(reportLog, "SHoundNeighboursFromGi", "GI", itos1, "FLinkSetPtr", "NULL");
    }
    else
    {
      testFlag = 1;
      logWrite(reportLog, "SHoundNeighboursFromGi", "GI", itos1, "FLinkSetPtr", "NOTNULL");
    }
    
    if (flsp != NULL) flsp = FLinkSetFree (flsp);
    SHoundNeighboursFromGi(MAXINT, &flsp, 100, 1);
    if (flsp == NULL)
    {
      logWrite(reportLog, "SHoundNeighboursFromGi", "GI", "MAXINT", "FLinkSetPtr", "NULL");
    }
    else
    {
      testFlag = 0;
      logWrite(reportLog, "SHoundNeighboursFromGi", "GI", "MAXINT", "FLinkSetPtr", "NOTNULL");
    }

    if (flsp != NULL) flsp = FLinkSetFree (flsp);
    SHoundNeighboursFromGi(MINUSONE,&flsp, 100, 1);
    if (flsp == NULL)
    {
      logWrite(reportLog, "SHoundNeighboursFromGi", "GI", "-1", "FLinkSetPtr", "NULL");
    }
    else
    {
      testFlag = 0;
      logWrite(reportLog, "SHoundNeighboursFromGi", "GI", "-1", "FLinkSetPtr", "NOTNULL");
    }
     if ( testFlag = 1)
     {
           logWrite(summaryLog, "SHoundNeighboursFromGi", "PASS", "","","");

     }
     else
     {
           logWrite(summaryLog, "SHoundNeighboursFromGi", "FAIL", "","","");

     }
    if (flsp != NULL) flsp = FLinkSetFree (flsp);
	
	/* SHoundNeighboursFromGiList */ 
      if ( pvngi == NULL)
      {
	  ValNodeAddInt(&pvngi, 0, myargs[0].intvalue);
	  ValNodeAddInt(&pvngi, 0, myargs[0].intvalue);
	  ValNodeAddInt(&pvngi, 0, myargs[0].intvalue);
      }

    if (flslp != NULL) flslp = FLinkSetListFree (flslp);
    SHoundNeighboursFromGiList(pvngi,&flslp, 100);
    if (flslp == NULL)
    {
      testFlag = 0;
      logWrite(reportLog, "SHoundNeighboursFromGiList", "ValNodePtr", "NOTNULL", "FLinkSetListPtr", "NULL");
    }
    else
    {
      testFlag = 1;
      logWrite(reportLog, "SHoundNeighboursFromGiList", "ValNodePtr", "NOTNULL", "FLinkSetListPtr", "NOTNULL");
    }
    
    if (flslp != NULL) flslp = FLinkSetListFree (flslp);
    SHoundNeighboursFromGiList(nullValNodePtr, &flslp, 100);
    if (flslp == NULL)
    {
      logWrite(reportLog, "SHoundNeighboursFromGiList", "ValNodePtr", "NULL", "FLinkSetListPtr", "NULL");
    }
    else
    {
      testFlag = 0;
      logWrite(reportLog, "SHoundNeighboursFromGiList", "ValNodePtr", "NULL", "FLinkSetListPtr", "NOTNULL");
    }
     if ( testFlag = 1)
     {
           logWrite(summaryLog, "SHoundNeighboursFromGiList", "PASS", "","","");

     }
     else
     {
           logWrite(summaryLog, "SHoundNeighboursFromGiList", "FAIL", "","","");

     }
    if (flslp != NULL) flslp = FLinkSetListFree (flslp);

	/* SHoundNeighboursFromTaxID */ 

    sprintf(itos1, "%ld", myargs[3].intvalue);
    if (flslp != NULL) flslp = FLinkSetListFree (flslp);
    SHoundNeighboursFromTaxID(myargs[3].intvalue, &flslp, 1);
    if (flslp == NULL)
    {
      testFlag = 0;
      logWrite(reportLog, "SHoundNeighboursFromTaxID", "TAXID", itos1, "FLinkSetListPtr", "NULL");
    }
    else
    {
      testFlag = 1;
      logWrite(reportLog, "SHoundNeighboursFromTaxID", "TAXID", itos1, "FLinkSetListPtr", "NOTNULL");
    }
    
    if (flslp != NULL) flslp = FLinkSetListFree (flslp);
    SHoundNeighboursFromTaxID(MAXINT, &flslp, 1);
    if (flslp == NULL)
    {
      logWrite(reportLog, "SHoundNeighboursFromTaxID", "TAXID", "MAXINT", "FLinkSetListPtr", "NULL");
    }
    else
    {
      testFlag = 0;
      logWrite(reportLog, "SHoundNeighboursFromTaxID", "TAXID", "MAXINT", "FLinkSetListPtr", "NOTNULL");
    }

    if (flslp != NULL) flslp = FLinkSetListFree (flslp);
    SHoundNeighboursFromTaxID(MINUSONE, &flslp, 1);
    if (flslp == NULL)
    {
      logWrite(reportLog, "SHoundNeighboursFromTaxID", "TAXID", "-1", "FLinkSetListPtr", "NULL");
    }
    else
    {
      testFlag = 0;
      logWrite(reportLog, "SHoundNeighboursFromTaxID", "TAXID", "-1", "FLinkSetListPtr", "NOTNULL");
    }
     if ( testFlag = 1)
     {
           logWrite(summaryLog, "SHoundNeighboursFromTaxID", "PASS", "","","");

     }
     else
     {
           logWrite(summaryLog, "SHoundNeighboursFromTaxID", "FAIL", "","","");

     }
    if (flslp != NULL) flslp = FLinkSetListFree (flslp);

	/* SHoundNeighboursOfNeighbours */ 

    sprintf(itos1, "%ld", myargs[0].intvalue);
    if (flslp != NULL) flslp = FLinkSetListFree (flslp);
    SHoundNeighboursOfNeighbours(myargs[0].intvalue, &flslp, 1);
    if (flslp == NULL)
    {
      testFlag = 0;
      logWrite(reportLog, "SHoundNeighboursOfNeighbours", "GI", itos1, "FLinkSetListPtr", "NULL");
    }
    else
    {
      testFlag = 1;
      logWrite(reportLog, "SHoundNeighboursOfNeighbours", "GI", itos1, "FLinkSetListPtr", "NOTNULL");
    }
    
    if (flslp != NULL) flslp = FLinkSetListFree (flslp);
    SHoundNeighboursOfNeighbours(MAXINT, &flslp, 1);
    if (flslp == NULL)
    {
      logWrite(reportLog, "SHoundNeighboursOfNeighbours", "GI", "MAXINT", "FLinkSetListPtr", "NULL");
    }
    else
    {
      testFlag = 0;
      logWrite(reportLog, "SHoundNeighboursOfNeighbours", "GI", "MAXINT", "FLinkSetListPtr", "NOTNULL");
    }

    if (flslp != NULL) flslp = FLinkSetListFree (flslp);
    SHoundNeighboursOfNeighbours(MINUSONE, &flslp, 1);
    if (flslp == NULL)
    {
      logWrite(reportLog, "SHoundNeighboursOfNeighbours", "GI", "-1", "FLinkSetListPtr", "NULL");
    }
    else
    {
      testFlag = 0;
      logWrite(reportLog, "SHoundNeighboursOfNeighbours", "GI", "-1", "FLinkSetListPtr", "NOTNULL");
    }
     if ( testFlag = 1)
     {
           logWrite(summaryLog, "SHoundNeighboursOfNeighbours", "PASS", "","","");

     }
     else
     {
           logWrite(summaryLog, "SHoundNeighboursOfNeighbours", "FAIL", "","","");

     }
    if (flslp != NULL) flslp = FLinkSetListFree (flslp);

	/* SHoundNeighboursOfNeighboursList */ 
          

	    
    if ( pvngi == NULL) {
	    
      ValNodeAddInt(&pvngi, 0, myargs[0].intvalue);
      ValNodeAddInt(&pvngi, 0, myargs[0].intvalue);
      ValNodeAddInt(&pvngi, 0, myargs[0].intvalue);
    }

    if (flsl2p != NULL) flsl2p = FLinkSetList2Free (flsl2p);
    SHoundNeighboursOfNeighboursList(pvngi,&flsl2p, 100);
    if (flsl2p == NULL)
    {
      testFlag = 0;
      logWrite(reportLog, "SHoundNeighboursOfNeighboursList", "ValNodePtr", "NOTNULL", "FLinkSetList2Ptr", "NULL");
    }
    else
    {
      testFlag = 1;
      logWrite(reportLog, "SHoundNeighboursOfNeighboursList", "ValNodePtr", "NOTNULL", "FLinkSetList2Ptr", "NOTNULL");
    }
    
    if (flsl2p != NULL) flsl2p = FLinkSetList2Free (flsl2p);
    SHoundNeighboursOfNeighboursList(nullValNodePtr, &flsl2p, 100);
    if (flsl2p == NULL)
    {
      logWrite(reportLog, "SHoundNeighboursOfNeighboursList", "ValNodePtr", "NULL", "FLinkSetList2Ptr", "NULL");
    }
    else
    {
      testFlag = 0;
      logWrite(reportLog, "SHoundNeighboursOfNeighboursList", "ValNodePtr", "NULL", "FLinkSetList2Ptr", "NOTNULL");
    }
     if ( testFlag = 1)
     {
           logWrite(summaryLog, "SHoundNeighboursOfNeighboursList", "PASS", "","","");

     }
     else
     {
           logWrite(summaryLog, "SHoundNeighboursOfNeighboursList", "FAIL", "","","");

     }
    if (flsl2p != NULL) flsl2p = FLinkSetList2Free (flsl2p);
    

	/* SHoundNumNeighboursInDB */ 
    ret = SLRI_FAIL;
    ret = SHoundNumNeighboursInDB(&NumNeighbours);
    if (ret != SLRI_SUCCESS)
    {
      testFlag = 0;
      logWrite(reportLog, "SHoundNumNeighboursInDB", "none", "N/A", "Integer Ptr", "0");
    }
    else
    {
      testFlag = 1;
      logWrite(reportLog, "SHoundNumNeighboursInDB", "none", "N/A", "Integer Ptr", "Not 0");
    }
     if ( testFlag = 1)
     {
           logWrite(summaryLog, "SHoundNumNeighboursInDB", "PASS", "","","");

     }
     else
     {
           logWrite(summaryLog, "SHoundNumNeighboursInDB", "FAIL", "","","");

     }

/* Code commented out as the function is deprecated 
    sprintf(itos1, "%ld", intValue);
    if (flsl2p != NULL) flsl2p = FLinkSetList2Free (flsl2p);
    SHoundNeighboursOfNeighboursFromTaxID(intValue, &flsl2p, 100);
    if (flsl2p == NULL)
    {
      testFlag = 0;
      logWrite(reportLog, "SHoundNeighboursOfNeighboursFromTaxID", "TAXID", itos1, "FLinkSetList2Ptr", "NULL");
    }
    else
    {
      testFlag = 1;
      logWrite(reportLog, "SHoundNeighboursOfNeighboursFromTaxID", "TAXID", itos1, "FLinkSetList2Ptr", "NOTNULL");
    }
    
    if (flsl2p != NULL) flsl2p = FLinkSetList2Free (flsl2p);
    SHoundNeighboursOfNeighboursFromTaxID(MAXINT, &flsl2p, 100);
    if (flsl2p == NULL)
    {
      logWrite(reportLog, "SHoundNeighboursOfNeighboursFromTaxID", "TAXID", "MAXINT", "FLinkSetList2Ptr", "NULL");
    }
    else
    {
      testFlag = 0;
      logWrite(reportLog, "SHoundNeighboursOfNeighboursFromTaxID", "TAXID", "MAXINT", "FLinkSetList2Ptr", "NOTNULL");
    }

    if (flsl2p != NULL) flsl2p = FLinkSetList2Free (flsl2p);
    SHoundNeighboursOfNeighboursFromTaxID(MINUSONE, &flsl2p, 100);
    if (flsl2p == NULL)
    {
      logWrite(reportLog, "SHoundNeighboursOfNeighboursFromTaxID", "TAXID", "-1", "FLinkSetList2Ptr", "NULL");
    }
    else
    {
      testFlag = 0;
      logWrite(reportLog, "SHoundNeighboursOfNeighboursFromTaxID", "TAXID", "-1", "FLinkSetList2Ptr", "NOTNULL");
    }
     if ( testFlag = 1)
     {
           logWrite(summaryLog, "SHoundNeighboursOfNeighboursFromTaxID", "PASS", "","","");

     }
     else
     {
           logWrite(summaryLog, "SHoundNeighboursOfNeighboursFromTaxID", "FAIL", "","","");

     }
    if (flsl2p != NULL) flsl2p = FLinkSetList2Free (flsl2p);
*/

  
 /*----------------------------------------------------------
    SHoundIsProtein
   ----------------------------------------------------------*/
    intValue = 6322015;
    sprintf(itos1, "%ld", intValue);
   
    if( !SHoundIsProtein(intValue))
    {
      testFlag = 0;
      logWrite(reportLog, " SHoundIsProtein", "GI", itos1, "Boolean", "FALSE");
    }
    else
    {
      testFlag = 1;
      logWrite(reportLog, " SHoundIsProtein", "GI", itos1, "Boolean", "TRUE");
    }
    
    if( SHoundIsProtein(MAXINT))
    {
      testFlag = 0;
      logWrite(reportLog, " SHoundIsProtein", "GI", "MAXINT", "Boolean", "TRUE");
    }
    else
    {
      testFlag = 1;
      logWrite(reportLog, " SHoundIsProtein", "GI", "MAXINT", "Boolean", "FALSE");
    }
    
    if( SHoundIsProtein(MINUSONE))
    {
      testFlag = 0;
      logWrite(reportLog, " SHoundIsProtein", "GI", "-1", "Boolean", "TRUE");
    }
    else
    {
      testFlag = 1;
      logWrite(reportLog, " SHoundIsProtein", "GI", "-1", "Boolean", "FALSE");
    }
     if ( testFlag = 1)
     {
           logWrite(summaryLog, "SHoundIsProtein", "PASS", "","","");

     }
     else
     {
           logWrite(summaryLog, "SHoundIsProtein", "FAIL", "","","");

     }


 /*-----------------------------------------------------------
     SHoundMoleculeType
     -----------------------------------------------------------*/
    intValue = 6322015;
    sprintf(itos1, "%ld", intValue);
    if (pchar == NULL) MemFree (pchar);
    pchar = SHoundMoleculeType(intValue);
    if (pchar == NULL)
    {
      testFlag = 0;
      logWrite(reportLog, "SHoundMoleculeType", "GI", itos1, "Pointer", "NULL");
    }
    else
    {
      testFlag = 1;
      logWrite(reportLog, "SHoundMoleculeType", "GI", itos1, "CharPtr", pchar);
    }
    
    if (pchar == NULL) MemFree (pchar);
    pchar =SHoundMoleculeType(MAXINT);
    if (pchar == NULL)
    {
      logWrite(reportLog, "SHoundMoleculeType", "GI", "MAXINT", "Pointer", "NULL");
    }
    else
    {
      testFlag = 0;
      logWrite(reportLog, "SHoundMoleculeType", "GI", "MAXINT", "CharPtr", pchar);
    }

    if (pchar == NULL) MemFree (pchar);
    pchar =SHoundMoleculeType(MINUSONE);
    if (pchar == NULL)
    {
      logWrite(reportLog, "SHoundMoleculeType", "GI", "-1", "Pointer", "NULL");
    }
    else
    {
      testFlag = 0;
      logWrite(reportLog, "SHoundMoleculeType", "GI", "-1", "CharPtr", pchar);
    }
     if ( testFlag = 1)
     {
           logWrite(summaryLog, "SHoundMoleculeType", "PASS", "","","");

     }
     else
     {
           logWrite(summaryLog, "SHoundMoleculeType", "FAIL", "","","");

     }


 /*-----------------------------------------------------------
     SHoundSequenceLength
     -----------------------------------------------------------*/

    sprintf(itos1, "%ld", myargs[0].intvalue);
    intValue = 0;
    intValue = SHoundSequenceLength(myargs[0].intvalue);
    sprintf(itos2, "%ld", intValue);
    if (intValue <= 0)
    {
      testFlag = 0;
      logWrite(reportLog, "SHoundSequenceLength", "GI", itos1, "INT", itos2);
    }
    else
    {
      testFlag = 1;
      logWrite(reportLog, "SHoundSequenceLength", "GI", itos1, "INT", itos2);
    }
    
    intValue = 0;
    intValue = SHoundSequenceLength(MAXINT);
    sprintf(itos2, "%ld", intValue);
    if (intValue <= 0)
    {
      logWrite(reportLog, "SHoundSequenceLength", "GI", "MAXINT", "INT", itos2);
    }
    else
    {
      testFlag = 0;
      logWrite(reportLog, "SHoundSequenceLength", "GI", "MAXINT", "INT", itos2);
    }

    intValue = 0;
    intValue = SHoundSequenceLength(MINUSONE);
    sprintf(itos2, "%ld", intValue);
    if (intValue <= 0)
    {
      logWrite(reportLog, "SHoundSequenceLength", "GI", "-1", "INT", itos2);
    }
    else
    {
      testFlag = 0;
      logWrite(reportLog, "SHoundSequenceLength", "GI", "-1", "INT", itos2);
    }
     if ( testFlag = 1)
     {
           logWrite(summaryLog, "SHoundSequenceLength", "PASS", "","","");

     }
     else
     {
           logWrite(summaryLog, "SHoundSequenceLength", "FAIL", "","","");

     }

  /*------------------------------------------
    SHoundGetLargestMMDBID
    ------------------------------------------*/
    
    intValue = 0;
    intValue = SHoundGetLargestMMDBID();
    sprintf(itos2, "%ld", intValue);
    if (intValue <= 0)
    {
      testFlag = 0;
      logWrite(reportLog, "SHoundGetLargestMMDBID", "", itos1, "INT", itos2);
    }
    else
    {
      testFlag = 1;
      logWrite(reportLog, "SHoundGetLargestMMDBID", "", itos1, "INT", itos2);
    }
     if ( testFlag = 1)
     {
           logWrite(summaryLog, "SHoundGetLargestMMDBID", "PASS", "","","");

     }
     else
     {
           logWrite(summaryLog, "SHoundGetLargestMMDBID", "FAIL", "","","");

     }
    
   /*--------------------------------------
     SHoundGetNameByGi
     --------------------------------------*/
    
    /* Valid Call */
    pname = SHoundGetNameByGi(myargs[0].intvalue);
    if(pname == NULL)
    {
       sprintf(itos1, "%ld", myargs[0].intvalue);
       logWrite(reportLog, "SHoundGetNameByGi",  "GI", itos1, "CharPtr", "NULL");
       printf("SHoundGetNameByGi failed.\n");
       testFlag = 0;
    }
    else
    {
       logWrite(reportLog, "SHoundGetNameByGi",  "GI", itos1, "CharPtr", "NOTNULL");
       testFlag = 1;
       printf("SHoundGetNameByGi OK.\n");
       MemFree (pname);
    }
    /**** Invalid GI input ****/
    pname = SHoundGetNameByGi(MAXINT);
    if(pname == NULL)
    {
       logWrite(reportLog, "SHoundGetNameByGi",  "GI", "MAXINT", "CharPtr", "NULL");
       printf("SHoundTitleFromGi failed.\n");
    }
    else
    {
       logWrite(reportLog, "SHoundTitleFromGi",  "GI", "MAXINT", "CharPtr", "NOTNULL");
       printf("SHoundGetNameByGi OK.\n");
       testFlag = 0;
       MemFree(pname);
    }
    /**** GI = -1 as input ****/
    pname = NULL;
    pname = SHoundGetNameByGi(MINUSONE);
    if(pname == NULL)
    {
       logWrite(reportLog, "SHoundGetNameByGi",  "GI", "-1", "CharPtr", "NULL");
       printf("SHoundGetNameByGi failed.\n");
    }
    else
    {
       logWrite(reportLog, "SHoundGetNameByGi",  "GI", "-1", "CharPtr", "NOTNULL");
       printf("SHoundGetNameByGi OK.\n");
       testFlag = 0;
       MemFree(pname);
    } 
     if ( testFlag = 1)
     {
           logWrite(summaryLog, "SHoundGetNameByGi", "PASS", "", "", "");

     }
     else
     {
           logWrite(summaryLog, "SHoundGetNameByGi", "FAIL", "", "", "");

     }


     

     /*--------------------------------
       SHoundGetReferenceIDFromGi
       --------------------------------*/ 
      intValue = 6322016;
      if (vnp != NULL) vnp = ValNodeFree(vnp);
      sprintf(itos1, "%ld", intValue);
      vnp = SHoundGetReferenceIDFromGi(intValue, 2);
      if (vnp == NULL)
      {
	  printf("SHoundGetReferenceIDFromGi failed.\n");
        testFlag = 0;
    	  logWrite(reportLog, "SHoundGetReferenceIDFromGi", "GI/RefType", itos2, "ValNodePtr", "NULL");
      }
      else
      {
	  printf("SHoundGetReferenceIDFromGi OK.\n");
        testFlag = 1;
	  logWrite(reportLog, "SHoundGetReferenceIDFromGi", "GI/RefType", itos2, "ValNodePtr", "NOTNULL");
      }
	vnp = ValNodeFree(vnp);
      /* Invalid Param */ 
	vnp = SHoundGetReferenceIDFromGi(MAXINT,2);
	if (vnp == NULL)
      {
	  printf("SHoundGetReferenceIDFromGi failed.\n");
	  logWrite(reportLog, "SHoundGetReferenceIDFromGi", "GI/RefType", "MAXINT", "ValNodePtr", "NULL");
      }
      else
      {
        printf("SHoundGetReferenceIDFromGi OK.\n");
        testFlag = 0;
	  logWrite(reportLog, "SHoundGetReferenceIDFromGi", "GI/RefType", "MAXINT", "ValNodePtr", "NOTNULL");
      }
	vnp = ValNodeFree(vnp);
      /* Invalid Param */ 
	vnp = SHoundGetReferenceIDFromGi(MINUSONE,2);
	if (vnp == NULL)
      {
	  printf("SHoundGetReferenceIDFromGi failed.\n");
	  logWrite(reportLog, "SHoundGetReferenceIDFromGi", "GI/RefType", "-1", "ValNodePtr", "NULL");
      }
      else
      {
        printf("SHoundGetReferenceIDFromGi OK.\n");
        testFlag = 0;
	   logWrite(reportLog, "SHoundGetReferenceIDFromGi", "GI/RefType", "-1", "ValNodePtr", "NOTNULL");
      }
     if ( testFlag = 1)
     {
             logWrite(summaryLog, "SHoundGetReferenceIDFromGi", "PASS", "", "", "");

     }
     else
     {
             logWrite(summaryLog, "SHoundGetReferenceIDFromGi", "FAIL", "", "", "");

     }
	vnp = ValNodeFree(vnp);


       /* SHoundGetReferenceIDFromGiList */ 

      if (pvngi == NULL)
      {
        ValNodeAddInt(&pvngi, 0, 6322016);
        ValNodeAddInt(&pvngi, 0, 4557225);
        ValNodeAddInt(&pvngi, 0, 21071030);
      }
      if (vnp != NULL) vnp = ValNodeFree(vnp);
      vnp = SHoundGetReferenceIDFromGiList(pvngi, 2);
      if (vnp == NULL)
      {
	  printf("SHoundGetReferenceIDFromGiList failed.\n");
        testFlag = 0;
    	  logWrite(reportLog, "SHoundGetReferenceIDFromGiList", "ValNodePtr/RefType", "NOTNULL", "ValNodePtr", "NULL");
      }
      else
      {
	  printf("SHoundGetReferenceIDFromGiList OK.\n");
        testFlag = 1;
	  logWrite(reportLog, "SHoundGetReferenceIDFromGiList", "ValNodePtr/RefType", "NOTNULL", "ValNodePtr", "NOTNULL");
      }
	vnp = ValNodeFree(vnp);
      /* Invalid Param */ 
	vnp = SHoundGetReferenceIDFromGiList(nullValNodePtr,2);
	if (vnp == NULL)
      {
	  printf("SHoundGetReferenceIDFromGiList failed.\n");
	  logWrite(reportLog, "SHoundGetReferenceIDFromGiList", "ValNodePtr/RefType", "NULL", "ValNodePtr", "NULL");
      }
      else
      {
        printf("SHoundGetReferenceIDFromGiList OK.\n");
        testFlag = 0;
	   logWrite(reportLog, "SHoundGetReferenceIDFromGiList", "ValNodePtr/RefType", "NULL", "ValNodePtr", "NOTNULL");
      }
     if ( testFlag = 1)
     {
             logWrite(summaryLog, "SHoundGetReferenceIDFromGiList", "PASS", "", "", "");

     }
     else
     {
             logWrite(summaryLog, "SHoundGetReferenceIDFromGiList", "FAIL", "", "", "");

     }
	vnp = ValNodeFree(vnp);

       /* SHoundGiFromReferenceID */ 

      if (vnp != NULL) vnp = ValNodeFree(vnp);
/* Reference ID which can be either an identifier in Pubmed (ref type 1)or Medline (ref type 2)*/
      intValue = 85190481; 
      sprintf(itos1, "%ld", intValue);
      vnp = SHoundGiFromReferenceID(intValue, 2);
      if (vnp == NULL)
      {
	  printf("SHoundGiFromReferenceID failed.\n");
        testFlag = 0;
    	  logWrite(reportLog, "SHoundGiFromReferenceID", "RefID", itos1, "ValNodePtr", "NULL");
      }
      else
      {
	  printf("SHoundGiFromReferenceID OK.\n");
        testFlag = 1;
	  logWrite(reportLog, "SHoundGiFromReferenceID", "RefID", itos1, "ValNodePtr", "NOTNULL");
      }
	vnp = ValNodeFree(vnp);
      /* Invalid Param */ 
	vnp = SHoundGiFromReferenceID(MAXINT,2);
	if (vnp == NULL)
      {
	  printf("SHoundGiFromReferenceID failed.\n");
	  logWrite(reportLog, "SHoundGiFromReferenceID", "RefID", "MAXINT", "ValNodePtr", "NULL");
      }
      else
      {
        printf("SHoundGiFromReferenceID OK.\n");
        testFlag = 0;
	  logWrite(reportLog, "SHoundGiFromReferenceID", "RefID", "MAXINT", "ValNodePtr", "NOTNULL");
      }
	vnp = ValNodeFree(vnp);
      /* Invalid Param */ 
	vnp = SHoundGiFromReferenceID(MINUSONE,2);
	if (vnp == NULL)
      {
	  printf("SHoundGiFromReferenceID failed.\n");
	  logWrite(reportLog, "SHoundGiFromReferenceID", "RefID", "-1", "ValNodePtr", "NULL");
      }
      else
      {
        printf("SHoundGiFromReferenceID OK.\n");
        testFlag = 0;
	   logWrite(reportLog, "SHoundGiFromReferenceID", "RefID", "-1", "ValNodePtr", "NOTNULL");
      }
     if ( testFlag = 1)
     {
             logWrite(summaryLog, "SHoundGiFromReferenceID", "PASS", "", "", "");

     }
     else
     {
             logWrite(summaryLog, "SHoundGiFromReferenceID", "FAIL", "", "", "");

     }
	vnp = ValNodeFree(vnp);


       /* SHoundGiFromReferenceList */ 
/*ValNodePtr valNodeRefList = NULL;*/

        ValNodeAddInt(&valNodeRefList, 0, 97002444);
        ValNodeAddInt(&valNodeRefList, 0, 85190481);
        ValNodeAddInt(&valNodeRefList, 0, 25800776);

      if (vnp != NULL) vnp = ValNodeFree(vnp);
      vnp = SHoundGiFromReferenceList(valNodeRefList, 2);
      if (vnp == NULL)
      {
	  printf("SHoundGiFromReferenceList failed.\n");
        testFlag = 0;
    	  logWrite(reportLog, "SHoundGiFromReferenceList", "ValNodePtr/RefType", "NOTNULL", "ValNodePtr", "NULL");
      }
      else
      {
	  printf("SHoundGiFromReferenceList OK.\n");
        testFlag = 1;
	  logWrite(reportLog, "SHoundGiFromReferenceList", "ValNodePtr/RefType", "NOTNULL", "ValNodePtr", "NOTNULL");
      }
	vnp = ValNodeFree(vnp);
      /* Invalid Param */ 
	vnp = SHoundGiFromReferenceList(nullValNodePtr,1);
	if (vnp == NULL)
      {
	  printf("SHoundGiFromReferenceList failed.\n");
	  logWrite(reportLog, "SHoundGiFromReferenceList", "ValNodePtr/RefType", "NULL", "ValNodePtr", "NULL");
      }
      else
      {
        printf("SHoundGiFromReferenceList OK.\n");
        testFlag = 0;
	   logWrite(reportLog, "SHoundGiFromReferenceList", "ValNodePtr/RefType", "NULL", "ValNodePtr", "NOTNULL");
      }
     if ( testFlag = 1)
     {
             logWrite(summaryLog, "SHoundGiFromReferenceList", "PASS", "", "", "");

     }
     else
     {
             logWrite(summaryLog, "SHoundGiFromReferenceList", "FAIL", "", "", "");

     }
	vnp = ValNodeFree(vnp);

 /*-----------------------------------------------------------
     SHoundGetAccFromRetiredGi
     -----------------------------------------------------------*/
    intValue = 800;
    sprintf(itos1, "%ld", intValue);
    if (pchar != NULL) MemFree(pchar);
    pchar =SHoundGetAccFromRetiredGi(intValue);
    if (pchar == NULL)
    {
      testFlag = 0;
      logWrite(reportLog, "SHoundGetAccFromRetiredGi", "GI", itos1, "POINTER", "NULL");
    }
    else
    {
      testFlag = 1;
      logWrite(reportLog, "SHoundGetAccFromRetiredGi", "GI", itos1, "POINTER", "NOTNULL");
    }
    
    if (pchar != NULL) MemFree(pchar);
    pchar =SHoundGetAccFromRetiredGi(MAXINT);
    if (pchar == NULL)
    {
      logWrite(reportLog, "SHoundGetAccFromRetiredGi", "GI", "MAXINT", "POINTER", "NULL");
    }
    else
    {
      testFlag = 0;
      logWrite(reportLog, "SHoundGetAccFromRetiredGi", "GI", "MAXINT", "POINTER", "NOTNULL");
    }

    if (pchar != NULL) MemFree(pchar);
    pchar =SHoundGetAccFromRetiredGi(MINUSONE);
    if (pchar == NULL)
    {
      logWrite(reportLog, "SHoundGetAccFromRetiredGi", "GI", "-1", "POINTER", "NULL");
    }
    else
    {
      testFlag = 0;
      logWrite(reportLog, "SHoundGetAccFromRetiredGi", "GI", "-1", "POINTER", "NOTNULL");
    }
     if ( testFlag = 1)
     {
           logWrite(summaryLog, "SHoundGetAccFromRetiredGi", "PASS", "","","");

     }
     else
     {
           logWrite(summaryLog, "SHoundGetAccFromRetiredGi", "FAIL", "","","");

     }


       /* SHoundGiFromGOIDAndECode */ 
      intValue = 5554;
      if (vnp != NULL) vnp = ValNodeFree(vnp);
      sprintf(itos1, "%ld", intValue);
      vnp = SHoundGiFromGOIDAndECode(intValue, "NR");
      if (vnp == NULL)
      {
	  printf("SHoundGiFromGOIDAndECode failed.\n");
        testFlag = 0;
    	  logWrite(reportLog, "SHoundGiFromGOIDAndECode", "GOID/Ecode", itos1, "ValNodePtr", "NULL");
      }
      else
      {
	  printf("SHoundGiFromGOIDAndECode OK.\n");
        testFlag = 1;
	  logWrite(reportLog, "SHoundGiFromGOIDAndECode", "GOID/Ecode", itos1, "ValNodePtr", "NOTNULL");
      }
	vnp = ValNodeFree(vnp);
      /* Invalid Param */ 
	vnp = SHoundGiFromGOIDAndECode(MAXINT,"ND");
	if (vnp == NULL)
      {
	  printf("SHoundGiFromGOIDAndECode failed.\n");
	  logWrite(reportLog, "SHoundGiFromGOIDAndECode", "GOID/Ecode", "MAXINT", "ValNodePtr", "NULL");
      }
      else
      {
        printf("SHoundGiFromGOIDAndECode OK.\n");
        testFlag = 0;
	  logWrite(reportLog, "SHoundGiFromGOIDAndECode", "GOID/Ecode", "MAXINT", "ValNodePtr", "NOTNULL");
      }
	vnp = ValNodeFree(vnp);
      /* Invalid Param */ 
	vnp = SHoundGiFromGOIDAndECode(MINUSONE,"ND");
	if (vnp == NULL)
      {
	  printf("SHoundGiFromGOIDAndECode failed.\n");
	  logWrite(reportLog, "SHoundGiFromGOIDAndECode", "GOID/Ecode", "-1", "ValNodePtr", "NULL");
      }
      else
      {
        printf("SHoundGiFromGOIDAndECode OK.\n");
        testFlag = 0;
	   logWrite(reportLog, "SHoundGiFromGOIDAndECode", "GOID/Ecode", "-1", "ValNodePtr", "NOTNULL");
      }
     if ( testFlag = 1)
     {
             logWrite(summaryLog, "SHoundGiFromGOIDAndECode", "PASS", "", "", "");

     }
     else
     {
             logWrite(summaryLog, "SHoundGiFromGOIDAndECode", "FAIL", "", "", "");

     }
	vnp = ValNodeFree(vnp);


       /* SHoundGiFromGOIDListAndECode */ 
      if (vnp != NULL) vnp = ValNodeFree(vnp);
/*  ValNodePtr knownGoList = NULL;*/
    ValNodeAddInt(&knownGoList, 0, 5554);
      vnp = SHoundGiFromGOIDListAndECode(knownGoList, "NR");
      if (vnp == NULL)
      {
	  printf("SHoundGiFromGOIDListAndECode failed.\n");
        testFlag = 0;
    	  logWrite(reportLog, "SHoundGiFromGOIDListAndECode", "ValNodePtr/Ecode", "NOTNULL", "ValNodePtr", "NULL");
      }
      else
      {
	  printf("SHoundGiFromGOIDListAndECode OK.\n");
        testFlag = 1;
	  logWrite(reportLog, "SHoundGiFromGOIDListAndECode", "ValNodePtr/Ecode", "NOTNULL", "ValNodePtr", "NOTNULL");
      }
	vnp = ValNodeFree(vnp);
      
      /* Invalid Param */ 
	vnp = SHoundGiFromGOIDListAndECode(nullValNodePtr,"ND");
	if (vnp == NULL)
      {
	  printf("SHoundGiFromGOIDListAndECode failed.\n");
	  logWrite(reportLog, "SHoundGiFromGOIDListAndECode", "ValNodePtr/Ecode", "NULL", "ValNodePtr", "NULL");
      }
      else
      {
        printf("SHoundGiFromGOIDListAndECode OK.\n");
        testFlag = 0;
	   logWrite(reportLog, "SHoundGiFromGOIDListAndECode", "ValNodePtr/Ecode", "NULL", "ValNodePtr", "NOTNULL");
      }
     if ( testFlag = 1)
     {
             logWrite(summaryLog, "SHoundGiFromGOIDListAndECode", "PASS", "", "", "");

     }
     else
     {
             logWrite(summaryLog, "SHoundGiFromGOIDListAndECode", "FAIL", "", "", "");

     }
      vnp = ValNodeFree(vnp);
	
	
	
    /*-----------------------
     *SHoundGetFastaKeyedList
     -----------------------*/	
    pvngi = ValNodeFree(pvngi);
    ValNodeAddInt(&pvngi, 0, 666);
    ValNodeAddInt(&pvngi, 0, 4557225);
    
    pvnret = SHoundGetFastaKeyedList(pvngi);
    if (pvnret == NULL)
    {
      printf("SHoundGetFastaKeyedList failed.\n");
      testFlag = 0;
      logWrite(reportLog, "SHoundGetFastaKeyedList", "ValNodePtr", "NOTNULL", "Pointer","NULL");
    }
    else
    {
      printf("SHoundGetFastaKeyedList OK.\n");
      testFlag = 1;
      logWrite(reportLog, "SHoundGetFastaKeyedList", "ValNodePtr", "NOTNULL", "Pointer","NOTNULL");
    }
    Slri_FreeFastaKeyedList(pvnret);

    /* Invalid Param */
    pvnret= SHoundGetFastaList(nullValNodePtr);
    if (pvnret == NULL)
    {
      printf("SHoundGetFastaKeyedList failed.\n");
      logWrite(reportLog, "SHoundGetFastaKeyedList", "ValNodePtr", "NULL", "Pointer","NULL");
    }
    else
    {
      printf("SHoundGetFastaKeyedList OK.\n");
      testFlag = 0;
      logWrite(reportLog, "SHoundGetFastaKeyedList", "ValNodePtr", "NULL", "Pointer","NOTNULL");
    }
     if ( testFlag = 1)
     {
           logWrite(summaryLog, "SHoundGetFastaKeyedList", "PASS", "", "", "");

     }
     else
     {
           logWrite(summaryLog, "SHoundGetFastaKeyedList", "FAIL", "", "", "");

     }
    Slri_FreeFastaKeyedList(pvnret);

    /*---------------------
     *SHoundRedundantGroupKeyedList
     ----------------------*/
    pvnret = SHoundRedundantGroupKeyedList(pvngi);
    if(pvnret == NULL)
    {
      printf("SHoundRedundantGroupKeyedList failed.\n");
      testFlag = 0;
      logWrite(reportLog, "SHoundRedundantGroupKeyedList", "ValNodePtr", "NOTNULL", "Pointer", "NULL");
    }
    else
    {
      printf("SHoundRedundantGroupKeyedList OK.\n");
      testFlag = 1;
      logWrite(reportLog, "SHoundRedundantGroupKeyedList", "ValNodePtr", "NOTNULL", "Pointer", "NOTNULL");
    }
    Slri_FreeKeyedList(pvnret);
  
    /* Invalid param */
    pvnret = SHoundRedundantGroupKeyedList(nullValNodePtr);
    if(pvnret == NULL)
    {
      printf("SHoundRedundantGroupKeyedList failed.\n");
      logWrite(reportLog, "SHoundRedundantGroupKeyedList", "ValNodePtr", "NULL", "Pointer", "NULL");
    }
    else
    {
      printf("SHoundRedundantGroupKeyedList OK.\n");
      testFlag = 0;
      logWrite(reportLog, "SHoundRedundantGroupKeyedList", "ValNodePtr", "NULL", "Pointer", "NOTNULL");
    }
     if ( testFlag = 1)
     {
           logWrite(summaryLog, "SHoundRedundantGroupKeyedList", "PASS", "", "", "");

     }
     else
     {
           logWrite(summaryLog, "SHoundRedundantGroupKeyedList", "FAIL", "", "", "");

     }
    Slri_FreeKeyedList(pvnret);

    
    /*---------------
     *SHoundGetFastaFromRedundantGroupID
     ----------------*/
    if (pfa!=NULL) SLRIFastaFree(pfa);
    intValue = 11;
    pfa = SHoundGetFastaFromRedundantGroupID(intValue);
    sprintf(itos1, "%ld", intValue);
    if (pfa == NULL)
    {
      printf("SHoundGetFastaFromRedundantGroupID failed.\n");     
      testFlag = 0;
      logWrite(reportLog, "SHoundGetFastaFromRedundantGroupID",  "GROUPID", itos1, "Pointer", "NULL");
    }
    else
    {
      printf("SHoundGetFastaFromRedundantGroupID OK.\n"); 
      testFlag = 1;
      logWrite(reportLog, "SHoundGetFastaFromRedundantGroupID",  "GROUPID", itos1, "Pointer", "NOTNULL");
    }
    /* Groupid = MAXINT */    
    if (pfa != NULL) SLRIFastaFree(pfa);
    pfa = SHoundGetFastaFromRedundantGroupID(MAXINT);
    if (pfa == NULL)
    {
      printf("SHoundGetFastaFromRedundantGroupID gets the expeceted result.\n");
      logWrite(reportLog, "SHoundGetFastaFromRedundantGroupID",  "GROUPID", "MAXINT", "Pointer", "NULL");
    }
    else
    {
      printf("SHoundGetFastaFromRedundantGroupID fails to get expected result.\n");
      testFlag = 0;
      logWrite(reportLog, "SHoundGetFastaFromRedundantGroupID",  "GROUPID", "MAXINT", "Pointer", "NOTNULL");
    }
    /* Groupid = -1 */
    if (pfa != NULL) SLRIFastaFree(pfa);
    pfa = SHoundGetFastaFromRedundantGroupID(MINUSONE);
    sprintf(itos1, "%ld", MINUSONE);
    if (pfa == NULL)
    {
      printf("SHoundGetFastaFromRedundantGroupID gets the expected result.\n");
      logWrite(reportLog, "SHoundGetFastaFromRedundantGroupID",  "GROUPID", itos1, "Pointer", "NULL");
    }
    else
    {
      printf("SHoundGetFastaFromRedundantGroupID fails to get the expected result.\n");
      testFlag = 0;
      logWrite(reportLog, "SHoundGetFastaFromRedundantGroupID",  "GROUPID", itos1, "Pointer", "NOTNULL");
    }
     if ( testFlag = 1)
     {
           logWrite(summaryLog, "SHoundGetFastaFromRedundantGroupID", "PASS", "", "", "");

     }
     else
     {
           logWrite(summaryLog, "SHoundGetFastaFromRedundantGroupID", "FAIL", "", "", "");

     }
    SLRIFastaFree(pfa);
    
    
    /*----------------------------------------
     SHoundGetFastaFromRedundantGroupIDKeyedList
     -----------------------------------------*/
    pvnret = SHoundGetFastaFromRedundantGroupIDKeyedList(pvnredund);
    if (pvnret == NULL)
    {
      printf("SHoundGetFastaFromRedundantGroupIDKeyedList failed.\n");
      testFlag = 0;
      logWrite(reportLog, "SHoundGetFastaFromRedundantGroupIDKeyedList", "ValNodePtr", "NOTNULL", "Pointer","NULL");
    }
    else
    {
      printf("SHoundGetFastaFromRedundantGroupIDKeyedList OK.\n");
      testFlag = 1;
      logWrite(reportLog, "SHoundGetFastaFromRedundantGroupIDKeyedList", "ValNodePtr", "NOTNULL", "Pointer","NOTNULL");
    }
    Slri_FreeFastaKeyedList(pvnret);

    /* Invalid Param */
    pvnret= SHoundGetFastaFromRedundantGroupIDKeyedList(nullValNodePtr);
    if (pvnret == NULL)
    {
      printf("SHoundGetFastaKeyedList failed.\n");
      logWrite(reportLog, "SHoundGetFastaFromRedundantGroupIDKeyedList", "ValNodePtr", "NULL", "Pointer","NULL");
    }
    else
    {
      printf("SHoundGetFastaFromRedundantGroupIDKeyedList OK.\n");
      testFlag = 0;
      logWrite(reportLog, "SHoundGetFastaFromRedundantGroupIDKeyedList", "ValNodePtr", "NULL", "Pointer","NOTNULL");
    }
     if ( testFlag = 1)
     {
           logWrite(summaryLog, "SHoundGetFastaFromRedundantGroupIDKeyedList", "PASS", "", "", "");

     }
     else
     {
           logWrite(summaryLog, "SHoundGetFastaFromRedundantGroupIDKeyedList", "FAIL", "", "", "");

     }
    Slri_FreeFastaKeyedList(pvnret);
    
     
    
    CloseMMDBAPI();
    printf("MMDBAPI closed.\n");
    logWrite(summaryLog, "closeMMDBAPI", "OK", "", "", "");
    SHoundFini();
    printf("SHoundFini OK.\n");
    logWrite(summaryLog, "SHoundFini", "OK", "", "", "");
    fflush(reportLog);
    if (reportLog != NULL) fclose (reportLog);

    ValNodeFree(pvngi);
    ValNodeFree(pvngi3);
    ValNodeFree(pvnmmdbid);
    ValNodeFree(pvntaxid);
    ValNodeFree(pvnchromid);
    ValNodeFree(pvnredund);
    ValNodeFree(pvnInvalRedund);
    ValNodeFree(lllist);
    ValNodeFree(goList);
    ValNodeFree(omimList);
    ValNodeFree(giList);
    ValNodeFreeData(cddidlist);
/*ValNodeFree(valNodeRefList);*/

    tim2=GetSecs();
    printf("Total elapsed time: %ld sec. ",(tim2-tim1));
    sprintf(itos1, "%ld", (tim2-tim1));
    logWrite(summaryLog, "Total Elapsed Time",itos1 , "", "", "");
/* if (summaryLog != NULL) */
    fflush(summaryLog);
    fclose (summaryLog);
    
    printf("Done.\n");
    return 0;
}

void logWrite (FILE *fp, char *str1, char *str2, char *str3, char *str4, char *str5)
{
	fprintf(fp, "%-38s   %-20s   %-18s   %-18s   %-8s\n", str1, str2, str3,str4, str5);
      fflush(fp);
}
/*
 $Log: shunittest.c,v $
 Revision 1.35  2005/04/14 18:23:34  vgu
 Change flagging method in testing API calls

 Revision 1.34  2005/02/03 20:11:52  ryao
 change the value for precomTaxid

 Revision 1.33  2005/01/25 16:36:20  vgu
 Remove free file pointer calls

 Revision 1.32  2004/11/26 21:51:07  vgu
 Replace taxid 9606 for better performance

 Revision 1.31  2004/11/25 21:44:26  vgu
 Add parameter for API SHoundAccFromGi bug number 4005

 Revision 1.30  2004/10/18 16:20:10  ryao
 update some test cases

 Revision 1.29  2004/10/13 21:46:13  ryao
 added more test functions. see bug [1214]

 Revision 1.28  2004/10/01 15:52:20  ryao
 add test for SHoundGetNewTaxIDFromMergedTaxID. bug[3474]. Activate this test until seqrem updated on production server

 Revision 1.27  2004/09/03 19:50:27  zwang
 Change testing case to valid values.

 Revision 1.26  2004/08/12 18:02:31  zwang
 Test new API added to get title by gi from ACCDB

 Revision 1.25  2004/08/09 20:47:46  zwang
 Use a valid gi for SHoundLocusFromGi

 Revision 1.24  2004/07/24 02:06:26  vgu
 Update test parameters for odbc fcgi

 Revision 1.23  2004/07/08 20:05:28  ryao
 Free KeyedList functions use slri library calls.

 Revision 1.22  2004/06/24 15:16:29  vgu
 remove vars declared in C++ style

 Revision 1.21  2004/06/21 19:10:40  vgu
 Use different taxid for API 3dListFromTaxID

 Revision 1.20  2004/06/17 14:56:08  zwang
 Change the value of myargs parameters: TaxID and ChromID.

 Revision 1.19  2004/06/02 17:30:40  ryao
 added testing for new locus link functions.

 Revision 1.18  2004/04/23 16:31:55  vgu
 make comments comply to ANSI C coding style

 Revision 1.17  2004/04/15 20:15:56  ryao
 added test functions for SHoundGetFastaKeyedList SHoundRedundantGroupKeyedList SHoundGetFastaFromRedundantGroupID SHoundGetFastaFromRedundantGroupIDKeyedList

 Revision 1.16  2004/03/26 14:29:27  vgu
 Minor change: change taxid value for API SHoundProteinsfromTaxIDIII for faster response

 Revision 1.15  2004/03/24 14:59:09  vgu
 Add API SHoundProteinsFromTaxIDIII for access to precomputed data

 Revision 1.14  2004/03/05 17:06:10  vgu
 remove API SHoundNeighboursOfNeighboursFromTaxID

 Revision 1.13  2004/02/19 20:43:13  zwang
 Changed taxid for testing SHoundIsTaxDeleted and SHoundIsTaxMerged. Added test case to test if taxid is not deleted/merged. See bug #1663

 Revision 1.12  2004/02/19 18:45:56  mmatan
 Added tests for SHoundNumNeighboursInDB and SHoundNeighboursOfNeighboursList.  Changed GIs and Taxids used in SHoundGetBlastResult, SHoundGetBlastSeqAlign and SHoundNeighboursOfNeighboursFromTaxID calls to valid values.  See bugzilla report 1665 for details.

 Revision 1.11  2004/02/18 16:41:34  zwang
 Removed wrong stdout print

 Revision 1.10  2004/02/18 15:58:20  zwang
 Changed %%lds to %ld%s.

 Revision 1.9  2004/02/06 21:28:04  vgu
 fix report file buffer overrun error

 Revision 1.8  2004/02/06 17:21:14  vgu
 correct errors in sprintf calls

 Revision 1.7  2004/02/06 15:26:21  vgu
 make changes to enable compilation on Intel Linux

 Revision 1.6  2004/02/03 23:02:56  vgu
 uncomment code for API test SHoundFindName

 Revision 1.5  2004/02/03 22:58:21  vgu
 correct typo SHoundGetGetFastaList

 Revision 1.4  2004/01/06 16:49:50  vgu
 include testing code for APIs in bug 1155

 Revision 1.3  2003/12/05 20:40:21  vgu
 format report file

 Revision 1.2  2003/12/05 16:06:53  vgu
 Updated code to include the testing of another 48 APIs

 Revision 1.1  2003/12/01 19:42:34  vgu
 Added new file shunittest.c to expand on shtest.c

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




