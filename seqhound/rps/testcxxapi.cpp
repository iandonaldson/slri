#include <corelib/ncbiargs.hpp>
#include <corelib/ncbiapp.hpp>
#include <corelib/ncbireg.hpp>
#include <corelib/ncbienv.hpp>
#include "rpsdbapi.hpp"
#include "slri_misc.hpp"



BEGIN_NCBI_SCOPE
/*Test class to check API functions */
class CTestApi : public CNcbiApplication {
	private:
	virtual void Init();
	virtual int Run();
};
END_NCBI_SCOPE

USING_NCBI_SCOPE;
using namespace slri;

/*Check Domaname API calls */
void CheckDOMNAME(string acc, string label)
{
	/*check for GetDomainLabelFromDomainId*/
	cout<<"Calling GetDomainLabelFromDomainId("<<acc<<")\n";
	ERR_POST(Info<<"Calling GetDomainLabelFromDomainId("<<acc<<")");
	string s=GetDomainLabelFromDomainId(acc);
	cout<<"Domain "<<acc<<" is "<<s<<endl;
	ERR_POST(Info<<"Domain "<<acc<<"is "<< s);

	/*check for Get GetDomainIdFromName*/
	cout<<"Calling GetDomainIdFromName("<<label<<")\n";
	ERR_POST(Info<<"Calling GetDomainIdFromName("<<label<<")");
	list<string>* accessions=GetDomainIdFromName(label);
	cout<<"Domain "<<label<<" is "<<SLRIMisc::PrintList(*accessions)<<endl;
	ERR_POST(Info<<"Domain "<<label<<" is "<<SLRIMisc::PrintList(*accessions));

	/*calling GetGIsByDomainLabel */
	cout<<"calling GetGIsByDomainLabel("<<label<<")\n";
	ERR_POST(Info<<"Calling GetGIsByDomainLabel("<<label<<")");
	list<int>* GIs=GetGIsByDomainLabel(label);
	ERR_POST(Info<<"Printed "<<SLRIMisc::PrintList(*GIs)<<" GI's");

	return;
}

/* Checks RPSDB API functions */
void CheckRPSDB(int gi, string domid, double cutoff)
{
	/*Check GetDomainsFromGi()*/
	cout<<"Calling GetDomainsFromGi("<<gi<<")\n";
	ERR_POST(Info<<"Calling GetDomainsFromGi("<<gi<<")");
	list<RpsDBRecord*>* dom_list=GetDomainsFromGi(gi);
	if(dom_list==NULL || dom_list->empty()){
		cout<<gi<<" is not in RPSDB\n";
		ERR_POST("[CheckRPSDB]: "<<gi<<" is not in RPSDB");
		return;
	}
	ERR_POST(Info<< "Printed "<< SLRIMisc::PrintList(*dom_list)<<" records");
	delete dom_list;

	cout<<"Calling GetDomainsFromGiWithEvalue("<<gi<<", "<<cutoff<<")\n";
	ERR_POST(Info<<"Calling GetDomainsFromGiWithEvalue("<<gi<<", "<<cutoff<<")");

	/*Check GetDomainsFromGiWithEvalue()*/
	dom_list=GetDomainsFromGiWithEvalue(gi, cutoff);
	ERR_POST(Info<<"Printed "<<SLRIMisc::PrintList(*dom_list)<<" records");
	delete dom_list;

	cout<<"Calling GetDomainIDsFromGiWithEvalue("<<gi<<", "<<cutoff<<")\n";
	ERR_POST(Info<<"Calling GetDomainIDsFromGiWithEvalue("<<gi<<", "<<cutoff<<")");
	list<string>* domain_ids=GetDomainIDsFromGiWithEvalue(gi,cutoff);
	ERR_POST(Info<<"Printed "<<SLRIMisc::PrintList(*domain_ids)<<" records");
	if(domain_ids!=NULL)
		delete domain_ids;

	/*Check GetGisFromDomainId()*/
	cout<<"Calling GetGisFromDomainId("<<domid<<")\n";
	ERR_POST(Info<<"Calling GetGisFromDomainId("<<domid<<")");
	list<int >* Gilist=GetGisFromDomainId(domid);
	if(Gilist==NULL){
		ERR_POST(Warning<<"[CheckRPSDB] GetGisFromDomainId failed for "<<domid);
	}
	else{
		ERR_POST(Info<<"Size of GI list "<<Gilist->size());
		ERR_POST(Info<<"Printed "<<SLRIMisc::PrintList(*Gilist)<<" records");
		//FreeList(*Gilist);
		delete Gilist;
	}

	/*Check GetGisFromDomainId()*/
	cout<<"Calling GetGisFromDomainId("<<domid<<","<<cutoff<<")\n";
	ERR_POST(Info<<"Calling GetGisFromDomainId("<<domid<<","<<cutoff<<")");
	Gilist=GetGisFromDomainId(domid, cutoff);
	if(Gilist==NULL){
		ERR_POST(Warning<<"[CheckRPSDB] GetGisFromDomainId failed");
	}
	else{
		ERR_POST(Info<<"Printed "<<SLRIMisc::PrintList(*Gilist)<<" records");
		delete Gilist;
	}
	return;
}

void CTestApi::Init() {

	SetDiagPostLevel(eDiag_Info);
	SetDiagDieLevel(eDiag_Fatal);
	SetDiagTrace(eDT_Disable);
	SetThrowTraceAbort(false);

	auto_ptr<CArgDescriptions> argdesc(new CArgDescriptions);
	argdesc->SetUsageContext(GetArguments().GetProgramBasename(),
		"SeqHound RPSDB & DOMNAME table C++ API testing program");

	argdesc->AddKey("g","GeneIdentifier",
		"Map domains to this protein",CArgDescriptions::eInteger);
	argdesc->AddKey("d","DomainIdentifier",
		"Domain Identifier (Pfam or Smart)",CArgDescriptions::eString);
	argdesc->AddDefaultKey("n", "DomainName",
		"Domain label (e.g. PTB, SH3, AAA, ...)",CArgDescriptions::eString, "SH2");
	argdesc->AddDefaultKey("e","EvalueCutoff",
		"Evalue cutoff",CArgDescriptions::eDouble, NStr::DoubleToString(0.001));
	CArgAllow* constraint = new CArgAllow_Doubles(1.0, 10e-30);
	argdesc->SetConstraint("e", constraint);
	SetupArgDescriptions(argdesc.release());
}

/*TestApi Application */
int CTestApi::Run() {

	CArgs args;

	args = GetArgs();
	/*Set diag stream after getting arguments*/
	CNcbiOfstream diag("cppapitest.log");
	SetDiagStream(&diag, true);


	int gi =(args)["g"].AsInteger();
	string domain=(args)["d"].AsString();
	double cutoff=(args)["e"].AsDouble();
	string label=(args)["n"].AsString();

	/* Read path from config file */
 	CNcbiIfstream config_file(".intrezrc");
	CNcbiRegistry reg(config_file,CNcbiRegistry::eTransient);
	string database=reg.Get("datab","pathrps");
	string rpsTable=database;
	rpsTable +="MASTER";
	string domnameTable =database;
	domnameTable +="DOMNAME";

	/* initialize tables */
	InitRPSDB(rpsTable);
	InitDOMNAME(domnameTable);

	CheckRPSDB(gi,domain, cutoff);
	/*CheckDOMNAME(domain, label);*/

	CloseTables();

	cout<<" More information in log file\n";
	return 0;
}

int main(int argc, const char* argv[])
{

	CTestApi test;
	return test.AppMain(argc, argv);
}
