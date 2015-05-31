/***************************************************************************
 The Blueprint Initiative
 522 University Avenue, 9th Floor, Suite 900
 Toronto, Ontario, Canada, M5G 1W7

 Hogue Lab - University of Toronto Biochemistry Department
 Samuel Lunenfeld Research Institute, Mount Sinai Hospital

 Copyright Notice:

 Copyright 2003, 2004 Mount Sinai Hospital (MSH)

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

  PROGRAM:  testing for Seqhound remote C++ API functions

  AUTHORS:  Rong Yao

***********************************************************************
*/
#include "SeqhoundTest.hpp"
#include <Seqhound.hpp>
#include <slri_misc.hpp>

USING_NCBI_SCOPE;
using namespace objects;
using namespace slri;
using namespace seqhound;
using namespace std;

void SeqhoundTest::TestInitialization() {

  SeqHound shound;
  
  string func = "SHoundInit";
  cout << func << endl;
  string input = "true & test";
  int ret =  shound.SHoundInit(true, "test");
  checkValue (func, input, ret, 1);
  
 
  func = "SHoundIsNetEntrezOn";
  cout << func << endl;
  input = "NULL";
  ret = shound.SHoundIsNetEntrezOn(); 
  checkValue (func, input, ret, 1);
   		
  func =  "SHoundNetEntrezInit";
  cout << func << endl;
  input = "test"; 
  ret = shound.SHoundNetEntrezInit(input); 
  checkValue (func, input, ret, 1);
  
  func = "SHoundGetCurrentServer"; 
  input = "NULL";
  cout << func << endl;
  string expectedStr = "seqhound.blueprint.org";
  string retStr =  shound.SHoundGetCurrentServer();
  checkValue (func, input, retStr, expectedStr);
}


void SeqhoundTest::TestCoreFunctions ()
{
   SeqHound shound;
   shound.SHoundInit(true, "test");
   
   string func = "SHoundFindAcc";
   string input = "P00951";
   int ret = shound.SHoundFindAcc(input);
   int expected = 135198;
   checkValue (func, input, ret, expected);

   ret = shound.SHoundFindAcc(NcbiEmptyString);
   checkValue (func, "NcbiEmptyString", ret, -1);

}

void SeqhoundTest::TestLocusLinkFunctions ()
{
   SeqHound shound;
   shound.SHoundInit(true, "test");
   
   int neg1 = -1;
   string neg1Str = NStr::IntToString(neg1);
   list<int> *emptyIntList = new list<int>;

   string func = "SHoundGOIDFromLLID";
   cout << func << endl;
   int input = 1;
   string inputStr = NStr::IntToString(input);
   list<int> *retList = shound.SHoundGOIDFromLLID(input);
   /* only check the length of the return list */
   int expLen = 3;
   checkValue (func, inputStr, (int)retList->size(), expLen);
   delete &retList;

   
   func = "SHoundOMIMFromLLID";
   cout << func << endl;
   input = 1;
   inputStr = NStr::IntToString(input);
   int ret = shound.SHoundOMIMFromLLID(input);
   int expected = 138670;
   checkValue (func, inputStr, ret, expected);
   ret = shound.SHoundOMIMFromLLID(neg1);
   checkValue (func, neg1Str, ret, -1);


   func = "SHoundCDDIDFromLLID";
   cout << func << endl;
   input = 2;
   inputStr = NStr::IntToString(input);
   list<string> *retStrList = shound.SHoundCDDIDFromLLID(input);
   expLen = 3;
   checkValue (func, inputStr, (int)retStrList->size(), expLen);
   delete &retStrList;
   retStrList = shound.SHoundCDDIDFromLLID (neg1);
   /*return an empty list, so check if size == 0 */
   checkValue (func, neg1Str, (int)retStrList->size(), 0);
   delete &retStrList;
  
   
   func = "SHoundLLIDFromGOIDAndECode";
   cout << func << endl;
   input = 6810;
   string ecode = "IEA";
   retList = shound.SHoundLLIDFromGOIDAndECode(input, ecode);
   expLen = 2390;
   checkValue (func, "6810 & IEA", (int)retList->size(), expLen);
   delete &retList; 
   retList = shound.SHoundLLIDFromGOIDAndECode (neg1, ecode);
   checkValue (func, "-1 & IEA", (int)retList->size(), 0);
   delete &retList;
   retList = shound.SHoundLLIDFromGOIDAndECode (input, NcbiEmptyString);
   checkValue(func, "6810 & NcbiEmptyString", (int)retList->size(), 0);
   delete &retList;
 
   
   func = "SHoundLLIDFromOMIM";
   cout << func << endl;
   input = 103950;
   inputStr = NStr::IntToString(input);
   retList = shound.SHoundLLIDFromOMIM(input);
   expected = 2;
   checkValue (func, inputStr, retList->front(), expected);
   delete &retList;
   retList = shound.SHoundLLIDFromOMIM(neg1);
   checkValue (func, neg1Str, (int)retList->size(), 0);
   delete &retList;

   
   func = "SHoundLLIDFromCDDID";
   inputStr = "pfam00664";
   retList = shound.SHoundLLIDFromCDDID(inputStr);
   expected = 83;
   checkValue (func, inputStr, (int)retList->size(), expected);
   delete &retList;
   retList = shound.SHoundLLIDFromCDDID(NcbiEmptyString);
   checkValue (func, "NcbiEmptyString", (int)retList->size(), 0); 
   delete &retList;
   
   func = "SHoundGOIDFromGiList";
   inputStr = "4557225,123,666";
   list<int> inputList;
   SLRIMisc::StrInt2ListInt(&inputList, inputStr);
   retList = shound.SHoundGOIDFromGiList(inputList);
   checkValue (func, inputStr, (int)retList->size(), 12);
   delete retList;
   retList = shound.SHoundGOIDFromGiList(*emptyIntList);
   checkValue (func, "emIntList", (int)retList->size(), 0);
   delete retList; 
   delete emptyIntList;
   
}


void SeqhoundTest::TestKeyedListFunctions ()
{
   SeqHound shound;
   shound.SHoundInit(true, "test");
   
   list<int> *emptyIntList = new list<int>;
   
   
   string func = "SHoundGetFastaKeyedList";
   cout << func << endl;
   string str = "123,44,666,4557225";
   list<int> *inputList = new list<int>;
   SLRIMisc::StrInt2ListInt(inputList, str);
   map <int, CSLRIFasta*> *m;
   m = shound.SHoundGetFastaKeyedList(*inputList);
   map<int, CSLRIFasta* >::iterator ic;
   ic = m->find(44);
   string value = ((CSLRIFasta*)ic->second)->GetDefline();
   int pos = (int) value.find("gi|44|emb|", 0);
   /*map size always equals to the input list size, so here only test on one not-null query, 
     partially check if get the correct data */
   checkValue (func, str, pos, 0); 
   delete &m; 
   m = shound.SHoundGetFastaKeyedList(*emptyIntList);
   checkValue (func, "emptyList", (int)m->size(), 0); 
   delete &m;
   
   
   func = "SHoundRedundantGroupKeyedList";
   cout << func << endl;
   map<int, list<int>* > *m2;
   m2 =  shound.SHoundRedundantGroupKeyedList(*inputList);
   map<int, list<int>* >::iterator it;
   it = m2->find(4557225);
   checkValue (func, str, (int)(it->second)->size(),5);
   SLRIMisc::FreeMap(*m2); 
   m2 = shound.SHoundRedundantGroupKeyedList(*emptyIntList);
   checkValue (func, "emptyList", (int)m2->size(), 0);
   SLRIMisc::FreeMap(*m2);
   
   
   func = "SHoundGetFastaFromRedundantGroupIDKeyedList";
   cout << func << endl;
   m = shound.SHoundGetFastaFromRedundantGroupIDKeyedList(*inputList);
   ic = m->find(123);
   value = ((CSLRIFasta*)ic->second)->GetDefline();
   pos = (int) value.find("gi|20090833|ref|NP_616908", 0);
   checkValue (func, str, pos, 0); 
   delete &m; 
   m = shound.SHoundGetFastaFromRedundantGroupIDKeyedList(*emptyIntList);
   checkValue (func, "emptyList", (int)m->size(), 0); 
   delete &m;
   delete inputList;
   
}


void SeqhoundTest::logWrite (string func, string result, string input)
{
    std::ofstream out("cxx_test.log", ios_base::app);
    out.fill(' ');
    out.width(50);
    out << left <<  func;
    
    out.fill(' ');
    out.width(25);
    out << left << result;
    
    out.fill(' ');
    out.width(40);
    out  << left << input ;
    
    out << endl;
    flush (out);
}

void SeqhoundTest::checkValue (string func, string input, int ret, int expected)
{
    if (ret == expected){
         logWrite (func, "OK", input);  
    } 
    else {
         logWrite (func, "FAIL", input);
    }
}

void SeqhoundTest::checkValue (string func, string input, string& ret, string& expected)
{
    if (NStr::CompareNocase(ret, expected) == 0)
         logWrite (func, "OK", input);   
    else 
         logWrite (func, "FAIL", input);
}

int main () 
{

  SeqhoundTest test;
  
  test.logWrite ("FUNCTION", "RESULT", "INPUT");
  test.logWrite ("--------", "------", "-----");
  
  test.TestInitialization();
  test.TestCoreFunctions();
  test.TestLocusLinkFunctions();
  test.TestKeyedListFunctions();
  
  cout << "Testing Done" << endl;
  cout << "Please see cxx_test.log for details." << endl;
  
  return 0;
  
}


