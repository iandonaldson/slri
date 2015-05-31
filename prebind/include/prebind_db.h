/*$Id: prebind_db.h,v 1.3 2004/03/18 14:50:33 kzheng Exp $*/
/*
-- ***************************************************************************
--
--  Pre Biomolecular Interaction Network Database (PreBIND)
--
--  File: prebind_db.h
--  Description: header for database specific functions and variables
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




#ifndef _PREBIND_DB_
#define _PREBIND_DB_


#ifndef D4ALL_INC
#include <d4all.h>		/*codebase header must be included before ncbi.h*/
#endif

#ifndef _PREBIND_COMMON_
#include "prebind_common.h"
#endif

#ifndef _objprebind_ 
#include <objprebind.h> /*asntool generated header for objects mirroring prebind databases*/
#endif

#ifndef _PREBIND_LIB_
#include "prebind_lib.h"
#endif

#ifdef __cplusplus
extern "C"{
#endif

/*MEMO field length defines: ACCession database*/
#define MAX_DEFINITION_LENGTH			100
#define MAX_GENE_PRODUCT_NAME_LENGTH	100



/*************************/
/* database declarations */
/*************************/

CODE4 codeBase; /*codebase is a default instance of a CODE4 structure required for other codebase 
				functions */

DATA4
	*pd4_ACC,	/*this is a pointer of type DATA4 to the database ACCession database*/
	*pd4_DB,	/*this is a pointer of type DATA4 to the database DataBase database*/
	*pd4_ONAME, /*this is a pointer of type DATA4 to the Other NAME database*/
	*pd4_OGI,	/*this is a pointer of type DATA4 to the Other GI database*/
	*pd4_CDR,	/*this is a pointer of type DATA4 to the CoDing Region database*/
	*pd4_LOCUS,	/*this is a pointer of type DATA4 to the LOCUS name database*/
	*pd4_LOSYN,	/*this is a pointer of type DATA4 to the LOcus SYNonym database*/
	*pd4_YENA,	/*this is a pointer of type DATA4 to the YEast NAme database*/
	*pd4_SRCH,	/*this is a pointer of type DATA4 to the SeaRCH database*/
	*pd4_RESULT,/*this is a pointer of type DATA4 to the RESULT database*/
	*pd4_ISVM,	/*this is a pointer of type DATA4 to the Interaction SVM database*/
	*pd4_CORE,	/*this is a pointer of type DATA4 to the REsults COunt database*/
	*pd4_REGI,	/*this is a pointer of type DATA4 to the REsults by GI database*/
	*pd4_POINT,	/*this is a pointer of type DATA4 to the POtential INTeraction database*/
	*pd4_SUBMIT;	/*this is a pointer of type DATA4 to the SUBMIT database of potential interaction*/


FIELD4
	/*ACC: ACCession database*/
	/*note that field pointer names are chosen so they will be unique*/
	*pf4_ACC_ACC,		/*this is a pointer of type FIELD4 to the field called ACCESSION*/
	*pf4_ACC_GI,		/*this is a pointer of type FIELD4 to the field called GI*/
	*pf4_ACC_DB,		/*this is a pointer of type FIELD4 to the field called DATABASE*/
	*pf4_ACC_LOCUS,		/*this is a pointer of type FIELD4 to the field called LOCUS*/
	*pf4_ACC_DEF,		/*this is a pointer of type FIELD4 to the field called DEFINITION*/
	*pf4_ACC_PPROD,		/*this is a pointer of type FIELD4 to the field called PPROD*/
	*pf4_ACC_TAXID,		/*this is a pointer of type FIELD4 to the field called TAXID*/
	*pf4_ACC_UPDATED,	/*this is a pointer of type FIELD4 to the field called UPDATED*/

	/*DB: Database database*/
	*pf4_DB_GIN,		/*this is a pointer of type FIELD4 to the field called GI of Nucleotide*/
	*pf4_DB_FROM,		/*this is a pointer of type FIELD4 to the field called coding region FROM*/
	*pf4_DB_TO,			/*this is a pointer of type FIELD4 to the field called coding region TO*/
	*pf4_DB_GIP,		/*this is a pointer of type FIELD4 to the field called GI of encoded Protein*/
	*pf4_DB_DBNAME,		/*this is a pointer of type FIELD4 to the field called DBNAME*/
	*pf4_DB_IDINT,		/*this is a pointer of type FIELD4 to the field called ID (INTeger)*/
	*pf4_DB_IDSTR,		/*this is a pointer of type FIELD4 to the field called ID (STRing)*/

	/*ONAME: Other NAME database*/
	*pf4_ONAME_ACC,		/*this is a pointer of type FIELD4 to the field called ACCESSION*/
	*pf4_ONAME_GI,		/*this is a pointer of type FIELD4 to the field called GI*/
	*pf4_ONAME_ONAME,	/*this is a pointer of type FIELD4 to the field called ONAME*/

	/*OGI: Other GI database*/
	*pf4_OGI_ACC,		/*this is a pointer of type FIELD4 to the field called ACCESSION*/
	*pf4_OGI_GI,		/*this is a pointer of type FIELD4 to the field called GI*/
	*pf4_OGI_OGI,		/*this is a pointer of type FIELD4 to the field called OGI*/

	/*CDR: CoDing Region database*/
	*pf4_CDR_GIN,		/*this is a pointer of type FIELD4 to the field called GI of Nucleotide*/
	*pf4_CDR_FROM,		/*this is a pointer of type FIELD4 to the field called coding region FROM*/
	*pf4_CDR_TO,		/*this is a pointer of type FIELD4 to the field called coding region TO*/
	*pf4_CDR_GIP,		/*this is a pointer of type FIELD4 to the field called GI of encoded Protein*/

	/*LOCUS: LOCUS name database*/
	*pf4_LOCUS_GIN,		/*this is a pointer of type FIELD4 to the field called GI of Nucleotide*/
	*pf4_LOCUS_FROM,	/*this is a pointer of type FIELD4 to the field called coding region FROM*/
	*pf4_LOCUS_TO,		/*this is a pointer of type FIELD4 to the field called coding region TO*/
	*pf4_LOCUS_NAME,	/*this is a pointer of type FIELD4 to the field called NAme of Locus*/
	*pf4_LOCUS_GIP,		/*this is a pointer of type FIELD4 to the field called GI of encoded Protein*/
	
	/*LOSYN: LOcus SYNonym database*/
	*pf4_LOSYN_GIN,		/*this is a pointer of type FIELD4 to the field called GI of Nucleotide*/
	*pf4_LOSYN_FROM,	/*this is a pointer of type FIELD4 to the field called coding region FROM*/
	*pf4_LOSYN_TO,		/*this is a pointer of type FIELD4 to the field called coding region TO*/
	*pf4_LOSYN_SYN,		/*this is a pointer of type FIELD4 to the field called Locus SYNonym*/
	*pf4_LOSYN_GIP,		/*this is a pointer of type FIELD4 to the field called GI of encoded Protein*/

	/*YENA: YEast NAme database*/
	*pf4_YENA_GIP,		/*this is a pointer of type FIELD4 to the field called GI of Protein*/
	*pf4_YENA_NAME,		/*this is a pointer of type FIELD4 to the field called NAME of protein*/

	/*SRCH: SeaRCH database*/
	*pf4_SRCH_GIP,		/*this is a pointer of type FIELD4 to the field called GI of Protein*/
	*pf4_SRCH_NAME,		/*this is a pointer of type FIELD4 to the field called NAME of protein*/
	*pf4_SRCH_SOURCE,	/*this is a pointer of type FIELD4 to the field called SOURCE of name of protein*/
	*pf4_SRCH_TAXID,	/*this is a pointer of type FIELD4 to the field called TAXon ID of the source organism*/
	*pf4_SRCH_SEARCHED,	/*this is a pointer of type FIELD4 to the field called last_SEARCHED*/
	*pf4_SRCH_STATUS,	/*this is a pointer of type FIELD4 to the field called STATUS*/
	*pf4_SRCH_NAMERED,	/*this is a pointer of type FIELD4 to the field called NAME_REDundancy in organism*/
	*pf4_SRCH_RESULTS,	/*this is a pointer of type FIELD4 to the field called number_RESULTS */

	/*RESULT: RESULT database*/
	*pf4_RESULT_GIP,		/*this is a pointer of type FIELD4 to the field called GI of Protein*/
	*pf4_RESULT_NAME,		/*this is a pointer of type FIELD4 to the field called NAME of protein*/
	*pf4_RESULT_PMID,		/*this is a pointer of type FIELD4 to the field called PubMed IDentifier */

	/*ISVM: Interaction SVM database*/
	*pf4_ISVM_PMID,		/*this is a pointer of type FIELD4 to the field called PubMed IDentifier */
	*pf4_ISVM_MUID,		/*this is a pointer of type FIELD4 to the field called Medine IDentifier */
	*pf4_ISVM_ISCORE,	/*this is a pointer of type FIELD4 to the field called Interaction SCORE*/
	*pf4_ISVM_ISCORE2,	/*this is a pointer of type FIELD4 to the field called Interaction SCORE2*/
	*pf4_ISVM_OPNO,		/*this is a pointer of type FIELD4 to the field called OPinion: NO not an interaction paper*/
	*pf4_ISVM_OPYES,	/*this is a pointer of type FIELD4 to the field called OPinion: YES it is an interaction paper*/
	*pf4_ISVM_ITRUTH,	/*this is a pointer of type FIELD4 to the field called Interaction TRUTH */

	/*CORE: REsults COunt database*/
	*pf4_CORE_PMID,	/*this is a pointer of type FIELD4 to the field called PubMed IDentifier */
	*pf4_CORE_MUID,	/*this is a pointer of type FIELD4 to the field called Medine IDentifier */
	*pf4_CORE_NUMN,	/*this is a pointer of type FIELD4 to the field called NUMber of Names that found the result*/
	*pf4_CORE_NUMG,	/*this is a pointer of type FIELD4 to the field called NUMber of Gi's that found the result*/

	/*REGI: REsults by GI database*/
	*pf4_REGI_PMID,	/*this is a pointer of type FIELD4 to the field called PubMed IDentifier */
	*pf4_REGI_MUID,	/*this is a pointer of type FIELD4 to the field called Medine IDentifier */
	*pf4_REGI_GIP,	/*this is a pointer of type FIELD4 to the field called GIs of Proteins that found the result*/

	/*POINT: POtential INTeraction database*/
	*pf4_POINT_PMID,	/*this is a pointer of type FIELD4 to the field called PubMed IDentifier */
	*pf4_POINT_MUID,	/*this is a pointer of type FIELD4 to the field called Medine IDentifier */
	*pf4_POINT_GIA,		/*this is a pointer of type FIELD4 to the field called GIs of protein A in interaction*/
	*pf4_POINT_NAMA,	/*this is a pointer of type FIELD4 to the field called NAMes of protein A in interaction*/
	*pf4_POINT_ATYPE,	/*this is a pointer of type FIELD4 to the field called A: TYPE of molecule in interaction*/
	*pf4_POINT_GIB,		/*this is a pointer of type FIELD4 to the field called GIs of protein B in interaction*/
	*pf4_POINT_NAMB,	/*this is a pointer of type FIELD4 to the field called NAMes of protein B in interaction*/
	*pf4_POINT_BTYPE,	/*this is a pointer of type FIELD4 to the field called B: TYPE of molecule in interaction*/
	*pf4_POINT_OPNO,	/*this is a pointer of type FIELD4 to the field called OPinion count: NO this is not an interaction*/
	*pf4_POINT_OPYES,	/*this is a pointer of type FIELD4 to the field called OPinion count: YES this is an interaction*/
	*pf4_POINT_PISCORE,	/*this is a pointer of type FIELD4 to the field called Pairwise Interaction SCORE-  protein::protein interaction*/
	*pf4_POINT_PISCORE2,/*this is a pointer of type FIELD4 to the field called Pairwise Interaction SCORE2- protein::DNA interaction*/
	*pf4_POINT_PITRUTH,	/*this is a pointer of type FIELD4 to the field called Pairwise Interaction TRUTH*/

	/*SUBMIT: SUBMITted potential interactions database*/
	*pf4_SUBMIT_PMID,	/*this is a pointer of type FIELD4 to the field called PubMed IDentifier */
	*pf4_SUBMIT_MUID,	/*this is a pointer of type FIELD4 to the field called Medine IDentifier */
	*pf4_SUBMIT_GIA,	/*this is a pointer of type FIELD4 to the field called GIs of protein A in interaction*/
	*pf4_SUBMIT_GIB,	/*this is a pointer of type FIELD4 to the field called GIs of protein B in interaction*/
	*pf4_SUBMIT_SGIA,	/*this is a pointer of type FIELD4 to the field called Submitted GI of molecule A*/
	*pf4_SUBMIT_NAMA,	/*this is a pointer of type FIELD4 to the field called NAMes of molecule A in interaction*/
	*pf4_SUBMIT_ADESC,	/*this is a pointer of type FIELD4 to the field called DESCription of molecule A in interaction*/
	*pf4_SUBMIT_ATYPE,	/*this is a pointer of type FIELD4 to the field called A: TYPE of molecule in interaction*/
	*pf4_SUBMIT_AOK,	/*this is a pointer of type FIELD4 to the field called sgiA describes molecule A OK?*/
	*pf4_SUBMIT_AFROM,	/*this is a pointer of type FIELD4 to the field called AFROM binding location*/
	*pf4_SUBMIT_ATO,	/*this is a pointer of type FIELD4 to the field called ATO binding location*/
	*pf4_SUBMIT_SGIB,	/*this is a pointer of type FIELD4 to the field called Submitted GI of molecule B*/
	*pf4_SUBMIT_NAMB,	/*this is a pointer of type FIELD4 to the field called NAMes of molecule B in interaction*/
	*pf4_SUBMIT_BDESC,	/*this is a pointer of type FIELD4 to the field called DESCription of molecule B in interaction*/
	*pf4_SUBMIT_BTYPE,	/*this is a pointer of type FIELD4 to the field called B: TYPE of molecule in interaction*/
	*pf4_SUBMIT_BOK,	/*this is a pointer of type FIELD4 to the field called sgiB describes molecule B OK?*/
	*pf4_SUBMIT_BFROM,	/*this is a pointer of type FIELD4 to the field called BFROM binding location*/
	*pf4_SUBMIT_BTO,	/*this is a pointer of type FIELD4 to the field called BTO binding location*/
	*pf4_SUBMIT_METHOD,	/*this is a pointer of type FIELD4 to the field called METHOD used to show binding*/
	*pf4_SUBMIT_OPIN,	/*this is a pointer of type FIELD4 to the field called OPINION (interaction or not)*/
	*pf4_SUBMIT_FNAME,	/*this is a pointer of type FIELD4 to the field called First NAME*/
	*pf4_SUBMIT_LNAME,	/*this is a pointer of type FIELD4 to the field called Last NAME*/
	*pf4_SUBMIT_BUID,	/*this is a pointer of type FIELD4 to the field called Bind User ID*/
	*pf4_SUBMIT_DATE,	/*this is a pointer of type FIELD4 to the field called DATE submitted*/
	*pf4_SUBMIT_STATUS;	/*this is a pointer of type FIELD4 to the field called STATUS*/




	
TAG4
	/*ACC: Accession database*/
	*pt4_ACC_ACC,		/*this is a pointer of type TAG4 to the tag for the ACCession field*/
	*pt4_ACC_GI,		/*this is a pointer of type TAG4 to the tag for the GI field*/
	*pt4_ACC_RS,		/*this is a pointer of type TAG4 to the tag for the DataBase field*/

	/*DB: Database database*/
	*pt4_DB_GIN,		/*this is a pointer of type TAG4 to the tag for the ACCession field*/
	*pt4_DB_GIP,		/*this is a pointer of type TAG4 to the tag for the GI field*/
	*pt4_DB_IDINT,		/*this is a pointer of type TAG4 to the tag for the ID INTeger field*/
	*pt4_DB_IDSTR,		/*this is a pointer of type TAG4 to the tag for the ID STRing field*/
	*pt4_DB_UNIQUE,		/*this is a pointer of type TAG4 to the tag for a compound index key that is UNIQUE*/

	/*ONAME: Other NAME database*/
	*pt4_ONAME_ACC,		/*this is a pointer of type TAG4 to the tag for the ACCession field*/
	*pt4_ONAME_GI,		/*this is a pointer of type TAG4 to the tag for the GI field*/

	/*OGI: Other GI database*/
	*pt4_OGI_ACC,		/*this is a pointer of type TAG4 to the tag for the ACCession field*/
	*pt4_OGI_GI,		/*this is a pointer of type TAG4 to the tag for the GI field*/

	/*CDR: CoDing Region database*/
	*pt4_CDR_GIN,		/*this is a pointer of type TAG4 to the tag for the GI of Nucleotide field*/
	*pt4_CDR_GIP,		/*this is a pointer of type TAG4 to the tag for the GI of Protein field*/

	/*LOCUS: LOCUS name database*/
	*pt4_LOCUS_GIN,		/*this is a pointer of type TAG4 to the tag for the GI of Nucleotide field*/
	*pt4_LOCUS_GIP,		/*this is a pointer of type TAG4 to the tag for the GI of Protein field*/
	*pt4_LOCUS_UNIQUE,	/*this is a pointer of type TAG4 to the tag for a compound key that is unique*/

	/*LOSYN: LOcus SYNonym database*/
	*pt4_LOSYN_GIN,		/*this is a pointer of type TAG4 to the tag for the GI of Nucleotide field*/
	*pt4_LOSYN_GIP,		/*this is a pointer of type TAG4 to the tag for the GI of Protein field*/
	*pt4_LOSYN_UNIQUE,	/*this is a pointer of type TAG4 to the tag for a compound key that is unique*/

	/*YENA: YEast NAme database*/
	*pt4_YENA_GIP,		/*this is a pointer of type TAG4 to the tag for the GI of Protein field*/
	*pt4_YENA_UNIQUE,	/*this is a pointer of type TAG4 to the tag for a compound key that is unique*/

	/*SRCH: SeaRCH database*/
	*pt4_SRCH_GIP,		/*this is a pointer of type TAG4 to the field called GI of Protein*/
	*pt4_SRCH_NAME,		/*this is a pointer of type TAG4 to the field called NAME of protein*/
	*pt4_SRCH_CIN,		/*this is a pointer of type TAG4 to the field called NAME of protein for Case Insensitive searces*/
	*pt4_SRCH_UNIQUE,	/*this is a pointer of type TAG4 to the tag for a compound key that is unique*/

	/*RESULT: RESULT database*/
	*pt4_RESULT_GIP,	/*this is a pointer of type TAG4 to the field called GI of Protein*/
	*pt4_RESULT_NAME,	/*this is a pointer of type TAG4 to the field called NAME of protein*/
	*pt4_RESULT_PMID,	/*this is a pointer of type TAG4 to the field called PubMed IDentifier*/
	*pt4_RESULT_UNIQUE,	/*this is a pointer of type TAG4 to the tag for a compound key that is unique*/

	/*ISVM: Interaction SVM database*/
	*pt4_ISVM_PMID,	/*this is a pointer of type TAG4 to the field called PubMed IDentifier*/
	*pt4_ISVM_MUID,	/*this is a pointer of type TAG4 to the field called MedLine IDentifier*/

	/*CORE: REsults COunt*/
	*pt4_CORE_PMID,	/*this is a pointer of type TAG4 to the field called PubMed IDentifier*/
	*pt4_CORE_MUID,	/*this is a pointer of type TAG4 to the field called MedLine IDentifier*/
	*pt4_CORE_NUMN,	/*this is a pointer of type TAG4 to the field called NUMber of Names that found the result*/
	*pt4_CORE_NUMG,	/*this is a pointer of type TAG4 to the field called NUMber of Gi's that found the result*/
	
	/*REGI: REsults by GI*/
	*pt4_REGI_PMID,	/*this is a pointer of type TAG4 to the field called PubMed IDentifier*/
	*pt4_REGI_MUID,	/*this is a pointer of type TAG4 to the field called MedLine IDentifier*/
	*pt4_REGI_GIP,	/*this is a pointer of type TAG4 to the field called GI's of Proteins that found the result*/
	*pt4_REGI_UNIQUE,/*this is a pointer of type TAG4 to a unique compound key*/

	/*POINT: POtential INTeraction database*/
	*pt4_POINT_PMID,	/*this is a pointer of type TAG4 to the field called PubMed IDentifier */
	*pt4_POINT_MUID,	/*this is a pointer of type TAG4 to the field called Medine IDentifier */
	*pt4_POINT_GIA,		/*this is a pointer of type TAG4 to the field called GIs of protein A in interaction*/
	*pt4_POINT_GIB,		/*this is a pointer of type TAG4 to the field called GIs of protein B in interaction*/
	*pt4_POINT_PISCORE,	/*this is a pointer of type TAG4 to the field called Pairwise Interaction SCORE*/
	*pt4_POINT_PITRUTH,	/*this is a pointer of type TAG4 to the field called Pairwise Interaction TRUTH*/

	/*SUBMIT: SUBMITted potential interactions database*/
	*pt4_SUBMIT_PMID,	/*this is a pointer of type TAG4 to the field called PubMed IDentifier */
	*pt4_SUBMIT_MUID,	/*this is a pointer of type TAG4 to the field called Medine IDentifier */
	*pt4_SUBMIT_GIA,	/*this is a pointer of type TAG4 to the field called GIs of protein A in interaction*/
	*pt4_SUBMIT_GIB,	/*this is a pointer of type TAG4 to the field called GIs of protein B in interaction*/
	*pt4_SUBMIT_SGIA,	/*this is a pointer of type TAG4 to the field called Submitted GI of molecule A*/
	*pt4_SUBMIT_NAMA,	/*this is a pointer of type TAG4 to the field called NAMes of molecule A in interaction*/
	*pt4_SUBMIT_ADESC,	/*this is a pointer of type TAG4 to the field called DESCription of molecule A in interaction*/
	*pt4_SUBMIT_ATYPE,	/*this is a pointer of type TAG4 to the field called A: TYPE of molecule in interaction*/
	*pt4_SUBMIT_AOK,	/*this is a pointer of type TAG4 to the field called sgiA describes molecule A OK?*/
	*pt4_SUBMIT_AFROM,	/*this is a pointer of type TAG4 to the field called AFROM binding location*/
	*pt4_SUBMIT_ATO,	/*this is a pointer of type TAG4 to the field called ATO binding location*/
	*pt4_SUBMIT_SGIB,	/*this is a pointer of type TAG4 to the field called Submitted GI of molecule B*/
	*pt4_SUBMIT_NAMB,	/*this is a pointer of type TAG4 to the field called NAMes of molecule B in interaction*/
	*pt4_SUBMIT_BDESC,	/*this is a pointer of type TAG4 to the field called DESCription of molecule B in interaction*/
	*pt4_SUBMIT_BTYPE,	/*this is a pointer of type TAG4 to the field called B: TYPE of molecule in interaction*/
	*pt4_SUBMIT_BOK,	/*this is a pointer of type TAG4 to the field called sgiB describes molecule B OK?*/
	*pt4_SUBMIT_BFROM,	/*this is a pointer of type TAG4 to the field called BFROM binding location*/
	*pt4_SUBMIT_BTO,	/*this is a pointer of type TAG4 to the field called BTO binding location*/
	*pt4_SUBMIT_METHOD,	/*this is a pointer of type TAG4 to the field called METHOD used to show binding*/
	*pt4_SUBMIT_OPIN,	/*this is a pointer of type TAG4 to the field called OPinion (is this an interaction or not)*/
	*pt4_SUBMIT_FNAME,	/*this is a pointer of type TAG4 to the field called First NAME*/
	*pt4_SUBMIT_LNAME,	/*this is a pointer of type TAG4 to the field called Last NAME*/
	*pt4_SUBMIT_BUID,	/*this is a pointer of type TAG4 to the field called Bind User ID*/
	*pt4_SUBMIT_DATE,	/*this is a pointer of type TAG4 to the field called DATE submitted*/
	*pt4_SUBMIT_STATUS;	/*this is a pointer of type TAG4 to the field called STATUS*/



/***********************************************/
/* CodeBase FIELD4INFO and TAG4INFO structures */
/***********************************************/

/*ACC: Accession database*/
static FIELD4INFO f4iACC[]=
{
	{ "ACCESSION",	'C', 20, 0 }, /*RefSeq accession*//*field names must be <= 10 characters*/
	{ "GI",			'N', 20, 0 }, /*RefSeq GI in most recent version*/
	{ "DATABASE",	'C', 20, 0 }, /*DataBase (usually RefSeq)*/
	{ "LOCUS",	    'C', 40, 0 }, /*Official Gene Name is the LOCUS name*/
	{ "DEFINITION",	'M', 10, 0 }, /*Definition line*/
	{ "TAXID",		'N', 20, 0 }, /*NCBI taxon id*/
	{ "PPROD",      'M', 10, 0 }, /*Preferred product name*/
	{ "UPDATED",    'D',  8, 0 }, /*Date that accession entry was last updated*/
	{ 0,0,0,0 } 
};

static TAG4INFO t4iACC[]=
{
	{ "ACC_TAG", "ACCESSION", 0,                     r4unique,	0 }, 
	{ "GI_TAG",  "GI",        0,                     0,			0 },
	{ "RSQ_TAG", "DATABASE", "DATABASE <> 'REFSEQ'", 0,			0 },
    { 0, 0, 0, 0, 0 },
};


/*DB: Database database*/
static FIELD4INFO f4iDB[]=
{
	{ "GI_NUC",	'N', 20, 0 }, /*RefSeq GI of encoding NUCleotide*/
	{ "FROM",	'N', 20, 0 }, /*position FROM which coding region starts*/	
	{ "TO",		'N', 20, 0 }, /*position TO which coding region extends*/
	{ "GI_PROT",'N', 20, 0 }, /*RefSeq GI of encoded protein*/
	{ "DBNAME",	'C', 20, 0 }, /*NAME of the external DataBase*/
	{ "IDINT",	'N', 20, 0 }, /*IDentifier (INTeger) in external database*/
	{ "IDSTR",	'C', 20, 0 }, /*IDentifier (STRing) in external database*/
	{ 0,0,0,0 } 
};


static TAG4INFO t4iDB[]=
{
	{ "GIN_TAG",	"GI_NUC", 0, 0, 0 },
    { "GIP_TAG",	"GI_PROT", 0, 0, 0 },
	{ "IDINT_TAG",	"IDINT", 0, 0, 0 },
	{ "IDSTR_TAG",	"IDSTR", 0, 0, 0 },
	{ "UNIQUE_TAG", "STR(GI_NUC, 20, 0) + STR(GI_PROT, 20, 0) + DBNAME + STR(IDINT, 20, 0) + IDSTR", 0, r4unique, 0},
	{ 0, 0, 0, 0, 0 },
};



/*ONAME: Other NAME database*/
static FIELD4INFO f4iONAME[]=
{
	{ "ACCESSION",  'C', 20, 0 }, /*RefSeq accession*/
	{ "GI",			'N', 20, 0 }, /*RefSeq GI in most recent version*/	
	{ "ONAME",		'M', 10, 0 }, /*Other NAMEs  for the same protein*/
	{ 0,0,0,0 } 
};


static TAG4INFO t4iONAME[]=
{
	{ "ACC_TAG", "ACCESSION", 0, 0, 0 }, 
    { "GI_TAG",  "GI",        0, 0, 0 },
	{ 0, 0, 0, 0, 0 },
};


/*OGI: Other GI database*/
static FIELD4INFO f4iOGI[]=
{
	{ "ACCESSION", 'C', 20, 0 }, /*RefSeq accession*/
	{ "GI",		   'N', 20, 0 }, /*RefSeq GI in most recent version*/	
	{ "OGI",       'N', 20, 0 }, /*Other GI's for the same protein*/
	{ 0,0,0,0 }  
};


static TAG4INFO t4iOGI[]=
{
	{ "ACC_TAG", "ACCESSION", 0, 0, 0 }, 
	{ "GI_TAG",  "GI",        0, 0, 0 },
    { 0, 0, 0, 0, 0 },
};


/*CDR: CoDing Region database*/
static FIELD4INFO f4iCDR[]=
{
	{ "GI_NUC",	'N', 20, 0 }, /*RefSeq GI of encoding nucleotide*/
	{ "FROM",	'N', 20, 0 }, /*position FROM which coding region starts*/	
	{ "TO",		'N', 20, 0 }, /*position TO which coding region extends*/
	{ "GI_PROT",'N', 20, 0 }, /*RefSeq GI of encoded protein*/
	{ 0,0,0,0 }  
};


static TAG4INFO t4iCDR[]=
{
	{ "GIN_TAG", "GI_NUC",  0, 0, 0 }, 
	{ "GIP_TAG", "GI_PROT", 0, 0, 0 },
    { 0, 0, 0, 0, 0 },
};


/*LOCUS: LOCUS name database*/
static FIELD4INFO f4iLOCUS[]=
{
	{ "GI_NUC",	'N', 20, 0 }, /*RefSeq GI of encoding nucleotide*/
	{ "FROM",	'N', 20, 0 }, /*position FROM which coding region starts*/	
	{ "TO",		'N', 20, 0 }, /*position TO which coding region extends*/
	{ "NAME",	'C', 40, 0 }, /*NAME of locus*/
	{ "GI_PROT",'N', 20, 0 }, /*RefSeq GI of encoded protein*/
	{ 0,0,0,0 }  
};


static TAG4INFO t4iLOCUS[]=
{
	{ "GIN_TAG", "GI_NUC",  0, 0, 0 }, 
	{ "GIP_TAG", "GI_PROT", 0, 0, 0 },
	{ "UNIQUE_TAG", "STR(GI_NUC, 20, 0) + STR(FROM, 20, 0) + NAME", 0, r4unique, 0},
    { 0, 0, 0, 0, 0 },
};



/*LOSYN: LOcus SYNonym database*/
static FIELD4INFO f4iLOSYN[]=
{
	{ "GI_NUC",	'N', 20, 0 }, /*RefSeq GI of encoding nucleotide*/
	{ "FROM",	'N', 20, 0 }, /*position FROM which coding region starts*/	
	{ "TO",		'N', 20, 0 }, /*position TO which coding region extends*/
	{ "SYN",	'C', 40, 0 }, /*SYN of locus*/
	{ "GI_PROT",'N', 20, 0 }, /*RefSeq GI of encoded protein*/
	{ 0,0,0,0 }  
};


static TAG4INFO t4iLOSYN[]=
{
	{ "GIN_TAG", "GI_NUC",  0, 0, 0 }, 
	{ "GIP_TAG", "GI_PROT", 0, 0, 0 },
	{ "UNIQUE_TAG", "STR(GI_NUC, 20, 0) + STR(FROM, 20, 0) + SYN", 0, r4unique, 0},
    { 0, 0, 0, 0, 0 },
};

/*YENA: YEast NAme database*/
static FIELD4INFO f4iYENA[]=
{
	{ "GI_PROT",'N', 20, 0 }, /*RefSeq GI of encoded protein*/
	{ "NAME",	'C', 40, 0 }, /*NAME of protein*/
	{ 0,0,0,0 }  
};


static TAG4INFO t4iYENA[]=
{
	{ "GIP_TAG", "GI_PROT", 0, 0, 0 },
	{ "UNIQUE_TAG", "STR(GI_PROT, 20, 0) + NAME", 0, r4unique, 0},
    { 0, 0, 0, 0, 0 },
};


/*SRCH: SeaRCH database*/
static FIELD4INFO f4iSRCH[]=
{
	{ "GI_PROT",		'N', 20, 0 }, /*RefSeq GI of encoded PROTein*/
	{ "NAME",			'C', 40, 0 }, /*NAME of protein*/
	{ "SOURCE",			'N', 20, 0 }, /*SOURCE of name of protein*/
	{ "TAXID",			'N', 20, 0 }, /*TAXon id for source orgonism of encoded protein*/
	{ "SEARCHED",		'D',  8, 0 }, /*Date that search was last performed*/
	{ "STATUS",			'N',  1, 0 }, /*0- not approved, 1- approved, 2- search performed*/
	{ "NAMERED",		'N',  3, 0 }, /*How many times does the name appear for this organism*/
	{ "RESULTS",		'N', 20, 0 }, /*Number of search results in results database*/
	{ 0,0,0,0 }  
};


static TAG4INFO t4iSRCH[]=
{
	{ "GIP_TAG",	"GI_PROT",		0, 0, 0 },
	{ "NAME_TAG",	"NAME",			0, 0, 0 },
	{ "CIN_TAG", "UPPER(NAME)",		0, 0, 0 },/*for case insensitive name searches*/
	{ "UNIQUE_TAG",	"STR(GI_PROT, 20, 0) + NAME", 0, r4unique, 0},
    { 0, 0, 0, 0, 0 },
};


/*RESULT: RESULT database*/
static FIELD4INFO f4iRESULT[]=
{
	{ "GI_PROT",'N', 20, 0 }, /*RefSeq GI of encoded PROTein*/
	{ "NAME",	'C', 40, 0 }, /*NAME of protein*/
	{ "PMID",	'N', 20, 0 }, /*PubMed IDentifier*/
	{ 0,0,0,0 }  
};


static TAG4INFO t4iRESULT[]=
{
	{ "GIP_TAG",   "GI_PROT", 0, 0, 0 },
	{ "NAME_TAG",  "NAME", 0, 0, 0 },
	{ "PMID_TAG",  "PMID", 0, 0, 0 },
	{ "UNIQUE_TAG","STR(GI_PROT, 20, 0) + NAME + STR(PMID, 20, 0)", 0, r4unique, 0},
    { 0, 0, 0, 0, 0 },
};


/*ISVM: Interaction SVM database*/
static FIELD4INFO f4iISVM[]=
{
	{ "PMID",	'N', 20, 0 }, /*PubMed IDentifier*/
	{ "MUID",	'N', 20, 0 }, /*MedLine Unique IDentifier*/
	{ "ISCORE",	'N', 20, 10 }, /*Interaction SCORE from SVM*/
	{ "ISCORE2",'N', 20, 10 }, /*Interaction SCORE2: second score from SVM or other system*/
	{ "OPNO",	'N', 20, 0 }, /*OPinion: NO this is not an interaction paper*/
	{ "OPYES",	'N', 20, 0 }, /*OPinion: YES this is an interaction paper*/
	{ "ITRUTH",	'N', 20,  0 }, /*Interation TRUTH (1 = interaction paper, 0 = undecided, -1 = not)*/
	{ 0,0,0,0 }  
};


static TAG4INFO t4iISVM[]=
{
	{ "PMID_TAG",   "PMID", 0, r4unique, 0 },
	{ "MUID_TAG",   "MUID", 0, 0, 0 },
    { 0, 0, 0, 0, 0 },
};


/*CORE: REsults COunt database*/
static FIELD4INFO f4iCORE[]=
{
	{ "PMID",	'N', 20, 0 }, /*PubMed IDentifier*/
	{ "MUID",	'N', 20, 0 }, /*MedLine Unique IDentifier*/
	{ "NUMN",	'N', 20, 0 }, /*NUMber of Names that found the result*/
	{ "NUMG",	'N', 20, 0 }, /*NUMber of Gi's that found the result*/
	{ 0,0,0,0 }  
};


static TAG4INFO t4iCORE[]=
{
	{ "PMID_TAG",   "PMID", 0, r4unique, 0 },
	{ "MUID_TAG",   "MUID", 0, 0, 0 },
	{ "NUMN_TAG",   "NUMN", 0, 0, 0 },
	{ "NUMG_TAG",   "NUMG", 0, 0, 0 },
    { 0, 0, 0, 0, 0 },
};


/*REGI: REsults by GI database*/
static FIELD4INFO f4iREGI[]=
{
	{ "PMID",	'N', 20, 0 }, /*PubMed IDentifier*/
	{ "MUID",	'N', 20, 0 }, /*MedLine Unique IDentifier*/
	{ "GIP",	'N', 20, 0 }, /*GI of Proteins that found the result*/
	{ 0,0,0,0 }  
};


static TAG4INFO t4iREGI[]=
{
	{ "PMID_TAG",   "PMID", 0, 0, 0 },
	{ "MUID_TAG",   "MUID", 0, 0, 0 },
	{ "GIP_TAG",    "GIP",  0, 0, 0 },
	{ "UNIQUE_TAG","STR(PMID, 20, 0) + STR(GIP, 20, 0)", 0, r4unique, 0},
    { 0, 0, 0, 0, 0 },
};



/*POINT: POtential INTeraction database*/
static FIELD4INFO f4iPOINT[]=
{
	{ "PMID",	 'N', 20, 0 }, /*PubMed IDentifier*/
	{ "MUID",	 'N', 20, 0 }, /*MedLine Unique IDentifier*/
	{ "GIA",	 'N', 20, 0 }, /*GI of protein A in putative interaction*/
	{ "NAMA",	 'C', 60, 0 }, /*NAMes of protein A in putative interaction*/
	{ "ATYPE",	 'N', 20, 0 }, /*A: molecule type in interaction (0- undefined, 1- DNA, 2- protein)*/
	{ "GIB",	 'N', 20, 0 }, /*GI of protein B in putative interaction*/
	{ "NAMB",	 'C', 60, 0 }, /*NAMes of protein B in putative interaction*/
	{ "BTYPE",	 'N', 20, 0 }, /*B: molecule type in interaction (0- undefined, 1- DNA, 2- protein)*/
	{ "OPNO",	 'N', 20, 0 }, /*OPinion count: NO this is not an interaction*/
	{ "OPYES",	 'N', 20, 0 }, /*OPinion count: YES this is an interaction*/
	{ "PISCORE", 'N', 20, 0 }, /*Pairwise Interaction SCORE- protein::protein interaction*/
	{ "PISCORE2",'N', 20, 0 }, /*Pairwise Interaction SCORE- protein::DNA interaction*/
	{ "PITRUTH", 'N', 20, 0 }, /*Pairwise Interaction TRUTH */
	{ 0,0,0,0 }  
};


static TAG4INFO t4iPOINT[]=
{
	{ "PMID_TAG",   "PMID", 0, 0, 0 },
	{ "MUID_TAG",   "MUID", 0, 0, 0 },
	{ "GIA_TAG",    "GIA",  0, 0, 0 },
	{ "GIB_TAG",    "GIB",  0, 0, 0 },
	{ "PISCORE_TAG","PISCORE",  0, 0, 0 },
	{ "PITRUTH_TAG","PITRUTH",  0, 0, 0 },
    { 0, 0, 0, 0, 0 },
};


/*SUBMIT: SUBMITted potential interactions database*/
static FIELD4INFO f4iSUBMIT[]=
{
	{ "PMID",	'N', 20, 0}, 	/*PubMed IDentifier */
	{ "MUID",   'N', 20, 0}, 	/*Medine IDentifier */
	{ "GIA",	'N', 20, 0}, 	/*GIs of protein A in interaction*/
	{ "GIB",	'N', 20, 0}, 	/*GIs of protein B in interaction*/
	{ "SGIA",   'N', 20, 0}, 	/*Submitted GI of molecule A*/
	{ "NAMA",   'C', 60, 0}, 	/*NAMes of molecule A in interaction*/
	{ "ADESC",  'M', 10, 0}, 	/*DESCription of molecule A in interaction*/
	{ "ATYPE",  'N', 20, 0}, 	/*A: TYPE of molecule in interaction*/
	{ "AOK",	'N', 20, 0}, 	/*sgiA describes molecule A OK?*/
	{ "AFROM",  'N', 20, 0}, 	/*AFROM binding location*/
	{ "ATO",	'N', 20, 0}, 	/*ATO binding location*/
	{ "SGIB",   'N', 20, 0}, 	/*Submitted GI of molecule B*/
	{ "NAMB",   'C', 60, 0}, 	/*NAMes of molecule B in interaction*/
	{ "BDESC",  'M', 10, 0}, 	/*DESCription of molecule B in interaction*/
	{ "BTYPE",  'N', 20, 0}, 	/*B: TYPE of molecule in interaction*/
	{ "BOK",	'N', 20, 0}, 	/*sgiB describes molecule B OK?*/
	{ "BFROM",  'N', 20, 0}, 	/*BFROM binding location*/
	{ "BTO",	'N', 20, 0}, 	/*BTO binding location*/
	{ "METHOD",	'N', 20, 0}, 	/*METHOD used to show interaction*/
	{ "OPIN",	'N', 20, 0}, 	/*OPINion (is this an interaction or not)*/
	{ "FNAME",  'C', 20, 0}, 	/*First NAME*/
	{ "LNAME",  'C', 20, 0}, 	/*Last NAME*/
	{ "BUID",   'N', 20, 0}, 	/*Bind User ID*/
	{ "DATE",   'D', 20, 0}, 	/*DATE submitted*/
	{ "STATUS", 'N', 20, 0},	/*STATUS of SUBMIT record*/
	{ 0,0,0,0 }  
};


static TAG4INFO t4iSUBMIT[]=
{
	{ "PMID_TAG",  "PMID",	 0, 0, 0}, 	/*PubMed IDentifier */
	{ "MUID_TAG",  "MUID",   0, 0, 0}, 	/*Medine IDentifier */
	{ "GIA_TAG",   "GIA",	 0, 0, 0}, 	/*GIs of protein A in interaction*/
	{ "GIB_TAG",   "GIB",	 0, 0, 0}, 	/*GIs of protein B in interaction*/
	{ "SGIA_TAG",  "SGIA",   0, 0, 0}, 	/*Submitted GI of molecule A*/
	{ "NAMA_TAG",  "NAMA",   0, 0, 0}, 	/*NAMes of molecule A in interaction*/
	{ "SGIB_TAG",  "SGIB",   0, 0, 0}, 	/*Submitted GI of molecule B*/
	{ "NAMB_TAG",  "NAMB",   0, 0, 0}, 	/*NAMes of molecule B in interaction*/
	{ "FNAME_TAG", "FNAME",  0, 0, 0}, 	/*First NAME*/
	{ "LNAME_TAG", "LNAME",  0, 0, 0}, 	/*Last NAME*/
	{ "BUID_TAG",  "BUID",   0, 0, 0}, 	/*Bind User ID*/
	{ "DATE_TAG",  "DATE",   0, 0, 0}, 	/*DATE submitted*/
	{ "STATUS_TAG","STATUS", 0, 0, 0},	/*STATUS of SUBMIT record*/
    { 0, 0, 0, 0, 0 },
};





/*************************************************/
/* function protoypes							 */
/*************************************************/
/*prebind_db.c*/
Boolean OpenPreBIND(void);
Boolean InitCodeBaseForPreBIND(void);

Boolean OpenACC(CharPtr, Boolean);
Boolean OpenDBDB(CharPtr, Boolean);
Boolean OpenONAME(CharPtr, Boolean);
Boolean OpenOGI(CharPtr, Boolean);
Boolean OpenCDR(CharPtr, Boolean);
Boolean OpenLOCUS(CharPtr, Boolean);
Boolean OpenLOSYN(CharPtr, Boolean);
Boolean OpenYENA(CharPtr, Boolean);
Boolean OpenSRCH(CharPtr, Boolean);
Boolean OpenRESULT(CharPtr, Boolean);
Boolean OpenISVM(CharPtr, Boolean);
Boolean OpenCORE(CharPtr, Boolean);
Boolean OpenREGI(CharPtr, Boolean);
Boolean OpenPOINT(CharPtr, Boolean);
Boolean OpenSUBMIT(CharPtr, Boolean);

Boolean OpenACCNoidx(CharPtr);
Boolean OpenDBDBNoidx(CharPtr);
Boolean OpenONAMENoidx(CharPtr;
Boolean OpenOGINoidx(CharPtr);
Boolean OpenCDRNoidx(CharPtr);
Boolean OpenLOCUSNoidx(CharPtr);
Boolean OpenLOSYNNoidx(CharPtr);
Boolean OpenYENANoidx(CharPtr);
Boolean OpenSRCHNoidx(CharPtr);
Boolean OpenRESULTNoidx(CharPtr);
Boolean OpenISVMNoidx(CharPtr);
Boolean OpenCORENoidx(CharPtr);
Boolean OpenREGINoidx(CharPtr);
Boolean OpenPOINTNoidx(CharPtr);
Boolean OpenSUBMITNoidx(CharPtr);


void AssignFieldsREGI(void)
void AssignFieldsDBDB(void);
void AssignFieldsONAME(void;
void AssignFieldsOGI(void);
void AssignFieldsCDR(void);
void AssignFieldsLOCUS(void);
void AssignFieldsLOSYN(void);
void AssignFieldsYENA(void);
void AssignFieldsSRCH(void);
void AssignFieldsRESULT(void);
void AssignFieldsISVM(void);
void AssignFieldsCORE(void);
void AssignFieldsREGI(void);
void AssignFieldsPOINT(void);
void AssignFieldsSUBMIT(void);

void AssignTagsREGI(void)
void AssignTagsDBDB(void);
void AssignTagsONAME(void;
void AssignTagsOGI(void);
void AssignTagsCDR(void);
void AssignTagsLOCUS(void);
void AssignTagsLOSYN(void);
void AssignTagsYENA(void);
void AssignTagsSRCH(void);
void AssignTagsRESULT(void);
void AssignTagsISVM(void);
void AssignTagsCORE(void);
void AssignTagsREGI(void);
void AssignTagsPOINT(void);
void AssignTagsSUBMIT(void);

void GetFullPath(CharPtr pathdatab, CharPtr name)

Int4			GetTaxidbyGI(Int4 gi);
PREBINDAccPtr	GetAccessionRecordbyGI(Int4 gi);

#ifdef __cplusplus
}
#endif

#endif  /* _PREBIND_DB_ */


/*
$Log: prebind_db.h,v $
Revision 1.3  2004/03/18 14:50:33  kzheng
added functions for opening tables with no index; added reindexing of tables

Revision 1.2  2003/02/26 22:56:40  janicek
Reorganized code for better modularity

Revision 1.1.1.1  2002/07/17 19:36:35  janicek
initial import

*/



