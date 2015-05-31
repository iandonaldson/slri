/* $Id: intrez_misc.h,v 1.3 2004/10/19 14:43:37 eburgess Exp $
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
           Katerina Michalickova and Christopher W.V. Hogue hogue@mshri.on.ca


  REVISION/CONTRIBUTION NOTES:

  June 1, 2000  Original MSH Revision.
                Hogue Lab - University of Toronto Biochemistry
                Department and the Samuel Lunenfeld Research
                Institute, Mount Sinai Hospital
                http://bioinfo.mshri.on.ca  hogue@mshri.on.ca
                ftp://ftp.mshri.on.ca/


  PUBLICATION TO CITE:


  DESCRIPTION
  November 1998
  header file for SeqHound database parsers and wwwseekgi cgi-bin,
  and SeqHoundAPI

******************************************************************************************/

#ifndef _INTREZ_MISC_
#define _INTREZ_MISC_


#include <ncbi.h>
#include <asn.h>
#include <objslritax.h>
#include <slri_misc.h>


#ifdef __cplusplus
extern "C" { /* } */
#endif



/****************/
/* miscelaneous */
/****************/
void        LIBCALL PrintValNodeOfInt(ValNodePtr pvn);
void        LIBCALL PrintValNodeOfStr(ValNodePtr pvn);
Boolean     LIBCALL StrToInt4 (CharPtr str, Int4Ptr longval);
Boolean     LIBCALL IsDivisionEST(SeqDescrPtr);
CharPtr     LIBCALL CleanCommas(CharPtr string);
CharPtr     LIBCALL TrimSpace(CharPtr);   /* cuts off spaces at the end of a string */
CharPtr     LIBCALL TrimSpaceButOne(CharPtr);   /* cuts off spaces at the end of a string but leaves one at the end; good for searching CodeBase */
CharPtr     LIBCALL PadString(CharPtr);   /* adds one space at the end of a string */
Int4        LIBCALL GetGI(BioseqPtr);
CharPtr     LIBCALL Misc_GetAccession(BioseqPtr);
Int2        LIBCALL Misc_GetVersion(BioseqPtr pbs);
Boolean     LIBCALL ValNodeIsIn(Int4 num, ValNodePtr pvnval);
Int2        LIBCALL Misc_IswwStringTrueOrFalse(CharPtr);
ValNodePtr  LIBCALL ValNodeUnique(ValNodePtr);
CharPtr     LIBCALL ValNodeToCharPtr(ValNodePtr);



#ifdef __cplusplus
/* { */ }
#endif


#endif  /* _INTREZ_ */


/*
$Log: intrez_misc.h,v $
Revision 1.3  2004/10/19 14:43:37  eburgess
Moved functions to parse gis from bioseqs to their own file: parseasn.c.
This eliminates the dependency of seqrem on intrez_odbc.c.

Revision 1.2  2004/10/15 19:54:28  eburgess
Moved a number of functions from mother.c to intrez_misc.c so that they
can be accessed by updkldge.c. Bug 2952, complete genomes.

Revision 1.1.1.3  2002/08/09 08:29:48  root
Automated import from Sourceforge.net

Revision 1.3  2002/08/08 22:18:18  kaca
added Misc_GetVersion

Revision 1.2  2001/11/13 23:21:50  kaca
added CleanCommas function

Revision 1.1.1.2  2001/10/10 21:53:47  kaca
initial import

Revision 1.4  2001/10/04 18:24:14  kaca
added GetAccession from Bioseq function

Revision 1.3  2001/08/08 00:54:41  kaca
added TrimSpacesButOne

Revision 1.2  2001/03/27 15:44:36  kaca
added newline at EOF

Revision 1.1  2001/03/26 22:33:13  kaca
created intrez_misc file and moved some API functions into it

*/


