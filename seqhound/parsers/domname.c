/*$Id: domname.c,v 1.2 2004/01/06 22:32:13 skuznets Exp $*/
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

Boolean FillDomNameNode(StDomNamedbPtr pDomname, CddPtr pCdd)
{
	CddIdPtr pCddid=NULL;
	GlobalIdPtr pGlobid=NULL;
	BiostrucAnnotSetPtr pStrucAnnot=NULL;
	ValNodePtr pStrucid=NULL;

	FILE* csq_file=NULL;
	Char buff[MAX_LINE];
	CharPtr temp=NULL, name=NULL;
	Char csq[MAX_LINE];

	if(pCdd==NULL)
	{
		ErrPostEx(SEV_ERROR, 0,0,"[FillDomNameNode]:Cdd pointer is null");
		return FALSE;
	}
	/* SK: NULL ptr check */
	if ( pDomname == NULL )
	{
		ErrPostEx(SEV_ERROR, 0,0,"[FillDomNameNode]:Domname pointer is null");
		return FALSE;
	}

	/*Get domain's accession */
	pCddid=pCdd->id;
	while(pCddid!=NULL){
		if(pCddid->choice==CddId_gid){
			pGlobid=(pCddid->data.ptrvalue);
			/* SK: NULL ptr check */
			if((pGlobid == NULL)||(pGlobid->accession==NULL)){
				ErrPostEx(SEV_ERROR, 0,0,"[FillDomNameNode]:No Accession found");
				break;
			}
			else{
				pDomname->accession=StringSave(pGlobid->accession);
			}
		}
		pCddid=pCddid->next;
	}

	/* Domain's short lable */
	/*Domains description is placed in <domain>.csq files*/
	StringCpy(csq, g_cdd_path);
	StringCat(csq, pDomname->accession);
	StringCat(csq,".csq");
	csq_file=FileOpen(csq, "rb");
	if(csq_file==NULL)
	{
		ErrPostEx(SEV_FATAL, 0,0,"[FillDomNameNode]:Cannot open %s file",IsNullStr(csq) );
		return FALSE;
	}

	fgets(buff, MAX_LINE-1, csq_file);
	name=strchr(buff, ' ');
	name++;
	temp=strchr(name, ',');
	temp[0]='\0';
	pDomname->name=StringSave(name);
	temp[0]=',';
	FileClose(csq_file);

	/*get mmdbi-id*/
	pStrucAnnot=pCdd->features;
	if(pStrucAnnot!=NULL){
		ErrPostEx(SEV_WARNING, 0,0,"[FillDomNameNode]:No mmdb-id found");
		pStrucid=pStrucAnnot->id;
		while(pStrucid!=NULL){
			if(pStrucid->choice==BiostrucId_mmdb_id){
				pDomname->mmdb_id= pStrucid->data.intvalue;
			}
			pStrucid=pStrucid->next;
		}
	}
	/*Get cdd*/
	pDomname->asn1=pCdd;
	if(!AppendRecordDOMNAME(pDomname))
	{
		ErrPostEx(SEV_FATAL,0,0,"GenerateDomName:cannot commit to table");
		return FALSE;
	}
	else{
		ErrPostEx(SEV_INFO, 0,0,"Appended %s", IsNullStr(pDomname->accession));
	}
	return TRUE;
}

/*Generate DomName record*/
void GenerateDomName(char* filename)
{
	CddPtr pcdd=NULL;
	StDomNamedbPtr pdmn=NULL;
	AsnIoPtr aip=NULL;

	/* SK: NULL ptr check */
	if(filename==NULL)
	{
		ErrPostEx(SEV_FATAL,0,0,"GenerateDomName: NULL filename");
		return;
	}

	/*load Cdd <domain>.acd */
	aip=AsnIoOpen(filename,"rb");
	/* SK: NULL ptr check */
	if ( aip == NULL )
	{
		ErrPostEx(SEV_FATAL,0,0,"GenerateDomName: NULL aip");
		return;
	}

	pcdd=CddAsnRead(aip, NULL);
	AsnIoClose(aip);
	if(pcdd==NULL)
	{
		ErrPostEx(SEV_FATAL,0,0,"GenerateDomName:cannot load %s", filename);
		/* SK: return skipped */
		return;
	}

	/*create new DomName struct*/
	pdmn=StDomNamedbNew();
	/* SK: NULL ptr check */
	if ( pdmn == NULL )
	{
	    ErrPostEx(SEV_FATAL,0,0,"GenerateDomName: NULL pdmn");
	    return;
	}

	ResetStDOMNAME(&pdmn);

	/* fill it up and commit */
	if(!FillDomNameNode(pdmn, pcdd)){
		ErrPostEx(SEV_FATAL, 0,0,"GenerateDomName:Generating %s record failed",filename);
		/* SK: return skipped */
		return;
	}

	/*Note StDomNamedbFree calls
	* CddFree, no need to call it separately. */
	pcdd=CddFree(pcdd);
	pdmn->asn1=NULL;
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
	
	/* SK: NULL ptr check */
	if ( dir == NULL )
	{
		ErrPostEx(SEV_FATAL, 0,0,"DirWalk: NULL dir");
		return;
	}

	/* SK: NULL ptr check */
	if ( fcn == NULL )
	{
		ErrPostEx(SEV_FATAL, 0,0,"DirWalk: NULL fcn");
		return;
	}

	/* SK: should be dir, not g_cdd_path */
	// if((dfd=opendir(g_cdd_path))==NULL){
	if((dfd=opendir(dir))==NULL){
		ErrPostEx(SEV_FATAL, 0,0,"DirWalk: cannot open %s", dir);
		return;
	}
	while((dp=readdir(dfd))!=NULL){
		/*skip all non-acd files*/
		temp=strchr(dp->d_name, '.');
		if( strcmp(dp->d_name,".")==0|| strcmp(dp->d_name, "..")==0 ||
			temp==NULL || strcmp(temp,".acd")!=0 )
			continue;
		sprintf(name, "%s/%s", dir, IsNullStr(dp->d_name));
		(*fcn)(name);
		temp=NULL;
	}
	closedir(dfd);
	return;
}

Int2 Main()
{
	void(*fcn)(char*)=NULL;

	/*Error loging*/
	ErrSetOptFlags(EO_LOG_SEVERITY | EO_LOG_CODES | EO_LOG_FILELINE | EO_LOG_USRFILE);
	ErrSetLogfile("dombname.log", ELOG_BANNER| ELOG_APPEND);
	ErrSetLogLevel(SEV_INFO);
	ErrSetMessageLevel(SEV_FATAL);

	/*Initialize code base*/
	if(!InitCodeBase()){
		ErrPostEx(SEV_FATAL, 0,0,"Main: cannot initialize Codebase");
		return -1;
	}

	/* Open Domaname table */
	if(!OpenDOMNAME("DOMNAME", TRUE)){
		ErrPostEx(SEV_FATAL, 0,0,"Main: cannot create DOMNAME");
		return -1;
	}

	/* Get CDD path from config file */
	GetAppParam("intrez", "datab","pathcdd", NULL,
		g_cdd_path, (size_t)PATH_MAX * (sizeof(Char)));

	/*Walk through cdd dir */
	fcn=GenerateDomName;
	DirWalk(g_cdd_path, fcn);
	PrintDOMNAME();
	/*free and close*/
	CloseDOMNAME();
	CloseCodeBase();
}
/*
$Log: domname.c,v $
Revision 1.2  2004/01/06 22:32:13  skuznets
Code audit has been done sucessfully.
Possible NULL pointer issues has been eliminated.

Revision 1.1.1.3  2002/03/19 09:27:26  root
Automated import from Sourceforge.net

Revision 1.3  2002/03/18 22:02:42  betel
Fixed Print and compile bugs

Revision 1.2  2002/03/12 21:11:20  betel
Moved rpsdb files to rps dir

Revision 1.1  2002/03/11 23:02:38  betel
DomName table parser

*/
