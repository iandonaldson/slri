/*$Id: rpsdbapi.cpp,v 1.1.1.19 2002/12/05 09:35:25 root Exp $ */
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


  PROGRAM: RPSDB & DOMNAME C++ API Layer


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

#include <corelib/ncbistd.hpp>
#include <corelib/ncbiutil.hpp>
#include <math.h>
#include <slri_misc.hpp>
#include "rpsdbapi.hpp"

#define LOG_DEFAULT_EVALUE 100

USING_NCBI_SCOPE;
using namespace slri;

bool glb_is_rpsdb_open=false;
bool glb_is_domname_open=false;

/******************
*Private functions
******************/
/* remove domains where (len alignment)/(len domain)< threshold
* In some cases a small portion of the domain is aligned with low e-value
* those domains are removed */
bool RemoveLowCoverageDomains(list<RpsDBRecord*>& domains, double threshold=0.6);

/*Initialize rpsdb, table name and path
* is stored in config file */
bool InitRPSDB(string table_name)
{
	if(table_name.empty()){
		ERR_POST("[InitRPSDB]: no table name given");
		return false;
	}

	/* note the order of the conditions !! */
	if(!IsCodeBaseInit() && !InitCodeBase()){
		ERR_POST("[InitRPSDB]: CodeBase open failed");
		return false;
	}
	if(!OpenRPSDB(table_name.c_str(), false)){
		FiniCodeBase();
		glb_is_rpsdb_open=false;
		ERR_POST("[InitRPSDB]: RPSDB failed to open "<<table_name);
		return false;
	}
	glb_is_rpsdb_open=true;
	return glb_is_rpsdb_open;
}

/*Initialize domname */
bool InitDOMNAME(string table)
{
	if(table.empty()){
		ERR_POST("[InitDOMNAME]: no table name is given");
		return false;
	}
	if(!IsCodeBaseInit() && !InitCodeBase())
	{
		ERR_POST("[InitDOMNAME]: CodeBase open failed");
		return false;
	}
	if(!OpenDOMNAME(table.c_str(), false)){
		FiniCodeBase();
		glb_is_domname_open=false;
		ERR_POST("[InitDOMNAME]: failed to open Domname table "<<table);
		return false;
	}
	glb_is_domname_open=true;
	return glb_is_domname_open;
}

bool FiniRPSDB(void)
{
	CloseRPSDB();
	glb_is_rpsdb_open=false;
	return glb_is_rpsdb_open;
}

bool FiniDOMNAME(void)
{
	CloseDOMNAME();
	glb_is_rpsdb_open=false;
	return glb_is_rpsdb_open;
}

/*close RPSDB and DOMNAME tables*/
void CloseTables(void)
{
	FiniRPSDB();
	FiniDOMNAME();
	FiniCodeBase();
	return;
}

/*bool calls to check table status */
bool IsRPSDBOpen(void) { return glb_is_rpsdb_open; }
bool DOMNAMEAreYouListening(void) { return glb_is_domname_open;}

/* Get the list of domains for a protein*/
list<RpsDBRecord*>* GetDomainsFromGi(int gi)
{
	RpsDBRecord* record=NULL;
	list<RpsDBRecord*>* results_list=new list<RpsDBRecord*>;
	if(gi<=0)
	{
		ERR_POST("[GetDomainsFromGi] illegal gi value"<<gi);
		delete results_list;
		return NULL;
	}
	record= new RpsDBRecord();
	record->SetGi(gi);
	record->SetEvalue(LOG_DEFAULT_EVALUE);
	results_list->push_back(record);
	if(SearchRPSDB(*results_list)!=1)
	{
		ERR_POST(Warning<<"[GetDomainsFromGi] SearchRPSDB failed for "<<gi);
		delete results_list;
		return NULL;
	}
	/*remove low coverage */
	/*if(!RemoveLowCoverageDomains(*results_list)){
		ERR_POST("[GetDomainsFromGi] failed to remove low coverage");
	}*/
	return results_list;
}

/*Get a list of domains from GI with Evalue <= cutoff
* Note that cutoff value can be zero
* It is the calling functions responsibility to free the list
* If return value is NULL then no need to free list.*/
list<RpsDBRecord*>* GetDomainsFromGiWithEvalue(int gi, double cutoff)
{
	double log_cutoff= (cutoff==0.0)?-300.0: log10(cutoff);
	list<RpsDBRecord*>* results_list=new list<RpsDBRecord*>;
	if(gi<=0)
	{
		ERR_POST("[GetDomainsFromGiWithEvalue]: invalid value="<<gi);
		delete results_list;
		return NULL;
	}
	RpsDBRecord* record= new RpsDBRecord();
	record->SetGi(gi);
	record->SetEvalue(log_cutoff);
	results_list->push_back(record);
	if(SearchRPSDB(*results_list)!=1)
	{
		ERR_POST(Warning<<"[GetDomainsFromGiWithEvalue]: search failed for "
				<<gi<<"and cutoff "<<cutoff);

		for(list<RpsDBRecord*>::iterator i=results_list->begin();
			i!=results_list->end(); i++){
			delete *i;
		}
		delete results_list;
		return NULL;
	}
	/*remove low coverage */
	/*if(!RemoveLowCoverageDomains(*results_list)){
		ERR_POST("[GetDomainsFromGiWithEvalue] failed to remove low coverage");
	}*/
	return results_list;
}

/* Get the list of Domain ID's from GI with eValue*/
list<string>* GetDomainIDsFromGiWithEvalue(int gi, double cutoff)
{
	if(gi<=0)
	{
		ERR_POST("[GetDomainIDsFromGiWithEvalue]: invalid value="<<gi);
		return NULL;
	}

	list<RpsDBRecord*>* results=NULL;
	list<string>* domain_ids=new list<string>;
	results=GetDomainsFromGiWithEvalue(gi, cutoff);

	if(results==NULL){
		ERR_POST(Warning<<"[GetDomainIDsFromGiWithEvalue] No domains found for "<<gi);
		delete domain_ids;
		return NULL;
	}

	list<RpsDBRecord*>::const_iterator i;
	for(i=results->begin(); i!=results->end(); i++)
	{
		string temp((*i)->GetDomId());
		domain_ids->push_back(temp);
	}

	SLRIMisc::FreeList(*results);
	delete results;
	return domain_ids;
}

/* Get a list of Gis from DomainId
* returns the unique list of GI's. */
list<int>* GetGisFromDomainId(string domid)
{
	RpsDBRecord* record=NULL;
	if(domid.empty()){
		ERR_POST(Warning<<"[GetGisByDomainId] no domid provided");
		return NULL;
	}

	list<RpsDBRecord*>* results_list=new list<RpsDBRecord*>;
	record= new RpsDBRecord();
	record->SetDomId(domid);
	record->SetEvalue(LOG_DEFAULT_EVALUE);
	results_list->push_back(record);

	if(SearchRPSDB(*results_list)!=1)
	{
		ERR_POST(Warning<<"[GetGisByDomainId]SearchRPSDB failed for "<<domid);
		//delete record;
		//results_list->pop_front();
		//if(!results_list->empty())
		delete results_list;
		return NULL;
	}

	/*collect GIs into an int list*/
	list<int>* gis=new list<int>;
	std::list<RpsDBRecord*>::iterator i;
	int gi=-1;
	for(i=results_list->begin(); i!=results_list->end(); i++)
	{
		/*Generate a unique list of gi-
		Some domains appear more than once in a protein
		We only report the unique list */
		const int x=(**i).GetGi();
		if(gi!=x){
			gi=x;
			gis->push_back(x);
		}
	}
	/*FreeList(*results_list);*/
	delete results_list;

	return gis;
}
/*overloaded function- retrieves GI's from DomId's with cutoff value */
list<int>* GetGisFromDomainId(string domid, double cutoff)
{
	double log_cutoff= (cutoff==0.0)?-300.0: log10(cutoff);
	int gi=0;
	if(domid.empty())
	{
		ERR_POST("[GetGisFromDomainId]: invalid domid");
		return NULL;
	}

	list<RpsDBRecord*>* results_list=new list<RpsDBRecord*>;
	RpsDBRecord* record= new RpsDBRecord();
	record->SetDomId(domid);
	record->SetEvalue(log_cutoff);
	results_list->push_back(record);

	if(SearchRPSDB(*results_list)!=1)
	{
		ERR_POST(Warning<<"[GetGisFromDomainId]: search failed for "
				<<domid<<"and cutoff "<<cutoff);
		delete record;
		for(list<RpsDBRecord*>::iterator i=results_list->begin();
			i!=results_list->end(); i++){
			delete *i;
		}
		delete results_list;
		return NULL;
	}

	list<int>* gis=new list<int>;
	std::list<RpsDBRecord*>::iterator i;

	for(i=results_list->begin(); i!=results_list->end(); i++)
	{
		const int x=(**i).GetGi();
		if(gi!=x){
			gi=x;
			gis->push_back(x);
		}
	}
	/*FreeList(*results_list);*/
	delete results_list;

	return gis;
}

/*Retrieve GIs that contain x domains or more */
list<int>* GetGisByNumberOfDomains(int x)
{
	RpsDBRecord* record=NULL;
	if(x<1){
		ERR_POST(Warning<<"[GetGisByNumberOfDomains] invalid parameter "<<x);
		return NULL;
	}

	list<RpsDBRecord*>* results_list=new list<RpsDBRecord*>;
	record= new RpsDBRecord();
	record->SetNumdom(x);
	record->SetEvalue(LOG_DEFAULT_EVALUE);
	results_list->push_back(record);

	if(SearchRPSDB(*results_list)!=1)
	{
		ERR_POST(Warning<<"[GetGisByNumberOfDomains]SearchRPSDB failed. ");
		delete record;
		results_list->erase(results_list->begin());
		delete results_list;
		return NULL;
	}

	/*collect GIs into an int list*/
	list<int>* gis=new list<int>;
	std::list<RpsDBRecord*>::iterator i;

	for(i=results_list->begin(); i!=results_list->end(); i++)
	{
		const int x=(**i).GetGi();
		gis->push_back(x);
	}
	/*FreeList(*results_list);*/
	delete results_list;

	return gis;
}

/*Retrieve GIs that contain x domains or more with cutoff */
list<int>* GetGisByNumberOfDomains(int x, double cutoff)
{
	double log_cutoff= (cutoff==0.0)?-300.0: log10(cutoff);
	if(x<1)
	{
		ERR_POST("[GetGisByNumberOfDomains]: invalid parameter "<<x);
		return NULL;
	}

	list<RpsDBRecord*>* results_list=new list<RpsDBRecord*>;
	RpsDBRecord* record= new RpsDBRecord();
	record->SetNumdom(x);
	record->SetEvalue(log_cutoff);
	results_list->push_back(record);

	if(SearchRPSDB(*results_list)!=1)
	{
		ERR_POST(Warning<<"[GetGisByNumberOfDomains]: search failed. ");
		record=NULL;
		for(list<RpsDBRecord*>::iterator i=results_list->begin();
			i!=results_list->end(); i++){
			delete *i;
		}
		delete results_list;
		return NULL;
	}

	list<int>* gis=new list<int>;
	std::list<RpsDBRecord*>::iterator i;
	int gi;
	for(i=results_list->begin(); i!=results_list->end(); i++)
	{
		gi=(**i).GetGi();
		gis->push_back(x);
	}
	/*FreeList(*results_list);*/
	delete results_list;

	return gis;
}

/* remove domains where (len alignment)/(len domain)< threshold
* In some cases a small portion of the domain is aligned with low e-value
* those domains are removed */
bool RemoveLowCoverageDomains(list<RpsDBRecord*>& domains, double threshold)
{
	double c_missing=0.0, n_missing=0.0, len=0.0;
	if(domains.empty()){
		ERR_POST(Warning<<"[RemoveLowCoverageDomains]: list is empty");
		return false;
	}
	list<RpsDBRecord*>::iterator i, prev;
	for(i=domains.begin(), prev=domains.begin(); i!=domains.end(); ++i){
		c_missing=(double)(*i)->GetMissingC();
		n_missing=(double)(*i)->GetMissingN();
		len=(double)(*i)->GetAlignLen();
		if((len/(n_missing+len+c_missing) )<threshold){
			delete *i;
			domains.erase(i);
			i=prev;
		}else{
			prev=i;
		}
	}
	return true;
}
/*********************
* DOMNAME calls
**********************/
/*retrieve domain's short name by domain id */
string GetDomainLabelFromDomainId(string s)
{
	if(s.empty())
	{
		ERR_POST("[GetDomainLabelFromDomainId]: no id passed");
		return NULL;
	}
	/*call overloaded SearchDOMNAME*/
	return SearchDOMNAME(s);
}

/*retrieve domain's Ids (pfams, smart) by domains short name */
list<string>* GetDomainIdFromName(string name)
{
	if(name.empty())
	{
		ERR_POST("[GetDomainIdFromName]: no name passed");
		return NULL;
	}
	list<DomnameDBRecord*>* return_list=new list<DomnameDBRecord*>;
	DomnameDBRecord* record=new DomnameDBRecord();
	record->SetName(name);
	return_list->push_back(record);
	list<DomnameDBRecord*>::iterator i;
	if(SearchDOMNAME(*return_list)!=1){
		/*if search failed delete list*/
		for(i=return_list->begin();i!=return_list->end(); i++){
			delete *i;
		}
		delete return_list;
		ERR_POST(Warning<<"No domain id found for "<<name);
	}
	/*collect domain's ids from the database search */
	list<string>* acc=new list<string>;
	list<DomnameDBRecord*>::iterator j;
	for( j=return_list->begin();j!=return_list->end(); j++){
		acc->push_back((*j)->GetAccession());
	}
	/* delete and return */
	for(i=return_list->begin();i!=return_list->end(); i++){
		delete *i;
	}
	delete return_list;
	return acc;
}

/*Get 3D PDB-ID for domain from domain ID*/
string Get3DStructureFromDomainId(string s)
{
	if(s.empty())
	{
		ERR_POST("[Get3DStructureFromDomainId]: no id passed");
		return NULL;
	}
	list<DomnameDBRecord*>* return_list=new list<DomnameDBRecord*>;
	DomnameDBRecord* record=new DomnameDBRecord();
	record->SetPdbId(s);
	return_list->push_back(record);
	list<DomnameDBRecord*>::iterator i;
	if(SearchDOMNAME(*return_list)!=1){
		/*if search failed delete list*/
		for(i=return_list->begin();i!=return_list->end(); i++){
			delete *i;
		}
		delete return_list;
		ERR_POST(Warning<<"No 3D structure found for "<<s);
	}

	i=return_list->begin();
	string structure=(*i)->GetAccession();
	delete return_list;
	return structure;
}

/*Get the list of proteins that contain domain name with some evalue
* note that if no evalue is passed then there is virtually no evalue
* filtering.
* returns the UNIQUE GI list of proteins containing domain <label> */
list<int>* GetGIsByDomainLabel(string label, double evalue)
{
	if(label.empty())
	{
		ERR_POST("[GetGIsByDomainLabel]: no name passed");
		return NULL;
	}
	list<string>* acc=GetDomainIdFromName(label);
	list<int>* GIs=new list<int>;
	list<int>* temp=new list<int>;
	for(list<string>::iterator i=acc->begin(); i!=acc->end(); i++)
	{
		/*concatenate the GI lists*/
		string accession=*i;
		temp=GetGisFromDomainId(accession, evalue);
		ERR_POST(Info<<"Size of GI list from "<<(*i)<<"is "<<temp->size());
		temp->sort();
		GIs->merge(*temp);

		delete temp;
	}
	delete acc;
	GIs->unique();
	ERR_POST(Info<<"Size of unique lists is "<<GIs->size());
	return GIs;

}

/*Get a list of all domain ID currently in table*/
list<string>* GetAllDomainIDs(void)
{
	return DOMNAMECollectDomainIDs();
}

/*
$Log: rpsdbapi.cpp,v $
Revision 1.1.1.19  2002/12/05 09:35:25  root
Automated import from Sourceforge.net

Revision 1.19  2002/12/04 18:56:52  betel
Fixed memory bug

Revision 1.18  2002/11/15 16:55:14  betel
Fixed GCC3.2 compile errors

Revision 1.17  2002/11/14 19:51:44  betel
Fixed bugs in searches and new function

Revision 1.16  2002/11/01 19:46:20  betel
Small changes

Revision 1.15  2002/11/01 00:07:18  betel
Minor changes

Revision 1.14  2002/10/30 18:23:39  betel
Added slri namespace and changed path in Make

Revision 1.13  2002/10/28 23:11:20  betel
Added RemoveLowCoverageDomains to some functions

Revision 1.12  2002/10/25 00:59:50  betel
Added 3D function and changes to DOMNAME table

Revision 1.11  2002/10/18 22:07:20  betel
Fixed DOMNAMEAreYouListening :)

Revision 1.10  2002/08/07 19:29:02  betel
Small changes to api

Revision 1.9  2002/08/06 16:21:31  betel
Added SLRIMisc static call

Revision 1.8  2002/07/08 16:56:02  kaca
renamed two RPS-BLAST functions Accession is DomainId

Revision 1.7  2002/05/09 18:53:48  betel
Moved utility template functions to slri_misc.hpp

Revision 1.6  2002/05/03 22:57:45  betel
Added a call to check for GetDomainIDsFromGiWithEvalue

Revision 1.5  2002/05/02 21:29:35  betel
Added a new function and correct memory clean up

Revision 1.4  2002/04/19 21:41:48  betel
More Print functions and small changes

Revision 1.3  2002/04/15 20:01:59  betel
Added api functions

Revision 1.2  2002/04/02 00:39:09  betel
Added more API functions

Revision 1.1  2002/03/21 02:51:10  betel
RPSDB C++ API layer

*/
