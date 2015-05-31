/********************************************************************************
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
           Katerina Michalickova and Christopher W.V. Hogue hogue@mshri.on.ca
	   Renan A. CAVERO


  REVISION/CONTRIBUTION NOTES:

  June 1, 2000  Original MSH Revision.
                Hogue Lab - University of Toronto Biochemistry
                Department and the Samuel Lunenfeld Research
                Institute, Mount Sinai Hospital
                http://bioinfo.mshri.on.ca  hogue@mshri.on.ca
                ftp://ftp.mshri.on.ca/


  PUBLICATION TO CITE:


  DESCRIPTION
  April 2003
  SeqHound GOA database query.
  GOA Module
 

*******************************************************************************/

#ifndef __GOA_QUERY_H__
#define __GOA_QUERY_H__

#include <ncbi.h>

#ifdef __cplusplus
extern "C" {
#endif


/* Connect to Database Server. This funciton must be called 
   prior to any queries to the database can be properly executed. 
   Call this at the start of your program. */
Boolean GOA_ConectToDB(Boolean CREATE);


/* Closes the Database Connection.
   Call this function at the end of your program. */
Boolean GOA_CloseDB(void);

/*ValNodePtr LIBCALL SHoundSearchDBXREF(ValNodePtr vnpgo); */
ValNodePtr LIBCALL SHoundGiFromGOIDII(Int4 go);
ValNodePtr LIBCALL SHoundGiFromGOIDAndECodeII(Int4 go, CharPtr ECode);
ValNodePtr LIBCALL SHoundGiFromGOIDListII(ValNodePtr goList);
ValNodePtr LIBCALL SHoundGiFromGOIDListAndECodeII(ValNodePtr goList, CharPtr ECode);

ValNodePtr LIBCALL SHoundGOIDFromGiII(Int4 gi);
ValNodePtr LIBCALL SHoundGOIDFromGiListII(ValNodePtr giList);
ValNodePtr LIBCALL SHoundGOIDFromRedundantGiII(Int4 gi);
ValNodePtr LIBCALL SHoundGOIDFromRedundantGiListII(ValNodePtr giList);
ValNodePtr LIBCALL SHoundGOECodeFromGiAndGOIDII(Int4 gi, Int4 go);
ValNodePtr LIBCALL SHoundGOIDFromGENEIDII(Int4 gene);
ValNodePtr LIBCALL SHoundGOIDFromLLIDII(Int4 ll);

ValNodePtr LIBCALL SHoundPMIDFromGiAndGOID(Int4 gi, Int4 go);


#ifdef __cplusplus
}
#endif

#endif
