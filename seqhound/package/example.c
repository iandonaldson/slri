#include <seqhound.h>
#include <stdio.h>

int main ( int argc, char *argv[])
{
  int gi = 0;
  char *acc="X88853";

  

  
  /*Start logging */
   ErrSetOptFlags(EO_LOG_SEVERITY | EO_LOG_CODES | EO_LOG_FILELINE | EO_LOG_USRFILE);
   BSErrLogOpen("seqhound_example_logs", ELOG_BANNER | ELOG_APPEND);
   /*ErrSetLogfile("seqrem_logs", ELOG_BANNER | ELOG_APPEND );*/
   ErrSetLogLevel(SEV_INFO);
   ErrSetMessageLevel(SEV_MAX);

   printf("Calling SeqHoundInit\n");   
   if(!SHoundInitReadOnly(FALSE, "seqhoundExample"))
   {
       ErrPostEx(SEV_ERROR,0,0, "SHoundInit failed.<br>");
       BSErrLogClose();
       return 1;
   }
   printf("Calling SeqHoundFindAcc\n");
   gi = SHoundFindAcc(acc);
   if (gi > 0)
   {
     printf("SHoundFindAcc OK: gi=%d\n", gi);

   }   
   else
   {
     printf("SHoundFindAcc Failed: gi=%d\n", gi);

   }
   
   
   SHoundFini();
   BSErrLogClose();

}
