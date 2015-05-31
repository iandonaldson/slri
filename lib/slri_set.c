/* $Id: slri_set.c,v 1.1.1.4 2002/03/08 09:27:04 root Exp $
***************************************************************************

  Samuel Lunenfeld Research Institute (SLRI)

  File: slri_set.c
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

#include <stdlib.h>
#include <slri_set.h>
#include <slri_prime.h>


/* Macro for ensuring supplied hash stays within bounds */
#define hash(s,e) (abs((Int4)((*(s->HashingFcn))(e)) % s->numBuckets))



/********************/
/* Global variables */
/********************/

/* Pseudo-prime number for hashing function */
Int4 g_SetPrime = 0;

/* Function pack for high-level functions */
SLRI_SetFunPack g_funPack;

/***********************/
/* Low-Level Functions */
/***********************/

/* Creates a new set */
SLRI_SetPtr
SLRI_SetCreate(SLRI_SetFunPackPtr funPack)
{
  SLRI_SetPtr spSet = NULL;
  Int4 i;

  if (funPack != NULL) {
    /* Use user-defined function pack */

    /* Check if numBuckets is valid */
    if (funPack->numBuckets < 1) {
      ErrPostEx(SEV_ERROR, 0, 0, "SLRI_SetCreate: numBuckets not valid");
      return(NULL);
    }

    /* Allocate new set */
    spSet = (SLRI_SetPtr)MemNew(sizeof(SLRI_Set));
    spSet->datum = NULL;
    spSet->numBuckets = funPack->numBuckets;
    spSet->HashingFcn = funPack->HashingFcn;
    spSet->Comparator = funPack->Comparator;
    spSet->buckets = (SLRI_BucketElemPtr *)MemNew(spSet->numBuckets * sizeof(SLRI_BucketElemPtr));

    /* Initialize buckets */
    for (i = 0; i < spSet->numBuckets; i++) {
      spSet->buckets[i] = NULL;
    }

    return(spSet);
  }
  else {
    /* Use library's global function pack; check before using */
    if (g_funPack.HashingFcn == NULL) {
      ErrPostEx(SEV_ERROR, 0, 0, "SLRI_SetCreate: global hashing function is NULL");
      return(NULL);
    }
    if (g_funPack.Comparator == NULL) {
      ErrPostEx(SEV_ERROR, 0, 0, "SLRI_SetCreate: global comparator function is NULL");
      return(NULL);
    }

    return(SLRI_SetCreate(&g_funPack));
  }
} 


/* Frees an existing set */
/* NOTE: user is responsible for freeing set datum */
SLRI_SetPtr
SLRI_SetFree(SLRI_SetPtr spSet)
{
  if (spSet == NULL) {
    return(NULL);
  }

  /* Free all buckets */
  SLRI_SetClear(spSet);

  /* Free set itself */
  MemFree(spSet->buckets);
  MemFree(spSet);

  return(NULL);
}


/* Frees an existing set */
Int4
SLRI_SetClear(SLRI_SetPtr spSet)
{
  SLRI_BucketElemPtr bepCurr = NULL, bepNext = NULL;
  Int4 i;

  if (spSet == NULL) {
    ErrPostEx(SEV_ERROR, 0, 0, "SLRI_SetClear: spSet is NULL");
    return(-1);
  }

  /* Free buckets */
  for (i = 0; i < spSet->numBuckets; i++) {
    if (spSet->buckets[i] != NULL) {
      bepCurr = spSet->buckets[i];
      while(bepCurr != NULL) {
        bepNext = bepCurr->next;
        bepCurr = MemFree(bepCurr);
        bepCurr = bepNext;
      }
      spSet->buckets[i] = NULL;
    }
  }

  return(0);
}


/* Inserts a new element into an existing set */
Int4 
SLRI_SetInsertElem(SLRI_SetPtr spSet, VoidPtr vpElem)
{
  SLRI_BucketElemPtr bepCurr = NULL, bepNewElem = NULL;
  Int4 bucket = 0;

  if (spSet == NULL) {
    ErrPostEx(SEV_ERROR, 0, 0, "SLRI_SetInsertElem: spSet is NULL");
    return(-1);
  }
  if (vpElem == NULL) {
    ErrPostEx(SEV_ERROR, 0, 0, "SLRI_SetInsertElem: vpElem is NULL");
    return(-1);
  }

  /* Get key from hashing function */
  bucket = hash(spSet, vpElem);

  /* Check to see if element is already in hash */
  for (bepCurr = spSet->buckets[bucket]; bepCurr != NULL; bepCurr = bepCurr->next) {
    if ((*(spSet->Comparator))(bepCurr->datum, vpElem)) {
      /* Element already in, so exit */
      return(0);
    }
  }

  /* If not, allocate and insert new element */
  bepNewElem = (SLRI_BucketElemPtr)MemNew(sizeof(SLRI_BucketElem));

  if (bepNewElem == NULL) {
    ErrPostEx(SEV_ERROR, 0, 0, "SLRI_SetInsertElem: bepNewElem is NULL");
    return(-1);
  }

  bepNewElem->datum = vpElem;
  bepNewElem->next = spSet->buckets[bucket];
  spSet->buckets[bucket] = bepNewElem;

  return(0);
}


/* Deletes an element from an existing set */
Int4 
SLRI_SetDeleteElem(SLRI_SetPtr spSet, VoidPtr vpElem)
{
  SLRI_BucketElemPtr bepCurr = NULL, bepPrev = NULL;
  Boolean foundElem = FALSE;
  Int4 bucket;

  if (spSet == NULL) {
    ErrPostEx(SEV_ERROR, 0, 0, "SLRI_SetDeleteElem: spSet is NULL");
    return(-1);
  }
  if (vpElem == NULL) {
    ErrPostEx(SEV_ERROR, 0, 0, "SLRI_SetDeleteElem: vpElem is NULL");
    return(-1);
  }

  /* Get key from hashing function */
  bucket = hash(spSet, vpElem);

  if ((bepCurr = spSet->buckets[bucket]) == NULL) {
    /* Bucket empty */
    return(0);
  }

  if ((*(spSet->Comparator))(bepCurr->datum, vpElem)) {
    /* Elem to be deleted is first one */
    spSet->buckets[bucket] = bepCurr->next;
    bepCurr = MemFree(bepCurr);
  }
  else {
    /* Cycle through rest of bucket to find elem */
    while (bepCurr->next != NULL) {
      bepPrev = bepCurr;
      bepCurr = bepCurr->next;

      if ((*(spSet->Comparator))(bepCurr->datum, vpElem)) {
        foundElem = TRUE;
        break;
      }
    }

    if (foundElem) {
      /* Close linked list where elem used to be */
      bepPrev->next = bepCurr->next;
      bepCurr = MemFree(bepCurr);
    }
  }

  return(0);
}


/* Checks if set is empty */
Boolean 
SLRI_SetIsEmpty(SLRI_SetPtr spSet)
{
  Int4 i;

  if (spSet == NULL) {
    ErrPostEx(SEV_ERROR, 0, 0, "SLRI_SetIsEmpty: spSet is NULL");
    return(TRUE);
  }

  for (i = 0; i < spSet->numBuckets; i++) {
    if (spSet->buckets[i] != NULL) {
      return(FALSE);
    }
  }

  return(TRUE);
}

/* Returns number of elements in given set (returns -1 for error) */
Int4
SLRI_SetCount(SLRI_SetPtr spSet)
{
  SLRI_BucketElemPtr bep = NULL;
  Int4 i=0, count=0;

  if (spSet == NULL) {
    ErrPostEx(SEV_ERROR, 0, 0, "SLRI_SetCount: spSet is NULL");
    return(-1);
  }

  /* iterate through buckets */
  for (i = 0; i < spSet->numBuckets; i++) {
    if (spSet->buckets[i] != NULL) {
      bep = spSet->buckets[i];
      while(bep != NULL) {
        count++;
        bep = bep->next;
      }
    }
  }

  return(count);
}

/* Checks if an element is a member of a given set */
/* NOTE: vpMemberElem is optional (can be NULL) */
Boolean 
SLRI_SetIsMember(SLRI_SetPtr spSet, VoidPtr vpElem, VoidPtr *vpMemberElem)
{
  SLRI_BucketElemPtr bepCurr = NULL;
  Int4 bucket;

  if (spSet == NULL) {
    ErrPostEx(SEV_ERROR, 0, 0, "SLRI_SetIsMember: spSet is NULL");
    return(FALSE);
  }
  if (vpElem == NULL) {
    ErrPostEx(SEV_ERROR, 0, 0, "SLRI_SetIsMember: vpElem is NULL");
    return(FALSE);
  }

  /* Get key from hashing function */
  bucket = hash(spSet, vpElem);

  for (bepCurr = spSet->buckets[bucket]; bepCurr != NULL; bepCurr = bepCurr->next) {
    if ((*(spSet->Comparator))(bepCurr->datum, vpElem)) {
      if (vpMemberElem != NULL) {
        /* Return matched element */
        *vpMemberElem = bepCurr->datum;
      }
      return(TRUE);
    }
  }

  return(FALSE);
}


/* Copies an existing set */
/* NOTE: destination set does not inherit set datum from source set */
Int4 
SLRI_SetCopy(SLRI_SetPtr spDest, SLRI_SetPtr spSource)
{
  SLRI_BucketElemPtr bepCurr = NULL, bepNewElem = NULL;
  Int4 i, bucket;

  if (spDest == NULL) {
    ErrPostEx(SEV_ERROR, 0, 0, "SLRI_SetCopy: spDest is NULL");
    return(-1);
  }
  if (spSource == NULL) {
    ErrPostEx(SEV_ERROR, 0, 0, "SLRI_SetCopy: spSource is NULL");
    return(-1);
  }

  if (SLRI_SetClear(spDest) != 0) {
    ErrPostEx(SEV_ERROR, 0, 0, "SLRI_SetCopy: Error clearing destination set");
    return(-1);
  }

  /* Copy buckets */
  for (i = 0; i < spSource->numBuckets; i++) {
    if (spSource->buckets[i] == NULL) {
      continue;
    }

    for (bepCurr = spSource->buckets[i]; bepCurr != NULL; bepCurr = bepCurr->next) {
      /* Get key from hashing function */
      bucket = hash(spDest, bepCurr->datum);

      bepNewElem = (SLRI_BucketElemPtr)MemNew(sizeof(SLRI_BucketElem));

      if (bepNewElem == NULL) {
        ErrPostEx(SEV_ERROR, 0, 0, "SLRI_SetCopy: Error allocating new element");
        return(-1);
      }

      bepNewElem->datum = bepCurr->datum;
      bepNewElem->next = spDest->buckets[bucket];
      spDest->buckets[bucket] = bepNewElem;
    }
  }

  return(0);
}


/* Returns union of two sets (OR) */
/* NOTE: - resulting set does not inherit set data from operand sets */
/*       - passed function is optional (can be NULL); operates on ANDed elements */
Int4
SLRI_SetUnite(SLRI_SetPtr spUnion, 
              SLRI_SetPtr spSet1, SLRI_SetPtr spSet2,
              void (*ANDElems)(VoidPtr *vpInsertionElem, VoidPtr vpMatchedElem))
{
  SLRI_BucketElemPtr bepCurr = NULL;
  VoidPtr vpMemberElem = NULL;
  Int4 i;

  if (spUnion == NULL) {
    ErrPostEx(SEV_ERROR, 0, 0, "SLRI_SetUnite: spUnion is NULL");
    return(-1);
  }
  if (spSet1 == NULL) {
    ErrPostEx(SEV_ERROR, 0, 0, "SLRI_SetUnite: spSet1 is NULL");
    return(-1);
  }
  if (spSet2 == NULL) {
    ErrPostEx(SEV_ERROR, 0, 0, "SLRI_SetUnite: spSet2 is NULL");
    return(-1);
  }

  /* Initialize spUnion with first set */
  if (SLRI_SetCopy(spUnion, spSet1) != 0) {
    ErrPostEx(SEV_ERROR, 0, 0, "SLRI_SetUnite: Error copying spSet1 into spUnion");
    return(-1);
  }

  for (i = 0; i < spSet2->numBuckets; i++) {
    if (spSet2->buckets[i] == NULL) {
      continue;
    }

    for (bepCurr = spSet2->buckets[i]; bepCurr != NULL; bepCurr = bepCurr->next) {
      if (!SLRI_SetIsMember(spUnion, bepCurr->datum, &vpMemberElem)) {
        /* Insert element into union */
        if (SLRI_SetInsertElem(spUnion, bepCurr->datum) != 0) {
          ErrPostEx(SEV_ERROR, 0, 0, "SLRI_SetUnite: Error inserting element into union");
          return(-1);
        }
      }
      else {
        /* Element already member, so treat as if ANDing */
        if (ANDElems != NULL) {
          /* Process elements with supplied function (vpMemberElem is destination) */
          (*ANDElems)(vpMemberElem, bepCurr->datum);
        }
      }
    }
  }

  return(0);
}


/* Returns intersection of two sets (AND) */
/* NOTE: - resulting set does not inherit set data from operand sets */
/*       - passed function is optional (can be NULL); operates on ANDed elements */
Int4
SLRI_SetIntersect(SLRI_SetPtr spIntersection, 
                  SLRI_SetPtr spSet1, SLRI_SetPtr spSet2,
                  void (*ANDElems)(VoidPtr *vpInsertionElem, VoidPtr vpMatchedElem))
{
  SLRI_BucketElemPtr bepCurr = NULL;
  VoidPtr vpMemberElem = NULL;
  Int4 i;

  if (spIntersection == NULL) {
    ErrPostEx(SEV_ERROR, 0, 0, "SLRI_SetIntersect: spIntersection is NULL");
    return(-1);
  }
  if (spSet1 == NULL) {
    ErrPostEx(SEV_ERROR, 0, 0, "SLRI_SetIntersect: spSet1 is NULL");
    return(-1);
  }
  if (spSet2 == NULL) {
    ErrPostEx(SEV_ERROR, 0, 0, "SLRI_SetIntersect: spSet2 is NULL");
    return(-1);
  }

  if (SLRI_SetClear(spIntersection) != 0) {
    ErrPostEx(SEV_ERROR, 0, 0, "SLRI_SetIntersect: Error clearing intersection set");
    return(-1);
  }

  for (i = 0; i < spSet1->numBuckets; i++) {
    if (spSet1->buckets[i] == NULL) {
      continue;
    }

    for (bepCurr = spSet1->buckets[i]; bepCurr != NULL; bepCurr = bepCurr->next) {
      if (SLRI_SetIsMember(spSet2, bepCurr->datum, &vpMemberElem)) {

        if (ANDElems != NULL) {
          /* Process elements with supplied function (bepCurr->datum is destination) */
          (*ANDElems)(bepCurr->datum, vpMemberElem);
        }

        /* Insert ANDed element into intersection set */
        if (SLRI_SetInsertElem(spIntersection, bepCurr->datum) != 0) {
          ErrPostEx(SEV_ERROR, 0, 0, "SLRI_SetIntersect: Error inserting element into intersection");
          return(-1);
        }
      }
    }
  }

  return(0);
}


/* Subtracts s2 from s1 (NOT) */
/* NOTE: resulting set does not inherit set data from operand sets */
Int4
SLRI_SetSubtract(SLRI_SetPtr spSubtraction, SLRI_SetPtr spSet1, SLRI_SetPtr spSet2)
{
  SLRI_BucketElemPtr bepCurr = NULL;
  Int4 i;

  if (spSubtraction == NULL) {
    ErrPostEx(SEV_ERROR, 0, 0, "SLRI_SetSubtract: spSubtraction is NULL");
    return(-1);
  }
  if (spSet1 == NULL) {
    ErrPostEx(SEV_ERROR, 0, 0, "SLRI_SetSubtract: spSet1 is NULL");
    return(-1);
  }
  if (spSet2 == NULL) {
    ErrPostEx(SEV_ERROR, 0, 0, "SLRI_SetSubtact: spSet2 is NULL");
    return(-1);
  }

  if (SLRI_SetClear(spSubtraction) != 0) {
    ErrPostEx(SEV_ERROR, 0, 0, "SLRI_SetSubtract: Error clearing subtraction set");
    return(-1);
  }

  for (i = 0; i < spSet1->numBuckets; i++) {
    if (spSet1->buckets[i] == NULL) {
      continue;
    }

    for (bepCurr = spSet1->buckets[i]; bepCurr != NULL; bepCurr = bepCurr->next) {
      if (!SLRI_SetIsMember(spSet2, bepCurr->datum, NULL)) {
        if (SLRI_SetInsertElem(spSubtraction, bepCurr->datum) != 0) {
          ErrPostEx(SEV_ERROR, 0, 0, "SLRI_SetSubtract: Error inserting element into subtraction");
          return(-1);
        }
      }
    }
  }

  return(0);
}


/* Iterates a given function over all elements in the set */
Int4
SLRI_SetIterateFcn(SLRI_SetPtr spSet, 
                   void (*f)(SLRI_SetPtr spSet, 
                             Boolean *deletedBucketElem, 
                             VoidPtr datum, VoidPtr vpArgs), 
                   VoidPtr vpArgs)
{
  SLRI_BucketElemPtr bepCurr = NULL, bepNext = NULL;
  Boolean deletedBucketElem = FALSE;
  Int4 i;

  if (spSet == NULL) {
    ErrPostEx(SEV_ERROR, 0, 0, "SLRI_SetIterateFcn: spSet is NULL");
    return(-1);
  }
  if (f == NULL) {
    ErrPostEx(SEV_ERROR, 0, 0, "SLRI_SetIterateFcn: Iteration function is NULL");
    return(-1);
  }

  /* Traverse all buckets */
  for (i = 0; i < spSet->numBuckets; i++) {
    /* Set bepCurr to first element in bucket */
    bepCurr = spSet->buckets[i];

    /* Traverse all elements in bucket */
    while (bepCurr != NULL) {
      /* Save next element in case current is deleted by passed function */
      bepNext = bepCurr->next;

      /* Execute iterated function */
      (*f)(spSet, &deletedBucketElem, bepCurr->datum, vpArgs);

      if (!deletedBucketElem) {
        /* Advance normally */
        bepCurr = bepCurr->next;
      }
      else {
        /* Set bepCurr to proper next element */
        bepCurr = bepNext;
        deletedBucketElem = FALSE;
      }
    }
  }

  return(0);
}


/************************/
/* High-Level Functions */
/************************/

/* Initializes:
 *   - Pseudo-prime g_SetPrime for hashing function
 *   - Optimal number of buckets based on entries per bucket and next largest prime
 *
 * 'numVerifyTrials' represents the number of times the probabilistic prime verification
 * algorithm will run to ensure that the resulting number is prime (1-25 is reasonable)
 *
 * NOTE: userFunPack is optional; if NULL, library's global function pack will be used
 *
 */
Int4 
SLRI_InitSetParameters(SLRI_SetFunPackPtr userFunPack, Int4 totalEntries, 
                       Int4 entriesPerBucket, Int4 numVerifyTrials)
{
  SLRI_SetFunPackPtr funPack = NULL;

  if (totalEntries < 1) {
    ErrPostEx(SEV_ERROR, 0, 0, "SLRI_InitSetParameters: totalEntries must be >= 1");
    return(-1);
  }
  if (entriesPerBucket < 1) {
    ErrPostEx(SEV_ERROR, 0, 0, "SLRI_InitSetParameters: entriesPerBucket must be >= 1");
    return(-1);
  }
  if (numVerifyTrials < 1) {
    ErrPostEx(SEV_ERROR, 0, 0, "SLRI_InitSetParameters: numVerifyTrials must be >= 1");
    return(-1);
  }

  /* Generate pseudo-prime for hashing function */
  /* (Smallest prime > (Total # entries / 0.8)) */
  g_SetPrime = SLRI_PRIME_NextPrime((Int4)(totalEntries / 0.8), numVerifyTrials);

  /* Select which function pack to use (user's or system's) */
  if (userFunPack == NULL) {
    funPack = &g_funPack;
  }
  else {
    funPack = userFunPack;
  }

  /* Generate optimal number of buckets */
  if(totalEntries<entriesPerBucket) {
    /*avoid zero buckets when totalEntries is smaller than entriesPerBucket*/
    funPack->numBuckets=1;
  }
  else {
    funPack->numBuckets = (Int4)(totalEntries / entriesPerBucket);
  }

  /* Want number of buckets to be prime for optimal performance */
  if (!SLRI_PRIME_IsPrime(g_funPack.numBuckets, numVerifyTrials)) {
    funPack->numBuckets = SLRI_PRIME_NextPrime(g_funPack.numBuckets, numVerifyTrials);
  }

  /* Initialize function pointers */
  funPack->HashingFcn = NULL;
  funPack->Comparator = NULL;

  return(0);
}


/* Assign functions to global function pack */
/* NOTE: SLRI_InitSetParameters() must be called -before- this function */
Int4
SLRI_AssignGlobalSetFcns(Int4 (*HashingFcn)(VoidPtr vpItem),
                         Boolean (*Comparator)(VoidPtr vpItem1, VoidPtr vpItem2))
{
  if (HashingFcn == NULL) {
    ErrPostEx(SEV_ERROR, 0, 0, "SLRI_AssignGlobalFcns: HashingFcn is NULL");
    return(-1);
  }
  if (Comparator == NULL) {
    ErrPostEx(SEV_ERROR, 0, 0, "SLRI_AssignGlobalFcns: Comparator is NULL");
    return(-1);
  }

  g_funPack.HashingFcn = HashingFcn;
  g_funPack.Comparator = Comparator;

  return(0);
}


/* String hashing function (example) */
/* NOTE: User's hashing function(s) must follow this prototype */
Int4
StringHashingFcn(VoidPtr vpElem)
{
  CharPtr string = NULL;
  Int4 i, num = 0, len = 0;

  if (vpElem == NULL) {
    ErrPostEx(SEV_ERROR, 0, 0, "StringHashingFcn: vpElem is NULL");
    return(-1);
  }

  string = (CharPtr)vpElem;
  len = StringLen(string);

  /* Add up ASCII values of all chars */
  for (i = 0; i < len; i++) {
    num += (Int4)(string[i]);
  }

  return(num % g_SetPrime);
}


/* String comparator (example) */
/* NOTE: User's comparator(s) must follow this prototype */
Boolean
StringComparator(VoidPtr vpElem1, VoidPtr vpElem2)
{
  CharPtr string1 = NULL, string2 = NULL;

  if (vpElem1 == NULL) {
    ErrPostEx(SEV_ERROR, 0, 0, "StringComparator: vpElem1 is NULL");
    return(FALSE);
  }
  if (vpElem2 == NULL) {
    ErrPostEx(SEV_ERROR, 0, 0, "StringComparator: vpElem2 is NULL");
    return(FALSE);
  }

  string1 = (CharPtr)vpElem1;
  string2 = (CharPtr)vpElem2;

  if (StringCmp(string1, string2) == 0) {
    return(TRUE);
  }

  return(FALSE);
}


/* Wrapper for initializing global function pack with string functions */
Int4
SLRI_InitStringFunctionPack(void)
{
  return(SLRI_AssignGlobalSetFcns(StringHashingFcn, StringComparator));
}

/* Integer hashing function (example) */
/* NOTE: User's hashing function(s) must follow this prototype */
Int4
IntHashingFcn(VoidPtr vpElem)
{
  Int4 number = 0;

  if (vpElem == NULL) {
    ErrPostEx(SEV_ERROR, 0, 0, "IntHashingFcn: vpElem is NULL");
    return(-1);
  }

  number = *(Int4Ptr)vpElem;

  return(number % g_SetPrime);
}


/* Integer comparator (example) */
/* NOTE: User's comparator(s) must follow this prototype */
Boolean
IntComparator(VoidPtr vpElem1, VoidPtr vpElem2)
{
  Int4 number1 = 0, number2 = 0;

  if (vpElem1 == NULL) {
    ErrPostEx(SEV_ERROR, 0, 0, "IntComparator: vpElem1 is NULL");
    return(FALSE);
  }
  if (vpElem2 == NULL) {
    ErrPostEx(SEV_ERROR, 0, 0, "IntComparator: vpElem2 is NULL");
    return(FALSE);
  }

  number1 = *(Int4Ptr)vpElem1;
  number2 = *(Int4Ptr)vpElem2;

  if (number1 == number2) {
    return(TRUE);
  }

  return(FALSE);
}

/* Wrapper for initializing global function pack with integer functions */
Int4
SLRI_InitIntFunctionPack(void)
{
  return(SLRI_AssignGlobalSetFcns(IntHashingFcn, IntComparator));
}

/*

$Log: slri_set.c,v $
Revision 1.1.1.4  2002/03/08 09:27:04  root
Automated import from Sourceforge.net

Revision 1.5  2002/03/08 00:43:34  gbader
Fixed bug in SetCreate if totalEntries smaller than bucket size.

Revision 1.4  2002/03/07 17:16:49  gbader
Added a function to count elements in a set.

Revision 1.3  2002/03/06 00:46:54  gbader
Added integer set high-level functions.

Revision 1.2  2002/03/01 23:57:45  gbader
Fixed compiler warnings
Moved generic asn set read/write mode to slri_asn

Revision 1.1.1.1  2001/10/10 22:11:04  kaca
initial import

Revision 1.12  2001/09/06 21:44:04  gbader
Fixed memory leak.

Revision 1.11  2001/08/30 19:16:29  gbader
Moved globals to c file

Revision 1.10  2001/08/30 14:09:13  hrvojic
Changed g_MPrime to g_SetPrime to fix conflict with slri_ti_query

Revision 1.9  2001/08/29 14:09:05  hrvojic
Added high-level functions for init, sample hashing/comparison functions

Revision 1.8  2001/08/14 15:14:23  hrvojic
Added ability to process ANDed elements as intersection/union performed

Revision 1.7  2001/08/08 13:34:00  hrvojic
Set iterator can now adjust its traversal if passed function deletes a bucket element

Revision 1.6  2001/08/02 18:44:30  hrvojic
Additional fixes to DeleteElem, commented out MemFree functions

Revision 1.5  2001/08/01 19:20:53  hrvojic
Fixed small bug in DeleteElem function

Revision 1.4  2001/07/09 15:23:38  hrvojic
Added datum pointer to SLRI_Set struct

Revision 1.3  2001/07/03 14:41:54  hrvojic
Set iterator can now pass arguments to iterated function

Revision 1.2  2001/06/29 18:21:12  hrvojic
Added SLRI_Set function pack

Revision 1.1  2001/06/26 20:59:01  hrvojic
Initial import of slri_set library


*/

