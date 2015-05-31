/** $Id: prebind_import.c,v 1.3 2004/05/26 21:52:00 kzheng Exp $  
***************************************************************************
*
*  Pre Biomolecular Interaction Network Database (PreBIND)
*
*  File: prebind_searcher.c
*  Description: This program searches MedLine for names of proteins and
*               their encoding loci.  This program uses names obtained by
*                 pb_parser.c from the ncbi refseq database release file 
*               (rscu.bna).  MedLine search results are stored in the
*               PreBIND database and are analyzed for the co-occurence of names
*                 in the same abstract.  Abstracts and pairs of potentially
*                 interacting proteins are scored by a separate program (Textomy)
*                 for the liklihood that an interaction is talked about. 
*                 pb_searcher can import these scores into the PreBIND database.  
*                 
*
*  Authors: Ian Donaldson (ian.donaldson@utoronto.ca)
*             and Christopher W.V. Hogue (hogue@mshri.on.ca)
*
*  Date: June 2002
*
*  Hogue Lab - University of Toronto Biochemistry Department
*  Samuel Lunenfeld Research Institute, Mount Sinai Hospital
*
*  REVISION/CONTRIBUTION NOTES:
*
*  June 1, 2000  Original MSH Revision.
*              Hogue Lab - University of Toronto Biochemistry Department 
*              and the Samuel Lunenfeld Research Institute, Mount Sinai Hospital
*              http://bioinfo.mshri.on.ca  hogue@mshri.on.ca
*               ftp://ftp.mshri.on.ca/
*
*
*  Copyright Notice:
*
*  Copyright ©2000 Mount Sinai Hospital (MSH)
*
*  This program is free software; you can redistribute it and/or
*  modify it under the terms of the GNU General Public License as
*  published by the Free Software Foundation; either version 2 of
*  the License, or any later version.
*
*  This program is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*
*  See the GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with this program; if not, write to the
*         Free Software Foundation, Inc.,
*         59 Temple Place, Suite 330, Boston, MA
*         02111-1307  USA
*  or visit http://www.gnu.org/copyleft/gpl.html
*
*  SPECIAL EXCEPTIONS
*
*  As a special exception, Mount Sinai Hospital gives permission to
*  link this program with the following non-GPL programs or libraries,
*  and distribute the resulting executable, without including the source
*  code for these in the source distribution:
*
*  a) CodeBase 6.5 or greater from Sequiter Software Inc.
*
*  b) The C or C++ interface to Oracle 8.x or greater, from
*     Oracle Corporation or IBM DB2 UDB.
*
*****************************************************************************
*/

/*****************************************************
*
* This is a command line program used to 
* import searcher tables from Mysql dump flat files
* for classic prebind cgi
* Command syntax:
*    ./prebind_import [-c T/F] [-table_option table_flatfile]
*
********************************************************/

#ifndef _PREBIND_DB_
#include "prebind_db.h"
#endif  


#ifndef _PREBIND_COMMON_
#include "prebind_common.h"
#endif


#ifndef _PREBIND_LIB_
#include "prebind_lib.h"
#endif

#include <ncbi.h>
#include <accentr.h>
#include <accutils.h>
#include <tofasta.h>
#include <tomedlin.h>
#include <asn2ff.h>

/**
*****************************************************
* set up command line arguments
********************************************************/
#define NUMARGS 6 
#define MAXLINE 500
Args myargs[NUMARGS] = 
{
    {"Create databases if they do not already exist",          "F",     NULL, NULL, TRUE,  'c', ARG_BOOLEAN, 0.0, 0, NULL},
    {"Import RESULT Table from a tab deliminated text file",   NULL,    NULL, NULL, TRUE,  'R', ARG_STRING,  0.0, 0, NULL},
    {"Import ISVM Table from a tab deliminated text file",     NULL,    NULL, NULL, TRUE,  'I', ARG_STRING,  0.0, 0, NULL},
    {"Import CORE Table from a tab deliminated text file ",    NULL,    NULL, NULL, TRUE,  'C', ARG_STRING,  0.0, 0, NULL},
	{"Import REGI  Table from a tab deliminated text file",    NULL,    NULL, NULL, TRUE,  'E', ARG_STRING,  0.0, 0, NULL},
	{"Import POINT  Table from a tab deliminated text file",   NULL,    NULL, NULL, TRUE,  'P', ARG_STRING,  0.0, 0, NULL},
};


/**
*****************************************************
* 
*  shout off indexing for codebase when importing tables
*
********************************************************/

void setCodeBaseFlagsExclNoidx()
{
	codeBase.accessMode=OPEN4DENY_RW;
    codeBase.autoOpen=0;
}

/**
*****************************************************
*
* ImportResultTable:    this function imports            
*     search result of names against PUBMED from a text file    
*     (specified by filename) into the RESULT database.
*
********************************************************/

Int2 ImportResultTable(CharPtr filename)
{  
    /*declarations and initializations*/
    FILE     *pFile = NULL;
    Char     line_buffer[MAXLINE];
    CharPtr  cur_line_ptr = NULL;
    
	CharPtr  gip_ptr=NULL;
    CharPtr  pmid_ptr=NULL;
    Int4     gip = 0;
    CharPtr  name= NULL;
    Int4     pmid = 0;
    
	Int2     append_status = 0;
    Int4     results_appended=0;
    Int4     result_already_found=0;
    Int4     append_error=0;
    Char     daytime[100];

	printf("ImportResultTable: create=%d , filename=%s\n",myargs[0].intvalue,  filename );

    /* create table with index first and close it */
    if (myargs[0].intvalue!=0)
    {  
        /* if table exists, exit the program */
		printf("open table normly.\n");
		if(!OpenRESULT("RESULT", (Boolean)myargs[0].intvalue)){
            printf("OpenTables: Create table failed, probably already exists!");
            return 1;
        }
		printf("Table opened normly. \n");
		/* close the table and reopen using no index to allow reindexing
		   avoid open two index instance */
		if(d4close(pd4_RESULT) != r4success) return 1;
    }
	setCodeBaseFlagsExclNoidx();				

	if(!OpenRESULTNoidx("RESULT")){
        printf("OpenTables: OpenRESULT no indexing failed.");
        return 1;   
    }
		printf("Table opened without indexing. \n");

    if ((pFile=FileOpen(filename, "r")) == NULL)
    {
        printf("ImportResult: Failed to open file: %s.  ", filename);
        return 1;
    }

    DayTimeStr(daytime, TRUE, TRUE);
    printf("Start import RESULT table from . [%s].\n",daytime);

	cur_line_ptr = fgets(line_buffer, (size_t)MAXLINE, pFile);
    while (cur_line_ptr)
    {
		if(strchr(cur_line_ptr, '\t')!= NULL){
			/*line is split by  tabs*/
			gip_ptr =  strtok(cur_line_ptr, "\t");
			name = strtok(NULL, "\t");
			pmid_ptr =  strtok(NULL, "\t");
			
			gip = (Int4)atoi(gip_ptr);
			pmid = (Int4)atoi(pmid_ptr);

			/* insert record into RESULT.dbf */
			d4appendStart(pd4_RESULT, 0);
			d4blank(pd4_RESULT);
			
			f4assignLong(pf4_RESULT_GIP, (long)gip); 
			f4memoAssign(pf4_RESULT_NAME, name);
			f4assignLong(pf4_RESULT_PMID, (long)pmid);
			
			append_status = d4append(pd4_RESULT);
			switch(append_status)
			{
				case r4success: 
					results_appended++;
					break;
				case r4unique:
					result_already_found++;
					break;
				default:
					printf( "ImportResult failed to append record. append_status = %d", append_status);
					append_error++;
			}
		}
        cur_line_ptr = fgets(line_buffer, (size_t)MAXLINE, pFile);
    }
    printf("\n==\nappended= %ld; \tpresent= %ld; error= %ld\n\n", (long) results_appended, (long) result_already_found, (long)append_error);

    /*close the file*/
    FileClose(pFile);

    DayTimeStr(daytime, TRUE, TRUE);
    printf("Finished importing table. [%s]\n",  daytime);
 
    printf("Start rebuilding index ... [%s]\n",  daytime);
	if(ReindexRESULT()==FALSE){
        printf("reindex table error \n");
        return 1;
    }
	
    DayTimeStr(daytime, TRUE, TRUE);
    printf("Finished rebuilding index. [%s]\n",  daytime);

    /* close RESULT table */
    if(d4close(pd4_RESULT) != r4success) return 1;
    
    return 0;
}

/**
*****************************************************
* use old table for now
*
* ImportIsvmTable:    this function imports            
*     support vector machine scores from a text file    
*     (specified by filename) into the ISVM database.
*
********************************************************/

Int2 ImportIsvmTable(CharPtr filename){
	
	FILE     *pFile =            NULL;
    Char     line_buffer[MAXLINE];
    CharPtr  cur_line_ptr =        NULL;
    
	CharPtr  pmid_ptr =            NULL;
    CharPtr  iscore_ptr =        NULL;
    
	Int4     pmid =                0;
    FloatHi  iscore =              0.0;
    
	Int2    append_status =            0;
  
	printf("ImportIsvmTable: create=%d , filename=%s\n",myargs[0].intvalue,  filename );

    /* create table with index first and close it */
    if (myargs[0].intvalue!=0)
    {  
        /* if table exists, exit the program */
		printf("open table normly.\n");
		if(!OpenISVM("ISVM", (Boolean)myargs[0].intvalue)){
            printf("OpenTables: Create table failed, probably already exists!");
            return 1;
        }
		printf("Table opened normly. \n");
		if(d4close(pd4_ISVM) != r4success) return 1;
    }
	setCodeBaseFlagsExclNoidx();

	if(!OpenISVMNoidx("ISVM")){
        printf("OpenTables: OpenISVM no indexing failed.");
        return 1;   
    }
		printf("Table opened without indexing. \n");

    /*open the point scores file*/
    if ((pFile = FileOpen(filename, "r")) == NULL)
    {
        printf("ImportIsvm: ISVM file failed to open.  Check the path to the file: %s.\n", filename);
        return 1;
    }
    
	cur_line_ptr = fgets(line_buffer, (size_t)MAXLINE, pFile);
    while (cur_line_ptr)
    {
        if(strchr(cur_line_ptr, '\t')!= NULL){
		/*divide the line by the "\t" token*/
        pmid_ptr =  strtok(cur_line_ptr, "\t");/*PMID*/
        strtok(NULL, "\t");     /*MUID*/
        iscore_ptr = strtok(NULL, "\t");
        
        pmid = (Int4)atoi(pmid_ptr);
        iscore = (FloatHi)atof(iscore_ptr);

        /*write the interaction to the database*/
        d4appendStart(pd4_ISVM, 0);
        d4blank(pd4_ISVM);
		
        f4assignLong (pf4_ISVM_PMID, (long) pmid);
        f4assignLong (pf4_ISVM_MUID, 0);
		f4assignDouble (pf4_ISVM_ISCORE, iscore);
		f4assignDouble (pf4_ISVM_ISCORE2, 0.0);
        f4assignLong (pf4_ISVM_OPNO, 0);
        f4assignLong (pf4_ISVM_OPYES, 0);
        f4assignLong (pf4_ISVM_ITRUTH, 0);

        append_status = d4append(pd4_ISVM);


        switch (append_status)
        {
        case r4success:
            break; /*ok result appended*/
        default:
            ErrPostEx(SEV_ERROR,0,0, "BuildISVM failed to append pmid = %ld. append_status = %d", (long) pmid, append_status);
            return 2;
        }
		}

        /*get the next line and reset values*/
        cur_line_ptr = fgets(line_buffer, (size_t)MAXLINE, pFile);
    }
	

    /*close the file*/
    FileClose(pFile);
    
    if(ReindexISVM()==FALSE){
        printf("reindex ISVM table error !\n");
        return 1;
    }
    else{
        printf("reindex ISVM table successfully \n");
    }
    return 0;
}

/**
*****************************************************
*
* ImportCoreTable:    this function imports            
*      COunt REsult table from a text file    
*     (specified by filename) into the CORE database.
*
********************************************************/

Int2 ImportCoreTable(CharPtr filename){
	
	FILE     *pFile =            NULL;
    Char     line_buffer[MAXLINE];
    CharPtr  cur_line_ptr =        NULL;
    
	CharPtr  pmid_ptr =            NULL;
    CharPtr  numn_ptr =            NULL;
    CharPtr  numg_ptr =            NULL;
    
	Int4     pmid =                0;
    Int4     numn =                0;
    Int4     numg =                0;
    
	Int2    append_status =            0;
  
	printf("ImportCoreTable: create=%d , filename=%s\n",myargs[0].intvalue,  filename );

    /* create table with index first and close it */
    if (myargs[0].intvalue!=0)
    {  
        /* if table exists, exit the program */
		printf("open table normly.\n");
		if(!OpenCORE("CORE", (Boolean)myargs[0].intvalue)){
            printf("OpenTables: Create table failed, probably already exists!");
            return 1;
        }
		printf("Table opened normly. \n");
		if(d4close(pd4_CORE) != r4success) return 1;
    }
	setCodeBaseFlagsExclNoidx();

	if(!OpenCORENoidx("CORE")){
        printf("OpenTables: OpenCORE no indexing failed.");
        return 1;   
    }
		printf("Table opened without indexing. \n");

    /*open the point scores file*/
    if ((pFile = FileOpen(filename, "r")) == NULL)
    {
        printf("ImportCore: CORE file failed to open.  Check the path to the file: %s.\n", filename);
        return 1;
    }
    
	cur_line_ptr = fgets(line_buffer, (size_t)MAXLINE, pFile);
    while (cur_line_ptr)
    {
        if(strchr(cur_line_ptr, '\t')!= NULL){
		/*divide the line by the "\t" token*/
        pmid_ptr =  strtok(cur_line_ptr, "\t");/*PMID*/
        strtok(NULL, "\t");     /*MUID*/
        numn_ptr = strtok(NULL, "\t");/*GIA*/
        numg_ptr = strtok(NULL, "\t"); /*NAMB*/
        
        pmid = (Int4)atoi(pmid_ptr);
        numn = (Int4)atoi(numn_ptr);
        numg = (Int4)atoi(numg_ptr);

        /*write the interaction to the database*/
        d4appendStart(pd4_CORE, 0);
        d4blank(pd4_CORE);

        f4assignLong (pf4_CORE_PMID, (long) pmid);
        f4assignLong (pf4_CORE_MUID, 0);
		f4assignInt  (pf4_CORE_NUMN, numn);
        f4assignInt  (pf4_CORE_NUMG, numg);

        append_status = d4append(pd4_CORE);


        switch (append_status)
        {
        case r4success:
            break; /*ok result appended*/
        default:
            ErrPostEx(SEV_ERROR,0,0, "BuildCORE failed to append pmid = %ld. append_status = %d", (long) pmid, append_status);
            return 2;
        }
		}

        /*get the next line and reset values*/
        cur_line_ptr = fgets(line_buffer, (size_t)MAXLINE, pFile);
    }
	

    /*close the file*/
    FileClose(pFile);
    
    if(ReindexCORE()==FALSE){
        printf("reindex CORE table error !\n");
        return 1;
    }
    else{
        printf("reindex CORE table successfully \n");
    }

	return 0;
}


/**
*****************************************************
*
* ImportRegiTable:    this function imports            
*      REsult by GI table from a text file    
*     (specified by filename) into the REGI database.
*
********************************************************/

Int2 ImportRegiTable(CharPtr filename){ 
	FILE     *pFile =            NULL;
    Char     line_buffer[MAXLINE];
    CharPtr  cur_line_ptr =        NULL;
    
	CharPtr  pmid_ptr =            NULL;
    CharPtr  gip_ptr =            NULL;
    
	Int4     pmid =                0;
    Int4     gip =                0;
    
	Int2    append_status =            0;
  
	printf("ImportRegiTable: create=%d , filename=%s\n",myargs[0].intvalue,  filename );

    /* create table with index first and close it */
    if (myargs[0].intvalue!=0)
    {  
        /* if table exists, exit the program */
		printf("open table normly.\n");
		if(!OpenREGI("REGI", (Boolean)myargs[0].intvalue)){
            printf("OpenTables: Create table failed, probably already exists!");
            return 1;
        }
		printf("Table opened normly. \n");
		if(d4close(pd4_REGI) != r4success) return 1;
    }
	setCodeBaseFlagsExclNoidx();

	if(!OpenREGINoidx("REGI")){
        printf("OpenTables: OpenREGI no indexing failed.");
        return 1;   
    }
		printf("Table opened without indexing. \n");

    /*open the point scores file*/
    if ((pFile = FileOpen(filename, "r")) == NULL)
    {
        printf("ImportRegi: REGI file failed to open.  Check the path to the file: %s.\n", filename);
        return 1;
    }
    
	cur_line_ptr = fgets(line_buffer, (size_t)MAXLINE, pFile);
    while (cur_line_ptr)
    {
        if(strchr(cur_line_ptr, '\t')!= NULL){
		/*divide the line by the "\t" token*/
        pmid_ptr =  strtok(cur_line_ptr, "\t");/*PMID*/
        strtok(NULL, "\t");     /*MUID*/
        gip_ptr = strtok(NULL, "\t");/*GI*/
        
        pmid = (Int4)atoi(pmid_ptr);
        gip = (Int4)atoi(gip_ptr);

        /*write the interaction to the database*/
        d4appendStart(pd4_REGI, 0);
        d4blank(pd4_REGI);

        f4assignLong (pf4_REGI_PMID, (long) pmid);
        f4assignLong (pf4_REGI_MUID, 0);
        f4assignLong (pf4_REGI_GIP,  (long) gip);

        append_status = d4append(pd4_REGI);


        switch (append_status)
        {
        case r4success:
            break; /*ok result appended*/
        default:
            ErrPostEx(SEV_ERROR,0,0, "BuildREGI failed to append pmid = %ld. append_status = %d", (long) pmid, append_status);
            return 2;
        }
		}

        /*get the next line and reset values*/
        cur_line_ptr = fgets(line_buffer, (size_t)MAXLINE, pFile);
    }
	

    /*close the file*/
    FileClose(pFile);
    
    if(ReindexREGI()==FALSE){
        printf("reindex REGI table error !\n");
        return 1;
    }
    else{
        printf("reindex REGI table successfully \n");
    }

	return 0;
}


/**
*****************************************************
*
* ImportPointScores:    this function imports            
*     POtential INTeraction scores from a text file    
*     (specified by filename) into the POINT database.
*     Scores are imported into either piscore (protein
*
********************************************************/

Int2 ImportPointTable(CharPtr filename){
	
	FILE     *pFile =            NULL;
    Char     line_buffer[MAXLINE];
    CharPtr  cur_line_ptr =        NULL;
    
	CharPtr  pmid_ptr =            NULL;
    CharPtr  score_ptr =        NULL;
    CharPtr  gia_ptr =            NULL;
    CharPtr  gib_ptr =            NULL;
	Boolean  record_found =     FALSE;
    
	Int4     pmid =                0;
    Int4     gia =                0;
    Int4     gib =                0;
    CharPtr    nama =  NULL;
    CharPtr    namb =  NULL;
    
	Int2    append_status =            0;
  
	printf("ImportPointTable: create=%d , filename=%s\n",myargs[0].intvalue,  filename );

    /* create table with index first and close it */
    if (myargs[0].intvalue!=0)
    {  
        /* if table exists, exit the program */
		printf("open table normly.\n");
		if(!OpenPOINT("POINT", (Boolean)myargs[0].intvalue)){
            printf("OpenTables: Create table failed, probably already exists!");
            return 1;
        }
		printf("Table opened normly. \n");
		if(d4close(pd4_POINT) != r4success) return 1;
    }
	setCodeBaseFlagsExclNoidx();

	if(!OpenPOINTNoidx("POINT")){
        printf("OpenTables: OpenPOINT no indexing failed.");
        return 1;   
    }
		printf("Table opened without indexing. \n");

    /*open the point scores file*/
    if ((pFile = FileOpen(filename, "r")) == NULL)
    {
        printf("ImportPoint: POINT file failed to open.  Check the path to the file: %s.\n", filename);
        return 1;
    }
    
	cur_line_ptr = fgets(line_buffer, (size_t)MAXLINE, pFile);
    while (cur_line_ptr)
    {
        if(strchr(cur_line_ptr, '\t')!= NULL){
		/*divide the line by the "\t" token*/
        pmid_ptr =  strtok(cur_line_ptr, "\t");/*PMID*/
        strtok(NULL, "\t");     /*MUID*/
        gia_ptr = strtok(NULL, "\t");/*GIA*/
        nama=strtok(NULL, "\t");/*NAMA*/
        strtok(NULL, "\t");/*ATYPE*/
        gib_ptr = strtok(NULL, "\t");/*GIB*/
        namb=strtok(NULL, "\t"); /*NAMB*/
        /* printf("%s\t%s\t%s\t%s\t%s\n", pmid_ptr, gia_ptr,nama, gib_ptr, namb);*/
        
        pmid = (Int4)atoi(pmid_ptr);
        gia = (Int4)atoi(gia_ptr);
        gib = (Int4)atoi(gib_ptr);

        /*write the interaction to the database*/
        d4appendStart(pd4_POINT, 0);
        d4blank(pd4_POINT);

        f4assignLong (pf4_POINT_PMID, (long) pmid);
        f4assignLong (pf4_POINT_MUID, 0);

        f4assignLong (pf4_POINT_GIA,  (long) gia);
        f4assign     (pf4_POINT_NAMA, nama);
        f4assignInt  (pf4_POINT_ATYPE, 0); 

        f4assignLong (pf4_POINT_GIB,  (long) gib);
        f4assign     (pf4_POINT_NAMB, namb);
        f4assignInt  (pf4_POINT_BTYPE, 0);

        f4assignInt     (pf4_POINT_OPNO, 0);
        f4assignInt     (pf4_POINT_OPYES, 0);
                          
        f4assignInt     (pf4_POINT_PISCORE, 0);            
        f4assignInt     (pf4_POINT_PISCORE2, 0);                              
        f4assignInt     (pf4_POINT_PITRUTH, 0);

        append_status = d4append(pd4_POINT);


        switch (append_status)
        {
        case r4success:
            break; /*ok result appended*/
        default:
            ErrPostEx(SEV_ERROR,0,0, "BuildPOINT failed to append pmid = %ld. append_status = %d", (long) pmid, append_status);
            return 2;
        }
		}

        /*get the next line and reset values*/
        cur_line_ptr = fgets(line_buffer, (size_t)MAXLINE, pFile);
    }
	

    /*close the file*/
    FileClose(pFile);
    
    if(ReindexPOINT()==FALSE){
        printf("reindex POINT table error !\n");
        return 1;
    }
    else{
        printf("reindex POINT table successfully \n");
    }
    return 0;
}

/**
*****************************************************
*                                                   
* MAIN:                                             
*                                                   
* @param                                       
*     command line                                  
* @return                                          
*    0    ok           
*    1    error
********************************************************/


Int2 Main(void)
{  
    CharPtr filename = NULL;
    Char     daytime[100];
    
    DayTimeStr(daytime, TRUE, TRUE);
    printf("== PreBIND import starts. [%s]==\n",  daytime);
    
    /* check command line arguments (see NCBI toolkit corelib) */
    if (!GetArgs("prebind_import", NUMARGS, myargs))
    {
         printf("Main: can not get command line arguments\n");
          return 1;
    }
    else printf("Finished getting arguments\n");

        /*initialize codeBase*/
    if (!InitCodeBaseForPreBIND())
    {
        printf( "Main: InitCodeBaseForPreBIND failed.");
        error4exitTest(&codeBase); /*quits program */
        return 1;
    }
    	/* setCodeBaseFlagsExclNoidx */


	/*
    else 
    {
        codeBase.accessMode=OPEN4DENY_RW;
        codeBase.autoOpen=0;
        printf("Finished init codebase with ExclNoindex.\n");
    }*/
    
    /*import RESULT table from text file and then quit*/
    if ( myargs[1].strvalue != NULL)
    {
        filename = myargs[1].strvalue;
        /*cycle through the RESULT file and place info in the RESULT database*/
        if (ImportResultTable(filename) != 0)
        {
            printf( "Import RESULT failed.");
            return 1;
        }
        printf("Import RESULT Finished.\n");
    }
	else if (myargs[2].strvalue != NULL){
		filename = myargs[2].strvalue;
        if (ImportIsvmTable(filename) != 0)
        {
            printf( "Import ISVM failed.");
            return 1;
        }
        printf("Import ISVM Finished.\n");
    }
	else if (myargs[3].strvalue != NULL){
		filename = myargs[3].strvalue;
        if (ImportCoreTable(filename) != 0)
        {
            printf( "Import CORE failed.");
            return 1;
        }
        printf("Import CORE Finished.\n");
    }
	else if (myargs[4].strvalue != NULL){
		filename = myargs[4].strvalue;
        if (ImportRegiTable(filename) != 0)
        {
            printf( "Import REGI failed.");
            return 1;
        }
        printf("Import REGI Finished.\n");
    }
	else if (myargs[5].strvalue != NULL){
		filename = myargs[5].strvalue;
        if (ImportPointTable(filename) != 0)
        {
            printf( "Import POINT failed.");
            return 1;
        }
        printf("Import POINT Finished.\n");
    }
	else{
        printf("Usage: ./prebind_import [-RICEP] [file]\n");
        printf("Options: \n");
        printf("\t-R RESULT table\n");
        printf("\t-I ISVM table\n");
        printf("\t-C CORE table\n");
        printf("\t-E REGI table\n");
        printf("\t-P POINT table\n");
	
    }

    
    /* close codebase */
    code4close(&codeBase);
    code4initUndo(&codeBase);
    DayTimeStr(daytime, TRUE, TRUE);
    printf("== PreBIND import stops. [%s] ==\n",  daytime);

    return 0;
}

/*
 $Log: prebind_import.c,v $
 Revision 1.3  2004/05/26 21:52:00  kzheng
 add import function for ISVM table, add comments

 */
