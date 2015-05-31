/********************************************************************************
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
           Katerina Michalickova and Christopher W.V. Hogue hogue@mshri.on.ca
	   Renan A. CAVERO


  REVISION/CONTRIBUTION NOTES:

  June 1, 2000  Original MSH Revision.
                Hogue Lab - University of Toronto Biochemistry
                Department and the Samuel Lunenfeld Research
                Institute, Mount Sinai Hospital
                http://bioinfo.mshri.on.ca  hogue@mshri.on.ca
                ftp://ftp.mshri.on.ca/


  PUBLICATION TO CITE:


  DESCRIPTION
  April 2005
  SeqHound GOA Module
 

*******************************************************************************/

#ifndef __GOA_ODBC_H__
#define __GOA_ODBC_H__

/* include standard C libraries */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


/* include the sql library */
#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>


#include <intrez_odbc.h>

/* include the seqhound library */
#include <slri_misc.h>

/* include ODBC library */
#include <objslristruc.h>
#include <slri_odbc.h>


#ifdef __cplusplus
extern "C" { /* } */
{
#endif

/* ****************************************************** */

/* handles for each of the databases. */

static SQLHANDLE hstmtGoaGOIDFromGi;
static SQLHANDLE hstmtGoaGOIDFromRedundantGi;
static SQLHANDLE hstmtGoaGiFromGOID;
static SQLHANDLE hstmtGoaGiFromGOIDAndECode;
static SQLHANDLE hstmtGoaGOECodeFromGiAndGOID;
static SQLHANDLE hstmtGoaGOIDFromGENEID;

static SQLHANDLE hstmtPMIDFromGiAndGOID;


/* Variables to bind goa tables */

SQLINTEGER  goa_gi;
SQLINTEGER  goa_go;

Char goa_ecode[4];     /* size of goa_referce.code */
Char goa_gene[30];     /* size of goa_seq_dbxref.xref_key */

/****************************************************************
                       GOA FUNCTION
*****************************************************************/
Boolean InitGOA(void);
extern Int4 GOA_SearchGOIDFromGi(Int4 gi, ValNodePtr PNTR vnp);
extern Int4 GOA_SearchGOIDFromGiList(ValNodePtr giList, ValNodePtr PNTR vnp);
extern Int4 GOA_SearchGOIDFromRedundantGi(Int4 gi, ValNodePtr PNTR vnp);
extern Int4 GOA_SearchGOIDFromRedundantGiList(ValNodePtr giList, ValNodePtr PNTR vnp);

extern Int4 GOA_SearchGiFromGOID(Int4 go, ValNodePtr PNTR vnp);
extern Int4 GOA_SearchGiFromGOIDAndECode(Int4 go, CharPtr ECode, ValNodePtr PNTR vnp);
extern Int4 GOA_SearchGiFromGOIDListAndECode(ValNodePtr goList, CharPtr ECode, ValNodePtr PNTR vnp);
extern Int4 GOA_SearchGOECodeFromGiAndGOID(Int4 gi, Int4 go, ValNodePtr PNTR vnp);
extern Int4 GOA_SearchGOIDFromGENEID(Int4 gene, ValNodePtr PNTR vnp);

extern Int4 GOA_SearchPMIDFromGiAndGOID(Int4 gi, Int4 go, ValNodePtr PNTR vnp);

Boolean CloseGOA(void);



#ifdef __cplusplus
}
#endif
#endif  /* __GOA_ODBC_H__ */
