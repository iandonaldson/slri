/* $Id: slri_set.h,v 1.1.1.3 2002/03/08 09:27:04 root Exp $
***************************************************************************

  Samuel Lunenfeld Research Institute (SLRI)

  File: slri_set.h
  Description: Set library for boolean operations

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

#ifndef _SLRI_SET_
#define _SLRI_SET_

#include <asn.h>

#ifdef __cplusplus
extern "C" {
#endif

  typedef struct struct_SLRI_BucketElem {
    struct struct_SLRI_BucketElem *next;
    VoidPtr datum;
  } SLRI_BucketElem, *SLRI_BucketElemPtr;

  typedef struct struct_SLRI_Set {
    Int4 numBuckets;
    VoidPtr datum;
    SLRI_BucketElemPtr *buckets;
    Int4 (*HashingFcn)(VoidPtr vpItem);
    Boolean (*Comparator)(VoidPtr vpItem1, VoidPtr vpItem2);
  } SLRI_Set, *SLRI_SetPtr;

  typedef struct struct_SLRI_SetFunPack {
    Int4 numBuckets;
    Int4 (*HashingFcn)(VoidPtr vpItem);
    Boolean (*Comparator)(VoidPtr vpItem1, VoidPtr vpItem2);
  } SLRI_SetFunPack, *SLRI_SetFunPackPtr;

  /***********************/
  /* Low-Level Functions */
  /***********************/

  /* Creates a new set */
  extern SLRI_SetPtr SLRI_SetCreate(SLRI_SetFunPackPtr funPack); 

  /* Frees an existing set */
  /* NOTE: user is responsible for freeing set datum */
  extern SLRI_SetPtr SLRI_SetFree(SLRI_SetPtr spSet);

  /* Clears an existing set */
  extern Int4 SLRI_SetClear(SLRI_SetPtr spSet);

  /* Inserts a new element into an existing set */
  extern Int4 SLRI_SetInsertElem(SLRI_SetPtr spSet, VoidPtr vpElem);

  /* Deletes an element from an existing set */
  extern Int4 SLRI_SetDeleteElem(SLRI_SetPtr spSet, VoidPtr vpElem);

  /* Checks if set is empty */
  extern Boolean SLRI_SetIsEmpty(SLRI_SetPtr spSet);

  /* Returns number of elements in given set (returns -1 for error) */
  extern Int4 SLRI_SetCount(SLRI_SetPtr spSet);

  /* Checks if an element is a member of a given set */
  /* NOTE: vpMemberElem is optional (can be NULL) */
  extern Boolean SLRI_SetIsMember(SLRI_SetPtr spSet, VoidPtr vpElem, VoidPtr *vpMemberElem);

  /* Copies an existing set */
  /* NOTE: destination set does not inherit set datum from source set */
  extern Int4 SLRI_SetCopy(SLRI_SetPtr spDest, SLRI_SetPtr spSource);

  /* Returns union of two sets (OR) */
  /* NOTE: - resulting set does not inherit set data from operand sets */
  /*       - passed function is optional (can be NULL); operates on ANDed elements */
  extern Int4 SLRI_SetUnite(SLRI_SetPtr spUnion, 
                            SLRI_SetPtr spSet1, SLRI_SetPtr spSet2,
                            void (*ANDElems)(VoidPtr *vpInsertionElem, VoidPtr vpMatchedElem));

  /* Returns intersection of two sets (AND) */
  /* NOTE: - resulting set does not inherit set data from operand sets */
  /*       - passed function is optional (can be NULL); operates on ANDed elements */
  extern Int4 SLRI_SetIntersect(SLRI_SetPtr spIntersection, 
                                SLRI_SetPtr spSet1, SLRI_SetPtr spSet2,
                                void (*ANDElems)(VoidPtr *vpInsertionElem, VoidPtr vpMatchedElem));

  /* Subtracts s2 from s1 (NOT) */
  /* NOTE: resulting set does not inherit set data from operand sets */
  extern Int4 SLRI_SetSubtract(SLRI_SetPtr spSubtraction, SLRI_SetPtr spSet1, SLRI_SetPtr spSet2);

  /* Iterates a given function over all elements in the set */
  extern Int4 SLRI_SetIterateFcn(SLRI_SetPtr spSet, 
                                 void (*f)(SLRI_SetPtr spSet, 
                                           Boolean *deletedBucketElem, 
                                           VoidPtr datum, VoidPtr vpArgs), 
                                 VoidPtr vpArgs);


  /************************/
  /* High-Level Functions */
  /************************/

  /* Initializes:
   *   - Pseudo-prime gSetPrime for hashing function
   *   - Optimal number of buckets based on entries per bucket and next largest prime
   *
   * 'numVerifyTrials' represents the number of times the probabilistic prime verification
   * algorithm will run to ensure that the resulting number is prime (1-25 is reasonable)
   *
   * NOTE: userFunPack is optional; if NULL, library's global function pack will be used
   *
   */
  extern Int4 SLRI_InitSetParameters(SLRI_SetFunPackPtr userFunPack, Int4 totalEntries, 
                                     Int4 entriesPerBucket, Int4 numVerifyTrials);

  /* Assign functions to global function pack */
  extern Int4 SLRI_AssignGlobalSetFcns(Int4 (*HashingFcn)(VoidPtr vpItem),
                                       Boolean (*Comparator)(VoidPtr vpItem1, VoidPtr vpItem2));

  /* Initialize global function pack with library's string functions */
  extern Int4 SLRI_InitStringFunctionPack(void);

  /* Wrapper for initializing global function pack with integer functions */
  extern Int4 SLRI_InitIntFunctionPack(void);


#ifdef __cplusplus
}
#endif

#endif


/*

$Log: slri_set.h,v $
Revision 1.1.1.3  2002/03/08 09:27:04  root
Automated import from Sourceforge.net

Revision 1.3  2002/03/07 17:16:49  gbader
Added a function to count elements in a set.

Revision 1.2  2002/03/06 00:46:54  gbader
Added integer set high-level functions.

Revision 1.1.1.1  2001/10/10 22:11:04  kaca
initial import

Revision 1.9  2001/08/30 19:16:29  gbader
Moved globals to c file

Revision 1.8  2001/08/30 14:09:13  hrvojic
Changed g_MPrime to g_SetPrime to fix conflict with slri_ti_query

Revision 1.7  2001/08/29 14:09:05  hrvojic
Added high-level functions for init, sample hashing/comparison functions

Revision 1.6  2001/08/14 15:14:23  hrvojic
Added ability to process ANDed elements as intersection/union performed

Revision 1.5  2001/08/08 13:34:00  hrvojic
Set iterator can now adjust its traversal if passed function deletes a bucket element

Revision 1.4  2001/07/09 15:23:21  hrvojic
Added datum pointer to SLRI_Set struct

Revision 1.3  2001/07/03 14:42:28  hrvojic
Set iterator can now pass arguments to iterated function

Revision 1.2  2001/06/29 18:21:33  hrvojic
Added SLRI_Set function pack

Revision 1.1  2001/06/26 20:59:01  hrvojic
Initial import of slri_set library


*/
