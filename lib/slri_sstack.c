/* $Id: slri_sstack.c,v 1.1.1.1 2001/10/12 08:21:54 root Exp $
***************************************************************************

  Samuel Lunenfeld Research Institute (SLRI)

  File: slri_sstack.c
  Description: String stack library

  Author: Anthony Hrvojic (hrvojic@ieee.org)
  Date: June 2001

  Hogue Lab - University of Toronto Biochemistry Department
  Samuel Lunenfeld Research Institute, Mount Sinai Hospital

  Copyright Notice:

  Copyright (C) 2001 Mount Sinai Hospital (MSH)

  This program is free software; you can redistribute it and/or 
  modify it under the terms of the GNU General Public License as 
  published by the Free Software Foundation; either version 2 of 
  the License, or any later version.

  This program is distributed in the hope that it will be useful, 
  but WITHOUT ANY WARRANTY; without even the implied warranty of 
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 

  See the GNU General Public License for more details.

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

***************************************************************************
*/

#include <slri_sstack.h>


/* Creates a new StringStack */
SLRI_SStackPtr
SLRI_SSNew(void) 
{
  SLRI_SStackPtr newStack = MemNew(sizeof(SLRI_SStack));

  newStack->totalStackSize = 0;
  newStack->totalNumItems = 0;
  newStack->top = NULL;
  newStack->bottom = NULL;

  return(newStack);
}


/* Frees a StringStack and all items contained in it */
SLRI_SStackPtr 
SLRI_SSFree(SLRI_SStackPtr ssp)
{
  SLRI_SStackItemPtr ssipCurr = NULL;
  SLRI_SStackItemPtr ssipTemp = NULL;

  if (ssp == NULL) {
    ErrPostEx(SEV_ERROR, 0, 0, "SLRI_SSFree: stack pointer is NULL");
    return(NULL);
  }

  ssipCurr = ssp->top;
  while (ssipCurr != NULL) {
    ssipTemp = ssipCurr->prev;
    ssipCurr = MemFree(ssipCurr);
    ssipCurr = ssipTemp;
  }

  /* Free stack */
  MemFree(ssp);

  return(NULL);
}

/* Clears all items in StringStack */
Int4 
SLRI_SSClear(SLRI_SStackPtr ssp)
{
  SLRI_SStackItemPtr ssipCurr = NULL;
  SLRI_SStackItemPtr ssipTemp = NULL;

  if (ssp == NULL) {
    ErrPostEx(SEV_ERROR, 0, 0, "SLRI_SSClear: stack pointer is NULL");
    return(-1);
  }

  ssipCurr = ssp->top;
  while (ssipCurr != NULL) {
    ssipTemp = ssipCurr->prev;
    ssipCurr = MemFree(ssipCurr);
    ssipCurr = ssipTemp;
  }

  /* Re-initialize pointers */
  ssp->totalStackSize = 0;
  ssp->totalNumItems = 0;
  ssp->top = NULL;
  ssp->bottom = NULL;

  return(0);
}


/* Pushes a StringStack item onto StringStack */
Int4 
SLRI_SSPush(SLRI_SStackPtr ssp, CharPtr string) 
{
  SLRI_SStackItemPtr newItem = NULL;

  if (ssp == NULL) {
    ErrPostEx(SEV_ERROR, 0, 0, "SLRI_SSPush: stack pointer is NULL");
    return(-1);
  }

  newItem = MemNew(sizeof(SLRI_SStackItem));

  newItem->string = string;
  newItem->prev = ssp->top;
  newItem->next = NULL; 

  if (ssp->top != NULL) {
    ssp->top->next = newItem;
  }

  ssp->top = newItem;
  ssp->totalStackSize += StringLen(newItem->string);
  ssp->totalNumItems++;

  /* If first item, set bottom pointer to top */
  if (ssp->bottom == NULL) {
    ssp->bottom = ssp->top;
  }

  return(0);
}


/* Pops top StringStack item from StringStack */
CharPtr 
SLRI_SSPop(SLRI_SStackPtr ssp) 
{
  CharPtr string = NULL;
  SLRI_SStackItemPtr ssipNewTop = NULL;

  if (ssp == NULL) {
    ErrPostEx(SEV_ERROR, 0, 0, "SLRI_SSPop: stack pointer is NULL");
    return(NULL);
  }

  if (SLRI_SSIsEmpty(ssp)) {
    ErrPostEx(SEV_ERROR, 0, 0, "SLRI_SSPop: stack is empty");
    return(NULL);
  }

  ssipNewTop = ssp->top->prev;
  string = StringSave(ssp->top->string);
  ssp->top = MemFree(ssp->top);
  ssp->top = ssipNewTop;
  ssp->totalStackSize -= StringLen(string);
  ssp->totalNumItems--;

  /* If last item on stack, set bottom pointer to top */
  if (ssp->top == NULL) {
    ssp->bottom = ssp->top;
  }

  return(string);
}


/* Returns string from top StringStack item */
CharPtr 
SLRI_SSPeek(SLRI_SStackPtr ssp)
{
  if (ssp == NULL) {
    ErrPostEx(SEV_ERROR, 0, 0, "SLRI_SSPeek: stack pointer is NULL");
    return(NULL);
  }

  if (SLRI_SSIsEmpty(ssp)) {
    return(NULL);
  }
  else {
    return(ssp->top->string);
  }
}


/* Checks if StringStack empty */
Boolean 
SLRI_SSIsEmpty(SLRI_SStackPtr ssp)
{
  if (ssp == NULL) {
    ErrPostEx(SEV_ERROR, 0, 0, "SLRI_SSIsEmpty: stack pointer is NULL");
    return(TRUE);
  }

  if (ssp->bottom == NULL) {
    /* Stack is empty */
    return(TRUE);
  }
  else {
    return(FALSE);
  }
}

/* Returns total size of all strings in stack */
Int4 
SLRI_SSGetTotalStackSize(SLRI_SStackPtr ssp)
{
  return(ssp->totalStackSize);
}


/* Returns total number of elements in stack */
Int4 
SLRI_SSGetTotalNumItems(SLRI_SStackPtr ssp)
{
  return(ssp->totalNumItems);
}


/* Returns a concatenation of all stack contents */
CharPtr
SLRI_SSGetWholeString(SLRI_SStackPtr ssp)
{
  SLRI_SStackItemPtr ssipCurr = NULL;
  CharPtr wholeString = NULL;

  if (ssp == NULL) {
    ErrPostEx(SEV_ERROR, 0, 0, "SLRI_SSGetWholeString: stack pointer is NULL");
    return(NULL);
  }

  if (SLRI_SSIsEmpty(ssp)) {
    return(NULL);
  }

  wholeString = MemNew((ssp->totalStackSize + 1) * sizeof(Char));

  /* Initialize wholeString with first item */
  StringCpy(wholeString, ssp->bottom->string);

  /* Cycle through rest of stack to get remaining items */
  ssipCurr = ssp->bottom->next;
  while (ssipCurr != NULL) {
    StringCat(wholeString, ssipCurr->string);
    ssipCurr = ssipCurr->next;
  }

  return(wholeString);
}


/*

$Log: slri_sstack.c,v $
Revision 1.1.1.1  2001/10/12 08:21:54  root
Automated import from Sourceforge.net

Revision 1.1.1.1  2001/10/10 22:11:04  kaca
initial import

Revision 1.3  2001/08/01 18:11:58  hrvojic
Strings in stack no longer need to be freed; removed MemFree() calls

Revision 1.2  2001/07/31 13:56:17  hrvojic
Removed StringSave call in push function

Revision 1.1  2001/06/26 20:59:30  hrvojic
Initial import of slri_sstack library


*/








