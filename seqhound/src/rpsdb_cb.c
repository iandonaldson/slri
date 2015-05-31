/*$Id: rpsdb_cb.c,v 1.10 2004/06/07 22:01:10 hlieu Exp $*/
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


  PROGRAM: Rps Table (domains) codebase layer


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

#include <ncbi.h>
#include <intrez_misc.h>
#include <slri_cb.h>
#include <slri_math.h>
#include <rpsdb_db.h>
#include <d4all.h>   /* move to end to void _LARGE_FILE_SOURCE redefinition warnings */

extern CODE4 cb;
static DATA4
	*pd4rpsdb, /*RPS-BLAST domain database */
	*pd4domname; /*Domain name database */
static FIELD4
	/*RPSdb*/
	*pf4rpsgi,
	*pf4rpscddid,
	*pf4rpsdomid,
	*pf4rpsfrom,
	*pf4rpsalignlen,
	*pf4rpsscore,
	*pf4rpsevalue,
	*pf4rpsbitscore,
	*pf4rpsmisN,
	*pf4rpsmisC,
	*pf4rpsnumdom,
	/* DOMNAME db*/
	*pf4dnaccession,
	*pf4dnname,
	*pf4dnpdbid,
	*pf4dnasn1;

static TAG4
	/*RPSdb*/
	*tagrpsgi,
	*tagrpscddid,
	*tagrpsdomid,
	*tagrpslen,
	*tagrpsnumdom,
	/* Domname*/
	*tagdnaccession,
	*tagdnname,
	*tagdnpdbid;

/* CodeBase FIELD4INFO  and TAG4INFO structures */

/*New rpsdb tables added CDDid, score and bitscore*/
static FIELD4INFO f4rpsdb[ ]=
{
    /*name, type, len, dec*/
    {"GI",r4num, 10, 0 },
    {"CDDID",r4num,10,0},
    {"DOMID",r4str, 12, 0 },
    {"FROM", r4num, 6, 0 },
    {"ALIGN_LEN", r4num, 6, 0},
    {"SCORE", r4num, 10, 0 },
    {"EVALUE", r4float, 15, 8},
    {"BITSCORE", r4float, 15, 8},
    {"MISSING_N", r4num, 6, 0},
    {"MISSING_C", r4num, 6, 0},
    {"NUMDOM", r4num, 4, 0},
    {0, 0, 0, 0}
};

static TAG4INFO t4rpsdb[ ]=
{
    /*name, expression, filter, unique, descending*/
    {"GI", "GI", 0, 0, 0},
    {"CDDID", "CDDID", 0, 0},
    {"DOMID", "DOMID", 0, 0},
    {"ALIGN_LEN", "ALIGN_LEN", 0, 0},
    {"NUMDOM", "NUMDOM", 0 ,0},
    {0, 0, 0, 0, 0}
};

/*  Domain Name */
static FIELD4INFO f4domname[ ]=
{
	/* name, type, len, dec*/
	{"ACCESSION", r4str, 15,0},
	{"NAME", r4str, 25,0},
	{"PDBID", r4str, 12, 0},
	{ "ASN1", 'M', 10, 0 },
	{0, 0, 0, 0}
};

static TAG4INFO t4domname[ ]=
{
	/* name, expression, filter, unique, descending*/
	{"ACCESSION", "ACCESSION", 0 ,0 ,0},
	{"NAME", "NAME", 0, 0, 0},
	{"PDBID", "PDBID", 0,0,0},
	{0, 0, 0, 0, 0}
};

/*Append to RPS Database*/
Boolean LIBCALL AppendRecordRPSDB(StRpsdbPtr record)
{
	d4appendStart(pd4rpsdb, 0);
	d4blank(pd4rpsdb);
	if(record == NULL)
	{
		ErrPostEx(SEV_ERROR, 0, 0, "AppendRecordRPSDB: missing information.");
		return FALSE;
	}
	/*convert the evalue to the log
	* codebase cannot deal with double in sci. notation */
	SLRI_SafeLog10(&(record->evalue));
	SLRI_SafeLog10(&(record->bitscore));

	f4assignLong(pf4rpsgi, (long)record->gi);
	/*new rps fields*/
	f4assignLong(pf4rpscddid, (long)record->cdd_id);
	f4assignLong(pf4rpsscore, (long)record->score);
	f4assignDouble(pf4rpsbitscore, (double)record->bitscore);

	f4memoAssign(pf4rpsdomid, record->dom_id );
	f4assignLong(pf4rpsfrom, (long)record->from);
	f4assignLong(pf4rpsmisN, (long)record->n_missing);
	f4assignLong(pf4rpsmisC, (long)record->c_missing);
	f4assignLong(pf4rpsalignlen, (long)record->len);
	f4assignDouble(pf4rpsevalue, (double)record->evalue);
	f4assignLong(pf4rpsnumdom, (long)record->num_of_dom);

	d4append(pd4rpsdb);
	return TRUE;
}

/* Append to DOMNAME table */
Boolean LIBCALL AppendRecordDOMNAME(StDomNamedbPtr dnp)
{
	if( dnp == NULL)
	{
		ErrPostEx(SEV_ERROR, 0, 0, " AppendRecordDOMNAME: NULL parameter.");
		return FALSE;
	}
	d4appendStart(pd4domname, 0);
	d4blank(pd4domname);

	if( dnp->asn1 !=NULL)
	{
		if(!AssignASNMemMemo((Pointer) dnp->asn1, (AsnWriteFunc) CddAsnWrite, pf4dnasn1))
		{
			ErrPostEx(SEV_ERROR,0,0,"AppendRecordDOMNAME: Call to AssignASNMemMemo failed.");
			return FALSE;
		}
	}

	printf("Domain: %s ,%s, %s\n",IsNullStr(dnp->name),IsNullStr(dnp->accession),IsNullStr(dnp->pdb_id));

	f4memoAssign(pf4dnaccession, dnp->accession );
	f4memoAssign(pf4dnname, dnp->name );
	f4memoAssign(pf4dnpdbid, dnp->pdb_id);
	d4append(pd4domname);
	return TRUE;
}

/*Delete record from RPS table*/
Boolean LIBCALL DeleteGIinRPSDB(Int4 gi, Boolean destroy)
{
	int rc;
	Boolean delcd = FALSE;
	d4tagSelect(pd4rpsdb, tagrpsgi);
	d4top(pd4rpsdb);
	rc=d4seekDouble(pd4rpsdb, gi);
	
	while(rc==r4success)
	{
		if(destroy)
		{
		  d4blank(pd4rpsdb);
		  f4assignLong(pf4rpsgi, (long)-1);
		}
		d4delete(pd4rpsdb);
		delcd= TRUE;

		/* find the next instance of this GI in rpsdb */
		rc=d4seekDouble(pd4rpsdb, gi);		

	}
	return delcd;
}

/*Delete record from DomName table*/
Boolean LIBCALL DeleteAccinDOMNAME(CharPtr accession, Boolean destroy)
{
	int rc;
	Boolean delcd = FALSE;
	d4tagSelect(pd4domname, tagdnaccession);
	d4top(pd4domname);
	rc=d4seek(pd4domname, accession);
	if(rc==r4success)
	{
		if(destroy)
		{
			d4blank(pd4domname);
			f4memoAssign(pf4dnaccession, "n/a");
		}
		d4delete(pd4domname);
		delcd= TRUE;
	}
	return delcd;
}

/*Check if GI is in RPSDB */
Boolean LIBCALL IsExistInRPSDB(Int4 gi)
{
	int rc;
	d4tagSelect(pd4rpsdb, tagrpsgi);
	d4top(pd4rpsdb);
	rc=d4seekDouble(pd4rpsdb, gi);

	return ((rc==r4success)? TRUE: FALSE);

}

/*Check if domain accession exist in DOMNAME */
Boolean LIBCALL IsExistInDOMNAME(CharPtr accession)
{
	int rc;
	d4tagSelect(pd4domname, tagdnaccession);
	d4top(pd4domname);
	rc=d4seek(pd4domname, accession);

	return ((rc==r4success)?TRUE:FALSE);
}

/*Packing and memory compression*/
void LIBCALL PackRPSDB(void)
{

  /* if ( pd4rpsdb->dataFile->recordLockWriteCount > 1) pd4rpsdb->dataFile->recordLockWriteCount = 0; */
  d4pack(pd4rpsdb);
  d4memoCompress(pd4rpsdb);
}

void LIBCALL PackDOMNAME(void)
{
  d4pack(pd4domname);
  d4memoCompress(pd4domname);
}

/****************************
* Search Functions
*****************************/

/*Return record from RPSDb
* searches can be done using any of the
* tags defined for this table. Returns all
* matching records. Note that Evalue must be set to some value*/
Int2 LIBCALL SearchRPSDB(StRpsdbPtr PNTR pprs)
{
	int rc;
	CharPtr stringkey=NULL;
	int length = 0;
	Int4 key=0;
	FloatHi evalue=0.0;
	StRpsdbPtr rpsdbp;
	StRpsdbPtr prev=NULL;

	/* SK: NULL ptr check */
	if ( pprs == NULL || *pprs == NULL )
	{
	    ErrPostEx(SEV_ERROR, 0, 0,"SearchRPSDB: NULL pprs");
	    return 0;
	}

	rpsdbp=*pprs;
	/*check if any of the tags are set*/
	if( ( ((rpsdbp)->gi<=0) && ((rpsdbp)->dom_id==NULL) &&
		((rpsdbp)->num_of_dom==0) && ((rpsdbp)->len==0) && ((rpsdbp)->cdd_id==0) ) ){
		ErrPostEx(SEV_ERROR, 0, 0,"SearchRPSDB: no valid search key");
		return 0;
	}
	evalue=rpsdbp->evalue;

	/*searches based on gi,number of domains, or length or cddid*/
	if((rpsdbp)->gi>0 ||(rpsdbp)->num_of_dom>0 || (rpsdbp)->len>0 || (rpsdbp)->cdd_id>0 )
	{
		if((rpsdbp)->gi>0){
			key=(rpsdbp)->gi;
			d4tagSelect(pd4rpsdb, tagrpsgi);
		}
		if((rpsdbp)->num_of_dom>0){
			key=(rpsdbp)->num_of_dom;
			d4tagSelect(pd4rpsdb, tagrpsnumdom);
		}
		if((rpsdbp)->len>0){
			key=(rpsdbp)->len;
			d4tagSelect(pd4rpsdb, tagrpslen);
		}
		if((rpsdbp)->cdd_id>0){
			key=(rpsdbp)->cdd_id;
			d4tagSelect(pd4rpsdb, tagrpscddid);
		}
		d4top(pd4rpsdb);
		rc = d4seekDouble(pd4rpsdb, key);
		if(rc!=r4success)
			return -1;
		for(rc=d4seekDouble(pd4rpsdb, key); rc==r4success; rc=d4seekNextDouble(pd4rpsdb,key))
		{
			/*get domain id lower or equel to evalue */
			(rpsdbp)->evalue=f4double(pf4rpsevalue);
			if((rpsdbp)->evalue<=evalue || (rpsdbp)->evalue==0){
				(rpsdbp)->gi=(Int4) f4double(pf4rpsgi);
				length=strlen(f4str(pf4rpsdomid));
				(rpsdbp)->dom_id = TrimSpace(StrSave(f4str(pf4rpsdomid)));
				(rpsdbp)->from=(Int4) f4double(pf4rpsfrom);

				(rpsdbp)->n_missing=(Int4) f4double(pf4rpsmisN);
				(rpsdbp)->c_missing=(Int4) f4double(pf4rpsmisC);
				(rpsdbp)->len=(Int4) f4double(pf4rpsalignlen);
				(rpsdbp)->num_of_dom=(Int4) f4double(pf4rpsnumdom);
				(rpsdbp)->cdd_id=(Int4) f4double(pf4rpscddid);
				(rpsdbp)->score=(Int4) f4double(pf4rpsscore);
				(rpsdbp)->bitscore=f4double(pf4rpsbitscore);
				(rpsdbp)->next=(StRpsdbPtr)StRpsdbNew();
				prev=rpsdbp;
				rpsdbp=(rpsdbp)->next;
			}
		}
		/*last node is deleted*/
		/*if((*pprs)->num_of_dom==0)
			 *pprs=NULL; */ /*possible memory leak?*/
		if((*pprs)->num_of_dom!=0)
			rpsdbp=StRpsdbFree(rpsdbp);  /*only erase empty structure if some hits were found*/
		if(prev!=NULL)
			prev->next=NULL;
		return 1;
	}

	/* search based on domain_id*/
	if((rpsdbp)->dom_id!=NULL)
	{
		d4tagSelect(pd4rpsdb, tagrpsdomid);
		d4top(pd4rpsdb);
		stringkey=PadString((rpsdbp)->dom_id);
		rc = d4seekN(pd4rpsdb, stringkey, (size_t)strlen(stringkey));
		if(rc!=r4success)
			return -1;
		for(rc=d4seekN(pd4rpsdb, stringkey, (size_t)strlen(stringkey)); rc == r4success;
			rc = d4seekNextN(pd4rpsdb, stringkey, (size_t)strlen(stringkey)))
		{
			/*get domain id lower or equel to evalue */
			if(f4double(pf4rpsevalue)<=evalue)
			{
				(rpsdbp)->gi=(Int4) f4double(pf4rpsgi);
				length=strlen(f4str(pf4rpsdomid));
				(rpsdbp)->dom_id = TrimSpace(StrSave(f4str(pf4rpsdomid)));
				(rpsdbp)->from=(Int4) f4double(pf4rpsfrom);
				(rpsdbp)->evalue=f4double(pf4rpsevalue);
				(rpsdbp)->n_missing=(Int4) f4double(pf4rpsmisN);
				(rpsdbp)->c_missing=(Int4) f4double(pf4rpsmisC);
				(rpsdbp)->len=(Int4) f4double(pf4rpsalignlen);
				(rpsdbp)->num_of_dom=(Int4) f4double(pf4rpsnumdom);
				(rpsdbp)->cdd_id=(Int4) f4double(pf4rpscddid);
				(rpsdbp)->score=(Int4) f4double(pf4rpsscore);
				(rpsdbp)->bitscore=f4double(pf4rpsbitscore);
				(rpsdbp)->next=(StRpsdbPtr)StRpsdbNew();
				prev=rpsdbp;
				rpsdbp=(rpsdbp)->next;
			}
		}
		/*Free the last object in the list
		* In cases where the search is completed here with no
		* results then rpsdbp is pointing to the first (and empty)
		* element and we set *pprs to null */
		/*if((*pprs)->num_of_dom==0)
			*pprs=NULL; */ /*probably memory leak*/

		if((*pprs)->num_of_dom!=0)
			rpsdbp=StRpsdbFree(rpsdbp);/*only erase empty structure if some hits were found*/


		if(prev!=NULL)
			prev->next=NULL;
		MemFree(stringkey);
		return 1;
	}
	return -1;
}

/* This function is currently not in use!!
* Query times are too long to be practical.
*-----------------------------------------
* Search RPSDB using a combination of any
* two tags from RPSDB and an Evalue (gi, dom_id, len, numdom)
* Can search any combination of two tags with evalue or any single tag with evalue.
* Precondition- No more than two tags can be set. Evalue must be set even if just default value
* Note that evalues should be in log form.
*/
Int2 SearchRPSDBByTagsAndEvalue(StRpsdbPtr PNTR pprs)
{
	RELATE4 *relation=NULL;
	int rc, length=0;
	char buff[1000];
	char temp_holder[1000];
	StRpsdbPtr rpsdbp;
	StRpsdbPtr prev=NULL;

	/* SK: NULL ptr check */
	if ( pprs == NULL || *pprs == NULL )
	{
	    ErrPostEx(SEV_ERROR, 0,0,"SearchRPSDByTagsAndEval: NULL pprs");
	    return -1;
	}

	rpsdbp=*pprs;
	if((rpsdbp->gi<=0 &&
		rpsdbp->dom_id==NULL && rpsdbp->num_of_dom==0&&rpsdbp->len==0))
	{
		if(rpsdbp!=NULL)
			ErrPostEx(SEV_ERROR,0,0,"SearchRPSByTagsandEval: check StRpsdb values");
		else
			ErrPostEx(SEV_ERROR, 0,0,"SearchRPSDByTagsAndEval: empty StRpsdbPtr");
		return -1;
	}

	/*select the two tags to use */
	buff[0]='\0';
	temp_holder[0]='\0';

	/*This routine checks what tags are set and builds a query expression
	 * each expression ends with an EVALUE cutoff*/
	if(rpsdbp->gi>0)
	{
		/*key=rpsdbp->gi;
		d4tagSelect(pd4rpsdb, tagrpsgi);*/
		sprintf(buff, "GI = %d .AND. ",rpsdbp->gi);
		if(rpsdbp->dom_id!=NULL){
			sprintf(temp_holder,"DOMID ='%s' .AND. ", IsNullStr(rpsdbp->dom_id));
		}else if (rpsdbp->num_of_dom>0){
			sprintf(temp_holder,"NUMDOM =%d .AND. ", rpsdbp->num_of_dom);
		}else if (rpsdbp->len>0){
			sprintf(temp_holder,"ALIGN_LEN =%d .AND. ", rpsdbp->len);
		}
		strcat(buff, temp_holder);
	}
	else if(rpsdbp->dom_id!=NULL)
	{
		/*stringkey=PadString(rpsdbp->dom_id);
		d4tagSelect(pd4rpsdb, tagrpsdomid);*/

		sprintf(buff, "DOMID ='%s' .AND. ", IsNullStr(rpsdbp->dom_id));
		if (rpsdbp->num_of_dom>0){
			sprintf(temp_holder,"NUMDOM =%d .AND. ", rpsdbp->num_of_dom);
		}else if (rpsdbp->len>0){
			sprintf(temp_holder,"ALIGN_LEN =%d .AND. ", rpsdbp->len);
		}
		strcat(buff, temp_holder);
	}
	else if(rpsdbp->num_of_dom>0){
		/*key=rpsdbp->num_of_dom;
		d4tagSelect(pd4rpsdb, tagrpsnumdom);*/

		sprintf(buff, "NUMDOM =%d .AND. ", rpsdbp->num_of_dom);
		if(rpsdbp->len>0){
			sprintf(temp_holder,"ALIGN_LEN =%d .AND. ", rpsdbp->len);
		}
		strcat(buff,temp_holder);
	}

	temp_holder[0]='\0';
	sprintf(buff, "EVALUE<%ld",(long)rpsdbp->evalue);
	strcat(buff,temp_holder);
	relation=relate4init(pd4rpsdb);

	if(relation==NULL){
		ErrPostEx(SEV_ERROR, 0,0,"Search2TagRPSDB: cannot create relation");
		return -1;
	}

	relate4querySet(relation,buff);
	for(rc=relate4top(relation); rc==r4success; rc=relate4skip(relation, 1L))
	/*for(rc=d4seekDouble(pd4rpsdb,key); rc==r4success; rc=d4seekNextDouble(pd4rpsdb, key))*/
	/*rc=relate4top(relation);*/
	/*while(rc==r4success)*/
	{
		(rpsdbp)->gi=(Int4) f4double(pf4rpsgi);
		length=strlen(f4str(pf4rpsdomid));
		(rpsdbp)->dom_id = TrimSpace(StrSave(f4str(pf4rpsdomid)));
		(rpsdbp)->from=(Int4) f4double(pf4rpsfrom);
		(rpsdbp)->evalue=f4double(pf4rpsevalue);
		(rpsdbp)->n_missing=(Int4) f4double(pf4rpsmisN);
		(rpsdbp)->c_missing=(Int4) f4double(pf4rpsmisC);
		(rpsdbp)->len=(Int4) f4double(pf4rpsalignlen);
		(rpsdbp)->num_of_dom=(Int4) f4double(pf4rpsnumdom);
		(rpsdbp)->next=(StRpsdbPtr)StRpsdbNew();
		prev=rpsdbp;
		rpsdbp=(rpsdbp)->next;
	}
	rpsdbp=StRpsdbFree(rpsdbp);
	prev->next=NULL;
	code4unlock(&cb);
	relate4free(relation,0);
	return 1;
}


/*search DOMNAME table based on domain accession,
* domain name, or mmdb-id */
Int2 LIBCALL SearchDOMNAME(StDomNamedbPtr PNTR ppdn, Boolean noasn)
{
	int rc;
	CharPtr stringkey=NULL;
	Int4 key=0;
	StDomNamedbPtr domnamep;

	/* SK: NULL ptr check */
	if ( ppdn == NULL || *ppdn == NULL )
	{
		ErrPostEx(SEV_ERROR, 0, 0,"SearchDOMNAME: NULL ppdn");
		return 0;
	}

	domnamep=*ppdn;
	/*check if any of the tags are set*/
	if(((domnamep)->pdb_id==NULL) && ((domnamep)->accession==NULL) &&
			((domnamep)->name==NULL)){
		ErrPostEx(SEV_ERROR, 0, 0,"SearchDOMNAME: no valid search key");
		return 0;
	}

	/*searches based on mmdbid*/
	if((domnamep)->pdb_id!=NULL)
	{
		stringkey=PadString((domnamep)->pdb_id);
		d4tagSelect(pd4domname, tagdnpdbid);
		d4top(pd4domname);
		rc = d4seekDouble(pd4domname,key);
		if(rc!=r4success)
			return -1;
		for(rc=d4seekDouble(pd4domname, key); rc==r4success; rc=d4seekNextDouble(pd4domname,key))
		{
			/*get domain id*/
			(domnamep)->accession = TrimSpace(StrSave(f4str(pf4dnaccession)));
			(domnamep)->name = TrimSpace(StrSave(f4str(pf4dnname)));
			if(noasn!=FALSE){
				(domnamep)->asn1=GetCddFROMRDOMNAME();
			}else {
				(domnamep)->asn1=NULL;
			}
			(domnamep)->next=(StDomNameNodePtr)StDomNamedbNew();
			domnamep=(domnamep)->next;
		}
		StDomNamedbFree((domnamep)->next);
		(domnamep)->next=NULL;
		return 1;
	}

	/* search based on domain_id*/
	if((domnamep)->accession!=NULL || (domnamep)->name!=NULL)
	{
		if((domnamep)->accession!=NULL){
			stringkey=PadString((domnamep)->accession);
			/*stringkey=(domnamep)->accession;*/
			d4tagSelect(pd4domname, tagdnaccession);
		}
		if((domnamep)->name!=NULL){
			stringkey=PadString((domnamep)->name);
			/*stringkey=(domnamep)->name;*/
			d4tagSelect(pd4domname, tagdnname);
		}

		d4top(pd4domname);
		rc = d4seekN(pd4domname, stringkey, (size_t)strlen(stringkey));
		if(rc!=r4success)
			return -1;
		for(rc=d4seekN(pd4domname, stringkey, (size_t) strlen(stringkey)); rc == r4success;
			rc = d4seekNextN(pd4domname, stringkey, (size_t) strlen(stringkey)))
		{
			(domnamep)->pdb_id= TrimSpace(StrSave(f4str(pf4dnpdbid)));
			(domnamep)->accession = TrimSpace(StrSave(f4str(pf4dnaccession)));
			(domnamep)->name = TrimSpace(StrSave(f4str(pf4dnname)));
			if(noasn!=FALSE){
				(domnamep)->asn1=GetCddFROMRDOMNAME();
			}else {
				(domnamep)->asn1=NULL;
			}
			(domnamep)->next=(StDomNameNodePtr)StDomNamedbNew();
			domnamep=(domnamep)->next;
		}
		StDomNamedbFree((domnamep)->next);
		(domnamep)->next=NULL;
		MemFree(stringkey);
		return 1;
	}
	return -1;
}

/******************
* Reset functions
******************/

/*Reset RPSDB record */
void LIBCALL ResetStRPSDB (StRpsdbPtr PNTR pprs)
{
	/* SK: NULL ptr check */
	if(pprs == NULL || *pprs == NULL)
	{
		ErrPostEx(SEV_ERROR, 0,0,"ResetStRPSDB: structure is null");
		return;
	}
	(*pprs)->gi=0;
	(*pprs)->dom_id=NULL;
	(*pprs)->from=0;
	(*pprs)->evalue=0;
	(*pprs)->n_missing=0;
	(*pprs)->c_missing=0;
	(*pprs)->len=0;
	(*pprs)->num_of_dom=0;
	(*pprs)->cdd_id=0;
	(*pprs)->score=0;
	(*pprs)->bitscore=0.0;
	(*pprs)->next=NULL;
	return;
}

/*Reset DOMNAME record*/
void LIBCALL ResetStDOMNAME (StDomNamedbPtr PNTR ppdn)
{
	/* SK: NULL ptr check */
	if(ppdn == NULL || *ppdn == NULL)
	{
		ErrPostEx(SEV_ERROR, 0,0,"ResetStRPSDB: structure is null");
		return;
	}
	(*ppdn)->accession=NULL;
	(*ppdn)->name=NULL;
	(*ppdn)->pdb_id=NULL;
	(*ppdn)->asn1=NULL;
	(*ppdn)->next=NULL;
	return;
}
/*CodeBase function that are needed to build table*/
/***************************************
* Codebase open and close calls
* comment out when using local SeqHound
***************************************/

Boolean LIBCALL RPS_InitCodeBase(void)
{
	code4init(&cb);
	cb.safety = 1;
	error4exitTest(&cb);
	code4largeOn(&cb);
	cb.errDefaultUnique = r4unique;

	return TRUE;
}

Boolean LIBCALL RPS_CloseCodeBase(void)
{
	if ((code4close(&cb) == r4success) && (code4initUndo(&cb) == r4success))
		return TRUE;
	else
		return FALSE;
}

/*************************************
* Codebase open and close calls
**************************************/

/*Close RPS database*/

Boolean LIBCALL CloseRPSDB(void)
{
	if(d4close(pd4rpsdb) == r4success)
		return TRUE;
	else
		return FALSE;
}

/*Close DOMNAME database*/
Boolean LIBCALL CloseDOMNAME(void)
{
	if(d4close(pd4domname) == r4success)
		return TRUE;
	else
		return FALSE;
}

/*Open RPS database*/
Boolean LIBCALL OpenRPSDB( CharPtr name, Boolean create)
{
	Char pathdatab[PATH_MAX];
	CharPtr path=NULL;
	Boolean pathAllocated=FALSE;

	/*Get info from config file*/
	GetAppParam("intrez", "datab","pathrps", "", pathdatab, (size_t)PATH_MAX*(sizeof(char)));
	if((pathdatab!=NULL)&&(StringCmp(pathdatab, "")!=0)) {
		path = AllocateAndCreateDBPath(pathdatab, name);
		pathAllocated=TRUE;
	}
	else if(name!=NULL) {
		path=name;
	}
	else {
		ErrPostEx(SEV_ERROR,0,0,"OpenRPSDB: Can't find a path to open RPSDB database table in either argument list or intrez configuration file.");
		return(FALSE);
	}
	/* Open */
	if(create == FALSE)
	{
		pd4rpsdb = d4open(&cb, path);
		if(pd4rpsdb == NULL)
		{
		if(pathAllocated)MemFree(path);
		return FALSE;
		}
	}

	/* create */
	else {
		pd4rpsdb = d4create(&cb, path, f4rpsdb, t4rpsdb);
		if (cb.errorCode)
		{
		if(pathAllocated)MemFree(path);
		return FALSE;
		}
	}

	/* fields */
	pf4rpsgi = d4field(pd4rpsdb, "GI");
	pf4rpsdomid = d4field(pd4rpsdb,"DomId" );
	pf4rpsfrom = d4field(pd4rpsdb,"From" );
	pf4rpsmisN = d4field(pd4rpsdb, "Missing_N");
	pf4rpsmisC = d4field(pd4rpsdb, "Missing_C");
	pf4rpsalignlen = d4field(pd4rpsdb, "Align_Len");
	pf4rpsevalue = d4field(pd4rpsdb,"Evalue" );
	pf4rpsnumdom = d4field(pd4rpsdb,"NumDom" );
	pf4rpscddid = d4field(pd4rpsdb, "CddId");
	pf4rpsscore = d4field(pd4rpsdb, "Score");
	pf4rpsbitscore= d4field(pd4rpsdb, "BitScore");
	/* tags */
	tagrpsgi = d4tag (pd4rpsdb, "GI");
	tagrpsdomid = d4tag (pd4rpsdb,"DomId");
	tagrpsnumdom = d4tag (pd4rpsdb,"NumDom");
	tagrpslen = d4tag (pd4rpsdb, "Align_len");
	tagrpscddid = d4tag (pd4rpsdb, "CddId");
	if ((tagrpsgi==NULL) || (tagrpsdomid==NULL) ||(tagrpsnumdom==NULL) ||(tagrpslen==NULL))
	{
		ErrPostEx(SEV_ERROR,0,0," RPSDB tags not initialized");
		return FALSE;
	}
	if(pathAllocated)MemFree(path);
	return TRUE;
	}

/*Open domain name database*/
Boolean LIBCALL OpenDOMNAME(CharPtr name, Boolean create)
{
	Char pathdatab[PATH_MAX];
	CharPtr path=NULL;

	/*Get info from config file*/
	GetAppParam("intrez", "datab","pathrps", "", pathdatab, (size_t)PATH_MAX*(sizeof(char)));
	if((pathdatab!=NULL)&&(StringCmp(pathdatab, "")!=0)) {
		path = AllocateAndCreateDBPath(pathdatab, name);
	}
	else if(name!=NULL) {
		path=name;
	}
	else {
		ErrPostEx(SEV_ERROR,0,0,"OpenDOMNAME: Can't find a path to open DOMNAME database table in either argument list or intrez configuration file.");
		return(FALSE);
	}

	/* Open */
	if(create == FALSE)
	{
		pd4domname = d4open(&cb, path);
		if(pd4domname == NULL)
		{
			MemFree(path);
			return FALSE;
		}
	}

	/* create */
	else {
		pd4domname = d4create(&cb, path, f4domname, t4domname);
		if (cb.errorCode)
		{
			MemFree(path);
			return FALSE;
		}
	}

	/* fields */
	pf4dnaccession = d4field(pd4domname, "ACCESSION");
	pf4dnname = d4field(pd4domname, "NAME");
	pf4dnpdbid = d4field(pd4domname, "PDBID");
	pf4dnasn1 = d4field(pd4domname, "ASN1");

	/* tags */
	tagdnaccession = d4tag (pd4domname, "ACCESSION");
	tagdnname = d4tag (pd4domname, "NAME");
	tagdnpdbid = d4tag (pd4domname, "PDBID");

	MemFree(path);
	return TRUE;
}


/***********************
*Print Functions
***********************/

/*Print RPSDB */
Boolean LIBCALL PrintRPSDB(void)
{
	int rc;
	Int4 gi=0, from=0, domnum=0, missing_n=0, missing_c=0, align_len=0, cddid=0,score=0;
	Uint4 count=0;
	CharPtr domain_id=NULL;
	FloatHi evalue=0.0, bitscore=0.0;
	for(rc=d4top(pd4rpsdb); rc==r4success; rc=d4skip(pd4rpsdb, (long)1))
	{
		/*domain_id=GetCddIDFromRPSDB();*/
		gi=f4long(pf4rpsgi);
		domain_id=f4str(pf4rpsdomid);
		from = f4long(pf4rpsfrom);
		missing_n = f4long(pf4rpsmisN);
		missing_c = f4long(pf4rpsmisC);
		evalue = f4double(pf4rpsevalue);
		align_len = f4long(pf4rpsalignlen);
		domnum = f4long(pf4rpsnumdom);
		cddid=f4long(pf4rpscddid);
		score = f4long(pf4rpsscore);
		bitscore = f4double(pf4rpsbitscore);
		fprintf(stdout, "%ld: %ld: %s: %ld: %ld: %e: %e: %ld: %ld: %ld: %ld:\n",
		(long)gi,(long)cddid, (char*)IsNullStr(domain_id),(long)from,(long)score, (double)evalue,(double)bitscore,
			(long)missing_c, (long)missing_n, (long)align_len, (long)domnum);
		count++;
	}
	fprintf(stdout, "Number of records: %ld\n", (long)count);
	return TRUE;
}

/*Print Domain Name table*/
Boolean LIBCALL PrintDOMNAME(void)
{
	int rc;
	Int4 num=0;
	CharPtr accession=NULL, pdbid=NULL, name=NULL;
	for (rc= d4top(pd4domname); rc == r4success; rc = d4skip(pd4domname, (long)1))
	{
		num++;
		accession=f4str(pf4dnaccession);
		printf("%s: ", (char*)IsNullStr(accession));
		name=f4str(pf4dnname);
		printf("%s:", (char*)IsNullStr(name) );
		pdbid = f4str(pf4dnpdbid);
		printf("%s:\n", (char*)IsNullStr(pdbid) );
	}
	printf("Number of records: %ld\n", (long) num);
	return TRUE;
}

/***********************
* Utilities functions
***********************/

/* Utility function to read Cdd asn1*/
CddPtr LIBCALL GetCddFROMRDOMNAME(void)
	{
	int iSize = 0;
	CharPtr pcASN = NULL;

	CddPtr cddptr = NULL;
	AsnIoMemPtr aimp;


	/* dump out memo field into a file and read it up into memory with BioseqAsnRead */
	iSize = f4memoLen(pf4dnasn1);
	pcASN = (CharPtr) MemNew((size_t) iSize+1);
	if (pcASN == NULL)
	{
		ErrPostEx(SEV_ERROR,0,0, "GetCddIdFROMDOMNAME: Failed to allocate memory.");
		if (pcASN) MemFree(pcASN);
			return NULL;
	}
	f4memoNcpy(pf4dnasn1, pcASN, (unsigned) iSize);
	aimp = AsnIoMemOpen("rb", (BytePtr) pcASN, iSize);
	if (aimp == NULL)
	{
		ErrPostEx(SEV_ERROR,0,0, "GetCddIdFROMDOMNAME: Failed to get AsnIo.");
		if (pcASN) MemFree(pcASN);
		return NULL;
	}
	cddptr = CddAsnRead(aimp->aip, NULL);
	if (cddptr == NULL)
	{
		ErrPostEx(SEV_ERROR,0,0, "GetSLRIFastaCD: Bioseq pointer is NULL.");
		if (pcASN) MemFree(pcASN);
		AsnIoMemClose(aimp);
		return NULL;
	}

	AsnIoFlush(aimp->aip);
	AsnIoMemClose(aimp);
	if (pcASN) MemFree(pcASN);
	return cddptr;
}

/*
$Log: rpsdb_cb.c,v $
Revision 1.10  2004/06/07 22:01:10  hlieu
minor fix, bug 2185, cross comp warnings

Revision 1.9  2004/06/07 21:59:08  hlieu
minor fix

Revision 1.8  2004/06/02 18:02:45  hlieu
commented out recordLockWriteCount reference. not supported in non-intel unix
platforms

Revision 1.7  2004/05/12 20:10:11  mmatan
Modified the SearchRPSDB function so that in the cases where it gets no hits in
the rpsdb, it does not delete the pprs argument/return structure, it simply does
not modify it.  This fixes bug 2215.

Revision 1.6  2004/02/16 16:45:40  mmatan
Specify deletion of all records for an obselete GI and set codebase option so that records set for deletion are actually deleted.

Revision 1.5  2004/01/12 17:46:36  mmatan
Altered TAG4INFO structure t4domname in order to fix bug 1061 which caused
crashes on solaris.  Added RPS_InitCodeBase and RPS_CloseCodeBase function
definitions as per Zhe's request.

Revision 1.4  2004/01/06 22:32:16  skuznets
Code audit has been done sucessfully.
Possible NULL pointer issues has been eliminated.

Revision 1.3  2003/11/27 20:14:22  idonalds
removed comments

Revision 1.2  2003/11/03 15:50:27  haocl
changed order of includes to avoid _LARGE_FILE_SOURCE redefinition warnings

Revision 1.1.1.16  2002/11/05 09:35:19  root
Automated import from Sourceforge.net

Revision 1.23  2002/11/04 19:09:53  betel
Changed field size

Revision 1.22  2002/11/04 15:27:41  betel
Fixed crosscomp warnings

Revision 1.21  2002/10/30 17:47:07  betel
Added IsExist* functions

Revision 1.20  2002/10/28 23:41:01  betel
Removed #ifdef NEW_RPSDB, new tables fields are in effect

Revision 1.19  2002/10/26 20:54:41  betel
Fixed RPSDBSearch

Revision 1.18  2002/10/25 00:05:08  betel
Changed field in DOMNAME table and added 3D strcture function for domains

Revision 1.17  2002/10/15 17:51:39  betel
Changes to delete functions

Revision 1.16  2002/10/13 22:54:04  betel
Added table packing calls

Revision 1.15  2002/09/30 21:03:35  betel
Added CDDid, score and bit score fields (contains temp ifdef's)

Revision 1.14  2002/09/30 18:57:31  gbader
Fixed open domname table function - now accepts path parameter properly (same as previous fix for Open rpsdb function)
*/
