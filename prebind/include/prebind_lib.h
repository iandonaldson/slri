/* $Id: prebind_lib.h,v 1.1.1.1 2002/07/17 19:36:35 janicek Exp $*/
/*
-- ***************************************************************************
--
--  Pre Biomolecular Interaction Network Database (PreBIND)
--
--  File: prebind_lib.h
--  Description: header for library specific functions and variables
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
--  Copyright �2000 Mount Sinai Hospital (MSH)
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




#ifndef _PREBIND_LIB_
#define _PREBIND_LIB_


#ifndef _PREBIND_DB_
#include "prebind_db.h"
#endif

#ifndef _BINDAPI_
#include "bindapi.h"
#endif

#ifndef _SEQHOUND_
#include "seqhoundapi.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

Int2			TrimSpaces(CharPtr);
CharPtr			GetTaxNameByTaxId(Int4 taxid);
Int2			CheckForSubmitErrorsUsingSeqHound(Int4 submitted_gi, Int4 submitted_type);	
BINDObjectPtr	ConstructAMolecularObject(CharPtr name, CharPtr descr, Int4 molecule_type, Int4 gi);

#ifdef __cplusplus
}
#endif

#endif  /* _PREBIND_LIB_ */


/*
$Log: prebind_lib.h,v $
Revision 1.1.1.1  2002/07/17 19:36:35  janicek
initial import

*/





