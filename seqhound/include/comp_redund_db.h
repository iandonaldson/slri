/* $Id: comp_redund_db.h,v 1.1.1.3 2002/09/26 08:33:27 root Exp $
**************************************************************************
****************************************************************************/
#ifndef __REDUND_DB_H__
#define __REDUND_DB_H__

#include <asn.h>
#include <objgen.h>
#include <comp_redund_db_p.h>
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
  extern Boolean LIBCALL REDUND_OpenAllDBs(Boolean CREATE, RedundDBPtr pRed, CharPtr path_old, CharPtr path_new);

  /********************************************************
             PUBLIC CLOSE DATABASE FUNCTIONS
  *********************************************************/
  /* closes all the LL DBs and closes the CODE4 structure */
  extern void LIBCALL CloseCodeBaseAndDBs(RedundDBPtr pRed);

  /* this does the job of comparing the REDUNDANT tables */
  extern Boolean LIBCALL RedundantCheck(RedundDBPtr pRed, RedundDBPtr pRedCheck, FILE *fp);
#ifdef __cplusplus
}
#endif
#endif


/* 
$Log: comp_redund_db.h,v $
Revision 1.1.1.3  2002/09/26 08:33:27  root
Automated import from Sourceforge.net

Revision 1.4  2002/09/25 18:36:24  kaca
changed function prototype

Revision 1.3  2002/08/22 18:34:01  haocl
REDUND_OpenAllDBS now takes path_old and path_new

Revision 1.2  2002/08/22 17:36:55  kaca
changed include from redund_db_p.h comp_redund_db_p.h

Revision 1.1  2002/08/21 17:43:37  haocl
renamed files from redund* to comp_redund*

Revision 1.3  2002/08/19 17:39:19  haocl
modified to take path to redund_old/redund_new from command line

Revision 1.2  2002/08/19 15:18:01  haocl
renaming changes

Revision 1.1  2002/08/19 15:15:21  haocl
initial import of redund_db files
*/

