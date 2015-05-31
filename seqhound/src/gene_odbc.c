/* $Id: gene_odbc.c,v 1.1 2005/04/26 19:08:50 eburgess Exp $
*******************************************************************************************
  Copyright (C) 2000 Mount Sinai Hospital (MSH)

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of
  the License, or any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
xxxx2
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
           Elizabeth Burgess and Christopher W.V. Hogue hogue@mshri.on.ca


  REVISION/CONTRIBUTION NOTES:

  January 17, 2005  Original MSH Revision.
                    Hogue Lab - University of Toronto Biochemistry
                    Department and the Samuel Lunenfeld Research
                    Institute, Mount Sinai Hospital
                    http://bioinfo.mshri.on.ca  hogue@mshri.on.ca
                    ftp://ftp.mshri.on.ca/


  PUBLICATION TO CITE:


  DESCRIPTION
  Comgen related functions.


******************************************************************************************/
#include <gene_odbc.h>
#include <odbc_func.h>
#include <slri_odbc.h>

/* SeqHound Global Variables */
SQLCHAR database[50]; 

Boolean LIBCALL InitGENE()
{
  SQLRETURN retcode;
  CharPtr pSQL_stmt = (Char *) Malloc(sizeof(Char) * 150);
  if(pSQL_stmt == NULL)
  {
	ErrPostEx(SEV_ERROR, 0, 0, "[InitGENE]: Could not allocate "
			  "space");
	return FALSE;
  }

  /***********************************************
   ** search for gi by geneid. 
   ************************************************/ 
  sprintf((char *) pSQL_stmt, "select g.gi  from "
		   "%s.gene_gi g, %s.gene_object o where "
		  "g.geneobjectid = o.id and o.geneid=?;", 
		  database, database);
  
  if( !PrepStmt( &HNDLGeneSearchGiByGeneid, pSQL_stmt, 1, 
				 'd', &geneobject_geneid.val, 0 ) ) 
  {
	free( pSQL_stmt );
	return FALSE; 
  }

  /***********************************************
   ** search for gi by database and database accession. 
   ************************************************/ 
  sprintf((char *) pSQL_stmt, "select g.gi  from "
		   "%s.gene_gi g, %s.gene_dbxrefs d where g.geneobjectid = "
		  "d.geneobjectid and d.dbname = ? and d.dbaccess = ?;", 
		  database, database);
  
  if( !PrepStmt( &HNDLGeneSearchGiByDbxref, pSQL_stmt, 2, 
				 's', genedbxref_dbname.val, 
				 sizeof( genedbxref_dbname.val ), 
				 's', genedbxref_dbaccess.val, 
				 sizeof( genedbxref_dbaccess.val ) ) )
  {
	free( pSQL_stmt );
	return FALSE; 
  }

   /***********************************************
   ** search for a database accession by gi and db name. 
   ************************************************/ 
  sprintf((char *) pSQL_stmt, "select d.dbaccess from  "
		  "%s.gene_dbxrefs d, %s.gene_gi g where "
		 "d.geneobjectid = g.geneobjectid and "
		 "g.gi = ? and d.dbname = ?", 
		  database, database);
		  
  if( !PrepStmt( &HNDLGeneSearchDbxrefByGi, pSQL_stmt, 2, 
				 'd', &genegi_gi.val, 0,
				 's', genedbxref_dbname.val,/*30  ))*/
	 				 sizeof( genedbxref_dbname.val ) ) ) 
  {
	free( pSQL_stmt );
	return FALSE; 
  }

/***********************************************
   ** search for a locus link id (geneid) by 
      database name and accession. 
   ************************************************/ 
  sprintf( (char *) pSQL_stmt, "select o.geneid from gene_dbxrefs d, "
		   "gene_object o where d.geneobjectid = o.id and "
		   "d.dbname = ? and d.dbaccess = ?" );

  if( !PrepStmt( &HNDLGeneSearchGeneidByDbxref, pSQL_stmt, 2, 
				 's', genedbxref_dbname.val, 
				 sizeof( genedbxref_dbname.val ), 
				 's', genedbxref_dbaccess.val, 
				 sizeof( genedbxref_dbaccess.val ) ) )				 
  {
	free( pSQL_stmt );
	return FALSE; 
  }

/***********************************************
   ** search for a database accession by 
      database name and geneid. 
   ************************************************/ 
  sprintf( (char *) pSQL_stmt, "select d.dbaccess from "
		   "gene_dbxrefs d, gene_object o where d.geneobjectid "
		   "= o.id and o.geneid = ?" );



  if( !PrepStmt( &HNDLGeneSearchDbxrefByGeneid, pSQL_stmt, 1, 
				 'd', &geneobject_geneid.val, 0 ) )							 
  {
	free( pSQL_stmt );
	return FALSE; 
  }


/***********************************************
   ** search for a geneid (llid) by gi. 
   ************************************************/ 
  sprintf( (char *) pSQL_stmt, "select o.geneid from gene_gi g, "
		   "gene_object o where g.geneobjectid = o.id and "
		   "g.gi = ?" );

  if( !PrepStmt( &HNDLGeneSearchGeneidByGi, pSQL_stmt, 1, 
				 'd', &genegi_gi.val, 0 ) ) 
  {
	free( pSQL_stmt );
	return FALSE; 
  }

/***********************************************
   ** search for a geneid (llid) by gi. 
   ************************************************/ 
  sprintf( (char *) pSQL_stmt, "select i.maplocation from gene_info i, "
		   "gene_gi g where g.geneobjectid = i.geneobjectid and "
		   "g.gi = ?" );

  if( !PrepStmt( &HNDLGeneSearchLocusByGi, pSQL_stmt, 1, 
				 'd', &genegi_gi.val, 0 ) ) 
  {
	free( pSQL_stmt );
	return FALSE; 
  }

  free(pSQL_stmt);
  return TRUE;
}

/**************************************************************/
/** CloseGENE
 * 
 *  Frees all the statement handles. Returns TRUE.
 */
Boolean LIBCALL CloseGENE()
{
  if (SQLFreeHandle(SQL_HANDLE_STMT, HNDLGeneSearchDbxrefByGi) != SQL_SUCCESS)
	print_err(HNDLGeneSearchDbxrefByGi);

  if (SQLFreeHandle(SQL_HANDLE_STMT, HNDLGeneSearchGiByDbxref) != SQL_SUCCESS)
	print_err(HNDLGeneSearchGiByDbxref);

  if (SQLFreeHandle(SQL_HANDLE_STMT, HNDLGeneSearchGiByGeneid) != SQL_SUCCESS)
	print_err(HNDLGeneSearchGiByGeneid);

  if (SQLFreeHandle(SQL_HANDLE_STMT, HNDLGeneSearchDbxrefByGeneid) != SQL_SUCCESS)
	print_err(HNDLGeneSearchDbxrefByGeneid);

  if (SQLFreeHandle(SQL_HANDLE_STMT, HNDLGeneSearchGeneidByDbxref) != SQL_SUCCESS)
	print_err(HNDLGeneSearchGeneidByDbxref);

  if (SQLFreeHandle(SQL_HANDLE_STMT, HNDLGeneSearchGeneidByGi) != SQL_SUCCESS)
	print_err(HNDLGeneSearchGeneidByGi);

  if (SQLFreeHandle(SQL_HANDLE_STMT, HNDLGeneSearchLocusByGi) != SQL_SUCCESS)
	print_err(HNDLGeneSearchLocusByGi);


  return TRUE;
}

ValNodePtr GetGENEDbxrefFromGi( Int4 gi, CharPtr name )
{
  ValNodePtr pvnAccess = NULL;
  Int4 retcode = 0;

  if( 0 >= gi || NULL == name )
  {
	ErrPostEx(SEV_ERROR,0,0, "GetGENEDbxrefFromGi: Bad Input" );

	return NULL;
  }

  sprintf( (char *)genedbxref_dbname.val, "%s", name );
  genegi_gi.val = gi;

  if( !ExecuteStmt( &HNDLGeneSearchDbxrefByGi, 1,
					's', genedbxref_dbname.val, 
					30 ) )
  {
	ErrPostEx(SEV_ERROR,0,0, "GetGENEDbxrefFromGi: Unable to "
			  "execute statement for %s %ld.", name, gi );

	return NULL;
  }

  while((retcode=SQLFetch(HNDLGeneSearchDbxrefByGi)) == SQL_SUCCESS || 
		 retcode == SQL_SUCCESS_WITH_INFO ) 
  {
	pvnAccess = Slri_ValNodeAddPointer( &pvnAccess, 1, 
										genedbxref_dbaccess.val );
  }

 /* Close the cursor to reuse the handle */
  retcode = SQLFreeStmt(HNDLGeneSearchDbxrefByGi, SQL_CLOSE);
  if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
  {
	ErrPostEx(SEV_ERROR, 0, 0, "GetGENEDbxrefFromGi: "
			  "Error in closing cursor.");
	return NULL;
  }

  return pvnAccess;
}

ValNodePtr GetGiFromGENEDbxref( CharPtr name, CharPtr access )
{
  ValNodePtr pGis = NULL;
  Int4 retcode = 0;

  if( NULL == name || NULL == access )
  {
	ErrPostEx(SEV_ERROR,0,0, "GetGiFromGeneDbxref: Bad Input" );

	return NULL;
  }

  sprintf( (char *)genedbxref_dbname.val, "%s", name );
  sprintf( (char *)genedbxref_dbaccess.val, "%s", access );
  
  if( !ExecuteStmt( &HNDLGeneSearchGiByDbxref, 1,
					'd', &genegi_gi.val, 0 ) )
  {
	ErrPostEx(SEV_ERROR,0,0, "GetGiFromGeneDbxref: Unable to "
			  "execute statement for %s %s.", name, access );

	return NULL;
  }

  while((retcode=SQLFetch(HNDLGeneSearchGiByDbxref)) == SQL_SUCCESS || 
		 retcode == SQL_SUCCESS_WITH_INFO ) 
  {
	pGis = Slri_ValNodeAddInt( &pGis, 1, genegi_gi.val );
  }

 
 /* Close the cursor to reuse the handle */
  retcode = SQLFreeStmt(HNDLGeneSearchGiByDbxref, SQL_CLOSE);
  if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
  {
	ErrPostEx(SEV_ERROR, 0, 0, "GetGiFromGeneDbxref: "
			  "Error in closing cursor.");
	return NULL;
  }

  return pGis;
}

ValNodePtr LIBCALL GetGiFromGeneid( Int4 geneid )
{
  ValNodePtr pGis = NULL;
  Int4 retcode = 0;

  if( 0 >= geneid  )
  {
	ErrPostEx(SEV_ERROR,0,0, "GetGiFromGeneid: Bad Input" );

	return NULL;
  }

  geneobject_geneid.val = geneid;

  if( !ExecuteStmt( &HNDLGeneSearchGiByGeneid, 1,
					'd', &genegi_gi.val, 0 ) )
  {
	ErrPostEx(SEV_ERROR,0,0, "GetGiFromGeneid: Unable to"
			  "execute statement for geneid %ld", geneid );

	return NULL;
  }

  while((retcode=SQLFetch(HNDLGeneSearchGiByGeneid)) == SQL_SUCCESS || 
		 retcode == SQL_SUCCESS_WITH_INFO ) 
  {
	pGis = Slri_ValNodeAddInt( &pGis, 1, genegi_gi.val );
  }

 

/* Close the cursor to reuse the handle */
  retcode = SQLFreeStmt(HNDLGeneSearchGiByGeneid, SQL_CLOSE);
  if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
  {
	ErrPostEx(SEV_ERROR, 0, 0, "GetGiFromGeneid: "
			  "Error in closing cursor.");
	return NULL;
  }

  return pGis;
}

ValNodePtr LIBCALL GetGeneidFromGENEDbxref( CharPtr name, 
										CharPtr access )
{
  ValNodePtr pGeneids = NULL;
  Int4 retcode = 0;

  if( NULL == name || NULL == access )
  {
	ErrPostEx(SEV_ERROR,0,0, "GetGeneidFromGENEDbxref: Bad Input" );

	return NULL;
  }

  sprintf( (char *)genedbxref_dbname.val, "%s", name );
  sprintf( (char *)genedbxref_dbaccess.val, "%s", access );
  
  if( !ExecuteStmt( &HNDLGeneSearchGeneidByDbxref, 1,
					'd', &geneobject_geneid.val, 0 ) )
  {
	ErrPostEx(SEV_ERROR,0,0, "GetGeneidFromGENEDbxref: Unable to "
			  "execute statement for %s %s.", name, access );

	return NULL;
  }

  while((retcode=SQLFetch(HNDLGeneSearchGeneidByDbxref)) == SQL_SUCCESS || 
		 retcode == SQL_SUCCESS_WITH_INFO ) 
  {
	pGeneids = Slri_ValNodeAddInt( &pGeneids, 1, 
								   geneobject_geneid.val );
  }

 
 /* Close the cursor to reuse the handle */
  retcode = SQLFreeStmt(HNDLGeneSearchGeneidByDbxref, SQL_CLOSE);
  if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
  {
	ErrPostEx(SEV_ERROR, 0, 0, "GetGeneidFromGENEDbxref: "
			  "Error in closing cursor.");
	return NULL;
  }

  return pGeneids;
}

ValNodePtr LIBCALL GetGENEDbxrefFromGeneid( Int4 geneid, 
											CharPtr name )
{
  ValNodePtr pvnAccess = NULL;
  Int4 retcode = 0;

  if( 0 >= geneid || NULL == name )
  {
	ErrPostEx(SEV_ERROR,0,0, "GetGENEDbxrefFromGeneid: Bad Input" );

	return NULL;
  }

  sprintf( (char *)genedbxref_dbname.val, "%s", name );
  geneobject_geneid.val = geneid;

  if( !ExecuteStmt( &HNDLGeneSearchDbxrefByGi, 1,
					's', genedbxref_dbaccess.val, 
					30 ) )
  {
	ErrPostEx(SEV_ERROR,0,0, "GetGENEDbxrefFromGeneid: Unable to "
			  "execute statement for %s %ld.", name, geneid );

	return NULL;
  }

  while((retcode=SQLFetch(HNDLGeneSearchDbxrefByGi)) == SQL_SUCCESS || 
		 retcode == SQL_SUCCESS_WITH_INFO ) 
  {
	pvnAccess = Slri_ValNodeAddPointer( &pvnAccess, 1, 
										genedbxref_dbaccess.val );
  }

 /* Close the cursor to reuse the handle */
  retcode = SQLFreeStmt(HNDLGeneSearchDbxrefByGi, SQL_CLOSE);
  if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
  {
	ErrPostEx(SEV_ERROR, 0, 0, "GetGENEDbxrefFromGeneid: "
			  "Error in closing cursor.");
	return NULL;
  }

  return pvnAccess;
}

Int4 LIBCALL GetGeneidFromGi( Int4 gi )
{
  Int4 geneid = 0;
  Int4 retcode = -1;

  if( 0 >= gi  )
  {
	ErrPostEx(SEV_ERROR,0,0, "GetGeneidFromGi: Bad Input" );

	return -1;
  }

  genegi_gi.val = gi;

  if( !ExecuteStmt( &HNDLGeneSearchGeneidByGi, 1,
					'd', &geneobject_geneid.val, 0 ) )
  {
	ErrPostEx(SEV_ERROR,0,0, "GetGeneidFromGi: Unable to "
			  "execute statement for %ld.", gi );

	return -1;
  }

  retcode = SQLFetch( HNDLGeneSearchGeneidByGi );

  if( retcode == SQL_SUCCESS || 
	  retcode == SQL_SUCCESS_WITH_INFO )
  { 
	geneid = geneobject_geneid.val;
  }

  return geneid;
}

CharPtr    LIBCALL GetLocusFromGi( Int4 gi )
{
  CharPtr locus = NULL;
  Int4 retcode = -1;

  if( 0 >= gi  )
  {
	ErrPostEx(SEV_ERROR,0,0, "GetLocusFromGi: Bad Input" );

	return locus;
  }

  genegi_gi.val = gi;

  if( !ExecuteStmt( &HNDLGeneSearchLocusByGi, 1,
					's', geneinfo_location.val, 
					sizeof(geneinfo_location.val) ) )
  {
	ErrPostEx(SEV_ERROR,0,0, "GetLocusFromGi: Unable to "
			  "execute statement for %ld.", gi );

	return locus;
  }

  retcode = SQLFetch( HNDLGeneSearchLocusByGi );

  if( retcode == SQL_SUCCESS || 
	  retcode == SQL_SUCCESS_WITH_INFO )
  { 
    locus = StringSave((const char*) geneinfo_location.val );
  }

  return locus;

}

/*
$Log: gene_odbc.c,v $
Revision 1.1  2005/04/26 19:08:50  eburgess
Functions to access GENE tables. Initial revision.

*/
