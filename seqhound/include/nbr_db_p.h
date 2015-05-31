/* $Id: nbr_db_p.h,v 1.1.1.1 2001/10/11 17:37:13 gbader Exp $
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
 

  PROGRAM:  SeqHound Neighbours:  Private Header (CodeBase)
						
  AUTHORS:  Michel Dumontier (micheld@mshri.on.ca)
            and Christopher W.V. Hogue  (hogue@mshri.on.ca)


  REVISION/CONTRIBUTION NOTES:

  March 1, 2001  Original MSH Revision.
                Hogue Lab - University of Toronto Biochemistry 
                Department and the Samuel Lunenfeld Research 
                Institute, Mount Sinai Hospital
                http://bioinfo.mshri.on.ca  hogue@mshri.on.ca
                ftp://ftp.mshri.on.ca/


  PUBLICATION TO CITE:



***************************************************************************
*/
#ifndef _NBR_DB_P_H
#define _NBR_DB_P_H

#ifdef __cplusplus
extern "C" {
#endif

#include <d4all.h>		/* Codebase Header */
#include <nbr_db.h>

/************ CodeBase database variables ************/

/* Main database structure */
extern CODE4 cb;


/* Data file pointers */
DATA4 *pd4NPBlastDB = NULL;   /* Protein Blast Result db */
DATA4 *pd4NPDB      = NULL;   /* Protein Neighbour db */
DATA4 *pd4N3D       = NULL;   /* 3D Neighbours db */

/* Field pointers */
/* Blast DB */
FIELD4 *pf4NPBlastuid  = NULL;
FIELD4 *pf4NPBlasteval = NULL;
FIELD4 *pf4NPBlastasn  = NULL;

/* Neighbours */
FIELD4 *pf4NPord  = NULL;
FIELD4 *pf4NPgi   = NULL;
FIELD4 *pf4NPnum  = NULL;
FIELD4 *pf4NPasn  = NULL;

/* 3D Neighbours */
FIELD4 *pf4N3Dgi  = NULL;
FIELD4 *pf4N3Dnum = NULL;
FIELD4 *pf4N3Dasn = NULL;

/* Blast Protein Results Database */
FIELD4INFO f4iNPBlastDB[]=
{
	{"UID",  r4num,  14,0},    /* Unique Identifier */  
	{"EVAL", r4float,12,7},    /* E value */
	{"ASNSA",  r4memo, 10,0},  /* ASN of modified seqalign */
	{0,0,0,0}
};

/* Neighbour Protein Database */
FIELD4INFO f4iNPDB[] =
{
	{"ORD", r4num,10,0},     /* Unique ordinal number */
	{"GI", r4num,10,0},      /* GI of sequence */
	{"NUM", r4num,10,0},     /* Number of neighbours */
	{"ASNNBR", r4memo,10,0}, /* ASN list of neighbours */
	{0,0,0,0}
};

FIELD4INFO f4iN3D[] =
{
	{"GI", r4num,10,0},      /* GI of sequence */
	{"NUM", r4num,6,0},     /* Number of 3D neighbours */
	{"ASN", r4memo,10,0}, /* ASN list of 3D neighbours */
	{0,0,0,0}
};


/* Searchable tags */
TAG4INFO t4iNPBlastDB[]=
{
	{"UID_TAG","UID",0,r4unique,0}, /* Index by primary key */
	{0,0,0,0,0}
};

TAG4INFO t4iNPDB[]=
{
	{"GI_TAG","GI",0,r4unique,0},
	{"NUM_TAG","NUM",0,0,0},
	{0,0,0,0,0}
};

TAG4INFO t4iN3D[]=
{
	{"GI_TAG","GI",0,r4unique,0},
	{"NUM_TAG","NUM",0,0,0},
	{0,0,0,0,0}
};



#ifdef __cplusplus
}
#endif

#endif /*NBR_DB_P_H*/

/*
$Log: nbr_db_p.h,v $
Revision 1.1.1.1  2001/10/11 17:37:13  gbader
Automated import from Sourceforge.net

Revision 1.1.1.1  2001/10/10 21:53:47  kaca
initial import

Revision 1.4  2001/09/10 20:40:50  michel
made cb variable extern

Revision 1.3  2001/09/03 20:31:08  michel
Mainly fixed compiler warnings and changed CODE4 reference for GO and LL

Revision 1.2  2001/08/24 20:59:44  michel
removed SEQHOUNDNBRS compile flag from library
added new functions for 3DNeighbours database retrieval
temporary fix for redundant neighbours coming from NBLAST

Revision 1.1  2001/08/02 16:13:32  michel
Added Neighbour Retrieval to SeqHound local

*/

