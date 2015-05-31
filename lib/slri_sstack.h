/* $Id: slri_sstack.h,v 1.1.1.1 2001/10/12 08:21:54 root Exp $
***************************************************************************

  Samuel Lunenfeld Research Institute (SLRI)

  File: slri_sstack.h
  Description: String stack library

  Author: Anthony Hrvojic (hrvojic@ieee.org)
  Date: June 2001

  Hogue Lab - University of Toronto Biochemistry Department
  Samuel Lunenfeld Research Institute, Mount Sinai Hospital

  Copyright Notice:

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

***************************************************************************
*/

#ifndef _SLRI_SSTACK_
#define _SLRI_SSTACK_

#include <asn.h>

#ifdef __cplusplus
extern "C" {
#endif

  /*************************************/
  /* StringStack structs and functions */
  /*************************************/

  typedef struct struct_SLRI_SStackItem {
    struct struct_SLRI_SStackItem *next;
    struct struct_SLRI_SStackItem *prev;
    CharPtr string;
  } SLRI_SStackItem, *SLRI_SStackItemPtr;

  typedef struct struct_SLRI_SStack {
    Int4 totalStackSize;
    Int4 totalNumItems;
    SLRI_SStackItemPtr top;
    SLRI_SStackItemPtr bottom;
  } SLRI_SStack, *SLRI_SStackPtr;

  /* Creates a new StringStack */
  extern SLRI_SStackPtr SLRI_SSNew(void);

  /* Frees a StringStack and all items contained in it */
  extern SLRI_SStackPtr SLRI_SSFree(SLRI_SStackPtr ssp);

  /* Clears all items in StringStack */
  extern Int4 SLRI_SSClear(SLRI_SStackPtr ssp);

  /* Pushes a StringStack item onto StringStack */
  extern Int4 SLRI_SSPush(SLRI_SStackPtr ssp, CharPtr string);

  /* Pops top StringStack item from StringStack */
  extern CharPtr SLRI_SSPop(SLRI_SStackPtr ssp);

  /* Returns string from top StringStack item */
  extern CharPtr SLRI_SSPeek(SLRI_SStackPtr ssp);

  /* Checks if StringStack empty */
  extern Boolean SLRI_SSIsEmpty(SLRI_SStackPtr ssp);

  /* Returns a concatenation of all stack contents */
  extern CharPtr SLRI_SSGetWholeString(SLRI_SStackPtr ssp);

  /* Returns total size of all strings in stack */
  extern Int4 SLRI_SSGetTotalStackSize(SLRI_SStackPtr ssp);

  /* Returns total number of elements in stack */
  extern Int4 SLRI_SSGetTotalNumItems(SLRI_SStackPtr ssp);


#ifdef __cplusplus
}
#endif

#endif


/*

$Log: slri_sstack.h,v $
Revision 1.1.1.1  2001/10/12 08:21:54  root
Automated import from Sourceforge.net

Revision 1.1.1.1  2001/10/10 22:11:04  kaca
initial import

Revision 1.1  2001/06/26 20:59:30  hrvojic
Initial import of slri_sstack library


*/
