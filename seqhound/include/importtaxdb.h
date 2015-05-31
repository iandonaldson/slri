/* $Id: importtaxdb.h,v 1.4 2004/07/09 19:23:49 vgu Exp $
-- ***************************************************************************
  Copyright (C) 2000 Mount Sinai Hospital (MSH)

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
           Gary Bader and Christopher W.V. Hogue hogue@mshri.on.ca


  REVISION/CONTRIBUTION NOTES:

  June 1, 2000  Original MSH Revision.
                Hogue Lab - University of Toronto Biochemistry
                Department and the Samuel Lunenfeld Research
                Institute, Mount Sinai Hospital
                http://bioinfo.mshri.on.ca  hogue@mshri.on.ca
                ftp://ftp.mshri.on.ca/


  PUBLICATION TO CITE:


  DESCRIPTION
  taxonomy database part of SeqHound


******************************************************************************************/



#ifndef _IMPORTTAXDB_
#define _IMPORTTAXDB_

#ifdef __cplusplus
extern "C" {
#endif

#ifndef ODBC 
#include <d4all.h>
#endif

#include <asn.h>
#include <objslritax.h>
#include <slri_asn.h>
#ifndef ODBC 
#include <slri_cb.h>
#endif
#include <slri_misc.h>
#include <slri_rbtree.h>
#include <taxdb_cons.h>
#include <taxdb_db.h>
#include <taxdb_enum.h>

/*DEFINES*/

/*Define this to parse taxdb in memory.
  Much faster, but takes up about 4 times as much memory as the size of the NODESFILE
  and takes about 1/10th the time to parse*/
#define PARSE_IN_MEMORY

#define NODESFILE "nodes.dmp"
#define NAMESFILE "names.dmp"
#define DIVISIONFILE "division.dmp"
#define GENCODEFILE "gencode.dmp"
#define DELNODESFILE "delnodes.dmp"
#define MERGEDNODESFILE "merged.dmp"
#define FIELDEND "\t|\t"
#define LINEEND "\t|\n"
#define LOG
#define LOG_FILE_NAME "importtaxdb_log.txt"


/*****************/
/*Parse Functions*/
/*****************/

/* Read an entire TaxDB record (up until the given end of record string) */
extern Int2 BSGetTaxRecordFromFile(ByteStorePtr bsp, FILE *fp, CharPtr eorString);

/*Add a character to a string and shift existing characters over by one - leftmost char gets bumped*/
extern Int2 ShiftCharsInBuffer(CharPtr buffer, Int2 ch, Int4 bufferLength);

/*Parse a field (with a given field delimiter) from a bytestore
Note: allocates memory - caller responsible for freeing returned string*/
extern CharPtr Parse_StringFromBSP(ByteStorePtr bspSource, CharPtr fieldDelimStr, Boolean *error);

/*Parse an integer from a field in a bytestore (with a given field delimiter)
Returns INT4_MIN and sets the error flag in case of error*/
extern Int4 Parse_IntegerFromBSP(ByteStorePtr bspSource, CharPtr fieldDelimStr, Boolean *error);

/*Parse a record in a bytestore from the NODESFILE*/
extern SLRITaxonPtr Parse_TaxDBNodeRecord(ByteStorePtr bsp);

/*Parse a record in a bytestore from the NAMESFILE*/
extern SLRITaxonNamePtr Parse_TaxDBNameRecord(ByteStorePtr bsp, Int4Ptr taxIdRet);

/*Parse a record in a bytestore from the DIVISIONFILE*/
extern SLRITaxonDivPtr Parse_TaxDBDivRecord(ByteStorePtr bsp);

/*Parse a record in a bytestore from the GENCODEFILE*/
extern SLRITaxonGencodePtr Parse_TaxDBGenCodeRecord(ByteStorePtr bsp);

#ifdef PARSE_IN_MEMORY
/*Red black tree functions*/

/*data free function for taxID tree - frees the ValNode data*/
extern Pointer TaxIDTree_DataFree(Pointer data);

/*write to disk and free*/
extern RedBlackTreeNodePtr TaxIDTree_WriteAndFree(RedBlackTreeNodePtr rbtnpRoot, RedBlackTreeFunctionPackagePtr funPack);
#endif /*PARSE_IN_MEMORY*/


#ifdef __cplusplus
}
#endif
#endif		/* _IMPORTTAXDB_ */


/*
Log
*/

