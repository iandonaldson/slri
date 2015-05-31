/* $Id: sgd_db.h,v 1.1.1.1 2003/06/26 08:53:34 root Exp $
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
#define GI_SIZE  10
#define ORF_SIZE 10

  typedef struct sgd_st
  {
    Int4 iGI;
    CharPtr pAcc;
    CharPtr pORF;
  }SGD_ST, *SGD_STPtr;

	
  static CODE4 sgd_cb;
  static DATA4 *pd4sgd = NULL;

  static FIELD4 *pf4acc;
  static FIELD4 *pf4gi;
  static FIELD4 *pf4orf;

  static TAG4 *pt4acc;
  static TAG4 *pt4gi;
  static TAG4 *pt4orf;

  static FIELD4INFO fieldInfo_sgd[] =
  {
    {"YST_ACCESS", 'C', ACC_SIZE, 0},
    {"YST_GI", 'N', GI_SIZE, 0},
    {"YST_ORF", 'C', ORF_SIZE, 0},
    {0, 0, 0, 0, 0}
  };

  static TAG4INFO tagInfo_sgd[] =
  {
    {"YST_ACC_TAG", "YST_ACCESS", 0, 0, 0},
    {"GI_TAG", "YST_GI", 0, 0, 0},
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


  /* search the SGD database for the record specified
   * by sgd. The search key is in sgd and must be
   * set by the user prior to calling SearchSGD
   * eg:
   *   sgd.iSGDID = 1234;
   *   SearchSGD(&sgd);
   *   ... then print out the record
   *  */
  extern Boolean SearchSGD(SGD_ST *sgd);
  
#ifdef __cplusplus
}
#endif
#endif

/*
$Log: sgd_db.h,v $
Revision 1.1.1.1  2003/06/26 08:53:34  root
Automated import from Sourceforge.net

Revision 1.1  2003/06/25 14:28:42  haocl
moving the sgd and yeast includes

Revision 1.1  2003/06/20 17:55:13  haocl
initial import of SGD database, used by Yeast GO annotation database

*/
