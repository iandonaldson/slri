/*$Id: prebind_lib.c,v 1.10 2003/02/20 22:34:10 janicek Exp $*/
/*
-- ***************************************************************************
--
--  Pre Biomolecular Interaction Network Database (PreBIND)
--
--  File: prebind_lib.c
--  Description: functions commonly used by prebind applications
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




#ifndef _PREBIND_LIB_
#include "prebind_lib.h"
#endif




/********************************************************/
/*														*/
/*	TrimSpaces: removes spaces from the end of a string	*/
/*														*/
/*	PARAMETERS:											*/
/*			CharPtr		name_tobe_trimmed				*/
/*	RETURNS:											*/
/*			Int2		0 for success					*/
/*						1 null string was passed		*/
/********************************************************/
Int2 TrimSpaces(CharPtr name_tobe_trimmed)
{
	Int2	array_pointer =	0;

	array_pointer = sizeof(Char)*strlen(name_tobe_trimmed) -1;
	if (array_pointer < 0) return 1; 
	while (name_tobe_trimmed[array_pointer] == ' ')
	{
		name_tobe_trimmed[array_pointer] = '\0';
		--array_pointer;
	}
	return 0;

}


/********************************************************/
/*														*/
/*	GetTaxNameByTaxId: returns the taxon name from the 	*/
/*		taxlist.txt file in the ncbi/data directory		*/
/*		given the taxon id number						*/
/*														*/
/*	PARAMETERS:											*/
/*			taxid		taxon id number					*/
/*	RETURNS:											*/
/*			CharPtr		Taxon name						*/
/*						NULL if taxid not found in list	*/
/*						NULL if an error ocuurs			*/
/********************************************************/
CharPtr GetTaxNameByTaxId(Int4 taxid)
{
	CharPtr taxname =		NULL;
	CharPtr path =			NULL;
	CharPtr tax_list_file = NULL;
	CharPtr string =		NULL;
	CharPtr	token =			NULL;
	CharPtr name_ptr =		NULL;
	Int4	cur_taxid =		0;
	FILE *	tax_file_ptr =	NULL;


	if(taxid == 0) {
		ErrPostEx(SEV_ERROR, 0, 0, "GetTaxNameByTaxId: Passed zero for taxid");
		return NULL;
	}

	

	path = MemNew((size_t) PATH_MAX*sizeof(Char));
	tax_list_file = MemNew((size_t) PATH_MAX*sizeof(Char));

	/*get directions to NCBI data directory from NCBI config file*/
	if(GetAppParam("NCBI", "NCBI", "DATA", "", path, PATH_MAX*sizeof(Char))) 
	{
		if(GetAppParam("prebind", "data", "org_name_list", "taxlist.txt", tax_list_file, PATH_MAX*sizeof(Char))) 
		{
			StringCat(path, DIRDELIMSTR);
			StringCat(path, tax_list_file);
			tax_file_ptr = FileOpen(path, "r");
		}
		else  {
			ErrPostEx(SEV_FATAL, 0, 0, "GetTaxNameByTaxId: Can't find prebind configuration file.");
			return NULL;
		}
	}
	else {
		ErrPostEx(SEV_FATAL, 0, 0, "GetTaxNameByTaxId: Can't find NCBI configuration file.");
		return NULL;
	}

	path = MemFree(path);
	tax_list_file = MemFree(tax_list_file);

	if(tax_file_ptr == NULL) {
		ErrPostEx(SEV_FATAL, 0, 0, "GetTaxNameByTaxId: Unable to open NCBI taxonomy data file.");
		return NULL;
	}

	string = MemNew((size_t) 512*sizeof(Char));

	/*read a line at a time and look for the taxon id*/
	fgets(string, 511, tax_file_ptr);	/*skip header line*/
	while(!feof(tax_file_ptr)) 
	{
		fgets(string, 511, tax_file_ptr);

		name_ptr = StringTok(string, "\t"); /*org name is the first string on the line*/
		
		/*get the taxon id: it's the last number on the line*/
		while (TRUE) 
		{
			token = StringTok(NULL, "\t"); 
			if (token == NULL) 
				break;
			cur_taxid = atoi(token);
		}

		/*check if this is the taxon name you want*/
		if (taxid == cur_taxid)
		{
			taxname = StrSave(name_ptr);
			break;
		}

		/*otherwise go to the next line*/

	}

	string = MemFree(string);

	/*files*/
	if(tax_file_ptr)
	FileClose(tax_file_ptr);

	return(taxname);
}


/********************************************************/
/*														*/
/*	CheckForSubmitErrorsUsingSeqHound:				 	*/
/*		Checks that a gi is present in SeqHound	
/*		and if it isnt, an alternative gi is suggested.	*/
/*		Also checks that the submitted molecule type	*/
/*		matches the gi type in SeqHound					*/
/*	PARAMETERS:											*/
/*			see below									*/
/*	RETURNS:											*/
/*			Boolean		0	SeqHound was not available	*/
/*						-1	there was an error			*/
/*						+1  there was no error			*/
/********************************************************/
Int2	CheckForSubmitErrorsUsingSeqHound(Int4 submitted_gi, Int4 submitted_type)	
{
	Int2					return_value = 0;
	PREBINDAccPtr			acc_rec_ptr =	NULL;
	Int4					temp_gi = 0;
	CharPtr					acc_num = NULL;
	Boolean					gi_found = FALSE;
	Boolean					type_ok = FALSE;
	
	/* initialize the SeqHound system */
    if (!SHoundIsInited()) 
	{
		SHoundInitReadOnly(FALSE, "prebind");
	}

	if (!SHoundIsInited())
	{
		ErrPostEx(SEV_ERROR,0,0,"CheckForSubmitErrorsUsingSeqHound: Unable to Initialize SeqHound.");
		return 0;
	}
	else /*go ahead with the check*/
	{
		/*check that the gi exists in SeqHound*/
		if (SHoundExists(submitted_gi))
		{
			gi_found = TRUE;
		}
		else
		{
			ErrPostEx(SEV_ERROR,0,0,"There was no record in SeqHound for gi %ld.\n", submitted_gi);
			fprintf (stdout,"<tr><td>There was no record in SeqHound for gi %ld.<br>\n", submitted_gi);
			
			/*if not, check for an equivalent gi based on the accession*/
			acc_rec_ptr = GetAccessionRecordbyGI(submitted_gi);
			if (acc_rec_ptr != NULL)
			{
				acc_num = acc_rec_ptr->acc;
				if (acc_num != NULL)
				{
					temp_gi = SHoundFindAcc(acc_num);
					if (temp_gi > 0)
					{
						fprintf (stdout,"<tr><td>Try replacing gi %ld with gi %ld which refers to the same accession number %s <br>\n", submitted_gi, temp_gi, acc_num);
						gi_found = TRUE;
						return -1;
					}
				}
			}
		}
		
		if (!gi_found)
		{
			fprintf (stdout,"<tr><td>Unable to find an equivalent gi in SeqHound.<br>\n");
			return -1;
		}

		
		/*check to see if molecule type is correct*/
		if (submitted_type == 2) /*user thinks its a protein*/
		{
			if (!SHoundIsProtein(submitted_gi))
			{
				ErrPostEx(SEV_ERROR,0,0,"According to SeqHound, gi %ld is not a protein.\n", submitted_gi);
				fprintf (stdout,"<tr><td>According to SeqHound, gi %ld is not a protein.  Check the molecule type.<br>\n", submitted_gi);
				/*the record should not be made*/
				return -1;
			}
			else
				type_ok = TRUE;
		}
		else
			type_ok = TRUE;
	}

	if (gi_found && type_ok)
		return 1;
	else
		return -1;
}


/********************************************************/
/*	ConstructAMolecularObject							*/
/*		Returns a pointer to a BIND Object suitable	*/
/*		for use in construction of a BIND record		*/
/*	PARAMETERS:											*/
/*			see below									*/
/*	RETURNS:											*/
/*			BINDObjectPtr	non-NULL success			*/
/*							NULL = error				*/
/********************************************************/

BINDObjectPtr ConstructAMolecularObject(CharPtr name, CharPtr descr, Int4 molecule_type, Int4 gi)
{
	BINDObjectPtr			bop =			NULL;
	BINDIdPtr				bidp =			NULL;
	BINDObjectTypeIdPtr		id =			NULL;
	Int4					taxid =			0;
	CharPtr					taxname=		NULL;
	OrgRefPtr				orp =			NULL;
	BioSourcePtr			bsp =			NULL;
	BINDObjectOriginPtr		origin=			NULL;
	
	/* initialize the SeqHound system */
    if (!SHoundIsInited()) SHoundInitReadOnly(FALSE, "prebind"); 
	if (!SHoundIsInited())
	{
		ErrPostEx(SEV_ERROR,0,0,"ConstructABINDObject: Unable to Initialize SeqHound.");
	}
	
	/*STEP 1, construct an id object and return a pointer to it*/
	bidp = C_BINDId_New(gi, 0, NULL);
	if (bidp != NULL)
	{
		if (molecule_type == 1) 
		{
			id = C_BINDObjectTypeId_New(BINDObjectTypeId_dna, 0, bidp, NULL);
		}
		if (molecule_type == 2) 
		{
			id = C_BINDObjectTypeId_New(BINDObjectTypeId_protein, 0, bidp, NULL);
		}
	}
	if (id == NULL)
	{
		if (SHoundIsInited()) SHoundFini();
		ErrPostEx(SEV_ERROR,0,0,"ConstructABINDObject: Unable to create id.");
		return NULL;
	}
	
	/*STEP 2, construct an origin object and return a pointer to it*/
	if (SHoundIsInited())
	{
		taxid = SHoundTaxIDFromGi(gi);
		if (taxid > 0)
		{
			taxname = TaxDB_GetNameFromTaxIDByClass(taxid, 12);
		}
	}
	if ((!SHoundIsInited()) || (taxid < 1))
	{
		taxid = GetTaxidbyGI(gi);
		if (taxid < 1)
		{
			/*a taxid cannot be found*/
			ErrPostEx(SEV_ERROR,0,0,"ConstructABINDObject: Unable to find taxid.");
			if (SHoundIsInited()) SHoundFini();
			return NULL;
		}
		
	}	
	if ((!SHoundIsInited()) || (taxname == NULL))
	{
		taxname = GetTaxNameByTaxId(taxid);
		if (taxname == NULL)
		{
			/*a tax name cannot be found*/
			ErrPostEx(SEV_ERROR,0,0,"ConstructABINDObject: Unable to find a taxon name.");
			if (SHoundIsInited()) SHoundFini();
			return NULL;
		}
	}
	
	orp = C_OrgRef_New(taxname, taxid);
	if (orp != NULL)
	{
		bsp = C_BioSource_New(orp);
		if (bsp != NULL)
		{
			origin = C_BINDObjectOrigin_New(bsp, NULL);
			if (origin == NULL)
			{
				if (SHoundIsInited()) SHoundFini();
				ErrPostEx(SEV_ERROR,0,0,"ConstructABINDObject: Failed to create an origin object.");
				return NULL;
			}
		}
	}
	
	/*STEP 3 and last, construct the new BIND object and return a pointer to it*/
	bop = C_BINDObject_New(name, id, origin);
	if (bop == NULL)
	{
		if (SHoundIsInited()) SHoundFini();
		ErrPostEx(SEV_ERROR,0,0,"ConstructABINDObject: Failed to create a BIND object pointer");
		return NULL;
	}
	else
	{
		C_BINDObject_AddDescr(bop, descr);
	}
	
	return bop;

}


/*
$Log: prebind_lib.c,v $
Revision 1.10  2003/02/20 22:34:10  janicek
minor changes

Revision 1.9  2002/12/02 20:13:32  janicek
removed debug statements and comments

Revision 1.8  2002/12/02 19:55:17  janicek
Dont close SHound inside library function

Revision 1.7  2002/12/02 19:13:10  janicek
Added more cgi debug points

Revision 1.6  2002/11/11 22:14:48  janicek
minor changes

Revision 1.5  2002/11/11 21:51:20  janicek
minor changes

Revision 1.4  2002/11/11 21:44:06  janicek
Check before SHoundInit and SHoundFini

Revision 1.3  2002/11/11 16:52:15  janicek
Changed SHoundInit to SHoundInitReadOnly

Revision 1.2  2002/07/25 16:06:12  janicek
fixed cvs log

*/




