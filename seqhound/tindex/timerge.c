/* $Id: timerge.c,v 1.2 2004/01/06 22:32:18 skuznets Exp $
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

******************************************************************************************/
#include <slri_cb.h>
#include <netdb.h>
#include <slri_tim.h>
#include <slri_misc.h>

#define LOGING "timergelog"

#define NUMARGS 5
Args myargs[NUMARGS] = {
	    {"List of databases",NULL,NULL,NULL,FALSE,'i',ARG_STRING,0.0,0,NULL},
	    {"Name of output database","TI",NULL,NULL,TRUE,'d',ARG_STRING,0.0,0,NULL},
     {"Delete input tables","F", NULL, NULL, TRUE, 'r', ARG_BOOLEAN, 0.0, 0, NULL},
     {"Make temporary file for cluster implementation","F", NULL, NULL, TRUE, 't', ARG_BOOLEAN, 0.0, 0, NULL},
     {"Path to temporary file","./",NULL,NULL,TRUE,'p',ARG_STRING,0.0,0,NULL},
        };


static void DeleteIntermediateFiles(ValNodePtr pvntempo)
{
      CharPtr pcommand = NULL;
      /* SK: */
      size_t pcommandLen = 0;

      printf("Deleting.\n");
      while(pvntempo != NULL)
      {
         /* SK: */
	 pcommandLen = (size_t)(StringLen(pvntempo->data.ptrvalue)+20)*sizeof(Char);
         pcommand = MemNew(pcommandLen);
	 /* SK: */
         snprintf(pcommand, pcommandLen, "rm -f %s*", IsNullStr(pvntempo->data.ptrvalue));
         system(pcommand);
         pcommand = MemFree(pcommand);
         pvntempo = pvntempo->next;
      }
}

/* makes temporary file containing hostname and final table */
/* the "pseudo-unique" file name consists of hostname and final table concatenated together */
static void MakeTempFile(CharPtr path, CharPtr ptable)
{
      FILE* pfile = NULL;
      char* phost = NULL;
      Char pfname[PATH_MAX];
      struct hostent *hostinfo=NULL;
      char* ip = NULL;
      /* SK: Modified for IsNullStr */
      char* nameptr = NULL;

      phost = MemNew((size_t)80*sizeof(Char));
      gethostname(phost,(size_t) 80);
      if(!phost)
         ErrPostEx(SEV_ERROR, 0, 0, "Unable to get hostname.");
      hostinfo = (struct hostent *) gethostbyname(phost);
      if(!hostinfo)
         ErrPostEx(SEV_ERROR, 0, 0, "Unable to get hostinfo.");
      ip = *(hostinfo->h_addr_list);

      /* SK: */
      snprintf(pfname, PATH_MAX, "%s/%s%s",IsNullStr(path), IsNullStr(phost), IsNullStr(ptable));
      pfile = FileOpen(pfname, "w");
      /* SK: Modified for IsNullStr */
      nameptr = inet_ntoa(*(struct in_addr*)ip);
      fprintf(pfile, "%s\t%s\n",IsNullStr(nameptr), IsNullStr(ptable));
      FileClose(pfile);
      MemFree(phost);
      MemFree(ip);
}

/* allocates memory for return string */
CharPtr CreateTIDatabPath(CharPtr basename, Int2 tablecount)
{
   Char dbnam[PATH_MAX];
   Char pathdatab[PATH_MAX];

   /* SK: */
   if(tablecount != 0)
      snprintf(dbnam, PATH_MAX, "%sTI%d", IsNullStr(basename), tablecount);
   else
      snprintf(dbnam, PATH_MAX, "%s", IsNullStr(basename));

   pathdatab[0]='\0';
   if (GetAppParam("tindex", "datab", "path", NULL, pathdatab, (size_t)PATH_MAX * (sizeof(Char))) == 0)
   {
      /* no config just use the current dir */
      ErrPostEx(SEV_ERROR, 0, 0, "Failed to read path from config file .tindexrc.");
      return StringSave(dbnam);
   }
   else
      return (AllocateAndCreateDBPath(pathdatab,dbnam));
}

ValNodePtr AddTIPaths(ValNodePtr pvnnames)
{
  Char pathdatab[PATH_MAX];
  CharPtr ppath = NULL;
  ValNodePtr pvnfin = NULL;

  if (GetAppParam("tindex", "datab", "path", NULL, pathdatab, (size_t)PATH_MAX * (sizeof(Char))) == 0)
  {
    while(pvnnames != NULL)
    {
       ValNodeCopyStr(&pvnfin, 0, pvnnames->data.ptrvalue);
       pvnnames = pvnnames->next;
    }
    return pvnfin;
  }

  while(pvnnames != NULL)
  {
     ppath = AllocateAndCreateDBPath(pathdatab,pvnnames->data.ptrvalue);
     if(ppath != NULL)
        ValNodeAddStr(&pvnfin, 0, ppath);
     pvnnames = pvnnames->next;
  }
  return pvnfin;
}


Int2 Main()
{

   time_t tim1,tim2;
   ValNodePtr pvnnames = NULL, pvntemp = NULL, pvnfin = NULL;
   CharPtr dbnam = NULL;

   ErrSetOptFlags(EO_LOG_SEVERITY | EO_LOG_CODES | EO_LOG_FILELINE | EO_LOG_USRFILE);
   ErrSetLogfile(LOGING, ELOG_BANNER);
   ErrSetLogLevel(SEV_INFO);
   ErrSetMessageLevel(SEV_MAX);

   tim1=GetSecs();
   /* check command line arguments */
   if ( ! GetArgs("timerge",NUMARGS, myargs))
          return 1;


   pvnnames = Misc_CommaStrString2ValNodeList(myargs[0].strvalue, ",");
   /* prepend config path to every name */
   pvnfin = AddTIPaths(pvnnames);
   if(Misc_CountVnp(pvnfin) <= 1)
   {
       printf("Not enough databases to merge.\n");
       ErrPostEx(SEV_ERROR,0,0,"Main: Not enough databases to merge.");
       return 0;
   }

   printf("Merging.\n");
   pvntemp = pvnfin;
   while(pvntemp != NULL)
   {
     printf("%s\n", IsNullStr(pvntemp->data.ptrvalue));
     pvntemp = pvntemp->next;
   }

   /* merge databases */
   dbnam = CreateTIDatabPath(myargs[1].strvalue, 0);
   if(TI_MergeAllDb(dbnam, pvnfin) != 0)
   {
       ErrPostEx(SEV_ERROR,0,0,"Main: Call to TI_MergeAllDb failed.");
       return 0;
   }
   MemFree(dbnam);

   /* delete all input tables if needed */
   if(myargs[2].intvalue)
     DeleteIntermediateFiles(pvnfin);

   /* make a temp file if needed*/
   if(myargs[3].intvalue)
     MakeTempFile(myargs[4].strvalue, myargs[1].strvalue);

   ValNodeFreeData(pvnfin);
   ValNodeFreeData(pvnnames);

   tim2=GetSecs();
   printf("Done. Elapsed time merging %ld min.\n",(tim2-tim1)/60);
   ErrPostEx(SEV_WARNING,11,1,"Done. Elapsed time merging %ld min.",(tim2-tim1)/60);

   return 0;

}

/*
$Log: timerge.c,v $
Revision 1.2  2004/01/06 22:32:18  skuznets
Code audit has been done sucessfully.
Possible NULL pointer issues has been eliminated.

Revision 1.1.1.3  2001/11/29 09:22:30  root
Automated import from Sourceforge.net

Revision 1.5  2001/11/28 23:14:54  kaca
fixed valnode freeing second try

Revision 1.3  2001/11/22 22:34:38  kaca
added log file functionality

Revision 1.2  2001/11/19 23:17:36  kaca
change to arguments

Revision 1.1  2001/11/19 22:38:23  kaca
initial import

*/




