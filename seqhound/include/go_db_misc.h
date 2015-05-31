/* $Id: go_db_misc.h,v 1.1 2004/06/07 22:41:04 hlieu Exp $
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
  Gene Ontology miscellaneous
  Part of SeqHound

****************************************************************************/
#ifndef __GO_DB_MISC_H__
#define __GO_DB_MISC_H__

#include <ncbi.h>

#ifdef __cplusplus
extern "C" {
#endif

  typedef struct {
    ByteStorePtr pGO_name;
    ValNodePtr pvnGO_id;
    ValNodePtr pvnGO_synonym;
    ValNodePtr pvnGO_reference;
    ValNodePtr pvnGO_refType;
  } ByteStructure, *ByteStructurePtr;

  /*extern Boolean IsBSEmpty(ByteStorePtr bsp);*/
  extern Boolean BSIsEmpty(ByteStorePtr bsp);
  extern void BSClear(ByteStorePtr bsp);
  /*extern void InitBStructure(ByteStructure *bs);*/
  extern void UndoInitBStructure(ByteStructure *bs);

#ifdef __cplusplus
}
#endif
#endif


/*
$Log: go_db_misc.h,v $
Revision 1.1  2004/06/07 22:41:04  hlieu
moved over from go

Revision 1.1.1.3  2002/07/09 08:28:28  root
Automated import from Sourceforge.net

Revision 1.3  2002/07/08 18:13:00  kaca
fixed compiler varnings

Revision 1.2  2001/12/11 20:12:38  kaca
added Id and Log keywords

*/

