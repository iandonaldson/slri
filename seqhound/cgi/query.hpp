/* $Id: query.hpp,v 1.2 2003/11/07 18:06:29 haocl Exp $ */ 
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


  PROGRAM: SLRI C++ Seqhound WebAPI search function


  AUTHORS: Hao Lieu


  REVISION/CONTRIBUTION NOTES:

  September 1, 2002  Original MSH Revision.
                Hogue Lab - University of Toronto Biochemistry
                Department and the Samuel Lunenfeld Research
                Institute, Mount Sinai Hospital
                http://bioinfo.mshri.on.ca  hogue@mshri.on.ca
                ftp://ftp.mshri.on.ca/


  PUBLICATION TO CITE:

************************************************************************/

#ifndef QUERY_HPP
#define QUERY_HPP

#include "logic.hpp"
#include "intrez.hpp"
#include "shoundtypes.hpp"

USING_NCBI_SCOPE;
using namespace shoundlogic;  /* (logic.hpp) */
BEGIN_SCOPE(shoundquery)

class Query 
{
private:
    shound::Intrez *shound;    /* intrez C++ layer that interfaces with the C layer */

public:   /* public functions */

    /* executes a query based on the parameters specified by the 3 lists 
     * and the molecule type. reference to results of search returned. 
     * keys is list of string of comma delimited identifiers, eg:
     *    keys[0] = "543423,2343243,2343245" 
     *    keys[1] = "3243,4324,345" ...
     */
    list<int>& ExecuteQuery(const list<eQueryType> &types, list<string> &keys, 
		       const list<SHoundLogic::eLogic> &logics, 
		       const list<string> &ECodeSTL, const list<Int2> &degreeSTL,
		       const eMolType type);
    
    list<CSHoundRecord*>& RetrieveRecords(list<int> &RequestedGi, list<CSHoundRecord*>& ReturnSTL, eFormatType eFormat);

    /* explicit constructor, implicit destructor */
    Query();
    ~Query();

};

END_SCOPE(shoundquery)

#endif

/* 
 * $Log: query.hpp,v $
 * Revision 1.2  2003/11/07 18:06:29  haocl
 * fixed compiling for new toolkit
 *
 * Revision 1.1.1.4  2003/08/19 08:57:35  root
 * Automated import from Sourceforge.net
 *
 * Revision 1.4  2003/08/18 19:17:09  haocl
 * minor changes, got rid of some magic numbers,
 * changed returned list to passed in references
 *
 * Revision 1.3  2003/08/12 19:37:11  haocl
 * added shoundrecords
 *
 * Revision 1.2  2003/08/08 16:22:43  haocl
 * new files shoundrecords and changes to others
 *
 * Revision 1.1  2003/07/31 21:16:38  haocl
 * checking in portion of the new web interface
 *
 *
 *
 */
