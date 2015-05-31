/* $Id: ll_db_misc.c,v 1.1.1.3 2001/12/20 09:23:48 root Exp $
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
  Locus Link miscellaneous
  Part of SeqHound

****************************************************************************/
#include <ll_db_misc.h>
#include <slri_misc.h>

/* frees cdd */
static void UnInitLL_CDDStruct(LL_CDDStruct *cdd)
{
  LL_CDDStructPtr tmpcdd, ptmpcdd;
  tmpcdd = cdd;
  ptmpcdd = NULL;
  while(tmpcdd != NULL){    
    ValNodeFreeData(tmpcdd->vnpCDD_ID);
    ValNodeFree(tmpcdd->vnpE_VAL);
    ptmpcdd = tmpcdd;
    tmpcdd = tmpcdd->next;
    MemFree(ptmpcdd);
  }
}

/* uninitializes ps */
void UndoInitLL_ParseStruct(LL_ParseStruct *ps)
{
  UnInitLL_CDDStruct(ps->cdd_st);
  ValNodeFree(ps->vnpOMIM);
  ValNodeFree(ps->vnpGOID);
  ValNodeFree(ps->vnpPMID);
  ValNodeFreeData(ps->vnpE_CODE);
  MemFree(ps->cpMAP);
}

/*
$Log: ll_db_misc.c,v $
Revision 1.1.1.3  2001/12/20 09:23:48  root
Automated import from Sourceforge.net

Revision 1.3  2001/12/19 23:26:37  kaca
added newline at the EOF

Revision 1.2  2001/12/11 20:19:51  kaca
added Log and Id keywords

*/




