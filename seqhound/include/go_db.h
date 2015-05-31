/* $Id: go_db.h,v 1.3 2004/10/01 16:27:02 zwang Exp $
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
 

  PROGRAM:
 

  AUTHORS:  
            Hao Lieu


  REVISION/CONTRIBUTION NOTES:

  March 1, 2001  Original MSH Revision.
                Hogue Lab - University of Toronto Biochemistry 
                Department and the Samuel Lunenfeld Research 
                Institute, Mount Sinai Hospital
                http://bioinfo.mshri.on.ca  hogue@mshri.on.ca
                ftp://ftp.mshri.on.ca/


  PUBLICATION TO CITE:

  DESCRIPTION:
  Gene Ontology database management
  Part of SeqHound

****************************************************************************/
#ifndef __GO_DB_H__
#define __GO_DB_H__

#include <go_db_misc.h>
#include <ncbi.h>

#ifdef __cplusplus
extern "C" {
#endif

  /* the size of the GO ID number */

#define GO_REC_NOTFOUND -1
#define GO_REC_SUCCESS 0

#define GO_APPEND_SUCCESS 0
#define GO_APPEND_ALREADY_EXIST -1

  /* initializes the CODE4 structure and sets various flags.
     This function must be called in any application that uses 
     this database, prior to doing anything to the database. */
  extern void GO_InitCodeBase(void);

  /* This function must called to close the CODE4 structure */
  extern void GO_InitUndoCodeBase(void);

  /* opens all the databases associated with the GO.  This
     function must be called prior to accessing the database. */
  extern Boolean GO_OpenAllDBs(Boolean CREATE);

  /* closes all the databases associated with the GO. */
  extern void CloseAllGODBs(void);

  /* explicitly flushes the databases and closes the CODE4 structure */
  extern void CloseCodeBaseAndGODBs(void);

  /**
  ** start a transaction by setting auto_commit off
  **
  ** @return    TRUE if successful, FALSE otherwise
  */
        extern Boolean GO_StartTransaction(void);

  /**
  ** end a transaction by commiting all the changes
  **
  ** @return    TRUE if successful, FALSE otherwise
  */
        extern Boolean GO_CommitTransaction(void);

  /**
  ** end a transaction by rolling back all the changes
  **
  ** @return    TRUE if successful, FALSE otherwise
  */
        extern Boolean GO_RollBackTransaction(void);

  /* appends the record with information bs and the parent of bs. 
     DB is the organization prinicple, ie if molecular function, DB is 1; if 
     biological process, DB is 2; if cellular component, DB is 3. 
     Level is the hierarchy level that bs is in. If the record
     already exists, the function does not add. This function should be
     called indirectly through the parser program. You should not 
     manually call this yourself. Returns 1 if successfull, -1 if the GO id
     is NULL or if the name of the gene is NULL.
  */
  extern Int4 GO_AppendRecord(ByteStructure *bsp, ByteStructure *pbsp, Int4 DB, Int4 Level);

  /* delete all records in GO tables
     return TRUE if success, FALSE otherwise.
  */
  extern Boolean DeleteAllRecordsInGO(void);

  /**************************************************************
                      PRIVATE FUNCTIONS
  THESE FUNCTIONS BELOW ARE NOT INTENDED TO BE CALLED BY END USER
         SEE QUERY.C/QUERY.H FOR THEIR PUBLIC COUNTERPARTS
  ***************************************************************/
  /* called by GODB_PrintAllRecords to print out the records */
  extern void GO_PrintRecords(void);

  /* prints the records of vnp. This function is called by GODB_PrintRecordsInList
     of query.c. You should not call this yourself. */
  extern void GO_PrintRecordsInList(ValNodePtr vnp);

  /* returns a linked list of GO ids of the records with ID as a GO id.
     This function is called by GODB_GetRecordByID of query.c. You should not call this yourself.
  */
  extern ValNodePtr GO_GetRecordByID(Int4 ID);

  /* returns a linked list of all the records with reference type ref.
     This function is called by GODB_GetRecordByReference of query.c
     You should not call this yourself.
  */
  extern ValNodePtr GO_GetRecordByReference(CharPtr ref);

  /* returns the children of goID. This function is called by 
     GODB_GetChildrenOf of query.c. You should not call this yourself. */
  extern ValNodePtr GO_GetChildrenOf(Int4 goID);
  
  /* returns the parents of goID. This function is called by
     GODB_GetParentOf of query.c. You should not call this yourself. */
  extern ValNodePtr GO_GetParentOf(Int4 goID);

  /* returns the ancestors of goID.  This function is called by
     GODB_GetAllAncestors of query.c. You should not call this yourself.*/
  extern ValNodePtr GO_GetAllAncestors(Int4 goID);

  /* returns the children of goID and all their children. This function is called by
     GODB_GetAllChildren of query.c. You should not call this yourself. */
  extern ValNodePtr GO_GetAllChildren(Int4 goID);

  /* returns the name of goID, this function is called by
     GODB_GetNameByID of query.c. You should not call this yourself. */
  extern CharPtr GO_GetNameByID(Int4 goID);

  /* GO databases are organized into 3 principles, molecular function, biological process
     and cellular component. This function returns the GO database file
     goID was parsed from. This function is called by GODB_GetClassification of query.c. 
     You should not call this yourself.
  */
  extern CharPtr GO_GetClassification(Int4 goID);

  /* returns the hierarchy level of goID. If return value is -1
     goID is not in the database.
  */
  /* function disabled since it has a wrong logic
  extern Int4 GO_GetHierarchyLevel(Int4 goID);*/

#ifdef __cplusplus
}
#endif
#endif


/*
$Log: go_db.h,v $
Revision 1.3  2004/10/01 16:27:02  zwang
Added transaction-related functions.

Revision 1.2  2003/08/22 15:23:37  haocl
changed GetClassification back to char *

Revision 1.1.1.3  2003/08/21 08:48:09  root
Automated import from Sourceforge.net

Revision 1.3  2003/08/20 17:12:36  haocl
updating GetClassification to return a ValNodePtr

Revision 1.2  2002/01/15 17:26:40  kaca
disabled SHoundGODBGetHierarchyLevel due to wrong logic

Revision 1.1  2001/12/19 23:22:33  kaca
files moved from ../go ../locuslink

Revision 1.3  2001/12/11 19:08:29  kaca
added Id and Log keywords

*/
