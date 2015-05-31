/* $Id: slri_endec.h,v 1.1.1.2 2002/03/11 09:27:32 root Exp $
***************************************************************************

  Samuel Lunenfeld Research Institute (SLRI)

  File: slri_endec.c
  Description: Endcoding/Decoding routines

  Author: Howard Feldman (feldman@mshri.on.ca)
  Date: March 2002

  Hogue Lab - University of Toronto Biochemistry Department
  Samuel Lunenfeld Research Institute, Mount Sinai Hospital

  Copyright Notice:

  Copyright (C) 2001 Mount Sinai Hospital (MSH)

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

***************************************************************************
*/

#ifndef _SLRI_ENDEC_
#define _SLRI_ENDEC_

#include <ncbi.h>

#ifdef __cplusplus
extern "C" {
#endif

/* encode/decode string in src and return a newly allocated string */
/* NULL is returned upon error */
/* note that by convention when outputting the encoded string for later
   decoding by a DIFFERENT piece of software, you should only print 72
   characters per line; newlines are not inserted automatically by this
   function every 72 characters */
/* note that src may contain '\0's and non-printable characters, hence the
   need for explicitly specifying the length; also, pc is NOT normally NULL
   terminated */
CharPtr Base64Encode(CharPtr src, Int2 len);
Int2 Base64Decode(CharPtr src,CharPtr *retval);

/* UUencode len bytes starting at pc and return a newly allocated string */
/* note that pc may contain '\0's and non-printable characters, hence the
   need for explicitly specifying the length; also, pc is NOT normally NULL
   terminated */
CharPtr UUencode(CharPtr pc, Int2 len);
/* UUdecode the text string in buf and set the previously unallocated string
   *retval to point to a newly allocated block of memory conatining the decoded
   message; note that the decoded message may contain '\0' characters and other
   non-printable characters, so the return value is always the true length of the
   encoded message; *retval will NOT normally be NULL terminated */
Int2 UUdecode(CharPtr buf,CharPtr *retval);

#ifdef __cplusplus
}
#endif

#endif

/*
$Log: slri_endec.h,v $
Revision 1.1.1.2  2002/03/11 09:27:32  root
Automated import from Sourceforge.net

Revision 1.2  2002/03/10 16:54:14  hfeldman
added base64 support for binary

Revision 1.1  2002/03/07 19:32:35  micheld
Initial Import


*/
