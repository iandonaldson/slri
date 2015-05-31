/* $Id: logic.hpp,v 1.3 2003/11/21 21:11:42 haocl Exp $ */ 
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


  PROGRAM: SeqHound C++ STL List logic functions 


  AUTHORS: Hao Lieu


  REVISION/CONTRIBUTION NOTES:

  September 1, 2002  Original MSH Revision.
                Hogue Lab - University of Toronto Biochemistry
                Department and the Samuel Lunenfeld Research
                Institute, Mount Sinai Hospital
                http://bioinfo.mshri.on.ca  hogue@mshri.on.ca
                ftp://ftp.mshri.on.ca/


  PUBLICATION TO CITE:

   auxiliary template class for the C++ web api

***********************************************************************
*/

#ifndef SH_LOGIC_HPP
#define SH_LOGIC_HPP

#include <corelib/ncbistd.hpp>
#include <list>
#include <iostream>
#include <iterator>
#include <string>
#include <functional>
#include <algorithm>

USING_NCBI_SCOPE;
BEGIN_SCOPE(shoundlogic)

/************************PREDICATE CLASS****************************/

/* a unary function is SIMILAR in concept to a function pointer. It
 * is actually a function object, ie in C, you would pass a function
 * pointer; in C++, you can use a unary function instead. This
 * function object is then applied as needed. In STL functions, 
 * a function needs to be applied to each element of the container.
 * A function object containing the function to be applied
 * is passed into the STL function, and this is used on each 
 * element of the STL. You can look up the for_each function,
 * or find_if function (and others) to see how it is used.
 */

/* class PKey represents a function object.
 * the unary operator is the operator '()' (ie the casting operator). 
 * It is used by STL functions for making a comparision on each element 
 * in the STL. So for a linked list eg list = (1, 4, 5, 7, 34), the
 * STL function will iterate through 1 ... 34. At each element,
 * the STL function will apply PKey's operator() to each element. 
 * The operator() will compare each element against the
 * test value passed to PKey's constructor.
 */

/* Type T must support a boolean comparison using == operator. 
 * In cases where T does not have a built in == operator, you 
 * must define your own. Supported type T's include: int, string,
 * floats, ... and your own defined T, provided you define '=='. */
template<class T>
class PKey : public unary_function<T,bool>
{
  const T test;  /* compare every element in list against test */ 
public:

  /* explicit constructor: pass the constructor the value
   * that you want each element in the list to be compared with 
   */ 
  explicit PKey( const T t ) : test(t){}

  /* the ()operator is IMPLICITLY called by the STL algorithm 
   * functions. 'e' is/are the elements in the list used in the 
   * STL algorithm functions. each element is converted to a 
   * PKey and gets compared to 'test' that was passed in 
   * during the constructor call. 
   */
  bool operator() (const T e) const {  /* element in list */
    /* ... in this case the comparison is ==. */
    return test == e; 
  }
};
/****************************END OF PREDICATE CLASS***************************/

/************************ SHOUNDLOGIC CLASS **************************/
/* the SHoundLogic class, contains a static function ApplyLogic. 
 * You cannot instantiate an instance of SHoundLogic, you can only 
 * call the function ApplyLogic. It will take lists of strings, 
 * or list of ints or list of floats, ... or your own user defined 
 * class, provided your user defined class has the operator == defined
 * (See PKey class definition above). 
 * eg of usage:
 *
 *   using namepace shoundlogic;
 *  
 *   main(){
 *   fill list1, 
 *   fill list2, 
 * 
 *   list3 = SHoundLogic::ApplyLogic(list1, list2, SHoundLogic::eAND);
 */
class SHoundLogic 
{
public:
  
  /* the 3 logic operations that can be applied. 
   * the binary operations work on lists. 
   *   list3 = list1 && list2 
   *   list3 = list1 || list2
   *   list3 = list1 - list2 */	
  enum eLogic 
  {
    eAND, 
    eOR,
    eNOT,
    eUND
  };
  
  /* these are friend logic functions as defined below */

  /* returns a list of keys that are present in both STL1 AND STL2 */
  template <class T>
  friend list<T>& operator&&(const list< T >& STL1, const list<T>& STL2);

  /* returns a list of values that are in either STL1 OR STL2 */
  template <class T>
  friend list<T>& operator||(const list< T >& STL1, const list<T>& STL2);

  /* returns a list of values in STL1 and NOT in STL2 */
  template <class T>
  friend list<T>& operator-(const list< T >& STL1, const list<T>& STL2);

  /* a nice overloaded operater for you to print your lists out with.
   * To use it, include this file, and ....
   * 
   *   cout << list << endl; 
   */ 
  template<class T>
  friend ostream& operator<<(ostream& cout, list< T > STLList);
  
  /* applies logic to STL1 and STL2: order of STL1 and STL2 
   * DOES MATTER for the '-' operator:  STL1 logic STL2 eg:
   *     STL1 && STL2 (same as STL2 && STL1)
   *     STL1 || STL2 (same as STL2 || STL1)
   *     STL1 - STL2 (differs from STL2 - STL1)
   */
  template< class T > 
  static list< T >& ApplyLogic(const list< T >& STL1, const list< T >& STL2, eLogic logic)
  {
     switch(logic)
     {
       case eAND:
         return STL1 && STL2;
       case eOR:
	 return STL1 || STL2;
       case eNOT:
         return STL1 - STL2;
       default: /* this should not happen */
         return *(new list<T>);
     }
  }
};

/****************************LOGIC FUNCTIONS**********************************/

/* logic functions AND, OR and NOT applied to 2 lists:
 *   AND return a list containing those keys that are present in both lists,
 *   OR returns a list that is the result of merging list 1 with list 2, no duplicates,
 *   NOT returns a list that is the list1 without the values found in list2.
 *
 * the logic functions are defined as operators && (AND), || (OR) and - (NOT)
 * for convenient usage:
 *   list_result = list1 && list2
 *   list_result = list1 || list2
 *   list_result = list1 - list2
 */

/******************/

/* overload << operator for writing out lists quickly.
 * used to print out a list using the cout << style. */
template <class T>
ostream& operator<<(ostream& cout, list< T > STLList)
{
   copy(STLList.begin(), STLList.end(), ostream_iterator< T >(cout, " ") );
   return cout;
}


/* returns a list containing those keys that are found in both STL1 and STL2.
 * The returned list will be sorted and contain NO duplicates. */
template <class T>
list<T>& operator&&(const list< T >& STL1, const list<T>& STL2)
{
  list< T > *STL = new list< T >;
  typename list< T >::const_iterator it;

  for(typename list< T >::const_iterator it2 = STL2.begin(); it2 != STL2.end(); it2++)
  { 
    /* we iterate through STL2 and look for 
     * its members inside  STL1 (using the 
     * STL algorithm find_if). use the PKey defined above. */
    it = find_if( STL1.begin(), STL1.end(), PKey<T>(*it2) );

    /* if not end of STL1, so it2 was found in STL1, so it2 in both lists */
    if(it != STL1.end()) STL->push_back(*it2); 
  }

  STL->sort();
  STL->unique();
  return *STL; 
}


/* returns a list containing keys that are found in either STL1 or STL2.
 * The returned list will be sorted and contain NO duplicates. */
template <class T>
list<T>& operator||(const list< T >& STL1, const list<T>& STL2)
{
  list< T > *STL = new list< T >(STL1.size());
  typename list< T >::const_iterator it;

  /* first we copy all of STL1 */
  copy(STL1.begin(), STL1.end(), STL->begin());

  for(typename list< T >::const_iterator it2 = STL2.begin(); it2 != STL2.end(); it2++)
  { 
    /* iterate through STL2 and look for its 
     * members inside STL1 (using find algorithm 
     * of STL) */
    it = find_if( STL->begin(), STL->end(), PKey<T>(*it2) );

    /* we reach end of STL, so it2 not in STL, so we keep it */
    if(it == STL->end()) STL->push_back(*it2);
  }
  STL->sort();
  STL->unique();
  return *STL; 
}

/* returns a list containing those keys that are found in STL1 and NOT in STL2.
 * The returned list will be sorted and contain NO duplicates. */
template< class T >
list< T >& operator-( const list< T >& STL1, const list< T >& STL2)
{
  list< T > *STL = new list< T >(STL1.size()); 
  typename list< T >::const_iterator it;

  copy(STL1.begin(), STL1.end(), STL->begin());

  for(typename list< T >::const_iterator it2 = STL2.begin(); it2 != STL2.end(); it2++)
  {
    /* using the STL algorithms */
    it = find_if( STL->begin(), STL->end(), PKey<T>(*it2) );

    /* found it, so remove it */
    if(it2 != STL->end()) STL->remove(*it2);
  }
  STL->sort();
  STL->unique();
  return *STL; 
} 

/****************************END OF LOGIC FUNCTIONS*******************************/

/************************LIST COMPARISON FUNCTIONS************************/
/* returns true if all the elements of STL2 are found in STL1, otherwise
 * returns false. */
template <class T>
bool compare_helper(const list< T >& STL1, const list< T >& STL2)
{
   typename list< T >::const_iterator it;
   for(typename list< T >::const_iterator it2 = STL2.begin(); it2 != STL2.end(); it2++)
   {
      it = find_if( STL1.begin(), STL1.end(), PKey<T>(*it2) );
      /* if equal to STL1.end(), then not in STL1, so not equal */
      if(it == STL1.end()) return false;
   }
   return true;
}

/* returns true if the two lists contain the same elements 
 * otherwise returns false. */
template <class T>
bool is_equivalent(const list< T >& STL1, const list< T >& STL2)
{
   /* have to check lists in both directions: list1 has same 
    * elements as list2, and list2 has same elements as list1 */
   if(!compare_helper(STL1, STL2)) return false;
   return compare_helper(STL2, STL1);
}
  
END_SCOPE(shoundlogic)

#endif

/* 
 * $Log: logic.hpp,v $
 * Revision 1.3  2003/11/21 21:11:42  haocl
 * major refactoring, splitting cgi into 2 executables, renaming files
 * from seqxxx to seqdata, renaming Makefile
 *
 * Revision 1.2  2003/11/03 22:53:24  haocl
 * added shproteinrecord type
 *
 * Revision 1.1.1.2  2003/08/21 08:48:07  root
 * Automated import from Sourceforge.net
 *
 * Revision 1.2  2003/08/20 19:33:44  haocl
 * check in for switchover
 *
 * Revision 1.1  2003/07/31 21:16:38  haocl
 * checking in portion of the new web interface
 *
 *
 *
 */
