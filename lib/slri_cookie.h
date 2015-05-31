/* $Id: slri_cookie.h,v 1.1.1.1 2001/10/12 08:21:54 root Exp $
-- ***************************************************************************
--
--  Samuel Lunenfeld Research Institute (SLRI)
--
--  File: slri_cookie.h
--  Description: WWW cookie functions
--
--  Author: Gary Bader
--  Date: October 1999
--
--  Hogue Lab - University of Toronto Biochemistry Department
--  Samuel Lunenfeld Research Institute, Mount Sinai Hospital
--
--  Copyright Notice:
--
--  Copyright ©2000 Mount Sinai Hospital (MSH)
--
--  This program is free software; you can redistribute it and/or 
--  modify it under the terms of the GNU General Public License as 
--  published by the Free Software Foundation; either version 2 of 
--  the License, or any later version.
--
--  This program is distributed in the hope that it will be useful, 
--  but WITHOUT ANY WARRANTY; without even the implied warranty of 
--  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
--
--  See the GNU General Public License for more details.
--
--  You should have received a copy of the GNU General Public License 
--  along with this program; if not, write to the
--         Free Software Foundation, Inc., 
--         59 Temple Place, Suite 330, Boston, MA  
--         02111-1307  USA 
--  or visit http://www.gnu.org/copyleft/gpl.html
--
--  SPECIAL EXCEPTIONS
--
--  As a special exception, Mount Sinai Hospital gives permission to 
--  link this program with the following non-GPL programs or libraries, 
--  and distribute the resulting executable, without including the source 
--  code for these in the source distribution:
--
--  a) CodeBase 6.5 or greater from Sequiter Software Inc.
--
--  b) The C or C++ interface to Oracle 8.x or greater, from
--     Oracle Corporation or IBM DB2 UDB.
--
-- ***************************************************************************
*/

#ifndef _SLRI_COOKIE_
#define _SLRI_COOKIE_

#include <ncbi.h>

#ifdef __cplusplus
extern "C" {
#endif

/************************/
/* Function definitions */
/************************/

/***********************/
/* WWW Browser Cookies */
/***********************/

/*sets an HTTP cookie - must be called when outputting the HTTP header*/
/*See http://home.netscape.com/newsref/std/cookie_spec.html for the cookie specification*/
/*NOTE: path only specifies what path is saved with the cookie - the path is not returned with the cookie
in all browsers.  In some, it only determines what cookies get sent, as per the spec*/
extern void Cookie_Set(CharPtr nameAndValueString, Int4 secFromNow2Expire, CharPtr path);

/*gets a cookie of a specified name - cookie must be set first by the machine wanting to get the cookie*/
extern CharPtr Cookie_Get(CharPtr name);

/*removes a previously set cookie from the browser's cookie jar*/
extern Int4 Cookie_Eat(CharPtr name, CharPtr path);

#ifdef __cplusplus
}
#endif
#endif		/* _SLRI_COOKIE_ */

/*
$Log: slri_cookie.h,v $
Revision 1.1.1.1  2001/10/12 08:21:54  root
Automated import from Sourceforge.net

Revision 1.1.1.2  2001/05/24 15:08:46  micheld
Update of slrilib

Revision 1.6  2001/03/28 21:45:51  gbader
Moved externs to proper place before function return type

Revision 1.5  2001/03/15 21:09:46  gbader
Moved extern "C" statements to proper place in header files (after includes)

Revision 1.4  2001/01/13 04:04:10  gbader
Added IBM DB2 UDB to license exceptions.

Revision 1.3  2000/09/05 13:55:08  gbader
Updated License so that it is now GNU.
Added CVS keywords ID and Log as well as new line at end of files so compilers on some platforms don't complain

*/

