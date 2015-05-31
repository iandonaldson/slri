/*  $Id: taxdb_odbc.h,v 1.3 2004/10/01 16:27:02 zwang Exp $
-- ***************************************************************************
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


  REVISION/CONTRIBUTION NOTES:

  June 1, 2000  Original MSH Revision.
                Hogue Lab - University of Toronto Biochemistry
                Department and the Samuel Lunenfeld Research
                Institute, Mount Sinai Hospital
                http://bioinfo.mshri.on.ca  hogue@mshri.on.ca
                ftp://ftp.mshri.on.ca/


  PUBLICATION TO CITE:


******************************************************************************************/
/**
** @file	taxdb_odbc.h
**
** @brief	taxonomy database management (on ODBC layer)
**		Part of SeqHound
** @authors	Gary Bader, Christopher W.V., Hogue hogue@mshri.on.ca, 
**		Minzhi Lu
** @version	2.0
** @date	June 03, 2003
**
*/



#ifndef _TAXDB_ODBC_
#define _TAXDB_ODBC_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <ncbi.h>

#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>

#include <asn.h>
#include <slri_math.h>
#include <slri_asn.h>
#include <objslritax.h>

/* odbc library */
#include <slri_odbc.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
#define TAX_APPEND_SUCCESS 0
#define TAX_UPDATE_SUCCESS 0
*/
#define TAX_SUCCESS 0

  /************************/
  /*General Initialization*/
  /************************/

  /**
  ** Forms a connection to the database. 
  **
  ** @return	TRUE if connection is successful, FALSE otherwise
  ** @note	This function must be called in any application that uses 
  **		this database, prior to doing anything to the database. 
  */
	extern Boolean OpenTaxDB(Boolean CREATE);

  /**
  ** closes the databases. 
  **
  ** @return	TRUE if disconnection is successful, FALSE otherwise
  ** @note	This function must called at the
  **		end of your function, if you called LL_InitCodeBase. 
  */
	extern Boolean CloseTaxDB(void);

  /**
  ** Initialize connection for the taxonomy database
  **
  ** @return	TRUE if connection is successful, FALSE otherwise
  ** @note	This function must be called in any application that uses 
  **		this database, prior to doing anything to the database. 
  **
  */
	extern Boolean InitCodeBaseTax(void);

  /**
  ** Opens all taxonomy database 
  **
  ** @returns 	TRUE if any database was created
  ** @note	not implemented in ODBC
  */
	extern Boolean OpenAllTaxDBs(Boolean CREATE);

  /**
  ** Close all taxonomy databases
  **
  ** @note	not implemented in ODBC
  */
	extern void CloseAllTaxDBs(void);

  /**
  ** Close codebase for the taxonomy database
  **
  ** @return 	TRUE if closing is successful, FALSE otherwise
  */
	extern Boolean CloseCodeBaseTax(void);


  /**
  ** start a transaction by setting auto_commit off
  **
  ** @return    TRUE if successful, FALSE otherwise
  */
        extern Boolean StartTransactionTax(void);

  /**
  ** end a transaction by commiting all the changes
  **
  ** @return    TRUE if successful, FALSE otherwise
  */
        extern Boolean CommitTransactionTax(void);

  /**
  ** end a transaction by rolling back all the changes
  **
  ** @return    TRUE if successful, FALSE otherwise
  */
        extern Boolean RollBackTransactionTax(void);



  /**********************/
  /*	APPENDING 	*/
  /**********************/


  /**
  ** Append an SLRI-taxon record to the taxdb table 
  **
  ** @param 	stp
  ** @return 	TAX_APPEND_SUCCESS if append is successful, 
  **		-1 if primary key is already exist
  **		-2 otherwise
  */
	extern Int2 DB_WriteTaxRec(SLRITaxonPtr stp);

  /**
  ** Append an SLRITaxonDiv record to the divdb table
  **
  ** @param 	stdp
  ** @retur	TAX_APPEND_SUCCESS if append is successful, 
  **		-1 if primary key is already exist
  **		-2 otherwise
  */
	extern Int2 DB_WriteTaxDivRec(SLRITaxonDivPtr stdp);

  /**
  ** Append an SLRITaxonGencode record to the gcodedb table
  **
  ** @param	stgp
  ** @return	TAX_APPEND_SUCCESS if append is successful, 
  **		-1 if primary key is already exist
  **		-2 otherwise
  */
	extern Int2 DB_WriteTaxGenCodeRec(SLRITaxonGencodePtr stgp);


  /**
  ** Append a deleted node record to the del table
  **
  ** @param	taxId taxid
  ** @return	TAX_APPEND_SUCCESS if append is successful, 
  **		-1 if primary key is already exist
  **		-2 otherwise
  */
	extern Int2 DB_WriteTaxDelNodeRec(Int4 taxId);


  /**
  ** Append a merged node record to the merge table
  **
  ** @param	oldTaxId
  ** @param	newTaxId
  ** @return	TAX_APPEND_SUCCESS if append is successful, 
  **		-1 if primary key is already exist
  **		-2 otherwise
  */
	extern Int2 DB_WriteTaxMergedNodeRec(Int4 oldTaxId, Int4 newTaxId);


  /**********************/
  /*	UPDATING 	*/
  /**********************/

  /**
  ** update an SLRI-Taxon record in the taxdb table
  **
  ** @param 	stp
  ** @return	TAX_UPDATE_SUCCESS if updating is successful, 
  **		-1 if input data is NULL
  **		-2 otherwise 
  ** 
  */
	extern Int2 DB_ReplaceTaxRec(SLRITaxonPtr stp);

  /**********************/
  /*	SEARCHING 	*/
  /**********************/


  /**
  ** search an SLRI-taxon record from the taxdb table
  **
  ** @param 	taxId taxid
  **
  ** @return 	a struct that holds the record
  ** 		or NULL if not found
  */
	extern SLRITaxonPtr DB_GetTaxRec(Int4 taxId);


  /**
  ** Search an SLRITaxonDiv record from the divdb table
  **
  ** @param 	divId divid
  ** @return	a struct that holds the record
  **		NULL if not found
  */
	extern SLRITaxonDivPtr DB_GetTaxDivRec(Int4 divId);


  /**
  ** Search an SLRITaxonGencode record from the gcodedb table
  **
  ** @param	genCodeId gcodeid
  ** @return	a struct that holds the record
  **		NULL if not found
  */

	extern SLRITaxonGencodePtr DB_GetTaxGenCodeRec(Int4 genCodeId);

  /**
  ** Search a deleted node record from the del table
  ** 
  ** @param	taxId taxid
  ** @return
  */

	extern Int4 DB_GetTaxDelNodeRec(Int4 taxId);


  /**
  ** Search an old Tax ID given a new Tax ID from the merged node database 
  **
  ** @param	newTaxId
  ** @return	the old tax id
  */

	extern Int4 DB_GetTaxMergedNodeRecOldTIDByNewTID(Int4 newTaxId);

  /**
  ** Search a new Tax ID given an old Tax ID from the merged node database 
  ** 
  ** @param	oldTaxId
  ** @return	a new tax id
  */

	extern Int4 DB_GetTaxMergedNodeRecNewTIDByOldTID(Int4 oldTaxId);

  /** 
  ** This function takes each node in the taxdb database.  It gets its parent node
  ** and then the current node is added to the list of children of the parent node  
  ** that was retrieved.  This updated parent node is then replaced in the database.
  ** @return 	0 if function successful
  **		-1 otherwise
  ** @note	used in importtaxdb.h
  */
	extern Int2 TaxIDDisk_BuildChildrenNodeList(void);

  /*********************************/
  /*	DELETING 	           */
  /* note: not implemented in ODBC */
  /*********************************/
  /* delete all records in TAX tables
     return TRUE if success, FALSE otherwise.
  */
  extern Boolean DeleteAllRecordsInTAX(void);

  /* Delete an SLRI-taxon record from the database */
	extern Int4 DB_DelTaxRec(Int4 taxId);

  /* Undelete an SLRI-taxon record from the database */
	extern Int4 DB_UnDelTaxRec(Int4 taxId);

  /* Delete an SLRITaxonDiv record from the database */
	extern Int4 DB_DelTaxDivRec(Int4 divId);

  /* Undelete an SLRITaxonDiv record from the database */
	extern Int4 DB_UnDelTaxDivRec(Int4 divId);

  /* Delete an SLRITaxonGencode record from the database */
	extern Int4 DB_DelTaxGenCodeRec(Int4 genCodeId);

  /* Undelete an SLRITaxonGencode record from the database */
	extern Int4 DB_UnDelTaxGenCodeRec(Int4 genCodeId);

  /* Delete a deleted node record from the database */
	extern Int4 DB_DelTaxDelNodeRec(Int4 taxId);

  /* Undelete a deleted node record from the database */
	extern Int4 DB_UnDelTaxDelNodeRec(Int4 taxId);

  /* Delete a merged node record from the database by new Tax ID*/
	extern Int4 DB_DelTaxMergedNodeRec(Int4 newTaxId);

  /* Undelete a merged node record from the database by New Tax ID*/
	extern Int4 DB_UnDelTaxMergedNodeRec(Int4 newTaxId);

  /*********************************/
  /*	PACKING 	           */
  /* note: not implemented in ODBC */
  /*********************************/

  /*Pack the TAX DB*/
	extern void DB_PackTAXDB();

  /*Pack the DIV DB*/
	extern void DB_PackDIVDB();

  /*Pack the GCODE DB*/
	extern void DB_PackGCODEDB();

  /*Pack the DEL DB*/
	extern void DB_PackDELDB();

  /*Pack the MERGE DB*/
	extern void DB_PackMERGEDB();

  /* Pack all of the taxonomy databases */
	extern void DB_PackAllTaxDB(void);

  
	static Boolean InitBindParameterforInsert();


#ifdef __cplusplus
}
#endif
#endif		/* _TAXDB_DB_ */
