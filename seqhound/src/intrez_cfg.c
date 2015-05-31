/* $Id: intrez_cfg.c,v 1.4 2004/04/08 20:12:30 mmatan Exp $
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
           Katerina Michalickova and Christopher W.V. Hogue hogue@mshri.on.ca


  REVISION/CONTRIBUTION NOTES:

  June 1, 2000  Original MSH Revision.
                Hogue Lab - University of Toronto Biochemistry
                Department and the Samuel Lunenfeld Research
                Institute, Mount Sinai Hospital
                http://bioinfo.mshri.on.ca  hogue@mshri.on.ca
                ftp://ftp.mshri.on.ca/


  PUBLICATION TO CITE:


  DESCRIPTION
  November 1998
  SeqHound system API

    July 1999
    added a new field to the ACCDB database: version of accession to reflect the
    NCBI versoning system
    September 1999
    added the REDUND database and took redundant info out of TAXGIFIN and TAXGI
    TAXGIFIN taken out of the system
    October 1999
    title in ACCDB now stored in Memo field
    June 2000
    removed all remote functions, they live in ambiseqhoundapi.c only the lighweight
    (no local obtion) version is maintained

******************************************************************************************/
#include<intrez_cfg.h>
#include<slri_misc.h>
#include <stdio.h>


/***************************************************/
/*     functions accessing the config file         */
/***************************************************/

static SHoundCfgPtr cfgSHoundp = NULL;

/*allocates new variable at start of program - memory is private to the cfg module*/
void Cfg_shound_new(void)
{
   cfgSHoundp = MemNew((size_t) sizeof(SHoundCfg));
   cfgSHoundp->MaxQueryTime = -1;
   cfgSHoundp->MaxQueries = -1;
   cfgSHoundp->QueryCount = 0;
   cfgSHoundp->godb = -1;
   cfgSHoundp->lldb = -1;
   cfgSHoundp->taxdb = -1;
   cfgSHoundp->neigdb = -1;
   cfgSHoundp->strucdb = -1;
   cfgSHoundp->gendb = -1;
   cfgSHoundp->redundb = -1;
   cfgSHoundp->cddb = -1;
   cfgSHoundp->rpsdb = -1;
   cfgSHoundp->precompu_path = NULL;
   cfgSHoundp->indexFile = NULL;
   cfgSHoundp->Cgi_path = NULL;
   cfgSHoundp->dbxref = -1;
}

/*deallocates the memory stored.*/
void Cfg_FreeMem(void)
{
   /* SK: NULL ptr check */
   if ( cfgSHoundp == NULL )
      return;

   if(cfgSHoundp->precompu_path) MemFree(cfgSHoundp->precompu_path);
   if(cfgSHoundp->indexFile) MemFree(cfgSHoundp->indexFile);
   if(cfgSHoundp->Cgi_path) MemFree(cfgSHoundp->Cgi_path);
   MemFree(cfgSHoundp);
}


Boolean Cfg_exists(void)
{
  if(cfgSHoundp == NULL)
    return FALSE;
  else
    return TRUE;
}

/*gets the cgi path from the intrez file (or from memory
depending on whether is has been accessed before)*/
CharPtr Cfg_GetCgiPath(void)
{
        CharPtr buf=NULL;
        Int2 bufSize = 128;

        if(cfgSHoundp==NULL) {
                ErrPostEx(SEV_ERROR, 0, 0, "Programmer error: cfgSHoundp has not been allocated.");
                return(NULL);
        }

        if(cfgSHoundp->Cgi_path != NULL)
                return(cfgSHoundp->Cgi_path);

        buf = MemNew((size_t) bufSize*sizeof(Char));

        GetAppParam("intrez", "config", "CGI", "Error: Set the CGI tag in the config file!", buf, (Int2)(bufSize * sizeof(Char)));
        if(strlen(buf) == 0)
           ErrPostEx(SEV_ERROR, 0, 0, "Cfg_GetCgiPath: Config file or an entry in a config file missing.");
        cfgSHoundp->Cgi_path = StringSave(buf);

        buf = MemFree(buf);
        return(cfgSHoundp->Cgi_path);
}

/*gets the precomputed data path from the intrez file (or from memory
depending on whether is has been accessed before)*/
CharPtr Cfg_GetPrecompuPath(void)
{
        CharPtr buf=NULL;
        Int2 bufSize = 128;

        if(cfgSHoundp==NULL) {
                ErrPostEx(SEV_ERROR, 0, 0, "Programmer error: cfgSHoundp has not been allocated.");
                return(NULL);
        }

        if(cfgSHoundp->precompu_path != NULL)
                return(cfgSHoundp->precompu_path);

        buf = MemNew((size_t) bufSize*sizeof(Char));

        GetAppParam("intrez", "precompute", "path", "Error: Set the precompute path tag in the config file!", buf, (Int2)(bufSize * sizeof(Char)));
        if(strlen(buf) == 0)
           ErrPostEx(SEV_ERROR, 0, 0, "Cfg_GetPrecompuPath: Config file or an entry in a config file missing.");
        cfgSHoundp->precompu_path = StringSave(buf);

        buf = MemFree(buf);
        return(cfgSHoundp->precompu_path);
}

/*gets the index file path from the intrez file (or from memory
depending on whether is has been accessed before)*/
CharPtr Cfg_GetIndexFile(void)
{
        CharPtr buf=NULL;
        Int2 bufSize = 128;

        if(cfgSHoundp==NULL) {
                ErrPostEx(SEV_ERROR, 0, 0, "Programmer error: cfgSHoundp has not been allocated.");
                return(NULL);
        }

        if(cfgSHoundp->indexFile != NULL)
                return(cfgSHoundp->indexFile);

        buf = MemNew((size_t) bufSize*sizeof(Char));

        GetAppParam("intrez", "precompute", "indexfile", "Error: Set the index path tag in the config file!", buf, (Int2)(bufSize * sizeof(Char)));
        if(strlen(buf) == 0)
           ErrPostEx(SEV_ERROR, 0, 0, "Cfg_GetIndexFile: Config file or an entry in a config file missing.");
        cfgSHoundp->indexFile = StringSave(buf);

        buf = MemFree(buf);
        return(cfgSHoundp->indexFile);
}

 /* Gets any config file integer, given a cfg file section/key pair.
   Returns -1 if there is an error.*/
Int4 Cfg_GetIntValue(CharPtr section, CharPtr key)
{
        Int4 value=0;
        CharPtr buf=NULL, tmp=NULL;

        buf = MemNew((size_t) 4*sizeof(Char));
        Mem_Check(buf);
        tmp = MemNew((size_t) 32*sizeof(Char));
        Mem_Check(tmp);

        /*Error - Please make sure that a SHound configuration file exists.*/
        sprintf(buf, "-1");

        /*search the init file for this information*/
        GetAppParam("intrez", section, key, buf, tmp, (size_t) 32 * sizeof(Char));
        if(strlen(tmp) == 0)
           ErrPostEx(SEV_ERROR, 0, 0, "Cfg_GetIntValue: Config file or an entry in a config file missing.");
        /*convert to int*/
        tmp[31]='\0';   /*protect atoi function*/
        value = atoi(tmp);

        buf = MemFree(buf);
        tmp = MemFree(tmp);
        return(value);
}

/*gets the maximum time spent on query before precomputing the search
 from the intrez file (or from memory
depending on whether is has been accessed before)*/
Int4 Cfg_GetMaxQueryTime(void)
{

        if(cfgSHoundp==NULL) {
                ErrPostEx(SEV_ERROR, 0, 0, "Programmer error: cfgshoundp has not been allocated.");
                return -1;
        }

        if(cfgSHoundp->MaxQueryTime != -1)
                return(cfgSHoundp->MaxQueryTime);
        cfgSHoundp->MaxQueryTime = Cfg_GetIntValue("precompute","MaxQueryTime");
	

        return(cfgSHoundp->MaxQueryTime);
}

/*gets the number of precomputed queries done
 from the intrez file (or from memory
depending on whether is has been accessed before)*/
Int4 Cfg_GetQueryCount(void)
{
  CharPtr indexFileName = NULL;
  FILE* indexfp = NULL;
  Int4 indexFileSizeInBytes = 0;

        if(cfgSHoundp==NULL) {
                ErrPostEx(SEV_ERROR, 0, 0, "Programmer error: cfgshoundp has not been allocated. ");
                return -1;
        }

	indexFileName=Cfg_GetIndexFile();
	if ((indexfp = fopen(indexFileName, "rb")) == NULL ) {
	  ErrPostEx(SEV_ERROR, 0, 0, "Cfg_GetQueryCount: Unable to open precompute index file ");
                return -1;
	}
	if (fseek(indexfp,0,SEEK_END) != 0) {
	  ErrPostEx(SEV_ERROR, 0, 0, "Cfg_GetQueryCount: Unable to fseek() precompute index file ");
	}
	/* Get offset of end of file, which should be equivalent to the size of the file */
	if ((indexFileSizeInBytes = (Int4) ftell(indexfp)) == -1) {
	  ErrPostEx(SEV_ERROR, 0, 0, "Cfg_GetQueryCount: ftell() of precompute index file failed, returned %d ",indexFileSizeInBytes);
	}
	if (fclose(indexfp) != 0) {
	  ErrPostEx(SEV_ERROR, 0, 0, "Cfg_GetQueryCount: fclose() of precompute index file failed");
	}
	cfgSHoundp->QueryCount = (Int4) (indexFileSizeInBytes / ((Int4) sizeof(struct Tax_index)));
	
        return(cfgSHoundp->QueryCount);
}

/*Whenever we increment the query count we increase its
value by one. */
Boolean Cfg_IncrementQueryCount(void){
	Char * buf;

	buf = Malloc(256*sizeof(Char));

	/* SK: NULL ptr check */
	if ( buf == NULL )
	{
	    ErrPostEx(SEV_ERROR, 0, 0, "Cannot allocate memory for buffer.");
	    return FALSE;
	}
	
	cfgSHoundp->QueryCount = cfgSHoundp->QueryCount+1;
	sprintf(buf, "%ld",(long) cfgSHoundp->QueryCount);
	if(!SetAppParam("intrez", "precompute", "QueryCount",buf))
	{
	    ErrPostEx(SEV_ERROR, 0, 0, "Cannot set the parameters in the config file; check permissions.");
	    MemFree(buf);
	    return FALSE;
	}
	MemFree(buf);
	return TRUE;
}

/*Whenever we increment the amount in our index we double it
size*/
Boolean Cfg_IncrementMaxQuery(void){
	Char * buf;

	buf = Malloc(256*sizeof(Char));

	/* SK: NULL ptr check */
	if ( buf == NULL )
	{
	    ErrPostEx(SEV_ERROR, 0, 0, "Cannot allocate memory for buffer.");
	    return FALSE;
	}

	cfgSHoundp->MaxQueries = cfgSHoundp->MaxQueries*2;
	sprintf(buf, "%ld", (long) cfgSHoundp->MaxQueries);
	if(!SetAppParam("intrez", "precompute", "MaxQueries", buf))
	{
	    ErrPostEx(SEV_ERROR, 0, 0, "Cannot set the parameters in the config file; check permissions.");
	    MemFree(buf);
	    return FALSE;
	}
	MemFree(buf);
	return TRUE;
}

Boolean Cfg_ResetQueryCount(void)
{
   return Cfg_SetQueryCount(0);
}

Boolean Cfg_ResetMaxQuery(void)
{

   return Cfg_SetMaxQuery(0);
}

Boolean Cfg_SetQueryCount(Int4 number)
{
   Char str[11];

   cfgSHoundp->QueryCount = number;
   sprintf(str, "%ld", (long) number);
   if(!SetAppParam("intrez", "precompute", "QueryCount", str))
   {
	    ErrPostEx(SEV_ERROR, 0, 0, "Cannot set the parameters in the config file; check permissions.");
	    return FALSE;
   }
   return TRUE;
}


Boolean Cfg_SetMaxQuery(Int4 number)
{
  Char str[11];

  cfgSHoundp->MaxQueries = number;
  sprintf(str, "%ld", (long)number);
  if(!SetAppParam("intrez", "precompute", "MaxQueries", str))
  {
    ErrPostEx(SEV_ERROR, 0, 0, "Cannot set the parameters in the config file; check permissions.");
	    return FALSE;
  }
  return TRUE;

}

/*gets the max number of queries to be precomputed
 from the intrez file (or from memory
depending on whether is has been accessed before)*/
Int4 Cfg_GetMaxQuery(void)
{
        if(cfgSHoundp==NULL) {
                ErrPostEx(SEV_ERROR, 0, 0, "Programmer error: cfgSHoundp has not been allocated.");
                return -1;
        }

        if(cfgSHoundp->MaxQueries != -1)
                return(cfgSHoundp->MaxQueries);
        cfgSHoundp->MaxQueries = Cfg_GetIntValue("precompute","MaxQueries");

        return(cfgSHoundp->MaxQueries);
}


Boolean SHoundModule(CharPtr psection)
{
    if(cfgSHoundp==NULL)
    {
      ErrPostEx(SEV_ERROR, 0, 0, "Programmer error: cfgshoundp has not been allocated. ");
      return FALSE;
    }


    if(StringCmp(psection, "godb") == 0)
    {
      if(cfgSHoundp->godb == -1)
        cfgSHoundp->godb = Cfg_GetIntValue("sections","godb");

      if (cfgSHoundp->godb == 1)
        return TRUE;
      if (cfgSHoundp->godb == 0)
        return FALSE;
      else
      {
        ErrPostEx(SEV_ERROR, 0, 0, "SHoundModule: Cannot get config file entry for section %s", IsNullStr(psection));
        return FALSE;
      }
    }
    if(StringCmp(psection, "lldb") == 0)
    {
      if(cfgSHoundp->lldb == -1)
        cfgSHoundp->lldb = Cfg_GetIntValue("sections","lldb");

      if (cfgSHoundp->lldb == 1)
        return TRUE;
      if (cfgSHoundp->lldb == 0)
        return FALSE;
      else
      {
        ErrPostEx(SEV_ERROR, 0, 0, "SHoundModule: Cannot get config file entry for section %s", IsNullStr(psection));
        return FALSE;
      }
    }
    if(StringCmp(psection, "taxdb") == 0)
    {
      if(cfgSHoundp->taxdb == -1)
        cfgSHoundp->taxdb = Cfg_GetIntValue("sections","taxdb");

      if (cfgSHoundp->taxdb == 1)
        return TRUE;
      if (cfgSHoundp->taxdb == 0)
        return FALSE;
      else
      {
        ErrPostEx(SEV_ERROR, 0, 0, "SHoundModule: Cannot get config file entry for section %s", IsNullStr(psection));
        return FALSE;
      }
    }
    if(StringCmp(psection, "neigdb") == 0)
    {
      if(cfgSHoundp->neigdb == -1)
        cfgSHoundp->neigdb = Cfg_GetIntValue("sections","neigdb");

      if (cfgSHoundp->neigdb == 1)
        return TRUE;
      if (cfgSHoundp->neigdb == 0)
        return FALSE;
      else
      {
        ErrPostEx(SEV_ERROR, 0, 0, "SHoundModule: Cannot get config file entry for section %s", IsNullStr(psection));
        return FALSE;
      }
    }
    if(StringCmp(psection, "strucdb") == 0)
    {
      if(cfgSHoundp->strucdb == -1)
        cfgSHoundp->strucdb = Cfg_GetIntValue("sections","strucdb");

      if (cfgSHoundp->strucdb == 1)
        return TRUE;
      if (cfgSHoundp->strucdb == 0)
        return FALSE;
      else
      {
        ErrPostEx(SEV_ERROR, 0, 0, "SHoundModule: Cannot get config file entry for section %s", IsNullStr(psection));
        return FALSE;
      }
    }
    if(StringCmp(psection, "gendb") == 0)
    {
      if(cfgSHoundp->gendb == -1)
        cfgSHoundp->gendb = Cfg_GetIntValue("sections","gendb");

      if (cfgSHoundp->gendb == 1)
        return TRUE;
      if (cfgSHoundp->gendb == 0)
        return FALSE;
      else
      {
        ErrPostEx(SEV_ERROR, 0, 0, "SHoundModule: Cannot get config file entry for section %s", IsNullStr(psection));
        return FALSE;
      }
    }
    if(StringCmp(psection, "redundb") == 0)
    {
      if(cfgSHoundp->redundb == -1)
        cfgSHoundp->redundb = Cfg_GetIntValue("sections","redundb");

      if (cfgSHoundp->redundb == 1)
        return TRUE;
      if (cfgSHoundp->redundb == 0)
        return FALSE;
      else
      {
        ErrPostEx(SEV_ERROR, 0, 0, "SHoundModule: Cannot get config file entry for section %s", IsNullStr(psection));
        return FALSE;
      }
    }
    if(StringCmp(psection, "cddb") == 0)
    {
      if(cfgSHoundp->cddb == -1)
        cfgSHoundp->cddb = Cfg_GetIntValue("sections","cddb");

      if (cfgSHoundp->cddb == 1)
        return TRUE;
      if (cfgSHoundp->cddb == 0)
        return FALSE;
      else
      {
        ErrPostEx(SEV_ERROR, 0, 0, "SHoundModule: Cannot get config file entry for section %s", IsNullStr(psection));
        return FALSE;
      }
    }
    if(StringCmp(psection, "rpsdb") == 0)
    {
      if(cfgSHoundp->rpsdb == -1)
        cfgSHoundp->rpsdb = Cfg_GetIntValue("sections","rpsdb");

      if (cfgSHoundp->rpsdb == 1)
        return TRUE;
      if (cfgSHoundp->rpsdb == 0)
        return FALSE;
      else
      {
        ErrPostEx(SEV_ERROR, 0, 0, "SHoundModule: Cannot get config file entry for section %s", IsNullStr(psection));
        return FALSE;
      }
    }
    if(StringCmp(psection, "dbxref") == 0)
    {  
	if(cfgSHoundp->dbxref == -1)
	    cfgSHoundp->dbxref = Cfg_GetIntValue("sections","dbxref");

	if (cfgSHoundp->dbxref == 1)
	    return TRUE;
	if (cfgSHoundp->dbxref == 0)
	    return FALSE;
	else
	{
	    ErrPostEx(SEV_ERROR, 0, 0, "SHoundModule: Cannot get config file entry for section %s", psection);
	    return FALSE;
	}
    }

    return FALSE;
}



/*
$Log: intrez_cfg.c,v $
Revision 1.4  2004/04/08 20:12:30  mmatan
Removed the dependence on the .intrezrc variable QueryCount for determining the number of elements in the precompute index file.  Instead, the number of records in the file is determined from the file itself, in a platform independant manner.  This addresses bug #1992

Revision 1.3  2004/01/13 21:39:38  rcavero
nblast_odbc it's a draf version for nblast's ODBC port.

Revision 1.2  2004/01/06 22:32:15  skuznets
Code audit has been done sucessfully.
Possible NULL pointer issues has been eliminated.

Revision 1.1.1.2  2002/04/04 09:27:12  root
Automated import from Sourceforge.net

Revision 1.2  2002/04/03 23:49:58  kaca
added RPSDB module

Revision 1.1.1.2  2001/10/10 21:53:47  kaca
initial import

Revision 1.5  2001/10/10 13:33:23  kaca
added SeqHound modules tracking

Revision 1.4  2001/04/06 22:47:02  kaca
added new fncts

Revision 1.3  2001/03/29 16:12:27  kaca
added newline at EOF

Revision 1.2  2001/03/29 15:34:07  gbader
Added slri_misc.h include

Revision 1.1  2001/03/26 19:58:00  kaca
config file related; split off intrez.c to support uniformity between local and remote APIs

*/



