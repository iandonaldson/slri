/*$Id: rpsdb_db.h,v 1.4 2004/12/14 19:49:05 zwang Exp $*/
/*
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


  PROGRAM:  Rps Table (domains) codebase header


  AUTHORS:     Doron Betel          betel@mshri.on.ca
            and Christopher W.V. Hogue hogue@mshri.on.ca


  REVISION/CONTRIBUTION NOTES:

  March 1, 2001  Original MSH Revision.
                Hogue Lab - University of Toronto Biochemistry
                Department and the Samuel Lunenfeld Research
                Institute, Mount Sinai Hospital
                http://bioinfo.mshri.on.ca  hogue@mshri.on.ca
                ftp://ftp.mshri.on.ca/


  PUBLICATION TO CITE:



***************************************************************************
*/
#ifndef _RPSDB_CB_
#define _RPSDB_CB_

#include <asn.h>
#include <mmdbapi.h>
#include <objslristruc.h>
#include <objcdd.h>


#ifdef __cplusplus
extern "C" { /* } */
#endif

/* connect to database */
Boolean RPS_ConnectToDB(void);
Boolean RPS_DisconnectDB(void);

/* transaction related */
Boolean LIBCALL RPS_StartTransaction(void);
Boolean LIBCALL RPS_CommitTransaction(void);
Boolean LIBCALL RPS_RollBackTransaction(void);

/*Open tables*/
Boolean LIBCALL OpenRPSDB(CharPtr name, Boolean Create);
Boolean LIBCALL OpenDOMNAME(CharPtr name, Boolean Create);

/*Print tables*/
Boolean LIBCALL PrintRPSDB(void);
Boolean LIBCALL PrintDOMNAME(void);

/*Append to tables*/
Boolean LIBCALL AppendRecordRPSDB(StRpsdbPtr prs);
Boolean LIBCALL AppendRecordDOMNAME(StDomNamedbPtr dnp);

/*Delete from tables*/
Boolean LIBCALL DeleteGIinRPSDB(Int4 gi, Boolean destroy);
Boolean LIBCALL DeleteAccinDOMNAME(CharPtr, Boolean destroy);
Boolean LIBCALL DeleteAllRecordsInDOMNAME();

/*Is exist calls*/
Boolean LIBCALL IsExistInRPSDB(Int4 gi);
Boolean LIBCALL IsExistInDOMNAME(CharPtr);

/*packing and compression*/
void LIBCALL PackRPSDB(void);
void LIBCALL PackDOMNAME(void);

/*Close tables*/
Boolean LIBCALL CloseRPSDB(void);
Boolean LIBCALL CloseDOMNAME(void);

/**************************************
*Search calls
* Precondition - the search key
* is present in the parameter struct
* and evalue is set to some value
***************************************/
/* Search using a tag and Evalue cutoff */
Int2	LIBCALL SearchRPSDB(StRpsdbPtr PNTR pprs);
Int2	LIBCALL SearchDOMNAME(StDomNamedbPtr PNTR ppdm, Boolean noasn);

/*Search RPSDB using a combination of Tags and Evalue */
Int2 SearchRPSDBByTagsAndEvalue(StRpsdbPtr PNTR pprs);

/*Reset Calls*/
void LIBCALL ResetStRPSDB (StRpsdbPtr PNTR pprs);
void LIBCALL ResetStDOMNAME (StDomNamedbPtr PNTR ppdn);

/*******************
* Utility Functions
*******************/
CddPtr LIBCALL GetCddFROMRDOMNAME(void);

/*********************************
* Internal Functions
* Should be commented not for use
**********************************/
/*CODE4 cb; */
Boolean LIBCALL RPS_InitCodeBase(void);
Boolean LIBCALL RPS_CloseCodeBase(void);

#ifdef __cplusplus
/* { */ }
#endif


#endif

/*
$Log: rpsdb_db.h,v $
Revision 1.4  2004/12/14 19:49:05  zwang
Added transaction related functions.

Revision 1.3  2004/08/19 20:38:33  zwang
Added RPS_ConnectToDB, RPS_DisconnectDB

Revision 1.2  2004/01/12 17:46:55  mmatan
Added prefix RPS_ to names of InitCodeBase and CloseCodeBase function
prototypes.  This is being checked in for Zhe.

Revision 1.1.1.3  2002/10/31 09:34:19  root
Automated import from Sourceforge.net

Revision 1.3  2002/10/30 17:47:07  betel
Added IsExist* functions

Revision 1.2  2002/10/13 22:54:03  betel
Added table packing calls

Revision 1.1  2002/09/30 17:33:48  kaca
former rpsdb_cb.h

Revision 1.5  2002/08/22 18:38:00  betel
Changed SearchDOMANE to search without returning asn1

Revision 1.4  2002/07/29 22:30:46  betel
Added GetDomainsFromGiListWithEvalue for local&remote C api (no Perl or C++ yet)

Revision 1.3  2002/05/17 15:06:10  betel
Fixed Windows MSVC build

Revision 1.2  2002/04/11 20:27:58  betel
All search most include e-value. Searching without codebase relations

Revision 1.1  2002/04/02 21:41:54  kaca
moved from ../rps

Revision 1.4  2002/03/18 21:58:38  betel
Added Domain Table CodeBase functions

Revision 1.3  2002/03/15 22:13:15  betel
Added query function based on two tags and evalue

Revision 1.2  2002/03/15 00:41:12  betel
Added more query functions-not fully tested

Revision 1.1  2002/03/12 21:12:17  betel
Moved all rpsdb relevant files to rps dir

Revision 1.2  2002/03/11 22:59:22  betel
Small changes

Revision 1.1  2002/02/27 18:14:11  betel
Codebase layer for RPS results table

*/

