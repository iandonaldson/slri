/* $Id: dbxref_query.c,v 1.6 2004/01/22 22:20:01 zwang Exp $
********************************************************************************
  Copyright (C) 2000 Mount Sinai Hospital (MSH)

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


  PROGRAM:


  AUTHORS:
           Katerina Michalickova and Christopher W.V. Hogue hogue@mshri.on.ca
	   Renan A. CAVERO


  REVISION/CONTRIBUTION NOTES:

  June 1, 2000  Original MSH Revision.
                Hogue Lab - University of Toronto Biochemistry
                Department and the Samuel Lunenfeld Research
                Institute, Mount Sinai Hospital
                http://bioinfo.mshri.on.ca  hogue@mshri.on.ca
                ftp://ftp.mshri.on.ca/


  PUBLICATION TO CITE:


  DESCRIPTION
  September 2003
  SeqHound DBXref database query.
  DBXref Module
 

*******************************************************************************/



#include <asn.h>


#ifdef ODBC
 #include <intrez_odbc.h>
 #include <dbxref_odbc.h>
 #include <dbxref_query.h>
#else
 #include <intrez_db.h>
#endif

#include <slri_misc.h>
#include <intrez_cfg.h>
#include <seqhound.h>
#include <objslristruc.h>



/**************************************************************
                 PUBLIC INITIALIZATION FUNCTIONS
***************************************************************/
/* Connect to Database Server. This funciton must be called 
   prior to any queries to the database can be properly executed. 
   Call this at the start of your program. */
Boolean DBXREF_ConectToDB(Boolean CREATE)
{
    return ConnectToDb();
}


/* Closes the Database Connection.
   Call this function at the end of your program. */
Boolean DBXREF_CloseDB(void)
{
    return CloseDB();
}


SHDBXrefPtr LIBCALL SHoundDBXREFGetDBXrefListBySourceRecord(CharPtr DBName, CharPtr RecordID, CharPtr Field, Int4 CV)
{

    SHDBXrefPtr pSHDBXref = NULL;                  /* Our Return value */
    SHDBXrefPtr auxpSHDBXref = NULL;
    Int2 retcode;
    Char *cvbuffer;

    if(!SHoundModule("dbxref"))
    {
	ErrPostEx(SEV_ERROR,0,0, "[SHoundDBXREFGetDBXrefListBySourceRecord]: Missing required dbxref SeqHound module.");
	return NULL;
    }

    /* Check Arguments */
    if (DBName == NULL)
    {
	ErrPostEx(SEV_ERROR, 0, 0, "[SHoundDBXREFGetDBXrefListBySourceRecord]: DBName can't be NULL.");
	return NULL;
    }
    else if (RecordID == NULL)
    {
	ErrPostEx(SEV_ERROR, 0, 0, "[SHoundDBXREFGetDBXrefListBySourceRecord]: RecordID can't be NULL.");
	return NULL;
    }

    StDbXrefPtr pdbxref = NULL;
    StDbXrefPtr pdbxrefAux = NULL;
    pdbxref = StDbXrefNew();
   
    retcode = GetDBXrefListBySourceRecordInDBXREF(&pdbxref, DBName, RecordID, Field, CV);
    if (retcode != SH_SUCCESS)
    {
	if (retcode == SH_ERROR)
	{
	   ErrPostEx(SEV_ERROR, 0, 0, "[SHoundDBXREFGetDBXrefListBySourceRecord]: Error running GetDBXrefListBySourceRecordInDBXREF."); 
	}
	free(pdbxref);
	return NULL;
    }
   
    pdbxrefAux = pdbxref;
    
    pSHDBXref = SHDBXrefNew();
    auxpSHDBXref = pSHDBXref;

    while(pdbxref != NULL)
    {
	auxpSHDBXref->sourcedb = StringSave((char*) pdbxref->sourcedb);
	auxpSHDBXref->recordid = StringSave((char*) pdbxref->recordid);
	auxpSHDBXref->field    = StringSave((char*) pdbxref->field);
	auxpSHDBXref->cv       = pdbxref->cv;

	pdbxref = pdbxref->next;

	if(pdbxref != NULL)
	{
	    auxpSHDBXref->next = SHDBXrefNew();
	    auxpSHDBXref = auxpSHDBXref->next;
	}
    }
     
    /* Free StDbXrefPtr Link List */
    while(pdbxref != NULL)
    {
	pdbxref = pdbxrefAux;
	pdbxrefAux = pdbxref->next;
	StDbXrefFree(pdbxref);
    }
    return pSHDBXref;
}


SHDBXrefPtr LIBCALL SHoundDBXREFGetSourceRecordListByDBXref(CharPtr DBName, CharPtr RecordID, CharPtr Field, Int4 CV)
{
    SHDBXrefPtr pSHDBXref = NULL;                  /* Our Return value */
    SHDBXrefPtr auxpSHDBXref = NULL;
    Boolean retcode = TRUE;

    if(!SHoundModule("dbxref"))
    {
	ErrPostEx(SEV_ERROR,0,0, "[SHoundDBXREFGetSourceRecordListByDBXref]: Missing required dbxref SeqHound module.");
	return NULL;
    }
    /* Check Arguments */

    if (DBName == NULL)
    {
	ErrPostEx(SEV_ERROR, 0, 0, "[SHoundDBXREFGetSourceRecordListByDBXref]: DBName can't be NULL.");
	return NULL;
    }
    else if (RecordID == NULL)
    {
	ErrPostEx(SEV_ERROR, 0, 0, "[SHoundDBXREFGetSourceRecordListByDBXref]: RecordID can't be NULL.");
	return NULL;
    }

    StDbXrefPtr pdbxref = NULL;
    StDbXrefPtr pdbxrefAux = NULL;
    pdbxref = StDbXrefNew();

    retcode = GetSourceRecordListByDBXrefInDBXREF(&pdbxref, DBName, RecordID, Field, CV);
    if (retcode != SH_SUCCESS)
    {
	if (retcode == SH_ERROR)
	{
	    ErrPostEx(SEV_ERROR, 0, 0, "[SHoundDBXREFGetSourceRecordListByDBXref]: Error running GetSourceRecordListByDBXrefInDBXREF.");
	}
	free(pdbxref);
	return NULL;
    }

    pdbxrefAux = pdbxref;
    
    pSHDBXref = SHDBXrefNew();
    auxpSHDBXref = pSHDBXref;
    
    while(pdbxref != NULL)
    {
	auxpSHDBXref->sourcedb = StringSave((char*) pdbxref->sourcedb);
	auxpSHDBXref->recordid = StringSave((char*) pdbxref->recordid);
	auxpSHDBXref->field    = StringSave((char*) pdbxref->field);
	auxpSHDBXref->cv       = pdbxref->cv;

	pdbxref = pdbxref->next;

	if(pdbxref != NULL)
	{
	    auxpSHDBXref->next = SHDBXrefNew();
	    auxpSHDBXref = auxpSHDBXref->next;
	}
    }
    
    /* Free StDbXrefPtr Link List */
    while(pdbxref != NULL)
    {
	pdbxref = pdbxrefAux;
	pdbxrefAux = pdbxref->next;
	StDbXrefFree(pdbxref);
    }
    return pSHDBXref;
}


ValNodePtr LIBCALL SHoundDBXREFGetListOfSourceDBNames()
{
    ValNodePtr vnp = NULL;                         
    Int4 retcode;

    if(!SHoundModule("dbxref"))
    {
	ErrPostEx(SEV_ERROR,0,0, "[SHoundDBXREFGetListOfSourceDBNames]: Missing required SeqHound module.");
	return NULL;
    }

    retcode = GetDBNameListInDBXREF(&vnp, 0);
    if (retcode != SH_SUCCESS)
    {
	if(retcode == SH_ERROR)
	{
	    ErrPostEx(SEV_ERROR,0,0, "[SHoundDBXREFGetListOfSourceDBNames]: Error running GetDBNameListInDBXREF.");
	}
	return NULL;
    }
    return vnp;
}


ValNodePtr LIBCALL SHoundDBXREFGetListOfXrefDBNames()
{
    ValNodePtr vnp = NULL;                         
    Int4 retcode;

    if(!SHoundModule("dbxref"))
    {
	ErrPostEx(SEV_ERROR,0,0, "[SHoundDBXREFGetListOfXrefDBNames]: Missing required dbxref SeqHound module.");
	return NULL;
    }

    retcode = GetDBNameListInDBXREF(&vnp, 1);
    if (retcode != SH_SUCCESS)
    {
	if (retcode == SH_ERROR)
	{
	    ErrPostEx(SEV_ERROR, 0, 0, "[SHoundDBXREFGetListOfXrefDBNames]:  Error runningGetSourceRecordListByDBXrefInDBXREF.");
	}
	return NULL;
    }

    return vnp;
}


ValNodePtr LIBCALL SHoundDBXREFGetListOfAllDBNames()
{
    ValNodePtr vnp = NULL;                         
    Int4 retcode;

    if(!SHoundModule("dbxref"))
    {
	ErrPostEx(SEV_ERROR,0,0, "[SHoundDBXREFGetListOfAllDBNames]: Missing required dbxref SeqHound module.");
	return NULL;
    }

    retcode = GetDBNameListInDBXREF(&vnp, 2);
    if (retcode != SH_SUCCESS)
    {
	if (retcode == SH_ERROR)
	{
	    ErrPostEx(SEV_ERROR, 0, 0, "[SHoundDBXREFGetListOfAllDBNames]:  Error running GetDBNameListInDBXREF.");
	}
	return NULL;
    }

    return vnp;
}


ValNodePtr LIBCALL SHoundDBXREFGetSourceRecordIDListByDBName(CharPtr DBName)
{
    ValNodePtr vnp = NULL;                         
    Int2 retcode;

    if(!SHoundModule("dbxref"))
    {
	ErrPostEx(SEV_ERROR,0,0, "[SHoundDBXREFGetSourceRecordIDListByDBName]: Missing required dbxref SeqHound module.");
	return NULL;
    }

    if (DBName == NULL)
    {
	ErrPostEx(SEV_ERROR, 0, 0, "[SHoundDBXREFGetSourceRecordIDListByDBName]: DBName can't be NULL.");
	return NULL;
    }

    StDbXrefPtr pdbxref = NULL;
    StDbXrefPtr pdbxrefAux = NULL;
    pdbxref = StDbXrefNew();
    pdbxref->sourcedb = DBName;
    pdbxref->parentid = -1;
    retcode = SearchDBXREF(&pdbxref);
    if (retcode != SH_SUCCESS)
    {
	if (retcode == SH_ERROR)
	{
	    ErrPostEx(SEV_ERROR, 0, 0, "[SHoundDBXREFGetSourceRecordIDListByDBName]:  Error running SearchDBXREF.");
	}
	free(pdbxref);
	return NULL;
    }

    pdbxrefAux = pdbxref;
    while(pdbxref != NULL)
    {
	ValNodeCopyStr(&vnp, 0, pdbxref->recordid);
	pdbxref = pdbxref->next;
    }

    /* Free DBXref Link List */

    pdbxref = pdbxrefAux;
    while (pdbxref != NULL)
    {
	 pdbxrefAux = pdbxref->next;
	 StDbXrefNodeFree(pdbxref);
	 pdbxref = pdbxrefAux;
    }

    return vnp;
}


ValNodePtr LIBCALL SHoundDBXREFGetXrefRecordIDListByDBName(CharPtr DBName)
{

    ValNodePtr vnp = NULL;                         
    Int4 retcode;

    if(!SHoundModule("dbxref"))
    {
	ErrPostEx(SEV_ERROR,0,0, "[SHoundDBXREFGetXrefRecordIDListByDBName]: Missing required dbxref SeqHound module.");
	return NULL;
    }
    if (DBName == NULL)
    {
	ErrPostEx(SEV_ERROR, 0, 0, "[SHoundDBXREFGetXrefRecordIDListByDBName]: DBName can't be NULL.");
	return NULL;
    }

    StDbXrefPtr pdbxref = NULL;
    StDbXrefPtr pdbxrefAux = NULL;
    pdbxref = StDbXrefNew();
    pdbxref->sourcedb = DBName;
    pdbxref->parentid = -2;

    retcode = SearchDBXREF(&pdbxref);
    if (retcode != SH_SUCCESS)
    {
	if (retcode == SH_ERROR)
	{
	    ErrPostEx(SEV_ERROR, 0, 0, "[SHoundDBXREFGetXrefRecordIDListByDBName]:  Error running SearchDBXREF.");
	}
	free(pdbxref);
	return NULL;
    }

    pdbxrefAux = pdbxref;
    /* Copy Records Id from the DBXref LinkList to the ValNodePtr LinkList. */
    while(pdbxref != NULL)
    {
	ValNodeCopyStr(&vnp, 0, pdbxref->recordid);
	pdbxref = pdbxref->next;
    }

    /* Free DBXref Link List */

     pdbxref = pdbxrefAux;

     while (pdbxref != NULL)
     {
	 pdbxrefAux = pdbxref->next;
	 StDbXrefNodeFree(pdbxref);
	 pdbxref = pdbxrefAux;
     }
    return vnp;
}


ValNodePtr LIBCALL SHoundDBXREFGetCVList()
{
    ValNodePtr vnp = NULL;
    ValNodePtr vnpStr = NULL;
    ValNodePtr vnpAux = NULL;
    Boolean retcode;
    Char cvStr[6];
    CharPtr pcv = NULL;

    if(!SHoundModule("dbxref"))
    {
	ErrPostEx(SEV_ERROR,0,0, "[SHoundDBXREFGetCVList]: Missing required dbxref SeqHound module.");
	return NULL;
    }

    retcode = GetCVListInDBXrefSourcedb(&vnp);
    vnpAux = vnp;
    
    if (retcode != SH_SUCCESS)
    {
	if (retcode == SH_ERROR)
	{
	    ErrPostEx(SEV_ERROR, 0, 0, "[SHoundDBXREFGetCVList]: Error running GetCVListInDBXrefSourcedb.");
	}
	return NULL;
    }

    return vnp;
}


ValNodePtr LIBCALL SHoundDBXREFGetXrefSourceFieldName()
{

    ValNodePtr vnp = NULL;
    ValNodePtr vnpStr = NULL;

    Boolean retcode;

    if(!SHoundModule("dbxref"))
    {
	ErrPostEx(SEV_ERROR,0,0, "[SHoundDBXREFGetXrefSourceFieldName]: Missing required dbxref SeqHound module.");
	return NULL;
    }

    retcode = GetXrefSourceFiledName(&vnp);

    if (retcode != SH_SUCCESS)
    {
	if (retcode == SH_ERROR)
	{
	    ErrPostEx(SEV_ERROR, 0, 0, "[SHoundDBXREFGetXrefSourceFieldName]:  Error running GetXrefSourceFiledName.");
	}
	return NULL;
    }

    return vnp;
}


/* returns a linked list of the Gi of the records with goid in vnpgo.
   It is the callers responsibility to free this link list. Returns NULL
   if an error occurs or if goids not found. */
ValNodePtr LIBCALL SHoundSearchDBXREF(ValNodePtr vnpgo)
{
/*
  ValNodePtr v = NULL, vp = NULL,      
  vnpgi = NULL,                      
  vgi = NULL;                         


  if(!SHoundModule("lldb"))
  {
     ErrPostEx(SEV_ERROR,0,0, "SHoundGiFromGOIDList: Missing required SeqHound module.");
     return NULL;
  }

  if (vnpgo == NULL){
    ErrPostEx(SEV_ERROR, 0, 0, "SHoundGiFromGOIDList: vnpgo is NULL.");
    return NULL;
  }

  v = vnpgo;
  while(v != NULL){

   
    vgi = SHoundGiFromGOID(v->data.intvalue);
    vp = vgi;
    while(vp != NULL){

    
      Slri_ValNodeAddInt(&vnpgi, 0, vp->data.intvalue);
      vp = vp->next;
    }
    vgi = ValNodeFree(vgi);
    v = v->next;       
  }
  Misc_IntVnpUnique(&vnpgi);      
  return vnpgi;

*/
}
