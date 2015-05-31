/* $Id: ll_db.h,v 1.9 2005/04/26 18:54:20 eburgess Exp $
**************************************************************************
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
  Locus Link database management
  Part of SeqHound

****************************************************************************/
#ifndef __LL_DB_H__
#define __LL_DB_H__

#include <asn.h>
#include <ll_db_misc.h>
#include <objslristruc.h>

#ifdef __cplusplus
extern "C" {
#endif

#define LL_NOTFOUND 0
#define LL_SUCCESS 1
#define LL_ERROR -1

#define LL_APPEND_SUCCESS 0

/**************************************************
 *       PUBLIC CODE BASE INITIALIZTION           *
 **************************************************/

  /* initializes the CODE4 structure and sets various flags.
     This function must be called in any application that uses 
     this database, prior to doing anything to the database. */
  extern void LL_InitCodeBase(Boolean bConnect);

  /* closes the databases. This function must called at the
     end of your function, if you called LL_InitCodeBase. */
  extern void LL_InitUndoCodeBase(void);
  
  /********************************************************
             PUBLIC OPEN DATABASE FUNCTIONS
  *********************************************************/
  /* opens all the databases associated with Locus Link.  This
     function must be called prior to accessing the database. */
  extern Boolean LL_OpenAllDBs(Boolean CREATE);

  
  /********************************************************
             PUBLIC CLOSE DATABASE FUNCTIONS
  *********************************************************/
  /* closes all the databases associated with Locus Link. */
  extern void CloseAllLLDBs(void);

  /* closes all the LL DBs and closes the CODE4 structure */
  extern void CloseCodeBaseAndLLDBs(void);


  /********************************************************
             PUBLIC TRANSACTION FUNCTIONS
  *********************************************************/
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


  /**************************************************************
                      PRIVATE FUNCTIONS
   THESE FUNCTIONS BELOW ARE NOT INTENDED TO BE CALLED BY END USER
         SEE QUERY.C/QUERY.H FOR THEIR PUBLIC COUNTERPARTS
  ***************************************************************/
  
  /* The function is called by the parser addgoid to open the table 
     LL_GO for update. The table is opened with no indexing for better
     performance. The Reindexing function ReindexLL_GO is called by the 
     parser addgoid when the update is done. */
  Boolean OpenLL_GONoidx(CharPtr);
  Boolean ReindexLL_GO(void);

  /* The function is called by the parser addgoid after it has done
     the insertion and indexing of records in the table LL_GO. */
  Boolean CloseLL_GO(void);

  /* searches the LL_GODB for a key specified in ppgo. If such a key is found, 
     fills in ppgo with the relevant data, and returns LL_SUCCESS. 
     If an error occurs, return LL_ERROR, if the key is not found 
     returns LL_NOTFOUND. */
  extern Int4 LL_SearchGODB(StGOdbPtr PNTR ppgo);
  
  /* searches for the key stored in ppomim in the LL_OMIM db.
     If an error occurs, return LL_ERROR. If the key is not found in the db, 
     returns LL_NOTFOUND, otherwise, fills in ppomim with the
     necessary field data and returns LL_SUCCESS. */ 
  extern Int4 LL_SearchOMIMDB(StOMIMdbPtr PNTR ppomim);

  /* searches for the key stored in ppomim in the LL_OMIM db. 
     If an error occurs, return LL_ERROR. If the key is not found in the db, 
     returns LL_NOTFOUND, otherwise, fills in ppomim with the
     necessary field data and returns LL_SUCCESS. */
  extern Int4 LL_SearchCDDDB(StCDDdbPtr PNTR ppcdd);

  /* searches for the key stored in ppll in the LL_LINK db. 
     If an error occurs, return LL_ERROR. If the key is not found in the db, 
     returns LL_NOTFOUND, otherwise, fills in ppll with the
     necessary field data and returns LL_SUCCESS. */
  extern Int4 LL_SearchLLDB(StLLdbPtr PNTR ppll);

/****************************************************************
                       PUBLIC APPEND FUNCTION
*****************************************************************/
  /* appends a record stored in ps to the 4 data bases LL_GO, LL_CDD,
     LL_OMIM, LL_LLINK. This program does not check for duplicates. If
     you are reparsing the locus link database, delete the old files 
     before reparsing, otherwise you will have doubles of all entries.
  */
  extern Int4 LL_AppendRecord(LL_ParseStructPtr ps);

  extern Int4 LL_Append2GO_DB(Int4 llid, Int4 goID, Int4 PMID, CharPtr eviCode);
  extern Int4 LL_Append2LLID_DB(Int4 llid, Int4 npid, CharPtr map);


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


Boolean  LIBCALL InitGENE(void);

ValNodePtr LIBCALL GetGENEDbxrefFromGi( Int4 gi, CharPtr name );
ValNodePtr LIBCALL GetGiFromGENEDbxref( CharPtr name, CharPtr access );
ValNodePtr LIBCALL GetGiFromGeneid( Int4 geneid );
ValNodePtr LIBCALL GetGeneidFromGENEDbxref( CharPtr name, 
											CharPtr access );
ValNodePtr LIBCALL GetGENEDbxrefFromGeneid( Int4 geneid, 
											CharPtr name );
Int4       LIBCALL GetGeneidFromGi( Int4 gi );
CharPtr    LIBCALL GetLocusFromGi( Int4 gi );


#ifdef __cplusplus
}
#endif
#endif


/* 
$Log: ll_db.h,v $
Revision 1.9  2005/04/26 18:54:20  eburgess
Added functions to access GENE tables.

Revision 1.8  2004/12/14 19:49:05  zwang
Added transaction related functions.

Revision 1.7  2004/06/10 16:57:42  zwang
Add flag to determine if a database connection should be established. See bug #2324.

Revision 1.6  2004/05/17 18:04:43  hlieu
fix db layer for negative locus

Revision 1.5  2004/04/16 18:20:27  vgu
Rollback changes in version 1.4

Revision 1.4  2004/04/14 14:13:28  vgu
Added Reindexing Functions to locus link tables

Revision 1.3  2004/03/17 20:06:45  eburgess
Victor: Added OpenLL_GONoidx(), ReindexLL_GO() and CloseLL_GO() to increase
speed.

Revision 1.2  2003/09/29 21:20:32  haocl
merged [obj]ll.h, [obj]shproteinreport.h into [obj]slristruc.h,
changed [obj]NBlastasn.h, ll_db.h, [obj]NBlastasn.h, [obj]slritax.h, seqhound.h
to reflect deleted file

Revision 1.1.1.3  2003/06/26 08:53:34  root
Automated import from Sourceforge.net

Revision 1.3  2003/06/25 14:12:37  haocl
modifications to allow access from yeast GO annotation

Revision 1.2  2002/02/28 17:22:59  micheld
New MSVC project files to build cb/db2/rem apps and libraries
Moved include order to accomodate msvc, removed #ifdef [CODEBASE|DB2]

Revision 1.1  2001/12/19 23:22:34  kaca
files moved from ../go ../locuslink

Revision 1.4  2001/12/11 20:19:51  kaca
added Log and Id keywords

Revision 1.3  2001/12/11 19:08:43  kaca
added Id and Log keywords

*/

