/* $Id: go_db_misc.c,v 1.1.1.3 2001/12/20 09:23:47 root Exp $
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
#include <go_db_misc.h>
#include <slri_misc.h>

/* returns whether bsp is empty or not */
Boolean BSIsEmpty(ByteStorePtr bsp)
{
  if(BSLen(bsp) == 0){
    return (TRUE);
  }
  
  return (FALSE);
}

/* clears bsp */
void BSClear(ByteStorePtr bsp) 
{
  BSSeek(bsp, 0L, SEEK_SET);
  BSDelete(bsp, BSLen(bsp));
  BSSeek(bsp, 0L, SEEK_SET);
}

/* uninitializes bs */
void UndoInitBStructure(ByteStructure *bs)
{
  BSFree(bs->pGO_name);
  ValNodeFreeData(bs->pvnGO_id);
  ValNodeFreeData(bs->pvnGO_synonym);
  ValNodeFreeData(bs->pvnGO_reference);
  ValNodeFreeData(bs->pvnGO_refType);
}

/*
$Log: go_db_misc.c,v $
Revision 1.1.1.3  2001/12/20 09:23:47  root
Automated import from Sourceforge.net

Revision 1.3  2001/12/19 23:21:06  kaca
added DB2 layer code

Revision 1.2  2001/12/11 20:12:38  kaca
added Id and Log keywords

*/


