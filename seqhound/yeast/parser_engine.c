#ifdef __cplusplus
extern "C" {
#endif
#include <ncbi.h>
#include <yparser.h>

#define LOGFILE "yparserlog"
#define NUMARGS 1

#ifndef IsNullStr
#define IsNullStr(x)	(((x)==NULL)?"":(x))
#endif
	
Args myargs[NUMARGS] = {
   {"Input file", "orf_geneontology.all.tab", NULL, NULL, TRUE, 'i', ARG_FILE_IN, 0.0, 0, NULL}
};
Int2 Main(void){

  ErrSetOptFlags(EO_LOG_SEVERITY | EO_LOG_CODES | EO_LOG_FILELINE | EO_LOG_USRFILE);
  ErrSetLogfile(LOGFILE, ELOG_BANNER);
  ErrSetLogLevel(SEV_INFO);
  ErrSetMessageLevel(SEV_MAX);

  if(! GetArgs("engine", NUMARGS, myargs)) return 0;
  if(FileLength(myargs[0].strvalue) == 0)
  {
    ErrPostEx(SEV_ERROR, 0, 0, "Main: Cannot find %s file.", IsNullStr(myargs[0].strvalue));
    return -1;
  }
  if(Y_ParseFile(myargs[0].strvalue) == -1){
    ErrPostEx(SEV_ERROR, 0, 0, "[YEAST_ANN]: Parse Error");
    return -1;
  }
  ErrPostEx(SEV_INFO, 0, 0, "Main: Done!");
  return 0;
}
	
#ifdef __cplusplus
}
#endif
	
