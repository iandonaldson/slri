/*  $Id: taxdb_cons.c,v 1.1.1.1 2001/10/11 17:37:13 gbader Exp $
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

#include <taxdb_cons.h>

/***********************/
/*Constructor Functions*/
/***********************/

/*Create an SLRITaxon object*/
SLRITaxonPtr C_SLRITaxon_New(Int4 taxId, Int4 parentTaxId, SLRITaxonNamePtr stnpNames, SLRITaxonRankPtr strpRank, Int4 divId, Int4 genCodeId, Int4 mitoGenCodeId)
{
	SLRITaxonPtr stp=NULL;

	if(taxId < 1) {
		ErrPostEx(SEV_INFO, 0, 0, "C_SLRITaxon_New: Tax ID is < 1.");
		return(NULL);
	}
	if(parentTaxId < 1) {
		ErrPostEx(SEV_INFO, 0, 0, "C_SLRITaxon_New: Parent tax ID is < 1.");
		return(NULL);
	}
	if(stnpNames==NULL) {
		ErrPostEx(SEV_INFO, 0, 0, "C_SLRITaxon_New: SLRITaxonNamePtr was NULL.");
		return(NULL);
	}
	if(strpRank==NULL) {
		ErrPostEx(SEV_INFO, 0, 0, "C_SLRITaxon_New: SLRITaxonRankPtr was NULL.");
		return(NULL);
	}
	if(divId<0) {
		ErrPostEx(SEV_INFO, 0, 0, "C_SLRITaxon_New: divId was < 0.");
		return(NULL);
	}
	if(genCodeId<0) {
		ErrPostEx(SEV_INFO, 0, 0, "C_SLRITaxon_New: genCodeId was < 0.");
		return(NULL);
	}
	if(mitoGenCodeId<0) {
		ErrPostEx(SEV_INFO, 0, 0, "C_SLRITaxon_New: mitoGenCodeId was < 0.");
		return(NULL);
	}

	stp = SLRITaxonNew();
	if(stp==NULL) {
		ErrPostEx(SEV_INFO, 0, 0, "C_SLRITaxon_New: SLRITaxonNew failed.");
		return(NULL);
	}

	stp->taxId = taxId;
	stp->parent_taxId = parentTaxId;
	stp->names = stnpNames;
	stp->rank = strpRank;
	stp->division = divId;
	stp->gencode = genCodeId;
	stp->mito_gencode = mitoGenCodeId;

	return(stp);
}

/*adds a child tax ID to an SLRITaxon object*/
Int2 C_SLRITaxon_AddChildTaxId(SLRITaxonPtr stp, Int4 childTaxId)
{
	if(stp==NULL) {
		ErrPostEx(SEV_INFO, 0, 0, "C_SLRITaxon_AddChildTaxId: SLRITaxonPtr was NULL.");
		return(1);
	}
	if(childTaxId<0) {
		ErrPostEx(SEV_INFO, 0, 0, "C_SLRITaxon_AddChildTaxId: childTaxId was < 0.");
		return(1);
	}

	Misc_AddInt2SortedList(&(stp->children_taxId), childTaxId, 0, TRUE);

	return(0);
}

/*adds an SLRITaxonName to an SLRITaxon object*/
Int2 C_SLRITaxon_AddTaxName(SLRITaxonPtr stp, SLRITaxonNamePtr stnp)
{
	if(stp==NULL) {
		ErrPostEx(SEV_INFO, 0, 0, "C_SLRITaxon_AddTaxName: SLRITaxonPtr was NULL.");
		return(1);
	}
	if(stnp==NULL) {
		ErrPostEx(SEV_INFO, 0, 0, "C_SLRITaxon_AddTaxName: SLRITaxonNamePtr was NULL.");
		return(1);
	}

	/*add name to end of names list*/
	Misc_GenericAddObjectToListEnd((Pointer *)(&(stp->names)), stnp);

	return(0);
}

/*adds an EMBL Code to an SLRITaxon object*/
Int2 C_SLRITaxon_AddEMBLCode(SLRITaxonPtr stp, CharPtr emblCode)
{
	if(stp==NULL) {
		ErrPostEx(SEV_INFO, 0, 0, "C_SLRITaxon_AddEMBLCode: SLRITaxonPtr was NULL.");
		return(1);
	}
	if(emblCode==NULL) {
		ErrPostEx(SEV_INFO, 0, 0, "C_SLRITaxon_AddEMBLCode: emblCode was NULL.");
		return(1);
	}

	stp->embl_code = StringSave(emblCode);

	return(0);
}

/*adds a comment to an SLRITaxon object*/
Int2 C_SLRITaxon_AddComments(SLRITaxonPtr stp, CharPtr comments)
{
	if(stp==NULL) {
		ErrPostEx(SEV_INFO, 0, 0, "C_SLRITaxon_AddComments: SLRITaxonPtr was NULL.");
		return(1);
	}
	if(comments==NULL) {
		ErrPostEx(SEV_INFO, 0, 0, "C_SLRITaxon_AddComments: comments was NULL.");
		return(1);
	}

	stp->comments = StringSave(comments);

	return(0);
}

/*create a new SLRITaxonName object*/
SLRITaxonNamePtr C_SLRITaxonName_New(CharPtr name, Uint2 nameClass)
{
	SLRITaxonNamePtr stnp=NULL;

	if(name==NULL) {
		ErrPostEx(SEV_INFO, 0, 0, "C_SLRITaxonName_New: name was NULL.");
		return(NULL);
	}

	stnp = SLRITaxonNameNew();
	if(stnp==NULL) {
		ErrPostEx(SEV_INFO, 0, 0, "C_SLRITaxonName_New: SLRITaxonNameNew failed.");
		return(NULL);
	}

	stnp->name = StringSave(name);
	stnp->name_class = nameClass;

	return(stnp);
}

/*adds a unique name to a SLRITaxonName object*/
Int2 C_SLRITaxonName_AddUniqueName(SLRITaxonNamePtr stnp, CharPtr uniqueName)
{
	if(stnp==NULL) {
		ErrPostEx(SEV_INFO, 0, 0, "C_SLRITaxonName_AddUniqueName: SLRITaxonNamePtr was NULL.");
		return(1);
	}
	if(uniqueName==NULL) {
		ErrPostEx(SEV_INFO, 0, 0, "C_SLRITaxonName_AddUniqueName: uniqueName was NULL.");
		return(1);
	}

	stnp->unique_name = StringSave(uniqueName);

	return(0);
}

/*adds a other-class to a SLRITaxonName object*/
Int2 C_SLRITaxonName_AddOtherClass(SLRITaxonNamePtr stnp, CharPtr otherClass)
{
	if(stnp==NULL) {
		ErrPostEx(SEV_INFO, 0, 0, "C_SLRITaxonName_AddOtherClass: SLRITaxonNamePtr was NULL.");
		return(1);
	}
	if(otherClass==NULL) {
		ErrPostEx(SEV_INFO, 0, 0, "C_SLRITaxonName_AddOtherClass: otherClass was NULL.");
		return(1);
	}

	stnp->other_class = StringSave(otherClass);

	return(0);
}

/*create a new SLRITaxonRank object*/
SLRITaxonRankPtr C_SLRITaxonRank_New(Uint2 rank)
{
	SLRITaxonRankPtr strp=NULL;

	strp = SLRITaxonRankNew();
	if(strp==NULL) {
		ErrPostEx(SEV_INFO, 0, 0, "C_SLRITaxonRank_New: SLRITaxonRankNew failed.");
		return(NULL);
	}

	strp->rank = rank;

	return(strp);
}

/*adds a other-rank to a SLRITaxonRank object*/
Int2 C_SLRITaxonRank_AddOtherRank(SLRITaxonRankPtr strp, CharPtr otherRank)
{
	if(strp==NULL) {
		ErrPostEx(SEV_INFO, 0, 0, "C_SLRITaxonRank_AddOtherRank: SLRITaxonRankPtr was NULL.");
		return(1);
	}
	if(otherRank==NULL) {
		ErrPostEx(SEV_INFO, 0, 0, "C_SLRITaxonRank_AddOtherRank: otherRank was NULL.");
		return(1);
	}

	strp->other_rank = StringSave(otherRank);

	return(0);
}

/*adds a pre-modifier to a SLRITaxonRank object*/
Int2 C_SLRITaxonRank_AddPreMod(SLRITaxonRankPtr strp, Uint2 preMod, CharPtr otherPreMod)
{
	if(strp==NULL) {
		ErrPostEx(SEV_INFO, 0, 0, "C_SLRITaxonRank_AddPreMod: SLRITaxonRankPtr was NULL.");
		return(1);
	}
	if((preMod==SLRI_taxon_rank_premod_other)&&(otherPreMod==NULL)) {
		ErrPostEx(SEV_INFO, 0, 0, "C_SLRITaxonRank_AddPreMod: otherPreMod was NULL.");
		return(1);
	}

	strp->premod = preMod;
	if(preMod==SLRI_taxon_rank_premod_other) {
		strp->other_premod = StringSave(otherPreMod);
	}

	return(0);
}

/*adds a post-modifier to a SLRITaxonRank object*/
Int2 C_SLRITaxonRank_AddPostMod(SLRITaxonRankPtr strp, Uint2 postMod, CharPtr otherPostMod)
{
	if(strp==NULL) {
		ErrPostEx(SEV_INFO, 0, 0, "C_SLRITaxonRank_AddPostMod: SLRITaxonRankPtr was NULL.");
		return(1);
	}
	if((postMod==SLRI_taxon_rank_postmod_other)&&(otherPostMod==NULL)) {
		ErrPostEx(SEV_INFO, 0, 0, "C_SLRITaxonRank_AddPostMod: otherPostMod was NULL.");
		return(1);
	}

	strp->postmod = postMod;
	if(postMod==SLRI_taxon_rank_postmod_other) {
		strp->other_postmod = StringSave(otherPostMod);
	}

	return(0);
}

/*create a new SLRITaxonDiv object*/
SLRITaxonDivPtr C_SLRITaxonDiv_New(Int4 divId, CharPtr divCode, CharPtr divName)
{
	SLRITaxonDivPtr stdp=NULL;

	if(divId<0) {
		ErrPostEx(SEV_INFO, 0, 0, "C_SLRITaxonDiv_New: divId was < 0.");
		return(NULL);
	}
	if(divCode==NULL) {
		ErrPostEx(SEV_INFO, 0, 0, "C_SLRITaxonDiv_New: divCode was NULL.");
		return(NULL);
	}
	if(divName==NULL) {
		ErrPostEx(SEV_INFO, 0, 0, "C_SLRITaxonDiv_New: divName was NULL.");
		return(NULL);
	}

	stdp = SLRITaxonDivNew();
	if(stdp==NULL) {
		ErrPostEx(SEV_INFO, 0, 0, "C_SLRITaxonDiv_New: SLRITaxonDivNew failed.");
		return(NULL);
	}

	stdp->div_id = divId;
	stdp->div_code = StringSave(divCode);
	stdp->div_name = StringSave(divName);

	return(stdp);
}

/*adds a comment to an SLRITaxonDiv object*/
Int2 C_SLRITaxonDiv_AddComments(SLRITaxonDivPtr stdp, CharPtr comments)
{
	if(stdp==NULL) {
		ErrPostEx(SEV_INFO, 0, 0, "C_SLRITaxonDiv_AddComments: SLRITaxonDivPtr was NULL.");
		return(1);
	}
	if(comments==NULL) {
		ErrPostEx(SEV_INFO, 0, 0, "C_SLRITaxonDiv_AddComments: comments was NULL.");
		return(1);
	}

	stdp->comments = StringSave(comments);

	return(0);
}

/*create a new SLRITaxonGencode object*/
SLRITaxonGencodePtr C_SLRITaxonGencode_New(Int4 genCodeId, CharPtr name)
{
	SLRITaxonGencodePtr stgp=NULL;

	if(genCodeId<0) {
		ErrPostEx(SEV_INFO, 0, 0, "C_SLRITaxonGencode_New: genCodeId was < 0.");
		return(NULL);
	}
	if(name==NULL) {
		ErrPostEx(SEV_INFO, 0, 0, "C_SLRITaxonGencode_New: name was NULL.");
		return(NULL);
	}

	stgp = SLRITaxonGencodeNew();
	if(stgp==NULL) {
		ErrPostEx(SEV_INFO, 0, 0, "C_SLRITaxonGencode_New: SLRITaxonGencodeNew failed.");
		return(NULL);
	}

	stgp->gencode_id = genCodeId;
	stgp->name = StringSave(name);

	return(stgp);
}

/*adds a abbrev to an SLRITaxonGencode object*/
Int2 C_SLRITaxonGencode_AddAbbrev(SLRITaxonGencodePtr stgp, CharPtr abbrev)
{
	if(stgp==NULL) {
		ErrPostEx(SEV_INFO, 0, 0, "C_SLRITaxonGencode_AddAbbrev: SLRITaxonGencodePtr was NULL.");
		return(1);
	}
	if(abbrev==NULL) {
		ErrPostEx(SEV_INFO, 0, 0, "C_SLRITaxonGencode_AddAbbrev: abbrev was NULL.");
		return(1);
	}

	stgp->abbrev = StringSave(abbrev);

	return(0);
}

/*adds a transTable to an SLRITaxonGencode object*/
Int2 C_SLRITaxonGencode_AddTransTable(SLRITaxonGencodePtr stgp, CharPtr transTable)
{
	if(stgp==NULL) {
		ErrPostEx(SEV_INFO, 0, 0, "C_SLRITaxonGencode_AddTransTable: SLRITaxonGencodePtr was NULL.");
		return(1);
	}
	if(transTable==NULL) {
		ErrPostEx(SEV_INFO, 0, 0, "C_SLRITaxonGencode_AddTransTable: transTable was NULL.");
		return(1);
	}

	stgp->trans_table = StringSave(transTable);

	return(0);
}

/*adds a startCodons field to an SLRITaxonGencode object*/
Int2 C_SLRITaxonGencode_AddStartCodons(SLRITaxonGencodePtr stgp, CharPtr startCodons)
{
	if(stgp==NULL) {
		ErrPostEx(SEV_INFO, 0, 0, "C_SLRITaxonGencode_AddStartCodons: SLRITaxonGencodePtr was NULL.");
		return(1);
	}
	if(startCodons==NULL) {
		ErrPostEx(SEV_INFO, 0, 0, "C_SLRITaxonGencode_AddStartCodons: startCodons was NULL.");
		return(1);
	}

	stgp->start_codons = StringSave(startCodons);

	return(0);
}

/* 
$Log: taxdb_cons.c,v $
Revision 1.1.1.1  2001/10/11 17:37:13  gbader
Automated import from Sourceforge.net

Revision 1.1.1.1  2001/05/25 21:36:24  kaca
initial import

Revision 1.1  2001/03/20 16:22:28  kaca
migrated from ../taxon directory; library shoundtaxlib merged with shoundlocllib

Revision 1.4  2001/02/23 21:20:11  kaca
license change

Revision 1.3  2000/09/20 16:31:27  kaca
fixed Log keyword

 Revision 1.2  2000/09/19 23:44:42  kaca
 added CVS Id and Log keywords
*/


