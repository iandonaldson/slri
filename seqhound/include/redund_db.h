/* $Id: redund_db.h,v 1.1.1.1 2002/08/20 08:29:52 root Exp $
**************************************************************************
****************************************************************************/
#ifndef __REDUND_DB_H__
#define __REDUND_DB_H__

#include <asn.h>
#include <objgen.h>
#include <redund_db_p.h>
#include <objslristruc.h>

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************
 *       PUBLIC CODE BASE INITIALIZTION           *
 **************************************************/

  /* initializes the CODE4 structure and sets various flags.
     This function must be called in any application that uses 
     this database, prior to doing anything to the database. */
  extern void REDUND_InitCodeBase(void);

  /* closes the databases. This function must called at the
     end of your function, if you called LL_InitCodeBase. */
  extern void REDUND_InitUndoCodeBase(void);
  
  /********************************************************
             PUBLIC OPEN DATABASE FUNCTIONS
  *********************************************************/
  /* opens all the databases associated with Locus Link.  This
     function must be called prior to accessing the database. */
  extern Boolean LIBCALL REDUND_OpenAllDBs(Boolean CREATE, RedundDBPtr pRed, char *path[]);

  /********************************************************
             PUBLIC CLOSE DATABASE FUNCTIONS
  *********************************************************/
  /* closes all the LL DBs and closes the CODE4 structure */
  extern void LIBCALL CloseCodeBaseAndDBs(RedundDBPtr pRed);

  extern Boolean LIBCALL RedundantCheck(RedundDBPtr pRed, FILE *fp);
  extern CharPtr LIBCALL getstringdate(NCBI_DatePtr date);
#ifdef __cplusplus
}
#endif
#endif


/* 
$Log: redund_db.h,v $
Revision 1.1.1.1  2002/08/20 08:29:52  root
Automated import from Sourceforge.net

Revision 1.3  2002/08/19 17:39:19  haocl
modified to take path to redund_old/redund_new from command line

Revision 1.2  2002/08/19 15:18:01  haocl
renaming changes

Revision 1.1  2002/08/19 15:15:21  haocl
initial import of redund_db files
*/

