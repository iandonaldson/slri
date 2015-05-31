/* $Id: ll_db_misc.h,v 1.4 2004/05/13 17:39:13 hlieu Exp $
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
#ifndef __LL_DB_MISC_H__
#define __LL_DB_MISC_H__

#include <ncbi.h>

#ifdef __cplusplus
extern "C" {
#endif

  typedef struct ll_cdd_struct
  {
    Int4 iNPid;                     /* the NPid */
    ValNodePtr vnpCDD_ID;           /* the cdd id */
    ValNodePtr vnpE_VAL;            /* the e-values */
    struct ll_cdd_struct *next;     /* next in linked list */
  } LL_CDDStruct, *LL_CDDStructPtr;

  typedef struct 
  {
    Int4 iLLID;
    LL_CDDStruct *cdd_st;
    LL_CDDStruct *last;
    CharPtr cpMAP;
    ValNodePtr vnpOMIM;
    ValNodePtr vnpGOID;
    ValNodePtr vnpPMID;
    ValNodePtr vnpE_CODE;
  } LL_ParseStruct, *LL_ParseStructPtr;
  
  extern void UndoInitLL_ParseStruct(LL_ParseStruct *ps);
  
#ifdef __cplusplus
}
#endif
#endif


/*
$Log: ll_db_misc.h,v $
Revision 1.4  2004/05/13 17:39:13  hlieu
major refactor of locus link module. Primary key changed from gi to
locus id. ASN source code also required update.

Revision 1.3  2004/01/12 22:26:07  zwang
Removed redefined function.

Revision 1.2  2004/01/12 22:08:04  zwang
Fixed typo. Added function to close all LL databases.

Revision 1.1  2003/08/26 19:18:41  haocl
moved from locuslink

Revision 1.1.1.2  2001/12/12 09:22:27  root
Automated import from Sourceforge.net

Revision 1.2  2001/12/11 20:19:51  kaca
added Log and Id keywords

*/
