/* $Id: shoundrecords.cpp,v 1.3 2003/11/21 21:11:42 haocl Exp $ */ 
/***************************************************************************
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
#include <corelib/ncbistd.hpp>
#include <shoundrecords.hpp>
#include <shoundtypes.hpp>
#include <sstream>
#include <ncbi.h>
#include <seqhound.h>
#include <objslristruc.h>
#include <asn2ff.h>


using namespace ncbi;
using namespace shoundquery;


/***************************************************************************
 * shoundrecord namespace
 *   includes the shound record hierarchy and the shound record registry 
 *   class. the classes are modelled in the factory method design. The
 *   registry class acts as the factory, and clones the record classes.
 *   Everything is transparent to the user. The only actions a user class
 *   needs to do is create a registry class, load registry, and then
 *   call GetDataRecord with a format and (gene info) identifier, ie
 *   
 *      CSHoundRecordRegistry reg;
 *      CSHoundRecord *rec; 
 *      
 *      reg.LoadRegistry();
 *      rec = reg.GetDataRecord(eGenBank, 4557225);
 *      cout << rec->DisplayRecord(eHTML) << endl;
 * 
 **************************************************************************/
namespace shoundrecord
{

/****************************************************************
 * 
 * CSHoundRecordRegistry. 
 *
 * Implements the factory method design. Is used to clone
 *  objects that inherit from CSHoundRecord.
 * 
 * Holds a copy of all CSHoundRecord types,
 * and clones the requested type as requested.
 * 
 ****************************************************************/
CSHoundRecordRegistry::CSHoundRecordRegistry(void)
{
   registry = new list<CSHoundRecord*>;
}

CSHoundRecordRegistry::~CSHoundRecordRegistry(void)
{
   delete(registry);
}

/******************************************************************************
 *
 * LoadRegistry: loads the registry with the record type. 
 *
 ************************************************************************/
void CSHoundRecordRegistry::LoadRegistry(void)
{
   /* if expanding the CSHoundRecord hierarchy, add new
    * class here 
    */
	
   /* don't need the CSHoundRecord, this is same as GetDefaultRecord */
   /* registry->push_back(new CSHoundRecord());  */
   registry->push_back(new CSHoundDefline()); 
   registry->push_back(new CSHoundFasta()); 
   registry->push_back(new CSHoundASN()); 
   registry->push_back(new CSHoundXML()); 
   registry->push_back(new CSHoundGenBank()); 
   registry->push_back(new CSHoundProteinReport());
}


/*******************************************************************
 * GetDefaultRecord
 *   when the registry cannot find a specified format, it 
 *   returns the default format (the base class CSHoundRecord) .
 * 
 *******************************************************************/
CSHoundRecord* CSHoundRecordRegistry::GetDefaultRecord(void)
{
   return new CSHoundRecord();
}


/*********************************************************************
 *
 *  returns the CSHoundRecord of format 'FormatToGet' if found,
 * otherwise returns CSHoundRecord* base class. 
 *********************************************************************/
CSHoundRecord* CSHoundRecordRegistry::GetRecord(eFormatType FormatToGet)
{
   RecIT = find_if(registry->begin(), registry->end(), PRecord<CSHoundRecord*>(FormatToGet));
   return RecIT == registry->end() ?  GetDefaultRecord()->Clone() : (*RecIT)->Clone();
}

/******************************************************************
 * GetDataRecord:
 *   The public interface to the registry class. The allowed
 *   'FormatToGet' are:
 *      eDefline, eFASTA, eASN, eXML, eGenBank, eSeqReport ...
 *      see shoundtypes.hpp
 * 
 *   'key' is the identifier of the record to be retrieved. ex
 *   if 'FormatToGet' = eFASTA, then key is a gi, eg 4557225.
 *
 *   returns a pointer to CSHoundRecord of the key, in the format specified.
 *   
 ******************************************************************/
CSHoundRecord* CSHoundRecordRegistry::GetDataRecord(eFormatType FormatToGet, Int4 key)
{
   CSHoundRecord* rec = GetRecord(FormatToGet);
   rec->SetKey(key);
   return rec; 
}


/**************** THE BASE CLASS *********************/

/*********************************************************************
 * CSHoundRecord, the base class from which other CSHoundRecord types
 * inherit from. It's content is an empty string ("") and its 
 * format is eUndef
 * 
 *********************************************************************/
CSHoundRecord::CSHoundRecord(void)
{}

CSHoundRecord::~CSHoundRecord()
{}

string CSHoundRecord::StringRecord(void)
{
   return kEmptyStr; 
}

string CSHoundRecord::HTMLRecord(void)
{
   return kEmptyStr;
}

string CSHoundRecord::DisplayRecord(eContentType eFormat)
{
   return (eFormat == eHTML) ? HTMLRecord() : StringRecord();
}

eFormatType CSHoundRecord::RecordType(void)
{
   return eUndef;
}

void CSHoundRecord::SetKey(Int4 key)
{
   iKey = key;
   return;
}

CSHoundRecord* CSHoundRecord::Clone(void)
{
   return new CSHoundRecord();
}

string CSHoundRecord::SetRecord(Pointer rec)
{
   return kEmptyStr;
}

Int4 CSHoundRecord::GetKey(void)
{
   return iKey;
}
/************************END BASE CLASS ***************************/


/************************DEFLINE CLASS ******************************/
CSHoundDefline::CSHoundDefline(void)
{}

CSHoundDefline::~CSHoundDefline()
{
   delete(pRec);
}

string CSHoundDefline::SetRecord(Pointer record)
{
   pRec = (char*)malloc(sizeof(char) * StringLen((char*)record));
   StringCpy(pRec, (char*)record);
   istringstream iss(pRec);
   sRec = iss.str();
   return sRec;
}

eFormatType CSHoundDefline::RecordType(void)
{
   return eDefline;
}

string CSHoundDefline::DisplayRecord(eContentType eFormat)
{
   return (eFormat == eHTML) ? HTMLRecord() : StringRecord(); 
}

string CSHoundDefline::HTMLRecord(void)
{
  return string("<pre>") + sRec + string("</pre>");
}

string CSHoundDefline::StringRecord(void)
{
   return sRec;
}

void CSHoundDefline::SetKey(Int4 key)
{
   iKey = key;
   SetRecord(static_cast<Pointer>(SHoundGetDefline(key)));
}

CSHoundRecord* CSHoundDefline::Clone(void)
{
   return new CSHoundDefline();
}
/*************************END DEFLINE CLASS ************************/


/************************FASTA CLASS **********************/
CSHoundFasta::CSHoundFasta(void)
{}

CSHoundFasta::~CSHoundFasta()
{
   //delete(pRec);
}

eFormatType CSHoundFasta::RecordType(void)
{
   return eFASTA;
}

string CSHoundFasta::DisplayRecord(eContentType eFormat)
{
   return (eFormat == eHTML) ? HTMLRecord() : StringRecord();
}

string CSHoundFasta::HTMLRecord(void)
{
   return string("<pre>") + sRec + string("</pre>");
}

string CSHoundFasta::StringRecord(void)
{
   return sRec;
}

CSHoundRecord* CSHoundFasta::Clone(void)
{
   return new CSHoundFasta();
}

string CSHoundFasta::SetRecord(Pointer rec)
{
   SLRIFasta* ptr = (SLRIFasta*)rec;
   pRec = (char*) malloc (strlen(ptr->defline) * sizeof(char));
   strcat(strcat(strcpy(pRec, ptr->defline), "\n"), ptr->sequence);
   istringstream s(pRec);
   sRec = s.str();
   return sRec;
}

void CSHoundFasta::SetKey(Int4 key)
{
   iKey = key;
   SetRecord(static_cast<Pointer>(SHoundGetFasta(key)));
}
/************************END FASTA CLASS *******************/


/************************ASN CLASS **********************/
CSHoundASN::CSHoundASN(void)
{}

CSHoundASN::~CSHoundASN()
{
   //delete pRec;
}

eFormatType CSHoundASN::RecordType(void)
{
   return eASN;
}

string CSHoundASN::DisplayRecord(eContentType eFormat)
{
   return (eFormat == eHTML) ? HTMLRecord() : StringRecord();
}

string CSHoundASN::HTMLRecord(void)
{
   return string("<pre>") + sRec + string("</pre>");
}

string CSHoundASN::StringRecord(void)
{
   return sRec;
}

CSHoundRecord* CSHoundASN::Clone(void)
{
   return new CSHoundASN();
}

string CSHoundASN::SetRecord(Pointer rec)
{
   char format[] = "w";
   ByteStorePtr bsp = BSNew(10000);
   AsnIoBSPtr aibp = AsnIoBSOpen(format, bsp);
   BioseqAsnWrite((BioseqPtr)rec, aibp->aip, NULL);
   AsnIoBSClose(aibp);
   pRec = BSStringSave(bsp);
   istringstream iss(pRec);
   sRec = iss.str();
   BSFree(bsp);
   return sRec;
}

void CSHoundASN::SetKey(Int4 key)
{
   iKey = key;
   SetRecord(static_cast<Pointer>(SHoundGetBioseq(key)));
}
/************************END ASN CLASS *******************/


/************************XML CLASS **********************/
CSHoundXML::CSHoundXML(void)
{}

CSHoundXML::~CSHoundXML()
{
   //delete(pRec);
}

eFormatType CSHoundXML::RecordType(void)
{
   return eXML;
}

string CSHoundXML::DisplayRecord(eContentType eFormat)
{
   return (eFormat == eHTML) ? HTMLRecord() : StringRecord();
}

string CSHoundXML::HTMLRecord(void)
{
   string sXML, sXML2;
   NStr::Replace(sRec, "<", "&lt;", sXML);
   NStr::Replace(sXML, ">", "&gt;", sXML2);
   return string("<pre>") + sXML2 + string("</pre>");
}

string CSHoundXML::StringRecord(void)
{
   return sRec;
}

CSHoundRecord* CSHoundXML::Clone(void)
{
   return new CSHoundXML();
}

string CSHoundXML::SetRecord(Pointer rec)
{
   char format[] = "wx";
   ByteStorePtr bsp = BSNew(10000);
   AsnIoBSPtr aibp = AsnIoBSOpen(format, bsp);
   BioseqAsnWrite((BioseqPtr)rec, aibp->aip, NULL);
   AsnIoBSClose(aibp);
   pRec = BSStringSave(bsp);
   istringstream iss(pRec);
   sRec = iss.str();
   BSFree(bsp);
   return sRec;
}

void CSHoundXML::SetKey(Int4 key)
{
   iKey = key;
   SetRecord(static_cast<Pointer>(SHoundGetBioseq(key)));
}

/************************END XML CLASS *******************/



/************************GENBANK CLASS **********************/
CSHoundGenBank::CSHoundGenBank(void)
{}

CSHoundGenBank::~CSHoundGenBank()
{
   //delete(pRec);
}

eFormatType CSHoundGenBank::RecordType(void)
{
   return eGenBank;
}

string CSHoundGenBank::DisplayRecord(eContentType eFormat)
{
   return (eFormat == eHTML) ? HTMLRecord() : StringRecord();
}

string CSHoundGenBank::HTMLRecord(void)
{
   return string("<pre>") + sRec + string("</pre>");
}

string CSHoundGenBank::StringRecord(void)
{
   return sRec;
}

CSHoundRecord* CSHoundGenBank::Clone(void)
{
   return new CSHoundGenBank();
}

string CSHoundGenBank::SetRecord(Pointer rec)
{
  BioseqPtr pbs = NULL;
  BioseqSetPtr pbss = NULL;
  LinkStrPtr lsp = NULL, lsp_tmp = NULL;
  SeqEntryPtr pse = (SeqEntryPtr) rec;
  ByteStorePtr bsp = BSNew(10000);
  BSSeek(bsp, 0, SEEK_SET);
  if(pse->choice == 1)
  {
    pbs = (BioseqPtr)pse->data.ptrvalue;
    SeqMgrSeqEntry(SM_BIOSEQ, (Pointer)pbs, pse);
    
  }else{
    pbss = (BioseqSetPtr)pse->data.ptrvalue;
    SeqMgrSeqEntry(SM_BIOSEQSET, (Pointer)pbss, pse);
  }
  if(!SHoundIsProtein(iKey))
     lsp = SeqEntryToStrArrayEx(pse, GENBANK_FMT, iKey, false);
  else
     lsp = SeqEntryToStrArrayEx(pse, GENPEPT_FMT, iKey, false);

  for(lsp_tmp = lsp; lsp_tmp != NULL; lsp_tmp = lsp_tmp->next)
  {
    BSWrite(bsp, lsp_tmp->line, strlen(lsp_tmp->line));
  }
  
  istringstream iss(BSStringSave(bsp));
  sRec = iss.str();
  BSFree(bsp);
  return sRec;
}

void CSHoundGenBank::SetKey(Int4 key)
{
   iKey = key;
   SetRecord(static_cast<Pointer>(SHoundGetSeqEntry(key)));
}

/************************END GENBANK CLASS *******************/

/******************** PROTEIN REPORT CLASS ***************/
CSHoundProteinReport::CSHoundProteinReport(void)
{}

CSHoundProteinReport::~CSHoundProteinReport(void)
{
   // delete (pRec);
}

CSHoundRecord *CSHoundProteinReport::Clone(void)
{
   return new CSHoundProteinReport();
}

eFormatType CSHoundProteinReport::RecordType(void)
{
   return eSeqReport;
}

string CSHoundProteinReport::SetRecord(Pointer record)
{
   char format[] = "w";
   ByteStorePtr bsp = BSNew(10000);
   AsnIoBSPtr aibp = AsnIoBSOpen(format, bsp);
   SHProteinReportSetDescriptionAsnWrite((SHProteinReportSetDescriptionPtr)record, aibp->aip, NULL);
   AsnIoBSClose(aibp);
   pRec = BSStringSave(bsp);
   istringstream iss(pRec);
   sRec = iss.str();
   BSFree(bsp);
   return sRec;
}

string CSHoundProteinReport::HTMLRecord(void)
{
   return string("<pre>") + sRec + string("</pre>");
}

string CSHoundProteinReport::StringRecord(void)
{
   return sRec;
}

string CSHoundProteinReport::DisplayRecord(eContentType eFormat)
{
   return (eFormat == eHTML) ? HTMLRecord() : StringRecord();
}	

void CSHoundProteinReport::SetKey(Int4 key)
{
   SHProteinReportSetDescriptionPtr pprsd = NULL;
   Uint2 flags = 0;
   pprsd = SHProteinReportSetDescriptionNew(); 
   pprsd->query = key;
   flags |= REPORT_ALL;
   SHoundProteinReportSetDescription(pprsd, flags);
   SetRecord( static_cast<Pointer>( pprsd ) );
}
/********************* END PROTEIN REPORT CLASS ********************/

} /* end namespace shoundrecords */


/*
 *  $Log: shoundrecords.cpp,v $
 *  Revision 1.3  2003/11/21 21:11:42  haocl
 *  major refactoring, splitting cgi into 2 executables, renaming files
 *  from seqxxx to seqdata, renaming Makefile
 *
 *  Revision 1.2  2003/11/03 22:53:24  haocl
 *  added shproteinrecord type
 *
 *
 * 
 */
