/*$Id: rpsdbapi.hpp,v 1.1.1.13 2002/11/15 09:34:14 root Exp $*/
/***************************************************************************
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


  PROGRAM: RPSDB & DOMNAME C++ API Layer


  AUTHORS:	Doron Betel betel@mshri.on.ca
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
#ifndef _RPSDB_API_HPP_
#define _RPSDB_API_HPP_
#include "rpsdb_cb.hpp"

/***************************
*RPSDB api layer
****************************/
/*Initalize/Fini RPSDB */
bool InitRPSDB(string table_name);
bool FiniRPSDB(void);
bool IsRPSDBOpen(void);

/************************
* RPSDB searches
************************/
/* Get the list of domains for a protein
* Use GI's with ordinal 1 in SeqHound Redund table*/
list<RpsDBRecord*>* GetDomainsFromGi(int gi);

/*Get the list of domains from Gi with eValue cutoff. Note that the gi
* should be of ordinal 1 in the SeqHound Redund table.
* Calling function must free list unless NULL value returned. */
list<RpsDBRecord*>* GetDomainsFromGiWithEvalue(int gi, double cutoff);

/* Get the list of Domain ID's from GI with eValue
* Calling function must free list unless NULL value returned. */
list<string>* GetDomainIDsFromGiWithEvalue(int gi, double cutoff);

/* Get a list of Gis from DomainId with or without evalue cutoff
* Returns the unique list og GI's. Certain domains appear more than once
* in a protein, we report the unique list.
* Calling function must free list unless NULL value returned.*/
list<int>* GetGisFromDomainId(string domid);
list<int>* GetGisFromDomainId(string domid, double cutoff);

/*Get a list of gis that have x number of domains, with or without evalue cutoff
* Calling function must free list unless NULL value returned. */
list<int>* GetGisByNumberOfDomains(int x);
list<int>* GetGisByNumberOfDomains(int x, double cutoff);

/*************************
* DOMNAME searches
**************************/
bool InitDOMNAME(string table);
bool FiniDOMNAME(void);
bool DOMNAMEAreYouListening(void);

string GetDomainLabelFromDomainId(string s);
list<string>* GetDomainIdFromName(string name);

/*Get 3D PDB-ID for domain from domain ID*/
string Get3DStructureFromDomainId(string s);

/*Get a list of all domain ID currently in table
* User must free the list */
list<string>* GetAllDomainIDs(void);

/**************************
* Relational Calls
***************************/
list<int>* GetGIsByDomainLabel(string label, double evalue=10.0);

/*Close both RPSDB and DOMNAME tables plus Finialize CodeBase*/
void CloseTables(void);


#endif
/*
$Log: rpsdbapi.hpp,v $
Revision 1.1.1.13  2002/11/15 09:34:14  root
Automated import from Sourceforge.net

Revision 1.13  2002/11/14 19:51:44  betel
Fixed bugs in searches and new function

Revision 1.12  2002/11/01 19:46:20  betel
Small changes

Revision 1.11  2002/10/25 00:59:50  betel
Added 3D function and changes to DOMNAME table

Revision 1.10  2002/07/08 16:56:02  kaca
renamed two RPS-BLAST functions Accession is DomainId

Revision 1.9  2002/05/09 18:53:48  betel
Moved utility template functions to slri_misc.hpp

Revision 1.8  2002/05/09 00:07:18  betel
Fixed PrintMap

Revision 1.7  2002/05/06 20:45:32  betel
Added more function annotation

Revision 1.6  2002/05/02 21:29:35  betel
Added a new function and correct memory clean up

Revision 1.5  2002/04/26 23:49:09  betel
Added PrintVector template function

Revision 1.4  2002/04/19 21:41:48  betel
More Print functions and small changes

Revision 1.3  2002/04/15 20:01:59  betel
Added api functions

Revision 1.2  2002/04/02 00:39:09  betel
Added more API functions

Revision 1.1  2002/03/21 02:51:10  betel
RPSDB C++ API layer

*/
