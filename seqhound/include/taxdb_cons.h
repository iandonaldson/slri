/* $Id: taxdb_cons.h,v 1.1.1.1 2001/10/11 17:37:13 gbader Exp $
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


#ifndef _TAXDB_CONS_
#define _TAXDB_CONS_

#ifdef __cplusplus
extern "C" {
#endif

#include <asn.h>
#include <objslritax.h>
#include <slri_misc.h>

/************************/
/* Function definitions */
/************************/

/*Create an SLRITaxon object*/
extern SLRITaxonPtr C_SLRITaxon_New(Int4 taxId, Int4 parentTaxId, SLRITaxonNamePtr stnpNames, SLRITaxonRankPtr strpRank, Int4 divId, Int4 genCodeId, Int4 mitoGenCodeId);

/*adds a child tax ID to an SLRITaxon object*/
extern Int2 C_SLRITaxon_AddChildTaxId(SLRITaxonPtr stp, Int4 childTaxId);

/*adds an SLRITaxonName to an SLRITaxon object*/
extern Int2 C_SLRITaxon_AddTaxName(SLRITaxonPtr stp, SLRITaxonNamePtr stnp);

/*adds an EMBL Code to an SLRITaxon object*/
extern Int2 C_SLRITaxon_AddEMBLCode(SLRITaxonPtr stp, CharPtr emblCode);

/*adds a comment to an SLRITaxon object*/
extern Int2 C_SLRITaxon_AddComments(SLRITaxonPtr stp, CharPtr comments);

/*create a new SLRITaxonName object*/
extern SLRITaxonNamePtr C_SLRITaxonName_New(CharPtr name, Uint2 nameClass);

/*adds a unique name to a SLRITaxonName object*/
extern Int2 C_SLRITaxonName_AddUniqueName(SLRITaxonNamePtr stnp, CharPtr uniqueName);

/*adds a other-class to a SLRITaxonName object*/
extern Int2 C_SLRITaxonName_AddOtherClass(SLRITaxonNamePtr stnp, CharPtr otherClass);

/*create a new SLRITaxonRank object*/
extern SLRITaxonRankPtr C_SLRITaxonRank_New(Uint2 rank);

/*adds a other-rank to a SLRITaxonRank object*/
extern Int2 C_SLRITaxonRank_AddOtherRank(SLRITaxonRankPtr strp, CharPtr otherRank);

/*adds a pre-modifier to a SLRITaxonRank object*/
extern Int2 C_SLRITaxonRank_AddPreMod(SLRITaxonRankPtr strp, Uint2 preMod, CharPtr otherPreMod);

/*adds a post-modifier to a SLRITaxonRank object*/
extern Int2 C_SLRITaxonRank_AddPostMod(SLRITaxonRankPtr strp, Uint2 postMod, CharPtr otherPostMod);

/*create a new SLRITaxonDiv object*/
extern SLRITaxonDivPtr C_SLRITaxonDiv_New(Int4 divId, CharPtr divCode, CharPtr divName);

/*adds a comment to an SLRITaxonDiv object*/
extern Int2 C_SLRITaxonDiv_AddComments(SLRITaxonDivPtr stdp, CharPtr comments);

/*create a new SLRITaxonGencode object*/
extern SLRITaxonGencodePtr C_SLRITaxonGencode_New(Int4 genCodeId, CharPtr name);

/*adds a abbrev to an SLRITaxonGencode object*/
extern Int2 C_SLRITaxonGencode_AddAbbrev(SLRITaxonGencodePtr stgp, CharPtr abbrev);

/*adds a transTable to an SLRITaxonGencode object*/
extern Int2 C_SLRITaxonGencode_AddTransTable(SLRITaxonGencodePtr stgp, CharPtr transTable);

/*adds a startCodons field to an SLRITaxonGencode object*/
extern Int2 C_SLRITaxonGencode_AddStartCodons(SLRITaxonGencodePtr stgp, CharPtr startCodons);

#ifdef __cplusplus
}
#endif
#endif		/* _TAXDB_CONS_ */


/*
$Log: taxdb_cons.h,v $
Revision 1.1.1.1  2001/10/11 17:37:13  gbader
Automated import from Sourceforge.net

Revision 1.1.1.1  2001/05/25 21:36:20  kaca
initial import

Revision 1.3  2001/04/02 15:00:40  kaca
removed some extern declarations following the function type

Revision 1.2  2001/03/29 22:14:59  gbader
Moved statics to before function prototype

Revision 1.1  2001/03/20 16:20:24  kaca
migrated from the ../taxon directory

Revision 1.5  2001/02/23 21:20:11  kaca
license change

Revision 1.4  2000/09/20 16:31:27  kaca
fixed Log keyword

Revision 1.3  2000/09/19 23:44:42  kaca
added CVS Id and Log keywords

*/




