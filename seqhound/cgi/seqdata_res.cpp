/*  $Id: seqdata_res.cpp,v 1.1 2003/11/21 21:11:42 haocl Exp $
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


  PROGRAM: SLRI C++ Seqhound WebAPI search functions.
        This defines the main section of the program.

        This software is based on and implements software
	developed by Lewis Geer, Vsevolod Sandomirskiy, etc.
        at the National Center for Biotechnology Information
	(NCBI) http://www.ncbi.nlm.nih.gov/

  AUTHORS: Hao Lieu


  REVISION/CONTRIBUTION NOTES:

  September 1, 2002  Original MSH Revision.
                Hogue Lab - University of Toronto Biochemistry
                Department and the Samuel Lunenfeld Research
                Institute, Mount Sinai Hospital
                http://bioinfo.mshri.on.ca  hogue@mshri.on.ca
                ftp://ftp.mshri.on.ca/

************************************************************************/
#include "seqdata_res.hpp"
#include "seqdata_cmd.hpp"

#include <cgi/cgictx.hpp>
#include <corelib/ncbiutil.hpp>

BEGIN_SCOPE(shoundcgi)
USING_NCBI_SCOPE;

/*
 * class CSeqResource 
 */
CSeqResource::CSeqResource( CNcbiRegistry& config )
    : CNcbiResource( config )
{}
    
CSeqResource::~CSeqResource()
{}

CNcbiCommand* CSeqResource::GetDefaultCommand( void ) const
{
    return new CSeqBasicCommand( const_cast<CSeqResource&>( *this ) );
    
    /* CSeqBasicCommand is defined in seqcmd.hpp */
}

END_SCOPE(shoundcgi)

/* 
 * $Log: seqdata_res.cpp,v $
 * Revision 1.1  2003/11/21 21:11:42  haocl
 * major refactoring, splitting cgi into 2 executables, renaming files
 * from seqxxx to seqdata, renaming Makefile
 *
 * Revision 1.1.1.3  2003/08/16 08:45:57  root
 * Automated import from Sourceforge.net
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
 */
