/* $Id: csan.h,v 1.1.1.2 2002/03/25 09:27:20 root Exp $
* ===========================================================================
*
*                         COPYRIGHT NOTICE
*                Samuel Lunenfeld Research Institute
*
*  This software/database is copyrighted.
*
*  Although all reasonable efforts have been taken to ensure the accuracy
*  and reliability of the software and data, the Samuel Lunenfeld
*  Research Institute does not and cannot warrant the performance or
*  results that may be obtained by using this software or data. The
*  The Samuel Lunenfeld Research Institutes disclaims all warranties, express
*  or implied, including warranties of performance, merchantability or fitness
*  for any particular purpose.
*
* ===========================================================================
*
* File Name:  slapamol.c
*
* Author:  Christopher Hogue
*
* Version Creation Date:   04/23/98
*
* $Revision: 1.1.1.2 $
*
* File Description: Clustal-style Masked Structure Alignments
*
* Modifications:
* --------------------------------------------------------------------------
* Date     Name        Description of modification
* -------  ----------  -----------------------------------------------------
*
*
*
* $Log: csan.h,v $
* Revision 1.1.1.2  2002/03/25 09:27:20  root
* Automated import from Sourceforge.net
*
* Revision 1.2  2002/03/25 06:57:33  micheld
* minor change
*
* Revision 1.1.1.1  2001/05/25 21:36:19  kaca
* initial import
*
* Revision 1.1.1.1  2001/05/25 20:43:00  kaca
* code examples for SeqHound
*
* Revision 1.3  2001/05/10 18:14:47  kaca
* removed writefastafromgi function
*
* Revision 1.2  2001/03/26 22:30:07  kaca
* removed old include
*
* Revision 1.1.1.1  2001/02/23 17:54:40  kaca
* moved out of Source Forge  part of SeqHound
*
* Revision 1.1.1.1  2000/06/09 18:14:05  feldman
* TraDES project
*
*
* ==========================================================================
*/


#ifndef _CSANFUNC_
#define _CSANFUNC_

#include <ncbi.h>
#include <mmdbapi.h>
#include <seqport.h>
#include <objslristruc.h>
#include <intrez.h>
#include <intrez_misc.h>


#ifdef __cplusplus
extern "C" {
#endif


typedef struct csanode {
   struct csanode PNTR next;
   CharPtr pcSeqName;
   Int4    iGi;
   Int4    iMMDB;
   ValNodePtr pvnSeqAlnPart;
   CharPtr pcSeqAln;
   Int4    iLen;
   PDNMS   pdnmsStructure;
   Boolean bStrucMask; /*  TRUE if this is just the sec-structure mask */
   Boolean bPosGapPen; /* TRUE if this is postition specific gap penalties */
} CSAN, *PCSAN;




PCSAN LIBCALL NewCSAN(void);
void FreeCSAN(PCSAN pcsanThis);
Int4 LIBCALL WriteCSAN(PCSAN pcsanThis, FILE *pFile);
PCSAN LIBCALL LoadCSAN(CharPtr pcFile);
static PMMD MMDBGetMoleculeByGI(PDNMS pdnmsThis, Int4 iGi);
static Int4  FillCSANWithStru(PCSAN pcsanThis, PMMD pmmdThis, Int4 iLen);
static Int4 FillCSANWithMask(PCSAN pcsanThis, PMMD pmmdThis, Int4 iLen);
Int4 MMDBidfromGI(Int4 iGi);
PCSAN CSANStructMask(Int4 iMMDBid, Int4 iGiStruc);

#ifdef __cplusplus
}
#endif

#endif /*_CSANFUNC_*/

/*
$Log: csan.h,v $
Revision 1.1.1.2  2002/03/25 09:27:20  root
Automated import from Sourceforge.net

Revision 1.2  2002/03/25 06:57:33  micheld
minor change

Revision 1.1.1.1  2001/05/25 21:36:19  kaca
initial import

Revision 1.1.1.1  2001/05/25 20:43:00  kaca
code examples for SeqHound

Revision 1.3  2001/05/10 18:14:47  kaca
removed writefastafromgi function

Revision 1.2  2001/03/26 22:30:07  kaca
removed old include

Revision 1.1.1.1  2001/02/23 17:54:40  kaca
moved out of Source Forge  part of SeqHound

*/













