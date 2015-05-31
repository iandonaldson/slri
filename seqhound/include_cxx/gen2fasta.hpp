/* $Id: gen2fasta.hpp,v 1.1.1.3 2003/03/07 09:32:59 root Exp $
***************************************************************************
Mount Sinai Hospital (MSH)

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
           Hao Lieu and Christopher W.V. Hogue
           hogue@mshri.on.ca


  REVISION/CONTRIBUTION NOTES:

  June 1, 2000  Original MSH Revision.
                Hogue Lab - University of Toronto Biochemistry
                Department and the Samuel Lunenfeld Research
                Institute, Mount Sinai Hospital
                http://bioinfo.mshri.on.ca  hogue@mshri.on.ca
                ftp://ftp.mshri.on.ca/


  PUBLICATION TO CITE:


  DESCRIPTION
  prints out FASTA protein database for a complete genome;
  input value is a taxonomy ID for the genome

*****************************************************************************/

#ifndef GEN2FASTA_HPP
#define GEN2FASTA_HPP

#include <corelib/ncbiapp.hpp>
#include <corelib/ncbienv.hpp>
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

class gen2fasta : public CNcbiApplication {

public:
  void Init();
  int Run();

};

#endif

/*
$Log: gen2fasta.hpp,v $
Revision 1.1.1.3  2003/03/07 09:32:59  root
Automated import from Sourceforge.net

Revision 1.3  2003/03/06 20:04:26  haocl
added license, fixed #ifndef directives

Revision 1.2  2003/02/06 15:46:16  haocl
new nblast and slristruc files and modifications to reflect new files

Revision 1.1  2002/08/23 19:57:34  haocl
initial import of examples of c++ seqhound use

*/
