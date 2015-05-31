/* $Id: intrez.cpp,v 1.4 2003/11/21 21:11:42 haocl Exp $ */ 
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

#include <corelib/ncbistr.hpp>
#include <corelib/ncbiexpt.hpp>
#include <corelib/ncbistd.hpp>
#include <cgi/cgictx.hpp>
#include <corelib/ncbireg.hpp>
#include <html/page.hpp>
#include <corelib/ncbiobj.hpp>
#include <memory>
#include <asn.h>
#include <ncbi.h>
#include <objsset.h>
#include <seqport.h>
#include <asn2ff.h>
#include <asn2ffg.h>
#include <seqhound.h>
#include <rpsdbapi.h>
#include "intrez.hpp"
#include <list>
#include <iterator>
#include <iostream>
#include <fstream>
#include <shoundtypes.hpp>
#include <objslrilibstruc.hpp>
#include <shoundrecords.hpp>

USING_NCBI_SCOPE;
using namespace shoundquery;
using namespace shoundrecord;


BEGIN_SCOPE(shound)


/* a unary_function is a function that acts as an object.
 * You can think of it as similar to a function pointer.
 * It is used in STL functions where a particular function
 * has to be applied to list. Whereas in C you could
 * pass a function pointer, in C++ you pass a unary_function
 * to the for_each funtion. To see how this class gets used,
 * search for an instance of the for_each function. */
template<class T> 
/* this unary_function class (INT2STRING) is used on a list 
 * of string integers that must be converted into ints. */
class INT2STRING :public unary_function<T, void>	
{
public:
  /* the constructor to the function object. It
   * takes as a parameter a list<int> which is
   * where the integers will get inserted to.*/
  INT2STRING( list<int>& iInput ) : i(iInput) {} 

  /* ()operator applied to each of the strings in
   * the list. Converts the string into integer and
   * pushes the integer into the list passed in constructor. */
  void operator()(T e)
  {
    try { i.push_back(NStr::StringToInt(e)); }
    catch(CStringException e) { i.push_back(0); }
  }
  
  /* reference to an int list */
  list<int> &i;
};


/* Another unary_function class */
template<class T>
class ADDINT2VALNODE : public unary_function<T, void>
{
public:
   ADDINT2VALNODE(ValNodePtr &vnp) : v(vnp) {}
   void operator()(T add)
   {
      ValNodeAddInt(&v, 0, add);
   }
   ValNodePtr &v;
};


/* ************************************************************** */

/* The Intrez Class, acts as an interface between the C++ web 
 * interface and the C intrez layer */

/*constructor initializes SHound */
Intrez::Intrez() : PROTEIN_STR("protein"), DNA_STR("dna"), RNA_STR("rna"), UNDEF_STR("undef")
{
   char appname[] = "intrezcxx";
   reg = new CSHoundRecordRegistry;
   reg->LoadRegistry();
   sInputSTL = new list<string>;
   iInputSTL = new list<Int4>;
   iReturnSTL = new list<Int4>;
   sReturnSTL = new list<string>;
   SHoundInit(TRUE, appname);
}

Intrez::~Intrez()
{
   SHoundFini();
   delete(reg);
   delete(sInputSTL);
   delete(iInputSTL);
   delete(iReturnSTL);
   delete(sReturnSTL);
}

/* converts a list of string integers to a list of ints.
 * returns the list of ints. */
list<int>* Intrez::StringList2IntList(list<string> &sSTL)
{
   /* convert each of the strings into integers, see unary_function */
   delete iInputSTL;
   iInputSTL = new list<Int4>;
   for_each(sSTL.begin(), sSTL.end(), INT2STRING<string>(*iInputSTL) );
   return iInputSTL;
}

/* splits 'input' based on 'delimitor' */
list<string> *Intrez::StringTokenizer(string input, string delimitor)
{
   /* split function, similar to split in perl,
    * tokenizes input based on 'delimitor' and inputs
    * the tokens into sInputSTL. */
   delete sInputSTL;
   sInputSTL = new list<string>;
   NStr::Split(input, delimitor, *sInputSTL);
   return sInputSTL;
}

/* moves all Int4 values from the ValNode list 'VNPNode' to list 'RET' */
void Intrez::ValNodeInt2List(ValNodePtr VNPNode, list<Int4>& RET)
{
   for(ValNodePtr v = VNPNode; v != NULL; v = v->next)
   {
      RET.push_back(v->data.intvalue);
   }
}


/* Returns all the gi of 'taxid' that is of the molecule type 'eMol' */
list<Int4>* Intrez::GetRecordFromTaxID(string taxid, eMolType eMol)
{
   /* the input is gi string; if multiple gi, string is comma delimited.
    * convert the string to a list of strings ... */
   list<string> *sTaxSTL = StringTokenizer(taxid, ",");

   /* convert list of strings to list of ints */
   list<Int4> *iTaxSTL = StringList2IntList(*sTaxSTL);

   ValNodePtr va = NULL;
   /* convert the list to a ValNode (C api uses ValNodes) */
   for_each(iTaxSTL->begin(), iTaxSTL->end(), ADDINT2VALNODE<Int4>(va));

   /* call the C layer code equivalent of this function based
    * on the molecule type. */
   ValNodePtr VNPRet = (eMol ? SHoundDNAFromTaxIDList(va) : SHoundProteinsFromTaxIDList(va));
   ValNodeFree(va);

   /* convert the return ValNode to a list */
   delete iReturnSTL;
   iReturnSTL = new list<Int4>;
   ValNodeInt2List(VNPRet, *iReturnSTL);
   ValNodeFree(VNPRet);
   return iReturnSTL;
}

/* returns all gi records from chromosome 'chromid' that are of
 * molecule type 'eMol' */
list<Int4>* Intrez::GetRecordFromChromID(string chromid, eMolType eMol)
{
   /* the input is gi string; if multiple gi, string is comma delimited.
    * convert the string to a list of strings ... */
   list<string> *sChromSTL = StringTokenizer(chromid, ",");

   /* convert list of strings to list of ints */
   list<Int4> *iChromSTL = StringList2IntList(*sChromSTL);

   ValNodePtr va = NULL;
   /* convert the list to a ValNode (C api uses ValNodes) */
   for_each(iChromSTL->begin(), iChromSTL->end(), ADDINT2VALNODE<Int4>(va));

   /* call the C layer code equivalent of this function based
    * on the molecule type. */
   ValNodePtr VNPRet = (eMol ? SHoundDNAFromChromosomeList(va) : SHoundProteinsFromChromosomeList(va));
   ValNodeFree(va);
   
   /* convert the return ValNode to a list */
   delete iReturnSTL;
   iReturnSTL = new list<Int4>;
   ValNodeInt2List(VNPRet, *iReturnSTL);
   ValNodeFree(VNPRet);
   return iReturnSTL;
}

/* returns all gi records of OMIM 'omimid' of molecule type 'eMol' */
list<Int4>* Intrez::GetRecordFromOMIMID(string omimid, eMolType eMol)
{
   /* the input is gi string; if multiple gi, string is comma delimited.
    * convert the string to a list of strings ... */
   list<string> *sOMIMSTL = StringTokenizer(omimid, ",");

   /* convert list of strings to list of ints */
   list<Int4> *iOMIMSTL = StringList2IntList(*sOMIMSTL);

   /* convert the list to a ValNode (C api uses ValNodes) */
   ValNodePtr iOMIMVNP = NULL;
   for_each(iOMIMSTL->begin(), iOMIMSTL->end(), ADDINT2VALNODE<Int4>(iOMIMVNP));

   /* call the C layer code equivalent of this function */ 
   ValNodePtr VNPProtein = SHoundGiFromOMIMList(iOMIMVNP);
   ValNodeFree(iOMIMVNP);
   
   ValNodePtr VNPRet = NULL;
   /* All the gi in LocusLink are proteins, so if requesting
    * DNA need to get the DNA equivalents of the protein. */
   if(eMol == eDNA) /* requesting DNA */
   {
      VNPRet = SHoundDNAFromProteinList(VNPProtein);
      ValNodeFree(VNPProtein);
   }else{ VNPRet = VNPProtein; }

   /* convert the return ValNode to a list */
   delete iReturnSTL;
   iReturnSTL = new list<Int4>;
   ValNodeInt2List(VNPRet, *iReturnSTL);
   ValNodeFree(VNPRet);
   return iReturnSTL;
}

/* returns all gi records from chromosome 'chromid' that are of
 * molecule type 'eMol' */
list<Int4>* Intrez::GetRecordFromMMDBID(string mmdbid, eMolType eMol)
{
   string mol;
   /* the input is gi string; if multiple gi, string is comma delimited.
    * convert the string to a list of strings ... */
   list<string> *sMmdbidSTL = StringTokenizer(mmdbid, ",");

   /* convert list of strings to list of ints */
   list<Int4> *iMmdbidSTL = StringList2IntList(*sMmdbidSTL);

   ValNodePtr va = NULL;
   /* convert the list to a ValNode (C api uses ValNodes) */
   for_each(iMmdbidSTL->begin(), iMmdbidSTL->end(), ADDINT2VALNODE<Int4>(va));

   /* call the C layer code equivalent of this function based
    * on the molecule type. The 2nd parameter in GiFrom3dList
    * specifies whether to retrieve proteins only; if TRUE
    * then get back only protein gi, otherwise, get
    * proteins, dna, rna etc. */
   ValNodePtr VNPProt = (eMol ? SHoundGiFrom3DList(va, FALSE) : SHoundGiFrom3DList(va, TRUE));
   ValNodeFree(va);
   CharPtr molecule = NULL;
   ValNodePtr VNPRet = NULL;
   if(eMol) /* true when eMol == eDNA */
   { /* got protein, dna, rna, etc, so need to send back */
     /* dna only, so have to filter list */

     for(ValNodePtr v = VNPProt; v != NULL; v = v->next)
     {
       molecule = SHoundMoleculeType(v->data.intvalue);
       mol = MoleculeType(eMol);
       if( strcmp(molecule, mol.c_str()) == 0)
         ValNodeAddInt(&VNPRet, 0, v->data.intvalue); 
     }
     ValNodeFree(VNPProt);
   }else{ VNPRet = VNPProt; }
   
   /* convert the return ValNode to a list */
   delete iReturnSTL;
   iReturnSTL = new list<Int4>;
   ValNodeInt2List(VNPRet, *iReturnSTL);
   ValNodeFree(VNPRet);
   return iReturnSTL;
}

/* HCL: TO DO: not completed: still need to incorporate the degrees functionality */
list<Int4>* Intrez::GetRecordFromGOID(string goid, eMolType eMol, string Ecode, Int2 degree)
{
   /* the input is gi string; if multiple gi, string is comma delimited.
    * convert the string to a list of strings ... */
   list<string> *sGoidSTL = StringTokenizer(goid, ",");

   /* convert list of strings to list of ints */ 
   list<Int4> *iGoidSTL = StringList2IntList(*sGoidSTL);

   ValNodePtr va = NULL;
   /* convert the list to a ValNode (C api uses ValNodes) */
   for_each(iGoidSTL->begin(), iGoidSTL->end(), ADDINT2VALNODE<Int4>(va));

   /* call the C layer code equivalent of this function */
   ValNodePtr VNPProt;
   if(strcmp(Ecode.c_str(), "none") == 0) VNPProt = SHoundGiFromGOIDList(va);
   else VNPProt = SHoundGiFromGOIDListAndECode( va, const_cast<char*>(Ecode.c_str()) );
   ValNodeFree(va);
  
   ValNodePtr VNPRet = NULL;
   /* All records in LocusLink are proteins, so if requesting
    * DNA, need to get the Proteins. */
   if(eMol == eDNA)
   {
      VNPRet = SHoundDNAFromProteinList(VNPProt);
      ValNodeFree(VNPProt);
   }else{ VNPRet = VNPProt; }
   
   /* convert the return ValNode to a list */
   delete iReturnSTL;
   iReturnSTL = new list<Int4>;
   ValNodeInt2List(VNPRet, *iReturnSTL);
   ValNodeFree(VNPRet);
   return iReturnSTL;
}

list<Int4>* Intrez::GetRecordFromLocusLink(string llid, eMolType eMol)
{
   /* the input is gi string; if multiple gi, string is comma delimited.
    * convert the string to a list of strings ... */
   list<string> *sLLSTL = StringTokenizer(llid, ",");

   /* convert list of strings to list of ints */
   list<Int4> *iLLSTL = StringList2IntList(*sLLSTL);

   ValNodePtr va = NULL;
   /* convert the list to a ValNode (C api uses ValNodes) */
   for_each(iLLSTL->begin(), iLLSTL->end(), ADDINT2VALNODE<Int4>(va));

   /* call the C layer code equivalent of this function */
   ValNodePtr VNPProtein = SHoundGiFromLLIDList(va);
   ValNodeFree(va);
  
   ValNodePtr VNPRet = NULL;
   /* All records in LocusLink are proteins, so if requesting
    * DNA, need to get the Proteins. */
   if(eMol == eDNA)
   {
      VNPRet = SHoundDNAFromProteinList(VNPProtein);
      ValNodeFree(VNPProtein);
   }else{ VNPRet = VNPProtein; }
   
   /* convert the return ValNode to a list */
   delete iReturnSTL;
   iReturnSTL = new list<Int4>;
   ValNodeInt2List(VNPRet, *iReturnSTL);
   ValNodeFree(VNPRet);
   return iReturnSTL;
}

list<Int4>* Intrez::GetRecordFromCDDID(string cddid, eMolType eMol)
{
   /* the input is gi string; if multiple gi, string is comma delimited.
    * convert the string to a list of strings ... */
   list<string> *sCDDIDSTL = StringTokenizer(cddid, ",");

   /* call the C layer code equivalent of this function */
   ValNodePtr VNPProt = NULL;
   list<string>::iterator it;
   for(it = sCDDIDSTL->begin(); it != sCDDIDSTL->end(); it++)
   {
      ValNodePtr va = SHoundGetGisByDomainId(const_cast<char*>( (*it).c_str() ));
      for(ValNodePtr v = va; v != NULL; v = v->next)
      {
         ValNodeAddInt(&VNPProt, 0, v->data.intvalue);
      }    
      ValNodeFree(va);
   }
  
   ValNodePtr VNPRet = NULL;
   /* All records in LocusLink are proteins, so if requesting
    * DNA, need to get the Proteins. */
   if(eMol == eDNA)
   {
      VNPRet = SHoundDNAFromProteinList(VNPProt);
      ValNodeFree(VNPProt);
   }else{ VNPRet = VNPProt; }
   
   /* convert the return ValNode to a list */
   delete iReturnSTL;
   iReturnSTL = new list<Int4>;
   ValNodeInt2List(VNPRet, *iReturnSTL);
   ValNodeFree(VNPRet);
   return iReturnSTL;
}

list<Int4>* Intrez::GetRecordFromPMID(string pmid, eMolType eMol)
{
   string mol;
   /* the input is gi string; if multiple gi, string is comma delimited.
    * convert the string to a list of strings ... */
   list<string> *sPMIDSTL = StringTokenizer(pmid, ",");

   list<Int4> *iPMIDSTL = StringList2IntList(*sPMIDSTL);

   ValNodePtr va = NULL;
   /* convert the list to a ValNode (C api uses ValNodes) */
   for_each(iPMIDSTL->begin(), iPMIDSTL->end(), ADDINT2VALNODE<Int4>(va));

   /* call the C layer code equivalent of this function */
   ValNodePtr VNPAll = SHoundGiFromReferenceList(va, 1);
   ValNodeFree(va);

   /* check the molecule type of each of the return value before returning */
   ValNodePtr VNPRet = NULL;
   CharPtr molecule = NULL;
   for(ValNodePtr v = VNPAll; v != NULL; v = v->next)
   {

      molecule = SHoundMoleculeType(v->data.intvalue);
      mol = MoleculeType(eMol);
      if(strcmp(molecule, mol.c_str()) == 0)
      {
	 /* found a match of the molecule type */
         ValNodeAddInt(&VNPRet, 0, v->data.intvalue);
      }    
   }
   ValNodeFree(VNPAll); 
   
   /* convert the return ValNode to a list */
   delete iReturnSTL;
   iReturnSTL = new list<Int4>;
   ValNodeInt2List(VNPRet, *iReturnSTL);
   ValNodeFree(VNPRet);
   return iReturnSTL;
}

list<CSHoundRecord*>& Intrez::GetRecords(list<int> &iRequestSTL, list<CSHoundRecord*>& STLReturn, eFormatType eFormat)
{
   CSHoundRecord* rec;
    
   list<int>::iterator iIT = iRequestSTL.begin();
   for(; iIT != iRequestSTL.end(); iIT++)
   {
      rec = reg->GetDataRecord(eFormat, *iIT);
      STLReturn.push_back(rec);
   }
   return STLReturn;
}


END_SCOPE(shound)

/* 
 * $Log: intrez.cpp,v $
 * Revision 1.4  2003/11/21 21:11:42  haocl
 * major refactoring, splitting cgi into 2 executables, renaming files
 * from seqxxx to seqdata, renaming Makefile
 *
 * Revision 1.3  2003/11/07 18:51:02  haocl
 * adjusted to change in SHoundMoleculeType
 *
 * Revision 1.2  2003/11/07 18:06:29  haocl
 * fixed compiling for new toolkit
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
