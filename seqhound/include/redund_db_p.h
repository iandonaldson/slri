/* $Id: redund_db_p.h,v 1.1.1.1 2002/08/20 08:29:52 root Exp $
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
  Locus Link code base privates
  Part of SeqHound

****************************************************************************/

#ifndef __REDUND_DB_P_H__
#define __REDUND_DB_P_H__

#include <d4all.h>

#ifdef __cplusplus
extern "C" {
#endif

  extern CODE4 cb;

  typedef struct struct_redundant{
    CharPtr name;
    DATA4  *pd4redund;    /* the RefSeq-LLID relation db */

    /* REDUNDDB */
    FIELD4 *pf4rdGi;    /* the evidence code in the GOID DB */
    FIELD4 *pf4rdOrdinal;       /* the OMIM code in the OMIM DB */
    FIELD4 *pf4rdGroup;        /* the CDD_ID code in the CDD DB */

    TAG4 *pt4rdGi;
    TAG4 *pt4rdOrdinal;
    TAG4 *pt4rdGroup;
    struct struct_redundant *next;
  } RedundDB, *RedundDBPtr;
  /* REDUNDDB*/
  static FIELD4INFO fieldInfo_Redund[] =
  {
    {"GI", r4num, 10, 0},
    {"ORDINAL", r4num, 10, 0},
    {"GROUP", r4num, 10, 0},
    {0, 0, 0, 0, 0}
  };

  static TAG4INFO tagInfo_Redund[] =
  {
    {"GI", "GI", 0, 0, 0},
    {"ORDINAL", "ORDINAL", 0, 0, 0},
    {"GROUP", "GROUP", 0, 0, 0},
    {0, 0, 0, 0, 0}
  };

#ifdef __cplusplus
}
#endif
#endif

/*
$Log: redund_db_p.h,v $
Revision 1.1.1.1  2002/08/20 08:29:52  root
Automated import from Sourceforge.net

Revision 1.1  2002/08/19 15:15:22  haocl
initial import of redund_db files


*/
