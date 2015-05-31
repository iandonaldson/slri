/* $Id: nblast_.hpp,v 1.1.1.2 2003/03/07 09:32:59 root Exp $
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

#ifndef NBLAST_BASE_HPP
#define NBLAST_BASE_HPP

/* standard includes */
#include <serial/serialbase.hpp>

/* generated includes */
#include <list>

#ifndef BEGIN_seqhound_SCOPE
#  define BEGIN_seqhound_SCOPE BEGIN_SCOPE(seqhound)
#  define END_seqhound_SCOPE END_SCOPE(seqhound)
#endif
BEGIN_seqhound_SCOPE // namespace seqhound::


// forward declarations
class CNBlast_GiAndEval;
class CNBlast_Result;
class CNBlast_Scores;


// generated classes

class CNBlast_Result_Set_Base : public NCBI_NS_NCBI::CSerialObject
{
    typedef NCBI_NS_NCBI::CSerialObject Tparent;
public:
    // constructor
    CNBlast_Result_Set_Base(void);
    // destructor
    virtual ~CNBlast_Result_Set_Base(void);

    // type info
    DECLARE_INTERNAL_TYPE_INFO();

    // members' types
    typedef int TQuery_gi;
    typedef int TSubject_gi;
    typedef NCBI_NS_STD::list< NCBI_NS_NCBI::CRef< CNBlast_Result > > TResults;

    // members' getters
    // members' setters
    bool IsSetQuery_gi(void) const;
    void ResetQuery_gi(void);
    const int& GetQuery_gi(void) const;
    void SetQuery_gi(const int& value);
    int& SetQuery_gi(void);

    bool IsSetSubject_gi(void) const;
    void ResetSubject_gi(void);
    const int& GetSubject_gi(void) const;
    void SetSubject_gi(const int& value);
    int& SetSubject_gi(void);

    void ResetResults(void);
    const NCBI_NS_STD::list< NCBI_NS_NCBI::CRef< CNBlast_Result > >& GetResults(void) const;
    NCBI_NS_STD::list< NCBI_NS_NCBI::CRef< CNBlast_Result > >& SetResults(void);

    // reset whole object
    virtual void Reset(void);


private:
    // Prohibit copy constructor and assignment operator
    CNBlast_Result_Set_Base(const CNBlast_Result_Set_Base&);
    CNBlast_Result_Set_Base& operator=(const CNBlast_Result_Set_Base&);

    // members' data
    bool m_set_Query_gi;
    bool m_set_Subject_gi;
    TQuery_gi m_Query_gi;
    TSubject_gi m_Subject_gi;
    TResults m_Results;
};
class CNBlast_Scores_Base : public NCBI_NS_NCBI::CSerialObject
{
    typedef NCBI_NS_NCBI::CSerialObject Tparent;
public:
    // constructor
    CNBlast_Scores_Base(void);
    // destructor
    virtual ~CNBlast_Scores_Base(void);

    // type info
    DECLARE_INTERNAL_TYPE_INFO();

    // members' types
    typedef double TBitscore;
    typedef double TEvalue;

    // members' getters
    // members' setters
    void ResetBitscore(void);
    const double& GetBitscore(void) const;
    void SetBitscore(const double& value);
    double& SetBitscore(void);

    void ResetEvalue(void);
    const double& GetEvalue(void) const;
    void SetEvalue(const double& value);
    double& SetEvalue(void);

    // reset whole object
    virtual void Reset(void);


private:
    // Prohibit copy constructor and assignment operator
    CNBlast_Scores_Base(const CNBlast_Scores_Base&);
    CNBlast_Scores_Base& operator=(const CNBlast_Scores_Base&);

    // members' data
    TBitscore m_Bitscore;
    TEvalue m_Evalue;
};
class CNBlast_Result_Base : public NCBI_NS_NCBI::CSerialObject
{
    typedef NCBI_NS_NCBI::CSerialObject Tparent;
public:
    // constructor
    CNBlast_Result_Base(void);
    // destructor
    virtual ~CNBlast_Result_Base(void);

    // type info
    DECLARE_INTERNAL_TYPE_INFO();

    // members' types
    typedef int TNumseg;
    typedef NCBI_NS_STD::list< int > TStarts;
    typedef NCBI_NS_STD::list< int > TLens;
    typedef NCBI_NS_STD::list< NCBI_NS_NCBI::CRef< CNBlast_Scores > > TScores;

    // members' getters
    // members' setters
    void ResetNumseg(void);
    const int& GetNumseg(void) const;
    void SetNumseg(const int& value);
    int& SetNumseg(void);

    void ResetStarts(void);
    const NCBI_NS_STD::list< int >& GetStarts(void) const;
    NCBI_NS_STD::list< int >& SetStarts(void);

    void ResetLens(void);
    const NCBI_NS_STD::list< int >& GetLens(void) const;
    NCBI_NS_STD::list< int >& SetLens(void);

    void ResetScores(void);
    const NCBI_NS_STD::list< NCBI_NS_NCBI::CRef< CNBlast_Scores > >& GetScores(void) const;
    NCBI_NS_STD::list< NCBI_NS_NCBI::CRef< CNBlast_Scores > >& SetScores(void);

    // reset whole object
    virtual void Reset(void);


private:
    // Prohibit copy constructor and assignment operator
    CNBlast_Result_Base(const CNBlast_Result_Base&);
    CNBlast_Result_Base& operator=(const CNBlast_Result_Base&);

    // members' data
    TNumseg m_Numseg;
    TStarts m_Starts;
    TLens m_Lens;
    TScores m_Scores;
};
class CNBlast_GiAndEval_set_Base : public NCBI_NS_NCBI::CSerialObject
{
    typedef NCBI_NS_NCBI::CSerialObject Tparent;
public:
    // constructor
    CNBlast_GiAndEval_set_Base(void);
    // destructor
    virtual ~CNBlast_GiAndEval_set_Base(void);

    // type info
    DECLARE_INTERNAL_TYPE_INFO();

    // members' types
    typedef int TQuery_gi;
    typedef NCBI_NS_STD::list< NCBI_NS_NCBI::CRef< CNBlast_GiAndEval > > TGiandevals;

    // members' getters
    // members' setters
    bool IsSetQuery_gi(void) const;
    void ResetQuery_gi(void);
    const int& GetQuery_gi(void) const;
    void SetQuery_gi(const int& value);
    int& SetQuery_gi(void);

    void ResetGiandevals(void);
    const NCBI_NS_STD::list< NCBI_NS_NCBI::CRef< CNBlast_GiAndEval > >& GetGiandevals(void) const;
    NCBI_NS_STD::list< NCBI_NS_NCBI::CRef< CNBlast_GiAndEval > >& SetGiandevals(void);

    // reset whole object
    virtual void Reset(void);


private:
    // Prohibit copy constructor and assignment operator
    CNBlast_GiAndEval_set_Base(const CNBlast_GiAndEval_set_Base&);
    CNBlast_GiAndEval_set_Base& operator=(const CNBlast_GiAndEval_set_Base&);

    // members' data
    bool m_set_Query_gi;
    TQuery_gi m_Query_gi;
    TGiandevals m_Giandevals;
};
class CNBlast_GiAndEval_Base : public NCBI_NS_NCBI::CSerialObject
{
    typedef NCBI_NS_NCBI::CSerialObject Tparent;
public:
    // constructor
    CNBlast_GiAndEval_Base(void);
    // destructor
    virtual ~CNBlast_GiAndEval_Base(void);

    // type info
    DECLARE_INTERNAL_TYPE_INFO();

    // members' types
    typedef int TSubject_gi;
    typedef double TEvalue;

    // members' getters
    // members' setters
    void ResetSubject_gi(void);
    const int& GetSubject_gi(void) const;
    void SetSubject_gi(const int& value);
    int& SetSubject_gi(void);

    void ResetEvalue(void);
    const double& GetEvalue(void) const;
    void SetEvalue(const double& value);
    double& SetEvalue(void);

    // reset whole object
    virtual void Reset(void);


private:
    // Prohibit copy constructor and assignment operator
    CNBlast_GiAndEval_Base(const CNBlast_GiAndEval_Base&);
    CNBlast_GiAndEval_Base& operator=(const CNBlast_GiAndEval_Base&);

    // members' data
    TSubject_gi m_Subject_gi;
    TEvalue m_Evalue;
};






///////////////////////////////////////////////////////////
///////////////////// inline methods //////////////////////
///////////////////////////////////////////////////////////
inline
bool CNBlast_Result_Set_Base::IsSetQuery_gi(void) const
{
    return m_set_Query_gi;
}

inline
void CNBlast_Result_Set_Base::ResetQuery_gi(void)
{
    m_Query_gi = 0;
    m_set_Query_gi = false;
}

inline
const CNBlast_Result_Set_Base::TQuery_gi& CNBlast_Result_Set_Base::GetQuery_gi(void) const
{
    return m_Query_gi;
}

inline
void CNBlast_Result_Set_Base::SetQuery_gi(const TQuery_gi& value)
{
    m_Query_gi = value;
    m_set_Query_gi = true;
}

inline
CNBlast_Result_Set_Base::TQuery_gi& CNBlast_Result_Set_Base::SetQuery_gi(void)
{
    m_set_Query_gi = true;
    return m_Query_gi;
}

inline
bool CNBlast_Result_Set_Base::IsSetSubject_gi(void) const
{
    return m_set_Subject_gi;
}

inline
void CNBlast_Result_Set_Base::ResetSubject_gi(void)
{
    m_Subject_gi = 0;
    m_set_Subject_gi = false;
}

inline
const CNBlast_Result_Set_Base::TSubject_gi& CNBlast_Result_Set_Base::GetSubject_gi(void) const
{
    return m_Subject_gi;
}

inline
void CNBlast_Result_Set_Base::SetSubject_gi(const TSubject_gi& value)
{
    m_Subject_gi = value;
    m_set_Subject_gi = true;
}

inline
CNBlast_Result_Set_Base::TSubject_gi& CNBlast_Result_Set_Base::SetSubject_gi(void)
{
    m_set_Subject_gi = true;
    return m_Subject_gi;
}

inline
const CNBlast_Result_Set_Base::TResults& CNBlast_Result_Set_Base::GetResults(void) const
{
    return m_Results;
}

inline
CNBlast_Result_Set_Base::TResults& CNBlast_Result_Set_Base::SetResults(void)
{
    return m_Results;
}

inline
void CNBlast_Scores_Base::ResetBitscore(void)
{
    m_Bitscore = 0;
}

inline
const CNBlast_Scores_Base::TBitscore& CNBlast_Scores_Base::GetBitscore(void) const
{
    return m_Bitscore;
}

inline
void CNBlast_Scores_Base::SetBitscore(const TBitscore& value)
{
    m_Bitscore = value;
}

inline
CNBlast_Scores_Base::TBitscore& CNBlast_Scores_Base::SetBitscore(void)
{
    return m_Bitscore;
}

inline
void CNBlast_Scores_Base::ResetEvalue(void)
{
    m_Evalue = 0;
}

inline
const CNBlast_Scores_Base::TEvalue& CNBlast_Scores_Base::GetEvalue(void) const
{
    return m_Evalue;
}

inline
void CNBlast_Scores_Base::SetEvalue(const TEvalue& value)
{
    m_Evalue = value;
}

inline
CNBlast_Scores_Base::TEvalue& CNBlast_Scores_Base::SetEvalue(void)
{
    return m_Evalue;
}

inline
void CNBlast_Result_Base::ResetNumseg(void)
{
    m_Numseg = 0;
}

inline
const CNBlast_Result_Base::TNumseg& CNBlast_Result_Base::GetNumseg(void) const
{
    return m_Numseg;
}

inline
void CNBlast_Result_Base::SetNumseg(const TNumseg& value)
{
    m_Numseg = value;
}

inline
CNBlast_Result_Base::TNumseg& CNBlast_Result_Base::SetNumseg(void)
{
    return m_Numseg;
}

inline
const CNBlast_Result_Base::TStarts& CNBlast_Result_Base::GetStarts(void) const
{
    return m_Starts;
}

inline
CNBlast_Result_Base::TStarts& CNBlast_Result_Base::SetStarts(void)
{
    return m_Starts;
}

inline
const CNBlast_Result_Base::TLens& CNBlast_Result_Base::GetLens(void) const
{
    return m_Lens;
}

inline
CNBlast_Result_Base::TLens& CNBlast_Result_Base::SetLens(void)
{
    return m_Lens;
}

inline
const CNBlast_Result_Base::TScores& CNBlast_Result_Base::GetScores(void) const
{
    return m_Scores;
}

inline
CNBlast_Result_Base::TScores& CNBlast_Result_Base::SetScores(void)
{
    return m_Scores;
}

inline
bool CNBlast_GiAndEval_set_Base::IsSetQuery_gi(void) const
{
    return m_set_Query_gi;
}

inline
void CNBlast_GiAndEval_set_Base::ResetQuery_gi(void)
{
    m_Query_gi = 0;
    m_set_Query_gi = false;
}

inline
const CNBlast_GiAndEval_set_Base::TQuery_gi& CNBlast_GiAndEval_set_Base::GetQuery_gi(void) const
{
    return m_Query_gi;
}

inline
void CNBlast_GiAndEval_set_Base::SetQuery_gi(const TQuery_gi& value)
{
    m_Query_gi = value;
    m_set_Query_gi = true;
}

inline
CNBlast_GiAndEval_set_Base::TQuery_gi& CNBlast_GiAndEval_set_Base::SetQuery_gi(void)
{
    m_set_Query_gi = true;
    return m_Query_gi;
}

inline
const CNBlast_GiAndEval_set_Base::TGiandevals& CNBlast_GiAndEval_set_Base::GetGiandevals(void) const
{
    return m_Giandevals;
}

inline
CNBlast_GiAndEval_set_Base::TGiandevals& CNBlast_GiAndEval_set_Base::SetGiandevals(void)
{
    return m_Giandevals;
}

inline
void CNBlast_GiAndEval_Base::ResetSubject_gi(void)
{
    m_Subject_gi = 0;
}

inline
const CNBlast_GiAndEval_Base::TSubject_gi& CNBlast_GiAndEval_Base::GetSubject_gi(void) const
{
    return m_Subject_gi;
}

inline
void CNBlast_GiAndEval_Base::SetSubject_gi(const TSubject_gi& value)
{
    m_Subject_gi = value;
}

inline
CNBlast_GiAndEval_Base::TSubject_gi& CNBlast_GiAndEval_Base::SetSubject_gi(void)
{
    return m_Subject_gi;
}

inline
void CNBlast_GiAndEval_Base::ResetEvalue(void)
{
    m_Evalue = 0;
}

inline
const CNBlast_GiAndEval_Base::TEvalue& CNBlast_GiAndEval_Base::GetEvalue(void) const
{
    return m_Evalue;
}

inline
void CNBlast_GiAndEval_Base::SetEvalue(const TEvalue& value)
{
    m_Evalue = value;
}

inline
CNBlast_GiAndEval_Base::TEvalue& CNBlast_GiAndEval_Base::SetEvalue(void)
{
    return m_Evalue;
}

///////////////////////////////////////////////////////////
////////////////// end of inline methods //////////////////
///////////////////////////////////////////////////////////





END_seqhound_SCOPE // namespace seqhound::


#endif // NBLAST_BASE_HPP