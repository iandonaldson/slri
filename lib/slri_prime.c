/* $Id: slri_prime.c,v 1.1.1.1 2001/10/12 08:21:54 root Exp $
***************************************************************************

  Samuel Lunenfeld Research Institute (SLRI)

  File: slri_prime.c
  Description: Prime number library

  Author: Anthony Hrvojic (hrvojic@ieee.org)
  Date: July 2001

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

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <slri_prime.h>


/*
 * The "numTrials" parameter specifies how many times a given number
 * will be checked for primality to ensure a reasonably good
 * prediction is made since the verification algorithm is 
 * probabilistic.
 *
 */

/* Seeds the random number generator */
void 
SLRI_PRIME_RandSeed(void) 
{
   srand(time(0));
}


/* low <= rand_range <= high */
Int4 
SLRI_PRIME_RandRange(Int4 low, Int4 high) 
{
   return (rand() % (high - low + 1)) + low;
}


/* Quickly checks obvious primes before using Miller-Rabin test */
Boolean 
SLRI_PRIME_IsPrime(Int4 n, Int4 numTrials)
{
  Int4 first[] = {2, 3, 5, 7, 11, 13, 17, 19, 23, 0};
  Int4 i;

  /* Check obvious primes first */
  for (i = 0; first[i] != 0; i++) {
    if (n == first[i]) {
      return(TRUE);
    }
  }

  /* Use Miller-Rabin to determine if prime */
  return(SLRI_PRIME_MillerRabinTest(n, numTrials));
}


/* Miller-Rabin probabilistic primality test */
Boolean 
SLRI_PRIME_MillerRabinTest(Int4 n, Int4 numTrials)
{
  Int4 a, j, r, s, y; /* Just formula terms */
  Int4 i;             /* Loop index */

  /* n must be >= 3 and numTrials must t >= 1 to initiate test */
  if (n < 3) {
    return(TRUE);
  }
  if (numTrials < 1) {
    ErrPostEx(SEV_ERROR, 0, 0, "SLRI_PRIME_MillerRabinTest: numTrials must be >= 1");
    return(FALSE);
  }

  /* n must be odd */
  if ((n % 2) == 0) {
    return(FALSE);
  }

  r = 1;

  /* Write n-1 = 2^s*r such that r is odd */
  for (s = SLRI_PRIME_Log2(n); s >= 1; s--) {
    r = (n - 1) / (Int4) EXP2(s);
    if (((r * EXP2(s)) == (n - 1)) && ((r % 2) != 0)) {
      break;
    }
  }  

  /* Begin trials for determining primality */
  for (i = 0; i < numTrials; i++) {
    /* Choose a random int a, 2 <= a <= n-2 */
    do {
      SLRI_PRIME_RandSeed();
      a = SLRI_PRIME_RandRange(2, n - 2);
    } while (a < 2);

    /* Compute y = a^r mod n */
    y = SLRI_PRIME_PowMod(a, r, n);

    /* If composite, return FALSE */
    if ((y != 1) && (y != (n - 1))) {
      j = 1;
      while ((j <= (s - 1)) && (y != (n - 1))) {
        y = SLRI_PRIME_PowMod(y, 2, n);

        if (y == 1) {
          return(FALSE);
        }

        j++;
      }

      if (y != (n - 1)) {
        return(FALSE);
      }
    }
  }

  return(TRUE);
}


/* Finds the smallest prime larger than n */
Int4 
SLRI_PRIME_NextPrime(Int4 n, Int4 numTrials)
{
  Int4 i;

  i = n;
  for (++i; !SLRI_PRIME_IsPrime(i, numTrials); i++)
    ;

  return(i);
}


/* MOD operator that handles negatives properly */
Int4 
SLRI_PRIME_Mod(Int4 a, Int4 n)
{
  Int4 result;

  result = a % n;

  if (result < 0) {
    result += n;
  }

  return(result);
}


/* Computes a^m mod n */
Int4 
SLRI_PRIME_PowMod(Int4 a, Int4 m, Int4 n)
{
  Int4 bits, z;

  z = 1;
  for (bits = 16; bits >= 0; bits--) {
    z = SLRI_PRIME_Mod(z * z, n);

    if ((m & (1 << bits)) != 0) {
      z = SLRI_PRIME_Mod(z * a, n);
    }
  }

  return(z);
}


/* Computes nearest base-2 logarithm */
Int4
SLRI_PRIME_Log2(Int4 n)
{
  Int4 i, bits, num;

  num = n;
  bits = 32;
  for (i = 0; i < bits; i++) {
    if((num >>= 1) == 0) {
      return(i);
    }
  }

  return(i);
}


/*

$Log: slri_prime.c,v $
Revision 1.1.1.1  2001/10/12 08:21:54  root
Automated import from Sourceforge.net

Revision 1.1.1.1  2001/10/10 22:11:04  kaca
initial import

Revision 1.4  2001/08/30 18:34:43  gbader
Fixed msvc compiler warning

Revision 1.3  2001/08/10 14:42:16  hrvojic
Replaced srand48() and lrand48() with srand() and rand() to accomodate Windows

Revision 1.2  2001/07/26 15:42:51  hrvojic
For numbers < 3, Miller-Rabin primality test should return TRUE

Revision 1.1  2001/07/13 16:35:45  hrvojic
Initial import of SLRI_PRIME library


*/






