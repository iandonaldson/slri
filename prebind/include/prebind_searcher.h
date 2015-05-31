/*$Id: prebind_searcher.h,v 1.7 2003/03/28 06:55:41 janicek Exp $*/
/*
-- ***************************************************************************
--
--  Pre Biomolecular Interaction Network Database (PreBIND)
--
--  File: prebind_searcher.h
--  Description: header for prebind searcher specific functions and variables
--
--  Author: Ian Donaldson
--  Date: June 2002
--
--  Hogue Lab - University of Toronto Biochemistry Department
--  Samuel Lunenfeld Research Institute, Mount Sinai Hospital
--
--
--  Copyright Notice:
--
--  Copyright ©2000 Mount Sinai Hospital (MSH)
--
--  This program is free software; you can redistribute it and/or
--  modify it under the terms of the GNU General Public License as
--  published by the Free Software Foundation; either version 2 of
--  the License, or any later version.
--
--  This program is distributed in the hope that it will be useful,
--  but WITHOUT ANY WARRANTY; without even the implied warranty of
--  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
--
--  See the GNU General Public License for more details.
--
--  You should have received a copy of the GNU General Public License
--  along with this program; if not, write to the
--         Free Software Foundation, Inc.,
--         59 Temple Place, Suite 330, Boston, MA
--         02111-1307  USA
--  or visit http://www.gnu.org/copyleft/gpl.html
--
--  SPECIAL EXCEPTIONS
--
--  As a special exception, Mount Sinai Hospital gives permission to
--  link this program with the following non-GPL programs or libraries,
--  and distribute the resulting executable, without including the source
--  code for these in the source distribution:
--
--  a) CodeBase 6.5 or greater from Sequiter Software Inc.
--
--  b) The C or C++ interface to Oracle 8.x or greater, from
--     Oracle Corporation or IBM DB2 UDB.
--
-- ***************************************************************************
*/



#ifndef _PREBIND_SEARCHER_
#define _PREBIND_SEARCHER_


#ifndef _PREBIND_DB_
#include "prebind_db.h"
#endif  


#ifndef _PREBIND_COMMON_
#include "prebind_common.h"
#endif


#ifndef _PREBIND_LIB_
#include "prebind_lib.h"
#endif

#include <accentr.h>
#include <accutils.h>
#include <tofasta.h>
#include <tomedlin.h>
#include <asn2ff.h>

#ifdef __cplusplus
extern "C" {
#endif

#define LOGGING "pb_searcher_log.txt" /*name of file where errors are logged to*/
#define ALL_SEARCHES  0
#define NEW_SEARCHES  1
#define UPDATE_SEARCHES  2
#define MAX_RESULTS 10000
#define MAX_NAME_LIST_LENGTH 61 /*see GetNameListForGIinPMID*/

typedef struct {
    Uint4 num;
    DocUidPtr uids;
    Int4Ptr weights;
} * LocalLinkSetPtr;


/*global declarations and initializations*/
static FILE*	results_file_ptr = NULL;


/*function protypes*/
LocalLinkSetPtr LocalLinkSetNew(void);
Int2 CheckSearches(Int2 do_searches, Int4 do_only_taxid, Boolean requeue_completed_searches);
Int2 DoSearches(Int4 max_searches);
Int2 AddResultsToISVM(void);
Int2 CountResults(void);
Int2 CountResultsByGI(Int4 max_numr);
Int2 ImportSvmScores(CharPtr filename, Int4 score_list, Boolean overwrite);
Int2 ImportPointScores(Int4 score_list);
Boolean IsInteractionInPOINT(Int4 gia, Int4 gib, Int4 pmid);
CharPtr GetNameListForGIinPMID(Int4 gi, Int4 pmid);
Int2 RemoveSearchResultsForAllTaxonsExceptX(Int4 keep_taxon_searches);
Int2 CalculateNameUsage(void);
Int2 ExportPoints(void);
Int2 ExportIsvm(void);
Int2 ExportLosyn(void);
Int2 ExportSrch(void);
Int2 ExportAcc(void);

#ifdef __cplusplus
}
#endif

#endif  /*_PREBIND_SEARCHER_*/


/*
$Log: prebind_searcher.h,v $
Revision 1.7  2003/03/28 06:55:41  janicek
Added ExportAcc

Revision 1.6  2003/03/13 04:40:08  janicek
Added ExportSrch function

Revision 1.5  2002/12/11 19:24:52  janicek
Added function to export LOSYN data table

Revision 1.4  2002/11/16 00:53:14  janicek
Added function to export ISVM records

Revision 1.3  2002/10/31 20:57:05  janicek
added functions to export tables to text: ExportPoints and ExportResults

Revision 1.2  2002/08/08 15:35:39  janicek
allowed possibility of overwriting in ImportSvmScores

Revision 1.1.1.1  2002/07/17 19:36:35  janicek
initial import

*/


