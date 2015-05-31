/* $Id: slri_cb.c,v 1.3 2003/12/04 19:10:53 mrdumont Exp $
-- ***************************************************************************
--
--  Samuel Lunenfeld Research Institute (SLRI)
--
--  File: slri_cb.c
--  Description: Generic CodeBase database functions
--
--  Author: Katerina Michalickova, Chris Hogue, Gary Bader
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

#include <slri_cb.h>
#include <slri_misc.h>
#include <slri_asn.h>

/*allocates and creates a full database name given a path and a database name in a platform
independent manner*/
CharPtr AllocateAndCreateDBPath(CharPtr dbPath, CharPtr dbName)
{
	CharPtr dbPathRet=NULL;
	Boolean dbPathEndsWithDirDelim=FALSE;

	if(dbPath==NULL) {
		ErrPostEx(SEV_ERROR, 0, 0, "AllocateAndCreateDBPath: dbPath is NULL.");
		return(NULL); 
	}
	if(dbName==NULL) {
		ErrPostEx(SEV_ERROR, 0, 0, "AllocateAndCreateDBPath: dbName is NULL.");
		return(NULL); 
	}

	if(dbPath[strlen(dbPath)-1]==DIRDELIMCHR) {
		dbPathEndsWithDirDelim=TRUE;
	}

	dbPathRet = MemNew((size_t)PATH_MAX*sizeof(Char));
	sprintf(dbPathRet, "%s%s%s", dbPath, (dbPathEndsWithDirDelim)?"":DIRDELIMSTR, IsNullStr(dbName));

	return(dbPathRet);
}

/*generically open a CodeBase database*/
Boolean OpenDB(CharPtr pathname, DATA4 **pd4db, FIELD4INFO *f4i,
			   TAG4INFO *t4i, InitFieldFunc initFieldFunc, CODE4 *codebase)
{
	Boolean created=FALSE;

	(*pd4db) = d4open(codebase, pathname);
	if((*pd4db) == NULL) {
		ErrPostEx(SEV_WARNING,0,0,"d4open returned NULL: %s (%ld)\n",pathname,codebase->errorCode);
		(*pd4db) = d4create(codebase, pathname, f4i, t4i);
		created = TRUE;
	}

	if(initFieldFunc) {
		(*initFieldFunc)();
	}

	return(created);
}

/*generically open a CodeBase database, the user decides if the database should be just opened or newly created*/ 
/* if the function returns FALSE, the database was not found or could not be created */
Boolean OpenDBII(CharPtr pathname, DATA4 **pd4db, FIELD4INFO *f4i,
			   TAG4INFO *t4i, InitFieldFunc initFieldFunc, CODE4 *codebase, Boolean CREATE)
{

	if(CREATE) {
		(*pd4db) = d4create(codebase, pathname, f4i, t4i);
		if((*codebase).errorCode < 0)
			return FALSE;
	}
	else {
		(*pd4db) = d4open(codebase, pathname);
		if((*pd4db) == NULL) 
			return FALSE;
	}


	if(initFieldFunc) {
		(*initFieldFunc)();
	}

	return TRUE;
}


/*deletes the database files - useful for a refresh function*/
Boolean DeleteDB(DATA4 *d4datap, CharPtr dbPath)
{
	Char dbName[PATH_MAX], dbNameRoot[PATH_MAX];
	CharPtr dbPathAndName=NULL;
	Boolean success=TRUE;

	StringCpy(dbNameRoot, d4alias(d4datap));

	/*close database so we can delete the files*/
	d4close(d4datap);

	/*Make sure buffer will not be overflowed by "path/dbName.ext"*/
	if(strlen(dbName)+strlen(dbPath)+6 > PATH_MAX) {
		return(FALSE);
	}

	if((dbPath!=NULL)||(dbPath[0]=='\0')) {
		dbPathAndName = AllocateAndCreateDBPath(dbPath, dbName);
	}
	else {
		dbPathAndName = StringSave(dbName);
	}

	/*remove .dbf, .cdx, .fpt*/
	sprintf(dbName, "%s.dbf", IsNullStr(dbPathAndName));
	if(!FileRemove(dbName)) {
		success=FALSE;	/*.dbf is always present*/
	}

	sprintf(dbName, "%s.cdx", IsNullStr(dbPathAndName));
	FileRemove(dbName);

	sprintf(dbName, "%s.fpt", IsNullStr(dbPathAndName));
	FileRemove(dbName);

	dbPathAndName=MemFree(dbPathAndName);

	return(success);
}

/********************/
/*ASN.1 <-> CodeBase*/
/********************/

/* Stores binary blobs into CodeBase Memo field */
Boolean AssignASNMemMemo(Pointer pASNdata, AsnWriteFunc pAsnWrite, FIELD4 *pf4Field)
{
	AsnIoBSPtr aibp=NULL;
	ByteStorePtr bsp=NULL;
	Int4 size=5000;	/*start size of ByteStore*/
	CharPtr buf = NULL;
	Int4 bsLength=0;

	if (pASNdata == NULL) {
		ErrPostEx(SEV_ERROR, 0, 0, "AssignASNMemMemo: pASNdata is NULL.");
		return FALSE; 
	}
    if (pf4Field == NULL) {
		ErrPostEx(SEV_ERROR, 0, 0, "AssignASNMemMemo: pf4Field is NULL.");
		return FALSE;
	}
	if (pAsnWrite == NULL) { 
		ErrPostEx(SEV_ERROR, 0, 0, "AssignASNMemMemo: pAsnWrite is NULL.");
		return FALSE;
	}     

	bsp = BSNew(size);
	BSSeek(bsp, 0L, SEEK_SET);
	aibp = AsnIoBSOpen(GetASNWriteMode(), bsp);

	if (aibp == NULL) {
		ErrPostEx(SEV_ERROR, 0, 0, "AssignASNMemMemo: AsnIoBSPtr is NULL.");
		return FALSE;
	}
	if( ! (*pAsnWrite) (pASNdata, aibp->aip, NULL)) {
		AsnIoBSClose(aibp);
		ErrPostEx(SEV_ERROR, 0, 0, "AssignASNMemMemo: Asn ByteStore mapped write function failed.");
		bsp = BSFree(bsp);
		return FALSE;
	}
	aibp = AsnIoBSClose(aibp);

	BSSeek(bsp, 0L, SEEK_SET);
	bsLength = BSLen(bsp);
	buf = MemNew((size_t) bsLength);
	if(buf == NULL) {
		ErrPostEx(SEV_ERROR, 0, 0, "AssignASNMemMemo: Out of memory.");
		bsp = BSFree(bsp);
		return FALSE;
	}
	if(BSRead(bsp, buf, bsLength) != bsLength) {
		ErrPostEx(SEV_ERROR, 0, 0, "AssignASNMemMemo: BSRead failed.");
		bsp = BSFree(bsp);
		buf = MemFree(buf);
		return FALSE;
	}
	if (f4memoAssignN(pf4Field, buf, bsLength) < 0) {
		ErrPostEx(SEV_ERROR, 0, 0, "AssignASNMemMemo: f4memoAssignN failed.");
		bsp = BSFree(bsp);
		buf = MemFree(buf);
		return FALSE;
	}
	else {
		bsp = BSFree(bsp);
		buf = MemFree(buf);
		return TRUE;
	}
}

/* Generically gets the ASN.1 from the CodeBase ASN.1 field given */
Pointer GetASNMemMemo(FIELD4 *pf4Field, AsnReadFunc pAsnRead)
{
	Int4 length=0;
	AsnIoMemPtr aimp = NULL;
	Pointer pASNdata=NULL;
	CODE4 *getASNMemMemoCBTmp;

	if (pf4Field == NULL) {
		ErrPostEx(SEV_ERROR, 0, 0, "GetASNMemMemo: pf4Field is NULL.");
		return NULL;
	}
	if (pAsnRead == NULL) {
		ErrPostEx(SEV_ERROR, 0, 0, "GetASNMemMemo: pAsnRead is NULL.");
		return NULL;
	}

	/*get length of the memo field*/
	length = f4memoLen(pf4Field);
	if(length<=0) {
		/*attempt to determine if an error occured, if so post an error message*/
		if((pf4Field->data!=NULL)&&(pf4Field->data->codeBase!=NULL)) {
			getASNMemMemoCBTmp = pf4Field->data->codeBase;
			if(getASNMemMemoCBTmp->errorCode<0) {
				ErrPostEx(SEV_ERROR, 0, 0, "GetASNMemMemo: f4memoLen error %d, unable to determine memo field size.",getASNMemMemoCBTmp->errorCode);
			}
		}
		return(NULL);
	}

	/*open the asn io stream*/
	aimp = AsnIoMemOpen(GetASNReadMode(), (BytePtr) f4memoStr(pf4Field), length);
	if (aimp == NULL) {
		ErrPostEx(SEV_ERROR, 0, 0, "GetASNMemMemo: AsnIoMemPtr is NULL.");
		return NULL;
	}

	/*read the asn.1 into a data structure*/
	pASNdata = (Pointer) (*pAsnRead) (aimp->aip, NULL);

	/*clean up*/
    AsnIoMemClose(aimp);

	/*return a pointer to the data structure*/
	return(pASNdata);
}

/***********************************************/
/*Comma separated lists <-> CodeBase Memo Field*/
/***********************************************/

#define COMMA_ENCODE_CHAR 1 /*^A*/

/*Adds a string to a string list in a memo field - optionally keep the list unique*/
Boolean AddString2ListMemo(CharPtr string, FIELD4 *pf4MemoField, Boolean unique)
{
	CharPtr curList=NULL, newList=NULL, strTmp=NULL, strCopy=NULL;
	ValNodePtr vnp=NULL;
	CODE4 *getASNMemMemoCBTmp;

	if(string == NULL) {
		ErrPostEx(SEV_ERROR, 0, 0, "AddString2ListMemo: string is NULL.");
		return(FALSE);
	}
    if(pf4MemoField == NULL) {
		ErrPostEx(SEV_ERROR, 0, 0, "AddString2ListMemo: pf4MemoField is NULL.");
		return FALSE;
	}

	/*copy string to new memory to avoid side effect of comma replacement*/
	strCopy = StringSave(string);

	/*encode commas in input*/
	strTmp = strCopy;
	while((strTmp=StringChr(strTmp, ','))!=NULL) {
		strTmp[0]=COMMA_ENCODE_CHAR;
		strTmp++;
	}

	curList = (CharPtr) f4memoStr(pf4MemoField);
	if((curList!=NULL)&&(curList[0]!='\0')) {
		if(unique) {
			vnp = Misc_CommaStrString2ValNodeList(curList, ",");
			if(!Misc_IsStrInStrVnpList(vnp, strCopy, TRUE)) {
				/*string not present, add it*/
				newList = MemNew((size_t) (strlen(curList)+strlen(strCopy)+8)*sizeof(Char));
				sprintf(newList, "%s,%s", IsNullStr(curList), IsNullStr(strCopy));
			}
			vnp = ValNodeFreeData(vnp);
		}
		else {
			/*string is present, still add it*/
			newList = MemNew((size_t) (strlen(curList)+strlen(strCopy)+8)*sizeof(Char));
			sprintf(newList, "%s,%s", IsNullStr(curList), IsNullStr(strCopy));
		}
		if(newList!=NULL) {
			if(f4memoAssign(pf4MemoField, newList)<0) {
				ErrPostEx(SEV_ERROR, 0, 0, "AddString2ListMemo: f4memoAssign failed.");
				strCopy=MemFree(strCopy);
				return(FALSE);
			}
			newList = MemFree(newList);
		}
	}
	else {
		/*check for error*/
		getASNMemMemoCBTmp = pf4MemoField->data->codeBase;
		if(getASNMemMemoCBTmp->errorCode<0) {
			ErrPostEx(SEV_ERROR, 0, 0, "AddString2ListMemo: f4memoStr failed.");
			strCopy=MemFree(strCopy);
			return(FALSE);
		}
		/*no strings have been added, add one*/
		if(f4memoAssign(pf4MemoField, strCopy)<0) {
			ErrPostEx(SEV_ERROR, 0, 0, "AddString2ListMemo: f4memoAssign failed.");
			strCopy=MemFree(strCopy);
			return(FALSE);
		}
	}

	strCopy=MemFree(strCopy);
	return(TRUE);
}

/*Gets a string from a string list in a memo field*/
CharPtr GetStringFromListMemo(FIELD4 *pf4MemoField)
{
	CharPtr string=NULL, strRet=NULL, strTmp=NULL;
	ByteStorePtr bsp=NULL;

	string = (CharPtr) f4memoStr(pf4MemoField);

	/*unencode string*/
	bsp=BSNew(16);
	strTmp = string;
	while(strTmp[0]!='\0') {
		switch(strTmp[0]) {
		case ',':
			BSWrite(bsp, ", ", 2);
			break;
		case COMMA_ENCODE_CHAR:
			BSWrite(bsp, ",", 1);
			break;
		default:
			BSWrite(bsp, strTmp, 1);
		}
		strTmp++;
	}
	strRet = BSStringSave(bsp);
	bsp=BSFree(bsp);

	return(strRet);
}

/*Adds an integer to an integer list in a memo field - optionally keep the list sorted or sorted&unique*/
Boolean AddInt2ListMemo(Int4 integer, FIELD4 *pf4MemoField, Boolean sorted, Boolean unique)
{
	CharPtr curList=NULL, newList=NULL;
	ValNodePtr vnpInts=NULL;
	CODE4 *getASNMemMemoCBTmp;

    if(pf4MemoField == NULL) {
		ErrPostEx(SEV_ERROR, 0, 0, "AddInt2ListMemo: pf4MemoField is NULL.");
		return FALSE;
	}

	curList = (CharPtr) f4memoStr(pf4MemoField);
	if((curList!=NULL)&&(curList[0]!='\0')) {
		/*integers have already been added, add one*/
		vnpInts = Misc_CommaIntString2ValNodeListOrdered(curList, ",");
		if(sorted) {
			Misc_AddInt2SortedList(&vnpInts, integer, 0, unique);
		}
		else {
			ValNodeAddInt(&vnpInts, 0, integer);
		}
		newList = Misc_SaveValNodeListAsString(vnpInts);
		vnpInts = ValNodeFree(vnpInts);
		if(f4memoAssign(pf4MemoField, newList)<0) {
			ErrPostEx(SEV_ERROR, 0, 0, "AddInt2ListMemo: f4memoAssign failed.");
			newList = MemFree(newList);
			return(FALSE);
		}
		newList = MemFree(newList);
	}
	else {
		getASNMemMemoCBTmp = pf4MemoField->data->codeBase;
		if(getASNMemMemoCBTmp->errorCode<0) {
			ErrPostEx(SEV_ERROR, 0, 0, "AddInt2ListMemo: f4memoStr failed.");
			return(FALSE);
		}
		/*no integers have been added, add one*/
		if(f4memoAssign(pf4MemoField, Ltostr(integer, 0))<0) {
			ErrPostEx(SEV_ERROR, 0, 0, "AddInt2ListMemo: f4memoAssign failed.");
			return(FALSE);
		}
	}

	return(TRUE);
}

/*
$Log: slri_cb.c,v $
Revision 1.3  2003/12/04 19:10:53  mrdumont
added diagnostics to OpenDB

Revision 1.2  2003/10/16 20:59:50  skuznets

1. Checking for NULL pointers when print values
2. Added function Misc_TrimSpacesAroundString
3. Added macro IsNullStr

Revision 1.1.1.6  2002/08/07 08:29:37  root
Automated import from Sourceforge.net

Revision 1.7  2002/08/06 23:03:09  betel
Removed changes made previously

Revision 1.6  2002/08/06 15:28:29  betel
Synchronize with latest C++ toolkit

Revision 1.5  2002/07/09 14:11:36  gbader
Fixed error message

Revision 1.4  2002/03/01 23:57:45  gbader
Fixed compiler warnings
Moved generic asn set read/write mode to slri_asn

Revision 1.3  2001/11/22 17:28:52  kaca
fix of handling dir delimiter in AllocateAndCreateDBPath

Revision 1.2  2001/11/21 23:55:23  kaca
added OpenDBII accepts Boolean flag for creating a database

Revision 1.1.1.3  2001/10/10 22:11:03  kaca
initial import

Revision 1.17  2001/08/24 22:47:32  michel
modified error to return error type in log

Revision 1.16  2001/08/20 15:46:40  gbader
Added parameter error checking to AllocateAndCreateDBPath

Revision 1.15  2001/07/03 18:54:02  gbader
Fixed NULL checking bug in IntMemoList function

Revision 1.14  2001/06/20 02:06:27  gbader
Fixed side-effect in AddString2ListMemo

Revision 1.13  2001/06/18 19:42:29  gbader
ASN.1 read/write mode is binary by default.

Revision 1.12  2001/05/25 19:04:45  gbader
User can now select database ASN.1 read/write mode.

Revision 1.11  2001/04/27 01:45:37  gbader
Fixed memory free problem for int memolist

Revision 1.10  2001/04/05 22:57:03  gbader
Added GetStringFromListMemo and fixed ListMemo to encode important characters

Revision 1.9  2001/03/28 01:19:46  gbader
Added option of keeping memo lists unique

Revision 1.8  2001/03/26 19:27:08  gbader
Fixed list order problem

Revision 1.7  2001/03/23 19:28:06  gbader
Fixed memory cross-linking error in memolist function

Revision 1.6  2001/03/08 00:11:13  gbader
Added comma separated list functions, deleteDB function and create path/database name function

Revision 1.5  2001/02/07 21:24:48  gbader
Moved remaining CodeBase functions to slri_cb.[ch].  No other lib file should ever include d4all.h

Revision 1.4  2001/01/13 04:04:10  gbader
Added IBM DB2 UDB to license exceptions.

Revision 1.3  2000/09/05 13:55:08  gbader
Updated License so that it is now GNU.
Added CVS keywords ID and Log as well as new line at end of files so compilers on some platforms don't complain

*/
