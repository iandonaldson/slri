/* $Id
***************************************************************************
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
            Katerina Michalickova
            and Christopher Hogue hogue@mshri.on.ca


  REVISION/CONTRIBUTION NOTES:

  June 1, 2001  Original MSH Revision.
                Hogue Lab - University of Toronto Biochemistry
                Department and the Samuel Lunenfeld Research
                Institute, Mount Sinai Hospital
                http://bioinfo.mshri.on.ca  hogue@mshri.on.ca
                ftp://ftp.mshri.on.ca/


  PUBLICATION TO CITE:

  DESCRIPTION
  Gene Ontology flat file parser which supplements LL_GO table contents
  parses gene_association.compugen.Genbank.xls and gene_association.compugen.Swissprot.xls files from
  www.geneontology.org
  note these files do not provide PMID information
  program directly dependent on SeqHound API

****************************************************************************/
#include<ncbi.h>
#include<asn.h>
#include<objslristruc.h>
#include<ll_db.h>
#include<ll_db_p.h>
#include<intrez_db.h>
#include<intrez_misc.h>
#include<seqhound.h>
/*
#include <ncbi.h>
#include <accentr.h>
#include <accutils.h>
#include <tofasta.h>
#include <tomedlin.h>
#include <asn2ff.h>
*/
#define NUMARGS 5
#define MAXLINE 500
CODE4 codeBase;

Args myargs[NUMARGS] = {
    {"Create databases if they do not already exist",            "F",  NULL, NULL, TRUE,  'c', ARG_BOOLEAN, 0.0, 0, NULL},
    {"skip a number of lines first ",                            "0" , NULL, NULL, TRUE,  'z', ARG_INT,     0.0, 0, NULL},
    {"Maximum number of import to perform at one time  ",        "0", NULL, NULL, TRUE,  'm', ARG_INT,     0.0, 0, NULL},
    {"reindex the datatables",                                   "F",  NULL, NULL, TRUE,  'r', ARG_BOOLEAN, 0.0, 0, NULL},
    {"Import Table from a tab deliminated text file",           NULL,   NULL, NULL, TRUE,  'R', ARG_STRING,  0.0, 0, NULL},
};

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
     /*assign the tag pointers*/
      pT4LL_NP_GO = d4tag(pD4LL_GO, "NP_TAG_GO");
      pT4LL_GOID = d4tag(pD4LL_GO, "GOID_TAG");
      pT4LL_PMID = d4tag(pD4LL_GO, "PMID_TAG");
   return TRUE;
}

Boolean MyOpenLL_GO(CharPtr name, Boolean Create)
{
   Char pathdatab[PATH_MAX];

    /*find path name of database (see NCBI toolkit corelib 
    and file "prebind.ini" in windows directory)*/
    pathdatab[0] = '\0';
    GetAppParam("dbxref", "databases", "path", NULL, pathdatab, (size_t)PATH_MAX * (sizeof(Char)));
    StringCat(pathdatab, name);
    /*printf("%s\n", pathdatab);*/

    if (Create == FALSE)
    {
      /*open the datafile or return error*/
        pD4LL_GO = d4open(&codeBase, pathdatab);
        
       if (pD4LL_GO == NULL) return FALSE;
    }
    else
    {
      /*open the datafile or create the datafile or return error*/
        pD4LL_GO = d4open(&codeBase, pathdatab);
        if (pD4LL_GO == NULL) 
            pD4LL_GO = d4create(&codeBase, pathdatab, fieldInfo_GO, tagInfo_GO);
      
       if (codeBase.errorCode) return FALSE;
    }

    /*assign the field pointers*/
    pF4LL_NP_GO  = d4field(pD4LL_GO, "NP_GO" );
    pF4LL_GOID = d4field(pD4LL_GO, "GO_ID"    );
    pF4LL_PMID = d4field(pD4LL_GO, "PMID"    );
    pF4LL_EVICODE = d4field(pD4LL_GO, "EVICODE"    );

    /*assign the tag pointers*/
    pT4LL_NP_GO = d4tag(pD4LL_GO, "NP_TAG_GO");
    pT4LL_GOID = d4tag(pD4LL_GO, "GOID_TAG");
    pT4LL_PMID = d4tag(pD4LL_GO, "PMID_TAG");

    return TRUE;

}

/* create always true for this one */
Boolean LIBCALL OpenLL_GONoidx(CharPtr name)
{
    Char pathdatab[PATH_MAX];
    /*find path name of database (see NCBI toolkit corelib 
    and file "prebind.ini" in windows directory)*/
    pathdatab[0] = '\0';
    GetAppParam("dbxref", "databases", "path", NULL, pathdatab, (size_t)PATH_MAX * (sizeof(Char)));
    StringCat(pathdatab, name);
    
    /* open RESULT without index */
    pD4LL_GO = d4open(&codeBase, pathdatab );
    
    /*assign the field pointers*/
    pF4LL_NP_GO  = d4field(pD4LL_GO, "NP_GO" );
    pF4LL_GOID = d4field(pD4LL_GO, "GO_ID"    );
    pF4LL_PMID = d4field(pD4LL_GO, "PMID"    );
    pF4LL_EVICODE = d4field(pD4LL_GO, "EVICODE"    );
    
    return TRUE;

}



Int2 ImportLL_GOTable(CharPtr filename)
{  
    /*declarations and initializations*/
    FILE     *pFile = NULL;
    Char     line_buffer[MAXLINE];
    CharPtr  cur_line_ptr = NULL;
    
    CharPtr  gi_ptr=NULL;
    CharPtr  go_ptr=NULL;
    CharPtr  pmid_ptr=NULL;
    
    Int4     gi = 0;
    Int4     go =0;
    Int4     pmid = 0;
    CharPtr  evicode= NULL;
    
    Int2     append_status = 0;
    Int4     results_appended=0;
    Int4     result_already_found=0;
    Int4     append_error=0;
    Char     daytime[100];
    
    Int4     count=0;
    Int4     percent=0;
    Int4     step =0;
    Int4     z=0;
    Int4     max=0;
    Int4     i ;

    

    if(!OpenLL_GONoidx("LL_GO")){
        printf("OpenTables: OpenLL_GO no indexing failed.");
        return 1;   
    }
    printf("LL_GO table opened without indexing. \n");

    

    pFile = FileOpen(filename, "r");
    if (pFile == NULL)
    {
        printf("ImportResult: Failed to open file: %s.  ", filename);
        return 1;
    }

    DayTimeStr(daytime, TRUE, TRUE);
    printf("Start importing table. [%s].\n",daytime);

    cur_line_ptr = fgets(line_buffer, (size_t)100, pFile);
    
    if((Int4)myargs[1].intvalue >0 ) z = (Int4)myargs[1].intvalue;
    if((Int4)myargs[2].intvalue >0 ) max = (Int4)myargs[2].intvalue;
    step =(Int4) max/100.0;
    
    printf("start from line [ %ld ]; import [ %ld ] lines; step = [ %ld ] \n", z+1, max, step);
    
    for(i=0; i<z; i++)
    {
        fgets(line_buffer, (size_t)MAXLINE, pFile);
    }
    if(setvbuf(stdout,  NULL, _IONBF, 0))
          printf( "Couldn't turn off stdout buffer\n");
          
    printf("%ld", percent);
    while (cur_line_ptr && count < max)
    {
        /*line is split by  tabs*/
        gi_ptr =  strtok(cur_line_ptr, "\t\n");
        go_ptr =strtok(NULL, "\t\n");     
        pmid_ptr =  strtok(NULL, "\t\n");
        evicode = strtok(NULL, "\t\n");
        
        gi = (Int4)atoi(gi_ptr);
        go = (Int4)atoi(go_ptr);
        pmid = (Int4)atoi(pmid_ptr);
        
        if(gi <= 0){
            printf( "line %d: Invalid gi %d. Record skipped.\n",count+z, gi);
            continue;
        }
        if(go <= 0){
            printf( "line %d: Invalid go id: %d.Record skipped.\n", count+z, go );
            continue;
        }
        /*if(pmid <= 0){
            ErrPostEx(SEV_ERROR, 0, 0, "LL_Append2GO: Invalid pmid on record with NPID: %d.\nRecord skipped.", NPid);
            return LL_ERROR;
        }*/
        if(evicode == NULL){
            printf( "line %d: Invalid E Code . Record skipped.\n", count+z);
            continue;
        }
        /* insert record into RESULT.dbf */
        d4appendStart(pD4LL_GO, 0);
        d4blank(pD4LL_GO);

        f4assignInt(pF4LL_NP_GO, gi);
        f4assignInt(pF4LL_GOID, go);
        f4assignInt(pF4LL_PMID, pmid);
        f4assign(pF4LL_EVICODE, evicode);
                
        append_status = d4append(pD4LL_GO);
        switch(append_status){
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
        
        count++;
        if (count%step==0) {
            percent++;
            printf("\b\b%ld", percent);
        }
        /*get the next line*/
        cur_line_ptr = fgets(line_buffer, (size_t)MAXLINE, pFile);
    }
    printf("\n==\nappended= %ld; \tpresent= %ld; error= %ld\n\n", (long) results_appended, (long) result_already_found, (long)append_error);

    /*close the file*/
    FileClose(pFile);

    DayTimeStr(daytime, TRUE, TRUE);
    printf("Finished importing RESULT table. [%s]\n",  daytime);
 
    if((Boolean)myargs[3].intvalue == TRUE) {
        printf("Start rebuilding index ... [%s]\n",  daytime);
        if(ReindexLL_GO()==FALSE){
            printf("reindex RESULT table error \n");
            return 1;
        };
        DayTimeStr(daytime, TRUE, TRUE);
        printf("Finished rebuilding index. [%s]\n",  daytime);
    }
    /* close RESULT table */
    if(d4close(pD4LL_GO) != r4success) return 1;
    
    return 0;
}

Boolean InitCodeBaseForDBXREF(void)
{
    code4init(&codeBase);
    code4largeOn(&codeBase);                /* CodeBase support for large files*/
    codeBase.errOpen = 0;                   /*prevents error message when datafile fails to open*/
    codeBase.safety = 1;                    /*prevents d4create from overwriting existing files*/
    codeBase.errDefaultUnique = r4unique;   /*prevents adding duplicate records to unique keys*/    
    if (codeBase.errorCode)
        return FALSE; 
    else
       return TRUE;
}

Int2 Main(void)
{  
    CharPtr filename = NULL;
    Char     daytime[100];
    Int4 success = 0;
    
    DayTimeStr(daytime, TRUE, TRUE);
    
    printf("import starts. [%s]\n",  daytime);
    
    /* check command line arguments (see NCBI toolkit corelib) */
    if (!GetArgs("dbxref", NUMARGS, myargs))
    {
         printf("Main: can not get command line arguments\n");
          return 1;
    }
    else printf("Finished getting arguments\n");

    /*initialize codeBase*/
    if (!InitCodeBaseForDBXREF())
    {
        printf( "Main: InitCodeBaseForPreBIND failed.");
        error4exitTest(&codeBase); /*quits program */
        return 1;
    }
    else 
    {
        /* setCodeBaseFlagsExclNoidx */
        codeBase.accessMode=OPEN4DENY_RW;
        codeBase.autoOpen=0;
        printf("Finished init codebase with ExclNoindex.\n");
    }
    
    if ((Boolean) myargs[0].intvalue== TRUE)
    {  
        if(!MyOpenLL_GO("LL_GO", TRUE)){
            printf("OpenTables: OpenLL_GO failed.");
            success=1;
        }
        else{
            printf("LL_GO table created successfully!\n");
            if(d4close(pD4LL_GO) != r4success) success=1;
        }
    }
    
    /*import RESULT table from text file and then quit*/
    if (success==0 && myargs[4].strvalue != NULL)
    {
        filename = myargs[4].strvalue;
        printf("Import RESULT from %s.\n", filename); 

        /*cycle through the RESULT file and place info in the RESULT database*/
        if (ImportLL_GOTable(filename) != 0)
        {
            printf( "Import RESULT failed.");
            success=1;
        }

        /*diagnostic line*/
        printf("Import Finished.\n");
    }
    
    /* close codebase */
    code4close(&codeBase);
    code4initUndo(&codeBase);
    return success;
}

/*
$Log
*/


