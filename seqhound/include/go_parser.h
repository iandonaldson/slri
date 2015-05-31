/* $Id: go_parser.h,v 1.1 2004/06/07 22:41:04 hlieu Exp $
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

  June 1, 2001  Original MSH Revision.
                Hogue Lab - University of Toronto Biochemistry 
                Department and the Samuel Lunenfeld Research 
                Institute, Mount Sinai Hospital
                http://bioinfo.mshri.on.ca  hogue@mshri.on.ca
                ftp://ftp.mshri.on.ca/


  PUBLICATION TO CITE:

  DESCRIPTION
  Gene Ontology flat file parser
  Part of SeqHound

****************************************************************************/

#ifndef __GO_PARSER_H__
#define __GO_PARSER_H__

#include <ncbi.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SYN -10
#define GOID -20
#define REF -30

#define GO_MOLFUNCTION 1
#define GO_BIOPROCESS 2
#define GO_CELLCOMPONENT 3

#define GO_MOL_FUN "molecular_function"
#define GO_BIO_PRO "biological_process"
#define GO_CELL_COM "cellular_component" 

  /* This is the one public function.  Pass it the path to the file
     to be parsed. The contents of the file must be 
     in the format of the GO datafiles
  */
  extern void GODB_ParseFile(CharPtr ParsePath);

#ifdef __cplusplus
}
#endif
#endif


/*
$Log: go_parser.h,v $
Revision 1.1  2004/06/07 22:41:04  hlieu
moved over from go

Revision 1.1.1.2  2001/12/12 09:22:27  root
Automated import from Sourceforge.net

Revision 1.2  2001/12/11 20:12:38  kaca
added Id and Log keywords

*/

