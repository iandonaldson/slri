/* $Id: nbr_db.c,v 1.1.1.3 2001/12/05 09:22:31 root Exp $
***************************************************************************
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
 

  PROGRAM:  SeqHound Neighbours:  Database Layer (Codebase)

  AUTHORS:  Michel Dumontier (micheld@mshri.on.ca)
            and Christopher W.V. Hogue  (hogue@mshri.on.ca)


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

#include <nbr_db_p.h>
#include <ncbi.h>
#include <slri_math.h>
#include <slri_cb.h>


static void InitNPBlastDBFields(void)
{
	pf4NPBlastuid=d4field(pd4NPBlastDB,"UID");
	pf4NPBlasteval=d4field(pd4NPBlastDB,"EVAL");
	pf4NPBlastasn=d4field(pd4NPBlastDB,"ASNSA");
}

static void InitNPDBFields(void)
{
	pf4NPord=d4field(pd4NPDB,"ORD");
	pf4NPgi =d4field(pd4NPDB,"GI");
	pf4NPnum=d4field(pd4NPDB,"NUM");
	pf4NPasn=d4field(pd4NPDB,"ASNNBR");
}

static void InitND3Fields(void)
{
	pf4N3Dgi=d4field(pd4N3D,"GI");
	pf4N3Dnum=d4field(pd4N3D,"NUM");
	pf4N3Dasn=d4field(pd4N3D,"ASN");
}

Boolean OpenNPBlastDB(Boolean CREATE)
{
	Char pathdatab[PATH_MAX];
	
	if(!GetAppParam("intrez", "datab", "pathnbr", NULL, pathdatab, (size_t)PATH_MAX * (sizeof(Char)))) {
		return FALSE;
	}
	StringCat(pathdatab, "B");
	return (OpenDBII(pathdatab, &pd4NPBlastDB, f4iNPBlastDB, t4iNPBlastDB, (InitFieldFunc) InitNPBlastDBFields, &cb, CREATE));
}

Boolean OpenNPDB(Boolean CREATE)
{
	Char pathdatab[PATH_MAX];
	
	if(!GetAppParam("intrez", "datab", "pathnbr", NULL, pathdatab, (size_t)PATH_MAX * (sizeof(Char)))) {
		return FALSE;
	}
	StringCat(pathdatab, "N");
	return (OpenDBII(pathdatab, &pd4NPDB, f4iNPDB, t4iNPDB, (InitFieldFunc) InitNPDBFields, &cb, CREATE));
}

Boolean OpenN3D(Boolean CREATE)
{
	Char pathdatab[PATH_MAX];
	
	if(!GetAppParam("intrez", "datab", "pathnbr", NULL, pathdatab, (size_t)PATH_MAX * (sizeof(Char)))) {
		return FALSE;
	}
	StringCat(pathdatab, "3D");
	return (OpenDBII(pathdatab, &pd4NPDB, f4iNPDB, t4iNPDB, (InitFieldFunc) InitNPDBFields, &cb, CREATE));
}

Boolean OpenAllNbrDBs(Boolean CREATE)
{
	Boolean c1=FALSE, c2=FALSE;

	/* Use this with both Blast and Neighbour databases available */
	c1 = OpenNPBlastDB(CREATE);
	c2 = OpenNPDB(CREATE);
	/* c3 = OpenN3D(CREATE); */

	if(c1 && c2) return TRUE;
	
	else return FALSE;
}

void InitCodeBaseNbr(void)
{
	code4init(&cb);
	cb.errOpen = 0;
	cb.safety = 0;
}

Boolean OpenCodeBaseNbr(Boolean CREATE)
{
	InitCodeBaseNbr();
	return OpenAllNbrDBs(CREATE);
}

void CloseCodeBaseNbr(void)
{
	/*explicitly flush all DB's*/
	d4flush(pd4NPBlastDB);
	d4flush(pd4NPDB);
	/*close Codebase*/
	code4close(&cb);
	code4initUndo(&cb);
}

/* Close the Blast and Neighbour protein databases*/
void CloseAllNbrDBs(void)
{
	if(pd4NPBlastDB) d4close(pd4NPBlastDB);
	if(pd4NPDB) d4close(pd4NPDB);
	if(pd4N3D) d4close(pd4N3D);
}

/**************************************************************************
* Database Retrieval Functions                                            *
***************************************************************************
* GetBlastStrucFromDB             Get Blast Result set from BlastDB       *
* GetNBlastStrucFromDB            Get Neighbour set from NBlastDB         *
* GetNumberofNeighboursListFromDB Get the number of neighbours for each GI*
**************************************************************************/

SLRI_ERR ConvertBlastResultToSeqAlign(NBlastResultSetPtr pResultSet, SeqAlignPtr PNTR psap)
{
	NBlastResultPtr pResult = NULL, pResultTemp = NULL;
	DenseSegPtr dsp = NULL;
	SeqAlignPtr sap = NULL;
	ValNodePtr sip = NULL;
	ScorePtr sp1 = NULL, sp2 = NULL;
	ObjectIdPtr obid = NULL;
	Int4 num = 0;


	if(pResultSet == NULL) {
		ErrPostEx(SEV_ERROR,0,0,"Invalid ResultSetPtr");
		return SLRI_FAIL;
	}

	if((pResult = pResultSet->results) == NULL) {
		ErrPostEx(SEV_ERROR,0,0,"Invalid ResultPtr");
		return SLRI_FAIL;
	}

	sap = SeqAlignNew();

	sap->segtype = 2; /* Dense Seg */
	dsp = DenseSegNew();
	/* Results */
	pResultTemp = (NBlastResultPtr) AsnIoMemCopy(pResult,(AsnReadFunc) NBlastResultAsnRead,(AsnWriteFunc) NBlastResultAsnWrite);
	dsp->numseg = pResultTemp->numseg;
	Misc_ValNodeListToInt4Array(pResultTemp->starts,&(dsp->starts),&num); /* size of this should be num (2) * numseg */
	Misc_ValNodeListToInt4Array(pResultTemp->lens,&(dsp->lens),&num);
	dsp->numseg = (Int2) num;
	
	
	NBlastResultFree(pResultTemp);

	sap->segs = (Pointer) dsp;

	/* GIs */
	ValNodeAddInt(&sip,SEQID_GI,pResultSet->query_gi);
	ValNodeAddInt(&sip,SEQID_GI,pResultSet->subject_gi);	
	dsp->ids = sip;

	/* Scores */
	sp1 = ScoreNew();
	obid = ObjectIdNew();
	obid->str = StringSave("bit_score");
	sp1->id = obid;
	sp1->choice = 1;
	sp1->value.intvalue = pResult->scores->bitscore;
	obid = NULL;
	sp2 = ScoreNew();
	obid = ObjectIdNew();
	obid->str = StringSave("e_value");
	sp2->id = obid;
	sp2->choice = 2;
	sp2->value.realvalue = pResult->scores->evalue;

	sp1->next = sp2;
	dsp->scores = sp1;

	*psap = sap;
	return SLRI_SUCCESS;
}

SLRI_ERR GetUIDfromGIPair(Int4 gi1, Int4 gi2, Int8 PNTR puid)
{
	Int4 ord1 = 0, ord2 = 0;

	d4tagSelect(pd4NPDB, d4tag(pd4NPDB, "GI_TAG"));
	if(d4seekDouble(pd4NPDB,(double)gi1) != r4success) {
		ErrPostEx(SEV_ERROR,0,0,"GetBlastSeqAlignFromDB: Could not find gi %ld in NPDB",gi1);
		return SLRI_FAIL;
	}
	ord1 = (Int4) f4long(pf4NPord);
	if(d4seekDouble(pd4NPDB,(double)gi2) != r4success) {
		ErrPostEx(SEV_ERROR,0,0,"GetBlastSeqAlignFromDB: Could not find gi %ld in NPDB",gi2);
		return SLRI_FAIL;
	}
	ord2 = (Int4) f4long(pf4NPord);
	
	if(ord1 < ord2) {
		ORDtoUID(ord1,ord2,&(*puid));
	} else {
		ORDtoUID(ord2,ord1,&(*puid));
	}

	return SLRI_SUCCESS;
}


SLRI_ERR GetBlastSeqAlignFromDB(Int4 gi1, Int4 gi2, SeqAlignPtr PNTR psap)
{
	NBlastResultSetPtr pResultSet = NULL;
	Int8 uid = 0;

	GetUIDfromGIPair(gi1,gi2,&uid);
	if(uid <= 0) {
		ErrPostEx(SEV_ERROR,0,0, "GetBlastSeqalignFromUID: Invalid Parameters");
		return SLRI_FAIL;
	}

	d4tagSelect(pd4NPBlastDB, d4tag(pd4NPBlastDB, "UID_TAG"));
	if((d4seekDouble(pd4NPBlastDB,(double)uid)) == r4success) {
		if(psap) {
			pResultSet = (NBlastResultSetPtr) GetASNMemMemo(pf4NPBlastasn, (AsnReadFunc) NBlastResultSetAsnRead);
			if(pResultSet) ConvertBlastResultToSeqAlign(pResultSet,&(*psap));
		}
	} else {
		/* ErrPostEx(SEV_WARNING,0,0, "GetBlastSeqalignFromUID: Cannot find uid in BlastDB."); */
	}
	return SLRI_SUCCESS;
}

	

SLRI_ERR GetBlastResultsFromDB(Int4 gi1, Int4 gi2, NBlastResultSetPtr PNTR nbrsp)
{
	Int8 uid = 0;

	GetUIDfromGIPair(gi1,gi2,&uid);
	if(uid <= 0) {
		ErrPostEx(SEV_ERROR,0,0, "GetBlastResultsFromDB: Invalid Parameters");
		return SLRI_FAIL;
	}

	d4tagSelect(pd4NPBlastDB, d4tag(pd4NPBlastDB, "UID_TAG"));
	if((d4seekDouble(pd4NPBlastDB,(double)uid)) == r4success) {
		if(nbrsp) *nbrsp = (NBlastResultSetPtr) GetASNMemMemo(pf4NPBlastasn, (AsnReadFunc) NBlastResultSetAsnRead);
	} else {
		/* ErrPostEx(SEV_WARNING,0,0, "GetBlastResultsFromDB: Cannot find uid in BlastDB."); */
	}
	return SLRI_SUCCESS;
}

SLRI_ERR GetNeighboursFromDB(Int4 gi, Int4 PNTR pnum, NBlastGiAndEvalSetPtr PNTR nbgesp)
{
	if(gi < 0) {
		ErrPostEx(SEV_ERROR,0,0, "GetNeighboursFromDB: Invalid Parameters");
		return SLRI_FAIL;
	}
		
	d4tagSelect(pd4NPDB, d4tag(pd4NPDB, "GI_TAG"));
	if((d4seekDouble(pd4NPDB,(double)gi)) == r4success) {
		if(nbgesp) *nbgesp = (NBlastGiAndEvalSetPtr) GetASNMemMemo(pf4NPasn, (AsnReadFunc) NBlastGiAndEvalSetAsnRead);
		if(pnum) *pnum = (Int4) f4long(pf4NPnum);
	} else {	
		/* ErrPostEx(SEV_WARNING,0,0, "GetNeighboursFromDB: Cannot find gi %ld in NBlastDB.",gi); */
	}
	return SLRI_SUCCESS;
}


SLRI_ERR Get3DNeighboursFromDB(Int4 gi, Int4 PNTR pnum, ILinkSetPtr PNTR pp3DNeighbours)
{
	if(gi < 0) {
		ErrPostEx(SEV_ERROR,0,0, "Get3DNeighboursFromDB: Invalid Parameters");
		return SLRI_FAIL;
	}
		
	d4tagSelect(pd4N3D, d4tag(pd4N3D, "GI_TAG"));
	if((d4seekDouble(pd4N3D,(double)gi)) == r4success) {
		if(pp3DNeighbours) *pp3DNeighbours = (ILinkSetPtr) GetASNMemMemo(pf4N3Dasn, (AsnReadFunc) ILinkSetAsnRead);
		if(pnum) *pnum = (Int4) f4long(pf4N3Dnum);
	} else {	
		/* ErrPostEx(SEV_WARNING,0,0, "Get3DNeighboursStrucFromDB: Cannot find gi %ld in N3D.",gi); */
	}
	return SLRI_SUCCESS;
}


SLRI_ERR GetNumberofNeighboursListFromDB(Int4 PNTR pnum, ILinkSetPtr PNTR ppNumGiList, Int4 iDB)
{
	Int4 rc = 0, recs = 0, i = 0;
	ILinkSetPtr pNumGiList = NULL;
	DATA4 *phere = NULL;
	FIELD4 *pf4gihere = NULL;
	FIELD4 *pf4numhere = NULL;

	if(pnum == NULL || ppNumGiList == NULL) {
		ErrPostEx(SEV_ERROR,0,0, "GetNumberofNeighboursListFromDB: Invalid Parameters");
		return SLRI_FAIL;
	}
	
	if(iDB == NBLASTDB) {
		phere = pd4NPDB;
		pf4gihere = pf4NPgi;
		pf4numhere = pf4NPnum;
	} else if(iDB == N3D) {
		phere = pd4N3D;
		pf4gihere = pf4N3Dgi;
		pf4numhere = pf4N3Dnum;
	}

	if((recs = d4recCount(phere)) == 0) {
		ErrPostEx(SEV_ERROR,0,0, "GetNumberofNeighboursList: No records in Database!");
		return SLRI_FAIL;
	}

	if((pNumGiList = ILinkSetNew2(recs)) == NULL) {
		ErrPostEx(SEV_ERROR,0,0, "GetNumberofNeighboursList: Could not allocate memory for array.");
		return SLRI_FAIL;
	}

	d4tagSelect(phere, d4tag(phere, "NUM_TAG"));
	for(rc = d4top(phere); rc == r4success; rc = d4skip(phere,1L)) {
		pNumGiList->uids[i] = (Int4) f4long(pf4gihere);
		pNumGiList->weights[i] = (Int4) f4long(pf4numhere);
		i++;
	}

	*ppNumGiList = pNumGiList;	
	return SLRI_SUCCESS;
}

/*
$Log: nbr_db.c,v $
Revision 1.1.1.3  2001/12/05 09:22:31  root
Automated import from Sourceforge.net

Revision 1.3  2001/12/05 00:52:07  micheld
Added access to BLAST pairwise results... returns either NBLASTDB format or seqalign

Revision 1.2  2001/11/21 23:51:16  kaca
database opening controled by CREATE Boolean

Revision 1.1.1.1  2001/10/10 21:53:48  kaca
initial import

Revision 1.7  2001/09/03 20:31:08  michel
Mainly fixed compiler warnings and changed CODE4 reference for GO and LL

Revision 1.6  2001/08/24 20:59:44  michel
removed SEQHOUNDNBRS compile flag from library
added new functions for 3DNeighbours database retrieval
temporary fix for redundant neighbours coming from NBLAST

Revision 1.5  2001/08/15 19:07:37  michel
bug fixes on new functions

Revision 1.4  2001/08/15 18:11:47  michel
Added pathnbr for config file to find neighbour database
Added additional functions and error checking

Revision 1.3  2001/08/14 14:29:59  michel
Fixed and tested Neighbour Functions, with the exception of 3D Neighbours
Tested Get3DListFromTaxID - returns all redundant group's 3d structures

Revision 1.2  2001/08/07 18:54:49  michel
Tested SHoundNeighboursFromGiEx
Added the rest of the remote API and CGI calls (untested)

Revision 1.1  2001/08/02 16:13:32  michel
Added Neighbour Retrieval to SeqHound local

*/


