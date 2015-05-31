#ifdef __cplusplus
extern "C" {
#endif

#include <yparser.h>
#include <yeast_db.h>
#include <seqhoundapi.h>
/* returns the token at position pos in the line pline
 * from a flat file based on tab delimiters */
static CharPtr yGetToken(CharPtr pline, Int4 pos)
{
  Char pcopy[StringLen(pline) + 1];
  CharPtr p = NULL, q = NULL;
  CharPtr goid;
  Int2 i;
  StringCpy(pcopy, pline);

  p = StringChr(pcopy, '\t');
  /* search once above and once below loop, so 
   * only iterate pos - 2 times in loop */
  for( i = 0; i < (pos - 2); i++){
    p = StringChr(p+1, '\t');
  }
  /* token could be last in line, need to consider '\0' */
  q = StringTok(p+1, "\t\0");
  
  if( q == NULL){return NULL;}
  
  i = StringLen(q);
  goid = MemNew((i + 1)*sizeof(Char));
  StringCpy(goid, q); 
  return goid;
}

static void ReadYEAST_File(FILE *fp)
{
  ByteStorePtr bsp = BSNew(0L);
  Int4 i = 0;
  CharPtr pline = NULL;
  YEAST_ST yst = {0, 0, 0, NULL};
  CharPtr acc = NULL;
  CharPtr p = NULL;  
 
  while( (i = BSGetLineFromFile(bsp, fp)) != EOF ){
    if(feof(fp)) break;
    /* get the string */
    pline = BSStringSave(bsp);
    /* printf("%s", IsNullStr(pline)); */
    if( (p = yGetToken(pline, GOID_POS)) != NULL ){
      yst.iGOID = atoi(p); MemFree(p);
    }
    if( (p = yGetToken(pline, EVID_POS)) != NULL){
      yst.pEviCode = MemNew((strlen(p)+1) * sizeof(Char));
      StringCpy(yst.pEviCode, p); MemFree(p);
    }
    if( (p = yGetToken(pline, PMID_POS)) != NULL){
      yst.iPMID = atoi(p); MemFree(p);
    }
    if( (p = yGetToken(pline, ACC_POS)) != NULL){
      acc = MemNew((strlen(p)+1) * sizeof(Char));
      StringCpy(acc, p); MemFree(p);
    }
    Y_Append_LL(&yst);
    //printf("%d %s %d %s\n", yst.iGOID, IsNullStr(yst.pEviCode), yst.iPMID, IsNullStr(acc)); 
    pline = MemFree(pline);
  }
}

extern Int2 Y_ParseFile(CharPtr file)
{
  FILE *fp;

  fp = FileOpen(file, "r");
  if(fp == NULL){
     ErrPostEx(SEV_FATAL, 0, 0, "[YEAST_ANN] File not found");
     return -1;
  }
  if(!SHoundInit(TRUE, "engine")){
     ErrPostEx(SEV_FATAL, 0, 0, "[YEAST_ANN]: Cound not initialize SeqHound");
     return -1;
  }
  /* grab the necessary informaton, look up GI on seqhound */
  /* store into database file */
  YEAST_InitCodeBase();
  OpenYEASTDB(FALSE);
  ReadYEAST_File(fp);
  FileClose(fp);
  SHoundFini();
  return 0;
}

#ifdef __cplusplus
}
#endif
