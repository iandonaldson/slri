/* $Id: slri_cookie.c,v 1.2 2003/10/16 20:59:50 skuznets Exp $
-- ***************************************************************************
--
--  Samuel Lunenfeld Research Institute (SLRI)
--
--  File: slri_cookie.c
--  Description: WWW cookie functions
--
--  Author: Gary Bader
--  Date: October 1998
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

#include <slri_cookie.h>
#include <slri_misc.h>

#define slriSuccess 0
#define slriFail 1

/*sets an HTTP cookie - must be called when outputting the HTTP header*/
/*See http://home.netscape.com/newsref/std/cookie_spec.html for the cookie specification*/
/*NOTE: path only specifies what path is saved with the cookie - the path is not returned with the cookie
in all browsers.  In some, it only determines what cookies get sent, as per the spec*/
void Cookie_Set(CharPtr nameAndValueString, Int4 secFromNow2Expire, CharPtr path)
{
	time_t timeSecs;
	struct tm *time;
	CharPtr expireTimeStr=NULL;

	if(nameAndValueString==NULL) {
		return;
	}

	printf("Set-Cookie: %s", IsNullStr(nameAndValueString));
	/*calculate expiration time*/
	if(secFromNow2Expire != 0) {
		timeSecs = GetSecs();
		timeSecs += secFromNow2Expire;
		time = gmtime(&timeSecs);
		expireTimeStr = MemNew((size_t) 64*sizeof(Char));
		strftime(expireTimeStr, 63, "%A, %d-%b-%Y %H:%M:%S GMT", time);
		printf("; expires=%s", IsNullStr(expireTimeStr));
		expireTimeStr = MemFree(expireTimeStr);
	}
	if(path!=NULL) {
		printf("; path=%s", IsNullStr(path));
	}
	printf("\r\n");
}

/*gets a cookie of a specified name - cookie must be set first by the machine wanting to get the cookie*/
/*NOTE: allocates memory - free returned pointer after use*/
CharPtr Cookie_Get(CharPtr name)
{
	CharPtr cookieStr=NULL, singleCookie=NULL, nameTmp=NULL, cookieRet=NULL;
	Boolean namePathOK=FALSE;

	if(name==NULL) {
		return(NULL);
	}

	cookieStr = StringSave(getenv("HTTP_COOKIE"));
	if(cookieStr == NULL) {
		return(NULL);
	}
	nameTmp = MemNew((size_t) (8+StringLen(name))*sizeof(Char));
	sprintf(nameTmp, "%s=", IsNullStr(name));

	/*parse cookie string and get cookie value by name*/
	singleCookie = StringTok(cookieStr, ";");
	while(singleCookie != NULL) {
		if(StringStr(singleCookie, nameTmp) != NULL) {
			namePathOK = TRUE;
			break;
		}
		singleCookie = StringTok(NULL, ";");
	}

	if((singleCookie != NULL)&&(namePathOK)) {
		cookieRet = StringSave(singleCookie);
	}

	nameTmp = MemFree(nameTmp);
	cookieStr = MemFree(cookieStr);

	return(cookieRet);
}

/*removes a previously set cookie from the browser's cookie jar*/
Int4 Cookie_Eat(CharPtr name, CharPtr path)
{
	if(name==NULL) {
		return(slriFail);
	}

	Cookie_Set(name, -1000000, path);

	return(slriSuccess);
}

/*
$Log: slri_cookie.c,v $
Revision 1.2  2003/10/16 20:59:50  skuznets

1. Checking for NULL pointers when print values
2. Added function Misc_TrimSpacesAroundString
3. Added macro IsNullStr

Revision 1.1.1.1  2001/10/12 08:21:54  root
Automated import from Sourceforge.net

Revision 1.1.1.1  2001/01/13 06:58:00  gbader
Initial Source Forge import

Revision 1.5  2001/01/13 04:04:10  gbader
Added IBM DB2 UDB to license exceptions.

Revision 1.4  2000/09/05 13:55:08  gbader
Updated License so that it is now GNU.
Added CVS keywords ID and Log as well as new line at end of files so compilers on some platforms don't complain

*/
