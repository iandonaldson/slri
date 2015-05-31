/* $Id: logictester.cpp,v 1.2 2003/12/04 21:11:22 hlieu Exp $ */
/*******************************************************************************************
*  Copyright (C) 2000 Mount Sinai Hospital (MSH)
*
*  This program is free software; you can redistribute it and/or
*  modify it under the terms of the GNU General Public License as
*  published by the Free Software Foundation; either version 2 of
*  the License, or any later version.
*
*  This program is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*
*  See the GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with this program; if not, write to the
*         Free Software Foundation, Inc.,
*         59 Temple Place, Suite 330, Boston, MA
*         02111-1307  USA
*  or visit http://www.gnu.org/copyleft/gpl.html
*
*  SPECIAL EXCEPTIONS
*
*  As a special exception, Mount Sinai Hospital gives permission to
*  link this program with the following non-GPL programs or libraries,
*  and distribute the resulting executable, without including the source
*  code for these in the source distribution:
*
*  a) CodeBase 6.5 or greater from Sequiter Software Inc.
*
*  b) The C or C++ interface to Oracle 8.x or greater, from
*     Oracle Corporation or IBM DB2 UDB.
*
*
*  PROGRAM:
*
*  AUTHORS:
*          Hao Lieu (hlieu@blueprint.org)
*
*
*  REVISION/CONTRIBUTION NOTES:
*
*  June 1, 2000  Original MSH Revision.
*                Hogue Lab - University of Toronto Biochemistry
*                Department and the Samuel Lunenfeld Research
*                Institute, Mount Sinai Hospital
*                http://bioinfo.mshri.on.ca  hogue@mshri.on.ca
*                ftp://ftp.mshri.on.ca/
*
*
*  PUBLICATION TO CITE:
*
*
*  DESCRIPTION
*
******************************************************************************************/

#include <corelib/ncbistd.hpp>
#include <corelib/ncbifile.hpp>

#include <iostream>
#include <fstream>
#include <list>
#include <iterator>
#include <sstream>

#include <rpsdbapi.h>
#include <logic.hpp>

/*
 * test application for the logic.hpp class 
 * The test file uses a test file that takes the format of:
 *
 * 2
 * 3 1 3 4
 * 5 1 2 3 4 5
 * 0 2 1 4
 *
 * 5 1 3 1 3 4
 * 2 2 5
 * 0 0
 *
 * line 1 is the number of test in the file, in this case 2 tests,
 * each test is a group of 3 lines (in this example there are 2 sets
 * of triples. The first and second lines in the triplette are inputs
 * to a list, the first value is the size of the list 
 * (eg 3 in test 1, line 1; 5 in test 1, line 2). Following the list
 * size is the values to be input into the list, so the first list has
 * values of 1, 3, 4, the 2nd list has 1,2,3,4,5
 *
 * The last line of the triplette represents the logic to be
 * applied and the resulting list. The first value in the line
 * is the logic, 0 = AND, 1 = OR, 2 = NOT. Following the logic is
 * the list that is the correct value (size of list, then values in list). 
 * 
 * So in test 1, list1=2,3,4 && list2=1,2,3,4,5
 * will yield a list of size 2 that contains 1,4
 *
 * The test application will carry out the logic, and then compare the 
 * resulting list with the list generated from line 3 of the triplette.
 * The results are logged the the log file. The logged messages are:
 * 
 *   correct results:      Test passed for 'logic X'
 *   incorrect results:    Test failed for 'logic X'
 *
 *   
 */


using namespace ncbi;
using namespace shoundlogic;

/* the input file that is as described above. */
const string input_file = "tests/inputs_logic.txt";

/* the log file */
const string log_file   = "logs/logic.log";




/*************************************************************** 
 * GetLine
 *   reads a line from 'data_in', and populates the list 'STL' 
 *   
 ***************************************************************/
void GetLine(list<int>& STL, ifstream& data_in)
{
   int size = 0;     /* number of iterations */
   
   data_in >> size;

   /* read in and populate list */
   for(int i = 0, j = 0; i < size && data_in >> j; i++)
      { STL.push_back(j); }
}




/***********************************************************************
 * LogicTest
 *   reads in the list inputs from 'data_in', and applies
 *   the specified logicall operator on the lists. 
 *   
 *   Outputs message to 'log_out'.
 *   
 *   'test' == 1 specifies to use ApplyLogic function
 *   'test' == 0 specifies to use actual logic operators  
 *    (see logic.hpp for details)
 * 
 *   returns the number of failed tests
 ***********************************************************************/
int LogicTest(ifstream& data_in, ofstream& log_out, const int test)
{
   int    logic = 0,    /* what logic to apply */ 
         failed = 0;    /* the number of failures */
   list<int> iSTL1,     /* input list 1    */ 
             iSTL2,     /* input list 2    */
	     iResult1,  /* expected result */
	     iSTL3;     /* actual result   */

   string logic_type;   /* used in logging */

   /* read in the 2 list */
   GetLine(iSTL1, data_in);
   GetLine(iSTL2, data_in);
  
   /* read in the logic operator */ 
   data_in >> logic;

   /* read in the expected results */
   GetLine(iResult1, data_in);

   switch(logic)
   {
      /* apply the logic to the first 2 list and get the actual results */
	   
	   
      case 0: 
         iSTL3 = (test ? SHoundLogic::ApplyLogic(iSTL1, iSTL2, SHoundLogic::eAND): iSTL1 && iSTL2);
	 logic_type = "AND";
	 break;
      case 1: 
	 iSTL3 = (test ? SHoundLogic::ApplyLogic(iSTL1, iSTL2, SHoundLogic::eOR): iSTL1 || iSTL2);
	 logic_type = "OR";
	 break;
      case 2: 
	 iSTL3 = (test ? SHoundLogic::ApplyLogic(iSTL1, iSTL2, SHoundLogic::eNOT): iSTL1 - iSTL2);
	 logic_type = "NOT";
	 break;
      default: break; 
   }


   /* compare actual result with expected result 
    * is_equivalent is defined in logic.hpp */
   if(is_equivalent(iResult1, iSTL3))
   {
      log_out << "[ApplyLogicTest]: Test passed for " << logic_type << endl;
   } else {
      failed++;
      log_out << "[ApplyLogicTest]: Test failed for " << logic_type << endl;
   }
   return failed;
}




/*********************************************************************** 
 * TestLogic
 *    testing the logic functions. Does 2 things 
 *     1) opens the data input file for reading, and 
 *     2) opens the log file for appending log messages
 *     3) calls the test functions and then 
 *     4) closes the 2 files. 
 *
 *     Returns the number of failed tests. 
 *
 *     parameter 'test' specifies whether to directly use the 
 *     logic operators (&&, ||, and -) or to use ApplyLogic function
 ***********************************************************************/
int TestLogic(string data_file, string log_file, const int test)
{
   int       iTest = 0, 
           iFailed = 0;

   /* open file holding our test data and results expected */
   ifstream data_in (data_file.c_str(), ios::in);
   if(! data_in.is_open())
      { cerr << "Error opening file: " << data_file << endl; exit(1); }


   /* file holding our logs */
   ofstream log_out (log_file.c_str(), ios::app);
   if(! log_out.is_open())
      { cerr << "Error opening file: " << log_file << endl; exit(1); }

 
   /* The 1st line in the input file specifies the number of tests,
    * here reading in the number of tests */
   data_in >> iTest;

   /* loop for the duration of the number of tests */
   for(int k = 0; k < iTest; k++) 
   {
      /* enumerate the number of failures */
      iFailed += LogicTest(data_in, log_out, test);
   }

   /* clean up */ 
   data_in.close();
   log_out.close();
   return iFailed;
}




/*
 * a test application for registry/shoundrecord. Runs 2 test on the
 * registry class. 
 * Test1 checks that the registry will return the correct record format.
 * Test2 checks that the registry will return the correct record.
 *
 * Will print a notice of the failed tests.
 */
int main()
{
   int iFailed = 0;           /* count of failed tests */

   /* when the 3rd parameter is set to 0, test the 
    * logic operators directly, when 
    * set to 1, testing ApplyLogic function which applies
    * the logic operators */ 
   iFailed += TestLogic(input_file, log_file, 1);
   iFailed += TestLogic(input_file, log_file, 0);

   cerr << "[logictester]: Failed tests " << iFailed << endl;
   return 0;
}





/* 
 * $Log : $ 
 */

