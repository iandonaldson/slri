/*$Id: rpsdbapi.h,v 1.4 2004/04/16 15:24:20 mmatan Exp $*/
/*
Copyright (C) 2001 Mount Sinai Hospital (MSH)

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


  PROGRAM: RPSDB API layer


  AUTHORS:    Doron Betel           betel@mshri.on.ca
            and Christopher W.V. Hogue hogue@mshri.on.ca


  REVISION/CONTRIBUTION NOTES:

  March 1, 2001  Original MSH Revision.
                Hogue Lab - University of Toronto Biochemistry
                Department and the Samuel Lunenfeld Research
                Institute, Mount Sinai Hospital
                http://bioinfo.mshri.on.ca  hogue@mshri.on.ca
                ftp://ftp.mshri.on.ca/


  PUBLICATION TO CITE:



***************************************************************************
*/
#ifndef _RPSDB_API_H
#define _RPSDB_API_H

#include <asn.h>
#include <objslristruc.h>

#ifdef __cplusplus
extern "C" { /* } */
#endif


/***************************
*RPSDB & DOMNAME api layer
****************************/
/*Initalize RPSDB */
Boolean InitRPSDB(CharPtr database_name);
Boolean FiniRPSDB(void);
Boolean InitDOMNAME(CharPtr database_name);
Boolean FiniDOMNAME(void);

/************************
* RPSDB searches
************************/
/*Is RPSDB open for business? */
Boolean HelloRPSDBAreYouOpen(void);

/* Get the list of domains for a protein*/
StRpsdbPtr LIBCALL SHoundGetDomainsFromGi(Int4 gi);

/* Get a list of filtered domains for a protein */
StRpsdbPtr LIBCALL SHoundGetFilteredDomainsFromGi(Int4 gi, FloatHi coverage_filter);

/* Get the list of domains for a protein with score <cutoff> or lower */
StRpsdbPtr LIBCALL SHoundGetDomainsFromGiWithEvalue(Int4 gi, FloatHi eValueCutoff);

/* Get the list of domains from a list of proteins*/
StRpsdbPtr LIBCALL SHoundGetDomainsFromGiListWithEvalue(ValNodePtr pvnGis, FloatHi eValueCutoff);

/* Get the list of domains from a list of proteins*/
StRpsdbPtr LIBCALL SHoundGetFilteredDomainsFromGiWithEvalue(Int4 gi, FloatHi eValueCutoff, FloatHi coverage_filter );

/*Search for proteins by domain id */
ValNodePtr LIBCALL SHoundGetGisByDomainId(CharPtr domainId);

/*Search for proteins with domain-id below cutoff evalue score */
ValNodePtr LIBCALL SHoundGetGisByDomainIdAndEvalue(CharPtr domainId, FloatHi eValueCutoff);

/*Get all Gi's that have <numdom> or more domains */
ValNodePtr LIBCALL SHoundGetGisByNumberOfDomains(Int4 numdom);

/*******************
* DOMNAME searches
*******************/
/*Is anyone home ?*/
Boolean DOMNAMEAreYouListening(void);

/*Get short label (e.g. SH3) from Domain Id (i.e. Pfam or Smart id) */
CharPtr LIBCALL SHoundGetDomainLabelFromDomainId(CharPtr domainId);

/*Get the Domain Id (i.e. Pfam or Smart id) from short label (e.g. SH3) */
ValNodePtr LIBCALL SHoundGetDomainIdFromLabel(CharPtr label);

/*Get PDB-ID 3D structure representative of the domain from domain ID*/
CharPtr LIBCALL SHoundGetDomain3DStructure (CharPtr domainId);
/***********************
*Relational Searches
***********************/

ValNodePtr SHoundGetGisByDomainLabel(CharPtr label);
ValNodePtr SHoundGetGisByDomainLabelAndEvalue(CharPtr label, FloatHi eValueCutoff);

ValNodePtr My_Misc_CommaStrString2ValNodeListEx(CharPtr string, CharPtr seps, Boolean maintainOrder);
    ValNodePtr LIBCALL My_ValNodeAddStr (ValNodePtr PNTR head, Nlm_Int2 choice, Nlm_CharPtr str);

#ifdef __cplusplus
/* { */ }
#endif

#endif
/*
$Log: rpsdbapi.h,v $
Revision 1.4  2004/04/16 15:24:20  mmatan
Include declaration for SHoundGetFilteredDomainsFromGiWithEvalue.  See bug 1847.

Revision 1.3  2004/04/16 14:25:00  ryao
Added SHoundGetFilteredDomainsFromGi. bug [1847] related.

Revision 1.2  2004/01/13 21:49:56  rcavero
nblast_odbc.h it's a draf version for nblast's ODBC port.

Revision 1.1.1.9  2003/07/25 08:55:19  root
Automated import from Sourceforge.net

Revision 1.10  2003/07/24 15:34:29  haocl
added macro __cplusplus

Revision 1.9  2002/10/25 00:05:41  betel
Added 3D strcture function for domains

Revision 1.8  2002/09/30 18:55:05  gbader
Added missing required header file and standardized parameter names (now matches functions)

Revision 1.7  2002/07/29 22:30:46  betel
Added GetDomainsFromGiListWithEvalue for local&remote C api (no Perl or C++ yet)

Revision 1.6  2002/07/11 16:52:20  micheld
added LIBCALL to prototype functions

Revision 1.5  2002/07/08 17:02:33  kaca
renamed two RPS-BLAST functions Accession is DomainId

Revision 1.4  2002/05/17 15:06:10  betel
Fixed Windows MSVC build

Revision 1.3  2002/04/11 20:45:27  betel
Removed redundant function declaration

Revision 1.2  2002/04/02 23:12:19  kaca
changed all API calls to SHound* to accomodate general SHound naming scheme

Revision 1.1  2002/04/02 21:41:54  kaca
moved from ../rps

Revision 1.2  2002/03/18 21:57:27  betel
Added Domain Table searches and lookup

Revision 1.1  2002/03/15 22:26:52  betel
RPSDB API layer

*/
