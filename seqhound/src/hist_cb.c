/* $Id: hist_cb.c,v 1.8 2004/07/22 20:54:17 zwang Exp $
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
  Part of SeqHound

****************************************************************************/
#include <intrez_db.h>
#include <ncbi.h>
#include <hist_db_p.h>
#include <hist_db.h>
#include <slri_cb.h>
#include <slri_misc.h>
#include <objslristruc.h>
#include <intrez_cb.h>
#include <d4all.h>     /* move to end to avoid _LARGE_FILE_SOURCE redefinition warnings */

/**************************************************
 *       PUBLIC CODE BASE INITIALIZTION           *
 **************************************************/

/* these functions initialize the codebase tags and field
   information.  Alter these only if you know what you are doing */

/* initializes the CODE4 structure and sets various flags.
   This function must be called in any application that uses 
   this database, prior to doing anything to the database.*/
void HIST_InitCodeBase(Boolean Create)
{
  code4init(&cb);
  cb.errOpen = 0;
  cb.safety = 0;
  code4largeOn(&cb);
}

/* closes the databases.  This function must called at the
   end of your function, if you called LL_InitCodeBase.*/
void HIST_InitUndoCodeBase(void)
{
  code4close(&cb);
  code4initUndo(&cb);
}

/**********************************************************
       PRIVATE CODE BASE INITIALIZATION FUNCTIONS
***********************************************************/

/* initializes the fields and tags of the file
   of the HISTDB file */
static void Init_HIST_Fields (void)
{
  pF4H_gi = d4field(pD4hist, "HIST_GI");
  pF4H_accession = d4field(pD4hist, "HIST_ACC");
  pF4H_version = d4field(pD4hist, "HIST_VER");
  pF4H_date = d4field(pD4hist, "HIST_DATE");
  pF4H_action = d4field(pD4hist, "HIST_ACT");

  pT4H_gi = d4tag(pD4hist, "TAG_GI");
  pT4H_acc = d4tag(pD4hist, "TAG_ACC");
  pT4H_date = d4tag(pD4hist, "TAG_DATE");
  pT4H_action = d4tag(pD4hist, "TAG_ACT");
}

/* initializes the fields and tags of the file
   of the ACCDB file */
/*static void Init_ACCDB_Fields(void)
{
   pf4acGi = d4field(pd4Accdb,"GI");
   pf4acDb = d4field(pd4Accdb, "DB");
   pf4acName = d4field(pd4Accdb, "NAME");
   pf4acAccess = d4field(pd4Accdb, "ACCESS");
   pf4acChain = d4field(pd4Accdb, "CHAIN");
   pf4acRelease = d4field(pd4Accdb, "RELEASE");
   pf4acVersion = d4field(pd4Accdb, "VERSION");
   pf4acTitle = d4field(pd4Accdb, "TITLE");
   pf4acNamelow = d4field(pd4Accdb, "NAMELOW");

   tagacGi = d4tag(pd4Accdb, "GI");
   tagacAcc = d4tag(pd4Accdb, "ACCESS");
   tagacName = d4tag(pd4Accdb, "NAME");
   tagacNamelow = d4tag(pd4Accdb, "NAMELOW");
}*/

/***************************************************
           PRIVATE OPEN DATABASE FUNCTIONS
****************************************************/
/* opens and sets the fields and tags of HISTDB code base files */
static Boolean OpenHISTdb(CharPtr path, Boolean CREATE)
{
  CharPtr DBpath;
  Boolean res = FALSE;


  DBpath = AllocateAndCreateDBPath(path, "HISTDB");
  res = OpenDBII(DBpath, &pD4hist, fieldInfo_HIST, tagInfo_HIST,
		  (InitFieldFunc) Init_HIST_Fields, &cb, CREATE);
  MemFree(DBpath);
  return res;
}

/* opens and sets the fields and tags of ACCDB code base files */
/*static Boolean Open_ACCDB(CharPtr path, Boolean CREATE)
{
  CharPtr DBpath;
  Boolean res = FALSE;


  DBpath = AllocateAndCreateDBPath(path, "ACCDB");
  res = OpenDBII(DBpath, &pd4Accdb, f4iAccdb, t4iAccdb,
		  (InitFieldFunc) Init_ACCDB_Fields, &cb, CREATE);
  MemFree(DBpath);
  return res;
}*/


/********************************************************
             PUBLIC OPEN DATABASE FUNCTIONS
*********************************************************/

/* opens all the databases associated with Histdb.  This
   function must be called prior to accessing the database. */
Boolean HIST_OpenAllDBs(Boolean CREATE)
{
  Char path[PATH_MAX] = {0};
  Boolean res1 = FALSE;
  Boolean res2 = FALSE;
  GetAppParam("intrez", "datab", "path", NULL, path, (size_t)PATH_MAX * (sizeof(Char)));
  res1 = OpenHISTdb(path, CREATE);
  if(res1 == FALSE)
     fprintf(stderr, "HISTDB false\n");
  res2 = OpenACCDB("ACCDB", FALSE);
  if(res2 == FALSE)
     fprintf(stderr, "ACCB false\n");

  if(res1 && res2) return TRUE;
  else return FALSE;
}

Boolean OpenHISTDB(Boolean CREATE)
{
  Char path[PATH_MAX] = {0};
  GetAppParam("intrez", "datab", "path", NULL, path, (size_t)PATH_MAX * (sizeof(Char)));
  if(OpenHISTdb(path, CREATE)) return TRUE;
  else return FALSE;
}


/********************************************************
             PUBLIC CLOSE DATABASE FUNCTIONS
*********************************************************/
void CloseCodeBaseAndHISTDBs(void)
{
  /* explicity flush all DBs */
  if(pD4hist)   d4flush(pD4hist);
  /* close CodeBase */
  code4close(&cb);
  code4initUndo(&cb);
}
                  
void CloseAllHISTDBs(void)
{
  if(pD4hist)   d4close(pD4hist);
}

/* parses from ACCDB and extracts the GI, accesion & version
   and fills the HISTDB.
*/
Boolean LIBCALL Extract_ACCDB(void)
{
  int rc;
  Int4 ginum = 0;
  StHistdbNodePtr pHis = NULL;
  NCBI_DatePtr pDate = NULL;
  Int4 recnum_histdb = 0;
  Int4 recnum_accdb = 0;
  Int4 target = 1;

  /* count the number of records in HISTDB */
  if((recnum_histdb = d4recCount(pD4hist)) >=1)
  {
    /* HISTDB has more records than ACCDB, so upto date */
    if(recnum_histdb > (recnum_accdb = d4recCount(pd4Accdb)))
    {
       ErrPostEx(SEV_ERROR,0,0, "Number of records in ACCDB is lower than in HISTDB.");
       return FALSE;
    } /* upto date */
    else if (recnum_histdb == recnum_accdb)
    {
       ErrPostEx(SEV_ERROR,0,0, "Nothing to do. Number of records in ACCDB is the same as in HISTDB.");
       return FALSE;
    }
    else
       target = recnum_histdb+1;
  } 
  /* adding to HISTDB new records */
  pHis = StHistdbNodeNew();
  /* get the date from the system using an NCBI function 
     and then convert it to a string format YYYMMDD */
  pDate = DateCurr();
  pHis->date = GetStringDate(pDate);

  /* adding new records, so action is ADDED */
  pHis->action = ACTION_ADDED;
  /* start at where we last left off, if new, then target = 1, 1st record. 
   * else, target is the last record in ACCDB + 1, the new record to be added. */
  for (rc = d4go(pd4Accdb, target); rc == r4success; rc = d4skip(pd4Accdb, (long) 1))
  {
    /* grab the accession, version and gi from ACCDB
     * to save into HISTDB. */
    ginum++;  
    pHis->gi = f4long(pf4acGi);           
    pHis->version = f4int(pf4acVersion);
    pHis->accession = f4str(pf4acAccess);
    AppendRecordHISTDB(pHis);
  }
  DateFree(pDate);
  StHistdbNodeFree(pHis);
  return TRUE;
}

/* converts a string of format YYYYMMDD into a NCIB_DatePtr object.
   The calling function must free the object.
*/
NCBI_DatePtr LIBCALL RetrieveDate(CharPtr date){
  char year[5] = {0};
  char month[3] = {0};
  char day[3] = {0};
  char *p = date;
  NCBI_DatePtr pdate = DateNew();

  strncpy(year, date, 4 * sizeof(char)); p+= 4;
  strncpy(month, p, 2 * sizeof(char)); p += 2;
  strncpy(day, p, 2 * sizeof(char)); p += 2;
  DateWrite(pdate, atoi(year), atoi(month), atoi(day), NULL);
  return pdate;
}

/* adds a record to HISTDB */
Boolean LIBCALL AppendRecordHISTDB(StHistdbNodePtr pHis)
{
  /* SK: NULL ptr check */
  if ( pHis == NULL )
  {
    ErrPostEx(SEV_ERROR, 0, 0, "[AppendRecordHISTDB]: Null StHistdbNodePtr.");
    return FALSE;
  }

  if(strstr(pHis->accession, "n/a") != NULL)
    return TRUE;

  d4appendStart(pD4hist, 0);
  
  if(pHis->gi) f4assignLong(pF4H_gi, (long) pHis->gi);
  if(pHis->accession) f4assign(pF4H_accession, pHis->accession);
  if(pHis->version) f4assignInt(pF4H_version, pHis->version);
  f4assign(pF4H_date, pHis->date);
  f4assignInt(pF4H_action, pHis->action);
  d4append(pD4hist);
  return TRUE;
}

/* searches for all records with the action
   between (inclusive) the 2 dates.
   date_from must preceed date_to. Caller has to free both dates.
*/
StHistdbNodePtr GetAll(Int4 ACTION, NCBI_DatePtr date_from, NCBI_DatePtr date_to)
{
  Int2 i;
  StHistdbNodePtr pHis = NULL;
  StHistdbNodePtr pPrev = NULL, pCur = NULL;
  CharPtr key = (CharPtr) MemNew(25*sizeof(Char));
  Int4 rc = 0;
  NCBI_DatePtr pdate = NULL;
  if(date_from == NULL)
  {
    date_from = DateCurr();
  }
  if(date_to == NULL)
  {
    date_to = DateCurr();
  }
  i = DateMatch(date_from, date_to, FALSE);
  if( i == -1 || i == 0 )
  {
    /* date in correct order */
    d4tagSelect(pD4hist, pT4H_action);

    /* search for those with correct action and then match the dates */
    d4top(pD4hist);
    for(rc = d4seekDouble(pD4hist, ACTION); rc == r4success; 
	rc = d4seekNextDouble(pD4hist, ACTION))
    {
      /* get the record's date and compare with date_from */
      pdate = RetrieveDate(f4str(pF4H_date));
      i = DateMatch(date_from, pdate, FALSE);
      if( i == -1 || i == 0)
      {
	/* the date_from preceeds or is the same date as the records
	   so compare with date_to */
	i = DateMatch(pdate, date_to, FALSE);
	if( i == -1 || i == 0)
        {
	  /* the record date preceeds date_to
	     so the record is what we are looking for */
	  pCur = StHistdbNodeNew();
	  pCur->accession = (CharPtr) MemNew(25*sizeof(Char));
	  pCur->gi = f4long(pF4H_gi);
	  strcpy(pCur->accession, f4str(pF4H_accession));
	  pCur->version = f4int(pF4H_version);
	  DatePrint(pdate, key);
	  pCur->date = (CharPtr) MemNew(25*sizeof(Char));
	  strcpy(pCur->date, key);
	  if(pHis == NULL) pHis = pCur;
	  else pPrev->next = pCur;
	  pPrev = pCur;
	}
      }
    }
  }
  DateFree(pdate);
  MemFree(key);
  return pHis;
}

void LIBCALL PrintStHist(StHistdbNodePtr pHis){
  StHistdbNodePtr pCur = NULL;
  for(pCur = pHis; pCur != NULL; pCur = pCur->next){
    printf("%d\t%s\t%d\t%s\n", pCur->gi, IsNullStr(pCur->accession), pCur->version, IsNullStr(pCur->date));
  }
}

Boolean LIBCALL SearchHISTDB(StHistdbNodePtr pHis)
{
   Int4 rc;
   StHistdbNodePtr pTmp;
   if(pHis == NULL)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[SearchHISTDB]: Null StHistdbNodePtr.");
      goto end;
   }
   if(pHis->gi < 1)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[SearchHISTDB]: Invalid keys.");
      goto end;
   }
   /* if(!OpenHISTDB(FALSE))
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[SearchHISTDB]: Could not open HISTDB.");
      goto end;
   } */
   
   d4tagSelect(pD4hist, pT4H_gi);

   for(rc = d4seekDouble(pD4hist, pHis->gi), pTmp = pHis; rc == r4success; rc = d4seekNextDouble(pD4hist, pHis->gi))
   {
      if(strstr(f4str(pF4H_accession), "n/a") != NULL) continue;
      pTmp->accession = (char *)malloc((strlen(f4str(pF4H_accession))+1) * sizeof(char));
      strcpy(pTmp->accession, f4str(pF4H_accession));
      pTmp->gi = f4int(pF4H_gi);
      pTmp->version = f4int(pF4H_version);
      pTmp->action = f4int(pF4H_action);
      pTmp->date = (char*) malloc((strlen(f4str(pF4H_date))+1) * sizeof(char));
      strcpy(pTmp->date, f4str(pF4H_date));
      /* printf("%s %d %d %d %s\n", IsNullStr(pHis->accession), pHis->gi, pHis->version, pHis->action, IsNullStr(pHis->date)); */
      return TRUE;
   }
   /* CloseAllHISTDBs(); */ 
   return FALSE; 
end:
   /* CloseAllHISTDBs(); */
   return FALSE;
}


/*
$Log: hist_cb.c,v $
Revision 1.8  2004/07/22 20:54:17  zwang
Change function argument according to hist_odbc.c

Revision 1.7  2004/01/06 22:32:15  skuznets
Code audit has been done sucessfully.
Possible NULL pointer issues has been eliminated.

Revision 1.6  2003/11/27 20:56:11  idonalds
corrected implicit function declaration error

Revision 1.5  2003/11/03 15:50:27  haocl
changed order of includes to avoid _LARGE_FILE_SOURCE redefinition warnings

Revision 1.4  2003/10/02 21:45:26  volodya
OpenHIST, CloseHISTDB now in SHoundInit and SHoundFini

Revision 1.3  2003/08/25 15:13:24  haocl
changed Append so that accessions of n/a do not get inserted

Revision 1.2  2003/08/22 13:35:10  haocl
added SHoundGetAccFromRetiredGi and SearchHISTDB

Revision 1.1.1.5  2002/08/20 08:29:54  root
Automated import from Sourceforge.net

Revision 1.10  2002/08/19 20:56:04  kaca
fixed record positioning in Extract_ACCDB yet again

Revision 1.7  2002/08/16 17:07:26  haocl
added proper memory freeing

Revision 1.6  2002/08/15 16:47:54  haocl
removed double slash comments

Revision 1.5  2002/08/15 15:33:58  haocl
moved GetStringDate to slri_misc

Revision 1.3  2002/08/12 18:23:08  haocl
changed boolean to prevent accidental deletion of ACCDB

Revision 1.2  2002/08/08 22:17:44  kaca
removed ACCDB initialization code, added OpenHISTDB

Revision 1.1  2002/08/08 18:59:43  haocl
initial import of hist

*/
