/*$Id: prebind_parser.h,v 1.3 2004/01/16 15:08:45 eburgess Exp $*/
/*
-- ***************************************************************************
--
--  Pre Biomolecular Interaction Network Database (PreBIND)
--
--  File: prebind_parser.h
--  Description: header for prebind parser specific functions and variables
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



#ifndef _PREBIND_PARSER_
#define _PREBIND_PARSER_



#ifndef _PREBIND_DB_
#include "prebind_db.h"
#endif  


#ifndef _PREBIND_COMMON_
#include "prebind_common.h"
#endif


#ifndef _PREBIND_LIB_
#include "prebind_lib.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define LOGGING "pb_parser_log.txt" /*name of file where errors are logged to*/

/* SeqFeat defines */
#define ANNOT_TABLE 1
#define ANNOT_ALIGN 2
#define ANNOT_GRAPH 3
#define ANNOT_IDS   4
#define ANNOT_LOCS  5

/* Strand Defines */
#define UNKNOWN     0
#define PLUS        1
#define MINUS       2

/* Protein Defines */

#define NOTSET     0
#define PREPROTEIN 1
#define MATURE     2
#define SIGNALPP   3
#define TRANSITPP  4

/*global declarations and initializations*/
Int4	number_bioseqs =	0;		/*counts the number of bioseqs parsed*/
Int4	number_bioseqsets = 0;		/*counts the number of bioseq-sets parsed*/
CharPtr	name_ptr =			NULL;	/*points to a title of a Bioseq-set*/
Uint1	bss_class =			0;		/*describes the class of a Bioseq-set*/
Int4	number_proteins =	0;		/*describes the number of proteins parsed*/
Int4	cdr_flag =			0;		/*a flag to look for Coding Region Data in the Bioseq*/

/*function protypes*/
/*parsing*/
Boolean VisitBioseqs(SeqEntryPtr, Int4, BioseqSetPtr, SeqDescrPtr);
Boolean ParseBioseq(BioseqPtr, BioseqSetPtr, SeqDescrPtr);
Int4	PreBINDGetGI(BioseqPtr);
CharPtr	GetAccession(BioseqPtr);
CharPtr GetProtName(BioseqPtr);
CharPtr GetTitle(BioseqPtr, SeqDescrPtr);
Int4	GetTAXONID(SeqDescrPtr);
Int4	ParseCDRInfo(BioseqSetPtr, BioseqPtr);
Int4	ParseLocusInfo(BioseqPtr, BioseqSetPtr);

/* Elizabeth */
Int4 GetGIFromProductLocation( SeqLocPtr psl );
Int4 GetSourceLocationInfo( SeqLocPtr psl, Int4Ptr pgi, Int4Ptr pfloc, 
                              Int4Ptr ptloc, Boolean transSpliced );
Int4 GetLocationInfoFromSeqInt( SeqLocPtr psl, Int4Ptr pgi,
                                Int4Ptr pfloc, Int4Ptr ptloc );
Int4 GetLocationInfoFromSeqMix( SeqLocPtr psl, Int4Ptr pgi, Int4Ptr pfloc,
                                 Int4Ptr ptloc, Boolean transSpliced );
Int4 ParseCDRInfoFromFeatureTable(  SeqFeatPtr psf, Int4 gi );  
Int4 ParseLocusInfoFromFeatureTable(  SeqFeatPtr psf, Int4 gi  );  
void GetDatabaseXRefs( ValNodePtr pdbvn, Int4 gi, Int4 floc, Int4 tloc );
  /*    CharPtr    GetNameFromProtRef( ProtRefPtr pProtRef );*/
Int4       GetGIFromSeqID( SeqIdPtr psi );
Int4 GetLocationInfoFromPackedInt( SeqLocPtr psl, Int4Ptr pgi, Int4Ptr pfloc,
                                   Int4Ptr ptloc, Boolean transSpliced );

/*'write to' and build datafile functions*/
Int4	WriteACC(Int4, CharPtr, CharPtr, CharPtr, Int4, CharPtr);
Int4	WriteCDR(Int4, Int4, Int4, Int4);
Int4	WriteLOCUS(Int4, Int4, Int4, CharPtr);
Int4	WriteLOSYN(Int4, Int4, Int4, CharPtr);
Int4	WriteDB(Int4, Int4, Int4, Int4, CharPtr, Int4 ,CharPtr);
Int4	WriteSRCH(Int4 gip, CharPtr name, Int4 source, Int4 taxid, CharPtr date);

Int4	BuildLOCUS(void);
Int4	BuildLOSYN(void);
Int4	BuildDB(void);
Int4	BuildACC(void);
Int4	BuildSRCH(void);


#ifdef __cplusplus
}
#endif

#endif  /*_PREBIND_PARSER_*/

/*
$Log: prebind_parser.h,v $
Revision 1.3  2004/01/16 15:08:45  eburgess
Added added functions to simplify prebind_parser.c and to extend its
functionality.

Revision 1.2  2002/07/30 15:23:10  janicek
changed GetGI to PreBINDGetGI

Revision 1.1.1.1  2002/07/17 19:36:35  janicek
initial import

*/


