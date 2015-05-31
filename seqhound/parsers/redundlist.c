/*$Id*/

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
            if(!SHoundInit(FALSE, "rpsdb")){
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
Revision 1.1.1.1  2002/03/01 09:25:27  root
Automated import from Sourceforge.net

Revision 1.1  2002/02/28 20:59:40  betel
A program to collect non-redund list from REDUND table

*/
