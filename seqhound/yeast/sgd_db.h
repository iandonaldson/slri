/* $Id: sgd_db.h,v 1.1.1.1 2003/06/22 08:49:47 root Exp $
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
  SGD CodeBase database layer, listing of prototypes in interface.
  
  Includes the specification for the size of each field in the db,
  the fields and tags (indexes), functions to initialize the 
  database, append records to the database, print out the content. 

****************************************************************************/

#ifndef __SGD_DB_H__
#define __SGD_DB_H__

#include <slri_cb.h>
#include <slri_misc.h>
#include <ncbi.h>
#include <d4all.h>
#ifdef __cplusplus
extern "C" {
#endif

#define ACC_SIZE 10
#define PROT_ID_SIZE 11 
#define SGD_ID_SIZE 7
#define ORF_SIZE 10

  typedef struct yeast_st
  {
    CharPtr pAcc;
    CharPtr pProtID;
    Int4 iSGDID;
    CharPtr pORF;
  }SGD_ST, *SGD_STPtr;

	
  static CODE4 sgd_cb;

  static DATA4 *pd4sgd = NULL;
  static FIELD4 *pf4acc;
  static FIELD4 *pf4prot_id;
  static FIELD4 *pf4sgd_id;
  static FIELD4 *pf4orf;

  static TAG4 *pt4acc;
  static TAG4 *pt4prot_id;
  static TAG4 *pt4sgd_id;
  static TAG4 *pt4orf;

  static FIELD4INFO fieldInfo_sgd[] =
  {
    {"YST_ACCESS", 'C', ACC_SIZE, 0},
    {"YST_PROTID", 'C', PROT_ID_SIZE, 0},
    {"YST_SGD_ID", 'N', SGD_ID_SIZE, 0},
    {"YST_ORF", 'C', ORF_SIZE, 0},
    {0, 0, 0, 0, 0}
  };

  static TAG4INFO tagInfo_sgd[] =
  {
    {"YST_ACC_TAG", "YST_ACCESS", 0, 0, 0},
    {"PROT_ID_TAG", "YST_PROTID", 0, 0, 0},
    {"SGD_ID_TAG", "YST_SGD_ID", 0, 0, 0},
    {"ORF_TAG", "YST_ORF", 0, 0, 0},
    {0, 0, 0, 0, 0}
  };

  /* initialize the database */ 
  extern void SGD_InitDataBase(void);

  /* close the database */
  extern void SGD_CloseDataBase(void);
  
  /* open the database and initialize the fields and 
   * tags, if CREATE is TRUE, SGD database gets created,
   * else use the existing SGD database. */	  
  extern Boolean SGD_OpenDB(Boolean CREATE);

  /* append a record into the SGD database */
  extern Boolean SGD_AppendRecord(SGD_ST* sgd);
  
  /* print out the SGD database, one record a line
   * in alphabetically order based on the accession number */
  extern Boolean SGD_PrintDataBase(void); 
#ifdef __cplusplus
}
#endif
#endif

/*
$Log: sgd_db.h,v $
Revision 1.1.1.1  2003/06/22 08:49:47  root
Automated import from Sourceforge.net

Revision 1.1  2003/06/20 17:55:13  haocl
initial import of SGD database, used by Yeast GO annotation database

*/
