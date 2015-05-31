/*$Id: domname.c,v 1.9 2005/04/11 21:04:17 zwang Exp $*/
/*
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


  PROGRAM: DomNameToDB  Generates DomainName Table 


  AUTHORS:     Doron Betel          betel@mshri.on.ca
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

#include "domname.h"
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#define MAX_LINE 1000

/* Global variable */
static Char g_cdd_path[PATH_MAX];
static Boolean isUpdate=FALSE;
static Int4 rowCount=0;

/* function prototypes */
Boolean RetrieveCddid(CharPtr path);
Boolean RetrievePdbid(StDomNamedbPtr pDomname, CharPtr path);
Boolean InsertDomName(StDomNamedbPtr pDomname);

/* read through file cddid.tbl, 
 * for each row in cddid.tbl, open the corresponding *.acd file */
Boolean RetrieveCddid(CharPtr path) {
	FILE *fp_cdd = NULL;
	Char file_cdd[PATH_MAX];
	Char line[600];
	Char *tokens[3] = {0};
	Int4 itokens[2] = {0};

	if (path == NULL) {
		ErrPostEx(SEV_ERROR,0,0,"RetrieveCddid: Null path");
		return FALSE;
	}
	sprintf(file_cdd, "%scddid.tbl", path);	

	/* read through file cddid.tbl, 
	 * for each row in cddid.tbl, open the corresponding *.acd file */
	if ((fp_cdd = FileOpen(file_cdd, "r")) == NULL) {
		ErrPostEx(SEV_ERROR,0,0,"RetrieveCddid: Cannot open file %s.", IsNullStr(file_cdd)); 
		return FALSE;
	}
	while (FileGets(line, 600, fp_cdd) != NULL ) {
		StDomNamedbPtr pdmn=NULL;
		/*create new DomName struct*/
		pdmn=StDomNamedbNew();
		if (pdmn == NULL) {
			ErrPostEx(SEV_ERROR,0,0,"RetrieveCddid: NULL pointer for pdmn");
			return FALSE;
		}
		ResetStDOMNAME(&pdmn);

		itokens[0] = atoi(strtok ( line, "\t" ));	/* pssmid */
		tokens[0] = strtok (NULL, "\t");		/* accession */
		tokens[1] = strtok (NULL, "\t");		/* name */	
		tokens[2] = strtok (NULL, "\t");		/* description */	
		itokens[1] = atoi(strtok (NULL, "\t"));		/* length */	

		pdmn->pssmid = itokens[0];
		if (tokens[0] != NULL) {
			pdmn->accession = StringSave(tokens[0]);
		} else {
			pdmn->accession = NULL;
		}
		if (tokens[1] != NULL) {
			pdmn->name = StringSave(tokens[1]);
		} else {
			pdmn->name = NULL;
		}
		if (tokens[2] != NULL) {
			pdmn->description = StringSave(tokens[2]);
		} else {
			pdmn->description = NULL;
		}
		pdmn->length = itokens[1];
fprintf(stdout, "pssmid is %ld, acc is %s, name is %s, length is %ld\n", pdmn->pssmid, pdmn->accession, pdmn->name, pdmn->length);
		/* open corresponding *.acd file to retrieve pdbid */
		if (RetrievePdbid(pdmn, path) == FALSE) {
			ErrPostEx(SEV_ERROR,0,0,"RetrieveCddid: Unable to get pdbid for accession=%s", IsNullStr(pdmn->accession));
			return FALSE;
		}

		/* insert to DB */
		if (InsertDomName(pdmn) == FALSE) {
			ErrPostEx(SEV_ERROR,0,0,"RetrieveCddid: Unable to insert to DB for accession=%s", IsNullStr(pdmn->accession));
			return FALSE;	
		}

	
		/*Note StDomNamedbFree calls
		* CddFree, no need to call it separately. */
		StDomNamedbFree(pdmn);

	} /* while (FileGets(line, 600, fp)) */
	FileClose(fp_cdd);

	return TRUE;

}

Boolean RetrievePdbid(StDomNamedbPtr pDomname, CharPtr path) {

	CddPtr pcdd=NULL;
	AsnIoPtr aip=NULL;
	Char acd_file[PATH_MAX];

	CddIdPtr pCddid=NULL;
	GlobalIdPtr pGlobid=NULL;
	PDBSeqIdPtr pPdbseqid=NULL;
	ValNodePtr pStrucid=NULL;

	if (pDomname == NULL) {
		ErrPostEx(SEV_FATAL,0,0,"RetrievePdbid: Null pDomname");
		return FALSE;
	}
	if (path == NULL) {
		ErrPostEx(SEV_FATAL,0,0,"RetrievePdbid: Null path");
		return FALSE;
	}
	if (pDomname->accession == NULL) {
		ErrPostEx(SEV_FATAL,0,0,"RetrievePdbid: Null accession");
		return FALSE;
	}

	/* construct file name *.acd */
	sprintf(acd_file, "%s%s.acd", path, pDomname->accession);	
	if (FileLength(acd_file) == 0) {
		ErrPostEx(SEV_ERROR,0,0,"RetrievePdbid: File %s is empty", IsNullStr(acd_file));
		return TRUE;	/* ignore the file if it has no content */
	}

	/* load Cdd */
	aip=AsnIoOpen(acd_file,"rb");
	if ( aip == NULL ){
		ErrPostEx(SEV_ERROR,0,0,"RetrievePdbid: Null aip for %s has", IsNullStr(acd_file));
		return FALSE;
	}

fprintf(stdout, "line 195 done\n");
if (pDomname->pdb_id == NULL)
fprintf(stdout, "pDomname->pdb_id is NULL line 202\n");
else
fprintf(stdout, "pDomname->pdb_id is NOT NULL line 202\n");
	pcdd=CddAsnRead(aip, NULL);
if (pDomname->pdb_id == NULL)
fprintf(stdout, "pDomname->pdb_id is NULL line 207\n");
else {
fprintf(stdout, "pDomname->pdb_id is NOT NULL line 207\n");
fprintf(stdout, "pDomname->pdb_id is %s\n", pDomname->pdb_id);
}
	AsnIoClose(aip);
	if (pcdd==NULL) {
		ErrPostEx(SEV_FATAL,0,0,"RetrievePdbid: cannot load %s", IsNullStr(acd_file));  
		return FALSE;
	}

if (pDomname->pdb_id == NULL)
fprintf(stdout, "pDomname->pdb_id is NULL before StringSave\n");
else
fprintf(stdout, "pDomname->pdb_id is NOT NULL before StringSave\n");




	/* get pdbid */
	pStrucid=pcdd->master3d;
	while (pStrucid!=NULL) {
		if(pStrucid->choice==SEQID_PDB){
			pPdbseqid=(pStrucid->data.ptrvalue);
			pDomname->pdb_id=StringSave(pPdbseqid->mol);
		} else {
			ErrPostEx(SEV_INFO,0,0,"RetrievePdbid: 3D choice field %ld",pStrucid->choice);
		}
		pStrucid=pStrucid->next;
	}
if (pDomname->pdb_id == NULL)
fprintf(stdout, "pDomname->pdb_id is NULL at the end of retrieve pdbid\n");

	pcdd = CddFree(pcdd);
	return TRUE;

}


Boolean InsertDomName(StDomNamedbPtr pDomname)
{
	if (pDomname == NULL) {
		ErrPostEx(SEV_FATAL,0,0,"InsertDomName: Null pDomname");
		return FALSE;
	}
fprintf(stdout, "Start appending\n");
	if(!AppendRecordDOMNAME(pDomname)) {
		ErrPostEx(SEV_FATAL,0,0,"InsertDomName: Cannot append to table for accession=%s", IsNullStr(pDomname->accession));
		return FALSE;
	} else {
		rowCount++;
	}

	if (!isUpdate && rowCount >= 500) {
		rowCount = 0;
		/* commit */
		if (!RPS_CommitTransaction()) {
			ErrPostEx(SEV_ERROR, 0,0,"InsertDomName: Cannot commit.");
			if (!RPS_RollBackTransaction()) {
				ErrPostEx(SEV_ERROR, 0, 0, "InsertDomName: Cannot roll back");
				return FALSE;
			}
			return FALSE;
		}
	}

	return TRUE;
}


Int2 Main()
{
	Args myargs[]={
		{"Is this an update","F", NULL, NULL, TRUE, 'u', ARG_BOOLEAN, 0.0, 0, NULL},
	};
	void(*fcn)(char*)=NULL;
	Boolean isUpdate=FALSE;
	time_t	time_now;
	CharPtr	TimeNowStr;
	CharPtr file_cdd = NULL;

	/*Error loging*/
	ErrSetOptFlags(EO_LOG_SEVERITY | EO_LOG_CODES | EO_LOG_FILELINE | EO_LOG_USRFILE);
	ErrSetLogfile("domname.log", ELOG_BANNER| ELOG_APPEND);
	ErrSetLogLevel(SEV_INFO);
	ErrSetMessageLevel(SEV_FATAL);
	time_now = GetSecs();
	TimeNowStr = ctime(&time_now);
	TimeNowStr[24] = '\0';
	ErrPostEx(SEV_INFO,0,0,"Start: %s\n",IsNullStr(TimeNowStr));

	objmmdb1AsnLoad();
  	objmmdb2AsnLoad();
	objmmdb3AsnLoad();
  
	if(!GetArgs("DomnameToDB", DIM(myargs), myargs)) {
		ErrPostEx(SEV_ERROR, 0,0,"Main: cannot get arguments");
		return -1;
	}

	/* Get CDD path from config file */
	GetAppParam("intrez", "datab","pathcdd", NULL,
		g_cdd_path, (size_t)PATH_MAX * (sizeof(Char)));

	if (myargs[0].intvalue)
		isUpdate = TRUE;

	/*Initialize code base*/
#ifdef ODBC
	if(!RPS_InitCodeBase()){
		ErrPostEx(SEV_FATAL, 0,0,"Main: cannot connect to database");
		return -1;
	}
#else
	if(!InitCodeBase()){
		ErrPostEx(SEV_FATAL, 0,0,"Main: cannot initialize Codebase");
		return -1;
	}
#endif

	/* starting transcation */
	if (!RPS_StartTransaction()) {
		ErrPostEx(SEV_ERROR, 0, 0, "Cannot start transaction!");
		return -1;
	}

	/* deleting all records from table DOMNAME */
	if (isUpdate) {
		if (!DeleteAllRecordsInDOMNAME()) {
			ErrPostEx(SEV_ERROR, 0, 0, "Cannot delete all records from DOMNAME!");
			if (!RPS_RollBackTransaction()) {
				ErrPostEx(SEV_ERROR, 0, 0, "Cannot roll back!");
				RPS_CloseCodeBase();
				return -1;
			}
			RPS_CloseCodeBase();
			return -1;
		}
	}	


	/* Open Domaname table */
	if(!OpenDOMNAME("DOMNAME", TRUE)){
		ErrPostEx(SEV_FATAL, 0,0,"Main: cannot create DOMNAME");
		return -1;
	}
fprintf(stdout, "g_cdd_path is %s\n", g_cdd_path);
	/* read through cddid.tbl and *.acd */
	if (RetrieveCddid((CharPtr)g_cdd_path) == FALSE) {
		ErrPostEx(SEV_FATAL, 0,0,"Main: cannot retrieve CDD");
		return -1;
	}

	/* commit for the last time:
	 * all changes if this is an update,
	 * otherwize the last bit of insertion */
	if (!RPS_CommitTransaction()) {
		ErrPostEx(SEV_ERROR, 0, 0, "Cannot commit transaction!");
		if (!RPS_RollBackTransaction()) {
			ErrPostEx(SEV_ERROR,0,0, "Cannot roll back!");
			RPS_CloseCodeBase();
			return -1;
		}
		RPS_CloseCodeBase();
		return -1;
	}


	/*free and close*/
	CloseDOMNAME();
#ifdef ODBC
	RPS_CloseCodeBase();
#else
	CloseCodeBase();
#endif

	time_now = GetSecs();
	TimeNowStr = ctime(&time_now);
	TimeNowStr[24] = '\0';
	ErrPostEx(SEV_INFO,0,0,"End: %s\n", IsNullStr(TimeNowStr));
}
/*
$Log: domname.c,v $
Revision 1.9  2005/04/11 21:04:17  zwang
Refactored to take files cddid.tbl and *.acd. Imcompleted. See bug 3974.

Revision 1.8  2005/02/01 21:49:20  zwang
Added ability of building a transactional database.
See bug 3441.

Revision 1.7  2004/08/19 20:33:00  zwang
Call RPS_ConnectToDB and RPS_DisconnectDB ifdef ODBC.

Revision 1.6  2004/06/29 14:18:13  mmatan

This fixes bug 1733, which involved the parser crashing when attempting to
load a certain subset of "cd" records.  The parser should now be able to
parse all CDD records without problem.

Revision 1.5  2004/06/28 14:23:55  mmatan

Slight alteration from last commit: will now put "no name" in domname name
field when it is unable to parse the name from the seqalign.

Revision 1.4  2004/06/28 14:17:18  mmatan

Altered parsing of name field from seqalign to handle case where there is no
comma to use as a reference point to parsing, as the program was segfaulting,
when there was not one.

Revision 1.3  2004/04/14 15:10:39  mmatan
This fixes attempts by the domname parser to print the pCddid data structure as
though it were a character string, and which is generally not allocated at the
times it is being called.  I replaced the pCddid reference to a reference to
pDomname->accession.

Revision 1.2  2004/01/06 22:32:14  skuznets
Code audit has been done sucessfully.
Possible NULL pointer issues has been eliminated.

Revision 1.1.1.4  2002/10/25 08:34:48  root
Automated import from Sourceforge.net

Revision 1.6  2002/10/25 00:07:17  betel
Changed and fixed DOMNAME table build

Revision 1.5  2002/09/20 00:00:45  betel
Added time logging

Revision 1.4  2002/09/19 23:42:32  betel
Added time report

Revision 1.3  2002/09/19 23:27:23  betel
Added create argument

Revision 1.2  2002/07/08 16:56:02  kaca
renamed two RPS-BLAST functions Accession is DomainId

Revision 1.1  2002/04/05 16:32:53  kaca
moved from ../parsers

Revision 1.3  2002/03/18 22:02:42  betel
Fixed Print and compile bugs

Revision 1.2  2002/03/12 21:11:20  betel
Moved rpsdb files to rps dir

Revision 1.1  2002/03/11 23:02:38  betel
DomName table parser

*/
