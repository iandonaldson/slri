
/* $Id: nbr.c,v 1.2 2004/07/23 17:14:53 zwang Exp $
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
 

  PROGRAM:  SeqHound Neighbours:  Function Layer 

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

#include <ncbi.h>
#include <seqhound.h>
#include <nbr_db.h>
#include <slri_math.h>
#include <intrez_cfg.h>

#ifndef NBR_LIMIT
#define NBR_LIMIT 100
#endif 


/* Get a BLAST Result record from the NBLAST generated pairwise BLAST result database. */
SLRI_ERR LIBCALL SHoundGetBlastResult (Int4 gi1, Int4 gi2, NBlastResultSetPtr PNTR ppResult)
{
	if(ppResult == NULL) {
		ErrPostEx(SEV_ERROR,0,0,"Invalid Result reference pointer");
		return SLRI_FAIL;
	}

	if(GetBlastResultsFromDB(gi1, gi2,  &(*ppResult))) return SLRI_FAIL;
	return SLRI_SUCCESS;
}

/* Get a BLAST Result record from the NBLAST generated pairwise BLAST result database in seqalign format */
SLRI_ERR LIBCALL SHoundGetBlastSeqAlign(Int4 gi1, Int4 gi2, SeqAlignPtr PNTR psap)
{
	if(psap == NULL) {
		ErrPostEx(SEV_ERROR,0,0,"Invalid SeqAlign reference pointer");
		return SLRI_FAIL;
	}

	GetBlastSeqAlignFromDB(gi1, gi2, &(*psap));
	
	return SLRI_SUCCESS;
}


Boolean LIBCALL HasNeighbours(Int4 gi)
{
	Int4 iNeighbours = 0;
	if(GetNeighboursFromDB(gi,&iNeighbours,NULL)) return FALSE; 
	if(iNeighbours == 0 ) return FALSE;
	return TRUE; 
        
}        

Boolean LIBCALL Has3DNeighbours(Int4 gi)
{
        Int4 immdb = 0, gihere = 0, i = 0;
        FLinkSetPtr flsp = NULL;
 
        if((!SHoundModule("neigdb")) || (!SHoundModule("redundb")) || (!SHoundModule("strucdb"))){
                ErrPostEx(SEV_ERROR,0,0, "SHound3DNeighboursFromGiEx: Missing required SeqHound module.");
                return SLRI_FAIL;
        }

        if( (gi < 0) ) {
                ErrPostEx(SEV_ERROR, 0, 0, "Has3DNeighbours: Invalid GI.");
                return SLRI_FAIL;
        }
  
        /* First Get Neighbours for the gi*/
        SHoundNeighboursFromGi(gi, &flsp,0.01,FALSE);
 
        /* Find whether neighbours have structures and add to vnp lists*/
        if(flsp != NULL) {
                while(i < flsp->num) {
                        gihere = flsp->uids[i];
                        immdb = SHound3DFromGi(gihere);
                        if(immdb > 0) {
				flsp = FLinkSetFree(flsp);
                                return TRUE;
                        }
                        i++;
                        immdb = 0;
                        gihere = 0;
                }
                flsp = FLinkSetFree(flsp);
        }
        return FALSE;
}        



/* Find a neighbour record in neighbour database */
SLRI_ERR LIBCALL SHoundNeighboursFromGiEx(Int4 gi, FLinkSetPtr PNTR pNeighbours, FloatHi evalue, Boolean bLimit)
{
	NBlastGiAndEvalPtr      nbgep = NULL;
	NBlastGiAndEvalSetPtr   nbgesp = NULL;
	FLinkSetPtr             flspHere = NULL, flsp = NULL;
	Int4                    num = 0, i = 0, index = 0;

	if(!SHoundModule("neigdb")){
     ErrPostEx(SEV_ERROR,0,0, "SHoundNeighboursFromGiEx: Missing required SeqHound module.");
     return SLRI_FAIL;
	}

	if(gi < 0) {
		ErrPostEx(SEV_ERROR,0,0, "SHoundNeighboursFromGi: Invalid GI.");
		return SLRI_FAIL;
	}
	if(pNeighbours == NULL) {
		ErrPostEx(SEV_ERROR, 0, 0, "SHoundNeighboursFromGi: Address of passed flsp is NULL (you must pass a variable by reference).");
		return SLRI_FAIL;
	}
	
	/* Get Struc from NBlastDB */
	if(GetNeighboursFromDB(gi, &num, &nbgesp)) {
		return SLRI_FAIL;
	}
	
	if((nbgesp != NULL) && ((nbgep = nbgesp->giandevals) != NULL)) {
		/* Determine number in this nbrsp in case cleanup program not run */
		Misc_GenericGetNumberofNodes(nbgep, &num);

		if(num > 0) {
			if((flspHere = FLinkSetNew2(num)) == NULL) {
				ErrPostEx(SEV_ERROR,0,0, "SHoundNeighboursFromGi: Could not allocate memory for FLinkSetPtr.");
				nbgesp = NBlastGiAndEvalSetFree(nbgesp);
				return SLRI_FAIL;
			}
			/* Assign the values from the result set to the FLinkSet arrays */
			while(nbgep !=NULL) {
				if(nbgep->evalue <= evalue) {
					flspHere->uids[index] = nbgep->subject_gi;
					flspHere->weights[index] = nbgep->evalue;
					index++;
				} else {
					break;
				}
				if(bLimit == TRUE) {
					if(index >= NBR_LIMIT) 	break;
				}
				nbgep = nbgep->next;
			}
		} 
	}

	if(index > 0) { /*  neighbours with specified evalue*/
		if((flsp = FLinkSetNew2(index)) == NULL) {
			ErrPostEx(SEV_ERROR,0,0, "SHoundNeighboursFromGiEx: Could not allocate memory for FLinkSetPtr flsp.");
			FLinkSetFree(flspHere);
			NBlastGiAndEvalSetFree(nbgesp);
			return SLRI_FAIL;
		}
		flsp->num = index;
		for(i=0; i < index; i++) {
			flsp->uids[i] = flspHere->uids[i];
			flsp->weights[i] = flspHere->weights[i];
		}

		*pNeighbours = flsp;
	}
	if(nbgesp) nbgesp = NBlastGiAndEvalSetFree(nbgesp);
	if(flspHere) flspHere = FLinkSetFree(flspHere);

	return SLRI_SUCCESS;
}

/* Redundancy layer to get Neighbours from a GI */
SLRI_ERR LIBCALL SHoundNeighboursFromGi(Int4 gi, FLinkSetPtr PNTR pNeighbours, FloatHi evalue, Boolean bLimit)
{
	ValNodePtr  vnp = NULL, vnpHead = NULL;
	FLinkSetPtr flspHere = NULL;

	if((!SHoundModule("neigdb")) || (!SHoundModule("redundb"))){
     ErrPostEx(SEV_ERROR,0,0, "SHoundNeighboursFromGi: Missing required SeqHound module.");
     return SLRI_FAIL;
	}

	/* Try using gi to get neighbours from database */
	if(SHoundNeighboursFromGiEx(gi, &flspHere, evalue, bLimit)) {
		return SLRI_FAIL;
	}
	if(flspHere == NULL) {
		/* Try using redundant group gi's */
		vnp = SHoundRedundantGroup(gi);
		vnpHead = vnp;
		while(vnp != NULL) {
			if(SHoundNeighboursFromGiEx(vnp->data.intvalue, &flspHere, evalue, bLimit)) {
				ValNodeFree(vnpHead);
				FLinkSetFree(flspHere);
				return SLRI_FAIL;
			}
			if(flspHere != NULL) {
				break;
			}	
			vnp = vnp->next;
		}
		vnpHead = ValNodeFree(vnpHead);
	}
	*pNeighbours = flspHere;
	return SLRI_SUCCESS;
}


SLRI_ERR LIBCALL SHoundNeighboursFromGiList(ValNodePtr pvngilist, FLinkSetListPtr PNTR pNeighboursList, FloatHi evalue)
{
	ValNodePtr          pvnHere = NULL;
	FLinkSetPtr         flsp = NULL;
	FLinkSetListPtr     flslpHere = NULL, flslpLast = NULL, flslpHead = NULL;

	if((!SHoundModule("neigdb")) || (!SHoundModule("redundb"))){
     ErrPostEx(SEV_ERROR,0,0, "SHoundNeighboursFromGilist: Missing required SeqHound module.");
     return SLRI_FAIL;
	}

	if(pvngilist == NULL) {
		ErrPostEx(SEV_ERROR,0,0, "SHoundNeighboursList: Invalid pvn gilist.");
		return SLRI_FAIL;
	}
	if(pNeighboursList == NULL) {
		ErrPostEx(SEV_ERROR, 0, 0, "SHoundNeighboursFromGiList: Address of passed pNeighbourList is null (you must pass a variable by reference).");
		return SLRI_FAIL;
	}

	/* Iterate through gi list and get neighbours*/
	for(pvnHere = pvngilist; pvnHere != NULL; pvnHere = pvnHere->next) {
		flsp = NULL;
		if(SHoundNeighboursFromGi(pvnHere->data.intvalue, &flsp, evalue, FALSE)) {
			flslpHead = FLinkSetListFree(flslpHead);
			return SLRI_FAIL;
		}
		if(flsp != NULL) {
			if((flslpHere = FLinkSetListNew()) == NULL) {
				ErrPostEx(SEV_ERROR, 0, 0, "Could not allocate Memory for flslpHere");
				FLinkSetFree(flsp);
				FLinkSetListFree(flslpHead);
				return SLRI_FAIL;
			}
			if(flslpHead == NULL) {
				flslpHead = flslpHere;
			}
			flslpHere->id = pvnHere->data.intvalue;
			flslpHere->set1 = flsp;
			if(flslpLast) {
				flslpLast->next = flslpHere;
			}
			flslpLast = flslpHere;
			flslpHere = NULL;
		}
	}

	*pNeighboursList = flslpHead;
	return SLRI_SUCCESS;
}


SLRI_ERR LIBCALL SHoundNeighboursFromTaxID(Int4 TaxID, FLinkSetListPtr PNTR pNeighboursList, FloatHi evalue)
{
	ValNodePtr			pvngilist = NULL, pvngilistHead = NULL;
	FLinkSetListPtr		flslpHere = NULL;

	if((!SHoundModule("neigdb")) || (!SHoundModule("redundb")) || (!SHoundModule("gendb"))){
     ErrPostEx(SEV_ERROR,0,0, "SHoundNeighboursFromTaxID: Missing required SeqHound module.");
     return SLRI_FAIL;
	}

	if(TaxID < 0) {
		ErrPostEx(SEV_ERROR,0,0, "SHoundNeighboursFromTaxID: Invalid TaxID.");
		return SLRI_FAIL;
	}
	if(pNeighboursList == NULL) {
		ErrPostEx(SEV_ERROR, 0, 0, "SHoundNeighboursFromTaxID: Address of passed flslp is null (you must pass a variable by reference).");
		return SLRI_FAIL;
	}

	if((pvngilist = SHoundProteinsFromOrganism(TaxID, CHROM_ALL)) == NULL) {
		ErrPostEx(SEV_ERROR,0,0,"SHoundNeighboursFromTaxID: SHoundProteinsFromOrganism returned NULL.");
		return SLRI_FAIL;
	}
	
	pvngilistHead = pvngilist;
	if(SHoundNeighboursFromGiList(pvngilist, &flslpHere, evalue)) {
		pvngilistHead = ValNodeFree(pvngilistHead);
		return SLRI_FAIL;
	}

	pvngilistHead = ValNodeFree(pvngilistHead);
	*pNeighboursList = flslpHere;
	
	return SLRI_SUCCESS;
}


/* This function returns the first neighbours of the gi in the first list set, */
/* and then the neighbours of the first set follows in the next link sets */
SLRI_ERR LIBCALL SHoundNeighboursOfNeighbours(Int4 gi, FLinkSetListPtr PNTR pGINeighboursList, FloatHi evalue)
{
	FLinkSetPtr	    flspList = NULL, flspNBR = NULL;
	FLinkSetListPtr	flslpHere = NULL, flslpHead = NULL, flslpLast = NULL;
	Int4			i = 0;

	if((!SHoundModule("neigdb")) || (!SHoundModule("redundb"))){
     ErrPostEx(SEV_ERROR,0,0, "SHoundNeighboursOfNeighbours: Missing required SeqHound module.");
     return SLRI_FAIL;
	}

	if(gi < 0) {
		ErrPostEx(SEV_ERROR,0,0, "SHoundNeighboursOfNeighbours: Invalid gi.");
		return SLRI_FAIL;
	}
	if(pGINeighboursList == NULL) {
		ErrPostEx(SEV_ERROR, 0, 0, "SHoundNeighboursOfNeighbours: Address of passed pointers are null (you must pass a variable by reference).");
		return SLRI_FAIL;
	}

	/* Get Neighbours */
	if(SHoundNeighboursFromGi(gi, &flspList, evalue, TRUE)) {
		return SLRI_FAIL;
	}
	if(flspList != NULL) {
		if((flslpHead = FLinkSetListNew()) == NULL) {
			ErrPostEx(SEV_ERROR, 0, 0, "Could not allocate Memory for flslpHead");
			FLinkSetFree(flspList);
			return SLRI_FAIL;
		}
		flslpHead->id = gi;
		flslpHead->set1 = flspList;
		flslpLast = flslpHead;
		
		/* Iterate through list */
		for(i=0; i < flspList->num; i++) {
			/* Get neighbours */
			flspNBR = NULL;
			if(SHoundNeighboursFromGi(flspList->uids[i], &flspNBR, evalue, TRUE)) {
				FLinkSetListFree(flslpHead);
				return SLRI_FAIL;
			}
			/* Add to list */
			if(flspNBR != NULL) {
				if((flslpHere = FLinkSetListNew()) == NULL) {
					ErrPostEx(SEV_ERROR, 0, 0, "Could not allocate Memory for flslpHere");
					FLinkSetFree(flspNBR);
					FLinkSetListFree(flslpHead);
					return SLRI_FAIL;
				}
				flslpHere->id = flspList->uids[i];
				flslpHere->set1 = flspNBR;
				flslpLast->next = flslpHere;
				flslpLast = flslpHere;
				flspNBR = NULL;
			}
		}
	} 

	*pGINeighboursList = flslpHead;
	return SLRI_SUCCESS;
}

/* This function returns the first neighbours of the gi list in the first link set, */
/* and then the neighbours of the first set follows in the next link sets */
SLRI_ERR LIBCALL SHoundNeighboursOfNeighboursList(ValNodePtr pvngilist, FLinkSetList2Ptr PNTR ppAllNeighboursList, FloatHi evalue)
{
	ValNodePtr		 pvnHere = NULL;
	FLinkSetListPtr	 pGINeighboursList = NULL;
	FLinkSetList2Ptr pNeighboursListHead = NULL, pNeighboursListHere = NULL, pNeighboursListLast = NULL;

	if((!SHoundModule("neigdb")) || (!SHoundModule("redundb"))){
     ErrPostEx(SEV_ERROR,0,0, "SHoundNeighboursOfNeighboursList: Missing required SeqHound module.");
     return SLRI_FAIL;
	}

	if(pvngilist == NULL) {
		ErrPostEx(SEV_ERROR,0,0, "SHoundNeighboursOfNeighboursList: Invalid pvn gilist.");
		return SLRI_FAIL;
	}
	if(ppAllNeighboursList == NULL) {
		ErrPostEx(SEV_ERROR, 0, 0, "SHoundNeighboursOfNeighboursList: Address of passed flsl2p is null (you must pass a variable by reference).");
		return SLRI_FAIL;
	}
	/* Iterate through gilist */
	for(pvnHere = pvngilist; pvnHere != NULL; pvnHere = pvnHere->next) {
		/* Get the neighbours */
		pGINeighboursList = NULL;
		if(SHoundNeighboursOfNeighbours(pvnHere->data.intvalue, &pGINeighboursList, evalue)) {
			return SLRI_FAIL;
		}
		/*If neighbours, add to list */
		if(pGINeighboursList != NULL) {
			if(pNeighboursListHead == NULL) {
				if((pNeighboursListHead = FLinkSetList2New()) == NULL) {
					ErrPostEx(SEV_ERROR,0,0, "SHoundNeighboursOfNeighboursList: Could not allocate memory for FLinkSet2.");
					FLinkSetListFree(pGINeighboursList);
					return SLRI_FAIL;
				}
				pNeighboursListHead->id = pvnHere->data.intvalue;
				pNeighboursListHead->set2 = pGINeighboursList;
				pNeighboursListLast = pNeighboursListHead;
			} else {
				if((pNeighboursListHere = FLinkSetList2New()) == NULL) {
					ErrPostEx(SEV_ERROR,0,0, "SHoundNeighboursOfNeighboursList: Could not allocate memory for FLinkSet2.");
					FLinkSetListFree(pGINeighboursList);
					FLinkSetList2Free(pNeighboursListHead);
					return SLRI_FAIL;
				}
				pNeighboursListHere->id = pvnHere->data.intvalue;
				pNeighboursListHere->set2 = pGINeighboursList;
				pNeighboursListLast->next = pNeighboursListHere;
				pNeighboursListLast = pNeighboursListHere;
				pNeighboursListHere = NULL;
			} /* All Neighbours List */
		} /* First Neighbours List */
	}

	*ppAllNeighboursList = pNeighboursListHead;
	return SLRI_SUCCESS;
}

SLRI_ERR LIBCALL SHoundNeighboursOfNeighboursFromTaxID(Int4 TaxID, FLinkSetList2Ptr PNTR ppAllNeighboursList, FloatHi evalue)
{
	ValNodePtr			pvngilist = NULL, pvngilistHead = NULL;
	FLinkSetList2Ptr		flsl2pHere = NULL;

	if((!SHoundModule("neigdb")) || (!SHoundModule("redundb")) || (!SHoundModule("gendb"))){
     ErrPostEx(SEV_ERROR,0,0, "SHoundNeighboursOfNeighboursFromTaxID: Missing required SeqHound module.");
     return SLRI_FAIL;
	}
	
	if(TaxID < 0) {
		ErrPostEx(SEV_ERROR,0,0, "SHoundNeighboursOfNeighboursFromTaxID: Invalid TaxID.");
		return SLRI_FAIL;
	}
	if(ppAllNeighboursList == NULL) {
		ErrPostEx(SEV_ERROR, 0, 0, "SHoundNeighboursOfNeighboursFromTaxID: Address of passed flslp is null (you must pass a variable by reference).");
		return SLRI_FAIL;
	}

	if((pvngilist = pvngilistHead = SHoundProteinsFromOrganism(TaxID, CHROM_ALL)) == NULL) {
		ErrPostEx(SEV_ERROR,0,0,"SHoundNeighboursFromTaxID: SHoundProteinsFromOrganism returned NULL.");
		return SLRI_FAIL;
	}

	if(SHoundNeighboursOfNeighboursList(pvngilist, &flsl2pHere, evalue)) {
		pvngilistHead = ValNodeFree(pvngilistHead);
		return SLRI_FAIL;
	}

	pvngilistHead = ValNodeFree(pvngilistHead);
	*ppAllNeighboursList = flsl2pHere;
	
	return SLRI_SUCCESS;
}


SLRI_ERR LIBCALL SHound3DNeighboursFromGiEx(Int4 gi, ILinkSetPtr PNTR p3DNeighbours, FloatHi evalue)
{
	Int4 immdb = 0, gihere = 0, i = 0, j = 0;
	Int4 i3D = 0;
	FLinkSetPtr flsp = NULL;
	ILinkSetPtr ilsp3D = NULL;
	ValNodePtr  vnpGI3D = NULL, vnpGI3DHead = NULL, vnpMMDB3D = NULL, vnpMMDB3DHead = NULL;


	if((!SHoundModule("neigdb")) || (!SHoundModule("redundb")) || (!SHoundModule("strucdb"))){
     ErrPostEx(SEV_ERROR,0,0, "SHound3DNeighboursFromGiEx: Missing required SeqHound module.");
     return SLRI_FAIL;
	}

	if( (gi < 0) && (p3DNeighbours == NULL) ) {
		ErrPostEx(SEV_ERROR, 0, 0, "SHound3DNeighboursFromGiEx: Invalid Parameters.");
		return SLRI_FAIL;
	}

	/* First Get Neighbours for the gi*/
	if(SHoundNeighboursFromGi (gi, &flsp, evalue, FALSE)) {
		return SLRI_FAIL;
	}

	/* Find whether neighbours have structures and add to vnp lists*/
	if(flsp != NULL) {
		while(i < flsp->num) {
			gihere = flsp->uids[i];
			immdb = SHound3DFromGi(gihere);
			if(immdb > 0) {
				Slri_ValNodeAddInt(&vnpGI3D,0, gihere);
				Slri_ValNodeAddInt(&vnpMMDB3D,0,immdb);
			}
			i++;
			immdb = 0;
			gihere = 0;
		}
		flsp = FLinkSetFree(flsp);
	}

	i3D = ValNodeLen(vnpGI3D);
	if(i3D > 0) {
		/* Make new ilsp the right size & copy 3D neighbours */
		if((ilsp3D = ILinkSetNew2(i3D)) == NULL) {
			ErrPostEx(SEV_ERROR, 0, 0, "SHound3DNeighboursFromGiEx: Could not allocate memory to ilsp3D.");
			ValNodeFree(vnpGI3DHead);
			ValNodeFree(vnpMMDB3DHead);
			return SLRI_FAIL;
		}
		vnpGI3DHead = vnpGI3D;
		vnpMMDB3DHead = vnpMMDB3D;
		while(j < ilsp3D->num) {
			ilsp3D->uids[j] = vnpGI3D->data.intvalue;
			ilsp3D->weights[j] = vnpMMDB3D->data.intvalue;
			vnpGI3D=vnpGI3D->next;
			vnpMMDB3D=vnpMMDB3D->next;
			j++;
		}
		vnpGI3DHead = ValNodeFree(vnpGI3DHead);
		vnpMMDB3DHead = ValNodeFree(vnpMMDB3DHead);
	}
	
	*p3DNeighbours = ilsp3D;
	return SLRI_SUCCESS;
}

/* Neighbours from a GI */
SLRI_ERR LIBCALL SHound3DNeighboursFromGi(Int4 gi, ILinkSetPtr PNTR p3DNeighbours, FloatHi evalue)
{
	ILinkSetPtr ilspHere = NULL;

	if((!SHoundModule("neigdb")) || (!SHoundModule("redundb")) || (!SHoundModule("strucdb"))){
     ErrPostEx(SEV_ERROR,0,0, "SHound3DNeighboursFromGi: Missing required SeqHound module.");
     return SLRI_FAIL;
	}

	if(SHound3DNeighboursFromGiEx(gi, &ilspHere, evalue)) {
		return SLRI_FAIL;
	}
	*p3DNeighbours = ilspHere;
	return SLRI_SUCCESS;
}

SLRI_ERR LIBCALL SHound3DNeighboursFromGiList(ValNodePtr pvngilist, ILinkSetListPtr PNTR p3DNeighboursList, FloatHi evalue)
{
	ValNodePtr pvnHere = NULL;
	ILinkSetPtr ilsp = NULL;
	ILinkSetListPtr ilslpHere = NULL, ilslpHead = NULL, ilslpLast = NULL;
	Int4 gi = 0;

	if((!SHoundModule("neigdb")) || (!SHoundModule("redundb")) || (!SHoundModule("strucdb"))){
     ErrPostEx(SEV_ERROR,0,0, "SHound3DNeighboursFromGiList: Missing required SeqHound module.");
     return SLRI_FAIL;
	}

	if( (pvngilist == NULL) & (p3DNeighboursList == NULL) ) {
		ErrPostEx(SEV_ERROR, 0, 0, "SHound3DNeighboursList: Invalid Parameters.");
		return SLRI_FAIL;
	}
	
	/*Iterate through the list of gi's*/
	for(pvnHere = pvngilist; pvnHere != NULL; pvnHere = pvnHere->next) {
		/*Get the 3D neighbours */
		ilsp = NULL;
		gi = pvnHere->data.intvalue;
		if(SHound3DNeighboursFromGi(gi, &ilsp, evalue)) {
			return SLRI_FAIL;	
		}
		
		if(ilsp != NULL) {
			/*add to list */
			if((ilslpHere = ILinkSetListNew()) == NULL) {
				ErrPostEx(SEV_ERROR, 0, 0, "SHound3DNeighboursFromGiList: Could not allocate memory to ilslpHere.");
				ILinkSetFree(ilsp);
				ILinkSetListFree(ilslpHead);
				return SLRI_FAIL;
			}

			if(ilslpHead == NULL) {
				ilslpHead = ilslpHere;
			}
			ilslpHere->id = gi;
			ilslpHere->set1 = ilsp;
			if(ilslpLast) {
				ilslpLast->next = ilslpHere;
			}
			ilslpLast = ilslpHere;
			ilslpHere = NULL;
		}
	}
	
	*p3DNeighboursList = ilslpHead;
	return SLRI_SUCCESS;
}


SLRI_ERR LIBCALL SHound3DNeighboursFromTaxID(Int4 TaxID, ILinkSetListPtr PNTR p3DNeighboursList, FloatHi evalue)
{
	ValNodePtr			pvngilist = NULL, pvngilistHead = NULL;
	ILinkSetListPtr		ilslpHere = NULL;

	if((!SHoundModule("neigdb")) || (!SHoundModule("redundb")) || (!SHoundModule("strucdb")) || (!SHoundModule("gendb"))){
     ErrPostEx(SEV_ERROR,0,0, "SHound3DNeighboursFromTaxID: Missing required SeqHound module.");
     return SLRI_FAIL;
	}

	if(TaxID < 0) {
		ErrPostEx(SEV_ERROR,0,0, "SHound3DNeighboursFromTaxID: Invalid TaxID.");
		return SLRI_FAIL;
	}
	if(p3DNeighboursList == NULL) {
		ErrPostEx(SEV_ERROR, 0, 0, "SHound3DNeighboursFromTaxID: Address of passed flsp is null (you must pass a variable by reference).");
		return SLRI_FAIL;
	}

	if((pvngilist = SHoundProteinsFromOrganism(TaxID, CHROM_ALL)) == NULL) {
		ErrPostEx(SEV_ERROR,0,0,"SHound3DNeighboursFromTaxID: SHoundProteinsFromOrganism returned NULL for taxid %ld.",(long)TaxID);
		return SLRI_FAIL;
	}
	
	pvngilistHead = pvngilist;  
	if(SHound3DNeighboursFromGiList(pvngilist, &ilslpHere, evalue)) {
		pvngilistHead = ValNodeFree(pvngilistHead);
		return SLRI_FAIL;
	}

	pvngilistHead = ValNodeFree(pvngilistHead);
	*p3DNeighboursList = ilslpHere;
	return SLRI_SUCCESS;
}

/* Returns a list of GIs with 3D structures that belong to a given TaxID */
SLRI_ERR LIBCALL SHound3DListFromTaxID(Int4 TaxID, ILinkSetPtr PNTR p3DList)
{
	ValNodePtr    pvnGIlist = NULL, pvnGIlistHead = NULL;
	ValNodePtr    pvnGI3D = NULL, pvnGI3DHead = NULL, pvnMMDB3D = NULL, pvnMMDB3DHead = NULL;
	ILinkSetPtr   p3DlistHere = NULL;
	Int4          pvnLength = 0, i = 0, gi = 0, iMMDBid = 0;
	ValNodePtr pvnMMDB3DHere = NULL;
	Boolean bFound = FALSE;

	if((!SHoundModule("redundb")) || (!SHoundModule("strucdb")) || (!SHoundModule("gendb"))){
     ErrPostEx(SEV_ERROR,0,0, "SHound3DListFromTaxID: Missing required SeqHound module.");
     return SLRI_FAIL;
	}

	if(TaxID < 0) {
		ErrPostEx(SEV_ERROR,0,0, "SHound3DListFromTaxID: Invalid TaxID.");
		return SLRI_FAIL;
	}
	if(p3DList == NULL) {
		ErrPostEx(SEV_ERROR, 0, 0, "SHound3DListFromTaxID: Address of passed ilsp is NULL (you must pass a variable by reference).");
		return SLRI_FAIL;
	}

	if((pvnGIlist = pvnGIlistHead = SHoundProteinsFromTaxID(TaxID)) == NULL) { 
		ErrPostEx(SEV_ERROR,0,0,"SHound3DListFromTaxID: SHoundProteinsFromOrganism returned NULL.");
		return SLRI_FAIL;
	}
	
	while(pvnGIlist != NULL) {	
		gi = pvnGIlist->data.intvalue;
				iMMDBid = SHound3DFromGi(gi);
				if(iMMDBid > 0) {
					/* We'll add a single representative for each mmdb */
					pvnMMDB3DHere = pvnMMDB3D;
					while(pvnMMDB3DHere) {
						if(pvnMMDB3DHere->data.intvalue == iMMDBid) {
							bFound = TRUE;
							break;
						}
						pvnMMDB3DHere = pvnMMDB3DHere->next;
					}
					if(!bFound) {
						Slri_ValNodeAddInt(&pvnGI3D,0, gi);
						Slri_ValNodeAddInt(&pvnMMDB3D,0,iMMDBid);
					}
					bFound = FALSE;
				}
		pvnGIlist = pvnGIlist->next;
	}
	pvnGIlist = pvnGIlistHead = ValNodeFree(pvnGIlistHead);	
	pvnGI3DHead = pvnGI3D;
	pvnMMDB3DHead = pvnMMDB3D;
	pvnLength = ValNodeLen(pvnGI3D);
	if(pvnLength > 0) {
		/* Copy to shorter array */
		if((p3DlistHere = ILinkSetNew2(pvnLength)) == NULL) {
			ErrPostEx(SEV_ERROR, 0, 0, "SHound3DNeighboursFromTaxID: Could not allocate memory to p3DlistHere.");
			ValNodeFree(pvnGI3DHead);
			ValNodeFree(pvnMMDB3DHead);
			return SLRI_FAIL;
		}

		while((pvnGI3D != NULL) && (pvnMMDB3D != NULL)) {
			p3DlistHere->uids[i] = pvnGI3D->data.intvalue;
			p3DlistHere->weights[i] = pvnMMDB3D->data.intvalue;
			pvnGI3D = pvnGI3D->next;
			pvnMMDB3D = pvnMMDB3D->next;
			i++;
		}
	}
	pvnGI3D = pvnGI3DHead = ValNodeFree(pvnGI3DHead);
	pvnMMDB3D = pvnMMDB3DHead = ValNodeFree(pvnMMDB3DHead);
	*p3DList = p3DlistHere;
	return SLRI_SUCCESS;
}

/* Returns a dual array of GIs and the number of neighbours that they have in the NBlastDB*/
SLRI_ERR LIBCALL SHoundGiAndNumNeighboursList (ILinkSetPtr PNTR pNumGiNeighboursList)
{
	Int4 num = 0;
	ILinkSetPtr pNumGiList = NULL;

	if(!SHoundModule("neigdb")){
     ErrPostEx(SEV_ERROR,0,0, "SHoundGiAndNumNeighboursList: Missing required SeqHound module.");
     return SLRI_FAIL;
	}

	if(pNumGiNeighboursList == NULL) {
		ErrPostEx(SEV_ERROR, 0, 0, "SHoundGiAndNumNeighboursList: Address of passed ilsp is NULL (you must pass a variable by reference).");
		return SLRI_FAIL;
	}

	if(GetNumberofNeighboursListFromDB(&num, &pNumGiList, NBLASTDB )) {
		return SLRI_FAIL;
	}

	*pNumGiNeighboursList = pNumGiList;
	return SLRI_SUCCESS;
}

SLRI_ERR LIBCALL SHoundNumNeighboursInDB (Int4 PNTR pnum)
{
	ILinkSetPtr ilsp = NULL;
	Int4 i = 0, total = 0;

	if(!SHoundModule("neigdb")){
     ErrPostEx(SEV_ERROR,0,0, "SHoundNumNeighboursInDB: Missing required SeqHound module.");
     return SLRI_FAIL;
	}

	if(SHoundGiAndNumNeighboursList(&ilsp)) {
		return SLRI_FAIL;
	}
	if(ilsp == NULL) {
		ErrPostEx(SEV_ERROR, 0, 0, "SHoundNumNeighboursInDB: ilsp is NULL.");
		return SLRI_FAIL;
	} 
#ifdef ODBC
	/* for ODBC, use 'select sum(num) from nblastdb'. 
	   ilsp has only one entry with a fake gi whose weight is the total number of neighbours. */
	total = ilsp->weights[0];
#else
	for(i = 0; i < ilsp->num; i++) {
		total += ilsp->weights[i];
	}
#endif
	*pnum = total;
	ILinkSetFree(ilsp);
	return SLRI_SUCCESS;
}
/* Returns a dual array of GIs and the number of neighbours that they have in the NBlastDB*/
SLRI_ERR LIBCALL SHoundGiAndNum3DNeighboursList (ILinkSetPtr PNTR pNumGi3DNeighboursList)
{
	Int4 num = 0;
	ILinkSetPtr pNumGiList = NULL;

	if(!SHoundModule("neigdb")) {
     ErrPostEx(SEV_ERROR,0,0, "SHound3DNeighboursFromTaxID: Missing required SeqHound module.");
     return SLRI_FAIL;
	}

	if(pNumGi3DNeighboursList == NULL) {
		ErrPostEx(SEV_ERROR, 0, 0, "SHoundGiAndNum3DNeighboursList: Address of passed ilsp is NULL (you must pass a variable by reference).");
		return SLRI_FAIL;
	}

	if(GetNumberofNeighboursListFromDB(&num, &pNumGiList, N3D)) {
		return SLRI_FAIL;
	}

	*pNumGi3DNeighboursList = pNumGiList;
	return SLRI_SUCCESS;
}

SLRI_ERR LIBCALL SHoundNum3DNeighboursInDB (Int4 PNTR pnum)
{
	ILinkSetPtr ilsp = NULL;
	Int4 i = 0, total = 0;
 
	if(!SHoundModule("neigdb")) {
     ErrPostEx(SEV_ERROR,0,0, "SHound3DNeighboursFromTaxID: Missing required SeqHound module.");
     return SLRI_FAIL;
	}

	if(pnum == NULL) {
		ErrPostEx(SEV_ERROR, 0, 0, "SHoundNum3DNeighboursInDB: passed num reference is NULL.");
		return SLRI_FAIL;
	}
	if(SHoundGiAndNum3DNeighboursList(&ilsp)) {
		return SLRI_FAIL;
	}
	if(ilsp == NULL) {
		ErrPostEx(SEV_ERROR, 0, 0, "SHoundNum3DNeighboursInDB: ilsp is NULL.");
		return SLRI_FAIL;
	} 
	for(i = 0; i < ilsp->num; i++) {
		total += ilsp->weights[i];
	}
	*pnum = total;
	ILinkSetFree(ilsp);
	return SLRI_SUCCESS;
}


/*
$Log: nbr.c,v $
Revision 1.2  2004/07/23 17:14:53  zwang
Change the logic of getting the total number of neighbours from nblastdb id ODBC to speed it up.

Revision 1.1.1.11  2002/03/04 09:25:29  root
Automated import from Sourceforge.net

Revision 1.12  2002/03/03 23:02:52  gbader
Fixed MSVC compiler warnings - mostly unused variables and uncasted type changes.

Revision 1.11  2002/02/28 17:23:01  micheld
New MSVC project files to build cb/db2/rem apps and libraries
Moved include order to accomodate msvc, removed #ifdef [CODEBASE|DB2]

Revision 1.10  2002/02/12 15:29:25  mdumontier
minor bug fix to free FLinkSet

Revision 1.9  2002/02/12 15:25:52  mdumontier
Has3DNeighbours optimization

Revision 1.8  2001/12/23 20:29:43  micheld
Fixed compiler warnings

Revision 1.7  2001/12/11 16:50:36  kaca
added SeqHound module check to API functions

nbr.c

Revision 1.6  2001/12/07 19:56:53  kaca
added LIBCALL modifier to SHoundGetBlastSeqAlign

Revision 1.5  2001/12/05 18:33:34  micheld
fixed make file, corrected compiler warnings

Revision 1.4  2001/12/05 00:52:07  micheld
Added access to BLAST pairwise results... returns either NBLASTDB format or seqalign

Revision 1.3  2001/11/26 16:12:05  kaca
removed unused variables

Revision 1.2  2001/11/02 19:54:51  micheld
SHound3DListFromTaxID now returns a non-redundant list of mmdbs for that taxonomy

Revision 1.1.1.1  2001/10/10 21:53:48  kaca
initial import

Revision 1.11  2001/09/07 23:37:15  michel
added LIBCALL to functions

Revision 1.10  2001/09/06 22:07:35  michel
removed redundancy in redundancy call in SHoundGet3DList ;-)

Revision 1.9  2001/09/03 20:31:08  michel
Mainly fixed compiler warnings and changed CODE4 reference for GO and LL

Revision 1.8  2001/08/30 21:19:02  michel
neighbour table fixed, reverting to normal processing

Revision 1.7  2001/08/24 20:59:44  michel
removed SEQHOUNDNBRS compile flag from library
added new functions for 3DNeighbours database retrieval
temporary fix for redundant neighbours coming from NBLAST

Revision 1.6  2001/08/15 19:07:37  michel
bug fixes on new functions

Revision 1.5  2001/08/15 18:11:47  michel
Added pathnbr for config file to find neighbour database
Added additional functions and error checking

Revision 1.4  2001/08/14 14:29:59  michel
Fixed and tested Neighbour Functions, with the exception of 3D Neighbours
Tested Get3DListFromTaxID - returns all redundant group's 3d structures

Revision 1.3  2001/08/07 18:54:49  michel
Tested SHoundNeighboursFromGiEx
Added the rest of the remote API and CGI calls (untested)

Revision 1.2  2001/08/02 18:24:25  michel
Added #ifdef SEQHOUNDNBRS to exlude neighbour database and calls until
testing is complete and databases are fully available
Added path info for remotelibraries

Revision 1.1  2001/08/02 16:13:32  michel
Added Neighbour Retrieval to SeqHound local

*/

