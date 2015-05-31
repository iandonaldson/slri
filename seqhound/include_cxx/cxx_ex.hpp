/* $Id: cxx_ex.hpp,v 1.1.1.3 2003/03/07 09:32:59 root Exp $ */ 
/***************************************************************************
  Copyright (C) 2001 Mount Sinai Hospital (MSH)
    
  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public License
  as published by the Free Software Foundation; either version 2.1 of
  the License, or any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

  See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with this program; if not, write to the
         Free Software Foundation, Inc.,
         59 Temple Place, Suite 330, Boston, MA
         02111-1307  USA
  or visit http://www.gnu.org/licenses/lgpl.txt

  SPECIAL EXCEPTIONS

  As a special exception, Mount Sinai Hospital gives permission to
  link this program with the following non-GPL programs or libraries,
  and distribute the resulting executable, without including the source
  code for these in the source distribution:

  a) CodeBase 6.5 or greater from Sequiter Software Inc.

  b) The C or C++ interface to Oracle 8.x or greater, from
     Oracle Corporation or IBM DB2 UDB.


  PROGRAM: SLRI C++ Seqhound test functions.


  AUTHORS: Hao Lieu


  REVISION/CONTRIBUTION NOTES:

  September 1, 2002  Original MSH Revision.
                Hogue Lab - University of Toronto Biochemistry
                Department and the Samuel Lunenfeld Research
                Institute, Mount Sinai Hospital
                http://bioinfo.mshri.on.ca  hogue@mshri.on.ca
                ftp://ftp.mshri.on.ca/


  PUBLICATION TO CITE:

***********************************************************************
*/

#ifndef CXX_EX_HPP
#define CXX_EX_HPP


#include <corelib/ncbienv.hpp>
#include <corelib/ncbiapp.hpp>
#include <corelib/ncbiargs.hpp>
#include <Seqhound.hpp>

#include <serial/serial.hpp>

#include <objects/mmdb1/Biostruc.hpp>
#include <objects/seqalign/Seq_align.hpp>
#include <objects/seq/Bioseq.hpp>
#include <objects/seqset/Bioseq_set.hpp>
#include <objects/seqset/Seq_entry.hpp>

#include <nblast.hpp>
#include <objslrilinkset.hpp>
#include <objslrilibstruc.hpp>
#include <slristruc.hpp>
#include <slri_misc.hpp>


USING_NCBI_SCOPE;

class cxx_ex : public CNcbiApplication {

public:
  void Init();
  int Run();

};

#endif

/* 
 * $Log: cxx_ex.hpp,v $
 * Revision 1.1.1.3  2003/03/07 09:32:59  root
 * Automated import from Sourceforge.net
 *
 * Revision 1.3  2003/03/06 20:04:25  haocl
 * added license, fixed #ifndef directives
 *
 *
 */
