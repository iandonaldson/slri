/* $Id: seq_util.c,v 1.3 2004/05/31 20:36:17 hlieu Exp $
*******************************************************************************************
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


  AUTHORS: Michel Dumontier
           Katerina Michalickova 
		   Christopher W.V. Hogue hogue@mshri.on.ca


  REVISION/CONTRIBUTION NOTES:

  December 1, 2001  Original MSH Revision.
                Hogue Lab - University of Toronto Biochemistry
                Department and the Samuel Lunenfeld Research
                Institute, Mount Sinai Hospital
                http://bioinfo.mshri.on.ca  hogue@mshri.on.ca
                ftp://ftp.mshri.on.ca/


  PUBLICATION TO CITE:


  DESCRIPTION
  December 2001
  API iterators and other utilities

******************************************************************************************/

#include <seqhound.h>


/* Iterators */
#define ITER_TAXON 0
#define ITER_PROTEIN 1
#define ITER_DNA 2
#define ITER_REDUND 3

/*  SHoundIterator */
/*
	This function is to be used to iterate through results from specific API calls contained within using a callback function
	provided by the user.  The user function must have type:
	typedef SLRI_ERR (*SHoundIteratorFunc) PROTO((Int4 iterationnum, Int4 invalue, Pointer inpointer, Pointer PNTR poutpointer));
	where 
		iterationnum refers to how many iterations have been done so far
		invalue refers to the primary value used by the SeqHound API function
		inpointer refers to any values/data structures that a user wants to be able to use in their call back function
		outpointer refers to any values/data structures that a user wants to get out of their callback function
	
*/
SLRI_ERR SHoundIterator(Int2 iteratortype, Int4 ivalue, Pointer inpointer, SHoundIteratorFunc iteratorFunc, Pointer PNTR poutpointer)
{
	ValNodePtr pvnIteratorList = NULL, pvnIteratorListHead = NULL;
	Int4 iterationnum = 0;
	SLRI_ERR err = 0;

	if(iteratortype == ITER_TAXON) {
		if((pvnIteratorList = pvnIteratorListHead = SHoundAllGenomes()) == NULL) {
			ErrPostEx(SEV_ERROR,0,0,"SHoundIterator:  Could not obtain genomes list!");
			return SLRI_FAIL;
		}
	} else if(iteratortype == ITER_PROTEIN) {
		if((pvnIteratorList = pvnIteratorListHead = SHoundProteinsFromOrganism(ivalue, CHROM_ALL)) == NULL) {
			ErrPostEx(SEV_ERROR,0,1,"SHoundIterator:  No Proteins from taxid %ld!",(long) ivalue);
			return SLRI_FAIL;
		}
	} else if(iteratortype == ITER_DNA) {
		if((pvnIteratorList = pvnIteratorListHead = SHoundDNAFromOrganism(ivalue, CHROM_ALL)) == NULL) {
			ErrPostEx(SEV_ERROR,0,2,"SHoundIterator:  No DNA from taxid %ld!",(long) ivalue);
			return SLRI_FAIL;
		}
	} else if(iteratortype == ITER_REDUND) {
		if((pvnIteratorList = pvnIteratorListHead = SHoundRedundantGroup(ivalue)) == NULL) {
			ErrPostEx(SEV_ERROR,0,2,"SHoundIterator:  No RedundantGroup for GI %ld!",(long) ivalue);
			return SLRI_FAIL;
		}
	}
	
	while(pvnIteratorList != NULL) {
		/* CallBack Function */
		if((err = (*iteratorFunc)(iterationnum, pvnIteratorList->data.intvalue, inpointer, &(*poutpointer))) != SLRI_SUCCESS) {
			if (err == SLRI_CONTINUE) {
				ErrPostEx(SEV_INFO,1,0,"SHoundIterator:  User iterator function returned error %ld",(long) err);
			} else if(err == SLRI_FAIL || err == SLRI_NOMEM) {
				ErrPostEx(SEV_ERROR,1,0,"SHoundIterator:  User iterator function returned non-recoverable error %ld",(long) err);
				ValNodeFree(pvnIteratorListHead);
				return err;
			}
		}
		pvnIteratorList = pvnIteratorList->next;
		iterationnum++;
	}
	pvnIteratorListHead = ValNodeFree(pvnIteratorListHead);

	return SLRI_SUCCESS;
}

/* Iterates through the list of complete genomes */
SLRI_ERR SHoundTaxonIterator(Pointer inpointer, SHoundIteratorFunc iteratorFunc, Pointer PNTR poutpointer)
{
	Int4 itaxid = 0; /* not used */
	if(SHoundIterator(ITER_TAXON, itaxid, inpointer, iteratorFunc, &(*poutpointer)) == SLRI_FAIL) ;
	return SLRI_SUCCESS;
}

/* Iterates through the list of protein gis from a given taxonomy id */
SLRI_ERR SHoundProteinIterator(Int4 itaxid, Pointer inpointer, SHoundIteratorFunc iteratorFunc, Pointer PNTR poutpointer)
{
	if(SHoundIterator(ITER_PROTEIN, itaxid, inpointer, iteratorFunc, &(*poutpointer)) == SLRI_FAIL) ;
	return SLRI_SUCCESS;
}

/* Iterates through the list of DNA gis from a given taxonomy id */
SLRI_ERR SHoundDNAIterator(Int4 itaxid, Pointer inpointer, SHoundIteratorFunc iteratorFunc, Pointer PNTR poutpointer)
{
	if(SHoundIterator(ITER_DNA, itaxid, inpointer, iteratorFunc, &(*poutpointer)) == SLRI_FAIL) ;
	return SLRI_SUCCESS;
}

/* Iterates through the redundant group of gis */
SLRI_ERR SHoundRedundGroupIterator(Int4 gi, Pointer inpointer, SHoundIteratorFunc iteratorFunc, Pointer PNTR poutpointer)
{
	if(SHoundIterator(ITER_REDUND, gi, inpointer, iteratorFunc, &(*poutpointer)) == SLRI_FAIL) ;
	return SLRI_SUCCESS;
}


/*
$Log: seq_util.c,v $
Revision 1.3  2004/05/31 20:36:17  hlieu
minor compiler warning fixes for bug 2185

Revision 1.2  2004/01/06 22:32:17  skuznets
Code audit has been done sucessfully.
Possible NULL pointer issues has been eliminated.

Revision 1.1.1.4  2002/03/31 09:30:39  root
Automated import from Sourceforge.net

Revision 1.4  2002/03/30 22:23:14  micheld
added more error checking

Revision 1.3  2002/02/06 23:56:46  micheld
Changed TaxDB_ Functions to SHound*Tax* function call, legacy preserved for C api but *NOT* PERL

Revision 1.2  2001/12/23 20:29:43  micheld
Fixed compiler warnings

Revision 1.1  2001/12/21 22:35:05  micheld
Added taxon, protein, dna, redund Iterator functions with user callback

*/


