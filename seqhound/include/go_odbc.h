/* $Id: go_odbc.h,v 1.6 2004/10/01 16:27:02 zwang Exp $
***************************************************************************
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
** @file 	go_odbc.h
**
** @brief 	Gene Ontology database management (on ODBC layer)
**		Part of SeqHound
** @author  	Hao Lieu, Minzhi Lu
** @version 	2.0
** @date 	May 26, 2003
**
*/


#ifndef __GO_ODBC_H__
#define __GO_ODBC_H__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* include the ncbi library */
#include <ncbi.h>

/* include the sql library */
#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>

/* include the seqhound library */
#include <go_db_misc.h>		/* this file defines ByteStructure */
#include <go_parser.h>
#include <slri_misc.h>

/* include ODBC library */
#include <slri_odbc.h>

#ifdef __cplusplus
extern "C" 
{
#endif


#define GO_REC_NOTFOUND -1
#define GO_REC_SUCCESS 0

#define GO_APPEND_SUCCESS 0
#define GO_APPEND_ALREADY_EXIST -1

#define GO_ERROR -1

/***************************************************************************
	public functions
***************************************************************************/

  /** 
  ** initializes the CODE4 structure and sets various flags.
  **
  ** @note	This function must be called in any application that uses 
  ** 		this database, prior to doing anything to the database.
  */
	extern Boolean GO_InitCodeBase(void);

  /**
  ** Forms a connection to the database
  **
  ** @return 	TRUE if connection is successful, FALSE otherwise
  **
  ** @note	this function must be called in any application that uses
  ** 		this database, prior to do anything to the database.
  */
	extern Boolean GO_ConnectToDb(void);
 
  /** 
  ** closes the database and free the environment handles
  **
  ** @return 	TRUE if disconnection is successful, FALSE otherwise
  **
  ** @note	This function must called at the end of application if you
  ** 		called GO_InitCodeBase or GO_ConnectToDb  
  */
	extern Boolean GO_InitUndoCodeBase(void);

  /**
  ** opens all the databases associated with the GO. 
  **
  ** @note	This function must be called prior to accessing the database.
  */
	extern Boolean GO_OpenAllDBs(Boolean CREATE);

  /** 
  ** closes all the databases associated with the GO.
  **
  ** @note 	not implemented 
  */
	extern void CloseAllGODBs(void);

  /**
  ** explicitly flushes the databases and closes the CODE4 structure 
  ** 
  ** @note 	not implemented
  */
	extern void CloseCodeBaseAndGODBs(void);

  /**
  ** start a transaction by setting auto_commit off 
  ** 
  ** @return 	GO_SUCCESS if successful, GO_FAILURE if fail
  */
	extern Boolean GO_StartTransaction(void);

  /**
  ** end a transaction by commiting all the changes 
  ** 
  ** @return 	GO_SUCCESS if successful, GO_FAILURE if fail
  */
	extern Boolean GO_CommitTransaction(void);

  /**
  ** end a transaction by rolling back all the changes 
  ** 
  ** @return 	GO_SUCCESS if successful, GO_FAILURE if fail
  */
	extern Boolean GO_RollBackTransaction(void);

  /**
  ** appends the record with information bs and the parent of bs. 
  **
  ** @param 	bsp
  ** @param 	pbsp
  ** @param 	DB is the organization prinicple,
  **			ie. if molecular funciton, DB is 1; if biological process, DB is 2;
  **			if cellular component, DB is 3.
  ** @param 	Level is the hierarchy level that bs is in
  **
  ** @return 	GO_SUCCESS if successful, GO_FAILURE if the GO id is NULL or if name of the gene is NULL
  **
  ** @note 	if the record already exists, the function does not add.
  **		this function should be called indirectly through the parser program. you 
  **		should not manually call this yourself
  */

	extern Int4 GO_AppendRecord(ByteStructure *bsp, ByteStructure *pbsp, Int4 DB, Int4 Level);

  /** 
  ** delete all records from tables go_name, go_parent, go_reference and go_synonym
  **
  ** @return    TRUE if successful,
  **            FALSE otherwise
  */
        extern Boolean DeleteAllRecordsInGO(void);


  /**************************************************************
                      PRIVATE FUNCTIONS
  THESE FUNCTIONS BELOW ARE NOT INTENDED TO BE CALLED BY END USER
         SEE QUERY.C/QUERY.H FOR THEIR PUBLIC COUNTERPARTS
  ***************************************************************/

  /**
  ** called by GODB_PrintAllRecords to print out the records 
  */
	extern void GO_PrintRecords(void);

  /**
  ** prints the records of vnp. 
  ** @note	This function is called by GODB_PrintRecordsInList
  **		of query.c. You should not call this yourself. 
  */
	extern void GO_PrintRecordsInList(ValNodePtr vnp);


  /* search */

  /**
  ** search go_id in table go_name
  ** 
  ** @param	ID go_id in table go_name
  **
  ** @return 	a linked list of GO ids of the records with ID as a GO id.
  **		NULL if data not found
  **
  ** @note 	This function is called by GODB_GetRecordByID of query.c. 
  **		You should not call this yourself.
  */
	extern ValNodePtr GO_GetRecordByID(Int4 ID);

  /** 
  ** search go_id in table go_reference
  **
  ** @param 	ref go_ref_db in table go_reference
  ** 
  ** @return 	a linked list of all the go_id with reference type ref.
  **		NULL if data not found
  ** @note 	This function is called by GODB_GetRecordByReference of query.c
  **		You should not call this yourself.
  */
	extern ValNodePtr GO_GetRecordByReference(CharPtr ref);

  /** 
  ** select go_id from table go_parent
  ** 
  ** @param
  **		goID  parent_goid in table go_parent
  ** @return 	the children of goID. 
  **		NULL if data not found
  ** @note 	This function is called by 
  **		GODB_GetChildrenOf of query.c. 
  **		You should not call this yourself. 
  */
	extern ValNodePtr GO_GetChildrenOf(Int4 goID);
  
  /** 
  ** search parent_goid in table go_parent
  **
  ** @param
  **		goID go_id in table go_parent
  ** @return 	the parents of goID. 
  **		NULL if data not found
  ** @note	This function is called by
  **		GODB_GetParentOf of query.c. 
  **		You should not call this yourself. 
  */
	extern ValNodePtr GO_GetParentOf(Int4 goID);

  
  /** 
  ** search all the ancestors go_id from table go_parent
  **
  ** @param
  **		goID  go_id in table go_parent
  ** @return 	the ancestors of goID.  
  **		NULL if data not found
  ** @note	This function is called by
  **		GODB_GetAllAncestors of query.c. 
  **		You should not call this yourself.
  */
	extern ValNodePtr GO_GetAllAncestors(Int4 goID);

  /**
  ** search all the children go_id from table go_parent
  ** 
  ** @param
  **		goID parent_goid in table go_parent 
  ** @return 	the children of goID and all their children. 
  **		NULL if data not found
  ** @note	This function is called by
  **		GODB_GetAllChildren of query.c. 
  **		You should not call this yourself. 
  */
	extern ValNodePtr GO_GetAllChildren(Int4 goID);

  /**
  ** search the name of go_id from table go_name
  **
  ** @param
  ** 		goID  go_id in table go_name 
  ** @return 	the name of goID, 
  **		NULL if data not found
  ** @note	this function is called by
  **		GODB_GetNameByID of query.c. 
  **		You should not call this yourself. 
  */
	extern CharPtr GO_GetNameByID(Int4 goID);

  /** 
  ** search the db classification from table go_name
  ** GO databases are organized into 3 principles, molecular function, biological process
  ** and cellular component. 
  **
  ** @param
  **		goID go_id in table go_name
  ** @return 	the GO database file goID was parsed from. 
  **		NULL if data not found
  ** @note	This function is called by GODB_GetClassification of query.c. 
  **		You should not call this yourself.
  */
	extern CharPtr GO_GetClassification(Int4 goID);


  /* search the table, used for other search functions */
	static ValNodePtr getRecord(SQLINTEGER, CharPtr, CharPtr, Int4, CharPtr, CharPtr);



  /* append */

  /** 
  ** append one row of data into table go_name 
  ** 
  ** @param	goID go_id in table go_name
  ** @param	name go_name
  ** @param	DB go_db
  ** @param	Level go_level
  ** @return	GO_APPEND_SUCCESS if append is successful,
  **		GO_FAILURE otherwise
  */
	Int4 GO_Append_Name(Int4 goID, CharPtr name, Int4 DB, Int4 Level);

  /** append one row of data into table go_parent 
  ** 
  ** @param 	goID go_id in table go_parent
  ** @param	parentID parent_goid
  ** @return	GO_APPEND_SUCCESS if append is successful,
  **		GO_FAILURE otherwise
  */
	Int4 GO_Append_Parent(Int4 goID, Int4 parentID);

  /** 
  ** append one row of data into table go_synonym 
  **
  ** @param	goID go_id in table go_synonym
  ** @param	synonym go_syn
  ** @return	GO_APPEND_SUCCESS if append is successful,
  **		GO_FAILURE otherwise
  */
	Int4 GO_Append_Synonym(Int4 goID, CharPtr synonym);

  /** append one row of data into table go_reference 
  **
  ** @param	goID go_id in table go_reference
  ** @param	reference go_ref
  ** @return	GO_APPEND_SUCCESS if append is successful,
  **		GO_FAILURE otherwise
  */
	Int4 GO_Append_Reference(Int4 goID, CharPtr reference);

  /* bind the variable for insert use */
	static Boolean InitBindParameterforInsert();

  void CloseAllGODBs(void);
  void GO_PrintRecordsInList(ValNodePtr vnp);


#ifdef __cplusplus
}
#endif

#endif
