/* $Id: ll_cb.c,v 1.12 2004/06/16 15:05:53 hlieu Exp $
**************************************************************************
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
 

  PROGRAM:
 

  AUTHORS:  
            Hao Lieu


  REVISION/CONTRIBUTION NOTES:

  March 1, 2001  Original MSH Revision.
                Hogue Lab - University of Toronto Biochemistry 
                Department and the Samuel Lunenfeld Research 
                Institute, Mount Sinai Hospital
                http://bioinfo.mshri.on.ca  hogue@mshri.on.ca
                ftp://ftp.mshri.on.ca/


  PUBLICATION TO CITE:

  DESCRIPTION:
  Locus Link database management
  Part of SeqHound

****************************************************************************/
#include <ncbi.h>
#include <ll_db.h>
#include <ll_db_p.h>
#include <intrez_misc.h>
#include <slri_cb.h>
#include <slri_misc.h>
#include <slri_math.h>
#include <d4all.h>

/**************************************************
 *       PUBLIC CODE BASE INITIALIZTION           *
 **************************************************/

/* these functions initialize the codebase tags and field
   information.  Alter these only if you know what you are doing */

/* initializes the CODE4 structure and sets various flags.
   This function must be called in any application that uses 
   this database, prior to doing anything to the database.*/
void LL_InitCodeBase(Boolean CREATE)
{
  code4init(&cb);
  cb.errOpen = 0;
  cb.safety = 0;
}

/* closes the databases.  This function must called at the
   end of your function, if you called LL_InitCodeBase.*/
void LL_InitUndoCodeBase(void)
{
  code4close(&cb);
  code4initUndo(&cb);
}

/**********************************************************
       PRIVATE CODE BASE INITIALIZATION FUNCTIONS
***********************************************************/

/* initializes the fields and tags of the file
   of the LL_GO DB file */
static void LL_Init_GO_db_Fields (void)
{
  pF4LL_LL_GO  = d4field(pD4LL_GO, "LL_GO");
  pF4LL_GOID = d4field(pD4LL_GO, "GO_ID");
  pF4LL_PMID = d4field(pD4LL_GO, "PMID");
  pF4LL_EVICODE = d4field(pD4LL_GO, "EVICODE");

  pT4LL_LL_GO = d4tag(pD4LL_GO, "LL_TAG_GO");
  pT4LL_GOID = d4tag(pD4LL_GO, "GOID_TAG");
  pT4LL_PMID = d4tag(pD4LL_GO, "PMID_TAG");
}

/* initializes the fields and tags of the files
   of the LL_OMIM DB file */
static void LL_Init_OMIM_db_Fields(void)
{
  pF4LL_LL_OMIM = d4field(pD4LL_OMIM, "LL_OMIM");
  pF4LL_OMIM = d4field(pD4LL_OMIM, "OMIM");

  pT4LL_LL_OMIM = d4tag(pD4LL_OMIM, "LL_TAG_OMIM");
  pT4LL_OMIM = d4tag(pD4LL_OMIM, "OMIM_TAG");
}
  

/* initializes the fields and tags of the files
   of the LL_CDD DB files */
static void LL_Init_CDD_db_Fields(void)
{
  pF4LL_LL_CDD = d4field(pD4LL_CDD, "LL_CDD");
  pF4LL_CDD = d4field(pD4LL_CDD, "CDD_ID");
  pF4LL_E_VAL = d4field(pD4LL_CDD, "E_VALUE");

  pT4LL_LL_CDD = d4tag(pD4LL_CDD, "LL_TAG_CDD");
  pT4LL_CDD = d4tag(pD4LL_CDD, "CDD_TAG");
}

/* initializes the fields and tags of the files
   of the LL_LLINK DB file */
static void LL_Init_LLID_db_Fields(void)
{
  pF4LL_LL_LLID = d4field(pD4LL_LLID, "LL_LLID");
  pF4LL_GI = d4field(pD4LL_LLID, "GI_ID");
  pF4LL_MAP = d4field(pD4LL_LLID, "LL_MAP");

  pT4LL_LL_LLID = d4tag(pD4LL_LLID, "LL_TAG_LL");
  pT4LL_GI = d4tag(pD4LL_LLID, "GI_TAG");
}

/***************************************************
           PRIVATE OPEN DATABASE FUNCTIONS
****************************************************/
/* opens and sets the fields and tags of LL_GO code base files */
static Boolean LL_OpenGO_db(CharPtr path, Boolean CREATE)
{
  CharPtr DBpath;
  Boolean res = FALSE;

  DBpath = AllocateAndCreateDBPath(path, "LL_GO");
  /* Create GO table if the flag is TRUE */
    res = OpenDBII(DBpath, &pD4LL_GO, fieldInfo_GO, tagInfo_GO,
		  (InitFieldFunc) LL_Init_GO_db_Fields, &cb, CREATE);
  MemFree(DBpath);
  return res;
}

/* opens and sets the fields and tags of LL_OMIM code base files */
static Boolean LL_OpenOMIM_db(CharPtr path, Boolean CREATE)
{
  CharPtr DBpath;
  Boolean res = FALSE;


  DBpath = AllocateAndCreateDBPath(path, "LL_OMIM");
    res = OpenDBII(DBpath, &pD4LL_OMIM, fieldInfo_OMIM, tagInfo_OMIM,
		  (InitFieldFunc) LL_Init_OMIM_db_Fields, &cb, CREATE);
  MemFree(DBpath);
  return res;
}

/* open and sets the fields and tags of LL_CDD code base files */
static Boolean LL_OpenCDD_db(CharPtr path, Boolean CREATE)
{
  CharPtr DBpath;
  Boolean res = FALSE;
 
  DBpath = AllocateAndCreateDBPath(path, "LL_CDD");
  res = OpenDBII(DBpath, &pD4LL_CDD, fieldInfo_CDD, tagInfo_CDD,
		  (InitFieldFunc) LL_Init_CDD_db_Fields, &cb, CREATE);  
  MemFree(DBpath);
  return res;
}

/* opens and sets the fields and tags of LL_LINK code base files */
static Boolean LL_OpenLLID_db(CharPtr path, Boolean CREATE)
{
  CharPtr DBpath;
  Boolean res = FALSE;

  DBpath = AllocateAndCreateDBPath(path, "LL_LLINK");
  res = OpenDBII(DBpath, &pD4LL_LLID, fieldInfo_LL, tagInfo_LL,
		  (InitFieldFunc) LL_Init_LLID_db_Fields, &cb, CREATE);  
  MemFree(DBpath);
  return res;
}

/* Function Called by the parser addgoid to reindex the table LL_GO
   after update is completed. */
Boolean ReindexLL_GO(void)
{
	INDEX4 *index;

	if (pD4LL_GO==NULL)
		return FALSE;
	index=i4open(pD4LL_GO,NULL);
	if (index==NULL)
		return FALSE;	
	if (d4reindex(pD4LL_GO)!=0)
		return FALSE;

      pT4LL_LL_GO = d4tag(pD4LL_GO, "LL_TAG_GO");
      pT4LL_GOID = d4tag(pD4LL_GO, "GOID_TAG");
      pT4LL_PMID = d4tag(pD4LL_GO, "PMID_TAG");
	return TRUE;
}
/* Function Called by the parser addgoid to open the table LL_GO
   without index for table update. */
Boolean OpenLL_GONoidx(CharPtr name)
{
   Char pathdatab[PATH_MAX];
   CharPtr path = NULL;

   pathdatab[0] = '\0';
   GetAppParam ( "intrez", "datab", "path", "", 
	         pathdatab, (size_t)PATH_MAX * (sizeof(char)) );
   path = AllocateAndCreateDBPath(pathdatab, name);
   pD4LL_GO = d4open(&cb, path);
   
   pF4LL_LL_GO = d4field(pD4LL_GO, "LL_GO");
   pF4LL_GOID = d4field(pD4LL_GO, "GO_ID");
   pF4LL_PMID = d4field(pD4LL_GO, "PMID");
   pF4LL_EVICODE = d4field(pD4LL_GO, "EVICODE");
   MemFree(path);
   return TRUE;
}
/* Function Called by the parser addgoid to close the table LL_GO
   after table update and indexing is completed. */
Boolean CloseLL_GO(void)
{
  if(pD4LL_GO)
  {
   if (d4close(pD4LL_GO) == r4success)
     return TRUE;
   else
     return FALSE;
  }
  return FALSE;
}


/********************************************************
             PUBLIC OPEN DATABASE FUNCTIONS
*********************************************************/

/* opens all the databases associated with Locus Link.  This
   function must be called prior to accessing the database.*/
Boolean LL_OpenAllDBs(Boolean CREATE)
{
  Char path[PATH_MAX]; 
  Boolean res1 = FALSE, res2 = FALSE, res3 = FALSE, res4 = FALSE;

  GetAppParam("intrez", "datab", "path", NULL, path, (size_t)PATH_MAX * (sizeof(Char))); 
  res1 = LL_OpenGO_db(path, CREATE);
  res2 = LL_OpenOMIM_db(path, CREATE);
  res3 = LL_OpenCDD_db(path, CREATE);
  res4 = LL_OpenLLID_db(path, CREATE);

  if(res1 && res2 && res3 && res4) return TRUE;
  else return FALSE;

}


/********************************************************
             PUBLIC CLOSE DATABASE FUNCTIONS
*********************************************************/
void CloseCodeBaseAndLLDBs(void)
{
        /* explicity flush all DBs */
	if(pD4LL_GO)   d4flush(pD4LL_GO);
	if(pD4LL_OMIM) d4flush(pD4LL_OMIM);
	if(pD4LL_CDD)  d4flush(pD4LL_CDD);
	if(pD4LL_LLID) d4flush(pD4LL_LLID);
        /* close CodeBase */
        code4close(&cb);
        code4initUndo(&cb);
}
                  
  
void CloseAllLLDBs(void)
{ 
	if(pD4LL_GO)   d4close(pD4LL_GO);
	if(pD4LL_OMIM) d4close(pD4LL_OMIM);
	if(pD4LL_CDD)  d4close(pD4LL_CDD);
	if(pD4LL_LLID) d4close(pD4LL_LLID);
}


/******************************************************************
		    PRIVATE HELPER FUNCTION
*******************************************************************/
/* retrieves a string stored in a memo field. It is the responsibility
   of the calling function to free the string. */
static CharPtr GetMemoField(FIELD4 *field)
{
   Int4 iSize = 0;
   CharPtr ptitle = NULL, title = NULL;
   CharPtr nospaces = NULL, retVal = NULL;

   iSize = f4memoLen(field);

   title = (CharPtr) f4memoStr(field);
   ptitle = StringSave(title);
   nospaces = TrimSpace(ptitle);
   iSize = StringLen(nospaces);
   retVal = (CharPtr) MemNew( (size_t)(iSize+1)*sizeof(Char) );
   StringCpy(retVal, nospaces);
   MemFree(ptitle);
   return retVal;
}

/*********************************************************************
                    PRIVATE SEARCH FUNCTIONS
   THESE FUNCTIONS ARE NOT INTENDED TO BY CALLED BY THE END USER.
    SEE QUERY.H/QUERY.C FOR THE PUBLIC SHOUND SEARCH FUNCTIONS
**********************************************************************/

/* searches the LL_GODB for a key specified in ppgo. If such a key is found, 
   fills in ppgo with the relevant data, and returns LL_SUCCESS. 
   If an error occurs, return LL_ERROR, if the key is not found returns LL_NOTFOUND.
*/
Int4 LL_SearchGODB(StGOdbPtr PNTR ppgo)
{
  StGOdbPtr pgo = NULL;
  Int4 key = 0;
  Int4 rc = 0;

  /* SK: NULL ptr check */
  if(ppgo == NULL || *ppgo == NULL)
  {
    ErrPostEx(SEV_ERROR,0,0, "LL_SearchGODB: StGOdbPtr is NULL.");
    return LL_ERROR;
  }

  pgo = *ppgo;
  if(pgo->llid != 0){
    key = pgo->llid;
    /* tag select the npid */ /* now the llid */
    d4tagSelect(pD4LL_GO, pT4LL_LL_GO);
  }
  if(pgo->goid > 0){
    key = pgo->goid;
    /* tag select the goid */
    d4tagSelect(pD4LL_GO, pT4LL_GOID);
  }
  if(pgo->pmid > 0){
    key = pgo->pmid;
    /* tag select the pmid */
    d4tagSelect(pD4LL_GO, pT4LL_PMID);
  }

  /* fill in the ppgo */
  d4top(pD4LL_GO);
  rc = d4seekDouble(pD4LL_GO, key);
  if (rc == r4success)
  {
    pgo->llid = f4int(pF4LL_LL_GO);
    pgo->goid = f4int(pF4LL_GOID);
    pgo->pmid = f4int(pF4LL_PMID);
    
    if(pgo->pmid == 1000) 
       { pgo->pmid = -1; }

    /*pgo->eviCode = GetMemoField(pF4LL_EVICODE);*/
    pgo->eviCode = Misc_TrimSpacesAroundString(StringSave(f4str(pF4LL_EVICODE)));
    for ( rc = d4seekNextDouble(pD4LL_GO, key); 
          rc == r4success; 
	  rc = d4seekNextDouble(pD4LL_GO, key) )
    {
      pgo->next = StGOdbNodeNew();
      pgo = pgo->next;
      pgo->llid = f4int(pF4LL_LL_GO);
      pgo->goid = f4int(pF4LL_GOID);
      pgo->pmid = f4int(pF4LL_PMID);
      if(pgo->pmid == 1000)
      {
	pgo->pmid = -1;
      }
      /*pgo->eviCode = GetMemoField(pF4LL_EVICODE);*/
      pgo->eviCode = Misc_TrimSpacesAroundString(StringSave(f4str(pF4LL_EVICODE)));
    }
    return LL_SUCCESS;
  }
  else{
    return LL_NOTFOUND;
  }
}

Boolean LIBCALL PrintLL_GO(void)
{
   int rc;
   Int4 gi = 0;
   Int4 goid = 0;
   Int4 pmid = 0;
   Int4 ginum = 0;


   for (rc = d4top(pD4LL_GO); rc == r4success; rc = d4skip(pD4LL_GO, (long) 1))
   {
      ginum++;
      gi = f4int(pF4LL_LL_GO);
      goid = f4int(pF4LL_GOID);
      pmid = f4int(pF4LL_PMID);

      printf("%ld\t%ld\t%ld\n", (long) gi, (long) goid, (long) pmid);
   }
   printf("Number of records: %ld\n", (long) ginum);
   return TRUE;
}

/* searches for the key stored in ppomim in the LL_OMIM db.
   If an error occurs, return LL_ERROR. If the key is not found in the db, 
   returns LL_NOTFOUND, otherwise, fills in ppomim with the
   necessary field data and returns LL_SUCCESS. */
Int4 LL_SearchOMIMDB ( StOMIMdbPtr PNTR ppomim )
{
  StOMIMdbPtr pomim = NULL;
  Int4 key = 0;
  Int4 rc = 0;

  /* SK: NULL ptr check */
  if(ppomim == NULL || *ppomim == NULL)
  {
    ErrPostEx(SEV_ERROR,0,0, "LL_SearchOMIMDB: StOMIMdbPtr is NULL.");
    return LL_ERROR;
  }

  pomim = *ppomim;
  if(pomim->llid != 0){
    key = pomim->llid;
    /* tag select the npid */
    d4tagSelect(pD4LL_OMIM, pT4LL_LL_OMIM);
  }
  if(pomim->omimid > 0){
    key = pomim->omimid;
    /* tag select the goid */
    d4tagSelect(pD4LL_OMIM, pT4LL_OMIM);
  }

  /* fill in the ppomim */
  d4top(pD4LL_OMIM);
  rc = d4seekDouble(pD4LL_OMIM, key);
  if (rc == r4success)
  {
    pomim->llid = f4int(pF4LL_LL_OMIM);
    pomim->omimid = f4int(pF4LL_OMIM);
    for ( rc = d4seekNextDouble(pD4LL_OMIM, key); 
	  rc == r4success; 
	  rc = d4seekNextDouble(pD4LL_OMIM, key) )
    {
      pomim->next = StOMIMdbNodeNew();
      pomim = pomim->next;
      pomim->llid = f4int(pF4LL_LL_OMIM);
      pomim->omimid = f4int(pF4LL_OMIM);
    }
    return LL_SUCCESS;
  }
  else{
    return LL_NOTFOUND;
  } 
}

/* searches for the key stored in ppomim in the LL_OMIM db. 
   If an error occurs, return LL_ERROR. If the key is not found in the db, 
   returns LL_NOTFOUND, otherwise, fills in ppomim with the
   necessary field data and returns LL_SUCCESS. */
Int4 LL_SearchCDDDB(StCDDdbPtr PNTR ppcdd)
{
  StCDDdbPtr pcdd = NULL;
  CharPtr key = NULL;
  Int4 rc = 0;
  FloatHi eVal = 0;

  /* SK: NULL ptr check */
  if(ppcdd == NULL || *ppcdd == NULL)
  {
    ErrPostEx(SEV_ERROR,0,0, "LL_SearchCDDDB: StCDDdbPtr is NULL.");
    return LL_ERROR;
  }

  pcdd = *ppcdd;
  if(pcdd->llid != 0)
  {
    key = (CharPtr)MemNew((NP_ID_SIZE+1)*sizeof(Char));
    sprintf(key, "%d", pcdd->llid);
    /* tag select the npid */ 
    d4tagSelect(pD4LL_CDD, pT4LL_LL_CDD);
  }
  else if(pcdd->cddid != NULL)
  {
    key = (CharPtr) MemNew((StringLen(pcdd->cddid)+1)*sizeof(Char));
    sprintf(key, "%s", IsNullStr(pcdd->cddid));
    /* tag select the goid */ 
    d4tagSelect(pD4LL_CDD, pT4LL_CDD);
  }

  /* fill in the ppomim */
  d4top(pD4LL_CDD);
  rc = d4seek(pD4LL_CDD, key);
  if (rc == r4success){
    pcdd->llid = f4int(pF4LL_LL_CDD);
    /*pcdd->cddid = GetMemoField(pF4LL_CDD);*/
    pcdd->cddid = Misc_TrimSpacesAroundString(StringSave(f4str(pF4LL_CDD)));
    eVal = f4double(pF4LL_E_VAL);
    if(eVal < 7){
      SLRI_SafeExp10(&eVal);
      pcdd->eValue = eVal;
    } else {
      pcdd->eValue = -1;
    }
    for(rc = d4seekNext(pD4LL_CDD, key); rc == r4success; rc = d4seekNext(pD4LL_CDD, key)){
      pcdd->next = StCDDdbNodeNew();
      pcdd = pcdd->next;
      pcdd->llid = f4int(pF4LL_LL_CDD);
      /*pcdd->cddid = GetMemoField(pF4LL_CDD);*/
      pcdd->cddid = Misc_TrimSpacesAroundString(StringSave(f4str(pF4LL_CDD)));
      eVal = f4double(pF4LL_E_VAL);
      if(eVal < 7){
	SLRI_SafeExp10(&eVal);
	pcdd->eValue = eVal;
      }else {
	pcdd->eValue = -1;
      }
    }
    MemFree(key);
    return LL_SUCCESS;
  }
  else{
    MemFree(key);
    return LL_NOTFOUND;
  } 
}

/* searches for the key stored in ppll in the LL_LINK db. 
   If an error occurs, return LL_ERROR. If the key is not found in the db, 
   returns LL_NOTFOUND, otherwise, fills in ppll with the
   necessary field data and returns LL_SUCCESS. */
Int4 LL_SearchLLDB(StLLdbPtr PNTR ppll)
{
  StLLdbPtr pll = NULL;
  Int4 key = 0;
  Int4 rc = 0;

  /* SK: NULL ptr check */
  if(ppll == NULL || *ppll == NULL)
  {
    ErrPostEx(SEV_ERROR,0,0, "LL_SearchLLDB: StLLdbPtr is NULL.");
    return LL_ERROR;
  }

  pll = *ppll;
  if(pll->llid != 0)
  {
    key = pll->llid;
    /* tag select the npid */
    d4tagSelect(pD4LL_LLID, pT4LL_LL_LLID);
  }
  else if(pll->npid > 0){
    key = pll->npid;
    /* tag select the goid */
    d4tagSelect(pD4LL_LLID, pT4LL_GI);
  }

  /* fill in the ppomim */
  d4top(pD4LL_LLID);
  rc = d4seekDouble(pD4LL_LLID, key);
  if (rc == r4success)
  {
    pll->llid = f4int(pF4LL_LL_LLID);
    pll->npid = f4int(pF4LL_GI);
    pll->map = GetMemoField(pF4LL_MAP);
    for ( rc = d4seekNextDouble(pD4LL_LLID, key); 
          rc == r4success; 
	  rc = d4seekNextDouble(pD4LL_LLID, key) )
    {
      pll->next = StLLdbNodeNew();
      pll = pll->next;
      pll->llid = f4int(pF4LL_LL_LLID);
      pll->npid = f4int(pF4LL_GI);
      pll->map = GetMemoField(pF4LL_MAP);
    }
    return LL_SUCCESS;
  }
  else{
    return LL_NOTFOUND;
  } 

}


/**************************************************************
                 PRIVATE APPENDING FUNCTIONS			       
***************************************************************/
/* Adds a entry consisting of npid, goID, PMID and eviCode to
   LL_GO and then returns LL_APPEND_SUCCESS. */
Int4 LL_Append2GO_DB(Int4 llid, Int4 goID, Int4 PMID, CharPtr eviCode)
{
  /* if(llid < 0){
    ErrPostEx(SEV_ERROR, 0, 0, "LL_Append2GO: Invalid NP id on record with PMID: %d.\nRecord skipped.", PMID);
    return LL_ERROR;
  } */
  if(goID <= 0){
    ErrPostEx(SEV_ERROR, 0, 0, "LL_Append2GO: Invalid go id on record with NPID: %d.\nRecord skipped.", llid);
    return LL_ERROR;
  }
  /*if(PMID <= 0){
    ErrPostEx(SEV_ERROR, 0, 0, "LL_Append2GO: Invalid pmid on record with NPID: %d.\nRecord skipped.", llid);
    return LL_ERROR;
  }*/
  if(eviCode == NULL){
    ErrPostEx(SEV_ERROR, 0, 0, "LL_Append2GO: Invalid E Code on record with NPID: %d.\nRecord skipped.", llid);
    return LL_ERROR;
  }
/* March 16, 2004. This function is not necessary and cause run time error when a table is opened without
   indexing. It is commented out. Other d4tagSelect functions in the file are removed for the same reason. */
/*  d4tagSelect(pD4LL_GO, pT4LL_NP_GO); */

  d4appendStart(pD4LL_GO, 0);
  f4assignInt(pF4LL_LL_GO, llid);
  f4assignInt(pF4LL_GOID, goID);
  f4assignInt(pF4LL_PMID, PMID);
  f4assign(pF4LL_EVICODE, eviCode);

  d4append(pD4LL_GO);
  /* printf("Inside LL_GO: %d %d %d %s\n", llid, goID, PMID, IsNullStr(eviCode)); */
  return LL_APPEND_SUCCESS;
}

/* adds an entry consisting of NPid and OMIM to the LL_OMIM DB and 
   LL_APPEND_SUCCESS. */
Int4 LL_Append2OMIM_DB(Int4 llid, Int4 OMIM)
{
  if(llid < 0){
    ErrPostEx(SEV_ERROR, 0, 0, "LL_Append2OMIM: Invalid npid on record with OMIM: %d.\nRecord skipped.", OMIM);
    return LL_ERROR;
  }
  if(OMIM <= 0){
    ErrPostEx(SEV_ERROR, 0, 0, "LL_Append2OMIM: Invalid omim on record with NP id: %d.\nRecord skipped.", llid);
    return LL_ERROR;
  }
  /* d4tagSelect(pD4LL_OMIM, pT4LL_NP_OMIM); */
  if ( llid == 0 )
	  printf ( "Zero GI on OMIM: %d\n", OMIM );

  d4appendStart(pD4LL_OMIM, 0);
  f4assignInt(pF4LL_LL_OMIM, llid);
  f4assignInt(pF4LL_OMIM, OMIM);
  
  d4append(pD4LL_OMIM);
  return LL_APPEND_SUCCESS;
}

/* adds an entry consisting of NPid, CDD, evalue to the LL_CDD DB 
   and returns LL_APPEND_SUCCESS */
Int4 LL_Append2CDD_DB(Int4 llid, CharPtr CDD, FloatHi evalue)
{
  if(llid < 0){
    ErrPostEx ( SEV_ERROR, 0, 0, 
                "LL_Append2CDD: Invalid npid on record with CDD: %s.\nRecord skipped.", 
		IsNullStr(CDD) );
    return LL_ERROR;
  }
  if(CDD == NULL){
    ErrPostEx ( SEV_ERROR, 0, 0, 
		"LL_Append2CDD: Invalid cdd id on record with NPID: %d.\nRecord skipped.", 
		llid );
    return LL_ERROR;
  }
  /* if(evalue <= 0){
    ErrPostEx(SEV_ERROR, 0, 0, "LL_Append2CDD: Invalid evalue on record with NPID: %d.\nRecord skipped.", llid);
    return LL_ERROR;
    }*/
  /* d4tagSelect(pD4LL_CDD, pT4LL_NP_CDD); */

  d4appendStart(pD4LL_CDD, 0);
  f4assignInt(pF4LL_LL_CDD, llid);
  f4assign(pF4LL_CDD, CDD);
  if(evalue > 0){
    SLRI_SafeLog10(&evalue);
  }
  f4assignDouble(pF4LL_E_VAL, evalue);  
  d4append(pD4LL_CDD);
  return LL_APPEND_SUCCESS;
}

/* adds an entry consisting of llid, npid, pMAP and then 
   returns GO_APPEND_SUCCESS */
Int4 LL_Append2LLID_DB(Int4 llid, Int4 npid, CharPtr pMAP)
{
  if ( npid <= 0 )
  {
    ErrPostEx ( SEV_ERROR, 0, 0, "[LL_Append2LLID]: Invalid %s: %d.\nRecord skipped.", 
	        npid <= 0 ? "npid" : "llid", npid < 0 ? npid : llid );
    return LL_ERROR;
  }

  /* if(pMAP == NULL){
    ErrPostEx(SEV_ERROR, 0, 0, "LL_Append2LLID: Invalid map on record with LLID: %d.\nRecord skipped.", llid);
    return LL_ERROR;
  } */
  /* d4tagSelect(pD4LL_LLID, pT4LL_LLID); */

  d4appendStart(pD4LL_LLID, 0);
  f4assignInt(pF4LL_LL_LLID, llid);
  f4assignInt(pF4LL_GI, npid);
  f4memoAssign(pF4LL_MAP, pMAP);
  d4append(pD4LL_LLID);
  /* printf("Inside LL_ID: %d %d %s\n", llid, npid, IsNullStr(pMAP)); */
  return LL_APPEND_SUCCESS;
}


/****************************************************************
                       PUBLIC APPEND FUNCTION
*****************************************************************/
/* appends a record stored in ps to the 4 data bases LL_GO, LL_CDD,
   LL_OMIM, LL_LLINK. This program does not check for duplicates. If
   you are reparsing the locus link database, delete the old files 
   before reparsing, otherwise you will have doubles of all entries.
   Returns -1 if a record is skipped (those that don't have gi). Otherwise 
   returns 0.
*/
Int4 LL_AppendRecord(LL_ParseStructPtr ps)
{
  ValNodePtr vnp1 = NULL,
             vnp2 = NULL,
             vnp3 = NULL;
  LL_CDDStructPtr cdd_tmp;
  
  /* adding a name and go id entry for each go id */
  /* SK: NULL ptr check */
  if(ps == NULL /* || ps->cdd_st == NULL */){
    ErrPostEx(SEV_INFO, 0, 0, "LL_AppendRecord: No NP id. Record skipped.");
    /* printf("Record skipped\n"); */
    return -1;
  }
  if ( NULL == ps->cdd_st )
  {
     ps->cdd_st = MemNew ( sizeof ( LL_CDDStruct ) );
     ps->cdd_st->iNPid = 0;
  }
  
  /* adding to the GO DB */
  if ( ps->vnpGOID != NULL )
  {
     for ( vnp1 = ps->vnpGOID, vnp2 = ps->vnpPMID, vnp3 = ps->vnpE_CODE; 
           vnp1 != NULL; 
	   vnp1 = vnp1->next, vnp2 = vnp2->next, vnp3 = vnp3->next )
     {
        LL_Append2GO_DB ( ps->iLLID, vnp1->data.intvalue, 
			  vnp2->data.intvalue, vnp3->data.ptrvalue );
     }
  }
  
  if ( ps->vnpOMIM != NULL )
  {
     for ( vnp1 = ps->vnpOMIM; vnp1 != NULL; vnp1 = vnp1->next )
     {
        LL_Append2OMIM_DB ( ps->iLLID, vnp1->data.intvalue );
     }
  }
  
  
  for ( cdd_tmp = ps->cdd_st; 
	cdd_tmp != NULL ; 
	cdd_tmp = cdd_tmp->next )
  {
     if ( ps->cpMAP == NULL )
        LL_Append2LLID_DB ( ps->iLLID, cdd_tmp->iNPid, "NULL" );
     else
        LL_Append2LLID_DB ( ps->iLLID, cdd_tmp->iNPid, ps->cpMAP );
     
     for ( vnp1 = cdd_tmp->vnpCDD_ID, vnp2 = cdd_tmp->vnpE_VAL;
	   vnp1 != NULL;
	   vnp1 = vnp1->next, vnp2 = vnp2->next )
     {
        LL_Append2CDD_DB ( ps->iLLID, vnp1->data.ptrvalue, vnp2->data.realvalue );
     }
  } 
  
  return 0;
}


/*
$Log: ll_cb.c,v $
Revision 1.12  2004/06/16 15:05:53  hlieu
add boolean to LL_InitCodeBase

Revision 1.11  2004/05/17 19:56:52  hlieu
fix cross comp error

Revision 1.10  2004/05/17 19:07:28  hlieu
fix for negative locus links

Revision 1.9  2004/05/17 18:04:20  hlieu
fix cb layer for negative locus id

Revision 1.8  2004/05/13 17:38:19  hlieu
major refactor of locus link module. primary key changed from gi to locuslink
id. Also required update of objXXX source code to reflect the change in
ASN structure

Revision 1.7  2004/04/16 18:11:46  vgu
Rollback changes in version 1.6

Revision 1.6  2004/04/14 14:14:54  vgu
Added Reindexing Functions to locus link tables and allow open tables without indexing

Revision 1.5  2004/03/17 20:07:04  eburgess
Victor: Added OpenLL_GONoidx(), ReindexLL_GO() and CloseLL_GO() to increase
speed.

Revision 1.4  2004/01/06 22:32:16  skuznets
Code audit has been done sucessfully.
Possible NULL pointer issues has been eliminated.

Revision 1.3  2003/11/03 15:50:27  haocl
changed order of includes to avoid _LARGE_FILE_SOURCE redefinition warnings

Revision 1.2  2003/10/07 19:08:07  kaca
removed double quotes

Revision 1.1.1.3  2003/08/02 08:42:13  root
Automated import from Sourceforge.net

Revision 1.3  2003/07/30 14:14:07  haocl
changed evidence code to string from memo field.

Revision 1.2  2003/06/25 14:11:54  haocl
modifications to allow yeast GO annotation access

Revision 1.1  2001/12/19 23:59:52  kaca
renamed to support database layer naming convention

Revision 1.5  2001/12/11 19:04:57  kaca
added Id keyword

Revision 1.4  2001/12/11 19:02:57  kaca
added Log keyword

*/
