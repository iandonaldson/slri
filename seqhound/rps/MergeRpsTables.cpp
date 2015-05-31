/*$Id: MergeRpsTables.cpp,v 1.3 2004/04/16 14:33:46 mmatan Exp $*/
/*
***************************************************************************
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


  PROGRAM:MergeTable -merges two rpsresults tables.


  AUTHORS: Doron Betel betel@mshri.on.ca
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
#include <dirent.h>
#include <fstream.h>
#include "rpsdb_cb_p.hpp"
#include "MergeRpsTables.hpp"
#include <d4all.hpp>

/* Compile using :
*g++ MergeRpsTables.cpp -lcbx -o MergeTables
*-I../../../codebase/intel-unix/source -L../../../codebase/intel-unix/source
*/
void OpenDataFile( char* tableName )
{
	rpsdb.open(codeBase, tableName);
	if (!rpsdb.isValid())
	{
		rpsdb.create(codeBase, tableName, f4rpsdb, t4rpsdb);
	}
	pf4rpsgi.init(rpsdb,"GI");
	pf4rpsdomid.init(rpsdb,"DOMID");
	pf4rpscddid.init(rpsdb,"CDDID");
	pf4rpsfrom.init(rpsdb,"FROM");
	pf4rpsscore.init(rpsdb,"SCORE");
	pf4rpsalignlen.init(rpsdb,"ALIGN_LEN");
	pf4rpsevalue.init(rpsdb,"EVALUE");
	pf4rpsbitscore.init(rpsdb,"BITSCORE");
	pf4rpsmisN.init(rpsdb,"MISSING_N");
	pf4rpsmisC.init(rpsdb,"MISSING_C");
	pf4rpsnumdom.init(rpsdb,"NUMDOM");

	tagrpsgi.init(rpsdb,"GI");
	tagrpsdomid.init(rpsdb, "DOMID");
	tagrpscddid.init(rpsdb, "CDDID");
	tagrpslen.init(rpsdb,"ALIGN_LEN");
	tagrpsnumdom.init(rpsdb, "NUMDOM");

	return;
}

bool AddNewRecord(RpsDBRecord& record)
{
	rpsdb.lockAll();
        rpsdb.appendStart();
        rpsdb.blank(); /*clear the values of new record */

        pf4rpsgi.assignLong(record.GetGi());
        pf4rpsdomid.assign(record.GetDomId().c_str());
	pf4rpscddid.assignLong(record.GetCddId());
	pf4rpsfrom.assignLong(record.GetFrom());
        pf4rpsalignlen.assignLong(record.GetAlignLen());
	pf4rpsscore.assignLong(record.GetScore());
        pf4rpsevalue.assignDouble((double)record.GetEvalue(),15,8);
	pf4rpsbitscore.assignDouble((double)record.GetBitScore(), 15, 8);
	pf4rpsmisN.assignLong(record.GetMissingN());
        pf4rpsmisC.assignLong(record.GetMissingC());
        pf4rpsnumdom.assignLong(record.GetNumDom());

        rpsdb.append();
        rpsdb.unlock();
	return true;
}

unsigned long ReadTable(Data4& table)
{
	/*Create a new RpsDBRecord object */
	unsigned long i=0;
	Field4 fgi(table, "GI");
	Field4 fdomname(table,"DOMID");
	Field4 fcddid(table, "CDDID");
	Field4 ffrom(table, "FROM");
	Field4 fscore(table, "SCORE");
	Field4 falignlen(table, "ALIGN_LEN");
	Field4 fevalue(table, "EVALUE");
	Field4 fbitscore(table, "BITSCORE");
	Field4 fmissn(table,"MISSING_N");
	Field4 fmissc(table, "MISSING_C");
	Field4 fnumdom(table, "NUMDOM");

	Str4large evalue, bitscore;
	Str4large name;

	RpsDBRecord record;
	record.Reset();

	for(int rc=table.top(); rc==r4success; rc=table.skip())
	{
		record.SetGi((int)fgi);
		name.assign(fdomname);
		name.trim();
		record.SetDomId(name.ptr());
		record.SetCddId((int)fcddid);
		record.SetFrom((int)ffrom);
		record.SetAlignLen((int)falignlen);
		record.SetScore((int)fscore);
		/*Note the values of last two parameter*/
		evalue.assignDouble((double)fevalue,15,8);
		record.SetEvalue(double(evalue));
		bitscore.assignDouble((double)fbitscore,15,8);
		record.SetBitScore(double(bitscore));
		record.SetMissingN((int)fmissn);
		record.SetMissingC((int)fmissc);
		record.SetNumdom ((int)fnumdom);

		AddNewRecord(record);
		i++;
	}
	return i;
}

void PrintRecords(void)
{
	int count=0;
	Str4ten evalue, bitscore;
	Str4large name, cddid;
	for(int rc=rpsdb.top(); rc==r4success; rc=rpsdb.skip())
	{
		evalue.assignDouble((double)pf4rpsevalue, 15,8);
		bitscore.assignDouble((double)pf4rpsbitscore,15,8);
		name.assign(pf4rpsdomid);
		cddid.assign(pf4rpscddid);
		cout<<"----------------------------\n";
		cout<<"Protein "<< (int)pf4rpsgi<<" Domain "<<name.str()<<"log(E-val) "<<evalue.str()<<endl;
		count++;
	}
	cout<<"Total number of records is "<<count<<endl;
	return;
}

int main( int argc, char* argv[] )
{
	struct dirent* dp=NULL;
	DIR* dfd=NULL;
	char *temp=NULL, *masterTable=NULL, *dir=NULL;
	Data4 newtable;
	unsigned long counter=0, current_size=0;
	int l=0, table_count=0;
	ofstream logfile("merge.log");

	codeBase.errOpen=0;
	codeBase.safety=0;
	codeBase.lockEnforce=1;

	if(argc<3){
		cout<<"USAGE: MergeTables <MASTER table> from smaller tables in <dir>\n";
		cout<< "Note that both table most have .dbf extension\n";
	}
	masterTable=argv[1];
	l=strlen(masterTable);
	dir=argv[2];

	/*open logfile */
	if(!logfile){
		cerr<<"unable to open logfile\n";
	}

	/*Open master table */
	OpenDataFile(masterTable);
	/*Data4 newtable(codeBase, argv[2]);
	ReadTable(newtable);*/

	/*open dir containing tables to merge */
	if((dfd=opendir(dir))==NULL){
		cerr<<"main: cannot open "<<dir;
		logfile<<"main: cannot open "<<dir;
		return -1;
	}

	while((dp=readdir(dfd))!=NULL)
	{
		temp=strchr(dp->d_name, '.');
		if( strcmp(dp->d_name,".")==0|| strcmp(dp->d_name, "..")==0 ||
			strncmp(dp->d_name,masterTable, l)==0 || temp==NULL || strcmp(temp,".dbf")!=0 )
			continue;
		else{

			/* open table to copy */
			newtable.open(codeBase,dp->d_name);
			if(!newtable.isValid()){
				cerr<<"cannot open "<<dp->d_name<<" as a table\n";
				logfile<<"failed to open "<<dp->d_name<<endl<<flush;
				return -1;
			}
			logfile<<"Merging "<<dp->d_name<<"..."<<flush;
			table_count++;
			/*read the table and copy to master */
			current_size=ReadTable(newtable);
			counter=counter+current_size;
			/* close copied table */
			newtable.close();
			logfile<<"..completed :"<<current_size<<" records added\n"<<flush;
		}
	}

	/* close master table */
	/*PrintRecords();*/
	cout<<counter<<" records were merged\n";
	logfile<<counter<<" records were merged from "<<table_count<<"tables.\n"<<flush;

	codeBase.closeAll();
	codeBase.initUndo();
	logfile.close();
	return 0;
}
/*
$Log: MergeRpsTables.cpp,v $
Revision 1.3  2004/04/16 14:33:46  mmatan
Define evalue and bitscore as Str4large structures instead of Str4ten structures
to accomodate evalues which cannot fit inside a ten character string.  This
resolves bug 2095.

Revision 1.2  2004/02/10 19:22:20  mmatan
Fixes erroneous switching of the values in the Missing_n and Missing_c fields between source and target databases.  Also, fixes rounding of bitscore and evalue fields from 8 to 6 decimal points(they are no longer rounded).  See bug 1579 for details.

Revision 1.1.1.3  2002/10/22 08:33:28  root
Automated import from Sourceforge.net

Revision 1.5  2002/10/21 21:31:13  betel
Update new table fields

Revision 1.4  2002/03/15 00:43:39  betel
Fixed evalue assignments

Revision 1.3  2002/03/12 23:05:12  betel
Added log file

Revision 1.2  2002/03/12 21:22:53  betel
Small cahnges

Revision 1.1  2002/03/12 21:12:17  betel
Moved all rpsdb relevant files to rps dir

*/
