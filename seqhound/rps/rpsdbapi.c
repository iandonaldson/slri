/*$Id: rpsdbapi.c,v 1.2 2004/01/06 22:32:14 skuznets Exp $*/
/*
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


  PROGRAM: RPSDB API layer


  AUTHORS:    Doron Betel           betel@mshri.on.ca
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
#include "rpsdbapi.h"

static Boolean glb_is_rpsdb_open=FALSE;
static Boolean glb_is_domname_open=FALSE;

/*Initalize RPSDB */
Boolean InitRPSDB(CharPtr database_name)
{
	if(database_name==NULL)
	{
		ErrPostEx(SEV_ERROR,0,0,"InitRPSDB: no application name provided");
		return FALSE;
	}

	if(!OpenRPSDB(database_name, FALSE))
	{
		ErrPostEx(SEV_ERROR,0,0,"InitRPSDB: Why can't I open RPSDB?, why? why?...");
		return FALSE;
	}

	else {
		ErrPostEx(SEV_ERROR,0,0,"InitRPSDB: %s is open for business.", IsNullStr(database_name));
	}

	glb_is_rpsdb_open=TRUE;
	return glb_is_rpsdb_open;
}

/*Is RPSDB open for business? */
Boolean HelloRPSDBAreYouOpen(void)
{
	return glb_is_rpsdb_open;
}

Boolean FiniRPSDB(void)
{
	if(!CloseRPSDB() ){
		ErrPostEx(SEV_ERROR, 0,0,"FiniRPSDB: can't close RPSDB");
		return FALSE;
	}
	glb_is_rpsdb_open=FALSE;
	return TRUE;
}

/*Initalize DOMNAME */
Boolean InitDOMNAME(CharPtr database_name)
{
	if(database_name==NULL)
	{
		ErrPostEx(SEV_ERROR,0,0,"InitDONAME: no application name provided");
		return FALSE;
	}

	if(!OpenDOMNAME(database_name, FALSE))
	{
		ErrPostEx(SEV_ERROR,0,0,"InitDOMNAME: Why can't I open DOMNAMEDB?, why? why?...");
		return FALSE;
	}

	else {
		ErrPostEx(SEV_ERROR,0,0,"InitDOMNAME: %s is open for business.", IsNullStr(database_name));
	}

	glb_is_domname_open=TRUE;
	return glb_is_domname_open;
}

Boolean FiniDOMNAME(void)
{
	if(!CloseDOMNAME() ){
		ErrPostEx(SEV_ERROR, 0,0,"FiniDOMNAME: can't close RPSDB");
		return FALSE;
	}
	glb_is_domname_open=FALSE;
	return TRUE;
}

/*Is DOMNAME? */
Boolean DOMNAMEAreYouListening(void)
{
	return glb_is_domname_open;
}

/**************************
*RPSDB Search Calls
**************************/
/* Get a list of */
StRpsdbPtr GetDomainsFromGi(Int4 gi)
{
	StRpsdbPtr results=NULL;

	if(gi<=0)
	{
		ErrPostEx(SEV_ERROR,0,0,"GetDomainsFromGi: gi is invalid");
		return NULL;
	}
	results=StRpsdbNew();
	results->gi=gi;
	if(SearchRPSDB(&results)!= 1)
	{
		ErrPostEx(SEV_ERROR,0,0,"GetDomainsFromGi: Failed search");
		StRpsdbFree(results);
		return NULL;
	}
	return results;
}

/*Search for proteins with domain by name */
ValNodePtr GetGisByDomainId(CharPtr domain_id)
{
	StRpsdbPtr rpsdbp=NULL;
	StRpsdbPtr head=NULL;
	Int4 gi=0;
	ValNodePtr results=NULL;
	if(domain_id==NULL)
	{
		ErrPostEx(SEV_ERROR,0,0,"GetGiByDomainId: Domain id is NULL");
		return NULL;
	}
	rpsdbp=StRpsdbNew();
	rpsdbp->dom_id=domain_id;
	if(SearchRPSDB(&rpsdbp)!= 1)
	{
		ErrPostEx(SEV_ERROR,0,0,"GetGiByDomainId: Failed search");
		StRpsdbFree(rpsdbp);
		return NULL;
	}
	head=rpsdbp;

	/*from the returned list select the gis*/
	/*gi=head->gi;*/
	while(head!=NULL)
	{
		if(gi!=head->gi){
			gi=head->gi;
			ValNodeAddInt(&results, 1,gi);
		}
		head=head->next;
	}
	return results;
}

/*Search for proteins with domain-id below cutoff evalue score */
ValNodePtr GetGisByDomainIdAndEvalue(CharPtr domain_id, FloatHi cutoff)
{
	StRpsdbPtr rpsdbp=NULL;
	StRpsdbPtr head=NULL;
	Int4 gi=0;
	ValNodePtr results=NULL;
	FloatHi log_cutoff=0.0;

	if(domain_id==NULL)
	{
		ErrPostEx(SEV_ERROR,0,0,"GetGiByDomainId: Domain id is NULL");
		return NULL;
	}
	rpsdbp=StRpsdbNew();
	rpsdbp->dom_id=domain_id;

	/*convert cutoff to log*/
	if(cutoff==0.0){
		log_cutoff=-300.0;
	} else {
		log_cutoff=LOG10(cutoff);
	}
	rpsdbp->evalue=log_cutoff;

	if(SearchRPSDBByTagsAndEvalue(&rpsdbp)!= 1)
	{
		ErrPostEx(SEV_ERROR,0,0,"GetGiByDomainIdAndEvalue: Failed search");
		StRpsdbFree(rpsdbp);
		return NULL;
	}
	head=rpsdbp;

	/*from the returned list select the gis*/
	/*gi=head->gi;*/
	while(head!=NULL)
	{
		if(gi!=head->gi){
			gi=head->gi;
			ValNodeAddInt(&results, 1,gi);
		}
		head=head->next;
	}
	return results;
}

/* Get the list of domains for a protein with score <cutoff> or lower
* a second version using codebase relation */
StRpsdbPtr GetDomainsFromGiWithEvalue(Int4 gi, FloatHi cutoff)
{
	StRpsdbPtr results=NULL;
	FloatHi log_cutoff=0.0;
	if(gi<=0)
	{
		ErrPostEx(SEV_ERROR,0,0,"GetGisByDomainId: invalid parameters");
		return NULL;
	}
	/*Create and set a new dummey RPSDB record
	* note that the values are going to be overwritten
	* by the query.*/
	results=StRpsdbNew();
	results->gi=gi;

	/*convert cutoff to log*/
	if(cutoff==0.0){
		log_cutoff=-300.0;
	} else {
		log_cutoff=LOG10(cutoff);
	}
	results->evalue=log_cutoff;

	if(SearchRPSDBByTagsAndEvalue(&results)==-1)
	{
		ErrPostEx(SEV_ERROR,0,0,"SearchRPSDBByGiAndEvalue failed");
		StRpsdbFree(results);
		return NULL;
	}
	return results;
}

/*Get the list of gi that contain domain -id and below cutoff value */
ValNodePtr GetGisByDomainIdWithEvalue(CharPtr domain_name, FloatHi cutoff)
{
	StRpsdbPtr rpsdbp=NULL;
	StRpsdbPtr head=NULL;
	FloatHi log_cutoff=0.0;
	ValNodePtr results=NULL;
	Int4 gi=0;

	if(domain_name==NULL || cutoff<0.0 )
	{
		ErrPostEx(SEV_ERROR,0,0,"GetGisByDomainIdWithEvalue: invalid parameters");
		return NULL;
	}

	/*Create and set a new dummey RPSDB record
	* note that the values are going to be overwritten
	* by the query.*/
	rpsdbp=StRpsdbNew();
	rpsdbp->dom_id=domain_name;

	/*convert cutoff to log*/
	if(cutoff==0.0){
		log_cutoff=-300.0;
	} else {
		log_cutoff=LOG10(cutoff);
	}
	rpsdbp->evalue=log_cutoff;

	/*if(SearchRPSDBByDomIdAndEvalue(&rpsdbp)==-1)*/
	if(SearchRPSDBByTagsAndEvalue(&rpsdbp)==-1)
	{
		ErrPostEx(SEV_ERROR,0,0,"SearchRPSDBByDomIdAndEvalue failed");
		StRpsdbFree(rpsdbp);
		return NULL;
	}

	head=rpsdbp;
	/*from the returned list select the gis*/
	/*gi=head->gi;*/
	while(head!=NULL)
	{
		if(gi!=head->gi){
			gi=head->gi;
			ValNodeAddInt(&results, 1,gi);
		}
		head=head->next;
	}
	return results;
}

/*Get all Gi's that have <numdom> or more domains */
ValNodePtr GetGisByNumberOfDomains(Int4 numdom)
{
	StRpsdbPtr rpsdbp=NULL;
	StRpsdbPtr head=NULL;
	ValNodePtr results=NULL;
	Int4 gi=0;

	if(numdom<=0)
	{
		ErrPostEx(SEV_ERROR,0,0,"GetDomainsFromGi: Domain id is NULL");
		return NULL;
	}
	rpsdbp=StRpsdbNew();
	rpsdbp->num_of_dom=numdom;
	if(SearchRPSDB(&rpsdbp)!= 1)
	{
		ErrPostEx(SEV_ERROR,0,0,"GetDomainsFromGi: Failed search");
		StRpsdbFree(rpsdbp);
		return NULL;
	}
	head=rpsdbp;

	/*from the returned list select the gis*/
	/*gi=head->gi;*/
	while(head!=NULL)
	{
		if(gi!=head->gi){
			gi=head->gi;
			ValNodeAddInt(&results, 1,gi);
		}
		head=head->next;
	}
	return results;
}

/*Get Short label from Domain accession name (i.e. Pfam or Smart id) */
CharPtr GetDomainLabelFromAccession(CharPtr accession)
{
	StDomNamedbPtr dmnp=NULL;
	CharPtr name=NULL;

	if(accession==NULL)
	{
		ErrPostEx(SEV_ERROR,0,0,"GetDomainLabelFromAccession: null parameter");
		return NULL;
	}
	dmnp=StDomNamedbNew();
	dmnp->accession=accession;
	if(SearchDOMNAME(&dmnp)!=1)
	{
		ErrPostEx(SEV_ERROR,0,0,"GetDomainLabelFromAccession: SearchDOMNAME failed");
		return NULL;
	}
	name=StringSave(dmnp->name);
	StDomNamedbFree(dmnp);
	return name;
}

/*Get the Domain accession (i.e. Pfam or smart id) from short name
* Note that it returns a ValNode since a domain can appear in both
* Pfam and Smart */
ValNodePtr GetDomainAccessionFromLabel(CharPtr label)
{
	StDomNamedbPtr dmnp=NULL;
	StDomNamedbPtr head=NULL;
	ValNodePtr results=NULL;
	CharPtr accession=NULL;

	if(label==NULL)
	{
		ErrPostEx(SEV_ERROR,0,0,"GetDomainAccessionFromLabel: null parameter");
		return NULL;
	}
	dmnp=StDomNamedbNew();
	dmnp->name=label;
	if(SearchDOMNAME(&dmnp)!=1)
	{
		ErrPostEx(SEV_ERROR,0,0,"GetDomainAccessionFromLabel: SearchDOMNAME failed");
		return NULL;
	}
	head=dmnp;
	while(dmnp!=NULL){
		if(dmnp->accession!=NULL){
			accession=StringSave(dmnp->accession);
			ValNodeAddStr(&results,1,accession);
		}
		dmnp=dmnp->next;
	}
	StDomNamedbFree(head);
	return results;
}
/***********************
*Relational Searches
***********************/
/*Get a list of Gi's that contain domain <label>*/
ValNodePtr GetGisByDomainLabel(CharPtr label)
{
	CharPtr accession=NULL;
	ValNodePtr domain_ids=NULL;
	ValNodePtr domid_gis=NULL;
	ValNodePtr gis=NULL;
	ValNodePtr head=NULL;
	if(label==NULL)
	{
		ErrPostEx(SEV_ERROR,0,0,"GetGisByDomainLabel:null parameter");
		return NULL;
	}
	/*gat all Domain-id from this short-label */
	domain_ids=GetDomainAccessionFromLabel(label);
	if(domain_ids==NULL)
	{
		ErrPostEx(SEV_ERROR,0,0,"GetGisByDomainLabel: could not find %s in DOMNAME", IsNullStr(label));
		return NULL;
	}
	else{
		/*collect all Gi's from each Domain-ID*/
		head=domain_ids;
		while(domain_ids!=NULL){
			domid_gis=GetGisByDomainId((char*)domain_ids->data.ptrvalue);
			ValNodeLink(&gis, domid_gis);
			domain_ids=domain_ids->next;
		}
		ValNodeFree(head);
	}
	return gis;
}

/*Get a list of Gi's that contain domain <label> with
* Evalue of <cutoff> or lower*/
ValNodePtr GetGisByDomainLabelAndEvalue(CharPtr label, FloatHi cutoff)
{
CharPtr accession=NULL;
	ValNodePtr domain_ids=NULL;
	ValNodePtr domid_gis=NULL;
	ValNodePtr gis=NULL;
	ValNodePtr head=NULL;
	if(label==NULL)
	{
		ErrPostEx(SEV_ERROR,0,0,"GetGisByDomainLabel&Eval:null parameter");
		return NULL;
	}
	/*gat all Domain-id from this short-label */
	domain_ids=GetDomainAccessionFromLabel(label);
	if(domain_ids==NULL)
	{
		ErrPostEx(SEV_ERROR,0,0,"GetGisByDomainLabel&Eval: could not find %s in DOMNAME", IsNullStr(label));
		return NULL;
	}
	else{
		/*collect all Gi's from each Domain-ID*/
		head=domain_ids;
		while(domain_ids!=NULL){
			domid_gis=GetGisByDomainIdAndEvalue((char*)domain_ids->data.ptrvalue,cutoff);
			ValNodeLink(&gis, domid_gis);
			domain_ids=domain_ids->next;
		}
		ValNodeFree(head);
	}
	return gis;
}
/*
$Log: rpsdbapi.c,v $
Revision 1.2  2004/01/06 22:32:14  skuznets
Code audit has been done sucessfully.
Possible NULL pointer issues has been eliminated.

Revision 1.1.1.2  2002/03/19 09:27:29  root
Automated import from Sourceforge.net

Revision 1.2  2002/03/18 21:57:27  betel
Added Domain Table searches and lookup

Revision 1.1  2002/03/15 22:26:52  betel
RPSDB API layer

*/
