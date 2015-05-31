/*  $Id: taxdb_enum.h,v 1.1.1.3 2002/03/01 09:25:27 root Exp $
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

#ifndef _TAXDB_ENUM_
#define _TAXDB_ENUM_

#ifdef __cplusplus
extern "C" {
#endif

#include <asn.h>
#include <slri_misc.h>
#include <objslritax.h>

/************************/
/* Function definitions */
/************************/

extern Uint2 TaxDB_Enum_NameClassString2Enum(CharPtr nameClass);

extern Uint2 TaxDB_Enum_RankString2Enum(CharPtr rank);

extern Uint2 TaxDB_Enum_RankPremodString2Enum(CharPtr premod);

extern Uint2 TaxDB_Enum_RankPostmodString2Enum(CharPtr postmod);

/*tries to find an element of array in string - if found remove from string
-returns the index of array that was found*/
extern Int4 TaxDB_FindIndexAndStripString(CharPtr *string, CharPtr array[], Int4 numElmts);

/*process post modification rank name*/
extern Int4 TaxDB_GetPostModFromRankName(CharPtr *rank);

/*process pre modification rank name*/
extern Int4 TaxDB_GetPreModFromRankName(CharPtr *rank);


#ifdef __cplusplus
}
#endif
#endif		/* _TAXDB_ENUM_ */


/* 
 $Log: taxdb_enum.h,v $
 Revision 1.1.1.3  2002/03/01 09:25:27  root
 Automated import from Sourceforge.net

 Revision 1.3  2002/02/28 17:22:59  micheld
 New MSVC project files to build cb/db2/rem apps and libraries
 Moved include order to accomodate msvc, removed #ifdef [CODEBASE|DB2]

 Revision 1.2  2002/01/03 20:56:07  risserlin
 added ifdefs for d4all.h in header file so can be used when using db2

 Revision 1.1.1.1  2001/05/25 21:36:20  kaca
 initial import

 Revision 1.3  2001/04/02 15:00:40  kaca
 removed some extern declarations following the function type

 Revision 1.2  2001/03/29 22:14:59  gbader
 Moved statics to before function prototype

 Revision 1.1  2001/03/20 16:20:24  kaca
 migrated from the ../taxon directory

 Revision 1.5  2001/02/23 21:20:11  kaca
 license change

 Revision 1.4  2000/09/20 16:31:27  kaca
 fixed Log keyword

 Revision 1.3  2000/09/19 23:44:42  kaca
 added CVS Id and Log keywords
 */


