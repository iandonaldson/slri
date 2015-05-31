/* $Id: hist_db_p.h,v 1.2 2003/08/22 13:35:59 haocl Exp $
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

#ifndef __HIST_DB_P_H__
#define __HIST_DB_P_H__

#include <d4all.h>

#ifdef __cplusplus
extern "C" {
#endif

#define HIST_GI_SIZE 10
#define VERSION_SIZE 10
#define DATE_SIZE 8
#define ACTION_SIZE 1
#define ACCESS_SIZE 20

  extern CODE4 cb;

  static DATA4  *pD4hist = NULL;    /* the RefSeq-LLID relation db */

  /* HISTDB */
  static FIELD4 *pF4H_gi;             /* the Gi */
  static FIELD4 *pF4H_accession;      /* the accession */
  static FIELD4 *pF4H_version;        /* the version */
  static FIELD4 *pF4H_date;           /* the date */
  static FIELD4 *pF4H_action;         /* ACTION_ADDED, ACTION_KILLED, or ACTION_CHANGED  */

  static TAG4 *pT4H_gi;         /* gi tag in the GOID DB */
  static TAG4 *pT4H_acc;        /* accession tag */
  static TAG4 *pT4H_date;       /* data tag */
  static TAG4 *pT4H_action;     /* action tag */

  /* HISTDB */
  static FIELD4INFO fieldInfo_HIST[] =
  {
    {"HIST_GI", r4num, HIST_GI_SIZE, 0},
    {"HIST_ACC", r4str, ACCESS_SIZE, 0},
    {"HIST_VER", r4num, VERSION_SIZE, 0},
    {"HIST_DATE", r4str, DATE_SIZE, 0},
    {"HIST_ACT", r4num, ACTION_SIZE, 0},
    {0, 0, 0, 0, 0}
  };

  static TAG4INFO tagInfo_HIST[] =
  {
    {"TAG_GI", "HIST_GI", 0, 0, 0},
    {"TAG_ACC", "HIST_ACC", 0, 0, 0},
    {"TAG_DATE", "HIST_DATE", 0, 0, 0},
    {"TAG_ACT", "HIST_ACT", 0, 0, 0},
    {0, 0, 0, 0, 0}
  };

#ifdef __cplusplus
}
#endif
#endif

/*
$Log: hist_db_p.h,v $
Revision 1.2  2003/08/22 13:35:59  haocl
removed unused codebase ACCDB references. Using intrez.c references

Revision 1.1.1.1  2002/08/09 08:29:48  root
Automated import from Sourceforge.net

Revision 1.2  2002/08/08 22:20:14  kaca
removed ACCDB table pointers, these can be gotten by including intrez_cb.h

Revision 1.1  2002/08/08 18:35:37  haocl
initial import of hist files

*/
