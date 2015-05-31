/* $Id: y_parser.h,v 1.1.1.2 2003/06/28 08:45:34 root Exp $
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
 

  PROGRAM:
 

  AUTHORS:  
            Hao Lieu


  REVISION/CONTRIBUTION NOTES:

  March 1, 2001  Original MSH Revision.
                Hogue Lab - University of Toronto Biochemistry 
                Department and the Samuel Lunenfeld Research 
                Institute, Mount Sinai Hospital
                http://bioinfo.mshri.on.ca  hogue@mshri.on.ca
                ftp://ftp.mshri.on.ca/


  PUBLICATION TO CITE:

  DESCRIPTION:
  Parser for orf_geneontology.all.tab which can be downloaded at 
  ftp://genome-ftp.stanford.edu/pub/yeast/data_download/literature_curation

****************************************************************************/

#ifndef __YPARSER_H__
#define __YPARSER_H__

#include <ncbi.h>

#ifdef __cplusplus
extern "C" {
#endif



/* the 'file' you want to parse */
extern Int2 Y_ParseFile(CharPtr file);
	
#ifdef __cplusplus
}
#endif
#endif

/*
$Log: y_parser.h,v $
Revision 1.1.1.2  2003/06/28 08:45:34  root
Automated import from Sourceforge.net

Revision 1.2  2003/06/26 19:35:25  haocl
cleaned up code, added comments and cvs tags


*/
