/*$Id: prebind_cgi.h,v 1.17 2003/07/28 20:24:26 ryao Exp $*/

/*
-- ***************************************************************************
--
--  Pre Biomolecular Interaction Network Database (PreBIND)
--
--  File: prebind_cgi.h
--  Description: header for prebind cgi specific functions and variables
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



#ifndef _PREBIND_CGI_
#define _PREBIND_CGI_
 


#ifndef _PREBIND_DB_
#include "prebind_db.h"
#endif  


#ifndef _PREBIND_COMMON_
#include "prebind_common.h"
#endif


#ifndef _PREBIND_LIB_
#include "prebind_lib.h"
#endif


#include <mdwww.h>


#ifdef __cplusplus
extern "C" {
#endif

/*global to keep taskapi happy--used for tracking progress of downloading large files*/
Int4 volatile ProgramProgress;
Int4 volatile ProgramProgressMax;


/*general function prototypes*/
Int2					ClosePreBIND(void);
Int2					PadStringII(FIELD4 *pf4_ACC_ACC, CharPtr accession);
PREBINDInteractorPtr	FindInteractorInListByGI(PREBINDInteractorSetPtr pisp, Int4 gi);
CharPtr					DescribeInteractionLikelihood(PREBINDPointPtr ppp);
CharPtr					DescribeInteractorLikelihood(PREBINDInteractorPtr pip);
CharPtr					DescribeInteractionPaperLikelihood(PREBINDIsvmPtr pisvmp);
ValNodePtr				SlashDelimitedNamesToValNodeList(CharPtr nama);
Int2					DescribeNameList(CharPtr slash_delimited_name_list, ValNodePtr unofficial_names, ValNodePtr redundant_names);
Boolean					AreAnyNamesInBothLists(ValNodePtr these_names, ValNodePtr unofficial_names);
Boolean					AreAllFirstListNamesinSecondList(ValNodePtr gi_names, ValNodePtr gi_many_names);
Boolean					FilterPointByNamesWithTooManyResults(Int4 gia, Int4 gib, Int4 many);


/*sorting functions*/
int LIBCALLBACK ComparePotentialInteractors(VoidPtr vp1, VoidPtr vp2);
int LIBCALLBACK CompareInteractionPapersFor2Points(VoidPtr vp1, VoidPtr vp2);
int LIBCALLBACK CompareInteractionPapersFor2Results(VoidPtr vp1, VoidPtr vp2);


/*db retrieval function prototypes*/
Int2					GetGIbyNameAndTaxon(CharPtr name, Int4 taxon);/*no longer used*/
PREBINDAccPtr			GetAccessionRecordbyGI(Int4 gi);
PREBINDSearchSetPtr		GetListofSearches(Int4 gi);
PREBINDSearchPtr		SearchFillNode(PREBINDSearchPtr head, PREBINDSearchPtr last, Int4 gi, CharPtr name, Int4 source, CharPtr searched, Uint2 status, Int4 namered, Int4 results);
PREBINDResultSetPtr		GetListofResults(PREBINDSearchPtr psp);
PREBINDResultPtr		ResultFillNode(PREBINDResultPtr head, PREBINDResultPtr last, Int4 gi, CharPtr name, Int4 result_pmid, FloatHi iscore, FloatHi iscore2, Int4 itruth, Int4 opno, Int4 opyes);
PREBINDPointSetPtr		GetListofPointsbyPMID(Int4 pmid);
PREBINDPointPtr			GetPointbyPMIDandGIs(Int4 pmid, Int4 gia, Int4 gib);
PREBINDPointSetPtr		GetListofPointsby2GIs(Int4 gia, Int4 gib, Boolean filter_nonint_papers, Int4 filter_toomanyresults);
PREBINDInteractorSetPtr GetListofInteractorsForGI(Int4 gi, Boolean filter_nonint_papers, Int4 filter_toomanyresults);
PREBINDIsvmPtr			GetIsvmbyPMID(Int4 pmid);
ValNodePtr				GetListofUnofficialNamesbyGI(Int4 gi);
ValNodePtr				GetListofRedundantNamesbyGI(Int4 gi);
ValNodePtr				GetListofNamesWithManyResults(Int4 gii, Int4 filter_toomanyresults);
Int4					GetGIbyAccession(CharPtr accession_input);
ValNodePtr				GetListofGIsbyNameAndTaxon(CharPtr name, Int4 taxon_to_search);

/*db write function prototypes*/
Int2					Submit(Int4 pmid, Int4 gia, Int4 gib, Int4 sgia, Int4 sgib, CharPtr nama, CharPtr namb, CharPtr adesc, CharPtr bdesc, Int4 atype, Int4 btype, Int4 aok, Int4 bok, Int4 afrom, Int4 bfrom, Int4 ato, Int4 bto, Int4 opin, Int4 method, CharPtr fname, CharPtr lname, Int4 buid, CharPtr piid);
Int2					RecordPointOpinion(Int4 pmid, Int4 gia, Int4 gib, Int4 new_opinion);
Int4 					SubmitToBIND(Int4 pmid, Int4 gia, Int4 gib, Int4 sgia, Int4 sgib, CharPtr nama, CharPtr namb, CharPtr adesc, CharPtr bdesc, Int4 atype, Int4 btype, Int4 aok, Int4 bok, Int4 afrom, Int4 bfrom, Int4 ato, Int4 bto, Int4 opin, Int4 method, CharPtr fname, CharPtr lname, Int4 buid);
Int4                    SubmitToExistingBINDRecord(Int4 pmid, Int4 sgia, Int4 sgib, Int4 gia, Int4 gib, CharPtr fname, CharPtr lname, Int4 buid, Int4 iid);

/*output function prototypes*/
Int4					PrintListofSearches(PREBINDSearchSetPtr pssp);
Int2					PrintListofResults(PREBINDResultSetPtr prsp);
Int2					PrintSummaryAccessionRecord(PREBINDAccPtr pap);
Int2					PrintListofPointsForGI(PREBINDPointSetPtr ppsp, Int4 gi);
Int2					PrintListofPointsForPMID(Int4 pmid, Int4 gia, Int4 gib, Int2 frames_ready);
Int2					PrintListofEvidenceSupportingInteractor(ValNodePtr vnp_head, Int4 gi_interactee, Int4 gi_interactor);
Int2					PrintErrorinFindingAccession(CharPtr accession);
Int2					PrintDisambiguationPage(ValNodePtr gi_list);
Int2					PrintAccessionRecordInfo(PREBINDAccPtr pap);

/*page function prototypes*/
Int2					GetAllDatabyGIorACCandPrint(Int4 gi, CharPtr accession);
Int2					ReviewAPoint(Int4 pmid, Int4 gia, Int4 gib);

/*linkage*/
void LinkToView(Int4 Submit_id);
void LinkToEdit(CharPtr fname, CharPtr lname, Int4 buid, Int4 Submit_id);

/*defines*/
#define LOGFILE "PreBIND_log.txt" /*name of file where errors are logged to*/


/*global declarations and initializations*/
static FILE*	test_file_ptr = NULL;
static FILE*	html_file_ptr = NULL;
static Char     cgi_path[PATH_MAX];

/*global defines required by taskaspi*/
Int4 volatile ProgramProgress;
Int4 volatile ProgramProgressMax;


#define HEADERFILE "prebind_head.txt"
#define FOOTERFILE "prebind_tail.txt"
#define SEARCHTOP  "searches_table_top.txt"
#define RESULTTOP  "result_table_top.txt"
#define INTERACTORSUMMARYHEAD  "interactor_summary_head.txt"
#define INTERACTOREVIDENCEHEAD  "interactor_evidence_head.txt"
#define RESULTHEAD "result_table_head.txt"
#define GIPOINTTOP "points_for_gi_table_top.txt"
#define GIPOINTHEAD "points_for_gi_table_head.txt"
#define PMIDPOINTHEADER "pmid_point_header.txt"
#define PMIDPOINTTOP "pmid_point_top.txt"
#define REVIEWPOINTHEADER "review_point_header.txt"
#define OPTIONSHEAD "options_head.txt"


#ifdef __cplusplus
}
#endif

#endif /*_PREBIND_CGI_*/


/*
$Log: prebind_cgi.h,v $
Revision 1.17  2003/07/28 20:24:26  ryao
Add html link functions

Revision 1.16  2003/07/25 18:18:45  ryao
Allow users to add pmids to an existing BIND record

Revision 1.15  2003/06/19 18:43:58  janicek
roll back to 1.13 and added globals to keep taskapi happy

Revision 1.14  2003/06/18 14:32:21  ryao
allow users to add interaction papers to existing BIND record.

Revision 1.13  2003/03/21 08:42:38  janicek
Added sort and filter functions

Revision 1.12  2003/03/18 22:44:45  janicek
Added result filter for non-int papers

Revision 1.11  2003/02/27 08:00:49  janicek
Added Summary List of Interactors to output

Revision 1.10  2003/02/26 22:56:28  janicek
Reorganized code for better modularity

Revision 1.9  2003/02/26 06:55:10  janicek
Added code to add Molecule A and B names to interactor list output

Revision 1.8  2003/02/21 22:35:09  janicek
Finished sorting interactors

Revision 1.7  2003/02/20 22:32:03  janicek
Added functions to output interactor list

Revision 1.6  2003/02/18 22:21:21  janicek
Added function GetListofPointsby2GIs

Revision 1.5  2003/02/12 23:01:37  janicek
Began adding functions to retrieve and sort interactor sets

Revision 1.4  2003/01/30 19:57:33  janicek
Improved logging

Revision 1.3  2002/12/03 19:33:09  janicek
Moved Anonymous and Genetic User Id's to config file

Revision 1.2  2002/07/30 00:09:53  janicek
changed PadString to PadStringII

Revision 1.1.1.1  2002/07/17 19:36:35  janicek
initial import

*/




