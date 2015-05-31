/* $Id: comgen_odbc.c,v 1.6 2005/05/02 15:48:50 eburgess Exp $
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
#include <comgen_odbc.h>
#include <odbc_func.h>
#include <intrez_misc.h>


/* SeqHound Global Variables */
SQLCHAR database[50]; 

/**************************************************************/
/** InitCOMGEN
 * 
 *  Initialize the NAMES tables Bioentity and Bioname. Other
 *  NAMES tables are only look up tables and are populated by
 *  scripts. Returns FALSE on failure, TRUE on success.
 */
Boolean LIBCALL InitCOMGEN()
{
  if(!InitCHROM())
  {
	ErrPostEx(SEV_ERROR, 0, 0, "[InitCodeBase]: Could not initialize CHROM");
	return FALSE;
  }

  if(!InitGICHROMID())
  {
	ErrPostEx(SEV_ERROR, 0, 0, "[InitCodeBase]: Could not initialize GICHROMID");
	return FALSE;
  }

  if(!InitCONTIGCHROMID())
  {
	ErrPostEx(SEV_ERROR, 0, 0, "[InitCodeBase]: Could not initialize CONTIGCHROMID");
	return FALSE;
  }

  if(!InitGICHROMOSOME())
  {
	ErrPostEx(SEV_ERROR, 0, 0, "[InitCodeBase]: "
			  "Could not initialize GICHROMOSOME");
	return FALSE;
  }

  if(!InitCONTIGCHROMOSOME())
  {
	ErrPostEx(SEV_ERROR, 0, 0, "[InitCodeBase]: "
			  "Could not initialize GICHROMOSOME");
	return FALSE;
  }

  return TRUE;
}

/**************************************************************/
/** CloseCOMGEN
 * 
 *  Frees all the statement handles. Returns TRUE.
 */
Boolean LIBCALL CloseCOMGEN()
{
 if (SQLFreeHandle(SQL_HANDLE_STMT, HNDLChrom) != SQL_SUCCESS)
	 print_err(HNDLChrom);
   if (SQLFreeHandle(SQL_HANDLE_STMT, HNDLChromSearchAccess) != SQL_SUCCESS)
	 print_err(HNDLChromSearchAccess);
   if (SQLFreeHandle(SQL_HANDLE_STMT, HNDLChromDeleteAccess) != SQL_SUCCESS)
	 print_err(HNDLChromDeleteAccess);
   if (SQLFreeHandle(SQL_HANDLE_STMT, HNDLChromSearchChromId) != SQL_SUCCESS)
	 print_err(HNDLChromSearchChromId);
   if (SQLFreeHandle(SQL_HANDLE_STMT, HNDLChromSearchChromNum) != SQL_SUCCESS)
	 print_err(HNDLChromSearchChromNum);
   if (SQLFreeHandle(SQL_HANDLE_STMT, HNDLGichromidInsert) != SQL_SUCCESS)
	 print_err(HNDLGichromidInsert);
   if (SQLFreeHandle(SQL_HANDLE_STMT, HNDLGichromidReplace) != SQL_SUCCESS)
	 print_err(HNDLGichromidReplace);
   if (SQLFreeHandle(SQL_HANDLE_STMT, HNDLGichromidSearchGi) != SQL_SUCCESS)
	 print_err(HNDLGichromidSearchGi);
   if (SQLFreeHandle(SQL_HANDLE_STMT, HNDLGichromidDeleteChromid) != SQL_SUCCESS)
	 print_err(HNDLGichromidDeleteChromid);
   if (SQLFreeHandle(SQL_HANDLE_STMT, HNDLGichromidDeleteGi) != SQL_SUCCESS)
	 print_err(HNDLGichromidDeleteGi);
   if (SQLFreeHandle(SQL_HANDLE_STMT, HNDLContigchromidInsert) != SQL_SUCCESS)
	 print_err(HNDLContigchromidInsert);
   if (SQLFreeHandle(SQL_HANDLE_STMT, HNDLContigchromidUpdate) != SQL_SUCCESS)
	 print_err(HNDLContigchromidUpdate);
   if (SQLFreeHandle(SQL_HANDLE_STMT, HNDLContigchromidDeleteChromid) != SQL_SUCCESS)
	 print_err(HNDLContigchromidDeleteChromid);
   if (SQLFreeHandle(SQL_HANDLE_STMT, HNDLGichromoInsert) != SQL_SUCCESS)
	 print_err(HNDLGichromoInsert);
   if (SQLFreeHandle(SQL_HANDLE_STMT, HNDLContigchromoInsert) != SQL_SUCCESS)
	 print_err(HNDLContigchromoInsert);
   if (SQLFreeHandle(SQL_HANDLE_STMT, HNDLContigchromidDeleteGi) != SQL_SUCCESS)
	 print_err(HNDLContigchromidDeleteGi);
   if (SQLFreeHandle(SQL_HANDLE_STMT, HNDLGichromidDeleteContigGi) != SQL_SUCCESS)
	 print_err(HNDLGichromidDeleteContigGi);
   if (SQLFreeHandle(SQL_HANDLE_STMT, HNDLContigchromidDeleteTopGi) != SQL_SUCCESS)
	 print_err(HNDLContigchromidDeleteTopGi);
   if (SQLFreeHandle(SQL_HANDLE_STMT, HNDLTaxgiSearchKloodgeType) != SQL_SUCCESS)
	 print_err(HNDLTaxgiSearchKloodgeType);
  
  return TRUE;
}

Boolean LIBCALL OpenCHROM(CharPtr name, Boolean Create)
{ return TRUE;}

Boolean LIBCALL CloseCHROM(void)
{
   return TRUE;
}

/* initialization of CHROM for insertion of records:
 *  1) Creates a handle for the insertion command and
 *  2) binds the parameters.
 *  This function is called by InitCodeBase. To insert into
 * the CHROM database, use AppendRecordCHROM */
Boolean InitCHROM(void)
{
   SQLRETURN retcode;
   CharPtr pSQL_stmt = (Char *) Malloc(sizeof(Char) * 150);
   if(pSQL_stmt == NULL)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[InitCHROM]: Could not allocate space");
     return FALSE;
   }

   /***********************************************
   ** insert
   ************************************************/
   sprintf((char *) pSQL_stmt, "insert ignore into %s.chrom( "
		   "taxid, chromid, chromflag, access, name, projectid, "
           "chromnum ) values(?, ?, ?, ?, ?, ?, ?);", database);


   if( !PrepStmt( &HNDLChrom, pSQL_stmt, 7,
				  'd', &chrom_taxid.val, 0,
				  'd', &chrom_chromid.val, 0,
				  'd', &chrom_chromfl.val, 0,
				  's', chrom_access.val, sizeof( chrom_access.val ),
				  's', chrom_name.val, 1024,
				  'd', chrom_projectid.val, 0,
				  's', chrom_chromnum.val, sizeof( chrom_chromnum.val ) ) )
   {
	 Free( pSQL_stmt );
	 return FALSE;
   }
				  
  /***********************************************
   ** Delete based on accession
   ************************************************/
   sprintf((char *) pSQL_stmt, "DELETE FROM %s.chrom WHERE access = ?;",
		   database );

   if(CreateHandle(&HNDLChromDeleteAccess, pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitCHROM]: Could not create HNDLChromDeleteAccess.");
      Free(pSQL_stmt);
      return FALSE;
   }

   retcode = SQLBindParameter(HNDLChromDeleteAccess, 1, SQL_PARAM_INPUT, 
							  SQL_C_CHAR, SQL_VARCHAR, 20, 0, 
							  chrom_access.val, sizeof(chrom_access.val), NULL);

   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitCHROM]: Could not bind parameter 1, access");
      Free(pSQL_stmt);
	  SQLFreeHandle(SQL_HANDLE_STMT, HNDLChromDeleteAccess);
      return FALSE;
   }

   /***********************************************
   ** search kloodge by access. Used in SHoundGetKloodgeFromAccess
   ************************************************/ 
   sprintf((char *) pSQL_stmt, "select kloodge from %s.chrom where access=?;", database);
   if(CreateHandle(&HNDLChromSearchAccess, pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitCHROM]: Could not create handle.");
      Free(pSQL_stmt);
      return FALSE;
   }

   retcode = SQLBindParameter(HNDLChromSearchAccess, 1, SQL_PARAM_INPUT, SQL_C_CHAR, 
							  SQL_VARCHAR, 20, 0, chrom_access.val, sizeof(chrom_access.val), NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitCHROM]: Could not bind parameter 1 for HNDLChromSearchAccess");
      Free(pSQL_stmt);
	  SQLFreeHandle(SQL_HANDLE_STMT, HNDLChromSearchAccess);
      return FALSE;
   }

 /***********************************************
   ** search for kloodge by chromid. Used in SearchyCHROMBychromid
   ************************************************/ 
   sprintf((char *) pSQL_stmt, "select kloodge from %s.chrom where chromid=?;", database);
   if(CreateHandle(&HNDLChromSearchChromId, pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitCHROM]: Could not create handle.");
      Free(pSQL_stmt);
      return FALSE;
   }

   retcode = SQLBindParameter(HNDLChromSearchChromId, 1, SQL_PARAM_INPUT, SQL_C_SLONG, 
							  SQL_INTEGER, 0, 0, &chrom_chromid.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitCHROM]: Could not bind parameter 1 for HNDLChromSearchChromId");
      Free(pSQL_stmt);
	  SQLFreeHandle(SQL_HANDLE_STMT, HNDLChromSearchChromId);
      return FALSE;
   }

/***********************************************
   ** search for kloodge by chromnum. Used in 
	 SearchCHROMByChromNum
   ************************************************/ 
   sprintf((char *) pSQL_stmt, "select kloodge, chromid from "
		   "%s.chrom where chromnum=? and taxid = ?;", database);
   if(CreateHandle(&HNDLChromSearchChromNum, pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitCHROM]: Could not create handle.");
      Free(pSQL_stmt);
      return FALSE;
   }

   retcode = SQLBindParameter(HNDLChromSearchChromNum, 1, 
							  SQL_PARAM_INPUT, SQL_C_CHAR, 
							  SQL_VARCHAR, 0, 0, chrom_chromnum.val,
							  sizeof( chrom_chromnum.val ), NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitCHROM]: Could not bind "
				"parameter 1 for HNDLChromSearchChromNum");
      Free(pSQL_stmt);
	  SQLFreeHandle(SQL_HANDLE_STMT, HNDLChromSearchChromNum );
      return FALSE;
   }

   retcode = SQLBindParameter(HNDLChromSearchChromNum, 2, 
							  SQL_PARAM_INPUT, SQL_C_SLONG, 
							  SQL_INTEGER, 0, 0, &chrom_taxid.val,
							  0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitCHROM]: Could not bind "
				"parameter 2 for HNDLChromSearchChromNum");
      Free(pSQL_stmt);
	  SQLFreeHandle(SQL_HANDLE_STMT, HNDLChromSearchChromNum );
      return FALSE;
   }

/***********************************************
   Search taxgi by kloodge and type. Used by api functions 
   ************************************************/ 
   sprintf((char *) pSQL_stmt, "select t.gi from taxgi t, "
		   "chrom c where t.kloodge = c.kloodge and t.type = "
		   "? and c.chromid = ?;");
 
   if( !PrepStmt( &HNDLTaxgiSearchKloodgeType, pSQL_stmt, 2, 
				  's', taxgi_type.val, sizeof( taxgi_type.val ), 
				  'd', &chrom_chromid.val, 0 ) )
				  
				  
   {
	 Free( pSQL_stmt );
	 return FALSE; 
   }

   
/***********************************************
   Search taxgi by negative and type. Used by api functions
   to retrieve dna and proteins that have not been 
   assigned to a specific chromosome.
   ************************************************/ 
   sprintf((char *) pSQL_stmt, "select gi from taxgi "
		   "where kloodge = ? and type = ?", database);

   if(CreateHandle(&HNDLTaxgiUnknownChromosome, pSQL_stmt, hdbc, &henv) != 
	  SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitCHROM]: Could not create "
				"handle HNDLTaxgiUnknownChromosome.");
      Free(pSQL_stmt);
      return FALSE;
   }

   retcode = SQLBindParameter(HNDLTaxgiUnknownChromosome, 2, SQL_PARAM_INPUT, SQL_C_SLONG, 
							  SQL_INTEGER, 0, 0, &taxgi_kloodge.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitCHROM]: Could not bind parameter 2 "
				"for HNDLTaxgiUnknownChromosome");
      Free(pSQL_stmt);
	  SQLFreeHandle(SQL_HANDLE_STMT, HNDLTaxgiUnknownChromosome);
      return FALSE;
   }

   retcode = SQLBindParameter(HNDLTaxgiUnknownChromosome, 2, SQL_PARAM_INPUT, 
							  SQL_C_CHAR, SQL_VARCHAR, 0, 0, 
							  taxgi_type.val, sizeof(taxgi_type.val), NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitCHROM]: Could not bind parameter 2 "
				"for HNDLTaxgiUnknownChromosome");
	  Free(pSQL_stmt);
	  SQLFreeHandle(SQL_HANDLE_STMT, HNDLTaxgiUnknownChromosome);
      return FALSE;
   }

   Free(pSQL_stmt);
   return TRUE;
}


Boolean InitGICHROMID(void)
{
   SQLRETURN retcode;
   CharPtr pSQL_stmt = (Char *) malloc(sizeof(Char) * 150);
   if(pSQL_stmt == NULL)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[InitGICHROMID]: Could not allocate space");
     return FALSE;
   }

   /***********************************************
   ** insert
   ************************************************/
      sprintf((char *) pSQL_stmt, "insert ignore into %s.gichromid(gi, "
		   "contiggi, chromid ) values(?, ?, ?);", database);
   if(CreateHandle(&HNDLGichromidInsert, pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitGICHROMID]: Could not create handle.");
      free(pSQL_stmt);
      return FALSE;
   }

   retcode = SQLBindParameter(HNDLGichromidInsert, 1, SQL_PARAM_INPUT, 
							  SQL_C_SLONG, SQL_INTEGER, 0, 0,
							  &gichromid_gi.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitGICHROMID]: Could not bind parameter 1");
      free(pSQL_stmt);
	  SQLFreeHandle(SQL_HANDLE_STMT, HNDLGichromidInsert);
      return FALSE;
   }

   retcode = SQLBindParameter(HNDLGichromidInsert, 2, SQL_PARAM_INPUT, 
							  SQL_C_SLONG, SQL_INTEGER, 0, 0,
							  &gichromid_contiggi.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitGICHROMID]: Could not bind parameter 1");
      free(pSQL_stmt);
	  SQLFreeHandle(SQL_HANDLE_STMT, HNDLGichromidInsert);
      return FALSE;
   }
   retcode = SQLBindParameter(HNDLGichromidInsert, 3, SQL_PARAM_INPUT, 
							  SQL_C_SLONG, SQL_INTEGER, 0, 0,
							  &gichromid_chromid.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitGICHROMID]: Could not bind parameter 2");
      free(pSQL_stmt);
	  SQLFreeHandle(SQL_HANDLE_STMT, HNDLGichromidInsert);
      return FALSE;
   }

/***********************************************
   ** replace
   ************************************************/
   sprintf((char *) pSQL_stmt, "replace into %s.gichromid(gi, "
		   "contiggi, chromid ) values(?,?,?);", database);
   if(CreateHandle(&HNDLGichromidReplace, pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitGICHROMID]: Could not create handle.");
      free(pSQL_stmt);
      return FALSE;
   }

   retcode = SQLBindParameter(HNDLGichromidReplace, 1, SQL_PARAM_INPUT, 
							  SQL_C_SLONG, SQL_INTEGER, 0, 0,
							  &gichromid_gi.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitGICHROMID]: Could not bind parameter 1");
      free(pSQL_stmt);
	  SQLFreeHandle(SQL_HANDLE_STMT, HNDLGichromidReplace);
      return FALSE;
   }

   retcode = SQLBindParameter(HNDLGichromidReplace, 2, SQL_PARAM_INPUT, 
							  SQL_C_SLONG, SQL_INTEGER, 0, 0,
							  &gichromid_contiggi.val, 0, NULL);

   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitGICHROMID]: Could not bind "
				"parameter 2");
      free(pSQL_stmt);
	  SQLFreeHandle(SQL_HANDLE_STMT, HNDLGichromidReplace);
      return FALSE;
   }

   retcode = SQLBindParameter(HNDLGichromidReplace, 3, SQL_PARAM_INPUT, 
							  SQL_C_SLONG, SQL_INTEGER, 0, 0,
							  &gichromid_chromid.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitGICHROMID]: Could not bind "
				"parameter 3");
      free(pSQL_stmt);
	  SQLFreeHandle(SQL_HANDLE_STMT, HNDLGichromidReplace);
      return FALSE;
   }

/***********************************************
   ** delete based on contiggi
   ************************************************/
   sprintf( (char *) pSQL_stmt, "delete from %s.gichromid "
			"where contiggi = ?", database );
			

   if( CreateHandle( &HNDLGichromidDeleteContigGi, pSQL_stmt, hdbc, &henv) != 
	   SQL_SUCCESS)
   {
	 ErrPostEx(SEV_ERROR, 0, 0, "[InitGICHROMID]: "
			   "Could not create HNDLGichromidDeleteGi.");
	 free(pSQL_stmt);
	 return FALSE;
   }

   retcode = SQLBindParameter(HNDLGichromidDeleteContigGi, 1, SQL_PARAM_INPUT, 
							  SQL_C_SLONG, SQL_INTEGER, 0, 0,
							  &gichromid_contiggi.val, 0, NULL);

   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitGICHROMID]: "
				"Could not bind parameter 1 gi");
      free(pSQL_stmt);
	  SQLFreeHandle(SQL_HANDLE_STMT, HNDLGichromidDeleteContigGi);
      return FALSE;
   }

 /***********************************************
   ** delete based on gi
   ************************************************/
   sprintf( (char *) pSQL_stmt, "delete from %s.gichromid "
			"where gi = ?", database );
			

   if( CreateHandle( &HNDLGichromidDeleteGi, pSQL_stmt, hdbc, &henv) != 
	   SQL_SUCCESS)
   {
	 ErrPostEx(SEV_ERROR, 0, 0, "[InitGICHROMID]: "
			   "Could not create HNDLGichromidDeleteGi.");
	 free(pSQL_stmt);
	 return FALSE;
   }

   retcode = SQLBindParameter(HNDLGichromidDeleteGi, 1, SQL_PARAM_INPUT, 
							  SQL_C_SLONG, SQL_INTEGER, 0, 0,
							  &gichromid_gi.val, 0, NULL);

   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitGICHROMID]: "
				"Could not bind parameter 1 gi");
      free(pSQL_stmt);
	  SQLFreeHandle(SQL_HANDLE_STMT, HNDLGichromidDeleteGi);
      return FALSE;
   }


 /***********************************************
   ** delete based on chromid
   ************************************************/
   sprintf( (char *) pSQL_stmt, "delete from %s.gichromid "
			"where chromid = ?", database );
			

   if( CreateHandle( &HNDLGichromidDeleteChromid, pSQL_stmt, hdbc, &henv) != 
	   SQL_SUCCESS)
   {
	 ErrPostEx(SEV_ERROR, 0, 0, "[InitGICHROMID]: "
			   "Could not create HNDLGichromidDeleteChromid.");
	 free(pSQL_stmt);
	 return FALSE;
   }

   retcode = SQLBindParameter(HNDLGichromidDeleteChromid, 1, SQL_PARAM_INPUT, 
							  SQL_C_SLONG, SQL_INTEGER, 0, 0,
							  &gichromid_chromid.val, 0, NULL);

   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitGICHROMID]: "
				"Could not bind parameter 1 chromid");
      free(pSQL_stmt);
	  SQLFreeHandle(SQL_HANDLE_STMT, HNDLGichromidDeleteChromid);
      return FALSE;
   }

 /***********************************************
   ** Search based on gi
   ************************************************/
   sprintf((char *) pSQL_stmt, "select chromid, contiggi from %s.gichromid "
		   "where gi = ?;", database);

   if(CreateHandle(&HNDLGichromidSearchGi, pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitGICHROMID]: "
				"Could not create handle.");
      free(pSQL_stmt);
      return FALSE;
   }

   retcode = SQLBindParameter(HNDLGichromidSearchGi, 1, SQL_PARAM_INPUT, 
							  SQL_C_SLONG, SQL_INTEGER, 0, 0,
							  &gichromid_gi.val, 0, NULL);

   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitGICHROMID]: Could not bind parameter 1");
      free(pSQL_stmt);
	  SQLFreeHandle(SQL_HANDLE_STMT, HNDLGichromidSearchGi);
      return FALSE;

   }

/***********************************************
   ** Search based on contiggi
   ************************************************/
   sprintf((char *) pSQL_stmt, "select gi from %s.gichromid "
		   "where contiggi = ?;", database);

   if(CreateHandle(&HNDLGichromidSearchContiggi, pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitGICHROMID]: "
				"Could not create handle.");
      free(pSQL_stmt);
      return FALSE;
   }

   retcode = SQLBindParameter(HNDLGichromidSearchContiggi, 1, SQL_PARAM_INPUT, 
							  SQL_C_SLONG, SQL_INTEGER, 0, 0,
							  &gichromid_gi.val, 0, NULL);

   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitGICHROMID]: Could not bind parameter 1");
      free(pSQL_stmt);
	  SQLFreeHandle(SQL_HANDLE_STMT, HNDLGichromidSearchContiggi);
      return FALSE;

   }

   free(pSQL_stmt);
   return TRUE;
}


Boolean InitCONTIGCHROMID(void)
{
   SQLRETURN retcode;
   CharPtr pSQL_stmt = (Char *) malloc(sizeof(Char) * 150);
   if(pSQL_stmt == NULL)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[InitCONTIGCHROMID]: Could not allocate space");
     return FALSE;
   }

   /***********************************************
   ** insert (replace)
   ************************************************/
   sprintf((char *) pSQL_stmt, "REPLACE INTO %s.contigchromid( contiggi, "
		   "topgi, chromid, changed ) values(?,?,?,?);", database);
   if(CreateHandle(&HNDLContigchromidInsert, pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitCONTIGCHROMID]: Could not create handle.");
      free(pSQL_stmt);
      return FALSE;
   }

   retcode = SQLBindParameter(HNDLContigchromidInsert, 1, SQL_PARAM_INPUT, 
							  SQL_C_SLONG, SQL_INTEGER, 0, 0,
							  &contigchromid_contiggi.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitCONTIGCHROMID]: Could not bind "
				"parameter 1 contiggi" );

      free(pSQL_stmt);
	  SQLFreeHandle(SQL_HANDLE_STMT, HNDLContigchromidInsert);
      return FALSE;
   }

   retcode = SQLBindParameter(HNDLContigchromidInsert, 2, SQL_PARAM_INPUT, 
							  SQL_C_SLONG, SQL_INTEGER, 0, 0,
							  &contigchromid_topgi.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitCONTIGCHROMID]: Could not bind "
				"parameter 2");
      free(pSQL_stmt);
	  SQLFreeHandle(SQL_HANDLE_STMT, HNDLContigchromidInsert);
      return FALSE;
   }

   retcode = SQLBindParameter(HNDLContigchromidInsert, 3, SQL_PARAM_INPUT, 
							  SQL_C_SLONG, SQL_INTEGER, 0, 0,
							  &contigchromid_chromid.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitCONTIGCHROMID]: Could not bind "
				"parameter 3");
      free(pSQL_stmt);
	  SQLFreeHandle(SQL_HANDLE_STMT, HNDLContigchromidInsert);
      return FALSE;
   }

   retcode = SQLBindParameter(HNDLContigchromidInsert, 4, SQL_PARAM_INPUT, 
							  SQL_C_SLONG, SQL_INTEGER, 0, 0,
							  &contigchromid_changed.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitCONTIGCHROMID]: Could not bind parameter 4");
      free(pSQL_stmt);
	  SQLFreeHandle(SQL_HANDLE_STMT, HNDLContigchromidInsert);
      return FALSE;
   }


   /***********************************************
   ** Update based on contiggi
   ************************************************/
   sprintf((char *) pSQL_stmt, "UPDATE %s.contigchromid SET "
		   "changed = ? WHERE contiggi = ?;", database);

   if( !PrepStmt( &HNDLContigchromidUpdate, pSQL_stmt, 2,
				 'd', &contigchromid_changed.val, 0,
				 'd', &contigchromid_contiggi.val, 0 ) )
   {
	 Free( pSQL_stmt );
	 return FALSE;
   }
   /*
   if(CreateHandle(&HNDLContigchromidUpdate, pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitCONTIGCHROMID]: Could not create handle.");
      free(pSQL_stmt);
      return FALSE;
   }

   retcode = SQLBindParameter(HNDLContigchromidUpdate, 1, SQL_PARAM_INPUT, 
							  SQL_C_SLONG, SQL_INTEGER, 0, 0,
							  &contigchromid_changed.val, 0, NULL);
   
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitCONTIGCHROMID]: Could not bind parameter 2");
      free(pSQL_stmt);
	  SQLFreeHandle(SQL_HANDLE_STMT, HNDLContigchromidUpdate);
      return FALSE;
   }

   retcode = SQLBindParameter(HNDLContigchromidUpdate, 2, SQL_PARAM_INPUT, 
							  SQL_C_SLONG, SQL_INTEGER, 0, 0,
							  &contigchromid_contiggi.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitCONTIGCHROMID]: Could not bind parameter 1");
      free(pSQL_stmt);
	  SQLFreeHandle(SQL_HANDLE_STMT, HNDLContigchromidUpdate);
      return FALSE;
   }
   */
   /***********************************************
   ** delete based on topgi
   ************************************************/
   sprintf( (char *) pSQL_stmt, "delete from %s.contigchromid "
			"where topgi = ?", database );
			

   if( CreateHandle( &HNDLContigchromidDeleteTopGi, pSQL_stmt, hdbc, &henv) != 
	   SQL_SUCCESS)
   {
	 ErrPostEx(SEV_ERROR, 0, 0, "[InitCONTIGCHROMID]: "
			   "Could not create HNDLContigchromidDeleteGi.");
	 free(pSQL_stmt);
	 return FALSE;
   }

   retcode = SQLBindParameter(HNDLContigchromidDeleteTopGi, 1, SQL_PARAM_INPUT, 
							  SQL_C_SLONG, SQL_INTEGER, 0, 0,
							  &contigchromid_topgi.val, 0, NULL);

   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitCONTIGCHROMID]: "
				"Could not bind parameter 1 gi");
      free(pSQL_stmt);
	  SQLFreeHandle(SQL_HANDLE_STMT, HNDLContigchromidDeleteTopGi);
      return FALSE;
   }
 
  
/***********************************************
   ** delete based on gi
   ************************************************/
   sprintf( (char *) pSQL_stmt, "delete from %s.contigchromid "
			"where contiggi = ?", database );
			

   if( CreateHandle( &HNDLContigchromidDeleteGi, pSQL_stmt, hdbc, &henv) != 
	   SQL_SUCCESS)
   {
	 ErrPostEx(SEV_ERROR, 0, 0, "[InitCONTIGCHROMID]: "
			   "Could not create HNDLContigchromidDeleteGi.");
	 free(pSQL_stmt);
	 return FALSE;
   }

   retcode = SQLBindParameter(HNDLContigchromidDeleteGi, 1, SQL_PARAM_INPUT, 
							  SQL_C_SLONG, SQL_INTEGER, 0, 0,
							  &contigchromid_contiggi.val, 0, NULL);

   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitCONTIGCHROMID]: "
				"Could not bind parameter 1 gi");
      free(pSQL_stmt);
	  SQLFreeHandle(SQL_HANDLE_STMT, HNDLContigchromidDeleteGi);
      return FALSE;
   }


 /***********************************************
   ** delete based on chromid
   ************************************************/
   sprintf( (char *) pSQL_stmt, "delete from %s.contigchromid "
			"where chromid = ?", database );
			

   if( CreateHandle( &HNDLContigchromidDeleteChromid, pSQL_stmt, hdbc, &henv) != 
	   SQL_SUCCESS)
   {
	 ErrPostEx(SEV_ERROR, 0, 0, "[InitCONTIGCHROMID]: "
			   "Could not create HNDLContigchromidDeleteChromid.");
	 free(pSQL_stmt);
	 return FALSE;
   }

   retcode = SQLBindParameter(HNDLContigchromidDeleteChromid, 1, SQL_PARAM_INPUT, 
							  SQL_C_SLONG, SQL_INTEGER, 0, 0,
							  &contigchromid_chromid.val, 0, NULL);

   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitCONTIGCHROMID]: "
				"Could not bind parameter 1 chromid");
      free(pSQL_stmt);
	  SQLFreeHandle(SQL_HANDLE_STMT, HNDLContigchromidDeleteChromid);
      return FALSE;
   }
/***********************************************
   ** Search based on topgi
   ************************************************/
   sprintf((char *) pSQL_stmt, "select contiggi from %s.contigchromid "
		   "where topgi = ?;", database);

   if(CreateHandle(&HNDLContigchromidSearchTopGi, pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitGICHROMID]: "
				"Could not create handle.");
      free(pSQL_stmt);
      return FALSE;
   }

   retcode = SQLBindParameter(HNDLContigchromidSearchTopGi, 1, SQL_PARAM_INPUT, 
							  SQL_C_SLONG, SQL_INTEGER, 0, 0,
							  &contigchromid_topgi.val, 0, NULL);

   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitGICHROMID]: Could not bind parameter 1");
      free(pSQL_stmt);
	  SQLFreeHandle(SQL_HANDLE_STMT, HNDLContigchromidSearchTopGi);
      return FALSE;

   }

   free(pSQL_stmt);
   return TRUE;
}

Boolean InitGICHROMOSOME(void)
{
   SQLRETURN retcode;
   CharPtr pSQL_stmt = (Char *) malloc(sizeof(Char) * 150);
   if(pSQL_stmt == NULL)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[InitGICHROMOSOME]: "
			   "Could not allocate space");
     return FALSE;
   }

   /***********************************************
   ** insert
   ************************************************/
   sprintf((char *) pSQL_stmt, "insert ignore into %s.gichromosome(gi, "
		   "chromnum ) values(?, ?);", database);
   if(CreateHandle(&HNDLGichromoInsert, pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitGICHROMOSOME]: Could not create handle.");
      free(pSQL_stmt);
      return FALSE;
   }

   retcode = SQLBindParameter(HNDLGichromoInsert, 1, SQL_PARAM_INPUT, 
							  SQL_C_SLONG, SQL_INTEGER, 0, 0,
							  &gichrom_gi.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitGICHROMOSOME]:"
				"Could not bind parameter 1 gi");
      free(pSQL_stmt);
	  SQLFreeHandle(SQL_HANDLE_STMT, HNDLGichromoInsert);
      return FALSE;
   }

   retcode = SQLBindParameter(HNDLGichromoInsert, 2, SQL_PARAM_INPUT, 
							  SQL_C_CHAR, SQL_VARCHAR, 0, 0,
							  gichrom_chromnum.val, 
							  sizeof( gichrom_chromnum.val), NULL);

   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitGICHROMOSOME]: "
				"Could not bind parameter 2 chromnum");
      free(pSQL_stmt);
	  SQLFreeHandle(SQL_HANDLE_STMT, HNDLGichromoInsert);
      return FALSE;
   }

   free(pSQL_stmt);
   return TRUE;
}

Boolean InitCONTIGCHROMOSOME(void)
{
  SQLRETURN retcode;
   CharPtr pSQL_stmt = (Char *) malloc(sizeof(Char) * 150);
   if(pSQL_stmt == NULL)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[InitCONTIGCHROMOSOME]: "
			   "Could not allocate space");
     return FALSE;
   }

   /***********************************************
   ** insert
   ************************************************/
   sprintf((char *) pSQL_stmt, "insert ignore into "
		   "%s.contigchromosome(contiggi, chromnum"
		   " ) values(?, ?);", database);
   if(CreateHandle(&HNDLContigchromoInsert, pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitCONTIGCHROMOSOME]: Could not create handle.");
      free(pSQL_stmt);
      return FALSE;
   }

   retcode = SQLBindParameter(HNDLContigchromoInsert, 1, SQL_PARAM_INPUT, 
							  SQL_C_SLONG, SQL_INTEGER, 0, 0,
							  &contigchrom_contiggi.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitCONTIGCHROMOSOME]:"
				"Could not bind parameter 1 gi");
      free(pSQL_stmt);
	  SQLFreeHandle(SQL_HANDLE_STMT, HNDLContigchromoInsert);
      return FALSE;
   }

   retcode = SQLBindParameter(HNDLContigchromoInsert, 2, SQL_PARAM_INPUT, 
							  SQL_C_CHAR, SQL_VARCHAR, 0, 0,
							  contigchrom_chromnum.val, 
							  sizeof( contigchrom_chromnum.val), NULL);

   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitCONTIGCHROMOSOME]: "
				"Could not bind parameter 2 chromnum");
      free(pSQL_stmt);
	  SQLFreeHandle(SQL_HANDLE_STMT, HNDLContigchromoInsert);
      return FALSE;
   }

   free(pSQL_stmt);
   return TRUE;
}


/*===============================================================
  CHROM FUNCTIONS
================================================================*/
Boolean LIBCALL AppendRecordCHROM(StChromPtr pch) {

   SQLRETURN retcode;
   
   if(pch == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "[AppendRecordCHROM]: NULL parameter.");
      return FALSE;
    }

   chrom_taxid.val = pch->taxid;
   chrom_chromid.val = pch->chromid;
   chrom_chromfl.val = pch->chromfl;
   sprintf((char *)chrom_access.val,"%s", IsNullStr(pch->access));
   sprintf((char *)chrom_name.val,"%s", IsNullStr(pch->name));
   sprintf((char *)chrom_chromnum.val, "%s", IsNullStr(pch->chromnum ));

  /* execute statment */
   retcode = SQLExecute(HNDLChrom);
   if (retcode != SQL_SUCCESS)
   {
	  ErrPostEx(SEV_ERROR,0,0, "[AppendRecordCHROM]: unable to execute CHROM insert statement. (taxid=%ld)",chrom_taxid.val);
	  LogSQLInfo(HNDLChrom);
      return FALSE;
   }

	return TRUE;
}
/*******************************************************/
/*Given a pointer to the asn structure that represents */
/*  an accdb record this function pulls out the        */
/*  taxid from the asn structure and                   */
/*  retrieves all the records associated with that key */
/*Returns : 1 if successful, 0 if fails, -1 if no data */
/*******************************************************/

Int2 LIBCALL SearchCHROM (StChromPtr PNTR ppch)
{
   StChromPtr pch = NULL;                /* linked list to hold return values */
   SQLINTEGER iKey = 0, iLength = 0;           /* search key and length of key */
   CharPtr pKey = NULL;                  /* search key for strings */
   Char *pSQL_stmt;                      /* SQL select statement */
   SQLHANDLE hstmt;                      /* handle to select statement */
   SQLRETURN retcode;                    /* return values for odbc layer */
   SQLINTEGER fetchCount = 0;

   if (ppch == NULL)
   {
       ErrPostEx(SEV_ERROR,0,0, "[SearchCHROM]: StAccdbPtr is NULL.");
       return 0;
   }
   pch = *ppch;

   /* allocate space for the select statement */
   pSQL_stmt = (char*) malloc(sizeof(char) * 150);
   if(pSQL_stmt == NULL)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[SearchCHROM]: Could not allocate space");
      return 0;
   }
   /* fill in the select statement based on the search key passed in.
      database is a global variable initialized in ConnectToDb. The
      value of database is set in the ini file. eg database = seqhound. */
   if (pch->taxid > 0)
   {
      iKey = pch->taxid;
      sprintf((char*)pSQL_stmt, "select taxid, chromid, chromflag, access, name, kloodge from %s.chrom where taxid=?;", database);
   }
   else if (pch->access != NULL) {
	  pKey = pch->access;
      sprintf((char*)pSQL_stmt, "select taxid, chromid, chromflag, access, name, kloodge from %s.chrom where access=?;", database);
   }
   else{
      ErrPostEx(SEV_ERROR, 0, 0, "[SearchCHROM]: No search key found.");
      free(pSQL_stmt);
      return 0;
   }

   /* create handle to statement, once handle exists we can free char* */
   if(CreateHandle(&hstmt, pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[SearchCHROM]: Could not create handle.");
      free(pSQL_stmt);
      return 0;
   }
   free(pSQL_stmt);


   /* bind the search key */
   if (iKey > 0)
      retcode = SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &iKey, 0, NULL);
   else
      retcode = SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_DEFAULT, SQL_VARCHAR, 20, 0, pKey, 0, NULL);

   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[SearchCHROM]: Could not bind parameter");
	  SQLFreeStmt(hstmt, SQL_DROP);
      return 0;
   }


   /* execute the select statement */
   retcode = SQLExecute(hstmt);
   if (retcode != SQL_SUCCESS)
   {
	  ErrPostEx(SEV_ERROR,0,0, "[SearchCHROM]: Could not execute select statement.");
	  LogSQLInfo(hstmt);
      SQLFreeStmt(hstmt, SQL_DROP);
      return 0;
   }

   /* binding our results from the database */

   retcode = SQLBindCol(hstmt, 1, SQL_C_SLONG, &chrom_taxid.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[SearchCHROM]: Could not bind column 1.");
	 SQLFreeStmt(hstmt, SQL_DROP);
     return 0;
   }
   retcode = SQLBindCol(hstmt, 2, SQL_C_SLONG, &chrom_chromid.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[SearchCHROM]: Could not bind column 2.");
	 SQLFreeStmt(hstmt, SQL_DROP);
     return 0;
   }
   retcode = SQLBindCol(hstmt, 3, SQL_C_SLONG, &chrom_chromfl.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[SearchCHROM]: Could not bind column 3.");
	 SQLFreeStmt(hstmt, SQL_DROP);
     return 0;
   }
   retcode = SQLBindCol(hstmt, 4, SQL_C_CHAR, chrom_access.val, 20, (int*)&chrom_access.len);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[SearchCHROM]: Could not bind column 4.");
	 SQLFreeStmt(hstmt, SQL_DROP);
     return 0;
   }
   retcode = SQLBindCol(hstmt, 5, SQL_C_CHAR, chrom_name.val, TEXT_LENGTH, (int*)&chrom_name.len);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[SearchCHROM]: Could not bind column 5.");
	 SQLFreeStmt(hstmt, SQL_DROP);
     return 0;
   }

   retcode = SQLBindCol(hstmt, 6, SQL_C_SLONG, &chrom_kloodge.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[SearchCHROM]: Could not bind column 6, kloodge.");
	 SQLFreeStmt(hstmt, SQL_DROP);
     return 0;
   }

   /* fetch result and save in the linked list */
   while((retcode=SQLFetch(hstmt)) == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
   {
      fetchCount++;
      if (fetchCount>1)
      {
         pch->next = StChromNew();
         pch = pch->next;
      }

      pch->taxid = chrom_taxid.val;
      pch->chromid = chrom_chromid.val;
      pch->chromfl = chrom_chromfl.val;
	  pch->kloodge = chrom_kloodge.val;
      if(chrom_access.len >0 ){
         pch->access = StringSave((char*)chrom_access.val);
      } else{
         pch->access = NULL;
      }
      if(chrom_name.len >0 ){
         pch->name = StringSave((char*)chrom_name.val);
      } else{
         pch->name = NULL;
      }

   }

   if(SQLFreeStmt(hstmt, SQL_DROP) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[SearchCHROM]: Could not free statement.");
      return 0;
   }

   if (fetchCount == 0)
	  return -1;

   return 1;
}

Int4 LIBCALL SearchCHROMByChromId(Int4 chromid )
{
  SQLRETURN retcode = SQL_SUCCESS;
  Int4 kloodge = 0;

  if( chromid == 0 )
  {
	ErrPostEx(SEV_ERROR,0,0, "[SearchCHROMByChromId] Invalid kloodge.");
	return kloodge;
  }

  chrom_chromid.val = chromid;

  retcode = SQLExecute(HNDLChromSearchChromId);

  if (retcode != SQL_SUCCESS  && retcode != SQL_SUCCESS_WITH_INFO)
  {
	ErrPostEx(SEV_ERROR,0,0, "[SearchCHROMByChromId]: unable to execute search statement. (chromid=%d)", chromid);
	LogSQLInfo(HNDLChromSearchChromId);
	return kloodge;
  }

  retcode = SQLBindCol(HNDLChromSearchChromId, 1, SQL_C_SLONG, 
					   &chrom_kloodge.val, 0, NULL);

  if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
  {
	ErrPostEx(SEV_ERROR, 0, 0, "[SearchCHROMByChromId]: Could not bind column 1.");
	return kloodge;
  }

  while((retcode=SQLFetch(HNDLChromSearchChromId)) == SQL_SUCCESS || 
		 retcode == SQL_SUCCESS_WITH_INFO)
  {
	kloodge = chrom_kloodge.val;
  }

   /* close the cursor and discard all pending results. */
   /* must do in order to re-use the handle created in InitCHROM */
  retcode = SQLFreeStmt(HNDLChromSearchChromId, SQL_CLOSE);
  if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
  {
	ErrPostEx(SEV_ERROR, 0, 0, "[SearchCHROMByChromId]: Error in closing cursor.");
	return 0;
  }

  return kloodge;

}

Int4 LIBCALL SearchCHROMByChromNum(CharPtr xmeNum, 
								   Int4 * chromId, Int4 taxid )
{
SQLRETURN retcode = SQL_SUCCESS;
  Int4 kloodge = 0;

  if( xmeNum == NULL || taxid <= 0)
  {
	ErrPostEx(SEV_ERROR,0,0, "[SearchCHROMByChromNum] "
			  "Invalid kloodge.");
	return kloodge;
  }

  sprintf( ( char * )chrom_chromnum.val, "%s", xmeNum );
  chrom_taxid.val = taxid;

  retcode = SQLExecute(HNDLChromSearchChromNum);

  if (retcode != SQL_SUCCESS  && retcode != SQL_SUCCESS_WITH_INFO)
  {
	ErrPostEx(SEV_ERROR,0,0, "[SearchCHROMByChromNum]: "
			  "unable to execute search statement. (chromnum=%s)",
			   xmeNum);
	LogSQLInfo(HNDLChromSearchChromNum);
	return kloodge;
  }

  retcode = SQLBindCol(HNDLChromSearchChromNum, 1, SQL_C_SLONG, 
					   &chrom_kloodge.val, 0, NULL);

  if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
  {
	ErrPostEx(SEV_ERROR, 0, 0, "[SearchCHROMByChromNum]: "
			  "Could not bind column 1.");
	return kloodge;
  }

 retcode = SQLBindCol(HNDLChromSearchChromNum, 2, SQL_C_SLONG, 
					   &chrom_chromid.val, 0, NULL);

  if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
  {
	ErrPostEx(SEV_ERROR, 0, 0, "[SearchCHROMByChromNum]: "
			  "Could not bind column 1.");
	return kloodge;
  }

  while((retcode=SQLFetch(HNDLChromSearchChromNum)) == SQL_SUCCESS || 
		 retcode == SQL_SUCCESS_WITH_INFO)
  {
	kloodge = chrom_kloodge.val;
	*chromId = chrom_chromid.val;
  }

   /* close the cursor and discard all pending results. */
   /* must do in order to re-use the handle created in InitCHROM */
  retcode = SQLFreeStmt(HNDLChromSearchChromNum, SQL_CLOSE);
  if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
  {
	ErrPostEx(SEV_ERROR, 0, 0, "[SearchCHROMByChromNum]: Error in closing cursor.");
	return 0;
  }

  return kloodge;
}

Boolean LIBCALL DeleteAccessInCHROM( CharPtr access )
{
  SQLRETURN retcode = SQL_SUCCESS;

  if( access == NULL )
  {
	ErrPostEx(SEV_ERROR,0,0, "[ DeleteAccessInCHROM] Access is NULL.");
	return FALSE;
  }

  sprintf((char *)chrom_access.val,"%s", IsNullStr(access));

  retcode = SQLExecute(HNDLChromDeleteAccess);
  if (retcode != SQL_SUCCESS)
  {
	ErrPostEx(SEV_ERROR,0,0, "[ DeleteAccessInCHROM]: unable to execute search statement. (access=%s)", 
			  access);
	LogSQLInfo(HNDLChromDeleteAccess);
	return FALSE;
  }

  if(SQLFreeStmt(HNDLChromDeleteAccess, SQL_CLOSE) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[SearchCHROM]: Could not free statement.");
      return FALSE;
   }
  return TRUE;
  
}



Boolean DeleteAllRecordsInCHROM()
{
   SQLHANDLE hstmt;
   Int2 sql_return = SQL_SUCCESS;

   CharPtr delete_stmt = (char *) malloc(sizeof(Char) * 150);
   if (delete_stmt == NULL)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[DeleteAllRecordsInCHROM]: Could not allocate space");
     free(delete_stmt);
     return FALSE;
   }
   sprintf((char *)delete_stmt, "delete from %s.chrom", database);

   if (CreateHandle(&hstmt, delete_stmt, hdbc, &henv) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[DeleteAllRecordsInCHROM]: Could not create handle");
      free(delete_stmt);
      return FALSE;
   }
   free(delete_stmt);

   sql_return = SQLExecute(hstmt);
   /* execute statment */
   if (sql_return != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR,0,0, "[DeleteAllRecordsInCHROM]:unable to execute delete statement.");
      LogSQLInfo(hstmt);
      SQLFreeStmt(hstmt, SQL_DROP);
      return FALSE;
   }

   SQLFreeStmt(hstmt, SQL_DROP);
   return TRUE;
}

void LIBCALL ResetStCHROM(StChromPtr PNTR ppch)
{
   StChromPtr pch = NULL;

   pch = *ppch;
   if(pch == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "ResetStCHROM: NULL parameter.");
      return;
    }

   pch->taxid = 0;
   pch->kloodge = 0;
   pch->chromfl = 0;
   pch->access = NULL;
   pch->name = NULL;

   return;
}

/*===============================================================
  GICHROMID FUNCTIONS
================================================================*/ 
Boolean LIBCALL AppendRecordGICHROMID( StGichromidPtr pRecord )
{
  SQLRETURN retcode;
   
  if(pRecord == NULL)
  {
	ErrPostEx(SEV_ERROR,0,0, "[AppendRecordGICHROMID]: NULL parameter.");
	return FALSE;
  }

  gichromid_gi.val       = pRecord->gi;
  gichromid_contiggi.val = pRecord->contiggi;
  gichromid_chromid.val  = pRecord->chromid;

  /* execute statment */
  retcode = SQLExecute(HNDLGichromidInsert);
  if (retcode != SQL_SUCCESS)
  {
	ErrPostEx(SEV_ERROR,0,0, "[AppendRecordGICHROMID]: "
			  "unable to execute insert statement. (gi=%ld, chromid = %d)",
			  gichromid_gi.val, gichromid_chromid.val);
	LogSQLInfo(HNDLGichromidInsert);
	return FALSE;
  }

  return TRUE;
}

Boolean LIBCALL ReplaceRecordGICHROMID( StGichromidPtr pRecord )
{
  SQLRETURN retcode;
   
  if(pRecord == NULL)
  {
	ErrPostEx(SEV_ERROR,0,0, "[ReplaceRecordGICHROMID]: NULL parameter.");
	return FALSE;
  }

  gichromid_gi.val       = pRecord->gi;
  gichromid_contiggi.val = pRecord->contiggi;
  gichromid_chromid.val  = pRecord->chromid;

  /* execute statment */
  retcode = SQLExecute(HNDLGichromidReplace);
  if (retcode < 0)
  {
	ErrPostEx(SEV_ERROR,0,0, "[ReplaceRecordGICHROMID]: "
			  "unable to execute replace statement. (gi=%ld, chromid = %d)",
			  gichromid_gi.val, gichromid_chromid.val);
	LogSQLInfo(HNDLGichromidReplace);
	return FALSE;
  }

  return TRUE;
}

/** AddChromIdsToGICHROMID()
 *
 *  Inserts gis from GICHROMOSOME int GICHROMID with the 
 *  appropriate chromid. This is only done for gis that are not
 *  already in gikloodge.
 *
 *  @return TRUE or FALSE.
 *
 */
Boolean LIBCALL AddChromIdsToGICHROMID()
{
  SQLHANDLE      hstmt;
  Int4           retcode = 0;

  CharPtr pSQL_stmt = ( Char * ) Malloc( sizeof( Char ) * 250 );

  if(pSQL_stmt == NULL)
  {
	ErrPostEx(SEV_ERROR, 0, 0, "[AddChromIdsToGICHROMID]: "
			  "Could not allocate space");
	return FALSE;
  }
   /*--------------------------------------------------------------
	 Insert the records.
	-------------------------------------------------------------*/
   sprintf( (char *) pSQL_stmt, "insert ignore into %s.gichromid ( "
			"gi, chromid ) select g.gi, c.chromid from gichromosome g "
			"inner join chrom c on c.chromnum = g.chromnum INNER JOIN "
			"taxgi t on g.gi = t.gi where c.taxid = t.taxid;",
			database );

   if(CreateHandle(&hstmt, pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
   {
	 ErrPostEx(SEV_ERROR, 0, 0, "[AddChromIdsToGICHROMID]: "
			   "Unable to create handle to insert records." );
	 Free(pSQL_stmt);
	 return FALSE;
   }

   /* execute statment */
   retcode = SQLExecute(hstmt);
   if (retcode != SQL_SUCCESS)
   {
	 ErrPostEx(SEV_ERROR,0,0, "[AddChromIdsToGICHROMID]: "
			   "unable to add records.");
	 LogSQLInfo(hstmt);
	 SQLFreeStmt(hstmt, SQL_DROP );
	 Free(pSQL_stmt);
	 return FALSE;
   }

   SQLFreeStmt(hstmt, SQL_DROP );

   Free(pSQL_stmt);
   return TRUE;

}

Boolean LIBCALL RemoveGenBankRecordsFromGICHROMID()
{
  SQLHANDLE      hstmt;
  Int4           retcode = 0;

  CharPtr pSQL_stmt = ( Char * ) Malloc( sizeof( Char ) * 250 );

  if(pSQL_stmt == NULL)
  {
	ErrPostEx(SEV_ERROR, 0, 0, "[RemoveGenBankRecordsFromGICHROMID]: "
			  "Could not allocate space");
	return FALSE;
  }

  /*--------------------------------------------------------------
	First create the temporary table that holds all non-refseq gis .
	-------------------------------------------------------------*/

  sprintf((char *) pSQL_stmt, "CREATE TEMPORARY TABLE gbgis SELECT "
		  "g.gi FROM %s.gichromid g LEFT JOIN %s.accdb a ON "
		  "a.gi = g.gi AND db = 'ref' WHERE a.gi IS NULL;",
		  database, database );

  if(CreateHandle(&hstmt, pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[RemoveGenBankRecordsFromGICHROMID]: "
				"Could not create handle to create temp table.");
      Free(pSQL_stmt);
      return FALSE;
   }

  /* execute statment */
  retcode = SQLExecute(hstmt);
  if (retcode != SQL_SUCCESS)
  {
	ErrPostEx(SEV_ERROR,0,0, "[RemoveGenBankRecordsFromGICHROMID]: "
			  "unable to create temporary table.");
	LogSQLInfo(hstmt);
	SQLFreeStmt(hstmt, SQL_DROP );
	Free(pSQL_stmt);
	return FALSE;
  }

  SQLFreeStmt(hstmt, SQL_DROP );

  /*--------------------------------------------------------------
	Delete the records from gichromid.
	-------------------------------------------------------------*/
  sprintf((char *) pSQL_stmt, "DELETE k FROM %s.gichromid k "
		  "INNER JOIN gbgis g ON k.gi = g.gi;", database );


  if(CreateHandle(&hstmt, pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[RemoveGenBankRecordsFromGICHROMID]: "
				"Could not create handle to delete records.");
      Free(pSQL_stmt);
      return FALSE;
   }

  /* execute statment */
  retcode = SQLExecute(hstmt);
  if (retcode != SQL_SUCCESS)
  {
	ErrPostEx(SEV_ERROR,0,0, "[RemoveGenBankRecordsFromGICHROMID]: "
			  "unable to delete records.");
	LogSQLInfo(hstmt);
	SQLFreeStmt(hstmt, SQL_DROP );
	Free(pSQL_stmt);
	return FALSE;
  }

  SQLFreeStmt(hstmt, SQL_DROP );
  /*--------------------------------------------------------------
	Drop the temporary table.
	-------------------------------------------------------------*/
  sprintf((char *) pSQL_stmt, "DROP TABLE gbgis;" );

  if(CreateHandle(&hstmt, pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
  {
	ErrPostEx(SEV_ERROR, 0, 0, "[RemoveGenBankRecordsFromGICHROMID]: "
				"Could not create handle to drop temporary table");
	Free(pSQL_stmt);
	return FALSE;
  }

  /* execute statment */
  retcode = SQLExecute(hstmt);
  if (retcode != SQL_SUCCESS)
  {
	ErrPostEx(SEV_ERROR,0,0, "[RemoveGenBankRecordsFromGICHROMID]: "
			  "Could not drop temporary table" );

	LogSQLInfo(hstmt);
	SQLFreeStmt(hstmt, SQL_DROP );
	Free(pSQL_stmt);
	return FALSE;
  }

  SQLFreeStmt(hstmt, SQL_DROP );
  Free(pSQL_stmt);
  return TRUE;
}

StGichromidPtr LIBCALL GetAllRecordsFromGICHROMID()
{
  StGichromidPtr pRecord = NULL;
  StGichromidPtr pHead = NULL;
  SQLHANDLE      hstmt;
  Int4           retcode = 0;
  SQLINTEGER     fetchCount  = 0;

  CharPtr pSQL_stmt = (Char *) Malloc(sizeof(Char) * 150);
  if(pSQL_stmt == NULL)
  {
	ErrPostEx(SEV_ERROR, 0, 0, "[GetAllRecordsFromGICHROMID]: Could not allocate space");
	return FALSE;
  }

  sprintf((char *) pSQL_stmt, "select g.gi, c.kloodge from "
		  "%s.gichromid g, %s.chrom c where g.chromid = c.chromid;", 
		  database, database );

  if(CreateHandle(&hstmt, pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[GetAllRecordsFromGICHROMID]: Could not create handle.");
      Free(pSQL_stmt);
      return NULL;
   }

  /* execute statment */
  retcode = SQLExecute(hstmt);
  if (retcode != SQL_SUCCESS)
  {
	ErrPostEx(SEV_ERROR,0,0, "[GetAllRecordsFromGICHROMID]: "
			  "unable to execute statement.");
	LogSQLInfo(hstmt);
	SQLFreeStmt(hstmt, SQL_DROP );
    Free(pSQL_stmt);
	return pRecord;
  }

  retcode = SQLBindCol(hstmt, 1, SQL_C_SLONG, 
					   &gichromid_gi.val, 0, NULL);

  if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
  {
	ErrPostEx(SEV_ERROR, 0, 0, "[GetAllRecordsFromGICHROMID]: "
			  "Could not bind column 1.");
	SQLFreeStmt(hstmt, SQL_DROP);
	Free(pSQL_stmt);
	return NULL;
  }

  /* Return the kloodge in the gichromid_chromid variable. */
  retcode = SQLBindCol(hstmt, 2, SQL_C_SLONG, 
					   &gichromid_chromid.val, 0, NULL);
  if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
  {
	ErrPostEx(SEV_ERROR, 0, 0, "[GetAllRecordsFromGICHROMID]: "
			  "Could not bind column 2.");

	SQLFreeStmt(hstmt, SQL_DROP);
	Free(pSQL_stmt);
	return NULL;
  }

/* fetch result and save in the linked list */
   while((retcode=SQLFetch(hstmt)) == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
   {
	 fetchCount++;
	 if (fetchCount == 1)
	 {
         pHead = StGichromidNew();
		 pHead->next = NULL;
		 pRecord = pHead;
	 }
	 else
	 {
	   pRecord->next = StGichromidNew();
	   pRecord = pRecord->next;
	 }

	 pRecord->gi       = gichromid_gi.val;
	 pRecord->chromid  = gichromid_chromid.val;
   }

   
   retcode = SQLFreeStmt(hstmt, SQL_DROP);
   if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[GetAllRecordsFromGICHROMID]: "
				"Error in closing cursor.");
   }

   Free (pSQL_stmt);
  return pHead;
}

Boolean LIBCALL DeleteChromidInGICHROMID( Int4 chromid )
{
  SQLRETURN retcode;

  gichromid_chromid.val = chromid;

  retcode = SQLExecute( HNDLGichromidDeleteChromid );

  if( retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
  {
	ErrPostEx(SEV_ERROR, 0, 0, "[DeleteChromidInGichromid]: "
			  "Unable to execute statement.");
	LogSQLInfo( HNDLGichromidDeleteChromid );
	return FALSE;
  }

  SQLFreeStmt( HNDLGichromidDeleteChromid, SQL_CLOSE );

  return TRUE;
}
Boolean LIBCALL DeleteGiInGICHROMID( Int4 gi)
{
  SQLRETURN retcode;

  gichromid_gi.val  = gi;

  retcode = SQLExecute( HNDLGichromidDeleteGi );

  if( retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
  {
	ErrPostEx(SEV_ERROR, 0, 0, "[DeleteGiInGichromid]: "
			  "Unable to execute statement for gi %ld.", gi);
	LogSQLInfo( HNDLGichromidDeleteGi );
	return FALSE;
  }

  SQLFreeStmt( HNDLGichromidDeleteGi, SQL_CLOSE );

  return TRUE;
}


StGichromidPtr LIBCALL SearchGICHROMIDByGi( Int4 gi )
{
  SQLRETURN retcode = SQL_SUCCESS;
  StGichromidPtr pRecord = NULL, pHead = NULL;
  Int4 fetchCount = 0;
  

  if( gi <= 0 )
  {
	ErrPostEx(SEV_ERROR,0,0, "[SearchGICHROMIDByGi] Invalid gi.");
	return pHead;
  }

  gichromid_gi.val = gi;

  retcode = SQLExecute(HNDLGichromidSearchGi);

  if (retcode != SQL_SUCCESS  && retcode != SQL_SUCCESS_WITH_INFO)
  {
	ErrPostEx(SEV_ERROR,0,0, "[SearchGICHROMIDByGi]: "
			  "unable to execute search statement. (gi=%d)", gi);
	LogSQLInfo(HNDLGichromidSearchGi);
	return pHead;
  }

  retcode = SQLBindCol(HNDLGichromidSearchGi, 1, SQL_C_SLONG, 
					   &gichromid_chromid.val, 0, NULL);

  if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
  {
	ErrPostEx(SEV_ERROR, 0, 0, "[SearchGICHROMIDByGi]: "
			  "Could not bind column 1.");
	return pHead;
  }

  retcode = SQLBindCol(HNDLGichromidSearchGi, 2, SQL_C_SLONG, 
					   &gichromid_contiggi.val, 0, NULL);

  if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
  {
	ErrPostEx(SEV_ERROR, 0, 0, "[SearchGICHROMIDByGi]: "
			  "Could not bind column 2.");
	return pHead;
  }

  while((retcode=SQLFetch(HNDLGichromidSearchGi)) == SQL_SUCCESS || 
		 retcode == SQL_SUCCESS_WITH_INFO)
  {
	fetchCount++;
	
	if( fetchCount == 1 )
	{
	  pHead = StGichromidNew();
	  pHead->next = NULL;
	  pRecord = pHead;
	}
	else
	{
	  pRecord->next = StGichromidNew();
	  pRecord = pRecord->next;
	}
	
	pRecord->chromid  = gichromid_chromid.val;
	pRecord->contiggi = gichromid_contiggi.val;
  }

  if( fetchCount > 1 )
  {
		ErrPostEx(SEV_ERROR, 0, 0, "[SearchGICHROMIDByGi]: "
			  "2 records with the same gi %ld.", gi );
  }

   /* close the cursor and discard all pending results. */
   /* must do in order to re-use the handle created in InitCHROM */
  retcode = SQLFreeStmt(HNDLGichromidSearchGi, SQL_CLOSE);
  if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
  {
	ErrPostEx(SEV_ERROR, 0, 0, "[SearchGICHROMIDByGi]: "
			  "Error in closing cursor.");
  }

  return pHead;
}

StGichromidPtr LIBCALL SearchGICHROMIDByContigGi( Int4 contigGi )
{

  SQLRETURN retcode = SQL_SUCCESS;
  StGichromidPtr pRecord = NULL, pHead = NULL;
  Int4 fetchCount = 0;

  if( contigGi <= 0 )
  {
	ErrPostEx(SEV_ERROR,0,0, "[SearchGICHROMIDByContigGi] Invalid gi.");
	return pHead;
  }

  gichromid_contiggi.val = contigGi;

  retcode = SQLExecute(HNDLGichromidSearchContiggi);

  if (retcode != SQL_SUCCESS  && retcode != SQL_SUCCESS_WITH_INFO)
  {
	ErrPostEx(SEV_ERROR,0,0, "[SearchGICHROMIDByContigGi]: "
			  "unable to execute search statement. (gi=%d)", contigGi);
	LogSQLInfo(HNDLGichromidSearchContiggi);
	return pHead;
  }

  retcode = SQLBindCol(HNDLGichromidSearchContiggi, 1, SQL_C_SLONG, 
					   &gichromid_gi.val, 0, NULL);

  if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
  {
	ErrPostEx(SEV_ERROR, 0, 0, "[SearchGICHROMIDByContigGi]: "
			  "Could not bind column 1.");
	return pRecord;
  }

  while((retcode=SQLFetch(HNDLGichromidSearchContiggi)) == SQL_SUCCESS || 
		 retcode == SQL_SUCCESS_WITH_INFO)
  {
	fetchCount++;

	if( fetchCount == 1 )
	{
	  pHead = StGichromidNew();
	  pHead->next = NULL;
	  pRecord = pHead;
	}
	else
	{
	  pRecord->next = StGichromidNew();
	  pRecord = pRecord->next;
	}
	
	pRecord->gi = gichromid_gi.val;
  }

   /* close the cursor and discard all pending results. */
   /* must do in order to re-use the handle created in InitCHROM */
  retcode = SQLFreeStmt(HNDLGichromidSearchContiggi, SQL_CLOSE);
  if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
  {
	ErrPostEx(SEV_ERROR, 0, 0, "[SearchGICHROMIDByContigGi]: "
			  "Error in closing cursor.");
  }

  return pHead;

}

void LIBCALL ResetStGICHROMID(StGichromidPtr PNTR pGichrom)
{
  StGichromidPtr pRecord = *pGichrom;

  pRecord->gi = 0;
  pRecord->contiggi = 0;
  pRecord->chromid = 0;
}

Boolean LIBCALL DeleteContigGiInGICHROMID( Int4 gi)
{
  SQLRETURN retcode;

  gichromid_contiggi.val = gi;

  retcode = SQLExecute( HNDLGichromidDeleteContigGi );

  if( retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
  {
	ErrPostEx(SEV_ERROR, 0, 0, "[DeleteContigGiInGICHROMID]: "
			  "Unable to execute statement for %ld.", gi);
	LogSQLInfo( HNDLGichromidDeleteContigGi );
	return FALSE;
  }

  SQLFreeStmt( HNDLGichromidDeleteContigGi, SQL_CLOSE );

  return TRUE;
}


/*===============================================================
  CONTIGCHROMID FUNCTIONS
================================================================*/

Boolean LIBCALL ReplaceRecordCONTIGCHROMID( StContigchromidPtr pRecord )
{
  SQLRETURN retcode;
   
  if(pRecord == NULL)
  {
	ErrPostEx(SEV_ERROR,0,0, "[ReplaceRecordCONTIGCHROMID]: "
			  "NULL parameter.");
	return FALSE;
  }

  contigchromid_contiggi.val = pRecord->contiggi;
  contigchromid_topgi.val = pRecord->topgi;
  contigchromid_chromid.val  = pRecord->chromid;
  contigchromid_changed.val = pRecord->changed;

  /* execute statment */
  retcode = SQLExecute(HNDLContigchromidInsert);
  if (retcode != SQL_SUCCESS  && retcode != SQL_SUCCESS_WITH_INFO)
  {
	ErrPostEx(SEV_ERROR,0,0, "[ReplaceRecordCONTIGCHROMID]: "
			  "unable to execute statement. (gi=%ld)", pRecord->contiggi);
	LogSQLInfo(HNDLContigchromidInsert);
	return FALSE;
  }

  return TRUE;
}

/**
 *   EditRecordCONTIGCHROMIDUpdate
 *
 *   Update the changed field for the given gi.
 *
 *  @param gi      The gi of the record to update.
 *
 *  @param update  The value with which to update the changed 
 *                 field. 1 for changed, 2 for unchanged.
 * 
 *  @return        True on success, false on failure.
 */         
Boolean LIBCALL EditRecordCONTIGCHROMIDUpdate( Int4 gi, 
											   Int4 update)
{
  SQLRETURN retcode;
   
  if(gi < NULL)
  {
	ErrPostEx(SEV_ERROR,0,0, "[EditRecordCONTIGCHROMIDUpdate]: "
			  "Bad gi.");
	return FALSE;
  }

  contigchromid_contiggi.val = gi;
  contigchromid_changed.val = update;

  /* execute statment */
  retcode = SQLExecute(HNDLContigchromidUpdate);
  if (retcode != SQL_SUCCESS  && retcode != SQL_SUCCESS_WITH_INFO)
  {
	ErrPostEx(SEV_ERROR,0,0, "[EditRecordCONTIGCHROMIDUpdate]: "
			  "unable to execute statement. (gi=%ld)", 
			  contigchromid_contiggi.val);
	LogSQLInfo(HNDLContigchromidUpdate);
	return FALSE;
  }

  return TRUE;
}

/**
 *   EditRecordCONTIGCHROMIDUpdateAll
 *
 *   Sets the changed field to 2 for the entire CONTIGCHROMID
 *   table.
 *
 *  @return        True on success, false on failure.
 */   
Boolean LIBCALL EditRecordCONTIGCHROMIDUpdateAll()
{
  SQLHANDLE      hstmt;
  Int4           retcode = 0;

  CharPtr pSQL_stmt = (Char *) malloc(sizeof(Char) * 150);
  if(pSQL_stmt == NULL)
  {
	ErrPostEx(SEV_ERROR, 0, 0, "[EditRecordCONTIGCHROMIDUpdateAll]: Could not allocate space");
	return FALSE;
  }

  sprintf((char *) pSQL_stmt,"UPDATE %s.contigchromid SET "
		  "changed = 2;", database );

  if(CreateHandle(&hstmt, pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[EditRecordCONTIGCHROMIDUpdateAll]: Could not create handle.");
      free(pSQL_stmt);
      return FALSE;
   }

  /* execute statment */
  retcode = SQLExecute(hstmt);
  if (retcode != SQL_SUCCESS  && retcode != SQL_SUCCESS_WITH_INFO)
  {
	ErrPostEx(SEV_ERROR,0,0, "[EditRecordCONTIGCHROMIDUpdateAll]: "
			  "unable to execute statement.");
	LogSQLInfo(hstmt);
	SQLFreeStmt(hstmt, SQL_DROP);
	free(pSQL_stmt);
	return FALSE;
  }
  retcode = SQLFreeStmt(hstmt, SQL_DROP);
  if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
  {
	ErrPostEx(SEV_ERROR, 0, 0, "[EditRecordCONTIGCHROMIDUpdateAll]: "
			  "Error in closing cursor.");
  }

  free(pSQL_stmt);
  return TRUE;
}

/**
 * GetAllRecordsFromCONTIGCHROMID()
 *
 * Get gi and our chromid from CONTIGCHROMID and CHROM.
 *
 */ 
StContigchromidPtr LIBCALL GetAllRecordsFromCONTIGCHROMID()
{
  StContigchromidPtr pRecord = NULL;
  StContigchromidPtr pHead = NULL;
  SQLHANDLE      hstmt;
  Int4           retcode = 0;
  SQLINTEGER     fetchCount  = 0;

  CharPtr pSQL_stmt = (Char *) malloc(sizeof(Char) * 150);
  if(pSQL_stmt == NULL)
  {
	ErrPostEx(SEV_ERROR, 0, 0, "[GetAllRecordsFromCONTIGCHROMID]: Could not allocate space");
	return FALSE;
  }

  /*----------------------------------------------------------------
	Note that we return the chromid here because these values will 
	be written to gichromid, which stores the chromid.
	----------------------------------------------------------------*/
  sprintf((char *) pSQL_stmt,"select g.contiggi, c.chromid "
		"from %s.contigchromid g, %s.chrom c where g.chromid = "
		  "c.chromid and g.changed = 1;",
	database, database );

  if(CreateHandle(&hstmt, pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[GetAllRecordsFromCONTIGCHROMID]: Could not create handle.");
      free(pSQL_stmt);
      return FALSE;
   }

  /* execute statment */
  retcode = SQLExecute(hstmt);
  if (retcode != SQL_SUCCESS  && retcode != SQL_SUCCESS_WITH_INFO)
  {
	ErrPostEx(SEV_ERROR,0,0, "[GetAllRecordsFromCONTIGCHROMID]: "
			  "unable to execute statement.");
	LogSQLInfo(hstmt);
	SQLFreeStmt(hstmt, SQL_DROP);
	free(pSQL_stmt);
	return pRecord;
  }

  retcode = SQLBindCol(hstmt, 1, SQL_C_SLONG, 
					   &contigchromid_contiggi.val, 0, NULL);

  if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
  {
	ErrPostEx(SEV_ERROR, 0, 0, "[GetAllRecordsFromCONTIGCHROMID]: "
			  "Could not bind column 1.");
	SQLFreeStmt(hstmt, SQL_DROP);
	free(pSQL_stmt);
	return pRecord;
  }
  retcode = SQLBindCol(hstmt, 2, SQL_C_SLONG, 
					   &contigchromid_chromid.val, 0, NULL);
  if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
  {
	ErrPostEx(SEV_ERROR, 0, 0, "[GetAllRecordsFromCONTIGCHROMID]: "
			  "Could not bind column 2.");
	SQLFreeStmt(hstmt, SQL_DROP);
	free(pSQL_stmt);
	return pRecord;
  }

/* fetch result and save in the linked list */
   while((retcode=SQLFetch(hstmt)) == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
   {
	 fetchCount++;
	 if (fetchCount == 1)
	 {
         pHead = StContigchromidNew();
		 pHead->next = NULL;
		 pRecord = pHead;
	 }
	 else
	 {
	   pRecord->next = StContigchromidNew();
	   pRecord = pRecord->next;
	 }

	 pRecord->contiggi = contigchromid_contiggi.val;
	 pRecord->chromid = contigchromid_chromid.val;
   }

   retcode = SQLFreeStmt(hstmt, SQL_DROP);
   if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[GetAllRecordsFromCONTIGCHROMID]: "
				"Error in closing cursor.");
   }

   free(pSQL_stmt);
   return pHead;
}

Boolean LIBCALL DeleteChromidInCONTIGCHROMID( Int4 chromid )
{
 SQLRETURN retcode;

  contigchromid_chromid.val = chromid;

  retcode = SQLExecute( HNDLContigchromidDeleteChromid );

  if( retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
  {
	ErrPostEx(SEV_ERROR, 0, 0, "[DeleteChromidInContigchromid]: "
			  "Unable to execute statement.%ld", chromid);
	LogSQLInfo( HNDLContigchromidDeleteChromid );
	return FALSE;
  }

  return TRUE;
}

Boolean LIBCALL DeleteGiInCONTIGCHROMID( Int4 gi )
{
 SQLRETURN retcode;

  contigchromid_contiggi.val = gi;

  retcode = SQLExecute( HNDLContigchromidDeleteGi );

  if( retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
  {
	ErrPostEx(SEV_ERROR, 0, 0, "[DeleteGiInContigchromid]: "
			  "Unable to execute statement for %ld.", gi);
	LogSQLInfo( HNDLContigchromidDeleteGi );
	return FALSE;
  }

  return TRUE;
}

Boolean LIBCALL DeleteTopGiInCONTIGCHROMID( Int4 topgi)
{
SQLRETURN retcode;

  contigchromid_topgi.val = topgi;

  retcode = SQLExecute( HNDLContigchromidDeleteTopGi );

  if( retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
  {
	ErrPostEx(SEV_ERROR, 0, 0, "[DeleteContigGiInGICHROMID]: "
			  "Unable to execute statement for %ld.", topgi);
	LogSQLInfo( HNDLContigchromidDeleteTopGi );
	return FALSE;
  }

  return TRUE;
}

void LIBCALL ResetStCONTIGCHROMID(StContigchromidPtr PNTR pContigchromid)
{
  StContigchromidPtr pRecord = *pContigchromid;

  pRecord->contiggi = 0;
  pRecord->topgi = 0;
  pRecord->chromid = 0;
  pRecord->changed = 0;
}

/** AddChromIdsToCONTIGCHROMID()
 *
 *  Inserts gis from CONTIGCHROMOSOME int CONTIGCHROMID with the 
 *  appropriate chromid. This is only done for gis that are not
 *  already in CONTIGCHROMID.
 *
 *  @return TRUE or FALSE.
 *
 */
Boolean LIBCALL AddChromIdsToCONTIGCHROMID()
{
  SQLHANDLE      hstmt;
  Int4           retcode = 0;

  CharPtr pSQL_stmt = ( Char * ) Malloc( sizeof( Char ) * 250 );

  if(pSQL_stmt == NULL)
  {
	ErrPostEx(SEV_ERROR, 0, 0, "[AddChromIdsToCONTIGCHROMID]: "
			  "Could not allocate space");
	return FALSE;
  }
   /*--------------------------------------------------------------
	 Insert the records.
	-------------------------------------------------------------*/
   sprintf( (char *) pSQL_stmt, "insert ignore into %s.contigchromid ( "
			"contiggi, chromid ) select g.contiggi, c.chromid from contigchromosome g "
			"inner join chrom c on c.chromnum = g.chromnum INNER JOIN "
			"taxgi t on g.contiggi = t.gi where c.taxid = t.taxid;",
			database );

   if(CreateHandle(&hstmt, pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
   {
	 ErrPostEx(SEV_ERROR, 0, 0, "[AddChromIdsToCONTIGCHROMID]: "
			   "Unable to create handle to insert records." );
	 Free(pSQL_stmt);
	 return FALSE;
   }

   /* execute statment */
   retcode = SQLExecute(hstmt);
   if ( retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO )
   {
	 ErrPostEx(SEV_ERROR,0,0, "[AddChromIdsToCONTIGCHROMID]: "
			   "unable to add records.");
	 LogSQLInfo(hstmt);
	 SQLFreeStmt(hstmt, SQL_DROP );
	 Free(pSQL_stmt);
	 return FALSE;
   }

   SQLFreeStmt(hstmt, SQL_DROP );

   Free(pSQL_stmt);
   return TRUE;

}

StContigchromidPtr LIBCALL SearchCONTIGCHROMIDByTopGi( Int4 topGi )
{
  SQLRETURN retcode = SQL_SUCCESS;
  StContigchromidPtr pRecord = NULL, pHead = NULL;
  Int4 fetchCount = 0;

  if( topGi <= 0 )
  {
	ErrPostEx(SEV_ERROR,0,0, "[SearchCONTIGCHROMIDByTopGi] Invalid gi.");
	return pHead;
  }

  contigchromid_topgi.val = topGi;

  retcode = SQLExecute(HNDLContigchromidSearchTopGi);

  if (retcode != SQL_SUCCESS  && retcode != SQL_SUCCESS_WITH_INFO)
  {
	ErrPostEx(SEV_ERROR,0,0, "[SearchCONTIGCHROMIDByTopGi]: "
			  "unable to execute search statement. (topgi=%d)", topGi);
	LogSQLInfo(HNDLContigchromidSearchTopGi);
	return pHead;
  }

  retcode = SQLBindCol(HNDLContigchromidSearchTopGi, 1, SQL_C_SLONG, 
					   &contigchromid_contiggi.val, 0, NULL);

  if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
  {
	ErrPostEx(SEV_ERROR, 0, 0, "[SearchCONTIGCHROMIDByTopGi]: "
			  "Could not bind column 1.");
	return pHead;
  }

  while((retcode=SQLFetch(HNDLContigchromidSearchTopGi)) == SQL_SUCCESS || 
		 retcode == SQL_SUCCESS_WITH_INFO)
  {
	fetchCount++;

	if( fetchCount == 1 )
	{
	  pHead = StContigchromidNew();
	  pHead->next = NULL;
	  pRecord = pHead;
	}
	else
	{
	  pRecord->next = StContigchromidNew();
	  pRecord = pRecord->next;
	}
	
	pRecord->contiggi = contigchromid_contiggi.val;
  }

   /* close the cursor and discard all pending results. */
   /* must do in order to re-use the handle created in InitCHROM */
  retcode = SQLFreeStmt(HNDLContigchromidSearchTopGi, SQL_CLOSE);
  if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
  {
	ErrPostEx(SEV_ERROR, 0, 0, "[SearchCONTIGCHROMIDByTopGi]: "
			  "Error in closing cursor.");
  }

  return pRecord;

}

Boolean LIBCALL RemoveGenBankRecordsFromCONTIGCHROMID()
{
  SQLHANDLE      hstmt;
  Int4           retcode = 0;

  CharPtr pSQL_stmt = ( Char * ) Malloc( sizeof( Char ) * 250 );

  if(pSQL_stmt == NULL)
  {
	ErrPostEx(SEV_ERROR, 0, 0, "[RemoveGenBankRecordsFromCONTIGCHROMID]: "
			  "Could not allocate space");
	return FALSE;
  }

  /*--------------------------------------------------------------
	First create the temporary table that holds all non-refseq gis .
	-------------------------------------------------------------*/

  sprintf((char *) pSQL_stmt, "CREATE TEMPORARY TABLE gbcontigs SELECT "
		  "g.contiggi FROM %s.contigchromid g LEFT JOIN %s.accdb a ON "
		  "a.gi = g.contiggi AND db = 'ref' WHERE a.gi IS NULL;",
		  database, database );

  if(CreateHandle(&hstmt, pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[RemoveGenBankRecordsFromCONTIGCHROMID]: "
				"Could not create handle to create temp table.");
      Free(pSQL_stmt);
      return FALSE;
   }

  /* execute statment */
  retcode = SQLExecute(hstmt);
  if (retcode != SQL_SUCCESS)
  {
	ErrPostEx(SEV_ERROR,0,0, "[RemoveGenBankRecordsFromCONTIGCHROMID]: "
			  "unable to create temporary table.");
	LogSQLInfo(hstmt);
	SQLFreeStmt(hstmt, SQL_DROP );
	Free(pSQL_stmt);
	return FALSE;
  }

  SQLFreeStmt(hstmt, SQL_DROP );
  /*--------------------------------------------------------------
	Delete the records from contigkloodge.
	-------------------------------------------------------------*/
  sprintf((char *) pSQL_stmt, "DELETE c FROM contigchromid c,"
		  "gbcontigs g where c.contiggi = g.contiggi;", 
		  database );


  if(CreateHandle(&hstmt, pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[RemoveGenBankRecordsFromCONTIGCHROMID]: "
				"Could not create handle to delete records.");
      Free(pSQL_stmt);
      return FALSE;
   }

  /* execute statment */
  retcode = SQLExecute(hstmt);
  if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO )
  {
	ErrPostEx(SEV_ERROR,0,0, "[RemoveGenBankRecordsFromCONTIGCHROMID]: "
			  "unable to delete records.");
	LogSQLInfo(hstmt);
	SQLFreeStmt(hstmt, SQL_DROP );
	Free(pSQL_stmt);
	return FALSE;
  }

  SQLFreeStmt(hstmt, SQL_DROP );
  /*--------------------------------------------------------------
	Drop the temporary table.
	-------------------------------------------------------------*/
  sprintf((char *) pSQL_stmt, "DROP TABLE gbcontigs;", database );

  if(CreateHandle(&hstmt, pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
  {
	ErrPostEx(SEV_ERROR, 0, 0, "[RemoveGenBankRecordsFromCONTIGCHROMID]: "
				"Could not create handle to drop temporary table");
	Free(pSQL_stmt);
	return FALSE;
  }

  /* execute statment */
  retcode = SQLExecute(hstmt);
  if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO )
  {
	ErrPostEx(SEV_ERROR,0,0, "[RemoveGenBankRecordsFromCONTIGCHROMID]: "
			  "Could not drop temporary table" );

	LogSQLInfo(hstmt);
	SQLFreeStmt(hstmt, SQL_DROP );
	Free(pSQL_stmt);
	return FALSE;
  }

  SQLFreeStmt(hstmt, SQL_DROP );
  Free(pSQL_stmt);
  return TRUE;
}

/*===============================================================
  GICHROMOSOME FUNCTIONS
================================================================*/
Boolean LIBCALL AppendRecordGICHROMOSOME( StGichromPtr pRecord )
{
  SQLRETURN retcode;
   
  if(pRecord == NULL || pRecord->gi <= 0 || pRecord->chromnum == NULL)
  {
	ErrPostEx(SEV_ERROR,0,0, "[AppendRecordGICHROMOSOME]: Bad input.");
	return FALSE;
  }

  gichrom_gi.val      = pRecord->gi;
  sprintf( (char *)gichrom_chromnum.val, "%s", pRecord->chromnum );

  /* execute statment */
  retcode = SQLExecute(HNDLGichromoInsert);
  if (retcode != SQL_SUCCESS)
  { 
	ErrPostEx(SEV_ERROR,0,0, "[AppendRecordGICHROMOSOME]: "
			  "unable to execute insert statement. (gi=%ld, chromnum = %w)",
			  gichrom_gi.val, gichrom_chromnum.val);
	LogSQLInfo(HNDLGichromoInsert);
	return FALSE;
  }

  return TRUE;
}

/**
 * GetAllRecordsFromGICHROMOSOME
 *
 * Select gi from GICHROMOSOME, NCBI chromid from CHROM using chromnum
 * and the taxig from TAXGI. We get the chromid because we are writing
 * to GICHROMID.
 *
 * @return StGikloodgePtr with gi and klooodge.
 */
StGichromidPtr LIBCALL GetAllRecordsFromGICHROMOSOME()
{
  StGichromidPtr pRecord = NULL;
  StGichromidPtr pHead = NULL;
  SQLHANDLE      hstmt;
  Int4           retcode = 0;
  SQLINTEGER     fetchCount  = 0;

  CharPtr pSQL_stmt = (Char *) malloc(sizeof(Char) * 150);
  if(pSQL_stmt == NULL)
  {
	ErrPostEx(SEV_ERROR, 0, 0, "[GetAllRecordsFromGICHROMOSOME]: Could not allocate space");
	return FALSE;
  }

  /*  sprintf((char *) pSQL_stmt, "select g.gi, c.kloodge from "
		  "%s.gichromosome g, %s.chrom c, %s.taxgi t where "
		  "g.gi = t.gi and "
		  "t.taxid = c.taxid and "
          "g.chromnum = c.chromnum;", 
		  database, database, database );*/

  /* Get the ncbi chromid from chrom because we are going to
	 write this to gichromid. */
  sprintf( (char *) pSQL_stmt, "SELECT g.gi, c.chromid FROM "
		   "%s.gichromosome g INNER JOIN chrom c "
		   "ON c.chromnum = g.chromnum INNER JOIN taxgi t "
		   "ON g.gi = t.gi WHERE c.taxid = t.taxid;" );

  if(CreateHandle(&hstmt, pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[GetAllRecordsFromGICHROMOSOME]: Could not create handle.");
      free(pSQL_stmt);
      return FALSE;
   }

  /* execute statment */
  retcode = SQLExecute(hstmt);
  if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
  {
	ErrPostEx(SEV_ERROR,0,0, "[GetAllRecordsFromGICHROMOSOME]: "
			  "unable to execute statement.");
	LogSQLInfo(hstmt);
	return pRecord;

  }

  retcode = SQLBindCol(hstmt, 1, SQL_C_SLONG, 
					   &gichromid_gi.val, 0, NULL);

  if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
  {
	ErrPostEx(SEV_ERROR, 0, 0, "[GetAllRecordsFromGICHROMOSOME]: "
			  "Could not bind column 1.");
	SQLFreeStmt(hstmt, SQL_DROP);
	return 0;
  }

  retcode = SQLBindCol(hstmt, 2, SQL_C_SLONG, 
					   &gichromid_chromid.val, 0, NULL);
  if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
  {
	ErrPostEx(SEV_ERROR, 0, 0, "[GetAllRecordsFromGICHROMOSOME]: "
			  "Could not bind column 2.");
	return 0;
  }

/* fetch result and save in the linked list */
   while((retcode=SQLFetch(hstmt)) == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
   {
	 fetchCount++;
	 if (fetchCount == 1)
	 {
         pHead = StGichromidNew();
		 pHead->next = NULL;
		 pRecord = pHead;
	 }
	 else
	 {
	   pRecord->next = StGichromidNew();
	   pRecord = pRecord->next;
	 }

	 pRecord->gi = gichromid_gi.val;
	 pRecord->chromid = gichromid_chromid.val;
   }

   retcode = SQLFreeStmt(hstmt, SQL_DROP);
   if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[GetAllRecordsFromGICHROMOSOME]: "
				"Error in closing cursor.");
   }

  return pHead;
}

void LIBCALL ResetStGICHROM(StGichromPtr PNTR ppGiChrom)
{
  StGichromPtr pRecord = *ppGiChrom;

  pRecord->gi = 0;
  if( pRecord->chromnum != NULL )
  {
	Free( pRecord->chromnum );
	pRecord->chromnum = NULL;
  }
}

Boolean LIBCALL DeleteAllGICHROMOSOME()
{
  SQLHANDLE hstmt;
  Int2 retcode = 0;

  CharPtr pSQL_stmt= ( Char * ) Malloc( sizeof( Char ) * 150 );

  if(pSQL_stmt == NULL)
  {
	ErrPostEx(SEV_ERROR, 0, 0, "[DeleteAllGICHROMOSOME]: "
			  "Could not allocate space");
	return FALSE;
  }

  sprintf((char *) pSQL_stmt, "delete from "
		  "%s.gichromosome;", database );

  if(CreateHandle(&hstmt, pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[DeleteAllGICHROMOSOME]: "
				"Could not create handle.");
      Free(pSQL_stmt);
      return FALSE;
   }

  /* execute statment */
  retcode = SQLExecute(hstmt);
  if (retcode != SQL_SUCCESS)
  {
	ErrPostEx(SEV_ERROR,0,0, "[DeleteAllGICHROMOSOME]: "
			  "unable to execute statement.");
	LogSQLInfo(hstmt);
	SQLFreeStmt(hstmt, SQL_DROP );
	Free(pSQL_stmt);
	return FALSE;
  }

  retcode = SQLFreeStmt(hstmt, SQL_DROP);
  if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
  {
	ErrPostEx(SEV_ERROR, 0, 0, "[DeleteAllGICHROMOSOME]: "
			  "Error in closing cursor.");
	return FALSE;
  }
  Free(pSQL_stmt);
  return TRUE;
}

Boolean LIBCALL RemoveGenBankRecordsFromGICHROMOSOME()
{
  SQLHANDLE      hstmt;
  Int4           retcode = 0;

  CharPtr pSQL_stmt = ( Char * ) Malloc( sizeof( Char ) * 250 );

  if(pSQL_stmt == NULL)
  {
	ErrPostEx(SEV_ERROR, 0, 0, "[RemoveGenBankRecordsFromGICHROMOSOME]: "
			  "Could not allocate space");
	return FALSE;
  }

  /*--------------------------------------------------------------
	First create the temporary table that holds all non-refseq gis .
	-------------------------------------------------------------*/

  sprintf((char *) pSQL_stmt, "CREATE TEMPORARY TABLE gbgis SELECT "
		  "g.gi FROM %s.gichromosome g LEFT JOIN %s.accdb a ON "
		  "a.gi = g.gi AND db = 'ref' WHERE a.gi IS NULL;",
		  database, database );

  if(CreateHandle(&hstmt, pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[RemoveGenBankRecordsFromGICHROMOSOME]: "
				"Could not create handle to create temp table.");
      Free(pSQL_stmt);
      return FALSE;
   }

  /* execute statment */
  retcode = SQLExecute(hstmt);
  if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
  {
	ErrPostEx(SEV_ERROR,0,0, "[RemoveGenBankRecordsFromGICHROMOSOME]: "
			  "unable to create temporary table.");
	LogSQLInfo(hstmt);
	SQLFreeStmt(hstmt, SQL_DROP );
	Free(pSQL_stmt);
	return FALSE;
  }

  SQLFreeStmt(hstmt, SQL_DROP );
  /*--------------------------------------------------------------
	Delete the records from gichromosome.
	-------------------------------------------------------------*/
  sprintf((char *) pSQL_stmt, "DELETE k FROM %s.gichromosome k "
		  "INNER JOIN gbgis g ON k.gi = g.gi;", database );


  if(CreateHandle(&hstmt, pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[RemoveGenBankRecordsFromGICHROMOSOME]: "
				"Could not create handle to delete records.");
      Free(pSQL_stmt);
      return FALSE;
   }

  /* execute statment */
  retcode = SQLExecute(hstmt);
  if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
  {
	ErrPostEx(SEV_ERROR,0,0, "[RemoveGenBankRecordsFromGICHROMOSOME]: "
			  "unable to delete records.");
	LogSQLInfo(hstmt);
	Free(pSQL_stmt);
	return FALSE;
  }

  SQLFreeStmt(hstmt, SQL_DROP );
  /*--------------------------------------------------------------
	Drop the temporary table.
	-------------------------------------------------------------*/
  sprintf((char *) pSQL_stmt, "DROP TABLE gbgis;", database );

  if(CreateHandle(&hstmt, pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
  {
	ErrPostEx(SEV_ERROR, 0, 0, "[RemoveGenBankRecordsFromGICHROMOSOME]: "
				"Could not create handle to drop temporary table");
	Free(pSQL_stmt);
	return FALSE;
  }

  /* execute statment */
  retcode = SQLExecute(hstmt);
  if (retcode != SQL_SUCCESS)
  {
	ErrPostEx(SEV_ERROR,0,0, "[RemoveGenBankRecordsFromGICHROMOSOME]: "
			  "Could not drop temporary table" );

	LogSQLInfo(hstmt);
	SQLFreeStmt(hstmt, SQL_DROP );
	Free(pSQL_stmt);
	return FALSE;
  }

  SQLFreeStmt(hstmt, SQL_DROP );
  Free(pSQL_stmt);
  return TRUE;
}

/*===============================================================
  CONTIGCHROMOSOME FUNCTIONS
================================================================*/
Boolean LIBCALL AppendRecordCONTIGCHROMOSOME( StContigchromPtr pRecord)
{
 SQLRETURN retcode;
   
  if(pRecord == NULL || pRecord->contiggi <= 0 ||
	 pRecord->chromnum == NULL)
  {
	ErrPostEx(SEV_ERROR,0,0, "[AppendRecordCONTIGCHROMOSOME]:"
			  "Bad input.");
	return FALSE;
  }

  contigchrom_contiggi.val      = pRecord->contiggi;
  sprintf( (char *)contigchrom_chromnum.val, "%s", pRecord->chromnum );

  /* execute statment */
  retcode = SQLExecute(HNDLContigchromoInsert);
  if (retcode != SQL_SUCCESS)
  { 
	ErrPostEx(SEV_ERROR,0,0, "[AppendRecordCONTIGCHROMOSOME]: "
			  "unable to execute insert statement. (gi=%ld, chromnum = %w)",
			  contigchrom_contiggi.val, contigchrom_chromnum.val);
	LogSQLInfo(HNDLContigchromoInsert);
	return FALSE;
  }

  return TRUE;
}

Boolean LIBCALL DeleteAllCONTIGCHROMOSOME()
{
  SQLHANDLE hstmt;
  Int2 retcode = 0;

  CharPtr pSQL_stmt= ( Char * ) Malloc( sizeof( Char ) * 150 );

  if(pSQL_stmt == NULL)
  {
	ErrPostEx(SEV_ERROR, 0, 0, "[DeleteAllCONTIGCHROMOSOME]: "
			  "Could not allocate space");
	return FALSE;
  }

  sprintf((char *) pSQL_stmt, "delete from "
		  "%s.contigchromosome;", database );

  if(CreateHandle(&hstmt, pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[DeleteAllCONTIGCHROMOSOME]: "
				"Could not create handle.");
      Free(pSQL_stmt);
      return FALSE;
   }

  /* execute statment */
  retcode = SQLExecute(hstmt);
  if (retcode != SQL_SUCCESS)
  {
	ErrPostEx(SEV_ERROR,0,0, "[DeleteAllCONTIGCHROMOSOME]: "
			  "unable to execute statement.");
	LogSQLInfo(hstmt);
	SQLFreeStmt(hstmt, SQL_DROP );
	Free(pSQL_stmt);
	return FALSE;
  }

  retcode = SQLFreeStmt(hstmt, SQL_DROP);
  if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
  {
	ErrPostEx(SEV_ERROR, 0, 0, "[DeleteAllCONTIGCHROMOSOME]: "
			  "Error in closing cursor.");
	Free(pSQL_stmt);
	return FALSE;
  }
  Free(pSQL_stmt);
  return TRUE;
}

StContigchromidPtr LIBCALL GetAllRecordsFromCONTIGCHROMOSOME()
{
 StContigchromidPtr pRecord = NULL;
  StContigchromidPtr pHead = NULL;
  SQLHANDLE      hstmt;
  Int4           retcode = 0;
  SQLINTEGER     fetchCount  = 0;

  CharPtr pSQL_stmt = (Char *) Malloc(sizeof(Char) * 250);
  if(pSQL_stmt == NULL)
  {
	ErrPostEx(SEV_ERROR, 0, 0, "[GetAllRecordsFromCONTIGCHROMOSOME]: Could not allocate space");
	return pRecord;
  }

  /* Get the NCBI chromid from chrom because we are writing to 
	 GICHROMID. */

  sprintf((char *) pSQL_stmt, "select g.contiggi, c.chromid from "
		  "%s.contigchromosome g, %s.chrom c, %s.taxgi t where "
		  "g.contiggi = t.gi and "
		  "t.taxid = c.taxid and "
          "g.chromnum = c.chromnum;", 
		  database, database, database );

  if(CreateHandle(&hstmt, pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[GetAllRecordsFromCONTIGCHROMOSOME]: Could not create handle.");
      Free(pSQL_stmt);
      return pRecord;
   }

  Free(pSQL_stmt);

  /* execute statment */
  retcode = SQLExecute(hstmt);
  if (retcode != SQL_SUCCESS)
  {
	ErrPostEx(SEV_ERROR,0,0, "[GetAllRecordsFromCONTIGCHROMOSOME]: "
			  "unable to execute statement.");
	LogSQLInfo(hstmt);
	return pRecord;
  }

  retcode = SQLBindCol(hstmt, 1, SQL_C_SLONG, 
					   &contigchromid_contiggi.val, 0, NULL);

  if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
  {
	ErrPostEx(SEV_ERROR, 0, 0, "[GetAllRecordsFromCONTIGCHROMOSOME]: "
			  "Could not bind column 1.");
	SQLFreeStmt(hstmt, SQL_DROP);
	return pRecord;
  }
  retcode = SQLBindCol(hstmt, 2, SQL_C_SLONG, 
					   &contigchromid_chromid.val, 0, NULL);
  if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
  {
	ErrPostEx(SEV_ERROR, 0, 0, "[GetAllRecordsFromCONTIGCHROMOSOME]: "
			  "Could not bind column 2.");
	SQLFreeStmt(hstmt, SQL_DROP);
	return pRecord;
  }

/* fetch result and save in the linked list */
   while((retcode=SQLFetch(hstmt)) == SQL_SUCCESS ||
		 retcode == SQL_SUCCESS_WITH_INFO)
   {
	 fetchCount++;
	 if (fetchCount == 1)
	 {
         pHead = StContigchromidNew();
		 pHead->next = NULL;
		 pRecord = pHead;
	 }
	 else
	 {
	   pRecord->next = StContigchromidNew();
	   pRecord = pRecord->next;
	 }

	 pRecord->contiggi = contigchromid_contiggi.val;
	 pRecord->chromid = contigchromid_chromid.val;
   }

   retcode = SQLFreeStmt(hstmt, SQL_DROP);
   if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[GetAllRecordsFromCONTIGCHROMOSOME]: "
				"Error in closing cursor.");
   }

  return pHead;
}

Boolean LIBCALL RemoveGenBankRecordsFromCONTIGCHROMOSOME()
{
  SQLHANDLE      hstmt;
  Int4           retcode = 0;

  CharPtr pSQL_stmt = ( Char * ) Malloc( sizeof( Char ) * 250 );

  if(pSQL_stmt == NULL)
  {
	ErrPostEx(SEV_ERROR, 0, 0, "[RemoveGenBankRecordsFromCONTIGCHROMOSOME]: "
			  "Could not allocate space");
	return FALSE;
  }

  /*--------------------------------------------------------------
	First create the temporary table that holds all non-refseq gis .
	-------------------------------------------------------------*/
  sprintf((char *) pSQL_stmt, "CREATE TEMPORARY TABLE gbcontigs SELECT "
		  "g.contiggi FROM %s.contigchromosome g LEFT JOIN %s.accdb a ON "
		  "a.gi = g.contiggi AND db = 'ref' WHERE a.gi IS NULL;",
		  database, database );

  if(CreateHandle(&hstmt, pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[RemoveGenBankRecordsFromCONTIGCHROMOSOME]: "
				"Could not create handle to create temp table.");
      Free(pSQL_stmt);
      return FALSE;
   }

  /* execute statment */
  retcode = SQLExecute(hstmt);

  if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
  {
	ErrPostEx(SEV_ERROR,0,0, "[RemoveGenBankRecordsFromCONTIGCHROMOSOME]: "
			  "unable to create temporary table.");
	LogSQLInfo(hstmt);
	SQLFreeStmt(hstmt, SQL_DROP );
	Free(pSQL_stmt);
	return FALSE;
  }

  SQLFreeStmt(hstmt, SQL_DROP );
  /*--------------------------------------------------------------
	Delete the records from contigchromosome.
	-------------------------------------------------------------*/
  sprintf((char *) pSQL_stmt, "DELETE c FROM contigchromosome c, "
		  "gbcontigs g WHERE c.contiggi = g.contiggi;", 
		  database );

  if(CreateHandle(&hstmt, pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[RemoveGenBankRecordsFromCONTIGCHROMOSOME]: "
				"Could not create handle to delete records.");
      Free(pSQL_stmt);
      return FALSE;
   }

  /* execute statment */
  retcode = SQLExecute(hstmt);

  if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
  {
	ErrPostEx(SEV_ERROR,0,0, "[RemoveGenBankRecordsFromCONTIGCHROMOSOME]: "
			  "unable to delete records.");
	LogSQLInfo(hstmt);
	Free(pSQL_stmt);
	SQLFreeStmt(hstmt, SQL_DROP );
	
	return FALSE;
  }

  SQLFreeStmt(hstmt, SQL_DROP );
	
  /*--------------------------------------------------------------
	Drop the temporary table.
	-------------------------------------------------------------*/
  sprintf((char *) pSQL_stmt, "DROP TABLE gbcontigs;", database );

  if(CreateHandle(&hstmt, pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
  {
	ErrPostEx(SEV_ERROR, 0, 0, "[RemoveGenBankRecordsFromCONTIGCHROMOSOME]: "
				"Could not create handle to drop temporary table");
	Free(pSQL_stmt);
	return FALSE;
  }

  /* execute statment */
  retcode = SQLExecute(hstmt);
  if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
  {
	ErrPostEx(SEV_ERROR,0,0, "[RemoveGenBankRecordsFromCONTIGCHROMOSOME]: "
			  "Could not drop temporary table" );

	LogSQLInfo(hstmt);
	Free(pSQL_stmt);
	SQLFreeStmt(hstmt, SQL_DROP );

	return FALSE;
  }

  Free(pSQL_stmt);
  SQLFreeStmt(hstmt, SQL_DROP );	

  return TRUE;
}

void LIBCALL ResetStCONTIGCHROM(StContigchromPtr PNTR ppContigchrom)
{
  StContigchromPtr pRecord = *ppContigchrom;
 
  pRecord->contiggi = 0;
  if( pRecord->chromnum != NULL )
  {
	Free( pRecord->chromnum );
	pRecord->chromnum = NULL;
  }
}

/*===============================================================
  API and other utility functions
================================================================*/ 

/* returns a ValNode list with data.intvalue = taxid of each org with a completed genome */
ValNodePtr LIBCALL SHoundAllGenomes(void)
{
   ValNodePtr pvnTax = NULL;
   SQLRETURN sqlrc=SQL_SUCCESS;
   SQLHANDLE hstmt;

   CharPtr search_chrom = (char *) malloc(sizeof(Char) * 150);
   if (search_chrom == NULL)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[SHoundAllGenomes]: Could not allocate space");
     free(search_chrom);
     return FALSE;
   }
   sprintf((char *)search_chrom, "SELECT taxid from %s.chrom", database);

   struct{
     SQLINTEGER len;
     SQLINTEGER val;
   }taxid;

  if(CreateHandle(&hstmt,search_chrom, hdbc, &henv) != SQL_SUCCESS)
  {
    ErrPostEx(SEV_ERROR, 0, 0, "[SHoundAllGenomes]: Could not create handle.");
    return NULL;
  }
  free(search_chrom);

  /* execute statment */
  sqlrc=SQLExecute(hstmt);

   if (sqlrc != SQL_SUCCESS)
   {
	  ErrPostEx(SEV_ERROR,0,0, "[SHoundAllGenomes]: unable to execute select statement.");
	  LogSQLInfo(hstmt);
      SQLFreeStmt(hstmt, SQL_DROP);
      return NULL;
   }

  sqlrc=SQLBindCol(hstmt,1,SQL_C_SLONG,&taxid.val,0,(int*)&taxid.len);
  if (sqlrc != SQL_SUCCESS && sqlrc != SQL_SUCCESS_WITH_INFO)
  {
    ErrPostEx(SEV_ERROR,0,0,"[SHoundAllGenomes]: unable to execute bind column 1.");
    SQLFreeStmt(hstmt, SQL_DROP);
    return NULL;
  }

   /* fetch the data in columns iteratively */
   SQLINTEGER fetchCount = 0;
   /* fetch result and save in the linked list */
   while((sqlrc=SQLFetch(hstmt)) == SQL_SUCCESS || sqlrc == SQL_SUCCESS_WITH_INFO)
   {
      fetchCount++;
	  if (!ValNodeIsIn(taxid.val, pvnTax))
           pvnTax = Slri_ValNodeAddInt(&pvnTax, 0, taxid.val);
   }

  if(SQLFreeStmt(hstmt, SQL_DROP) != SQL_SUCCESS)
  {
     ErrPostEx(SEV_INFO, 0 ,0, "[SHoundAllGenomes] : unable to free statement.");
     print_err(hstmt);
     return NULL;
   }

  if (fetchCount == 0) {
    ErrPostEx(SEV_INFO, 0, 0, "[SHoundAllGenomes] No data found");
	return NULL;
  }

  return pvnTax;
}


/* returns a ValNode list with all accessions to chromosomes of completed genomes -
do not forget to free all strings */
ValNodePtr LIBCALL SHoundAllGenAccess(void)
{
   ValNodePtr pvnacc = NULL;
   CharPtr ptemp = NULL;
   SQLRETURN sqlrc=SQL_SUCCESS;
   SQLHANDLE hstmt;

   CharPtr search_chrom = (char *) malloc(sizeof(Char) * 150);
   if (search_chrom == NULL)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[SHoundAllGenAccess]: Could not allocate space");
     free(search_chrom);
     return FALSE;
   }
   sprintf((char *)search_chrom, "SELECT access from %s.chrom", database);

   struct{
     SQLINTEGER len;
     SQLCHAR val[50];
   }access;

  if(CreateHandle(&hstmt,search_chrom, hdbc, &henv) != SQL_SUCCESS)
  {
    ErrPostEx(SEV_ERROR, 0, 0, "[SHoundAllGenAccess]: Could not create handle.");
    return NULL;
  }
  free(search_chrom);

  /* execute statment */
  sqlrc=SQLExecute(hstmt);
   if (sqlrc != SQL_SUCCESS)
   {
	  ErrPostEx(SEV_ERROR,0,0, "[SHoundAllGenAccess]: unable to execute select statement.");
	  LogSQLInfo(hstmt);
      SQLFreeStmt(hstmt, SQL_DROP);
      return NULL;
   }

  sqlrc=SQLBindCol(hstmt,1,SQL_C_CHAR,access.val,50,(int*)&access.len);
  if (sqlrc != SQL_SUCCESS && sqlrc != SQL_SUCCESS_WITH_INFO)
  {
    ErrPostEx(SEV_ERROR,0,0,"[SHoundAllGenAccess]: unable to execute bind column 1.");
    SQLFreeStmt(hstmt, SQL_DROP);
    return NULL;
  }

   /* fetch the data in columns iteratively */
   SQLINTEGER fetchCount = 0;
   /* fetch result and save in the linked list */
   while((sqlrc=SQLFetch(hstmt)) == SQL_SUCCESS || sqlrc == SQL_SUCCESS_WITH_INFO)
   {
      fetchCount++;
      ptemp = TrimSpace(StrSave((char*)access.val));
      Slri_ValNodeAddStr(&pvnacc, 0, ptemp);
   }

  if(SQLFreeStmt(hstmt, SQL_DROP) != SQL_SUCCESS)
  {
     ErrPostEx(SEV_INFO, 0 ,0, "[SHoundAllGenAccess ]: unable to free statement.");
     print_err(hstmt);
     return NULL;
   }

  if (fetchCount == 0) {
    ErrPostEx(SEV_INFO, 0, 0, "[SHoundAllGenAccess] No data found.");
	return NULL;
  }

  return pvnacc;
}

/*******************************************************/
/* given access, retrieve kloodge                      */
/*Returns : kloodge if successful, 0 otherwise         */
/*******************************************************/
Int4 LIBCALL SHoundGetKloodgeFromAccess(CharPtr paccess)
{
   SQLRETURN retcode = SQL_SUCCESS;
   Int4 ikloodge = 0;

   if(paccess == NULL)
   {
       ErrPostEx(SEV_ERROR,0,0, "[SHoundGetKloodgeFromAccess] Access is NULL.");
       return 0;
   }

   sprintf((char *)chrom_access.val,"%s", IsNullStr(paccess));

   retcode = SQLExecute(HNDLChromSearchAccess);

   if (retcode != SQL_SUCCESS  && retcode != SQL_SUCCESS_WITH_INFO)
   {
	  ErrPostEx(SEV_ERROR,0,0, "[SHoundGetKloodgeFromAccess]: unable to execute search statement. (access=%s)", paccess);
	  LogSQLInfo(HNDLChromSearchAccess);
      return 0;
   }

   retcode = SQLBindCol(HNDLChromSearchAccess, 1, SQL_C_SLONG, 
						&chrom_kloodge.val, 0, NULL);

   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[SHoundGetKloodgeFromAccess]: Could not bind column 1.");
     return 0;
   }

   while((retcode=SQLFetch(HNDLChromSearchAccess)) == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
   {
	   ikloodge = chrom_kloodge.val;
   }

   /* close the cursor and discard all pending results. */
   /* must do in order to re-use the handle created in InitCHROM */
   retcode = SQLFreeStmt(HNDLChromSearchAccess, SQL_CLOSE);
   if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[SHoundGetKloodgeFromAccess]: Error in closing cursor.");
      return 0;
   }

   return ikloodge;

}

/* returns a ValNode list with data.intvalue = kloodge of each DNA unit of completed genomes */
ValNodePtr LIBCALL SHoundAllKloodge(void)
{
   ValNodePtr pvnTax = NULL;
   Int2 rc = 0;
   SQLRETURN sqlrc=SQL_SUCCESS;
   SQLHANDLE hstmt;

   CharPtr search_chrom = (char *) malloc(sizeof(Char) * 150);
   if (search_chrom == NULL)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[SHoundAllKloodge]: Could not allocate space");
     free(search_chrom);
     return FALSE;
   }
   sprintf((char *)search_chrom, "SELECT kloodge from %s.chrom", database);

   struct{
     SQLINTEGER len;
     SQLINTEGER val;
   }kloodge;

  if(CreateHandle(&hstmt,search_chrom, hdbc, &henv) != SQL_SUCCESS)
  {
    ErrPostEx(SEV_ERROR, 0, 0, "[SHoundAllKloodge]: Could not create handle.");
	free(search_chrom);
    return NULL;
  }
  free(search_chrom);

  /* execute statment */
  sqlrc=SQLExecute(hstmt);
   if (sqlrc != SQL_SUCCESS)
   {
	  ErrPostEx(SEV_ERROR,0,0, "[SHoundAllKloodge]: unable to execute select statement.");
	  LogSQLInfo(hstmt);
      SQLFreeStmt(hstmt, SQL_DROP);
      return NULL;
   }

  sqlrc=SQLBindCol(hstmt,1,SQL_C_SLONG,&kloodge.val,0,(int*)&kloodge.len);
  if (sqlrc != SQL_SUCCESS && sqlrc != SQL_SUCCESS_WITH_INFO)
  {
    ErrPostEx(SEV_ERROR,0,0,"[SHoundAllKloodge]: unable to execute bind column 1.");
    SQLFreeStmt(hstmt, SQL_DROP);
    return NULL;
  }

   /* fetch the data in columns iteratively */
   SQLINTEGER fetchCount = 0;
   /* fetch result and save in the linked list */
   while((sqlrc=SQLFetch(hstmt)) == SQL_SUCCESS || sqlrc == SQL_SUCCESS_WITH_INFO)
   {
      fetchCount++;
      Slri_ValNodeAddInt(&pvnTax, 0, kloodge.val);
   }

  if(SQLFreeStmt(hstmt, SQL_DROP) != SQL_SUCCESS)
  {
     ErrPostEx(SEV_INFO, 0 ,0, "[SHoundAllKloodge] : unable to free statement.");
     print_err(hstmt);
     return NULL;
   }

  if (fetchCount == 0) {
    ErrPostEx(SEV_INFO, 0, 0, "[SHoundAllKloodge] No data found");
	return NULL;
  }

  return pvnTax;
}

ValNodePtr LIBCALL GetMoleculeFromChromosome( Int4 chromid, CharPtr type )
{
  Int2       retcode    = SQL_SUCCESS;
  ValNodePtr pvnGis     = NULL;
  Int4       fetchCount = 0;

  if( chromid == 0 || type == NULL)
  {
	ErrPostEx(SEV_ERROR,0,0, "GetMoleculeFromChromosome: "
			  "Bad input");
	return NULL;
  }

  chrom_chromid.val = chromid;
  sprintf((char *)taxgi_type.val,"%s", type);

  if( !ExecuteStmt( &HNDLTaxgiSearchKloodgeType, 1, 
					'd', &taxgi_gi.val, 0 ) )
  {
	ErrPostEx(SEV_ERROR,0,0, "GetMoleculeFromChromosome: "
			  "unable to execute search statement. (chromid=%d)", chromid);
	return NULL;
  }

  while((retcode=SQLFetch(HNDLTaxgiSearchKloodgeType)) == SQL_SUCCESS || 
		 retcode == SQL_SUCCESS_WITH_INFO) 
  {
	fetchCount++;

	pvnGis = Slri_ValNodeAddInt( &pvnGis, 1, taxgi_gi.val );
  }

   /* close the cursor and discard all pending results. */
   /* must do in order to re-use the handle created in InitTaxgi */
  retcode = SQLFreeStmt(HNDLTaxgiSearchKloodgeType, SQL_CLOSE);
  if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
  {
	ErrPostEx(SEV_ERROR, 0, 0, "GetMoleculeFromChromosome: "
			  "Error in closing cursor.");
	return NULL;
  }

  if (fetchCount == 0)  
  {
    ErrPostEx(SEV_INFO, 0, 0, "GetMoleculeFromChromosome: "
			  "No data found for chromid: %ld", chromid);
	return NULL;
  }

  return pvnGis;
}

Int4 LIBCALL TranslateChromFlag( Int4 chromflag )
{
  Int4 flag = 0;

  switch ( chromflag )
  {
    case SOURCE_GENOMIC:
    case SOURCE_MACRONUCLEAR:
	  flag = CHROM_CHROM;
	  break;
    case SOURCE_CHLOROPLAST:
	  flag = CHROM_CHLO;
	  break;
    case SOURCE_MITOCHONDRION:
	  flag = CHROM_MITO;
	  break;
    case SOURCE_PLASTID:
    case SOURCE_CHROMOPLAST:
    case SOURCE_KINETOPLAST:
    case SOURCE_CYANELLE:
    case SOURCE_NUCLEOMORPH:
    case SOURCE_APICOPLAST:
    case SOURCE_LEUCOPLAST:
    case SOURCE_PROPLASTID:
	  flag = CHROM_PLASTID;
	  break;
    case SOURCE_EXTRACHROM:
	  flag = CHROM_ECE;
	  break;
    case SOURCE_PLASMID:
	  flag = CHROM_PLMD;
	  break;
    case SOURCE_TRANSPOSON:
    case SOURCE_INSERTION_SEQ:
	  flag = CHROM_NR;
	  break;
    case SOURCE_PROVIRAL:  
    case SOURCE_VIRION:
    case SOURCE_ENDOGENOUS_VIRUS:
	  flag = CHROM_PHAGE;
	  break;
    default:
	  ErrPostEx(SEV_ERROR,0,0, "TranslateChromFlag: Unknown chromflag in "
				"database.");
	  break;
  }
  return flag;

}

ValNodePtr LIBCALL GetMoleculeFromUnknownChromosome( Int4 taxid, 
													 CharPtr type )
{

Int2       retcode    = SQL_SUCCESS;
  ValNodePtr pvnGis     = NULL;
  Int4       fetchCount = 0;

  if( taxid == 0 || type == NULL)
  {
	ErrPostEx(SEV_ERROR,0,0, "GetMoleculeFromUnknownChromosome: "
			  "Bad input");
	return NULL;
  }

  taxgi_kloodge.val = 0 - taxid;
  sprintf((char *)taxgi_type.val,"%s", type);

  retcode = SQLExecute(HNDLTaxgiSearchKloodgeType);

  if (retcode != SQL_SUCCESS  && retcode != SQL_SUCCESS_WITH_INFO)
  {
	ErrPostEx(SEV_ERROR,0,0, "GetMoleculeFromUnknownChromosome: "
			  "unable to execute search statement. (taxid=%d)", 
			  taxid);
	LogSQLInfo(HNDLTaxgiSearchKloodgeType);
	return NULL;
  }

  retcode = SQLBindCol(HNDLTaxgiSearchKloodgeType, 1, SQL_C_SLONG, 
					   &taxgi_gi.val, 0, NULL);

  if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
  {
	ErrPostEx(SEV_ERROR, 0, 0, "GetMoleculeFromUnknownChromosome: "
			  "Could not bind column 1.");
	return NULL;
  }

  while((retcode=SQLFetch(HNDLTaxgiSearchKloodgeType)) == SQL_SUCCESS || 
		 retcode == SQL_SUCCESS_WITH_INFO)
  {
	fetchCount++;

	pvnGis = Slri_ValNodeAddInt( &pvnGis, 1, taxgi_gi.val );
  }

   /* close the cursor and discard all pending results. */
   /* must do in order to re-use the handle created in InitTaxgi */
  retcode = SQLFreeStmt(HNDLTaxgiSearchKloodgeType, SQL_CLOSE);
  if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
  {
	ErrPostEx(SEV_ERROR, 0, 0, "GetMoleculeFromUnknownChromosome: "
			  "Error in closing cursor.");
	return NULL;
  }

  return pvnGis;

}

/*
$Log: comgen_odbc.c,v $
Revision 1.6  2005/05/02 15:48:50  eburgess
Modified handle HNDLChrom to account for the new projectid field. Bug 5438

Revision 1.5  2005/04/27 15:12:31  eburgess
Changed some free's to Free.

Revision 1.4  2005/04/26 19:03:50  eburgess
Fixed a memory leak.

*/
