/* $Id: nblast_odbc.c,v 1.6 2004/08/06 14:47:22 mmatan Exp $
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
           Michel Dumontier (micheld@mshri.on.ca);
	   Christopher W.V. Hogue hogue@mshri.on.ca
	   Renan A. CAVERO rcavero@blueprint.org


  REVISION/CONTRIBUTION NOTES:

  June 1, 2000  Original MSH Revision.
                Hogue Lab - University of Toronto Biochemistry
                Department and the Samuel Lunenfeld Research
                Institute, Mount Sinai Hospital
                http://bioinfo.mshri.on.ca  hogue@mshri.on.ca
                ftp://ftp.mshri.on.ca/



  PUBLICATION TO CITE:
	M Dumontier, CWV Hogue. NBLAST: a cluster variant of BLAST for NxN comparisons.
	BMC Bioinformatics, 2002, 3: 13.



  DESCRIPTION
  January 2004
  SeqHound NBlast Module.


*******************************************************************************/


#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>

/* #include <intrez_odbc.h> */

#include <nblast_odbc.h>
#include <objslristruc.h>
#include <slri_math.h>
#include <slri_odbc.h>

#include <ncbi.h>
#include <asn.h>
extern SQLHDBC hdbc;
extern SQLHENV henv;

/* SeqHound Global Variables */
/*  int connected;  */
extern SQLCHAR database[50];





/* initialization of BlastDB for insertion of records:
 *  1) Creates a handle for the insertion command and
 *  2) binds the parameters.
 *  This function is called by Init or OpenBlastDB. To insert into
 *  the BlastDB table, used by  AppendRecordtoBlastDB */
Boolean InitBlastDB(void)
{
   SQLRETURN retcode;
   CharPtr pSQL_stmt = (Char *) malloc (sizeof(Char) * 150);
   if(pSQL_stmt == NULL)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[InitBlastDB]: Could not allocate space.");
     return FALSE;
   }

   /* create handle */
   sprintf(pSQL_stmt, "insert into %s.blastdb(uid, eval, asnsa) values(?, ?, ?)", database);
   if(CreateHandle(&HNDLBlastdb, pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitBlastDB]: Could not create handle to insert statement.");
      return FALSE;
   }
   /* binding paramters 1 & 2 */
   retcode = SQLBindParameter(HNDLBlastdb, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &blastdb_uid.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitBlastDB]: Could not bind parameter 1.");
      return FALSE;
   }

   retcode = SQLBindParameter(HNDLBlastdb, 2, SQL_PARAM_INPUT, SQL_C_DOUBLE, SQL_DOUBLE, 0, 0, &blastdb_eval.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitBlastDB]: Could not bind parameter 2.");
      return FALSE;
   }

    return TRUE;
}



/* initialization of NBlastDB for insertion of records:
 *  1) Creates a handle for the insertion command and
 *  2) binds the parameters.
 *  This function is called by Init or OpenNBlastDB. To insert into
 *  the NBlastDB table, used by AppendRecordtoNBlastDB */
Boolean InitNBlastDB(void)
{
    printf("InitNBlastDB Start \n");
   SQLRETURN retcode;
   CharPtr pSQL_stmt = (Char *) malloc (sizeof(Char) * 150);
   if(pSQL_stmt == NULL)
   {
       ErrPostEx(SEV_ERROR, 0, 0, "[InitNBlastDB]: Could not allocate space.");
       return FALSE;
   }
   /* create handle */
   sprintf(pSQL_stmt, "insert into %s.nblastdb(ord, gi, num, asnbr) values(?, ?, ?, ?)", database);
   if(CreateHandle(&HNDLNBlastdb, pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
   {
       ErrPostEx(SEV_ERROR, 0, 0, "[InitNBlastDB]: Could not create handle to insert statement.");
       return FALSE;
   }
   /* binding paramters 1, 2, 3                                */
   /* 4th Parameter will be bind under AppendRecordtoNBlastDB. */
   retcode = SQLBindParameter(HNDLNBlastdb, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &nblastdb_ord.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitNBlastDB]: Could not bind parameter 1.");
      return FALSE;
   }

   retcode = SQLBindParameter(HNDLNBlastdb, 2, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &nblastdb_gi.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "[InitNBlastDB]: Could not bind parameter 2.");
      return FALSE;
   }

   retcode = SQLBindParameter(HNDLNBlastdb, 3, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &nblastdb_num.val, 0, NULL);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
       ErrPostEx(SEV_ERROR, 0, 0, "[InitNBlastDB]: Could not bind parameter 3.");
       return FALSE;
   }
 printf("InitNBlastDB END \n");
   return TRUE;
}



/* Initialize codebase data structure
 * This function will change name to InitNBLAST  to initialize the module.
 * ConnectToDb() needs to be removed from here.
*/

SLRI_ERR NBlastDBInit(void)
{
    if (!ConnectToDb())
    {
	return SLRI_FAIL;
    }

    /* ODBC to initialize STMT Handler. */

    if (!InitNBlastDB())
    {
	return SLRI_FAIL;
    }
    /* ODBC where to initialize this one ??? */
    if (!InitBlastDB())
    {
	return SLRI_FAIL;
    }
    return SLRI_SUCCESS;
}



/* Obs.: In ODBC a Database is connected or disconected we don't have the
 *       concept of Open Table.
 *       Do you need to check if the Database is Connected and the Table exist ???
 */
Boolean NBlast_IsDBOpen(Int2 iDB)
{
    	if(iDB == BLASTDB)
	{
/*	if(pd4BlastDB) return TRUE; */
	    return TRUE;
	}
	else if (iDB == BLASTDBt)
	{
/*	if(pd4BlastDBt) return TRUE; */
	    return TRUE;
	}
	else if (iDB == NBLASTDB)
	{
/*	if(pd4NBlastDB) return TRUE; */
	    return TRUE;
	}
	return FALSE;
}



/* Generic ODBC Connect to DB functions.
 *(Codebase) initializer and DB opener function
 * This function need to be removed.
 * or change it to call InitBlastDB() and InitNBlastDB()
 */
SLRI_ERR NBlastOpenDBs(CharPtr dbname, Boolean BlastDB, Boolean NBlastDB)
{
/*
    if(!InitBlastDB())
    {
	return SLRI_FAIL;
    }

    if(NBlastDBInit()) return SLRI_FAIL;
    if(BlastDB) if(OpenBlastDB(dbname, -1, FALSE, FALSE, FALSE)) return SLRI_FAIL;
    if(NBlastDB) if(OpenNBlastDB(dbname, FALSE, TRUE)) return SLRI_FAIL;
*/
    return SLRI_SUCCESS;
}



/* Open the NBlastDB using accessmode options   */
/* What it's accessmode options???              */
/* This function will be replased by InitNBLAST */

SLRI_ERR OpenNBlastDB(CharPtr pcNBlastDB, Boolean bCreate, Boolean bDENY_NONE)
{
/*
	if(bDENY_NONE == TRUE)
	{
*/
        /* cb.accessMode = OPEN4DENY_NONE; */
/*
	} else
	{
*/
    /* cb.accessMode = OPEN4DENY_RW;  */
/*
	}
	if(OpenNBlast_NBlastDB(pcNBlastDB, bCreate)) {
		ErrPostEx(SEV_FATAL,0,0,"OpenNBlastDB: Unable to Open NBlastDB.");
		return SLRI_FAIL;
	}
*/
	return SLRI_SUCCESS;
}



/* Open the BlastDB using accessmode options
 * Obs.: What means accessmode options ???
 *       What are all the parameters under OpenNBlast_BlastDB ???
 *       This function will be replased by InitBLAST
 */
SLRI_ERR OpenBlastDB(CharPtr pcBlastDB, Int4 imachine_num, Boolean bCreate, Boolean bTemp, Boolean bDENY_NONE)
{
/*
	if(bDENY_NONE == TRUE) {
*/
/*		cb.accessMode = OPEN4DENY_NONE;  */
/*
	} else {
*/
/*		cb.accessMode = OPEN4DENY_RW;   */
/*
	}
	if(OpenNBlast_BlastDB(pcBlastDB,  imachine_num,  bCreate,  bTemp)) {
		ErrPostEx(SEV_FATAL,0,0,"OpenBlastDB: Unable to Open BlastDB.");
		return SLRI_FAIL;
	}
*/
	return SLRI_SUCCESS;
}


/*******************************************************************
* NBlastClose                                                      *
* Close the databases and the codebase data structure              *
*******************************************************************/
/*
 * Obs.: All Close functions need to return True or False.!!!
 *       Implement FlushNBlast
 */
void NBlastClose(void)
{
    NBlastFlushDBs();
    NBlastDBClose();

}


/* Open the Blast Database */
/*
 * Obs.: What measn OpenNBlast_BlastDB ???
 *       I think this function need to be removed.
 */
SLRI_ERR OpenNBlast_BlastDB(CharPtr pcBlastDB, Int4 imachine_num, Boolean bCreate, Boolean bTemp)
{
/*
	CharPtr pcBlastDBname = NULL;
	Char local_writepath[PATH_MAX];

	if(GetAppParam("nblast","NBLAST","writepath","",local_writepath,PATH_MAX)) {
		if((pcBlastDBname = MemNew(StrLen(pcBlastDB)+StrLen(local_writepath)+12)) == NULL) {
			ErrPostEx(SEV_FATAL,0,0,"Could not allocate memory for NBlastDBOpen CharPtr");
			return SLRI_FAIL;
		}
#ifdef MOBIDICK_API
		sprintf(pcBlastDBname,"%s%sB%ld.dbf", CFG_local_writepath, pcBlastDB, (long) imachine_num);
#else
		if(imachine_num == -1) {
	                sprintf(pcBlastDBname,"%s%sB.dbf", local_writepath, pcBlastDB);
		} else {
			sprintf(pcBlastDBname,"%s%sB%ld.dbf", local_writepath, pcBlastDB, (long) imachine_num);
		}
#endif
	} else {
*/
    /* Not valid Config file or file not in specified directory */
/*
		ErrPostEx(SEV_FATAL,0,0,"Could not find BlastDB file.\nPlease check that your configuration file is valid\nor that the database file you are trying to open exists\nin this directory.\n");
		return SLRI_FAIL;
	}
*/
	if(bCreate == TRUE)
	{
         /*
		pd4BlastDB = d4create(&cb,pcBlastDBname,f4iBlastDB,t4iBlastDB);
		if (pd4BlastDB == NULL)
		{
			ErrPostEx(SEV_FATAL,0,0,"Could not create data file %s\n",pcBlastDBname);
			return SLRI_FAIL;
		}
         */
	}
	else if (bTemp == FALSE)
	{
	 /*
		pd4BlastDB=d4open(&cb,pcBlastDBname);
		if (pd4BlastDB == NULL)
		{
			ErrPostEx(SEV_WARNING,0,0,"Could not open data file %s\n",pcBlastDBname);
			return SLRI_FAIL;
		}
	 */
	}
	else
	{
	 /*
		pd4BlastDBt = NULL;
		pd4BlastDBt=d4open(&cb,pcBlastDBname);
		if (pd4BlastDBt == NULL)
		{
			ErrPostEx(SEV_ERROR,0,0,"Could not open data file %s\n",pcBlastDBname);
			return SLRI_FAIL;
		}
	 */
	}
/*	pcBlastDBname = MemFree(pcBlastDBname); */

	/* Assign global field/tag pointers to database fields/tags */
/*
	if(bTemp == FALSE) {
		pf4uid=d4field(pd4BlastDB,"UID");
		pf4eval=d4field(pd4BlastDB,"EVAL");
		pf4asnsa=d4field(pd4BlastDB,"ASNSA");
*/
	/* Assign and select the uid as the default tag */
/*
		pt4uid=d4tag(pd4BlastDB,"UID_TAG");
		d4tagSelect(pd4BlastDB,pt4uid);
	} else {
		pf4uidt=NULL;
		pf4evalt=NULL;
		pf4asnsat=NULL;
		pf4uidt=d4field(pd4BlastDBt,"UID");
		pf4evalt=d4field(pd4BlastDBt,"EVAL");
		pf4asnsat=d4field(pd4BlastDBt,"ASNSA");
	}
*/
	return SLRI_SUCCESS;
}



/* Open the NBlast Database
 * ???
 */

SLRI_ERR OpenNBlast_NBlastDB(CharPtr pcNBlastDB, Boolean bCreate)
{
 /*	CharPtr pcNBlastDBname = NULL;
	Char local_writepath[PATH_MAX];

	if(GetAppParam("nblast","NBLAST","writepath","",local_writepath,PATH_MAX))
	{
		if((pcNBlastDBname = MemNew(StrLen(pcNBlastDB)+StrLen(local_writepath)+12)) == NULL)
		{
			ErrPostEx(SEV_FATAL,0,0,"Could not allocate memory for NBlastDBOpen CharPtr");
			return SLRI_FAIL;
		}
#ifdef MOBIDICK_API
		sprintf(pcNBlastDBname,"%s%sN.dbf", CFG_local_writepath, pcNBlastDB);
#else
		sprintf(pcNBlastDBname,"%s%sN.dbf", local_writepath, pcNBlastDB);
#endif
        }
	else
	{
 */
        /*  Not valid Config file or file not in specified directory  */
/*
		ErrPostEx(SEV_FATAL,0,0,"Could not find NBlastDB file.\nPlease check that your configuration file is valid\nor that the database file you are trying to open exists\nin this directory.\n");
		return SLRI_FAIL;
	}
 */

/*        cb.errorCode = 0; */

    if(bCreate == TRUE)
    {
     /*
	pd4NBlastDB = d4create(&cb,pcNBlastDBname,f4iNBlastDB,t4iNBlastDB);
	if (pd4NBlastDB == NULL)
	{
	    ErrPostEx(SEV_FATAL,0,0,"NBlastDBOpen: Could not create data file %s",pcNBlastDBname);
	    pcNBlastDBname = MemFree(pcNBlastDBname);
	    return SLRI_FAIL;
	}
     */
    }
    else
    {
     /*
	pd4NBlastDB=d4open(&cb,pcNBlastDBname);
	if (pd4NBlastDB == NULL)
	{
	    ErrPostEx(SEV_FATAL,0,0,"NBlastDBOpen: Could not open/create data file %s",pcNBlastDBname);
	    pcNBlastDBname = MemFree(pcNBlastDBname);
	    return SLRI_FAIL;
	}
     */
    }

/*	pcNBlastDBname = MemFree(pcNBlastDBname); */

/*
	pf4ord=d4field(pd4NBlastDB,"ORD");
	pf4gi=d4field(pd4NBlastDB,"GI");
	pf4num=d4field(pd4NBlastDB,"NUM");
	pf4asnnbr=d4field(pd4NBlastDB, "ASNNBR");

	pt4ord=d4tag(pd4NBlastDB,"ORD_TAG");
	pt4gi=d4tag(pd4NBlastDB,"GI_TAG");
	pt4num=d4tag(pd4NBlastDB,"NUM_TAG");

	d4tagSelect(pd4NBlastDB,pt4ord);
*/
	return SLRI_SUCCESS;
}




/* Close the codebase data structure */
/*
 * Obs.: Can be Removed ???
 *       All Close functions need to return True or False.!!!
 */
void NBlastDBClose(void)
{
    /*	code4close(&cb); */
    /*	code4initUndo(&cb); */
    /*    if (  */
    CloseDB();  /* To be implemented ??? */
    /*       )  */
    {

/*	return SLRI_SUCCESS; */
    }
/*    return SLRI_FAIL; */
}


void NBlastFlushDBs(void)
{
	NBlastCloseDBs(TRUE,TRUE,TRUE);
}

Int2 FlushTable(CharPtr tblname)
{
    Char *pSQL_stmt;                      /* SQL select statement             */
    SQLHANDLE hstmt;                      /* handle to select statement       */
    SQLRETURN retcode;                    /* return values for odbc layer     */

    pSQL_stmt = (char*) malloc(sizeof(char) * 150);
    if(pSQL_stmt == NULL)
    {
	ErrPostEx(SEV_ERROR, 0, 0, "[FlushTable]: Could not allocate space");
	return SH_ERROR;
    }

    sprintf((char*)pSQL_stmt, "FLUSH TABLE %s.%s;", database, tblname);

    /* Create Handle Statement, once handle exists we can free pSQL_stmt */
    if(CreateHandle(&hstmt, pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
    {
	ErrPostEx(SEV_ERROR, 0, 0, "[FlushTable]: Could not create handle.");
	free(pSQL_stmt);
	return SH_ERROR;
    }
    free(pSQL_stmt);

    /* execute the select statement */
    if( (retcode = SQLExecute(hstmt)) != SQL_SUCCESS )
    {
	ErrPostEx(SEV_ERROR, 0, 0, "[FlushTable]: Could not execute FLUSH statement.");
	LogSQLInfo(hstmt);
	SQLFreeStmt(hstmt, SQL_DROP);
	return SH_ERROR;
    }

    if(SQLFreeStmt(hstmt, SQL_DROP) != SQL_SUCCESS)
    {
	ErrPostEx(SEV_ERROR, 0, 0, "[FlushTable]: Could not free statement.");
	return SH_ERROR;
    }

    return  SH_SUCCESS;
}



/* Close the NBLAST databases*/
/*
 * Obs.: Close DBs need to be changed by CloseDB() to disconect to the database,
 *       NOT to close a file.
 *       Here I'm calling NBlastClose
 *       Why there are 3 Three Tables ???
 */
void NBlastCloseDBs(Boolean bBlastDB, Boolean bBlastDBt, Boolean bNBlastDB)
{
    if(bBlastDB)
	/* if(pd4BlastDB)
	   {
	   d4close(pd4BlastDB);
	   pd4BlastDB = NULL;
	   }
	*/
	if(bBlastDBt)
	    /*	if(pd4BlastDBt)
		{
		d4close(pd4BlastDBt);
		pd4BlastDBt = NULL;
		}
	    */
	    if(bNBlastDB)
	    {
		/*	if(pd4NBlastDB)       */
		/*	{                     */
		/*    	d4close(pd4NBlastDB); */
		/*	pd4NBlastDB = NULL;   */
		/*	}                     */
	    }


/*	    NBlastClose();  */
}


/*Appends Record to BlastDB */
/*
 * Obs.: r4unique Where this variable or Tag is defined ???
 *       Function name will be change to AppendRecordBLAST
 */
SLRI_ERR AppendRecordtoBlastDB(Int8 uid, FloatHi evalue, NBlastResultSetPtr pResult, Boolean bUnique)
{
    SQLRETURN retcode;
    pABL pABLtmp = NULL;

    if( uid < 0)
    {
	ErrPostEx(SEV_ERROR,0,0, "AppendRecordtoBlastDB: Invalid Parameters!");
	return SLRI_FAIL;
    }

    blastdb_uid.val = uid;
    blastdb_eval.val = evalue;


    Int2 pf4asnsa; /* For test only */
	if(pResult)
	{

	     /* get the asn1 bioseq into a bytestore pointer so that we can put it into a buffer. */
	    pABLtmp = (pABL)AssignASNMemChar((Pointer) pResult, (AsnWriteFunc) NBlastResultSetAsnWrite);

	    if(!pABLtmp)
	    {
		ErrPostEx(SEV_ERROR,0,0, "[AppendRecordtoBlastDB]: Did not assign asn to memo field!");
		return SLRI_FAIL;
	    }

	    blastdb_asnsa.val = pABLtmp->buf;
	    blastdb_asnsa.len = pABLtmp->len;

  	    /* Bind the  3rd input parameters            */
	    /* Parameter 1, 2 were Bind in InitBlastDB   */
 	    retcode = SQLBindParameter(HNDLBlastdb, 3, SQL_PARAM_INPUT, SQL_C_DEFAULT,  SQL_LONGVARBINARY, 0, 0,
				       pABLtmp->buf, 0, (long*)&pABLtmp->len);
	    if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
	    {
		ErrPostEx(SEV_ERROR, 0, 0, "[AppendRecordtoBlastDB]: Could not bind parameter 2.");
		return  SLRI_FAIL;
	    }
   	}

	/* execute statment */
	retcode = SQLExecute(HNDLBlastdb);
	if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
	{
	    LogSQLInfo(HNDLBlastdb);
	    ErrPostEx(SEV_ERROR,0,0, "[AppendRecordtoBlastDB]: Unable to execute BlastDB insert statement.");
	    return  SLRI_FAIL;
	}

	return SLRI_SUCCESS;
}

/*Appends Record to NBlastDB */
/* Obs.: Function name will be change to AppendRecordtoNBLAST()
 */
SLRI_ERR AppendRecordtoNBlastDB(Int4 ord, Int4 gi, Int4 num, NBlastGiAndEvalSetPtr pNeighbours, Boolean bUnique)
{
    SQLRETURN retcode;
    pABL pABLtmp = NULL;

    if( (ord < 0) || (gi < 0) )
    {
	ErrPostEx(SEV_ERROR,0,0, "[AppendRecordtoNBlastDB]: Invalid Parameters!");
	return SLRI_FAIL; /* SH_ERROR; */
    }

    nblastdb_ord.val = ord;
    nblastdb_gi.val  = gi;
    nblastdb_num.val = num;

	if(pNeighbours)
	{
	    pABLtmp = (pABL)AssignASNMemChar((Pointer) pNeighbours, (AsnWriteFunc) NBlastGiAndEvalSetAsnWrite);
	    if(!pABLtmp)
	    {
		ErrPostEx(SEV_ERROR,0,0, "[AppendRecordtoNBlastDB]: Did not assign asn to memo field!");
		return SLRI_FAIL; /* SH_ERROR; */
	    }

	    nblastdb_asnbr.val = pABLtmp->buf;
	    nblastdb_asnbr.len = pABLtmp->len;

 	    /* Bind 4th input parameters            */
	    /* Parameters 1, 2, 3  were Bind in InitNBlastDB   */
 	    retcode = SQLBindParameter(HNDLNBlastdb, 4, SQL_PARAM_INPUT, SQL_C_DEFAULT,  SQL_LONGVARBINARY, 0, 0,
				       pABLtmp->buf, 0, (long*)&pABLtmp->len);
	    if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
	    {
		ErrPostEx(SEV_ERROR, 0, 0, "[AppendRecordtoNBlastDB]: Could not bind parameter 2.");
		return  SLRI_FAIL; /* SH_ERROR; */ 
	    }
	}

        /* execute statment */
	retcode = SQLExecute(HNDLNBlastdb);
	if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
	{
	    LogSQLInfo(HNDLBlastdb);
	    ErrPostEx(SEV_ERROR,0,0, "[AppendRecordtoNBlastDB]: Unable to execute NBlastDB insert statement.");
	    return SLRI_FAIL; /* SH_ERROR; */
	}

	return SLRI_SUCCESS;
}


/*
 * Obs.: What it's NBlastGetRecordFromNBlastDB() and AppendRecordtoNBlastDB() and
 *       why they return SLRI_FAIL.
 *       Function name will be change to AddRecordToNBLAST.
 */
SLRI_ERR AddRecordtoNBlastDB(Int4 gi, Int4 num, NBlastGiAndEvalSetPtr pNeighbours)
{
	Int4 irec = 0;
	Int4 ord  = 0;

	NBlastRecCount(NBLASTDB, &irec);
	if(NBlastGetRecordFromNBlastDB(irec,&ord,NULL,NULL,NULL))
	    return SLRI_FAIL;
	if(AppendRecordtoNBlastDB(++ord,gi,num,pNeighbours, TRUE))
	    return SLRI_FAIL;

	return SLRI_SUCCESS;
}

/*
 * Obs.: This function needs to be call DeleteUIDinNBlastDB or DeleteRecordInBLASTbyUid()
 */
SLRI_ERR NBlastDeleteBlastDBRecord(Int8 uid)
{
    CharPtr nblastdb_delete = (char *) malloc(sizeof(Char) * 150);
    if (nblastdb_delete == NULL)
    {
	ErrPostEx(SEV_ERROR, 0, 0, "[NBlastDeleteBlastDBRecord]: Could not allocate space");
	free(nblastdb_delete);
	return SLRI_FAIL; /* SH_ERROR; */
    }
    sprintf((char *)nblastdb_delete, "delete from %s.blastdb where uid=?", database);

    SQLHANDLE hstmt;
    Int2 sql_return = SQL_SUCCESS;

    CreateHandle(&hstmt, nblastdb_delete, hdbc, &henv );
    free(nblastdb_delete);

    SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_SLONG,SQL_INTEGER, 0, 0, &uid, 0, NULL);

    /* set auto commit on */
    SQLSetConnectAttr( hdbc, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_OFF, SQL_NTS);

    sql_return = SQLExecute(hstmt);

    /* execute statment */
    if (sql_return != SQL_SUCCESS)
    {
	ErrPostEx(SEV_ERROR,0,0, "[NBlastDeleteBlastDBRecord]:unable to execute delete statement.");
	LogSQLInfo(hstmt);
	SQLFreeStmt(hstmt, SQL_DROP);
	return SLRI_FAIL; /* SH_ERROR; */
    }

    if(SQLFreeStmt(hstmt, SQL_DROP) != SQL_SUCCESS)
    {
	ErrPostEx(SEV_ERROR, 0, 0, "[NBlastDeleteBlastDBRecord]: Could not free statement.");
	return SLRI_FAIL; /* SH_ERROR; */
    }

    return SLRI_SUCCESS; /*  SH_SUCCESS; */
}



/* Obs.: This function name will change to SearchGIsInNBLASTandGetUid or SearchUidInNBLASTbyGi1Gi2
 */
Int4 LIBCALL NBLASTFindGIsAndGetUID(Int4 gi1, Int4 gi2, Int8 PNTR pUID)
{
    Int4 ord1 = 0;
    Int4 ord2 = 0;
    Int8 uid = 0;

    if ( SearchORDbyGIinNBLASTDB(gi1, &ord1) != SLRI_SUCCESS ) /* SH_SUCCESS ) */
    {
	ErrPostEx(SEV_WARNING,0,0, "[NBLASTFindGIsAndGetUID]: Can't find gi1 %ld in NBlastDB.",gi1);
	return SLRI_FAIL; /* SH_NODATA; */
    }

    if ( SearchORDbyGIinNBLASTDB(gi2, &ord2) != SLRI_SUCCESS ) /* SH_SUCCESS ) */
    {
	ErrPostEx(SEV_WARNING,0,0, "[NBLASTFindGIsAndGetUID]: Can't find gi2 %ld in NBlastDB.",gi2);
	return SLRI_FAIL; /* SH_ERROR; */
    }


    if(ord1 < ord2)
    {
	ORDtoUID(ord1, ord2, &uid);
    }
    else
    {
	ORDtoUID(ord2, ord1, &uid);
    }
    printf("uid: %ld\n", uid);

    *pUID = uid;
    return  SLRI_SUCCESS; /* SH_SUCCESS; */
}



/*
 * This needs to be call DeleteRecordInBLASTbyGi1Gi2()
 */
SLRI_ERR NBlastRemoveBlastDBRecord(Int4 gi1, Int4 gi2)
{
        Int8 uid = 0;
	printf("Start NBlastRemoveBlastDBRecord \n");
	if(NBLASTFindGIsAndGetUID(gi1, gi2, &uid) == SLRI_FAIL)
	{
	    return SLRI_FAIL;
	}
	printf("uid: %d\n", uid);
	NBlastDeleteBlastDBRecord(uid);
	return SLRI_SUCCESS;
}


/*
 * This needs to be call DeleteRecordInNBLASTbyGi()
 */
SLRI_ERR NBlastRemoveNBlastDBRecord(Int4 gi)
{
    CharPtr nblastdb_delete = (char *) malloc(sizeof(Char) * 150);
    if (nblastdb_delete == NULL)
    {
	ErrPostEx(SEV_ERROR, 0, 0, "[NBlastRemoveNBlastDBRecord]: Could not allocate space");
	free(nblastdb_delete);
	return SLRI_FAIL; /* SH_ERROR; */
    }
    sprintf((char *)nblastdb_delete, "delete from %s.nblastdb where gi=?", database);

    SQLHANDLE hstmt;
    Int2 sql_return = SQL_SUCCESS;

    CreateHandle(&hstmt, nblastdb_delete, hdbc, &henv );   /* !!! The handler needs to be crated in the Init. ???  */
    free(nblastdb_delete);

    SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_SLONG,SQL_INTEGER, 0, 0, &gi, 0, NULL);

    /* set auto commit on */
    SQLSetConnectAttr( hdbc, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_OFF, SQL_NTS);

    sql_return = SQLExecute(hstmt);

    /* execute statment */
    if (sql_return != SQL_SUCCESS)
    {
	ErrPostEx(SEV_ERROR,0,0, "[NBlastRemoveNBlastDBRecord]:unable to execute delete statement.");
	LogSQLInfo(hstmt);
	SQLFreeStmt(hstmt, SQL_DROP);
	return SLRI_FAIL; /* SH_ERROR; */
    }

    SQLFreeStmt(hstmt, SQL_DROP);

    return SLRI_SUCCESS; /* SH_SUCCESS; */
}

/*
 * Obs.: This function name will be change to ClearNBLASTbyGi()
 */

SLRI_ERR NBlastClearNBlastDBRecord(Int4 gi)
{
	if(NBlastModifyNBlastDBRecord((Int4) CLEAR_NBLASTDB_RECORD, 0, gi, 0, NULL))
	    return SLRI_FAIL;

	return SLRI_SUCCESS;
}

/*Counts the number of neighbours from the asn structure*/
/*And adds the number to the database field*/
/* Obs.: This finction needs to have a different name.
 */

SLRI_ERR AddNumNeighboursToDB(Int4 PNTR piTotalNeighbours)
{
        Char *pSQL_stmt;                         /* SQL select statement         */
	SQLINTEGER fetchCount = 0;               /* fetch results Counter        */
	SQLHANDLE hstmt;                         /* handle to select statement   */
	SQLRETURN retcode;                       /* return values for odbc layer */
	SQLINTEGER blobloc = 0;                  /* to grab the size of the blob */
	CharPtr pcASN = NULL;
	Int4 bloblen = 0, indicator = 0, asnbrvalue = 0, gi = 0;

	NBlastGiAndEvalSetPtr nbgespHere = NULL;
	NBlastGiAndEvalPtr nbgep = NULL;
	Int4 rc = 0, numnodes = 0, i = 0;
	MonitorPtr pmonitor = NULL;

	pSQL_stmt = (char*) malloc(sizeof(char) * 200);
	if(pSQL_stmt == NULL)
	{
	    ErrPostEx(SEV_ERROR, 0, 0, "[AddNumNeighboursToDB]: Could not allocate space");
	    return SH_ERROR;
	}

	sprintf((char*)pSQL_stmt, "select gi, asnbr from %s.nblastdb;", database);
        /* Create Handle Statement, once handle exists we can free char* */
	retcode = CreateHandle(&hstmt, pSQL_stmt, hdbc, &henv);
	if(retcode != SQL_SUCCESS)
	{
	    ErrPostEx(SEV_ERROR, 0, 0, "[AddNumNeighboursToDB]: Could not create handle.");
	    free(pSQL_stmt);
	    return SH_ERROR;
	}
	free(pSQL_stmt);

	/* execute the select statement */
	if( (retcode = SQLExecute(hstmt)) != SQL_SUCCESS )
	{
	    ErrPostEx(SEV_ERROR, 0, 0, "[AddNumNeighboursToDB]: Could not execute select statement.");
	    LogSQLInfo(hstmt);
	    SQLFreeStmt(hstmt, SQL_DROP);
	    return SH_ERROR;
	}

	if( (retcode = SQLBindCol(hstmt, 1, SQL_C_SLONG, &nblastdb_gi.val, sizeof(nblastdb_gi.val), NULL)) != SQL_SUCCESS && (retcode != SQL_SUCCESS_WITH_INFO) )
	{
	    ErrPostEx(SEV_ERROR, 0, 0, "[AddNumNeighboursToDB] Could not bind column 1. Could not execute select statement.");
	    SQLFreeStmt(hstmt, SQL_DROP);
	    return SH_ERROR;
	}
	if( (( retcode = SQLBindCol(hstmt, 2, SQL_C_BINARY, &blobloc, 0, (long*)&asnbrvalue) ) != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO) )
	{
	    ErrPostEx(SEV_ERROR, 0, 0, "[NBlastGetRecordFromNBlastDB]: Could not bind column 2 asnbr.");
	    SQLFreeStmt(hstmt, SQL_DROP);
	    return SLRI_FAIL; /* SH_ERROR; */
	}

	while( (retcode = SQLFetch(hstmt) ==  SQL_SUCCESS)  || (retcode == SQL_SUCCESS_WITH_INFO) )
	{
	    fetchCount++;

	    printf("gi:  %d \n", nblastdb_gi.val);
	    gi = nblastdb_gi.val;
	    bloblen = (Int4) asnbrvalue;
	    if (bloblen > 0)
	    {    printf("bloblen: %d\n", bloblen);
		/* allocate a buffer big enough to store the blob. */
		pcASN = (CharPtr) MemNew((size_t) bloblen);
                /*read the blob from the database */
		SQLGetData(hstmt, 2, SQL_C_BINARY, (SQLPOINTER)pcASN, bloblen, (long*)&indicator);
		if (pcASN != NULL)
		{
		    nbgespHere =  (NBlastGiAndEvalSetPtr) GetASNMemChar(pcASN,(AsnReadFunc)NBlastGiAndEvalSetAsnRead, bloblen);
		    if (nbgespHere != NULL)
		    {
			numnodes = 0;
			nbgep = nbgespHere->giandevals;
			printf("nbgespHere->query_gi: %d\n", nbgespHere->query_gi);
			if (nbgep != NULL)
			{
			    if(Misc_GenericGetNumberofNodes(nbgep, &numnodes))
			    {
				ErrPostEx(SEV_INFO, 0, 0, "[AddNumNeighboursToDB]: Fail in Misc_GenericGetNumberofNodes for gi: %dl.", gi);
				return SLRI_FAIL;
			    }
			}
			else
			{
			    numnodes = 0;
			}
			printf("numnodes: %d\n", numnodes);
			if ( numnodes > -1)
			{
			    retcode = NBlastModifyNBlastDBRecord(-1, 0, gi, numnodes, NULL);
			    if ( retcode ==  SLRI_FAIL)
			    {
				ErrPostEx(SEV_INFO, 0, 0, "[AddNumNeighboursToDB]: Fail in NBlastModifyNBlastDBRecord when try to update num..");
				return SLRI_FAIL;
			    }
			}
			nbgespHere = NBlastGiAndEvalSetFree(nbgespHere);
		    }
		}
		if (pcASN) {MemFree(pcASN);}
	    }
	    else
	    {
		ErrPostEx(SEV_INFO, 0, 0, "[AddNumNeighboursToDB]: ASN1 Blob retrieved has length = 0.");
	    }

	}
/* 	if((pmonitor = MonitorIntNew("Adding Neighbours to Database Field", 1, d4recCount(pd4NBlastDB))) == NULL) */
	if((pmonitor = MonitorIntNew("Adding Neighbours to Database Field", 1, CountRecordsFromTable("nblastdb"))) == NULL)
	{
		ErrPostEx(SEV_INFO,0,0, "AddNumNeighboursToDB: Could not create monitor");
		return SLRI_FAIL;
	}
/* 	cb.errDefaultUnique = r4uniqueContinue; */







/*  	for(rc = d4top(pd4NBlastDB); rc == r4success; rc = d4skip(pd4NBlastDB, 1L))
	{
		nbgespHere = (NBlastGiAndEvalSetPtr) GetASNMemMemo(pf4asnnbr, (AsnReadFunc) NBlastGiAndEvalSetAsnRead);
		if(nbgespHere != NULL)
		{
			numnodes = 0;
			nbgep = nbgespHere->giandevals;
			if(Misc_GenericGetNumberofNodes(nbgep,&numnodes))
			{
				return 1;
			}
			nbgespHere = NBlastGiAndEvalSetFree(nbgespHere);
			f4assignLong(pf4num,(long)numnodes);
			*piTotalNeighbours += numnodes;
		}
		else
		{
		    f4assignLong(pf4num,0L);
		}
		MonitorIntValue(pmonitor, i++);
	}
	pmonitor = MonitorFree(pmonitor);

	cb.errDefaultUnique = r4unique;
*/
	return SLRI_SUCCESS;
}



/*
 * Obs.: This function name will be change to UpdateRecordNBLAST()
 */
#define nblast_buf_len 2
SLRI_ERR NBlastModifyNBlastDBRecord(Int4 RecordNum, Int4 ord, Int4 gi, Int4 num, NBlastGiAndEvalSetPtr pNeighbours)
{
    printf("NBlastModifyNBlastDBRecord Start \n");
    SQLRETURN retcode;
    pABL pABLtmp = NULL;
    Int4 auxOrd = -1;
    Int2 countParam = 1;

    CharPtr strord = "";                /* to create "ord=?";    */
    CharPtr strgi = "";                 /* to create "gi=?";     */
    CharPtr strnum = "";                /* to create "num=?";    */
    CharPtr strasnbr = "";              /* to create "asnbr=?";  */

    printf("nblastdb_num.val %d\n", nblastdb_num.val );

    CharPtr pSQL_stmt = (Char *) malloc (sizeof(Char) * 150);
    if(pSQL_stmt == NULL)
    {
	ErrPostEx(SEV_ERROR, 0, 0, "[NBlastModifyNBlastDBRecord]: Could not allocate space.");
	return SLRI_FAIL;
    }

    /* GI Find */
    /* Construct SQL field statements dinamicaly*/

    if (num > 0 || gi > 0)
    {
	printf("num > 0\n");
	if (RecordNum < 1)
	{
	    if (ord > 0)
		strnum = ", num=?";
	    else
		strnum = ", num=?";
	}
	else
	    strnum = "num=?";
	nblastdb_num.val = num;
    }
    if (pNeighbours)
    {
	if ( (ord > 0) || (num > 0) )
	    strasnbr = ", asnbr=?";
	else
	    strasnbr = "asnbr=?";

	pABLtmp = (pABL)AssignASNMemChar((Pointer) pNeighbours, (AsnWriteFunc) NBlastGiAndEvalSetAsnWrite);
	if(!pABLtmp)
	{
	    ErrPostEx(SEV_ERROR,0,0, "[NBlastModifyNBlastDBRecord]: Did not assign asn to memo field!");
	    return SLRI_FAIL;
	}

	nblastdb_asnbr.val = pABLtmp->buf;
	nblastdb_asnbr.len = pABLtmp->len;
    }

    if(RecordNum == -1 && gi > 0)
    {
	strord = "gi=?";
	nblastdb_gi.val = gi;

	if (ord > 0)
	{
	    strord = "ord=?";
	    nblastdb_ord.val = ord;
	}
	/* create handle */
	sprintf(pSQL_stmt, "update %s.nblastdb set %s %s %s where gi=?", database, strord, strnum, strasnbr);
	printf("pSQL_stmt: %s\n", pSQL_stmt);
	if(CreateHandle(&HNDLNBlastdbUpdate, pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
	{
	    ErrPostEx(SEV_ERROR, 0, 0, "[NBlastModifyNBlastDBRecord]: Could not create handle to insert statement.");
	    return SLRI_FAIL; /* FALSE; */
	}

	/* bind parameters */

	if (gi > 1)
	{
	    printf("Binding gi\n");
	    retcode = SQLBindParameter(HNDLNBlastdbUpdate, countParam, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &nblastdb_gi.val, 0, NULL);
	    if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
	    {
		ErrPostEx(SEV_ERROR, 0, 0, "[NBlastModifyNBlastDBRecord]: Could not bind gi parameter.");
		return SLRI_FAIL; /* FALSE; */
	    }
	    countParam++;
	}
	if (ord > 1)
	{
	    retcode = SQLBindParameter(HNDLNBlastdbUpdate, countParam, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &nblastdb_ord.val, 0, NULL);
	    if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
	    {
		ErrPostEx(SEV_ERROR, 0, 0, "[NBlastModifyNBlastDBRecord]: Could not bind ord parameter.");
		return SLRI_FAIL; /* FALSE; */
	    }
	    countParam++;
	}
	if (num > -1)
	{
	    printf("Binding num\n");
	    retcode = SQLBindParameter(HNDLNBlastdbUpdate, countParam, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &nblastdb_num.val, 0, NULL);
	    if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
	    {
		ErrPostEx(SEV_ERROR, 0, 0, "[NBlastModifyNBlastDBRecord]: Could not bind num parameter.");
		return SLRI_FAIL; /* FALSE; */
	    }
	    countParam++;
	}
	if (pNeighbours)
	{

 	    retcode = SQLBindParameter(HNDLNBlastdbUpdate, countParam, SQL_PARAM_INPUT, SQL_C_DEFAULT,  SQL_LONGVARBINARY, 0, 0,
				       pABLtmp->buf, 0, (long*)&pABLtmp->len);
	    if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
	    {
		ErrPostEx(SEV_ERROR, 0, 0, "[NBlastModifyNBlastDBRecord]: Could not bind asnbr parameter.");
		return SLRI_FAIL; /*  FALSE; */
	    }
	    countParam++;
	}
	if ( ord > 0)
	{
	    retcode = SQLBindParameter(HNDLNBlastdbUpdate, countParam, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &nblastdb_gi.val, 0, NULL);
	    if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
	    {
		ErrPostEx(SEV_ERROR, 0, 0, "[NBlastModifyNBlastDBRecord]: Could not bind asnbr parameter.");
		return SLRI_FAIL; /* FALSE; */
	    }
	}
	if (gi > 1)
	{
	    printf("Binding gi\n");
	    retcode = SQLBindParameter(HNDLNBlastdbUpdate, countParam, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &nblastdb_gi.val, 0, NULL);
	    if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
	    {
		ErrPostEx(SEV_ERROR, 0, 0, "[NBlastModifyNBlastDBRecord]: Could not bind gi parameter.");
		return SLRI_FAIL; /* FALSE; */
	    }
	    countParam++;
	}
    /* Ord Find */
    }
    else if (RecordNum == 0 && ord > 0)
    {
	strgi = "ord=?";
	nblastdb_ord.val = ord;

	if (gi > 0)
	{
	    strgi = "gi=?";
	    nblastdb_gi.val = gi;
	}
	/* create handle */
	sprintf(pSQL_stmt, "update %s.nblastdb set %s %s %s where %s", database, strgi, strnum, strasnbr, strord);
	printf("pSQL_stmt: %s\n", pSQL_stmt);
	if(CreateHandle(&HNDLNBlastdbUpdate, pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
	{
	    ErrPostEx(SEV_ERROR, 0, 0, "[NBlastModifyNBlastDBRecord]: Could not create handle to insert statement.");
	    return SLRI_FAIL; /* FALSE; */
	}

	/* bind parameters */
	if (gi > 1)
	{
	    retcode = SQLBindParameter(HNDLNBlastdbUpdate, countParam, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &nblastdb_gi.val, 0, NULL);
	    if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
	    {
		ErrPostEx(SEV_ERROR, 0, 0, "[NBlastModifyNBlastDBRecord]: Could not bind ord parameter.");
		return SLRI_FAIL; /* FALSE; */
	    }
	    countParam++;
	}
	if (num > 1)
	{
	    retcode = SQLBindParameter(HNDLNBlastdbUpdate, countParam, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &nblastdb_num.val, 0, NULL);
	    if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
	    {
		ErrPostEx(SEV_ERROR, 0, 0, "[NBlastModifyNBlastDBRecord]: Could not bind num parameter.");
		return SLRI_FAIL; /* FALSE; */
	    }
	    countParam++;
	}
	if (pNeighbours)
	{
 	    retcode = SQLBindParameter(HNDLNBlastdbUpdate, countParam, SQL_PARAM_INPUT, SQL_C_DEFAULT,  SQL_LONGVARBINARY, 0, 0,
				       pABLtmp->buf, 0, (long*)&pABLtmp->len);
	    if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
	    {
		ErrPostEx(SEV_ERROR, 0, 0, "[NBlastModifyNBlastDBRecord]: Could not bind asnbr parameter.");
		return SLRI_FAIL; /* FALSE; */
	    }
	    countParam++;
	}

	retcode = SQLBindParameter(HNDLNBlastdbUpdate, countParam, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &nblastdb_ord.val, 0, NULL);
	if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
	{
	    ErrPostEx(SEV_ERROR, 0, 0, "[NBlastModifyNBlastDBRecord]: Could not bind ord parameter.");
	    return SLRI_FAIL; /*  FALSE; */
	}

	/* Record Number Find */
    }                                   /* replace d4recCount() !!! */
/*	else if (RecordNum > 0 && RecordNum <= d4recCount(pd4NBlastDB)) */
    else if (RecordNum > 0)
    {

	ErrPostEx(SEV_ERROR, 0, 0, "[NBlastModifyNBlastDBRecord]: RecordNum > 0 Not part of the logic.");
	return SLRI_FAIL; /* SH_ERROR; */
        /* NOT IMPLEMENTED seems that it's not needed. */
    }
    else if (RecordNum == CLEAR_NBLASTDB_RECORD && gi > 0)    /* clear record */
    {
	printf("CLEAR_NBLASTDB_RECORD: %d\n", CLEAR_NBLASTDB_RECORD);
	nblastdb_gi.val = gi;

	sprintf(pSQL_stmt, "update %s.nblastdb set num = 0, asnbr=NULL where gi=?", database);
	printf("pSQL_stmt: %s\n", pSQL_stmt);

	if(CreateHandle(&HNDLNBlastdbUpdate, pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
	{
	    ErrPostEx(SEV_ERROR, 0, 0, "[NBlastModifyNBlastDBRecord]: Could not create handle to insert statement.");
	    return SLRI_FAIL; /* SH_ERROR; */
	}

	retcode = SQLBindParameter(HNDLNBlastdbUpdate, countParam, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &nblastdb_gi.val, 0, NULL);
	if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
	{
	    ErrPostEx(SEV_ERROR, 0, 0, "[NBlastModifyNBlastDBRecord]: Could not bind ord parameter.");
	    return SLRI_FAIL; /* SH_ERROR; */
	}
    }

    /* execute statment */
    retcode = SQLExecute(HNDLNBlastdbUpdate);
    if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
    {
	LogSQLInfo(HNDLNBlastdbUpdate);
	ErrPostEx(SEV_ERROR,0,0, "[NBlastModifyNBlastDBRecord]: Unable to execute NBlastDB Update statement.");
	return SLRI_FAIL; /* FALSE; */
    }

    if(SQLFreeStmt(HNDLNBlastdbUpdate, SQL_DROP) != SQL_SUCCESS)
    {
	ErrPostEx(SEV_ERROR, 0, 0, "[NBlastModifyNBlastDBRecord]: Could not free statement.");
	return SLRI_FAIL; /* SH_ERROR; */
    }

    return SLRI_SUCCESS;
}





/*
 *  Is this counting the number of records in the Database ???
 *  What are pd4BlastDB, pd4BlastDBt, pd4NBlastDB ???
 *  Obs.: Function need to be changed to get the MAX of table.id
 *  Logic need to be changed to get the MAX of id into table, because
 *  it easy to process records based in 'id' than possition into the table.
 *  New function will be SearchTableByMaxId()
 */
void NBlastRecCount(Int2 iDB, Int4 PNTR irecCount)
{
	if(iDB == BLASTDB)
	{
/*	    if(pd4BlastDB)
              *irecCount = (Int4)d4recCount(pd4BlastDB);  */
	    *irecCount = (Int4) CountRecordsFromTable("blastdb");

	}
	else if (iDB == BLASTDBt)
	{
/*	    if(pd4BlastDBt)
              *irecCount = (Int4)d4recCount(pd4BlastDBt);  */

	    *irecCount = (Int4) CountRecordsFromTable("blastdbt");
	}
	else if (iDB == NBLASTDB)
	{
/*	    if(pd4NBlastDB)
              *irecCount = (Int4)d4recCount(pd4NBlastDB); */
	    *irecCount = (Int4) CountRecordsFromTable("nblastdb");
	}

}

Int4 LIBCALL CountRecordsFromTable(CharPtr tblname)
{

    Char *pSQL_stmt;                      /* SQL select statement             */
    SQLHANDLE hstmt;                      /* handle to select statement       */
    SQLRETURN retcode;                    /* return values for odbc layer     */
    SQLINTEGER numRec = 0;                /* Number of record count           */
    pSQL_stmt = (char*) malloc(sizeof(char) * 150);
    if(pSQL_stmt == NULL)
    {
	ErrPostEx(SEV_ERROR, 0, 0, "[CountRecordsFromTable]: Could not allocate space");
	return SH_NODATA;
    }

    sprintf((char*)pSQL_stmt, "select count(*) from %s.%s;", database, tblname);

    /* Create Handle Statement, once handle exists we can free char* */
    retcode = CreateHandle(&hstmt, pSQL_stmt, hdbc, &henv);
    if(retcode != SQL_SUCCESS)
    {
	ErrPostEx(SEV_ERROR, 0, 0, "[CountRecordsFromTable]: Could not create handle.");
	free(pSQL_stmt);
	return SH_NODATA;
    }
    free(pSQL_stmt);

    /* execute the select statement */
    if( (retcode = SQLExecute(hstmt)) != SQL_SUCCESS )
    {
	ErrPostEx(SEV_ERROR, 0, 0, "[CountRecordsFromTable]: Could not execute select statement.");
	LogSQLInfo(hstmt);
	SQLFreeStmt(hstmt, SQL_DROP);
	return SH_NODATA;
    }

    retcode = SQLBindCol(hstmt, 1, SQL_C_SLONG, &numRec, sizeof(numRec), NULL);
    if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
    {
	ErrPostEx(SEV_ERROR, 0, 0, "[Could not bind column 1 in SearchBLASTDB]: Could not execute select statement.");
	SQLFreeStmt(hstmt, SQL_DROP);
	return SH_NODATA;
    }

    retcode = SQLFetch(hstmt);

    if(SQLFreeStmt(hstmt, SQL_DROP) != SQL_SUCCESS)
    {
	ErrPostEx(SEV_ERROR, 0, 0, "[CountRecordsFromTable]: Could not free statement.");
	return SH_NODATA;
    }

    /* Retruning the number of records in the database. */
    if ( (retcode  ==  SQL_SUCCESS)  || (retcode == SQL_SUCCESS_WITH_INFO) )
    {
	return numRec;
    }

    return SH_NODATA;
}

/* NBlastGetRecordFromNBlastDB      Get a full record from the NBlastDB     *
 *                                    use -1 for RecordNum to get rec by GI *
 *                                    use 0 for RecordNum to get rec by ORD */

SLRI_ERR NBlastGetRecordFromNBlastDB(Int4 RecordNum, Int4 PNTR pord, Int4 PNTR pgi, Int4 PNTR pnum, NBlastGiAndEvalSetPtr PNTR pNeighbours)
{
    printf("NBlastGetRecordFromNBlastDB Start \n");
    SQLRETURN retcode;                    /* return values for odbc layer      */
    pABL pABLtmp = NULL;
    CharPtr pcASN = NULL;
    Int4 auxOrd = -1;
    Int4 bloblen = 0, indicator = 0, asnbrvalue = 0;
    SQLINTEGER fetchCount = 0;            /* fetch results Counter             */
    SQLINTEGER blobloc = 0;               /* to grab the size of the blob      */
    Char *pSQL_stmt;                      /* SQL select statement              */
    SQLHANDLE hstmt;                      /* handle to select statement        */

    CharPtr strfields = "";                /* to create "ord";    */

    pSQL_stmt = (char*) malloc(sizeof(char) * 200);
    if(pSQL_stmt == NULL)
    {
	ErrPostEx(SEV_ERROR, 0, 0, "[NBlastGetRecordFromNBlastDB]: Could not allocate space");
	return SLRI_FAIL; /* SH_ERROR; */
    }
    if (RecordNum > 0)
    {
	nblastdb_id.val = RecordNum;
	sprintf(pSQL_stmt, "select ord, num, asnbr, gi from %s.nblastdb where id=?", database);
    }
    else if ( *pgi > 0 )
    {
	nblastdb_gi.val = *pgi;
	sprintf(pSQL_stmt, "select ord, num, asnbr from %s.nblastdb where gi=?", database);
    }
    else if ( *pord > 0 )
    {
	nblastdb_ord.val = *pord;
	sprintf(pSQL_stmt, "select gi, num, asnbr from %s.nblastdb where ord=?", database);
    }

    printf("pSQL_stmt: %s\n", pSQL_stmt);
    if(CreateHandle(&hstmt, pSQL_stmt, hdbc, &henv) != SQL_SUCCESS)
    {
	ErrPostEx(SEV_ERROR, 0, 0, "[NBlastGetRecordFromNBlastDB]: Could not create handle for select statement.");
	return SLRI_FAIL; /* FALSE; */
    }

    /* GI Find */
    if(RecordNum == -1 && *pgi > 0)
    {
	retcode = SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &nblastdb_gi.val, 0, NULL);
	if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
	{
	    ErrPostEx(SEV_ERROR, 0, 0, "[NBlastGetRecordFromNBlastDB]: Could not bind gi parameter.");
	    return SLRI_FAIL; /* FALSE; */
	}
	/* Ord Find */
    } else if (RecordNum == 0 && *pord > 0)
    {
	retcode = SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &nblastdb_ord.val, 0, NULL);
	if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
	{
	    ErrPostEx(SEV_ERROR, 0, 0, "[NBlastGetRecordFromNBlastDB]: Could not bind ord parameter.");
	    return SLRI_FAIL; /* FALSE; */
	}
	/* Record Number Find */
    } else if (RecordNum > 0) /* && RecordNum <= d4recCount(pd4NBlastDB)) */
    {
	retcode = SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &nblastdb_id.val, 0, NULL);
	if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
	{
	    ErrPostEx(SEV_ERROR, 0, 0, "[NBlastGetRecordFromNBlastDB]: Could not bind id parameter.");
	    return SLRI_FAIL; /* FALSE; */
	}
	printf("[NBlastGetRecordFromNBlastDB] ERROR wrong combination of Arguments\n");
    }
    /* execute the select statement */
    if( (retcode = SQLExecute(hstmt)) != SQL_SUCCESS )
    {
	ErrPostEx(SEV_ERROR, 0, 0, "[NBlastGetRecordFromNBlastDB]: Could not execute select statement.");
	LogSQLInfo(hstmt);
	SQLFreeStmt(hstmt, SQL_DROP);
	return SLRI_FAIL; /* SH_ERROR; */
    }

    if ( *pgi > 0 )
    {
	retcode = SQLBindCol(hstmt, 1, SQL_C_SLONG, &nblastdb_ord.val, sizeof(nblastdb_ord.val), NULL);
	if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
	{
	    ErrPostEx(SEV_ERROR, 0, 0, "[NBlastGetRecordFromNBlastDB] Could not bind column 1 ord.");
	    SQLFreeStmt(hstmt, SQL_DROP);
	    return SLRI_FAIL; /* SH_ERROR; */
	}
    }
    else if ( *pord > 0 )
    {
	retcode = SQLBindCol(hstmt, 1, SQL_C_SLONG, &nblastdb_gi.val, sizeof(nblastdb_gi.val), NULL);
	if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
	{
	    ErrPostEx(SEV_ERROR, 0, 0, "[NBlastGetRecordFromNBlastDB] Could not bind column 1 gi.");
	    SQLFreeStmt(hstmt, SQL_DROP);
	    return SLRI_FAIL; /* SH_ERROR; */
	}
    }
    else if (RecordNum > 0)
    {
	retcode = SQLBindCol(hstmt, 1, SQL_C_SLONG, &nblastdb_ord.val, sizeof(nblastdb_ord.val), NULL);
	if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
	{
	    ErrPostEx(SEV_ERROR, 0, 0, "[NBlastGetRecordFromNBlastDB] Could not bind column 1 ord.");
	    SQLFreeStmt(hstmt, SQL_DROP);
	    return SLRI_FAIL; /* SH_ERROR; */
	}
	retcode = SQLBindCol(hstmt, 4, SQL_C_SLONG, &nblastdb_gi.val, sizeof(nblastdb_gi.val), NULL);
	if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
	{
	    ErrPostEx(SEV_ERROR, 0, 0, "[NBlastGetRecordFromNBlastDB] Could not bind column 4 gi.");
	    SQLFreeStmt(hstmt, SQL_DROP);
	    return SLRI_FAIL; /* SH_ERROR; */
	}
    }

    retcode = SQLBindCol(hstmt, 2, SQL_C_SLONG, &nblastdb_num.val, sizeof(nblastdb_num.val), NULL);
    if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
    {
	ErrPostEx(SEV_ERROR, 0, 0, "[NBlastGetRecordFromNBlastDB] Could not bind column 2 num.");
	SQLFreeStmt(hstmt, SQL_DROP);
	return SLRI_FAIL; /* SH_ERROR; */
    }

    if( (( retcode = SQLBindCol(hstmt, 3, SQL_C_BINARY, &blobloc, 0, (long*)&asnbrvalue) ) != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO) )
    {
	ErrPostEx(SEV_ERROR, 0, 0, "[NBlastGetRecordFromNBlastDB]: Could not bind column 2 asnbr.");
	SQLFreeStmt(hstmt, SQL_DROP);
	return SLRI_FAIL; /* SH_ERROR; */
    }

    while( (retcode = SQLFetch(hstmt) ==  SQL_SUCCESS)  || (retcode == SQL_SUCCESS_WITH_INFO) )
    {
	fetchCount++;

	if ( *pgi > 0 && RecordNum == -1 )
	{
	      printf("ord:  %d \t", nblastdb_ord.val);
	      *pord = nblastdb_ord.val;
	}
	else if ( *pord > 0 && RecordNum == 0 )
	{
	      printf("gi:   %d \t", nblastdb_gi.val);
	      *pgi = nblastdb_gi.val;
	}
	else if (RecordNum > 0)
	{
	    *pord = nblastdb_ord.val;
	    *pgi = nblastdb_gi.val;
	}
	printf("num:  %d \n", nblastdb_num.val);
	*pnum = nblastdb_num.val;

	bloblen = (Int4) asnbrvalue;
	if (bloblen > 0)
	{
	      /* allocate a buffer big enough to store the blob. */
	      pcASN = (CharPtr) MemNew((size_t) bloblen);
              /*read the blob from the database */
	      SQLGetData(hstmt, 3, SQL_C_BINARY, (SQLPOINTER)pcASN, bloblen, (long*)&indicator);
	      if (pcASN != NULL)
	      {
		  *pNeighbours =  (NBlastGiAndEvalSetPtr) GetASNMemChar(pcASN,(AsnReadFunc)NBlastGiAndEvalSetAsnRead, bloblen);
	      }
	      if (pcASN) {MemFree(pcASN);}
	}
	else
	{
	    ErrPostEx(SEV_INFO, 0, 0, "[NBlastGetRecordFromNBlastDB]: ASN1 Blob retrieved has length = 0.");
	}
    }

    if(SQLFreeStmt(hstmt, SQL_DROP) != SQL_SUCCESS)
    {
	ErrPostEx(SEV_ERROR, 0, 0, "[NBlastGetRecordFromNBlastDB]: Could not free statement.");
	return SLRI_FAIL; /* SH_ERROR; */
    }

    if (fetchCount == 0)
    {
	return SLRI_FAIL; /* SH_NODATA; */
    }

    return SLRI_SUCCESS;  /* SH_SUCCESS; */
}



/*
 * Obs.: This function is call by NBLASTFindGIsAndGetUID()
 */

Int2 LIBCALL SearchORDbyGIinNBLASTDB(Int4 gi, Int4 PNTR pORD)
{

    SQLINTEGER key = 0;
    CharPtr pKey = NULL;                  /* search key for strings            */

    Char *pSQL_stmt;                      /* SQL select statement              */
    SQLHANDLE hstmt;                      /* handle to select statement        */
    SQLRETURN retcode;                    /* return values for odbc layer      */

    if ( gi <= 0)
    {
	ErrPostEx(SEV_ERROR, 0, 0, "[SearchORDbyGIinNBLASTDB]: Wrong gi. Could not be negative.");
	return SLRI_FAIL; /* SH_ERROR; */
    }
    key = gi;

    pSQL_stmt = (char*) malloc(sizeof(char) * 150);
    if(pSQL_stmt == NULL)
    {
	ErrPostEx(SEV_ERROR, 0, 0, "[SearchORDbyGIinNBLASTDB]: Could not allocate space");
	return SLRI_FAIL; /* SH_ERROR; */
    }

    sprintf((char*)pSQL_stmt, "select ord from %s.nblastdb where gi=?;", database);

    /* Create Handle Statement, once handle exists we can free char* */
    retcode = CreateHandle(&hstmt, pSQL_stmt, hdbc, &henv);
    if(retcode != SQL_SUCCESS)
    {
	ErrPostEx(SEV_ERROR, 0, 0, "[SearchORDbyGIinNBLASTDB]: Could not create handle.");
	free(pSQL_stmt);
	return SLRI_FAIL;  /* SH_ERROR; */
    }
    free(pSQL_stmt);

    retcode = SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &key, 0, NULL);
    if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
    {
	SQLFreeStmt(hstmt, SQL_DROP);
	ErrPostEx(SEV_ERROR, 0, 0, "[SearchORDbyGIinNBLASTDB]: Could not bind parameter");
	return SLRI_FAIL;  /* SH_ERROR; */ 
    }

    /* execute the select statement */
    if( (retcode = SQLExecute(hstmt)) != SQL_SUCCESS )
    {
	ErrPostEx(SEV_ERROR, 0, 0, "[SearchORDbyGIinNBLASTDB]: Could not execute select statement.");
	LogSQLInfo(hstmt);
	SQLFreeStmt(hstmt, SQL_DROP);
	return SLRI_FAIL;  /* SH_ERROR; */
    }

    retcode = SQLBindCol(hstmt, 1, SQL_C_SLONG, &nblastdb_ord.val, sizeof(nblastdb_ord.val), NULL);
    if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
    {
	ErrPostEx(SEV_ERROR, 0, 0, "[SearchORDbyGIinNBLASTDB]: Could not bind column 1. Could not execute select statement.");
	SQLFreeStmt(hstmt, SQL_DROP);
	return SLRI_FAIL;  /* SH_ERROR; */
    }

    /* fetch results and save in your linked list */
    SQLINTEGER fetchCount = 0;
    retcode = SQLFetch(hstmt);
    if ( (retcode ==  SQL_SUCCESS) || (retcode == SQL_SUCCESS_WITH_INFO) )
    {
	fetchCount++;
    }
    else
    {
	ErrPostEx(SEV_ERROR, 0, 0, "[SearchORDbyGIinNBLASTDB]: Could not Fetch statement.");
	return SLRI_FAIL;  /* SH_ERROR; */

    }

    if(SQLFreeStmt(hstmt, SQL_DROP) != SQL_SUCCESS)
    {
	ErrPostEx(SEV_ERROR, 0, 0, "[SearchORDbyGIinNBLASTDB]: Could not free statement.");
	return SLRI_FAIL;  /* SH_ERROR; */
    }

    if (fetchCount == 0)
    {
	ErrPostEx(SEV_ERROR, 0, 0, "[SearchORDbyGIinNBLASTDB]: gi not found.");
	return SLRI_FAIL;  /* SH_ERROR; */
    }
    if (fetchCount > 1)
    {
	ErrPostEx(SEV_ERROR, 0, 0, "[SearchORDbyGIinNBLASTDB]: More than 1 gi found.");
	return SLRI_FAIL;  /* SH_ERROR; */
    }
    *pORD = nblastdb_ord.val;
    return SLRI_SUCCESS;  /* SH_SUCCESS; */
}




Int2 LIBCALL SearchForOrdInNBLASTDBbyRecordNumber(Int4 recNum, Int4 PNTR pORD)
{

    SQLINTEGER key = 0;
    CharPtr pKey = NULL;                  /* search key for strings            */

    Char *pSQL_stmt;                      /* SQL select statement              */
    SQLHANDLE hstmt;                      /* handle to select statement        */
    SQLRETURN retcode;                    /* return values for odbc layer      */

    if ( recNum < 0)
    {
	ErrPostEx(SEV_ERROR, 0, 0, "[SearchForOrdInBLASTDBbyRecordNumber]: Wrong Record Number. Could not be negative.");
	return SH_ERROR;
    }
    key = recNum;
    printf("key: %d\n", key);
    pSQL_stmt = (char*) malloc(sizeof(char) * 150);
    if(pSQL_stmt == NULL)
    {
	ErrPostEx(SEV_ERROR, 0, 0, "[SearchForOrdInBLASTDBbyRecordNumber]: Could not allocate space");
	return SH_ERROR;
    }

    sprintf((char*)pSQL_stmt, "select ord from %s.nblastdb limit ?, 1;", database);
    printf("pSQL_stmt: %s\n", pSQL_stmt);
    /* Create Handle Statement, once handle exists we can free char* */
    retcode = CreateHandle(&hstmt, pSQL_stmt, hdbc, &henv);
    if(retcode != SQL_SUCCESS)
    {
	ErrPostEx(SEV_ERROR, 0, 0, "[SearchForOrdInBLASTDBbyRecordNumber]: Could not create handle.");
	free(pSQL_stmt);
	return SH_ERROR;
    }
    free(pSQL_stmt);

    retcode = SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &key, 0, NULL);
    if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
    {
	SQLFreeStmt(hstmt, SQL_DROP);
	ErrPostEx(SEV_ERROR, 0, 0, "[SearchForOrdInBLASTDBbyRecordNumber]: Could not bind Record Number parameter");
	return SH_ERROR;
    }

    /* execute the select statement */
    if( (retcode = SQLExecute(hstmt)) != SQL_SUCCESS )
    {
	ErrPostEx(SEV_ERROR, 0, 0, "[SearchForOrdInBLASTDBbyRecordNumber]: Could not execute select statement.");
	LogSQLInfo(hstmt);
	SQLFreeStmt(hstmt, SQL_DROP);
	return SH_ERROR;
    }

    retcode = SQLBindCol(hstmt, 1, SQL_C_SLONG, &nblastdb_ord.val, sizeof(nblastdb_ord.val), NULL);
    if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
    {
	ErrPostEx(SEV_ERROR, 0, 0, "[SearchForOrdInBLASTDBbyRecordNumber]: Could not bind column 1. Could not execute select statement.");
	SQLFreeStmt(hstmt, SQL_DROP);
	return SH_ERROR;
    }

    /* fetch results and save in your linked list */
    SQLINTEGER fetchCount = 0;
    retcode = SQLFetch(hstmt);
    if ( (retcode ==  SQL_SUCCESS) || (retcode == SQL_SUCCESS_WITH_INFO) )
    {
	fetchCount++;
    }
    else
    {
	ErrPostEx(SEV_ERROR, 0, 0, "[SearchForOrdInBLASTDBbyRecordNumber]: Could not Fetch statement.");
	return SH_ERROR;
    }

    if(SQLFreeStmt(hstmt, SQL_DROP) != SQL_SUCCESS)
    {
	ErrPostEx(SEV_ERROR, 0, 0, "[SearchForOrdInBLASTDBbyRecordNumber]: Could not free statement.");
	return SH_ERROR;
    }

    if (fetchCount == 0)
    {
	ErrPostEx(SEV_ERROR, 0, 0, "[SearchForOrdInBLASTDBbyRecordNumber]: gi not found.");
	return SH_NODATA;
    }
    if (fetchCount > 1)
    {
	ErrPostEx(SEV_ERROR, 0, 0, "[SearchForOrdInBLASTDBbyRecordNumber]: More than 1 gi found.");
	return SH_ERROR;
    }
    printf("ord: %d\n", nblastdb_ord.val);
    *pORD = nblastdb_ord.val;
    return SH_SUCCESS;
}


/* Save new BLAST struc in BlastDB */
SLRI_ERR SaveBlastInfoInBlastDB(NBlastResultSetPtr nbrsp, FloatHi best_evalue)
{
	Int8 uid = 0;
	Int4 ord1 = 0, ord2 = 0, ordtmp = 0, query_gi = 0, subject_gi = 0;
	char pcuidkey[SLRI_BUF_MAX];
/* Un-necessary Code query_gi and subject_gi allways will be 0.
	if( (query_gi<0) || (subject_gi<0) || (nbrsp == NULL) )
*/
	if( nbrsp == NULL)
	{
	    ErrPostEx(SEV_ERROR,0,0, "[SaveBlastInfoInBlastDB]: Invalid Parameter.");
	    return SLRI_FAIL;
	}



/* First Get the ords corresponding to the gi's */
/*	d4tagSelect(pd4NBlastDB,pt4gi);
	if(d4seekDouble(pd4NBlastDB,(double)nbrsp->query_gi) != r4success) {
		ErrPostEx(SEV_ERROR,0,0, "SaveBlastInfoInBlastDB: Could not find Query Gi in NBlastDB.");
		return SLRI_FAIL;
	}
	ord1 = (Int4) f4long(pf4ord);
	if(d4seekDouble(pd4NBlastDB,(double)nbrsp->subject_gi)!= r4success) {
		ErrPostEx(SEV_ERROR,0,0, "SaveBlastInfoInBlastDB: Could not find Subject Gi in NBlastDB.");
		return SLRI_FAIL;
	}
	ord2 = (Int4) f4long(pf4ord);
*/

	if ( SearchORDbyGIinNBLASTDB(nbrsp->query_gi, &ord1) != SLRI_SUCCESS )
	{
	    ErrPostEx(SEV_ERROR,0,0, "[SaveBlastInfoInBlastDB] Could not find Query Gi in NBlastDB.");
	    return SLRI_FAIL;
	}
	printf("ord1: %ld\n", ord1);

	if ( SearchORDbyGIinNBLASTDB(nbrsp->subject_gi, &ord2) != SLRI_SUCCESS )
	{
	    ErrPostEx(SEV_ERROR,0,0, "[SaveBlastInfoInBlastDB] Could not find Subject Gi in NBlastDB.");
	    return SLRI_FAIL;
	}
	printf("ord1: %ld\n", ord2);


/* for nblastupdate, or nblastneighbours */
	if(ord1 > ord2) {

		ordtmp = ord1;
		ord1 = ord2;
		ord2 = ordtmp;
	}
/* Now use the hash table function to get the uid */
	ORDtoUID(ord1, ord2, &uid);
	printf("uid: %ld\n", uid);
/* We can store up to 14 digits b4 rounding by using a double */
/* This let's us have a database of ~15M records b4 having to change to char array */
/*  To be implemented
          if(uid < MAX_UID) {
		if((d4seekDouble(pd4BlastDB, (double)uid)) != r4success)
                {
			SLRI_SafeLog10(&best_evalue);
			if(AppendRecordtoBlastDB(uid,best_evalue,nbrsp,TRUE))
			  return SLRI_FAIL;
		}
		else
		{
		     if(NJOB != NBLASTFROMUPDATE)
		     {
		          ErrPostEx(SEV_ERROR,0,0, "SaveBlastInfoInBlastDB: Entry Already Found in BlastDB!");
			  return SLRI_FAIL;
		     }
		}
*/

/* Otherwise, we can store by converting to char array*/
/* To be implemented.
	} else if (uid > MAX_UID) {
		Int8toStr(uid, pcuidkey);
		if((d4seek(pd4BlastDB, pcuidkey)) != r4success) {
			d4appendStart(pd4BlastDB, 0);
			f4assign(pf4uid, pcuidkey);
			SLRI_SafeLog10(&best_evalue);
			f4assignDouble(pf4eval, (double)best_evalue);
			AssignASNMemMemo(nbrsp, (AsnWriteFunc) NBlastResultSetAsnWrite, pf4asnsa);
			d4append(pd4BlastDB);
		} else {
			ErrPostEx(SEV_ERROR,0,0, "SaveBlastInfoInBlastDB: Entry Found in BlastDB!");
			return SLRI_FAIL;
		}
	}
*/
	return SLRI_SUCCESS;
}






Int2 LIBCALL SearchBLASTDB()
{

    SQLINTEGER key = 0;
    CharPtr pKey = NULL;                  /* search key for strings            */

    SQLINTEGER fetchCount = 0;            /* fetch results Counter             */
    Char *pSQL_stmt;                      /* SQL select statement              */
    SQLHANDLE hstmt;                      /* handle to select statement        */
    SQLRETURN retcode;                    /* return values for odbc layer      */

    pSQL_stmt = (char*) malloc(sizeof(char) * 200);
    if(pSQL_stmt == NULL)
    {
	ErrPostEx(SEV_ERROR, 0, 0, "[SearchBLASTDB]: Could not allocate space");
	return SH_ERROR;
    }

    sprintf((char*)pSQL_stmt, "select uid, eval from %s.blastdb where uid=13;", database);

    /* Create Handle Statement, once handle exists we can free char* */
    retcode = CreateHandle(&hstmt, pSQL_stmt, hdbc, &henv);
    if(retcode != SQL_SUCCESS)
    {
	ErrPostEx(SEV_ERROR, 0, 0, "[SearchBLASTDB]: Could not create handle.");
	free(pSQL_stmt);
	return SH_ERROR;
    }
    free(pSQL_stmt);

    retcode = SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &key, 0, NULL);
    if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
    {
	SQLFreeStmt(hstmt, SQL_DROP);
	ErrPostEx(SEV_ERROR, 0, 0, "[SearchBLASTDB]: Could not bind parameter");
	return SH_ERROR;
    }

    /* execute the select statement */
    if( (retcode = SQLExecute(hstmt)) != SQL_SUCCESS )
    {
	ErrPostEx(SEV_ERROR, 0, 0, "[SearchBLASTDB]: Could not execute select statement.");
	LogSQLInfo(hstmt);
	SQLFreeStmt(hstmt, SQL_DROP);
	return SH_ERROR;
    }

    retcode = SQLBindCol(hstmt, 1, SQL_C_SLONG, &blastdb_uid.val, sizeof(blastdb_uid.val), NULL);
    if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
    {
	ErrPostEx(SEV_ERROR, 0, 0, "[Could not bind column 1 in SearchBLASTDB]: Could not execute select statement.");
	SQLFreeStmt(hstmt, SQL_DROP);
	return SH_ERROR;
    }

    retcode = SQLBindCol(hstmt, 2, SQL_C_DOUBLE, &blastdb_eval.val, sizeof(blastdb_eval.val), NULL);
    if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
    {
	ErrPostEx(SEV_ERROR, 0, 0, "[Could not bind column 2 in SearchBLASTDB]: Could not execute select statement.");
	SQLFreeStmt(hstmt, SQL_DROP);
	return SH_ERROR;
    }

    while( (retcode = SQLFetch(hstmt) ==  SQL_SUCCESS)  || (retcode == SQL_SUCCESS_WITH_INFO) )
    {
	fetchCount++;
	if(retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO && fetchCount > 1)
	{
	    printf("Fetch OK\n");
	    /* pdbxref->next = StDbXrefNew(); */
	    /* pdbxref = pdbxref->next;       */
	}

	printf("uid:  %d \t", blastdb_uid.val);
	printf("eval: %f \n", blastdb_eval.val);
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

/* Makes an Int4 array of the nblast database gi's for quick lookup */
SLRI_ERR MakeORDtoGIarray(Int4Ptr PNTR ppaORDtoGI, Int4 PNTR pnumrec)
{
	Int4Ptr paORDtoGI = NULL;
	Int4 ord = 0, gi = 0, numrec = 0;
	MonitorPtr pmonitor = NULL;

    Char *pSQL_stmt;                      /* SQL select statement              */
    SQLHANDLE hstmt;                      /* handle to select statement        */
    SQLRETURN retcode;                    /* return values for odbc layer      */

    struct
    {
       SQLINTEGER len;
       SQLINTEGER val;
    }maxord;

	/* NULL returned for max() if the table is empty */
    /* initialize them to avoid a weird number */
	maxord.val=0; maxord.len=0;

	if(ppaORDtoGI == NULL) {
		ErrPostEx(SEV_ERROR,0,0, "[MakeORDtoGIarray]: You must pass in a reference variable!.");
		return SLRI_FAIL;
	}

	/* determine the highest ordinal number */

    pSQL_stmt = (char*) malloc(sizeof(char) * 200);
    if(pSQL_stmt == NULL)
    {
	   ErrPostEx(SEV_ERROR, 0, 0, "[MakeORDtoGIarray]: Could not allocate space");
	   return SLRI_FAIL;
    }

    sprintf((char*)pSQL_stmt, "select max(ord) as maxord from %s.nblastdb;", database);

    /* Create Handle Statement, once handle exists we can free char* */
    retcode = CreateHandle(&hstmt, pSQL_stmt, hdbc, &henv);
    if(retcode != SQL_SUCCESS)
    {
	   ErrPostEx(SEV_ERROR, 0, 0, "[MakeORDtoGIarray]: Could not create handle.");
	   free(pSQL_stmt);
	   return SLRI_FAIL;
    }
    free(pSQL_stmt);

    /* execute the select statement */
    if( (retcode = SQLExecute(hstmt)) != SQL_SUCCESS )
    {
	   ErrPostEx(SEV_ERROR, 0, 0, "[MakeORDtoGIarray]: Could not execute select statement.");
	   LogSQLInfo(hstmt);
	   SQLFreeStmt(hstmt, SQL_DROP);
	   return SLRI_FAIL;
    }

    retcode = SQLBindCol(hstmt, 1, SQL_C_SLONG, &maxord.val, sizeof(maxord.val), NULL);
    if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
    {
	   ErrPostEx(SEV_ERROR, 0, 0, "[MakeORDtoGIarray]: Could not bind col 1.");
	   SQLFreeStmt(hstmt, SQL_DROP);
	   return SLRI_FAIL;
    }

    while( (retcode = SQLFetch(hstmt) ==  SQL_SUCCESS)  || (retcode == SQL_SUCCESS_WITH_INFO) )
    {
	   numrec = maxord.val + 1;
    }

    if(SQLFreeStmt(hstmt, SQL_DROP) != SQL_SUCCESS)
    {
	   ErrPostEx(SEV_ERROR, 0, 0, "[MakeORDtoGIarray]: Could not free statement.");
	   return SLRI_FAIL;
    }

	if((paORDtoGI = MemNew((size_t)sizeof(Int4)*numrec+1)) == NULL) {
		ErrPostEx(SEV_ERROR,0,0, "[MakeORDtoGIarray]: Could not allocate memory for array!.");
		return SLRI_FAIL;
	}
#ifndef MOBIDICK_API
	if(!pmonitor) {
		if((pmonitor = MonitorIntNew("Ord to GI array:", 1, numrec)) == NULL) {
			ErrPostEx(SEV_ERROR,0,0, "[MakeORDtoGIarray]: Could not allocate memory for monitor!.");
			return SLRI_FAIL;
		}
	}
#endif
	/*Build Ord to GI array */
    pSQL_stmt = (char*) malloc(sizeof(char) * 200);
    if(pSQL_stmt == NULL)
    {
	   ErrPostEx(SEV_ERROR, 0, 0, "[MakeORDtoGIarray]: Could not allocate space");
	   return SLRI_FAIL;
    }

    sprintf((char*)pSQL_stmt, "select gi, ord from %s.nblastdb;", database);

    /* Create Handle Statement, once handle exists we can free char* */
    retcode = CreateHandle(&hstmt, pSQL_stmt, hdbc, &henv);
    if(retcode != SQL_SUCCESS)
    {
	   ErrPostEx(SEV_ERROR, 0, 0, "[MakeORDtoGIarray]: Could not create handle.");
	   free(pSQL_stmt);
	   return SLRI_FAIL;
    }
    free(pSQL_stmt);

    /* execute the select statement */
    if( (retcode = SQLExecute(hstmt)) != SQL_SUCCESS )
    {
	   ErrPostEx(SEV_ERROR, 0, 0, "[MakeORDtoGIarray]: Could not execute select statement.");
	   LogSQLInfo(hstmt);
	   SQLFreeStmt(hstmt, SQL_DROP);
	   return SLRI_FAIL;
    }

    retcode = SQLBindCol(hstmt, 1, SQL_C_SLONG, &nblastdb_gi.val, sizeof(nblastdb_gi.val), NULL);
    if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
    {
	   ErrPostEx(SEV_ERROR, 0, 0, "[MakeORDtoGIarray]: Could not bind col 1.");
	   SQLFreeStmt(hstmt, SQL_DROP);
	   return SLRI_FAIL;
    }

    retcode = SQLBindCol(hstmt, 2, SQL_C_SLONG, &nblastdb_ord.val, sizeof(nblastdb_ord.val), NULL);
    if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
    {
	   ErrPostEx(SEV_ERROR, 0, 0, "[MakeORDtoGIarray]: Could not bind col 1.");
	   SQLFreeStmt(hstmt, SQL_DROP);
	   return SLRI_FAIL;
    }

    while( (retcode = SQLFetch(hstmt) ==  SQL_SUCCESS)  || (retcode == SQL_SUCCESS_WITH_INFO) )
    {
	   gi = nblastdb_gi.val;
	   ord = nblastdb_ord.val;
	   paORDtoGI[ord] = gi;

#ifndef MOBIDICK_API
		MonitorIntValue(pmonitor, ord);
	}
	pmonitor = MonitorFree(pmonitor);
#else
	}
#endif

    if(SQLFreeStmt(hstmt, SQL_DROP) != SQL_SUCCESS)
    {
	   ErrPostEx(SEV_ERROR, 0, 0, "[MakeORDtoGIarray]: Could not free statement.");
	   return SH_ERROR;
    }

	printf("ORD to GI array build successful\n");

	*pnumrec = numrec;
	*ppaORDtoGI = paORDtoGI;

	return SLRI_SUCCESS;
}

/***************************************************************************
* BlastDBRecordCopy                                                        *
* Copies record from a the pd4BlastDBt database to the pd4BlastDB database *
* In order to build a single BlastDB database from multiple databases      *
****************************************************************************/
/* used for collecting databases from cluster nodes */
static SLRI_ERR BlastDBRecordCopy(void)
{
/*	d4appendStart(pd4BlastDB, 0);
	f4assignField(pf4uid,pf4uidt);
	f4assignField(pf4eval,pf4evalt);
	f4memoAssignN(pf4asnsa,f4memoStr(pf4asnsat),f4memoLen(pf4asnsat));
	d4append(pd4BlastDB);
*/	return SLRI_SUCCESS;
}

/************************************************************************
* NBlast Retrieval Functions                                            *
*************************************************************************
* GetBlastStrucFromDB           Get Blast Result set from BlastDB       *
* GetNBlastStrucFromDB          Get Neighbour set from NBlastDB         *
************************************************************************/
SLRI_ERR GetBlastStrucFromDB(Int8 uid, NBlastResultSetPtr PNTR nbrsp)
{
	Char pcbuf[SLRI_BUF_MAX];
	NBlastResultSetPtr nbrspHere = NULL;

	SQLDOUBLE key=0;
    Char *pSQL_stmt;                      /* SQL select statement              */
    SQLHANDLE hstmt;                      /* handle to select statement        */
    SQLRETURN retcode;                    /* return values for odbc layer      */

	Int4 bloblen = 0, indicator = 0;
	SQLINTEGER blobloc = 0;
	Int4 pbbvalue = 0;
	CharPtr pcASN = NULL;
	Int4 fetchCount=0;

	key = uid;
	Int8toStr(uid, pcbuf);

    pSQL_stmt = (char*) malloc(sizeof(char) * 200);
    if(pSQL_stmt == NULL)
    {
	   ErrPostEx(SEV_ERROR, 0, 0, "[GetBlastStrucFromDB]: Could not allocate space");
	   return SLRI_FAIL;
    }

    sprintf((char*)pSQL_stmt, "select asna from %s.blastdb where uid=?;", database);

    /* Create Handle Statement, once handle exists we can free char* */
    retcode = CreateHandle(&hstmt, pSQL_stmt, hdbc, &henv);
    if(retcode != SQL_SUCCESS)
    {
	   ErrPostEx(SEV_ERROR, 0, 0, "[GetBlastStrucFromDB]: Could not create handle.");
	   free(pSQL_stmt);
	   return SLRI_FAIL;
    }
    free(pSQL_stmt);

    retcode = SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_DOUBLE, SQL_DOUBLE, 0, 0, &key, 0, NULL);
    if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
    {
	   SQLFreeStmt(hstmt, SQL_DROP);
	   ErrPostEx(SEV_ERROR, 0, 0, "[GetBlastStrucFromDB]: Could not bind parameter 1");
	   return SLRI_FAIL;
    }

    /* execute the select statement */
    if( (retcode = SQLExecute(hstmt)) != SQL_SUCCESS )
    {
	   ErrPostEx(SEV_ERROR, 0, 0, "[GetBlastStrucFromDB]: Could not execute select statement.");
	   LogSQLInfo(hstmt);
	   SQLFreeStmt(hstmt, SQL_DROP);
	   return SLRI_FAIL;
    }

	retcode = SQLBindCol(hstmt, 1, SQL_C_BINARY, &blobloc, 0, (int*)&pbbvalue);
	if((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO))
	{
      ErrPostEx(SEV_ERROR,0,0, "[GetBlastStrucFromDB]: Binding Column 1 failed.");
	  SQLFreeStmt(hstmt, SQL_DROP);
      return SLRI_FAIL;
	}

    while( (retcode = SQLFetch(hstmt) ==  SQL_SUCCESS)  || (retcode == SQL_SUCCESS_WITH_INFO) )
    {
	  fetchCount++;

	  bloblen = (Int4)pbbvalue;

	  if(bloblen > 0)
	  {
	      /* allocate a buffer big enough to store the blob. */
	      pcASN = (CharPtr) MemNew((size_t) bloblen);

	      /*read the blob from the database */
	      retcode = SQLGetData(hstmt, 1, SQL_C_BINARY, (SQLPOINTER)pcASN, bloblen, (int*)&indicator);
		  if((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO))
		  {
			  ErrPostEx(SEV_ERROR,0,0, "[GetBlastStrucFromDB]: Getting blob failed.");
			  SQLFreeStmt(hstmt, SQL_DROP);
			  return SLRI_FAIL;
		  }

	      if(pcASN != NULL)
	      {
		      nbrspHere = (NBlastResultSetPtr) GetASNMemChar(pcASN,(AsnReadFunc)NBlastResultSetAsnRead, bloblen);
	      }
	      if (pcASN) { MemFree(pcASN); }

	  }
	  else
	  {
	      ErrPostEx(SEV_INFO, 0, 0, "[GetBlastStrucFromDB]: ASN1 Blob retrieved has length = 0. uid=%s", pcbuf);
	  }
    }

    if(SQLFreeStmt(hstmt, SQL_DROP) != SQL_SUCCESS)
    {
	   ErrPostEx(SEV_ERROR, 0, 0, "[GetBlastStrucFromDB]: Could not free statement.");
	   return SLRI_FAIL;
    }

	if (fetchCount <= 0) {
	   ErrPostEx(SEV_ERROR, 0, 0, "[GetBlastStrucFromDB]: Cannot find uid %s in BlastDB.", pcbuf);
	   return SLRI_FAIL;
    }


	if(nbrspHere == NULL) {
		ErrPostEx(SEV_INFO,0,0, "GetBlastStrucFromDB: No struc associated with uid %s in NBlastDB.", pcbuf);
	}

	*nbrsp = nbrspHere;
	return SLRI_SUCCESS;
}

/* Set the variables for the neighbour table build */
void NBlastBuildTableSetDataBaseVariables(void)
{
/*	cb.autoOpen     = 0;		*/   /* don't need indices for temp databases */
/*	cb.accessMode   = OPEN4DENY_RW; */   /* allows exclusive access to opened temp db file */
/*	cb.lockAttempts = WAIT4EVER;	*/   /* wait until lock is removed */
/* 	cb.errOff       = TRUE;         */

}

/* Enable database locks */
/* not used in even Codebase version*/
void NBlastDataBaseSetReadLock(Boolean bLock)
{
/*	if(bLock) cb.readLock  = TRUE;
	else cb.readLock = FALSE;
*/
}

void NBlastDataBaseLock(Int2 idb)
{
   CharPtr pSQL_stmt;
   SQLRETURN retcode=0;
   SQLHANDLE hstmt;

   pSQL_stmt = (char *) malloc(sizeof(Char) * 150);
   if (pSQL_stmt == NULL)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[NBlastDataBaseLock]: Could not allocate space");
     free(pSQL_stmt);
     return;
   }

   if(idb == BLASTDB) {
      sprintf((char *)pSQL_stmt, "lock tables %s.blastdb;", database);
   }

   if(idb == NBLASTDB) {
      sprintf((char *)pSQL_stmt, "lock tables %s.nblastdb;", database);
   }

   if(idb == BLASTDBt) {
      return;
   }

   retcode = CreateHandle(&hstmt, pSQL_stmt, hdbc, &henv);
   if (retcode != SQL_SUCCESS) {
     ErrPostEx(SEV_ERROR, 0, 0, "[NBlastDataBaseLock]: Could not create statement handle.");
     free(pSQL_stmt);
     return;
   }
   free(pSQL_stmt);

   retcode = SQLExecute(hstmt);
   if (retcode != SQL_SUCCESS)
   {
	  ErrPostEx(SEV_ERROR,0,0, "[NBlastDataBaseLock]: Unable to execute select statement.");
	  LogSQLInfo(hstmt);
	  SQLFreeStmt(hstmt, SQL_DROP);
     return;
   }

   retcode = SQLFreeStmt(hstmt, SQL_DROP);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_INFO, 0 ,0, "[NBlastDataBaseLock]: Unable to free statement.");
     return;
   }

}

void NBlastDataBaseUnlock(Int2 idb)
{
   CharPtr pSQL_stmt;
   SQLRETURN retcode=0;
   SQLHANDLE hstmt;

   pSQL_stmt = (char *) malloc(sizeof(Char) * 150);
   if (pSQL_stmt == NULL)
   {
     ErrPostEx(SEV_ERROR, 0, 0, "[NBlastDataBaseUnlock]: Could not allocate space");
     free(pSQL_stmt);
     return;
   }

   sprintf((char *)pSQL_stmt, "unlock tables;", database);

   retcode = CreateHandle(&hstmt, pSQL_stmt, hdbc, &henv);
   if (retcode != SQL_SUCCESS) {
     ErrPostEx(SEV_ERROR, 0, 0, "[NBlastDataBaseUnlock]: Could not create statement handle.");
     free(pSQL_stmt);
     return;
   }
   free(pSQL_stmt);

   retcode = SQLExecute(hstmt);
   if (retcode != SQL_SUCCESS)
   {
	  ErrPostEx(SEV_ERROR,0,0, "[NBlastDataBaseUnlock]: Unable to execute select statement.");
	  LogSQLInfo(hstmt);
	  SQLFreeStmt(hstmt, SQL_DROP);
     return;
   }

   retcode = SQLFreeStmt(hstmt, SQL_DROP);
   if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
   {
      ErrPostEx(SEV_INFO, 0 ,0, "[NBlastDataBaseUnlock]: Unable to free statement.");
     return;
   }

}

/* Pack the databases and their memofield contents */
/* not needed in ODBC */
SLRI_ERR NBlastPack(Int2 iDB)
{
/*	if(iDB == NBLASTDB) {
		if(pd4NBlastDB) {
			d4pack(pd4NBlastDB);
			d4memoCompress(pd4NBlastDB);
			d4reindex(pd4NBlastDB);
		} else {
			return SLRI_FAIL;
		}
	}
	if(iDB == BLASTDB) {
		if(pd4BlastDB) {
			d4pack(pd4BlastDB);
			d4memoCompress(pd4BlastDB);
			d4reindex(pd4BlastDB);
		} else {
			return SLRI_FAIL;
		}
	}
*/
	return SLRI_SUCCESS;
}


/* not needed in ODBC */
/* indices are created in creation script */
void NBlastTagSelect(Boolean bORD, Boolean bGI, Boolean bNUM, Boolean bUID)
{
/*	if(bORD) d4tagSelect(pd4NBlastDB,pt4gi);
	else if(bGI) d4tagSelect(pd4NBlastDB,pt4ord);
	else if(bNUM) d4tagSelect(pd4NBlastDB,pt4num);
	else if(bUID) d4tagSelect(pd4BlastDB,pt4uid);
*/
}


/* $Log: nblast_odbc.c,v $
/* Revision 1.6  2004/08/06 14:47:22  mmatan
/* Changed CharPtr database[50] to SQLCHAR database[50]
/*
/* Revision 1.5  2004/07/20 15:29:11  rcavero
/* Remove C++ Comments
/*
/* Revision 1.4  2004/06/02 21:38:01  zwang
/* Implement db layer functions.
/* */
