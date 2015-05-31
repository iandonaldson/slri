/**$Id: prebind_searcher.c,v 1.24 2004/03/26 22:08:03 kzheng Exp $
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


/*pb_searcher.c*/
/*this is a console application in the PreBIND_searcher project made using the MCD build of the NCBI toolkit*/
/*the following libraries have been included in the Project Settings
c4lib.lib ncbicdr.lib medarch.lib ncbi.lib ncbiid1.lib ncbimmdb.lib ncbiobj.lib ncbinacc.lib 
ncbitool.lib ncbitxc2.lib netblast.lib netcli.lib netentr.lib ncbimain.obj prebindlib.lib 
kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib 
ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib 

switching the order of ncbimmdb.lib and ncbiobj.lib will produce errors and provide hours of entertainment

*/

/*this includes 
the Codebase c4lib.lib
libraries from the MCD build of the NCBI toolkit
the prebindlib.lib library
all of the default libraries that start with a w32 Console Application
and the wsock32.lib
in that order*/


#ifndef _PREBIND_SEARCHER_
#include "prebind_searcher.h" /*prebind_searcher specific includes and definitions*/
#endif


/*set up command line arguments*/
#define NUMARGS 19
#define MAXLINE 500
Args myargs[NUMARGS] = 
{
    {"Create databases if they do not already exist(T/F(default))",        "F",   NULL, NULL, TRUE,  'c', ARG_BOOLEAN, 0.0, 0, NULL},
    {"Search type: (n)ew only, (u)pdates only, (a)ll types (default)",     "x",   NULL, NULL, TRUE,  's', ARG_STRING,  0.0, 0, NULL},
    {"Taxon-specific search/export (enter taxid)",                         "0" ,  NULL, NULL, TRUE,  't', ARG_INT,     0.0, 0, NULL},
    {"Allow requeuing of previously peformed searches (T/F(default))",     "F",   NULL, NULL, TRUE,  'r', ARG_BOOLEAN, 0.0, 0, NULL},
    {"Build the ISVM and CORE databases only (T/F(default))",              "F",   NULL, NULL, TRUE,  'a', ARG_BOOLEAN, 0.0, 0, NULL},
    {"Import SVM interaction scores only (enter filename)",                NULL,  NULL, NULL, TRUE,  'i', ARG_STRING,  0.0, 0, NULL},
    {"Build potential interaction database (POINT) only (T/F(default))",   "F",   NULL, NULL, TRUE,  'p', ARG_BOOLEAN, 0.0, 0, NULL},
    {"Import POINT scores only and then quit (T/F(default)",               "F",   NULL, NULL, TRUE,  'n', ARG_BOOLEAN, 0.0, 0, NULL},
    {"Score_list to import scores into(enter a number (1, 2 or 3))",       "0" ,  NULL, NULL, TRUE,  'l', ARG_INT,     0.0, 0, NULL},
    {"User must be present for searches (non-automatic)(T(default)/F)",    "T",   NULL, NULL, TRUE,  'u', ARG_BOOLEAN, 0.0, 0, NULL},
    {"Maximum number of searches to perform at one time (default 50)",     "50" , NULL, NULL, TRUE,  'm', ARG_INT,     0.0, 0, NULL},
    {"Remove all search results except for taxon x (default -1) and quit", "-1" , NULL, NULL, TRUE,  'x', ARG_INT,     0.0, 0, NULL},
    {"Calculate name redundancy and quit (T/F(default))",                  "F" ,  NULL, NULL, TRUE,  'h', ARG_BOOLEAN, 0.0, 0, NULL},
    {"Overwrite scores already present.  (T/F(default))",                  "F" ,  NULL, NULL, TRUE,  'o', ARG_BOOLEAN, 0.0, 0, NULL},
    {"Import RESULT Table from a tab deliminated text file",               NULL,  NULL, NULL, TRUE,  'R', ARG_STRING,  0.0, 0, NULL},
    {"Import POINT Table from a tab deliminated text file",                NULL,  NULL, NULL, TRUE,  'P', ARG_STRING,  0.0, 0, NULL},
    {"Export POINTS to be scored. (enter filename)",                       NULL  ,NULL, NULL, TRUE,  'e', ARG_STRING,  0.0, 0, NULL},
    {"Export POINTS/RESULTs in non-interaction papers (T/F(default))",  "F" ,  NULL, NULL, TRUE,  'y', ARG_BOOLEAN, 0.0, 0, NULL},
    {"Export POINTS already scored (rescore?)(T/F(default))",           "F" ,  NULL, NULL, TRUE,  'w', ARG_BOOLEAN, 0.0, 0, NULL},
};

/*sample command line options for debugging*/
/* -c T -s a -t 10090 -u T -m 10 :to search*/
/* -z T -f export_file_a.txt -y T -w T : to export POINTS even if they have been scored already*/
/* -b T -f export_file_a.txt -y T : to export RESULTs even if they are in non-interaction papers*/

/*   LocalLinkSetNew */
static LocalLinkSetPtr LocalLinkSetNew(void)
{
    LocalLinkSetPtr lsp;

    lsp = MemNew(sizeof(*lsp));
    lsp->num = 0;
    lsp->uids = NULL;
    lsp->weights = NULL;

    return lsp;
}


/* LocalLinkSetFree */
static LocalLinkSetPtr LocalLinkSetFree(LocalLinkSetPtr lsp)
{
    MemFree(lsp->uids);
    MemFree(lsp->weights);
    MemFree(lsp);

    return NULL;
}


/* BSPtoLSP */
static LocalLinkSetPtr BSPtoLSP(ByteStorePtr bsp)
{
    LocalLinkSetPtr lsp;
    
    if (bsp == NULL)
        return NULL;

    lsp = LocalLinkSetNew();

    lsp->num = BSLen(bsp) / sizeof(DocUid);
    if ((lsp->uids = MemNew(BSLen(bsp))) == NULL)
    { /* platforms which can't allocate this are out of luck */
        lsp = LocalLinkSetFree(lsp);
    } else {
        BSSeek (bsp, 0L, 0);
        BSRead (bsp, lsp->uids, lsp->num * sizeof (DocUid));
    }

    return lsp;
}


/********************************************************/
/*                                                      */
/*   ExportPoints:   cycle through the POINT db and      */
/*                   write POtential INTeractions to be  */
/*                   scored to a text file               */
/*                                                       */
/*   PARAMETERS:     relevant command line params are:   */
/*                   -f file to export to                */
/*                   -u is user present                  */
/*                   -t tax_select                       */
/*                   -y score_points_in_non_int_papers   */
/*                   -w rescore                          */
/*                                                       */
/*   RETURNS:                                            */
/*       Int2    0: success                              */
/********************************************************/
Int2 ExportPoints(void)
{
    FILE*   export_file_ptr =       NULL;
  Int4    number_records =        0;
 Char    line[100];
 Int4    point_record =          0;
 Int4    cur_pmid =              0;
 Int4    cur_gia =               0;
 CharPtr cur_nama =              NULL;
  Int4    cur_gib =               0;
 CharPtr cur_namb =              NULL;
  Int4    piscore =               0;
 FloatHi isvm =                  0.0;
   Int4    tax_select =            myargs[2].intvalue;
    Int4    cur_taxa =              0;
 Int4    cur_taxb =              0;
 Int4    cur_taxi =              0;
 Boolean score_points_in_non_int_papers = myargs[17].intvalue;
  Boolean rescore =               myargs[18].intvalue;
   Boolean print_to_file =         FALSE;
 /*counters - see report printout at end*/
  Int4    counter =               0;
 Int4    counter_multiplier =    0;
 Int4    p =                     0;
 Int4    t =                     0;
 Int4    a =                     0;
 Int4    b =                     0;
 Int4    c =                     0;
 Int4    d =                     0;
 Int4    e =                     0;
 Int4    f =                     0;
 Int4    g =                     0;

    AssignTagsACC();
    AssignTagsPOINT();
    AssignTagsISVM();
    
   /*open file for export*/
   if ((export_file_ptr = FileOpen(myargs[16].strvalue, "w")) == NULL)
    {
      printf("The path '%s' is not a valid path to the export file.  Please give a file name using the -y command line parameter.\n", myargs[15].strvalue);
      ErrPostEx(SEV_ERROR, 0, 0, "ExportPoints: Failed to open file %s for export.", myargs[15].strvalue);
       return 1;
  }


    /*diagnostic line*/
    printf("PreBIND searcher is exporting POtential INTeractions to be scored to a text file called %s (ExportPoints)\n", myargs[15].strvalue);
    fprintf(results_file_ptr, "PreBIND searcher is exporting POtential INTeractions to be scored to a text file called %s(ExportPoints)\n", myargs[15].strvalue);
  number_records = d4recCount(pd4_POINT);
    printf("Total records in POINT database to process: %ld\n", (long) number_records);
    fprintf(results_file_ptr, "Total records in POINT database to process: %ld\n", (long) number_records);
 printf("Limit export of POtential INTeraction to taxon: %ld\n", (long) tax_select);
    fprintf(results_file_ptr, "Limit export of POtential INTeraction to taxon: %ld\n", (long) tax_select);
 printf("Print POtential INTeractions for scoring if they are in non-interaction papers? %s\n",(score_points_in_non_int_papers)?"YES":"NO");
    fprintf(results_file_ptr, "Print POtential INTeractions for scoring if they are in non-interaction papers? %s\n",(score_points_in_non_int_papers)?"YES":"NO");
 printf("Print POtential INTeractions for scoring if they have already been scored? %s\n",(rescore)?"YES":"NO");
    fprintf(results_file_ptr, "Print POtential INTeractions for scoring if they have already been scored? %s\n",(rescore)?"YES":"NO");
 fflush(results_file_ptr);

 /*continue- by users consent or automatically if user is absent (see option -u)*/
  if ((Boolean) myargs[9].intvalue == TRUE) /*user is present*/
  {
      printf("Press the 'y' key and return to proceed. Press any other key to quit.\n");
     (void) fgets(line, sizeof(line), stdin);
       line[StrLen(line)-1] = '\0'; /*remove the newline character*/
      switch (*line)
     {
      case 'y':/*do nothing and continue*/break;
     case 'Y':/*do nothing and continue*/break;
     default:
           /*quit*/ 
          printf("Nothing will be done. \n"); 
           fprintf(results_file_ptr, "Nothing will be done.\n");
          fflush(results_file_ptr);
          return 0;
      }
  }
  else /*user is absent*/
    {
      printf("Proceeding with automatic export.\n");
     fprintf(results_file_ptr, "Proceeding with automatic export.\n");
  }



   /*cycle through the POtential INTeraction db*/
 for (point_record = d4top(pd4_POINT); point_record == 0; point_record = d4skip(pd4_POINT, 1L))
 {
      ++p;

      /*let the user know how many records are left to process*/
     ++counter;
     if (counter == 1000)
       {
          counter = counter * counter_multiplier;
            printf("POINT records analyzed: %ld\n", (long) counter);
           counter = 0;
           ++counter_multiplier;
      }

     /*get relevant data from record*/
      cur_pmid = f4long(pf4_POINT_PMID);

        cur_gia = f4long(pf4_POINT_GIA);
       cur_nama = StrSave(f4str(pf4_POINT_NAMA));
     if (cur_nama == NULL) cur_nama = StrSave("NOT FOUND");
     if(TrimSpaces(cur_nama) != 0) ErrPostEx(SEV_ERROR,0,0, "ExportPoints: TrimSpaces failed, possibly passed a null string");
          
       cur_gib = f4long(pf4_POINT_GIB);
       cur_namb = StrSave(f4str(pf4_POINT_NAMB));
     if (cur_namb == NULL) cur_namb = StrSave("NOT FOUND");
     if(TrimSpaces(cur_namb) != 0) ErrPostEx(SEV_ERROR,0,0, "GetPointbyPMIDandGIs: TrimSpaces failed, possibly passed a null string");
          
       piscore = f4long(pf4_POINT_PISCORE);

      /*find this pmid in the ISVM database get its ISCORE*/
     d4tagSelect(pd4_ISVM, pt4_ISVM_PMID);
      if (d4seekDouble(pd4_ISVM, cur_pmid) == 0)
     {
          isvm = f4double(pf4_ISVM_ISCORE);
      }
      else isvm = 0;

        
       /*find the taxid of the interaction*/
      if (tax_select != 0)
       {
          /*get tax of a*/
           d4tagSelect(pd4_ACC, pt4_ACC_GI);
          if (d4seekDouble(pd4_ACC, cur_gia) == 0)
           {
              cur_taxa = f4long(pf4_ACC_TAXID);
          }
          else 
          {
              cur_taxa = 0;
              ErrPostEx(SEV_ERROR,0,0, "ExportResults: Taxid not found in ACC db for gi %ld\n", (long) cur_gia);
         }
          /*get tax of b*/
           if (d4seekDouble(pd4_ACC, cur_gib) == 0)
           {
              cur_taxb = f4long(pf4_ACC_TAXID);
          }
          else 
          {
              cur_taxb = 0;
              ErrPostEx(SEV_ERROR,0,0, "ExportResults: Taxid not found in ACC db for gi %ld\n", (long) cur_gib);
         }
          /*determine taxon of interaction*/
         if (cur_taxa == cur_taxb)
          {
              cur_taxi = cur_taxa;
           }
          else
           {
              cur_taxi = -1;
             ErrPostEx(SEV_ERROR,0,0, "ExportResults: Potential interaction between gia %ld and gib %ld in pmid %ld is for two different taxons!\n", (long) cur_gia, (long) cur_gib, (long) cur_pmid);
          }
      }
      


     /*decide if this POINT is to be printed out for scoring*/ 
     if (tax_select == 0 || cur_taxi == tax_select)
     {
          ++t;
           if (isvm <= 0) /*a non-interaction paper*/
         {
              if (score_points_in_non_int_papers == TRUE) 
               {
                  if (rescore == FALSE)
                  {
                      if (piscore == 0)
                      {
                          print_to_file = TRUE;/*print POINTS in non-int papers that have not been scored*/
                          ++a;
                       }
                      else ++b;
                      /*dont print POINTS in non-int papers that have already been scored*/
                  }
                  else 
                  {
                      print_to_file = TRUE; /*print POINTS in non-int papers even if they have been scored*/
                     ++c;
                   }
              }
              else ++d;
              /*dont print POINTS in non-int papers*/
            }
          else if (isvm > 0)
         {
              if (rescore == FALSE)
              {
                  if (piscore == 0)
                  {
                      print_to_file = TRUE;/*print POINTS in int papers that have been scored*/
                      ++e;
                   }
                  else ++f;
                  /*dont print POINTS in int papers that have already been scored*/
              }
              else 
              {
                  print_to_file = TRUE; /*print POINTS in all int papers even if they have been scored*/
                 ++g;
               }
          }
      }

     /*print to file*/
      if (print_to_file)
     {
          fprintf(export_file_ptr, "%ld\t%ld\t%s\t%ld\t%s\t%ld\n",(long) cur_pmid, (long) cur_gia, cur_nama, (long) cur_gib, cur_namb, (long) piscore);
          fflush(export_file_ptr);
       }
      
       /*reset*/
      cur_pmid =              0;
     cur_gia =               0;
     if(cur_nama) cur_nama = MemFree(cur_nama);
     if(cur_namb) cur_namb = MemFree(cur_namb);
     cur_gib =               0;
     cur_namb =              NULL;
      piscore =               0;
     isvm =                  0.0;
       cur_taxa =              0;
     cur_taxb =              0;
     cur_taxi =              0;
     print_to_file =         FALSE;

    }

 /*print report*/
   printf("Number of POtential INTeraction records examined: %ld\n", (long) p);
   printf("Number of POtential INTeraction records considered for export because they matched the selected taxon (%ld): %ld\n", (long) tax_select, (long) t);
 printf("Number of non-scored POINTS in non-interaction papers that were exported: %ld\n", (long) a);
   printf("Number of scored POINTS in non-interactions papers NOT exported: %ld\n", (long) b);
    printf("Number of POINTS non-interaction papers that were exported: %ld\n", (long) c);
 printf("Number of POINTS in non-interaction papers that were NOT exported: %ld\n", (long) d);
  printf("Number of non-scored POINTS in interaction papers that were exported: %ld\n", (long) e);
   printf("Number of scored POINTS in interaction papers that were NOT exported: %ld\n", (long) f);
   printf("Number of POINTS in interaction papers that were exported: %ld\n", (long) g);

 fprintf(results_file_ptr, "Number of POtential INTeraction records examined: %ld\n", (long) p);
    fprintf(results_file_ptr, "Number of POtential INTeraction records considered for export because they matched the selected taxon (%ld): %ld\n", (long) tax_select, (long) t);
  fprintf(results_file_ptr, "Number of non-scored POINTS in non-interaction papers that were exported: %ld\n", (long) a);
    fprintf(results_file_ptr, "Number of scored POINTS in non-interactions papers NOT exported: %ld\n", (long) b);
 fprintf(results_file_ptr, "Number of POINTS non-interaction papers that were exported: %ld\n", (long) c);
  fprintf(results_file_ptr, "Number of POINTS in non-interaction papers that were NOT exported: %ld\n", (long) d);
   fprintf(results_file_ptr, "Number of non-scored POINTS in interaction papers that were exported: %ld\n", (long) e);
    fprintf(results_file_ptr, "Number of scored POINTS in interaction papers that were NOT exported: %ld\n", (long) f);
    fprintf(results_file_ptr, "Number of POINTS in interaction papers that were exported: %ld\n", (long) g);
   fflush(results_file_ptr);

 return 0;

}


/**
*****************************************************
*                                                     
* CalculateNameUsage                                    
*     cycle through the SeaRCH database and for each    
*     name calculate the number of times that it is    
*     used in that taxon.  This number is stored in    
*     NAMERED field of the SeaRCH database            
* @param                                            
*     none                                            
* @return                                            
*     Int2    0: success                                
********************************************************/
Int2 CalculateNameUsage(void)
{
    Int4        number_records =            0;
    Int4        loop =                        0;
    Int4        srch_record =                0;
    Int4        cur_namered =                0;
    CharPtr        cur_name =                    NULL;
    Int4        cur_taxid =                    0;
    Int4        cur_gi=                        0;
    Int4        matching_name_record =        0;
    Int4        match_taxid =                0;
    Int4        match_gi =                    0;
    Int4        name_redundancy =            1;
    
    AssignTagsSRCH();
    /*diagnostic line*/
    printf("PreBIND searcher is calculating name redundancies.\n");
    fprintf(results_file_ptr, "PreBIND searcher is calculating name redundancies.\n");
    
    /*cycle through each entry in the search database and reset the name redundancy count to 0*/
    for (srch_record = d4top(pd4_SRCH); srch_record == 0; srch_record = d4skip(pd4_SRCH, 1L))
    {
        f4assignInt (pf4_SRCH_NAMERED, 0);    
    }
    printf("PreBIND searcher has reset all name redundancies to zero.\n");
    fprintf(results_file_ptr, "PreBIND searcher has reset all name redundancies to zero.\n");
    

    /*cycle through each entry in the SRCH database and analyze the names in the search db*/
    number_records = d4recCount(pd4_SRCH);
    for (loop = 1; loop <= number_records; ++loop)
    {
        
        d4go(pd4_SRCH, loop);
        
        cur_namered = f4int(pf4_SRCH_NAMERED);
        /*value of namered should never be zero if it has been calculated before*/
        if (cur_namered == 0)
        {
            cur_name = StrSave(f4str(pf4_SRCH_NAME));
            cur_taxid = f4int(pf4_SRCH_TAXID);
            cur_gi = f4int(pf4_SRCH_GIP);
            if (cur_name == NULL || cur_taxid < 0 || cur_gi < 0)
            {
                ErrPostEx(SEV_INFO,0,0, "CalculateNameUsage.  Nonsensical value. name: %s taxon: %ld gi: %ld", cur_name, (long) cur_taxid, (long) cur_gi);
                continue;
            }
            
            /*look for the name elsewhere in the db and count the number of times that it appears*/
            d4tagSelect(pd4_SRCH, pt4_SRCH_NAME);
            matching_name_record = d4seek(pd4_SRCH, cur_name);
            
            while (matching_name_record == 0)
            {
                /*get taxid for the current record*/
                match_taxid = f4long(pf4_SRCH_TAXID);
                if (match_taxid < 0)
                {
                    ErrPostEx(SEV_INFO,0,0, "CalculateNameUsage.  Nonsensical value. name: %s match_taxid: %ld gi: %ld", cur_name, (long) match_taxid, (long) cur_gi);
                    continue;
                }
                
                /*get the gi for the current record- this is used below to get info from the accession database*/
                match_gi = f4long(pf4_SRCH_GIP);
                if (match_gi < 0)
                {
                    ErrPostEx(SEV_INFO,0,0, "CalculateNameUsage.  Nonsensical value. name: %s match_taxid: %ld match_gi: %ld", cur_name, (long) match_taxid, (long) match_gi);
                    continue;
                }
                
                /*increase the name redundancy only if the name is used for another protein in the same taxon*/
                if ((cur_taxid == match_taxid) && (cur_gi != match_gi))
                {
                    ++name_redundancy;
                }
                match_gi = 0;
                match_taxid = 0;
                matching_name_record = d4seekNext(pd4_SRCH, cur_name);/*look for the next match*/
                
            }/*while there is a matching name record*/
            
            if (name_redundancy > 1)
            {
                fprintf(results_file_ptr, "name: %s taxon: %ld  redund: %ld\n", cur_name, (long) cur_taxid, (long) name_redundancy);
            }
            
            /*repeat the above while loop but write name_redundancy value in the NAMERED field for every search record that has that name*/
            d4tagSelect(pd4_SRCH, pt4_SRCH_NAME);
            matching_name_record = d4seek(pd4_SRCH, cur_name);
            
            while (matching_name_record == 0)
            {
                /*get taxid for the current record*/
                match_taxid = f4long(pf4_SRCH_TAXID);
                if (match_taxid < 0)
                {
                    continue;
                }
                
                /*get the gi for the current record- this is used below to get info from the accession database*/
                match_gi = f4long(pf4_SRCH_GIP);
                if (match_gi < 0)
                {
                    continue;
                }
                
                /*write the name redundancy for this occurence of the cur_name only if it is in the same taxon*/
                if (cur_taxid == match_taxid) 
                {
                    f4assignInt(pf4_SRCH_NAMERED, name_redundancy);
                }
                match_gi = 0;
                match_taxid = 0;
                matching_name_record = d4seekNext(pd4_SRCH, cur_name);/*look for the next match*/
                
            }/*while there is a matching name record*/
            
            /*reset the name redundancy*/
            name_redundancy = 1;
            
        }/*only if name redundancy has not already been calculated*/
        
    }/*next record in the SRCH database*/
    
    return 0;
    
}


/**
*****************************************************
*                                                     
* RemoveSearchResultsForAllTaxonsExceptX                
*     Cycle through the SeaRCH database and the        
*     RESULTS database and remove all search results    
*     for all taxons besides taxon x                    
* @param    keep_taxon_searches                     
*                 from command line parameter -x        
* @return                                            
*     Int2    0: success                                
********************************************************/
Int2 RemoveSearchResultsForAllTaxonsExceptX(Int4 keep_taxon_searches)
{

    Char    line[100];
    Int4    srch_record =            0;
    Int4    no_search_results =        0;
    Int4    no_results_to_delete =    0;
    Int4    cur_taxid =                0;
    Int4    cur_gi =                0;
    Int4    result_record =            0;
    Int4    no_results_deleted =    0;
    Int4    total_search_results =  0;
    Int4    number_searches =        0;
    Int4    no_searches_to_delete = 0;
    
    AssignTagsRESULT();
    /*continue- by only by users consent*/
    printf("Press the 'y' key then return to proceed with deleting all search results besides those for taxon %ld. Press any other key to quit.\n", (long) keep_taxon_searches);
    (void) fgets(line, sizeof(line), stdin);
    line[StrLen(line)-1] = '\0'; /*remove the newline character*/
    switch (*line)
    {
    case 'y':/*do nothing and continue*/break;
    case 'Y':/*do nothing and continue*/break;
    default:
        /*quit*/ 
        printf("Nothing will be deleted. Quitting. \n"); 
        /*closes all open data, index and memo files*/
        code4close(&codeBase);        
        code4initUndo(&codeBase);
        return 1;
    }

    printf("PreBIND Searcher is deleting all search results besides those for taxon %ld.\n", (long) keep_taxon_searches);
    fprintf(results_file_ptr, "PreBIND Searcher is deleting all search results besides those for taxon %ld.\n", (long) keep_taxon_searches);

    /*cycle through each entry in the search database and remove search results if not for taxon specified*/
    for (srch_record = d4top(pd4_SRCH); srch_record == 0; srch_record = d4skip(pd4_SRCH, 1L))
    {
        ++number_searches;
        no_search_results = f4int(pf4_SRCH_RESULTS);
        total_search_results = total_search_results + no_search_results;
        cur_taxid = f4int(pf4_SRCH_TAXID);

        if (cur_taxid != keep_taxon_searches && no_search_results > 0)
        {
            ++no_searches_to_delete;
            no_results_to_delete = no_results_to_delete + no_search_results;
            
            /*get the current gi and name*/
            cur_gi = f4long(pf4_SRCH_GIP);

            /*reset results found to zero*/
            f4assignInt(pf4_SRCH_RESULTS, 0);
            f4assignInt(pf4_SRCH_STATUS, 0);
            f4assign   (pf4_SRCH_SEARCHED, "10000101");

            /*find the search results in the RESULTS database and delete*/
            /*select the gi tag in the RESULT datafile*/
            d4tagSelect(pd4_RESULT, pt4_RESULT_GIP);
                
            /*look for the current gi in the RESULT datafile*/
            result_record = d4seekDouble(pd4_RESULT, cur_gi);

            
            /*delete all records with that gi from the RESULT datafile*/
            while (result_record == 0) /*r4success*/
            {
                printf("deleting search result for protein gi = %ld, taxon = %ld\n", (long) cur_gi, (long) cur_taxid );
                fprintf(results_file_ptr,"deleting search result for protein gi = %ld, taxon = %ld\n", (long) cur_gi, (long) cur_taxid );
                d4delete(pd4_RESULT);
                /*f4assignInt(pf4_RESULT_GIP, 0);this does not fix the no_results_deleted problem*/
                ++no_results_deleted;/*this number is wrong*/
                result_record = d4seekNextDouble(pd4_RESULT, cur_gi);
            }


        }/*if search results are to be deleted*/

    }/*for loop cycling through SeaRCH datafile*/

    /*pack the RESULT datafile*/
    d4pack(pd4_RESULT);

    printf("Total number searches %ld \nTotal search results %ld \nTotal searches to delete %ld \nResults found to delete = %ld \nResults deleted (this number is wrong ?) = %ld.\n",
        (long) number_searches, 
        (long) total_search_results,
        (long) no_searches_to_delete,
        (long) no_results_to_delete,
        (long) no_results_deleted);

    fprintf(results_file_ptr, "Total number searches %ld \nTotal search results %ld \nTotal searches to delete %ld \nResults found to delete = %ld \nResults deleted = %ld.\n",
        (long) number_searches, 
        (long) total_search_results,
        (long) no_searches_to_delete,
        (long) no_results_to_delete,
        (long) no_results_deleted);

    return 0;
}


/**
*****************************************************
*                                                     
* AddResultsToISVM:    cycle through the RESULT        
*                     database and add PMIDs if they    
*                     do not already exist            
* @param            none                            
* @return                                            
*     Int2    0: success                                
********************************************************/
Int2 AddResultsToISVM(void)
{    
    Int4    result_record =        0;
    Int4    cur_pmid =            0;
    Int2    append_status =        0;
    Char    daytime[100];

    /*diagnostic line*/
    DayTimeStr(daytime, TRUE, TRUE);
    printf("PreBIND searcher is building the ISVM database. The time is: %s.\n", daytime);
    fprintf(results_file_ptr, "PreBIND searcher is building the ISVM database. The time is: %s.\n", daytime);
    fflush(results_file_ptr);

    /*cycle through the results database and add any pmid numbers that are not already present*/
    for (result_record = d4top(pd4_RESULT); result_record == 0; result_record = d4skip(pd4_RESULT, 1L))
    {
        cur_pmid = f4int(pf4_RESULT_PMID);

        /*try appending the record to the SeaRCH datafile*/
        d4appendStart(pd4_ISVM, 0);
        d4blank(pd4_ISVM);
        
        f4assignLong (pf4_ISVM_PMID, (long) cur_pmid);
        f4assignLong (pf4_ISVM_MUID, 0);
        f4assignDouble (pf4_ISVM_ISCORE, 0.0);
        f4assignDouble (pf4_ISVM_ISCORE2, 0.0);
        f4assignLong (pf4_ISVM_OPNO, 0);
        f4assignLong (pf4_ISVM_OPYES, 0);
        f4assignLong (pf4_ISVM_ITRUTH, 0);

        append_status = d4append(pd4_ISVM);
        
        switch (append_status)
        {
        case r4success:
            break; /*ok result appended*/
        case r4unique:
            break; /*the pmid already exists in the database- old info is not overwritten*/
        default:
            ErrPostEx(SEV_ERROR,0,0, "AddResultsToISVM failed to append pmid = %ld. append_status = %d", (long) cur_pmid, append_status);
            return 1;
        }
            
    }
    
    /*diagnostic line*/
    DayTimeStr(daytime, TRUE, TRUE);
    printf("PreBIND searcher is finished building the ISVM database. The time is: %s.\n", daytime);
    fprintf(results_file_ptr, "PreBIND searcher is finished building the ISVM database. The time is: %s.\n", daytime);
    fflush(results_file_ptr);


    return 0;
}

/**
*****************************************************
*                                                     
* CountResultsByGI:    cycle through the CORE db and    
*                     calculate the number of unique     
*                     gi's that return each PMID    in    
*                     the RESULT database.  Record the
*                     result in the NUMG field of the 
*                     REsults COunt database            
* @param     max_numxr                                        
* @return                                            
*     Int2    0: success                                
********************************************************/
Int2 CountResultsByGI(Int4 max_numr)
{
    Int4*    unique_gip;
    Int2    core_record =            0;
    Int4    index =                    0;
    Int4    cur_pmid =                0;
    Int2    match_record =            0;
    Int4    result_gip =            0;
    Int2    append_status =            0;
    Int4    max_gi =                0;

    Int4    loop =                    0;
    Int4    num_pmids =                0;

    
    unique_gip = malloc(sizeof(Int4) * max_numr);
    if (unique_gip == NULL)
    {
        ErrPostEx(SEV_ERROR,0,0, "CountResultsByGI: Failed to allocate sufficient memory");
        return 1;
    }


    /*diagnostic line*/
    printf("PreBIND searcher is building the COuntREsults database(CountResultsByGI)\n");
    fprintf(results_file_ptr, "PreBIND searcher is building the COuntREsults database(CountResultsByGI)\n");


    /*zero the unique gi array*/
    for (index = 0; index < max_numr; ++index)
    {
        unique_gip[index] = 0;
    }

    /*cycle through the COunt REsults db*/
    for (core_record = d4top(pd4_CORE); core_record == 0; core_record = d4skip(pd4_CORE, 1L))
    {
        /*get the current pmid*/
        cur_pmid = f4int(pf4_CORE_PMID);

        /*find all occurences of this pmid in the RESULT database and make a list of unique gi's that returned it*/
        d4tagSelect(pd4_RESULT, pt4_RESULT_PMID);
        match_record = d4seekDouble(pd4_RESULT, cur_pmid);
        while (match_record == 0)
        {
            result_gip = f4int(pf4_RESULT_GIP); /*get the gi that returned the current pmid*/
            index = 0;
            while (index < max_numr && result_gip != unique_gip[index])
            {
                if (unique_gip[index] == 0) 
                {
                    /*the gip has not occurred in the list so add it*/
                    unique_gip[index] = result_gip;
                    break;
                }
                ++index;
            }

            match_record = d4seekNextDouble(pd4_RESULT, cur_pmid);

        }

        /*add the unique gi list to the REsults by GI db*/
        index = 0;
        while (index < max_numr && unique_gip[index] != 0)
        {
            d4appendStart(pd4_REGI, 0);
            d4blank(pd4_REGI);

            f4assignLong (pf4_REGI_PMID, (long) cur_pmid);
            f4assignLong (pf4_REGI_MUID, 0);
            f4assignLong (pf4_REGI_GIP, (long) unique_gip[index]);

            append_status = d4append(pd4_REGI);
            
            switch (append_status)
            {
            case r4success:
                break; /*ok result appended*/
            case r4unique:
                /*the pmid-gip pair already exists in the database- old info is not overwritten*/
                break; 
            default:
                ErrPostEx(SEV_ERROR,0,0, "CountResultsByGI failed to append pmid = %ld. append_status = %d", (long) cur_pmid, append_status);
                return 2;
            }

            ++index;
        }

        /*record the number of gi's that returned the pmid in the current REcord COunt database*/
        f4assignLong (pf4_CORE_NUMG, index);
        if (index > max_gi) max_gi = index; /*keep track of the largest number of gi's that return a single search*/

        /*rezero the unique gi array*/
        for (index = 0; index < max_numr; ++index)
        {
            if (unique_gip[index] == 0) break;
            unique_gip[index] = 0;
        }

    }/*next reco record*/

    fprintf(results_file_ptr, "The maximum number of gi's returning a single paper is = %ld\n",(long) max_gi);

    /*analyze the records: count number of records in each class of numg*/
    d4tagSelect(pd4_CORE, pt4_CORE_NUMG);
    for (loop = max_gi; loop >0; --loop)
    {
        match_record = d4seekDouble(pd4_CORE, loop);
        while (match_record == 0)
        {
            ++num_pmids;
            match_record = d4seekNextDouble(pd4_CORE, loop);
        }
        fprintf(results_file_ptr, "%4ld gi's calling one paper occured in %5ld case(s)\n",loop, num_pmids);
        num_pmids = 0;
    }
    
    fprintf(results_file_ptr, "\n-pmid's resulting from searches are listed below \nstarting with those that were returned by two, three, four etc. gi's \n-pmid's returned by only one gi are listed last\n\n");
    /*print out pmid's to file- starting with those that were returned by 2 or more gi's*/
    for (loop = 2; loop <= max_gi; ++loop)
    {
        match_record = d4seekDouble(pd4_CORE, loop);
        while (match_record == 0)
        {
            cur_pmid = f4int(pf4_CORE_PMID);
            fprintf(results_file_ptr, "%ld\n", (long) cur_pmid);
            match_record = d4seekNextDouble(pd4_CORE, loop);
        }
    }

    /*print out pmid's to file that were returned by only 1 gi*/
    match_record = d4seekDouble(pd4_CORE, 1);
    while (match_record == 0)
    {
        cur_pmid = f4int(pf4_CORE_PMID);
        fprintf(results_file_ptr, "%ld\n", (long) cur_pmid);
        match_record = d4seekNextDouble(pd4_CORE, 1);
    }
    
    return 0;
}



/**
*****************************************************
*                                                     
* CountResults:        cycle through the RESULT db and    
*                     count the number of times that    
*                     each PMID was found (NUMN) -    
*                     record the results in the        
*                     COunt REsults db                

* @param            none                            
* @return                                            
*     Int2    0: success                                
********************************************************/
Int2 CountResults(void)
{    
    Int2    reco_record =        0;
    Int2    result_record =        0;
    Int2    match_record =        0;
    Int4    cur_pmid =            0;    /*PubMed ID of the current record in the results database*/
    Int4    cur_muid =            0;    /*Medline Unique ID of the current record in the results database*/
    Int2    cur_numn =            0;    /*NUMber of Name searches that returned the pmid in the current COuntREsults db record*/
    Boolean do_append =            TRUE;
    Int2    append_status =        0;
    Int4    max_numn =            0;    /*largest cur_numn found*/
    Char    daytime[100];
    
    /*diagnostic line*/
    DayTimeStr (daytime, TRUE, TRUE);
    printf("PreBIND searcher is building the COuntREsults database. Counting pmid's. The time is: %s.\n", daytime);
    fprintf(results_file_ptr, "PreBIND searcher is building the COuntREsults database. Counting pmid's. The time is %s: .\n", daytime);
    fflush(results_file_ptr);

    /*cycle through the REsults COunt db and zero the NUMN and NUMG fields*/
    for (reco_record = d4top(pd4_CORE); reco_record == 0; reco_record = d4skip(pd4_CORE, 1L))
    {
        f4assignInt(pf4_CORE_NUMN, 0);
        f4assignInt(pf4_CORE_NUMG, 0);
    }


    /*cycle through the results database and add any pmid numbers that are not already present*/
    for (result_record = d4top(pd4_RESULT); result_record == 0; result_record = d4skip(pd4_RESULT, 1L))
    {
        /*get the current record info*/
        cur_pmid = f4int(pf4_RESULT_PMID);
        cur_muid = 0;
        do_append = TRUE;

        /*first make certain that the pmid has not already been entered in the CORE db*/
        d4tagSelect(pd4_CORE, pt4_CORE_PMID);
        match_record = d4seekDouble(pd4_CORE, cur_pmid);
        if (match_record == 0)
        {
            cur_numn = f4int(pf4_CORE_NUMN);
            ++cur_numn;
            if (cur_numn > max_numn) max_numn = cur_numn;
            f4assignInt(pf4_CORE_NUMN, cur_numn);
            do_append = FALSE;
        }


        /* append the record to the COunt REsult datafile if it was not found*/
        if (do_append == TRUE)
        {
            
            d4appendStart(pd4_CORE, 0);
            d4blank(pd4_CORE);
            
            f4assignLong (pf4_CORE_PMID, (long) cur_pmid);
            f4assignLong (pf4_CORE_MUID, (long) cur_muid);
            f4assignInt  (pf4_CORE_NUMN, 1);
            f4assignInt  (pf4_CORE_NUMG, 0); /*this value is calculated later by CountResultsByGI*/
            
            append_status = d4append(pd4_CORE);
            
            switch (append_status)
            {
            case r4success:
                break; /*ok result appended*/
            case r4unique:
                /*the pmid already exists in the database- old info is not overwritten*/
                break; 
            default:
                ErrPostEx(SEV_ERROR,0,0, "AddResultsToCORE failed to append pmid = %ld. append_status = %d", (long) cur_pmid, append_status);
                return 2;
            }

        }/*if do_append*/
            
    }

    DayTimeStr (daytime, TRUE, TRUE);
    printf("PreBIND searcher is building the COuntREsults database. Counting gi's returning each pmid. The time is: %s.\n", daytime);
    fprintf(results_file_ptr, "PreBIND searcher is building the COuntREsults database. Counting gi's returning each pmid. The time is %s: .\n", daytime);
    fflush(results_file_ptr);

    /*now count the number of unique gi's that return each pmid; i.e., fill in the NUMG field of the CORE database*/
    if (CountResultsByGI(max_numn) != 0)
    {
        ErrPostEx(SEV_ERROR, 0, 0, "CountResults: CountResultsByGI failed.");
        return 1;
    }

    DayTimeStr (daytime, TRUE, TRUE);
    printf("PreBIND searcher is finished building the COuntREsults database. The time is: %s.\n", daytime);
    fprintf(results_file_ptr, "PreBIND searcher is finished building the COuntREsults database. The time is %s: .\n", daytime);
    fflush(results_file_ptr);

    return 0;
}


/**
*****************************************************
*                                                     
* ImportPointScores:    this function imports            
*     POtential INTeraction scores from a text file    
*     (specified by filename) into the POINT database.
*     Scores are imported into either piscore (protein
*     -protein interaction score) or piscore2 (protein
*     -DNA interaction score)                            
*     -only scores of zero (not previously scored) are
*     overwritten    unless the -o optionis TRUE            
*     -scores can only be added to pmid numbers that    
*     are present    in the database already                
* @param                                            
*     Int4            score_list (1 or 2)                
* @return                                            
*     0    ok                                            
*     >1    less ok                                        
********************************************************/
Int2 ImportPointScores(Int4 score_list)
{  
    /*declarations and initializations*/
    FILE     *pFile =            NULL;
    Char     path_to_point_scores[PATH_MAX];
    Char     line_buffer[MAXLINE];
    CharPtr  cur_line_ptr =        NULL;
    CharPtr  pmid_ptr =            NULL;
    CharPtr  score_ptr =        NULL;
    CharPtr  gia_ptr =            NULL;
    CharPtr  gib_ptr =            NULL;
    Boolean  record_found =     FALSE;
    Int4     pmid =                0;
    Int4     score =            0; /*score to be added to the database*/
    Int4     gia =                0;
    Int4     gib =                0;
    Int4     cur_gia =            0;
    Int4     cur_gib =            0;
    Int4     cur_score =        0; /*score that is already present in the POINT database*/
    Int2     point_record =        0;
    /*counters*/
    Int4    a =                    0;
    Int4    b =                    0;
    Int4    c =                    0;
    Int4    d =                    0;
    Int4    e =                    0;
    Int4    f =                    0;

    AssignTagsPOINT();
    /*find path to the point scores file (see NCBI toolkit corelib 
    and file "prebind.ini" in windows directory)*/
    path_to_point_scores[0] = '\0';
    GetAppParam("prebind", "searcher_input_files", "point_scores", NULL, path_to_point_scores, (size_t)PATH_MAX * (sizeof(Char)));
    

    /*open the point scores file*/
    if ((pFile = FileOpen(path_to_point_scores, "r")) == NULL)
    {
        printf("ImportPointScores: File of scores failed to open.  Check the path to point_scores (%s) under [searcher_input_files] to ensure that it correctly points to a text file of POtential INTeraction scores.\n", path_to_point_scores);
        ErrPostEx(SEV_ERROR, 0, 0, "ImportPointScores: File of scores failed to open.  Check the path to point_scores (%s) under [searcher_input_files] to ensure that it correctly points to a text file of POtential INTeraction scores.\n", path_to_point_scores);
        return 1;
    }


    /*diagnostic line*/
    printf("PreBIND searcher has started importing POtential INTeraction scores\n");
    if (score_list == 1) /*protein::protein interaction score*/
    {
        printf("Score will be imported from file %s to the piscore field (protein::protein interaction score)\n", path_to_point_scores);
        fprintf(results_file_ptr, "Score will be imported from file %s to the piscore field (protein::protein interaction score)\n", path_to_point_scores);
    }
    if (score_list == 2) /*protein::DNA interaction score*/
    {
        printf("Score will be imported from file %s to the piscore field (protein::DNA interaction score)\n", path_to_point_scores);
        fprintf(results_file_ptr, "Score will be imported from file %s to the piscore field (protein::DNA interaction score)\n", path_to_point_scores);
    }
    fprintf(results_file_ptr, "PreBIND searcher has started importing POtential INTeraction scores.  File = %s and score_list = %ld\n", path_to_point_scores, (long) score_list);


    cur_line_ptr = fgets(line_buffer, (size_t)MAXLINE, pFile);
    while (cur_line_ptr)
    {
        /*divide the line by the "|" token*/
        pmid_ptr =  strtok(cur_line_ptr, "\t\n");/*line is split by either tabs or returns*/
        score_ptr = strtok(NULL, "\t\n");
        gia_ptr = strtok(NULL, "\t\n");
        gib_ptr = strtok(NULL, "\t\n");


        pmid = (Int4)atoi(pmid_ptr);
        score = (Int4)atoi(score_ptr);
        gia = (Int4)atoi(gia_ptr);
        gib = (Int4)atoi(gib_ptr);
        /*
        printf("pmid= %ld \tscore= %ld \tgia= %ld \tgib= %ld",(long) pmid, (long) score, (long) gia, (long) gib);
        fprintf(results_file_ptr, "pmid= %ld \tscore= %ld \tgia= %ld \tgib= %ld",(long) pmid, (long) score, (long) gia, (long) gib);
        */
        ++a;


        /*given the pmid and score, look for the pmid in the POINT database*/
        d4tagSelect(pd4_POINT, pt4_POINT_PMID); /*select the pmid tag*/

        point_record = d4seekDouble(pd4_POINT, pmid);
        if (point_record != 0)/*r4success*/
        {
            ErrPostEx(SEV_ERROR, 0, 0, "ImportPointScores: pmid not found pmid= %ld error = %d\n", (long) pmid, point_record);
            printf("pmid not found pmid= %ld error = %d\n", (long) pmid, point_record);
            fprintf(results_file_ptr, "pmid not found pmid= %ld error = %d\n", (long) pmid, point_record);

        }
        
        while (point_record == 0)
        {
            cur_gia = f4long(pf4_POINT_GIA);
            cur_gib = f4long(pf4_POINT_GIB);
            if ((gia == cur_gia && gib == cur_gib) || (gia == cur_gib && gib == cur_gia))
            {
                record_found = TRUE;
                ++b;
                cur_score = f4long(pf4_POINT_PISCORE);
                if (score_list == 1) /*protein::protein interaction score*/
                {
                    cur_score = f4long(pf4_POINT_PISCORE);
                }
                if (score_list == 2) /*protein::DNA interaction score*/
                {
                    cur_score = f4long(pf4_POINT_PISCORE2);
                }
                if (cur_score == 0) ++c; else ++d;

                if (cur_score == 0 || myargs[13].intvalue == 1) /*no POINT scores will be overwritten unless -o option is TRUE*/
                {
                    /*add the score to the POINT database*/
                    if (score_list == 1) /*protein::protein interaction score*/
                    {
                        f4assignLong(pf4_POINT_PISCORE, score);
                        ++e;
                        /*
                        score = f4long(pf4_POINT_PISCORE);
                        printf("new score = %ld\n", (long) score);
                        fprintf(results_file_ptr, "new score = %ld\n", (long) score);
                        */
                    }
                    if (score_list == 2) /*protein::DNA interaction score*/
                    {
                        f4assignLong(pf4_POINT_PISCORE2, score);
                        /*
                        score = f4long(pf4_POINT_PISCORE);
                        printf("new score = %ld\n", (long) score);
                        fprintf(results_file_ptr, "new score = %ld\n", (long) score);
                        */
                    }    
                }
            }
            
            point_record = d4seekNextDouble(pd4_POINT, pmid);
        }
        if (record_found == FALSE)
        {
            printf("Corresponding POINT record not found.  Could not import pmid= %ld \tscore= %ld \tgia= %ld \tgib= %ld\n",(long) pmid, (long) score, (long) gia, (long) gib);
            fprintf(results_file_ptr, "Corresponding POINT record not found.  Could not import pmid= %ld \tscore= %ld \tgia= %ld \tgib= %ld\n",(long) pmid, (long) score, (long) gia, (long) gib);
            ++f;
        }

        /*get the next line and reset values*/
        cur_line_ptr = fgets(line_buffer, (size_t)MAXLINE, pFile);
        fflush(stdout);
        fflush(results_file_ptr);

        pmid = 0;
        score = 0;
        gia = 0;
        gib = 0;
        record_found = FALSE;
    }

    /*close the file*/
    FileClose(pFile);

    /*print report*/
    printf("Number of imported scores read from file: %ld\n", (long) a);
    printf("Number of corresponding records found in POINT datafile: %ld\n", (long) b);
    printf("Number of POINT records with no score: %ld\n", (long) c);    
    printf("Number of POINT records already having scores: %ld\n", (long) d);
    printf("Number of scores written to POINT datafile: %ld\n", (long) e);
    printf("Number of corresponding records not found in POINT datafile: %ld\n", (long) f);

    fprintf(results_file_ptr, "Number of imported scores read from file: %ld\n", (long) a);
    fprintf(results_file_ptr, "Number of corresponding records found in POINT datafile: %ld\n", (long) b);
    fprintf(results_file_ptr, "Number of POINT records with no score: %ld\n", (long) c);    
    fprintf(results_file_ptr, "Number of POINT records already having scores: %ld\n", (long) d);
    fprintf(results_file_ptr, "Number of scores written to POINT datafile: %ld\n", (long) e);
    fprintf(results_file_ptr, "Number of corresponding records not found in POINT datafile: %ld\n", (long) f);
    
    if(ReindexPOINT()==FALSE){
        printf("reindex  POINT table error !\n");
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
* ImportPointScores:    this function imports            
*     POtential INTeraction scores from a text file    
*     (specified by filename) into the POINT database.
*     Scores are imported into either piscore (protein
*     -protein interaction score) or piscore2 (protein
*     -DNA interaction score)                            
*     -only scores of zero (not previously scored) are
*     overwritten    unless the -o optionis TRUE            
*     -scores can only be added to pmid numbers that    
*     are present    in the database already                
* @param   filename tab-deliminated POINT table            
* @return                                            
*     0    ok                                            
*     >1    less ok                                        
********************************************************/

Int2 ImportPoint(CharPtr filename)
{  
    /*declarations and initializations*/
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
  

    /*open the point scores file*/
    if ((pFile = FileOpen(filename, "r")) == NULL)
    {
        printf("ImportPoint: POINT file failed to open.  Check the path to the file: %s.\n", filename);
        ErrPostEx(SEV_ERROR, 0, 0, "ImportPoint: POINT file failed to open.  Check the path to the file: %s.\n", filename);
        return 1;
    }
    memset(line_buffer, 0, (size_t)MAXLINE);
    cur_line_ptr = fgets(line_buffer, (size_t)MAXLINE, pFile);
    while (cur_line_ptr)
    {
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
        
        memset(line_buffer, 0, (size_t)MAXLINE);

        /*get the next line and reset values*/
        cur_line_ptr = fgets(line_buffer, (size_t)MAXLINE, pFile);
        fflush(stdout);
        fflush(results_file_ptr);

        pmid = 0;
        nama=NULL;
        namb=NULL;
        gia = 0;
        gib = 0;
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
* ImportSvmScores:    this function imports svm scores
*                     for interaction into the ISVM    
*                     database                        
*                     -only scores of zero (not        
*                     previously scored) are            
*                     overwritten    unless 'overwrite    
*                     is set to TRUE                    
*                     -scores can only be added to    
*                     pmid numbers that are present    
*                     in the database already            
*                                        
* @param   filename      CharPtr                  
* @param   score_list      Int4                
* @param    overwrite     Boolean                  
* @return                                            
*     0    ok                                            
*    >0    less ok                                        
********************************************************/
Int2 ImportSvmScores(CharPtr filename, Int4 score_list, Boolean overwrite)
{  
    /*declarations and initializations*/
    FILE     *pFile =            NULL;
    Char     line_buffer[100];
    CharPtr  cur_line_ptr =        NULL;
    CharPtr  pmid_ptr =            NULL;
    CharPtr  score_ptr =        NULL;
    Int4     pmid =                0;
    FloatHi     score =            0.0; /*score to be added to the ISVM database*/
    FloatHi     cur_score =        0.0; /*score that is already present in the ISVM database*/
    FloatHi     cur_score2 =        0.0; /*score that is already present in the ISVM database*/
    Int4     itruth =            0;
    Int2     isvm_record =        0;
    Char     daytime[100];
    Int4    num_scores =        0;
    Int4    num_scores_imported = 0;
    Int4    num_pmids_not_found = 0;
    Int4    num_scores_present = 0;
    Int4    num_scores_overwritten = 0;

    AssignTagsISVM();
    /*open the svm scores file*/
    pFile = FileOpen(filename, "r");
    if (pFile == NULL || (score_list != 1 && score_list != 2 && score_list !=3))
    {
        ErrPostEx(SEV_ERROR, 0, 0, "ImportSvmScores: Failed to open file: %s.  Score list is %ld", filename, (long) score_list);
        return 1;
    }

    /*diagnostic line*/
    DayTimeStr(daytime, TRUE, TRUE);
    printf("PreBIND searcher has started importing svm interaction scores for score set number %ld. The time is: %s.\n", (long) score_list, daytime);
    fprintf(results_file_ptr, "PreBIND searcher has started importing svm interaction scoresfor score set number %ld. The time is: %s.\n", (long) score_list, daytime);


    cur_line_ptr = fgets(line_buffer, (size_t)100, pFile);
    while (cur_line_ptr)
    {
        /*divide the line by the "|" token*/
        pmid_ptr =  strtok(cur_line_ptr, "\t\n");/*line is split by either tabs  or returns*/
        score_ptr = strtok(NULL, "\t\n");

        pmid = (Int4)atoi(pmid_ptr);
        score = (FloatHi)atof(score_ptr);
        ++num_scores;
        /*printf("pmid= %ld\tscore= %f\n",(long) pmid, score);*/
        
        /*given the pmid and score, look for the pmid in the ISVM database*/
        /*select the pmid tag*/
        d4tagSelect(pd4_ISVM, pt4_ISVM_PMID);

        isvm_record = d4seekDouble(pd4_ISVM, pmid);
        if (isvm_record == 0)/*r4success*/
        {
            if (score_list == 1) 
            {
                cur_score =  f4double(pf4_ISVM_ISCORE);
            }
            if (score_list == 2) 
            {
                cur_score =  f4double(pf4_ISVM_ISCORE2);
            }
            if (score_list == 3) 
            {
                cur_score =  f4long(pf4_ISVM_ITRUTH);
            }
            
            if (cur_score != 0)
            {
                ++num_scores_present;
                if (overwrite == TRUE)
                {
                    ++num_scores_overwritten;
                }
            }

            if (cur_score == 0 || overwrite == TRUE)/*so no ISVM scores will be overwritten unless overwrite is TRUE*/
            {
                /*add the score to the ISVM database*/
                if (score_list == 1) 
                {
                    f4assignDouble(pf4_ISVM_ISCORE, score);
                }
                if (score_list == 2) 
                {
                    f4assignDouble(pf4_ISVM_ISCORE2, score);
                }    
                if (score_list == 3) 
                {
                    f4assignLong(pf4_ISVM_ITRUTH, (long)score);
                }    

                /*read and print the new score*/
                /*cur_score = f4double(pf4_ISVM_ISCORE);
                cur_score2 = f4double(pf4_ISVM_ISCORE2);
                itruth = f4long(pf4_ISVM_ITRUTH);
                printf("\tnew score = %lf\tnew score2 = %lf\titruth = %ld\n", cur_score, cur_score2, (long) itruth);
                */
                ++num_scores_imported;
              }
        }
        else if (isvm_record != 0)
        {
            ErrPostEx(SEV_ERROR, 0, 0, "ImportSvmScores: pmid not found pmid= %ld error = %d\n", (long) pmid, isvm_record);
            fprintf(results_file_ptr, "pmid not found pmid= %ld error = %d\n", (long) pmid, isvm_record);
            ++num_pmids_not_found;
        }


        /*get the next line*/
        cur_line_ptr = fgets(line_buffer, (size_t)100, pFile);
    }

    /*close the file*/
    FileClose(pFile);

    DayTimeStr(daytime, TRUE, TRUE);
    printf("PreBIND searcher has finished importing svm interaction scores for score set number %ld. The time is: %s.\n", (long) score_list, daytime);
    fprintf(results_file_ptr, "PreBIND searcher has finished importing svm interaction scoresfor score set number %ld. The time is: %s.\n", (long) score_list, daytime);

    printf("Scores in import list: %ld\n Scores imported: %ld\n Number of pmids not found: %ld\n Number pmids already scored: %ld\n Number of pmid scores overwritten: %ld\n", (long) num_scores, (long) num_scores_imported, (long) num_pmids_not_found, (long) num_scores_present, (long) num_scores_overwritten);
    fprintf(results_file_ptr, "Scores in import list: %ld\n Scores imported: %ld\n Number of pmids not found: %ld\n Number pmids already scored: %ld\n Number of pmid scores overwritten %ld\n", (long) num_scores, (long) num_scores_imported, (long) num_pmids_not_found, (long) num_scores_present, (long) num_scores_overwritten);

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
* ImportSvmScores:    this function imports RESULT table
*                  from text files generated by mymed seaches
*                                        
* @param   filename      CharPtr                                   
* @return                                            
*     0    ok                                            
*    >0    less ok                                        
********************************************************/
Int2 ImportResult(CharPtr filename)
{  
    /*declarations and initializations*/
    FILE     *pFile = NULL;
    Char     line_buffer[100];
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

    /*open the svm scores file*/
    pFile = FileOpen(filename, "r");
    if (pFile == NULL)
    {
        ErrPostEx(SEV_ERROR, 0, 0, "ImportResult: Failed to open file: %s.  ", filename);
        return 1;
    }

    DayTimeStr(daytime, TRUE, TRUE);
    printf("PreBIND searcher has started importing RESULT table. [%s].\n",daytime);
    fprintf(results_file_ptr, "PreBIND searcher has started importing started importing RESULT table. [%s].\n",  daytime);

    cur_line_ptr = fgets(line_buffer, (size_t)100, pFile);
    while (cur_line_ptr)
    {
        /*line is split by either tabs*/
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
        switch(append_status){
            case r4success: 
                results_appended++;
                break;
            case r4unique:
                result_already_found++;
                break;
            default:
                ErrPostEx(SEV_ERROR,0,0, "ImportResult failed to append record. append_status = %d", append_status);
                append_error++;
        }
        
        /*get the next line*/
        cur_line_ptr = fgets(line_buffer, (size_t)100, pFile);
    }
    printf("appended= %ld; \tpresent= %ld; error= %ld\n\n", (long) results_appended, (long) result_already_found, (long)append_error);
    fprintf(results_file_ptr, "appended= %ld; \tpresent= %ld; error= %ld\n", (long) results_appended, (long) result_already_found, (long)append_error);

    /*close the file*/
    FileClose(pFile);

    DayTimeStr(daytime, TRUE, TRUE);
    printf("PreBIND searcher has finished importing RESULT table. [%s]\n",  daytime);
    fprintf(results_file_ptr, "PreBIND searcher has finished importing RESULT table. [%s]\n", daytime);
    
    if(ReindexRESULT()==FALSE){
        printf("reindex RESULT table error !\n");
        return 1;
    }
    else{
        printf("reindex RESULT table successfully \n");
    }
    
    return 0;
}


/**
*****************************************************
* IsInteractionInPOINT: determine if a gi pair is in     
*                     the POINT database in either    
*                     orientation    for a given paper    
*                     (pmid)                            
* @param    gia gi of protein A            
* @param    gib gi of protein B            
* @param    pmid: pbmed id
* @return                                            
*             Boolean    TRUE: yes its present            
********************************************************/
Boolean IsInteractionInPOINT(Int4 gia, Int4 gib, Int4 pmid)
{
    Boolean interaction_present =    FALSE;
    Int2    match_record =            0;

    
    AssignTagsPOINT();
    d4tagSelect(pd4_POINT, pt4_POINT_PMID);
    match_record = d4seekDouble(pd4_POINT, pmid);
    while (match_record == 0)
    {
        /*look for the gi pair in one oritentation*/
        if (f4int(pf4_POINT_GIA) == gia  && f4int(pf4_POINT_GIB) == gib)
        {
            interaction_present = TRUE;
            break;
        }
        /*then in the other*/
        if (f4int(pf4_POINT_GIB) == gia  && f4int(pf4_POINT_GIA) == gib)
        {
            interaction_present = TRUE;
            break;
        }
        /*then move on to the next occurrence of the pmid in the POINT database*/
        match_record = d4seekNextDouble(pd4_POINT, pmid);
    }
    
    return interaction_present;
}


/**
*****************************************************
*                                                     
* GetNameListForGIinPMID                                
*                                                     
* @param    Int4 gi:    gi of protein                
* @param    Int4 pmid:    pmid of paper                
* @return                                            
*     CharPtr    slash-delimited list of names that        
*             describe a given protein (gi) in a        
*             given paper (pmid)                        
*             a NULL value is an error                
********************************************************/
CharPtr GetNameListForGIinPMID(Int4 gi, Int4 pmid)
{
    CharPtr        name_list =            NULL;
    Int2        match_record =        0;
    CharPtr        cur_name =            NULL;
    CharPtr        temp_name_list =    NULL; 

    AssignTagsRESULT();
    /*allocate memory to name_list and assign an initial value of "n/a"*/
    name_list = (CharPtr) malloc(sizeof(Char) * MAX_NAME_LIST_LENGTH);
    if (name_list != NULL)
    {
        StrCpy(name_list, "n/a"); 
    }
    else
    {
        ErrPostEx(SEV_ERROR,0,0, "GetNameListForGIinPMID: Failed to allocate sufficient memory");
        return NULL;
    }
    
    /*allocate memory to cur_name*/
    cur_name = (CharPtr) malloc(sizeof(Char) * MAX_NAME_LIST_LENGTH);
    if (cur_name == NULL)
    {
        ErrPostEx(SEV_ERROR,0,0, "GetNameListForGIinPMID: Failed to allocate sufficient memory");
        return NULL;
    }
    
    /*find each occurrence of a given protein (gi) in a paper (pmid) in the RESULT database- concatentate all associated protein names*/
    d4tagSelect(pd4_RESULT, pt4_RESULT_PMID);
    match_record = d4seekDouble(pd4_RESULT, pmid);
    while (match_record == 0)
    {
        if (f4int(pf4_RESULT_GIP) == gi)
        {
            StrCpy(cur_name, f4str(pf4_RESULT_NAME));
            
            if(TrimSpaces(cur_name) != 0)
                ErrPostEx(SEV_ERROR,0,0, "GetNameListForGIinPMID: TrimSpaces failed, possibly passed a null string");
            
            if (StrNCmp(name_list, "n/a", 3) == 0)
            {
                StrCpy(name_list, cur_name);
            }
            else if ((StrLen(name_list) + StrLen(cur_name) + 1) < MAX_NAME_LIST_LENGTH)
            {
                StrCat(name_list, "/");
                StrCat(name_list, cur_name);
            }
            else 
            {    
                /*save the name_list so far in a temporay space*/
                temp_name_list = StrSave(name_list);
                if (name_list) name_list = MemFree(name_list);
                /*reallocate more space to the name_list space*/        
                name_list = (CharPtr) malloc(sizeof(Char) * (StrLen(name_list) + StrLen(cur_name) +1));
                if (name_list != NULL)
                {
                    StrCpy(name_list, temp_name_list);
                    if (temp_name_list) temp_name_list = MemFree(temp_name_list);
                    StrCat(name_list, "/");
                    StrCat(name_list, cur_name);
                }
                else
                {
                    ErrPostEx(SEV_ERROR,0,0, "GetNameListForGIinPMID: Failed to allocate sufficient memory");
                    return NULL;                    
                }
            }
            
        }

        match_record = d4seekNextDouble(pd4_RESULT, pmid);
    }
    if (StrLen(name_list) > MAX_NAME_LIST_LENGTH)
    {
        ErrPostEx(SEV_INFO,0,0, "GetNameListForGIinPMID: %ld characters in name_list.  This is > MAX_NAME_LIST_LENGTH for gi =%ld \n", (long) StrLen(name_list), (long) gi);
    }



    /*MemFree*/
    if (cur_name) cur_name = MemFree(cur_name);

    if (StrNCmp(name_list, "n/a", 3) == 0)
    {
        ErrPostEx(SEV_INFO,0,0, "GetNameListForGIinPMID: No names were found for gi = %ld in pmid = %ld.", (long) gi, (long) pmid);
        name_list = NULL;
    }
    return name_list;
}


/**
*****************************************************
*                                                   
* BuildPOINT: cycle through the COunt REsult db and 
*             for each NUMG that is greater than one
*             assemble a list of potential pairwise 
*             protein interactions and record       
*             these in the POtential INTeraction db 
* @param            none                       
* @return                                          
*     Int2    0: success                            
********************************************************/
Int2 BuildPOINT(void)
{    
    Int4    number_records =        0;
    Int4    counter =                0;
    Int4    counter_multiplier =    1;
    Int4    cur_numg =                0;
    Int2    core_record =            0;
    Int4    cur_pmid =                0;
    Int4    cur_gi =                0;
    Int4    cur_muid =                0;
    Int2    match_record =            0;
    ValNodePtr pvngi_head =            NULL;
    ValNodePtr pvngi =                NULL;
    ValNodePtr pvngi_a =            NULL;
    ValNodePtr pvngi_b =            NULL;
    Int4    gia =                    0;
    Int4    gib =                    0;
    Int4    taxid_a    =                0;
    Int4    taxid_b =                0;
    CharPtr    name_list_A =            NULL;
    CharPtr    name_list_B =            NULL;
    Int2    append_status =            0;
    Char daytime[100];
    
    /*diagnostic line*/
    DayTimeStr(daytime, TRUE, TRUE);
    printf("PreBIND searcher is building the POtential INTeractions database. The time is: %s.\n", daytime);
    fprintf(results_file_ptr, "PreBIND searcher is building the POtential INTeractions database. The time is: %s.\n", daytime);
    number_records = d4recCount(pd4_CORE);
    printf("Total records in CORE database to process: %ld\n", (long) number_records);
    fprintf(results_file_ptr, "Total records in CORE database to process: %ld\n", (long) number_records);
    fflush(results_file_ptr);

    /*cycle through the COunt REsults db and do each time numg is greater than 1*/
    for (core_record = d4top(pd4_CORE); core_record == 0; core_record = d4skip(pd4_CORE, 1L))
    {
        /*let the user know how many records are left to process*/
        ++counter;

        if (counter == 1000)
        {
            counter = counter * counter_multiplier;
            printf("CORE records analyzed: %ld\n", (long) counter);
            counter = 0;
            ++counter_multiplier;
        }

        /*get the current numg- number of gi's that return the pmid of the current CORE record*/
        cur_numg = f4int(pf4_CORE_NUMG);

        if (cur_numg > 1)
        {
            /*allocate a valnode list header for a list of gi's*/
            pvngi_head = ValNodeNew(NULL);

            /*get the current pmid*/
            cur_pmid = f4int(pf4_CORE_PMID);

            
            /*fill a valnode linked list with the gi's that found cur_pmid (from the REsults by GI database)*/
            d4tagSelect(pd4_REGI, pt4_REGI_PMID);
            match_record = d4seekDouble(pd4_REGI, cur_pmid);
            while (match_record == 0)
            {
                cur_gi = f4int(pf4_REGI_GIP);
                /*check that this still works*/
                pvngi = Slri_ValNodeAddInt(&pvngi_head, 1, cur_gi);
                match_record = d4seekNextDouble(pd4_REGI, cur_pmid);
            }
            pvngi = NULL; /*so that you can properly free the linked list of ValNodes when you are done with them*/ 

            /*print out the gi values*/
            /*pvngi = pvngi_head->next;
            while (pvngi != NULL)
            {
                printf("%ld\n", (long) pvngi->data.intvalue);
                pvngi = pvngi->next;
            }*/
            
            /*compose all possible pairwise combinations of these gi's and write them to the POINT database if they do not already exist there*/
            pvngi_a = pvngi_head;
            while (pvngi_a != NULL)
            {
                gia = pvngi_a->data.intvalue;
                pvngi_b = pvngi_a->next;
                while (pvngi_b != NULL && pvngi_b->data.intvalue != 0)
                {
                    gib = pvngi_b->data.intvalue;

                    /*only make potential interactions between proteins in the same taxon*/
                    taxid_a = GetTaxidbyGI(gia);
                    taxid_b = GetTaxidbyGI(gib);

                    if (taxid_a == taxid_b)
                    {
                        
                        /*determine if the potential interaction has already been entered into the database*/
                        if (IsInteractionInPOINT(gia, gib, cur_pmid) == FALSE)
                        {
                            /*compose the name_lists for each of the interacting proteins*/
                            name_list_A = GetNameListForGIinPMID(gia, cur_pmid);
                            if (name_list_A == NULL)
                            {
                                ErrPostEx(SEV_ERROR,0,0, "GetNameListForGIinPMID Failed for name_list_A. POINT record will not be made for pmid = %ld, gia = %ld and gib = %ld", (long) cur_pmid, (long) gia, (long) gib);
                                break;
                            }
                            name_list_B = GetNameListForGIinPMID(gib, cur_pmid);
                            if (name_list_B == NULL)
                            {
                                ErrPostEx(SEV_ERROR,0,0, "GetNameListForGIinPMID Failed for name_list_B. POINT record will not be made for pmid = %ld, gia = %ld and gib = %ld", (long) cur_pmid, (long) gia, (long) gib);
                                break;
                            }

                            /*assign a dummy muid*/
                            cur_muid = 0;
                            
                            /*write the interaction to the database*/
                            d4appendStart(pd4_POINT, 0);
                            d4blank(pd4_POINT);
                            
                            f4assignLong (pf4_POINT_PMID, (long) cur_pmid);
                            f4assignLong (pf4_POINT_MUID, (long) cur_muid);
                            
                            f4assignLong (pf4_POINT_GIA,  (long) gia);
                            f4assign     (pf4_POINT_NAMA, name_list_A);
                            f4assignInt  (pf4_POINT_ATYPE, 0); 
                            
                            f4assignLong (pf4_POINT_GIB,  (long) gib);
                            f4assign     (pf4_POINT_NAMB, name_list_B);
                            f4assignInt  (pf4_POINT_BTYPE, 0);
                            
                            f4assignInt     (pf4_POINT_OPNO, 0);
                            f4assignInt     (pf4_POINT_OPYES, 0);
                            f4assignInt     (pf4_POINT_PISCORE, 0);
                            f4assignInt     (pf4_POINT_PISCORE2, 0);
                            f4assignInt     (pf4_POINT_PITRUTH, 0);
                            
                            append_status = d4append(pd4_POINT);

                            /*printf("name list A %s, \tname list B %s\n", name_list_A, name_list_B);*/
                            
                            /*MemFree*/
                            if (name_list_A) name_list_A = MemFree(name_list_A);
                            if (name_list_B) name_list_B = MemFree(name_list_B);
                            
                            switch (append_status)
                            {
                            case r4success:
                                break; /*ok result appended*/
                            default:
                                ErrPostEx(SEV_ERROR,0,0, "BuildPOINT failed to append pmid = %ld. append_status = %d", (long) cur_pmid, append_status);
                                return 2;
                            }
                            
                        }/*if the interaction is not already in the database*/
                    }/*if the interaction is between two proteins of the same taxon*/

                    pvngi_b = pvngi_b->next;
                }

                pvngi_a = pvngi_a->next;
            }

            
            /*MemFree*/
            if (pvngi_head) pvngi_head = ValNodeFree(pvngi_head);            
            /*pvngi pointed to the last valnode in this list which was freed in the line above*/
            if (pvngi_a) pvngi_a = MemFree(pvngi_a);
            if (pvngi_b) pvngi_b = MemFree(pvngi_b);
            
        }/*if the number of proteins (gi's) finding this paper is > 1*/

    }/*for each entry in the COunt REsults database*/

    DayTimeStr(daytime, TRUE, TRUE);
    printf("PreBIND searcher is finished building the POtential INTeractions database. The time is: %s.\n", daytime);
    fprintf(results_file_ptr, "PreBIND searcher is finished building the POtential INTeractions database. The time is: %s.\n", daytime);
    
    return 0;
}


/**
*****************************************************
*                                                     
* CheckSearches:    cycle through the SeaRCH database to
*                 determine which searches to perform    
*                 and ensure that all searches will    
*                 not break the term list parser and    
*                 mark ok searches as "to be searched"
* @param                                            
*     Int2    do_searches        ALL_SEARCHES  0            
*                             NEW_SEARCHES  1            
*                             UPDATE_SEARCHES  2        
* @param    Int2    do_only_taxid    0- do all taxids        
*                             4932- do only yeast etc.
* @return                                            
*     Int2    0: success                                
********************************************************/
Int2 CheckSearches(Int2 do_searches, Int4 do_only_taxid, Boolean requeue_completed_searches)
{
    Int4    srch_record =               0;
    Int2    cur_status =                -3;
    Int4    searches_unqed =            0;
    CharPtr cur_name =                  NULL;
    Int4    cur_taxid =                 0;
    Char    cur_last_searched[10];
    CharPtr org_name =                  NULL;
    Char    search_date_str[15];
    CharPtr query =                     NULL;
    Int2    size_to_allocate =          0;
    Int4    searches_checked =          0;
    Int4    searches_with_errors =      0;
    DocType database=                   TYP_ML;
    Int2    beginErr=                   0;
    Int2    endErr=                     0;
    Boolean do_search =                 FALSE;
    Int4    qed_searches =              0;
    CharPtr quoted_name_ptr =           NULL;


    /*diagnostic line*/
    printf("PreBIND searcher is checking the search database\n");
    fprintf(results_file_ptr, "PreBIND searcher is checking the search database\n");

    /*cycle through each entry in the search database and
    set any queued searches to non-queued: STATUS 2 to STATUS 1*/
    for (srch_record = d4top(pd4_SRCH); srch_record == 0; srch_record = d4skip(pd4_SRCH, 1L))
    {
        cur_status = f4int(pf4_SRCH_STATUS);
        if (cur_status == 2)
        {
            f4assignInt(pf4_SRCH_STATUS, 1);
            ++searches_unqed;
        }
        if (cur_status == 3 && requeue_completed_searches == TRUE)
        {
            f4assignInt(pf4_SRCH_STATUS, 0);/*this allows completed searches to be requeued*/
        }
    }

    /*cycle through each entry in the search database and ensure the search is parseable*/
    for (srch_record = d4top(pd4_SRCH); srch_record == 0; srch_record = d4skip(pd4_SRCH, 1L))
    {
        cur_status = f4int(pf4_SRCH_STATUS);

        if (cur_status == 0)/*the current search has never been checked before*/
        {
            /*get taxon id and date that search was last performed*/
            cur_taxid =    f4int(pf4_SRCH_TAXID);
            u4ncpy(cur_last_searched, f4str(pf4_SRCH_SEARCHED), sizeof(cur_last_searched));
            
            /*get the current name*/
            cur_name = StrSave(f4str(pf4_SRCH_NAME));
            if(TrimSpaces(cur_name) != 0)
                ErrPostEx(SEV_ERROR,0,0, "CheckSearches: TrimSpaces failed, possibly passed a null string");
            
            /* place quotes around names*/
            size_to_allocate = sizeof(Char)*strlen(cur_name) + sizeof(Char)*3;
            quoted_name_ptr = (CharPtr) malloc (size_to_allocate);
            if (quoted_name_ptr != NULL)
            {
                strcpy(quoted_name_ptr, "\"");
                strcat(quoted_name_ptr, cur_name);
                strcat(quoted_name_ptr, "\"");
                strcpy(cur_name, quoted_name_ptr);
                MemFree(quoted_name_ptr);
            }

            /*get name of organism for searching*/
            if (cur_taxid == 4932)
            {
                org_name = StrSave("Saccharomyces_cerevisiae");
            }
            if (cur_taxid == 9606)
            {
                org_name = StrSave("human");
            }
            if (cur_taxid == 10090)
            {
                org_name = StrSave("mice");
            }
            /*get the date component of the search*/
            if (strncmp(cur_last_searched, "10000101", 8) == 0)
            {
                search_date_str[0] = '\0';
            }
            else
            {
                strcpy (search_date_str, " & ");
                strncat(search_date_str, cur_last_searched, 4);
                strcat (search_date_str, "[DATE]");
            }

            /*compose the search*/
            size_to_allocate = sizeof(Char)*strlen(cur_name) + sizeof(Char)*strlen(org_name) + sizeof(Char)*40;
            query = (CharPtr)malloc(size_to_allocate);
            if (query != NULL)
            {
                strcpy(query,  cur_name);
                strcat(query, "[WORD]");
                strcat(query, " & ");
                strcat(query,  org_name);
                strcat(query, "[MESH]");
                strcat(query,  search_date_str);
                ++searches_checked;
            }


            /*check the search using EntrezTLParseString (this function is a wrapper for EntrezPMTLParseString)*/
            if (! EntrezTLParseString(query, database, -1, &beginErr, &endErr))
            {
                ErrPostEx(SEV_INFO, 0, 0, "Syntax error in query= %s at position= %d to %d\n", query, beginErr, endErr);
                ++searches_with_errors;
            }    
            else
            {
                /*mark the search as approved*/
                f4assignInt(pf4_SRCH_STATUS, 1);
            }

            MemFree(query);

        }/*if (search has never been checked)*/

    }/*next search record*/

    /*cycle through each entry in the search database and decide which searches are to be queued*/
    for (srch_record = d4top(pd4_SRCH); srch_record == 0; srch_record = d4skip(pd4_SRCH, 1L))
    {
        do_search = FALSE;
        cur_status = f4int(pf4_SRCH_STATUS);
        if (cur_status == 1) /*search has been approved*/
        {
            u4ncpy(cur_last_searched, f4str(pf4_SRCH_SEARCHED), sizeof(cur_last_searched));
            cur_taxid =    f4int(pf4_SRCH_TAXID);
            
            /*determine if the search is to be queued*/
            if (strncmp(cur_last_searched, "10000101", 8) == 0)
            {
                /*this is a new search*/
                if (do_searches == ALL_SEARCHES || do_searches == NEW_SEARCHES)
                {
                    if (do_only_taxid !=0)
                    {
                        if (cur_taxid == do_only_taxid)
                        {
                            do_search = TRUE;
                        }
                        else 
                        {
                            do_search = FALSE;
                        }
                    }
                    else
                    {
                        do_search = TRUE;
                    }
                }
            }
            else
            {
                /*this is not a new search*/
                if (do_searches == ALL_SEARCHES || do_searches == UPDATE_SEARCHES)
                {
                    if (do_only_taxid !=0)
                    {
                        if (cur_taxid == do_only_taxid)
                        {
                            do_search = TRUE;
                        }
                        else
                        {
                            do_search = FALSE;
                        }
                    }
                    else
                    {
                        do_search = TRUE;
                    }
                }
            }
            if (do_search == TRUE)     
            {
                f4assignInt(pf4_SRCH_STATUS, 2);
                ++qed_searches;
            }
        }/*if search is approved*/
    }/*next search record*/
    
    /*printout results of CheckSearches to screen*/
    printf("Finished checking searches\n");
    printf("searches that were unqueued = %ld\n", (long) searches_unqed);
    printf("searches that were checked = %ld\n", (long) searches_checked);
    printf("searches found to have errors = %ld\n", (long) searches_with_errors);
    printf("type of searches to do: ");
    if (do_searches == 0) printf("UPDATES + NEWSEARCHES\n");
    if (do_searches == 1) printf("NEWSEARCHES ONLY\n");
    if (do_searches == 2) printf("UPDATE SEARCHES ONLY\n");
    printf("restrict searches to taxon id: ");
    if (do_only_taxid) printf("%ld\n", (long) do_only_taxid); else printf("NO\n");
    printf("searches to do= %ld\n\n", (long) qed_searches);

    /*printout results of CheckSearches to logfile*/
    fprintf(results_file_ptr,"Finished checking searches\n");
    fprintf(results_file_ptr,"searches that were unqueued = %ld\n", (long) searches_unqed);
    fprintf(results_file_ptr,"searches that were checked = %ld\n", (long) searches_checked);
    fprintf(results_file_ptr,"searches found to have errors = %ld\n", (long) searches_with_errors);
    fprintf(results_file_ptr,"type of searches to do: ");
    if (do_searches == 0) fprintf(results_file_ptr,"UPDATES + NEWSEARCHES\n");
    if (do_searches == 1) fprintf(results_file_ptr,"NEWSEARCHES ONLY\n");
    if (do_searches == 2) fprintf(results_file_ptr,"UPDATE SEARCHES ONLY\n");
    fprintf(results_file_ptr,"restrict searches to taxon id: ");
    if (do_only_taxid) fprintf(results_file_ptr,"%ld\n", (long) do_only_taxid); else fprintf(results_file_ptr,"NO\n");
    fprintf(results_file_ptr,"searches to do= %ld\n\n", (long) qed_searches);

    return 0;
}


/**
******************************************************
*                                                     
* DoSearches:    cycle through the SeaRCH database to    
*             and do the searches that were queued    
*             by CheckSearches                        
* @param
 *        Int4    max_searches                             
*                from command line parameter -m        
* @return                                            
*     Int2    0: success                                
********************************************************/
Int2 DoSearches(Int4 max_searches)
{
    Char    todays_date[8];
    Char    format_date[20];
    Char    line[100];
    CharPtr    report =                NULL;
    Int4    srch_record =            0;
    Int4    to_be_searched =        0;
    Int4    cur_gi =                0;
    CharPtr    temp_name =                NULL;
    CharPtr    cur_name =                NULL;
    Int4    cur_taxid =                0;
    Char    cur_last_searched[10];
    CharPtr    org_name =                NULL;
    Char    search_date_str[15];
    CharPtr query =                    NULL;
    Int2    size_to_allocate =        0;
    DocType    database=                TYP_ML;
    Int4    searches_performed =    0;
    ByteStorePtr    bsp=            NULL;
    LocalLinkSetPtr lsp=            NULL;
    long    number_results=            0;
    Int4    counter=                0;
    Int2    append_status=            0;
    Int4    cur_results =            0;
    Int4    results_appended =        0;
    Int4    results_already_found = 0;
    Int4    total_results_found =    0;
    Int4    seconds =                0;
    CharPtr quoted_name_ptr =       NULL;

    /*get start time and todays date*/
    seconds = GetSecs();
    date4today(todays_date);
    date4format(todays_date, format_date, "CCYY.MM.DD");
    printf("Today's date (in the form CCYY.MM.DD) is= %s\n", format_date);
    fprintf(results_file_ptr, "Today's date (in the form CCYY.MM.DD) is= %s\n", format_date);

    /*continue- by users consent or automatically if user is absent (see option -u)*/
    if ((Boolean) myargs[9].intvalue == TRUE) /*user is present*/
    {
        printf("Press the 'y' key and return if this is correct. Press any other key to quit.\n");
        (void) fgets(line, sizeof(line), stdin);
        line[StrLen(line)-1] = '\0'; /*remove the newline character*/
        switch (*line)
        {
        case 'y':/*do nothing and continue*/break;
        case 'Y':/*do nothing and continue*/break;
        default:
            /*quit*/ 
            printf("No searches performed. Quitting. \n"); 
            /*closes all open data, index and memo files*/
            code4close(&codeBase);        
            code4initUndo(&codeBase);
            return 0;
        }
    }
    else /*user is absent*/
    {
        printf("Proceeding with automatic searches using the above date.\n");
        fprintf(results_file_ptr, "Proceeding with automatic searches using the above date.\n");
    }

    /*open a line to Entrez*/
    if (!EntrezInit("entrcmd", FALSE, NULL))
    {
        printf("EntrezInit failed\n");
        ErrPostEx(SEV_ERROR,0,0, "DoSearches: EntrezInit failed\n");
        return 1; 
    }
    else
    {
        report = EntrezDetailedInfo();
        printf("STATUS REPORT\n%s\n", report);
        ErrPostEx(SEV_INFO,0,0, "EntrezInit worked \n STATUS REPORT\n%s\n", report);
    }

    /*cycle through each entry in the search database and do searches for STATUS set to 2*/
    for (srch_record = d4top(pd4_SRCH); srch_record == 0; srch_record = d4skip(pd4_SRCH, 1L))
    {
        to_be_searched = f4int(pf4_SRCH_STATUS);
        if (to_be_searched == 2 && searches_performed < max_searches)
        {
            /*get the current gi, name, taxid, and date last searched*/
            cur_gi = f4long(pf4_SRCH_GIP);
            temp_name = f4str(pf4_SRCH_NAME);
            size_to_allocate = sizeof(Char)*strlen(temp_name);
            cur_name = (CharPtr)malloc(size_to_allocate);
            if (cur_name != NULL) strcpy(cur_name, temp_name);
            if(TrimSpaces(cur_name) != 0)
                ErrPostEx(SEV_ERROR,0,0, "CheckSearches: TrimSpaces failed, possibly passed a null string\n");
            
            /* place quotes around names*/
            size_to_allocate = sizeof(Char)*strlen(cur_name) + sizeof(Char)*3;
            quoted_name_ptr = (CharPtr) malloc (size_to_allocate);
            if (quoted_name_ptr != NULL)
            {
                strcpy(quoted_name_ptr, "\"");
                strcat(quoted_name_ptr, cur_name);
                strcat(quoted_name_ptr, "\"");
                /*strcpy(cur_name, quoted_name_ptr);*/
                /*MemFree(quoted_name_ptr);*/
            }
            
            cur_taxid =    f4int(pf4_SRCH_TAXID);
            u4ncpy(cur_last_searched, f4str(pf4_SRCH_SEARCHED), sizeof(cur_last_searched));
        
            /*get name of organism for searching*/
            if (cur_taxid == 4932)
            {
                org_name = StrSave("Saccharomyces_cerevisiae");
            }
            if (cur_taxid == 9606)
            {
                org_name = StrSave("human");
            }
            if (cur_taxid == 10090)
            {
                org_name = StrSave("mice");
            }

            /*get the date component of the search*/
            if (strncmp(cur_last_searched, "10000101", 8) == 0)
            {
                search_date_str[0] = '\0';
            }
            else
            {
                strcpy (search_date_str, " & ");
                strncat(search_date_str, cur_last_searched, 4); /*search using the year component of date_last_searched*/
                strcat (search_date_str, "[DATE]");
            }

            /*compose the search*/
            size_to_allocate = sizeof(Char)*strlen(quoted_name_ptr) + sizeof(Char)*strlen(org_name) + sizeof(Char)*40;
            query = (CharPtr)malloc(size_to_allocate);
            if (query != NULL)
            {
                strcpy(query,  quoted_name_ptr);
                strcat(query, "[WORD]");
                strcat(query, " & ");
                strcat(query,  org_name);
                strcat(query, "[MESH]");
                strcat(query,  search_date_str);    
            }
            MemFree(quoted_name_ptr);

            
            /*submit the query*/
            if ((bsp = EntrezTLEvalXString(query, database, -1, NULL, NULL)) != NULL)
            {
                lsp = BSPtoLSP(bsp);
                BSFree(bsp);
            }
            else
            {
                printf("EntrezTLEvalXString failed to return a byte store pointer\n");
                ErrPostEx(SEV_INFO,0,0, "EntrezTLEvalXString failed to return a byte store pointer for search %s\n",query);
            }
            
            printf("srch %5ld\t", searches_performed);
            fprintf(results_file_ptr, "srch %5ld\t", searches_performed);
            printf("query= %60s   ", query);
            fprintf(results_file_ptr, "query= %60s   ", query);
            MemFree(query);
            ++searches_performed;
            f4assignInt(pf4_SRCH_STATUS, 3);    /*mark status as searched*/
            f4assign    (pf4_SRCH_SEARCHED, todays_date);
            /*f4assign(pf4_SRCH_SEARCHED, "10000101");*/


            /*print out the results*/
            if(lsp == NULL)
            {
                /*ErrPostEx(SEV_INFO,0,0, "BSPtoLSP failed to return a LocalLinkSet pointer\n");*/
                printf("%5d results\n", 0);
                fprintf(results_file_ptr, "%5d results\n", 0);
            }
            else
            {
                number_results = lsp->num;
                printf("%5ld results\t", number_results);
                fprintf(results_file_ptr, "%5ld results\t", number_results);
                if (number_results > MAX_RESULTS) number_results = MAX_RESULTS;
                
                    for (counter = 0; counter < number_results; counter++)
                    {
                        /*printf("%ld    ", (long) lsp->uids[counter]);*/
                        
                        /*try appending the record to the SeaRCH datafile*/
                        d4appendStart(pd4_RESULT, 0);
                        d4blank(pd4_RESULT);
                        
                        f4assignLong(pf4_RESULT_GIP, (long)cur_gi);
                        f4memoAssign(pf4_RESULT_NAME, cur_name);
                        f4assignLong(pf4_RESULT_PMID, (long)lsp->uids[counter]);
                        
                        append_status = d4append(pd4_RESULT);
                        
                        switch (append_status)
                        {
                        case r4success:
                            ++results_appended;
                            /*printf("\tresult appended\n");*/
                            cur_results = f4long(pf4_SRCH_RESULTS);
                            f4assignInt (pf4_SRCH_RESULTS, (long)(cur_results + 1));
                            break;
                        case r4unique:
                            ++results_already_found; /*the gi|name|pmid entry already exists in the database- old info is not overwritten*/
                            /*printf("\tresult already present\n");*/
                            break;
                        default:
                            ErrPostEx(SEV_ERROR,0,0, "DoSearches failed to append record. append_status = %d", append_status);
                        }

                    }/*for output loop*/
                    printf("appended= %ld\tpresent= %ld\n\n", (long) results_appended, (long) results_already_found);
                    fprintf(results_file_ptr, "appended= %ld\tpresent= %ld\n", (long) results_appended, (long) results_already_found);                    
                    fflush(results_file_ptr);
                    total_results_found = total_results_found + number_results;
                    results_appended = 0;
                    results_already_found = 0;

            }/*if there is data to output to results database*/

        }/*if (to_be_searched)*/
        
    }/*next search record*/
    printf("total results found= %ld\n",(long) total_results_found);
    fprintf(results_file_ptr, "total results found= %ld\n",(long) total_results_found);

    /*print elapsed time*/
    seconds = GetSecs() - seconds;
    printf("total time = %ld seconds\n",(long)seconds);
    fprintf(results_file_ptr, "total time = %ld seconds\n",(long)seconds);

    /*close the line to Entrez*/
    EntrezFini();
    printf("Entrez closed\n");
    fprintf(results_file_ptr, "Entrez closed\n");
    
    return 0;
}

/* open/create database files with index */
static Int2 OpenTables(void)
{
    if (!OpenACC("ACC", (Boolean) myargs[0].intvalue))
    {
        ErrPostEx(SEV_ERROR, codeBase.errorCode, 0, "OpenTables: OpenACC failed.");
        return 1;
    }

    if (!OpenSRCH("SRCH", (Boolean) myargs[0].intvalue))
    {
        ErrPostEx(SEV_ERROR, codeBase.errorCode, 0, "OpenTables: OpenSRCH failed.");
        return 1;
    }
    
    if (!OpenRESULT("RESULT", (Boolean) myargs[0].intvalue))
    {
        ErrPostEx(SEV_ERROR, codeBase.errorCode, 0, "OpenTables: OpenRESULT failed.");
        return 1;
    }

    if (!OpenISVM("ISVM", (Boolean) myargs[0].intvalue))
    {
        ErrPostEx(SEV_ERROR, codeBase.errorCode, 0, "OpenTables: OpenISVM failed.");
        return 1;
    }

    if (!OpenCORE("CORE", (Boolean) myargs[0].intvalue))
    {
        ErrPostEx(SEV_ERROR, codeBase.errorCode, 0, "OpenTables: OpenCORE failed.");
        return 1;
    }

    if (!OpenREGI("REGI", (Boolean) myargs[0].intvalue))
    {
        ErrPostEx(SEV_ERROR, codeBase.errorCode, 0, "OpenTables: OpenREGI failed.");
        return 1;
    }

    if (!OpenPOINT("POINT", (Boolean) myargs[0].intvalue))
    {
        ErrPostEx(SEV_ERROR, codeBase.errorCode, 0, "OpenTables: OpenPOINT failed.");
        return 1;
    }

    if (!OpenLOSYN("LOSYN", (Boolean) myargs[0].intvalue))
    {
        ErrPostEx(SEV_ERROR, codeBase.errorCode, 0, "OpenTables: OpenLOSYN failed.");
        return 1;
    }
  
    return 0;
}

/* open database files */
static Int2 OpenTablesNoidx(void)
{
    if (!OpenACCNoidx("ACC"))
    {
        ErrPostEx(SEV_ERROR, codeBase.errorCode, 0, "OpenTablesNoidx: OpenACCNoidx failed.");
        return 1;
    }
    
    if (!OpenLOSYNNoidx("LOSYN"))
    {
        ErrPostEx(SEV_ERROR, codeBase.errorCode, 0, "OpenTablesNoidx: OpenLOSYNNoidx failed.");
        return 1;
    }
    
    if (!OpenSRCHNoidx("SRCH"))
    {
        ErrPostEx(SEV_ERROR, codeBase.errorCode, 0, "OpenTablesNoidx: OpenSRCHNoidx failed.");
        return 1;
    }
    
    if (!OpenRESULTNoidx("RESULT"))
    {
        ErrPostEx(SEV_ERROR, codeBase.errorCode, 0, "OpenTablesNoidx: OpenRESULTNoidx failed.");
        return 1;
    }

    if (!OpenISVMNoidx("ISVM"))
    {
        ErrPostEx(SEV_ERROR, codeBase.errorCode, 0, "OpenTablesNoidx: OpenISVMNoidx failed.");
        return 1;
    }

    if (!OpenCORENoidx("CORE"))
    {
        ErrPostEx(SEV_ERROR, codeBase.errorCode, 0, "OpenTablesNoidx: OpenCORENoidx failed.");
        return 1;
    }

    if (!OpenREGINoidx("REGI"))
    {
        ErrPostEx(SEV_ERROR, codeBase.errorCode, 0, "OpenTablesNoidx: OpenREGINoidx failed.");
        return 1;
    }

    if (!OpenPOINTNoidx("POINT"))
    {
        ErrPostEx(SEV_ERROR, codeBase.errorCode, 0, "OpenTablesNoidx: OpenPOINTNoidx failed.");
        return 1;
    }

    return 0;
}

/* close database files */
static Int2 CloseTables(void)
{
    Int2 success = 0;
    if (d4close(pd4_ACC) != r4success)
    {
        ErrPostEx(SEV_ERROR, codeBase.errorCode, 0, "CloseTables: CloseACC failed.");
        success = 1;
    }

    if (d4close(pd4_SRCH) != r4success)
    {
        ErrPostEx(SEV_ERROR, codeBase.errorCode, 0, "CloseTables: CloseSRCH failed.");
        success = 1;
    }
    
    if (d4close(pd4_RESULT) != r4success)
    {
        ErrPostEx(SEV_ERROR, codeBase.errorCode, 0, "CloseTables: CloseRESULT failed.");
        success = 1;
    }

    if (d4close(pd4_ISVM) != r4success)
    {
        ErrPostEx(SEV_ERROR, codeBase.errorCode, 0, "CloseTables: CloseISVM failed.");
        success = 1;
    }

    if (d4close(pd4_CORE) != r4success)
    {
        ErrPostEx(SEV_ERROR, codeBase.errorCode, 0, "CloseTables: CloseCORE failed.");
        success = 1;
    }

    if (d4close(pd4_REGI) != r4success)
    {
        ErrPostEx(SEV_ERROR, codeBase.errorCode, 0, "CloseTables: CloseREGI failed.");
        success = 1;
    }

    if (d4close(pd4_POINT) != r4success)
    {
        ErrPostEx(SEV_ERROR, codeBase.errorCode, 0, "CloseTables: ClosePOINT failed.");
        success = 1;
    }

    if (d4close(pd4_LOSYN) != r4success)
    {
        ErrPostEx(SEV_ERROR, codeBase.errorCode, 0, "CloseTables: CloseLOSYN failed.");
        success = 1;
    }
  
    return success;
}

/* choose operations according to command line options */
static Int2 DoOptions(void)
{
    char            line[100];    
    Int2            search_type = 0; /*decides if all, new or update searches are to be performed*/
    Int4            search_taxon = 0;/*limits searches to a specific taxon*/
    Int4            keep_taxon_searches = 0; /*to remove search results except for for a specified taxon*/
    Boolean         requeue = 0; /*determines if completed searches can be requeued*/
    CharPtr         filename = NULL;
    Boolean         import_point_scores = 0; /*for importing potential interaction scores*/
    Int4            score_list = 0; /*for importing potential interaction scores: 1 = piscore list, 2 = piscore2 list*/
    Boolean         calculate_name_redundancy = 0;/*for calculating the number of times a name is used within the same taxon*/
    Boolean         overwrite = FALSE; /*are imported scores to overwrite present scores?*/
    
    /*import POINT table from text file and then quit*/
    if ( myargs[15].strvalue != NULL)
    {
        filename = myargs[15].strvalue;

        /*cycle through the RESULT file and place info in the RESULT database*/
        if (ImportPoint(filename) != 0)
        {
            ErrPostEx(SEV_ERROR, 0, 0, "DoOptions: ImportPOINT failed.");
            printf("PreBIND searcher has failed to import POINT table. Quitting.\n");
            fprintf(results_file_ptr, "PreBIND searcher has failed to import POINT table . Quitting.\n");
            return 1;
        }

        /*diagnostic line*/
        printf("Quitting.\n");
        fprintf(results_file_ptr, "Quitting.\n");
        return 0;
    }
    
    /*import RESULT table from text file and then quit*/
    if ( myargs[14].strvalue != NULL)
    {
        filename = myargs[14].strvalue;

        /*cycle through the RESULT file and place info in the RESULT database*/
        if (ImportResult(filename) != 0)
        {
            ErrPostEx(SEV_ERROR, 0, 0, "DoOptions: ImportResultScores failed.");
            printf("PreBIND searcher has failed to import Result scores. Quitting.\n");
            fprintf(results_file_ptr, "PreBIND searcher has failed to import Result scores. Quitting.\n");
            return 1;
        }

        /*diagnostic line*/
        printf("Quitting.\n");
        fprintf(results_file_ptr, "Quitting.\n");
        return 0;
    }

    /*remove all search results except for a given taxon and then quit- see option -x*/
    if (myargs[11].intvalue != -1)
    {
        keep_taxon_searches = myargs[11].intvalue;
        /*cycle through the SeaRCH and RESULTS databases and
        remove all search results except for a given taxon*/
        if (RemoveSearchResultsForAllTaxonsExceptX(keep_taxon_searches) != 0)
        {
            ErrPostEx(SEV_ERROR, 0, 0, "DoOptions: RemoveSearchResultsForAllTaxonsExceptX failed.");
            return 1;
        }
        else
        {
            printf("PreBIND searcher has finished removing all search results from the SeaRCH and RESULTS database except those for taxon %ld.\n", (long) keep_taxon_searches);
            fprintf(results_file_ptr, "PreBIND searcher has finished removing all search results from the SeaRCH and RESULTS database except those for taxon %ld.\n", (long) keep_taxon_searches);
            return 0;
        }        
    }

    /*build and analyze the search databases and then quit*/
    if ((Boolean) myargs[4].intvalue == TRUE)
    {

         AssignTagsRESULT();
         printf("finished Asisgn tags RESULT !\n");
        AssignTagsACC();
        printf("finished Asisgn tags ACC !\n");
        AssignTagsLOSYN();
        printf("finished Asisgn tags LOSYN!\n");

        /*cycle through RESULT database and add search results to the ISVM database*/
        if (AddResultsToISVM() != 0)
        {
            ErrPostEx(SEV_ERROR, 0, 0, "DoOptions: AddResultsToISVM failed.");
            return 1;
        }
        
        if(ReindexISVM()==FALSE){
            printf("reindex ISVM table error !\n");
            return 1;
        }
        else{
            printf("reindex ISVM table successfully \n");
        }
        
        /*cycle through RESULT database and count the number of names and gi's that returned each search result
        if (CountResults() != 0)
        {
            ErrPostEx(SEV_ERROR, 0, 0, "DoOptions: CountResults failed.");
            return 1;
        }
        
        if(ReindexCORE()==FALSE){
            printf("reindex CORE table error !\n");
            return 1;
        }
        else{
            printf("reindex CORE table successfully \n");
        }
        
        if(ReindexREGI()==FALSE){
            printf("reindex REGI table error !\n");
            return  1;
        }
        else{
            printf("reindex REGI table successfully \n");
        }
        */
        
        /*diagnostic line*/
        printf("Quitting.\n");
        fprintf(results_file_ptr, "Quitting.\n");
        return 0;
    }


    /*import svm interaction scores into the ISVM database and then quit*/
    if ( myargs[5].strvalue != NULL)
    {
        filename = myargs[5].strvalue;
        score_list = myargs[8].intvalue;
        overwrite = myargs[13].intvalue;
        if (score_list != 0)
        {
            /*cycle through the svm score file and place info in the ISVM database*/
            if (ImportSvmScores(filename, score_list, overwrite) != 0)
            {
                ErrPostEx(SEV_ERROR, 0, 0, "DoOptions: ImportSvmScores failed.");
                printf("PreBIND searcher has failed to import svm scores. Quitting.\n");
                fprintf(results_file_ptr, "PreBIND searcher has failed to import svm scores. Quitting.\n");
                return 1;
            }
            
            /*diagnostic line*/
            printf("Quitting.\n");
            fprintf(results_file_ptr, "Quitting.\n");
            return 0;
        }
        else if (score_list == 0)
        {
            printf("PreBIND searcher argument -l must be followed by a list number 1,2,or 3. Quitting.\n");
            return 1;
        }
    }

    /*build the potential interaction POINT database and then quit*/
    if ((Boolean) myargs[6].intvalue == TRUE)
    {
           /* 
        * open index for searching
        */
        AssignTagsREGI();
        AssignTagsCORE();
        AssignTagsACC();
        AssignTagsLOCUS();
        AssignTagsRESULT();
        
        /*cycle through CORE (COunt REsults) database and assemble a list of potential interactions in the POINT database*/
        if (BuildPOINT() != 0)
        {
            ErrPostEx(SEV_ERROR, 0, 0, "DoOptions: BuildPOINT failed.");
            printf("PreBIND searcher has failed to build the potential interaction database (POINT). Quitting.\n");
            fprintf(results_file_ptr, "PreBIND searcher has failed to build the potential interaction database (POINT). Quitting.\n");
            return 1;
        }

        if(ReindexPOINT()==FALSE){
            printf("reindex POINT table error !\n");
            return 1;
        }
        else{
            printf("reindex POINT table successfully \n");
        }
        
         /*diagnostic line*/
        printf("Quitting.\n");
        fprintf(results_file_ptr, "Quitting.\n");
        
        return 0;
    }


    /*import POtential INTeraction scores into the POINT database and then quit*/
    if ((Boolean) myargs[7].intvalue == TRUE)
    {
        import_point_scores = (Boolean) myargs[7].intvalue;
        score_list = myargs[8].intvalue;
        if (score_list == 1 || score_list == 2)
        {
            /*cycle through the point score file and place info in the POINT database*/
            if (ImportPointScores(score_list) != 0)
            {
                ErrPostEx(SEV_ERROR, 0, 0, "DoOptions: ImportPointScores failed.");
                printf("PreBIND searcher has failed to import point scores. Quitting.\n");
                fprintf(results_file_ptr, "PreBIND searcher has failed to import point scores. Quitting.\n");
                return 1;
            }    
            /*diagnostic line*/
            printf("PreBIND searcher has finished importing point scores. Quitting.\n");
            fprintf(results_file_ptr, "PreBIND searcher has finished importing point scores. Quitting.\n");
            return 0;
        }
        else
        {
            printf("PreBIND searcher argument -l must be specified in order to import POtential INTeraction scores.  Choose either -l 1 (protein-protein interaction scores) or -l 2 (protein-DNA interaction scores). Quitting.\n");
            return 1;
        }
    }
    
    /*calculate number of times that each name is used within a taxon-see NAMERED in SeaRCH db*/
    if ((Boolean) myargs[12].intvalue == TRUE)
    {
        calculate_name_redundancy = (Boolean) myargs[12].intvalue;
        /*cycle through the point score file and place info in the POINT database*/
        if (CalculateNameUsage() != 0)
        {
            ErrPostEx(SEV_ERROR, 0, 0, "DoOptions: CalculateNameUsage failed.");
            printf("PreBIND searcher has failed to calculate name usage. Quitting.\n");
            fprintf(results_file_ptr, "PreBIND searcher has failed calculate name usage. Quitting.\n");
            return 1;
        }    
        /*diagnostic line*/
        printf("PreBIND searcher has finished calculating name usage. Quitting.\n");
        fprintf(results_file_ptr, "PreBIND searcher has finished calculating name usage. Quitting.\n");
        return 0;    
    }

    /*******************************************************************
        the default functionality of pb_searcher is to perform searches
    ********************************************************************/
    if(myargs[1].strvalue != NULL){
        if (StrCmp(myargs[1].strvalue, "a") == 0)
        {
            search_type = 0;
        }
        else if (StrCmp(myargs[1].strvalue, "n") == 0)
        {
            search_type = 1;
        }
        else if (StrCmp(myargs[1].strvalue, "u") == 0)
        {
            search_type = 2;
        }
        else
        {
            printf("No correct parameters were given. Type \"PreBIND_searcher -\" for a list of possible parameter values\n");
            printf("Parameter -s (search type) must be one of three possible letters: (a)ll, (n)ew only or (u)pdate only. Quitting.\n");
            return 1;
        }

        search_taxon = myargs[2].intvalue;
        requeue = (Boolean) myargs[3].intvalue;

        /*cycle through the SeaRCH database to ensure that the medLineQuery is parseable.  Queue approved searches for searching*/
        if (CheckSearches(search_type, search_taxon, requeue) != 0)/*4932 is taxid for yeast*/
        {
            ErrPostEx(SEV_ERROR, 0, 0, "DoOptions: CheckSearches failed.");
            return 1;
        }

        /*continue- by users consent or automatically if user is absent (see option -u)*/
        if ((Boolean) myargs[9].intvalue == TRUE) /*user is present*/
        {
            printf("Press the 'y' key and return to perform %ld searches. Press any other key to quit.\n", (long) myargs[10].intvalue);
            (void) fgets(line, sizeof(line), stdin);
            line[StrLen(line)-1] = '\0'; /*remove the newline character*/
            switch (*line)
            {
                case 'y':/*do nothing and continue*/break;
                case 'Y':/*do nothing and continue*/break;
                default:
                /*quit*/ 
                printf("No searches performed. Quitting. \n"); 
                return 0;
            }
        }
        else /*user is absent*/
        {
            printf("Proceeding with %ld automatic searches.\n", (long) myargs[10].intvalue);
            fprintf(results_file_ptr, "Proceeding with %ld automatic searches.\n", (long) myargs[10].intvalue);
        }


        /*cycle through SeaRCH database and do all the searches that were approved*/
        if (DoSearches((Int4) myargs[10].intvalue) != 0)
        {
            ErrPostEx(SEV_ERROR, 0, 0, "DoOptions: DoSearches failed.");
            return 1;
        }
        return 0;
    } 
    else {
        ErrPostEx(SEV_ERROR, 0, 0, "DoOptions: Command Syntax error.");
        printf("Command Syntax error. Quitting.\n");
        fprintf(results_file_ptr, "Command Syntax error. Quitting.\n");
        return 1;
    }
  
  
}
/**
*****************************************************
*                                                   
* MAIN:                                             
*                                                   
* @param                                       
*     command line                                  
* @return                                          
*     0    ok                                       
********************************************************/
Int2 Main(void)
{  
    Char            path_to_searcher_log[PATH_MAX];
    Char            path_to_error_log[PATH_MAX];
    Char            daytime[100];
    Int2            success=0;

    /*set up error handling log*/
    path_to_error_log[0] = '\0';
    GetAppParam("prebind", "log_files", "search_errors_path", NULL, path_to_error_log, (size_t)PATH_MAX * (sizeof(Char)));
    ErrSetOptFlags(EO_LOG_SEVERITY | EO_LOG_CODES | EO_LOG_FILELINE | EO_LOG_USRFILE); /*style of log file*/
    ErrSetLogfile(path_to_error_log, ELOG_BANNER); /*error log file is defined in the prebind config file under [log_files] 'search_errors_path'*/
    ErrSetLogLevel(SEV_INFO); /*any errors => SEV_INFO are logged*/
    ErrSetMessageLevel(SEV_MAX); /*any errors => SEV_MAX are displayed as messages to the user*/
    
    /*set up a log file to which runtime output can be appended*/
    path_to_searcher_log[0] = '\0';
    GetAppParam("prebind", "log_files", "search_results_path", NULL, path_to_searcher_log, (size_t)PATH_MAX * (sizeof(Char)));
    /*open the results log file*/
    if ((results_file_ptr = FileOpen(path_to_searcher_log, "a+")) == NULL)
    {
        printf("The path '%s' is not a valid path to the search results log file.  Please set a valid path for 'search_results_path' under [log_files] in the prebind configuration file.\n",path_to_searcher_log );
        ErrPostEx(SEV_ERROR, 0, 0, "Main: Failed to open %s.", path_to_searcher_log);
        return 1;
    }

    /*diagnostic line*/
    DayTimeStr(daytime, TRUE, TRUE);
    printf("PreBIND searcher has started. The time is: %s.\n", daytime);
    fprintf(results_file_ptr, "\n\n*==*\nPreBIND searcher has started. The time is: %s.\n", daytime);
    printf("Errors will be logged to %s\n", path_to_error_log);
    fprintf(results_file_ptr, "Errors will be logged to %s\n", path_to_error_log);

    /* check command line arguments (see NCBI toolkit corelib) */
    if (!GetArgs("pb_searcher", NUMARGS, myargs))
    {
          ErrPostEx(SEV_ERROR,0,0 ,"Main: can not get command line arguments\n");
          return 1;
    }
    
    printf("Finished getting arguments\n");
    
    /*initialize codeBase*/
    if (!InitCodeBaseForPreBIND())
    {
        ErrPostEx(SEV_ERROR, codeBase.errorCode, 0, "Main: InitCodeBaseForPreBIND failed.");
        error4exitTest(&codeBase); /*quits program */
        return 1;
    }
    else 
    {
        printf("Finished init codebase.\n");
    }
    
    /*if create == TRUE, only create data tables then close, and do nothing*/
    if((Boolean) myargs[0].intvalue == FALSE){
         /* setCodeBaseFlagsExclNoidx */
        codeBase.accessMode=OPEN4DENY_RW;
        codeBase.autoOpen=0;
        success=OpenTablesNoidx();
        /* choose operations according to command line options */
        if(success!=0) printf("Failed opening all tables with no index\n");
        else 
            success = DoOptions();    
    }
    else{
        printf("Start Creating tables\n");  
        success=OpenTables();
        if(success ==0) printf("Finished Creating tables\n"); 
        else printf("Error in  Creating tables\n");
    }
    
    
    
    /* close data tables */
    success=CloseTables();
        
    DayTimeStr(daytime, TRUE, TRUE);
    ErrPostEx(SEV_INFO, 0, 0, "PreBIND searcher stops. [ %s ]", daytime);
    printf("PreBIND searcher stops. The time is: %s.\n", daytime);
    fprintf(results_file_ptr, "\n\n*==*\nPreBIND searcher stops. The time is: %s.\n", daytime);
    
    /*closes all open data, index and memo files*/
    code4close(&codeBase);        
    code4initUndo(&codeBase);

    /*close the searches log file*/
    FileClose(results_file_ptr);
    
    return success;
}

/*
$Log: prebind_searcher.c,v $
Revision 1.24  2004/03/26 22:08:03  kzheng
initial import

Revision 1.23  2004/03/19 15:34:24  kzheng
initial import

Revision 1.22  2004/03/17 23:24:39  kzheng
added functions for opening tables with no index; added reindexing of tables

Revision 1.21  2004/03/10 21:34:39  kzheng
remove export functions, add ImportPoint function

Revision 1.20  2004/02/06 21:56:49  kzheng
convert comments to javadoc format for doxygen; always use quoted molecular names to aovid term mapping

Revision 1.19  2004/01/29 21:57:21  kzheng
always add double quota to names

Revision 1.18  2003/03/28 06:55:19  janicek
Added ExportAcc

Revision 1.17  2003/03/13 04:40:52  janicek
Added ExportSrch function

Revision 1.16  2002/12/11 19:25:29  janicek
Added function to export LOSYN data table

Revision 1.15  2002/11/19 17:39:00  janicek
minor changes to random export

Revision 1.14  2002/11/16 02:46:44  janicek
Allow user to export only a percentage of ISVM records

Revision 1.13  2002/11/16 02:05:31  janicek
Export only 1/1000 pmids in ExportIsvm

Revision 1.12  2002/11/16 00:51:31  janicek
Added function to export ISVM records

Revision 1.11  2002/11/12 21:02:26  janicek
added ability to export POINTS by taxon

Revision 1.10  2002/11/08 06:50:35  janicek
minor changes

Revision 1.9  2002/11/08 06:30:18  janicek
minor changes

Revision 1.8  2002/11/08 06:22:57  janicek
minor changes to resolve conflict

Revision 1.7  2002/11/08 06:09:32  janicek
allow ImportPointScores to overwrite POINT records that already have scores is -o option is TRUE

Revision 1.6  2002/11/01 16:56:31  janicek
Added ability to export RESULTs for a specific taxon

Revision 1.5  2002/10/31 21:00:38  janicek
added functions to export tables to text: ExportPoints and ExportResults

Revision 1.4  2002/08/22 21:23:52  janicek
allowed for searching taxon 10090 (mice)

Revision 1.3  2002/08/08 18:15:47  janicek
minor changes

Revision 1.2  2002/08/08 15:33:35  janicek
improved ImportSvmScores reporting and allowed overwriting

Revision 1.1.1.1  2002/07/17 19:36:37  janicek
initial import

*/



