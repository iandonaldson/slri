/*$Id:*/
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


  PROGRAM: Rps Table (domains) DB2 layer


  AUTHORS:  Marc Dumontier (mdumontier@mdsp.com)


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

#include <ncbi.h>
#include <intrez_misc.h>
#include <slri_math.h>
#include <rpsdb_db.h>

/*Append to RPS Database*/
Boolean LIBCALL AppendRecordRPSDB(StRpsdbPtr record)
{
	return TRUE;
}

/* Append to DOMNAME table */
Boolean LIBCALL AppendRecordDOMNAME(StDomNamedbPtr dnp)
{
	return TRUE;
}

/*Delete record from RPS table*/
Boolean LIBCALL DeleteGIinRPSDB(Int4 gi, Boolean destroy)
{
	return TRUE;
}

/*Delete record from DomName table*/
Boolean LIBCALL DeleteAccinDOMNAME(CharPtr accession, Boolean destroy)
{
	return TRUE;
}

/*Is exist calls*/
Boolean LIBCALL IsExistInRPSDB(Int4 gi)
{
	return FALSE;
}
Boolean LIBCALL IsExistInDOMNAME(CharPtr acc)
{
	return FALSE;
}
/*Packing and memory compression*/
void LIBCALL PackRPSDB(void)
{
	return;
}

void LIBCALL PackDOMNAME(void)
{
	return;
}
/****************************
* Search Functions
*****************************/

/*Return record from RPSDb
* searches can be done using any of the
* tags defined for this table. Returns all
* matching records. Note that Evalue must be set to some value*/
Int2 LIBCALL SearchRPSDB(StRpsdbPtr PNTR pprs)
{
	return -1;
}

/* This function is currently not in use!!
* Query times are too long to be practical.
*-----------------------------------------
* Search RPSDB using a combination of any
* two tags from RPSDB and an Evalue (gi, dom_id, len, numdom)
* Can search any combination of two tags with evalue or any single tag with evalue.
* Precondition- No more than two tags can be set. Evalue must be set even if just default value
* Note that evalues should be in log form.
*/
Int2 SearchRPSDBByTagsAndEvalue(StRpsdbPtr PNTR pprs)
{
	return -1;
}

/* domain name, or mmdb-id */
Int2 LIBCALL SearchDOMNAME(StDomNamedbPtr PNTR ppdn, Boolean noasn)
{
	return -1;
}

/******************
* Reset functions
******************/

/*Reset RPSDB record */
void LIBCALL ResetStRPSDB (StRpsdbPtr PNTR pprs)
{
	/* SK: NULL ptr check */
	if(pprs == NULL || *pprs == NULL)
	{
		ErrPostEx(SEV_ERROR, 0,0,"ResetStRPSDB: structure is null");
		return;
	}
	(*pprs)->gi=0;
	(*pprs)->dom_id=NULL;
	(*pprs)->from=0;
	(*pprs)->evalue=0;
	(*pprs)->n_missing=0;
	(*pprs)->c_missing=0;
	(*pprs)->len=0;
	(*pprs)->num_of_dom=0;
	(*pprs)->next=NULL;
	return;
}

/*Reset DOMNAME record*/
void LIBCALL ResetStDOMNAME (StDomNamedbPtr PNTR ppdn)
{
	return;
}

/*************************************
* Codebase open and close calls
**************************************/
/*Boolean LIBCALL InitCodeBase(void)
{
	return TRUE;
}

Boolean LIBCALL CloseCodeBase(void)
{
	return TRUE;
}*/

/*Close RPS database*/
Boolean LIBCALL CloseRPSDB(void)
{
	return TRUE;
}

/*Close DOMNAME database*/
Boolean LIBCALL CloseDOMNAME(void)
{
	return TRUE;
}

/*Open RPS database*/
Boolean LIBCALL OpenRPSDB( CharPtr name, Boolean create)
{
	return TRUE;
}

/*Open domain name database*/
Boolean LIBCALL OpenDOMNAME(CharPtr name, Boolean create)
{
	return TRUE;
}


/***********************
*Print Functions
***********************/

/*Print RPSDB */
Boolean LIBCALL PrintRPSDB(void)
{
	return FALSE;
}
/*Print Domain Name table*/
Boolean LIBCALL PrintDOMNAME(void)
{
	return TRUE;
}

/***********************
* Utilities functions
***********************/

/* Utility function to read Cdd asn1*/
CddPtr LIBCALL GetCddFROMRDOMNAME(void)
{
	return NULL;
}
/*
$Log
*/
