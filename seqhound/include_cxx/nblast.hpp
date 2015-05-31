/* $Id: nblast.hpp,v 1.1.1.1 2003/02/07 09:32:24 root Exp $
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
 *   'NBlastasn.asn'.
 */

#ifndef NBLAST_HPP
#define NBLAST_HPP


// generated includes
#include <nblast_.hpp>

// generated classes

BEGIN_seqhound_SCOPE // namespace seqhound::

class CNBlast_Result_Set : public CNBlast_Result_Set_Base
{
    typedef CNBlast_Result_Set_Base Tparent;
public:
    // constructor
    CNBlast_Result_Set(void);
    // destructor
    ~CNBlast_Result_Set(void);

private:
    // Prohibit copy constructor and assignment operator
    CNBlast_Result_Set(const CNBlast_Result_Set& value);
    CNBlast_Result_Set& operator=(const CNBlast_Result_Set& value);

};



/////////////////// CNBlast_Result_Set inline methods

// constructor
inline
CNBlast_Result_Set::CNBlast_Result_Set(void)
{
}


/////////////////// end of CNBlast_Result_Set inline methods


class CNBlast_Scores : public CNBlast_Scores_Base
{
    typedef CNBlast_Scores_Base Tparent;
public:
    // constructor
    CNBlast_Scores(void);
    // destructor
    ~CNBlast_Scores(void);

private:
    // Prohibit copy constructor and assignment operator
    CNBlast_Scores(const CNBlast_Scores& value);
    CNBlast_Scores& operator=(const CNBlast_Scores& value);

};



/////////////////// CNBlast_Scores inline methods

// constructor
inline
CNBlast_Scores::CNBlast_Scores(void)
{
}


/////////////////// end of CNBlast_Scores inline methods


class CNBlast_Result : public CNBlast_Result_Base
{
    typedef CNBlast_Result_Base Tparent;
public:
    // constructor
    CNBlast_Result(void);
    // destructor
    ~CNBlast_Result(void);

private:
    // Prohibit copy constructor and assignment operator
    CNBlast_Result(const CNBlast_Result& value);
    CNBlast_Result& operator=(const CNBlast_Result& value);

};



/////////////////// CNBlast_Result inline methods

// constructor
inline
CNBlast_Result::CNBlast_Result(void)
{
}


/////////////////// end of CNBlast_Result inline methods


class CNBlast_GiAndEval_set : public CNBlast_GiAndEval_set_Base
{
    typedef CNBlast_GiAndEval_set_Base Tparent;
public:
    // constructor
    CNBlast_GiAndEval_set(void);
    // destructor
    ~CNBlast_GiAndEval_set(void);

private:
    // Prohibit copy constructor and assignment operator
    CNBlast_GiAndEval_set(const CNBlast_GiAndEval_set& value);
    CNBlast_GiAndEval_set& operator=(const CNBlast_GiAndEval_set& value);

};



/////////////////// CNBlast_GiAndEval_set inline methods

// constructor
inline
CNBlast_GiAndEval_set::CNBlast_GiAndEval_set(void)
{
}


/////////////////// end of CNBlast_GiAndEval_set inline methods


class CNBlast_GiAndEval : public CNBlast_GiAndEval_Base
{
    typedef CNBlast_GiAndEval_Base Tparent;
public:
    // constructor
    CNBlast_GiAndEval(void);
    // destructor
    ~CNBlast_GiAndEval(void);

private:
    // Prohibit copy constructor and assignment operator
    CNBlast_GiAndEval(const CNBlast_GiAndEval& value);
    CNBlast_GiAndEval& operator=(const CNBlast_GiAndEval& value);

};



/////////////////// CNBlast_GiAndEval inline methods

// constructor
inline
CNBlast_GiAndEval::CNBlast_GiAndEval(void)
{
}


/////////////////// end of CNBlast_GiAndEval inline methods


END_seqhound_SCOPE // namespace seqhound::


/*
* ===========================================================================
*
* $Log: nblast.hpp,v $
* Revision 1.1.1.1  2003/02/07 09:32:24  root
* Automated import from Sourceforge.net
*
* Revision 1.1  2003/02/06 15:46:16  haocl
* new nblast and slristruc files and modifications to reflect new files
*
*
* ===========================================================================
*/

#endif // NBLAST_HPP
/* Original file checksum: lines: 209, chars: 4598, CRC32: 95b9de9c */