
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

#include <slri_misc.hpp>
#include <Seqhound.hpp>

USING_NCBI_SCOPE;

class SeqhoundTest {
    public:
       void logWrite (string func, string result, string input);
       void checkValue (string func, string input, int ret, int expected);
       void checkValue (string func, string input, string& ret, string& expected);
       void TestInitialization();
       void TestCoreFunctions();
       void TestLocusLinkFunctions ();
       void TestKeyedListFunctions ();
       void TestASNFunctions ();
       void TestStructureFunctions ();
       void TestTaxonomyFunctions ();
       void TestRedundantGiFunctions ();
       void TestOMIMFunctions ();
       void TestNeighboursFunctions ();
       void TestGeneOntologyFunctions ();
       void TestConservedDomainFunctions ();
       void TestUninitializationFunctions ();
};



