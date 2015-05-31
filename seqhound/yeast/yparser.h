#ifndef __YPARSER_H__
#define __YPARSER_H__

#include <ncbi.h>

#ifdef __cplusplus
extern "C" {
#endif

#define GOID_POS 6
#define EVID_POS 8
#define PMID_POS 10
#define ACC_POS 11
	
static CharPtr yGetToken(CharPtr pline, Int4 pos);
static void ReadYEAST_File(FILE *fp);
extern Int2 Y_ParseFile(CharPtr file);
	
#ifdef __cplusplus
}
#endif
#endif
