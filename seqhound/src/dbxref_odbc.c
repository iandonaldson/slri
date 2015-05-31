/* $Id: dbxref_odbc.c,v 1.8 2004/08/06 14:47:21 mmatan Exp $
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
  SeqHound DBXref Module
 

*******************************************************************************/


#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>

#include <dbxref_odbc.h>
#include <objslristruc.h>
#include <slri_odbc.h>
#include <slri_misc.h>



extern SQLHDBC hdbc;
extern SQLHENV henv;


/* DBXREF fields in SQLBindParameter*/
struct
{
   SQLINTEGER len;
   CharPtr val;
}dbxref_asn;


/* SeqHound Global Variables */
extern SQLCHAR database[50];


/**************************************************************/
/*Given a pointer to the asn structure that represents        */
/*  a dbxref record, this function search the DBXREF database */
/*  for the record specified by ppdbxref.                     */
/*  The search key is in ppdbxref and must be set by the user */
/*  prior to calling SearchDBXREF.                            */
/*  Returns all records associated with that key in a link    */
/*  list pointed by ppdbxref.                                 */
/*  Arg: StDbXrefPtr a pointer to dbxref structure.           */
/*       Combining elementes of StDbXref                      */
/*       When Source_db != NULL and parentid = -1             */
/*            will retrieve all Source Record for that        */
/*            Sourcedb       .                                */
/*       When Source_db != NULL and parentid = -2             */
/*            will retrieve all Record_id for that Sourcedb   */
/*  Returns : TRUE if successful and FALSE otherwise.         */
/**************************************************************/
Int2 LIBCALL SearchDBXREF (StDbXrefPtr PNTR ppdbxref)
{
   StDbXrefPtr pdbxref = NULL;           /* linked list to hold return values */

   Int4 iLength = 0;                     /* search key and length of key      */
   SQLINTEGER key = 0;
   CharPtr pKey = NULL;                  /* search key for strings            */

   Char *pSQL_stmt;                      /* SQL select statement              */
   SQLHANDLE hstmt;                      /* handle to select statement        */ 
   SQLRETURN retcode;                    /* return values for odbc layer      */
   Boolean flagSourceDB = FALSE;         /* Store if source_db or source_id is requested */

   /* SK: NULL ptr check */
   if (ppdbxref == NULL || *ppdbxref == NULL)
   {
       ErrPostEx(SEV_ERROR,0,0, "[SearchDBXREF]: SHdbXrefPtr is NULL.");
       return SH_ERROR;
   }
   pdbxref = *ppdbxref;
   
   /* allocate space for the select statement */
   pSQL_stmt = (char*) malloc(sizeof(char) * 200);
   if(pSQL_stmt == NULL)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[SearchDBXREF]: Could not allocate space");
      return SH_ERROR;
   }

   /* fill in the select statement based on the search key passed in.
      database is a global variable initialized in ConnectToDb. The
      value of database is set in the ini file. eg database = seqhound. */
  
   if(pdbxref->id > 0)
   {
       key = pdbxref->id;
       sprintf((char*)pSQL_stmt, "select id, source_id, record_id, parent_id, link, field, cv from %s.dbxref where id=?;", database);
   }
   else if(pdbxref->recordid != NULL)
   {
       pKey = pdbxref->recordid;
       iLength = strlen(pdbxref->recordid);
       sprintf((char*)pSQL_stmt, "select id, source_id, record_id, parent_id, link, field, cv from %s.dbxref where record_id=?;", database);
   }
   else if(pdbxref->sourceid > 0 && pdbxref->parentid < 0)
   {
       key = pdbxref->sourceid;
       if (pdbxref->parentid == -1)
	   sprintf((char*)pSQL_stmt, "select id, source_id, record_id, parent_id, link, field, cv from %s.dbxref where source_id=? and parent_id = 0;", database);
       else if (pdbxref->parentid == -2)
	   sprintf((char*)pSQL_stmt, "select id, source_id, record_id, parent_id, link, field, cv from %s.dbxref where source_id=? and parent_id > 0;", database);
   }
   else if(pdbxref->sourceid > 0)
   {
       key = pdbxref->sourceid;
       sprintf((char*)pSQL_stmt, "select id, source_id, record_id, parent_id, link, field, cv from %s.dbxref where source_id=?;", database);
   }
   else if(pdbxref->sourcedb != NULL && pdbxref->parentid < 0)
   {
       pKey = pdbxref->sourcedb;
       iLength = strlen(pdbxref->sourcedb);
       if (pdbxref->parentid == -1)
       {
	   sprintf((char*)pSQL_stmt, "select a.id, b.source_db, a.record_id, a.parent_id, a.link, a.field, a.cv from %s.dbxref a, %s.dbxrefsourcedb b where a.source_id=b.source_id and b.source_db=? and a.parent_id = 0;", database, database);
       }
       else if (pdbxref->parentid == -2)
       {
	   sprintf((char*)pSQL_stmt, "select a.id, b.source_db, a.record_id, a.parent_id, a.link, a.field, a.cv from %s.dbxref a, %s.dbxrefsourcedb b where a.source_id=b.source_id and b.source_db=? and a.parent_id > 0;", database, database);
       }
       flagSourceDB = TRUE;
   }
   else if(pdbxref->sourcedb != NULL)
   { 
       pKey = pdbxref->sourcedb;
       iLength = strlen(pdbxref->sourcedb);
       sprintf((char*)pSQL_stmt, "select a.id, b.source_db, a.record_id, a.parent_id, a.link, a.field, a.cv from %s.dbxref a, %s.dbxrefsourcedb b where a.source_id=b.source_id and b.source_db=?;", database, database);
       flagSourceDB = TRUE;
   }
   else if(pdbxref->parentid > 0)
   {
       key = pdbxref->parentid;
       sprintf((char*)pSQL_stmt, "select id, source_id, record_id, parent_id, link, field, cv from %s.dbxref where parent_id=?;", database);
   }
   else if(pdbxref->link > 0)
   {
       key = pdbxref->link;
       sprintf((char*)pSQL_stmt, "select id, source_id, record_id, parent_id, link, field, cv from %s.dbxref where link=?;", database);
   }
   else if(pdbxref->cv > 0)
   {
       key = pdbxref->cv;
       sprintf((char*)pSQL_stmt, "select id, source_id, record_id, parent_id, link, field, cv from %s.dbxref where cv=?;", database);
   }
   else
   {
       free(pSQL_stmt);
       return SH_ERROR;    /* Nothing was selected */
   }

   /* Create Handle Statement, once handle exists we can free char* */
   retcode = CreateHandle(&hstmt, pSQL_stmt, hdbc, &henv);
   if(retcode != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[SearchDBXREF]: Could not create handle.");
      free(pSQL_stmt);
      return SH_ERROR;
   }
   free(pSQL_stmt);

   /* Bind Parameters for search */

   if( (pdbxref->id > 0) || (pdbxref->sourceid > 0) || (pdbxref->parentid > 0) || (pdbxref->link > 0) || (pdbxref->cv > 0) )
   {
       retcode = SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &key, 0, NULL);
   }
   else if( (pdbxref->recordid != NULL) || (pdbxref->sourcedb != NULL) )
   {
       retcode = SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 0, 0, pKey, iLength, NULL);
   }
   

   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
       SQLFreeStmt(hstmt, SQL_DROP);
       ErrPostEx(SEV_ERROR, 0, 0, "[SearchDBXREF]: Could not bind parameter");
       return SH_ERROR;
   }

   /* execute the select statement */
   if( (retcode = SQLExecute(hstmt)) != SQL_SUCCESS )
   {
       ErrPostEx(SEV_ERROR, 0, 0, "[SearchDBXREF]: Could not execute select statement.");
       LogSQLInfo(hstmt);
       SQLFreeStmt(hstmt, SQL_DROP);       
       return SH_ERROR;
   }

   /* binding our Results from the database to DBXref results  */
   if (pdbxref->sourcedb == NULL)
   {
       retcode = bindDBXrefParameters(hstmt, "SearchDBXREF", FALSE);
   }
   else
   {   /* To bind 2nd paramter sourcedb.val under bindDBXrefParamters */
       retcode = bindDBXrefParameters(hstmt, "SearchDBXREF", TRUE); 
   }
   if (retcode == FALSE)
   {
       SQLFreeStmt(hstmt, SQL_DROP);
       return SH_ERROR;
   }
  
   SQLINTEGER fetchCount = 0;
   /* fetch results and save in your linked list */
   while( (retcode = SQLFetch(hstmt) ==  SQL_SUCCESS)  || (retcode == SQL_SUCCESS_WITH_INFO) )
   {
       fetchCount++;
      
       if(retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO && fetchCount > 1)
       {
	   pdbxref->next = StDbXrefNew();
	   pdbxref = pdbxref->next;  
       } 
       pdbxref->id       = dbxref_id.val;
       if (flagSourceDB == FALSE)
	   pdbxref->sourceid = dbxref_sourceid.val;
       else
	   pdbxref->sourcedb = StringSave((char*)dbxref_sourcedb.val);
       pdbxref->recordid = StringSave((char*)dbxref_recordid.val);
       pdbxref->parentid = dbxref_parentid.val;
       pdbxref->link     = dbxref_link.val;
       pdbxref->field    = StringSave((char*)dbxref_field.val);
       pdbxref->cv       = dbxref_cv.val;
   }
   
   if(SQLFreeStmt(hstmt, SQL_DROP) != SQL_SUCCESS)
   {
       ErrPostEx(SEV_ERROR, 0, 0, "[SearchDBXREF]: Could not free statement.");
       return SH_ERROR;
   }

   if (fetchCount == 0)
   {
       return SH_NODATA;
   }
   return SH_SUCCESS;
}



/********************************************************/
/* Given a pointer to the asn structure that represents */
/*  an dbxref record and a Database name, Source Record,*/
/*  field and a Control Vocabulary.                     */
/*  this function pulls out the Source Record and       */
/*  retrieves all dbXref records associated with that   */
/*  Source Record.                                      */
/*                                                      */
/*  Parameters:                                         */
/*      ppdbxref: Pointer to DBXref structure.          */
/*      dbname:   Database name of the SourceRecord.    */
/*      recordid: SourceRecord.                         */
/*      field:    Field description, Optional  NULL     */
/*      cv:       Control vocabulary, Optional -1       */
/*                                                      */
/*  Returns: TRUE if successful and FALSE otherwise.    */
/*           DBXref LinkList with all record_id         */
/********************************************************/
Int2 LIBCALL GetDBXrefListBySourceRecordInDBXREF (StDbXrefPtr PNTR ppdbxref, CharPtr dbname, CharPtr recordid, CharPtr field, Int4 cv)
{
   StDbXrefPtr pdbxref = NULL;           /* linked list to hold return values */

   CharPtr pKey = NULL;                  /* search key for strings            */

   Char *pSQL_stmt;                      /* SQL select statement              */
   SQLHANDLE hstmt;                      /* handle to select statement        */ 
   SQLRETURN retcode;                    /* return values for odbc layer      */

   if (ppdbxref == NULL)
   {
       ErrPostEx(SEV_ERROR,0,0, "[GetDBXrefListBySourceRecordInDBXREF]: SHdbXrefPtr is NULL.");
       return SH_ERROR;
   }
   pdbxref = *ppdbxref;
   
   /* allocate space for the select statement */
   pSQL_stmt = (char*) malloc(sizeof(char) * 400);
   if(pSQL_stmt == NULL)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[GetDBXrefListBySourceRecordInDBXREF]: Could not allocate space");
      return SH_ERROR;
   }

   /* fill in the select statement based on the search key passed in.
      database is a global variable initialized in ConnectToDb. The
      value of database is set in the ini file. eg database = seqhound. */
   if ( (dbname != NULL) && (recordid != NULL) && (field == NULL) && (cv == -1) )
   {
       sprintf((char*)pSQL_stmt, "select a.id, d.source_db, a.record_id, a.parent_id, a.link, a.field, a.cv from %s.dbxref a, %s.dbxref b, %s.dbxrefsourcedb c, %s.dbxrefsourcedb d where a.parent_id=b.id and b.source_id=c.source_id and a.source_id=d.source_id and c.source_db=? and b.record_id=?;", database, database, database, database);
   }
   else if ( (dbname != NULL) && (recordid != NULL) && (field != NULL) && (cv == -1) )
   {
       sprintf((char*)pSQL_stmt, "select a.id, d.source_db, a.record_id, a.parent_id, a.link, a.field, a.cv from %s.dbxref a, %s.dbxref b, %s.dbxrefsourcedb c, %s.dbxrefsourcedb d where a.parent_id=b.id and b.source_id=c.source_id and a.source_id=d.source_id and c.source_db=? and b.record_id=? and a.field=?;", database, database, database, database);
   }
   else if ( (dbname != NULL) && (recordid != NULL) && (field != NULL) && (cv > -1) )
   {
       sprintf((char*)pSQL_stmt, "select a.id, d.source_db, a.record_id, a.parent_id, a.link, a.field, a.cv from %s.dbxref a, %s.dbxref b, %s.dbxrefsourcedb c, %s.dbxrefsourcedb d where a.parent_id=b.id and b.source_id=c.source_id and a.source_id=d.source_id and c.source_db=? and b.record_id=? and a.field=? and a.cv=?;", database, database, database, database);
   }
   else if ( (dbname != NULL) && (recordid != NULL) && (field == NULL) && (cv > -1) )
   {
       sprintf((char*)pSQL_stmt, "select a.id, d.source_db, a.record_id, a.parent_id, a.link, a.field, a.cv from %s.dbxref a, %s.dbxref b, %s.dbxrefsourcedb c, %s.dbxrefsourcedb d where a.parent_id=b.id and b.source_id=c.source_id and a.source_id=d.source_id and c.source_db=? and b.record_id=? and a.cv=?;", database, database, database, database);
   }
   else
   {
       free(pSQL_stmt);
       ErrPostEx(SEV_ERROR, 0, 0, "[GetDBXrefListBySourceRecordInDBXREF]: Nothing was selected.");
       return SH_ERROR;    /* Nothing was selected */
   }

   /* Create Handle Statement, once handle exists we can free char* */
   retcode = CreateHandle(&hstmt, pSQL_stmt, hdbc, &henv);
   if(retcode != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[GetDBXrefListBySourceRecordInDBXREF]: Could not create handle.");
      free(pSQL_stmt);
      return SH_ERROR;
   }
   free(pSQL_stmt);

   /* Bind Parameters for search */

   if ( (dbname != NULL) && (recordid != NULL) && (field == NULL) && (cv == -1) )
   {
       retcode = SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 0, 0, dbname, strlen(dbname), NULL);
       retcode = SQLBindParameter(hstmt, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 0, 0, recordid,     strlen(recordid),     NULL);
   }
   else if ( (dbname != NULL) && (recordid != NULL) && (field != NULL) && (cv == -1) )
   {
       retcode = SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 0, 0, dbname, strlen(dbname), NULL);
       retcode = SQLBindParameter(hstmt, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 0, 0, recordid,     strlen(recordid),     NULL);
       retcode = SQLBindParameter(hstmt, 3, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 0, 0, field,  strlen(field),  NULL);
   }
   else if ( (dbname != NULL) && (recordid != NULL) && (field != NULL) && (cv > -1) )
   {
       retcode = SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR,  SQL_VARCHAR, 0, 0, dbname,   strlen(dbname),   NULL);
       retcode = SQLBindParameter(hstmt, 2, SQL_PARAM_INPUT, SQL_C_CHAR,  SQL_VARCHAR, 0, 0, recordid, strlen(recordid), NULL);
       retcode = SQLBindParameter(hstmt, 3, SQL_PARAM_INPUT, SQL_C_CHAR,  SQL_VARCHAR, 0, 0, field,    strlen(field),    NULL);
       retcode = SQLBindParameter(hstmt, 4, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &cv,      sizeof(cv),       NULL);
   }
   else if ( (dbname != NULL) && (recordid != NULL) && (field == NULL) && (cv > -1) )
   {
       retcode = SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR,  SQL_VARCHAR, 0, 0, dbname, strlen(dbname), NULL);
       retcode = SQLBindParameter(hstmt, 2, SQL_PARAM_INPUT, SQL_C_CHAR,  SQL_VARCHAR, 0, 0, recordid,     strlen(recordid),     NULL);
       retcode = SQLBindParameter(hstmt, 3, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &cv,    sizeof(cv),     NULL);
   }

   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
       SQLFreeStmt(hstmt, SQL_DROP);
       ErrPostEx(SEV_ERROR, 0, 0, "[GetDBXrefListBySourceRecordInDBXREF]: Could not bind parameter");
       return SH_ERROR;
   }

   /* execute the select statement */
   if( (retcode = SQLExecute(hstmt)) != SQL_SUCCESS )
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[GetDBXrefListBySourceRecordInDBXREF]: Could not execute select statement.");
      LogSQLInfo(hstmt);
      SQLFreeStmt(hstmt, SQL_DROP);
      return SH_ERROR;
   }

   /* binding our Results from the database to DBXref results  */
   retcode = bindDBXrefParameters(hstmt, "[GetDBXrefListBySourceRecordInDBXREF]", TRUE);
   if (retcode == FALSE)
   {
       SQLFreeStmt(hstmt, SQL_DROP);
       ErrPostEx(SEV_ERROR, 0, 0, "[GetDBXrefListBySourceRecordInDBXREF]: Could not bind result columns.");
       return SH_ERROR;
   }

   /* fetch results and save in your linked list */
   SQLINTEGER fetchCount = 0;

   while( (retcode = SQLFetch(hstmt) == SQL_SUCCESS) || (retcode == SQL_SUCCESS_WITH_INFO) )
   {
       fetchCount++;
       if(retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO && fetchCount > 1)
       {
	   pdbxref->next = StDbXrefNew();
	   pdbxref = pdbxref->next;  
       } 
       pdbxref->id       = dbxref_id.val;
       pdbxref->sourcedb = StringSave((char*)dbxref_sourcedb.val);
       pdbxref->recordid = StringSave((char*)dbxref_recordid.val);
       pdbxref->parentid = dbxref_parentid.val;
       pdbxref->link     = dbxref_link.val;
       pdbxref->field    = StringSave((char*)dbxref_field.val);
       pdbxref->cv       = dbxref_cv.val;
   }

   if(SQLFreeStmt(hstmt, SQL_DROP) != SQL_SUCCESS)
   {
       ErrPostEx(SEV_ERROR, 0, 0, "[GetDBXrefListBySourceRecordInDBXREF]: Could not free statement.");
       return SH_ERROR;
   }
   if (fetchCount == 0)
   {
       return SH_NODATA;
   }
   return SH_SUCCESS;
}


/********************************************************/
/* Given a pointer to the asn structure that represents */
/*  an dbxref record and a Database name, Record Id,    */
/*  field and a Control Vocabulary.                     */
/*  this function retrieves all possible Source_Records */
/*  for that Record ID                                  */
/*                                                      */
/*  Parameters:                                         */
/*      ppdbxref: Pointer to DBXref structure.          */
/*      dbname:   Database name of the SourceRecord.    */
/*      recordid: SourceRecord.                         */
/*      field:    Field description, Optional  NULL     */
/*      cv:       Control vocabulary, Optional -1       */
/*                                                      */
/*  Returns: TRUE if successful and FALSE otherwise.    */
/*           DBXref LinkList with all SourceRecords.    */
/********************************************************/
Int2 LIBCALL GetSourceRecordListByDBXrefInDBXREF (StDbXrefPtr PNTR ppdbxref, CharPtr dbname, CharPtr recordid, CharPtr field, Int4 cv)
{
   StDbXrefPtr pdbxref = NULL;           /* linked list to hold return values */

   Char *pSQL_stmt;                      /* SQL select statement              */
   SQLHANDLE hstmt;                      /* handle to select statement        */ 
   SQLRETURN retcode;                    /* return values for odbc layer      */

   if (ppdbxref == NULL)
   {
       ErrPostEx(SEV_ERROR,0,0, "[GetSourceRecordListByDBXrefInDBXREF]: SHdbXrefPtr is NULL.");
       return SH_ERROR;
   }
   pdbxref = *ppdbxref;
   
   /* allocate space for the select statement */
   pSQL_stmt = (char*) malloc(sizeof(char) * 400);
   if(pSQL_stmt == NULL)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[GetSourceRecordListByDBXrefInDBXREF]: Could not allocate space");
      return SH_ERROR;
   }

   /* fill in the select statement based on the search key passed in.
      database is a global variable initialized in ConnectToDb. The
      value of database is set in the ini file. eg database = seqhound. */
   if ( (dbname != NULL) && (recordid != NULL) && (field == NULL) && (cv == -1) )
   {
       sprintf((char*)pSQL_stmt, "select a.id, d.source_db, a.record_id, a.parent_id, a.link, a.field, a.cv from %s.dbxref a, %s.dbxref b, %s.dbxrefsourcedb c, %s.dbxrefsourcedb d where a.id=b.parent_id and b.source_id=c.source_id and a.source_id=d.source_id and c.source_db=? and b.record_id=?;", database, database, database, database);
   }
   else if ( (dbname != NULL) && (recordid != NULL) && (field != NULL) && (cv == -1) )
   {
       sprintf((char*)pSQL_stmt, "select a.id, d.source_db, a.record_id, a.parent_id, a.link, a.field, a.cv from %s.dbxref a, %s.dbxref b, %s.dbxrefsourcedb c, %s.dbxrefsourcedb d where a.id=b.parent_id and b.source_id=c.source_id and a.source_id=d.source_id and c.source_db=? and b.record_id=? and b.field=?;", database, database, database, database);
   }
   else if ( (dbname != NULL) && (recordid != NULL) && (field != NULL) && (cv > -1) )
   {
       sprintf((char*)pSQL_stmt, "select a.id, d.source_db, a.record_id, a.parent_id, a.link, a.field, a.cv from %s.dbxref a, %s.dbxref b, %s.dbxrefsourcedb c, %s.dbxrefsourcedb d where a.id=b.parent_id and b.source_id=c.source_id and a.source_id=d.source_id and c.source_db=? and b.record_id=? and b.field=? and b.cv=?;", database, database, database, database);
   }
   else if ( (dbname != NULL) && (recordid != NULL) && (field == NULL) && (cv > -1) )
   {
       sprintf((char*)pSQL_stmt, "select a.id, d.source_db, a.record_id, a.parent_id, a.link, a.field, a.cv from %s.dbxref a, %s.dbxref b, %s.dbxrefsourcedb c, %s.dbxrefsourcedb d where a.id=b.parent_id and b.source_id=c.source_id and a.source_id=d.source_id and c.source_db=? and b.record_id=? and b.cv=?;", database, database, database, database);
   }
   else
   {
       free(pSQL_stmt);
             ErrPostEx(SEV_ERROR, 0, 0, "[GetSourceRecordListByDBXrefInDBXREF]: Nothing was selected.");
       return SH_ERROR;    /* Nothing was selected */
   }

   /* Create Handle Statement, once handle exists we can free char* */
   retcode = CreateHandle(&hstmt, pSQL_stmt, hdbc, &henv);
   if(retcode != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[GetSourceRecordListByDBXrefInDBXREF]: Could not create handle.");
      free(pSQL_stmt);
      return SH_ERROR;
   }
   free(pSQL_stmt);

   /* Bind Parameters for search */

   if ( (dbname != NULL) && (recordid != NULL) && (field == NULL) && (cv == -1) )
   {
       retcode = SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 0, 0, dbname, strlen(dbname), NULL);
       retcode = SQLBindParameter(hstmt, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 0, 0, recordid,     strlen(recordid),     NULL);
   }
   else if ( (dbname != NULL) && (recordid != NULL) && (field != NULL) && (cv == -1) )
   {
       retcode = SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 0, 0, dbname, strlen(dbname), NULL);
       retcode = SQLBindParameter(hstmt, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 0, 0, recordid,     strlen(recordid),     NULL);
       retcode = SQLBindParameter(hstmt, 3, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 0, 0, field,  strlen(field),  NULL);
   }
   else if ( (dbname != NULL) && (recordid != NULL) && (field != NULL) && (cv > -1) )
   {
       retcode = SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR,  SQL_VARCHAR, 0, 0, dbname, strlen(dbname), NULL);
       retcode = SQLBindParameter(hstmt, 2, SQL_PARAM_INPUT, SQL_C_CHAR,  SQL_VARCHAR, 0, 0, recordid,     strlen(recordid),     NULL);
       retcode = SQLBindParameter(hstmt, 3, SQL_PARAM_INPUT, SQL_C_CHAR,  SQL_VARCHAR, 0, 0, field,  strlen(field),  NULL);
       retcode = SQLBindParameter(hstmt, 4, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &cv,    sizeof(cv),     NULL);
   }
   else if ( (dbname != NULL) && (recordid != NULL) && (field == NULL) && (cv > -1) )
   {
       retcode = SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR,  SQL_VARCHAR, 0, 0, dbname, strlen(dbname), NULL);
       retcode = SQLBindParameter(hstmt, 2, SQL_PARAM_INPUT, SQL_C_CHAR,  SQL_VARCHAR, 0, 0, recordid,     strlen(recordid),     NULL);
       retcode = SQLBindParameter(hstmt, 3, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &cv,    sizeof(cv),     NULL);
   }

   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
       SQLFreeStmt(hstmt, SQL_DROP);
       ErrPostEx(SEV_ERROR, 0, 0, "[GetSourceRecordListByDBXrefInDBXREF]: Could not bind parameter");
       return SH_ERROR;
   }

   /* execute the select statement */
   if( (retcode = SQLExecute(hstmt)) != SQL_SUCCESS )
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[GetSourceRecordListByDBXrefInDBXREF]: Could not execute select statement.");
      LogSQLInfo(hstmt);
      SQLFreeStmt(hstmt, SQL_DROP);
      return SH_ERROR;
   }

   /* binding our Results from the database to DBXref results  */
   retcode = bindDBXrefParameters(hstmt, "GetSourceRecordListByDBXrefInDBXREF", TRUE);
   if (retcode == FALSE)
   {
       SQLFreeStmt(hstmt, SQL_DROP);
       ErrPostEx(SEV_ERROR, 0, 0, "[GetSourceRecordListByDBXrefInDBXREF]: Could not bind result columns.");
       return SH_ERROR;
   }

   /* fetch results and save in your linked list */
   SQLINTEGER fetchCount = 0;

   while( (retcode = SQLFetch(hstmt) ==  SQL_SUCCESS)  || (retcode == SQL_SUCCESS_WITH_INFO) )
   {
       fetchCount++;
       if(retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO && fetchCount > 1)
       {
	   pdbxref->next = StDbXrefNew();
	   pdbxref = pdbxref->next;  
       } 
       pdbxref->id       = dbxref_id.val;
       pdbxref->sourcedb = StringSave((char*)dbxref_sourcedb.val);
       pdbxref->recordid = StringSave((char*)dbxref_recordid.val);
       pdbxref->parentid = dbxref_parentid.val;
       pdbxref->link     = dbxref_link.val;
       pdbxref->field    = StringSave((char*)dbxref_field.val);
       pdbxref->cv       = dbxref_cv.val;
   }

   if(SQLFreeStmt(hstmt, SQL_DROP) != SQL_SUCCESS)
   {
       ErrPostEx(SEV_ERROR, 0, 0, "[GetSourceRecordListByDBXrefInDBXREF]: Could not free statement.");
       return SH_ERROR;
   }
   if (fetchCount == 0)
   {
       return SH_NODATA;
   }

   return SH_SUCCESS;
}


/********************************************************/
/* Given a flag that represent Source Record or         */
/*  Record ID retrive al list of unique Database Names  */
/*  from DBXref.                                        */
/*                                                      */
/*  Parameters:                                         */
/*      recordType: int flag 1 or 0          .          */
/*      where 0 = Source Record.                        */
/*            1 = Record ID.                            */
/*            2 = All Database Names.                   */ 
/*                                                      */
/*  Returns: TRUE if successful and FALSE otherwise.    */
/*           DBXref LinkList with all SourceRecords.    */
/********************************************************/
Int2 LIBCALL GetDBNameListInDBXREF (ValNodePtr PNTR vnp, Int4 recordType)
{
    ValNodePtr vnpDBNameList = NULL;     /* linked list to hold return values */

   Char *pSQL_stmt;                      /* SQL select statement              */
   SQLHANDLE hstmt;                      /* handle to select statement        */ 
   SQLRETURN retcode;                    /* return values for odbc layer      */

   /* allocate space for the select statement */

   pSQL_stmt = (char*) malloc(sizeof(char) * 200);
   if(pSQL_stmt == NULL)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[GetDBNameListInDBXREF]: Could not allocate space");
      return SH_ERROR;
   }

   /* fill in the select statement based on the search key passed in.
      database is a global variable initialized in ConnectToDb. The
      value of database is set in the ini file. eg database = seqhound. */
 
   if ( recordType == 0 )
   {
       sprintf((char*)pSQL_stmt, "select distinct b.source_db from %s.dbxref a, %s.dbxrefsourcedb b where a.source_id=b.source_id and parent_id=0;", database, database);
   }
   else if ( recordType == 1 )
   {
       sprintf((char*)pSQL_stmt, "select distinct b.source_db from %s.dbxref a, %s.dbxrefsourcedb b where a.source_id=b.source_id and parent_id<>0;", database, database);
   }
   else if ( recordType == 2 )
   {   /* This statement shut be: select source_db from %s.dbxrefsourcedb;  */
       sprintf((char*)pSQL_stmt, "select distinct b.source_db from %s.dbxref a, %s.dbxrefsourcedb b where a.source_id=b.source_id;", database, database);
   }
   else
   {
       free(pSQL_stmt);
       ErrPostEx(SEV_ERROR, 0, 0, "[GetDBNameListInDBXREF]: Nothing was selected.");
       return SH_ERROR;    /* Nothing was selected */
   }

   /* Create Handle Statement, once handle exists we can free char* */
   retcode = CreateHandle(&hstmt, pSQL_stmt, hdbc, &henv);
   if(retcode != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[GetDBNameListInDBXREF]: Could not create handle.");
      free(pSQL_stmt);
      return SH_ERROR;
   }
   free(pSQL_stmt);

   /* NO Bind Parameters for search is needed */

   /* execute the select statement */

   if( (retcode = SQLExecute(hstmt)) != SQL_SUCCESS )
   {
       ErrPostEx(SEV_ERROR, 0, 0, "[GetDBNameListInDBXREF]: Could not execute select statement.");
       LogSQLInfo(hstmt);
       SQLFreeStmt(hstmt, SQL_DROP);
       return SH_ERROR;
   }

   /* binding our Results from the database to DBXref results  */

    retcode = SQLBindCol(hstmt, 1,  SQL_C_CHAR, &dbxref_sourcedb.val, sizeof(dbxref_sourcedb.val), NULL);
    if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
    {
	ErrPostEx(SEV_ERROR, 0, 0,"[GetDBNameListInDBXREF]: Could not bind column 1." );
	SQLFreeStmt(hstmt, SQL_DROP);
	return SH_ERROR;
    }

   /* fetch results and save in your linked list */
    SQLINTEGER fetchCount = 0;
    while( (retcode = SQLFetch(hstmt) == SQL_SUCCESS) || (retcode == SQL_SUCCESS_WITH_INFO) )
    {
	fetchCount++;
	ValNodeCopyStr(&vnpDBNameList, 0, dbxref_sourcedb.val);
    }

   *vnp = vnpDBNameList;

   if(SQLFreeStmt(hstmt, SQL_DROP) != SQL_SUCCESS)
   {
       ErrPostEx(SEV_ERROR, 0, 0, "[GetSourceRecordListByDBXrefInDBXREF]: Could not free statement.");
       return SH_ERROR;
   }
   
   if (fetchCount == 0)
   {
       return SH_NODATA;
   }
   return SH_SUCCESS;
}


Int2 LIBCALL GetCVListInDBXrefSourcedb (ValNodePtr PNTR vnp)
{
    ValNodePtr vnpCVList = NULL;     /* linked list to hold return values */
    
    Char *pSQL_stmt;                      /* SQL select statement              */
    SQLHANDLE hstmt;                      /* handle to select statement        */ 
    SQLRETURN retcode;                    /* return values for odbc layer      */
    SQLINTEGER cv;

   /* allocate space for the select statement */
   pSQL_stmt = (char*) malloc(sizeof(char) * 200);
   if(pSQL_stmt == NULL)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[GetCVListInDBXrefSourcedb]: Could not allocate space");
      return FALSE;
   }

   sprintf((char*)pSQL_stmt, "select distinct cv from %s.dbxref", database);
   
   /* Create Handle Statement, once handle exists we can free char* */

   retcode = CreateHandle(&hstmt, pSQL_stmt, hdbc, &henv);
   if(retcode != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[GetCVListInDBXrefSourcedb]: Could not create handle.");
      free(pSQL_stmt);
      return SH_ERROR;
   }
   free(pSQL_stmt);

   /* NO Bind Parameters for search is needed */

   /* execute the select statement */
   if( (retcode = SQLExecute(hstmt)) != SQL_SUCCESS )
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[GetCVListInDBXrefSourcedb]: Could not execute select statement.");
      LogSQLInfo(hstmt);
      SQLFreeStmt(hstmt, SQL_DROP);
      return SH_ERROR;
   }

   /* binding our Results from the database to DBXref results  */

   retcode = SQLBindCol(hstmt, 1, SQL_C_SLONG, &cv, sizeof(cv), NULL);
    if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
    {
	ErrPostEx(SEV_ERROR, 0, 0,"[GetCVListInDBXrefSourcedb]: Could not bind column 1." );
	SQLFreeStmt(hstmt, SQL_DROP);
	return SH_ERROR;
    }

   /* fetch results and save in your linked list */
    SQLINTEGER fetchCount = 0;

    while( (retcode = SQLFetch(hstmt) == SQL_SUCCESS) || (retcode == SQL_SUCCESS_WITH_INFO) )
    {
	fetchCount++;
	ValNodeAddInt(&vnpCVList, 1, cv);
    }

   *vnp = vnpCVList;

   if(SQLFreeStmt(hstmt, SQL_DROP) != SQL_SUCCESS)
   {
       ErrPostEx(SEV_ERROR, 0, 0, "[GetCVListInDBXrefSourcedb]: Could not free statement.");
       return SH_ERROR;
   }
   if(fetchCount ==0)
   {
       return SH_NODATA;
   }
   return SH_SUCCESS;
}


Int2 LIBCALL GetXrefSourceFiledName (ValNodePtr PNTR vnp)
{
    ValNodePtr vnpFieldList = NULL;     /* linked list to hold return values */

    Char *pSQL_stmt;                      /* SQL select statement              */
    SQLHANDLE hstmt;                      /* handle to select statement        */ 
    SQLRETURN retcode;                    /* return values for odbc layer      */
    Char field[20];

    /* allocate space for the select statement */

   pSQL_stmt = (char*) malloc(sizeof(char) * 200);
   if(pSQL_stmt == NULL)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[GetXrefSourceFiledName]: Could not allocate space");
      return SH_ERROR;
   }

   sprintf((char*)pSQL_stmt, "select distinct field from %s.dbxref where parent_id != 0", database);

   /* Create Handle Statement, once handle exists we can free char* */
   retcode = CreateHandle(&hstmt, pSQL_stmt, hdbc, &henv);
   if(retcode != SQL_SUCCESS)
   {
       ErrPostEx(SEV_ERROR, 0, 0, "[GetXrefSourceFiledName]: Could not create handle.");
       free(pSQL_stmt);
       return SH_ERROR;
   }
   free(pSQL_stmt);

   /* NO Bind Parameters for search is needed */

   /* execute the select statement */
   if( (retcode = SQLExecute(hstmt)) != SQL_SUCCESS )
   {
       ErrPostEx(SEV_ERROR, 0, 0, "[GetXrefSourceFiledName]: Could not execute select statement.");
       LogSQLInfo(hstmt);
       SQLFreeStmt(hstmt, SQL_DROP);
       return SH_ERROR;
   }

   /* binding our Results from the database to DBXref results  */
   
   retcode = SQLBindCol(hstmt, 1, SQL_C_CHAR, &field, sizeof(field), NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
	ErrPostEx(SEV_ERROR, 0, 0,"[GetXrefSourceFiledName]: Could not bind column 1." );
	SQLFreeStmt(hstmt, SQL_DROP);
	return SH_ERROR;
   }

   /* fetch results and save in your linked list */
    CharPtr aux = NULL;

    SQLINTEGER fetchCount = 0;
    while( (retcode = SQLFetch(hstmt) == SQL_SUCCESS) || (retcode == SQL_SUCCESS_WITH_INFO) )
    {
	fetchCount++;
	ValNodeCopyStr(&vnpFieldList, 0, field);
    }

   *vnp = vnpFieldList;
   
   if(SQLFreeStmt(hstmt, SQL_DROP) != SQL_SUCCESS)
   {
       ErrPostEx(SEV_ERROR, 0, 0, "[GetXrefSourceFiledName]: Could not free statement.");
       return SH_ERROR;
   }
   if (fetchCount == 0)
   {
       return SH_NODATA;
   }
   return SH_SUCCESS;
}

Int2 LIBCALL GetObjectIDbyAccInDBXREF (StDbXrefPtr PNTR ppdbxref, CharPtr acc)
{
   StDbXrefPtr pdbxref = NULL;           /* linked list to hold return values */

   Int4 iKey = 0, iLength = 0;           /* search key and length of key */
   CharPtr pKey = NULL;                  /* search key for strings */

   Char *pSQL_stmt;                      /* SQL select statement */
   SQLHANDLE hstmt;                      /* handle to select statement */ 
   SQLRETURN retcode;                    /* return values for odbc layer */

   SQLCHAR ref[30];

   if (ppdbxref == NULL)
   {
       ErrPostEx(SEV_ERROR,0,0, "[GetObjectIDbyAccInDBXREF]: SHdbXrefPtr is NULL.");
       return SH_ERROR;
   }
   pdbxref = *ppdbxref;
   
   /* allocate space for the select statement */
   pSQL_stmt = (char*) malloc(sizeof(char) * 200);
   if(pSQL_stmt == NULL)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[GetObjectIDbyAccInDBXREF]: Could not allocate space");
      return SH_ERROR;
   }

   /* fill in the select statement based on the search key passed in.
      database is a global variable initialized in ConnectToDb. The
      value of database is set in the ini file. eg database = seqhound. */
 
   sprintf((char*)pSQL_stmt, "select a.id, a.source_id, a.record_id, a.parent_id, a.link, a.field, a.cv from %s.dbxref a, %s.dbxref b where a.id=b.parent_id and b.record_id=?;", database, database);

   /* create handle to statement, once handle exists we can free char* */
   retcode = CreateHandle(&hstmt, pSQL_stmt, hdbc, &henv);

   if(retcode != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[GetObjectIDbyAccInDBXREF]: Could not create handle.");
      free(pSQL_stmt);
      return SH_ERROR;
   }
   free(pSQL_stmt);

   /* bind the search key */
   pKey = acc;

   iLength = strlen(pKey);
   sprintf(ref, acc); 

   sprintf(dbxef_recParam.val, acc);

   retcode = SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 0, 0, ref, sizeof(ref), NULL);
   if(retcode != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[GetObjectIDbyAccInDBXREF]: Could not bind parameter");
      return SH_ERROR;
   }

   /* execute the select statement */
   if( (retcode = SQLExecute(hstmt)) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[GetObjectIDbyAccInDBXREF]: Could not execute select statement.");
      LogSQLInfo(hstmt);
      SQLFreeStmt(hstmt, SQL_DROP);
      return SH_ERROR;
   }

   /* binding our Results from the database to DBXref results  */
   retcode = bindDBXrefParameters(hstmt, "GetObjectIDbyAccInDBXREF", FALSE);
   if (retcode == FALSE)
   {
       return SH_ERROR;
   }

   /* fetch results and save in your linked list */
   SQLINTEGER fetchCount = 0;

   while( (retcode = SQLFetch(hstmt) == SQL_SUCCESS) || (retcode == SQL_SUCCESS_WITH_INFO) )
   {
       fetchCount++;

       if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO && fetchCount > 1)
       {
	   pdbxref->next = StDbXrefNew();
	   pdbxref = pdbxref->next;  
       }
       pdbxref->id       = dbxref_id.val;
       pdbxref->sourceid = dbxref_sourceid.val; 
       pdbxref->recordid = StringSave((char*)dbxref_recordid.val);
       pdbxref->parentid = dbxref_parentid.val;
       pdbxref->link     = dbxref_link.val;
       pdbxref->field    = StringSave((char*)dbxref_field.val);
       pdbxref->cv       = dbxref_cv.val;
   }

   if(SQLFreeStmt(hstmt, SQL_DROP) != SQL_SUCCESS)
   {
       ErrPostEx(SEV_ERROR, 0, 0, "[DBXREFGetObjectIDbyAcc]: Could not free statement.");
       return SH_ERROR;
   }
   if (fetchCount == 0)
   {
       return SH_NODATA;
   }
   return SH_SUCCESS;
}



Boolean CloseDB(void)
{
   Int2 sqlrc = SQL_SUCCESS;

   /* commit all changes */
   SQLTransact(henv, hdbc, SQL_COMMIT);


   /*Free the database handles used to communicate with the database*/
   sqlrc = SQLDisconnect(hdbc);
   if (sqlrc != SQL_SUCCESS && sqlrc != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "Could not disconnect from the database.");
   }

   /*free the connnection handle */
   sqlrc = SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
   if (sqlrc != SQL_SUCCESS && sqlrc != SQL_SUCCESS_WITH_INFO) 
   {
     ErrPostEx(SEV_ERROR, 0, 0, "could not free the connection handle.");
   }

   /*free envirnment handle */
   sqlrc = SQLFreeHandle(SQL_HANDLE_ENV, henv);
   if (sqlrc != SQL_SUCCESS && sqlrc != SQL_SUCCESS_WITH_INFO) 
   {
     ErrPostEx(SEV_ERROR, 0, 0, "could not free the environment handle.");	
   }

   return TRUE;
}


Boolean bindDBXrefParameters(SQLHANDLE *hstmt2bind, CharPtr function, Boolean flagSourceDBStr)
{
    SQLRETURN retcode;
    Char msg[200];
    if (function != NULL)
    {
	strncat(msg, "[", strlen("["));
	strncat(msg, function, strlen(function));
    }
    retcode = SQLBindCol(hstmt2bind, 1, SQL_C_SLONG, &dbxref_id.val, sizeof(dbxref_id.val), NULL);
    if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
    {
	strncat(msg, "]: Could not bind column 1.", strlen("]: Could not bind column 1."));
	ErrPostEx(SEV_ERROR, 0, 0, msg);
	return FALSE;
    }
    if (flagSourceDBStr == TRUE) 
    {
	retcode = SQLBindCol(hstmt2bind, 2, SQL_C_CHAR, &dbxref_sourcedb.val, sizeof(dbxref_sourcedb.val), NULL);
    }
    else 
    {
	retcode = SQLBindCol(hstmt2bind, 2, SQL_C_SLONG, &dbxref_sourceid.val, sizeof(dbxref_sourceid.val), NULL);
    }
    if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
    {
	strncat(msg, "]: Could not bind column 2.", strlen("]: Could not bind column 2."));
	ErrPostEx(SEV_ERROR, 0, 0, msg);
	return FALSE;
    }
    retcode = SQLBindCol(hstmt2bind, 3, SQL_C_CHAR, &dbxref_recordid.val, sizeof(dbxref_recordid.val), NULL);
    if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
    {
	strncat(msg, "]: Could not bind column 3.", strlen("]: Could not bind column 3."));
	ErrPostEx(SEV_ERROR, 0, 0, msg);
	return FALSE; 
    }
    retcode = SQLBindCol(hstmt2bind, 4, SQL_C_SLONG, &dbxref_parentid.val, sizeof(dbxref_parentid.val), NULL);
    if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
    {
	strncat(msg, "]: Could not bind column 4.", strlen("]: Could not bind column 4."));
	ErrPostEx(SEV_ERROR, 0, 0, msg);
	return FALSE;
    }
    retcode = SQLBindCol(hstmt2bind, 5, SQL_C_SLONG, &dbxref_link.val, sizeof(dbxref_link.val), NULL);
    if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
    {
	strncat(msg, "]: Could not bind column 5.", strlen("]: Could not bind column 5."));
	ErrPostEx(SEV_ERROR, 0, 0, msg);
	return FALSE;
    }
    retcode = SQLBindCol(hstmt2bind, 6, SQL_C_CHAR, &dbxref_field.val, sizeof(dbxref_field.val), NULL);
    if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
    {
	strncat(msg, "]: Could not bind column 6.", strlen("]: Could not bind column 6."));
	ErrPostEx(SEV_ERROR, 0, 0, msg);
	return FALSE;
    }
    retcode = SQLBindCol(hstmt2bind, 7, SQL_C_SLONG, &dbxref_cv.val, sizeof(dbxref_cv.val), NULL);
    if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
    {
	strncat(msg, "]: Could not bind column 7.", strlen("]: Could not bind column 7."));
	ErrPostEx(SEV_ERROR, 0, 0, msg);
	return FALSE;
    }
    return TRUE;
}
