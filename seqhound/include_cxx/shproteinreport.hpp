/* $Id: shproteinreport.hpp,v 1.2 2003/09/29 22:18:49 haocl Exp $
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
 * Author:  .......
 *
 * File Description:
 *   .......
 *
 * Remark:
 *   This code was originally generated by application DATATOOL
 *   using specifications from the data definition file
 *   'slristruc.asn'.
 */

#ifndef SHPROTEINREPORT_HPP
#define SHPROTEINREPORT_HPP


// generated includes
#include <shproteinreport_.hpp>

// generated classes

BEGIN_shoundcgi_SCOPE // namespace shoundcgi::

class CSHProteinReportSetDescription : public CSHProteinReportSetDescription_Base
{
    typedef CSHProteinReportSetDescription_Base Tparent;
public:
    // constructor
    CSHProteinReportSetDescription(void);
    // destructor
    ~CSHProteinReportSetDescription(void);

private:
    // Prohibit copy constructor and assignment operator
    CSHProteinReportSetDescription(const CSHProteinReportSetDescription& value);
    CSHProteinReportSetDescription& operator=(const CSHProteinReportSetDescription& value);

};



/////////////////// CSHProteinReportSetDescription inline methods

// constructor
inline
CSHProteinReportSetDescription::CSHProteinReportSetDescription(void)
{
}


/////////////////// end of CSHProteinReportSetDescription inline methods


class CSHProteinReportSet : public CSHProteinReportSet_Base
{
    typedef CSHProteinReportSet_Base Tparent;
public:
    // constructor
    CSHProteinReportSet(void);
    // destructor
    ~CSHProteinReportSet(void);

private:
    // Prohibit copy constructor and assignment operator
    CSHProteinReportSet(const CSHProteinReportSet& value);
    CSHProteinReportSet& operator=(const CSHProteinReportSet& value);

};



/////////////////// CSHProteinReportSet inline methods

// constructor
inline
CSHProteinReportSet::CSHProteinReportSet(void)
{
}


/////////////////// end of CSHProteinReportSet inline methods


class CSHProteinReport : public CSHProteinReport_Base
{
    typedef CSHProteinReport_Base Tparent;
public:
    // constructor
    CSHProteinReport(void);
    // destructor
    ~CSHProteinReport(void);

private:
    // Prohibit copy constructor and assignment operator
    CSHProteinReport(const CSHProteinReport& value);
    CSHProteinReport& operator=(const CSHProteinReport& value);

};



/////////////////// CSHProteinReport inline methods

// constructor
inline
CSHProteinReport::CSHProteinReport(void)
{
}


/////////////////// end of CSHProteinReport inline methods


class CSHNeighbour : public CSHNeighbour_Base
{
    typedef CSHNeighbour_Base Tparent;
public:
    // constructor
    CSHNeighbour(void);
    // destructor
    ~CSHNeighbour(void);

private:
    // Prohibit copy constructor and assignment operator
    CSHNeighbour(const CSHNeighbour& value);
    CSHNeighbour& operator=(const CSHNeighbour& value);

};



/////////////////// CSHNeighbour inline methods

// constructor
inline
CSHNeighbour::CSHNeighbour(void)
{
}


/////////////////// end of CSHNeighbour inline methods


class CSHLocusProduct : public CSHLocusProduct_Base
{
    typedef CSHLocusProduct_Base Tparent;
public:
    // constructor
    CSHLocusProduct(void);
    // destructor
    ~CSHLocusProduct(void);

private:
    // Prohibit copy constructor and assignment operator
    CSHLocusProduct(const CSHLocusProduct& value);
    CSHLocusProduct& operator=(const CSHLocusProduct& value);

};



/////////////////// CSHLocusProduct inline methods

// constructor
inline
CSHLocusProduct::CSHLocusProduct(void)
{
}


/////////////////// end of CSHLocusProduct inline methods


class CSHLocusLinkReport : public CSHLocusLinkReport_Base
{
    typedef CSHLocusLinkReport_Base Tparent;
public:
    // constructor
    CSHLocusLinkReport(void);
    // destructor
    ~CSHLocusLinkReport(void);

private:
    // Prohibit copy constructor and assignment operator
    CSHLocusLinkReport(const CSHLocusLinkReport& value);
    CSHLocusLinkReport& operator=(const CSHLocusLinkReport& value);

};



/////////////////// CSHLocusLinkReport inline methods

// constructor
inline
CSHLocusLinkReport::CSHLocusLinkReport(void)
{
}


/////////////////// end of CSHLocusLinkReport inline methods


class CSHGoReport : public CSHGoReport_Base
{
    typedef CSHGoReport_Base Tparent;
public:
    // constructor
    CSHGoReport(void);
    // destructor
    ~CSHGoReport(void);

private:
    // Prohibit copy constructor and assignment operator
    CSHGoReport(const CSHGoReport& value);
    CSHGoReport& operator=(const CSHGoReport& value);

};



/////////////////// CSHGoReport inline methods

// constructor
inline
CSHGoReport::CSHGoReport(void)
{
}


/////////////////// end of CSHGoReport inline methods


class CSHGoAnnotNode : public CSHGoAnnotNode_Base
{
    typedef CSHGoAnnotNode_Base Tparent;
public:
    // constructor
    CSHGoAnnotNode(void);
    // destructor
    ~CSHGoAnnotNode(void);

private:
    // Prohibit copy constructor and assignment operator
    CSHGoAnnotNode(const CSHGoAnnotNode& value);
    CSHGoAnnotNode& operator=(const CSHGoAnnotNode& value);

};



/////////////////// CSHGoAnnotNode inline methods

// constructor
inline
CSHGoAnnotNode::CSHGoAnnotNode(void)
{
}


/////////////////// end of CSHGoAnnotNode inline methods


class CSHGoAnnot : public CSHGoAnnot_Base
{
    typedef CSHGoAnnot_Base Tparent;
public:
    // constructor
    CSHGoAnnot(void);
    // destructor
    ~CSHGoAnnot(void);

private:
    // Prohibit copy constructor and assignment operator
    CSHGoAnnot(const CSHGoAnnot& value);
    CSHGoAnnot& operator=(const CSHGoAnnot& value);

};



/////////////////// CSHGoAnnot inline methods

// constructor
inline
CSHGoAnnot::CSHGoAnnot(void)
{
}


/////////////////// end of CSHGoAnnot inline methods


END_shoundcgi_SCOPE // namespace shoundcgi::


/*
* ===========================================================================
*
* $Log: shproteinreport.hpp,v $
* Revision 1.2  2003/09/29 22:18:49  haocl
* checkin after newly generated
*
*
* ===========================================================================
*/

#endif // SHPROTEINREPORT_HPP
/* Original file checksum: lines: 329, chars: 6779, CRC32: 8baebccb */
