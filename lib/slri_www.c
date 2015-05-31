/* $Id: slri_www.c,v 1.2 2003/10/16 20:59:50 skuznets Exp $
-- ***************************************************************************
--
--  Samuel Lunenfeld Research Institute (SLRI)
--
--  File: slri_www.c
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

#include <slri_misc.h>
#include <slri_www.h>
#include <tsprintf.h>	/*for TSPRINTF macro*/

/*these functions were rewritten to support binary file upload*/

WWWErrorCode WWWGetArgsSLRI(WWWInfoPtr PNTR info)
{
  return WWWGetArgsInternalSLRI(info, TRUE);
}

/* ----------------------  WWWGetArgsInternalSLRI  -------------------------
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
WWWErrorCode WWWGetArgsInternalSLRI(WWWInfoPtr PNTR info_out, Boolean ReadArgv)
{
	Int4 WWWLen=0;
	WWWInfoDataPtr info=NULL;
	CharPtr PNTR WWWargv=NULL;
	Int4 WWWargc=0;
	FILE *binStdIn=NULL;
	Boolean multipartFormOK=FALSE;

	if((info = (WWWInfoDataPtr) WWWInfoNewSLRI()) == NULL)
		return WWWErrNoMem;

	/*open stdin as binary*/
	binStdIn = FileOpen("stdin", "rb");

	/* Reading environment from HTTPD */
	if(!WWWReadEnvironmentSLRI(info)) {
		info->method = COMMAND_LINE;

		if(ReadArgv == TRUE) {
			WWWargc = GetArgc();
			WWWargv = GetArgv();

			/* Now try to initialize WWWInfo structure from binStdIn or command line */
			if(WWWargc == 1) { /* reading binStdIn */
				if((info->query = WWWReadFileInMemory(binStdIn, 0, FALSE)) != NULL) {
					info->entries = WWWGetEntriesSLRI(&info->num_entries, info->query, multipartFormOK, WWWLen);
				}
			}
			else { /* treat 1st parameter as input buffer */
				if((info->query = StringSave(WWWargv[1])) != NULL) {
					info->entries = WWWGetEntriesSLRI(&info->num_entries, info->query, multipartFormOK, WWWLen);
				}
			}
		}

		*info_out = (VoidPtr) info;
		return WWWErrOk;

	} /* COMMAND_LINE */


	if(info->method == WWW_GET) { /* Getting GET data */
		info->query = StringSave(getenv("QUERY_STRING"));
	}
	else if (info->method == WWW_POST) { /* Getting POST data */
		if((getenv("CONTENT_LENGTH") != NULL) && (WWWLen = atol(getenv("CONTENT_LENGTH"))) > 0) {
			if((info->query = WWWReadFileInMemory(binStdIn, WWWLen, FALSE)) == NULL)
				return WWWErrNetwork;
		}
	} /* Method == POST */  
  
	if(info->query != NULL) {
		/*determine multipart browser status*/
/*		if(StringStr(info->agent,"Mozilla/2")||StringStr(info->agent,"Mozilla/3")||StringStr(info->agent,"Mozilla/4")) {*/
		multipartFormOK = TRUE;
/*		}*/
		info->entries = WWWGetEntriesSLRI(&info->num_entries, info->query, multipartFormOK, WWWLen);
	}

	*info_out = (VoidPtr) info;
	FileClose(binStdIn);
	return WWWErrOk;
}

void WWWGetWordSLRI(CharPtr word, CharPtr line, Char stop) {
  register Int4 x = 0,y =0;
  
  for(x=0;((line[x]) && (line[x] != stop));x++)
    word[x] = line[x];
  
  word[x] = '\0';
  
  if(line[x]) ++x;
  
  while((line[y++] = line[x++]) != NULLB)
	  continue;
}

void PlusToSpaceSLRI(CharPtr str) {
    register Int4 x;

    for(x=0; str[x]; x++) if(str[x] == '+') 
      str[x] = ' ';
}

Char WWWx2cSLRI(CharPtr what) {
    register Char digit;

    digit = (Char) (what[0] >= 'A' ? ((what[0] & 0xdf) - 'A')+10 : (what[0] - '0'));
    digit *= 16;
    digit += (Char) (what[1] >= 'A' ? ((what[1] & 0xdf) - 'A')+10 : (what[1] - '0'));

    return(digit);
}

void WWWUnescapeUrlSLRI(CharPtr url) {
    register Int4 x,y;

    for(x=0,y=0;url[y];++x,++y) {
        if((url[x] = url[y]) == '%') {
            url[x] = WWWx2cSLRI(&url[y+1]);
            y+=2;
        }
    }
    url[x] = '\0';
}

/* ----------------------  WWWGetEntriesSLRI  -------------------------
   Purpose:     Assuming, that input buffer is in HTTP or RFC 1867
                this function converts input into array of name, value
                pairs in the form of WWWEntry -es.
   Parameters:  num_entries - number of paires returned
                WWWBuffer   - main input HTTP buffer
                multipartFormOK - if TRUE check for RFC 1867 will 
                be performed before standard processing

   Returns:     Pointer to array of WWWEntry pairs
   NOTE:        RFC 1867 may be enabled only with Netscape Version 2 and 
                higher.
  ------------------------------------------------------------------*/
WWWEntryPtr PNTR WWWGetEntriesSLRI(Int4Ptr num_entries, CharPtr WWWBuffer_in, Boolean multipartFormOK, Int4 WWWLen)
{
  register Int4 i;
  Int4 size;
  CharPtr WWWBuffer;
  WWWEntryPtr PNTR entries;

  if(WWWBuffer_in == NULL || WWWBuffer_in[0] == NULLB)
    return NULL;
  
  if ( !(entries = (WWWEntryPtr*)MemNew(sizeof(WWWEntryPtr)*MAX_WWW_ENTRIES)) )
    return NULL;
  
  if(multipartFormOK && StringStr(WWWBuffer_in, 
                             "Content-Disposition: form-data;") != NULL) {
    *num_entries = WWWGetEntriesFormDataSLRI(entries, WWWBuffer_in, WWWLen); 
    return entries;
  }

  WWWBuffer = StringSave(WWWBuffer_in); /* this copy may be used for logs */
  size = StringLen(WWWBuffer) + 1;
  
  for(i=0; WWWBuffer[0] != NULLB; i++) {
    entries[i] = (WWWEntryPtr) MemNew(sizeof(WWWEntry)); 
    entries[i]->name = (CharPtr) MemNew(WWW_MAX_NAME_LEN); 
    entries[i]->val  = (CharPtr) MemNew(size);  
    
    WWWGetWordSLRI(entries[i]->val,WWWBuffer,'&');
    PlusToSpaceSLRI(entries[i]->val); 
    WWWUnescapeUrlSLRI(entries[i]->val);  
    WWWGetWordSLRI(entries[i]->name,entries[i]->val,'=');
    
    entries[i]->name = (CharPtr) Realloc(entries[i]->name, 
                                         StringLen(entries[i]->name)+1); 
    entries[i]->val  = (CharPtr) Realloc(entries[i]->val, 
                                         StringLen(entries[i]->val)+1);  
  }

  ASSERT ( i < MAX_WWW_ENTRIES );
  entries[i] = (WWWEntryPtr) MemNew(sizeof(WWWEntry));  
  entries[i]->name = NULL;
  entries[i]->val  = NULL;

  MemFree(WWWBuffer);
  *num_entries = i; 
  return entries;
}

/* --------------------  WWWGetEntriesFormDataSLRI  -----------------------
   Purpose:     Assuming, that input buffer is in RFC 1867
                ftp://ds.internic.net/rfc/rfc1867.txt  
                (multipart/form-data) encoding this function 
                converts input into array of name, value pairs 
                in the form of WWWEntry -es.

   IMPORTANT:		Files are assumed to be terminated by '\r\n' so that
   							the boundary always starts on a new line
   Returns:     Number of WWW entries returned
   NOTE:        RFC 1867 may be enabled only with Netscape Version 2 and 
                higher and MSIE 3 and higher (Any Mozilla/2 compatible browser).
  ------------------------------------------------------------------*/
Int4 WWWGetEntriesFormDataSLRI(WWWEntryPtr PNTR entries, CharPtr WWWBuffer, Int4 WWWLen)
{
	Int4 FieldLen=0;
	register Int4 i, j;
	Char BoundaryString[512];
	CharPtr FieldValue=NULL, FieldTmp=NULL;
	CharPtr WWWBufferOrig;
	CharPtr NextString=NULL, bufTmp=NULL, nameTmp=NULL, fileNameTmp=NULL, mimeTypeTmp=NULL, bsp2StrTmp=NULL, testStrPresent=NULL;
	ByteStorePtr bsp=NULL, bsp2=NULL;
/*	Char dumpf[PATH_MAX];
	FILE *fp; */
	
	if(WWWBuffer == NULL) {
		return -1;
	}
	WWWBufferOrig=WWWBuffer;
	/*determine boundary string*/
	for(i=0; !isspace(WWWBuffer[i]); i++) {
		BoundaryString[i] = WWWBuffer[i];
		if(i>=512) {
			ErrPostEx(SEV_ERROR, 0, 0, "WWWGetEntriesFormDataSLRI: BoundaryString could not be determined (Programmer: Try increasing buffer size).");
			break;
		}
	}
	BoundaryString[i] = NULLB;
	i=-1;
	while (WWWBuffer-WWWBufferOrig<=WWWLen) {

		i++;
		/*check and move to the boundary string*/
		if((WWWBuffer=StringStr(WWWBuffer, BoundaryString)) == NULL) {
			break;
		}
		/*check for end of post (boundary string followed by --)*/
		if((*(WWWBuffer + StringLen(BoundaryString)) == '-') && (*(WWWBuffer + StringLen(BoundaryString) + 1) == '-')) {
			break;
		}
		/*skip up until start of "form-data;"*/
		if((WWWBuffer = StringStr(WWWBuffer , "form-data;")) == NULL) {
			break;
		}
		entries[i] = (WWWEntryPtr) MemNew(sizeof(WWWEntry));

		/*skip to end of "form-data;"+1*/
		WWWBuffer += 11;

		/*check for file upload - but only on the current line (until \n), not the whole query*/
		bsp = BSNew(128);
		/*BSSeek(bsp2, 0L, SEEK_SET);*/
		for(j=0;WWWBuffer[j] != '\n';j++) {
			BSWrite(bsp, &WWWBuffer[j], 1);
		}
		bsp2StrTmp = BSStringSave(bsp);
		bsp = BSFree(bsp);
		if(StringStr(bsp2StrTmp, "\"; filename=\"") != NULL) {
			/*file upload detected*/
			bsp2StrTmp = MemFree(bsp2StrTmp);
			/*get input field name*/
			bufTmp = (CharPtr) MemNew((size_t) (WWW_MAX_NAME_LEN+1024)*sizeof(Char));
			sscanf(WWWBuffer, "name=\"%s ", IsNullStr(bufTmp));
			/*format name*/
			if(bufTmp[StringLen(bufTmp)-1] != ';') {
				bufTmp[StringLen(bufTmp)-1] = NULLB;
			}
			else {
				bufTmp[StringLen(bufTmp)-2] = NULLB;
			}
			/* nameTmp holds the name= field */
			nameTmp = StringSave(bufTmp);
			bufTmp[0] = '\0';
			/*skip to start of filename string*/
			if((WWWBuffer = StringStr(WWWBuffer, "\"; ")) == NULL) {
				ErrPostEx(SEV_ERROR, 0, 0, "WWWGetEntriesFormDataSLRI: Premature end of file upload (at filename).");
				break;
			}
			WWWBuffer += 3;
			/*get filename - only save filename, not path*/
			sscanf(WWWBuffer, "filename=\"%s", IsNullStr(bufTmp));
			/* remove trailing quote */
			bufTmp[StringLen(bufTmp)-1] = NULLB;
			/* fileNameTmp stores filename= field */
			fileNameTmp = StringSave(bufTmp);
			bufTmp[0] = '\0';
			/*skip to start of mime type string - mime type might not be present*/
			if((testStrPresent = StringStr(WWWBuffer, "Content-Type: ")) != NULL) {
				WWWBuffer = testStrPresent;
				WWWBuffer += 14;
				/*get mime type*/
				sscanf(WWWBuffer, "%s", IsNullStr(bufTmp));
				/* for example, application/octet-stream */
				mimeTypeTmp = StringSave(bufTmp);
				bufTmp[0] = '\0';
				/*skip to start of file*/
				if((WWWBuffer = StringStr(WWWBuffer, mimeTypeTmp)) == NULL) {
					ErrPostEx(SEV_ERROR, 0, 0, "WWWGetEntriesFormDataSLRI: Premature end of file upload (at mime type).");
					break;
				}
				WWWBuffer += (StringLen(mimeTypeTmp)+4);	/*4 = /r/n/r/n*/
			}
			else {
				/*skip to start of file*/
				if((WWWBuffer = StringStr(WWWBuffer, fileNameTmp)) == NULL) {
					ErrPostEx(SEV_ERROR, 0, 0, "WWWGetEntriesFormDataSLRI: Premature end of file upload (at file).");
					break;
				}
				WWWBuffer += (StringLen(fileNameTmp)+5);	/*5 = "/r/n/r/n*/
			}
			/*get file*/
			bsp = BSNew(1024);
			bsp2 = BSNew(1024);
			while (WWWBuffer-WWWBufferOrig<=WWWLen) {
				/*initialize ByteStore*/
				BSSeek(bsp2, 0L, SEEK_SET);
				BSDelete(bsp2, BSLen(bsp2));
				/*read up until \n*/
				for(;*WWWBuffer != '\n';WWWBuffer++) {
					BSWrite(bsp2, WWWBuffer, 1);
					if (WWWBuffer-WWWBufferOrig>WWWLen)
						break;
				}
				if (WWWBuffer-WWWBufferOrig>WWWLen)
					break;
				BSPutByte(bsp2, '\n');
				bsp2StrTmp = BSStringSave(bsp2);
				if(StringStr(bsp2StrTmp, BoundaryString) != NULL) {
					/*remove "\r\n" from bsp*/
					BSSeek(bsp, -2L, SEEK_CUR);
					BSDelete(bsp, 2);
					bsp2StrTmp = MemFree(bsp2StrTmp);
					/* backup to start of boundary */
					WWWBuffer=WWWBuffer+1-BSLen(bsp2);
					break;
				}
				else {
					BSWrite(bsp, bsp2StrTmp, BSLen(bsp2));
					WWWBuffer ++;
				}
				bsp2StrTmp = MemFree(bsp2StrTmp);
			}
			bsp2 = BSFree(bsp2);
			if (WWWBuffer-WWWBufferOrig>WWWLen) {
					ErrPostEx(SEV_ERROR, 0, 0, "WWWGetEntriesFormDataSLRI: Attempt to read past end of data");
					/* dump data for inspection */
/*					sprintf(dumpf,"errordump%ld",(long)GetAppProcessID());
					fp=FileOpen(dumpf,"wb");
					FileWrite(WWWBufferOrig,sizeof(Char),WWWLen,fp);
					FileClose(fp);
					sprintf(dumpf,"error2dump%ld",(long)GetAppProcessID());
					fp=FileOpen(dumpf,"wb");
					FileWrite(WWWBufferOrig,sizeof(Char),WWWLen*2,fp);
					FileClose(fp);
*/					if (bsp!=NULL)
						bsp = BSFree(bsp);
					if (bsp2!=NULL)
						bsp2 = BSFree(bsp2);
					if (bufTmp!=NULL)
						bufTmp = MemFree(bufTmp);
					if (nameTmp!=NULL)
						nameTmp = MemFree(nameTmp);
					if (fileNameTmp!=NULL)
						fileNameTmp = MemFree(fileNameTmp);
					if (mimeTypeTmp!=NULL)
						mimeTypeTmp = MemFree(mimeTypeTmp);
					break;
			}
			/*build name*/
			if(mimeTypeTmp==NULL) {
				mimeTypeTmp = StringSave("Mime type not specified by browser");
			}
			sprintf(bufTmp, "name=\"%s\";filename=\"%s\";mimetype=\"%s\";filelength=%ld", IsNullStr(nameTmp), IsNullStr(fileNameTmp), IsNullStr(mimeTypeTmp), (long) BSLen(bsp));
			entries[i]->name = StringSave(bufTmp);
			bufTmp = MemFree(bufTmp);
			nameTmp = MemFree(nameTmp);
			fileNameTmp = MemFree(fileNameTmp);
			mimeTypeTmp = MemFree(mimeTypeTmp);
			/*value - file*/
			entries[i]->val = BSStringSave(bsp);
			bsp = BSFree(bsp);
			continue;
		}
		else {
			bsp2StrTmp = MemFree(bsp2StrTmp);
		}

		/*free memory*/
		if (bsp!=NULL)
			bsp = BSFree(bsp);
		if (bsp2!=NULL)
			bsp2 = BSFree(bsp2);
		if (bufTmp!=NULL)
			bufTmp = MemFree(bufTmp);
		if (nameTmp!=NULL)
			nameTmp = MemFree(nameTmp);
		if (fileNameTmp!=NULL)
			fileNameTmp = MemFree(fileNameTmp);
		if (mimeTypeTmp!=NULL)
			mimeTypeTmp = MemFree(mimeTypeTmp);

		if((NextString = StringStr(WWWBuffer, BoundaryString)) == NULL) {
			break;
		}
		FieldLen = NextString - WWWBuffer - 1;

		FieldValue = (CharPtr) MemNew(FieldLen+1);
		MemCopy(FieldValue, WWWBuffer, FieldLen);
		FieldValue[FieldLen] = NULLB;

		nameTmp = (CharPtr) MemNew(WWW_MAX_NAME_LEN);
		sscanf(FieldValue, "name=\"%s ", IsNullStr(nameTmp));
		entries[i]->name = StringSave(nameTmp);
		nameTmp = MemFree(nameTmp);

		if(entries[i]->name[StringLen(entries[i]->name)-1] != ';') {
			entries[i]->name[StringLen(entries[i]->name)-1] = NULLB;
		}
		else {
			entries[i]->name[StringLen(entries[i]->name)-2] = NULLB;
		}

		if((FieldTmp = StringStr(FieldValue, "\r\n\r\n")) != NULL) {
			FieldTmp = FieldTmp + 4;
			FieldTmp[StringLen(FieldTmp) - 1] = NULLB;
			entries[i]->val = StringSave(FieldTmp);
		}
		else {
			entries[i]->val = StringSave("");
		}
		MemFree(FieldValue);
	}
	if (i<0)
		i=0;

	entries[i] = (WWWEntryPtr) MemNew(sizeof(WWWEntry));
	entries[i]->name = NULL;
	entries[i]->val  = NULL;

	return i;
}

/*get the filename of the file uploaded with given fieldName.
The file name is local to the client computer and could include the path.*/
/*WARNING: allocates memory - you must free the returned pointer*/
CharPtr WWWGetFileName(WWWInfoPtr infoIn, CharPtr fieldName)
{
	WWWEntryPtr PNTR entries=NULL;
	Int4 numEntries=0, i=0;
	CharPtr searchString=NULL, entryName=NULL, fileName=NULL, strTmp=NULL;
	WWWInfoDataPtr info = (WWWInfoDataPtr) infoIn;

	if((fieldName == NULL)||(info==NULL))
		return(NULL);

	searchString = MemNew((size_t) (32+StringLen(fieldName))*sizeof(Char));
	sprintf(searchString, "name=\"%s\";", IsNullStr(fieldName));

	/*find file entry*/
	entries = info->entries;
	numEntries = info->num_entries;
	for(i=0;i<numEntries;i++) {
		entryName = entries[i]->name;
		if(StringStr(entryName, searchString) != NULL) {
			if((entryName = StringStr(entryName, "\";filename=\"")) == NULL) {
				searchString = MemFree(searchString);
				return(NULL);
			}
			entryName += 12;
			fileName = StringSave(entryName);
			/*terminate properly*/
			if((strTmp = StringStr(fileName, "\";")) == NULL) {
				searchString = MemFree(searchString);
				fileName = MemFree(fileName);
				return(NULL);
			}
			fileName[strTmp - fileName] = NULLB;
			break;
		}
	}

	searchString = MemFree(searchString);
	return(fileName);
}

/*get the browser given mime-type of the file uploaded with given fieldName*/
/*WARNING: allocates memory - you must free the returned pointer*/
CharPtr WWWGetFileMimeType(WWWInfoPtr infoIn, CharPtr fieldName)
{
	WWWEntryPtr PNTR entries=NULL;
	Int4 numEntries=0, i=0;
	CharPtr searchString=NULL, entryName=NULL, mimeType=NULL, strTmp=NULL;
	WWWInfoDataPtr info = (WWWInfoDataPtr) infoIn;

	if((fieldName == NULL)||(info==NULL))
		return(NULL);

	searchString = MemNew((size_t) (32+StringLen(fieldName))*sizeof(Char));
	sprintf(searchString, "name=\"%s\";", IsNullStr(fieldName));

	/*find file entry*/
	entries = info->entries;
	numEntries = info->num_entries;
	for(i=0;i<numEntries;i++) {
		entryName = entries[i]->name;
		if(StringStr(entryName, searchString) != NULL) {
			if((entryName = StringStr(entryName, "\";mimetype=\"")) == NULL) {
				searchString = MemFree(searchString);
				return(NULL);
			}
			entryName += 12;
			mimeType = StringSave(entryName);
			/*terminate properly*/
			if((strTmp = StringStr(mimeType, "\";")) == NULL) {
				mimeType = MemFree(mimeType);
				searchString = MemFree(searchString);
				return(NULL);
			}
			mimeType[strTmp - mimeType] = NULLB;
			break;
		}
	}

	searchString = MemFree(searchString);
	return(mimeType);
}

/*get the length in bytes of the file uploaded with given fieldName*/
Int4 WWWGetFileLength(WWWInfoPtr infoIn, CharPtr fieldName)
{
	WWWEntryPtr PNTR entries=NULL;
	Int4 numEntries=0, i=0, fileLen=0;
	CharPtr searchString=NULL, entryName=NULL, fileLenStr=NULL;
	WWWInfoDataPtr info = (WWWInfoDataPtr) infoIn;

	if((fieldName == NULL)||(info==NULL))
		return 0;

	searchString = MemNew((size_t) (32+StringLen(fieldName))*sizeof(Char));
	sprintf(searchString, "name=\"%s\";", IsNullStr(fieldName));

	/*find file entry*/
	entries = info->entries;
	numEntries = info->num_entries;
	for(i=0;i<numEntries;i++) {
		entryName = entries[i]->name;
		if(StringStr(entryName, searchString) != NULL) {
			if((entryName = StringStr(entryName, "\";filelength=")) == NULL) {
				searchString = MemFree(searchString);
				return 0;
			}
			entryName += 13;
			fileLenStr = StringSave(entryName);
			if(!Misc_IsNumeric(fileLenStr)) {
				fileLenStr = MemFree(fileLenStr);
				searchString = MemFree(searchString);
				return 0;
			}
			fileLen = atol(fileLenStr);
			fileLenStr = MemFree(fileLenStr);
			break;
		}
	}

	searchString = MemFree(searchString);
	return(fileLen);
}

/*save an uploaded file to a ByteStore with the given fieldName.*/
Int4 WWWFile2BSFromWWWInfo(WWWInfoPtr infoIn, CharPtr fieldName, ByteStorePtr bsp)
{
	WWWEntryPtr PNTR entries=NULL;
	CharPtr fileLenTmp=NULL, entryName=NULL, searchString=NULL;
	Int4 fileLen=0, numEntries=0, i=0;
	Boolean foundFile=FALSE;
	WWWInfoDataPtr info = (WWWInfoDataPtr) infoIn;

	if((fieldName == NULL)||(info==NULL)||(bsp == NULL))
		return(1);

	searchString = MemNew((size_t) (16+StringLen(fieldName))*sizeof(Char));
	sprintf(searchString, "name=\"%s\";", IsNullStr(fieldName));

	/*find file entry*/
	entries = info->entries;
	numEntries = info->num_entries;
	for(i=0;i<numEntries;i++) {
		entryName = entries[i]->name;
		if(StringStr(entryName, searchString) != NULL) {
			/*name*/
			if((entryName = StringStr(entryName, "=\"")) == NULL) {
				searchString = MemFree(searchString);
				return(1);
			}
			entryName += 2;
			/*filelength*/
			if((entryName = StringStr(entryName, "\";filelength=")) == NULL) {
				searchString = MemFree(searchString);
				return(1);
			}
			entryName += 13;
			fileLenTmp = StringSave(entryName);
			if(!Misc_IsNumeric(fileLenTmp)) {
				fileLenTmp = MemFree(fileLenTmp);
				searchString = MemFree(searchString);
				return(1);
			}
			fileLen = atol(fileLenTmp);
			fileLenTmp = MemFree(fileLenTmp);
			foundFile = TRUE;
			break;
		}
	}

	if(foundFile) {
		/*write to bytestore*/
		BSSeek(bsp, 0L, SEEK_SET);
		if(BSLen(bsp) != 0)
			BSDelete(bsp, BSLen(bsp));
		BSWrite(bsp, entries[i]->val, fileLen);
	}

	searchString = MemFree(searchString);

	return(0);
}

/*write an uploaded file to disk with the given fieldName.  An optional path to write to may be specified*/
Int4 WWWWriteFileFromWWWInfo(WWWInfoPtr infoIn, CharPtr fieldName, CharPtr path)
{
	WWWEntryPtr PNTR entries=NULL;
	FILE *fp=NULL;
	CharPtr fileLenTmp=NULL, entryName=NULL, searchString=NULL, fileName=NULL;
	Int4 fileLen=0, numEntries=0, i=0;
	Boolean fileNameMemNew=FALSE, foundFile=FALSE;
	WWWInfoDataPtr info = (WWWInfoDataPtr) infoIn;

	if((fieldName == NULL)||(info==NULL))
		return(1);

	searchString = MemNew((size_t) (32+StringLen(fieldName))*sizeof(Char));
	sprintf(searchString, "name=\"%s\";", IsNullStr(fieldName));

	if(path != NULL) {
		fileName = MemNew((size_t) (16+StringLen(fieldName)+StringLen(path))*sizeof(Char));
		fileNameMemNew = TRUE;
		/*check if user has supplied a DIRDELIMCHR at end of path*/
		if(path[StringLen(path)-1] == DIRDELIMCHR)
			sprintf(fileName, "%s%s", IsNullStr(path), IsNullStr(fieldName));
		else
			sprintf(fileName, "%s%s%s", IsNullStr(path), DIRDELIMSTR, IsNullStr(fieldName));
	}
	else
		fileName = fieldName;

	/*find file entry*/
	entries = info->entries;
	numEntries = info->num_entries;
	for(i=0;i<numEntries;i++) {
		entryName = entries[i]->name;
		if(StringStr(entryName, searchString) != NULL) {
			/*name*/
			if((entryName = StringStr(entryName, "=\"")) == NULL) {
				searchString = MemFree(searchString);
				if(fileNameMemNew) fileName = MemFree(fileName);
				return(1);
			}
			entryName += 2;
			/*filelength*/
			if((entryName = StringStr(entryName, "\";filelength=")) == NULL) {
				searchString = MemFree(searchString);
				if(fileNameMemNew) fileName = MemFree(fileName);
				return(1);
			}
			entryName += 13;
			fileLenTmp = StringSave(entryName);
			if(!Misc_IsNumeric(fileLenTmp)) {
				fileLenTmp = MemFree(fileLenTmp);
				searchString = MemFree(searchString);
				if(fileNameMemNew) fileName = MemFree(fileName);
				return(1);
			}
			fileLen = atol(fileLenTmp);
			fileLenTmp = MemFree(fileLenTmp);
			foundFile = TRUE;
			break;
		}
	}

	/*write file*/
	if(foundFile) {
		fp = FileOpen(fileName, "wb");
		FileWrite(entries[i]->val, sizeof(Byte), fileLen, fp);
		FileClose(fp);
	}

	searchString = MemFree(searchString);
	if(fileNameMemNew) fileName = MemFree(fileName);

	return(0);
}

/*write all uploaded files to disk.  An optional path to write to may be specified*/
Int4 WWWWriteAllFilesFromWWWInfo(WWWInfoPtr infoIn, CharPtr path)
{
	WWWEntryPtr PNTR entries=NULL;
	FILE *fp=NULL;
	CharPtr fieldName=NULL, fileLenTmp=NULL, strTmp=NULL, entryName=NULL, fileName=NULL;
	Int4 fileLen=0, numEntries=0, i=0;
	Boolean fileNameMemNew=FALSE;
	WWWInfoDataPtr info = (WWWInfoDataPtr) infoIn;

	if(info==NULL)
		return(1);

	/*find file entry*/
	entries = info->entries;
	numEntries = info->num_entries;
	for(i=0;i<numEntries;i++) {
		entryName = entries[i]->name;
		if(StringStr(entryName, "\";filename=\"") != NULL) {
			/*name*/
			if((entryName = StringStr(entryName, "name=\"")) == NULL)
				return(1);
			entryName += 6;
			fieldName = StringSave(entryName);
			/*terminate properly*/
			if((strTmp = StringStr(fieldName, "\";")) == NULL) {
				fieldName = MemFree(fieldName);
				return(1);
			}
			fieldName[strTmp - fieldName] = NULLB;
			/*filelength*/
			if((entryName = StringStr(entryName, "\";filelength=")) == NULL) {
				fieldName = MemFree(fieldName);
				return(1);
			}
			entryName += 13;
			fileLenTmp = StringSave(entryName);
			if(!Misc_IsNumeric(fileLenTmp)) {
				fileLenTmp = MemFree(fileLenTmp);
				fieldName = MemFree(fieldName);
				return(1);
			}
			fileLen = atol(fileLenTmp);
			fileLenTmp = MemFree(fileLenTmp);
			/*build filename*/
			if(path != NULL) {
				fileName = MemNew((size_t) (16+StringLen(fieldName)+StringLen(path))*sizeof(Char));
				fileNameMemNew = TRUE;
				/*check if user has supplied a DIRDELIMCHR at end of path*/
				if(path[StringLen(path)-1] == DIRDELIMCHR)
					sprintf(fileName, "%s%s", IsNullStr(path), IsNullStr(fieldName));
				else
					sprintf(fileName, "%s%s%s", IsNullStr(path), DIRDELIMSTR, IsNullStr(fieldName));
			}
			else
				fileName = fieldName;
			/*write file*/
			fp = FileOpen(fileName, "wb");
			FileWrite(entries[i]->val, sizeof(Byte), fileLen, fp);
			FileClose(fp);
			if(fileNameMemNew) fileName = MemFree(fileName);
			fieldName = MemFree(fieldName);
		}
	}

	return(0);
}

WWWInfoPtr WWWInfoNewSLRI(void)
{
  return (WWWInfoPtr) MemNew(sizeof(WWWInfoData));
}

Boolean WWWReadEnvironmentSLRI(WWWInfoDataPtr info)
{
  CharPtr Method;

  if(!info) return 0;
  
  info->method = COMMAND_LINE;
  
  if((Method = getenv("REQUEST_METHOD")) != NULL) {

    if(!StringICmp(Method, "GET")) {    
      info->method = WWW_GET;
    } else if (!StringICmp(Method, "POST")) {
      info->method= WWW_POST;
    }
  }

  if((info->host = StringSave(getenv("REMOTE_HOST"))) == NULL) {
    info->host = StringSave("Host unknown");
  }
  if((info->address = StringSave(getenv("REMOTE_ADDR"))) == NULL) {
    info->address =StringSave("Address unknown");
  }
  if((info->doc_root = StringSave(getenv("DOCUMENT_ROOT"))) == NULL) {
    info->doc_root =StringSave("_unknown_");
  }
  if((info->agent = StringSave(getenv("HTTP_USER_AGENT"))) == NULL) {
    info->agent =StringSave("Agent unknown"); 
  }
  if((getenv("SERVER_PORT") == NULL) || 
	  (info->port = atol(getenv("SERVER_PORT"))) == 0) {
    info->port = -1; 
  }
  if((info->server_name = StringSave(getenv("SERVER_NAME"))) == NULL) {
    info->server_name = StringSave("Server unknown"); 
  }
  if((info->script_name = StringSave(getenv("SCRIPT_NAME"))) == NULL) {
    info->script_name = StringSave("Script unknown"); 
  }
  
  info->browser = MISC_BROWSER;
  
  if(StringStr(info->agent, "Mozilla/2") ||
     StringStr(info->agent, "Mozilla/3") ||
     StringStr(info->agent, "Mozilla/4")) {
    info->browser = NETSCAPE;
  }

  if(StringStr(info->agent, "MSIE") ||
	  StringStr(info->agent, "Microsoft")) {
    info->browser = EXPLORER;
  }

  if(info->method == WWW_POST || info->method == WWW_GET) {
    return TRUE;
  }
  else {
    return FALSE;
  }
}

/***********/
/* WWW CGI */
/***********/

/*records an HTML error in the HTML error buffer (vnpError)*/
Int4 SetHTMLError(ValNodePtr *vnpError, CharPtr formatErrorStringHTML, ...)
{
	va_list args;
	CharPtr buf=NULL, errorTxt=NULL;

	if(formatErrorStringHTML == NULL) {
		return(1);
	}

	va_start(args, formatErrorStringHTML);
	buf = (CharPtr) TSPrintfArgs(formatErrorStringHTML, args);
	va_end(args);
	if(buf != NULL) {
		errorTxt = StringSave(buf);	/*must use buf right away - no guarantee that it won't be used by something else*/
	}
	ValNodeAddPointer(vnpError, 0, errorTxt);

	return(0);
}

/*returns the integer value of CGI argument name, if this name is not found in the query,
errorStringHTML is saved in vnpError and the default value (defVal) is returned.
If setErrForMissingVal is FALSE, caller must take care of errors for the case where
the argument name is present but no value has been passed*/
Int4 GetIntegerArg(WWWInfoPtr info, CharPtr argName, CharPtr errorStringHTML, ValNodePtr *vnpError, Int4 defVal,
				   Boolean setErrForMissingVal)
{
	Int4 IndexArgs=-1, argVal=0;
	CharPtr pcThis=NULL, stopstring=NULL;
	CharPtr errorString=NULL;

	if((info == NULL)||(argName==NULL)) {
		return(defVal);
	}

	errorString = MemNew((size_t) (64+StringLen(argName))*sizeof(Char));

	if((IndexArgs = WWWFindName(info, argName)) >= 0) {
		pcThis = WWWGetValueByIndex(info,IndexArgs);
		if(StringCmp(pcThis, "") != 0) {
			/*test pcThis for alpha characters*/
			if(!Misc_IsNumeric(pcThis)) {
				sprintf(errorString, "<H3>'%s' argument requires integer input.</H3>\n", IsNullStr(argName));
				SetHTMLError(vnpError, "%s", IsNullStr(errorString));
				errorString=MemFree(errorString);
				return(defVal);
			}
			argVal = strtol(pcThis, &stopstring, 10);
			errorString=MemFree(errorString);
			return(argVal);
		}
	}
	else {	/*argName was not found in the passed argument string*/
		if(errorStringHTML!=NULL) {
			SetHTMLError(vnpError, "%s", IsNullStr(errorStringHTML));
		}
		errorString=MemFree(errorString);
		return(defVal);
	}

	/*argName was found in the passed argument string, but not properly set*/
	if((setErrForMissingVal)&&(errorStringHTML!=NULL)) {
		SetHTMLError(vnpError, "%s", IsNullStr(errorStringHTML));
	}

	errorString = MemFree(errorString);

	return(defVal);
}

/*returns the character value of CGI argument name, if this name was not found,
errorString is saved in vnpError
If setErrForMissingVal is FALSE, caller must take care of errors for the case where
the argument name is present but no value has been passed*/
CharPtr GetTextArg(WWWInfoPtr info, CharPtr argName, CharPtr errorStringHTML, ValNodePtr *vnpError, CharPtr defVal,
				   Boolean setErrForMissingVal)
{
	Int4 IndexArgs = -1;
	CharPtr pcThis=NULL;
	CharPtr errorString=NULL;

	if((info == NULL)||(argName==NULL)) {
		return(defVal);
	}

	errorString = MemNew((size_t) (64+StringLen(argName))*sizeof(Char));

	if((IndexArgs = WWWFindName(info, argName)) >= 0) {
		pcThis = WWWGetValueByIndex(info,IndexArgs);
		if(StringCmp(pcThis, "") != 0) {
			errorString=MemFree(errorString);
			return(pcThis);
		}
	}
	else {	/*argName was not found in the passed argument string*/
		if(errorStringHTML!=NULL) {
			SetHTMLError(vnpError, "%s", IsNullStr(errorStringHTML));
		}
		errorString=MemFree(errorString);
		return(defVal);
	}

	/*argName was found in the passed argument string, but not properly set*/
	if((setErrForMissingVal)&&(errorStringHTML!=NULL)) {
		SetHTMLError(vnpError, "%s", IsNullStr(errorStringHTML));
	}

	errorString = MemFree(errorString);

	return(defVal);
}

/*prints the HTML CGI error buffer.*/
void PrintHTMLErrBuf(ValNodePtr vnpError)
{
	if(vnpError == NULL) {
		return;
	}

	printf("<FONT color=\"red\">\n");
	while(vnpError != NULL) {
		if(vnpError->data.ptrvalue!=NULL) {
			printf("%s", IsNullStr((CharPtr) vnpError->data.ptrvalue));
		}
		vnpError=vnpError->next;
	}
	printf("</FONT>\n");

	fflush(stdout);
}

/*logs the HTML CGI error buffer to the log file.
Will only log if NCBI logging has been enabled with the following calls
	if( !ErrSetLogfile (LogFile, ELOG_APPEND) ) {
		printf("Content-type: text/html\r\n\r\n");
		printf("<H2>Error</H2>\n - Unable to Open Log - contact %s\n", Cfg_GetMailTo());
		exit(1);
	}

	ErrSetOptFlags(EO_LOGTO_USRFILE);
*/
Int2 LogHTMLErrBuf(ValNodePtr vnpError)
{
	if(vnpError == NULL) {
		return(1);
	}

	while(vnpError != NULL) {
		ErrLogPrintf(vnpError->data.ptrvalue);
		vnpError=vnpError->next;
	}

	return(0);
}

/*frees the HTML CGI error buffer*/
ValNodePtr FreeHTMLErrBuf(ValNodePtr vnpError)
{
	ValNodePtr vnpTmp=NULL;

	if(vnpError == NULL)
		return NULL;

	vnpTmp = vnpError;
	while(vnpTmp != NULL) {
		MemFree(vnpTmp->data.ptrvalue);
		vnpTmp=vnpTmp->next;
	}

	return(ValNodeFree(vnpError));
}

/*returns a new string that represents the escaped version of the given string - CGI escape rules*/
CharPtr WWW_HexEscapeString(CharPtr string)
{
	CharPtr retWord=NULL;
	ByteStorePtr bsp=NULL;
	Int4 len=0, i=0;
	Char buf[10];

	if(string==NULL) {
		return(NULL);
	}

	len = StringLen(string);
	if(len==0) {
		return(NULL);
	}

	bsp=BSNew(16);
	BSSeek(bsp, 0L, SEEK_SET);

	for(i=0;i<len;i++) {
		if(string[i]==' ') {
			BSPutByte(bsp, '+');
		}
		else if(string[i]=='-') {
			BSWrite(bsp, "%2d", 3);
		}
		else if(string[i]=='+') {
			BSWrite(bsp, "%2b", 3);
		}
		else if(string[i]=='*') {
			BSWrite(bsp, "%2a", 3);
		}
		else if((!IS_ALPHA(string[i]))&&(!IS_DIGIT(string[i]))) {
			sprintf(buf, "%%%x", string[i]);
			BSWrite(bsp, buf, strlen(buf));
		}
		else {
			BSPutByte(bsp, string[i]);
		}
	}

	retWord = BSStringSave(bsp);
	bsp = BSFree(bsp);

	return(retWord);
}

/*returns a new string that represents the escaped version of the given string - HTML escape rules*/
CharPtr WWW_HTMLEscapeString(CharPtr string)
{
	CharPtr retWord=NULL;
	ByteStorePtr bsp=NULL;
	Int4 len=0, i=0;

	if(string==NULL) {
		return(NULL);
	}

	len = StringLen(string);
	if(len==0) {
		return(NULL);
	}

	bsp=BSNew(16);
	BSSeek(bsp, 0L, SEEK_SET);

	for(i=0;i<len;i++) {
		switch(string[i]) {
		case '<':
			BSWrite(bsp, "&lt;", 4);
			break;
		case '>':
			BSWrite(bsp, "&gt;", 4);
			break;
		case '&':
			BSWrite(bsp, "&amp;", 5);
			break;
		case '"':
			BSWrite(bsp, "&quot;", 6);
			break;
		default:
			BSWrite(bsp, &(string[i]), 1);
		}
	}

	retWord = BSStringSave(bsp);
	bsp = BSFree(bsp);

	return(retWord);
}

/*Printf that will print out text in an HTML safe way (i.e. encodes characters <>&")
Returns the number of bytes printed*/
/*Encodes insecure cross-site scripting vulnerable characters. See (http://www.cert.org/advisories/CA-2000-02.html)*/
Int4 HTMLPrintf(CharPtr format, ...)
{
	CharPtr buf=NULL, buf2=NULL, output=NULL;
	Int4 len=0;
	va_list args;

	if(format==NULL) {
		return(0);
	}

	va_start(args, format);
	buf = (CharPtr) TSPrintfArgs(format, args);
	va_end(args);
	if(buf != NULL) {
		/*you must use TSPrintfArgs return value right away - scratch buffer may be used for other tasks*/
		buf2 = StringSave(buf);
	}
	else {
		return(0);
	}

	output = WWW_HTMLEscapeString(buf2);
	buf2 = MemFree(buf2);
	len = printf("%s", IsNullStr(output));
	output = MemFree(output);

	return(len);
}

#define XMLSUBS 4
static CharPtr xmlsubstrs[XMLSUBS] = { "&amp;","&lt;","&gt;","&quot;" };

/*prints out NCBI generated XML as HTML
(Won't encode already encoded characters in the set <>&" and will change &apos; to ')*/
Int4 XML2HTMLPrintf(CharPtr format, ...)
{
	CharPtr buf, buf2, buf2Head, output;
	va_list args;
	Int4 len, i, j;
	ByteStorePtr bsp;
	Char encoding[7];
	Boolean encoded;

	if(format==NULL) {
		return(0);
	}

	va_start(args, format);
	buf = (CharPtr) TSPrintfArgs(format, args);
	va_end(args);
	if(buf != NULL) {
		/*you must use TSPrintfArgs return value right away - scratch buffer may be used for other tasks*/
		buf2 = StringSave(buf);
		buf2Head = buf2;
	}
	else {
		return(0);
	}

	len=StringLen(buf2);
	bsp = BSNew(len);
	for(i=0;i<len;i++) {
		switch(buf2[0]) {
		case '<':
			BSWrite(bsp, "&lt;", 4);
			break;
		case '>':
			BSWrite(bsp, "&gt;", 4);
			break;
		case '&':
			/*before encoding, check if it is already encoded*/
			strncpy(encoding, buf2, 6);
			encoding[6]='\0';
			encoded = FALSE;
			if(strstr(encoding, "&apos;")) {
				/*replace XML &apos; with an apostrophe for HTML*/
				BSWrite(bsp, "'", 1);
				buf2 += 5;
			}
			else {
				/*don't encode already encoded characters*/
				for(j=0;j<XMLSUBS;j++) {
					if(strstr(encoding, xmlsubstrs[j])) {
						encoded=TRUE;
						break;
					}
				}
				if(!encoded) {
					BSWrite(bsp, "&amp;", 5);
				}
				else {
					BSWrite(bsp, buf2, 1);
				}
			}
			break;
		case '\'':
			BSWrite(bsp, "&apos;", 6);
			break;
		case '"':
			BSWrite(bsp, "&quot;", 6);
			break;
		default:
			BSWrite(bsp, buf2, 1);
		}
		buf2++;
	}

	buf2Head = MemFree(buf2Head);
	output = BSStringSave(bsp);
	printf("%s", IsNullStr(output));
	output = MemFree(output);

	len = BSLen(bsp);
	bsp = BSFree(bsp);

	return(len);
}

/*Environment test*/

/*test the file system environment to make sure process has read and write permissions on files*/
/*Call this function from the beginning of any CGI program.*/
void CGI_TestFileSystem(void)
{
	FILE *test=NULL;
	CharPtr testName=NULL;

	testName = MemNew((size_t) PATH_MAX*sizeof(Char));
	Mem_Check(testName);

	sprintf(testName, "%s.tmp", TmpNam(NULL));

	test = FileOpen (testName, "w+");
	if(test == NULL) {
		printf("Content-type: text/html\r\n\r\n");
		printf("<H2>Error</H2>\n");
		printf("This CGI program does not have read/write access to the server disk."
			" It is possible that file permissions are not correctly set or that the"
			" server hard disk is full.\n");
		printf("<P>\nPlease report this problem to the web site administrator.\n");
		exit(1);
	}

	FileClose(test);
	FileRemove(testName);
	testName = MemFree(testName);
}

/*
$Log: slri_www.c,v $
Revision 1.2  2003/10/16 20:59:50  skuznets

1. Checking for NULL pointers when print values
2. Added function Misc_TrimSpacesAroundString
3. Added macro IsNullStr

Revision 1.1.1.6  2002/03/15 09:27:27  root
Automated import from Sourceforge.net

Revision 1.7  2002/03/14 15:48:23  hfeldman
moved wwwuploadtextfiles to taskapi for now

Revision 1.6  2002/03/11 22:19:49  hfeldman
rewrote a bit so basic proxy works ok in WWWUploadTextFiles

Revision 1.4  2002/03/07 23:07:13  micheld
Added user_header option for uploadtextfiles

Revision 1.3  2002/03/01 23:57:45  gbader
Fixed compiler warnings
Moved generic asn set read/write mode to slri_asn

Revision 1.2  2002/02/25 18:26:20  gbader
Added Howie's rewrite of binary file upload handling.

Revision 1.1.1.3  2001/10/10 22:11:04  kaca
initial import

Revision 1.21  2001/09/26 19:26:05  gbader
Added new escape functions.

Revision 1.20  2001/09/07 01:17:57  gbader
Fixed mem leaks in GetInteger and GetText functions.

Revision 1.19  2001/09/04 19:05:42  feldman
Fixed compiler warnings

Revision 1.18  2001/08/31 18:16:10  feldman
Updated UploadTextFiles to use CONN instead of SOCK interface, higher level now

Revision 1.17  2001/05/25 21:49:00  feldman
Fixed so filenames can have full paths

Revision 1.16  2001/04/10 01:38:06  feldman
File upload via form data should work for binary files now too
(will test)

Revision 1.15  2001/04/08 22:18:31  feldman
fixed compiler warnings

Revision 1.14  2001/04/05 14:12:02  feldman
Fixed a few compiler warnings

Revision 1.13  2001/04/04 21:19:58  feldman
Removed Netscape check for form data file upload (will try regardless
of browser) and added file upload function

Revision 1.12  2001/03/08 00:13:29  gbader
Moved includes to C files from header files to reduce header dependencies and speed compilation

Revision 1.11  2001/01/13 04:04:10  gbader
Added IBM DB2 UDB to license exceptions.

Revision 1.10  2000/10/04 15:08:40  gbader
Fixed binary file upload so that it deals with browser generated content from Linux Netscape (mime type not present)

Revision 1.9  2000/09/22 21:30:55  gbader
Code clean up from -Wall compiler flag on Linux

Revision 1.8  2000/09/13 18:42:54  gbader
Added XML2HTMLPrintf function to print XML as HTML

Revision 1.7  2000/09/09 17:46:39  gbader
Added (long) type casts for all Int4 variables used with ANSI format string functions (e.g. printf) to avoid -Wall warnings on Linux

Revision 1.6  2000/09/05 13:55:08  gbader
Updated License so that it is now GNU.
Added CVS keywords ID and Log as well as new line at end of files so compilers on some platforms don't complain

*/
