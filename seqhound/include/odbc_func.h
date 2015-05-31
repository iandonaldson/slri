/* $Id: odbc_func.h,v 1.1 2005/04/26 19:04:42 eburgess Exp $
*******************************************************************************************
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
  and distribute the resultsing executable, without including the source
  code for these in the source distribution:

  a) CodeBase 6.5 or greater from Sequiter Software Inc.

  b) The C or C++ interface to Oracle 8.x or greater, from
     Oracle Corporation or IBM DB2 UDB.


  PROGRAM:


  AUTHORS:
           Elizabeth Burgess and Christopher W.V. Hogue hogue@mshri.on.ca


  REVISION/CONTRIBUTION NOTES:

  June 1, 2000  Original MSH Revision.
                Hogue Lab - University of Toronto Biochemistry
                Department and the Samuel Lunenfeld Research
                Institute, Mount Sinai Hospital
                http://bioinfo.mshri.on.ca  hogue@mshri.on.ca
                ftp://ftp.mshri.on.ca/


cd   PUBLICATION TO CITE:


  DESCRIPTION
  August 2004
  Reads the chrom database and for each record, gets either
  the bioseqset or bioseq and assigns Kloodge ids in taxgi
  for all protein and rna gis derived from that record.

  - requires 1) a full refseq build with a full chrom table.


****************************************************************/

#ifndef _ODBC_FUNC_H_
#define _ODBC_FUNC_H_

#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>
#include <slri_odbc.h>
#include <stdlib.h>
#include <stdarg.h>

Boolean PrepStmt( SQLHANDLE *handle, CharPtr pSQL, 
				  Int4 numArgs, ... );	

Boolean ExecuteStmt( SQLHANDLE *handle, Int4 numArgs, ... );	
					 
					 
  
  

#endif /* _ODBC_FUNC_H_ defined */
