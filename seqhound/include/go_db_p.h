/* $Id: go_db_p.h,v 1.1.1.2 2003/07/30 08:49:14 root Exp $
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
  Gene Ontology code base privates
  Part of SeqHound

****************************************************************************/

#ifndef __GO_DB_P_H__
#define __GO_DB_P_H__

#include <d4all.h>
/*#include <ncbi.h>*/

#ifdef __cplusplus
extern "C" {
#endif

#define GO_ID_SIZE 7
#define GO_NAME_SIZE 10
#define GO_SYN_SIZE 10
#define GO_REF_SIZE 10
#define GO_DB_SIZE 20
#define GO_FILE_SIZE 2

extern CODE4 cb;

  static DATA4  *pD4GO_N = NULL;     /* the GO_ID-name relation db */
  static DATA4  *pD4GO_P = NULL;     /* the GO_ID-parent relation db */
  static DATA4  *pD4GO_S = NULL;     /* the GO_ID-synonym relation db */    
  static DATA4  *pD4GO_R = NULL;     /* the GO_ID-cross reference relation db */

  static FIELD4 *pF4goID_N;      /* the go_id field in the Name DB */
  static FIELD4 *pF4goID_P;      /* the go_id field in the Parent DB */
  static FIELD4 *pF4goID_S;      /* the go_id field in the Synonym DB */
  static FIELD4 *pF4goID_R;      /* the go_id field in the Reference DB */
  static FIELD4 *pF4go_N;        /* the name of the gene */
  static FIELD4 *pF4go_P;        /* the parent of a gene */
  static FIELD4 *pF4go_S;        /* the synonym of a gene */
  static FIELD4 *pF4go_R;        /* the reference of a gene */
  static FIELD4 *pF4go_I;        /* the reference database */
  static FIELD4 *pF4go_F;        /* the DB file, is it from the biological, 
				    cellular, or molecular file */
  static FIELD4 *pF4go_L;        /* the hierarchy level of the gene */

  static TAG4 *pT4goID_N;    /* the go_id tag in the Name DB */
  static TAG4 *pT4goID_P;    /* the go_id tag in the Parent DB */
  static TAG4 *pT4goID_S;    /* the go_id tag in the Synonym DB */
  static TAG4 *pT4goID_R;    /* the go_id tag in the Reference DB */
  static TAG4 *pT4p;         /* the parent tag in the Parent DB */  
  static TAG4 *pT4i;         /* the reference database tag in the Reference DB */
  static TAG4 *pT4f;         /* the DB file tage */
  static TAG4 *pT4L;         /* the hierarchy level tag */

  static FIELD4INFO fieldInfo_Name[] =
  {
    {"GO_ID_NAME", r4num, GO_ID_SIZE, 0},
    {"GO_NAME", r4memo, GO_REF_SIZE, 0},
    {"GO_DB", r4num, GO_FILE_SIZE, 0},
    {"GO_LEVEL", r4num, GO_FILE_SIZE, 0},
    {0, 0, 0, 0, 0}
  };

  static FIELD4INFO fieldInfo_Parent[] =
  {
    {"GO_ID_PAR", r4num, GO_ID_SIZE, 0},
    {"GO_PAR", r4num, GO_ID_SIZE, 0},
    {0, 0, 0, 0, 0}
  };

  static FIELD4INFO fieldInfo_Synonym[] = 
  {
    {"GO_ID_SYN", r4num, GO_ID_SIZE, 0},
    {"GO_SYN", r4memo, GO_REF_SIZE, 0},
    {0, 0, 0, 0, 0}
  };

  static FIELD4INFO fieldInfo_Reference[] =
  {
    {"GO_ID_REF", r4num, GO_ID_SIZE , 0},
    {"GO_REF", r4memo, GO_REF_SIZE, 0},
    {"GO_REF_DB", r4str, GO_DB_SIZE, 0},
    {0, 0, 0, 0, 0}
  };

  static TAG4INFO tagInfo_Name[] =
  {
    {"ID_TAG_NAME", "GO_ID_NAME", 0, 0, 0},
    {"DB_TAG", "GO_DB", 0, 0, 0},
    {"DB_LEVEL", "GO_LEVEL", 0, 0, 0},
    {0, 0, 0, 0, 0}
  };
  
  static TAG4INFO tagInfo_Parent[] =
  {
    {"ID_TAG_PAR", "GO_ID_PAR", 0, 0, 0},
    {"PAR_TAG", "GO_PAR", 0, 0, 0},
    {0, 0, 0, 0, 0}
  };
  
  static TAG4INFO tagInfo_Synonym[] =
  {
    {"ID_TAG_SYN", "GO_ID_SYN", 0, 0, 0},
    {0, 0, 0, 0, 0}
  };
  
  static TAG4INFO tagInfo_Reference[] =
  {
    {"ID_TAG_REF", "GO_ID_REF", 0, 0, 0},
    {"DB_TAG", "GO_REF_DB", 0, 0, 0},
    {0, 0, 0, 0, 0}
  };

#ifdef __cplusplus
}
#endif
#endif

/*
$Log: go_db_p.h,v $
Revision 1.1.1.2  2003/07/30 08:49:14  root
Automated import from Sourceforge.net

Revision 1.2  2003/07/28 14:19:13  haocl
fixed a size error in the reference db

Revision 1.1  2001/12/19 23:22:33  kaca
files moved from ../go ../locuslink

Revision 1.2  2001/12/11 20:12:38  kaca
added Id and Log keywords

*/


