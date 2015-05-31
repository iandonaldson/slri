/*  $Id: seqdata_res.hpp,v 1.1 2003/11/21 21:11:42 haocl Exp $
 ***************************************************************************
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


  PROGRAM: SLRI C++ Seqhound WebAPI search functions


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

#ifndef SEQRES__HPP
#define SEQRES__HPP
#include <cgi/ncbires.hpp>

BEGIN_SCOPE(shoundcgi)
USING_NCBI_SCOPE;

/*
 * class CSeqResource
 */
class CSeqResource : public CNcbiResource    /* see ncbires.hpp */
{
public:

    CSeqResource( CNcbiRegistry& config );
    virtual ~CSeqResource();   
   
    /* define the command to be executed when no other command matches */
   
    virtual CNcbiCommand* GetDefaultCommand( void ) const;
    
};

END_SCOPE(shoundcgi)

#endif /* NCBI_SEQRES__HPP */

/* $Log 
 */
	

