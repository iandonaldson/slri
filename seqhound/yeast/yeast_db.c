#include <d4all.h>
#include <yeast_db.h>
#ifdef __cplusplus
extern "C" {
#endif

extern void YEAST_InitCodeBase(void)
{
  code4init(&cb);
  cb.safety = 0;
}
static void YEAST_InitFieldnTag(void)
{
  pF4ll_go = d4field(pD4LL_GO, "NP_GO");
  pF4ll_goid = d4field(pD4LL_GO, "GO_ID");
  pF4ll_pmid = d4field(pD4LL_GO, "PMID");
  pF4ll_evicode = d4field(pD4LL_GO, "EVICODE");

  pT4ll_go = d4tag(pD4LL_GO, "NP_TAG_GO");
  pT4ll_goid = d4tag(pD4LL_GO, "GOID_TAG");
  pT4ll_pmid = d4tag(pD4LL_GO, "PMID_TAG");
  return;
}

extern Boolean OpenYEASTDB(Boolean CREATE)
{
  Char path[PATH_MAX];
  Boolean res = FALSE;
  CharPtr DBpath;
  
  GetAppParam("intrez", "datab", "path", NULL, path, (size_t)PATH_MAX * (sizeof(Char)));
  DBpath = AllocateAndCreateDBPath(path, "LL_GO");
  res = OpenDBII(DBpath, &pD4LL_GO, fieldInfo_go, tagInfo_go,
		(InitFieldFunc) YEAST_InitFieldnTag, &cb, CREATE);
 MemFree(DBpath);
 return res; 
}

extern Boolean Y_Append_LL(YEAST_STPtr ydata)
{
  /* SK: NULL ptr check */
  if ( ydata == NULL )
     return FALSE;

  printf("%d %s %d %d\n", ydata->iGOID, IsNullStr(ydata->pEviCode), ydata->iPMID, ydata->iGi);
  return TRUE;
}

#ifdef __cplusplus
}
#endif
