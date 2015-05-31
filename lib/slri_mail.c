/* $Id: slri_mail.c,v 1.3 2004/10/18 20:49:15 hfeldman Exp $
-- ***************************************************************************
--
--  Samuel Lunenfeld Research Institute (SLRI)
--
--  File: slri_mail.c
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

#include <slri_mail.h>
#include <connect/ncbi_connection.h>
#include <connect/ncbi_socket_connector.h>
#include <ncbisock.h>
#include <slri_misc.h>

#define SMTP_STATUS 211
#define SMTP_HELPMSG 214
#define SMTP_SERVICE_READY 220
#define SMTP_SERVICE_CLOSING 221
#define SMTP_OKAY 250
#define SMTP_FORWARD 251
#define SMTP_START_INPUT 354
#define SMTP_SERVICE_NOT_AVAILABLE 421
#define SMTP_MAILBOX_UNAVAILABLE 450
#define SMTP_LOCAL_ERROR 451
#define SMTP_OUT_OF_SYSTEM_STORAGE 452
#define SMTP_SYNTAX_ERROR 500
#define SMTP_SYNTAX_ERROR_PARAM 501
#define SMTP_COMMAND_NOT_IMPLEMENTED 502
#define SMTP_BAD_COMMAND_SEQUENCE 503
#define SMTP_COMMAND_PARAM_NOT_IMPLEMENTED 504
#define SMTP_MAILBOX_ERROR 550
#define SMTP_USER_NOT_LOCAL 551
#define SMTP_EXCEEDED_ALLOWED_STORAGE 552
#define SMTP_MAILBOX_NAME_NOT_ALLOWED 553
#define SMTP_TRANSACTION_FAILED 554

/*converts a server response string into an error number, returns -1 for error*/
static Int4 MailServerResponseString2ErrNum(CharPtr string)
{
	Char errCodeStr[4];

	if(string==NULL) {
		ErrPostEx(SEV_INFO, 0, 0, "MailServerResponseString2ErrNum: Passed string was NULL.");
		return(-1);
	}

	strncpy(errCodeStr, string, 3);
	errCodeStr[3]='\0';
	if(Misc_IsNumeric(errCodeStr)) {
		return(atol(errCodeStr));
	}

	return(-1);
}

/*reads and returns an error code, assumes readBuf is allocated, -1 for error*/
static Int4 MailServerReadResponse(CONN conn, CharPtr readBuf, CharPtr SMTPServer)
{
	EIO_Status status;
	size_t nBytes=0;
	Int4 errCode=0;

	status = CONN_Read(conn, readBuf, 511, &nBytes, eIO_Plain);
	if(status != eIO_Success) {
		ErrPostEx(SEV_ERROR,0,0,"MailServerReadResponse: Server %s is unavailable. Aborting.",IsNullStr(SMTPServer));
		return(-1);
	}
	if((errCode = MailServerResponseString2ErrNum(readBuf)) < 0) {
		ErrPostEx(SEV_ERROR,0,0,"MailServerReadResponse: Server %s sent invalid return code. Aborting.", IsNullStr(SMTPServer));
		return(-1);
	}

	return(errCode);
}

/*Send possibly multiple to addresses (can be a comma separated list)*/
static Int4 SendToEMailList(CONN conn, CharPtr sendBuf, CharPtr readBuf, CharPtr toEMail, CharPtr SMTPServer)
{
	size_t nBytes=0;
	CharPtr nextAddress=NULL, toEMailTmpHead=NULL, toEMailTmp=NULL;
	ValNodePtr vnpAddressList=NULL;
	Int4 errorCode=0;

	if(conn==NULL) {
		ErrPostEx(SEV_INFO, 0, 0, "SendToEMailList: Passed conn was NULL.");
		return(-1);
	}
	if(sendBuf==NULL) {
		ErrPostEx(SEV_INFO, 0, 0, "SendToEMailList: Passed sendBuf was NULL.");
		return(-1);
	}
	if(readBuf==NULL) {
		ErrPostEx(SEV_INFO, 0, 0, "SendToEMailList: Passed readBuf was NULL.");
		return(-1);
	}
	if(toEMail==NULL) {
		ErrPostEx(SEV_INFO, 0, 0, "SendToEMailList: Passed toEMail was NULL.");
		return(-1);
	}
	if(SMTPServer==NULL) {
		ErrPostEx(SEV_INFO, 0, 0, "SendToEMailList: Passed SMTPServer was NULL.");
		return(-1);
	}

	/*convert toEMail to a linked list of e-mail addresses*/
	if(strpbrk(toEMail, " ,")!=NULL) {
		/*copy toEMail to temp buffer*/
		toEMailTmp = StringSave(toEMail);
		toEMailTmpHead = toEMailTmp;
		/*multiple e-mail addresses present*/
		while(toEMailTmp!=NULL) {
			nextAddress = strchr(toEMailTmp, ',');
			if(nextAddress!=NULL) {
				nextAddress[0]='\0';
				nextAddress++;
			}
			ValNodeCopyStr(&vnpAddressList, 0, toEMailTmp);
			toEMailTmp = nextAddress;
		}
		toEMailTmpHead = MemFree(toEMailTmpHead);
	}
	else {
		/*only one e-mail address present*/
		ValNodeCopyStr(&vnpAddressList, 0, toEMail);
	}

	while(vnpAddressList!=NULL) {
		sprintf(sendBuf, "RCPT TO:<%s>\r\n", IsNullStr((CharPtr)(vnpAddressList->data.ptrvalue)));
                if(CONN_Write(conn, sendBuf, StringLen(sendBuf), &nBytes, eIO_WritePlain)!=eIO_Success) {
			ErrPostEx(SEV_ERROR,0,0,"SLRI_Mail2ServerExtra: Can't send RCPT TO %s to server %s.", IsNullStr(vnpAddressList->data.ptrvalue), IsNullStr(SMTPServer));
			return(-1);
		}
		if((errorCode=MailServerReadResponse(conn, readBuf, SMTPServer))!=SMTP_OKAY) {
			ErrPostEx(SEV_WARNING,0,0,"SLRI_Mail2ServerExtra: Error %ld returned from server after RCPT TO %s.", (long) errorCode, IsNullStr(vnpAddressList->data.ptrvalue));
			/*not a fatal error - some e-mail addresses may be bad*/
		}
		vnpAddressList = vnpAddressList->next;
	}

	/*free memory*/
	ValNodeFreeData(vnpAddressList);

	return(0);
}

/*send the message header - replyToEMail is optional*/
static Int4 SendHeader(CONN conn, CharPtr sendBuf, CharPtr fromEMail, CharPtr toEMail, CharPtr replyToEMail, CharPtr subject, CharPtr SMTPServer)
{
	time_t ltime;
	struct tm *ptmtime=NULL;
	Char timeDate[PATH_MAX];
	size_t nBytes=0;

	if(conn==NULL) {
		ErrPostEx(SEV_INFO, 0, 0, "SendHeader: Passed conn was NULL.");
		return(-1);
	}
	if(sendBuf==NULL) {
		ErrPostEx(SEV_INFO, 0, 0, "SendHeader: Passed sendBuf was NULL.");
		return(-1);
	}
	if(fromEMail==NULL) {
		ErrPostEx(SEV_INFO, 0, 0, "SendHeader: Passed fromEMail was NULL.");
		return(-1);
	}
	if(toEMail==NULL) {
		ErrPostEx(SEV_INFO, 0, 0, "SendHeader: Passed toEMail was NULL.");
		return(-1);
	}
	if(subject==NULL) {
		ErrPostEx(SEV_INFO, 0, 0, "SendHeader: Passed subject was NULL.");
		return(-1);
	}
	if(SMTPServer==NULL) {
		ErrPostEx(SEV_INFO, 0, 0, "SendHeader: Passed SMTPServer was NULL.");
		return(-1);
	}

	/*Date*/
	time(&ltime);
	ptmtime=localtime(&ltime);
	strftime(timeDate,(size_t)PATH_MAX,"%a, %d %b %Y %H:%M:%S %z",ptmtime);
	sprintf(sendBuf, "Date: %s\r\n", IsNullStr(timeDate));
        if(CONN_Write(conn, sendBuf, StringLen(sendBuf), &nBytes, eIO_WritePlain) != eIO_Success) {
		ErrPostEx(SEV_ERROR,0,0,"SendHeader: Can't send date: to server %s.", IsNullStr(SMTPServer));
		return(-1);
	}
	/*From*/
	sprintf(sendBuf, "From: %s\r\n", IsNullStr(fromEMail));
        if(CONN_Write(conn, sendBuf, StringLen(sendBuf), &nBytes, eIO_WritePlain) != eIO_Success) {
		ErrPostEx(SEV_ERROR,0,0,"SendHeader: Can't send from: to server %s.", IsNullStr(SMTPServer));
		return(-1);
	}
	/*Subject*/
	sprintf(sendBuf, "Subject: %s\r\n", IsNullStr(subject));
        if(CONN_Write(conn, sendBuf, StringLen(sendBuf), &nBytes, eIO_WritePlain) != eIO_Success) {
		ErrPostEx(SEV_ERROR,0,0,"SendHeader: Can't send subject: to server %s.", IsNullStr(SMTPServer));
		return(-1);
	}
	/*To*/
	sprintf(sendBuf, "To: %s\r\n", IsNullStr(toEMail));
        if(CONN_Write(conn, sendBuf, StringLen(sendBuf), &nBytes, eIO_WritePlain) != eIO_Success) {
		ErrPostEx(SEV_ERROR,0,0,"SendHeader: Can't send to: to server %s.", IsNullStr(SMTPServer));
		return(-1);
	}
	if(replyToEMail!=NULL) {
		/*Reply-To*/
		sprintf(sendBuf, "Reply-To: %s\r\n", IsNullStr(replyToEMail));
                if(CONN_Write(conn, sendBuf, StringLen(sendBuf), &nBytes, eIO_WritePlain) != eIO_Success) {
			ErrPostEx(SEV_ERROR,0,0,"SendHeader: Can't send to: to server %s.", IsNullStr(SMTPServer));
			return(-1);
		}
	}
	/*Blank line*/
	sprintf(sendBuf, "\r\n");
        if(CONN_Write(conn, sendBuf, StringLen(sendBuf), &nBytes, eIO_WritePlain) != eIO_Success) {
		ErrPostEx(SEV_ERROR,0,0,"SendHeader: Can't send blank line to server %s.", IsNullStr(SMTPServer));
		return(-1);
	}

	return(0);
}

/*send the message - encode . as ..*/
static Int4 SendMessage(CONN conn, CharPtr sendBuf, CharPtr message, CharPtr SMTPServer)
{ 
	size_t nBytes=0;
	Int4 maxLineLength=128, actualLineLength=0;
	CharPtr pcTmp=NULL;
	Boolean dotStart=FALSE;
	Char sendLine[129];

	if(conn==NULL) {
		ErrPostEx(SEV_INFO, 0, 0, "SendMessage: Passed conn was NULL.");
		return(-1);
	}
	if(sendBuf==NULL) {
		ErrPostEx(SEV_INFO, 0, 0, "SendMessage: Passed sendBuf was NULL.");
		return(-1);
	}
	if(message==NULL) {
		ErrPostEx(SEV_INFO, 0, 0, "SendMessage: Passed message was NULL.");
		return(-1);
	}
	if(SMTPServer==NULL) {
		ErrPostEx(SEV_INFO, 0, 0, "SendMessage: Passed SMTPServer was NULL.");
		return(-1);
	}

	/*send message until \n, otherwise limit to maxLineLength*/
	while(message!=NULL) {
		/*test start of message for .*/
		if(message[0]=='.') {
			dotStart=TRUE;
		}
		if((pcTmp = strchr(message, '\n'))!=NULL) {
			if((pcTmp-message+1)<maxLineLength) {
				/*print until (not including) newline*/
				strncpy(sendLine, message, (size_t) (pcTmp-message));
				sendLine[pcTmp-message]='\0';
				sprintf(sendBuf, "%s%s\r\n", (dotStart)?".":"", IsNullStr(sendLine));
				pcTmp++;	/*skip newline*/
			}
		}
		else {
			/*chunk into maxLineLength length lines*/
			if((actualLineLength=strlen(message))>maxLineLength) {
				actualLineLength = maxLineLength;
			}
			strncpy(sendLine, message, (size_t) actualLineLength);
			sendLine[actualLineLength]='\0';
			sprintf(sendBuf, "%s%s\r\n", (dotStart)?".":"", IsNullStr(sendLine));
		}
                if(CONN_Write(conn, sendBuf, StringLen(sendBuf), &nBytes, eIO_WritePlain) != eIO_Success) {
			ErrPostEx(SEV_ERROR,0,0,"SendHeader: Can't send blank line to server %s.", IsNullStr(SMTPServer));
			return(-1);
		}
		message=pcTmp;
		dotStart=FALSE;
	}

	return(0);
}

/*send end of message command*/
static Int4 SendEndOfMessage(CONN conn, CharPtr sendBuf, CharPtr SMTPServer)
{
	size_t nBytes=0;

	if(conn==NULL) {
		ErrPostEx(SEV_INFO, 0, 0, "SendEndOfMessage: Passed conn was NULL.");
		return(-1);
	}
	if(sendBuf==NULL) {
		ErrPostEx(SEV_INFO, 0, 0, "SendEndOfMessage: Passed sendBuf was NULL.");
		return(-1);
	}
	if(SMTPServer==NULL) {
		ErrPostEx(SEV_INFO, 0, 0, "SendEndOfMessage: Passed SMTPServer was NULL.");
		return(-1);
	}

	sprintf(sendBuf, ".\r\n");
        if(CONN_Write(conn, sendBuf, StringLen(sendBuf), &nBytes, eIO_WritePlain) != eIO_Success) {
		ErrPostEx(SEV_ERROR,0,0,"SendEndOfMessage: Can't send end of message to server %s.", IsNullStr(SMTPServer));
		return(-1);
	}

	return(0);
}

/*Sends an e-mail message to a specific SMTP server with a given from address.
If fromEMail or SMTPServer is NULL, values for these will be looked up in a the slrimail configuration file, if present.*/
/*toEMail may be a comma separated list of addresses*/
/*replyToEMail is optional and may be NULL, bccToEMail is optional and may be a comma separated list of addresses*/
Int4 SLRI_Mail2ServerExtra(CharPtr toEMail, CharPtr replyToEMail, CharPtr subject, CharPtr message, CharPtr SMTPServer, CharPtr fromEMail, CharPtr bccToEMail)
{
	CONN conn;
	CONNECTOR connector;
	EIO_Status status;
	Char sendBuf[512], readBuf[512], localHostName[50], SMTPServerbuf[128], fromEMailbuf[128];
	size_t nBytes=0;
	Int4 errorCode=0;

	if(toEMail==NULL) {
		ErrPostEx(SEV_INFO, 0, 0, "SLRI_Mail2ServerExtra: Passed toEMail was NULL.");
		return(-1);
	}
	if(subject==NULL) {
		ErrPostEx(SEV_INFO, 0, 0, "SLRI_Mail2ServerExtra: Passed subject was NULL.");
		return(-1);
	}
	if(message==NULL) {
		ErrPostEx(SEV_INFO, 0, 0, "SLRI_Mail2ServerExtra: Passed message was NULL.");
		return(-1);
	}
	/*bccToEMail is optional*/

	/*SMTPServer and fromEMail are optional*/
	if(SMTPServer==NULL) {
		/*get from config file*/
		GetAppParam("slrimail", "mail", "SMTPServer", "none", SMTPServerbuf, 128);
		/*if not in local config file, abort*/
		if(StringICmp(SMTPServerbuf, "none")==0) {
			ErrPostEx(SEV_INFO, 0, 0, "SLRI_Mail2ServerExtra: SMTP server not passed and not specified in config file.");
			return(-1);
		}
		SMTPServer = SMTPServerbuf;
	}
	if(fromEMail==NULL) {
		/*get from config file*/
		/*if not in local config file, use default*/
		GetAppParam("slrimail", "mail", "From", "admin@localhost", fromEMailbuf, 128);
		fromEMail = fromEMailbuf;
	}

	/*connect to server on port SLRIMAIL_SMTP_PORT - try 3 times*/
	connector = SOCK_CreateConnector(SMTPServer, SLRIMAIL_SMTP_PORT, 3);
	status = CONN_Create(connector, &conn);
	if(status != eIO_Success) {
		ErrPostEx(SEV_ERROR,0,0,"SLRI_Mail2ServerExtra: Can't connect to %s. Aborting.", IsNullStr(SMTPServer));
		/*TODO: find out how to properly free CONNECTOR*/
		MemFree(connector);
		CONN_Close(conn);
		return(-1);
	}

	/*initiate SMTP conversation*/
	if((errorCode=MailServerReadResponse(conn, readBuf, SMTPServer))!=SMTP_SERVICE_READY) {
		ErrPostEx(SEV_ERROR,0,0,"SLRI_Mail2ServerExtra: Error %ld returned from server before conversation.", (long) errorCode);
		CONN_Close(conn);
		return(-1);
	}

	/*say HELO - it's only polite*/
	if(GetHostName(localHostName, (size_t) 49)==FALSE) {
		ErrPostEx(SEV_INFO,0,0,"SLRI_Mail2ServerExtra: Error getting local host name, using 'localhost'.");
		/*make one up*/
		strcpy(localHostName, "localhost");
	}

	sprintf(sendBuf, "HELO %s\r\n", IsNullStr(localHostName));
        status = CONN_Write(conn, sendBuf, StringLen(sendBuf), &nBytes, eIO_WritePlain);
	if(status != eIO_Success) {
		ErrPostEx(SEV_ERROR,0,0,"SLRI_Mail2ServerExtra: Can't send HELO to server %s.", IsNullStr(SMTPServer));
		CONN_Close(conn);
		return(-1);
	}
	if((errorCode=MailServerReadResponse(conn, readBuf, SMTPServer))!=SMTP_OKAY) {
		ErrPostEx(SEV_ERROR,0,0,"SLRI_Mail2ServerExtra: Error %ld returned from server after HELO.", (long) errorCode);
		CONN_Close(conn);
		return(-1);
	}

	/*mail from:*/
	sprintf(sendBuf, "MAIL FROM:<%s>\r\n", IsNullStr(fromEMail));
        status = CONN_Write(conn, sendBuf, StringLen(sendBuf), &nBytes, eIO_WritePlain);
	if(status != eIO_Success) {
		ErrPostEx(SEV_ERROR,0,0,"SLRI_Mail2ServerExtra: Can't send MAIL FROM to server %s.", IsNullStr(SMTPServer));
		CONN_Close(conn);
		return(-1);
	}
	if((errorCode=MailServerReadResponse(conn, readBuf, SMTPServer))!=SMTP_OKAY) {
		ErrPostEx(SEV_ERROR,0,0,"SLRI_Mail2ServerExtra: Error %ld returned from server after MAIL FROM.", (long) errorCode);
		CONN_Close(conn);
		return(-1);
	}

	/*send to:*/
	if(SendToEMailList(conn, sendBuf, readBuf, toEMail, SMTPServer)<0) {
		ErrPostEx(SEV_ERROR,0,0,"SLRI_Mail2ServerExtra: Unable to send to e-mail address.");
		CONN_Close(conn);
		return(-1);
	}

	if(bccToEMail!=NULL) {
		/*send bcc:*/
		if(SendToEMailList(conn, sendBuf, readBuf, bccToEMail, SMTPServer)<0) {
			ErrPostEx(SEV_ERROR,0,0,"SLRI_Mail2ServerExtra: Unable to send to e-mail address.");
			CONN_Close(conn);
			return(-1);
		}
	}

	/*send message*/
	sprintf(sendBuf, "DATA\r\n");
        status = CONN_Write(conn, sendBuf, StringLen(sendBuf), &nBytes, eIO_WritePlain);
	if(status != eIO_Success) {
		ErrPostEx(SEV_ERROR,0,0,"SLRI_Mail2ServerExtra: Can't send DATA to server %s.", IsNullStr(SMTPServer));
		CONN_Close(conn);
		return(-1);
	}
	if((errorCode=MailServerReadResponse(conn, readBuf, SMTPServer))!=SMTP_START_INPUT) {
		ErrPostEx(SEV_ERROR,0,0,"SLRI_Mail2ServerExtra: Error %ld returned from server after DATA.", (long) errorCode);
		CONN_Close(conn);
		return(-1);
	}
	if(SendHeader(conn, sendBuf, fromEMail, toEMail, replyToEMail, subject, SMTPServer)<0) {
		ErrPostEx(SEV_ERROR,0,0,"SLRI_Mail2ServerExtra: Unable to send message header.");
		CONN_Close(conn);
		return(-1);
	}
	if(SendMessage(conn, sendBuf, message, SMTPServer)<0) {
		ErrPostEx(SEV_ERROR,0,0,"SLRI_Mail2ServerExtra: Unable to send message.");
		CONN_Close(conn);
		return(-1);
	}
	if(SendEndOfMessage(conn, sendBuf, SMTPServer)<0) {
		ErrPostEx(SEV_ERROR,0,0,"SLRI_Mail2ServerExtra: Unable to send end of message.");
		CONN_Close(conn);
		return(-1);
	}
	if((errorCode=MailServerReadResponse(conn, readBuf, SMTPServer))!=SMTP_OKAY) {
		ErrPostEx(SEV_ERROR,0,0,"SLRI_Mail2ServerExtra: Error %ld returned from server after sending end of message.", (long) errorCode);
		CONN_Close(conn);
		return(-1);
	}

	/*quit*/
	sprintf(sendBuf, "QUIT\r\n");
        status = CONN_Write(conn, sendBuf, StringLen(sendBuf), &nBytes, eIO_WritePlain);
	if(status != eIO_Success) {
		ErrPostEx(SEV_ERROR,0,0,"SLRI_Mail2ServerExtra: Can't send QUIT to server %s.", IsNullStr(SMTPServer));
		CONN_Close(conn);
		return(-1);
	}
	if((errorCode=MailServerReadResponse(conn, readBuf, SMTPServer))!=SMTP_SERVICE_CLOSING) {
		ErrPostEx(SEV_ERROR,0,0,"SLRI_Mail2ServerExtra: Error %ld returned from server after QUIT.", (long) errorCode);
		CONN_Close(conn);
		return(-1);
	}

	/*close socket*/
	CONN_Close(conn);

	return(0);
}

/*Sends an e-mail using the SMTP server and from address specified in the slrimail configuration file*/
Int4 SLRI_Mail2Server(CharPtr toEMail, CharPtr subject, CharPtr message)
{
	return(SLRI_Mail2ServerExtra(toEMail, NULL, subject, message, NULL, NULL, NULL));
}

/*
$Log: slri_mail.c,v $
Revision 1.3  2004/10/18 20:49:15  hfeldman
Changed to new CONN_Write args

Revision 1.2  2003/10/16 20:59:50  skuznets

1. Checking for NULL pointers when print values
2. Added function Misc_TrimSpacesAroundString
3. Added macro IsNullStr

Revision 1.1.1.3  2003/04/03 09:35:37  root
Automated import from Sourceforge.net

Revision 1.3  2003/04/02 20:48:47  hfeldman
fixed malformed date in mail function

Revision 1.2  2002/09/05 17:34:26  gbader
Added BCC feature

Revision 1.1.1.2  2001/05/24 15:08:46  micheld
Update of slrilib

Revision 1.9  2001/04/17 00:58:36  feldman
Fixed compiler warning

Revision 1.8  2001/04/16 19:22:50  feldman
Updated for new toolkit

Revision 1.7  2001/04/05 14:12:02  feldman
Fixed a few compiler warnings

Revision 1.6  2001/04/04 22:22:13  gbader
Added optional Reply-to field

Revision 1.5  2001/04/03 19:08:47  feldman
fixed illegal comparison and added mail to library makefile

Revision 1.4  2001/03/08 00:13:29  gbader
Moved includes to C files from header files to reduce header dependencies and speed compilation

Revision 1.3  2001/01/13 04:04:10  gbader
Added IBM DB2 UDB to license exceptions.

Revision 1.2  2000/12/12 19:50:18  gbader
Added support for multiple e-mail addresses and config file to store SMTP server information

Revision 1.1  2000/12/07 00:23:20  gbader
Added functionality to send e-mail to someone via an SMTP server from a C program using the SLRI lib.  Right now you must specify an SMTP server, but a configuration file where that is specified is planned.

*/

