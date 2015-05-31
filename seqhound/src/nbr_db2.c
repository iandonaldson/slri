/* $Id: nbr_db2.c,v 1.2 2005/04/26 14:30:28 rcavero Exp $
***************************************************************************
  Copyright (C) 2005 Mount Sinai Hospital (MSH)

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
 

  PROGRAM:  SeqHound Neighbours:  Database Layer (DB2)

  AUTHORS:  Marc Dumontier (mdumontier@mdsproteomics.com)
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

#include <sqlcli1.h>
#include <ncbi.h>
#include <nbr_db.h>
#include <slri_math.h>
#include <slri_db2.h>

  
static SQLHANDLE henv; /*envirnment handle */
static SQLHANDLE hdbc; /* connection handle */
static SQLHANDLE hstmt_nblast;
static SQLHANDLE hstmt_nblastdb;
static SQLHANDLE countNeighboursStmt;
static SQLHANDLE hLoc;

static void InitNPBlastDBFields(void)
{
}

static void InitNPDBFields(void)
{
}

static void InitND3Fields(void)
{
}

Boolean OpenNPBlastDB(Boolean CREATE)
{
	return TRUE;
}

Boolean OpenNPDB(Boolean CREATE)
{
}

Boolean OpenN3D(Boolean CREATE)
{
}

Boolean OpenAllNbrDBs(Boolean CREATE)
{
	return (OpenCodeBaseNbr(FALSE));
}

void InitCodeBaseNbr(void)
{
}

Boolean OpenCodeBaseNbr(Boolean CREATE)
{
  SQLCHAR db2alias[10];
  SQLCHAR usr[10];
  SQLCHAR passwd[10]; 
  
  GetAppParam("intrez", "datab", "username", NULL, (Char*)usr, (size_t)10 * (sizeof(Char)));
  GetAppParam("intrez", "datab", "password", NULL, (Char*)passwd, (size_t)10 * (sizeof(Char)));
  GetAppParam("intrez", "datab", "db2alias", NULL, (Char*)db2alias, (size_t)10 * (sizeof(Char)));
  
   /* allocate an envirnment handle */
   if (SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv) != SQL_SUCCESS){
     print_err(henv);
     ErrPostEx(SEV_ERROR,0,0, "Unable to allocate envirnment handle.");
   }

   /* allocate a connection handle */
   if (SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc) == SQL_ERROR){
        print_err(henv);
        ErrPostEx(SEV_ERROR,0,0, "An error has occurred!  Bastards!");
   }

   if(SQLConnect( hdbc, db2alias, SQL_NTS, usr, SQL_NTS, passwd, SQL_NTS) != SQL_SUCCESS){
        ErrPostEx(SEV_ERROR,0,0, "could not connect to the database. ");
   }


  if(SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hLoc)!=SQL_SUCCESS){
                printf("CANNOT ALLOCATE BLOB HANDLE!!!<br>");
                return FALSE;
   }
 
   CreateHandle(&hstmt_nblastdb, "SELECT ASNNBR,NUM from seqhound.nblastdb where gi = ?",&hdbc,&henv);
   CreateHandle(&countNeighboursStmt,"SELECT COUNT(*) FROM seqhound.nblastdb",&hdbc,&henv);
   return (TRUE);
}

void CloseCodeBaseNbr(void)
{
}

/* Close the Blast and Neighbour protein databases*/
void CloseAllNbrDBs(void)
{
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
/*	NBlastResultPtr pResult = NULL, pResultTemp = NULL;
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

	sap->segtype = 2; 
	dsp = DenseSegNew();
	pResultTemp = (NBlastResultPtr) AsnIoMemCopy(pResult,(AsnReadFunc) NBlastResultAsnRead,(AsnWriteFunc) NBlastResultAsnWrite);
	dsp->numseg = pResultTemp->numseg;
	Misc_ValNodeListToInt4Array(pResultTemp->starts,&(dsp->starts),&num); 
	Misc_ValNodeListToInt4Array(pResultTemp->lens,&(dsp->lens),&num);
	dsp->numseg = (Int2) num;
	
	
	NBlastResultFree(pResultTemp);

	sap->segs = (Pointer) dsp;

	ValNodeAddInt(&sip,SEQID_GI,pResultSet->query_gi);
	ValNodeAddInt(&sip,SEQID_GI,pResultSet->subject_gi);	
	dsp->ids = sip;

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
	*/
	return SLRI_FAIL;
}

SLRI_ERR GetUIDfromGIPair(Int4 gi1, Int4 gi2, Int8 PNTR puid)
{
/*	Int4 ord1 = 0, ord2 = 0;

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
*/
	return SLRI_FAIL;
}


SLRI_ERR GetBlastSeqAlignFromDB(Int4 gi1, Int4 gi2, SeqAlignPtr PNTR psap)
{
/*	NBlastResultSetPtr pResultSet = NULL;
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
	}
*/	return SLRI_SUCCESS;
}

	

SLRI_ERR GetBlastResultsFromDB(Int4 gi1, Int4 gi2, NBlastResultSetPtr PNTR nbrsp)
{
/*	Int8 uid = 0;

	GetUIDfromGIPair(gi1,gi2,&uid);
	if(uid <= 0) {
		ErrPostEx(SEV_ERROR,0,0, "GetBlastResultsFromDB: Invalid Parameters");
		return SLRI_FAIL;
	}

	d4tagSelect(pd4NPBlastDB, d4tag(pd4NPBlastDB, "UID_TAG"));
	if((d4seekDouble(pd4NPBlastDB,(double)uid)) == r4success) {
		if(nbrsp) *nbrsp = (NBlastResultSetPtr) GetASNMemMemo(pf4NPBlastasn, (AsnReadFunc) NBlastResultSetAsnRead);
	} else {
	}
*/	return SLRI_SUCCESS;
}

SLRI_ERR GetNeighboursFromDB(Int4 gi, Int4 PNTR pnum, NBlastGiAndEvalSetPtr PNTR nbgesp)
{
        SQLRETURN rc=SQL_SUCCESS;
        SQLINTEGER blobloc,pbbvalue,bloblen,indicator;
        CharPtr pcASN=NULL;
        AsnIoMemPtr aimp;
        Int4 n=0;
        
        SQLBindParameter(hstmt_nblastdb, 1, SQL_PARAM_INPUT, SQL_C_LONG,SQL_INTEGER, 0, 0, &gi, 0, NULL);
        
        if (SQLExecute (hstmt_nblastdb) != SQL_SUCCESS){
                        print_err(hstmt_nblastdb);
                        return SLRI_FAIL;
        }
  
        if((rc=SQLBindCol(hstmt_nblastdb,1,SQL_C_BLOB_LOCATOR,&blobloc,0,&pbbvalue))!=SQL_SUCCESS)
                {print_err(hstmt_nblastdb);return SLRI_FAIL;}
 
        if((rc=SQLBindCol(hstmt_nblastdb,2,SQL_C_LONG,&n,0,0))!=SQL_SUCCESS)
                {print_err(hstmt_nblastdb); return SLRI_FAIL;}


        if((rc=SQLFetch(hstmt_nblastdb))!=SQL_SUCCESS)
        {
                if(rc == SQL_ERROR) {
                        print_err(hstmt_nblastdb);
                        return SLRI_FAIL;
                }
                else if(rc == SQL_NO_DATA_FOUND){
		        *pnum = n;
		        SQLFreeStmt(hstmt_nblastdb,SQL_CLOSE);
			return 0;
                }
        }

	if(n == 0) {
		/*success but 0 neighbours*/
	        *pnum = n;
	        SQLFreeStmt(hstmt_nblastdb,SQL_CLOSE);
		return 0;
	}
 
        if((rc=SQLGetLength(hLoc, SQL_C_BLOB_LOCATOR, blobloc, &bloblen, &indicator))!=SQL_SUCCESS){
                print_err(hstmt_nblastdb);
                SQLFreeStmt(hLoc,SQL_CLOSE);
                return SLRI_FAIL;
        }
        if(bloblen<=0) return -1;

       pcASN = (CharPtr) MemNew((size_t) bloblen+10);
        
        if((rc=SQLGetData(hstmt_nblastdb,1,SQL_C_BINARY,(SQLPOINTER)pcASN,bloblen+10,&indicator)) != SQL_SUCCESS)
                {print_err(hstmt_nblastdb);return SLRI_FAIL;}
         
        aimp = AsnIoMemOpen("rb", (BytePtr) pcASN, bloblen);
                        
        if (aimp == NULL)
        {
                ErrPostEx(SEV_ERROR,0,0, "GetNBlastStrucFromDb: Failed to get AsnIo.");
                if (pcASN) MemFree(pcASN);
                return SLRI_FAIL;
        }

	if(nbgesp)                
        	*nbgesp =  NBlastGiAndEvalSetAsnRead(aimp->aip, NULL);
	if(pnum)
        	*pnum = n;
        
        if (pcASN)
                MemFree(pcASN);
        AsnIoMemClose(aimp);
        SQLFreeStmt(hstmt_nblastdb,SQL_CLOSE);
      SQLFreeStmt(hLoc,SQL_CLOSE);
        
        return 0;
}


SLRI_ERR Get3DNeighboursFromDB(Int4 gi, Int4 PNTR pnum, ILinkSetPtr PNTR pp3DNeighbours)
{
	/*not yet implemented in either codebase or db2*/
	return SLRI_FAIL;
}


SLRI_ERR GetNumberofNeighboursListFromDB(Int4 PNTR pnum, ILinkSetPtr PNTR ppNumGiList, Int4 iDB)
{
	Int4 recs = 0, i = 0;
	ILinkSetPtr pNumGiList = NULL;
        SQLRETURN sqlrc=SQL_SUCCESS;
        SQLHANDLE hstmt;
        SQLCHAR stmt[255];
        Int4 rowsFetchedNb=0,rowStatus=0;
        Int4* gival, *numval;


	if(pnum == NULL || ppNumGiList == NULL) {
		ErrPostEx(SEV_ERROR,0,0, "GetNumberofNeighboursListFromDB: Invalid Parameters");
		return SLRI_FAIL;
	}

	if (SQLExecute (countNeighboursStmt) != SQL_SUCCESS){
		print_err(countNeighboursStmt);
		return SLRI_FAIL;
	}
	if(SQLFetch(countNeighboursStmt)!=SQL_SUCCESS){
		print_err(countNeighboursStmt);
		return SLRI_FAIL;
	}
	if(SQLGetData(countNeighboursStmt,1,SQL_C_LONG,&recs,0,0) != SQL_SUCCESS){
		print_err(countNeighboursStmt);
		return SLRI_FAIL;
	}
	SQLFreeStmt(countNeighboursStmt,SQL_CLOSE);
        

	if((pNumGiList = ILinkSetNew2(recs)) == NULL) {
		ErrPostEx(SEV_ERROR,0,0, "GetNumberofNeighboursList: Could not allocate memory for array.");
		return SLRI_FAIL;
	}

                
        gival = (Int4*)MemNew(sizeof(Int4) * recs);
        numval = (Int4*)MemNew(sizeof(Int4) * recs);

	sprintf((Char*)stmt,"SELECT GI,NUM from seqhound.nblastdb");

        if((sqlrc=SQLAllocStmt(hdbc,&hstmt)) != SQL_SUCCESS)
                print_err(hstmt);


    sqlrc = SQLSetStmtAttr( hstmt,
                         SQL_ATTR_ROW_ARRAY_SIZE,
                         (SQLPOINTER) recs,
                         0); 
    
    sqlrc = SQLSetStmtAttr( hstmt,
                         SQL_ATTR_ROW_BIND_TYPE,
                         SQL_BIND_BY_COLUMN,
                         0);
    
    sqlrc = SQLSetStmtAttr( hstmt,
                         SQL_ATTR_ROWS_FETCHED_PTR,
                         &rowsFetchedNb,
                         0);
   
    sqlrc = SQLSetStmtAttr( hstmt,
                         SQL_ATTR_CURSOR_TYPE, 
                         (SQLPOINTER) SQL_CURSOR_STATIC, 
                         0);
    
    sqlrc = SQLSetStmtAttr( hstmt,
                         SQL_ATTR_USE_BOOKMARKS,
                         (SQLPOINTER) SQL_UB_VARIABLE,
                         0);
    
    sqlrc = SQLSetStmtAttr( hstmt,
                         SQL_ATTR_ROW_STATUS_PTR,
                         (SQLPOINTER) rowStatus,
                         0);
   
        if((sqlrc=SQLExecDirect(hstmt,stmt,SQL_NTS))!=SQL_SUCCESS)
                print_err(hstmt);

        if((sqlrc=SQLBindCol(hstmt,1,SQL_C_LONG,gival,0,0))!=SQL_SUCCESS)
                print_err(hstmt);

        if((sqlrc=SQLBindCol(hstmt,2,SQL_C_LONG,numval,0,0))!=SQL_SUCCESS)
                print_err(hstmt);

        sqlrc = SQLFetchScroll(hstmt,SQL_FETCH_ABSOLUTE,1);

        if(sqlrc != SQL_SUCCESS)
        {
                print_err(hstmt);
        }

        for(i=0;i<rowsFetchedNb;i++) {
		pNumGiList->uids[i] = (Int4) gival[i];
		pNumGiList->weights[i] = (Int4) numval[i];
		i++;
	}

	*ppNumGiList = pNumGiList;	

        MemFree(gival);
	MemFree(numval);
        SQLFreeStmt(hstmt,SQL_DROP);
	return SLRI_SUCCESS;
}

/*
$Log: nbr_db2.c,v $
Revision 1.2  2005/04/26 14:30:28  rcavero
Change Copyright Note.

Revision 1.1.1.6  2002/03/04 09:25:29  root
Automated import from Sourceforge.net

Revision 1.6  2002/03/03 23:02:52  gbader
Fixed MSVC compiler warnings - mostly unused variables and uncasted type changes.

Revision 1.5  2002/02/28 17:23:02  micheld
New MSVC project files to build cb/db2/rem apps and libraries
Moved include order to accomodate msvc, removed #ifdef [CODEBASE|DB2]

Revision 1.4  2002/02/12 16:21:23  mdumontier
fixed warnings

Revision 1.3  2002/02/06 14:26:56  mdumontier

minor bug fixes:

Revision 1.2  2002/01/10 21:15:57  mdumontier
changes to neighbours db2...added a check for 0 neighbour case and implemented getting total neighbours

Revision 1.1  2002/01/03 20:44:28  risserlin
added additional db2 layer files

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


