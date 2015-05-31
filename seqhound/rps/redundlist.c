/*$Id: redundlist.c,v 1.3 2004/04/14 14:37:43 mmatan Exp $*/

/*
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


  PROGRAM: Prints SeqHound REDUND table to file


  AUTHORS: Doron Betel betel@mshri.on.ca
            and Christopher W.V. Hogue hogue@mshri.on.ca


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

#include <ncbi.h>
#include <seqhound.h>
#include <intrez_db.h>

/*Retrieve a list of gi's from REDUND Seqhound Table
* and dumps it to file.
* Precondition - StRedundPtr ordinal field is set to some value
* If ordinal=1 then it get the first seq of every redund group.
****/
Boolean GetShoundRedundList(StRedundPtr head, FILE* output)
{
            StRedundPtr current=head;
            Int8 counter=0;

            if(head==NULL || output==NULL)
            {
                        ErrPostEx(SEV_FATAL, 0,0,"GetShoundRedundList: NULL parameters");
                        return FALSE;
            }
            if(SearchREDUND(&head)<=0){
                        ErrPostEx(SEV_FATAL, 0,0,"GetShoundRedundList: SearchREDUND failed");
                        return FALSE;
            }

            /*write list to file*/
            while(current!=NULL)
            {
                        fprintf(output, "%d\n", current->gi);
                        counter++;
                        current=current->next;
            }
           ErrPostEx(SEV_INFO, 0,0,"GetShoundRedundList: %d gi retrieved", counter);
           return TRUE;
}

#define NUMARGS 1
#define ORDINAL 1
Int2 Main(){

            Args myargs[NUMARGS]={
                        {"Output file of gi", NULL, NULL,NULL, FALSE, 'o', ARG_FILE_OUT, 0.0, 0, NULL},
            };

            StRedundPtr head=NULL;
            FILE* output=NULL;

            ErrSetOptFlags(EO_LOG_SEVERITY | EO_LOG_CODES | EO_LOG_FILELINE | EO_LOG_USRFILE);
            BSErrLogOpen("redundlist.log", ELOG_BANNER| ELOG_APPEND);
            ErrSetLogLevel(SEV_INFO);
            ErrSetMessageLevel(SEV_MAX);

            if(!GetArgs("redundlist", NUMARGS, myargs)) {
                        ErrPostEx(SEV_FATAL, 0,0,"Main: cannot get arguments");
                        BSErrLogClose();
                        return -1;
            }

            /*open seqhound*/
            if(!SHoundInitReadOnly(FALSE, "redundlist")){
                        ErrPostEx(SEV_FATAL, 0,0,"Main: cannot open Seqhound");
                        BSErrLogClose();
                        return -1;
            }

            /*Open output file */
            output=FileOpen(myargs[0].strvalue, "w+");

            /*create new StRpsdbPtr struct */
            head=StRedundNew();
            head->ordinal=ORDINAL;

            if(GetShoundRedundList(head, output)!=TRUE){

                        ErrPostEx(SEV_FATAL, 0,0,"Main: GetShoundRedundList failed");
                        BSErrLogClose();
            }

            /*Free structure*/
            StRedundFree(head);
            FileClose(output);
            SHoundFini();
            return 1;
}

/*
$Log: redundlist.c,v $
Revision 1.3  2004/04/14 14:37:43  mmatan
Changed SHoundInit to SHoundInitReadOnly, as write access is unnecessary and may be dangerous.

Revision 1.2  2004/01/06 22:32:14  skuznets
Code audit has been done sucessfully.
Possible NULL pointer issues has been eliminated.

Revision 1.1.1.2  2002/05/18 08:27:28  root
Automated import from Sourceforge.net

Revision 1.2  2002/05/17 15:06:10  betel
Fixed Windows MSVC build

Revision 1.1  2002/03/12 21:12:17  betel
Moved all rpsdb relevant files to rps dir

Revision 1.1  2002/02/28 20:59:40  betel
A program to collect non-redund list from REDUND table

*/
