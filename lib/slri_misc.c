/* $Id: slri_misc.c,v 1.17 2005/03/14 19:27:45 ksnyder Exp $
-- ***************************************************************************
--
--  Samuel Lunenfeld Research Institute (SLRI)
--
--  File: slri_misc.c
--  Description: General and miscellaneous functions
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
#include <tsprintf.h>	/*for TSPRINTF macro*/
#include <utilpub.h>	/*for vnpHeapSort function*/

#define CHECKSUM_LENGTH (26L) /* bytes */

/*************************/
/*Miscellaneous Functions*/
/*************************/

/* Converts a Int4 Array of iArraysize size to a ValNodeList, maintains list order */
SLRI_ERR Misc_Int4ArrayToValNodeList(Int4Ptr piArray, Int4 iArraysize, ValNodePtr PNTR ppvnlist)
{
	Int4 i = 0;
	ValNodePtr vnpHead = NULL, vnpNode = NULL;

	for(i = 0; i < iArraysize; i++) {
		if((vnpNode = ValNodeAddInt(&vnpNode,0,piArray[i])) == NULL) {
			ValNodeFree(vnpHead);
			return SLRI_FAIL;
		}
		if(vnpHead == NULL) {
			vnpHead = vnpNode;
		}
	}
	*ppvnlist = vnpHead;
	return SLRI_SUCCESS;
}

/* Converts a ValNodeList of Integers to an Int4 array of piArraySize size, maintains list order*/
SLRI_ERR Misc_ValNodeListToInt4Array(ValNodePtr pvnlist, Int4Ptr PNTR ppiArray, Int4 PNTR piArraysize)
{
	Int4 len = 0, i = 0;
	Int4Ptr piArray = NULL;
	ValNodePtr pvnHere = NULL;
	
	if((pvnHere = pvnlist) == NULL) return SLRI_FAIL;
	len = ValNodeLen(pvnHere);
	if((piArray = (Int4Ptr) MemNew((size_t) sizeof(Int4) * len )) == NULL) return SLRI_FAIL;
	pvnHere = pvnlist;
	while (pvnHere) {
		piArray[i] = pvnHere->data.intvalue;
		i++;
		pvnHere = pvnHere->next;
	}

	*ppiArray = piArray;
	*piArraysize = i;
	return SLRI_SUCCESS;
}

/* Converts a FloatHi Array of iArraysize size to a ValNodeList, maintains list order */
SLRI_ERR Misc_FloatHiArrayToValNodeList(FloatHiPtr pfArray, Int4 iArraysize, ValNodePtr PNTR ppvnlist)
{
	Int4 i = 0;
	ValNodePtr vnpHead = NULL, vnpNode = NULL;

	for(i = 0; i < iArraysize; i++) {
		if((vnpNode = ValNodeAddFloat(&vnpNode,0,pfArray[i])) == NULL) {
			ValNodeFree(vnpHead);
			return SLRI_FAIL;
		}
		if(vnpHead == NULL) {
			vnpHead = vnpNode;
		}
	}
	*ppvnlist = vnpHead;
	return SLRI_SUCCESS;
}

/* Converts a ValNodeList of FloatHi to an Int4 array of piArraySize size, maintains list order*/
SLRI_ERR Misc_ValNodeListToFloatHiArray(ValNodePtr pvnlist, FloatHiPtr PNTR ppfArray, Int4 PNTR piArraysize)
{
	Int4 len = 0, i = 0;
	FloatHiPtr pfArray = NULL;
	ValNodePtr pvnHere = NULL;
	
	if((pvnHere = pvnlist) == NULL) return SLRI_FAIL;
	len = ValNodeLen(pvnHere);
	if((pfArray = (FloatHiPtr) MemNew((size_t) sizeof(FloatHi) * len )) == NULL) return SLRI_FAIL;
	pvnHere = pvnlist;
	while (pvnHere) {
		pfArray[i] = pvnHere->data.realvalue;
		i++;
		pvnHere = pvnHere->next;
	}

	*ppfArray = pfArray;
	*piArraysize = i;
	return SLRI_SUCCESS;
}

/* Converts a FloatLo Array of iArraysize size to a ValNodeList, maintains list order */
SLRI_ERR Misc_FloatLoArrayToValNodeList(FloatLoPtr pfArray, Int4 iArraysize, ValNodePtr PNTR ppvnlist)
{
	Int4 i = 0;
	ValNodePtr vnpHead = NULL, vnpNode = NULL;

	for(i = 0; i < iArraysize; i++) {
		if((vnpNode = ValNodeAddFloat(&vnpNode,0,(FloatHi)pfArray[i])) == NULL) {
			ValNodeFree(vnpHead);
			return SLRI_FAIL;
		}
		if(vnpHead == NULL) {
			vnpHead = vnpNode;
		}
	}
	*ppvnlist = vnpHead;
	return SLRI_SUCCESS;
}

/* Converts a ValNodeList of FloatLo to an Int4 array of piArraySize size, maintains list order*/
SLRI_ERR Misc_ValNodeListToFloatLoArray(ValNodePtr pvnlist, FloatLoPtr PNTR ppfArray, Int4 PNTR piArraysize)
{
	Int4 len = 0, i = 0;
	FloatLoPtr pfArray = NULL;
	ValNodePtr pvnHere = NULL;
	
	if((pvnHere = pvnlist) == NULL) return SLRI_FAIL;
	len = ValNodeLen(pvnHere);
	if((pfArray = (FloatLoPtr) MemNew((size_t) sizeof(FloatLo) * len )) == NULL) return SLRI_FAIL;
	pvnHere = pvnlist;
	while (pvnHere) {
		pfArray[i] = (FloatLo) pvnHere->data.realvalue;
		i++;
		pvnHere = pvnHere->next;
	}

	*ppfArray = pfArray;
	*piArraysize = i;
	return SLRI_SUCCESS;
}

/* Converts a CharPtr Array of iArraysize size to a ValNodeList, maintains list order */
SLRI_ERR Misc_CharPtrArrayToValNodeList(CharPtr *pcArray, Int4 iArraysize, ValNodePtr PNTR ppvnlist)
{
	Int4 i = 0;
	ValNodePtr vnpHead = NULL, vnpNode = NULL;

	for(i = 0; i < iArraysize; i++) {
		if((vnpNode = ValNodeCopyStr(&vnpNode,0,pcArray[i])) == NULL) {
			ValNodeFree(vnpHead);
			return SLRI_FAIL;
		}
		if(vnpHead == NULL) {
			vnpHead = vnpNode;
		}
	}
	*ppvnlist = vnpHead;
	return SLRI_SUCCESS;
}

/* Converts a ValNodeList of CharPtrs to an Int4 array of piArraySize size, maintains list order*/
SLRI_ERR Misc_ValNodeListToCharPtrArray(ValNodePtr pvnlist, CharPtr PNTR *ppcArray, Int4 PNTR piArraysize)
{
	Int4 len = 0, i = 0;
	CharPtr *pcArray = NULL;
	ValNodePtr pvnHere = NULL;
	
	if((pvnHere = pvnlist) == NULL) return SLRI_FAIL;
	len = ValNodeLen(pvnHere);
	if((pcArray = (CharPtr *) MemNew((size_t) sizeof(CharPtr) * len )) == NULL) return SLRI_FAIL;
	pvnHere = pvnlist;
	while (pvnHere) {
		pcArray[i] = StringSave((CharPtr) pvnHere->data.ptrvalue);
		i++;
		pvnHere = pvnHere->next;
	}

	*ppcArray = pcArray;
	*piArraysize = i;
	return SLRI_SUCCESS;
}

/*Generic: Converts a ValNode list of objects to a string of comma separated objects*/
CharPtr Misc_ValNodeListToString(ValNodePtr vnp, CharPtr (LIBCALLBACK *toString )PROTO ((ValNodePtr, Boolean *)))
{
	ByteStorePtr bsp=NULL;
	CharPtr string=NULL, tmpStr=NULL;
	Boolean free=FALSE;

	if(vnp==NULL) {
		ErrPostEx(SEV_ERROR, 0, 0, "Misc_ValNodeListToString: Passed vnp list was NULL.");
		return(NULL);
	}

	bsp = BSNew(10);	/*arbitrary set start size for bsp*/
	BSSeek(bsp, 0L, SEEK_SET);
	while(vnp != NULL) {
		/*convert node to a string*/
		tmpStr = toString(vnp, &free);
		BSWrite(bsp, tmpStr, StringLen(tmpStr));
		if(free) {
			tmpStr = MemFree(tmpStr);
			free=FALSE;
		}
		if(vnp->next != NULL) {
			BSWrite(bsp, ",", 1);
		}
		vnp = vnp->next;
	}
	string = BSStringSave(bsp);
	BSFree(bsp);

	return(string);
}

/*Misc_ValNodeListToString callback: Converts an integer to a string*/
static CharPtr LIBCALLBACK VnpNodeInt2Str(ValNodePtr vnp, Boolean *free)
{
	CharPtr tmpStr=NULL;

	/*uses NCBI static buffer*/
	*free=FALSE;
	tmpStr = Ltostr(vnp->data.intvalue, 0);

	return(tmpStr);
}

/*Misc_ValNodeListToString callback: Converts a string to a string*/
static CharPtr LIBCALLBACK VnpNodeStr2Str(ValNodePtr vnp, Boolean *free)
{
	CharPtr tmpStr=NULL;

	*free=FALSE;
	tmpStr = (CharPtr) vnp->data.ptrvalue;

	return(tmpStr);
}

/*Misc_ValNodeListToString callback: Converts a FloatHi to a string*/
static CharPtr LIBCALLBACK VnpNodeFloatHi2Str(ValNodePtr vnp, Boolean *free)
{
	CharPtr tmpStr=NULL;
	ByteStorePtr bsp=NULL;

	/*uses NCBI static buffer*/
	*free=TRUE;
	bsp = BSNew(10);	/*arbitrary set start size for bsp*/
	BSprintf(bsp, "%f", vnp->data.realvalue);
	tmpStr = BSStringSave(bsp);
	BSFree(bsp);

	return(tmpStr);
}

/*Saves a ValNode list of integers as a string of comma separated numbers
NOTE: allocates memory - free returned string*/
CharPtr Misc_SaveValNodeListAsString(ValNodePtr vnp)
{
	return(Misc_ValNodeListToString(vnp, VnpNodeInt2Str));
}

/*Converts a ValNode list of strings to a comma separated list of strings*/
CharPtr Misc_SaveStrValNodeListAsString(ValNodePtr vnp)
{
	return(Misc_ValNodeListToString(vnp, VnpNodeStr2Str));
}

/*Converts a ValNode list of FloatHis to a comma separated list of strings*/
CharPtr Misc_SaveFloatHiValNodeListAsString(ValNodePtr vnp)
{
	return(Misc_ValNodeListToString(vnp, VnpNodeFloatHi2Str));
}

/*converts a "sep" separated string of integers to a ValNode list of integers*/
/*Note: if maintainOrder=TRUE, sequence order will be maintained, but the function will be slower.*/
static ValNodePtr Misc_CommaIntString2ValNodeListEx(CharPtr string, CharPtr seps, Boolean maintainOrder)
{
	ValNodePtr vnpList=NULL;
	CharPtr stopstring=NULL, token=NULL, stringTmp=NULL;

	if((string==NULL)||(seps==NULL)) {
		ErrPostEx(SEV_ERROR, 0, 0, "Misc_CommaIntString2ValNodeList: Passed string or seps was NULL.");
		return(NULL);
	}

	/*copy string so that StringTok doesn't destroy the passed string*/
	stringTmp = StringSave(string);
	Mem_Check(stringTmp);

	token = StringTok(stringTmp, seps);
	while(token != NULL) {
		if(Misc_IsNumeric(token)) {
			if(maintainOrder) {
				ValNodeAddInt(&vnpList, 0, strtol(token, &stopstring, 10));
			}
			else {
				Slri_ValNodeAddInt(&vnpList, 0, strtol(token, &stopstring, 10));
			}
		}
		else {
			ErrPostEx(SEV_ERROR, 0, 0, "Misc_CommaIntString2ValNodeList: non numeric value present in list.");
			stringTmp = MemFree(stringTmp);
			vnpList = ValNodeFree(vnpList);
			return(NULL);
		}
		token = StringTok(NULL, seps);
	}

	stringTmp = MemFree(stringTmp);
	return(vnpList);
}

/*converts a "sep" separated string of integers to a ValNode list of integers*/
/*fast version*/
ValNodePtr Misc_CommaIntString2ValNodeList(CharPtr string, CharPtr seps)
{
	return(Misc_CommaIntString2ValNodeListEx(string, seps, FALSE));
}

/*converts a "sep" separated string of integers to a ValNode list of integers*/
/*slow version*/
ValNodePtr Misc_CommaIntString2ValNodeListOrdered(CharPtr string, CharPtr seps)
{
	return(Misc_CommaIntString2ValNodeListEx(string, seps, TRUE));
}

/* Converts a seps separated string to a ValNode list of strings */
/*Note: if maintainOrder=TRUE, sequence order will be maintained, but the function will be slower.*/
static ValNodePtr Misc_CommaStrString2ValNodeListEx(CharPtr string, CharPtr seps, Boolean maintainOrder)
{
	ValNodePtr vnpList=NULL;
	CharPtr token=NULL, stringTmp=NULL;

	if((string==NULL)||(seps==NULL)) {
		return(NULL);
	}

	/*copy string so that StringTok doesn't destroy the passed string*/
	stringTmp = StringSave(string);
	Mem_Check(stringTmp);

	token = StringTok(stringTmp, seps);
	while(token != NULL) {
		if(maintainOrder) {
			ValNodeAddStr(&vnpList, 0, StringSave(token));
		}
		else {
			Slri_ValNodeAddStr(&vnpList, 0, StringSave(token));
		}
		token = StringTok(NULL, seps);
	}

	stringTmp = MemFree(stringTmp);
	return(vnpList);
}

/*converts a "sep" separated string of strings to a ValNode list of strings*/
/*fast version*/
ValNodePtr Misc_CommaStrString2ValNodeList(CharPtr string, CharPtr seps)
{
	return(Misc_CommaStrString2ValNodeListEx(string, seps, FALSE));
}

/*converts a "sep" separated string of strings to a ValNode list of strings*/
/*slow version*/
ValNodePtr Misc_CommaStrString2ValNodeListOrdered(CharPtr string, CharPtr seps)
{
	return(Misc_CommaStrString2ValNodeListEx(string, seps, TRUE));
}


/*Adds an integer to a sorted valnode list*/
/*If unique is set, will not add to a list that already has the given integer*/
Int2 Misc_AddInt2SortedList(ValNodePtr *vnp, Int4 num, Int2 choice, Boolean unique)
{
	ValNodePtr vnpCur=NULL, vnpPrev=NULL, vnpNew=NULL;
	Int4 retVal=0;

	/*list is empty*/
	if((*vnp)==NULL) {
		ValNodeAddInt(vnp, choice, num);
	}
	else {
		/*list is not empty*/
		vnpCur = (*vnp);
		/*create new node*/
		ValNodeAddInt(&vnpNew, choice, num);
		while(vnpCur!=NULL) {
			retVal = vnpCur->data.intvalue - num;
			if(retVal>0) {
				/*current > num - add num before cur*/
				if(vnpPrev==NULL) {
					/*num must be at start of list*/
					vnpNew->next = vnpCur;
					(*vnp) = vnpNew;
					break;
				}
				else {
					vnpNew->next = vnpPrev->next;
					vnpPrev->next = vnpNew;
					break;
				}
			}
			else if(retVal<0) {
				/*current < num - we are at end of list, add num*/
				if(vnpCur->next==NULL) {
					vnpCur->next=vnpNew;
					vnpNew->next=NULL;
					break;
				}
			}
			else if(retVal==0) {
				/*num == cur - add before cur if unique flag not set*/
				if(unique) {
					vnpNew = ValNodeFree(vnpNew);
					return(1);
				}
				else {
					if(vnpPrev==NULL) {
						/*num must be at start of list*/
						vnpNew->next = vnpCur;
						(*vnp) = vnpNew;
						break;
					}
					else {
						vnpNew->next = vnpPrev->next;
						vnpPrev->next = vnpNew;
						break;
					}
				}
			}
			vnpPrev = vnpCur;
			vnpCur = vnpCur->next;
		}
	}

	return(0);
}

/*Removes an integer from a sorted valnode list*/
/*If there are duplicate integers, only one will be removed*/
Int2 Misc_RemoveIntFromSortedList(ValNodePtr *vnp, Int4 num)
{
	ValNodePtr vnpCur=NULL, vnpPrev=NULL;

	if((*vnp)==NULL) {
		return(1);
	}

	/*check if element is in list*/
	if(!Misc_IsIntInList((*vnp), num)) {
		return(1);
	}

	/*list is not empty*/
	vnpCur = (*vnp);
	while(vnpCur!=NULL) {
		if(vnpCur->data.intvalue == num) {
			/*ints are equal, remove current element and end loop*/
			/*start of list*/
			if(vnpPrev==NULL) {
				(*vnp) = vnpCur->next;
				vnpCur->next=NULL;
				vnpCur = ValNodeFree(vnpCur);
			}
			else {
				/*not start of list*/
				vnpPrev->next = vnpCur->next;
				vnpCur->next=NULL;
				vnpCur = ValNodeFree(vnpCur);
			}
			break;
		}
		vnpPrev=vnpCur;
		vnpCur=vnpCur->next;
	}

	return(0);
}

/*Adds an integer to the end of a non-sorted valnode list, but maintains uniqueness of num*/
/*returns 0 if the integer was added, -1 if not and for errors*/
Int2 Misc_AddInt2UniqueList(ValNodePtr *vnpList, Int2 choice, Int4 num)
{
	ValNodePtr vnp=NULL, vnpPrev=NULL;

	if(vnpList==NULL) {
		return(-1);
	}

	vnp = *vnpList;

	if(vnp==NULL) {
		ValNodeAddInt(vnpList, choice, num);
		return(0);
	}

	while(vnp!=NULL) {
		if(vnp->data.intvalue == num) {
			return(-1);
		}
		vnpPrev = vnp;
		vnp = vnp->next;
	}

	/*if we get here, num is not in list and we can add it*/
	ValNodeAddInt(&vnpPrev, choice, num);

	return(0);
}

/*Adds an integer to the end of a non-sorted valnode list, but maintains uniqueness of num and choice*/
/*returns 0 if the integer and choice was added, -1 if not and for errors*/
Int2 Misc_AddIntChoice2UniqueListByNumAndChoice(ValNodePtr *vnpList, Int2 choice, Int4 num)
{
	ValNodePtr vnp=NULL, vnpPrev=NULL;

	if(vnpList==NULL) {
		return(-1);
	}

	vnp = *vnpList;

	if(vnp==NULL) {
		ValNodeAddInt(vnpList, choice, num);
		return(0);
	}

	while(vnp!=NULL) {
		if((vnp->data.intvalue == num)&&(vnp->choice==choice)) {
			return(-1);
		}
		vnpPrev = vnp;
		vnp = vnp->next;
	}

	/*if we get here, num is not in list and we can add it*/
	ValNodeAddInt(&vnpPrev, choice, num);

	return(0);
}

/* Adds a string to a unique DValNode */
void Misc_AddString2DVnpUnique(DValNodePtr *vnp, Int4 choice, CharPtr str)
{
	DValNodePtr index = NULL;

	if(str == NULL)
		return;

	if(*vnp == NULL) {
		DValNodeAddStr(vnp,(Int2) choice,str);
		return;
	}

	index = (*vnp);

	while(index != NULL) {
		if(StringCmp((CharPtr)index->data.ptrvalue,str)==0)
			return;
		index = index->next;
	}
	DValNodeAddStr(vnp,(Int2) choice,str);
}

/*returns TRUE if query string is in the given sorted list - NULL terminated.*/
Boolean Misc_IsStringInList(CharPtr sortedList[], CharPtr query)
{
	Boolean retVal=FALSE;
	Int4 i=0, strcmpRet=0;

	if((sortedList==NULL)||(query==NULL)) {
		return(FALSE);
	}

	for(i=0;sortedList[i] != NULL;i++) {
		if((strcmpRet = StringCmp(sortedList[i], query)) == 0) {
			retVal = TRUE;
			break;
		}
		if(strcmpRet > 0) { /*since list is sorted, stop if query is smaller than list element*/
			break;
		}
	}

	return(retVal);
}

/*checks if a word is in an ascending sorted list of words*/
/*Return values: -2 parameter error, -1 not found, n>=0 index of found word*/
Int4 Misc_BinaryWordSearch(CharPtr word, CharPtr sortedWordList[], Int4 listLen, Boolean caseSensitive)
{
	/*this function implements typical iterative binary search*/
	Int4 left=0, mid=0, right=listLen-1;
	Int2 strCmpRc=0;

	if((word==NULL)||(sortedWordList==NULL)) {
		return(-2);
	}

	while(left<=right) {
		mid=(left+right)/2;
		if(caseSensitive) {
			strCmpRc = (Int2) StringCmp(word, sortedWordList[mid]);
		}
		else {
			strCmpRc = (Int2) StringICmp(word, sortedWordList[mid]);
		}
		if(strCmpRc==0) {
			return(mid);
		}
		else if(strCmpRc>0) {
			left=mid+1;
		}
		else {
			right=mid-1;
		}
	}

	return(-1);
}

/*find the index of name in a string array of numElmts - returns -1 in case of failure*/
Int4 Misc_IndexCharPtrArray(CharPtr array[], Int4 numElmts, CharPtr name)
{
	Int4 i=0;
	Boolean found=FALSE;

	for(i=0;i<numElmts;i++) {
		if((array[i]!=NULL)&&(strcmp(array[i], name)==0)) {
			found=TRUE;
			break;
		}
	}

	if(found)
		return(i);

	return(-1);
}

/*Find an integer in an integer list*/
ValNodePtr Misc_FindIntInIntList(ValNodePtr list, Int4 num)
{
	if(list==NULL) {
		return(FALSE);
	}

	while(list!=NULL) {
		if(list->data.intvalue == num) {
			return(list);
		}
		list = list->next;
	}

	return(NULL);
}

/*returns TRUE if an integer is in the given number list.*/
Boolean Misc_IsIntInList(ValNodePtr list, Int4 num)
{
	if(Misc_FindIntInIntList(list, num)!=NULL) {
		return(TRUE);
	}

	return(FALSE);
}

/*returns TRUE if a string is in the given ValNode string list - optionally case sensitive.*/
Boolean Misc_IsStrInStrVnpList(ValNodePtr list, CharPtr string, Boolean caseSensitive)
{
	if(list==NULL) {
		return(FALSE);
	}

	while(list!=NULL) {
		if(caseSensitive) {
			if(StringCmp((CharPtr) list->data.ptrvalue, string) == 0) {
				return(TRUE);
			}
		}
		else {
			if(StringICmp((CharPtr) list->data.ptrvalue, string) == 0) {
				return(TRUE);
			}
		}
		list = list->next;
	}

	return(FALSE);
}

/*returns TRUE if an integer with given choice is in the given integer list.*/
Boolean Misc_IsIntChoiceInList(ValNodePtr list, Int4 num, Int2 choice)
{
	if(list==NULL) {
		return(FALSE);
	}

	while(list!=NULL) {
		if((list->data.intvalue == num)&&(choice==list->choice)) {
			return(TRUE);
		}
		list = list->next;
	}

	return(FALSE);
}

/*returns 1 if string is equivalent to TRUE
  returns 0 if string is equivalent to FALSE
  returns -1 if string is equivalent to neither*/
Int2 Misc_IsStringTrueOrFalse(CharPtr string)
{
	Int2 i=0;
	CharPtr yes[]= {"y", "yes", "t", "true", "affirmative", NULL};
	CharPtr no[] = {"n", "no", "f", "false", "negative", NULL};

	if(string==NULL)
		return(-1);

	/*check this against the array*/
	while(yes[i] != NULL) {
		if(StringICmp(yes[i], string) == 0)
			return 1;
		i++;
	}
	i=0;
	while(no[i] != NULL) {
		if(StringICmp(no[i], string) == 0)
			return 0;
		i++;
	}
	/*nothing found from yes,no lists*/
	return (-1);
}

/*remove a vnp from a list
IMPORTANT USAGE EXAMPLE: (Make sure to catch the return value)
head = Misc_RemoveNode(head, x);*/
ValNodePtr Misc_RemoveNode(ValNodePtr head, ValNodePtr x)
/*from utilpub.c*/
{
	ValNodePtr	v=NULL, p=NULL;

	if (head == NULL) {
		return NULL;
	}
	if (x == head) {
		head = x->next;
		x->next = NULL;
		ValNodeFree(x);
		return head;
	}
	for (v = head; v != NULL && v != x; v = v->next) {
		p = v;
	}
	if (v != NULL) {
		p->next = x->next;
		x->next = NULL;
		ValNodeFree(x);
	}
	return head;
}

/*tests if a given string is numeric - integer*/
Boolean Misc_IsNumeric(CharPtr string)
{
	Int4 i=0;
	Boolean isNumeric=TRUE;

	if(string==NULL)
		return(FALSE);

	while(string[i] != '\0') {
		if(!((string[i]>='0')&&(string[i]<='9'))) {
			if((i==0)&&(string[i]=='-')) {}	/*negative sign at start is OK*/
			else {
				isNumeric=FALSE;
				break;
			}
		}
		i++;
	}

	return(isNumeric);
}

/*tests if a given string is numeric - real number*/
Boolean Misc_IsFloatNumber(CharPtr string)
{
	Int4 i=0;
	Boolean isNumeric=TRUE, dotSeen=FALSE;

	if(string==NULL)
		return(FALSE);

	while(string[i] != '\0') {
		if(!((string[i]>='0')&&(string[i]<='9'))) {
			if((i==0)&&(string[i]=='-')) {}	/*negative sign at start is OK*/
			else if ((i>0)&&(string[i]=='.')&&(!dotSeen)) {	/*decimal point is OK*/
				dotSeen=TRUE;
			}
			else {
				isNumeric=FALSE;
				break;
			}
		}
		i++;
	}

	return(isNumeric);
}

/*callback function for vnpHeapSort - numerical ascending sort*/
int LIBCALLBACK Misc_NumSortAscendCallBack(Nlm_VoidPtr vp1, Nlm_VoidPtr vp2)
{
	int status;
	ValNodePtr vnp1, vnp2;
	ValNodePtr PNTR vnpp1;
	ValNodePtr PNTR vnpp2;

	Int4 num1, num2;

	vnpp1 = (ValNodePtr PNTR) vp1;
	vnpp2 = (ValNodePtr PNTR) vp2;
	vnp1 = *vnpp1;
	vnp2 = *vnpp2;

	num1 = vnp1->data.intvalue;
	num2 = vnp2->data.intvalue;

	status = (int)(num1 - num2);

	return status;
}

/*callback function for vnpHeapSort - string ascending sort*/
int LIBCALLBACK Misc_StringSortAscendCallBack(Nlm_VoidPtr vp1, Nlm_VoidPtr vp2)
{
	int status;
	ValNodePtr vnp1, vnp2;
	ValNodePtr PNTR vnpp1;
	ValNodePtr PNTR vnpp2;

	CharPtr pc1, pc2;

	vnpp1 = (ValNodePtr PNTR) vp1;
	vnpp2 = (ValNodePtr PNTR) vp2;
	vnp1 = *vnpp1;
	vnp2 = *vnpp2;

	pc1 = vnp1->data.ptrvalue;
	pc2 = vnp2->data.ptrvalue;

	status = (int)StringCmp(pc1, pc2);

	return status;
}

/*String memory free callback function for Misc_VnpUniqueEx*/
VoidPtr LIBCALLBACK Misc_StringFreeCallBack(VoidPtr ptr)
{
	return(MemFree(ptr));
}

/*generically uniques a vnp list - O(n2log(n))*/
static void Misc_VnpUniqueEx(ValNodePtr *vnp, int (LIBCALLBACK *compar )PROTO ((VoidPtr, VoidPtr )),
							 VoidPtr (LIBCALLBACK *free )PROTO ((VoidPtr)))
{
	ValNodePtr vnpTmp=NULL, vnpHead=NULL, vnpCur=NULL, vnpNext=NULL;

	if((*vnp)==NULL) {
		ErrPostEx(SEV_INFO, 1, 0, "Misc_VnpUniqueEx: Invalid type passed.");
		return;
	}

	/*return if only one node in the list*/
	if((*vnp)->next == NULL) {
		return;
	}

	/*sort*/
	VnpHeapSort(vnp, compar);
	vnpHead = (*vnp);
	/*unique*/
	vnpCur = vnpHead;
	vnpNext = vnpHead->next;
	while((*vnp) != NULL) {
		if(compar(&vnpCur, &vnpNext)==0) {
			vnpTmp = (*vnp);
			vnpCur = (*vnp) = (*vnp)->next;
			/*if free function defined, use it*/
			if(free!=NULL) {
				vnpTmp->data.ptrvalue = free(vnpTmp->data.ptrvalue);
			}
			vnpHead = Misc_RemoveNode(vnpHead, vnpTmp);
			if((*vnp) == NULL) {
				break;
			}
			if((*vnp)->next != NULL) {
				vnpNext = (*vnp)->next;
			}
			else {
				break;
			}
		}
		else {
			(*vnp) = (*vnp)->next;
			if((*vnp) != NULL) {
				vnpCur = (*vnp);
				if((*vnp)->next != NULL) {
					vnpNext = (*vnp)->next;
				}
				else {
					break;
				}
			}
		}
	}
	(*vnp) = vnpHead;
}

/*uniques a vnp filled with integers*/
void Misc_IntVnpUnique(ValNodePtr *vnp)
{
	Misc_VnpUniqueEx(vnp, Misc_NumSortAscendCallBack, NULL);
}


/*uniques a vnp filled with strings*/
void Misc_StringVnpUnique(ValNodePtr *vnp)
{
	Misc_VnpUniqueEx(vnp, Misc_StringSortAscendCallBack, NULL);
}

/*uniques a vnp filled with strings and frees the associated string of any deleted list nodes*/
void Misc_StringVnpUniqueAndFree(ValNodePtr *vnp)
{
	Misc_VnpUniqueEx(vnp, Misc_StringSortAscendCallBack, Misc_StringFreeCallBack);
}

/*generically uniques a vnp list, but maintains order - 0(n2)*/
static void Misc_VnpUniqueOrderedEx(ValNodePtr *vnp, int (LIBCALLBACK *compar )PROTO ((VoidPtr, VoidPtr )),
							 VoidPtr (LIBCALLBACK *free )PROTO ((VoidPtr)))
{
	ValNodePtr vnpTmp=NULL, vnpHead=NULL, vnpTmp2=NULL;

	if((*vnp)==NULL) {
		ErrPostEx(SEV_INFO, 1, 0, "Misc_VnpUniqueOrderedEx: Invalid type passed.");
		return;
	}

	/*return if only one node in the list*/
	if((*vnp)->next == NULL) {
		return;
	}

	/*unique*/
	vnpTmp = vnpHead = *vnp;
	while(vnpTmp!=NULL) {
		vnpTmp2 = vnpTmp->next;
		while(vnpTmp2!=NULL) {
			/*look through all nodes after this one*/
			if(compar(&vnpTmp, &vnpTmp2)==0) {
				/*nodes are equal*/
				/*if free function defined, use it*/
				if(free!=NULL) {
					vnpTmp2->data.ptrvalue = free(vnpTmp2->data.ptrvalue);
				}
				vnpTmp = Misc_RemoveNode(vnpTmp, vnpTmp2);
				break;
			}
			vnpTmp2=vnpTmp2->next;
		}
		vnpTmp=vnpTmp->next;
	}
}

/*uniques a vnp filled with integers, but maintains order*/
void Misc_IntVnpUniqueOrdered(ValNodePtr *vnp)
{
	Misc_VnpUniqueOrderedEx(vnp, Misc_NumSortAscendCallBack, NULL);
}

/*uniques a vnp filled with strings, but maintains order*/
void Misc_StringVnpUniqueOrdered(ValNodePtr *vnp)
{
	Misc_VnpUniqueOrderedEx(vnp, Misc_StringSortAscendCallBack, NULL);
}

/*uniques a vnp filled with strings, but maintains order and frees the associated string of any deleted list nodes*/
void Misc_StringVnpUniqueOrderedAndFree(ValNodePtr *vnp)
{
	Misc_VnpUniqueOrderedEx(vnp, Misc_StringSortAscendCallBack, Misc_StringFreeCallBack);
}

/* filters out all entries in the valnode with the specified choice and returns a new valnode*/
ValNodePtr Misc_FilterValNodeByChoice(ValNodePtr vnpHead, Int2 choice)
{
	ValNodePtr vnp=NULL, vnpResult=NULL;

	vnp = vnpHead;

	while(vnp != NULL) 
	{
		if(vnp->choice == choice) {
			ValNodeAddInt(&vnpResult,vnp->choice,vnp->data.intvalue);
		}
		vnp = vnp->next;
	}
	return vnpResult;
}

/*returns the number of nodes in a ValNode linked list*/
Int4 Misc_CountVnp(ValNodePtr vnp)
{
	Int4 i=0;

	if(vnp==NULL) {
		return(0);
	}

	while(vnp != NULL) {
		i++;
		vnp=vnp->next;
	}

	return(i);
}

/*given a seq-loc, returns a sequence # string - opposite of SeqString2SeqLoc
NOTE: allocates memory! - take care of CharPtr returned!*/
CharPtr Misc_SeqLoc2SeqString(SeqLocPtr seqLoc)
{
	CharPtr sequence=NULL;
	ValNodePtr vnp=NULL;
	SeqPntPtr spp=NULL;
	SeqIntPtr sip=NULL;
	ByteStorePtr bsp=NULL;

	if(seqLoc==NULL) {
		return NULL;
	}
	if(seqLoc->choice != SEQLOC_MIX) {
		return NULL;
	}

	bsp = BSNew(32);
	BSSeek(bsp, 0L, SEEK_SET);
	vnp=seqLoc->data.ptrvalue;

	while(vnp != NULL) {
		if(vnp->choice == SEQLOC_INT) {
			sip=vnp->data.ptrvalue;
			BSWrite(bsp, Ltostr(sip->from, 0), StringLen(Ltostr(sip->from, 0)));
			BSWrite(bsp, "-", 1);
			BSWrite(bsp, Ltostr(sip->to, 0), StringLen(Ltostr(sip->to, 0)));
		}
		else if(vnp->choice == SEQLOC_PNT) {
			spp=vnp->data.ptrvalue;
			BSWrite(bsp, Ltostr(spp->point, 0), StringLen(Ltostr(spp->point, 0)));
		}
		vnp = vnp->next;
		if((vnp!=NULL)&&((vnp->choice == SEQLOC_PNT)||(vnp->choice == SEQLOC_INT))) {
			BSWrite(bsp, ",", 1);
		}
	}

	BSSeek(bsp, 0L, SEEK_SET);
	sequence = MemNew((size_t) BSLen(bsp)+1);
	Mem_Check(sequence);
	BSRead(bsp, sequence, BSLen(bsp)+1);
	sequence[BSLen(bsp)]='\0';	/*null terminate*/
	bsp = BSFree(bsp);

	return(sequence);
}

/*tests if a given string contains a dash*/
static Boolean Misc_ContainsInternalDash(CharPtr string)
{
	Int4 i=1; /*skip first character*/
	Boolean hasDash=FALSE;

	if(string == NULL)
		return(FALSE);

	while(string[i] != '\0') {
		if(string[i] == '-') {
			hasDash=TRUE;
			break;
		}
		i++;
	}

	return(hasDash);
}

/*parses input text in the form number1-number2*/
/*usage Misc_ParseInterval(string, number1, number2);
number entries should be a character buffer*/
static Boolean Misc_ParseInterval(CharPtr string, Int4 *from, Int4 *to)
{
	Int4 i=0;
	CharPtr stopstring=NULL, number1=NULL, number2=NULL;
	Int2 numLength1=0, numLength2=0;

	if(string==NULL)
		return(FALSE);

	number1 = MemNew((size_t) 128*sizeof(Char));
	Mem_Check(number1);
	number2 = MemNew((size_t) 128*sizeof(Char));
	Mem_Check(number2);

	/*copy 1st number to 'number1'*/
	while(string[i] != '-') {
		number1[numLength1] = string[i];
		numLength1++;
		i++;
	}
	number1[numLength1]='\0';	/*terminate string*/
	i++;	/*skip dash*/
	/*copy 2nd number to 'number2'*/
	while(string[i] != '\0') {
		number2[numLength2] = string[i];
		numLength2++;
		i++;
	}
	number2[numLength2]='\0';	/*terminate string*/

	if((!Misc_IsNumeric(number1))||(!Misc_IsNumeric(number2))) {
		number1 = MemFree(number1);
		number2 = MemFree(number2);
		return(FALSE);
	}

	(*from) = strtol(number1, &stopstring, 10);
	(*to) = strtol(number2, &stopstring, 10);

	number1=MemFree(number1);
	number2=MemFree(number2);

	/*make sure interval is smaller number to bigger number*/
	if((*from) >= (*to)) {
		return(FALSE);
	}

	return(TRUE);
}

/*given a sequence string e.g. 3,14-45,58 produce a seq-loc containing them*/
SeqLocPtr Misc_SeqString2SeqLoc(CharPtr stringInput, Int4 sequenceGi)
{
	ValNodePtr seqLoc=NULL;
	SeqPntPtr spp=NULL;
	SeqIntPtr sip=NULL;
	Int4 from=0, to=0;
	CharPtr token=NULL, stopstring=NULL;

	if(stringInput == NULL)
		return(NULL);

	seqLoc = ValNodeNew(NULL);
	seqLoc->choice = SEQLOC_MIX;

	/*tokenize input*/
	token = StringTok(stringInput, ", \t");
	while(token != NULL) {
		if(Misc_ContainsInternalDash(token)) {
			/*this token specifies a sequence interval*/
			if(Misc_ParseInterval(token, &from, &to)) {
				sip = SeqIntNew();
				sip->from = from;
				sip->to = to;
				ValNodeAddInt(&(sip->id), SEQID_GI, sequenceGi);
				ValNodeAddPointer((ValNodePtr *)&(seqLoc->data.ptrvalue), SEQLOC_INT, sip);
			}
			else {
				seqLoc = SeqLocFree(seqLoc);
				return(NULL);
			}
		}
		else if(Misc_IsNumeric(token)) {
			spp = SeqPntNew();
			spp->point = strtol(token, &stopstring, 10);
			ValNodeAddInt(&(spp->id), SEQID_GI, sequenceGi);
			ValNodeAddPointer((ValNodePtr *)&(seqLoc->data.ptrvalue), SEQLOC_PNT, spp);
		}
		else {
			seqLoc = SeqLocFree(seqLoc);
			return(NULL);
		}
		token = StringTok(NULL, ", ");
	}

	return(seqLoc);
}

/*Checks an e-mail address to see it it's formatted in name@server.ext*/
/*modified from unknown code snippet HTML page and toolkit'ized*/
Boolean Misc_GoodEmailAddress(CharPtr email)
{
	if(email==NULL) {
		return(FALSE);
	}

	if(StringChr(email, '@') == NULL) {
		return FALSE;
	}

	if((email[StringLen(email) - 1] == '@') || (email[StringLen(email) - 1] == '.')) {
		return FALSE;
	}
	if (StringChr(StringChr(email, '@'), '.') == NULL) {
		return FALSE;
	}

	if(StringChr(email, ' ') != NULL) {
		return FALSE;
	}

	if(email[0] == '@' || email[0] == '.') {
		return FALSE;
	}

	return TRUE;
}

/*Checks a phone number to see if it's formatted properly*/
Boolean Misc_GoodPhoneNumber(CharPtr number)
{
	Int2 strlength=0, count=0, i=0;

	if(number==NULL) {
		return(FALSE);
	}

	strlength=(Int2)StringLen(number);

	/*support international phone numbers, check for at least 10 digits (require area code)*/
	for(i=0;i<strlength;i++) {
		if(IS_DIGIT(number[i])) {
			count++;
		}
	}
	if(count<10) {
		return(FALSE);
	}
	/*TODO: other validation?*/

	return(TRUE);
}

/*Checks an IP address to see if it's formatted properly*/
Boolean Misc_GoodIPAddress(CharPtr address)
{
	CharPtr addressSave=NULL, tmpStr=NULL;
	Uint1 i=0;
	Int4 num=0;
	Boolean containsAlphas=FALSE;

	if(address==NULL) {
		return(FALSE);
	}

	addressSave = address;

	/*check if string contains alphas*/
	while(address[0]!='\0') {
		if(IS_ALPHA(address[0])) {
			containsAlphas=TRUE;
			break;
		}
		address++;
	}
	address = addressSave;

	/*test if it's an IP address*/
	if(containsAlphas) {
		return(FALSE);
	}
	else {
		/*count dots*/
		while(address[0]!='\0') {
			if(address[0]=='.') {
				i++;
			}
			address++;
		}
		if(i!=3) {
			return(FALSE);
		}
		/*check if all chars are digits and dots*/
		address = addressSave;
		while(address[0]!='\0') {
			if(!((IS_DIGIT(address[0]))||(address[0]=='.'))) {
				return(FALSE);
			}
			address++;
		}
		/*check if numbers are <= 255*/
		address = addressSave;
		while((tmpStr = strchr(address, '.'))!=NULL) {
			tmpStr[0]='\0';
			num=atol(address);
			if(num>255) {
				return(FALSE);
			}
			tmpStr[0]='.';
			address=tmpStr+1;
		}
		/*check last number*/
		num=atol(address);
		if(num>255) {
			return(FALSE);
		}
	}

	return(TRUE);
}

/*Copy an Int8 to a String for printf, ErrpostEx functions */
SLRI_ERR Int8toStr(Int8 num, CharPtr pcbuf)
{				
#ifdef OS_MSWIN								
	sprintf(pcbuf,"%I64d",(__int64)num);
#else		
#ifndef LONG_BIT
#define LONG_BIT 0
	if(LONG_BIT == 64)
		sprintf(pcbuf, "%ld", (long) num);
	else						
#endif										
	sprintf(pcbuf, "%lld", (long long) num);
#endif		
	return SLRI_SUCCESS;
}

/*Copy a String to an Int8 for scanf */
int StrtoInt8(CharPtr pcbuf, Int8 PNTR inum)
{	
	int err;
#ifdef OS_MSWIN								
	__int64 num;
#else
#if LONG_BIT == 64
	long num;
#else
	long long num;
#endif
#endif

	num=0;
#ifdef OS_MSWIN								
	err = sscanf(pcbuf,"%I64d", (__int64 *) &num);
#else		
#ifndef LONG_BIT
#define LONG_BIT 0
	if(LONG_BIT == 64)
		err = sscanf(pcbuf, "%ld", (long *) &num);
	else						
#endif										
	err = sscanf(pcbuf, "%lld", (long long *) &num);
#endif		
	*inum = (Int8) num;
	return err;
}

/* Generates a linked list of ValNodePtr sublists according to specified length.  InList is set to NULL by this call! */
SLRI_ERR Misc_PartitionValNodeList(ValNodePtr PNTR vnppInList, ValNodePtr PNTR vnppOutList, Int4 length)
{
	Int4 i = 0;
	ValNodePtr vnpList = NULL, vnpOutList = NULL, vnpLast = NULL;
	
	if(vnppInList == NULL || *vnppInList == NULL || vnppOutList == NULL || length <= 0) {
		ErrPostEx(SEV_ERROR,0,0,"Invalid parameters\n");
		return SLRI_FAIL;
	}

	vnpList = *vnppInList;
	*vnppInList = NULL;
	ValNodeAddPointer(&vnpOutList, 0,vnpList);
	while(vnpList != NULL) {
		for(i = 0; i < length; i++) {
			vnpLast = vnpList;
			vnpList = vnpList->next;
			if(vnpList == NULL) break;
		}
		vnpLast->next = NULL;
		if(vnpList != NULL) ValNodeAddPointer(&vnpOutList, 0, vnpList);
	}

	*vnppOutList = vnpOutList;
	return SLRI_SUCCESS;
}


/*convert a string of format e.g. 3,14-45,58 to a valnode with specialized choice values*/
ValNodePtr Misc_IntervalString2ValNode(CharPtr string)
{
	ValNodePtr vnpInterval=NULL;
	Int4 from=0, to=0;
	CharPtr token=NULL, stopstring=NULL, stringCopy=NULL;

	if(string==NULL) {
		return(NULL);
	}

	/*copy string to avoid side effects of strtok function*/
	stringCopy = StringSave(string);
	Mem_Check(stringCopy);

	/*tokenize input*/
	token = StringTok(stringCopy, ", \t");
	while(token != NULL) {
		if(Misc_ContainsInternalDash(token)) {
			/*this token specifies an interval*/
			if(Misc_ParseInterval(token, &from, &to)) {
				ValNodeAddInt(&vnpInterval, INTERVAL_FROM, from);
				ValNodeAddInt(&vnpInterval, INTERVAL_TO, to);
			}
			else {
				/*malformed interval string*/
				vnpInterval = ValNodeFree(vnpInterval);
				return(NULL);
			}
		}
		else if(Misc_IsNumeric(token)) {
			ValNodeAddInt(&vnpInterval, INTERVAL_INDIVIDUAL, strtol(token, &stopstring, 10));
		}
		else {
			/*malformed interval string*/
			vnpInterval = ValNodeFree(vnpInterval);
			return(NULL);
		}
		token = StringTok(NULL, ", \t");
	}
	stringCopy = MemFree(stringCopy);

	return(vnpInterval);
}

/*gets the next element from an integer interval valnode list - free returned valnode*/
ValNodePtr Misc_IntervalValNodeNextElement(ValNodePtr *vnpIntervalList)
{
	ValNodePtr vnpListInternal=NULL, vnpElement=NULL;

	if((*vnpIntervalList)==NULL) {
		return(NULL);
	}

	vnpListInternal=(*vnpIntervalList);

	/*validate if interval*/
	if(vnpListInternal->choice==INTERVAL_FROM) {
		if(vnpListInternal->next==NULL) {
			return(NULL);
		}
		if(vnpListInternal->next->choice != INTERVAL_TO) {
			return(NULL);
		}
		if(vnpListInternal->data.intvalue >= vnpListInternal->next->data.intvalue) {
			return(NULL);
		}
	}

	if(vnpListInternal->choice==INTERVAL_INDIVIDUAL) {
		vnpElement=vnpListInternal;
		vnpListInternal=vnpListInternal->next;
	}
	else if(vnpListInternal->choice==INTERVAL_FROM) {
		if((vnpListInternal->data.intvalue + 1) == vnpListInternal->next->data.intvalue) {
			/*from + 1 = to*/
			vnpElement=vnpListInternal;
			vnpListInternal=vnpListInternal->next;
			vnpListInternal->choice=INTERVAL_INDIVIDUAL;
		}
		else {
			/*from is 1 or more away from to*/
			ValNodeAddInt(&vnpElement, vnpListInternal->choice, vnpListInternal->data.intvalue);
			vnpListInternal->data.intvalue++;
		}
	}
	else if(vnpListInternal->choice==INTERVAL_TO) {
		/*error - this function should never see an INTERVAL_TO in the current node*/
		return(NULL);
	}

	(*vnpIntervalList)=vnpListInternal;
	vnpElement->next=NULL;
	return(vnpElement);
}

/*copy an integer ValNodePtr list*/
ValNodePtr Misc_ValNodeIntegerCopy(ValNodePtr vnp)
{
	ValNodePtr vnpNew=NULL, vnpNewHead=NULL;

	if(vnp==NULL) {
		ErrPostEx(SEV_INFO, 1, 0, "Misc_ValNodeIntegerCopy: Passed ValNodePtr is null.");
		return(NULL);
	}

	while(vnp!=NULL) {
		vnpNew = ValNodeAddInt(&vnpNew, vnp->choice, vnp->data.intvalue);
		if(vnpNewHead==NULL) {
			vnpNewHead = vnpNew;
		}
		vnp = vnp->next;
	}

	return(vnpNewHead);
}

/*copy a string ValNodePtr list*/
ValNodePtr Misc_ValNodeStringListCopy(ValNodePtr vnp)
{
	ValNodePtr vnpNew=NULL, vnpNewHead=NULL;

	if(vnp==NULL) {
		ErrPostEx(SEV_INFO, 1, 0, "Misc_ValNodeStringListCopy: Passed ValNodePtr is null.");
		return(NULL);
	}

	while(vnp!=NULL) {
		vnpNew = ValNodeCopyStr(&vnpNew, 0, vnp->data.ptrvalue);
		if(vnpNewHead==NULL) {
			vnpNewHead = vnpNew;
		}
		vnp = vnp->next;
	}

	return(vnpNewHead);
}

/*generically adds a pointer to the end of any linked list that contains a next field at the top of the structure*/
Int2 Misc_GenericAddObjectToListEnd(Pointer *list, Pointer addObj)
{
	SlriGenericLinkListPtr sgllp=NULL;

	if(list==NULL) {
		ErrPostEx(SEV_INFO, 1, 0, "Misc_GenericAddObjectToListEnd: Address of passed list is null (you must pass a variable by reference).");
		return(1);
	}
	if(addObj==NULL) {
		ErrPostEx(SEV_INFO, 1, 0, "Misc_GenericAddObjectToListEnd: Passed object to add is null.");
		return(1);
	}

	/*add to end of list*/
	if((*list) == NULL) {
		*list = addObj;
	}
	else {
		sgllp=(SlriGenericLinkListPtr)(*list);
		while(sgllp->next!=NULL) {
			sgllp=sgllp->next;
		}
		/*sgllp is now last node of list*/
		sgllp->next=addObj;
	}

	return(0);
}

/*generically adds a pointer to the head of any linked list that contains a next field at the top of the structure*/
Int2 Misc_GenericAddObjectToListStart(Pointer *list, Pointer addObj)
{
	SlriGenericLinkListPtr sgllp=NULL, sgllpTmp=NULL;

	if(list==NULL) {
		ErrPostEx(SEV_INFO, 1, 0, "Misc_GenericAddObjectToListStart: Address of passed list is null (you must pass a variable by reference).");
		return(1);
	}
	if(addObj==NULL) {
		ErrPostEx(SEV_INFO, 1, 0, "Misc_GenericAddObjectToListStart: Passed object to add is null.");
		return(1);
	}

	/*add to start of list*/
	if((*list)==NULL) {
		*list = addObj;
	}
	else {
		sgllp=(SlriGenericLinkListPtr)(*list);
		*list = addObj;
		sgllpTmp = (SlriGenericLinkListPtr)(*list);
		sgllpTmp->next = sgllp;
	}

	return(0);
}

/*generically removes a node from any linked list that contains a next field at the top the the node structure (e.g. ASN.1 generated code)*/
SLRI_ERR Misc_GenericRemoveObjectFromList(Pointer *listHead, Pointer removeObj, AsnOptFreeFunc yourObjectFreeFunc)
{
	SlriGenericLinkListPtr	v=NULL, p=NULL, x=NULL, head=NULL;

	head = *listHead;
	if(head == NULL) {
		return SLRI_FAIL;
	}

	x = removeObj;
	if(x == head) {
		head = x->next;
		x->next = NULL;
		yourObjectFreeFunc(x);
		*listHead=head;
		return SLRI_SUCCESS;
	}
	for(v = head; v != NULL && v != x; v = v->next) {
		p = v;
	}
	if(v != NULL) {
		p->next = x->next;
		x->next = NULL;
		yourObjectFreeFunc(x);
	}

	*listHead=head;
	return SLRI_SUCCESS;
}

/*generically determines the number of objects in list */
SLRI_ERR Misc_GenericGetNumberofNodes(Pointer list, Int4 PNTR num)
{
	SlriGenericLinkListPtr sgllp=NULL;
	Int4 count = 0;

	if(list == NULL) {
		ErrPostEx(SEV_ERROR, 1, 0, "Misc_GenericGetNumberofNodes: Invalid Pointer.");
		return SLRI_FAIL;
	}
	if(num == NULL) {
		ErrPostEx(SEV_ERROR, 1, 0, "Misc_GenericGetNumberofNodes: Passed Pointer is null.");
		return SLRI_FAIL;
	}

	sgllp=(SlriGenericLinkListPtr)(list);
	while(sgllp != NULL) {
		sgllp = sgllp->next;
		count++;
	}

	*num = count;

	return SLRI_SUCCESS;
}

/**********/
/* Memory */
/**********/

void Mem_Check(Pointer ptr)
{
	if(ptr==NULL) {
		ErrPostEx(SEV_FATAL, 4, 0, "Unable to allocate memory!\n");
		exit(1);
	}
}

/*************/
/* ByteStore */
/*************/

/* deletes "size" bytes from front of bsp, and resets position to front of bsp */
void BSRemoveLeadingChar(ByteStorePtr bsp, long size)
{
  if(bsp == NULL){
    ErrPostEx(SEV_ERROR, 0, 0, "BSRemoveLeadingChar: ByteStorePtr is NULL.");
    return;
  }
  if(size < 0){
    ErrPostEx(SEV_ERROR, 0, 0, "BSRemoveLeadingChar: Negative size not valid.");
    return;
  }
  BSSeek(bsp, 0L, SEEK_SET); 
  BSDelete(bsp, size);
  BSSeek(bsp, 0L, SEEK_SET);
}

/*printf to a Bytestore - return number of bytes printed*/
Int4 BSprintf(ByteStorePtr bsp, CharPtr format, ...)
{
	CharPtr buf=NULL;
	va_list args;
	Int4 len=0;

	if((bsp==NULL)||(format==NULL)) {
		return(len);
	}

	va_start(args, format);
	buf = (CharPtr) TSPrintfArgs(format, args);
	va_end(args);
	if(buf != NULL) {
		len=StringLen(buf);
		BSWrite(bsp, buf, StringLen(buf));
	}
	return(len);
}

/*saves a string stored in a ByteStore to a CharPtr*/
CharPtr BSStringSave(ByteStorePtr bsp)
{
	CharPtr string=NULL;
	Int4 len=0;

	if(bsp==NULL) {
		return(NULL);
	}

	len = BSLen(bsp);
	BSSeek(bsp, 0L, SEEK_SET);
	string = MemNew((size_t) (len+1)*sizeof(Char));
	Mem_Check(string);
	BSRead(bsp, string, len+1);
	string[len]='\0';	/*null terminate*/

	return(string);
}

/*Read a line (until a newline is seen) from a file
newline is not included in destination*/
Int2 BSGetLineFromFile(ByteStorePtr bspDest, FILE *fp)
{
	Int2 ch;

	if(fp==NULL) {
		ErrPostEx(SEV_ERROR, 0, 0, "BSGetLineFromFile: Passed file pointer is NULL.");
		return(1);
	}
	if(bspDest==NULL) {
		ErrPostEx(SEV_ERROR, 0, 0, "BSGetLineFromFile: Passed bspDest is NULL.");
		return(1);
	}

	if(feof(fp)) {
		return(EOF);
	}

	/*overwrite destination - make sure it is clear*/
	BSSeek(bspDest, 0L, SEEK_SET);
	BSDelete(bspDest, BSLen(bspDest));
	BSSeek(bspDest, 0L, SEEK_SET);

	/*transfer string*/
	while((!(feof(fp))&&(ch=(Int2)(fgetc(fp))) != EOF)) {
		if(ch=='\n') {
			break;	/*don't write newline to destination*/
		}
		BSPutByte(bspDest, ch);
	}

	BSSeek(bspDest, 0L, SEEK_SET);

	return(0);
}

/*print a bytestore to a stream*/
Boolean PrintBS(ByteStorePtr bsp, FILE *fp)
{
	CharPtr str=NULL;

	if(!bsp) {
		ErrPostEx(SEV_ERROR, 0, 0, "PrintBS: Passed ByteStorePtr is NULL.");
		return(1);
	}
	if(!fp) {
		ErrPostEx(SEV_ERROR, 0, 0, "PrintBS: Passed file pointer is NULL.");
		return(1);
	}

	str = BSStringSave(bsp);
	fprintf(fp, "%s", IsNullStr(str));
	str = MemFree(str);

	return(0);
}

/*Errlog bytestore - useful for CGIs that must write to a log file in a single
write call so that multiple concurrent CGIs don't write interleaved logs lines*/

ByteStorePtr g_bspErrorLog;

/*copied from ncbierr.c*/
static char * _szSevDef [SEV_MAX + 1] = {
  "", "NOTE:", "WARNING:", "ERROR:", "REJECT:", "FATAL ERROR:", "FATAL ERROR:" };

/*error handler function to write errors to a global variable ByteStore*/
/*set with ErrSetHandler((ErrHookProc) BSErrorHandler);*/
int LIBCALLBACK BSErrLogHandler PROTO((const ErrDescPtr err))
{
	if(!g_bspErrorLog) {
		g_bspErrorLog = BSNew(256);
		if(!g_bspErrorLog) {
			return(0);
		}
	}

	if(err->severity >= ErrGetLogLevel()) {
		BSprintf(g_bspErrorLog, "%s {%s, line %ld} %s\n", IsNullStr(_szSevDef[err->severity]), IsNullStr(err->srcfile), (long) err->srcline, IsNullStr(err->errtext));
	}

	return(ANS_OK);
}

/*like ErrSetLogfile, but set up errlog bytestore*/
/*returns TRUE if log file was successfully opened*/
Boolean BSErrLogOpen(const char *filename, unsigned long flags)
{
	if(!ErrSetLogfile (filename, flags)) {
		return(FALSE);
	}

	if(!g_bspErrorLog) {
		g_bspErrorLog = BSNew(256);
	}
	ErrSetHandler((ErrHookProc) BSErrLogHandler);
	ErrClearOptFlags(EO_LOGTO_USRFILE);	/*don't log to file until end*/
	return(TRUE);
}

/*print bytestore error log to a predefined NCBI corelib errorlog
NOTE: this should not have to be used, unless you want to bypass the functionality of BSError logging*/
Boolean BSErrLogFlush(void)
{
	CharPtr str=NULL;

	if(!g_bspErrorLog) {
		ErrPostEx(SEV_ERROR, 0, 0, "BSErrLogPrint: g_bspErrorLog is NULL.");
		return(FALSE);
	}

	ErrSetOptFlags(EO_LOGTO_USRFILE);	/*log to file*/
	str = BSStringSave(g_bspErrorLog);
	ErrLogPrintStr(str);
	str = MemFree(str);

	return(TRUE);
}

/*close the errlog bytestore - and write out to the log file*/
ByteStorePtr BSErrLogClose(void)
{
	if(!BSErrLogFlush()) {
		ErrSetOptFlags(EO_LOGTO_USRFILE);	/*log to file*/
		ErrPostEx(SEV_MAX, 0, 0, "BSErrLogClose: Unable to print to log. BSErrLogOpen() was probably not called.");
	}

	if(g_bspErrorLog) {
		g_bspErrorLog = BSFree(g_bspErrorLog);
	}

	return(NULL);
}

/*get a reference to the bytestore errlog for user*/
ByteStorePtr BSErrLogGet(void)
{
	return(g_bspErrorLog);
}

/*printf to the ByteStore log - return number of bytes printed*/
Int4 BSErrLogPrintf(const char *format, ...)
{
	va_list args;
	CharPtr buf=NULL;
	Int4 len=0;

	if(!g_bspErrorLog) {
		ErrPostEx(SEV_ERROR, 0, 0, "BSErrLogPrintf: g_bspErrorLog is NULL.");
		return(FALSE);
	}

	if(format==NULL) {
		ErrSetOptFlags(EO_LOGTO_USRFILE);	/*log to file*/
		ErrPostEx(SEV_MAX, 0, 0, "BSErrLogPrintf: format string was NULL.");
		return(len);
	}

	va_start(args, format);
	buf = (CharPtr) TSPrintfArgs(format, args);
	va_end(args);
	if(buf != NULL) {
		len=StringLen(buf);
		BSWrite(g_bspErrorLog, buf, len);
	}
	return(len);
}

/**************/
/*File Parsing*/
/**************/

/*This function is like fgets, but it dynamically allocates the correct string size for the line of the file*/
/*NOTE: allocates a new string*/
CharPtr Misc_GetLineFromFile(FILE *fp)
{
	ByteStorePtr bsp=NULL;
	CharPtr string=NULL;

	if(fp==NULL) {
		ErrPostEx(SEV_ERROR, 0, 0, "Misc_GetLineFromFile: Passed file pointer is NULL.");
		return(NULL);
	}

	bsp = BSNew(64);	/*64 initial size arbitrarily set*/
	if(BSGetLineFromFile(bsp, fp)==EOF) {
		bsp=BSFree(bsp);
		return(NULL);
	}
	string = BSStringSave(bsp);
	bsp=BSFree(bsp);

	return(string);
}

/*A version of strtok that's thread-safe.
pos is the starting position in the string and it gets updated by the function
Returns newly allocated memory.*/
CharPtr Misc_StrTok(CharPtr input, CharPtr delim, Uint4Ptr pos)
{
	CharPtr nextDelim=NULL, token=NULL;
	Char saveChar=0;
	Int4 posInc=0, numDelims=0;

	if(input==NULL) {
		ErrPostEx(SEV_ERROR, 0, 0, "Misc_StrTok: Passed input is NULL.");
		return(NULL);
	}
	if(delim==NULL) {
		ErrPostEx(SEV_ERROR, 0, 0, "Misc_StrTok: Passed delim is NULL.");
		return(NULL);
	}
	if(pos==NULL) {
		ErrPostEx(SEV_ERROR, 0, 0, "Misc_StrTok: Passed pos is NULL.");
		return(NULL);
	}

	if(((*pos)+1)>strlen(input)) {
		ErrPostEx(SEV_INFO, 0, 0, "Misc_StrTok: Passed pos is > length of line.");
		(*pos)=0;
		return(NULL);
	}

	/*remove consecutive delimiters at start of input*/
	if((*pos)==0) {
		*pos = strspn(input, delim);
	}

	input+=(*pos);
	nextDelim = strpbrk(input, delim);
	if(nextDelim!=NULL) {
		saveChar = nextDelim[0];
		nextDelim[0] = '\0';
		token=StringSave(input);
		nextDelim[0] = saveChar;
		/*consecutive deliminters*/
		numDelims=strspn(nextDelim, delim);
	}
	else {
		token=StringSave(input);
		numDelims=0;
	}
	/*update position*/
	posInc = strlen(token)+numDelims;
	(*pos)+=posInc;

	return(token);
}

/*Function to generically parse a delimited columnar file*/
/*Format defines data types of successive columns, support s=string,i=Int4,f=FloatHi
(e.g. "sif" means the file contains 3 columns, with the first column being strings, etc.
delim = the character delimiters between columns, behaves like strtok
Columns are returned in a linked list of linked lists (linked list of columns)*/
/*NOTE: This function can be called multiple times on the same file to parse different parts
of the file that have different columnar formats*/
/*Returns -1 for error and EOF, vnpColumns==NULL if not able to parse*/
Int2 Misc_GenericColumnParseEx(FILE *fp, CharPtr format, CharPtr delim, ValNodePtr *vnpColumns, Int4 NumLinesToParse)
{
	CharPtr line=NULL, token=NULL;
	ValNodePtr vnpNodeTmp=NULL;
	Int4 numLineTokens=0, lenFormat=0, lineNum=0, i=0, filePos=0;
	Uint4 pos=0;
	ValNodePtr *vnppHeadColsArray=NULL, *vnppCurrColsArray=NULL, *vnppPrevColsArray=NULL;
	Boolean error=FALSE;

	if(fp==NULL) {
		ErrPostEx(SEV_ERROR, 0, 0, "Misc_GenericColumnParse: Passed file pointer is NULL.");
		return(-1);
	}
	if(format==NULL) {
		ErrPostEx(SEV_ERROR, 0, 0, "Misc_GenericColumnParse: Passed format is NULL.");
		return(-1);
	}
	if(delim==NULL) {
		ErrPostEx(SEV_ERROR, 0, 0, "Misc_GenericColumnParse: Passed delim is NULL.");
		return(-1);
	}
	if(vnpColumns==NULL) {
		ErrPostEx(SEV_ERROR, 0, 0, "Misc_GenericColumnParse: Passed *vnpColumns is NULL.");
		return(-1);
	}
	if(feof(fp)) {
		return(-1);
	}

	lenFormat = strlen(format);
	vnppHeadColsArray = MemNew((size_t)lenFormat*sizeof(ValNodePtr));
	vnppCurrColsArray = MemNew((size_t)lenFormat*sizeof(ValNodePtr));
	vnppPrevColsArray = MemNew((size_t)lenFormat*sizeof(ValNodePtr));

	filePos=ftell(fp);
	while((line=Misc_GetLineFromFile(fp))!=NULL) {
		lineNum++;
		/*parse line*/
		numLineTokens=0;
		while((token = Misc_StrTok(line, delim, &pos))!=NULL) {
			/*check if line has too many columns*/
			if(numLineTokens>=lenFormat) {
				ErrPostEx(SEV_WARNING, 0, 0, "Misc_GenericColumnParse: Input file line %ld has too many columns.",
					(long) lineNum);
				token=MemFree(token);
				error=TRUE;
				goto Misc_GenericColumnParse_loopBreak;
			}
			switch(format[numLineTokens]) {
			case 's':
				ValNodeAddPointer(&vnpNodeTmp, 's', token);
				break;
			case 'i':
				if(Misc_IsNumeric(token)) {
					ValNodeAddInt(&vnpNodeTmp, 'i', atol(token));
					token=MemFree(token);
				}
				else {
					ErrPostEx(SEV_WARNING, 0, 0, "Misc_GenericColumnParse: Input file line %ld, "
						"column %ld is expected to be an integer, but is instead %s.",
						(long) lineNum, (long) numLineTokens+1, IsNullStr(token));
					token=MemFree(token);
					error=TRUE;
					goto Misc_GenericColumnParse_loopBreak;
				}
				break;
			case 'f':
				if(Misc_IsFloatNumber(token)) {
					ValNodeAddFloat(&vnpNodeTmp, 'f', atof(token));
					token=MemFree(token);
				}
				else {
					ErrPostEx(SEV_WARNING, 0, 0, "Misc_GenericColumnParse: Input file line %ld, "
						"column %ld is expected to be an integer, but is instead %s.",
						(long) lineNum, (long) numLineTokens+1, IsNullStr(token));
					token=MemFree(token);
					error=TRUE;
					goto Misc_GenericColumnParse_loopBreak;
				}
				break;
			default:
				ErrPostEx(SEV_ERROR, 0, 0, "Misc_GenericColumnParse: Format string contains invalid character %c.",
					format[numLineTokens]);
				token=MemFree(token);
				error=TRUE;
				goto Misc_GenericColumnParse_loopBreak;
			}
			if(vnppHeadColsArray[numLineTokens]==NULL) {
				/*first outer while loop*/
				vnppHeadColsArray[numLineTokens] = vnpNodeTmp;
				vnppCurrColsArray[numLineTokens] = vnpNodeTmp;
				vnppPrevColsArray[numLineTokens] = NULL;
			}
			else {
				/*subsequent outer while loops*/
				vnppPrevColsArray[numLineTokens] = vnppCurrColsArray[numLineTokens];
				(vnppCurrColsArray[numLineTokens])->next = vnpNodeTmp;
				vnppCurrColsArray[numLineTokens] = vnpNodeTmp;
			}
			vnpNodeTmp=NULL;
			numLineTokens++;
		}
		/*check if line has not enough columns*/
		if(numLineTokens!=lenFormat) {
			ErrPostEx(SEV_INFO, 0, 0, "Misc_GenericColumnParse: Number of columns don't match expected format.");
			error=TRUE;
			goto Misc_GenericColumnParse_loopBreak;
		}
		line=MemFree(line);
		filePos=ftell(fp);
		if(lineNum == NumLinesToParse) break;
	}

Misc_GenericColumnParse_loopBreak:	/*nested loop break continuation point*/

	if(error) {
		/*remove parsing results of the line that contained the error*/
		for(i=0;i<numLineTokens;i++) {
			if(vnppPrevColsArray[i]!=NULL) {
				/*take care of proper end of vnps if it exists*/
				(vnppPrevColsArray[i])->next=NULL;
			}
			switch(format[i]) {
			case 's':
				vnppCurrColsArray[i] = ValNodeFreeData(vnppCurrColsArray[i]);
				break;
			case 'i':
			case 'f':
				vnppCurrColsArray[i] = ValNodeFree(vnppCurrColsArray[i]);
				break;
			}
			if(lineNum==1) {
				/*the only line parsed contains a format error, don't return it*/
				vnppHeadColsArray[i] = NULL;
			}
		}
		/*clean up memory*/
		line=MemFree(line);
		fseek(fp, filePos, 0);
	}

	for(i=0;i<lenFormat;i++) {
		ValNodeAddPointer(vnpColumns, (Int2) i, vnppHeadColsArray[i]);
	}
	/*add a copy of the heads of each column list to the end of the column list - used for keeping track of the rows*/
	for(i=0;i<lenFormat;i++) {
		ValNodeAddPointer(vnpColumns, (Int2) i, vnppHeadColsArray[i]);
	}

	vnppHeadColsArray = MemFree(vnppHeadColsArray);
	vnppCurrColsArray = MemFree(vnppCurrColsArray);
	vnppPrevColsArray = MemFree(vnppPrevColsArray);

	return(0);
}


Int2 Misc_GenericColumnParse(FILE *fp, CharPtr format, CharPtr delim, ValNodePtr *vnpColumns)
{
	return Misc_GenericColumnParseEx(fp, format, delim, &(*vnpColumns), 0);
}

Int2 Misc_GenericColumnParseWithNumLines(FILE *fp, CharPtr format, CharPtr delim, ValNodePtr *vnpColumns, Int4 NumLinesToParse)
{
	return Misc_GenericColumnParseEx(fp, format, delim, &(*vnpColumns), NumLinesToParse);
}


/*Like Misc_GenericColumnParse, but takes a filename for convenience.
Don't use this function if you want to parse parts of the same file that have different formats*/
Int2 Misc_GenericColumnParseFileName(CharPtr fileName, CharPtr format, CharPtr delim, ValNodePtr *vnpColumns)
{
	FILE *fp=NULL;

	if(fileName==NULL) {
		ErrPostEx(SEV_ERROR, 0, 0, "Misc_GenericColumnParseFileName: Passed filename is NULL.");
		return(-1);
	}

	if((fp=FileOpen(fileName, "r"))==NULL) {
		ErrPostEx(SEV_ERROR, 0, 0, "Misc_GenericColumnParseFileName: Can't open file %s.", IsNullStr(fileName));
		return(-1);
	}

	if(Misc_GenericColumnParse(fp, format, delim, vnpColumns)<0) {
		return(-1);
	}

	FileClose(fp);

	return(0);
}

/*write out columns parsed in by Misc_GenericColumnParse.  Each column will be separated by the delim string*/
Int2 Misc_GenericColumnWrite(FILE *fp, ValNodePtr vnpColumns, CharPtr delim)
{
	ValNodePtr *vnppCurrColsArray=NULL;
	Int4 numCols=0, i=0;

	if(fp==NULL) {
		ErrPostEx(SEV_ERROR, 0, 0, "Misc_GenericColumnWrite: Passed file pointer is NULL.");
		return(-1);
	}
	if(vnpColumns==NULL) {
		ErrPostEx(SEV_ERROR, 0, 0, "Misc_GenericColumnWrite: Passed vnpColumns is NULL.");
		return(-1);
	}
	if(delim==NULL) {
		ErrPostEx(SEV_ERROR, 0, 0, "Misc_GenericColumnWrite: Passed delim is NULL.");
		return(-1);
	}

	/*save array of columns*/
	numCols = ValNodeLen(vnpColumns);
	numCols = numCols/2;
	vnppCurrColsArray = MemNew((size_t)numCols*sizeof(ValNodePtr));
	for(i=0;i<numCols;i++) {
		vnppCurrColsArray[i]=vnpColumns->data.ptrvalue;
		vnpColumns=vnpColumns->next;
	}

	/*print out columns*/
	while(vnppCurrColsArray[0]!=NULL) {
		/*line*/
		for(i=0;i<numCols;i++) {
			if(vnppCurrColsArray[i]==NULL) {
				ErrPostEx(SEV_ERROR, 0, 0, "Misc_GenericColumnWrite: Column %ld is not the same length as the first column.", (long) i);
				goto Misc_GenericColumnWrite_loopBreak;
			}
			/*column*/
			switch(vnppCurrColsArray[i]->choice) {
			case 's':
				fprintf(fp, "%s", IsNullStr((CharPtr) (vnppCurrColsArray[i])->data.ptrvalue));
				break;
			case 'i':
				fprintf(fp, "%ld", (long) (vnppCurrColsArray[i])->data.intvalue);
				break;
			case 'f':
				fprintf(fp, "%f", (double) (vnppCurrColsArray[i])->data.realvalue);
				break;
			}
			if((i+1)<numCols) {
				fprintf(fp, "%s", IsNullStr(delim));
			}
			vnppCurrColsArray[i]=vnppCurrColsArray[i]->next;
		}
		fprintf(fp, "\n");
	}

Misc_GenericColumnWrite_loopBreak:	/*nested loop break continuation point*/

	vnppCurrColsArray = MemFree(vnppCurrColsArray);

	return(0);
}

/*write out columns parsed in by Misc_GenericColumnParse, given a filename.
Each column will be separated by the delim string*/
Int2 Misc_GenericColumnWriteFileName(CharPtr fileName, ValNodePtr vnpColumns, CharPtr delim)
{
	FILE *fp=NULL;

	if(fileName==NULL) {
		ErrPostEx(SEV_ERROR, 0, 0, "Misc_GenericColumnWriteFileName: Passed filename is NULL.");
		return(-1);
	}

	if((fp=FileOpen(fileName, "w"))==NULL) {
		ErrPostEx(SEV_ERROR, 0, 0, "Misc_GenericColumnWriteFileName: Can't open file %s.", IsNullStr(fileName));
		return(-1);
	}

	if(Misc_GenericColumnWrite(fp, vnpColumns, delim)<0) {
		return(-1);
	}

	FileClose(fp);

	return(0);
}

static ValNodePtr Misc_GenericMoveToRefListStart(ValNodePtr vnpColumns)
{
	Int4 i=0;

	/*move to start of reference pointer list*/
	while((vnpColumns->next!=NULL)&&(vnpColumns->choice <= vnpColumns->next->choice)) {
		vnpColumns = vnpColumns->next;
		i++;
	}
	if(vnpColumns->next!=NULL) {
		vnpColumns = vnpColumns->next;
	}

	return(vnpColumns);
}

/*Advance a vnpColumns data structure by given number of rows*/
/*returns 1 for error, -1 for EOF*/
Int4 Misc_GenericColumnAdvanceRow(ValNodePtr vnpColumns, Int4 numRows2Advance)
{
	ValNodePtr vnpTmp=NULL;
	Int4 i=0;

	if(vnpColumns==NULL) {
		ErrPostEx(SEV_ERROR, 0, 0, "Misc_GenericColumnAdvanceRow: Passed vnpColumns is NULL.");
		return(1);
	}

	/*move to start of reference pointer list*/
	vnpColumns = Misc_GenericMoveToRefListStart(vnpColumns);

	/*advance reference pointers by number of rows given*/
	while(vnpColumns!=NULL) {
		vnpTmp = vnpColumns->data.ptrvalue;
		for(i=0;i<numRows2Advance;i++) {
			if(vnpTmp!=NULL) {
				vnpTmp=vnpTmp->next;
			}
			else {
				return(1);
			}
		}
		vnpColumns->data.ptrvalue = vnpTmp;
		vnpColumns=vnpColumns->next;
	}

	/*signal if there are no more rows*/
	if(vnpTmp==NULL) {
		/*assume input is valid (should be true if always using genericColumnParse functions)*/
		return(-1);
	}

	return(0);
}

/*Set the vnpColumns data structure back to a specific row number*/
/*NOTE: goes through entire list starting from the head - can be slow for large lists*/
/*returns -1 for error*/
Int4 Misc_GenericColumnReset2Row(ValNodePtr vnpColumns, Int4 rowNum)
{
	ValNodePtr vnpTmp=NULL, vnpColumnsHead=NULL, vnpColumnsRefHead=NULL;
	Int4 i=0;

	if(vnpColumns==NULL) {
		ErrPostEx(SEV_ERROR, 0, 0, "Misc_GenericColumnAdvanceRow: Passed vnpColumns is NULL.");
		return(-1);
	}

	vnpColumnsHead = vnpColumns;
	/*move to start of reference pointer list*/
	vnpColumns = Misc_GenericMoveToRefListStart(vnpColumns);
	vnpColumnsRefHead = vnpColumns;

	/*reset reference pointer list*/
	while(vnpColumns!=NULL) {
		vnpColumns->data.ptrvalue = vnpColumnsHead->data.ptrvalue;
		vnpColumns=vnpColumns->next;
		vnpColumnsHead=vnpColumnsHead->next;
	}

	vnpColumns = vnpColumnsRefHead;
	/*advance reference pointers by number of rows given*/
	while(vnpColumns!=NULL) {
		vnpTmp = vnpColumns->data.ptrvalue;
		for(i=0;i<rowNum;i++) {
			if(vnpTmp!=NULL) {
				vnpTmp=vnpTmp->next;
			}
			else {
				return(-1);
			}
		}
		vnpColumns->data.ptrvalue = vnpTmp;
		vnpColumns=vnpColumns->next;
	}

	return(0);
}

/*Get the data value from a vnpColumns data structure by column number of the current row*/
/*NOTE: column numbers start at 0*/
/*returns NULL for error*/
ValNodePtr Misc_GenericColumnGetColumn(ValNodePtr vnpColumns, Int4 colNum)
{
	Int4 i=0;

	if(vnpColumns==NULL) {
		ErrPostEx(SEV_ERROR, 0, 0, "Misc_GenericColumnGetColumn: Passed vnpColumns is NULL.");
		return(NULL);
	}

	/*move to start of reference pointer list*/
	vnpColumns = Misc_GenericMoveToRefListStart(vnpColumns);

	/*move to given column*/
	for(i=0;i<colNum;i++) {
		if(vnpColumns!=NULL) {
			vnpColumns=vnpColumns->next;
		}
		else {
			return(NULL);
		}
	}

	if(vnpColumns==NULL) {
		ErrPostEx(SEV_ERROR, 0, 0, "Misc_GenericColumnGetColumn: Passed colNum is invalid.");
		return(NULL);
	}

	return(vnpColumns->data.ptrvalue);
}

/*Get a pointer to the head of a column vnpColumns data structure by column number*/
/*NOTE: column numbers start at 0*/
/*returns NULL for error*/
ValNodePtr Misc_GenericColumnGetColumnHead(ValNodePtr vnpColumns, Int4 colNum)
{
	Int4 i=0;

	if(vnpColumns==NULL) {
		ErrPostEx(SEV_ERROR, 0, 0, "Misc_GenericColumnGetColumn: Passed vnpColumns is NULL.");
		return(NULL);
	}

	/*move to given column*/
	for(i=0;i<colNum;i++) {
		if(vnpColumns!=NULL) {
			vnpColumns=vnpColumns->next;
		}
		else {
			return(NULL);
		}
	}

	if(vnpColumns==NULL) {
		ErrPostEx(SEV_ERROR, 0, 0, "Misc_GenericColumnGetColumn: Passed colNum is invalid.");
		return(NULL);
	}

	return(vnpColumns->data.ptrvalue);
}

/*gets the number of rows (lines) read in from vnpColumns data structure*/
Int4 Misc_GenericColumnGetNumberRows(ValNodePtr vnpColumns)
{
	ValNodePtr vnpCol=NULL;

	if(vnpColumns==NULL) {
		ErrPostEx(SEV_ERROR, 0, 0, "Misc_GenericColumnGetColumn: Passed vnpColumns is NULL.");
		return(-1);
	}

	/*assume properly constructed vnpColumns data structure - just get length of first column*/
	vnpCol = vnpColumns->data.ptrvalue;

	return(ValNodeLen(vnpCol));
}

/*Frees a vnpColumns data structure*/
ValNodePtr Misc_VnpColumnsFree(ValNodePtr vnpColumns)
{
	ValNodePtr vnpCol=NULL, vnpTmp=NULL, vnpColumnsHead=NULL;

	if(vnpColumns==NULL) {
		return(NULL);
	}

	vnpColumnsHead = vnpColumns;
	while(vnpColumns!=NULL) {
		vnpCol = vnpColumns->data.ptrvalue;
		while(vnpCol!=NULL) {
			vnpTmp = vnpCol;
			vnpCol=vnpCol->next;
			vnpTmp->next=NULL;
			switch(vnpTmp->choice) {
			case 's':
				vnpTmp = ValNodeFreeData(vnpTmp);
				break;
			case 'i':
			case 'f':
				vnpTmp = ValNodeFree(vnpTmp);
				break;
			}
		}
		if((vnpColumns->next!=NULL)&&(vnpColumns->choice >= vnpColumns->next->choice)) {
			/*marks end of real columns list - subsequent nodes are just reference pointers to the main list*/
			break;
		}
		vnpColumns=vnpColumns->next;
	}
	
	return(ValNodeFree(vnpColumnsHead));
}

/*****************************************************************************
*
*   Slri_ValNodeAdd* (head, choice, value)
*      adds like NCBI's ValNodeAdd* except in the fromt of the list
*      sets newnode->choice = choice (if choice does not matter, use 0)
*      sets newnode->data.*value = value
*		adds the new node to the head of the list
*		returns a pointer to the head of the list (new node).
*****************************************************************************/
ValNodePtr Slri_ValNodeAddInt (ValNodePtr PNTR head, Int2 choice, Int4 value)
{
	ValNodePtr newnode;

	newnode = (ValNodePtr) MemNew(sizeof(ValNode));
	Mem_Check(newnode);
	newnode->choice = (Uint1)choice;
	newnode->data.intvalue = value;

	/*place the new node at the front of the list.*/
	newnode->next = *head;
	*head = newnode;

	return newnode;
}

/*This function does not save a copy of str*/
ValNodePtr Slri_ValNodeAddStr (ValNodePtr PNTR head, Int2 choice, CharPtr str)
{
	ValNodePtr newnode;
	
	if (str == NULL) {
		return NULL;
	}

	newnode = Slri_ValNodeAddPointer(head, choice, (Pointer) str);

	return newnode;
}

ValNodePtr Slri_ValNodeAddPointer (ValNodePtr PNTR head, Int2 choice, Pointer value)
{
	ValNodePtr newnode;

	if (value == NULL) return NULL;
	
	newnode = (ValNodePtr) MemNew(sizeof(ValNode));
	Mem_Check(newnode);
	newnode->choice = (Uint1)choice;
	newnode->data.ptrvalue = value;

	/*place the new node at the front of the list.*/
	newnode->next = *head;
	*head = newnode;

	return newnode;
}



ValNodePtr Slri_ValNodeCopyStr (ValNodePtr PNTR head, Int2 choice, CharPtr str)
{
	ValNodePtr newnode;
	
	if(str == NULL) {
		return NULL;
	}
	
	newnode = (ValNodePtr) MemNew(sizeof(ValNode));
	Mem_Check(newnode);
	newnode->choice = (Uint1)choice;
	newnode->data.ptrvalue = StringSave(str);

	/*place the new node at the front of the list.*/
	newnode->next = *head;
	*head = newnode;

	return newnode;
}

/*randomize a valnode list*/
Boolean RandomizeValNodeList(ValNodePtr PNTR head, Boolean *start)
{
	Int4 listLen=0, rand=0, i=0;
	ValNodePtr vnpTmp=NULL, vnpPrev=NULL, vnpNewHead=NULL, vnpOldHead=NULL;

	if((head == NULL)||(*head==NULL)) {
		return(FALSE);
	}
	if(start == NULL) {
		return(FALSE);
	}

	if(start) {
		RandomSeed(clock()+GetAppProcessID());
		start=FALSE;
	}

	vnpOldHead = vnpTmp = *head;
	listLen = ValNodeLen(vnpTmp);
	while(vnpTmp!=NULL) {
		vnpPrev=NULL;
		rand=RandomNum();
		rand=rand%listLen;
		/*randomly move to a position in the list*/
		for(i=0;i<rand;i++) {
			vnpPrev=vnpTmp;
			vnpTmp=vnpTmp->next;
		}
		/*remove chosen node and put in new list*/
		if(vnpPrev!=NULL) {
			vnpPrev->next=vnpTmp->next;
			/*add to start of list*/
			vnpTmp->next = vnpNewHead;
			vnpNewHead=vnpTmp;
		}
		else {
			/*node is start of list - just add to start of list*/
			vnpOldHead = vnpTmp->next;
			vnpTmp->next = vnpNewHead;
			vnpNewHead=vnpTmp;
		}
		vnpTmp=vnpOldHead;
		listLen--;
	}

	*head = vnpNewHead;

	return(TRUE);
}


/*RY: helper function to implement keyed list API functions.
 *    two major differences from ValNodeAddPointer:
 *    - value can be NULL
 *    - value add to the end of the list, and return the head of the list
 */
ValNodePtr Slri_ValNodeAddPointerAtEnd (ValNodePtr PNTR head, Int2 choice, Pointer value)
{
	ValNodePtr newnode=NULL, v= NULL;
	
	newnode = (ValNodePtr) MemNew(sizeof(ValNode));
	Mem_Check(newnode);
	newnode->choice = (Uint1)choice;
	newnode->data.ptrvalue = value;
	
	/*place the new node at the end of the list.*/
	/*travers the list*/
	v= *head;
	while (v!=NULL && v->next!=NULL)
	{
	    v = v->next;    /*v points to the end of the list*/
	}
        
	if (v == NULL) *head = newnode;
	else v->next = newnode;

	return *head;
}




/*RY: helper function to print out the query-return pair for a int query with a
 *    int list return value. The query-return pair is stored in a valnodelist
 *    with the choice field indicates. 
 */
void Slri_PrintKeyedList(ValNodePtr retlist)
{
    ValNodePtr v, v1;
    
    for (v=retlist; v!=NULL; v=v->next)
    { 
	if (v->choice==0)
	{
	    printf ("query =  %ld\n", v->data.intvalue);
        }
	if (v->choice==1)
	{
	    if (v->data.ptrvalue==NULL)
	    {
		 printf ("No pointer value\n");
	    }
	    else
	    {
		  printf ("return int list:\n");
		  for (v1=v->data.ptrvalue; v1!=NULL;v1=v1->next)
        	  { 
           	      printf ("%ld,", v1->data.intvalue);
		  }
		   printf ("\n");
	     }
	  }
      }
}


/*free a keyed list where its consecutive valnode store query (the valnose choice == 0, stored in data.intvalue) and
  return (the valnode choice ==1, store a valnodelist in data.ptrvalue*/
ValNodePtr Slri_FreeKeyedList (ValNodePtr retList)
{
   ValNodePtr tmp = NULL;
   if (retList == NULL) return NULL;

   for (tmp = retList; tmp != NULL; tmp= tmp->next)
   {
       if (tmp->choice == 1)
            ValNodeFree(tmp->data.ptrvalue);
   }
   return ValNodeFree(retList);
}


/*RY: helper function to print out the query-return pair for a int query with a
 *    SLRIFasta return value. The query-return pair is stored in a valnodelist
 *    with the choice field indicates. 
 */
void Slri_PrintFastaKeyedList(ValNodePtr retlist)
{
    ValNodePtr v=NULL, v1=NULL;
    for (v= retlist; v!=NULL; v=v->next)
    {
         if (v->choice == 0)
	 {
	     printf ("query = %ld\n", v->data.intvalue);
	 }
	 if (v->choice == 1)
	 {
	     if (v->data.ptrvalue != NULL)
	     {
	          printf ("%s\n", ((SLRIFastaPtr)v->data.ptrvalue)->defline);
		  printf ("%s\n", ((SLRIFastaPtr)v->data.ptrvalue)->sequence);
	     }
	     else
	     {
	          printf ("No return value.\n");
	     }
	 }
    }
}


/* Free a fasta keyedlist where its consecutive valnode stored query (the valnode choice == 0, stored in data.intvalue )  
   and return (the valnode choice == 1, stored as fasta structure in data.ptrvalue) */
ValNodePtr Slri_FreeFastaKeyedList(ValNodePtr retList)
{
   ValNodePtr tmp = NULL;
   if (retList== NULL) return NULL;

   for (tmp = retList; tmp!=NULL; tmp=tmp->next)
   {
       if (tmp->choice ==1)
		   SLRIFastaFree(tmp->data.ptrvalue);
   }
   return ValNodeFree (retList);
}

/*process a string str into an array of string poiters delimited by sep
  arrayLen is the length of the array, count is number of the not NULL
  entries of array. */
void Misc_Str2StrArray(CharPtr str, CharPtr *array, Int4 arrayLen, Int4 *count, CharPtr sep)
{

    int i=0,j=0;
	CharPtr a;
	
	if (NULL==str) return;
	while(i<arrayLen){
		array[i++]=NULL;
	}
	i=0;
	while(strlen(str)>0){
		array[i]=str;
		a=strstr(str, sep);
		if(a != NULL){
			str=a+strlen(sep);
			j=0;
			while(j<strlen(sep)){
					*(a+j)='\0';
					j++;
			}
			++(*count);
			i++;
		}
	}
}


ValNodePtr Misc_StrArray2ValNodeList(CharPtr *array, Int4 *count)
{
   int i, q, size, pos, len;
   CharPtr query=NULL, value=NULL, a =NULL;
   ValNodePtr retlist = NULL, vlist= NULL;
   
   for (i=0; i< *count;i++)
   {  
      if (array[i] != NULL)
      {
	 size=strlen(array[i]);
	 a = strstr(array[i], Q_R);
	 pos=strlen(a);
	 len = size-pos;
	 query= (char*)malloc(100);
	 strncpy(query, array[i], len);
	 value=StrSave(a+strlen(Q_R));
	 q=atoi(query);	 
         if (query) MemFree(query);
	 if (value !=NULL && (StrCmp (value, "NULL") !=0) )
	 {
	    vlist = Misc_CommaIntString2ValNodeList(value, ",");
	 }
	 else 
	 {
	    vlist = NULL;   
	 }
	
	 ValNodeAddInt(&retlist, 0, q);  
	 retlist = Slri_ValNodeAddPointerAtEnd(&retlist, 1, vlist);
	 
      }   
   }
   return retlist; 
}

/*******************************************/
/* FASTA FILE PARSING                      */
/*******************************************/


/* 
	Misc_GetFastaFromFile
	
	Reads a single FASTA sequence (with defline and sequence) from file and puts it in an
	SLRIFasta data structure.  Provide valid file pointer and SLRIFastaPtr reference pointer
	caller must free memory!
*/
#define BUF_MAX 1000
SLRI_ERR Misc_GetFastaFromFile(FILE *fp, SLRIFastaPtr PNTR sf)
{
	ByteStorePtr bsp = NULL;
	Char buf[BUF_MAX];
	Char ch;
	CharPtr pc = NULL;
	Int4 i = 0;

	if(fp == NULL || sf == NULL) {
		ErrPostEx(SEV_ERROR,0,0,"Invalid parameters");
		return SLRI_FAIL;
	}

	/* find the first seq */
	do {
        ch = (Char) getc(fp);
        if (ch == '!' || ch == '#') { /* comment symbol - ignore rest of line */
            do {
				ch = (Char) getc(fp);
            } while (ch != '\n' && ch != '\r' && ch != '\0' && ch != EOF);
        }
		if (ch == EOF) {
			return SLRI_SUCCESS;
		}
    } while (IS_WHITESP(ch));


	/* get the defline */
	if(ch == '>') {
        buf[0] = (Char) ch;
		fgets(buf+1, 1000-1, fp);
		if((*sf = SLRIFastaNew()) == NULL) return SLRI_FAIL;
		pc = (CharPtr) MemNew((size_t)sizeof(Char)*StringLen(buf));
		StringNCpy(pc,buf,StringLen(buf)-1);
		(*sf)->defline = pc;
	} else {
		ErrPostEx(SEV_ERROR,0,0,"FASTA FILE does not contain defline starting with '>'");
		return SLRI_FAIL;
	}
	
	bsp = BSNew(128);
	BSSeek(bsp, 0L, SEEK_SET);

	ch = (Char) getc(fp);
	while(ch != '>') {
		i = 0;
		buf[0] = (Char) ch;
		fgets(buf+1, BUF_MAX, fp);
		while (i < BUF_MAX-1) {
			if(buf[i] == '\n' || buf[i] == '\r' || buf[i] == NULLB) {
				buf[i] = NULLB;
				break;
            }
            i++;
        }
		BSWrite(bsp, buf, i);
		ch = (Char) getc(fp);
		if(ch == EOF) break;
	}
	ungetc(ch,fp);

	(*sf)->sequence = BSStringSave(bsp);
	bsp = BSFree(bsp);

	return SLRI_SUCCESS;
}

/* 
	Misc_FastaToSLRIFasta
	
	Reads all FASTA sequences (with defline and sequence) from file and puts them in an
	valnode list of SLRIFastaPtrs. Provide valid file pointer and ValNodePtr reference pointer
	caller must free memory!  Do NOT use ValNodeFreeData!!!
*/
SLRI_ERR Misc_FastaToSLRIFasta(FILE *fp, ValNodePtr PNTR vnppSLRIFasta)
{
	SLRIFastaPtr sf = NULL;

	do {
		sf = NULL;
		if((Misc_GetFastaFromFile(fp, &sf)) == SLRI_FAIL) return SLRI_FAIL;
		if(sf) ValNodeAddPointer(&(*vnppSLRIFasta),0,(Pointer) sf);
	} while (sf != NULL);

	return SLRI_SUCCESS;
}

/*
	Misc_ValNodeFastaListFree

	Frees a valnode list of SLRIFastaPtrs
	Reference Pointer is set to NULL
*/
void Misc_ValNodeFastaListFree(ValNodePtr PNTR vnppSLRIFasta)
{
	ValNodePtr head = *vnppSLRIFasta, vnp = NULL;
	vnp = head;
	while(vnp) {
		vnp->data.ptrvalue = SLRIFastaFree(vnp->data.ptrvalue);
		vnp = vnp->next;
	}
	head = ValNodeFree(head);
	*vnppSLRIFasta = head;
}





/*******************************************/
/* NCBI ASN.1 object constructor functions */
/*******************************************/

/*RULE: if a string is passed to a constructor function, a copy of it is integrated into the object*/

/*construct a new ObjectIdPtr with a string choice value*/
ObjectIdPtr Misc_C_ObjectIdStringNew(CharPtr string)
{
	ObjectIdPtr oip=NULL;

	oip = ObjectIdNew();
	if(oip == NULL) {
		return(NULL);
	}

	oip->str = StringSave(string);

	return(oip);
}

/*construct a new ObjectIdPtr with an integer choice value*/
ObjectIdPtr Misc_C_ObjectIdIntNew(Int4 integer)
{
	ObjectIdPtr oip=NULL;

	oip = ObjectIdNew();
	if(oip == NULL) {
		return(NULL);
	}

	oip->id = integer;

	return(oip);
}

/*construct a new DbtagPtr with a string database key*/
DbtagPtr Misc_C_DbtagStringNew(CharPtr db, CharPtr string)
{
	DbtagPtr dtp=NULL;

	dtp = DbtagNew();
	if(dtp == NULL) {
		return(NULL);
	}

	dtp->db=StringSave(db);
	dtp->tag=Misc_C_ObjectIdStringNew(string);

	return(dtp);
}

/*construct a new DbtagPtr with an integer database key*/
DbtagPtr Misc_C_DbtagIntNew(CharPtr db, Int4 integer)
{
	DbtagPtr dtp=NULL;

	dtp = DbtagNew();
	if(dtp == NULL) {
		return(NULL);
	}

	dtp->db=StringSave(db);
	dtp->tag=Misc_C_ObjectIdIntNew(integer);

	return(dtp);
}

/* note these flip functions may not be portable as they assume Int4 and
   FloatLo are 4 bytes each -- but its the best guess we can make at any
   rate */
Uint4 FlipUI4(Uint4 *me)
{
  Uint4 res;

  ((CharPtr)(&res))[0]=((CharPtr)me)[3];
  ((CharPtr)(&res))[1]=((CharPtr)me)[2];
  ((CharPtr)(&res))[2]=((CharPtr)me)[1];
  ((CharPtr)(&res))[3]=((CharPtr)me)[0];
  return res;
}

Int4 FlipI4(Int4 *me)
{
  Int4 res;

  ((CharPtr)(&res))[0]=((CharPtr)me)[3];
  ((CharPtr)(&res))[1]=((CharPtr)me)[2];
  ((CharPtr)(&res))[2]=((CharPtr)me)[1];
  ((CharPtr)(&res))[3]=((CharPtr)me)[0];
  return res;
}

FloatLo FlipF(FloatLo *me)
{
  FloatLo res;

  ((CharPtr)(&res))[0]=((CharPtr)me)[3];
  ((CharPtr)(&res))[1]=((CharPtr)me)[2];
  ((CharPtr)(&res))[2]=((CharPtr)me)[1];
  ((CharPtr)(&res))[3]=((CharPtr)me)[0];
  return res;
}

/* use these to add checksums to text files and read them back to
verify authenticity.  Should work for most binaries too. */
void BSComputeFileChecksum(ByteStorePtr bsp)
{
	Uint4 numpiece,cnt;
	Char rdbuf[4];
	Byte cksum[4]={0,0,0,0};
	Uint4 endian;
	Char checksum[28];
	
	if (bsp==NULL)
		return;
	BSSeek(bsp,0L,SEEK_SET);
	numpiece=BSLen(bsp)/4;
	for (cnt=0;cnt<numpiece;cnt++) {
		BSRead(bsp,rdbuf,4);
		/* add up the file in blocks of 4 */
		cksum[0]+=(Byte)rdbuf[0];
		cksum[1]+=(Byte)rdbuf[1];
		cksum[2]+=(Byte)rdbuf[2];
		cksum[3]+=(Byte)rdbuf[3];
	}
	endian=0x41424344;
	BSSeek(bsp,0L,SEEK_END);
	/* make middle/end 8 bits random to mess people up */
	cksum[2]+=(GetSecs()%256);
	/* now write checksum: CZ cksum endian */
	sprintf(checksum,"QZ%8lX%2X%2X%2X%2X%8lX",(long)numpiece,cksum[0],cksum[1],cksum[2],cksum[3],(long)endian);
	BSWrite(bsp,checksum,CHECKSUM_LENGTH);
}

Boolean BSStripFileChecksum(ByteStorePtr bsp)
{
	Int4 bl;

	if (bsp==NULL)
		return FALSE;
	bl=BSLen(bsp);
	if (bl<CHECKSUM_LENGTH)
		return FALSE;
	BSSeek(bsp,-CHECKSUM_LENGTH,SEEK_END);
	bl=BSDelete(bsp,CHECKSUM_LENGTH);
	return TRUE;
}

Boolean BSCheckFileChecksum(ByteStorePtr bsp)
{
	Uint4 numpiece,cnt;
	Char rdbuf[4];
	Char dbrdbuf[9];
	Char bytebuf[3];
	Byte cksum[4]={0,0,0,0};
	Int4 ckTest0=0,ckTest1=0,ckTest3=0,slack;
	long lendian,lpieces;
	Boolean magic=FALSE;

	if (bsp==NULL)
		return FALSE;
	BSSeek(bsp,0L,SEEK_SET);
	
	/* Make sure there is a checksum of sufficient length to verify */
	if (BSLen(bsp) < CHECKSUM_LENGTH){
	    return FALSE;
	}
	
	/* checksum is 26 characters */
	numpiece=(BSLen(bsp)-CHECKSUM_LENGTH)/4;
	slack=(BSLen(bsp)-CHECKSUM_LENGTH)%4;
	for (cnt=0;cnt<numpiece;cnt++) {
		BSRead(bsp,rdbuf,4);
		/* add up the file as blocks of 4 */
		cksum[0]+=(Byte)rdbuf[0];
		cksum[1]+=(Byte)rdbuf[1];
		cksum[2]+=(Byte)rdbuf[2];
		cksum[3]+=(Byte)rdbuf[3];
	}
	if (slack>0)
		BSRead(bsp,rdbuf,slack);
	BSRead(bsp,rdbuf,4);
	if (rdbuf[0]=='Q' && rdbuf[1]=='Z') {
		BSSeek(bsp,-2L,SEEK_CUR);
		BSRead(bsp,dbrdbuf,8);
		dbrdbuf[8]='\0';
		sscanf(dbrdbuf,"%8lX",&lpieces);
		if (cnt==(Uint4)lpieces)
			magic=TRUE;
	}
	if (magic==FALSE)
		return FALSE;
	BSRead(bsp,dbrdbuf,8);
	dbrdbuf[8]='\0';
	bytebuf[2]='\0';
	bytebuf[0]=dbrdbuf[0];
	bytebuf[1]=dbrdbuf[1];
	sscanf(bytebuf,"%2X",&ckTest0);
	bytebuf[0]=dbrdbuf[2];
	bytebuf[1]=dbrdbuf[3];
	sscanf(bytebuf,"%2X",&ckTest1);
	bytebuf[0]=dbrdbuf[6];
	bytebuf[1]=dbrdbuf[7];
	sscanf(bytebuf,"%2X",&ckTest3);
	BSRead(bsp,dbrdbuf,8);
	sscanf(dbrdbuf,"%8lX",&lendian);
	if ((lendian!=0x44434241) && (lendian!=0x41424344))
		return FALSE;
	if (cksum[0]!=(Byte)ckTest0)
		return FALSE;
	if (cksum[1]!=(Byte)ckTest1)
		return FALSE;
	/* dont check byte 2 - it is randomized */
	if (cksum[3]!=(Byte)ckTest3)
		return FALSE;
	return TRUE;
}

void ComputeFileChecksum(CharPtr fnam)
{
	FILE *f;
	Uint4 numpiece,cnt;
	Char rdbuf[4];
	Byte cksum[4]={0,0,0,0};
	Uint4 endian;
	Char checksum[28];
	Int4 fl;
	
	if (fnam==NULL)
		return;
	fl=(Int4) FileLength(fnam);
	f=FileOpen(fnam,"rb");
	if (f==NULL)
		return;
	numpiece=fl/4;
	for (cnt=0;cnt<numpiece;cnt++) {
		FileRead(rdbuf,sizeof(Char),4,f);
		/* add up the file in blocks of 4 */
		cksum[0]+=(Byte)rdbuf[0];
		cksum[1]+=(Byte)rdbuf[1];
		cksum[2]+=(Byte)rdbuf[2];
		cksum[3]+=(Byte)rdbuf[3];
	}
	endian=0x41424344;
	FileClose(f);
	f=FileOpen(fnam,"ab");
	/* make middle/end 8 bits random to mess people up */
	cksum[2]+=(GetSecs()%256);
	/* now write checksum: QZ cksum endian */
	sprintf(checksum,"QZ%8lX%2X%2X%2X%2X%8lX",(long)numpiece,cksum[0],cksum[1],cksum[2],cksum[3],(long)endian);
	FileWrite(checksum,sizeof(Char),StringLen(checksum),f);
	FileClose(f);
}

Boolean StripFileChecksum(CharPtr fnam)
{
	Int4 fl;
	FILE *f;
	CharPtr tmpbuf;
	
	if (fnam==NULL)
		return FALSE;
	fl=(Int4)FileLength(fnam);
	if (fl<CHECKSUM_LENGTH)
		return FALSE;
	tmpbuf=(CharPtr)MemNew(sizeof(Char)*(fl-CHECKSUM_LENGTH));
	f=FileOpen(fnam,"rb");
	FileRead(tmpbuf,sizeof(Char),fl-CHECKSUM_LENGTH,f);
	FileClose(f);
	FileRemove(fnam);
	f=FileOpen(fnam,"wb");
	FileWrite(tmpbuf,sizeof(Char),fl-CHECKSUM_LENGTH,f);
	FileClose(f);
	tmpbuf=MemFree(tmpbuf);
	return TRUE;
}

Boolean CheckFileChecksum(CharPtr fnam)
{
	Uint4 numpiece,cnt;
	Char rdbuf[4];
	Char bytebuf[3];
	Char dbrdbuf[9];
	Byte cksum[4]={0,0,0,0};
	Int4 ckTest0=0,ckTest1=0,ckTest3=0;
	long lendian,lpieces;
	Boolean magic=FALSE;
	Int4 fl,slack;
	FILE *f;

	if (fnam==NULL)
		return FALSE;
	fl=(Int4)FileLength(fnam);
	if (fl<CHECKSUM_LENGTH)
		return FALSE;
	f=FileOpen(fnam,"rb");
	if (f==NULL)
		return FALSE;
	/* checksum is 26 characters */
	numpiece=(fl-CHECKSUM_LENGTH)/4;
	slack=(fl-CHECKSUM_LENGTH)%4;
	for (cnt=0;cnt<numpiece;cnt++) {
		FileRead(rdbuf,sizeof(Char),4,f);
		/* add up the file as blocks of 4 */
		cksum[0]+=(Byte)rdbuf[0];
		cksum[1]+=(Byte)rdbuf[1];
		cksum[2]+=(Byte)rdbuf[2];
		cksum[3]+=(Byte)rdbuf[3];
	}
	if (slack>0)
		FileRead(rdbuf,sizeof(Char),slack,f);
	FileRead(rdbuf,sizeof(Char),4,f);
	if (rdbuf[0]=='Q' && rdbuf[1]=='Z') {
		fseek(f,-2L,SEEK_CUR);
		FileRead(dbrdbuf,sizeof(Char),8,f);
		dbrdbuf[8]='\0';
		sscanf(dbrdbuf,"%8lX",&lpieces);
		if (cnt==(Uint4)lpieces)
			magic=TRUE;
	}
	if (magic==FALSE) {
		FileClose(f);
		return FALSE;
	}
	FileRead(dbrdbuf,sizeof(Char),8,f);
	dbrdbuf[8]='\0';
	bytebuf[2]='\0';
	bytebuf[0]=dbrdbuf[0];
	bytebuf[1]=dbrdbuf[1];
	sscanf(bytebuf,"%2X",&ckTest0);
	bytebuf[0]=dbrdbuf[2];
	bytebuf[1]=dbrdbuf[3];
	sscanf(bytebuf,"%2X",&ckTest1);
	bytebuf[0]=dbrdbuf[6];
	bytebuf[1]=dbrdbuf[7];
	sscanf(bytebuf,"%2X",&ckTest3);
	FileRead(dbrdbuf,sizeof(Char),8,f);
	sscanf(dbrdbuf,"%8lX",&lendian);
	FileClose(f);
	if ((lendian!=0x44434241) && (lendian!=0x41424344))
		return FALSE;
	if (cksum[0]!=(Byte)ckTest0)
		return FALSE;
	if (cksum[1]!=(Byte)ckTest1)
		return FALSE;
	/* dont check byte 2 - it is randomized */
	if (cksum[3]!=(Byte)ckTest3)
		return FALSE;
	return TRUE;
}

/* converts the date into a string of format YYYYMMDD, if date is
   NULL, YYYYMMDD is today's date. date must have the year, month
   and day set properly. Caller must free the string.
 */
CharPtr LIBCALL GetStringDate(NCBI_DatePtr date)
{
  Char year[5] = {0};
  Char month[3] = {0};
  Char day[3] = {0};
  CharPtr cpdate = NULL;
  
 if(date == NULL)
  {
    date = DateCurr();
  }

  cpdate = (CharPtr) MemNew(9 * sizeof(Char));
  sprintf(year, "%d", date->data[1] + 1900); strcpy(cpdate, year);
  if(date->data[2] < 10) strcat(cpdate, "0");
  sprintf(month, "%d",date->data[2]); strcat(cpdate, month);
  if(date->data[3] < 10) strcat(cpdate, "0");
  sprintf(day, "%d", date->data[3]); strcat(cpdate, day);
  return cpdate;
}

/* SK: This function replaces unstable function TrimSpacesAroundString */

CharPtr Misc_TrimSpacesAroundString ( CharPtr str )
{
    char *ptr, *dst, *revPtr;
    int spaceCounter = 0;

    ptr = dst = revPtr = str;
    
    if ( !str || str[0] == '\0' )
	return str;

    while ( *revPtr != '\0' )
	if ( *revPtr++ <= ' ' )
	    spaceCounter++;
    
    if ( (revPtr - str) <= spaceCounter )
    {
	*str = '\0';
	return str;
    }

    while ( revPtr > str && *revPtr <= ' ' ) revPtr--;
    while ( ptr < revPtr && *ptr <= ' ' ) ptr++;
    while ( ptr <= revPtr ) *dst++ = *ptr++;
    *dst = '\0';
    
    return str;
}

/* like fgets but for bytestore */
CharPtr BSGets(CharPtr dest,Int2 len,ByteStorePtr bsp)
{
	Int2 ptr=0;
	Char ch;

	do {
		ch= (Char) BSGetByte(bsp);
		if (ch==EOF) break;
		dest[ptr]=ch;
		ptr++;
		if (ptr+1>=len) break;
		if (ch=='\n') break;
	} while (1);
	dest[ptr]='\0';
	if (ptr==0) return NULL;
	return dest;
}


SLRI_ERR LIBCALL GetAppTime(FloatLo *pftime)
{
	CPUTimePtr pTime;
	if((pTime = CPUTimeMeasure()) == NULL)  return SLRI_FAIL;
	*pftime = (FloatLo) CPUTimeGetUser(pTime);
	*pftime += (FloatLo) CPUTimeGetSys(pTime);
	CPUTimeFree(pTime);
	return SLRI_SUCCESS;
}



CharPtr LIBCALL MyGetFile(CharPtr pcFileName)
{
	CharPtr function = "MGetFile";
	CharPtr pc = NULL;
	Int4 len = 0, num = 0;
	FILE *fp = NULL;
	
	if(pcFileName == NULL) {
		ErrPostEx(SEV_ERROR,0,0,"%s:NULL file name",function);
		return NULL;
	}
	len = (Int4) Nlm_FileLength(pcFileName);
	if(len == 0) {
		ErrPostEx(SEV_ERROR,0,0,"%s:File %s does not exist (file length 0)", function, pcFileName);
		return NULL;
	}
	if((pc = (CharPtr) MemNew((size_t)sizeof(Char)*(len+1))) == NULL) {
		ErrPostEx(SEV_ERROR,0,0,"%s:Unable to allocate enough memory to read file %s",function, pcFileName);
		return NULL;
	}
	if((fp = FileOpen(pcFileName,"rb")) == NULL) {
		ErrPostEx(SEV_ERROR,0,0,"%s:Unable to open %s",function, pcFileName);
		MemFree(pc);
		return NULL;
	}
	num = Nlm_FileRead(pc,sizeof(char), (size_t) len, fp);
	FileClose(fp);
	if(num != len) {
		ErrPostEx(SEV_ERROR,0,0,"%s:Read %ld of %ld bytes", function, (long) num, (long) len);
		MemFree(pc);
		return NULL;
	}
	return pc;
}

CharPtr LIBCALL MyGetLine(CharPtr buf, CharPtr *pcur, CharPtr *pdest)
{
	CharPtr function = "MGetLineEx";
	CharPtr end = NULL, pc = NULL;
	Int4 len;

	if(buf == NULL) {
		ErrPostEx(SEV_ERROR,0,0,"%s:file buffer is NULL",function);
		return NULL;
	}
	if(pcur == NULL) {
		ErrPostEx(SEV_ERROR,0,0,"%s:You must use a valid current pointer");
		return NULL;
	}

	if(*pcur == NULL) {
		*pcur = buf;
	}
	/** Find the next instance of the new line character */
	if ((end=StringChr(*pcur,(int)'\n')) == NULL) {
		return NULL;
	}
	end++;
	len = end - *pcur;
	if(pdest) {
		/** if destination pointer is provided, allocate memory and copy */
		*pdest = MemFree(*pdest);
		pc = (CharPtr) MemNew((size_t)sizeof(Char)*(len+1));
		if(pc == NULL) {
			ErrPostEx(SEV_ERROR,0,0,"%s:Unable to allocate memory",function);
			return NULL;
		}
		StringNCpy(pc,*pcur,len);
		*pdest = pc;
	}
	*pcur += len;
	return *pcur;
}

void LIBCALL SubstituteSubString ( CharPtr istr, CharPtr search, CharPtr sub, CharPtr ostr)
{
	char *p;
	
	if (strstr(istr, search) == NULL) {
		sprintf(ostr, "%s", istr);
	} else {
		p = strtok(istr, search);
		sprintf(ostr, "%s%s", p, sub);
		do {
			p = strtok(NULL, search);
			if (p) {
				strcat(ostr, p);
				strcat(ostr, sub);
			}
		} while (p);
		ostr[strlen(ostr)-1] = '\0';
	}
} 

/* Function will break up an input string into substrings based on the input separator string. Substrings are stored into an array, which is returned to the user. Note
that the input string is not altered by the process. The final element in all output arrays is set to NULL.*/
CharPtr* StringBreaker(CharPtr string, CharPtr sep) {
        CharPtr *substrings = NULL;
        CharPtr stringCpy = NULL, start = NULL, end = NULL, next = NULL, info = NULL;
        Int4 numSubStrings = 0, x;
                                                
        stringCpy = StringSave(string);
        start = stringCpy;
        while(start) {
		/* This will occur is the sep. character is the last one in the input string */
		if(start[0] == '\0') {
			start = NULL;
			continue;
		}
                if(end = StringStr(start, sep)) {
                        numSubStrings++;
                        next = end;
                        next++;
                        *end = '\0';
                        start = next;
                }
                else {                   
                        start = NULL;
                }
        }
        
        numSubStrings++;
        substrings = (CharPtr*)MemNew((numSubStrings + 1)*sizeof (CharPtr));
        
        start = stringCpy;
        next = stringCpy;
        for(x = 0; x < numSubStrings; x++) {
                substrings[x] = StringSave(start);
		Misc_TrimSpacesAroundString(substrings[x]);
                start = StringChr(next, '\0');
                if(x != (numSubStrings - 1)) {
                        start++;
                        next = start;
                }                                       
        }
        substrings[x] = NULL;
        
        stringCpy = MemFree(stringCpy);
                        
        return substrings;
}

/* Function frees an array of strings */
void FreeStrArr(CharPtr *array) {
        CharPtr stringFree = NULL;
        Int4 x = 0;                      
                        
        stringFree = array[x];
        while(stringFree) {
                stringFree = MemFree(stringFree);
                x++;
                stringFree = array[x];
        }
        
        array = MemFree(array);
}

/* Function returns the number of elements in an array of Strings */
Int4 GetNumStrArrUnits(CharPtr *array) {
	Int4 numUnits = 0, x = 0;
	CharPtr unit = NULL;

	unit = array[x];
	while(unit) {
		numUnits++;
		x++;
		unit = array[x]; 
	}
	
	return numUnits;
}

/*
$Log: slri_misc.c,v $
Revision 1.17  2005/03/14 19:27:45  ksnyder
Modified StringBreaker function to set last array node to NULL. Trim spaces around each substring in array; Changed function FreeBreakerArr to FreeStrArr, which is more generic. Function frees strings in an array whose last element is NULL; Added function GetNumStrArrUnits

Revision 1.16  2005/03/07 20:08:46  ryao
Modified Misc_StrArray2ValNodeList to adapt separator as a sequence of chars. Previous implementation uses strtok() which can only treat separator as a single character.

Revision 1.15  2005/03/07 17:27:10  ryao
Modified Misc_Str2StrArray to adapt separator as a sequence of chars. Previous implementation uses strtok() which can only treat separator as a single character.

Revision 1.14  2005/01/06 22:33:42  ksnyder
Added StringBreaker and FreeBreakerArr functions; StringBreaker breaks up an input string into substrings based on the input separator string.  The substrings are stored in an array, which is returned to the user. The FreeBreakerArr function frees the array returned by StringBreaker

Revision 1.13  2004/11/17 17:34:14  ryao
Added NULL pointer check in Misc_StrArray2ValNodeList.

Revision 1.12  2004/07/19 16:46:55  vgu
Remove C++ style comment

Revision 1.11  2004/07/12 20:38:58  ysun
change in Slri_ValNodeAddPointerAtEnd() to allow to add value into an empty list

Revision 1.10  2004/07/08 20:02:34  ryao
added Slri_FreeKeyedList Slri_FreeFastaKeyedList; changed prototype of Slri_PrintValNodeList to Slri_PrintKeyedList, Slri_PrintFastaValNodeList to Slri_PrintFastaKeyedList

Revision 1.9  2004/06/18 15:43:26  zwang
Change in SubstituteSubString: get rid of the trailing character added to the string which has a matched substring..

Revision 1.8  2004/06/16 20:41:47  zwang
Added function SubstituteSubString for substituting a substring with a new one.

Revision 1.7  2004/04/05 16:41:01  ryao
Added helper functions for processing list API functions. New functions: Slri_ValNodeAddPointerAtEnd Slri_PrintValNodeList Slri_PrintFastaValNodeList Misc_Str2StrArray Misc_StrArray2ValNodeList

Revision 1.6  2004/02/19 23:47:07  mjdumont
Changed function name due to conflict

Revision 1.5  2004/02/09 19:55:17  egarderm
Added explicit check for checksum length

Revision 1.4  2004/02/02 20:54:53  mjdumont
added GetAppTime and MGetLine

Revision 1.3  2003/11/14 21:21:43  feldman
Added BSGets

Revision 1.2  2003/10/16 20:59:50  skuznets

1. Checking for NULL pointers when print values
2. Added function Misc_TrimSpacesAroundString
3. Added macro IsNullStr

Revision 1.1.1.33  2003/06/12 08:38:20  root
Automated import from Sourceforge.net

Revision 1.38  2003/06/11 18:05:23  mdumonti
added a function for unique DValNode lists

Revision 1.37  2003/03/19 21:47:41  micheld
minor changes

Revision 1.36  2003/02/12 22:09:54  micheld
Fixed bug in FASTA parser

Revision 1.35  2003/01/09 00:52:19  micheld
Changed buffer size

Revision 1.34  2002/10/20 17:15:42  micheld
Added VNP<->FloatLoArray and stat functions

Revision 1.33  2002/08/15 19:45:41  kaca
moved variable declaration in GetStringDate to the top of the function

Revision 1.32  2002/08/15 16:39:48  haocl
reverted to old version and added GetStringDate

Revision 1.30  2002/07/09 14:11:59  gbader
Added flush function to ByteStore logging.

Revision 1.29  2002/06/28 22:23:24  gbader
Fixed get column function.

Revision 1.28  2002/06/10 23:41:16  micheld
bug fix

Revision 1.27  2002/05/21 21:52:08  gbader
Fixed generic column parsing functions for parsing files with a single column.

Revision 1.26  2002/04/11 20:26:21  gbader
Added another add to unique list function.

Revision 1.25  2002/04/06 01:36:04  gbader
Fix in generic column write function to write out proper number of columns.

Revision 1.24  2002/04/03 19:33:46  mdumonti
added a valnode filtering function

Revision 1.23  2002/03/25 06:44:22  micheld
Added comprehensive suite of linksets (only clinkset, flinkset and ilinkset tested)

Revision 1.22  2002/03/11 23:00:29  gbader
Added unique functions that maintain order - slightly slower in worst case from existing unique functions that don't maintain order.

Revision 1.21  2002/03/07 02:10:13  gbader
Fixed generic column parse - now returns error at EOF

Revision 1.20  2002/03/06 00:47:06  gbader
Added comment

Revision 1.19  2002/03/01 23:57:45  gbader
Fixed compiler warnings
Moved generic asn set read/write mode to slri_asn

Revision 1.18  2002/02/25 18:25:44  gbader
Added choice copying to valnode list copy function

Revision 1.17  2002/02/15 21:52:29  gbader
Fixed bug in generic columns

Revision 1.16  2002/02/11 19:24:09  micheld
Moved asn out of seqhound and into library (this file should generate all future slrilib asn structs)
Added asn auto-generator script files
Added slri.h to be all-inclusive include file (does not include database specific files)

Revision 1.15  2002/02/04 16:42:55  micheld
Added floathi array <=> valnodelist

Revision 1.14  2002/01/18 20:49:58  micheld
Added StrToInt8 function

Revision 1.13  2002/01/16 03:31:40  gbader
Added another generic get column utility function.

Revision 1.12  2002/01/15 16:48:21  betel
Changed 'this' to 'me' in byte-swap functions

Revision 1.11  2002/01/14 21:23:08  micheld
Added valnode list partition function to split up long valnode lists into a list of lists
of size given by function caller

Revision 1.10  2001/12/14 23:40:06  gbader
Added error checking in generic column parse function

Revision 1.9  2001/12/12 17:10:25  gbader
Fixed return value in Misc_GenericColumnGetNumberRows

Revision 1.8  2001/12/12 16:51:38  gbader
Added count row function to generic column parse module

Revision 1.7  2001/12/11 04:51:42  gbader
Misc_GenericColumnParse - advance row function now properly signals end of file.

Revision 1.6  2001/12/09 19:08:47  gbader
Added convenience functions to generic column parser

Revision 1.5  2001/12/04 22:56:23  micheld
added int4 array to valnodelist converters

Revision 1.4  2001/11/29 04:26:49  gbader
Added find in list function

Revision 1.3  2001/11/28 20:46:33  gbader
Minor change

Revision 1.2  2001/11/27 00:56:29  gbader
Fixed bug in Misc_RemoveIntFromSortedList

Revision 1.1.1.3  2001/10/10 22:11:04  kaca
initial import

Revision 1.58  2001/09/07 01:18:24  gbader
Added binary word search function.

Revision 1.57  2001/09/06 21:43:52  gbader
Strengthened return codes in Misc_AddInt2UniqueList

Revision 1.56  2001/09/03 23:42:36  gbader
Fixed Misc_AddInt2UniqueList

Revision 1.55  2001/09/03 23:03:22  gbader
Fixed Misc_AddInt2UniqueList

Revision 1.54  2001/09/03 20:20:51  gbader
Added Misc_AddInt2UniqueList function

Revision 1.53  2001/09/03 19:39:20  gbader
Fixed compiler warnings in Misc_GenericColumnWrite

Revision 1.52  2001/08/30 18:34:29  gbader
Made some comments more clear

Revision 1.51  2001/08/30 14:59:48  michel
fixed compiler warning

Revision 1.50  2001/08/28 17:59:27  gbader
Added a free function for file parsing functions.  Tested and run through purify.

Revision 1.49  2001/08/28 16:32:00  gbader
Added File parsing section and functions

Revision 1.48  2001/08/27 02:09:01  gbader
Fixed EOF problem in BSGetLineFromFile

Revision 1.47  2001/08/21 15:46:43  gbader
Added Misc_SaveFloatHiValNodeListAsString and generalized vnp->string functions.

Revision 1.46  2001/08/09 22:36:23  gbader
Added more modular implementation of StringFree callback function for Misc_VnpUniqueEx function.

Revision 1.45  2001/08/09 16:57:49  gbader
Added option to free data in Misc_StringVnpUniqueEx and added Misc_StringVnpUniqueAndFree function

Revision 1.44  2001/07/24 15:38:44  haocl
added method BSRemoveLeadingChar

Revision 1.43  2001/06/26 20:10:57  gbader
Updated comments

Revision 1.42  2001/06/18 14:44:31  gbader
Removed unused variable.

Revision 1.41  2001/06/05 22:23:12  gbader
Added randomize linked list function (Useful for random models)

Revision 1.40  2001/05/23 21:53:00  gbader
Fixed Misc_GenericGetNumberofNodes to return correct number of nodes and added Misc_GenericRemoveObjectFromList

Revision 1.39  2001/04/13 02:34:28  gbader
Added CopyValNodeStringlist function

Revision 1.38  2001/04/11 15:09:34  feldman
removed \n from end of checksum, not needed

Revision 1.37  2001/04/11 14:49:47  feldman
Rewrote checksum functions again, now they are easily removed with
handy Strip functions, and shoudl work fine even on binary files

Revision 1.36  2001/04/08 22:18:31  feldman
fixed compiler warnings

Revision 1.35  2001/04/06 21:36:51  feldman
Rewrote checksum function - should work now independent of Endianness
and no Bus Errors either ;)

Revision 1.34  2001/04/06 14:48:17  feldman
Added byteflipping functions

Revision 1.33  2001/04/06 14:27:24  feldman
Added checksum functions for file verification

Revision 1.32  2001/04/04 22:22:00  gbader
Fixed memory leak

Revision 1.31  2001/04/03 14:05:33  michel
Removed ReadDB function - required ncbitool.lib -> moved to nblast_cb

Revision 1.30  2001/04/02 19:54:28  michel
added generic object list counter

Revision 1.29  2001/04/02 18:48:01  michel
Moved math functions to slri_math[c.h]
Added Misc_ReadDBGetGiFromOrd
Added Modified LinkSet (SLRILinkset) for use with FloatHis, and nested Linksetlists
Modified make files to include these new files

Revision 1.28  2001/03/30 21:27:50  michel
Added Int8toStr, SLRI_SafeLog10 and SLRI_SafeExp10 for general use
Added support for using generalized SLRI_ERR return type and values

Revision 1.27  2001/03/29 22:14:09  gbader
Moved statics to before function prototype

Revision 1.26  2001/03/28 01:20:15  gbader
Added Misc_IsStrInStrVnpList function

Revision 1.25  2001/03/27 20:49:45  gbader
Fixed Misc_CommaIntString2ValNodeListEx bug

Revision 1.24  2001/03/26 21:57:30  gbader
Reimplemented and fixed Unique functions - wasn't taking into account 1st element of list

Revision 1.23  2001/03/26 19:39:49  gbader
Fixed bug in ordered list function

Revision 1.22  2001/03/26 19:27:20  gbader
Added new ordered list functions

Revision 1.21  2001/03/20 20:23:34  gbader
Minor changes

Revision 1.20  2001/03/12 14:10:09  gbader
removed unused variables in two string vnp list functions

Revision 1.19  2001/03/09 20:00:07  gbader
Changed behaviour of Slri_ValNode functions to save head value in first 'passed by reference' parameter (like NCBIValNode functions)

Revision 1.18  2001/03/08 00:12:34  gbader
Added DbTag constructor functions.  Added functions to interconvert string lists to ValNode lists

Revision 1.17  2001/01/13 04:04:10  gbader
Added IBM DB2 UDB to license exceptions.

Revision 1.16  2000/12/26 14:02:28  gbader
Finished IP address checking function

Revision 1.15  2000/12/23 21:58:14  gbader
added numerical IP address validation function

Revision 1.14  2000/11/23 20:00:36  gbader
optimized sorted list function

Revision 1.13  2000/09/13 18:42:07  gbader
minor style fix

Revision 1.12  2000/09/09 17:46:39  gbader
Added (long) type casts for all Int4 variables used with ANSI format string functions (e.g. printf) to avoid -Wall warnings on Linux

Revision 1.11  2000/09/08 20:06:05  gbader
minor changes

Revision 1.10  2000/09/05 13:55:08  gbader
Updated License so that it is now GNU.
Added CVS keywords ID and Log as well as new line at end of files so compilers on some platforms don't complain

*/

