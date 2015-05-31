
#include <names_odbc.h>

/* SeqHound Global Variables */
SQLCHAR database[50]; 

/**************************************************************/
/** InitNames
 * 
 *  Initialize the NAMES tables Bioentity and Bioname. Other
 *  NAMES tables are only look up tables and are populated by
 *  scripts. Returns FALSE on failure, TRUE on success.
 */
Boolean LIBCALL InitNAMES()
{
  if( !InitBioentity() )
  {
    ErrPostEx(SEV_ERROR, 0, 0, "InitNAMES: Could not initialize bioentity.");
    return FALSE;
  }

  if( !InitBioname() )
  {
    ErrPostEx(SEV_ERROR, 0, 0, "InitNAMES: Could not initialize bioentity.");
    return FALSE;
  }

  return TRUE;
}

/**************************************************************/
/** CloseNames
 * 
 *  Frees all the statement handles. Returns TRUE.
 */
Boolean LIBCALL CloseNAMES()
{
  if (SQLFreeHandle(SQL_HANDLE_STMT, HNDLBioentity) != SQL_SUCCESS)
    print_err(&HNDLBioentity);

  if (SQLFreeHandle(SQL_HANDLE_STMT, HNDLBioentDeleteGi) != SQL_SUCCESS)
    print_err(&HNDLBioentDeleteGi);

  if (SQLFreeHandle(SQL_HANDLE_STMT, HNDLBioentSearchAccess) != SQL_SUCCESS)
    print_err(&HNDLBioentSearchGi);

  if (SQLFreeHandle(SQL_HANDLE_STMT, HNDLBioentSearchGi) != SQL_SUCCESS)
    print_err(&HNDLBioentSearchGi);

  if (SQLFreeHandle(SQL_HANDLE_STMT, HNDLBioentUpdateId) != SQL_SUCCESS)
    print_err(&HNDLBioentUpdateId);

  if (SQLFreeHandle(SQL_HANDLE_STMT, HNDLBioentMaxId) != SQL_SUCCESS)
    print_err(&HNDLBioentMaxId);

  if (SQLFreeHandle(SQL_HANDLE_STMT, HNDLBioname) != SQL_SUCCESS)
    print_err(&HNDLBioname);

  if (SQLFreeHandle(SQL_HANDLE_STMT, HNDLBionameSearchBioentityId) != 
	  SQL_SUCCESS)
    print_err(&HNDLBionameSearchBioentityId);

 if (SQLFreeHandle(SQL_HANDLE_STMT, HNDLBionameEditId) != SQL_SUCCESS)
    print_err(&HNDLBionameEditId);

  if (SQLFreeHandle(SQL_HANDLE_STMT, HNDLBionameDeleteGi) != SQL_SUCCESS)
    print_err(&HNDLBionameDeleteGi);
  return TRUE;
}

/**************************************************************/
/** InitBioentity
 * 
 *  Set up the statement handles for the bioentity table. Returns
 *  FALSE on failure, TRUE on success.
 */
Boolean LIBCALL InitBioentity()
{
  SQLRETURN retcode;
  CharPtr pSQL_stmt = (Char *) malloc (sizeof(Char) * 150);
  if(pSQL_stmt == NULL)
  {
    ErrPostEx(SEV_ERROR, 0, 0, "InitBIOENTITY: Could not allocate space.");
    return FALSE;
  }
  
  /*-------------------------------------------------------------------
    Create handle to bioentity: variable database is global, and
    is retrieved from .intrezrc in ConnectToDb function. It
    contains the database name, eg: seqhound. 
    ------------------------------------------------------------------- */
   sprintf(pSQL_stmt, "insert into %s.bioentity( id, bioentitytypeid, db, "
		   "access, identifier, field) values(?,?,?,?,?,?)", database);
   if(CreateHandle(&HNDLBioentity,pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "InitBioEntity: Could not create "
		"handle HNDLBioentity");
      free(pSQL_stmt);
      return FALSE;
   }

  /*-------------------------------------------------------------------
    Bind parameters. 
    ------------------------------------------------------------------- */ 
   retcode = SQLBindParameter(HNDLBioentity, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &bioentity_id.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "Bioentity: Could not bind parameter 1, id");
      free(pSQL_stmt);
      return FALSE;
   }
   
   retcode = SQLBindParameter(HNDLBioentity, 2, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &bioentitytype_id.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "Bioentity: Could not bind parameter 2, bioentitytypeid");
      free(pSQL_stmt);
      return FALSE;
   }

   retcode = SQLBindParameter(HNDLBioentity, 3, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR,
							  0, 0, bioentity_db.val, sizeof(bioentity_db.val), NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "InitBioentity: Could not bind parameter 3, db");
      free(pSQL_stmt);
      return FALSE;
   }

   retcode = SQLBindParameter(HNDLBioentity, 4, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 
							  0, 0, bioentity_access.val, sizeof(bioentity_access.val), NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "InitBioentity: Could not bind parameter 4, access");
      free(pSQL_stmt);
      return FALSE;
   }
   
   retcode = SQLBindParameter(HNDLBioentity, 5, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER,
							  0, 0, &bioentity_identifier.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "InitBioentity: Could not bind parameter 5, identifier");
      free(pSQL_stmt);
      return FALSE;
   }

   retcode = SQLBindParameter(HNDLBioentity, 6, SQL_PARAM_INPUT, 
							  SQL_C_SLONG, SQL_INTEGER, 0, 0,
							  &bioentity_field.val, 0, NULL);

   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "InitBioentity: Could not bind parameter 6, field");
      free(pSQL_stmt);
      return FALSE;
   }

   /*------------------------------------------------------------
     Create a handle to delete a record from bioentity based on the
     gi.
     -------------------------------------------------------------*/
   sprintf((char *)pSQL_stmt, "delete from %s.bioentity "
           "where identifier=?", database);

   if( CreateHandle(&HNDLBioentDeleteGi, pSQL_stmt, hdbc, &henv) )
   {
     ErrPostEx(SEV_ERROR, 0, 0, "InitBioentity: Could "
		"not create handle HNDLBioentDeleteGi.");
     free(pSQL_stmt);
     return 0;
   }

   retcode = SQLBindParameter(HNDLBioentDeleteGi, 1, SQL_PARAM_INPUT, 
                    SQL_C_SLONG,SQL_INTEGER, 0, 0, &bioentity_identifier.val, 0, NULL);

   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "InitBioentity: Could not bind "
		"parameter 1 for HNDLBioentDeleteGi, identifier");
      free(pSQL_stmt);
      return FALSE;
   }


  /*------------------------------------------------------------
     Create a handle to search for a record from bioentity based on the
     accession.
     -------------------------------------------------------------*/
   sprintf( (char*)pSQL_stmt, "SELECT id, bioentitytypeid, db, access, "
	    "identifier, field FROM %s.bioentity WHERE access = ?", 
	     database );
  
   if(CreateHandle(&HNDLBioentSearchAccess, pSQL_stmt, hdbc, &henv) != 
	  SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "InitBioentity: Could "
		"not create handle HNDLBioentSearchGi.");
      free(pSQL_stmt);
      return 0;
   }

   retcode = SQLBindParameter(HNDLBioentSearchAccess, 1, SQL_PARAM_INPUT, 
							  SQL_C_CHAR,SQL_CHAR, 0, 0, bioentity_access.val, 
							  sizeof( bioentity_access.val ), NULL);

   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "InitBioentity: Could not bind "
		"parameter 1 for HNDLBioentSearchAccess, access");
      free(pSQL_stmt);
      return FALSE;
   }

  /*------------------------------------------------------------
     Create a handle to search for a record from bioentity based on the
     gi.
     -------------------------------------------------------------*/
   sprintf( (char*)pSQL_stmt, "SELECT id, bioentitytypeid, db, access, "
	    "identifier, field FROM %s.bioentity WHERE identifier = ?", 
	     database );
  
   if(CreateHandle(&HNDLBioentSearchGi, pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "InitBioentity: Could "
		"not create handle HNDLBioentSearchGi.");
      free(pSQL_stmt);
      return 0;
   }

   retcode = SQLBindParameter(HNDLBioentSearchGi, 1, SQL_PARAM_INPUT, 
							  SQL_C_SLONG,SQL_INTEGER, 0, 0, 
							  &bioentity_identifier.val, 0, NULL);
							  

   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "InitBioentity: Could not bind "
		"parameter 1 for HNDLBioentSearchGi, identifier");
      free(pSQL_stmt);
      return FALSE;
   }

 /*------------------------------------------------------------
     Create a handle to update a bioentity record based on the
     id.
     -------------------------------------------------------------*/
   sprintf(pSQL_stmt, "update %s.bioentity set bioentitytypeid = ?, "
		   "db = ?, access = ?, identifier = ?, field = ? where id = ?", 
		   database);

   if(CreateHandle(&HNDLBioentUpdateId, pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "InitBioentity: Could "
				"not create handle HNDLBioentUpdateID.");
      free(pSQL_stmt);
      return 0;
   }
   
   retcode = SQLBindParameter( HNDLBioentUpdateId, 1, SQL_PARAM_INPUT, 
							   SQL_C_SLONG,SQL_INTEGER, 0, 0, 
							   &bioentitytype_id.val, 0, NULL);

   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "InitBioentity: Could not bind "
		"parameter 1 for HNDLBioentUpdateById, bioentitytypeid");
      free(pSQL_stmt);
      return FALSE;
   }
   retcode = SQLBindParameter( HNDLBioentUpdateId, 2, SQL_PARAM_INPUT, 
							   SQL_C_CHAR,SQL_CHAR, 0, 0, bioentity_db.val, 
							   sizeof( bioentity_db.val ), NULL);

   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "InitBioentity: Could not bind "
		"parameter 2 for HNDLBioentUpdateById, db");
      free(pSQL_stmt);
      return FALSE;
   }
   retcode = SQLBindParameter( HNDLBioentUpdateId, 3, SQL_PARAM_INPUT, 
							   SQL_C_CHAR,SQL_CHAR, 0, 0, bioentity_access.val, 
							   sizeof( bioentity_access.val ), NULL);
	
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "InitBioentity: Could not bind "
		"parameter 3 for HNDLBioentUpdateById, access");
      free(pSQL_stmt);
      return FALSE;
   }
						 
   retcode = SQLBindParameter( HNDLBioentUpdateId, 4, SQL_PARAM_INPUT, 
							   SQL_C_SLONG,SQL_INTEGER, 0, 0, 
							   &bioentity_identifier.val, 0, NULL);

   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
	 {
      ErrPostEx(SEV_ERROR, 0, 0, "InitBioentity: Could not bind "
		"parameter 4 for HNDLBioentUpdateById, identifier");
      free(pSQL_stmt);
      return FALSE;
   }

   retcode = SQLBindParameter( HNDLBioentUpdateId, 5, SQL_PARAM_INPUT, 
							   SQL_C_SLONG,SQL_INTEGER, 0, 0, 
							   &bioentity_field.val, 0, NULL);

   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "InitBioentity: Could not bind "
		"parameter 5 for HNDLBioentUpdateById, field");
      free(pSQL_stmt);
      return FALSE;
   }

   retcode = SQLBindParameter( HNDLBioentUpdateId, 6, SQL_PARAM_INPUT, 
							   SQL_C_SLONG,SQL_INTEGER, 0, 0, 
							   &bioentity_id.val, 0, NULL);
   
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "InitBioentity: Could not bind "
				"parameter 6 for HNDLBioentUpdateById, id");
      free(pSQL_stmt);
      return FALSE;
   }

   /*------------------------------------------------------------
     Create a handle to retrieve the record with the max id.
     -------------------------------------------------------------*/

   sprintf((char *)pSQL_stmt, "SELECT MAX(id) as Max "
	  "from %s.bioentity", database);

   if( CreateHandle(&HNDLBioentMaxId, pSQL_stmt, hdbc, &henv) != 
       SQL_SUCCESS )
   {
     ErrPostEx(SEV_ERROR, 0, 0, "InitBioentity: Could "
		"not create handle HNDLBioentMaxId.");
      free(pSQL_stmt);
      return 0;
   }

   free( pSQL_stmt );

   return TRUE;
}

/**************************************************************/
/** AppendRecordBioentity
 *  
 *  @param  pbio  Record to append.
 *
 *  @return  False on failure, True on success.
 *
 */   
Boolean LIBCALL AppendRecordBioentity( StBioentityPtr pbio )
{
  SQLRETURN retcode;

  if( pbio == NULL )
  {
    ErrPostEx(SEV_ERROR,0,0, "AppendRecordBioEntity: NULL parameter.");
    return FALSE;
  }
  
  bioentity_id.val     = pbio->id;
  bioentitytype_id.val = pbio->bioentitytypeid;
  bioentity_identifier.val = pbio->identifier;
  bioentity_field.val = pbio->field;

  sprintf( (char *)bioentity_db.val, "%s", IsNullStr(pbio->db) );
  sprintf( (char *)bioentity_access.val, "%s", IsNullStr(pbio->access) );

  retcode = SQLExecute(HNDLBioentity);

  if (retcode != SQL_SUCCESS)
  {
    ErrPostEx(SEV_ERROR,0,0, "AppendRecordBioentity: "
                             "unable to execute bioentity "
                             "insert statement (Gi:%ld)", 
                              bioentity_identifier.val);
    LogSQLInfo(HNDLBioentity);

    return FALSE;
  }

  return TRUE;
}

/**************************************************************/
/** SearchBioentityByGi
 *  
 *  @param  ppRecord  Record containing the gi (identifier) to
 *                    search by. This record will be filled with
 *                    the results of the search.
 *
 *  @return           Returns TRUE on success, FALSE on
 *                    failure.  
 *
 */  
Boolean LIBCALL SearchBioentityByGi( StBioentityPtr PNTR ppRecord  )
{
  StBioentityPtr pReturn = NULL;  /* returned rows */
  Int4           bioentityId;
  SQLRETURN      retcode;

  if( !ppRecord || !(*ppRecord)->identifier )
  {
    ErrPostEx(SEV_ERROR, 0, 0, "SearchBioentityByGi: bad gi");
    return FALSE;
  }

  pReturn = *ppRecord;

  bioentity_identifier.val = pReturn->identifier;

 /*-------------------------------------------------------------------
    Execute.
    -------------------------------------------------------------------*/
   retcode = SQLExecute(HNDLBioentSearchGi);
   if (retcode != SQL_SUCCESS)
   {
     ErrPostEx(SEV_ERROR,0,0, "SearchBientity: Could not execute select statement.");
     LogSQLInfo(HNDLBioentSearchGi);
     return FALSE;
   }

 /*-------------------------------------------------------------------
    Bind the results.
    -------------------------------------------------------------------*/
   retcode = SQLBindCol(HNDLBioentSearchGi, 1, SQL_C_SLONG, &bioentity_id.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "SearchBioentity: Could not bind "
	       "column 1.");
     return FALSE;
   }

   retcode = SQLBindCol(HNDLBioentSearchGi, 2, SQL_C_SLONG, &bioentitytype_id.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "SearchBioentity: Could not bind "
	       "column 2.");
     return FALSE;
   }

   retcode = SQLBindCol(HNDLBioentSearchGi, 3, SQL_C_CHAR, 
						bioentity_db.val, sizeof( bioentity_db.val), 
						(int*)&bioentity_db.len);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "SearchBioentity: Could not bind "
	       "column 3.");
     return FALSE;
   }

   retcode = 
	 SQLBindCol(HNDLBioentSearchGi, 4, SQL_C_CHAR, 
				bioentity_access.val, sizeof(bioentity_access.val),
				(int*)&bioentity_access.len);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "SearchBioentity: Could not bind "
	       "column 3.");
     return FALSE;
   }

   retcode = SQLBindCol(HNDLBioentSearchGi, 5, SQL_C_SLONG, 
			&bioentity_identifier.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "SearchBioentity: Could not bind "
	       "column 2.");
     return FALSE;
   }

  retcode = SQLBindCol(HNDLBioentSearchGi, 6, SQL_C_SLONG,
					   &bioentity_field.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "SearchBioentity: Could not bind "
	       "column 3.");
     return FALSE;
   }

/* fetch results and save in your linked list */
   SQLINTEGER fetchCount = 0;
   /* fetch result and save in the linked list */
   while((retcode=SQLFetch(HNDLBioentSearchGi)) == SQL_SUCCESS || 
	  retcode == SQL_SUCCESS_WITH_INFO)
   {
      fetchCount++;
      if (fetchCount>1)
      {
         pReturn->next = StBioentityNew();
         pReturn = pReturn->next;
      }
		  
      pReturn->id = bioentity_id.val;
      pReturn->bioentitytypeid = bioentitytype_id.val;
      pReturn->db = StringSave((char*)bioentity_db.val);
      pReturn->access = StringSave((char*)bioentity_access.val);
      pReturn->identifier = bioentity_identifier.val;
      pReturn->field = bioentity_field.val;
   }

 /*------------------------------------------------------
    Close the cursor, but do not free the statement, so that
    it can be used again. Use SQLFreeStmt with the 
    SQL_CLOSE flag.
    --------------------------------------------------------*/
  SQLFreeStmt( HNDLBioentSearchGi, SQL_CLOSE );

  if (fetchCount == 0)
    return FALSE;
 
   return TRUE;
}

/**************************************************************/
/** SearchBioentityByAccess
 * 
 *  Searches the bioentity table by accession. NOTE:this may need
 *  to be generalized to include database name if we include
 *  databases that have similar accessions.
 *  
 *  @param  ppRecord  Record containing the accession to
 *                    search by. This record will be filled with
 *                    the results of the search.
 *
 *  @return           Returns TRUE on success, FALSE on
 *                    failure.  
 *
 */  
Boolean LIBCALL SearchBioentityByAccess( StBioentityPtr PNTR ppRecord  )
{
  StBioentityPtr pReturn = NULL;  /* returned rows */
  SQLRETURN      retcode;

  if( !ppRecord || !(*ppRecord)->access )
  {
    ErrPostEx(SEV_ERROR, 0, 0, "SearchBioentityByAccess: bad accession");
    return FALSE;
  }

  pReturn = *ppRecord;

  sprintf( (char *)bioentity_access.val, "%s", 
		   IsNullStr( pReturn->access ) );
 /*-------------------------------------------------------------------
    Execute.
    -------------------------------------------------------------------*/
   retcode = SQLExecute(HNDLBioentSearchAccess);
   if (retcode != SQL_SUCCESS)
   {
     ErrPostEx(SEV_ERROR,0,0, "SearchBientity: Could not execute select statement.");
     LogSQLInfo(HNDLBioentSearchAccess);
     return FALSE;
   }

 /*-------------------------------------------------------------------
    Bind the results.
    -------------------------------------------------------------------*/
   retcode = SQLBindCol(HNDLBioentSearchAccess, 1, SQL_C_SLONG, &bioentity_id.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "SearchBioentityByAccess: Could not bind "
	       "column 1.");
     return FALSE;
   }

   retcode = SQLBindCol(HNDLBioentSearchAccess, 2, SQL_C_SLONG, &bioentitytype_id.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "SearchBioentityByAccess: Could not bind "
	       "column 2.");
     return FALSE;
   }

   retcode = SQLBindCol(HNDLBioentSearchAccess, 3, SQL_C_CHAR, 
						bioentity_db.val,  sizeof( bioentity_db.val), 
						(int*)&bioentity_db.len);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "SearchBioentityByAccess: Could not bind "
	       "column 3.");
     return FALSE;
   }

   retcode = 
	 SQLBindCol(HNDLBioentSearchAccess, 4, SQL_C_CHAR, 
				bioentity_access.val, sizeof(bioentity_access.val),
				(int*)&bioentity_access.len);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "SearchBioentityByAccess: Could not bind "
	       "column 3.");
     return FALSE;
   }

   retcode = SQLBindCol(HNDLBioentSearchAccess, 5, SQL_C_SLONG, 
			&bioentity_identifier.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "SearchBioentityByAccess: Could not bind "
	       "column 2.");
     return FALSE;
   }

  retcode = SQLBindCol(HNDLBioentSearchAccess, 6, SQL_C_SLONG, 
					   &bioentity_field.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "SearchBioentityByAccess: Could not bind "
	       "column 3.");
     return FALSE;
   }

/* fetch results and save in your linked list */
   SQLINTEGER fetchCount = 0;
   /* fetch result and save in the linked list */
   while((retcode=SQLFetch(HNDLBioentSearchAccess)) == SQL_SUCCESS || 
	  retcode == SQL_SUCCESS_WITH_INFO)
   {
      fetchCount++;
      if (fetchCount>1)
      {
         pReturn->next = StBioentityNew();
         pReturn = pReturn->next;
      }
		  
      pReturn->id = bioentity_id.val;
      pReturn->bioentitytypeid = bioentitytype_id.val;
      pReturn->db = StringSave((char*)bioentity_db.val);
      /*pReturn->access = StringSave((char*)bioentity_access.val);*/
      pReturn->identifier = bioentity_identifier.val;
      pReturn->field = bioentity_field.val;
   }

 /*------------------------------------------------------
    Close the cursor, but do not free the statement, so that
    it can be used again. Use SQLFreeStmt with the 
    SQL_CLOSE flag.
    --------------------------------------------------------*/
  SQLFreeStmt( HNDLBioentSearchAccess, SQL_CLOSE );

  if (fetchCount == 0)
    return FALSE;


   return TRUE;
}

Boolean LIBCALL EditBioentityById( StBioentityPtr pRecord )
{
  SQLRETURN      retcode;

  if( !pRecord || ! pRecord->id )
  {
    ErrPostEx(SEV_ERROR, 0, 0, "EditBioentityById: bad id");
    return FALSE;
  }
  
  bioentity_id.val = pRecord->id;
  bioentitytype_id.val = pRecord->bioentitytypeid;
  bioentity_identifier.val = pRecord->identifier;
  bioentity_field.val = pRecord->field;

  sprintf( (char *)bioentity_db.val, "%s", IsNullStr(pRecord->db) );
  sprintf( (char *)bioentity_access.val, "%s", IsNullStr(pRecord->access) );

 /*-------------------------------------------------------------------
    Execute.
    -------------------------------------------------------------------*/
   retcode = SQLExecute(HNDLBioentUpdateId);
   if (retcode != SQL_SUCCESS)
   {
     ErrPostEx(SEV_ERROR,0,0, "EditBioentityById: "
			   "Could not execute update statement for %d.",
			   bioentity_id.val );
     LogSQLInfo(HNDLBioentUpdateId);
     return FALSE;
   }

   return TRUE;
}

/**************************************************************/
/** InitBioname
 * 
 *  Set up the statement handles for the bioname table. Returns
 *  FALSE on failure, TRUE on success.
 */
Boolean LIBCALL InitBioname()
{
  SQLRETURN retcode;
  CharPtr pSQL_stmt = (Char *) malloc (sizeof(Char) * 500);
  if(pSQL_stmt == NULL)
  {
    ErrPostEx(SEV_ERROR, 0, 0, "InitBIOENTITY: Could not allocate space.");
    return FALSE;
  }
  
  /*-------------------------------------------------------------------
    Create handle to bioentity: variable database is global, and
    is retrieved from .intrezrc in ConnectToDb function. It
    contains the database name, eg: seqhound. 
    ------------------------------------------------------------------- */
   sprintf(pSQL_stmt, "insert into %s.bioname( id, bioentityid, name, "
            "nametypeid, db, access, identifier, field, official, "
            "deprecated, datedeprecated, ruleid, action, actiondate ) "
		    "values(?,?,?,?,?,?,?,?,?,?,?,?,?,NOW())", database);
   if(CreateHandle(&HNDLBioname,pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "InitBioEntity: Could not create "
		"handle HNDLBioname");
      free( pSQL_stmt );
      return FALSE;
   }

  /*-------------------------------------------------------------------
    Bind parameters. 
    ------------------------------------------------------------------- */ 
   retcode = SQLBindParameter(HNDLBioname, 1, SQL_PARAM_INPUT, SQL_C_SLONG, 
                              SQL_INTEGER, 0, 0, &bioname_id.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "Bioname: Could not bind parameter 1, id");
      free( pSQL_stmt );
      return FALSE;
   }
   
   retcode = SQLBindParameter(HNDLBioname, 2, SQL_PARAM_INPUT, SQL_C_SLONG, 
                              SQL_INTEGER, 0, 0, &bioname_bioentityid.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "Bioname: Could not bind parameter 2, bioentitid");
      free( pSQL_stmt );
      return FALSE;
   }

  /*-------------------------------------------------------------------
    Bind bioname_name in AppendRecordBioname() because the size could be large. 
    ------------------------------------------------------------------- */ 

   retcode = SQLBindParameter(HNDLBioname, 4, SQL_PARAM_INPUT, 
                              SQL_C_SLONG, SQL_INTEGER, 0, 0, 
                              &bioname_nametypeid.val, 0, NULL);

   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "InitBioname: Could not bind parameter 4, nametypeid");
      free( pSQL_stmt );
      return FALSE;
   }

   retcode = SQLBindParameter(HNDLBioname, 5, SQL_PARAM_INPUT, 
                              SQL_C_CHAR, SQL_VARCHAR, 0, 0, 
                              bioname_db.val, sizeof(bioname_db.val),
							  (int *)bioname_db.len);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "InitBioname: Could not bind parameter 5, db");
      free( pSQL_stmt );
      return FALSE;
   }
   retcode = SQLBindParameter(HNDLBioname, 6, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 0, 0, 
							  bioname_access.val, sizeof(bioname_access.val), 
							  (int *)bioname_access.len);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "InitBioname: Could not bind parameter 6, access");
      free( pSQL_stmt );
      return FALSE;
   }
   
   retcode = SQLBindParameter(HNDLBioname, 7, SQL_PARAM_INPUT, SQL_C_SLONG, 
                              SQL_INTEGER, 0, 0, &bioname_identifier.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "InitBionameName: Could not bind parameter 7, identifier");
      free( pSQL_stmt );
      return FALSE;
   }

   retcode = SQLBindParameter(HNDLBioname, 8, SQL_PARAM_INPUT, SQL_C_SLONG,
                              SQL_INTEGER, 0, 0, &bioname_field.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "InitBioname: Could not bind parameter 8, field");
      free( pSQL_stmt );
      return FALSE;
   }

    retcode = SQLBindParameter(HNDLBioname, 9, SQL_PARAM_INPUT, SQL_C_SLONG, 
                              SQL_INTEGER, 0, 0, &bioname_official.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "Bioname: Could not bind parameter 9, official");
      free( pSQL_stmt );
      return FALSE;
   }

   retcode = SQLBindParameter(HNDLBioname, 10, SQL_PARAM_INPUT, SQL_C_SLONG, 
                              SQL_INTEGER, 0, 0, &bioname_deprecated.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "Bioname: Could not bind parameter 10, deprecated");
      free( pSQL_stmt );
      return FALSE;
   }

   retcode = SQLBindParameter(HNDLBioname, 11, SQL_PARAM_INPUT, 
							  SQL_C_TIMESTAMP, SQL_TIMESTAMP, 0, 0, 
                              bioname_datedep.val, 
							  sizeof(bioname_datedep.val), 
							  (int *)bioname_datedep.len);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "InitBioname: Could not bind parameter 11, datedeprecated");
      return FALSE;
   }
   
   retcode = SQLBindParameter(HNDLBioname, 12, SQL_PARAM_INPUT, SQL_C_SLONG, 
                              SQL_INTEGER, 0, 0, &bioname_ruleid.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "Bioname: Could not bind parameter 12, ruleid");
      free( pSQL_stmt );
      return FALSE;
   }
   
   retcode = SQLBindParameter(HNDLBioname, 13, SQL_PARAM_INPUT, SQL_C_SLONG, 
                              SQL_INTEGER, 0, 0, &bioname_action.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "Bioname: Could not bind parameter 13, ruleid");
      free( pSQL_stmt );
      return FALSE;
   }

   /*  retcode = SQLBindParameter(HNDLBioname, 14, SQL_PARAM_INPUT, 
							  SQL_C_CHAR, SQL_VARCHAR, 0, 0, 
                              bioname_actiondate.val, 
							  sizeof(bioname_actiondate.val), 
							  (int *)bioname_actiondate.len);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "InitBioname: Could not bind parameter 11, datedeprecated");
      return FALSE;
	  }*/
 /*------------------------------------------------------------
     Create a handle to search bioname based on bioentity id.
     -------------------------------------------------------------*/
   sprintf( (char*)pSQL_stmt, "SELECT id, bioentityid, name, "
			"nametypeid, db, access, identifier, field, official, "
			"deprecated, "
			"DATE_FORMAT(datedeprecated, '%%Y-%%m-%%d %%H:%%i:%%s' ), "
			" ruleid, action, "
			"DATE_FORMAT( actiondate,  '%%Y-%%m-%%d %%H:%%i:%%s' ) "
			"FROM %s.bioname WHERE bioentityid = ?", database );

   if( CreateHandle(&HNDLBionameSearchBioentityId ,pSQL_stmt, hdbc, 
					&henv) != SQL_SUCCESS )
   {
      ErrPostEx(SEV_ERROR, 0, 0, "InitBioEntity: "
		"Could not create handle HNDLBionameDeleteGi");
      free( pSQL_stmt );
      return FALSE;
   }

    retcode = 
	  SQLBindParameter(HNDLBionameSearchBioentityId, 1, 
					   SQL_PARAM_INPUT, SQL_C_SLONG,SQL_INTEGER,  0,
					   0, &bioname_bioentityid.val, 0, NULL);

   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "InitBioname: Could not bind "
		"parameter 1 for HNDLBionameSearchBioentityId, bioentityid");
      free(pSQL_stmt);
      return FALSE;
   }

 /*------------------------------------------------------------
     Create a handle to Edit a bioname based on bioname id.

	 NOTE: for now we are setting the date to be the actual date
           of the update.
     -------------------------------------------------------------*/
   sprintf( (char*)pSQL_stmt, "UPDATE %s.bioname set action = ?, "
			"actiondate = NOW() WHERE id = ?", 
			database );

 if(CreateHandle(&HNDLBionameEditId, pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "InitBioentity: Could "
				"not create handle HNDLBionameEditId.");
      free(pSQL_stmt);
      return FALSE;
   }
   
   retcode = SQLBindParameter( HNDLBionameEditId, 1, SQL_PARAM_INPUT, 
							   SQL_C_SLONG,SQL_INTEGER, 0, 0, 
							   &bioname_action.val, 0, NULL);

   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "InitBioentity: Could not bind "
		"parameter 1 for HNDLBionameEditId, action");
      free(pSQL_stmt);
      return FALSE;
   }

   /* retcode = SQLBindParameter( HNDLBionameEditId, 2, SQL_PARAM_INPUT, 
							   SQL_C_CHAR,SQL_CHAR, 0, 0, 
							   bioname_actiondate.val, 
							  sizeof( bioname_actiondate.val), NULL);

   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "InitBioentity: Could not bind "
		"parameter 2 for HNDLBionameEditId, actiondate");
      free(pSQL_stmt);
      return FALSE;
	  }*/


   retcode = SQLBindParameter( HNDLBionameEditId, 2, SQL_PARAM_INPUT, 
							   SQL_C_SLONG,SQL_INTEGER, 0, 0, &bioname_id.val, 
							   0, NULL);

   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "InitBioentity: Could not bind "
		"parameter 2 for HNDLBionameEditId, id");
      free(pSQL_stmt);
      return FALSE;
   }
 
  /*------------------------------------------------------------
     Create a handle to delete a bioname record based on the gi.
     -------------------------------------------------------------*/
   sprintf((char *)pSQL_stmt, "delete from %s.bioname where "
	   "identifier=?", database);

   if( CreateHandle(&HNDLBionameDeleteGi ,pSQL_stmt, hdbc, &henv) != 
       SQL_SUCCESS )
   {
      ErrPostEx(SEV_ERROR, 0, 0, "InitBioEntity: "
		"Could not create handle HNDLBionameDeleteGi");
      free( pSQL_stmt );
      return FALSE;
   }

   retcode = SQLBindParameter(HNDLBionameDeleteGi, 1, SQL_PARAM_INPUT,
			      SQL_C_SLONG, SQL_INTEGER, 0, 0,
			      &bioname_identifier.val, 0, NULL);

   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "Bioname: Could not bind "
		"parameter 1 for HNDLBionameDeleteGi, identifier");
      free( pSQL_stmt );
      return FALSE;
   }
   free( pSQL_stmt );

   return TRUE;
}

/**************************************************************/
/** AppendRecordBioname
 *  
 *  @param  pbio  Record to append.
 *
 *  @return  False on failure, True on success.
 *
 */  
Boolean LIBCALL AppendRecordBioname( StBionamePtr pbio )
{
  SQLRETURN retcode;

  /* if( pbio == NULL )
  {
    ErrPostEx(SEV_ERROR,0,0, "AppendRecordBioname: NULL parameter.");
    return FALSE;
	}*/

  while( pbio )
  {
	bioname_id.val                = pbio->id;
	bioname_bioentityid.val       = pbio->bioentityid;
	bioname_nametypeid.val        = pbio->nametypeid;
	bioname_identifier.val        = pbio->identifier;
	bioname_field.val             = pbio->field;
	bioname_official.val          = pbio->official;
	bioname_deprecated.val        = pbio->deprecated;
	bioname_ruleid.val            = pbio->ruleid;
	bioname_action.val            = pbio->action;
	/*-----------------------------------------------------------
	  NOTE: the name is bound here, because the length could be
            very large.
	  ------------------------------------------------------------*/
	if( pbio->name != NULL )
	{
	  retcode = SQLBindParameter(HNDLBioname, 3, SQL_PARAM_INPUT, 
								 SQL_C_CHAR, SQL_LONGVARCHAR, 
								 strlen(pbio->name), 0, 
								 pbio->name, 0, NULL);
	}
	else
	{
	  retcode = SQLBindParameter(HNDLBioname, 3, SQL_PARAM_INPUT, 
								 SQL_C_CHAR, SQL_LONGVARCHAR, 
								 strlen(""), 0, "", 0, NULL);
	}

	if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
	{
	  ErrPostEx(SEV_ERROR, 0, 0, "AppendRecordBioname: Could not bind parameter 8");
	  return FALSE;
	  }

	sprintf( (char *)bioname_db.val, "%s", IsNullStr(pbio->db) );
	sprintf( (char *)bioname_access.val, "%s", IsNullStr(pbio->access) );
	sprintf( (char *)bioname_datedep.val, "%s",
			 IsNullStr(pbio->date) );

	/*sprintf( (char *)bioname_actiondate.val, "%s",
	  IsNullStr(pbio->actiondate) );*/

	retcode = SQLExecute(HNDLBioname);

	if (retcode != SQL_SUCCESS)
	{
	  ErrPostEx(SEV_ERROR,0,0, "AppendRecordBioname: "
				"unable to execute bioname "
				"insert statement (Gi:%ld)", 
				bioname_identifier.val);
	  LogSQLInfo(HNDLBioname);
	  
	  return FALSE;
	}

	pbio = pbio->next;
  }

  return TRUE;
}

/**************************************************************/
/** SearchBionameByBioentityId
 *  
 *  Gets all the names for a give bioentity. Note that the date
 *  fields are not returned at the moment.
 *
 *  @param  ppRecord  Record containing the bioentityid to
 *                    search by. This record will be filled with
 *                    the results of the search.
 *
 *  @return           Returns the TRUE on success, FALSE on
 *                    failure.  
 *
 */  
Boolean LIBCALL SearchBionameByBioentityId(StBionamePtr PNTR ppRecord)
{
  StBionamePtr pReturn = NULL;  /* returned rows */
  SQLRETURN      retcode;

  if( !ppRecord || !(*ppRecord)->bioentityid )
  {
    ErrPostEx(SEV_ERROR, 0, 0, "SearchBionameByBioentityId: bad id");
    return FALSE;
  }

  pReturn = *ppRecord;

  bioname_bioentityid.val = pReturn->bioentityid;

 /*-------------------------------------------------------------------
    Execute.
    -------------------------------------------------------------------*/
   retcode = SQLExecute(HNDLBionameSearchBioentityId);
   if (retcode != SQL_SUCCESS)
   {
     ErrPostEx(SEV_ERROR,0,0, "SearchBionameByBioentityId: "
			   "Could not execute select statement.");
     LogSQLInfo(HNDLBioentSearchGi);
     return FALSE;
   }

 /*-------------------------------------------------------------------
    Bind the results.
    -------------------------------------------------------------------*/
   retcode = 
	 SQLBindCol(HNDLBionameSearchBioentityId, 1, SQL_C_SLONG, 
				&bioname_id.val, 0, NULL);

   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "SearchBionameByBioentityId: "
			   "Could not bind column 1." );

     return FALSE;
   }

   retcode = SQLBindCol(HNDLBionameSearchBioentityId, 2, SQL_C_SLONG, 
						&bioname_bioentityid.val, 0, NULL);

   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "SearchBionameByBioentityId: Could not bind "
	       "column 2.");
     return FALSE;
   }

  /*----------------------------------------------------------------
   NOTE: We must set a large length so that the name is not truncated.
   --------------------------------------------------------------------*/ 
   retcode = SQLBindCol(HNDLBionameSearchBioentityId, 3, SQL_C_CHAR, 
						bioname_name.val, TEXT_LENGTH, 
						(int*)bioname_name.len);
						
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "SearchBionameByBioentityId: "
			   "Could not bind column 3.");
	       
     return FALSE;
   }

   retcode = SQLBindCol(HNDLBionameSearchBioentityId, 4, SQL_C_SLONG, 
			&bioname_nametypeid.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "SearchBionameByBioentityId: "
			   "Could not bind column 4.");
	       
     return FALSE;
   }
   retcode = SQLBindCol(HNDLBionameSearchBioentityId, 5, SQL_C_CHAR, 
						bioname_db.val, 15, 
						(int*)bioname_db.len);
						
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "SearchBionameByBioentityId: "
			   "Could not bind column 5.");
	       
     return FALSE;
   }

   retcode = 
	 SQLBindCol(HNDLBionameSearchBioentityId, 6, SQL_C_CHAR, 
				bioname_access.val, 20, 
				(int*)&bioname_access.len);
			
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "SearchBionameByBioentityId: "
			   "Could not bind column 6.");
	       
     return FALSE;
   }

   retcode = SQLBindCol(HNDLBionameSearchBioentityId, 7, SQL_C_SLONG, 
			&bioname_identifier.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "SearchBionameByBioentityId: "
			   "Could not bind column 7.");
	       
     return FALSE;
   }

  retcode = SQLBindCol(HNDLBionameSearchBioentityId, 8, SQL_C_SLONG,
					   &bioname_field.val, 0, NULL);

   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "SearchBionameByBioentityId: "
			   "Could not bind column 8.");
	       
     return FALSE;
   }

   retcode = SQLBindCol(HNDLBionameSearchBioentityId, 9, SQL_C_SLONG, 
						&bioname_official.val, 1, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "SearchBionameByBioentityId: "
			   "Could not bind column 9.");
	       
     return FALSE;
   }

   retcode = SQLBindCol(HNDLBionameSearchBioentityId, 10, SQL_C_SLONG, 
						&bioname_deprecated.val, 1, NULL);

   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "SearchBionameByBioentityId: "
			   "Could not bind column 10.");
	       
     return FALSE;
   }

   retcode = SQLBindCol(HNDLBionameSearchBioentityId, 11, SQL_C_CHAR, 
						bioname_datedep.val, 18,
						(int*)bioname_datedep.len);
						
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "SearchBionameByBioentityId: "
			   "Could not bind column 11.");
	       
     return FALSE;
   }

   retcode = SQLBindCol(HNDLBionameSearchBioentityId, 12, SQL_C_SLONG, 
						&bioname_ruleid.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "SearchBionameByBioentityId: "
			   "Could not bind column 12.");
	       
     return FALSE;
   }

   retcode = SQLBindCol(HNDLBionameSearchBioentityId, 13, SQL_C_SLONG, 
						&bioname_action.val, 0, NULL);

   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "SearchBionameByBioentityId: "
			   "Could not bind column 13.");
	       
     return FALSE;
   }
   
   retcode = SQLBindCol(HNDLBionameSearchBioentityId, 14, SQL_C_CHAR, 
						bioname_actiondate.val, 18, 	NULL); 
						
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "SearchBionameByBioentityId: "
			   "Could not bind column 14.");
	       
     return FALSE;
	 }

   SQLINTEGER fetchCount = 0;
   /* fetch result and save in the linked list */
   while((retcode=SQLFetch(HNDLBionameSearchBioentityId)) == SQL_SUCCESS || 
	  retcode == SQL_SUCCESS_WITH_INFO)
   {
      fetchCount++;
      if (fetchCount>1)
      {
         pReturn->next = StBionameNew();
         pReturn = pReturn->next;
      }
		  
      pReturn->id = bioname_id.val;
	  pReturn->bioentityid = bioname_bioentityid.val;
	  pReturn->name = StringSave((char*)bioname_name.val);
      pReturn->nametypeid = bioname_nametypeid.val;
      pReturn->db = StringSave((char*)bioname_db.val);
      pReturn->access = StringSave((char*)bioname_access.val);
      pReturn->identifier = bioname_identifier.val;
      pReturn->field = bioname_field.val;
      pReturn->official = bioname_official.val;
      pReturn->deprecated = bioname_deprecated.val;
      pReturn->date= StringSave((char*)bioname_datedep.val);
      pReturn->ruleid = bioname_ruleid.val;
      pReturn->action = bioname_action.val;
	  pReturn->actiondate = 
		StringSave((char*)bioname_actiondate.val);
   }

 /*------------------------------------------------------
    Close the cursor, but do not free the statement, so that
    it can be used again. Use SQLFreeStmt with the 
    SQL_CLOSE flag.
    --------------------------------------------------------*/
  SQLFreeStmt( HNDLBionameSearchBioentityId, SQL_CLOSE );

  if (fetchCount == 0)
    return FALSE;

   return TRUE;
}

Boolean LIBCALL EditBionameById( StBionamePtr pBioname )
{
 SQLRETURN    retcode;
 StBionamePtr pRecord = pBioname;

 /*  if( !pRecord || ! pRecord->id )
  {
    ErrPostEx(SEV_ERROR, 0, 0, "EditBionameById: bad id");
    return FALSE;
	}*/
  
  while( pRecord )
  {
	bioname_id.val     = pRecord->id;
	bioname_action.val = pRecord->action;
	/*sprintf( (char *)bioname_actiondate.val, "%s", 
	  IsNullStr(pRecord->actiondate) );*/

 /*------------------------------------------------------------------
    Execute.
    -------------------------------------------------------------------*/
	retcode = SQLExecute(HNDLBionameEditId);

	if (retcode != SQL_SUCCESS  && retcode != SQL_SUCCESS_WITH_INFO)
	{
	  ErrPostEx(SEV_ERROR,0,0, "EditBionameById: "
				"Could not execute update statement for %d.",
				bioname_id.val );
	  LogSQLInfo(HNDLBionameEditId);
	  return FALSE;
	}
	
	pRecord = pRecord->next;
  }

  return TRUE;
}

/**  GetLargestBioentityId
 *
 *   Returns the largest id in the Bioentity table. Used after
 *   inserts to get the newly created id, as this is generated by
 *   AUTO_INCREMENT.
 *
 *   @returns 0 if the table has no records or there is a problem 
 *              with the query.
 */
Int4 LIBCALL GetLargestBioentityId()
{
  Int2 sqlrc=SQL_SUCCESS;
  Int4 retval = -1;

  struct
  {
    Int4       len;
    SQLINTEGER val;
  }max;

  /*----------------------------------------------------
    If the table is empty, NULL will be returned for max(id).
    Therefore, initialize max to avoid weird numbers.
    ----------------------------------------------------*/
  max.val=0; 
  max.len=0;

  /* execute statment */
  sqlrc = SQLExecute(HNDLBioentMaxId);
  if (sqlrc != SQL_SUCCESS)
  {
    ErrPostEx(SEV_ERROR,0,0, "GetLargestBioentity:unable to "
              "execute select statement.");
    LogSQLInfo(HNDLBioentMaxId);
    return 0;
  }

  SQLBindCol(HNDLBioentMaxId,1,SQL_C_SLONG,&max.val,0,&max.len);

  sqlrc = SQLFetch(HNDLBioentMaxId);
  if(sqlrc == SQL_NO_DATA_FOUND){
    ErrPostEx(SEV_INFO, 0, 0, "GetLargestBioentity: No data found");
    return 0;
  }

  if(sqlrc == SQL_SUCCESS || sqlrc == SQL_SUCCESS_WITH_INFO)
  {
    retval =  max.val;
  }

  /*------------------------------------------------------
    Close the cursor, but do not free the statement, so that
    it can be used again. Use SQLFreeStmt with the 
    SQL_CLOSE flag.
    --------------------------------------------------------*/
  SQLFreeStmt( HNDLBioentMaxId, SQL_CLOSE );

  return retval;
}

/**************************************************************/
/** DeleteGiinBioentity
 *
 *  Deletes a record in bioentity based on the gi. Obsolete.
 *                    We no longer delete bioentity records.
 *  
 *  @param  gi        gi of the record to be deleted. 
 *
 *  @return           Returns the TRUE on success, FALSE on
 *                    failure.  
 *
 */  
Boolean LIBCALL DeleteGIinBioentity(SQLINTEGER gi)
{
  Int2 sql_return = SQL_SUCCESS;

  bioentity_identifier.val = gi;

  sql_return = SQLExecute(HNDLBioentDeleteGi);

  /* execute statment */
  if (sql_return != SQL_SUCCESS)
  {
    ErrPostEx(SEV_ERROR,0,0, "DeleteGIinBioentity:unable to execute delete statement.");
    LogSQLInfo(HNDLBioentDeleteGi);
	  
    return FALSE;
  }

  return TRUE;
}

/**************************************************************/
/** DeleteGiinBioname
 *
 *  Deletes a record in bioname based on the gi. Obsolete.
 *                    We no longer delete bioentity records.
 *  
 *  @param  gi        gi of the record to be deleted. 
 *
 *  @return           Returns the TRUE on success, FALSE on
 *                    failure.  
 *
 */  
Boolean LIBCALL DeleteGIinBioname(SQLINTEGER gi)
{
   Int2 sql_return = SQL_SUCCESS;

   bioname_identifier.val = gi;
   
   sql_return = SQLExecute( HNDLBionameDeleteGi);

   /* execute statment */
   if (sql_return != SQL_SUCCESS)
   {
     ErrPostEx(SEV_ERROR,0,0, "DeleteGIinBioname:unable to execute delete statement.");
     LogSQLInfo( HNDLBionameDeleteGi );
     
     return FALSE;
   }

   return TRUE;
}

