/* $Id: precompute.c,v 1.2 2000/09/19 23:59:51 kaca Exp
****************************************************************************************
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
           Ruth Isserlin, Katerina Michalickova, and Christopher W.V. Hogue hogue@mshri.on.ca


  REVISION/CONTRIBUTION NOTES:

  June 1, 2000  Original MSH Revision.
                Hogue Lab - University of Toronto Biochemistry
                Department and the Samuel Lunenfeld Research
                Institute, Mount Sinai Hospital
                http://bioinfo.mshri.on.ca  hogue@mshri.on.ca
                ftp://ftp.mshri.on.ca/


  PUBLICATION TO CITE:


  DESCRIPTION
  Program set ups and maintains the ability to
  have precomputed searches of all taxonomy information
  that exceeds a set time limit.  A precomputed search
  is simply all the gis associated with a single taxonomy
  stored in a file and referenced in the precomputed index
  file.
  The user has the choice to update his current precomputed searches,
  to print out his index file, to print any one of the precomputed
  searches or to redo the entire database search for all searches
  that exceed the time limit.
******************************************************************************************/
#include <slri_post.h>
#include <intrez_cfg.h>
#include <intrez_db.h>
#include <intrez.h>

#define NUMARGS 3
Args myargs[NUMARGS]={
	{"Action (redo/redo_fast/update/print_index/add_refseq/check_search/show_usage)", NULL, NULL,NULL,FALSE,'a', ARG_STRING, 0.0, 0, NULL},
	{"Input file for fast re-computation.", "taxidff", NULL, NULL, TRUE, 'i', ARG_FILE_IN, 0.0,0,NULL},
	{"Check search taxid.","-9606",NULL,NULL,TRUE,'t',ARG_INT,0.0,0,NULL}
};


#define LOGING "precomputelog"


Boolean Print_index();
Boolean Compute_precomputed();
Boolean Update_Precomputed();
void Print_Search(Int4 taxid, CharPtr type);
Boolean Print_to_file(PostingPtr * psp, CharPtr type, Int4 taxid);
Boolean Add_to_index(Int4 taxid, Int4 all, Int4 proteins, Int4 NA);
Boolean add_precomputed_RefSeq();
Boolean AllocatePostings(ByteStorePtr PNTR bsp_all, PostingPtr PNTR psp_all, ByteStorePtr PNTR bsp_na, PostingPtr PNTR psp_na, ByteStorePtr PNTR bsp_prot, PostingPtr PNTR psp_prot);
void FreePostings(ByteStorePtr PNTR bsp_all, PostingPtr PNTR psp_all, ByteStorePtr PNTR bsp_na, PostingPtr PNTR psp_na, ByteStorePtr PNTR bsp_prot, PostingPtr PNTR psp_prot);
Boolean Compute_precomputed_fast();
Boolean StoreRefSeq(Int4 *count_all, Int4 *count_na, Int4 *count_prot);

Boolean AllocatePostings(ByteStorePtr PNTR bsp_all, PostingPtr PNTR psp_all, ByteStorePtr PNTR bsp_na, PostingPtr PNTR psp_na, ByteStorePtr PNTR bsp_prot, PostingPtr PNTR psp_prot)
{

  /* SK: May be it's better to check pointers after each allocation and free the previous pointers if it's failed */
  *bsp_all = BSNew(10 * sizeof(Int4));
  *psp_all = NewPost(*bsp_all, 10 * sizeof(Int4));
  *bsp_na = BSNew(10 * sizeof(Int4));
  *psp_na = NewPost(*bsp_na, 10 * sizeof(Int4));
  *bsp_prot = BSNew(10 * sizeof(Int4));
  *psp_prot = NewPost(*bsp_prot, 10 * sizeof(Int4));
  if(*bsp_all && *psp_all && *bsp_na && *psp_na && *bsp_prot && *psp_prot)
    return TRUE;
  else
    return FALSE;

}

void FreePostings(ByteStorePtr PNTR bsp_all, PostingPtr PNTR psp_all, ByteStorePtr PNTR bsp_na, PostingPtr PNTR psp_na, ByteStorePtr PNTR bsp_prot, PostingPtr PNTR psp_prot)
{
  if(*psp_all) *psp_all = FreePost(*psp_all);
  if(*psp_na) *psp_na = FreePost(*psp_na);
  if(*psp_prot) *psp_prot = FreePost(*psp_prot);
  /* SK: Not all pointers freeing. Maybe it's a good idea to free it instead of NULLing? */
  *bsp_all = NULL;
  *bsp_na = NULL;
  *bsp_prot = NULL;
}


/* try out every taxonomy id from 0 to the largest one in the database
and time the search; if the search takes longer than the value set in
the config file store the precomputed search */
Boolean Compute_precomputed()
{

	
	Int4 i =0, largest=0;
	Int4 time1=0,time2=0;
	ByteStorePtr bsp_all = NULL, bsp_na = NULL, bsp_prot = NULL;
	PostingPtr psp_all = NULL, psp_na = NULL, psp_prot = NULL;
    Int4 num_all = 0, num_na = 0, num_prot = 0, count = 0;

    if(!Cfg_ResetQueryCount())
    {
	  ErrPostEx(SEV_ERROR, 0, 0, "Cfg_ResetQueryCount failed.");
	  return FALSE;
	}
	if(!Cfg_ResetMaxQuery())
    {
      ErrPostEx(SEV_ERROR, 0, 0, "Cfg_ResetMaxQuery failed.");
      return FALSE;
    }
    largest = GetLargestTaxid();
	for(i=1; i < largest;i++)
	{
		time1 = GetSecs();
		ErrPostEx(SEV_INFO,0,0,"%ld", (long) i);
		if(!AllocatePostings(&bsp_all, &psp_all, &bsp_na, &psp_na, &bsp_prot, &psp_prot))
		{
		  FreePostings(&bsp_all, &psp_all, &bsp_na, &psp_na, &bsp_prot, &psp_prot);
		  ErrPostEx(SEV_ERROR,0,0, "Cannot allocate memory.");
		  return FALSE;
		}
		num_all = 0;
		num_na = 0;
		num_prot = 0;
        if((count = getTaxInfoIII(i, &psp_all, &psp_na, &psp_prot, &num_all, &num_na, &num_prot)) > 0)
		{
		   time2 = GetSecs();
		   if (time2-time1 > Cfg_GetMaxQueryTime())
		   {
			ErrPostEx(SEV_INFO,0,0,"count:%ld all:%ld na:%ld prot:%ld", (long) count, (long) num_all, (long) num_na, (long) num_prot);
    			if(!Print_to_file(&psp_all, " ", i))
    			   return FALSE;
    			if(!Print_to_file(&psp_na, "NA", i))
    			   return FALSE;
    			if(!Print_to_file(&psp_prot, "protein", i))
    			   return FALSE;
			if(!Add_to_index(i,num_all, num_prot, num_na))
			   return FALSE;
		    }
		}
		FreePostings(&bsp_all, &psp_all, &bsp_na, &psp_na, &bsp_prot, &psp_prot);
	}
	return TRUE;
}

/*Add_to_index
*  Given: the taxid, the number of all gis corresponding to that taxid, 
*  the number of all proteins corresponding to that taxid, and the number of
*  of all NA correspondind to that taxid.
*  Method adds this new information to the end of index file.
*  Returns : nothing.
*/
Boolean Add_to_index(Int4 taxid, Int4 all, Int4 proteins, Int4 NA)
{

	FILE * fptr;
	CharPtr filename;
	Tax_index * index;
	Int4 count2=0;
	/* SK: */
	CharPtr ptr = NULL;


	filename = Malloc(256 * sizeof(Char));
	/* SK: NULL ptr check */
	if ( filename == NULL )
	{
	    ErrPostEx(SEV_ERROR,0,0, "Couldn't allocate memory for file name.");
	    return FALSE;
	}

        /* SK: */
	ptr = Cfg_GetIndexFile();
	sprintf(filename, "%s",IsNullStr(ptr));
	if((index = Malloc((Cfg_GetMaxQuery()+1) * sizeof(struct Tax_index))) == NULL)
	{
		ErrPostEx(SEV_ERROR,0,0, "Couldn't allocate memory for index.");
		MemFree(filename);
		return FALSE;
	}

	/*open up the index file and read in index (if there is one existing)*/
	if(Cfg_GetMaxQuery() > 0)
	{
    	fptr = FileOpen(filename, "rb");
    	if(fptr == NULL)
    	{
    		ErrPostEx(SEV_ERROR,0,0, "Couldn't open the index file.");
    		MemFree(filename);
    		MemFree(index);
    		return FALSE;
    	}

    	if(fread(index,sizeof(struct Tax_index),(Cfg_GetMaxQuery()+1), fptr) == 0)
    	{
    		ErrPostEx(SEV_ERROR,0,0, "Didn't read anything from the file.");
    		MemFree(filename);
    		MemFree(index);
    	    return FALSE;
    	}
    	fclose(fptr);
	}

	/*place to put the next record into the index file*/
	count2 = Cfg_GetQueryCount();
	/*set the index file to store new file*/
	index[count2].tax_id = taxid;
	index[count2].num_gis = all;
	index[count2].num_DNA_gis = NA;
	index[count2].num_proteins_gis = proteins;
	
	fptr = NULL;
	/*reopen index file and write the new index out*/
	fptr = FileOpen(filename, "wb");
	if(fptr == NULL)
	{
		ErrPostEx(SEV_ERROR,0,0, "Couldn't open the index file.");
		MemFree(filename);
		MemFree(index);
		return FALSE;
	}
	if(fwrite(index,sizeof(struct Tax_index),(Cfg_GetMaxQuery()+1),fptr) == 0)
	{
		ErrPostEx(SEV_ERROR,0,0, "Zero bytes of index written.");
		fclose(fptr);
		MemFree(filename);
        MemFree(index);
		return FALSE;
    }
	if(fflush(fptr)!=0)
	{
		ErrPostEx(SEV_ERROR,0,0, "Couldn't flush the buffer.");
		MemFree(filename);
		MemFree(index);
		fclose(fptr);
		return FALSE;	
	}
	if(!Cfg_IncrementQueryCount())
	{
		ErrPostEx(SEV_ERROR, 0, 0, "Cfg_IncrementQueryCount failed.");
		MemFree(filename);
		MemFree(index);
		fclose(fptr);
		return FALSE;
	}
	
    if(!Cfg_SetMaxQuery(Cfg_GetQueryCount()))
    {
     	ErrPostEx(SEV_ERROR, 0, 0, "Cfg_SetMaxQuery failed.");
     	MemFree(filename);
		MemFree(index);
		fclose(fptr);
     	return FALSE;
    }
	
	fclose(fptr);	
	MemFree(filename);
    MemFree(index);
    return TRUE;
}


	
/* Update_Precomputed searches */
/* Gets input from index of precomputed searches */
Boolean Update_Precomputed()
{
	Tax_index * index = NULL;
	Int4 tax_id =0;
	FILE * fptr = NULL;
	CharPtr filename = NULL;
	Int4 i=0;
    	ByteStorePtr bsp_all = NULL, bsp_na = NULL, bsp_prot = NULL;
	PostingPtr psp_all = NULL, psp_na = NULL, psp_prot = NULL;
	Int4 num_all = 0, num_na = 0, num_prot = 0, count = 0;
	Int4 all = 0, na = 0, prot = 0;
	/* SK: */
	CharPtr ptr = NULL;


	filename = Malloc(256 * sizeof(Char));
	/* SK: NULL ptr check */
	if(filename == NULL)
	{
		ErrPostEx(SEV_ERROR,0,0, "Couldn't allocate memory for the file name.");
		return FALSE;
	}
        ptr = Cfg_GetIndexFile();
	sprintf(filename, "%s",IsNullStr(ptr));
	index = Malloc(Cfg_GetMaxQuery() * sizeof(struct Tax_index));
	/* SK: NULL ptr check */
	if ( index == NULL )
	{
		ErrPostEx(SEV_ERROR,0,0, "Couldn't allocate memory for the index.");
		free(filename);
		return FALSE;
	}


	/*open up the index file*/
	fptr = FileOpen(filename, "rb");
	if(fptr == NULL)
	{
		ErrPostEx(SEV_ERROR,0,0, "Couldn't open the index file.");
		MemFree(filename);
		MemFree(index);
		return FALSE;
	}
	if(fread(index,sizeof(struct Tax_index),Cfg_GetMaxQuery(), fptr) == 0)
	{
		ErrPostEx(SEV_ERROR,0,0, "Didn't read anything from the file.");
		MemFree(filename);
		MemFree(index);
		return FALSE;
	}
	fclose(fptr);

	/*go through each item in the index list redoing the taxinfo search*/
	for(i=0; i<Cfg_GetQueryCount();i++)
	{
		tax_id = index[i].tax_id;
		ErrPostEx(SEV_INFO,0,0,"%ld", (long) tax_id);
		/*check to see if humans is the primates only
		which is indicated if it is a negative number*/
		if(tax_id == (-9606))
		{
			if(!StoreRefSeq(&all, &na, &prot))
			{
			   ErrPostEx(SEV_ERROR,0,0, "Primates did not get stored properly.");
			}
			index[i].tax_id = tax_id;
			index[i].num_gis = all;
			index[i].num_DNA_gis = na;
			index[i].num_proteins_gis = prot;
			ErrPostEx(SEV_INFO,0,0,"all:%ld na:%ld prot:%ld", (long) all, (long) na, (long) prot);
		}
		else if(tax_id > 0)
		{
		    if(!AllocatePostings(&bsp_all, &psp_all, &bsp_na, &psp_na, &bsp_prot, &psp_prot))
    		{
    		  FreePostings(&bsp_all, &psp_all, &bsp_na, &psp_na, &bsp_prot, &psp_prot);
    		  ErrPostEx(SEV_ERROR,0,0, "Cannot allocate memory.");
    		  return FALSE;
    		}
		        num_all = 0;
			num_na = 0;
			num_prot = 0;
			if((count = getTaxInfoIII(tax_id, &psp_all, &psp_na, &psp_prot, &num_all, &num_na, &num_prot)) > 0)
			{
    			/* overwrite the index information*/
    			index[i].tax_id = tax_id;
    			index[i].num_gis = num_all;
    			index[i].num_DNA_gis = num_na;
    			index[i].num_proteins_gis = num_prot;
    			ErrPostEx(SEV_INFO,0,0,"count:%ld all:%ld na:%ld prot:%ld", (long) count, (long) num_all, (long) num_na, (long) num_prot);
    			if(!Print_to_file(&psp_all, " ", tax_id))
    			   return FALSE;
    			if(!Print_to_file(&psp_na, "NA", tax_id))
    			   return FALSE;
    			if(!Print_to_file(&psp_prot, "protein", tax_id))
    			   return FALSE;
			}
			FreePostings(&bsp_all, &psp_all, &bsp_na, &psp_na, &bsp_prot, &psp_prot);
		}
	}

	/* write the index into a file */
	fptr = FileOpen(filename, "w+b");
	if(fptr == NULL)
	{
		ErrPostEx(SEV_ERROR,0,0, "Couldn't open the index file.");
		MemFree(filename);
		MemFree(index);
		return FALSE;
	}
	if(fwrite(index,sizeof(struct Tax_index),Cfg_GetMaxQuery(),fptr) == 0)
	{
		ErrPostEx(SEV_ERROR,0,0, "Zero bytes of index written.");
		fclose(fptr);
		MemFree(filename);
	        MemFree(index);
		return FALSE;
        }
	if(fflush(fptr)!=0)
	{
		ErrPostEx(SEV_ERROR,0,0, "Couldn't flush the buffer.");
		fclose(fptr);
		MemFree(filename);
	        MemFree(index);
		return FALSE;
	}
	fclose(fptr);
	if(filename)MemFree(filename);
	if(index)MemFree(index);
	return TRUE;
}


/* recreates the precomputed directory from scratch
basically does an update but it reads the list of GIs from a text file
comes handy if you lose your index and do not want to redo everything */
Boolean Compute_precomputed_fast()
{
	Tax_index * index;
	Int4 tax_id =0;
	FILE * fptr;
	CharPtr filename = NULL;
	Int4 i=0;
	ByteStorePtr bsp_all = NULL, bsp_na = NULL, bsp_prot = NULL;
	PostingPtr psp_all = NULL, psp_na = NULL, psp_prot = NULL;
        Char pcBuf[100];
	CharPtr pcTest = NULL;
        ValNodePtr pvntaxid = NULL;
	CharPtr stopstring = NULL;
        Int4 taxidnum = 0;
	Int4 count = 0;
        Int4 num_all = 0, num_na = 0, num_prot = 0;


	if(!Cfg_ResetQueryCount())
        {
	   ErrPostEx(SEV_ERROR, 0, 0, "Cfg_ResetQueryCount failed.");
	   return FALSE;
	}
	
	/* SK: NULL ptr check */
	if ( myargs[1].strvalue == NULL || *(myargs[1].strvalue) == '\0' )
        {
	   ErrPostEx(SEV_ERROR, 0, 0, "Index file name is empty.");
	   return FALSE;
	}

	/*open up the text file with GIs file*/
	fptr = FileOpen(myargs[1].strvalue, "r");
	if(fptr == NULL)
	{
		ErrPostEx(SEV_ERROR,0,0, "Couldn't open the index file.");
		return FALSE;
	}	
	
	/* get the file content into valnode to drive the searches; count the number of taxids */
	do{
	  pcTest = fgets(pcBuf, (size_t)100, fptr);
	  if(pcTest)
	  {
	    ValNodeAddInt(&pvntaxid, 0, strtol(pcTest, &stopstring, 10));
	    taxidnum++;
	  }
	  if((stopstring != NULL) && (stopstring[0] != '\n'))
	      ErrPostEx(SEV_ERROR,3,3, "Integer out of range.");
	}while(pcTest);
		
	/* allocate memory for a new index */
	index = Malloc(taxidnum * sizeof(struct Tax_index));
	/* SK: NULL ptr check */
	if ( index == NULL )
        {
	   ErrPostEx(SEV_ERROR, 0, 0, "Cannot allocate memory for index.");
	   return FALSE;
	}
	
	/* redo the searches for taxids from the valnode */
	while(pvntaxid != NULL)
	{	
		tax_id = pvntaxid->data.intvalue;
		ErrPostEx(SEV_INFO,0,0,"%ld", (long) tax_id);
		
		/*check to see if humans is the primates only
		which is indicated if it is a negative number*/
		if(tax_id >=0)
		{
		    if(!AllocatePostings(&bsp_all, &psp_all, &bsp_na, &psp_na, &bsp_prot, &psp_prot))
    		    {
			FreePostings(&bsp_all, &psp_all, &bsp_na, &psp_na, &bsp_prot, &psp_prot);
			ErrPostEx(SEV_ERROR,0,0, "Cannot allocate memory.");
			return FALSE;
		    }
		    index[i].tax_id = tax_id;
		    num_all = 0;
		    num_na = 0;
		    num_prot = 0;
		    if((count = getTaxInfoIII(tax_id, &psp_all, &psp_na, &psp_prot, &num_all, &num_na, &num_prot)) > 0)
		    {
    			index[i].num_gis = num_all;
    			index[i].num_DNA_gis = num_na;
    			index[i].num_proteins_gis = num_prot;
    			ErrPostEx(SEV_INFO,0,0,"count:%ld all:%ld na:%ld prot:%ld", (long) count, (long) num_all, (long) num_na, (long) num_prot);
    			if(!Print_to_file(&psp_all, " ", tax_id))
    			   return FALSE;
    			if(!Print_to_file(&psp_na, "NA", tax_id))
    			   return FALSE;
    			if(!Print_to_file(&psp_prot, "protein", tax_id))
    			   return FALSE;
    			i++;
    			if(!Cfg_IncrementQueryCount())
    			{
			  ErrPostEx(SEV_ERROR, 0, 0, "Cfg_IncrementQueryCount failed.");
			  return FALSE;
			}
		    }
		    FreePostings(&bsp_all, &psp_all, &bsp_na, &psp_na, &bsp_prot, &psp_prot);
		}
	    pvntaxid = pvntaxid->next;
	}
	
	
	if(!Cfg_SetMaxQuery(Cfg_GetQueryCount()))
	{
	    ErrPostEx(SEV_ERROR, 0, 0, "Cfg_SetMaxQuery failed.");
	    MemFree(index);
	    return FALSE;
	}
	/*write new index into a file*/
	filename = Malloc(256 * sizeof(Char));
	/* SK: NULL ptr check */
	if ( filename == NULL )
	{
	    ErrPostEx(SEV_ERROR, 0, 0, "Cannot allocate memory for file name.");
	    MemFree(index);
	    return FALSE;
	}
	sprintf(filename, "%s",IsNullStr(Cfg_GetIndexFile()));
	fptr = FileOpen(filename, "w+b");
	if(fptr == NULL)
	{
		ErrPostEx(SEV_ERROR,0,0, "Couldn't open the index file.");
		MemFree(filename);
	        MemFree(index);
		return FALSE;
	}
	if(fwrite(index,sizeof(struct Tax_index),Cfg_GetQueryCount(),fptr) == 0)
	{
		ErrPostEx(SEV_ERROR,0,0, "Zero bytes of index written.");
		fclose(fptr);
		MemFree(filename);
	        MemFree(index);
		return FALSE;
        }
	if(fflush(fptr)!=0)
	{
		ErrPostEx(SEV_ERROR,0,0, "Couldn't flush the buffer.");
		fclose(fptr);
		MemFree(filename);
	        MemFree(index);
		return FALSE;
        }
	fclose(fptr);
	MemFree(filename);
        MemFree(index);
	return TRUE;
}


/*
*Doron- Nov 2001
*Formaly StorePrimates was changed to Store RefSeq
* Same function as before but stores gi from human RefSeq rather
* than human primates division*/
/* writes out files with gis from primates, uses the precomputed human searches*/
Boolean StoreRefSeq(Int4 *count_all, Int4 *count_na, Int4 *count_prot)
{
   Int4 gival=0,size=0, i=0;
   ByteStorePtr bsp_all = NULL, bsp_na = NULL, bsp_prot = NULL;
   PostingPtr psp_all = NULL, psp_na = NULL, psp_prot = NULL;
   Int4 *gis = NULL;

   if (!OpenPARTI("PARTI", FALSE))
   {
        ErrPostEx(SEV_ERROR,0,0, "Cannot open database PARTI.");
	return FALSE;
   }

   /* SK: NULL ptr check */
   if ( AllocatePostings(&bsp_all, &psp_all, &bsp_na, &psp_na, &bsp_prot, &psp_prot) == FALSE )
   {
        ErrPostEx(SEV_ERROR,0,0, "Cannot allocate postings.");
	FreePostings(&bsp_all, &psp_all, &bsp_na, &psp_na, &bsp_prot, &psp_prot);
	return FALSE;
   }

    /*get all the tax infor for humans*/
    size = Check_precomputed_index(&gis, 9606, " ");
    /*go through each one to see if the gi is from the pri database.*/
    if(size == 0)
    {
      ErrPostEx(SEV_ERROR,0,0,"Nothing retrieved for humans from precomputed searches.\n");
      ClosePARTI();
      return FALSE;
    }
    for(i=0;i < size;i++)
    {
        gival = gis[i];
	/*Doron -Nov 2001
	* Changed Is this division to check for RefSeq
	* rather than primates- only cahnge that was made*/
	/*if (IsThisDivision(gival, "pri"))*/
	if(IsThisDivision(gival, "gbrscu"))
	{
         WriteItem(psp_all,gival);
         (*count_all)++;
        }
    }
    if(BSLen(bsp_all) == 0)
    {
      ErrPostEx(SEV_ERROR,0,0,"The BS for all is empty.\n");
      ClosePARTI();
      return FALSE;
    }

    size = 0;
    size = Check_precomputed_index(&gis, 9606, "NA");
    if(size == 0)
    {
      ErrPostEx(SEV_ERROR,0,0,"Nothing read from the index file of precomputed.\n");
      ClosePARTI();
      return FALSE;
    }
    for(i=0;i < size;i++)
    {
    	gival = gis[i];
    	if (IsThisDivision(gival, "gbrscu"))
    	{
    	  WriteItem(psp_na,gival);
    	  (*count_na)++;
    	}
    }
    if(BSLen(bsp_na) == 0)
    {
      ErrPostEx(SEV_ERROR,0,0,"The BS for na is empty.\n");
      ClosePARTI();
      return FALSE;
    }

    size = 0;
    size = Check_precomputed_index(&gis, 9606, "protein");
    if(size == 0)
    {
      ErrPostEx(SEV_ERROR,0,0,"Nothing read from the index file of precomputed.\n");
      ClosePARTI();
      return FALSE;
    }
    for(i=0;i < size;i++)
    {
    	gival = gis[i];
    	if (IsThisDivision(gival, "gbrscu"))
    	{
    	  WriteItem(psp_prot,gival);
    	  (*count_prot)++;
    	}
    }
    if(BSLen(bsp_prot) == 0)
    {
      ErrPostEx(SEV_ERROR,0,0,"The BS for prot is empty.\n");
      ClosePARTI();
      return FALSE;
    }

    if(!Print_to_file(&psp_all, " ", -9606))
    {
        FreePostings(&bsp_all, &psp_all, &bsp_na, &psp_na, &bsp_prot, &psp_prot);
        if(gis)MemFree(gis);
        ClosePARTI();
        return FALSE;
    }
    if(!Print_to_file(&psp_na, "NA", -9606))
    {
	    FreePostings(&bsp_all, &psp_all, &bsp_na, &psp_na, &bsp_prot, &psp_prot);
	    if(gis)MemFree(gis);
	    ClosePARTI();
	    return FALSE;
    }
    if(!Print_to_file(&psp_prot, "protein", -9606))
    {
	    FreePostings(&bsp_all, &psp_all, &bsp_na, &psp_na, &bsp_prot, &psp_prot);
	    if(gis)MemFree(gis);
	    ClosePARTI();
	    return FALSE;
    }

    FreePostings(&bsp_all, &psp_all, &bsp_na, &psp_na, &bsp_prot, &psp_prot);
    if(gis) MemFree(gis);
    ClosePARTI();
    return TRUE;
}

/*Doron -Nov. 2001
* changed from primates to RefSeq*/
/* calls a function which selects all RefSeq from human precomputed
file and writes them out in binary */
Boolean add_precomputed_RefSeq()
{
	Int4 count_all=0, count_na=0, count_prot=0;

	if(!StoreRefSeq(&count_all, &count_na, &count_prot))
	{
	  ErrPostEx(SEV_ERROR,0,0, "StoreRefSeq failed.");
	  return FALSE;
	}
	if(!Add_to_index(-9606,count_all,count_prot,count_na))
	  return FALSE;

   	return TRUE;
}



/*Given the posting, the type of file(NA,protein,all), and the
* taxid of the information stored in the posting
* method takes information and stores it in a binary file with
* the name precomputedpath/TaxidType
*/
Boolean Print_to_file(PostingPtr * psp, CharPtr type, Int4 tax_id)
{
	FILE * newfptr = NULL;
	CharPtr newFilename = NULL;
	LocalLinkSetPtr lsp = NULL;
	Int2 bytes=0;


	    if(*psp == NULL)
	    {
	       ErrPostEx(SEV_INFO,0,0, "Posting is NULL for type %s, taxid %ld", type, tax_id);
	       return FALSE;
	    }

	    if(BSLen((*psp)->uids) == 0)
	    {
	       ErrPostEx(SEV_INFO,0,0, "Posting has no data for type %s, taxid %ld", type, tax_id);
	       return TRUE;
	    }

	    /*write the new info to the file*/
		newFilename = Malloc(256 * sizeof(Char));
		/* SK: NULL ptr check */
		if(newFilename == NULL)
		{
			ErrPostEx(SEV_ERROR,0,0, "Cannot allocate memory for new file name." );
			return FALSE;
		}
		sprintf(newFilename, "%s%ld%s",IsNullStr(Cfg_GetPrecompuPath()), (long) tax_id, IsNullStr(type));
		newfptr = FileOpen(newFilename, "wb");
		if(newfptr == NULL)
		{
			ErrPostEx(SEV_ERROR,0,0, "Couldn't open file for %ld.", (long) tax_id);
			MemFree(newFilename);
			return FALSE;
		}
		if((lsp = BSPtoLSP((*psp)->uids)) == NULL)
		{
			ErrPostEx(SEV_ERROR,0,0, "BSPtoLSP failed on %ld.", (long) tax_id);
			fclose(newfptr);
			MemFree(newFilename);
			return FALSE;
	    }
		bytes = fwrite(lsp->uids,sizeof(Int4),lsp->num, newfptr);
		if(fflush(newfptr)!=0)
		{
			ErrPostEx(SEV_ERROR,0,0, "Couldn't flush the buffer.");
			LocalLinkSetFree(lsp);
			fclose(newfptr);
			MemFree(newFilename);
			return FALSE;
		}
		if(bytes==0)
		{
			ErrPostEx(SEV_ERROR,0,0, "Didn't put anything in the file for taxid %ld.", (long) tax_id);
			LocalLinkSetFree(lsp);
			fclose(newfptr);
			MemFree(newFilename);
			return FALSE;
		}

		if(lsp)LocalLinkSetFree(lsp);
		fclose(newfptr);
		MemFree(newFilename);
        return TRUE;
}

/*Prints all the contents of the index file*/
Boolean Print_index()
{
	Tax_index * index;
	FILE * fptr;
	CharPtr filename;
	Int4 i=0;
	/* SK: */
	CharPtr ptr;

	filename = Malloc(256 * sizeof(Char));
	/* SK: NULL ptr check */
	if(filename == NULL)
	{
		ErrPostEx(SEV_ERROR,0,0, "Couldn't allocate memory for file name.");
		return FALSE;
	}
	/* SK: */
	ptr = Cfg_GetIndexFile();
	sprintf(filename, "%s",IsNullStr(ptr));
	index = Malloc(Cfg_GetMaxQuery() * sizeof(struct Tax_index));
	/* SK: NULL ptr check */
	if(index == NULL)
	{
		ErrPostEx(SEV_ERROR,0,0, "Couldn't allocate memory for index.");
		MemFree(filename);
		return FALSE;
	}
	
	fptr = FileOpen(filename, "rb");
	if(fptr == NULL)
	{
		ErrPostEx(SEV_ERROR,0,0, "Couldn't open the index file.");
		MemFree(filename);
		MemFree(index);
		return FALSE;
	}

	if(fread(index,sizeof(struct Tax_index),Cfg_GetMaxQuery(), fptr) == 0)
	{
		ErrPostEx(SEV_ERROR,0,0, "Didn't read anything from the file.");
		fclose(fptr);
		MemFree(filename);
	        MemFree(index);
		return FALSE;
	}

	for(i=0; i<Cfg_GetQueryCount(); i++)
	{
		fprintf(stdout, "%ld:  Tax id - %ld  Num gis-%ld  Num NA gis-%ld  Num protein gis-%ld\n", (long) i, (long) index[i].tax_id,(long) index[i].num_gis, (long) index[i].num_DNA_gis, (long) index[i].num_proteins_gis);
	}

	fclose(fptr);
	MemFree(filename);
	MemFree(index);
        return TRUE;
}

/*Prints all the contents of the index file*/
/*Given : the taxid of the file that you want to see*/
void Print_Search(Int4 taxid, CharPtr type)
{
	Int4 * gis;
	Int4  count =0, size = 0;

	size = Check_precomputed_index(&gis, taxid, type);
	/*print out all the gis stored in this file*/
	while(count < size)
	{
		fprintf(stdout, "%ld\n", (long) gis[count]);
		count++;
	}
	fprintf(stdout, "count:%ld\n", (long) count);
	if(count == 0)
		fprintf(stdout, "There is no precomputed search for taxid %ld.", (long) taxid);
}


Int2 Main()
{
	CharPtr action;


	/* check command line arguments */
	if (!GetArgs("precompute",NUMARGS, myargs))
		return 1;

	ErrSetOptFlags(EO_LOG_SEVERITY | EO_LOG_CODES | EO_LOG_FILELINE | EO_LOG_USRFILE);
	ErrSetLogfile(LOGING, ELOG_BANNER);
	ErrSetLogLevel(SEV_INFO);
	ErrSetMessageLevel(SEV_MAX);

	Cfg_intrez_new();
	action = myargs[0].strvalue;
	ErrPostEx(SEV_INFO,0,0, "%s",IsNullStr(myargs[0].strvalue));

	InitCodeBase();
	if(!OpenTAXGI("TAXGI", FALSE))
	{
		ErrPostEx(SEV_ERROR,0,0, "OpenTAXGI failed.\n");
		CloseCodeBase();
		Cfg_FreeMem();
		return 1;
	}

	if(strcmp(action, "redo") == 0)
	{
		if(!Compute_precomputed()) goto getout;
	}
	else if(strcmp(action, "update") == 0)
	{
		if(!Update_Precomputed()) goto getout;
	}
	else if(strcmp(action, "print_index") == 0)
	{
		if(!Print_index())goto getout;
        }
	else if(strcmp(action, "add_refseq") == 0)
	{
		if(!add_precomputed_RefSeq()) goto getout;
	}
	else if(strcmp(action, "check_search") == 0)
	{
		Print_Search(myargs[2].intvalue," ");
	}
	else if(strcmp(action, "redo_fast") == 0)
	{
		if(!Compute_precomputed_fast())goto getout;
	}
	else
	{
		fprintf(stdout, "usage: precompute -a redo|update|print_index|redo_fast|add_refseq|check_search|show_usage\n");
		fprintf(stdout, "SeqHound precomputed searches:\n");
		fprintf(stdout, "(redo)  Test timing for all taxids and precompute searches above threshold time from config file.\n");
		fprintf(stdout, "(redo_fast) Redo the searches from a list of taxonomy IDs.\n");
		fprintf(stdout, "(update)  Recompute the searches in the index file.\n");
		fprintf(stdout, "(print_index)  Print out the content of index file.\n");
		fprintf(stdout, "(add_refseq) Add refseq division to precomputed searches.\n");
		fprintf(stdout, "(check_search) Print precomputed GIs for taxonomy ID.\n");
	}


	Cfg_FreeMem();
	CloseTAXGI();
	CloseCodeBase();
	ErrPostEx(SEV_INFO,0,0, "Done.");
	return 0;

   getout:
    {
     	Cfg_FreeMem();
    	CloseTAXGI();
    	CloseCodeBase();
    	return 1;
    }
}

/*
 $Log: precompute.c,v $
 Revision 1.3  2004/01/12 21:08:33  zwang
 Check the length of bytestore at the beginning of function Print_to_file. It fixes bug #1208 in bugzilla.

 Revision 1.2  2004/01/06 22:32:12  skuznets
 Code audit has been done sucessfully.
 Possible NULL pointer issues has been eliminated.

 Revision 1.1.1.4  2002/09/27 08:33:40  root
 Automated import from Sourceforge.net

 Revision 1.4  2002/09/26 15:42:16  kaca
 changed action names to more user friendly

 Revision 1.3  2001/12/03 16:31:40  betel
 Correction to RefSeq precompute and cddb parser

 Revision 1.1.1.2  2001/10/10 21:53:47  kaca
 initial import

 Revision 1.19  2001/07/25 20:18:01  kaca
 consolidated function versions

 Revision 1.18  2001/05/25 20:49:04  kaca
 PARTI database open moved out of the library

 Revision 1.17  2001/05/02 19:20:35  kaca
 update bug fix

 Revision 1.16  2001/04/27 23:14:14  kaca
 cosmetic fix

 Revision 1.15  2001/04/17 16:18:59  kaca
 print results to log file, better for cron job

 Revision 1.14  2001/04/16 17:11:37  kaca
 finalizing new sped-up version

 Revision 1.13  2001/04/09 15:52:32  kaca
 removed unused variables

 Revision 1.12  2001/04/06 22:39:06  kaca
 major rewrite; fixed memory management; sped up to few minutes full precompute search

 Revision 1.11  2001/04/03 19:21:12  kaca
 added include

 Revision 1.10  2001/03/26 19:47:53  kaca
 changes in include files

 Revision 1.9  2001/03/19 20:49:36  kaca
 added log file loging

 Revision 1.8  2001/02/23 20:40:38  kaca
 license change

 Revision 1.7  2001/02/07 21:45:01  kaca
 removed unused functions

 Revision 1.6  2001/02/05 21:10:48  kaca
 commented out unused functions

 Revision 1.5  2001/01/26 19:18:50  kaca
 removed unused variables

 Revision 1.4  2000/11/06 17:30:41  kaca
 looks for NAs instead of DNAs
 Revision 1.2  2000/09/19 23:59:51  kaca
 added header
*/





