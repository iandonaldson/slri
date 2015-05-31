/* $Id: rpsdb_cb.cpp,v 1.1.1.15 2002/11/16 09:33:47 root Exp $ */
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


  PROGRAM: RPSDB C++ CodeBase Layer


  AUTHORS:	Doron Betel betel@mshri.on.ca
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
#include <sstream>
#include <corelib/ncbistd.hpp>
#include <corelib/ncbiutil.hpp>
#include <corelib/ncbidiag.hpp>
#include <serial/serial.hpp>
#include <serial/objostr.hpp>
#include "rpsdb_cb_p.hpp"
#include "rpsdb_cb.hpp"


USING_NCBI_SCOPE;
/*********************
*RpsDBRecord functions
***********************/
ostream& operator<< (ostream& os, RpsDBRecord& record)
{
	return os<<"GI "<<record.GetGi()<<" "<<record.GetDomId()<<" "<<record.GetCddId()<<" "<<
			record.GetAlignLen()<<" "<<record.GetScore()<<" "<<record.GetEvalue()<<" "<<
				record.GetBitScore()<<" "<<record.GetMissingC()<<" "<<record.GetMissingN()<<" "<<
					record.GetNumDom()<<endl;
}

/***************************
* DomnameDBRecord functions
***************************/
void DomnameDBRecord::SetCdd(ncbi::objects::CCdd& asn1)
{
	SerialAssign<ncbi::objects::CCdd>(m_cdd, asn1);
	return;
}

ostream& operator<< (ostream& os,DomnameDBRecord& record)
{
	return os<<"Domain "<<record.GetName()<<" "<<record.GetAccession()<<" "<<
		record.GetPdbId()<<endl;
}

/***********************
*CodeBase variabels
************************/
bool glb_codebase_init=false;


/********************************
* Codebase open and close calls
*********************************/
bool InitCodeBase(void)
{
	codeBase.init();
	codeBase.safety=1;
	codeBase.errOpen=1;
	codeBase.exitTest();
	glb_codebase_init=true;
	return glb_codebase_init;
}

void FiniCodeBase(void)
{
	codeBase.closeAll();
	codeBase.initUndo();
	glb_codebase_init=false;
}

/* Has codebase been initialized? */
bool IsCodeBaseInit(void) {return glb_codebase_init;}

/* close calls */
void CloseRPSDB(void)
{
	rpsdb.close();
}

void CloseDOMNAME(void)
{
	domnamedb.close();
}

/*Open RPSDB table*/
bool OpenRPSDB(const char* tableName, bool create )
{
	if(tableName==NULL)
	{
		ERR_POST("[OpenRPSDB]: No table name given");
		return false;
	}

	if(!IsCodeBaseInit() && !InitCodeBase()){
		ERR_POST(Fatal<<"[OpenRPSDB]:Cannot initiate CodeBase for RPSDB&DOMNAME");
		return false;
	}

	if (create)
        {
                rpsdb.create(codeBase, tableName, f4rpsdb, t4rpsdb);
        }
	else{
		rpsdb.open(codeBase, tableName);
	}
	if(!rpsdb.isValid()){
		ERR_POST("[OpenRPSDB]: failed to open "<<tableName);
		return false;
	}
        pf4rpsgi.init(rpsdb,"GI");
        pf4rpsdomid.init(rpsdb,"DOMID");
	pf4rpsfrom.init(rpsdb,"FROM");
        pf4rpsalignlen.init(rpsdb,"ALIGN_LEN");
        pf4rpsevalue.init(rpsdb,"EVALUE");
        pf4rpsmisN.init(rpsdb,"MISSING_N");
        pf4rpsmisC.init(rpsdb,"MISSING_C");
        pf4rpsnumdom.init(rpsdb,"NUMDOM");
	pf4rpscddid.init(rpsdb,"CDDID");
	tagrpscddid.init(rpsdb, "CDDID");
	pf4rpsbitscore.init(rpsdb,"BITSCORE");
	pf4rpsscore.init(rpsdb,"SCORE");
	tagrpsgi.init(rpsdb,"GI");
        tagrpsdomid.init(rpsdb, "DOMID");
        tagrpslen.init(rpsdb,"ALIGN_LEN");
        tagrpsnumdom.init(rpsdb, "NUMDOM");

        return true;
}

/*Open DomNamedb */
bool OpenDOMNAME(string name, bool create)
{
	if(name.empty())
	{
		ERR_POST("[OpenDOMNAME]: No table name given");
		return false;
	}
	/*Init codeBase if not init yet*/
	if(!IsCodeBaseInit() && !InitCodeBase()){
		ERR_POST(Fatal<<"[OpenDOMNAME]:Cannot initiate CodeBase for RPSDB&DOMNAME");
		return false;
	}

	if(create){
		domnamedb.create(codeBase, name.c_str(), f4domname, t4domname);
		ERR_POST(Info<<"[OpenDOMNAME]: Creating new DomName table");
	}
	else {
		domnamedb.open(codeBase, name.c_str());
	}
	if(!domnamedb.isValid()){
		ERR_POST("[OpenDOMNAME]:failed to open "<<name);
		return false;
	}
	pf4dnaccession.init(domnamedb,"ACCESSION");
        pf4dnname.init(domnamedb,"NAME");
        pf4dnpdbid.init(domnamedb,"PDBID");
        pf4dnasn1.init(domnamedb,"ASN1");

	tagdnaccession.init(domnamedb,"ACCESSION");
        tagdnname.init(domnamedb,"NAME");
        tagdnpdbid.init(domnamedb,"PDBID");
	return true;
}

bool AppendRPSDBRecord(RpsDBRecord& record)
{
	rpsdb.lockAll();
        rpsdb.appendStart();
        rpsdb.blank(); /*clear the values of new record */

        pf4rpsgi.assignLong(record.GetGi());
        pf4rpsdomid.assign(record.GetDomId().c_str());
	pf4rpsfrom.assignLong(record.GetFrom());
        pf4rpsalignlen.assignLong(record.GetAlignLen());
        pf4rpsevalue.assignDouble((double)record.GetEvalue(),15,6);
	pf4rpsmisN.assignLong(record.GetMissingN());
        pf4rpsmisC.assignLong(record.GetMissingC());
        pf4rpsnumdom.assignLong(record.GetNumDom());
	pf4rpsbitscore.assignDouble((double)record.GetBitScore(), 15, 6);
	pf4rpsscore.assignLong(record.GetScore());
	pf4rpscddid.assignLong(record.GetCddId());
	rpsdb.append();
        rpsdb.unlock();
	return true;
}

/*Append a DomNAme record to the table
* Assignment of the Cdd ASN.1 field is not confirmed. */
bool AppendDOMNAMERecord(DomnameDBRecord& record)
{
	domnamedb.lockAll();
	domnamedb.appendStart();
	domnamedb.blank();

	pf4dnaccession.assign(record.GetAccession().c_str());
        pf4dnname.assign(record.GetName().c_str());
        pf4dnpdbid.assign(record.GetPdbId().c_str());
        /*write Cdd to strstream and assign it to
	codebase MemoField */
	CNcbiStrstream instr;
	auto_ptr<ncbi::CObjectOStream> asn_out (CObjectOStream::Open(eSerial_AsnBinary, instr));
	*asn_out<<record.GetCCdd();
	string temp=instr.str();
	if(pf4dnasn1.assign(temp.c_str()) < 0)
	{
		ERR_POST("[AppendDOMNAMERecord] can't assign Cdd asn");
		asn_out->Close();
		return false;
	}
	asn_out->Close();
	return true;
}

/*Search RPSDB. Pass a list containing only one
* object with the search keys.
* Returns a filled list */
int SearchRPSDB(list<RpsDBRecord*>& return_list)
{
	int key=0;
	int gi=0, cddid=0;
	string domid;
	int alignlen=0;
	int score=0;
	int numdom=0;
	double evalue=0.0, bitscore=0.0, record_e=0.0;
	int rc=0;
	Str4large dom_id;
	if(return_list.empty())
	{
		ERR_POST(Error<<"[SearchRPSDB] parameter list is empty");
		return -1;
	}
	list<RpsDBRecord*>::iterator i=return_list.begin();
	RpsDBRecord* record=(*i);

	/*get data from first record */
	gi=record->GetGi();
	domid=record->GetDomId();
	cddid=record->GetCddId();
	alignlen=record->GetAlignLen();
	score=record->GetScore();
	numdom=record->GetNumDom();
	evalue=record->GetEvalue();
	bitscore=record->GetBitScore();

	/*remove dummy record from list-*/
	return_list.erase(return_list.begin());

	if(gi==0 && domid.empty() && alignlen==0 && numdom==0 && cddid==0){
		ERR_POST("[SearchRPSDB]: no tags to search by");
	}
	/*searches based on gi,number of domains, or length*/
	if(gi>0 || alignlen>0 || numdom>0)
	{
		/*with GI's we have to check
		* redundancy group. It's best if the calling
		* function ensures that the 1st GI is used for
		* every redund group */
		if(gi>0)
		{
			key=gi;
			rpsdb.select(tagrpsgi);
		}
		if(alignlen>0)
		{
			key=alignlen;
			rpsdb.select(tagrpslen);
		}
		if(numdom>0)
		{
			key=numdom;
			rpsdb.select(tagrpsnumdom);
		}
		if(cddid>0)
		{
			key=cddid;
			rpsdb.select(tagrpscddid);
		}
		rpsdb.top();
		rc=rpsdb.seek(key);
		if(rc != r4success){
			delete record;
			ERR_POST(Warning<<"[SearchRPSDB]:no records found for "<<key);
			return -1;
		}
		for(rc=rpsdb.seek(key); rc==r4success; rc=rpsdb.seekNext(key))
		{
			record_e=double(pf4rpsevalue);
			if(record_e<=evalue or record_e==0){
				/*Fill record and add to list */
				record->SetGi(int(pf4rpsgi));
				dom_id.assign(pf4rpsdomid);
				dom_id.trim();
				string temp(dom_id.str());
				/*record->SetDomId((char*)temp.c_str());*/
				record->SetDomId(temp);
				record->SetFrom(int(pf4rpsfrom));
				record->SetAlignLen(int(pf4rpsalignlen));
				record->SetEvalue(double(pf4rpsevalue));
				record->SetMissingN(int(pf4rpsmisN));
				record->SetMissingC(int(pf4rpsmisC));
				record->SetNumdom(int(pf4rpsnumdom));
				record->SetScore(int(pf4rpsscore));
				record->SetBitScore(double(pf4rpsbitscore));
				record->SetCddId(int(pf4rpscddid));
				return_list.push_back(record);
				record =new RpsDBRecord();
			}
		}
		delete record;
	}

	/*search based on Domain Id or cddid */
	else {
		const char* stringkey=domid.c_str();
		rpsdb.select(tagrpsdomid);

		rpsdb.top();
		rc=rpsdb.seek(stringkey);
		if(rc != r4success){
			delete record;
			ERR_POST(Warning<<"[SearchRPSDB]:no records found for "<<stringkey);
			return -1;
		}
		for(rc=rpsdb.seek(stringkey); rc==r4success; rc=rpsdb.seekNext(stringkey))
		{
			record_e=double(pf4rpsevalue);
			if(record_e<=evalue || record_e==0){
				record->SetGi(int(pf4rpsgi));
				dom_id.assign(pf4rpsdomid);
				dom_id.trim();
				string temp(dom_id.str());
				record->SetDomId(temp);
				record->SetFrom(int(pf4rpsfrom));
				record->SetAlignLen(int(pf4rpsalignlen));
				record->SetEvalue(double(pf4rpsevalue));
				record->SetMissingN(int(pf4rpsmisN));
				record->SetMissingC(int(pf4rpsmisC));
				record->SetNumdom(int(pf4rpsnumdom));
				record->SetBitScore(double(pf4rpsbitscore));
				record->SetScore(int(pf4rpsscore));
				record->SetCddId(int(pf4rpscddid));
				return_list.push_back(record);
				record =new RpsDBRecord();
			}
		}
		delete record;
	}
	return 1;
}

/*Search by combination of tags and evalue */
int SearchRPSDBByTagsAndEvalue(std::list<RpsDBRecord*>& return_list)
{
	int gi=0;
	string domid;
	int alignlen=0;
	int numdom=0;
	double evalue=0.0;
	Str4large dom_id;
	Relate4set relation;

	/* check for empty list */
	if(return_list.empty())
	{
		ERR_POST(Error<<"[SearchRPSDB] parameter list is empty");
		return -1;
	}
	list<RpsDBRecord*>::iterator i=return_list.begin();
	RpsDBRecord* record=(*i);
	gi=record->GetGi();
	domid=record->GetDomId();
	alignlen=record->GetAlignLen();
	numdom=record->GetNumDom();
	evalue=record->GetEvalue();
	string express;
	CNcbiStrstream string_buf;
	/*remove dummy record from list-*/
	return_list.erase(return_list.begin());

	if(gi==0 && domid.empty() && alignlen==0 && numdom==0 ){
		ERR_POST("[SearchRPSDB]: no tags to search for");
	}

	/*if gi is set check other tags*/
	if(gi>0)
	{
		express="GI = " + NStr::IntToString(gi);
		express+= ".AND. ";
		if(!domid.empty()){
			express=express+"DOMID = '"+domid+ "' .AND. ";
		}else if(numdom>0){
			express=express+"NUMDOM ="+NStr::IntToString(numdom);
			express+=" .AND. ";
		}else if(alignlen>0){
			express=express+"ALIGN_LEN ="+NStr::IntToString(alignlen);
			express+= ".AND. ";
		}
	}
	/*id Domid is set check for numdom and alignlen */
	else if(!domid.empty()){
		express="DOMID= '"+domid+"' .AND. ";
		if(numdom>0){
			express=express+"NUMDOM ="+NStr::IntToString(numdom);
			express+= ".AND. ";
		}else if(alignlen>0) {
			express=express+"ALIGN_LEN ="+NStr::IntToString(alignlen);
			express+=".AND. ";
		}
	}
	else if(numdom>0){
		express="NUMDOM =";
		express+=numdom;
		express+=".AND. ";
		if(alignlen>0){
			express=express+"ALIGN_LEN ="+NStr::IntToString(alignlen);
			express+=".AND. ";
		}
	}
	/*add evalue cut off to the query expression */
	express=express+"EVALUE<"+NStr::DoubleToString(evalue);

	/*create a single relation with one table*/
	relation.init(rpsdb);
	if(!relation.isValid()){
		ERR_POST("Cannot create relation");
		return -1;
	}
	/*set a query for the relation and find all records */
	relation.querySet(express.c_str());
	for(int rc=relation.top(); rc!=r4eof; rc=relation.skip())
	{
		/*Fill record and add to list */
		record->SetGi(int(pf4rpsgi));
		dom_id.assign(pf4rpsdomid);
		dom_id.trim();
		string temp(dom_id.str());
		record->SetDomId(temp);
		record->SetFrom(int(pf4rpsfrom));
		record->SetAlignLen(int(pf4rpsalignlen));
		record->SetEvalue(double(pf4rpsevalue));
		record->SetMissingN(int(pf4rpsmisN));
		record->SetMissingC(int(pf4rpsmisC));
		record->SetNumdom(int(pf4rpsnumdom));
		return_list.push_back(record);
		record =new RpsDBRecord();
	}
	delete record;
	codeBase.unlock();
	relation.free();
	return 1;
}

int SearchDOMNAME(std::list<DomnameDBRecord*>& return_list)
{
	string accession;
	string name;
	string pdbid;
	int rc=-20;
	Str4large strfield;
	if(return_list.empty())
	{
		ERR_POST(Error<<"[SearchDOMNAME] no parameters passed");
		return -1;
	}
	list<DomnameDBRecord*>::iterator i=return_list.begin();
	DomnameDBRecord* record=(*i);
	/* collect data from record */
	accession=record->GetAccession();
	name=record->GetName();
	pdbid=record->GetPdbId();
	/*remove first dummy record */
	return_list.erase(return_list.begin());
	if(accession.empty() && name.empty() && pdbid.empty()){
		ERR_POST("[SearchDOMNAME]: no tags are set to search");
		return -1;
	}
	/*search by mmdbid */
	if(!pdbid.empty()){
		string stringkey=pdbid;
		domnamedb.select(tagdnpdbid);
		domnamedb.top();
		rc=domnamedb.seek(stringkey.c_str());
		if(rc!=r4success){
			delete record;
			ERR_POST(Info<<"[SearchDOMNAME]: no record found for pdbid "<<pdbid);
			return -1;
		}
		for (rc=domnamedb.seek(stringkey.c_str()); rc==r4success;
			rc=domnamedb.seekNext(stringkey.c_str()))
		{
			/*fill record */
			strfield.assign(pf4dnaccession);
			strfield.trim();
			string temp(strfield.str());
			record->SetAccession(temp);

			strfield.assign(pf4dnname);
			strfield.trim();
			temp=strfield.str();
			record->SetName(temp);

			strfield.assign(pf4dnpdbid);
			strfield.trim();
			temp=strfield.str();
			record->SetPdbId(temp);

			return_list.push_back(record);
                        record =new DomnameDBRecord();
		}
		delete record;
	}
	/*search by domain accession or domain's name */
	else {
		string stringkey;
		if(!accession.empty())
		{
			stringkey=accession;
			domnamedb.select(tagdnaccession);
		}
		if(!name.empty())
		{
			stringkey=name;
			domnamedb.select(tagdnname);
		}
		domnamedb.top();
		rc=domnamedb.seek(stringkey.c_str());
		if(rc!=r4success){
			delete record;
			ERR_POST(Info<<"[SearchDOMNAME]: no record found for "<<stringkey);
			return -1;
		}
		for (rc=domnamedb.seek(stringkey.c_str());
			rc==r4success;rc=domnamedb.seekNext(stringkey.c_str()))
		{
			/*fill record */
			strfield.assign(pf4dnaccession);
			strfield.trim();
			string temp(strfield.str());
			record->SetAccession(temp);

			strfield.assign(pf4dnname);
			strfield.trim();
			temp=strfield.str();
			record->SetName(temp);

			strfield.assign(pf4dnpdbid);
			strfield.trim();
			temp=strfield.str();
			record->SetPdbId(temp);

			return_list.push_back(record);
                        record =new DomnameDBRecord();
		}
		delete record;
	}
	return 1;
}

/* search domname for domain label by accession */
string SearchDOMNAME(string accession)
{
	int rc=-20;
	if(accession.empty())
	{
		ERR_POST("[SearchDOMNAME]: empty string");
		return NULL;
	}
	domnamedb.select(tagdnaccession);
	domnamedb.top();
	rc=domnamedb.seek(accession.c_str());
	if(rc!=r4success){
		ERR_POST(Info<<"[SearchDOMNAME]: no record found for "<<accession);
		return NULL;
	}
	Str4large strfield;
	strfield.assign(pf4dnname);
	strfield.trim();
	string s=strfield.str();
	return s;
}

/*Is Exist in RPSDB */
bool IsExistInRPSDB( int gi)
{
	int rc=-20;
	rpsdb.select(tagrpsgi);
	rpsdb.top();
	rc=rpsdb.seek(gi);
	return ((rc==r4success)?true:false);

}
bool IsExistInDOMNAME( string acc)
{
	int rc=-20;
	string stringkey;
	if(!acc.empty())
	{
		stringkey=acc;
		domnamedb.select(tagdnaccession);
	}
	else
		return false;
	domnamedb.top();
	rc=domnamedb.seek(acc.c_str());
	return ((rc==r4success)?true: false);
}

/*Collect list of domain from DOMNAME */
list<string>* DOMNAMECollectDomainIDs(void)
{
	list<string>* domids=new list<string>;
	int rc=r4off;
	Str4large strfield;

	for(rc=domnamedb.top(); rc==r4success; rc=domnamedb.skip(1L))
	{
		strfield.assign(pf4dnaccession);
		strfield.trim();
		domids->push_back(strfield.str());
	}
	return domids;
}
/*
$Log: rpsdb_cb.cpp,v $
Revision 1.1.1.15  2002/11/16 09:33:47  root
Automated import from Sourceforge.net

Revision 1.16  2002/11/15 16:55:15  betel
Fixed GCC3.2 compile errors

Revision 1.15  2002/11/14 19:51:44  betel
Fixed bugs in searches and new function

Revision 1.14  2002/11/01 19:46:20  betel
Small changes

Revision 1.13  2002/11/01 00:07:19  betel
Minor changes

Revision 1.12  2002/10/30 18:21:20  betel
Added IsExist* functions

Revision 1.11  2002/10/28 23:41:01  betel
Removed #ifdef NEW_RPSDB, new tables fields are in effect

Revision 1.10  2002/10/26 20:57:56  betel
Fixed SearchRPSDB

Revision 1.9  2002/10/25 00:59:49  betel
Added 3D function and changes to DOMNAME table

Revision 1.8  2002/10/22 23:06:38  betel
Added ifdef for compatibility with old RPSDB tables

Revision 1.7  2002/10/21 21:30:19  betel
Code update to new table fields

Revision 1.6  2002/08/07 19:29:03  betel
Small changes to api

Revision 1.5  2002/05/06 20:45:32  betel
Added more function annotation

Revision 1.4  2002/04/15 20:01:19  betel
Added and Changed search strategies

Revision 1.3  2002/04/02 00:37:43  betel
Added relation query to search with multiple tags

Revision 1.2  2002/03/21 19:42:11  betel
Fixed search function

Revision 1.1  2002/03/21 02:50:34  betel
RPSDB C++ CodeBase layer

*/
