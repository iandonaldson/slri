/* $Id: ll_parser.h,v 1.2 2004/12/14 19:49:56 zwang Exp $
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
  Locus Link flat file parser
  Part of SeqHound

****************************************************************************/

#ifndef __LL_PARSER_H__
#define __LL_PARSER_H__

#include <ncbi.h>

#ifdef __cplusplus
extern "C" {
#endif

  /* when the eValue is "na", we store in this */
#define LL_EVAL_CONST 10000000

  /* when the PMID is "na" we store in this */
#define LL_PMID_CONST 1000

#define ALPHA_STRING " 1234567890abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"  

  /* This is the one public function.  Pass it the path to the file
     to be parsed. The contents of the file must be 
     in the format of the LL datafiles
  */
  extern Int4 LL_ParseFile(CharPtr ParsePath, Int2 IsUpdate);

#ifdef __cplusplus
}
#endif
#endif

/*
$Log: ll_parser.h,v $
Revision 1.2  2004/12/14 19:49:56  zwang
Made LL_ParseFile take one more parameter for transactional purpose.

Revision 1.1  2003/08/26 19:18:41  haocl
moved from locuslink

Revision 1.1.1.3  2001/12/20 09:23:48  root
Automated import from Sourceforge.net

Revision 1.3  2001/12/19 23:26:37  kaca
added newline at the EOF

Revision 1.2  2001/12/11 20:19:51  kaca
added Log and Id keywords

*/



