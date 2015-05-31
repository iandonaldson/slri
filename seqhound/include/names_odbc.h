/* $Id: names_odbc.h,v 1.4 2005/02/01 16:41:07 eburgess Exp $
********************************************************************************
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
  January 2004
  SeqHound Names Module
 

*******************************************************************************/
#ifndef __NAMES_ODBC_H__
#define __NAMES_ODBC_H__

/* include standard C libraries 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>*/

#include <intrez_odbc.h>

/* include the seqhound library */

#ifdef __cplusplus
extern "C" { 
#endif

/* ************************************************************ */

 /* SeqHound defined structures: Used for ODBC functions that
    require binding to the parameters or columns of the
    result set. See SQLBindParameter & SQLBindCol
  */


/* ************************************************************ */
/* ODBC variables used in this file 
 *  These are defined in intrez_odbc.
 *
 *  static SQLHENV    henv;                  Handle ODBC environment 
 *  static SQLHDBC    hdbc;                  Handle ODBC connection  
 *  static SQLHANDLE  hstmt;                 Handle ODBC statement   
 *
 ******************************************************************* */
 static SH_IntParam  bioentity_id;
 static SH_IntParam  bioentitytype_id;
 static SH_CharParam bioentity_db;
 static SH_CharParam bioentity_access;
 static SH_IntParam  bioentity_identifier;
 static SH_IntParam bioentity_field;

 static SQLHANDLE HNDLBioentity;
 static SQLHANDLE HNDLBioentSearchGi;
 static SQLHANDLE HNDLBioentDeleteGi;
 static SQLHANDLE HNDLBioentMaxId;
 static SQLHANDLE HNDLBioentSearchAccess;
 static SQLHANDLE HNDLBioentUpdateId;

 static SH_IntParam  bioname_id;
 static SH_IntParam  bioname_bioentityid;
 static SH_IntParam  bioname_nametypeid;
 static SH_CharParam bioname_db;
 static SH_CharParam bioname_access;
 static SH_IntParam  bioname_identifier;
 static SH_IntParam  bioname_field;
 static SH_IntParam  bioname_official;
 static SH_IntParam  bioname_deprecated;
 static SH_CharParam bioname_datedep;
 static SH_IntParam  bioname_ruleid;
 static SH_IntParam  bioname_action;
 static SH_CharParam bioname_actiondate;

 static struct
 {
   SQLCHAR    val[1024];
   SQLINTEGER len;
 } bioname_name;

 static SQLHANDLE HNDLBioname;
 static SQLHANDLE HNDLBionameDeleteGi;
 static SQLHANDLE HNDLBionameSearchBioentityId;
 static SQLHANDLE HNDLBionameEditId;

/******************************************************************
                      PRIVATE FUNCTIONS
  THESE FUNCTIONS BELOW ARE NOT INTENDED TO BE CALLED BY END USERS
  *******************************************************************/
 
 Boolean LIBCALL InitNAMES();
 Boolean LIBCALL CloseNAMES();

 Boolean LIBCALL InitBioentity();
 Boolean LIBCALL AppendRecordBioentity( StBioentityPtr pRecord );
 Boolean LIBCALL SearchBioentityByGi( StBioentityPtr PNTR pRecord );
 Boolean LIBCALL SearchBioentityByAccess( StBioentityPtr PNTR pRecord );				      
 Boolean LIBCALL EditBioentityById( StBioentityPtr pRecord );
 Boolean LIBCALL DeleteGIinBioentity(SQLINTEGER gi);
 Int4    LIBCALL GetLargestBioentityId();

 Boolean LIBCALL InitBioname();
 Boolean LIBCALL AppendRecordBioname( StBionamePtr pRecord );
 Boolean LIBCALL SearchBionameByBioentityId ( StBionamePtr PNTR ppREcord );
 Boolean LIBCALL EditBionameById( StBionamePtr pRecord );
 Boolean LIBCALL DeleteGIinBioname(SQLINTEGER gi);

#ifdef __cplusplus
 }
#endif

#endif /* if defined __NAMES_ODBC_H__        */




