 /* $Id: slri_post.h,v 1.1.1.1 2001/10/11 17:37:13 gbader Exp $
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
           taken from NCBI's cdentrez.c


  REVISION/CONTRIBUTION NOTES:

  June 1, 2000  Original MSH Revision.
                Hogue Lab - University of Toronto Biochemistry
                Department and the Samuel Lunenfeld Research
                Institute, Mount Sinai Hospital
                http://bioinfo.mshri.on.ca  hogue@mshri.on.ca
                ftp://ftp.mshri.on.ca/


  PUBLICATION TO CITE:


  DESCRIPTION



******************************************************************************************/

#ifndef _SLRI_POST_
#define _SLRI_POST_

#include <asn.h>
#include <ncbi.h>
#include <mmdblocl.h>

#ifdef __cplusplus
extern "C" { /* } */
#endif

#ifndef DocUid
#define DocUid Int4
#endif


typedef struct posting {
  ByteStorePtr  uids;
  DocUidPtr     buffer;
  Int4          bufsize;
  Int4          index;
  Int4          count;
} Posting, PNTR PostingPtr;

PostingPtr LIBCALL NewPost PROTO((ByteStorePtr lst, Int4 defsize));
PostingPtr LIBCALL  FreePost PROTO((PostingPtr pst));
Int4 LIBCALL PostLength PROTO((PostingPtr pst));
void LIBCALL RewindPost PROTO((PostingPtr pst));
DocUid LIBCALL ReadItem PROTO((PostingPtr pst));
void LIBCALL WriteItem PROTO((PostingPtr pst, DocUid value));
void LIBCALL FlushItems PROTO((PostingPtr pst));

typedef struct{
     Int4 num;
     DocUidPtr uids;
     Int4Ptr weights;
 } PNTR LocalLinkSetPtr;

LocalLinkSetPtr LIBCALL LocalLinkSetNew(void);
LocalLinkSetPtr LIBCALL LocalLinkSetFree(LocalLinkSetPtr lsp);
LocalLinkSetPtr LIBCALL BSPtoLSP(ByteStorePtr bsp);

#ifdef __cplusplus
/* { */ }
#endif



#endif  /* _INTREZ_CB */


/*
 $Log: slri_post.h,v $
 Revision 1.1.1.1  2001/10/11 17:37:13  gbader
 Automated import from Sourceforge.net

 Revision 1.1.1.1  2001/05/25 21:36:20  kaca
 initial import

 Revision 1.2  2001/04/09 15:52:53  kaca
 added newline at EOF

 Revision 1.1  2001/04/06 22:43:42  kaca
 Posting and LinkSet declarations

 Revision 1.1  2001/04/03 19:20:11  kaca
 CodeBase globals; needed for an un-abstracted legacy code

*/

