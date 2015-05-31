/*$Id: rpsdb_cb.hpp,v 1.1.1.8 2002/11/15 09:34:09 root Exp $*/
/***************************************************************************
  Copyright (C) 2001 Mount Sinai Hospital (MSH)

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of
  the License, or any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

  See the GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the
         Free Software Foundation, Inc.,
         59 Temple Place, Suite 330, Boston, MA
         02111-1307  USA
  or visit http://www.gnu.org/copyleft/gpl.html

  SPECIAL EXCEPTIONS

  As a special exception, Mount Sinai Hospital gives permission to
  link this program with the following non-GPL programs or libraries,
  and distribute the resulting executable, without including the source
  code for these in the source distribution:

  a) CodeBase 6.5 or greater from Sequiter Software Inc.

  b) The C or C++ interface to Oracle 8.x or greater, from
     Oracle Corporation or IBM DB2 UDB.


  PROGRAM: RPSDB C++ CodeBase Layer


  AUTHORS:	Doron Betel betel@mshri.on.ca
            and Christopher W.V. Hogue hogue@mshri.on.ca


  REVISION/CONTRIBUTION NOTES:

  March 1, 2001  Original MSH Revision.
                Hogue Lab - University of Toronto Biochemistry
                Department and the Samuel Lunenfeld Research
                Institute, Mount Sinai Hospital
                http://bioinfo.mshri.on.ca  hogue@mshri.on.ca
                ftp://ftp.mshri.on.ca/


  PUBLICATION TO CITE:



***************************************************************************
*/
#ifndef _RPSDB_API_HPP
#define _RPSDB_API_HPP

#include <list>
#include <d4all.hpp>
#include <objects/cdd/Cdd.hpp>
USING_NCBI_SCOPE;
using namespace objects;

/*RpsRecord class to hold the intermedate
* record being copied */
class RpsDBRecord{
	public:
	/*default constructor*/
	RpsDBRecord(void) {Reset();}

	/* set functions */
	void SetGi(int i){ gi=i;}
	void SetDomId(char* s){ domid=s;}
	void SetDomId(string s) { domid=s;}
	void SetCddId(int s){ cddid=s;}
	void SetFrom(int i){ from=i;}
	void SetAlignLen(int i){ alignlen=i; }
	void SetScore(int i) { score=i; }
	void SetEvalue(double e){ evalue=e; }
	void SetBitScore (double e) { bitscore=e; }
	void SetMissingC (int i){ missingc=i; }
	void SetMissingN (int i) { missingn=i; }
	void SetNumdom (int i){ numdom=i; }

	/* get functions */
	int GetGi(void) { return gi;}
	string GetDomId(void) { return domid; }
	int GetCddId(void) { return cddid; }
	int GetFrom(void) {return from;}
	int GetAlignLen(void) {return alignlen;}
	int GetScore(void) {return score;}
	double GetEvalue(void) {return evalue;}
	double GetBitScore(void) {return bitscore;}
	int GetMissingC(void) {return missingc;}
	int GetMissingN(void) {return missingn;}
	int GetNumDom(void) {return numdom;}

	void Reset(void) { gi=0;
		from=0;
		domid.clear();
		cddid=0;
		alignlen=0;
		score=0;
		evalue=0.0;
		bitscore=0.0;
		missingc=0;
		missingn=0;
		numdom=0; }

	/* overloaded << operator */
	friend ostream& operator<< (ostream& os, RpsDBRecord& record);
	private:
	int gi;
	string domid;
	int cddid;
	int from;
	int alignlen;
	int score;
	double evalue;
	double bitscore;
	int missingn;
	int missingc;
	int numdom;
};

class DomnameDBRecord{
	public:
	/*default constructor */
	DomnameDBRecord(void) { Reset();}

	/*set functions*/
	void SetAccession(string a) {accession=a;}
	void SetName(string a) {name=a;}
	void SetPdbId(string x) {pdbid=x;}
	void SetCdd(ncbi::objects::CCdd& asn1);
	/* get functions */
	string GetAccession(void) {return accession;}
	string GetName(void) {return name;}
	string GetPdbId(void) {return pdbid;}
	CCdd& GetCCdd(void) {return m_cdd;}
	void Reset(void)
	{
		accession.clear();
		name.clear();
		pdbid.clear();
	}

	/* overloaded << operator */
	friend ostream& operator<< (ostream& os, RpsDBRecord& record);
	private:
	string accession;
	string name;
	string pdbid;
	ncbi::objects::CCdd m_cdd;
};

/**********************
* CodeBase Functions
***********************/
/* Init codebase*/
bool InitCodeBase(void);
bool IsCodeBaseInit(void);

/* Fini codebase */
void FiniCodeBase(void);

/*open table */
bool OpenRPSDB(const char* name, bool create);
bool OpenDOMNAME(string name, bool create);

/*close all tables */
void CloseRPSDB(void);
void CloseDOMNAME(void);

/*Append record*/
bool AppendRPSDBRecord(RpsDBRecord& record);
/*DOMNAME append is not tested. In particular writing
* the Cdd ASN1 object is not confirmed to work. */
bool AppendDOMNAMERecord(DomnameDBRecord& record);

/*print table -not in use, use the << operator instead. */
void PrintRPSDB(void);

/*Search by field value
* Pass it a list where the first record contains
* the search keys */
int SearchRPSDB(std::list<RpsDBRecord*>& return_list);
int SearchDOMNAME(std::list<DomnameDBRecord*>& return_list);

/*Collect list of domain from DOMNAME */
list<string>* DOMNAMECollectDomainIDs(void);

/*Is Exist functions */
bool IsExistInRPSDB( int gi);
bool IsExistInDOMNAME( string acc);

/* search domname for domain label by accession */
string SearchDOMNAME(string accession);
/*Search by combination of tags and evalue */
int SearchRPSDBByTagsAndEvalue(std::list<RpsDBRecord*>& return_list);

#endif
/*
$Log: rpsdb_cb.hpp,v $
Revision 1.1.1.8  2002/11/15 09:34:09  root
Automated import from Sourceforge.net

Revision 1.9  2002/11/14 19:51:44  betel
Fixed bugs in searches and new function

Revision 1.8  2002/11/01 19:46:21  betel
Small changes

Revision 1.7  2002/10/30 18:21:20  betel
Added IsExist* functions

Revision 1.6  2002/10/25 00:59:50  betel
Added 3D function and changes to DOMNAME table

Revision 1.5  2002/10/21 21:30:19  betel
Code update to new table fields

Revision 1.4  2002/04/15 20:01:19  betel
Added and Changed search strategies

Revision 1.3  2002/04/02 00:37:43  betel
Added relation query to search with multiple tags

Revision 1.2  2002/03/21 02:58:17  betel
Small changes and comments

Revision 1.1  2002/03/21 02:50:34  betel
RPSDB C++ CodeBase layer

*/
