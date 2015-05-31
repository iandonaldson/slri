/*$Id: rpsdb.c,v 1.11 2004/04/22 20:47:47 mmatan Exp $*/
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


  PROGRAM: RPSDB- Fills RPS database that computes domain
  		annotation for proteins.

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
*/
#include <d4all.h>
#include "rpsdb.h"
#include <rpsdb_db.h>
/*#include <rpsutil.h>*/
CddHitPtr RPSBgetCddHits(SeqAlignPtr sap);
CddHitPtr CddHitDestruct(CddHitPtr cdd);
/*global variables that marks the end of sequences
* that are "BLASTed"*/
static Boolean glb_end_of_data;
static CharPtr g_cdd_path;
static CharPtr g_cdd_filename;
static ReadDBFILEPtr g_rdbfp=NULL; /*used in FASTA mode*/
static Uint4 g_counter;
static Uint4 g_end_index; /*last sequence in FASTA file to use*/
static FloatHi g_expectVal;
static MonitorPtr g_monitor;

/*Use this cb variable when using remote SeqHound*/
#ifndef _INTREZ_CB_
CODE4 cb;
#endif

/*Callback function when running rpsdb using SeqHound Redund table
* Called (internally) by RPS-BLAST to process the resutls
* data is the result structure that needs to be filled up
*/
static Boolean LIBCALLBACK RPSDBSHoundRedundResultsCallback(BioseqPtr query_bsp,RPSBlastOptionsPtr rpsbop,SeqAlignPtr seqalign,
		ValNodePtr other_returns,ValNodePtr error_returns,VoidPtr data)
{
	/*Variables*/
	SeqAlignPtr sapHere=NULL, sapHead=NULL;
	ObjectIdPtr oipAlign=NULL;
	DbtagPtr dtpHere=NULL;
	CddHitPtr cdd_head=NULL, cdd=NULL;
	StRpsdbPtr results=NULL, headResults=NULL;

	/* SK: NULL ptr check */
	ValNodePtr gi_list;
	Int4 seqAlignCounter=0,distinct_targets=0;
	Int4 nmissg=0, cmissg=0, query_start=0,query_stop=0, cnt=0;
	DenseSegPtr dspHere=NULL;
	SeqIdPtr sipQuery=NULL,sipAlign=NULL, subject_id=NULL;
	CharPtr temp=NULL, definition=NULL;
	BioseqPtr bspAlign=NULL;

	/* SK: NULL ptr check */
	if ( data == NULL )
	{
	    ErrPostEx(SEV_ERROR,0,0,"RPSDBResultsCallback: NULL data");
	    return FALSE;
	}
	gi_list=*((ValNodePtr*)data);

	/* check if any hits were found */
	if(seqalign==NULL)
	{
		(*(ValNodePtr*)data)=(*(ValNodePtr*)data)->next;
		ErrPostEx(SEV_ERROR,0,0,"RPSDBResultsCallback: no hits found");
		return FALSE;
	}

	/*Allocate StRpsDB struc*/
	results=StRpsdbNew();
	headResults=results;
	results->gi=gi_list->data.intvalue;

	/* traverse SeqAlign pointer which contains the list of hits
	* each SeqAlign is made up by DenseSeg segments*/
	sapHead=seqalign;
	sapHere=sapHead;

	/*count domains*/
	seqAlignCounter=SLRICddCountSeqAligns(seqalign,&distinct_targets);

	results->num_of_dom=seqAlignCounter;

	/*Cdd hits*/
	cdd_head=(CddHitPtr)RPSBgetCddHits(sapHere);
	cdd=cdd_head;
	/*traverse SeqAlign list*/
	while(sapHere!=NULL && cdd!=NULL){
		/*Check the SeqAlign type */
		if(sapHere->type!=SAT_PARTIAL || sapHere->segtype!=SAS_DENSEG ||
			sapHere->dim!=2){
			ErrPostEx(SEV_FATAL, 0,0,"RPSDBResultsCallback:An unexpected error has occurred");
		}

		dspHere=(DenseSegPtr)(sapHere->segs);

		/* Get subjectId (domain) from the alignment */
                subject_id=SeqIdDup(TxGetSubjectIdFromSeqAlign(sapHere));
		/* retreive domain's Bioseq, name and len */
		bspAlign=BioseqLockById(subject_id);
		if(bspAlign==NULL){
			ErrPostEx(SEV_ERROR, 0,0,"RPSDBResultsCallback: cannot create domain's Bioseq");
		}

		/*get missing-n and missing-c.
		* Dimension is always 2; there is always two sequences.
		* To reach the next segment (i.e. block of alignment)
		* we jump two positions forward in the array.
		* Nmissing portion is simply the start position of the query
		* against the consesus. Cmissing is strat+len */
		for (cnt=0;cnt<dspHere->numseg;cnt++) {
			if (dspHere->starts[2*cnt]>-1) {
				query_start=dspHere->starts[2*cnt]+1;
				nmissg=dspHere->starts[2*cnt+1];
				break;
			}
		}
		for (cnt=0;cnt<dspHere->numseg;cnt++) {
			if (dspHere->starts[2*cnt]>-1) {
				query_stop=dspHere->starts[2*cnt]+1+dspHere->lens[cnt]-1;
			}
			if (dspHere->starts[2*cnt+1]>-1) {
				cmissg=dspHere->starts[2*cnt+1]+dspHere->lens[cnt]-1;
			}
		}
		/*Get query and align names, first SeqId is always query*/
		sipQuery=(SeqIdPtr)(dspHere->ids);
		sipAlign=(SeqIdPtr)(sipQuery->next);

		switch(sipAlign->choice){
			case SEQID_LOCAL:
				oipAlign=(ObjectIdPtr)(sipAlign->data.ptrvalue);
				break;
			case SEQID_GENERAL:
				/*CDD, pfam or Smart db tag*/
				dtpHere=(DbtagPtr)(sipAlign->data.ptrvalue);
				/*db unique identifier*/
				oipAlign=(ObjectIdPtr)(dtpHere->tag);
				results->cdd_id=oipAlign->id;
				break;
			default:
				ErrPostEx(SEV_FATAL, 0,0,"RPSDBResultsCallback:Wrong SeqId");
		}

		if((cdd->CDDid)==NULL) {
		  ErrPostEx(SEV_FATAL,0,0,"[RPSDBResultsCallback]: CDD:%ld has not domain_id",(long)oipAlign->id);

		  results->dom_id=StringSave(cdd->CDDid);
		}
		else {

		  results->dom_id=StringSave(cdd->CDDid);
		}


		/*store information*/
		results->from=cdd->start;
		results->len=(cdd->stop)-(cdd->start)-1;
		results->n_missing=nmissg;
		results->c_missing=bspAlign->length-cmissg-1;
		results->score=cdd->score;
		results->bitscore=cdd->bit_score;
		results->evalue=cdd->evalue;

		cdd=cdd->next;
		sapHere=sapHere->next;
		/*PrintResults(results);*/
		BioseqUnlockById(subject_id);
		/* Add results to the linked list
		* and initialize the next results structure */

		if(sapHere!=NULL){
			results->next=StRpsdbNew();
			results->next->gi=results->gi;
			results->next->num_of_dom=seqAlignCounter;
	}
	CddHitDestruct(cdd_head);
	/* Add results to database*/

		/*IMPORTANT!- GI ValNodePtr is incremented here, at the end of the
		 * Results Callback function. See Notes for details.*/
		(*(ValNodePtr*)data)=(*(ValNodePtr*)data)->next;

		ErrPostEx(SEV_ERROR, 0,0,"RPSDBResultsCallback: commit to table failed");
		return FALSE;

	}

	/* free results */
	StRpsdbFree(headResults);

	/*IMPORTANT!- GI ValNodePtr is incremented here, at the end of the
	* Results Callback function. See Notes for details.*/
	(*(ValNodePtr*)data)=(*(ValNodePtr*)data)->next;

	return TRUE;
}


/*Callback function when running rpsdb using FASTA index table (a la - formatdb style)
* Same deal as RPSDBSHoundRedundResultsCallback except for minor changes.
* Called (internally) by RPS-BLAST to process the resutls
* data is the result structure that needs to be filled up
*/
static Boolean LIBCALLBACK RPSDBFastaResultsCallback(BioseqPtr query_bsp,RPSBlastOptionsPtr rpsbop,SeqAlignPtr seqalign,
		ValNodePtr other_returns,ValNodePtr error_returns,VoidPtr data)
{
	/*Variables*/
	SeqAlignPtr sapHere=NULL,sapHead=NULL;
	DenseSegPtr dspHere=NULL;
	SeqIdPtr subject_id=NULL,sipQuery=NULL, sipAlign=NULL;
	Int4 domainlen=0, cnt=0;
	Int4 query_stop=0, query_start=0, cmissg=0,nmissg=0, seqAlignCounter=0;
	DbtagPtr dtpHere=NULL;
	CddHitPtr cdd_head=NULL, cdd=NULL;
	ObjectIdPtr oipAlign=NULL;
	BioseqPtr bspAlign=NULL;
	StRpsdbPtr results=NULL, headResults=NULL;
	Int4 distinct_targets=0; /*used to count the number of
				distinct target sequences represented*/
	CharPtr temp=NULL, definition=NULL;

	static TNlmMutex read_mutex;
	BioseqPtr bseqptr=NULL;

	DenseSegPtr dsp;
	Int4 numseg, num_ident, align_length;

	/* SK: NULL ptr check */
	if(data==NULL)
	{
		ErrPostEx(SEV_INFO,0,0,"RPSDBResultsCallback: NULL data");
		return FALSE;
	}

	/* check is any hits were found */
	if(seqalign==NULL)
	{
		ErrPostEx(SEV_INFO,0,0,"RPSDBResultsCallback: no hits found");
		return FALSE;
	}

	/*Allocate StRpsDB struc*/
	results=StRpsdbNew();
	results->gi=(*((Int4*)data))-1;	/*Correct already incremented ordinal ID in RPSDBFastaGetSeq*/

	/* Get the sequence's gi from the bioseq corresponding to this index of the blast database */
	
	/*Initialize and lock mutex memory
	* thread safe; so no threads can touch it*/
	NlmMutexInit(&read_mutex);
	NlmMutexLock(read_mutex);

	bseqptr=readdb_get_bioseq(g_rdbfp,results->gi);
	if(bseqptr==NULL)
	{
		glb_end_of_data = TRUE;
		NlmMutexUnlock(read_mutex);
		ErrPostEx(SEV_ERROR, 0,0,"RPSDDFastaResultsCallback: No Bioseq found for index %ld", results->gi);
		return FALSE;
	}
	NlmMutexUnlock(read_mutex);

	/* extract the gi for this record from the bioseq */
	results->gi=(Int4)bseqptr->id->data.intvalue;
	headResults=results;

	/* traverse SeqAlign pointer which contains the list of hits
	* each SeqAlign is made up by DenseSeg segments*/
	sapHead=seqalign;
	sapHere=sapHead;

	/*get SeqAlign info */
	seqAlignCounter=SLRICddCountSeqAligns(seqalign,&distinct_targets);

	results->num_of_dom=seqAlignCounter;

	/*Cdd hits*/
	cdd_head=(CddHitPtr)RPSBgetCddHits(sapHere);
	cdd=cdd_head;
	/*traverse SeqAlign list*/
	while(sapHere!=NULL && cdd!=NULL){
		if(sapHere->type!=SAT_PARTIAL || sapHere->segtype!=SAS_DENSEG ||
			sapHere->dim!=2){
			ErrPostEx(SEV_FATAL, 0,0,"RPSDBResultsCallback:An unexpected error has occurred");
		}
		dspHere=(DenseSegPtr)(sapHere->segs);

		/* Get subjectId (domain) from the alignment */
		subject_id=SeqIdDup(TxGetSubjectIdFromSeqAlign(sapHere));

		bspAlign=BioseqLockById(subject_id);
		if(bspAlign==NULL){
			ErrPostEx(SEV_ERROR, 0,0,"[RPSDBFastaResultsCallback]:cannot create domain's Bioseq");
		}

		/*get missing-n and missing-c.
		* Dimension is always 2; there are always two sequences.
		* To reach the next segment (i.e. block of alignment)
		* we jump two positions forward in the array.
		* Nmissing portion is simply the start position of the query
		* against the consesus. Cmissing is strat+len */
		for (cnt=0;cnt<dspHere->numseg;cnt++) {
			if (dspHere->starts[2*cnt]>-1) {
				query_start=dspHere->starts[2*cnt]+1;
				nmissg=dspHere->starts[2*cnt+1];
				break;
			}
		}
		for (cnt=0;cnt<dspHere->numseg;cnt++) {
			if (dspHere->starts[2*cnt]>-1) {
				query_stop=dspHere->starts[2*cnt]+1+dspHere->lens[cnt]-1;
			}
			if (dspHere->starts[2*cnt+1]>-1) {
				cmissg=dspHere->starts[2*cnt+1]+dspHere->lens[cnt]-1;
			}
		}

		/*Get query and align names*/
		sipQuery=(SeqIdPtr)(dspHere->ids);
		sipAlign=(SeqIdPtr)(sipQuery->next);

		switch(sipAlign->choice){
			case SEQID_LOCAL:
				oipAlign=(ObjectIdPtr)(sipAlign->data.ptrvalue);
				break;
			case SEQID_GENERAL:
				dtpHere=(DbtagPtr)(sipAlign->data.ptrvalue);
				oipAlign=(ObjectIdPtr)(dtpHere->tag);
				results->cdd_id=oipAlign->id;
				break;
			default:
				ErrPostEx(SEV_FATAL, 0,0,"RPSDBResultsCallback:Wrong SeqId");
		}

		if((cdd->CDDid)==NULL) {
		  ErrPostEx(SEV_FATAL,0,0,"[RPSDBResultsCallback]: CDD:%ld has not domain_id",(long)oipAlign->id);

		  results->dom_id=StringSave(cdd->CDDid);
		}
		else {

		  results->dom_id=StringSave(cdd->CDDid);
		}

		align_length=0;
		for (cnt=0; cnt<dspHere->numseg; cnt++) {
		  align_length += dspHere->lens[cnt];
		}

		/*store information*/
		results->from=query_start;
		results->len=align_length; 
		results->n_missing=nmissg;
		results->c_missing=bspAlign->length-cmissg-1;
		results->score=cdd->score;
		results->bitscore=cdd->bit_score;
		results->evalue=cdd->evalue;

		/* Add results to the linked list
		* and initialize the next results structure */
		cdd=cdd->next;
		sapHere=sapHere->next;
		BioseqUnlockById(subject_id);
		if(sapHere!=NULL){
			results->next=StRpsdbNew();
			results->next->gi=results->gi;
			results->next->num_of_dom=seqAlignCounter;
			results=results->next;
		}
	}
	CddHitDestruct(cdd_head);
	/* Add results to database */
	if(!AddResultsToTable(headResults, seqAlignCounter))
	{
		ErrPostEx(SEV_ERROR, 0,0,"RPSDBResultsCallback: commit to table failed");
		return FALSE;
	}

	/* free results */
	StRpsdbFree(headResults);
	return TRUE;
}


/* Get the next sequence to BLAST.
*Called by RPS-BLAST to collect the next sequence when using SeqHound Redund table
* data- the next sequence to read
* slp- is a dummy pointer. Not used
* Note- the gi_list ValNode is NOT incremented forward in this function
* that is done in the results callback */
static SeqEntryPtr LIBCALLBACK RPSDBSHoundRedundGetSeq(SeqLocPtr PNTR slp, VoidPtr data)
{
	SeqEntryPtr sep=NULL;
	BioseqPtr bseqptr=NULL;
	ValNodePtr gi_list=*((ValNodePtr*)data);
	Int4 Gi=-1;
	static TNlmMutex read_mutex;

	ValNodePtr redundgroupptr = NULL;

	/*Check if this is the end of GI list*/
	if(gi_list==NULL){
		glb_end_of_data=TRUE;
		return NULL;
	}
	/*Initialize and lock mutex memory
	* thread safe; so no threads can touch it*/
	NlmMutexInit(&read_mutex);
	NlmMutexLock(read_mutex);

	/*if no more seq stop blast*/
	if (glb_end_of_data) {
		NlmMutexUnlock(read_mutex);
		return NULL;
	}
	/* collect bioseq from seqhound*/
	if(!SHoundIsInited()){
		glb_end_of_data = TRUE;
		NlmMutexUnlock(read_mutex);
		ErrPostEx(SEV_ERROR, 0,0,"RPSDBGetSeq: Seqhound is not available");
		return sep;
	}

	if(gi_list->choice!=GI_CHOICE)
	{
		NlmMutexUnlock(read_mutex);
		glb_end_of_data = TRUE;
		ErrPostEx(SEV_ERROR,0,0,"RPSDBGetSeq: Wrong ValNode choice selected");
		return NULL;
	}

	Gi=gi_list->data.intvalue;

	bseqptr = NULL;
	while (bseqptr == NULL) {
	  if ((bseqptr=SHoundGetBioseq(Gi))==NULL) {	    
	    ErrPostEx(SEV_INFO, 0,0,"RPSDBGetSeq: No Bioseq found for %ld, attempting to retrieve it's redundant group... ", Gi);
	    if ((redundgroupptr=SHoundRedundantGroup(Gi)) != NULL) {
	      ErrPostEx(SEV_INFO, 0,0,"RPSDBGetSeq: Redundant group found for %ld, attempting to retrieve alternate GI with sequence information... ", Gi);
	      while (redundgroupptr != NULL) {
		if ((bseqptr=SHoundGetBioseq(redundgroupptr->data.intvalue)) != NULL) {
		  ErrPostEx(SEV_INFO, 0,0,"RPSDBGetSeq: Alternate Gi found for %ld which has sequence information in seqhound", Gi);
		  break;
		}
		else {
		  redundgroupptr=redundgroupptr->next;
		  if (redundgroupptr == NULL) {
		    ErrPostEx(SEV_WARNING, 0,0,"RPSDBGetSeq: No alternate GI with sequence information found for %ld, skipping this redundant group/sequence", Gi);
		    gi_list=gi_list->next;
		    /*increment the actual (global) GI list as well, so that results callback gets the right info, and this same sequence is not rpsblasted again*/
		    (*(ValNodePtr*)data)=(*(ValNodePtr*)data)->next;
		    /*Check if this is the end of GI list*/
		    if(gi_list==NULL){
		      glb_end_of_data=TRUE;
		      return NULL;
		    }
		    Gi=gi_list->data.intvalue;
		  }
		}
	      }
	      
	      
	    }
	    else {
	      ErrPostEx(SEV_WARNING, 0,0,"RPSDBGetSeq: No redundant group found for %ld, skipping this sequence. ", Gi);
	      /*increment the actual(global) GI list as well, so that results callback gets the right info, and this same sequence is not rpsblasted again*/
	      (*(ValNodePtr*)data)=(*(ValNodePtr*)data)->next;
	      gi_list=gi_list->next;
	      /*Check if this is the end of GI list*/
	      if(gi_list==NULL){
		glb_end_of_data=TRUE;
		return NULL;
	      }
	      Gi=gi_list->data.intvalue;
	    }
	  }
	}
	    
	/* create a new seq entry for the bioseq */
	sep=(SeqEntryPtr)SeqEntryNew();
	sep->choice=1; /*=bioseq*/
	sep->data.ptrvalue=bseqptr;
	NlmMutexUnlock(read_mutex);
	return sep;
}

/* Get the next sequence to BLAST.
* Called by RPS-BLAST to collect the next sequence when using FASTA files
* just like its close cousin PSI-BLAST :)
* data- the next sequence to read
* slp- is a dummy pointer. Not used
*/
static SeqEntryPtr LIBCALLBACK RPSDBFastaGetSeq(SeqLocPtr PNTR slp, VoidPtr data)
{
	SeqEntryPtr sep=NULL;
	BioseqPtr bseqptr=NULL;
	Int4 fdb_index=*((Int4*)data);
	static TNlmMutex read_mutex;

	/*Check if this is the end of GI list*/
	if((Uint4) fdb_index > g_end_index){
		glb_end_of_data=TRUE;
		return NULL;
	}

	/*Initialize and lock mutex memory
	* thread safe; so no threads can touch it*/
	NlmMutexInit(&read_mutex);
	NlmMutexLock(read_mutex);

	/*if no more seq stop blast*/
	if (glb_end_of_data) {
		NlmMutexUnlock(read_mutex);
		return NULL;
	}

	bseqptr=readdb_get_bioseq(g_rdbfp, fdb_index);
	if(bseqptr==NULL)
	{
		glb_end_of_data = TRUE;
		NlmMutexUnlock(read_mutex);
		ErrPostEx(SEV_ERROR, 0,0,"RPSDBGetSeq: No Bioseq found for %ld", fdb_index);
		return sep;
	}

	/* create a new seq entry for the bioseq */
	sep=(SeqEntryPtr)SeqEntryNew();
	sep->choice=1; /*=bioseq*/
	sep->data.ptrvalue=bseqptr;
	NlmMutexUnlock(read_mutex);

	MonitorIntValue(g_monitor, fdb_index);
	/*move formatdb index forward */
	(*((Int4*)data))++;
	return sep;
}


/* First function called.
* Initializes RPS-BLAST options structure
* and calls RPSBlastSearchMT */
SLRI_ERR RPSDatabaseBlast(ValNodePtr gi_list, Int2 numpass, RunningMode mode)
{
	RPSBlastOptionsPtr rpsbop;
	BLAST_OptionsBlkPtr options;
	Char cdd_dbpath[PATH_MAX];
	glb_end_of_data=FALSE;

	/*results must contain the gi number*/
	if(gi_list == NULL)
	{
		ErrPostEx(SEV_ERROR, 0,0,"RPSDatabaseBlast: Results pointer is not initialized");
		return SLRI_FAIL;
	}


	/*******************************
	* Initialize rps-BLAST options *
	********************************/
	/* Note that there are two options structs
	* RPSBlastOptions are rps-specific options
	* and BLAST_OptionsBlk are golbal BLAST options defined in
	* blastdef.h*/

	rpsbop=MemNew(sizeof(RPSBlastOptions));

	/* Note: these 2 parameters are necessary to intialize RPS Blast */
	rpsbop->query_is_protein=TRUE;
	rpsbop->num_threads=NUM_THREADS;

	/* Handle of the domain database */
	sprintf(cdd_dbpath, "%s%s%s", IsNullStr(g_cdd_path), (g_cdd_path[strlen(g_cdd_path)-1]==DIRDELIMCHR)?"":DIRDELIMSTR, IsNullStr(g_cdd_filename));
	if((rpsbop->rdfp=readdb_new(cdd_dbpath,TRUE))==NULL) {
		return SLRI_FAIL;
	}

	rpsbop->rps_database=StringSave(cdd_dbpath);

	/*Don't know what this is*/
	rpsbop->believe_query=FALSE;
	options=BLASTOptionNew("blastp",TRUE);
	/* SK: NULL ptr check */
	if ( options == NULL )
	    return SLRI_FAIL;

	rpsbop->options=options;

	/* Necessary options for RPS Blast */
	options->do_sum_stats=FALSE;
	options->is_rps_blast=TRUE;

	/*number of alignments*/
	rpsbop->number_of_descriptions=500;
	rpsbop->number_of_alignments=500;
	BLASTOptionSetGapParams(options,BLOSUM_FNAME,0,0);
	rpsbop->align_options=TXALIGN_COMPRESS+TXALIGN_END_NUM+TXALIGN_MASTER+TXALIGN_FLAT_INS+TXALIGN_BLUNT_END;
	rpsbop->print_options=0;

	/* decrement by one to agree with program values. */
	/*What region of the query is required for the alignment*/
	options->required_start=0;
	options->required_end=-1;
	if (options->required_end!=-1)
		options->required_end--;
	options->dropoff_2nd_pass=7;
	options->expect_value=g_expectVal;
	options->hitlist_size=MAX(rpsbop->number_of_descriptions,rpsbop->number_of_alignments);

	/* { gapped BLAST only */
	if (numpass==0) {
		options->two_pass_method=FALSE;
		options->multiple_hits_only=TRUE;
	}
	else if (numpass==1) {
		options->two_pass_method=FALSE;
		options->multiple_hits_only=FALSE;
	}
	else {
		options->two_pass_method=TRUE;
		options->multiple_hits_only=FALSE;
	}
	options->gap_open=11;
	options->gap_extend=1;
	options->gap_x_dropoff=15;
	options->gap_x_dropoff_final=25;
	options->gap_trigger=22.0;

	/* } gapped BLAST */
	/* set to S to filter */
	options->filter_string=StringSave("F");

	/* Only one CPU may be used at this time inside the core engine*/
	options->number_of_cpus=(Int2)1;
	options->isPatternSearch=FALSE;
	options=BLASTOptionValidate(options,"blastp");
	if (options==NULL) {
		ErrPostEx(SEV_ERROR, 0,0,"RPSDatabaseBlast: error in rps options");
		return SLRI_FAIL;
	}

	/*****************
	* call RPS-BLAST *
	******************/
	if(mode==eSHoundRedund){
		ErrPostEx(SEV_INFO,0,0,"Running RPS-BLAST using SeqHound Redund table");
		RPSBlastSearchMT(rpsbop,RPSDBSHoundRedundGetSeq,(VoidPtr)&gi_list,RPSDBSHoundRedundResultsCallback,
				(VoidPtr)&gi_list);
	}
	else if(mode==eFASTA){
		ErrPostEx(SEV_INFO,0,0,"Running RPS-BLAST using FASTA files");
		RPSBlastSearchMT(rpsbop,RPSDBFastaGetSeq, (VoidPtr)&(gi_list->data.intvalue), RPSDBFastaResultsCallback, (VoidPtr)&(gi_list->data.intvalue));
	}
	/* free */
	BLASTOptionDelete(rpsbop->options);
	readdb_destruct(rpsbop->rdfp);
	MemFree(rpsbop->rps_database);
	rpsbop=MemFree(rpsbop);
	return SLRI_SUCCESS;
}

void PrintResults(StRpsdbPtr results)
{
	/* SK: NULL ptr check */
	if ( results == NULL )
	    return;

	fprintf(stdout, "%ld: %ld: %s: %ld: %ld: %e: %e :%ld: %ld: %ld: %ld:\n",
			(long)results->gi,(long)results->cdd_id, (char*)IsNullStr(results->dom_id),(long)results->from,
				(long)results->score,(double)results->evalue,(double)results->bitscore, (long)results->c_missing, (long)results->n_missing,
						(long)results->len, (long)results->num_of_dom);
}

/*Count the number of SeqAligns (i.e.  domains) and number of distinct domains*/
Int4 LIBCALL SLRICddCountSeqAligns(SeqAlignPtr listOfSeqAligns, Int4 * numSequences)
{
	SeqAlignPtr curSeqAlign, prevSeqAlign;
	Int4        seqAlignCounter;
	DenseSegPtr curSegs;
	SeqIdPtr    curId=NULL, prevId=NULL;
	seqAlignCounter = 0;
	/* SK: NULL ptr check */
	if ( numSequences == NULL )
	    return -1;

	*numSequences = 0;
	curSeqAlign = listOfSeqAligns;
	prevSeqAlign = NULL;
	while (NULL != curSeqAlign) {
		curSegs = (DenseSegPtr) curSeqAlign->segs;
		if(curSegs->ids == NULL) break;
			curId = curSegs->ids->next;
			seqAlignCounter++;
		if ((NULL == prevSeqAlign) || (!(SeqIdMatch(curId, prevId))))
				(*numSequences)++;
		prevSeqAlign = curSeqAlign;
		prevId = curId;
		curSeqAlign = curSeqAlign->next;
	}
	return(seqAlignCounter);
}

/*commit the results list to the database*/
Boolean AddResultsToTable(StRpsdbPtr head, Int4 counter)
{
	Int4 i=0;
	StRpsdbPtr currnt=head;
	if(counter<=0){
		ErrPostEx(SEV_ERROR, 0,0,"AddResultsToTable: num of domains is incorrect");
		return FALSE;
	}
	while(currnt!=NULL)
	{
		if(!AppendRecordRPSDB(currnt)){
			ErrPostEx(SEV_ERROR, 0,0,"AddResultsToTable: Appending record failed");
			return FALSE;
		}
		i++;
		currnt=currnt->next;
	}

	/*check if the number of domains is equal to records appended */
	if(i != counter){
		ErrPostEx(SEV_ERROR, 0,0,"AddResultsToTable: Not all results were added to database");
		return FALSE;
	}
	g_counter++;
	return TRUE;
}

/*Delete the list of GIs from the table
* ValNode is a list of ValNodes. Each node is a ValNode of redundant GIs.
* if the first gi is not in table we search the full list until completed */
Boolean DeleteFromTable(ValNodePtr delete)
{
	Int4 i=0;
	ValNodePtr givnp=NULL;
	if(delete==NULL){
		ErrPostEx(SEV_ERROR,0,0,"[DeleteFromTable]: GI list is empty");
		return FALSE;
	}
	while(delete!=NULL){
		givnp=delete->data.ptrvalue;
		while(givnp!=NULL && !DeleteGIinRPSDB(givnp->data.intvalue,TRUE)){
			givnp=givnp->next;
		}
		/* SK: could be logical error */
		if(givnp==NULL){
			givnp=delete->data.ptrvalue;
			ErrPostEx(SEV_WARNING,0,0,"[DeleteFromTable]: error in removing %ld",(long)givnp->data.intvalue);
			i--;
		}

		i++;
		delete=delete->next;
	}
	/*Pack table */
	PackRPSDB();
	ErrPostEx(SEV_INFO,0,0,"[DeleteFromTable]: %ld records removed from database", (long)i);
	return TRUE;

}
/*check the table. */
void CheckTable(void)
{
	/*Initialize code base*/
	if(!RPS_InitCodeBase()){
		ErrPostEx(SEV_ERROR, 0,0,"CheckTable: cannot Initialize codebase");
		return;
	}

	/* open RPSDB */
	if(!OpenRPSDB("RPSDB", FALSE)){
		ErrPostEx(SEV_ERROR, 0,0,"CheckTable: cannot open RPSDB");
		return;
	}
	/* print RPSDB */
	if(!PrintRPSDB()){
		ErrPostEx(SEV_ERROR, 0,0,"CheckTable: cannot print RPSDB");
		return;
	}

	CloseRPSDB();
	RPS_CloseCodeBase();
	return;
}

void FreeResults(StRpsdbPtr head)
{
	StRpsdbPtr prev=head;
	/* SK: This check is not necessary */
	if (head==NULL)
			return;
	while(head!=NULL)
	{
		prev=head;
		head=head->next;
		StRpsdbFree(head);
	}
	return;
}

/*Get GI from SeqId*/
Int4 GetGIFromSeqId(SeqIdPtr sip)
{
	if(sip==NULL){
		ErrPostEx(SEV_ERROR,0,0,"[GetGIFromSeqId]SeqId is null");
		return -1;
	}
	if(sip->choice==SEQID_GI)
		return sip->data.intvalue;
	return -1;
}

#define MAX_LINE 6000
Int2 Main(){
	Args myargs[]={
		{"Input fasta file (formatdb must have been run on it) or file containing GI list.",
			NULL, NULL,NULL, FALSE, 'i', ARG_FILE_IN, 0.0, 0, NULL},
		{"Input GI list to be deleted (required if running in update mode)",NULL,NULL,NULL, TRUE, 'd', ARG_FILE_IN, 0.0,0,NULL},
		{"Input sequence mode: 1=FASTA files, 2=GI list (SeqHound Redund)",NULL,NULL,NULL,FALSE,'m',ARG_INT, 0.0,0,NULL},
		{"Running mode: TRUE=build, FALSE=update",NULL, NULL,NULL, FALSE, 'b', ARG_BOOLEAN, 0.0, 0,NULL},
		{"Full path of CDD database (must have trailing slash).", NULL, NULL,NULL, FALSE, 'p', ARG_FILE_IN, 0.0, 0, NULL},
		{"Filename of CDD database.", NULL, NULL,NULL, FALSE, 'f', ARG_STRING, 0.0, 0, NULL},
		{"Root name of RPSDB to be created or table name of exsiting table (update mode).", "", NULL,NULL, FALSE, 'r', ARG_STRING, 0.0, 0, NULL},
		{"Expect value cutoff (smaller=faster).", "0.01", NULL,NULL, TRUE, 'x', ARG_FLOAT, 0.0, 0, NULL},
		{"Sequence number to start at.", "0", NULL,NULL, TRUE, 's', ARG_INT, 0.0, 0, NULL},
		{"Sequence number to end at.", "999999999", NULL,NULL, TRUE, 'e', ARG_INT, 0.0, 0, NULL}
	};
	ValNodePtr gi_list=NULL, head=NULL, gi_delete=NULL;
	Int4 totalFaaSeqs=0, start=0;
	FILE* input=NULL;
	FILE* input_del=NULL;
	Int4 gi=0, err=0;
	CharPtr gi_line;
	Char rpsDBName[PATH_MAX];
	CharPtr rootName=NULL, delete_file=NULL, token=NULL, stopstring=NULL;
	RunningMode mode=0;
	Boolean build=FALSE;

	/*time log*/
	time_t	time_now;
	CharPtr	TimeNowStr;

	/*Error loging*/
	ErrSetOptFlags(EO_LOG_SEVERITY | EO_LOG_CODES | EO_LOG_FILELINE | EO_LOG_USRFILE);
	ErrSetLogfile("rpsDB.log", ELOG_BANNER| ELOG_APPEND);
	ErrSetLogLevel(SEV_INFO);
	ErrSetMessageLevel(SEV_MAX);
	ErrSetFatalLevel(SEV_MAX);

	time_now = GetSecs();
	TimeNowStr = ctime(&time_now);
	TimeNowStr[24] = '\0';
	ErrPostEx(SEV_INFO,0,0,"Start: %s\n",IsNullStr(TimeNowStr));

	if(!GetArgs("rpsdb", DIM(myargs), myargs)) {
		ErrPostEx(SEV_ERROR, 0,0,"Main: cannot get arguments");
		return -1;
	}

	delete_file= myargs[1].strvalue;
	build = myargs[3].intvalue;
	g_cdd_path = myargs[4].strvalue;
	g_cdd_filename = myargs[5].strvalue;
	rootName = myargs[6].strvalue;
	g_expectVal = myargs[7].floatvalue;
	start = myargs[8].intvalue;
	g_end_index = myargs[9].intvalue;

	/*Check what input mode to run RPS-BLAST*/
	if(myargs[2].intvalue==1) {
		mode=eFASTA;
	}
	else if(myargs[2].intvalue==2) {
		mode=eSHoundRedund;
	}
	else {
		printf("-m option must be 1 (FASTA format) or 2 (GI list (SeqHound Redund))\n");
		ErrPostEx(SEV_ERROR,0,0,"[Main]:Running mode is invalid or not specified");
		return -1;
	}

	/*Make sure CDD path will not be too long*/
	if((strlen(g_cdd_path)+strlen(g_cdd_filename))>PATH_MAX) {
		printf("Your CDD database is in a path that is too long for this program (%ld). Put it higher in your directory tree.\n", PATH_MAX);
		ErrPostEx(SEV_ERROR,0,0,"[Main]:CDD database path longer than PATH_MAX.");
		return -1;
	}

	/*Initialize code base*/
	if(!RPS_InitCodeBase()){
		ErrPostEx(SEV_ERROR, 0,0,"Main: cannot Initialize codebase");
		return -1;
	}

	/* open RPSDB depending on running mode*/
	if(!build){
		sprintf(rpsDBName,"%s", IsNullStr(rootName));
	}else{
		sprintf(rpsDBName, "%sRPSDB", IsNullStr(rootName));
	}

	if(!OpenRPSDB(rpsDBName, build)){
		ErrPostEx(SEV_ERROR, 0,0,"Main: cannot create RPSDB");
		return -1;
	}

	g_counter=0;
	/*SeqHound GI list mode*/
	if(mode==eSHoundRedund){
		/*open seqhound- remote only!!*/
		if(!SHoundInitReadOnly(FALSE, "rpsdb")){
			ErrPostEx(SEV_ERROR, 0,0,"Main: cannot open Seqhound");
			return -1;
		}
		/*Open input file*/
		input=FileOpen(myargs[0].strvalue, "r");
		if(input==NULL)
		{
			ErrPostEx(SEV_ERROR,0,0,"Main: cannot open input file");
			return -1;
		}

		gi_line=MemNew(MAX_LINE);
		/*collect GI list to add
		* Note that if in update mode, the */
		head=gi_list;
		while(err != EOF)
		{
			err=fscanf(input,"%s", IsNullStr(gi_line));
			if(err==EOF)
				continue;
			/*Add to valNode, parse the first GI from the list */
			token=StringTok(gi_line, ",");
			if(Misc_IsNumeric(token)){
				gi=(Int4)strtol(token,&stopstring, 10);
				/*Check if GI is in RPSTABLE when running update mode*/
				if((!build) && (!IsExistInRPSDB(gi))){
					gi_list=ValNodeAddInt(&head, GI_CHOICE, gi);
				}
				else if (build)
					gi_list=ValNodeAddInt(&head, GI_CHOICE, gi);
			}else{
				ErrPostEx(SEV_ERROR,0,0,"[Main]: Line does not contain GI (%s)",IsNullStr(token));
			}
			gi_line[0]='\0';
			/*Debugging- can check the value of current ValNode here*/
		}
		gi_line=MemFree(gi_line);
		gi_list=head;

		g_monitor=NULL;
		g_rdbfp=NULL;
	}


	/*FASTA file mode, input file must be formatdb-generated*/
	else{
		/*Open input fasta file - must be formatted with formatdb*/
		g_rdbfp=readdb_new(myargs[0].strvalue, TRUE);
		if(g_rdbfp==NULL)
		{
			ErrPostEx(SEV_WARNING,0,0,"Main: cannot open fasta file. Check path and formatdb indexes.");
			return -1;
		}
		/*adjust ending index*/
		totalFaaSeqs=readdb_get_num_entries(g_rdbfp);
		totalFaaSeqs--;	/*since ordinal IDs start at 0*/
		if(g_end_index> (Uint4) totalFaaSeqs) {
			g_end_index=totalFaaSeqs;
		}

		g_monitor = MonitorIntNew("Progress", start, g_end_index);
		/*create one Valnode where to store the starting index*/
		ValNodeAddInt(&head, GI_CHOICE, start);
  		gi_list=head;
		input=NULL;
	}

	if (!build)
	  ErrPostEx(SEV_INFO,0,0,"%ld GIs to RPS-BLAST", ValNodeLen(gi_list));
	else
	  ErrPostEx(SEV_INFO,0,0,"%ld GIs to RPS-BLAST", readdb_get_num_entries(g_rdbfp));
	/* Call RPS-BLAST */
	if(RPSDatabaseBlast(gi_list, 1, mode)==SLRI_FAIL){
		ErrPostEx(SEV_ERROR, 0,0,"Main: RPSDatabaseBlast did not run or failed checking for deletions");
	}

	/*Free Gi list*/
	if(head!=NULL)
		head=ValNodeFree(head);
	if(g_monitor!=NULL)
		g_monitor=MonitorFree(g_monitor);
	if((g_rdbfp!=NULL)&&(mode==eFASTA)) {
		g_rdbfp = readdb_destruct(g_rdbfp);
	}
	if(input!=NULL)
		FileClose(input);


	/*check if the GI list to be deleted is provided*/
	if(delete_file==NULL){
		printf("No records are going to be deleted, GI list was not provided\n");
		ErrPostEx(SEV_WARNING,0,0,"[Main]: No delete file provided, will not be deleting records");
	}
	/* if running in update mode, delete the records*/
	else if(!build){
		err=0;
		input_del=FileOpen(delete_file, "r");
		if(input_del==NULL)
		{
			ErrPostEx(SEV_ERROR,0,0,"[Main]: cannot open delete file %s", IsNullStr(delete_file));
			return -1;
		}
		/*collect GI list*/
		head=gi_delete;
		gi_line=MemNew(MAX_LINE);
		while(err != EOF)
		{
			err=fscanf(input_del,"%s", IsNullStr(gi_line));
			if(err==EOF)
				continue;
			/*Add to valNode */
			gi_list=Misc_CommaIntString2ValNodeListOrdered(gi_line,",");
			gi_delete=ValNodeAddPointer(&head, GI_CHOICE, gi_list);
			gi_line[0]='\0';
			/*Debugging- can check the value of current ValNode here*/
		}
		gi_delete=head;
		gi_line=MemFree(gi_line);
		FileClose(input_del);
		/* delete records */
		if(!DeleteFromTable(gi_delete)){
			ErrPostEx(SEV_ERROR,0,0,"[Main]: Failed to remove records from RPSDB table");
		}

		/*Free the deleted GI ValNode*/
		if(head!=NULL){
			gi_delete=head;
			while(gi_delete!=NULL){
				ValNodeFree(gi_delete->data.ptrvalue);
				gi_delete=gi_delete->next;
			}
			head=ValNodeFree(head);
		}
	}

	ErrPostEx(SEV_INFO, 0,0,"Main: %ld proteins commited to table", g_counter);
	/*When using local SH close RPSDB is called internely
	* by SHoundFini. In remote SH, CloseRPSDB has to be called 
	* explicitly as well as CloseCodeBase*/
	/* CloseRPSDB();*/
	if(mode==eSHoundRedund) {
	  if (!CloseRPSDB()) {
	    ErrPostEx(SEV_INFO,0,0,"Warning:  CloseRPSDB failed\n");
	  }
	  SHoundFini();

	}
	else {
	  if (!RPS_CloseCodeBase()) {
	  ErrPostEx(SEV_INFO,0,0,"Warning:  Unable to close database, db may be in corrupted state\n");
	  }
	}
	time_now = GetSecs();
	TimeNowStr = ctime(&time_now);
	TimeNowStr[24] = '\0';
	ErrPostEx(SEV_INFO,0,0,"End: %s\n", IsNullStr(TimeNowStr));
	return 1;
}

/*CodeBase function that are needed to build table*/
/***************************************
* Codebase open and close calls
* comment out when using local SeqHound
***************************************/
/*
Boolean LIBCALL InitCodeBase(void)
{
	code4init(&cb);
	cb.safety = 1;
	error4exitTest(&cb);
	code4largeOn(&cb);
	cb.errDefaultUnique = r4unique;

	return TRUE;
}

Boolean LIBCALL CloseCodeBase(void)
{
	if ((code4close(&cb) == r4success) && (code4initUndo(&cb) == r4success))
		return TRUE;
	else
		return FALSE;
}
*/

/*
$Log: rpsdb.c,v $
Revision 1.11  2004/04/22 20:47:47  mmatan

removed c++ style comments

Revision 1.10  2004/04/14 18:03:47  mmatan

Several bug fixes which correct problems which resulted in the parser
performing invalid rpsblast searches and incorrectly recording data to
the table.  This resolves bugs 1650 and 1614.

Revision 1.9  2004/02/16 16:46:44  mmatan
Minor changes in order of shutting down codebase files.

Revision 1.8  2004/01/27 22:17:59  mmatan
Changed RPSDBSHoundRedundResultsCallback to get the domain id from the cdd->cddid field instead of attempting to parse if from the definition field.  This accomodates a change in NCBI's CDD asn.1 format

Revision 1.7  2004/01/27 21:52:37  mmatan
Increment variable "data" when AddResultsToTable fails in RPSDBResultsCallback to prevent infinite looping.  Also, SHoundInitReadOnly instead of SHoundInit

Revision 1.6  2004/01/12 17:46:29  mmatan
Changed calls to InitCodeBase and CloseCodeBase functions to calls to
RPS_InitCodeBase and RPS_CloseCodeBase.  This is necessary in order to make
these functions visible when compiling in ODBC mode.  This is being checked in
for Zhe.

Revision 1.5  2004/01/07 15:36:19  mmatan
This fixes several important bugs which are detailed in bugzilla reports
 1091, 1096 and 1074.  Briefly, this fixes: domain ID parsing errors due to
changes in the asn.1 structure for cdd records, errors when running in FASTA
mode which resulted in incorrect GIs being recorded in the rpsdb, and several
implicit function declarations which caused solaris executables to crash.

Revision 1.4  2004/01/06 22:32:14  skuznets
Code audit has been done sucessfully.
Possible NULL pointer issues has been eliminated.

Revision 1.3  2003/12/02 18:53:17  dbetel
Clean up and addition of CodeBase Init&Close

Revision 1.2  2003/11/27 20:32:34  idonalds
code4largeOn required to access files larger than 2 GB

Revision 1.1.1.11  2002/12/03 09:34:11  root
Automated import from Sourceforge.net

Revision 1.21  2002/12/02 22:25:56  betel
Small addition

Automated import from Sourceforge.net

Automated import from Sourceforge.net

Revision 1.21  2002/12/02 22:25:56  betel
Small addition

Revision 1.19  2002/11/03 23:27:46  betel
Adopted to work with SHound local

Revision 1.18  2002/10/31 15:28:16  betel
Fixed GI check

Revision 1.17  2002/10/30 18:24:56  betel
Added GI check in update mode

Revision 1.16  2002/10/30 16:53:07  betel
Commented print record

Revision 1.15  2002/10/15 23:22:54  betel
changed to accept redundant group GI's as input

Revision 1.14  2002/10/15 17:52:11  betel
Fixed errors

Revision 1.13  2002/10/13 23:06:53  betel
Fixed errors

Revision 1.12  2002/10/13 22:56:22  betel
Added delete and update functionality

Revision 1.11  2002/09/30 21:42:34  betel
Changes to results callback due to changes in RPS-BLAST and CDD

Revision 1.10  2002/09/29 01:56:46  gbader
Removed currently unneeded .acd file loading code in RPSFASTAResultsCallback - about 40% speedup

Revision 1.9  2002/09/20 12:50:22  gbader
Oops, fixed previous fix for FASTA mode - increment must occur in getseq function becuase result function only called when result exists.

Revision 1.8  2002/09/20 00:00:45  betel
Added time logging

Revision 1.7  2002/09/19 15:56:10  gbader
Fixed off by 1 error in FASTA mode for ordinal IDs.

Revision 1.6  2002/09/18 14:48:49  gbader
Fixed memory deallocation and data storage bugs for FASTA mode.

Revision 1.5  2002/09/18 02:11:05  gbader
Initial testing on windows.  Fixed bugs.

Revision 1.4  2002/09/18 01:03:46  gbader
Fixed compiler warnings and compile problems on Windows.

Revision 1.3  2002/09/17 21:02:09  betel
Added FASTA input a la BLAST (not tested)

Revision 1.2  2002/07/09 21:52:59  betel
Changed RPS-BLAST callback functions

Revision 1.1  2002/03/12 21:12:17  betel
Moved all rpsdb relevant files to rps dir

Revision 1.9  2002/03/06 20:49:36  betel
Changed log mode and removed print statements

Revision 1.8  2002/03/06 00:06:29  betel
Cdd path read from config file

Revision 1.7  2002/03/01 22:38:51  betel
Fixed memory leaks

Revision 1.5  2002/02/28 20:57:19  betel
Fixed memory problems, and process multp. Gi's from file

Revision 1.4  2002/02/27 18:43:33  betel
Added functions and fixes

Revision 1.2  2002/02/22 00:03:17  betel
Change of license. Rpsdb still not tested.

Revision 1.1  2002/02/21 23:29:31  betel
Initial import of RPS-BLAST parser. Not Tested ls

*/
