/* $Id: intrez.hpp,v 1.2 2003/11/07 18:51:02 haocl Exp $ */ 
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

#ifndef _SHOUND__HPP_
#define _SHOUND__HPP_
#include <ncbi.h>       /* for the ValNodePtr stuff */
#include <cgi/ncbires.hpp>
#include <cgi/cgiapp.hpp>
#include <shoundtypes.hpp>
#include <shoundrecords.hpp>

USING_NCBI_SCOPE;
using namespace shoundquery;
using namespace shoundrecord;
BEGIN_SCOPE(shound)

typedef struct valnode* ValNodePtr;      /* forward declaration for the compiler */	

/* this is the C++ interface to local SeqHound in C. Only 
 * some functionalities have been implemented, as needed by the
 * web interface. Please do not randomly add new functionality into
 * this class as it makes the cgi unnecessarily larger and more sluggish. */
class Intrez
{
private: /* class variables */

    /* Private variables that hold data during processing of requests. */
    list <string> *sInputSTL;     /* holds the comma delimited keys being search */
    list<Int4> *iInputSTL;        /* holds keys converted to integers */
    list<Int4> *iReturnSTL;       /* int values found */
    list<string> *sReturnSTL;
    CSHoundRecordRegistry *reg;

    const string PROTEIN_STR;
    const string DNA_STR;
    const string RNA_STR;
    const string UNDEF_STR;
    
public: /* functions */
    Intrez();
    ~Intrez( void );
    list<Int4> *StringList2IntList(list<string> &sSTL);
    list<string> *StringTokenizer(string input, string delimitor);
    list<Int4> *GetRecordFromTaxID(string ecode, eMolType eMol);
    list<Int4> *GetRecordFromChromID(string chromid, eMolType eMol);    
    list<Int4> *GetRecordFromOMIMID(string omimid, eMolType eMol);    
    list<Int4> *GetRecordFromMMDBID(string mmdbid, eMolType eMol);
    list<Int4> *GetRecordFromGOID(string goid, eMolType eMol, string Ecode, Int2 degree);
    list<Int4> *GetRecordFromLocusLink(string llid, eMolType eMol);
    list<Int4> *GetRecordFromCDDID(string cddid, eMolType eMol);
    list<Int4> *GetRecordFromPMID(string pmid, eMolType eMol);
    list<CSHoundRecord*>& GetRecords(list<int> &iRequestSTL, list<CSHoundRecord*>& STLReturn, eFormatType eFormat);


private:

    string MoleculeType(eMolType eMol);

    /* private helper functions */
    void ValNodeInt2List(ValNodePtr VNPNode, list<Int4>& List );
};

inline string Intrez::MoleculeType(eMolType eMol)
{
   if(eMol == eProtein) return PROTEIN_STR;
   else if(eMol == eDNA) return DNA_STR;
   else if(eMol == eRNA) return RNA_STR;
   else return UNDEF_STR;
}



END_SCOPE(shound)


#endif

/* 
 * $Log: intrez.hpp,v $
 * Revision 1.2  2003/11/07 18:51:02  haocl
 * adjusted to change in SHoundMoleculeType
 *
 * Revision 1.1.1.4  2003/08/19 08:57:35  root
 * Automated import from Sourceforge.net
 *
 * Revision 1.4  2003/08/18 19:17:09  haocl
 * minor changes, got rid of some magic numbers,
 * changed returned list to passed in references
 *
 * Revision 1.3  2003/08/12 19:37:11  haocl
 * added shoundrecords
 *
 * Revision 1.2  2003/08/08 16:22:43  haocl
 * new files shoundrecords and changes to others
 *
 * Revision 1.1  2003/07/31 21:16:38  haocl
 * checking in portion of the new web interface
 * 
 * 
 */ 


