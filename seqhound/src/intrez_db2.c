/* $Id: intrez_db2.c,v 1.2 2004/01/06 22:32:15 skuznets Exp $
*******************************************************************************************
  Copyright (C) 2000 Mount Sinai Hospital (MSH)

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of
  the License, or any later version.
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

  See the GNU General Public License for more details.

  You should hav received a copy of the GNU General Public License
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
           Ruth Isserlin, Katerina Michalickova and Christopher W.V. Hogue hogue@mshri.on.ca

  REVISION/CONTRIBUTION NOTES:

  June 1, 2000  Original MSH Revision.
                Hogue Lab - University of Toronto Biochemistry
                Department and the Samuel Lunenfeld Research
                Institute, Mount Sinai Hospital
                http://bioinfo.mshri.on.ca  hogue@mshri.on.ca
                ftp://ftp.mshri.on.ca/


  PUBLICATION TO CITE:


  DESCRIPTION
   DB2 database layer.  Duplicate of intrez_db but using db2 calls instead of codebase.

******************************************************************************************/

#include <intrez_db.h>
#include <intrez_misc.h>
#include <intrez.h>
#include <slri_db2.h>

/* two variables used in every method to communicate with 
the db2 database */
static SQLHANDLE henv; /*envirnment handle */
static SQLHANDLE hdbc; /* connection handle */
static Boolean connected = FALSE; /*maintains connection status*/

/*in order to increase the speed of the parsing program
add all the insert statements as global variables so that
for every insert we can reuse the statements and just
rebind the variables.*/
SQLHANDLE hstmt_asndb=0, hstmt_parti=0, hstmt_taxgi=0, hstmt_pubseq=0, 
hstmt_nucprot=0, hstmt_accdb=0, hstmt_sendb=0, hstmt_sengi=0, 
hstmt_mmdb=0, hstmt_mmgi=0, hstmt_domdb=0, hstmt_cddb, hstmt_deletedgis = 0;

/********************************************/
/*  Open databases                          */
/********************************************/
/*Separate function to connect to the database*/
/*****************************************************/
/*  In db2 intializing a connection won't maintain   */
/*  a connection.  Every search function within the  */   
/*  db layer with have to make a connection to the database */
/*  and it can not relie on the initialization function */
/*  because if it is using the remote api each call is a*/
/*  separate call and once it is over the connection to */
/*  the database is lost.                            */
/*****************************************************/
Boolean ConnectToDb(void){

  SQLCHAR db2alias[10];
  SQLCHAR usr[10];
  SQLCHAR passwd[10];
  Char instance[10];
  Char instancebuf[25];

 GetAppParam("intrez", "datab", "username", NULL, (Char*)usr, (size_t)10 * (sizeof(Char)));
 GetAppParam("intrez", "datab", "password", NULL, (Char*)passwd, (size_t)10 * (sizeof(Char)));
 GetAppParam("intrez", "datab", "db2alias", NULL, (Char*)db2alias, (size_t)10 * (sizeof(Char)));
 GetAppParam("intrez", "datab", "instance", NULL, instance, (size_t)10 * (sizeof(Char)));

 /*make sure the proper DB2 instance variable is defined--only for aix system*/
  sprintf(instancebuf,"DB2INSTANCE=%s",IsNullStr(instance));
  putenv(instancebuf);

  /* allocate an envirnment handle */
  if (SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv) != SQL_SUCCESS){
    print_err(henv);
    ErrPostEx(SEV_ERROR,0,0, "Unable to allocate environment handle.");
    return FALSE;
  }

  /* allocate a connection handle */
  if (SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc) == SQL_ERROR){
       print_err(henv);
       ErrPostEx(SEV_ERROR,0,0, "Unable to allocate connection handle");
       return FALSE;
  }

  if(SQLConnect( hdbc, db2alias, SQL_NTS, usr, SQL_NTS, passwd, SQL_NTS) != SQL_SUCCESS){
       ErrPostEx(SEV_ERROR,0,0, "Connection to database failed");
       return FALSE;
  }
  connected = TRUE;
  return TRUE;
}
/*originally intializing codebase involves setting up variable
that deal with the codebase environment.  Now the intializaion 
involves connecting to the database using the cli interface.  
And setting up global varables. (insert globals should only
be intialized depending on a flag --> TO DO*/
Boolean LIBCALL InitCodeBase(void)
{
   Char instance[10];
   Char instancebuf[25];

   ConnectToDb();

 GetAppParam("intrez", "datab", "instance", NULL, instance, (size_t)10 * (sizeof(Char)));

   /*make sure the the DB2 instance variable is defined--only for aix system*/
    sprintf(instancebuf,"DB2INSTANCE=%s",IsNullStr(instance));

   putenv(instancebuf);

   /* turn autocommit off : optimization for the intial Load of data.
      Might consider making a function that stores all the load specific
      parameters and is activated by a flag when calling init*/

   SQLSetConnectAttr(hdbc,SQL_ATTR_AUTOCOMMIT,(SQLPOINTER)SQL_AUTOCOMMIT_OFF,SQL_NTS);

   /* create the global insert handles. */
   CreateHandle(&hstmt_asndb,"INSERT into seqhound.asndb(gi,asn1,division,release,type) values(?,?,?,?,?)", &hdbc,&henv);
   CreateHandle(&hstmt_parti,"INSERT into seqhound.parti(gi,division) values(?,?)",&hdbc,&henv);
   CreateHandle(&hstmt_taxgi,"INSERT INTO seqhound.taxgi(gi,taxid,kloodge,type) VALUES(?,?,?,?)",&hdbc,&henv);
   CreateHandle(&hstmt_nucprot,"INSERT into seqhound.nucprot(gia,gin) values(?,?)",&hdbc,&henv);
   CreateHandle(&hstmt_accdb,"INSERT into seqhound.accdb(gi,db,name,access,chain,release,version,title) values(?,?,?,?,?,?,?,?)",&hdbc,&henv);
   CreateHandle(&hstmt_pubseq,"INSERT into seqhound.pubseq(gi,muid) values(?,?)",&hdbc,&henv);
   CreateHandle(&hstmt_sengi,"INSERT into seqhound.sengi(gi,seid,division) values(?,?,?)",&hdbc,&henv);
   CreateHandle(&hstmt_sendb, "INSERT into seqhound.sendb(seid,asn1) values(?,?)",&hdbc,&henv);
   CreateHandle(&hstmt_mmdb, "INSERT into seqhound.mmdb(mmdbid,asn1,pdbid,bwhat,models,molecules,size,bzsize) values(?,?,?,?,?,?,?,?)",&hdbc,&henv);
   CreateHandle(&hstmt_mmgi, "INSERT into seqhound.mmgi(gi,mmdbid) values(?,?)",&hdbc,&henv);
   CreateHandle(&hstmt_domdb, "INSERT into seqhound.domdb(gi,asn1,mmdbid,pdbid,chain,domno,domall,domis,rep) values(?,?,?,?,?,?,?,?,?)",&hdbc,&henv);
   CreateHandle(&hstmt_cddb, "INSERT into seqhound.cddb(gi,asn1) values(?,?)",&hdbc,&henv);
   CreateHandle(&hstmt_deletedgis, "INSERT into seqhound.deletedGis(gi,access) values(?,?)", &hdbc, &henv);

   return TRUE;
}
/* this is to mirror CodeBase function which initializes tables with read only permissions */
Boolean LIBCALL InitCodeBaseReadOnly(void)
{
   return InitCodeBase();
}


/************************************************************/
/*  Given : the name of the table to be created and whether */
/*  the table is to be created (both are remenants          */
/*  from codebase)*/
/*  This method opens the table in codebase but in DB2      */
/*  there is no need to open the table.  This function      */
/*  creates the table in db2 if create is true and nothing  */
/*  otherwise.                                              */
/*  Returns : trueif successful and False otherwise         */
/*  ABOVE COMMECT APPLIES FOR ALL OF THE OPENXXXXX FUNCTIONS*/
/************************************************************/

Boolean LIBCALL OpenASNDB(CharPtr name, Boolean Create)
{
   SQLCHAR * asndb_create = (SQLCHAR *) "create table seqhound.asndb ("
	"gi INTEGER NOT NULL PRIMARY KEY," 
	"asn1 BLOB(1M) NOT LOGGED ,"
	"division VARCHAR(10)NOT NULL,"
	"release VARCHAR(10)NOT NULL," 
	"type VARCHAR(15) NOT NULL)";

/*check to see if table is supposed to be created.if yes then create it.*/
 if(Create){
	if(!execute_sql(asndb_create,&hdbc,&henv)){
		return FALSE;
	}
   }

   return TRUE;
}


Boolean LIBCALL OpenPARTI(CharPtr name, Boolean Create)
{

/*question the presence of this table in a db2 database becasue 
  theoretically we should not need to divide the database up into 
  partitions because the db2 database should be able to make
  a table that is very large.  For now this will remain but
  it is possible to depending on which backend one is using
  to have different tables as well as different database
  system.
*/

   SQLCHAR * parti_create = (SQLCHAR *) "create table seqhound.parti ("
	"gi INTEGER NOT NULL Primary KEY,"
	"division VARCHAR(10) NOT NULL)";

/*check to see if table is supposed to be created.if yes then create it.*/
 if(Create){
	if(!execute_sql(parti_create,&hdbc,&henv)){
		return FALSE;
	}
   }

   return TRUE;
}


Boolean LIBCALL OpenNUCPROT(CharPtr name, Boolean Create)
{

   SQLCHAR * nucprot_create = (SQLCHAR *) "create table seqhound.nucprot ("
	"gia INTEGER NOT NULL Primary KEY,"  
	"gin INTEGER NOT NULL)";

/*check to see if table is supposed to be created.if yes then create it.*/
 if(Create){
	if(!execute_sql(nucprot_create,&hdbc,&henv)){
		return FALSE;
	}
   }

   return TRUE;
}

Boolean LIBCALL OpenACCDB(CharPtr name, Boolean Create)
{
   SQLCHAR * accdb_create = (SQLCHAR *) "create table seqhound.accdb ("
	"gi INTEGER NOT NULL,"
	"db VARCHAR(15) NOT NULL,"
	"name VARCHAR(30),"
	"accession VARCHAR(20) NOT NULL,"
	"chain VARCHAR(20),"
  	"release VARCHAR(20),"
	"version INTEGER,"
        "title VARCHAR(250),"
         "Primary KEY(gi,db,accession))";

/*check to see if table is supposed to be created.if yes then create it.*/
 if(Create){
	if(!execute_sql(accdb_create,&hdbc,&henv)){
		return FALSE;
	}
   }

   return TRUE;
}


Boolean LIBCALL OpenPUBSEQ(CharPtr name, Boolean Create)
{
   SQLCHAR * pubseq_create = (SQLCHAR *) "create table seqhound.pubseq ("
	"gi INTEGER NOT NULL,"
	"muid INTEGER NOT NULL, "
        "Primary KEY (gi, muid)  )";

/*check to see if table is supposed to be created.if yes then create it.*/
 if(Create){
	if(!execute_sql(pubseq_create,&hdbc,&henv)){
		return FALSE;
	}
  }

   return TRUE;
}


Boolean LIBCALL OpenTAXGI(CharPtr name, Boolean Create)
{
   SQLCHAR * taxgi_create = (SQLCHAR *) "create table seqhound.taxgi ("
	"gi INTEGER NOT NULL Primary KEY,"
	"taxid INTEGER NOT NULL,"
	"kloodge INTEGER,"
	"type VARCHAR(20))";

/*check to see if table is supposed to be created.if yes then create it.*/
 if(Create){
	if(!execute_sql(taxgi_create,&hdbc,&henv)){
		return FALSE;
	}
   }

   return TRUE;
}

Boolean LIBCALL OpenCHROM(CharPtr name, Boolean Create)
{
   SQLCHAR * chrom_create = (SQLCHAR *) "create table seqhound.chrom ("
	"taxid INTEGER NOT NULL,"
	"kloodge INTEGER NOT NULL,"
	"chromfl INTEGER NOT NULL,"
	"Primary Key(taxid,kloodge,chromfl))";

/*check to see if table is supposed to be created.if yes then create it.*/
 if(Create){
	if(!execute_sql(chrom_create,&hdbc,&henv)){
		return FALSE;
	}
   }

   return TRUE;
}

/***********************************************************/
/*  DB2 version.  No longer need to open the table before  */
/*  it is accessed.  Method is given the name of the table */
/*  and whether or not it needs to be created.  True if the*/
/*  table needs to be created and false otherwise.         */
/***********************************************************/
Boolean LIBCALL OpenREDUND(CharPtr name, Boolean Create)
{
   SQLCHAR * redund_create = (SQLCHAR *) "create table seqhound.redund ("
	"gi INTEGER NOT NULL PRIMARY KEY," 
	"ordinal INTEGER NOT NULL," 
	"group INTEGER NOT NULL)";

/*check to see if table is supposed to be created.if yes then create it.*/
 if(Create){
	if(!execute_sql(redund_create,&hdbc,&henv)){
		return FALSE;
	}
   }

   return TRUE;
}


Boolean LIBCALL OpenSENDB(CharPtr name, Boolean Create)
{
  SQLCHAR * sendb_create = (SQLCHAR *) "create table seqhound.sendb ("
    "seid INTEGER NOT NULL PRIMARY KEY,"
    "asn1 BLOB(1M) NOT NULL NOT LOGGED)";

/*check to see if table is supposed to be created.if yes then create it.*/
 if(Create){
	if(!execute_sql(sendb_create,&hdbc,&henv)){
		return FALSE;
	}
   }

   return TRUE;
}

Boolean LIBCALL OpenSENGI(CharPtr name, Boolean Create)
{
  SQLCHAR * sengi_create = (SQLCHAR *) "create table seqhound.sengi ("
    "gi INTEGER NOT NULL PRIMARY KEY, "
    "seid INTEGER NOT NULL,"
    "division VARCHAR(10) NOT NULL)";    

/*check to see if table is supposed to be created.if yes then create it.*/
 if(Create){
	if(!execute_sql(sengi_create,&hdbc,&henv)){
		return FALSE;
	}
   }

   return TRUE;
}


Boolean LIBCALL OpenMMDB(CharPtr name, Boolean Create)
{
 SQLCHAR * mmdb_create = (SQLCHAR *) "create table seqhound.mmdb ("
    "mmdbid INTEGER NOT NULL PRIMARY KEY, "
	"asn1 BLOB(10M) NOT LOGGED, "
    "pdbid VARCHAR(20),"
	"bwhat INTEGER,"
	"models INTEGER,"
	"molecules INTEGER,"
	"size INTEGER,"
	"bzsize INTEGER,)";    

/*check to see if table is supposed to be created.if yes then create it.*/
 if(Create){
	if(!execute_sql(mmdb_create,&hdbc,&henv)){
		return FALSE;
	}
   }
   return TRUE;
}

Boolean LIBCALL OpenMMGI(CharPtr name, Boolean Create)
{
 SQLCHAR * mmgi_create = (SQLCHAR *) "create table seqhound.mmgi ("
    "gi INTEGER NOT NULL, "
    "mmdbid INTEGER NOT NULL,"
	"Primary KEY (gi, mmdbid)  )";    

/*check to see if table is supposed to be created.if yes then create it.*/
 if(Create){
	if(!execute_sql(mmgi_create,&hdbc,&henv)){
		return FALSE;
	}
   }
 
   return TRUE;
}

Boolean LIBCALL OpenDOMDB(CharPtr name, Boolean Create)
{
SQLCHAR * domdb_create = (SQLCHAR *) "create table seqhound.domdb ("
    "gi INTEGER NOT NULL, "
    "asn1 BLOB NOT LOGGED,"
    "mmdbid INTEGER NOT NULL,"
    "pdbid VARCHAR(20) NOT NULL,"
    "chain VARCHAR(10),"
    "domno INTEGER,"
    "domall INTEGER,"
    "domid INTEGER,"
    "rep INTEGER)";    

/*check to see if table is supposed to be created.if yes then create it.*/
 if(Create){
	if(!execute_sql(domdb_create,&hdbc,&henv)){
		return FALSE;
	}
  }
		return TRUE;
}

Boolean LIBCALL OpenCDDB(CharPtr name, Boolean Create)
{
	return TRUE;
}


/***************************************/
/*     Append records to databases     */
/***************************************/

Boolean LIBCALL AppendRecordASNDB(StAsndbPtr pas)
{
   pABL pABLtmp = NULL;
   Int4 bsLength=0;
   Int4 gi, releaselen,typelen, divisionlen;
   CharPtr asn1;
   Char division[10], release[10],type[15];  

   if(pas == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "AppendRecordASNDB: NULL parameter.");
      return FALSE;
    }

   /*get the asn1 bioseq into a bytestore pointer so that we can put it into a buffer.*/
  pABLtmp = AssignASNMemChar((Pointer) pas->asn1, (AsnWriteFunc) BioseqAsnWrite);

  /* SK: NULL ptr check */
  if ( pABLtmp == NULL )
  {
	ErrPostEx(SEV_ERROR,0,0, "AppendRecordASNDB: NULL pABLtmp");
	return FALSE;
  }
  
  asn1 = pABLtmp->buf;
  bsLength = pABLtmp->len;
  
   /*set up the input parameters */
  SQLBindParameter(hstmt_asndb, 1, SQL_PARAM_INPUT, SQL_C_LONG,SQL_INTEGER, 0, 0, &gi, 0, NULL);
  SQLBindParameter(hstmt_asndb, 2, SQL_PARAM_INPUT, SQL_C_DEFAULT,SQL_BLOB, 0, 0, asn1, 0, (long*)&bsLength);
  SQLBindParameter(hstmt_asndb, 3, SQL_PARAM_INPUT, SQL_C_DEFAULT,SQL_VARCHAR, 10, 0, division, 0, (long*)&divisionlen);
 SQLBindParameter(hstmt_asndb, 4, SQL_PARAM_INPUT, SQL_C_DEFAULT,SQL_VARCHAR, 10, 0, release, 0, (long*)&releaselen);
 SQLBindParameter(hstmt_asndb, 5, SQL_PARAM_INPUT, SQL_C_DEFAULT,SQL_VARCHAR, 15, 0, type, 0, (long*)&typelen);

 gi = pas->gi;
 sprintf(type,"%s",IsNullStr(pas->type));
 typelen = strlen(type);
 sprintf(division,"%s",IsNullStr(pas->division));
 divisionlen = strlen(division);
 sprintf(release,"%s",IsNullStr(pas->release));
 releaselen = strlen(release);

  /* execute statment */
  if (SQLExecute (hstmt_asndb) != SQL_SUCCESS){
	ErrPostEx(SEV_ERROR,0,0, "AppendRecordASNDB: unable to execute ASNDB insert statement (Gi:%ld)",gi);
	print_err(hstmt_asndb);
	return FALSE;
  }

  /*free all the memory that we have allocated*/
  asn1 = MemFree(asn1);
/*  pABLtmp->buf = MemFree(pABLtmp->buf);*/
  FreeABL(pABLtmp);
   return TRUE;
}


Boolean LIBCALL AppendRecordDeletedGis(Int4 gi, CharPtr accession){

	Char access[20];    
	Int4 accesslen = 0;

    if ((gi < 0) || (accession == NULL)){
      ErrPostEx(SEV_ERROR,0,0, "AppendRecordDeletedGis: NULL parameter.");
      return FALSE;
     }

   /*set up the input parameters */
  SQLBindParameter(hstmt_deletedgis, 1, SQL_PARAM_INPUT, SQL_C_LONG,SQL_INTEGER, 0, 0, &gi, 0, NULL);
  
  if(SQLBindParameter(hstmt_deletedgis, 2, SQL_PARAM_INPUT, SQL_C_DEFAULT,SQL_VARCHAR, 15, 0, access, 0, (long*)accesslen) != SQL_SUCCESS){	
	ErrPostEx(SEV_ERROR,0,0, "AppendRecordDeletedGis: unable to execute DeletedGis insert statement. (Gi=%ld)",gi);
	print_err(hstmt_deletedgis);
	}

   sprintf(access,"%s",IsNullStr(accession));
   accesslen = strlen(access);
/*   printf("accession : %s, accesslen : %ld\n", IsNullStr(access), accesslen);*/

  /* execute statment */
  if (SQLExecute (hstmt_deletedgis) != SQL_SUCCESS){
	ErrPostEx(SEV_ERROR,0,0, "AppendRecordDeletedGis: unable to execute DeletedGis insert statement. (Gi=%ld)",gi);
	print_err(hstmt_deletedgis);
	return FALSE;
  }

   return TRUE;
}

Boolean LIBCALL AppendRecordPARTI(StPartiPtr ppa)
{
   Int4 divisionlen, gi;
   Char division[10];

 if(ppa == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "AppendRecordPARTI: NULL parameter.");
      return FALSE;
    } 

 
   /*set up the input parameters */
  SQLBindParameter(hstmt_parti, 1, SQL_PARAM_INPUT, SQL_C_LONG,SQL_INTEGER, 0, 0, &gi, 0, NULL);
 SQLBindParameter(hstmt_parti, 2, SQL_PARAM_INPUT, SQL_C_DEFAULT,SQL_VARCHAR, 10, 0, division, 0, (long*)&divisionlen);

 gi = ppa->gi;
 sprintf(division,"%s",IsNullStr(ppa->partition));
 divisionlen = strlen(division);

  /* execute statment */
  if (SQLExecute (hstmt_parti) != SQL_SUCCESS){
	ErrPostEx(SEV_ERROR,0,0, "AppendRecordPARTI: unable to execute PARTI insert statement. (Gi=%ld)",gi);
	print_err(hstmt_parti);
	return FALSE;
  }

   return TRUE;

}

Boolean LIBCALL AppendRecordNUCPROT(StNucprotPtr pnc)
{
   Int4 gin, gia;

  if(pnc == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "AppendRecordNUCPROT: NULL parameter.");
      return FALSE;
    }


   /*set up the input parameters */
  SQLBindParameter(hstmt_nucprot, 1, SQL_PARAM_INPUT, SQL_C_LONG,SQL_INTEGER, 0, 0, &gia, 0, NULL);
  SQLBindParameter(hstmt_nucprot, 2, SQL_PARAM_INPUT, SQL_C_LONG,SQL_INTEGER, 0, 0, &gin, 0, NULL);

/* assign values to the parameters according to what was passed into the method. */
  gia = pnc->gia;
  gin = pnc->gin;

  /* execute statment */
  if (SQLExecute (hstmt_nucprot) != SQL_SUCCESS){
	ErrPostEx(SEV_ERROR,0,0, "AppendRecordNUCPROT: unable to execute NUCPROT insert statement.");
	print_err(hstmt_nucprot);
	return FALSE;
  }

   return TRUE;
}



Boolean LIBCALL AppendRecordACCDB(StAccdbPtr pac)
{
   Char db[15],name[30],access[20],chain[20],release[20],title[1024];
   Int4 gi,version,dblen,namelen,accesslen,chainlen,releaselen,titlelen;

   if(pac == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "AppendRecordACCDB: NULL parameter.");
      return FALSE;
    }

    /*set up the input parameters */
   SQLBindParameter(hstmt_accdb, 1, SQL_PARAM_INPUT, SQL_C_LONG,SQL_INTEGER, 0, 0, &gi, 0, NULL);
   SQLBindParameter(hstmt_accdb, 2, SQL_PARAM_INPUT, SQL_C_DEFAULT,SQL_VARCHAR, 15, 0, db, 0, (long*)&dblen);
   SQLBindParameter(hstmt_accdb, 3, SQL_PARAM_INPUT, SQL_C_DEFAULT,SQL_VARCHAR, 30, 0, name, 0, (long*)&namelen);
   SQLBindParameter(hstmt_accdb, 4, SQL_PARAM_INPUT, SQL_C_DEFAULT,SQL_VARCHAR, 15, 0, access, 0, (long*)&accesslen);
   SQLBindParameter(hstmt_accdb, 5, SQL_PARAM_INPUT, SQL_C_DEFAULT,SQL_VARCHAR, 20, 0, chain, 0, (long*)&chainlen);
   SQLBindParameter(hstmt_accdb, 6, SQL_PARAM_INPUT, SQL_C_DEFAULT,SQL_VARCHAR, 20, 0, release, 0, (long*)&releaselen);
   SQLBindParameter(hstmt_accdb, 7, SQL_PARAM_INPUT, SQL_C_LONG,SQL_INTEGER, 0, 0, &version, 0, NULL);
   SQLBindParameter(hstmt_accdb, 8, SQL_PARAM_INPUT, SQL_C_DEFAULT,SQL_VARCHAR, 1024, 0, title, 0, (long*)&titlelen);

 gi = pac->gi;
 sprintf(db,"%s",IsNullStr(pac->db));
 dblen = strlen(db);
 sprintf(name,"%s",IsNullStr(pac->name));
 namelen = strlen(name);
 sprintf(access,"%s",IsNullStr(pac->access));
 accesslen = strlen(access);
 sprintf(chain,"%s",IsNullStr(pac->chain));
 chainlen = strlen(chain);
 sprintf(release,"%s",IsNullStr(pac->release));
 releaselen = strlen(release);
 version = pac->version;
 sprintf(title,"%s",IsNullStr(pac->title));
 titlelen = strlen(title);

  /* execute statment */
  if (SQLExecute (hstmt_accdb) != SQL_SUCCESS){
	ErrPostEx(SEV_ERROR,0,0, "AppendRecordACCDB: unable to execute ACCDB insert statement. (gi=%ld)",gi);
	print_err(hstmt_accdb);
	return FALSE;
  }

   return TRUE;
}

Boolean LIBCALL AppendRecordPUBSEQ(StPubseqPtr pps, Int2 choice)
{
   Int4 gi, muid;

  if(pps == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "AppendRecordPUBSEQ: NULL parameter.");
      return FALSE;
    }

    /*set up the input parameters */
   SQLBindParameter(hstmt_pubseq, 1, SQL_PARAM_INPUT, SQL_C_LONG,SQL_INTEGER, 0, 0, &gi, 0, NULL);
  SQLBindParameter(hstmt_pubseq, 2, SQL_PARAM_INPUT, SQL_C_LONG,SQL_INTEGER, 0, 0, &muid, 0, NULL);

 gi = pps->gi;
 muid = pps->muid;

  /* execute statment */
  if (SQLExecute (hstmt_pubseq) != SQL_SUCCESS){
	ErrPostEx(SEV_ERROR,0,0, "AppendRecordPUBSEQ: unable to execute PUBSEQ insert statement. (gi=%ld)",gi);
	print_err(hstmt_pubseq);
	return FALSE;
  }

   return TRUE;
}

Boolean LIBCALL AppendRecordTAXGI(StTaxgiPtr ptg)
{
   Char type[15];
   Int4 gi, kloodge, taxid, typelen;
 
  if(ptg == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "AppendRecordTAXGI: NULL parameter.");
      return FALSE;
    }

    /*set up the input parameters */
   SQLBindParameter(hstmt_taxgi, 1, SQL_PARAM_INPUT, SQL_C_LONG,SQL_INTEGER, 0, 0, &gi, 0, NULL);
  SQLBindParameter(hstmt_taxgi, 2, SQL_PARAM_INPUT, SQL_C_LONG,SQL_INTEGER, 0, 0, &taxid, 0, NULL);
  SQLBindParameter(hstmt_taxgi, 3, SQL_PARAM_INPUT, SQL_C_LONG,SQL_INTEGER, 0, 0, &kloodge, 0, NULL);
   SQLBindParameter(hstmt_taxgi, 4, SQL_PARAM_INPUT, SQL_C_DEFAULT,SQL_VARCHAR, 15, 0, type, 0, (long*)&typelen);

 gi = ptg->gi;
 sprintf(type,"%s",IsNullStr(ptg->type));
 typelen = strlen(type);
 kloodge = ptg->kloodge;
 taxid = ptg->taxid;

  /* execute statment */
  if (SQLExecute (hstmt_taxgi) != SQL_SUCCESS){
	ErrPostEx(SEV_ERROR,0,0, "AppendRecordTAXGI: unable to execute TAXGI insert statement. (gi=%ld)",gi);
	print_err(hstmt_taxgi);
	return FALSE;
  }

   return TRUE;
}


Boolean LIBCALL AppendRecordREDUND(StRedundPtr prd)
{
   Int4 gi, ordinal, group;
   SQLHANDLE hstmt;
   CharPtr redund_insert =  "INSERT into seqhound.redund values(?,?,?)";
 
   if(prd == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "AppendRecordREDUND: NULL parameter.");
      return FALSE;
    }

  CreateHandle(&hstmt, redund_insert,&henv,&hdbc); 

   /*set up the input parameters */
  SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_LONG,SQL_INTEGER, 0, 0, &gi, 0, NULL);
  SQLBindParameter(hstmt, 2, SQL_PARAM_INPUT, SQL_C_LONG,SQL_INTEGER, 0, 0, &ordinal, 0, NULL);
  SQLBindParameter(hstmt, 3, SQL_PARAM_INPUT, SQL_C_LONG,SQL_INTEGER, 0, 0, &group, 0, NULL);

/* assign values to the parameters according to what was passed into the method. */
  gi = prd->gi;
  ordinal = prd->ordinal;
  group = prd->group;

  /* execute statment */
  if (SQLExecute (hstmt) != SQL_SUCCESS){
	ErrPostEx(SEV_ERROR,0,0, "AppendRecordREDUND: unable to execute redund insert statement. (gi=%ld)",gi);
	print_err(hstmt);
	SQLFreeStmt(hstmt, SQL_DROP);
	return FALSE;
  }

  SQLFreeStmt(hstmt, SQL_DROP);
  return TRUE;
}

Boolean LIBCALL AppendRecordSENDB(StSendbPtr psd)
{
   Int4 bsLength=0;
   Int4 seid;
   CharPtr asn1;
   pABL pABLtmp = NULL;

 if(psd == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "AppendRecordSENDB: NULL parameter.");
      return FALSE;
    }
 /*get the asn1 bioseq into a bytestore pointer so that we can put it into a buffer.*/
  pABLtmp = AssignASNMemChar((Pointer) psd->asn1, (AsnWriteFunc) SeqEntryAsnWrite);
  /* SK: NULL ptr check */
  if ( pABLtmp == NULL )
  {
	ErrPostEx(SEV_ERROR,0,0, "AppendRecordSENDB: NULL pABLtmp");
	return FALSE;
  }
  bsLength = pABLtmp->len;
  asn1 = pABLtmp->buf;

   /*set up the input parameters */
  SQLBindParameter(hstmt_sendb, 1, SQL_PARAM_INPUT, SQL_C_LONG,SQL_INTEGER, 0, 0, &seid, 0, NULL);
  SQLBindParameter(hstmt_sendb, 2, SQL_PARAM_INPUT, SQL_C_DEFAULT,SQL_BLOB, 0, 0, asn1, 0, (long*)&bsLength);

 seid = psd->seid;

  /* execute statment */
  if (SQLExecute (hstmt_sendb) != SQL_SUCCESS){
	ErrPostEx(SEV_ERROR,0,0, "AppendRecordSENDB: unable to execute SENDB insert statement. (seid=%ld)",seid);
	print_err(hstmt_sendb);
	return FALSE;
  }

 
  asn1 = MemFree(asn1);
  /*pABLtmp->buf = MemFree(pABLtmp->buf);*/
  FreeABL(pABLtmp);
  return TRUE;

}


Boolean LIBCALL AppendRecordSENGI(StSengiPtr psg)
{

   Int4 divisionlen, gi,seid;
   Char division[10];

 if(psg == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "AppendRecordSENGI: NULL parameter.");
      return FALSE;
    } 
 
   /*set up the input parameters */
  SQLBindParameter(hstmt_sengi, 1, SQL_PARAM_INPUT, SQL_C_LONG,SQL_INTEGER, 0, 0, &gi, 0, NULL);
  SQLBindParameter(hstmt_sengi, 2, SQL_PARAM_INPUT, SQL_C_LONG,SQL_INTEGER, 0, 0, &seid, 0, NULL);
  SQLBindParameter(hstmt_sengi, 3, SQL_PARAM_INPUT, SQL_C_DEFAULT,SQL_VARCHAR, 10, 0, division, 0, (long*)&divisionlen);

 gi = psg->gi;
 seid = psg->seid;
 sprintf(division,"%s",IsNullStr(psg->division));
 divisionlen = strlen(division);

  /* execute statment */
  if (SQLExecute (hstmt_sengi) != SQL_SUCCESS){
	ErrPostEx(SEV_ERROR,0,0, "AppendRecordSENGI: unable to execute SENGI insert statement. (gi=%ld)",gi);
	print_err(hstmt_sengi);
	return FALSE;
  }

   return TRUE;
}

Boolean LIBCALL AppendRecordMMDB(StMmdbPtr pmd)
{
   Int4 bzLength=0,bsplen=0,models=0,molecules=0,bwhat=0,mmdbid=0,pdbidlen=0;
   CharPtr asn1=NULL;
   Char pdbid[20];
   pMBL pMBLtmp = NULL;

   if(pmd == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "AppendRecordMMDB: NULL parameter.");
      return FALSE;
    }

 /*get the asn1 bioseq into a bytestore pointer so that we can put it into a buffer.*/
  if(pmd->asn1 != NULL)
    {
      pMBLtmp = AssignASNMemBZChar((Pointer) pmd->asn1, (AsnWriteFunc) BiostrucAsnWrite);
      if (pMBLtmp == NULL)
       {
           ErrPostEx(SEV_ERROR,5,1,"AssignASNMemChar failed");
           ClearStructures();
           return FALSE;
       }
    }
 
  bzLength = pMBLtmp->bzlen;
  bsplen = pMBLtmp->bslen;
  asn1 = pMBLtmp->buf;

   /*set up the input parameters */
  SQLBindParameter(hstmt_mmdb, 1, SQL_PARAM_INPUT, SQL_C_LONG,SQL_INTEGER, 0, 0, &mmdbid, 0, NULL);
  SQLBindParameter(hstmt_mmdb, 2, SQL_PARAM_INPUT, SQL_C_DEFAULT,SQL_BLOB, 0, 0, asn1, 0, (long*)&bzLength);
  SQLBindParameter(hstmt_mmdb, 3, SQL_PARAM_INPUT, SQL_C_DEFAULT,SQL_VARCHAR, 20, 0, pdbid, 0, (long*)&pdbidlen);
  SQLBindParameter(hstmt_mmdb, 4, SQL_PARAM_INPUT, SQL_C_LONG,SQL_INTEGER, 0, 0, &bwhat, 0, NULL);
  SQLBindParameter(hstmt_mmdb, 5, SQL_PARAM_INPUT, SQL_C_LONG,SQL_INTEGER, 0, 0, &models, 0, NULL);
  SQLBindParameter(hstmt_mmdb, 6, SQL_PARAM_INPUT, SQL_C_LONG,SQL_INTEGER, 0, 0, &molecules, 0, NULL);
  SQLBindParameter(hstmt_mmdb, 7, SQL_PARAM_INPUT, SQL_C_LONG,SQL_INTEGER, 0, 0, &bsplen, 0, NULL);
  SQLBindParameter(hstmt_mmdb, 8, SQL_PARAM_INPUT, SQL_C_LONG,SQL_INTEGER, 0, 0, &bzLength, 0, NULL);

  mmdbid = pmd->mmdbid;
  sprintf(pdbid,"%s",IsNullStr(pmd->pdbid));
  pdbidlen = strlen(pdbid);
  bwhat = pmd->bwhat;
  models = pmd->models;
  molecules = pmd->molecules;
  
  /* execute statment */
  if (SQLExecute (hstmt_mmdb) != SQL_SUCCESS){
	ErrPostEx(SEV_ERROR,0,0, "AppendRecordMMDB: unable to execute MMDB insert statement. (mmdb id=%ld",mmdbid);
	print_err(hstmt_mmdb);
	return FALSE;
  }

  asn1 = MemFree(asn1);
  /*pABLtmp->buf = MemFree(pABLtmp->buf);*/
  FreeMBL(pMBLtmp);
  return TRUE;
  
}


Boolean LIBCALL AppendRecordMMGI(StMmgiPtr pmg)
{
  Int4 gi=0, mmdbid=0;

   if(pmg == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "AppendRecordMMGI: NULL parameter.");
      return FALSE;
    }

    /*set up the input parameters */
   SQLBindParameter(hstmt_mmgi, 1, SQL_PARAM_INPUT, SQL_C_LONG,SQL_INTEGER, 0, 0, &gi, 0, NULL);
   SQLBindParameter(hstmt_mmgi, 2, SQL_PARAM_INPUT, SQL_C_LONG,SQL_INTEGER, 0, 0, &mmdbid, 0, NULL);

   gi = pmg->gi;
   mmdbid = pmg->mmdbid;

  /* execute statment */
  if (SQLExecute (hstmt_mmgi) != SQL_SUCCESS){
	ErrPostEx(SEV_ERROR,0,0, "AppendRecordMMGI: unable to execute MMGI insert statement. (gi=%ld)",gi);
	print_err(hstmt_mmgi);
	return FALSE;
  }

   return TRUE;

}

Boolean LIBCALL AppendRecordDOMDB(StDomdbPtr pdd)
{
   pABL pABLtmp = NULL;
   Int4 bsLength=0;
   Int4 gi, mmdbid,domno,domall,domid,rep,pdbidlen, chainlen;
   CharPtr asn1;
   Char pdbid[20],chain[10];  

  if(pdd == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "AppendRecordDOMDB: NULL parameter.");
      return FALSE;
    }

   /*get the asn1 bioseq into a bytestore pointer so that we can put it into a buffer.*/
  pABLtmp = AssignASNMemChar((Pointer) pdd->asn1,(AsnWriteFunc)SLRIValNodeAsnWrite);
  /* SK: NULL ptr check */
  if ( pABLtmp == NULL )
  {
	ErrPostEx(SEV_ERROR,0,0, "AppendRecordDOMDB: NULL pABLtmp");
	return FALSE;
  }
  asn1 = pABLtmp->buf;
  bsLength = pABLtmp->len;
  
   /*set up the input parameters */
  SQLBindParameter(hstmt_domdb, 1, SQL_PARAM_INPUT, SQL_C_LONG,SQL_INTEGER, 0, 0, &gi, 0, NULL);
  SQLBindParameter(hstmt_domdb, 2, SQL_PARAM_INPUT, SQL_C_DEFAULT,SQL_BLOB, 0, 0, asn1, 0, (long*)&bsLength);
 SQLBindParameter(hstmt_domdb, 3, SQL_PARAM_INPUT, SQL_C_LONG,SQL_INTEGER, 0, 0, &mmdbid, 0, NULL);
  SQLBindParameter(hstmt_domdb, 4, SQL_PARAM_INPUT, SQL_C_DEFAULT,SQL_VARCHAR, 10, 0, pdbid, 0, (long*)&pdbidlen);
 SQLBindParameter(hstmt_domdb, 5, SQL_PARAM_INPUT, SQL_C_DEFAULT,SQL_VARCHAR, 10, 0, chain, 0, (long*)&chainlen);
 SQLBindParameter(hstmt_domdb, 6, SQL_PARAM_INPUT, SQL_C_LONG,SQL_INTEGER, 0, 0, &domno, 0, NULL);
 SQLBindParameter(hstmt_domdb, 7, SQL_PARAM_INPUT, SQL_C_LONG,SQL_INTEGER, 0, 0, &domall, 0, NULL);
 SQLBindParameter(hstmt_domdb, 8, SQL_PARAM_INPUT, SQL_C_LONG,SQL_INTEGER, 0, 0, &domid, 0, NULL);
 SQLBindParameter(hstmt_domdb, 9, SQL_PARAM_INPUT, SQL_C_LONG,SQL_INTEGER, 0, 0, &rep, 0, NULL);

 gi = pdd->gi;
 sprintf(pdbid,"%s",IsNullStr(pdd->pdbid));
 pdbidlen = strlen(pdbid);
 sprintf(chain,"%s",IsNullStr(pdd->chain));
 chainlen = strlen(chain);
 mmdbid = pdd->mmdbid;
 domid = pdd->domid;
 domno = pdd->domno;
 domall = pdd->domall;
 rep = pdd->rep;

  /* execute statment */
  if (SQLExecute (hstmt_domdb) != SQL_SUCCESS){
	ErrPostEx(SEV_ERROR,0,0, "AppendRecordDOMDB: unable to execute ASNDB insert statement. (gi=%ld)",gi);
	print_err(hstmt_domdb);
	return FALSE;
  }

  /*free all the memory that we have allocated*/
  asn1 = MemFree(asn1);
/*  pABLtmp->buf = MemFree(pABLtmp->buf);*/
  FreeABL(pABLtmp);
   return TRUE;

}


Boolean LIBCALL AppendRecordCHROM(StChromPtr pch)
{
  SQLRETURN sqlrc = SQL_SUCCESS;
   Int4 taxid,kloodge,chromfl;
   SQLHANDLE hstmt;
   SQLCHAR * chrom_insert = (SQLCHAR *) "INSERT into seqhound.chrom(taxonid,chromid,chromflag) values(?,?,?)";
  
   if(pch == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "AppendRecordCHROM: NULL parameter.");
      return FALSE;
    }
   /* allocate a statement handle */
   sqlrc = SQLAllocStmt(hdbc, &hstmt);
   if (sqlrc != SQL_SUCCESS){
	ErrPostEx(SEV_ERROR,0,0, "AppendRecordCHROM: could not allocate a statement handle.");
	return FALSE;
   }

  /* prepare the insert statement */
  sqlrc = SQLPrepare( hstmt, chrom_insert, SQL_NTS);
  if (sqlrc != SQL_SUCCESS){
	ErrPostEx(SEV_ERROR,0,0, "AppendRecordCHROM: could not prepare the statement ");
	SQLFreeStmt(hstmt, SQL_DROP);
	return FALSE;
   }

   /*set up the input parameters */
  SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_LONG,SQL_INTEGER, 0, 0, &taxid, 0, NULL);
  SQLBindParameter(hstmt, 2, SQL_PARAM_INPUT, SQL_C_LONG,SQL_INTEGER, 0, 0, &kloodge, 0, NULL);
  SQLBindParameter(hstmt, 3, SQL_PARAM_INPUT, SQL_C_LONG,SQL_INTEGER, 0, 0, &chromfl, 0, NULL);

/* assign values to the parameters according to what was passed into the method. */
  taxid = pch->taxid;
  kloodge = pch->kloodge;
  chromfl = pch->chromfl;

  /* execute statment */
  if (SQLExecute (hstmt) != SQL_SUCCESS){
	ErrPostEx(SEV_ERROR,0,0, "AppendRecordCHROM: unable to execute insert statement. ");
	print_err(hstmt);
	SQLFreeStmt(hstmt, SQL_DROP);
	return FALSE;
  }

  /* commit the inserts */
  SQLTransact(henv, hdbc, SQL_COMMIT);

  SQLFreeStmt(hstmt, SQL_DROP);

   return TRUE;

}

Boolean LIBCALL AppendRecordCDDB(StCddbPtr pcd)
{
   Int4 bsLength=0;
   Int4 gi;  
   CharPtr asn1;
   pABL pABLtmp = NULL;
  
   if(pcd == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "AppendRecordCDDB: NULL parameter.");
      return FALSE;
    }
   
   /*get the asn1 bioseq into a bytestore pointer so that we can put it into a buffer.*/
  pABLtmp = AssignASNMemChar((Pointer) pcd->asn1, (AsnWriteFunc) SLRIFastaAsnWrite);
  /* SK: NULL ptr check */
  if ( pABLtmp == NULL )
  {
	ErrPostEx(SEV_ERROR,0,0, "AppendRecordCDDB: NULL pABLtmp");
	return FALSE;
  }

  asn1 = pABLtmp->buf;
  bsLength = pABLtmp->len;

  SQLBindParameter(hstmt_cddb, 1, SQL_PARAM_INPUT, SQL_C_LONG,SQL_INTEGER, 0, 0, &gi, 0, NULL);
  SQLBindParameter(hstmt_cddb, 2, SQL_PARAM_INPUT, SQL_C_DEFAULT,SQL_BLOB, 0, 0, asn1, 0, &bsLength);

  gi = pcd->gi;
   
 /* execute statment */
  if (SQLExecute (hstmt_cddb) != SQL_SUCCESS){
        ErrPostEx(SEV_ERROR,0,0, "AppendRecordCDDB: unable to execute insert statement");
        print_err(hstmt_cddb);
        return FALSE;  
  }
   
  /*free all the memory that we have allocated*/
  asn1 = MemFree(asn1);
  FreeABL(pABLtmp);
  return TRUE;
}

/***************************************/
/*     Close databases                 */
/***************************************/

Boolean LIBCALL CloseASNDB(void)
{
   return TRUE;
}


Boolean LIBCALL ClosePARTI(void)
{
    return TRUE;
}


Boolean LIBCALL CloseNUCPROT(void)
{
   return TRUE;
}


Boolean LIBCALL CloseACCDB(void)
{
   return TRUE;
}


Boolean LIBCALL ClosePUBSEQ(void)
{
   return TRUE;
}


Boolean LIBCALL CloseTAXGI(void)
{
   return TRUE;
}


Boolean LIBCALL CloseCHROM(void)
{
   return TRUE;
}

Boolean LIBCALL CloseREDUND(void)
{
  return TRUE;
}

Boolean LIBCALL CloseSENDB(void)
{
     return TRUE;
}


Boolean LIBCALL CloseSENGI(void)
{
     return TRUE;
}


Boolean LIBCALL CloseMMDB(void)
{
     return TRUE;
}

Boolean LIBCALL CloseMMGI(void)
{
     return TRUE;
}


Boolean LIBCALL CloseDOMDB(void)
{
     return TRUE;
}

Boolean LIBCALL CloseVASTDB(void)
{
     return TRUE;
}


Boolean LIBCALL Commit_inserts(void)
{

 /* commit all changes */
  if( SQLTransact(henv, hdbc, SQL_COMMIT) != SQL_SUCCESS){
	ErrPostEx(SEV_INFO, 0, 0, "Could not commit the changes.");
   }
  return TRUE;

}


Boolean LIBCALL CloseCodeBase(void)
{

   Int2 sqlrc = SQL_SUCCESS;

 /* commit all changes */
  SQLTransact(henv, hdbc, SQL_COMMIT);

   /* Free statement handles. */
   if (SQLFreeHandle(SQL_HANDLE_STMT, hstmt_asndb) != SQL_SUCCESS)
     print_err(hstmt_asndb);
   if (SQLFreeHandle(SQL_HANDLE_STMT, hstmt_parti) != SQL_SUCCESS)
     print_err(hstmt_parti);
   if (SQLFreeHandle(SQL_HANDLE_STMT, hstmt_nucprot) != SQL_SUCCESS)
     print_err(hstmt_nucprot);
   if (SQLFreeHandle(SQL_HANDLE_STMT, hstmt_accdb) != SQL_SUCCESS)
     print_err(hstmt_accdb);
   if (SQLFreeHandle(SQL_HANDLE_STMT, hstmt_pubseq) != SQL_SUCCESS)
     print_err(hstmt_pubseq);
   if (SQLFreeHandle(SQL_HANDLE_STMT, hstmt_taxgi) != SQL_SUCCESS)
     print_err(hstmt_taxgi);
   if (SQLFreeHandle(SQL_HANDLE_STMT, hstmt_sendb) != SQL_SUCCESS)
     print_err(hstmt_sendb);
   if (SQLFreeHandle(SQL_HANDLE_STMT, hstmt_sengi) != SQL_SUCCESS)
     print_err(hstmt_sengi);
   if (SQLFreeHandle(SQL_HANDLE_STMT, hstmt_mmdb) != SQL_SUCCESS)
     print_err(hstmt_mmdb);
   if (SQLFreeHandle(SQL_HANDLE_STMT, hstmt_mmgi) != SQL_SUCCESS)
     print_err(hstmt_mmgi);
   if (SQLFreeHandle(SQL_HANDLE_STMT, hstmt_cddb) != SQL_SUCCESS)
     print_err(hstmt_cddb);

   /*Free the database handles used to communicate with the database*/
   sqlrc = SQLDisconnect( hdbc);
   if (sqlrc != SQL_SUCCESS){
	ErrPostEx(SEV_INFO, 0, 0, "Could not disconnect from the database.\n");
   }

   /*free the connnection handle */
   sqlrc = SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
   if (sqlrc != SQL_SUCCESS) {
	ErrPostEx(SEV_INFO, 0, 0, "could not free the connection handle. \n");
   }

   /*free envirnment handle */
   sqlrc = SQLFreeHandle(SQL_HANDLE_ENV, henv);
   if (sqlrc != SQL_SUCCESS) {
	ErrPostEx(SEV_INFO, 0, 0, "could not free the environment handle. \n");	
   }

   return TRUE;
 
}

/**************************************************/
/*Given : a binary buffer that was just extracted */
/*  from the database and the length of info in   */
/*  the buffer this method reads the binary info  */
/*  And assuming that the info represents a Bioseq*/
/*Returns : a Bioseq of the handed in buffer      */
/**************************************************/
BioseqPtr GetBioseq(CharPtr pcASN, Int4 bloblen){
  AsnIoMemPtr aimp; 
  BioseqPtr pbsq;
 
  aimp = AsnIoMemOpen("rb", (BytePtr) pcASN, bloblen);
   if (aimp == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "GetBioseq: Failed to get AsnIo.");
      if (pcASN) MemFree(pcASN);
      return NULL;
    }
   pbsq = BioseqAsnRead(aimp->aip, NULL);
   if (pbsq == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "GetBioseq: Bioseq pointer is NULL.");
      if (pcASN) MemFree(pcASN);
      AsnIoMemClose(aimp);
      return NULL;
    }

   AsnIoFlush(aimp->aip);
   AsnIoMemClose(aimp);

   return pbsq;
}

/*************************************/
/* SHound databases search functions */
/*************************************/
StAsndbPtr LIBCALL SearchASNDBBulk(ValNodePtr pvn){
   StAsndbPtr pasCurrent = NULL, pas = NULL;
   Int4 bloblen=0,indicator=0;
   CharPtr search_asndb = "select gi,division,release,type,asn1 from seqhound.asndb where gi IN (";
   SQLHANDLE hstmt=0,LocUse=0;
   Int2 sqlrc=SQL_SUCCESS;
   SQLINTEGER blobloc=0, pbbvalue=0;
   CharPtr pcASN = NULL;
   CharPtr buf=NULL;
   struct{
     Int4 len;
     Char val[15];
   }division, release, type;
   struct{
     Int4 len;
     Int4 val;
   }gi;

   pasCurrent = StAsndbNew();
   pas = pasCurrent;

   if (pvn == NULL)
     {
      ErrPostEx(SEV_ERROR,0,0, "SearchAsndbBulk: list of gis is null");
      return NULL;
     }
   /*put all the gis into the select statement*/
    buf = MemNew(80*sizeof(Char));
   while(pvn != NULL){
     sprintf(buf,"%ld",pvn->data.intvalue);
     strcat(search_asndb, buf);
     pvn = pvn->next;
     if(pvn != NULL)
       strcat(search_asndb, " , ");
   }
   strcat(search_asndb, " )");
   MemFree(buf);

   /* search ASNDB */
   CreateHandle(&hstmt,search_asndb,&henv,&hdbc);

  /* set auto commit on */
   SQLSetConnectAttr( hdbc, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_ON, SQL_NTS);  

  /* execute statment */
  if (SQLExecute(hstmt) != SQL_SUCCESS){
	ErrPostEx(SEV_ERROR,0,0,"SEARCH_ASNDB_BULK:unable to execute select statement.");
	print_err(hstmt);
	SQLFreeStmt(hstmt, SQL_DROP);
	return FALSE;
  }
  SQLBindCol(hstmt,1,SQL_C_LONG,&gi.val,0,(long*)&gi.len);
  SQLBindCol(hstmt,2,SQL_C_CHAR,division.val,10,(long*)&division.len);  
  SQLBindCol(hstmt,3,SQL_C_CHAR,release.val,10,(long*)&release.len);
  SQLBindCol(hstmt,4,SQL_C_CHAR,type.val,15,(long*)&type.len);
  SQLBindCol(hstmt,5, SQL_C_BLOB_LOCATOR,&blobloc,0,(long*)&pbbvalue);
  
  sqlrc = SQLFetch(hstmt);
  if(sqlrc == SQL_NO_DATA_FOUND){
      ErrPostEx(SEV_INFO, 0, 0, "No data found for gi : %ld",gi);
      SQLFreeStmt(hstmt, SQL_DROP);
      return NULL;
  }
  while(sqlrc == SQL_SUCCESS || sqlrc == SQL_SUCCESS_WITH_INFO){
 
    pasCurrent->gi = gi.val;  
    pasCurrent->type =  (CharPtr) MemNew((size_t) type.len);
    sprintf(pas->type, "%s",IsNullStr(type.val));
    pasCurrent->division =  (CharPtr) MemNew((size_t) division.len);
    sprintf(pasCurrent->division, "%s",IsNullStr(division.val));
    pasCurrent->release = (CharPtr) MemNew((size_t) release.len);
    sprintf(pasCurrent->release, "%s",IsNullStr(release.val));

    /*Get the length of of the blob so that we can read it */
    SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &LocUse);
    if(SQLGetLength(LocUse, SQL_C_BLOB_LOCATOR, blobloc, (long*)&bloblen, (long*)&indicator) != SQL_SUCCESS)
      print_err(hstmt);
      
    if(bloblen > 0){

        /* allocate a buffer big enough to store the blob. */
        pcASN = (CharPtr) MemNew((size_t) bloblen);

       /*read the blob from the database */
       SQLGetData(hstmt,4,SQL_C_BINARY,(SQLPOINTER)pcASN,bloblen,(long*)&indicator);
       if(pcASN != NULL)
         pasCurrent->asn1 =  (BioseqPtr) GetASNMemChar(pcASN,(AsnReadFunc)BioseqAsnRead,bloblen);
      
       if (pcASN) 
	 MemFree(pcASN);
      
      } else{
         ErrPostEx(SEV_INFO, 0, 0, "blob to retrieved from asndb is length 0");
         if(pasCurrent->type) MemFree(pasCurrent->type);
         if(pasCurrent->division) MemFree(pasCurrent->division);
         if(pasCurrent->release) MemFree(pasCurrent->release);
      }
     
    /*Get next record, if there is one add a node to the valnode.*/
    sqlrc = SQLFetch(hstmt);
    if(sqlrc == SQL_SUCCESS || sqlrc == SQL_SUCCESS_WITH_INFO){
      pasCurrent->next = StAsndbNew();
      pasCurrent = pasCurrent->next;
    }
  }

       if (SQLFreeHandle(SQL_HANDLE_STMT, LocUse) != SQL_SUCCESS){
          print_err(hstmt);
          return NULL;
       }

       if (SQLFreeHandle(SQL_HANDLE_STMT, hstmt) != SQL_SUCCESS){
          print_err(hstmt);
          return NULL;
       }
     
     return pas;

}

Int2 LIBCALL SearchASNDB(StAsndbPtr PNTR ppas)
{
   StAsndbPtr pas = NULL;
   Int4 gi=0,bloblen=0,indicator=0;
   CharPtr search_asndb = "select division,release,type,asn1 from seqhound.asndb where gi=?";
   SQLHANDLE hstmt,LocUse;
   Int2 sqlrc=SQL_SUCCESS;
   SQLINTEGER blobloc=0, pbbvalue=0;

   CharPtr pcASN = NULL;
   struct{
     Int4 len;
     Char val[15];
   }division, release, type;

   /* SK: NULL ptr check */
   if (ppas == NULL || *ppas == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "SearchASNDB: StAsndbPtr is NULL.");
       return 0;
    }
   pas = *ppas;
   if (pas->gi <= 0)
    {
       ErrPostEx(SEV_ERROR,0,0, "SearchASNDB: No allowed search key (GI).");
       return 0;
    }

   /* search ASNDB */
   CreateHandle(&hstmt,search_asndb,&henv,&hdbc);

  SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_LONG,SQL_INTEGER, 0, 0, &gi, 0, NULL);

  gi = pas->gi;

  /* set auto commit on */
   SQLSetConnectAttr( hdbc, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_ON, SQL_NTS);  

  /* execute statment */
  if (SQLExecute(hstmt) != SQL_SUCCESS){
	ErrPostEx(SEV_ERROR,0,0,"SearchASNDB:unable to execute select statement.");
	print_err(hstmt);
	SQLFreeStmt(hstmt, SQL_DROP);
	return FALSE;
  }

  SQLBindCol(hstmt,1,SQL_C_CHAR,division.val,10,(long*)&division.len);  
  SQLBindCol(hstmt,2,SQL_C_CHAR,release.val,10,(long*)&release.len);
  SQLBindCol(hstmt,3,SQL_C_CHAR,type.val,15,(long*)&type.len);
  SQLBindCol(hstmt,4, SQL_C_BLOB_LOCATOR,&blobloc,0,(long*)&pbbvalue);
  
  sqlrc = SQLFetch(hstmt);
  if(sqlrc == SQL_NO_DATA_FOUND){
      ErrPostEx(SEV_INFO, 0, 0, "SearchASNDB: No data found for gi : %ld",gi);
      SQLFreeStmt(hstmt, SQL_DROP);
      return -1;
  }
  else if(sqlrc == SQL_SUCCESS || sqlrc == SQL_SUCCESS_WITH_INFO){
      pas->type = StringSave(type.val);
      pas->division = StringSave(division.val);
      pas->release = StringSave(release.val);

      /*Get the length of of the blob so that we can read it */
      SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &LocUse);
      if(SQLGetLength(LocUse, SQL_C_BLOB_LOCATOR, blobloc, (long*)&bloblen, (long*)&indicator) != SQL_SUCCESS)
	print_err(hstmt);
      
      if(bloblen > 0){

        /* allocate a buffer big enough to store the blob. */
        pcASN = (CharPtr) MemNew((size_t) bloblen);

       /*read the blob from the database */
       SQLGetData(hstmt,4,SQL_C_BINARY,(SQLPOINTER)pcASN,bloblen,(long*)&indicator);
       if(pcASN != NULL)
         pas->asn1 =  (BioseqPtr) GetASNMemChar(pcASN,(AsnReadFunc)BioseqAsnRead,bloblen);
      
       if (pcASN) MemFree(pcASN);
       
      }
      else{
         ErrPostEx(SEV_INFO, 0, 0, "SearchASNDB: ASN1 Blob retrieved has length = 0");
         if(pas->type) MemFree(pas->type);
         if(pas->division) MemFree(pas->division);
         if(pas->release) MemFree(pas->release);
      }

       if (SQLFreeHandle(SQL_HANDLE_STMT, LocUse) != SQL_SUCCESS){
          print_err(hstmt);
          return FALSE;
       }

       if (SQLFreeHandle(SQL_HANDLE_STMT, hstmt) != SQL_SUCCESS){
          print_err(hstmt);
          return FALSE;
       }
     
     return TRUE;

  }
  else{
    print_err(hstmt);
    SQLFreeStmt(hstmt, SQL_DROP);
    return FALSE;
  }

}

/*******************************************************/
/*Given a pointer to the asn structure that represents */
/*  a record in the parti table this function pulls out*/
/*  The gi from the asn structure that we are interested*/ 
/*  in finding out more information and pulls it out of*/
/*  the database and populates the rest of the         */
/*  asn parti structure.                               */
/*Returns : TRUE if successful and FALSE otherwise.    */
/*******************************************************/ 
Int2 LIBCALL SearchPARTI (StPartiPtr PNTR pppa)
{
   StPartiPtr ppa = NULL;
   Int4 gi;
   CharPtr search_parti = "select division from seqhound.parti where gi=?";
   SQLHANDLE hstmt;
   Int2 sqlrc=SQL_SUCCESS;
   struct{
     SQLINTEGER len;
     Char val[10];
   }division;

   /* SK: NULL ptr check */
   if (pppa == NULL || *pppa == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "SearchPARTI: StPartiPtr is NULL.");
       return 0;
    }
   ppa = *pppa;
   if (ppa->gi <= 0)
    {
       ErrPostEx(SEV_ERROR,0,0, "SearchPARTI: No allowed search key (GI).");
       return 0;
    }

   /* search PARTI */
   CreateHandle(&hstmt, search_parti,&henv,&hdbc);

  SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_LONG,SQL_INTEGER, 0, 0, &gi, 0, NULL);

  gi = ppa->gi;

  /* set auto commit on */
   SQLSetConnectAttr( hdbc, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_ON, SQL_NTS);  

  /* execute statment */
  if (SQLExecute(hstmt) != SQL_SUCCESS){
	ErrPostEx(SEV_ERROR,0,0,"SearchPARTI: unable to execute select statement.");
	print_err(hstmt);
	SQLFreeStmt(hstmt, SQL_DROP);
	return FALSE;
  }

  SQLBindCol(hstmt,1,SQL_C_CHAR,division.val,10,(long*)&division.len);  
  
  sqlrc = SQLFetch(hstmt);
  if(sqlrc == SQL_NO_DATA_FOUND){
      ErrPostEx(SEV_INFO, 0, 0, "No data found for gi : %ld",gi);
      SQLFreeStmt(hstmt, SQL_DROP) ;
      return FALSE;
  }
  while(sqlrc == SQL_SUCCESS || sqlrc == SQL_SUCCESS_WITH_INFO){
   ppa->partition = StringSave(division.val);

   /*Get next record, if there is one add a node to the valnode.*/
   sqlrc = SQLFetch(hstmt);
   if(sqlrc == SQL_SUCCESS || sqlrc == SQL_SUCCESS_WITH_INFO){
     ppa->next = StPartiNew();
     ppa = ppa->next;
   }
  }

   /* Free statement handle. */
   if (SQLFreeHandle(SQL_HANDLE_STMT, hstmt) != SQL_SUCCESS){
	print_err(hstmt);
	return FALSE;
   }
   return TRUE;
}


/*******************************************************/
/*Given a pointer to the asn structure that represents */
/*  a nucprot record this function pulls out either the*/
/*  nucleotide gi or the amino acid gi depending on    */
/*  which one is populated.  This function then goes   */
/*  into the database and pulls out all the nucleotide */
/*  protein relations for that gi.                     */
/*Returns : a linked list of the nucprot records that  */
/*  were retrieved from the database.                  */
/*******************************************************/ 
Int2 LIBCALL SearchNUCPROT (StNucprotPtr PNTR ppnc)
{
   StNucprotPtr pnc = NULL;
   Int4 keynum = 0, gi=0;
   CharPtr amino ="SELECT gia,gin from seqhound.nucprot where gia = ?";
   CharPtr nucleo ="SELECT gia, gin from seqhound.nucprot where gin=?";
   CharPtr search_nucprot = NULL;
   SQLHANDLE hstmt;
   Int2 sqlrc=SQL_SUCCESS;
   struct{
     SQLINTEGER len;
     SQLINTEGER val;
   }gia, gin;

   /* SK: NULL ptr check */
   if (ppnc == NULL || *ppnc == NULL)
    {
     ErrPostEx(SEV_ERROR,0,0, "SearchNUCPROT: StNucprotPtr is NULL.");
     return 0;
    }
   pnc = *ppnc;

   if (pnc->gin > 0)
    {
     keynum = pnc->gin;
     search_nucprot = nucleo;
   }
   if (pnc->gia > 0)
    {
     keynum = pnc->gia;
     search_nucprot = amino;
    }
   if (keynum == 0)
    {
     ErrPostEx(SEV_ERROR,0,0, "SearchNUCPROT: No allowed search key (GI).");
     return 0;
    }

   CreateHandle(&hstmt,search_nucprot,&henv,&hdbc);
  
  SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_LONG,SQL_INTEGER, 0, 0, &gi, 0, NULL);

  gi = keynum;

  /* set auto commit on */
  SQLSetConnectAttr( hdbc, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_ON, SQL_NTS);  

  /* execute statment */
  if (SQLExecute(hstmt) != SQL_SUCCESS){
    ErrPostEx(SEV_ERROR,0,0,"SearchNUCPROT: unable to execute select statement.");
    print_err(hstmt);
    SQLFreeStmt(hstmt, SQL_DROP);
    return FALSE;
  }

  SQLBindCol(hstmt,1,SQL_C_LONG,&gia.val,0,(long*)&gia.len);  
  SQLBindCol(hstmt,2,SQL_C_LONG,&gin.val,0,(long*)&gin.len);  
  
  sqlrc = SQLFetch(hstmt);
  if(sqlrc == SQL_NO_DATA_FOUND){
    ErrPostEx(SEV_INFO, 0, 0, "No data found for gi : %ld",gi);
      SQLFreeStmt(hstmt, SQL_DROP);
    return FALSE;
  }
  while(sqlrc == SQL_SUCCESS || sqlrc == SQL_SUCCESS_WITH_INFO){
    pnc->gin = gin.val;
    pnc->gia = gia.val;
    
    /*Get next record, if there is one add a node to the valnode.*/
   sqlrc = SQLFetch(hstmt);
   if(sqlrc == SQL_SUCCESS || sqlrc == SQL_SUCCESS_WITH_INFO){
    pnc->next = StNucprotNew();
    pnc= pnc->next;
   }
  }

   /* Free statement handle. */
   if (SQLFreeHandle(SQL_HANDLE_STMT, hstmt) != SQL_SUCCESS){
     print_err(hstmt);
     return FALSE;
   }
   return TRUE;
 
}


/*******************************************************/
/*Given a pointer to the asn structure that represents */
/*  an accdb record this function pulls out the        */
/*  accession, name, or gi from the asn structure and  */
/*  retrieves all the records associated with that key */
/*Returns : TRUE if successful and FALSE otherwise.    */
/*******************************************************/

Int2 LIBCALL SearchACCDB (StAccdbPtr PNTR ppac)
{
   StAccdbPtr pac = NULL;
   Int4 keynum = 0, length;
   CharPtr access = "SELECT gi,db,name,access,chain,release,version,title from seqhound.accdb where access=?";
   CharPtr name_stmt = "SELECT gi,db,name,access,chain,release,version,title from seqhound.accdb where name=?";
   CharPtr gi_stmt = "SELECT gi,db,name,access,chain,release,version,title from seqhound.accdb where gi=?";
   CharPtr key = NULL;
   SQLHANDLE hstmt=0;
   Int2 sqlrc=SQL_SUCCESS;
   struct{
     SQLINTEGER len;
     SQLINTEGER val;
   }gi, version;
   struct{
     SQLINTEGER len;
     Char val[30];
   }accession,name,db,chain,release;
   struct{
     SQLINTEGER len;
     Char val[1024];
   }title;

   /* SK: NULL ptr check */
   if (ppac == NULL || *ppac == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "SearchACCDB: StAccdbPtr is NULL.");
       return 0;
    }
   pac = *ppac;
   if ((pac->gi <= 0) && (pac->access == NULL) && (pac->namelow == NULL))
    {
       ErrPostEx(SEV_ERROR,0,0, "SearchACCDB: No allowed search key.");
       return 0;
    }

   if (pac->gi > 0)
    {
      CreateHandle(&hstmt,gi_stmt,&henv,&hdbc);
      SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_LONG,SQL_INTEGER, 0, 0, &keynum, 0, NULL);
      keynum=(Int4)pac->gi;
    }
    
   else if (pac->access != NULL)
    {
      key = StrUpper(pac->access);
      length = strlen(key);
      CreateHandle(&hstmt, access,&henv,&hdbc);
      SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_DEFAULT,SQL_VARCHAR, 15, 0, key, 0, (long*)&length);
     }

   else if (pac->namelow != NULL)
    {
     key = StrUpper(pac->namelow);
      length = strlen(key);
      CreateHandle(&hstmt, name_stmt,&henv, &hdbc);
      SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_DEFAULT,SQL_VARCHAR, 30, 0, key, 0, (long*)&length);
    }

   if (key == NULL && keynum == 0)
    {
       ErrPostEx(SEV_ERROR,0,0, "SearchACCDB: No allowed search key.");
       return 0;
    }

  /* set auto commit on */
  SQLSetConnectAttr( hdbc, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_ON, SQL_NTS);  

  /* execute statment */
  if (SQLExecute(hstmt) != SQL_SUCCESS){
    ErrPostEx(SEV_ERROR,0,0,"SearchACCDB:unable to execute select statement.");
    print_err(hstmt);
    SQLFreeStmt(hstmt, SQL_DROP);
    return FALSE;
  }

  SQLBindCol(hstmt,1,SQL_C_LONG,&gi.val,0,(long*)&gi.len);  
  SQLBindCol(hstmt,2,SQL_C_CHAR,db.val,15,(long*)&db.len);  
  SQLBindCol(hstmt,3,SQL_C_CHAR,name.val,30,(long*)&name.len);  
  SQLBindCol(hstmt,4,SQL_C_CHAR,accession.val,20,(long*)&accession.len);  
  SQLBindCol(hstmt,5,SQL_C_CHAR,chain.val,10,(long*)&chain.len);
  SQLBindCol(hstmt,6,SQL_C_CHAR,release.val,20,(long*)&release.len);
  SQLBindCol(hstmt,7,SQL_C_LONG,&version.val,0,(long*)&version.len);  
  SQLBindCol(hstmt,8,SQL_C_CHAR,title.val,1024,(long*)&title.len);

    sqlrc = SQLFetch(hstmt);
  if(sqlrc == SQL_NO_DATA_FOUND){
    SQLFreeStmt(hstmt, SQL_DROP);
    ErrPostEx(SEV_INFO, 0, 0, "No data found for gi : %ld, accession : %s",keynum, IsNullStr(key));
      SQLFreeStmt(hstmt, SQL_DROP) ;
    return -1;
  }
  
  while(sqlrc == SQL_SUCCESS || sqlrc == SQL_SUCCESS_WITH_INFO){
    pac->gi = gi.val;
    pac->db = StringSave(db.val);
    pac->name = StringSave(name.val);
    pac->namelow = StrLower(StringSave(name.val));
    pac->access = StringSave(accession.val);
    pac->chain = StringSave(chain.val);
    pac->release = StringSave(release.val);
    pac->version = version.val;
    if(title.len >0 )
      pac->title = StringSave(title.val);
    else{
      pac->title = NULL;
    }

    /*Get next record, if there is one add a node to the valnode.*/
    sqlrc = SQLFetch(hstmt);
    if(sqlrc == SQL_SUCCESS || sqlrc == SQL_SUCCESS_WITH_INFO){
     pac->next = StAccdbNew();
     pac = pac->next;  
     }
    }
  

   if(SQLFreeStmt(hstmt, SQL_DROP) != SQL_SUCCESS){
     print_err(hstmt);
     return FALSE;
   }
 
   return TRUE;
}

CharPtr LIBCALL  GetTitlefromACCDB(void)
{
	return NULL;
}


/*******************************************************/
/*Given a pointer to the asn structure that represents */
/*  an pubseq record this function pulls out the       */
/*  gi, or muid from the asn structure and             */
/*  retrieves all the records associated with that key */
/*Returns : TRUE if successful and FALSE otherwise.    */
/*******************************************************/

Int2  LIBCALL SearchPUBSEQ (StPubseqPtr PNTR ppps)
{
   StPubseqPtr pps= NULL;
   Int4 keynum = 0;
   CharPtr gi_stmt ="SELECT gi, muid from seqhound.pubseq where gi= ?";
   CharPtr muid_stmt ="SELECT gi, muid from seqhound.pubseq where muid=?";
   CharPtr search_pubseq = NULL;
   SQLHANDLE hstmt;
   Int2 sqlrc=SQL_SUCCESS;
   struct{
     SQLINTEGER len;
     SQLINTEGER val;
   }gi, muid;

   /* SK: NULL ptr check */
   if (ppps == NULL || *ppps == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "SearchPUBSEQ: StPubseqPtr is NULL.");
       return 0;
    }
   pps = *ppps;
   if (pps->gi > 0)
    {
      keynum = pps->gi;
      search_pubseq = gi_stmt;
    }
   if (pps->muid > 0)
    {
      keynum = pps->muid;
      search_pubseq = muid_stmt;
    }
   if (keynum == 0)
    {
       ErrPostEx(SEV_ERROR,0,0, "SearchPUBSEQ: No allowed search key.");
       return 0;
    }

   CreateHandle(&hstmt,search_pubseq,&henv,&hdbc);
  
  SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_LONG,SQL_INTEGER, 0, 0, &keynum, 0, NULL);

  /* set auto commit on */
  SQLSetConnectAttr( hdbc, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_ON, SQL_NTS);  

  /* execute statment */
  if (SQLExecute(hstmt) != SQL_SUCCESS){
    ErrPostEx(SEV_ERROR,0,0,"SearchPUBSEQ: unable to execute select statement.");
    print_err(hstmt);
    SQLFreeStmt(hstmt, SQL_DROP);
    return FALSE;
  }

  SQLBindCol(hstmt,1,SQL_C_LONG,&gi.val,0,&gi.len);  
  SQLBindCol(hstmt,2,SQL_C_LONG,&muid.val,0,&muid.len);  
  
  sqlrc = SQLFetch(hstmt);
  if(sqlrc == SQL_NO_DATA_FOUND){
    ErrPostEx(SEV_INFO, 0, 0, "No data found for gi : %ld",gi);
      SQLFreeStmt(hstmt, SQL_DROP);
    return FALSE;
  }

  while(sqlrc == SQL_SUCCESS || sqlrc == SQL_SUCCESS_WITH_INFO){
    pps->gi = gi.val;
    pps->muid = muid.val; 
    sqlrc = SQLFetch(hstmt);
    if(sqlrc == SQL_SUCCESS || sqlrc == SQL_SUCCESS_WITH_INFO){
     pps->next = StPubseqNew();
     pps = pps->next;
     }
  }

 if(SQLFreeStmt(hstmt, SQL_DROP) != SQL_SUCCESS){
     print_err(hstmt);
     return FALSE;
   }
 
  
   return TRUE;
  
}


/*******************************************************/
/*Given a pointer to the asn structure that represents */
/*  a  taxgi record this function pulls out the        */
/*  taxid, kloodge, or gi from the asn structure and  */
/*  retrieves all the records associated with that key */
/*Returns : TRUE if successful and FALSE otherwise.    */
/*******************************************************/
Int2 LIBCALL SearchTAXGI (StTaxgiPtr PNTR pptg)
{
   StTaxgiPtr ptg = NULL;
   Int4 keynum = 0;
   CharPtr gi_stmt ="SELECT gi,taxid, kloodge,type from seqhound.taxgi where gi= ?";
   CharPtr taxid_stmt ="SELECT gi, taxid, kloodge,type from seqhound.taxgi where taxid=?";
   CharPtr kloodge_stmt ="SELECT gi, taxid, kloodge,type from seqhound.taxgi where kloodge= ?";
   CharPtr search_taxgi = NULL;
   SQLHANDLE hstmt;
   Int2 sqlrc=SQL_SUCCESS;
   struct{
     SQLINTEGER len;
     SQLINTEGER val;
   }gi, taxid, kloodge;
    struct{
     SQLINTEGER len;
     Char val[20];
    }type;

   /* SK: NULL ptr check */
   if (pptg == NULL || *pptg == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "SearchTAXGI: StTaxgiPtr is NULL.");
       return FALSE;
    }
   ptg = *pptg;
   if ((ptg->gi <= 0) && (ptg->kloodge <= 0) && (ptg->taxid <= 0))
    {
       ErrPostEx(SEV_ERROR,0,0, "SearchTAXGI: No allowed search key.");
       return FALSE;
    }
   if (ptg->gi > 0)
    {
      keynum = ptg->gi;
      search_taxgi = gi_stmt;
    }
   if (ptg->kloodge > 0)
    {
      keynum = ptg->kloodge;
      search_taxgi = kloodge_stmt;
    }
   if (ptg->taxid > 0)
    {
      keynum = ptg->taxid;
      search_taxgi = taxid_stmt;
    }
   if (keynum == 0)
    {
       ErrPostEx(SEV_ERROR,0,0, "SearchTAXGI: No allowed search key.");
       return FALSE;
    }

/*  if(!connected)
    ConnectToDb();*/

  CreateHandle(&hstmt,search_taxgi,&henv,&hdbc);
  
  SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_LONG,SQL_INTEGER, 0, 0, &keynum, 0, NULL);
 
  /* set auto commit on */
  SQLSetConnectAttr( hdbc, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_ON, SQL_NTS);  
 
  /* execute statment */
  if (SQLExecute(hstmt) != SQL_SUCCESS){
    ErrPostEx(SEV_ERROR,0,0,"SearchTAXGI:unable to execute select statement.");
    print_err(hstmt);
    SQLFreeStmt(hstmt, SQL_DROP);
    return FALSE;
  }

  SQLBindCol(hstmt,1,SQL_C_LONG,&gi.val,0,&gi.len);  
  SQLBindCol(hstmt,2,SQL_C_LONG,&taxid.val,0,&taxid.len);  
  SQLBindCol(hstmt,3,SQL_C_LONG,&kloodge.val,0,&kloodge.len); 
  SQLBindCol(hstmt,4,SQL_C_CHAR,type.val,20,&type.len);  

  sqlrc = SQLFetch(hstmt);
  if(sqlrc == SQL_NO_DATA_FOUND){
    ErrPostEx(SEV_INFO, 0, 0, "SearchTAXGI No data found for keynum : %ld",keynum);
    SQLFreeStmt(hstmt, SQL_DROP);
    return FALSE;
  }
  while(sqlrc == SQL_SUCCESS || sqlrc == SQL_SUCCESS_WITH_INFO){
    ptg->gi = gi.val;
    ptg->taxid = taxid.val;
    ptg->kloodge = kloodge.val;
    ptg->type = StringSave(type.val);

    sqlrc = SQLFetch(hstmt);
    if(sqlrc == SQL_SUCCESS || sqlrc == SQL_SUCCESS_WITH_INFO){
     ptg->next = StTaxgiNew();
     ptg= ptg->next;
     }
  }
 
  if(SQLFreeStmt(hstmt, SQL_DROP) != SQL_SUCCESS){
     ErrPostEx(SEV_INFO, 0 ,0, "SearchTaxgi : unable to free statement.");
     print_err(hstmt);
     return FALSE;
   }
   
   return TRUE;
  
}


ValNodePtr LIBCALL SearchTAXGIProt(Int4 taxid)
{
   Int4 keynum = 0;
   CharPtr search_taxgi ="SELECT gi from seqhound.taxgi where taxid=? and type = \'protein\'";
   SQLHANDLE hstmt;
   Int2 sqlrc=SQL_SUCCESS;
   ValNodePtr vnp=NULL;

   struct{
     SQLINTEGER len;
     SQLINTEGER val;
   }gi;

   if (taxid <= 0)
    {
       ErrPostEx(SEV_ERROR,0,0, "SearchTAXGIProt: No allowed search key.");
       return NULL;
    }

  CreateHandle(&hstmt,search_taxgi,&henv,&hdbc);
  
  SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_LONG,SQL_INTEGER, 0, 0, &taxid, 0, NULL);
 
  /* set auto commit on */
  SQLSetConnectAttr( hdbc, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_ON, SQL_NTS);  
 
  /* execute statment */
  if (SQLExecute(hstmt) != SQL_SUCCESS){
    ErrPostEx(SEV_ERROR,0,0,"SearchTAXGIProt: unable to execute select statement.");
    print_err(hstmt);
    SQLFreeStmt(hstmt, SQL_DROP);
    return NULL;
  }

  SQLBindCol(hstmt,1,SQL_C_LONG,&gi.val,0,&gi.len);  

  sqlrc = SQLFetch(hstmt);
  if(sqlrc == SQL_NO_DATA_FOUND){
    ErrPostEx(SEV_INFO, 0, 0, "SearchTAXGIProt No data found for keynum : %ld",keynum);
      SQLFreeStmt(hstmt, SQL_DROP);
    return NULL;
  }
  while(sqlrc == SQL_SUCCESS || sqlrc == SQL_SUCCESS_WITH_INFO){
    ValNodeAddInt(&vnp,1,gi.val);
    sqlrc = SQLFetch(hstmt);
  }
 
  if(SQLFreeStmt(hstmt, SQL_DROP) != SQL_SUCCESS){
     ErrPostEx(SEV_INFO, 0 ,0, "SearchTAXGIProt : unable to free statement.");
     print_err(hstmt);
     return NULL;
   }

   return vnp;
}


ValNodePtr LIBCALL SearchTAXGIDNA(Int4 taxid)
{
   Int4 keynum = 0;
   CharPtr search_taxgi ="SELECT gi from seqhound.taxgi where taxid=? and type = \'DNA\'";
   SQLHANDLE hstmt;
   Int2 sqlrc=SQL_SUCCESS;
   ValNodePtr vnp=NULL;

   struct{
     SQLINTEGER len;
     SQLINTEGER val;
   }gi;

   if (taxid <= 0)
    {
       ErrPostEx(SEV_ERROR,0,0, "SearchTAXGIDNA: No allowed search key.");
       return NULL;
    }

  CreateHandle(&hstmt,search_taxgi,&henv,&hdbc);
  
  SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_LONG,SQL_INTEGER, 0, 0, &taxid, 0, NULL);
 
  /* set auto commit on */
  SQLSetConnectAttr( hdbc, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_ON, SQL_NTS);  
 
  /* execute statment */
  if (SQLExecute(hstmt) != SQL_SUCCESS){
    ErrPostEx(SEV_ERROR,0,0,"SearchTAXGIDNA: unable to execute select statement.");
    print_err(hstmt);
    SQLFreeStmt(hstmt, SQL_DROP);
    return NULL;
  }

  SQLBindCol(hstmt,1,SQL_C_LONG,&gi.val,0,&gi.len);  

  sqlrc = SQLFetch(hstmt);
  if(sqlrc == SQL_NO_DATA_FOUND){
    ErrPostEx(SEV_INFO, 0, 0, "SearchTAXGIDNA No data found for keynum : %ld",keynum);
     SQLFreeStmt(hstmt, SQL_DROP);
    return NULL;
  }
  while(sqlrc == SQL_SUCCESS || sqlrc == SQL_SUCCESS_WITH_INFO){
    ValNodeAddInt(&vnp,1,gi.val);
    sqlrc = SQLFetch(hstmt);
  }
 
  if(SQLFreeStmt(hstmt, SQL_DROP) != SQL_SUCCESS){
     ErrPostEx(SEV_INFO, 0 ,0, "SearchTAXGIDNA : unable to free statement.");
     print_err(hstmt);
     return NULL;
   }

   return vnp;
}


static Boolean LIBCALL ExpandSeqEntry(SeqEntryPtr PNTR ppse)
{
   BioseqSetPtr pbss = NULL;
   SeqEntryPtr pseNew = NULL, pse = NULL;
   BioseqPtr pbsfull = NULL, pbs = NULL;
   Int4 gival = 0;

   /* SK: NULL ptr check */
   if ( ppse == NULL || *ppse == NULL )
   {
      ErrPostEx(SEV_ERROR,0,0,"ExpandSeqEntry: NULL ppse.");
      return FALSE;
   }

   pse = *ppse;
   if (pse->choice == 1) /* It is a Bioseq */
    {
      pbs = (BioseqPtr) pse->data.ptrvalue;
      if (pbs == NULL)
       {
         ErrPostEx(SEV_ERROR,0,0,"ExpandSeqEntry: Null pointer to Bioseq.");
         return FALSE;
       }

      if ((gival = GetGI(pbs)) <= 0)
       {
         ErrPostEx(SEV_ERROR,0,0,"ExpandSeqEntry: Invalid GI.");
         return FALSE;
       }
      /* get the full bioseq */
      pbsfull = SHoundGetBioseq(gival);
      pse->data.ptrvalue = pbsfull;
      /* free the original bioseq*/
      if(pbs)BioseqFree(pbs);

    }
   else if (pse->choice == 2) /* it is nested Bioseq-set */
    {

         pbss = (BioseqSetPtr) pse->data.ptrvalue;
         if (pbss == NULL)
           {
             ErrPostEx(SEV_ERROR,0,0,"ExpandSeqEntry: Null pointer to Bioseq-Set.");
             return FALSE;
           }

         pseNew = pbss->seq_set;
         while (pseNew != NULL)
            {
             if(!ExpandSeqEntry(&pseNew))
               {
                ErrPostEx(SEV_ERROR,0,0,"ExpandSeqEntry: The recursive call to ToBiseqAs failed");
                return FALSE;
               }
              pseNew = pseNew->next;
            }
    }
   else
    {
         ErrPostEx(SEV_ERROR,0,0,"ExpandSeqEntry: Invalid choice field.");
         return FALSE;
    }
   return TRUE;
}




SeqEntryPtr LIBCALL GetSeqEntry(CharPtr pcASN, Int4 iSize)
{
   AsnIoMemPtr aimp;
   SeqEntryPtr pse = NULL;
   
   if(pcASN == NULL)
     return NULL;

    if(iSize<=0){
   	/*post error message*/
   	ErrPostEx(SEV_ERROR, 0,0, "GetSeqEntry:returned an empty asn.1 record.");
   	return NULL;
   }

   aimp = AsnIoMemOpen("rb", (BytePtr) pcASN, iSize);
   if (aimp == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "GetSeqEntry: Failed to get AsnIo.");
      if (pcASN) MemFree(pcASN);
      return NULL;
    }
   pse = SeqEntryAsnRead(aimp->aip, NULL);
   if (pse == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "GetSeqEntry: Bioseq pointer is NULL.");
      if (pcASN) MemFree(pcASN);
      AsnIoMemClose(aimp);
      return NULL;
    }

   AsnIoFlush(aimp->aip);
   AsnIoMemClose(aimp);

   if(!ExpandSeqEntry(&pse))
    {
      ErrPostEx(SEV_ERROR,0,0, "GetSeqEntry: ExpandSeqEntry failed.");
      return NULL;
    }
   return pse;
}




Int2 LIBCALL SearchSENDB(StSendbPtr PNTR ppsd)
{
   StSendbPtr psd = NULL;
   Int4 keynum = 0;
   CharPtr search_sendb ="SELECT seid,asn1 FROM seqhound.sendb where seid=?";
   SQLHANDLE hstmt=0;
   Int4 bloblen,indicator;
   Int2 sqlrc=SQL_SUCCESS;
   SQLINTEGER blobloc, pbbvalue;
   SQLHANDLE LocUse=0;
   CharPtr pcASN = NULL;
  struct{
     SQLINTEGER len;
     SQLINTEGER val;
   }seid;

   /* SK: NULL ptr check */
   if (ppsd == NULL || *ppsd == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "SearchSENDB: StSendbPtr is NULL.");
       return 0;
    }
   psd = *ppsd;
   if (psd->seid <= 0)
    {
       ErrPostEx(SEV_ERROR,0,0, "SearchSENDB: No allowed search key (SEID).");
       return 0;
    }

   keynum = psd->seid;

  CreateHandle(&hstmt,search_sendb,&henv,&hdbc);
  
  SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_LONG,SQL_INTEGER, 0, 0, &keynum, 0, NULL);

  /* set auto commit on */
  SQLSetConnectAttr( hdbc, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_ON, SQL_NTS);  

  /* execute statment */
  if (SQLExecute(hstmt) != SQL_SUCCESS){
    ErrPostEx(SEV_ERROR,0,0,"SearchSENDB: unable to execute select statement.");
    print_err(hstmt);
    SQLFreeStmt(hstmt, SQL_DROP);
    return FALSE;
  }

  SQLBindCol(hstmt,1,SQL_C_LONG,&seid.val,0,&seid.len);  
  SQLBindCol(hstmt,2, SQL_C_BLOB_LOCATOR,&blobloc,0,&pbbvalue);

  sqlrc = SQLFetch(hstmt);
  if(sqlrc == SQL_NO_DATA_FOUND){
    ErrPostEx(SEV_INFO, 0, 0, "No data found for seid : %ld",keynum);
    SQLFreeStmt(hstmt, SQL_DROP);
    return FALSE;
  }
  while(sqlrc == SQL_SUCCESS || sqlrc == SQL_SUCCESS_WITH_INFO){
     /*Get the length of of the blob so that we can read it */
    SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &LocUse);
    if(SQLGetLength(LocUse, SQL_C_BLOB_LOCATOR, blobloc, (long*)&bloblen, (long*)&indicator)!= SQL_SUCCESS)
      print_err(hstmt);
     
    if(bloblen > 0){
      /* allocate a buffer big enough to store the blob. */ 
      pcASN = (CharPtr) MemNew((size_t) bloblen);
      /*read the blob from the database */
      if(SQLGetData(hstmt,2,SQL_C_BINARY,(SQLPOINTER)pcASN,bloblen,(long*)&indicator) != SQL_SUCCESS)
	print_err(hstmt);

      psd->seid = seid.val;
      if(pcASN != NULL)   
        psd->asn1 = GetSeqEntry(pcASN, bloblen);
   
      sqlrc = SQLFetch(hstmt);
      if(sqlrc == SQL_SUCCESS || sqlrc == SQL_SUCCESS_WITH_INFO){
       psd->next = StSendbNew();
       psd = psd->next;
      }
    }
  }

  if (SQLFreeHandle(SQL_HANDLE_STMT, LocUse) != SQL_SUCCESS){
    print_err(hstmt);
    return FALSE;
  }

  if(SQLFreeStmt(hstmt, SQL_DROP) != SQL_SUCCESS){
     print_err(hstmt);
     return FALSE;
   }

   return TRUE;
}


SeqEntryPtr LIBCALL GetSeqEntryfromSENDB(void)
{
  return NULL;
}


/* no compression version of the function */

/*BiostrucPtr  LIBCALL GetBiostrucfromMMDB(CharPtr pcASN, Int4 iSize)
{
  int rtn = 0;
  BiostrucPtr pbstr = NULL;
  AsnIoMemPtr aimp;
  Int2 err = 0;

   aimp = AsnIoMemOpen("rb",(BytePtr) pcASN, iSize);
   if (aimp == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "GetBiostrucfromMMDB: Failed to get AsnIoMem.");
      if (pcASN) MemFree(pcASN);
      return NULL;
    }

   pbstr = BiostrucAsnRead(aimp->aip, NULL);
   if (pbstr == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "GetBiostrucfromMMDB: Bioseq pointer is NULL.");
      if (pcASN) MemFree(pcASN);
      AsnIoMemClose(aimp);
      return NULL;
    }

   AsnIoFlush(aimp->aip);
   AsnIoMemClose(aimp);
   if (pcASN) MemFree(pcASN);
   return pbstr;
}
*/


BiostrucPtr  LIBCALL GetBiostrucfromMMDB(Int4 mdlLvl, Int4 maxModels)
{
/*  unsigned int iSize = 0;
  unsigned int iSizebz = 0;
  CharPtr pcASN = NULL;
  BiostrucPtr pbstr = NULL;
  AsnIoMemPtr aimp;

   iSizebz = f4memoLen(pf4mdAsn1);
   iSize =  f4long(pf4mdSize);
   pcASN = (CharPtr) MemNew((size_t) iSize + 1);
   if (pcASN == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "GetBiostrucfromMMDB: Failed to allocate memory.");
      return NULL;
    }
   if (BZ2_bzBuffToBuffDecompress(pcASN, &iSize, f4memoPtr(pf4mdAsn1),iSizebz,0,0)!=BZ_OK)
    {
       ErrPostEx(SEV_ERROR,0,0, "GetBiostrucfromMMDB: bzBuffToBuffDecompress failed.");
       if (pcASN) MemFree(pcASN);
       return NULL;
    }
   aimp = AsnIoMemOpen("rb",(BytePtr) pcASN, iSize);
   if (aimp == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "GetBiostrucfromMMDB: Failed to get AsnIoMem.");
      if (pcASN) MemFree(pcASN);
      return NULL;
    }


   pbstr = BiostrucAsnGet(aimp->aip, NULL, mdlLvl, maxModels);
   if (pbstr == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "GetBiostrucfromMMDB: Biostruc pointer is NULL.");
      if (pcASN) MemFree(pcASN);
      AsnIoMemClose(aimp);
      return NULL;
    }

   AsnIoFlush(aimp->aip);
   AsnIoMemClose(aimp);
   if (pcASN) MemFree(pcASN);
   return pbstr;*/
	return NULL;
}


/*******************************************************/
/*Given a pointer to the asn structure that represents */
/*  an mmdb record this function pulls out the         */
/*  mmdbid or pdbid from the asn structure and         */
/*  retrieves all the records associated with that key */
/*Returns : TRUE if successful and FALSE otherwise.    */
/*******************************************************/

Int2 LIBCALL SearchMMDB (StMmdbPtr PNTR ppmd)
{
   StMmdbPtr pmd= NULL;
   CharPtr search_mmdbid = "SELECT mmdbid,asn1,pdbid,bwhat, models,molecules,size,bzsize from seqhound.mmdb where mmdbid = ?";
   CharPtr search_pdbid = "SELECT mmdbid, asn1,pdbid,bwhat,modles,molecules,size,bzsize from seqhound.mmdb where pdbid=?";
   Int4 keynum = 0,length;
   Int4 bloblen,indicator;
   SQLHANDLE hstmt=0;
   Int2 sqlrc=SQL_SUCCESS;
   SQLINTEGER blobloc, pbbvalue;
   SQLHANDLE LocUse=0;
   CharPtr pcASN = NULL,key= NULL;
   struct{
     Int4 len;
     Char val[15];
   }pdbid;
   struct{
     SQLINTEGER len;
     SQLINTEGER val;
   }mmdbid,bwhat,models,molecules,size,bzsize;

  /* SK: NULL ptr check */
  if (ppmd == NULL || *ppmd == NULL)
  {
    ErrPostEx(SEV_ERROR,0,0, "SearchMMDB: StMmdbPtr is NULL.");
    return 0;
    }
   pmd = *ppmd;

   /* search according to mmdbid,  set corresponding statement.*/
   if (pmd->mmdbid > 0){
     CreateHandle(&hstmt,search_mmdbid,&henv,&hdbc);
     SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_LONG,SQL_INTEGER, 0, 0, &keynum, 0, NULL);
     keynum = pmd->mmdbid;
    }
   if (pmd->pdbid != NULL){
     CreateHandle(&hstmt, search_pdbid,&henv,&hdbc);
     SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_DEFAULT,SQL_CHAR, 20, 0, key, 0,(long*) &length);
     key = pmd->pdbid;
     length = strlen(key);
    }

  /* set auto commit on */
   SQLSetConnectAttr( hdbc, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_ON, SQL_NTS);  

  /* execute statment */
  if (SQLExecute(hstmt) != SQL_SUCCESS){
    ErrPostEx(SEV_ERROR,0,0,"SearchMMDB: unable to execute select statement.");
    print_err(hstmt);
    SQLFreeStmt(hstmt, SQL_DROP);
    return FALSE;
  }

  SQLBindCol(hstmt,1,SQL_C_LONG,&mmdbid.val,0,&mmdbid.len); 
  SQLBindCol(hstmt,2, SQL_C_BLOB_LOCATOR,&blobloc,0,&pbbvalue); 
  SQLBindCol(hstmt,3,SQL_C_CHAR,pdbid.val,20,(long*)&pdbid.len);
  SQLBindCol(hstmt,4,SQL_C_LONG,&bwhat.val,0,&bwhat.len); 
  SQLBindCol(hstmt,5,SQL_C_LONG,&models.val,0,&models.len); 
  SQLBindCol(hstmt,6,SQL_C_LONG,&molecules.val,0,&molecules.len); 
  SQLBindCol(hstmt,7,SQL_C_LONG,&size.val,0,&size.len); 
  SQLBindCol(hstmt,8,SQL_C_LONG,&bzsize.val,0,&bzsize.len); 
 
  sqlrc = SQLFetch(hstmt);
  if(sqlrc == SQL_NO_DATA_FOUND){
      ErrPostEx(SEV_INFO, 0, 0, "No data found for mmdbid or pdbid");
        SQLFreeStmt(hstmt, SQL_DROP) ;
      return FALSE;
  }
  while(sqlrc == SQL_SUCCESS || sqlrc == SQL_SUCCESS_WITH_INFO){
    pmd->mmdbid = mmdbid.val;
    pmd->pdbid = StringSave(pdbid.val);
    pmd->bwhat =  bwhat.val;
    pmd->models =  models.val;
    pmd->molecules = molecules.val;
    pmd->size = size.val;
    pmd->bzsize  = bzsize.val;
    
    /*Get the length of of the blob so that we can read it */
    SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &LocUse);
    SQLGetLength(LocUse, SQL_C_BLOB_LOCATOR, blobloc, (long*)&bloblen, (long*)&indicator);

    /* allocate a buffer big enough to store the blob. */ 
    pcASN = (CharPtr) MemNew((size_t) bloblen);
    /*read the blob from the database */
    SQLGetData(hstmt,2,SQL_C_BINARY,(SQLPOINTER)pcASN,bloblen,(long*)&indicator);

    pmd->asn1 = (BiostrucPtr)GetASNBZMemChar(pcASN,(AsnReadFunc)BiostrucAsnRead,pmd->bzsize,pmd->size);

    if (pcASN) MemFree(pcASN);
    sqlrc = SQLFetch(hstmt);
    if(sqlrc==SQL_SUCCESS || sqlrc == SQL_SUCCESS_WITH_INFO){
      pmd->next = StMmdbNew();
      pmd = pmd->next;
    }
  }
 
 if (SQLFreeHandle(SQL_HANDLE_STMT, LocUse) != SQL_SUCCESS){
    print_err(hstmt);
    return FALSE;
  }

   if(SQLFreeStmt(hstmt, SQL_DROP) != SQL_SUCCESS){
     print_err(hstmt);
     return FALSE;
   }
   return TRUE;  

}

Int2 LIBCALL SearchMMGI (StMmgiPtr PNTR ppmg)
{
   StMmgiPtr pmg= NULL;
   Int4 keynum = 0;
   CharPtr gi_stmt ="SELECT gi, mmdbid from seqhound.mmgi where gi= ?";
   CharPtr mmdbid_stmt ="SELECT gi, mmdbid from seqhound.mmgi where mmdbid=?";
   CharPtr search_mmgi = NULL;
   SQLHANDLE hstmt;
   Int2 sqlrc=SQL_SUCCESS;
   struct{
     SQLINTEGER len;
     SQLINTEGER val;
   }gi, mmdbid;

    /* SK: NULL ptr check */
    if (ppmg == NULL || *ppmg == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "SearchMMGI: StMmgiPtr is NULL.");
       return 0;
    }
   pmg = *ppmg;
   if (pmg->mmdbid > 0)
    {
      keynum = pmg->mmdbid;
      search_mmgi = mmdbid_stmt;
    }
   if (pmg->gi > 0)
    {
      keynum = pmg->gi;
      search_mmgi = gi_stmt;
    }
   if (keynum == 0)
    {
       ErrPostEx(SEV_ERROR,0,0, "SearchMMGI: No allowed search key.");
       return 0;
    }

   CreateHandle(&hstmt,search_mmgi,&henv,&hdbc);
  
  SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_LONG,SQL_INTEGER, 0, 0, &keynum, 0, NULL);

  /* set auto commit on */
  SQLSetConnectAttr( hdbc, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_ON, SQL_NTS);  

  /* execute statment */
  if (SQLExecute(hstmt) != SQL_SUCCESS){
    ErrPostEx(SEV_ERROR,0,0,"SearchMMGI:unable to execute select statement.");
    print_err(hstmt);
    SQLFreeStmt(hstmt, SQL_DROP);
    return FALSE;
  }

  SQLBindCol(hstmt,1,SQL_C_LONG,&gi.val,0,&gi.len);  
  SQLBindCol(hstmt,2,SQL_C_LONG,&mmdbid.val,0,&mmdbid.len);  
  
  sqlrc = SQLFetch(hstmt);
  if(sqlrc == SQL_NO_DATA_FOUND){
    SQLFreeStmt(hstmt, SQL_DROP);
    /*ErrPostEx(SEV_INFO, 0, 0, "No data found for keynum : %ld",keynum);*/
    return FALSE;
  }

  while(sqlrc == SQL_SUCCESS || sqlrc == SQL_SUCCESS_WITH_INFO){
     pmg->mmdbid = mmdbid.val;
     pmg->gi = gi.val;
    sqlrc = SQLFetch(hstmt);
    if(sqlrc == SQL_SUCCESS || sqlrc == SQL_SUCCESS_WITH_INFO){
     pmg->next = StMmgiNew();
     pmg = pmg->next;
     }
  }
  if(SQLFreeStmt(hstmt, SQL_DROP) != SQL_SUCCESS){
     print_err(hstmt);
     return FALSE;
   }
  
   return TRUE;
 
}



Int2 LIBCALL SearchDOMDB (StDomdbPtr PNTR ppdd)
{
   StDomdbPtr pdd= NULL;
   Int4 keynum = 0;
   CharPtr key = NULL;
   Int4 bloblen,indicator,length;
   CharPtr domdb_stmt = "select gi, asn1,mmdbid,pdbid,chain,domno,domall,domid,rep from seqhound.domdb";
   SQLHANDLE hstmt=0;
   Int2 sqlrc=SQL_SUCCESS;
   SQLINTEGER blobloc, pbbvalue;
   SQLHANDLE LocUse;
   CharPtr pcASN = NULL;
   struct{
     Int4 len;
     Char val[20];
   }pdbid,chain;
   struct{
     Int4 len;
     Int4 val;
   }gi,mmdbid,domno,domall,domid,rep;

    /* SK: NULL ptr check */
    if (ppdd == NULL || *ppdd == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "SearchDOMDB: StDomdbPtr is NULL.");
       return 0;
    }
   pdd = *ppdd;
 
   if (pdd->domall > 0)
    {
      strcat(domdb_stmt, " where domall=?");
      CreateHandle(&hstmt,domdb_stmt,&henv,&hdbc);
      SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_LONG,SQL_INTEGER, 0, 0, &keynum, 0, NULL);  
      keynum =(Int4) pdd->domall;
    }
   if (pdd->mmdbid > 0)
    {
      strcat(domdb_stmt, " where mmdbid=?");
      CreateHandle(&hstmt,domdb_stmt,&henv,&hdbc);
      SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_LONG,SQL_INTEGER, 0, 0, &keynum, 0, NULL);  
      keynum = (Int4)pdd->mmdbid;
    }
   if (pdd->gi > 0)
    {
      strcat(domdb_stmt, " where gi=?");
      CreateHandle(&hstmt,domdb_stmt,&henv,&hdbc);
      SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_LONG,SQL_INTEGER, 0, 0, &keynum, 0, NULL);  
      keynum = pdd->gi;
    }
   if (pdd->domid > 0)
    {
      strcat(domdb_stmt, " where domid=?");
      CreateHandle(&hstmt,domdb_stmt,&henv,&hdbc);
      SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_LONG,SQL_INTEGER, 0, 0, &keynum, 0, NULL);  
      keynum = pdd->domid;
    }

   if (pdd->pdbid != NULL)
    {
      strcat(domdb_stmt, " where pdbid=?");
      key = pdd->pdbid;
      length = strlen(key);
      CreateHandle(&hstmt, domdb_stmt,&henv,&hdbc);
      SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_DEFAULT,SQL_VARCHAR, 15, 0, key, 0, (long*)&length);
    }
   if ((keynum == 0) && (key == NULL))
    {
       ErrPostEx(SEV_ERROR,0,0, "SearchDOMDB: No allowed search key.");
       return 0;
    }
   /* search DOMDB */

  /* set auto commit on */
   SQLSetConnectAttr( hdbc, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_ON, SQL_NTS);  

  /* execute statment */
  if (SQLExecute(hstmt) != SQL_SUCCESS){
	ErrPostEx(SEV_ERROR,0,0,"SearchDOMDB:unable to execute select statement.");
	print_err(hstmt);
	SQLFreeStmt(hstmt, SQL_DROP);
	return FALSE;
  }

  SQLBindCol(hstmt,1,SQL_C_LONG,&gi.val,0,(long*)&gi.len);  
  SQLBindCol(hstmt,2, SQL_C_BLOB_LOCATOR,&blobloc,0,&pbbvalue);
  SQLBindCol(hstmt,3,SQL_C_LONG,&mmdbid.val,0,(long*)&mmdbid.len);    
  SQLBindCol(hstmt,4,SQL_C_CHAR,pdbid.val,20,(long*)&pdbid.len);
  SQLBindCol(hstmt,5,SQL_C_CHAR,chain.val,10,(long*)&chain.len);
  SQLBindCol(hstmt,6,SQL_C_LONG,&domno.val,0,(long*)&domno.len); 
  SQLBindCol(hstmt,7,SQL_C_LONG,&domall.val,0,(long*)&domall.len);
  SQLBindCol(hstmt,8,SQL_C_LONG,&domid.val,0,(long*)&domid.len);
  SQLBindCol(hstmt,9,SQL_C_LONG,&rep.val,0,(long*)&rep.len);    

  sqlrc = SQLFetch(hstmt);
  if(sqlrc == SQL_NO_DATA_FOUND){
    ErrPostEx(SEV_INFO, 0, 0, "No data found for gi : %ld",gi);
      SQLFreeStmt(hstmt, SQL_DROP) ;
    return FALSE;
  }
  while(sqlrc == SQL_SUCCESS || sqlrc == SQL_SUCCESS_WITH_INFO){
     /*Get the length of of the blob so that we can read it */
    SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &LocUse);
    SQLGetLength(LocUse, SQL_C_BLOB_LOCATOR, blobloc, (long*)&bloblen, (long*)&indicator);

    /* allocate a buffer big enough to store the blob. */ 
    pcASN = (CharPtr) MemNew((size_t) bloblen);
    /*read the blob from the database */
    SQLGetData(hstmt,2,SQL_C_BINARY,(SQLPOINTER)pcASN,bloblen,(long*)&indicator);
   
    pdd->mmdbid = mmdbid.val;
    pdd->pdbid = StringSave(pdbid.val);
    pdd->chain = StringSave(chain.val);  
    pdd->gi = gi.val;
    pdd->domno =domno.val;
    pdd->domall = domall.val;
    pdd->domid =  domid.val;
    pdd->rep = rep.val;
    pdd->asn1 = (SLRIValNodePtr) GetASNMemChar(pcASN,(AsnReadFunc)SLRIValNodeAsnRead,bloblen);
    if (pdd->asn1 == NULL)
          {
             ErrPostEx(SEV_ERROR,0,0, "SearchDOMDB: GetDomainfromDOMDB failed.");
             return 0;
          }
   
    sqlrc = SQLFetch(hstmt);
    if(sqlrc == SQL_SUCCESS || sqlrc == SQL_SUCCESS_WITH_INFO){
     pdd->next = StDomdbNew();
     pdd = pdd->next;
     }
  }

  if (pcASN) MemFree(pcASN);
 
  if(SQLFreeStmt(hstmt, SQL_DROP) != SQL_SUCCESS){
     print_err(hstmt);
     return FALSE;
   }   
 
   return TRUE;  

}


SLRIValNodePtr LIBCALL GetDomainfromDOMDB(void)
{
   return NULL;
}


Int2 LIBCALL SearchREDUND (StRedundPtr PNTR pprd)
{
   StRedundPtr prd= NULL;
   Int4 keynum = 0;
   CharPtr gi_stmt ="SELECT gi,ordinal,group from seqhound.redund where gi= ?";
   CharPtr ordinal_stmt ="SELECT gi,ordinal,group from seqhound.redund where ordinal=?";
   CharPtr group_stmt ="SELECT gi,ordinal,group from seqhound.redund where group=?"; 
   CharPtr search_redund = NULL;
   SQLHANDLE hstmt;
   Int2 sqlrc=SQL_SUCCESS;
   struct{
     SQLINTEGER len;
     SQLINTEGER val;
   }gi,ordinal,group;

    /* SK: NULL ptr check */
    if (pprd == NULL || *pprd == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "SearchREDUND: StRedundPtr is NULL.");
       return 0;
    }
   prd = *pprd;
   if (prd->gi > 0)
    {
      keynum = prd->gi;
      search_redund = gi_stmt;
    }
   if (prd->ordinal > 0)
    {
      keynum = prd->ordinal;
      search_redund = ordinal_stmt;
    }
   if (prd->group > 0)
    {
      keynum = prd->group;
      search_redund = group_stmt;
    }
   if (keynum == 0)
    {
       ErrPostEx(SEV_ERROR,0,0, "SearchREDUND: No allowed search key.");
       return 0;
    }

   CreateHandle(&hstmt,search_redund,&henv,&hdbc);
  
  SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_LONG,SQL_INTEGER, 0, 0, &keynum, 0, NULL);

  /* set auto commit on */
  SQLSetConnectAttr( hdbc, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_ON, SQL_NTS);  

  /* execute statment */
  if (SQLExecute(hstmt) != SQL_SUCCESS){
    ErrPostEx(SEV_ERROR,0,0,"SearchREDUND:unable to execute select statement.");
    print_err(hstmt);
    SQLFreeStmt(hstmt, SQL_DROP);
    return FALSE;
  }

  SQLBindCol(hstmt,1,SQL_C_LONG,&gi.val,0,&gi.len);  
  SQLBindCol(hstmt,2,SQL_C_LONG,&ordinal.val,0,&ordinal.len);  
  SQLBindCol(hstmt,3,SQL_C_LONG,&group.val,0,&group.len);  
  
  sqlrc = SQLFetch(hstmt);
  if(sqlrc == SQL_NO_DATA_FOUND){
    ErrPostEx(SEV_INFO, 0, 0, "No data found for keynum : %ld",keynum);
    SQLFreeStmt(hstmt, SQL_DROP);
    return FALSE;
  }

  while(sqlrc == SQL_SUCCESS || sqlrc == SQL_SUCCESS_WITH_INFO){     
	prd->gi = gi.val;
    prd->ordinal = ordinal.val;
    prd->group = group.val;

    sqlrc = SQLFetch(hstmt);
    if(sqlrc == SQL_SUCCESS || sqlrc == SQL_SUCCESS_WITH_INFO){
      prd->next = StRedundNew();
      prd = prd->next;     
     }
  }
 
   if(SQLFreeStmt(hstmt, SQL_DROP) != SQL_SUCCESS){
     print_err(hstmt);
     return FALSE;
   }   
 
   return TRUE;
}

Int2 LIBCALL SearchCHROM (StChromPtr PNTR ppch)
{
   StChromPtr pch = NULL;
   Int4 keynum = 0;
   CharPtr taxid_stmt ="SELECT taxonid, chromid,chromflag from seqhound.chrom where taxonid= ?";
   CharPtr kloodge_stmt ="SELECT taxonid,chromid,chromflag from seqhound.chrom where chromid=?";
   CharPtr chromfl_stmt ="SELECT taxonid,chromid,chromflag from seqhound.chrom where chromflag=?"; 
   CharPtr search_chrom = NULL;
   SQLHANDLE hstmt;
   Int2 sqlrc=SQL_SUCCESS;
   struct{
     SQLINTEGER len;
     SQLINTEGER val;
   }taxid,kloodge, chromfl;

   /* SK: NULL ptr check */
   if (ppch == NULL || *ppch == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "SearchCHROM: StChromPtr is NULL.");
       return 0;
    }

   pch = *ppch;
   if ((pch->chromfl <= 0) && (pch->kloodge <= 0) && (pch->taxid <= 0))
    {
       ErrPostEx(SEV_ERROR,0,0, "SearchCHROM: No allowed search key.");
       return 0;
    }

   if (pch->taxid > 0)
    {
      keynum = pch->taxid;
      search_chrom = taxid_stmt;
    }
   if (pch->kloodge > 0)
    {
      keynum = pch->kloodge;
      search_chrom = kloodge_stmt;
    }
   if (pch->chromfl > 0)
    {
      keynum = pch->chromfl;
      search_chrom = chromfl_stmt;
    }
   if (keynum == 0)
    {
       ErrPostEx(SEV_ERROR,0,0, "SearchCHROM: No allowed search key.");
       return 0;
    }

   CreateHandle(&hstmt,search_chrom,&henv,&hdbc);
  
  SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_LONG,SQL_INTEGER, 0, 0, &keynum, 0, NULL);

  /* set auto commit on */
  SQLSetConnectAttr( hdbc, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_ON, SQL_NTS);  

  /* execute statment */
  if (SQLExecute(hstmt) != SQL_SUCCESS){
    ErrPostEx(SEV_ERROR,0,0,"SearchCHROM: unable to execute select statement.");
    print_err(hstmt);
    SQLFreeStmt(hstmt, SQL_DROP);
    return FALSE;
  }

  SQLBindCol(hstmt,1,SQL_C_LONG,&taxid.val,0,&taxid.len);  
  SQLBindCol(hstmt,2,SQL_C_LONG,&kloodge.val,0,&kloodge.len);  
  SQLBindCol(hstmt,3,SQL_C_LONG,&chromfl.val,0,&chromfl.len);  
  
  sqlrc = SQLFetch(hstmt);
  if(sqlrc == SQL_NO_DATA_FOUND){
    ErrPostEx(SEV_INFO, 0, 0, "No data found for keynum : %ld",keynum);
    SQLFreeStmt(hstmt, SQL_DROP) ;
    return FALSE;
  }

  while(sqlrc == SQL_SUCCESS || sqlrc == SQL_SUCCESS_WITH_INFO){
    pch->kloodge = kloodge.val;
    pch->taxid = taxid.val;
    pch->chromfl = chromfl.val;
    sqlrc = SQLFetch(hstmt);
    if(sqlrc == SQL_SUCCESS || sqlrc == SQL_SUCCESS_WITH_INFO){
      pch->next = StChromNew();
      pch = pch->next;
     }
  }

  if(SQLFreeStmt(hstmt, SQL_DROP) != SQL_SUCCESS){
     print_err(hstmt);
     return FALSE;
   }   
   return TRUE;
}


Int2  LIBCALL SearchSENGI (StSengiPtr PNTR ppsg)
{
   StSengiPtr pps= NULL;
   Int4 keynum = 0;
   CharPtr seid_stmt ="SELECT gi from seqhound.sengi where seid= ?";
   CharPtr gi_stmt ="SELECT seid from seqhound.sengi where gi=?";
   CharPtr search_sengi = NULL;
   SQLHANDLE hstmt;
   Int2 sqlrc=SQL_SUCCESS;

   struct{
     SQLINTEGER len;
     SQLINTEGER val;
   }gi, seid;

   /* SK: NULL ptr check */
   if (ppsg == NULL || *ppsg == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "SearchSENGI: StSengiPtr is NULL.");
       return 0;
    }
   pps = *ppsg;
   if (pps->gi > 0)
    {
      keynum = pps->gi;
      search_sengi = gi_stmt;
    }
   if (pps->seid > 0)
    {
      keynum = pps->seid;
      search_sengi = seid_stmt;
    }
   if (keynum == 0)
    {
       ErrPostEx(SEV_ERROR,0,0, "SearchSENGI: No allowed search key.");
       return 0;
    }

   CreateHandle(&hstmt,search_sengi,&henv,&hdbc);
  
  SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_LONG,SQL_INTEGER, 0, 0, &keynum, 0, NULL);

  /* set auto commit on */
  SQLSetConnectAttr( hdbc, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_ON, SQL_NTS);  

  /* execute statment */
  if (SQLExecute(hstmt) != SQL_SUCCESS){
    ErrPostEx(SEV_ERROR,0,0,"SearchSENGI: unable to execute select statement.");
    print_err(hstmt);
    SQLFreeStmt(hstmt, SQL_DROP);
    return -1;
  }

   if (pps->gi > 0)
   {
	SQLBindCol(hstmt,1,SQL_C_LONG,&seid.val,0,&seid.len);  
   }
   else if(pps->seid > 0)
   {
	SQLBindCol(hstmt,1,SQL_C_LONG,&gi.val,0,&gi.len);  

   }
  sqlrc = SQLFetch(hstmt);
  if(sqlrc == SQL_NO_DATA_FOUND){
    ErrPostEx(SEV_INFO, 0, 0, "No data found for gi : %ld",keynum);
      SQLFreeStmt(hstmt, SQL_DROP);
    return -1;
  }

  while(sqlrc == SQL_SUCCESS || sqlrc == SQL_SUCCESS_WITH_INFO){
   if (pps->gi > 0)
   {
   	pps->seid = seid.val;
   }
   else if(pps->seid > 0)
   {
   	pps->gi = gi.val;
   }
    sqlrc = SQLFetch(hstmt);
    if(sqlrc == SQL_SUCCESS || sqlrc == SQL_SUCCESS_WITH_INFO){
     pps->next = StSengiNew();
     pps = pps->next;
     }
  }

 if(SQLFreeStmt(hstmt, SQL_DROP) != SQL_SUCCESS){
     print_err(hstmt);
     return -1;
   }
 
      return 1;
}


Int2 LIBCALL SearchSENDBforGI(StSendbPtr PNTR ppsd, Int4 gi)
{
   StSendbPtr psd = NULL;
   CharPtr search_sendb = "Select seqhound.sendb.seid,asn1 from seqhound.sendb, seqhound.sengi where seqhound.sengi.seid = seqhound.sendb.seid and gi = ?";
   SQLHANDLE hstmt, LocUse=0;
   Int4 bloblen=0,indicator=0, gi_passed = 0;
   Int2 sqlrc=SQL_SUCCESS;
   SQLINTEGER blobloc, pbbvalue;
   CharPtr pcASN = NULL;
  struct{
     SQLINTEGER len;
     SQLINTEGER val;
   }seid;

   /* SK: NULL ptr check */
   if (ppsd == NULL || *ppsd == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "SearchSENDBforGI: StSendbPtr is NULL.");
       return 0;
    }
   psd = *ppsd;

  CreateHandle(&hstmt,search_sendb,&henv,&hdbc);
  
  SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_LONG,SQL_INTEGER, 0, 0, &gi_passed, 0, NULL);
	gi_passed = gi;

  /* set auto commit on */
  SQLSetConnectAttr( hdbc, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_ON, SQL_NTS);  

  /* execute statment */
  if (SQLExecute(hstmt) != SQL_SUCCESS){
    ErrPostEx(SEV_ERROR,0,0,"SearchSENDBforGI: unable to execute select statement.");
    print_err(hstmt);
    SQLFreeStmt(hstmt, SQL_DROP);
    return FALSE;
  }

  SQLBindCol(hstmt,1,SQL_C_LONG,&seid.val,0,&seid.len);  
  SQLBindCol(hstmt,2, SQL_C_BLOB_LOCATOR,&blobloc,0,&pbbvalue);

  sqlrc = SQLFetch(hstmt);
  if(sqlrc == SQL_ERROR){
	print_err(hstmt);
	return FALSE;
  }
  else if(sqlrc == SQL_NO_DATA_FOUND){
    SQLFreeStmt(hstmt, SQL_DROP);
    ErrPostEx(SEV_INFO, 0, 0, "No data found for gi : %ld",gi);
     SQLFreeStmt(hstmt, SQL_DROP) ;
    return FALSE;
  }
  else{
    while(sqlrc == SQL_SUCCESS || sqlrc == SQL_SUCCESS_WITH_INFO){
      /*Get the length of of the blob so that we can read it */
      SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &LocUse);
      if(SQLGetLength(LocUse, SQL_C_BLOB_LOCATOR, blobloc, (long*)&bloblen, (long*)&indicator)!= SQL_SUCCESS)
        print_err(hstmt);
     
      if(bloblen > 0){
        /* allocate a buffer big enough to store the blob. */ 
        pcASN = (CharPtr) MemNew((size_t) bloblen);
        /*read the blob from the database */
        if(SQLGetData(hstmt,2,SQL_C_BINARY,(SQLPOINTER)pcASN,bloblen,(long*)&indicator) != SQL_SUCCESS)
	  print_err(hstmt);

        psd->seid = seid.val;
        if(pcASN != NULL)   
          psd->asn1 = GetSeqEntry(pcASN, bloblen);

        if (pcASN) MemFree(pcASN);   

        sqlrc = SQLFetch(hstmt);
        if(sqlrc == SQL_SUCCESS || sqlrc == SQL_SUCCESS_WITH_INFO){
          psd->next = StSendbNew();
         psd = psd->next;
        }
      }
    }
  }
   if (SQLFreeHandle(SQL_HANDLE_STMT, LocUse) != SQL_SUCCESS){
          print_err(hstmt);
          return FALSE;
       }

  if(SQLFreeStmt(hstmt, SQL_DROP) != SQL_SUCCESS){
     print_err(hstmt);
     return FALSE;
   } 
   
   return TRUE;        
}


/* returns record number of the SENGI table */
Int4 LIBCALL SHoundRecNoSENGI(void)
{
  SQLHANDLE hstmt_count;
  SQLCHAR * stmt = (SQLCHAR *)"SELECT count(*) from seqhound.sengi";
  Int4 recordcount=-1;

  CreateHandle(&hstmt_count,stmt,&henv,&hdbc);

  if(SQLBindCol(hstmt_count,1,SQL_C_LONG,&recordcount,0,0)!=SQL_SUCCESS){
  	ErrPostEx(SEV_ERROR,0,0, "SHoundRecNoSENGI: could not bind record count.");
        print_err(hstmt_count);
   }
  
  if(SQLExecute(hstmt_count)!=SQL_SUCCESS){
	ErrPostEx(SEV_ERROR,0,0, "SHoundRecNoSENGI: could not execute statment.");
        print_err(hstmt_count);
   }

  if(SQLFetch(hstmt_count)!=SQL_SUCCESS){
	ErrPostEx(SEV_ERROR,0,0, "SHoundRecNoSENGI: could not fetch result.");
        print_err(hstmt_count);
   }
  
  SQLCloseCursor(hstmt_count);
  SQLFreeHandle(SQL_HANDLE_STMT,hstmt_count);   

  return recordcount;

}

/* returns record number */
Int4 LIBCALL SHoundRecNoDOMDB(void)
{
	return 0;
}

StRedundPtr LIBCALL SHoundGetRecREDUND(Int4 group){
  StRedundPtr highest_group = NULL;
 
  highest_group = StRedundNew();
  highest_group->group = group;
  SearchREDUND(&highest_group);

  return highest_group;
}

ValNodePtr LIBCALL SHoundAllGenomes(void){
  ValNodePtr pallgenome = NULL;
   CharPtr chrom_stmt ="SELECT taxonid from seqhound.chrom";
   SQLHANDLE hstmt;
   Int2 sqlrc=SQL_SUCCESS;
   struct{
     SQLINTEGER len;
     SQLINTEGER val;
   }taxid;

  CreateHandle(&hstmt,chrom_stmt,&henv,&hdbc);
      
  /* set auto commit on */
  SQLSetConnectAttr( hdbc, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_ON, SQL_NTS);
     
  /* execute statment */
  if (SQLExecute(hstmt) != SQL_SUCCESS){
    ErrPostEx(SEV_ERROR,0,0,"SHoundAllGenomes:unable to execute select statement.");
    print_err(hstmt);   
    SQLFreeStmt(hstmt, SQL_DROP);
    return NULL;
  }
     
  SQLBindCol(hstmt,1,SQL_C_LONG,&taxid.val,0,&taxid.len);
       
  sqlrc = SQLFetch(hstmt);
  if(sqlrc == SQL_NO_DATA_FOUND){
    ErrPostEx(SEV_INFO, 0, 0, "No data found for all genomes");
     SQLFreeStmt(hstmt, SQL_DROP) ;
    return NULL;
  }
      
  while(sqlrc == SQL_SUCCESS || sqlrc == SQL_SUCCESS_WITH_INFO){
    pallgenome = Slri_ValNodeAddInt(&pallgenome, 0, taxid.val);

    sqlrc = SQLFetch(hstmt);
  }

    
  if(SQLFreeStmt(hstmt, SQL_DROP) != SQL_SUCCESS){
     print_err(hstmt);
   }   
   return pallgenome;
} 

/* returns record number */
/* Purpose of this method is to get the last entered record */
/* so if you are building redund and stop in the middle you */
/* can start the building off from where you left off.      */
/* RETURNS : the record in redund that has the largest group*/
/*  returns zero if there is nothing in the table.          */
Int4 LIBCALL SHoundRecNoREDUND(void)
{
   CharPtr search_redund ="SELECT MAX(group) as Max from seqhound.redund";
   SQLHANDLE hstmt;
   Int2 sqlrc=SQL_SUCCESS;
   struct{
     SQLINTEGER len;
     SQLINTEGER val;
   }max;

   CreateHandle(&hstmt,search_redund,&henv,&hdbc);
 
  /* set auto commit on */
  SQLSetConnectAttr( hdbc, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_ON, SQL_NTS);  

  /* execute statment */
  if (SQLExecute(hstmt) != SQL_SUCCESS){
    ErrPostEx(SEV_ERROR,0,0,"SHoundRecNoRedund:unable to execute select statement.");
    print_err(hstmt);
    SQLFreeStmt(hstmt, SQL_DROP);
    return FALSE;
  }

  SQLBindCol(hstmt,1,SQL_C_LONG,&max.val,0,&max.len);  

  sqlrc = SQLFetch(hstmt);
  if(sqlrc == SQL_NO_DATA_FOUND){
    ErrPostEx(SEV_INFO, 0, 0, "No data found");
      SQLFreeStmt(hstmt, SQL_DROP);
    return 0;
  }

  if(sqlrc == SQL_SUCCESS || sqlrc == SQL_SUCCESS_WITH_INFO){
	SQLFreeStmt(hstmt, SQL_DROP);
	return max.val;    
  }
 
   SQLFreeStmt(hstmt, SQL_DROP); 
   return -1;
}

Int2 LIBCALL SearchSENDBforGIforTindex(StSendbPtr PNTR ppsd, Int4 gi)
{
return -1;
}


/***************************************/
/*     Delete records in databases     */
/***************************************/
/* if Boolean variable "destroy" is TRUE the record will be blanked out
and GI substituted with -1 if it is FALSE record will be simply deleted */
/*  In db2 there is no such thing as a logical delete unless a flag has
been defined in the table so the delete functions in this db2 layer simply 
delete the record.  */
Boolean LIBCALL DeleteGIinPARTI(Int4 gi, Boolean destroy)
{
   CharPtr parti_delete = "delete from seqhound.parti where gi=?";
   SQLHANDLE hstmt;
   Int2 sql_return = SQL_SUCCESS;

   CreateHandle(&hstmt,parti_delete,&henv,&hdbc);

   SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_LONG,SQL_INTEGER, 0, 0, &gi, 0, NULL);

  /* set auto commit on */
      SQLSetConnectAttr( hdbc, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_OFF, SQL_NTS);  

   sql_return = SQLExecute(hstmt);

  /* execute statment */
  if ((sql_return != SQL_SUCCESS) && (sql_return != SQL_NO_DATA_FOUND)){
	ErrPostEx(SEV_ERROR,0,0,"DeleteGIinParti:unable to execute delete statement.");
	print_err(hstmt);
	SQLFreeStmt(hstmt, SQL_DROP);
	return FALSE;
  }

    SQLFreeStmt(hstmt, SQL_DROP);

    return TRUE;
}


/* both the div and destroy parameters are ignored in db2 version.*/
Boolean LIBCALL DeleteGIinASNDB(CharPtr div, Int4 gi, Boolean destroy)
{
   CharPtr asndb_delete = "delete from seqhound.asndb where gi=?";
   SQLHANDLE hstmt;
   Int2 sql_return = SQL_SUCCESS;

   CreateHandle(&hstmt,asndb_delete,&henv,&hdbc);

   SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_LONG,SQL_INTEGER, 0, 0, &gi, 0, NULL);

  /* set auto commit off */
      SQLSetConnectAttr( hdbc, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_OFF, SQL_NTS);  
   sql_return = SQLExecute(hstmt);

  /* execute statment */
  if ((sql_return != SQL_SUCCESS) && (sql_return != SQL_NO_DATA_FOUND)){
	ErrPostEx(SEV_ERROR,0,0,"DeleteGIinASNDB:unable to execute delete statement.");
	print_err(hstmt);
	SQLFreeStmt(hstmt, SQL_DROP);
	return FALSE;
  }
  SQLFreeStmt(hstmt, SQL_DROP);

    return TRUE;
}


Boolean LIBCALL DeleteGIinACCDB(Int4 gi, Boolean destroy)
{
   CharPtr accdb_delete = "delete from seqhound.accdb where gi=?";
   SQLHANDLE hstmt;
   Int2 sql_return = SQL_SUCCESS;

   CreateHandle(&hstmt,accdb_delete,&henv,&hdbc);

   SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_LONG,SQL_INTEGER, 0, 0, &gi, 0, NULL);

  /* set auto commit off */
      SQLSetConnectAttr( hdbc, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_OFF, SQL_NTS); 
 sql_return = SQLExecute(hstmt);

  /* execute statment */
  if ((sql_return != SQL_SUCCESS) && (sql_return != SQL_NO_DATA_FOUND)){
	ErrPostEx(SEV_ERROR,0,0,"DeleteGIinACCDB:unable to execute delete statement.");
	print_err(hstmt);
	SQLFreeStmt(hstmt, SQL_DROP);
	return FALSE;
  }
  SQLFreeStmt(hstmt, SQL_DROP);

    return TRUE;
}


Boolean LIBCALL DeleteGIinTAXGI(Int4 gi, Boolean destroy)
{
   CharPtr taxgi_delete = "delete from seqhound.taxgi where gi=?";
   SQLHANDLE hstmt;
   Int2 sql_return = SQL_SUCCESS;

   CreateHandle(&hstmt,taxgi_delete,&henv,&hdbc);

   SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_LONG,SQL_INTEGER, 0, 0, &gi, 0, NULL);

  /* set auto commit off */
     SQLSetConnectAttr( hdbc, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_OFF, SQL_NTS);  
  
   sql_return = SQLExecute(hstmt);

  /* execute statment */
  if ((sql_return != SQL_SUCCESS) && (sql_return != SQL_NO_DATA_FOUND)){
	ErrPostEx(SEV_ERROR,0,0,"DeleteGIinTAXGI:unable to execute delete statement.");
	print_err(hstmt);
	SQLFreeStmt(hstmt, SQL_DROP);
	return FALSE;
  }
  SQLFreeStmt(hstmt, SQL_DROP);

    return TRUE;
}


Boolean LIBCALL DeleteGIinPUBSEQ(Int4 gi, Boolean destroy)
{
   CharPtr pubseq_delete = "delete from seqhound.pubseq where gi=?";
   SQLHANDLE hstmt;
   Int2 sql_return = SQL_SUCCESS;

   CreateHandle(&hstmt,pubseq_delete,&henv,&hdbc);

   SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_LONG,SQL_INTEGER, 0, 0, &gi, 0, NULL);

   /* set auto commit off */
      SQLSetConnectAttr( hdbc, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_OFF, SQL_NTS);  
  
  sql_return = SQLExecute(hstmt);

  /* execute statment */
  if ((sql_return != SQL_SUCCESS) && (sql_return != SQL_NO_DATA_FOUND)){
	ErrPostEx(SEV_ERROR,0,0,"DeleteGIinPUBSEQ:unable to execute delete statement.");
	print_err(hstmt);
	/*SQLFreeStmt(hstmt, SQL_DROP);*/
	return FALSE;
  }
  SQLFreeStmt(hstmt, SQL_DROP);

    return TRUE;
}


Boolean LIBCALL DeleteGIinNUCPROT(Int4 gi, Boolean destroy)
{
  
    CharPtr nucprot_delete = "delete from seqhound.nucprot where gia=?";
    CharPtr nucprot2_delete = "delete from seqhound.nucprot where gin =?";
   SQLHANDLE hstmt, hstmt2;
   Int2 sql_return = SQL_SUCCESS;
   CreateHandle(&hstmt,nucprot_delete,&henv,&hdbc);


   SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_LONG,SQL_INTEGER, 0, 0, &gi, 0, NULL);

  /* set auto commit off */
     SQLSetConnectAttr( hdbc, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_OFF, SQL_NTS);  
  
  sql_return = SQLExecute(hstmt);

  /* execute statment */
  if ((sql_return != SQL_SUCCESS) && (sql_return != SQL_NO_DATA_FOUND)){
	ErrPostEx(SEV_ERROR,0,0,"DeleteGIinNUCPROT:unable to execute delete statement.");
	print_err(hstmt);
	SQLFreeStmt(hstmt, SQL_DROP);
	return FALSE;
  }
  SQLFreeStmt(hstmt, SQL_DROP);

 CreateHandle(&hstmt2,nucprot2_delete,&henv,&hdbc);


   SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_LONG,SQL_INTEGER, 0, 0, &gi, 0, NULL);

  /* set auto commit off */
     SQLSetConnectAttr( hdbc, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_OFF, SQL_NTS);  
  
  sql_return = SQLExecute(hstmt2);

  /* execute statment */
  if ((sql_return != SQL_SUCCESS) && (sql_return != SQL_NO_DATA_FOUND)){
	ErrPostEx(SEV_ERROR,0,0,"DeleteGIinNUCPROT:unable to execute delete statement.");
	print_err(hstmt2);
	SQLFreeStmt(hstmt2, SQL_DROP);
	return FALSE;
  }
  SQLFreeStmt(hstmt2, SQL_DROP);

    return TRUE;
}


Boolean LIBCALL DeleteGIinSENGI(Int4 gi, Boolean destroy)
{
   CharPtr sengi_delete = "delete from seqhound.sengi where gi=?";
   SQLHANDLE hstmt;
   Int2 sql_return = SQL_SUCCESS;

   CreateHandle(&hstmt,sengi_delete,&henv,&hdbc);

   SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_LONG,SQL_INTEGER, 0, 0, &gi, 0, NULL);

  /* set auto commit off */
     SQLSetConnectAttr( hdbc, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_OFF, SQL_NTS);  
 
  sql_return = SQLExecute(hstmt);

  /* execute statment */
  if ((sql_return != SQL_SUCCESS) && (sql_return != SQL_NO_DATA_FOUND)){
	ErrPostEx(SEV_ERROR,0,0,"DeleteGIinSENGI:unable to execute delete statement.");
	print_err(hstmt);
	SQLFreeStmt(hstmt, SQL_DROP);
	return FALSE;
  }
  SQLFreeStmt(hstmt, SQL_DROP);

    return TRUE;
}


Boolean LIBCALL DeleteGIinSENDB(CharPtr div, Int4 gi, Boolean destroy)
{
  /*CharPtr sendb_delete = "delete from seqhound.sendb where seid=?";*/
  CharPtr sendb_delete = "delete from seqhound.sendb where seid In (select seid from seqhound.sengi where gi = ? AND seid is not NULL)";
   SQLHANDLE hstmt;
   Int2 sql_return = SQL_SUCCESS;
   
   /*Int4 seid = 0;
   
   StSendbPtr psd = NULL, psdhead = NULL;

   psd = StSendbNew();
   psdhead = psd;
   SearchSENDBforGI(&psd, gi);
   
   seid = psd->seid;

   StSendbFree(psdhead);*/

   CreateHandle(&hstmt,sendb_delete,&henv,&hdbc);

      SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_LONG,SQL_INTEGER, 0, 0, &gi, 0, NULL);

  /* set auto commit off */
     SQLSetConnectAttr( hdbc, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_OFF, SQL_NTS);  

  sql_return = SQLExecute(hstmt);

  /* execute statment */
  if ((sql_return != SQL_SUCCESS) && (sql_return != SQL_NO_DATA_FOUND)){
	ErrPostEx(SEV_ERROR,0,0,"DeleteGIinSENDB:unable to execute delete statement.");
	print_err(hstmt);
	SQLFreeStmt(hstmt, SQL_DROP);
	return FALSE;
  }
  SQLFreeStmt(hstmt, SQL_DROP);

    return TRUE;
}



Boolean LIBCALL DeleteGIinREDUND(Int4 gi, Boolean destroy)
{
   CharPtr redund_delete = "delete from seqhound.redund where gi=?";
   SQLHANDLE hstmt;
   Int2 sql_return = SQL_SUCCESS;

   CreateHandle(&hstmt,redund_delete,&henv,&hdbc);

   SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_LONG,SQL_INTEGER, 0, 0, &gi, 0, NULL);

  /* set auto commit on */
   SQLSetConnectAttr( hdbc, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_ON, SQL_NTS);  

  sql_return = SQLExecute(hstmt);

  /* execute statment */
  if ((sql_return != SQL_SUCCESS) && (sql_return != SQL_NO_DATA_FOUND)){
	ErrPostEx(SEV_ERROR,0,0,"DeleteGIinREDUND:unable to execute delete statement.");
	print_err(hstmt);
	SQLFreeStmt(hstmt, SQL_DROP);
	return FALSE;
  }
  SQLFreeStmt(hstmt, SQL_DROP);

    return TRUE;
}


/*******************************************/
/*     Reset structures representing       */
/*     database fields (structs declared   */
/*     in objslristruc.h)                  */
/* none of these actually free memory !!   */
/*******************************************/


void LIBCALL ResetStASNDB(StAsndbPtr PNTR ppas)
{
   StAsndbPtr pas = NULL;

   /* SK: NULL ptr check */
   if(ppas == NULL || *ppas == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "ResetStASNDB: NULL parameter.");
      return;
    }

   pas = *ppas;
   pas->asn1 = NULL;
   pas->gi = 0;
   pas->type = NULL;
   pas->division = NULL;
   pas->release = NULL;

   return;
}


void LIBCALL ResetStPARTI(StPartiPtr PNTR pppa)
{
   StPartiPtr ppa = NULL;

   /* SK: NULL ptr check */
   if(pppa == NULL || *pppa == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "ResetStPARTI: NULL parameter.");
      return;
    }

   ppa = *pppa;
   ppa->gi = 0;
   ppa->partition = NULL;

   return;
}



void LIBCALL ResetStNUCPROT(StNucprotPtr PNTR ppnc)
{
   StNucprotPtr pnc = NULL;

   /* SK: NULL ptr check */
   if(ppnc == NULL || *ppnc == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "ResetStNUCPROT: NULL parameter.");
      return;
    }

   pnc = *ppnc;
   pnc->gin = 0;
   pnc->gia = 0;

   return;
}



void LIBCALL ResetStACCDB(StAccdbPtr PNTR ppac)
{
   StAccdbPtr pac = NULL;

   /* SK: NULL ptr check */
   if(ppac == NULL || *ppac == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "ResetStACCDB: NULL parameter.");
      return;
    }

   pac = *ppac;
   pac->title = NULL;
   pac->gi = 0;
   pac->db = NULL;
   pac->name = NULL;
   pac->namelow = NULL;
   pac->access = NULL;
   pac->release = NULL;
   pac->version = 0;
   pac->chain = NULL;

   return;
}



void LIBCALL ResetStPUBSEQ(StPubseqPtr PNTR ppps)
{
   StPubseqPtr pps = NULL;

   /* SK: NULL ptr check */
   if(ppps == NULL || *ppps == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "ResetStPUBSEQ: NULL parameter.");
      return;
    }

   pps = *ppps;
   pps->gi = 0;
   pps->muid = 0;

   return;
}

void LIBCALL ResetStTAXGI(StTaxgiPtr PNTR pptg)
{
   StTaxgiPtr ptg = NULL;

   /* SK: NULL ptr check */
   if(pptg == NULL || *pptg == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "ResetStTAXGI: NULL parameter.");
      return;
    }

   ptg = *pptg;
   ptg->taxid = 0;
   ptg->gi = 0;
   ptg->type = NULL;

   return;
}


void LIBCALL ResetStCHROM(StChromPtr PNTR ppch)
{
   StChromPtr pch = NULL;

   /* SK: NULL ptr check */
   if(ppch == NULL || *ppch == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "ResetStCHROM: NULL parameter.");
      return;
    }

   pch = *ppch;
   pch->taxid = 0;
   pch->kloodge = 0;
   pch->chromfl = 0;

   return;
}


void LIBCALL ResetStSENDB(StSendbPtr PNTR ppsd)
{
   StSendbPtr psd = NULL;

   /* SK: NULL ptr check */
   if(ppsd == NULL || *ppsd == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "ResetStSENDB: NULL parameter.");
      return;
    }

   psd = *ppsd;
   psd->asn1 = NULL;
   psd->seid = 0;

   return;
}


void LIBCALL ResetStSENGI(StSengiPtr PNTR ppsg)
{
   StSengiPtr psg = NULL;

   /* SK: NULL ptr check */
   if(ppsg == NULL || *ppsg == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "ResetStSENGI: NULL parameter.");
      return;
    }

   psg = *ppsg;
   psg->seid = 0;
   psg->gi = 0;
   psg->division = NULL;

   return;
}


void LIBCALL ResetStMMDB(StMmdbPtr PNTR ppmd)
{
   StMmdbPtr pmd = NULL;

   /* SK: NULL ptr check */   
   if(ppmd == NULL || *ppmd == NULL )
    {
      ErrPostEx(SEV_ERROR,0,0, "ResetStMMDB: NULL parameter.");
      return;
    }


   pmd = *ppmd;
   pmd->asn1 = NULL;
   pmd->bwhat = 0;
   pmd->mmdbid = 0;
   pmd->pdbid = NULL;
   pmd->models = 0;
   pmd->molecules =0;

   return;
}


void LIBCALL ResetStMMGI(StMmgiPtr PNTR ppmg)
{
   StMmgiPtr pmg = NULL;

   /* SK: NULL ptr check */
   if(ppmg == NULL || *ppmg == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "ResetStMMGI: NULL parameter.");
      return;
    }

   pmg = *ppmg;
   pmg->mmdbid = 0;
   pmg->gi = 0;

   return;
}


void LIBCALL ResetStREDUND(StRedundPtr PNTR pprd)
{
   StRedundPtr prd = NULL;

   /* SK: NULL ptr check */
   if(pprd == NULL || *pprd == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "ResetStREDUND: NULL parameter.");
      return;
    }

   prd = *pprd;
   prd->gi = 0;
   prd->ordinal = 0;
   prd->group = 0;

   return;
}



void LIBCALL ResetStDOMDB(StDomdbPtr PNTR ppdd)
{
   StDomdbPtr pdd = NULL;

   /* SK: NULL ptr check */
   if(ppdd == NULL || *ppdd == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "ResetStDOMDB: NULL parameter.");
      return;
    }

   pdd = *ppdd;
   pdd->asn1 = NULL;
   pdd->mmdbid = 0;
   pdd->domid = 0;
   pdd->gi = 0;
   pdd->domall = 0;
   pdd->pdbid = NULL;
   pdd->chain = NULL;
   pdd->rep = 0;

   return;


}

/*
void LIBCALL ResetStVASTDB(StVastdbPtr PNTR ppvd)
{

   StVastdbPtr pvd = NULL;

   /* SK: NULL ptr check */
   if(ppvd == NULL || *ppvd == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "ResetStVASTDB: NULL parameter.");
      return;
    }

   pvd = *ppvd;
   pvd->asn1 = NULL;
   pvd->mmdbid = 0;
   pvd->pdbid = NULL;
   pvd->chain = NULL;
   pvd->vastid1 = 0;
   pvd->vastid2 = 0;
   pvd->score = 0;
   pvd->pval = 0;
   pvd->res = 0;
   pvd->ident = 0;
   pvd->rmsd = 0;
   pvd->scale = 0;

   return;
}
*/

/*March 2001, Doron*/    
/* Reset StCDDB pointer*/
void LIBCALL ResetStCDDB (StCddbPtr PNTR pcdp)
{
        StCddbPtr cdp=NULL;

	/* SK: NULL ptr check */
        if(pcdp == NULL || *pcdp == NULL)
        {
                ErrPostEx (SEV_ERROR,0 ,0 , "ReserStCDDB : Null parameter.");
       		 return;  
        }

        cdp = *pcdp;
        cdp->gi=0;
        cdp->asn1=NULL;
        return;

}

/*March 2001, Doron
Get SLRIFasta coding region*/
SLRIFastaPtr LIBCALL GetSLRIFastaCDFromCDDB( void)
{
  return NULL;
}
/* lighweight version of SHoundProteinsFromTaxID which does not fill out linked list of structures from TAXGI */
ValNodePtr LIBCALL SHoundProteinsFromTaxIDII(Int4 taxid)
{
	return SHoundProteinsFromTaxID(taxid);
}

/* lighweight version of SHoundProteinsFromTaxID which does not fill out linked list of structures from TAXGI */
ValNodePtr LIBCALL SHoundDNAFromTaxIDII(Int4 taxid)
{
	return SHoundDNAFromTaxID(taxid);
}


Int2 LIBCALL SearchCDDB(StCddbPtr PNTR ppcd)
{
/*   StCddbPtr pcd = NULL;
   Int4 keynum = 0;
   int rc;
   SLRIFastaPtr sfp;

   // SK: NULL ptr check
   if (ppcd == NULL || *ppcd == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "SearchCDDB: StCddbPtr is NULL.");
       return 0;
    }
   pcd = *ppcd;
   if (pcd->gi <= 0)
    {
       ErrPostEx(SEV_ERROR,0,0, "SearchCDDB: No allowed search key (GI).");
       return 0;
    }

   keynum = pcd->gi;

   d4tagSelect(pd4Cddb, tagcdGi);
   d4top(pd4Cddb);
   rc = d4seekDouble(pd4Cddb, keynum);
   if (rc == r4success)
   {
*/      /* fill out the StSendb structure */
/*      pcd->gi = f4long(pf4cdGi); */
      /* get Seq-entry */
/*      sfp = GetSLRIFastaCDFromCDDB();
      if (sfp == NULL)
       {
         ErrPostEx(SEV_ERROR,0,0, "SearchCDDB: SLRIFasta pointer is NULL.");
         return 0;
       }
      pcd->asn1 = sfp;
      for(rc = d4seekNextDouble(pd4Cddb, keynum); rc == r4success; rc = d4seekNextDouble(pd4Cddb, keynum))
        {
*/          /* add and fill out the next StSendb structure */
/*          pcd->next = StCddbNew();
          pcd = pcd->next;
          pcd->gi = f4long(pf4cdGi);
          sfp = GetSLRIFastaCDFromCDDB();
	if (sfp == NULL)
       	{
           ErrPostEx(SEV_ERROR,0,0, "SearchCDDB: SLRIFasta pointer is NULL.");
           return 0;
         }
          pcd->asn1 = sfp;
        }
      return 1;
    }
   else*/
      return -1;
}

/*************************************/
/* Edit record functions - needed for*/
/* some parsers (comgen and pdbrep)  */
/* search key is always in the name  */
/* of the function                   */
/*************************************/

Boolean LIBCALL EditRecordACCDBByGI(StAccdbPtr pac, Boolean overwrite)
{
    StAccdbPtr pactemp = NULL;
	Char stmt[250];
    CharPtr update_stmt = "UPDATE seqhound.accdb set db = ?, name = ?, access = ?, chain = ?, release = ?, version = ? where gi = ?";
    Char db[15],name[30],access[20],chain[20],release[20];
    Int4 gi,version,dblen,namelen,accesslen,chainlen,releaselen;
    SQLHANDLE hstmt_update;

	if (pac->gi <= 0)
     {
       ErrPostEx(SEV_ERROR,0,0, "EditRecordACCDBByGI: Invalid parameters.");
       return FALSE;
     }
    pactemp = StAccdbNew();
	pactemp->gi = pac->gi;
	SearchACCDB(&pactemp);

	if(pactemp->access ==NULL){
	  ErrPostEx(SEV_ERROR,0,0,"EditRecordACCDBByGi: no record fot gi %ld",pac->gi);
	  return FALSE;
	}
    
	if(overwrite)  /* replace all values except GI but only it the replacement values are valid*/
     {
         CreateHandle(&hstmt_update, update_stmt,&henv, &hdbc);

	    /*set up the input parameters */
		SQLBindParameter(hstmt_update, 1, SQL_PARAM_INPUT, SQL_C_DEFAULT,SQL_VARCHAR, 15, 0, db, 0, (long*)&dblen);
		SQLBindParameter(hstmt_update, 2, SQL_PARAM_INPUT, SQL_C_DEFAULT,SQL_VARCHAR, 30, 0, name, 0, (long*)&namelen);
		SQLBindParameter(hstmt_update, 3, SQL_PARAM_INPUT, SQL_C_DEFAULT,SQL_VARCHAR, 15, 0, access, 0, (long*)&accesslen);
		SQLBindParameter(hstmt_update, 4, SQL_PARAM_INPUT, SQL_C_DEFAULT,SQL_VARCHAR, 20, 0, chain, 0, (long*)&chainlen);
		SQLBindParameter(hstmt_update, 5, SQL_PARAM_INPUT, SQL_C_DEFAULT,SQL_VARCHAR, 20, 0, release, 0, (long*)&releaselen);
		SQLBindParameter(hstmt_update, 6, SQL_PARAM_INPUT, SQL_C_LONG,SQL_INTEGER, 0, 0, &version, 0, NULL);
		SQLBindParameter(hstmt_update, 7, SQL_PARAM_INPUT, SQL_C_LONG,SQL_INTEGER, 0, 0, &gi, 0, NULL);

		gi = pac->gi;
		sprintf(db,"%s",IsNullStr(pac->db));
		dblen = strlen(db);
		sprintf(name,"%s",IsNullStr(pac->name));
		namelen = strlen(name);
		sprintf(access,"%s",IsNullStr(pac->access));
		accesslen = strlen(access);
		sprintf(chain,"%s",IsNullStr(pac->chain));
		chainlen = strlen(chain);
		sprintf(release,"%s",IsNullStr(pac->release));
		releaselen = strlen(release);
		version = pac->version;

		  /* execute statment */
		if (SQLExecute (hstmt_update) != SQL_SUCCESS){
			ErrPostEx(SEV_ERROR,0,0, "EditRecordACCDBByGi: unable to execute ACCDB update statement. ");
			print_err(hstmt_accdb);
			return FALSE;
		}


     }
     else  /* replace only if no value in the database*/
     {
		 if((strstr(pactemp->db, "n/a") != NULL) && (pac->db != NULL)) {
			sprintf(stmt, "UPDATE seqhound.accdb set db='%s' where gi = %ld",IsNullStr(pac->db),pac->gi);
			execute_sql(stmt,&henv,&hdbc);
		 }
        if((strstr(pactemp->name, "n/a") != NULL) && (pac->name != NULL)) {
			sprintf(stmt, "UPDATE seqhound.accdb set name='%s' where gi= %ld",IsNullStr(pac->name),pac->gi);
			execute_sql(stmt,&henv,&hdbc);
		 }
        if((strstr(pactemp->access, "n/a") != NULL) && (pac->access != NULL)){
			sprintf(stmt, "UPDATE seqhound.accdb set access='%s' where gi= %ld",IsNullStr(pac->access),pac->gi);
			execute_sql(stmt,&henv,&hdbc);
		 }
        if((strstr(pactemp->release, "n/a") != NULL) && (pac->release != NULL)) {
			sprintf(stmt, "UPDATE seqhound.accdb set release='%s' where gi= %ld",IsNullStr(pac->release),pac->gi);
			execute_sql(stmt,&henv,&hdbc);
		 }
        if((pactemp->version == 0) && (pac->version != 0)) {
			sprintf(stmt, "UPDATE seqhound.accdb set version=%ld where gi= %ld",pac->version,pac->gi);
			execute_sql(stmt,&henv,&hdbc);
		 }
        if((strstr(pactemp->chain, "n/a") != NULL) && (pac->chain != NULL)) {
			sprintf(stmt, "UPDATE seqhound.accdb set chain='%s' where gi= %ld",IsNullStr(pac->chain),pac->gi);
			execute_sql(stmt,&henv,&hdbc);
		 }
     }

    StAccdbFree(pactemp);
    return TRUE;
}


Boolean LIBCALL EditRecordTAXGIByGI(StTaxgiPtr ptg, Boolean overwrite)
{
    StTaxgiPtr ptgtemp = NULL;
	Char stmt[250];
    CharPtr update_stmt = "UPDATE seqhound.taxgi set taxid = ?, kloodge = ?, type = ? where gi = ?";
    Char type[15];
    Int4 gi,taxid,kloodge,typelen;
    SQLHANDLE hstmt_update;

   if(ptg->gi == 0)
     {
       ErrPostEx(SEV_ERROR,0,0, "EditRecordTAXGIByGI: Invalid parameters.");
       return FALSE;
     }
    ptgtemp = StTaxgiNew();
    ptgtemp->gi = ptg->gi;
    SearchTAXGI(&ptgtemp);
    	
    if(ptgtemp->taxid <=0){
	  ErrPostEx(SEV_ERROR,0,0,"EditRecordTAXGIByGi: no record fot gi %ld",ptg->gi);
	  return FALSE;
	}
	if(overwrite)  /* replace all values except GI but only it the replacement values are valid*/
     {
         CreateHandle(&hstmt_update, update_stmt,&henv,&hdbc);

	    /*set up the input parameters */
	
		SQLBindParameter(hstmt_update, 1, SQL_PARAM_INPUT, SQL_C_LONG,SQL_INTEGER, 0, 0, &taxid, 0, NULL);
		SQLBindParameter(hstmt_update, 2, SQL_PARAM_INPUT, SQL_C_LONG,SQL_INTEGER, 0, 0, &kloodge, 0, NULL);
		SQLBindParameter(hstmt_update, 3, SQL_PARAM_INPUT, SQL_C_DEFAULT,SQL_VARCHAR, 20, 0, type, 0, &typelen);
		SQLBindParameter(hstmt_update, 4, SQL_PARAM_INPUT, SQL_C_LONG,SQL_INTEGER, 0, 0, &gi, 0, NULL);

		gi = ptg->gi;
		taxid = ptg->taxid;
		kloodge = ptg->kloodge;
		sprintf(type,"%s",IsNullStr(ptg->type));
		typelen = strlen(type);
		
		  /* execute statment */
		if (SQLExecute (hstmt_update) != SQL_SUCCESS){
			ErrPostEx(SEV_ERROR,0,0, "EditRecordTAXGIByGi: unable to execute TAXGI update statement. ");
			print_err(hstmt_update);
    			SQLFreeStmt(hstmt_update, SQL_DROP);

			return FALSE;
		}


     }
     else  /* replace only if no value in the database*/
     {
		 if((ptgtemp->taxid == 0) && (ptg->taxid != 0))  {
		   /*printf("1\n");*/
		   sprintf(stmt, "UPDATE seqhound.taxgi set taxid=%ld where gi= %ld ",ptg->taxid,ptg->gi);
			execute_sql(stmt,&henv,&hdbc);
		 }
        	if((ptgtemp->kloodge == 0) && (ptg->kloodge != 0)) {
			sprintf(stmt, "UPDATE seqhound.taxgi set kloodge=%ld where gi= %ld ",ptg->kloodge,ptg->gi);
			execute_sql(stmt,&henv,&hdbc);
		 }
	        if((ptgtemp->type == NULL) && (ptg->type != NULL)){
			sprintf(stmt, "UPDATE seqhound.taxgi set type='%s' where gi= %ld ",IsNullStr(ptg->type),ptg->gi  );
			execute_sql(stmt,&henv,&hdbc);
		 }
     }

    StTaxgiFree(ptgtemp);
    return TRUE;
}


/* returns a ValNode list with data.intvalue = kloodge of each DNA unit of completed genomes */
ValNodePtr LIBCALL SHoundAllKloodge(void)
{
   ValNodePtr pvnTax = 0;
   CharPtr search_kloodge ="SELECT chromid from seqhound.chrom"; 
   SQLHANDLE hstmt;
   Int2 sqlrc=SQL_SUCCESS;
   struct{
     SQLINTEGER len;
     SQLINTEGER val;
   }kloodge;

   CreateHandle(&hstmt,search_kloodge, &hdbc,&henv);

  /* set auto commit on */
  SQLSetConnectAttr( hdbc, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_ON, SQL_NTS);  

  /* execute statment */
  if (SQLExecute(hstmt) != SQL_SUCCESS){
    ErrPostEx(SEV_ERROR,0,0,"SHoundAllKloodge:unable to execute select statement.");
    print_err(hstmt);
    SQLFreeStmt(hstmt, SQL_DROP);
    return NULL;
  }

  SQLBindCol(hstmt,1,SQL_C_LONG,&kloodge.val,0,&kloodge.len);  
  
  sqlrc = SQLFetch(hstmt);
  if(sqlrc == SQL_NO_DATA_FOUND){
    ErrPostEx(SEV_INFO, 0, 0, "No data found");
     SQLFreeStmt(hstmt, SQL_DROP) ;
    return NULL;
  }

  while(sqlrc == SQL_SUCCESS || sqlrc == SQL_SUCCESS_WITH_INFO){     
	 if(!ValNodeIsIn(kloodge.val, pvnTax))
       pvnTax = Slri_ValNodeAddInt(&pvnTax, 0, kloodge.val);
     sqlrc = SQLFetch(hstmt);
  }

   /* Free statement handle. */
   if (SQLFreeHandle(SQL_HANDLE_STMT, hstmt) != SQL_SUCCESS){
     print_err(hstmt);
     return NULL;
   }
   return pvnTax;
}

Boolean LIBCALL EditRecordDOMDBByDOMID(StDomdbPtr pdd, Boolean overwrite)
{
  return FALSE;
}


/*
 $Log: intrez_db2.c,v $
 Revision 1.2  2004/01/06 22:32:15  skuznets
 Code audit has been done sucessfully.
 Possible NULL pointer issues has been eliminated.

 Revision 1.1.1.13  2003/07/26 08:42:02  root
 Automated import from Sourceforge.net

 Revision 1.15  2003/07/25 15:18:29  haocl
 changed AppendPUBSEQ prototype to prevent compile errors, but did not change functionality.

 Revision 1.14  2002/09/27 16:45:31  kaca
 added dummy function to mirror  read only mode for SHound initialization in CodeBase

 Revision 1.13  2002/08/12 19:18:40  mdumonti
 added SearchSENGI code

 Revision 1.12  2002/05/15 18:33:45  risserlin
 added freestamtemetn for when there is no data

 Revision 1.11  2002/04/10 16:23:09  risserlin
 fixed accdb retrieval of names in db2

 Revision 1.10  2002/03/03 23:02:52  gbader
 Fixed MSVC compiler warnings - mostly unused variables and uncasted type changes.

 Revision 1.9  2002/02/28 17:23:01  micheld
 New MSVC project files to build cb/db2/rem apps and libraries
 Moved include order to accomodate msvc, removed #ifdef [CODEBASE|DB2]

 Revision 1.8  2002/02/12 14:57:34  mdumontier
 fixed all ErrPostEx messages to be accurate

 Revision 1.7  2002/02/10 18:45:19  mdumontier
 compiler warnings, and added AppendRecordCDDB

 Revision 1.6  2002/02/07 00:24:48  mdumontier
 bug fixes, added 2 db2 specific functions for ***TaxID

 Revision 1.5  2002/02/06 14:26:56  mdumontier

 minor bug fixes:

 Revision 1.4  2002/01/09 18:52:39  risserlin
 fixed running out of handles when searching redund over and over again.

 Revision 1.3  2002/01/03 20:35:44  risserlin
 updated version of db2 layer compatible with db2 EEE

 Revision 1.2  2002/01/03 20:31:21  risserlin
 updated version of db2 layer compatible with db2 EEE

*/
