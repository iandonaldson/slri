/* $Id: reg_tester.cpp,v 1.2 2004/02/12 20:46:19 hlieu Exp $ */
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
#include <iostream>
#include <fstream>
#include <corelib/ncbistd.hpp>
#include <corelib/ncbifile.hpp>
#include <shoundrecords.hpp>
#include <seqhound.h>
#include <rpsdbapi.h>
#include <list>
#include <iterator>
#include <sstream>
using namespace ncbi;
using namespace shoundrecord;


/* 
 * format of 'inputs_reg1.txt' file
 *
 * int1 int2
 *
 * where 0 < int1 < 7
 * and int2 == int1 if int1 is 0 ... 6
 * else int2 == 6
 *
 * eg input file:
 * 0 0
 * 1 1
 * 2 2
 * 5 5
 * 6 6
 * -4 6
 * 15 6
 *
 * to understand why, see the logic of TestRegistry1 and 
 * shoundrecords.[ch]pp definition
 */



/*
 * format of 'input2_reg2.txt' file
 *
 * int1 gi1
 * where 0 < int1 < 7
 * and gi1 is a valid gi key
 *
 * eg input file
 * 1 1392161
 * 2 1769936
 * 4 4127863
 *  
 */




/* print out the record */
int TestRecords(CSHoundRecordRegistry& reg)
{
   CSHoundRecord * test;
   
   test = reg.GetDataRecord(eSeqReport, 4557225);
   
   cout << test->DisplayRecord(eTEXT) << endl; 
   return 0;
}




/*
 * TestRegistry2  
 *   test registry's ability to return the correct record.
 *   the record type is retrieved from the registry, and then the
 *   record type and record's gi is compared to the input file's.
 *
 *   returns the number of failed tests.
 */
int TestRegistry2(string input_file, string log_file, CSHoundRecordRegistry& reg)
{
   ifstream data_in(input_file.c_str(), ios::in);
   if(! data_in.is_open())
   { cerr << "Error opening file: " << input_file << endl; exit(1); }

   ofstream log_out(log_file.c_str(), ios::app);
   if(! log_out.is_open())
   { cerr << "Error opening file: " << log_file << endl; exit(1); }

   CSHoundRecord* test;
   eFormatType e = eUndef;
   int i      = 0,    /* gi          */
       j      = 0,    /* record type */
       failed = 0;

       
   while(data_in >> j >> i) 
   {
      e = static_cast<eFormatType> (j);

      /* calling the registry to get a record of type e, with gi 'i'*/
      test = reg.GetDataRecord(e, i);

      /* compare record's gi with input */
      if( i != test->GetKey() )
      {
	failed++;
        log_out << "[TestRegistry2]: Wrong record. Expected " << i << endl;  
      } else {
        log_out << "[TestRegistry2]: Correct record retrieved." << endl;

	/* display the record with html tags, right now the
	 * only html tag used is <pre>RECORD HERE</pre>*/
	cout << test->DisplayRecord(eHTML) << endl;
      }
   }
   
   data_in.close();
   log_out.close();
   return failed;
}





/*************************************************************************
 * Testing of the registry and the shoundrecords.
 * This function test the registry's ability to retrieve the correct
 * record type and clone it.  
 *************************************************************************/
int TestRegistry1(string input_file, string log_file, CSHoundRecordRegistry& reg)
{
   /* file holding our test data and results expected */
   ifstream data_in(input_file.c_str(), ios::in);
   if(! data_in.is_open())
     { cerr << "Error opening file: " << input_file << endl; exit(1); }



   /* file for logging results */
   ofstream log_out(log_file.c_str(), ios::out); 
   if(! log_out.is_open())
     { cerr << "Error opening log file: " << log_file << endl; exit(1); }


   
   CSHoundRecord* test;
   eFormatType e = eUndef;

   /* input_file has two columns, 1st column is record format requested,
    * 2nd column is record format expected. */
   int i = 0, j = 0, failed = 0;
   while( data_in >> i >> j)
   {
      e = static_cast<eFormatType>(i);

      /* get from registry a record of type e */
      test = reg.GetRecord(e);	   

      /* compare record with input record type */
      if(static_cast<eFormatType>(j) != test->RecordType())
      {
	 failed++;
         log_out << "[TestRegistry1]: Wrong type for type "<< i << endl;
      } else {
         log_out << "[TestRegistry1]: Test passed for type " << i << endl;
      }
   }


   data_in.close();   
   log_out.close();
   return failed;
}





/************************************************************************
 * a test application for registry/shoundrecord. Runs 2 test on the
 * registry class. 
 * Test1 checks that the registry will return the correct record format.
 * Test2 checks that the registry will return the correct record.
 *
 * Will print a notice if 1 or more failures occured.
 ************************************************************************/
int main()
{
   /* keep track of the number of failures */
   int failed = 0;
   
   SHoundInit(TRUE, "reg_test");


   /* the registry that holds the records */
   CSHoundRecordRegistry reg;
   reg.LoadRegistry();

   const string test1_input = "tests/inputs_reg1.txt";
   const string test2_input = "tests/inputs_reg2.txt";
   const string log_file    = "logs/registry.log";

   // failed += TestRegistry1(test1_input, log_file, reg);
   failed += TestRegistry2(test2_input, log_file, reg);
   // failed += TestRecords(reg);



   SHoundFini();
    
   /* prints a message if 1 or more failures */
   cerr << "[reg_test]: " << failed << " tests failed. " << endl;
   return 0;
}



/* 
 * $Log: reg_tester.cpp,v $
 * Revision 1.2  2004/02/12 20:46:19  hlieu
 * a catchup script for daily updates
 *
 * Revision 1.1  2003/12/04 21:11:23  hlieu
 * initial check in of registry tester
 *
 */
