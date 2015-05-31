/*  $Id: seqdata_cmd.hpp,v 1.1 2003/11/21 21:11:42 haocl Exp $
 ***************************************************************************
  Copyright (C) 2001 Mount Sinai Hospital (MSH)
    
  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public License
  as published by the Free Software Foundation; either version 2.1 of
  the License, or any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

  See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with this program; if not, write to the
         Free Software Foundation, Inc.,
         59 Temple Place, Suite 330, Boston, MA
         02111-1307  USA
  or visit http://www.gnu.org/licenses/lgpl.txt

  SPECIAL EXCEPTIONS

  As a special exception, Mount Sinai Hospital gives permission to
  link this program with the following non-GPL programs or libraries,
  and distribute the resulting executable, without including the source
  code for these in the source distribution:

  a) CodeBase 6.5 or greater from Sequiter Software Inc.

  b) The C or C++ interface to Oracle 8.x or greater, from
     Oracle Corporation or IBM DB2 UDB.


  PROGRAM: SLRI C++ Seqhound WebAPI search functions.
        This defines the main section of the program.

        This software is based on and implements software
	developed by Lewis Geer, Vsevolod Sandomirskiy, etc.
        at the National Center for Biotechnology Information
	(NCBI) http://www.ncbi.nlm.nih.gov/

  AUTHORS: Hao Lieu


  REVISION/CONTRIBUTION NOTES:

  September 1, 2002  Original MSH Revision.
                Hogue Lab - University of Toronto Biochemistry
                Department and the Samuel Lunenfeld Research
                Institute, Mount Sinai Hospital
                http://bioinfo.mshri.on.ca  hogue@mshri.on.ca
                ftp://ftp.mshri.on.ca/

************************************************************************/

#ifndef SEQCMD__HPP
#define SEQCMD__HPP

#include "intrez.hpp"
#include "logic.hpp"
#include "seqdata_res.hpp"
#include <cgi/ncbires.hpp>
#include <cgi/cgiapp.hpp>
#include <vector>

BEGIN_SCOPE(shoundcgi)
USING_NCBI_SCOPE;
using namespace shound;
using namespace shoundlogic;

class shound::Intrez;    /* forward declaration, for compiler */

/* Abstract base class for seq command classes */
class CSeqCommand : public CNcbiCommand
{
public:
  
    CSeqCommand( CNcbiResource& resource );
    virtual ~CSeqCommand( void );

    virtual void Execute( CCgiContext& ctx );
    virtual string GetLink(CCgiContext&) const 
        { return NcbiEmptyString; }

    virtual CNCBINode* CreateView( CCgiContext& ctx ) = 0;

protected:

    CSeqResource& GetSeqResource() const
        { return dynamic_cast<CSeqResource&>( GetResource() ); }

    /* retrieves the value of the query string associated with the key */
    string GetQueryStringValue(CCgiContext& ctx, const string& key);

    /* hide operator= for this abstract class */
    CSeqCommand& operator=( const CSeqCommand& rhs );

    /* returns the string used to match the name in a cgi request.
       e.g. for "?cmd=search", GetEntry should return "cmd" */
    virtual string GetEntry() const;
};

/*************************************************************/
/* CSeqBasicCommand: welcome page */
/*************************************************************/
class CSeqBasicCommand : public CSeqCommand
{
public:

    CSeqBasicCommand( CNcbiResource& resource );
    virtual ~CSeqBasicCommand( void );

    virtual CNcbiCommand* Clone( void ) const;

    /* GetName() returns the string used to match the name in a cgi request.
       e.g. for "?cmd=search", GetName() should return "search" */
    virtual string GetName( void ) const;
    CNCBINode* CreateQuickSearch( CCgiContext& ctx );
    virtual CNCBINode* CreateView( CCgiContext& ctx );
    virtual void Execute( CCgiContext& ctx );
protected:

    /* creates a html text object of "&nbsp;" concated together, 'num'
     * specifies how many of the '&nbsp'. Use this for small white spaces
     * between data cells. */
    CNCBINode* nbsp(const int num);

    /* creates a datacell of specified 'CellHeight', width (as a percentage, eg 25%)
     * 'centered' specifies if content is centered, CellContent is the
     * pointer to the object to be put into the datacell */
    CNCBINode* CreateDataCell(const string CellWidth, const int CellHeight, 
		    	      const bool centered, CNCBINode* CellContent);

    /* takes an array (DataCell) of content contained in <td> </td> tags.
     * creates a row containing the cells, of background color BgColor.
     * Array size of ArraySize. Returns the row. Row size gets
     * specified by the size of the data cells. */
    CNCBINode* CreateRow(CNCBINode* *, const int ArraySize, const string BgColor);

    /* creates the fetch row that specifies the format and return format */ 
    CNCBINode* CreateFetchRow(void);


    /* creates a HTML radio button SET in a data cell as specified in
     * the array TypeArray (contains strings of all names of radio button), of
     * array size SizeArray. name is the name of the radio buttons,
     * CellWidth (given as a xx% string) and CellHeight are dimensions for cell,
     * selected is the selected button, functional is onClick="searchupdate(document);"
     * gives JavaScript functionality, may decide to make this more generic later on. */
    CNCBINode* CreateRadioButtonsInDataCell
       (const string* TypeArray, const int SizeArray, const string name, 
	const string CellWidth, const int CellHeight, const int selected=4, 
	const bool functional=true);

    /* creates the default 1st row of the advanced search table
     * on load up. 'sel1' & 'sel2' defaults to 0, 'sel3' defaults to 4,
     * 'id' defaults to 'Enter ID', 'functional' defaults to 'true',
     */
    CNCBINode* CreateDefaultRow
       (const eMolType mol_sel=eProtein, const eQueryType idtype_sel=eQuery_TaxID, 
	const SHoundLogic::eLogic logic_sel=SHoundLogic::eUND, 
	string id="Enter ID", bool functional=true);

    /* creates an HTML select element containing the options as specified in
     * the array pointed to by TypeArray (of size SizeArray), select name is name,
     * multi specifies if select is a multi select (true) or not (false).
     * selected specifies which option is selected, defaults to 0 */
    CNCBINode* CreateSelect
       (const string* TypeArray, const int sizeArray, const string selectName, 
	const int selected=0, const bool multiSelect=false);

    /* creates parametered search row */ 
    CNCBINode* CreateParameteredSearchRow
       (SHoundLogic::eLogic last_logic, int iteration, const eQueryType idtype=eQuery_TaxID, 
	string id="Enter Another ID", const SHoundLogic::eLogic log=SHoundLogic::eUND, 
	bool functional=true);

    CNCBINode* CreateTableTitleInDataCell(const string CellWidth, const string title);

    /* creats a link to an external website */
    CNCBINode* CreateExternalLink(string url, string text, string target);
    CNCBINode* ExternalLinkCheck(const eQueryType eIDType);
    virtual CNCBINode* CreateErrorView( CCgiContext& ctx );
    CNCBINode* GOFields( CCgiContext& ctx, int iteration);
private:
    /* use variables for table dimension. Keeps a uniform look & feel */
    static const int iTableTitleHeight = 20;   /* height of row containing the table title */
    static const int iTableRowHeight = 30;     /* height of table row, I fix it to keep look uniform */
    static const int iIDTextBoxSize = 20;      /* size of the id text box */
   
    /* height of short rows that hold external links and GOID parameters */ 
    static const int iTableRowHeightShort = 10;
    string pTableColor;
    string taxonURL;
    string pubmedURL;
    
    /* use variables for the array size to make updates easy, 
     * values are set in the constructor */
    static const int iMoleArraySize = 2;      /* size of the molecules array */
    static const int iIDTypeArraySize = 8;    /* size of the idtype array */
    static const int iLogicArraySize = 3;     /* size of the logic array */
    static const int iFormatArraySize = 6;    /* size of the format array */
    static const int iSizeArraySize = 6;      /* size of the size array */
    static const int iEcodeArraySize = 13;    /* size of the ecode array */
    static const int iDegreesArraySize = 7;   /* size of the degree array */
    static const int iQuickKeysArraySize = 4;

#if 0
    static string molecules[2];
    static string idtype[8];
    static string logic[3];
    static string format[6];
    static string size[6];
    static string ecode[13];
    static string degrees[7];
    static string quickkeys[4];
#endif
};
static const string molecules[2] = {"Protein", "Nucleic Acid"};
static const string idtype[8] = {"Taxon ID", "PubMed ID", "MMDB ID",
                                 "Chromosome ID", "GO ID", "OMIM ID",
                                 "CDD ID", "LocusLink ID"};
static const string logic[3] = {"AND", "OR", "NOT"};
static const string format[6] = {"Defline", "FASTA", "ASN.1", "XML", "GenBank", "SeqHound Report"};
static const string size[6] = {"5", "10", "20", "50", "75", "100"};
static const string ecode[13] = {"none", "IC", "IDA", "IEA", "IEP", "IGI", "IMP", 
	                        "IPI", "ISS", "NAS", "ND", "TAS", "NR"};
static const string degrees[7] = {"0", "1", "2", "3", "4", "5", "6"};
static const string quickkeys[4] = {"Gen Info ID", "Accession", "Protein Name", "Gene NAme"};
/*******************************************************************/
/* CSeqAdvancedCommand */
/*******************************************************************/
class CSeqAdvancedCommand : public CSeqBasicCommand
{
public:

    CSeqAdvancedCommand( CNcbiResource& resource );
    virtual ~CSeqAdvancedCommand( void );

    virtual CNcbiCommand* Clone( void ) const;
    virtual void Execute( CCgiContext& ctx );

    /* GetName() returns the string used to match the name in a cgi request.
       e.g. for "?cmd=search", GetName() should return "search" */
    virtual string GetName( void ) const;
    virtual CNCBINode* CreateView( CCgiContext& ctx );
protected:
private:
    CNCBINode* CreateSpacer(CCgiContext&);
};

/****************************************************************/
/* CSeqAdvancedGOCommand */
/****************************************************************/

class CSeqAdvancedGOCommand : public CSeqBasicCommand
{
public:

    CSeqAdvancedGOCommand( CNcbiResource& resource );
    virtual ~CSeqAdvancedGOCommand( void );

    virtual CNcbiCommand* Clone( void ) const;
    virtual void Execute( CCgiContext& ctx );

    /* GetName() returns the string used to match the name in a cgi request.
       e.g. for "?cmd=search", GetName() should return "search" */
    virtual string GetName( void ) const;
    virtual CNCBINode* CreateView( CCgiContext& ctx );
protected:
private:
    CNCBINode* CreateSpacer(CCgiContext&);
};





END_SCOPE(shoundcgi)
#endif

/* 
 * $Log: seqdata_cmd.hpp,v $
 * Revision 1.1  2003/11/21 21:11:42  haocl
 * major refactoring, splitting cgi into 2 executables, renaming files
 * from seqxxx to seqdata, renaming Makefile
 *
 * Revision 1.1.1.5  2003/08/21 08:48:07  root
 * Automated import from Sourceforge.net
 *
 * Revision 1.5  2003/08/20 19:33:44  haocl
 * check in for switchover
 *
 * Revision 1.4  2003/08/18 19:17:09  haocl
 * minor changes, got rid of some magic numbers,
 * changed returned list to passed in references
 *
 * Revision 1.3  2003/08/08 16:22:43  haocl
 * new files shoundrecords and changes to others
 *
 * Revision 1.2  2003/08/06 21:24:52  haocl
 * added XML, ASN processing
 *
 * Revision 1.1  2003/07/31 21:16:38  haocl
 * checking in portion of the new web interface
 */ 


