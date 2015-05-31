/* $Id: db2redund.c,v 1.2 2004/01/06 22:32:11 skuznets Exp $
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


  PROGRAM:  Preprocessor for redund for use by DB2 Autoloader

  AUTHORS:  Marc Dumontier (mdumontier@mdsp.com)

  REVISION/CONTRIBUTION NOTES:
         
  March 1, 2001  Original MSH Revision.
                Hogue Lab - University of Toronto Biochemistry
                Department and the Samuel Lunenfeld Research
                Institute, Mount Sinai Hospital
                http://bioinfo.mshri.on.ca  hogue@mshri.on.ca
                ftp://ftp.mshri.on.ca/
  
  
  PUBLICATION TO CITE:
  

  
***************************************************************************
*/

#include <intrez_db.h>
#include <seqhound.h>


#define LOGING "db2redund.log"

#define NUMARGS 2
Args myargs[NUMARGS] = {
        {"Input non-redundant database fasta file",NULL,NULL,NULL,FALSE,'i',ARG_FILE_IN,0.0,0,NULL},
        {"Output File",NULL,NULL,NULL,FALSE,'o',ARG_FILE_OUT,0.0,0,NULL}
  };


StRedundPtr prd;

/* functions */
Boolean LabelGI(CharPtr, Int4,  Int4);
Boolean AssignRedund(long int, Int4, Int4);

FILE* pFileOut=NULL;

Boolean AssignRedund(long int gival, Int4 set, Int4 line)
{
   fprintf(pFileOut,"%ld,%ld,%ld\n",gival,set,line);
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

   /* SK: NULL ptr check */
   if ( prd == NULL )
   {
      ErrPostEx(SEV_ERROR,0,0, "LabelGI: prd is NULL.");
      return FALSE;
   }

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
     } while ((strpbrk(pposThis, pend)) != ppos);

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
   Char pcBuf[40000];
   CharPtr pcTest = NULL;
   Int4 set = 1;
   Int4 line = 0;
   Int4 last = 0;
   StRedundPtr prd = NULL;
    
   if (! GetArgs("predund",NUMARGS,myargs))
	return 1;

   ErrSetOptFlags(EO_LOG_SEVERITY | EO_LOG_CODES | EO_LOG_FILELINE | EO_LOG_USRFILE);
   ErrSetLogfile(LOGING, ELOG_BANNER);
   ErrSetLogLevel(SEV_INFO);
   ErrSetMessageLevel(SEV_MAX);	

   if ((pFile = FileOpen(myargs[0].strvalue, "r")) == NULL) 
        return 1;

   if ((pFileOut = FileOpen(myargs[1].strvalue, "w")) == NULL) 
        return 1;
   
   do {     
       pcTest = fgets(pcBuf, (size_t)40000,  pFile);
       if (pcTest)
        {
           if (*pcTest == '>')
            {            
              set = 1;
              line++;
              if(!LabelGI(pcTest, set, line))
               {
                 ErrPostEx(SEV_ERROR,0,0, "LabelGI failed.");
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
                           return 1;
                         }
                     }      
                   set++;
                  }
                 pcTest++;
              } while(*pcTest != '\n');              
           }  
        }  
   } while (pcTest);  
  
   MemFree(prd);
   FileClose(pFile);
   FileClose(pFileOut);
   ErrPostEx(SEV_INFO,0,0, "Done.");
   return 0;
}

