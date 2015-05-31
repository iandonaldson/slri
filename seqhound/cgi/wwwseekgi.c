/* $Id: wwwseekgi.c,v 1.36 2005/04/27 16:01:36 vgu Exp $
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
     Oracle Corporation.


  PROGRAM:


  AUTHORS:
           Katerina Michalickova, Ruth Isserlin,  and Christopher
W.V. Hogue hogue@mshri.on.ca


  REVISION/CONTRIBUTION NOTES:

  June 1, 2000  Original MSH Revision.
                Hogue Lab - University of Toronto Biochemistry
                Department and the Samuel Lunenfeld Research
                Institute, Mount Sinai Hospital
                http://bioinfo.mshri.on.ca  hogue@mshri.on.ca
                ftp://ftp.mshri.on.ca/


  PUBLICATION TO CITE:


  DESCRIPTION
  July 1998
  web server for SeqHound system

******************************************************************************************/
#ifdef SHFCGI
#include <fcgi_stdio.h>
#include <fcgi_config.h>
#endif

#ifdef ODBC
#include <intrez_odbc.h>
#else
#include <intrez_db.h>
#endif

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <nbr_db.h>
#include <cdentrez.h>
#include <ncbibs.h>
#include <slri_post.h>
#include <intrez_misc.h>
#include <intrez_cfg.h>
#include <intrez.h>
#include <slri.h>
#include <seqhound.h>
/* include <asn2ff.h> */

#define HEADFILE "seekhead.txt"
#define TAILFILE "seektail.txt"


/* functions */
static void WWWPrintFileData(CharPtr,  FILE*);
Boolean SeekGI(Int4, CharPtr, Int4);
Boolean PrintRec(CharPtr, StAsndbPtr);
Int2 CreateDefline(Int4);
Boolean WriteffII(Int4, CharPtr);
Int2 GetTaxonomy(Int4);
Int2 GetMedline(Int4);
CharPtr TrimSpaceII(CharPtr);
CharPtr Pad(CharPtr);
Int4 Error_msg_return(CharPtr error, Int4 default_return);
Int4 Error_msg_return_soft(CharPtr error, Int4 default_return);
Int4 Error_msg_free_return_soft(CharPtr error, Int4 default_return);
Int4 Error_msg_free_return(CharPtr error, Int4 default_return);
Int4 GetIntegerValue(WWWInfoPtr, CharPtr, Int4);
Int4 num_pages(WWWInfoPtr info, Int4 count);
void print_Top(WWWInfoPtr info, Int4 count);
Int4 open_databases();
void close_databases();
Boolean CheckMedline(Int4 gival);

Boolean PrintFasta ( Int4 gival, CharPtr typestr );


static void WWWPrintFileData(CharPtr FName,  FILE *pFile)
{

   FILE *f = NULL;
   Char fullpath [PATH_MAX];
   CharPtr ptr;
   Char pcBuf[1024];

   f = FileOpen (FName, "r");
   if (f == NULL)  {
    ProgramPath (fullpath, sizeof (fullpath) - 1);
    ptr = StringRChr (fullpath, DIRDELIMCHR);
    if (ptr != NULL) {
          *ptr = '\0';
    }
    FileBuildPath (fullpath, "data", FName);
    f = FileOpen (fullpath, "r");
    if (f == NULL) return;
   }

   do {
     pcBuf[0] = '\0';
     ptr = fgets(pcBuf, (size_t)1024, f);
     if (ptr) fprintf(pFile, ptr);
   } while (ptr);

   FileClose(f);
   return;
}


/* Given : a string which represents the error and value to be returned due to this error
 * Purpose : outputs error message to the screen and to the error file.
 * Returns : the return value of the functions called from.
 */
Int4 Error_msg_return(CharPtr error, Int4 default_return)
 {
    /*WWWPrintFileData(HEADFILE, stdout);*/
    printf("<h2>Error</h2>\n <b>Your request failed.</b><br>  Please mail the description of the problem to: <a href=\"mailto:seqhound@blueprint.org\">SeqHound administrator.</a> Thank you.<p>");
    printf("<b>Error message:</b>\n %s<p>\n", IsNullStr(error));
    ErrPostEx(SEV_ERROR,0,0, error);
    return default_return;
 }

Int4 Error_msg_free_return(CharPtr error, Int4 default_return)
 {
    /*WWWPrintFileData(HEADFILE, stdout);*/
    printf("<h2>Error</h2>\n <b>Your request failed.</b><br>  Please mail the description of the problem to: <a href=\"mailto:seqhound@blueprint.org\">SeqHound administrator.</a> Thank you.<p>");
    printf("<b>Error message:</b>\n %s<p>\n", IsNullStr(error));
    ErrPostEx(SEV_ERROR,0,0, error);
    MemFree(error);
    return default_return;
 }

Int4 Error_msg_return_soft(CharPtr error, Int4 default_return)
 {
    WWWPrintFileData(HEADFILE, stdout);
    printf("<b>%s</b>\n", IsNullStr(error));
    printf("<i>Please make sure that you selected the right search field.</i><p>");
    ErrPostEx(SEV_ERROR,0,0, error);
    return default_return;
 }

Int4 Error_msg_free_return_soft(CharPtr error, Int4 default_return)
 {
    WWWPrintFileData(HEADFILE, stdout);
    printf("<b>%s</b>\n", IsNullStr(error));
    printf("<i>Please make sure that you selected the right search field.</i><p>");
    ErrPostEx(SEV_ERROR,0,0, error);
    MemFree(error);
    return default_return;
 }


/*
Gets rid of white space at the end of a string
*/
CharPtr TrimSpaceII(CharPtr string)
{
   int i;

   if (string == NULL)
     return NULL;

   if (strstr(string, "n/a") != NULL)
    {
      string[0] = ' ';
      string[1] = '\0';
      return string;
    }
   for (i = 0; i <= (int)strlen(string); i++)
    {
      if ((string[i] == ' ') && (string[i+1] == ' '))
       {
          string[i] = '\0';
          return string;
       }
    }
   return string;
}


/*
Adds a space at the end of a string; allocates memory!!!!
*/
CharPtr Pad(CharPtr string)
{
   CharPtr newstr = NULL;

   /* SK: NULL ptr check. strlen will die on NULL string */
   if ( string == NULL )
      return NULL;

   newstr = MemNew(strlen(string) + 2);
   if (newstr == NULL){
     Error_msg_return( "Pad: Failed to allocate memory.", FALSE);
     return NULL;
   }
   StringCpy(newstr, string);
   newstr[strlen(string)] = ' ';
   newstr[strlen(string) + 1] = '\0';
   return newstr;
}




 /* Given : The total number of records that we have retrieved.
 * Purpose : prints out the top of the page
 * Returns : nothing
 */
void print_Top(WWWInfoPtr info, Int4 count)
{
    CharPtr fieldptr = NULL;
    CharPtr formptr = NULL;
    CharPtr valptr = NULL;
    CharPtr npp = "20";
    Int4 total_pages = 1, page = 1, start = 0;
    Int4 i = 0, num_per_page = 20;
    /* SK: */
    CharPtr strptr= NULL;

    /*find out which field is selected at this point*/
    fieldptr = WWWGetValueByName(info, "field");
    formptr = WWWGetValueByName(info, "format");
    valptr = WWWGetValueByName(info, "GI");
    npp = WWWGetValueByName(info, "npp");
    if(npp == NULL)
       npp = "20";

    /* SK: */
    strptr = WWWGetAddress(info);
    ErrPostEx(SEV_INFO,0,0, "Address %s\n Search for key %s| field %s| format %s|", IsNullStr(strptr), IsNullStr(valptr), IsNullStr(fieldptr), IsNullStr(formptr));
    /* print the top search form from a file */
    WWWPrintFileData(HEADFILE, stdout);
    /* display the search results */
    num_per_page = GetIntegerValue(info, "npp", 20);
    page = GetIntegerValue(info,"page",1);
    start = (page-1) * num_per_page;

    if(count > num_per_page)
     {
       char* cgiPathPtr = Cfg_GetCgiPath();

       printf("<table width=\"100%%\" cellpadding=\"0\"> <tr>\n");
       printf("<td align=center bgcolor = \"f7f7f7\">\n");
       printf("<center><b>Seqhound results %ld - %ld of %ld ", (long) start+1, (long) ((start+num_per_page)>count ? count : start+num_per_page), (long) count);
       printf("for %s : %s</b></center>",IsNullStr(fieldptr), IsNullStr(valptr));
       /* VG Make changes for null pointer exception handling */

       printf("</td>\n");
       printf("<FORM ACTION= \"%s\" METHOD=\"GET\">", IsNullStr(cgiPathPtr));
       printf("<td bgcolor = \"f7f7f7\" align=center><b>Go to page:\n");

       printf("<INPUT TYPE=\"hidden\"  VALUE=\"%s\" NAME=\"field\">\n", IsNullStr(fieldptr));
       printf("<INPUT TYPE=\"hidden\"  VALUE=\"%s\" NAME=\"format\">\n", IsNullStr(formptr));
       printf("<INPUT TYPE=\"hidden\"  NAME=\"GI\" VALUE=\"%s\">\n",IsNullStr(valptr));
       printf("<INPUT TYPE=\"hidden\"  NAME=\"npp\" VALUE=\"%s\">\n",IsNullStr(npp));

       printf("<SELECT NAME=\"page\">\n");
       total_pages = num_pages(info, count);

       if(total_pages == 0)
         total_pages = 1;
       if(page > total_pages)
          page = 1;
       for(i=1;i<=total_pages;i++)
       {
         if(i == page)
           printf("<OPTION SELECTED>%ld\n",(long) i);
         else
           printf("<OPTION>%ld\n",(long) i);
       }
       printf("</SELECT><INPUT TYPE=\"submit\" VALUE=\"Go\">\n");
       printf("</td></tr></b></form></table><p> \n");
     }
    else
     {
       printf("<table width=\"100%%\" cellpadding=\"0\"> <tr>\n");
       printf("<td align=center bgcolor = \"f7f7f7\">\n");
       printf("<center><b>Seqhound results %ld - %ld of %ld ",(long) start+1, (long)((start+num_per_page)>count ? count : start+num_per_page), (long) count);
       printf("for GI identifier: %s</b></center>", IsNullStr(valptr));
       printf("</td></tr></b></form></table><p>\n");
     }
}




/* Given : The info buffer and the number of record retrieved with
 * this search.
 * Purpose : calculate the total number of pages to be displayed.
 * Returns : the total number of pages.
 */
Int4 num_pages(WWWInfoPtr info, Int4 count){
  Int4 num_per_page = 20;
  Int4 total_pages = 1;

  /*get the number of items per page*/
  num_per_page = GetIntegerValue(info, "npp", 20);

  /*figure out the total number of pages */
  if(count % num_per_page == 0)
     total_pages = count / num_per_page;
  else
     total_pages = count / num_per_page + 1;
  return total_pages;
}



/*  Given : info (text from the cgi arguments), name of field, default value
 *  Purpose: goes through info and finds field with specified name.
 *  Returns : value of specified name and default otherwise.  Must be an integer.
 */
Int4 GetIntegerValue(WWWInfoPtr info, CharPtr name, Int4 default_value){

  Int4 value = 0;
  CharPtr valueptr = NULL;
  CharPtr errormsg = NULL;

  valueptr =  WWWGetValueByName(info, name);

  /*if there is no start then start at 0.*/
  if(valueptr == NULL)
    value = default_value;
  else
    if(!StrToInt4(valueptr, &value))
    {
       errormsg = MemNew((size_t)(strlen(name)+100)*sizeof(Char));
       /* SK: NULL ptr check */
       if ( errormsg == NULL )
          return default_value;

       sprintf(errormsg, "Invalid value for %s input", IsNullStr(name));
       return Error_msg_free_return(errormsg, default_value);
    }
  return value;
}


/*Given: nothing
*  This method intializes codebase and opens all the databases
*  Returns: TRUE on success and FALSE on failure.
*/
Int4 open_databases(){
	if (!InitCodeBaseReadOnly())
          return Error_msg_return("Database initialization failed.", FALSE);
	if (!OpenPARTI("PARTI", FALSE))
          return Error_msg_return("Cannot open database PARTI.", FALSE);

	if (!OpenNUCPROT("NUCPROT", FALSE))
         return Error_msg_return("Cannot open database NUCPROT.", FALSE);

	if (!OpenACCDB("ACCDB", FALSE))
          return Error_msg_return("Cannot open database ACCDB.", FALSE);

	if (!OpenPUBSEQ("PUBSEQ", FALSE))
          return Error_msg_return("Cannot open database PUBSEQ.", FALSE);

	if (!OpenTAXGI("TAXGI", FALSE))
          return Error_msg_return("Cannot open database TAXGI.", FALSE);

	if (!OpenREDUND("REDUND", FALSE))
          return Error_msg_return("Cannot open database REDUND.", FALSE);

	if(!OpenMMGI("MMGI", FALSE))
          return Error_msg_return("Cannot open database MMGI.", FALSE);
/*
	if(!OpenMMDB("MMDB", FALSE))
          return Error_msg_return("Cannot open MMDB.", FALSE);
*/
	if(!OpenAllTaxDBs(FALSE))
	  return Error_msg_return("OpenAllTaxDBs failed.", FALSE);
	if(!OpenAllNbrDBs(FALSE))
	  return Error_msg_return("OpenAllNbrsDBs failed.",FALSE);

	return TRUE;
}


/*closes all of the databases*/
void close_databases(){

	ClosePARTI();
	CloseNUCPROT();
	CloseACCDB();
	ClosePUBSEQ();
	CloseTAXGI();
	CloseREDUND();
 CloseAllTaxDBs();
 CloseAllNbrDBs(FALSE);
	CloseCodeBase();
}

/*
Writes out definition line from ACCDB
*/
Int2 CreateDefline(Int4 gival)
{
   StAccdbPtr pac;
   CharPtr    returnPtrTrimPacDb;
   CharPtr    returnPtrTrimPacAccess;
   CharPtr    returnPtrTrimPacName;
   CharPtr    returnPtrTrimPacChain;

   pac = StAccdbNew();
 
   /* SK: NULL ptr check */
   if ( pac == NULL )
      return -1;

   ResetStACCDB(&pac);

   pac->gi = gival;

   if (gival == 0)
      return 0;

   if(SearchACCDB(&pac) != 1)
   {
     StAccdbFree(pac);
     return Error_msg_return("createdefline:GI %ld not found in Accdb.",FALSE);
   }
   else
   {
     /* VG: Make changes for hadling of null pointer exceptions */
     returnPtrTrimPacDb = TrimSpaceII(pac->db);
     returnPtrTrimPacAccess = TrimSpaceII(pac->access);
     returnPtrTrimPacName = TrimSpaceII(pac->name);
     returnPtrTrimPacChain = TrimSpaceII(pac->chain);
 
     if (strstr(pac->access, " ") != NULL)
       /* VG: printf("%ld|%s|%s|%s|%s", (long) pac->gi, TrimSpaceII(pac->db), TrimSpaceII(pac->access),TrimSpaceII(pac->name), TrimSpaceII(pac->chain));*/
       printf("%ld|%s|%s|%s|%s", (long) pac->gi, IsNullStr(returnPtrTrimPacDb), IsNullStr(returnPtrTrimPacAccess), IsNullStr(returnPtrTrimPacName), IsNullStr(returnPtrTrimPacChain));
     else
       /* VG: printf("%ld|%s|%s.%ld|%s|%s", (long) pac->gi, TrimSpaceII(pac->db), TrimSpaceII(pac->access), (long) pac->version, TrimSpaceII(pac->name), TrimSpaceII(pac->chain)); */
       printf("%ld|%s|%s.%ld|%s|%s", (long) pac->gi, IsNullStr(returnPtrTrimPacDb), IsNullStr(returnPtrTrimPacAccess), (long) pac->version, IsNullStr(returnPtrTrimPacName), IsNullStr(returnPtrTrimPacChain)); 

     if ( pac->title == NULL || strcmp(pac->title, "n/a") != 0)
        printf("%s<br>", IsNullStr(pac->title));
     else
        printf("<br>");

     StAccdbFree(pac);
   }
   return 1;
}


/**
 * HCL: Prints out an HTML Fasta object.
 * Used for broken ASN.1/GenBank objects in SeqHound
 * as a temporary fix.
 */
Boolean PrintFasta ( Int4 gival, CharPtr typestr )
{
   Boolean protein      = TRUE;  /* is gival a protein */
   SLRIFastaPtr pfa     = NULL;  /* the fasta object */

   /* determine the type of molecule */
   if ( NULL != strstr ( typestr, "protein" ) )
      { protein = FALSE; }
   else if ( NULL != strstr ( typestr, "NA" ) )
      { protein = TRUE; }

   fprintf ( stdout, "<br />" ); 
   fprintf ( stdout, "<b>The ASN.1/GenBank for %d is currently not available.</b><p/>\n", 
             gival );
   fprintf ( stdout, "Check with the NCBI's Entrez system " );
   fprintf ( stdout, "<a href=\"http://www.ncbi.nlm.nih.gov/entrez/query.fcgi?cmd=Retrieve&db=%s&list_uids=%ld&dopt=Citation&tool=SeqHound\" target=\"_blank\">"
             "here</a>", protein == TRUE ? "nucleotide" : "protein", (long) gival );
   fprintf ( stdout, "<p/>" );

   if ( NULL == (pfa = SHoundGetFasta ( gival )) )
      { return FALSE; }
  
   fprintf ( stdout, "<pre>" ); 
   if ( !PrintSLRIFasta ( pfa, 80, stdout ) )
      { return FALSE; } 
  
   fprintf ( stdout, "<p/></pre><p/>" );

   return TRUE;
}

/*
 * Displays GenBank flat file
 */
Boolean WriteffII(Int4 gival, CharPtr typestr)
{
 /* 
    HL: June 18 2004 
    if 0 out the old version. Incorporating fix 
    implemented by Renan.
  */

#if 0
   FILE *fp = NULL;
   SeqEntryPtr pse = NULL;
   SeqEntryPtr pse_single = NULL;
   SeqIdPtr sip = NULL;
   AsnIoBSPtr aibp=NULL;
   AsnIoBSPtr aibp2=NULL;
   ByteStorePtr bsp = NULL;
#endif 
    
   if (!OpenSENGI("SENGI", FALSE)) 
    {
       return Error_msg_return("Writeff: Cannot open SENGI.", FALSE);
    }

#if 0
    if( NULL == (pse = SHoundGetSeqEntry(gival) ) )
    {
      CloseSENGI();
      return PrintFasta ( gival, typestr ); 
      /* HL: for broken gi's 
      return Error_msg_return("Writeff: SHoundGetSeqEntry returned NULL.", FALSE);*/
    }
 
    fp = FileOpen("stdout", "w");
    if (fp == NULL)
    {
       SeqEntryFree(pse);
       CloseSENGI();
       return Error_msg_return("Writeff: NULL file pointer.", FALSE);
    }

   printf("<pre>");
   if (strstr(typestr, "NA") != NULL)
   {
        ObjMgrRegister(OBJ_SEQENTRY, pse);
        if (! SeqEntryToFlatEx(pse, fp, GENBANK_FMT, RELEASE_MODE, NULL, FF_REGULAR))
        {
           SeqEntryFree(pse);
           CloseSENGI();
           FileClose(fp);
           return Error_msg_return("Writeff: SeqEntryToFlat failed.", FALSE);
        }
   }
   else if (strstr(typestr, "protein") != NULL)
   {

         /* process the seqentry to contain only one bioseq with SeqEntryAsnGet */
        if (!SeqEntryLoad())
	{
            SeqEntryFree(pse);
            CloseSENGI();
            FileClose(fp);
            return Error_msg_return("Writeff: SeqEntryLoad failed.", FALSE);
	} 
        sip = ValNodeNew(NULL);
	/* SK: NULL ptr check */
	if ( sip == NULL )
	{
           SeqEntryFree(pse);
           CloseSENGI();
           FileClose(fp);
	   return Error_msg_return("Writeff: ValNodeNew failed.", FALSE);
	}
        sip->choice = SEQID_GI;
        sip->data.intvalue = gival;	
        
        bsp = BSNew(5000);
	/* SK: NULL ptr check */
	if ( bsp == NULL )
	{
           SeqEntryFree(pse);
           CloseSENGI();
           FileClose(fp);
	   return Error_msg_return("Writeff: BSNew failed.", FALSE);
	}

        aibp = AsnIoBSOpen("wb", bsp);
        if (aibp == NULL)
        {
           SeqEntryFree(pse);
           CloseSENGI();
           FileClose(fp);
           return Error_msg_return("Writeff: AsnIoBSOpen failed.", FALSE);
	}
        SeqEntryAsnWrite(pse, aibp->aip, NULL);
        AsnIoBSClose(aibp);

        aibp2 = AsnIoBSOpen("rb", bsp);
        if (aibp2 == NULL)
	{
          SeqEntryFree(pse);
          CloseSENGI();
          FileClose(fp);
          return Error_msg_return("Writeff: AsnIoBSOpen failed.", FALSE);
	}
        pse_single = SeqEntryAsnGet(aibp2->aip, NULL, sip, SEQENTRY_READ_BIOSEQ);
        if(pse_single == NULL)
	{
           SeqEntryFree(pse);
           SeqEntryFree(pse_single);
           CloseSENGI();
           FileClose(fp);
           return PrintFasta ( gival, typestr ); 
           /*return Error_msg_return("Writeff: SeqEntryAsnGet failed.", FALSE);*/
	}
        AsnIoBSClose(aibp2);
        BSFree(bsp);
        
	ObjMgrRegister(OBJ_SEQENTRY, pse_single);

        if (! SeqEntryToFlatEx(pse_single, fp, GENPEPT_FMT, RELEASE_MODE, NULL, FF_REGULAR))
        {
           SeqEntryFree(pse);
           SeqEntryFree(pse_single);
           CloseSENGI();
           FileClose(fp);
           return Error_msg_return("Writeff: SeqEntryToFlat failed.", FALSE);
        }
   }
   else
   {
       SeqEntryFree(pse);
       CloseSENGI();
       FileClose(fp);
       return Error_msg_return("Writeff: Invalid molecule type.", FALSE);
   }
#endif

   fprintf ( stdout, "<pre>" );
    
   if ( !SHoundGetGenBankff ( gival, stdout ) )
   {
      fprintf ( stdout, "<b>Invalid GenBankff format</b><br/>" );
      return FALSE;
   }

   fprintf ( stdout, "</pre>" );
   fflush ( stdout );

#if 0
   FileClose ( fp );
   SeqEntryFree ( pse );
   CloseSENGI ();
#endif

   return TRUE;
}


/*
Displays taxonomy info about given sequence
*/
Int2 GetTaxonomy(Int4 gival)
{
   Int4 taxval = 0;
   CharPtr pname = NULL;
   CharPtr plin = NULL;
   StTaxgiPtr ptg = NULL;
   /* SK: */
   CharPtr strptr = NULL;

   ptg = StTaxgiNew();
   /* SK: NULL ptr check */
   if ( ptg == NULL )
      return -1;

   if (gival == 0)
      return 0;
   else
     ptg->gi = gival;

   if(SearchTAXGI(&ptg) != 1) {
      StTaxgiFree(ptg);
      printf("<b>GI %ld does not have an entry in taxonomy database.</b><br>", (long) gival);
      ErrPostEx(SEV_ERROR,0,0, "No taxonomy assignment for GI %ld was found.", (long) gival);
      return -1;
   } else {
     taxval = ptg->taxid;
     StTaxgiFree(ptg);
     pname = SHoundGetTaxNameFromTaxID(taxval);
     if(pname == NULL) {
       printf("<b>GI %ld does not have an entry in taxonomy database.</b><br>", (long) gival);
       ErrPostEx(SEV_ERROR,0,0, "TaxDB_GetNameFromTaxID returned NULL.");
       return -1;
     }
     plin = SHoundGetTaxLineageFromTaxID(taxval, SHoundGetTaxLineage_GENBANK);
     if(plin == NULL) {
       printf("<b>GI %ld does not have an entry in taxonomy database.</b><br>", (long) gival);
       ErrPostEx(SEV_ERROR,0,0, "SHoundGetTaxLineageFromTaxID returned NULL.");
       return -1;
     }
     /* SK: */
     strptr = Cfg_GetCgiPath();
     printf("<b>Taxonomy information<br><a href=\"%s?field=Taxonomy+ID&GI=%ld&format=defline\">taxonomy ID: %ld</a></b><br>  ",IsNullStr(strptr), (long) taxval, (long) taxval);
     printf("<b>name:</b> %s<br>", IsNullStr(pname));
     printf("<b>lineage:</b> %s<br><br>", IsNullStr(plin));
   }

   fprintf ( stdout, 
         "<a href=\"http://www.ncbi.nlm.nih.gov/entrez/query.fcgi?cmd=Retrieve&db=Taxonomy&list_uids=%ld&dopt=DocSum&tool=SeqHound\" target=\"_blank\">"
         "Get Taxonomy info from NCBI</a><br>", (long) taxval);
   return 1;
}


/* checks if there is a MEDLINE ID for a given GI */
Boolean CheckMedline(Int4 gival)
{
  StPubseqPtr pps = NULL;

  pps = StPubseqNew();
  /* SK: NULL ptr check */
  if ( pps == NULL )
     return FALSE;
  if (gival == 0)
     return FALSE;
   else
     pps->gi = gival;

 if(SearchPUBSEQ(&pps) == 1)
 {
   StPubseqFree(pps);
   return TRUE;
 }
 else
 {
   StPubseqFree(pps);
   return FALSE;
 }
}

/*
 * Displays Medline and PUBMED info link to NCBI
 */
Int2 GetMedline(Int4 gival)
{
   StPubseqPtr        pps = NULL, 
                  ppshead = NULL;

   /* SK: NULL ptr check */
   if ( NULL == (pps = StPubseqNew()) ) { return -1; }

   ppshead = pps;

   if ( 0 == gival ) 
      { return 0; }
   else 
      { pps->gi = gival; }

   if( SearchPUBSEQ( &pps ) != 1 )
   {
      StPubseqFree( pps );
      printf( "<b>GI %ld does not have an entry in medline/pubmed database.</b><br>", (long) gival );
      return -1;
   } else {
    
      printf( "GI %ld is linked to medline record(s):", (long) gival );
      while( NULL != pps )
      {
         if ( 0 < pps->muid )
         {
            printf( " <a href=\"http://www.ncbi.nlm.nih.gov/entrez/query.fcgi?cmd=Retrieve&db=PubMed&list_uids=%ld&dopt=Citation&tool=SeqHound\" target=\"_blank\">%ld</a>&nbsp&nbsp;", 
                    (long) pps->muid, (long) pps->muid );
         } 
         pps = pps->next;
      }
      

      printf("<br/>");
      pps = ppshead; 
      printf( "GI %ld is linked to pubmed record(s):", (long) gival );
      while ( NULL != pps )
      {
         if ( 0 < pps->pmid )
         {
            printf( " <a href=\"http://www.ncbi.nlm.nih.gov/entrez/query.fcgi?cmd=Retrieve&db=PubMed&list_uids=%ld&dopt=Citation&tool=SeqHound\" target=\"_blank\">%ld</a>&nbsp&nbsp;", 
                    (long) pps->pmid, (long) pps->pmid );
         }
         pps= pps->next;
      }
      StPubseqFree ( pps );
   }
   printf("<br>");
   return 1;
}


/*
Finds GI in ASNDB and passes the Data4 buffer to PrintRec
*/
Boolean SeekGI(Int4 gival, CharPtr formptr, Int4 ordinal)
{
   StAsndbPtr ppa;

   /* create new asndb structure to put the info into*/
   ppa = StAsndbNew();
   /* SK: NULL ptr check */
   if ( ppa == NULL )
      return Error_msg_return("SeekGI: StAsndbNew failed.", FALSE);
   ppa->gi = gival;

   if(!strstr(formptr,"GenBank")) {
	   if(SearchASNDB(&ppa) != 1) 
	   {
	     StAsndbFree(ppa);
	     printf("<b>GI %ld was not found in SeqHound.</b><p>", (long) gival);
	     fprintf ( stdout, "Check NCBI's Entrez system ");
	     fprintf ( stdout, "<a href=\"http://www.ncbi.nlm.nih.gov/entrez/query.fcgi?db=Protein&cmd=search&term=%ld\" "
	                        "target=\"_blank\">",
	               (long) gival );
	     fprintf ( stdout, "here</a><p/>" );
	     /*return FALSE;*/
	     return TRUE;
	   }
   }

   if (!PrintRec(formptr, ppa))
   {
   	if(!strstr(formptr,"GenBank")) {
	       StAsndbFree(ppa);
	}
      return Error_msg_return("SeekGI: PrintRec failed.", FALSE);
   }
   if(!strstr(formptr,"GenBank")) {
   	StAsndbFree(ppa);
   }
   printf("<br>");
   return TRUE;

}
#ifdef ENABLE_ONTOGLYPHS
Boolean displayOntoglyphs ( Int4 gi )
{
	Char queryString[50];


	
	Char* ogf_slv_dir=NULL;
	Char  ogf_slv_path[80];
	size_t ogf_slv_path_length = 65;
	ogf_slv_dir = getcwd(ogf_slv_dir,ogf_slv_path_length);
	sprintf(ogf_slv_path,"%s/ogf_slv.pl",ogf_slv_dir);
	

	sprintf(queryString,"%d",gi);

	pid_t pid;
	int status, died;
	printf("<center>");	
	fflush(stdout);
   switch (pid = fork1()) {  /* fork and exec a perl process which will handle the drawing of the ontoglyphs single line view */
	case -1: return FALSE;  /*Unable to fork*/
	case  0: execlp(ogf_slv_path,"ogf_slv.pl", queryString,NULL); exit(0); /*child process launches perl code TODO do this using relative paths */
        default: died = wait(&status); /*parent process waits for child to finish */
   } 
	printf("</center>");
   return TRUE; 
}
#endif

/*
Displays sequence (given in form of ASNDB record) in a desired format
*/
Boolean PrintRec(CharPtr formptr, StAsndbPtr ppa)
{
   Int4 gival = 0,
        immdb = 0;
   FILE *pFaa = NULL;
   BioseqPtr pbsq = NULL;
   CharPtr typestr = NULL,
           cgi_ptr = NULL,
           bufXML  = NULL;
   ValNodePtr pvngi = NULL, pvngia = NULL;
   AsnIoPtr aip = NULL;
   AsnIoBSPtr aipbs = NULL;
   ByteStorePtr bspXML = NULL;
   struct stat buf;
   Int4 iGroupID = 0,
        iOnto    = gival;
   Char jpg[100] = {0};


   /* SK: NULL ptr check */
   if ( ppa == NULL )
	     return Error_msg_return("PrintRec: NULL ppa.", FALSE);

   gival = ppa->gi;
   iOnto = gival;

  if(!strstr(formptr,"GenBank")) {

	   typestr = ppa->type;

	   if((pbsq = ppa->asn1) == NULL)
	     return Error_msg_return("PrintRec: No Bioseq.", FALSE);

   }
   else {
	typestr = SHoundMoleculeType(gival);
   }
   printf("<b> %s GI %ld:</b><br>", IsNullStr(typestr), (long) gival);

   sprintf ( jpg, "/ontoglyph/ontoglyphs/%d.jpg" ); 
   if ( 0 == stat ( jpg, &buf ) )
   {
                                      /* gival's jpg exists */
      iOnto = gival;  

   } else {                           /* gival's jpg doesnot exist, so use redundant group */
      if ( (iGroupID = SHoundRedundantGroupIDFromGI ( gival )) > 0 )
      {
        iOnto = SHoundFirstOfRedundantGroupFromID ( iGroupID );
      }
   }

   /* show desired format */
   if (strstr(formptr, "defline") != NULL)    /* defline */
   {
      if ( (CreateDefline(gival)) == 0 )
      {
         return Error_msg_return("PrintRec: Createdefline failed.", FALSE);
      }
	printf("<BR>");
#ifdef ENABLE_ONTOGLYPHS
      displayOntoglyphs ( iOnto );
#endif
	printf("<BR>");
   } 
   else if (strstr(formptr, "FASTA") != NULL)     /* FASTA */
   {
       pFaa = FileOpen("stdout", "w");
       if (pFaa == NULL)
          return Error_msg_return("PrintRec: Cannot open a file.", FALSE);
       if (strstr(typestr, "protein") != NULL)
        {
         printf("<pre>");
         if (!BioseqToFasta(pbsq, pFaa, FALSE))   /* write out in ASN.1 */
            printf("The sequence record might not contain any sequence.<br>");
         fflush(pFaa);
         printf("</pre>");
        }
       else if (strstr(typestr, "NA") != NULL)
        {
         printf("<pre>");
         if (!BioseqToFasta(pbsq, pFaa,  TRUE))   /* write out in ASN.1 */
            printf("The sequence record might not contain any sequence.<br>");
         fflush(pFaa);
         printf("</pre>");
        }
       else
          return Error_msg_return("PrintRec: Invalid molecule type.", FALSE);
       FileClose(pFaa);
	printf("<BR>");
#ifdef ENABLE_ONTOGLYPHS
       displayOntoglyphs ( iOnto );
#endif
	printf("<BR>");
    }


   else if (strstr(formptr, "ASN.1") != NULL)     /* ASN.1 */
   {
      aip = AsnIoNew(ASNIO_TEXT_OUT, stdout, NULL, NULL, NULL);

      if (aip == NULL) 
      {
         return Error_msg_return("PrintRec: AsnIoPtr is NULL.",FALSE);
      }

      printf("<pre>");

      if (!BioseqAsnWrite(pbsq, aip, NULL))  /* write out in ASCII ASN.1 form */
          return Error_msg_return("PrintRec: BioseqAsnWrite failed.", FALSE);
      AsnIoFlush(aip);
      AsnIoClose(aip);
      printf("</pre><p>");
   }


   else if (strstr(formptr, "GenBank") != NULL)    /* GenBank */
   { 
 
      if (!WriteffII(gival, typestr)) {

         return Error_msg_return("PrintRec: Writeff failed.", FALSE);
	}
 	

   }

   else if (strstr(formptr, "Taxon") != NULL)    /* Taxon */
    {
       if ((GetTaxonomy(gival)) == 0)
          return Error_msg_return("PrintRec: GetTaxonomy failed.", FALSE);
    }

   else if (strstr(formptr, "Medline") != NULL)      /* Medline */
    {
      if ((GetMedline(gival)) == 0)
         return Error_msg_return("PrintRec: GetMedline failed.", FALSE);

    }

   else if(strstr(formptr, "XML") != NULL)
    {
       bspXML = BSNew(5000);
       /* SK: NULL ptr check */
       if (bspXML == NULL)
          return Error_msg_return("PrintRec: bspXML is NULL.",FALSE);
       aipbs = AsnIoBSOpen("w", bspXML);
       if (aipbs == NULL)
          return Error_msg_return("PrintRec: AsnIoBSPtr is NULL.",FALSE);
       /* SK: NULL ptr check */
       if ( aipbs->aip != NULL )
          aipbs->aip->type |= ASNIO_XML;
       if (!BioseqAsnWrite(pbsq, aipbs->aip, NULL))  /* write out in ASCII ASN.1 form */
          return Error_msg_return("PrintRec: BioseqAsnWrite failed.", FALSE);
       AsnIoFlush(aipbs->aip);
       bufXML = BSStringSave(bspXML);
       printf("<pre>");
       XML2HTMLPrintf("%s", IsNullStr(bufXML));
       printf("</pre><p>");
       AsnIoBSClose(aipbs);
       BSFree(bspXML);
       MemFree(bufXML);
    }
   else {
    return Error_msg_return("PrintRec: Sequence format unknown.", FALSE);
   }


   /* after desired format was displayed, show all links */
   printf("Display: \n");
   /* VG: */
#ifdef ENABLE_ONTOGLYPHS 
   /* Ontoglyph link */
   
   fprintf ( stdout, "<a href=\"ogf_info.pl?gi=%d\""
                     " target=\"_blank\" onMouseOver=\"window.status='Ontoglyph Legend'; return true;\" "
                     "onMouseOut=\"window.status='';\">Ontoglyph</a>\n"
                     , iOnto );
#endif
   cgi_ptr = Cfg_GetCgiPath();
   cgi_ptr = IsNullStr(cgi_ptr);
   printf("<a href=\"%s?field=GI+identifier&GI=%ld&format=defline\">Definition line</a>  ", cgi_ptr, (long) gival);
   printf("<a href=\"%s?field=GI+identifier&GI=%ld&format=FASTA\">FASTA format</a>  ", cgi_ptr, (long) gival);
   printf("<a href=\"%s?field=GI+identifier&GI=%ld&format=ASN.1\">ASN.1 format</a>  ", cgi_ptr, (long) gival);
   printf("<a href=\"%s?field=GI+identifier&GI=%ld&format=XML\">XML format</a>  ", cgi_ptr, (long) gival);
   printf("<a href=\"%s?field=GI+identifier&GI=%ld&format=GenBank\">GenBank flat file</a>  ", cgi_ptr, (long) gival);
   printf("<a href=\"%s?field=GI+identifier&GI=%ld&format=Taxon\">Taxonomy link</a>  ", cgi_ptr, (long) gival);
   if(CheckMedline(gival)) {
	 printf("<a href=\"%s?field=GI+identifier&GI=%ld&format=Medline\">Medline link</a>  ", cgi_ptr, (long) gival);
   }

   if (((pvngia = IntrezProteinFromDNA(gival)) != NULL) || (IntrezDNAFromProtein(gival) > 0))
    {
       if(pvngia != NULL) ValNodeFree(pvngia);
       if (strstr(typestr, "protein") != NULL)
          printf("<a href=\"%s?field=Link&GI=%ld&format=%s\">Nucleotide link</a>  ", cgi_ptr, (long)gival, IsNullStr(formptr));
       else if (strstr(typestr, "NA") != NULL)
          printf("<a href=\"%s?field=Link&GI=%ld&format=%s\">Protein link</a>  ", cgi_ptr, (long) gival, IsNullStr(formptr));
    }

   pvngi = IntrezRedundantGroup(gival);
   if((pvngi != NULL) && (pvngi->next != NULL))
       printf("<a href=\"%s?field=Redund&GI=%ld&format=%s\">Redundant group</a>  ", cgi_ptr, (long) gival, IsNullStr(formptr));
   if(pvngi != NULL) ValNodeFree(pvngi);


   if(HasNeighbours(gival)) 
   {
      printf("<a href=\"%s?field=Neighbours&GI=%ld&format=%s\">Sequence Neighbours</a>  ", 
             cgi_ptr, (long) gival, IsNullStr(formptr));
   }

   if(Has3DNeighbours(gival)) 
   {
      printf("<a href=\"%s?field=3DNeighbours&GI=%ld&format=%s\">Neighbours with Structures</a>  ", 
             cgi_ptr, (long) gival, IsNullStr(formptr));
   }

   if((immdb = SHound3DFromGi(gival)) > 0) 
   {
      printf("<a href=\"http://www.ncbi.nlm.nih.gov/entrez/query.fcgi?cmd=Retrieve&db=Structure&list_uids=%ld&dopt=DocSum&tool=SeqHound\" target=\"_blank\">Structure Summary</a>  ",(long)immdb);
   }
   printf("<br>");

   return TRUE;
}




/*Given : the information retrieved from the cgi
*  and a pointer to a posting to store all the gis.
*  Returns all the Gis that correspond to that query.
*  -1 if there was nothing found or if an error occurred.
*/
Int4 GetGis(WWWInfoPtr info, PostingPtr* psp){

   CharPtr fieldptr = NULL;
   CharPtr valptr0 = NULL;
   CharPtr valptr = NULL;
   CharPtr formatptr = NULL;
   ValNodePtr pvngi = NULL, pvngihead = NULL;
   Int4 gival = 0;
   Int4 numval = -1;
   Int4 count = 0;
   Int4 i = 0;
   Boolean isstr = FALSE;
   Int4 gin = 0;
 
   CharPtr TaxType;
   Int4 * gis;
   struct Tax_index * index;
   FILE * fptr, * pre_compfptr;
   Int4 querycounter =0, size =0;
   Char * precompname;
   Char errors[256];
   ValNodePtr gis_token = NULL, gis_token_head = NULL, pvntemp = NULL;
   FLinkSetPtr flsp = NULL;
   ILinkSetPtr ilsp = NULL;
   FloatHi evalue = 0.01;
   Int4 icounter = 0, vallen=0;
   CharPtr perrors = NULL;
   StAccdbPtr pac = NULL, pachead = NULL;
   StTaxgiPtr ptg = NULL, ptghead = NULL;
   StPubseqPtr pps = NULL, ppshead = NULL;

   CharPtr pVersion = NULL;

   if ((valptr0 = WWWGetValueByName(info, "GI")) == NULL)
       return Error_msg_return("No valid search key entered.", 0);

   if(strcmp(valptr0, "") == 0)
      return Error_msg_return_soft("No valid value entered.<br>", 0);

   /* here decide if there are more numbers/accessions */
   if(!StrToInt4 (valptr0, &numval))   /* numerical value of valptr0 (if any) is in numval */
      isstr = TRUE;

   /* E.B. pad function appears to be unnecessary and seems to be 
      causing crashes. Just copy valptr0 and free valptr at the end */

   
   if(isstr == TRUE) {
#ifdef ODBC
     valptr = StringSave( valptr0 );
#else
     if ((valptr = Pad(valptr0)) == NULL)
       return Error_msg_return("Pad function failed.", 0);
#endif
       }

   if ((fieldptr = WWWGetValueByName(info, "field")) == NULL)
        return Error_msg_return("Did not get search field.", 0);
   if ((formatptr = WWWGetValueByName(info, "format")) == NULL)
        return Error_msg_return("Did not get search field.", 0);

   if(strstr(fieldptr, "Accession") != NULL)           /* process accessions */
   {
      if (isstr == TRUE)
      {
          pac = StAccdbNew();
	  /* SK: NULL ptr check */
	  if ( pac == NULL )
	  {
	    MemFree( valptr );
	     return -1;
	  }   
	  /*RY: allows search accession number along with its version number
	    e.g. accepts both NP_011554.1 and NP_011554, so finds the extension .X and 
	    removes extended version number before searching in ACCDB, bug 1243 related*/
	  
	  pVersion = strstr(valptr, ".");

	  if( pVersion )
	  {
	    *pVersion = '\0';
	  }

          pac->access = StringSave(valptr);
          pachead = pac;
	         if(SearchACCDB(&pac) != 1)
          {
             perrors = MemNew((size_t)((strlen(valptr)+100)*sizeof(Char)));
	     /* SK: NULL ptr check */
	     if ( perrors == NULL )
	     {
                MemFree(valptr);
                StAccdbFree(pachead);
	        return -1;
	     }
             sprintf(perrors,"The key %s was not found in the database of accessions.<br>", IsNullStr(valptr));
             MemFree(valptr);
             StAccdbFree(pachead);
             return Error_msg_free_return_soft(perrors, 0);
          }
          else
          {
             while(pac !=NULL)
             {
                gival = pac->gi;
                WriteItem(*psp,gival);
                count++;
                pac = pac->next;
             }
             StAccdbFree(pachead);
          }
      }
      else
      {
         sprintf(errors,"The key %ld is not an accession.<br>", (long) numval);
         MemFree(valptr);
         return Error_msg_return_soft(errors, 0);
      }
  }

  if(strstr(fieldptr, "Non-GenBank Identifier") != NULL)                  /* process sequence names */
  {
      if (isstr == TRUE)
      {
          pac = StAccdbNew();
	  /* SK: NULL ptr check */
	  if ( pac == NULL )
	  {
	    MemFree( valptr );
	    return -1;
	  }

          pac->namelow = StrLower(valptr);
          pachead = pac;
	         if(SearchACCDB(&pac) != 1)
          {
             perrors = MemNew((size_t)((strlen(valptr)+100)*sizeof(Char)));
	     /* SK: NULL ptr check */
	     if ( perrors == NULL )
	     {
                MemFree(valptr);
                StAccdbFree(pachead);
		return -1;
	     }
             sprintf(perrors,"The key %s was not found in the database of accessions.<br>", IsNullStr(valptr));
             MemFree(valptr);
             StAccdbFree(pachead);
             return Error_msg_free_return_soft(perrors, 0);
          }
          else
          {
             while(pac !=NULL)
             {
                gival = pac->gi;
                WriteItem(*psp,gival);
                count++;
                pac = pac->next;
             }
             StAccdbFree(pachead);
          }
      }
      else
      {
         sprintf(errors,"The key %ld is not a sequence name.<br>", (long) numval);
         MemFree(valptr);
         return Error_msg_return_soft(errors, 0);
      }
  }

  else if (strstr(fieldptr, "Taxonomy ID") != NULL)              /* process taxonomy IDs */
  {
     if (isstr == FALSE)
     {
          if (strstr(fieldptr, "Taxonomy ID NA") != NULL)
               TaxType = "NA";
          else if (strstr(fieldptr, "Taxonomy ID proteins") != NULL)
               TaxType = "protein";
          else
               TaxType = " ";

          if((fptr = FileOpen(Cfg_GetIndexFile(), "rb")) != NULL)
          {
             index = Malloc(Cfg_GetQueryCount() * sizeof(struct Tax_index));
             if(fread(index,sizeof(struct Tax_index), Cfg_GetQueryCount(), fptr)!=0)
             {
                querycounter=0;
                while(querycounter<Cfg_GetQueryCount())
                {
                   if(numval == index[querycounter].tax_id)
                   {
		     char *precompu_path = Cfg_GetPrecompuPath();

                     precompname = Malloc(256 * sizeof(Char));

                     sprintf(precompname , "%s%ld%s", IsNullStr(precompu_path), (long) index[querycounter].tax_id, IsNullStr(TaxType));
                     if((pre_compfptr = FileOpen(precompname, "rb")) != NULL)
                     {
                       if(strstr("NA", TaxType) != NULL)
                         size = index[querycounter].num_DNA_gis;
                       else if(strstr("protein", TaxType) != NULL)
                         size = index[querycounter].num_proteins_gis;
                       else
                         size = index[querycounter].num_gis;

                       gis = Malloc(size * sizeof(Int4));
                       if(fread(gis, sizeof(Int4),size, pre_compfptr)!=0)
                       {
                           while(count < size)
                           {
                             WriteItem(*psp,gis[count]);
                             count++;
                           }
                           if(count != 0)
                           {
                             MemFree(gis);
                             MemFree(precompname);
                             MemFree(index);

			     MemFree( valptr );
                             return count;
                           }
                       }
                       else
                           ErrPostEx(SEV_ERROR,0,0, "Nothing read from file %s.", IsNullStr(precompname));
                       MemFree(gis);
                     }
                     else
                        ErrPostEx(SEV_ERROR,0,0, "Couldn't open %s.",IsNullStr(precompname));
                     MemFree(precompname);
                   }
                  querycounter++;
                }
             }
             else
                ErrPostEx(SEV_ERROR,0,0, "Nothing read from the index file of precomputed.");
             MemFree(index);
             FileClose(fptr);
          }
          else
             ErrPostEx(SEV_ERROR,0,0, "Couldn't open the index file of precomputed.");



         ptg = StTaxgiNew();
	 /* SK: NULL ptr check */
	 if ( ptg == NULL )
	 {
	   MemFree( valptr );
	   return -1;
	 }
         ptghead = ptg;
         ptg->taxid = numval;

        if(SearchTAXGI(&ptg) != 1)
        {
           StTaxgiFree(ptghead);
           sprintf(errors,"The taxon %ld was not found in the database.<br>",  (long) numval);

	   MemFree( valptr );
           return Error_msg_return_soft(errors, 0);
        }
        else
        {
           while(ptg != NULL)
           {
              gival = ptg->gi;
              if (strstr(" ", TaxType) != NULL)
              {
                WriteItem(*psp,gival);
                count++;
              }
              else if (strstr(ptg->type,TaxType) != NULL)
              {
                WriteItem(*psp,gival);
                count++;
              }
              ptg = ptg->next;
           }
           StTaxgiFree(ptghead);
        }
     }
     else
     {
         perrors = MemNew((size_t)((strlen(valptr)+100)*sizeof(Char)));
	 /* SK: NULL ptr check */
	 if ( perrors == NULL )
	 {
	     MemFree( valptr );
	    return -1;
	 }

         sprintf(perrors,"The key %s is not a taxonomy id.<br>", IsNullStr(valptr));

	 MemFree(valptr);
         return Error_msg_free_return_soft(perrors, 0);
     }
  }


  else if (strstr(fieldptr, "Medline ID") != NULL)                /* process medline IDs */
  {
    if (isstr == FALSE)
    {
       pps = StPubseqNew();
       /* SK: NULL ptr check */
       if ( pps == NULL )
       {
	 MemFree( valptr );
	 return -1;
       }

       pps->muid = numval;
       ppshead = pps;

       if(SearchPUBSEQ(&pps) != 1)
       {
         sprintf(errors,"The key %ld was not found in the database of medline links.<br>",  (long) numval);
         StPubseqFree(ppshead);
	 MemFree( valptr );
         return Error_msg_return_soft(errors, 0);
       }
       else
       {
          while(pps != NULL)
          {
	     gival = pps->gi;
	     WriteItem(*psp,gival);
	     count++;
	     pps = pps->next;
          }
          StPubseqFree(ppshead);
       }
    }
    else
    {
        perrors = MemNew((size_t)((strlen(valptr)+100)*sizeof(Char)));
	/* SK: NULL ptr check */
	if ( perrors == NULL )
	{
	  MemFree( valptr );
	  return -1;
	}
        sprintf(perrors, "The key %s is not a medline id.<br>", IsNullStr(valptr));

	MemFree( valptr );
        return Error_msg_free_return_soft(perrors, 0);
    }

  }
  else if (strstr(fieldptr, "Pubmed ID") != NULL)                /* process medline IDs */
  {
    if (isstr == FALSE)
    {
       pps = StPubseqNew();
       /* SK: NULL ptr check */
       if ( pps == NULL )
       {
	 MemFree( valptr );
	 return -1;
       }

       pps->pmid = numval;
       ppshead = pps;

       if(SearchPUBSEQ(&pps) != 1)
       {
         sprintf(errors,"The key %ld was not found in the database of pubmed links.<br>",  (long) numval);
         StPubseqFree(ppshead);
	 MemFree( valptr );
         return Error_msg_return_soft(errors, 0);
       }
       else
       {
          while(pps != NULL)
          {
	     gival = pps->gi;
	     WriteItem(*psp,gival);
	     count++;
	     pps = pps->next;
          }
          StPubseqFree(ppshead);
       }
    }
    else
    {
        perrors = MemNew((size_t)((strlen(valptr)+100)*sizeof(Char)));
	/* SK: NULL ptr check */
	if ( perrors == NULL )
	{
	  MemFree( valptr );
	  return -1;
	}
        sprintf(perrors, "The key %s is not a pubmed id.<br>", IsNullStr(valptr));

	MemFree( valptr );
        return Error_msg_free_return_soft(perrors, 0);
    }

  }

  else if (strstr(fieldptr, "GI identifier") != NULL)                /* process GIs */
  {
     gis_token = Misc_IntervalString2ValNode(WWWGetValueByName(info, "GI"));
     if(gis_token == NULL)
     {
        sprintf(errors, "No valid value(s) entered for GI identifier. SeqHound recognizes an integer or a comma separated list of integers.<br>");

	MemFree( valptr );

        return Error_msg_return_soft(errors, 0);
     }
     gis_token_head = gis_token;

     pvntemp = Misc_IntervalValNodeNextElement(&gis_token);
     while(pvntemp != NULL)
     {
        WriteItem(*psp, pvntemp->data.intvalue);
	count++;
	pvntemp = Misc_IntervalValNodeNextElement(&gis_token);
     }
     if (count == 0)
     {
        ValNodeFree(gis_token_head);
	sprintf(errors, "WriteItem failed.<br>");
	MemFree( valptr );
	return Error_msg_return_soft(errors, 0);
     }
     ValNodeFree(gis_token_head);
  }

  else if (strstr(fieldptr, "Link") != NULL)                        /* process Link */
  {
    if (isstr == FALSE)
    {
		    pvngi = IntrezProteinFromDNA(numval);
		    pvngihead = pvngi;
		    if (pvngi == NULL)
      {
			     gin = IntrezDNAFromProtein(numval);
			     if (gin > 0)
        {
				      WriteItem(*psp, gin);
				      count++;
		      }
        else
        {
     			   sprintf(errors,"No links found for gi %ld.<br>",  (long) numval);
			   MemFree( valptr );
     			   return Error_msg_return_soft(errors, 0);
		      }
	     }
      else
      {
        while(pvngi != NULL)
        {
           WriteItem(*psp, pvngi->data.intvalue);
           pvngi = pvngi->next;
           count++;
        }
	     }
	     ValNodeFree(pvngihead);
    }
    else
    {
      perrors = MemNew((size_t)((strlen(valptr)+100)*sizeof(Char)));
      /* SK: NULL ptr check */
      if ( perrors == NULL )
      {
	MemFree( valptr );
	return -1;
      }
      sprintf(perrors,"The key %s is not a GI.<br>", IsNullStr(valptr));

      MemFree( valptr );
      return Error_msg_free_return_soft(perrors, 0);
    }

  }

  else if (strstr(fieldptr, "Redund") != NULL)                           /* process Redund */
  {
     if (isstr == FALSE)
     {
         pvngi = IntrezRedundantGroup(numval);
         pvngihead = pvngi;
         if(pvngi == NULL)
	 {
	   MemFree( valptr );
	   return Error_msg_return("GetRedundantGroup failed.", 0);
	 }
	 /*newly retrieved data into the bytestore*/
	 else
	 {
	   while(pvngi != NULL)
	   {
	     WriteItem(*psp, pvngi->data.intvalue);
	     pvngi = pvngi->next;
	     count++;
	   }
	 }
	 ValNodeFree(pvngihead);
     }
     else
     {
       perrors = MemNew((size_t)((strlen(valptr)+100)*sizeof(Char)));
       /* SK: NULL ptr check */
       if ( perrors == NULL )
       {
	 MemFree( valptr );
	 return -1;
       }
       sprintf(perrors,"The key %s is not a GI.<br>", IsNullStr(valptr));

       MemFree( valptr );
       return Error_msg_free_return_soft(perrors, 0);
     }
  }


  else if (StringCmp(fieldptr, "Neighbours") == 0) {             /* process neighbours */
	if (isstr == FALSE) {
	  if(SHoundNeighboursFromGi(numval,&flsp,evalue,FALSE)) {
		  sprintf(errors,"No sequence neighbours found for gi %ld.<br>",  (long) numval);
		  MemFree( valptr );
		  return Error_msg_return_soft(errors, 0);
	  }
	  if(flsp != NULL) {
	    if(StringCmp(formatptr, "Medline") == 0) {
	      while (i < flsp->num) {
		if(CheckMedline(flsp->uids[i]) == TRUE) {
		  WriteItem(*psp, flsp->uids[i]);
		  count++;
		}
					i++;
				}
				flsp = FLinkSetFree(flsp);
			} else {
				while (count < flsp->num) {
					WriteItem(*psp, flsp->uids[count]);
					count++;
				}
				flsp = FLinkSetFree(flsp);
			}
	  } else {
	    MemFree( valptr );
	    return Error_msg_return_soft(errors, 0);
	  }
	} else {
	   perrors = MemNew((size_t)((strlen(valptr)+100)*sizeof(Char)));
	   /* SK: NULL ptr check */
	   if ( perrors == NULL )
	   {
	     MemFree( valptr );
	     return -1;
	   }
	   sprintf(perrors,"The key %s is not a GI.<br>", IsNullStr(valptr));

	   MemFree( valptr );
	   return Error_msg_free_return_soft(perrors, 0);
  	}

 } else if (StringCmp(fieldptr, "3DNeighbours") == 0) {
	if (isstr == FALSE) {
		if(SHound3DNeighboursFromGi(numval,&ilsp,evalue)) {
			sprintf(errors,"Error in structure neighbours retrieval for gi %ld.<br>",  (long) numval);
			MemFree( valptr );
			return Error_msg_return_soft(errors, 0);
		}
		if(ilsp != NULL) {
			icounter = ilsp->num - 1;
			while (count < ilsp->num) {
				WriteItem(*psp, ilsp->uids[icounter--]);
				count++;
			}
			ilsp = ILinkSetFree(ilsp);
		} else {
		  MemFree( valptr );
		  return Error_msg_return_soft(errors, 0);
		}
	} else {
	   perrors = MemNew((size_t)((strlen(valptr)+100)*sizeof(Char)));
	   /* SK: NULL ptr check */
	   if ( perrors == NULL )
	   {
	     MemFree( valptr );
	     return -1;
	   }
	   sprintf(perrors,"The key %s is not a GI.<br>", IsNullStr(valptr));

	   MemFree( valptr );
	   return Error_msg_free_return_soft(perrors, 0);
  	}

 }

  MemFree( valptr );
 return count;
}


Int2 Main()
{

   WWWInfoPtr info;
   Int4 count = 0;
   Int4 num_per_page = 20, start = 0, page = 1, i = 0,record_num = 0;
   Int4 initial = 0, final = 0;
   ByteStorePtr bsp;
   PostingPtr psp;
   LocalLinkSetPtr lsp;


   ErrSetOptFlags(EO_LOG_SEVERITY | EO_LOG_CODES | EO_LOG_FILELINE | EO_LOG_USRFILE);
   BSErrLogOpen("wwwseekgi_logs", ELOG_BANNER | ELOG_APPEND);
   ErrSetLogLevel(SEV_INFO);
   ErrSetMessageLevel(SEV_MAX);

/*#ifndef SHFCGI*/
   printf("Content-type: text/html\r\n\r\n");
/*#endif*/
   /* open CodeBase databases */
   if(!open_databases())
   {
	 Error_msg_return("Unable to open databases.", 0);
	 WWWPrintFileData(TAILFILE, stdout);
	 return 0;
   }
#ifdef SHFCGI
   while (FCGI_Accept() >= 0) /* begin fast cgi loop */	
   {
     BSErrLogOpen("wwwseekgi_logs", ELOG_BANNER | ELOG_APPEND);
     printf("Content-type: text/html\r\n\r\n");
     printf("<h2>POWERED BY FAST CGI</h2><p>");
#endif

   Cfg_shound_new();
   initial = GetSecs();
   if (WWWGetArgs(&info)!= WWWErrOk)
   {
      Error_msg_return("Failed to process arguments.",1);
      WWWPrintFileData(HEADFILE, stdout);
      WWWPrintFileData(TAILFILE, stdout);
      BSErrLogClose();
      Cfg_FreeMem();
#ifdef SHFCGI
      continue;
#else
      return 0;
#endif
   }

   ErrPostEx(SEV_INFO,0,0, "Request:");
   bsp = BSNew(1000 * sizeof(Int4));
   /* SK: NULL ptr check */
   if ( bsp == NULL )
   {
      Error_msg_return("BSNew failed.",1);
      WWWPrintFileData(HEADFILE, stdout);
      WWWPrintFileData(TAILFILE, stdout);
      BSErrLogClose();
      Cfg_FreeMem();
      return 0;
   }

   psp = NewPost(bsp, 1000 * sizeof(Int4));
   /* SK: NULL ptr check */
   if ( psp == NULL )
   {
      Error_msg_return("NewPost failed.",1);
      WWWPrintFileData(HEADFILE, stdout);
      WWWPrintFileData(TAILFILE, stdout);
      BSErrLogClose();
      Cfg_FreeMem();
      return 0;
   }

   count = GetGis(info, &psp);
   if(count == 0)
   {
      /*WWWPrintFileData(HEADFILE, stdout);*/
      WWWPrintFileData(TAILFILE, stdout);
      BSErrLogClose();
      WWWInfoFree(info);
      if(psp)FreePost(psp);
      Cfg_FreeMem();
#ifdef SHFCGI
      continue;
#else
      return 0;
#endif
   }

   num_per_page = GetIntegerValue(info, "npp", 20);
   page = GetIntegerValue(info, "page",1);
   if(page > num_pages(info,count))
      page = 1;
   start = (page-1) * num_per_page;
   FlushItems(psp);
   if((lsp = BSPtoLSP(psp->uids)) == NULL)
   {
      Error_msg_return("BSPtoLSP failed.",1);
      WWWPrintFileData(HEADFILE, stdout);
      WWWPrintFileData(TAILFILE, stdout);
      if(psp)FreePost(psp);
      WWWInfoFree(info);
      Cfg_FreeMem();
      BSErrLogClose();
#ifdef SHFCGI
      continue;
#else
      return 0;
#endif
   }
   print_Top(info, count);
   record_num=start;
   i = 0;
   while((i < num_per_page) && (record_num < lsp->num))
    {
      if (!SeekGI (lsp->uids[record_num], WWWGetValueByName(info, "format"), record_num+1))
      {
        /*Error_msg_return("SeekGI failed.", 1);*/
        if(psp)FreePost(psp);
        if(lsp)LocalLinkSetFree(lsp);
        WWWInfoFree(info);
        Cfg_FreeMem();
        WWWPrintFileData(TAILFILE, stdout);
        BSErrLogClose();
#ifdef SHFCGI
        continue;
#else
        return 0;
#endif
      }
      record_num++;
      i++;
    }
   LocalLinkSetFree(lsp);
   FreePost(psp);
   WWWInfoFree(info);
   WWWPrintFileData(TAILFILE, stdout);
   final = GetSecs();
   ErrPostEx(SEV_INFO,0,0, "time taken = %ld",  (long) final-initial);
   ErrPostEx(SEV_INFO,0,0, "Done.");
#ifdef SHFCGI
   Cfg_FreeMem();
   BSErrLogClose();
   } /*end of the fast cgi while loop*/
#endif

   Cfg_FreeMem();
   close_databases();
   BSErrLogClose();
   return 0;
}

/*
$Log: wwwseekgi.c,v $
Revision 1.36  2005/04/27 16:01:36  vgu
Fix missing conditional compilation flag for ontoglyph function

Revision 1.35  2005/02/22 17:08:31  ryao
removed duplicated header files

Revision 1.34  2005/02/22 15:02:15  mmatan
Now only calls SearchASNDB if the request is not for the genbank flat file,
since calling this before calling SHoundGetGenBankff appears to cause the latter
to malfunction.  This is a workaround to fix bug 3969.

Revision 1.33  2004/11/10 21:48:16  mmatan
Ontoglyphs functionality is now controlled by an #ifdef ENABLE_ONTOGLYPHS
preprocessor conditional compilation flag.  The default compilation for the odbc
target of wwwseekgi will be WITHOUT ontoglyphs functionality, since this is for
release to the general public.  If one wishes to enable ontoglyphs
functionality, they should compile the target odbc_internal.

Revision 1.32  2004/10/22 13:46:25  mmatan
changed drop down field option "Seq. Name" to "Non-GenBank Identifier" as
requested in bugzilla report 3661

Revision 1.31  2004/10/07 14:54:10  mmatan
Modified to generate single line view of ontoglyphs.  The web interface is now
dependant on the perl program ogf_slv.pl for generation of ontoglyphs, which
should be in the same directory as the wwwseekgi cgi.

Revision 1.30  2004/10/06 17:09:48  kbantoft
Nasty typo in public facing error messages are not good

Revision 1.29  2004/09/08 19:09:35  mmatan
This adds support for searching by pubmed ID to the interface.  This completes
the fix for bug 3113.

Revision 1.28  2004/07/28 21:07:17  eburgess
In CreateDefline(), check that pac->title is not NULL before calling
strcmp to prevent a crash.

Revision 1.27  2004/07/09 16:29:34  vgu
Remove memory free call to pointer pRemainer

Revision 1.26  2004/07/09 16:09:08  vgu
Forgot to check whether compile is for ODBC.

Revision 1.25  2004/07/09 15:09:56  vgu
Do not call Pad() in the ODBC version as it appears to cause crashes. Also,
fixed some memory leaks in GetGis(). EB. Bugzilla 2713

Revision 1.24  2004/06/18 22:16:49  hlieu
incorporate bug fix 1162, the new GenBankff into wwwseekgi and seqrem

Revision 1.23  2004/06/15 16:23:08  seqhound
Add Boolean flag to function call CloseAllNbrDBs

Revision 1.22  2004/06/14 20:48:17  hlieu
redundancy all correct

Revision 1.21  2004/06/11 21:23:19  hlieu
ontoglyphs, add redundancy check

Revision 1.20  2004/06/10 17:30:02  zwang
Add flag to determine if a database connection should be freed. See bug #2324.

Revision 1.19  2004/06/07 20:20:49  hlieu
better checking of map/jpg files without having to open file. use fstat

Revision 1.18  2004/06/07 15:32:56  hlieu
fix ontoglyphs

Revision 1.17  2004/05/31 20:19:15  hlieu
minor fixes to prevent warnings in bug 2185

Revision 1.16  2004/05/17 22:39:46  hlieu
remove temp fix

Revision 1.15  2004/05/17 22:33:31  hlieu
added map to ontoglyphs


Revision 1.14  2004/05/17 20:21:09  hlieu
remove warnings

Revision 1.13  2004/05/17 20:11:37  hlieu
remove C++ style comments

Revision 1.12  2004/05/14 15:05:34  hlieu
fix broken compile farm. New fix uses different library

Revision 1.11  2004/05/03 13:18:48  eburgess
Fix cross compile errors: Declare variables at the start of a block.

Revision 1.10  2004/04/29 15:40:48  hlieu
centered ontoglyph added link

Revision 1.9  2004/04/28 20:12:47  hlieu
added ontoglyph support

Revision 1.8  2004/04/21 19:55:40  hlieu
fix bug 1946, <p/> after message

Revision 1.7  2004/04/21 19:07:16  hlieu
add pmid links

Revision 1.6  2004/04/07 15:23:23  hlieu
fixed filtering so that broken gi's are found on the fly
implemented a fasta output for broken gi's with a screen message
and link to ncbi's entrez system.

Revision 1.5  2004/04/06 20:12:57  hlieu
fixed filter for broken gi's, links to NCBI

Revision 1.4  2004/03/12 17:00:36  hlieu
fixed old entrez links to new entrez links

Revision 1.3  2004/02/04 21:42:09  ryao
accept accession version numbers as accesssions, fix bug 1243.

Revision 1.2  2004/01/06 22:32:10  skuznets
Code audit has been done sucessfully.
Possible NULL pointer issues has been eliminated.

Revision 1.19  2003/07/23 21:25:30  haocl
fixed pubmed linkage, ncbi changed url format

Revision 1.18  2003/05/31 01:30:45  kaca
flat file fix has to be optimized

Revision 1.17  2003/05/23 19:00:23  kaca
changed error message

Revision 1.16  2003/05/23 02:20:24  kaca
fixed flat file fetch; shows only one sequence per gi

Revision 1.15  2002/10/15 19:59:09  kaca
used to crush when one of many record was not found in the system; now it just reports the missing one and finishes

Revision 1.14  2002/09/27 16:52:59  kaca
swithched to  read only mode for SHound initialization

Revision 1.13  2002/07/15 21:42:48  kaca
sequence name searches work in lower case

Revision 1.12  2002/06/06 17:11:07  kaca
small change to MEDLINE ID display

Revision 1.11  2002/03/25 07:51:32  micheld
New genome functions. changed includes to new slrilinkset.h

Revision 1.10  2002/02/13 22:26:05  kaca
changed the order of functions for easier perousing, small change in error handling

Revision 1.9  2002/02/12 15:30:09  mdumontier
make call to Has3DNeighbours

Revision 1.8  2002/02/06 23:56:45  micheld
Changed TaxDB_ Functions to SHound*Tax* function call, legacy preserved for C api but *NOT* PERL

Revision 1.7  2002/01/21 18:43:10  kaca
removed last pieces of CodeBase specific code

Revision 1.6  2002/01/15 21:28:11  kaca
abstracted all database calls

Revision 1.5  2001/12/11 17:53:05  kaca
fixed potential buffer overflow upon error messaging

Revision 1.4  2001/11/21 23:52:23  kaca
database opening controled by CREATE Boolean

Revision 1.3  2001/10/10 22:07:12  kaca
resolved import conflict


Revision 1.1.1.2  2001/10/10 21:53:46  kaca
initial import

Revision 1.26  2001/10/04 22:50:01  kaca
merged back development of fast and slow CGI

Revision 1.25  2001/08/24 22:03:16  michel
Reversed order for neighbours with structures to show most best eval first
Added defline display to Medline Link

Revision 1.24  2001/08/24 20:57:36  michel
Added Neighbours, 3D Neighbours, and MMDB-NCBI link to Web Seqhound
SEQHOUNDNBRS compiler flag required for compile of these functions

Revision 1.23  2001/06/20 17:33:56  kaca
proteins displayed as single GenPept file

Revision 1.22  2001/05/04 15:49:55  kaca
numbering of records behaved differently on different platforms ..fixed

Revision 1.21  2001/04/19 15:41:15  kaca
changed FileOpen mode upon reading precomputed

Revision 1.20  2001/04/06 22:37:23  kaca
removed Posting and LinkSet declarations; they are in SeqHound local library now

Revision 1.19  2001/04/03 19:26:43  kaca
changed behaviour upon not finding precomputed; upon not finding a specific GI; parsing of multiple GI input

Revision 1.18  2001/03/30 23:30:59  kaca
changes in error handling for fast cgi

Revision 1.16  2001/03/29 16:31:46  kaca
added slri_misc include

Revision 1.15  2001/03/26 19:47:29  kaca
changes in include files

Revision 1.13  2001/03/15 21:13:10  kaca
added XML functionality, removed server disk writes, fixed format obtion capitalization

Revision 1.12  2001/02/08 19:47:04  kaca
cosmetic changes

Revision 1.11  2001/02/07 21:33:40  kaca
removed unused variables

Revision 1.10  2001/02/07 16:44:58  kaca
cosmetic changes

Revision 1.9  2000/11/14 15:01:05  kaca
display pager only if more pages of results

Revision 1.8  2000/11/13 21:15:34  kaca
fixed record counter after myself

Revision 1.7  2000/11/13 17:47:05  kaca
top form of SeqHound result page restructured

Revision 1.6  2000/10/24 18:49:41  kaca
taxonomy ID DNA changed into NA

*/






