/* $Id: slri_misc.cpp,v 1.5 2005/03/02 20:20:37 ryao Exp $ */
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


  AUTHORS:  Doron Betel (betel@mshri.on.ca)
            Hao Lieu (lieu@mshri.on.ca)
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
/* #include <ncbi.h> */
#include <slri_misc.hpp>


BEGIN_SCOPE(slri)
USING_NCBI_SCOPE;
typedef list<int> TIList;
typedef list<string> TSList;
typedef map<int, list<int> > TIMap;

/* takes a string of comma delimited string of int and stores it into
   a list of int.
*/
void SLRIMisc::StrInt2ListInt(list<int> *stRet, string info){
  //return soo->front();
  for(size_t pos = 0; ; ){
    size_t not_pos = info.find_first_not_of(",", pos);
    pos = info.find_first_of(",", not_pos);
    if(pos == not_pos) break;
    stRet->push_back(atoi(info.substr(not_pos, pos - not_pos).c_str()));
  }
}

/* converts a comma delimited string of strings to a list <string>
   This function actually uses an NCBI built in function, but to keep
   the interface consistent its wrapped with this SLRIMisc function */
void SLRIMisc::StrStr2ListStr(list<string>* stRet, string info){

  NStr::Split(info, ",", *stRet);
  
}

/* converts a delimited string of strings to a list <string>
 * this function is similar as the StrStr2ListStr but with a string delimiter instead of comma
 * DB: Note that this function is meant to split by a full match to the delimiter and not by
 * a match to ANY of the chars in the delimiter. Therefore the call to NStr::Split will not do the trick
 */
void SLRIMisc::StrStr2ListStr(list<string>* stRet, string info, string delimitor){

	if (info.empty())
		return;
	else if (delimitor.empty()){
		stRet->push_back(info);
		return;
	}
	size_t prev_pos=0, pos=0;
	try {
		for(size_t pos=0 ; ;)
		{
			prev_pos=pos;
			if(prev_pos==NPOS){
				break;
			}
			pos=info.find(delimitor,prev_pos);
			if(pos==NPOS){
				stRet->push_back(info.substr(prev_pos));
				break;
			}else {
				stRet->push_back(info.substr(prev_pos, pos-prev_pos));
				pos =pos+delimitor.length();
			}
		}
	}
	STD_CATCH_ALL("[StrStr2ListStr]" << info<<"\n"<<prev_pos<<" "<<pos<<"\n");
	return;

}

/* takes a list of int and returns a comma delimited string of ints. The
   list is not changed.
*/
string SLRIMisc::SaveListIntAsString(list<int>& stlInt){
  
  if(stlInt.empty()){
    return kEmptyStr;
  }

  string s;
  while(!stlInt.empty()){
    s+=NStr::IntToString(stlInt.front(), false) + ",";
    stlInt.pop_front();
  }
  return s.substr(0, s.length() - 1);
}
/* takes a list of strings and returns a comma delimited string of string.
   The list is not changed.
*/
string SLRIMisc::SaveListStrAsString(list<string>& stlStr){

  if(stlStr.empty()){
    return kEmptyStr;
  }
  string s;
  while(!stlStr.empty()){
    s+=stlStr.front()+",";
    stlStr.pop_front();
  }
  return s.substr(0, s.length() - 1);
}

/* move everything from val to a STL list of int */
/* list<int>* SLRIMisc::ValNodeIntToListInt(const ValNodePtr val)
{
   list<int> *iSTL = new list<int>;
   for(ValNodePtr cur = val; cur != NULL; cur = cur->next)
   {
      iSTL->push_back(cur->data.intvalue);
   }
   return iSTL;
} */


/* compares 2 strings, case insensitive.
 */
int SLRIMisc::compare_nocase(const string& s, const string& s2){

  string::const_iterator p = s.begin();
  string::const_iterator p2 = s2.begin();
  
  while(p != s.end() && p2 != s2.end()){
    if(toupper(*p) != toupper(*p2)){
      return (toupper(*p) < toupper(*p2)) ? -1 : 1;
    }
    ++p;
    ++p2;
  }

  return (s2.size() == s.size()) ? 0 : (s.size() < s2.size()) ? -1 : 1;
}
/* returns whether the string is boolean in nature: ie "true", "TRUE",
   "false", "FALSE", "TrUe" "fALSe"...etc will return true, else false
*/
bool SLRIMisc::IsBoolean(const string reply){

  string tr = "true";
  string fal = "false";

  if(SLRIMisc::compare_nocase(reply, tr) == 0 || SLRIMisc::compare_nocase(reply, fal) == 0){
    return true;
  }
  return false;
}

/*RY: helper functions to handle list API functions
 */

/* converts a list<string> to a map<int, list<int>* >
 */
void SLRIMisc::List2Map(list<string> &v, map<int, list<int>* > &m )
{
	list<string>::iterator it;
	try{
		for ( it = v.begin(); it != v.end(); it++ )
		{
			TSList *strlist = new TSList;

			/*split the query and return pair string (separated by Q_R) into a list<string>*/
			SLRIMisc::StrStr2ListStr(strlist, *it, Q_R);
			list<string>::iterator ic;
			for (ic = strlist->begin(); ic != strlist->end(); ic++)
			{

				TIList *intlist = new TIList;
				string key  = *ic;
				string  value = *(++ic);

				int intkey = NStr::StringToInt(key);
				if (!NStr::CompareNocase(value, "NULL")){
					m[intkey] = NULL;
				}
				else {
					SLRIMisc::StrInt2ListInt(intlist, value);
					//intlist->sort();
					//intlist->unique();
					m[intkey] = intlist;
				}
			}
			delete strlist;
		}
	}
	STD_CATCH_ALL("List2Map - FAILED ");

}


/*delete a map<int, list<int>* > */
void SLRIMisc::FreeMap ( map<int, list<int>* > &m)
{
   map<int, list<int>* >::iterator it;
   for (it = m.begin() ; it != m.end(); it++)
   {
       list<int> *valueList= it->second;
       if (valueList != NULL)
       {
           list<int>::iterator i;
           for (i=valueList->begin(); i!=valueList->end(); i++){
              delete &i;
	   }
       }
   }
   delete &m;
}


/*converts a list<string> to a map<int, CSLRIFasta*> object
  return this map
 */
void SLRIMisc::List2Map (list<string> &v, map<int, CSLRIFasta*> &m)
{
   int akey;
   string key, value;
   TSList::iterator it;
   for (it = v.begin(); it != v.end(); it++)
   {
	TSList q_r;
	SLRIMisc::StrStr2ListStr(&q_r, *it, Q_R);

	TSList::iterator ic;
	for (ic = q_r.begin(); ic != q_r.end(); ic++)
	{
	    key = *ic;
	    value = *(++ic);
	    akey = NStr::StringToInt(key);

	    if (!NStr::CompareNocase(value, "NULL"))
	    {
	        m[akey] = NULL;
	    }
	    else {
	       CSLRIFasta *f = SLRIMisc::Str2Fasta(value);
	       m[akey] = f;
	    }
        }
    }
}

/*converts a string to SLRI fasta object
  return a pointer to CSLRIFasta
 */
CSLRIFasta* SLRIMisc::Str2Fasta(string &s)
{
	CSLRIFasta *f = new CSLRIFasta();
	SIZE_TYPE pos=0;
	try{
		while ((pos=s.find('\1',pos))!=NPOS)
		{
			s.replace(pos, 1,"*");
			++pos;
		}
		istringstream outstr(s);
		auto_ptr<CObjectIStream> fasta_in(CObjectIStream::Open(eSerial_AsnText,outstr));
		*fasta_in >> *f;
	}
	STD_CATCH_ALL("Failed to read: SLRIMisc::Str2Fasta.\n");
	return f;
}


END_SCOPE(slri)

/*
$Log: slri_misc.cpp,v $
Revision 1.5  2005/03/02 20:20:37  ryao
minor change of STD_CATCH_ALL statement

Revision 1.4  2005/02/28 16:58:03  dbetel
Modified parsing functions to deal with new delimiter #;#

Revision 1.3  2004/06/15 17:58:43  ryao
Changed prototype of function List2Map and further modified the function; added FreeMap function.

Revision 1.2  2004/04/05 18:36:59  ryao
Added helper functions for keyed list function. Added List2Map  Str2Fasta

Revision 1.1.1.9  2003/05/24 08:36:02  root
Automated import from Sourceforge.net

Revision 1.10  2003/05/23 20:02:12  haocl
removed the dependency on ncbi toolkit

Revision 1.9  2003/03/14 18:01:47  haocl
added function to copy valnode to list
used to bridge C valnode with C++ list object

Revision 1.8  2002/11/15 16:15:35  betel
Fixed compile warnings

*/

