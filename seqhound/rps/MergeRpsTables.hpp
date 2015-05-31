/*$Id: MergeRpsTables.hpp,v 1.1.1.3 2002/10/22 08:33:28 root Exp $*/
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


  PROGRAM:MergeTable -merges two rpsresults tables.


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



***************************************************************************
*/
#ifndef _MERGE_RPS_TABLES_H
#define _MERGE_RPS_TABLES_H

#include <d4all.hpp>
#include "rpsdb_cb.hpp"
/***********************************************************************
*Short program that merges RPSDB.dbf tables.
*Usage: MergeTables <mastertable> <datafile to merge>
*
*To compile :
* g++ MergeRpsTables.cpp -lcbx -o MergeTables\
* -I../../../codebase/intel-unix/source -L../../../codebase/intel-unix/source
***********************************************************************/

/* Function prototypes */
void OpenDataFile( char* tableName );
bool AddNewRecord(RpsDBRecord& record);
unsigned long ReadTable(Data4& table);
void PrintRecords(void);

#endif
/*
$Log: MergeRpsTables.hpp,v $
Revision 1.1.1.3  2002/10/22 08:33:28  root
Automated import from Sourceforge.net

Revision 1.4  2002/10/21 21:31:14  betel
Update new table fields

Revision 1.3  2002/03/13 22:16:32  betel
Added #ifndef for header include

Revision 1.2  2002/03/12 21:22:53  betel
Small cahnges

Revision 1.1  2002/03/12 21:12:17  betel
Moved all rpsdb relevant files to rps dir

*/
