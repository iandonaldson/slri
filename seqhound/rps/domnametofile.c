/*$Id: domnametofile.c,v 1.1 2005/04/08 20:46:52 zwang Exp $*/
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
static FILE *fp = NULL;
static CharPtr outFile = "pdbid.txt";

Boolean FillDomNameNode(StDomNamedbPtr pDomname, CddPtr pCdd)
{
	CddIdPtr pCddid=NULL;
	GlobalIdPtr pGlobid=NULL;
	PDBSeqIdPtr pPdbseqid=NULL;
	ValNodePtr pStrucid=NULL;

	if(pCdd==NULL)
	{
		ErrPostEx(SEV_ERROR, 0,0,"[FillDomNameNode]:Cdd pointer is null");
		return FALSE;
	}

	/* SK: NULL ptr check */
	if(pDomname==NULL)
	{
		ErrPostEx(SEV_ERROR, 0,0,"[FillDomNameNode]:Domname pointer is null");
		return FALSE;
	}
	/*Get domain's accession */
	pCddid=pCdd->id;
	while(pCddid!=NULL){
		if(pCddid->choice==CddId_gid){
			pGlobid=(pCddid->data.ptrvalue);
			if(pGlobid->accession==NULL){
				ErrPostEx(SEV_ERROR, 0,0,"[FillDomNameNode]:No Accession found");
				break;
			}
			else{
				pDomname->accession=StringSave(pGlobid->accession);
			}
		}
		pCddid=pCddid->next;
	}
	/*get pdb-id*/
	pStrucid=pCdd->master3d;
	while(pStrucid!=NULL){
		if(pStrucid->choice==SEQID_PDB){
			pPdbseqid=(pStrucid->data.ptrvalue);
			/*printf("3D %s ",IsNullStr(pPdbseqid->mol));*/
			pDomname->pdb_id=StringSave(pPdbseqid->mol);
		}
		else{
			ErrPostEx(SEV_INFO,0,0,"3D choice field %ld",pStrucid->choice);
		}
		pStrucid=pStrucid->next;
	}

	/* print to the output file */
	fprintf(fp, "%s\t", pDomname->accession);
	if (pDomname->pdb_id != NULL) {
		fprintf(fp, "%s", pDomname->pdb_id);
	}
	fprintf(fp, "\n");

	return TRUE;
}

/*Generate DomName record*/
void GenerateDomName(char* filename)
{
	CddPtr pcdd=NULL;
	StDomNamedbPtr pdmn=NULL;
	AsnIoPtr aip=NULL;
	CharPtr accession = NULL;

	/* SK: NULL ptr check */
	if ( filename == NULL )
	    return;

	/* return if the file is empty */
	if (FileLength(filename) == 0) {
		ErrPostEx(SEV_INFO,0,0,"GenerateDomName: File %s is empty", IsNullStr(filename));
		return;
	}


	/*load Cdd <domain>.acd */
	aip=AsnIoOpen(filename,"rb");
	/* SK: NULL ptr check */
	if ( aip == NULL )
	    return;
	pcdd=CddAsnRead(aip, NULL);
	AsnIoClose(aip);
	if(pcdd==NULL)
	{
		ErrPostEx(SEV_FATAL,0,0,"GenerateDomName:cannot load %s", IsNullStr(filename));
		return;
	}

	/*create new DomName struct*/
	pdmn=StDomNamedbNew();
	ResetStDOMNAME(&pdmn);

	/* write to output file */
	if(!FillDomNameNode(pdmn, pcdd)){
		ErrPostEx(SEV_FATAL, 0,0,"GenerateDomName:Generating %s record failed",IsNullStr(filename));
		return;
	}

	/*Note StDomNamedbFree calls
	* CddFree, no need to call it separately. */
	pcdd=CddFree(pcdd);
	StDomNamedbFree(pdmn);
	return;
}

/* Walk through dir and call fcn for every matched file */
void DirWalk(CharPtr dir, void (*fcn)(char *))
{
	char name[PATH_MAX];
	struct dirent* dp=NULL;
	DIR* dfd=NULL;
	char* temp=NULL;
	Int4 counter=0;
	/* SK: NULL ptr check */
	if ( dir == NULL )
	{
	    ErrPostEx(SEV_FATAL, 0,0,"DirWalk: NULL dir" );
	    return;
	}
	/* SK: NULL ptr check */
	if ( fcn == NULL )
	{
	    ErrPostEx(SEV_FATAL, 0,0,"DirWalk: NULL fcn" );
	    return;
	}
	if((dfd=opendir(dir))==NULL){
		ErrPostEx(SEV_FATAL, 0,0,"Main: cannot open %s", IsNullStr(dir));
		return;
	}
	while((dp=readdir(dfd))!=NULL){
		/*skip all non-acd files*/
		temp=strchr(dp->d_name, '.');
		if( strcmp(dp->d_name,".")==0|| strcmp(dp->d_name, "..")==0 ||
			temp==NULL || strcmp(temp,".acd")!=0 )
			continue;
		sprintf(name, "%s/%s", IsNullStr(dir), IsNullStr(dp->d_name));
		(*fcn)(name);
		temp=NULL;
		counter++;
	}
	ErrPostEx(SEV_INFO,0,0,"[DirWalk]: %ld Domains written to file %s",(long)counter, outFile);
	closedir(dfd);
	return;
}

Int2 Main()
{
	void(*fcn)(char*)=NULL;
	Boolean isUpdate=FALSE;
	time_t	time_now;
	CharPtr	TimeNowStr;

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

        /* Get CDD path from config file */
	GetAppParam("intrez", "datab","pathcdd", NULL,
		g_cdd_path, (size_t)PATH_MAX * (sizeof(Char)));

	/*Walk through cdd dir, generate a tab delimited file of filename and pdbid */
	if ((fp=FileOpen(outFile, "w")) == NULL) {
		ErrPostEx(SEV_ERROR, 0,0,"Main: Unable to open file %s", outFile);
		return -1;
	}
	fcn=GenerateDomName;
	DirWalk(g_cdd_path, fcn);
	FileClose(fp);

	time_now = GetSecs();
	TimeNowStr = ctime(&time_now);
	TimeNowStr[24] = '\0';
	ErrPostEx(SEV_INFO,0,0,"End: %s\n", IsNullStr(TimeNowStr));
}
/*
$Log: domnametofile.c,v $
Revision 1.1  2005/04/08 20:46:52  zwang
New file to adapt the change in the source file.
Iterate through all *.acd in a given directory, print pairs of accession and pdbid into a file.
Ignore empty .acd files.
See Bug 3974.

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
