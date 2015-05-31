/* $Id: seqhoundapi.c,v 1.42 2005/03/07 20:27:40 ryao Exp $
*******************************************************************************************
  Copyright (C) 2000 Mount Sinai Hospital (MSH)

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of
  the License, or any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

  See the LESSER GNU General Public License for more details.

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


  AUTHORS:  Katerina Michalickova
            Michel Dumontier 
	    Hao Lieu
	    Christopher W.V. Hogue hogue@mshri.on.ca


  REVISION/CONTRIBUTION NOTES:

  June 1, 2000  Original MSH Revision.
                Hogue Lab - University of Toronto Biochemistry
                Department and the Samuel Lunenfeld Research
                Institute, Mount Sinai Hospital
                http://bioinfo.mshri.on.ca  hogue@mshri.on.ca
                ftp://ftp.mshri.on.ca/


  PUBLICATION TO CITE:


  DESCRIPTION
  January 2000
  remote SeqHound API

    June 2000
    changed the way the data is passed from the wwwgetfile function to the API function -
    change from a file to byte store
    June 2000
    split queries to chunks managable by apache server (maximum query directive - 8190 Bytes)

******************************************************************************************/

#include <seqhoundapi.h>
#include <intrez_misc.h>


static CharPtr slri_sername = NULL;
static Boolean isInited = FALSE;
static Boolean NetEntrez = FALSE;
static ByteStorePtr replyBS;
static Int2       LIBCALL ParseSeqHoundError(CharPtr pline);
static Boolean    LIBCALL ReplyBSTrueorFalse(void);
static ValNodePtr LIBCALL ReplyBSGetValNode(Boolean IsInteger);
static Int4       LIBCALL ReplyBSGetInteger(void);
static CharPtr    LIBCALL ReplyBSGetString(void);
static Int2 SHoundWWWGetfile(CharPtr host, CharPtr url);
static CharPtr slri_cgipath;

/* number of query identifiers to send to the server at the time */
#define MAXQ 800
#define MAXNBRQ 400
/* number of bytes for the arguments to be passed to server at one time */
#define ARGS_LIMIT 8000

#define BIN_MODE /* default */
/* This define applies to changing the type of ASN that is read by the client   */
/* Make sure that the server (seqrem) has the correct setting for writing the   */
/* file (i.e. binary or text )                                                  */
/* Comment out the define to use text asn read mode from the ByteStore          */
#ifdef BIN_MODE
#define FILE_MODE "rb"
#else 
#define FILE_MODE "r"
#endif

/* Timeout setting for network connection */
#define DEFAULTTO 180
#define USERTO 120
#define NBRTO 9999  /* neighbours timeout */
Int4 gsec = DEFAULTTO;  /* default is 30 secs */
Int4 gUsersec = USERTO;
static void SHoundSetTimeout(Int4 sec) 
{
	if(gUsersec == USERTO) { /*Connection settings not specified by user*/
		gsec = sec;      /* so allow the change */
	} else {
		gsec = gUsersec; /* Use user specified timeout */
	}
}
static Int4 SHoundGetTimeout(void) {return gsec;}

/* public user functions */
void SHoundUserSetTimeout(Int4 sec) {gUsersec = sec;}
Int4 SHoundUserGetTimeout(void) {return gsec;}


/****************************************************************************
*
*
*          SeqHound remote API
*
*
*
****************************************************************************/

static Int2 SHoundWWWGetfile(CharPtr host, CharPtr url) {
  CONN conn;
  CONNECTOR connector;
  EIO_Status status;
  size_t n_written;
  size_t n_read;
  Uint4 urlLen;
  Uint4 argssize = 0;
  Char buffer[BUFSIZ];
  SConnNetInfo* info = NULL;
  CharPtr args = NULL;
  Char puterr[BUFSIZ];
  Int2 putpos = 0;
  Int2 ercode = 0;
  STimeout timeout;
  /* SK: */
  CharPtr ptr;
  
  if ((host == NULL)||(url == NULL)) {
    ErrPostEx(SEV_ERROR,0,0,"[WWWGet] Invalild input arguments.\n");
    return 0;
  }
  if ((info = ConnNetInfo_Create(0)) == NULL) {
    ErrPostEx(SEV_ERROR,0,0,"Can't create ConnInfo structure. Aborting.");
    return 0;
  }

  args = StringChr(url, '?');
  if (args) {
    args = StringSave(++args);
    if((argssize = StringLen(args)) > ARGS_LIMIT) {
		ErrPostEx(SEV_ERROR,0,0,"Args structure too large: %ld bytes. Aborting.",argssize);
 		MemFree(args);
		return 0;
	}
	urlLen = StrLen(url) - StrLen(args) - 1;
    url[urlLen] = '\0';
  }
  

  StringCpy(info->host, host);
  info->port = 80;		/* !! port 80 is assumed--should really be a function arg !! */

  StringCpy(info->path, url);
  /*
     setting info->args to the args causes the Content-Length tag (in HTTP header)
     to be sent with a value of zero, rendering the server app unable to read the args
     from stdin (since a POST request is made), though the args themselves
     are indeed sent. So instead we write the args to the connection separately
     with CONN_Write below.
  */
  timeout.sec = SHoundGetTimeout();
  timeout.usec = 0;
  connector = HTTP_CreateConnector(info,NULL,0);
  status = CONN_Create(connector, &conn);
  if (status != eIO_Success) {
    ErrPostEx(SEV_ERROR,0,0,"Can't connect to %s. Aborting.", IsNullStr(host));
    ConnNetInfo_Destroy(info);
    return 0;
  }
  CONN_SetTimeout(conn,eIO_ReadWrite,&timeout);
  /* extract CGI args, if any, from URL;
     if args given, send request explicitly now;
     otherwise request sent implicitly upon CONN_Read below */
  if (args) {
    status = CONN_Write(conn, args, (size_t)StrLen(args), &n_written,eIO_WritePlain);
	args = MemFree(args);
    if (status != eIO_Success) {
      /* SK: */
      ptr = (char *)IO_StatusStr(status);
      ErrPostEx(SEV_ERROR, 0, 0, "Error writing to URL(%s)", IsNullStr(ptr));
      CONN_Close(conn);
      ConnNetInfo_Destroy(info);
      return 0;
    }
  }

  /* Kaca - read first line from the reply which contains SEQHOUND return code */
	putpos = 0;
	for(;;) {
		status = CONN_Read(conn, (void*)buffer, (size_t) 1, &n_read, eIO_ReadPlain);
		if (status == eIO_Timeout) 
			ErrPostEx(SEV_ERROR,0,0,"[WWWGet] Connection Timeout.");
		if (status != eIO_Success) break;
		puterr[putpos++] = buffer[0];
		if (buffer[0] == '\n') {
			puterr[putpos++] = '\0';	/* clear the buffer */
			break;
		}
	}
	
  /* if there was no first line get out */
  if(status != eIO_Success){
    ErrPostEx(SEV_ERROR,0,0,"[WWWGet]Empty reply buffer.");
    CONN_Close(conn);
    ConnNetInfo_Destroy(info);
    return 0;
  }
  /* parse SEQHOUND return code */
  if((ercode = ParseSeqHoundError(puterr)) == 2) {
    ErrPostEx(SEV_ERROR,0,0,"[WWWGet] SEQHOUND_NULL value detected.");
    CONN_Close(conn);
    ConnNetInfo_Destroy(info);
    return 2;
  }
  else if (ercode == 0) {
   ErrPostEx(SEV_ERROR,0,0,"[WWWGet] SEQHOUND_ERROR detected.");
   CONN_Close(conn);
   ConnNetInfo_Destroy(info);
   return 0;
  }
  else if (ercode == 11) {
    ErrPostEx(SEV_ERROR,0,0,"[WWWGet]Empty reply buffer.");
    CONN_Close(conn);
    ConnNetInfo_Destroy(info);
    return 0;
  }
  /* end Kaca */

    replyBS = NULL;
    replyBS = BSNew(10);
    if(replyBS == NULL){
      ErrPostEx(SEV_ERROR,0,0,"[WWWGet] Cannot open a bytestore.");
	  CONN_Close(conn);
	  ConnNetInfo_Destroy(info);
      return 0;
    }
    BSSeek(replyBS, 0L, SEEK_SET);

    /* Read reply from connection, write it to bytestore */
    for (;;) {
      status = CONN_Read(conn, (void*)buffer, (size_t) sizeof(buffer), &n_read, eIO_Plain);
      if (status != eIO_Success)
        break;
      BSWrite(replyBS, (Nlm_VoidPtr) buffer, (size_t)n_read);
    }

  CONN_Close(conn);
  ConnNetInfo_Destroy(info);
  if (status != eIO_Closed){
    /* SK: */
    ptr = (char *)IO_StatusStr(status);
    ErrPostEx(SEV_WARNING, 0, 0,
              "Error reading from host |%s|, URL |%s| (status=%s)",
			  IsNullStr(host), IsNullStr(url), IsNullStr(ptr));
    return 0;
  }
  return 1;

} /*SHoundWWWGetfile*/





/*******************************************************/
/*  Reply file testing and SeqHound error code testing */
/*******************************************************/
static Int2 LIBCALL ParseSeqHoundError(CharPtr pline)
{
  Char test[4];
  CharPtr pword = NULL;
  CharPtr pline_orig = NULL;

  test[0] = ' ';
  test[1] = '\n';
  test[2] = '\t';
  test[3] = '\0';

  if(pline == NULL)
    return 11;

  pline_orig = StringSave(pline);
  pword = strtok(pline, test);
  if(strcmp(pword, "SEQHOUND_ERROR") == 0)
   {
      ErrPostEx(SEV_ERROR,0,0, "%s", IsNullStr(pline_orig));
      MemFree(pline_orig);
      return 0;
   }
  else if(strcmp(pword, "SEQHOUND_NULL") == 0)
   {
      ErrPostEx(SEV_ERROR,0,0, "%s", IsNullStr(pline_orig));
      MemFree(pline_orig);
      return 2;
   }
  else if(strcmp(pword, "SEQHOUND_OK") == 0)
   {
      MemFree(pline_orig);
      return 1;
   }
  else
   {
     ErrPostEx(SEV_ERROR,0,0, "Abnormal SEQHOUND code: %s", IsNullStr(pline_orig));
     return 0;
   }
}


static Boolean LIBCALL ReplyBSTrueorFalse(void)
{
  CharPtr pword = NULL;
  Char pcBuf[100];
  Int2 res = -1;
  Char test[4];
  Int4 pcBufLen=0;

  test[0] = ' ';
  test[1] = '\n';
  test[2] = '\t';
  test[3] = '\0';

  BSSeek(replyBS, 0L, SEEK_SET);
  pcBufLen = BSLen(replyBS)+ 10;
  BSRead(replyBS, (Nlm_VoidPtr) pcBuf, pcBufLen);
  pword = strtok(pcBuf, test);
  if(pword == NULL)
   {
     ErrPostEx(SEV_ERROR,0,0, "ReplyBSTrueorFalse: Reply file empty.");
     BSFree(replyBS);
     return 0;
   }
  if ((res = Misc_IswwStringTrueOrFalse(pword)) == 1)
   {
      BSFree(replyBS);
      return TRUE;
   }
  else if(res == 0)
   {
     BSFree(replyBS);
     return FALSE;
   }
  else if(res == -1)
   {
     ErrPostEx(SEV_ERROR,0,0, "ReplyBSTrueorFalse: Ilegal Boolean value in return file.");
     BSFree(replyBS);
     replyBS = NULL;
     return FALSE;
   }
  return FALSE;
}


static Int4 LIBCALL ReplyBSGetInteger(void)
{
  CharPtr pword = NULL;
  CharPtr pcBuf=NULL;
  Int4 pcBufLen=0;
  Char test[4];
  Int4 gi = 0;


  test[0] = ' ';
  test[1] = '\n';
  test[2] = '\t';
  test[3] = '\0';

  BSSeek(replyBS, 0L, SEEK_SET);
  pcBufLen = BSLen(replyBS)+ 10;
  pcBuf = MemNew((size_t) pcBufLen*sizeof(Char));
  if(pcBuf==NULL)
   {
     ErrPostEx(SEV_ERROR,0,0, "MemNew failed.");
     BSFree(replyBS);
     return 0;
   }
  BSRead(replyBS, (Nlm_VoidPtr) pcBuf, pcBufLen);
  pword = strtok(pcBuf, test);
  if(pword == NULL)
   {
     ErrPostEx(SEV_ERROR,0,0, "ReplyBSGetInteger: Reply file empty.");
     if(pcBuf) MemFree(pcBuf);
     BSFree(replyBS);
	 replyBS = NULL;
     return 0;
   }
  if(strcmp(pword, "-1") == 0)
   {
     if(pcBuf) MemFree(pcBuf);
     BSFree(replyBS);
	 replyBS = NULL;
     return -1;
   }
  if(!StrToInt4 (pword, &gi))
   {
     ErrPostEx(SEV_ERROR,0,0, "ReplyBSGetInteger: The value is not an integer.");
     if(pcBuf) MemFree(pcBuf);
     BSFree(replyBS);
	 replyBS = NULL;
     return 0;
   }
  else
   {
     if(pcBuf) MemFree(pcBuf);
     BSFree(replyBS);
	 replyBS = NULL;
     return gi;
   }
}
static FloatHi LIBCALL ReplyBSGetFloat(void)
{
  CharPtr pword = NULL;
  CharPtr pcBuf=NULL;
  Int4 pcBufLen=0;
  Char test[4];
  FloatHi fl = 0;


  test[0] = ' ';
  test[1] = '\n';
  test[2] = '\t';
  test[3] = '\0';

  BSSeek(replyBS, 0L, SEEK_SET);
  pcBufLen = BSLen(replyBS)+ 10;
  pcBuf = MemNew((size_t) pcBufLen*sizeof(Char));
  if(pcBuf==NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "MemNew failed.");
      BSFree(replyBS);
      return 0;
    }
  BSRead(replyBS, (Nlm_VoidPtr) pcBuf, pcBufLen);
  pword = strtok(pcBuf, test);
  if(pword == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "ReplyBSGetFloat: Reply file empty.");
      if(pcBuf) MemFree(pcBuf);
      BSFree(replyBS);
      replyBS = NULL;
      return 0;
    }
  if(strcmp(pword, "-1") == 0)
    {
      if(pcBuf) MemFree(pcBuf);
      BSFree(replyBS);
      replyBS = NULL;
      return -1;
    }
  fl = strtod (pword, NULL);
  if (fl < 0)  
    {
      ErrPostEx(SEV_ERROR,0,0, "ReplyBSGetFloat: The value is not a float.");
      if(pcBuf) MemFree(pcBuf);
      BSFree(replyBS);
      replyBS = NULL;
      return -1;
    }else {
      if(pcBuf) MemFree(pcBuf);
      BSFree(replyBS);
      replyBS = NULL;
      return fl;
    }

}

static CharPtr LIBCALL ReplyBSGetString(void)
{
  CharPtr pword = NULL;
  CharPtr pcBuf=NULL;
  Int4 pcBufLen=0;
  Char test[3];


  /*test[0] = ' ';*/
  test[0] = '\n';
  test[1] = '\t';
  test[2] = '\0';

  BSSeek(replyBS, 0L, SEEK_SET);
  pcBufLen = BSLen(replyBS)+ 10;
  pcBuf = MemNew((size_t) pcBufLen*sizeof(Char));
  if(pcBuf==NULL)
   {
     ErrPostEx(SEV_ERROR,0,0, "MemNew failed.");
     BSFree(replyBS);
     return NULL;
   }
  BSRead(replyBS, (Nlm_VoidPtr) pcBuf, pcBufLen);
  pword = StringSave(strtok(pcBuf, test));
  if(pword == NULL)
   {
     ErrPostEx(SEV_ERROR,0,0, "ReplyBSGetString: Reply file empty.");
     if(pcBuf) MemFree(pcBuf);
     BSFree(replyBS);
     return NULL;
   }
  if(strcmp(pword, "(null)") == 0)
   {
     ErrPostEx(SEV_ERROR,0,0, "ReplyBSGetString: Reply file contains --NULL--.");
     if(pcBuf) MemFree(pcBuf);
     BSFree(replyBS);
     return NULL;
   }
  if(pcBuf) MemFree(pcBuf);
  if (replyBS) BSFree(replyBS);
  replyBS = NULL;
  return pword;
}

/* returns the full string, tabs and spaces and all. */
/*  static CharPtr LIBCALL ReplyBSGetFullString(void)
{
  CharPtr pword = NULL;
  CharPtr pcBuf=NULL;
  Int4 pcBufLen=0;
  Char test[1];
  
  test[0] = '\n';

  BSSeek(replyBS, 0L, SEEK_SET);
  pcBufLen = BSLen(replyBS)+ 10;
  pcBuf = MemNew((size_t) pcBufLen*sizeof(Char));
  if(pcBuf==NULL)
   {
     ErrPostEx(SEV_ERROR,0,0, "MemNew failed.");
     BSFree(replyBS);
     return NULL;
   }
  BSRead(replyBS, (Nlm_VoidPtr) pcBuf, pcBufLen);
  pword = StringSave(strtok(pcBuf, test));
  if(pword == NULL)
   {
     ErrPostEx(SEV_ERROR,0,0, "ReplyBSGetString: Reply file empty.");
     if(pcBuf) MemFree(pcBuf);
     BSFree(replyBS);
     return NULL;
   }
  if(strcmp(pword, "(null)") == 0)
   {
     ErrPostEx(SEV_ERROR,0,0, "ReplyBSGetString: Reply file contains --NULL--.");
     if(pcBuf) MemFree(pcBuf);
     BSFree(replyBS);
     return NULL;
   }
  if(pcBuf) MemFree(pcBuf);
  return pword;
}
*/


/* Converts a comma delimited list of integers of strings to ValNode
IsInteger means that string is supposed to be converted to ValNode of Integers */
static ValNodePtr LIBCALL ReplyBSGetValNode(Boolean IsInteger)
{
  CharPtr pword = NULL;
  CharPtr pcBuf=NULL;	/*Gary*/
  Int4 pcBufLen=0;	/*Gary*/
  Char test[2];
  ValNodePtr pvnThis = NULL;
  Char sep[2];


  sep[0] = ',';
  sep[1] = '\0';
  /*test[0] = ' ';
  test[1] = '\n';
  test[2] = '\t';
  test[3] = '\0';*/
  test[0] = '\n';
  test[1] = '\0';


  BSSeek(replyBS, 0L, SEEK_SET);
  pcBufLen = BSLen(replyBS)+ 10;
  pcBuf = MemNew((size_t) pcBufLen*sizeof(Char));
  if(pcBuf==NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "MemNew failed.");
      BSFree(replyBS);
      return NULL;
    }
  BSRead(replyBS, (Nlm_VoidPtr) pcBuf, pcBufLen);
  pword = strtok(pcBuf, test);

  if(pword == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "ReplyBSGetValNode: Reply file empty.");
      if(pcBuf) MemFree(pcBuf);
      /*if(replyBS)BSFree(replyBS);*/
      return NULL;
    }
  if(strcmp(pword, "(null)") == 0)
    {
      ErrPostEx(SEV_ERROR,0,0, "ReplyBSGetValNode: Reply file contains --NULL--.");
      if(pcBuf) MemFree(pcBuf);
      BSFree(replyBS);
      return NULL;
    }
  if(IsInteger)
    pvnThis = Misc_CommaIntString2ValNodeList(pword, sep);
  else
    pvnThis = Misc_CommaStrString2ValNodeList(pword, sep);
  if(pvnThis == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "ReplyBSGetValNode: Misc_Comma*String2ValNodeList function failed.");
      if(pcBuf) MemFree(pcBuf);
      BSFree(replyBS);
      return NULL;
    }
  if(pcBuf) MemFree(pcBuf);
  if(replyBS) BSFree(replyBS);
  replyBS = NULL;
  return pvnThis;
}

/* Converts a comma delimited list of integers of strings to an Order ValNode
IsInteger means that string is supposed to be converted to ValNode of Integers */
static ValNodePtr LIBCALL ReplyBSGetValNodeOrdered(Boolean IsInteger)
{
  CharPtr pword = NULL;
  CharPtr pcBuf=NULL;	/*Gary*/
  Int4 pcBufLen=0;	/*Gary*/
  Char test[2];
  ValNodePtr pvnThis = NULL;
  Char sep[2];


  sep[0] = ',';
  sep[1] = '\0';
  /*test[0] = ' ';
  test[1] = '\n';
  test[2] = '\t';
  test[3] = '\0';*/
  test[0] = '\n';
  test[1] = '\0';


  BSSeek(replyBS, 0L, SEEK_SET);
  pcBufLen = BSLen(replyBS)+ 10;
  pcBuf = MemNew((size_t) pcBufLen*sizeof(Char));
  if(pcBuf==NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "MemNew failed.");
      BSFree(replyBS);
      return NULL;
    }
  BSRead(replyBS, (Nlm_VoidPtr) pcBuf, pcBufLen);
  pword = strtok(pcBuf, test);

  if(pword == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "ReplyBSGetValNodeOrdered: Reply file empty.");
      if(pcBuf) MemFree(pcBuf);
      /*if(replyBS)BSFree(replyBS);*/
      return NULL;
    }
  if(strcmp(pword, "(null)") == 0)
    {
      ErrPostEx(SEV_ERROR,0,0, "ReplyBSGetValNodeOrdered: Reply file contains --NULL--.");
      if(pcBuf) MemFree(pcBuf);
      BSFree(replyBS);
      return NULL;
    }
  if(IsInteger)
    pvnThis = Misc_CommaIntString2ValNodeListOrdered(pword, sep);
  else
    pvnThis = (ValNodePtr)My_Misc_CommaStrStringWithNull2ValNodeListEx(pword, sep, TRUE);
  if(pvnThis == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "ReplyBSGetValNodeOrdered: Misc_Comma*String2ValNodeList function failed.");
      if(pcBuf) MemFree(pcBuf);
      BSFree(replyBS);
      return NULL;
    }
  if(pcBuf) MemFree(pcBuf);
  if(replyBS) BSFree(replyBS);
  replyBS = NULL;
  return pvnThis;
}

ValNodePtr My_Misc_CommaStrStringWithNull2ValNodeListEx(CharPtr string, CharPtr seps, Boolean maintainOrder)
{
	ValNodePtr vnpList=NULL;
	CharPtr stringTmp=NULL;

	CharPtr pch;
	CharPtr aux = "";
	int last = 0;

	if((string==NULL)||(seps==NULL)) 
	{
		return(NULL);
	} 

	stringTmp = StringSave(string);
	Mem_Check(stringTmp);

	pch = StringChr(stringTmp, ',');

	while(pch != NULL) 
	{
	    aux = (CharPtr) getSubString(stringTmp, last, (Int4)((CharPtr)pch - (CharPtr)stringTmp));
/*	    if ( aux == "")
	    {
		printf("Null token\n");
	    }
*/
	    if(maintainOrder) 
	    {
		ValNodeAddStr(&vnpList, 0, StringSave(aux));
	    }
	    else {
		Slri_ValNodeAddStr(&vnpList, 0, StringSave(aux));
	    }
	    if (aux != "")
		Free(aux);
	    last = pch-stringTmp+1;
	    pch++;
	    pch = StringChr(pch, ',');
	}
	if (last == strlen(stringTmp))
	{
	    if(maintainOrder) 
	    {
		ValNodeAddStr(&vnpList, 0, StringSave(aux));
	    }
	    else {
		Slri_ValNodeAddStr(&vnpList, 0, StringSave(aux));
	    }
	}
	else
	    if (last < strlen(stringTmp))
	    {
		aux = (CharPtr) getSubString(stringTmp, last, strlen(stringTmp));
		if(maintainOrder) 
		{
		    ValNodeAddStr(&vnpList, 0, StringSave(aux));
		}
		else {
		    Slri_ValNodeAddStr(&vnpList, 0, StringSave(aux));
		}
	    }
	if (aux != "")
	    Free(aux);
	stringTmp = MemFree(stringTmp);
	return(vnpList);
}

CharPtr getSubString(CharPtr auxch, Int4 i, Int4 j)
{
    int k = i, z = 0; 
    CharPtr retch = NULL;

    if (i == j)
    {
	return "";
    }
    
     retch = (Char *)malloc( sizeof(Char) *((j-i)+1) );
     while(k<j)
     {
	    retch[z] =  auxch[k];
	    k++; z++;
     }
     retch[z] =  (Char)'\0';
     return retch;
}

ValNodePtr My_Misc_CommaStrString2ValNodeListEx(CharPtr string, CharPtr seps, Boolean maintainOrder)
{
    ValNodePtr vnpList=NULL;
    CharPtr token=NULL, stringTmp=NULL;
    
    if((string==NULL)||(seps==NULL)) {
	return(NULL);
    }
    
    /*copy string so that StringTok doesn't destroy the passed string*/
    stringTmp = StringSave(string);
    Mem_Check(stringTmp);
	
    token = strtok(stringTmp, seps); /*StringTok(stringTmp, seps);*/
    printf("string: %s\n", string);
    while(token != NULL) 
    {
	printf("                       token: %s.\n", token);
	if ( token == "")
	{
	    printf("Null token\n");
	}
	if(maintainOrder) 
	{
	    ValNodeAddStr(&vnpList, 0, StringSave(token));
	}
	else {
	    Slri_ValNodeAddStr(&vnpList, 0, StringSave(token));
	}
	
	token = strtok(NULL, seps);/*StringTok(NULL, seps);*/
	printf("                       token: %s.\n", token);
	if ( token == NULL)
	{
	    printf("Null token\n");
	}
    }
    
    stringTmp = MemFree(stringTmp);
    return(vnpList);
}


/************************************************/
/* SeqHound Initialization, Finishing and Testing */
/************************************************/

/*Try a SeqHound server to see if it is alive*/
static Boolean SHoundIsServerAlive(CharPtr configFileKey, CharPtr fPath)
{


  if(configFileKey==NULL) {
    ErrPostEx(SEV_ERROR,0,0,"SHoundIsServerAlive: Passed configFileKey was NULL.");
    return(FALSE);
  }
  if(fPath==NULL) {
    ErrPostEx(SEV_ERROR,0,0,"SHoundIsServerAlive: Passed fPath was NULL.");
    return(FALSE);
  }
  
  /*global var slri_sername must be allocated*/
  if(slri_sername==NULL) {
    ErrPostEx(SEV_ERROR,0,0,"SHoundIsServerAlive: global var slri_sername not allocated.");
    return(FALSE);
  }
  
  if(GetAppParam("shoundrem", "remote", configFileKey, NULL, slri_sername, (size_t)PATH_MAX * (sizeof(Char))) == 0){
    ErrPostEx(SEV_ERROR,0,0,"SHoundIsServerAlive: Cannot get %s name from config file.", IsNullStr(configFileKey));
    return(FALSE);
  }
  if(SHoundWWWGetfile(slri_sername, fPath) == 0) {
    ErrPostEx(SEV_INFO,0,0,"SHoundIsServerAlive: %s is down.", IsNullStr(slri_sername));
    return(FALSE);
  }
  if(ReplyBSTrueorFalse())
    {
      ErrPostEx(SEV_INFO,0,0,"SHoundIsServerAlive: %s is alive.", IsNullStr(slri_sername));
      return TRUE;
    }
  else
    {
      ErrPostEx(SEV_INFO,0,0,"SHoundIsServerAlive: %s is dead.", IsNullStr(slri_sername));
      return FALSE;
    }
}

Boolean LIBCALL SHoundInit(Boolean NetEntrezOnToo, CharPtr appname)
{

  Char fpath[PATH_MAX];
  CharPtr fpathbk=NULL;
  NetEntrez = NetEntrezOnToo;
  isInited=FALSE;
  slri_cgipath = MemNew((size_t)PATH_MAX * (sizeof(Char)));
  GetAppParam("shoundrem", "remote", "CGI", NULL, slri_cgipath, (size_t)PATH_MAX * (sizeof(Char)));
  if(strlen(slri_cgipath) == 0)
  {
    ErrPostEx(SEV_ERROR,0,0, "SeqHoundInit: Failed to get the CGI path.");
    return (FALSE);
  }

  sprintf(fpath,"%s?fnct=SeqHoundInit&NetEntrezOnToo=%s&appname=%s", IsNullStr(slri_cgipath), (NetEntrezOnToo)?"TRUE":"FALSE", IsNullStr(appname));
  ErrPostEx(SEV_INFO,0,0, "SeqHoundInit request: %s\n", IsNullStr(fpath));
  fpathbk=StringSave(fpath);
  
  slri_sername = MemNew((size_t)PATH_MAX * (sizeof(Char)));
  if(SHoundIsServerAlive("server1", fpathbk)) {
    fpathbk=MemFree(fpathbk);
    isInited=TRUE;
    return(TRUE);
  }
  fpathbk=MemFree(fpathbk);
  fpathbk=StringSave(fpath);
  if(SHoundIsServerAlive("server2", fpathbk)) {
    fpathbk=MemFree(fpathbk);
    isInited=TRUE;
    return(TRUE);
  }
  fpathbk=MemFree(fpathbk);
  return(FALSE);
}
/* if NetEntrezOnToo is set it does an EntrezInit() */


/* this is useful only in local setting but has to be here for consistency between remote and local API */
Boolean LIBCALL SHoundInitReadOnly(Boolean NetEntrezOnToo, CharPtr appname)
{
   return SHoundInit(NetEntrezOnToo, appname);
}



Boolean LIBCALL SHoundIsNetEntrezOn(void)
{

  Char fpath[PATH_MAX];
  
  sprintf(fpath,"%s?fnct=SeqHoundIsNetEntrezOn", IsNullStr(slri_cgipath));
  ErrPostEx(SEV_INFO,2,1,"SeqHoundIsNetEntrezOn request: %s.\n", IsNullStr(fpath));
  if(SHoundWWWGetfile(slri_sername, fpath) == 0)
    {
      ErrPostEx(SEV_ERROR,0,0, "SHoundSHoundWWWGetfile failed.");
      return FALSE;
    }
  if(ReplyBSTrueorFalse())
    return TRUE;
  else
    return FALSE;
}


Boolean LIBCALL SHoundNetEntrezInit(CharPtr appname)
{

  Char fpath[PATH_MAX];
  
  sprintf(fpath,"%s?fnct=SeqHoundNetEntrezInit&appname=%s", IsNullStr(slri_cgipath),  IsNullStr(appname));
  ErrPostEx(SEV_INFO,0,0, "SeqHoundNetEntrezInit request: %s.\n", IsNullStr(fpath));
  if(SHoundWWWGetfile(slri_sername, fpath) == 0)
    {
      ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
      return FALSE;
    }
  if(ReplyBSTrueorFalse())
    return TRUE;
  else
    return FALSE;
}


void  LIBCALL SHoundFini(void)
{

  Char fpath[PATH_MAX];
  
  sprintf(fpath,"%s?fnct=SeqHoundFini", IsNullStr(slri_cgipath));
  ErrPostEx(SEV_INFO,0,0, "SeqHoundFini request: %s.\n", IsNullStr(fpath));
  if(SHoundWWWGetfile(slri_sername, fpath) == 0)
    {
      ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
      return;
    }
  
  if(slri_sername) MemFree(slri_sername);
  if(slri_cgipath) MemFree(slri_cgipath);
  if(replyBS) BSFree(replyBS);
  isInited=FALSE;
  return;
}


Boolean LIBCALL SHoundIsInited(void)
{
  return(isInited);
}

/*Allocates memory - free returned value*/
CharPtr LIBCALL SHoundGetCurrentServer(void)
{
  if(slri_sername==NULL) {
    ErrPostEx(SEV_ERROR,0,0, "SeqHound is not Inited. Call SHoundInit first.");
    return NULL;
  }
  
  return(StringSave(slri_sername));
}


/*****************************************************
 *              HISTDB FUNCTION CALL
 *****************************************************/
CharPtr LIBCALL SHoundGetAccFromRetiredGi(Int4 gi)
{

  Char fpath[PATH_MAX];
  CharPtr pacc = NULL;


  if (gi <= 0)
    {
      ErrPostEx(SEV_ERROR,0,0, "Invalid GI.");
      return NULL;
    }

  sprintf(fpath,"%s?fnct=SeqHoundGetAccFromRetiredGi&gi=%ld", IsNullStr(slri_cgipath), (long) gi);
  ErrPostEx(SEV_INFO,0,0, "SeqHoundGetAccFromRetiredGi request: %s.\n", IsNullStr(fpath));
  if(SHoundWWWGetfile(slri_sername, fpath) == 0)
    {
      ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
      return NULL;
    }
  pacc = ReplyBSGetString();
  if (pacc == NULL)
    {
      ErrPostEx(SEV_INFO,0,0, "SeqHoundGetAccFromRetiredGi returned NULL.");
      return NULL;
    }
  return pacc;

}


/******************************************************/
/* SeqId to/from Gi, Accession to/from Gi Conversions */
/******************************************************/

Int4 LIBCALL SHoundFindAcc(CharPtr pcAcc)
{

  Char fpath[PATH_MAX];
  Int4 gi = 0;


  if(pcAcc == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "Invalid parameter.");
      return 0;
    }
  
  sprintf(fpath,"%s?fnct=SeqHoundFindAcc&acc=%s", IsNullStr(slri_cgipath), IsNullStr(pcAcc));
  ErrPostEx(SEV_INFO,0,0, "SeqHoundFindAcc request: %s.\n", IsNullStr(fpath));
  if(SHoundWWWGetfile(slri_sername, fpath) == 0)
    {
      ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
      return 0;
    }
  gi = ReplyBSGetInteger();
  if (gi == 0)
    {
      ErrPostEx(SEV_INFO,0,0, "SeqHoundFindAcc returned zero.");
      return 0;
    }
  return gi;
}


ValNodePtr LIBCALL SHoundFindAccList(ValNodePtr pvnAcc)
{
  CharPtr fpath=NULL;
  CharPtr pvalstr = NULL;
  ValNodePtr pvngi = NULL;
  ValNodePtr pvntemp = NULL, pvnshort = NULL;
  Int4 counter = 0;
  ValNodePtr pvngifin = NULL;
  
  if(pvnAcc == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "Invalid parameter.");
      return NULL;
    }
  
  
  pvntemp = pvnAcc;
  while (pvntemp != NULL)
    {
      counter++;
      ValNodeCopyStr(&pvnshort, 0, pvntemp->data.ptrvalue);
      if(counter == 700 || pvntemp->next == NULL)
	{
	  pvalstr = Misc_SaveStrValNodeListAsString(pvnshort);
	  if(pvalstr == NULL)
	    {
	      ErrPostEx(SEV_ERROR,0,0, "Misc_SaveStrValNodeListAsString returned NULL.");
	      pvnshort = ValNodeFreeData(pvnshort);
	      return NULL;
	    }
	  fpath = MemNew((size_t)(StringLen(pvalstr)+PATH_MAX+100)*sizeof(Char));
	  if(fpath == NULL)
	    {
	      ErrPostEx(SEV_ERROR,0,0, "MemNew returned NULL.");
	      pvalstr = MemFree(pvalstr);
	      pvnshort = ValNodeFreeData(pvnshort);
	      return NULL;
	    }
	  sprintf(fpath,"%s?fnct=SeqHoundFindAccList&pacc=%s", IsNullStr(slri_cgipath), IsNullStr(pvalstr));
	  ErrPostEx(SEV_INFO,0,0, "SeqHoundFindAccList request: %s.\n", IsNullStr(fpath));
	  pvalstr = MemFree(pvalstr);
	  if(SHoundWWWGetfile(slri_sername, fpath) == 0)
	    {
	      ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
	      fpath = MemFree(fpath);
	      pvnshort = ValNodeFreeData(pvnshort);
	      return NULL;
	    }
	  pvngi = ReplyBSGetValNode(TRUE);
	  ValNodeLink(&pvngifin, pvngi);
	  counter = 0;
	  pvnshort = ValNodeFreeData(pvnshort);
	  fpath = MemFree(fpath);
	}
      pvntemp = pvntemp->next;
    }
  Misc_IntVnpUnique(&pvngifin);
  return pvngifin;
}




Int4 LIBCALL SHoundFindName(CharPtr pName)
{
     Char fpath[PATH_MAX];
     Int4 gi = 0;

     if(pName == NULL)
      {
        ErrPostEx(SEV_ERROR,0,0, "Invalid parameter.");
        return 0;
      }

  sprintf(fpath,"%s?fnct=SeqHoundFindName&name=%s", IsNullStr(slri_cgipath), IsNullStr(pName));
  ErrPostEx(SEV_INFO,0,0, "SeqHoundFindName request: %s.\n", IsNullStr(fpath));
  if(SHoundWWWGetfile(slri_sername, fpath) == 0)
    {
      ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
      return 0;
    }
  gi = ReplyBSGetInteger();
  if (gi == 0)
    {
      ErrPostEx(SEV_INFO,0,0, "SeqHoundFindName returned zero.");
      return 0;
    }
  return gi;
}


ValNodePtr LIBCALL SHoundFindNameList(ValNodePtr pvnName)
{
  CharPtr fpath=NULL;
  CharPtr pvalstr = NULL;
  ValNodePtr pvngi = NULL;
  ValNodePtr pvntemp = NULL, pvnshort = NULL;
  Int4 counter = 0;
  ValNodePtr pvngifin = NULL;
  
  if(pvnName == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "Invalid parameter.");
      return NULL;
    }
  
  
  pvntemp = pvnName;
  while (pvntemp != NULL)
    {
      counter++;
      ValNodeCopyStr(&pvnshort, 0, pvntemp->data.ptrvalue);
      if(counter == 700 || pvntemp->next == NULL)
	{
	  pvalstr = Misc_SaveStrValNodeListAsString(pvnshort);
	  if(pvalstr == NULL)
	    {
	      ErrPostEx(SEV_ERROR,0,0, "Misc_SaveStrValNodeListAsString returned NULL.");
	      pvnshort = ValNodeFreeData(pvnshort);
	      return NULL;
	    }
	  fpath = MemNew((size_t)(StringLen(pvalstr)+PATH_MAX+100)*sizeof(Char));
	  if(fpath == NULL)
	    {
	      ErrPostEx(SEV_ERROR,0,0, "MemNew returned NULL.");
	      pvalstr = MemFree(pvalstr);
	      pvnshort = ValNodeFreeData(pvnshort);
	      return NULL;
	    }
	  sprintf(fpath,"%s?fnct=SeqHoundFindNameList&pname=%s", IsNullStr(slri_cgipath), IsNullStr(pvalstr));
	  ErrPostEx(SEV_INFO,0,0, "SeqHoundFindNameList request: %s.\n", IsNullStr(fpath));
	  pvalstr = MemFree(pvalstr);
	  if(SHoundWWWGetfile(slri_sername, fpath) == 0)
	    {
	      ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
	      fpath = MemFree(fpath);
	      pvnshort = ValNodeFreeData(pvnshort);
	      return NULL;
	    }
	  pvngi = ReplyBSGetValNode(TRUE);
	  ValNodeLink(&pvngifin, pvngi);
	  counter = 0;
	  pvnshort = ValNodeFreeData(pvnshort);
	  fpath = MemFree(fpath);
	}
      pvntemp = pvntemp->next;
    }
  Misc_IntVnpUnique(&pvngifin);
  return pvngifin;
}



CharPtr LIBCALL SHoundAccFromGi(Int4 gi)
{

  Char fpath[PATH_MAX];
  CharPtr pacc = NULL;


  if (gi <= 0)
    {
      ErrPostEx(SEV_ERROR,0,0, "Invalid GI.");
      return NULL;
    }

  sprintf(fpath,"%s?fnct=SeqHoundAccFromGi&gi=%ld", IsNullStr(slri_cgipath), (long) gi);
  ErrPostEx(SEV_INFO,0,0, "SeqHoundAccFromGi request: %s.\n", IsNullStr(fpath));
  if(SHoundWWWGetfile(slri_sername, fpath) == 0)
    {
      ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
      return NULL;
    }
  pacc = ReplyBSGetString();
  if (pacc == NULL)
    {
      ErrPostEx(SEV_INFO,0,0, "SeqHoundAccFromGi returned NULL.");
      return NULL;
    }
  return pacc;

}


ValNodePtr LIBCALL SHoundAccFromGiList(ValNodePtr pvnGi)
{
  CharPtr pvalstr = NULL;
  ValNodePtr pvnacc = NULL;
  CharPtr fpath=NULL;
  ValNodePtr pvntemp = NULL, pvnshort = NULL, pvnaccfin = NULL;
  Int4 counter = 0;

  if(pvnGi == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "Invalid parameter.");
      return NULL;
    }

  pvntemp = pvnGi;
  while (pvntemp != NULL)
    {
      counter++;
      pvnshort = Slri_ValNodeAddInt(&pvnshort, 0, pvntemp->data.intvalue);
      if(counter == MAXQ || pvntemp->next == NULL)
	{

	  pvalstr = Misc_SaveValNodeListAsString(pvnshort);
	  if(pvalstr == NULL)
	    {
	      ErrPostEx(SEV_ERROR,0,0, "Misc_SaveValNodeListAsString returned NULL.");
	      pvnshort = ValNodeFree(pvnshort);
	      return NULL;
	    }
	  fpath = MemNew((size_t)(StringLen(pvalstr)+PATH_MAX+100)*sizeof(Char));
	  if(fpath == NULL)
	    {
	      ErrPostEx(SEV_ERROR,0,0, "MemNew returned NULL.");
	      pvalstr = MemFree(pvalstr);
	      pvnshort = ValNodeFree(pvnshort);
	      return NULL;
	    }
	  sprintf(fpath,"%s?fnct=SeqHoundAccFromGiList&pgi=%s", IsNullStr(slri_cgipath), IsNullStr(pvalstr));
	  ErrPostEx(SEV_INFO,0,0, "SeqHoundAccFromGiList request: %s.\n", IsNullStr(fpath));
	  pvalstr = MemFree(pvalstr);
	  if(SHoundWWWGetfile(slri_sername, fpath) == 0)
	    {
	      ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
	      fpath = MemFree(fpath);
	      pvnshort = ValNodeFree(pvnshort);
	      return NULL;
	    }
	  pvnacc = ReplyBSGetValNode(FALSE);
	  fpath = MemFree(fpath);
	  ValNodeLink(&pvnaccfin, pvnacc);
	  counter = 0;
	  pvnshort = ValNodeFree(pvnshort);
	  fpath = MemFree(fpath);
	}
      pvntemp = pvntemp->next;
    }
  Misc_StringVnpUnique(&pvnaccfin);
  return pvnaccfin;
}

CharPtr LIBCALL SHoundTitleFromGi(Int4 gi)
{

  Char fpath[PATH_MAX];
  CharPtr ptitle = NULL;


  if (gi <= 0)
    {
      ErrPostEx(SEV_ERROR,0,0, "Invalid GI.");
      return NULL;
    }

  sprintf(fpath,"%s?fnct=SeqHoundTitleFromGi&gi=%ld", IsNullStr(slri_cgipath), (long) gi);
  ErrPostEx(SEV_INFO,0,0, "SeqHoundTitleFromGi request: %s.\n", IsNullStr(fpath));
  if(SHoundWWWGetfile(slri_sername, fpath) == 0)
    {
      ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
      return NULL;
    }
  ptitle = ReplyBSGetString();
  if (ptitle == NULL)
    {
      ErrPostEx(SEV_INFO,0,0, "SeqHoundTitleFromGi returned NULL.");
      return NULL;
    }
  return ptitle;

}


ValNodePtr LIBCALL SHoundTitleFromGiList(ValNodePtr pvnGi)
{
  CharPtr pvalstr = NULL;
  ValNodePtr pvntitle = NULL;
  CharPtr fpath=NULL;
  ValNodePtr pvntemp = NULL, pvnshort = NULL, pvntitlefin = NULL;
  Int4 counter = 0;

  if(pvnGi == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "Invalid parameter.");
      return NULL;
    }

  pvntemp = pvnGi;
  while (pvntemp != NULL)
    {
      counter++;
      pvnshort = Slri_ValNodeAddInt(&pvnshort, 0, pvntemp->data.intvalue);
      if(counter == MAXQ || pvntemp->next == NULL)
	{

	  pvalstr = Misc_SaveValNodeListAsString(pvnshort);
	  if(pvalstr == NULL)
	    {
	      ErrPostEx(SEV_ERROR,0,0, "Misc_SaveValNodeListAsString returned NULL.");
	      pvnshort = ValNodeFree(pvnshort);
	      return NULL;
	    }
	  fpath = MemNew((size_t)(StringLen(pvalstr)+PATH_MAX+100)*sizeof(Char));
	  if(fpath == NULL)
	    {
	      ErrPostEx(SEV_ERROR,0,0, "MemNew returned NULL.");
	      pvalstr = MemFree(pvalstr);
	      pvnshort = ValNodeFree(pvnshort);
	      return NULL;
	    }
	  sprintf(fpath,"%s?fnct=SeqHoundTitleFromGiList&pgi=%s", IsNullStr(slri_cgipath), IsNullStr(pvalstr));
	  ErrPostEx(SEV_INFO,0,0, "SeqHoundTitleFromGiList request: %s.\n", IsNullStr(fpath));
	  pvalstr = MemFree(pvalstr);
	  if(SHoundWWWGetfile(slri_sername, fpath) == 0)
	    {
	      ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
	      fpath = MemFree(fpath);
	      pvnshort = ValNodeFree(pvnshort);
	      return NULL;
	    }
	  pvntitle = ReplyBSGetValNode(FALSE);
	  fpath = MemFree(fpath);
	  ValNodeLink(&pvntitlefin, pvntitle);
	  counter = 0;
	  pvnshort = ValNodeFree(pvnshort);
	  fpath = MemFree(fpath);
	}
      pvntemp = pvntemp->next;
    }
  Misc_StringVnpUnique(&pvntitlefin);
  return pvntitlefin;
}


SeqIdPtr LIBCALL SHoundSeqIdFromGi(Int4 gi)
{
  Char fpath[PATH_MAX];
  SeqIdPtr psi = NULL;
  /* AsnIoPtr aip = NULL;*/
  Int2 ercode = 0;
  AsnIoBSPtr aipbs = NULL;

  if (gi <= 0)
    {
      ErrPostEx(SEV_ERROR,0,0, "Invalid GI.");
      return NULL;
    }

  sprintf(fpath,"%s?fnct=SeqHoundSeqIdFromGi&gi=%ld", IsNullStr(slri_cgipath), (long)  gi);
  ErrPostEx(SEV_INFO,2,1,"SeqHoundSeqIdFromGi request: %s\n", IsNullStr(fpath));
  if((ercode = SHoundWWWGetfile(slri_sername, fpath)) == 0)
    {
      ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
      return NULL;
    }
  else if (ercode == 2)
    return NULL;
  
  aipbs = AsnIoBSOpen(FILE_MODE, replyBS);
  if (aipbs == NULL)
    {
      ErrPostEx(SEV_ERROR,2,1,"AsnIoBSOpen returned NULL.");
      return NULL;
    }
  psi = SLRISeqIdAsnRead(aipbs->aip, NULL);
  if (psi == NULL)
    {
      ErrPostEx(SEV_INFO,2,1,"SeqIdAsnRead returned NULL.");
      AsnIoBSClose(aipbs);
      BSFree(replyBS);
      return NULL;
    }
  AsnIoBSClose(aipbs);
  BSFree(replyBS);
  return psi;
}



ValNodePtr LIBCALL SHoundSeqIdFromGiList(ValNodePtr pvnGi)
{
     CharPtr fpath=NULL;
     ValNodePtr pvnpsi = NULL, pvnpsifin = NULL;
     CharPtr pvalstr = NULL;
     Int2 ercode = 0;
     AsnIoBSPtr aipbs = NULL;
     ValNodePtr pvntemp = NULL, pvnshort = NULL;
     Int4 counter = 0;


     if(pvnGi == NULL)
      {
        ErrPostEx(SEV_ERROR,0,0, "Invalid parameter.");
        return NULL;
      }

     pvntemp = pvnGi;
     while (pvntemp != NULL)
      {
         counter++;
         pvnshort = Slri_ValNodeAddInt(&pvnshort, 0, pvntemp->data.intvalue);
         if(counter == MAXQ || pvntemp->next == NULL)
          {
             pvalstr = Misc_SaveValNodeListAsString(pvnshort);
             if(pvalstr == NULL)
              {
                ErrPostEx(SEV_ERROR,0,0, "Misc_SaveValNodeListAsString returned NULL.");
                pvnshort = ValNodeFree(pvnshort);
                return NULL;
              }
             fpath = MemNew((size_t)(StringLen(pvalstr)+PATH_MAX+100)*sizeof(Char));
             if(fpath==NULL)
              {
                ErrPostEx(SEV_ERROR,0,0, "MemNew returned NULL.");
                pvalstr = MemFree(pvalstr);
                pvnshort = ValNodeFree(pvnshort);
                return NULL;
              }
             sprintf(fpath,"%s?fnct=SeqHoundSeqIdFromGiList&pgi=%s", IsNullStr(slri_cgipath), IsNullStr(pvalstr));
             ErrPostEx(SEV_INFO,2,1,"SeqHoundSeqIdFromGiList request: %s\n", IsNullStr(fpath));
             pvalstr = MemFree(pvalstr);
             if((ercode = SHoundWWWGetfile(slri_sername, fpath)) == 0)
              {
                 ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
                 fpath = MemFree(fpath);
                 pvnshort = ValNodeFree(pvnshort);
                 return NULL;
              }
             else if (ercode == 2)
              {
                 fpath = MemFree(fpath);
                 pvnshort = ValNodeFree(pvnshort);
                 return NULL;
              }
             fpath = MemFree(fpath);
             aipbs = AsnIoBSOpen(FILE_MODE, replyBS);
             if (aipbs == NULL)
              {
                ErrPostEx(SEV_ERROR,2,1,"AsnIoBSOpen returned NULL.");
                pvnshort = ValNodeFree(pvnshort);
                return NULL;
              }
             pvnpsi = SLRIValNodeAsnRead(aipbs->aip, NULL);
             if (pvnpsi == NULL)
              {
                ErrPostEx(SEV_ERROR,2,1," SLRIValNodeAsnRead returned NULL.");
                AsnIoBSClose(aipbs);
                BSFree(replyBS);
                pvnshort = ValNodeFree(pvnshort);
                return NULL;
              }
             AsnIoBSClose(aipbs);
             BSFree(replyBS);
             pvnshort = ValNodeFree(pvnshort);
             ValNodeLink(&pvnpsifin, pvnpsi);
             counter = 0;
          }
         pvntemp = pvntemp->next;
      }
     return pvnpsifin;
}



SeqIdPtr  LIBCALL FreeSeqId(SeqIdPtr psi)
{
  SeqIdPtr psiNew = NULL;

  while (psi != NULL)
    {
      psiNew = psi->next;
      SeqIdFree(psi);
      psi = psiNew;
    }
  return (SeqIdPtr) NULL;
}


ValNodePtr LIBCALL FreeVNSeqIdList(ValNodePtr pvnSeqId)
{
  ValNodePtr pvnSeqIdNew = NULL;
  SeqIdPtr psi = NULL;
  
  while (pvnSeqId != NULL)
    {
      pvnSeqIdNew = pvnSeqId->next;
      psi = (SeqIdPtr) pvnSeqId->data.ptrvalue;
      FreeSeqId(psi);
      pvnSeqId = pvnSeqIdNew;
    }
  return (ValNodePtr) NULL;
}


/******************************************************/
/*  Definition Line                                   */
/******************************************************/

CharPtr LIBCALL SHoundGetDefline(Int4  Gi)
{

  Char fpath[PATH_MAX];
  CharPtr pdef = NULL;


  if (Gi <= 0)
    {
      ErrPostEx(SEV_ERROR,0,0, "Invalid GI.");
      return NULL;
    }

  sprintf(fpath,"%s?fnct=SeqHoundGetDefline&gi=%ld", IsNullStr(slri_cgipath), (long) Gi);
  ErrPostEx(SEV_INFO,0,0, "SeqHoundGetDefline request: %s.\n", IsNullStr(fpath));
  if(SHoundWWWGetfile(slri_sername, fpath) == 0)
    {
      ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
      return NULL;
    }
  pdef = ReplyBSGetString();
  if (pdef == NULL)
    {
      ErrPostEx(SEV_INFO,0,0, "SeqHoundAccFromGi returned NULL.");
      return NULL;
    }
  return pdef;
}

ValNodePtr LIBCALL SHoundGetDeflineList(ValNodePtr pvnGi)
{
  CharPtr pvalstr = NULL;
  ValNodePtr pvndef = NULL;
  CharPtr fpath=NULL;
  ValNodePtr pvntemp = NULL, pvnshort = NULL, pvnfin = NULL;
  Int4 counter = 0;

  if(pvnGi == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "Invalid parameter.");
      return NULL;
    }

  pvntemp = pvnGi;
  while (pvntemp != NULL)
    {
      counter++;
      pvnshort = Slri_ValNodeAddInt(&pvnshort, 0, pvntemp->data.intvalue);
      if(counter == MAXQ || pvntemp->next == NULL)
	{

	  pvalstr = Misc_SaveValNodeListAsString(pvnshort);
	  if(pvalstr == NULL)
	    {
	      ErrPostEx(SEV_ERROR,0,0, "Misc_SaveValNodeListAsString returned NULL.");
	      pvnshort = ValNodeFree(pvnshort);
	      return NULL;
	    }
	  fpath = MemNew((size_t)(StringLen(pvalstr)+PATH_MAX+100)*sizeof(Char));
	  if(fpath == NULL)
	    {
	      ErrPostEx(SEV_ERROR,0,0, "MemNew returned NULL.");
	      pvalstr = MemFree(pvalstr);
	      pvnshort = ValNodeFree(pvnshort);
	      return NULL;
	    }
	  sprintf(fpath,"%s?fnct=SeqHoundGetDeflineList&pgi=%s", IsNullStr(slri_cgipath), IsNullStr(pvalstr));
	  ErrPostEx(SEV_INFO,0,0, "SeqHoundGetDeflineList request: %s.\n", IsNullStr(fpath));
	  pvalstr = MemFree(pvalstr);
	  if(SHoundWWWGetfile(slri_sername, fpath) == 0)
	    {
	      ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
	      fpath = MemFree(fpath);
	      pvnshort = ValNodeFree(pvnshort);
	      return NULL;
	    }
	  pvndef = ReplyBSGetValNode(FALSE);
	  fpath = MemFree(fpath);
	  ValNodeLink(&pvnfin, pvndef);
	  counter = 0;
	  pvnshort = ValNodeFree(pvnshort);
	  fpath = MemFree(fpath);
	}
      pvntemp = pvntemp->next;
    }
  Misc_StringVnpUnique(&pvnfin);
  return pvnfin;
}


/************************************************/
/* Sequence and Structure checks                */
/************************************************/

/*SHoundMoleculeType takes a gi and returns the type of molecule the gi is.
 *Return:
 * protein if given gi is the type of a protein
 * dna if given gi is the type of a dna
 * rna if given gi is the type of a rna
 * else returns null
 *
 * this function deprecates SHoundIsProtein
 */
CharPtr LIBCALL SHoundMoleculeType (Int4 Gi)
{
  Char fpath[PATH_MAX];
  if (Gi<=0)
  { 
    ErrPostEx(SEV_ERROR,0,0,"[SHoundMoleculeType]: Invalid GI.");
    return NULL;
  }
  sprintf(fpath, "%s?fnct=SeqHoundMoleculeType&gi=%ld", IsNullStr(slri_cgipath), (long)Gi);
  ErrPostEx(SEV_INFO,2,1,"SeqHoundMoleculeType request: %s\n", IsNullStr(fpath));
  if(SHoundWWWGetfile(slri_sername, fpath) ==0)
  {
    ErrPostEx(SEV_ERROR,0,0,"[SHoundMoleculeType]: SHoundWWWGetfile failed.");
    return NULL;
  }
  return ReplyBSGetString();
} 


Boolean LIBCALL SHoundIsProtein(Int4 Gi)
{
  Char fpath[PATH_MAX];

  if (Gi <= 0)
    {
      ErrPostEx(SEV_ERROR,0,0, "Invalid GI.");
      return FALSE;
    }
  sprintf(fpath,"%s?fnct=SeqHoundIsProtein&gi=%ld", IsNullStr(slri_cgipath), (long) Gi);
  ErrPostEx(SEV_INFO,2,1,"SeqHoundIsProtein request: %s\n", IsNullStr(fpath));
  if(SHoundWWWGetfile(slri_sername, fpath) == 0)
    {
      ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
      return FALSE;
    }
  if(ReplyBSTrueorFalse())
    return TRUE;
  else
    return FALSE;
  
}


Boolean LIBCALL SHoundIsNRFirst(Int4 Gi)
{
  Char fpath[PATH_MAX];
  
  if (Gi <= 0)
    {
      ErrPostEx(SEV_ERROR,0,0, "Invalid GI.");
      return FALSE;
    }
  
  sprintf(fpath,"%s?fnct=SeqHoundIsNRFirst&gi=%ld", IsNullStr(slri_cgipath), (long) Gi);
  ErrPostEx(SEV_INFO,2,1,"SeqHoundIsNRFirst request: %s\n", IsNullStr(fpath));
  if(SHoundWWWGetfile(slri_sername, fpath) == 0)
    {
      ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
      return FALSE;
    }
  if(ReplyBSTrueorFalse())
    return TRUE;
  else
    return FALSE;
  
}



Boolean LIBCALL SHoundExists(Int4 Gi)
{
  Char fpath[PATH_MAX];
  
  if (Gi <= 0)
    {
      ErrPostEx(SEV_ERROR,0,0, "Invalid GI.");
      return FALSE;
    }
  
  sprintf(fpath,"%s?fnct=SeqHoundExists&gi=%ld", IsNullStr(slri_cgipath), (long) Gi);
  ErrPostEx(SEV_INFO,2,1,"SeqHoundExists request: %s\n", IsNullStr(fpath));
  if(SHoundWWWGetfile(slri_sername, fpath) == 0)
    {
      ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
      return FALSE;
    }
  if(ReplyBSTrueorFalse())
    return TRUE;
  else
    return FALSE;
}


Int4 LIBCALL SHound3DbWhat(Int4 mmdbid)
{

     Char fpath[PATH_MAX];
     Int4 bwhat = 0;

     if (mmdbid <= 0)
      {
        ErrPostEx(SEV_ERROR,0,0, "Invalid GI.");
        return FALSE;
      }
     sprintf(fpath,"%s?fnct=SeqHound3DbWhat&mmdbid=%ld", IsNullStr(slri_cgipath), (long) mmdbid);
     ErrPostEx(SEV_INFO,2,1,"SeqHound3DbWhat request: %s\n", IsNullStr(fpath));
     if(SHoundWWWGetfile(slri_sername, fpath) == 0)
      {
         ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
         return 0;
      }
     bwhat = ReplyBSGetInteger();
     if (bwhat == 0) 
      {
        ErrPostEx(SEV_INFO,0,0, "SeqHound3DbWhat returned zero.");
        return 0;
      }
     return bwhat;
}


Int4 LIBCALL SHoundGetLargestMMDBID(void)
{

     Char fpath[PATH_MAX];
     Int4 mmdbid = 0;

     sprintf(fpath,"%s?fnct=SeqHoundGetLargestMMDBID", IsNullStr(slri_cgipath));
     ErrPostEx(SEV_INFO,0,0,"SeqHoundGetLargestMMDBID request: %s\n", IsNullStr(fpath));
     if(SHoundWWWGetfile(slri_sername, fpath) == 0)
      {
         ErrPostEx(SEV_ERROR,0,0, "SeqHoundGetLargestMMDBID: SHoundWWWGetfile failed.");
         return 0;
      }
     mmdbid = ReplyBSGetInteger();
     if (mmdbid == 0 || mmdbid == -1) 
      {
        ErrPostEx(SEV_INFO,0,0, "SeqHoundGetLargestMMDBID: ReplyBSGetInterger return error.");
        return -1;
      }
     return mmdbid;
}

Boolean LIBCALL SHound3DExists(Int4 mmdbid)
{

     Char fpath[PATH_MAX];

     if (mmdbid <= 0)
      {
        ErrPostEx(SEV_ERROR,0,0, "Invalid MMDBID.");
        return FALSE;
      }

     sprintf(fpath,"%s?fnct=SeqHound3DExists&mmdbid=%ld", IsNullStr(slri_cgipath), (long) mmdbid);
     ErrPostEx(SEV_INFO,2,1,"SeqHound3DExists request: %s\n", IsNullStr(fpath));
     if(SHoundWWWGetfile(slri_sername, fpath) == 0)
      {
         ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
         return FALSE;
      }
     if(ReplyBSTrueorFalse())
       return TRUE;
     else
       return FALSE;
}


Int4  LIBCALL SHoundSequenceLength(Int4 Gi)
{
  Char fpath[PATH_MAX];
  Int2 ercode = 0;

  if(Gi <= 0) {
    ErrPostEx(SEV_ERROR,0,0, "Invalid parameter.");
    return 0;
  }
  sprintf(fpath,"%s?fnct=SeqHoundSequenceLength&gi=%ld", IsNullStr(slri_cgipath), (long) Gi);
  ErrPostEx(SEV_INFO,0,0, "SeqHoundSequenceLength request: %s.\n", IsNullStr(fpath));
  if((ercode = SHoundWWWGetfile(slri_sername, fpath)) == 0) {
    ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
    return 0;
  } else if (ercode == 2) {
    return -1;
  }
  return ReplyBSGetInteger();
}

/************************************************/
/* Sequence fetch - Bioseq                      */
/************************************************/
BioseqPtr LIBCALL SHoundGetBioseq(Int4 gi)
{

     BioseqPtr pbs = NULL;
     Char fpath[PATH_MAX];
     Int2 ercode = 0;
     AsnIoBSPtr aipbs = NULL;

     if (gi <= 0)
      {
        ErrPostEx(SEV_ERROR,0,0, "Invalid GI.");
        return NULL;
      }

     sprintf(fpath,"%s?fnct=SeqHoundGetBioseq&gi=%ld", IsNullStr(slri_cgipath),(long) gi);
     ErrPostEx(SEV_INFO,2,1,"SeqHoundGetBioseq request: %s\n", IsNullStr(fpath));

     if((ercode = SHoundWWWGetfile(slri_sername, fpath)) == 0)
      {
         ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
         return NULL;
      }
     else if (ercode == 2)
         return NULL;
     aipbs = AsnIoBSOpen(FILE_MODE, replyBS);
       if (aipbs == NULL)
        {
          ErrPostEx(SEV_ERROR,2,1,"AsnIoBSOpen returned NULL.");
          return NULL;
        }
     pbs = BioseqAsnRead(aipbs->aip, NULL);
     if (pbs == NULL)
      {
        ErrPostEx(SEV_ERROR,2,1,"BioseqAsnRead returned NULL.");
        AsnIoBSClose(aipbs);
        BSFree(replyBS);
        return NULL;
      }
     AsnIoBSClose(aipbs);
     BSFree(replyBS);
     return pbs;

}



ValNodePtr LIBCALL SHoundGetBioseqList(ValNodePtr pvnGi)
{
  ValNodePtr pvnbs = NULL, pvnbsfin = NULL;
  CharPtr fpath=NULL;
  CharPtr pvalstr = NULL;
  Int2 ercode = 0;
  AsnIoBSPtr aipbs = NULL;
  ValNodePtr pvntemp = NULL, pvnshort = NULL;
  Int4 counter = 0;

  if (pvnGi == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "Invalid parameter.");
      return NULL;
    }
  
  pvntemp = pvnGi;
  while (pvntemp != NULL)
    {
      counter++;
      pvnshort = Slri_ValNodeAddInt(&pvnshort, 0, pvntemp->data.intvalue);
      if(counter == MAXQ || pvntemp->next == NULL)
	{
	  pvalstr = Misc_SaveValNodeListAsString(pvnshort);
	  if(pvalstr == NULL)
	    {
	      ErrPostEx(SEV_ERROR,0,0, "Misc_SaveValNodeListAsString returned NULL.");
	      pvnshort = ValNodeFree(pvnshort);
	      return NULL;
	    }
	  fpath = MemNew((size_t)(StringLen(pvalstr)+PATH_MAX+100)*sizeof(Char));
	  if(fpath==NULL)
	    {
	      ErrPostEx(SEV_ERROR,0,0, "MemNew returned NULL.");
	      pvalstr = MemFree(pvalstr);
	      pvnshort = ValNodeFree(pvnshort);
	      return NULL;
	    }
	  sprintf(fpath,"%s?fnct=SeqHoundGetBioseqList&pgi=%s", IsNullStr(slri_cgipath), IsNullStr(pvalstr));
	  ErrPostEx(SEV_INFO,2,1,"SeqHoundGetBioseqList request: %s\n", IsNullStr(fpath));
	  pvalstr = MemFree(pvalstr);
	  if((ercode = SHoundWWWGetfile(slri_sername, fpath)) == 0)
	    {
	      ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
	      fpath = MemFree(fpath);
	      pvnshort = ValNodeFree(pvnshort);
	      return NULL;
	    }
	  else if (ercode == 2)
	    {
	      fpath = MemFree(fpath);
	      pvnshort = ValNodeFree(pvnshort);
	      return NULL;
	    }
	  fpath = MemFree(fpath);
	  aipbs = AsnIoBSOpen(FILE_MODE, replyBS);
	  if (aipbs == NULL)
	    {
	      ErrPostEx(SEV_ERROR,2,1,"AsnIoBSOpen returned NULL.");
	      pvnshort = ValNodeFree(pvnshort);
	      return NULL;
	    }
	  pvnbs = SLRIValNodeAsnRead(aipbs->aip, NULL);
	  if (pvnbs == NULL)
	    {
	      ErrPostEx(SEV_ERROR,2,1,"SLRIValNodeAsnRead returned NULL.");
	      AsnIoBSClose(aipbs);
	      BSFree(replyBS);
	      pvnshort = ValNodeFree(pvnshort);
	      return NULL;
	    }
	  AsnIoBSClose(aipbs);
	  BSFree(replyBS);
	  pvnshort = ValNodeFree(pvnshort);
	  ValNodeLink(&pvnbsfin, pvnbs);
	  counter = 0;
	}
      pvntemp = pvntemp->next;
    }
  return pvnbsfin;
}


ValNodePtr LIBCALL FreeBioseqList(ValNodePtr pvnBioseq)
{
  ValNodePtr pvnBioseqNew = NULL;
  ValNodePtr pvnhead = NULL;

  pvnhead = pvnBioseq;
  while (pvnBioseq != NULL)
    {
      pvnBioseqNew = pvnBioseq->next;
      BioseqFree(pvnBioseq->data.ptrvalue);
      pvnBioseq = pvnBioseqNew;
    }
  ValNodeFree(pvnhead);
  return (ValNodePtr) NULL;
}






/************************************************/
/* Sequence fetch - BioseqSet                   */
/************************************************/
BioseqSetPtr LIBCALL SHoundGetBioseqSet(Int4 gi)
{
     BioseqSetPtr pbs = NULL;
     Char fpath[PATH_MAX];
     Int2 ercode = 0;
     AsnIoBSPtr aipbs = NULL;

     if (gi <= 0)
      {
        ErrPostEx(SEV_ERROR,0,0, "Invalid GI.");
        return NULL;
      }

     sprintf(fpath,"%s?fnct=SeqHoundGetBioseqSet&gi=%ld", IsNullStr(slri_cgipath),(long) gi);
     ErrPostEx(SEV_INFO,2,1,"SeqHoundGetBioseqSet request: %s\n", IsNullStr(fpath));


     if((ercode = SHoundWWWGetfile(slri_sername, fpath)) == 0)
      {
         ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
         return NULL;
      }
     else if (ercode == 2)
         return NULL;
     aipbs = AsnIoBSOpen(FILE_MODE, replyBS);
       if (aipbs == NULL)
        {
          ErrPostEx(SEV_ERROR,2,1,"AsnIoBSOpen returned NULL.");
          return NULL;
        }
     pbs = BioseqSetAsnRead(aipbs->aip, NULL);
     if (pbs == NULL)
      {
        ErrPostEx(SEV_ERROR,2,1,"BioseqAsnRead returned NULL.");
        AsnIoBSClose(aipbs);
        BSFree(replyBS);
        return NULL;
      }
     AsnIoBSClose(aipbs);
     BSFree(replyBS);
     return pbs;
}



ValNodePtr LIBCALL SHoundGetBioseqSetList(ValNodePtr pvnGi)
{
  ValNodePtr pvnbs = NULL, pvnbsfin = NULL;
  CharPtr fpath=NULL;
  CharPtr pvalstr = NULL;
  Int2 ercode = 0;
  AsnIoBSPtr aipbs = NULL;
  ValNodePtr pvntemp = NULL, pvnshort = NULL;
  Int4 counter = 0;
  
  if (pvnGi == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "Invalid parameter.");
      return NULL;
    }
  
  pvntemp = pvnGi;
  while (pvntemp != NULL)
    {
      counter++;
      pvnshort = Slri_ValNodeAddInt(&pvnshort, 0, pvntemp->data.intvalue);
      if(counter == MAXQ || pvntemp->next == NULL)
	{
	  pvalstr = Misc_SaveValNodeListAsString(pvnshort);
	  if(pvalstr == NULL)
	    {
	      ErrPostEx(SEV_ERROR,0,0, "Misc_SaveValNodeListAsString returned NULL.");
	      pvnshort = ValNodeFree(pvnshort);
	      return NULL;
	    }
	  fpath = MemNew((size_t)(StringLen(pvalstr)+PATH_MAX+100)*sizeof(Char));
	  if(fpath==NULL)
	    {
	      ErrPostEx(SEV_ERROR,0,0, "MemNew returned NULL.");
	      pvalstr = MemFree(pvalstr);
	      pvnshort = ValNodeFree(pvnshort);
	      return NULL;
	    }
	  sprintf(fpath,"%s?fnct=SeqHoundGetBioseqSetList&pgi=%s", IsNullStr(slri_cgipath), IsNullStr(pvalstr));
	  ErrPostEx(SEV_INFO,2,1,"SeqHoundGetBioseqSetList request: %s\n", IsNullStr(fpath));
	  pvalstr = MemFree(pvalstr);
	  if((ercode = SHoundWWWGetfile(slri_sername, fpath)) == 0)
	    {
	      ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
	      fpath = MemFree(fpath);
	      pvnshort = ValNodeFree(pvnshort);
	      return NULL;
	    }
	  else if (ercode == 2)
	    {
	      fpath = MemFree(fpath);
	      pvnshort = ValNodeFree(pvnshort);
	      return NULL;
	    }
	  fpath = MemFree(fpath);
	  aipbs = AsnIoBSOpen(FILE_MODE, replyBS);
	  if (aipbs == NULL)
	    {
	      ErrPostEx(SEV_ERROR,2,1,"AsnIoBSOpen returned NULL.");
	      pvnshort = ValNodeFree(pvnshort);
	      return NULL;
	    }
	  pvnbs = SLRIValNodeAsnRead(aipbs->aip, NULL);
	  if (pvnbs == NULL)
	    {
	      ErrPostEx(SEV_ERROR,2,1,"SLRIValNodeAsnRead returned NULL.");
	      AsnIoBSClose(aipbs);
	      BSFree(replyBS);
	      pvnshort = ValNodeFree(pvnshort);
	      return NULL;
	    }
	  AsnIoBSClose(aipbs);
	  BSFree(replyBS);
	  pvnshort = ValNodeFree(pvnshort);
	  ValNodeLink(&pvnbsfin, pvnbs);
	  counter = 0;
	}
      pvntemp = pvntemp->next;
    }
  return pvnbsfin;
}




ValNodePtr LIBCALL FreeBioseqSetList(ValNodePtr pvnBioseqSet)
{
  ValNodePtr pvnBioseqSetNew = NULL;
  ValNodePtr pvnhead = NULL;
  
  pvnhead = pvnBioseqSet;
  while (pvnBioseqSet != NULL)
    {
      pvnBioseqSetNew = pvnBioseqSet->next;
      BioseqSetFree(pvnBioseqSet->data.ptrvalue);
      pvnBioseqSet = pvnBioseqSetNew;
    }
  ValNodeFree(pvnhead);
  return (ValNodePtr) NULL;
}



/************************************************/
/* Sequence fetch - SeqEntry                      */
/************************************************/
SeqEntryPtr LIBCALL SHoundGetSeqEntry(Int4 gi)
{
  SeqEntryPtr pse = NULL;
  Char fpath[PATH_MAX];
  Int2 ercode = 0;
  AsnIoBSPtr aipbs = NULL;
  
  
  if (gi <= 0)
    {
      ErrPostEx(SEV_ERROR,0,0, "Invalid GI.");
      return NULL;
    }
  
  sprintf(fpath,"%s?fnct=SeqHoundGetSeqEntry&gi=%ld", IsNullStr(slri_cgipath),(long) gi);
  ErrPostEx(SEV_INFO,2,1,"SeqHoundGetSeqEntry request: %s\n", IsNullStr(fpath));
  
  
  if((ercode = SHoundWWWGetfile(slri_sername, fpath)) == 0)
    {
      ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
      return NULL;
    }
  else if (ercode == 2)
    return NULL;
  aipbs = AsnIoBSOpen(FILE_MODE, replyBS);
  if (aipbs == NULL)
    {
      ErrPostEx(SEV_ERROR,2,1,"AsnIoBSOpen returned NULL.");
      return NULL;
    }
  
  pse = SeqEntryAsnRead(aipbs->aip, NULL);
  if (pse == NULL)
    {
      ErrPostEx(SEV_ERROR,2,1,"SeqEntryAsnRead returned NULL.");
      AsnIoBSClose(aipbs);
      BSFree(replyBS);
      return NULL;
    }
  AsnIoBSClose(aipbs);
  BSFree(replyBS);
  return pse;

}



ValNodePtr LIBCALL SHoundGetSeqEntryList(ValNodePtr pvnGi)
{
     ValNodePtr pvnbs = NULL, pvnbsfin = NULL;
     CharPtr fpath=NULL;
     CharPtr pvalstr = NULL;
     Int2 ercode = 0;
     AsnIoBSPtr aipbs = NULL;
     ValNodePtr pvntemp = NULL, pvnshort = NULL;
     Int4 counter = 0;

     if (pvnGi == NULL)
      {
        ErrPostEx(SEV_ERROR,0,0, "Invalid parameter.");
        return NULL;
      }

     pvntemp = pvnGi;
     while (pvntemp != NULL)
      {
         counter++;
         pvnshort = Slri_ValNodeAddInt(&pvnshort, 0, pvntemp->data.intvalue);
         if(counter == MAXQ || pvntemp->next == NULL)
          {
             pvalstr = Misc_SaveValNodeListAsString(pvnshort);
             if(pvalstr == NULL)
              {
                ErrPostEx(SEV_ERROR,0,0, "Misc_SaveValNodeListAsString returned NULL.");
                pvnshort = ValNodeFree(pvnshort);
                return NULL;
              }
             fpath = MemNew((size_t)(StringLen(pvalstr)+PATH_MAX+100)*sizeof(Char));
             if(fpath==NULL)
              {
                ErrPostEx(SEV_ERROR,0,0, "MemNew returned NULL.");
                pvalstr = MemFree(pvalstr);
                pvnshort = ValNodeFree(pvnshort);
                return NULL;
              }
             sprintf(fpath,"%s?fnct=SeqHoundGetSeqEntryList&pgi=%s", IsNullStr(slri_cgipath), IsNullStr(pvalstr));
             ErrPostEx(SEV_INFO,2,1,"SeqHoundGetSeqEntryList request: %s\n", IsNullStr(fpath));
             pvalstr = MemFree(pvalstr);
             if((ercode = SHoundWWWGetfile(slri_sername, fpath)) == 0)
              {
                 ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
                 fpath = MemFree(fpath);
                 pvnshort = ValNodeFree(pvnshort);
                 return NULL;
              }
             else if (ercode == 2)
              {
                  fpath = MemFree(fpath);
                  pvnshort = ValNodeFree(pvnshort);
                  return NULL;
              }
             fpath = MemFree(fpath);
             aipbs = AsnIoBSOpen(FILE_MODE, replyBS);
               if (aipbs == NULL)
                {
                  ErrPostEx(SEV_ERROR,2,1,"AsnIoBSOpen returned NULL.");
                  pvnshort = ValNodeFree(pvnshort);
                  return NULL;
                }
             pvnbs = SLRIValNodeAsnRead(aipbs->aip, NULL);
             if (pvnbs == NULL)
              {
                ErrPostEx(SEV_ERROR,2,1,"SLRIValNodeAsnRead returned NULL.");
                AsnIoBSClose(aipbs);
                BSFree(replyBS);
                pvnshort = ValNodeFree(pvnshort);
                return NULL;
              }
             AsnIoBSClose(aipbs);
             BSFree(replyBS);
             pvnshort = ValNodeFree(pvnshort);
             ValNodeLink(&pvnbsfin, pvnbs);
             counter = 0;
          }
         pvntemp = pvntemp->next;
      }
     return pvnbsfin;
}


ValNodePtr LIBCALL FreeSeqEntryList(ValNodePtr pvnSeqEntry)
{
  ValNodePtr pvnSeqEntryNew = NULL;
  ValNodePtr pvnhead = NULL;
  
  pvnhead = pvnSeqEntry;
  while (pvnSeqEntry != NULL)
    {
      pvnSeqEntryNew = pvnSeqEntry->next;
      SeqEntryFree(pvnSeqEntry->data.ptrvalue);
      pvnSeqEntry = pvnSeqEntryNew;
    }
  ValNodeFree(pvnhead);
  return (ValNodePtr) NULL;
}



/************************************************/
/* Sequence fetch - Fasta                      */
/************************************************/

void NetEntrezGetFasta(Int4 gi)
{
   CONNECTOR connector;
   SConnNetInfo* net_info;
   THCC_Flags flags = 0;
   CONN conn;
   size_t n;
   Char blob[1000];
   EIO_Status status;
   /* CharPtr accession = NULL; */
   CharPtr url = "http://www.ncbi.nlm.nih.gov/entrez/eutils/efetch.fcgi?db=protein&rettype=fasta&id=";
   CharPtr the_url = NULL;
   
   the_url = MemNew((StringLen(url) * sizeof(Char)) + 20);
   if(!the_url)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "Couldnot allocate memory");
      return;
   }
   sprintf(the_url, "%s%ld", url, (long)gi);

   if(!(net_info = ConnNetInfo_Create(0)))
   {
      ErrPostEx(SEV_ERROR, 0, 0, "NetEntrez not working.");
      return ;
   }
   if(!(ConnNetInfo_ParseURL(net_info, the_url)))
   {
      ErrPostEx(SEV_ERROR, 0, 0, "NetEntrez not working.");
      return ;
   }
   if(!(connector = HTTP_CreateConnector(net_info, 0, flags)))
   {
      ErrPostEx(SEV_ERROR, 0, 0, "NetEntrez not working.");
      return ;
   }
   if(CONN_Create(connector, &conn) != eIO_Success)
   {
      ErrPostEx(SEV_ERROR, 0, 0, "NetEntrez not working.");
      return ;
   }
   replyBS = BSNew(10); 
   for(;;){
      status = CONN_Read(conn, (void*)blob, (size_t)sizeof(blob), &n, eIO_Plain);
      if(status != eIO_Success) break;
      BSWrite(replyBS, (Nlm_VoidPtr) blob, (size_t)n);    
   }
}



SLRIFastaPtr LIBCALL SHoundGetFasta(Int4 Gi)
{
  Char fpath[PATH_MAX];
  AsnIoBSPtr aipbs = NULL;
  SLRIFastaPtr pfa = NULL;
  Int2 ercode = 0;
  if (Gi <= 0)
    {
      ErrPostEx(SEV_ERROR,0,0, "Invalid GI.");
      return NULL;
    }
  
  sprintf(fpath,"%s?fnct=SeqHoundGetFasta&gi=%ld", IsNullStr(slri_cgipath), (long) Gi);
  ErrPostEx(SEV_INFO,2,1,"SeqHoundGetFasta request: %s\n", IsNullStr(fpath));
  if((ercode = SHoundWWWGetfile(slri_sername, fpath)) == 0)
    {
      ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
      return NULL;
    }
  else if (ercode == 2)
    return NULL;
  
  aipbs = AsnIoBSOpen("r", replyBS);
  if (aipbs == NULL)
    {
      ErrPostEx(SEV_ERROR,2,1,"AsnIoBSOpen returned NULL.");
      return NULL;
    }
  pfa = (SLRIFastaPtr)SLRIFastaAsnRead(aipbs->aip, NULL);
  
  if(pfa == NULL && NetEntrez)
  {
     Char defline[1000];
     Char sequence[1000];
     CharPtr p, ps;
     NetEntrezGetFasta(Gi);
     p = BSStringSave(replyBS);
     pfa = SLRIFastaNew();
     ps = strtok(p, "\n");
     strcpy(defline, ps); pfa->defline = defline;
     while( (ps = strtok(NULL, "\n")) != NULL )
	     strcat(sequence, ps);
     pfa->sequence = sequence;
  } else if (pfa == NULL){
     ErrPostEx(SEV_ERROR,2,1,"SLRIFastaAsnRead returned NULL.");
     AsnIoBSClose(aipbs);
     BSFree(replyBS);	
     return NULL;
  }
  AsnIoBSClose(aipbs);
  BSFree(replyBS);	
  return pfa;
}



ValNodePtr LIBCALL SHoundGetFastaList(ValNodePtr pvnGi)
{

  ValNodePtr pvnfasta = NULL, pvnfastafin = NULL;
  CharPtr fpath = NULL;
  CharPtr pvalstr = NULL;
  Int2 ercode = 0;
  AsnIoBSPtr aipbs = NULL;
  ValNodePtr pvntemp = NULL, pvnshort = NULL;
  Int4 counter = 0;
  
  
  if (pvnGi == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "Invalid parameter.");
      return NULL;
    }
  pvntemp = pvnGi;
  while (pvntemp != NULL)
    {
      counter++;
      pvnshort = Slri_ValNodeAddInt(&pvnshort, 0, pvntemp->data.intvalue);
      if(counter == MAXQ || pvntemp->next == NULL)
	{
	  pvalstr = Misc_SaveValNodeListAsString(pvnshort);
	  if(pvalstr == NULL)
	    {
	      ErrPostEx(SEV_ERROR,0,0, "Misc_SaveValNodeListAsString returned NULL.");
	      pvnshort = ValNodeFree(pvnshort);
	      return NULL;
	    }
	  pvnshort = ValNodeFree(pvnshort);
	  fpath = MemNew((size_t)(StringLen(pvalstr)+PATH_MAX + 100)*sizeof(Char));
	  if(fpath==NULL)
	    {
	      ErrPostEx(SEV_ERROR,0,0, "MemNew returned NULL.");
	      pvalstr = MemFree(pvalstr);
	      return NULL;
	    }
	  sprintf(fpath,"%s?fnct=SeqHoundGetFastaList&pgi=%s", IsNullStr(slri_cgipath), IsNullStr(pvalstr));
	  ErrPostEx(SEV_INFO,2,1,"SeqHoundGetFastaList request: %s\n", IsNullStr(fpath));
	  pvalstr = MemFree(pvalstr);
	  if((ercode = SHoundWWWGetfile(slri_sername, fpath)) == 0)
	    {
	      ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
	      fpath = MemFree(fpath);
	      return NULL;
	    }
	  else if (ercode == 2)
	    {
	      fpath = MemFree(fpath);
	      BSFree(replyBS);
	      return NULL;
	    }
	  aipbs = AsnIoBSOpen("r", replyBS);
	  if (aipbs == NULL)
	    {
	      ErrPostEx(SEV_ERROR,2,1,"AsnIoBSOpen returned NULL.");
	      fpath = MemFree(fpath);
	      BSFree(replyBS);
	      return NULL;
	    }
	  fpath = MemFree(fpath);
	  pvnfasta = (SLRIValNodePtr) SLRIValNodeAsnRead(aipbs->aip, NULL);
	  if (pvnfasta == NULL)
	    {
	      ErrPostEx(SEV_ERROR,2,1,"SLRIValNodeAsnRead returned NULL.");
	      AsnIoBSClose(aipbs);
	      BSFree(replyBS);	
	      return NULL;
	    }
	  ValNodeLink(&pvnfastafin, pvnfasta);
	  counter = 0;
	  AsnIoBSClose(aipbs);
	  BSFree(replyBS);
	}
      pvntemp = pvntemp->next;
    }
  return pvnfastafin;
}


ValNodePtr LIBCALL FreeFastaList(ValNodePtr pvnFasta)
{
  
  ValNodePtr pvnFastaNew = NULL;
  ValNodePtr pvnhead = NULL;
  
  pvnhead = pvnFasta;
  while (pvnFasta != NULL)
    {
      pvnFastaNew = pvnFasta->next;
      SLRIFastaFree(pvnFasta->data.ptrvalue);
      pvnFasta = pvnFastaNew;
    }
  ValNodeFree(pvnhead);
  return (ValNodePtr) NULL;
}


/* chops up a sequence string into lines of length linelen and writes it out in a file pfile */	
Boolean LIBCALL PrintSLRIFastaSequence(CharPtr pseq, Int4 linelen, FILE *pfile)	
{
  CharPtr ptemp = NULL;
  Int4 count = 0;
  
  
  if(pseq == NULL)
    {
      ErrPostEx(SEV_ERROR,2,1,"PrintSLRIFastaSequence:Invalid parameter.");
      return FALSE;
    }
  ptemp = pseq;
  
  while(ptemp[0] != '\0')
    {
      fprintf(pfile, "%c", ptemp[0]);
      count++;
      if(count == linelen)
	{
	  fprintf(pfile, "\n");
	  count = 0;
	}
      ptemp++;
    }
  fprintf(pfile, "\n");
  
  
  return TRUE;
}

Boolean LIBCALL PrintSLRIFasta(SLRIFastaPtr pfa, Int4 linelen, FILE *pfile)	
{
  
  if((pfa == NULL)|| (pfile == NULL))
    {
      ErrPostEx(SEV_ERROR,2,1,"PrintSLRIFasta:Invalid parameter.");
      return FALSE;
    }
  if(linelen == 0) linelen = 80;
  
  if((pfa->defline != NULL) && (pfa->sequence == NULL))
    {
      fprintf(pfile, ">%s\n", IsNullStr(pfa->defline));
    }
  if((pfa->defline != NULL) && (pfa->sequence != NULL))
    {
      fprintf(pfile, ">%s\n", IsNullStr(pfa->defline));
      if(!PrintSLRIFastaSequence(pfa->sequence, linelen, pfile))
	{
	  ErrPostEx(SEV_ERROR,2,1,"PrintSLRIFasta:PrintSLRIFastaSequence failed.");
	  return FALSE;
	}
    }
  return TRUE;
  
}

Boolean LIBCALL PrintSLRIFastaList (ValNodePtr pvnfa, Int4 linelen, FILE *pfile)	
{

  SLRIFastaPtr pfa = NULL;
  
  if((pvnfa == NULL)|| (pfile == NULL))
    {
      ErrPostEx(SEV_ERROR,2,1,"PrintSLRIFasta:Invalid parameter.");
      return FALSE;
    }
  if(linelen == 0) linelen = 80;

  
  while(pvnfa != NULL)
  {
    pfa = (SLRIFastaPtr) pvnfa->data.ptrvalue;
    PrintSLRIFasta(pfa, linelen, pfile);
    pvnfa = pvnfa->next;
  }
  return TRUE;
  
}


/*retrieve SLRIFasta stucture. bug [2929]*/
SLRIFastaPtr LIBCALL IntrezGetCDSLRIFasta(Int4 Gi)
{
  Char fpath[PATH_MAX];
  AsnIoBSPtr aipbs = NULL;
  SLRIFastaPtr pfa = NULL;
  Int2 ercode = 0;
  if (Gi <= 0)
    {
      ErrPostEx(SEV_ERROR,0,0, "Invalid GI.");
      return NULL;
    }
  
  sprintf(fpath,"%s?fnct=SeqHoundIntrezGetCDSLRIFasta&gi=%ld", IsNullStr(slri_cgipath), (long) Gi);
  ErrPostEx(SEV_INFO,2,1,"SeqHoundIntrezGetCDSLRIFasta request: %s\n", IsNullStr(fpath));
  if((ercode = SHoundWWWGetfile(slri_sername, fpath)) == 0)
    {
      ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
      return NULL;
    }
  else if (ercode == 2)
    return NULL;
  
  aipbs = AsnIoBSOpen("r", replyBS);
  if (aipbs == NULL)
    {
      ErrPostEx(SEV_ERROR,2,1,"AsnIoBSOpen returned NULL.");
      return NULL;
    }
  pfa = (SLRIFastaPtr)SLRIFastaAsnRead(aipbs->aip, NULL);
  
  if (pfa == NULL){
     ErrPostEx(SEV_ERROR,2,1,"SLRIFastaAsnRead returned NULL.");
     AsnIoBSClose(aipbs);
     BSFree(replyBS);	
     return NULL;
  }
  AsnIoBSClose(aipbs);
  BSFree(replyBS);	
  return pfa;
}


/******************************************************/
/*  Sequence fetch - GenBank flat file                */
/* writes into a file                                 */
/******************************************************/


Boolean LIBCALL SHoundGetGenBankff(Int4 gi, FILE *pfile)
{
  Char fpath[PATH_MAX];
  Int2 ercode = 0;
  
  if (gi <= 0)
    {
      ErrPostEx(SEV_ERROR,0,0, "Invalid GI.");
      return FALSE;
    }
  
  sprintf(fpath,"%s?fnct=SeqHoundGetGenBankff&gi=%ld", IsNullStr(slri_cgipath), (long) gi);
  ErrPostEx(SEV_INFO,2,1,"SeqHoundGetGenBankff request: %s\n", IsNullStr(fpath));
       if((ercode = SHoundWWWGetfile(slri_sername, fpath)) == 0)
	 {
           ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
           return FALSE;
        }
       else if (ercode == 2)
	 return FALSE;
       
       if(PrintBS(replyBS, pfile) == 1)
	 {
	   ErrPostEx(SEV_ERROR,2,1,"PrintBS failed.");
	   BSFree(replyBS);	
	   return FALSE;
	 }

       BSFree(replyBS);	
       return TRUE;
}



Boolean LIBCALL SHoundGetGenBankffList(ValNodePtr pvnGi, FILE *pfile)
{
  
  CharPtr fpath = NULL;
  CharPtr pvalstr = NULL;
  Int2 ercode = 0;
  ValNodePtr pvntemp = NULL, pvnshort = NULL;
  Int4 counter = 0;
  
  
  if (pvnGi == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "Invalid parameter.");
      return FALSE;
    }
   pvntemp = pvnGi;
   while (pvntemp != NULL)
     {
       counter++;
       pvnshort = Slri_ValNodeAddInt(&pvnshort, 0, pvntemp->data.intvalue);
       if(counter == MAXQ || pvntemp->next == NULL)
	 {
            pvalstr = Misc_SaveValNodeListAsString(pvnshort);
            if(pvalstr == NULL)
	      {
		ErrPostEx(SEV_ERROR,0,0, "Misc_SaveValNodeListAsString returned NULL.");
		pvnshort = ValNodeFree(pvnshort);
		return FALSE;
	      }
            pvnshort = ValNodeFree(pvnshort);
            fpath = MemNew((size_t)(StringLen(pvalstr)+PATH_MAX + 100)*sizeof(Char));
            if(fpath==NULL)
	      {
		ErrPostEx(SEV_ERROR,0,0, "MemNew returned NULL.");
		pvalstr = MemFree(pvalstr);
		return FALSE;
	      }
            sprintf(fpath,"%s?fnct=SeqHoundGetGenBankffList&pgi=%s", IsNullStr(slri_cgipath), IsNullStr(pvalstr));
            ErrPostEx(SEV_INFO,2,1,"SeqHoundGetGenBankffList request: %s\n", IsNullStr(fpath));
            pvalstr = MemFree(pvalstr);
            if((ercode = SHoundWWWGetfile(slri_sername, fpath)) == 0)
	      {
                ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
                fpath = MemFree(fpath);
                return FALSE;
	      }
            else if (ercode == 2)
	      {
                fpath = MemFree(fpath);
                BSFree(replyBS);
                return FALSE;
	      }
	    
            fpath = MemFree(fpath);
            if(PrintBS(replyBS, pfile) == 1)
	      {
		ErrPostEx(SEV_ERROR,2,1,"PrintBS failed.");
		BSFree(replyBS);
		return FALSE;
	      }
	    counter = 0;
	    BSFree(replyBS);
	 }
       pvntemp = pvntemp->next;
     }
   return TRUE;
}


/******************************************************/
/*  Sequence fetch - XML format                       */
/******************************************************/

Boolean LIBCALL SHoundGetXMLBioseq(Int4 gi, CharPtr file_name)
{
  BioseqPtr pbs = NULL;
   AsnIoPtr aip = NULL;


   if (gi <= 0)
    {
       ErrPostEx(SEV_ERROR,0,0, "SHoundGetXMLBioseq: Invalid GI.");
       return FALSE;
    }
   if (file_name == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "SHoundGetXMLBioseq: Invalid file pointer.");
       return FALSE;
    }

   aip = AsnIoOpen(file_name, "wx");
   if(aip == NULL)
   {
     ErrPostEx(SEV_ERROR,0,0, "SHoundGetXMLBioseq: Cannot open %s.\n", IsNullStr(file_name));
     return FALSE;
   }
   pbs = SHoundGetBioseq(gi);
   if(!BioseqAsnWrite(pbs, aip, NULL))
    {
     ErrPostEx(SEV_ERROR,0,0, "SHoundGetXMLBioseq: BioseqAsnWrite failed.\n", file_name);
     return FALSE;
   }

   AsnIoClose(aip);
   return TRUE;
}


Boolean LIBCALL SHoundGetXMLSeqEntry(Int4 gi, CharPtr file_name)
{
    AsnIoPtr aip = NULL;
    SeqEntryPtr pse = NULL;

    if (gi <= 0)
    {
       ErrPostEx(SEV_ERROR,0,0, "SHoundGetXMLSeqEntry: Invalid GI.");
       return FALSE;
    }
   if (file_name == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "SHoundGetXMLSeqEntry: Invalid file pointer.");
       return FALSE;
    }

   aip = AsnIoOpen(file_name, "wx");
   if(aip == NULL)
   {
     ErrPostEx(SEV_ERROR,0,0, "SHoundGetXMLSeqEntry: Cannot open %s.\n", IsNullStr(file_name));
     return FALSE;
   }
   pse = SHoundGetSeqEntry(gi);
   if(!SeqEntryAsnWrite(pse, aip, NULL))
    {
     ErrPostEx(SEV_ERROR,0,0, "SHoundGetXMLSeqEntry: BioseqAsnWrite failed.\n", file_name);
     return FALSE;
   }

   AsnIoClose(aip);
   return TRUE;
}


#ifndef DocUid
#define DocUid Int4
#endif

/************************************************/
/* Structure fetch - Biostruc                   */
/************************************************/
BiostrucPtr LIBCALL SLRI_MMDBBiostrucGet(DocUid uid, Int4 mdlLvl, Int4 maxModels)
{ 
	return SHoundGet3DEx((Int4)uid,mdlLvl,maxModels);
}

BiostrucPtr LIBCALL SHoundGet3DEx(Int4 Gi,Int4 mdlLvl,Int4 maxModels)
{
       Char fpath[PATH_MAX];
       BiostrucPtr pbio = NULL;
       Int2 ercode = 0;
       AsnIoBSPtr aipbs = NULL;

       if (Gi <= 0)
        {
          ErrPostEx(SEV_ERROR,0,0, "Invalid GI.");
          return NULL;
        }

       sprintf(fpath,"%s?fnct=SeqHoundGet3DEx&mmdbid=%ld&mdlLvl=%d&maxModels=%d", IsNullStr(slri_cgipath), (long) Gi,mdlLvl,maxModels);
       ErrPostEx(SEV_INFO,2,1,"SeqHoundGet3DEx request: %s\n", IsNullStr(fpath));

       if((ercode = SHoundWWWGetfile(slri_sername, fpath)) == 0)
        {
           ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
           return NULL;
        }
       else if (ercode == 2)
         return NULL;
       aipbs = AsnIoBSOpen(FILE_MODE, replyBS);
       if (aipbs == NULL)
        {
          ErrPostEx(SEV_ERROR,2,1,"AsnIoBSOpen returned NULL.");
          return NULL;
        }
       pbio = BiostrucAsnRead(aipbs->aip, NULL);
       if (pbio == NULL)
        {
          ErrPostEx(SEV_ERROR,2,1,"BiostrucAsnRead returned NULL.");
          AsnIoBSClose(aipbs);
          BSFree(replyBS);	
          return NULL;
        }
       AsnIoBSClose(aipbs);
       BSFree(replyBS);	
       return pbio;
}

BiostrucPtr LIBCALL SHoundGet3D(Int4 Gi)
{
	return SHoundGet3DEx(Gi,ALLMDL,100);
}

BiostrucPtr LIBCALL SHoundGetFixed3D(Int4 Gi,Int4 mdlLvl, Int4 maxModels)
{
        Char fpath[PATH_MAX];
       BiostrucPtr pbio = NULL;
       Int2 ercode = 0;
       AsnIoBSPtr aipbs = NULL;

       if (Gi <= 0)
        {
          ErrPostEx(SEV_ERROR,0,0, "Invalid GI.");
          return NULL;
        }

       sprintf(fpath,"%s?fnct=SeqHoundGetFixed3D&mmdbid=%ld&mdlLvl=%d&maxModels=%d", IsNullStr(slri_cgipath), (long) Gi,mdlLvl,maxModels);
       ErrPostEx(SEV_INFO,2,1,"SeqHoundGetFixed3D request: %s\n", IsNullStr(fpath));

       if((ercode = SHoundWWWGetfile(slri_sername, fpath)) == 0)
        {
           ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
           return NULL;
        }
       else if (ercode == 2)
         return NULL;
       aipbs = AsnIoBSOpen(FILE_MODE, replyBS);
       if (aipbs == NULL)
        {
          ErrPostEx(SEV_ERROR,2,1,"AsnIoBSOpen returned NULL.");
          return NULL;
        }
       pbio = BiostrucAsnRead(aipbs->aip, NULL);
       if (pbio == NULL)
        {
          ErrPostEx(SEV_ERROR,2,1,"BiostrucAsnRead returned NULL.");
          AsnIoBSClose(aipbs);
          BSFree(replyBS);	
          return NULL;
        }
       AsnIoBSClose(aipbs);
       BSFree(replyBS);	
       return pbio;
}

BiostrucPtr LIBCALL SHoundGet3DfromPdbIdEx(CharPtr pdbid,Int4 mdlLvl,Int4 maxModels)
{
       Char fpath[PATH_MAX];
       BiostrucPtr pbio = NULL;
       Int2 ercode = 0;
       AsnIoBSPtr aipbs = NULL;

       if (pdbid == NULL)
        {
          ErrPostEx(SEV_ERROR,0,0, "Invalid parameter.");
          return NULL;
        }

       sprintf(fpath,"%s?fnct=SeqHoundGet3DfromPdbIdEx&pdbid=%s&mdlLvl=%d&maxModels=%d", IsNullStr(slri_cgipath), IsNullStr(pdbid), mdlLvl, maxModels);
       ErrPostEx(SEV_INFO,2,1,"SeqHoundGet3DfromPdbIdEx request: %s\n", IsNullStr(fpath));

       if((ercode = SHoundWWWGetfile(slri_sername, fpath)) == 0)
        {
           ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
           return NULL;
        }
       else if (ercode == 2)
         return NULL;
       aipbs = AsnIoBSOpen(FILE_MODE, replyBS);
       if (aipbs == NULL)
        {
          ErrPostEx(SEV_ERROR,2,1,"AsnIoBSOpen returned NULL.");
          return NULL;
        }
       pbio = BiostrucAsnRead(aipbs->aip, NULL);
       if (pbio == NULL)
        {
          ErrPostEx(SEV_ERROR,2,1,"BiostrucAsnRead returned NULL.");
          AsnIoBSClose(aipbs);
          BSFree(replyBS);	
          return NULL;
        }
       AsnIoBSClose(aipbs);
       BSFree(replyBS);	
       return pbio;
}


BiostrucPtr LIBCALL SHoundGet3DfromPdbId(CharPtr pdbid)
{
	return SHoundGet3DfromPdbIdEx(pdbid,ALLMDL,100);
}

Boolean LIBCALL SHoundGetXML3D(Int4 mmdbid, CharPtr file_name)
{
    AsnIoPtr aip = NULL;
    BiostrucPtr pstruc = NULL;

   if (mmdbid <= 0)
    {
       ErrPostEx(SEV_ERROR,0,0, "SHoundGetXML3D: Invalid GI.");
       return FALSE;
    }
   if (file_name == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "SHoundGetXML3D: Invalid file pointer.");
       return FALSE;
    }

   aip = AsnIoOpen(file_name, "wx");
   if(aip == NULL)
   {
     ErrPostEx(SEV_ERROR,0,0, "SHoundGetXML3D: Cannot open %s.\n", IsNullStr(file_name));
     return FALSE;
   }
   pstruc = SHoundGet3D(mmdbid);
   if(!BiostrucAsnWrite(pstruc, aip, NULL))
    {
     ErrPostEx(SEV_ERROR,0,0, "SHoundGetXML3D: BioseqAsnWrite failed.\n", file_name);
     return FALSE;
   }

   AsnIoClose(aip);
   return TRUE;
}


Boolean LIBCALL SHoundGetPDB3D(Int4 mmdbid, CharPtr file_name)
{
   BiostrucPtr pstruc = NULL;
   PDNMS pmodelstruc=NULL;
   FILE *pfile = NULL;
   Int2 result = 0;


   if (mmdbid <= 0)
    {
       ErrPostEx(SEV_ERROR,0,0, "SHoundGetPDB3D: Invalid GI.");
       return FALSE;
    }
   if (file_name == NULL)
    {
       ErrPostEx(SEV_ERROR,0,0, "SHoundGetPDB3D: Invalid file pointer.");
       return FALSE;
    }

   pfile = FileOpen(file_name, "w");
   if (pfile == NULL)
    {
      ErrPostEx(SEV_ERROR,2,1,"SHoundGetPDB3D: Unable to open the file %s.", IsNullStr(file_name));
      return FALSE;
    }


   pstruc = SHoundGet3D(mmdbid);
   if(pstruc == NULL)
    {
      ErrPostEx(SEV_ERROR,2,1,"SHoundGetPDB3D: SHoundGet3D failed.");
      FileClose(pfile);
      return FALSE;
    }
   pmodelstruc=MakeAModelstruc(pstruc);
   if(pmodelstruc == NULL)
    {
      ErrPostEx(SEV_ERROR,2,1,"SHoundGetPDB3D: MakeAModelstruc failed.");
      FileClose(pfile);
      return FALSE;
    }

   result = WritePDBAllModel(pmodelstruc, pfile);
   if(result != 1)
    {
      ErrPostEx(SEV_ERROR,2,1,"SHoundGetPDB3D: WritePDBAllModel failed.");
      ClearStructures();
      FileClose(pfile);
      return FALSE;
    }
   ClearStructures();
   FileClose(pfile);
   return TRUE;
}


/************************************************/
/* SeqHound links                               */
/************************************************/

ValNodePtr LIBCALL SHoundGiFrom3D(Int4 mmdbid, Boolean ProteinOnly)
{
     Char fpath[PATH_MAX];
     ValNodePtr pvngi = NULL;


     if(mmdbid <= 0)
      {
        ErrPostEx(SEV_ERROR,0,0, "Invalid parameter.");
        return NULL;
      }

     sprintf(fpath,"%s?fnct=SeqHoundGiFrom3D&mmdbid=%ld&ProteinOnly=%s", IsNullStr(slri_cgipath), (long) mmdbid, (ProteinOnly)?"TRUE":"FALSE");
     ErrPostEx(SEV_INFO,0,0, "SeqHoundGiFrom3D request: %s.\n", IsNullStr(fpath));
     if(SHoundWWWGetfile(slri_sername, fpath) == 0)
      {
         ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
         return NULL;
      }
     pvngi = ReplyBSGetValNode(TRUE);
     return pvngi;
}



ValNodePtr LIBCALL SHoundGiFrom3DList(ValNodePtr pvnMmdbid, Boolean ProteinOnly)
{
     CharPtr fpath=NULL;
     CharPtr pvalstr = NULL;
     ValNodePtr pvngi = NULL;
     Int4 counter = 0;
     ValNodePtr pvngifin = NULL;
     ValNodePtr pvntemp = NULL, pvnshort = NULL;

     if(pvnMmdbid == NULL)
      {
        ErrPostEx(SEV_ERROR,0,0, "Invalid parameter.");
        return NULL;
      }

     pvntemp = pvnMmdbid;
     while (pvntemp != NULL)
      {
         counter++;
         pvnshort = Slri_ValNodeAddInt(&pvnshort, 0, pvntemp->data.intvalue);
         if(counter == MAXQ || pvntemp->next == NULL)
          {
            pvalstr = Misc_SaveValNodeListAsString(pvnshort);
            if(pvalstr == NULL)
             {
               ErrPostEx(SEV_ERROR,0,0, "Misc_SaveValNodeListAsString returned NULL.");
               pvnshort = ValNodeFree(pvnshort);
               return NULL;
             }
            fpath = MemNew((size_t)(StringLen(pvalstr)+PATH_MAX+100)*sizeof(Char));
            if(fpath == NULL)
             {
               ErrPostEx(SEV_ERROR,0,0, "MemNew returned NULL.");
               pvalstr = MemFree(pvalstr);
               pvnshort = ValNodeFree(pvnshort);
               return NULL;
             }
            sprintf(fpath,"%s?fnct=SeqHoundGiFrom3DList&pmmdbid=%s&ProteinOnly=%s", IsNullStr(slri_cgipath), IsNullStr(pvalstr), (ProteinOnly)?"TRUE":"FALSE");
            ErrPostEx(SEV_INFO,0,0, "SeqHoundGiFrom3DList request: %s.\n", IsNullStr(fpath));
            pvalstr = MemFree(pvalstr);
            if(SHoundWWWGetfile(slri_sername, fpath) == 0)
             {
                ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
                fpath = MemFree(fpath);
                pvnshort = ValNodeFree(pvnshort);
                return NULL;
             }
            fpath = MemFree(fpath);
            pvngi = ReplyBSGetValNode(TRUE);
            ValNodeLink(&pvngifin, pvngi);
            counter = 0;
            pvnshort = ValNodeFree(pvnshort);
          }
         pvntemp = pvntemp->next;
      }
     Misc_IntVnpUnique(&pvngifin);
     return pvngifin;
}


Int4 LIBCALL SHoundGiFromPDBchain(CharPtr pdbcode, CharPtr chain)
{
     Char fpath[PATH_MAX];
     Int4 gi = 0;


     if(pdbcode == NULL)
      {
        ErrPostEx(SEV_ERROR,0,0, "Invalid parameter.");
        return 0;
      }
     if(chain == NULL)
        chain = "";
     sprintf(fpath,"%s?fnct=SeqHoundGiFromPDBchain&pdbcode=%s&chain=%s", IsNullStr(slri_cgipath), IsNullStr(pdbcode), IsNullStr(chain));
     ErrPostEx(SEV_INFO,0,0, "SeqHoundGiFromPDBchain request: %s.\n", IsNullStr(fpath));
     if(SHoundWWWGetfile(slri_sername, fpath) == 0)
      {
         ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
         return 0;
      }
     gi = ReplyBSGetInteger();
     return gi;
}






Int4 LIBCALL SHound3DFromGi(Int4 Gi)
{
     Char fpath[PATH_MAX];
     Int4 mmdbid = 0;


     if(Gi <= 0)
      {
        ErrPostEx(SEV_ERROR,0,0, "Invalid parameter.");
        return 0;
      }

     sprintf(fpath,"%s?fnct=SeqHound3DFromGi&gi=%ld", IsNullStr(slri_cgipath), (long) Gi);
     ErrPostEx(SEV_INFO,0,0, "SeqHound3DFromGi request: %s.\n", IsNullStr(fpath));
     if(SHoundWWWGetfile(slri_sername, fpath) == 0)
      {
         ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
         return 0;
      }
     mmdbid = ReplyBSGetInteger();
     return mmdbid;
}



ValNodePtr  LIBCALL SHound3DFromGiList(ValNodePtr pvnGi)
{
     CharPtr fpath = NULL;
     CharPtr pvalstr = NULL;
     ValNodePtr pvnmmdbid = NULL, pvnmmdbidfin = NULL;
     Int4 counter = 0;
     ValNodePtr pvntemp = NULL, pvnshort = NULL;

     if(pvnGi == NULL)
      {
        ErrPostEx(SEV_ERROR,0,0, "Invalid parameter.");
        return NULL;
      }


     pvntemp = pvnGi;
     while (pvntemp != NULL)
      {
         counter++;
         pvnshort = Slri_ValNodeAddInt(&pvnshort, 0, pvntemp->data.intvalue);
         if(counter == MAXQ || pvntemp->next == NULL)
          {
            pvalstr = Misc_SaveValNodeListAsString(pvnshort);
            if(pvalstr == NULL)
             {
               ErrPostEx(SEV_ERROR,0,0, "Misc_SaveValNodeListAsString returned NULL.");
               pvnshort = ValNodeFree(pvnshort);
               return NULL;
             }
            fpath = MemNew((size_t)(StringLen(pvalstr)+PATH_MAX+100)*sizeof(Char));
            if(fpath == NULL)
             {
               ErrPostEx(SEV_ERROR,0,0, "MemNew returned NULL.");
               pvalstr = MemFree(pvalstr);
               pvnshort = ValNodeFree(pvnshort);
               return NULL;
             }
            sprintf(fpath,"%s?fnct=SeqHound3DFromGiList&pgi=%s", IsNullStr(slri_cgipath), IsNullStr(pvalstr));
            ErrPostEx(SEV_INFO,0,0, "SeqHound3DFromGiList request: %s.\n", IsNullStr(fpath));
            pvalstr = MemFree(pvalstr);
            if(SHoundWWWGetfile(slri_sername, fpath) == 0)
             {
                ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
                fpath = MemFree(fpath);
                pvnshort = ValNodeFree(pvnshort);
                return NULL;
             }
            fpath = MemFree(fpath);
            pvnmmdbid = ReplyBSGetValNode(TRUE);
            ValNodeLink(&pvnmmdbidfin, pvnmmdbid);
            counter = 0;
            pvnshort = ValNodeFree(pvnshort);
          }
         pvntemp = pvntemp->next;
      }
     Misc_IntVnpUnique(&pvnmmdbidfin);
     return pvnmmdbidfin;
}



Int4 LIBCALL SHoundDNAFromProtein(Int4 Gi)
{
     Char fpath[PATH_MAX];
     Int4 gin = 0;


     if(Gi <= 0)
      {
        ErrPostEx(SEV_ERROR,0,0, "Invalid parameter.");
        return 0;
      }

     sprintf(fpath,"%s?fnct=SeqHoundDNAFromProtein&gi=%ld", IsNullStr(slri_cgipath), (long) Gi);
     ErrPostEx(SEV_INFO,0,0, "SeqHoundDNAFromProtein request: %s.\n", IsNullStr(fpath));
     if(SHoundWWWGetfile(slri_sername, fpath) == 0)
      {
         ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
         return FALSE;
      }
     gin = ReplyBSGetInteger();
     return gin;
}



ValNodePtr  LIBCALL SHoundDNAFromProteinList(ValNodePtr pvnGi)
{
     CharPtr fpath = NULL;
     CharPtr pvalstr = NULL;
     ValNodePtr pvngin = NULL, pvnginfin = NULL;
     ValNodePtr pvntemp = NULL, pvnshort = NULL;
     Int4 counter = 0;


     if(pvnGi == NULL)
      {
        ErrPostEx(SEV_ERROR,0,0, "Invalid parameter.");
        return NULL;
      }

     pvntemp = pvnGi;
     while (pvntemp != NULL)
      {
         counter++;
         pvnshort = Slri_ValNodeAddInt(&pvnshort, 0, pvntemp->data.intvalue);
         if(counter == MAXQ || pvntemp->next == NULL)
          {
            pvalstr = Misc_SaveValNodeListAsString(pvnshort);
            if(pvalstr == NULL)
             {
               ErrPostEx(SEV_ERROR,0,0, "Misc_SaveValNodeListAsString returned NULL.");
               pvnshort = ValNodeFree(pvnshort);
               return NULL;
             }
            fpath = MemNew((size_t)(StringLen(pvalstr)+PATH_MAX+100)*sizeof(Char));
            if(fpath == NULL)
             {
               ErrPostEx(SEV_ERROR,0,0, "MemNew returned NULL.");
               pvalstr = MemFree(pvalstr);
               pvnshort = ValNodeFree(pvnshort);
               return NULL;
             }
            sprintf(fpath,"%s?fnct=SeqHoundDNAFromProteinList&pgi=%s", IsNullStr(slri_cgipath), IsNullStr(pvalstr));
            ErrPostEx(SEV_INFO,0,0, "SeqHoundDNAFromProteinList request: %s.\n", IsNullStr(fpath));
            pvalstr = MemFree(pvalstr);
            if(SHoundWWWGetfile(slri_sername, fpath) == 0)
             {
                ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
                fpath = MemFree(fpath);
                pvnshort = ValNodeFree(pvnshort);
                return FALSE;
             }
            fpath = MemFree(fpath);
            pvngin = ReplyBSGetValNode(TRUE);
            ValNodeLink(&pvnginfin, pvngin);
            counter = 0;
            pvnshort = ValNodeFree(pvnshort);
          }
         pvntemp = pvntemp->next;
      }
     Misc_IntVnpUnique(&pvnginfin);
     return pvnginfin;
}


ValNodePtr  LIBCALL SHoundProteinFromDNA(Int4 Gi)
{

     Char fpath[PATH_MAX];
     ValNodePtr pvngia = NULL;


     if(Gi <= 0)
      {
        ErrPostEx(SEV_ERROR,0,0, "Invalid parameter.");
        return NULL;
      }

     sprintf(fpath,"%s?fnct=SeqHoundProteinFromDNA&gi=%ld", IsNullStr(slri_cgipath), (long) Gi);

     ErrPostEx(SEV_INFO,0,0, "SeqHoundProteinFromDNA request: %s.\n", IsNullStr(fpath));
     if(SHoundWWWGetfile(slri_sername, fpath) == 0)
      {
         ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
         return NULL;
      }
     pvngia = ReplyBSGetValNode(TRUE);
     return pvngia;
}


ValNodePtr  LIBCALL SHoundProteinFromDNAList(ValNodePtr pvnGi)
{

     CharPtr pvalstr = NULL;
     ValNodePtr pvngia = NULL, pvngiafin = NULL;
     CharPtr fpath = NULL;
     ValNodePtr pvntemp = NULL, pvnshort = NULL;
     Int4 counter = 0;


     if(pvnGi == NULL)
      {
        ErrPostEx(SEV_ERROR,0,0, "Invalid parameter.");
        return NULL;
      }
     pvntemp = pvnGi;
     while (pvntemp != NULL)
      {
         counter++;
         pvnshort = Slri_ValNodeAddInt(&pvnshort, 0, pvntemp->data.intvalue);
         if(counter == MAXQ || pvntemp->next == NULL)
          {
            pvalstr = Misc_SaveValNodeListAsString(pvnshort);
            if(pvalstr == NULL)
             {
               ErrPostEx(SEV_ERROR,0,0, "Misc_SaveValNodeListAsString returned NULL.");
               pvnshort = ValNodeFree(pvnshort);
               return NULL;
             }
            fpath = MemNew((size_t)(StringLen(pvalstr)+PATH_MAX+100)*sizeof(Char));
            if(fpath == NULL)
             {
               ErrPostEx(SEV_ERROR,0,0, "MemNew returned NULL.");
               pvalstr = MemFree(pvalstr);
               pvnshort = ValNodeFree(pvnshort);
               return NULL;
             }
            sprintf(fpath,"%s?fnct=SeqHoundProteinFromDNAList&pgi=%s", IsNullStr(slri_cgipath), IsNullStr(pvalstr));
            ErrPostEx(SEV_INFO,0,0, "SeqHoundProteinFromDNAList request: %s.\n", IsNullStr(fpath));
            pvalstr = MemFree(pvalstr);
            if(SHoundWWWGetfile(slri_sername, fpath) == 0)
             {
                ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
                fpath = MemFree(fpath);
                pvnshort = ValNodeFree(pvnshort);
                return FALSE;
             }
            fpath = MemFree(fpath);
            pvngia = ReplyBSGetValNode(TRUE);
            ValNodeLink(&pvngiafin, pvngia);
            counter = 0;
            pvnshort = ValNodeFree(pvnshort);
          }
         pvntemp = pvntemp->next;
      }
     Misc_IntVnpUnique(&pvngiafin);
     return pvngiafin;
}



/*Retrieves the pmids or muids given a Gi and the reference id queried.
 *if want PubMed ID, set 'ReferenceType'==1, if want MedLine ID, set 
 *'ReferenceType'=2.
 *This function deprecates SHoundMuidFromGi
 */
ValNodePtr  LIBCALL SHoundGetReferenceIDFromGi(Int4 Gi, Int2 referencetype)
{
  Char fpath[PATH_MAX];
  ValNodePtr vnpref = NULL;
  Int2 ercode = 0;

  if(Gi <= 0 || referencetype <=0) {
    ErrPostEx(SEV_ERROR,0,0, "SHoundGetReferenceIDFromGi:Invalid parameter.");
    return NULL;
  }
  sprintf(fpath,"%s?fnct=SeqHoundGetReferenceIDFromGi&gi=%ld&referencetype=%ld", IsNullStr(slri_cgipath), (long)Gi, (long)referencetype);
  ErrPostEx(SEV_INFO,0,0, "SeqHoundGetReferenceIDFromGi request: %s.\n", IsNullStr(fpath));
  if((ercode = SHoundWWWGetfile(slri_sername, fpath)) == 0) {
    ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
    return NULL;
  } else if (ercode == 2) {
    return NULL;
  }
  vnpref = ReplyBSGetValNode(TRUE);
  return vnpref;
}

/* Retrieves the pmids or muids given list of gis and the reference type queried.
 *if want PubMed ID, set 'ReferenceType'==1, if want MedLine ID, set 
 *'ReferenceType'=2.
 *This function deprecates SHoundMuidFromGiList
 */
ValNodePtr  LIBCALL SHoundGetReferenceIDFromGiList(ValNodePtr vnpgi, Int2 referencetype)
{
  CharPtr fpath = NULL;
  CharPtr pstr = NULL;
  ValNodePtr pvntemp = NULL, pvnshort = NULL, pvnfin = NULL, pvnid = NULL;
  Int4 counter = 0;

  if(vnpgi == NULL || referencetype <= 0)
  {
    ErrPostEx(SEV_ERROR,0,0, "SHoundGetReferenceIDFromGiList: Invalid parameter.");
    return NULL;
  }

  pvntemp = vnpgi;
  while (pvntemp != NULL)
  {
     counter++;
     pvnshort = Slri_ValNodeAddInt(&pvnshort, 0, pvntemp->data.intvalue);
     if(counter == MAXQ || pvntemp->next == NULL)
     {
        pstr = Misc_SaveValNodeListAsString(pvnshort);
        if(pstr == NULL)
        {
           ErrPostEx(SEV_ERROR,0,0, "SHoundGetReferenceIDFromGiList: Misc_SaveValNodeListAsString returned NULL.");
           pvnshort = ValNodeFree(pvnshort);
           return NULL;
        }
        fpath = MemNew((size_t)(StringLen(pstr)+PATH_MAX+100)*sizeof(Char));
        if(fpath == NULL)
        {
           ErrPostEx(SEV_ERROR,0,0, "SHoundGetReferenceIDFromGiList: MemNew returned NULL.");
           pstr = MemFree(pstr);
           pvnshort = ValNodeFree(pvnshort);
           return NULL;
        }
        sprintf(fpath,"%s?fnct=SeqHoundGetReferenceIDFromGiList&gilist=%s&reftype=%ld", slri_cgipath, pstr, (long)referencetype);
        ErrPostEx(SEV_INFO,0,0, "SHoundGetReferenceIDFromGiList request: %s.\n", fpath);
        pstr = MemFree(pstr);
        if(SHoundWWWGetfile(slri_sername, fpath) == 0)
        {
           ErrPostEx(SEV_ERROR,0,0, "SHoundGetReferenceIDFromGiList: SHoundWWWGetfile failed.");
           fpath = MemFree(fpath);
           pvnshort = ValNodeFree(pvnshort);
           return NULL;
        }
        
        fpath = MemFree(fpath);
        pvnid = ReplyBSGetValNode(TRUE);
        ValNodeLink(&pvnfin, pvnid);
        counter = 0;
        pvnshort = ValNodeFree(pvnshort);
     }
     pvntemp = pvntemp->next;
  }
  Misc_IntVnpUnique(&pvnfin);
  return pvnfin;
}


ValNodePtr  LIBCALL SHoundMuidFromGi(Int4 Gi)
{
     Char fpath[PATH_MAX];
     ValNodePtr pvnmuid = NULL;


     if(Gi <= 0)
      {
        ErrPostEx(SEV_ERROR,0,0, "Invalid parameter.");
        return NULL;
      }
     sprintf(fpath,"%s?fnct=SeqHoundMuidFromGi&gi=%ld", IsNullStr(slri_cgipath), (long) Gi);
     ErrPostEx(SEV_INFO,0,0, "SeqHoundMuidFromGi request: %s.\n", IsNullStr(fpath));
     if(SHoundWWWGetfile(slri_sername, fpath) == 0)
      {
         ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
         return NULL;
      }
     pvnmuid = ReplyBSGetValNode(TRUE);
     return pvnmuid;
}



ValNodePtr  LIBCALL SHoundMuidFromGiList(ValNodePtr pvnGi)
{
     CharPtr fpath = NULL;
     CharPtr pvalstr = NULL;
     ValNodePtr pvnmuid = NULL, pvnmuidfin = NULL;
     ValNodePtr pvntemp = NULL, pvnshort = NULL;
     Int4 counter = 0;


     if(pvnGi == NULL)
      {
        ErrPostEx(SEV_ERROR,0,0, "Invalid parameter.");
        return NULL;
      }
     pvntemp = pvnGi;
     while (pvntemp != NULL)
      {
         counter++;
         pvnshort = Slri_ValNodeAddInt(&pvnshort, 0, pvntemp->data.intvalue);
         if(counter == MAXQ || pvntemp->next == NULL)
          {
            pvalstr = Misc_SaveValNodeListAsString(pvnshort);
            if(pvalstr == NULL)
             {
               ErrPostEx(SEV_ERROR,0,0, "Misc_SaveValNodeListAsString returned NULL.");
               pvnshort = ValNodeFree(pvnshort);
               return NULL;
             }
            fpath = MemNew((size_t)(StringLen(pvalstr)+PATH_MAX+100)*sizeof(Char));
            if(fpath == NULL)
             {
               ErrPostEx(SEV_ERROR,0,0, "MemNew returned NULL.");
               pvalstr = MemFree(pvalstr);
               pvnshort = ValNodeFree(pvnshort);
               return NULL;
             }

            sprintf(fpath,"%s?fnct=SeqHoundMuidFromGiList&pgi=%s", IsNullStr(slri_cgipath), IsNullStr(pvalstr));
            ErrPostEx(SEV_INFO,0,0, "SeqHoundMuidFromGiList request: %s.\n", IsNullStr(fpath));
            pvalstr = MemFree(pvalstr);
            if(SHoundWWWGetfile(slri_sername, fpath) == 0)
             {
                ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
                fpath = MemFree(fpath);
                pvnshort = ValNodeFree(pvnshort);
                return NULL;
             }
            fpath = MemFree(fpath);
            pvnmuid = ReplyBSGetValNode(TRUE);
            ValNodeLink(&pvnmuidfin, pvnmuid);
            counter = 0;
            pvnshort = ValNodeFree(pvnshort);
          }
         pvntemp = pvntemp->next;
      }
     Misc_IntVnpUnique(&pvnmuidfin);
     return pvnmuidfin;
}


/* Return the gis associated with a publication id 'ReferenceID'.
 * If 'ReferenceID' is a pubmed publication, set 'ReferenceType' == 1,
 * if 'ReferenceID' is a medline publication, set 'ReferenceType' == 2, 
 *
 * A publication may contain many gis, eg a high throughput publication.
 *
 * The return list may contain duplicates.
 */
ValNodePtr  LIBCALL SHoundGiFromReferenceID(Int4 referenceid, Int2 referencetype)
{
  Char fpath[PATH_MAX];
  ValNodePtr vnpgi = NULL;
  Int2 ercode = 0;

  if(referenceid <= 0 || referencetype <=0) {
    ErrPostEx(SEV_ERROR,0,0, "SHoundGiFromReferenceID:Invalid parameter.");
    return NULL;
  }
  sprintf(fpath,"%s?fnct=SeqHoundGiFromReferenceID&referenceid=%ld&referencetype=%ld", slri_cgipath, (long)referenceid, (long)referencetype);
  ErrPostEx(SEV_INFO,0,0, "SeqHoundGiFromReferenceID request: %s.\n", fpath);
  if((ercode = SHoundWWWGetfile(slri_sername, fpath)) == 0) {
    ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
    return NULL;
  } else if (ercode == 2) {
    return NULL;
  }
  vnpgi = ReplyBSGetValNode(TRUE);
  return vnpgi;
}


/* Return the gis associated with a publication list 'ReferenceList'.
 * If 'ReferenceList' contains pubmed publications, set 'ReferenceType' == 1,
 * if 'ReferenceList' contains medline publications, set 'ReferenceType' == 2, 
 *
 * The return list may contain duplicates.
 */
ValNodePtr  LIBCALL SHoundGiFromReferenceList(ValNodePtr vnpref, Int2 referencetype)
{
  CharPtr fpath = NULL;
  CharPtr pstr = NULL;
  ValNodePtr pvntemp = NULL, pvnshort = NULL, pvnfin = NULL, pvngi = NULL;
  Int4 counter = 0;

  if(vnpref== NULL || referencetype <=0)
  {
    ErrPostEx(SEV_ERROR,0,0, "SHoundGiFromReferenceList: Invalid parameter.");
    return NULL;
  }

  pvntemp = vnpref;
  while (pvntemp != NULL)
  {
     counter++;
     pvnshort = Slri_ValNodeAddInt(&pvnshort, 0, pvntemp->data.intvalue);
     if(counter == MAXQ || pvntemp->next == NULL)
     {
        pstr = Misc_SaveValNodeListAsString(pvnshort);
        if(pstr == NULL)
        {
           ErrPostEx(SEV_ERROR,0,0, "SHoundGiFromReferenceList: Misc_SaveValNodeListAsString returned NULL.");
           pvnshort = ValNodeFree(pvnshort);
           return NULL;
        }
        fpath = MemNew((size_t)(StringLen(pstr)+PATH_MAX+100)*sizeof(Char));
        if(fpath == NULL)
        {
           ErrPostEx(SEV_ERROR,0,0, "SHoundGiFromReferenceList: MemNew returned NULL.");
           pstr = MemFree(pstr);
           pvnshort = ValNodeFree(pvnshort);
           return NULL;
        }
        sprintf(fpath,"%s?fnct=SeqHoundGiFromReferenceList&reflist=%s&reftype=%ld", slri_cgipath, pstr, (long)referencetype);
        ErrPostEx(SEV_INFO,0,0, "SHoundGiFromReferenceList request: %s.\n", fpath);
        pstr = MemFree(pstr);
        if(SHoundWWWGetfile(slri_sername, fpath) == 0)
        {
           ErrPostEx(SEV_ERROR,0,0, "SHoundGiFromReferenceList: SHoundWWWGetfile failed.");
           fpath = MemFree(fpath);
           pvnshort = ValNodeFree(pvnshort);
           return NULL;
        }
        
        fpath = MemFree(fpath);
        pvngi = ReplyBSGetValNode(TRUE);
        ValNodeLink(&pvnfin, pvngi);
        counter = 0;
        pvnshort = ValNodeFree(pvnshort);
     }
     pvntemp = pvntemp->next;
  }
  Misc_IntVnpUnique(&pvnfin);
  return pvnfin;
}


ValNodePtr  LIBCALL SHoundMuidFrom3D(Int4 mmdbid)
{

     Char fpath[PATH_MAX];
     ValNodePtr pvnmuid = NULL;


     if(mmdbid <= 0)
      {
        ErrPostEx(SEV_ERROR,0,0, "Invalid parameter.");
        return NULL;
      }

     sprintf(fpath,"%s?fnct=SeqHoundMuidFrom3D&mmdbid=%ld", IsNullStr(slri_cgipath), (long) mmdbid);
     ErrPostEx(SEV_INFO,0,0, "SeqHoundMuidFrom3D request: %s.\n", IsNullStr(fpath));
     if(SHoundWWWGetfile(slri_sername, fpath) == 0)
      {
         ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
         return NULL;
      }
     pvnmuid = ReplyBSGetValNode(TRUE);
     return pvnmuid;
}


ValNodePtr  LIBCALL SHoundMuidFrom3DList(ValNodePtr pvnMmdbid)
{
     CharPtr fpath = NULL;
     CharPtr pvalstr = NULL;
     ValNodePtr pvnmuid = NULL, pvnmuidfin = NULL;
     ValNodePtr pvntemp = NULL, pvnshort = NULL;
     Int4 counter = 0;

     if(pvnMmdbid == NULL)
      {
        ErrPostEx(SEV_ERROR,0,0, "Invalid parameter.");
        return NULL;
      }
     pvntemp = pvnMmdbid;
     while (pvntemp != NULL)
      {
         counter++;
         pvnshort = Slri_ValNodeAddInt(&pvnshort, 0, pvntemp->data.intvalue);
         if(counter == MAXQ || pvntemp->next == NULL)
          {
            pvalstr = Misc_SaveValNodeListAsString(pvnshort);
            if(pvalstr == NULL)
             {
               ErrPostEx(SEV_ERROR,0,0, "Misc_SaveValNodeListAsString returned NULL.");
               pvnshort = ValNodeFree(pvnshort);
               return NULL;
             }
            fpath = MemNew((size_t)(StringLen(pvalstr)+PATH_MAX+100)*sizeof(Char));
            if(fpath == NULL)
             {
               ErrPostEx(SEV_ERROR,0,0, "MemNew returned NULL.");
               pvalstr = MemFree(pvalstr);
               pvnshort = ValNodeFree(pvnshort);
               return NULL;
             }
            sprintf(fpath,"%s?fnct=SeqHoundMuidFrom3DList&pmmdbid=%s", IsNullStr(slri_cgipath), IsNullStr(pvalstr));
            ErrPostEx(SEV_INFO,0,0, "SeqHoundMuidFrom3DList request: %s.\n", IsNullStr(fpath));
            pvalstr = MemFree(pvalstr);
            if(SHoundWWWGetfile(slri_sername, fpath) == 0)
             {
                ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
                fpath = MemFree(fpath);
                pvnshort = ValNodeFree(pvnshort);
                return NULL;
             }
            fpath = MemFree(fpath);
            pvnmuid = ReplyBSGetValNode(TRUE);
            ValNodeLink(&pvnmuidfin, pvnmuid);
            counter = 0;
            pvnshort = ValNodeFree(pvnshort);
          }
         pvntemp = pvntemp->next;
      }
     Misc_IntVnpUnique(&pvnmuidfin);
     return pvnmuidfin;
}


Int4 LIBCALL SHoundTaxIDFromGi(Int4 Gi)
{
     Char fpath[PATH_MAX];
     Int4 taxid = 0;


     if(Gi <= 0)
      {
        ErrPostEx(SEV_ERROR,0,0, "Invalid parameter.");
        return 0;
      }

     sprintf(fpath,"%s?fnct=SeqHoundTaxIDFromGi&gi=%ld", IsNullStr(slri_cgipath), (long) Gi);
     ErrPostEx(SEV_INFO,0,0, "SeqHoundTaxIDFromGi request: %s.\n", IsNullStr(fpath));
     if(SHoundWWWGetfile(slri_sername, fpath) == 0)
      {
         ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
         return FALSE;
      }
     taxid = ReplyBSGetInteger();
     return taxid;
}


ValNodePtr  LIBCALL SHoundTaxIDFromGiList(ValNodePtr pvnGi)
{
     CharPtr fpath = NULL;
     CharPtr pvalstr = NULL;
     ValNodePtr pvntaxid = NULL, pvntaxidfin = NULL;
     ValNodePtr pvntemp = NULL, pvnshort = NULL;
     Int4 counter = 0;

     if(pvnGi == NULL)
      {
        ErrPostEx(SEV_ERROR,0,0, "Invalid parameter.");
        return NULL;
      }
     pvntemp = pvnGi;
     while (pvntemp != NULL)
      {
         counter++;
         pvnshort = Slri_ValNodeAddInt(&pvnshort, 0, pvntemp->data.intvalue);
         if(counter == MAXQ || pvntemp->next == NULL)
          {
            pvalstr = Misc_SaveValNodeListAsString(pvnshort);
            if(pvalstr == NULL)
             {
               ErrPostEx(SEV_ERROR,0,0, "Misc_SaveValNodeListAsString returned NULL.");
               pvnshort = ValNodeFree(pvnshort);
               return NULL;
             }
            fpath = MemNew((size_t)(StringLen(pvalstr)+PATH_MAX+100)*sizeof(Char));
            if(fpath == NULL)
             {
               ErrPostEx(SEV_ERROR,0,0, "MemNew returned NULL.");
               pvalstr = MemFree(pvalstr);
               pvnshort = ValNodeFree(pvnshort);
               return NULL;
             }
            sprintf(fpath,"%s?fnct=SeqHoundTaxIDFromGiList&pgi=%s", IsNullStr(slri_cgipath), IsNullStr(pvalstr));
            ErrPostEx(SEV_INFO,0,0, "SeqHoundTaxIDFromGiList request: %s.\n", IsNullStr(fpath));
            pvalstr = MemFree(pvalstr);
            if(SHoundWWWGetfile(slri_sername, fpath) == 0)
             {
                ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
                fpath = MemFree(fpath);
                pvnshort = ValNodeFree(pvnshort);
                return FALSE;
             }
            fpath = MemFree(fpath);
            pvntaxid = ReplyBSGetValNode(TRUE);
            ValNodeLink(&pvntaxidfin, pvntaxid);
            counter = 0;
            pvnshort = ValNodeFree(pvnshort);
          }
         pvntemp = pvntemp->next;
      }
     Misc_IntVnpUnique(&pvntaxidfin);
     return pvntaxidfin;
}



ValNodePtr  LIBCALL SHoundTaxIDFrom3D(Int4 mmdbid)
{
     Char fpath[PATH_MAX];
     ValNodePtr pvntaxid = NULL;


     if(mmdbid <= 0)
      {
        ErrPostEx(SEV_ERROR,0,0, "Invalid parameter.");
        return NULL;
      }
     sprintf(fpath,"%s?fnct=SeqHoundTaxIDFrom3D&mmdbid=%ld", IsNullStr(slri_cgipath), (long) mmdbid);
     ErrPostEx(SEV_INFO,0,0, "SeqHoundTaxIDFrom3D request: %s.\n", IsNullStr(fpath));
     if(SHoundWWWGetfile(slri_sername, fpath) == 0)
      {
         ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
         return NULL;
      }
     pvntaxid = ReplyBSGetValNode(TRUE);
     return pvntaxid;
}



ValNodePtr  LIBCALL SHoundTaxIDFrom3DList(ValNodePtr pvnMmdbid)
{
     CharPtr fpath = NULL;	
     CharPtr pvalstr = NULL;
     ValNodePtr pvntaxid = NULL, pvntaxidfin = NULL;
     ValNodePtr pvntemp = NULL, pvnshort = NULL;
     Int4 counter = 0;

     if(pvnMmdbid == NULL)
      {
        ErrPostEx(SEV_ERROR,0,0, "Invalid parameter.");
        return NULL;
      }
     pvntemp = pvnMmdbid;
     while (pvntemp != NULL)
      {
         counter++;
         pvnshort = Slri_ValNodeAddInt(&pvnshort, 0, pvntemp->data.intvalue);
         if(counter == MAXQ || pvntemp->next == NULL)
          {
            pvalstr = Misc_SaveValNodeListAsString(pvnshort);
            if(pvalstr == NULL)
             {
               ErrPostEx(SEV_ERROR,0,0, "Misc_SaveValNodeListAsString returned NULL.");
               pvnshort = ValNodeFree(pvnshort);
               return NULL;
             }
            fpath = MemNew((size_t)(StringLen(pvalstr)+PATH_MAX+100)*sizeof(Char));
            if(fpath == NULL)
             {
               ErrPostEx(SEV_ERROR,0,0, "MemNew returned NULL.");
               pvalstr = MemFree(pvalstr);
               pvnshort = ValNodeFree(pvnshort);
               return NULL;
             }
            sprintf(fpath,"%s?fnct=SeqHoundTaxIDFrom3DList&pmmdbid=%s", IsNullStr(slri_cgipath), IsNullStr(pvalstr));
            ErrPostEx(SEV_INFO,0,0, "SeqHoundTaxIDFrom3DList request: %s.\n", IsNullStr(fpath));
            pvalstr = MemFree(pvalstr);
            if(SHoundWWWGetfile(slri_sername, fpath) == 0)
             {
                ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
                fpath = MemFree(fpath);
                pvnshort = ValNodeFree(pvnshort);
                return NULL;
             }
            fpath = MemFree(fpath);
            pvntaxid = ReplyBSGetValNode(TRUE);
            ValNodeLink(&pvntaxidfin, pvntaxid);
            counter = 0;
            pvnshort = ValNodeFree(pvnshort);
          }
         pvntemp = pvntemp->next;
      }
     Misc_IntVnpUnique(&pvntaxidfin);
     return pvntaxidfin;
}


ValNodePtr  LIBCALL SHoundProteinsFromTaxID(Int4 taxid)
{
     Char fpath[PATH_MAX];
     ValNodePtr pvngi = NULL;


     if(taxid <= 0)
      {
        ErrPostEx(SEV_ERROR,0,0, "Invalid parameter.");
        return NULL;
      }
     sprintf(fpath,"%s?fnct=SeqHoundProteinsFromTaxID&taxid=%ld", IsNullStr(slri_cgipath), (long) taxid);
     ErrPostEx(SEV_INFO,0,0, "SeqHoundProteinsFromTaxID request: %s.\n", IsNullStr(fpath));
     if(SHoundWWWGetfile(slri_sername, fpath) == 0)
      {
         ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
         return NULL;
      }
     pvngi = ReplyBSGetValNode(TRUE);
     return pvngi;
}


ValNodePtr  LIBCALL SHoundProteinsFromTaxIDIII(Int4 taxid)
{
     Char fpath[PATH_MAX];
     ValNodePtr pvngi = NULL;


     if(taxid <= 0)
      {
        ErrPostEx(SEV_ERROR,0,0, "Invalid parameter.");
        return NULL;
      }
     sprintf(fpath,"%s?fnct=SeqHoundProteinsFromTaxIDIII&taxid=%ld", IsNullStr(slri_cgipath), (long) taxid);
     ErrPostEx(SEV_INFO,0,0, "SeqHoundProteinsFromTaxIDIII request: %s.\n", IsNullStr(fpath));
     if(SHoundWWWGetfile(slri_sername, fpath) == 0)
      {
         ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
         return NULL;
      }
     pvngi = ReplyBSGetValNode(TRUE);
     return pvngi;
}

ValNodePtr  LIBCALL SHoundProteinsFromTaxIDList(ValNodePtr pvntaxid)
{
     CharPtr fpath = NULL;
     CharPtr pvalstr = NULL;
     ValNodePtr pvngi = NULL, pvngifin = NULL;
     ValNodePtr pvntemp = NULL, pvnshort = NULL;
     Int4 counter = 0;

     if(pvntaxid == NULL)
      {
        ErrPostEx(SEV_ERROR,0,0, "Invalid parameter.");
        return NULL;
      }

     pvntemp = pvntaxid;
     while (pvntemp != NULL)
      {
         counter++;
         pvnshort = Slri_ValNodeAddInt(&pvnshort, 0, pvntemp->data.intvalue);
         if(counter == MAXQ || pvntemp->next == NULL)
          {
            pvalstr = Misc_SaveValNodeListAsString(pvnshort);
            if(pvalstr == NULL)
             {
               ErrPostEx(SEV_ERROR,0,0, "Misc_SaveValNodeListAsString returned NULL.");
               pvnshort = ValNodeFree(pvnshort);
               return NULL;
             }
            fpath = MemNew((size_t)(StringLen(pvalstr)+PATH_MAX+100)*sizeof(Char));
            if(fpath == NULL)
             {
               ErrPostEx(SEV_ERROR,0,0, "MemNew returned NULL.");
               pvalstr = MemFree(pvalstr);
               pvnshort = ValNodeFree(pvnshort);
               return NULL;
             }
            sprintf(fpath,"%s?fnct=SeqHoundProteinsFromTaxIDList&ptaxid=%s", IsNullStr(slri_cgipath), IsNullStr(pvalstr));
            ErrPostEx(SEV_INFO,0,0, "SeqHoundProteinsFromTaxIDList request: %s.\n", IsNullStr(fpath));
            pvalstr = MemFree(pvalstr);
            if(SHoundWWWGetfile(slri_sername, fpath) == 0)
             {
                ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
                fpath = MemFree(fpath);
                pvnshort = ValNodeFree(pvnshort);
                return NULL;
             }
            fpath = MemFree(fpath);
            pvngi = ReplyBSGetValNode(TRUE);
            ValNodeLink(&pvngifin, pvngi);
            counter = 0;
            pvnshort = ValNodeFree(pvnshort);
          }
         pvntemp = pvntemp->next;
      }
     Misc_IntVnpUnique(&pvngifin);
     return pvngifin;
}

ValNodePtr  LIBCALL SHoundDNAFromTaxID(Int4 taxid)
{
     Char fpath[PATH_MAX];
     ValNodePtr pvngi = NULL;


     if(taxid <= 0)
      {
        ErrPostEx(SEV_ERROR,0,0, "Invalid parameter.");
        return NULL;
      }
     sprintf(fpath,"%s?fnct=SeqHoundDNAFromTaxID&taxid=%ld", IsNullStr(slri_cgipath), (long) taxid);
     ErrPostEx(SEV_INFO,0,0, "SeqHoundDNAFromTaxID request: %s.\n", IsNullStr(fpath));
     if(SHoundWWWGetfile(slri_sername, fpath) == 0)
      {
         ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
         return NULL;
      }
     pvngi = ReplyBSGetValNode(TRUE);
     return pvngi;
}


ValNodePtr  LIBCALL SHoundDNAFromTaxIDIII(Int4 taxid)
{
     Char fpath[PATH_MAX];
     ValNodePtr pvngi = NULL;


     if(taxid <= 0)
      {
        ErrPostEx(SEV_ERROR,0,0, "Invalid parameter.");
        return NULL;
      }
     sprintf(fpath,"%s?fnct=SeqHoundDNAFromTaxIDIII&taxid=%ld", IsNullStr(slri_cgipath), (long) taxid);
     ErrPostEx(SEV_INFO,0,0, "SeqHoundDNAFromTaxIDIII request: %s.\n", IsNullStr(fpath));
     if(SHoundWWWGetfile(slri_sername, fpath) == 0)
      {
         ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
         return NULL;
      }
     pvngi = ReplyBSGetValNode(TRUE);
     return pvngi;
}


ValNodePtr  LIBCALL SHoundDNAFromTaxIDList(ValNodePtr pvntaxid)
{
     CharPtr fpath = NULL;
     CharPtr pvalstr = NULL;
     ValNodePtr pvngi = NULL, pvngifin = NULL;
     ValNodePtr pvntemp = NULL, pvnshort = NULL;
     Int4 counter = 0;

     if(pvntaxid == NULL)
      {
        ErrPostEx(SEV_ERROR,0,0, "Invalid parameter.");
        return NULL;
      }
     pvntemp = pvntaxid;
     while (pvntemp != NULL)
      {
         counter++;
         pvnshort = Slri_ValNodeAddInt(&pvnshort, 0, pvntemp->data.intvalue);
         if(counter == MAXQ || pvntemp->next == NULL)
          {
            pvalstr = Misc_SaveValNodeListAsString(pvnshort);
            if(pvalstr == NULL)
             {
               ErrPostEx(SEV_ERROR,0,0, "Misc_SaveValNodeListAsString returned NULL.");
               pvnshort = ValNodeFree(pvnshort);
               return NULL;
             }
            fpath = MemNew((size_t)(StringLen(pvalstr)+PATH_MAX+100)*sizeof(Char));
            if(fpath == NULL)
             {
               ErrPostEx(SEV_ERROR,0,0, "MemNew returned NULL.");
               pvalstr = MemFree(pvalstr);
               pvnshort = ValNodeFree(pvnshort);
               return NULL;
             }
            sprintf(fpath,"%s?fnct=SeqHoundDNAFromTaxIDList&ptaxid=%s", IsNullStr(slri_cgipath), IsNullStr(pvalstr));
            ErrPostEx(SEV_INFO,0,0, "SeqHoundProteinsFromTaxIDList request: %s.\n", IsNullStr(fpath));
            pvalstr = MemFree(pvalstr);
            if(SHoundWWWGetfile(slri_sername, fpath) == 0)
             {
                ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
                fpath = MemFree(fpath);
                pvnshort = ValNodeFree(pvnshort);
                return NULL;
             }
            fpath = MemFree(fpath);
            pvngi = ReplyBSGetValNode(TRUE);
            ValNodeLink(&pvngifin, pvngi);
            counter = 0;
            pvnshort = ValNodeFree(pvnshort);
          }
         pvntemp = pvntemp->next;
      }
     Misc_IntVnpUnique(&pvngifin);
     return pvngifin;
}


/************************************************/
/* Genome iterators                             */
/************************************************/
ValNodePtr   LIBCALL SHoundProteinsFromOrganism(Int4 taxid, Byte chromflags)
{
     Char fpath[PATH_MAX];
     ValNodePtr pvngia = NULL;


     if(taxid <= 0)
      {
        ErrPostEx(SEV_ERROR,0,0, "Invalid parameter.");
        return NULL;
      }
     if(chromflags <= 0)
      {
        ErrPostEx(SEV_ERROR,0,0, "Invalid parameter.");
        return NULL;
      }

     sprintf(fpath,"%s?fnct=SeqHoundProteinsFromOrganism&taxid=%ld&chromflags=%d", IsNullStr(slri_cgipath), (long) taxid, chromflags);
     ErrPostEx(SEV_INFO,0,0, "SeqHoundProteinsFromOrganism request: %s.\n", IsNullStr(fpath));
     if(SHoundWWWGetfile(slri_sername, fpath) == 0)
      {
         ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
         return NULL;
      }
     pvngia = ReplyBSGetValNode(TRUE);
     return pvngia;
}



ValNodePtr   LIBCALL SHoundProteinsFromOrganismList(ValNodePtr pvnTax, Byte chromflags)
{
     CharPtr fpath = NULL;
     CharPtr pvalstr = NULL;
     ValNodePtr pvngia = NULL, pvngiafin = NULL;
     ValNodePtr pvntemp = NULL, pvnshort = NULL;
     Int4 counter = 0;

     if(pvnTax == NULL)
      {
        ErrPostEx(SEV_ERROR,0,0, "Invalid parameter.");
        return NULL;
      }
     if(chromflags <= 0)
      {
        ErrPostEx(SEV_ERROR,0,0, "Invalid parameter.");
        return NULL;
      }

     pvntemp = pvnTax;
     while (pvntemp != NULL)
      {
         counter++;
         pvnshort = Slri_ValNodeAddInt(&pvnshort, 0, pvntemp->data.intvalue);
         if(counter == MAXQ || pvntemp->next == NULL)
          {
            pvalstr = Misc_SaveValNodeListAsString(pvnshort);
            if(pvalstr == NULL)
             {
               ErrPostEx(SEV_ERROR,0,0, "Misc_SaveValNodeListAsString returned NULL.");
               pvnshort = ValNodeFree(pvnshort);
               return NULL;
             }
            fpath = MemNew((size_t)(StringLen(pvalstr)+PATH_MAX+100)*sizeof(Char));
            if(fpath == NULL)
             {
               ErrPostEx(SEV_ERROR,0,0, "MemNew returned NULL.");
               pvalstr = MemFree(pvalstr);
               pvnshort = ValNodeFree(pvnshort);
               return NULL;
             }
            sprintf(fpath,"%s?fnct=SeqHoundProteinsFromOrganismList&ptaxid=%s&chromflags=%d", IsNullStr(slri_cgipath), IsNullStr(pvalstr), chromflags);

            ErrPostEx(SEV_INFO,0,0, "SeqHoundProteinsFromOrganismList request: %s.\n", IsNullStr(fpath));
            pvalstr = MemFree(pvalstr);
            if(SHoundWWWGetfile(slri_sername, fpath) == 0)
             {
                ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
                fpath = MemFree(fpath);
                pvnshort = ValNodeFree(pvnshort);
                return NULL;
             }
            fpath = MemFree(fpath);
            pvngia = ReplyBSGetValNode(TRUE);
            ValNodeLink(&pvngiafin, pvngia);
            counter = 0;
            pvnshort = ValNodeFree(pvnshort);
          }
         pvntemp = pvntemp->next;
      }
     Misc_IntVnpUnique(&pvngiafin);
     return pvngiafin;
}


ValNodePtr   LIBCALL SHoundProteinsFromChromosome(Int4 chromid)
{

     Char fpath[PATH_MAX];
     ValNodePtr pvngia = NULL;

     if(chromid <= 0)
      {
        ErrPostEx(SEV_ERROR,0,0, "Invalid parameter.");
        return NULL;
      }

     sprintf(fpath,"%s?fnct=SeqHoundProteinsFromChromosome&chromid=%ld", IsNullStr(slri_cgipath), (long) chromid);
     ErrPostEx(SEV_INFO,0,0, "SeqHoundProteinsFromChromosome request: %s.\n", IsNullStr(fpath));
     if(SHoundWWWGetfile(slri_sername, fpath) == 0)
      {
         ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
         return NULL;
      }
     pvngia = ReplyBSGetValNode(TRUE);
     return pvngia;
}


ValNodePtr   LIBCALL SHoundProteinsFromChromosomeList(ValNodePtr pvnChrom)
{
     CharPtr fpath = NULL;
     CharPtr pvalstr = NULL;
     ValNodePtr pvngia = NULL, pvngiafin = NULL;
     ValNodePtr pvntemp = NULL, pvnshort = NULL;
     Int4 counter = 0;

     if(pvnChrom == NULL)
      {
        ErrPostEx(SEV_ERROR,0,0, "Invalid parameter.");
        return NULL;
      }


     pvntemp = pvnChrom;
     while (pvntemp != NULL)
      {
         counter++;
         pvnshort = Slri_ValNodeAddInt(&pvnshort, 0, pvntemp->data.intvalue);
         if(counter == MAXQ || pvntemp->next == NULL)
          {
            pvalstr = Misc_SaveValNodeListAsString(pvnshort);
            if(pvalstr == NULL)
             {
               ErrPostEx(SEV_ERROR,0,0, "Misc_SaveValNodeListAsString returned NULL.");
               pvnshort = ValNodeFree(pvnshort);
               return NULL;
             }
            fpath = MemNew((size_t)(StringLen(pvalstr)+PATH_MAX+100)*sizeof(Char));
            if(fpath == NULL)
             {
               ErrPostEx(SEV_ERROR,0,0, "MemNew returned NULL.");
               pvalstr = MemFree(pvalstr);
               pvnshort = ValNodeFree(pvnshort);
               return NULL;
             }
            sprintf(fpath,"%s?fnct=SeqHoundProteinsFromChromosomeList&pchromid=%s", IsNullStr(slri_cgipath), IsNullStr(pvalstr));
            ErrPostEx(SEV_INFO,0,0, "SeqHoundProteinsFromChromosomeList request: %s.\n", IsNullStr(fpath));
            pvalstr = MemFree(pvalstr);
            if(SHoundWWWGetfile(slri_sername, fpath) == 0)
             {
                ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
                fpath = MemFree(fpath);
                pvnshort = ValNodeFree(pvnshort);
                return NULL;
             }
            fpath = MemFree(fpath);
            pvngia = ReplyBSGetValNode(TRUE);
            ValNodeLink(&pvngiafin, pvngia);
            counter = 0;
            pvnshort = ValNodeFree(pvnshort);
          }
         pvntemp = pvntemp->next;
      }
     Misc_IntVnpUnique(&pvngiafin);
     return pvngiafin;
}


ValNodePtr  LIBCALL SHoundAllGenomes(void)
{
     Char fpath[PATH_MAX];
     ValNodePtr pvngen = NULL;

     sprintf(fpath,"%s?fnct=SeqHoundAllGenomes", IsNullStr(slri_cgipath));
     ErrPostEx(SEV_INFO,0,0, "SeqHoundAllGenomes request: %s.\n", IsNullStr(fpath));
     if(SHoundWWWGetfile(slri_sername, fpath) == 0)
      {
         ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
         return NULL;
      }
     pvngen = ReplyBSGetValNode(TRUE);
     return pvngen;
}


ValNodePtr  LIBCALL SHoundChromosomeFromGenome(Int4 taxid, Byte chromflags)
{

     Char fpath[PATH_MAX];
     ValNodePtr pvnchrom = NULL;


     if(taxid <= 0)
      {
        ErrPostEx(SEV_ERROR,0,0, "Invalid parameter.");
        return NULL;
      }
     if(chromflags <= 0)
      {
        ErrPostEx(SEV_ERROR,0,0, "Invalid parameter.");
        return NULL;
      }

     sprintf(fpath,"%s?fnct=SeqHoundChromosomeFromGenome&taxid=%ld&chromflags=%d", IsNullStr(slri_cgipath), (long) taxid, chromflags);
     ErrPostEx(SEV_INFO,0,0, "SeqHoundChromosomeFromGenome request: %s.\n", IsNullStr(fpath));
     if(SHoundWWWGetfile(slri_sername, fpath) == 0)
      {
         ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
         return NULL;
      }
     pvnchrom = ReplyBSGetValNode(TRUE);
     return pvnchrom;
}


ValNodePtr   LIBCALL SHoundChromosomeFromGenomeList(ValNodePtr pvnGenom, Byte chromflags)
{
     CharPtr fpath = NULL;
     CharPtr pvalstr = NULL;
     ValNodePtr pvnchrom = NULL, pvnchromfin = NULL;
     ValNodePtr pvntemp = NULL, pvnshort = NULL;
     Int4 counter = 0;

     if(pvnGenom == NULL)
      {
        ErrPostEx(SEV_ERROR,0,0, "Invalid parameter.");
        return NULL;
      }
     if(chromflags <= 0)
      {
        ErrPostEx(SEV_ERROR,0,0, "Invalid parameter.");
        return NULL;
      }
     pvntemp = pvnGenom;
     while (pvntemp != NULL)
      {
         counter++;
         pvnshort = Slri_ValNodeAddInt(&pvnshort, 0, pvntemp->data.intvalue);
         if(counter == MAXQ || pvntemp->next == NULL)
          {
            pvalstr = Misc_SaveValNodeListAsString(pvnshort);
            if(pvalstr == NULL)
             {
               ErrPostEx(SEV_ERROR,0,0, "Misc_SaveValNodeListAsString returned NULL.");
               pvnshort = ValNodeFree(pvnshort);
               return NULL;
             }

            fpath = MemNew((size_t)(StringLen(pvalstr)+PATH_MAX+100)*sizeof(Char));
            if(fpath == NULL)
             {
               ErrPostEx(SEV_ERROR,0,0, "MemNew returned NULL.");
               pvalstr = MemFree(pvalstr);
               pvnshort = ValNodeFree(pvnshort);
               return NULL;
             }
            sprintf(fpath,"%s?fnct=SeqHoundChromosomeFromGenomeList&ptaxid=%s&chromflags=%d", IsNullStr(slri_cgipath), IsNullStr(pvalstr), chromflags);
            ErrPostEx(SEV_INFO,0,0, "SeqHoundChromosomeFromGenomeList request: %s.\n", IsNullStr(fpath));
            pvalstr = MemFree(pvalstr);
            if(SHoundWWWGetfile(slri_sername, fpath) == 0)
             {
                ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
                fpath = MemFree(fpath);
                pvnshort = ValNodeFree(pvnshort);
                return NULL;
             }
            fpath = MemFree(fpath);
            pvnchrom = ReplyBSGetValNode(TRUE);
            ValNodeLink(&pvnchromfin, pvnchrom);
            counter = 0;
            pvnshort = ValNodeFree(pvnshort);
          }
         pvntemp = pvntemp->next;
      }
     Misc_IntVnpUnique(&pvnchromfin);
     return pvnchromfin;
}


ValNodePtr   LIBCALL SHoundDNAFromOrganism(Int4 taxid, Byte chromflags)
{

     Char fpath[PATH_MAX];
     ValNodePtr pvngin = NULL;

     if(taxid <= 0)
      {
        ErrPostEx(SEV_ERROR,0,0, "Invalid parameter.");
        return NULL;
      }
     if(chromflags <= 0)
      {
        ErrPostEx(SEV_ERROR,0,0, "Invalid parameter.");
        return NULL;
      }
     sprintf(fpath,"%s?fnct=SeqHoundDNAFromOrganism&taxid=%ld&chromflags=%d", IsNullStr(slri_cgipath), (long) taxid, chromflags);
     ErrPostEx(SEV_INFO,0,0, "SeqHoundDNAFromOrganism request: %s.\n", IsNullStr(fpath));
     if(SHoundWWWGetfile(slri_sername, fpath) == 0)
      {
         ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
         return NULL;
      }
     pvngin = ReplyBSGetValNode(TRUE);
     return pvngin;
}


ValNodePtr   LIBCALL SHoundDNAFromOrganismList(ValNodePtr pvnTax, Byte chromflags)
{
     CharPtr fpath = NULL;
     CharPtr pvalstr = NULL;
     ValNodePtr pvngin = NULL, pvnginfin = NULL;
     ValNodePtr pvntemp = NULL, pvnshort = NULL;
     Int4 counter = 0;

     if(pvnTax == NULL)
      {
        ErrPostEx(SEV_ERROR,0,0, "Invalid parameter.");
        return NULL;
      }
     if(chromflags <= 0)
      {
        ErrPostEx(SEV_ERROR,0,0, "Invalid parameter.");
        return NULL;
      }

     pvntemp = pvnTax;
     while (pvntemp != NULL)
      {
         counter++;
         pvnshort = Slri_ValNodeAddInt(&pvnshort, 0, pvntemp->data.intvalue);
         if(counter == MAXQ || pvntemp->next == NULL)
          {
            pvalstr = Misc_SaveValNodeListAsString(pvnshort);
            if(pvalstr == NULL)
             {
               ErrPostEx(SEV_ERROR,0,0, "Misc_SaveValNodeListAsString returned NULL.");
               pvnshort = ValNodeFree(pvnshort);
               return NULL;
             }
            fpath = MemNew((size_t)(StringLen(pvalstr)+PATH_MAX+100)*sizeof(Char));
            if(fpath == NULL)
             {
               ErrPostEx(SEV_ERROR,0,0, "MemNew returned NULL.");
               pvalstr = MemFree(pvalstr);
               pvnshort = ValNodeFree(pvnshort);
               return NULL;
             }
            sprintf(fpath,"%s?fnct=SeqHoundDNAFromOrganismList&ptaxid=%s&chromflags=%d", IsNullStr(slri_cgipath), IsNullStr(pvalstr), chromflags);
            ErrPostEx(SEV_INFO,0,0, "SeqHoundDNAFromOrganismList request: %s.\n", IsNullStr(fpath));
            pvalstr = MemFree(pvalstr);
            if(SHoundWWWGetfile(slri_sername, fpath) == 0)
             {
                ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
                fpath = MemFree(fpath);
                pvnshort = ValNodeFree(pvnshort);
                return NULL;
             }
            fpath = MemFree(fpath);
            pvngin = ReplyBSGetValNode(TRUE);
            ValNodeLink(&pvnginfin, pvngin);
            counter = 0;
            pvnshort = ValNodeFree(pvnshort);
          }
         pvntemp = pvntemp->next;
      }
     Misc_IntVnpUnique(&pvnginfin);
     return pvnginfin;
}



ValNodePtr   LIBCALL SHoundDNAFromChromosome(Int4 chromid)
{
     Char fpath[PATH_MAX];
     ValNodePtr pvngin = NULL;

     if(chromid <= 0)
      {
        ErrPostEx(SEV_ERROR,0,0, "Invalid parameter.");
        return NULL;
      }

     sprintf(fpath,"%s?fnct=SeqHoundDNAFromChromosome&chromid=%ld", IsNullStr(slri_cgipath), (long) chromid);
     ErrPostEx(SEV_INFO,0,0, "SeqHoundDNAFromChromosome request: %s.\n", IsNullStr(fpath));
     if(SHoundWWWGetfile(slri_sername, fpath) == 0)
      {
         ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
         return NULL;
      }
     pvngin = ReplyBSGetValNode(TRUE);
     return pvngin;
}


ValNodePtr   LIBCALL SHoundDNAFromChromosomeList(ValNodePtr pvnChrom)
{
     CharPtr fpath = NULL;
     CharPtr pvalstr = NULL;
     ValNodePtr pvngin = NULL, pvnginfin = NULL;
     ValNodePtr pvntemp = NULL, pvnshort = NULL;
     Int4 counter = 0;

     if(pvnChrom == NULL)
      {
        ErrPostEx(SEV_ERROR,0,0, "Invalid parameter.");
        return NULL;
      }

     pvntemp = pvnChrom;
     while (pvntemp != NULL)
      {
         counter++;
         pvnshort = Slri_ValNodeAddInt(&pvnshort, 0, pvntemp->data.intvalue);
         if(counter == MAXQ || pvntemp->next == NULL)
          {
            pvalstr = Misc_SaveValNodeListAsString(pvnshort);
            if(pvalstr == NULL)
             {
               ErrPostEx(SEV_ERROR,0,0, "Misc_SaveValNodeListAsString returned NULL.");
               pvnshort = ValNodeFree(pvnshort);
               return NULL;
             }
            fpath = MemNew((size_t)(StringLen(pvalstr)+PATH_MAX+100)*sizeof(Char));
            if(fpath == NULL)
             {
               ErrPostEx(SEV_ERROR,0,0, "MemNew returned NULL.");
               pvalstr = MemFree(pvalstr);
               pvnshort = ValNodeFree(pvnshort);
               return NULL;
             }
            sprintf(fpath,"%s?fnct=SeqHoundDNAFromChromosomeList&pchromid=%s", IsNullStr(slri_cgipath), IsNullStr(pvalstr));
            ErrPostEx(SEV_INFO,0,0, "SeqHoundDNAFromChromosomeList request: %s.\n", IsNullStr(fpath));
            pvalstr = MemFree(pvalstr);
            if(SHoundWWWGetfile(slri_sername, fpath) == 0)
             {
                ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
                fpath = MemFree(fpath);
                pvnshort = ValNodeFree(pvnshort);
                return NULL;
             }
            fpath = MemFree(fpath);
            pvngin = ReplyBSGetValNode(TRUE);
            ValNodeLink(&pvnginfin, pvngin);
            counter = 0;
            pvnshort = ValNodeFree(pvnshort);
          }
         pvntemp = pvntemp->next;
      }
     Misc_IntVnpUnique(&pvnginfin);
     return pvnginfin;
}



/************************************************/
/* Redundancy                                   */
/************************************************/
ValNodePtr LIBCALL SHoundRedundantGroup(Int4 Gi)
{
     Char fpath[PATH_MAX];
     ValNodePtr pvngi = NULL;


     if(Gi <= 0)
      {
        ErrPostEx(SEV_ERROR,0,0, "Invalid parameter.");
        return NULL;
      }
     sprintf(fpath,"%s?fnct=SeqHoundRedundantGroup&gi=%ld", IsNullStr(slri_cgipath), (long) Gi);
     ErrPostEx(SEV_INFO,0,0, "SeqHoundRedundantGroup request: %s.\n", IsNullStr(fpath));
     if(SHoundWWWGetfile(slri_sername, fpath) == 0)
      {
         ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
         return NULL;
      }
     pvngi = ReplyBSGetValNode(TRUE);
     return pvngi;
}


ValNodePtr LIBCALL SHoundRedundantGroupList(ValNodePtr pvnGi)
{
     CharPtr fpath = NULL;
     CharPtr pvalstr = NULL;
     ValNodePtr pvngi = NULL, pvngifin = NULL;
     ValNodePtr pvntemp = NULL, pvnshort = NULL;
     Int4 counter = 0;

     if(pvnGi == NULL)
      {
        ErrPostEx(SEV_ERROR,0,0, "Invalid parameter.");
        return NULL;
      }

     pvntemp = pvnGi;
     while (pvntemp != NULL)
      {
         counter++;
         pvnshort = Slri_ValNodeAddInt(&pvnshort, 0, pvntemp->data.intvalue);
         if(counter == MAXQ || pvntemp->next == NULL)
          {
            pvalstr = Misc_SaveValNodeListAsString(pvnshort);
            if(pvalstr == NULL)
             {
               ErrPostEx(SEV_ERROR,0,0, "Misc_SaveValNodeListAsString returned NULL.");
               pvnshort = ValNodeFree(pvnshort);
               return NULL;
             }
            fpath = MemNew((size_t)(StringLen(pvalstr)+PATH_MAX+100)*sizeof(Char));
            if(fpath == NULL)
             {
               ErrPostEx(SEV_ERROR,0,0, "MemNew returned NULL.");
               pvalstr = MemFree(pvalstr);
               pvnshort = ValNodeFree(pvnshort);
               return NULL;
             }
            sprintf(fpath,"%s?fnct=SeqHoundRedundantGroupList&pgi=%s", IsNullStr(slri_cgipath), IsNullStr(pvalstr));
            ErrPostEx(SEV_INFO,0,0, "SeqHoundRedundantGroupList request: %s.\n", IsNullStr(fpath));
            pvalstr = MemFree(pvalstr);
            if(SHoundWWWGetfile(slri_sername, fpath) == 0)
             {
                ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
                fpath = MemFree(fpath);
                pvnshort = ValNodeFree(pvnshort);
                return NULL;
             }
            fpath = MemFree(fpath);
            pvngi = ReplyBSGetValNode(TRUE);
            ValNodeLink(&pvngifin, pvngi);
            counter = 0;
            pvnshort = ValNodeFree(pvnshort);
          }
         pvntemp = pvntemp->next;
      }
     Misc_IntVnpUnique(&pvngifin);
     return pvngifin;
}


Int4 LIBCALL SHoundRedundantGroupIDFromGI(Int4 Gi)
{
     Char fpath[PATH_MAX];
     Int4 id = 0;


     if(Gi <= 0)
      {
        ErrPostEx(SEV_ERROR,0,0, "Invalid parameter.");
        return 0;
      }

     sprintf(fpath,"%s?fnct=SeqHoundRedundantGroupIDFromGI&gi=%ld", IsNullStr(slri_cgipath), (long) Gi);
     ErrPostEx(SEV_INFO,0,0, "SeqHoundRedundantGroupIDFromGI request: %s.\n", IsNullStr(fpath));
     if(SHoundWWWGetfile(slri_sername, fpath) == 0)
      {
         ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
         return FALSE;
      }
     id = ReplyBSGetInteger();
     return id;
}


ValNodePtr LIBCALL SHoundRedundantGroupIDFromGIList(ValNodePtr pvnGi)
{
     CharPtr fpath = NULL;
     CharPtr pvalstr = NULL;
     ValNodePtr pvnid = NULL, pvnidfin = NULL;
     ValNodePtr pvntemp = NULL, pvnshort = NULL;
     Int4 counter = 0;

     if(pvnGi == NULL)
      {
        ErrPostEx(SEV_ERROR,0,0, "Invalid parameter.");
        return NULL;
      }

     pvntemp = pvnGi;
     while (pvntemp != NULL)
      {
         counter++;
         pvnshort = Slri_ValNodeAddInt(&pvnshort, 0, pvntemp->data.intvalue);
         if(counter == MAXQ || pvntemp->next == NULL)
          {
            pvalstr = Misc_SaveValNodeListAsString(pvnshort);
            if(pvalstr == NULL)
             {
               ErrPostEx(SEV_ERROR,0,0, "Misc_SaveValNodeListAsString returned NULL.");
               pvnshort = ValNodeFree(pvnshort);
               return NULL;
             }
            fpath = MemNew((size_t)(StringLen(pvalstr)+PATH_MAX+100)*sizeof(Char));
            if(fpath == NULL)
             {
               ErrPostEx(SEV_ERROR,0,0, "MemNew returned NULL.");
               pvalstr = MemFree(pvalstr);
               pvnshort = ValNodeFree(pvnshort);
               return NULL;
             }
            sprintf(fpath,"%s?fnct=SeqHoundRedundantGroupIDFromGIList&pgi=%s", IsNullStr(slri_cgipath), pvalstr);
            ErrPostEx(SEV_INFO,0,0, "SeqHoundRedundantGroupIDFromGIList request: %s.\n", IsNullStr(fpath));
            pvalstr = MemFree(pvalstr);
            if(SHoundWWWGetfile(slri_sername, fpath) == 0)
             {
                ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
                fpath = MemFree(fpath);
                pvnshort = ValNodeFree(pvnshort);
                return NULL;
             }
            fpath = MemFree(fpath);
            pvnid = ReplyBSGetValNode(TRUE);
            ValNodeLink(&pvnidfin, pvnid);
            counter = 0;
            pvnshort = ValNodeFree(pvnshort);
          }
         pvntemp = pvntemp->next;
      }
     Misc_IntVnpUnique(&pvnidfin);
     return pvnidfin;
}


ValNodePtr LIBCALL SHoundRedundantGroupFromID(Int4 group)
{
     Char fpath[PATH_MAX];
     ValNodePtr pvngi = NULL;


     if(group <= 0)
      {
        ErrPostEx(SEV_ERROR,0,0, "Invalid parameter.");
        return NULL;
      }
     sprintf(fpath,"%s?fnct=SeqHoundRedundantGroupFromID&group=%ld", IsNullStr(slri_cgipath), (long) group);
     ErrPostEx(SEV_INFO,0,0, "SeqHoundRedundantGroupFromID request: %s.\n", IsNullStr(fpath));
     if(SHoundWWWGetfile(slri_sername, fpath) == 0)
      {
         ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
         return NULL;
      }
     pvngi = ReplyBSGetValNode(TRUE);
     return pvngi;
}




ValNodePtr LIBCALL SHoundRedundantGroupFromIDList(ValNodePtr pvngroup)
{
     CharPtr fpath = NULL;
     CharPtr pvalstr = NULL;
     ValNodePtr pvngi = NULL, pvngifin = NULL;
     ValNodePtr pvntemp = NULL, pvnshort = NULL;
     Int4 counter = 0;

     if(pvngroup == NULL)
      {
        ErrPostEx(SEV_ERROR,0,0, "Invalid parameter.");
        return NULL;
      }

     pvntemp = pvngroup;
     while (pvntemp != NULL)
      {
         counter++;
         pvnshort = Slri_ValNodeAddInt(&pvnshort, 0, pvntemp->data.intvalue);
         if(counter == MAXQ || pvntemp->next == NULL)
          {
            pvalstr = Misc_SaveValNodeListAsString(pvnshort);
            if(pvalstr == NULL)
             {
               ErrPostEx(SEV_ERROR,0,0, "Misc_SaveValNodeListAsString returned NULL.");
               pvnshort = ValNodeFree(pvnshort);
               return NULL;
             }
            fpath = MemNew((size_t)(StringLen(pvalstr)+PATH_MAX+100)*sizeof(Char));
            if(fpath == NULL)
             {
               ErrPostEx(SEV_ERROR,0,0, "MemNew returned NULL.");
               pvalstr = MemFree(pvalstr);
               pvnshort = ValNodeFree(pvnshort);
               return NULL;
             }
            sprintf(fpath,"%s?fnct=SeqHoundRedundantGroupFromIDList&pgroup=%s", IsNullStr(slri_cgipath), IsNullStr(pvalstr));
            ErrPostEx(SEV_INFO,0,0, "SeqHoundRedundantGroupFromIDList request: %s.\n", IsNullStr(fpath));
            pvalstr = MemFree(pvalstr);
            if(SHoundWWWGetfile(slri_sername, fpath) == 0)
             {
                ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
                fpath = MemFree(fpath);
                pvnshort = ValNodeFree(pvnshort);
                return NULL;
             }
            fpath = MemFree(fpath);
            pvngi = ReplyBSGetValNode(TRUE);
            ValNodeLink(&pvngifin, pvngi);
            counter = 0;
            pvnshort = ValNodeFree(pvnshort);
          }
         pvntemp = pvntemp->next;
      }
     Misc_IntVnpUnique(&pvngifin);
     return pvngifin;
}



Int4 LIBCALL SHoundFirstOfRedundantGroupFromID(Int4 group)
{
     Char fpath[PATH_MAX];
     Int4 gival = 0;

     if(group <= 0)
      {
        ErrPostEx(SEV_ERROR,0,0, "Invalid parameter.");
        return 0;
      }
     sprintf(fpath,"%s?fnct=SeqHoundFirstOfRedundantGroupFromID&group=%ld", IsNullStr(slri_cgipath), (long) group);
     ErrPostEx(SEV_INFO,0,0, "SeqHoundFirstOfRedundantGroupFromID request: %s.\n", IsNullStr(fpath));
     if(SHoundWWWGetfile(slri_sername, fpath) == 0)
      {
         ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
         return 0;
      }
     gival = ReplyBSGetInteger();
     return gival;
}




ValNodePtr LIBCALL SHoundFirstOfRedundantGroupFromIDList(ValNodePtr pvngroup)
{
     CharPtr fpath = NULL;
     CharPtr pvalstr = NULL;
     ValNodePtr pvngi = NULL, pvngifin = NULL;
     ValNodePtr pvntemp = NULL, pvnshort = NULL;
     Int4 counter = 0;

     if(pvngroup == NULL)
      {
        ErrPostEx(SEV_ERROR,0,0, "Invalid parameter.");
        return NULL;
      }

     pvntemp = pvngroup;
     while (pvntemp != NULL)
      {
         counter++;
         pvnshort = Slri_ValNodeAddInt(&pvnshort, 0, pvntemp->data.intvalue);
         if(counter == MAXQ || pvntemp->next == NULL)
          {
            pvalstr = Misc_SaveValNodeListAsString(pvnshort);
            if(pvalstr == NULL)
             {
               ErrPostEx(SEV_ERROR,0,0, "Misc_SaveValNodeListAsString returned NULL.");
               pvnshort = ValNodeFree(pvnshort);
               return NULL;
             }
            fpath = MemNew((size_t)(StringLen(pvalstr)+PATH_MAX+100)*sizeof(Char));
            if(fpath == NULL)
             {
               ErrPostEx(SEV_ERROR,0,0, "MemNew returned NULL.");
               pvalstr = MemFree(pvalstr);
               pvnshort = ValNodeFree(pvnshort);
               return NULL;
             }
            sprintf(fpath,"%s?fnct=SeqHoundFirstOfRedundantGroupFromIDList&pgroup=%s", IsNullStr(slri_cgipath), IsNullStr(pvalstr));
            ErrPostEx(SEV_INFO,0,0, "SeqHoundFirstOfRedundantGroupIDFromIDList request: %s.\n", IsNullStr(fpath));
            pvalstr = MemFree(pvalstr);
            if(SHoundWWWGetfile(slri_sername, fpath) == 0)
             {
                ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
                fpath = MemFree(fpath);
                pvnshort = ValNodeFree(pvnshort);
                return NULL;
             }
            fpath = MemFree(fpath);
            pvngi = ReplyBSGetValNode(TRUE);
            ValNodeLink(&pvngifin, pvngi);
            counter = 0;
            pvnshort = ValNodeFree(pvnshort);
          }
         pvntemp = pvntemp->next;
      }
     Misc_IntVnpUnique(&pvngifin);
     return pvngifin;
}



/************************************************/
/* Taxonomy                                     */
/************************************************/
ValNodePtr LIBCALL SHoundGetTaxChildNodes(Int4 taxid)
{
     Char fpath[PATH_MAX];
     ValNodePtr pvnnodes = NULL;

     if(taxid <= 0) {
        ErrPostEx(SEV_ERROR,0,0, "Invalid parameter.");
        return NULL;
     }
     sprintf(fpath,"%s?fnct=SeqHoundGetTaxChildNodes&taxid=%ld", IsNullStr(slri_cgipath), (long) taxid);
     ErrPostEx(SEV_INFO,0,0, "SeqHoundGetTaxChildNodes request: %s.\n", IsNullStr(fpath));
     if(SHoundWWWGetfile(slri_sername, fpath) == 0) {
         ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
         return NULL;
     }
     pvnnodes = ReplyBSGetValNode(TRUE);
     return pvnnodes;
}


ValNodePtr LIBCALL SHoundGetTaxChildNodesList(ValNodePtr taxidlist)
{
     CharPtr fpath = NULL;
     CharPtr pvalstr = NULL;
     ValNodePtr pvnnodes = NULL, pvnnodesfin = NULL;
     ValNodePtr pvntemp = NULL, pvnshort = NULL;
     Int4 counter = 0;

     if(taxidlist == NULL)
      {
        ErrPostEx(SEV_ERROR,0,0, "Invalid parameter.");
        return NULL;
      }

     pvntemp = taxidlist;
     while (pvntemp != NULL)
      {
         counter++;
         pvnshort = Slri_ValNodeAddInt(&pvnshort, 0, pvntemp->data.intvalue);
         if(counter == MAXQ || pvntemp->next == NULL)
          {
            pvalstr = Misc_SaveValNodeListAsString(pvnshort);
            if(pvalstr == NULL)
             {
               ErrPostEx(SEV_ERROR,0,0, "Misc_SaveValNodeListAsString returned NULL.");
               pvnshort = ValNodeFree(pvnshort);
               return NULL;
             }
            fpath = MemNew((size_t)(StringLen(pvalstr)+PATH_MAX+100)*sizeof(Char));
            if(fpath == NULL)
             {
               ErrPostEx(SEV_ERROR,0,0, "MemNew returned NULL.");
               pvalstr = MemFree(pvalstr);
               pvnshort = ValNodeFree(pvnshort);
               return NULL;
             }
            sprintf(fpath,"%s?fnct=SeqHoundGetTaxChildNodesList&ptaxid=%s", IsNullStr(slri_cgipath), IsNullStr(pvalstr));
            ErrPostEx(SEV_INFO,0,0, "SeqHoundGetTaxChildNodesList request: %s.\n", IsNullStr(fpath));
            pvalstr = MemFree(pvalstr);
            if(SHoundWWWGetfile(slri_sername, fpath) == 0)
             {
                ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
                fpath = MemFree(fpath);
                pvnshort = ValNodeFree(pvnshort);
                return NULL;
             }
            fpath = MemFree(fpath);
            pvnnodes = ReplyBSGetValNode(TRUE);
            ValNodeLink(&pvnnodesfin, pvnnodes);
            counter = 0;
            pvnshort = ValNodeFree(pvnshort);
          }
         pvntemp = pvntemp->next;
      }
     Misc_IntVnpUnique(&pvnnodesfin);
     return pvnnodesfin;
}


ValNodePtr LIBCALL SHoundGetAllTaxProgeny(Int4 taxid)
{
     Char fpath[PATH_MAX];
     ValNodePtr pvnnodes = NULL;


     if(taxid <= 0)
      {
        ErrPostEx(SEV_ERROR,0,0, "Invalid parameter.");
        return NULL;
      }
     sprintf(fpath,"%s?fnct=SeqHoundGetAllTaxProgeny&taxid=%ld", IsNullStr(slri_cgipath), (long) taxid);
     ErrPostEx(SEV_INFO,0,0, "SeqHoundGetAllTaxProgeny request: %s.\n", IsNullStr(fpath));
     if(SHoundWWWGetfile(slri_sername, fpath) == 0)
      {
         ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
         return NULL;
      }
     pvnnodes = ReplyBSGetValNode(TRUE);
     return pvnnodes;
}


Int4 LIBCALL SHoundGetTaxParent(Int4 taxid)
{
     Char fpath[PATH_MAX];
     Int4 id = 0;


     if(taxid <= 0)
      {
        ErrPostEx(SEV_ERROR,0,0, "Invalid parameter.");
        return 0;
      }

     sprintf(fpath,"%s?fnct=SeqHoundGetTaxParent&taxid=%ld", IsNullStr(slri_cgipath), (long) taxid);
     ErrPostEx(SEV_INFO,0,0, "SeqHoundGetTaxParent request: %s.\n", IsNullStr(fpath));
     if(SHoundWWWGetfile(slri_sername, fpath) == 0)
      {
         ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
         return FALSE;
      }
     id = ReplyBSGetInteger();
     return id;
}



ValNodePtr LIBCALL SHoundGetAllTaxAncestors(Int4 taxid)
{
     Char fpath[PATH_MAX];
     ValNodePtr pvnnodes = NULL;


     if(taxid <= 0)
      {
        ErrPostEx(SEV_ERROR,0,0, "Invalid parameter.");
        return NULL;
      }
     sprintf(fpath,"%s?fnct=SeqHoundGetAllTaxAncestors&taxid=%ld", IsNullStr(slri_cgipath), (long) taxid);
     ErrPostEx(SEV_INFO,0,0, "SeqHoundGetAllTaxAncestors request: %s.\n", IsNullStr(fpath));
     if(SHoundWWWGetfile(slri_sername, fpath) == 0)
      {
         ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
         return NULL;
      }
     pvnnodes = ReplyBSGetValNode(TRUE);
     return pvnnodes;
}


Boolean LIBCALL SHoundIsTaxMerged(Int4 taxid)
{
     Char fpath[PATH_MAX];

     if (taxid <= 0)
      {
        ErrPostEx(SEV_ERROR,0,0, "Invalid parameter.");
        return FALSE;
      }
     sprintf(fpath,"%s?fnct=SeqHoundIsTaxMerged&taxid=%ld", IsNullStr(slri_cgipath), (long) taxid);
     ErrPostEx(SEV_INFO,2,1,"SeqHoundIsTaxMerged request: %s\n", IsNullStr(fpath));
     if(SHoundWWWGetfile(slri_sername, fpath) == 0)
      {
         ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
         return FALSE;
      }
     if(ReplyBSTrueorFalse())
       return TRUE;
     else
       return FALSE;

}


Boolean LIBCALL SHoundIsTaxDeleted(Int4 taxid)
{
     Char fpath[PATH_MAX];

     if (taxid <= 0)
      {
        ErrPostEx(SEV_ERROR,0,0, "Invalid parameter.");
        return FALSE;
      }
     sprintf(fpath,"%s?fnct=SeqHoundIsTaxDeleted&taxid=%ld", IsNullStr(slri_cgipath), (long) taxid);
     ErrPostEx(SEV_INFO,2,1,"SeqHoundIsTaxDeleted request: %s\n", IsNullStr(fpath));
     if(SHoundWWWGetfile(slri_sername, fpath) == 0)
      {
         ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
         return FALSE;
      }
     if(ReplyBSTrueorFalse())
       return TRUE;
     else
       return FALSE;

}


CharPtr LIBCALL SHoundGetTaxNameFromTaxIDByClass(Int4 taxid, Uint2 nameClass)
{

   Char fpath[PATH_MAX];
   CharPtr pname = NULL;

   if (taxid < 0)
   {
      ErrPostEx ( SEV_ERROR, 0, 0, "[SHoundGetTaxNameFromTaxIDByClass]: Invalid taxid." );
      return NULL;
   } else if ( 0 == taxid ) {
      return "unknown";
   }

     sprintf(fpath,"%s?fnct=SeqHoundGetTaxNameFromTaxIDByClass&taxid=%ld&type=%u", IsNullStr(slri_cgipath), (long) taxid, (unsigned int) nameClass);
     ErrPostEx(SEV_INFO,0,0, "SeqHoundGetTaxNameFromTaxIDByClass request: %s.\n", IsNullStr(fpath));
     if(SHoundWWWGetfile(slri_sername, fpath) == 0)
      {
         ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
         return NULL;
      }
     pname = ReplyBSGetString();
     return pname;
}


CharPtr LIBCALL SHoundGetTaxNameFromTaxID(Int4 taxid)
{
   Char fpath[PATH_MAX];
   CharPtr pname = NULL;

   if (taxid < 0)
   {
      ErrPostEx ( SEV_ERROR, 0, 0, "[SHoundGetTaxNameFromTaxID]: Invalid taxid." );
      return NULL;
   } else if ( 0 == taxid ) {
      return "unknown";
   }

   sprintf ( fpath,"%s?fnct=SeqHoundGetTaxNameFromTaxID&taxid=%ld", 
             IsNullStr(slri_cgipath), (long) taxid );
   ErrPostEx ( SEV_INFO, 0, 0, 
	       "[SHoundGetTaxNameFromTaxID]: SeqHoundGetTaxNameFromTaxID request: %s.\n", 
	       IsNullStr(fpath) );

   if( 0 == SHoundWWWGetfile(slri_sername, fpath) )
   {
      ErrPostEx ( SEV_ERROR, 0, 0, "[SHoundGetTaxNameFromTaxID]: SHoundWWWGetfile failed." );
      return NULL;
   }

   pname = ReplyBSGetString();
   return pname;
}

Int4 LIBCALL SHoundGetNewTaxIDFromMergedTaxID(Int4 taxid)
{
   Char fpath[PATH_MAX];
   Int4 id = 0;


   if(taxid <= 0)
   {
       ErrPostEx(SEV_ERROR,0,0, "Invalid parameter.");
       return 0;
   }

   sprintf(fpath,"%s?fnct=SeqHoundGetNewTaxIDFromMergedTaxID&taxid=%ld", IsNullStr(slri_cgipath), (long) taxid);
   ErrPostEx(SEV_INFO,0,0, "SeqHoundGetNewTaxIDFromMergedTaxID request: %s.\n", IsNullStr(fpath));
   if(SHoundWWWGetfile(slri_sername, fpath) == 0)
   {
       ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
       return 0;
   }
   id = ReplyBSGetInteger();
   return id;
}

CharPtr LIBCALL SHoundGetTaxLineageFromTaxID(Int4 taxid, Uint2 type)
{

   Char fpath[PATH_MAX];
   CharPtr pname = NULL;

   if ( taxid < 0 )
   {
      ErrPostEx ( SEV_ERROR, 0, 0, "[SHoundGetTaxLineageFromTaxID]: Invalid taxid." );
      return NULL;
   } else if ( 0 == taxid ) {
      return "unknown";
   }

   sprintf(fpath,"%s?fnct=SeqHoundGetTaxLineageFromTaxID&taxid=%ld&type=%u", IsNullStr(slri_cgipath), (long) taxid, (unsigned int) type);
   ErrPostEx(SEV_INFO,0,0, "SeqHoundGetTaxLineageFromTaxID request: %s.\n", IsNullStr(fpath));
   if(SHoundWWWGetfile(slri_sername, fpath) == 0)
   {
      ErrPostEx ( SEV_ERROR, 0, 0, "[SHoundGetTaxLineageFromTaxID]: SHoundWWWGetfile failed." );
      return NULL;
   }
   pname = ReplyBSGetString();
   return pname;
}

/************************************************/
/* BLAST seqalign fetch                         */
/************************************************/

SLRI_ERR LIBCALL SHoundGetBlastResult(Int4 gi1, Int4 gi2, NBlastResultSetPtr PNTR ppResult)
{
	Char fpath[PATH_MAX];
	Int2 ercode = 0;
	AsnIoBSPtr aipbs = NULL;

	if( (gi1 < 0) || (gi2 < 0)) {
		ErrPostEx(SEV_ERROR,0,0, "SHoundGetBlastResult: Invalid GIs.");
		return SLRI_FAIL;
	}
	if(ppResult == NULL) {
		ErrPostEx(SEV_ERROR, 0, 0, "SHoundGetBlastResult: Address of passed ppResult is NULL (you must pass a variable by reference).");
		return SLRI_FAIL;
	}

	sprintf(fpath,"%s?fnct=SeqHoundGetBlastResult&gi1=%ld&gi2=%ld", IsNullStr(slri_cgipath),(long) gi1,(long) gi2);
	ErrPostEx(SEV_INFO,2,1,"SeqHoundGetBlastResult request: %s\n", IsNullStr(fpath));

	if((ercode = SHoundWWWGetfile(slri_sername, fpath)) == 0) {
		ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
		return SLRI_FAIL;
	} else if (ercode == 2) {
		return SLRI_FAIL;
	}
	if ((aipbs = AsnIoBSOpen(FILE_MODE, replyBS)) == NULL) {
		ErrPostEx(SEV_ERROR,2,1,"AsnIoBSOpen returned NULL.");
		return SLRI_FAIL;
	}
	if ((*ppResult = NBlastResultSetAsnRead(aipbs->aip, NULL)) == NULL) {
		ErrPostEx(SEV_ERROR,2,1,"NBlastResultSetAsnRead returned NULL.");
		AsnIoBSClose(aipbs);
		BSFree(replyBS);
		replyBS = NULL;
		return SLRI_FAIL;
	}
	aipbs = AsnIoBSClose(aipbs);
	BSFree(replyBS);
	replyBS = NULL;
	return SLRI_SUCCESS;
}


SLRI_ERR LIBCALL SHoundGetBlastSeqAlign(Int4 gi1, Int4 gi2, SeqAlignPtr PNTR psap)
{
	Char fpath[PATH_MAX];
	Int2 ercode = 0;
	AsnIoBSPtr aipbs = NULL;

	if( (gi1 < 0) || (gi2 < 0)) {
		ErrPostEx(SEV_ERROR,0,0, "SHoundGetBlastSeqalign: Invalid GIs.");
		return SLRI_FAIL;
	}
	if(psap == NULL) {
		ErrPostEx(SEV_ERROR, 0, 0, "SHoundGetBlastSeqalign: Address of passed psap is NULL (you must pass a variable by reference).");
		return SLRI_FAIL;
	}

	sprintf(fpath,"%s?fnct=SeqHoundGetBlastSeqAlign&gi1=%ld&gi2=%ld", IsNullStr(slri_cgipath),(long) gi1,(long) gi2);
	ErrPostEx(SEV_INFO,2,1,"SeqHoundGetBlastSeqAlign request: %s\n", IsNullStr(fpath));

	if((ercode = SHoundWWWGetfile(slri_sername, fpath)) == 0) {
		ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
		return SLRI_FAIL;
	} else if (ercode == 2) {
		return SLRI_FAIL;
	}
	if ((aipbs = AsnIoBSOpen(FILE_MODE, replyBS)) == NULL) {
		ErrPostEx(SEV_ERROR,2,1,"AsnIoBSOpen returned NULL.");
		return SLRI_FAIL;
	}
	if ((*psap = SeqAlignAsnRead(aipbs->aip, NULL)) == NULL) {
		ErrPostEx(SEV_ERROR,2,1,"SeqAlignAsnRead returned NULL.");
		AsnIoBSClose(aipbs);
		BSFree(replyBS);
		replyBS = NULL;
		return SLRI_FAIL;
	}
	aipbs = AsnIoBSClose(aipbs);
	BSFree(replyBS);
	replyBS = NULL;
	return SLRI_SUCCESS;
}



/************************************************/
/* Neighbours fetch                             */
/************************************************/
SLRI_ERR LIBCALL SHoundNeighboursFromGiEx(Int4 gi, FLinkSetPtr PNTR pNeighbours, FloatHi evalue, Boolean bLimit)
{
	Char fpath[PATH_MAX];
	Int2 ercode = 0;
	AsnIoBSPtr aipbs = NULL;

	if(gi < 0) {
		ErrPostEx(SEV_ERROR,0,0, "SHoundNeighboursFromGiEx: Invalid GI.");
		return SLRI_FAIL;
	}
	if(pNeighbours == NULL) {
		ErrPostEx(SEV_ERROR, 0, 0, "SHoundNeighboursFromGiEx: Address of passed flsp is NULL (you must pass a variable by reference).");
		return SLRI_FAIL;
	}

	sprintf(fpath,"%s?fnct=SeqHoundNeighboursFromGiEx&gi=%ld&evalue=%g&bLimit=%d", IsNullStr(slri_cgipath),(long) gi,(double) evalue,(int)bLimit);
	ErrPostEx(SEV_INFO,2,1,"SeqHoundNeighboursFromGiEx request: %s\n", IsNullStr(fpath));

	if((ercode = SHoundWWWGetfile(slri_sername, fpath)) == 0) {
		ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
		return SLRI_FAIL;
	} else if (ercode == 2) {
		return SLRI_FAIL;
	}
	if ((aipbs = AsnIoBSOpen(FILE_MODE, replyBS)) == NULL) {
		ErrPostEx(SEV_ERROR,2,1,"AsnIoBSOpen returned NULL.");
		return SLRI_FAIL;
	}
	if ((*pNeighbours = FLinkSetAsnRead(aipbs->aip, NULL)) == NULL) {
		ErrPostEx(SEV_ERROR,2,1,"FLinkSetAsnRead returned NULL.");
		AsnIoBSClose(aipbs);
		BSFree(replyBS);
		replyBS = NULL;
		return SLRI_FAIL;
	}
	aipbs = AsnIoBSClose(aipbs);
	BSFree(replyBS);
	replyBS = NULL;
	return SLRI_SUCCESS;
}



SLRI_ERR LIBCALL SHoundNeighboursFromGi(Int4 gi, FLinkSetPtr PNTR pNeighbours, FloatHi evalue, Boolean bLimit)
{
	Char fpath[PATH_MAX];
	Int2 ercode = 0;
	AsnIoBSPtr aipbs = NULL;

	if(gi < 0) {
		ErrPostEx(SEV_ERROR,0,0, "SHoundNeighboursFromGi: Invalid GI.");
		return SLRI_FAIL;
	}
	if(pNeighbours == NULL) {
		ErrPostEx(SEV_ERROR, 0, 0, "SHoundNeighboursFromGi: Address of passed flsp is NULL (you must pass a variable by reference).");
		return SLRI_FAIL;
	}

	sprintf(fpath,"%s?fnct=SeqHoundNeighboursFromGi&gi=%ld&evalue=%g&bLimit=%d", IsNullStr(slri_cgipath),(long) gi,(double) evalue,(int)bLimit);
	ErrPostEx(SEV_INFO,2,1,"SeqHoundNeighboursFromGi request: %s\n", IsNullStr(fpath));

	if((ercode = SHoundWWWGetfile(slri_sername, fpath)) == 0) {
		ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
		return SLRI_FAIL;
	} else if (ercode == 2) {
		return SLRI_FAIL;
	}
	if ((aipbs = AsnIoBSOpen(FILE_MODE, replyBS)) == NULL) {
		ErrPostEx(SEV_ERROR,2,1,"AsnIoBSOpen returned NULL.");
		return SLRI_FAIL;
	}
	if ((*pNeighbours = FLinkSetAsnRead(aipbs->aip, NULL)) == NULL) {
		ErrPostEx(SEV_ERROR,2,1,"FLinkSetAsnRead returned NULL.");
		AsnIoBSClose(aipbs);
		BSFree(replyBS);
		replyBS = NULL;
		return SLRI_FAIL;
	}
	aipbs = AsnIoBSClose(aipbs);
	BSFree(replyBS);
	replyBS = NULL;
	return SLRI_SUCCESS;
}

SLRI_ERR LIBCALL SHoundNeighboursFromGiList(ValNodePtr pvngilist, FLinkSetListPtr PNTR pNeighboursList, FloatHi evalue)
{
	CharPtr fpath = NULL, pcvalstr = NULL;
	Boolean bErr = FALSE;
	Int2 ercode = 0;
	Int4 counter = 0;
	AsnIoBSPtr aipbs = NULL;
	ValNodePtr pvntemp = NULL, pvnshort = NULL;
	FLinkSetListPtr pNeighboursHead = NULL;
	FLinkSetList2Ptr pPackage = NULL;

	if(pvngilist == NULL) {
		ErrPostEx(SEV_ERROR,0,0, "SHoundNeighboursFromGiList: pvngilist is NULL.");
		return SLRI_FAIL;
	}
	if(pNeighboursList == NULL) {
		ErrPostEx(SEV_ERROR, 0, 0, "SHoundNeighboursFromGiList: Address of passed flslp is NULL (you must pass a variable by reference).");
		return SLRI_FAIL;
	}

	SHoundSetTimeout(NBRTO);     
	
	pvntemp = pvngilist;
	while (pvntemp != NULL) {
		counter++;
		if((pvnshort = Slri_ValNodeAddInt(&pvnshort, 0, pvntemp->data.intvalue)) != NULL) {
			if(counter == MAXQ || pvntemp->next == NULL) {
				if((pcvalstr = Misc_SaveValNodeListAsString(pvnshort)) != NULL) {
					if(((fpath = MemNew((size_t)(StringLen(pcvalstr)+PATH_MAX+100)*sizeof(Char))) != NULL)) {
						sprintf(fpath,"%s?fnct=SeqHoundNeighboursFromGiList&gilist=%s&evalue=%g", IsNullStr(slri_cgipath),IsNullStr(pcvalstr),(double) evalue);
						ErrPostEx(SEV_INFO,2,1,"SeqHoundNeighboursFromGiList request: %s\n", IsNullStr(fpath));			
						
						if((ercode = SHoundWWWGetfile(slri_sername, fpath)) == 1) {
							if ((aipbs = AsnIoBSOpen(FILE_MODE, replyBS)) != NULL) {
								if ((pPackage = FLinkSetList2AsnRead(aipbs->aip, NULL)) != NULL) {
									Misc_GenericAddObjectToListEnd((Pointer) &pNeighboursHead, (Pointer) pPackage->set2);
									*pNeighboursList = pNeighboursHead;
									pPackage->set2 = NULL;
									pPackage = FLinkSetList2Free(pPackage);
								} 
							} else {
								ErrPostEx(SEV_ERROR,2,1,"AsnIoBSOpen returned NULL.");
								bErr = TRUE;
							}
						} else if (ercode == 0) {
							ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
							bErr = TRUE;
						} else if (ercode == 2) {
							/* No results */
						}
					} else {
						ErrPostEx(SEV_ERROR,0,0, "MemNew returned NULL.");
						bErr = TRUE;
					}
				} else {
					ErrPostEx(SEV_ERROR,0,0, "Misc_SaveValNodeListAsString returned NULL.");
					bErr = TRUE;
				}
				if(pvnshort) pvnshort = ValNodeFree(pvnshort);
				if(pcvalstr) pcvalstr = MemFree(pcvalstr);
				if(fpath) fpath = MemFree(fpath);
				if(aipbs) aipbs = AsnIoBSClose(aipbs);
				if(replyBS) replyBS = BSFree(replyBS);
				replyBS = NULL;
				counter = 0;
				if(bErr == TRUE) break;
			}
			pvntemp = pvntemp->next;
		}
	}
	SHoundSetTimeout(DEFAULTTO);
	if(pvnshort) pvnshort = ValNodeFree(pvnshort);
	if(bErr == TRUE) return SLRI_FAIL;
	if(*pNeighboursList == NULL) {
		ErrPostEx(SEV_ERROR,0,0, "SHoundNeighboursFromGiList pNeighboursList returned NULL.");
	}
	return SLRI_SUCCESS;
}

SLRI_ERR LIBCALL SHoundNeighboursFromTaxID(Int4 TaxID, FLinkSetListPtr PNTR pNeighboursList, FloatHi evalue)
{
	Char fpath[PATH_MAX];
	Int2 ercode = 0;
	AsnIoBSPtr aipbs = NULL;
	FLinkSetList2Ptr pPackage = NULL;

	if(TaxID < 0) {
		ErrPostEx(SEV_ERROR,0,0, "SHoundNeighboursFromTaxID: Invalid TaxID.");
		return SLRI_FAIL;
	}
	if(pNeighboursList == NULL) {
		ErrPostEx(SEV_ERROR, 0, 0, "SHoundNeighboursFromTaxID: Address of passed flslp is NULL (you must pass a variable by reference).");
		return SLRI_FAIL;
	}

	sprintf(fpath,"%s?fnct=SeqHoundNeighboursFromTaxID&taxid=%ld&evalue=%g", IsNullStr(slri_cgipath),(long) TaxID,(double) evalue);
	ErrPostEx(SEV_INFO,2,1,"SeqHoundNeighboursFromTaxID request: %s\n", IsNullStr(fpath));

	SHoundSetTimeout(NBRTO);
	if((ercode = SHoundWWWGetfile(slri_sername, fpath)) == 0) {
		ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
		return SLRI_FAIL;
	} else if (ercode == 2) {
		return SLRI_FAIL;
	}
	SHoundSetTimeout(DEFAULTTO);
	if ((aipbs = AsnIoBSOpen(FILE_MODE, replyBS)) == NULL) {
		ErrPostEx(SEV_ERROR,2,1,"AsnIoBSOpen returned NULL.");
		return SLRI_FAIL;
	}
	if ((pPackage = FLinkSetList2AsnRead(aipbs->aip, NULL)) == NULL) {
		ErrPostEx(SEV_ERROR,2,1,"FLinkSetList2AsnRead returned NULL.");
		AsnIoBSClose(aipbs);
		BSFree(replyBS);
		return SLRI_FAIL;
	}
	aipbs = AsnIoBSClose(aipbs);
	BSFree(replyBS);
	replyBS = NULL;
	*pNeighboursList = pPackage->set2;
	pPackage->set2 = NULL;
	pPackage = FLinkSetList2Free(pPackage);
	return SLRI_SUCCESS;
}

SLRI_ERR LIBCALL SHoundNeighboursOfNeighbours(Int4 gi, FLinkSetListPtr PNTR ppGINeighboursList, FloatHi evalue)
{	
	Char fpath[PATH_MAX];
	Int2 ercode = 0;
	AsnIoBSPtr aipbs = NULL;
	FLinkSetList2Ptr pPackage = NULL;

	if(gi < 0) {
		ErrPostEx(SEV_ERROR,0,0, "SHoundNeighboursOfNeighbours: Invalid gi.");
		return SLRI_FAIL;
	}
	if(ppGINeighboursList == NULL) {
		ErrPostEx(SEV_ERROR, 0, 0, "SHoundNeighboursOfNeighbours: Address of passed flslp is NULL (you must pass a variable by reference).");
		return SLRI_FAIL;
	}

	sprintf(fpath,"%s?fnct=SeqHoundNeighboursOfNeighbours&gi=%ld&evalue=%g", IsNullStr(slri_cgipath),(long) gi,(double) evalue);
	ErrPostEx(SEV_INFO,2,1,"SeqHoundNeighboursOfNeighbours request: %s\n", IsNullStr(fpath));
	
	SHoundSetTimeout(NBRTO);
	if((ercode = SHoundWWWGetfile(slri_sername, fpath)) == 0) {
		ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
		return SLRI_FAIL;
	} else if (ercode == 2) {
		return SLRI_FAIL;
	}
	SHoundSetTimeout(DEFAULTTO);
	if ((aipbs = AsnIoBSOpen(FILE_MODE, replyBS)) == NULL) {
		ErrPostEx(SEV_ERROR,2,1,"AsnIoBSOpen returned NULL.");
		return SLRI_FAIL;
	}
	if ((pPackage = FLinkSetList2AsnRead(aipbs->aip, NULL)) == NULL) {
		ErrPostEx(SEV_ERROR,2,1,"FLinkSetList2AsnRead returned NULL.");
		AsnIoBSClose(aipbs);
		BSFree(replyBS);
		return SLRI_FAIL;
	}
	aipbs = AsnIoBSClose(aipbs);
	BSFree(replyBS);
	replyBS = NULL;
	*ppGINeighboursList = pPackage->set2;
	pPackage->set2 = NULL;
	pPackage = FLinkSetList2Free(pPackage);
	return SLRI_SUCCESS;
}

SLRI_ERR LIBCALL SHoundNeighboursOfNeighboursList(ValNodePtr pvngilist, FLinkSetList2Ptr PNTR ppAllNeighboursList, FloatHi evalue)
{
	CharPtr fpath = NULL, pcvalstr = NULL;
	Boolean bErr = FALSE;
	Int2 ercode = 0;
	Int4 counter = 0;
	AsnIoBSPtr aipbs = NULL;
	ValNodePtr pvntemp = NULL, pvnshort = NULL;
	FLinkSetList2Ptr pNeighboursListHead = NULL;
	FLinkSetList3Ptr pPackage = NULL;

	if(pvngilist == NULL) {
		ErrPostEx(SEV_ERROR,0,0, "SHoundNeighboursFromGiList: Invalid GI.");
		return SLRI_FAIL;
	}
	if(ppAllNeighboursList == NULL) {
		ErrPostEx(SEV_ERROR, 0, 0, "SHoundNeighboursFromGiList: Address of passed pNeighboursList is null (you must pass a variable by reference).");
		return SLRI_FAIL;
	}
	SHoundSetTimeout(NBRTO);
	
	pvntemp = pvngilist;
	while (pvntemp != NULL) {
		counter++;
		if((pvnshort = Slri_ValNodeAddInt(&pvnshort, 0, pvntemp->data.intvalue)) != NULL) {
			if(counter == MAXNBRQ || pvntemp->next == NULL) {
				if((pcvalstr = Misc_SaveValNodeListAsString(pvnshort)) != NULL) {
					if(((fpath = MemNew((size_t)(StringLen(pcvalstr)+PATH_MAX+100)*sizeof(Char))) != NULL)) {
						sprintf(fpath,"%s?fnct=SeqHoundNeighboursOfNeighboursList&gilist=%s&evalue=%g", IsNullStr(slri_cgipath), IsNullStr(pcvalstr), (double) evalue);
						ErrPostEx(SEV_INFO,2,1,"SeqHoundNeighboursOfNeighboursList request: %s\n", IsNullStr(fpath));
						
						
						if((ercode = SHoundWWWGetfile(slri_sername, fpath)) == 1) {
							if ((aipbs = AsnIoBSOpen(FILE_MODE, replyBS)) != NULL) {
								if ((pPackage = FLinkSetList3AsnRead(aipbs->aip, NULL)) != NULL) {
									Misc_GenericAddObjectToListEnd((Pointer *) &pNeighboursListHead, (Pointer) pPackage->set3);
									*ppAllNeighboursList = pNeighboursListHead;
									pPackage->set3 = NULL;
									pPackage = FLinkSetList3Free(pPackage);
								} 
							} else {
								ErrPostEx(SEV_ERROR,0,0,"AsnIoBSOpen returned NULL.");
								bErr = TRUE;
							}
						} else if (ercode == 0) {
							ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
							bErr = TRUE;
						} else if (ercode == 2) {
							/* No results */
						}			
					} else {
						ErrPostEx(SEV_ERROR,0,0, "MemNew returned NULL.");
						bErr = TRUE;
					}
				} else {
					ErrPostEx(SEV_ERROR,0,0, "Misc_SaveValNodeListAsString returned NULL.");
					bErr = TRUE;
				}
				if(pvnshort) pvnshort = ValNodeFree(pvnshort);
				if(pcvalstr) pcvalstr = MemFree(pcvalstr);
				if(fpath) fpath = MemFree(fpath);
				if(aipbs) aipbs = AsnIoBSClose(aipbs);
				if(replyBS) replyBS = BSFree(replyBS);
				replyBS = NULL;
				counter = 0;
				if(bErr == TRUE) break;
			}
			pvntemp = pvntemp->next;
		}
	}
	SHoundSetTimeout(DEFAULTTO);
	if(pvnshort) pvnshort = ValNodeFree(pvnshort);
	if(bErr == TRUE) return SLRI_FAIL;
	if(*ppAllNeighboursList == NULL) { 
		ErrPostEx(SEV_ERROR,0,0, "SHoundNeighboursFromGiList pNeighboursList returned NULL.");
	}
	return SLRI_SUCCESS;
}

SLRI_ERR LIBCALL SHoundNeighboursOfNeighboursFromTaxID(Int4 TaxID, FLinkSetList2Ptr PNTR ppAllNeighboursList, FloatHi evalue)
{
	Char fpath[PATH_MAX];
	Int2 ercode = 0;
	AsnIoBSPtr aipbs = NULL;
	FLinkSetList3Ptr pPackage = NULL;

	if(TaxID < 0) {
		ErrPostEx(SEV_ERROR,0,0, "SHoundNeighboursOfNeighboursFromTaxID: Invalid TaxID.");
		return SLRI_FAIL;
	}
	if(ppAllNeighboursList == NULL) {
		ErrPostEx(SEV_ERROR, 0, 0, "SHoundNeighboursOfNeighboursFromTaxID: Address of passed flsl2p is NULL (you must pass a variable by reference).");
		return SLRI_FAIL;
	}
	
	sprintf(fpath,"%s?fnct=SeqHoundNeighboursOfNeighboursFromTaxID&taxid=%ld&evalue=%g", IsNullStr(slri_cgipath),(long) TaxID,(double) evalue);
	ErrPostEx(SEV_INFO,2,1,"SeqHoundNeighboursOfNeighboursFromTaxID request: %s\n", IsNullStr(fpath));

	SHoundSetTimeout(NBRTO);
	if((ercode = SHoundWWWGetfile(slri_sername, fpath)) == 0) {
		ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
		return SLRI_FAIL;
	} else if (ercode == 2) {
		return SLRI_FAIL;
	}
	SHoundSetTimeout(DEFAULTTO);
	if ((aipbs = AsnIoBSOpen(FILE_MODE, replyBS)) == NULL) {
		ErrPostEx(SEV_ERROR,2,1,"AsnIoBSOpen returned NULL.");
		return SLRI_FAIL;
	}
	if ((pPackage = FLinkSetList3AsnRead(aipbs->aip, NULL)) == NULL) {
		ErrPostEx(SEV_ERROR,2,1,"FLinkSetList3AsnRead returned NULL.");
		AsnIoBSClose(aipbs);
		BSFree(replyBS);
		return SLRI_FAIL;
	}
	aipbs = AsnIoBSClose(aipbs);
	BSFree(replyBS);
	replyBS = NULL;
	*ppAllNeighboursList = pPackage->set3;
	pPackage->set3 = NULL;
	pPackage = FLinkSetList3Free(pPackage);
	return SLRI_SUCCESS;
}


SLRI_ERR LIBCALL SHound3DNeighboursFromGiEx(Int4 gi, ILinkSetPtr PNTR pp3DNeighbours, FloatHi evalue)
{
	Char fpath[PATH_MAX];
	Int2 ercode = 0;
	AsnIoBSPtr aipbs = NULL;

	if(gi < 0) {
		ErrPostEx(SEV_ERROR,0,0, "SHound3DNeighboursFromGiEx: Invalid GI.");
		return SLRI_FAIL;
	}
	if(pp3DNeighbours == NULL) {
		ErrPostEx(SEV_ERROR, 0, 0, "SHound3DNeighboursFromGiEx: Address of passed ilsp is NULL (you must pass a variable by reference).");
		return SLRI_FAIL;
	}

	sprintf(fpath,"%s?fnct=SeqHound3DNeighboursFromGiEx&gi=%ld&evalue=%g", IsNullStr(slri_cgipath),(long) gi,(double) evalue);
	ErrPostEx(SEV_INFO,2,1,"SeqHound3DNeighboursFromGiEx request: %s\n", IsNullStr(fpath));

	if((ercode = SHoundWWWGetfile(slri_sername, fpath)) == 0) {
		ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
		return SLRI_FAIL;
	} else if (ercode == 2) {
		return SLRI_FAIL;
	}
	if ((aipbs = AsnIoBSOpen(FILE_MODE, replyBS)) == NULL) {
		ErrPostEx(SEV_ERROR,2,1,"AsnIoBSOpen returned NULL.");
		return SLRI_FAIL;
	}
	if ((*pp3DNeighbours = ILinkSetAsnRead(aipbs->aip, NULL)) == NULL) {
		ErrPostEx(SEV_ERROR,2,1,"ILinkSetAsnRead returned NULL.");
		AsnIoBSClose(aipbs);
		BSFree(replyBS);
		return SLRI_FAIL;
	}
	aipbs = AsnIoBSClose(aipbs);
	BSFree(replyBS);
	replyBS = NULL;
	return SLRI_SUCCESS;
}

SLRI_ERR LIBCALL SHound3DNeighboursFromGi(Int4 gi, ILinkSetPtr PNTR pp3DNeighbours, FloatHi evalue)
{
	Char fpath[PATH_MAX];
	Int2 ercode = 0;
	AsnIoBSPtr aipbs = NULL;

	if(gi < 0) {
		ErrPostEx(SEV_ERROR,0,0, "SHound3DNeighboursFromGi: Invalid GI.");
		return SLRI_FAIL;
	}
	if(pp3DNeighbours == NULL) {
		ErrPostEx(SEV_ERROR, 0, 0, "SHound3DNeighboursFromGi: Address of passed ilsp is NULL (you must pass a variable by reference).");
		return SLRI_FAIL;
	}

	sprintf(fpath,"%s?fnct=SeqHound3DNeighboursFromGi&gi=%ld&evalue=%g", IsNullStr(slri_cgipath),(long) gi,(double) evalue);
	ErrPostEx(SEV_INFO,2,1,"SeqHound3DNeighboursFromGi request: %s\n", IsNullStr(fpath));

	if((ercode = SHoundWWWGetfile(slri_sername, fpath)) == 0) {
		ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
		return SLRI_FAIL;
	} else if (ercode == 2) {
		return SLRI_FAIL;
	}
	if ((aipbs = AsnIoBSOpen(FILE_MODE, replyBS)) == NULL) {
		ErrPostEx(SEV_ERROR,2,1,"AsnIoBSOpen returned NULL.");
		return SLRI_FAIL;
	}
	if ((*pp3DNeighbours = ILinkSetAsnRead(aipbs->aip, NULL)) == NULL) {
		ErrPostEx(SEV_ERROR,2,1,"ILinkSetAsnRead returned NULL.");
		AsnIoBSClose(aipbs);
		BSFree(replyBS);
		return SLRI_FAIL;
	}
	aipbs = AsnIoBSClose(aipbs);
	BSFree(replyBS);
	replyBS = NULL;
	return SLRI_SUCCESS;
}


SLRI_ERR LIBCALL SHound3DNeighboursFromGiList(ValNodePtr pvngilist, ILinkSetListPtr PNTR pp3DNeighboursList, FloatHi evalue)
{
	CharPtr fpath = NULL, pcvalstr = NULL;
	Boolean bErr = FALSE;
	Int2 ercode = 0;
	Int4 counter = 0;
	AsnIoBSPtr aipbs = NULL;
	ValNodePtr pvntemp = NULL, pvnshort = NULL;
	ILinkSetListPtr	p3DNeighboursHead = NULL;
	ILinkSetList2Ptr pPackage = NULL;

	if(pvngilist == NULL) {
		ErrPostEx(SEV_ERROR,0,0, "SHound3DNeighboursFromGiList: Invalid pvngilist.");
		return SLRI_FAIL;
	}
	if(pp3DNeighboursList == NULL) {
		ErrPostEx(SEV_ERROR, 0, 0, "SHound3DNeighboursFromGiList: Address of passed ilslp is NULL (you must pass a variable by reference).");
		return SLRI_FAIL;
	}
	
	SHoundSetTimeout(NBRTO);	     
	pvntemp = pvngilist;
	while (pvntemp != NULL) {
		counter++;
		if((pvnshort = Slri_ValNodeAddInt(&pvnshort, 0, pvntemp->data.intvalue)) != NULL) {
			if(counter == MAXQ || pvntemp->next == NULL) {
				if((pcvalstr = Misc_SaveValNodeListAsString(pvnshort)) != NULL) {
					if(((fpath = MemNew((size_t)(StringLen(pcvalstr)+PATH_MAX+100)*sizeof(Char))) != NULL)) {
						sprintf(fpath,"%s?fnct=SeqHound3DNeighboursFromGiList&gilist=%s&evalue=%g", IsNullStr(slri_cgipath),IsNullStr(pcvalstr),(double) evalue);
						ErrPostEx(SEV_INFO,2,1,"SeqHound3DNeighboursFromGiList request: %s\n", IsNullStr(fpath));
						
						SHoundSetTimeout(NBRTO);
						if((ercode = SHoundWWWGetfile(slri_sername, fpath)) == 1) {
							if ((aipbs = AsnIoBSOpen(FILE_MODE, replyBS)) != NULL) {
								if ((pPackage = ILinkSetList2AsnRead(aipbs->aip, NULL)) != NULL) {
									Misc_GenericAddObjectToListEnd((Pointer *)&p3DNeighboursHead,(Pointer)pPackage->set2);
									*pp3DNeighboursList = p3DNeighboursHead;
									pPackage->set2 = NULL;
									pPackage = ILinkSetList2Free(pPackage);
								} else {
									ErrPostEx(SEV_ERROR,2,1,"ILinkSetList2AsnRead returned NULL.");
									bErr = TRUE;
								}
							} else {
								ErrPostEx(SEV_ERROR,2,1,"AsnIoBSOpen returned NULL.");
								bErr = TRUE;
							}
						} else if (ercode == 0) {
							ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
							bErr = TRUE;
						} else if (ercode == 2) {
							/* No results */
						}			
					} else {
						ErrPostEx(SEV_ERROR,0,0, "MemNew returned NULL.");
						bErr = TRUE;
					}
				} else {
					ErrPostEx(SEV_ERROR,0,0, "Misc_SaveValNodeListAsString returned NULL.");
					bErr = TRUE;
				}
				if(pvnshort) pvnshort = ValNodeFree(pvnshort);
				if(pcvalstr) pcvalstr = MemFree(pcvalstr);
				if(fpath) fpath = MemFree(fpath);
				if(aipbs) aipbs = AsnIoBSClose(aipbs);
				if(replyBS) replyBS = BSFree(replyBS);
				counter = 0;
				if(bErr == TRUE) break;
			}
			pvntemp = pvntemp->next;
		}
	}
	SHoundSetTimeout(DEFAULTTO);
	if(pvnshort) pvnshort = ValNodeFree(pvnshort);
	if(bErr == TRUE) return SLRI_FAIL;
	
	if(*pp3DNeighboursList == NULL) {
		ErrPostEx(SEV_ERROR,0,0, "SHound3DNeighboursFromGiList pp3DNeighboursList returned NULL.");
	}
	return SLRI_SUCCESS;
}


SLRI_ERR LIBCALL SHound3DNeighboursFromTaxID(Int4 TaxID, ILinkSetListPtr PNTR pp3DNeighboursList, FloatHi evalue)
{
	Char fpath[PATH_MAX];
	Int2 ercode = 0;
	AsnIoBSPtr aipbs = NULL;
	ILinkSetList2Ptr pPackage = NULL;

	if(TaxID < 0) {
		ErrPostEx(SEV_ERROR,0,0, "SHound3DNeighboursFromTaxID: Invalid TaxID.");
		return SLRI_FAIL;
	}
	if(pp3DNeighboursList == NULL) {
		ErrPostEx(SEV_ERROR, 0, 0, "SHound3DNeighboursFromTaxID: Address of passed ilslp is NULL (you must pass a variable by reference).");
		return SLRI_FAIL;
	}

	sprintf(fpath,"%s?fnct=SeqHound3DNeighboursFromTaxID&taxid=%ld&evalue=%g", IsNullStr(slri_cgipath),(long) TaxID,(double) evalue);
	ErrPostEx(SEV_INFO,2,1,"SeqHound3DNeighboursFromTaxID request: %s\n", IsNullStr(fpath));
	
	SHoundSetTimeout(NBRTO);
	if((ercode = SHoundWWWGetfile(slri_sername, fpath)) == 0) {
		ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
		return SLRI_FAIL;
	} else if (ercode == 2) {
		return SLRI_FAIL;
	}
	SHoundSetTimeout(DEFAULTTO);
	if ((aipbs = AsnIoBSOpen(FILE_MODE, replyBS)) == NULL) {
		ErrPostEx(SEV_ERROR,2,1,"AsnIoBSOpen returned NULL.");
		return SLRI_FAIL;
	}
	if ((pPackage = ILinkSetList2AsnRead(aipbs->aip, NULL)) == NULL) {
		ErrPostEx(SEV_ERROR,2,1,"ILinkSetList2AsnRead returned NULL.");
		AsnIoBSClose(aipbs);
		BSFree(replyBS);
		return SLRI_FAIL;
	}
	AsnIoBSClose(aipbs);
	BSFree(replyBS);

	*pp3DNeighboursList = pPackage->set2;
	pPackage->set2 = NULL;
	pPackage = ILinkSetList2Free(pPackage);
	return SLRI_SUCCESS;
}

SLRI_ERR LIBCALL SHound3DListFromTaxID(Int4 TaxID, ILinkSetPtr PNTR pp3DList)
{
	Char fpath[PATH_MAX];
	Int2 ercode = 0;
	AsnIoBSPtr aipbs = NULL;

	if(TaxID < 0) {
		ErrPostEx(SEV_ERROR,0,0, "SHound3DNeighboursFromTaxID: Invalid TaxID.");
		return SLRI_FAIL;
	}
	if(pp3DList == NULL) {
		ErrPostEx(SEV_ERROR, 0, 0, "SHound3DNeighboursFromTaxID: Address of passed ilsp is NULL (you must pass a variable by reference).");
		return SLRI_FAIL;
	}

	sprintf(fpath,"%s?fnct=SeqHound3DListFromTaxID&taxid=%ld", IsNullStr(slri_cgipath),(long) TaxID);
	ErrPostEx(SEV_INFO,2,1,"SeqHound3DListFromTaxID request: %s\n", IsNullStr(fpath));

	if((ercode = SHoundWWWGetfile(slri_sername, fpath)) == 0) {
		ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
		return SLRI_FAIL;
	} else if (ercode == 2) {
		return SLRI_FAIL;
	}
	if ((aipbs = AsnIoBSOpen(FILE_MODE, replyBS)) == NULL) {
		ErrPostEx(SEV_ERROR,2,1,"AsnIoBSOpen returned NULL.");
		return SLRI_FAIL;
	}
	if ((*pp3DList = ILinkSetAsnRead(aipbs->aip, NULL)) == NULL) {
		ErrPostEx(SEV_ERROR,2,1,"ILinkSetAsnRead returned NULL.");
		AsnIoBSClose(aipbs);
		BSFree(replyBS);
		return SLRI_FAIL;
	}
	AsnIoBSClose(aipbs);
	BSFree(replyBS);
	return SLRI_SUCCESS;
}




SLRI_ERR LIBCALL SHoundGiAndNumNeighboursList (ILinkSetPtr PNTR pilsp)
{
	Char fpath[PATH_MAX];
	Int2 ercode = 0;
	AsnIoBSPtr aipbs = NULL;

	if(pilsp == NULL) {
		ErrPostEx(SEV_ERROR, 0, 0, "SHoundGiAndNumNeighboursList: Address of passed ilsp is NULL (you must pass a variable by reference).");
		return SLRI_FAIL;
	}

	sprintf(fpath,"%s?fnct=SeqHoundGiAndNumNeighboursList", IsNullStr(slri_cgipath));
	ErrPostEx(SEV_INFO,2,1,"SeqHoundGiAndNumNeighboursList request: %s\n", IsNullStr(fpath));
	
	SHoundSetTimeout(NBRTO);
	if((ercode = SHoundWWWGetfile(slri_sername, fpath)) == 0) {
		ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
		return SLRI_FAIL;
	} else if (ercode == 2) {
		return SLRI_FAIL;
	}
	SHoundSetTimeout(DEFAULTTO);
	if ((aipbs = AsnIoBSOpen(FILE_MODE, replyBS)) == NULL) {
		ErrPostEx(SEV_ERROR,2,1,"AsnIoBSOpen returned NULL.");
		return SLRI_FAIL;
	}
	if ((*pilsp = ILinkSetAsnRead(aipbs->aip, NULL)) == NULL) {
		ErrPostEx(SEV_ERROR,2,1,"ILinkSetAsnRead returned NULL.");
		AsnIoBSClose(aipbs);
		BSFree(replyBS);
		return SLRI_FAIL;
	}
	AsnIoBSClose(aipbs);
	BSFree(replyBS);
	replyBS = NULL;
	return SLRI_SUCCESS;
}



SLRI_ERR LIBCALL SHoundNumNeighboursInDB (Int4 PNTR pnum)
{
	Char fpath[PATH_MAX];
	Int2 ercode = 0;

	if(pnum == NULL) {
		ErrPostEx(SEV_ERROR, 0, 0, "SHoundNumNeighboursInDB: Address of passed pnum is NULL (you must pass a variable by reference).");
		return SLRI_FAIL;
	}

	sprintf(fpath,"%s?fnct=SeqHoundNumNeighboursInDB", IsNullStr(slri_cgipath));
	ErrPostEx(SEV_INFO,2,1,"SeqHoundNumNeighboursInDB request: %s\n", IsNullStr(fpath));
	
	SHoundSetTimeout(NBRTO);
	if((ercode = SHoundWWWGetfile(slri_sername, fpath)) == 0) {
		ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
		return SLRI_FAIL;
	} else if (ercode == 2) {
		return SLRI_FAIL;
	}
	SHoundSetTimeout(DEFAULTTO);
	*pnum = ReplyBSGetInteger();
	return SLRI_SUCCESS;
}





/*******************************************************
The Locus Link Remote Function Calls
*******************************************************/

/*retrives all the goids with a locus link id*/
ValNodePtr  LIBCALL SHoundGOIDFromLLID(Int4 llid)
{
  Char fpath[PATH_MAX];
  ValNodePtr pvngoid = NULL;
  Int2 ercode = 0;
  
  /* since goid from llid cross LL-LLink and LL-GO tables, current implementation allows llid
      to be negative*/
  /*if(llid <= 0) {
    ErrPostEx(SEV_ERROR,0,0, "[SHoundGOIDFromLLID]: Invalid parameter.");
    return NULL;
  }*/
  sprintf(fpath,"%s?fnct=SeqHoundGOIDFromLLID&llid=%ld", IsNullStr(slri_cgipath), (long) llid);
  ErrPostEx(SEV_INFO,0,0, "SeqHoundGOIDFromLLID request: %s.\n", IsNullStr(fpath));
  if((ercode = SHoundWWWGetfile(slri_sername, fpath)) == 0) {
    ErrPostEx(SEV_ERROR,0,0, "SHoundGOIDFromLLID SHoundWWWGetfile failed.");
    return NULL;
  } else if (ercode == 2) {
    return NULL;
  }
  pvngoid = ReplyBSGetValNode(TRUE);
  return pvngoid;
}

/*retrives an OMIM ids from a locus link id*/
Int4 LIBCALL SHoundOMIMFromLLID(Int4 llid)
{

  Char fpath[PATH_MAX];
  Int4 omimid = 0;


  if(llid <= 0)
    {
      ErrPostEx(SEV_ERROR,0,0, "[SHoundOMIMFromLLID]: Invalid parameter.");
      return 0;
    }
  
  sprintf(fpath,"%s?fnct=SeqHoundOMIMFromLLID&llid=%ld", IsNullStr(slri_cgipath), (long)llid );
  ErrPostEx(SEV_INFO,0,0, "SeqHoundOMIMFromLLID request: %s.\n", IsNullStr(fpath));
  if(SHoundWWWGetfile(slri_sername, fpath) == 0)
    {
      ErrPostEx(SEV_ERROR,0,0, "[SHoundOMIMFromLLID] SHoundWWWGetfile failed.");
      return 0;
    }
  omimid=ReplyBSGetInteger();
  if (omimid == 0)
    {
      ErrPostEx(SEV_INFO,0,0, "[SHoundOMIMFromLLID] returned zero.");
      return 0;
    }
  return omimid;
}

/*retrieves all the conserved domain ids from a locus link id*/
ValNodePtr  LIBCALL SHoundCDDIDFromLLID(Int4 llid)
{
  Char fpath[PATH_MAX];
  ValNodePtr pvncddid = NULL;
  Int2 ercode = 0;

  if(llid <= 0) {
    ErrPostEx(SEV_ERROR,0,0, "[SHoundCDDIDFromLLID]: Invalid parameter.");
    return NULL;
  }
  sprintf(fpath,"%s?fnct=SeqHoundCDDIDFromLLID&llid=%ld", IsNullStr(slri_cgipath), (long) llid);
  ErrPostEx(SEV_INFO,0,0, "SeqHoundCDDIDFromLLID request: %s.\n", IsNullStr(fpath));
  if((ercode = SHoundWWWGetfile(slri_sername, fpath)) == 0) {
    ErrPostEx(SEV_ERROR,0,0, "[SHoundCDDIDFromLLID] SHoundWWWGetfile failed.");
    return NULL;
  } else if (ercode == 2) {
    return NULL;
  }
  pvncddid = ReplyBSGetValNode(FALSE);
  return pvncddid;
}

/*retrieves all the locus link ids from a goid and a go evidence code*/
ValNodePtr  LIBCALL SHoundLLIDFromGOIDAndECode(Int4 goid, CharPtr ecode)
{
  Char fpath[PATH_MAX];
  ValNodePtr vnpllid = NULL;
  Int2 ercode = 0;

  if(goid <= 0 || ecode == NULL || StrLen(ecode) == 0) {
    ErrPostEx(SEV_ERROR,0,0, "[SHoundLLIDFromGOIDAndECode] Invalid parameter.");
    return NULL;
  }
  sprintf(fpath,"%s?fnct=SeqHoundLLIDFromGOIDAndECode&goid=%ld&ecode=%s", IsNullStr(slri_cgipath), (long)goid, IsNullStr(ecode));
  ErrPostEx(SEV_INFO,0,0, "SeqHoundLLIDFromGOIDAndECode request: %s.\n", IsNullStr(fpath));
  if((ercode = SHoundWWWGetfile(slri_sername, fpath)) == 0) {
    ErrPostEx(SEV_ERROR,0,0, "[SHoundLLIDFromGOIDAndECode] SHoundWWWGetfile failed.");
    return NULL;
  } else if (ercode == 2) {
    return NULL;
  }
  vnpllid = ReplyBSGetValNode(TRUE);
  return vnpllid;

}

/*retrieves all the locus link ids from an omim id*/
ValNodePtr  LIBCALL SHoundLLIDFromOMIM(Int4 omimid)
{
  Char fpath[PATH_MAX];
  ValNodePtr pvnllid = NULL;
  Int2 ercode = 0;

  if(omimid <= 0) {
    ErrPostEx(SEV_ERROR,0,0, "[SHoundLLIDFromOMIM] Invalid parameter.");
    return NULL;
  }
  sprintf(fpath,"%s?fnct=SeqHoundLLIDFromOMIM&omimid=%ld", IsNullStr(slri_cgipath), (long) omimid);
  ErrPostEx(SEV_INFO,0,0, "SeqHoundLLIDFromOMIM request: %s.\n", IsNullStr(fpath));
  if((ercode = SHoundWWWGetfile(slri_sername, fpath)) == 0) {
    ErrPostEx(SEV_ERROR,0,0, "[SHoundLLIDFromOMIM] SHoundWWWGetfile failed.");
    return NULL;
  } else if (ercode == 2) {
    return NULL;
  }
  pvnllid = ReplyBSGetValNode(TRUE);
  return pvnllid;
}

/*retrives all the locus link ids from a conserved domain id*/
ValNodePtr  LIBCALL SHoundLLIDFromCDDID(CharPtr cddid)
{
  Char fpath[PATH_MAX];
  ValNodePtr pvnllid = NULL;
  Int2 ercode = 0;

  if(cddid == NULL || StrLen(cddid) == 0) {
    ErrPostEx(SEV_ERROR,0,0, "[SHoundLLIDFromCDDID] Invalid parameter.");
    return NULL;
  }
  sprintf(fpath,"%s?fnct=SeqHoundLLIDFromCDDID&cddid=%s", IsNullStr(slri_cgipath), IsNullStr(cddid));
  ErrPostEx(SEV_INFO,0,0, "SeqHoundLLIDFromCDDID request: %s.\n", IsNullStr(fpath));
  if((ercode = SHoundWWWGetfile(slri_sername, fpath)) == 0) {
    ErrPostEx(SEV_ERROR,0,0, "[SHoundLLIDFromCDDID] SHoundWWWGetfile failed.");
    return NULL;
  } else if (ercode == 2) {
    return NULL;
  }
  pvnllid = ReplyBSGetValNode(TRUE);
  return pvnllid;
}

/* returns all the gi with GOID */
ValNodePtr  LIBCALL SHoundGiFromGOID(Int4 GOID)
{
  Char fpath[PATH_MAX];
  ValNodePtr pvnmuid = NULL;
  Int2 ercode = 0;

  if(GOID <= 0) {
    ErrPostEx(SEV_ERROR,0,0, "Invalid parameter.");
    return NULL;
  }
  sprintf(fpath,"%s?fnct=SeqHoundGiFromGOID&goid=%ld", IsNullStr(slri_cgipath), (long) GOID);
  ErrPostEx(SEV_INFO,0,0, "SeqHoundGiFromGOID request: %s.\n", IsNullStr(fpath));
  if((ercode = SHoundWWWGetfile(slri_sername, fpath)) == 0) {
    ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
    return NULL;
  } else if (ercode == 2) {
    return NULL;
  }
  pvnmuid = ReplyBSGetValNode(TRUE);
  return pvnmuid;
}

/*returns all the gis with the given goid and evidence code*/
ValNodePtr  LIBCALL SHoundGiFromGOIDAndECode(Int4 goid, CharPtr ecode)
{
  Char fpath[PATH_MAX];
  ValNodePtr vnpgi = NULL;
  Int2 ercode = 0;

  if(goid <= 0 || ecode == NULL) {
    ErrPostEx(SEV_ERROR,0,0, "SHoundGiFromGOIDAndECode:Invalid parameter.");
    return NULL;
  }
  sprintf(fpath,"%s?fnct=SeqHoundGiFromGOIDAndECode&goid=%ld&ecode=%s", IsNullStr(slri_cgipath), (long)goid, ecode);
  ErrPostEx(SEV_INFO,0,0, "SeqHoundGiFromGOIDAndECode request: %s.\n", IsNullStr(fpath));
  if((ercode = SHoundWWWGetfile(slri_sername, fpath)) == 0) {
    ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
    return NULL;
  } else if (ercode == 2) {
    return NULL;
  }
  vnpgi = ReplyBSGetValNode(TRUE);
  return vnpgi;

}



/* returns all the gis for given goid list and the evidence code */
ValNodePtr  LIBCALL SHoundGiFromGOIDListAndECode(ValNodePtr vnpgo, CharPtr ecode)
{
  CharPtr fpath = NULL;
  CharPtr pstr = NULL;
  ValNodePtr pvntemp = NULL, pvnshort = NULL, pvnfin = NULL, pvngi = NULL;
  Int4 counter = 0;

  if(vnpgo == NULL || ecode == NULL)
  {
    ErrPostEx(SEV_ERROR,0,0, "SHoundGiFromGOIDListAndECode: Invalid parameter.");
    return NULL;
  }

  pvntemp = vnpgo;
  while (pvntemp != NULL)
  {
     counter++;
     pvnshort = Slri_ValNodeAddInt(&pvnshort, 0, pvntemp->data.intvalue);
     if(counter == MAXQ || pvntemp->next == NULL)
     {
        pstr = Misc_SaveValNodeListAsString(pvnshort);
        if(pstr == NULL)
        {
           ErrPostEx(SEV_ERROR,0,0, "SHoundGiFromGOIDListAndECode: Misc_SaveValNodeListAsString returned NULL.");
           pvnshort = ValNodeFree(pvnshort);
           return NULL;
        }
        fpath = MemNew((size_t)(StringLen(pstr)+PATH_MAX+100)*sizeof(Char));
        if(fpath == NULL)
        {
           ErrPostEx(SEV_ERROR,0,0, "SHoundGiFromGOIDListAndECode: MemNew returned NULL.");
           pstr = MemFree(pstr);
           pvnshort = ValNodeFree(pvnshort);
           return NULL;
        }
        sprintf(fpath,"%s?fnct=SeqHoundGiFromGOIDListAndECode&golist=%s&ecode=%s", IsNullStr(slri_cgipath), pstr, ecode);
        ErrPostEx(SEV_INFO,0,0, "SHoundGiFromGOIDListAndECode request: %s.\n", IsNullStr(fpath));
        pstr = MemFree(pstr);
        if(SHoundWWWGetfile(slri_sername, fpath) == 0)
        {
           ErrPostEx(SEV_ERROR,0,0, "SHoundGiFromGOIDListAndECode: SHoundWWWGetfile failed.");
           fpath = MemFree(fpath);
           pvnshort = ValNodeFree(pvnshort);
           return NULL;
        }
        
        fpath = MemFree(fpath);
        pvngi = ReplyBSGetValNode(TRUE);
        ValNodeLink(&pvnfin, pvngi);
        counter = 0;
        pvnshort = ValNodeFree(pvnshort);
     }
     pvntemp = pvntemp->next;
  }
  Misc_IntVnpUnique(&pvnfin);
  return pvnfin;
}



/* returns all the gi of all the instances of vnpgo */
ValNodePtr  LIBCALL SHoundGiFromGOIDList(ValNodePtr vnpgo)
{
  CharPtr fpath = NULL;
  CharPtr pstr = NULL;
  ValNodePtr pvntemp = NULL, pvnshort = NULL, pvnfin = NULL, pvnid = NULL;
  Int4 counter = 0;

  if(vnpgo == NULL)
  {
    ErrPostEx(SEV_ERROR,0,0, "Invalid parameter.");
    return NULL;
  }

  pvntemp = vnpgo;
  while (pvntemp != NULL)
  {
     counter++;
     pvnshort = Slri_ValNodeAddInt(&pvnshort, 0, pvntemp->data.intvalue);
     if(counter == MAXQ || pvntemp->next == NULL)
     {
        pstr = Misc_SaveValNodeListAsString(pvnshort);
        if(pstr == NULL)
        {
           ErrPostEx(SEV_ERROR,0,0, "Misc_SaveValNodeListAsString returned NULL.");
           pvnshort = ValNodeFree(pvnshort);
           return NULL;
        }
        fpath = MemNew((size_t)(StringLen(pstr)+PATH_MAX+100)*sizeof(Char));
        if(fpath == NULL)
        {
           ErrPostEx(SEV_ERROR,0,0, "MemNew returned NULL.");
           pstr = MemFree(pstr);
           pvnshort = ValNodeFree(pvnshort);
           return NULL;
        }
        sprintf(fpath,"%s?fnct=SeqHoundGiFromGOIDList&vnpgo=%s", IsNullStr(slri_cgipath), IsNullStr(pstr));
        ErrPostEx(SEV_INFO,0,0, "SeqHoundGiFromGOIDList request: %s.\n", IsNullStr(fpath));
        pstr = MemFree(pstr);
        if(SHoundWWWGetfile(slri_sername, fpath) == 0)
        {
           ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
           fpath = MemFree(fpath);
           pvnshort = ValNodeFree(pvnshort);
           return NULL;
        }
        
        fpath = MemFree(fpath);
        pvnid = ReplyBSGetValNode(TRUE);
        ValNodeLink(&pvnfin, pvnid);
        counter = 0;
        pvnshort = ValNodeFree(pvnshort);
     }
     pvntemp = pvntemp->next;
  }
  Misc_IntVnpUnique(&pvnfin);
  return pvnfin;
}


/* returns all the GOID of Gi */
ValNodePtr  LIBCALL SHoundGOIDFromGi(Int4 Gi)
{
  Char fpath[PATH_MAX];
  ValNodePtr vnpgo = NULL;
  Int2 ercode = 0;


  if(Gi <= 0) {
    ErrPostEx(SEV_ERROR,0,0, "Invalid parameter.");
    return NULL;
  }
  sprintf(fpath,"%s?fnct=SeqHoundGOIDFromGi&gi=%ld", IsNullStr(slri_cgipath), (long) Gi);
  ErrPostEx(SEV_INFO,0,0, "SeqHoundGOIDFromGi request: %s.\n", IsNullStr(fpath));
  if((ercode = SHoundWWWGetfile(slri_sername, fpath)) == 0) {
    ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
    return NULL;
  } else if (ercode == 2) {
    return NULL;
  }
  vnpgo = ReplyBSGetValNode(TRUE);
  return vnpgo;
}


/* returns all the GOID for all the instances of vnpgi */
ValNodePtr  LIBCALL SHoundGOIDFromGiList(ValNodePtr vnpgi)
{
  CharPtr fpath = NULL;
  CharPtr pstr = NULL;
  ValNodePtr pvntemp = NULL, pvnshort = NULL, pvnfin = NULL, pvnid = NULL;
  Int4 counter = 0;

  if(vnpgi == NULL)
  {
    ErrPostEx(SEV_ERROR,0,0, "Invalid parameter.");
    return NULL;
  }

  pvntemp = vnpgi;
  while (pvntemp != NULL)
  {
     counter++;
     pvnshort = Slri_ValNodeAddInt(&pvnshort, 0, pvntemp->data.intvalue);
     if(counter == MAXQ || pvntemp->next == NULL)
     {
        pstr = Misc_SaveValNodeListAsString(pvnshort);
        if(pstr == NULL)
        {
           ErrPostEx(SEV_ERROR,0,0, "Misc_SaveValNodeListAsString returned NULL.");
           pvnshort = ValNodeFree(pvnshort);
           return NULL;
        }
        fpath = MemNew((size_t)(StringLen(pstr)+PATH_MAX+100)*sizeof(Char));
        if(fpath == NULL)
        {
           ErrPostEx(SEV_ERROR,0,0, "MemNew returned NULL.");
           pstr = MemFree(pstr);
           pvnshort = ValNodeFree(pvnshort);
           return NULL;
        }
        sprintf(fpath,"%s?fnct=SeqHoundGOIDFromGiList&vnpgi=%s", IsNullStr(slri_cgipath), IsNullStr(pstr));
        ErrPostEx(SEV_INFO,0,0, "SeqHoundGOIDFromGiList request: %s.\n", IsNullStr(fpath));
        pstr = MemFree(pstr);
        if(SHoundWWWGetfile(slri_sername, fpath) == 0)
        {
           ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
           fpath = MemFree(fpath);
           pvnshort = ValNodeFree(pvnshort);
           return NULL;
        }
       
        fpath = MemFree(fpath);
        pvnid = ReplyBSGetValNode(TRUE);
        ValNodeLink(&pvnfin, pvnid);
        counter = 0;
        pvnshort = ValNodeFree(pvnshort);
     }
     pvntemp = pvntemp->next;
  }
  Misc_IntVnpUnique(&pvnfin);
  return pvnfin;
}

/* returns all the GOID of GI and its redundant group */
ValNodePtr  LIBCALL SHoundGOIDFromRedundantGi(Int4 Gi)
{
  Char fpath[PATH_MAX];
  ValNodePtr vnpgo = NULL;
  Int2 ercode = 0;


  if(Gi <= 0) {
    ErrPostEx(SEV_ERROR,0,0, "Invalid parameter.");
    return NULL;
  }
  sprintf(fpath,"%s?fnct=SeqHoundGOIDFromRedundantGi&gi=%ld", IsNullStr(slri_cgipath), (long) Gi);
  ErrPostEx(SEV_INFO,0,0, "SeqHoundGOIDFromRedundantGi request: %s.\n", IsNullStr(fpath));
  if((ercode = SHoundWWWGetfile(slri_sername, fpath)) == 0) {
    ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
    return NULL;
  } else if (ercode == 2) {
    return NULL;
  }
  vnpgo = ReplyBSGetValNode(TRUE);
  return vnpgo;
}

/* returns all the GOID of GI list and all redundant groups */
ValNodePtr  LIBCALL SHoundGOIDFromRedundantGiList(ValNodePtr vnpgi)
{
  CharPtr fpath = NULL;
  CharPtr pstr = NULL;
  ValNodePtr pvntemp = NULL, pvnshort = NULL, pvnfin = NULL, pvnid = NULL;
  Int4 counter = 0;

  if(vnpgi == NULL)
  {
    ErrPostEx(SEV_ERROR,0,0, "Invalid parameter.");
    return NULL;
  }

  pvntemp = vnpgi;
  while (pvntemp != NULL)
  {
     counter++;
     pvnshort = Slri_ValNodeAddInt(&pvnshort, 0, pvntemp->data.intvalue);
     if(counter == MAXQ || pvntemp->next == NULL)
     {
        pstr = Misc_SaveValNodeListAsString(pvnshort);
        if(pstr == NULL)
        {
           ErrPostEx(SEV_ERROR,0,0, "Misc_SaveValNodeListAsString returned NULL.");
           pvnshort = ValNodeFree(pvnshort);
           return NULL;
        }
        fpath = MemNew((size_t)(StringLen(pstr)+PATH_MAX+100)*sizeof(Char));
        if(fpath == NULL)
        {
           ErrPostEx(SEV_ERROR,0,0, "MemNew returned NULL.");
           pstr = MemFree(pstr);
           pvnshort = ValNodeFree(pvnshort);
           return NULL;
        }
        sprintf(fpath,"%s?fnct=SeqHoundGOIDFromRedundantGiList&vnpgi=%s", IsNullStr(slri_cgipath), IsNullStr(pstr));
        ErrPostEx(SEV_INFO,0,0, "SeqHoundGOIDFromRedundantGiList request: %s.\n", IsNullStr(fpath));
        pstr = MemFree(pstr);
        if(SHoundWWWGetfile(slri_sername, fpath) == 0)
        {
           ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
           fpath = MemFree(fpath);
           pvnshort = ValNodeFree(pvnshort);
           return NULL;
        }
        
        fpath = MemFree(fpath);
        pvnid = ReplyBSGetValNode(TRUE);
        ValNodeLink(&pvnfin, pvnid);
        counter = 0;
        pvnshort = ValNodeFree(pvnshort);
     }
     pvntemp = pvntemp->next;
  }
  Misc_IntVnpUnique(&pvnfin);
  return pvnfin;
}


/* returns the GO Evidence Code of Gi */
ValNodePtr  LIBCALL SHoundGOECodeFromGiAndGOID(Int4 Gi, Int4 goid)
{
  Char fpath[PATH_MAX];
  ValNodePtr vnpecode = NULL;
  Int2 ercode = 0;

  if(Gi <= 0 || goid <= 0) {
    ErrPostEx(SEV_ERROR,0,0, "SHoundGOECodeFromGiAndGOID: invalid parameter.");
    return NULL;
  }
  sprintf(fpath,"%s?fnct=SeqHoundGOECodeFromGiAndGOID&gi=%ld&goid=%ld", IsNullStr(slri_cgipath), (long)Gi, (long)goid);
  ErrPostEx(SEV_INFO,0,0, "SeqHoundGOECodeFromGiAndGOID request: %s.\n", IsNullStr(fpath));
  if((ercode = SHoundWWWGetfile(slri_sername, fpath)) == 0) {
    ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
    return NULL;
  } else if (ercode == 2) {
    return NULL;
  }
  vnpecode = ReplyBSGetValNode(FALSE);
  if (vnpecode == NULL)
    {
      ErrPostEx(SEV_INFO,0,0, "SeqHoundGOECodeFromGiAndGOID returned NULL.");
      return NULL;
    }
  return vnpecode;

}


/* returns the GO PMID of Gi */
ValNodePtr  LIBCALL SHoundGOPMIDFromGiAndGOID(Int4 Gi, Int4 goid)
{
  Char fpath[PATH_MAX];
  ValNodePtr vnpgopmid = NULL;
  Int2 ercode = 0;


  if(Gi <= 0 || goid <= 0 ) {
    ErrPostEx(SEV_ERROR,0,0, "SHoundGOPMIDFromGiAndGOID: Invalid parameter.");
    return NULL;
  }
  sprintf(fpath,"%s?fnct=SeqHoundGOPMIDFromGiAndGOID&gi=%ld&goid=%ld", IsNullStr(slri_cgipath), (long) Gi, (long)goid);
  ErrPostEx(SEV_INFO,0,0, "SeqHoundGOPMIDFromGiAndGOID request: %s.\n", IsNullStr(fpath));
  if((ercode = SHoundWWWGetfile(slri_sername, fpath)) == 0) {
    ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
    return NULL;
  } else if (ercode == 2) {
    return NULL;
  }
  vnpgopmid = ReplyBSGetValNode(TRUE);
  return vnpgopmid;
}


/* returns all the Gi of all with OMIM */
ValNodePtr  LIBCALL SHoundGiFromOMIM(Int4 omimid)
{
  Char fpath[PATH_MAX];
  ValNodePtr vnpgi = NULL;
  Int2 ercode = 0;

  if(omimid <= 0) {
    ErrPostEx(SEV_ERROR,0,0, "Invalid parameter.");
    return NULL;
  }
  sprintf(fpath,"%s?fnct=SeqHoundGiFromOMIM&omimid=%ld", IsNullStr(slri_cgipath), (long) omimid);
  ErrPostEx(SEV_INFO,0,0, "SeqHoundGiFromOMIM request: %s.\n", IsNullStr(fpath));
  if((ercode = SHoundWWWGetfile(slri_sername, fpath)) == 0) {
    ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
    return NULL;
  } else if (ercode == 2) {
    return NULL;
  }
  vnpgi = ReplyBSGetValNode(TRUE);
  return vnpgi;
}

/* returns all Gi of all the instances of vnpomim */
ValNodePtr  LIBCALL SHoundGiFromOMIMList(ValNodePtr vnpomim)
{
  CharPtr fpath = NULL;
  CharPtr pstr = NULL;
  ValNodePtr pvntemp = NULL, pvnshort = NULL, pvnfin = NULL, pvnid = NULL;
  Int4 counter = 0;

  if(vnpomim == NULL)
  {
    ErrPostEx(SEV_ERROR,0,0, "Invalid parameter.");
    return NULL;
  }

  pvntemp = vnpomim;
  while (pvntemp != NULL)
  {
     counter++;
     pvnshort = Slri_ValNodeAddInt(&pvnshort, 0, pvntemp->data.intvalue);
     if(counter == MAXQ || pvntemp->next == NULL)
     {
        pstr = Misc_SaveValNodeListAsString(pvnshort);
        if(pstr == NULL)
        {
           ErrPostEx(SEV_ERROR,0,0, "Misc_SaveValNodeListAsString returned NULL.");
           pvnshort = ValNodeFree(pvnshort);
           return NULL;
        }
        fpath = MemNew((size_t)(StringLen(pstr)+PATH_MAX+100)*sizeof(Char));
        if(fpath == NULL)
        {
           ErrPostEx(SEV_ERROR,0,0, "MemNew returned NULL.");
           pstr = MemFree(pstr);
           pvnshort = ValNodeFree(pvnshort);
           return NULL;
        }
        sprintf(fpath,"%s?fnct=SeqHoundGiFromOMIMList&vnpomim=%s", IsNullStr(slri_cgipath), IsNullStr(pstr));
        ErrPostEx(SEV_INFO,0,0, "SeqHoundGiFromOMIMList request: %s.\n", IsNullStr(fpath));
        pstr = MemFree(pstr);
        if(SHoundWWWGetfile(slri_sername, fpath) == 0)
        {
           ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
           fpath = MemFree(fpath);
           pvnshort = ValNodeFree(pvnshort);
           return NULL;
        }
        
        fpath = MemFree(fpath);
        pvnid = ReplyBSGetValNode(TRUE);
        ValNodeLink(&pvnfin, pvnid);
        counter = 0;
        pvnshort = ValNodeFree(pvnshort);
     }
     pvntemp = pvntemp->next;
  }
  Misc_IntVnpUnique(&pvnfin);
  return pvnfin;
}


/* returns all the omim of Gi */
ValNodePtr  LIBCALL SHoundOMIMFromGi(Int4 Gi)
{
  Char fpath[PATH_MAX];
  ValNodePtr vnpomim = NULL;
  Int2 ercode = 0;


  if(Gi <= 0) {
    ErrPostEx(SEV_ERROR,0,0, "Invalid parameter.");
    return NULL;
  }
  sprintf(fpath,"%s?fnct=SeqHoundOMIMFromGi&gi=%ld", IsNullStr(slri_cgipath), (long) Gi);
  ErrPostEx(SEV_INFO,0,0, "SeqHoundOMIMFromGi request: %s.\n", IsNullStr(fpath));
  if((ercode = SHoundWWWGetfile(slri_sername, fpath)) == 0) {
    ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
    return NULL;
  } else if (ercode == 2) {
    return NULL;
  }
  vnpomim = ReplyBSGetValNode(TRUE);
  return vnpomim;
}

/* returns all the OMIM of all the instances of vnpgi */
ValNodePtr  LIBCALL SHoundOMIMFromGiList(ValNodePtr vnpgi)
{
  CharPtr fpath = NULL;
  CharPtr pstr = NULL;
  ValNodePtr pvntemp = NULL, pvnshort = NULL, pvnfin = NULL, pvnid = NULL;
  Int4 counter = 0;

  if(vnpgi == NULL)
  {
    ErrPostEx(SEV_ERROR,0,0, "Invalid parameter.");
    return NULL;
  }

  pvntemp = vnpgi;
  while (pvntemp != NULL)
  {
     counter++;
     pvnshort = Slri_ValNodeAddInt(&pvnshort, 0, pvntemp->data.intvalue);
     if(counter == MAXQ || pvntemp->next == NULL)
     {
        pstr = Misc_SaveValNodeListAsString(pvnshort);
        if(pstr == NULL)
        {
           ErrPostEx(SEV_ERROR,0,0, "Misc_SaveValNodeListAsString returned NULL.");
           pvnshort = ValNodeFree(pvnshort);
           return NULL;
        }
        fpath = MemNew((size_t)(StringLen(pstr)+PATH_MAX+100)*sizeof(Char));
        if(fpath == NULL)
        {
           ErrPostEx(SEV_ERROR,0,0, "MemNew returned NULL.");
           pstr = MemFree(pstr);
           pvnshort = ValNodeFree(pvnshort);
           return NULL;
        }
        sprintf(fpath,"%s?fnct=SeqHoundOMIMFromGiList&vnpgi=%s", IsNullStr(slri_cgipath), IsNullStr(pstr));
        ErrPostEx(SEV_INFO,0,0, "SeqHoundOMIMFromGiList request: %s.\n", IsNullStr(fpath));
        pstr = MemFree(pstr);
        if(SHoundWWWGetfile(slri_sername, fpath) == 0)
        {
           ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
           fpath = MemFree(fpath);
           pvnshort = ValNodeFree(pvnshort);
           return NULL;
        }
       
        fpath = MemFree(fpath);
        pvnid = ReplyBSGetValNode(TRUE);
        ValNodeLink(&pvnfin, pvnid);
        counter = 0;
        pvnshort = ValNodeFree(pvnshort);
     }
     pvntemp = pvntemp->next;
  }
  Misc_IntVnpUnique(&pvnfin);
  return pvnfin;
}


/* returns the Gi of all with cddid */
ValNodePtr  LIBCALL SHoundGiFromCDDID(CharPtr cddid)
{
  Char fpath[PATH_MAX];
  ValNodePtr vnpgi = NULL;
  Int2 ercode = 0;

  if(cddid == NULL) {
    ErrPostEx(SEV_ERROR,0,0, "Invalid parameter.");
    return NULL;
  }

  sprintf(fpath,"%s?fnct=SeqHoundGiFromCDDID&cddid=%s", IsNullStr(slri_cgipath), IsNullStr(cddid));
  ErrPostEx(SEV_INFO,0,0, "SeqHoundGiFromCDDID request: %s.\n", IsNullStr(fpath));
  if((ercode = SHoundWWWGetfile(slri_sername, fpath)) == 0) {
    ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
    return NULL;
  }  else if (ercode == 2) {
    return NULL;
  }
  vnpgi = ReplyBSGetValNode(TRUE);
  return vnpgi;
}


/* returns all the gi of all the instances of vnpcdd */
ValNodePtr  LIBCALL SHoundGiFromCDDIDList(ValNodePtr vnpcdd)
{
  CharPtr fpath = NULL;
  CharPtr pstr = NULL;
  ValNodePtr pvntemp = NULL, pvnshort = NULL, pvnfin = NULL, pvnid = NULL;
  Int4 counter = 0;

  if(vnpcdd == NULL)
  {
    ErrPostEx(SEV_ERROR,0,0, "Invalid parameter.");
    return NULL;
  }

  pvntemp = vnpcdd;
  while (pvntemp != NULL)
  {
     counter++;
     pvnshort = Slri_ValNodeCopyStr(&pvnshort, 0, pvntemp->data.ptrvalue);
     if(counter == MAXQ || pvntemp->next == NULL)
     {
        pstr = Misc_SaveStrValNodeListAsString(pvnshort);
        if(pstr == NULL)
        {
           ErrPostEx(SEV_ERROR,0,0, "Misc_SaveValNodeListAsString returned NULL.");
           pvnshort = ValNodeFreeData(pvnshort);
           return NULL;
        }
        fpath = MemNew((size_t)(StringLen(pstr)+PATH_MAX+100)*sizeof(Char));
        if(fpath == NULL)
        {
           ErrPostEx(SEV_ERROR,0,0, "MemNew returned NULL.");
           pstr = MemFree(pstr);
           pvnshort = ValNodeFreeData(pvnshort);
           return NULL;
        }
        sprintf(fpath,"%s?fnct=SeqHoundGiFromCDDIDList&vnpcdd=%s", IsNullStr(slri_cgipath), IsNullStr(pstr));
        ErrPostEx(SEV_INFO,0,0, "SeqHoundGiFromCDDIDList request: %s.\n", IsNullStr(fpath));
        pstr = MemFree(pstr);
        if(SHoundWWWGetfile(slri_sername, fpath) == 0)
        {
           ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
           fpath = MemFree(fpath);
           pvnshort = ValNodeFreeData(pvnshort);
           return NULL;
        }
        
        fpath = MemFree(fpath);
        pvnid = ReplyBSGetValNode(TRUE);
        ValNodeLink(&pvnfin, pvnid);
        counter = 0;
        pvnshort = ValNodeFree(pvnshort);
     }
     pvntemp = pvntemp->next;
  }
  Misc_IntVnpUnique(&pvnfin);
  return pvnfin;
}


/* returns all the cddid of Gi */
ValNodePtr  LIBCALL SHoundCDDIDFromGi(Int4 Gi)
{
  Char fpath[PATH_MAX];
  ValNodePtr vnpcddid = NULL;
  Int2 ercode = 0;

  if(Gi <= 0) {
    ErrPostEx(SEV_ERROR,0,0, "Invalid parameter.");
    return NULL;
  }
  sprintf(fpath,"%s?fnct=SeqHoundCDDIDFromGi&gi=%ld", IsNullStr(slri_cgipath), (long) Gi);
  ErrPostEx(SEV_INFO,0,0, "SeqHoundCDDIDFromGi request: %s.\n", IsNullStr(fpath));
  if((ercode = SHoundWWWGetfile(slri_sername, fpath)) == 0) {
    ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
    return NULL;
  } else if (ercode == 2) {
    return NULL;
  }
  vnpcddid = ReplyBSGetValNode(FALSE);
  return vnpcddid;
}


/* returns the CDDID for all instances of vnpgi */
ValNodePtr  LIBCALL SHoundCDDIDFromGiList(ValNodePtr vnpgi)
{
  CharPtr fpath = NULL;
  CharPtr pstr = NULL;
  ValNodePtr pvntemp = NULL, pvnshort = NULL, pvnfin = NULL, pvnid = NULL;
  Int4 counter = 0;

  if(vnpgi == NULL)
  {
    ErrPostEx(SEV_ERROR,0,0, "Invalid parameter.");
    return NULL;
  }

  pvntemp = vnpgi;
  while (pvntemp != NULL)
  {
     counter++;
     pvnshort = Slri_ValNodeAddInt(&pvnshort, 0, pvntemp->data.intvalue);
     if(counter == MAXQ || pvntemp->next == NULL)
     {
        pstr = Misc_SaveValNodeListAsString(pvnshort);
        if(pstr == NULL)
        {
           ErrPostEx(SEV_ERROR,0,0, "Misc_SaveValNodeListAsString returned NULL.");
           pvnshort = ValNodeFree(pvnshort);
           return NULL;
        }
        fpath = MemNew((size_t)(StringLen(pstr)+PATH_MAX+100)*sizeof(Char));
        if(fpath == NULL)
        {
           ErrPostEx(SEV_ERROR,0,0, "MemNew returned NULL.");
           pstr = MemFree(pstr);
           pvnshort = ValNodeFree(pvnshort);
           return NULL;
        }
        sprintf(fpath,"%s?fnct=SeqHoundCDDIDFromGiList&vnpgi=%s", IsNullStr(slri_cgipath), IsNullStr(pstr));
        ErrPostEx(SEV_INFO,0,0, "SeqHoundCDDIDFromGiList request: %s.\n", IsNullStr(fpath));
        pstr = MemFree(pstr);
        if(SHoundWWWGetfile(slri_sername, fpath) == 0)
        {
           ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
           fpath = MemFree(fpath);
           pvnshort = ValNodeFree(pvnshort);
           return NULL;
        }

        fpath = MemFree(fpath);
        pvnid = ReplyBSGetValNode(FALSE);
        ValNodeLink(&pvnfin, pvnid);
        counter = 0;
        pvnshort = ValNodeFree(pvnshort);
     }
     pvntemp = pvntemp->next;
  }
  Misc_IntVnpUnique(&pvnfin);
  return pvnfin;
}

/* returns the CDD Score of the instance with Gi and cddid */
FloatHi  LIBCALL SHoundCDDScoreFromGi(Int4 Gi, CharPtr cddid)
{
  Char fpath[PATH_MAX];
  FloatHi score = 0;
  Int2 ercode = 0;

  if(Gi <= 0) {
    ErrPostEx(SEV_ERROR,0,0, "Invalid parameter.");
    return 0;
  }
  if(cddid == NULL){
    ErrPostEx(SEV_ERROR,0,0, "Invalid parameter.");
    return 0;
  }
  sprintf(fpath,"%s?fnct=SeqHoundCDDScoreFromGi&gi=%ld&cddid=%s", IsNullStr(slri_cgipath), (long) Gi, IsNullStr(cddid));
  ErrPostEx(SEV_INFO,0,0, "SeqHoundCDDScoreFromGi request: %s.\n", IsNullStr(fpath));
  if((ercode = SHoundWWWGetfile(slri_sername, fpath)) == 0) {
    ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
    return 0;
  } else if (ercode == 2) {
    return -1;
  }
  score = ReplyBSGetFloat();
  return score;
}

/* returns Gi of llid */
ValNodePtr  LIBCALL SHoundGiFromLLID(Int4 llid)
{
  Char fpath[PATH_MAX];
  ValNodePtr vnpgi = NULL;
  Int2 ercode = 0;

  if(llid <= 0) {
    ErrPostEx(SEV_ERROR,0,0, "Invalid parameter.");
    return NULL;
  }
  sprintf(fpath,"%s?fnct=SeqHoundGiFromLLID&llid=%ld", IsNullStr(slri_cgipath), (long) llid);
  ErrPostEx(SEV_INFO,0,0, "SeqHoundGiFromLLID request: %s.\n", IsNullStr(fpath));
  if((ercode = SHoundWWWGetfile(slri_sername, fpath)) == 0) {
    ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
    return NULL;
  } else if (ercode == 2) {
    return NULL;
  }
  vnpgi = ReplyBSGetValNode(TRUE);
  return vnpgi;
}

/* returns the Gi of all the instances of vnpllid */
ValNodePtr  LIBCALL SHoundGiFromLLIDList(ValNodePtr vnpllid)
{
  CharPtr fpath = NULL;
  CharPtr pstr = NULL;
  ValNodePtr pvntemp = NULL, pvnshort = NULL, pvnfin = NULL, pvnid = NULL;
  Int4 counter = 0;

  if(vnpllid == NULL)
  {
    ErrPostEx(SEV_ERROR,0,0, "Invalid parameter.");
    return NULL;
  }

  pvntemp = vnpllid;
  while (pvntemp != NULL)
  {
     counter++;
     pvnshort = Slri_ValNodeAddInt(&pvnshort, 0, pvntemp->data.intvalue);
     if(counter == MAXQ || pvntemp->next == NULL)
     {
        pstr = Misc_SaveValNodeListAsString(pvnshort);
        if(pstr == NULL)
        {
           ErrPostEx(SEV_ERROR,0,0, "Misc_SaveValNodeListAsString returned NULL.");
           pvnshort = ValNodeFree(pvnshort);
           return NULL;
        }
        fpath = MemNew((size_t)(StringLen(pstr)+PATH_MAX+100)*sizeof(Char));
        if(fpath == NULL)
        {
           ErrPostEx(SEV_ERROR,0,0, "MemNew returned NULL.");
           pstr = MemFree(pstr);
           pvnshort = ValNodeFree(pvnshort);
           return NULL;
        }
        sprintf(fpath,"%s?fnct=SeqHoundGiFromLLIDList&vnpll=%s", IsNullStr(slri_cgipath), IsNullStr(pstr));
        ErrPostEx(SEV_INFO,0,0, "SeqHoundGiFromLLIDList request: %s.\n", IsNullStr(fpath));
        pstr = MemFree(pstr);
        if(SHoundWWWGetfile(slri_sername, fpath) == 0)
        {
           ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
           fpath = MemFree(fpath);
           pvnshort = ValNodeFree(pvnshort);
           return NULL;
        }
       
        fpath = MemFree(fpath);
        pvnid = ReplyBSGetValNode(TRUE);
        ValNodeLink(&pvnfin, pvnid);
        counter = 0;
        pvnshort = ValNodeFree(pvnshort);
     }
     pvntemp = pvntemp->next;
  }
  Misc_IntVnpUnique(&pvnfin);
  return pvnfin;
}

/* returns the llid of Gi */
Int4  LIBCALL SHoundLLIDFromGi(Int4 Gi)
{
  Char fpath[PATH_MAX];
  Int4 llid = 0;
  Int2 ercode = 0;

  if(Gi <= 0) {
    ErrPostEx(SEV_ERROR,0,0, "Invalid parameter.");
    return 0;
  }
  sprintf(fpath,"%s?fnct=SeqHoundLLIDFromGi&gi=%ld", IsNullStr(slri_cgipath), (long) Gi);
  ErrPostEx(SEV_INFO,0,0, "SeqHoundLLIDFromGi request: %s.\n", IsNullStr(fpath));
  if((ercode = SHoundWWWGetfile(slri_sername, fpath)) == 0) {
    ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
    return 0;
  } else if (ercode == 2) {
    return -1;
  }
  llid = ReplyBSGetInteger();
  return llid;
}

/* returns the llid for all the instances of vnpgi */
ValNodePtr  LIBCALL SHoundLLIDFromGiList(ValNodePtr vnpgi)
{
  CharPtr fpath = NULL;
  CharPtr pstr = NULL;
  ValNodePtr pvntemp = NULL, pvnshort = NULL, pvnfin = NULL, pvnid = NULL;
  Int4 counter = 0;

  if(vnpgi == NULL)
  {
    ErrPostEx(SEV_ERROR,0,0, "Invalid parameter.");
    return NULL;
  }

  pvntemp = vnpgi;
  while (pvntemp != NULL)
  {
     counter++;
     pvnshort = Slri_ValNodeAddInt(&pvnshort, 0, pvntemp->data.intvalue);
     if(counter == MAXQ || pvntemp->next == NULL)
     {
        pstr = Misc_SaveValNodeListAsString(pvnshort);
        if(pstr == NULL)
        {
           ErrPostEx(SEV_ERROR,0,0, "Misc_SaveValNodeListAsString returned NULL.");
           pvnshort = ValNodeFree(pvnshort);
           return NULL;
        }
        fpath = MemNew((size_t)(StringLen(pstr)+PATH_MAX+100)*sizeof(Char));
        if(fpath == NULL)
        {
           ErrPostEx(SEV_ERROR,0,0, "MemNew returned NULL.");
           pstr = MemFree(pstr);
           pvnshort = ValNodeFree(pvnshort);
           return NULL;
        }
        sprintf(fpath,"%s?fnct=SeqHoundLLIDFromGiList&vnpgi=%s", IsNullStr(slri_cgipath), IsNullStr(pstr));
        ErrPostEx(SEV_INFO,0,0, "SeqHoundLLIDFromGiList request: %s.\n", IsNullStr(fpath));
        pstr = MemFree(pstr);
        if(SHoundWWWGetfile(slri_sername, fpath) == 0)
        {
           ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
           fpath = MemFree(fpath);
           pvnshort = ValNodeFree(pvnshort);
           return NULL;
        }
        fpath = MemFree(fpath);
        pvnid = ReplyBSGetValNode(TRUE);
        ValNodeLink(&pvnfin, pvnid);
        counter = 0;
        pvnshort = ValNodeFree(pvnshort);
     }
     pvntemp = pvntemp->next;
  }
  Misc_IntVnpUnique(&pvnfin);
  return pvnfin;
}


/* returns the locus of Gi */
CharPtr  LIBCALL SHoundLocusFromGi(Int4 Gi)
{
  Char fpath[PATH_MAX];
  CharPtr locus = NULL;
  Int2 ercode = 0;


  if(Gi <= 0) {
    ErrPostEx(SEV_ERROR,0,0, "Invalid parameter.");
    return NULL;
  }
  sprintf(fpath,"%s?fnct=SeqHoundLocusFromGi&gi=%ld", IsNullStr(slri_cgipath), (long) Gi);
  ErrPostEx(SEV_INFO,0,0, "SeqHoundLocusFromGi request: %s.\n", IsNullStr(fpath));
  if((ercode = SHoundWWWGetfile(slri_sername, fpath)) == 0) {
    ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
    return NULL;
  }  else if (ercode == 2) {
    return NULL;
  }
  locus = ReplyBSGetString();
  return locus;
}

/******************************************************
       The Gene Ontology Remote Functions
*******************************************************/


/* returns the goid of all genes with reference source refType.
   eg. refType could be "EC" for enzyme commission. */
ValNodePtr  LIBCALL SHoundGODBGetRecordByReference(CharPtr refType)
{
  Char fpath[PATH_MAX];
  ValNodePtr vnpgoid = NULL;
  Int2 ercode = 0;

  if(refType == NULL) {
    ErrPostEx(SEV_ERROR,0,0, "Invalid parameter.");
    return NULL;
  }
  sprintf(fpath,"%s?fnct=SeqHoundGODBGetRecordByReference&refType=%s", IsNullStr(slri_cgipath), IsNullStr(refType));
  ErrPostEx(SEV_INFO,0,0, "SeqHoundGODBGetRecordByReference request: %s.\n", IsNullStr(fpath));
  if((ercode = SHoundWWWGetfile(slri_sername, fpath)) == 0) {
    ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
    return NULL;
  } else if (ercode == 2) {
    return NULL;
  }
  vnpgoid = ReplyBSGetValNode(TRUE);
  return vnpgoid;
}

/* returns the children of goid */
ValNodePtr  LIBCALL SHoundGODBGetChildrenOf(Int4 goid)
{
  Char fpath[PATH_MAX];
  ValNodePtr vnpgoid = NULL;
  Int2 ercode = 0;

  if(goid <= 0) {
    ErrPostEx(SEV_ERROR,0,0, "Invalid parameter.");
    return NULL;
  }
  sprintf(fpath,"%s?fnct=SeqHoundGODBGetChildrenOf&goid=%ld", IsNullStr(slri_cgipath), (long) goid);
  ErrPostEx(SEV_INFO,0,0, "SeqHoundGODBGetChildrenOf request: %s.\n", IsNullStr(fpath));
  if((ercode = SHoundWWWGetfile(slri_sername, fpath)) == 0) {
    ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
    return NULL;
  } else if (ercode == 2) {
    return NULL;
  }
  vnpgoid = ReplyBSGetValNode(TRUE);
  return vnpgoid;
}

/* returns the immediate parents of goid */
ValNodePtr  LIBCALL SHoundGODBGetParentOf(Int4 goid)
{
  Char fpath[PATH_MAX];
  ValNodePtr vnpgoid = NULL;
  Int2 ercode = 0;

  
  if(goid <= 0) {
    ErrPostEx(SEV_ERROR,0,0, "Invalid parameter.");
    return NULL;
  }
  sprintf(fpath,"%s?fnct=SeqHoundGODBGetParentOf&goid=%ld", IsNullStr(slri_cgipath), (long) goid);
  ErrPostEx(SEV_INFO,0,0, "SeqHoundGODBGetParentOf request: %s.\n", IsNullStr(fpath));
  if((ercode = SHoundWWWGetfile(slri_sername, fpath)) == 0) {
    ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
    return NULL;
  } else if (ercode == 2) {
    return NULL;
  }
  vnpgoid = ReplyBSGetValNode(TRUE);
  return vnpgoid;
}

/* returns all the ancestors of goid, including its parent, 
   grandparents, great grandparents etc... */
ValNodePtr  LIBCALL SHoundGODBGetAllAncestors(Int4 goid)
{
  Char fpath[PATH_MAX];
  ValNodePtr vnpgoid = NULL;
  Int2 ercode = 0;

  
  if(goid <= 0) {
    ErrPostEx(SEV_ERROR,0,0, "Invalid parameter.");
    return NULL;
  }
  sprintf(fpath,"%s?fnct=SeqHoundGODBGetAllAncestors&goid=%ld", IsNullStr(slri_cgipath), (long) goid);
  ErrPostEx(SEV_INFO,0,0, "SeqHoundGODBGetAllAncestors request: %s.\n", IsNullStr(fpath));
  if((ercode = SHoundWWWGetfile(slri_sername, fpath)) == 0) {
    ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
    return NULL;
  }  else if (ercode == 2) {
    return NULL;
  }
  vnpgoid = ReplyBSGetValNode(TRUE);
  return vnpgoid;
}

/* returns all children of goid, and the children of the children of ...etc */
ValNodePtr LIBCALL SHoundGODBGetAllChildren(Int4 goid)
{
  Char fpath[PATH_MAX];
  ValNodePtr vnpgoid = NULL;
  Int2 ercode = 0;

  
  if(goid <= 0) {
    ErrPostEx(SEV_ERROR,0,0, "Invalid parameter.");
    return NULL;
  }
  sprintf(fpath,"%s?fnct=SeqHoundGODBGetAllChildren&goid=%ld", IsNullStr(slri_cgipath), (long) goid);
  ErrPostEx(SEV_INFO,0,0, "SeqHoundGODBGetAllChildren request: %s.\n", IsNullStr(fpath));
  if((ercode = SHoundWWWGetfile(slri_sername, fpath)) == 0) {
    ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
    return NULL;
  } else if (ercode == 2) {
    return NULL;
  }
  vnpgoid = ReplyBSGetValNode(TRUE);
  return vnpgoid;
}

/* returns the full name of goid. Returns null if goid is not found. */
CharPtr LIBCALL SHoundGODBGetNameByID(Int4 goid)
{
  Char fpath[PATH_MAX];
  CharPtr cpname = NULL;
  Int2 ercode = 0;

  if(goid <= 0) {
    ErrPostEx(SEV_ERROR,0,0, "Invalid parameter.");
    return NULL;
  }
  sprintf(fpath,"%s?fnct=SeqHoundGODBGetNameByID&goid=%ld", IsNullStr(slri_cgipath), (long) goid);
  ErrPostEx(SEV_INFO,0,0, "SeqHoundGODBGetNameByID request: %s.\n", IsNullStr(fpath));
  if((ercode = SHoundWWWGetfile(slri_sername, fpath)) == 0) {
    ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
    return NULL;
  } else if (ercode == 2) {
    return NULL;
  }
  cpname = ReplyBSGetString();
  return cpname;
}

/* returns the file from which goid was parsed from. goid could have
   been parsed from 1 of 3 files, molecular function, biological process or
   cellular component. */
CharPtr LIBCALL SHoundGODBGetClassification(Int4 goid)
{
  Char fpath[PATH_MAX];
  CharPtr class = NULL;
  Int2 ercode = 0;

  if(goid <= 0) {
    ErrPostEx(SEV_ERROR,0,0, "Invalid parameter.");
    return NULL;
  }
  sprintf(fpath,"%s?fnct=SeqHoundGODBGetClassification&goid=%ld", IsNullStr(slri_cgipath), (long) goid);
  ErrPostEx(SEV_INFO,0,0, "SeqHoundGODBGetClassification request: %s.\n", IsNullStr(fpath));
  if((ercode = SHoundWWWGetfile(slri_sername, fpath)) == 0) {
    ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
    return NULL;
  } else if (ercode == 2) {
    return NULL;
  }
  class = ReplyBSGetString();
  return class;
}

/* returns the hierarchy of goid */
/* function disabled since it has a wrong logic
Int4 LIBCALL SHoundGODBGetHierarchyLevel(Int4 goid)
{
  Char fpath[PATH_MAX];
  Int4 iHLevel = 0;
  Int2 ercode = 0;

  if(goid <= 0) {
    ErrPostEx(SEV_ERROR,0,0, "Invalid parameter.");
    return -1;
  }
  sprintf(fpath,"%s?fnct=SeqHoundGODBGetHierarchyLevel&goid=%ld", IsNullStr(slri_cgipath), (long) goid);
  ErrPostEx(SEV_INFO,0,0, "SeqHoundGODBGetHierarchyLevel request: %s.\n", IsNullStr(fpath));
  if((ercode = SHoundWWWGetfile(slri_sername, fpath)) == 0) {
    ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
    return -1;
  }  else if (ercode == 2) {
    return -1;
  }
  iHLevel = ReplyBSGetInteger();
  return iHLevel;
}*/


/* returns an CLS with num genomes, uids field with taxids, and weights field with the scientific names */
SLRI_ERR LIBCALL SHoundAllGenomesWithScientificNames (CLinkSetPtr PNTR pAllGenomes)
{
	Char fpath[PATH_MAX];
	Int2 ercode = 0;
	AsnIoBSPtr aipbs = NULL;

	if(pAllGenomes == NULL) {
		ErrPostEx(SEV_ERROR, 0, 0, "SHoundAllGenomesWithScientificNames: Address of passed clsp is NULL (you must pass a variable by reference).");
		return SLRI_FAIL;
	}

	sprintf(fpath,"%s?fnct=SeqHoundAllGenomesWithScientificNames", IsNullStr(slri_cgipath));
	ErrPostEx(SEV_INFO,2,1,"SeqHoundAllGenomesWithScientificNames request: %s\n", IsNullStr(fpath));
	
	if((ercode = SHoundWWWGetfile(slri_sername, fpath)) == 0) {
		ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
		return SLRI_FAIL;
	} else if (ercode == 2) {
		return SLRI_FAIL;
	}

	if ((aipbs = AsnIoBSOpen(FILE_MODE, replyBS)) == NULL) {
		ErrPostEx(SEV_ERROR,2,1,"AsnIoBSOpen returned NULL.");
		return SLRI_FAIL;
	}
	if ((*pAllGenomes = CLinkSetAsnRead(aipbs->aip, NULL)) == NULL) {
		ErrPostEx(SEV_ERROR,2,1,"CLinkSetAsnRead returned NULL.");
		AsnIoBSClose(aipbs);
		BSFree(replyBS);
		return SLRI_FAIL;
	}
	AsnIoBSClose(aipbs);
	BSFree(replyBS);
	replyBS = NULL;
	return SLRI_SUCCESS;
}


/* returns an ILS with num genomes, uids field with taxids, and weights field with the number of proteins */
SLRI_ERR LIBCALL SHoundAllGenomesWithNumProteins (ILinkSetPtr PNTR pAllGenomes)
{
	Char fpath[PATH_MAX];
	Int2 ercode = 0;
	AsnIoBSPtr aipbs = NULL;

	if(pAllGenomes == NULL) {
		ErrPostEx(SEV_ERROR, 0, 0, "SHoundAllGenomesWithNumProteins: Address of passed ilsp is NULL (you must pass a variable by reference).");
		return SLRI_FAIL;
	}

	sprintf(fpath,"%s?fnct=SeqHoundAllGenomesWithNumProteins", IsNullStr(slri_cgipath));
	ErrPostEx(SEV_INFO,2,1,"SeqHoundAllGenomesWithNumProteins request: %s\n", IsNullStr(fpath));
	
	if((ercode = SHoundWWWGetfile(slri_sername, fpath)) == 0) {
		ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
		return SLRI_FAIL;
	} else if (ercode == 2) {
		return SLRI_FAIL;
	}

	if ((aipbs = AsnIoBSOpen(FILE_MODE, replyBS)) == NULL) {
		ErrPostEx(SEV_ERROR,2,1,"AsnIoBSOpen returned NULL.");
		return SLRI_FAIL;
	}
	if ((*pAllGenomes = ILinkSetAsnRead(aipbs->aip, NULL)) == NULL) {
		ErrPostEx(SEV_ERROR,2,1,"ILinkSetAsnRead returned NULL.");
		AsnIoBSClose(aipbs);
		BSFree(replyBS);
		return SLRI_FAIL;
	}
	AsnIoBSClose(aipbs);
	BSFree(replyBS);
	replyBS = NULL;
	return SLRI_SUCCESS;
}

/**********************
       RPS Functions
***********************/

ValNodePtr  LIBCALL SHoundGetGisByNumberOfDomains(Int4 numdom)
{
  Char fpath[PATH_MAX];
  ValNodePtr pvnmuid = NULL;
  
  
  if(numdom <= 0)
    {
      ErrPostEx(SEV_ERROR,0,0, "Invalid parameter.");
      return NULL;
    }
  sprintf(fpath,"%s?fnct=SeqHoundGetGisByNumberOfDomains&numdom=%d", IsNullStr(slri_cgipath), (Int4) numdom);
  ErrPostEx(SEV_INFO,0,0, "SeqHoundGetGisByNumberOfDomains request: %s.\n", IsNullStr(fpath));
  if(SHoundWWWGetfile(slri_sername, fpath) == 0)
    {
      ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
      return NULL;
    }
  pvnmuid = ReplyBSGetValNode(TRUE);
  return pvnmuid;
}

/* returns the Accession of all with label */
ValNodePtr  LIBCALL SHoundGetDomainIdFromLabel(CharPtr label)
{
  Char fpath[PATH_MAX];
  ValNodePtr vnpacc = NULL;
  Int2 ercode = 0;

  if(label == NULL) {
    ErrPostEx(SEV_ERROR,0,0, "Invalid parameter.");
    return NULL;
  }

  sprintf(fpath,"%s?fnct=SeqHoundGetDomainIdFromLabel&label=%s", IsNullStr(slri_cgipath), IsNullStr(label));
  ErrPostEx(SEV_INFO,0,0, "SeqHoundGetDomainIdFromLabel request: %s.\n", IsNullStr(fpath));
  if((ercode = SHoundWWWGetfile(slri_sername, fpath)) == 0) {
    ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
    return NULL;
  }  else if (ercode == 2) {
    return NULL;
  }
  vnpacc = ReplyBSGetValNode(FALSE);
  return vnpacc;
}


/* returns the Accession of all with label */
ValNodePtr  LIBCALL SHoundGetGisByDomainId(CharPtr domain_id)
{
  Char fpath[PATH_MAX];
  ValNodePtr vnpgi = NULL;
  Int2 ercode = 0;

  if(domain_id == NULL) {
    ErrPostEx(SEV_ERROR,0,0, "Invalid parameter.");
    return NULL;
  }

  sprintf(fpath,"%s?fnct=SeqHoundGetGisByDomainId&domain_id=%s", IsNullStr(slri_cgipath), IsNullStr(domain_id));
  ErrPostEx(SEV_INFO,0,0, "SeqHoundGisByDomainId request: %s.\n", IsNullStr(fpath));
  if((ercode = SHoundWWWGetfile(slri_sername, fpath)) == 0) {
    ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
    return NULL;
  }  else if (ercode == 2) {
    return NULL;
  }
  vnpgi = ReplyBSGetValNode(TRUE);
  return vnpgi;
}

/* returns the full name of goid. Returns null if goid is not found. */
CharPtr LIBCALL SHoundGetDomainLabelFromDomainId(CharPtr accession)
{
  Char fpath[PATH_MAX];
  CharPtr cpname = NULL;
  Int2 ercode = 0;

  if(accession == NULL) {
    ErrPostEx(SEV_ERROR,0,0, "Invalid parameter.");
    return NULL;
  }

  sprintf(fpath,"%s?fnct=SeqHoundGetDomainLabelFromDomainId&accession=%s", IsNullStr(slri_cgipath), IsNullStr(accession));
  ErrPostEx(SEV_INFO,0,0, "SeqHoundGetDomainLabelFromDomainId request: %s.\n", IsNullStr(fpath));
  if((ercode = SHoundWWWGetfile(slri_sername, fpath)) == 0) {
    ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
    return NULL;
  } else if (ercode == 2) {
    return NULL;
  }
  cpname = ReplyBSGetString();
  return cpname;
}

ValNodePtr  LIBCALL SHoundGetGisByDomainIdAndEvalue(CharPtr domain_id, FloatHi cutoff)
{
  Char fpath[PATH_MAX];
  ValNodePtr pvngi = NULL;

  if(domain_id == NULL)
    {
      ErrPostEx(SEV_ERROR,0,0, "Invalid parameter.");
      return NULL;
    }

  sprintf(fpath,"%s?fnct=SeqHoundGetGisByDomainIdAndEvalue&domain=%s&evalue=%g", IsNullStr(slri_cgipath), IsNullStr(domain_id),(double)cutoff);
  ErrPostEx(SEV_INFO,0,0, "SeqHoundGetGisByDomainIdAndEvalue request: %s.\n", IsNullStr(fpath));
  if(SHoundWWWGetfile(slri_sername, fpath) == 0)
    {
      ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
      return NULL;
    }
  pvngi = ReplyBSGetValNode(TRUE);
  return pvngi;
}



StRpsdbPtr LIBCALL SHoundGetDomainsFromGi(Int4 gi)
{

  StRpsdbPtr pbs = NULL;
  Char fpath[PATH_MAX];
  Int2 ercode = 0;
  AsnIoBSPtr aipbs = NULL;

  if (gi <= 0)
    {
      ErrPostEx(SEV_ERROR,0,0, "Invalid GI.");
      return NULL;
    }

  sprintf(fpath,"%s?fnct=SeqHoundGetDomainsFromGi&gi=%ld", IsNullStr(slri_cgipath),(long) gi);
  ErrPostEx(SEV_INFO,2,1,"SeqHoundGetDomainsFromGi request: %s\n", IsNullStr(fpath));

  if((ercode = SHoundWWWGetfile(slri_sername, fpath)) == 0)
    {
      ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
      return NULL;
    }
  else if (ercode == 2)
    return NULL;
  aipbs = AsnIoBSOpen(FILE_MODE, replyBS);
  if (aipbs == NULL)
    {
      ErrPostEx(SEV_ERROR,2,1,"AsnIoBSOpen returned NULL.");
      return NULL;
    }
  pbs = StRpsdbAsnRead(aipbs->aip, NULL);
  if (pbs == NULL)
    {
      ErrPostEx(SEV_ERROR,2,1,"StPpsdbAsnRead returned NULL.");
      AsnIoBSClose(aipbs);
      BSFree(replyBS);
      return NULL;
    }
  AsnIoBSClose(aipbs);
  BSFree(replyBS);
  return pbs;

}

StRpsdbPtr LIBCALL SHoundGetDomainsFromGiWithEvalue(Int4 gi, FloatHi cutoff)
{

  StRpsdbPtr pbs = NULL;
  Char fpath[PATH_MAX];
  Int2 ercode = 0;
  AsnIoBSPtr aipbs = NULL;

  if (gi <= 0)
    {
      ErrPostEx(SEV_ERROR,0,0, "Invalid GI.");
      return NULL;
    }

  sprintf(fpath,"%s?fnct=SeqHoundGetDomainsFromGiWithEvalue&gi=%ld&evalue=%g", IsNullStr(slri_cgipath),(long) gi, (double) cutoff);
  ErrPostEx(SEV_INFO,2,1,"SeqHoundGetDomainsFromGiWithEvalue request: %s\n", IsNullStr(fpath));

  if((ercode = SHoundWWWGetfile(slri_sername, fpath)) == 0)
    {
      ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
      return NULL;
    }
  else if (ercode == 2)
    return NULL;
  aipbs = AsnIoBSOpen(FILE_MODE, replyBS);
  if (aipbs == NULL)
    {
      ErrPostEx(SEV_ERROR,2,1,"AsnIoBSOpen returned NULL.");
      return NULL;
    }
  pbs = StRpsdbAsnRead(aipbs->aip, NULL);
  if (pbs == NULL)
    {
      ErrPostEx(SEV_ERROR,2,1,"StPpsdbAsnRead returned NULL.");
      AsnIoBSClose(aipbs);
      BSFree(replyBS);
      return NULL;
    }
  AsnIoBSClose(aipbs);
  BSFree(replyBS);
  return pbs;

}

/* Get the list of domains from a list of proteins*/
StRpsdbPtr LIBCALL SHoundGetDomainsFromGiListWithEvalue(ValNodePtr pvnGis, FloatHi cutoff)
{
	StRpsdbPtr rpsdb_list= NULL, rpsdb_head=NULL, temp=NULL;
	CharPtr fpath=NULL;
	Int2 ercode = 0;
	CharPtr pvalstr = NULL;
	ValNodePtr pvnTemp=NULL, pvnshort = NULL;
	AsnIoBSPtr aipbs = NULL;
	Int4 counter = 0;

	if (pvnGis == NULL)
	{
		ErrPostEx(SEV_ERROR,0,0, "Invalid GI.");
		return NULL;
	}
	pvnTemp = pvnGis;
	while (pvnTemp != NULL)
	{
		counter++;
		pvnshort = Slri_ValNodeAddInt(&pvnshort, 0, pvnTemp->data.intvalue);
		if(counter ==MAXQ || pvnTemp->next ==NULL)
		{
			pvalstr = Misc_SaveValNodeListAsString(pvnshort);
			if(pvalstr == NULL){
				 ErrPostEx(SEV_ERROR,0,0, "Misc_SaveValNodeListAsString returned NULL.");
				 pvnshort = ValNodeFree(pvnshort);
				 return NULL;
			}
			fpath = MemNew((size_t)(StringLen(pvalstr)+PATH_MAX+100)*sizeof(Char));
			if(fpath==NULL)
			{
				ErrPostEx(SEV_ERROR,0,0, "MemNew returned NULL.");
				pvalstr = MemFree(pvalstr);
				pvnshort = ValNodeFree(pvnshort);
				return NULL;
			}
			sprintf(fpath,"%s?fnct=SeqHoundGetDomainsFromGiListWithEvalue&pgi=%s&evalue=%g", IsNullStr(slri_cgipath), IsNullStr(pvalstr),(double) cutoff);
			ErrPostEx(SEV_INFO,2,1,"SeqHoundGetDomainsFromGiListWithEvalue request: %s\n", IsNullStr(fpath));
			pvalstr = MemFree(pvalstr);
			if((ercode = SHoundWWWGetfile(slri_sername, fpath)) == 0)
			{
				ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
				fpath = MemFree(fpath);
				pvnshort = ValNodeFree(pvnshort);
				return NULL;
			}
			else if (ercode == 2)
			{
				fpath = MemFree(fpath);
				pvnshort = ValNodeFree(pvnshort);
				return NULL;
			}
			fpath = MemFree(fpath);
			aipbs = AsnIoBSOpen(FILE_MODE, replyBS);
			if (aipbs == NULL)
			{
				ErrPostEx(SEV_ERROR,2,1,"AsnIoBSOpen returned NULL.");
				pvnshort = ValNodeFree(pvnshort);
				return NULL;
			}
			rpsdb_list = StRpsdbAsnRead(aipbs->aip, NULL);
			if (rpsdb_list == NULL)
			{
				ErrPostEx(SEV_ERROR,2,1," StRpsdbAsnRead returned NULL.");
				AsnIoBSClose(aipbs);
				BSFree(replyBS);
				pvnshort = ValNodeFree(pvnshort);
				return NULL;
			}
			AsnIoBSClose(aipbs);
			BSFree(replyBS);
			pvnshort = ValNodeFree(pvnshort);

			/*Link to the gorwing list*/
			if(rpsdb_head==NULL){
				rpsdb_head=rpsdb_list;
			}else{
				temp=rpsdb_head;
				while(temp->next!=NULL)
					temp=temp->next;
				temp->next=rpsdb_list;
			}
			counter = 0;
		}
		pvnTemp=pvnTemp->next;
	}
	/*return */
	return rpsdb_head;
}

/**
 * The function, given a gi, will return molecule name -
 * use it if SeqHoundAccFromGi fails
 **
 */
CharPtr	LIBCALL SHoundGetNameByGi(Int4 gi)
{
	Char fpath[PATH_MAX];
	CharPtr pname = 0;

	if(gi<=0)
	{
		ErrPostEx(SEV_ERROR,0,0, "Invalid parameter.");
		return NULL;
	}

	sprintf(fpath,"%s?fnct=SeqHoundGetNameByGi&gi=%ld", IsNullStr(slri_cgipath), (long)gi);
	ErrPostEx(SEV_INFO,0,0, "SeqHoundGetNameByGi request: %s.\n", IsNullStr(fpath));
	if(SHoundWWWGetfile(slri_sername, fpath) == 0)
	{
		ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
		return NULL;
	}

	if (!(pname = ReplyBSGetString()))
	{
		ErrPostEx(SEV_INFO,0,0, "SeqHoundGetNameByGi returned NULL.");
		return NULL;
	}
	return pname;
}



/**************************************************************
 *
 *  DBXref Module
 *
 *************************************************************/


SHDBXrefPtr LIBCALL SHoundDBXREFGetDBXrefListBySourceRecord(CharPtr dbname, CharPtr recordid, CharPtr field, Int4 cv)
{
    Char fpath[PATH_MAX];
    SHDBXrefPtr pSHDBXref = NULL;
    SHDBXrefPtr auxpSHDBXref = NULL;

    ValNodePtr vnp = NULL;
    ValNodePtr auxvnp = NULL;
    Int2 ercode = 0;

    auxvnp = vnp;

    if ( (dbname == NULL) || (recordid == NULL) )
    {
	ErrPostEx(SEV_ERROR,0,0, "Invalid parameter.");
	return 0;
    }

    if (field == NULL)
    {
	sprintf(fpath,"%s?fnct=SeqHoundDBXREFGetDBXrefListBySourceRecord&dbname=%s&recordid=%s&field=NULL&cv=%d", slri_cgipath, dbname, recordid, cv);
    }
    else
    {
	sprintf(fpath,"%s?fnct=SeqHoundDBXREFGetDBXrefListBySourceRecord&dbname=%s&recordid=%s&field=%s&cv=%d", slri_cgipath, dbname, recordid, field, cv);
    }

    ErrPostEx(SEV_INFO,0,0, "SeqHoundDBXREFGetDBXrefListBySourceRecord request: %s.\n", fpath);
    if((ercode = SHoundWWWGetfile(slri_sername, fpath)) == 0) 
    {
	ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
	return NULL;
    } 
    else if (ercode == 2)
    {
	return NULL;
    }

    if ( (vnp = ReplyBSGetValNodeOrdered(FALSE)) == NULL)
    {
	return NULL;
    }
    pSHDBXref = SHDBXrefNew();
    auxpSHDBXref = pSHDBXref;

    while(vnp != NULL)
    {   
	auxpSHDBXref->sourcedb = StringSave((char*) vnp->data.ptrvalue);
	vnp = vnp->next;

	auxpSHDBXref->recordid = StringSave((char*) vnp->data.ptrvalue);
	vnp = vnp->next;

	auxpSHDBXref->field    = StringSave((char*) vnp->data.ptrvalue);
	vnp = vnp->next;

	/* Test if StrToInt4 Faild */
	auxpSHDBXref->cv = atol(vnp->data.ptrvalue);
	vnp = vnp->next;

	if(vnp != NULL)
	{
	    auxpSHDBXref->next = SHDBXrefNew();
	    auxpSHDBXref = auxpSHDBXref->next;
	}
    }

    ValNodeFreeData(auxvnp);

    return pSHDBXref;
}


SHDBXrefPtr LIBCALL SHoundDBXREFGetSourceRecordListByDBXref(CharPtr dbname, CharPtr recordid, CharPtr field, Int4 cv)
{
    Char fpath[PATH_MAX];
    SHDBXrefPtr pSHDBXref = NULL;
    SHDBXrefPtr auxpSHDBXref = NULL;

    ValNodePtr vnp = NULL;
    ValNodePtr auxvnp = NULL;
    Int2 ercode = 0;

    auxvnp = vnp;

    if ( (dbname == NULL) || (recordid == NULL) )
    {
	ErrPostEx(SEV_ERROR,0,0, "Invalid parameter.");
	return 0;
    }


    if (field == NULL)
    {
	sprintf(fpath,"%s?fnct=SeqHoundDBXREFGetSourceRecordListByDBXref&dbname=%s&recordid=%s&field=NULL&cv=%d", slri_cgipath, dbname, recordid, cv);
    }
    else
    {
	sprintf(fpath,"%s?fnct=SeqHoundDBXREFGetSourceRecordListByDBXref&dbname=%s&recordid=%s&field=%s&cv=%d", slri_cgipath, dbname, recordid, field, cv);
    }

    ErrPostEx(SEV_INFO,0,0, "SeqHoundDBXREFGetSourceRecordListByDBXref request: %s.\n", fpath);
    if((ercode = SHoundWWWGetfile(slri_sername, fpath)) == 0) 
    {
	ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
	return NULL;
    } 
    else if (ercode == 2)
    {
	return NULL;
    }

    if ( (vnp = ReplyBSGetValNodeOrdered(FALSE)) == NULL)
    {
	return NULL;
    }
    pSHDBXref = SHDBXrefNew();
    auxpSHDBXref = pSHDBXref;

    while(vnp != NULL)
    {   
	auxpSHDBXref->sourcedb = StringSave((char*) vnp->data.ptrvalue);
	vnp = vnp->next;

	auxpSHDBXref->recordid = StringSave((char*) vnp->data.ptrvalue);
	vnp = vnp->next;

	auxpSHDBXref->field    = StringSave((char*) vnp->data.ptrvalue);
	vnp = vnp->next;

	/* Test if StrToInt4 Faild */
	auxpSHDBXref->cv = atol(vnp->data.ptrvalue);
	vnp = vnp->next;

	if(vnp != NULL)
	{
	    auxpSHDBXref->next = SHDBXrefNew();
	    auxpSHDBXref = auxpSHDBXref->next;
	}
    }

    ValNodeFreeData(auxvnp);

    return pSHDBXref;
}



ValNodePtr LIBCALL SHoundDBXREFGetSourceRecordIDListByDBName(CharPtr dbname)
{
    Char fpath[PATH_MAX];
    ValNodePtr vnpDBnames = NULL;

    Int2 ercode = 0;
    
    if (dbname == NULL)
    {
	ErrPostEx(SEV_ERROR,0,0, "Invalid parameter.");
	return 0;
    }

    sprintf(fpath,"%s?fnct=SeqHoundDBXREFGetSourceRecordIDListByDBName&dbname=%s", slri_cgipath, dbname);
    ErrPostEx(SEV_INFO,0,0, "SeqHoundDBXREFGetSourceRecordIDListByDBName request: %s.\n", fpath);
    if((ercode = SHoundWWWGetfile(slri_sername, fpath)) == 0) 
    {
	ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
	return NULL;
    } 
    else if (ercode == 2)
    {
	return NULL;
    }
    if ( (vnpDBnames = ReplyBSGetValNodeOrdered(FALSE)) == NULL)
    {
	return NULL;
    }
    return vnpDBnames;
}


ValNodePtr LIBCALL SHoundDBXREFGetXrefRecordIDListByDBName(CharPtr dbname)
{
    Char fpath[PATH_MAX];
    ValNodePtr vnpDBnames = NULL;
    Int2 ercode = 0;

    
    if (dbname == NULL)
    {
	ErrPostEx(SEV_ERROR,0,0, "Invalid parameter.");
	return 0;
    }

    sprintf(fpath,"%s?fnct=SeqHoundDBXREFGetXrefRecordIDListByDBName&dbname=%s", slri_cgipath, dbname);
    ErrPostEx(SEV_INFO,0,0, "SeqHoundDBXREFGetXrefRecordIDListByDBName request: %s.\n", fpath);
    if((ercode = SHoundWWWGetfile(slri_sername, fpath)) == 0) 
    {
	ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
	return NULL;
    } 
    else if (ercode == 2)
    {
	return NULL;
    }
    if ( (vnpDBnames = ReplyBSGetValNodeOrdered(FALSE)) == NULL)
    {
	return NULL;
    }
    return vnpDBnames;
}




ValNodePtr LIBCALL SHoundDBXREFGetCVList()
{
    Char fpath[PATH_MAX];
    ValNodePtr vnpCV = NULL;
    
    Int2 ercode = 0;

    sprintf(fpath,"%s?fnct=SeqHoundDBXREFGetCVList", slri_cgipath);
    ErrPostEx(SEV_INFO,0,0, "SeqHoundDBXREFGetCVList request: %s.\n", fpath);
    if((ercode = SHoundWWWGetfile(slri_sername, fpath)) == 0) 
    {
	ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
	return NULL;
    } 
    else if (ercode == 2)
    {
	return NULL;
    }
    if ( (vnpCV = ReplyBSGetValNodeOrdered(TRUE)) == NULL)
    {
	return NULL;
    }
    return vnpCV;
}


ValNodePtr LIBCALL SHoundDBXREFGetListOfAllDBNames()
{
    Char fpath[PATH_MAX];
    ValNodePtr vnpCV = NULL;
    
    Int2 ercode = 0;

    sprintf(fpath,"%s?fnct=SeqHoundDBXREFGetListOfAllDBNames", slri_cgipath);
    ErrPostEx(SEV_INFO,0,0, "SeqHoundDBXREFGetListOfAllDBNames request: %s.\n", fpath);
    if((ercode = SHoundWWWGetfile(slri_sername, fpath)) == 0) 
    {
	ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
	return NULL;
    } 
    else if (ercode == 2)
    {
	return NULL;
    }
    if ( (vnpCV = ReplyBSGetValNodeOrdered(FALSE)) == NULL)
    {
	return NULL;
    }
    return vnpCV;
}


ValNodePtr LIBCALL SHoundDBXREFGetListOfSourceDBNames()
{
    Char fpath[PATH_MAX];
    ValNodePtr vnpCV = NULL;
    
    Int2 ercode = 0;

    sprintf(fpath,"%s?fnct=SeqHoundDBXREFGetListOfSourceDBNames", slri_cgipath);
    ErrPostEx(SEV_INFO,0,0, "SeqHoundDBXREFGetListOfSourceDBNames request: %s.\n", fpath);
    if((ercode = SHoundWWWGetfile(slri_sername, fpath)) == 0) 
    {
	ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
	return NULL;
    } 
    else if (ercode == 2)
    {
	return NULL;
    }
    if ( (vnpCV = ReplyBSGetValNodeOrdered(FALSE)) == NULL)
    {
	return NULL;
    }
    return vnpCV;
}


ValNodePtr LIBCALL SHoundDBXREFGetListOfXrefDBNames()
{
    Char fpath[PATH_MAX];
    ValNodePtr vnpCV = NULL;
    
    Int2 ercode = 0;

    sprintf(fpath,"%s?fnct=SeqHoundDBXREFGetListOfXreferedDBNames", slri_cgipath);
    ErrPostEx(SEV_INFO,0,0, "SeqHoundDBXREFGetListOfXreferedDBNames request: %s.\n", fpath);
    if((ercode = SHoundWWWGetfile(slri_sername, fpath)) == 0) 
    {
	ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
	return NULL;
    } 
    else if (ercode == 2)
    {
	return NULL;
    }
    if ( (vnpCV = ReplyBSGetValNodeOrdered(FALSE)) == NULL)
    {
	return NULL;
    }
    return vnpCV;
}


ValNodePtr LIBCALL SHoundDBXREFGetXrefSourceFieldName()
{
    Char fpath[PATH_MAX];
    ValNodePtr vnpFieldNames = NULL;
    
    Int2 ercode = 0;

    sprintf(fpath,"%s?fnct=SeqHoundDBXREFGetXrefSourceFieldName", slri_cgipath);
    ErrPostEx(SEV_INFO,0,0, "SeqHoundDBXREFGetXrefSourceFieldName request: %s.\n", fpath);
    if((ercode = SHoundWWWGetfile(slri_sername, fpath)) == 0) 
    {
	ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
	return NULL;
    } 
    else if (ercode == 2)
    {
	return NULL;
    }
    if ( (vnpFieldNames = ReplyBSGetValNodeOrdered(FALSE)) == NULL)
    {
	return NULL;
    }
	
    return vnpFieldNames;
}


/* **************************************
 *  API List functions. Added March 2004
 * **************************************/
 
/*process an array of string(each string is the query-return pair, seperated by Q_R)
 *return a valnode list. The choice field of the ValNodeList indicates query or
 *return, a FastaPtr is stored in the valnode's data.ptrvalue
 *array is a pointer to CharPtr
 *count is a pointer to an int value which is the non-empty entries of the array
 */ 
ValNodePtr StrArray2FastaValNodeList(CharPtr *array, Int4* count)
{
   int i, q, size, pos, len;
   CharPtr query=NULL, value=NULL, a=NULL;
   ValNodePtr retlist = NULL;
   SLRIFastaPtr pfa=NULL;
 
   for (i=0; i< *count;i++)
   {  
      ByteStorePtr bsp = BSNew(10);
      AsnIoBSPtr aipbs = NULL;
      if (array[i] != NULL)
      {
	 size=strlen(array[i]);
	 a = strstr(array[i], Q_R);
	 pos=strlen(a);
	 len=size-pos;
	 query = (char*)malloc(100);
	 strncpy(query, array[i], len);
         value = StrSave(a+strlen(Q_R));
	 q = atoi(query);
	 if (query) MemFree(query);
	 
	 if (value != NULL && (StrCmp(value, "NULL")!=0))
	 {
	    if (BSprintf (bsp, "%s", value ))
	    { 
	        aipbs = AsnIoBSOpen("r", bsp);
		if (aipbs == NULL)
    		{
      		    ErrPostEx(SEV_ERROR,2,1,"[StrArray2FastaValNodeList]: AsnIoBSOpen returned NULL.");
		    BSFree(bsp);
      	       	    return NULL;
    		}
		pfa = SLRIFastaNew();
		pfa = (SLRIFastaPtr)SLRIFastaAsnRead(aipbs->aip, NULL);
		ValNodeAddInt(&retlist, 0, q);  
	        retlist = Slri_ValNodeAddPointerAtEnd(&retlist, 1, pfa);
		AsnIoBSClose(aipbs);
	    }
	    else
	    {
	        ErrPostEx(SEV_ERROR,2,1,"[StrArray2FastaValNodeList]: BSprintf failed.");
		BSFree(bsp);
		return NULL;
	    } 
	    
	 }
	 else
	 {
	    ValNodeAddInt(&retlist, 0, q);
	    retlist = Slri_ValNodeAddPointerAtEnd(&retlist, 1, NULL);   
	 }
      }
      BSFree(bsp);  
   }
   return retlist; 
}


 
/*caller must free this return ValNodePtr*/
/*call function Slri_FreeFastaKeyedList to free memory*/ 
ValNodePtr LIBCALL SHoundGetFastaKeyedList(ValNodePtr pvnGi)
{
  Int4 counter = 0, count = 0;
  ValNodePtr pvnfastafin = NULL;
  CharPtr array[MAXQ];
  Char fpath[PATH_MAX];
  CharPtr retstr= NULL, pvalstr = NULL;
  Int2 ercode = 0;
  ValNodePtr pvntemp = NULL, pvnshort = NULL, pvnret = NULL;
 
  
  if (pvnGi == NULL)
  {
      ErrPostEx(SEV_ERROR,0,0, "[SHoundGetFastaKeyedList]: Invalid parameter.");
      return NULL;
  }
  pvntemp = pvnGi;
  while (pvntemp != NULL)
  {
      counter++;
      pvnshort = Slri_ValNodeAddInt(&pvnshort, 0, pvntemp->data.intvalue);
      if(counter == MAXQ || pvntemp->next == NULL)
      {
	  pvalstr = Misc_SaveValNodeListAsString(pvnshort);
	  if(pvalstr == NULL)
	  {
	      ErrPostEx(SEV_ERROR,0,0, "[SHoundGetFastaKeyedList]: Misc_SaveValNodeListAsString returned NULL.");
	      pvnshort = ValNodeFree(pvnshort);
	      return NULL;
	  }
	  pvnshort = ValNodeFree(pvnshort);
	  sprintf(fpath,"%s?fnct=SeqHoundGetFastaKeyedList&pgi=%s", IsNullStr(slri_cgipath), IsNullStr(pvalstr));
	  ErrPostEx(SEV_INFO,2,1,"SeqHoundGetFastaKeyedList request: %s\n", IsNullStr(fpath));
	  pvalstr = MemFree(pvalstr);
	  if((ercode = SHoundWWWGetfile(slri_sername, fpath)) == 0)
	  {
	      ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
	      return NULL;
	  }
	  else if (ercode == 2)
	  {
	      BSFree(replyBS);
	      return NULL;
          }
	  if (!(retstr = BSStringSave(replyBS)))
	  {
	       ErrPostEx(SEV_INFO,0,0, "[SHoundGetFastaKeyedList]: returned NULL.");
	       BSFree(replyBS);
	       return NULL;
	  }
	  /*printf ("%s\n", IsNullStr(retstr));*/
	  Misc_Str2StrArray(retstr, array, MAXQ, &count, GROUP);
	  pvnret = StrArray2FastaValNodeList(array, &count);
	  ValNodeLink(&pvnfastafin, pvnret);
          counter = 0;
      }
      pvntemp = pvntemp->next;
  }
  if (replyBS) BSFree(replyBS);
  if (retstr)  MemFree(retstr);
  if (pvalstr) MemFree(pvalstr);
  return pvnfastafin;
}


/* caller free the return ValNodePtr*/
/* call Slri_FreeKeyedList to free return ValNodeList */
ValNodePtr LIBCALL SHoundRedundantGroupKeyedList(ValNodePtr pvnGi)
{
     Int4 counter = 0, count = 0;
     Int2 ercode =0;
     CharPtr array[MAXQ];
     Char fpath[PATH_MAX];
     CharPtr pvalstr = NULL, retstr = NULL;
     ValNodePtr pvngifin = NULL, pvnret = NULL;
     ValNodePtr pvntemp = NULL, pvnshort = NULL;
     

     if(pvnGi == NULL)
     {
        ErrPostEx(SEV_ERROR,0,0, "[SHoundRedundantGroupKeyedList]: Invalid parameter.");
        return NULL;
     }

     pvntemp = pvnGi;
     while (pvntemp != NULL)
     {
         counter++;
         pvnshort = Slri_ValNodeAddInt(&pvnshort, 0, pvntemp->data.intvalue);
         if(counter == MAXQ || pvntemp->next == NULL)
         {
            pvalstr = Misc_SaveValNodeListAsString(pvnshort);
            if(pvalstr == NULL)
            {
               ErrPostEx(SEV_ERROR,0,0, "Misc_SaveValNodeListAsString returned NULL.");
               pvnshort = ValNodeFree(pvnshort);
               return NULL;
            }
	    pvnshort = ValNodeFree(pvnshort);
            sprintf(fpath,"%s?fnct=SeqHoundRedundantGroupKeyedList&pgi=%s", IsNullStr(slri_cgipath), IsNullStr(pvalstr));
            ErrPostEx(SEV_INFO,0,0, "SeqHoundRedundantGroupKeyedList request: %s.\n", IsNullStr(fpath));
            pvalstr = MemFree(pvalstr);
            if(SHoundWWWGetfile(slri_sername, fpath) == 0)
            {
                ErrPostEx(SEV_ERROR,0,0, "SHoundWWWGetfile failed.");
                return NULL;
            }
	    else if (ercode == 2){
	      BSFree(replyBS);
	      return NULL;
            }
	    if (!(retstr = BSStringSave(replyBS)))
	    {
	        BSFree(replyBS);
	    	ErrPostEx(SEV_INFO,0,0, "[SHoundRedundantGroupKeyedList]: returned NULL.");
		return NULL;
	    }
	    /*printf ("%s\n", IsNullStr(retstr));*/
	    Misc_Str2StrArray(retstr, array, MAXQ, &count, GROUP);
	    pvnret = Misc_StrArray2ValNodeList(array, &count);
            ValNodeLink(&pvngifin, pvnret);
            counter = 0; 
          }
         pvntemp = pvntemp->next;
      }
     if (replyBS) BSFree(replyBS);
     if (retstr)  MemFree(retstr);
     if (pvalstr) MemFree(pvalstr);
     return pvngifin;
}

/* caller must free returned SLRIFasta */
SLRIFastaPtr LIBCALL SHoundGetFastaFromRedundantGroupID(Int4 groupid)
{
  Char fpath[PATH_MAX];
  Int2 ercode = 0;
  AsnIoBSPtr aipbs = NULL;
  SLRIFastaPtr pfa = NULL;
  
  if (groupid <= 0)
  {
      ErrPostEx(SEV_ERROR,0,0, "[SHoundGetFastaFromRedundantGroupID]: Invalid groupid.");
      return NULL;
  }
  
  sprintf(fpath,"%s?fnct=SeqHoundGetFastaFromRedundantGroupID&gid=%ld", IsNullStr(slri_cgipath), (long) groupid);
  ErrPostEx(SEV_INFO,2,1,"SeqHoundGetFastaFromRedundantGroupID request: %s\n", IsNullStr(fpath));
  if((ercode = SHoundWWWGetfile(slri_sername, fpath)) == 0)
  {
      ErrPostEx(SEV_ERROR,0,0, "[SHoundGetFastaFromRedundantGroupID]: SHoundWWWGetfile failed.");
      return NULL;
  }
  else if (ercode == 2)
  {
      BSFree(replyBS);
      return NULL;
  }
  aipbs = AsnIoBSOpen("r", replyBS);
  if (aipbs == NULL)
    {
      ErrPostEx(SEV_ERROR,2,1,"[SHoundGetFastaFromRedundantGroupID]:AsnIoBSOpen returned NULL.");
      return NULL;
    }
  pfa = (SLRIFastaPtr)SLRIFastaAsnRead(aipbs->aip, NULL);
  AsnIoBSClose(aipbs);
  BSFree(replyBS);	
  return pfa;
}


/*caller must free this return ValNodePtr*/ 
/*call Slri_FreeFastakeyedList to free return ValNodeList*/
ValNodePtr LIBCALL SHoundGetFastaFromRedundantGroupIDKeyedList(ValNodePtr pvnGroupid)
{
  Int4 counter = 0, count = 0;
  ValNodePtr pvnfastafin = NULL;
  CharPtr array[MAXQ];
  Char fpath[PATH_MAX];
  CharPtr retstr= NULL, pvalstr = NULL;
  Int2 ercode = 0;
  ValNodePtr pvntemp = NULL, pvnshort = NULL, pvnret = NULL;
 
  
  
  if (pvnGroupid == NULL)
  {
      ErrPostEx(SEV_ERROR,0,0, "[SHoundGetFastaFromRedundantGroupIDKeyedList]: Invalid parameter.");
      return NULL;
  }
  pvntemp = pvnGroupid;
  while (pvntemp != NULL)
  {
      counter++;
      pvnshort = Slri_ValNodeAddInt(&pvnshort, 0, pvntemp->data.intvalue);
      if(counter == MAXQ || pvntemp->next == NULL)
      {
	  pvalstr = Misc_SaveValNodeListAsString(pvnshort);
	  if(pvalstr == NULL)
	  {
	      ErrPostEx(SEV_ERROR,0,0, "[SHoundGetFastaFromRedundantGroupIDKeyedList]: Misc_SaveValNodeListAsString returned NULL.");
	      pvnshort = ValNodeFree(pvnshort);
	      pvalstr = MemFree(pvalstr);
	      return NULL;
	  }
	  pvnshort = ValNodeFree(pvnshort);
	  sprintf(fpath,"%s?fnct=SeqHoundGetFastaFromRedundantGroupIDKeyedList&pgid=%s", IsNullStr(slri_cgipath), IsNullStr(pvalstr));
	  ErrPostEx(SEV_INFO,2,1,"SeqHoundGetFastaFromRedundantGroupIDKeyedList request: %s\n", IsNullStr(fpath));
	  pvalstr = MemFree(pvalstr);
	  if((ercode = SHoundWWWGetfile(slri_sername, fpath)) == 0)
	  {
	      ErrPostEx(SEV_ERROR,0,0, "[SHoundGetFastaFromRedundantGroupIDKeyedList]: SHoundWWWGetfile failed.");
	      return NULL;
	  }
	  else if (ercode == 2)
	  {
	      BSFree(replyBS);
	      return NULL;
          }
	  if (!(retstr = BSStringSave(replyBS)))
	  {
		ErrPostEx(SEV_INFO,0,0, "[SHoundGetFastaFromRedundantGroupIDKeyedList]: returned NULL.");
		BSFree(replyBS);
		return NULL;
	  }
	  /*printf ("%s\n", IsNullStr(retstr));*/
	  Misc_Str2StrArray(retstr, array, MAXQ, &count, GROUP);
	  pvnret = StrArray2FastaValNodeList(array, &count);
          ValNodeLink(&pvnfastafin, pvnret);
          counter = 0;
          
      }
      pvntemp = pvntemp->next;
  }
  if (replyBS) BSFree(replyBS);
  if (retstr)  MemFree(retstr);
  if (pvalstr) MemFree(pvalstr);
  return pvnfastafin;
}



/*
$Log: seqhoundapi.c,v $
Revision 1.42  2005/03/07 20:27:40  ryao
in three keyedList functions: use fpath[PATH_MAX] instead of malloc memory for fpath; minor code integration in these functions.

Revision 1.41  2005/03/07 17:32:43  ryao
Modified StrArray2FastaValNodeList() to adapt separator Q_R as a sequence of chars instead of a single char. Modified SHoundGetFastaKeyedList() to better control memory free.

Revision 1.40  2005/01/21 19:55:19  vgu
Code change related to bug 4254: increase default timeout to 3 minutes for large mmdb objects

Revision 1.39  2004/11/22 14:56:58  mmatan

Modified shoundgetfixed3d so that is takes the same parameters as shoundget3dex.
See bug 3930 for details.

Revision 1.38  2004/11/18 19:08:45  mmatan
Added function SHoundGetFixed3D to the remote C api.  See bug 3930.

Revision 1.37  2004/11/17 15:21:37  ryao
added StrCmp check.

Revision 1.36  2004/11/16 21:33:23  vgu
Fix null pointer check error

Revision 1.35  2004/11/11 22:06:29  mmatan
Made sure the file handles are closed in case of error returns for
SHoundGetPDB3D.  This fixes bug 3905.

Revision 1.34  2004/10/18 20:49:36  hfeldman
New CONN_Write args

Revision 1.33  2004/10/01 15:47:06  ryao
Added SHoundGetNewTaxIDFromMergedTaxID. bug[3474]

Revision 1.32  2004/09/07 19:51:45  ryao
Implemented IntrezGetCDSLRIFasta. bug [2929]

Revision 1.31  2004/08/12 17:55:49  zwang
New API added to get title by gi from ACCDB

Revision 1.30  2004/06/07 22:06:18  hlieu
minor cross comp fixes, bug 2185

Revision 1.29  2004/06/02 17:33:38  ryao
added new locus link functions.

Revision 1.28  2004/05/31 20:58:36  hlieu
minor compiler warning fixes, bug 2185

Revision 1.27  2004/05/31 20:36:17  hlieu
minor compiler warning fixes for bug 2185

Revision 1.26  2004/05/10 16:15:07  ryao
removed unused variables according to crosscompileError

Revision 1.25  2004/04/22 18:52:52  egarderm
Fixed comment lines to adhere to C standards

Revision 1.24  2004/04/22 16:37:40  hfeldman
Fixed illegal line in code

Revision 1.23  2004/04/21 20:50:33  hlieu
further to fix to bug 1768

Revision 1.22  2004/04/21 20:38:39  hlieu
fix bug 1768

Revision 1.21  2004/04/19 16:46:46  ryao
Added SHoundGetFastaFromRedundantGroupID SHoundGetFastaFromRedundantGroupIDKeyedList

Revision 1.20  2004/04/08 16:36:48  ryao
Modified StrArray2FastaValNodeList

Revision 1.19  2004/04/05 19:06:09  ryao
Added SHoundGetFastaKeyedList SHoundRedundantGroupKeyedList StrArray2FastaValNodeList

Revision 1.18  2004/03/30 15:47:07  ryao
Fix the inconsistency of the return value for some functions. bug 372 related.

Revision 1.17  2004/01/14 18:40:57  rcavero
Previous version was breaking when compiling in Strict C.

Revision 1.16  2004/01/13 21:29:43  rcavero
Including DBXref Module

Revision 1.15  2004/01/12 22:20:28  ryao
minor change

Revision 1.14  2004/01/09 22:12:01  ryao
add SHoundGetLargestMMDBID

Revision 1.13  2004/01/06 22:32:17  skuznets
Code audit has been done sucessfully.
Possible NULL pointer issues has been eliminated.

Revision 1.12  2004/01/02 22:37:53  hlieu
added NetEntrez support

Revision 1.11  2003/11/26 20:10:55  ryao
added SHoundGetReferenceIDFromGiList SHoundGiFromReferenceList SHoundGiFromGOIDListAndECode

Revision 1.10  2003/11/20 21:16:31  feldman
fixed memory leak

Revision 1.9  2003/11/07 21:06:27  ryao
added SHoundGiFromGOIDAndECode

Revision 1.8  2003/11/06 22:53:46  ryao
added SHoundGiFromReferenceID

Revision 1.7  2003/10/24 17:53:11  feldman
Added SHoundGet3DEx function to allow user to choose model level and num models when fetching biostruc

Revision 1.6  2003/10/09 21:28:43  ryao
add SHoundGetReferenceIDFromGi

Revision 1.5  2003/09/10 21:17:43  ryao
Added functionality SHoundMoleculeType, SHoundGOPMIDFromGiAndGOID and
SHoundGOECodeFromGiAndGOID

Revision 1.4  2003/09/03 19:41:04  volodya
added function SHoundGetNameByGi

Revision 1.3  2003/08/26 20:37:46  haocl
changed SHoundGOECodeFromGi to GiAndGOID

Revision 1.2  2003/08/22 15:25:23  haocl
added GetAccFromRetiredGi

Revision 1.1.1.19  2003/08/16 08:46:07  root
Automated import from Sourceforge.net

Revision 1.25  2003/08/13 20:10:11  haocl
added SHoundSequenceLength

Revision 1.24  2002/10/28 20:52:14  kaca
added dummy fnct SHoundInitReadOnly for consistency between remote and local APIs

Revision 1.23  2002/07/30 18:07:53  betel
Fixed GetDomainsFromGiListWithEvalue

Revision 1.22  2002/07/29 22:30:46  betel
Added GetDomainsFromGiListWithEvalue for local&remote C api (no Perl or C++ yet)

Revision 1.21  2002/07/08 16:58:02  kaca
renamed two RPS-BLAST functions Accession is DomainId

Revision 1.20  2002/06/28 21:50:32  kaca
additional param check in SHoundGiFromPDBchain

Revision 1.19  2002/05/13 17:52:26  haocl
added remote rps functions

Revision 1.18  2002/03/25 21:04:06  micheld
Returned SHoundGet3D to binary mode.

Revision 1.17  2002/03/25 08:15:55  micheld
Changed binary mode to text mode for SHoundGet3D on client & server
(doesn't appear to work in binary mode)

Revision 1.16  2002/03/25 07:51:32  micheld
New genome functions. changed includes to new slrilinkset.h

Revision 1.15  2002/03/02 23:51:55  micheld
Added \0 to clear remaining buffer after reading from server.

Revision 1.14  2002/02/06 23:56:46  micheld
Changed TaxDB_ Functions to SHound*Tax* function call, legacy preserved for C api but *NOT* PERL

Revision 1.13  2002/01/15 17:28:17  kaca
disabled SHoundGODBGetHierarchyLevel due to a wrong logic

Revision 1.12  2001/12/19 04:14:18  kaca
error catching cosmetic fixes

Revision 1.11  2001/12/18 18:45:51  kaca
fixed SHoundRedundantGroupFromIDList

Revision 1.10  2001/12/11 19:11:09  gbader
SHoundIsInited now returns global init variable instead of asking server.

Revision 1.9  2001/12/05 00:55:36  micheld
minor fix

Revision 1.8  2001/12/05 00:52:07  micheld
Added access to BLAST pairwise results... returns either NBLASTDB format or seqalign

Revision 1.7  2001/11/13 22:57:08  kaca
ReplyBSGetValNode tokenizer change

Revision 1.6  2001/11/13 21:57:45  kaca
new defline and goid annotation functions

Revision 1.5  2001/11/05 23:29:35  kaca
removed unused variable

Revision 1.4  2001/11/02 21:23:25  micheld
Amended TaxDB_ to TAXDB_ remote function calls to match seqrem expect

Revision 1.3  2001/10/10 22:02:42  kaca
resolved import conflict


Revision 1.1.1.2  2001/10/10 21:53:48  kaca
initial import

Revision 1.74  2001/09/25 19:43:38  kaca
consolidating functional annotation project

Revision 1.73  2001/09/21 17:40:09  kaca
consolidating GO hierarchy project

Revision 1.72  2001/09/20 21:37:33  kaca
added extra return from SHoundInit

Revision 1.69  2001/09/19 15:49:17  kaca
increased default timeout to library value 120 sec

Revision 1.68  2001/09/10 16:49:38  michel
added SLRI_MMDBBiostrucGet to call MMDBGetBiostrucGet (for clustal library)

Revision 1.67  2001/09/09 05:40:24  michel
Added MMDBBiostrucGet as linkable symbol (which calls SHoundGet3D),
if application also links MMDB library, use -z muldefs to use first linked symbol (i.e. shoundrem library)

Revision 1.66  2001/09/04 21:33:58  michel
Added local equivalent of the set and get timeout functions for local API (which do nothing)
Removed compiler warning in ll_db.h

Revision 1.65  2001/09/04 05:36:51  michel
Added user functions for user to change connnection settings for seqhound remote api

Revision 1.64  2001/08/30 19:42:50  haocl
added comments to locus link and gene ontology remote functions

Revision 1.63  2001/08/30 18:37:45  haocl
fixed ReplyBSGetFloat

Revision 1.62  2001/08/30 18:17:48  haocl
changed SHoundGODBGetClassification to return CharPtr

Revision 1.61  2001/08/29 16:41:55  haocl
fixed errors from renaming SHoundECodeFromGi

Revision 1.60  2001/08/29 16:34:07  haocl
rename SHoundECodeFromGi

Revision 1.59  2001/08/28 18:57:55  haocl
corrected SHoundCDDScoreFromGi

Revision 1.58  2001/08/27 20:14:14  haocl
created function ReplyBSGetFullString

Revision 1.57  2001/08/27 18:02:46  haocl
added gene ontology remote api

Revision 1.56  2001/08/27 15:07:24  hogue
fixed compile errors

Revision 1.55  2001/08/24 20:55:58  michel
SEQHOUNDNBRS flag is no longer required
removed compiler warnings

Revision 1.54  2001/08/23 19:03:28  haocl
added a BSReplyGetFloat

Revision 1.53  2001/08/23 18:56:43  haocl
added locuslink functions

Revision 1.52  2001/08/23 18:49:19  haocl
locuslink functions added

Revision 1.51  2001/08/23 18:11:00  michel
Returned timeout connection setting to default after change

Revision 1.50  2001/08/23 17:46:11  michel
fixed source error

Revision 1.49  2001/08/23 16:14:55  michel
Added option to change timeout in connection... important for long server-side computations

Revision 1.48  2001/08/23 13:53:35  haocl
added locuslink function

Revision 1.47  2001/08/22 17:37:48  haocl
fixed SHoundECodeFromGi

Revision 1.46  2001/08/15 23:15:51  kaca
change to lesser GNU licence

Revision 1.45  2001/08/15 22:26:34  kaca
removed conflict

Revision 1.44  2001/08/15 22:05:45  haocl
added locuslink function

Revision 1.43  2001/08/15 20:38:29  michel
bug fix

Revision 1.42  2001/08/15 19:06:50  michel
bug fixes in new functions

Revision 1.41  2001/08/15 18:11:47  michel
Added pathnbr for config file to find neighbour database
Added additional functions and error checking

Revision 1.40  2001/08/14 14:29:59  michel
Fixed and tested Neighbour Functions, with the exception of 3D Neighbours
Tested Get3DListFromTaxID - returns all redundant group's 3d structures

Revision 1.39  2001/08/09 15:42:27  michel
minor change

Revision 1.38  2001/08/09 15:36:50  michel
Changed string conversion functions
Added more error checking

Revision 1.37  2001/08/07 18:54:49  michel
Tested SHoundNeighboursFromGiEx
Added the rest of the remote API and CGI calls (untested)

Revision 1.36  2001/08/03 19:02:55  michel
Added remote API SHoundNeighboursFromGiEx (untested) and CGI call

Revision 1.35  2001/06/07 19:13:28  kaca
replaced Get*FromTaxIDII with Get*FromTaxIDIII

Revision 1.34  2001/05/10 19:53:55  kaca
initialized slri_sername

Revision 1.33  2001/04/27 23:12:28  kaca
added SHoundFindName and SHoundFindNameList functions

Revision 1.32  2001/04/18 15:51:02  kaca
more new toolkit connectivity changes

Revision 1.31  2001/04/16 20:43:06  kaca
reflect changes in toolkit connect stuff

Revision 1.30  2001/04/12 21:00:15  kaca
added functions

Revision 1.29  2001/04/06 22:45:14  kaca
added SHoundGiFromPDBchain

Revision 1.28  2001/04/03 19:17:04  kaca
small fix

Revision 1.27  2001/03/30 18:51:32  gbader
Fixed WWWGetFile memory leaks from Purify

Revision 1.26  2001/03/28 22:59:23  gbader
Added #include <intrez_misc.h>

Revision 1.25  2001/03/27 21:26:33  kaca
path to the server read from config file

Revision 1.24  2001/03/26 22:34:06  kaca
created slri+misc and moved some API functions into it

Revision 1.23  2001/03/26 19:58:34  kaca
changes in include files

Revision 1.22  2001/03/19 22:11:44  kaca
added remote XML functions

Revision 1.21  2001/03/15 18:03:58  gbader
Added SHoundGetCurrentServer function

Revision 1.20  2001/03/09 20:16:45  kaca
small fix

Revision 1.19  2001/03/09 20:09:25  kaca
freeing functions now return input param type

Revision 1.18  2001/03/09 19:17:08  kaca
added SeqHoundFirstOfRedundantGroupFromID functions

Revision 1.17  2001/03/09 16:41:07  gbader
Added PrintSLRIFasta and PrintSLRIFastaList from intrez.c

Revision 1.16  2001/03/08 22:35:07  kaca
added flat file functionality

Revision 1.15  2001/03/08 15:42:58  kaca
removed valnode/string functions which migratedinto slrilib

Revision 1.14  2001/02/23 20:43:26  kaca
license change

Revision 1.13  2001/01/26 19:20:14  kaca
removed unused variables

Revision 1.12  2000/11/01 23:29:09  kaca
added redundancy and taxonomy functions

Revision 1.11  2000/10/26 13:44:54  gbader
Fixed my previous attempt at SHoundInit

Revision 1.10  2000/10/25 21:53:06  gbader
Removed debug printf's from SHoundInit to prevent interference with CGI's

Revision 1.9  2000/10/20 17:28:53  kaca
changed config file name

Revision 1.8  2000/10/19 22:59:13  kaca
minor change

Revision 1.7  2000/10/19 19:29:55  kaca
SHoundInit looks at ookpik server in case zaphod fails

Revision 1.6  2000/10/19 19:27:05  kaca
renamed libraries to lshoundlocl and lshoundrem

Revision 1.5  2000/10/16 20:16:11  kaca
removed define of generated code

Revision 1.4  2000/10/13 22:59:01  kaca
the server name is read from the config file

Revision 1.3  2000/10/13 21:54:23  kaca
changed includes

Revision 1.2  2000/09/20 18:49:59  kaca
split SeqHound headers into private and public one and changed includes

Revision 1.1.1.1  2000/09/18 17:41:56  kaca
Initial import

Revision 1.3  2000/07/12 17:30:47  kaca
ValNodeAdd* changed to faster Slri_ValNodeAdd

*/
