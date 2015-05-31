/* $Id: nblast_odbc.h,v 1.4 2004/07/20 15:19:38 rcavero Exp $
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
           Michel Dumontier (micheld@mshri.on.ca); 
	   Christopher W.V. Hogue hogue@mshri.on.ca
	   Renan A. CAVERO rcavero@blueprint.org


  REVISION/CONTRIBUTION NOTES:

  June 1, 2000  Original MSH Revision.
                Hogue Lab - University of Toronto Biochemistry
                Department and the Samuel Lunenfeld Research
                Institute, Mount Sinai Hospital
                http://bioinfo.mshri.on.ca  hogue@mshri.on.ca
                ftp://ftp.mshri.on.ca/



  PUBLICATION TO CITE:
	M Dumontier, CWV Hogue. NBLAST: a cluster variant of BLAST for NxN comparisons. 
	BMC Bioinformatics, 2002, 3: 13.



  DESCRIPTION
  January 2004
  SeqHound NBlast Module.
 

*******************************************************************************/


#ifndef __NBLAST_ODBC_H__
#define __NBLAST_ODBC_H__

/* include standard C libraries */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <ncbi.h>
#include <asn.h>
/* include the sql library */
#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>


/* #include <intrez_odbc.h> */

/* include the seqhound library */
#include <slri_misc.h>
#include <objNBlastasn.h>            /* Needed by NBLAST */

/* include ODBC library */
#include <objslristruc.h>
#include <slri_odbc.h>


#ifdef __cplusplus
extern "C" { /* } */
{
#endif
#define SH_ERROR 0
#define SH_NODATA -1
#define SH_SUCCESS 1

/* ************************************************************ */

 /* SeqHound defined structures: Used for ODBC functions that
    required binding to the parameters or columns of the
    result set. See SQLBindParameter & SQLBindCol
  */



/*   This Structures are defined under intrez_odbc.h but needs to be defined under slri_odbc.h. */
#define SH_SUCCESS 1
#define SH_ERROR 0
#define SH_NODATA -1


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

typedef struct mySH_SQLFParam {
   FloatHi val;
} mySH_FloatParam;

/* ASNDB fields in SQLBindParameter*/
typedef struct mySH_SQLASNParam
{
   SQLINTEGER len;
   CharPtr val;
} mySH_AsnParam;


/*     SH_SQLIParam SH_IntParam; */



/* ******************************************************************* */


/* ODBC variables used in this file */
/*  This are defined in intrez_odbc.*/
/*  static SQLHENV    henv;   */              /* Handle ODBC environment */
/*  static SQLHDBC    hdbc;   */              /* Handle ODBC connection  */
/*  static SQLHANDLE  hstmt;  */              /* Handle ODBC statement   */


/* ******************************************************************* */

 /* Global variables used in the SQL statements to bind
    to the SQL statement handles and return values.
    SH_IntParam & SHCharParam defined above.
  */ 

/* BlastDB variables */
    static mySH_IntParam   blastdb_uid;
    static mySH_FloatParam blastdb_eval;
    static mySH_AsnParam   blastdb_asnsa;

/* NBlastDB variables */
    static mySH_IntParam nblastdb_id;
    static mySH_IntParam nblastdb_ord;
    static mySH_IntParam nblastdb_gi;
    static mySH_IntParam nblastdb_num;
    static mySH_AsnParam nblastdb_asnbr;

 static mySH_CharParam dbxref_sourcedb;

/* PARTI variables */

/*  static SH_CharParam parti_div; */


/* ****************************************************** */


/* handles for each of the databases. */

    static SQLHANDLE HNDLBlastdb, HNDLNBlastdb, HNDLNBlastdbUpdate;
	
/*    HNDLParti,   */
/*    HNDLPubseq,  */
/*    HNDLNucprot, */
/*    HNDLAccdb;   */


/* ***************************************************S**** */

/* NBLAST DEFINITIONS **************************************/

/* enumerated defines for iDB argument passing */
#define BLASTDB  1
#define BLASTDBt 2
#define NBLASTDB 3

Int2 NJOB;
#define NBUILDDB         1
#define NBLAST           2
#define NCONTINUE        3
#define NBUILDTABLE      4
#define NUPDATE          5
#define NBLASTFROMUPDATE 6
#define NBLASTNEIGHBOURS 7




/* END NBLAST DEFINITIONS **********************************/








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
Int2 LIBCALL SearchBLASTDB ();

/*******************************************************************************
* Main Database Functions                                                      *
********************************************************************************
* NBlast_IsDBOpen                      Check to see if database is open        *
* NBlastOpenDBs                        Initialize codebase and open either or  *
*                                       both databases                         *
* OpenBlastDB                          Open BlastDB and set access modes       *
* OpenNBlastDB                         Open NBlastDB and set access modes      *
* NBlastClose                          Close all dbs and codebase              *
*******************************************************************************/
Boolean NBlast_IsDBOpen(Int2 iDB);
SLRI_ERR NBlastOpenDBs(CharPtr dbname, Boolean BlastDB, Boolean NBlastDB);
SLRI_ERR OpenNBlastDB(CharPtr pcNBlastDB, Boolean bCreate, Boolean bDENY_NONE);
SLRI_ERR OpenBlastDB(CharPtr pcBlastDB, Int4 imachine_num, Boolean bCreate, Boolean bTemp, Boolean bDENY_NONE);
void NBlastClose(void);

/*******************************************************************************
* Support Database Functions                                                   *
********************************************************************************
* NBlastDBInit                         Initialize codebase	                   *
* NBlastDBClose                        Close codebase structure                *
* NBlastRecCount                       Count the number of records in the dbs  *
*                                                                              *
* OpenNBlast_BlastDB                   Open Blast Database	                   *
* OpenNBlast_NBlastDB                  Open NBlast Database                    *
* CheckDatabases                       Validates integrity of BlastDBs         *
* NBlastFlushDBs                       Flush all the databases before closing  *
* NBlastCloseDBs                       Close any of the dbs                    *
*******************************************************************************/
void NBlastBuildTableSetDataBaseVariables(void); /*Set database variables for table build*/
void NBlastDataBaseSetReadLock(Boolean bLock);   /*Set database read lock*/
void NBlastDataBaseLock(Int2 idb);               /*Lock the databases*/
void NBlastDataBaseUnlock(Int2 idb);             /*Unlock the databases*/
SLRI_ERR NBlastPack(Int2 iDB);                   /*Remove records, pack and compress memo*/
void NBlastTagSelect(Boolean bORD, Boolean bGI, Boolean bNUM, Boolean bUID); /*Select the index tag*/

SLRI_ERR NBlastDBInit(void);
void NBlastDBClose(void);
Boolean InitBlastDB(void);
Boolean InitNBlastDB(void);

void NBlastRecCount(Int2 iDB, Int4 PNTR irecCount);
Int4 LIBCALL CountRecordsFromTable(CharPtr tblname);

SLRI_ERR OpenNBlast_BlastDB(CharPtr pcBlastDB, Int4 imachine_num, Boolean bCreate, Boolean bTemp);
SLRI_ERR OpenNBlast_NBlastDB(CharPtr pcNBlastDB, Boolean bCreate);

void NBlastFlushDBs(void);
    Int2 FlushTable(CharPtr tblname);
void NBlastCloseDBs(Boolean bBlastDB, Boolean bBlastDBt, Boolean bNBlastDB);

/*******************************************************************************
* Miscellaneous Database Functions                                             *
********************************************************************************
* AppendRecordtoBlastDB                Adds a new record to the BlastDB        *
* AppendRecordtoNBlastDB               Adds a new record to the NBlastDB       *
* AddRecordtoNBlastDB                  Adds a record to the NBlastDB, assigning*
*                                       the next unique ordinal number         *
* NBlastRemoveBlastDBRecord            Tag a uid for deletion                  *
* NBlastRemoveNBlastDBRecord           Tag a Gi for deletion                   *
* NBlastModifyNBlastDBRecord           Modify any part of an nblast record, use*
*                                       -1 for RecordNum to call record by GI  *
*                                       0  for RecordNum to call record by ORD *
* NBlastAssignASNMemMemo               External call to write to ASN memofield *
*                                       on record in buffer                    *
* NBlastReplaceAllNeighboursWithNewGi  Replaces the Old Gi with the New GI in  *
*                                       all neighbours                         *
* NBlastRemoveNeighbours               Removes Gi1 in Gi2's list               *
* AddNumNeighboursToDB                 Walks through the NBlastDB and counts   *
*                                       the number of neighbours in each record*
* PrintNBlastDB                        Print the contents of the NBlastDB      *
* PrintBlastDB                         Print the contents of the BlastDB       *
*******************************************************************************/
SLRI_ERR AppendRecordtoBlastDB(Int8 uid, FloatHi evalue, NBlastResultSetPtr pResult, Boolean bUnique);
SLRI_ERR AppendRecordtoNBlastDB(Int4 ord, Int4 gi, Int4 num, NBlastGiAndEvalSetPtr pNeighbours, Boolean bUnique);
SLRI_ERR AddRecordtoNBlastDB(Int4 gi, Int4 num, NBlastGiAndEvalSetPtr pNeighbours);
SLRI_ERR NBlastDeleteBlastDBRecord(Int8 uid);
SLRI_ERR NBlastRemoveBlastDBRecord(Int4 gi1, Int4 gi2);
SLRI_ERR NBlastRemoveNBlastDBRecord(Int4 gi);
SLRI_ERR NBlastClearNBlastDBRecord(Int4 gi);

SLRI_ERR AddNumNeighboursToDB(Int4 PNTR piTotalNeighbours);

/* For ModifyNBlastDBRecord as RecordNum */
#define CLEAR_NBLASTDB_RECORD (-20)
SLRI_ERR NBlastModifyNBlastDBRecord(Int4 RecordNum, Int4 ord, Int4 gi, Int4 num, NBlastGiAndEvalSetPtr pNeighbours);


/************************************************************************
* NBlast Functions                                                      *
*************************************************************************
* SaveBlastInfoInBlastDB        Save new blast struc in BlastDB	        *
* AddNeighbourToRecord          Adds a neighbour to a gi specifed record*
* SaveNeighbourSetInNBlastDB    Save a nbr struc in NBlastDB            *
* WriteNBlastResultstoDB        Higher level function to create nbr     *
*                                struc and save to database             *
* MakeORDtoGIarray              Makes an ORD to GI array in memory from *
*                                NBlastDB                               *
* NBlastWriteTabletoDB          Writes an array of neighbour lists to db*
* NBlastAssembleBlastDBs        Combines the BLASTDBs into a single db  *
************************************************************************/
SLRI_ERR SaveBlastInfoInBlastDB(NBlastResultSetPtr nbrsp, FloatHi best_evalue);
/* SLRI_ERR AddNeighbourToRecord(Int4 gi1, Int4 gi2, FloatHi evalue); */
/* SLRI_ERR SaveNeighbourSetInNBlastDB(NBlastGiAndEvalSetPtr nbgesp); */
/* SLRI_ERR WriteNBlastResultstoDB(Int4 query_gi, Int4 subject_gi, FloatHi evalue); */
SLRI_ERR MakeORDtoGIarray(Int4Ptr PNTR ppaORDtoGI, Int4 PNTR pnumrec);
/* SLRI_ERR NBlastWriteTabletoDB(NBlastGiAndEvalSetPtr PNTR *ppaGE, Int4 numrec, Int4 dbnum, Int4 dbcount); */
/* SLRI_ERR NBlastAssembleBlastDBs(CharPtr pcBlastDB, Int4 machines); */


/***************************************************************************
* Database Retrieval Functions                                             *
****************************************************************************
* GetBlastStrucFromDB              Get Blast Result set                    *
* GetNBlastStrucFromDB             Get Neighbour set                       *
* NBlastGetRecordFromBlastDB       Get a full record from the BlastDB      *
* NBlastGetRecordFromNBlastDB      Get a full record from the NBlastDB     *
*                                    use -1 for RecordNum to get rec by GI *
*                                    use 0 for RecordNum to get rec by ORD *
* GetNumberofNeighboursListFromDB  Get the number of neighbours in a       *
*                                   separate gi and number array           *
****************************************************************************/
Int2 LIBCALL SearchORDbyGIinNBLASTDB(Int4 gi, Int4 PNTR pORD); 
Int2 LIBCALL SearchForOrdInBLASTDBbyRecordNumber(Int4 recNum, Int4 PNTR pORD);

/*  But this is not defined in nblast_cb.h. */
Int4 LIBCALL NBLASTFindGIsAndGetUID(Int4 gi1, Int4 gi2, Int8 PNTR pUID);
SLRI_ERR NBlastGetRecordFromNBlastDB(Int4 RecordNum, Int4 PNTR pord, Int4 PNTR pgi, Int4 PNTR pnum, NBlastGiAndEvalSetPtr PNTR pNeighbours);

static SLRI_ERR BlastDBRecordCopy(void);


#ifdef __cplusplus
/* { */ }
#endif

#endif  /* __NBLAST_ODBC_H__ */
