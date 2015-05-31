/* $Id: ll_query.h,v 1.1.1.1 2001/12/20 09:23:48 root Exp $
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

  DESCRIPTION
  Locus Link database query
  Part of SeqHound

****************************************************************************/
#ifndef __LL_QUERY_H__
#define __LL_QUERY_H__

#include <ncbi.h>

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************
                 PUBLIC INITIALIZATION FUNCTIONS
***************************************************************/

  /* initializes all the database fields and tags. This funciton 
     must be called prior to any queries to the database can be
     properly executed. Call this at the start of your program. */
  extern void LLDB_InitCodeBase(void);

  /* Opens all the database files needed for querying. This funciton 
     must be called prior to any queries to the database can be
     properly executed. Call this at the start of your program.
     DBpath is the path to the data base files. Do not include the
     file names in the path. */
  extern Boolean LLDB_OpenAllDBs(Boolean CREATE);
  
  /* closes the database files and flushes all buffers.
     at the end of your program you must call this function. */
  extern void LLDB_InitUndoCodeBase(void);

  /******************************************************************
			 PUBLIC PRINT FUNCTIONS
  *******************************************************************/
  /* prints the int data in vnp */
  extern void LLDB_PrintIntList(ValNodePtr vnp);

  /* prints the ptr value data in vnp */
  extern void LLDB_PrintECode(ValNodePtr vnp);

  
#ifdef __cplusplus
}
#endif
#endif

/*
$Log: ll_query.h,v $
Revision 1.1.1.1  2001/12/20 09:23:48  root
Automated import from Sourceforge.net

Revision 1.1  2001/12/19 23:22:34  kaca
files moved from ../go ../locuslink

Revision 1.5  2001/12/11 19:08:43  kaca
added Id and Log keywords

*/



