/* $Id: slri_misc.hpp,v 1.5 2005/03/02 20:26:00 ryao Exp $ */
/***************************************************************************
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


  PROGRAM: SLRI C++  miscellaneous functions.


  AUTHORS:  Doron Betel betel@mshri.on.ca
            Hao Lieu lieu@mshri.on.ca
            and Christopher W.V. Hogue hogue@mshri.on.ca


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
#ifndef MISC_HPP
#define MISC_HPP


/* #include <ncbi.h> */
#include <string>
#include <map>
#include <list>
#include <iterator>
#include <vector>
#include <iostream>

#include <corelib/ncbistd.hpp>
#include <corelib/ncbiapp.hpp>
#include <serial/objostr.hpp>
#include <serial/objistr.hpp>
#include <sstream>
#include <serial/serial.hpp>
#include <objects/mmdb1/Biostruc.hpp>
#include <objects/seqalign/Seq_align.hpp>
#include <objects/seq/Bioseq.hpp>
#include <objects/seqset/Bioseq_set.hpp>
#include <objects/seqset/Seq_entry.hpp>
//#include <objects/nblast/nblast.hpp>
#include <objslrilinkset.hpp>
//#include <slristruc.hpp>
#include <objslrilibstruc.hpp>


/*RY: For API list function handling*/
#define GROUP "#;#"
#define Q_R "#~#"

USING_NCBI_SCOPE;
using namespace objects;

BEGIN_SCOPE(slri)

/* 
   Miscellaneous class: contains various functions that belongs in no
   particular module, general utility funcitons that can be used for
   many purposes. Because of the nature of the functions, most methods 
   are implemented as static. An object does NOT have to be instantiated
   to use the methods. Simply scope it like any other static methods in
   C++: ie: 
     SLRIMisc::StrInt2ListInt(listVariable, stringVariable);
*/
class SLRIMisc{

public:

  /* takes a string of comma delimited string of int and stores it into
     a list of int.
  */
  static void StrInt2ListInt(list<int> *stRet, string info75);

  /* takes a string of comma delimited string of strings and stores it into
     a list <string>.
  */
  static void StrStr2ListStr(list<string>* stRet, string info);
  
  /* takes a delimitor delimited string of strings and store into a list of strings*/
  static void StrStr2ListStr(list<string>* stRet, string info,string delimitor);

  /* takes a list of int and returns a comma delimited string of ints. The
     list is not changed.
   */
  static string SaveListIntAsString(list<int>& stlInt);

  /* takes a list of strings and returns a comma delimited string of string.
     The list is not changed.
   */
  static string SaveListStrAsString(list<string>& stlStr);

  /* move everything from val to a STL list of int */
  /* static list<int> *ValNodeIntToListInt(const ValNodePtr val); */


  /* compares 2 strings, case insensitive.
   */
  static int compare_nocase(const string& s, const string& s2);

  /* returns whether the string is boolean in nature: ie "true", "TRUE",
     "false", "FALSE", "TrUe" "fALSe"...etc will return true, else false
   */
  static bool IsBoolean(const string reply);


  /* prints a list of ASN objects to the file specified, in the format
     specified.
  */
  template <class T>
  static bool PrintASNList(list<T>& stlASN, const string& file_name, ESerialDataFormat format){

    if(stlASN.empty()){
      ERR_POST(Error << "PrintSLRILinkSetList: Empty list.");
      return false;
    }
    if(file_name.empty()){
      ERR_POST(Error << "PrintSLRILinkSetList: Empty file name.");
      return false;
    }

    typename list<T>::iterator iter;
    auto_ptr<CObjectOStream>
      out(CObjectOStream::Open(file_name, format));
    for(iter = stlASN.begin(); iter != stlASN.end(); iter++){
      *out << **iter;
    }
    return true;
  }

  /* prints a list of objects into the file stream specified.
     Precondition, the << operator of the object must be defined.
   */
  template <class T>
  static void PrintList(const list< T >* mylist, ofstream& out){
    if( mylist->empty() ){
      ERR_POST (Warning << "List is empty");
      return;
    }

    ostream_iterator< T > output(out, " ");
    copy (mylist->begin(), mylist->end(), output);
    return;
  }

  /* print list of <Type> pointers
   * Precondition: Type has '<<' overloaded
   * returns number of printed elements */
  template <class T>
  static unsigned int PrintList(list< T* >& list){

    unsigned int count=0;
    typename std::list< T* >::iterator i;
    for(i=list.begin(); i!=list.end(); i++){
      cout<<(**i);
      count++;
      if((count%5)==0)
	cout<<"\n";
    }
    cout<<endl;
    return count;
  }

  /*print list of <Type> objects
   *Precondition: Type has '<<' overloaded
   * returns the number of printed elements  */
  template <class Type>
  static unsigned long PrintList(list<Type>& list)
  {
    unsigned int count=0;
    typename std::list<Type>::iterator i;
    for(i=list.begin(); i!=list.end(); i++)
      {
	Type t=*i;
	cout<<t<<", ";
	count++;
	if((count%5)==0)
	  cout<<"\n";
      }
    cout<<endl;
    return count;
  }

  /*Another PrintList template that takes ofstream and
   * adds newline (mostly for strings, deflines
   * Precondition <Type> has "<<" overloaded */
  template <class Type> 
  static unsigned long PrintList(CNcbiOfstream& os, list<Type>& list)
  {
    unsigned int count=0;
    typename std::list<Type>::iterator i;
    for(i=list.begin(); i!=list.end(); i++)
      {
	Type t=*i;
	os<<t<<endl;
	count++;
      }
    return count;
  }

  /* Print Vectors
   * Same preconditions - Type must have "<<" overloaded
   * returns the number of printed elements */
  template <class Type> 
  static unsigned long PrintVector(vector<Type>& vect)
  {
    if(vect.empty()){
      ERR_POST(Warning<<"[PrintVector] nothing to print");
      return 0;
    }

    unsigned int count=0;
    typename std::vector<Type>::const_iterator i;
    for(i=vect.begin(); i!=vect.end(); i++)
      {
	const Type t=*i;
	cout<<t<<", ";
	count++;
	if((count%5)==0)
	  cout<<"\n";
      }
    cout<<endl;
    return count;
    }
    

  /* Print Map
   * Print Map containers
   * Precondition: The map<key> has "<<" overloaded and
   * <value> type is a vector
   * return total number of vector elements printed*/
  template <class TypeA, class TypeB> 
  static unsigned int PrintMap( map< TypeA, vector<TypeB>  >& mappie)
  {
    if(mappie.empty())
      {
	ERR_POST(Warning<<"[PrintMap] nothing to print");
	return 0;
      }
    unsigned long count=0;
    typename std::map<TypeA, vector<TypeB> >::const_iterator i;
    for(i=mappie.begin(); i!=mappie.end(); i++)
      {
	const TypeA& key=(*i).first;
	const vector<TypeB>& value=(*i).second;
	typename vector<TypeB>::const_iterator j;
	cout<<"Key "<<key<<endl;
	for(j=value.begin(); j!=value.end(); j++)
	  {
	    cout<<(*j)<<": ";
	    count++;
	  }
	cout<<"\n-------\n";
      }
    return count;
  }
  /*
   * Print map of standard types (e.g. int, double, string,...)
   * */
  template <class Key, class Value> 
  static unsigned long PrintMap(map<Key, Value>& mappie)
  {
    if(mappie.empty())
      {
	ERR_POST(Warning<<"[PrintMap] nothing to print");
	return 0;
      }
    unsigned long count=0;
    typename std::map<Key,Value>::const_iterator i;
    for(i=mappie.begin(); i!=mappie.end(); i++){
      cout<<"Key: "<<(*i).first<<" Value: "<<(*i).second<<endl;
      count++;
    }
    return count;
  }
  

  /* template function to free a list of Type pointers*/
  template <class Type> 
  static int FreeList(list<Type*>& list)
  {
    typename std::list<Type*>::iterator i;
    for(i=list.begin(); i!=list.end(); i++){
      delete (*i);
    }
    return 0;
  }
  
  template <class Type> 
  static void FreeList(list<Type>& list)
  {
    typename std::list<Type>::iterator i;
    for (i=list.begin(); i!=list.end(); i++){
      delete &i;
    }
  }
 
  
  /*RY  helper function to handle API list function
   */
  
  /*converts a list<string> to a map<int, list<int> >
   */
   static void List2Map(list<string> &v, map<int, list<int>* > &m);
   
   /*converts a list<string> to a map <int, CSLRIFasta*> 
    */
   static void List2Map(list<string> &v, map<int, CSLRIFasta*> &m);
   
   /*converts a string to SLRI fasta object, return a pointer to CSLRIFasta
   */
   static CSLRIFasta* Str2Fasta(string &s);
   
   /* free map<int, list<int>* > */
   static void FreeMap ( map<int, list<int>* > &m);


public:

  
private:
  int i;
};
END_SCOPE(slri) 

#endif

/*
$Log: slri_misc.hpp,v $
Revision 1.5  2005/03/02 20:26:00  ryao
changed query and return pair symbol from '~' to '#~#', since some gis might contain '~' in defline, example gi 9971716. bug [4100].

Revision 1.4  2004/11/02 16:31:24  ryao
changed the GROUP separator from ';' to '#;#'. bug 3787 related.

Revision 1.3  2004/06/15 17:54:38  ryao
Changed prototype of function List2Map; added function FreeMap.

Revision 1.2  2004/04/05 18:34:31  ryao
Added helper functions for keyed list function. Added List2Map  Str2Fasta

Revision 1.1.1.13  2003/05/24 08:36:02  root
Automated import from Sourceforge.net

Revision 1.15  2003/05/23 20:02:50  haocl
remove dependency on C ncbi toolkit

Revision 1.14  2003/03/14 18:02:25  haocl
added function to copy valnode to list object

Revision 1.13  2002/11/15 16:15:35  betel
Fixed compile warnings

Revision 1.12  2002/11/04 14:13:14  betel
Fixed PrintList function

Revision 1.11  2002/10/22 04:07:43  micheld
Added SCOPE to both header and source file

Revision 1.10  2002/10/22 03:38:46  micheld
Removed MSVC compiler errors

Revision 1.8  2002/10/11 20:38:49  haocl
fixed more Windows bugs

Revision 1.7  2002/10/11 20:33:34  haocl
made changes to work on MSC++

Revision 1.6  2002/09/26 20:38:32  haocl
added 2 include statements for uniform compiling
on different platforms

Revision 1.4  2002/07/15 20:31:28  haocl
changed StrXXX2ListXXX to use pointers

Revision 1.3  2002/07/10 15:34:26  haocl
added comments

Revision 1.2  2002/07/05 18:20:24  haocl
minor changes

Revision 1.1  2002/07/04 20:47:14  haocl
moved file from seqhound/include_cxx

Revision 1.4  2002/07/04 19:54:46  haocl
got ride of excessive include statements in slri_misc.hpp

Revision 1.3  2002/07/04 19:04:01  haocl
deleted functions that were same as NCBI built in functions

Revision 1.2  2002/07/03 21:18:34  haocl
changes to reflect the inclusion of slri_misc class

Revision 1.1  2002/05/09 18:53:48  betel
Moved utility template functions to slri_misc.hpp

*/
