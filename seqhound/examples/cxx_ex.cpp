/* $Id: cxx_ex.cpp,v 1.1.1.1 2002/08/24 08:29:29 root Exp $
***************************************************************************
Mount Sinai Hospital (MSH)

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


  PROGRAM:


  AUTHORS:
           Hao Lieu and Christopher W.V. Hogue
           hogue@mshri.on.ca


  REVISION/CONTRIBUTION NOTES:

  June 1, 2000  Original MSH Revision.
                Hogue Lab - University of Toronto Biochemistry
                Department and the Samuel Lunenfeld Research
                Institute, Mount Sinai Hospital
                http://bioinfo.mshri.on.ca  hogue@mshri.on.ca
                ftp://ftp.mshri.on.ca/


  PUBLICATION TO CITE:


  DESCRIPTION
  prints out FASTA protein database for a complete genome;
  input value is a taxonomy ID for the genome

*****************************************************************************/
#include "cxx_ex.hpp"
#include <Seqhound.hpp>
#include <corelib/ncbistre.hpp>

USING_NCBI_SCOPE;
using namespace objects;
using namespace slri;
using namespace seqhound;

/* initialize all command line arguments specifications */
void cxx_ex::Init(){
  auto_ptr<CArgDescriptions> arg_descript(new CArgDescriptions);
  arg_descript->SetUsageContext(GetArguments().GetProgramBasename(),
				"example c++ program");
  /* create an optional parameter that has a default value */
  arg_descript->AddDefaultKey("gi", "OptionalGI",
		       "This is an optional gi value",
		       CArgDescriptions::eInteger, "2392601");
  CArgAllow* constraint = new CArgAllow_Integers(0, 1000000000);
  arg_descript->SetConstraint("gi", constraint);
  SetupArgDescriptions(arg_descript.release());
}

int cxx_ex::Run() {

  /* retrieve the command line arguments */
  CArgs args = GetArgs();

  /* set the log level to info, and specify a log file */
  SetDiagPostLevel(eDiag_Info);
  CNcbiOfstream logout("cxx_ex.log");
  SetDiagStream((CNcbiOstream*)&logout, true, 0, 0);

  /* file to output all results */
  CNcbiOfstream out("cxx_ex.txt");

  /* a linked list of the gi provided as command line arg */
  list<int> stlGi;
  stlGi.push_back(args["gi"].AsInteger());

  SeqHound shound;

  if(!shound.SHoundInit(true, "cxx_ex"))
  {
    ERR_POST(Error << "Unable to Initialize Seqhound");
    return 1;
  }

  out << "SHoundGetDeflineList: " << flush;
  list<string> *stlRDef = shound.SHoundGetDeflineList(stlGi);
  SLRIMisc::PrintList(stlRDef, out);
  out << endl;

  string file = "GetGenBankff.txt";
  CNcbiOfstream ofile(file.c_str());

  out << "SHoundGetGenBankffList: written to GetGenBankff.txt" << flush;
  shound.SHoundGetGenBankffList(stlGi, ofile);
  ofile << endl;
  out << endl;
  
  out << "SHound3DFromGiList: " << flush;
  list<int> *stlR3D = shound.SHound3DFromGiList(stlGi);
  SLRIMisc::PrintList(stlR3D, out);
  out << endl;

  out << "SHound3DNeighboursFromGiList: written to 3dngilist.txt" << endl;
  CILink_set_List2 cilist;
  shound.SHound3DNeighboursFromGiList(stlGi, cilist, 1.0);
  auto_ptr<CObjectOStream>
    txt_out(CObjectOStream::Open("3dngilist.txt", eSerial_AsnText));
  *txt_out << cilist;

  shound.SHoundFini();
  ofile.close();
  out.close();
  logout.close();
  return 0; 
} 

int main(int argc, const char* argv[]){

  cxx_ex theTestApp;
  return theTestApp.AppMain(argc, argv, 0, eDS_Default, 0, "cxx_ex");
}

/*
$Log: cxx_ex.cpp,v $
Revision 1.1.1.1  2002/08/24 08:29:29  root
Automated import from Sourceforge.net

Revision 1.1  2002/08/23 19:55:49  haocl
initial import of a c++ example using SeqHound

*/

