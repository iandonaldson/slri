/* $Id: ll_odbc.h,v 1.12 2005/05/02 14:28:57 rcavero Exp $
**************************************************************************
>>>>>>> 1.3
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
 
  REVISION/CONTRIBUTION NOTES:

  March 1, 2001  Original MSH Revision.
                Hogue Lab - University of Toronto Biochemistry 
                Department and the Samuel Lunenfeld Research 
                Institute, Mount Sinai Hospital
                http://bioinfo.mshri.on.ca  hogue@mshri.on.ca
                ftp://ftp.mshri.on.ca/


  PUBLICATION TO CITE:

****************************************************************************/
/**
** @file	ll_odbc.h
**
** @brief	Locus Link database management (on ODBC layer)
**		Part of SeqHound
** @author	Hao Lieu, Minzhi Lu
** @version	2.0
** @date	May 26, 2003
**
*/

#ifndef __LL_ODBC_H__
#define __LL_ODBC_H__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <ncbi.h>

#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>

#include <asn.h>
#include <ll_db_misc.h>
#include <objslristruc.h>
#include <slri_math.h>

/* include ODBC library */
#include <slri_odbc.h>
/* some functions related to Core tables are defined in intrez_odbc.h */
#include <intrez_odbc.h>

#ifdef __cplusplus
extern "C" {
#endif

#define LL_NOTFOUND 0 
#define LL_SUCCESS 1 
#define LL_ERROR -1

#define LL_APPEND_SUCCESS 0


/**************************************************
 *       PUBLIC FUNCTIONS		           *
 **************************************************/

  /**
  ** Forms a connection to the database. 
  **
  ** @return	TRUE if connection is successful, FALSE otherwise
  ** @note	This function must be called in any application that uses 
  **		this database, prior to doing anything to the database. 
  */
 	extern Boolean LL_InitCodeBase(Boolean bConnect);

  /**
  ** closes the databases. 
  **
  ** @return	TRUE if disconnection is successful, FALSE otherwise
  ** @note	This function must called at the
  **		end of your function, if you called LL_InitCodeBase. 
  */
	extern Boolean LL_InitUndoCodeBase(Boolean bConnect);
  
  /********************************************************
             PUBLIC OPEN DATABASE FUNCTIONS
  *********************************************************/
  /**
  ** opens all the databases associated with Locus Link.  
  **
  ** @note	This function must be called prior to accessing the database. 
  **		no needed in ODBC	
  */
	extern Boolean LL_OpenAllDBs(Boolean CREATE);

  /********************************************************
             PUBLIC CLOSE DATABASE FUNCTIONS
  *********************************************************/
  /**
  ** closes all the databases associated with Locus Link. 
  **
  ** @note	no needed in ODBC
  */
	extern void CloseAllLLDBs(void);

  /**
  ** closes all the LL DBs and closes the CODE4 structure 
  **
  ** @note	no needed in ODBC
  */
 	extern void CloseCodeBaseAndLLDBs(void);

  /**
  ** start a transaction by setting auto_commit off
  **
  ** @return    TRUE if successful, FALSE if fail
  */
	extern Boolean LL_StartTransaction(void);

  /**
  ** end a transaction by commiting all the changes
  **
  ** @return    TRUE if successful, FALSE if fail
  */
	extern Boolean LL_CommitTransaction(void);

  /**
  ** end a transaction by rolling back all the changes
  **
  ** @return    TRUE if successful, FALSE if fail
  */
	extern Boolean LL_RollBackTransaction(void);


  /***********************************************************
    Stub functions that mirror codebase functions.
  **********************************************************/
  Boolean OpenLL_GONoidx(CharPtr);
  Boolean ReindexLL_GO(void);
  Boolean CloseLL_GO(void);

  /**************************************************************
                      PRIVATE FUNCTIONS
   THESE FUNCTIONS BELOW ARE NOT INTENDED TO BE CALLED BY END USER
         SEE QUERY.C/QUERY.H FOR THEIR PUBLIC COUNTERPARTS
  ***************************************************************/

  /**
  ** searches the LL_GO DB for a key specified in ppgo. 
  **
  ** @param	ppgo
  **
  ** @return	If such a key is found, fills in ppgo with the 
  **		relevant data, and returns LL_SUCCESS. 
  **		If an error occurs, return LL_ERROR, 
  **		if the key is not found returns LL_NOTFOUND. 
  **
  */
	extern Int4 LL_SearchGODB(StGOdbPtr PNTR ppgo);
  
  /**
  ** searches for the key stored in ppomim in the LL_OMIM db.
  **
  ** @param	ppomin
  **	
  ** @return	If an error occurs, return LL_ERROR. 
  **		If the key is not found in the db, returns LL_NOTFOUND, 
  **		otherwise, fills in ppomim with the
  **		necessary field data and returns LL_SUCCESS. 
  **
  */ 
	extern Int4 LL_SearchOMIMDB(StOMIMdbPtr PNTR ppomim);

  /**
  ** searches for the key stored in ppomim in the LL_OMIM db. 
  **
  ** @param	ppcdd 
  ** @return	If an error occurs, return LL_ERROR. 
  **		If the key is not found in the db, returns LL_NOTFOUND, 
  **		otherwise, fills in ppomim with the
  **		necessary field data and returns LL_SUCCESS. 
  **
  */
	extern Int4 LL_SearchCDDDB(StCDDdbPtr PNTR ppcdd);

  /**
  ** searches for the key stored in ppll in the LL_LINK db. 
  **
  ** @param 	ppll
  ** @return	If an error occurs, return LL_ERROR. 
  **		If the key is not found in the db, returns LL_NOTFOUND, 
  **		otherwise, fills in ppll with the
  **		necessary field data and returns LL_SUCCESS. 
  */
  	extern Int4 LL_SearchLLDB(StLLdbPtr PNTR ppll);


/****************************************************************
                       PUBLIC APPEND FUNCTION
*****************************************************************/

  /**
  ** appends a record stored in ps to the 4 data bases LL_GO, LL_CDD,
  ** LL_OMIM, LL_LLINK. 
  **
  ** @param	ps
  ** @return	TRUE if append is successful, FALSE otherwise
  ** @note	This program does not check for duplicates. If
  **		you are reparsing the locus link database, delete the old files 
  **		before reparsing, otherwise you will have doubles of all entries.
  */
  	extern Int4 LL_AppendRecord(LL_ParseStructPtr ps);


/****************************************************************
                       PUBLIC DELETE FUNCTION
*****************************************************************/

  /**
  ** delete records from tables ll_omim, ll_llink, ll_go
  **
  ** @return    TRUE if successful,
  **            FALSE otherwise
  */
	extern Boolean DeleteRecordsInLL(void);

  /**
  ** delete GOA records from tables ll_llink, ll_go
  **
  ** @return    TRUE if successful,
  **            FALSE otherwise
  */
	extern Boolean DeleteRecordsInLL_GOA(void);


/****************************************************************
                       HELPER FUNCTION
*****************************************************************/


	static Boolean InitBindParameter();

  /**
  ** appends a record to LL_GO table 
  **
  ** @param	NPid gi
  ** @param	goid 
  ** @param	PMID 
  ** @param	eviCode 
  ** @return	TRUE if append is successful, FALSE otherwise
  */
	Int4 LL_Append2GO_DB(Int4 NPid, Int4 goID, Int4 PMID, CharPtr eviCode); 
	
  /**
  ** appends a record to LL_OMIM table 
  **
  ** @param	NPid gi
  ** @param	OMIM omim 
  ** @return	TRUE if append is successful, FALSE otherwise
  */
	Int4 LL_Append2OMIM_DB(Int4 NPid, Int4 OMIM);
	
  /**
  ** appends a record to LL_CDD table 
  **
  ** @param	NPid gi
  ** @param	CDD cddid 
  ** @param	evalue 
  ** @return	TRUE if append is successful, FALSE otherwise
  */
	Int4 LL_Append2CDD_DB(Int4 NPid, CharPtr CDD, FloatHi evalue);
	
  /**
  ** appends a record to LL_LINK table 
  **
  ** @param	NPid gi
  ** @param	LLid 
  ** @param	pMap 
  ** @return	TRUE if append is successful, FALSE otherwise
  */
	Int4 LL_Append2LLID_DB(Int4 NPid, Int4 LLid, CharPtr pMAP);

  
#ifdef __cplusplus
}
#endif
#endif
