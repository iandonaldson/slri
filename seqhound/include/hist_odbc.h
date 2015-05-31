/* $Id: hist_odbc.h,v 1.5 2005/03/14 15:53:34 eburgess Exp $
**************************************************************************
****************************************************************************/
#ifndef __HIST_ODBC_H__
#define __HIST_ODBC_H__

#include <asn.h>
#include <objgen.h>
/* #include <hist_db_p.h> */
#include <objslristruc.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ACTION_ADDED 1
#define ACTION_KILLED 2
#define ACTION_CHANGED 3
#define ACTION_EXTRA 4

/**************************************************
 *       PUBLIC CODE BASE INITIALIZTION           *
 **************************************************/
  Boolean HIST_ConnectToDB(Boolean bConnect);
  Boolean HIST_DisconnectDB(Boolean bConnect);
  static Boolean InitBindParameterforInsert(void);


  /* initializes the CODE4 structure and sets various flags.
     This function must be called in any application that uses 
     this database, prior to doing anything to the database. */
  extern void HIST_InitCodeBase(Boolean bCreate);

  /* closes the databases. This function must called at the
     end of your function, if you called LL_InitCodeBase. */
  extern void HIST_InitUndoCodeBase(void);
  
  /********************************************************
             PUBLIC OPEN DATABASE FUNCTIONS
  *********************************************************/
  /* opens all the databases associated with history.  This
     function must be called prior to accessing the database. */
  extern Boolean HIST_OpenAllDBs(Boolean CREATE);

  /* opens only HISTDB table */
  extern Boolean OpenHISTDB(Boolean CREATE);
  /********************************************************
             PUBLIC CLOSE DATABASE FUNCTIONS
  *********************************************************/
  /* closes all the databases associated with Locus Link. */
  extern void CloseAllHISTDBs(void);

  /* closes all the LL DBs and closes the CODE4 structure */
  extern void LIBCALL CloseCodeBaseAndHISTDBs(void);
  extern void LIBCALL CheckDates(void);
  extern NCBI_DatePtr LIBCALL RetrieveDate(CharPtr date);
  extern Boolean LIBCALL Extract_ACCDB(void);
  extern void LIBCALL PrintStHist(StHistdbNodePtr pHis);
  extern Boolean LIBCALL AppendRecordHISTDB(StHistdbNodePtr pac);
  StHistdbNodePtr GetAll(Int4 ACTION, NCBI_DatePtr date_from, NCBI_DatePtr date_to);

  extern Boolean SearchHISTDB(StHistdbNodePtr pHis);
#ifdef __cplusplus
}
#endif
#endif


/* 
$Log: hist_odbc.h,v $
Revision 1.5  2005/03/14 15:53:34  eburgess
Added define for ACTION_EXTRA for tracking "extra" gis that may be associated
with an accession. Bug 5049

Revision 1.4  2004/09/07 19:25:23  vgu
Remove C++ style comment

Revision 1.3  2004/07/22 20:56:49  zwang
Change function prototypes to take a flag which specify if a database connection should be established.

Revision 1.2  2004/01/14 20:47:41  zwang
Fixed wrong #ifndef

Revision 1.1  2004/01/12 21:51:29  zwang
New file for HISTDB in ODBC.

*/

