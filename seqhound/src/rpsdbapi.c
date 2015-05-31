/*$Id: rpsdbapi.c,v 1.9 2004/06/16 20:23:15 mmatan Exp $*/
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


  PROGRAM: RPSDB & DOMNMAME C API layer


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
#include <slri_misc.h>
#include <intrez_cfg.h>
#include <rpsdb_db.h>
#include <rpsdbapi.h>
#include <intrez.h>


#define LOG_DEFAULT_EVALUE 100
#define COVERAGE_CUTOFF 0.6
static Boolean glb_is_rpsdb_open=FALSE;
static Boolean glb_is_domname_open=FALSE;

/********************************
*Private Function prototypes
*********************************/

/*Remove domains with low coverage. Where
* (len of align)/(len of domain)< threshold value */
Boolean RemoveLowCoverageDomains(StRpsdbPtr PNTR results, FloatHi threshold);

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
		ErrPostEx(SEV_INFO,0,0,"InitRPSDB: %s is open for business.", IsNullStr(database_name));
	}

	glb_is_rpsdb_open=TRUE;
	return glb_is_rpsdb_open;
}

/*Is RPSDB open for business? */
Boolean HelloRPSDBAreYouOpen(void)
{
	return glb_is_rpsdb_open;
}

/* Close RPSDB */
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
		ErrPostEx(SEV_INFO,0,0,"InitDOMNAME: %s is open for business.", IsNullStr(database_name));
	}

	glb_is_domname_open=TRUE;
	return glb_is_domname_open;
}

Boolean FiniDOMNAME(void)
{
	if(!CloseDOMNAME() ){
		ErrPostEx(SEV_ERROR, 0,0,"FiniDOMNAME: can't close DOMNAMEDB");
		return FALSE;
	}
	glb_is_domname_open=FALSE;
	return TRUE;
}

/* Is DOMNAME? */
Boolean DOMNAMEAreYouListening(void)
{
	return glb_is_domname_open;
}


/*relabel the gi in rps structure list with the given gi
 *param: gi is the value to replace the gis in the rps structure list
 *       results is a pointer to StRpsdb list 
 */
void RelabelGiForRpsList(Int4 gi, StRpsdbPtr results)
{
    StRpsdbPtr current = results;
    while (current !=NULL)
    {
    	if (current->gi != gi)
		current->gi = gi;
	current = current->next;
    }
}

/**************************
*RPSDB Search Calls
**************************/

/* Get a list of domains for GI */
StRpsdbPtr LIBCALL SHoundGetDomainsFromGi(Int4 gi)
{
	StRpsdbPtr results=NULL;
	ValNodePtr redundant_gilist=NULL, v=NULL;

	if(!SHoundModule("rpsdb") && !glb_is_rpsdb_open)
	{
		ErrPostEx(SEV_ERROR,0,0, "SHoundGetDomainsFromGi: Missing required SeqHound module.");
		return NULL;
	}

	if(gi<=0)
	{
		ErrPostEx(SEV_ERROR,0,0,"SHoundGetDomainsFromGi: gi is invalid");
		return NULL;
	}
	results=StRpsdbNew();
	results->gi=gi;
	/*Set evalue to default */
	results->evalue=LOG_DEFAULT_EVALUE;
	if(SearchRPSDB(&results)!= 1)
	{	
		redundant_gilist = SHoundRedundantGroup(gi);
		v = redundant_gilist;
		while(v!=NULL)
		{
			results->gi = v->data.intvalue;
			if (SearchRPSDB(&results))
				break;
			v=v->next;
		}
		/*check if first StRPSNode's domain length is valid*/
		if (results->len <= 0)
		{
			ErrPostEx(SEV_ERROR,0,0,"SHoundGetDomainsFromGi: Failed search");
			StRpsdbFree(results);  ValNodeFree(redundant_gilist);
			return NULL;
		}
		/*get the result from redundant group, so relabel the gi for the whole list to avoid
		           user's confusion*/
		else
		{
			RelabelGiForRpsList(gi, results);
		}

	}
	return results;
}





/*fetches information about domains from the Conserved Domain Database as assigned by Reverse Position
 *Specific BLast to a given protein sequence (gi). if this query gi is fail to get the result, try 
 *any other gis in the redundant group of the query gi, in this case the return domains are rebelled
 *their gi with the query gi. These domains are filtered out for which (len of align)/(len of domain) is
 *below a coverage_filter value.
 *params: gi is the given protein sequence
 *        coverage_filter is the cutoff for acceptable (len of align)/(len of domain) ratio
 */ 
StRpsdbPtr LIBCALL SHoundGetFilteredDomainsFromGi(Int4 gi, FloatHi coverage_filter)
{
	StRpsdbPtr results=NULL;
	ValNodePtr redundant_gilist=NULL, v=NULL;

	if(!SHoundModule("rpsdb") && !glb_is_rpsdb_open)
	{
		ErrPostEx(SEV_ERROR,0,0, "SHoundGetFilteredDomainsFromGi: Missing required SeqHound module.");
		return NULL;
	}

	if(gi<=0 || coverage_filter < 0)
	{
		ErrPostEx(SEV_ERROR,0,0, "SHoundGetFilteredDomainsFromGi: parameter is invalid");
		return NULL;
	}
	results=StRpsdbNew();
	results->gi=gi;
	/*Set evalue to default */
	results->evalue=LOG_DEFAULT_EVALUE;
	/* if the query gi does not get the search result, then use redundant group to search.*/
	if(SearchRPSDB(&results)!= 1)
	{
	        redundant_gilist = SHoundRedundantGroup(gi);
		v = redundant_gilist;
		while(v!=NULL)
		{
			results->gi = v->data.intvalue;
			if (SearchRPSDB(&results))
				break;
			v=v->next;
		}
		/*check if first StRPSNode's domain length is valid*/
		if (results->len <= 0)
		{
			ErrPostEx(SEV_ERROR,0,0,"SHoundGetFilteredDomainsFromGi: Failed search");
			StRpsdbFree(results);  ValNodeFree(redundant_gilist);
			return NULL;
		}
		/*get the result from redundant group, so relabel the gi for the whole list to avoid
		           user's confusion*/
		else
		{
			RelabelGiForRpsList(gi, results);
		}
	}
	/*Remove low coverage domains - i.e. C&N-terminus are longer than coverage*/
	if (coverage_filter > 0){
		if(!RemoveLowCoverageDomains(&results, coverage_filter)){
			ErrPostEx(SEV_ERROR,0,0,"[SHoundGetFilteredDomainsFromGi]: Failed to remove domains");
		}
	}
	ValNodeFree(redundant_gilist);
	return results;
}


/*Search for proteins with domain ID */
ValNodePtr LIBCALL SHoundGetGisByDomainId(CharPtr domain_id)
{
	StRpsdbPtr rpsdbp=NULL;
	StRpsdbPtr head=NULL;
	Int4 gi=0;
	ValNodePtr results=NULL;

	if(!SHoundModule("rpsdb") && !glb_is_rpsdb_open)
	{
		ErrPostEx(SEV_ERROR,0,0, "SHoundGetGisByDomainId: Missing required SeqHound module.");
		return FALSE;
	}

	if(domain_id==NULL)
	{
		ErrPostEx(SEV_ERROR,0,0,"SHoundGetGiByDomainId: Domain id is NULL");
		return NULL;
	}
	rpsdbp=StRpsdbNew();
	rpsdbp->dom_id=domain_id;
	/*set default evalue */
	rpsdbp->evalue=LOG_DEFAULT_EVALUE;
	if(SearchRPSDB(&rpsdbp)!= 1)
	{
		ErrPostEx(SEV_ERROR,0,0,"SHoundGetGiByDomainId: Failed search");
		StRpsdbFree(rpsdbp);
		return NULL;
	}
	head=rpsdbp;

	/*from the returned list select the unique gi list
	* Note that some domains appear more than once in a protein */
	while(head!=NULL)
	{
		if(gi!=head->gi){
			gi=head->gi;
			Slri_ValNodeAddInt(&results, 1,gi);
		}
		head=head->next;
	}
	StRpsdbFree(rpsdbp);
	return results;
}

/*Search for proteins with domain-id below cutoff evalue score */
ValNodePtr LIBCALL SHoundGetGisByDomainIdAndEvalue(CharPtr domain_id, FloatHi cutoff)
{
	StRpsdbPtr rpsdbp=NULL;
	StRpsdbPtr head=NULL;
	Int4 gi=0;
	ValNodePtr results=NULL;
	FloatHi log_cutoff=0.0;

	if(!SHoundModule("rpsdb") && !glb_is_rpsdb_open)
	{
		ErrPostEx(SEV_ERROR,0,0, "SHoundGetGisByDomainIdAndEvalue: Missing required SeqHound module.");
		return FALSE;
	}
	if(domain_id==NULL)
	{
		ErrPostEx(SEV_ERROR,0,0,"SHoundGetGiByDomainId: Domain id is NULL");
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

	/*if(SearchRPSDBByTagsAndEvalue(&rpsdbp)!= 1)*/
	if(SearchRPSDB(&rpsdbp)!= 1)
	{
		ErrPostEx(SEV_ERROR,0,0,"SHoundGetGiByDomainIdAndEvalue: Failed search");
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
			Slri_ValNodeAddInt(&results, 1,gi);
		}
		head=head->next;
	}
	StRpsdbFree(rpsdbp);
	return results;
}

/* Get the list of domains for a protein with score <cutoff> or lower */
StRpsdbPtr LIBCALL SHoundGetFilteredDomainsFromGiWithEvalue(Int4 gi, FloatHi cutoff, FloatHi coverage_filter )
{
	StRpsdbPtr results=NULL;
	ValNodePtr redundant_gilist=NULL, v=NULL;
	FloatHi log_cutoff=0.0;

	if(!SHoundModule("rpsdb") && !glb_is_rpsdb_open)
	{
		ErrPostEx(SEV_ERROR,0,0, "SHoundGetFilteredDomainsFromGiWithEvalue: Missing required SeqHound module.");
		return FALSE;
	}
	if(gi<=0 || coverage_filter < 0 )
	{
		ErrPostEx(SEV_ERROR,0,0,"SHoundGetFilteredDomainsFromGiWithEvalue: invalid parameters");
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

	if(SearchRPSDB(&results) != 1)
	{
		redundant_gilist = SHoundRedundantGroup(gi);
		v = redundant_gilist;
		while(v!=NULL)
		{
			results->gi = v->data.intvalue;
			if (SearchRPSDB(&results))
				break;
			v=v->next;
		}
		/*check if first stRPSNodes's domain length is valid*/
		if (results->len <= 0)
		{
			ErrPostEx(SEV_ERROR,0,0,"SHoundGetFilteredDomainsFromGiWithEvalue: failed search");
			StRpsdbFree(results); ValNodeFree(redundant_gilist);
			return NULL;
		}
		else 
		{
			RelabelGiForRpsList(gi, results);
		}
	}
	if (coverage_filter >0 )
	{
		/*remove low coverage domains */
		if(!RemoveLowCoverageDomains(&results, COVERAGE_CUTOFF)){
			ErrPostEx(SEV_ERROR,0,0,"[SHoundGetFilteredDomainsFromGiWithEvalue]: Failed to remove domains");
		}
	}
	return results;
}

/* Get the list of domains for a protein with score <cutoff> or lower */
StRpsdbPtr LIBCALL SHoundGetDomainsFromGiWithEvalue(Int4 gi, FloatHi cutoff)
{
	StRpsdbPtr results=NULL;
	FloatHi log_cutoff=0.0;
	ValNodePtr redundant_gilist=NULL, v=NULL;


	if(!SHoundModule("rpsdb") && !glb_is_rpsdb_open)
	{
		ErrPostEx(SEV_ERROR,0,0, "SHoundGetDomainsFromGiWithEvalue: Missing required SeqHound module.");
		return FALSE;
	}
	if(gi<=0)
	{
		ErrPostEx(SEV_ERROR,0,0,"SHoundGetGisByDomainId: invalid parameters");
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

	if(SearchRPSDB(&results)!=1)
	{
		redundant_gilist = SHoundRedundantGroup(gi);
		v = redundant_gilist;
		while(v!=NULL)
		{
			results->gi = v->data.intvalue;
			if (SearchRPSDB(&results))
				break;
			v=v->next;
		}
		/*check if first stRPSNodes's domain length is valid*/
		if (results->len <= 0)
		{
			ErrPostEx(SEV_ERROR,0,0,"SHoundGetDomainsFromGiWithEvalue: failed");
			StRpsdbFree(results); ValNodeFree(redundant_gilist);

			return NULL;
		}
		else
		{
			RelabelGiForRpsList(gi, results);
		}
	}


	return results;
}


/* Get the list of domains from a list of proteins*/
StRpsdbPtr LIBCALL SHoundGetDomainsFromGiListWithEvalue(ValNodePtr pvnGis, FloatHi cutoff)
{
	StRpsdbPtr head=NULL, current=NULL, temp=NULL;
	Int4 count=0;
	FloatHi log_cutoff=0.0;

	if(!SHoundModule("rpsdb") && !glb_is_rpsdb_open)
	{
		ErrPostEx(SEV_ERROR,0,0, "SHoundGetDomainsFromGiList: Missing required SeqHound module.");
		return NULL;
	}

	if(pvnGis==NULL){
		ErrPostEx(SEV_ERROR,0,0,"SHoundGetDomainsFromGiList: gi is invalid");
		return NULL;
	}
	/*convert cutoff to log*/
	if(cutoff==0.0){
		log_cutoff=-300.0;
	} else {
		log_cutoff=LOG10(cutoff);
	}

	while(pvnGis!=NULL){
		current=StRpsdbNew();
		current->gi=pvnGis->data.intvalue;
		/*Set evalue to default */
		current->evalue=log_cutoff;
		if(SearchRPSDB(&current)==-1){
			ErrPostEx(SEV_WARNING,0,0,"SHoundGetDomainsFromGiList: Failed search for %ld",(long)current->gi);
			StRpsdbFree(current);
			pvnGis=pvnGis->next;
			continue;
		}
		/*Keep the first node*/
		if(count==0)
			head=current;
		/*Add current domains to global list of domains*/
		else{
			temp=head;
			while(temp->next!=NULL)
				temp=temp->next;
			temp->next=current;
		}
		count++;
		pvnGis=pvnGis->next;
	}
	return head;
}

/*Get all Gi's that have <numdom> or more domains */
ValNodePtr LIBCALL SHoundGetGisByNumberOfDomains(Int4 numdom)
{
	StRpsdbPtr rpsdbp=NULL;
	StRpsdbPtr head=NULL;
	ValNodePtr results=NULL;
	Int4 gi=0;

	if(!SHoundModule("rpsdb") && !glb_is_rpsdb_open)
	{
		ErrPostEx(SEV_ERROR,0,0, "SHoundGetGisByNumberOfDomains: Missing required SeqHound module.");
		return FALSE;
	}

	if(numdom<=0)
	{
		ErrPostEx(SEV_ERROR,0,0,"SHoundGetDomainsFromGi: Domain id is NULL");
		return NULL;
	}
	rpsdbp=StRpsdbNew();
	rpsdbp->num_of_dom=numdom;
	if(SearchRPSDB(&rpsdbp)!= 1)
	{
		ErrPostEx(SEV_ERROR,0,0,"SHoundGetDomainsFromGi: Failed search");
		StRpsdbFree(rpsdbp);
		return NULL;
	}
	head=rpsdbp;

	/*from the returned list select the gis*/
	while(head!=NULL)
	{
		if(gi!=head->gi){
			gi=head->gi;
			Slri_ValNodeAddInt(&results, 1,gi);
		}
		head=head->next;
	}
        StRpsdbFree(rpsdbp);
	Misc_IntVnpUnique(&results);
	return results;
}

/*Remove domains with low coverage. Where
* (len of align)/(len of domain)< threshold value */
Boolean RemoveLowCoverageDomains(StRpsdbPtr PNTR results, FloatHi threshold)
{
	StRpsdbPtr current=NULL, head=NULL, prev=NULL, temp=NULL;

	FloatHi ratio=0.0;
	Boolean first=TRUE;
	if(results==NULL)
	{
		ErrPostEx(SEV_ERROR,0,0,"[RemoveLowCoverageDomains] Domain list is empty");
		return TRUE;
	}
	/*Set them all to the head*/
	head=*results;
	current=head;
	prev=head;
	first=TRUE;
	while(current!=NULL){
		ratio=(FloatHi)((FloatHi)current->len/((FloatHi)current->n_missing+(FloatHi)current->len+(FloatHi)current->c_missing));
		if(ratio<threshold){
			/*remove this domain*/
			if(first){
				current=head->next;
				prev=head->next;
				temp=head;
				head=head->next;
				temp->next=NULL;
				temp=StRpsdbFree(temp);
			}else{
				prev->next=current->next;
				temp=current;
				current=current->next;
				temp->next=NULL;
				temp=StRpsdbFree(temp);
			}
		} else{
			first=FALSE;
			prev=current;
			current=current->next;
		}
	}
	*results=head;
	return TRUE;
}



/************************
* DOMNAME Searches
************************/

/*Get Short label from Domain accession name (i.e. Pfam or Smart id) */
CharPtr LIBCALL SHoundGetDomainLabelFromDomainId(CharPtr accession)
{
	StDomNamedbPtr dmnp=NULL;
	CharPtr name=NULL;

	if(!SHoundModule("rpsdb") && !glb_is_domname_open)
	{
		ErrPostEx(SEV_ERROR,0,0, "SHoundGetDomainLabelFromDomainId: Missing required SeqHound module.");
		return FALSE;
	}

	if(accession==NULL)
	{
		ErrPostEx(SEV_ERROR,0,0,"SHoundGetDomainLabelFromDomainId: null parameter");
		return NULL;
	}
	dmnp=StDomNamedbNew();
	dmnp->accession=accession;
	if(SearchDOMNAME(&dmnp, FALSE)!=1)
	{
		ErrPostEx(SEV_ERROR,0,0,"SHoundGetDomainLabelFromDomainId: SearchDOMNAME failed");
		return NULL;
	}
	name=StringSave(dmnp->name);
	StDomNamedbFree(dmnp);
	return name;
}

/*Get the Domain accession (i.e. Pfam or smart id) from short name
* Note that it returns a ValNode since a domain can appear in both
* Pfam and Smart */
ValNodePtr LIBCALL SHoundGetDomainIdFromLabel(CharPtr label)
{
	StDomNamedbPtr dmnp=NULL;
	StDomNamedbPtr head=NULL;
	ValNodePtr results=NULL;
	CharPtr accession=NULL;

	if(!SHoundModule("rpsdb") && !glb_is_domname_open)
	{
		ErrPostEx(SEV_ERROR,0,0, "SHoundGetDomainIdFromLabel: Missing required SeqHound module.");
		return FALSE;
	}

	if(label==NULL)
	{
		ErrPostEx(SEV_ERROR,0,0,"SHoundGetDomainIdFromLabel: null parameter");
		return NULL;
	}
	dmnp=StDomNamedbNew();
	dmnp->name=label;
	if(SearchDOMNAME(&dmnp,FALSE)!=1)
	{
		ErrPostEx(SEV_ERROR,0,0,"SHoundGetDomainIdFromLabel: SearchDOMNAME failed");
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

/*Get PDB-ID 3D structure representative of the domain from domain ID*/
CharPtr LIBCALL SHoundGetDomain3DStructure (CharPtr domainId)
{
	StDomNamedbPtr dmnp=NULL;
	/* StDomNamedbPtr head=NULL; */
	CharPtr name=NULL;

	if(!SHoundModule("rpsdb") && !glb_is_domname_open)
	{
		ErrPostEx(SEV_ERROR,0,0, "SHoundGetDomain3DStructure: Missing required SeqHound module.");
		return FALSE;
	}
	if(domainId==NULL)
	{
		ErrPostEx(SEV_ERROR,0,0,"SHoundGetDomain3DStructure: null parameter");
		return NULL;
	}
	dmnp=StDomNamedbNew();
	dmnp->accession=domainId;
	if(SearchDOMNAME(&dmnp, FALSE)!=1)
	{
		ErrPostEx(SEV_ERROR,0,0,"SHoundGetDomain3DStructure: SearchDOMNAME failed");
		return NULL;
	}
	name=StringSave(dmnp->pdb_id);
	StDomNamedbFree(dmnp);
	return name;
}

/***********************
*Relational Searches
***********************/
/*Get a list of Gi's that contain domain <label>*/
ValNodePtr SHoundGetGisByDomainLabel(CharPtr label)
{
	ValNodePtr domain_ids=NULL;
	ValNodePtr domid_gis=NULL;
	ValNodePtr gis=NULL;
	ValNodePtr head=NULL;

	if(!SHoundModule("rpsdb") && !(glb_is_domname_open && glb_is_rpsdb_open))
	{
		ErrPostEx(SEV_ERROR,0,0, "SHoundGetGisByDomainLabel: Missing required SeqHound module.");
		return FALSE;
	}
	if(label==NULL)
	{
		ErrPostEx(SEV_ERROR,0,0,"SHoundGetGisByDomainLabel:null parameter");
		return NULL;
	}
	/*gat all Domain-id from this short-label */
	domain_ids=SHoundGetDomainIdFromLabel(label);
	if(domain_ids==NULL)
	{
		ErrPostEx(SEV_ERROR,0,0,"SHoundGetGisByDomainLabel: could not find %s in DOMNAME", IsNullStr(label));
		return NULL;
	}
	else{
		/*collect all Gi's from each Domain-ID*/
		head=domain_ids;
		while(domain_ids!=NULL){
			domid_gis=SHoundGetGisByDomainId((char*)domain_ids->data.ptrvalue);
			ValNodeLink(&gis, domid_gis);
			domain_ids=domain_ids->next;
		}
		ValNodeFree(head);
	}
	return gis;
}

/*Get a list of Gi's that contain domain <label> with
* Evalue of <cutoff> or lower*/
ValNodePtr SHoundGetGisByDomainLabelAndEvalue(CharPtr label, FloatHi cutoff)
{
	ValNodePtr domain_ids=NULL;
	ValNodePtr domid_gis=NULL;
	ValNodePtr gis=NULL;
	ValNodePtr head=NULL;

	if(!SHoundModule("rpsdb") && !(glb_is_domname_open && glb_is_rpsdb_open))
	{
		ErrPostEx(SEV_ERROR,0,0, "SHoundGetGisByDomainLabelAndEvalue: Missing required SeqHound module.");
		return FALSE;
	}
	if(label==NULL)
	{
		ErrPostEx(SEV_ERROR,0,0,"SHoundGetGisByDomainLabel&Eval:null parameter");
		return NULL;
	}
	/*gat all Domain-id from this short-label */
	domain_ids=SHoundGetDomainIdFromLabel(label);
	if(domain_ids==NULL)
	{
		ErrPostEx(SEV_ERROR,0,0,"SHoundGetGisByDomainLabel&Eval: could not find %s in DOMNAME", IsNullStr(label));
		return NULL;
	}
	else{
		/*collect all Gi's from each Domain-ID*/
		head=domain_ids;
		while(domain_ids!=NULL){
			domid_gis=SHoundGetGisByDomainIdAndEvalue((char*)domain_ids->data.ptrvalue,cutoff);
			ValNodeLink(&gis, domid_gis);
			domain_ids=domain_ids->next;
		}
		ValNodeFree(head);
	}
	return gis;
}
/*
$Log: rpsdbapi.c,v $
Revision 1.9  2004/06/16 20:23:15  mmatan
Eliminated filtering of low coverage domains in functions SHoundGetDomainsFromGI
and SHoundGetDomainsFromGIWithEvalue, as a fix for bug 1847.

Revision 1.8  2004/05/31 20:36:17  hlieu
minor compiler warning fixes for bug 2185

Revision 1.7  2004/04/19 16:38:57  mmatan
Reinstated the filtering in SHoundGetDomainsFromGiWithEvalue and
SHoundGetDomainsFromGi, which I mistakenly removed in the last commit.  See bug
1847 comment 10 for details.

Revision 1.6  2004/04/19 15:16:36  mmatan
Added redundant GI checking functionality to SHoundGetDomainsFromGi and
SHoundGetDomainsFromGiWithEvalue functions, so that they check for rpsdb entries
for other GIs in the query GIs redund group if none exist for query GI.  This
addresses bug 1847.

Revision 1.5  2004/04/16 15:50:48  ryao
minor change in function SHoundGetFilteredDomainsFromGiWithEvalue

Revision 1.4  2004/04/16 15:27:05  mmatan
Defined function SHoundGetFilteredDomainsFromGiWithEvalue, which takes a gi,
evalue cutoff and a coverage parameter, and returns those rpsdb hits for that gi
which are below the evalue cutoff and have coverage greater than or equal to the
coverage value.  See bug 1847.

Revision 1.3  2004/04/16 14:23:39  ryao
Added SHoundGetFilteredDomainsFromGi RelabelGiForRpsList. bug [1847] related.

Revision 1.2  2004/01/06 22:32:17  skuznets
Code audit has been done sucessfully.
Possible NULL pointer issues has been eliminated.

Revision 1.1.1.18  2002/11/16 09:33:48  root
Automated import from Sourceforge.net

Revision 1.25  2002/11/15 20:09:22  betel
Fixed memory bug

Revision 1.24  2002/10/28 22:01:47  betel
changed coverage threshold

Revision 1.23  2002/10/28 21:48:58  betel
removed printf

Revision 1.21  2002/10/28 16:53:22  betel
Low coverage domains are removed from GetDomainsFromGI calls

Revision 1.20  2002/10/25 00:05:08  betel
Changed field in DOMNAME table and added 3D strcture function for domains

Revision 1.19  2002/09/30 18:56:13  gbader
Fixed error in error message.

Revision 1.18  2002/09/30 17:36:09  kaca
fixed include name for RPS BLAST database layer

Revision 1.17  2002/08/22 18:35:53  betel
Changed SearchDOMANE to search without returning asn1

Revision 1.16  2002/08/20 21:42:58  micheld
Compiler warning/error fix - changed return and added LIBCALL to declared function

Revision 1.15  2002/08/07 21:16:47  kaca
added slri_misc.h include

Revision 1.14  2002/08/02 22:37:22  kaca
replaced all ValNodeAddInt with optimized Slri_ValNodeAddInt

Revision 1.13  2002/08/02 22:10:26  kaca
added memory freeing of search results

Revision 1.12  2002/07/30 19:39:34  betel
Bug fix in GetDomainsFromGiListWithEvalue

Revision 1.11  2002/07/30 15:22:42  betel
Fixes for  DomainsFromGiList function

Revision 1.10  2002/07/29 22:30:46  betel
Added GetDomainsFromGiListWithEvalue for local&remote C api (no Perl or C++ yet)

Revision 1.9  2002/07/25 15:11:53  kaca
reviewed ValNode sorting upon return

Revision 1.8  2002/07/08 16:58:02  kaca
renamed two RPS-BLAST functions Accession is DomainId

Revision 1.7  2002/07/05 16:23:42  betel
Fixed compile warnings

Revision 1.6  2002/05/17 15:06:10  betel
Fixed Windows MSVC build

Revision 1.5  2002/04/15 16:16:42  betel
Removed redunadant function and fixed compile warnings

Revision 1.4  2002/04/11 20:25:36  betel
Minor changes

Revision 1.3  2002/04/04 17:43:55  kaca
changed module check logic

Revision 1.2  2002/04/02 23:08:48  kaca
changed all API calls to SHound* to accomodate general SHound naming scheme

Revision 1.1  2002/04/02 21:40:37  kaca
moved from ../rps" rpsdbapi.c rpsdb_cb.c

Revision 1.2  2002/03/18 21:57:27  betel
Added Domain Table searches and lookup

Revision 1.1  2002/03/15 22:26:52  betel
RPSDB API layer

*/
