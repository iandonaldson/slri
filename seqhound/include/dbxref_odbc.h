/* $Id: dbxref_odbc.h,v 1.6 2004/07/20 15:13:44 rcavero Exp $
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
  September 2003
  SeqHound DBXref Module
 

*******************************************************************************/
#ifndef __DBXREF_ODBC_H__
#define __DBXREF_ODBC_H__

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


/* ************************************************************ */

 /* SeqHound defined structures: Used for ODBC functions that
    required binding to the parameters or columns of the
    result set. See SQLBindParameter & SQLBindCol
  */



    /*   This Structures are defined under intrez_odbc.h but needs to be defined under slri_odbc.h. */



/* #ifndef SH_IntParam */
typedef struct mySH_SQLIParam {
   SQLINTEGER val;
} mySH_IntParam;
/* #endif */


/* #ifndef SH_CharParam */
typedef struct mySH_SQLCParam {
   SQLINTEGER len;
   SQLCHAR val[50];
} mySH_CharParam;
/* #endif */


/*     SH_SQLIParam SH_IntParam; */



/* ******************************************************************* */


/* ODBC variables used in this file */
/*  This are defined in intrez_odbc. */
/* static SQLHENV    henv;  */               /* Handle ODBC environment */
/* static SQLHDBC    hdbc;  */               /* Handle ODBC connection  */
/* static SQLHANDLE  hstmt; */               /* Handle ODBC statement   */


/* ******************************************************************* */

 /* Global variables used in the SQL statements to bind
    to the SQL statement handles and return values.
    SH_IntParam & SHCharParam defined above.
  */ 

/* dbXref variables */
 static mySH_IntParam  dbxref_id;
 static mySH_IntParam  dbxref_sourceid;
 static mySH_CharParam dbxref_sourcedb;
 static mySH_CharParam dbxref_recordid;
 static mySH_IntParam  dbxref_parentid;
 static mySH_IntParam  dbxref_link;
 static mySH_CharParam dbxref_field;
 static mySH_IntParam  dbxref_cv;

 static mySH_CharParam dbxef_recParam;
/* PARTI variables               */
/*  static SH_IntParam parti_gi; */
/* static SH_CharParam parti_div;*/


/* ****************************************************** */


/* handles for each of the databases. */

/*static SQLHANDLE HNDLAsndb,  */
/*          HNDLParti,         */
/*          HNDLPubseq,        */
/*          HNDLNucprot,       */
/*    HNDLAccdb;               */


/* ******************************************************* */


 /******************************************************************
                      PRIVATE FUNCTIONS
   THESE FUNCTIONS BELOW ARE NOT INTENDED TO BE CALLED BY END USER
   SEE dbxref_query.c/ dbxref_query.c FOR THEIR PUBLIC COUNTERPARTS
 *******************************************************************/
 /* Function prototypes */

/**************************************************************/
/*Given a pointer to the asn structure that represents        */
/*  a dbxref record, this function search the DBXREF database */
/*  for the record specified by ppdbxref.                     */
/*  The search key is in ppdbxref and must be set by the user */
/*  prior to calling SearchDBXREF.                            */
/*  Returns all records associated with that key in a link    */
/*  list pointed by pdbxref.                                  */
/*  Arg: StDbXrefPtr a pointer to dbxref structure.           */
/*       Combining elementes of StDbXref                      */
/*       When Source_db != NULL and parentid = -1             */
/*            will retrieve all Source Record for that        */
/*            Sourcedb       .                                */
/*       When Source_db != NULL and parentid = -2             */
/*            will retrieve all Record_id for that Sourcedb   */
/*  Returns : TRUE if successful and FALSE otherwise.         */
/**************************************************************/
Int2 LIBCALL SearchDBXREF (StDbXrefPtr PNTR ppdbxref);


/********************************************************/
/* Given a pointer to the asn structure that represents */
/*  an dbxref record and a Database name, Record Id,    */
/*  field and a Control Vocabulary.                     */
/*  this function retrieves all possible Source_Records */
/*  for that Record ID                                  */
/*                                                      */
/*  Parameters:                                         */
/*      ppdbxref: Pointer to DBXref structure.          */
/*      dbname:   Database name of the SourceRecord.    */
/*      recordid: SourceRecord.                         */
/*      field:    Field description, Optional  NULL     */
/*      cv:       Control vocabulary, Optional -1       */
/*                                                      */
/*  Returns: TRUE if successful and FALSE otherwise.    */
/*           DBXref LinkList with all SourceRecords.    */
/********************************************************/
Int2 LIBCALL GetSourceRecordListByDBXrefInDBXREF (StDbXrefPtr PNTR ppdbxref, CharPtr dbname, CharPtr recordid, CharPtr field, Int4 cv);


/********************************************************/
/* Given a flag that represent Source Record or         */
/*  Record ID retrive al list of unique Database Names  */
/*  from DBXref.                                        */
/*                                                      */
/*  Parameters:                                         */
/*      recordType: int flag 1 or 0          .          */
/*      where 0 = Source Record.                        */
/*            1 = Record ID.                            */
/*            2 = All Database Names.                   */
/*                                                      */
/*  Returns: TRUE if successful and FALSE otherwise.    */
/*           DBXref LinkList with all SourceRecords.    */
/********************************************************/
Int2 LIBCALL GetDBNameListInDBXREF (ValNodePtr PNTR vnp, Int4 recordType);


/********************************************************/
/* Given a pointer to the asn structure that represents */
/*  an dbxref record and a Database name, Source Record,*/
/*  field and a Control Vocabulary.                     */
/*  this function pulls out the Source Record and       */
/*  retrieves all dbXref records associated with that   */
/*  Source Record.                                      */
/*                                                      */
/*  Parameters:                                         */
/*      ppdbxref: Pointer to DBXref structure.          */
/*      dbname:   Database name of the SourceRecord.    */
/*      recordid: SourceRecord.                         */
/*      field:    Field description, Optional  NULL     */
/*      cv:       Control vocabulary, Optional -1       */
/*                                                      */
/*  Returns: TRUE if successful and FALSE otherwise.    */
/*           DBXref LinkList with all record_id         */
/********************************************************/
Int2 LIBCALL GetDBXrefListBySourceRecordInDBXREF (StDbXrefPtr PNTR ppdbxref, CharPtr dbname, CharPtr recordid, CharPtr field, Int4 cv);


/********************************************************/
/* Given a flag that represent Source Record or         */
/*  Record ID retrive al list of unique Database Names  */
/*  from DBXref.                                        */
/*                                                      */
/*  Parameters:                                         */
/*      recordType: int flag 1 or 0          .          */
/*      where 0 = Source Record.                        */
/*            1 = Record ID.                            */
/*            2 = All Database Names.                   */ 
/*                                                      */
/*  Returns: TRUE if successful and FALSE otherwise.    */
/*           DBXref LinkList with all SourceRecords.    */
/********************************************************/
Int2 LIBCALL GetDBNamesListInDBXREF(ValNodePtr PNTR vnp, Int4 recordType);

/********************************************************/
/* Return a list of ValNodePtr containing CV Control    */
/*  Vocabulary.                                         */
/*                                                      */
/*  Parameters:                                         */
/*        vnp ValNodePtr                                */
/*  CV:   0 Not CV defined                              */
/*        1 Nucleotide                                  */
/*        2 Protein                                     */
/*        3 Domain                                      */
/*      100 Swissprot                                   */
/*      101 Trembl                                      */
/*                                                      */
/*  Returns: NULL if no CV were found.                  */
/*           ValNode LinkList with all CV.              */
/********************************************************/
Int2 LIBCALL GetCVListInDBXrefSourcedb (ValNodePtr PNTR vnp);


/********************************************************/
/* Return a list of ValNodePtr containing Field names.  */
/*                                                      */
/*  Parameters:                                         */
/*        vnp ValNodePtr                                */
/*                                                      */
/* Field: is a tag name from a 3rd party record where   */
/*        the information was extracted.                */
/*                                                      */
/*  Returns: NULL if no Field were found.               */
/*           ValNode LinkList with all Field Names.     */
/********************************************************/
Int2 LIBCALL GetXrefSourceFiledName (ValNodePtr PNTR vnp);


/********************************************************/
/* For test Only not part of the specification.         */
/********************************************************/
Int2 LIBCALL GetObjectIDbyAccInDBXREF (StDbXrefPtr PNTR ppdbxref, CharPtr acc);


Boolean CloseDB(void);


/********************************************************/
/* Auxiliary function to                                */
/* Bind Columns for fetch under UnixODBC.     .         */
/********************************************************/
Boolean bindDBXrefParameters(SQLHANDLE *hstmt2bind, CharPtr function, Boolean flagSourceDBStr);

#ifdef __cplusplus
/* { */ }
#endif

#endif  /* __DBXREF_ODBC_H__ */
