/*  $Id: seqdata_cmd.cpp,v 1.1 2003/11/21 21:11:42 haocl Exp $
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


  PROGRAM: SLRI C++ Seqhound WebAPI search functions


  AUTHORS: Hao Lieu


  REVISION/CONTRIBUTION NOTES:

  September 1, 2002  Original MSH Revision.
                Hogue Lab - University of Toronto Biochemistry
                Department and the Samuel Lunenfeld Research
                Institute, Mount Sinai Hospital
                http://bioinfo.mshri.on.ca  hogue@mshri.on.ca
                ftp://ftp.mshri.on.ca/


  PUBLICATION TO CITE:

************************************************************************/

#include <corelib/ncbistr.hpp>
#include <corelib/ncbiexpt.hpp>
#include <corelib/ncbistd.hpp>
#include <cgi/cgictx.hpp>
#include <corelib/ncbireg.hpp>
#include <html/page.hpp>
#include <html/node.hpp>
#include <html/html.hpp>
#include <html/jsmenu.hpp>
#include <corelib/ncbiobj.hpp>
#include <cgi/cgictx.hpp>
#include <sstream>
#include <memory>
#include <asn.h>
#include <ncbi.h>
#include <objsset.h>
#include <seqport.h>
#include <asn2ff.h>
#include <asn2ffg.h>
#include <seqhound.h>
#include "seqdata_res.hpp"
#include "seqdata_cmd.hpp"
#include "intrez.hpp"
#include "query.hpp"
#include "logic.hpp"
#include "shoundtypes.hpp"
#include <objslrilibstruc.hpp>

/* The cgi for seqhound web interface. Cgi implemented using 
 * NCBI's C++ API for HTML code generation & CGI generation
 */

/* Declaring the cgi in its own namespace shoundcgi. To create
 * applications that use this source code, you will need to 
 * import this namesapce. eg: 
 *   using namespace std;
 *   using namespace shoundcgi; */

USING_NCBI_SCOPE;
using namespace objects;
using namespace shound;
using namespace shoundlogic;
using namespace shoundquery;
using namespace slri;
BEGIN_SCOPE(shoundcgi)      /* set the cgi in its own namespace 'shoundcgi' */


CSeqCommand::CSeqCommand( CNcbiResource& resource )
  : CNcbiCommand( resource )
{}

CSeqCommand::~CSeqCommand()
{}

/* GetEntry() returns the string used to match the name in a cgi request.
 * e.g. for "?cmd=search", GetEntry should return "cmd" */
string CSeqCommand::GetEntry() const
{
    return string("cmd");  /* set the value of this string in seqapi.cpp */
}

void CSeqCommand::Execute( CCgiContext& ctx )
{
    const CNcbiRegistry& reg = ctx.GetConfig();
    
    /* load in the html template file */
    string baseFile = reg.Get( "filesystem", "HtmlBaseFile" );
    auto_ptr<CHTMLPage> page( new CHTMLPage( NcbiEmptyString, baseFile ) );
    
    /* set up to replace <@VIEW@> in template file with html returned
       from CreateView */
    /* page->AddTagMap( "POPMENU", CreatePopUp(ctx) ); */
    page->AddTagMap( "VIEW", CreateView( ctx ) );
    /* actual page output */
    ctx.GetResponse().WriteHeader();
    page->Print(ctx.GetResponse().out(), CNCBINode::eHTML );
}

/* returns the string 'value' portion of the 'key=value' pairing 
 * in the querystring. */
inline string CSeqCommand::GetQueryStringValue(CCgiContext& ctx, const string &key)
{
   TCgiEntries& entries = const_cast<TCgiEntries&> (ctx.GetRequest().GetEntries());
   pair<TCgiEntriesI, TCgiEntriesI> Map = entries.equal_range(key);
   string value = kEmptyStr; 
   for(TCgiEntriesI i = Map.first; i != Map.second; ++i)
   {
      value = i->second;
   }
   return value;
}

/**************************** CSeqBasicCommand **************************/
#if 0
string CSeqBasicCommand::molecules[2]   = {"Protein", "Nucleic Acid"};
string CSeqBasicCommand::idtype[8]      = {"Taxon ID", "PubMed ID", "MMDB ID",
                                           "Chromosome ID", "GO ID", "OMIM ID",
                                           "CDD ID", "LocusLink ID"};
string CSeqBasicCommand::logic[3]       = {"AND", "OR", "NOT"};
string CSeqBasicCommand::format[6]      = {"Defline", "FASTA", "ASN.1", "XML", "GenBank", "SeqHound Report"};
string CSeqBasicCommand::size[6]        = {"5", "10", "20", "50", "75", "100"};
string CSeqBasicCommand::ecode[13]      = {"none", "IC", "IDA", "IEA", "IEP", "IGI", "IMP", 
	                                   "IPI", "ISS", "NAS", "ND", "TAS", "NR"};
string CSeqBasicCommand::degrees[7]     = {"0", "1", "2", "3", "4", "5", "6"};
string CSeqBasicCommand::quickkeys[4]   = {"Gen Info ID", "Accession", "Protein Name", "Gene NAme"};
#endif


/* SeqBasicCommand is the SeqCommand class responsible for displaying the 
 * default home web page when a user first loads up the SeqHound website. */
CSeqBasicCommand::CSeqBasicCommand( CNcbiResource& resource )
  : CSeqCommand( resource )
{
   /* the exernal links to ncbi websites */
   taxonURL = "http://www.ncbi.nlm.nih.gov/Taxonomy/taxonomyhome.html"; 
   pubmedURL = "http://www.ncbi.nlm.nih.gov/entrez/query.fcgi?db=PubMed";

   /* color of the table */
   pTableColor = "D8D7D1";
}

CSeqBasicCommand::~CSeqBasicCommand( void )
{}

CNcbiCommand* CSeqBasicCommand::Clone( void ) const
{
    return new CSeqBasicCommand( GetSeqResource() );
}

/*
 * GetName() returns the string used to match the value in a cgi request.
 * e.g. for "?cmd=search", GetName() should return "search" */
inline string CSeqBasicCommand::GetName( void ) const
{
    /* the initiation class must return "init", the cgi context
     * class recognizes the initiation class by this function. */
    return string("init");
}

/* creates an HTML select element containing the options as specified in
 * the 'TypeArray' (of size 'SizeArray'), the name of the element is 'name',
 * 'multi' specifies if select is a multi-select (true) or not (false).
 * 'selected' specifies which option is selected, defaults to 0 */
CNCBINode* CSeqBasicCommand::CreateSelect
    (const string* TypeArray, const int SizeArray, const string name, 
     const int selected, const bool multi)
{
   auto_ptr<CHTML_select> SelectBox(new CHTML_select(name, multi));
   for(int i = 0; i < SizeArray; i++)
   {  
      /* selects the selected button */
      if(i == selected)
	 SelectBox->AppendOption(NStr::IntToString(i), TypeArray[i], true);
      else
	 SelectBox->AppendOption(NStr::IntToString(i), TypeArray[i], false);
   }
   return SelectBox.release();
}

/* creates a set of HTML radio buttons in a data cell. The values of the 
 * button are specified by TypeArray (contains strings of all names of radio button), 
 * of array size SizeArray. 'name' is the name of the radio buttons, 
 * 'CellWidth' (given as a xx% string) and 'CellHeight' are dimensions for cell,
 * 'selected' is the selected button, 'functional' is onClick="searchupdate(document);"
 * gives JavaScript functionality, may decide to make this more generic later on. */
CNCBINode* CSeqBasicCommand::CreateRadioButtonsInDataCell
     (const string* TypeArray, const int SizeArray, const string name, 
      const string CellWidth, const int CellHeight, const int selected,  
      const bool functional)
{
    auto_ptr<CHTML_tc> TData(new CHTML_tc("td"));
    TData->SetWidth(CellWidth)->SetHeight(CellHeight);
    CHTML_radio *button;
    for(int i = 0; i < SizeArray; i++)
    {
       if(i == selected)
       {  /* selects the selected button */
          button = new CHTML_radio(name, NStr::IntToString(i), true, TypeArray[i]);
       }else
       {  /* un-selected buttons */
	  button = new CHTML_radio(name, NStr::IntToString(i), false, TypeArray[i]);
       }
       if(functional) /* functional buttons activated */
	  button->SetAttribute("onClick", "searchupdate(document);");
       TData->AppendChild(button);
    }
    /* return the select element */    
    return TData.release();
}

/* creates a datacell that contains a title for a table. 
 * The title is enclosed in a span tag, the width of the datacell 
 * is specified by the user. Height is fixed at 20.*/
CNCBINode* 
CSeqBasicCommand::CreateTableTitleInDataCell(const string CellWidth, const string title)
{
    auto_ptr<CHTML_tc> Span(new CHTML_tc("span"));
    Span->SetAttribute("class", "H1");
    Span->AppendHTMLText(title);
    return CreateDataCell(CellWidth, iTableTitleHeight, false, Span.release());
}


CNCBINode* CSeqBasicCommand::CreateErrorView(CCgiContext& ctx)
{
    auto_ptr<CHTML_table> pTable(new CHTML_table);
    auto_ptr<CHTML_tr> pRow(new CHTML_tr("Error"));
    pTable->AppendChild(pRow.release());
    return pTable.release();
}

/* creates a datacell of specified 'CellHeight', width (as a percentage, eg 25%)
 * 'centered' specifies if content is centered, CellContent is the
 * pointer to the object to be put into the datacell */
CNCBINode* 
CSeqBasicCommand::CreateDataCell(const string CellWidth, const int CellHeight, 
		                 const bool centered, CNCBINode* CellContent)
{
   auto_ptr<CHTML_tc> Data(new CHTML_tc("td"));
   Data->SetWidth(CellWidth)->SetHeight(CellHeight);
   if(centered)
      Data->SetAlign("center");
   Data->AppendChild(CellContent);
   return Data.release();
}

/* takes an array (DataCell) of content contained in <td> </td> tags.
 * creates a row containing the cells, of background color BgColor.
 * Array size of ArraySize. Returns the row. Row size gets
 * specified by the size of the data cells. */
CNCBINode* 
CSeqBasicCommand::CreateRow(CNCBINode* *DataCells, const int ArraySize,  
		            const string BgColor)
{
   auto_ptr<CHTML_tr> Row(new CHTML_tr);
   /* each element in the array is added to the row */
   for(int i = 0; i < ArraySize; i++)
   {
     Row->AppendChild(DataCells[i]);
   }
   Row->SetBgColor(BgColor);
   /* return the row */
   return Row.release();
}

/* creates a html text object of "&nbsp;" concated together, num 
 * specifies how many of the '&nbsp'. Use this for small white spaces
 * between data cells. */
CNCBINode* CSeqBasicCommand::nbsp(const int num)
{
   auto_ptr<CHTMLText> Data(new CHTMLText(""));
   for(int i = 0; i < num; i++)
   {
     Data->AppendChild(new CHTML_nbsp);
   }
   return Data.release();
}

/* creates the default 1st row of the advanced search table 
 * on load up. 'sel1' & 'sel2' defaults to 0, 'sel3' defaults to 4,
 * 'id' defaults to 'Enter ID', 'functional' defaults to 'true',
 * see seqcmd.hpp for more info. */
CNCBINode* CSeqBasicCommand::CreateDefaultRow
   (const eMolType Sel_Mol, const eQueryType Sel_Type, 
    const SHoundLogic::eLogic Sel_Log, string id, bool functional)
{
   const int iNodeSize = 4;
   CNCBINode* NodeArray[iNodeSize];
   auto_ptr<CHTMLText> Data(new CHTMLText("Retrieve"));
   Data->AppendChild(nbsp(4));

   /* molecules is a global static string array that holds 
    * the types of molecules that can be searched for, 'Nucleic Acid' & 'Protein' */
   Data->AppendChild(CreateSelect(molecules, iMoleArraySize, "mol", static_cast<int>(Sel_Mol)));
   NodeArray[0] = CreateDataCell("25%", iTableRowHeight, true, Data.release());
   
   auto_ptr<CHTMLText> Data1(new CHTMLText("using"));
   /* add a few spaces for readability */
   Data1->AppendChild(nbsp(4));

   /* idtype is a global string array that holds the types of ID that 
    * can be used, includes 'Taxon ID', 'PubMed ID', 'MMDB ID', 
    * 'Chromosome ID', 'GO ID', "OMIM ID', 'CDD ID', & 'LocusLink ID' */
   CNCBINode* Select = CreateSelect(idtype, iIDTypeArraySize, "idtype0", static_cast<int>(Sel_Type));
   Select->SetAttribute("onChange","onSelect(this);");
   Data1->AppendChild(Select);
   NodeArray[1] = CreateDataCell("25%", iTableRowHeight, true, Data1.release());
   
   /* create a text field called 'id0' of size 20.*/
   CHTML_text* Text = new CHTML_text("id0", iIDTextBoxSize, id);
  /* when the user clicks on the field to enter data, the 
   * entire box gets selected using JavaScripts 'select()' function. */ 
   Text->SetAttribute("onfocus", "select();");
   NodeArray[2] = CreateDataCell("25%", iTableRowHeight, false, Text);
   
   /* create a radio button box (width = 25%, height = 30):
    * 'logic' is a string array that specifies the appliable logic: 'AND', 'OR', 'NOT'.
    * sel3 = 4, so none of buttons are selected. */
   NodeArray[3] = CreateRadioButtonsInDataCell(logic, iLogicArraySize, "logic0", "25%", iTableRowHeight, Sel_Log, functional);
   /* create a row of all the objects. */
   return CreateRow(NodeArray, iNodeSize, pTableColor);
}

/* creates a search row with parameters specified. Parameters include
 * the value of the last logic, a select element (based on iIdtype),  
 * a logic button (based on iLogic). */
CNCBINode* CSeqBasicCommand::CreateParameteredSearchRow
   (const SHoundLogic::eLogic eLogLast, int iteration, const eQueryType eIDType, 
    string id, const SHoundLogic::eLogic eLog, bool functional)
{
   static const int iNodeSize = 4;
   CNCBINode* NodeArray[iNodeSize]; 
   NodeArray[0] = CreateDataCell("25%", iTableRowHeight, 
		   true, new CHTMLText(logic[eLogLast]));

   /* the select element is added to the row */
   auto_ptr<CHTMLText> Data(new CHTMLText(""));
   CNCBINode* Select = CreateSelect(idtype, iIDTypeArraySize, "idtype"+NStr::IntToString(iteration), static_cast<int>(eIDType));
   Select->SetAttribute("onChange", "onSelect(this);");
   Data->AppendChild(Select);
   NodeArray[1] = CreateDataCell("25%", iTableRowHeight, true, Data.release());

   CHTML_text* Text = new CHTML_text("id"+NStr::IntToString(iteration), iIDTextBoxSize, id);
   Text->SetAttribute("onfocus", "select();");
   NodeArray[2] = CreateDataCell("25%", iTableRowHeight, false, Text);
   
   NodeArray[3] = CreateRadioButtonsInDataCell(logic, iLogicArraySize, "logic"+NStr::IntToString(iteration), "25%", iTableRowHeight, eLog, functional);

   return CreateRow(NodeArray, iNodeSize, pTableColor);
}


/* creates the fetch row that specifies the format and return format */
CNCBINode* CSeqBasicCommand::CreateFetchRow(void)
{
   static const int iNodeSize = 4;
   CNCBINode* NodeArray[iNodeSize];
   
   NodeArray[0] = CreateDataCell("25%", iTableRowHeight, true, new CHTMLText("Return formatted as "));

   /* a select form allowing user to specify the return format:
    * possible format include: 'Defline', 'FASTA', ASN.1', 'XML', 'GenBank' & 'SeqHound Report' */
   auto_ptr<CHTMLText> Data(new CHTMLText(""));
   Data->AppendChild(CreateSelect(format, iFormatArraySize, "format"));
   Data->AppendChild(nbsp(3));
   Data->AppendChild(new CHTMLText("With"));
   NodeArray[1] = CreateDataCell("25%", iTableRowHeight, false, Data.release());
   
   /* a select form: how many results per page: '5', '10', '20', '50', '75', '100'. */
   auto_ptr<CHTMLText> Data2(new CHTMLText(""));
   Data2->AppendChild(CreateSelect(size, iSizeArraySize, "size"));
   Data2->AppendChild(nbsp(3));
   Data2->AppendChild(new CHTMLText("results per page"));
   NodeArray[2] = CreateDataCell("25%", iTableRowHeight, false, Data2.release());

   /* the submit button */
   auto_ptr<CHTMLText> Data3(new CHTMLText(""));
   auto_ptr<CHTML_submit> FetchSubmit(new CHTML_submit("submit", "Fetch"));
   FetchSubmit->SetEventHandler(eHTML_EH_Click, "someFunction();");
   Data3->AppendChild( FetchSubmit.release() );
   NodeArray[3] = CreateDataCell("25%", iTableRowHeight, false, Data3.release());

   /* put all the above into a row. */
   return CreateRow(NodeArray, iNodeSize, pTableColor);
}

CNCBINode* CSeqBasicCommand::CreateExternalLink(string url, string text, string target)
{
   static const int iNodeSize = 4;
   CNCBINode* NodeArray[iNodeSize];
   /* add an anchor to open in a new window */
   CHTML_a* Anchor = new CHTML_a(url, text);
   Anchor->SetAttribute("target", target);
   auto_ptr<CHTML_font> Font(new CHTML_font(-1, Anchor)); 
   NodeArray[0] = CreateDataCell("25%", iTableRowHeightShort, true, new CHTMLText(""));
   NodeArray[1] = CreateDataCell("25%", iTableRowHeightShort, true, Font.release());
   NodeArray[2] = CreateDataCell("25%", iTableRowHeightShort, true, new CHTMLText(""));
   NodeArray[3] = CreateDataCell("25%", iTableRowHeightShort, true, new CHTMLText(""));
   return CreateRow(NodeArray, iNodeSize, pTableColor);	
}

CNCBINode* CSeqBasicCommand::ExternalLinkCheck(const eQueryType eIDType)
{
   CNCBINode* node;
   switch (eIDType) 
   {
      case eQuery_TaxID: /* iIdtype is Taxon ID */
	/* provide link to external taxon website */
	node = CreateExternalLink(taxonURL, "NCBI Taxon Browser", "taxon");
	break;
      case eQuery_PubMedID: /* iIdtype is PubMed ID */
	/* provide link to extern pubmed website */
	node = CreateExternalLink(pubmedURL, "NCBI PubMed Browser", "pubmed");
        break;
      default:
	break;
   }
   return node; 
}

/* When a GOID is specified, gives the user the ability to further
 * refine the search using the ECode & Degree away from the ecode
 * (based on the hierarchy). */
CNCBINode* CSeqBasicCommand::GOFields( CCgiContext& ctx, int iteration )
{
   static const int iNodeSize = 4;
   CNCBINode* NodeArray[iNodeSize];
   auto_ptr<CHTMLText> Text(new CHTMLText(""));;
   int iECode = 0, iDegrees = 0;
   try {
      iECode = NStr::StringToInt(GetQueryStringValue(ctx, "ecode"+NStr::IntToString(iteration)));
      iDegrees = NStr::StringToInt(GetQueryStringValue(ctx, "degrees"+NStr::IntToString(iteration)));
   }catch (CStringException& e) {iECode = iDegrees = 0;}

   auto_ptr<CHTMLText> ECode(new CHTMLText(""));
   ECode->AppendChild(CreateSelect(ecode, iEcodeArraySize, "ecode"+NStr::IntToString(iteration), iECode));
   NodeArray[0] = CreateDataCell("25%", iTableRowHeightShort, true, new CHTMLText(""));
   NodeArray[1] = CreateDataCell("25%", iTableRowHeightShort, true, new CHTMLText("Restrict to ECode"));
   NodeArray[2] = CreateDataCell("25%", iTableRowHeightShort, false, ECode.release());
   NodeArray[3] = CreateDataCell("25%", iTableRowHeightShort, true, new CHTMLText(""));
   Text->AppendChild(CreateRow(NodeArray, iNodeSize, pTableColor));

   auto_ptr<CHTMLText> Degrees(new CHTMLText(""));
   Degrees->AppendChild(CreateSelect(degrees, iDegreesArraySize, "degrees"+NStr::IntToString(iteration), iDegrees))->AppendChild(nbsp(4));
   NodeArray[0] = CreateDataCell("25%", iTableRowHeightShort, true, new CHTMLText("")); 
   NodeArray[1] = CreateDataCell("25%", iTableRowHeightShort, true, new CHTMLText("Use GOIDs related by")); ; 
   NodeArray[2] = CreateDataCell("25%", iTableRowHeightShort, false, (Degrees.release())->AppendChild(new CHTMLText("degrees"))); 
   NodeArray[3] = CreateDataCell("25%", iTableRowHeightShort, true, new CHTMLText("")); ; 
   Text->AppendChild(CreateRow(NodeArray, iNodeSize, pTableColor));
   return Text.release();
}

/* CreateView() is the function that creates the html to replace
 * the <@VIEW@> tag in the HtmlBaseFile. This class is responsible
 * for creating the web page when it initially loads up */
CNCBINode* CSeqBasicCommand::CreateView( CCgiContext& ctx )
{
    /* creates the initial home page. Gets the registry so 
     * we can access the config file. */
    const CNcbiRegistry& reg = ctx.GetConfig();

    /* add a table of the dimensions to the page */
    auto_ptr<CHTML_table> pTable(new CHTML_table);
    pTable->SetCellPadding(0)->SetCellSpacing(0)->SetBgColor("FFFFFF")->SetWidth(700);

    /* add a form to the table, get the value of 'URL' in section 'html' 
     * in the configuration file wwwseqgi_cxx.ini and set that to the 
     * form's action; this will specify the cgi script to call on submit */
    auto_ptr <CHTML_form> Form(new CHTML_form(reg.Get("html", "URL")));

    /* set the form's name */
    Form->SetNameAttribute("advancedsearch");
    /* add rows to the form: a title, % means take the 
     * default width of the table, which is set above ... */
    Form->AppendChild(new CHTML_tr(CreateTableTitleInDataCell("%", "Advanced Search")));

    /* add the first row to the 'advancedsearch' table which asks 
     * the user to specify the type of molecule + idtype (taxid, ... 
     * locuslink id) and the id value and the logic (optional). */   
    Form->AppendChild(CreateDefaultRow(/* using default parameters */));

    /* append the row that specifies the format of the return value, 
     * and the fetch button ... */
    Form->AppendChild(CreateFetchRow(/* using defaults */));

    /* ...and a white space for formatting ...*/
    Form->AppendChild((new CHTML_tr)->SetHeight(90));
    /* ... and some hidden information */
    Form->AppendChild(new CHTML_hidden("cmd", "fetch"));
    Form->AppendChild(new CHTML_hidden("loop", 1));
    pTable->AppendChild(Form.release());

    return pTable.release();
}
 
 
/*
 * CreateQuickSearch: 
 *  creates the quick search table by filling the 
 *  @QUICKSEARCH* tag in the html base file.
 * 
 *
 * the quick search table consists of a form holding 2 rows.
 *   1) a title 
 *   2) a data input row
 *
 *
 * the data input row allows user to use form elements such
 * as drop down menus and text boxes to specify keys, formats
 * and other information needed to retrieve a record. 
 *
 */
CNCBINode* CSeqBasicCommand::CreateQuickSearch( CCgiContext& ctx )
{
   /* reads from the configuration file */ 
   const CNcbiRegistry& reg = ctx.GetConfig();


   /*  the quick search table */
   auto_ptr<CHTML_table> pTable(new CHTML_table);
   pTable->SetCellPadding(0)->SetCellSpacing(0)->SetBgColor("FFFFFF")->SetWidth(700);
   
   /* the form in quick search */ 
   
   /* retrieves the value of 'URL' in section '[html]' of configuration
    * file. This value will specify the form's action and the cgi script
    * to call on submit
    */ 
   auto_ptr <CHTML_form> Form(new CHTML_form(reg.Get("html", "URL")));
   Form->SetNameAttribute("quicksearch");
  
   /* add rows in the table */
   Form->AppendChild(new CHTML_tr(new CHTML_img("http://www.ncbi.nlm.nih.gov/corehtml/spacer1.GIF", 10, 30)));
   Form->AppendChild(new CHTML_tr(CreateTableTitleInDataCell("%", "Quick Search")));

   const int iNodeSize = 3;
   CNCBINode* NodeArray[iNodeSize];
   auto_ptr<CHTMLText> Data(new CHTMLText("Search for"));          /* data cell 1 */
   auto_ptr<CHTMLText> Data1(new CHTMLText("Formatted as"));       /* data cell 2 */
   auto_ptr<CHTMLText> Data2(new CHTMLText(""));                   /* data cell 3 */

   /* populate the data cells */
   { /* data cell 1 */
      Data->AppendChild(nbsp(4));
      Data->AppendChild(CreateSelect(quickkeys, iQuickKeysArraySize, "type"));
      Data->AppendChild(nbsp(4))->AppendChild(new CHTMLText("for"))->AppendChild(nbsp(3));
      CHTML_text* Text = new CHTML_text("input", /*iIDTextBoxSize*/15); Text->SetAttribute("onfocus", "select();");
      Data->AppendChild(Text);
      //Data->AppendChild(new CHTML_hidden("cmd", "quick"));
      NodeArray[0] = CreateDataCell("50%", iTableRowHeight, true, Data.release());
   }{ /* data cell 2 */
      Data1->AppendChild(nbsp(2));
      Data1->AppendChild(CreateSelect(format, iFormatArraySize, "format"));
      NodeArray[1] = CreateDataCell("30%", iTableRowHeight, false, Data1.release());
   }{ /* data cell 3 */ 
      auto_ptr<CHTML_submit> FetchSubmit(new CHTML_submit("submit", "FETCH"));
      Data2->AppendChild(FetchSubmit.release());
      NodeArray[2] = CreateDataCell("10%", iTableRowHeight, false, Data2.release()); 
   }

   /* create a row with the data cells */ 
   Form->AppendChild(CreateRow(NodeArray, iNodeSize, pTableColor));


   /* add some white space as a row in the table */
   Form->AppendChild(new CHTML_tr(CreateDataCell("%", 90, true, new CHTMLText(""))));
   Form->AppendChild(new CHTML_tr(new CHTML_hidden("cmd", "quick")));
   pTable->AppendChild(Form.release());
   return pTable.release();
}


void CSeqBasicCommand::Execute( CCgiContext& ctx )
{
    const CNcbiRegistry& reg = ctx.GetConfig();
    
    /* load in the html template file */
    string baseFile = reg.Get( "filesystem", "HtmlBaseFile" );
    auto_ptr<CHTMLPage> page( new CHTMLPage( NcbiEmptyString, baseFile ) );
    
    /* set up to replace <@VIEW@> in template file with html returned
       from CreateView */
    page->AddTagMap( "QUICKSEARCH", CreateQuickSearch(ctx) );
    page->AddTagMap( "VIEW", CreateView( ctx ) );
    /* actual page output */
    ctx.GetResponse().WriteHeader();
    page->Print(ctx.GetResponse().out(), CNCBINode::eHTML );
}
/*********************************** CSeqAdvancedCommand **********************************/
/* this class is responsible for constructing the webpage after each time the
 * logic buttons are pressed. When the logic buttons get pressed, it means
 * the user is entering "advanced" search parameters. Inherits from 
 * CSeqBasicCommand class which defines the CreateView function that
 * this class uses. */
CSeqAdvancedCommand::CSeqAdvancedCommand( CNcbiResource& resource )
  : CSeqBasicCommand( resource )
{}

CSeqAdvancedCommand::~CSeqAdvancedCommand( void )
{}

CNcbiCommand* CSeqAdvancedCommand::Clone( void ) const
{
    return new CSeqAdvancedCommand( GetSeqResource() );
}

/* cmd=advanced is set in the query string, */
inline string CSeqAdvancedCommand::GetName( void ) const
{
    return string("advanced");
}

/* CreateView() is the function that creates the html to 
 * replace the <@VIEW@> tag in the HtmlBaseFile. */
CNCBINode* CSeqAdvancedCommand::CreateView( CCgiContext& ctx )
{
    /* creates the initial home page. Gets the registry so 
     * we can access the config file */
    const CNcbiRegistry& reg = ctx.GetConfig();
    
    /* add a table of the dimensions to the page */
    auto_ptr<CHTML_table> pTable(new CHTML_table);
    pTable->SetCellPadding(0)->SetCellSpacing(0)->SetBgColor("FFFFFF")->SetWidth(700);

    /* add a form to the table */
    auto_ptr <CHTML_form> Form(new CHTML_form(reg.Get("html", "URL")));
    Form->SetNameAttribute("advancedsearch");
    /* add rows to the form, a whitespace row, and a title */
    Form->AppendChild(new CHTML_tr(CreateTableTitleInDataCell("%", "Advanced Search")));
   
    /* since not initiation, iLoop > 0 */
    int iLoop = 0;
    try{ iLoop = NStr::StringToInt(GetQueryStringValue(ctx, "loop")); }
    catch (CStringException& e){ iLoop = 0;}

    /* page contain information that should persist in next page:
     * extract the molecule type (eMol): Protein or Nucleic Acid, 
     * extract the ID type (eIDType): taxid, PubMedID, MMDB ID, 
     *     Chromosome ID, GOID, OMIM ID, CDD ID, Locus Link ID
     * extract the Logic (iLogic): AND, OR, NOT */
     SHoundLogic::eLogic eLog = SHoundLogic::eAND;
     eQueryType eIDType = eQuery_TaxID;
     eMolType eMol = eProtein;
     try { 
	eMol = static_cast<eMolType>(NStr::StringToInt(GetQueryStringValue(ctx, "mol"))); 
        eIDType = static_cast<eQueryType>(NStr::StringToInt(GetQueryStringValue(ctx, "idtype0")));
	eLog = (SHoundLogic::eLogic)NStr::StringToInt(GetQueryStringValue(ctx, "logic0"));
     }catch (CStringException& e) { return CreateErrorView(ctx); }
     SHoundLogic::eLogic eLogLast = eLog;
     string id = GetQueryStringValue(ctx, "id0");
     
     /* reconstruct the row 0 which specifies the molecule, ... */
     Form->AppendChild(CreateDefaultRow(eMol, eIDType, eLog, id, false));
  
     /* create an external link to ncbi website if ID 
      * type is TaxID or PubMedID  
      * or to a GO browser if GOID */
     if(eIDType == eQuery_TaxID || eIDType == eQuery_PubMedID)
        Form->AppendChild(ExternalLinkCheck(eIDType));
     else if(eIDType == eQuery_GOID)
	Form->AppendChild(GOFields(ctx, 0));
     
     int i;
     
     /* for each of the remaining 'iLoop' rows in the table ... */
     for(i = 1; i < iLoop; i++)
     {
       /* grab the field values ... */
       try{
         eIDType = static_cast<eQueryType>(NStr::StringToInt(GetQueryStringValue(ctx, "idtype"+NStr::IntToString(i))));
	 id = GetQueryStringValue(ctx, "id"+NStr::IntToString(i));
	 eLogLast = (SHoundLogic::eLogic)NStr::StringToInt(GetQueryStringValue(ctx, "logic"+NStr::IntToString(i-1)));
	 eLog = (SHoundLogic::eLogic)NStr::StringToInt(GetQueryStringValue(ctx, "logic"+NStr::IntToString(i)));
       }catch(CStringException& e){
         return CreateErrorView(ctx);
       } /* ... and fill the search fields ... */

       Form->AppendChild(CreateParameteredSearchRow(eLogLast, i, eIDType, id, eLog, false));
       /* create an external link to ncbi website if ID type is TaxID 
	* or PubMedID, or to a GO browser if GOID */
       if(eIDType == eQuery_TaxID || eIDType == eQuery_PubMedID)
         Form->AppendChild(ExternalLinkCheck(eIDType));
       else if(eIDType == eQuery_GOID){
	 Form->AppendChild(GOFields(ctx, i));
       }
     } /* end for */

     /* construct the blank row for input */
     eLogLast = (SHoundLogic::eLogic)NStr::StringToInt(GetQueryStringValue(ctx, "logic"+NStr::IntToString(i-1)));
     Form->AppendChild(CreateParameteredSearchRow(eLogLast, iLoop));

    /* append the row that specifies the format of the return value, 
     * and the fetch button ... */
    Form->AppendChild(CreateFetchRow());
   
    /* ...and a white space for formatting ...*/
    Form->AppendChild((new CHTML_tr)->SetHeight(150));
    /* ... and some hidden information, since the logic button */
    Form->AppendChild(new CHTML_hidden("cmd", "fetch"));
    Form->AppendChild(new CHTML_hidden("loop", NStr::IntToString(++iLoop)));
    pTable->AppendChild(Form.release());
    
    return pTable.release();
}


/**
 * Adds some white space for pretty printing, and because we have to
 * fill in the <@QUICKSEARCH@> tag.
 */
CNCBINode* CSeqAdvancedCommand::CreateSpacer( CCgiContext& ctx )
{

   const CNcbiRegistry& reg = ctx.GetConfig();

   /* use white spacer from NCBI */
   auto_ptr<CHTML_table> pTable(new CHTML_table);
   pTable->SetCellPadding(0)->SetCellSpacing(0)->SetBgColor("#FFFFFF")->SetWidth(700);
   pTable->AppendChild(new CHTML_tr(
      new CHTML_img("http://www.ncbi.nlm.nih.gov/corehtml/spacer1.GIF", 700, 30))); 


   return pTable.release();
}


/* carries out the construction of the webpage when the 
 * cmd=advanced is set in the query string. */
void CSeqAdvancedCommand::Execute( CCgiContext& ctx )
{
    const CNcbiRegistry& reg = ctx.GetConfig();
    
    /* load in the html template file */
    string baseFile = reg.Get( "filesystem", "HtmlBaseFile" );
    auto_ptr<CHTMLPage> page( new CHTMLPage( NcbiEmptyString, baseFile ) );


    page->AddTagMap( "QUICKSEARCH", CreateSpacer( ctx )/* CreateQuickSearch(ctx)*/ );
    /* calls parent (CSeqBasicCommand) CreateView,
     * to add the "Advanced Search" table: allows
     * user to enter parameters for search. */
    page->AddTagMap( "VIEW", CreateView( ctx ) );

    /* actual page output */
    ctx.GetResponse().WriteHeader();
    page->Print(ctx.GetResponse().out(), CNCBINode::eHTML );
}

/*********************************** CSeqAdvancedGOCommand **********************************/
/* this class is responsible for constructing the webpage when the user 
 * specifies a goid. GOID can be further refined by ecode & degrees of
 * separation.  Inherits from CSeqBasicCommand class which defines 
 * the CreateView function that this class uses. */
CSeqAdvancedGOCommand::CSeqAdvancedGOCommand( CNcbiResource& resource )
  : CSeqBasicCommand( resource )
{}

CSeqAdvancedGOCommand::~CSeqAdvancedGOCommand( void )
{}

CNcbiCommand* CSeqAdvancedGOCommand::Clone( void ) const
{
    return new CSeqAdvancedGOCommand( GetSeqResource() );
}

/* cmd=goadvanced is set in the query string, */
inline string CSeqAdvancedGOCommand::GetName( void ) const
{
    return string("goadvanced");
}

/* CreateView() is the function that creates the html to replace the <@VIEW@> 
 * tag in the HtmlBaseFile. */
CNCBINode* CSeqAdvancedGOCommand::CreateView( CCgiContext& ctx )
{
    /* creates the initial home page. Gets the registry so 
     * we can access the config file */
    const CNcbiRegistry& reg = ctx.GetConfig();
 
    /* add a table of the dimensions to the page */
    auto_ptr<CHTML_table> pTable(new CHTML_table);
    pTable->SetCellPadding(0)->SetCellSpacing(0)->SetBgColor("FFFFFF")->SetWidth(700);

    /* add a form to the table */
    auto_ptr <CHTML_form> Form(new CHTML_form(reg.Get("html", "URL")));
    Form->SetNameAttribute("advancedsearch");
    /* add rows to the form, a whitespace row, and a title */
    Form->AppendChild(new CHTML_tr(CreateTableTitleInDataCell("%", "Advanced Search")));

    /* since not initiation, iLoop > 0
     * extract the molecule type (eMol): Protein, Nucleic Acid */ 
    int iLoop;
    SHoundLogic::eLogic eLogic = SHoundLogic::eAND;
    eQueryType eIDType = eQuery_TaxID;
    eMolType eMol = eProtein;
    try {
       iLoop = NStr::StringToInt(GetQueryStringValue(ctx, "loop"));
       eMol = static_cast<eMolType>(NStr::StringToInt(GetQueryStringValue(ctx, "mol")));
    } catch (CStringException& e){
       /* this should never happen, but need to set it anyways. */
       /* maybe put a createError here */
       /* iLoop = 1; eMol = 0; */
       CreateErrorView(ctx);
    }

    int i;
    SHoundLogic::eLogic eLog_Pre;
    for(i = 0; i < iLoop; i++)
    {
       eLog_Pre = eLogic;
       /* page contain information that should persist in next page:
        * extract the ID type (iIdytpe): taxid, PubMedID, MMDB ID, 
	*    Chromosome ID, GOID, OMIM ID, CDD ID, Locus Link ID
        * extract the Logic (iLogic): AND, OR, NOT */
       try { 
          eIDType = static_cast<eQueryType>(NStr::StringToInt(GetQueryStringValue(ctx, "idtype"+NStr::IntToString(i))));
	  eLogic = (SHoundLogic::eLogic)NStr::StringToInt(GetQueryStringValue(ctx, "logic"+NStr::IntToString(i)));
       }catch (CStringException& e) {
	  /* the only way to get here is if the logic button was not set; occurs
	   * only when the eIDType = GOID, so we make sure that as we update
	   * the webpage, the logic button is not set either(ie set it to value > size). */
	  eLogic = SHoundLogic::eUND;
       }

       /* grab the id string */
       string id = GetQueryStringValue(ctx, "id"+NStr::IntToString(i));
       if(iLoop == 1) /* only one row, so has to be a default row with functional logic buttons */
          /* reconstruct row 0 which specifies the molecule, ... */
          Form->AppendChild(CreateDefaultRow(eMol, eIDType, eLogic, id, true));
       else if(i == iLoop - 1) /* last row in search, so logic buttons must be functional */
	  Form->AppendChild(CreateParameteredSearchRow(eLog_Pre, i, eIDType, id, eLogic, true));
       else if(i == 0)
	  /* when iLoop > 1 & i == 0, need to make the first row  */
	  Form->AppendChild(CreateDefaultRow(eMol, eIDType, eLogic, id, false));
       else
          Form->AppendChild(CreateParameteredSearchRow(eLog_Pre, i, eIDType, id, eLogic, false));

       /* create an external link to ncbi website if ID
        * type is TaxID or PubMedID,
        * or to a GO browser if GOID */
       if(eIDType == eQuery_TaxID || eIDType == eQuery_PubMedID)
          Form->AppendChild(ExternalLinkCheck(eIDType));
       if(eIDType == eQuery_GOID)
	  Form->AppendChild(GOFields(ctx, i));
    }

    /* append the row that specifies the format of the return value, 
     * and the fetch button ... */
    Form->AppendChild(CreateFetchRow());
   
    /* ...and a white space for formatting ...*/
    Form->AppendChild((new CHTML_tr)->SetHeight(150));
    /* ... and some hidden information */
    Form->AppendChild(new CHTML_hidden("cmd", "fetch"));
    Form->AppendChild(new CHTML_hidden("loop", NStr::IntToString(iLoop)));
    pTable->AppendChild(Form.release());
    
    return pTable.release();
}

CNCBINode* CSeqAdvancedGOCommand::CreateSpacer( CCgiContext& ctx )
{

   const CNcbiRegistry& reg = ctx.GetConfig();

   /* use white spacer from NCBI */
   auto_ptr<CHTML_table> pTable(new CHTML_table);
   pTable->SetCellPadding(0)->SetCellSpacing(0)->SetBgColor("#FFFFFF")->SetWidth(700);
   pTable->AppendChild(new CHTML_tr(
      new CHTML_img("http://www.ncbi.nlm.nih.gov/corehtml/spacer1.GIF", 700, 30))); 


   return pTable.release();
}


/* carries out the construction of the webpage when the 
 * cmd=advanced is set in the query string. */
void CSeqAdvancedGOCommand::Execute( CCgiContext& ctx )
{
    const CNcbiRegistry& reg = ctx.GetConfig();
    
    /* load in the html template file */
    string baseFile = reg.Get( "filesystem", "HtmlBaseFile" );
    auto_ptr<CHTMLPage> page( new CHTMLPage( NcbiEmptyString, baseFile ) );

    page->AddTagMap("QUICKSEARCH", CreateSpacer( ctx ));

    /* calls parent (CSeqBasicCommand) CreateViewi,
     * to add the "Advanced Search" table: allows
     * user to enter parameters for search. */
    page->AddTagMap( "VIEW", CreateView( ctx ) );

    /* actual page output */
    ctx.GetResponse().WriteHeader();
    page->Print(ctx.GetResponse().out(), CNCBINode::eHTML );
}



END_SCOPE(shoundcgi)


/* 
 * $Log: seqdata_cmd.cpp,v $
 * Revision 1.1  2003/11/21 21:11:42  haocl
 * major refactoring, splitting cgi into 2 executables, renaming files
 * from seqxxx to seqdata, renaming Makefile
 *
 * Revision 1.2  2003/11/07 18:06:29  haocl
 * fixed compiling for new toolkit
 *
 * Revision 1.1.1.6  2003/08/21 08:48:07  root
 * Automated import from Sourceforge.net
 *
 * Revision 1.7  2003/08/20 19:33:44  haocl
 * check in for switchover
 *
 * Revision 1.6  2003/08/18 19:17:09  haocl
 * minor changes, got rid of some magic numbers,
 * changed returned list to passed in references
 *
 * Revision 1.5  2003/08/13 20:35:39  haocl
 * made use of the inheritance hierarchy of SHoundRecord classes
 *
 * Revision 1.4  2003/08/12 19:36:24  haocl
 * modularized display to use SHoundRecord hierarchy
 *
 * Revision 1.3  2003/08/08 16:22:43  haocl
 * new files shoundrecords and changes to others
 *
 * Revision 1.1  2003/07/31 21:16:38  haocl
 * checking in portion of the new web interface
 * 
 *
 */
