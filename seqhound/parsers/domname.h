/*$Id: domname.h,v 1.1.1.2 2002/03/13 09:27:19 root Exp $*/
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


  PROGRAM: DomNameToDB- Fills DomaName database containing Cdd profiles.

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
/*************************************************
* DomNameToDB generates SeqHounds DOMNAME table
* containing cdd domains.
**************************************************/

#ifndef _RPSDB_H_
#define _RPSDB_H_

#include <slri_misc.h>
#include <cddutil.h>
#include <objcdd.h>
#include <objslristruc.h>
#include <intrez_db.h>


#ifdef __cplusplus
extern "C" {
#endif

#define CDD_DBNAME "All"
#define CDD_EXT ".acd"

/*Map StDomNamedb structure to Cdd and commit to table*/
Boolean FillDomNameNode(StDomNamedbPtr pDomname, CddPtr pCdd);

/*filename contains the
* cdd profile in ASN.1 binary */
void GenerateDomName(char* filename);

/*Top level functions, iterates through a
* directory and call fcn for every *.acd file */
void DirWalk(CharPtr dir, void (*fcn)(char *));


#ifdef __cplusplus
}
#endif
#endif

/*
$Log: domname.h,v $
Revision 1.1.1.2  2002/03/13 09:27:19  root
Automated import from Sourceforge.net

Revision 1.2  2002/03/12 21:11:20  betel
Moved rpsdb files to rps dir

Revision 1.1  2002/03/11 23:02:38  betel
DomName table parser

*/

