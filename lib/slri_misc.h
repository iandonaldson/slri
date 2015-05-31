/* $Id: slri_misc.h,v 1.13 2005/03/14 19:28:55 ksnyder Exp $

-- ***************************************************************************
--
--  Samuel Lunenfeld Research Institute (SLRI)
--
--  File: slri_misc.h
--  Description: General utility functions
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

#ifndef _SLRI_MISC_
#define _SLRI_MISC_

#include <ncbi.h>
#include <objall.h>
#include <objslrilibstruc.h>
#include <dvncode.h>

#ifdef __cplusplus
extern "C" {
#endif

/* RY: list API function handling*/
#define GROUP "#;#"   
#define Q_R "#~#"

/* General Function return and error codes*/

typedef Int2 SLRI_ERR;

#define SLRI_SUCCESS  (SLRI_ERR) 0
#define SLRI_FAIL     (SLRI_ERR) 1
#define SLRI_NOMEM    (SLRI_ERR) 2
#define SLRI_CONTINUE (SLRI_ERR) 3
#define SLRI_EXISTS   (SLRI_ERR) 4

/*Use instead of PATH_MAX to indicate a buffer*/
#ifndef SLRI_BUF_MAX
#define SLRI_BUF_MAX 256
#endif

/*Generic linked list structure*/
typedef struct struct__Generic_linked_list {
    struct struct__Generic_linked_list PNTR next;
} SlriGenericLinkedList, PNTR SlriGenericLinkListPtr;


/************************/
/* Function definitions */
/************************/

/*************************/
/*Miscellaneous Functions*/
/*************************/

/* Converts the specified array of iArraysize size to a ValNodeList, maintains list order */
extern SLRI_ERR Misc_Int4ArrayToValNodeList(Int4Ptr piArray, Int4 iArraysize, ValNodePtr PNTR ppvnlist);
extern SLRI_ERR Misc_FloatHiArrayToValNodeList(FloatHiPtr pfArray, Int4 iArraysize, ValNodePtr PNTR ppvnlist);
extern SLRI_ERR Misc_FloatLoArrayToValNodeList(FloatLoPtr pfArray, Int4 iArraysize, ValNodePtr PNTR ppvnlist);
extern SLRI_ERR Misc_CharPtrArrayToValNodeList(CharPtr *pcArray, Int4 iArraysize, ValNodePtr PNTR ppvnlist);
/* Converts a ValNodeList of type to array of piArraySize size, maintains list order*/
extern SLRI_ERR Misc_ValNodeListToInt4Array(ValNodePtr pvnlist, Int4Ptr PNTR ppiArray, Int4 PNTR piArraysize);
extern SLRI_ERR Misc_ValNodeListToFloatHiArray(ValNodePtr pvnlist, FloatHiPtr PNTR ppfArray, Int4 PNTR piArraysize);
extern SLRI_ERR Misc_ValNodeListToFloatLoArray(ValNodePtr pvnlist, FloatLoPtr PNTR ppfArray, Int4 PNTR piArraysize);
extern SLRI_ERR Misc_ValNodeListToCharPtrArray(ValNodePtr pvnlist, CharPtr PNTR *ppcArray, Int4 PNTR piArraysize);


/*Saves a ValNode list of integers as a string of comma separated numbers
NOTE: allocates memory - free returned string*/
extern CharPtr Misc_SaveValNodeListAsString(ValNodePtr vnp);

/*Converts a ValNode list of strings to a comma separated list of strings*/
extern CharPtr Misc_SaveStrValNodeListAsString(ValNodePtr vnp);

/*Converts a ValNode list of FloatHis to a comma separated list of strings*/
extern CharPtr Misc_SaveFloatHiValNodeListAsString(ValNodePtr vnp);

/*converts a "sep" separated string of integers to a ValNode list of integers*/
/*fast version*/
extern ValNodePtr Misc_CommaIntString2ValNodeList(CharPtr string, CharPtr seps);

/*converts a "sep" separated string of integers to a ValNode list of integers*/
/*slow version*/
extern ValNodePtr Misc_CommaIntString2ValNodeListOrdered(CharPtr string, CharPtr seps);

/* Converts a seps separated string to a ValNode list of strings */
/*fast version*/
extern ValNodePtr Misc_CommaStrString2ValNodeList(CharPtr string, CharPtr seps);

/*converts a "sep" separated string of strings to a ValNode list of strings*/
/*slow version*/
extern ValNodePtr Misc_CommaStrString2ValNodeListOrdered(CharPtr string, CharPtr seps);

/*returns TRUE if query string is in the given sorted list - NULL terminated.*/
/*deprecated - use Misc_BinaryWordSearch instead*/
extern Boolean Misc_IsStringInList(CharPtr sortedList[], CharPtr query);

/*checks if a word is in an ascending sorted list of words*/
/*Return values: -2 parameter error, -1 not found, n>=0 index of found word*/
extern Int4 Misc_BinaryWordSearch(CharPtr word, CharPtr sortedWordList[], Int4 listLen, Boolean caseSensitive);

/*find the index of name in a string array of numElmts - returns -1 in case of failure*/
extern Int4 Misc_IndexCharPtrArray(CharPtr array[], Int4 numElmts, CharPtr name);

/*Find an integer in an integer list*/
extern ValNodePtr Misc_FindIntInIntList(ValNodePtr list, Int4 num);

/*returns TRUE if an integer is in the given number list.*/
extern Boolean Misc_IsIntInList(ValNodePtr list, Int4 num);

/*returns TRUE if a string is in the given ValNode string list - optionally case sensitive.*/
extern Boolean Misc_IsStrInStrVnpList(ValNodePtr list, CharPtr string, Boolean caseSensitive);

/*returns TRUE if an integer with given choice is in the given integer list.*/
extern Boolean Misc_IsIntChoiceInList(ValNodePtr list, Int4 num, Int2 choice);

/*Adds an integer to a sorted valnode list*/
/*If unique is set, will not add to a list that already has the given integer*/
extern Int2 Misc_AddInt2SortedList(ValNodePtr *vnp, Int4 num, Int2 choice, Boolean unique);

/*Removes an integer from a sorted valnode list*/
/*If there are duplicate integers, only one will be removed*/
extern Int2 Misc_RemoveIntFromSortedList(ValNodePtr *vnp, Int4 num);

/*Adds an integer to the end of a non-sorted valnode list, but maintains uniqueness of num*/
extern Int2 Misc_AddInt2UniqueList(ValNodePtr *vnpList, Int2 choice, Int4 num);

/*Adds an integer to the end of a non-sorted valnode list, but maintains uniqueness of num and choice*/
/*returns 0 if the integer and choice was added, -1 if not and for errors*/
extern Int2 Misc_AddIntChoice2UniqueListByNumAndChoice(ValNodePtr *vnpList, Int2 choice, Int4 num);

/*Add a string to a unique DValNode */ 
extern void Misc_AddString2DVnpUnique(DValNodePtr *vnp, Int4 choice, CharPtr str);

/*returns 1 if string is equivalent to TRUE
  returns 0 if string is equivalent to FALSE
  returns -1 if string is equivalent to neither*/
extern Int2 Misc_IsStringTrueOrFalse(CharPtr string);

/* SK: This function replaces TrimSpacesAroundString, which causes coredumps on Solaris */
extern CharPtr Misc_TrimSpacesAroundString(CharPtr string);

/*remove a vnp from a list*/
extern ValNodePtr Misc_RemoveNode(ValNodePtr head, ValNodePtr x);

/*tests if a given string is numeric*/
extern Boolean Misc_IsNumeric(CharPtr string);

/*tests if a given string is numeric - real number*/
extern Boolean Misc_IsFloatNumber(CharPtr string);

/*callback functions for sorting*/
/*callback function for vnpHeapSort - numerical ascending sort*/
extern int LIBCALLBACK Misc_NumSortAscendCallBack(Nlm_VoidPtr vp1, Nlm_VoidPtr vp2);

/*callback function for vnpHeapSort - string ascending sort*/
extern int LIBCALLBACK Misc_StringSortAscendCallBack(Nlm_VoidPtr vp1, Nlm_VoidPtr vp2);

/*sorts and uniques a vnp filled with integers*/
extern void Misc_IntVnpUnique(ValNodePtr *vnp);

/*uniques a vnp filled with strings*/
extern void Misc_StringVnpUnique(ValNodePtr *vnp);

/*uniques a vnp filled with strings and frees the associated string of any deleted list nodes*/
extern void Misc_StringVnpUniqueAndFree(ValNodePtr *vnp);

/*uniques a vnp filled with integers, but maintains order*/
extern void Misc_IntVnpUniqueOrdered(ValNodePtr *vnp);

/*uniques a vnp filled with strings, but maintains order*/
extern void Misc_StringVnpUniqueOrdered(ValNodePtr *vnp);

/*uniques a vnp filled with strings, but maintains order and frees the associated string of any deleted list nodes*/
extern void Misc_StringVnpUniqueOrderedAndFree(ValNodePtr *vnp);

/* filters out all entries in the valnode with the specified choice and returns a new valnode*/
extern ValNodePtr Misc_FilterValNodeByChoice(ValNodePtr vnpHead, Int2 choice);

/*returns the number of nodes in a ValNode linked list*/
extern Int4 Misc_CountVnp(ValNodePtr vnp);

/*given a seq-loc, returns a sequence # string - opposite to SeqString2SeqLoc
NOTE: allocates memory! - take care of CharPtr returned!*/
extern CharPtr Misc_SeqLoc2SeqString(SeqLocPtr seqLoc);

/*given a sequence string e.g. 3,14-45,58 produce a seq-loc containing them*/
extern SeqLocPtr Misc_SeqString2SeqLoc(CharPtr stringInput, Int4 sequenceGi);

/*Checks an e-mail address to see it it's formatted in name@server.ext*/
/*taken from unknown code snippet page and toolkit'ized*/
extern Boolean Misc_GoodEmailAddress(CharPtr email);

/*Checks a phone number to see if it's formatted properly*/
extern Boolean Misc_GoodPhoneNumber(CharPtr number);

/*Checks an ip address to see if it's formatted properly*/
extern Boolean Misc_GoodIPAddress(CharPtr address);

/*strip quotes from end of a word*/
extern CharPtr Misc_StripQuotes(CharPtr word);

/*Copy an Int8 to a pre-allocated String for printf, ErrpostEx functions*/
extern SLRI_ERR Int8toStr(Int8 num, CharPtr pcbuf);

/* calls appropriate scanf on a string and places the scanned value into an Int8
function returns sscanf return value */
extern int StrtoInt8(CharPtr pcbuf, Int8 PNTR inum);

#define INTERVAL_INDIVIDUAL 1
#define INTERVAL_FROM 2
#define INTERVAL_TO 3

/* Generates a linked list of ValNodePtr sublists according to specified length.  InList is set to NULL by this call! */
extern SLRI_ERR Misc_PartitionValNodeList(ValNodePtr PNTR vnpInList, ValNodePtr PNTR vnpOutList, Int4 length);

/*convert a string of format e.g. 3,14-45,58 to a valnode with specialized choice values*/
extern ValNodePtr Misc_IntervalString2ValNode(CharPtr string);

/*gets the next element from an integer interval valnode list - user must free returned valnode*/
extern ValNodePtr Misc_IntervalValNodeNextElement(ValNodePtr *vnpIntervalList);

/*copy an integer ValNodePtr list*/
extern ValNodePtr Misc_ValNodeIntegerCopy(ValNodePtr vnp);

/*copy a string ValNodePtr list*/
extern ValNodePtr Misc_ValNodeStringListCopy(ValNodePtr vnp);

/*generically adds a pointer to the end of any linked list that contains a next field at the top of the structure*/
extern Int2 Misc_GenericAddObjectToListEnd(Pointer *list, Pointer addObj);

/*generically adds a pointer to the head of any linked list that contains a next field at the top of the structure*/
extern Int2 Misc_GenericAddObjectToListStart(Pointer *list, Pointer addObj);

/*generically removes a node from any linked list that contains a next field at the top the the node structure (e.g. ASN.1 generated code)*/
extern SLRI_ERR Misc_GenericRemoveObjectFromList(Pointer *listHead, Pointer removeObj, AsnOptFreeFunc yourObjectFreeFunc);

/*generically determines the number of objects in list */
extern SLRI_ERR Misc_GenericGetNumberofNodes(Pointer list, Int4 PNTR num);

/* slri_ ValNodeAdd* functions behave the same as the ValNodeAdd functions from NCBI
but put the new node at the begining of the list */
extern ValNodePtr Slri_ValNodeAddInt (ValNodePtr PNTR head, Int2 choice, Int4 value);
extern ValNodePtr Slri_ValNodeAddStr (ValNodePtr PNTR head, Int2 choice, CharPtr str);
extern ValNodePtr Slri_ValNodeAddPointer (ValNodePtr PNTR head, Int2 choice, Pointer value);
extern ValNodePtr Slri_ValNodeCopyStr (ValNodePtr PNTR head, Int2 choice, CharPtr str);

/************************/
/*list function handling*/
/************************/
 
/* add a new valnode to the end of a valnode headed by head, the new node has specific choice and a pointer value,
   value can be NULL*/
extern ValNodePtr Slri_ValNodeAddPointerAtEnd (ValNodePtr PNTR head, Int2 choice, Pointer value);

/*print a keyed list where its consecutive valnode store query (the valnose choice == 0, stored in data.intvalue) and
  return (the valnode choice ==1, store a valnodelist in data.ptrvalue*/ 
extern void Slri_PrintKeyedList(ValNodePtr retlist);

/*free a keyed list where its consecutive valnode store query (the valnose choice == 0, stored in data.intvalue) and
  return (the valnode choice ==1, store a valnodelist in data.ptrvalue*/
extern ValNodePtr Slri_FreeKeyedList(ValNodePtr retList);

/*print a keyedlist where its consecutive valnode stored query (the valnode choice == 0, stored in data.intvalue ) and 
 return (the valnode choice == 1, stored as fasta structure in data.ptrvalue)*/
extern void Slri_PrintFastaKeyedList(ValNodePtr retlist);

/*free a fasta keyedlist where its consecutive valnode stored query (the valnode choice == 0, stored in data.intvalue ) and 
 return (the valnode choice == 1, stored as fasta structure in data.ptrvalue)*/
extern ValNodePtr Slri_FreeFastaKeyedList(ValNodePtr retList);

/*converts a sep deliminted retstr string into an array of string, arraylen is the defined length of the array, count is
 *the actual 'Not NULL' entries in array*/ 
extern void Misc_Str2StrArray(CharPtr retstr, CharPtr *array, Int4 arraylen, Int4 *count, CharPtr sep);

/* converts a string array into a ValNodeList, this ValNodeList stores the qurey-return pair. query is int, return is a
 * list of comma delimited int */
extern ValNodePtr Misc_StrArray2ValNodeList(CharPtr *array, Int4 *count);



/*randomize a valnode list*/
extern Boolean RandomizeValNodeList(ValNodePtr PNTR head, Boolean *start);

/**********/
/* Memory */
/**********/

/* SK: Added NULLpointer check to avoid coredumps on Solaris */
#define IsNullStr(x) ((x)!=NULL?(x):"")

extern void Mem_Check(Pointer ptr);

/*************/
/* ByteStore */
/*************/

/* deletes size bytes from front of bsp, and resets position to front of bsp */
extern void  BSRemoveLeadingChar(ByteStorePtr bsp, long size);

/*printf to a Bytestore - return number of bytes printed*/
extern Int4 BSprintf(ByteStorePtr bsp, CharPtr format, ...);

/*saves a string stored in a ByteStore to a CharPtr*/
extern CharPtr BSStringSave(ByteStorePtr bsp);

/*Read a line (until a newline is seen) from a file
newline is not included in destination*/
extern Int2 BSGetLineFromFile(ByteStorePtr bspDest, FILE *fp);

/*print a bytestore to a stream*/
extern Boolean PrintBS(ByteStorePtr bsp, FILE *fp);

/*Errlog bytestore - useful for CGIs that must write to a log file in a single
write call so that multiple concurrent CGIs don't write interleaved logs lines*/

/*like ErrSetLogfile, but set up errlog bytestore - returns a reference to the ByteStore log for user use
or NULL if log file can't be opened*/
extern Boolean BSErrLogOpen(const char *filename, unsigned long flags);

/*close the errlog bytestore - and write out to the log file*/
extern ByteStorePtr BSErrLogClose(void);

/*get a reference to the bytestore errlog for user*/
extern ByteStorePtr BSErrLogGet(void);

/*printf to the ByteStore log - return number of bytes printed*/
extern Int4 BSErrLogPrintf(const char *format, ...);

/*print bytestore error log to a predefined NCBI corelib errorlog
NOTE: this should not have to be used, unless you want to bypass the functionality of BSError logging*/
extern Boolean BSErrLogFlush(void);

/**************/
/*File Parsing*/
/**************/

/*This function is like fgets, but it dynamically allocates the correct string size for the line of the file*/
/*NOTE: allocates a new string*/
extern CharPtr Misc_GetLineFromFile(FILE *fp);

/*A version of strtok that's thread-safe.
pos is the starting position in the string and it gets updated by the function
Returns newly allocated memory.*/
extern CharPtr Misc_StrTok(CharPtr input, CharPtr delim, Uint4Ptr pos);

/*Function to generically parse a delimited columnar file*/
/*Format defines data types of successive columns, support s=string,i=Int4,f=FloatHi
(e.g. "sif" means the file contains 3 columns, with the first column being strings, etc.
delim = the character delimiters between columns, behaves like strtok
Columns are returned in a linked list of linked lists (linked list of columns)*/
/*NOTE: This function can be called multiple times on the same file to parse different parts
of the file that have different columnar formats*/
/*Returns -1 for error, vnpColumns==NULL if not able to parse*/
extern Int2 Misc_GenericColumnParse(FILE *fp, CharPtr format, CharPtr delim, ValNodePtr *vnpColumns);

/* Specify the number of lines that you want to parse from a file */
extern Int2 Misc_GenericColumnParseWithNumLines(FILE *fp, CharPtr format, CharPtr delim, ValNodePtr *vnpColumns, Int4 NumLinesToParse);

/*Like Misc_GenericColumnParse, but takes a filename for convenience.
Don't use this function if you want to parse parts of the same file that have different formats*/
extern Int2 Misc_GenericColumnParseFileName(CharPtr fileName, CharPtr format, CharPtr delim, ValNodePtr *vnpColumns);

/*write out columns parsed in by Misc_GenericColumnParse.  Each column will be separated by the delim string*/
extern Int2 Misc_GenericColumnWrite(FILE *fp, ValNodePtr vnpColumns, CharPtr delim);

/*write out columns parsed in by Misc_GenericColumnParse, given a filename.
Each column will be separated by the delim string*/
extern Int2 Misc_GenericColumnWriteFileName(CharPtr fileName, ValNodePtr vnpColumns, CharPtr delim);

/*Advance a vnpColumns data structure by given number of rows*/
/*returns 1 for error, -1 for EOF*/
extern Int4 Misc_GenericColumnAdvanceRow(ValNodePtr vnpColumns, Int4 numRows2Advance);

/*Set the vnpColumns data structure back to a specific row number*/
/*NOTE: goes through entire list starting from the head - can be slow for large lists*/
extern Int4 Misc_GenericColumnReset2Row(ValNodePtr vnpColumns, Int4 rowNum);

/*gets the number of rows (lines) read in from vnpColumns data structure*/
extern Int4 Misc_GenericColumnGetNumberRows(ValNodePtr vnpColumns);

/*Get the data value from a vnpColumns data structure by column number of the current row*/
/*NOTE: column numbers start at 0*/
extern ValNodePtr Misc_GenericColumnGetColumn(ValNodePtr vnpColumns, Int4 colNum);

/*Get a pointer to the head of a column vnpColumns data structure by column number*/
/*NOTE: column numbers start at 0*/
/*returns NULL for error*/
extern ValNodePtr Misc_GenericColumnGetColumnHead(ValNodePtr vnpColumns, Int4 colNum);

/*Frees a vnpColumns data structure*/
extern ValNodePtr Misc_VnpColumnsFree(ValNodePtr vnpColumns);


/*************************************************************************/
/* FASTA FILE PARSING                                                    */
/*************************************************************************/
/* Misc_GetFastaFromFile       Get a single fasta sequence from the file */
/* Misc_FastaToSLRIFasta       Get all the FASTA sequences               */
/* Misc_ValNodeFastaListFree   Free the ValNodePtr linked list           */
/*************************************************************************/
extern SLRI_ERR Misc_GetFastaFromFile(FILE *fp, SLRIFastaPtr PNTR sf);
extern SLRI_ERR Misc_FastaToSLRIFasta(FILE *fp, ValNodePtr PNTR vnppSLRIFasta);
extern void Misc_ValNodeFastaListFree(ValNodePtr PNTR vnppSLRIFasta);




/*******************************************/
/* NCBI ASN.1 object constructor functions */
/*******************************************/

/*RULE: if a string is passed to a constructor function, a copy of it is integrated into the object*/

/*construct a new ObjectIdPtr with a string choice value*/
extern ObjectIdPtr Misc_C_ObjectIdStringNew(CharPtr string);

/*construct a new ObjectIdPtr with an integer choice value*/
extern ObjectIdPtr Misc_C_ObjectIdIntNew(Int4 integer);

/*construct a new DbtagPtr with a string database key*/
extern DbtagPtr Misc_C_DbtagStringNew(CharPtr db, CharPtr string);

/*construct a new DbtagPtr with an integer database key*/
extern DbtagPtr Misc_C_DbtagIntNew(CharPtr db, Int4 integer);

/* checksum functions - add a checksum to end of file/bytestore, and
verify it later - works great on text files, may not work as well on
binary ones */
extern void BSComputeFileChecksum(ByteStorePtr bsp);
extern Boolean BSStripFileChecksum(ByteStorePtr bsp);
extern Boolean BSCheckFileChecksum(ByteStorePtr bsp);
extern void ComputeFileChecksum(CharPtr fnam);
extern Boolean StripFileChecksum(CharPtr fnam);
extern Boolean CheckFileChecksum(CharPtr fnam);
/* functions to byte-swap variables - use at your own peril */
extern Uint4 FlipUI4(Uint4* me);
extern Int4 FlipI4(Int4 *me);
extern FloatLo FlipF(FloatLo *me);

/* Bytestore equivalent of fgets */
extern CharPtr BSGets(CharPtr dest,Int2 len,ByteStorePtr bsp);

/* Time/Date functions */
/* converts a date into a string of format YYYYMMDD, if date is
   NULL, YYYYMMDD is today's date. date must have the year, month
   and day set properly. Caller must free the string.
*/
extern CharPtr LIBCALL GetStringDate(NCBI_DatePtr date);


/** Get application's lifetime in seconds.
 *  @param pftime is a pointer to a FloatLo.
 *  @return SLRI_SUCCESS on success else SLRI_FAIL.
 */
extern SLRI_ERR LIBCALL GetAppTime(FloatLo *pftime);



/** Read a file into a charptr and get pointers to the file buffer
 * @param pcFileName The path+filename to read into a buffer
 * @retrun An initialized pointer to the start of the file buffer or NULL if error
 */
extern CharPtr LIBCALL MyGetFile(CharPtr pcFileName);

/** Get the next line from the buffer
 * @param filebuf The file buffer
 * @param cur A pointer to the next line
 * @param dest Will free, allocate and copy the next line if pointer is not null
 * @return The next line or NULL if error/none
 */
extern CharPtr LIBCALL MyGetLine(CharPtr filebuf, CharPtr *cur, CharPtr *dest);


/** Substitute a substring of a string with a new substring
 */
extern void LIBCALL SubstituteSubString(CharPtr istr, CharPtr search, CharPtr sub, CharPtr ostr);

/** Function will break up an input string into substrings based on the input separator string. Substrings are stored into an array, which is returned to the user. Note
that the input string is not altered by the process. The final element in all output arrays contains the string "End".
*/
CharPtr* StringBreaker(CharPtr string, CharPtr sep);

/** Function frees the substring array returned by the StringBreaker function 
*/
void FreeStrArr(CharPtr *array);

/** Function returns the number of nodes in a StringBreaker returned array 
*/
Int4 GetNumStrArrUnits(CharPtr *array);

#ifdef __cplusplus
}
#endif
#endif		/* _SLRI_MISC_ */

/*
$Log: slri_misc.h,v $
Revision 1.13  2005/03/14 19:28:55  ksnyder
Remove prototype for function FreeBreakerArr; added prototypes for functions FreeStrArr and GetNumStrArrUnits

Revision 1.12  2005/03/02 20:24:28  ryao
changed query and return pair symbol from '~' to '#~#', since some gis might contain '~' in defline, example gi 9971716. bug [4100], this change also need to be updated on all the remote APIs

Revision 1.11  2005/01/06 22:33:42  ksnyder
Added StringBreaker and FreeBreakerArr functions; StringBreaker breaks up an input string into substrings based on the input separator string.  The substrings are stored in an array, which is returned to the user. The FreeBreakerArr function frees the array returned by StringBreaker

Revision 1.10  2004/11/02 16:30:30  ryao
changed the GROUP separator from ';' to '#;#'. bug 3787 related.

Revision 1.9  2004/07/08 20:01:32  ryao
added Slri_FreeKeyedList Slri_FreeFastaKeyedList; changed prototype of Slri_PrintValNodeList to Slri_PrintKeyedList, Slri_PrintFastaValNodeList to Slri_PrintFastaKeyedList

Revision 1.8  2004/06/16 20:41:47  zwang
Added function SubstituteSubString for substituting a substring with a new one.

Revision 1.7  2004/04/05 16:38:46  ryao
Added helper functions for processing list API functions. New functions: Slri_ValNodeAddPointerAtEnd Slri_PrintValNodeList Slri_PrintFastaValNodeList Misc_Str2StrArray Misc_StrArray2ValNodeList

Revision 1.6  2004/02/19 23:47:07  mjdumont
Changed function name due to conflict

Revision 1.5  2004/02/02 21:28:42  mjdumont
Changed parameter name

Revision 1.4  2004/02/02 20:54:53  mjdumont
added GetAppTime and MGetLine

Revision 1.3  2003/11/14 21:21:43  feldman
Added BSGets

Revision 1.2  2003/10/16 20:59:50  skuznets

1. Checking for NULL pointers when print values
2. Added function Misc_TrimSpacesAroundString
3. Added macro IsNullStr

Revision 1.1.1.23  2003/06/12 08:38:20  root
Automated import from Sourceforge.net

Revision 1.25  2003/06/11 18:05:25  mdumonti
added a function for unique DValNode lists

Revision 1.24  2003/03/19 21:47:41  micheld
minor changes

Revision 1.23  2002/10/20 17:15:42  micheld
Added VNP<->FloatLoArray and stat functions

Revision 1.22  2002/08/19 18:30:45  micheld
Added LIBCALL to function for compliance to declared function

Revision 1.21  2002/08/15 20:25:13  haocl
reverted to version 1.19 and added GetStringDate

Revision 1.19  2002/07/09 14:12:00  gbader
Added flush function to ByteStore logging.

Revision 1.18  2002/04/11 20:26:21  gbader
Added another add to unique list function.

Revision 1.17  2002/04/03 19:33:46  mdumonti
added a valnode filtering function

Revision 1.16  2002/03/30 22:23:50  micheld
Added another SLRI_ERR

Revision 1.15  2002/03/25 06:44:22  micheld
Added comprehensive suite of linksets (only clinkset, flinkset and ilinkset tested)

Revision 1.14  2002/03/11 23:00:29  gbader
Added unique functions that maintain order - slightly slower in worst case from existing unique functions that don't maintain order.

Revision 1.13  2002/03/06 00:47:06  gbader
Added comment

Revision 1.12  2002/02/11 19:24:09  micheld
Moved asn out of seqhound and into library (this file should generate all future slrilib asn structs)
Added asn auto-generator script files
Added slri.h to be all-inclusive include file (does not include database specific files)

Revision 1.11  2002/02/04 16:42:55  micheld
Added floathi array <=> valnodelist

Revision 1.10  2002/01/18 20:49:58  micheld
Added StrToInt8 function

Revision 1.9  2002/01/16 03:31:40  gbader
Added another generic get column utility function.

Revision 1.8  2002/01/15 16:48:21  betel
Changed 'this' to 'me' in byte-swap functions

Revision 1.7  2002/01/14 21:23:08  micheld
Added valnode list partition function to split up long valnode lists into a list of lists
of size given by function caller

Revision 1.6  2001/12/12 16:51:38  gbader
Added count row function to generic column parse module

Revision 1.5  2001/12/11 04:51:42  gbader
Misc_GenericColumnParse - advance row function now properly signals end of file.

Revision 1.4  2001/12/09 19:08:47  gbader
Added convenience functions to generic column parser

Revision 1.3  2001/12/04 22:56:23  micheld
added int4 array to valnodelist converters

Revision 1.2  2001/11/29 04:26:49  gbader
Added find in list function

Revision 1.1.1.3  2001/10/10 22:11:04  kaca
initial import

Revision 1.32  2001/09/07 01:18:24  gbader
Added binary word search function.

Revision 1.31  2001/09/03 20:20:51  gbader
Added Misc_AddInt2UniqueList function

Revision 1.30  2001/08/30 18:34:29  gbader
Made some comments more clear

Revision 1.29  2001/08/28 17:59:27  gbader
Added a free function for file parsing functions.  Tested and run through purify.

Revision 1.28  2001/08/28 16:32:00  gbader
Added File parsing section and functions

Revision 1.27  2001/08/21 15:46:43  gbader
Added Misc_SaveFloatHiValNodeListAsString and generalized vnp->string functions.

Revision 1.26  2001/08/09 16:57:49  gbader
Added option to free data in Misc_StringVnpUniqueEx and added Misc_StringVnpUniqueAndFree function

Revision 1.25  2001/08/08 18:18:01  michel
Moved NBlast Hash table functions into math library

Revision 1.24  2001/07/24 15:38:44  haocl
added method BSRemoveLeadingChar

Revision 1.23  2001/06/26 20:10:58  gbader
Updated comments

Revision 1.22  2001/06/05 22:23:12  gbader
Added randomize linked list function (Useful for random models)

Revision 1.21  2001/05/23 21:53:00  gbader
Fixed Misc_GenericGetNumberofNodes to return correct number of nodes and added Misc_GenericRemoveObjectFromList

Revision 1.20  2001/04/13 02:34:28  gbader
Added CopyValNodeStringlist function

Revision 1.19  2001/04/11 14:49:47  feldman
Rewrote checksum functions again, now they are easily removed with
handy Strip functions, and shoudl work fine even on binary files

Revision 1.18  2001/04/06 14:48:17  feldman
Added byteflipping functions

Revision 1.17  2001/04/06 14:27:24  feldman
Added checksum functions for file verification

Revision 1.16  2001/04/03 14:05:33  michel
Removed ReadDB function - required ncbitool.lib -> moved to nblast_cb

Revision 1.15  2001/04/02 19:54:28  michel
added generic object list counter

Revision 1.14  2001/04/02 18:48:01  michel
Moved math functions to slri_math[c.h]
Added Misc_ReadDBGetGiFromOrd
Added Modified LinkSet (SLRILinkset) for use with FloatHis, and nested Linksetlists
Modified make files to include these new files

Revision 1.13  2001/03/30 21:27:55  michel
Added Int8toStr, SLRI_SafeLog10 and SLRI_SafeExp10 for general use
Added support for using generalized SLRI_ERR return type and values

Revision 1.12  2001/03/28 21:45:51  gbader
Moved externs to proper place before function return type

Revision 1.11  2001/03/28 01:20:15  gbader
Added Misc_IsStrInStrVnpList function

Revision 1.10  2001/03/26 19:27:20  gbader
Added new ordered list functions

Revision 1.9  2001/03/15 21:09:46  gbader
Moved extern "C" statements to proper place in header files (after includes)

Revision 1.8  2001/03/08 00:09:55  gbader
Moved extraneous header files inside the C files to make reduce header dependencies and make compiling much more efficient.

Revision 1.7  2001/01/13 04:04:10  gbader
Added IBM DB2 UDB to license exceptions.

Revision 1.6  2000/12/23 21:58:14  gbader
added numerical IP address validation function

Revision 1.5  2000/09/05 13:55:08  gbader
Updated License so that it is now GNU.
Added CVS keywords ID and Log as well as new line at end of files so compilers on some platforms don't complain

*/

