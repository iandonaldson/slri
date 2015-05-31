/* $Id: slri_math.h,v 1.1.1.2 2002/07/19 08:29:03 root Exp $
-- ***************************************************************************
--
--  Samuel Lunenfeld Research Institute (SLRI)
--
--  File: slri_math.h
--  Description:  New or SLRI modified Math functions
--
--  Author: Michel Dumontier
--  Date: April 2001
--
--  Hogue Lab - University of Toronto Biochemistry Department
--  Samuel Lunenfeld Research Institute, Mount Sinai Hospital
--
--  Copyright Notice:
--
--  Copyright ©2000 Mount Sinai Hospital (MSH)
--
--  This program is free software; you can redistribute it and/or 
--  modify it under the terms of the GNU General Public License as 
--  published by the Free Software Foundation; either version 2 of 
--  the License, or any later version.
--
--  This program is distributed in the hope that it will be useful, 
--  but WITHOUT ANY WARRANTY; without even the implied warranty of 
--  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
--
--  See the GNU General Public License for more details.
--
--  You should have received a copy of the GNU General Public License 
--  along with this program; if not, write to the
--         Free Software Foundation, Inc., 
--         59 Temple Place, Suite 330, Boston, MA  
--         02111-1307  USA 
--  or visit http://www.gnu.org/copyleft/gpl.html
--
--  SPECIAL EXCEPTIONS
--
--  As a special exception, Mount Sinai Hospital gives permission to 
--  link this program with the following non-GPL programs or libraries, 
--  and distribute the resulting executable, without including the source 
--  code for these in the source distribution:
--
--  a) CodeBase 6.5 or greater from Sequiter Software Inc.
--
--  b) The C or C++ interface to Oracle 8.x or greater, from
--     Oracle Corporation or IBM DB2 UDB.
--
-- ***************************************************************************
*/

#ifndef _SLRI_MATH_
#define _SLRI_MATH_

#include <slri_misc.h>

#ifdef __cplusplus
extern "C" {
#endif

/************************/
/* Function definitions */
/************************/

/*****************
* Math functions *
*****************/
/* Moves a C-style pointer to a vector (starts at [0]) to Fortran-stype pointer (starts at [1]) */
FloatLoPtr MATH_C2FVector(FloatLoPtr pfVec);
/* Moves a C-style pointer to a matrix (starts at [0][0]) to Fortran-stype pointer (starts at 1[1]) */
FloatLoPtr *MATH_C2FMatrix(FloatLoPtr *ppfMatrix, Int4 iVecSize);
/* Moves a Fortran-style pointer to a vector (starts at [1]) to C-stype pointer (starts at [0]) */
FloatLoPtr MATH_F2CVector(FloatLoPtr pfVec);
/* Moves a Fortran-style pointer to a matrix (starts at [1][1]) to C-stype pointer (starts at [0][0]) */
FloatLoPtr *MATH_F2CMatrix(FloatLoPtr *ppfMatrix, Int4 iVecSize);


/* Allocates memory to a 1D Vector[iVecSize] */
FloatLoPtr MATH_VectorNew(Int4 iVecSize);
/* Allocates memory to a 2D Matrix[iVecNum][iVecSize] */
FloatLoPtr *MATH_MatrixNew(Int4 iVecNum, Int4 iVecSize);
/* Frees a Vector */
void MATH_VectorFree(FloatLoPtr PNTR pfVec);
/* Frees a Matrix [iVecNum][iVecSize] */
void MATH_MatrixFree(FloatLoPtr PNTR *ppfMatrix, Int4 iVecNum);


/* Take the log of a FloatHi value, safely */
extern SLRI_ERR SLRI_SafeLog10(FloatHi PNTR pfvalue);
/* Take the EXP10 of a FloatHi value, safely */
extern SLRI_ERR SLRI_SafeExp10(FloatHi PNTR pfvalue);



/* Convert a unique identifier into the two ordinal numbers  */
SLRI_ERR UIDtoORD(Int8 uid, Int4 PNTR pord1, Int4 PNTR pord2);
/* Convert 2 unique ordinal numbers into a unique identifier */
SLRI_ERR ORDtoUID(Int4 ord1, Int4 ord2, Int8 PNTR puid);


#ifdef __cplusplus
}
#endif
#endif		/* _SLRI_MATH_ */

/*
$Log: slri_math.h,v $
Revision 1.1.1.2  2002/07/19 08:29:03  root
Automated import from Sourceforge.net

Revision 1.4  2002/07/18 23:22:31  micheld
Added Principal Components Analysis and supporting functions

Revision 1.3  2001/10/10 22:15:59  kaca
resolved import conflicts


Revision 1.2  2001/09/27 19:23:43  micheld
Moved Hash Table function From NBLAST to library

Revision 1.3  2001/08/08 18:18:01  michel
Moved NBlast Hash table functions into math library

Revision 1.1.1.2  2001/10/10 22:11:04  kaca
initial import

Revision 1.3  2001/08/08 18:18:01  michel
Moved NBlast Hash table functions into math library

Revision 1.2  2001/04/05 18:02:15  michel
Removed SLRILinkset from slrilib, added new to slri_math.[c/h]
Made new Generic object library, slriobj, to include FLinkSet and ILinkSet

Revision 1.1  2001/04/02 18:48:01  michel
Moved math functions to slri_math[c.h]
Added Misc_ReadDBGetGiFromOrd
Added Modified LinkSet (SLRILinkset) for use with FloatHis, and nested Linksetlists
Modified make files to include these new files

*/

