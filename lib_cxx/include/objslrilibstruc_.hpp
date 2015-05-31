/* $Id: objslrilibstruc_.hpp,v 1.2 2003/09/30 18:08:51 haocl Exp $
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
 *   'slrilibstruc.asn'.
 *
 * ATTENTION:
 *   Don't edit or check-in this file to the CVS as this file will
 *   be overridden (by DATATOOL) without warning!
 * ===========================================================================
 */

#ifndef OBJSLRILIBSTRUC_BASE_HPP
#define OBJSLRILIBSTRUC_BASE_HPP

// standard includes
#include <serial/serialbase.hpp>

// generated includes
#include <list>
#include <string>
#include <vector>

BEGIN_NCBI_SCOPE

#ifndef BEGIN_objects_SCOPE
#  define BEGIN_objects_SCOPE BEGIN_SCOPE(objects)
#  define END_objects_SCOPE END_SCOPE(objects)
#endif
BEGIN_objects_SCOPE // namespace ncbi::objects::


// forward declarations
class CBioseq;
class CBioseq_set;
class CBiostruc;
END_objects_SCOPE // namespace ncbi::objects::

END_NCBI_SCOPE

#ifndef BEGIN_slri_SCOPE
#  define BEGIN_slri_SCOPE BEGIN_SCOPE(slri)
#  define END_slri_SCOPE END_SCOPE(slri)
#endif
BEGIN_slri_SCOPE // namespace slri::

class CDData;
class CDKey;
class CDNode;
class CSLRIDomain;
class CSLRIFasta;
class CSLRINode;
class CSLRISeq_id;
END_slri_SCOPE // namespace slri::

BEGIN_NCBI_SCOPE

#ifndef BEGIN_objects_SCOPE
#  define BEGIN_objects_SCOPE BEGIN_SCOPE(objects)
#  define END_objects_SCOPE END_SCOPE(objects)
#endif
BEGIN_objects_SCOPE // namespace ncbi::objects::

class CSeq_align;
class CSeq_entry;
class CSeq_id;

END_objects_SCOPE // namespace ncbi::objects::

END_NCBI_SCOPE

#ifndef BEGIN_slri_SCOPE
#  define BEGIN_slri_SCOPE BEGIN_SCOPE(slri)
#  define END_slri_SCOPE END_SCOPE(slri)
#endif
BEGIN_slri_SCOPE // namespace slri::


// generated classes

class CSLRIValNode_Base : public NCBI_NS_NCBI::CSerialObject
{
    typedef NCBI_NS_NCBI::CSerialObject Tparent;
public:
    // constructor
    CSLRIValNode_Base(void);
    // destructor
    virtual ~CSLRIValNode_Base(void);

    // type info
    DECLARE_INTERNAL_TYPE_INFO();

    // types
    typedef NCBI_NS_STD::list< NCBI_NS_NCBI::CRef< CSLRINode > > Tdata;

    // getters
    // setters

    // mandatory
    // typedef NCBI_NS_STD::list< NCBI_NS_NCBI::CRef< CSLRINode > > Tdata
    bool IsSet(void) const;
    bool CanGet(void) const;
    void Reset(void);
    const Tdata& Get(void) const;
    Tdata& Set(void);
    operator const Tdata& (void) const;
    operator Tdata& (void);

    // reset whole object
    

private:
    // Prohibit copy constructor and assignment operator
    CSLRIValNode_Base(const CSLRIValNode_Base&);
    CSLRIValNode_Base& operator=(const CSLRIValNode_Base&);

    // data
    Uint4 m_set_State[1];
    Tdata m_data;
};
class CSLRISeq_id_Base : public NCBI_NS_NCBI::CSerialObject
{
    typedef NCBI_NS_NCBI::CSerialObject Tparent;
public:
    // constructor
    CSLRISeq_id_Base(void);
    // destructor
    virtual ~CSLRISeq_id_Base(void);

    // type info
    DECLARE_INTERNAL_TYPE_INFO();

    // types
    typedef NCBI_NS_STD::list< NCBI_NS_NCBI::CRef< NCBI_NS_NCBI::objects::CSeq_id > > Tdata;

    // getters
    // setters

    // mandatory
    // typedef NCBI_NS_STD::list< NCBI_NS_NCBI::CRef< NCBI_NS_NCBI::objects::CSeq_id > > Tdata
    bool IsSet(void) const;
    bool CanGet(void) const;
    void Reset(void);
    const Tdata& Get(void) const;
    Tdata& Set(void);
    operator const Tdata& (void) const;
    operator Tdata& (void);

    // reset whole object
    

private:
    // Prohibit copy constructor and assignment operator
    CSLRISeq_id_Base(const CSLRISeq_id_Base&);
    CSLRISeq_id_Base& operator=(const CSLRISeq_id_Base&);

    // data
    Uint4 m_set_State[1];
    Tdata m_data;
};
class CSLRINode_Base : public NCBI_NS_NCBI::CSerialObject
{
    typedef NCBI_NS_NCBI::CSerialObject Tparent;
public:
    // constructor
    CSLRINode_Base(void);
    // destructor
    virtual ~CSLRINode_Base(void);

    // type info
    DECLARE_INTERNAL_TYPE_INFO();

    // choice state enum
    enum E_Choice {
        e_not_set = 0,
        e_Gi,
        e_Accession,
        e_Bioseq,
        e_Biostruc,
        e_Fasta,
        e_Seqid,
        e_Domain,
        e_Seqalign,
        e_Bioseqset,
        e_Seqentry
    };

    // reset selection to none
    virtual void Reset(void);

    // choice state
    E_Choice Which(void) const;
    // throw exception if current selection is not as requested
    void CheckSelected(E_Choice index) const;
    // throw exception about wrong selection
    void ThrowInvalidSelection(E_Choice index) const;
    // return selection name (for diagnostic purposes)
    static NCBI_NS_STD::string SelectionName(E_Choice index);

    // select requested variant if needed
    void Select(E_Choice index, NCBI_NS_NCBI::EResetVariant reset = NCBI_NS_NCBI::eDoResetVariant);

    // types
    typedef int TGi;
    typedef NCBI_NS_STD::string TAccession;
    typedef NCBI_NS_NCBI::objects::CBioseq TBioseq;
    typedef NCBI_NS_NCBI::objects::CBiostruc TBiostruc;
    typedef CSLRIFasta TFasta;
    typedef CSLRISeq_id TSeqid;
    typedef CSLRIDomain TDomain;
    typedef NCBI_NS_NCBI::objects::CSeq_align TSeqalign;
    typedef NCBI_NS_NCBI::objects::CBioseq_set TBioseqset;
    typedef NCBI_NS_NCBI::objects::CSeq_entry TSeqentry;

    // getters
    // setters

    // typedef int TGi
    bool IsGi(void) const;
    TGi GetGi(void) const;
    TGi& SetGi(void);
    void SetGi(const TGi& value);

    // typedef NCBI_NS_STD::string TAccession
    bool IsAccession(void) const;
    const TAccession& GetAccession(void) const;
    TAccession& SetAccession(void);
    void SetAccession(const TAccession& value);

    // typedef NCBI_NS_NCBI::objects::CBioseq TBioseq
    bool IsBioseq(void) const;
    const TBioseq& GetBioseq(void) const;
    TBioseq& SetBioseq(void);
    void SetBioseq(TBioseq& value);

    // typedef NCBI_NS_NCBI::objects::CBiostruc TBiostruc
    bool IsBiostruc(void) const;
    const TBiostruc& GetBiostruc(void) const;
    TBiostruc& SetBiostruc(void);
    void SetBiostruc(TBiostruc& value);

    // typedef CSLRIFasta TFasta
    bool IsFasta(void) const;
    const TFasta& GetFasta(void) const;
    TFasta& SetFasta(void);
    void SetFasta(TFasta& value);

    // typedef CSLRISeq_id TSeqid
    bool IsSeqid(void) const;
    const TSeqid& GetSeqid(void) const;
    TSeqid& SetSeqid(void);
    void SetSeqid(TSeqid& value);

    // typedef CSLRIDomain TDomain
    bool IsDomain(void) const;
    const TDomain& GetDomain(void) const;
    TDomain& SetDomain(void);
    void SetDomain(TDomain& value);

    // typedef NCBI_NS_NCBI::objects::CSeq_align TSeqalign
    bool IsSeqalign(void) const;
    const TSeqalign& GetSeqalign(void) const;
    TSeqalign& SetSeqalign(void);
    void SetSeqalign(TSeqalign& value);

    // typedef NCBI_NS_NCBI::objects::CBioseq_set TBioseqset
    bool IsBioseqset(void) const;
    const TBioseqset& GetBioseqset(void) const;
    TBioseqset& SetBioseqset(void);
    void SetBioseqset(TBioseqset& value);

    // typedef NCBI_NS_NCBI::objects::CSeq_entry TSeqentry
    bool IsSeqentry(void) const;
    const TSeqentry& GetSeqentry(void) const;
    TSeqentry& SetSeqentry(void);
    void SetSeqentry(TSeqentry& value);


private:
    // copy constructor and assignment operator
    CSLRINode_Base(const CSLRINode_Base& );
    CSLRINode_Base& operator=(const CSLRINode_Base& );

    // choice state
    E_Choice m_choice;
    // helper methods
    void DoSelect(E_Choice index);

    static const char* const sm_SelectionNames[];
    // data
    union {
        TGi m_Gi;
        NCBI_NS_STD::string *m_string;
        NCBI_NS_NCBI::CSerialObject *m_object;
    };
};
class CSLRIFasta_Base : public NCBI_NS_NCBI::CSerialObject
{
    typedef NCBI_NS_NCBI::CSerialObject Tparent;
public:
    // constructor
    CSLRIFasta_Base(void);
    // destructor
    virtual ~CSLRIFasta_Base(void);

    // type info
    DECLARE_INTERNAL_TYPE_INFO();

    // types
    typedef NCBI_NS_STD::string TDefline;
    typedef NCBI_NS_STD::string TSequence;

    // getters
    // setters

    // mandatory
    // typedef NCBI_NS_STD::string TDefline
    bool IsSetDefline(void) const;
    bool CanGetDefline(void) const;
    void ResetDefline(void);
    const TDefline& GetDefline(void) const;
    void SetDefline(const TDefline& value);
    TDefline& SetDefline(void);

    // mandatory
    // typedef NCBI_NS_STD::string TSequence
    bool IsSetSequence(void) const;
    bool CanGetSequence(void) const;
    void ResetSequence(void);
    const TSequence& GetSequence(void) const;
    void SetSequence(const TSequence& value);
    TSequence& SetSequence(void);

    // reset whole object
    virtual void Reset(void);


private:
    // Prohibit copy constructor and assignment operator
    CSLRIFasta_Base(const CSLRIFasta_Base&);
    CSLRIFasta_Base& operator=(const CSLRIFasta_Base&);

    // data
    Uint4 m_set_State[1];
    TDefline m_Defline;
    TSequence m_Sequence;
};
class CSLRIDomain_Base : public NCBI_NS_NCBI::CSerialObject
{
    typedef NCBI_NS_NCBI::CSerialObject Tparent;
public:
    // constructor
    CSLRIDomain_Base(void);
    // destructor
    virtual ~CSLRIDomain_Base(void);

    // type info
    DECLARE_INTERNAL_TYPE_INFO();

    // types
    typedef int TGi;
    typedef int TFrom;
    typedef int TTo;

    // getters
    // setters

    // mandatory
    // typedef int TGi
    bool IsSetGi(void) const;
    bool CanGetGi(void) const;
    void ResetGi(void);
    TGi GetGi(void) const;
    void SetGi(const TGi& value);
    TGi& SetGi(void);

    // mandatory
    // typedef int TFrom
    bool IsSetFrom(void) const;
    bool CanGetFrom(void) const;
    void ResetFrom(void);
    TFrom GetFrom(void) const;
    void SetFrom(const TFrom& value);
    TFrom& SetFrom(void);

    // mandatory
    // typedef int TTo
    bool IsSetTo(void) const;
    bool CanGetTo(void) const;
    void ResetTo(void);
    TTo GetTo(void) const;
    void SetTo(const TTo& value);
    TTo& SetTo(void);

    // reset whole object
    virtual void Reset(void);


private:
    // Prohibit copy constructor and assignment operator
    CSLRIDomain_Base(const CSLRIDomain_Base&);
    CSLRIDomain_Base& operator=(const CSLRIDomain_Base&);

    // data
    Uint4 m_set_State[1];
    TGi m_Gi;
    TFrom m_From;
    TTo m_To;
};
class CDTree_Base : public NCBI_NS_NCBI::CSerialObject
{
    typedef NCBI_NS_NCBI::CSerialObject Tparent;
public:
    // constructor
    CDTree_Base(void);
    // destructor
    virtual ~CDTree_Base(void);

    // type info
    DECLARE_INTERNAL_TYPE_INFO();

    // types
    typedef NCBI_NS_STD::list< NCBI_NS_NCBI::CRef< CDNode > > Tdata;

    // getters
    // setters

    // mandatory
    // typedef NCBI_NS_STD::list< NCBI_NS_NCBI::CRef< CDNode > > Tdata
    bool IsSet(void) const;
    bool CanGet(void) const;
    void Reset(void);
    const Tdata& Get(void) const;
    Tdata& Set(void);
    operator const Tdata& (void) const;
    operator Tdata& (void);

    // reset whole object
    

private:
    // Prohibit copy constructor and assignment operator
    CDTree_Base(const CDTree_Base&);
    CDTree_Base& operator=(const CDTree_Base&);

    // data
    Uint4 m_set_State[1];
    Tdata m_data;
};
class CDNode_Base : public NCBI_NS_NCBI::CSerialObject
{
    typedef NCBI_NS_NCBI::CSerialObject Tparent;
public:
    // constructor
    CDNode_Base(void);
    // destructor
    virtual ~CDNode_Base(void);

    // type info
    DECLARE_INTERNAL_TYPE_INFO();

    enum EState {
        eState_none = 0,
        eState_vis = 1
    };
    
    DECLARE_INTERNAL_ENUM_INFO(EState);
    
    // types
    typedef CDKey TKey;
    typedef CDData TData;
    typedef int TNum;
    typedef EState TState;
    typedef NCBI_NS_STD::list< NCBI_NS_NCBI::CRef< CDNode > > TP;
    typedef NCBI_NS_STD::list< NCBI_NS_NCBI::CRef< CDNode > > TC;

    // getters
    // setters

    // mandatory
    // typedef CDKey TKey
    bool IsSetKey(void) const;
    bool CanGetKey(void) const;
    void ResetKey(void);
    const TKey& GetKey(void) const;
    void SetKey(TKey& value);
    TKey& SetKey(void);

    // optional
    // typedef CDData TData
    bool IsSetData(void) const;
    bool CanGetData(void) const;
    void ResetData(void);
    const TData& GetData(void) const;
    void SetData(TData& value);
    TData& SetData(void);

    // optional
    // typedef int TNum
    bool IsSetNum(void) const;
    bool CanGetNum(void) const;
    void ResetNum(void);
    TNum GetNum(void) const;
    void SetNum(const TNum& value);
    TNum& SetNum(void);

    // mandatory with default
    // typedef EState TState
    bool IsSetState(void) const;
    bool CanGetState(void) const;
    void ResetState(void);
    TState GetState(void) const;
    void SetState(const TState& value);
    TState& SetState(void);

    // optional
    // typedef NCBI_NS_STD::list< NCBI_NS_NCBI::CRef< CDNode > > TP
    bool IsSetP(void) const;
    bool CanGetP(void) const;
    void ResetP(void);
    const TP& GetP(void) const;
    TP& SetP(void);

    // optional
    // typedef NCBI_NS_STD::list< NCBI_NS_NCBI::CRef< CDNode > > TC
    bool IsSetC(void) const;
    bool CanGetC(void) const;
    void ResetC(void);
    const TC& GetC(void) const;
    TC& SetC(void);

    // reset whole object
    virtual void Reset(void);


private:
    // Prohibit copy constructor and assignment operator
    CDNode_Base(const CDNode_Base&);
    CDNode_Base& operator=(const CDNode_Base&);

    // data
    Uint4 m_set_State[1];
    NCBI_NS_NCBI::CRef< TKey > m_Key;
    NCBI_NS_NCBI::CRef< TData > m_Data;
    TNum m_Num;
    TState m_State;
    TP m_P;
    TC m_C;
};
class CDKey_Base : public NCBI_NS_NCBI::CSerialObject
{
    typedef NCBI_NS_NCBI::CSerialObject Tparent;
public:
    // constructor
    CDKey_Base(void);
    // destructor
    virtual ~CDKey_Base(void);

    // type info
    DECLARE_INTERNAL_TYPE_INFO();

    // choice state enum
    enum E_Choice {
        e_not_set = 0,
        e_Id,
        e_Str,
        e_Real
    };

    // reset selection to none
    virtual void Reset(void);

    // choice state
    E_Choice Which(void) const;
    // throw exception if current selection is not as requested
    void CheckSelected(E_Choice index) const;
    // throw exception about wrong selection
    void ThrowInvalidSelection(E_Choice index) const;
    // return selection name (for diagnostic purposes)
    static NCBI_NS_STD::string SelectionName(E_Choice index);

    // select requested variant if needed
    void Select(E_Choice index, NCBI_NS_NCBI::EResetVariant reset = NCBI_NS_NCBI::eDoResetVariant);

    // types
    typedef int TId;
    typedef NCBI_NS_STD::string TStr;
    typedef double TReal;

    // getters
    // setters

    // typedef int TId
    bool IsId(void) const;
    TId GetId(void) const;
    TId& SetId(void);
    void SetId(const TId& value);

    // typedef NCBI_NS_STD::string TStr
    bool IsStr(void) const;
    const TStr& GetStr(void) const;
    TStr& SetStr(void);
    void SetStr(const TStr& value);

    // typedef double TReal
    bool IsReal(void) const;
    TReal GetReal(void) const;
    TReal& SetReal(void);
    void SetReal(const TReal& value);


private:
    // copy constructor and assignment operator
    CDKey_Base(const CDKey_Base& );
    CDKey_Base& operator=(const CDKey_Base& );

    // choice state
    E_Choice m_choice;
    // helper methods
    void DoSelect(E_Choice index);

    static const char* const sm_SelectionNames[];
    // data
    union {
        TId m_Id;
        TReal m_Real;
        NCBI_NS_STD::string *m_string;
    };
};
class CDData_Base : public NCBI_NS_NCBI::CSerialObject
{
    typedef NCBI_NS_NCBI::CSerialObject Tparent;
public:
    // constructor
    CDData_Base(void);
    // destructor
    virtual ~CDData_Base(void);

    // type info
    DECLARE_INTERNAL_TYPE_INFO();

    // choice state enum
    enum E_Choice {
        e_not_set = 0,
        e_Id,
        e_Str,
        e_Real,
        e_Bool,
        e_Os,
        e_Strs,
        e_Ids,
        e_Reals,
        e_Oss,
        e_Gen_memfree,
        e_Data_list,
        e_User_def
    };

    // reset selection to none
    virtual void Reset(void);

    // choice state
    E_Choice Which(void) const;
    // throw exception if current selection is not as requested
    void CheckSelected(E_Choice index) const;
    // throw exception about wrong selection
    void ThrowInvalidSelection(E_Choice index) const;
    // return selection name (for diagnostic purposes)
    static NCBI_NS_STD::string SelectionName(E_Choice index);

    // select requested variant if needed
    void Select(E_Choice index, NCBI_NS_NCBI::EResetVariant reset = NCBI_NS_NCBI::eDoResetVariant);

    // types
    typedef int TId;
    typedef NCBI_NS_STD::string TStr;
    typedef double TReal;
    typedef bool TBool;
    typedef NCBI_NS_STD::vector< char > TOs;
    typedef NCBI_NS_STD::list< NCBI_NS_STD::string > TStrs;
    typedef NCBI_NS_STD::list< int > TIds;
    typedef NCBI_NS_STD::list< double > TReals;
    typedef NCBI_NS_STD::list< NCBI_NS_STD::vector< char >* > TOss;
    typedef NCBI_NS_STD::string TGen_memfree;
    typedef NCBI_NS_STD::list< NCBI_NS_NCBI::CRef< CDData > > TData_list;
    typedef bool TUser_def;

    // getters
    // setters

    // typedef int TId
    bool IsId(void) const;
    TId GetId(void) const;
    TId& SetId(void);
    void SetId(const TId& value);

    // typedef NCBI_NS_STD::string TStr
    bool IsStr(void) const;
    const TStr& GetStr(void) const;
    TStr& SetStr(void);
    void SetStr(const TStr& value);

    // typedef double TReal
    bool IsReal(void) const;
    TReal GetReal(void) const;
    TReal& SetReal(void);
    void SetReal(const TReal& value);

    // typedef bool TBool
    bool IsBool(void) const;
    TBool GetBool(void) const;
    TBool& SetBool(void);
    void SetBool(const TBool& value);

    // typedef NCBI_NS_STD::vector< char > TOs
    bool IsOs(void) const;
    const TOs& GetOs(void) const;
    TOs& SetOs(void);

    // typedef NCBI_NS_STD::list< NCBI_NS_STD::string > TStrs
    bool IsStrs(void) const;
    const TStrs& GetStrs(void) const;
    TStrs& SetStrs(void);

    // typedef NCBI_NS_STD::list< int > TIds
    bool IsIds(void) const;
    const TIds& GetIds(void) const;
    TIds& SetIds(void);

    // typedef NCBI_NS_STD::list< double > TReals
    bool IsReals(void) const;
    const TReals& GetReals(void) const;
    TReals& SetReals(void);

    // typedef NCBI_NS_STD::list< NCBI_NS_STD::vector< char >* > TOss
    bool IsOss(void) const;
    const TOss& GetOss(void) const;
    TOss& SetOss(void);

    // typedef NCBI_NS_STD::string TGen_memfree
    bool IsGen_memfree(void) const;
    const TGen_memfree& GetGen_memfree(void) const;
    TGen_memfree& SetGen_memfree(void);
    void SetGen_memfree(const TGen_memfree& value);

    // typedef NCBI_NS_STD::list< NCBI_NS_NCBI::CRef< CDData > > TData_list
    bool IsData_list(void) const;
    const TData_list& GetData_list(void) const;
    TData_list& SetData_list(void);

    // typedef bool TUser_def
    bool IsUser_def(void) const;
    const TUser_def& GetUser_def(void) const;
    TUser_def& SetUser_def(void);
    void SetUser_def(const TUser_def& value);


private:
    // copy constructor and assignment operator
    CDData_Base(const CDData_Base& );
    CDData_Base& operator=(const CDData_Base& );

    // choice state
    E_Choice m_choice;
    // helper methods
    void DoSelect(E_Choice index);

    static const char* const sm_SelectionNames[];
    // data
    union {
        TId m_Id;
        TReal m_Real;
        TBool m_Bool;
        TOs *m_Os;
        TStrs *m_Strs;
        TIds *m_Ids;
        TReals *m_Reals;
        TOss *m_Oss;
        TData_list *m_Data_list;
        TUser_def m_User_def;
        NCBI_NS_STD::string *m_string;
    };
};






///////////////////////////////////////////////////////////
///////////////////// inline methods //////////////////////
///////////////////////////////////////////////////////////
inline
bool CSLRIValNode_Base::IsSet(void) const
{
    return ((m_set_State[0] & 0x3) != 0);
}

inline
bool CSLRIValNode_Base::CanGet(void) const
{
    return true;
}

inline
const NCBI_NS_STD::list< NCBI_NS_NCBI::CRef< CSLRINode > >& CSLRIValNode_Base::Get(void) const
{
    return m_data;
}

inline
NCBI_NS_STD::list< NCBI_NS_NCBI::CRef< CSLRINode > >& CSLRIValNode_Base::Set(void)
{
    m_set_State[0] |= 0x1;
    return m_data;
}

inline
CSLRIValNode_Base::operator const NCBI_NS_STD::list< NCBI_NS_NCBI::CRef< CSLRINode > >& (void) const
{
    return m_data;
}

inline
CSLRIValNode_Base::operator NCBI_NS_STD::list< NCBI_NS_NCBI::CRef< CSLRINode > >& (void)
{
    m_set_State[0] |= 0x1;
    return m_data;
}

inline
bool CSLRISeq_id_Base::IsSet(void) const
{
    return ((m_set_State[0] & 0x3) != 0);
}

inline
bool CSLRISeq_id_Base::CanGet(void) const
{
    return true;
}

inline
const NCBI_NS_STD::list< NCBI_NS_NCBI::CRef< NCBI_NS_NCBI::objects::CSeq_id > >& CSLRISeq_id_Base::Get(void) const
{
    return m_data;
}

inline
NCBI_NS_STD::list< NCBI_NS_NCBI::CRef< NCBI_NS_NCBI::objects::CSeq_id > >& CSLRISeq_id_Base::Set(void)
{
    m_set_State[0] |= 0x1;
    return m_data;
}

inline
CSLRISeq_id_Base::operator const NCBI_NS_STD::list< NCBI_NS_NCBI::CRef< NCBI_NS_NCBI::objects::CSeq_id > >& (void) const
{
    return m_data;
}

inline
CSLRISeq_id_Base::operator NCBI_NS_STD::list< NCBI_NS_NCBI::CRef< NCBI_NS_NCBI::objects::CSeq_id > >& (void)
{
    m_set_State[0] |= 0x1;
    return m_data;
}

inline
CSLRINode_Base::E_Choice CSLRINode_Base::Which(void) const
{
    return m_choice;
}

inline
void CSLRINode_Base::CheckSelected(E_Choice index) const
{
    if ( m_choice != index )
        ThrowInvalidSelection(index);
}

inline
void CSLRINode_Base::Select(E_Choice index, NCBI_NS_NCBI::EResetVariant reset)
{
    if ( reset == NCBI_NS_NCBI::eDoResetVariant || m_choice != index ) {
        if ( m_choice != e_not_set )
            Reset();
        DoSelect(index);
    }
}

inline
bool CSLRINode_Base::IsGi(void) const
{
    return m_choice == e_Gi;
}

inline
int CSLRINode_Base::GetGi(void) const
{
    CheckSelected(e_Gi);
    return m_Gi;
}

inline
int& CSLRINode_Base::SetGi(void)
{
    Select(e_Gi, NCBI_NS_NCBI::eDoNotResetVariant);
    return m_Gi;
}

inline
void CSLRINode_Base::SetGi(const int& value)
{
    Select(e_Gi, NCBI_NS_NCBI::eDoNotResetVariant);
    m_Gi = value;
}

inline
bool CSLRINode_Base::IsAccession(void) const
{
    return m_choice == e_Accession;
}

inline
const NCBI_NS_STD::string& CSLRINode_Base::GetAccession(void) const
{
    CheckSelected(e_Accession);
    return *m_string;
}

inline
NCBI_NS_STD::string& CSLRINode_Base::SetAccession(void)
{
    Select(e_Accession, NCBI_NS_NCBI::eDoNotResetVariant);
    return *m_string;
}

inline
void CSLRINode_Base::SetAccession(const NCBI_NS_STD::string& value)
{
    Select(e_Accession, NCBI_NS_NCBI::eDoNotResetVariant);
    *m_string = value;
}

inline
bool CSLRINode_Base::IsBioseq(void) const
{
    return m_choice == e_Bioseq;
}

inline
bool CSLRINode_Base::IsBiostruc(void) const
{
    return m_choice == e_Biostruc;
}

inline
bool CSLRINode_Base::IsFasta(void) const
{
    return m_choice == e_Fasta;
}

inline
bool CSLRINode_Base::IsSeqid(void) const
{
    return m_choice == e_Seqid;
}

inline
bool CSLRINode_Base::IsDomain(void) const
{
    return m_choice == e_Domain;
}

inline
bool CSLRINode_Base::IsSeqalign(void) const
{
    return m_choice == e_Seqalign;
}

inline
bool CSLRINode_Base::IsBioseqset(void) const
{
    return m_choice == e_Bioseqset;
}

inline
bool CSLRINode_Base::IsSeqentry(void) const
{
    return m_choice == e_Seqentry;
}

inline
bool CSLRIFasta_Base::IsSetDefline(void) const
{
    return ((m_set_State[0] & 0x3) != 0);
}

inline
bool CSLRIFasta_Base::CanGetDefline(void) const
{
    return IsSetDefline();
}

inline
const NCBI_NS_STD::string& CSLRIFasta_Base::GetDefline(void) const
{
    if (!CanGetDefline()) {
        ThrowUnassigned(0);
    }
    return m_Defline;
}

inline
void CSLRIFasta_Base::SetDefline(const NCBI_NS_STD::string& value)
{
    m_Defline = value;
    m_set_State[0] |= 0x3;
}

inline
NCBI_NS_STD::string& CSLRIFasta_Base::SetDefline(void)
{
#ifdef _DEBUG
    if (!IsSetDefline()) {
        m_Defline = ms_UnassignedStr;
    }
#endif
    m_set_State[0] |= 0x1;
    return m_Defline;
}

inline
bool CSLRIFasta_Base::IsSetSequence(void) const
{
    return ((m_set_State[0] & 0xc) != 0);
}

inline
bool CSLRIFasta_Base::CanGetSequence(void) const
{
    return IsSetSequence();
}

inline
const NCBI_NS_STD::string& CSLRIFasta_Base::GetSequence(void) const
{
    if (!CanGetSequence()) {
        ThrowUnassigned(1);
    }
    return m_Sequence;
}

inline
void CSLRIFasta_Base::SetSequence(const NCBI_NS_STD::string& value)
{
    m_Sequence = value;
    m_set_State[0] |= 0xc;
}

inline
NCBI_NS_STD::string& CSLRIFasta_Base::SetSequence(void)
{
#ifdef _DEBUG
    if (!IsSetSequence()) {
        m_Sequence = ms_UnassignedStr;
    }
#endif
    m_set_State[0] |= 0x4;
    return m_Sequence;
}

inline
bool CSLRIDomain_Base::IsSetGi(void) const
{
    return ((m_set_State[0] & 0x3) != 0);
}

inline
bool CSLRIDomain_Base::CanGetGi(void) const
{
    return IsSetGi();
}

inline
void CSLRIDomain_Base::ResetGi(void)
{
    m_Gi = 0;
    m_set_State[0] &= ~0x3;
}

inline
int CSLRIDomain_Base::GetGi(void) const
{
    if (!CanGetGi()) {
        ThrowUnassigned(0);
    }
    return m_Gi;
}

inline
void CSLRIDomain_Base::SetGi(const int& value)
{
    m_Gi = value;
    m_set_State[0] |= 0x3;
}

inline
int& CSLRIDomain_Base::SetGi(void)
{
#ifdef _DEBUG
    if (!IsSetGi()) {
        memset(&m_Gi,ms_UnassignedByte,sizeof(m_Gi));
    }
#endif
    m_set_State[0] |= 0x1;
    return m_Gi;
}

inline
bool CSLRIDomain_Base::IsSetFrom(void) const
{
    return ((m_set_State[0] & 0xc) != 0);
}

inline
bool CSLRIDomain_Base::CanGetFrom(void) const
{
    return IsSetFrom();
}

inline
void CSLRIDomain_Base::ResetFrom(void)
{
    m_From = 0;
    m_set_State[0] &= ~0xc;
}

inline
int CSLRIDomain_Base::GetFrom(void) const
{
    if (!CanGetFrom()) {
        ThrowUnassigned(1);
    }
    return m_From;
}

inline
void CSLRIDomain_Base::SetFrom(const int& value)
{
    m_From = value;
    m_set_State[0] |= 0xc;
}

inline
int& CSLRIDomain_Base::SetFrom(void)
{
#ifdef _DEBUG
    if (!IsSetFrom()) {
        memset(&m_From,ms_UnassignedByte,sizeof(m_From));
    }
#endif
    m_set_State[0] |= 0x4;
    return m_From;
}

inline
bool CSLRIDomain_Base::IsSetTo(void) const
{
    return ((m_set_State[0] & 0x30) != 0);
}

inline
bool CSLRIDomain_Base::CanGetTo(void) const
{
    return IsSetTo();
}

inline
void CSLRIDomain_Base::ResetTo(void)
{
    m_To = 0;
    m_set_State[0] &= ~0x30;
}

inline
int CSLRIDomain_Base::GetTo(void) const
{
    if (!CanGetTo()) {
        ThrowUnassigned(2);
    }
    return m_To;
}

inline
void CSLRIDomain_Base::SetTo(const int& value)
{
    m_To = value;
    m_set_State[0] |= 0x30;
}

inline
int& CSLRIDomain_Base::SetTo(void)
{
#ifdef _DEBUG
    if (!IsSetTo()) {
        memset(&m_To,ms_UnassignedByte,sizeof(m_To));
    }
#endif
    m_set_State[0] |= 0x10;
    return m_To;
}

inline
bool CDTree_Base::IsSet(void) const
{
    return ((m_set_State[0] & 0x3) != 0);
}

inline
bool CDTree_Base::CanGet(void) const
{
    return true;
}

inline
const NCBI_NS_STD::list< NCBI_NS_NCBI::CRef< CDNode > >& CDTree_Base::Get(void) const
{
    return m_data;
}

inline
NCBI_NS_STD::list< NCBI_NS_NCBI::CRef< CDNode > >& CDTree_Base::Set(void)
{
    m_set_State[0] |= 0x1;
    return m_data;
}

inline
CDTree_Base::operator const NCBI_NS_STD::list< NCBI_NS_NCBI::CRef< CDNode > >& (void) const
{
    return m_data;
}

inline
CDTree_Base::operator NCBI_NS_STD::list< NCBI_NS_NCBI::CRef< CDNode > >& (void)
{
    m_set_State[0] |= 0x1;
    return m_data;
}

inline
bool CDNode_Base::IsSetKey(void) const
{
    return m_Key;
}

inline
bool CDNode_Base::CanGetKey(void) const
{
    return IsSetKey();
}

inline
const CDKey& CDNode_Base::GetKey(void) const
{
    if (!CanGetKey()) {
        ThrowUnassigned(0);
    }
    return (*m_Key);
}

inline
CDKey& CDNode_Base::SetKey(void)
{
    return (*m_Key);
}

inline
bool CDNode_Base::IsSetData(void) const
{
    return m_Data;
}

inline
bool CDNode_Base::CanGetData(void) const
{
    return IsSetData();
}

inline
const CDData& CDNode_Base::GetData(void) const
{
    if (!CanGetData()) {
        ThrowUnassigned(1);
    }
    return (*m_Data);
}

inline
bool CDNode_Base::IsSetNum(void) const
{
    return ((m_set_State[0] & 0x30) != 0);
}

inline
bool CDNode_Base::CanGetNum(void) const
{
    return IsSetNum();
}

inline
void CDNode_Base::ResetNum(void)
{
    m_Num = 0;
    m_set_State[0] &= ~0x30;
}

inline
int CDNode_Base::GetNum(void) const
{
    if (!CanGetNum()) {
        ThrowUnassigned(2);
    }
    return m_Num;
}

inline
void CDNode_Base::SetNum(const int& value)
{
    m_Num = value;
    m_set_State[0] |= 0x30;
}

inline
int& CDNode_Base::SetNum(void)
{
#ifdef _DEBUG
    if (!IsSetNum()) {
        memset(&m_Num,ms_UnassignedByte,sizeof(m_Num));
    }
#endif
    m_set_State[0] |= 0x10;
    return m_Num;
}

inline
bool CDNode_Base::IsSetState(void) const
{
    return ((m_set_State[0] & 0xc0) != 0);
}

inline
bool CDNode_Base::CanGetState(void) const
{
    return true;
}

inline
void CDNode_Base::ResetState(void)
{
    m_State = eState_none;
    m_set_State[0] &= ~0xc0;
}

inline
CDNode_Base::EState CDNode_Base::GetState(void) const
{
    return m_State;
}

inline
void CDNode_Base::SetState(const CDNode_Base::EState& value)
{
    m_State = value;
    m_set_State[0] |= 0xc0;
}

inline
CDNode_Base::EState& CDNode_Base::SetState(void)
{
#ifdef _DEBUG
    if (!IsSetState()) {
        memset(&m_State,ms_UnassignedByte,sizeof(m_State));
    }
#endif
    m_set_State[0] |= 0x40;
    return m_State;
}

inline
bool CDNode_Base::IsSetP(void) const
{
    return ((m_set_State[0] & 0x300) != 0);
}

inline
bool CDNode_Base::CanGetP(void) const
{
    return true;
}

inline
const NCBI_NS_STD::list< NCBI_NS_NCBI::CRef< CDNode > >& CDNode_Base::GetP(void) const
{
    return m_P;
}

inline
NCBI_NS_STD::list< NCBI_NS_NCBI::CRef< CDNode > >& CDNode_Base::SetP(void)
{
    m_set_State[0] |= 0x100;
    return m_P;
}

inline
bool CDNode_Base::IsSetC(void) const
{
    return ((m_set_State[0] & 0xc00) != 0);
}

inline
bool CDNode_Base::CanGetC(void) const
{
    return true;
}

inline
const NCBI_NS_STD::list< NCBI_NS_NCBI::CRef< CDNode > >& CDNode_Base::GetC(void) const
{
    return m_C;
}

inline
NCBI_NS_STD::list< NCBI_NS_NCBI::CRef< CDNode > >& CDNode_Base::SetC(void)
{
    m_set_State[0] |= 0x400;
    return m_C;
}

inline
CDKey_Base::E_Choice CDKey_Base::Which(void) const
{
    return m_choice;
}

inline
void CDKey_Base::CheckSelected(E_Choice index) const
{
    if ( m_choice != index )
        ThrowInvalidSelection(index);
}

inline
void CDKey_Base::Select(E_Choice index, NCBI_NS_NCBI::EResetVariant reset)
{
    if ( reset == NCBI_NS_NCBI::eDoResetVariant || m_choice != index ) {
        if ( m_choice != e_not_set )
            Reset();
        DoSelect(index);
    }
}

inline
bool CDKey_Base::IsId(void) const
{
    return m_choice == e_Id;
}

inline
int CDKey_Base::GetId(void) const
{
    CheckSelected(e_Id);
    return m_Id;
}

inline
int& CDKey_Base::SetId(void)
{
    Select(e_Id, NCBI_NS_NCBI::eDoNotResetVariant);
    return m_Id;
}

inline
void CDKey_Base::SetId(const int& value)
{
    Select(e_Id, NCBI_NS_NCBI::eDoNotResetVariant);
    m_Id = value;
}

inline
bool CDKey_Base::IsStr(void) const
{
    return m_choice == e_Str;
}

inline
const NCBI_NS_STD::string& CDKey_Base::GetStr(void) const
{
    CheckSelected(e_Str);
    return *m_string;
}

inline
NCBI_NS_STD::string& CDKey_Base::SetStr(void)
{
    Select(e_Str, NCBI_NS_NCBI::eDoNotResetVariant);
    return *m_string;
}

inline
void CDKey_Base::SetStr(const NCBI_NS_STD::string& value)
{
    Select(e_Str, NCBI_NS_NCBI::eDoNotResetVariant);
    *m_string = value;
}

inline
bool CDKey_Base::IsReal(void) const
{
    return m_choice == e_Real;
}

inline
double CDKey_Base::GetReal(void) const
{
    CheckSelected(e_Real);
    return m_Real;
}

inline
double& CDKey_Base::SetReal(void)
{
    Select(e_Real, NCBI_NS_NCBI::eDoNotResetVariant);
    return m_Real;
}

inline
void CDKey_Base::SetReal(const double& value)
{
    Select(e_Real, NCBI_NS_NCBI::eDoNotResetVariant);
    m_Real = value;
}

inline
CDData_Base::E_Choice CDData_Base::Which(void) const
{
    return m_choice;
}

inline
void CDData_Base::CheckSelected(E_Choice index) const
{
    if ( m_choice != index )
        ThrowInvalidSelection(index);
}

inline
void CDData_Base::Select(E_Choice index, NCBI_NS_NCBI::EResetVariant reset)
{
    if ( reset == NCBI_NS_NCBI::eDoResetVariant || m_choice != index ) {
        if ( m_choice != e_not_set )
            Reset();
        DoSelect(index);
    }
}

inline
bool CDData_Base::IsId(void) const
{
    return m_choice == e_Id;
}

inline
int CDData_Base::GetId(void) const
{
    CheckSelected(e_Id);
    return m_Id;
}

inline
int& CDData_Base::SetId(void)
{
    Select(e_Id, NCBI_NS_NCBI::eDoNotResetVariant);
    return m_Id;
}

inline
void CDData_Base::SetId(const int& value)
{
    Select(e_Id, NCBI_NS_NCBI::eDoNotResetVariant);
    m_Id = value;
}

inline
bool CDData_Base::IsStr(void) const
{
    return m_choice == e_Str;
}

inline
const NCBI_NS_STD::string& CDData_Base::GetStr(void) const
{
    CheckSelected(e_Str);
    return *m_string;
}

inline
NCBI_NS_STD::string& CDData_Base::SetStr(void)
{
    Select(e_Str, NCBI_NS_NCBI::eDoNotResetVariant);
    return *m_string;
}

inline
void CDData_Base::SetStr(const NCBI_NS_STD::string& value)
{
    Select(e_Str, NCBI_NS_NCBI::eDoNotResetVariant);
    *m_string = value;
}

inline
bool CDData_Base::IsReal(void) const
{
    return m_choice == e_Real;
}

inline
double CDData_Base::GetReal(void) const
{
    CheckSelected(e_Real);
    return m_Real;
}

inline
double& CDData_Base::SetReal(void)
{
    Select(e_Real, NCBI_NS_NCBI::eDoNotResetVariant);
    return m_Real;
}

inline
void CDData_Base::SetReal(const double& value)
{
    Select(e_Real, NCBI_NS_NCBI::eDoNotResetVariant);
    m_Real = value;
}

inline
bool CDData_Base::IsBool(void) const
{
    return m_choice == e_Bool;
}

inline
bool CDData_Base::GetBool(void) const
{
    CheckSelected(e_Bool);
    return m_Bool;
}

inline
bool& CDData_Base::SetBool(void)
{
    Select(e_Bool, NCBI_NS_NCBI::eDoNotResetVariant);
    return m_Bool;
}

inline
void CDData_Base::SetBool(const bool& value)
{
    Select(e_Bool, NCBI_NS_NCBI::eDoNotResetVariant);
    m_Bool = value;
}

inline
bool CDData_Base::IsOs(void) const
{
    return m_choice == e_Os;
}

inline
const NCBI_NS_STD::vector< char >& CDData_Base::GetOs(void) const
{
    CheckSelected(e_Os);
    return *m_Os;
}

inline
NCBI_NS_STD::vector< char >& CDData_Base::SetOs(void)
{
    Select(e_Os, NCBI_NS_NCBI::eDoNotResetVariant);
    return *m_Os;
}

inline
bool CDData_Base::IsStrs(void) const
{
    return m_choice == e_Strs;
}

inline
const NCBI_NS_STD::list< NCBI_NS_STD::string >& CDData_Base::GetStrs(void) const
{
    CheckSelected(e_Strs);
    return *m_Strs;
}

inline
NCBI_NS_STD::list< NCBI_NS_STD::string >& CDData_Base::SetStrs(void)
{
    Select(e_Strs, NCBI_NS_NCBI::eDoNotResetVariant);
    return *m_Strs;
}

inline
bool CDData_Base::IsIds(void) const
{
    return m_choice == e_Ids;
}

inline
const NCBI_NS_STD::list< int >& CDData_Base::GetIds(void) const
{
    CheckSelected(e_Ids);
    return *m_Ids;
}

inline
NCBI_NS_STD::list< int >& CDData_Base::SetIds(void)
{
    Select(e_Ids, NCBI_NS_NCBI::eDoNotResetVariant);
    return *m_Ids;
}

inline
bool CDData_Base::IsReals(void) const
{
    return m_choice == e_Reals;
}

inline
const NCBI_NS_STD::list< double >& CDData_Base::GetReals(void) const
{
    CheckSelected(e_Reals);
    return *m_Reals;
}

inline
NCBI_NS_STD::list< double >& CDData_Base::SetReals(void)
{
    Select(e_Reals, NCBI_NS_NCBI::eDoNotResetVariant);
    return *m_Reals;
}

inline
bool CDData_Base::IsOss(void) const
{
    return m_choice == e_Oss;
}

inline
const NCBI_NS_STD::list< NCBI_NS_STD::vector< char >* >& CDData_Base::GetOss(void) const
{
    CheckSelected(e_Oss);
    return *m_Oss;
}

inline
NCBI_NS_STD::list< NCBI_NS_STD::vector< char >* >& CDData_Base::SetOss(void)
{
    Select(e_Oss, NCBI_NS_NCBI::eDoNotResetVariant);
    return *m_Oss;
}

inline
bool CDData_Base::IsGen_memfree(void) const
{
    return m_choice == e_Gen_memfree;
}

inline
const NCBI_NS_STD::string& CDData_Base::GetGen_memfree(void) const
{
    CheckSelected(e_Gen_memfree);
    return *m_string;
}

inline
NCBI_NS_STD::string& CDData_Base::SetGen_memfree(void)
{
    Select(e_Gen_memfree, NCBI_NS_NCBI::eDoNotResetVariant);
    return *m_string;
}

inline
void CDData_Base::SetGen_memfree(const NCBI_NS_STD::string& value)
{
    Select(e_Gen_memfree, NCBI_NS_NCBI::eDoNotResetVariant);
    *m_string = value;
}

inline
bool CDData_Base::IsData_list(void) const
{
    return m_choice == e_Data_list;
}

inline
const NCBI_NS_STD::list< NCBI_NS_NCBI::CRef< CDData > >& CDData_Base::GetData_list(void) const
{
    CheckSelected(e_Data_list);
    return *m_Data_list;
}

inline
NCBI_NS_STD::list< NCBI_NS_NCBI::CRef< CDData > >& CDData_Base::SetData_list(void)
{
    Select(e_Data_list, NCBI_NS_NCBI::eDoNotResetVariant);
    return *m_Data_list;
}

inline
bool CDData_Base::IsUser_def(void) const
{
    return m_choice == e_User_def;
}

inline
const bool& CDData_Base::GetUser_def(void) const
{
    CheckSelected(e_User_def);
    return m_User_def;
}

inline
bool& CDData_Base::SetUser_def(void)
{
    Select(e_User_def, NCBI_NS_NCBI::eDoNotResetVariant);
    return m_User_def;
}

inline
void CDData_Base::SetUser_def(const bool& value)
{
    Select(e_User_def, NCBI_NS_NCBI::eDoNotResetVariant);
    m_User_def = value;
}

///////////////////////////////////////////////////////////
////////////////// end of inline methods //////////////////
///////////////////////////////////////////////////////////





END_slri_SCOPE // namespace slri::


#endif // OBJSLRILIBSTRUC_BASE_HPP