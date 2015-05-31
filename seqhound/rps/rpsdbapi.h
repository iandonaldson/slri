/*$Id*/
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

#include <ncbi.h>
#include "rpsdb_cb.h"


/***************************
*RPSDB & DOMNAME api layer
****************************/
/*Initalize RPSDB */
Boolean InitRPSDB(CharPtr table_name);
Boolean FiniRPSDB(void);
Boolean InitDOMNAME(CharPtr table_name);
Boolean FiniDOMNAME(void);

/************************
* RPSDB searches
************************/
/*Is RPSDB open for business? */
Boolean HelloRPSDBAreYouOpen(void);

/* Get the list of domains for a protein*/
StRpsdbPtr GetDomainsFromGi(Int4 gi);

/* Get the list of domains for a protein with score <cutoff> or lower */
StRpsdbPtr GetDomainsFromGiWithEvalue(Int4 gi, FloatHi cutoff);

/*Search for proteins by domain id */
ValNodePtr GetGisByDomainId(CharPtr domain_name);

/*Search for proteins with domain-id below cutoff evalue score */
ValNodePtr GetGisByDomainIdAndEvalue(CharPtr domain_id, FloatHi cutoff);

/* Get the list of domains for a protein with score <cutoff> or lower
* a second version using codebase relation */
StRpsdbPtr GetDomainsFromGiWithEvalue(Int4 gi, FloatHi cutoff);

/*Get all Gi's that have <numdom> or more domains */
ValNodePtr GetGisByNumberOfDomains(Int4 numdom);

/*******************
* DOMNAME searches
*******************/
/*Is anyone home ?*/
Boolean DOMNAMEAreYouListening(void);

/*Get Short label from Domain accession name (i.e. Pfam or Smart id) */
CharPtr GetDomainLabelFromAccession(CharPtr accession);

/*Get the Domain accession (i.e. Pfam or smart id) from short name */
ValNodePtr GetDomainAccessionFromLabel(CharPtr label);

/***********************
*Relational Searches
***********************/

ValNodePtr GetGisByDomainLabel(CharPtr label);
ValNodePtr GetGisByDomainLabelAndEvalue(CharPtr label, FloatHi cutoff);

#endif
/*
$Log: rpsdbapi.h,v $
Revision 1.1.1.2  2002/03/19 09:27:29  root
Automated import from Sourceforge.net

Revision 1.2  2002/03/18 21:57:27  betel
Added Domain Table searches and lookup

Revision 1.1  2002/03/15 22:26:52  betel
RPSDB API layer

*/
