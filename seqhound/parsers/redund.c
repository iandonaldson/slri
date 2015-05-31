/* $Id: redund.c,v 1.7 2004/12/14 16:40:45 zwang Exp $
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
  January 1999
  makes CodeBase database (REDUND) from non-redundant Fasta database


******************************************************************************************/

#include <intrez_db.h>
#include <seqhound.h>


#define LOGING "redundlog"
#define PC_BUFFER_SIZE 16*1024*1024

#define NUMARGS 2
Args myargs[NUMARGS] = {
        {"Input non-redundant database fasta file",NULL,NULL,NULL,FALSE,'i',ARG_FILE_IN,0.0,0,NULL},
        {"Is this an update","F", NULL, NULL, TRUE, 'u', ARG_BOOLEAN, 0.0, 0, NULL}
  };


StRedundPtr prd;

/* functions */
Boolean LabelGI(CharPtr, Int4,  Int4);
Boolean AssignRedund(long int, Int4, Int4);

/* Number of rowd inserted into table. 
   Used to decide when to commit. 
   For the initial build only. */
static Int4 RowCount=0; 

Boolean AssignRedund(long int gival, Int4 set, Int4 line)
{

   ResetStREDUND(&prd);

   prd->gi = gival;
   prd->ordinal = set;
   prd->group = line;

   AppendRecordREDUND(prd);
   RowCount++;

   return TRUE;
}


Boolean LabelGI(CharPtr ppos, Int4 set,  Int4 line)
{
   Char gistr[20];
   Int2 n = 0;
   long int gival = 0;
   CharPtr pposThis =  NULL, pend = NULL;
   Char end[5];
   
   end[0] = '|';
   end[1] = ' ';
   end[2] = '\t';
   end[3] = '\n';
   end[4] = '\0';

   prd = StRedundNew();

   pend = end;
   
   if (ppos == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "LabelGI: ppos is NULL.");
      return FALSE;
    }
   
   ppos++;
   if ((*ppos == 'g') || (*ppos == 'G')) /* or G*/
      ppos++;
   else
    { 
      ErrPostEx(SEV_ERROR,0,0, "LabelGI: No \"g\".");
      return FALSE; 
    }
   if ((*ppos == 'i') ||  (*ppos == 'I')) /* or I */
      ppos++;
   else
    {
      ErrPostEx(SEV_ERROR,0,0, "LabelGI: No \"i\".");
      return FALSE;
    }
   if (*ppos == '|')
      ppos++;
   else
    {
      ErrPostEx(SEV_ERROR,0,0, "LabelGI: No \"|\".");
      return FALSE;  
    }
   
   pposThis = ppos;
   do {                           
       gistr[n] = *ppos;
       n++;
       ppos++;
     } while ((n<sizeof(gistr)-1)&&(strpbrk(pposThis, pend)) != ppos);

  gistr[n] = '\0'; 
  sscanf(gistr, "%ld", &gival);     /* make an integer out of gistr */

  if (gival != 0)
   {
    if (!AssignRedund(gival, set, line))
     {
       ErrPostEx(SEV_ERROR,0,0, "LabelGI: AssignRedund failed.", gival);
       return FALSE;
     }   
   }
  else
   {
     ErrPostEx(SEV_ERROR,0,0, "LabelGI: Integer GI is zero.");
     return FALSE;
   }
    
   return TRUE;
}




Int2 Nlm_Main(void)
{
  
   FILE *pFile;
   Char *pcBuf;
   CharPtr pcTest = NULL;
   Int4 set = 1;
   Int4 line = 0;
   Int4 last = 0;
   StRedundPtr prd = NULL;
   Boolean isUpdate = FALSE;
   RowCount = 0;
 
   if (! GetArgs("redund",NUMARGS,myargs))
	return 1;

   ErrSetOptFlags(EO_LOG_SEVERITY | EO_LOG_CODES | EO_LOG_FILELINE | EO_LOG_USRFILE);
   ErrSetLogfile(LOGING, ELOG_BANNER);
   ErrSetLogLevel(SEV_INFO);
   ErrSetMessageLevel(SEV_MAX);	

   /* SOLARIS 2.8 - have to use fopen64 to handle files larger than 2GB */
   /* if ((pFile = fopen64(myargs[0].strvalue, "r")) == NULL) */
   if ((pFile = FileOpen(myargs[0].strvalue, "r")) == NULL) 
        return 1;
   


   /* open databases and assign pointers to fields and tags */ 
   InitCodeBase();

   /* set autocommit off if this is an update */
   if (myargs[1].intvalue)
      isUpdate = TRUE;

#ifdef ODBC
   if (!StartTransaction()) {
      ErrPostEx(SEV_ERROR, 0, 0, "Cannot start transaction!");
      return 1;
   }
   
   if (isUpdate) {
     /* delete all exisiting records */
     if (!DeleteAllRecordsInREDUND()) {
        ErrPostEx(SEV_ERROR,0,0, "Cannot delete all records from redund!");
        return 1;
     } 
   }
#endif
       
   last = SHoundRecNoREDUND();
   if(last != 0)
   {
      prd = SHoundGetRecREDUND(last);
      /* SK: NULL ptr check */
      if ( prd != NULL )
      {
         line = prd->group;
         StRedundFree(prd);
      }
   }

   pcBuf = (char *) MemNew ( PC_BUFFER_SIZE );
   do {     
       pcTest = fgets(pcBuf, (size_t)PC_BUFFER_SIZE,  pFile);
       if (pcTest)
        {
           if (*pcTest == '>')
            {            
              set = 1;
              line++;
              if(!LabelGI(pcTest, set, line))
               {
                 ErrPostEx(SEV_ERROR,0,0, "LabelGI failed.");
                 if (!RollBackTransaction()) {
                    ErrPostEx(SEV_ERROR,0,0, "Cannot roll back!");
                    return 1;
                 }
                 return 1;
               } 
              set++;
              do {
                 if (*pcTest == 1) /* 1 ASCII value for control A */
                  {
                    if (*(pcTest+1) == 'g')
                     {
                        if(!LabelGI(pcTest, set, line))
                         {
                           ErrPostEx(SEV_ERROR,0,0, "LabelGI failed.");
                           if (!RollBackTransaction()) {
                              ErrPostEx(SEV_ERROR,0,0, "Cannot roll back!");
                              return 1;
                           }
                           return 1;
                         }
                     }      
                   set++;
                  }
                 pcTest++;

                 /* commit some records for initial build */
                 if (!isUpdate && RowCount>=10000) {
                    RowCount = 0;
                    if (!CommitTransaction()) {
                       ErrPostEx(SEV_ERROR, 0, 0, "Cannot commit transaction!");
                       if (!RollBackTransaction()) {
                          ErrPostEx(SEV_ERROR,0,0, "Cannot roll back!");
                          CloseCodeBase();
                          return 1;
                       }
                       CloseCodeBase();
                       return 1;
                    }
                 }

              } while((*pcTest!='\0')&&(*pcTest != '\n'));              
           }  
        }  
   } while (pcTest);  
  

#ifdef ODBC
   /* commit for the last time:
    * all changes if this is an update,
    * otherwize the last bit of insertion */
   if (!CommitTransaction()) {
      ErrPostEx(SEV_ERROR, 0, 0, "Cannot commit transaction!");
      if (!RollBackTransaction()) {
         ErrPostEx(SEV_ERROR, 0, 0, "Cannot roll back!");
         CloseCodeBase();
         return 1;
      }
      CloseCodeBase();
      return 1;
   }
#endif

   /* close all databases */
   CloseREDUND();
   CloseCodeBase();
   MemFree(pcBuf);
   MemFree(prd);

   FileClose(pFile);
   ErrPostEx(SEV_INFO,0,0, "Done.");
   return 0;
}

/*
$Log: redund.c,v $
Revision 1.7  2004/12/14 16:40:45  zwang
Took out unused CodeBase functions. Commit all at the end.

Revision 1.6  2004/10/01 17:38:37  zwang
Added ability of building a transactional database.
Added input parameter to specify if it's an update.
See bug 3441.

Revision 1.5  2004/03/09 23:25:05  hfeldman
changed reindexing of redund

Revision 1.4  2004/03/09 21:17:53  hfeldman
added functions for opening redun with no index

Revision 1.3  2004/02/17 22:07:11  skuznets
Fixed bug with lines, who can be more than 40Kb:
1. added check for limits of static buffers.
2. added check for '\0'
3. changed pcBuf from stack buffer to buffer on memory heap
   and increased buffer size from 40000 to 16Mb.

Revision 1.2  2004/01/06 22:32:13  skuznets
Code audit has been done sucessfully.
Possible NULL pointer issues has been eliminated.

Revision 1.1.1.1  2001/10/11 17:37:13  gbader
Automated import from Sourceforge.net

Revision 1.1.1.1  2001/05/25 21:36:21  kaca
initial import

Revision 1.10  2001/03/27 15:31:24  kaca
remove molecule type field from REDUND

Revision 1.9  2001/03/26 19:48:05  kaca
changes in include files

Revision 1.8  2001/02/23 20:41:16  kaca
license change

Revision 1.7  2001/02/05 20:41:33  kaca
abstracted database calls

Revision 1.6  2001/01/11 18:34:51  kaca
fixed return values

Revision 1.5  2000/10/22 18:39:45  kaca
handling files larger than 2GB

Revision 1.4  2000/10/13 21:52:18  kaca
changed includes

Revision 1.3  2000/09/20 18:54:36  kaca
added newline at EOF

Revision 1.2  2000/09/20 18:49:59  kaca
split SeqHound headers into private and public one and changed includes

Revision 1.1.1.1  2000/09/18 17:41:56  kaca
Initial import

Revision 1.1.1.1  2000/07/06 20:02:32  kaca
Initial import

*/
