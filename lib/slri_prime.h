/* $Id: slri_prime.h,v 1.1.1.1 2001/10/12 08:21:54 root Exp $
***************************************************************************

  Samuel Lunenfeld Research Institute (SLRI)

  File: slri_prime.h
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

#ifndef _SLRI_PRIME_
#define _SLRI_PRIME_

#include <ncbi.h>

#ifdef __cplusplus
extern "C" {
#endif

  /*
   * The "numTrials" parameter specifies how many times a given number
   * will be checked for primality to ensure a reasonably good
   * prediction is made since the verification algorithm is 
   * probabilistic.
   *
   */

  /* Quickly checks obvious primes before using Miller-Rabin test */
  extern Boolean SLRI_PRIME_IsPrime(Int4 num, Int4 numTrials);

  /* Miller-Rabin probabilistic primality test */
  extern Boolean SLRI_PRIME_MillerRabinTest(Int4 num, Int4 numTrials);

  /* Finds the smallest prime larger than n */
  extern Int4 SLRI_PRIME_NextPrime(Int4 num, Int4 numTrials);

  /* MOD operator that handles negatives properly */
  extern Int4 SLRI_PRIME_Mod(Int4 a, Int4 n);

  /* Computes (a^m) mod n */
  extern Int4 SLRI_PRIME_PowMod(Int4 a, Int4 m, Int4 n);

  /* Computes nearest base-2 logarithm */
  extern Int4 SLRI_PRIME_Log2(Int4 n);

  /* Seeds the randomizer */
  extern void SLRI_PRIME_RandSeed(void);

  /* Generates a pseudo-random number in the specified range */
  extern Int4 SLRI_PRIME_RandRange(Int4 low, Int4 high);


#ifdef __cplusplus
}
#endif

#endif


/*

$Log: slri_prime.h,v $
Revision 1.1.1.1  2001/10/12 08:21:54  root
Automated import from Sourceforge.net

Revision 1.1.1.1  2001/10/10 22:11:04  kaca
initial import

Revision 1.1  2001/07/13 16:35:29  hrvojic
Initial import of SLRI_PRIME library


*/
