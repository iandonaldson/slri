/*$Id: prebind_cgi.c,v 1.34 2003/07/29 01:08:26 janicek Exp $*/
/*
-- ***************************************************************************
--
--  Pre Biomolecular Interaction Network Database (PreBIND)
--
--  File: prebind_cgi.c
--  Description: PreBIND cgi interface to PreBIND databases
--				 (see also pb_parser.c and pb_searcher.c)  
--				 
--
--  Authors: Ian Donaldson (ian.donaldson@utoronto.ca)
--			 and Christopher W.V. Hogue (hogue@mshri.on.ca)
--
--  Date: June 2002
--
--  Hogue Lab - University of Toronto Biochemistry Department
--  Samuel Lunenfeld Research Institute, Mount Sinai Hospital
--
--  REVISION/CONTRIBUTION NOTES:
--
--  June 1, 2000  Original MSH Revision.
--              Hogue Lab - University of Toronto Biochemistry Department 
--              and the Samuel Lunenfeld Research Institute, Mount Sinai Hospital
--              http://bioinfo.mshri.on.ca  hogue@mshri.on.ca
--              ftp://ftp.mshri.on.ca/
--
--
--  Copyright Notice:
--
--  Copyright ©2000 Mount Sinai Hospital (MSH)
--
--  This program is free software; you can redistribute it and/or
--  modify it under the terms of the GNU General Public License as
--  published by the Free Software Foundation; either version 2 of
--  the License, or any later version.

--
--  This program is distributed in the hope that it will be useful,
--  but WITHOUT ANY WARRANTY; without even the implied warranty of
--  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
--
--  See the GNU General Public License for more details.
--
--  You should have received a copy of the GNU General Public License
--  along with this program; if not, write to the
--         Free Software Foundation, Inc.,
--         59 Temple Place, Suite 330, Boston, MA
--         02111-1307  USA
--  or visit http://www.gnu.org/copyleft/gpl.html
--
--  SPECIAL EXCEPTIONS
--
--  As a special exception, Mount Sinai Hospital gives permission to
--  link this program with the following non-GPL programs or libraries,
--  and distribute the resulting executable, without including the source
--  code for these in the source distribution:
--
--  a) CodeBase 6.5 or greater from Sequiter Software Inc.
--
--  b) The C or C++ interface to Oracle 8.x or greater, from
--     Oracle Corporation or IBM DB2 UDB.
--
-- ***************************************************************************
*/



/*PreBIND.c*/
/*this is a console application in the PreBIND project made using the MCD build of the NCBI toolkit*/
/*the following libraries have been included in the Project Settings
c4lib.lib ncbicdr.lib medarch.lib ncbi.lib ncbinacc.lib ncbiid1.lib ncbimmdb.lib ncbinacc.lib ncbiobj.lib 
ncbitool.lib ncbitxc2.lib netblast.lib netcli.lib netentr.lib ncbimain.obj PreBIND_lib.lib 
kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib 
uuid.lib odbc32.lib odbccp32.lib wsock32.lib*/

/*this includes 
the Codebase c4lib.lib
libraries from the MCD build of the NCBI toolkit
the PreBIND_lib.lib library
all of the default libraries that start with a w32 Console Application
wsock32.lib
and the taskapi.lib (Moez's library that includes WWWGetTaskArgs)
in that order*/


#ifndef _PREBIND_CGI_
#include "prebind_cgi.h"
#include "utilpub.h"
#endif



/*set up command line arguments (see taskapi library)*/
/*****************the taskarg struct*******************************
typedef struct taskargs {
  char			*prompt;            /* prompt for field 
  char			*defaultvalue;      /* default 
  char			*from;              /* range or datalink type 
  char			*to;
  Nlm_Boolean   optional;
  Nlm_Char      tag;				/* argument on command line 
  char			*wwwname;
  Nlm_Int1      type;				/* type of value 
  Nlm_FloatHi   floatvalue;
  Nlm_Int4      intvalue;
  CharPtr       strvalue;
} TaskArgs, * TaskArgPtr;			/* (*) not PNTR for MS Windows 
********************************************************************/
#define NUMARGS 33

TaskArgs myargs[NUMARGS]=
{
	{"Type of search",						"accession",					NULL, NULL, FALSE,	's', "search_type", ARG_STRING, 0.0, 0, NULL},
	{"Accession number to search for",		"no_accession_number_entered",	NULL, NULL, TRUE,	'a', "accession",	ARG_STRING, 0.0, 0, NULL},/*"NP_010141"*/
	{"Name to search for",					"no_name_entered",				NULL, NULL, TRUE,	'n', "name",		ARG_STRING, 0.0, 0, NULL},
	{"Taxon to search for",					"0",							NULL, NULL, TRUE,	't', "taxid",		ARG_INT,	0.0, 0, NULL},
	{"PubMed Identifier",					"0",							NULL, NULL, TRUE,	'P', "pmid",		ARG_INT,	0.0, 0, NULL},
	{"gi of gene product a",				"0",							NULL, NULL, TRUE,	'A', "gia",			ARG_INT,	0.0, 0, NULL},
	{"gi of gene product b",				"0",							NULL, NULL, TRUE,	'B', "gib",			ARG_INT,	0.0, 0, NULL},
	{"frames_ready",						"0",							NULL, NULL, TRUE,	'f', "frames",		ARG_INT,	0.0, 0, NULL},
	{"submitted gi of  a",					"0",							NULL, NULL, TRUE,	'C', "sgia",		ARG_INT,	0.0, 0, NULL},
	{"submitted gi of  b",					"0",							NULL, NULL, TRUE,	'D', "sgib",		ARG_INT,	0.0, 0, NULL},
	{"submitted name of a",					"no_nama_entered",				NULL, NULL, TRUE,	'E', "nama",		ARG_STRING, 0.0, 0, NULL},
	{"submitted name of b",					"no_namb_entered",				NULL, NULL, TRUE,	'F', "namb",		ARG_STRING, 0.0, 0, NULL},
	{"submitted desc of a",					"no_desca_entered",				NULL, NULL, TRUE,	'G', "adesc",		ARG_STRING, 0.0, 0, NULL},
	{"submitted desc of b",					"no_descb_entered",				NULL, NULL, TRUE,	'H', "bdesc",		ARG_STRING, 0.0, 0, NULL},
	{"submitted atype",						"0",							NULL, NULL, TRUE,	'I', "atype",		ARG_INT,	0.0, 0, NULL},
	{"submitted btype",						"0",							NULL, NULL, TRUE,	'J', "btype",		ARG_INT,	0.0, 0, NULL},
	{"submitted aok",						"0",							NULL, NULL, TRUE,	'K', "aok",			ARG_INT,	0.0, 0, NULL},
	{"submitted bok",						"0",							NULL, NULL, TRUE,	'L', "bok",			ARG_INT,	0.0, 0, NULL},
	{"submitted afrom",						"0",							NULL, NULL, TRUE,	'M', "afrom",		ARG_INT,	0.0, 0, NULL},
	{"submitted bfrom",						"0",							NULL, NULL, TRUE,	'N', "bfrom",		ARG_INT,	0.0, 0, NULL},
	{"submitted ato",						"0",							NULL, NULL, TRUE,	'O', "ato",			ARG_INT,	0.0, 0, NULL},
	{"submitted bto",						"0",							NULL, NULL, TRUE,	'P', "bto",			ARG_INT,	0.0, 0, NULL},
	{"submitted opinion",					"0",							NULL, NULL, TRUE,	'Q', "opin",		ARG_INT,	0.0, 0, NULL},
	{"submitted method",					"0",							NULL, NULL, TRUE,	'R', "method",		ARG_INT,	0.0, 0, NULL},
	{"submitted fname",						"Anonymous",					NULL, NULL, TRUE,	'S', "fname",		ARG_STRING, 0.0, 0, NULL},
	{"submitted lname",						"User",							NULL, NULL, TRUE,	'T', "lname",		ARG_STRING, 0.0, 0, NULL},
	{"submitted BIND UID",					"0",							NULL, NULL, TRUE,	'U', "buid",		ARG_INT,	0.0, 0, NULL},
	{"filter non-interaction papers",		"0",							NULL, NULL, TRUE,	'g', "fnip",		ARG_INT,	0.0, 0, NULL},
	{"filter names with too many results",	"0",							NULL, NULL, TRUE,	'h', "fnwtmr",		ARG_INT,	0.0, 0, NULL},
	{"show list of interactors",			"0",							NULL, NULL, TRUE,	'i', "sloi",		ARG_INT,	0.0, 0, NULL},
	{"show results by paper",				"0",							NULL, NULL, TRUE,	'j', "srbp",		ARG_INT,	0.0, 0, NULL},
	{"show list of papers",					"0",							NULL, NULL, TRUE,	'k', "slop",		ARG_INT,	0.0, 0, NULL},
	{"existing BIND interaction id",        "-99999",                       NULL, NULL, TRUE,   'W', "piid",        ARG_STRING, 0.0, 0, NULL},

};

/*arguments for debugging and testing*/
/*submit an interaction*/
/*-s "submit" -A 6320061 -B 6325445 -C 6320061 -D 6325445 -E "RPO21" -F "RPO26" -G "RNA polymerase II large subunit" -H "subunit common to RNA polymerases I, II, and III" -I 2 -J 2 -K 0 -L 0 -Q 1 -S "Ian" -T "Donaldson" -U 1*/
/*print an accession page with accession, search and search results info*/
/*-s "accession" -a NP_010141*/
/*-s "nametax" -n "pol"*/



/*****************************************************
 * WWWPrintFileData looks in the current CGI-BIN directory 
 *  or the "data" subdirectory for the data file.
 *  and prints it out.
 */ 
static void WWWPrintFileData(CharPtr FName,  FILE *pFile)
{
 
   FILE *f = NULL;
   Char fullpath [PATH_MAX];
   CharPtr ptr;  
   Char pcBuf[1024];
   
   /*f = FileOpen (FName, "r");*/
   /*if (f == NULL)  {*/
    ProgramPath (fullpath, sizeof (fullpath) - 1);
    ptr = StringRChr (fullpath, DIRDELIMCHR);
    if (ptr != NULL) {
          *ptr = '\0';
    }
    FileBuildPath (fullpath, "data", FName);
    f = FileOpen (fullpath, "r");
    if (f == NULL) return;  
   /*}*/
      
   do {
     pcBuf[0] = '\0';
     ptr = fgets(pcBuf, (size_t)1024, f);
     if (ptr) fprintf(pFile, ptr);
   } while (ptr);
  
   FileClose(f);
   return;
}



/************************************************************/
/*															*/
/*	ClosePreBIND:											*/
/*			Close all open datafiles and files.			 	*/
/*	PARAMETERS:												*/
/*			void
/*	RETURNS:												*/
/*		0	ok												*/
/************************************************************/
Int2 ClosePreBIND(void)
{	
	/*closes all open data, index and memo files*/
	code4close(&codeBase);		
	code4initUndo(&codeBase);

	return 0;
}


/************************************************************/
/*															*/
/*	PrintAccessionRecordInfo:								*/
/*															*/
/*	PARAMETERS:												*/
/*		PREBINDAccPtr	pap: pointer to an accession record */
/*	RETURNS:												*/
/*		0	ok												*/
/************************************************************/
Int2 PrintAccessionRecordInfo(PREBINDAccPtr pap)
{
	Char	seqhound_location[PATH_MAX];
	CharPtr taxname =	NULL;

	if (!pap)
	{
		ErrPostEx(SEV_ERROR, 0, 0, "PrintAccessionRecordInfo: Invalid function params\n");
		return 1;
	}


	/*get location information from the config file*/
	seqhound_location[0] = '\0';
	GetAppParam("prebind", "prebind_cgi", "seqhound_location", NULL, seqhound_location, PATH_MAX*sizeof(Char));


	/*table header and accesion*/
	fprintf (stdout,"\n<table border=\"0\"  width=\"100%%\" cellspacing=\"2\" cellpadding=\"2\" cols=\"4\">\n");
	fprintf (stdout,"<tr bgcolor=\"lightsteelblue\">\n");
	fprintf (stdout,"<td colspan=4 height=\"20\">\n");
	fprintf (stdout,"<h1>Protein description and name list</h1>\n");
	fprintf (stdout,"</td>\n");
	fprintf (stdout,"</tr>\n");
	fprintf (stdout,"<tr>\n");
	fprintf (stdout,"<td> \n");
	/*gene product*/
	fprintf (stdout,"<table border=\"0\"  width=\"100%%\" cellspacing=\"2\" cellpadding=\"2\" cols=\"4\">\n");
	fprintf (stdout,"<tr>\n");
	fprintf (stdout,"<td valign=\"top\" colspan=\"1\" height=\"20\">\n");
	fprintf (stdout,"<strong>protein name:<strong>\n");
	fprintf (stdout,"</td>\n");
	fprintf (stdout,"<td colspan=\"3\" height=\"20\">\n");
	fprintf (stdout,"%s\n", pap->pprod);
	fprintf (stdout,"</td>\n");
	fprintf (stdout,"</tr>\n");
	/*locus name*/
	fprintf (stdout,"<tr>");
	fprintf (stdout,"<td colspan=\"1\" height=\"20\">\n");
	fprintf (stdout,"<strong>encoding locus name:<strong>\n");
	fprintf (stdout,"</td>\n");
	fprintf (stdout,"<td colspan=\"3\" height=\"20\">\n");
	fprintf (stdout,"%s\n", pap->locus);
	fprintf (stdout,"</td>");
	fprintf (stdout,"</tr>\n");
	/*database*/
	fprintf (stdout,"<tr>\n");
	fprintf (stdout,"<td colspan=1 height=\"20\">\n");
	fprintf (stdout,"<strong>source database:<strong>\n");
	fprintf (stdout,"</td>");
	fprintf (stdout,"<td colspan=3 height=\"20\">\n");
	fprintf (stdout,"%s\n", pap->db);
	fprintf (stdout,"</td>\n");
	fprintf (stdout,"</tr>\n");
	/*accession number*/
	fprintf (stdout,"<tr>\n");
	fprintf (stdout,"<td colspan=1 height=\"20\">\n");
	fprintf (stdout,"<strong>accession number:<strong>\n");
	fprintf (stdout,"</td>");
	fprintf (stdout,"<td colspan=3 height=\"20\">\n");
	fprintf (stdout,"%s\n", pap->acc);
	fprintf (stdout,"</td>\n");
	fprintf (stdout,"</tr>\n");
	/*gi*/
	fprintf (stdout,"<tr>\n");
	fprintf (stdout,"<td colspan=1 height=\"20\">\n");
	fprintf (stdout,"<strong>gi of protein:<strong>\n");
	fprintf (stdout,"</td>\n");
	fprintf (stdout,"<td colspan=3 height=\"20\">\n");
	fprintf (stdout,"%ld ", pap->gi);
	fprintf (stdout, "<a href=\"javascript:void window.open('%s/wwwseekgi?field=GI+identifier&GI=%ld&format=defline', 'giWin', 'toolbar=no,location=no,directories=no,status=yes,menubar=yes,scrollbars=yes,resizable=yes,width=550,height=400');\" onmouseover=\"window.status='Go to SeqHound';return(true);\" onmouseout=\"window.status=''\">SeqHound</a>\n",seqhound_location, (long) pap->gi);
	fprintf (stdout, "(<a href=\"javascript:void window.open('http://www.ncbi.nlm.nih.gov/entrez/query.fcgi?cmd=Retrieve&db=Protein&dopt=DocSum&list_uids=%ld', 'giWin', 'toolbar=no,location=no,directories=no,status=yes,menubar=no,scrollbars=yes,resizable=yes,width=550,height=400');\" onmouseover=\"window.status='Go to Entrez';return(true);\" onmouseout=\"window.status=''\">NCBI</a>)\n", (long) pap->gi, (long) pap->gi);
	fprintf (stdout,"</td>\n");
	fprintf (stdout,"</tr>\n");
	/*taxid*/
	taxname = GetTaxNameByTaxId(pap->tax);
	if (taxname == NULL)
	{
		/*a tax name cannot be found*/
		ErrPostEx(SEV_ERROR,0,0,"PrintAccessionRecordInfo: Unable to find a taxon name.");
	}
	fprintf (stdout,"<tr>\n");
	fprintf (stdout,"<td colspan=1 height=\"20\">\n");
	fprintf (stdout,"<strong>taxon:<strong>\n");
	fprintf (stdout,"</td>");
	fprintf (stdout,"<td colspan=1 height=\"20\">\n");
	if(taxname)
	{
		fprintf (stdout, "%s  \n", taxname );
	}
	fprintf (stdout, "<a href=\"javascript:void window.open('http://www.ncbi.nlm.nih.gov/htbin-post/Taxonomy/wgetorg?id=%ld', 'taxWin', 'toolbar=no,location=no,directories=no,status=yes,menubar=no,scrollbars=yes,resizable=yes,width=550,height=400');\" onmouseover=\"window.status='Go to taxonomy browser';return(true);\" onmouseout=\"window.status=''\">(%ld)</a>\n", (long) pap->tax, (long) pap->tax );
	fprintf (stdout,"</td>\n");
	fprintf (stdout,"</tr>\n");


	/*end of the table*/
	fprintf (stdout,"</table>\n");
	fprintf (stdout,"</td>\n");
	fprintf (stdout,"</tr>\n");
	fprintf (stdout,"</table>\n");
	fprintf (stdout,"<BR><BR>\n");

	fflush(stdout);

	/*good old straight text
	fprintf (stdout, "<BR><h1>%s</h1>\n", accession);
	fprintf (stdout, "<BR>locus name: %s\n", locus_name);
	fprintf (stdout, "<BR>gene product: %s\n", gene_product);
	fprintf (stdout, "<BR>Origin of this accession record: %s\n", database);
	fprintf (stdout, "<BR>gi: %ld\n", (long) gi);
	fprintf (stdout, "<BR>taxon identifier: %ld\n", (long) taxid);
	fprintf (stdout, "<BR>\n");
	fprintf (stdout, "<HR>\n");
	*/

	return 0;
}

/************************************************************/
/*															*/
/*	PrintDisambiguationPage:								*/
/*			Prints a page informing the user that the		*/
/*			requested name refers to more than one protein	*/
/*			and asks them to specify what they want			*/
/*	PARAMETERS:												*/
/*			void											*/
/*	RETURNS:												*/
/*		0	ok												*/
/************************************************************/
Int2	PrintDisambiguationPage(ValNodePtr list_head)
{

	ValNodePtr				vnp =	NULL;
	CharPtr					name =	myargs[2].strvalue;
	Int4					taxid =	myargs[3].intvalue;
	Int4					gi =	0;
	PREBINDAccPtr		pap =	NULL;


	/*prepare for html output*/
	fprintf (stdout, "Content-type:text/html\r\n\r\n");
	WWWPrintFileData(HEADERFILE,  stdout);
	fprintf (stdout, " <BR>");
	if (taxid == 0)
	{
		fprintf (stdout, "<h2>Search Results for \"%s\":</h2><P>\n", name);	
	}
	else if (taxid != 0)
	{
		fprintf (stdout, "<h2>Search Results for \"%s\" and taxon %ld:</h2><P>\n", name, (long) taxid);	
	}
	fprintf (stdout, " <BR>");

	/*check for NULL results*/
	if (!list_head)
	{
		fprintf (stdout, "<BR>No matches were found.\n");
	}
	else vnp = list_head;

	while (vnp)
	{
		gi = vnp->data.intvalue;

		/*get accession record by gi and print a summary with a link to the record*/
		pap = GetAccessionRecordbyGI(gi);
		if (pap != NULL)
		{
			if (PrintSummaryAccessionRecord(pap) != 0)
			{
				ErrPostEx(SEV_ERROR, 0, 0, "PrintDisambiguationPage: PrintSummaryAccessionRecord failed\n");
				return 1;
			}
		}

		vnp = vnp->next;
	}

	/*finish html output*/
	WWWPrintFileData(FOOTERFILE,  stdout);
	fflush(stdout); 


	return 0;
}

/************************************************************/
/*															*/
/*	PrintErrorinFindingAccession:							*/
/*														 	*/
/*	PARAMETERS:												*/
/*			void
/*	RETURNS:												*/
/*		0	ok												*/
/************************************************************/
Int2	PrintErrorinFindingAccession(CharPtr accession)
{
	fprintf (stdout, "Content-type:text/html\r\n\r\n");
	fprintf (stdout, "<BR>Accession number: %s was not found in the PreBIND database.\n", accession);
	fprintf (stdout, "<BR>There are two possible reasons for this:\n");
	fprintf (stdout, "<BR>\n");
	fprintf (stdout, "<BR>(1) The accession number you entered does not correspond to a protein record in RefSeq.  ");
	fprintf (stdout, "You can look for REFSEQ accession numbers <a href = \"http://www.ncbi.nlm.nih.gov/\">here.</a>  ");
	fprintf (stdout, "Select \"Search Proteins for\" then enter a search term and press \"Go\".  ");
	fprintf (stdout, "All proteins in RefSeq will have accession numbers that begin with \"NP_\" and will have the letters \"ref\" in the definition line\n.");
	fprintf (stdout, "<BR>\n");
	fprintf (stdout, "<BR>(2) The RefSeq protein accession number you entered is not in the current update of PreBIND yet.\n");
	return 0;	
}

/************************************************************/
/*															*/
/*	GetGIbyAccession:										*/
/*														 	*/
/*	PARAMETERS:												*/
/*															*/
/*	RETURNS:												*/
/*		0	error											*/
/*		>0	gi												*/
/************************************************************/
Int4	GetGIbyAccession(CharPtr accession_input)
{
	
	Int4	gi =			0;
	CharPtr accession =		NULL;
	Int2	acc_record =	0;

	if (accession_input == NULL)
	{
		ErrPostEx(SEV_ERROR,0,0, "GetGIbyAccession: Failed to allocate sufficient memory");
		return 0;
	}

	/*select the accession tag*/
	d4tagSelect(pd4_ACC, pt4_ACC_ACC);
	
	/*allocate memory to accession and pad the accession number for searching*/
	accession = (CharPtr) malloc(sizeof(Char) * (f4len(pf4_ACC_ACC) + 1));
	if (accession == NULL)
	{
		ErrPostEx(SEV_ERROR,0,0, "GetGIbyAccession: Failed to allocate sufficient memory");
		return 0;
	}
	StrCpy(accession, accession_input);
	PadStringII(pf4_ACC_ACC, accession);/*sufficient memory was pre-allocated for padding*/
	
	acc_record = d4seek(pd4_ACC, accession);
	if (acc_record == 0)/*r4success*/
	{
		/*get gi for the current record*/
		gi = f4long(pf4_ACC_GI);
	}

	/*Mem Free*/
	if (accession) accession = MemFree(accession);

	
	return gi;

}


/************************************************************/
/*															*/
/*	FilterPointByNamesWithTooManyResults:					*/
/*		For a given POINT record, returns TRUE if the POINT	*/
/*		is to be filtered because the paper (pmid) was		*/
/*		entirely by names (for either gia or gib) that		*/
/*		returned too many results and is therefore			*/
/*		considered an unreliable search result.			 	*/
/*	PARAMETERS:												*/
/*		Int4 gia,	gi of protein a							*/
/*		Int4 gib,	gi of protein b							*/
/*		Int4 pmid,  pmid of paper							*/
/*		Int4 many	definition of too many search results	*/
/*	RETURNS:												*/
/*		1	this point should be filtered					*/
/*		0	this point should NOT be filterd				*/
/************************************************************/
Boolean	FilterPointByNamesWithTooManyResults(Int4 gia, Int4 gib, Int4 many)
{

	ValNodePtr			gia_many_names= NULL;
	CharPtr				namgia =		NULL;
	ValNodePtr			gia_names =		NULL;
	ValNodePtr			gib_many_names= NULL;
	CharPtr				namgib =		NULL;
	ValNodePtr			gib_names =		NULL;
	Boolean				filter =		FALSE; /*default*/



	/*get names for gia and gib*/
	if (gia == f4long(pf4_POINT_GIA))
	{
		namgia = StrSave(f4str(pf4_POINT_NAMA));
		namgib = StrSave(f4str(pf4_POINT_NAMB));
		gia_many_names = GetListofNamesWithManyResults(gia, many);
		gib_many_names = GetListofNamesWithManyResults(gib, many);
	}
	else if (gia == f4long(pf4_POINT_GIB))
	{
		namgia = StrSave(f4str(pf4_POINT_NAMA));
		namgib = StrSave(f4str(pf4_POINT_NAMB));
		gia_many_names = GetListofNamesWithManyResults(gib, many);
		gib_many_names = GetListofNamesWithManyResults(gia, many);
	}
	else
	{
		ErrPostEx(SEV_ERROR,0,0, "FilterPointByNamesWithTooManyResults: Programmer error.  Use of this function assumes that gia and gib are from the current POINT record");
		return filter;
	}
	if (gia_many_names == NULL && gib_many_names == NULL) return filter;
	if(TrimSpaces(namgia) != 0)
		ErrPostEx(SEV_ERROR,0,0, "FilterPointByNamesWithTooManyResults: TrimSpaces failed, possibly passed a null string");
	if(TrimSpaces(namgib) != 0)
		ErrPostEx(SEV_ERROR,0,0, "FilterPointByNamesWithTooManyResults: TrimSpaces failed, possibly passed a null string");
	gia_names = SlashDelimitedNamesToValNodeList(namgia);
	gib_names = SlashDelimitedNamesToValNodeList(namgib);			
	
	

	if (many && !filter)
	{
		if (gia_many_names)
		{
			if (gia_names)
			{
				if (AreAllFirstListNamesinSecondList(gia_names, gia_many_names) == TRUE)
				{
					filter = TRUE;
				}
			}
		}
	}			
				
	if (many && !filter)
	{
		if (gib_many_names)
		{
			if (gib_names)
			{
				if (AreAllFirstListNamesinSecondList(gib_names, gib_many_names) == TRUE)
				{
					filter = TRUE;
				}
			}
		}
	}


	if (gia_many_names) gia_many_names = MemFree(gia_many_names);
	if (namgia) namgia = MemFree(namgia);
	if (gia_names) gia_names = MemFree(gia_names);
	if (gib_many_names) gib_many_names = MemFree(gib_many_names);
	if (namgib) namgib = MemFree(namgib);
	if (gib_names) gib_names = MemFree(gib_names);	
	


	return filter;
}

/************************************************************/
/*															*/
/*	AreAnyNamesInBothLists:									*/
/*		Returns TRUE if any one name in one linked list		*/
/*		occurs in the other linked list						*/
/*	PARAMETERS:												*/
/*		vnp1 and vnp2_head are pointers to valnode lists of */
/*		strings												*/
/*	RETURNS:												*/
/*		TRUE or FALSE										*/
/************************************************************/
Boolean	 AreAnyNamesInBothLists(ValNodePtr vnp1, ValNodePtr vnp_head2)
{
	CharPtr pstr1 = NULL;
	CharPtr pstr2 = NULL;
	ValNodePtr vnp2 = NULL;

	if (!vnp1 || !vnp_head2)
	{
		return FALSE;
	}

	while (vnp1)
	{
		pstr1 = (CharPtr) vnp1->data.ptrvalue;
		vnp2 = vnp_head2;
		while (vnp2)
		{
			pstr2 = (CharPtr) vnp2->data.ptrvalue;
			if (StringCmp(pstr1, pstr2) == 0) return TRUE;
			vnp2 = vnp2->next;
		}

		vnp1 = vnp1->next;

	}

	return FALSE; /*default*/

}

/************************************************************/
/*															*/
/*	AreAllFirstListNamesinSecondList:						*/
/*		Returns FALSE if any one name in list 1 is NOT		*/
/*		in list 2.											*/
/*		occurs in the other linked list						*/
/*	PARAMETERS:												*/
/*		vnp1 and vnp2_head are pointers to valnode lists of */
/*		strings												*/
/*	RETURNS:												*/
/*		TRUE or FALSE										*/
/************************************************************/
Boolean	AreAllFirstListNamesinSecondList(ValNodePtr vnp1, ValNodePtr vnp_head2)
{
	CharPtr pstr1 = NULL;
	CharPtr pstr2 = NULL;
	ValNodePtr vnp2 = NULL;
	Boolean name_found = FALSE;

	if (!vnp1 || !vnp_head2)
	{
		return FALSE;
	}

	while (vnp1)
	{
		pstr1 = (CharPtr) vnp1->data.ptrvalue;
		vnp2 = vnp_head2;
		while (vnp2)
		{
			pstr2 = (CharPtr) vnp2->data.ptrvalue;
			if (StringCmp(pstr1, pstr2) == 0) 
			{
				name_found = TRUE;
			}
			vnp2 = vnp2->next;
		}
		if (name_found == FALSE) return FALSE;
		
		/*reset*/
		name_found = FALSE;
		vnp1 = vnp1->next;

	}

	return TRUE; /*default*/

}

/************************************************************/
/*															*/
/*	DescribeNameList:										*/
/*		Prints an input list of slash_delimited names		*/
/*		and a description based on whether any of the names	*/
/*		occur in either of two other lists	 				*/
/*	PARAMETERS:												*/
/*		CharPtr slash_delimited_name_list,					*/
/*		ValNodePtr unofficial_names,						*/
/*		ValNodePtr redundant_names							*/
/*	RETURNS:												*/
/*		0	:no redundant or unofficial names				*/
/*		1	:redundant or unofficial names present			*/
/************************************************************/
Int2	DescribeNameList(CharPtr slash_delimited_name_list, ValNodePtr unofficial_names, ValNodePtr redundant_names)
{
	CharPtr		name_descr =	NULL;
	ValNodePtr	these_names =	NULL;
	Int2		return_value =	0;


	fprintf(stdout,"%s ", slash_delimited_name_list);
	/*examine the list for unofficial names or redundant names*/
	if (unofficial_names || redundant_names)
	{
		these_names = SlashDelimitedNamesToValNodeList(slash_delimited_name_list);
		if (unofficial_names)
		{
			if (AreAnyNamesInBothLists(these_names, unofficial_names))
			{
				fprintf(stdout,"* ");
				return_value = 1;
			}
		}
		if (redundant_names)
		{
			if (AreAnyNamesInBothLists(these_names, redundant_names))
			{
				fprintf(stdout,"! ");
				return_value = 1;
			}			
		}

	}

	return return_value;

}



/************************************************************/
/*															*/
/*	GetListofNamesWithManyResults:							*/
/*		Places a list of names for a gi into a 				*/
/*		ValNode List										*/
/*		Each name returns more than many results			*/
/*	PARAMETERS:												*/
/*		Int4 gi												*/
/*		Int4 many											*/
/*	RETURNS:												*/
/*		ValNodePtr vnp_head: head of the list				*/
/************************************************************/
ValNodePtr	GetListofNamesWithManyResults(Int4 gi, Int4 many)
{
	Int4		srch_record = 0;
	Int4		srch_results = 0;
	CharPtr		srch_name =	  NULL;
	ValNodePtr	vnp_head =	NULL;


	/*find the record corresponding to the gi number in the SRCH database*/
	d4tagSelect(pd4_SRCH, pt4_SRCH_GIP);

	
	srch_record = d4seekDouble(pd4_SRCH, gi);
	if (srch_record != 0)
	{
		return NULL; /*NULL will be returned for an empty list*/
	}
	while (srch_record == 0)/*r4success*/
	{
		/*get source for the current record*/
		srch_results = (long) f4long(pf4_SRCH_RESULTS);

		if (srch_results > many) /*the name more than many results and should be added to the list*/
		{
			srch_name = StrSave(f4str(pf4_SRCH_NAME));
			if (srch_name)
			{
				if(TrimSpaces(srch_name) != 0)
				{
					ErrPostEx(SEV_ERROR,0,0, "GetListofUnofficialNamesbyGI: TrimSpaces failed, possibly passed a null string");
				}
				
				if (vnp_head == NULL)/*this will be the first entry in the list*/
				{
					vnp_head = ValNodeCopyStr(NULL, 1, srch_name);
				}
				else if (vnp_head) /*this will be a subsequent entry*/
				{
					ValNodeCopyStr(&vnp_head, 1, srch_name);
				}

				srch_name = MemFree(srch_name);

			}
		}
		/*look for the next record*/
		srch_record = d4seekNextDouble(pd4_SRCH, gi);
	
	}/*while another record is found for this gi*/	

	
	return vnp_head;
}




/************************************************************/
/*															*/
/*	GetListofUnofficialNamesbyGI:							*/
/*		Places a list of names for a gi into a 				*/
/*		ValNode List										*/
/*		Each name is considered and unofficial name			*/
/*	PARAMETERS:												*/
/*		Int4 gi												*/
/*	RETURNS:												*/
/*		ValNodePtr vnp_head: head of the list				*/
/************************************************************/
ValNodePtr	GetListofUnofficialNamesbyGI(Int4 gi)
{
	Int4		srch_record = 0;
	Int4		srch_source = 0;
	CharPtr		srch_name =	  NULL;
	ValNodePtr	vnp_head =	NULL;


	/*find the record corresponding to the gi number in the SRCH database*/
	d4tagSelect(pd4_SRCH, pt4_SRCH_GIP);

	
	srch_record = d4seekDouble(pd4_SRCH, gi);
	if (srch_record != 0)
	{
		return NULL; /*NULL will be returned for an empty list*/
	}
	while (srch_record == 0)/*r4success*/
	{
		/*get source for the current record*/
		srch_source = (long) f4long(pf4_SRCH_SOURCE);

		if (srch_source < 0) /*the name is unofficial and should be added to the list*/
		{
			srch_name = StrSave(f4str(pf4_SRCH_NAME));
			if (srch_name)
			{
				if(TrimSpaces(srch_name) != 0)
				{
					ErrPostEx(SEV_ERROR,0,0, "GetListofUnofficialNamesbyGI: TrimSpaces failed, possibly passed a null string");
				}
				
				if (vnp_head == NULL)/*this will be the first entry in the list*/
				{
					vnp_head = ValNodeCopyStr(NULL, 1, srch_name);
				}
				else if (vnp_head) /*this will be a subsequent entry*/
				{
					ValNodeCopyStr(&vnp_head, 1, srch_name);
				}

				srch_name = MemFree(srch_name);

			}
		}
		/*look for the next record*/
		srch_record = d4seekNextDouble(pd4_SRCH, gi);
	
	}/*while another record is found for this gi*/	

	
	return vnp_head;
}

/************************************************************/
/*															*/
/*	GetListofRedundantNamesbyGI:							*/
/*		Places a list of names for a gi into a 				*/
/*		ValNode List										*/
/*		Each name is used more than once in the same taxon  */
/*	PARAMETERS:												*/
/*		Int4 gi												*/
/*	RETURNS:												*/
/*		ValNodePtr vnp_head: head of the list				*/
/************************************************************/
ValNodePtr	GetListofRedundantNamesbyGI(Int4 gi)
{
	Int4		srch_record =	0;
	Int4		srch_namered =	0;
	CharPtr		srch_name =		NULL;
	ValNodePtr	vnp_head =		NULL;


	/*find the record corresponding to the gi number in the SRCH database*/
	d4tagSelect(pd4_SRCH, pt4_SRCH_GIP);

	
	srch_record = d4seekDouble(pd4_SRCH, gi);
	if (srch_record != 0)
	{
		return NULL; /*NULL will be returned for an empty list*/
	}
	while (srch_record == 0)/*r4success*/
	{

		/*get name redundancy for the current record*/
		srch_namered = f4int(pf4_SRCH_NAMERED);

		if (srch_namered > 1) /*the name is unofficial and should be added to the list*/
		{
			srch_name = StrSave(f4str(pf4_SRCH_NAME));
			if (srch_name)
			{
				if(TrimSpaces(srch_name) != 0)
				{
					ErrPostEx(SEV_ERROR,0,0, "GetListofRedundantNamesbyGI: TrimSpaces failed, possibly passed a null string");
				}
				
				if (vnp_head == NULL)/*this will be the first entry in the list*/
				{
					vnp_head = ValNodeCopyStr(NULL, 1, srch_name);
				}
				else if (vnp_head) /*this will be a subsequent entry*/
				{
					ValNodeCopyStr(&vnp_head, 1, srch_name);
				}

				srch_name = MemFree(srch_name);

			}
		}
		/*look for the next record*/
		srch_record = d4seekNextDouble(pd4_SRCH, gi);
	
	}/*while another record is found for this gi*/	

	
	return vnp_head;
}


/************************************************************/
/*															*/
/*	SlashDelimitedNamesToValNodeList:						*/
/*		Places a list of slash-delimited names into a 		*/
/*		ValNode List										*/
/*	PARAMETERS:												*/
/*		CharPtr name_list									*/
/*	RETURNS:												*/
/*		ValNodePtr vnp_head: head of the list				*/
/************************************************************/

ValNodePtr	SlashDelimitedNamesToValNodeList(CharPtr name_list)
{
	CharPtr		name_ptr =	NULL;
	ValNodePtr	vnp_head =	NULL;
	ValNodePtr	vnp =		NULL;


	/*divide the line by the "/" token*/
	name_ptr =  strtok(name_list, "/\n");/*line is split by either slashes or returns*/
	/*fill the head*/
	if (name_ptr)
	{
		vnp_head = ValNodeCopyStr(NULL, 1, name_ptr);
	}
	else
	{
		ErrPostEx(SEV_ERROR,0,0,"SlashDelimitedNamesToValNodeList: Invalid function params.");
		return NULL;
	}

	/*fill the rest of the list*/
	name_ptr = strtok(NULL, "/\n");
	while (name_ptr && vnp_head)
	{
		vnp = ValNodeCopyStr(&vnp_head, 1, name_ptr);
		name_ptr = strtok(NULL, "/\n");
	}

	return vnp_head;

}


/************************************************************/
/*															*/
/*	CompareInteractionPapersFor2Points:						*/
/*		Used by VnpHeapSort or any other sorting algorithm	*/
/*			.			 									*/
/*	PARAMETERS:												*/
/*															*/
/*	RETURNS:												*/
/*		-1 if vp1 result paper is better					*/
/*		1  if vp2 result paper is better					*/
/*		0  if no decision can be made						*/
/************************************************************/

int LIBCALLBACK CompareInteractionPapersFor2Results(VoidPtr vp1, VoidPtr vp2)
{
	ValNodePtr			vnp1, vnp2;
	ValNodePtr PNTR		vnpp1;
	ValNodePtr PNTR		vnpp2;
	PREBINDResultPtr	prp1 =			NULL;
	PREBINDResultPtr	prp2 =			NULL;
	int					return_value =	0;

	vnpp1 = (ValNodePtr PNTR) vp1;
	vnpp2 = (ValNodePtr PNTR) vp2;
	vnp1 = *vnpp1;
	vnp2 = *vnpp2;


	prp1 = (PREBINDResultPtr) vnp1->data.ptrvalue;
	prp2 = (PREBINDResultPtr) vnp2->data.ptrvalue;

	/* First, three cases that should not be, need to be checked for. */
	if (prp1 == NULL && prp2 != NULL)
	{
		return_value = -1;
	}
	else if (prp1 != NULL && prp2 == NULL)
	{
		return_value = 1;
	}
	else if (prp1 == NULL && prp2 == NULL)
	{
		return_value = 0;
	}


	if (prp1 != NULL && prp2 != NULL)
	{
		if (prp1->itruth != 0 || prp2->itruth != 0) /*truth*/
		{
			if (prp1->itruth > prp2->itruth)/*there is no interaction and this fact has been reviewed*/
			{
				return_value = -1;
			}
			else if (prp1->itruth < prp2->itruth)
			{
				return_value = 1;
			}
			else
			{
				return_value = 0;
			}
			
		}
		else if (prp1->opyes != 0 || prp1->opno != 0 || prp2->opyes != 0 || prp2->opno != 0)/*opno/opyes*/
		{
			if ((prp1->opyes - prp1->opno) > (prp2->opyes - prp2->opno))
			{
				return_value = -1;
			}
			else if ((prp1->opyes - prp1->opno) < (prp2->opyes - prp2->opno))
			{
				return_value = 1;
			}
			else
			{
				return_value = 0;
			}
		}
		else if (prp1->iscore != 0 || prp2->iscore != 0)/*iscore*/
		{
			if (prp1->iscore > prp2->iscore)
			{
				return_value = -1;
			}
			else if (prp1->iscore < prp2->iscore)
			{
				return_value = 1;
			}
			else
			{
				return_value = 0;
			}
		}
		else return_value = 0; /*default*/
	}

	return return_value;

}


/************************************************************/
/*															*/
/*	CompareInteractionPapersFor2Points:						*/
/*		Used by VnpHeapSort or any other sorting algorithm	*/
/*			.			 									*/
/*	PARAMETERS:												*/
/*															*/
/*	RETURNS:												*/
/*		-1 if vp1 interactor is better						*/
/*		1  if vp2 interactor is better						*/
/*		0  if no decision can be made						*/
/************************************************************/

int LIBCALLBACK CompareInteractionPapersFor2Points(VoidPtr vp1, VoidPtr vp2)
{
	ValNodePtr			vnp1, vnp2;
	ValNodePtr PNTR		vnpp1;
	ValNodePtr PNTR		vnpp2;
	PREBINDPointPtr		ppp1 =			NULL;
	PREBINDPointPtr		ppp2 =			NULL;
	PREBINDIsvmPtr		pisvmp1 =		NULL;
	PREBINDIsvmPtr		pisvmp2 =		NULL;
	int					return_value =	0;

	vnpp1 = (ValNodePtr PNTR) vp1;
	vnpp2 = (ValNodePtr PNTR) vp2;
	vnp1 = *vnpp1;
	vnp2 = *vnpp2;


	ppp1 = (PREBINDPointPtr) vnp1->data.ptrvalue;
	ppp2 = (PREBINDPointPtr) vnp2->data.ptrvalue;

	/* First, three cases that should not be, need to be checked for. */
	if (ppp1 == NULL && ppp2 != NULL)
	{
		return_value = -1;
	}
	else if (ppp1 != NULL && ppp2 == NULL)
	{
		return_value = 1;
	}
	else if (ppp1 == NULL && ppp2 == NULL)
	{
		return_value = 0;
	}
	else
	{
		/*Get descriptions of the two corresponding interaction papers from ISVM table*/
		pisvmp1 = GetIsvmbyPMID(ppp1->pmid);
		pisvmp2 = GetIsvmbyPMID(ppp2->pmid);
		/*check for NULL descriptions*/
		if (pisvmp1 == NULL && pisvmp2 != NULL)
			return_value = -1;
		else if (pisvmp1 != NULL && pisvmp2 == NULL)
			return_value = 1;
		else if (pisvmp1 == NULL && pisvmp2 == NULL)
			return_value = 0;
	}

	if (pisvmp1 != NULL && pisvmp2 != NULL)
	{
		if (pisvmp1->itruth != 0 || pisvmp2->itruth != 0) /*truth*/
		{
			if (pisvmp1->itruth > pisvmp2->itruth)/*there is no interaction and this fact has been reviewed*/
			{
				return_value = -1;
			}
			else if (pisvmp1->itruth < pisvmp2->itruth)
			{
				return_value = 1;
			}
			else
			{
				return_value = 0;
			}
			
		}
		else if (pisvmp1->opyes != 0 || pisvmp1->opno != 0 || pisvmp2->opyes != 0 || pisvmp2->opno != 0)/*opno/opyes*/
		{
			if ((pisvmp1->opyes - pisvmp1->opno) > (pisvmp2->opyes - pisvmp2->opno))
			{
				return_value = -1;
			}
			else if ((pisvmp1->opyes - pisvmp1->opno) < (pisvmp2->opyes - pisvmp2->opno))
			{
				return_value = 1;
			}
			else
			{
				return_value = 0;
			}
		}
		else if (pisvmp1->iscore != 0 || pisvmp2->iscore != 0)/*iscore*/
		{
			if (pisvmp1->iscore > pisvmp2->iscore)
			{
				return_value = -1;
			}
			else if (pisvmp1->iscore < pisvmp2->iscore)
			{
				return_value = 1;
			}
			else
			{
				return_value = 0;
			}
		}
		else return_value = 0; /*default*/
	}

	/*mem free*/
	if (pisvmp1) pisvmp1 = MemFree(pisvmp1);
	if (pisvmp2) pisvmp2 = MemFree(pisvmp2);

	return return_value;

}




/************************************************************/
/*															*/
/*	ComparePotentialInteractors:							*/
/*		Used by VnpHeapSort or any other sorting algorithm	*/
/*			.			 									*/
/*	PARAMETERS:												*/
/*															*/
/*	RETURNS:												*/
/*		-1 if vp1 interactor is better						*/
/*		1  if vp2 interactor is better						*/
/*		0  if no decision can be made						*/
/************************************************************/

int LIBCALLBACK ComparePotentialInteractors(VoidPtr vp1, VoidPtr vp2)
{
	ValNodePtr vnp1, vnp2;
	ValNodePtr PNTR vnpp1;
	ValNodePtr PNTR vnpp2;
	PREBINDInteractorPtr pip1 =	NULL;
	PREBINDInteractorPtr pip2 =	NULL;


	vnpp1 = (ValNodePtr PNTR) vp1;
	vnpp2 = (ValNodePtr PNTR) vp2;
	vnp1 = *vnpp1;
	vnp2 = *vnpp2;

	pip1 = (PREBINDInteractorPtr) vnp1->data.ptrvalue;
	pip2 = (PREBINDInteractorPtr) vnp2->data.ptrvalue;

	/* First, three cases that should not be, need to be checked for. */
	if (pip1 == NULL && pip2 != NULL)
		return -1;
	if (pip1 != NULL && pip2 == NULL)
		return 1;
	if (pip1 == NULL && pip2 == NULL)
		return 0;

	if (pip1->pitruth != 0 || pip2->pitruth != 0)/*pitruth*/
	{
		if (pip1->pitruth > pip2->pitruth)
		{
			return -1;
		}
		else if (pip1->pitruth < pip2->pitruth)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	else if (pip1->opyes != 0 || pip1->opno != 0 || pip2->opyes != 0 || pip2->opno != 0)/*opno/opyes*/
	{
		if ((pip1->opyes - pip1->opno) > (pip2->opyes - pip2->opno))
		{
			return -1;
		}
		else if ((pip1->opyes - pip1->opno) < (pip2->opyes - pip2->opno))
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	else if (pip1->piscore != 0 || pip2->piscore != 0)/*piscore*/
	{
		if (pip1->piscore > pip2->piscore)
		{
			return -1;
		}
		else if (pip1->piscore < pip2->piscore)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	else if (pip1->piscore2 != 0 || pip2->piscore2 != 0)/*piscore2*/
	{
		if (pip1->piscore2 > pip2->piscore2)
		{
			return -1;
		}
		else if (pip1->piscore2 < pip2->piscore2)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	else if (pip1->num_pmids != 0 || pip2->num_pmids != 0)/*num_pmids*/
	{
		if (pip1->num_pmids > pip2->num_pmids)
		{
			return -1;
		}
		else if (pip1->num_pmids < pip2->num_pmids)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	else
	{
		return 0;
	}

}
		



/************************************************************/
/*															*/
/*	DescribeInteractionPaperLikelihood:						*/
/*			Uses a ISVM table record to compose a			*/
/*			description of the likelihood that a paper		*/
/*			describes an interaction						*/
/*	PARAMETERS:												*/
/*			PREBINDIsvmPtr pisvmp: pointer to a ISVM record	*/
/*	RETURNS:												*/
/*			CharPtr	paper_descr:description					*/
/************************************************************/
CharPtr	DescribeInteractionPaperLikelihood(PREBINDIsvmPtr pisvmp)
{
	
	CharPtr paper_descr =	NULL;

	if (!pisvmp)
	{
		paper_descr = StrSave("Unknown");
		ErrPostEx(SEV_ERROR,0,0,"DescribeInteractionPaperLikelihood: Invalid function params.");
	}
	else if (pisvmp->itruth != 0) /*truth*/
	{
		if (pisvmp->itruth == -1)/*there is no interaction and this fact has been reviewed*/
		{
			paper_descr = StrSave("No: Reviewed.");
		}
		if (pisvmp->itruth == 1)/*there is an interaction and this fact has been reviewed*/
		{
			paper_descr = StrSave("Yes: Reviewed.");
		}
	}
	else if (pisvmp->opyes != 0 || pisvmp->opno != 0)/*opinion*/
	{
		if (pisvmp->opyes >0 && pisvmp->opno == 0)
		{
			paper_descr = StrSave("Yes");
		}
		else if (pisvmp->opno >0 && pisvmp->opyes == 0)
		{
			paper_descr = StrSave("No");
		}
		else if (pisvmp->opno >0 && pisvmp->opyes > 0)
		{
			paper_descr = StrSave("Unknown");
		}
	}
	else if (pisvmp->iscore != 0 || pisvmp->iscore2 != 0)/*scores*/
	{	
		if (pisvmp->iscore > 1 ) /*highly possible*/
		{
			paper_descr = StrSave("Highly possible");
		}
		else if (pisvmp->iscore > 0 ) /*possible*/
		{
			paper_descr = StrSave("Possible");
		}
		else if (pisvmp->iscore > -1) /*unlikely*/
		{
			paper_descr = StrSave("Unlikely");
		}
		else if (pisvmp->iscore < -1) /*highly unlikely*/
		{
			paper_descr = StrSave("Highly unlikely");
		}
	}
	else 
	{
		paper_descr = StrSave("Unknown");	
	}
				
	return(paper_descr);
}

/************************************************************/
/*															*/
/*	DescribeInteractorLikelihood:							*/
/*			Uses a PreBIND Interactor object to compose a	*/
/*			description of the potential interactors		*/
/*			likelihood.									 	*/
/*	PARAMETERS:												*/
/*			PREBINDInteractorPtr pip:						*/
/*			pointer to a PreBIND Interactor Object			*/
/*	RETURNS:												*/
/*			CharPtr	int_descr:	description					*/
/************************************************************/
CharPtr	DescribeInteractorLikelihood(PREBINDInteractorPtr pip)
{

	/*See definition of PreBIND-Interactor.
	This structure is generated by GetListofInteractorsforGI
	and is a composite picture of assessments for all the papers 
	that talk about this given interaction pair.
	*/

	
	CharPtr int_descr =	NULL;

	if (!pip)
	{
		int_descr = StrSave("Unknown");
		ErrPostEx(SEV_ERROR,0,0,"DescribeInteractionLikelihood: Invalid function params.");
	}	
	else if (pip->pitruth != 0) /*truth*/
	{
		if (pip->pitruth == -1)/*there is no interaction and this fact has been reviewed*/
		{
			int_descr = StrSave("No: reviewed");
		}
		if (pip->pitruth == 1)/*there is an interaction and this fact has been reviewed*/
		{
			int_descr = StrSave("Yes: reviewed");
		}
	}
	else if (pip->opyes != 0 || pip->opno != 0)/*opinion*/
	{
		if (pip->opyes >0 && pip->opno == 0)
		{
			int_descr = StrSave("Yes");
		}
		else if (pip->opno >0 && pip->opyes == 0)
		{
			int_descr = StrSave("No");
		}
		else if (pip->opno >0 && pip->opyes > 0)
		{
			int_descr = StrSave("Unknown");
		}
	}
	else if (pip->piscore > 0 || pip->piscore2 > 0)/*scores*/
	{	
		if (pip->piscore > 0 && pip->piscore2 == 0) /*it may be a protein::protein interaction*/
		{
			int_descr = StrSave("Probably");
		}
		else if (pip->piscore2 > 0 && pip->piscore == 0) /*it may be a protein::DNA interaction*/
		{
			int_descr = StrSave("Probably<br>(protein-DNA)");
		}
		else if (pip->piscore > 0 && pip->piscore2 > 0) /*both ? */
		{
			int_descr = StrSave("Probably<br>(protein or DNA?)");
		}
	}
	else 
	{
		int_descr = StrSave("Unknown");	
	}
				
	return(int_descr);
}


/************************************************************/
/*															*/
/*	DescribeInteractionLikelihood:							*/
/*			Uses a POINT table record to compose a			*/
/*			description of the potential interactions		*/
/*			likelihood.									 	*/
/*	PARAMETERS:												*/
/*			PREBINDPointPtr ppp: pointer to a POINT record	*/
/*	RETURNS:												*/
/*			CharPtr	int_descr:	description					*/
/************************************************************/
CharPtr	DescribeInteractionLikelihood(PREBINDPointPtr ppp)
{
	
	CharPtr int_descr =	NULL;

	if (!ppp)
	{
		int_descr = StrSave("Unknown");
		ErrPostEx(SEV_ERROR,0,0,"DescribeInteractionLikelihood: Invalid function params.");
	}	
	else if (ppp->pitruth != 0) /*truth*/
	{
		if (ppp->pitruth == -1)/*there is no interaction and this fact has been reviewed*/
		{
			int_descr = StrSave("No: reviewed");
		}
		if (ppp->pitruth == 1)/*there is an interaction and this fact has been reviewed*/
		{
			int_descr = StrSave("Yes: reviewed");
		}
	}
	else if (ppp->opyes != 0 || ppp->opno != 0)/*opinion*/
	{
		if (ppp->opyes >0 && ppp->opno == 0)
		{
			int_descr = StrSave("Yes");
		}
		else if (ppp->opno >0 && ppp->opyes == 0)
		{
			int_descr = StrSave("No");
		}
		else if (ppp->opno >0 && ppp->opyes > 0)
		{
			int_descr = StrSave("Unknown");
		}
	}
	else if (ppp->piscore > 0 || ppp->piscore2 > 0)/*scores*/
	{	
		if (ppp->piscore > 0 && ppp->piscore2 == 0) /*it may be a protein::protein interaction*/
		{
			int_descr = StrSave("Probably");
		}
		else if (ppp->piscore2 > 0 && ppp->piscore == 0) /*it may be a protein::DNA interaction*/
		{
			int_descr = StrSave("Probably<br>(protein-DNA)");
		}
		else if (ppp->piscore > 0 && ppp->piscore2 > 0) /*both ? */
		{
			int_descr = StrSave("Probably<br>(protein or DNA?)");
		}
	}
	else 
	{
		int_descr = StrSave("Unknown");	
	}
				
	return(int_descr);
}



/************************************************************/
/*															*/
/*	SubmitToExistingBINDRecord:								*/
/*		Submit an interaction paper to existing BIND		*/
/*		database											*/
/*	PARAMETERS:												*/
/*		see below 											*/
/*	RETURNS:												*/
/*			Int4	subid: submit id of record submitted 	*/
/*					< 0 : error encountered in submitting	*/
/************************************************************/
Int4 SubmitToExistingBINDRecord(Int4 pmid, Int4 sgia, Int4 sgib, Int4 gia, Int4 gib, CharPtr fname, CharPtr lname, Int4 buid, Int4 iid)
{
	Int4					subid =				0;
	Int4                    rec_gia=            0;
	Int4                    rec_gib=            0;
	BINDIdPtr				bida =				NULL;
	BINDIdPtr               bidb =              NULL;
	BINDObjectTypeIdPtr		id_a =				NULL;
	BINDObjectTypeIdPtr     id_b =              NULL;
	BINDObjectPtr			bopa =				NULL;
	BINDObjectPtr			bopb =				NULL;
	CharPtr					title =				NULL;
	CharPtr					pub_title =			NULL;
	BINDPubObjectPtr		bpop =				NULL;
	BINDInteractionPtr		bip =				NULL;
	Char					post_submit_script_url[PATH_MAX];
	

	OpenBIND(TRUE);	


	/*verify user and register that the user owns this id*/
	if(!CI_VerifyContactInfoRecord(buid, fname, lname))
	{
		ErrPostEx(SEV_ERROR,0,0,"SubmitToExistingBINDRecord: Incorrect user name and id.");
		return -2;
	}

	/*verify if this buid actually owns this BIND record*/
	if (!CI_VerifyRecordOwnership(buid, iid, IDIID)){
		ErrPostEx(SEV_ERROR,0,0,"SubmitToExistingBINDRecord: Invalid record ownership.");
		return -3;
	}

	/*extract interaction record(iid) from BIND database*/
	if ((bip=DB_GetInteractionRec(iid))==NULL){
		ErrPostEx(SEV_ERROR,0,0,"SubmitToExistingBINDRecord: cannot extract BIND record.");
		return -4;
	}
	
    /*check if this record is talking about the same molecules a and b as the sumbitted ones*/
	if (((bopa=bip->a)==NULL) || ((bopb=bip->b)==NULL)){
		ErrPostEx(SEV_ERROR,0,0,"SubmitToExistingBINDRecord: no molecule a or b.");
		return -5;
	}

	if (((id_a=bopa->id)==NULL) || ((id_b=bopb->id)==NULL)){
		ErrPostEx(SEV_ERROR,0,0,"SubmitToExistingBINDRecord: molecule a or b id not exist.");
		return -5;
	}
	bida=id_a->data.ptrvalue;
	bidb=id_b->data.ptrvalue;
	rec_gia=bida->gi;
	rec_gib=bidb->gi;

	/*if sgia or sgib not match record molecules a and b*/
	/*or if submit a and b have the same gi*/
	if( ( (rec_gia!=sgia)&&(rec_gia!=sgib) || (rec_gib!=sgib)&&(rec_gib!=sgia) ) || 
		( (sgia==rec_gia)&&(sgia==rec_gib) || (sgib==rec_gia)&&(sgib==rec_gib) )  ){
		ErrPostEx(SEV_ERROR,0,0,"SubmitToExistingBINDRecord: a or b in submit and record are not match");
		return -5;
	}


	/*construct a BIND publication object and return a pointer to it*/
	/*title = GetTitleFromEntrezbyPMID(pmid);**************this function not written yet************/
	pub_title = "Title not available";
	bpop = C_BINDPubObj_New(pub_title, BIND_pub_object_opinion_support, C_Pub_New(pmid));
	if (bpop == NULL)
	{
		ErrPostEx(SEV_ERROR,0,0,"Submit to BIND: Failed to create publication object.");
		return -6;
	}

	/*check if this pmid is already in the record, if not there, add this pmid to the record, else error message and return.*/
	if (Trav_BINDPubObj_Get(bip->source, bpop->pub->data.intvalue)==NULL){
		C_Interaction_AddPubObj(bip, bpop);

	}
	else {
		ErrPostEx(SEV_ERROR,0,0,"SubmitToExistingBINDRecord: pmid already exists in the record.");
		bpop=BINDPubObjectFree(bpop);
		return -7;
	}
	
	/*write the interaction to the database, replace the existing BIND record with update one */
	if (DB_ReplaceInteractionRec(iid, bip) != 0)
	{
		ErrPostEx(SEV_ERROR,0,0,"Submit to BIND: Failed to write interaction to database.");
		return -8;
	}

	/*trigger the script to inform the BIND curation team-- only on production server*/
	post_submit_script_url[0] = '\0';
	GetAppParam("prebind", "prebind_cgi", "PostSubmitLoc", "0", post_submit_script_url, (size_t)PATH_MAX * (sizeof(Char)));
	if (!post_submit_script_url) 
	{
		ErrPostEx(SEV_ERROR,0,0,"Submit to BIND: Administrator forgot to set valid url for the post submit script in prebind config file (see PostSubmitLoc under [prebind_cgi])\n.");
	}
	fprintf (stdout, "<SCRIPT SRC=\"%s?id=%ld&stype=Interaction&fname=%s&lname=%s&uid=%ld&change=0\"></SCRIPT>\n", post_submit_script_url, (long) subid, fname, lname, (long) buid);

	/*Free Memory*/
	BINDInteractionFree(bip);
	CloseBIND();

	return iid;

}



/************************************************************/
/*															*/
/*	SubmitToBIND:											*/
/*		Submit a potential interaction to a BIND			*/
/*		database											*/
/*	PARAMETERS:												*/
/*		see below 											*/
/*	RETURNS:												*/
/*			Int4	subid: submit id of record submitted 	*/
/*					< 0 : error encountered in submitting	*/
/************************************************************/
Int4 SubmitToBIND(Int4 pmid, Int4 gia, Int4 gib, Int4 sgia, Int4 sgib, CharPtr nama, CharPtr namb, CharPtr adesc, CharPtr bdesc, Int4 atype, Int4 btype, Int4 aok, Int4 bok, Int4 afrom, Int4 bfrom, Int4 ato, Int4 bto, Int4 opin, Int4 method, CharPtr fname, CharPtr lname, Int4 buid)
{
	Int4					subid =				0;
	BINDIdPtr				bidp =				NULL;
	BINDObjectTypeIdPtr		id =				NULL;
	BINDObjectPtr			bopa =				NULL;
	BINDObjectPtr			bopb =				NULL;
	Int4					taxid =				0;
	CharPtr					taxname=			NULL;
	CharPtr					message=			NULL;
	OrgRefPtr				orp =				NULL;
	BioSourcePtr			bsp =				NULL;
	BINDObjectOriginPtr		origin=				NULL;
	Int4					size_to_allocate =	0;
	CharPtr					title =				NULL;
	ValNodePtr				pubhead=			NULL;
	CharPtr					pub_title =			NULL;
	BINDPubObjectPtr		bopp =				NULL;
	BINDInteractionPtr		bip =				NULL;
	Char					post_submit_script_url[PATH_MAX];
	

	OpenBIND(TRUE);	

	/*get a new unique subid*/
	Cfg_IncMaxAccessionNum("IID");
	subid = Cfg_GetMaxAccessionNumber("IID");
	if(subid == 0)
	{
		ErrPostEx(SEV_ERROR,0,0,"Submit to BIND: Cfg_GetMaxAccessionNumber failed.");
		return -2;
	}

	/*verify user and register that the user owns this id*/
	if(CI_VerifyContactInfoRecord(buid, fname, lname)==FALSE)
	{
		ErrPostEx(SEV_ERROR,0,0,"Submit to BIND: Incorrect user name and id.");
		return -3;
	}
	else
	{
		if (DB_AddAccNum2ContactInfoRecord(buid, subid, IDIID) != 0)
		{
			ErrPostEx(SEV_ERROR,0,0,"Submit to BIND: DB_AddAccNum2ContactInfoRecord failed.");
			return -4;
		}
		else
		{
			/*ready to construct a BIND record*/
		}
		
	}
	
	/*STEP 1: constuct a molecular object 'a' and return a pointer to it*/
	bopa = ConstructAMolecularObject(nama, adesc, atype, sgia);
	if (bopa == NULL)
	{
		ErrPostEx(SEV_ERROR,0,0,"Submit to BIND: Failed to create object A.");
		return -5;
	}

	/*STEP 2: constuct a molecular object 'b' and return a pointer to it*/
	bopb = ConstructAMolecularObject(namb, bdesc, btype, sgib);
	if (bopb == NULL)
	{
		ErrPostEx(SEV_ERROR,0,0,"Submit to BIND: Failed to create object B.");
		return -6;
	}

	/*STEP3: construct a BIND publication object and return a pointer to it*/
	/*title = GetTitleFromEntrezbyPMID(pmid);**************this function not written yet************/
	pub_title = "Title not available";
	bopp = C_BINDPubObj_New(pub_title, BIND_pub_object_opinion_support, C_Pub_New(pmid));
	if (bopp == NULL)
	{
		ErrPostEx(SEV_ERROR,0,0,"Submit to BIND: Failed to create publication object.");
		return -7;
	}

	/*STEP 4: construct a new interaction record object and return a pointer to it*/
	bip = C_Interaction_New(subid, bopa, bopb, bopp, FALSE);
	if (bip == NULL)
	{
		ErrPostEx(SEV_ERROR,0,0,"Submit to BIND: Failed to create interaction object.");
		return -8;
	}

	/*add a description text to an interaction object*/
	size_to_allocate = sizeof(Char)*strlen(nama) + sizeof(Char)*strlen(namb) + sizeof(Char)*40;
	title = (CharPtr)malloc(size_to_allocate);
	if (title != NULL)
	{
		StrCpy(title, nama);
		StrCat(title, " interacts with ");
		StrCat(title, namb);	
		C_Interaction_AddSimpleDescr(bip, title);
	}

	

	/*STEP 5: write the interaction to the database*/
	if (DB_WriteInteractionRec(bip) != 0)
	{
		ErrPostEx(SEV_ERROR,0,0,"Submit to BIND: Failed to write interaction to database.");
		return -9;	
	}


	/*STEP 6: trigger the script to inform the BIND curation team-- only on production server*/
	post_submit_script_url[0] = '\0';
	GetAppParam("prebind", "prebind_cgi", "PostSubmitLoc", "0", post_submit_script_url, (size_t)PATH_MAX * (sizeof(Char)));
	if (!post_submit_script_url) 
	{
		ErrPostEx(SEV_ERROR,0,0,"Submit to BIND: Administrator forgot to set valid url for the post submit script in prebind config file (see PostSubmitLoc under [prebind_cgi])\n.");
	}
	fprintf (stdout, "<SCRIPT SRC=\"%s?id=%ld&stype=Interaction&fname=%s&lname=%s&uid=%ld&change=0\"></SCRIPT>\n", post_submit_script_url, (long) subid, fname, lname, (long) buid);

	/*Free Memory*/
	BINDInteractionFree(bip);

	CloseBIND();

	return subid;

}

/***********************************************************/
/*														   */
/* link to view:                                           */
/*      a link to view a BIND record                       */
/* PARAMETERS: all needed for a link infomation            */
/*                                                         */
/***********************************************************/
void LinkToView(Int4 submit_id)
{	   
	Char	dataget_location[PATH_MAX];
    dataget_location[0] = '\0';
    GetAppParam("prebind", "prebind_cgi", "dataget_location", NULL, dataget_location, PATH_MAX*sizeof(Char));
    fprintf (stdout, "<tr><td><br>In the mean time, you can ");
	fprintf (stdout, "<a href=\"javascript:void window.open('%s?get=search&rectype=4&type=int&id=%ld', 'giWin', 'toolbar=no,location=no,directories=no,status=yes,menubar=yes,scrollbars=yes,resizable=yes,width=550,height=400');\" onmouseover=\"window.status='View the generated BIND record';return(true);\" onmouseout=\"window.status=''\">view</a>\n", dataget_location, (long) submit_id);
}

                     
/***********************************************************/
/*														   */
/* link to edit:                                           */
/*      a link to edit a BIND record                       */
/* PARAMETERS: all needed for a link infomation            */
/*                                                         */
/***********************************************************/
void LinkToEdit(CharPtr fname, CharPtr lname, Int4 buid, Int4 submit_id)
{
		Char	dataentry_location[PATH_MAX];

		dataentry_location[0] = '\0';
		fprintf (stdout, " or ");
        GetAppParam("prebind", "prebind_cgi", "dataentry_location", NULL, dataentry_location, PATH_MAX*sizeof(Char));
        fprintf (stdout, "<a href=\"javascript:void window.open('%s?form=new_interaction&rectype=4&update=1&change=1&fname=%s&lname=%s&userid=%ld&subid=%ld', 'giWin', 'toolbar=no,location=no,directories=no,status=yes,menubar=yes,scrollbars=yes,resizable=yes,width=550,height=400');\" onmouseover=\"window.status='Edit the generated BIND record';return(true);\" onmouseout=\"window.status=''\">edit</a>\n",dataentry_location, fname, lname, buid, submit_id );
}


/************************************************************/
/*															*/
/*	Submit:													*/
/*		Submit a potential interaction to the SUBMIT		*/
/*		database											*/
/*	PARAMETERS:												*/
/*		see below 											*/
/*	RETURNS:												*/
/*			Int2	0: ok									*/
/************************************************************/
Int2	Submit(Int4 pmid, Int4 gia, Int4 gib, Int4 sgia, Int4 sgib, CharPtr nama, CharPtr namb, CharPtr adesc, CharPtr bdesc, Int4 atype, Int4 btype, Int4 aok, Int4 bok, Int4 afrom, Int4 bfrom, Int4 ato, Int4 bto, Int4 opin, Int4 method, CharPtr fname, CharPtr lname, Int4 buid, CharPtr piid)
{


	Char	todays_date[8];
	Int4	submit_id =		0;
	Int4    iid = 0;
	Int2	locus_record =	0;
	Int2	shoundcheck_A = 0;
	Int2	shoundcheck_B = 0;
	Char	anon_user_id_ptr[PATH_MAX];
	Int4	anon_user_id =  0;
	Char	genetic_user_id_ptr[PATH_MAX];
	Int4	genetic_user_id =  0;
	Int2	report =		0;
	Int2	append_status = 0;
	Boolean error =			FALSE;
	Boolean change =        FALSE; /*a flag indicate that if changed an existing BIND record*/
	Boolean exist  =        FALSE; /*a flag indicate that if a publication is already exist*/
	Boolean valid_user =    TRUE;
	Boolean valid_iid =     TRUE;
	PREBINDAccPtr pap =		NULL;


	/*get todays date*/
	date4today(todays_date);

	/*start print out of response to receiving record*/
	fprintf (stdout, "Content-type:text/html\r\n\r\n");
	WWWPrintFileData(REVIEWPOINTHEADER, stdout);
	fprintf (stdout, "<body bgcolor=\"white\">\n");
	fprintf (stdout,"<table width=100%% border=0 cellspacing=2 cellpadding=2 cols=4>\n");
	fprintf (stdout,"<tr bgcolor=\"lightsteelblue\">\n");
	fprintf (stdout,"<td colspan=4 height=\"20\">\n");
	fprintf (stdout,"<h1>Submission results</h1>\n");
	fprintf (stdout,"</td>\n");
	fprintf (stdout,"</tr>\n");
	fprintf (stdout,"<tr><td><br>\n");

	/*check for errors*/
	if (opin == 0) 
	{
		fprintf (stdout,"<tr><td>You must indicate if an interaction is described or not.<br>\n");
		error = TRUE;
	}
	if (StrCmp(nama, "no_nama_entered") == 0) 
	{
		fprintf (stdout,"<tr><td>You must enter a short label for molecule A.<br>\n");
		error = TRUE;
	}
	if (StrCmp(namb, "no_namb_entered") == 0) 
	{
		fprintf (stdout,"<tr><td>You must enter a short label for molecule B.<br>\n");
		error = TRUE;
	}
	if ((StrCmp(fname, "Anonymous") != 0) &&  (StrCmp(lname, "User") != 0) && buid == 0)
	{
		fprintf (stdout,"<tr><td>You must enter a valid BIND User ID.<br>\n");
		error = TRUE;
	}

	/*user enter an invalid iid*/
	iid = atoi(piid);
	if (iid == 0) { valid_iid = FALSE; }                              /*iid entered as string*/
	else if (iid == -99999) {iid = 0;}                                /*iid not entered, treat iid =0*/
	else if (iid <0 && iid != -99999) {valid_iid = FALSE;}			  /*iid entered as a negative number*/
	
	if (valid_iid == FALSE){
		fprintf (stdout, "<tr><td>The BIND record id you entered is not valid.<br>\n");
		error = TRUE;
	}

	/*detect errors and quit */
	if (error == TRUE){
		fprintf (stdout,"<tr><td><br>Use the back button in your browser to return to the form and correct any mistakes listed above.<br>\n");
		fprintf (stdout,"</table>\n");
		fflush(stdout);
		goto Exit;        
	}

	/*determine the submitted gi for A*/
	if (sgia == 0 && atype != 0)
	{
		if (atype == 2)
		{
			sgia = gia;
		}
		else if (atype == 1)
		{
			/*get source DNA info for A from the LOCUS database*/
			d4tagSelect(pd4_LOCUS, pt4_LOCUS_GIP);
			locus_record = d4seekDouble(pd4_LOCUS, gia);
			if (locus_record == 0)
			{
				sgia = f4long(pf4_LOCUS_GIN);
			}
		}
	}
	if (sgia == 0 || atype == 0) 
	{
		fprintf (stdout,"<tr><td>You must specify a molecule type for A (protein or DNA).  You can also specify the gi of A if the listed gi is not correct.<br>\n");
		error = TRUE;
	}

	/*determine the submitted gi for B*/
	if (sgib == 0 && btype != 0)
	{
		if (btype == 2)
		{
			sgib = gib;
		}
		else if (btype == 1)
		{
			/*get source DNA info for A from the LOCUS database*/
			d4tagSelect(pd4_LOCUS, pt4_LOCUS_GIP);
			locus_record = d4seekDouble(pd4_LOCUS, gib);
			if (locus_record == 0)
			{
				sgib = f4long(pf4_LOCUS_GIN);
			}
		}
	}
	if (sgib == 0 || btype == 0) 
	{
		fprintf (stdout,"<tr><td>You must specify a molecule type for B (protein or DNA).  You can also specify the gi of B if the listed gi is not correct.<br>\n");
		error = TRUE;
	}
	
	/*check for submit errors using SeqHound*/
	if (sgia != 0 && sgib != 0)
	{
		shoundcheck_A = CheckForSubmitErrorsUsingSeqHound(sgia, atype);
		shoundcheck_B = CheckForSubmitErrorsUsingSeqHound(sgib, btype);
		if ((shoundcheck_A == -1) || (shoundcheck_B == -1))
		{
			error = TRUE;
		}
		if ((shoundcheck_A == 0) || (shoundcheck_B == 0))
		{
			printf("<h4>SeqHound was not available to check the submission.</h4>\n");
		}
	}

	/*get anonymous user id if user id was not submitted*/
	if (buid == 0) 
	{
		anon_user_id_ptr[0] = '\0';
		GetAppParam("prebind", "prebind_cgi", "anon_user_id", "0", anon_user_id_ptr, (size_t)PATH_MAX * (sizeof(Char)));
		if (anon_user_id_ptr) 
		{
			anon_user_id = atoi(anon_user_id_ptr);
			buid = anon_user_id;
		}

	}

	/*get genetic user id so you can specifically recognize this submitter*/
	genetic_user_id_ptr[0] = '\0';
	GetAppParam("prebind", "prebind_cgi", "genetic_user_id", "0", genetic_user_id_ptr, (size_t)PATH_MAX * (sizeof(Char)));
	if (genetic_user_id_ptr) genetic_user_id = atoi(genetic_user_id_ptr);



	/*try to submit the interaction record to a Local BIND database*/
	if (opin == 1 && sgia != 0 && atype != 0 && sgib != 0 && btype != 0 && buid != genetic_user_id)
	{ 
		if (iid == 0){
			/*if user leave iid empty, ie. want to create a new record*/
			submit_id = SubmitToBIND(pmid, gia, gib, sgia, sgib, nama, namb, adesc, bdesc, atype, btype, aok, bok, afrom, bfrom, ato, bto, opin, method, fname, lname, buid);
			if (submit_id <= 0)
			{
				ErrPostEx(SEV_ERROR,0,0, "Submit: SubmitToBIND failed.  Returned value = %d\n", submit_id);
				if (submit_id == -3)
				{
					fprintf (stdout,"<tr><td>The user name \"%s %s\" and the BIND user id you entered are not valid.  Check your user name and id.<br>\n", fname, lname);
				}
				else
				{
					fprintf (stdout,"<tr><td>The BIND record could not be made. Contact the site administrator. Error code = %ld<br>\n", (long) submit_id);
				}
				error = TRUE;
			}
		}

		/*else user enter a valid existing BIND record, do SubmitToExistingBINDRecord*/
		else{
			/*if not an anonymous user to modify a record*/
			if (buid != anon_user_id) {
				submit_id = SubmitToExistingBINDRecord(pmid, sgia, sgib, gia, gib, fname, lname, buid, iid);
				if (submit_id <=0){
					if (submit_id==-2){
						fprintf (stdout,"<tr><td>The user name \"%s %s\" and the BIND user id you entered are not valid.  Check your user name and id.<br>\n", fname, lname);
						error = TRUE;
					}
					else if (submit_id==-3){
						fprintf(stdout, "You do not own the record or the submitted BIND ID does not exist.");
						error = TRUE;
					}
					else if (submit_id==-5) {
						/*link to view*/
						fprintf (stdout, "<tr><td><br>You submitted two sequences with the GenInfo identifiers : %ld and %ld.", sgia, sgib);
						fprintf(stdout, "These molecules do not match those found in BIND record %d. No changes were made to the record.", iid);
						LinkToView(iid);
						fprintf (stdout, " the molecules listed in this BIND record.\n<p>");
						fprintf (stdout,"<tr><td><br>\n");
						error = TRUE;
					}
					else if (submit_id== -7) {
						fprintf(stdout, "Publication: pmid %d is already present in the BIND interaction record (bid: %d). This record was not changed.", pmid, iid);
                        exist = TRUE;
                    }
					else
					{
						fprintf (stdout,"<tr><td>The BIND record could not be made. Contact the site administrator. Error code = %ld<br>\n", (long) submit_id);
					}
				}
				/*return value is an existing BIND id, either pmid there or has added a new pmid*/
				else {
					fprintf(stdout, "Thank you. pmid %d has been added to this record.\n", pmid);
					change = TRUE; /*successfully changed the record, add publication to the exist record*/
				}
			}
			else {
				fprintf(stdout, "Sorry, anonymous users cannot modify BIND records.\n");
				error = TRUE;
			}
		}
	}

	
	/*report errors and quit*/
	if (error == TRUE)
	{	
		fprintf (stdout,"<tr><td><br>Use the back button in your browser to return to the form and correct any mistakes listed above.<br>\n");
		fprintf (stdout,"</table>\n");
		fflush(stdout);
		goto Exit;
	
	}

	/*append the record to the Submit datafile*/
	d4appendStart(pd4_SUBMIT, 0);
	d4blank(pd4_SUBMIT);
		
	f4assignLong(pf4_SUBMIT_PMID,	pmid );
	f4assignLong(pf4_SUBMIT_MUID,	0);	
	f4assignLong(pf4_SUBMIT_GIA,	gia );	
	f4assignLong(pf4_SUBMIT_GIB,	gib );
	f4assignLong(pf4_SUBMIT_SGIA,	sgia );
	f4assign    (pf4_SUBMIT_NAMA,	nama );
	f4memoAssign(pf4_SUBMIT_ADESC,	adesc );
	f4assignLong(pf4_SUBMIT_ATYPE,	atype );	
	f4assignLong(pf4_SUBMIT_AOK,	aok );
	f4assignLong(pf4_SUBMIT_AFROM,	afrom );
	f4assignLong(pf4_SUBMIT_ATO,	ato );	
	f4assignLong(pf4_SUBMIT_SGIB,	sgib );
	f4assign    (pf4_SUBMIT_NAMB,	namb );
	f4memoAssign(pf4_SUBMIT_BDESC,	bdesc );	
	f4assignLong(pf4_SUBMIT_BTYPE,	btype );	
	f4assignLong(pf4_SUBMIT_BOK,	bok );
	f4assignLong(pf4_SUBMIT_BFROM,	bfrom );	
	f4assignLong(pf4_SUBMIT_BTO,	bto );
	f4assignLong(pf4_SUBMIT_METHOD,	method );
	f4assignLong(pf4_SUBMIT_OPIN,	opin );	
	f4assign    (pf4_SUBMIT_FNAME,	fname );	
	f4assign    (pf4_SUBMIT_LNAME,	lname );	
	f4assignLong(pf4_SUBMIT_BUID,	buid );	
	f4assign    (pf4_SUBMIT_DATE,	todays_date);	
	f4assignLong(pf4_SUBMIT_STATUS, submit_id );
	
	append_status = d4append(pd4_SUBMIT);
	
	/*record opinion in POINT database*/
	if (append_status == r4success)
	{
		if ((report = RecordPointOpinion(pmid, sgia, sgib, opin)) != 0)
		{
			ErrPostEx(SEV_ERROR,0,0, "RecordPointOpinion failed.   returned value = %d\n", report);
		}
	}
	
	if (append_status == r4success)
	{
		if (submit_id > 0 )
		{
			/*a new record was created*/
			if (change==FALSE){
				fprintf (stdout,"<tr>\n");
				fprintf (stdout,"<td colspan=4 height=\"20\">\n");
				fprintf (stdout,"<br>\n");
				fprintf (stdout,"<b>Thank you.  Your entry was received.</b><br><p>\n");
				fprintf (stdout, "A BIND record was generated and submitted to a local BIND database for you.  This record will have the submit id of %ld.  ", submit_id);
			
				if (buid != anon_user_id && buid != genetic_user_id)
				{
					fprintf (stdout,"You will be emailed once your entry has been reviewed by curators and forwarded to the public BIND database.\n<p>");
				}
				else if (buid == anon_user_id)
				{
					fprintf (stdout,"A curator will review this record and submit it to BIND for you. Since your record was submitted as an anonymous user, you will not be able to edit this record further.\n<p>");
				}
			}
		}
			
		if (submit_id > 0 || exist == TRUE)
		{
			if (exist == TRUE){
				LinkToView(iid);
			}
			else {
				LinkToView(submit_id);
			}
			/*only allow not anonymous user to edit a bind record*/
			if (buid != anon_user_id && exist != TRUE){
				LinkToEdit(fname, lname, buid, submit_id);
			}
			if (exist == TRUE){
                fprintf (stdout, " this BIND record.\n<p>");
            }
            else if (change == TRUE){
                fprintf (stdout, " the updated BIND record.\n<p>");
            }
            else{
                fprintf (stdout, " the generated BIND record.\n<p>");
            }
            fprintf (stdout,"<tr><td><br>\n");
        }

		
		if (submit_id <= 0)
		{	
			if (opin == -1){
				fprintf(stdout, "You have indicated that an interaction between %s and %s does not exist.", nama, namb);
			}
			if (buid == genetic_user_id)
			{
				fprintf (stdout,"You have used a user id that is reserved for entering genetic interactions.");
			}	
			fprintf (stdout,"<tr><td><br>\n");
			if (exist != TRUE) {
				fprintf (stdout,"Your information was saved but no BIND record was made.\n<p>");
			}
		}

		
		/*Choose an option*/
		fprintf (stdout, "<br>You may choose one of the following options.<br>\n");

		/*print the link to continue reviewing other interactions in this paper*/
		fprintf(stdout,"<tr>\n");
		fprintf(stdout,"<td  colspan=\"1\" height=\"20\">\n");	
		fprintf (stdout, "<form method=\"POST\" action=\"%s\" target=\"display\"> \n", cgi_path);	
		fprintf (stdout, "<input type=\"submit\" value=\"GO\">\n");
		fprintf (stdout, "<input type=hidden name=pmid value=\"%ld\">\n", (long) pmid);
		fprintf (stdout, "<input type=hidden name=gia value=\"%ld\">\n", (long) gia);
		fprintf (stdout, "<input type=hidden name=gib value=\"%ld\">\n", (long) gib);
		fprintf (stdout, "<input type=hidden name=frames value=\"0\">\n");
		fprintf (stdout, "<input type=hidden name=search_type value=\"points\">\n");
		fprintf (stdout,"  Return to review other interactions in this same paper.<br>\n");
		fprintf (stdout, "</form>\n");

		/*print links to go back to Accession pages*/
		fprintf (stdout, "Or return to the Accession page for either of the two molecules in this submitted record.<br>\n");
		pap = GetAccessionRecordbyGI(gia);
		if (pap != NULL)
		{
			PrintSummaryAccessionRecord(pap);
		}
		pap = GetAccessionRecordbyGI(gib);
		if (pap != NULL)
		{
			PrintSummaryAccessionRecord(pap);
		}
		/*start a new search*/
		fprintf (stdout, "<br>Or start a new search by clicking \"Search PreBIND\" in the menu to the left.\n");
	
	}
	else if (append_status != r4success)
	{
		fprintf (stdout,"<tr>\n");
		fprintf (stdout,"<td colspan=4 height=\"20\">\n");
		fprintf (stdout,"<br>\n");
		fprintf (stdout,"An error was encountered while processing your entry. Please use the back button in your browser and try again.  If this problem persists, email the friendly site administrator listed below.  Thank you.<p>\n");
		ErrPostEx(SEV_ERROR,0,0, "Submit failed to append record. append_status = %d", append_status);
	}

 
	/*print out the submitted search*/
	/*
	fprintf (stdout,"<tr>\n");
	fprintf (stdout,"<td colspan=4 height=\"20\">\n");
	fprintf (stdout,"<br><p><hr><br><hr>\n");
	fprintf (stdout,"The submitted record was as follows:</b><br><p>\n");
	fprintf (stdout,"<b>PubMed identifier: </b>%ld<br><p>\n", (long) pmid);
	if (opin == -1)
	{
		fprintf (stdout,"An interaction between %s and %s (either protein-protein or protein-DNA) IS NOT described in this paper. <br><p>\n", nama, namb);
	}
	else if (opin == 1)
	{
		fprintf (stdout,"<b>Molecule A</b><br>\n");
		fprintf (stdout,"<b>gene info identifier (gi): </b>%ld<br>\n", (long) sgia);
		fprintf (stdout,"<b>short label: </b>%s<br>\n", nama);
		fprintf (stdout,"<b>description: </b>%s<br>\n", adesc);
		fprintf (stdout,"<b>molecule type (1 = DNA, 2 = protein): </b>%ld<br>\n", (long) atype);
		fprintf (stdout,"<b>gi accurately describes molecule A (0 = unknown, 1 = yes, -1 = no): </b>%ld<br><br>\n", (long) aok);
		
		fprintf (stdout,"<b>Molecule B</b><br>\n");
		fprintf (stdout,"<b>gene info identifier (gi): </b>%ld<br>\n", (long) sgib);
		fprintf (stdout,"<b>short label: </b>%s<br>\n", namb);
		fprintf (stdout,"<b>description: </b>%s<br>\n", bdesc);
		fprintf (stdout,"<b>molecule type (1 = DNA, 2 = protein): </b>%ld<br>\n", (long) btype);
		fprintf (stdout,"<b>gi accurately describes molecule B (0 = unknown, 1 = yes, -1 = no): </b>%ld<br><br>\n", (long) bok);
	}
	fprintf (stdout,"<b>your first name: </b>%s<br>\n", fname);
	fprintf (stdout,"<b>your last name: </b>%s<br>\n", lname);
	fprintf (stdout,"<b>your BIND User ID: </b>%ld<br>\n", (long) buid);
	*/
	goto Exit;

Exit:
	/*finish the output*/
	fprintf (stdout,"</table>\n");
	WWWPrintFileData(FOOTERFILE,  stdout);
	fflush(stdout);

	/*MemFree()*/
	if (nama) MemFree (nama);
	if (namb) MemFree (namb);
	if (adesc) MemFree (adesc);
	if (bdesc) MemFree (bdesc);
	if (fname) MemFree (fname);
	if (lname) MemFree (lname);
	if (pap)   MemFree (pap);
	return 0;
}



/************************************************************/
/*															*/
/*	ReviewAPoint:											*/
/*	PARAMETERS:												*/
/*			Int4	pmid									*/
/*			Int4	gia										*/
/*			Int4	gib										*/
/*	RETURNS:												*/
/*			Int2	0: ok									*/
/************************************************************/
Int2 ReviewAPoint(Int4 pmid, Int4 gia, Int4 gib)
{	
	PREBINDPointPtr		ppp =	NULL;
	PREBINDAccPtr		papa=	NULL;
	PREBINDAccPtr		papb=	NULL;
	CharPtr				no_checked = " ";
	CharPtr				yes_checked = " ";
	CharPtr				a_protein_checked = " ";
	CharPtr				b_protein_checked = " ";
	Char				blueprint_location[PATH_MAX];
	Char				seqhound_location[PATH_MAX];
	Char				dataentry_location[PATH_MAX];

	/*get location information from the config file*/
	blueprint_location[0] = '\0';
	GetAppParam("prebind", "prebind_cgi", "blueprint_location", NULL, blueprint_location, PATH_MAX*sizeof(Char));

	seqhound_location[0] = '\0';
	GetAppParam("prebind", "prebind_cgi", "seqhound_location", NULL, seqhound_location, PATH_MAX*sizeof(Char));

	dataentry_location[0] = '\0';
	GetAppParam("prebind", "prebind_cgi", "dataentry_location", NULL, dataentry_location, PATH_MAX*sizeof(Char));


	/*print out the header for reviewing a POtential INTeraction and beginning of the form*/
	fprintf (stdout, "Content-type:text/html\r\n\r\n");
	WWWPrintFileData(REVIEWPOINTHEADER, stdout);
	fprintf (stdout, "<body bgcolor=\"white\">\n");
	fprintf (stdout, "<form method=\"POST\" action=\"%s\" target=\"display\">\n", cgi_path);
	fprintf (stdout,"<table width=95%% border=0 cellspacing=2 cellpadding=2 cols=4>");
	fprintf (stdout,"	<tr bgcolor=\"lightsteelblue\">");
	fprintf (stdout,"		<td colspan=4 height=\"20\">");
	fprintf (stdout,"			<h2>Review a potential interaction and make a submission to BIND</h2>");
	fprintf (stdout,"		</td\n>");
	fprintf (stdout,"	</tr>\n");
	fprintf (stdout,"	<tr>");
	fprintf (stdout,"		<td colspan=4 height=\"20\">");
	fprintf (stdout,"			This form allows you to submit a simple Biomolecular Interaction Network Database (BIND) record.  Information such as binding site location and experimental evidence cannot be entered using this form.\n");   
	fprintf (stdout,"			If you cannot describe the interaction using the gene info (\"gi\") numbers listed below or if you want to describe an interaction involving a molecular complex, small molecule or RNA then you must enter this\n");
	fprintf (stdout,"			information directly using the");
    fprintf (stdout,"			<a href=\"%s\" target=\"new\" onmouseover=\"window.status='BIND'\">BIND interface</a>.</p>\n", blueprint_location);
	fprintf (stdout,"		</td\n>");
	fprintf (stdout,"	</tr>\n");
	fprintf (stdout,"	<tr><td> </tr>\n");
	fprintf (stdout,"</table>\n");


	/*get a POtential INTeraction by PMID and two GI's*/
	ppp = GetPointbyPMIDandGIs(pmid, gia, gib);
	if (ppp == NULL)
	{
		ErrPostEx(SEV_ERROR,0,0, "ReviewAPoint: GetPointbyPMIDandGIs failed to find Point");
		fprintf (stdout, "Error: Failed to find Potential Interaction to Review<br>\n");
		return 0;
	}

	/*get ACCession record data for molecules A and B*/
	papa = GetAccessionRecordbyGI(gia);
	if (papa == NULL)
	{
		ErrPostEx(SEV_ERROR,0,0, "ReviewAPoint: GetAccessionRecordbyGI failed to find Accession data for A (gi = %ld)\n", (long) gia);
		fprintf (stdout, "Error: Failed to find Molecule A information to review. gi = %ld<br>\n", (long) gia);
		return 0;
	}
	papb = GetAccessionRecordbyGI(gib);
	if (papb == NULL)
	{
		ErrPostEx(SEV_ERROR,0,0, "ReviewAPoint: GetAccessionRecordbyGI failed to find Accession data for B (gi = %ld)\n", (long) gib);
		fprintf (stdout, "Error: Failed to find Molecule B information to review. gi = %ld<br>\n", (long) gib);
		return 0;
	}
	

	/*step 1: is there an interaction between A and B (prot::prot or prot::DNA) described in this PubMed paper*/
	fprintf (stdout,"<table width=95%% border=0 cellspacing=2 cellpadding=2 cols=4>\n");
	fprintf (stdout,"<tr bgcolor=\"lightsteelblue\">\n");
	fprintf (stdout,"<td colspan=4 height=\"20\">\n");
	fprintf (stdout,"<b>Step 1:</b> Is there an interaction between %s and %s (either protein-protein or protein-DNA) described in the above PubMed paper ", ppp->nama, ppp->namb);
	fprintf (stdout,"<a target=\"ncbi\" onmouseover=\"status='View abstract at PubMed'\"href=\"http://www.ncbi.nlm.nih.gov/htbin-post/Entrez/query?db=m&form=6&uid=%ld&Dopt=r&tool=EntrezLinkDoc\">%ld</a>?", (long) pmid, (long) pmid);
	fprintf (stdout,"</td>\n");
	fprintf (stdout,"</tr>\n");
	fprintf (stdout,"<tr><td colspan=4 height=\"20\"><br></td></tr>\n");/*empty row*/
	/*no*/
	if (ppp->pitruth < 0 || ppp->piscore <0 || ppp->piscore2 < 0 || ppp->opno < 0) no_checked = "checked";
	fprintf (stdout,"<tr>\n");
	fprintf (stdout,"<td colspan=4 height=\"20\">\n");
	fprintf (stdout,"<input type=radio %s name=opin value=\"-1\"> No, an interaction between these molecules is not described in this abstract.  Skip to the last step below and press submit.\n", no_checked);
	fprintf (stdout,"</td>\n");
	fprintf (stdout,"</tr>\n");
	/*yes*/
	if (ppp->pitruth > 0 || ppp->piscore >0 || ppp->piscore2 >0 ||ppp->opyes >0) yes_checked = "checked";
	fprintf (stdout,"<tr>\n");
	fprintf (stdout,"<td colspan=4 height=\"20\">\n");
	fprintf (stdout,"<input type=radio %s name=opin value=\"1\"> Yes, an interaction between these molecules is described in this abstract.  Continue with steps below and then press submit.\n", yes_checked);
	fprintf (stdout,"</td>\n");
	fprintf (stdout,"</tr>\n");
	fprintf (stdout,"<tr><td colspan=4 height=\"20\"><br></td></tr>\n");/*empty row*/
	fprintf (stdout,"</table>\n");

	/*step 2: describe Molecule A*/
	fprintf (stdout,"<table width=95%% border=0 cellspacing=2 cellpadding=2 cols=4>\n");
	fprintf (stdout,"<tr bgcolor=\"lightsteelblue\">\n");
	fprintf (stdout,"<td colspan=4 height=\"20\">\n");
	fprintf (stdout,"<b>Step 2:</b> Review and edit the description for molecule A\n");
	fprintf (stdout,"</td>\n");
	fprintf (stdout,"</tr>\n");
	fprintf (stdout,"<tr><td colspan=4 height=\"20\"><br></td></tr>\n");/*empty row*/
	
	/*short label*/
	fprintf (stdout,"<tr>\n");
    fprintf (stdout,"<td bgcolor=\"lightgoldenrodyellow\" colspan=4 height=\"20\">\n");
    fprintf (stdout,"<b>Enter a short label for molecule A:</b>\n"); 
	fprintf (stdout,"</td>\n");
    fprintf (stdout,"</tr>\n");
	fprintf (stdout,"<tr>\n");
    fprintf (stdout,"<td colspan=4 height=\"20\">\n");
    fprintf (stdout,"Choose the most appropriate name if more than one is presented<br><input type=\"text\" name=\"nama\" size=40 maxlength=60 value=\"%s\"> \n", ppp->nama);
	fprintf (stdout,"</td>\n");
    fprintf (stdout,"</tr>\n");
	fprintf (stdout,"<tr><td colspan=4 height=\"20\"><br></td></tr>\n");/*empty row*/
	
	/*description*/
	fprintf (stdout,"<tr>\n");
    fprintf (stdout,"<td bgcolor=\"lightgoldenrodyellow\" colspan=4 height=\"20\">\n");
    fprintf (stdout,"<b>Description of Molecule A:</b>\n");
	fprintf (stdout,"</td>\n");
    fprintf (stdout,"</tr>\n");
	fprintf (stdout,"<tr>\n");
    fprintf (stdout,"<td colspan=4 height=\"20\">\n");
    fprintf (stdout,"You may edit this text or leave it as is.<br><textarea name=\"adesc\" cols=40 rows=3 wrap=virtual>%s</textarea>\n", papa->pprod);
	fprintf (stdout,"</td>\n");
    fprintf (stdout,"</tr>\n");
	fprintf (stdout,"<tr><td colspan=4 height=\"20\"><br></td></tr>\n");/*empty row*/
	
	/*protein or DNA?*/
	fprintf (stdout,"<tr bgcolor=\"lightgoldenrodyellow\">\n");
	fprintf (stdout,"<td colspan=4 height=\"20\">\n");
	fprintf (stdout,"<b>Is Molecule A a protein or a DNA?</b>\n");
	fprintf (stdout,"</td>\n");
	fprintf (stdout,"</tr>\n");
	fprintf (stdout,"<tr>\n");
	fprintf (stdout,"<td colspan=4 height=\"20\">\n");
	fprintf (stdout,"Select one of the following:\n");
	fprintf (stdout,"</td>\n");
	fprintf (stdout,"</tr>\n");
	fprintf (stdout,"<tr><td colspan=4 height=\"20\"><br></td></tr>\n");/*empty row*/
	fprintf (stdout,"</table>");
	/*protein*/
	fprintf (stdout,"<table width=95%% border=0 cellspacing=2 cellpadding=2 cols=8>");
	if (ppp->atype == 2 || ppp->piscore > 0) a_protein_checked = "checked";
	fprintf (stdout,"<tr>\n");
	fprintf (stdout,"<td width=\"15%%\" valign=top colspan=1 height=\"20\">\n");
	fprintf (stdout,"	<input type=radio %s name=atype value=\"2\"> <b>Protein.</b>\n", a_protein_checked);
	fprintf (stdout,"</td>\n");
	fprintf (stdout,"<td colspan=7 height=\"20\">\n");
	fprintf (stdout,"Molecule A is a protein. \n");
	fprintf (stdout,"See the sequence in \n");
	fprintf (stdout, "<a href=\"javascript:void window.open('%s/wwwseekgi?field=GI+identifier&GI=%ld&format=defline', 'giWin', 'toolbar=no,location=no,directories=no,status=yes,menubar=yes,scrollbars=yes,resizable=yes,width=550,height=400');\" onmouseover=\"window.status='Go to SeqHound';return(true);\" onmouseout=\"window.status=''\">SeqHound</a>\n",seqhound_location, (long) gia);
	fprintf (stdout, " or at the <a href=\"javascript:void window.open('http://www.ncbi.nlm.nih.gov/entrez/query.fcgi?cmd=Retrieve&db=Protein&dopt=DocSum&list_uids=%ld', 'giWin', 'toolbar=no,location=no,directories=no,status=yes,menubar=yes,scrollbars=yes,resizable=yes,width=550,height=400');\" onmouseover=\"window.status='Go to Entrez';return(true);\" onmouseout=\"window.status=''\">NCBI</a>.\n", (long) gia);
	fprintf (stdout, "The Gene Info number (gi) is %ld.\n", (long) gia);
	fprintf (stdout,"</td>\n");
	fprintf (stdout,"</tr>\n");
	/*DNA*/
	fprintf (stdout,"<tr>\n");
	fprintf (stdout,"<td width=\"15%%\" valign=top colspan=1 height=\"20\">\n");
	fprintf (stdout,"<input type=radio name=atype value=\"1\"> <b>DNA.</b>\n");
	fprintf (stdout,"</td>\n");
	fprintf (stdout,"<td colspan=7 height=\"20\">\n");
	fprintf (stdout,"Molecule A is a DNA bound by Molecule B (a protein).  The binding site on Molecule A is somewhere in or around the sequence location: %ld to %ld in the following sequence: \n", (long) (ppp->adf + 1), (long) (ppp->adt + 1));
	fprintf (stdout,"View the sequence in \n");
	fprintf (stdout, "<a href=\"javascript:void window.open('%s/wwwseekgi?field=GI+identifier&GI=%ld&format=defline', 'giWin', 'toolbar=no,location=no,directories=no,status=yes,menubar=yes,scrollbars=yes,resizable=yes,width=550,height=400');\" onmouseover=\"window.status='Go to SeqHound';return(true);\" onmouseout=\"window.status=''\">SeqHound</a>\n",seqhound_location, (long) ppp->adgi);
	fprintf (stdout, " or at the <a href=\"javascript:void window.open('http://www.ncbi.nlm.nih.gov/entrez/query.fcgi?cmd=Retrieve&db=Nucleotide&list_uids=%ld', 'giWin', 'toolbar=no,location=no,directories=no,status=yes,menubar=yes,scrollbars=yes,resizable=yes,width=550,height=400');\" onmouseover=\"window.status='Go to Entrez';return(true);\" onmouseout=\"window.status=''\">NCBI</a>.\n", (long) ppp->adgi);
	fprintf (stdout, "The Gene Info number (gi) is %ld.\n",(long) ppp->adgi);
	fprintf (stdout,"</td>\n");
	fprintf (stdout,"</tr>\n");
	fprintf (stdout,"<tr><td colspan=8 height=\"20\"><br></td></tr>\n");/*empty row*/
	/*submit a completely different gi*/
	fprintf (stdout,"<tr><td colspan=8 height=\"20\">\n");	
    fprintf (stdout,"If neither of the gi's listed above apply, select protein or DNA above and enter the correct gi number here. <input type=\"text\" name=\"sgia\" size=40 maxlength=60>\n");
	fprintf (stdout,"</td>\n");
	fprintf (stdout,"</tr>\n");
	fprintf (stdout,"</table>");


	/*does the gi for A accurately describe the molecule a?*/
	fprintf (stdout,"<table width=95%% border=0 cellspacing=2 cellpadding=2 cols=\"4\">");
	fprintf (stdout,"<tr><td colspan=4 height=\"20\"><br></td></tr>\n");/*empty row*/
	fprintf (stdout,"<tr bgcolor=\"lightgoldenrodyellow\">\n");
	fprintf (stdout,"<td colspan=4 height=\"20\">\n");
	fprintf (stdout,"<b>If Molecule A is a protein, is it post-translationally modified? </b>\n");
	fprintf (stdout,"</td>\n");
	fprintf (stdout,"</tr>\n");
	fprintf (stdout,"<tr>\n");
	fprintf (stdout,"<td colspan=\"4\" height=\"20\">\n");
	fprintf (stdout,"Select one of the following:\n");
	fprintf (stdout,"</td>\n");
	fprintf (stdout,"</tr>\n");
	fprintf (stdout,"<tr><td colspan=4 height=\"20\"><br></td></tr>\n");/*empty row*/
	fprintf (stdout,"</table>");
	/*dont know*/
	fprintf (stdout,"<table width=95%% border=0 cellspacing=2 cellpadding=2 cols=8>");
	fprintf (stdout,"<tr>\n");
	fprintf (stdout,"<td width=\"15%%\" valign=top colspan=1 height=\"20\">\n");
	fprintf (stdout,"<input type=radio checked name=aok value=\"0\"> <b>Unknown.</b>\n");
	fprintf (stdout,"</td>\n");
	fprintf (stdout,"<td colspan=7 height=\"20\">\n");
	fprintf (stdout,"Molecule A may or may not be a modified.\n");
	fprintf (stdout,"</td>\n");
	fprintf (stdout,"</tr>\n");	
	/*yes*/
	fprintf (stdout,"<tr>\n");
	fprintf (stdout,"<td valign=top colspan=1 height=\"20\">\n");
	fprintf (stdout,"<input type=radio name=aok value=\"-1\"> <b>Yes.</b>\n");
	fprintf (stdout,"</td>\n");
	fprintf (stdout,"<td colspan=7 height=\"20\">\n");
	fprintf (stdout,"Molecule A is modified.  For example, molecule A may be a maturated protein or otherwise post-translationally modified.");  
	fprintf (stdout,"You may make a note of any such modifications in the 'Description of Molecule A' text box above.\n");
	fprintf (stdout,"</td>\n");
	fprintf (stdout,"</tr>\n");
	/*no*/
	fprintf (stdout,"<tr>\n");
	fprintf (stdout,"<td valign=top colspan=1 height=\"20\">\n");
	fprintf (stdout,"<input type=radio name=aok value=\"1\"> <b>No.</b>\n");
	fprintf (stdout,"</td>\n");
	fprintf (stdout,"<td colspan=7 height=\"20\">\n");
	fprintf (stdout,"Molecule A is not modified.  The amino acid sequence in the sequence record selected above is an accurate description of Molecule A.");  
	fprintf (stdout,"</td>\n");
	fprintf (stdout,"</tr>\n");
	fprintf (stdout,"<tr><td colspan=8 height=\"20\"><br></td></tr>\n");/*empty row*/
	fprintf (stdout,"</table>");

	/*step 3: describe Molecule B*/
	fprintf (stdout,"<table width=95%% border=0 cellspacing=2 cellpadding=2 cols=4>\n");
	fprintf (stdout,"<tr bgcolor=\"lightsteelblue\">\n");
	fprintf (stdout,"<td colspan=4 height=\"20\">\n");
	fprintf (stdout,"<b>Step 3:</b> Review and edit the description for molecule B\n");
	fprintf (stdout,"</td>\n");
	fprintf (stdout,"</tr>\n");
	fprintf (stdout,"<tr><td colspan=4 height=\"20\"><br></td></tr>\n");/*empty row*/
	
	/*short label*/
	fprintf (stdout,"<tr>\n");
    fprintf (stdout,"<td bgcolor=\"lightgoldenrodyellow\" colspan=4 height=\"20\">\n");
    fprintf (stdout,"<b>Enter a short label for molecule B:</b>\n"); 
	fprintf (stdout,"</td>\n");
    fprintf (stdout,"</tr>\n");
	fprintf (stdout,"<tr>\n");
    fprintf (stdout,"<td colspan=4 height=\"20\">\n");
    fprintf (stdout,"Choose the most appropriate name if more than one is presented<br><input type=\"text\" name=\"namb\" size=40 maxlength=40 value=\"%s\"> \n", ppp->namb);
	fprintf (stdout,"</td>\n");
    fprintf (stdout,"</tr>\n");
	fprintf (stdout,"<tr><td colspan=4 height=\"20\"><br></td></tr>\n");/*empty row*/
	
	/*description*/
	fprintf (stdout,"<tr>\n");
    fprintf (stdout,"<td bgcolor=\"lightgoldenrodyellow\" colspan=4 height=\"20\">\n");
    fprintf (stdout,"<b>Description of Molecule B:</b>\n");
	fprintf (stdout,"</td>\n");
    fprintf (stdout,"</tr>\n");
	fprintf (stdout,"<tr>\n");
    fprintf (stdout,"<td colspan=4 height=\"20\">\n");
    fprintf (stdout,"You may edit this text or leave it as is.<br><textarea name=\"bdesc\" cols=40 rows=3 wrap=virtual>%s</textarea>\n", papb->pprod);
	fprintf (stdout,"</td>\n");
    fprintf (stdout,"</tr>\n");
	fprintf (stdout,"<tr><td colspan=4 height=\"20\"><br></td></tr>\n");/*empty row*/
	
	/*protein or DNA?*/
	fprintf (stdout,"<tr bgcolor=\"lightgoldenrodyellow\">\n");
	fprintf (stdout,"<td colspan=4 height=\"20\">\n");
	fprintf (stdout,"<b>Is Molecule B a protein or a DNA?</b>\n");
	fprintf (stdout,"</td>\n");
	fprintf (stdout,"</tr>\n");
	fprintf (stdout,"<tr>\n");
	fprintf (stdout,"<td colspan=4 height=\"20\">\n");
	fprintf (stdout,"Select one of the following:\n");
	fprintf (stdout,"</td>\n");
	fprintf (stdout,"</tr>\n");
	fprintf (stdout,"<tr><td colspan=4 height=\"20\"><br></td></tr>\n");/*empty row*/
	fprintf (stdout,"</table>");
	/*protein*/
	fprintf (stdout,"<table width=95%% border=0 cellspacing=2 cellpadding=2 cols=8>");
	if (ppp->btype == 2 || ppp->piscore > 0) b_protein_checked = "checked";
	fprintf (stdout,"<tr>\n");
	fprintf (stdout,"<td width=\"15%%\" valign=top colspan=1 height=\"20\">\n");
	fprintf (stdout,"	<input type=radio %s name=btype value=\"2\"> <b>Protein.</b>\n", b_protein_checked);
	fprintf (stdout,"</td>\n");
	fprintf (stdout,"<td colspan=7 height=\"20\">\n");
	fprintf (stdout,"Molecule B is a protein. \n");
	fprintf (stdout,"See the sequence in \n");
	fprintf (stdout, "<a href=\"javascript:void window.open('%s/wwwseekgi?field=GI+identifier&GI=%ld&format=defline', 'giWin', 'toolbar=no,location=no,directories=no,status=yes,menubar=yes,scrollbars=yes,resizable=yes,width=550,height=400');\" onmouseover=\"window.status='Go to SeqHound';return(true);\" onmouseout=\"window.status=''\">SeqHound</a>\n",seqhound_location, (long) gib);
	fprintf (stdout, " or at the <a href=\"javascript:void window.open('http://www.ncbi.nlm.nih.gov/entrez/query.fcgi?cmd=Retrieve&db=Protein&dopt=DocSum&list_uids=%ld', 'giWin', 'toolbar=no,location=no,directories=no,status=yes,menubar=yes,scrollbars=yes,resizable=yes,width=550,height=400');\" onmouseover=\"window.status='Go to Entrez';return(true);\" onmouseout=\"window.status=''\">NCBI</a>.\n", (long) gib);
	fprintf (stdout, "The Gene Info number (gi) is %ld.\n", (long) gib);
	fprintf (stdout,"</td>\n");
	fprintf (stdout,"</tr>\n");
	/*DNA*/
	fprintf (stdout,"<tr>\n");
	fprintf (stdout,"<td width=\"15%%\" valign=top colspan=1 height=\"20\">\n");
	fprintf (stdout,"<input type=radio name=btype value=\"1\"> <b>DNA.</b>\n");
	fprintf (stdout,"</td>\n");
	fprintf (stdout,"<td colspan=7 height=\"20\">\n");
	fprintf (stdout,"Molecule B is a DNA bound by Molecule A (a protein).  The binding site on Molecule B is somewhere in or around the sequence location: %ld to %ld in the following sequence: \n", (long) (ppp->bdf + 1), (long) (ppp->bdt + 1));
	fprintf (stdout,"View the sequence in \n");
	fprintf (stdout, "<a href=\"javascript:void window.open('%s/wwwseekgi?field=GI+identifier&GI=%ld&format=defline', 'giWin', 'toolbar=no,location=no,directories=no,status=yes,menubar=yes,scrollbars=yes,resizable=yes,width=550,height=400');\" onmouseover=\"window.status='Go to SeqHound';return(true);\" onmouseout=\"window.status=''\">SeqHound</a>\n",seqhound_location, (long) ppp->bdgi);
	fprintf (stdout, " or at the <a href=\"javascript:void window.open('http://www.ncbi.nlm.nih.gov/entrez/query.fcgi?cmd=Retrieve&db=Nucleotide&list_uids=%ld', 'giWin', 'toolbar=no,location=no,directories=no,status=yes,menubar=yes,scrollbars=yes,resizable=yes,width=550,height=400');\" onmouseover=\"window.status='Go to Entrez';return(true);\" onmouseout=\"window.status=''\">NCBI</a>.\n", (long) ppp->bdgi);
	fprintf (stdout, "The Gene Info number (gi) is %ld.\n",(long) ppp->bdgi);
	fprintf (stdout,"</td>\n");
	fprintf (stdout,"</tr>\n");
	fprintf (stdout,"<tr><td colspan=8 height=\"20\"><br></td></tr>\n");/*empty row*/
	/*submit a completely different gi*/
	fprintf (stdout,"<tr><td colspan=8 height=\"20\">\n");	
    fprintf (stdout,"If neither of the gi's listed above apply, select protein or DNA above and enter the correct gi number here. <input type=\"text\" name=\"sgib\" size=40 maxlength=60>\n");
	fprintf (stdout,"</td>\n");
	fprintf (stdout,"</tr>\n");
	fprintf (stdout,"</table>");

	/*does the gi for a accurately describe the molecule b?*/
	fprintf (stdout,"<table width=\"95%%\" border=0 cellspacing=2 cellpadding=2 cols=4>");
	fprintf (stdout,"<tr><td colspan=4 height=\"20\"><br></td></tr>\n");/*empty row*/
	fprintf (stdout,"<tr bgcolor=\"lightgoldenrodyellow\">\n");
	fprintf (stdout,"<td colspan=4 height=\"20\">\n");
	fprintf (stdout,"<b>If Molecule B is a protein, is it post-translationally modified?</b>\n");
	fprintf (stdout,"</td>\n");
	fprintf (stdout,"</tr>\n");
	fprintf (stdout,"<tr>\n");
	fprintf (stdout,"<td colspan=\"4\" height=\"20\">\n");
	fprintf (stdout,"Select one of the following:\n");
	fprintf (stdout,"</td>\n");
	fprintf (stdout,"</tr>\n");
	fprintf (stdout,"<tr><td colspan=4 height=\"20\"><br></td></tr>\n");/*empty row*/
	fprintf (stdout,"</table>");
	/*dont know*/
	fprintf (stdout,"<table width=95%% border=0 cellspacing=2 cellpadding=2 cols=8>");
	fprintf (stdout,"<tr>\n");
	fprintf (stdout,"<td width=\"15%%\" valign=top colspan=1 height=\"20\">\n");
	fprintf (stdout,"<input type=radio checked name=bok value=\"0\"> <b>Unknown.</b>\n");
	fprintf (stdout,"</td>\n");
	fprintf (stdout,"<td colspan=7 height=\"20\">\n");
	fprintf (stdout,"Molecule B may or may not be a modified.\n");
	fprintf (stdout,"</td>\n");
	fprintf (stdout,"</tr>\n");	
	/*yes*/
	fprintf (stdout,"<tr>\n");
	fprintf (stdout,"<td valign=top colspan=1 height=\"20\">\n");
	fprintf (stdout,"<input type=radio name=bok value=\"-1\"> <b>Yes.</b>\n");
	fprintf (stdout,"</td>\n");
	fprintf (stdout,"<td colspan=7 height=\"20\">\n");
	fprintf (stdout,"Molecule B is modified.  For example, molecule B may be a maturated protein or may be otherwise post-translationally modified.");  
	fprintf (stdout,"You may make a note of any such modifications in the 'Description of Molecule B' text box above.\n");
	fprintf (stdout,"</td>\n");
	fprintf (stdout,"</tr>\n");
	/*no*/
	fprintf (stdout,"<tr>\n");
	fprintf (stdout,"<td valign=top colspan=1 height=\"20\">\n");
	fprintf (stdout,"<input type=radio name=bok value=\"1\"> <b>No.</b>\n");
	fprintf (stdout,"</td>\n");
	fprintf (stdout,"<td colspan=7 height=\"20\">\n");
	fprintf (stdout,"Molecule B is not modified.  The amino acid sequence in the sequence record selected above is an accurate description of Molecule B.");  
	fprintf (stdout,"</td>\n");
	fprintf (stdout,"</tr>\n");
	fprintf (stdout,"<tr><td colspan=8 height=\"20\"><br></td></tr>\n");/*empty row*/
	fprintf (stdout,"</table>");

	/*step 4: enter your BIND submit name and id*/
	fprintf (stdout,"<table width=95%% border=0 cellspacing=2 cellpadding=2 cols=4>\n");
	fprintf (stdout,"<tr bgcolor=\"lightsteelblue\">\n");
	fprintf (stdout,"<td colspan=4 height=\"20\">\n");
	fprintf (stdout,"<b>Step 4:</b> Enter your name and BIND user ID (optional).\n");
	fprintf (stdout,"</td>\n");
	fprintf (stdout,"</tr>\n");
	fprintf (stdout,"<tr><td colspan=4 height=\"20\"><br></td></tr>\n");/*empty row*/
	/*first name*/
	fprintf (stdout,"<tr>\n");
    fprintf (stdout,"<td colspan=4 height=\"20\">\n");
    fprintf (stdout,"First name:<br><input type=\"text\" name=\"fname\" size=40 maxlength=40> \n");
	fprintf (stdout,"</td>\n");
    fprintf (stdout,"</tr>\n");
	/*last name*/
	fprintf (stdout,"<tr>\n");
    fprintf (stdout,"<td colspan=4 height=\"20\">\n");
    fprintf (stdout,"Last name:<br><input type=\"text\" name=\"lname\" size=40 maxlength=40> \n");
	fprintf (stdout,"</td>\n");
    fprintf (stdout,"</tr>\n");
	/*user id*/
	fprintf (stdout,"<tr>\n");
    fprintf (stdout,"<td colspan=4 height=\"20\">\n");
    fprintf (stdout,"User ID:<br><input type=\"text\" name=\"buid\" size=40 maxlength=40> \n");
	fprintf (stdout,"</td>\n");
    fprintf (stdout,"</tr>\n");
	/*go get a user id*/
	fprintf (stdout,"<tr>\n");
    fprintf (stdout,"<td colspan=4 height=\"20\">\n");
    fprintf (stdout,"If you do not have a user id you can get one from\n");

	fprintf (stdout, "<a href=\"javascript:void window.open('%s?form=getuserid&rectype=4&change=1', 'get_user_id', 'toolbar=no,location=no,directories=no,status=yes,menubar=yes,scrollbars=yes,resizable=yes,width=550,height=400');\" onmouseover=\"window.status='Go get a BIND User ID';return(true);\" onmouseout=\"window.status=''\">here</a>.\n", dataentry_location);
	fprintf (stdout,"<br>(After you enter your contact info and press 'submit', write down the User ID, close the window and continue here.)\n");
    fprintf (stdout,"<br>Alternatively, you may leave this section blank and your record will be submitted as an anonymous user; however, you will not be able to further edit your record.\n");
	fprintf (stdout,"</td>\n");
    fprintf (stdout,"</tr>\n");
	fprintf (stdout,"</table>");

	fprintf (stdout, "<br>Optionally, you may just add this pmid to an existing BIND record.\n");
	fprintf (stdout,"<tr><td height=\"20\"><br></td></tr>\n");
	fprintf (stdout,"BIND record ID:<br><input type=\"text\" name=\"piid\" size=40 maxlength=40> \n");
    fprintf (stdout,"</td>\n");
    fprintf (stdout,"</tr>\n");
	fprintf (stdout,"<td colspan=4 height=\"20\">\n");
    fprintf (stdout,"</td>\n");
    fprintf (stdout,"</tr>\n");

	fprintf (stdout,"<tr><td height=\"20\"><br></td></tr>\n");
	fprintf (stdout,"<tr><td height=\"20\"><br></td></tr>\n");


	/*step 5: submit*/
	fprintf (stdout,"<table width=\"95%%\" border=0 cellspacing=2 cellpadding=2 cols=\"4\">\n");
	fprintf (stdout,"<tr bgcolor=\"lightsteelblue\">\n");
	fprintf (stdout,"<td width=\"100%%\" height=\"20\">\n");
	fprintf (stdout,"<b>Step 5:</b> Submit\n");
	fprintf (stdout,"</td>\n");
	fprintf (stdout,"</tr>\n");
	fprintf (stdout,"<tr><td height=\"20\"><br></td></tr>\n");/*empty row*/
	fprintf (stdout,"</table>");
	/*submit button*/
	fprintf (stdout,"<table width=\"95%%\" border=0 cellspacing=2 cellpadding=2 cols=\"8\">\n");
	fprintf (stdout,"<input type=hidden name=search_type value=\"submit\"> \n");
	fprintf (stdout,"<input type=hidden name=pmid value=\"%ld\"> \n", (long) pmid);
	fprintf (stdout,"<input type=hidden name=gia value=\"%ld\"> \n", (long) gia);
	fprintf (stdout,"<input type=hidden name=gib value=\"%ld\"> \n", (long) gib);
	/*
	fprintf (stdout,"<input type=hidden name=sgia value=\"%ld\"> \n", (long) gia);
	fprintf (stdout,"<input type=hidden name=sgib value=\"%ld\"> \n", (long) gib);
	*/
	fprintf (stdout,"	<tr valign=\"top\">\n");
    fprintf (stdout,"		<td colspan=\"1\" height=\"20\">\n");
	fprintf (stdout,"			<input type=submit value=Submit> \n");
	fprintf (stdout,"		</td>\n");
    fprintf (stdout,"		<td colspan=\"7\" height=\"20\">\n");
	fprintf (stdout,"			<b>Submit your interaction.</b> <p>If possible, a temporary BIND record will be generated. Other information such as binding site location and experimental evidence can be entered to this record.  Your record will be reviewed by a curator and you will be notified when it has been submitted to BIND.\n");  
	fprintf (stdout,"			If you want to describe an interaction involving a molecular complex, small molecule or RNA then you must enter this\n");
	fprintf (stdout,"			information directly using the\n");
	fprintf (stdout,"			<a href=\"%s\" target=\"new\" onmouseover=\"window.status='BIND'\">BIND interface</a>.</p>\n", blueprint_location);
	fprintf (stdout,"</td>\n");
    fprintf (stdout,"</tr>\n");
	fprintf (stdout,"<tr><td colspan=\"8\" height=\"20\"><br></td></tr>\n");/*empty row*/
	fprintf (stdout,"</table>");

	/*lessons from table building: first row defines the largest number of columns in a row without screwing things up*/
	/*test*/
	/*fprintf (stdout,"<tr><td colspan=4 height=\"20\"><br></td></tr>\n");
	fprintf (stdout, "pmid = %ld<br>\n", (long) pmid);
	fprintf (stdout, "gia = %ld<br>\n", (long) gia);
	fprintf (stdout, "gib = %ld<br>\n", (long) gib);
	fprintf (stdout, "score = %ld<br>\n", (long) ppp->piscore);
	fprintf (stdout, "atype = %ld<br>\n", (long) ppp->atype);
	fprintf (stdout, "A DNA gi= %ld<br>\n", (long) ppp->adgi);
	fprintf (stdout, "A DNA gi from = %ld<br>\n", (long) ppp->adf);
	fprintf (stdout, "A DNA gi to = %ld<br>\n", (long) ppp->adt);*/

	/*end the form and print footer*/
	fprintf (stdout, "</form>\n");
	WWWPrintFileData(FOOTERFILE,  stdout);
	fflush(stdout);

	/*MemFree();*/
	if (ppp)  MemFree(ppp);
	if (papa) MemFree(papa);
	if (papb) MemFree(papb);

	return 0;
}


/************************************************************/
/*															*/
/*	RecordPointOpinion:										*/
/*			Given a PubMed ID, two GI's and an opinion	 	*/
/*			about the interaction, this function			*/
/*			records the opinion in the POINT database		*/
/*	PARAMETERS:												*/
/*			Int4	pmid									*/
/*			Int4	gia										*/
/*			Int4	gib										*/
/*			Int4	new_opinion								*/
/*	RETURNS:												*/
/*			Int2:	0	ok									*/
/*					!=0	not ok								*/
/************************************************************/
Int2 RecordPointOpinion(Int4 pmid, Int4 gia, Int4 gib, Int4 new_opinion)
{	
	
	
	Int2				point_record =		0;
	Int4				cur_gia =			0;
	Int4				cur_gib =			0;
	Int4				current_opinion =	0;
	Boolean				opinion_recorded =	FALSE;
	


	/*find a POINT record with the pmid number passed to this function*/
	/*select pmid tag*/
	d4tagSelect(pd4_POINT, pt4_POINT_PMID);

	point_record = d4seekDouble(pd4_POINT, pmid);
	if (point_record != 0)
	{
		return 1; /*there are no POINT records for the pmid*/
	}
	while (point_record == 0) /*r4 success*/
	{
		cur_gia = f4long(pf4_POINT_GIA);
		cur_gib = f4long(pf4_POINT_GIB);
		if ((gia == cur_gia && gib == cur_gib) || (gia == cur_gib && gib == cur_gia))
		{

			if (new_opinion == -1)
			{
				/*get opinion count for NO this is not an interaction and increase it by one*/
				current_opinion = f4long(pf4_POINT_OPNO);
				++current_opinion;
				f4assignLong(pf4_POINT_OPNO, current_opinion);
				opinion_recorded = TRUE; 
				break;
			}
			if (new_opinion == 1)
			{
				/*get opinion count for YES this is an interaction and increase it by one*/
				current_opinion = f4long(pf4_POINT_OPYES);
				++current_opinion;
				f4assignLong(pf4_POINT_OPYES, current_opinion);
				opinion_recorded = TRUE; 
				break;
			}
			
			
		}/*if this is the record you were looking for*/

		/*look for the next POINT record*/
		point_record = d4seekNextDouble(pd4_POINT, pmid);

	}/*while another POINT record is found by pmid*/

	if (opinion_recorded ==	FALSE)
	{
		return 2;
	}

	return 0;
}


/************************************************************/
/*															*/
/*	GetPointbyPMIDandGIs:									*/
/*			Given a PubMed ID, and two GI's this function 	*/
/*			returns a POtential INTeraction record from the */
/*			POINT database									*/
/*	PARAMETERS:												*/
/*			Int4	pmid									*/
/*			Int4	gia										*/
/*			Int4	gib										*/
/*	RETURNS:												*/
/*			PREBINDPointSetPtr:	ptr to a POINT record		*/
/*			NULL:				no record found				*/
/************************************************************/
PREBINDPointPtr GetPointbyPMIDandGIs(Int4 pmid, Int4 gia, Int4 gib)
{	
	
	PREBINDPointPtr		ppp_tofill =	NULL;
	Int2				point_record =	0;
	Int4				cur_gia =		0;
	Int4				cur_gib =		0;
	Int2				locus_record =	0;

	/*start a new PREBIND Point */
	ppp_tofill = PREBINDPointNew();
	if (ppp_tofill == NULL)
	{
		ErrPostEx(SEV_ERROR,0,0, "GetPointbyPMIDandGIs: Failedto allocate memory for PREBINDPoint");
		return NULL;
	}


	/*find a POINT record with the pmid number passed to this function*/
	/*select pmid tag*/
	d4tagSelect(pd4_POINT, pt4_POINT_PMID);

	point_record = d4seekDouble(pd4_POINT, pmid);
	if (point_record != 0)
	{
		return NULL; /*there are no POINT records for the pmid*/
	}
	while (point_record == 0) /*r4 success*/
	{
		cur_gia = f4long(pf4_POINT_GIA);
		cur_gib = f4long(pf4_POINT_GIB);
		if ((gia == cur_gia && gib == cur_gib) || (gia == cur_gib && gib == cur_gia))
		{
			/*already have pmid*/
			ppp_tofill->pmid = pmid;
			
			/*get muid*/
			ppp_tofill->muid = f4long(pf4_POINT_MUID);
			
			/*get gi of A*/
			ppp_tofill->gia = cur_gia;
			
			/*get name list for A*/
			ppp_tofill->nama = StrSave(f4str(pf4_POINT_NAMA));
			if (ppp_tofill->nama == NULL) ppp_tofill->nama = StrSave("NOT FOUND");
			if(TrimSpaces(ppp_tofill->nama) != 0)
				ErrPostEx(SEV_ERROR,0,0, "GetPointbyPMIDandGIs: TrimSpaces failed, possibly passed a null string");
			
			/*get molecule type for A*/
			ppp_tofill->atype = f4long(pf4_POINT_ATYPE);
			
			/*get source DNA info for A from the LOCUS database*/
			d4tagSelect(pd4_LOCUS, pt4_LOCUS_GIP);
			locus_record = d4seekDouble(pd4_LOCUS, cur_gia);
			if (locus_record != 0)
			{
				ppp_tofill->adgi = 0;
				ppp_tofill->adf = 0;
				ppp_tofill->adt = 0;
			}
			else if (locus_record == 0)
			{
				ppp_tofill->adgi = f4long(pf4_LOCUS_GIN);
				ppp_tofill->adf = f4long(pf4_LOCUS_FROM);
				ppp_tofill->adt = f4long(pf4_LOCUS_TO);
			}
			
			/*get gi of B*/
			ppp_tofill->gib = cur_gib;
			
			/*get name list for B*/
			ppp_tofill->namb = StrSave(f4str(pf4_POINT_NAMB));
			if (ppp_tofill->namb == NULL) ppp_tofill->namb = StrSave("NOT FOUND");
			if(TrimSpaces(ppp_tofill->namb) != 0)
				ErrPostEx(SEV_ERROR,0,0, "GetPointbyPMIDandGIs: TrimSpaces failed, possibly passed a null string");
			
			/*get molecule type for B*/
			ppp_tofill->btype = f4long(pf4_POINT_BTYPE);
			
			/*get source DNA info for B from the LOCUS database*/
			locus_record = d4seekDouble(pd4_LOCUS, cur_gib);
			if (locus_record != 0)
			{
				ppp_tofill->bdgi = 0;
				ppp_tofill->bdf = 0;
				ppp_tofill->bdt = 0;
			}
			else if (locus_record == 0)
			{
				ppp_tofill->bdgi = f4long(pf4_LOCUS_GIN);
				ppp_tofill->bdf = f4long(pf4_LOCUS_FROM);
				ppp_tofill->bdt = f4long(pf4_LOCUS_TO);
			}
			
			/*get opinion count for NO this is not an interaction*/
			ppp_tofill->opno = f4long(pf4_POINT_OPNO);
			
			/*get opinion count for YES this is an interaction*/
			ppp_tofill->opyes = f4long(pf4_POINT_OPYES);
			
			/*get potential interaction score 1*/
			ppp_tofill->piscore = f4long(pf4_POINT_PISCORE);
			
			/*get potential interaction score 2*/
			ppp_tofill->piscore2 = f4long(pf4_POINT_PISCORE2);
			
			/*get interaction truth*/
			ppp_tofill->pitruth = f4long(pf4_POINT_PITRUTH);

			return ppp_tofill;
			
		}/*if this is the record you were looking for*/

		/*look for the next POINT record*/
		point_record = d4seekNextDouble(pd4_POINT, pmid);

	}/*while another POINT record is found by pmid*/

	return NULL;
}



/************************************************************/
/*															*/
/*	GetListofPointsby2GIs:									*/
/*			Given 2 GIs, this function returns a list		*/
/*			of all the POtential INTeractions present in the*/
/*			paper from the POINT database					*/
/*	PARAMETERS:												*/
/*			Int 4	gia										*/
/*			Int 4	gib										*/
/*	RETURNS:												*/
/*			PREBINDPointSetPtr	ptr to a list of POINT		*/
/*								records						*/
/*			NULL				empty list					*/
/************************************************************/
PREBINDPointSetPtr GetListofPointsby2GIs(Int4 gia, Int4 gib, Boolean filter_nonint_papers, Int4 filter_toomanyresults)
{	
	PREBINDPointSetPtr	ppsp =			NULL;
	PREBINDPointPtr		ppp_head =		NULL;
	PREBINDPointPtr		ppp_tofill =	NULL;
	PREBINDPointPtr		ppp_lastfilled= NULL;
	Int2				loop =			0;
	Int2				point_record =	0;
	Int4				second_gi =		0;
	Int4				pmid =			0;
	PREBINDIsvmPtr		pisvmp =		NULL;
	Boolean				add_to_list =	TRUE; /*default*/
	

	/*start a new PREBIND Point Set*/
	ppsp = PREBINDPointSetNew();
	ppp_head = PREBINDPointNew();
	if (ppsp && ppp_head)
	{
		ppsp->records = ppp_head;
		ppp_tofill = ppp_head;
	}
	else
	{
		ErrPostEx(SEV_ERROR,0,0, "GetListofPointsby2GIs: Failed to allocate memory for a new list");
		return NULL;
	}


	/*find a POINT record with the gia number passed to this function: first in the gia column then in the gib column*/
	for (loop=0;loop<2;++loop)
	{
		if (loop == 0) d4tagSelect(pd4_POINT, pt4_POINT_GIA);
		if (loop == 1) d4tagSelect(pd4_POINT, pt4_POINT_GIB);
		point_record = d4seekDouble(pd4_POINT, gia);
		
		while (point_record == 0) /*r4 success*/
		{
			/*only if second gi is gib?*/
			if (loop == 0) second_gi = f4long(pf4_POINT_GIB);
			if (loop == 1) second_gi = f4long(pf4_POINT_GIA);
			
			if (gib == second_gi)
			{
				/*get pmid and isvm record*/
				pmid = f4long(pf4_POINT_PMID);
				pisvmp = GetIsvmbyPMID(pmid);
				
				/*check to see if the paper should be added to the list; i.e. only interactors in interaction papers?*/
				if (filter_nonint_papers && pisvmp)
				{
					if (pisvmp->itruth < 0 || pisvmp->opyes < pisvmp->opno || pisvmp->iscore < 0)
					{
						add_to_list = FALSE;
					}
				}
				if (pisvmp) pisvmp = MemFree(pisvmp);
				
				/*check to see if the interactor should be added to the list; i.e. only interactor names with low number of search results*/		
				if (add_to_list && filter_toomanyresults)/*this is a costly filter--only use if the user requests it*/
				{
					if (FilterPointByNamesWithTooManyResults(gia, gib, filter_toomanyresults) == TRUE)
					{
						add_to_list = FALSE;
					}
				}
							
				
				if (add_to_list)
				{
					/*fill ppp_tofill with data*/
					
					/*get pmid*/
					ppp_tofill->pmid = f4long(pf4_POINT_PMID);
					
					/*get muid*/
					ppp_tofill->muid = f4long(pf4_POINT_MUID);
					
					/*get gi of A*/
					ppp_tofill->gia = f4long(pf4_POINT_GIA);
					
					/*get name list for A*/
					ppp_tofill->nama = StrSave(f4str(pf4_POINT_NAMA));
					if (ppp_tofill->nama == NULL) ppp_tofill->nama = StrSave("NOT FOUND");
					if(TrimSpaces(ppp_tofill->nama) != 0)
						ErrPostEx(SEV_ERROR,0,0, "GetListofPointsby2GIs: TrimSpaces failed, possibly passed a null string");
					
					/*get molecule type for A*/
					ppp_tofill->atype = f4long(pf4_POINT_ATYPE);
					
					/*get source DNA gi for A*/
					ppp_tofill->adgi = 0;
					
					/*get source DNA 'from' location for A*/
					ppp_tofill->adf = 0;
					
					/*get source DNA 'to' location for A*/
					ppp_tofill->adt = 0;
					
					/*get gi of B*/
					ppp_tofill->gib = f4long(pf4_POINT_GIB);
					
					/*get name list for B*/
					ppp_tofill->namb = StrSave(f4str(pf4_POINT_NAMB));
					if (ppp_tofill->namb == NULL) ppp_tofill->namb = StrSave("NOT FOUND");
					if(TrimSpaces(ppp_tofill->namb) != 0)
						ErrPostEx(SEV_ERROR,0,0, "GetListofPointsby2GIs: TrimSpaces failed, possibly passed a null string");
					/*examine for troubled names here*/
					
					/*get molecule type for B*/
					ppp_tofill->btype = f4long(pf4_POINT_BTYPE);
					
					/*get source DNA gi for B*/
					ppp_tofill->bdgi = 0;
					
					/*get source DNA 'from' location for B*/
					ppp_tofill->bdf = 0;
					
					/*get source DNA 'to' location for B*/
					ppp_tofill->bdt = 0;
					
					/*get opinion count for NO this is not an interaction*/
					ppp_tofill->opno = f4long(pf4_POINT_OPNO);
					
					/*get opinion count for YES this is an interaction*/
					ppp_tofill->opyes = f4long(pf4_POINT_OPYES);
					
					/*get potential interaction score 1*/
					ppp_tofill->piscore = f4long(pf4_POINT_PISCORE);
					
					/*get potential interaction score 2*/
					ppp_tofill->piscore2 = f4long(pf4_POINT_PISCORE2);
					
					/*get interaction truth*/
					ppp_tofill->pitruth = f4long(pf4_POINT_PITRUTH);
					
					/*get the next node to fill and move the 'to fill' pointer to it*/
					ppp_tofill->next = PREBINDPointNew();
					ppp_lastfilled = ppp_tofill; /*keep track of the last node filled*/
					if (ppp_tofill->next != NULL)
					{
						ppp_tofill = ppp_tofill->next;
					}
					else
					{
						ErrPostEx(SEV_ERROR,0,0, "GetListofPointsby2GIs: Failed to allocate memory for a new list node");
						if (ppsp) ppsp = MemFree(ppsp);
						return NULL;
					}
				}/*only if the point is to be added to the list*/
				
			}/*only if this record corresponds to gi a and gib*/

			/*reset variables*/
			add_to_list = TRUE;
			pmid = 0;
			second_gi = 0;
				 
			/*look for the next POINT record*/
			point_record = d4seekNextDouble(pd4_POINT, gia);
			
		}/*while another POINT record is found by pmid*/

	}/*for loop*/

	/*if the list contains nothing then return NULL*/
	if (ppp_head == ppp_tofill)
	{
		if (ppsp) ppsp = MemFree(ppsp);/*no matches for queried gi found in either gia or gib column*/
	}

	/*cap the list*/
	if (ppp_lastfilled)
	{
		if (ppp_lastfilled->next) MemFree(ppp_lastfilled->next);
		ppp_lastfilled->next = NULL;
	}

	return ppsp;
}



/************************************************************/
/*															*/
/*	GetListofPointsbyPMID:									*/
/*			Given a PubMed ID, this function returns a list	*/
/*			of all the POtential INTeractions present in the*/
/*			paper from the POINT database					*/
/*	PARAMETERS:												*/
/*			Int 4	pmid									*/
/*	RETURNS:												*/
/*			PREBINDPointSetPtr	ptr to a list of POINT		*/
/*								records						*/
/************************************************************/
PREBINDPointSetPtr GetListofPointsbyPMID(Int4 pmid)
{	
	PREBINDPointSetPtr	ppsp =			NULL;
	PREBINDPointPtr		ppp_head =		NULL;
	PREBINDPointPtr		ppp_tofill =	NULL;
	PREBINDPointPtr		ppp_lastfilled= NULL;
	Int2				point_record =	0;

	/*start a new PREBIND Point Set*/
	ppsp = PREBINDPointSetNew();
	ppp_head = PREBINDPointNew();
	ppsp->records = ppp_head;
	ppp_tofill = ppp_head;

	/*find a POINT record with the pmid number passed to this function*/
	/*select pmid tag*/
	d4tagSelect(pd4_POINT, pt4_POINT_PMID);

	point_record = d4seekDouble(pd4_POINT, pmid);
	if (point_record != 0)
	{
		return NULL; /*there are no POINT records for the pmid*/
	}
	while (point_record == 0) /*r4 success*/
	{
		/*fill ppp_tofill with data*/
		
		/*already have pmid*/
		ppp_tofill->pmid = pmid;

		/*get muid*/
		ppp_tofill->muid = f4long(pf4_POINT_MUID);

		/*get gi of A*/
		ppp_tofill->gia = f4long(pf4_POINT_GIA);

		/*get name list for A*/
		ppp_tofill->nama = StrSave(f4str(pf4_POINT_NAMA));
		if (ppp_tofill->nama == NULL) ppp_tofill->nama = StrSave("NOT FOUND");
		if(TrimSpaces(ppp_tofill->nama) != 0)
			ErrPostEx(SEV_ERROR,0,0, "GetListofPointsbyPMID: TrimSpaces failed, possibly passed a null string");
		
		/*get molecule type for A*/
		ppp_tofill->atype = f4long(pf4_POINT_ATYPE);

		/*get source DNA gi for A*/
		ppp_tofill->adgi = 0;

		/*get source DNA 'from' location for A*/
		ppp_tofill->adf = 0;

		/*get source DNA 'to' location for A*/
		ppp_tofill->adt = 0;

		/*get gi of B*/
		ppp_tofill->gib = f4long(pf4_POINT_GIB);

		/*get name list for B*/
		ppp_tofill->namb = StrSave(f4str(pf4_POINT_NAMB));
		if (ppp_tofill->namb == NULL) ppp_tofill->namb = StrSave("NOT FOUND");
		if(TrimSpaces(ppp_tofill->namb) != 0)
			ErrPostEx(SEV_ERROR,0,0, "GetListofPointsbyPMID: TrimSpaces failed, possibly passed a null string");

		/*get molecule type for B*/
		ppp_tofill->btype = f4long(pf4_POINT_BTYPE);

		/*get source DNA gi for B*/
		ppp_tofill->bdgi = 0;

		/*get source DNA 'from' location for B*/
		ppp_tofill->bdf = 0;

		/*get source DNA 'to' location for B*/
		ppp_tofill->bdt = 0;

		/*get opinion count for NO this is not an interaction*/
		ppp_tofill->opno = f4long(pf4_POINT_OPNO);

		/*get opinion count for YES this is an interaction*/
		ppp_tofill->opyes = f4long(pf4_POINT_OPYES);

		/*get potential interaction score 1*/
		ppp_tofill->piscore = f4long(pf4_POINT_PISCORE);

		/*get potential interaction score 2*/
		ppp_tofill->piscore2 = f4long(pf4_POINT_PISCORE2);

		/*get interaction truth*/
		ppp_tofill->pitruth = f4long(pf4_POINT_PITRUTH);

		/*get the next node to fill and move the 'to fill' pointer to it*/
		ppp_tofill->next = PREBINDPointNew();
		ppp_lastfilled = ppp_tofill; /*keep track of the last node filled*/
		if (ppp_tofill->next != NULL)
		{
			ppp_tofill = ppp_tofill->next;
		}

		/*look for the next POINT record*/
		point_record = d4seekNextDouble(pd4_POINT, pmid);

	}/*while another POINT record is found by pmid*/
	if (ppp_lastfilled->next) MemFree(ppp_lastfilled->next);
	ppp_lastfilled->next = NULL;

	return ppsp;
}


/************************************************************/
/*															*/
/*	PrintListofPointsForPMID								*/
/*		Given a PubMed Identifier (pmid) this function		*/
/*		returns a list of all of the potential pairwise		*/
/*		interactions that might be talked about in the paper*/
/*		from the points database.							*/
/*															*/
/*	PARAMETERS:												*/
/*		Int4	pmid										*/
/*		Int4	gia											*/
/*		Int4	gib											*/
/*	RETURNS:												*/
/*		0	ok												*/
/************************************************************/
Int2	PrintListofPointsForPMID(Int4 pmid, Int4 gia, Int4 gib, Int2 frames_ready)
{
	PREBINDPointSetPtr	ppsp =		NULL;
	PREBINDPointPtr		ppp =		NULL;
	PREBINDAccPtr		pap =			NULL;
	CharPtr				descr =			NULL;
	Char				int_descr[100];
	
	
	/*print out the mime type*/
	fprintf (stdout, "Content-type:text/html\r\n\r\n");
	
	
	/*print out the frames if they have not been printed already*/
	if (frames_ready == 0)
	{
		/*print  html header for page*/
		WWWPrintFileData(PMIDPOINTHEADER,  stdout);
		
		fprintf (stdout, "<frameset frameborder=\"yes\" framespacing=\"6\" border=\"2\" rows=\"50%%,*\">\n");
		fprintf (stdout,	"<frame src=\"http://www.ncbi.nlm.nih.gov/entrez/query.fcgi?cmd=Retrieve&db=PubMed&list_uids=%ld&dopt=Citation&tool=prebind\"  name=\"abstract\" frameborder=\"yes\"> \n", (long) pmid);
		fprintf (stdout,	"<frame src=\"%s?search_type=points&pmid=%ld&gia=%ld&gib=%ld&frames=1\" name=\"points\"  frameborder=\"yes\" marginwidth=\"10\" scrolling=\"yes\"> \n", cgi_path, (long) pmid, (long) gia, (long) gib);
		fprintf (stdout,	"<noframes>\n");
		fprintf (stdout,		"<body bgcolor=\"white\" topmargin=\"0\" leftmargin=\"0\">\n");
		fprintf (stdout,			"Your browser does not support frames.  To see the results of your search, click <a href=\"%s?search_type=points&pmid=%ld&gia=%ld&gib=%ld&frames=1\" name=\"points\" scrolling=\"yes\" frameborder=\"yes\">here.</a>marginwidth=\"10\"\n", cgi_path, (long) pmid, (long) gia, (long) gib);
		fprintf (stdout,		"</body>\n");
		fprintf (stdout,	"</noframes>\n");
		fprintf (stdout, "</frameset>\n");
		fprintf (stdout, "</html>\n");
		
	}
	else if (frames_ready == 1)
	{
		
		/*print the pmid of the interaction paper to which the info below applies*/
		WWWPrintFileData(PMIDPOINTHEADER,  stdout);
		fprintf (stdout,"<body bgcolor=\"white\">\n");
		
		fprintf (stdout,"<table width=\"100%%\" border=\"0\"  cellspacing=\"4\" cellpadding=\"2\" cols=\"16\" >\n");
		fprintf (stdout,"<tr bgcolor=\"lightsteelblue\">\n");
		fprintf (stdout,"	<td colspan=\"16\" height=\"20\">\n");
		fprintf (stdout,"		<h3>Review potential pairwise interactions.");
		fprintf	(stdout,"		(View this abstract at\n");
		fprintf (stdout,"		<a target=\"abstract\" onmouseover=\"status='View abstract at PubMed'\"href=\"http://www.ncbi.nlm.nih.gov/htbin-post/Entrez/query?db=m&form=6&uid=%ld&Dopt=r&tool=EntrezLinkDoc\">PubMed</a>\n", (long) pmid);
		fprintf (stdout,"		or using\n");	
		fprintf (stdout,"		<a target=\"abstract\" onmouseover=\"status='View abstract using Textomy'\"href=\"http://textomy.iit.nrc.ca/cgi-bin/bindpresent.cgi?qry=%ld\">Textomy). </a>\n", (long) pmid);		
		/*http://textomy.iit.nrc.ca/cgi-bin/bindpresent.cgi?qry=10485845*/
		fprintf (stdout,"		</h3>");
		fprintf (stdout,"	</td>\n");
		fprintf (stdout,"</tr>\n\n");
		fprintf (stdout,"<tr>\n");
		fprintf (stdout,"	<td colspan=\"16\" height=\"20\">\n");
		fprintf (stdout,"		The table below lists all of the potential, pairwise interactions between protein names found in this abstract.  Natural language processing was used to determine if the interaction was mentioned in the abstract (see the column \"Involved in an interaction?\" and the PreBIND Help page for more details).");
		fprintf (stdout,"		Use the \"GO\" button to submit feedback about this interaction to BIND.  Your input is appreciated.<br>\n");
		fprintf (stdout,"	</td>\n");
		fprintf (stdout,"</tr>\n\n");

		/*try to get a list of points for the PubMed Abstract*/
		ppsp = GetListofPointsbyPMID(pmid);
		if (ppsp == NULL)
		{
			fprintf(stdout, "There are no potential interactions listed in PreBIND for this paper.\n");
			/*print footer*/
			WWWPrintFileData(FOOTERFILE,  stdout);
			fflush(stdout);
			return 0;
		}
		else 
		{
			ppp = ppsp->records;
			if (ppp == NULL)
			{
				fprintf(stdout, "There are no potential interactions listed in PreBIND for this paper.\n");
				/*print footer*/
				WWWPrintFileData(FOOTERFILE,  stdout);
				fflush(stdout);
				return 0;
			}
		}
		
		/*the potential interactions are printed as a nested table in this table*/
		fprintf (stdout,"<tr><td>\n");
		fprintf (stdout,"<table width=\"100%%\" bordercolor=\"white\" border=\"2\"  cellspacing=\"2\" cellpadding=\"10\" cols=\"16\">\n");
		/*print out the column headings for the list of points for this pmid*/
		WWWPrintFileData(PMIDPOINTTOP, stdout);

		/*print out data retrieved from the POINTS database */
		while (ppp != NULL) 
		{
			/*print out the results of the current record in one row of a table*/
			fprintf(stdout,"<!--beginning of new row in table-->\n");
			fprintf(stdout,"<tr width=\"100%%\" valign=top>\n");

			/*print the link to review interactions in this paper*/
			fprintf(stdout,"	<td  colspan=\"2\" height=\"20\">\n");	
			fprintf (stdout,"		<form method=\"POST\" action=\"%s\">\n", cgi_path);	
			fprintf (stdout,"			<input type=\"submit\" value=\"GO\">\n");
			fprintf (stdout,"			<input type=hidden name=pmid value=\"%ld\">\n", (long) ppp->pmid);
			fprintf (stdout,"			<input type=hidden name=gia value=\"%ld\">\n", (long) ppp->gia);
			fprintf (stdout,"			<input type=hidden name=gib value=\"%ld\">\n", (long) ppp->gib);
			fprintf (stdout,"			<input type=hidden name=search_type value=\"review\">\n");
			fprintf (stdout,"		</form>\n");
			fprintf(stdout,"	</td>\n");

			/*describe what is known about the likelihood of the interaction- report truth over opinion over scores*/
			if (ppp->pitruth != 0) /*truth*/
			{
				if (ppp->pitruth == -1)/*there is no interaction and this fact has been reviewed*/
				{
					StrCpy(int_descr, "No<br>Reviewed.");
				}
				if (ppp->pitruth == 1)/*there is an interaction and this fact has been reviewed*/
				{
					StrCpy(int_descr, "Yes<br>Reviewed.");
				}
			}
			else if (ppp->opyes != 0 || ppp->opno != 0)/*opinion*/
			{
				if (ppp->opyes >0 && ppp->opno == 0)
				{
					StrCpy(int_descr, "Yes<br>Under review.");
				}
				else if (ppp->opno >0 && ppp->opyes == 0)
				{
					StrCpy(int_descr, "No<br>Under review.");
				}
				else if (ppp->opno >0 && ppp->opyes > 0)
				{
					StrCpy(int_descr, "Unknown<br>Under review.");
				}
			}
			else if (ppp->piscore > 0 || ppp->piscore2 > 0)/*scores from natural language processing*/
			{	
				if (ppp->piscore > 0 && ppp->piscore2 == 0) /*it may be a protein::protein interaction*/
				{
					StrCpy(int_descr, "Possible<br>(protein-protein)<br>Not reviewed.");
				}
				else if (ppp->piscore2 > 0 && ppp->piscore == 0) /*it may be a protein::DNA interaction*/
				{
					StrCpy(int_descr, "Possible<br>(protein-DNA)<br>Not reviewed.");
				}
				else if (ppp->piscore > 0 && ppp->piscore2 > 0) /*both ? */
				{
					StrCpy(int_descr, "Possible<br>(unknown type)<br>Not reviewed.");
				}
			}
			else 
			{
				StrCpy(int_descr, "Unknown<br>Not reviewed.");	
			}
			
			/*print the description of the interaction likelihood*/
			fprintf(stdout,"	<td  colspan=\"2\" height=\"20\">\n");
			fprintf(stdout,"		%s\n", int_descr);
			fprintf(stdout,"	</td>\n");
			
			/*print info for molecule A*/
			
			/*print out the name(s) that the gene product is referred to by in the paper*/
			fprintf(stdout,"	<td colspan=\"1\" height=\"20\">\n");
			fprintf(stdout,"		%s\n", ppp->nama);
			fprintf(stdout,"	</td>\n");
			
			/*get a description of the gene product*/
			pap = GetAccessionRecordbyGI(ppp->gia);
			if (pap != NULL)
			{
				descr = StrSave(pap->pprod);
				if (descr == NULL) descr = StrSave("not available");
				/*print the description of the gene product*/
				fprintf(stdout,"	<td colspan=\"4\" height=\"20\">\n");
				fprintf(stdout,"		%s\n", descr);
				fprintf(stdout,"	</td>\n");
				/*print the taxon of the molecule*/
				fprintf(stdout,"	<td colspan=\"1\" height=\"20\">\n");
				fprintf(stdout,"		%ld\n", (long) pap->tax);
				fprintf(stdout,"	</td>\n");
			}
			else if (pap == NULL)
			{
				fprintf(stdout,"	<td colspan=\"4\" height=\"20\">\n");
				fprintf(stdout,"		data not found\n");
				fprintf(stdout,"	</td>\n");
				fprintf(stdout,"	<td colspan=\"1\" height=\"20\">\n");
				fprintf(stdout,"		N/A\n");
				fprintf(stdout,"	</td>\n");
			}
			
			
			/*print info for molecule B*/
			
			/*print out the name(s) that the gene product is referred to by in the paper*/
			fprintf(stdout,"	<td colspan=\"1\" height=\"20\">\n");
			fprintf(stdout,"		%s\n", ppp->namb);
			fprintf(stdout,"	</td>\n");
			
			/*get a description of the gene product*/
			pap = GetAccessionRecordbyGI(ppp->gib);
			if (pap != NULL)
			{
				descr = StrSave(pap->pprod);
				if (descr == NULL) descr = StrSave("not available");
				/*print the description of the gene product*/
				fprintf(stdout,"	<td colspan=\"4\" height=\"20\">\n");
				fprintf(stdout,"		%s\n", descr);
				fprintf(stdout,"	</td>\n");
				/*print the taxon of the molecule*/
				fprintf(stdout,"	<td  colspan=\"1\" height=\"20\">\n");
				fprintf(stdout,"		%ld\n", (long) pap->tax);
				fprintf(stdout,"	</td>\n");
			}
			else if (pap == NULL)
			{
				fprintf(stdout,"	<td colspan=\"4\" height=\"20\">\n");
				fprintf(stdout,"		data not found\n");
				fprintf(stdout,"	</td>\n");
				fprintf(stdout,"	<td colspan=\"1\" height=\"20\">\n");
				fprintf(stdout,"		N/A\n");
				fprintf(stdout,"	</td>\n");
			}
			
							
			/*finish the row and print an empty row*/
			fprintf(stdout,"</tr>\n");
			fprintf(stdout,"<tr width=\"100%%\"><td colspan=\"16\"><br></td></tr><!--empty row in table-->\n\n");

		
			/*go to the next node in the list*/
			ppp = ppp->next;
		}

		/*finish printing out the table and page*/
		fprintf(stdout, "</table><p>\n");
		fprintf(stdout, "</td><p>\n");
		fprintf(stdout, "</table><p>\n");
		WWWPrintFileData(FOOTERFILE,  stdout);
		fflush(stdout); 

		/*MemFree*/
		if (ppsp) MemFree(ppsp);

	}

	return 0;

}


/************************************************************/
/*															*/
/*	PrintListofEvidenceSupportingInteractor					*/
/*		Given a ValNode list of the same					*/
/*		POtential INTeraction as it appears in a number		*/
/*		of papers this function prints out a single row for	*/
/*		each supporting paper along with confidence values	*/
/*															*/						
/*	PARAMETERS:												*/
/*		ValNodePtr	vnp_head: ptr to a list of Point records*/
/*		Int4		gii: gi of the interactor				*/
/*	RETURNS:												*/
/*		0	ok												*/
/************************************************************/
Int2	PrintListofEvidenceSupportingInteractor(ValNodePtr vnp_head, Int4 gi, Int4 gii)
{
	Int2			report =			0;
	ValNodePtr		vnp =				0;
	PREBINDPointPtr	ppp =				NULL;
	PREBINDAccPtr	pap =				NULL;
	CharPtr			int_descr =			NULL;
	CharPtr			paper_descr =		NULL;
	PREBINDIsvmPtr	pisvmp =			NULL;
	ValNodePtr		vnp_name =			NULL;
	ValNodePtr		vnp_unofficial_A =	NULL;
	ValNodePtr		vnp_redundant_A =	NULL;
	ValNodePtr		vnp_unofficial_B =	NULL;
	ValNodePtr		vnp_redundant_B =	NULL;	
	Int2			troubled_name_present = 0;
	
	if (vnp_head == NULL || gii <= 0)
	{
		/*error*/
		ErrPostEx(SEV_ERROR,0,0, "PrintListofEvidenceSupportingInteractor: Invalid function parameters");
		report = -1;
		return report;
	}

	
	/*print the table row headings*/
	fprintf (stdout,"<table border=0 width=100%%  cellspacing=2 cellpadding=2 cols=1>");
	fprintf(stdout,"<tr>\n");

	fprintf(stdout,"<td bgcolor=\"lightgoldenrodyellow\" width=\"10%%\" height=\"20\">\n");
	fprintf(stdout,"Review this paper");
	fprintf(stdout,"</td>\n");	
	
	/*note: the INTERACTEE is always printed first (Molecule A)*/
	fprintf(stdout,"<td bgcolor=\"lightgoldenrodyellow\" width=\"20%%\" height=\"20\">\n");
	fprintf(stdout,"<strong>Molecule A</strong><br>name(s) appearing in abstract");
	fprintf(stdout,"</td>\n");

	/*note: the INTERACTOR is always printed second (Molecule B)*/
	fprintf(stdout,"<td bgcolor=\"lightgoldenrodyellow\" width=\"20%%\" height=\"20\">\n");
	fprintf(stdout,"<strong>Molecule B</strong><br>name(s) appearing in abstract");
	fprintf(stdout,"</td>\n");
				
	fprintf(stdout,"<td bgcolor=\"lightgoldenrodyellow\" width=\"10%%\" height=\"20\">\n");
	fprintf(stdout,"Is this interaction <br>real?");
	fprintf(stdout,"</td>\n");
				
	fprintf(stdout,"<td bgcolor=\"lightgoldenrodyellow\" width=\"20%%\" height=\"20\">\n");
	fprintf(stdout,"Is this an <br>interaction paper?");
	fprintf(stdout,"</td>\n");

	fprintf(stdout,"<td bgcolor=\"lightgoldenrodyellow\" width=\"20%%\" height=\"20\">\n");
	fprintf(stdout,"svm score");
	fprintf(stdout,"</td>\n");				

	fprintf(stdout,"<tr>\n");	
	fflush(stdout);
	
	/*get list of redundant and unofficial names for the interactor (Molecule B) */
	vnp_unofficial_A = GetListofUnofficialNamesbyGI(gi);
	vnp_redundant_A = GetListofRedundantNamesbyGI(gi);
	
	/*get list of redundant and unofficial names for the interactor (Molecule B) */
	vnp_unofficial_B = GetListofUnofficialNamesbyGI(gii);
	vnp_redundant_B = GetListofRedundantNamesbyGI(gii);

	
	/*print contents of the table*/
	vnp = vnp_head;


	while (vnp)
	{
		ppp = vnp->data.ptrvalue;
		
		if (ppp)
		{
			
			/*print out the results of the current record in one row of a table*/
			fprintf(stdout,"<tr valign=top>\n");

			/*print the link to review the interaction in this paper*/
			fprintf(stdout,"<td width=\"10%%\" height=\"20\">\n");	
			fprintf (stdout, "<form method=\"POST\" action=\"%s\" target=\"display\"> \n", cgi_path);	
			fprintf (stdout, "<input type=\"submit\" value=\"GO\">\n");
			fprintf (stdout, "<input type=hidden name=pmid value=\"%ld\">\n", (long) ppp->pmid);
			fprintf (stdout, "<input type=hidden name=gia value=\"%ld\">\n", (long) ppp->gia);
			fprintf (stdout, "<input type=hidden name=gib value=\"%ld\">\n", (long) ppp->gib);
			fprintf (stdout, "<input type=hidden name=frames value=\"0\">\n");
			fprintf (stdout, "<input type=hidden name=search_type value=\"points\">\n");
			fprintf (stdout, "</form>");
			fprintf(stdout,"</td>\n");

			/*print out the name(s) that the interactee (Molecule A) is referred to by in the paper*/
			fprintf(stdout,"<td width=\"20%%\" height=\"20\">\n");
			if (gi == ppp->gia) 
			{
				if (DescribeNameList(ppp->nama, vnp_unofficial_A, vnp_redundant_A)) troubled_name_present = 1;
			}
			else if (gi == ppp->gib) 
			{
				if (DescribeNameList(ppp->namb, vnp_unofficial_A, vnp_redundant_A)) troubled_name_present = 1;

			}
			fprintf(stdout,"</td>\n");

			/*print out the name(s) that the interactor (Molecule B) is referred to by in the paper*/
			fprintf(stdout,"<td width=\"20%%\" height=\"20\">\n");
			if (gii == ppp->gia) 
			{
				if (DescribeNameList(ppp->nama, vnp_unofficial_B, vnp_redundant_B)) troubled_name_present = 1;
			}
			else if (gii == ppp->gib) 
			{
				if (DescribeNameList(ppp->namb, vnp_unofficial_B, vnp_redundant_B)) troubled_name_present = 1;
			}
			fprintf(stdout,"</td>\n");
							
			/*descibe what is known about the likelihood of the interaction- report truth over opinion over scores*/
			int_descr =	DescribeInteractionLikelihood(ppp);	
			
			/*print the description of the interaction likelihood*/
			fprintf(stdout,"<td width=\"20%%\" height=\"20\">\n");
			fprintf(stdout,"%s", int_descr);
			fprintf(stdout,"</td>\n");

			/*get a description of this paper from the isvm data file*/
			pisvmp = GetIsvmbyPMID(ppp->pmid);

			/*compose a description of the likelihood that this paper describes an interaction*/
			paper_descr = DescribeInteractionPaperLikelihood(pisvmp);

			/*print the description of the interaction paper likelihood*/
			fprintf(stdout,"<td width=\"10%%\" height=\"20\">\n");
			fprintf(stdout,"%s", paper_descr);
			fprintf(stdout,"</td>\n");

			/*print the description of the interaction likelihood*/
			fprintf(stdout,"<td width=\"20%%\" height=\"20\">\n");
			fprintf(stdout,"%f", pisvmp->iscore);
			fprintf(stdout,"</td>\n");
				
			/*finish the row*/
			fprintf(stdout,"</tr>\n");
			fflush(stdout); 

			/*free mem*/
			if (int_descr) int_descr = MemFree(int_descr);
			if (paper_descr) paper_descr = MemFree(paper_descr);
		
		}/*if ppp*/
		else
		{
			/*do nothing*/
		}
		
		vnp = vnp->next;

	}/*while there is another pointer to a POINT*/


	/*finish the table*/
	fprintf (stdout,"</table>");				
	fprintf (stdout,"<BR>\n");
	fflush(stdout);

	/*explanation of redundant or unofficial names*/
	if (troubled_name_present)
	{
		fprintf(stdout,"* = at least one of these names is unofficial.  This result should be viewed with caution.\n");
		fprintf(stdout,"<br>\n");
		fprintf(stdout,"! = at least one of these names is used to describe more than one protein in this taxon.  This result should be viewed with caution.\n");
		fprintf(stdout,"<br><br>\n");
		fflush(stdout);
	}
	
	return report;
}




/************************************************************/
/*															*/
/*	PrintListofPointsForGI									*/
/*		Given a pointer to a list of POtential INTeractions	*/
/*		in a paper, this function prints out only the 		*/
/*		interacting partners in the list that interact with */
/*		the gene product specified by the gi passed to the  */
/*		function											*/
/*	PARAMETERS:												*/
/*		PREBINDPointSetPtr ppsp:							*/
/*			ptr to a list of PREBIND Point records			*/
/*	RETURNS:												*/
/*		0	ok												*/
/************************************************************/
Int2	PrintListofPointsForGI(PREBINDPointSetPtr ppsp, Int4 referring_gi)
{

	PREBINDAccPtr		pap =			NULL;
	CharPtr				descr =			NULL;
	Char				int_descr[100];


	
	PREBINDPointPtr	ppp = NULL;
	
	if (ppsp == NULL)
	{
		return 1;
	}
	else 
	{
		ppp = ppsp->records;
		if (ppp == NULL)
		{
			return 2;
		}
	}

	
	/*print out the top of the table for the list of searches*/
	WWWPrintFileData(GIPOINTTOP, stdout);

	while (ppp != NULL) 
	{

		/*print out the results of the current record in one row of a table*/
		fprintf(stdout,"<tr>\n");


		/*only names of gene products other than the referring gi are printed out*/
		if (referring_gi == ppp->gia || referring_gi == ppp->gib)
		{

			/*print out the results of the current record in one row of a table*/
			fprintf(stdout,"<tr valign=top>\n");

			if (referring_gi == ppp->gib) /*then print info for molecule A*/
			{
				/*print out the name(s) that the gene product is referred to by in the paper*/
				fprintf(stdout,"<td width=\"25%%\" height=\"20\">\n");
				fprintf(stdout,"%s", ppp->nama);
				fprintf(stdout,"</td>\n");
				
				/*get a description of the gene product*/
				pap = GetAccessionRecordbyGI(ppp->gia);
				if (pap != NULL)
				{
					descr = StrSave(pap->pprod);
					if (descr == NULL) descr = StrSave("not available");
					/*print the description of the gene product*/
					fprintf(stdout,"<td width=\"25%%\" height=\"20\">\n");
					fprintf(stdout,"%s", descr);
					fprintf(stdout,"</td>\n");
				}
			}
			else /*print info for molecule B*/
			{
				/*print out the name(s) that the gene product is referred to by in the paper*/
				fprintf(stdout,"<td width=\"25%%\" height=\"20\">\n");
				fprintf(stdout,"%s", ppp->namb);
				fprintf(stdout,"</td>\n");

				/*get a description of the gene product*/
				pap = GetAccessionRecordbyGI(ppp->gib);
				if (pap != NULL)
				{	
					descr = StrSave(pap->pprod);
					if (descr == NULL) descr = StrSave("not available");
					/*print the description of the gene product*/
					fprintf(stdout,"<td width=\"25%%\" height=\"20\">\n");
					fprintf(stdout,"%s", descr);
					fprintf(stdout,"</td>\n");
				}
			}


			/*descibe what is known about the likelihood of the interaction- report truth over opinion over scores*/
			if (ppp->pitruth != 0) /*truth*/
			{
				if (ppp->pitruth == -1)/*there is no interaction and this fact has been reviewed*/
				{
					StrCpy(int_descr, "No<br>Reviewed.");
				}
				if (ppp->pitruth == 1)/*there is an interaction and this fact has been reviewed*/
				{
					StrCpy(int_descr, "Yes<br>Reviewed.");
				}
			}
			else if (ppp->opyes != 0 || ppp->opno != 0)/*opinion*/
			{
				if (ppp->opyes >0 && ppp->opno == 0)
				{
					StrCpy(int_descr, "Yes<br>Under review.");
				}
				else if (ppp->opno >0 && ppp->opyes == 0)
				{
					StrCpy(int_descr, "No<br>Under review.");
				}
				else if (ppp->opno >0 && ppp->opyes > 0)
				{
					StrCpy(int_descr, "Unknown<br>Under review.");
				}
			}
			else if (ppp->piscore > 0 || ppp->piscore2 > 0)/*scores*/
			{	
				if (ppp->piscore > 0 && ppp->piscore2 == 0) /*it may be a protein::protein interaction*/
				{
					StrCpy(int_descr, "Possible<br>(protein-protein)<br>Not reviewed.");
				}
				else if (ppp->piscore2 > 0 && ppp->piscore == 0) /*it may be a protein::DNA interaction*/
				{
					StrCpy(int_descr, "Possible<br>(protein-DNA)<br>Not reviewed.");
				}
				else if (ppp->piscore > 0 && ppp->piscore2 > 0) /*both ? */
				{
					StrCpy(int_descr, "Possible<br>(unknown type)<br>Not reviewed.");
				}
			}
			else 
			{
				StrCpy(int_descr, "Unknown");	
			}

			
			/*print the description of the interaction likelihood*/
			fprintf(stdout,"<td width=\"25%%\" height=\"20\">\n");
			fprintf(stdout,"%s", int_descr);
			fprintf(stdout,"</td>\n");

			/*print the link to review the interaction in this paper*/
			fprintf(stdout,"<td width=\"25%%\" height=\"20\">\n");	
			fprintf (stdout, "<form method=\"POST\" action=\"%s\" target=\"display\"> \n", cgi_path);	
			fprintf (stdout, "<input type=\"submit\" value=\"GO\">\n");
			fprintf (stdout, "<input type=hidden name=pmid value=\"%ld\">\n", (long) ppp->pmid);
			fprintf (stdout, "<input type=hidden name=gia value=\"%ld\">\n", (long) ppp->gia);
			fprintf (stdout, "<input type=hidden name=gib value=\"%ld\">\n", (long) ppp->gib);
			fprintf (stdout, "<input type=hidden name=frames value=\"0\">\n");
			fprintf (stdout, "<input type=hidden name=search_type value=\"points\">\n");
			fprintf (stdout, "</form>");
			fprintf(stdout,"</td>\n");

		}

		/*finish the row*/
		fprintf(stdout,"</tr>\n");
		
		/*go to the next node in the list*/
		ppp = ppp->next;
	}

	/*finish printing out the table html*/
	fprintf(stdout, "</table><p>\n");

	fflush(stdout); 

	return 0;
}

/************************************************************/
/*															*/
/*	PrintSummaryAccessionRecord								*/
/*		Prints a summary of an accession record given a		*/
/*		pointer to an accession record struct				*/
/*		Format is for html output and includes cgi links to */
/*		full accession records								*/
/*	PARAMETERS:												*/
/*		PREBINDAccPtr pap:									*/
/*			ptr to PREBIND ACCession record struct			*/
/*	RETURNS:												*/
/*		0	ok												*/
/************************************************************/
Int2 PrintSummaryAccessionRecord(PREBINDAccPtr pap)
{
	CharPtr taxname = NULL;

	/*get taxname*/
	taxname = GetTaxNameByTaxId(pap->tax);
	if (taxname == NULL)
	{
		/*a tax name cannot be found*/
		ErrPostEx(SEV_ERROR,0,0,"PrintSummaryAccessionRecord: Unable to find a taxon name.");
	}

	if (pap != NULL)
	{
		fprintf (stdout, "<form method=\"POST\" action=\"%s\">\n", cgi_path);	
		fprintf (stdout, "<input type=\"submit\" value=\"GO\">\n");
		fprintf (stdout, "<input type=hidden name=accession value=\"%s\">\n", pap->acc);
		fprintf (stdout, "<input type=hidden name=sloi value=1>\n");
		fprintf (stdout, "<input type=hidden name=fnip value=1>\n");
		fprintf (stdout, "<input type=hidden name=search_type value=\"accession\">\n");
		fprintf (stdout, "  %s | gene product: %s | gi: %ld | taxon: %s <a href=\"javascript:void window.open('http://www.ncbi.nlm.nih.gov/htbin-post/Taxonomy/wgetorg?id=%ld', 'taxWin', 'toolbar=no,location=no,directories=no,status=yes,menubar=no,scrollbars=yes,resizable=yes,width=550,height=400');\" onmouseover=\"window.status='Go to taxonomy browser';return(true);\" onmouseout=\"window.status=''\">(%ld)</a>\n", pap->acc, pap->pprod, (long) pap->gi, (taxname != NULL)?taxname:"unknown name", (long) pap->tax, (long) pap->tax);		
		fprintf (stdout, "</form>");
	}

	return 0;
}


/************************************************************/
/*															*/
/*	PrintListofResults:										*/
/*			Given a ptr to a list of PREBIND Results, this	*/
/*			function prints the records as HTML formatted	*/
/*			text.											*/
/*	PARAMETERS:												*/
/*		PREBINDSearchSetPtr pssp:							*/
/*			ptr to list of PREBIND Search records			*/
/*	RETURNS:												*/
/*		0	ok												*/
/************************************************************/
Int2 PrintListofResults(PREBINDResultSetPtr prsp)
{
	PREBINDResultPtr	prp = NULL;
	ValNodePtr			vnp_head = NULL;
	ValNodePtr			vnp = NULL;
	
	if (prsp == NULL)
	{
		return 1;
	}
	else 
	{
		prp = prsp->records;
		if (prp == NULL)
		{
			return 2;
		}
	}
	
	/*fill the head*/
	vnp_head = ValNodeAddPointer(NULL, 1, prp);
	vnp = vnp_head;
	prp=prp->next;
				
	/*add  the rest of the list*/			
	while (prp && vnp)
	{
		vnp = ValNodeAddPointer(&vnp_head, 1, prp);
		prp = prp->next;
	}
	prp = NULL;
	vnp = NULL;
	
	/*sort the list*/
	VnpHeapSort(&vnp_head, CompareInteractionPapersFor2Results);
	
	/*Mem Free*/
	if (prsp) MemFree(prsp);


	/*print out the top of the table for the list of searches*/
	WWWPrintFileData(RESULTTOP, stdout);


	vnp = vnp_head;
	while (vnp) 
	{
		prp = vnp->data.ptrvalue;

		if (prp)
		{
			/*print out the results of the current record in one row of a column*/
			fprintf(stdout,"<tr>\n");
			
			/*search name*/
			fprintf(stdout,"<td width=\"15%%\" height=\"20\">\n");
			fprintf(stdout,"%s", prp->name);
			fprintf(stdout,"</td>\n");
			
			/*pubmed abstract number: print out with '*' if iscore is > 0 and '!' if iscore2 is > 0*/
			fprintf(stdout,"<td width=\"30%%\" height=\"20\">\n");
			fprintf(stdout,"<a target=\"ncbi\" onmouseover=\"status='View abstract at PubMed'\"href=\"http://www.ncbi.nlm.nih.gov/htbin-post/Entrez/query?db=m&form=6&uid=%ld&Dopt=r&tool=EntrezLinkDoc\">%ld</a>", (long) prp->pmid, (long) prp->pmid);
			if (prp->iscore  > 0   && prp->iscore < 0.5) fprintf(stdout, " *");
			if (prp->iscore >= 0.5 && prp->iscore < 1.0) fprintf(stdout, " * *");
			if (prp->iscore >= 1.0 && prp->iscore < 1.5) fprintf(stdout, " * * *");
			if (prp->iscore >= 1.5 && prp->iscore < 2.1) fprintf(stdout, " * * * *");
			if (prp->itruth == 1) fprintf(stdout, " * * * *");
			if (prp->iscore2 >0) fprintf(stdout, " !");
			/*fprintf(stdout,"%ld", (long) prp->pmid);*/
			fprintf(stdout,"</td>\n");
			
			/*an interaction paper?*/
			if (prp->itruth == -1)
			{
				fprintf(stdout,"<td width=\"30%%\" height=\"20\">\n");
				fprintf(stdout,"No- reviewed");
				fprintf(stdout,"</td>\n");
			}
			if (prp->itruth == 1)
			{			
				fprintf(stdout,"<td width=\"30%%\" height=\"20\">\n");
				fprintf(stdout,"Yes- reviewed");
				fprintf(stdout,"</td>\n");
			}
			if (prp->itruth == 0)
			{
				fprintf(stdout,"<td width=\"30%%\" height=\"20\">\n");
				fprintf(stdout,"%lf", prp->iscore);
				fprintf(stdout,"</td>\n");
			}
			
			
			/*go to list of points for this paper*/
			fprintf(stdout,"<td width=\"25%%\" height=\"20\">\n");	
			fprintf (stdout, "<form method=\"POST\" action=\"%s\">\n", cgi_path);	
			fprintf (stdout, "<input type=\"submit\" value=\"GO\">\n");
			fprintf (stdout, "<input type=hidden name=pmid value=\"%ld\">\n", (long) prp->pmid);
			fprintf (stdout, "<input type=hidden name=gia value=\"0\">\n");
			fprintf (stdout, "<input type=hidden name=gib value=\"0\">\n");
			fprintf (stdout, "<input type=hidden name=search_type value=\"points\">\n");
			fprintf (stdout, "</form>");
			fprintf(stdout,"</td>\n");
			
			/*end of the row*/
			fprintf(stdout,"</tr>\n");
			
		}

		/*go to the next node in the list*/
		vnp = vnp->next;

	}

	/*finish printing out the table html*/
	fprintf(stdout, "</table>\n");

	fflush(stdout); 

	return 0;
}




/************************************************************/
/*															*/
/*	PrintListofSearches:									*/
/*			Given a ptr to a list of PREBIND Searches, this	*/
/*			function prints the records as HTML formatted	*/
/*			text.											*/
/*	PARAMETERS:												*/
/*		PREBINDSearchSetPtr pssp:							*/
/*			ptr to list of PREBIND Search records			*/
/*	RETURNS:												*/
/*		max_number_search_results							*/
/************************************************************/
Int4 PrintListofSearches(PREBINDSearchSetPtr pssp)
{
	PREBINDSearchPtr	psp = NULL;
	Boolean unofficial_name_present = FALSE;
	Boolean redundant_name_present = FALSE;
	Int4 max_number_search_results =	0;


	if (pssp == NULL)
	{
		return -1;
	}
	else 
	{
		psp = pssp->records;
		if (psp == NULL)
		{
			return -2;
		}
	}

	/*print out the top of the table for the list of searches*/
	WWWPrintFileData(SEARCHTOP, stdout);

	while (psp != NULL && psp->name != NULL) 
	{
		/*print out the results of the current record in one row of a column*/
		fprintf(stdout,"<tr>\n");

		/*name*/
		fprintf(stdout,"<td width=\"30%%\" height=\"20\">\n");
		fprintf(stdout,"%s", psp->name);
		fprintf(stdout,"</td>\n");
		/*date last searched*/
		fprintf(stdout,"<td width=\"30%%\" height=\"20\">\n");
		fprintf(stdout,"%s\n", psp->searched);
		fprintf(stdout,"</td>\n");
		/*results*/
		fprintf(stdout,"<td width=\"30%%\" height=\"20\">\n");
		fprintf(stdout,"%d\n", psp->results);
		fprintf(stdout,"</td>\n");
		if (psp->results > max_number_search_results)
		{
			max_number_search_results = psp->results;
		}
		/*notes*/
		fprintf(stdout,"<td width=\"10%%\" height=\"20\">\n");
		if (psp->source < 0) /*the name is not an official name*/
		{
			fprintf(stdout,"* ");
			unofficial_name_present = TRUE;
		}
		if (psp->namered > 1)/*redundant name*/
		{
			fprintf(stdout,"!(%ld)",psp->namered);
			redundant_name_present = TRUE;	
		}
		fprintf(stdout,"</td>\n");

		fprintf(stdout,"</tr>\n");
		
		/*go to the next node in the list*/
		psp = psp->next;
	}

	/*finish printing out the table html*/
	fprintf(stdout, "</table>\n");
	fprintf(stdout,"<hr>\n");
	if (unofficial_name_present == TRUE)
	{
		fprintf(stdout,"* indicates that the listed name is not considered an official name for this protein.  Search results obtained using this name should be viewed with caution.");
	}
	if (redundant_name_present == TRUE)
	{
		fprintf(stdout, "<P>\n");
		fprintf(stdout,"!(n) indicates that the listed name is used to describe (n) proteins in this same organism.  Search results obtained using this name should be viewed with caution.");
	}
	fprintf(stdout, "<P><P>\n");
	fflush(stdout);
	
	return max_number_search_results;
}




/************************************************************/
/*															*/
/*	ResultFillNode:	given the head and the last node		*/
/*			filled in a linked list, this function fills the*/
/*			head node with record data (if last is NULL) or	*/
/*			appends a new node to last and adds data to that*/
/*			The function returns a pointer to the current	*/
/*			node (last node filled). This					*/
/*			function can be passed a NULL head parameter	*/
/*			(data is added to a node after last) or a NULL	*/
/*			'last' parameter (the head is filled and a ptr	*/
/*			to the head is returned as the last filled node.*/
/*															*/
/*	PARAMETERS:												*/
/*		PREBINDResultPtr head:	ptr to first node in linked	*/
/*								list						*/
/*		PREBINDResultPtr last:	ptr to the last filled node	*/
/*	RETURNS:												*/
/*		PREBINDResultPtr	pointer to the node in a linked */
/*							list that was just filled		*/
/*		NULL				Returns NULL if a pointer to the*/
/*							next empty node is NULL.		*/
/************************************************************/
PREBINDResultPtr ResultFillNode(PREBINDResultPtr head, PREBINDResultPtr last, Int4 gi, CharPtr name, Int4 result_pmid, FloatHi iscore, FloatHi iscore2, Int4 itruth, Int4 opno, Int4 opyes)
{
	PREBINDResultPtr newnode =	NULL;
	
	/*if last node was not specified (NULL), then fill the head*/
	if (last == NULL && head != NULL) 
	{
		newnode = head;
	}
	/*otherwise append a new node to last and fill that*/
	else if (last != NULL)
	{
		last->next = PREBINDResultNew();
		newnode = last->next;
	}
	
	/*fill up newnode and return a pointer to it */
	if (newnode != NULL)
	{
		newnode->gi = gi;
		newnode->name = StrSave(name);
		newnode->pmid = result_pmid;
		newnode->iscore = iscore;
		newnode->iscore2= iscore2;
		newnode->itruth = itruth;
		newnode->opno = opno;
		newnode->opyes = opyes;
		return newnode;
	}
	else
	{
		return NULL;
	}
	
}




/************************************************************/
/*															*/
/*	SearchFillNode:	given the head and the last node		*/
/*			filled in a linked list, this function fills the*/
/*			head node with record data (if last is NULL) or	*/
/*			appends a new node to last and adds data to that*/
/*			The function returns a pointer to the current	*/
/*			node (last node filled). This					*/
/*			function can be passed a NULL head parameter	*/
/*			(data is added to a node after last) or a NULL	*/
/*			'last' parameter (the head is filled and a ptr	*/
/*			to the head is returned as the last filled node.*/
/*															*/
/*	PARAMETERS:												*/
/*		PREBINDSearchPtr head:	ptr to first node in linked	*/
/*								list						*/
/*		PREBINDSearchPtr last:	ptr to the last filled node	*/
/*	RETURNS:												*/
/*		PREBINDSearchPtr	pointer to the node in a linked */
/*							list that was just filled		*/
/*		NULL				Returns NULL if a pointer to the*/
/*							next empty node is NULL.		*/
/************************************************************/

PREBINDSearchPtr SearchFillNode(PREBINDSearchPtr head, PREBINDSearchPtr last, Int4 gi, CharPtr name, Int4 source, CharPtr searched, Uint2 status, Int4 namered, Int4 results)
{
        PREBINDSearchPtr newnode = NULL;
		
		/*if last node was not specified (NULL), then fill the head*/
		if (last == NULL && head != NULL) 
		{
			newnode = head;
		}
		/*otherwise append a new node to last and fill that*/
		else if (last != NULL)
		{
			last->next = PREBINDSearchNew();
			newnode = last->next;
		}

		/*fill up newnode and return a pointer to it */
		if (newnode != NULL)
		{
			newnode->gi = gi;
			newnode->name = StrSave(name);
			newnode->source = source;
			newnode->searched = StrSave(searched);
			newnode->status = status;
			newnode->namered = namered;
			newnode->results = results;
			return newnode;
		}
		else
		{
			return NULL;
		}

}



/************************************************************/
/*															*/
/*	PadStringII:This function is given a string that is		*/
/*				padded out with spaces to the number of		*/
/*				characters in some field type pointed to by */ 
/*				pfield.  Memory for string must have been	*/
/*				pre-allocated to at least the size of the	*/
/*				the field length.							*/		 								
/*															*/
/*	PARAMETERS:												*/
/*		CharPtr accession:	accession number 				*/
/*		FIELD4  pfield:		pointer to a field				*/
/*	RETURNS:												*/
/*		0	ok												*/
/************************************************************/
Int2 PadStringII(FIELD4 *pfield, CharPtr string)
{
	Int4 field_length;
	Int2 string_length;
	Int2 x;

	field_length = f4len(pfield);
	string_length = StrLen(string);
	
	for (x = string_length; x < field_length; ++x)
	{
		string[x] = ' ';
	}

	string[x] ='\0';
	
	/*printf("%s\n", string);*/

	return 0;
}



/************************************************************/
/*															*/
/*	GetIsvmbyPMID:											*/
/*		This function gets av ISVM record 					*/
/*		given a pmid.										*/
/*															*/
/*	PARAMETERS:												*/
/*		Int4	pmid: PubMed Identifier						*/
/*	RETURNS:												*/
/*		PREBINDIsvmPtr		pisvmp							*/
/*		NULL				no results found for search		*/
/************************************************************/
PREBINDIsvmPtr GetIsvmbyPMID(Int4 pmid)
{

	Int2				isvm_record =			0;
	PREBINDIsvmPtr		pisvmp =				NULL;

	/*start a new PREBIND Isvm record*/
	pisvmp = PREBINDIsvmNew();
	if (!pisvmp)
	{
		pisvmp = NULL;
		ErrPostEx(SEV_ERROR,0,0, "GetIsvmbyPMID: Failed to allocate sufficient memory");
	}
	
	/*select the gi tag in the ISVM database*/
	d4tagSelect(pd4_ISVM, pt4_ISVM_PMID);
	
	/*look for a ISVM record with the same pmid*/
	isvm_record = d4seekDouble(pd4_ISVM, pmid);
	if (isvm_record != 0)
	{
		pisvmp = NULL;
	}
	else 
	{
		pisvmp->pmid = f4long(pf4_ISVM_PMID);
		pisvmp->muid = f4long(pf4_ISVM_MUID);
		pisvmp->iscore = f4double(pf4_ISVM_ISCORE);
		pisvmp->iscore2 = f4double(pf4_ISVM_ISCORE2);
		pisvmp->itruth = f4long(pf4_ISVM_ITRUTH);
		pisvmp->opno = f4long(pf4_ISVM_OPNO);
		pisvmp->opyes = f4long(pf4_ISVM_OPYES);
	}

	return pisvmp;

}



/************************************************************/
/*															*/
/*	GetListofResults:	This function gets a list of 		*/
/*						search results and their isvm scores*/
/*						given a ptr to a PREBINDSearch		*/
/*						record. The results are placed		*/ 
/*						into a linked list of PREBINDResult	*/
/*						structs.  A ptr to the list is 		*/
/*						returned only if at least one result*/
/*						was found.							*/
/*															*/
/*	PARAMETERS:												*/
/*		PREBINDSearchPtr	psp: ptr to PREBIND Search		*/
/*	RETURNS:												*/
/*		PREBINDResultSetPtr		prp							*/
/*		NULL					no results found for search	*/
/************************************************************/
PREBINDResultSetPtr GetListofResults(PREBINDSearchPtr psp)
{
	Int4				gi =					0;		/*gi from SeaRCH record*/
	CharPtr				name =					NULL;	/*name from SeaRCH record*/
	Int2				result_record =			0;
	Int4				result_pmid =			0;
	Int2				isvm_record =			0;
	FloatHi				isvm_iscore =			0;
	FloatHi				isvm_iscore2=			0;
	Int4				isvm_itruth =			0;
	Int4				isvm_opno =				0;
	Int4				isvm_opyes =			0;
	Boolean				record_found =			FALSE;

	PREBINDResultSetPtr	prsp =					NULL;
	PREBINDResultPtr	prp_head =				NULL;
	PREBINDResultPtr	prp_last =				NULL;
	PREBINDResultPtr	prp =					NULL;

	/*start a new PREBIND Result Set*/
	prsp = PREBINDResultSetNew();
	prp_head = PREBINDResultNew();
	prsp->records = prp_head;
	
	
	/*the search to find the results for is: */
	gi = psp->gi;
	/*allocate memory to name for the search*/
	name = (CharPtr) malloc(sizeof(Char) * (f4len(pf4_RESULT_NAME) + 1));
	if (name == NULL)
	{
		ErrPostEx(SEV_ERROR,0,0, "GetListofResults: Failed to allocate sufficient memory");
		return NULL;
	}
	StrCpy(name, psp->name);	
	
	/*select the gi tag in the RESULT database*/
	d4tagSelect(pd4_RESULT, pt4_RESULT_GIP);
	
	/*look for a RESULT record with the same gi and then the same name*/
	result_record = d4seekDouble(pd4_RESULT, gi);
	if (result_record != 0)
	{
		return NULL;
	}
	while (result_record == 0)/*RESULT record gi matches the SeaRCH gi*/
	{
		/*determine if the name from this RESULT record matches the name from the SeaRCH record*/
		PadStringII(pf4_RESULT_NAME, name); /*sufficient memory was pre-allocated for padding*/
		if (StrCmp(name, f4str(pf4_RESULT_NAME)) == 0) 
		{
			TrimSpaces(name);
			record_found = TRUE;
			/*get the pmid and associated isvm data*/
			result_pmid = f4long(pf4_RESULT_PMID);
			d4tagSelect(pd4_ISVM, pt4_ISVM_PMID);
			isvm_record = d4seekDouble(pd4_ISVM, result_pmid);
			if (isvm_record == 0)/*found record*/
			{
				isvm_iscore = f4double(pf4_ISVM_ISCORE);
				isvm_iscore2= f4double(pf4_ISVM_ISCORE2);
				isvm_itruth = f4long(pf4_ISVM_ITRUTH);
				isvm_opno   = f4long(pf4_ISVM_OPNO);
				isvm_opyes  = f4long(pf4_ISVM_OPYES);
			}

			/*add the current record to a list*/
			prp_last = ResultFillNode(prp_head, prp_last, gi, name, result_pmid, isvm_iscore, isvm_iscore2, isvm_itruth, isvm_opno, isvm_opyes);
			if (prp_last == NULL)
			{
				ErrPostEx(SEV_ERROR,0,0, "GetListofResults: ResultFillNode failed");
			}
		}

		/*look for the next RESULT record with a matching gi*/
		result_record = d4seekNextDouble(pd4_RESULT, gi);
	}

	/*MemFree()*/
	if (name) MemFree(name);

	if (record_found == TRUE)
	{
		return prsp;
	}
	else
	{
		return NULL;
	}

}


/************************************************************/
/*															*/
/*	FindInteractorInListByGI								*/
/*			
/*										 					*/ 
/*															*/
/*															*/
/*	PARAMETERS:												*/
/*		PREBINDInteractorSetPtr pisp:	list to look in		*/
/*		Int4					gi:		gi to find in list	*/
/*	RETURNS:												*/
/*		PREBINDInteractorPtr	pip	of node with gi			*/
/*		NULL					gi not found in list		*/
/************************************************************/
PREBINDInteractorPtr	FindInteractorInListByGI(PREBINDInteractorSetPtr pisp, Int4 gi)
{
	PREBINDInteractorPtr pip =	NULL;
	Int4 cur_gi =				0;
	
	if (pisp != NULL)
	{
		pip = pisp->records;
	}

	while (pip)
	{
		cur_gi = pip->gi;
		if (gi == cur_gi) break;
		pip = pip->next;
	}

	return pip;
}


/************************************************************/
/*															*/
/*	GetListofInteractors:									*/
/*		This function gets a list of interactors for a 		*/
/*		given gi from the POINT data table					*/
/*										 					*/ 
/*															*/
/*	PARAMETERS:												*/
/*		Int4	gi:		gi number							*/
/*	RETURNS:												*/
/*		PREBINDInteractorSetPtr	pisp						*/
/*		NULL					no interactors found for gi	*/
/************************************************************/
PREBINDInteractorSetPtr GetListofInteractorsForGI(Int4 gi, Boolean filter_nonint_papers, Int4 filter_toomanyresults)
{
	PREBINDInteractorSetPtr	pisp =					NULL;
	PREBINDInteractorPtr	pip =					NULL;
	PREBINDInteractorPtr	pip_head =				NULL;
	PREBINDInteractorPtr	pip_empty =				NULL;
	PREBINDInteractorPtr	pip_lastfilled =		NULL;
	PREBINDInteractorPtr	pip_tofill=				NULL;
	Int2					loop =					0;
	Int2					point_record =			0;
	Int4					gii =					0; /*gi of interactor*/
	Int4					opno =					0;
	Int4					opyes =					0;
	Int4					piscore =				0;
	Int4					piscore2 =				0;
	Int4					pitruth =				0;
	Int4					best_piscore =			0;
	Int4					pmid =					0;
	PREBINDIsvmPtr			pisvmp =				NULL;
	FloatHi					best_iscore =			0.0;
	Boolean					add_to_interactor_list = TRUE; /*default*/


	/*start a new PREBIND Interactor Set*/
	pisp = PREBINDInteractorSetNew();
	pip_head = PREBINDInteractorNew();
	if (pisp && pip_head)
	{
		pisp->records = pip_head;
		pip_empty = pip_head;
	}
	else
	{		
		ErrPostEx(SEV_ERROR,0,0, "GetListofInteractorsForGI: Failed to allocate memory for a new list");
		return NULL;
	}

	/*find a POINT record with the gi number passed to this function: first in the gia column then in the gib column*/
	for (loop=0;loop<2;++loop)
	{
		if (loop == 0) d4tagSelect(pd4_POINT, pt4_POINT_GIA);
		if (loop == 1) d4tagSelect(pd4_POINT, pt4_POINT_GIB);
		
		point_record = d4seekDouble(pd4_POINT, gi);
		
		while (point_record == 0)
		{
			/*get gi of interactor*/
			if (loop == 0) gii = f4long(pf4_POINT_GIB);
			if (loop == 1) gii = f4long(pf4_POINT_GIA);
			
			
			pmid = f4long(pf4_POINT_PMID);			
			pisvmp = GetIsvmbyPMID(pmid);


			/*check to see if the interactor should be added to the list; i.e. only interactors in interaction papers?*/
		
			if (filter_nonint_papers && pisvmp)
			{
					if (pisvmp->itruth < 0 || pisvmp->opyes < pisvmp->opno || pisvmp->iscore < 0)
					{
						add_to_interactor_list = FALSE;
					}
			}


			/*check to see if the interactor should be added to the list; i.e. only interactor names with low number of search results*/
			if (add_to_interactor_list  && filter_toomanyresults)/*this is a costly filter--only use if requested by user*/
			{
				if (FilterPointByNamesWithTooManyResults(gi, gii, filter_toomanyresults))
				{
					add_to_interactor_list = FALSE;
				}
			}
	

			if (add_to_interactor_list)
			{
				/*get info on whether the interactor is known/likely?*/
				pitruth = f4long(pf4_POINT_PITRUTH);
				opyes = f4long(pf4_POINT_OPYES);
				opno = f4long(pf4_POINT_OPNO);
				piscore = f4long(pf4_POINT_PISCORE);
				piscore2 = f4long(pf4_POINT_PISCORE2);

				/*if the interactor is already in the list then update that node*/
				pip_tofill = FindInteractorInListByGI(pisp, gii);
				if (!pip_tofill) pip_tofill = pip_empty;
				
				/*add gii to current node*/
				pip_tofill->gi = gii;
				
				/*update best values for this interactor*/
				if (pip_tofill->pitruth < pitruth)
				{
					pip_tofill->pitruth = pitruth;
				}
				
				pip_tofill->opyes += opyes;
				pip_tofill->opno  += opno;
				
				if (pip_tofill->piscore < piscore) pip_tofill->piscore = piscore;
				
				if (pip_tofill->piscore2 < piscore2) pip_tofill->piscore2 = piscore2;
				
				++pip_tofill->num_pmids;			
				
				/*if the paper is known to be an interaction paper then increase the number of interaction papers*/
				if (pisvmp->itruth > 0)
				{
					++pip_tofill->known_pmids;
				}
				
				/*if more people think its an interaction paper than not, increase the number of papers thought to be about interactions*/
				if ((pisvmp->opyes - pisvmp->opno) > 0)
				{
					++pip_tofill->opyes_pmids;
				}
				
				/*if its an interaction paper according to the svm, increase the number of probable int papers*/
				if (pisvmp->iscore > 0)
				{
					++pip_tofill->prob_pmids;
				}
			}

			/*reset variables*/
			opno = 0;
			opyes = 0;
			piscore = 0;
			piscore2 = 0;
			pitruth = 0;	
			pmid = 0;
			if (pisvmp) pisvmp = MemFree(pisvmp);
			add_to_interactor_list = TRUE;
			pip_tofill = NULL;

			/*if the empty node was used then add a new empty one to the end of the list*/
			if (pip_empty->gi > 0) /*ie; its no longer empty*/
			{
				pip_empty->next = PREBINDInteractorNew();
				pip_lastfilled = pip_empty; /*keep track of the last node filled*/
				if (pip_empty->next != NULL)
				{
					pip_empty = pip_empty->next;
				}
				else
				{
					ErrPostEx(SEV_ERROR,0,0, "GetListofInteractorsForGI: Failed to allocate memory for a new list node");
					if (pisp) pisp = MemFree(pisp);
					return NULL;
				}
			}
			else /*the empty node has not been used yet*/
			{
				/*pip_lastfilled still points to the last node filled*/
				/*pip_empty still points to an unfilled node*/
			}

			/*look for the next POINT record*/
			point_record = d4seekNextDouble(pd4_POINT, gi);

		}/*while another POINT record is found by pmid*/
			
	}/*for loop--examine gi's in gia and gib columns of POINT data table*/


	/*if the list contains nothing then return NULL*/
	if (pip_head == pip_empty)
	{
		if (pisp) pisp = MemFree(pisp);/*no matches for queried gi found in either gia or gib column*/
	}
	
	/*cap the list*/
	if (pip_lastfilled)
	{
		if (pip_lastfilled->next) pip_lastfilled->next= MemFree(pip_lastfilled->next);
	}

	return pisp;
}



/************************************************************/
/*															*/
/*	GetListofSearches:	This function gets a list of 		*/
/*						searches for a given gi from the	*/
/*						SRCH database and places it in a 	*/ 
/*						linked list of PREBINDSearch structs*/
/*															*/
/*	PARAMETERS:												*/
/*		Int4	gi:		gi number							*/
/*	RETURNS:												*/
/*		PREBINDSearchSetPtr		pssp						*/
/*		NULL					no searches found for gi	*/
/************************************************************/
PREBINDSearchSetPtr GetListofSearches(Int4 gi)
{
	Int2				srch_record =			0;
	CharPtr				srch_name =				NULL;
	Int4				srch_taxid =			0;
	Int4				srch_source =			0;
	CharPtr				srch_srched	=			NULL;
	Char				date_last_searched[20];
	Int2				srch_status =			0;
	Int2				srch_namered =			0;
	Int4				srch_results =			0;
	PREBINDSearchSetPtr	pssp =					NULL;
	PREBINDSearchPtr	psp_head =				NULL;
	PREBINDSearchPtr	psp_last =				NULL;
	PREBINDSearchPtr	psp =					NULL;

	/*start a new PREBIND Search Set*/
	pssp = PREBINDSearchSetNew();
	psp_head = PREBINDSearchNew();
	pssp->records = psp_head;
	
	
	/*find the record corresponding to the gi number in the SRCH database*/
	/*select the gi tag*/
	d4tagSelect(pd4_SRCH, pt4_SRCH_GIP);

	
	srch_record = d4seekDouble(pd4_SRCH, gi);
	if (srch_record != 0)
	{
		if (pssp) pssp = MemFree(pssp); /*NULL will be returned for an empty list*/
	}
	while (srch_record == 0)/*r4success*/
	{
		
		/*get taxon id for the current record*/
		srch_taxid = f4long(pf4_SRCH_TAXID);

		
		/*allocate memory to srch_name and get srch_name*/
		srch_name = (CharPtr) malloc(sizeof(Char) * f4len(pf4_SRCH_NAME));
		if (srch_name == NULL)
		{
			ErrPostEx(SEV_ERROR,0,0, "GetListofSearches: Failed to allocate sufficient memory");
			srch_name = "NOT FOUND";
		}
		else
		{
			StrCpy(srch_name, f4str(pf4_SRCH_NAME));
			if(srch_name == NULL) srch_name = "NOT FOUND";
			if(TrimSpaces(srch_name) != 0)
				ErrPostEx(SEV_ERROR,0,0, "GetListofSearches: TrimSpaces failed, possibly passed a null string");
		}
		
		/*get source for the current record*/
		srch_source = (long) f4long(pf4_SRCH_SOURCE);

		/*get search status for the current record*/
		srch_status = f4int(pf4_SRCH_STATUS);
		if (srch_status != 3)
		{
			StrCpy(date_last_searched, "not_yet_searched");
		}
		else if (srch_status == 3)
		{
			/*get date last searched*/
			srch_srched = StrSave(f4str(pf4_SRCH_SEARCHED));
			if (srch_srched == NULL)
			{
				StrCpy(date_last_searched, "not_found");
			}
			else if (StrCmp(srch_srched, "10000101") == 0)
			{
				StrCpy(date_last_searched, "not_yet_searched");
			}
			else 
			{
				date4format(srch_srched, date_last_searched, "MMM DD, CCYY");
			}
		}


		/*get name redundancy for the current record*/
		srch_namered = f4int(pf4_SRCH_NAMERED);

		/*get number of search results for the current record*/
		srch_results = f4int(pf4_SRCH_RESULTS);

		/*add the current record to a list*/
		psp_last = SearchFillNode(psp_head, psp_last, gi, srch_name, srch_source, date_last_searched, srch_status, srch_namered, srch_results);
		if (psp_last == NULL)
		{
			ErrPostEx(SEV_ERROR,0,0, "GetListofSearches: SearchFillNode failed");
		}

		srch_record = d4seekNextDouble(pd4_SRCH, gi);
	
	}/*while another record is found for this gi*/	

	/*MemFree(pssp) is in calling function*/
	
	return pssp;
}



/************************************************************/
/*	GetListofGIsbyNameAndTaxon:								*/
/*		This function returns a list of 					*/
/*		gi's given a name and (optionally) a 				*/
/*		taxon number by searching through the				*/
/*		SeaRCH database.									*/
/*	PARAMETERS:												*/
/*		CharPtr name										*/
/*		Int4	taxon			-can be zero				*/
/*	RETURNS:												*/
/*		ValNodePtr	vnp_head:								*/
/*					the vnp_head->choice field contains 3	*/
/*					if the length of the list is greater	*/
/*					than 1.  NULL is returned				*/
/*					if the list is empty					*/
/************************************************************/

ValNodePtr	GetListofGIsbyNameAndTaxon(CharPtr name, Int4 taxon_to_search)
{
	Int4			field_length =				0;
	CharPtr			search_name =				NULL;
	CharPtr			pointer_to_a_character =	NULL;
	Int2			size_to_allocate =			0;
	Int2			array_position =			0;
	CharPtr			quoted_name_ptr  =			NULL;
	Int2			srch_record =				0;
	Int4			cur_taxid =					0;
	Int4			cur_gip =					0;
	ValNodePtr		vnp_head =					NULL;
	Int2			num_gips =					0;

	/*copy the name into a new string to be modified*/
	/*query name max size submitted by cgi is 40, field_length of name field to be searched is 40*/
	field_length = f4len(pf4_SRCH_NAME);
	size_to_allocate = sizeof(Char)*(field_length + 1); /*+1 for the end of string \0*/
	search_name = (CharPtr) malloc(size_to_allocate);
	if (search_name != NULL)
	{
		StrCpy(search_name, name);
	}
	else
	{
		ErrPostEx(SEV_ERROR,0,0, "GetListofGIsbyNameAndTaxon: Failed to allocate sufficient memory");
		return NULL;
	}

	/*replace spaces in names with underscores*/
	while ((pointer_to_a_character = StrChr(search_name, ' ')) != NULL)
	{
		*pointer_to_a_character = '_';
	} 
	
	/*replace all lower case letters in name with uppercase*/
	array_position = sizeof(Char)*StrLen(search_name)-1;
	while (array_position >= 0)
	{
		search_name[array_position] = toupper(search_name[array_position]);
		--array_position;
	}

	/*place quotes around names with spaces, commas or brackets or hyphens*/
	if (StrStr(search_name, " ") != NULL  || 
		StrStr(search_name, ",") != NULL  || 
		StrStr(search_name, "(") != NULL  ||
		StrStr(search_name, ")") != NULL  ||
		StrStr(search_name, "-") != NULL  )
	{
		if ((Int2)(StrLen(search_name) + 2) > field_length)
		{
			/*the quoted name will be longer than the maximum sized name in the name field*/
		}
		else
		{
			size_to_allocate = sizeof(Char)*(field_length + 1);/*+1 for the end of string \0*/
			quoted_name_ptr = (CharPtr) malloc (size_to_allocate);
			if (quoted_name_ptr != NULL)
			{
				StrCpy(quoted_name_ptr, "\"");
				StrCat(quoted_name_ptr, search_name);
				StrCat(quoted_name_ptr, "\"");
				StrCpy(search_name, quoted_name_ptr);
				MemFree(quoted_name_ptr);
			}
		}
	}

	/*prepare name for searching against the name field of the search database*/
	PadStringII(pf4_SRCH_NAME, search_name); /*sufficient memory was pre-allocated for padding*/


	/*select the name tag in the SeaRCH database for a case insensitive search*/
	d4tagSelect(pd4_SRCH, pt4_SRCH_CIN); /*this is an index of all names in the search database made into uppercase */
		
	srch_record = d4seek(pd4_SRCH, search_name);
	if (srch_record == 0)/*r4success*/
	{
		while (srch_record == 0)
		{
			/*get taxid for the current record*/
			cur_taxid = f4long(pf4_SRCH_TAXID);

			/*get the gi for the current record- this is used below to get info from the accession database*/
			cur_gip = f4long(pf4_SRCH_GIP);
			
			/*add the gi to the list if it belongs to the requested taxon*/
			if (taxon_to_search == 0 || taxon_to_search == cur_taxid)
			{
				if (vnp_head == NULL)/*this will be the first entry in the list*/
				{
					vnp_head = ValNodeAddInt(NULL, 1, cur_gip);
					++num_gips;
				}
				else if (vnp_head) /*this will be a subsequent entry*/
				{
					ValNodeAddInt(&vnp_head, 1, cur_gip);
					++num_gips;
				}
			}
			
			srch_record = d4seekNext(pd4_SRCH, search_name);/*look for the next match*/
		}

	}/*only if there was at least one record found*/

	/*tell the head if there is more than one value in the list*/
	if (vnp_head) 
	{
		if (num_gips > 1)
		{
			vnp_head->choice = 3;
		}
	}

	/*MemFree*/
	if (search_name) MemFree(search_name);

	return vnp_head;
}



/************************************************************/
/*	GetGIsbyNameAndTaxon:									*/
/*		This function returns a list of 					*/
/*		gi's given a name and (optionally) a 				*/
/*		taxon number by searching through the				*/
/*		SeaRCH database.									*/
/*	PARAMETERS:												*/
/*		CharPtr name										*/
/*		Int4	taxon			-can be zero				*/
/*	RETURNS:												*/
/*		0	ok												*/
/************************************************************/

Int2	GetGIsbyNameAndTaxon(CharPtr name, Int4 taxon_to_search)
{
	Int4			field_length =				0;
	CharPtr			search_name =				NULL;
	CharPtr			pointer_to_a_character =	NULL;
	Int2			size_to_allocate =			0;
	Int2			array_position =			0;
	CharPtr			quoted_name_ptr  =			NULL;
	Int2			srch_record =				0;
	Int4			cur_taxid =					0;
	Int4			cur_gip =					0;
	PREBINDAccPtr	pap =						NULL;

	/*copy the name into a new string to be modified*/
	/*query name max size submitted by cgi is 40, field_length of name field to be searched is 40*/
	field_length = f4len(pf4_SRCH_NAME);
	size_to_allocate = sizeof(Char)*(field_length + 1); /*+1 for the end of string \0*/
	search_name = (CharPtr) malloc(size_to_allocate);
	if (search_name != NULL)
	{
		StrCpy(search_name, name);
	}
	else
	{
		ErrPostEx(SEV_ERROR,0,0, "GetGIsbyNameAndTaxon: Failed to allocate sufficient memory");
		return 1;
	}

	/*replace spaces in names with underscores*/
	while ((pointer_to_a_character = StrChr(search_name, ' ')) != NULL)
	{
		*pointer_to_a_character = '_';
	} 
	
	/*replace all lower case letters in name with uppercase*/
	array_position = sizeof(Char)*StrLen(search_name)-1;
	while (array_position >= 0)
	{
		search_name[array_position] = toupper(search_name[array_position]);
		--array_position;
	}

	/*place quotes around names with spaces, commas or brackets or hyphens*/
	if (StrStr(search_name, " ") != NULL  || 
		StrStr(search_name, ",") != NULL  || 
		StrStr(search_name, "(") != NULL  ||
		StrStr(search_name, ")") != NULL  ||
		StrStr(search_name, "-") != NULL  )
	{
		if ((Int2)(StrLen(search_name) + 2) > field_length)
		{
			/*the quoted name will be longer than the maximum sized name in the name field*/
		}
		else
		{
			size_to_allocate = sizeof(Char)*(field_length + 1);/*+1 for the end of string \0*/
			quoted_name_ptr = (CharPtr) malloc (size_to_allocate);
			if (quoted_name_ptr != NULL)
			{
				StrCpy(quoted_name_ptr, "\"");
				StrCat(quoted_name_ptr, search_name);
				StrCat(quoted_name_ptr, "\"");
				StrCpy(search_name, quoted_name_ptr);
				MemFree(quoted_name_ptr);
			}
		}
	}

	/*prepare name for searching against the name field of the search database*/
	PadStringII(pf4_SRCH_NAME, search_name); /*sufficient memory was pre-allocated for padding*/

	/*prepare for html output*/
	fprintf (stdout, "Content-type:text/html\r\n\r\n");
	WWWPrintFileData(HEADERFILE,  stdout);
	fprintf (stdout, " <BR>");
	if (taxon_to_search == 0)
	{
		fprintf (stdout, "<h2>Search Results for \"%s\":</h2><P>\n", name);	
	}
	else if (taxon_to_search != 0)
	{
		fprintf (stdout, "<h2>Search Results for \"%s\" and taxon %ld:</h2><P>\n", name, (long) taxon_to_search);	
	}

	/*select the name tag in the SeaRCH database for a case insensitive search*/
	d4tagSelect(pd4_SRCH, pt4_SRCH_CIN); /*this is an index of all names in the search database made into uppercase */
		
	srch_record = d4seek(pd4_SRCH, search_name);
	if (srch_record == 0)/*r4success*/
	{
		while (srch_record == 0)
		{
			/*get taxid for the current record*/
			cur_taxid = f4long(pf4_SRCH_TAXID);

			/*get the gi for the current record- this is used below to get info from the accession database*/
			cur_gip = f4long(pf4_SRCH_GIP);
			
			/*print the results out if they match the taxon being searched for*/
			if (taxon_to_search == 0 || taxon_to_search == cur_taxid)
			{
				/*get accession record by gi and print a summary with a link to the record*/
				
				pap = GetAccessionRecordbyGI(cur_gip);
				if (pap != NULL)
				{
					if (PrintSummaryAccessionRecord(pap) != 0)
					{
						ErrPostEx(SEV_ERROR, 0, 0, "GetAccessionData: PrintSummaryAccessionRecord failed\n");
						return 3;
					}
				}
				
			}
			
			srch_record = d4seekNext(pd4_SRCH, search_name);/*look for the next match*/
		}

	}/*only if there was at least one record found*/

	else if (srch_record != 0)
	{
		fprintf (stdout, "<BR>No matches were found.\n");
		/*ErrPostEx(SEV_ERROR, 0, 0, "GetGIsbyNameandTaxon: record not found for name = %s\n", name);*/
	}

	/*finish html output*/
	WWWPrintFileData(FOOTERFILE,  stdout);
	fflush(stdout); 

	/*MemFree*/
	if (search_name) MemFree(search_name);
	return 0;
}





/************************************************************/
/*															*/
/*	GetAllDatabyGIandPrint:	This function returns			*/
/*						information from the ACCession		*/
/*						database the SeaRCH database the	*/
/*						RESULTS database and the POINT		*/
/*						database given a gi number			*/
/*						or an accession number.				*/ 
/*						 									*/
/*															*/
/*	PARAMETERS:												*/
/*		Int4	gi			-can be zero					*/
/*		CharPtr accession	-can be null					*/
/*	RETURNS:												*/
/*		0	ok												*/
/************************************************************/
Int2 GetAllDatabyGIorACCandPrint(Int4 gi, CharPtr accession_input)
{
	PREBINDAccPtr			pap =			NULL;

	Int2					acc_record =	0;
	CharPtr					accession =		NULL;
	CharPtr					database =		NULL;
	CharPtr					locus_name =	NULL;
	CharPtr					gene_product =	NULL;
	Int4					taxid =			0;

	PREBINDSearchSetPtr		pssp =			NULL;
	PREBINDSearchPtr		psp =			NULL;
	Int4					max_results =	0;
	PREBINDResultSetPtr		prsp =			NULL;
	PREBINDResultPtr		prp =			NULL;
	Int4					cur_pmid =		0;
	PREBINDPointSetPtr		ppsp =			NULL;
	Int4					number_printed = 0;
	
	PREBINDInteractorSetPtr pisp =			NULL;
	PREBINDInteractorPtr	pip =			NULL;
	ValNodePtr				vnp_head =		NULL;
	ValNodePtr				vnp =			NULL;
	PREBINDPointPtr			ppp =			NULL;
	ValNodePtr				vnp_head2 =		NULL;
	ValNodePtr				vnp2 =			NULL;
	PREBINDAccPtr			papi =			NULL;
	CharPtr					descr =			NULL;
	CharPtr					locus =			NULL;
	CharPtr					intor_descr =	NULL;
	Int4					anchor =		1;
	Char					seqhound_location[PATH_MAX];

	ValNodePtr				vnp_head3 =		NULL;


	/*get location information from the config file*/
	seqhound_location[0] = '\0';
	GetAppParam("prebind", "prebind_cgi", "seqhound_location", NULL, seqhound_location, PATH_MAX*sizeof(Char));

	/*get info from accession record or quit*/
	if (gi == 0)
	{
		ErrPostEx(SEV_ERROR,0,0, "GetAllDatabyGIorACCandPrint: Invalid function params");
		return 1;
	}
	else if(gi != 0)
	{
		pap = GetAccessionRecordbyGI(gi);

		if (!pap)
		{
			fprintf (stdout, "Content-type:text/html\r\n\r\n");
			fprintf (stdout, "<BR>gi number: %ld was not found in the PreBIND database.\n", (long) gi);
			ErrPostEx(SEV_ERROR, 0, 0, "GetAllDatabyGIorACCandPrint: record not found for gi number = %ld\n", (long) gi);
			return 2;
		}
		else
		{
			accession = StrSave(pap->acc);
			taxid = pap->tax;
			locus_name = StrSave(pap->locus);
			database = StrSave(pap->db);
			gene_product = StrSave(pap->pprod);
		}
	}

	

	/*begin html output for accession page*/
	fprintf (stdout, "Content-type:text/html\r\n\r\n");
	WWWPrintFileData(HEADERFILE,  stdout);


	/*********************************************************/
	/*print accession record info							 */
	/*********************************************************/
	if (PrintAccessionRecordInfo(pap) != 0)
	{
		ErrPostEx(SEV_ERROR, 0, 0, "GetAllDatabyGIorACCandPrint: PrintAccessionRecordInfo failed\n");
		return 1;
	}



	/************************************************************************************************/
	/*get and print out the list of searches that were performed for this accession if there are any*/
	/************************************************************************************************/
	pssp = GetListofSearches(gi);
	if (pssp != NULL)
	{
		max_results = PrintListofSearches(pssp);
		if (max_results < 0)
		{
			ErrPostEx(SEV_ERROR, 0, 0, "GetAllDatabyGIorACCandPrint: PrintListofSearches failed\n");
			return 1;
		}
	}
	else if (pssp == NULL)
	{
		fprintf(stdout,"<BR>There are no searches associated with this record\n");
		return 2;
	}


	/***************************************/
	/*show output options				   */
	/***************************************/
	fprintf (stdout,"<!--output options-->\n");
	fprintf (stdout, "<form method=\"POST\" action=\"%s\" target=\"display\"> \n", cgi_path);	
	WWWPrintFileData(OPTIONSHEAD,  stdout); /*print the header for the output options*/	
	fprintf (stdout,"<table border=0  width=100%%  cellspacing=2 cellpadding=2 cols=1>");	
	fprintf (stdout, "<tr bgcolor=\"lightgrey\">\n");
	/*options for interactor list*/
	fprintf (stdout, "<td width=30%%><input type=checkbox name=\"sloi\" value=\"1\" checked>Show list of potential interactors with supporting evidence.\n");
	fprintf (stdout, "<td width=70%%><input type=checkbox name=\"fnip\" value=\"1\" checked>Only use interaction papers when composing list of potential interactors.<br>\n");	
	if (max_results < 2000)
	{
		fprintf (stdout, "Omit names that returned more than  <input type=text name=\"fnwtmr\" size=\"5\" maxlength=\"5\"> results when calculating potential interactors. Type a value between 1 and 10000.<br>\n");
	}
	else
	{
		fprintf (stdout, "Omit names that returned more than  <input type=text name=\"fnwtmr\" size=\"5\" maxlength=\"5\" value = \"%ld\"> results when calculating potential interactors. Type a value between 1 and 10000.<br>\n", (max_results -1));
	}
	fprintf (stdout, "</td>\n");	
	fprintf (stdout, "</tr>\n");	
	fprintf (stdout,"</table>\n");
	
	/*optional papers section*/
	fprintf (stdout,"<table border=0  width=100%%  cellspacing=2 cellpadding=2 cols=1>");	
	if (myargs[30].intvalue)
	{
		fprintf (stdout, "<tr bgcolor=\"lightgrey\"><input type=checkbox name=\"srbp\" value=\"1\" checked>Show list of interaction papers and list of other proteins in these papers.\n");
		fprintf(stdout,"  Go to this <a href=\"#papers\">section</a>.<br>\n");
	}
	else
	{
		fprintf (stdout, "<tr bgcolor=\"lightgrey\"><input type=checkbox name=\"srbp\" value=\"1\">Show list of interaction papers and list of other proteins in these papers (not shown by default).<br>\n");
	}
	/*optional all_results section*/
	if (myargs[31].intvalue)
	{
		fprintf (stdout, "<tr bgcolor=\"lightgrey\"><input type=checkbox name=\"slop\" value=\"1\" checked>Show list of papers returned by all searches with svm scores for interaction content.\n");
		fprintf(stdout,"  Go to this <a href=\"#all_results\">section</a>.<br>\n");
	}
	else
	{
		fprintf (stdout, "<tr bgcolor=\"lightgrey\"><input type=checkbox name=\"slop\" value=\"1\">Show list of papers returned by all searches with svm scores for interaction content (not shown by default).<br>\n");
	}
	fprintf (stdout,"</table>\n");

	fprintf (stdout, "<input type=hidden name=\"accession\" value=\"%s\">\n", pap->acc);
	fprintf (stdout, "<input type=hidden name=search_type value=\"accession\">\n");
	fprintf (stdout, "<input type=\"submit\" value=\"Redraw this results page.\">\n");
	fprintf (stdout, "</form>\n");
	fflush(stdout);

	/*too many results*/
	if (max_results > 2000 && myargs[28].intvalue == 0)
	{
		/*This is a stopgap measure: proteins with names that resturn more than a certain number of search results may CGI timeout
		By stopping here, the user is given the chance to filter out names that have a large number of search results
		Eventually, results from names with many results should be filtered somehow and/or potential interactions should not be made with these.*/
		
		fprintf (stdout, "One of the searches returned an unacceptably high number of results.  Edit or keep the number in the text box beside \"Omit names that returned more than...\" then hit the \"Redraw this results page\" button to see the results.  The results may take some time to compile and the process may possibly time-out.  If you have difficulties, contact the site-administrator.\n");
		/*print footer*/
		WWWPrintFileData(FOOTERFILE,  stdout);
		fflush(stdout);
		
		/*MemFree();*/
		if (accession) MemFree(accession);
		if (locus_name) MemFree(locus_name);
		if (gene_product) MemFree(gene_product);
		if (database) MemFree(database);
		if (pap) pap = MemFree(pap);
		if (papi) pap = MemFree(papi);
		if (pssp) MemFree(pssp);
		if (prsp) MemFree(prsp);
		
		return 0;
	}


	if ((Boolean) myargs[29].intvalue)
	{
		/***************************************/
		/*get a list of interactors and sort it*/
		/***************************************/
		
		/*get list of interactors*/
		pisp = GetListofInteractorsForGI(gi, (Boolean) myargs[27].intvalue, myargs[28].intvalue);
		
		
		
		/*place pointers to Interactor objects into an array*/
		if (pisp) 
		{
			pip = pisp->records;
			/*fill the head*/
			if (pip) 
			{
				vnp_head = ValNodeAddPointer(NULL, 1, pip);
				vnp = vnp_head;
				pip=pip->next;
			}
			/*fill the rest of the linked list*/
			while (pip && vnp)
			{
				vnp = ValNodeAddPointer(&vnp_head, 1, pip);
				pip = pip->next;
			}
			pip = NULL;
			vnp = NULL;
		}
		/*MemFree*/
		if (pisp) pisp = MemFree(pisp);
		
		/*sort the array of Interactor objects*/ /*see utilpub.c and vastserv.c*/
		VnpHeapSort(&vnp_head, ComparePotentialInteractors);
		
		
		/***************************************/
		/*print list of interactors			   */
		/***************************************/
		/*set a top anchor here*/
		fprintf (stdout,"<a name=\"top\"></a>");
		/*print out each interactor with supporting evidence*/
		WWWPrintFileData(INTERACTORSUMMARYHEAD,  stdout); /*print the header for the output of Interactors for this GI*/
		
		if (vnp_head == NULL) 
		{
			fprintf(stdout, "There are no known potential interactors for this protein.\n");	
		}
		else if (vnp_head != NULL)
		{
			vnp = vnp_head;
			anchor = 1; /*so the summary list can be linked to the list of interactors with evidence*/
			
			/*print the table column headings*/
			
			/*start the table*/
			fprintf (stdout,"<table border=0 width=100%%  cellspacing=2 cellpadding=2 cols=1>");
			
			fprintf (stdout,"<tr bgcolor=\"lightsteelblue\">\n");
			/*locus*/
			fprintf(stdout,"<td width=\"5%%\" height=\"20\">\n");
			fprintf (stdout,"<strong>name</strong>");
			fprintf (stdout,"</td>");
			/*brief description*/
			fprintf(stdout,"<td width=\"50%%\" height=\"20\">\n");
			fprintf(stdout,"short description\n");
			fprintf (stdout,"</td>");
			/*get a description of the likelihood of this interactor and print*/
			fprintf(stdout,"<td width=\"15%%\" height=\"20\">\n");
			fprintf(stdout,"Is this interactor real?\n");
			fprintf (stdout,"</td>");
			/*number of papers that may support this interactor*/
			fprintf(stdout,"<td width=\"10%%\" height=\"20\">\n");
			fprintf(stdout,"View supporting<br>papers\n");
			fprintf(stdout,"</td>\n");
			/*link to SeqHound*/
			fprintf(stdout,"<td width=\"10%%\" height=\"20\">\n");
			fprintf(stdout,"more info\n");
			fprintf(stdout,"</td>\n");	
			/*link to PreBIND*/
			fprintf(stdout,"<td width=\"10%%\" height=\"20\">\n");
			fprintf(stdout,"more info\n");
			fprintf(stdout,"</td>\n");
			fprintf (stdout,"</tr>");
			
			while (vnp)
			{
				
				pip = vnp->data.ptrvalue;
				if (pip)
				{
					
					/*get a description of the gene product*/
					papi = GetAccessionRecordbyGI(pip->gi);
					
					if (papi != NULL)
					{
						descr = StrSave(papi->pprod);
						locus = StrSave(papi->locus);
					}
					if (descr == NULL) descr = StrSave("Description of potential interactor not available");
					if (locus == NULL) locus = StrSave("Encoding locus of potential interactor not available");
					
					/*start the table*/
					fprintf (stdout,"<table border=0 width=100%%  cellspacing=2 cellpadding=2 cols=1>");
					
					fprintf (stdout,"<tr>\n");
					/*locus*/
					fprintf(stdout,"<td width=\"5%%\" height=\"20\">\n");
					fprintf (stdout,"<strong>%s</strong>", locus);
					fprintf (stdout,"</td>");
					/*brief description*/
					fprintf(stdout,"<td width=\"50%%\" height=\"20\">\n");
					fprintf(stdout,"%s ", descr);
					fprintf (stdout,"</td>");
					/*get a description of the likelihood of this interactor and print*/
					intor_descr	= DescribeInteractorLikelihood(pip);
					fprintf(stdout,"<td width=\"15%%\" height=\"20\">\n");
					fprintf(stdout,"%s ", intor_descr);
					fprintf (stdout,"</td>");
					/*number of papers that may support this interactor*/
					fprintf(stdout,"<td width=\"10%%\" height=\"20\">\n");
					fprintf(stdout,"<a href=\"#anchor%ld\">%ld</a>",anchor, pip->num_pmids);
					fprintf(stdout,"</td>\n");
					/*link to SeqHound*/
					fprintf(stdout,"<td width=\"10%%\" height=\"20\">\n");
					fprintf (stdout, "<a href=\"javascript:void window.open('%s/wwwseekgi?field=GI+identifier&GI=%ld&format=defline', 'giWin', 'toolbar=no,location=no,directories=no,status=yes,menubar=yes,scrollbars=yes,resizable=yes,width=550,height=400');\" onmouseover=\"window.status='Go to SeqHound';return(true);\" onmouseout=\"window.status=''\">SeqHound</a>\n",seqhound_location, (long) pip->gi);
					fprintf(stdout,"</td>\n");	
					/*link to PreBIND*/
					fprintf(stdout,"<td width=\"10%%\" height=\"20\">\n");
					fprintf (stdout, "<a href=\"%s?accession=%s&search_type=accession\">PreBIND</a>\n",cgi_path, papi->acc);
					fprintf(stdout,"</td>\n");
					fprintf (stdout,"</tr>");
	
					fflush(stdout);
					++anchor;
					
				}/*if pip*/
				
				pip = NULL;
				vnp = vnp->next;
			}
			
			fprintf (stdout,"</table>");
			fflush(stdout);
		}
		
		
		
		/*print out each interactor with supporting evidence*/
		WWWPrintFileData(INTERACTOREVIDENCEHEAD,  stdout); /*print the header for the output of Interactors for this GI*/
		if (vnp_head == NULL) 
		{
			fprintf(stdout, "There are no known potential interactors for this protein.\n");	
		}
		else if (vnp_head != NULL)
		{
			anchor = 1; /*so the summary list can be linked to the list of interactors with evidence*/
			vnp = vnp_head;
			while (vnp)
			{
				
				pip = vnp->data.ptrvalue;
				if (pip)
				{
					
					/*get a description of the gene product*/
					papi = GetAccessionRecordbyGI(pip->gi);
					
					if (papi != NULL)
					{
						descr = StrSave(papi->pprod);
						locus = StrSave(papi->locus);
					}
					if (descr == NULL) descr = StrSave("Description of potential interactor not available");
					if (locus == NULL) locus = StrSave("Encoding locus of potential interactor not available");
					
					/*print a header for this potential interactor*/
					fprintf (stdout,"<table border=0 width=100%%  cellspacing=2 cellpadding=2 cols=1>");
					fprintf (stdout,"<tr bgcolor=\"lightsteelblue\">");
					
					fprintf (stdout,"<a name=\"anchor%ld\"></a>", anchor);
					/*locus and short descr*/
					fprintf(stdout,"<td bgcolor=\"lightsteelblue\" width=\"90%%\" height=\"20\">\n");
					fprintf (stdout,"<strong>%s</strong>", locus);
					fprintf(stdout,": %s ", descr);
					fprintf (stdout,"</td>");
					/*SeqHound*/
					fprintf(stdout,"<td bgcolor=\"lightsteelblue\" width=\"5%%\" height=\"20\">\n");
					fprintf (stdout, "<a href=\"javascript:void window.open('%s/wwwseekgi?field=GI+identifier&GI=%ld&format=defline', 'giWin', 'toolbar=no,location=no,directories=no,status=yes,menubar=yes,scrollbars=yes,resizable=yes,width=550,height=400');\" onmouseover=\"window.status='Go to SeqHound';return(true);\" onmouseout=\"window.status=''\">SeqHound</a>\n",seqhound_location, (long) pip->gi);
					fprintf(stdout,"</td>\n");	
					/*PreBIND*/
					fprintf(stdout,"<td bgcolor=\"lightsteelblue\" width=\"5%%\" height=\"20\">\n");
					fprintf (stdout, "<a href=\"%s?accession=%s&search_type=accession\">PreBIND</a>\n",cgi_path, papi->acc);
					fprintf(stdout,"</td>\n");
					/*back to the top*/
					fprintf(stdout,"<td bgcolor=\"lightsteelblue\" width=\"5%%\" height=\"20\">\n");
					fprintf(stdout,"<a href=\"#top\">top</a>");
					fprintf(stdout,"</td>\n");
					
					fprintf (stdout,"</tr>");
					fprintf (stdout,"</table>");
					
					
					/*print the description of the gene product*/
					fprintf (stdout,"<table border=0 width=100%%  cellspacing=2 cellpadding=2 cols=1>");
					fprintf(stdout,"<tr>\n");			
					fprintf(stdout,"<td bgcolor=\"lightsteelblue\" width=\"100%%\" height=\"20\">\n");
					fprintf(stdout,"Number of paper(s) that may provide support for an interaction between your query protein (Molecule A) and %s (Molecule B): %ld", locus, pip->num_pmids);
					fprintf(stdout,"</td>\n");
					fprintf(stdout,"<tr>\n");
					fprintf (stdout,"</table>");				
					fflush(stdout);
					
					
					/*get list of papers supporting this interaction*/
					ppsp = GetListofPointsby2GIs(gi, pip->gi, (Boolean) myargs[27].intvalue, myargs[28].intvalue);
					
					/*place pointers to POINTS supporting this interaction into an array*/
					if (ppsp) 
					{
						ppp = ppsp->records;
						/*fill the head*/
						if (ppp) 
						{
							vnp_head2 = ValNodeAddPointer(NULL, 1, ppp);
							vnp2 = vnp_head2;
							ppp=ppp->next;
						}
						/*fill the rest of the linked list*/
						while (ppp && vnp2)
						{
							vnp2 = ValNodeAddPointer(&vnp_head2, 1, ppp);
							ppp = ppp->next;
						}
						ppp = NULL;
						vnp2 = NULL;
					}
					/*MemFree*/
					if (ppsp) ppsp = MemFree(ppsp);
					
					/*sort the array of Interactor objects*/ /*see utilpub.c and vastserv.c*/
					VnpHeapSort(&vnp_head2, CompareInteractionPapersFor2Points);
					
					
					/*print the evidence supporting this interactor*/
					if (vnp_head2)
					{
						PrintListofEvidenceSupportingInteractor(vnp_head2, gi, pip->gi);
					}
					/*mem free*/
					if (vnp_head2) vnp_head2 = MemFree(vnp_head2);

				}/*if pip*/
				
				pip = NULL;
				++anchor;
				vnp = vnp->next;
			}
		}

		/*mem free*/
		if (vnp_head) vnp_head = MemFree(vnp_head);
	}
	

	if ((Boolean) myargs[30].intvalue)
	{
		fprintf (stdout,"<a name=\"papers\"></a>");
		/***********************************************************************************************/
		/*get and print out proteins found in the above list of papers (if they are interaction papers)*/
		/*using the POINT database																	   */
		/***********************************************************************************************/
		/*for each search, get a list of results*/
		psp = pssp->records;
		if (psp != NULL) /*print the header for the output of POINT data for this GI*/
		{
			WWWPrintFileData(GIPOINTHEAD,  stdout);
		}
		while (psp != NULL)
		{
			prsp = GetListofResults(psp);

			fprintf (stdout, "<strong>Interaction papers found by searching for \"%s\".  </strong>\n", psp->name);

					
			/*place pointers to Interactor objects into an array*/
			if (prsp) 
			{
				prp = prsp->records;
				/*fill the head*/
				if (prp) 
				{
					vnp_head3 = ValNodeAddPointer(NULL, 1, prp);
					vnp = vnp_head3;
					prp=prp->next;
				}
				/*fill the rest of the linked list*/
				while (prp && vnp)
				{
					vnp = ValNodeAddPointer(&vnp_head3, 1, prp);
					prp = prp->next;
				}
				prp = NULL;
				vnp = NULL;
			}
			
			/*sort the array of Interactor objects*/ /*see utilpub.c and vastserv.c*/
			VnpHeapSort(&vnp_head3, CompareInteractionPapersFor2Results);
			/*MemFree*/
			if (prsp) MemFree(prsp);
			
			if (vnp_head3)
			{
				vnp = vnp_head3;
				number_printed = 0;
				while (vnp)
				{
					prp = vnp->data.ptrvalue;
					if (prp->iscore  > 0) /*it is an interaction paper*/
					{
						/*get a list of potential interactions in this paper from the POtential INTeraction database*/
						cur_pmid = prp->pmid;
						ppsp = GetListofPointsbyPMID(cur_pmid);
						/*print the pmid of the interaction paper to which the info below applies*/
						fprintf (stdout,"<table border=0 width=100%%  cellspacing=2 cellpadding=2 cols=1>");
						fprintf (stdout,"<tr bgcolor=\"lightsteelblue\">");
						fprintf (stdout,"<td colspan=1 height=\"20\">");
						fprintf (stdout,"PubMed abstract <a target=\"ncbi\" onmouseover=\"status='View abstract at PubMed'\" href=\"http://www.ncbi.nlm.nih.gov/htbin-post/Entrez/query?db=m&form=6&uid=%ld&Dopt=r&tool=EntrezLinkDoc\">%ld</a>", (long) cur_pmid, (long) cur_pmid);
						fprintf (stdout, "was returned by a search for \"%s\" and may contain interaction information (score = %lf). ", psp->name, prp->iscore);
						fprintf (stdout,"</td>");
						fprintf (stdout,"</tr>");
						fprintf (stdout,"</table>");
						
						if (ppsp != NULL)
						{
							/*print out a list of objects that appear in the interaction paper that may interact with this gi*/
							if (PrintListofPointsForGI(ppsp, gi) != 0)
							{
								ErrPostEx(SEV_ERROR, 0, 0, "GetAccessionData: PrintListofPointsforGI failed\n");
							}
						}
						else 
						{
							fprintf(stdout,"No other names are mentioned in this paper.\n<P>");
						}
						
						++number_printed;
						
					} /*if it is an interaction paper*/		
					
					/*go to the next search result (paper)*/
					vnp = vnp->next;
					
				}/*while there are still results to look at for the current search*/
				
			}/*if there are results (papers) to look at for the search*/
			/*MemFree*/
			if (vnp_head3) vnp_head3 = MemFree(vnp_head3);
			
			/*check that something was printed*/
			if (number_printed < 1)
			{
				fprintf(stdout, "None of the papers returned by searches for this name were classified as \"interaction papers\".\n");
			}
			fprintf (stdout,"<hr>");
			/*go to the next search*/
			psp = psp->next;
			
		}/*while there is still another search*/
		
	}


	if ((Boolean) myargs[31].intvalue)
	{
		fprintf (stdout,"<a name=\"all_results\"></a>");
		/*********************************************************/
		/*for each search, get and print a list of search results*/
		/*********************************************************/
		psp = pssp->records;
		
		if (psp != NULL) /*print the header for the output of RESULT data */
		{
			WWWPrintFileData(RESULTHEAD,  stdout);
		}
		
		while (psp != NULL)
		{
			prsp = GetListofResults(psp);
			
			if (prsp != NULL)
			{
				
				if (PrintListofResults(prsp) != 0)
				{
					ErrPostEx(SEV_ERROR, 0, 0, "GetAccessionData: PrintListofResults failed\n");
					return 3;
				}
				
			}
			else 
			{
				/*there are no results associated with the search*/
			}
			psp = psp->next;
		}
	}
	


	/*print footer*/
	WWWPrintFileData(FOOTERFILE,  stdout);
	fflush(stdout);

	/*MemFree();*/
	if (accession) MemFree(accession);
	if (locus_name) MemFree(locus_name);
	if (gene_product) MemFree(gene_product);
	if (database) MemFree(database);
	if (pap) pap = MemFree(pap);
	if (papi) papi = MemFree(papi);
	if (pssp) MemFree(pssp);

	
	return 0;
}




/********************************************************/
/*														*/
/*	MAIN:												*/
/*														*/
/*	PARAMETERS:											*/
/*		command line									*/
/*	RETURNS:											*/
/*		0	ok											*/
/********************************************************/
Int2 Main(void)
{  
	Int2		args_error =	0;
	CharPtr		search_type =	NULL;
	CharPtr		accession =		NULL;
	Int4		gi =			0;
	ValNodePtr	vnp_gi_list =	NULL;
	CharPtr		name =			NULL;
	Int4		taxid =			0;
	Int4		pmid =			0;
	Int4		gia =			0;
	Int4		gib =			0;
	Int2		frames_ready =	0;
	/*submit args*/
	Int4		sgia =	0;
	Int4		sgib =	0;
	CharPtr		nama =	NULL;
	CharPtr		namb =	NULL;
	CharPtr		adesc =	NULL;
	CharPtr		bdesc =	NULL;
	Int4		atype =	0;
	Int4		btype = 0;
	Int4		aok =	0;
	Int4		bok =	0;
	Int4		afrom = 0;
	Int4		bfrom = 0;
	Int4		ato =	0;
	Int4		bto =	0;
	Int4		opin =	0;
	Int4		method= 0;
	CharPtr		fname =	NULL;
	CharPtr		lname =	NULL;
	Int4		buid =	0;
	CharPtr     piid = NULL;
	
	/*logging*/
	time_t  time_now;
	CharPtr TimeNowStr;
	/*web*/
	WWWInfoPtr info;

	
	/*set up error handling*/
	/*ErrSetLogfile(LOGFILE, ELOG_APPEND); error log file is defined by the macro "LOGFILE"*/
	if(!BSErrLogOpen(LOGFILE, ELOG_APPEND)) {
		printf("Content-type: text/html\r\n\r\n");
		printf("<H2>Error</H2>\n - Unable to Open Log - contact site administrator\n");
		exit(1);
	}
	ErrSetOptFlags(EO_LOG_SEVERITY | EO_LOG_CODES | EO_LOG_FILELINE | EO_LOG_USRFILE); /*style of log file*/
	ErrSetLogLevel(SEV_WARNING); /*any errors => SEV_WARNING are logged*/
	ErrSetMessageLevel(SEV_MAX); /*any errors => SEV_MAX are displayed as messages to the user*/
	ErrSetFatalLevel(SEV_MAX);   /*never abort*/

	/*print time and date to log file*/
	time_now = GetSecs();
	TimeNowStr = ctime(&time_now);
	TimeNowStr[24] = '\0';
	BSErrLogPrintf("\n*==*\n");
	BSErrLogPrintf("Start: %s\n", TimeNowStr);

	/* check command line arguments (see taskapi library) */
	args_error = GetTaskArgsInternal("PreBIND", NUMARGS, myargs, &info);
	if (args_error != 0)
	{
		return 3;
	}
	else 
	{
	
		search_type =	myargs[0].strvalue;
		accession =		myargs[1].strvalue;
		name =			myargs[2].strvalue;
		taxid =			myargs[3].intvalue;
		pmid =			myargs[4].intvalue;
		gia =			myargs[5].intvalue;
		gib =			myargs[6].intvalue;
		frames_ready =	myargs[7].intvalue;

		sgia =	myargs[8].intvalue;
		sgib =	myargs[9].intvalue;
		nama =	StrSave(myargs[10].strvalue);
		namb =	StrSave(myargs[11].strvalue);
		adesc =	StrSave(myargs[12].strvalue);
		bdesc =	StrSave(myargs[13].strvalue);
		atype =	myargs[14].intvalue;
		btype = myargs[15].intvalue;
		aok =	myargs[16].intvalue;
		bok =	myargs[17].intvalue;
		afrom = myargs[18].intvalue;
		bfrom = myargs[19].intvalue;
		ato =	myargs[20].intvalue;
		bto =	myargs[21].intvalue;
		opin =	myargs[22].intvalue;
		method= myargs[23].intvalue;
		fname =	StrSave(myargs[24].strvalue);
		lname =	StrSave(myargs[25].strvalue);
		buid =	myargs[26].intvalue;
		piid = StrSave(myargs[32].strvalue);


		if (myargs[28].intvalue < 1 || myargs[28].intvalue > 10000)/*filter names with too many results*/
		{
			myargs[28].intvalue = 0;
		}

	
		/*log query*/
		BSErrLogPrintf("Query: %s\n", WWWGetQuery(info));

	}


	/* initialize the SeqHound system */
    if(!SHoundInitReadOnly(FALSE, "prebind")) 
	{
		ErrPostEx(SEV_ERROR,0,0,"Unable to Initialize SeqHound.");
	}
	if (SHoundIsInited())
	{
		BSErrLogPrintf("SeqHound is Initiated\n");
	}


	/*check for obvious user errors*/
	if ((StrCmp(search_type, "accession") == 0) && (StrCmp(accession, "no_accession_number_entered") == 0))
	{
			fprintf (stdout, "Content-type:text/html\r\n\r\n");
			fprintf(stdout, "No accession number was entered for your accession number search.  Use the \"back\" button in your browser to return to the search page and enter an accession number to search for.\n");
			return 4;
	}

	if ((StrCmp(search_type, "nametax") == 0) && (StrCmp(name, "no_name_entered") == 0))
	{
			fprintf (stdout, "Content-type:text/html\r\n\r\n");
			fprintf(stdout, "No name was entered for your name search.  Use the \"back\" button in your browser to return to the search page and enter a name to search for.\n");
			return 5;
	}


	/*open PreBIND*/
	if (!OpenPreBIND())
	{
		ErrPostEx(SEV_ERROR, codeBase.errorCode, 0, "Main: OpenPreBIND failed.");
		return FALSE;
	}
	/*log query*/
	BSErrLogPrintf("PreBIND datafiles open\n");


	/*find path name of prebind executable*/
	cgi_path[0] = '\0';
	GetAppParam("prebind", "prebind_cgi", "path", NULL, cgi_path, (size_t)PATH_MAX * (sizeof(Char)));
	

	/*select the function to perform according to the search type*/
	if (StrCmp(search_type, "accession") == 0)
	{
		gi = GetGIbyAccession(accession);
		if (gi == 0)
		{
			PrintErrorinFindingAccession(accession);
			ErrPostEx(SEV_ERROR, 0, 0, "Main: record not found for accession number = %s\n", accession);
		}
		else if (gi > 0)
		{
			if ((GetAllDatabyGIorACCandPrint(gi, NULL)) != 0)
			{
				ErrPostEx(SEV_ERROR, 0, 0, "Main: GetAllDatabyGIorACCandPrint failed\n");
				ClosePreBIND();
				return 6;
			}
		}
	}
	else if (StrCmp(search_type, "nametax") == 0)
	{
		vnp_gi_list = GetListofGIsbyNameAndTaxon(name, taxid);
		if (vnp_gi_list == NULL)
		{
			/*Print No results found*/
			PrintDisambiguationPage(vnp_gi_list);
			ErrPostEx(SEV_ERROR, 0, 0, "Main: No results found for name %s and taxon %ls\n", name, taxid);
		}	
		else if (vnp_gi_list != NULL && vnp_gi_list->choice == 1)
		{
			/*theres only one gi in the list--go directly to search results page for that gi*/
			gi = vnp_gi_list->data.intvalue;
			if ((GetAllDatabyGIorACCandPrint(gi, NULL)) != 0)
			{
				ErrPostEx(SEV_ERROR, 0, 0, "Main: GetAllDatabyGIorACCandPrint failed\n");
				ClosePreBIND();
				return 7;
			}
		}
		else if (vnp_gi_list != NULL && vnp_gi_list->choice > 1)
		{
			/*theres more than one gi in the list--go to the disambiguation page*/
			PrintDisambiguationPage(vnp_gi_list);
		}
	}
	else if (StrCmp(search_type, "points") == 0)
	{
		if ((PrintListofPointsForPMID(pmid, gia, gib, frames_ready)) != 0)
		{
			ErrPostEx(SEV_ERROR, 0, 0, "Main: PrintListofPointsForPMID failed\n");
			ClosePreBIND();
			return 8;
		}
	}
	else if (StrCmp(search_type, "review") == 0)
	{
		if ((ReviewAPoint(pmid, gia, gib)) != 0)
		{
			ErrPostEx(SEV_ERROR, 0, 0, "Main: ReviewAPoint failed\n");
			ClosePreBIND();
			return 9;
		}
	}
	else if (StrCmp(search_type, "submit") == 0)
	{
		if ((Submit(pmid, gia, gib, sgia, sgib, nama, namb, adesc, bdesc, atype, btype, aok, bok, afrom, bfrom, ato, bto, opin, method, fname, lname, buid, piid)) != 0)
		{
			ErrPostEx(SEV_ERROR, 0, 0, "Main: Submit failed\n");
			ClosePreBIND();
			return 10;
		}
	}
	else 
	{
		ErrPostEx(SEV_ERROR, 0, 0, "Main: Failed to find function to perform\n");
		fprintf (stdout, "<BR>Error: failed to find function to perform\n");
		ClosePreBIND();
		return 11;
	}


	/*log user info and quit*/
	BSErrLogPrintf("User: %s|%s|%s|%s|%d\n", 
		(WWWGetAddress(info)==NULL)?"NULL":WWWGetAddress(info), 
		(WWWGetHost(info)==NULL)?"NULL":WWWGetHost(info), 
		(WWWGetAgent(info)==NULL)?"NULL":WWWGetAgent(info), 
		(WWWGetServer(info)==NULL)?"NULL":WWWGetServer(info), 
		WWWGetBrowser(info));
	time_now = GetSecs();
	TimeNowStr = ctime(&time_now);
	TimeNowStr[24] = '\0'; 
	BSErrLogPrintf("End: %s\n", TimeNowStr);
	
	BSErrLogClose();
	WWWInfoFree(info);
	ClosePreBIND();
	if (SHoundIsInited()) SHoundFini();

	return 0;
}




/*
$Log: prebind_cgi.c,v $
Revision 1.34  2003/07/29 01:08:26  janicek
Minor change to fix compiler warning

Revision 1.33  2003/07/28 20:23:56  ryao
Add html link functions

Revision 1.32  2003/07/25 18:10:41  ryao
Allow users to add pmids to an existing BIND record

Revision 1.31  2003/07/16 20:29:25  janicek
Changed to new method of linking to PubMed

Revision 1.30  2003/06/19 18:54:42  janicek
roll back to 1.28

Revision 1.29  2003/06/18 14:31:05  ryao
allow users to add interaction papers to existing BIND record.

Revision 1.28  2003/03/22 04:25:35  janicek
minor changes

Revision 1.27  2003/03/21 08:42:12  janicek
Added sort and filter functions

Revision 1.26  2003/03/18 22:44:16  janicek
Added result filter for non-int papers

Revision 1.25  2003/02/27 08:00:31  janicek
Added Summary List of Interactors to output

Revision 1.24  2003/02/26 22:55:54  janicek
Reorganized code for better modularity

Revision 1.23  2003/02/26 06:55:40  janicek
Added code to add Molecule A and B names to interactor list output

Revision 1.22  2003/02/21 22:36:02  janicek
Finished sorting interactors

Revision 1.21  2003/02/20 22:31:32  janicek
Added functions to output interactor list

Revision 1.20  2003/02/18 22:20:49  janicek
Added function GetListofPointsby2GIs

Revision 1.19  2003/02/14 21:48:32  janicek
Finished GetListofInteractors

Revision 1.18  2003/02/12 23:02:46  janicek
Began adding functions to retrieve and sort interactor sets

Revision 1.17  2003/01/30 22:12:19  janicek
Fixed conflicts

Revision 1.16  2003/01/30 22:08:41  michel
Fixed conflicts

Revision 1.15  2003/01/30 19:53:53  janicek
Improved logging

Revision 1.14  2002/12/17 15:48:45  janicek
minor changes

Revision 1.13  2002/12/17 15:23:34  janicek
minor changes

Revision 1.12  2002/12/17 14:56:40  janicek
Added title to submitted BIND record and moved PostSubmit script to SubmitToBIND function

Revision 1.11  2002/12/16 20:00:24  janicek
Added line to inform BIND curators of PreBIND submission to BIND

Revision 1.10  2002/12/03 20:17:52  janicek
Removed requirement to enter BIND user id

Revision 1.9  2002/12/03 19:28:58  janicek
Moved Anonymous and Genetic User Id's to config file

Revision 1.8  2002/12/02 20:14:02  janicek
removed debug statements and comments

Revision 1.7  2002/12/02 19:12:25  janicek
Added more cgi debug points

Revision 1.6  2002/12/02 18:39:53  janicek
added CGI debug lines

Revision 1.5  2002/11/11 22:17:58  janicek
minor changes

Revision 1.4  2002/11/11 21:43:06  janicek
Moved SHoundInit and Fini to main

Revision 1.3  2002/11/11 21:00:00  janicek
Added SHound check at beginning

Revision 1.2  2002/07/30 00:10:45  janicek
changed PadString to PadStringII

Revision 1.1.1.1  2002/07/17 19:36:35  janicek
initial import

*/


