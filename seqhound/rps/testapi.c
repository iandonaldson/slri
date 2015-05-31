#include <ncbi.h>
#include <intrez_db.h>
#include <rpsdbapi.h>
#include <assert.h>
#include <d4all.h>
CODE4 cb;
/*recursive mod
Precondition- m>0
PostCondition- returns x mod m */
Int4 Mod(Int4 x, Int4 m)
{
	assert(m>0);
	/*
	if(x>m)
		return Mod(x-m,m);
	else
		return x;
	*/
	return x-( (x/m)*m);
}


void PrintIntValNode(ValNodePtr head)
{
	Int4 count=0;
	while(head!=NULL)
	{
		printf("GI: %d\t",head->data.intvalue);
		head=head->next;
		count++;
		if(Mod(count, 5)==0)
			printf("\n");

	}
	ErrPostEx(SEV_INFO,0,0,"Printed %d records.",count);
	printf("\n");
}

void PrintRecord(StRpsdbPtr results)
{
	Int4 count=0;
	while(results!=NULL){
		fprintf(stdout, "%ld: %s: %ld: %e: %ld: %ld: %ld: %ld:\n",
				(long)results->gi,(char*)IsNullStr(results->dom_id),(long)results->from,
					(double)results->evalue,(long)results->c_missing, (long)results->n_missing,
							(long)results->len, (long)results->num_of_dom);
		results=results->next;
		count++;
	}
	ErrPostEx(SEV_INFO,0,0,"Printed %d records.",count);
}

/*************************************
* Codebase open and close calls
**************************************/
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


/* Check DOMNAME calls */
void CheckDOMNAME(CharPtr id, CharPtr label)
{
	ValNodePtr results=NULL;
	ValNodePtr head=NULL;
	CharPtr name=NULL;

	/*checking SHoundGetDomainLabelFromDomainId */
	printf("Calling SHoundGetDomainLabelFromDomainId() with %s\n",IsNullStr(id));
	ErrPostEx(SEV_INFO, 0,0,"Calling SHoundGetDomainLabelFromDomainId() with %s\n",IsNullStr(id));
	name=SHoundGetDomainLabelFromDomainId(id);
	if(name!=NULL){
		printf("%s is %s\n", IsNullStr(id),IsNullStr(name));
		ErrPostEx(SEV_INFO, 0,0,"%s is %s", IsNullStr(id),IsNullStr(name));
	}
	else
		printf("No label found for %s\n",IsNullStr(id));

	/*checking SHoundGetDomainIdFromLabel */
	printf("Calling SHoundGetDomainIdFromLabel() with %s\n",IsNullStr(label));
	ErrPostEx(SEV_INFO, 0,0,"Calling SHoundGetDomainIdFromLabel() with %s\n",IsNullStr(label));
	results=SHoundGetDomainIdFromLabel(label);
	head=results;
	while(results!=NULL)
	{
		printf("%s is %s\n", IsNullStr(label), (char*)IsNullStr(results->data.ptrvalue));
		ErrPostEx(SEV_INFO, 0,0,"%s is %s\n", IsNullStr(label), (char*)IsNullStr(results->data.ptrvalue));
		results=results->next;
	}
	/*if(name!=NULL)
		printf("%s is %s\n", IsNullStr(label), IsNullStr(name));
	else
		printf("No id found for %s\n",IsNullStr(label));*/
	results=ValNodeFree(head);
	/*Checking SHoundGetGisByDomainLabel*/
	printf("Calling SHoundGetGisByDomainLabel() with %s\n",IsNullStr(label));
	ErrPostEx(SEV_INFO, 0,0,"Calling SHoundGetGisByDomainLabel() with %s\n",IsNullStr(label));
	results=SHoundGetGisByDomainLabel(label);
	PrintIntValNode(results);
	results=ValNodeFree(results);

	/*Checking SHoundGetGisByDomainLabel*/
	printf("Calling SHoundGetGisByDomainLabelAndEvalue() with %s\n",IsNullStr(label));
	ErrPostEx(SEV_INFO, 0,0,"Calling SHoundGetGisByDomainLabelAndEvalue() with %s\n",IsNullStr(label));
	results=SHoundGetGisByDomainLabelAndEvalue(label, 0.001);
	PrintIntValNode(results);
	results=ValNodeFree(results);
	return;
}
/*Chaeck RPSDB calls*/
void CheckRPSDB(Int4 gi, CharPtr domid, Int4 num_of_domains)
{
	ValNodePtr results=NULL;
	StRpsdbPtr head=NULL;
	FloatHi e=0.001;

	/*Check Domains */
	printf("Calling SHoundGetDomainsFromGi() with %d\n",gi);
	ErrPostEx(SEV_INFO, 0,0,"Calling SHoundGetDomainsFromGi() with %d\n",gi);
	head=SHoundGetDomainsFromGi(gi);
	if(head==NULL){
		ErrPostEx(SEV_ERROR, 0,0,"Main: SHoundGetDomainsFromGi failed");
	}
	PrintRecord(head);
	/*Free structure *
	StRpsdbFree(head);
	/*Check for Domains by evalue */
	printf("Calling SHoundGetDomainsFromGiWithEvalue with %d, %e\n",gi, e);
	ErrPostEx(SEV_INFO, 0,0,"Calling SHoundGetDomainsFromGiWithEvalue with %d, %e\n",gi, e);
	head=SHoundGetDomainsFromGiWithEvalue(gi, e);
	if(head==NULL){
		ErrPostEx(SEV_FATAL, 0,0,"Main: SHoundGetDomainsFromGi failed");
	}
	PrintRecord(head);
	StRpsdbFree(head);

	/*Check for Gis by domain id */
	printf("Calling SHoundGetGisByDomainId with %s\n", IsNullStr(domid));
	ErrPostEx(SEV_INFO, 0,0,"Calling SHoundGetGisByDomainId with %s\n", IsNullStr(domid));
	results=SHoundGetGisByDomainId(domid);
	if(results==NULL)
	{
		ErrPostEx(SEV_FATAL,0,0,"Main:SHoundGetGisByDomainId failed ");
	}
	PrintIntValNode(results);
	results=ValNodeFree(results);

	/*Check for Gis by domain id+ evalue cutoff */
	printf("Calling SHoundGetGisByDomainIdAndEvalue with %s and %e\n", IsNullStr(domid), e);
	ErrPostEx(SEV_INFO, 0,0,"Calling SHoundGetGisByDomainIdAndEvalue with %s and %e\n", IsNullStr(domid), e);
	results=SHoundGetGisByDomainIdAndEvalue(domid, e);
	if(results==NULL)
	{
		ErrPostEx(SEV_FATAL,0,0,"Main:SHoundGetGisByDomainId failed ");
	}
	PrintIntValNode(results);
	results=ValNodeFree(results);

	/*Check for Gis by number of domains */
	printf("Calling SHoundGetGisByNumberOfDomains with %d domains.\n", num_of_domains);
	results=SHoundGetGisByNumberOfDomains(num_of_domains);
	if(results==NULL)
	{
		ErrPostEx(SEV_FATAL,0,0,"Main:SHoundGetGisBynumberOfDomains failed ");
	}
	PrintIntValNode(results);
	results=ValNodeFree(results);
	return;
}

#define NUMARGS 3
Int2 Main(){

	Args myargs[NUMARGS]={
		{"gi", NULL, NULL,NULL, FALSE, 'g', ARG_INT, 0.0, 0, NULL},
		{"Dom-id", NULL, NULL,NULL, FALSE, 'd', ARG_STRING, 0.0, 0, NULL},
		{"Dom name", NULL, NULL,NULL, FALSE, 'n', ARG_STRING, 0.0, 0, NULL},
	};

	Int4 gi=0;
	CharPtr domid=NULL;
	Int4 num_of_domains=0;
	CharPtr label=NULL;

	ErrSetOptFlags(EO_LOG_SEVERITY | EO_LOG_CODES | EO_LOG_FILELINE | EO_LOG_USRFILE);
	ErrSetLogfile("testapi.log", ELOG_BANNER| ELOG_APPEND);
	ErrSetLogLevel(SEV_INFO);
	ErrSetMessageLevel(SEV_MAX);

	if(!GetArgs("testapi", NUMARGS, myargs)) {
		ErrPostEx(SEV_FATAL, 0,0,"Main: cannot get arguments");
		return -1;
	}


    /*if(!SHoundInit(FALSE, "testapi"))
    {
        ErrPostEx(SEV_FATAL, 0,0,"Main: cannot open SHound.");
		return -1;
	}*/

	/* Initialize codebase locally */
	code4init(&cb);
	cb.errOpen=0;
	cb.safety=0;

	/*open rpsdb using the Table name*/
    InitCodeBase();
	if(!InitRPSDB("MASTER")){
		ErrPostEx(SEV_FATAL, 0,0,"Main: cannot open RPSDB");
		return -1;
	}

	if(!InitDOMNAME("DOMNAME"))
	{
		ErrPostEx(SEV_FATAL,0,0,"[Main]: cannot open DOMNAME");
		return -1;
	}

	gi=myargs[0].intvalue;
	domid=myargs[1].strvalue;
	num_of_domains=30;
	label=myargs[2].strvalue;

	/*Check RPSDB api */
	CheckRPSDB(gi,domid, num_of_domains);

	/*CheckDOMNAME(domid, label);*/

	/*close and exit*/
	if(!FiniRPSDB() ){
		ErrPostEx(SEV_FATAL, 0,0,"Main: can't close RPSDB");
		return -1;
	}
	if(!FiniDOMNAME() ){
		ErrPostEx(SEV_FATAL, 0,0,"Main: can't close DOMNAME");
		return -1;
	}
	CloseCodeBase();
	/*SHoundFini();*/

	/* close code base */
	code4close(&cb);
	code4initUndo(&cb);
	printf("More information is available in the log file\n");
	return 1;
}
