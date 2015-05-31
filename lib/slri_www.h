/* $Id: slri_www.h,v 1.1.1.4 2002/03/15 09:27:27 root Exp $
-- ***************************************************************************
--
--  Samuel Lunenfeld Research Institute (SLRI)
--
--  File: slri_www.h
--  Description: Rewritten NCBIWWW.C functions to support binary file upload
--               and general use CGI argument and error functions
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

#ifndef _SLRI_WWW_
#define _SLRI_WWW_

#include <ncbi.h>

#ifdef __cplusplus
extern "C" {
#endif

/************************/
/* Function definitions */
/************************/

/***************************/
/* WWW Rewritten NCBIWWW.C */
/***************************/

/*these functions were rewritten to support binary file upload.*/
/*NOTE: Your data entry HTML form must use multipart/form-data encoding and POST for file upload.
EXAMPLE FORM TAG:
printf("<FORM NAME=\"dataentry\" METHOD=\"POST\" ENCTYPE=\"multipart/form-data\" ACTION=\"your action\">\n");
*/

extern WWWErrorCode WWWGetArgsSLRI(WWWInfoPtr PNTR info);

/*file handling functions*/

/*get the filename of the file uploaded with given fieldName.
The file name is local to the client computer and could include the path.*/
/*WARNING: allocates memory - you must free the returned pointer*/
extern CharPtr WWWGetFileName(WWWInfoPtr infoIn, CharPtr fieldName);

/*get the browser given mime-type of the file uploaded with given fieldName*/
/*WARNING: allocates memory - you must free the returned pointer*/
extern CharPtr WWWGetFileMimeType(WWWInfoPtr infoIn, CharPtr fieldName);

/*get the length in bytes of the file uploaded with given fieldName*/
extern Int4 WWWGetFileLength(WWWInfoPtr infoIn, CharPtr fieldName);

/*save an uploaded file to a ByteStore with the given fieldName.*/
extern Int4 WWWFile2BSFromWWWInfo(WWWInfoPtr infoIn, CharPtr fieldName, ByteStorePtr bsp);

/*write an uploaded file to disk with the given fieldName.  An optional path to write to may be specified*/
extern Int4 WWWWriteFileFromWWWInfo(WWWInfoPtr infoIn, CharPtr fieldName, CharPtr path);

/*write all uploaded files to disk.  An optional path to write to may be specified*/
extern Int4 WWWWriteAllFilesFromWWWInfo(WWWInfoPtr infoIn, CharPtr path);

/*internal functions*/

/* ----------------------  WWWGetArgsInternal  -------------------------
   Purpose:     This function read HTML input in POST, GET or
                multipart/form-data encoding - depends upon
                environment. If used from command-line this
                function will return valid WWWInfo structure
                with all field blank exept info->method, that
                will be set to COMMAND_LINE

   Returns:     WWWInfoPtr structure with processed HTTP input and
                environment

  NOTE: Supports binary file upload via HTTPD
  ------------------------------------------------------------------*/
extern WWWErrorCode WWWGetArgsInternalSLRI(WWWInfoPtr PNTR info_out, Boolean ReadArgv);

extern WWWEntryPtr PNTR WWWGetEntriesSLRI(Int4Ptr num_entries, CharPtr WWWBuffer_in, Boolean multipartFormOK, Int4 WWWLen);

extern Int4 WWWGetEntriesFormDataSLRI(WWWEntryPtr PNTR entries, CharPtr WWWBuffer, Int4 WWWLen);

/*redefinition of NCBI static functions*/
extern WWWInfoPtr WWWInfoNewSLRI(void);

extern Boolean WWWReadEnvironmentSLRI(WWWInfoDataPtr info);

/***********/
/* WWW CGI */
/***********/

/*returns the integer value of CGI argument name, if this name was not found,
errorString is saved in vnpError and the default value (defVal) is returned.
If setErrForMissingVal is FALSE, caller must take care of errors for the case where
the argument name is present but no value has been passed*/
extern Int4 GetIntegerArg(WWWInfoPtr info, CharPtr argName, CharPtr errorStringHTML, ValNodePtr *vnpError, Int4 defVal,
				   Boolean setErrForMissingVal);

/*returns the character value of CGI argument name, if this name was not found,
errorString is saved in vnpError
If setErrForMissingVal is FALSE, caller must take care of errors for the case where
the argument name is present but no value has been passed*/
extern CharPtr GetTextArg(WWWInfoPtr info, CharPtr argName, CharPtr errorStringHTML, ValNodePtr *vnpError, CharPtr defVal,
				   Boolean setErrForMissingVal);

/*records an HTML error in the HTML error buffer (vnpError)*/
extern Int4 SetHTMLError(ValNodePtr *vnpError, CharPtr errorStringHTML, ...);

/*prints the HTML CGI error buffer to stdout.*/
extern void PrintHTMLErrBuf(ValNodePtr vnpError);

/*logs the HTML CGI error buffer to the log file.
Will only log if NCBI logging has been enabled with the following calls
	if( !ErrSetLogfile (LogFile, ELOG_APPEND) ) {
		printf("Content-type: text/html\r\n\r\n");
		printf("<H2>Error</H2>\n - Unable to Open Log - contact %s\n", Cfg_GetMailTo());
		exit(1);
	}

	ErrSetOptFlags(EO_LOGTO_USRFILE);
*/
extern Int2 LogHTMLErrBuf(ValNodePtr vnpError);

/*frees the HTML CGI error buffer*/
extern ValNodePtr FreeHTMLErrBuf(ValNodePtr vnpError);

/*returns a new string that represents the escaped version of the given string -
CGI hexadecimal escape rules*/
extern CharPtr WWW_HexEscapeString(CharPtr string);

/*returns a new string that represents the escaped version of the given string -
HTML escape rules*/
CharPtr WWW_HTMLEscapeString(CharPtr string);

/*Printf that will print out text in an HTML safe way (i.e. encodes characters <>&")
Returns the number of bytes printed*/
/*Encodes insecure cross-site scripting vulnerable characters. See (http://www.cert.org/advisories/CA-2000-02.html)*/
extern Int4 HTMLPrintf(CharPtr format, ...);

/*prints out NCBI generated XML as HTML
(Won't encode already encoded characters in the set <>&" and will change &apos; to ')*/
extern Int4 XML2HTMLPrintf(CharPtr format, ...);

/*test the file system environment to make sure process has read and write permissions on files*/
/*Call this function from the beginning of any CGI program.*/
extern void CGI_TestFileSystem(void);

#ifdef __cplusplus
}
#endif
#endif		/* _SLRI_WWW_ */

/*
$Log: slri_www.h,v $
Revision 1.1.1.4  2002/03/15 09:27:27  root
Automated import from Sourceforge.net

Revision 1.4  2002/03/14 15:48:24  hfeldman
moved wwwuploadtextfiles to taskapi for now

Revision 1.3  2002/03/11 18:07:42  hfeldman
Added params to uploadtextfiles for proxy password

Revision 1.2  2002/03/07 23:07:13  micheld
Added user_header option for uploadtextfiles

Revision 1.1.1.3  2001/10/10 22:11:04  kaca
initial import

Revision 1.12  2001/09/26 19:26:05  gbader
Added new escape functions.

Revision 1.11  2001/04/04 21:19:58  feldman
Removed Netscape check for form data file upload (will try regardless
of browser) and added file upload function

Revision 1.10  2001/03/28 21:45:51  gbader
Moved externs to proper place before function return type

Revision 1.9  2001/03/15 21:09:46  gbader
Moved extern "C" statements to proper place in header files (after includes)

Revision 1.8  2001/03/08 00:09:55  gbader
Moved extraneous header files inside the C files to make reduce header dependencies and make compiling much more efficient.

Revision 1.7  2001/01/13 04:04:10  gbader
Added IBM DB2 UDB to license exceptions.

Revision 1.6  2000/09/13 18:42:54  gbader
Added XML2HTMLPrintf function to print XML as HTML

Revision 1.5  2000/09/05 13:55:08  gbader
Updated License so that it is now GNU.
Added CVS keywords ID and Log as well as new line at end of files so compilers on some platforms don't complain

*/
