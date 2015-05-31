/* $Id: go_query.h,v 1.1.1.2 2002/01/16 09:23:29 root Exp $
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

  DESCRIPTION
  Gene Ontology database query
  Part of SeqHound

****************************************************************************/
#ifndef __GO_QUERY_H__
#define __GO_QUERY_H__

#include <ncbi.h>

#ifdef __cplusplus
extern "C" {
#endif

  /* initializes all the database fields and tags. This funciton 
     must be called prior to any queries to the database can be
     properly executed. Call this at the start of your program. */
  extern void GODB_InitCodeBase(void);

  /* Opens all the database files needed for querying. This funciton 
     must be called prior to any queries to the database can be
     properly executed. Call this at the start of your program. */
  extern Boolean GODB_OpenAllDBs(Boolean CREATE);
  
  /* closes the database files and flushes all buffers.
     at the end of your program you must call this function. */
  extern void GODB_InitUndoCodeBase(void);

  /* displays all the records in the GO databases, including any references, 
     parent/structure relationships, references and synonyms
  */
  extern void GODB_PrintAllRecords(void);

  /* prints all the information about the records in vnp */
  extern void GODB_PrintRecordsInList(ValNodePtr vnp);


  
  /* all below declarations are in seqhound.h */
  /* returns a linked list containing the GO id of all the records 
     with a reference type refType, where refType is eg. "EC".
     If refType not found, returns NULL.
     It is the calling function's responsibility to free the list and all the
     data in the list.
  */
  /*extern ValNodePtr SHoundGODBGetRecordByReference(CharPtr refType);*/

  /* returns a linked list containing all the GO id's of the
     parents of goID. 
     It is the calling function's responsibility to free the list and 
     all the data in the list.
  */
  /*extern ValNodePtr SHoundGODBGetParentOf(Int4 goID);*/
 
  /* returns a list of all the go ids of the ancestors of goID.
     It is the calling functions responsibility to free the list and all
     data in the list.
  */
  /*extern ValNodePtr SHoundGODBGetAllAncestors(Int4 goID);*/

/* returns a list of all the go ids of the children of goID and all their children...
   It is the calling functions responsibility to free the list and all
   data in the list.
*/
  /*extern ValNodePtr SHoundGODBGetAllChildren(Int4 goID);*/

  /* returns a linked list containing all the GO id's of the
     children of goID.
     It is the calling function's responsibility to free the list and 
     all the data in the list.
  */
  /*extern ValNodePtr SHoundGODBGetChildrenOf(Int4 goID);*/

  /* returns the name of goid.  It is the calling function's responsibility
     to free the memory of the returned string.
  */
  /*extern CharPtr SHoundGODBGetNameByID(Int4 goID);*/

  /* GO databases are organized into 3 principles, molecular function, 
     biological process and cellular component. This function returns
     the GO database file goID was parsed from.
     If return value is NULL, goID is not in the database.
     goID should not have leading zeros, eg 0002345 should be 2345.
  */
  /*extern CharPtr SHoundGODBGetClassification(Int4 goID);*/
  
  /* returns the hierarchy level of goID. If return value is -1, goID
     is not in the database. goID should not have any leading zeros.
  */
  /* function disabled since it has a wrong logic*/
  /*extern Int4 SHoundGODBGetHierarchyLevel(Int4 goID);*/
#ifdef __cplusplus
}
#endif
#endif

/* 
$Log: go_query.h,v $
Revision 1.1.1.2  2002/01/16 09:23:29  root
Automated import from Sourceforge.net

Revision 1.2  2002/01/15 17:26:40  kaca
disabled SHoundGODBGetHierarchyLevel due to wrong logic

Revision 1.1  2001/12/19 23:22:34  kaca
files moved from ../go ../locuslink

Revision 1.3  2001/12/11 19:08:29  kaca
added Id and Log keywords

*/


