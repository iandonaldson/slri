/* $Id: slri_mail.h,v 1.1.1.2 2002/09/06 08:29:48 root Exp $
-- ***************************************************************************
--
--  Samuel Lunenfeld Research Institute (SLRI)
--
--  File: slri_mail.h
--  Description: WWW SMTP mail sending functions
--               See RFC821 - Simple Mail Transfer Protocol
--
--  Author: Gary Bader
--  Date: December 2000
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

#ifndef _SLRI_MAIL_
#define _SLRI_MAIL_

#include <ncbi.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SLRIMAIL_SMTP_PORT 25

/************************/
/* Function definitions */
/************************/

/*NOTE: toEMail can be a comma separated list of e-mail addresses.
	An slrimail configuration file can be present of the form:
	
	[mail]
	#your SMTP server
	SMTPServer=yourserver
	#The e-mail address that mail will be from
	From=admin@myhost.com
	*/

/*Sends an e-mail message to a specific SMTP server with a given from address.
If fromEMail or SMTPServer is NULL, values for these will be looked up in a the slrimail configuration file, if present.*/
/*replyToEMail is optional and may be NULL*/
extern Int4 SLRI_Mail2ServerExtra(CharPtr toEMail, CharPtr replyToEMail, CharPtr subject, CharPtr message, CharPtr SMTPServer, CharPtr fromEMail, CharPtr bccToEMail);

/*Sends an e-mail using the SMTP server and from address specified in the slrimail configuration file*/
extern Int4 SLRI_Mail2Server(CharPtr toEMail, CharPtr subject, CharPtr message);

#ifdef __cplusplus
}
#endif
#endif		/* _SLRI_MAIL_ */

/*
$Log: slri_mail.h,v $
Revision 1.1.1.2  2002/09/06 08:29:48  root
Automated import from Sourceforge.net

Revision 1.2  2002/09/05 17:34:26  gbader
Added BCC feature

Revision 1.1.1.2  2001/05/24 15:08:46  micheld
Update of slrilib

Revision 1.7  2001/04/04 22:22:13  gbader
Added optional Reply-to field

Revision 1.6  2001/03/28 21:45:51  gbader
Moved externs to proper place before function return type

Revision 1.5  2001/03/15 21:09:46  gbader
Moved extern "C" statements to proper place in header files (after includes)

Revision 1.4  2001/03/08 00:09:55  gbader
Moved extraneous header files inside the C files to make reduce header dependencies and make compiling much more efficient.

Revision 1.3  2001/01/13 04:04:10  gbader
Added IBM DB2 UDB to license exceptions.

Revision 1.2  2000/12/12 19:50:18  gbader
Added support for multiple e-mail addresses and config file to store SMTP server information

Revision 1.1  2000/12/07 00:23:20  gbader
Added functionality to send e-mail to someone via an SMTP server from a C program using the SLRI lib.  Right now you must specify an SMTP server, but a configuration file where that is specified is planned.

*/

