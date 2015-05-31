/* $Id: slri_post.c,v 1.1.1.1 2001/10/11 17:37:13 gbader Exp $
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

#include<ncbi.h>
#include<slri_post.h>


/*****************************************************************************
*
*   Low level functions to manipulate postings lists.
*
*****************************************************************************/

PostingPtr LIBCALL  NewPost (ByteStorePtr lst, Int4 defsize)

{
  PostingPtr  pst;

  pst = NULL;
  if (lst != NULL) {
    pst = MemNew (sizeof (Posting));
    if (pst != NULL) {
      pst->uids = lst;
      pst->buffer = NULL;
      if (defsize == 0) {
        pst->bufsize = (Int4) MIN (16384L, BSLen (lst));
      } else {
        pst->bufsize = (Int4) MIN (16384L, defsize);
      }
      pst->count = 0;
      pst->index = 0;
    }
  }
  return pst;
}

PostingPtr LIBCALL  FreePost (PostingPtr pst)

{
  if (pst != NULL) {
    if (pst->uids != NULL) {
      BSFree (pst->uids);
    }
    if (pst->buffer != NULL) {
      MemFree (pst->buffer);
    }
    MemFree (pst);
  }
  return NULL;
}

Int4 LIBCALL  PostLength (PostingPtr pst)

{
  Int4  k;

  k = 0;
  if (pst != NULL) {
    k = (Int4) (BSLen (pst->uids) / (Int4) sizeof (DocUid));
  }
  return k;
}

void LIBCALL  RewindPost (PostingPtr pst)

{
  if (pst != NULL) {
    if (pst->uids != NULL) {
      BSSeek (pst->uids, 0L, 0);
    }
    pst->count = 0;
    pst->index = 0;
  }
}

DocUid LIBCALL  ReadItem (PostingPtr pst)

{
  DocUid  rsult;

  rsult = INT4_MAX;
  if (pst != NULL && pst->uids != NULL) {
    if (pst->buffer == NULL) {
      pst->buffer = MemNew ((size_t) pst->bufsize);
      pst->count = 0;
      pst->index = 0;
    }
    if (pst->count <= 0) {
      pst->count = (Int4) BSRead (pst->uids, pst->buffer, pst->bufsize);
      pst->index = 0;
    }
    if (pst->count > 0) {
      rsult = pst->buffer [pst->index];
      (pst->index)++;
      (pst->count) -= sizeof (DocUid);
    }
  }
  return rsult;
}

void LIBCALL  WriteItem (PostingPtr pst, DocUid value)

{
  if (pst != NULL && pst->uids != NULL) {
    if (pst->buffer == NULL) {
      pst->buffer = MemNew ((size_t) pst->bufsize);
      pst->count = 0;
      pst->index = 0;
    }
    pst->buffer [pst->index] = value;
    (pst->index)++;
    (pst->count) += sizeof (DocUid);
    if (pst->count >= pst->bufsize) {
      BSWrite (pst->uids, pst->buffer, pst->count);
      pst->count = 0;
      pst->index = 0;
    }
  }
}

void LIBCALL  FlushItems (PostingPtr pst)

{
  if (pst != NULL && pst->uids != NULL && pst->buffer != NULL) {
    BSWrite (pst->uids, pst->buffer, pst->count);
    if (pst->buffer != NULL) {
      pst->buffer = MemFree (pst->buffer);
    }
    pst->count = 0;
    pst->index = 0;
  }
}


LocalLinkSetPtr LIBCALL LocalLinkSetNew(void)
{
    LocalLinkSetPtr lsp;

    lsp = MemNew(sizeof(*lsp));
    lsp->num = 0;
    lsp->uids = NULL;
    lsp->weights = NULL;

    return lsp;
}

LocalLinkSetPtr LIBCALL LocalLinkSetFree(LocalLinkSetPtr lsp)
{
    MemFree(lsp->uids);
    MemFree(lsp->weights);
    MemFree(lsp);

    return NULL;
}

LocalLinkSetPtr LIBCALL BSPtoLSP(ByteStorePtr bsp)
 {
	LocalLinkSetPtr lsp;

     if (bsp == NULL)
     {
         ErrPostEx(SEV_ERROR,0,0, "Invalid parameter.");
         return NULL;
     }
     if(BSLen(bsp) == 0)
     {
         ErrPostEx(SEV_ERROR,0,0, "Byte store contains no data.");
         return NULL;
     }
     lsp = LocalLinkSetNew();
	 if(lsp == NULL)
	 {
	   ErrPostEx(SEV_ERROR,0,0, "LocalLinkSetNew failed.");
	   return NULL;
	
	 }	
	
     lsp->num = BSLen(bsp) / sizeof(DocUid);
     if ((lsp->uids = MemNew(BSLen(bsp))) == NULL)
     {
         /* platforms which can't allocate this are out of luck */
         ErrPostEx(SEV_ERROR,0,0, "Cannot allocate memory for LinkSet");
         lsp = LocalLinkSetFree(lsp);
         return NULL;
     }
     else
     {
        BSSeek (bsp, 0L, 0);
        BSRead (bsp, lsp->uids, lsp->num * sizeof (DocUid));
     }
    return lsp;
}




/*
 $Log: slri_post.c,v $
 Revision 1.1.1.1  2001/10/11 17:37:13  gbader
 Automated import from Sourceforge.net

 Revision 1.1.1.1  2001/05/25 21:36:24  kaca
 initial import

 Revision 1.2  2001/04/27 22:00:43  kaca
 cosmetics change

 Revision 1.1  2001/04/06 22:49:29  kaca
 Posting and LinkSet functionality

*/
