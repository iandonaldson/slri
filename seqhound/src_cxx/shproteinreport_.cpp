/* $Id: shproteinreport_.cpp,v 1.3 2003/09/29 22:17:49 haocl Exp $
 * ===========================================================================
 *
 *                            PUBLIC DOMAIN NOTICE
 *               National Center for Biotechnology Information
 *
 *  This software/database is a "United States Government Work" under the
 *  terms of the United States Copyright Act.  It was written as part of
 *  the author's official duties as a United States Government employee and
 *  thus cannot be copyrighted.  This software/database is freely available
 *  to the public for use. The National Library of Medicine and the U.S.
 *  Government have not placed any restriction on its use or reproduction.
 *
 *  Although all reasonable efforts have been taken to ensure the accuracy
 *  and reliability of the software and data, the NLM and the U.S.
 *  Government do not and cannot warrant the performance or results that
 *  may be obtained by using this software or data. The NLM and the U.S.
 *  Government disclaim all warranties, express or implied, including
 *  warranties of performance, merchantability or fitness for any particular
 *  purpose.
 *
 *  Please cite the author in any work or product based on this material.
 *
 * ===========================================================================
 *
 * File Description:
 *   This code is generated by application DATATOOL
 *   using specifications from the data definition file
 *   'slristruc.asn'.
 *
 * ATTENTION:
 *   Don't edit or check-in this file to the CVS as this file will
 *   be overridden (by DATATOOL) without warning!
 * ===========================================================================
 */

// standard includes
#include <serial/serialimpl.hpp>

// generated includes
#include <shproteinreport.hpp>
#include <FLink_set.hpp>
#include <shproteinreport.hpp>
BEGIN_shoundcgi_SCOPE // namespace shoundcgi::


// generated classes

void CSHProteinReportSetDescription_Base::ResetTitle(void)
{
    m_Title.erase();
    m_set_Title = false;
}

void CSHProteinReportSetDescription_Base::ResetQuery_report(void)
{
    m_Query_report.Reset();
}

const CSHProteinReportSetDescription_Base::TQuery_report& CSHProteinReportSetDescription_Base::GetQuery_report(void) const
{
    return (*m_Query_report);
}

void CSHProteinReportSetDescription_Base::SetQuery_report(TQuery_report& value)
{
    m_Query_report.Reset(&value);
}

CSHProteinReportSetDescription_Base::TQuery_report& CSHProteinReportSetDescription_Base::SetQuery_report(void)
{
    if ( !m_Query_report )
        m_Query_report.Reset(new shoundcgi::CSHProteinReport());
    return (*m_Query_report);
}

void CSHProteinReportSetDescription_Base::ResetRedundant_set(void)
{
    m_Redundant_set.clear();
}

void CSHProteinReportSetDescription_Base::ResetRedun_reports(void)
{
    m_Redun_reports.Reset();
}

const CSHProteinReportSetDescription_Base::TRedun_reports& CSHProteinReportSetDescription_Base::GetRedun_reports(void) const
{
    return (*m_Redun_reports);
}

void CSHProteinReportSetDescription_Base::SetRedun_reports(TRedun_reports& value)
{
    m_Redun_reports.Reset(&value);
}

CSHProteinReportSetDescription_Base::TRedun_reports& CSHProteinReportSetDescription_Base::SetRedun_reports(void)
{
    if ( !m_Redun_reports )
        m_Redun_reports.Reset(new shoundcgi::CSHProteinReportSet());
    return (*m_Redun_reports);
}

void CSHProteinReportSetDescription_Base::ResetNeighbour_set(void)
{
    m_Neighbour_set.Reset();
}

const CSHProteinReportSetDescription_Base::TNeighbour_set& CSHProteinReportSetDescription_Base::GetNeighbour_set(void) const
{
    return (*m_Neighbour_set);
}

void CSHProteinReportSetDescription_Base::SetNeighbour_set(TNeighbour_set& value)
{
    m_Neighbour_set.Reset(&value);
}

CSHProteinReportSetDescription_Base::TNeighbour_set& CSHProteinReportSetDescription_Base::SetNeighbour_set(void)
{
    if ( !m_Neighbour_set )
        m_Neighbour_set.Reset(new CFLink_set());
    return (*m_Neighbour_set);
}

void CSHProteinReportSetDescription_Base::ResetBest_report(void)
{
    m_Best_report.Reset();
}

const CSHProteinReportSetDescription_Base::TBest_report& CSHProteinReportSetDescription_Base::GetBest_report(void) const
{
    return (*m_Best_report);
}

void CSHProteinReportSetDescription_Base::SetBest_report(TBest_report& value)
{
    m_Best_report.Reset(&value);
}

CSHProteinReportSetDescription_Base::TBest_report& CSHProteinReportSetDescription_Base::SetBest_report(void)
{
    if ( !m_Best_report )
        m_Best_report.Reset(new shoundcgi::CSHProteinReport());
    return (*m_Best_report);
}

void CSHProteinReportSetDescription_Base::ResetNeighbour_reports(void)
{
    m_Neighbour_reports.Reset();
}

const CSHProteinReportSetDescription_Base::TNeighbour_reports& CSHProteinReportSetDescription_Base::GetNeighbour_reports(void) const
{
    return (*m_Neighbour_reports);
}

void CSHProteinReportSetDescription_Base::SetNeighbour_reports(TNeighbour_reports& value)
{
    m_Neighbour_reports.Reset(&value);
}

CSHProteinReportSetDescription_Base::TNeighbour_reports& CSHProteinReportSetDescription_Base::SetNeighbour_reports(void)
{
    if ( !m_Neighbour_reports )
        m_Neighbour_reports.Reset(new shoundcgi::CSHProteinReportSet());
    return (*m_Neighbour_reports);
}

void CSHProteinReportSetDescription_Base::Reset(void)
{
    ResetTitle();
    ResetTotal_reports();
    ResetQuery();
    ResetQuery_report();
    ResetRedundant_set();
    ResetRedun_reports();
    ResetNeighbour_set();
    ResetBest_report();
    ResetNeighbour_reports();
    ResetCutoff();
    ResetAcc();
    ResetDefline();
    ResetDna();
    ResetTaxid();
    ResetOmim();
    ResetMuid();
    ResetPmid();
    ResetMmdb_ids();
    ResetGo();
    ResetLlink();
    ResetRedund();
    ResetNeighbours();
}

BEGIN_NAMED_BASE_CLASS_INFO("SHProteinReportSetDescription", CSHProteinReportSetDescription)
{
    SET_CLASS_MODULE("INTREZ-databases");
    ADD_NAMED_STD_MEMBER("title", m_Title)->SetOptional(MEMBER_PTR(m_set_Title));
    ADD_NAMED_STD_MEMBER("total-reports", m_Total_reports);
    ADD_NAMED_STD_MEMBER("query", m_Query);
    ADD_NAMED_REF_MEMBER("query-report", m_Query_report, CSHProteinReport)->SetOptional();
    ADD_NAMED_MEMBER("redundant-set", m_Redundant_set, STL_list, (STD, (int)))->SetOptional();
    ADD_NAMED_REF_MEMBER("redun-reports", m_Redun_reports, CSHProteinReportSet)->SetOptional();
    ADD_NAMED_REF_MEMBER("neighbour-set", m_Neighbour_set, ::CFLink_set)->SetOptional();
    ADD_NAMED_REF_MEMBER("best-report", m_Best_report, CSHProteinReport)->SetOptional();
    ADD_NAMED_REF_MEMBER("neighbour-reports", m_Neighbour_reports, CSHProteinReportSet)->SetOptional();
    ADD_NAMED_STD_MEMBER("cutoff", m_Cutoff)->SetOptional(MEMBER_PTR(m_set_Cutoff));
    ADD_NAMED_STD_MEMBER("acc", m_Acc);
    ADD_NAMED_STD_MEMBER("defline", m_Defline);
    ADD_NAMED_STD_MEMBER("dna", m_Dna);
    ADD_NAMED_STD_MEMBER("taxid", m_Taxid);
    ADD_NAMED_STD_MEMBER("omim", m_Omim);
    ADD_NAMED_STD_MEMBER("muid", m_Muid);
    ADD_NAMED_STD_MEMBER("pmid", m_Pmid);
    ADD_NAMED_STD_MEMBER("mmdb-ids", m_Mmdb_ids);
    ADD_NAMED_STD_MEMBER("go", m_Go);
    ADD_NAMED_STD_MEMBER("llink", m_Llink);
    ADD_NAMED_STD_MEMBER("redund", m_Redund);
    ADD_NAMED_STD_MEMBER("neighbours", m_Neighbours);
    info->RandomOrder();
}
END_CLASS_INFO

// constructor
CSHProteinReportSetDescription_Base::CSHProteinReportSetDescription_Base(void)
    : m_set_Title(false), m_set_Cutoff(false), m_Total_reports(0), m_Query(0), m_Cutoff(0), m_Acc(0), m_Defline(0), m_Dna(0), m_Taxid(0), m_Omim(0), m_Muid(0), m_Pmid(0), m_Mmdb_ids(0), m_Go(0), m_Llink(0), m_Redund(0), m_Neighbours(0)
{
}

// destructor
CSHProteinReportSetDescription_Base::~CSHProteinReportSetDescription_Base(void)
{
}


void CSHProteinReportSet_Base::ResetReports(void)
{
    m_Reports.clear();
}

void CSHProteinReportSet_Base::Reset(void)
{
    ResetNumber();
    ResetReports();
}

BEGIN_NAMED_BASE_CLASS_INFO("SHProteinReportSet", CSHProteinReportSet)
{
    SET_CLASS_MODULE("INTREZ-databases");
    ADD_NAMED_STD_MEMBER("number", m_Number);
    ADD_NAMED_MEMBER("reports", m_Reports, STL_list, (STL_CRef, (CLASS, (CSHProteinReport))));
    info->RandomOrder();
}
END_CLASS_INFO

// constructor
CSHProteinReportSet_Base::CSHProteinReportSet_Base(void)
    : m_Number(0)
{
}

// destructor
CSHProteinReportSet_Base::~CSHProteinReportSet_Base(void)
{
}


void CSHProteinReport_Base::ResetAcc(void)
{
    m_Acc.erase();
    m_set_Acc = false;
}

void CSHProteinReport_Base::ResetDefline(void)
{
    m_Defline.erase();
    m_set_Defline = false;
}

void CSHProteinReport_Base::ResetMuids(void)
{
    m_Muids.clear();
}

void CSHProteinReport_Base::ResetPmids(void)
{
    m_Pmids.clear();
}

void CSHProteinReport_Base::ResetGo(void)
{
    m_Go.clear();
}

void CSHProteinReport_Base::ResetOmim(void)
{
    m_Omim.clear();
}

void CSHProteinReport_Base::ResetLlink(void)
{
    m_Llink.clear();
}

void CSHProteinReport_Base::Reset(void)
{
    ResetGi();
    ResetAcc();
    ResetDefline();
    ResetDna();
    ResetMuids();
    ResetPmids();
    ResetMmdbid();
    ResetTaxid();
    ResetGo();
    ResetOmim();
    ResetLlink();
}

BEGIN_NAMED_BASE_CLASS_INFO("SHProteinReport", CSHProteinReport)
{
    SET_CLASS_MODULE("INTREZ-databases");
    ADD_NAMED_STD_MEMBER("gi", m_Gi);
    ADD_NAMED_STD_MEMBER("acc", m_Acc)->SetOptional(MEMBER_PTR(m_set_Acc));
    ADD_NAMED_STD_MEMBER("defline", m_Defline)->SetOptional(MEMBER_PTR(m_set_Defline));
    ADD_NAMED_STD_MEMBER("dna", m_Dna)->SetOptional(MEMBER_PTR(m_set_Dna));
    ADD_NAMED_MEMBER("muids", m_Muids, STL_list, (STD, (int)))->SetOptional();
    ADD_NAMED_MEMBER("pmids", m_Pmids, STL_list, (STD, (int)))->SetOptional();
    ADD_NAMED_STD_MEMBER("mmdbid", m_Mmdbid)->SetOptional(MEMBER_PTR(m_set_Mmdbid));
    ADD_NAMED_STD_MEMBER("taxid", m_Taxid)->SetOptional(MEMBER_PTR(m_set_Taxid));
    ADD_NAMED_MEMBER("go", m_Go, STL_list, (STL_CRef, (CLASS, (CSHGoReport))))->SetOptional();
    ADD_NAMED_MEMBER("omim", m_Omim, STL_list, (STD, (int)))->SetOptional();
    ADD_NAMED_MEMBER("llink", m_Llink, STL_list, (STL_CRef, (CLASS, (CSHLocusLinkReport))))->SetOptional();
    info->RandomOrder();
}
END_CLASS_INFO

// constructor
CSHProteinReport_Base::CSHProteinReport_Base(void)
    : m_set_Acc(false), m_set_Defline(false), m_set_Dna(false), m_set_Mmdbid(false), m_set_Taxid(false), m_Gi(0), m_Dna(0), m_Mmdbid(0), m_Taxid(0)
{
}

// destructor
CSHProteinReport_Base::~CSHProteinReport_Base(void)
{
}


void CSHNeighbour_Base::Reset(void)
{
    ResetGi();
    ResetEvalue();
}

BEGIN_NAMED_BASE_CLASS_INFO("SHNeighbour", CSHNeighbour)
{
    SET_CLASS_MODULE("INTREZ-databases");
    ADD_NAMED_STD_MEMBER("gi", m_Gi);
    ADD_NAMED_STD_MEMBER("evalue", m_Evalue);
    info->RandomOrder();
}
END_CLASS_INFO

// constructor
CSHNeighbour_Base::CSHNeighbour_Base(void)
    : m_Gi(0), m_Evalue(0)
{
}

// destructor
CSHNeighbour_Base::~CSHNeighbour_Base(void)
{
}


void CSHLocusProduct_Base::ResetDefline(void)
{
    m_Defline.erase();
    m_set_Defline = false;
}

void CSHLocusProduct_Base::Reset(void)
{
    ResetGi();
    ResetDefline();
}

BEGIN_NAMED_BASE_CLASS_INFO("SHLocusProduct", CSHLocusProduct)
{
    SET_CLASS_MODULE("INTREZ-databases");
    ADD_NAMED_STD_MEMBER("gi", m_Gi);
    ADD_NAMED_STD_MEMBER("defline", m_Defline)->SetOptional(MEMBER_PTR(m_set_Defline));
    info->RandomOrder();
}
END_CLASS_INFO

// constructor
CSHLocusProduct_Base::CSHLocusProduct_Base(void)
    : m_set_Defline(false), m_Gi(0)
{
}

// destructor
CSHLocusProduct_Base::~CSHLocusProduct_Base(void)
{
}


void CSHLocusLinkReport_Base::ResetLocus(void)
{
    m_Locus.erase();
    m_set_Locus = false;
}

void CSHLocusLinkReport_Base::ResetProducts(void)
{
    m_Products.clear();
}

void CSHLocusLinkReport_Base::Reset(void)
{
    ResetLlid();
    ResetLocus();
    ResetProducts();
}

BEGIN_NAMED_BASE_CLASS_INFO("SHLocusLinkReport", CSHLocusLinkReport)
{
    SET_CLASS_MODULE("INTREZ-databases");
    ADD_NAMED_STD_MEMBER("llid", m_Llid)->SetOptional(MEMBER_PTR(m_set_Llid));
    ADD_NAMED_STD_MEMBER("locus", m_Locus)->SetOptional(MEMBER_PTR(m_set_Locus));
    ADD_NAMED_MEMBER("products", m_Products, STL_list, (STL_CRef, (CLASS, (CSHLocusProduct))))->SetOptional();
    info->RandomOrder();
}
END_CLASS_INFO

// constructor
CSHLocusLinkReport_Base::CSHLocusLinkReport_Base(void)
    : m_set_Llid(false), m_set_Locus(false), m_Llid(0)
{
}

// destructor
CSHLocusLinkReport_Base::~CSHLocusLinkReport_Base(void)
{
}


void CSHGoReport_Base::ResetMol_fun(void)
{
    m_Mol_fun.Reset();
}

const CSHGoReport_Base::TMol_fun& CSHGoReport_Base::GetMol_fun(void) const
{
    return (*m_Mol_fun);
}

void CSHGoReport_Base::SetMol_fun(TMol_fun& value)
{
    m_Mol_fun.Reset(&value);
}

CSHGoReport_Base::TMol_fun& CSHGoReport_Base::SetMol_fun(void)
{
    if ( !m_Mol_fun )
        m_Mol_fun.Reset(new shoundcgi::CSHGoAnnot());
    return (*m_Mol_fun);
}

void CSHGoReport_Base::ResetBio_pro(void)
{
    m_Bio_pro.Reset();
}

const CSHGoReport_Base::TBio_pro& CSHGoReport_Base::GetBio_pro(void) const
{
    return (*m_Bio_pro);
}

void CSHGoReport_Base::SetBio_pro(TBio_pro& value)
{
    m_Bio_pro.Reset(&value);
}

CSHGoReport_Base::TBio_pro& CSHGoReport_Base::SetBio_pro(void)
{
    if ( !m_Bio_pro )
        m_Bio_pro.Reset(new shoundcgi::CSHGoAnnot());
    return (*m_Bio_pro);
}

void CSHGoReport_Base::ResetCell_com(void)
{
    m_Cell_com.Reset();
}

const CSHGoReport_Base::TCell_com& CSHGoReport_Base::GetCell_com(void) const
{
    return (*m_Cell_com);
}

void CSHGoReport_Base::SetCell_com(TCell_com& value)
{
    m_Cell_com.Reset(&value);
}

CSHGoReport_Base::TCell_com& CSHGoReport_Base::SetCell_com(void)
{
    if ( !m_Cell_com )
        m_Cell_com.Reset(new shoundcgi::CSHGoAnnot());
    return (*m_Cell_com);
}

void CSHGoReport_Base::Reset(void)
{
    ResetNum_mol_fun();
    ResetNum_bio_pro();
    ResetNum_cell_com();
    ResetMol_fun();
    ResetBio_pro();
    ResetCell_com();
}

BEGIN_NAMED_BASE_CLASS_INFO("SHGoReport", CSHGoReport)
{
    SET_CLASS_MODULE("INTREZ-databases");
    ADD_NAMED_STD_MEMBER("num-mol-fun", m_Num_mol_fun);
    ADD_NAMED_STD_MEMBER("num-bio-pro", m_Num_bio_pro);
    ADD_NAMED_STD_MEMBER("num-cell-com", m_Num_cell_com);
    ADD_NAMED_REF_MEMBER("mol-fun", m_Mol_fun, CSHGoAnnot)->SetOptional();
    ADD_NAMED_REF_MEMBER("bio-pro", m_Bio_pro, CSHGoAnnot)->SetOptional();
    ADD_NAMED_REF_MEMBER("cell-com", m_Cell_com, CSHGoAnnot)->SetOptional();
    info->RandomOrder();
}
END_CLASS_INFO

// constructor
CSHGoReport_Base::CSHGoReport_Base(void)
    : m_Num_mol_fun(0), m_Num_bio_pro(0), m_Num_cell_com(0)
{
}

// destructor
CSHGoReport_Base::~CSHGoReport_Base(void)
{
}


void CSHGoAnnotNode_Base::ResetOntology(void)
{
    m_Ontology.erase();
}

void CSHGoAnnotNode_Base::ResetGoname(void)
{
    m_Goname.erase();
}

void CSHGoAnnotNode_Base::ResetEvidence_code(void)
{
    m_Evidence_code.clear();
}

void CSHGoAnnotNode_Base::ResetPmid(void)
{
    m_Pmid.clear();
}

void CSHGoAnnotNode_Base::Reset(void)
{
    ResetGoid();
    ResetOntology();
    ResetGoname();
    ResetEvidence_code();
    ResetPmid();
}

BEGIN_NAMED_BASE_CLASS_INFO("SHGoAnnotNode", CSHGoAnnotNode)
{
    SET_CLASS_MODULE("INTREZ-databases");
    ADD_NAMED_STD_MEMBER("goid", m_Goid);
    ADD_NAMED_STD_MEMBER("ontology", m_Ontology);
    ADD_NAMED_STD_MEMBER("goname", m_Goname);
    ADD_NAMED_MEMBER("evidence-code", m_Evidence_code, STL_list, (STD, (NCBI_NS_STD::string)));
    ADD_NAMED_MEMBER("pmid", m_Pmid, STL_list, (STD, (int)));
    info->RandomOrder();
}
END_CLASS_INFO

// constructor
CSHGoAnnotNode_Base::CSHGoAnnotNode_Base(void)
    : m_Goid(0)
{
}

// destructor
CSHGoAnnotNode_Base::~CSHGoAnnotNode_Base(void)
{
}


void CSHGoAnnot_Base::Reset(void)
{
    m_data.clear();
}

BEGIN_NAMED_BASE_IMPLICIT_CLASS_INFO("SHGoAnnot", CSHGoAnnot)
{
    SET_CLASS_MODULE("INTREZ-databases");
    ADD_NAMED_MEMBER("", m_data, STL_list, (STL_CRef, (CLASS, (CSHGoAnnotNode))));
    info->RandomOrder();
}
END_CLASS_INFO

// constructor
CSHGoAnnot_Base::CSHGoAnnot_Base(void)
{
}

// destructor
CSHGoAnnot_Base::~CSHGoAnnot_Base(void)
{
}



END_shoundcgi_SCOPE // namespace shoundcgi::

