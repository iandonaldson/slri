/*$Id: rpsdb_cb_p.hpp,v 1.1.1.6 2002/12/05 09:35:24 root Exp $*/


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


  PROGRAM: RPSDB CodeBase variables and structures


  AUTHORS:  Doron Betel betel@mshri.on.ca
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
#ifndef _RPSDB_CB_P_HPP
#define _RPSDB_CB_P_HPP

#include <d4all.hpp>
Code4 codeBase;
Data4 rpsdb;
Data4 domnamedb;
Field4 /*RPSdb*/
	pf4rpsgi,
	pf4rpscddid,
	pf4rpsdomid,
	pf4rpsfrom,
	pf4rpsalignlen,
	pf4rpsscore,
	pf4rpsevalue,
	pf4rpsbitscore,
	pf4rpsmisN,
	pf4rpsmisC,
	pf4rpsnumdom,
	/*DomnameDb */
	pf4dnaccession,
        pf4dnname,
        pf4dnpdbid;
Field4memo   pf4dnasn1;
Tag4
	/*RPSdb tags*/
	tagrpsgi,
	tagrpscddid,
	tagrpsdomid,
	tagrpslen,
	tagrpsnumdom,
	/* Domname */
	tagdnaccession,
        tagdnname,
        tagdnpdbid;

/*RPS-gi database */

static FIELD4INFO f4rpsdb[ ]=
{
	/*name, type, len, dec*/
	{"GI",r4num, 10, 0 },
	{"CDDID",r4num,10,0 },
	{"DOMID",r4str, 12, 0 },
	{"FROM", r4num, 6, 0 },
	{"ALIGN_LEN", r4num, 6, 0},
	{"SCORE", r4num, 10, 0 },
	{"EVALUE", r4float, 15, 8},
	{"BITSCORE", r4float, 15, 8},
	{"MISSING_N", r4num, 6, 0},
	{"MISSING_C", r4num, 6, 0},
	{"NUMDOM", r4num, 4, 0},
	{0, 0, 0, 0}
};

static TAG4INFO t4rpsdb[ ]=
{
	/*name, expression, filter, unique, descending*/
	{"GI", "GI", 0, 0, 0},
	{"CDDID", "CDDID", 0, 0},
	{"DOMID", "DOMID", 0, 0},
	{"ALIGN_LEN", "ALIGN_LEN", 0, 0},
	{"NUMDOM", "NUMDOM", 0 ,0},
	{0, 0, 0, 0, 0}
};

/*  Domain Name */
static FIELD4INFO f4domname[ ]=
{
	/* name, type, len, dec*/
	{"ACCESSION", r4str, 15,0},
	{"NAME", r4str, 25,0},
	{"PDBID", r4str, 10, 0},
	{ "ASN1", 'M', 10, 0 },
	{0, 0, 0, 0}
};

static TAG4INFO t4domname[ ]=
{
	/* name, expression, filter, unique, descending*/
	{"ACCESSION", "ACCESSION", 0 ,0 ,0},
	{"NAME", "NAME", 0, 0, 0},
	{"PDBID", "MMDBID", 0,0,0}
};

Data4 dataFile;

#endif
/*
$Log: rpsdb_cb_p.hpp,v $
Revision 1.1.1.6  2002/12/05 09:35:24  root
Automated import from Sourceforge.net

Revision 1.6  2002/12/04 18:55:55  betel
Added new fields

Revision 1.5  2002/11/04 19:09:54  betel
Changed field size

Revision 1.4  2002/10/28 23:41:01  betel
Removed #ifdef NEW_RPSDB, new tables fields are in effect

Revision 1.3  2002/10/25 00:59:49  betel
Added 3D function and changes to DOMNAME table

Revision 1.2  2002/10/22 23:06:38  betel
Added ifdef for compatibility with old RPSDB tables

Revision 1.1  2002/10/21 21:29:00  betel
New rpsdb codebase variables and header

*/
