/*$Id: prebind_parser.c,v 1.5 2004/01/29 21:56:09 kzheng Exp $*/

/*
-- ***************************************************************************
--
--  Pre Biomolecular Interaction Network Database (PreBIND)
--
--  File: prebind_parser.c
--  Description: This program parses the ncbi refseq database release file 
--               (rscu.bna) for names of protiens and their encoding loci.  
--               These names are inserted into the PreBIND database and are
--               then used by pb_searcher to search the MedLine database
--               for occurences of these names.                 
--
--  Authors: Ian Donaldson (ian.donaldson@utoronto.ca)
--             and Christopher W.V. Hogue (hogue@mshri.on.ca)
--
--  Date: June 2002
--
--  Hogue Lab - University of Toronto Biochemistry Department
--  Samuel Lunenfeld Research Institute, Mount Sinai Hospital
--
--  REVISION/CONTRIBUTION NOTES:
--
--  June 1, 2000  Original MSH Revision.
--              Hogue Lab - University of Toronto Biochemistry Department 
--              and the Samuel Lunenfeld Research Institute, Mount Sinai Hospital
--              http://bioinfo.mshri.on.ca  hogue@mshri.on.ca
--               ftp://ftp.mshri.on.ca/
--
--
--  Copyright Notice:
--
--  Copyright ©2000 Mount Sinai Hospital (MSH)
--
--  This program is free software; you can redistribute it and/or
--  modify it under the terms of the GNU General Public License as
--  published by the Free Software Foundation; either version 2 of
--  the License, or any later version.
--
--  This program is distributed in the hope that it will be useful,
--  but WITHOUT ANY WARRANTY; without even the implied warranty of
--  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
--
--  See the GNU General Public License for more details.
--
--  You should have received a copy of the GNU General Public License
--  along with this program; if not, write to the
--         Free Software Foundation, Inc.,
--         59 Temple Place, Suite 330, Boston, MA
--         02111-1307  USA
--  or visit http://www.gnu.org/copyleft/gpl.html
--
--  SPECIAL EXCEPTIONS
--
--  As a special exception, Mount Sinai Hospital gives permission to
--  link this program with the following non-GPL programs or libraries,
--  and distribute the resulting executable, without including the source
--  code for these in the source distribution:
--
--  a) CodeBase 6.5 or greater from Sequiter Software Inc.
--
--  b) The C or C++ interface to Oracle 8.x or greater, from
--     Oracle Corporation or IBM DB2 UDB.
--
-- ***************************************************************************
*/


/*pb_parser.c*/
/*this is a console application in the PreBIND_parser project made using the MCD build of the NCBI toolkit*/
/*the following libraries have been included in the Project Settings
/*c4lib.lib ncbi.lib ncbimain.obj prebindlib.lib ncbimmdb.lib ncbiobj.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib*/
/*this includes 
the Codebase c4lib.lib
libraries from the MCD build of the NCBI toolkit: ncbi.lib ncbimain.obj
the prebindlib.lib library
libraries from the MCD build of the NCBI toolkit: ncbi.lib ncbimain.obj: ncbimmdb.lib ncbiobj.lib
all of the default libraries that start with a w32 Console Application
and the wsock32.lib
in that order*/

 
#ifndef _PREBIND_PARSER_
#include "prebind_parser.h"
#endif  


/*set up command line arguments*/
#define NUMARGS 1
Args myargs[NUMARGS] = 
{
    {"Create databases if they do not already exist","F",  NULL, NULL, TRUE,  'c', ARG_BOOLEAN, 0.0, 0, NULL},
};


/************************************************************/
/*                                                            
/*    WriteSRCH: Writes data to the SeaRCH database           
/*                                                            
/*    PARAMETERS:                                             
/*        Int4    gip                gi of protein            
/*        CharPtr name            pointer to name of protein  
/*        Int4    source            source of name            
/*        Int4    taxid            taxonomy database id       
/*        CharPtr date            pointer to the date string  
/*    RETURNS:                                                
/*        Int4    0        success                            
/*                1        gi|name pair already present       
/*                2        some other error in appendig       
/*                3        name is not available              
/*                other    less successful                     
/************************************************************/
Int4 WriteSRCH(Int4 gip, CharPtr name, Int4 source, Int4 taxid, CharPtr date)
{

    /*declare variables*/
    Int4    append_status    =    0;
    CharPtr    replace_start    =    NULL;
    Int2    name_length =        0;
    Int2    name_ok =            0;
    Int2    index =                0;
    Int2    digits =            0;
    CharPtr pointer_to_a_space= NULL;

    /*get rid of all the spaces on the name*/
    if(TrimSpaces(name) != 0)
    {
        ErrPostEx(SEV_ERROR,0,0, "WriteSRCH: TrimSpaces failed, possibly passed a null string\n");
        return 7;
    }

    name_length = strlen(name);

    /*do not add names that are 1 character or less*/
    if (name_length<=1)
    {
        return 4; /*name is too short*/
    }

    /*do not add names that are one letter followed only by numbers*/
    if (isalpha(name[0]) != 0) /*first character is a letter*/
    {
        name_ok = 0;
        for (index = 1; index < name_length; ++index)
        {    
            if (isalpha(name[index]) != 0) /*its not a digit*/
            {
                name_ok = 1;
                break;
            }
        }
        if (name_ok != 1)
        {
            return 5;
        }
    }

    /*do not add yeast orf names*/
    /*yeast orf names are now added*/
    /*if (name[0] == 'Y')
    {
        if (isupper(name[1]) != 0) /-uppercase letter-/
        {
            if (name[2] == 'L' || name[2] == 'R')
            {
                for (index = 3; index < (name_length-1); ++index)
                {
                    if ((isdigit(name[index]) != 0) || name[index] == '.' || name[index] == '-')
                    {
                        ++digits;
                        if (digits > 2)
                            return 6;
                    }     
                }
            }
        }
    }
    */

    /*remove "-pending" or "-PENDING" from names*/
    if ((replace_start = strstr(name, "-pending")) != NULL)
    {
        name[replace_start - name] = '\0';
    }
    if ((replace_start = strstr(name, "-PENDING")) != NULL)
    {
        name[replace_start - name] = '\0';
    }

    /*remove "***" from names- asterisks indicate non-official names*/
    if ((replace_start = strstr(name, "***")) != NULL)
    {
        name[replace_start - name] = '\0';
        source = (source * -1); /*indicating a non-official name*/
    }
    

    /*do not add names that are N/A*/
    if (strncmp(name, "N/A", 3) == 0 || strncmp(name, "NO_PRIMARY_LOCUS_NAME", 21) == 0)
    {
        return 3; /*name is not available*/
    }

    /*replace spaces in names with underscores*/
    while ((pointer_to_a_space = strchr(name, ' ')) != NULL)
    {
        *pointer_to_a_space = '_';
    }


    /*try appending the record to the SeaRCH datafile*/
    d4appendStart(pd4_SRCH, 0);
    d4blank(pd4_SRCH);
    
    f4assignInt (pf4_SRCH_GIP, gip);
    f4memoAssign(pf4_SRCH_NAME, name);
    f4assignInt (pf4_SRCH_SOURCE, source);
    f4assignInt (pf4_SRCH_TAXID, taxid);
    f4assign    (pf4_SRCH_SEARCHED, date);    /*all dates are initially set to 10000101 until the search is performed*/
    f4assignInt (pf4_SRCH_STATUS, 0);        /*all searches are initially set as not approved*/
    f4assignInt (pf4_SRCH_NAMERED, 0);        /*the number of times that this name is used to describe a protein in this taxon*/
    f4assignInt (pf4_SRCH_RESULTS, 0);        /*number of search results is 0 until the search is performed*/
    
    append_status = d4append(pd4_SRCH);
    
    switch (append_status)
    {
        case r4success:
            return 0;
        case r4unique:
            return 1; /*the gi|name pair already exists in the database- old info is not overwritten*/
        default:
            ErrPostEx(SEV_ERROR,0,0, "WriteSRCH failed to append record. append_status = %d\n", append_status);
            return 2;
    }
    
    
}/*end of WriteSRCH*/


/************************************************************/
/*                                                            
/*    WriteACC: Writes data to the ACCession     database     
/*                                                            
/*    PARAMETERS:                                             
/*        Int4    gi                gi of protein             
/*        CharPtr acc_ptr            accession of protein     
/*        CharPtr db_ptr            name of database          
/*        CharPtr protname_ptr  name of protein               
/*        Int4    taxid            taxonomy database id       
/*        CharPtr date            pointer to the date string  
/*    RETURNS:                                                
/*        Int4    0        success                            
/*                other    less successful                    
/************************************************************/

Int4 WriteACC(Int4 gi, CharPtr acc_ptr, CharPtr db_ptr, CharPtr protname_ptr, Int4 taxid, CharPtr date)

{
    /*declare variables*/
    int        record    =  0;
    Int4    cur_gi    =  0;
    CharPtr    cur_acc =  NULL;
    Boolean write_flag=TRUE;
    CharPtr na_ptr=  "N/A";
    
    /*select the protein accession tag*/
    d4tagSelect(pd4_ACC, pt4_ACC_ACC);
    
    /*look for the current accession in the datafile*/
    record = d4seek(pd4_ACC, acc_ptr);
    
    /*if you find the acc of the protein then look for the corresponding gi*/
    if (record == 0) /*r4success*/
    {
        write_flag = FALSE; /*the accession number already exists in the database- old info is not overwritten*/
        
        /*check to make sure that the gi in release of GenBank being released is the gi in the current PreBIND database*/
        cur_gi=f4int(pf4_ACC_GI);
        if (gi != cur_gi)
        {
            printf("New gi for accession already in ACC database. gi = %ld replaces gi = %ld for accession = %s\n", gi, cur_gi, acc_ptr);
            ErrPostEx(SEV_INFO, 0, 0, "New gi for accession already in ACC database. gi = %ld replaces gi = %ld for accession = %s", gi, cur_gi, acc_ptr);
            /*replace the gi with the current_gi in all of the PreBIND databases then record the old_gi|new_gi pair in the other_gi database*/
            /*FixGI(gi, cur_gi);*/

        }    
    }        
    
    /*if you dont find a matching entry then add it to the datafile*/
    if (write_flag)     
    {
        d4appendStart(pd4_ACC, 0);
        d4blank(pd4_ACC);
        
        f4assignInt (pf4_ACC_GI,      gi);
        f4assign    (pf4_ACC_ACC,      acc_ptr);
        f4assign    (pf4_ACC_DB,      db_ptr);
        f4assign    (pf4_ACC_LOCUS,   "N/A");
        f4memoAssign(pf4_ACC_PPROD,      protname_ptr);
        f4assignInt (pf4_ACC_TAXID,   taxid);
        f4assign    (pf4_ACC_UPDATED, date);   
        
        d4append(pd4_ACC);
    }
    return 0;
}


/********************************************************/
/*                                                        
/*    WriteLOCUS: Writes data to the LOCUS name database  
/*                                                        
/*    PARAMETERS:                                         
/*        Int4    gin            gi of encoding nucleotide
/*        Int4    floc        from location               
/*        Int4    tloc        to location                 
/*        CharPtr    name_ptr    name of locus ptr        
/*    RETURNS:                                            
/*        Int4    0        success                        
/*                1        unique key already exists      
/*                2        append failed                  
/*                other    less successful                
/********************************************************/

/*info for the LOCUS database is parsed from the nucleotide Bioseq by ParseLocusInfo except for GIP (gi of protein).
GIP is found by the function BuildLOCUS in CDR database for a given set of values for GIN, FROM and TO.
Note that LOSYN has a unique compound key of:
{ "UNIQUE_TAG", "STR(GI_NUC, 20, 0) + STR(FROM, 20, 0) + NAME", 0, r4unique, 0},
to prevent the same name being listed twice for the same location on a nucleotide*/

Int4 WriteLOCUS(Int4 gin, Int4 floc, Int4 tloc, CharPtr name_ptr)
{
    Int4 append_status = 0;

    /*printf("appending %ld\t%ld\t%ld\t%s\n",gin,floc, tloc,name_ptr);*/
    d4appendStart(pd4_LOCUS, 0);
    d4blank(pd4_LOCUS);
    
    f4assignInt(pf4_LOCUS_GIN, gin);
    f4assignInt(pf4_LOCUS_FROM, floc);
    f4assignInt(pf4_LOCUS_TO, tloc);
    f4assign(pf4_LOCUS_NAME, name_ptr);
    
    append_status = d4append(pd4_LOCUS);
    
    switch (append_status)
    {
        case r4success:
            return 0;
        case r4unique:
            return 1; /*the unique key already exists in the database- old info is not overwritten*/
        default:
            ErrPostEx(SEV_ERROR,0,0, "WriteLOCUS failed to append record. append_status = %d\n", append_status);
            return 2;
    }
}


/********************************************************/
/*                                                        
/*    WriteLOSYN: Writes data to the LOcus SYNonymdatabase
/*                                                        
/*    PARAMETERS:                                            
/*        Int4    gin         gi of encoding nucleotide    
/*        Int4    floc        from location                
/*        Int4    tloc        to location                    
/*        CharPtr syn_ptr     locus synonym ptr            
/*    RETURNS:                                          
/*        Int4    0        success                            
/*                1        unique key already exists        
/*                2        error in appending           
/*                other    less successful                 
/********************************************************/
/*info for the LOSYN database is parsed from the nucleotide Bioseq by ParseLocusInfo except for GIP (gi of protein).
GIP is found by the function BuildLocsyn in CDR for a given set of values for GIN, FROM and TO.
Note that LOSYN has a unique compound key of:
{ "UNIQUE_TAG", "STR(GI_NUC, 20, 0) + STR(FROM, 20, 0) + SYN", 0, r4unique, 0},
to prevent the same synonym being listed twice for the same location on a nucleotide*/
Int4 WriteLOSYN(Int4 gin, Int4 floc, Int4 tloc, CharPtr syn_ptr)
{
    Int4 append_status = 0;

    /*printf("appending %ld\t%ld\t%ld\t%s\n",gin,floc, tloc,name_ptr);*/
    d4appendStart(pd4_LOSYN, 0);
    d4blank(pd4_LOSYN);
    
    f4assignInt(pf4_LOSYN_GIN, gin);
    f4assignInt(pf4_LOSYN_FROM, floc);
    f4assignInt(pf4_LOSYN_TO, tloc);
    f4assign(pf4_LOSYN_SYN, syn_ptr);

    append_status = d4append(pd4_LOSYN);
    
    switch (append_status)
    {
        case r4success:
            return 0;
        case r4unique:
            return 1; /*the unique key already exists in the database- old info is not overwritten*/
        default:
            ErrPostEx(SEV_ERROR,0,0, "WriteLOSYN failed to append record. append_status = %d\n", append_status);
            return 2;
    }
}


/********************************************************/
/*                                                        
/*    WriteDB: Writes data to the DataBase of DataBases   
/*    One of idint or idstr is likely to be 0/NULL        
/*    Numerical identifiers are quite often listed as     
/*    strings in Genbank(!)-these are all converted to    
/*    integers by this function and only stored as        
/*    integers.                                           
/*                                                        
/*    PARAMETERS:                                         
/*        Int4    gin            gi of source DNA          
/*        Int4    floc        from location               
/*        Int4    tloc        to location                   
/*        Int4    gip            gi of encoded protein      
/*        CharPtr    dbname_ptr    pointer to database name  
/*        Int4    idint        integer id in database       
/*        CharPtr    idstr        string id in database      
/*    RETURNS:                                            
/*        Int4    0        success                           
/*                1        the unique key already exists    
/*                2        append failed                   
/*                other    less successful                 
/********************************************************/
/*each record in this database is parsed from either/both of two sources:
1) each of the above values is parsed from the BioseqSet (or in rare cases from the protein Bioseq) 
Annotation feature table by ParseCDRInfo  AND/OR
2) each of the above values (except GIP) is parsed from the nucleotide Bioseq (or in rare cases from the BioseSet) 
Annotation feature table by ParseLocusInfo.  The GI of the encoded protein is retrieved from CDR by looking up the 
CDR entry for the GIN, FROM and TO values in the current DB record; this function is performed by the BuildDB function.

note records that are duplicate will not be appended because of the unique compound key:
{ "UNIQUE_TAG", "STR(GI_NUC, 20, 0) + STR(GI_PROT, 20, 0) + DBNAME + STR(IDINT, 20, 0) + IDSTR", 0, r4unique, 0},*/
Int4 WriteDB(Int4 gin, Int4 floc, Int4 tloc, Int4 gip, CharPtr dbname_ptr, Int4 idint, CharPtr idstr)
{
    /*declare variables*/
    Int4    numerical=        0;
    Int4    append_status = 0;
    
    /*printf("appending dbxref\n");*/
    d4appendStart(pd4_DB, 0);
    d4blank(pd4_DB);
    
    f4assignInt(pf4_DB_GIN, gin);
    f4assignInt(pf4_DB_FROM, floc);
    f4assignInt(pf4_DB_TO, tloc);
    f4assignInt(pf4_DB_GIP, gip);
    f4assign   (pf4_DB_DBNAME, dbname_ptr); /*this WriteDB function will not be called if this is NULL*/
    f4assignInt(pf4_DB_IDINT, idint);
    
    if (idstr != NULL)
    {
        /*determine if the string is actually numerical then assign the numerical value to IDINT field*/
        numerical = atoi(idstr);
        if (numerical && numerical !=0)
        {
            f4assignInt(pf4_DB_IDINT, numerical);
        }
        else
        {
            f4assign   (pf4_DB_IDSTR, idstr);
        }    
    }

    append_status = d4append(pd4_DB);
    
    switch (append_status)
    {
        case r4success:
            return 0;
        case r4unique:
            return 1; /*the unique key already exists in the database- old info is not overwritten*/
        default:
            ErrPostEx(SEV_ERROR,0,0, "WriteDB failed to append record. append_status = %d\n", append_status);
            return 2;
    }
}


/********************************************************/
/*                                                        
/*    WriteCDR: Writes data to the CoDing Region database  
/*                                                        
/*    PARAMETERS:                                           
/*        Int4    gi        gi of encoded    protein         
/*        Int4    floc    from location                    
/*        Int4    tloc    to location                        
/*        Int4    gis        gi of source DNA                
/*    RETURNS:                                            
/*        Int4    0        success                        
/*                other    less successful                
/********************************************************/
Int4 WriteCDR(Int4 gi, Int4 floc, Int4 tloc, Int4 gis)
{
    /*declare variables*/
    int        record    = 0;
    Int4    cur_gi    = 0;
    Int4    cur_from  = 0;
    Int4    cur_to    = 0;
    Int4    cur_gis    = 0;
    
    /*select the protein gi tag*/
    d4tagSelect(pd4_CDR, pt4_CDR_GIP);
       
    /*look for the current gi in the datafile*/
    record = d4seekDouble(pd4_CDR, gi);
    
    /*if you find the record then print it out*/
    /*if (record == 0) /*r4success*/
    /*{
        cur_gi    =f4int(pf4_CDR_GIP);
        cur_from=f4int(pf4_CDR_FROM);
        cur_to    =f4int(pf4_CDR_TO);
        cur_gis =f4int(pf4_CDR_GIN);
        printf("gi %ld matches record %ld\t%ld\t%ld\t%ld\n", gi, cur_gi, cur_from, cur_to, cur_gis);     
    }*/
    
    /*if you dont find a matching entry then add it to the datafile*/
    if (record != 0) /*no matching entry found then append it*/
    {
        /*printf("appending; %ld\t%ld\t%ld\t%ld\n", gi, floc, tloc, gis);*/

        d4appendStart(pd4_CDR, 0);
        d4blank(pd4_CDR);
        
        f4assignInt(pf4_CDR_GIP, gi);
        f4assignInt(pf4_CDR_FROM, floc);
        f4assignInt(pf4_CDR_TO, tloc);
        f4assignInt(pf4_CDR_GIN, gis);
        
        d4append(pd4_CDR);
    }  
    return 0;
}


/********************************************************/
/*                                                        
/*    BuildLOCUS: For each name in the LOCUS name database
/*                find and write the gi of the encoded   
/*                protein from the CDR database           
/*                                                       
/*    PARAMETERS:        none                             
/*                                                       
/*    RETURNS:                                           
/*        Int4    0        success                        
/*                other    less successful                
/********************************************************/

Int4 BuildLOCUS(void)
{
   /*declare variables*/
    int        locus_record=        1;
    Int4    cur_gip =            0;
    Int4    cur_gin    =            0;
    Int4    cur_from=            0;
    Int4    cur_to    =            0;
    int        cdr_record=            1;
    Int4    cdr_from=            0;
    Int4    cdr_to=                0;
    Int4    cdr_gip=            0;
    Int4    cdr_records_found = 0;
    Int4    cdr_gip_found =        0;
    
    /*cycle through each entry in the locus database*/
    for (locus_record = d4top(pd4_LOCUS); locus_record == 0; locus_record = d4skip(pd4_LOCUS, 1L))
    {
     /*only get the protein gi for each record if it is not already present*/
        cur_gip = f4int(pf4_LOCUS_GIP);
    if (cur_gip <=0)
    {
         /*read the current record*/
         cur_gin    = f4int(pf4_LOCUS_GIN);
         cur_from= f4int(pf4_LOCUS_FROM);
         cur_to    = f4int(pf4_LOCUS_TO);
         /*printf("Finding protein gi for gin= %ld\tfrom= %ld\tto= %ld\n",cur_gin, cur_from, cur_to);*/
                
         /*select the protein gi tag in the cdr data table and reset the cdr values*/
         d4tagSelect(pd4_CDR, pt4_CDR_GIN);
         cdr_record = 0;
         cdr_from=0;
         cdr_to  =0;
         cdr_gip =0;
                
         /*look for the current gi in the CDR datafile*/
         cdr_records_found = 0;
         cdr_gip_found = 0;
         cdr_record = d4seekDouble(pd4_CDR, cur_gin);

         /*if you find the gi of the nucleotide then look for the 
               corresponding to and from entry- if only one cdr entry is found
           for the cur_gin value then the corresponding cdr_gip 
               value is assumed to be the gi of the protein encoded by the current locus
           regardless of whether or not the cdr_from and cdr_to values match the cur_from and cur_to values of the locus.
           There is a reason for this- if there is only one cdr 
               (coding region) entry for a nucleotide then the coding 
               region is often listed (incorrectly) as being the entire 
               nucleotide (eg 0 to 1450) even though the protein may be encoded
           by a locus that is a subset of the nucleotide (eg 17 to 1435)*/
         while (cdr_record == 0) /*r4success*/
         {
         ++cdr_records_found;
         cdr_from=f4int(pf4_CDR_FROM);
         cdr_to  =f4int(pf4_CDR_TO);
         cdr_gip =f4int(pf4_CDR_GIP);
         if (cur_from == cdr_from  && cur_to == cdr_to)
         {
             /*found a match- cdr_gip is the gi of the protein encoded by the current locus*/
             cdr_gip_found = 1;
             break;
         }
         else
         {
             cdr_record = d4seekNextDouble(pd4_CDR, cur_gin);
         }
         }/*while loop*/    
                
         /*if the gi of the encoded protein was found or if there was only one cdr for the nucleotide then write the gi to the locus database*/
         if ((cdr_gip_found == 1 || cdr_records_found == 1) && (cdr_gip > 0))
         {
         f4assignInt(pf4_LOCUS_GIP, cdr_gip);
         }
         else
         {
         ErrPostEx(SEV_WARNING,0,0, "BuildLOCUS: No protein gi found for gin= %ld\tfrom= %ld\tto= %ld",cur_gin, cur_from, cur_to);
         }
    }/*only if the current LOCUS record does not have a protein gi value*/
            
    }/*for loop*/
    
    return 0;
    
}/*function end*/


/********************************************************/
/*                                                        
/*    BuildLOSYN: For each name in the LOCUS name database
/*                find and write the gi of the encoded    
/*                protein from the CDR database           
/*                                                       
/*    PARAMETERS:        none                              
/*                                                       
/*    RETURNS:                                            
/*        Int4    0        success                           
/*                other    less successful                 
/********************************************************/
Int4 BuildLOSYN(void)
{
    /*declare variables*/
    int        losyn_record=    1;
    Int4    cur_gip =        0;
    Int4    cur_gin    =        0;
    Int4    cur_from=        0;
    Int4    cur_to    =        0;
    int        cdr_record=        1;
    Int4    cdr_from=        0;
    Int4    cdr_to=            0;
    Int4    cdr_gip=        0;
    Int4    cdr_records_found = 0;
    Int4    cdr_gip_found = 0;
       
    /*cycle through each entry in the losyn database*/
    for (losyn_record = d4top(pd4_LOSYN); losyn_record == 0; losyn_record = d4skip(pd4_LOSYN, 1L))
    {
        /*only get the protein gi for each record if it is not already present*/
        cur_gip = f4int(pf4_LOSYN_GIP);
            if (cur_gip <=0)
            {            
                /*read the current record*/
                cur_gin    = f4int(pf4_LOSYN_GIN);
                cur_from= f4int(pf4_LOSYN_FROM);
                cur_to    = f4int(pf4_LOSYN_TO);
                /*printf("Finding protein gi for gin= %ld\tfrom= %ld\tto= %ld\n",cur_gin, cur_from, cur_to);*/
                
                /*select the protein gi tag in the CDR database and reset cdr variables to zero*/
                d4tagSelect(pd4_CDR, pt4_CDR_GIN);
                cdr_record = 0;
                cdr_from=0;
                cdr_to  =0;
                cdr_gip =0;
                
                /*look for the current gi in the CDR datafile*/
                cdr_records_found = 0;
                cdr_gip_found = 0;
                cdr_record = d4seekDouble(pd4_CDR, cur_gin);
                
                /*if you find the gi of the nucleotide then look for the corresponding to and from entry- if no other entries are
                found then you exit from the while loop with the cdr_gip value for the **only** cur_gin value in the cdr database
                regardless of whether or not the cdr_from and cdr_to values match the cur_from and cur_to values of the protein.
                There is a reason for this- if there is only one cdr (coding region) entry for a nucleotide then the coding region 
                is often listed (incorrectly) as being the entire nucleotide (eg 0 to 1450) even though the protein may be encoded
                by a subset of the nucleotide (eg 17 to 1435)*/
                while (cdr_record == 0) /*r4success*/
                {
                    ++cdr_records_found;
                    cdr_from=f4int(pf4_CDR_FROM);
                    cdr_to  =f4int(pf4_CDR_TO);
                    cdr_gip =f4int(pf4_CDR_GIP);
                    if (cur_from == cdr_from  && cur_to == cdr_to)
                    {
                        /*found a match- stop looking*/
                        cdr_gip_found = 1;
                        break;
                    }
                    else
                    {
                        cdr_record = d4seekNextDouble(pd4_CDR, cur_gin);
                    }
                }/*while loop*/    
                
                /*if the gi of the encoded protein was found or if there was only one cdr for the nucleotide then write the gi to the losyn database*/
                if ((cdr_gip_found == 1 || cdr_records_found == 1) && (cdr_gip > 0))
                {
                    f4assignInt(pf4_LOSYN_GIP, cdr_gip);
                }
                else
                {
                    printf("gi for synonym not found");
                    ErrPostEx(SEV_WARNING,0,0, "BuildLOSYN: No protein gi found for gin= %ld\tfrom= %ld\tto= %ld",cur_gin, cur_from, cur_to);
                }
            }/*only if the current LOSYN record does not have a protein gi value*/
    }/*for loop*/
    
    return 0;
    
}/*function end*/


/********************************************************/
/*                                                        
/*    BuildDB:    For each entry in the DataBase database
/*                check that the gi for the protein is   
/*                present or get it from the CDR database
/*                given the gi of the encoding nucleotide   
/*                and "from" and "to" location values.   
/*                                                       
/*    PARAMETERS:        none                              
/*                                                       
/*    RETURNS:                                            
/*        Int4    0        success                           
/*                other    less successful                 
/********************************************************/
Int4 BuildDB(void)
{
    /*declare variables*/
    int        db_record=    1;/**/
    Int4    cur_gip =        0;/**/
    Int4    cur_gin    =        0;/**/
    Int4    cur_from=        0;/**/
    Int4    cur_to    =        0;/**/
    int        cdr_record=        1;/**/
    Int4    cdr_from=        0;/**/
    Int4    cdr_to=            0;/**/
    Int4    cdr_gip=        0;/**/
    Int4    cdr_records_found = 0;
    Int4    cdr_gip_found = 0;
    Int4    newgip =        0;

    
    /*cycle through each entry in the DataBase database*/
    for (db_record = d4top(pd4_DB); db_record == 0; db_record = d4skip(pd4_DB, 1L))
    {
        /*read the current record to see if there is a gi for the protein-if there is not, look for it*/
        cur_gip = f4int(pf4_DB_GIP);
        if (cur_gip == 0)
        {
            /*get gi of nucleotide and from/to location data*/
            cur_gin    = f4int(pf4_DB_GIN);
            cur_from= f4int(pf4_DB_FROM);
            cur_to    = f4int(pf4_DB_TO);
            /*printf("Finding protein gi for gin= %ld\tfrom= %ld\tto= %ld\n",cur_gin, cur_from, cur_to);*/
                 
            /*select the nucleotide gi tag in the cdr data table and reset the cdr values*/
            d4tagSelect(pd4_CDR, pt4_CDR_GIN);
            cdr_record = 0;
            cdr_from=0;
            cdr_to  =0;
            cdr_gip =0;
            
            /*look for the current gi in the CDR datafile*/
            cdr_records_found = 0;
            cdr_gip_found = 0;
            cdr_record = d4seekDouble(pd4_CDR, cur_gin);
            
            /*if you find the gi of the nucleotide then look for the corresponding to and from entry- if no other entries are
            found then you exit from the while loop with the cdr_gip value for the **only** cur_gin value in the cdr database
            regardless of whether or not the cdr_from and cdr_to values match the cur_from and cur_to values of the protein.
            There is a reason for this- if there is only one cdr (coding region) entry for a nucleotide then the coding region 
            is often listed (incorrectly) as being the entire nucleotide (eg 0 to 1450) even though the protein may be encoded
            by a subset of the nucleotide (eg 17 to 1435)*/
            while (cdr_record == 0) /*r4success*/
            {
                ++cdr_records_found;
                cdr_from=f4int(pf4_CDR_FROM);
                cdr_to  =f4int(pf4_CDR_TO);
                cdr_gip = f4int(pf4_CDR_GIP);
                if (cur_from == cdr_from  && cur_to == cdr_to)
                {
                    /*found a match- stop looking*/
                    cdr_gip_found = 1;
                    break;
                }
                else
                {
                    cdr_record = d4seekNextDouble(pd4_CDR, cur_gin);
                }
            }/*while loop*/    
            
            /*if the gi of the encoded protein was found or if there was only one cdr for the nucleotide then write the gi to the Database database*/
            if ((cdr_gip_found == 1 || cdr_records_found == 1) && (cdr_gip > 0))            
            {
                f4assignInt(pf4_DB_GIP, cdr_gip);
                
                /*skip to the next record to check if this record (with the gi inserted) is a duplication of another*/
                db_record = d4skip(pd4_DB, 1L);
                if (db_record == 20) /*d4skip failed because writing the record would create a non-unique key*/
                /*{ "UNIQUE_TAG", "STR(GI_NUC, 20, 0) + STR(GI_PROT, 20, 0) + DBNAME + STR(IDINT, 20, 0) + IDSTR", 0, r4unique, 0},*/
                {
                    f4assignInt(pf4_DB_GIP, 0);/*re-assign zero to gip; this record will be deleted later-see below*/
                }
                else /*d4skip succeeded because db_record == r4success or r4eof*/
                {
                    db_record = d4skip(pd4_DB, -1L);
                }
            }
            else
            {    
                ErrPostEx(SEV_WARNING,0,0, "BuildDB: No protein gi found for gin= %ld\tfrom= %ld\tto= %ld",cur_gin, cur_from, cur_to);
            }
        }/*only if current DB record has no protein gi*/
        
    }/*for loop through each record in the DB database*/

    /*get rid of all the records with a zero value for gip*/
    for (db_record = d4top(pd4_DB); db_record == 0; db_record = d4skip(pd4_DB, 1L))
    {
        cur_gip = f4int(pf4_DB_GIP);
        if (cur_gip == 0)
        {
                    d4delete(pd4_DB);
        }
    }

    /*before quitting pack the DB database to retrieve space taken up by deleted records*/
    d4pack(pd4_DB);

    return 0;
    
}/*function end*/


/********************************************************/
/*                                                        
/*    BuildACC:    For each gi in the ACC database        
/*                find and write the locus name of the    
/*                gene that encodes the protein           
/*                                                       
/*    PARAMETERS:        none                             
/*                                                       
/*    RETURNS:                                            
/*        Int4    0        success                        
/*                other    less successful                
/********************************************************/
Int4 BuildACC(void)
{
    /*declare variables*/
    int        acc_record=        1;
    CharPtr cur_locus=        NULL;
    Int4    cur_gi=            0;
    int        locus_record=    1;
    int        losyn_record=   1;
    CharPtr locus_name=        NULL;    
    Int4    notfound=        0;
    
    /*cycle through each entry in the accession database*/
    for (acc_record = d4top(pd4_ACC); acc_record == 0; acc_record = d4skip(pd4_ACC, 1L))
    {
        /*get the gi  and locus name of the protein*/
        cur_gi    = f4int(pf4_ACC_GI);
        cur_locus = f4str(pf4_ACC_LOCUS);
            
        /*only get the locus name for each record if it is not already present*/
        if (strncmp(cur_locus, "N/A", 3) == 0)
        {                
            /*select the protein protein gi tag in the LOCUS database*/
            d4tagSelect(pd4_LOCUS, pt4_LOCUS_GIP);
            
            /*look for the current gi in the LOCUS datafile*/
            locus_record = d4seekDouble(pd4_LOCUS, cur_gi);
            
            /*if found write it to the ACCession database*/
            if (locus_record == 0) /*r4success*/
            {
                locus_name = f4str(pf4_LOCUS_NAME);
                f4assign(pf4_ACC_LOCUS, locus_name);
            }
            else if (locus_record != 0)
            {
                ++notfound;
                ErrPostEx(SEV_WARNING,0,0, "BuildACC: No locus name found for protein gi= %ld notfound= %ld", cur_gi, notfound);
            }
        }/*only if LOCUS filled was filled with N/A*/
            
        /*check to make sure that you found a primary name*/
        cur_locus = f4str(pf4_ACC_LOCUS);
        if (strncmp(cur_locus, "NO_PRIMARY_LOCUS_NAME", 6) == 0)
        {
            /*look for a temporary name under locus synonym*/
            /*select the protein protein gi tag in the LOCus SYNonym database*/
            d4tagSelect(pd4_LOSYN, pt4_LOSYN_GIP);

            /*look for the current gi in the LOCUS datafile*/
            losyn_record = d4seekDouble(pd4_LOSYN, cur_gi);
        
            /*if found write it to the ACCession database*/
            if (losyn_record == 0) /*r4success*/
            {
                locus_name = f4str(pf4_LOSYN_SYN);
                f4assign(pf4_ACC_LOCUS, locus_name);
            }
            else if (losyn_record != 0)
            {
                ErrPostEx(SEV_WARNING,0,0, "BuildACC: No locus primary name or locus synonym found for protein gi= %ld ", cur_gi);
            }
        }/*only if the current ACCession database record has no primary locus name*/

    }/*for loop*/
    
    return 0;
    
}/*function end*/


/********************************************************/
/*                                                        
/*    ImportYeastNames:    this function imports yeast loci
/*                        names into the yeast name        
/*                        database                       
/*    PARAMETERS:                                          
/*        command line                                   
/*        myargs[2].strvalue    filename to be imported   
/*    RETURNS:                                           
/*        0    ok                                           
/*        1    datafile(s) failed to open                  
/********************************************************/
Int2 ImportYeastNames(void)
{  
    /*declarations and initializations*/
    FILE    *pFile =            NULL;
    Char    line_buffer[100];
    CharPtr cur_line_ptr =        NULL;
    CharPtr name_ptr =            NULL;
    CharPtr sgdid_ptr =            NULL;
    Int4    db_record =            1;
    Int4    gip =                0;
    Int4    append_report=        0;

    Char path_to_yeast_names[PATH_MAX];

    /*find path to yeastnames file (see NCBI toolkit corelib 
    and file "prebind.ini" in windows directory)*/
    path_to_yeast_names[0] = '\0';
    GetAppParam("prebind", "parser_input_files", "yeast_names", NULL, path_to_yeast_names, (size_t)PATH_MAX * (sizeof(Char)));
    
    /*open the yeast names file*/
    if ((pFile = FileOpen(path_to_yeast_names, "r")) == NULL)
    {
        ErrPostEx(SEV_ERROR, 0, 0, "ImportYeastNames: File of names failed to open.\n");
        return 1;
    }

    cur_line_ptr = fgets(line_buffer, (size_t)100, pFile);
    while (cur_line_ptr)
    {
        /*divide the line by the "|" token*/
        name_ptr = strtok(cur_line_ptr, "|\n");/*line is split by either pipes or returns*/
        sgdid_ptr = strtok(NULL, "|\n");
        /*printf("name= %s\tsgdid= %s\n",name_ptr, sgdid_ptr);*/

        /*given the sgdid id, get the gi from the Database database*/
        /*select the database id string tag*/
        d4tagSelect(pd4_DB, pt4_DB_IDSTR);

        db_record = d4seek(pd4_DB, sgdid_ptr);
        if (db_record == 0)/*r4success*/
        {
            gip = f4int(pf4_DB_GIP);

            /*add the name to the YEast NAME database*/
            d4appendStart(pd4_YENA, 0);
            d4blank(pd4_YENA);

            f4assignLong(pf4_YENA_GIP, (long) gip);
            f4assign(pf4_YENA_NAME, name_ptr);

            append_report = d4append(pd4_YENA);
            /*if (append_report != 0)
            {
                printf("append_report= %d for gi= %ld and yeast name= %s\n",append_report, gip, name_ptr);
            }*/

        }
        else if (db_record != 0)
        {
            ErrPostEx(SEV_WARNING, 0, 0, "ImportYeastNames: gi not found for sgdid= %s d4seek= %d", sgdid_ptr, db_record);
        }

        /*get the next line*/
        cur_line_ptr = fgets(line_buffer, (size_t)100, pFile);
    }

    /*close the yeast names file*/
    FileClose(pFile);
    
    return 0;
}


/********************************************************/
/*                                                        
/*    BuildSRCH:    For each gi in the ACC database       
/*                that was updated today find and          
/*                write a list of all    associated        
/*                names                                   
/*                                                        
/*    PARAMETERS:        none                             
/*                                                      
/*    RETURNS:                                          
/*        Int4    0        success                      
/*                other    less successful                
/********************************************************/

Int4 BuildSRCH(void)
{
    /*declare variables*/
    int        acc_record=        1;
    char    cur_date[8]=    "00000000";
    Int4    cur_gip=        0;
    Int4    cur_source =    0;
    Int4    cur_taxid =        0;
    CharPtr cur_locus=        NULL;
    char    date[8]=        "10000101";
    char    todays_date[8]=    "00000000";
    
    int        cur_record=        1;
    CharPtr cur_name=        NULL;

    Int4    test = 0;   
    
    /*get todays date*/
    date4today(todays_date);
    /*printf("date = %s", date);*/
    
    /*cycle through each entry in the accession database*/
    for (acc_record = d4top(pd4_ACC); acc_record == 0; acc_record = d4skip(pd4_ACC, 1L))
    {
        /*write names associated with an ACCession record to the SeaRCH database only if the ACCession record was last updated today*/
        StrCpy(cur_date,f4str(pf4_ACC_UPDATED));
        if (1)
        /*if (date4long(cur_date) == date4long(todays_date))*/
        /*(date4long(cur_date) == date4long(todays_date)) or (1)*/
        {
            /*get the gi, locus name and taxon id of the protein*/
            cur_gip    = f4int(pf4_ACC_GI);
            cur_locus = f4str(pf4_ACC_LOCUS);
            cur_source = 1; /*locus name found in ACCession database*/
            cur_taxid = f4int(pf4_ACC_TAXID);
            
            /*write to the search database*/
            WriteSRCH(cur_gip, cur_locus, cur_source, cur_taxid, date);      
            
            /*for each matching gi in the LOSYN database, write the locus synonym to the SRCH database*/
            d4tagSelect(pd4_LOSYN, pt4_LOSYN_GIP);
            cur_record = d4seekDouble(pd4_LOSYN, cur_gip);
            while (cur_record == 0)
            {
                cur_name = f4str(pf4_LOSYN_SYN);
                cur_source = 2; /*locus name found in LOSYN database*/

                /*write to the search database*/
                WriteSRCH(cur_gip, cur_name, cur_source, cur_taxid, date);
                cur_record = d4seekNextDouble(pd4_LOSYN, cur_gip);
            }
            
            /*for each matching gi in the YENA database, write the protein name to the SRCH database*/
            d4tagSelect(pd4_YENA, pt4_YENA_GIP);
            cur_record = d4seekDouble(pd4_YENA, cur_gip);
            while (cur_record == 0)
            {
                cur_name = f4str(pf4_YENA_NAME);
                cur_source = 3; /*name from YEast NAme database*/

                /*write to the search database*/
                WriteSRCH(cur_gip, cur_name, cur_source, cur_taxid, date);
                cur_record = d4seekNextDouble(pd4_YENA, cur_gip);
            }
        }/*only if accession record was last updated today*/
        
    }/*for loop*/
    return 0;
    
}/*BuildSRCH end*/


/**************************************************************
      GetGIFromProductLocation:
      Find the gi from a SeqLocPtr.

      Parameters: psl - the input SeqLocPtr from which to extract
                        the gi. Handles SeqLocs of type Whole and
                        Int.

      Returns the gi on success, 0 on failure.
*************************************************************/
Int4 GetGIFromProductLocation( SeqLocPtr psl )
{
    SeqLocPtr pSeqLoc   = psl;
    Int4      gi        = 0;
    Int4      giLast    = 0;
    SeqIdPtr  pSeqID    = NULL;
    SeqIntPtr pSeqInt   = NULL;

    while( pSeqLoc != NULL && gi == 0)
    {
       if (pSeqLoc->choice == SEQLOC_WHOLE) 
       {
       pSeqID = pSeqLoc->data.ptrvalue; /*a seq-id struct is of type valnode */
       if (pSeqID == NULL)/*the product field of the seqloc struct points to nothing*/
       {
           ErrPostEx(SEV_ERROR,0,0, "GetGIFromLocation: NULL SeqId pointer.\n");
       }
       
           gi = GetGIFromSeqID( pSeqID );
        }
                        
    if (pSeqLoc->choice == SEQLOC_INT ) 
    {
        pSeqInt = pSeqLoc->data.ptrvalue; /*a seq-interval struct is of type valnode */
        if (pSeqInt == NULL)/*the pointer to a seq-interval struct for sequence location points to nothing*/
        {
        ErrPostEx(SEV_ERROR,0,0, "GetGIFromLocation: NULL Seq-Int pointer.\n");
        }
        else
        {
        /*get gi of source DNA*/
        pSeqID = pSeqInt->id; /*WARNING: Seq-id used to be optional in the Seq-interval struct*/
        gi = GetGIFromSeqID( pSeqID );
        }
        }
    
    pSeqLoc = pSeqLoc->next;
    }

    return gi;
}


/**************************************************************
      GetGIFromSeqID:
      Find the gi from a SeqID.

      Parameters: psi - the input SeqID from which to extract
                        the gi.

      Returns the gi on success, 0 on failure.
*************************************************************/
Int4 GetGIFromSeqID( SeqIdPtr psi )
{
    SeqIdPtr pSeqID = psi;
    Int4     gi     = 0;

    while (pSeqID != NULL)/*look through the SeqId nodes for a gi- there is probably only one node*/
    {
    if (pSeqID->choice == SEQID_GI) /*then you have found the gi*/
    {
        gi = pSeqID->data.intvalue; 
        break;
    }
    pSeqID = pSeqID->next;
    }
    return gi;
}


/**************************************************************
      GetLocationInfoFromSeqInt:
      Find the gi, from location, to location from a 
      SeqLoc of type SeqInt.
      

      Parameters: psl - the input SeqLocPtr.
                  pgi - pointer to the gi location to be filled.
                  pfloc - pointer to the from location.
                  ptloc - pointer to the to location.
      
      Returns 0 on success, 1 on failure. 
*******************************************************************/     
Int4 GetLocationInfoFromSeqInt( SeqLocPtr psl, Int4Ptr pgi,
                                Int4Ptr pfloc, Int4Ptr ptloc )
{
    SeqLocPtr pSeqLoc = psl;
    SeqIntPtr pSeqInt =    NULL;
    SeqIdPtr  pSeqID  =    NULL;
    Int4      success = 0;

    /*--------------------------------------------------------
      Do nothing if the SeqLocPtr does not point to a SeqInt.
      --------------------------------------------------------*/
    if (pSeqLoc->choice != SEQLOC_INT) 
        return 1;

    pSeqInt = pSeqLoc->data.ptrvalue; /*a seq-interval struct is of type valnode */
    if (pSeqInt == NULL)/*the pointer to a seq-interval struct for sequence location points to nothing*/
    {
    ErrPostEx(SEV_ERROR,0,0, "GetLocationInfoFromSeqInt: NULL SeqId pointer.\n");
        success = 1;
    }
    else
    {
    /*get location info*/
    *pfloc = pSeqInt->from; 
    *ptloc = pSeqInt->to;
                          
    /*get gi*/
     pSeqID = pSeqInt->id; /*WARNING: Seq-id used to be optional in the Seq-interval struct*/
     *pgi = GetGIFromSeqID( pSeqID );
    }
    return success;
}


/**************************************************************
      GetLocationInfoFromPackedInt:
      Find the gi, from location, to location from a 
      SeqLoc of type PackedInt.
      

      Parameters: psl - the input SeqLocPtr.
                  pgi - pointer to the gi location to be filled.
                  pfloc - pointer to the from location.
                  ptloc - pointer to the to location.
                  transSpliced - indicates that the product is
                  derived from an mRNA that is trans-spliced.
      
      Returns 0 on success, 1 on failure. 
*******************************************************************/     
Int4 GetLocationInfoFromPackedInt( SeqLocPtr psl, Int4Ptr pgi, Int4Ptr pfloc,
                                  Int4Ptr ptloc, Boolean transSpliced )
{
    SeqLocPtr pSeqLocm       =    psl;
    SeqLocPtr pSeqLocm_first =    NULL;
    SeqLocPtr pSeqLocm_last  =    NULL;
    SeqIntPtr pSeqInt        =    NULL;
    SeqIdPtr  pSeqID         =    NULL;
    Int2      strand         =  0;
    Int4      success        =  0;
    Int4      choice         =  psl->choice;
    /*----------------------------------------------------------
     Do nothing if psl is not of type MIX or PACKED_INT.
     ----------------------------------------------------------*/
    if ( psl->choice != SEQLOC_PACKED_INT) 
        return 1;

    pSeqLocm = psl->data.ptrvalue;
    
    if( pSeqLocm  == NULL ) return 1;
    
    /*get the first location value*/
    pSeqLocm_first = pSeqLocm;
    if (pSeqLocm_first->choice == SEQLOC_INT) /*only if pSeqLocm_from is a seq-int struct*/
      {
    pSeqInt = pSeqLocm_first->data.ptrvalue;
        if (pSeqInt != NULL)
        {
               strand = pSeqInt->strand;
        if (pSeqInt->strand == PLUS || pSeqInt->strand == UNKNOWN) /*treat unknown strand as plus strand*/
        {
            *pfloc = pSeqInt->from;/*from location is the from location in the first sequence interval*/
        }
        else if (pSeqInt->strand == MINUS) /*minus strand*/
        {
            *ptloc = pSeqInt->to; /*to location is the to location in the first sequence interval*/
        }

        /*get gi of source DNA*/
        pSeqID = pSeqInt->id; /*WARNING: Seq-id used to be optional in the Seq-interval struct*/
        *pgi = GetGIFromSeqID( pSeqID );
    }
    }/*only if pSeqLocm_first is a seq-int struct*/
    else
    {
        ErrPostEx(SEV_WARNING,0,0, "GetLocationInfoFromPackedInt: Non seqint in finding 'from' "
                  "position for gi source: %ld (pSeqLocm_first->choice= %d)", 
                  *pgi, pSeqLocm_first->choice);
        success = 1;
    }                    
                               
    /*get the last location value*/
    pSeqLocm_last = pSeqLocm;
    while (pSeqLocm_last != NULL)
    {
    if (pSeqLocm_last->choice == SEQLOC_INT) /*only if pSeqLocm_last is a seq-int struct*/
    {
        pSeqInt = pSeqLocm_last->data.ptrvalue;
        if (pSeqInt != NULL)
        {
        if (strand == pSeqInt->strand || transSpliced == TRUE)
        {
            if (pSeqInt->strand == PLUS || pSeqInt->strand == UNKNOWN) /*treat unknown as plus strand*/
             *ptloc = pSeqInt->to;/*to location is the to location of the last sequence interval*/
            if (pSeqInt->strand == MINUS) /*minus strand*/
             *pfloc = pSeqInt->from;/*from location is the from location of the last sequence interval*/
        }
                else
        {
            ErrPostEx(SEV_WARNING,0,0, "GetLocationInfoFromPackedInt: "
                              "Strandedness of first and last sequence "
                              "intervals do not match. gi source: %ld first strand = %d second strand %d)\n", 
                              *pgi, strand, pSeqInt->strand);
                    success = 1;
        }
        }
    }/*only if pSeqLocm_last is a seq-int struct*/
    else
    {
        ErrPostEx(SEV_ERROR,0,0, "GetLocationInfoFromPackedInt: Non seqint in "
                      "finding 'to' position for gi source: %ld (pSeqLocm_last->choice= %d)\n", 
                      *pgi, pSeqLocm_last->choice);
            success = 1;
        }
                                    
        pSeqLocm_last = pSeqLocm_last->next;
    }/*while pSeqLocm_last != NULL*/

    return success;
}


/**************************************************************
      GetLocationInfoFromSeqMix:
      Find the gi, from location, to location from a 
      SeqLoc of type Mix.

      SeqLocs of type NULL, EMPTY, INT and PNT are handled. 
      Others may be handled later.

      Parameters: psl - the input SeqLocPtr.
                  pgi - pointer to the gi location to be filled.
                  pfloc - pointer to the from location.
                  ptloc - pointer to the to location.
                  transSpliced - indicates that the product is
                  derived from an mRNA that is trans-spliced.
      
      Returns 0 on success, 1 on failure. 
*******************************************************************/     
Int4 GetLocationInfoFromSeqMix( SeqLocPtr psl, Int4Ptr pgi, Int4Ptr pfloc,
                                  Int4Ptr ptloc, Boolean transSpliced )
{
    SeqLocPtr pSeqLocm       =    psl;
    SeqLocPtr pSeqLocm_first =    NULL;
    SeqLocPtr pSeqLocm_last  =    NULL;
    SeqIntPtr pSeqInt        =    NULL;
    SeqIdPtr  pSeqID         =    NULL;
    Int2      strand         =  0;
    Int4      success        =  0;
    Int4      choice         =  psl->choice;
    /*----------------------------------------------------------
     Do nothing if psl is not of type MIX or PACKED_INT.
     ----------------------------------------------------------*/
    if ( psl->choice != SEQLOC_MIX) 
        return 1;

    pSeqLocm = psl->data.ptrvalue;
    
    if( pSeqLocm  == NULL ) return 1;
    
    /*get the first location value*/
    pSeqLocm_first = pSeqLocm;
    
    while ( pSeqLocm_first->choice == SEQLOC_NULL ||
            pSeqLocm_first->choice == SEQLOC_NULL )
      pSeqLocm_first = pSeqLocm->next;
            
    if (pSeqLocm_first->choice == SEQLOC_INT) /*only if pSeqLocm_from is a seq-int struct*/
    {
    pSeqInt = pSeqLocm_first->data.ptrvalue;
        if (pSeqInt != NULL)
        {
               strand = pSeqInt->strand;
        if (pSeqInt->strand == PLUS || pSeqInt->strand == UNKNOWN) /*treat unknown strand as plus strand*/
        {
            *pfloc = pSeqInt->from;/*from location is the from location in the first sequence interval*/
        }
        else if (pSeqInt->strand == MINUS) /*minus strand*/
        {
            *ptloc = pSeqInt->to; /*to location is the to location in the first sequence interval*/
        }

        /*get gi of source DNA*/
        pSeqID = pSeqInt->id; /*WARNING: Seq-id used to be optional in the Seq-interval struct*/
        *pgi = GetGIFromSeqID( pSeqID );
    }
    }
    else if (pSeqLocm_first->choice == SEQLOC_PNT) /*only if pSeqLocm_from is a seq-int struct*/
    {
        SeqPntPtr pPnt = pSeqLocm_first->data.ptrvalue;
        
        strand = pPnt->strand;
        if( pPnt->strand == PLUS || pPnt->strand == UNKNOWN )
    {
        *pfloc = pPnt->point;
        }
        else if (pPnt->strand == MINUS) /*minus strand*/
    { 
        *ptloc = pPnt->point;
    }
        /*get gi of source DNA*/
    pSeqID = pPnt->id; /*WARNING: Seq-id used to be optional in the Seq-interval struct*/
    *pgi = GetGIFromSeqID( pSeqID );
    }
    else
    {
        ErrPostEx(SEV_ERROR,0,0, "GetLocationInfoFromSeqMix: SeqLoc type wrong in finding 'from' "
                  "position for gi source: %ld (pSeqLocm_first->choice= %d)\n", 
                  *pgi, pSeqLocm_first->choice);
        success = 1;
    }                    
                               
    /*get the last location value*/
    pSeqLocm_last = pSeqLocm;
    while (pSeqLocm_last != NULL)
    {
    if (pSeqLocm_last->choice == SEQLOC_INT) /*only if pSeqLocm_last is a seq-int struct*/
    {
        pSeqInt = pSeqLocm_last->data.ptrvalue;
        if (pSeqInt != NULL)
        {
        if (strand == pSeqInt->strand || transSpliced == TRUE)
        {
            if (pSeqInt->strand == PLUS || pSeqInt->strand == UNKNOWN) /*treat unknown as plus strand*/
             *ptloc = pSeqInt->to;/*to location is the to location of the last sequence interval*/
            if (pSeqInt->strand == MINUS) /*minus strand*/
             *pfloc = pSeqInt->from;/*from location is the from location of the last sequence interval*/
        }
                else
        {
            ErrPostEx(SEV_ERROR,0,0, "GetLocationInfoFromSeqMix: "
                              "Strandedness of first and last sequence "
                              "intervals do not match. gi source: %ld first strand = %d second strand %d)\n", 
                              *pgi, strand, pSeqInt->strand);
                    success = 1;
        }
        }
    }
        else if (pSeqLocm_last->choice == SEQLOC_PNT) /*only if pSeqLocm_from is a seq-int struct*/
        {
            SeqPntPtr pPnt = pSeqLocm_last->data.ptrvalue;
            
            if( strand == pPnt->strand || transSpliced == TRUE )
        {
                if( pPnt->strand == PLUS || pPnt->strand == UNKNOWN )
            {
                *pfloc = pPnt->point;
                }
                else if (pPnt->strand == MINUS) /*minus strand*/
            { 
                *ptloc = pPnt->point;
            }
            }
            else
            {
            ErrPostEx(SEV_ERROR,0,0, "GetLocationInfoFromSeqMix: "
                              "Strandedness of first and last sequence "
                              "intervals do not match. gi source: %ld first strand = %d second strand %d)\n", 
                              *pgi, strand, pPnt->strand);
                    success = 1;
            }
        }
        /*--------------------------------------------------------------
        If the seqloc is NULL, or empty, then just move on the the
        next seqloc. Otherwise, raise a warning.
        --------------------------------------------------------------*/
        else if ( pSeqLocm_last->choice != SEQLOC_NULL &&
                      pSeqLocm_last->choice != SEQLOC_EMPTY )
        {
            ErrPostEx(SEV_WARNING,0,0, "GetLocationInfoFromSeqMix: SeqLoc type wrong in "
                      "finding 'to' position for gi source: %ld (pSeqLocm_last->choice= %d)", 
                      *pgi, pSeqLocm_last->choice);
            success = 1;
        }
                                    
        pSeqLocm_last = pSeqLocm_last->next;
    }/*while pSeqLocm_last != NULL*/

    return success;
}


/**************************************************************
      GetSourceLocationInfo:
      Find the source gi, from location, to location from a 
      SeqLocPtr. Handles SeqLocs of type SeqInt, Mix and Packed Int.
      

      Parameters: psl - the input SeqLocPtr.
                  pgi - pointer to the gi to be filled.
                  pfloc - pointer to the from.
                  ptloc - pointer to the to location.
                  transSpliced - indicates that the product is
                  derived from an mRNA that is trans-spliced.
      
      Returns 0 on success, 1 on failure. 
*******************************************************************/            
Int4 GetSourceLocationInfo( SeqLocPtr psl, Int4Ptr pgi, Int4Ptr pfloc, 
                              Int4Ptr ptloc, Boolean transSpliced )
{
    SeqLocPtr pSeqLoc = psl;
    
    SeqLocPtr pSeqLocm_first = NULL;
    SeqLocPtr pSeqLocm_last  = NULL;
    SeqIntPtr pSeqInt        = NULL;
    SeqIdPtr  pSeqID         = NULL;
    Int2      strand         = 0;
    Int4      success        = 0;

    while (pSeqLoc != NULL)/*look through the sequence location nodes for a seq-interval- there is probably ony one node*/
    {
        if (pSeqLoc->choice == SEQLOC_INT) /*then pSeqLoc->data.ptrvalue points to a seq-interval struct*/
    {
        success = 
            GetLocationInfoFromSeqInt( pSeqLoc, pgi, 
                                           pfloc, ptloc );
    }
    else if ( pSeqLoc->choice == SEQLOC_MIX )
        {
            success = 
               GetLocationInfoFromSeqMix( pSeqLoc, pgi, pfloc,
                                          ptloc, transSpliced );
    }
        else if ( psl->choice == SEQLOC_PACKED_INT ) 
        {
            success = 
               GetLocationInfoFromPackedInt( pSeqLoc, pgi, pfloc,
                                             ptloc, transSpliced );
    }
        pSeqLoc = pSeqLoc->next;
    }
    return success;
} 


/**************************************************************
   GetDatabaseXRefs:
   Finds all database cross references.

   Parameters: pdbvn - ValNodePtr to the database XRefs.

****************************************************************/
void GetDatabaseXRefs( ValNodePtr pdbvn, Int4 gis, Int4 floc, Int4 tloc )
{
    DbtagPtr     pdbTag     = NULL;
    CharPtr     dbname_ptr = NULL;
    ObjectIdPtr     ptag       = NULL;
    Int4     tagi       = 0;
    CharPtr     tagc       = NULL;
    
    while (pdbvn != NULL)
    {
    pdbTag = pdbvn->data.ptrvalue; /*get a pointer to a Dbtag structure*/
    if (pdbTag != NULL)
    {
               dbname_ptr = pdbTag->db; /*name of the database in the xreference*/
        ptag = pdbTag->tag; /*database identifier (of type ObjectId)*/
               if (ptag != NULL)
        {
             /*get the database identifier; 
                 may be either an integer or a string*/
        tagi = ptag->id;
        tagc = ptag->str;
        if (dbname_ptr != NULL)
               {             
                    WriteDB(gis, floc, tloc, 0, dbname_ptr, tagi, tagc);
            dbname_ptr = NULL;
            tagi = 0;
               tagc = NULL;
            }
              }
    }
    pdbvn = pdbvn->next;
   } 
}


/**************************************************************
      ParseLocusInfoFromFeatureTable:
      Finds the protein gi, DNA gi, and from and to locations
      from a SeqFeatPtr of type GeneRef. Returns 1 if the
      SeqFeatTable is not a CDRegion.

      Parameters: psf      - the SeqFeatPtr to parse.

      Returns 0 on success, 1 on failure.
**************************************************************/
Int4 ParseLocusInfoFromFeatureTable(  SeqFeatPtr psf, Int4 gi)
{
    GeneRefPtr pGeneRef        = NULL;
    Int4       floc            = 0;   /*from location*/
    Int4       tloc            = 0;   /*to location*/
    Int4       gis             = 0;   /*gi of source DNA*/
    ValNodePtr svnp            = NULL;/*valnode pointer to synonym list*/
    CharPtr    syn_ptr         = NULL;/*pointer to synonym string*/
    Int4       function_report = 0;
    /*dbxref stuff*/
    ValNodePtr pdbvn           = NULL;
    CharPtr    locus_ptr      = NULL;/*name of gene locus*/
    SeqLocPtr  psl             = NULL;/**/
    Boolean    transSpliced    = FALSE;

    Int4       success         = 0; 

    if (psf->data.choice != SEQFEAT_GENE)    /*seq feature data field contains a choice struct whose data field points to a GeneRef struct: look for the "SeqFeat" struct in the SourceBrowser*/
    {
        ErrPostEx(SEV_ERROR,0,0, "ParseLocusInfoFromFeatureTable: "
                  "Not a generef for %ld.\n", gi);
        return 1;
    }
    /* ------------------------------------------------------------
       Is there trans-splicing going on?
       ------------------------------------------------------------*/
    if( psf->excpt && psf->excpt == TRUE && psf->except_text
        && !strcmp( psf->except_text, "trans splicing" ) )
    {
        transSpliced = TRUE;
    }
    /*get a pointer to the generef struct*/
    pGeneRef = psf->data.value.ptrvalue;
    if (pGeneRef == NULL)/*NULL GeneRef pointer*/
    {
    ErrPostEx(SEV_ERROR,0,0, "ParseLocusInfoFromFeatureTable: NULL GeneRef pointer.\n");
    }
    else /*do the following only if pGeneRef != NULL*/
    {
        /********************/
    /*get the locus name*/
    /********************/
    locus_ptr = pGeneRef->locus;
    /*note you can also get synonyms and ids in other databases from this same pointer*/
    if (locus_ptr == NULL)/*NULL pointer to locus name*/
    {
            /*sometimes fails here especially for unnamed ORFs from yeast with no known function
          just ORF names listed under "synonym"*/
        locus_ptr = "NO_PRIMARY_LOCUS_NAME";
    }                    
            
        /****************************************/
    /*get the location info for the gene    */
    /****************************************/
     psl = psf->location;
     if (psl == NULL)/*the location field of the seqfeat struct points to nothing*/
     {
         ErrPostEx(SEV_ERROR,0,0, "ParseLocusInfoFromFeatureTable: "
                       "NULL SeqLoc pointer for %ld.\n", gi);
     }
                        
         success = GetSourceLocationInfo( psl, &gis, &floc, &tloc, transSpliced );
                            
     /****************************************/
     /*print out the results    if non-zero        */
     /****************************************/
     if (floc < 0) /*remeber 0 is the first base of a DNA sequence in C land*/
     {
         ErrPostEx(SEV_ERROR,0,0, "ParseLocusInfoFromFeatureTable: floc is < zero.\n");
     }
     if (tloc < 0)
     {
         ErrPostEx(SEV_ERROR,0,0, "ParseLocusInfoFromFeatureTable: tloc is < zero.\n");
     }
     if (gis <= 0)
     {
         ErrPostEx(SEV_ERROR,0,0, "ParseLocusInfoFromFeatureTable: "
                       "gis is <= zero for gi= %ld locus = %s floc = %ld tloc = %ld.\n", gi, locus_ptr, floc, tloc);
     }
                            
     /*write the data to a Locus database*/
     if (gis > 0 && floc > -1 && tloc > -1)
     {
         function_report = WriteLOCUS(gis, floc, tloc, locus_ptr);
     }
                            
     /*********************************************************************/
     /*for this locus get all synonyms and write them to a LOSYN database*/
     /*********************************************************************/
     svnp = pGeneRef->syn;
     while (svnp != NULL)
     {
         syn_ptr = svnp->data.ptrvalue;
         if (syn_ptr != NULL && syn_ptr != locus_ptr)
         {
         /*printf("synonym = %s\n", syn_ptr);*/
         WriteLOSYN(gis, floc, tloc, syn_ptr);
         }
         svnp = svnp->next;
     }
                            
     /*******************************************************************************/
     /*while at this locus, get all database xrefs and write them to the DB database*/
     /*******************************************************************************/
     pdbvn = pGeneRef->db;/*get the first valnode in a possble 
                                string of pointers to database xrefs*/
     GetDatabaseXRefs( pdbvn, gis, floc, tloc );    
                
    }/*only done if pGeneRef != NULL*/    
                    
}


/********************************************************/
/*         
/*    ParseLocusInfo:    
/*        looks for gene info in the feature 
/*        table of a Bioseq for gene information
/*        (if no gene locus info is found the BioseqSet   
/*        annotation is examined for gene locus info)   
/*        For each gene annotation, this function gets:   
/*        locus name (if this is found then find):    
/*            from location                       
/*            to location                    
/*            gi to which location info (from/to) applies    
/*                                   
/*                                      
/*    PARAMETERS:                            
/*        BioseqPtr        pbs    -points to a Bioseq  
/*                                       
/*    RETURNS:                               
/*        Int4            0    -success           
/*        Int4           >0    -less successful           
/********************************************************/

Int4    ParseLocusInfo(BioseqPtr pbs, BioseqSetPtr last_pbss)
{/**/
    
    SeqAnnotPtr        psa     = NULL;
    SeqFeatPtr        psf     = NULL;
    GeneRefPtr        pgr     = NULL;
    Int4                gi      = 0;
    Int4                success = 0;
    
    /*use the BioseqPtr to get a pointer to its annotation*/
    psa = pbs->annot;
    gi = PreBINDGetGI( pbs );
    if (psa == NULL) /*there is no annotation- it happens in rare cases that locus info is in BioseqSet annotation*/
    {
    if (last_pbss == NULL || last_pbss->annot == NULL)
    {
        ErrPostEx(SEV_WARNING,0,0, "ParseLocusInfo: NULL SeqAnnot pointer for both BioSeq and BioseqSet.");
        return 1;
    }
    else
        psa = last_pbss->annot;
    }

   /*look for the feature table while there is still a node in the BioseSet annotation*/
    while (psa != NULL) 
    {
    if (psa->type == ANNOT_TABLE)/*the data value of this annotation node points to a feature table*/
    {
        psf = (SeqFeatPtr) psa->data;
        if (psf == NULL)/*the first node of the feature table points to nothing*/
        {
        ErrPostEx(SEV_ERROR,0,0, "ParseLocusInfo: NULL SeqFeat pointer. %ld\n", gi);
        }
        /*do the following only if psf was not NULL*/
        while (psf != NULL) /*while there are still nodes in the feature table then look for gene data*/
        {
          /* ----------------------------------------------------------
                 Only parse the locus info if it's not a pseudogene.
                 ----------------------------------------------------------*/
        if ( psf->data.choice == SEQFEAT_GENE && psf->pseudo == FALSE )    
        {
             ParseLocusInfoFromFeatureTable( psf, gi );
                    
        }/*only for gene ref structs*/
                
        psf = psf->next;    /*look at the next sequence feature for a GeneRef struct*/
                
        }/*while psf != NULL*/
            
    }/*only done for annotations of type: feature table*/
        
        psa = psa->next;    /*look at the next sequence annotation for a SeqFeat struct*/
    }/*while psa != NULL*/
    
    return 0;
    
}/*function end*/


/**************************************************************
      ParseCDRInfoFromFeatureTable:
      Finds the protein gi, DNA gi, and from and to locations
      from a SeqFeatPtr of type CDRegion. Returns 1 if the
      SeqFeatTable is not a CDRegion.

      Parameters: psf      - the SeqFeatPtr to parse.
                  bioseqGI - solely for error information.

      Returns 0 on success, 1 on failure.
**************************************************************/
Int4 ParseCDRInfoFromFeatureTable(  SeqFeatPtr psf, Int4 bioseqGI )
{
    SeqFeatPtr pSeqFeat        = psf;
    SeqLocPtr  pSeqLoc         = NULL;
    Int4       gi              = 0;
    Int4       floc            = 0;
    Int4       tloc            = 0;
    Int4       gis             = 0;
    ValNodePtr pdbvn           = NULL;
    Boolean    transSpliced    = FALSE;
    Int4       success         = 0;

    /*-----------------------------------------------------------------
      Do nothing if pSeqFeat is not of type CDREGION.
      --------------------------------------------------------------*/
    if (pSeqFeat->data.choice != SEQFEAT_CDREGION)
        return 1; 

 /* ------------------------------------------------------------
       Is there trans-splicing going on?
   ------------------------------------------------------------*/
    if( pSeqFeat->excpt && pSeqFeat->excpt == TRUE && pSeqFeat->except_text
        && !strcmp( pSeqFeat->except_text, "trans splicing" ) )
    {
        transSpliced = TRUE;
    }    

    /*so then the product field of the seqfeat struct contains a ValNodePtr of type SeqLocPtr 
    /*******************************************/
    /*get the gi of the product of the CDregion*/
    /*******************************************/
    pSeqLoc = pSeqFeat->product;
    if (pSeqLoc == NULL)/*the product field of the seqfeat struct points to nothing*/
    {
    ErrPostEx(SEV_ERROR,0,0, "ParseCDRInfoFromFeatureTable: "
                  "No product SeqLoc pointer for %ld.\n", bioseqGI);
                           
    /*return 3;*/
    }
    /*do the following only if pSeqLoc was not NULL*/
    else
    {
    gi = GetGIFromProductLocation( pSeqLoc );
    }
                        
    /****************************************/
    /*get the location info for the CDregion*/
    /****************************************/
    pSeqLoc = pSeqFeat->location;
    if (pSeqLoc == NULL)/*the location field of the seqfeat struct points to nothing*/
    {
    ErrPostEx(SEV_ERROR,0,0, "ParseCDRInfoFromFeatureTable: NULL SeqLoc pointer.\n");
    }
             
    GetSourceLocationInfo( pSeqLoc, &gis, &floc, &tloc, transSpliced );

    /****************************************/
    /*print out the results    if non-zero        */
    /****************************************/
    if (gi <= 0)
    {
        if (pSeqFeat->pseudo == FALSE)/*only report missing gi's for cdr's for non-pseudogenes)*/
    {
        ErrPostEx(SEV_ERROR,0,0, "ParseCDRInfoFromFeatureTable: gi is <= zero for gi source %ld: location %ld to %ld. This is not a pseudogene.\n", gis, floc, tloc);
    }
    }

    if (floc < 0) /*remeber 0 is the first base of a DNA sequence in C land*/
    {
    ErrPostEx(SEV_ERROR,0,0, "ParseCDRInfoFromFeatureTable : floc is < zero for protein gi %ld source gi %ld.\n", gi, gis);
    }

    if (tloc < 0)
    {
    ErrPostEx(SEV_ERROR,0,0, "ParseCDRInfoFromFeatureTable: tloc is < zero for protein gi %ld source gi %ld.\n", gi, gis);
    }
    if (gis <= 0)
    {
    ErrPostEx(SEV_ERROR,0,0, "ParseCDRInfoFromFeatureTable: source gi is <= zero for protein gi %ld: location %ld to %ld.\n", gi, floc, tloc);
    }

    if (gi > 0 && gis > 0 && floc > -1 && tloc > -1)
    {
    WriteCDR(gi, floc, tloc, gis);
    }
                    
    /*****************************************************************************/
    /*while at this CDR, get all database xrefs and write them to the DB database*/
    /*****************************************************************************/
    pdbvn = pSeqFeat->dbxref;/*get the first valnode in a possble string of pointers to database xrefs*/
    GetDatabaseXRefs( pdbvn, gis, floc, tloc );
        
    return success;
}  


/********************************************************/
/*                                       
/*    ParseCDRInfo:                                  
/*        looks for coding region data in the feature    
/*        table of a BioseqSet and gets for each CDR:    
/*        gi of protein                         
/*        from location                           
/*        to location                             
/*        gi to which location info (from/to) applies   
/*                                          
/*                                     
/*    PARAMETERS:                                  
/*        BioseqSetPtr    pbss    -points to a BioseqSet 
/*                                       
/*    RETURNS:                                              
/*        Int4            0    -success       
/*        Int4           >0    -less successful      
/********************************************************/

Int4 ParseCDRInfo(BioseqSetPtr pbss, BioseqPtr pbs)
{
    SeqAnnotPtr        psa =        NULL;
    SeqFeatPtr        psf =        NULL;
    Int4                bioseqGI = -1;
    Int4                success = 0;
    
    if (pbs == NULL)/*the ususal case-look for CDR info in the BioseqSet*/
    {
       /*use the BioseqSetPtr to get a pointer to its annotation*/
       psa = pbss->annot;
       if (psa == NULL) /*there is no annotation*/
       {
      /*ErrPostEx(SEV_ERROR,0,0, "ParseCDRInfo: NULL SeqAnnot pointer for BioSeqSet.\n");*/
      return 1; /*sometimes fails here-info is in the annotation to the BioSeq*/
      /*set a flag to look for CDR info in the Bioseqs of this BioseqSet*/
       }
    }
    else /*look for CDR info in the Bioseq*/
    {
       /*use the BioseqPtr to get a pointer to its annotation*/
       if (pbs == NULL || pbs->annot == NULL)/*there is no annotation*/
       {
       ErrPostEx(SEV_ERROR,0,0, "ParseCDRInfo: NULL SeqAnnot pointer for both BioseqSet and Bioseq.\n");
          return 99; 
       }
      
       psa = pbs->annot; 
       bioseqGI = PreBINDGetGI( pbs );
    }
    
    /*look for the feature table while there is still a node in the BioseqSet annotation*/
    while (psa != NULL) 
    {
        if (psa->type == ANNOT_TABLE)/*the data value of this annotation node points to a feature table*/
    {
        psf = (SeqFeatPtr) psa->data;
        if (psf == NULL)/*the first node of the feature table points to nothing*/
        {
        ErrPostEx(SEV_ERROR,0,0, "ParseCDRInfo: NULL SeqFeat pointer %ld.\n", bioseqGI);
        return 2;
        }
            
        while (psf != NULL) /*while there are still nodes in the feature table then look for the CDRegion data*/
        {
          /* --------------------------------------------------------------------
                 Only parse the CDR Info if it's not a pseudo gene.
                 ------------------------------------------------------------------- */
            if (psf->data.choice == SEQFEAT_CDREGION && psf->pseudo == FALSE )    
            {    
                    success = ParseCDRInfoFromFeatureTable( psf, bioseqGI );                  
        }
        psf = psf->next; /*look at the next sequence feature for a CDRegion struct*/
        }
    }
    psa = psa->next; /*look at the next sequence annotation for a SeqFeat struct*/
    }
    
    return success;
}






/********************************************************/
/*                                                       
/*    GetProtName:                                        
/*        Looks for the name of a protein in the          
/*        feature table.                                  
/*    CALLS                                              
/*                                                       
/*    PARAMETERS:                                          
/*        BioseqPtr        pbs        points to a Bioseq   
/*                                                       
/*    RETURNS:                                           
/*        CharPtr            namestr    a pointer to the name
/********************************************************/
CharPtr GetProtName(BioseqPtr pbs)
{
    
    SeqAnnotPtr    psa =        NULL;
    SeqFeatPtr    psf =        NULL;
    ProtRefPtr    ppr =        NULL;
    ValNodePtr    pvn =        NULL;
    CharPtr     name_ptr =    NULL;
    Uint1       processed =     0;
    
    psa = pbs->annot;
    if (psa == NULL) /*there is no annotation for the Bioseq*/
    {
    ErrPostEx(SEV_ERROR,0,0, "GetProtName: NULL SeqAnnot pointer.\n");
    return NULL;
    }
    while (psa != NULL) /*while there is still a node in the Bioseq annotation, look for the feature table*/
    {
    if (psa->type == 1)/*the data value of this annotation node points to a feature table*/
    {
        psf = (SeqFeatPtr) psa->data;
        if (psf == NULL)/*the first node of the feature table points to nothing*/
        {
        ErrPostEx(SEV_ERROR,0,0, "GetProtName: NULL SeqFeat pointer.\n");
        return NULL;
        }
        /*do this only if psf is not NULL*/
        while (psf != NULL) /*while there are still nodes in the feature table then look for the protein name*/
        {
          if (psf->data.choice == 4)    /*seq feature data struct points to a ProtRef struct */
              {
              ppr = psf->data.value.ptrvalue;        /*this is convoluted;
                                  data        field of    SeqFeat struct is of type    Choice
                                  choice    field of    Choice struct is of type    Uint1
                                  ptrvalue    field of    Choice struct is of type    Pointer*/
              if (ppr == NULL)
              {
              ErrPostEx(SEV_ERROR,0,0, "GetProtName: NULL ProtRef pointer.\n");
              return NULL;
              }
              else /*only if ppr is not NULL*/
              {
              /*get names only of non-processed or mature proteins*/
              processed = ppr->processed;
              if (processed == NOTSET || processed == MATURE)
              {
                  pvn = ppr->name;                    
                            
                  if (pvn == NULL)
                  {
                 /*sometimes there is a "description" (under desc) instead of a name (See Prot-ref in ASN.1 browser)*/
                   name_ptr = ppr->desc;
                   if (name_ptr == NULL)
                   {
                       ErrPostEx(SEV_WARNING,0,0, "GetProtName: NULL name valnode pointer and NULL description pointer.");
                       return NULL;
                   }
                   else /*only if there is a description*/
                   {
                       /*printf("returned a desc: %s\n", name_ptr);*/
                    return name_ptr;
                   }
                   }
                   while (pvn != NULL) /*only if pvn is not NULL*/
                   {
                                
                   name_ptr = pvn->data.ptrvalue;    /*this is convoluted too; ppr points to a ProtRef struct*/
                                    /*name        field of    ProtRef struct is of type    ValNodePtr
                                      data        field of    ValNode struct is of type    DataVal
                                      ptrvalue    field of    DataVal union is of type    Nlm_VoidPtr
                                      therefore pvn->data.ptrvalue returns a pointer to the name string*/
                                
                   /*note that ProtRef struct has other goodies like 
                     ec number, ids in other databases and whether or not the 
                     protein is mature or preprocessed*/
                   if (name_ptr != NULL)
                   {
                       break;/*leave with the first name in the list*/
                   }
                   pvn = pvn->next;
                   }/*only while pvn is not NULL*/
               }/*only for non-processed or mature protein ref structs*/
            }/*only if ppr is not NULL*/
            }/*only if seq-feat points to a prot-ref structure*/
            if (name_ptr != NULL)
            {
            break;/*stop looking for Prot Ref structs once you have found a name for a non-processed or a mature protein*/
            }
            psf = psf->next;    /*otherwise look at the next sequence feature for a ProtRef struct*/
        }/*while psf is not NULL*/
        }/*only if psa ==1 (points to a seq feat*/
        psa = psa->next;/*look at the next sequence annotation for a SeqFeat struct*/
        }/*only while psa != NULL*/
    if (name_ptr == NULL)
    {
        ErrPostEx(SEV_WARNING,0,0, "GetProtName: NULL name pointer. processed = %d", 
                          processed);
        return NULL;
    }
    return name_ptr;
    
}/*end of function*/


/********************************************************/
/*                                                        
/*    GetTitle:                                           
/*        Called by FillACCDB-looks for the title of the   
/*        Bioseq.  If this fails it looks for the name of  
/*         the Bioseq.  If this fails, it looks for a title
/*        in the description of the Bioseq-set            
/*    CALLS                                               
/*        GetName if there is no title for the Bioseq       
/*    PARAMETERS:                                           
/*        BioseqPtr        pbs        -points to a Bioseq       
/*        SeqDescrPtr        pdesset    -points to a description
/*                                of the Bioseq-set of    
/*                                which this Bioseq is a    
/*                                member                    
/*    RETURNS:                                            
/*        CharPtr            title    -a pointer to the title  
/********************************************************/
CharPtr GetTitle(BioseqPtr pbs, SeqDescrPtr pdesset)
{

    SeqDescrPtr        pdes =        NULL;
    CharPtr            title =        NULL, 
                    name =        NULL;
    
    
    pdes = pbs->descr;
    if (pdes == NULL) /*there is no description for the Bioseq*/
    {
        /* ErrPostEx(SEV_ERROR,0,0, "GetTitle: NULL seqdesc pointer.\n"); */
        return NULL;
    }
    else /*find the title in the description for the Bioseq*/
    {
        while (pdes != NULL) /*there is still another Bioseq description node to check for a value for title*/
        {
            if (pdes->choice == 5) /*points to title of Bioseq*/
            {
                title = pdes->data.ptrvalue;
                break; /*you have whatever was assigned as title for this Bioseq*/
            }
            else /*go to the next description node to check for a title*/
            {
                pdes = pdes->next;
            }
        }
    }
 
    /*if there is no title then use the name as title*/
    /*if (title != NULL)
    {
        printf("*\n");
    }
    if (title == NULL)
    {
        name = GetName(pbs);
        if (name != NULL)
        {
            title = name;
        }

    }*/

    /*if there is still no title then go and look for it in the Bioseq-set description*/
    /*if (title == NULL)
    {
        while (pdesset != NULL) /*there is still another Bioseq-set description node to check for a value for title*/
        /*{
            if (pdesset->choice == 5) /*points to title of Bioseq-set*/
            /*{
                title = pdesset->data.ptrvalue;
                break; /*you have whatever was assigned as title for this Bioseq*/
            /*}
            else /*go to the next description node to check for a title*/
            /*{
                pdesset = pdesset->next;
            }
        }
    }*/
    return title; 
}


/********************************************************/
/*                                                        
/*    GetAccession:                                        
/*        This function looks for the    accession number  
/*        for a given BioSeq                              
/*                                                        
/*    PARAMETERS:                                           
/*        BioseqPtr        pbs        points to a Bioseq       
/*                                                       
/*    RETURNS:                                           
/*        CharPtr        pointer to accession string           
/*        NULL        if it fails                          
/********************************************************/
CharPtr    GetAccession(BioseqPtr pbs)
{
    CharPtr            acc_ptr=            NULL;
    SeqIdPtr        psi =                NULL;
    SeqIdPtr        psiThis =            NULL;
    TextSeqIdPtr    ptsi =                NULL;

    /*process Seq-id valnode and get the accession */
    psi = pbs->id;    /*this is convoluted; psi points to a SeqId struct of type ValNode*/
                    /*choice    field of    SeqId ValNode struct is of type    Nlm_Uint1
                      if choice == 10 then the id refers to a ref_seq record and 
                      data        field of    SeqId Valnode is of type ptrvalue
                      and points to a textseqid struct and
                      accession    field of    textseqid struct is of type CharPtr
                        ..............blap*/

    if (psi == NULL)
    {
        ErrPostEx(SEV_ERROR,0,0, "GetAccession: NULL Seq-id pointer.\n");
        return FALSE;
    }
    psiThis = psi;
    while (psiThis != NULL)
    {
        if (psiThis->choice == 10)  /*it is an "other" id of type Textseq-id*/
        {
            ptsi = psiThis->data.ptrvalue;
            if (ptsi == NULL)
            {
                ErrPostEx(SEV_ERROR,0,0, "GetAccession: NULL TextSeq-id pointer.\n");
                return NULL;
            }
            else
            {
                acc_ptr = ptsi->accession;
            } 
        }
        psiThis = psiThis->next;
    }
    return acc_ptr;
}


/********************************************************/
/*                                                        
/*    PreBINDGetGI:                                           
/*        This function looks for the    gi (gene info     
/*        number) for a given BioSeq                      
/*                                                       
/*    PARAMETERS:                                           
/*        BioseqPtr        pbs        points to a Bioseq      
/*                                                       
/*    RETURNS:                                           
/*        Int4    gi                                      
/*        Int4    -1    */
/*        Int4    <=0 (if it fails)                       
/********************************************************/

Int4 PreBINDGetGI(BioseqPtr pbs)
{
    Int4        gi =                0;
    SeqIdPtr    psi =                NULL;
    SeqIdPtr    psiThis =            NULL;

    /*process Seq-id valnode*/
    psi = pbs->id;    /*this is convoluted; psi points to a SeqId struct of type ValNode*/
                    /*choice    field of    SeqId ValNode struct is of type    Nlm_Uint1
                      if choice == 12 then the id refers to a gi number (Int4) and 
                      data        field of    SeqId Valnode is of type intvalue*/

    if (psi == NULL)
    {
        ErrPostEx(SEV_ERROR,0,0, "PreBINDGetGI: NULL Seq-id pointer.\n");
        return -1;
    }
    psiThis = psi;
    while (psiThis != NULL)
    {
        /*get gi number*/
        if (psiThis->choice == 12)  /*it is a gi of type INTEGER*/
        {
            gi = psiThis->data.intvalue;
            if (gi <= 0)
            {
                /*ErrPostEx(SEV_ERROR,0,0, "PreBINDGetGI failed: GI <=0.\n");*/
                return -2;
            }
        }
        psiThis = psiThis->next;
    }
    return gi;
}


/********************************************************/
/*                                                        
/*    GetTAXONID:                                            
/*        This function looks for the    TAXONomy IDentifier  
/*    CALLS                                                
/*                                                        
/*    PARAMETERS:                                           
/*        SeqDescrPtr        pdes    points to a description   
/*                                of a Bioseq-set or of   
/*                                a Bioseq               
/*                                                        
/*    RETURNS:                                            
/*        Int4    Taxonomy ID                                
/*        Int4    0 (if it fails)                           
/********************************************************/

Int4 GetTAXONID(SeqDescrPtr pdes)
{
    SeqDescrPtr        pdesThis=    NULL;
    OrgRefPtr        por=        NULL;
    BioSourcePtr    pbio=        NULL;
    ValNodePtr        pvn=        NULL;
    DbtagPtr        pdt=        NULL;
    ObjectIdPtr        poi=        NULL;
    Int4            taxid=        0;


    pdesThis = pdes;
    while (pdesThis != NULL)
    {
    if (pdesThis->choice == 23) /*data.ptrvalue points to an Org-ref structure*/
    {
        pbio = pdesThis->data.ptrvalue;
    }
    pdesThis = pdesThis->next;
    }
    if (pbio == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "GetTAXONID: BioSource pointer is NULL.\n"); 
      return 0;
    }

    por = pbio->org;/*the org field of the Org-ref structure contains a pointer to an Org-ref struct*/
    if (por == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "GetTAXONID: Org-ref pointer is NULL.\n"); 
      return 0;
    }
    
    pvn = por->db; /*db field in the Org-ref struct is a ValNodePtr*/
    if (pvn == NULL)
    {
    ErrPostEx(SEV_ERROR,0,0, "GetTAXONID: Db pointer is NULL.\n"); 
    return 0;
    }
    

    pdt = pvn->data.ptrvalue;/*the data.ptrvalue of the Valnode is a ptr to a dbtag struct*/
    while (strstr(pdt->db, "taxon") == NULL)/*the db field of the dbtag struct is a CharPtr to the name of the database (should be "taxon")*/
    {
    /*ErrPostEx(SEV_ERROR,0,0, "GetTAXONID: Database is not taxon but %s (NULL).\n", pdt->db);*/
    pvn = pvn->next;
    if (pvn != NULL) 
    {
        pdt = pvn->data.ptrvalue;
    }
    else
    {
        return 0;
    }
    }
    
    poi = pdt->tag;/*the tag field of the dbtag struct contains a ptr to an ObjectId struct*/
    if (poi == NULL)
    {
    ErrPostEx(SEV_ERROR,0,0, "GetTAXONID: Object Id pointer is NULL.\n");
    return 0;
    }

    taxid = poi->id; /*finally!!!!the id field of the ObjectId struct carries the TAXONomy IDentifier*/
    if (taxid == 0)
    {
     ErrPostEx(SEV_ERROR,0,0, "GetTAXONID: TaxId is zero or NULL.\n");
     return 0;
    }
    return taxid;    
}


/************************************************************/
/*                                                            
/*    ParseBioseq:                                            
/*        This function looks for the                            
/*        Bioseq's GI, Accession number, Database name       
/*        Official name, Definition line, TAXID, and          
/*        preferred product name                             
/*    CALLS                                                  
/*        GetTitle                                           
/*    PARAMETERS:                                               
/*        BioseqPtr        pbs            points to a Bioseq     
/*        BioseqSetPtr    last_pbss    points to the BioseqSet 
/*                                    of which this Bioseq is   
/*                                    is a part               
/*        SeqDescrPtr        pdesset        points to a description 
/*                                    of the Bioseq-set of    
/*                                    which this Bioseq is a 
/*                                    member                  
/*    RETURNS:                                              
/*        TRUE-    success                                    
/*        FALSE-    no success                                 
/************************************************************/
Boolean ParseBioseq(BioseqPtr pbs, BioseqSetPtr last_pbss, SeqDescrPtr pdesset)
{
    Int4            function_report =    0;

    CharPtr            protname_ptr =        NULL;
    CharPtr            acc_ptr=            NULL;
    Int4            gi=                    0;
    Int4            taxid=                0;
    CharPtr            db_ptr=                "REFSEQ";
    char            date[8]=            "00000000";
    SeqDescrPtr        pdes =                NULL;
    
    /*char            line[100];*/
    
    
    /*first get the gi (gene info number) of the current Bioseq*/
    if ((gi = PreBINDGetGI(pbs)) <= 0)
    {
        ErrPostEx(SEV_ERROR,0,0, "ParseBioseq: PreBINDGetGI failed.\n");
        return FALSE; /*move on to the next Bioseq*/
    }
    
    /*if the current Bioseq is a protein then get its name, accession taxid and write these to the ACC database*/
    if (pbs->mol == 3)
    {
        /*get name of protein*/
        protname_ptr = GetProtName(pbs);
        if (protname_ptr == NULL)
        {
            protname_ptr = "gene product";
            ErrPostEx(SEV_WARNING,0,0, "ParseBioseq: GetProtName failed for gi = %ld", gi);
        }
        
        /*get accession string*/

        if ((acc_ptr = GetAccession(pbs)) == NULL)
        {
            ErrPostEx(SEV_ERROR,0,0, "ParseBioseq: GetAccession failed for gi= %ld\n", gi);
            return FALSE; /*move on to the next Bioseq*/
        }
              
        /*look for TAXID in the BioseqSet first and then in the Bioseq description */
        /*first get a pointer to the Bioseq description*/
        pdes = pbs->descr;
        if (pdes = NULL)
        {
            ErrPostEx(SEV_ERROR,0,0, "ParseBioseq: Bioseq Descr pointer is NULL for gi= %ld\n", gi);
            return FALSE;
        }
        
        if ((taxid = GetTAXONID(pdesset)) == 0)
        {
             ErrPostEx(SEV_WARNING,0,0,"ParseBioseq: Look for taxon in the bioseq descr for gi = %ld", gi);

            if ((taxid = GetTAXONID(pdes)) == 0)
            {
                ErrPostEx(SEV_ERROR,0,0, "ParseBioseq: GetTAXONID failed for gi = %ld\n", gi);
                return FALSE;/*move on to the next Bioseq*/
            }
        }
        
        /*get organism binomial name*/
        
        /*get the date*/
        date4today(date);
        /*printf("date = %s", date);*/
           
        /*Write data from Bioseq to ACCession database*/
        WriteACC(gi, acc_ptr, db_ptr, protname_ptr, taxid, date);

        ++number_proteins;

    }/*only if the Bioseq is a protein*/

    /*if the current Bioseq is not a protein just get locus name/location info and then move on to the next Bioseq*/
    else if (pbs->mol != 3) 
    {            
        /*extract locus name and location info from the feature table*/
        function_report = ParseLocusInfo(pbs, last_pbss);
        if (function_report != 0)
        {
            /*ErrPostEx(SEV_INFO,0,0, "ParseBioseq: ParseLocusInfo failed");*/
            printf("ParseLocusInfo failed for gi = %ld: function report= %d\n", gi, function_report);
            ErrPostEx(SEV_WARNING, 0, 0, "ParseLocusInfo failed for gi = %ld: function report= %d", gi, function_report);
            function_report = 0;
        }
        return TRUE; /*move on to the next Bioseq*/
    }

    /*if coding region data was not found in the BioseqSet, then look for it in the Bioseq*/
    if (cdr_flag == 1)
    {
        ParseCDRInfo(NULL, pbs);
        cdr_flag = 0;
    }

    /*printf("bss= %d \ttitle= %s\n", number_bioseqsets, name_ptr);*/
    /*printf("bss= %d \tclass= %d\t bs= %d\t protein#= %d\n", number_bioseqsets, bss_class, number_bioseqs, number_proteins);*/
    /*printf("bss= %d \ttitle= %s\t bs= %d\n", number_bioseqsets, name_ptr, number_bioseqs);*/
    /*printf("bss= %d \tclass= %d\t bs= %d\n", number_bioseqsets, bss_class, number_bioseqs);*/
    /*printf("This Bioseq is not a protein. Press return to continue\n");*/
    /*(void) fgets(line, sizeof(line), stdin);*/
    /*printf("bss= %d \tclass= %d\t bs= %d\t protein#= %d\ttitle= %s\n", number_bioseqsets, bss_class, number_bioseqs, number_proteins, title);*/
    /*printf("bss= %d \t bs= %d\t acc= %s \tgi= %d\ttitle= %s\n", number_bioseqsets, number_bioseqs, acc_ptr, gi, name_ptr);*/
    /*printf("bss= %d \t bs= %d\t acc= %s \tgi= %d\ttaxon= %d\tlocus= %s\n", number_bioseqsets, number_bioseqs, acc_ptr, gi, taxid, genename_ptr);*/
    /*(void) fgets(line, sizeof(line), stdin);*/
    return TRUE;
}


/************************************************************************/
/*                                                                       
/*    VisitBioseqs:                                                      
/*        Visits all Bioseqs and (Bioseqs inside of BioseqSets)           
/*        recursively. The ParseBioseq function is called for each Bioseq
/*        to extract releveant info.  In addition, coding region data is  
/*        extracted from each BioseqSet by a call to ParseCDRInfo          
/*        If CDR info is not found in  a BioseqSet, a flag is set to      
/*        attempt to find it in the individual Bioseqs of the set          
/*                                                                       
/*    PARAMETERS:                                                          
/*        SeqEntryPtr    pse         current SeqEntry                  
/*        Int4           callno      number of times that VisitBioseqs  
/*                                   has    been called    recursively     
/*                                                                        
/*        BioseqSetPtr   last_pbss   maintains a pointer to the BioseqSet
/*                                   directly above the current Bioseq    
/*        SeqDescrPtr    pdesset     pointer to description of BioseqSet 
/*                                                                       
/*                                                                       
/*                                                                       
/*    RETURNS:                                                            
/*        TRUE-    success                                                   
/*        FALSE-    no success                                            
/************************************************************************/

Boolean VisitBioseqs(SeqEntryPtr pse, Int4 callno, BioseqSetPtr last_pbss, SeqDescrPtr pdesset)
{
    
    BioseqPtr        pbs =                NULL;    /*this is a pointer to a Bioseq*/    
    BioseqSetPtr    pbss =                NULL;    /*this is a pointer to a BioseqSet*/
    SeqEntryPtr        pseNew =            NULL;    /*this is a pointer to a new set of SeqEntrys inside a BioseqSet*/
    SeqDescPtr        current_pdesset =    NULL;    
    /*char            line[100];*/
    Int4            function_report=    0;

    /****************************************/
    /*            process Bioseq                */
    /****************************************/
    if (pse->choice == 1) /* It is a Bioseq */
    {
        /*get a pointer to this next Bioseq*/
        pbs = (BioseqPtr) pse->data.ptrvalue;
        if (pbs == NULL)
        {
            ErrPostEx(SEV_ERROR,0,0, "VisitBioseqs: Bioseq pointer is NULL.\n");
            return FALSE;
        }

        /*count the number of Bioseqs parsed*/
        ++number_bioseqs;
        
        if (!ParseBioseq(pbs, last_pbss, pdesset))
        {
            ErrPostEx(SEV_ERROR,0,0, "VisitBioseqs: ParseBioseq failed.\n");
            return FALSE;
        }
    }

    /****************************************/
    /*            process BioseqSet            */
    /****************************************/
    /* it is nested Bioseq-set extract Bioseqs recursively */
    else if (pse->choice == 2) 
    {
        /*diagnostic line*/
        ++number_bioseqsets;
        /*printf("pse->choice is     2\n");*/       
        
        /*get the pointer to the Bioseq-set*/
         pbss = (BioseqSetPtr) pse->data.ptrvalue;
         if (pbss == NULL)
          {
              ErrPostEx(SEV_ERROR,0,0, "VisitBioseqs: Bioseq-Set pointer is NULL.\n");
              return FALSE;
          }
        
        /*get the pointer to the Bioseq-set descriiotion and the title of the Bioseq-set*/
        if ((callno == 1) && ((pbss->descr) != NULL))
        {        
            pdesset = pbss->descr;        /*pdesset is a pointer to a description in a Bioseq-set; i.e., this description 
                                        applies to all Bioseqs contained in this Bioseq-set*/
            
            /*find the title of the Bioseq-set*/
            current_pdesset = pdesset;
            while (current_pdesset != NULL)
            {
                 if (current_pdesset->choice == 5) /*title*/
                 {
                    name_ptr = current_pdesset->data.ptrvalue;
                    break;
                 }
                 else
                 {
                    current_pdesset = current_pdesset->next;
                 }
            }
        }

        /*if the BioseqSet is a nucprot then extract the cdregion data*/
        if ((bss_class = pbss->_class) == 1) /*if bss_class =1 then the Bioseq-set is a nuc-prot*/
        {
            /*extract cdregion data from the feaure table to the CDR database*/
            function_report = ParseCDRInfo(pbss, NULL);
            if (function_report == 1)
            {
                cdr_flag = 1;/*sets a flag to look for coding region info in the Bioseq inside this BioseqSet*/
                function_report = 0;
            }    
        }

        /*the core of the recursive function*/
        pseNew = pbss->seq_set;
        while (pseNew != NULL)  /* get all Bioseqs recursively */
        {
            if(!VisitBioseqs(pseNew, ++callno, pbss, pdesset))
            {
                ErrPostEx(SEV_ERROR,0,0, "VisitBioseqs: The recursive call to VisitBioseqs failed.\n");
                return FALSE;
            }
            pseNew = pseNew->next;
        }
    }
    else
    {
         ErrPostEx(SEV_ERROR,0,0, "VisitBioseqs: Invalid choice field.\n");
         return FALSE;
    }
   return TRUE;
}


/********************************************************/
/*                            
/*    MAIN:                        
/*                           
/*    PARAMETERS:                    
/*        command line                
/*        -i    RefSeq_input_filename(binary)    
/*        -c    (T/F) Create databases if they do not    
/*            already exist                                
/*        -y    filename for yeast names                   
/*    RETURNS:                                            
/*        0    ok                                           
/*        1    datafile(s) failed to open                   
/********************************************************/
Int2 Main(void)
{  
    /*declarations and initializations*/
    AsnIoPtr        aipin = NULL;        /*an AsnIoPtr to the binary input file of Bioseq-sets*/
    BioseqSetPtr    pbss = NULL;
    AsnTypePtr      atp = NULL;
    AsnTypePtr      atp2 = NULL;
    AsnModulePtr    amp = NULL;
    Char            pathinput[PATH_MAX];
    SeqEntryPtr     sep = NULL;            /*a SeqEntry pointer*/
    CharPtr         pfilenam = NULL;
    char            line[100];            

    Char            path_to_log[PATH_MAX];
    Char            path_to_yeast_names[PATH_MAX];
    FILE            *pFile = NULL;
    time_t          time1, time2, time3;
    Char            daytime[100];
    
    time1 = GetSecs();

    /*find path to log files (see NCBI toolkit corelib 
    and file "prebind.ini" in windows directory)*/
    path_to_log[0] = '\0';
    GetAppParam("prebind", "log_files", "parser_path", NULL, path_to_log, (size_t)PATH_MAX * (sizeof(Char)));
    printf("Log entries will be printed to %s\n", path_to_log);
    
    /*set up error handling*/
    ErrSetOptFlags(EO_LOG_SEVERITY | EO_LOG_CODES | EO_LOG_FILELINE | EO_LOG_USRFILE); /*style of log file*/
    ErrSetLogfile(path_to_log, ELOG_BANNER); /*error log file is defined in the prebind initialization file*/
    ErrSetLogLevel(SEV_INFO); /*any errors => SEV_INFO are logged*/
    ErrSetMessageLevel(SEV_MAX); /*any errors => SEV_MAX are displayed as messages to the user*/
    ErrPostEx(SEV_INFO, 0, 0, "==Log begins==");

    /*diagnostic line*/
    ErrPostEx(SEV_INFO,0,0, "PreBIND parser has started");
    printf("PreBIND parser has started\n");

    /* check command line arguments (see NCBI toolkit corelib) */
    if (!GetArgs("pb_parser",NUMARGS, myargs))
          return 1;

    /*initialize codeBase*/
    if (!InitCodeBaseForPreBIND())
    {
        ErrPostEx(SEV_ERROR, codeBase.errorCode, 0, "Main: InitCodeBaseForPreBIND failed.\n");
        error4exitTest(&codeBase); /*quits program */
        return 1;
    }

    /*open database files*/
    if (!OpenACC("ACC", (Boolean) myargs[0].intvalue))
    {
        ErrPostEx(SEV_ERROR, codeBase.errorCode, 0, "Main: OpenACC failed.\n");
        return 1;
    }

    if (!OpenDBDB("DB", (Boolean) myargs[0].intvalue))
    {
        ErrPostEx(SEV_ERROR, codeBase.errorCode, 0, "Main: OpenDBDB failed.\n");
        return 1;
    }

    if (!OpenONAME("ONAME", (Boolean) myargs[0].intvalue))
    {
        ErrPostEx(SEV_ERROR, codeBase.errorCode, 0, "Main: OpenONAME failed.\n");
        return 1;
    }

    if (!OpenOGI("OGI", (Boolean) myargs[0].intvalue))
    {
        ErrPostEx(SEV_ERROR, codeBase.errorCode, 0, "Main: OpenOGI failed.\n");
        return 1;
    }

    if (!OpenCDR("CDR", (Boolean) myargs[0].intvalue))
    {
        ErrPostEx(SEV_ERROR, codeBase.errorCode, 0, "Main: OpenCDR failed.\n");
        return 1;
    }

    if (!OpenLOCUS("LOCUS", (Boolean) myargs[0].intvalue))
    {
        ErrPostEx(SEV_ERROR, codeBase.errorCode, 0, "Main: OpenLOCUS failed.\n");
        return 1;
    }

    if (!OpenLOSYN("LOSYN", (Boolean) myargs[0].intvalue))
    {
        ErrPostEx(SEV_ERROR, codeBase.errorCode, 0, "Main: OpenLOSYN failed.\n");
        return 1;
    }

    if (!OpenYENA("YENA", (Boolean) myargs[0].intvalue))
    {
        ErrPostEx(SEV_ERROR, codeBase.errorCode, 0, "Main: OpenYENA failed.\n");
        return 1;
    }

    if (!OpenSRCH("SRCH", (Boolean) myargs[0].intvalue))
    {
        ErrPostEx(SEV_ERROR, codeBase.errorCode, 0, "Main: OpenSRCH failed.\n");
        return 1;
    }
    
    /*find path to yeastnames file from the prebind.ini directory*/
    path_to_yeast_names[0] = '\0';
    GetAppParam("prebind", "parser_input_files", "yeast_names", NULL, path_to_yeast_names, (size_t)PATH_MAX * (sizeof(Char)));

    /*open the yeast names file*/
    if ((pFile = FileOpen(path_to_yeast_names, "r")) == NULL)
    {
        printf("The path '%s' is not valid path to a file of yeast names.  Please set a valid path for 'yeast_names' under 'paser_input files' in the configuration file.\n",path_to_yeast_names );
        ErrPostEx(SEV_ERROR, 0, 0, "Main: failed to open path to file of yeast names: %s.\n", path_to_yeast_names);
        return 1;
    }

    /*close the yeast names file*/
    FileClose(pFile);
    
    /*diagnostic line*/
    ErrPostEx(SEV_INFO, 0, 0, "PreBIND parser is finished opening databases and testing input files.");
    printf("PreBIND parser is finished opening databases and testing input files.\n");

    /*build the SeaRCH database*/
    ErrPostEx(SEV_INFO, 0, 0, "PreBIND parser is building the SeaRCH database.");
    printf("PreBIND parser is building the SeaRCH database\n");
    if (BuildSRCH() != 0)
    {
        ErrPostEx(SEV_ERROR,0,0, "Main: BuildSRCH failed.\n");
    }

    ErrPostEx(SEV_INFO, 0, 0, "Finished building the SeaRCH database.");
    printf("Finished building the SeaRCH database.\n");

    /*set up for AsnReadId reading; i.e. get amp and atp: see p. 93 of AsnLib and AsnReadId below*/
    /* load the sequence alphabets  */
    /* (and sequence parse trees)   */
    if (!SeqEntryLoad())
         ErrShow();
      
    /* get pointer to all loaded ASN.1 modules */
    amp = AsnAllModPtr();
    if (amp == NULL)
    {
        ErrShow();
        return 1;
    }

    /* get the initial type pointers */
    atp = AsnFind("Bioseq-set");    
    if (atp == NULL)
    {
        ErrShow();
        return 1;
    }

    atp2 = AsnFind("Bioseq-set.seq-set.E");
    if (atp2 == NULL)
    {
        ErrShow();
        return 1;
    }

    /*diagnostic line*/
    time2 = GetSecs();
    time3 = time2 - time1;
    DayTimeStr(daytime, TRUE, TRUE);
    ErrPostEx(SEV_INFO, 0, 0, "PreBIND parser is finished setting up for AsnRead. time = %ld. [ %s ]", time3, daytime);
    printf("PreBIND parser is finished setting up for AsnRead. time = %ld. [ %s ]\n", time3, daytime);

    /* open the ASN.1 input file in the read binary (rb) mode */
    /* note the RefSeq binary input file is of the type BioseqSet */
    /*find path name of input file (see NCBI toolkit corelib and file "prebind.ini" in windows directory)*/
    pathinput[0] = '\0';
    GetAppParam("prebind", "parser_input_files", "refseq_binary", NULL, pathinput, (size_t)PATH_MAX * (sizeof(Char)));

    if ((aipin = AsnIoOpen (pathinput, "rb")) == NULL)
    {
        printf("The path '%s' is not valid path to the RefSeq binary file.  Please set a valid path for 'refseq_binary' under 'paser_input files' in the configuration file.\n",pathinput);
        ErrPostEx(SEV_ERROR,0,0, "Main: Cannot open %s.\n", pathinput);
        return 1;
    }

    /*diagnostic line*/
    ErrPostEx(SEV_INFO, 0, 0, "PreBIND parser is reading through Seq-entries.");
    printf("PreBIND parser is reading through Seq-entries\n");

    /* read Seq-entry's from a Bioseq-set */
    while ((atp = AsnReadId(aipin, amp, atp)) != NULL)
    {
        if (atp == atp2)    /* top level Seq-entry */
        {
            sep = SeqEntryAsnRead(aipin, atp);
            if (sep == NULL)
            {
                ErrPostEx(SEV_ERROR,0,0, "Main: Seq-entry pointer is NULL.\n");
                return 1;
            }
            /* ErrPostEx(SEV_ERROR,0,0, "Main: Got Seq-entry pointer.\n"); */
            if (!VisitBioseqs(sep, 1, NULL, NULL))
            {
                ErrPostEx(SEV_ERROR,0,0, "Main: Call to VisitBioseqs failed.\n");
                return 1;
            }
            SeqEntryFree(sep);
        }
        else
            /* don't need to read the data */
            AsnReadVal(aipin, atp, NULL);
    }

    time1 = GetSecs();
    time3 = time1 - time2;
    DayTimeStr(daytime, TRUE, TRUE);
    /*print out the number of records parsed (see VisitBioseqs)*/
    ErrPostEx(SEV_INFO, 0, 0, "PreBIND parser has finished reading through Seq_entries. "
                                  "time = %ld. [ %s ]", time3, daytime);
    ErrPostEx(SEV_INFO, 0, 0, "Number bioseqsets read = \t\t%d\n", number_bioseqsets);
    ErrPostEx(SEV_INFO, 0, 0, "Number bioseqs read = \t\t%d\n", number_bioseqs);
    ErrPostEx(SEV_INFO, 0, 0, "Number proteins found = \t\t%d\n",number_proteins);
    
    printf("PreBIND parser has finished reading through Seq_entries "
               "time = %ld. [ %s ]\n", time3, daytime);
    printf("Number bioseqsets read = \t%d \n", number_bioseqsets);
    printf("Number bioseqs read =    \t%d \n", number_bioseqs);
    printf("Number proteins found =  \t%d \n",number_proteins);

    /*close the binary stream*/
    AsnIoClose(aipin);

    /*build the locus database*/
    ErrPostEx(SEV_INFO, 0, 0, "PreBIND parser is building the locus database.");
                  
    printf("PreBIND parser is building the locus database.\n");
    if (BuildLOCUS() != 0)
    {
        ErrPostEx(SEV_ERROR,0,0, "Main: BuildLOCUS failed.\n");
    }

    time2 = GetSecs();
    time3 = time2 - time1;
    DayTimeStr(daytime, TRUE, TRUE);
    /*build the locus synonym (losyn) database*/
    ErrPostEx(SEV_INFO, 0, 0, "Locus database took %ld. [ %s ]", time3, daytime);
    ErrPostEx(SEV_INFO, 0, 0, "PreBIND parser is building the synonym database.");
    printf("PreBIND parser is building the locus synonym database. \n" );
               
    if (BuildLOSYN() != 0)
    {
        ErrPostEx(SEV_ERROR,0,0, "Main: BuildLOCUS failed.\n");
    }

    time1 = GetSecs();
    time3 = time1 - time2;
    DayTimeStr(daytime, TRUE, TRUE);
    ErrPostEx(SEV_INFO, 0, 0, "Locus synonym  database took %ld. [ %s ]", time3, daytime);
    /*build the DB (DataBase) database; ie find any missing gi values for proteins*/
    ErrPostEx(SEV_INFO, 0, 0, "PreBIND parser is building the Database database.");
    printf("PreBIND parser is building the DataBase database.\n");
    if (BuildDB() != 0)
    {
        ErrPostEx(SEV_ERROR,0,0, "Main: BuildDB failed.\n");
    }


    time2 = GetSecs();
    time3 = time2 - time1;
    DayTimeStr(daytime, TRUE, TRUE);
    ErrPostEx(SEV_INFO, 0, 0, "Database  database took %ld. [ %s ]", time3, daytime);
    /*build the ACC (ACCession) database; ie fill in the encoding LOCUS names for each protein gi*/
    ErrPostEx(SEV_INFO,0,0,"PreBIND parser is building the ACCession database. ");    
    printf("PreBIND parser is building the ACCession database. \n");

    if (BuildACC() != 0)
    {
        ErrPostEx(SEV_ERROR,0,0, "Main: BuildDB failed.\n");
    }

    time1 = GetSecs();
    time3 = time1 - time2;
    DayTimeStr(daytime, TRUE, TRUE);
    ErrPostEx(SEV_INFO, 0, 0, "ACCession  database took %ld. [ %s ]", time3, daytime);
    /*import names from SGD into the YENA database*/
    ErrPostEx(SEV_INFO, 0, 0, "PreBIND parser is building the YEast "
                  "NAme database.");
    printf("PreBIND parser is importing yeast loci names to the "
               "YEast NAme database.\n");
    if (ImportYeastNames() != 0)
    {
        ErrPostEx(SEV_ERROR,0,0, "Main: ImportYeastNames failed.\n");
    }

    time2 = GetSecs();
    time3 = time2 - time1;
    DayTimeStr(daytime, TRUE, TRUE);
    ErrPostEx(SEV_INFO, 0, 0, "YEast NAme  database took %ld. [ %s ]", time3, daytime);

    /*    (void) fgets(line, sizeof(line), stdin);*/

    /*build the SeaRCH database*/
    ErrPostEx(SEV_INFO,0,0, "PreBIND parser is building the SeaRCH database.");
    printf("PreBIND parser is building the SeaRCH database.\n");
    if (BuildSRCH() != 0)
    {
        ErrPostEx(SEV_ERROR,0,0, "Main: BuildSRCH failed.\n");
    }

    time1 = GetSecs();
    time3 = time1 - time2;
    DayTimeStr(daytime, TRUE, TRUE);  
    ErrPostEx(SEV_INFO, 0, 0, "SeaRCH  database took %ld. [ %s ]", time3, daytime);
    /*closes all open data, index and memo files*/
    code4close(&codeBase);        
    code4initUndo(&codeBase);    

    /*Finish the log*/
    ErrPostEx(SEV_INFO, 0, 0, "==Log ends==");
    printf ("Finished\n");

    return 0;
}

/*
$Log: prebind_parser.c,v $
Revision 1.5  2004/01/29 21:56:09  kzheng
filter "no-primary-locus-name" as "N/A", leave add quota to searcher, show daytime; remove tabs.

Revision 1.4  2004/01/16 15:09:37  eburgess
Added functions to simplify the code and to increase the functionality.f

Revision 1.3  2002/07/30 15:31:23  janicek
fixed compiler warning

Revision 1.2  2002/07/30 15:21:01  janicek
changed GetGI to PreBINDGetGI

Revision 1.1.1.1  2002/07/17 19:36:35  janicek
initial import

*/


