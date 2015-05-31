/* $Id: parseasn.h,v 1.5 2005/03/15 21:13:56 eburgess Exp $
*******************************************************************************************
  Copyright (C) 2000 Mount Sinai Hospital (MSH)

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of
  the License, or any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

  See the GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the
         Free Software Foundation, Inc.,
         59 Temple Place, Suite 330, Boston, MA
         02111-1307  USA
  or visit http://www.gnu.org/copyleft/gpl.html

  SPECIAL EXCEPTIONS

  As a special exception, Mount Sinai Hospital gives permission to
  link this program with the following non-GPL programs or libraries,
  and distribute the resulting executable, without including the source
  code for these in the source distribution:

  a) CodeBase 6.5 or greater from Sequiter Software Inc.

  b) The C or C++ interface to Oracle 8.x or greater, from
     Oracle Corporation or IBM DB2 UDB.


  PROGRAM:


  AUTHORS:
           Elizabeth Burgess and Christopher W.V. Hogue hogue@mshri.on.ca


  REVISION/CONTRIBUTION NOTES:


  PUBLICATION TO CITE:


  DESCRIPTION
  October 19, 2004
  header file for SeqHound database parsers and updkldge.c,

******************************************************************************************/

#ifndef _PARSEASN_
#define _PARSEASN_

#include <intrez_db.h>
#include <seqhound.h>

/*====================================================================
  Functions used to parse data from bioseqs. Used by mother.c and 
  postcomgen.c in genomes.
  ====================================================================*/
#define DELTA_SEQ 4
#define SEQ_LOC   1

#define CHAIN_NOT_SET 0
#define CHAIN_DEFAULT 32

void GetGisFromSeqAnnot( SeqAnnotPtr pSeqAnnot, 
						 ValNodePtr *ppvnGis);
void GetGisFromSeqFeat( SeqFeatPtr pSeqFeat, ValNodePtr *pvnGis );
void GetGisFromSeqLoc( SeqLocPtr pSeqLoc, ValNodePtr *pvnGis,
					   Int4 seqlocType );
Int4 GetGiFromProductLocation( SeqLocPtr psl );
Int4 GetGiFromSeqId( SeqIdPtr psid );
Int4 GetGiFromUserObject( UserObjectPtr pExt );

CharPtr GetDbFromSeqId( SeqIdPtr pSeqId, Boolean refseq);
Boolean IsTextSeqId( SeqIdPtr pSeqId ); 

StAccdbPtr LIBCALL GetInfoFromSeqId( SeqIdPtr pSeqId, 
									 Boolean refseq);

void FillGiChromid( ValNodePtr pvnGis, Int4 contiggi,
					Int4 chromid );
void ReplaceGiChromid( ValNodePtr pvnGis, Int4 contiggi,
					   Int4 chromId );

ValNodePtr ParseContigs( ValNodePtr pSeq ); 
						 
#endif /* ifdef _PARSEASN_ */

/*
  $Log: parseasn.h,v $
  Revision 1.5  2005/03/15 21:13:56  eburgess
  Modified prototypes of GetDbFromSeqId() and GetInfoFromSeqId()to handle the
  refseq case.

  Revision 1.4  2005/03/14 15:57:55  eburgess
  Added GetDbFromSeqid(), IsTextSeqId from mother and added GetInfoFromSeqId()
  to handle updates better. Bug 4939 and 5049.

  Revision 1.3  2005/02/01 16:43:07  eburgess
  Changed gikloodge and contigkloodge to gichromid and contigchromid.

  Revision 1.2  2005/01/10 19:24:42  eburgess
  Added ReplaceGikloodge() for comgen module.

  Revision 1.1  2004/11/08 19:53:33  eburgess
  Initial revision.
 */
