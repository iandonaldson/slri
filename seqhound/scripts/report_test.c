/* $Id: report_test.c,v 1.5 2004/02/12 20:46:19 hlieu Exp $*/
/*******************************************************************************************
*  Copyright (C) 2000 Mount Sinai Hospital (MSH)
*
*  This program is free software; you can redistribute it and/or
*  modify it under the terms of the GNU General Public License as
*  published by the Free Software Foundation; either version 2 of
*  the License, or any later version.
*
*  This program is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*
*  See the GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with this program; if not, write to the
*         Free Software Foundation, Inc.,
*         59 Temple Place, Suite 330, Boston, MA
*         02111-1307  USA
*  or visit http://www.gnu.org/copyleft/gpl.html
*
*  SPECIAL EXCEPTIONS
*
*  As a special exception, Mount Sinai Hospital gives permission to
*  link this program with the following non-GPL programs or libraries,
*  and distribute the resulting executable, without including the source
*  code for these in the source distribution:
*
*  a) CodeBase 6.5 or greater from Sequiter Software Inc.
*
*  b) The C or C++ interface to Oracle 8.x or greater, from
*     Oracle Corporation or IBM DB2 UDB.
*
*
*  PROGRAM:
*
*  AUTHORS:
*          Hao Lieu (hlieu@blueprint.org)
*
*
*  REVISION/CONTRIBUTION NOTES:
*
*  June 1, 2000  Original MSH Revision.
*                Hogue Lab - University of Toronto Biochemistry
*                Department and the Samuel Lunenfeld Research
*                Institute, Mount Sinai Hospital
*                http://bioinfo.mshri.on.ca  hogue@mshri.on.ca
*                ftp://ftp.mshri.on.ca/
*
*
*  PUBLICATION TO CITE:
*
*
*  DESCRIPTION
*
******************************************************************************************/


#include <seqhound.h>
#include <objslristruc.h>
#include <go_db.h>


/* 
 * Application that tests the suite of ProteinReport functionality, 
 * including the helper functions to the public API.
 *
 * ProteinReport functionality are in intrez.c.
 *
 * This application is dependent on the input file described below.
 *
 * author: Hao Lieu
 */



/* 
 * The input file: format of the input file
 *
 *   n int1 ... intn 
 *    
 * where n is the size of the array, and int1 .. intn 
 * are the values to populate the array.  
 */
#define INPUTFILE "tests/inputs_protreport.txt"

/*
 * The log file, NCBI log functions will write to this the
 * status of the function testing
 */
#define LOGGING "logs/protreport.log"

/*
 * File where the ASN structures are written to, a record
 * of the tested functions.
 */
#define OUTPUTFILE "tests/outputs_protreport.txt"




/**********************************************************************
 * TestProteinReportSetDescription
 *
 * Description: retrieves the protein report set for a gi.
 *   -reads test input from 'data_in'
 *   -writes ASN proteinreport out to aip
 *  
 * returns the number of failures. 
 *
 **********************************************************************/
int TestProteinReportSetDescription(FILE* data_in, FILE* fp, AsnIoPtr aip)
{
   SHProteinReportSetDescriptionPtr pprsd = NULL;

   char test_value[10] = {0};    /* buffer for file inputs            */
   int gi       = 0,             /* */
       tests    = 0,             /* the size of the list in the array */
       failed   = 0,             /* count of the failures             */
       k        = 0;             /* */
   Uint2 flags  = 0;             /* */
   flags       |= REPORT_ALL;    /* REPORT_ALL defined in seqhound.h */

   /* read in the first value, which is the size of the list */
   if( !FileGets(test_value, 3, data_in) )
   {	
      fprintf(stderr, "[TestProteinReportSetDescription]: Illegal EOF encountered.\n");
      return 1;
   }
   tests = atoi(test_value);



   /* read in the list of values */
   for(k = 0; k < tests; k++)
   {

      pprsd = SHProteinReportSetDescriptionNew();
      if(!pprsd) 
         { fprintf(stderr, "[TestProteinReportSetDescription]: Null object.\n"); return 1; }



      if(!FileGets(test_value, 10, data_in) || (pprsd->query = atoi(test_value)) == 0)
      {
         fprintf(stderr, "[TestProteinReportSetDescription]: Invalid array input.\n");
	 return 1;
      }

      
      if(SHoundProteinReportSetDescription(pprsd, flags))
        { ErrPostEx(SEV_INFO, 0, 0, "[TestProteinReportSetDescription]: ProteinReportSetDescription successful"); }

      else    /*  failed a test  */
      {
         failed++;
         ErrPostEx(SEV_INFO, 0, 0, "[TestProteinReportSetDescription]: ProteinReportSetDescription failed"); 
	 continue;
      }
      


      /* write the result to a file */
      if(!SHProteinReportSetDescriptionAsnWrite(pprsd, aip, NULL))
         { ErrPostEx(SEV_INFO, 0, 0, "[TestProteinReportSetDescription]: AsnWrite failed."); }

      AsnIoFlush(aip);
      SHProteinReportSetDescriptionFree(pprsd);
   }
   fprintf(fp, "\n"); fflush(fp);
   return failed;
}




/**********************************************************************
 * TestProteinReportSetNeighboursReport
 *
 * Description: retrieves the protein report for the neighbours set.
 *   -reads test input from 'data_in'
 *   -writes ASN proteinreport out to aip
 *  
 * returns the number of failures. 
 *
 **********************************************************************/
int TestProteinReportSetNeighboursReport(FILE* data_in, FILE* fp, AsnIoPtr aip)
{
  SHProteinReportSetDescriptionPtr pprsd = NULL;

  char test_value[10] = {0};            /* buffer to read in file inputs */
  int gi              = 0,
      tests           = 0,              /* list size                     */
      k               = 0;

  int failed          = 0;              /* failed tests                  */
  Uint2 flags         = 0;
  flags |= REPORT_ALL;                  /* defined in seqhound.h         */


  if( !FileGets(test_value, 3, data_in) )
  {
      fprintf(stderr, "[TestProteinReportSetNeighboursReport]: Illegal EOF encountered.\n");
      return 1;
  }
  tests = atoi(test_value);



  /* read in the input values */
  for(k = 0; k < tests; k++)
  {

    pprsd = SHProteinReportSetDescriptionNew();
    if(!pprsd)
	 { fprintf(stderr, "[TestProteinReportSetNeighboursReport]: Null object.\n"); return 1; }


    if(!FileGets(test_value, 10, data_in) || (pprsd->query = atoi(test_value)) == 0)
    {
        fprintf(stderr, "[TestProteinReportSetNeighboursReport]: Invalid array input.\n");
        return 1;	
    }
    pprsd->cutoff = 1;

    if(ProteinReportSetNeighboursReport(pprsd, flags))
      { ErrPostEx(SEV_INFO, 0, 0, "[TestProteinReportSetNeighboursReport]: ProteinReportSetNeighbours successful"); }
    else 
    {                   /* failed a test */
       failed++;
       ErrPostEx(SEV_INFO, 0, 0, "[TestProteinReportSetNeighboursReport]: ProteinReportSetNeighbours failed");
       continue;
    }


    /* write to file */
    if(!SHProteinReportSetDescriptionAsnWrite(pprsd, aip, NULL))
       { ErrPostEx(SEV_INFO, 0, 0, "[TestProteinReportSetNeighbhoursReport]: AsnWrite failed"); }

    AsnIoFlush(aip);
    SHProteinReportSetDescriptionFree(pprsd);
  }
  fprintf(fp, "\n"); fflush(fp);
  return failed;
}




/*******************************************************************
 * TestProteinReportSetRedundantGroup
 *
 * Description: tests ProteinReportSetRedundantGroup
 *   -reads test inputs from 'data_in'
 *   -writes the ASN proteinreportsetdescription into 'aip'
 *       and flushes outputs with a '\n'
 *
 * 
 * returns the number of failures
 *******************************************************************/
int TestProteinReportSetRedundantGroup(FILE* data_in, FILE* fp, AsnIoPtr aip)
{
  SHProteinReportSetDescriptionPtr pprsd = NULL;

  char test_value[10] = {0};     /* file input buffer */
  int gi              = 0,
      tests           = 0,       /* the size of the array */
      k               = 0;

   int failed = 0;
   Uint2 flags = 0;
   flags |= REPORT_ALL;          /* REPORT_ALL defined in seqhound.h */

   if( !FileGets(test_value, 3, data_in) )
   {
      fprintf(stderr, "[TestProteinReportSetRedundantGroup]: Illegal EOF encountered.\n");
      return 1;      
   }
   tests = atoi(test_value);


   /* read in the inputs */
   for(k = 0; k < tests; k++)
   {
      pprsd = SHProteinReportSetDescriptionNew();
      if(!pprsd)
         { fprintf(stderr, "[TestProteinReportSetDescription]: Null object.\n"); return 1; } 
     


      if(!FileGets(test_value, 10, data_in) || (pprsd->query = atoi(test_value)) == 0)
      {
         fprintf(stderr, "[TestProteinReportSetDescription]: Invalid array input.\n");
	 return 1;
      }


      if(ProteinReportSetRedundantGroup(pprsd, flags))
         { ErrPostEx(SEV_INFO, 0, 0, "[TestProteinReportSetRedundantGroup]: ProteinReportSetRedundantGroup successful"); }

      else 
      {                 /* failed a test */ 
	 failed++;
	 ErrPostEx(SEV_INFO, 0, 0, "[TestProteinReportSetRedundantGroup]: ProteinReportSetRedundantGroup failed"); 
	 continue;
      }

      /* write the results to a file */
      if(!SHProteinReportSetDescriptionAsnWrite(pprsd, aip, NULL))
         { ErrPostEx(SEV_INFO, 0, 0, "[TestProteinReportSetRedundantGroup]: AsnWrite failed");}


      AsnIoFlush(aip);
      SHProteinReportSetDescriptionFree(pprsd);
   }
   fprintf(fp, "\n"); fflush(fp);
   return failed; 
}




int TestGetProteinReport(FILE* data_in, FILE* fp, AsnIoPtr aip)
{
   SHProteinReportPtr ppr    = NULL;
   
   char test_value[10]       = {0};     /* buffer to read in file */
   int gi                    = 0,
       tests                 = 0,       /* size of array */
       k                     = 0;
   int failed                = 0;       /* number of tests failed */

   Uint2 flags               = 0;

   /* the REPORT_XX flags are defined in seqhound.h */
   flags |= REPORT_LL | REPORT_OMIM | REPORT_GO | REPORT_TAX | REPORT_MMDB | REPORT_DNA | REPORT_MUID | REPORT_DEF | REPORT_ACC;

   if( !FileGets(test_value, 3, data_in) )
   {
      fprintf(stderr, "[TestGetProteinReport]: Illegal EOF encounterd.\n");
      return 1;      
   }
   tests = atoi(test_value);

   for(k = 0; k < tests; k++)
   {
      ppr = SHProteinReportNew();
      if(!ppr) 
         { fprintf(stderr, "[TestGetProteinReport]: Null object.\n"); return 1; }

      if(!FileGets(test_value, 10, data_in) || (ppr->gi = atoi(test_value)) == 0)
      {
         fprintf(stderr, "[TestProteinReportSetDescription]: Invalid array input.\n");
         return 1;	 
      }


      if(SHoundGetProteinReport(ppr, flags))
	{ ErrPostEx(SEV_INFO, 0, 0, "[TestGetProteinReport]: SHoundGetProteinReport"); }
      
      else 
      {               /* failed a test */
        failed++;
	ErrPostEx(SEV_INFO, 0, 0, "[TestGetProteinReport]: SHoundGetProteinReport");
	continue;
      }

      if(!SHProteinReportAsnWrite(ppr, aip, NULL))
	 { ErrPostEx(SEV_INFO, 0, 0, "[TestGetProteinReport]: SHoundGetProteinReport"); }


      AsnIoFlush(aip);
      SHProteinReportFree(ppr);
   }
   fprintf(fp, "\n"); fflush(fp);
   return failed;
}




/*****************************************************************
 * TestGetGOReport
 *
 * Description: tests SHoundGetGoReport
 *    -reads test inputs from 'data_in'
 *    -writes the ASN GoReport to the output stream aip
 *       and flushes outputs with a '\n'
 *
 * 
 * return the number of failures
 *****************************************************************/
int TestGetGOReport(FILE* data_in, FILE* fp, AsnIoPtr aip)
{
   int failed = 0;                 /* the number of failed tests */
   char test_value[50] = {0};     /* buffer to read from file */
   int gi = 0,
       k = 0,                      /* counter                 */
       tests = 0;
   SHGoReportPtr pgr = NULL;
   
   if ( !FileGets(test_value, 3, data_in) )
   {
      fprintf(stderr, "[TestGetGOReport]: Illegal EOF encountered.\n");
      return 1;
   }
   tests = atoi(test_value);
   
   for(; k < tests; k++)
   {
     pgr = SHGoReportNew();
     if(!pgr)
        { fprintf(stderr, "[TestGetGOReport]: Null object.\n");  return 1; }
    


     if( !FileGets(test_value, 10, data_in) || (gi = atoi(test_value)) == 0 )
     { 
	fprintf(stderr, "[TestGetGOReport]: Invalid array input.\n");
	return 1;
     }


     if(SHoundGetGOReport(pgr, gi))
        { ErrPostEx(SEV_INFO, 0, 0, "[TestGetGOProduct]: SHoundGetGOReport successful"); }

     else                /* failed a test */
     { 
        failed++;
	ErrPostEx(SEV_INFO, 0, 0, "[TestGetGOProduct]: SHoundGetGOReport failed"); 
        continue;
     }

     if(!SHGoReportAsnWrite(pgr, aip, NULL))
        { ErrPostEx(SEV_INFO, 0, 0, "[TestGetGOProduct]: SHGoReportAsnWrite failed"); }

     AsnIoFlush(aip);
     SHGoReportFree(pgr);
   }
   fprintf(fp, "\n"); fflush(fp);
   return failed;
}




/*******************************************************************
 * TestLocusLinkReport
 *
 * Description: test SHoundGetLocusLinkReport
 *  -reads test input from 'data_in'
 *  -writes the ASN structure out into output stream 'aip',
 *      and flush with a '\n' into fp
 *
 *  returns the number of errors
 ********************************************************************/
int TestLocusLinkReport(FILE* data_in, FILE* fp, AsnIoPtr aip)
{
   SHLocusLinkReportPtr pllr = NULL;         /* ptr to ASN object */
   char test_value[50]       = {0};          /* buffer to hold file inputs */
   int gi                    = 0,            /* objects gi, read in from file */
       j                     = 0,            /* an iterator */
       tests                 = 0;            /* the number of tests, read in from file */
   int failed                = 0;            /* number of failures */ 



   /* read input from input file and convert to correct type */
   if ( !FileGets(test_value, 3, data_in) )
   {
      fprintf(stderr, "[TestLocusLinkReport]: Illegal EOF encountered.\n");
      return 1;      
   }
   
   /* number of tests to perform */
   tests = atoi(test_value);
  
   /* iterate for each test */ 
   for(j = 0; j < tests; j++)
   {
      pllr = SHLocusLinkReportNew();
      if(!pllr)
	  { fprintf(stderr, "[TestLocusLinkReport]: Null object.\n"); return 1; }

      /* read testing gi from input file */
      if( !FileGets(test_value, 10, data_in) || (gi = atoi(test_value)) == 0 )
      {
	 fprintf(stderr, "[TestLocusLinkReport]: Invalid array input.\n");
	 return 1; 
      }

      /* try to retrieve the asn object from SeqHound */
      if(SHoundGetLocusLinkReport(pllr, gi))
	 { ErrPostEx(SEV_INFO, 0, 0, "[TestLocusLinkReport]: SHoundGetLocusLinkReport successful"); }

      else 
      {  
	 /* failed a test */
         failed++;
	 ErrPostEx(SEV_INFO, 0, 0, "[TestLocusLinkReport]: SHoundGetLocusLinkReport failed"); 
	 continue;
      }
     
      /* try to write to the output stream 'aip' */ 
      if(!SHLocusLinkReportAsnWrite(pllr, aip, NULL))
         { ErrPostEx(SEV_INFO, 0, 0, "[TestLocusLinkReport]: SHoundLocusLinkReportAsnWrite"); } 


      AsnIoFlush(aip);
      SHLocusLinkReportFree(pllr);
   }
   fprintf(fp, "\n"); fflush(fp);
   return 0;
}




/*******************************************************************
 *  
 *  TestLocusProduct:
 * 
 * Description: test SHoundGetLocusProduct. 
 *   -reads test inputs from 'data_in'
 *   -writes the ASN locusproduct to the output stream 'aip',
 *       and flush outputs with a '\n' 
 *     
 * 
 * return the number of failures  
 ********************************************************************/
int TestLocusProduct(FILE* data_in, FILE* fp, AsnIoPtr aip)
{
   SHLocusProductPtr plp   = NULL;             /* the asn object */
   char test_value[10]     = {0};              /* buffer to hold read-in values */
   int llid                = 0,                /* key of object, read in from 'data_in' */
       tests               = 0,                /* the number of tests to perform */ 
       k                   = 0;                /* iterator count */
   int failed              = 0;                /* count of failures */

   /* the first value is the number of tests in the file
    * and convert to correct format  */
   if( !FileGets(test_value, 3, data_in) )
   {
      fprintf(stderr, "[TestLocusProduct]: Illegal EOF encountered.\n");
      return 1;      
   } 
   tests = atoi(test_value);

   /* iterate for each test */
   for(k = 0; k < tests; k++)
   {
      plp = SHLocusProductNew();
      if(!plp)
         { fprintf(stderr, "[TestLocusProduct]: Null object.\n"); return 1; }

      /* read in the object keys from the data file */
      if(!FileGets(test_value, 3, data_in) || (llid = atoi(test_value)) == 0)
      {
         fprintf(stderr, "[TestLocusProduct]: Invalid array input.\n");
	 return 1;
      }
      
      /* test our API function call */
      if(SHoundGetLocusProduct(plp, llid))
         { ErrPostEx(SEV_INFO, 0, 0, "[TestLocusProduct]: SHoundGetLocusProduct successful"); }
      else 
      {  
	failed++;
	ErrPostEx(SEV_INFO, 0, 0, "[TestLocusProduct]: SHoundGetLocusProduct failed");
	continue;
      }

      /* write the asn out to file */
      if(!SHLocusProductAsnWrite(plp, aip, NULL))
         { ErrPostEx(SEV_INFO, 0, 0, "[TestLocusProduct]: SHoundLocusProductAsnWrite failed"); }

      AsnIoFlush(aip);
      SHLocusProductFree(plp);
   }
   /* flush the file for asthetics */
   fprintf(fp, "\n"); fflush(fp);
   return failed;
}





int main(int argc, char* argv[])
{
   int failed    = 0;                        /* a running count of the failed tests */

   /* to change the input file and output file change the macros at the top of page */
   FILE *data_in = FileOpen(INPUTFILE, "r");
   FILE *fp      = FileOpen(OUTPUTFILE, "a");
   AsnIoPtr aip  =  AsnIoNew(ASNIO_TEXT_OUT, fp, NULL, NULL, NULL);

   /* 
    * use the NCBI C toolkits logging capabilities because fwrite
    * is not very elegant and has buffering issues. At lease with the
    * NCBI C toolkit, it takes care of it for me.
    * 
    */

   /* set some flags for the logging:
    *  
    * tells log functions to log: the severity level, 
    *      the line of error and the calling file 
    *
    */
   ErrSetOptFlags(EO_LOG_SEVERITY | EO_LOG_FILELINE | EO_LOG_USRFILE);
   
   /* LOGGING is a macro defining the log file, see top of file */
   ErrSetLogfile(LOGGING, ELOG_BANNER);
   ErrSetLogLevel(SEV_INFO);
   ErrSetMessageLevel(SEV_MAX);

   /* seqhound testing */
   SHoundInit(TRUE, "report");

   /* In the data_in, there must be a line for each  
    * TestFunction called here */
   failed += TestLocusLinkReport(data_in, fp, aip);
   failed += TestLocusProduct(data_in, fp, aip);
   failed += TestGetGOReport(data_in, fp, aip);
   failed += TestGetProteinReport(data_in, fp, aip);
   failed += TestProteinReportSetRedundantGroup(data_in, fp, aip);
   failed += TestProteinReportSetNeighboursReport(data_in, fp, aip);
   failed += TestProteinReportSetDescription(data_in, fp, aip);

   /* file clean ups */
   SHoundFini();
   AsnIoFlush(aip); AsnIoClose(aip);
   AsnIoFree(aip, TRUE);
   FileClose(data_in);
   FileClose(fp);

   /* print out to screen the # of failed tests */
   fprintf(stderr, "There were %d errors.\n", failed);
   fprintf(stderr, "Check %s for logged reports.\n", LOGGING);

   return 0;
}


/* 
 * $Log : $
 */
