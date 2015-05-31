/*  $Id: taxdb_enum.c,v 1.1.1.2 2002/03/04 09:25:30 root Exp $
-- ***************************************************************************
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
           Gary Bader and Christopher W.V. Hogue hogue@mshri.on.ca


  REVISION/CONTRIBUTION NOTES:

  June 1, 2000  Original MSH Revision.
                Hogue Lab - University of Toronto Biochemistry
                Department and the Samuel Lunenfeld Research
                Institute, Mount Sinai Hospital
                http://bioinfo.mshri.on.ca  hogue@mshri.on.ca
                ftp://ftp.mshri.on.ca/


  PUBLICATION TO CITE:


  DESCRIPTION
  taxonomy database part of SeqHound


******************************************************************************************/

#include <taxdb_enum.h>
#include <taxdb_enum_p.h>

/***********************/
/*Enumeration functions*/
/***********************/

Uint2 TaxDB_Enum_NameClassString2Enum(CharPtr nameClass)
{
	Int4 retVal=0;

	if(nameClass==NULL) {
		ErrPostEx(SEV_INFO, 0, 0, "TaxDB_Enum_NameClassString2Enum: Passed nameClass was null.");
		return(0);
	}

	retVal = Misc_IndexCharPtrArray(SLRITaxonName_NameClassEnumStrList, SLRITaxonName_NAMECLASS_NUM_ELMTS, nameClass);

	if(retVal==-1)
		return(SLRI_taxon_name_name_class_other);

	return((Uint2)retVal);
}

Uint2 TaxDB_Enum_RankString2Enum(CharPtr rank)
{
	Int4 retVal=0;

	if(rank==NULL) {
		ErrPostEx(SEV_INFO, 0, 0, "TaxDB_Enum_RankString2Enum: Passed rank was null.");
		return(0);
	}

	retVal = Misc_IndexCharPtrArray(SLRITaxonRank_RankEnumStrList, SLRITaxonRank_RANK_NUM_ELMTS, rank);

	if(retVal==-1) {
		/*try the synonym array before returning*/
		retVal = Misc_IndexCharPtrArray(SLRITaxonRank_RankEnumStrList2, SLRITaxonRank_RANK_NUM_ELMTS, rank);
		if(retVal==-1) {
			return(SLRI_taxon_rank_rank_other);
		}
	}

	return((Uint2)retVal);
}

Uint2 TaxDB_Enum_RankPremodString2Enum(CharPtr premod)
{
	Int4 retVal=0;

	if(premod==NULL) {
		ErrPostEx(SEV_INFO, 0, 0, "TaxDB_Enum_RankPremodString2Enum: Passed premod was null.");
		return(0);
	}

	retVal = Misc_IndexCharPtrArray(SLRITaxonRank_PremodEnumStrList, SLRITaxonRank_PREMOD_NUM_ELMTS, premod);

	if(retVal==-1)
		return(SLRI_taxon_rank_premod_other);

	return((Uint2)retVal);
}

Uint2 TaxDB_Enum_RankPostmodString2Enum(CharPtr postmod)
{
	Int4 retVal=0;

	if(postmod==NULL) {
		ErrPostEx(SEV_INFO, 0, 0, "TaxDB_Enum_RankPostmodString2Enum: Passed postmod was null.");
		return(0);
	}

	retVal = Misc_IndexCharPtrArray(SLRITaxonRank_PostmodEnumStrList, SLRITaxonRank_POSTMOD_NUM_ELMTS, postmod);

	if(retVal==-1)
		return(SLRI_taxon_rank_postmod_other);

	return((Uint2)retVal);
}

/*tries to find an element of array in string - if found remove from string
-returns the index of array that was found*/
Int4 TaxDB_FindIndexAndStripString(CharPtr *string, CharPtr array[], Int4 numElmts)
{
	Int4 i=0;
	Boolean found=FALSE;
	CharPtr temp=NULL, newString=NULL;

	for(i=0;i<numElmts;i++) {
		if((array[i]!=NULL)&&((temp=strstr((*string), array[i]))!=NULL)) {
			found=TRUE;
			if(temp==(*string)) {
				/*pattern found at start of string*/
				if(strlen(array[i])==strlen(*string)) {
					/*pattern is full string*/
					MemFree(*string);
				}
				else {
					/*pattern is only at start of string, but is not full string*/
					temp = SkipPastString(temp,array[i]);
					newString = StringSave(temp);
					MemFree(*string);
					*string = newString;
				}
			}
			else {
				/*pattern is not at start of string*/
				temp[0]='\0';
			}
			break;
		}
	}

	if(found)
		return(i);

	return(-1);
}

/*process post modification rank name*/
Int4 TaxDB_GetPostModFromRankName(CharPtr *rank)
{
	return(TaxDB_FindIndexAndStripString(rank, SLRITaxonRank_PostmodEnumStrList, SLRITaxonRank_POSTMOD_NUM_ELMTS));
}

/*process pre modification rank name*/
Int4 TaxDB_GetPreModFromRankName(CharPtr *rank)
{
	return(TaxDB_FindIndexAndStripString(rank, SLRITaxonRank_PremodEnumStrList, SLRITaxonRank_PREMOD_NUM_ELMTS));
}


/* 
$Log: taxdb_enum.c,v $
Revision 1.1.1.2  2002/03/04 09:25:30  root
Automated import from Sourceforge.net

Revision 1.2  2002/03/03 23:02:52  gbader
Fixed MSVC compiler warnings - mostly unused variables and uncasted type changes.

Revision 1.1.1.1  2001/05/25 21:36:24  kaca
initial import

Revision 1.1  2001/03/20 16:22:28  kaca
migrated from ../taxon directory; library shoundtaxlib merged with shoundlocllib

Revision 1.5  2001/02/23 21:20:11  kaca
license change

Revision 1.4  2000/09/20 16:31:27  kaca
fixed Log keyword

 Revision 1.3  2000/09/19 23:44:42  kaca
 added CVS Id and Log keywords
*/


