/*$Id: prebind_db.c,v 1.6 2004/05/31 18:59:46 kzheng Exp $*/
/*
-- ***************************************************************************
--
--  Pre Biomolecular Interaction Network Database (PreBIND)
--
--  File: prebind_db.c
--  Description: prebind codebase database layer
--				 
--
--  Authors: Ian Donaldson (ian.donaldson@utoronto.ca)
--			 and Christopher W.V. Hogue (hogue@mshri.on.ca)
--
--  Date: June 2002
--
--  Hogue Lab - University of Toronto Biochemistry Department
--  Samuel Lunenfeld Research Institute, Mount Sinai Hospital
--
--  REVISION/CONTRIBUTION NOTES:
--
--  June 1, 2000  Original MSH Revision.
--              Hogue Lab - University of Toronto Biochemistry Department 
--              and the Samuel Lunenfeld Research Institute, Mount Sinai Hospital
--              http://bioinfo.mshri.on.ca  hogue@mshri.on.ca
--              ftp://ftp.mshri.on.ca/
--
--
--  Copyright Notice:
--
--  Copyright ©2000 Mount Sinai Hospital (MSH)
--
--  This program is free software; you can redistribute it and/or
--  modify it under the terms of the GNU General Public License as
--  published by the Free Software Foundation; either version 2 of
--  the License, or any later version.
--
--  This program is distributed in the hope that it will be useful,
--  but WITHOUT ANY WARRANTY; without even the implied warranty of
--  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
--
--  See the GNU General Public License for more details.
--
--  You should have received a copy of the GNU General Public License
--  along with this program; if not, write to the
--         Free Software Foundation, Inc.,
--         59 Temple Place, Suite 330, Boston, MA
--         02111-1307  USA
--  or visit http://www.gnu.org/copyleft/gpl.html
--
--  SPECIAL EXCEPTIONS
--
--  As a special exception, Mount Sinai Hospital gives permission to
--  link this program with the following non-GPL programs or libraries,
--  and distribute the resulting executable, without including the source
--  code for these in the source distribution:
--
--  a) CodeBase 6.5 or greater from Sequiter Software Inc.
--
--  b) The C or C++ interface to Oracle 8.x or greater, from
--     Oracle Corporation or IBM DB2 UDB.
--
-- ***************************************************************************
*/





#ifndef _PREBIND_DB_
#include "prebind_db.h"
#endif




	/***************CREATING NEW INDEX FILES FOR EXISTING DATAFILES******************************/
	/*temporary lines that can be inserted into OpenDATABASE functions to create new index files*/
	/*(1)these lines should be inserted after the line "StringCat(pathdatab, name);"*/
	/*(2)the old database.cdx index file should be removed*/
	/*(3)new tags may be added by specifying them in the TAG4INFO array (probably in the header file)*/
	/*(4)the OpenDatabase function should include statements assigning TAG4 pointers to the newly declared tags */
	/*(5)see more info under C User Guide p. 58*/
	/*the new index file will include indexing specified by the TAG4INFO pointer eg. t4iSRCH*/
	/*the codeBase.autoOpen = 0 prevents the production index from automatically having to be opened*/
	/*codeBase.accessMode = OPEN4DENY_RW;
	codeBase.autoOpen = 0; 
	pd4_SRCH = d4open(&codeBase, pathdatab);
	i4create(pd4_SRCH, NULL, t4iSRCH);
	return TRUE;*/



/********************************************************/
/*                                                       */
/*   OpenPreBIND: used by prebind_cgi                    */
/*       Inits CodeBase and Opens all PreBIND datatables */
/*                                                       */
/*   PARAMETERS:                                         */
/*           none                                        */
/*   RETURNS:                                            */
/*       TRUE-   success                                 */
/*       FALSE-  some problem                            */
/********************************************************/

Boolean OpenPreBIND(void)
{
	/*initialize codeBase*/
	if (!InitCodeBaseForPreBIND())
	{
		ErrPostEx(SEV_ERROR, codeBase.errorCode, 0, "OpenPreBIND: InitCodeBaseForPreBIND failed.");
		error4exitTest(&codeBase); /*quits program */
		return FALSE;
	}

	/*open database files*/
 	if (!OpenACC("ACC", (Boolean) "F"))
	{
		ErrPostEx(SEV_ERROR, codeBase.errorCode, 0, "OpenPreBIND: OpenACC failed.");
		return FALSE;
	}

	if (!OpenSRCH("SRCH", (Boolean) "F"))
	{
		ErrPostEx(SEV_ERROR, codeBase.errorCode, 0, "OpenPreBIND: OpenSRCH failed.");
		return FALSE;
	}
	
	if (!OpenRESULT("RESULT", (Boolean) "F"))
	{
		ErrPostEx(SEV_ERROR, codeBase.errorCode, 0, "OpenPreBIND: OpenRESULT failed.");
		return FALSE;
	}

	if (!OpenISVM("ISVM", (Boolean) "F"))
	{
		ErrPostEx(SEV_ERROR, codeBase.errorCode, 0, "OpenPreBIND: OpenISVM failed.");
		return FALSE;
	}

	if (!OpenCORE("CORE", (Boolean) "F"))
	{
		ErrPostEx(SEV_ERROR, codeBase.errorCode, 0, "OpenPreBIND: OpenCORE failed.");
		return FALSE;
	}

	if (!OpenREGI("REGI", (Boolean) "F"))
	{
		ErrPostEx(SEV_ERROR, codeBase.errorCode, 0, "OpenPreBIND: OpenREGI failed.");
		return FALSE;
	}

	if (!OpenPOINT("POINT", (Boolean) "F"))
	{
		ErrPostEx(SEV_ERROR, codeBase.errorCode, 0, "OpenPreBIND: OpenPOINT failed.");
		return FALSE;
	}

	if (!OpenLOCUS("LOCUS", (Boolean) "F"))
	{
		ErrPostEx(SEV_ERROR, codeBase.errorCode, 0, "OpenPreBIND: OpenLOCUS failed.");
		return FALSE;
	}

	if (!OpenSUBMIT("SUBMIT", (Boolean) "T"))
	{
		ErrPostEx(SEV_ERROR, codeBase.errorCode, 0, "OpenPreBIND: OpenSUBMIT failed.");
		return FALSE;
	}

	return TRUE;

}




/********************************************************/
/*														*/
/*	InitCodeBaseForPreBIND: Initializes CodeBase		*/
/*														*/
/*	PARAMETERS:											*/
/*			none										*/
/*	RETURNS:											*/
/*		TRUE-	success									*/
/*		FALSE-	no success								*/
/********************************************************/
Boolean InitCodeBaseForPreBIND(void)
{
	code4init(&codeBase);
    code4largeOn(&codeBase);                /* CodeBase support for large files*/
	codeBase.errOpen = 0;					/*prevents error message when datafile fails to open*/
	codeBase.safety = 1;					/*prevents d4create from overwriting existing files*/
	codeBase.errDefaultUnique = r4unique;	/*prevents adding duplicate records to unique keys*/	
	if (codeBase.errorCode)
		return FALSE; 
	else
		return TRUE;
}

/********************************************************/
/*														*/
/*	OpenREGI: Opens the REsults by GI database			*/
/*														*/
/*	PARAMETERS:											*/
/*		Create-	the database will be created if it		*/
/*				does not already exist	(TRUE/FALSE)	*/
/*	RETURNS:											*/
/*		TRUE-	success									*/
/*		FALSE-	no success								*/
/********************************************************/
Boolean OpenREGI(CharPtr name, Boolean Create)
{
	Char pathdatab[PATH_MAX];

	/*find path name of database (see NCBI toolkit corelib 
	and file "prebind.ini" in windows directory)*/
	pathdatab[0] = '\0';
	GetAppParam("prebind", "databases", "path", NULL, pathdatab, (size_t)PATH_MAX * (sizeof(Char)));
	StringCat(pathdatab, name);
	/*printf("%s\n", pathdatab);*/

	if (Create == FALSE)
	{
		/*open the datafile or return error*/
		pd4_REGI = d4open(&codeBase, pathdatab);
		if (pd4_REGI == NULL)
			return FALSE;
	}
	else
	{
		/*open the datafile or create the datafile or return error*/
		pd4_REGI = d4open(&codeBase, pathdatab);
		if (pd4_REGI == NULL)
			pd4_REGI = d4create(&codeBase, pathdatab, f4iREGI, t4iREGI);
				if (codeBase.errorCode)
					return FALSE;
	}

	/*assign the field pointers*/
	pf4_REGI_PMID   = d4field(pd4_REGI, "PMID" );
	pf4_REGI_MUID   = d4field(pd4_REGI, "MUID" );
	pf4_REGI_GIP	= d4field(pd4_REGI, "GIP" );

	/*assign the tag pointers*/
	pt4_REGI_PMID  = d4tag(pd4_REGI, "PMID_TAG");
	pt4_REGI_MUID  = d4tag(pd4_REGI, "MUID_TAG");
	pt4_REGI_GIP   = d4tag(pd4_REGI, "GIP_TAG");
	pt4_REGI_UNIQUE= d4tag(pd4_REGI, "UNIQUE_TAG");

	return TRUE;

}


/********************************************************/
/*														*/
/*	OpenSUBMIT: Opens the SUBMITted  potential			*/
/*				interaction database					*/
/*														*/
/*	PARAMETERS:											*/
/*		Create-	the database will be created if it		*/
/*				does not already exist	(TRUE/FALSE)	*/
/*	RETURNS:											*/
/*		TRUE-	success									*/
/*		FALSE-	no success								*/
/********************************************************/
Boolean OpenSUBMIT(CharPtr name, Boolean Create)
{
	Char pathdatab[PATH_MAX];

	/*find path name of database (see NCBI toolkit corelib 
	and file "prebind.ini" in windows directory)*/
	pathdatab[0] = '\0';
	GetAppParam("prebind", "databases", "path", NULL, pathdatab, (size_t)PATH_MAX * (sizeof(Char)));
	StringCat(pathdatab, name);
	/*printf("%s\n", pathdatab);*/

	if (Create == FALSE)
	{
		/*open the datafile or return error*/
		pd4_SUBMIT = d4open(&codeBase, pathdatab);
		if (pd4_SUBMIT == NULL)
			return FALSE;
	}
	else
	{
		/*open the datafile or create the datafile or return error*/
		pd4_SUBMIT = d4open(&codeBase, pathdatab);
		if (pd4_SUBMIT == NULL)
			pd4_SUBMIT = d4create(&codeBase, pathdatab, f4iSUBMIT, t4iSUBMIT);
				if (codeBase.errorCode)
					return FALSE;
	}

	/*assign the field pointers*/
	pf4_SUBMIT_PMID	  = d4field(pd4_SUBMIT, "PMID");	/*PubMed IDentifier */
	pf4_SUBMIT_MUID	  = d4field(pd4_SUBMIT, "MUID");	/*Medine IDentifier */
	pf4_SUBMIT_GIA	  = d4field(pd4_SUBMIT, "GIA");		/*GIs of protein A in interaction*/
	pf4_SUBMIT_GIB	  = d4field(pd4_SUBMIT, "GIB");		/*GIs of protein B in interaction*/
	pf4_SUBMIT_SGIA	  = d4field(pd4_SUBMIT, "SGIA");	/*Submitted GI of molecule A*/
	pf4_SUBMIT_NAMA	  = d4field(pd4_SUBMIT, "NAMA");	/*NAMes of molecule A in interaction*/
	pf4_SUBMIT_ADESC  = d4field(pd4_SUBMIT, "ADESC");	/*DESCription of molecule A in interaction*/
	pf4_SUBMIT_ATYPE  = d4field(pd4_SUBMIT, "ATYPE");	/*A: TYPE of molecule in interaction*/
	pf4_SUBMIT_AOK	  = d4field(pd4_SUBMIT, "AOK");		/*sgiA describes molecule A OK?*/
	pf4_SUBMIT_AFROM  = d4field(pd4_SUBMIT, "AFROM");	/*AFROM binding location*/
	pf4_SUBMIT_ATO	  = d4field(pd4_SUBMIT, "ATO");		/*ATO binding location*/
	pf4_SUBMIT_SGIB	  = d4field(pd4_SUBMIT, "SGIB");	/*Submitted GI of molecule B*/
	pf4_SUBMIT_NAMB	  = d4field(pd4_SUBMIT, "NAMB");	/*NAMes of molecule B in interaction*/
	pf4_SUBMIT_BDESC  = d4field(pd4_SUBMIT, "BDESC");	/*DESCription of molecule B in interaction*/
	pf4_SUBMIT_BTYPE  = d4field(pd4_SUBMIT, "BTYPE");	/*B: TYPE of molecule in interaction*/
	pf4_SUBMIT_BOK	  = d4field(pd4_SUBMIT, "BOK");		/*sgiB describes molecule B OK?*/
	pf4_SUBMIT_BFROM  = d4field(pd4_SUBMIT, "BFROM");	/*BFROM binding location*/
	pf4_SUBMIT_BTO	  = d4field(pd4_SUBMIT, "BTO");		/*BTO binding location*/
	pf4_SUBMIT_METHOD = d4field(pd4_SUBMIT, "METHOD");	/*METHOD used to show interaction*/
	pf4_SUBMIT_OPIN	  = d4field(pd4_SUBMIT, "OPIN");	/*OPINion (is this an interaction or not*/
	pf4_SUBMIT_FNAME  = d4field(pd4_SUBMIT, "FNAME");	/*First NAME*/
	pf4_SUBMIT_LNAME  = d4field(pd4_SUBMIT, "LNAME");	/*Last NAME*/
	pf4_SUBMIT_BUID	  = d4field(pd4_SUBMIT, "BUID");	/*Bind User ID*/
	pf4_SUBMIT_DATE	  = d4field(pd4_SUBMIT, "DATE");	/*DATE submitted*/
	pf4_SUBMIT_STATUS = d4field(pd4_SUBMIT, "STATUS");	/*STATUS*/


	/*assign the tag pointers*/
	pt4_SUBMIT_PMID	= d4tag(pd4_SUBMIT, "PMID_TAG");	/*PubMed IDentifier */
	pt4_SUBMIT_MUID	= d4tag(pd4_SUBMIT, "MUID_TAG");	/*Medine IDentifier */
	pt4_SUBMIT_GIA	= d4tag(pd4_SUBMIT, "GIA_TAG");		/*GIs of protein A in interaction*/
	pt4_SUBMIT_GIB	= d4tag(pd4_SUBMIT, "GIB_TAG");		/*GIs of protein B in interaction*/
	pt4_SUBMIT_SGIA	= d4tag(pd4_SUBMIT, "SGIA_TAG");	/*Submitted GI of molecule A*/
	pt4_SUBMIT_NAMA	= d4tag(pd4_SUBMIT, "NAMA_TAG");	/*NAMes of molecule A in interaction*/
	pt4_SUBMIT_SGIB	= d4tag(pd4_SUBMIT, "SGIB_TAG");	/*Submitted GI of molecule B*/
	pt4_SUBMIT_NAMB	= d4tag(pd4_SUBMIT, "NAMB_TAG");	/*NAMes of molecule B in interaction*/
	pt4_SUBMIT_FNAME= d4tag(pd4_SUBMIT, "FNAME_TAG");	/*First NAME*/
	pt4_SUBMIT_LNAME= d4tag(pd4_SUBMIT, "LNAME_TAG");	/*Last NAME*/
	pt4_SUBMIT_BUID	= d4tag(pd4_SUBMIT, "BUID_TAG");	/*Bind User ID*/
	pt4_SUBMIT_DATE	= d4tag(pd4_SUBMIT, "DATE_TAG");	/*DATE submitted*/
	pt4_SUBMIT_STATUS = d4tag(pd4_SUBMIT, "STATUS_TAG");/*STATUS*/


	return TRUE;

}





/********************************************************/
/*														*/
/*	OpenPOINT: Opens the POtential INTeraction database	*/
/*														*/
/*	PARAMETERS:											*/
/*		Create-	the database will be created if it		*/
/*				does not already exist	(TRUE/FALSE)	*/
/*	RETURNS:											*/
/*		TRUE-	success									*/
/*		FALSE-	no success								*/
/********************************************************/
Boolean OpenPOINT(CharPtr name, Boolean Create)
{
	Char pathdatab[PATH_MAX];

	/*find path name of database (see NCBI toolkit corelib 
	and file "prebind.ini" in windows directory)*/
	pathdatab[0] = '\0';
	GetAppParam("prebind", "databases", "path", NULL, pathdatab, (size_t)PATH_MAX * (sizeof(Char)));
	StringCat(pathdatab, name);
	/*printf("%s\n", pathdatab);*/

	if (Create == FALSE)
	{
		/*open the datafile or return error*/
		pd4_POINT = d4open(&codeBase, pathdatab);
		if (pd4_POINT == NULL)
			return FALSE;
	}
	else
	{
		/*open the datafile or create the datafile or return error*/
		pd4_POINT = d4open(&codeBase, pathdatab);
		if (pd4_POINT == NULL)
			pd4_POINT = d4create(&codeBase, pathdatab, f4iPOINT, t4iPOINT);
				if (codeBase.errorCode)
					return FALSE;
	}

	/*assign the field pointers*/
	pf4_POINT_PMID		= d4field(pd4_POINT, "PMID" );
	pf4_POINT_MUID		= d4field(pd4_POINT, "MUID" );
	pf4_POINT_GIA		= d4field(pd4_POINT, "GIA"  );
	pf4_POINT_NAMA		= d4field(pd4_POINT, "NAMA" );
	pf4_POINT_ATYPE		= d4field(pd4_POINT, "ATYPE");
	pf4_POINT_GIB		= d4field(pd4_POINT, "GIB"  );
	pf4_POINT_NAMB		= d4field(pd4_POINT, "NAMB" );
	pf4_POINT_BTYPE		= d4field(pd4_POINT, "BTYPE");
	pf4_POINT_OPNO		= d4field(pd4_POINT, "OPNO" );
	pf4_POINT_OPYES		= d4field(pd4_POINT, "OPYES");
	pf4_POINT_PISCORE	= d4field(pd4_POINT, "PISCORE" );
	pf4_POINT_PISCORE2	= d4field(pd4_POINT, "PISCORE2");
	pf4_POINT_PITRUTH	= d4field(pd4_POINT, "PITRUTH" );


	/*assign the tag pointers*/
	pt4_POINT_PMID		= d4tag(pd4_POINT, "PMID_TAG");
	pt4_POINT_MUID		= d4tag(pd4_POINT, "MUID_TAG");
	pt4_POINT_GIA		= d4tag(pd4_POINT, "GIA_TAG" );
	pt4_POINT_GIB		= d4tag(pd4_POINT, "GIB_TAG" );
	pt4_POINT_PISCORE	= d4tag(pd4_POINT, "PISCORE_TAG" );
	pt4_POINT_PITRUTH	= d4tag(pd4_POINT, "PITRUTH_TAG" );

	return TRUE;

}



/********************************************************/
/*														*/
/*	OpenCORE: Opens the REsults COunt database			*/
/*														*/
/*	PARAMETERS:											*/
/*		Create-	the database will be created if it		*/
/*				does not already exist	(TRUE/FALSE)	*/
/*	RETURNS:											*/
/*		TRUE-	success									*/
/*		FALSE-	no success								*/
/********************************************************/
Boolean OpenCORE(CharPtr name, Boolean Create)
{
	Char pathdatab[PATH_MAX];

	/*find path name of database (see NCBI toolkit corelib 
	and file "prebind.ini" in windows directory)*/
	pathdatab[0] = '\0';
	GetAppParam("prebind", "databases", "path", NULL, pathdatab, (size_t)PATH_MAX * (sizeof(Char)));
	StringCat(pathdatab, name);
	/*printf("%s\n", pathdatab);*/

	if (Create == FALSE)
	{
		/*open the datafile or return error*/
		pd4_CORE = d4open(&codeBase, pathdatab);
		if (pd4_CORE == NULL)
			return FALSE;
	}
	else
	{
		/*open the datafile or create the datafile or return error*/
		pd4_CORE = d4open(&codeBase, pathdatab);
		if (pd4_CORE == NULL)
			pd4_CORE = d4create(&codeBase, pathdatab, f4iCORE, t4iCORE);
				if (codeBase.errorCode)
					return FALSE;
	}

	/*assign the field pointers*/
	pf4_CORE_PMID   = d4field(pd4_CORE, "PMID" );
	pf4_CORE_MUID   = d4field(pd4_CORE, "MUID" );
	pf4_CORE_NUMN	= d4field(pd4_CORE, "NUMN" );
	pf4_CORE_NUMG	= d4field(pd4_CORE, "NUMG" );

	/*assign the tag pointers*/
	pt4_CORE_PMID   = d4tag(pd4_CORE, "PMID_TAG");
	pt4_CORE_MUID   = d4tag(pd4_CORE, "MUID_TAG");
	pt4_CORE_NUMN   = d4tag(pd4_CORE, "NUMN_TAG");
	pt4_CORE_NUMG   = d4tag(pd4_CORE, "NUMG_TAG");

	return TRUE;

}



/********************************************************/
/*														*/
/*	OpenISVM: Opens the ISVM database					*/
/*														*/
/*	PARAMETERS:											*/
/*		Create-	the database will be created if it		*/
/*				does not already exist	(TRUE/FALSE)	*/
/*	RETURNS:											*/
/*		TRUE-	success									*/
/*		FALSE-	no success								*/
/********************************************************/
Boolean OpenISVM(CharPtr name, Boolean Create)
{
	Char pathdatab[PATH_MAX];

	/*find path name of database (see NCBI toolkit corelib 
	and file "prebind.ini" in windows directory)*/
	pathdatab[0] = '\0';
	GetAppParam("prebind", "databases", "path", NULL, pathdatab, (size_t)PATH_MAX * (sizeof(Char)));
	StringCat(pathdatab, name);
	/*printf("%s\n", pathdatab);*/

	if (Create == FALSE)
	{
		/*open the datafile or return error*/
		pd4_ISVM = d4open(&codeBase, pathdatab);
		if (pd4_ISVM == NULL)
			return FALSE;
	}
	else
	{
		/*open the datafile or create the datafile or return error*/
		pd4_ISVM = d4open(&codeBase, pathdatab);
		if (pd4_ISVM == NULL)
			pd4_ISVM = d4create(&codeBase, pathdatab, f4iISVM, t4iISVM);
				if (codeBase.errorCode)
					return FALSE;
	}

	/*assign the field pointers*/
	pf4_ISVM_PMID    = d4field(pd4_ISVM, "PMID"   );
	pf4_ISVM_MUID    = d4field(pd4_ISVM, "MUID"   );
	pf4_ISVM_ISCORE  = d4field(pd4_ISVM, "ISCORE" );
	pf4_ISVM_ISCORE2 = d4field(pd4_ISVM, "ISCORE2" );
	pf4_ISVM_OPNO    = d4field(pd4_ISVM, "OPNO" );
	pf4_ISVM_OPYES   = d4field(pd4_ISVM, "OPYES" );
	pf4_ISVM_ITRUTH  = d4field(pd4_ISVM, "ITRUTH" );

	/*assign the tag pointers*/
	pt4_ISVM_PMID   = d4tag(pd4_ISVM, "PMID_TAG");
	pt4_ISVM_MUID   = d4tag(pd4_ISVM, "MUID_TAG");
	
	return TRUE;

}


/********************************************************/
/*														*/
/*	OpenRESULT: Opens the RESULT database				*/
/*														*/
/*	PARAMETERS:											*/
/*		Create-	the database will be created if it		*/
/*				does not already exist	(TRUE/FALSE)	*/
/*	RETURNS:											*/
/*		TRUE-	success									*/
/*		FALSE-	no success								*/
/********************************************************/
Boolean OpenRESULT(CharPtr name, Boolean Create)
{
	Char pathdatab[PATH_MAX];

	/*find path name of database (see NCBI toolkit corelib 
	and file "prebind.ini" in windows directory)*/
	pathdatab[0] = '\0';
	GetAppParam("prebind", "databases", "path", NULL, pathdatab, (size_t)PATH_MAX * (sizeof(Char)));
	StringCat(pathdatab, name);
	/*printf("%s\n", pathdatab);*/

	if (Create == FALSE)
	{
		/*open the datafile or return error*/
		pd4_RESULT = d4open(&codeBase, pathdatab);
		if (pd4_RESULT == NULL)
			return FALSE;
	}
	else
	{
		/*open the datafile or create the datafile or return error*/
		pd4_RESULT = d4open(&codeBase, pathdatab);
		if (pd4_RESULT == NULL)
			pd4_RESULT = d4create(&codeBase, pathdatab, f4iRESULT, t4iRESULT);
				if (codeBase.errorCode)
					return FALSE;
	}

	/*assign the field pointers*/
	pf4_RESULT_GIP  = d4field(pd4_RESULT, "GI_PROT" );
	pf4_RESULT_NAME = d4field(pd4_RESULT, "NAME"    );
	pf4_RESULT_PMID	= d4field(pd4_RESULT, "PMID"    );

	/*assign the tag pointers*/
	pt4_RESULT_GIP    = d4tag(pd4_RESULT,"GIP_TAG");
	pt4_RESULT_NAME   = d4tag(pd4_RESULT,"NAME_TAG");
	pt4_RESULT_PMID   = d4tag(pd4_RESULT,"PMID_TAG");
	pt4_RESULT_UNIQUE = d4tag(pd4_RESULT,"UNIQUE_TAG");
	
	return TRUE;

}




/********************************************************/
/*														*/
/*	OpenSRCH: Opens the SeaRCH database					*/
/*														*/
/*	PARAMETERS:											*/
/*		Create-	the database will be created if it		*/
/*				does not already exist	(TRUE/FALSE)	*/
/*	RETURNS:											*/
/*		TRUE-	success									*/
/*		FALSE-	no success								*/
/********************************************************/
Boolean OpenSRCH(CharPtr name, Boolean Create)
{
	Char pathdatab[PATH_MAX];

	/*find path name of database (see NCBI toolkit corelib 
	and file "prebind.ini" in windows directory)*/
	pathdatab[0] = '\0';
	GetAppParam("prebind", "databases", "path", NULL, pathdatab, (size_t)PATH_MAX * (sizeof(Char)));
	StringCat(pathdatab, name);
	/*printf("%s\n", pathdatab);*/
	

	if (Create == FALSE)
	{
		/*open the datafile or return error*/
		pd4_SRCH = d4open(&codeBase, pathdatab);
		if (pd4_SRCH == NULL)
			return FALSE;
	}
	else
	{
		/*open the datafile or create the datafile or return error*/
		pd4_SRCH = d4open(&codeBase, pathdatab);
		if (pd4_SRCH == NULL)
			pd4_SRCH = d4create(&codeBase, pathdatab, f4iSRCH, t4iSRCH);
				if (codeBase.errorCode)
					return FALSE;
	}


	/*assign the field pointers*/
	pf4_SRCH_GIP      = d4field(pd4_SRCH, "GI_PROT" );
	pf4_SRCH_NAME     = d4field(pd4_SRCH, "NAME"    );
	pf4_SRCH_SOURCE	  = d4field(pd4_SRCH, "SOURCE"  );
	pf4_SRCH_TAXID	  = d4field(pd4_SRCH, "TAXID"   );
	pf4_SRCH_SEARCHED = d4field(pd4_SRCH, "SEARCHED"); /*last date that search was performed*/
	pf4_SRCH_STATUS   = d4field(pd4_SRCH, "STATUS");   /*0- not approved, 1- approved, 2- search performed */
	pf4_SRCH_NAMERED  = d4field(pd4_SRCH, "NAMERED");  /*name redundancy- no. times this name appears for organism*/
	pf4_SRCH_RESULTS  = d4field(pd4_SRCH, "RESULTS");

	/*assign the tag pointers*/
	pt4_SRCH_GIP    = d4tag(pd4_SRCH,"GIP_TAG");
	pt4_SRCH_NAME   = d4tag(pd4_SRCH,"NAME_TAG");
	pt4_SRCH_CIN	= d4tag(pd4_SRCH,"CIN_TAG");
	pt4_SRCH_UNIQUE = d4tag(pd4_SRCH,"UNIQUE_TAG");
	
	return TRUE;

}




/********************************************************/
/*														*/
/*	OpenYENA: Opens the YEast NAme database				*/
/*														*/
/*	PARAMETERS:											*/
/*		Create-	the database will be created if it		*/
/*				does not already exist	(TRUE/FALSE)	*/
/*	RETURNS:											*/
/*		TRUE-	success									*/
/*		FALSE-	no success								*/
/********************************************************/
Boolean OpenYENA(CharPtr name, Boolean Create)
{
	Char pathdatab[PATH_MAX];

	/*find path name of database (see NCBI toolkit corelib 
	and file "prebind.ini" in windows directory)*/
	pathdatab[0] = '\0';
	GetAppParam("prebind", "databases", "path", NULL, pathdatab, (size_t)PATH_MAX * (sizeof(Char)));
	StringCat(pathdatab, name);
	/*printf("%s\n", pathdatab);*/

	if (Create == FALSE)
	{
		/*open the datafile or return error*/
		pd4_YENA = d4open(&codeBase, pathdatab);
		if (pd4_YENA == NULL)
			return FALSE;
	}
	else
	{
		/*open the datafile or create the datafile or return error*/
		pd4_YENA = d4open(&codeBase, pathdatab);
		if (pd4_YENA == NULL)
			pd4_YENA = d4create(&codeBase, pathdatab, f4iYENA, t4iYENA);
				if (codeBase.errorCode)
					return FALSE;
	}

	/*assign the field pointers*/
	pf4_YENA_GIP  = d4field(pd4_YENA, "GI_PROT");
	pf4_YENA_NAME = d4field(pd4_YENA, "NAME"   );

	/*assign the tag pointers*/
	pt4_YENA_GIP =    d4tag(pd4_YENA,"GIP_TAG");
	pt4_YENA_UNIQUE = d4tag(pd4_YENA,"UNIQUE_TAG");
	
	return TRUE;

}


/********************************************************/
/*														*/
/*	OpenACC: Opens the Accession database				*/
/*														*/
/*	PARAMETERS:											*/
/*		Create-	the database will be created if it		*/
/*				does not already exist	(TRUE/FALSE)	*/
/*	RETURNS:											*/
/*		TRUE-	success									*/
/*		FALSE-	no success								*/
/********************************************************/
Boolean OpenACC(CharPtr name, Boolean Create)
{
	Char pathdatab[PATH_MAX];

	/*find path name of database (see NCBI toolkit corelib 
	and file "prebind.ini" in windows directory)*/
	pathdatab[0] = '\0';
	GetAppParam("prebind", "databases", "path", NULL, pathdatab, (size_t)PATH_MAX * (sizeof(Char)));
	StringCat(pathdatab, name);
	/*printf("%s\n", pathdatab);*/

	if (Create == FALSE)
	{
		/*open the datafile or return error*/
		pd4_ACC = d4open(&codeBase, pathdatab);
		if (pd4_ACC == NULL)
			return FALSE;
	}
	else
	{
		/*open the datafile or create the datafile or return error*/
		pd4_ACC = d4open(&codeBase, pathdatab);
		if (pd4_ACC == NULL)
			pd4_ACC = d4create(&codeBase, pathdatab, f4iACC, t4iACC);
				if (codeBase.errorCode)
					return FALSE;
	}

	/*assign the field pointers*/
	pf4_ACC_ACC      = d4field(pd4_ACC, "ACCESSION" );
	pf4_ACC_GI       = d4field(pd4_ACC, "GI"        );
	pf4_ACC_DB       = d4field(pd4_ACC, "DATABASE"  ); 
	pf4_ACC_LOCUS	 = d4field(pd4_ACC, "LOCUS"   );
	pf4_ACC_DEF      = d4field(pd4_ACC, "DEFINITION");
	pf4_ACC_PPROD    = d4field(pd4_ACC, "PPROD"     );
	pf4_ACC_TAXID    = d4field(pd4_ACC, "TAXID"     );
	pf4_ACC_UPDATED  = d4field(pd4_ACC, "UPDATED"   ); 
	
	/*assign the tag pointers*/
	pt4_ACC_GI=   d4tag(pd4_ACC, "GI_TAG");	
	pt4_ACC_ACC=  d4tag(pd4_ACC, "ACC_TAG");/*accessions are unique to this database*/
	pt4_ACC_RS=   d4tag(pd4_ACC, "RSQ_TAG");


	return TRUE;

}



/********************************************************/
/*														*/
/*	OpenDBDB: Opens the DataBase database				*/
/*														*/
/*	PARAMETERS:											*/
/*		Create-	the database will be created if it		*/
/*				does not already exist	(TRUE/FALSE)	*/
/*	RETURNS:											*/
/*		TRUE-	success									*/
/*		FALSE-	no success								*/
/********************************************************/
Boolean OpenDBDB(CharPtr name, Boolean Create)
{
	Char pathdatab[PATH_MAX];

	/*find path name of database (see NCBI toolkit corelib 
	and file "prebind.ini" in windows directory)*/
	pathdatab[0] = '\0';
	GetAppParam("prebind", "databases", "path", NULL, pathdatab, (size_t)PATH_MAX * (sizeof(Char)));
	StringCat(pathdatab, name);

	if (Create == FALSE)
	{
		/*open the datafile or return error*/
		pd4_DB = d4open(&codeBase, pathdatab);
		if (pd4_DB == NULL)
			return FALSE;
	}
	else
	{
		/*open the datafile or create the datafile or return error*/
		pd4_DB = d4open(&codeBase, pathdatab);
		if (pd4_DB == NULL)
			pd4_DB = d4create(&codeBase, pathdatab, f4iDB, t4iDB);
				if (codeBase.errorCode)
					return FALSE;
	}

	/*assign the field pointers*/
	pf4_DB_GIN   = d4field(pd4_DB, "GI_NUC" );
	pf4_DB_FROM  = d4field(pd4_DB, "FROM"   );
	pf4_DB_TO    = d4field(pd4_DB, "TO"     );
	pf4_DB_GIP   = d4field(pd4_DB, "GI_PROT");
	pf4_DB_DBNAME= d4field(pd4_DB, "DBNAME" ); 
	pf4_DB_IDINT = d4field(pd4_DB, "IDINT"  );
	pf4_DB_IDSTR = d4field(pd4_DB, "IDSTR"  );

	/*assign the tag pointers*/
	pt4_DB_GIN=   d4tag(pd4_DB, "GIN_TAG");		
	pt4_DB_GIP=   d4tag(pd4_DB, "GIP_TAG");
	pt4_DB_IDINT= d4tag(pd4_DB, "IDINT_TAG");		
	pt4_DB_IDSTR= d4tag(pd4_DB, "IDSTR_TAG");
	pt4_DB_UNIQUE=d4tag(pd4_DB, "UNIQUE_TAG");


	return TRUE;

}




/********************************************************/
/*														*/
/*	OpenONAME: Opens the Other Name database			*/
/*														*/
/*	PARAMETERS:											*/
/*		Create-	the database will be created if it		*/
/*				does not already exist	(TRUE/FALSE)	*/
/*	RETURNS:											*/
/*		TRUE-	success									*/
/*		FALSE-	no success								*/
/********************************************************/
Boolean OpenONAME(CharPtr name, Boolean Create)
{
	Char pathdatab[PATH_MAX];

	/*find path name of database (see NCBI toolkit corelib 
	and file "prebind.ini" in windows directory)*/
	pathdatab[0] = '\0';
	GetAppParam("prebind", "databases", "path", NULL, pathdatab, (size_t)PATH_MAX * (sizeof(Char)));
	StringCat(pathdatab, name);

	if (Create == FALSE)
	{
		/*open the datafile or return error*/
		pd4_ONAME = d4open(&codeBase, pathdatab);
		if (pd4_ONAME == NULL)
			return FALSE;
	}
	else
	{
		/*open the datafile or create the datafile or return error*/
		pd4_ONAME= d4open(&codeBase, pathdatab);
		if (pd4_ONAME == NULL)
			pd4_ONAME = d4create(&codeBase, pathdatab, f4iONAME, t4iONAME);
				if (codeBase.errorCode)
					return FALSE;
	}

	/*assign the field pointers*/
	pf4_ONAME_ACC	= d4field(pd4_ONAME, "ACCESSION" );
	pf4_ONAME_GI	= d4field(pd4_ONAME, "GI"        );
	pf4_ONAME_ONAME	= d4field(pd4_ONAME, "ONAME"   ); 

	return TRUE;

}


/********************************************************/
/*														*/
/*	OpenOGI: Opens the Other GI database				*/
/*														*/
/*	PARAMETERS:											*/
/*		Create-	the database will be created if it		*/
/*				does not already exist	(TRUE/FALSE)	*/
/*	RETURNS:											*/
/*		TRUE-	success									*/
/*		FALSE-	no success								*/
/********************************************************/
Boolean OpenOGI(CharPtr name, Boolean Create)
{
	Char pathdatab[PATH_MAX];

	/*find path name of database (see NCBI toolkit corelib 
	and file "prebind.ini" in windows directory)*/
	pathdatab[0] = '\0';
	GetAppParam("prebind", "databases", "path", NULL, pathdatab, (size_t)PATH_MAX * (sizeof(Char)));
	StringCat(pathdatab, name);

	if (Create == FALSE)
	{
		/*open the datafile or return error*/
		pd4_OGI = d4open(&codeBase, pathdatab);
		if (pd4_OGI == NULL)
			return FALSE;
	}
	else
	{
		/*open the datafile or create the datafile or return error*/
		pd4_OGI = d4open(&codeBase, pathdatab);
		if (pd4_OGI == NULL)
			pd4_OGI = d4create(&codeBase, pathdatab, f4iOGI, t4iOGI);
				if (codeBase.errorCode)
					return FALSE;
	}

	/*assign the field pointers*/
	pf4_OGI_ACC	= d4field(pd4_OGI, "ACCESSION" );
	pf4_OGI_GI	= d4field(pd4_OGI, "GI"        );
	pf4_OGI_OGI	= d4field(pd4_OGI, "OGI"       ); 

	return TRUE;

}


/********************************************************/
/*														*/
/*	OpenCDR: Opens the CoDing Region database			*/
/*														*/
/*	PARAMETERS:											*/
/*		Create-	the database will be created if it		*/
/*				does not already exist	(TRUE/FALSE)	*/
/*	RETURNS:											*/
/*		TRUE-	success									*/
/*		FALSE-	no success								*/
/********************************************************/
Boolean OpenCDR(CharPtr name, Boolean Create)
{
	Char pathdatab[PATH_MAX];

	/*find path name of database (see NCBI toolkit corelib 
	and file "prebind.ini" in windows directory)*/
	pathdatab[0] = '\0';
	GetAppParam("prebind", "databases", "path", NULL, pathdatab, (size_t)PATH_MAX * (sizeof(Char)));
	StringCat(pathdatab, name);

	if (Create == FALSE)
	{
		/*open the datafile or return error*/
		pd4_CDR = d4open(&codeBase, pathdatab);
		if (pd4_CDR == NULL)
			return FALSE;
	}
	else
	{
		/*open the datafile or create the datafile or return error*/
		pd4_CDR = d4open(&codeBase, pathdatab);
		if (pd4_CDR == NULL)
			pd4_CDR = d4create(&codeBase, pathdatab, f4iCDR, t4iCDR);
				if (codeBase.errorCode)
					return FALSE;
	}

	/*assign the field pointers*/
	pf4_CDR_GIN	= d4field(pd4_CDR, "GI_NUC" );
	pf4_CDR_FROM= d4field(pd4_CDR, "FROM" );
	pf4_CDR_TO	= d4field(pd4_CDR, "TO" );
	pf4_CDR_GIP	= d4field(pd4_CDR, "GI_PROT");

	/*assign the tag pointers*/
	pt4_CDR_GIN= d4tag(pd4_CDR, "GIN_TAG");		
	pt4_CDR_GIP= d4tag(pd4_CDR, "GIP_TAG");

	return TRUE;

}


/********************************************************/
/*														*/
/*	OpenLOCUS: Opens the LOCUS name database			*/
/*														*/
/*	PARAMETERS:											*/
/*		Create-	the database will be created if it		*/
/*				does not already exist	(TRUE/FALSE)	*/
/*	RETURNS:											*/
/*		TRUE-	success									*/
/*		FALSE-	no success								*/
/********************************************************/

Boolean OpenLOCUS(CharPtr name, Boolean Create)
{
	Char pathdatab[PATH_MAX];

	/*find path name of database (see NCBI toolkit corelib 
	and file "prebind.ini" in windows directory)*/
	pathdatab[0] = '\0';
	GetAppParam("prebind", "databases", "path", NULL, pathdatab, (size_t)PATH_MAX * (sizeof(Char)));
	StringCat(pathdatab, name);

	if (Create == FALSE)
	{
		/*open the datafile or return error*/
		pd4_LOCUS = d4open(&codeBase, pathdatab);
		if (pd4_LOCUS == NULL)
			return FALSE;
	}
	else
	{
		/*open the datafile or create the datafile or return error*/
		pd4_LOCUS = d4open(&codeBase, pathdatab);
		if (pd4_LOCUS == NULL)
			pd4_LOCUS = d4create(&codeBase, pathdatab, f4iLOCUS, t4iLOCUS);
				if (codeBase.errorCode)
					return FALSE;
	}

	/*assign the field pointers*/
	pf4_LOCUS_GIN	= d4field(pd4_LOCUS, "GI_NUC" );
	pf4_LOCUS_FROM	= d4field(pd4_LOCUS, "FROM" );
	pf4_LOCUS_TO	= d4field(pd4_LOCUS, "TO" );
	pf4_LOCUS_NAME	= d4field(pd4_LOCUS, "NAME");
	pf4_LOCUS_GIP	= d4field(pd4_LOCUS, "GI_PROT");

	/*assign the tag pointers*/
	pt4_LOCUS_GIN=		d4tag(pd4_LOCUS, "GIN_TAG");		
	pt4_LOCUS_GIP=		d4tag(pd4_LOCUS, "GIP_TAG");
	pt4_LOCUS_UNIQUE=	d4tag(pd4_LOCUS, "UNIQUE_TAG");

	return TRUE;

}


/********************************************************/
/*														*/
/*	OpenLOSYN: Opens the LOcus SYNonym database			*/
/*														*/
/*	PARAMETERS:											*/
/*		Create-	the database will be created if it		*/
/*				does not already exist	(TRUE/FALSE)	*/
/*	RETURNS:											*/
/*		TRUE-	success									*/
/*		FALSE-	no success								*/
/********************************************************/
Boolean OpenLOSYN(CharPtr name, Boolean Create)
{
	Char pathdatab[PATH_MAX];

	/*find path name of database (see NCBI toolkit corelib 
	and file "prebind.ini" in windows directory)*/
	pathdatab[0] = '\0';
	GetAppParam("prebind", "databases", "path", NULL, pathdatab, (size_t)PATH_MAX * (sizeof(Char)));
	StringCat(pathdatab, name);

	if (Create == FALSE)
	{
		/*open the datafile or return error*/
		pd4_LOSYN = d4open(&codeBase, pathdatab);
		if (pd4_LOSYN == NULL)
			return FALSE;
	}
	else
	{
		/*open the datafile or create the datafile or return error*/
		pd4_LOSYN = d4open(&codeBase, pathdatab);
		if (pd4_LOSYN == NULL)
			pd4_LOSYN = d4create(&codeBase, pathdatab, f4iLOSYN, t4iLOSYN);
				if (codeBase.errorCode)
					return FALSE;
	}

	/*assign the field pointers*/
	pf4_LOSYN_GIN	= d4field(pd4_LOSYN, "GI_NUC" );
	pf4_LOSYN_FROM	= d4field(pd4_LOSYN, "FROM" );
	pf4_LOSYN_TO	= d4field(pd4_LOSYN, "TO" );
	pf4_LOSYN_SYN	= d4field(pd4_LOSYN, "SYN");
	pf4_LOSYN_GIP	= d4field(pd4_LOSYN, "GI_PROT");

	/*assign the tag pointers*/
	pt4_LOSYN_GIN=		d4tag(pd4_LOSYN, "GIN_TAG");		
	pt4_LOSYN_GIP=		d4tag(pd4_LOSYN, "GIP_TAG");
	pt4_LOSYN_UNIQUE =	d4tag(pd4_LOSYN, "UNIQUE_TAG");

	return TRUE;

}



/************************************************************/
/*															*/
/*	GetTaxidbyGI:											*/
/*			Given a GI return the Taxon id from the		 	*/
/*			ACCession database.
/*	PARAMETERS:												*/
/*			Int4	gi										*/
/*	RETURNS:												*/
/*			Int4:	taxon id of gi							*/
/*			0:		no taxon id found						*/
/************************************************************/
Int4 GetTaxidbyGI(Int4 gi)
{	
	
	Int4		taxid =	0;
	Int2		acc_record = 0;
	Int2		locus_record = 0;
	Int4		protein_gi = 0;

	/*Assume first that the passed gi is a protein (in which case it will be found
	in the ACCession database).  If the gi is not found then assume it is a DNA and 
	look for it (and a corresponding protein gi) in the LOCUS database. Call this 
	function with that gi.*/


	/*find a ACCesion record with the gi number passed to this function*/
	/*select ACCession tag*/
	d4tagSelect(pd4_ACC, pt4_ACC_GI);

	acc_record = d4seekDouble(pd4_ACC, gi);
	if (acc_record != 0)
	{
		d4tagSelect(pd4_LOCUS, pt4_LOCUS_GIN);
		locus_record = d4seekDouble (pd4_LOCUS, gi);
		if (locus_record == 0)
		{
			protein_gi = f4long(pf4_LOCUS_GIP);
			if (protein_gi != 0)
			{
				taxid = GetTaxidbyGI(protein_gi);
			}
			return taxid;
		}
		else
		{
			taxid = 0; /*there are no ACCession or LOCUS records for the gi*/
			return taxid;
		}
	}
	else if (acc_record == 0) /*r4 success*/
	{
		taxid = f4long(pf4_ACC_TAXID);
	}

	return taxid;
}


/************************************************************/
/*															*/
/*	GetAccessionRecordbyGI: given a gi, this function		*/
/*						finds the accession record in the	*/
/*						ACCession database and fills a		*/
/*						PREBINDAcc struct with data from the*/ 
/*						record and returns a pointer to the	*/ 
/*						struct								*/					
/*															*/
/*	PARAMETERS:												*/
/*		Int4			gi									*/
/*	RETURNS:												*/
/*		PREBINDAccPtr										*/
/*		NULL			Returns NULL if the record was		*/
/*						not found							*/
/************************************************************/

PREBINDAccPtr GetAccessionRecordbyGI(Int4 gi)
{
	PREBINDAccPtr	newrecord =		NULL;
	Int2			acc_record =	0;
	CharPtr			accession =		NULL;
	CharPtr			database =		NULL;
	CharPtr			locus_name =	NULL;
	CharPtr			gene_product =	NULL;
	Int4			taxid =			0;
	
	/*allocate memory for a new record*/
	newrecord = PREBINDAccNew();


	/*select the gi tag*/
	d4tagSelect(pd4_ACC, pt4_ACC_GI);
		
	acc_record = d4seekDouble(pd4_ACC, gi);
	if (acc_record == 0)/*r4success- copy each of the field contents into the PREBINDAcc struct*/
	{
		/*gi*/
		newrecord->gi = gi;

		/*accession*/
		newrecord->acc = StrSave(f4str(pf4_ACC_ACC));
		if (newrecord->acc == NULL) newrecord->acc = StrSave("NOT FOUND");
		if(TrimSpaces(newrecord->acc) != 0)
			ErrPostEx(SEV_ERROR,0,0, "GetAccessionRecordbyGI: TrimSpaces failed, possibly passed a null string");
		
		/*database*/
		newrecord->db = StrSave(f4str(pf4_ACC_DB)); 
		if (newrecord->db == NULL) newrecord->db = StrSave("NOT FOUND");
		if(TrimSpaces(newrecord->db) != 0)
			ErrPostEx(SEV_ERROR,0,0, "GetAccessionRecordbyGI: TrimSpaces failed, possibly passed a null string");
		
		/*locus*/
		newrecord->locus = StrSave(f4str(pf4_ACC_LOCUS)); 
		if (newrecord->locus == NULL) newrecord->locus = StrSave("NOT FOUND");
		if(TrimSpaces(newrecord->locus) != 0)
			ErrPostEx(SEV_ERROR,0,0, "GetAccessionRecordbyGI: TrimSpaces failed, possibly passed a null string");

		/*preferred product*/
		newrecord->pprod = StrSave(f4memoStr(pf4_ACC_PPROD)); 
		if (newrecord->pprod == NULL) newrecord->pprod = StrSave("NOT FOUND");

		/*taxid*/
		newrecord->tax = f4long(pf4_ACC_TAXID);
		
		/*last updated*/
		newrecord->update = StrSave(f4str(pf4_ACC_UPDATED));
		if (newrecord->update == NULL) newrecord->update = StrSave("00000000");

	}
	else if (acc_record != 0)
	{
		ErrPostEx(SEV_ERROR, 0, 0, "GetAccessionRecordbyGI: record not found for gi number = %ld\n", (long) gi);
		return NULL;
	}

	return newrecord;

}

void GetFullPath(CharPtr pathdatab, CharPtr name)
{
    pathdatab[0] = '\0';
    GetAppParam("prebind", "databases", "path", NULL, pathdatab, (size_t)PATH_MAX * (sizeof(Char)));
    StringCat(pathdatab, name);
}

/***************************************************/
/*assign the field pointers*/
void AssignFieldsREGI(void)
{
    pf4_REGI_PMID   = d4field(pd4_REGI, "PMID" );
    pf4_REGI_MUID   = d4field(pd4_REGI, "MUID" );
    pf4_REGI_GIP    = d4field(pd4_REGI, "GIP" );
}

/*assign the tag pointers*/
void AssignTagsREGI(void)
{
    INDEX4 *pi4_REGI;
    pi4_REGI = i4open(pd4_REGI, NULL);
    
    pt4_REGI_PMID  = d4tag(pd4_REGI, "PMID_TAG");
    pt4_REGI_MUID  = d4tag(pd4_REGI, "MUID_TAG");
    pt4_REGI_GIP   = d4tag(pd4_REGI, "GIP_TAG");
    pt4_REGI_UNIQUE= d4tag(pd4_REGI, "UNIQUE_TAG");
  
}
/* open the table without indexing to speed up operation*/
Boolean OpenREGINoidx(CharPtr name)
{
    Char pathdatab[PATH_MAX];
    GetFullPath(pathdatab, name);

    pd4_REGI = d4open(&codeBase, pathdatab);
    if (pd4_REGI == NULL)
        return FALSE;

   AssignFieldsREGI();
   return TRUE;
}

/* reindex and assign tags */
Boolean ReindexREGI(void)
{
    INDEX4 *pi4_REGI;
    if(pd4_REGI == NULL)
        return FALSE;
    pi4_REGI = i4open(pd4_REGI, NULL);
    if (pi4_REGI == NULL) 
        return FALSE;
    if(d4reindex(pd4_REGI)!=0)
        return FALSE;
 
    return TRUE;
}

/***************************************************/
void AssignFieldsSUBMIT(void)
{
   pf4_SUBMIT_PMID   = d4field(pd4_SUBMIT, "PMID");    /*PubMed IDentifier */
 pf4_SUBMIT_MUID   = d4field(pd4_SUBMIT, "MUID");    /*Medine IDentifier */
 pf4_SUBMIT_GIA    = d4field(pd4_SUBMIT, "GIA");     /*GIs of protein A in interaction*/
    pf4_SUBMIT_GIB    = d4field(pd4_SUBMIT, "GIB");     /*GIs of protein B in interaction*/
    pf4_SUBMIT_SGIA   = d4field(pd4_SUBMIT, "SGIA");    /*Submitted GI of molecule A*/
 pf4_SUBMIT_NAMA   = d4field(pd4_SUBMIT, "NAMA");    /*NAMes of molecule A in interaction*/
 pf4_SUBMIT_ADESC  = d4field(pd4_SUBMIT, "ADESC");   /*DESCription of molecule A in interaction*/
   pf4_SUBMIT_ATYPE  = d4field(pd4_SUBMIT, "ATYPE");   /*A: TYPE of molecule in interaction*/
 pf4_SUBMIT_AOK    = d4field(pd4_SUBMIT, "AOK");     /*sgiA describes molecule A OK?*/
  pf4_SUBMIT_AFROM  = d4field(pd4_SUBMIT, "AFROM");   /*AFROM binding location*/
 pf4_SUBMIT_ATO    = d4field(pd4_SUBMIT, "ATO");     /*ATO binding location*/
   pf4_SUBMIT_SGIB   = d4field(pd4_SUBMIT, "SGIB");    /*Submitted GI of molecule B*/
 pf4_SUBMIT_NAMB   = d4field(pd4_SUBMIT, "NAMB");    /*NAMes of molecule B in interaction*/
 pf4_SUBMIT_BDESC  = d4field(pd4_SUBMIT, "BDESC");   /*DESCription of molecule B in interaction*/
   pf4_SUBMIT_BTYPE  = d4field(pd4_SUBMIT, "BTYPE");   /*B: TYPE of molecule in interaction*/
 pf4_SUBMIT_BOK    = d4field(pd4_SUBMIT, "BOK");     /*sgiB describes molecule B OK?*/
  pf4_SUBMIT_BFROM  = d4field(pd4_SUBMIT, "BFROM");   /*BFROM binding location*/
 pf4_SUBMIT_BTO    = d4field(pd4_SUBMIT, "BTO");     /*BTO binding location*/
   pf4_SUBMIT_METHOD = d4field(pd4_SUBMIT, "METHOD");  /*METHOD used to show interaction*/
    pf4_SUBMIT_OPIN   = d4field(pd4_SUBMIT, "OPIN");    /*OPINion (is this an interaction or not*/
 pf4_SUBMIT_FNAME  = d4field(pd4_SUBMIT, "FNAME");   /*First NAME*/
 pf4_SUBMIT_LNAME  = d4field(pd4_SUBMIT, "LNAME");   /*Last NAME*/
  pf4_SUBMIT_BUID   = d4field(pd4_SUBMIT, "BUID");    /*Bind User ID*/
   pf4_SUBMIT_DATE   = d4field(pd4_SUBMIT, "DATE");    /*DATE submitted*/
 pf4_SUBMIT_STATUS = d4field(pd4_SUBMIT, "STATUS");  /*STATUS*/
}
void AssignTagsSUBMIT(void)
{
   INDEX4 *pi4_SUBMIT;
    pi4_SUBMIT = i4open(pd4_SUBMIT, NULL);
    
   pt4_SUBMIT_PMID = d4tag(pd4_SUBMIT, "PMID_TAG");    /*PubMed IDentifier */
    pt4_SUBMIT_MUID = d4tag(pd4_SUBMIT, "MUID_TAG");    /*Medine IDentifier */
    pt4_SUBMIT_GIA  = d4tag(pd4_SUBMIT, "GIA_TAG");     /*GIs of protein A in interaction*/
    pt4_SUBMIT_GIB  = d4tag(pd4_SUBMIT, "GIB_TAG");     /*GIs of protein B in interaction*/
    pt4_SUBMIT_SGIA = d4tag(pd4_SUBMIT, "SGIA_TAG");    /*Submitted GI of molecule A*/
    pt4_SUBMIT_NAMA = d4tag(pd4_SUBMIT, "NAMA_TAG");    /*NAMes of molecule A in interaction*/
    pt4_SUBMIT_SGIB = d4tag(pd4_SUBMIT, "SGIB_TAG");    /*Submitted GI of molecule B*/
    pt4_SUBMIT_NAMB = d4tag(pd4_SUBMIT, "NAMB_TAG");    /*NAMes of molecule B in interaction*/
    pt4_SUBMIT_FNAME= d4tag(pd4_SUBMIT, "FNAME_TAG");   /*First NAME*/
    pt4_SUBMIT_LNAME= d4tag(pd4_SUBMIT, "LNAME_TAG");   /*Last NAME*/
    pt4_SUBMIT_BUID = d4tag(pd4_SUBMIT, "BUID_TAG");    /*Bind User ID*/
    pt4_SUBMIT_DATE = d4tag(pd4_SUBMIT, "DATE_TAG");    /*DATE submitted*/
    pt4_SUBMIT_STATUS = d4tag(pd4_SUBMIT, "STATUS_TAG");/*STATUS*/
  
}
Boolean OpenSUBMITNoidx(CharPtr name)
{
   Char pathdatab[PATH_MAX];
    GetFullPath(pathdatab, name);

     /*open the datafile or return error*/
     pd4_SUBMIT = d4open(&codeBase, pathdatab);
     if (pd4_SUBMIT == NULL)
            return FALSE;
    
    AssignFieldsSUBMIT();
   return TRUE;
}

Boolean ReindexSUBMIT(void)
{
    INDEX4 *pi4_SUBMIT;
    if(pd4_SUBMIT == NULL)
        return FALSE;
    pi4_SUBMIT = i4open(pd4_SUBMIT, NULL);
    if (pi4_SUBMIT == NULL) 
        return FALSE;
    if(d4reindex(pd4_SUBMIT)!=0)
        return FALSE;

    return TRUE;
}

/***************************************************/
void AssignFieldsPOINT(void)
{
    pf4_POINT_PMID      = d4field(pd4_POINT, "PMID" );
 pf4_POINT_MUID      = d4field(pd4_POINT, "MUID" );
 pf4_POINT_GIA       = d4field(pd4_POINT, "GIA"  );
 pf4_POINT_NAMA      = d4field(pd4_POINT, "NAMA" );
 pf4_POINT_ATYPE     = d4field(pd4_POINT, "ATYPE");
 pf4_POINT_GIB       = d4field(pd4_POINT, "GIB"  );
 pf4_POINT_NAMB      = d4field(pd4_POINT, "NAMB" );
 pf4_POINT_BTYPE     = d4field(pd4_POINT, "BTYPE");
 pf4_POINT_OPNO      = d4field(pd4_POINT, "OPNO" );
 pf4_POINT_OPYES     = d4field(pd4_POINT, "OPYES");
 pf4_POINT_PISCORE   = d4field(pd4_POINT, "PISCORE" );
  pf4_POINT_PISCORE2  = d4field(pd4_POINT, "PISCORE2");
  pf4_POINT_PITRUTH   = d4field(pd4_POINT, "PITRUTH" );
  
}
void AssignTagsPOINT(void)
{
    INDEX4 *pi4_POINT;
    pi4_POINT = i4open(pd4_POINT, NULL);
    
      pt4_POINT_PMID      = d4tag(pd4_POINT, "PMID_TAG");
    pt4_POINT_MUID      = d4tag(pd4_POINT, "MUID_TAG");
    pt4_POINT_GIA       = d4tag(pd4_POINT, "GIA_TAG" );
    pt4_POINT_GIB       = d4tag(pd4_POINT, "GIB_TAG" );
    pt4_POINT_PISCORE   = d4tag(pd4_POINT, "PISCORE_TAG" );
    pt4_POINT_PITRUTH   = d4tag(pd4_POINT, "PITRUTH_TAG" );
}

Boolean OpenPOINTNoidx(CharPtr name)
{
    Char pathdatab[PATH_MAX];
    GetFullPath(pathdatab, name);

    pd4_POINT = d4open(&codeBase, pathdatab);
    if (pd4_POINT == NULL)
        return FALSE;
   
   AssignFieldsPOINT();

   return TRUE;

}

Boolean ReindexPOINT(void)
{
    INDEX4 *pi4_POINT;
    if(pd4_POINT == NULL)
        return FALSE;
    pi4_POINT= i4open(pd4_POINT, NULL);
    if (pi4_POINT == NULL) 
        return FALSE;
    if(d4reindex(pd4_POINT)!=0)
        return FALSE;
    return TRUE;
}        

/***************************************************/
void AssignFieldsCORE(void)
{
    pf4_CORE_PMID   = d4field(pd4_CORE, "PMID" );
  pf4_CORE_MUID   = d4field(pd4_CORE, "MUID" );
  pf4_CORE_NUMN   = d4field(pd4_CORE, "NUMN" );
  pf4_CORE_NUMG   = d4field(pd4_CORE, "NUMG" );
  
}
void AssignTagsCORE(void)
{
    INDEX4 *pi4_CORE;
    pi4_CORE = i4open(pd4_CORE, NULL);

    
    pt4_CORE_PMID   = d4tag(pd4_CORE, "PMID_TAG");
 pt4_CORE_MUID   = d4tag(pd4_CORE, "MUID_TAG");
 pt4_CORE_NUMN   = d4tag(pd4_CORE, "NUMN_TAG");
 pt4_CORE_NUMG   = d4tag(pd4_CORE, "NUMG_TAG");
  
}
Boolean OpenCORENoidx(CharPtr name)
{
 Char pathdatab[PATH_MAX];
    GetFullPath(pathdatab, name);
 
     pd4_CORE = d4open(&codeBase, pathdatab);
     if (pd4_CORE == NULL)
          return FALSE;

    AssignFieldsCORE();

    return TRUE;

}

Boolean ReindexCORE(void)
{
    INDEX4 *pi4_CORE;
    if(pd4_CORE == NULL)
        return FALSE;
    pi4_CORE= i4open(pd4_CORE, NULL);
    if (pi4_CORE == NULL) 
        return FALSE;
    if(d4reindex(pd4_CORE)!=0)
        return FALSE;
    return TRUE;
}   

/***************************************************/
void AssignFieldsISVM(void)
{ /*assign the field pointers*/
  pf4_ISVM_PMID    = d4field(pd4_ISVM, "PMID"   );
   pf4_ISVM_MUID    = d4field(pd4_ISVM, "MUID"   );
   pf4_ISVM_ISCORE  = d4field(pd4_ISVM, "ISCORE" );
   pf4_ISVM_ISCORE2 = d4field(pd4_ISVM, "ISCORE2" );
  pf4_ISVM_OPNO    = d4field(pd4_ISVM, "OPNO" );
 pf4_ISVM_OPYES   = d4field(pd4_ISVM, "OPYES" );
    pf4_ISVM_ITRUTH  = d4field(pd4_ISVM, "ITRUTH" );
}
void AssignTagsISVM(void)
{ 
    INDEX4 *pi4_ISVM;
    pi4_ISVM = i4open(pd4_ISVM, NULL);
    
    /*assign the tag pointers*/
    pt4_ISVM_PMID   = d4tag(pd4_ISVM, "PMID_TAG");
    pt4_ISVM_MUID   = d4tag(pd4_ISVM, "MUID_TAG");
 
}

Boolean OpenISVMNoidx(CharPtr name)
{
    Char pathdatab[PATH_MAX];
    GetFullPath(pathdatab, name);


      pd4_ISVM = d4open(&codeBase, pathdatab);
       if (pd4_ISVM == NULL)
          return FALSE;

    AssignFieldsISVM();
   return TRUE;

}

Boolean ReindexISVM(void)
{
    INDEX4 *pi4_ISVM;
    if(pd4_ISVM == NULL)
        return FALSE;
    pi4_ISVM = i4open(pd4_ISVM, NULL);
    if (pi4_ISVM == NULL) 
        return FALSE;
    if(d4reindex(pd4_ISVM)!=0)
        return FALSE;
    return TRUE;
}        

/***************************************************/
void AssignFieldsRESULT(void)
{    /*assign the field pointers*/
    pf4_RESULT_GIP  = d4field(pd4_RESULT, "GI_PROT" );
    pf4_RESULT_NAME = d4field(pd4_RESULT, "NAME"    );
    pf4_RESULT_PMID = d4field(pd4_RESULT, "PMID"    );
}
void AssignTagsRESULT(void)
{   
   INDEX4 *pi4_RESULT;
   if(pd4_RESULT == NULL) printf("AssignTagsRESULT(): pd4_RESULT is NULL\n");
   else{
        pi4_RESULT = i4open(pd4_RESULT, NULL);
        if(pi4_RESULT == NULL) printf("AssignTagsRESULT(): pi4_RESULT is NULL\n");
        else{
            /*assign the tag pointers so index can be used */
            pt4_RESULT_GIP    = d4tag(pd4_RESULT,"GIP_TAG");
            pt4_RESULT_NAME   = d4tag(pd4_RESULT,"NAME_TAG");
            pt4_RESULT_PMID   = d4tag(pd4_RESULT,"PMID_TAG");
            pt4_RESULT_UNIQUE = d4tag(pd4_RESULT,"UNIQUE_TAG");
            printf("AssignTagsRESULT(): successful\n");   
    }
   }
}

Boolean OpenRESULTNoidx(CharPtr name)
{
    Char pathdatab[PATH_MAX];
    GetFullPath(pathdatab, name);
    
    /* open RESULT without index */
    pd4_RESULT = d4open(&codeBase, pathdatab );
    if (pd4_RESULT == NULL)
          return FALSE;
    AssignFieldsRESULT();
    
    return TRUE;

}

Boolean ReindexRESULT(void)
{
    INDEX4 *pi4_RESULT;
    if(pd4_RESULT == NULL) return FALSE;
    pi4_RESULT = i4open(pd4_RESULT, NULL);
    if (pi4_RESULT == NULL) return FALSE;
    if(d4reindex(pd4_RESULT)!=0) 
        return FALSE;

    return TRUE;
    
}



/***************************************************/
void AssignFieldsSRCH(void)
{
    /*assign the field pointers*/
    pf4_SRCH_GIP      = d4field(pd4_SRCH, "GI_PROT" );
    pf4_SRCH_NAME     = d4field(pd4_SRCH, "NAME"    );
    pf4_SRCH_SOURCE   = d4field(pd4_SRCH, "SOURCE"  );
    pf4_SRCH_TAXID    = d4field(pd4_SRCH, "TAXID"   );
    pf4_SRCH_SEARCHED = d4field(pd4_SRCH, "SEARCHED"); /*last date that search was performed*/
    pf4_SRCH_STATUS   = d4field(pd4_SRCH, "STATUS");   /*0- not approved, 1- approved, 2- search performed */
    pf4_SRCH_NAMERED  = d4field(pd4_SRCH, "NAMERED");  /*name redundancy- no. times this name appears for organism*/
    pf4_SRCH_RESULTS  = d4field(pd4_SRCH, "RESULTS");
}
void AssignTagsSRCH(void)
{
   INDEX4 *pi4_SRCH;
    pi4_SRCH = i4open(pd4_SRCH, NULL);

 /*assign the tag pointers*/
    pt4_SRCH_GIP    = d4tag(pd4_SRCH,"GIP_TAG");
   pt4_SRCH_NAME   = d4tag(pd4_SRCH,"NAME_TAG");
  pt4_SRCH_CIN    = d4tag(pd4_SRCH,"CIN_TAG");
   pt4_SRCH_UNIQUE = d4tag(pd4_SRCH,"UNIQUE_TAG");
}
Boolean OpenSRCHNoidx(CharPtr name)
{
 Char pathdatab[PATH_MAX];
    GetFullPath(pathdatab, name);

      pd4_SRCH = d4open(&codeBase, pathdatab);
       if (pd4_SRCH == NULL)
          return FALSE;
          
    AssignFieldsSRCH();

   return TRUE;

}

Boolean ReindexSRCH(void)
{
    INDEX4 *pi4_SRCH;
    if(pd4_SRCH == NULL)
        return FALSE;
    pi4_SRCH = i4open(pd4_SRCH, NULL);
    if (pi4_SRCH == NULL) 
        return FALSE;
    if(d4reindex(pd4_SRCH)!=0)
        return FALSE;
    return TRUE;
}        



/***************************************************/
void AssignFieldsYENA(void)
{
 /*assign the field pointers*/
  pf4_YENA_GIP  = d4field(pd4_YENA, "GI_PROT");
  pf4_YENA_NAME = d4field(pd4_YENA, "NAME"   );
   
}
void AssignTagsYENA(void)
{
 INDEX4 *pi4_YENA;

    pi4_YENA = i4open(pd4_YENA, NULL);
    
    /*assign the tag pointers*/
    pt4_YENA_GIP =    d4tag(pd4_YENA,"GIP_TAG");
   pt4_YENA_UNIQUE = d4tag(pd4_YENA,"UNIQUE_TAG");
}

Boolean OpenYENANoidx(CharPtr name)
{
Char pathdatab[PATH_MAX];
    GetFullPath(pathdatab, name);

      pd4_YENA = d4open(&codeBase, pathdatab);
       if (pd4_YENA == NULL)
          return FALSE;
    AssignFieldsYENA();
   return TRUE;

}
Boolean ReindexYENA(void)
{
    INDEX4 *pi4_YENA;
    if(pd4_YENA == NULL)
        return FALSE;
    pi4_YENA = i4open(pd4_YENA, NULL);
    if (pi4_YENA == NULL) 
        return FALSE;
    if(d4reindex(pd4_YENA)!=0)
        return FALSE;
    return TRUE;
}        


/***************************************************/
void AssignFieldsACC(void)
{
 /*assign the field pointers*/
  pf4_ACC_ACC      = d4field(pd4_ACC, "ACCESSION" );
 pf4_ACC_GI       = d4field(pd4_ACC, "GI"        );
 pf4_ACC_DB       = d4field(pd4_ACC, "DATABASE"  ); 
    pf4_ACC_LOCUS    = d4field(pd4_ACC, "LOCUS"   );
   pf4_ACC_DEF      = d4field(pd4_ACC, "DEFINITION");
 pf4_ACC_PPROD    = d4field(pd4_ACC, "PPROD"     );
 pf4_ACC_TAXID    = d4field(pd4_ACC, "TAXID"     );
 pf4_ACC_UPDATED  = d4field(pd4_ACC, "UPDATED"   ); 
}
void AssignTagsACC(void)
{    
   INDEX4 *pi4_ACC;
    pi4_ACC = i4open(pd4_ACC, NULL);
    
    /*assign the tag pointers*/
    pt4_ACC_GI=   d4tag(pd4_ACC, "GI_TAG"); 
    pt4_ACC_ACC=  d4tag(pd4_ACC, "ACC_TAG");/*accessions are unique to this database*/
    pt4_ACC_RS=   d4tag(pd4_ACC, "RSQ_TAG");

 
}
Boolean OpenACCNoidx(CharPtr name)
{
  Char pathdatab[PATH_MAX];
    GetFullPath(pathdatab, name);
 
      pd4_ACC = d4open(&codeBase, pathdatab);
        if (pd4_ACC == NULL)
           return FALSE;
 AssignFieldsACC();
 return TRUE;

}

Boolean ReindexACC(void)
{
    INDEX4 *pi4_ACC;
    if(pd4_ACC == NULL)
        return FALSE;
    pi4_ACC = i4open(pd4_ACC, NULL);
    if (pi4_ACC == NULL) 
        return FALSE;
    if(d4reindex(pd4_ACC)!=0)
        return FALSE;
    return TRUE;
}        


/***************************************************/
void AssignFieldsDBDB(void)
{
 /*assign the field pointers*/
  pf4_DB_GIN   = d4field(pd4_DB, "GI_NUC" );
 pf4_DB_FROM  = d4field(pd4_DB, "FROM"   );
 pf4_DB_TO    = d4field(pd4_DB, "TO"     );
 pf4_DB_GIP   = d4field(pd4_DB, "GI_PROT");
 pf4_DB_DBNAME= d4field(pd4_DB, "DBNAME" ); 
    pf4_DB_IDINT = d4field(pd4_DB, "IDINT"  );
 pf4_DB_IDSTR = d4field(pd4_DB, "IDSTR"  );
}
void AssignTagsDBDB(void)
{
    INDEX4 *pi4_DB;
    pi4_DB = i4open(pd4_DB, NULL);
    
    /*assign the tag pointers*/
    pt4_DB_GIN=   d4tag(pd4_DB, "GIN_TAG");     
   pt4_DB_GIP=   d4tag(pd4_DB, "GIP_TAG");
    pt4_DB_IDINT= d4tag(pd4_DB, "IDINT_TAG");       
   pt4_DB_IDSTR= d4tag(pd4_DB, "IDSTR_TAG");
  pt4_DB_UNIQUE=d4tag(pd4_DB, "UNIQUE_TAG");
   
}
Boolean OpenDBDBNoidx(CharPtr name)
{
 Char pathdatab[PATH_MAX];
    GetFullPath(pathdatab, name);
 
      pd4_DB = d4open(&codeBase, pathdatab);
     if (pd4_DB == NULL)
            return FALSE;

    AssignFieldsDBDB();

   return TRUE;

}

Boolean ReindexDBDB(void)
{
    INDEX4 *pi4_DB;
    if(pd4_DB == NULL)
        return FALSE;
    pi4_DB= i4open(pd4_DB, NULL);
    if (pi4_DB == NULL) 
        return FALSE;
    if(d4reindex(pd4_DB)!=0)
        return FALSE;
    return TRUE;
}        



/***************************************************/
void AssignFieldsONAME(void)
{ /*assign the field pointers*/
  pf4_ONAME_ACC   = d4field(pd4_ONAME, "ACCESSION" );
    pf4_ONAME_GI    = d4field(pd4_ONAME, "GI"        );
    pf4_ONAME_ONAME = d4field(pd4_ONAME, "ONAME"   ); 
   
}
void AssignTagsONAME(void)
{
}
Boolean OpenONAMENoidx(CharPtr name)
{
   Char pathdatab[PATH_MAX];
    GetFullPath(pathdatab, name);

   
      pd4_ONAME = d4open(&codeBase, pathdatab);
      if (pd4_ONAME == NULL)
         return FALSE;

    AssignFieldsONAME();

    return TRUE;

}

Boolean ReindexONAME(void)
{
    INDEX4 *pi4_ONAME;
    if(pd4_ONAME == NULL)
        return FALSE;
    pi4_ONAME = i4open(pd4_ONAME, NULL);
    if (pi4_ONAME == NULL) 
        return FALSE;
    if(d4reindex(pd4_ONAME)!=0)
        return FALSE;
    return TRUE;
}        

/***************************************************/
void AssignFieldsOGI(void)
{ /*assign the field pointers*/
  pf4_OGI_ACC = d4field(pd4_OGI, "ACCESSION" );
  pf4_OGI_GI  = d4field(pd4_OGI, "GI"        );
  pf4_OGI_OGI = d4field(pd4_OGI, "OGI"       ); 
   
}
void AssignTagsOGI(void)
{
}
Boolean OpenOGINoidx(CharPtr name)
{
  Char pathdatab[PATH_MAX];
    GetFullPath(pathdatab, name);

      pd4_OGI = d4open(&codeBase, pathdatab);
        if (pd4_OGI == NULL)
           return FALSE;
    AssignFieldsOGI();

    return TRUE;

}

Boolean ReindexOGI(void)
{
    INDEX4 *pi4_OGI;
    if(pd4_OGI == NULL)
        return FALSE;
    pi4_OGI = i4open(pd4_OGI, NULL);
    if (pi4_OGI == NULL) 
        return FALSE;
    if(d4reindex(pd4_OGI)!=0)
        return FALSE;
    return TRUE;
}        

/***************************************************/
void AssignFieldsCDR(void)
{
 /*assign the field pointers*/
  pf4_CDR_GIN = d4field(pd4_CDR, "GI_NUC" );
 pf4_CDR_FROM= d4field(pd4_CDR, "FROM" );
   pf4_CDR_TO  = d4field(pd4_CDR, "TO" );
 pf4_CDR_GIP = d4field(pd4_CDR, "GI_PROT");
}
void AssignTagsCDR(void)
{   
INDEX4 *pi4_CDR;
    pi4_CDR = i4open(pd4_CDR, NULL);
    
    /*assign the tag pointers*/
    pt4_CDR_GIN= d4tag(pd4_CDR, "GIN_TAG");     
   pt4_CDR_GIP= d4tag(pd4_CDR, "GIP_TAG");
}
Boolean OpenCDRNoidx(CharPtr name)
{
  Char pathdatab[PATH_MAX];
    GetFullPath(pathdatab, name);

 
      pd4_CDR = d4open(&codeBase, pathdatab);
        if (pd4_CDR == NULL)
           return FALSE;
 
    AssignFieldsCDR();

   return TRUE;

}
Boolean ReindexCDR(void)
{
    INDEX4 *pi4_CDR;
    if(pd4_CDR == NULL)
        return FALSE;
    pi4_CDR = i4open(pd4_CDR, NULL);
    if (pi4_CDR == NULL) 
        return FALSE;
    if(d4reindex(pd4_CDR)!=0)
        return FALSE;
    return TRUE;

}        


/***************************************************/
void AssignFieldsLOCUS(void)
{
 /*assign the field pointers*/
  pf4_LOCUS_GIN   = d4field(pd4_LOCUS, "GI_NUC" );
   pf4_LOCUS_FROM  = d4field(pd4_LOCUS, "FROM" );
 pf4_LOCUS_TO    = d4field(pd4_LOCUS, "TO" );
   pf4_LOCUS_NAME  = d4field(pd4_LOCUS, "NAME");
  pf4_LOCUS_GIP   = d4field(pd4_LOCUS, "GI_PROT");
    
}
void AssignTagsLOCUS(void)
{  
    INDEX4 *pi4_LOCUS;
    pi4_LOCUS = i4open(pd4_LOCUS, NULL);
    
    /*assign the tag pointers*/
    pt4_LOCUS_GIN=      d4tag(pd4_LOCUS, "GIN_TAG");        
   pt4_LOCUS_GIP=      d4tag(pd4_LOCUS, "GIP_TAG");
   pt4_LOCUS_UNIQUE=   d4tag(pd4_LOCUS, "UNIQUE_TAG");
 
}

Boolean OpenLOCUSNoidx(CharPtr name)
{
   Char pathdatab[PATH_MAX];
    GetFullPath(pathdatab, name);

      pd4_LOCUS = d4open(&codeBase, pathdatab);
      if (pd4_LOCUS == NULL)
         return FALSE;

    AssignFieldsLOCUS();

   return TRUE;

}
Boolean ReindexLOCUS(void)
{
    INDEX4 *pi4_LOCUS;
    if(pd4_LOCUS == NULL)
        return FALSE;
    pi4_LOCUS = i4open(pd4_LOCUS, NULL);
    if (pi4_LOCUS == NULL) 
        return FALSE;
    if(d4reindex(pd4_LOCUS)!=0)
        return FALSE;
    return TRUE;
}        

/***************************************************/
void AssignFieldsLOSYN(void)
{
    pf4_LOSYN_GIN   = d4field(pd4_LOSYN, "GI_NUC" );
   pf4_LOSYN_FROM  = d4field(pd4_LOSYN, "FROM" );
 pf4_LOSYN_TO    = d4field(pd4_LOSYN, "TO" );
   pf4_LOSYN_SYN   = d4field(pd4_LOSYN, "SYN");
   pf4_LOSYN_GIP   = d4field(pd4_LOSYN, "GI_PROT");
  
}
void AssignTagsLOSYN(void)
{
   INDEX4 *pi4_LOSYN;
    pi4_LOSYN = i4open(pd4_LOSYN, NULL);
    
    pt4_LOSYN_GIN=      d4tag(pd4_LOSYN, "GIN_TAG");        
   pt4_LOSYN_GIP=      d4tag(pd4_LOSYN, "GIP_TAG");
   pt4_LOSYN_UNIQUE =  d4tag(pd4_LOSYN, "UNIQUE_TAG");
  
}

Boolean OpenLOSYNNoidx(CharPtr name)
{
    Char pathdatab[PATH_MAX];
    GetFullPath(pathdatab, name);


      pd4_LOSYN = d4open(&codeBase, pathdatab);
      if (pd4_LOSYN == NULL)
         return FALSE;
 

   AssignFieldsLOSYN();
   return TRUE;

}
Boolean ReindexLOSYN(void)
{
    INDEX4 *pi4_LOSYN;
    if(pd4_LOSYN == NULL)
        return FALSE;
    pi4_LOSYN = i4open(pd4_LOSYN, NULL);
    if (pi4_LOSYN == NULL) 
        return FALSE;
    if(d4reindex(pd4_LOSYN)!=0)
        return FALSE;
    return TRUE;
}





/*
$Log: prebind_db.c,v $
Revision 1.6  2004/05/31 18:59:46  kzheng
import codes generated from prebind_transfer.asn using asntool

Revision 1.5  2004/03/17 23:11:35  kzheng
added functions for opening tables with no index, add reindexing of tables

Revision 1.3  2003/02/26 22:57:11  janicek
Reorganized code for better modularity

Revision 1.2  2002/07/25 16:05:53  janicek
fixed cvs log

*/



