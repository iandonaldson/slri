/* $Id: nbr.h,v 1.1.1.4 2002/09/06 08:29:52 root Exp $
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
 

  PROGRAM:  SeqHound Neighbours:  Function Layer Header (USE seqhound.h)

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
#ifndef _NBR_H
#define _NBR_H

#ifdef __cplusplus
extern "C" {
#endif


#include <asn.h>
#include <objNBlastasn.h>
#include <objslrilinkset.h>
#include <slri_misc.h>



/*********************************************************************************
* BLAST Result Retrieval                                                         *
**********************************************************************************
* SHoundGetBlastSeqalign       Get the Blast Hit as a seqalign                   *
* SHoundGetBlastResult         Main function call to return a blast hit          *
*********************************************************************************/
extern SLRI_ERR SHoundGetBlastResult PROTO ((Int4 gi1, Int4 gi2, NBlastResultSetPtr PNTR nbrsp));
extern SLRI_ERR SHoundGetBlastSeqAlign PROTO ((Int4 gi1, Int4 gi2, SeqAlignPtr PNTR psap));


/*********************************************************************************
* Neighbours Retrieval Functions                                                 *
**********************************************************************************
* All Functions return a memory allocated FLinkSet or ILinkset - caller must     *
* use free functions described in objFLinkSet.h or objILinkSet.h                 *
**********************************************************************************

**********************************************************************************
* SEQUENCE Based Retrieval                                                       *
**********************************************************************************
*                                                                                *
* SHoundNeighboursFromGiEx      Main function call to return a list of protein   *
*                                neighbours in for a given gi                    *
* SHoundNeighboursFromGi        Adds GI group redundancy layer                   *
* SHoundNeighboursFromGiList    Gets Protein Neighbours from ValNode GI list     *
* SHoundNeighboursFromTaxID     Gets All Protein Neighbours for each GI for a    *
*                                given taxonomy ID                               *
* SHoundNeighboursOfNeighbours	Retrieves a list of first neighbours with their  *
*                                evalues in a flsp, as well as a list of all the *
*                                neighbours for that GI, with the flslp holding  *
*                                the evalues for the second neighbours           *
* SHoundNeighboursOfNeighboursList	Retrieves a list of first neighbours and a list  *
*                                of all the neighbours, each with the flsp giving*
*                                the evalue for the relationship                 *
* SHoundNumberofNeighboursInList   Returns dual array of GI and the number of       *
*                                neighbours associated with it                   * 
*                                                                                *
*********************************************************************************/
/* Database call */
extern SLRI_ERR SHoundNeighboursFromGiEx PROTO ((Int4 gi, FLinkSetPtr PNTR pNeighbours, FloatHi evalue, Boolean bLimit));
/* Below requires SeqHound --  */
extern SLRI_ERR SHoundNeighboursFromGi PROTO ((Int4 gi, FLinkSetPtr PNTR pNeighbours, FloatHi evalue, Boolean bLimit));
extern SLRI_ERR SHoundNeighboursFromGiList PROTO ((ValNodePtr gilist, FLinkSetListPtr PNTR pNeighboursList, FloatHi evalue));
extern SLRI_ERR SHoundNeighboursFromTaxID PROTO ((Int4 TaxID, FLinkSetListPtr PNTR pNeighboursList, FloatHi evalue)); 
extern SLRI_ERR SHoundNeighboursOfNeighbours PROTO ((Int4 gi, FLinkSetListPtr PNTR pGINeighboursList, FloatHi evalue));
extern SLRI_ERR SHoundNeighboursOfNeighboursList PROTO ((ValNodePtr pvngilist, FLinkSetList2Ptr PNTR pAllNeighboursList, FloatHi evalue));
extern SLRI_ERR SHoundNeighboursOfNeighboursFromTaxID PROTO ((Int4 TaxID, FLinkSetList2Ptr PNTR pAllNeighboursList, FloatHi evalue));

extern SLRI_ERR SHoundListNumberofNeighboursInDB PROTO ((ILinkSetPtr PNTR ilsp));

/*********************************************************************************
* STRUCTURE Based Retrieval                                                      * 
**********************************************************************************
* SHound3DListFromTaxID            Gets all the mmdb id's for 
* SHound3DNeighboursFromGiEx       Gets all neighbour proteins with 3D structure    *
* SHound3DNeighboursFromGi         Adds GI group redundancy layer                   *
* SHound3DNeighboursFromGiList     Takes a Valnode Linked List of gi's and finds    *
*                                their neighbours with 3D structures             *
* SHound3DNeighboursFromTaxID      Retrieves the neighbours with 3D structures      *
*                                for every protein in a genome                   *
**********************************************************************************/
extern SLRI_ERR SHound3DListFromTaxID(Int4 TaxID, ILinkSetPtr PNTR p3DList);

extern SLRI_ERR SHound3DNeighboursFromGiEx PROTO ((Int4 gi, ILinkSetPtr PNTR p3DNeighbours, FloatHi evalue));
extern SLRI_ERR SHound3DNeighboursFromGi PROTO ((Int4 gi, ILinkSetPtr PNTR p3DNeighbours, FloatHi evalue));
extern SLRI_ERR SHound3DNeighboursFromGiList PROTO ((ValNodePtr pvngilist, ILinkSetListPtr PNTR p3DNeighboursList, FloatHi evalue));
extern SLRI_ERR SHound3DNeighboursFromTaxID PROTO ((Int4 TaxID, ILinkSetListPtr PNTR p3DNeighboursList, FloatHi evalue));

/* Function Prototypes Not Yet Implemented */
extern SLRI_ERR SHoundGIListWithout3DStructures PROTO ((Int4 gi, FLinkSetPtr PNTR flsp));
extern SLRI_ERR SHoundGINeighbourListWithout3DStructures (void);

extern Boolean  HasNeighbours(Int4 gi);
extern Boolean  Has3DNeighbours(Int4 gi);

#ifdef __cplusplus
}
#endif

#endif /*_NBR_H*/

/*
$Log: nbr.h,v $
Revision 1.1.1.4  2002/09/06 08:29:52  root
Automated import from Sourceforge.net

Revision 1.4  2002/09/05 18:24:13  micheld
replaced executable include slri.h with slri_misc.h and objslrilinkset.h

Revision 1.3  2002/03/25 07:51:32  micheld
New genome functions. changed includes to new slrilinkset.h

Revision 1.2  2001/12/05 00:52:07  micheld
Added access to BLAST pairwise results... returns either NBLASTDB format or seqalign

Revision 1.1.1.1  2001/10/10 21:53:47  kaca
initial import

Revision 1.3  2001/08/24 20:59:44  michel
removed SEQHOUNDNBRS compile flag from library
added new functions for 3DNeighbours database retrieval
temporary fix for redundant neighbours coming from NBLAST

Revision 1.2  2001/08/07 18:54:49  michel
Tested SHoundNeighboursFromGiEx
Added the rest of the remote API and CGI calls (untested)

Revision 1.1  2001/08/02 16:13:32  michel
Added Neighbour Retrieval to SeqHound local

*/

