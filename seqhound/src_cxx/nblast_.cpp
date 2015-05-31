/* $Id: nblast_.cpp,v 1.1.1.1 2003/02/07 09:32:34 root Exp $
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
 *   'NBlastasn.asn'.
 *
 * ATTENTION:
 *   Don't edit or check-in this file to the CVS as this file will
 *   be overridden (by DATATOOL) without warning!
 * ===========================================================================
 */

// standard includes
#include <serial/serialimpl.hpp>

// generated includes
#include <nblast.hpp>
#include <nblast.hpp>
BEGIN_seqhound_SCOPE // namespace seqhound::


// generated classes

void CNBlast_Result_Set_Base::ResetResults(void)
{
    m_Results.clear();
}

void CNBlast_Result_Set_Base::Reset(void)
{
    ResetQuery_gi();
    ResetSubject_gi();
    ResetResults();
}

BEGIN_NAMED_BASE_CLASS_INFO("NBlast-Result-Set", CNBlast_Result_Set)
{
    SET_CLASS_MODULE("NBlast");
    ADD_NAMED_STD_MEMBER("query-gi", m_Query_gi)->SetOptional(MEMBER_PTR(m_set_Query_gi));
    ADD_NAMED_STD_MEMBER("subject-gi", m_Subject_gi)->SetOptional(MEMBER_PTR(m_set_Subject_gi));
    ADD_NAMED_MEMBER("results", m_Results, STL_list, (STL_CRef, (CLASS, (CNBlast_Result))));
    info->RandomOrder();
}
END_CLASS_INFO

// constructor
CNBlast_Result_Set_Base::CNBlast_Result_Set_Base(void)
    : m_set_Query_gi(false), m_set_Subject_gi(false), m_Query_gi(0), m_Subject_gi(0)
{
}

// destructor
CNBlast_Result_Set_Base::~CNBlast_Result_Set_Base(void)
{
}


void CNBlast_Scores_Base::Reset(void)
{
    ResetBitscore();
    ResetEvalue();
}

BEGIN_NAMED_BASE_CLASS_INFO("NBlast-Scores", CNBlast_Scores)
{
    SET_CLASS_MODULE("NBlast");
    ADD_NAMED_STD_MEMBER("bitscore", m_Bitscore);
    ADD_NAMED_STD_MEMBER("evalue", m_Evalue);
    info->RandomOrder();
}
END_CLASS_INFO

// constructor
CNBlast_Scores_Base::CNBlast_Scores_Base(void)
    : m_Bitscore(0), m_Evalue(0)
{
}

// destructor
CNBlast_Scores_Base::~CNBlast_Scores_Base(void)
{
}


void CNBlast_Result_Base::ResetStarts(void)
{
    m_Starts.clear();
}

void CNBlast_Result_Base::ResetLens(void)
{
    m_Lens.clear();
}

void CNBlast_Result_Base::ResetScores(void)
{
    m_Scores.clear();
}

void CNBlast_Result_Base::Reset(void)
{
    ResetNumseg();
    ResetStarts();
    ResetLens();
    ResetScores();
}

BEGIN_NAMED_BASE_CLASS_INFO("NBlast-Result", CNBlast_Result)
{
    SET_CLASS_MODULE("NBlast");
    ADD_NAMED_STD_MEMBER("numseg", m_Numseg);
    ADD_NAMED_MEMBER("starts", m_Starts, STL_list, (STD, (int)));
    ADD_NAMED_MEMBER("lens", m_Lens, STL_list, (STD, (int)));
    ADD_NAMED_MEMBER("scores", m_Scores, STL_list, (STL_CRef, (CLASS, (CNBlast_Scores))));
    info->RandomOrder();
}
END_CLASS_INFO

// constructor
CNBlast_Result_Base::CNBlast_Result_Base(void)
    : m_Numseg(0)
{
}

// destructor
CNBlast_Result_Base::~CNBlast_Result_Base(void)
{
}


void CNBlast_GiAndEval_set_Base::ResetGiandevals(void)
{
    m_Giandevals.clear();
}

void CNBlast_GiAndEval_set_Base::Reset(void)
{
    ResetQuery_gi();
    ResetGiandevals();
}

BEGIN_NAMED_BASE_CLASS_INFO("NBlast-GiAndEval-set", CNBlast_GiAndEval_set)
{
    SET_CLASS_MODULE("NBlast");
    ADD_NAMED_STD_MEMBER("query-gi", m_Query_gi)->SetOptional(MEMBER_PTR(m_set_Query_gi));
    ADD_NAMED_MEMBER("giandevals", m_Giandevals, STL_list, (STL_CRef, (CLASS, (CNBlast_GiAndEval))));
    info->RandomOrder();
}
END_CLASS_INFO

// constructor
CNBlast_GiAndEval_set_Base::CNBlast_GiAndEval_set_Base(void)
    : m_set_Query_gi(false), m_Query_gi(0)
{
}

// destructor
CNBlast_GiAndEval_set_Base::~CNBlast_GiAndEval_set_Base(void)
{
}


void CNBlast_GiAndEval_Base::Reset(void)
{
    ResetSubject_gi();
    ResetEvalue();
}

BEGIN_NAMED_BASE_CLASS_INFO("NBlast-GiAndEval", CNBlast_GiAndEval)
{
    SET_CLASS_MODULE("NBlast");
    ADD_NAMED_STD_MEMBER("subject-gi", m_Subject_gi);
    ADD_NAMED_STD_MEMBER("evalue", m_Evalue);
    info->RandomOrder();
}
END_CLASS_INFO

// constructor
CNBlast_GiAndEval_Base::CNBlast_GiAndEval_Base(void)
    : m_Subject_gi(0), m_Evalue(0)
{
}

// destructor
CNBlast_GiAndEval_Base::~CNBlast_GiAndEval_Base(void)
{
}



END_seqhound_SCOPE // namespace seqhound::

