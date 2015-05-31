/* $Id: slri_math.c,v 1.1.1.2 2002/07/19 08:29:02 root Exp $
-- ***************************************************************************
--
--  Samuel Lunenfeld Research Institute (SLRI)
--
--  File: slri_math.c
--  Description: New or SLRI modified Math functions
--
--  Author: Michel Dumontier
--  Date: April 20001
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

#include <ncbi.h>
#include <slri_math.h>

/* Replaces LOG(0) with a safe value */
#define SLRI_LOG_OF_0 (-300.00)

/*****************
* Math Functions *
*****************/


/* Moves a C-style pointer to a vector (starts at [0]) to Fortran-stype pointer (starts at [1]) */
FloatLoPtr MATH_C2FVector(FloatLoPtr pfVec)
{
	return pfVec-1;
}
/* Moves a C-style pointer to a matrix (starts at [0][0]) to Fortran-stype pointer (starts at 1[1]) */
FloatLoPtr *MATH_C2FMatrix(FloatLoPtr *ppfMatrix, Int4 iVecSize)
{
	Int4 i = 0;
	for(i = 0; i < iVecSize; i++) {
		ppfMatrix[i] -= 1;
	}
	return ppfMatrix-1;
}

/* Moves a Fortran-style pointer to a vector (starts at [1]) to C-stype pointer (starts at [0]) */
FloatLoPtr MATH_F2CVector(FloatLoPtr pfVec)
{
	return pfVec+1;
}

/* Moves a Fortran-style pointer to a matrix (starts at [1][1]) to C-stype pointer (starts at [0][0]) */
FloatLoPtr *MATH_F2CMatrix(FloatLoPtr *ppfMatrix, Int4 iVecSize)
{
	Int4 i = 0;
	ppfMatrix += 1;
	for(i = 0; i < iVecSize; i++) {
		ppfMatrix[i] += 1;
	}
	return ppfMatrix;
}



/* Allocates memory to a 1D Vector[iVecSize] */
FloatLoPtr MATH_VectorNew(Int4 iVecSize)
{
	FloatLoPtr pf = NULL;
	if(iVecSize <= 0) {
		ErrPostEx(SEV_ERROR,0,0,"Invalid Parameter");
		return NULL;
	}
	if((pf = (FloatLoPtr) MemNew ((size_t) sizeof(FloatLo) * iVecSize)) == NULL) {
		ErrPostEx(SEV_ERROR,0,0,"Not enough memory for vector");
		return NULL;
	}
	return pf;
}

/* Allocates memory to a 2D Matrix[iVecNum][iVecSize] */
FloatLoPtr *MATH_MatrixNew(Int4 iVecNum, Int4 iVecSize)
{
	Int4 i = 0;
	FloatLoPtr *ppfmat = NULL;

	if(iVecNum <= 0 || iVecSize <= 0) {
		ErrPostEx(SEV_ERROR,0,0,"Invalid Parameters");
		return NULL;
	}
	/* Allocate pointers to rows. */
	if((ppfmat = (FloatLoPtr *) MemNew((size_t) sizeof(FloatLoPtr) * iVecNum * iVecSize)) == NULL) {
		ErrPostEx(SEV_ERROR,0,0,"Not enough memory for matrix!");
		return NULL;
	}
	for (i = 0; i < iVecNum; i++) {
		if((ppfmat[i] = MATH_VectorNew(iVecSize)) == NULL) return NULL;
	}

	return ppfmat;
}

/* Frees a Vector */
void MATH_VectorFree(FloatLoPtr PNTR pfVec)
{
	if(pfVec && *pfVec) *pfVec = MemFree(*pfVec);
}

/* Frees a Matrix [iVecNum][iVecSize] */
void MATH_MatrixFree(FloatLoPtr PNTR *ppfMatrix, Int4 iVecNum)
{
	Int4 i = 0;
	if(ppfMatrix && *ppfMatrix) {
		for(i = 0; i < iVecNum; i++) {
			if((*ppfMatrix)[i]) (*ppfMatrix)[i] = MemFree((*ppfMatrix)[i]);
		}
		*ppfMatrix = MemFree(*ppfMatrix);
	}
}



/* Take the log of a FloatHi value, safely */
SLRI_ERR SLRI_SafeLog10(FloatHi PNTR pfvalue)
{
	if(*pfvalue < 0.0 ) {
		ErrPostEx(SEV_INFO,0,0, "SLRI_SafeLog10: Invalid Negative Float Value.");
		return SLRI_FAIL;
	}
	
	if(*pfvalue == 0.0) {
		*pfvalue = SLRI_LOG_OF_0;
	} else {
		*pfvalue = LOG10(*pfvalue);
	}

	return SLRI_SUCCESS;
}

/* Take the EXP10 of a FloatHi value, safely */
SLRI_ERR SLRI_SafeExp10(FloatHi PNTR pfvalue)
{
	if(*pfvalue < SLRI_LOG_OF_0) {
		*pfvalue = SLRI_LOG_OF_0;
	}

	if(*pfvalue > (-SLRI_LOG_OF_0)) {
		*pfvalue = (-SLRI_LOG_OF_0);
	}

	*pfvalue = EXP10(*pfvalue);

	return SLRI_SUCCESS;
}



/************************************************************************
* Hash Table -  Two 32-bit INTs used to generate one 64 bit integer (UID)*
*************************************************************************
* UIDtoORD    Convert a unique identifier into the two ordinal numbers  *
* ORDtoUID    Convert 2 unique ordinal numbers into a unique identifier *
************************************************************************/
SLRI_ERR UIDtoORD(Int8 uid, Int4 PNTR pord1, Int4 PNTR pord2)
{
	Int4 ord1 = 0, ord2 = 0;
	FloatHi ord2f = 0.0;
	
	if(uid < 0){
		ErrPostEx(SEV_ERROR,0,0, "UIDtoORD: Invalid UID.");
		return SLRI_FAIL;
	}

	ord2f = ceil((-1.0+(sqrt(1.0+8.0*(FloatHi)uid)))/2.0);
	ord2 = (Int4) ord2f;
	ord1 = (Int4) (uid -1 - ((Int8)ord2*((Int8)ord2-1)/2));

	if(pord1) *pord1 = ord1;
	if(pord2) *pord2 = ord2;

	return SLRI_SUCCESS;
}

					
SLRI_ERR ORDtoUID(Int4 ord1, Int4 ord2, Int8 PNTR puid)
{
	Int8 uid = 0;

	if( (ord1 < 0) || (ord2 < 0) ) {
		ErrPostEx(SEV_ERROR,0,0, "ORDtoUID: Invalid ordinal numbers.");
		return SLRI_FAIL;
	}
	if(ord1 < ord2) {
		uid = ((Int8)ord2*((Int8)ord2-1))/2+(Int8)ord1+1;
	} else {
		/* this doesn't work */
		uid = 0;
		ErrPostEx(SEV_ERROR,0,0,"ORDtoUID: Ord1 must be smaller than ORD2");
	} 

	if(puid) *puid = uid;

	return SLRI_SUCCESS;
}



/*
$Log: slri_math.c,v $
Revision 1.1.1.2  2002/07/19 08:29:02  root
Automated import from Sourceforge.net

Revision 1.4  2002/07/18 23:22:31  micheld
Added Principal Components Analysis and supporting functions

Revision 1.3  2001/10/10 22:15:59  kaca
resolved import conflicts


Revision 1.2  2001/09/27 19:23:43  micheld
Moved Hash Table function From NBLAST to library

Revision 1.4  2001/09/11 21:45:10  michel
Moved the ilinkset, flinkset, stack, set and prime files into slrilib

Revision 1.3  2001/08/08 18:18:01  michel
Moved NBlast Hash table functions into math library

Revision 1.1.1.2  2001/10/10 22:11:04  kaca
initial import

Revision 1.4  2001/09/11 21:45:10  michel
Moved the ilinkset, flinkset, stack, set and prime files into slrilib

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

