/* $Id: query.cpp,v 1.2 2003/11/03 22:53:24 haocl Exp $ */ 
/***************************************************************************
  Copyright (C) 2001 Mount Sinai Hospital (MSH)
    
  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public License
  as published by the Free Software Foundation; either version 2.1 of
  the License, or any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

  See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with this program; if not, write to the
         Free Software Foundation, Inc.,
         59 Temple Place, Suite 330, Boston, MA
         02111-1307  USA
  or visit http://www.gnu.org/licenses/lgpl.txt

  SPECIAL EXCEPTIONS

  As a special exception, Mount Sinai Hospital gives permission to
  link this program with the following non-GPL programs or libraries,
  and distribute the resulting executable, without including the source
  code for these in the source distribution:

  a) CodeBase 6.5 or greater from Sequiter Software Inc.

  b) The C or C++ interface to Oracle 8.x or greater, from
     Oracle Corporation or IBM DB2 UDB.


  PROGRAM: SLRI C++ Seqhound WebAPI search functions


  AUTHORS: Hao Lieu


  REVISION/CONTRIBUTION NOTES:

  September 1, 2002  Original MSH Revision.
                Hogue Lab - University of Toronto Biochemistry
                Department and the Samuel Lunenfeld Research
                Institute, Mount Sinai Hospital
                http://bioinfo.mshri.on.ca  hogue@mshri.on.ca
                ftp://ftp.mshri.on.ca/


  PUBLICATION TO CITE:

************************************************************************/

#include "logic.hpp"
#include "query.hpp"
#include <corelib/ncbistd.hpp>
#include <corelib/ncbistr.hpp>
#include <intrez.hpp>
#include <iostream>
#include <list>
#include <set>
#include <iterator>
#include <algorithm>
#include <string>
#include "intrez.hpp"
#include "shoundtypes.hpp"

USING_NCBI_SCOPE;
using namespace shound;
using namespace shoundlogic;
BEGIN_SCOPE(shoundquery)
/************************************************************************************/

/* constructor and destructor */
Query::Query() 
{ 
   shound = new Intrez();
}
Query::~Query() 
{ 
   delete(shound);
}

/* Searches for proteins using the search parameters specified
 * by Types, keys and the logic operators. returns the list of results. */
list<int>& 
Query::ExecuteQuery(const list<eQueryType> &Types, list<string>& keys, 
		    const list<SHoundLogic::eLogic> &logics, 
		    const list<string> &ECodeSTL, const list<Int2> &degreeSTL,
		    const eMolType type)
{ 
   /* the 2 lists that we apply logic operators to. STLKeys will
    * hold the return value after each logic has been applied, 
    * tmp will hold the list returned from the intrez layer, 
    * (ie the data from the SeqHound look up containing eg. GI's) */
   list<int> *STLKeys, *tmp;

   /* get the search parameters by iterating through them, 
    * so we need an iterator for each of the lists. */
   list<string>::iterator sKeyIT = keys.begin();
   list<eQueryType>::const_iterator eTQueryIT = Types.begin();
   list<SHoundLogic::eLogic>::const_iterator eLogicIT = logics.begin();
   list<string>::const_iterator sECodeIT = ECodeSTL.begin();
   list<Int2>::const_iterator iDegIT = degreeSTL.begin();

   /* iterate until no more keys found, in each iteration of the
    * loop (except the first), we have 2 lists (STLKeys & tmp) STLKeys
    * is the results from the previous iteration of the loop; tmp is 
    * from the query lookup in SeqHound,  eg. get gi from a taxid, 
    * or gi from a goid, etc... . On these 2 lists we apply the 
    * logic (AND, OR, NOT) and get 1 new list, which becomes the new 
    * STLKeys for the next iteration, or the final return value. */
   for(; sKeyIT != keys.end(); sKeyIT++, eTQueryIT++)
   {
     /* call an appropriate SeqHound function based on whether
      * the identifier is a GOID, TAXID, MMDB, etc ... */
     switch(*eTQueryIT)
     {
        case eQuery_TaxID:             /* identifier is a taxid */ 
	  tmp = shound->GetRecordFromTaxID(*sKeyIT, type);
	  break;
	case eQuery_PubMedID:          /* a PMID */ 
	  tmp = shound->GetRecordFromPMID(*sKeyIT, type);
	  break;
	case eQuery_MMDBID:            /* a MMDB id */
	  tmp = shound->GetRecordFromMMDBID(*sKeyIT, type);
	  break;
	case eQuery_ChromosomeID:      /* a chromosome id */
	  tmp = shound->GetRecordFromChromID(*sKeyIT, type);
	  break;
	case eQuery_GOID:              /* a goid */
	  /* we allow the user to specify the evidence code and a degress */
	  tmp = shound->GetRecordFromGOID(*sKeyIT, type, *sECodeIT, *iDegIT);
	  sECodeIT++; iDegIT++;
	  break;
	case eQuery_OMIMID:            /* a omim */
	  tmp = shound->GetRecordFromOMIMID(*sKeyIT, type);
	  break;
	case eQuery_CDDID:             /* a cdd id */
	  tmp = shound->GetRecordFromCDDID(*sKeyIT, type);
	  break;
	case eQuery_LocusLinkID:       /* a locuslink id */
	  tmp = shound->GetRecordFromLocusLink(*sKeyIT, type);
	  break;
	default:                       /* none of above */
	  /* this should never happen; if it does the data is invalid,
	   * so give an empty list. */
	  tmp = new list<int>(0);
     }
     if(sKeyIT == keys.begin())
     {
	/* first iteration through, so only 1 list,
	 * store the results into STLKeys for next iteration. */
	STLKeys = new list<int>(tmp->size());
	copy(tmp->begin(), tmp->end(), STLKeys->begin());
     }else {
	/* have 2 lists, so apply logic to STLKeys and tmp */
	*STLKeys = SHoundLogic::ApplyLogic(*STLKeys, *tmp, *eLogicIT);
	eLogicIT++;    /* advance to next logic */
     }
   }
   return *STLKeys; 
}
/* use this function as a bridge between seqcmd.cpp and intrez.cpp. 
 * Used to maintain the consistency of interfaces. */
list<CSHoundRecord*>&
Query::RetrieveRecords(list<int> &RequestedGiSTL, list<CSHoundRecord*>& RecordSTL, 
		       eFormatType eFormat)
{
   list<int> iTmp; 
   //iTmp.push_back(1392161);
   iTmp.push_back(1435868);
   //iTmp.push_back(1436048);

   /* The correct call */
   //RecordSTL = shound->GetRecords(RequestedGiSTL, RecordSTL, eFormat);

   /* a test hack */
   RecordSTL = shound->GetRecords(iTmp, RecordSTL, eFormat);
   return RecordSTL;
}


END_SCOPE(shoundquery)
/* 
 * $Log: query.cpp,v $
 * Revision 1.2  2003/11/03 22:53:24  haocl
 * added shproteinrecord type
 *
 * Revision 1.1.1.4  2003/08/19 08:57:35  root
 * Automated import from Sourceforge.net
 *
 * Revision 1.5  2003/08/18 19:17:09  haocl
 * minor changes, got rid of some magic numbers,
 * changed returned list to passed in references
 *
 * Revision 1.4  2003/08/13 20:35:39  haocl
 * made use of the inheritance hierarchy of SHoundRecord classes
 *
 * Revision 1.3  2003/08/12 19:37:11  haocl
 * added shoundrecords
 *
 * Revision 1.2  2003/08/08 16:22:43  haocl
 * new files shoundrecords and changes to others
 *
 * Revision 1.1  2003/07/31 21:16:38  haocl
 * checking in portion of the new web interface
 * *
 *
 */
