/*$Id: rpsdb.h,v 1.3 2004/01/12 17:46:33 mmatan Exp $*/
/*
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


  PROGRAM: RPSDB- Fills RPS database that computes domain
  		annotation for proteins.

  AUTHORS: Doron Betel betel@mshri.on.ca
            and Christopher W.V. Hogue hogue@mshri.on.ca


  REVISION/CONTRIBUTION NOTES:

  March 1, 2001  Original MSH Revision.
                Hogue Lab - University of Toronto Biochemistry
                Department and the Samuel Lunenfeld Research
                Institute, Mount Sinai Hospital
                http://bioinfo.mshri.on.ca  hogue@mshri.on.ca
                ftp://ftp.mshri.on.ca/


  PUBLICATION TO CITE:
*/
#ifndef _RPSDB_H_
#define _RPSDB_H_

#include <slri_misc.h>
#include <blast.h>
#include <alignmgr.h>
#include <cddutil.h>
#include <objcdd.h>
#include <objslristruc.h>
#include <seqhound.h>

#ifdef __cplusplus
extern "C" {
#endif

#define CDD_DBNAME "All"
#define NUM_THREADS 1
#define BLOSUM_FNAME "BLOSUM62"
#define CDD_EXT ".acd"
#define GI_CHOICE 3

/*RPS-BLAST options taken from rpsutil.h*/
typedef struct _rps_blast_options {
	Boolean query_is_protein;
	CharPtr rps_database;
	BLAST_OptionsBlkPtr options; /*Structure for the blast options */
	Int4 number_of_descriptions, number_of_alignments;
	Boolean html;
	Boolean believe_query;
	Uint4 align_options, print_options;
	Boolean is_xml_output;
	Int4 num_threads;
	/* These parameters are for formating convinience only */
	ReadDBFILEPtr rdfp;         /* Handle of the sequence database */
	FILE *outfp;                /* Output file opened descriptor */
	CharPtr out_filename;       /* Output filename */
	Boolean is_tabular;
} RPSBlastOptions, PNTR RPSBlastOptionsPtr;

/* Aron Bauer's structure used to save CDD hits. */
typedef struct _cdd_hit {
  CharPtr             CDDid;
  CharPtr             ShortName;
  CharPtr             Definition;
  Int4                start;
  Int4                stop;
  Int4                score;
  Nlm_FloatHi         evalue;
  Nlm_FloatHi         bit_score;
  struct _cdd_hit PNTR next;
} CddHit, PNTR CddHitPtr;

typedef enum _RunningMode{
	eFASTA=1,
	eSHoundRedund=2
} RunningMode;

/*function pointers that are called by RPSBlastSearchMT*/
typedef SeqEntryPtr (LIBCALLBACK *RPSReadBSPCallback)(SeqLocPtr PNTR slp,VoidPtr user_data);
typedef Boolean (LIBCALLBACK *RPSHandleResultsCallback)
	(BioseqPtr bsp, RPSBlastOptionsPtr rpsbop, SeqAlignPtr sap,
		ValNodePtr other_returns, ValNodePtr error_returns, VoidPtr user_data);

/* Definitions of multi-threaded batch RPS Blast search */
Boolean RPSBlastSearchMT(RPSBlastOptionsPtr rpsbop, RPSReadBSPCallback bsp_callback,
		VoidPtr bsp_user_data, RPSHandleResultsCallback print_callback,
			VoidPtr print_user_data);

/*Creates RPS-BLAST options and calls RPSBlastSearchMT*/
SLRI_ERR RPSDatabaseBlast(ValNodePtr gi_list, Int2 numpass, RunningMode mode);


/*Callback function when running rpsdb using FASTA index table (a la - formatdb style)
* Same deal as RPSDBSHoundRedundResultsCallback except for minor changes.
* Called (internally) by RPS-BLAST to process the resutls
* data is the result structure that needs to be filled up
*/
static Boolean LIBCALLBACK RPSDBFastaResultsCallback(BioseqPtr query_bsp,RPSBlastOptionsPtr rpsbop,SeqAlignPtr seqalign,
		ValNodePtr other_returns,ValNodePtr error_returns,VoidPtr data);

/*Callback function when running rpsdb using SeqHound Redund table
* Called (internally) by RPS-BLAST to process the resutls
* data is the result structure that needs to be filled up
*/
static Boolean LIBCALLBACK RPSDBSHoundRedundResultsCallback(BioseqPtr query_bsp,RPSBlastOptionsPtr rpsbop,SeqAlignPtr seqalign,
		ValNodePtr other_returns,ValNodePtr error_returns,VoidPtr data);
/*********************
* Print results struct
*********************/
void PrintResults(StRpsdbPtr results);

/********************************************
* Count the number of seqAligns in a list (returned) and count the number
* of distinct target sequences represented (passed back in numSequences);
* Important assumption: All SeqAligns with  the same target sequence are
* consecutive in the list
* ONLY works for lists of Seqaligns containing DenseSegs!!!
********************************************/
Int4 LIBCALL SLRICddCountSeqAligns(SeqAlignPtr listOfSeqAligns, Int4 * numSequences);

/***********************************************
* Commit results list to database RPSdb table
***********************************************/
Boolean AddResultsToTable(StRpsdbPtr head, Int4 counter);

/*****************************************
*Delete the list of GIs from the table
*****************************************/
Boolean DeleteFromTable(ValNodePtr delete);

/******************************
* Check generated tables
******************************/
void CheckTable(void);

/********************
* free results list
********************/
void FreeResults(StRpsdbPtr head);

/*Get GI from SeqId*/
Int4 GetGIFromSeqId(SeqIdPtr sip);

/*CodeBase functions*/
  /*Boolean LIBCALL InitCodeBase(void);*/
    /*Boolean LIBCALL CloseCodeBase(void);*/
#ifdef __cplusplus
}
#endif
#endif

/*
$Log: rpsdb.h,v $
Revision 1.3  2004/01/12 17:46:33  mmatan
Comment out function prototype declarations for InitCodeBase and CloseCodeBase
which have been renamed to allow ODBC builds.  This is being checked in for Zhe.

Revision 1.2  2003/12/02 18:53:17  dbetel
Clean up and addition of CodeBase Init&Close

Revision 1.1.1.5  2002/10/14 08:34:28  root
Automated import from Sourceforge.net

Revision 1.7  2002/10/13 22:56:22  betel
Added delete and update functionality

Revision 1.6  2002/10/10 15:10:42  betel
Added CddHits struct for new results callback

Revision 1.5  2002/09/18 02:11:05  gbader
Initial testing on windows.  Fixed bugs.

Revision 1.4  2002/09/18 01:03:47  gbader
Fixed compiler warnings and compile problems on Windows.

Revision 1.3  2002/09/17 21:02:10  betel
Added FASTA input a la BLAST (not tested)

Revision 1.2  2002/07/09 21:52:59  betel
Changed RPS-BLAST callback functions

Revision 1.1  2002/03/12 21:12:17  betel
Moved all rpsdb relevant files to rps dir

Revision 1.6  2002/03/06 00:06:29  betel
Cdd path read from config file

Revision 1.5  2002/03/01 21:49:27  betel
Fixed formatting and added cdd path

Revision 1.4  2002/02/27 18:43:33  betel
Added functions and fixes

Revision 1.3  2002/02/25 23:28:44  betel
added more parsing fucntions

Revision 1.2  2002/02/22 00:03:17  betel
Change of license. Rpsdb still not tested.

Revision 1.1  2002/02/21 23:29:30  betel
Initial import of RPS-BLAST parser. Not Tested ls

*/

