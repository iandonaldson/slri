/* $Id: go_query.c,v 1.3 2004/02/19 17:23:39 zwang Exp $
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

  June 1, 2001  Original MSH Revision.
                Hogue Lab - University of Toronto Biochemistry 
                Department and the Samuel Lunenfeld Research 
                Institute, Mount Sinai Hospital
                http://bioinfo.mshri.on.ca  hogue@mshri.on.ca
                ftp://ftp.mshri.on.ca/


  PUBLICATION TO CITE:

  DESCRIPTION:
  Gene Ontoloty database query
  Part of SeqHound

****************************************************************************/
#include <intrez_cfg.h>
#include <go_db.h>
#include <go_query.h>

/**************************************************************
                 PUBLIC INITIALIZATION FUNCTIONS
***************************************************************/

/* Opens all the database files needed for querying. This funciton 
   must be called prior to any queries to the database can be
   properly executed. Call this at the start of your program. */
Boolean GODB_OpenAllDBs(Boolean CREATE)
{
  return GO_OpenAllDBs(CREATE);
}

/* initializes all the database fields and tags. This funciton 
   must be called prior to any queries to the database can be
   properly executed. Call this at the start of your program. */
void GODB_InitCodeBase(void)
{
  GO_InitCodeBase();
}

/* closes the database files and flushes all buffers.
   at the end of your program you must call this function. */
void GODB_InitUndoCodeBase(void)
{
  GO_InitUndoCodeBase();
}

/****************************************************************
		     PUBLIC PRINT FUNCTIONS
*****************************************************************/
/* displays all the records in the GO databases, including any references, 
   parent/structure relationships, references and synonyms
*/
void GODB_PrintAllRecords(void)
{
  GO_PrintRecords();
}

/* prints all the information about the records in vnp */
void GODB_PrintRecordsInList(ValNodePtr vnp)
{
  if(vnp == NULL){
    ErrPostEx(SEV_ERROR, 0, 0, "GODB_PrintRecords: Passed ValNodePtr is NULL.");
    return;
  }

  GO_PrintRecordsInList(vnp);
  return;
}


/******************************************************************
                   PUBLIC RETRIEVAL FUNCTIONS
*******************************************************************/


/* returns a linked list containing the GO id of all the records 
   with a reference type refType, where refType is eg. "EC".
   If refType not found, returns NULL.
   It is the calling function's responsibility to free the list and all the
   data in the list.
*/
ValNodePtr LIBCALL SHoundGODBGetRecordByReference(CharPtr refType)
{
  ValNodePtr vnp = NULL;

  if(!SHoundModule("godb"))
  {
     ErrPostEx(SEV_ERROR,0,0, "SHoundGODBGetRecordByReference: Missing required SeqHound module.");
     return NULL;
  }

  if(refType == NULL || !StrCmp(refType, "")){
    ErrPostEx(SEV_ERROR, 0, 0, "SHoundGODBGetRecordByReference: Passed char pointer is NULL.");
    return NULL;
  }

  /* calling the private function in go_db.c to do the actual work */
  vnp = GO_GetRecordByReference(refType);
  if (vnp == NULL){
    ErrPostEx(SEV_ERROR, 0, 0, "SHoundGODBGetRecordsByReference: RECORD NOT FOUND.");
  }

  return vnp;
}

/* returns a linked list containing all the GO id's of the
   children of goID. goID should not have any leading zeros.
   It is the calling function's responsibility to free the list and 
   all the data in the list.
*/
ValNodePtr LIBCALL SHoundGODBGetChildrenOf(Int4 goID)
{
  ValNodePtr vnp = NULL;

  if(!SHoundModule("godb"))
  {
     ErrPostEx(SEV_ERROR,0,0, "SHoundGODBGetChildrenOf: Missing required SeqHound module.");
     return NULL;
  }

  if(goID <= 0){
    ErrPostEx(SEV_ERROR, 0, 0, "SHoundGODBGetChildrenOf: goID is NULL.");
    return NULL;
  }

  /* calling the private function in go_db.c to do the actual work */
  vnp = GO_GetChildrenOf(goID);
  if(vnp == NULL){
    ErrPostEx(SEV_ERROR, 0, 0, "SHoundGODBGetChildrenOf: GO:%d: RECORD NOT FOUND.", goID);
  }

  return vnp;
}

/* returns a linked list containing all the GO id's of the
   parents of goID. goID should not have leading zeros, eg 0003454 should be
   3454. It is the calling function's responsibility to free the list and 
   all the data in the list.
*/
ValNodePtr LIBCALL SHoundGODBGetParentOf(Int4 goID)
{
  ValNodePtr vnp = NULL;

  if(!SHoundModule("godb"))
  {
     ErrPostEx(SEV_ERROR,0,0, "SHoundGODBGetParentOf: Missing required SeqHound module.");
     return NULL;
  }

  if(goID <= 0){
    ErrPostEx(SEV_ERROR, 0, 0, "SHoundGODBGetParentOf: goID is NULL.");
    return NULL;
  }

  /* calling the private function in go_db.c to do the actual work */
  vnp = GO_GetParentOf(goID);
  if(vnp == NULL){
    ErrPostEx(SEV_ERROR, 0, 0, "SHoundGODBGetParentsOf: GO:%d: RECORD NOT FOUND.", goID);
  }

  return vnp;
}

/* returns a list of all the go ids of the ancestors of goID.
   It is the calling functions responsibility to free the list and all
   data in the list.
*/
ValNodePtr LIBCALL SHoundGODBGetAllAncestors(Int4 goID)
{
  ValNodePtr vnp = NULL;

  if(!SHoundModule("godb"))
  {
     ErrPostEx(SEV_ERROR,0,0, "SHoundGODBGetAllAncestors: Missing required SeqHound module.");
     return NULL;
  }

  if(goID <= 0){
    ErrPostEx(SEV_ERROR, 0, 0, "SHoundGODBGetAllAncestors: goID is NULL.");
    return NULL;
  }

  /* calling the private function in go_db.c to do the actual work */
  vnp = GO_GetAllAncestors(goID);
  if(vnp == NULL){
    ErrPostEx(SEV_ERROR, 0, 0, "SHoundGODBGetAllAncestors: GO:%d: RECORD NOT FOUND.", goID);
  }

  return vnp;
}

/* returns a list of all the go ids of the children of goID and all their children...
   It is the calling functions responsibility to free the list and all
   data in the list.
*/
ValNodePtr LIBCALL SHoundGODBGetAllChildren(Int4 goID)
{
  ValNodePtr vnp = NULL;

  if(!SHoundModule("godb"))
  {
     ErrPostEx(SEV_ERROR,0,0, "SHoundGODBGetAllChildren: Missing required SeqHound module.");
     return NULL;
  }

  if(goID <= 0){
    ErrPostEx(SEV_ERROR, 0, 0, "SHoundGODBGetAllChildren: goID is NULL.");
    return NULL;
  }

  /* calling the private function in go_db.c to do the actual work */
  vnp = GO_GetAllChildren(goID);
  if(vnp == NULL){
    ErrPostEx(SEV_ERROR, 0, 0, "SHoundGODBGetAllChildren: GO:%d: RECORD NOT FOUND.", goID);
  }

  return vnp;
}

/* returns the name of goID. goID should not have leading zeros, 
   eg 0002345 should be 2345. It is the calling function's responsibility 
   to free the memory of the returned string. */
CharPtr LIBCALL SHoundGODBGetNameByID(Int4 goID)
{
  CharPtr pchar = NULL;

  if(!SHoundModule("godb"))
  {
     ErrPostEx(SEV_ERROR,0,0, "SHoundGODBGetNameByID: Missing required SeqHound module.");
     return NULL;
  }

  if(goID <= 0){
    ErrPostEx(SEV_ERROR, 0, 0, "SHoundGODBGetNameByID: goID is NULL.");
    return NULL;
  }

  /* calling the private function in go_db.c to do the actual work */
  pchar = GO_GetNameByID(goID);
  if(pchar == NULL){
    ErrPostEx(SEV_ERROR, 0, 0, "SHoundGODBGetNameByID: GO:%d: RECORD NOT FOUND.", goID);
  }

  return pchar;
}

/* GO databases are organized into 3 principles, molecular function, biological process
   and cellular component. This function returns the GO database file
   goID was parsed from. goID should not have leading zeros, eg 0002345 should be 2345.
*/
CharPtr LIBCALL SHoundGODBGetClassification(Int4 goID)
{
  CharPtr class = NULL;
  CharPtr retVal = NULL;

  if(!SHoundModule("godb"))
  {
     ErrPostEx(SEV_ERROR,0,0, "SHoundGODBGetClassification: Missing required SeqHound module.");
     return NULL;
  }

  if(goID <= 0){
    ErrPostEx(SEV_ERROR, 0, 0, "SHoundGODBGetClassification: goID is NULL.");
    return NULL;
  }

  /* calling the private function in go_db.c to do the actual work */
  class = GO_GetClassification(goID);
  if(class == NULL){
    ErrPostEx(SEV_ERROR, 0, 0, "SHoundGODBGetClassification: GO:%d: RECORD NOT FOUND\n", goID);
  }
  retVal = MemNew((size_t) (StringLen(class) + 1)* sizeof(char));
  StringCpy(retVal, class);
  return retVal;
}

/* Returns the hierarchy level of goID. If return value is -1, goID
   is not in the database.
   goID should not have leading zeros, eg 0002345 should be 2345.
*/
/* function disabled since it has a wrong logic
Int4 LIBCALL SHoundGODBGetHierarchyLevel(Int4 goID)
{
  Int4 level = 0;

  if(!SHoundModule("godb"))
  {
     ErrPostEx(SEV_ERROR,0,0, "SHoundGODBGetHierarchyLevel: Missing required SeqHound module.");
     return -1;
  }

  if(goID <= 0){
    ErrPostEx(SEV_ERROR, 0, 0, "SHoundGODBHierarchyLevel: goID is NULL.");
    return -1;
  }

  
  level = GO_GetHierarchyLevel(goID);
  if (level == -1){
    ErrPostEx(SEV_ERROR, 0, 0, "SHoundGODBGetHierarchyLevel: GO:%d: RECORD NOT FOUND\n", goID);
  }

  return level;
}*/

/*
$Log: go_query.c,v $
Revision 1.3  2004/02/19 17:23:39  zwang
Check if pass in an empty string in SHoundGODBGetRecordByReference.

Revision 1.2  2003/08/22 15:23:11  haocl
changed GetClassification back to char*, only one classification/goid

Revision 1.1.1.7  2003/08/21 08:48:18  root
Automated import from Sourceforge.net

Revision 1.8  2003/08/20 17:16:07  haocl
changed return value for GetClassification

Revision 1.7  2002/01/15 17:28:17  kaca
disabled SHoundGODBGetHierarchyLevel due to a wrong logic

Revision 1.6  2002/01/15 15:17:55  kaca
added LIBCALL to public function declarations

Revision 1.5  2001/12/19 23:31:46  kaca
added newline at EOF

Revision 1.4  2001/12/11 19:01:51  kaca
added Log keyword

*/



