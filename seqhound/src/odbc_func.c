/* $Id: odbc_func.c,v 1.1 2005/04/26 19:05:21 eburgess Exp $
*******************************************************************************************
  Copyright (C) 2000 Mount Sinai Hospital (MSH)

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of
  the License, or any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
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

  April 1, 2005  Original MSH Revision.
                    Hogue Lab - University of Toronto Biochemistry
                    Department and the Samuel Lunenfeld Research
                    Institute, Mount Sinai Hospital
                    http://bioinfo.mshri.on.ca  hogue@mshri.on.ca
                    ftp://ftp.mshri.on.ca/


  PUBLICATION TO CITE:


  DESCRIPTION
  Functions to prepare and execute SQL statements.


******************************************************************************************/

#include <odbc_func.h>

/*********************************************************************
 *  PrepStmt
 *
 * Prepares a statement, including binding of parameters. The 
 * function takes a variable number of arguments depending on the
 * number of parameters that need to be bound. For each parameter to
 * be bound, you must give 3 parameters:
 *            1. The argument type. This is 'd' for integers and
 *               's' for strings.
 *            2. The buffer that will hold the argument. usually
 *               something like genedbxref_dbname. Note that this
 *               should be a pointer. If the argument type is 'd', an
 *               integer, you should pass in the pointer to the
 *               location that the integer will be stored.
 *            3. The length. This is only used for strings and can
 *               be set to 0 for integers.
 *
 * @param handle  The handle to the statement to be prepared. Required.
 *
 * @param pSQL    The sql statement to be used. Required.
 *
 * @param numArgs The number of arguments to be bound. Required.
 *
 * @param argType The type of the argument: 'd' = integer
 *                                          's' = string
 * @param buffer  The buffer to be bound.
 *
 * @param length  The length of the buffer
 * 
 * more arguments here if desired.
 *
 * @return     True if successful, false if not successful
 */
Boolean PrepStmt( SQLHANDLE *handle, CharPtr pSQL, 
				  Int4 numArgs, ... )				 
{
  va_list args;

  CharPtr       charValue;
  SQLINTEGER   *intValue;
  Int4           argLength;

  Int4 argCount = 0;
  Int4 argType  = 0;

  Int4 retcode = 0;

  if( CreateHandle( handle, pSQL, hdbc, &henv ) != SQL_SUCCESS )
  {
	ErrPostEx(SEV_ERROR, 0, 0, "[PrepStmt]: Could not create "
			  "handle for %s.", pSQL);
	return FALSE;
  }

  va_start( args, numArgs );

  while( argCount < numArgs )
  {
	argType = va_arg( args, int );
	argCount++;
	switch( argType )
	{
	case 'd':
	  intValue  = va_arg( args, SQLINTEGER * );

	  /*--------------------------------------------------------
		Throw away the length as we don't need it for ints.
		-------------------------------------------------------*/
	  argLength = va_arg( args, int );
	 
	  retcode = SQLBindParameter(*handle, argCount,
								 SQL_PARAM_INPUT,
								 SQL_C_SLONG,SQL_INTEGER,  0,
								 0, intValue, 
								 0, NULL);

	 
	  if(retcode != SQL_SUCCESS && 
		 retcode != SQL_SUCCESS_WITH_INFO)
	  {
		ErrPostEx(SEV_ERROR, 0, 0, "[PrepStmt]: Could not bind "
				  "parameter %d for %s: %ld", argCount, pSQL, 
				 intValue);
		SQLFreeHandle(SQL_HANDLE_STMT, 
					  handle);
		return FALSE;
	  }
	  break;
	case 's':
	  charValue = va_arg( args, char * );
	  argLength = va_arg( args, int );

	  retcode = SQLBindParameter(*handle, argCount,
								 SQL_PARAM_INPUT,  SQL_C_CHAR,
								 SQL_VARCHAR,  0,
								 0,  charValue, argLength,
								 NULL );

	  if(retcode != SQL_SUCCESS && 
		 retcode != SQL_SUCCESS_WITH_INFO)
	  {
		ErrPostEx(SEV_ERROR, 0, 0, "[PrepStmt]: Could not bind "
				  "parameter %d for %s: %x", argCount, pSQL,
				  charValue);
		SQLFreeHandle(SQL_HANDLE_STMT,  handle);
					 
		return FALSE;
	  }
	  break;
	case 'b':
	  /*--------------------------------------------------------
		If the column is a text or blob field, do not bind the
		parameter here as we do not know its size. Bind it just 
		before execution 'by hand'.
		----------------------------------------------------*/
	  break;
	case 't':
	  /*------------------------------------------------------
		date field: to be coded later.
		-----------------------------------------------------*/
	  break;
	default:
	  ErrPostEx(SEV_ERROR, 0, 0, "[PrepStmt]: Unknown argument "
				"type. %c for %s", argType, pSQL );
	}

	
  }
  va_end( args );
  return TRUE;
}	

/*********************************************************************
 * ExecuteStmt
 *
 * Function to execute a SQL statement and bind the required columns.
 *
 * The function takes a variable number of arguments depending on the
 * columns to be bound. Each column requires 3 arguments:
 *      1. Argument type: 'd' = integer, 's' = string.
 *      2. Buffer: The buffer that will contain the returned values.
 *         Note that this argument requires a pointer. Pass the address
 *         for an integer.
 *      3. Length: The length of the buffer. This is not used for
 *         integers and should be 0.
 *
 * @param handle   A pointer to the handle to be executed. Required.
 *
 * @param numArgs  The number of columns to be bound. Required.
 *
 * @param colType  The type of the column to be bound.
 *
 * @param buffer   The buffer to be bound to the column. Note that 
 *                 this is always a pointer, even if the column type 
 *                 is integer.
 * @param length   The length of the buffer. Only used for string types.
 *
 * more parameters if more columns are to be bound.
 *      
 * @return Boolean. True upon success, false upon failure.
 * 
 */
Boolean ExecuteStmt( SQLHANDLE *handle, Int4 numArgs, ... )
{
  va_list args;

  CharPtr       charValue;
  SQLINTEGER   *intValue;
  Int4          argLength;

  Int4 argCount = 0;
  Int4 argType  = 0;

  Int2 retcode = 0;

  retcode = SQLExecute( *handle );

  if (retcode != SQL_SUCCESS  && retcode != SQL_SUCCESS_WITH_INFO)
  {
	ErrPostEx(SEV_ERROR,0,0, "ExecuteStmt: unable to execute "
			  "search statement.");
	LogSQLInfo( *handle);
	return FALSE;
  }

  va_start( args, numArgs );

  while( argCount < numArgs )
  {
	argType = va_arg( args, int );

	argCount++;

	switch( argType )
	{
	case 'd':
	  intValue  = va_arg( args, SQLINTEGER * );
	  argLength = va_arg( args, int );

	  /*--------------------------------------------------------
		Throw away the length as we don't need it for ints.
		-------------------------------------------------------*/
	  retcode = SQLBindCol( *handle, argCount, 
							SQL_C_SLONG, 
							intValue, 0, NULL);
	  if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
	  {
		ErrPostEx(SEV_ERROR, 0, 0, "ExecuteStmt: "
				  "Could not bind column %d.", argCount);
	       
		return FALSE;
	  }
	  break;
	case 's':

	  charValue  = va_arg( args, CharPtr );
	  argLength  = va_arg( args, int );

	  retcode = SQLBindCol( *handle, argCount, SQL_C_CHAR,
							charValue, argLength, &argLength );
						
	  if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
	  {
		ErrPostEx(SEV_ERROR, 0, 0, "ExecuteStmt: "
				  "Could not bind column %d.", argCount);
	       
		return FALSE;
	  }
	  break;
	case 'b':
	  break;
	case 't':
	  break;
	}
  }

  va_end( args );

  return TRUE;
}	
					 
  
