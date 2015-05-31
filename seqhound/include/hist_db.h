/* $Id: hist_db.h,v 1.5 2005/03/14 15:53:18 eburgess Exp $
**************************************************************************
****************************************************************************/
#ifndef __HIST_CB_H__
#define __HIST_CB_H__

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
$Log: hist_db.h,v $
Revision 1.5  2005/03/14 15:53:18  eburgess
Added define for ACTION_EXTRA for tracking "extra" gis that may be associated
with an accession. Bug 5049

Revision 1.4  2004/07/22 20:56:50  zwang
Change function prototypes to take a flag which specify if a database connection should be established.

Revision 1.3  2004/06/07 21:47:11  hlieu
removed unnecessary file, for bug 2185

Revision 1.2  2003/08/22 13:34:10  haocl
added SearchHISTDB and SHoundGetAccFromRetiredGi

Revision 1.1.1.2  2002/08/16 08:29:38  root
Automated import from Sourceforge.net

Revision 1.2  2002/08/15 15:36:19  haocl
moved GetStringDate to slri_misc

Revision 1.1  2002/08/08 22:21:50  kaca
renamed from hist_cb.h

Revision 1.1  2002/08/08 18:35:37  haocl
initial import of hist files

*/

