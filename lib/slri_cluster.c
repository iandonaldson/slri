/* $Id: slri_cluster.c,v 1.10 2004/03/31 16:41:54 mjdumont Exp $
-- ***************************************************************************
--
--  Samuel Lunenfeld Research Institute (SLRI)
--
--  File: slri_cluster.c
--  Description:  Set of Data Clustering Functions
--
--
--  Original concept and detail coding obtained From P. Kleiweg ©1997-2002
--  http://odur.let.rug.nl/~kleiweg/indexs.html (all code released as GNU GPL)
--  
--  Functions were changed to be modular and suitable for library use 
--    Changed Data structures, modified dendrogram output
--
--  Author: Michel Dumontier
--  Date: July 2002
--
--  Hogue Lab - University of Toronto Biochemistry Department
--  Samuel Lunenfeld Research Institute, Mount Sinai Hospital
--
--  Copyright Notice:
--
--  Copyright ©2002 Mount Sinai Hospital (MSH)
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
#include "slri_cluster.h"


/** Parses the following file format
 * Indicate comments with #.  Blank lines ignored.
 * # Vector size 3 (integer)
 * 3
 * # Column Label
 * A
 * B
 * C
 * # Row Label (alpha-numeric)
 * One
 * # Row Values (floating point)
 * 0.05234
 * 0.04234
 * 0.22455
 * 
 * # Row Label
 * Two
 * # Row Values
 * 0.23423
 * 0.22113
 * 1.24235
 * 
 * ...
 *
 * @param pcFileName The file (and path) to parse
 * @param pf The Matrix of values
 * @param pcColLabels The array of column labels
 * @param pcRowLabels The array of row labels
 * @param iRowNum The number of rows
 * @param iColNum The number of columns
 * @return SLRI_SUCCESS on success, SLRI_FAIL on failure
 */
SLRI_ERR CLUSTER_ReadFile(CharPtr pcFileName, FloatLoPtr **pppf, CharPtr **pppcColLabels, CharPtr **pppcRowLabels, Int4 *piVecNum, Int4 *piVecSize)
{
	CharPtr function = "CLUSTER_ReadFile";
	Boolean bFirst = TRUE;
	Char buffer[PATH_MAX];
	CharPtr pcLabel = NULL, *ppcLabels = NULL, *ppcVecLabels = NULL;
	FILE *fp = NULL;
	float f = 0.0;
	FloatLoPtr pf = NULL;
	FloatLoPtr *ppf = NULL;
	Int4 i = 0, iVecNum = 0, iVecSize = 0, iArraySize = 0, iLabel = 0;
	long lVecSize = 0;
	ValNodePtr vnp = NULL, vnpHead = NULL, vnpLabel = NULL, vnpLabelHead = NULL, vnpVecLabel = NULL, vnpVecLabelHead = NULL;

	if(pcFileName == NULL || pppf == NULL || piVecNum == NULL || piVecSize == NULL) {
		ErrPostEx(SEV_ERROR,0,0,"%s:Invalid Parameters", function);
		return SLRI_FAIL;
	}

	if((fp = FileOpen(pcFileName,"r")) == NULL) {
		ErrPostEx(SEV_ERROR,0,0,"%s:Unable to open file: %s", function ,IsNullStr(pcFileName));
		return SLRI_FAIL;
	}

	while(fgets(buffer, PATH_MAX, fp)) {
		if(buffer[0] == '#' || buffer[0] == ' ' || buffer[0] == '\n') {
			continue;
		}
		if(bFirst) {
			/* get the vector size */
			if((sscanf(buffer, "%ld", &lVecSize)) != 1) {
				ErrPostEx(SEV_ERROR,0,0,"%s:First number in file must be vector size.", function);
				return SLRI_FAIL;
			}
			iVecSize = lVecSize;
			bFirst = FALSE;
			continue;
		}
		/* get the label */
		pcLabel = (CharPtr) MemNew((size_t)sizeof(Char)*StringLen(buffer));
		StringNCpy(pcLabel,buffer,StringLen(buffer)-1);
		
		if(iLabel < iVecSize) {
			/* Get the vector labels */
			vnpVecLabel = ValNodeAddPointer(&vnpVecLabel,0,(Pointer) pcLabel);
			if(vnpVecLabelHead == NULL) vnpVecLabelHead = vnpVecLabel;
			iLabel++;
			continue;
		} else {
			/* get the row labels */
			vnpLabel = ValNodeAddPointer(&vnpLabel,0,(Pointer) pcLabel);
			if(vnpLabelHead == NULL) vnpLabelHead = vnpLabel;
		}

		if((pf = (FloatLoPtr) MemNew((size_t)sizeof(FloatLo) * iVecSize)) == NULL) {
			return SLRI_NOMEM;
		}
		i = 0;
		/* get the row values */
		while(fgets(buffer, PATH_MAX, fp) != NULL) {
			if((sscanf(buffer,"%f",&f)) != 1) {
				ErrPostEx(SEV_ERROR,0,0,"%s:Only one floating point value per line!", function);
				return SLRI_FAIL;
			}
			pf[i] = (FloatLo) f;
			i++;
			if(i == iVecSize) break;
		}
		vnp = ValNodeAddPointer(&vnp,0,(Pointer)pf);
		if(vnpHead == NULL) vnpHead = vnp;

		iVecNum++;
	}

	/* Now put the pointers back into array */
	vnp = vnpHead;
	if((ppf = (FloatLoPtr *) MemNew((size_t)sizeof(FloatLoPtr) * iVecNum)) == NULL) return SLRI_NOMEM;
	i = 0;
	while(vnp != NULL && i < iVecNum) {
		ppf[i] = (FloatLoPtr) vnp->data.ptrvalue;
		vnp = vnp->next;
		i++;
	}
	vnpHead = vnp = ValNodeFree(vnpHead);

	Misc_ValNodeListToCharPtrArray(vnpVecLabelHead, &ppcVecLabels, &iArraySize);
	vnpVecLabelHead = ValNodeFreeData(vnpVecLabelHead);
	Misc_ValNodeListToCharPtrArray(vnpLabelHead, &ppcLabels, &iArraySize);
	vnpLabelHead = vnpLabel = ValNodeFreeData(vnpLabelHead);

	*pppf = ppf;
	if(pppcColLabels) *pppcColLabels = ppcVecLabels;
	else MemFree(ppcVecLabels);
	if(pppcRowLabels) *pppcRowLabels = ppcLabels;
	else MemFree(ppcLabels);
	*piVecNum = iVecNum;
	*piVecSize = iVecSize;
	return SLRI_SUCCESS;
}


/** Parses the following file format
 * 
 * # comment are ignored up until the newline\n
 * [IGNORED_TEXT]\tcol_name_1\tcol_name_2\t...\col_name_X\n (make sure there isn't an extra tab before the newline)
 * row_name_1\tcell_value_1\tcell_value_2\t...\tcell_value_X\n
 * row_name_2\tcell_value_1\tcell_value_2\t...\tcell_value_X\n
 *
 * @param pcFileName The file (and path) to parse
 * @param pf The Matrix of values
 * @param pcColLabels The array of column labels
 * @param pcRowLabels The array of row labels
 * @param iRowNum The number of rows
 * @param iColNum The number of columns
 * @return SLRI_SUCCESS on success, SLRI_FAIL on failure
 */
SLRI_ERR CLUSTER_ReadBlockFile(CharPtr pcFileName, FloatLoPtr **pf, CharPtr **pcColLabels, CharPtr **pcRowLabels, Int4 *iRowNum, Int4 *iColNum)
{
	CharPtr function = "CLUSTER_ReadBlockFile";
	Boolean eol = FALSE, cr = FALSE, bFirst = FALSE;
	CharPtr pc = NULL, p1 = NULL, p2 = NULL, cur = NULL, filebuf = NULL;
	Int4 i = 0;
	ValNodePtr pvn = NULL, pvnHead = NULL, pvnfll = NULL;
	float f = 0.0;
	FloatLoPtr fl = NULL;

	if((filebuf = MyGetFile(pcFileName)) == NULL) {
		ErrPostEx(SEV_ERROR,0,0,"Unable to open file %s", pcFileName);
		return SLRI_FAIL;
	}
	cur = filebuf;
	do {
		/** ignore comments and newlines */
		if(cur[0] != '#' && cur[0] != '\n') {
			if(bFirst == FALSE) {
				bFirst = TRUE;
				/** Get the column names into an CharPtr array */
				p1 = StringStr(filebuf, "\t"); /** go to first column name */
				while(++p1) {
					pc = p1;
					/** Find either the next tab or end of line */
					while(*p1 != '\t' && *p1 != '\n') *p1++;
					p2 = p1;
					/** Take care of the Windows special case of carriage return */
					if(*p1 == '\n') {
						eol = TRUE;
						if (*(p1-1) == '\r') {
							cr = TRUE;
							p2--;
						}
					}
					/** Make and copy the string to a valnode list */
					*p2 = '\0';
					ValNodeCopyStr(&pvnHead,0,pc);
					*p2 = '\t';

					/** Stop when reached the end of the line */
					if(eol == TRUE) {
						eol = FALSE;
						if(cr == TRUE) {
							cr = FALSE;
							*p2 = '\r';
							p2++;
						}
						*p2 = '\n';
						break;	
					}
				}
				pvn = pvnHead;
				*iColNum = ValNodeLen(pvnHead);
				*pcColLabels = (CharPtr*) MemNew((size_t)sizeof(CharPtr) * (*iColNum));
				for(i=0;i<(*iColNum);i++) {
					(*pcColLabels)[i] = (CharPtr) pvn->data.ptrvalue;
					pvn = pvn->next;
				}
				pvnHead = ValNodeFree(pvnHead);
			} else {
				/** Get the row, but only if the columns have been parsed */
				if((*iColNum) == 0) {
					ErrPostEx(SEV_ERROR,0,0,"%s:No column names found.  These must be specified", function);
					filebuf = MemFree(filebuf);
					return SLRI_FAIL;
				}
				i = 0;

				/** First get the row name */
				pc = p1 = cur;
				p1 = StringStr(p1, "\t");
				if(p1 == NULL) break;
				*p1 = '\0';
				ValNodeCopyStr(&pvnHead,0,pc);
				*p1 = '\t';
				/** Allocate memory for the row values */
				fl = (FloatLoPtr) MemNew((size_t)sizeof(FloatLo)*(*iColNum));
				while(++p1 && (*iColNum > i++)) {
					/** Now get the values */
					pc = p1;
					while(*p1 != '\t' && *p1 != '\n') *p1++;
					p2 = p1;
					if(*p1 == '\n') {
						eol = TRUE;
						if (*(p1-1) == '\r') {
							cr = TRUE;
							p2--;
						}
					}
					*p2 = '\0';
					if((sscanf(pc,"%f",&f)) != 1) {
						ErrPostEx(SEV_ERROR,0,0,"%s:Unable to parse %s into float value",function,pc);
					}
					*p2 = '\t';

					fl[i-1] = f;

					if(eol == TRUE) {
						eol = FALSE;
						if(cr == TRUE) {
							cr = FALSE;
							*p2 = '\r';
							p2++;
						}
						*p2 = '\n';
						break;
					}
				}

				/** Add the values to a list */
				ValNodeAddPointer((Pointer)&pvnfll,0,(Pointer)fl);
			}
		}
	} while(MyGetLine(filebuf,&cur,NULL) != NULL);
	filebuf = MemFree(filebuf);

	if(pvnHead) {
		/** Add the row names to the list */
		pvn = pvnHead;
		*iRowNum = ValNodeLen(pvnHead);
		*pcRowLabels = (CharPtr*) MemNew((size_t)sizeof(CharPtr) * (*iRowNum));
		for(i=0;i<(*iRowNum);i++) {
			(*pcRowLabels)[i] = (CharPtr) pvn->data.ptrvalue;
			pvn = pvn->next;
		}
		pvnHead = ValNodeFree(pvnHead);
	} else {
		ErrPostEx(SEV_ERROR,0,0,"%s:No row names found!");
		return SLRI_FAIL;
	}
	if(pvnfll) {
		pvn = pvnfll;
		*pf = (FloatLoPtr *) MemNew((size_t)sizeof(FloatLoPtr)*(*iRowNum));
		for(i=0;i<(*iRowNum);i++) {
			(*pf)[i] = (FloatLoPtr) pvn->data.ptrvalue;
			pvn = pvn->next;
		}
		pvnfll = ValNodeFree(pvnfll);
	} else {
		ErrPostEx(SEV_ERROR,0,0,"%s:No values found!");
		return SLRI_FAIL;
	}
	return SLRI_SUCCESS;
}


/** Frees memory from CLUSTER_ReadFile */
void CLUSTER_ReadFileFree(FloatLoPtr **pppfdata, CharPtr **pppcColLabels, CharPtr **pppcRowLabels, Int4 iVecNum, Int4 iVecSize)
{
	CharPtr *ppcColLabels = NULL, *ppcRowLabels = NULL;
	Int4 i = 0;
	FloatLoPtr *ppfdata = NULL;

	if(pppfdata && *pppfdata) {
		ppfdata = *pppfdata;
		for(i = 0; i < iVecNum; i++) {
			ppfdata[i] = MemFree(ppfdata[i]);
		}
		*pppfdata = MemFree(ppfdata);
	}

	if(pppcColLabels && *pppcColLabels) {
		ppcColLabels = *pppcColLabels;
		for(i = 0; i < iVecSize; i++) {
			ppcColLabels[i] = MemFree(ppcColLabels[i]);
		}
		*pppcColLabels = MemFree(ppcColLabels);
	}
	if(pppcRowLabels && *pppcRowLabels) {
		ppcRowLabels = *pppcRowLabels;
		for(i = 0; i < iVecNum; i++) {
			ppcRowLabels[i] = MemFree(ppcRowLabels[i]);
		}
		*pppcRowLabels = MemFree(ppcRowLabels);
	}
	return;
}


/** Calculates the Sum of the square difference between two vectors pf1 and pf2 of size iVecSize */
FloatLo CLUSTER_SquareDiff (FloatLoPtr pf1, FloatLoPtr pf2, Int4 iVecSize)
{
	Int4 i = 0;
	FloatLo f = 0.0, s = 0.0;

	for(i = 0; i < iVecSize; i++) {
		f = pf1[i] - pf2[i];
		s += f*f;
	}
	return s;
}

/** Calculates the Euclidean distance between two vectors pi1 and pi2 of size iVecSize */
FloatLo CLUSTER_Euclid (FloatLoPtr pf1, FloatLoPtr pf2, Int4 iVecSize)
{ 
	return (FloatLo) sqrt((double) CLUSTER_SquareDiff(pf1,pf2,iVecSize));
}

/** Pearson correlation coefficient.
 * It is 1/n(g1c1 + g2c2 + ... + gncn) 
 * defined between the class vector and the normalized logarithmic  vectors. 
 */
void CLUSTER_Pearson_First (FloatLoPtr *ppf, Int4 iNumVec, Int4 iVecSize)
{
    Int4 i = 0, j = 0;
    FloatLo sum = 0.0, m = 0.0, sd = 0.0;

    for (i = 0; i < iVecSize; i++) {
        sum = 0.0;
        for (j = 0; j < iNumVec; j++) sum += ppf[j][i];
        m = sum / (FloatLo) iNumVec;
        sum = 0.0;
        for (j = 0; j < iNumVec; j++) sum += (ppf[j][i] - m) * (ppf[j][i] - m);
        sd = (FloatLo) sqrt (sum / ((FloatLo)iNumVec - 1.0));

        for (j = 0; j < iNumVec; j++) ppf[j][i] = (FloatLo)((double) ppf[j][i] - m) / sd;
    }
}


/** Calculates the Pearson distance between two vectors pf1 and pf2 of size iVecSize */
FloatLo CLUSTER_Pearson (FloatLoPtr pf1, FloatLoPtr pf2, Int4 iVecSize)
{
	Int4 i = 0;
	FloatLo f = 0.0;

	for(i = 0; i < iVecSize; i++) {
		f += (FloatLo) (pf1[i] - pf2[i]);
	}
	return (FloatLo)(1.0 - (f / (FloatLo) iVecSize));
}

/** Calculates the Manhattan distance between two vectors pf1 and pf2 of size iVecSize */
FloatLo CLUSTER_Manhattan (FloatLoPtr pf1, FloatLoPtr pf2, Int4 iVecSize)
{
	Int4 i = 0;
	FloatLo f = 0.0;

	for(i = 0; i < iVecSize; i++) {
		f += (FloatLo) fabs((double) (pf1[i] - pf2[i]));
	}
	return f;
}

FloatLo CLUSTER_BrayCurtis (FloatLoPtr pf1, FloatLoPtr pf2, Int4 iVecSize)
{
	Int4 i = 0;
	FloatLo num = 0, f = 0.0;
	for(i = 0; i < iVecSize; i++) {
		

		if(pf1[i] >= pf2[i]) num += pf1[i] - pf2[i];
		else num += pf2[i] - pf1[i];
		f += pf1[i] + pf2[i];
	}
	return f ? num / f : 0;
}

FloatLo CLUSTER_Canberra (FloatLoPtr pf1, FloatLoPtr pf2, Int4 iVecSize)
{
	Int4 i = 0;
	FloatLo num = 0.0, f = 0.0;

	for(i = 0; i < iVecSize; i++) {
		if(pf1[i] > 0.0 || pf2[i] > 0.0) {
			if(pf1[i] >= pf2[i]) num += pf1[i] - pf2[i];
			else num += pf2[i] - pf1[i];
			f += num / (pf1[i] + pf2[i]);
		}
	}
	return f ? f / iVecSize : 0;
}


/** Similarity measures
 * The similarity measures for binary data are based on the four values from the
 * cross tabulation of the two observations.
 * 
 *                                       |  obs. j
 *                                        |  0    1 
 *                                 -------+----------
 *                                 obs. 0 |  d/3  c/2 
 *                                  i   1 |  b/1  a/0 
 * 
 * a is the number of variables where observations i and j both had ones, and d is
 * the number of variables where observations i and j both had zeros.  The number
 * of variables where observation i is one and observation j is zero is b, and the
 * number of variables where observation i is zero and observation j is one is c.
 * 
 */
void CLUSTER_FloatComp (FloatLoPtr pf1, FloatLoPtr pf2, Int4 iVecSize, Int4Ptr piCount)
{
	Int4 i = 0, index = 0;
	piCount[0] = piCount[1] = piCount[2] = piCount[3] = 0;
	for(i = 0; i < iVecSize; i++) {
		index = ((pf1[i] > 0.0) ? 0 : 2) + ((pf2[i] > 0.0) ? 0 : 1);
		++piCount[index];
	}
}

/**	Simple Matching binary similarity coefficient
 * (a+d)/(a+b+c+d)              
 * 	the proportion of matches between two observations.                         
 */
FloatLo CLUSTER_SimpleMatching(FloatLoPtr pf1, FloatLoPtr pf2, Int4 iVecSize)
{
	Int4 piCount[4], iTotal = 0;
	CLUSTER_FloatComp(pf1, pf2, iVecSize, piCount);
	iTotal = iVecSize;
	return iTotal ? ((FloatLo)(piCount[3] + piCount[0])) / (FloatLo) iTotal : 0;
}

/** Jaccard binary similarity coefficient
 *  a/(a+b+c)
 * 
 *     which is the proportion of matches when at least one of the observations
 *     had a one.  In the case where both observations are all zeros, this measure
 *     is 1 , meaning perfect agreement.  This is a reasonable choice for cluster analysis and
 *     will cause an all zero observation to have similarity of one only with
 *     another all zero observation.  In all other cases, an all zero observation
 *     will have Jaccard similarity of zero to the other observation.
 */
FloatLo CLUSTER_Jaccard (FloatLoPtr pf1, FloatLoPtr pf2, Int4 iVecSize)
{
	Int4 piCount[4], iTotal = 0;
	CLUSTER_FloatComp(pf1, pf2, iVecSize, piCount);
	iTotal = piCount[0] + piCount[1] + piCount[2];
	return iTotal ? ((FloatLo) piCount[0]) / (FloatLo) iTotal : 1;
}

/** Sorensen Similarity
 *  DCS = 2BC/(Bi+Bj) 
 * 	Dice, Czekanowski, or Sorensen or Nei & Lie's genetic distance
 */
FloatLo CLUSTER_Sorensen (FloatLoPtr pf1, FloatLoPtr pf2, Int4 iVecSize)
{
	Int4 piCount[4], iTotal = 0;
	CLUSTER_FloatComp(pf1, pf2, iVecSize, piCount);
	iTotal = 2 * piCount[0] + piCount[1] + piCount[2];
	return iTotal ? (FloatLo)piCount[0] / (FloatLo) iTotal : 0;
}



/** Baroni-Urbani and Buser similarity */
FloatLo CLUSTER_BaroniUrbani(FloatLoPtr pf1, FloatLoPtr pf2, Int4 iVecSize)
{
	Int4 piCount[4];
	FloatLo f = 0;
	CLUSTER_FloatComp(pf1, pf2, iVecSize, piCount);
	f = (FloatLo) sqrt((double) ((piCount[0] * piCount[3]) + piCount[0] + piCount[1] + piCount[2]));
	return f ? (FloatLo)(piCount[1] + piCount[2]) / f : 0;
}



/** Calculates the distance between two vectors pf1, pf2 of iVecSize size according to supplied metric */
FloatLo CLUSTER_CalculateDiffWithMetricEx(CLUSTER_METRIC metric, FloatLoPtr pf1, FloatLoPtr pf2, Int4 iVecSize)
{
	FloatLo f = 0.0;
	     if(metric == CLUSTER_METRIC_EUCLID)         f = CLUSTER_Euclid(pf1,pf2,iVecSize);
	else if(metric == CLUSTER_METRIC_SQUAREDIFF)     f = CLUSTER_SquareDiff(pf1,pf2,iVecSize);
	else if(metric == CLUSTER_METRIC_MANHATTAN)      f = CLUSTER_Manhattan(pf1,pf2,iVecSize);
	else if(metric == CLUSTER_METRIC_PEARSON)        f = CLUSTER_Pearson(pf1,pf2,iVecSize);
	else if(metric == CLUSTER_METRIC_SIMPLEMATCHING) f = CLUSTER_SimpleMatching(pf1,pf2,iVecSize);
	else if(metric == CLUSTER_METRIC_JACCARD)        f = CLUSTER_Jaccard(pf1,pf2,iVecSize);
	else if(metric == CLUSTER_METRIC_SORENSEN)       f = CLUSTER_Sorensen(pf1,pf2,iVecSize);
	else if(metric == CLUSTER_METRIC_BARONIURBANI)   f = CLUSTER_BaroniUrbani(pf1,pf2,iVecSize);
	return f;
}

/** The Metric Names */
CharPtr CLUSTER_MetricNames[] =
{
  "Square Difference",
  "Euclidean Distance",
  "Manhattan Distance",
  "Pearson Distance",
  "Bray-Curtis Distance",
  "Canberra Distance",
  "Simple Matching",
  "Jaccard Similarity",
  "Sorensen Similarity",
  "Baroni-Urbani Similarity"
};

/** Get a pointer to the Metric names */
CharPtr CLUSTER_GetMetricName(CLUSTER_METRIC metric) {
	return CLUSTER_MetricNames[(Int2)metric];
}

/** Calculates the difference between all vectors of iVecSize in FloatLo Array of size iVecNum 
 *  returns the values in a 2D Matrix 
 */
SLRI_ERR CLUSTER_Diff (CLUSTER_METRIC metric, FloatLoPtr *ppf, Int4 iVecNum, Int4 iVecSize, FloatLoPtr PNTR *ppfMatrix)
{
	Int4 i = 0, j = 0;
	FloatLoPtr *pfMatrix = NULL;


	if(ppf == NULL || iVecNum == 0 || iVecSize == 0 || ppfMatrix == NULL) {
		ErrPostEx(SEV_ERROR,0,0,"CLUSTER_CalculateDiffWithMetric: Invalid Parameters");
		return SLRI_FAIL;
	}
	if(*ppfMatrix == NULL) {
		*ppfMatrix = pfMatrix = (FloatLoPtr *) MemNew((size_t) sizeof(FloatLo)*(2*iVecNum-1));
		for(i = 0; i < (2*iVecNum-1); i++) {
			pfMatrix[i] = (FloatLoPtr) MemNew((size_t) sizeof(FloatLo)*(2*iVecNum-1));
		}
	}
	if(metric == CLUSTER_METRIC_PEARSON) CLUSTER_Pearson_First(ppf, iVecNum, iVecSize); /* normalize vectors */

	for(i = 1; i < iVecNum; i++) {
		for(j = 0; j < iVecNum; j++) {
			pfMatrix[i][j] = pfMatrix[j][i] = CLUSTER_CalculateDiffWithMetricEx(metric, ppf[i], ppf[j], iVecSize);
		}
	}

	return SLRI_SUCCESS;
}

/** Read in a matrix difference file */
SLRI_ERR CLUSTER_ReadDiff(CharPtr pcFileName, FloatLoPtr **pppfMatrix, CharPtr **pppcLabels, Int4 *piVecNum)
{
	Char temp[PATH_MAX];
	CharPtr pc = NULL, p = NULL, buffer = NULL;
	FILE *fp = NULL;
	Int4 i = 0, j = 0, iVecNum = 0;
	FloatLo f = 0;
	FloatLoPtr *ppfMatrix = NULL;
	CharPtr *ppcLabels = NULL;
	long num = 0;

	if((fp = FileOpen(pcFileName,"r")) == NULL) {
		ErrPostEx(SEV_ERROR,0,0,"Cannot open file: %s",IsNullStr(pcFileName));
		return SLRI_FAIL;
	}

	/** Determine the number of rows */
	while(fgets(temp, PATH_MAX, fp)) {
		if(temp[0] == '#' || temp[0] == ' ' || temp[0] == '\n') {
			continue;
		}
		if((sscanf(temp,"%ld",&num)) == 1) {
			iVecNum = num;
			break;
		}
	}
	if(iVecNum <= 0) {
		ErrPostEx(SEV_ERROR,0,0,"Could not get the number of matrix elements.  This must be the first line specified!");
		return SLRI_FAIL;
	}

	i = 0;
	ppcLabels = (CharPtr *) MemNew((size_t) sizeof(CharPtr) * (iVecNum) );
	ppfMatrix = (FloatLoPtr *) MemNew((size_t) sizeof(FloatLo)*(2*iVecNum));
	for(i = 0; i < (2*iVecNum); i++) {
		ppfMatrix[i] = (FloatLoPtr) MemNew((size_t) sizeof(FloatLo)*(2*iVecNum));
	}

	i = 0;
	buffer = (CharPtr) MemNew((size_t)sizeof(Char)*(1048576) );
	while(fgets(buffer, 1048576-1, fp) && (i < iVecNum) ) {
		if(buffer[0] == '#' || buffer[0] == ' ' || buffer[0] == '\n') {
			continue;
		}
		if((sscanf(buffer,"%s",temp)) == 1) {
			ppcLabels[i] = StringSave(temp);
		}
		j = 0;
		pc = buffer;
		while(((p = Nlm_StringChr(pc, '\t')) != NULL) && (j < iVecNum)) {
			p++;
			if(sscanf(p,"%f",&f)) {
				ppfMatrix[i][j] = f;
			} else {
				ErrPostEx(SEV_ERROR,0,0,"There are only %ld of %ld columns in this matrix");
				return SLRI_FAIL;
			}
			pc = p;
			j++;
		}
		i++;
	}
	MemFree(buffer);
	*piVecNum = iVecNum;
	*pppcLabels = ppcLabels;
	*pppfMatrix = ppfMatrix;

	return SLRI_SUCCESS;
}

/** Dump a matrix difference file */
SLRI_ERR CLUSTER_DumpDiff(CLUSTER_METRIC metric, FloatLoPtr *ppfMatrix, CharPtr *ppcLabels, Int4 iVecNum, CharPtr pcFileName)
{
	Int4 i = 0, j = 0;
	FILE *fp = NULL;

	if(ppfMatrix == NULL || iVecNum <= 0 || pcFileName == NULL) {
		ErrPostEx(SEV_ERROR,0,0,"Invalid Parameters");
		return SLRI_FAIL;
	}
	
	if((fp = FileOpen(pcFileName,"w")) == NULL) {
		ErrPostEx(SEV_ERROR,0,0,"Cannot open file: %s",IsNullStr(pcFileName));
		return SLRI_FAIL;
	}
	fprintf (fp, "# Difference matrix file\n");
	fprintf (fp, "# First state the number of rows/columns.\n");
	fprintf (fp, "# On every line thereafter, have a label followed by tab followed by the differences between the row and every column.\n");
	fprintf (fp, "# This file was created using the %s metric\n", IsNullStr(CLUSTER_MetricNames[(Int2) metric]));
	fprintf (fp, "# Here is the matrix row/column size\n");
	fprintf (fp, "%i\n", iVecNum);
	fprintf (fp, "# Here is the difference matrix\n");

	for(i=0; i < iVecNum; i++) {
		fprintf(fp, "%s\t", IsNullStr(ppcLabels[i]));
		for (j = 0; j < iVecNum; j++) {
			fprintf (fp, "%f\t", (float) ppfMatrix[i][j]);
		}
		fprintf(fp,"\n");
	}
	FileClose(fp);
	return SLRI_SUCCESS;
}


/* CLUSTERING ALGORITHMS */


/** Single Link (Nearest Neighbor) */
void CLUSTER_SL (CLUSTER *cl, Int4 iClustNum, FloatLoPtr *ppfMatrix)
{
    Int4 i = iClustNum, j = 0, cl1 = 0, cl2 = 0;
    cl1 = cl[i].n[0].id;
    cl2 = cl[i].n[1].id;
    for (j = 0; j < i; j++) {
        ppfMatrix [i][j] = ppfMatrix [j][i] = MIN (ppfMatrix [cl1][j], ppfMatrix [cl2][j]);
	}
    for (j = 0; j < i; j++) {
        if (cl[j].id == cl[i].n[0].id || cl[j].id == cl[i].n[1].id) {
			cl[j].id = i;
		}
	}
}

/** Complete Link (Furthest Neighbor) */
void CLUSTER_CL (CLUSTER *cl, Int4 iClustNum, FloatLoPtr *ppfMatrix)
{
    Int4 i = iClustNum, j = 0, cl1 = 0, cl2 = 0;
    cl1 = cl[i].n[0].id;
    cl2 = cl[i].n[1].id;
    for (j = 0; j < i; j++) {
        ppfMatrix[i][j] = ppfMatrix[j][i] = MAX (ppfMatrix[cl1][j], ppfMatrix[cl2][j]);
	}
    for (j = 0; j < i; j++) {
        if (cl[j].id == cl[i].n[0].id || cl[j].id == cl[i].n[1].id) {
			cl[j].id = i;
		}
	}
}

/** Group Average */
void CLUSTER_GA (CLUSTER *cl, Int4 iClustNum, FloatLoPtr *ppfMatrix)
{
    Int4 i = iClustNum, j = 0, cl1 = 0, cl2 = 0;
    cl1 = cl[i].n[0].id;
    cl2 = cl[i].n[1].id;
    for (j = 0; j < i; j++) {
        ppfMatrix [j][i] = ppfMatrix [i][j] =
            (   ((FloatLo) cl[cl1].nodes) * ppfMatrix[j][cl1]
              + ((FloatLo) cl[cl2].nodes) * ppfMatrix[j][cl2] )
            / ((FloatLo) (cl[cl1].nodes + cl[cl2].nodes));
	}
}

/** Weighted Average */
void CLUSTER_WA (CLUSTER *cl, Int4 iClustNum, FloatLoPtr *ppfMatrix)
{
    Int4 i = iClustNum, j = 0, cl1 = 0, cl2 = 0;
    cl1 = cl[i].n[0].id;
    cl2 = cl[i].n[1].id;
    for (j = 0; j < i; j++) {
        ppfMatrix[j][i] = ppfMatrix[i][j] =
            (ppfMatrix[j][cl1] + ppfMatrix[j][cl2]) * 0.5F;

	}
}

/** Unweighted Centroid */
void CLUSTER_UC (CLUSTER *cl, Int4 iClustNum, FloatLoPtr *ppfMatrix)
{
    Int4 i = iClustNum, j = 0, cl1 = 0, cl2 = 0;
    cl1 = cl[i].n[0].id;
    cl2 = cl[i].n[1].id;
    for (j = 0; j < i; j++) {
        ppfMatrix [j][i] = ppfMatrix [i][j] =
          (   ((FloatLo) cl[cl1].nodes) * ppfMatrix[j][cl1]
            + ((FloatLo) cl[cl2].nodes) * ppfMatrix[j][cl2]
            - ((FloatLo) cl[cl1].nodes) * ((FloatLo)cl [cl2].nodes) /
              ((FloatLo) (cl[cl1].nodes + cl[cl2].nodes)) *
              ppfMatrix [cl1][cl2] )
          / ((FloatLo) (cl[cl1].nodes + cl[cl2].nodes));
	}
}

/** Weighted Centroid */
void CLUSTER_WC (CLUSTER *cl, Int4 iClustNum, FloatLoPtr *ppfMatrix)
{
    Int4 i = iClustNum, j = 0, cl1 = 0, cl2 = 0;
    cl1 = cl[i].n[0].id;
    cl2 = cl[i].n[1].id;
    for (j = 0; j < i; j++) {
        ppfMatrix[j][i] = ppfMatrix[i][j] =
            (ppfMatrix[j][cl1] + ppfMatrix[j][cl2]) * 0.5F
          - ppfMatrix[cl1][cl2] * 0.25F;
	}
}


/** Ward's Method (Minimum Variance) */
void CLUSTER_WM (CLUSTER *cl, Int4 iClustNum, FloatLoPtr *ppfMatrix)
{
    Int4 i = iClustNum, j = 0, cl1 = 0, cl2 = 0;
    cl1 = cl[i].n[0].id;
    cl2 = cl[i].n[1].id;
    for (j = 0; j < i; j++) {
        ppfMatrix[j][i] = ppfMatrix[i][j] =
            (   ((FloatLo) (cl[j].nodes + cl[cl1].nodes)) * ppfMatrix[j][cl1]
              + ((FloatLo) (cl[j].nodes + cl[cl2].nodes)) * ppfMatrix[j][cl2]
              - ((FloatLo)  cl[j].nodes) * ppfMatrix[cl1][cl2] )
            / ((FloatLo)   (cl[j].nodes + cl[cl1].nodes + cl[cl2].nodes));
	}
}

/** Free the cluster struc */
CLUSTER CLUSTER_ClusterFree(CLUSTER cl)
{
	Int4 i = 0;
	if(&cl) {
		for(i = 0; i < 2; i++) {
			if(cl.n[i].label) cl.n[i].label = MemFree(cl.n[i].label);
		}
	}
	return cl;
}

/** Free all clusters */
CLUSTER * CLUSTER_ClusterFreeAll(CLUSTER *cl, Int4 iVecNum) 
{
	Int4 i = 0;
	for(i = 0; i < iVecNum; i++) {
		cl[i] = CLUSTER_ClusterFree(cl[i]);
	}
	return MemFree(cl);
}

/** Removes the First set of nodes */
SLRI_ERR CLUSTER_ReIndexClusters (CLUSTER **pcl, Int4 iVecNum)
{
	CLUSTER *t = NULL, *cl = *pcl;
	Int4 i = 0, j = 0;

	/* allocate new array for real clusters */
	if((t = (CLUSTER *) MemNew((size_t)sizeof(CLUSTER)*iVecNum)) == NULL) return SLRI_NOMEM;
	for(i = 0; i < iVecNum-1; i++) {
		t[i] = cl[iVecNum+i];
		for(j = 0; j < 2; j++) {
			if(t[i].ntype[j] == NODETYPE_CLUSTER) t[i].n[j].id = t[i].n[j].id - iVecNum;
		}
		if(t[i].head != 0) t[i].head = t[i].head - iVecNum;
	}
	cl = MemFree(cl);
	*pcl = t;
	return SLRI_SUCCESS;
}

/** Apply a cluster method to a difference matrix */
void CLUSTER_ApplyClusterMethod(CLUSTER_METHOD method, CLUSTER *cl, Int4 iClustNum, FloatLoPtr *ppfMatrix)
{
	     if(method == CLUSTER_METHOD_SL) CLUSTER_SL(cl, iClustNum, ppfMatrix);
	else if(method == CLUSTER_METHOD_CL) CLUSTER_CL(cl, iClustNum, ppfMatrix);
	else if(method == CLUSTER_METHOD_GA) CLUSTER_GA(cl, iClustNum, ppfMatrix);
	else if(method == CLUSTER_METHOD_WA) CLUSTER_WA(cl, iClustNum, ppfMatrix);
	else if(method == CLUSTER_METHOD_UC) CLUSTER_UC(cl, iClustNum, ppfMatrix);
	else if(method == CLUSTER_METHOD_WC) CLUSTER_WC(cl, iClustNum, ppfMatrix);
	else if(method == CLUSTER_METHOD_WM) CLUSTER_WM(cl, iClustNum, ppfMatrix);
}

/** Cluster a matrix of iVecNum x iVecNum differences according to prescribed method
 *  Generates 2*iVecNum - 1 nodes and clusters 
 */
SLRI_ERR CLUSTER_Cluster(CLUSTER_METHOD method, FloatLoPtr *ppfMatrix, CharPtr *ppcLabels, Int4 iVecNum, CLUSTER **pcl)
{
	CLUSTER * cl;
	Int4 i = 0, j = 0, k = 0;
	Int4 n[2];
	FloatLo diff = 0.0;
	SLRI_ERR err = 0;

	n[0] = 0;
	n[1] = 0;

	/* Allocate all clusters */
	if((cl = (CLUSTER *) MemNew ((size_t) sizeof (CLUSTER) * (2*iVecNum-1))) == NULL) return SLRI_FAIL;
	/* Initial nodes from data */
    for (i = 0; i < iVecNum; i++) {
        cl[i].head = 0;
        cl[i].nodes = 1;
        cl[i].n[0].id = cl[i].n[1].id = -1;
        cl[i].diff = 0.0;
        cl[i].id = i;
    }

	/* clusters */
    for (i = iVecNum; i < 2 * iVecNum - 1; i++) {
        cl[i].head = 0;
        diff = 1e20F;
		/* Find the smallest difference between unused nodes/clusters */
        for (j = 0; j < i; j++) {
            if (cl[j].head == 0) { 
				for (k = 0; k < j; k++) {
					if ((cl[k].head == 0) && (ppfMatrix[j][k] < diff)) {
                        n[0] = j;
                        n[1] = k;
                        diff = ppfMatrix[j][k];
					}
				}
			}
			/* The number of nodes in this cluster */
			cl[i].nodes = cl[n[0]].nodes + cl[n[1]].nodes;
		}
        cl[n[0]].head = cl[n[1]].head = i; /* indicate that these nodes/clusters have been clustered */
        /* set the nodes */
		for(j = 0; j < 2; j++) {
			cl[i].n[j].id = n[j];
			if(n[j] < iVecNum) {
				cl[i].ntype[j] = NODETYPE_LEAF;
				cl[i].n[j].label = StringSave(ppcLabels[n[j]]);
			} else {
				cl[i].ntype[j] = NODETYPE_CLUSTER;
			}
		}
		
        cl[i].diff = diff;
        cl[i].id = i;
		ppfMatrix[i][i] = 0.0;
        CLUSTER_ApplyClusterMethod (method, cl, i, ppfMatrix);
    }
	if((err = CLUSTER_ReIndexClusters(&cl, iVecNum)) > 0) return err;

	*pcl = cl;
	return SLRI_SUCCESS;
}

/** A list of the method names */
CharPtr CLUSTER_MethodNames[] = 
{
	"Single Linkage Method",
	"Complete Linkage Method",
	"Group Average Method",
	"Weighted Average Method",
	"Unweighted Centroid Method",
	"Weighted Centroid Method",
	"Ward's Mininum Variance Method"
};

/** Get a pointer to the method name */
CharPtr CLUSTER_GetMethodName(CLUSTER_METHOD method) {
	return CLUSTER_MethodNames[(Int2)method];
}

/** Dump the cluster to file */
SLRI_ERR CLUSTER_DumpCluster(CLUSTER_METHOD method, CLUSTER *cl, Int4 iVecNum, CharPtr pcFileName)
{
	Int4 i = 0;
	FILE *fp = NULL;

	if(cl == NULL || iVecNum <= 1 || pcFileName == NULL) {
		ErrPostEx(SEV_ERROR,0,0,"Invalid Parameters");
		return SLRI_FAIL;
	}

	if((fp = FileOpen(pcFileName,"w")) == NULL) return SLRI_FAIL;
	fprintf(fp,"# method: %s\n", IsNullStr(CLUSTER_GetMethodName((Int2)method)));
	for (i = 0; i < iVecNum-1; i++) {
        fprintf (fp, "%i %f\n", i, cl [i].diff);
        
		if (cl[i].n[0].label) fprintf (fp, "L %s\n", IsNullStr(cl[i].n[0].label));
        else fprintf (fp, "C %i\n", cl[i].n[0].id);
       
		if (cl[i].n[1].label) fprintf (fp, "L %s\n", IsNullStr(cl[i].n[1].label));
        else fprintf (fp, "C %i\n", cl[i].n[1].id);
    }
	FileClose(fp);
	return SLRI_SUCCESS;
}




#define BUFSIZE 1024
#define maxCOLORS 19
FloatLo colors [maxCOLORS][3] = {
	{ 0.0F, 0.0F, 1.0F },
	{ 0.0F, 1.0F, 0.0F },
	{ 0.0F, 1.0F, 1.0F },
	{ 1.0F, 0.0F, 0.0F },
	{ 1.0F, 0.0F, 1.0F },
	{ 1.0F, 1.0F, 0.0F },
	{ 0.0F, 0.0F, 0.5F },
	{ 0.0F, 0.5F, 0.0F },
	{ 0.0F, 0.5F, 0.5F },
	{ 0.5F, 0.0F, 0.0F },
	{ 0.5F, 0.0F, 0.5F },
	{ 0.5F, 0.5F, 0.0F },
	{ 0.3F, 0.3F, 0.7F },
	{ 0.3F, 0.7F, 0.3F },
	{ 0.3F, 0.7F, 0.7F },
	{ 0.7F, 0.3F, 0.3F },
	{ 0.7F, 0.3F, 0.7F },
	{ 0.7F, 0.7F, 0.3F },
	{ 0.3F, 0.3F, 0.3F } };

CharPtr pat [maxCOLORS] = {
	"4 4 {< 7f7fdfdf         >}",
	"2 2 {< 7fbf             >}",
	"6 6 {< bfa3bff717f7     >}",
	"4 4 {< 7fffdfff         >}",
	"4 4 {< 1fff4fff         >}",
	"3 4 {< 7f7fbfbf         >}",
	"3 3 {< 7fbfdf           >}",
	"4 4 {< 9fff0fff         >}",
	"8 6 {< 017d55d710ff     >}",
	"4 6 {< bf5fbfef57ef     >}",
	"6 4 {< ff17ffa3         >}",
	"2 4 {< 5fffbfff         >}",
	"8 8 {< 515f51ff15f515ff >}",
	"3 4 {< 7f7f7f9f         >}",
	"6 6 {< 27bf93fb4bef     >}",
	"4 8 {< bf5f5fbfef5f5fef >}",
	"5 4 {< 9f6f6fff         >}",
	"6 6 {< af27ff27afff     >}",
	"8 8 {< df99fdf7dfccfd7f >}"  };


static Int4 *fontwidths,
    helvetica [] = {
	0,  278,  278,  278,  278,  278,  278,  278,
	278,  278,  278,  278,  278,  278,  278,  278,
	278,  278,  278,  278,  278,  278,  278,  278,
	278,  278,  278,  278,  278,  278,  278,  278,
	278,  278,  355,  556,  556,  889,  667,  222,
	333,  333,  389,  584,  278,  584,  278,  278,
	556,  556,  556,  556,  556,  556,  556,  556,
	556,  556,  278,  278,  584,  584,  584,  556,
	1015,  667,  667,  722,  722,  667,  611,  778,
	722,  278,  500,  667,  556,  833,  722,  778,
	667,  778,  722,  667,  611,  722,  667,  944,
	667,  667,  611,  278,  278,  278,  469,  556,
	222,  556,  556,  500,  556,  556,  278,  556,
	556,  222,  222,  500,  222,  833,  556,  556,
	556,  556,  333,  500,  278,  556,  500,  722,
	500,  500,  500,  334,  260,  334,  584,  278,
	278,  278,  278,  278,  278,  278,  278,  278,
	278,  278,  278,  278,  278,  278,  278,  278,
	278,  333,  333,  333,  333,  333,  333,  333,
	333,  278,  333,  333,  278,  333,  333,  333,
	278,  333,  556,  556,  556,  556,  260,  556,
	333,  737,  370,  556,  584,  333,  737,  333,
	400,  584,  333,  333,  333,  556,  537,  278,
	333,  333,  365,  556,  834,  834,  834,  611,
	667,  667,  667,  667,  667,  667, 1000,  722,
	667,  667,  667,  667,  278,  278,  278,  278,
	722,  722,  778,  778,  778,  778,  778,  584,
	778,  722,  722,  722,  722,  667,  667,  611,
	556,  556,  556,  556,  556,  556,  889,  500,
	556,  556,  556,  556,  278,  278,  278,  278,
	556,  556,  556,  556,  556,  556,  556,  584,
	611,  556,  556,  556,  556,  500,  556,  500   },

	times [] = {
	0,  250,  250,  250,  250,  250,  250,  250,
	250,  250,  250,  250,  250,  250,  250,  250,
	250,  250,  250,  250,  250,  250,  250,  250,
	250,  250,  250,  250,  250,  250,  250,  250,
	250,  333,  408,  500,  500,  833,  778,  333,
	333,  333,  500,  564,  250,  564,  250,  278,
	500,  500,  500,  500,  500,  500,  500,  500,
	500,  500,  278,  278,  564,  564,  564,  444,
	921,  722,  667,  667,  722,  611,  556,  722,
	722,  333,  389,  722,  611,  889,  722,  722,
	556,  722,  667,  556,  611,  722,  722,  944,
	722,  722,  611,  333,  278,  333,  469,  500,
	333,  444,  500,  444,  500,  444,  333,  500,
	500,  278,  278,  500,  278,  778,  500,  500,
	500,  500,  333,  389,  278,  500,  500,  722,
	500,  500,  444,  480,  200,  480,  541,  250,
	250,  250,  250,  250,  250,  250,  250,  250,
	250,  250,  250,  250,  250,  250,  250,  250,
	278,  333,  333,  333,  333,  333,  333,  333,
	333,  250,  333,  333,  250,  333,  333,  333,
	250,  333,  500,  500,  500,  500,  200,  500,
	333,  760,  276,  500,  564,  333,  760,  333,
	400,  564,  300,  300,  333,  500,  453,  250,
	333,  300,  310,  500,  750,  750,  750,  444,
	722,  722,  722,  722,  722,  722,  889,  667,
	611,  611,  611,  611,  333,  333,  333,  333,
	722,  722,  722,  722,  722,  722,  722,  564,
	722,  722,  722,  722,  722,  722,  556,  500,
	444,  444,  444,  444,  444,  444,  667,  444,
	444,  444,  444,  444,  278,  278,  278,  278,
	500,  500,  500,  500,  500,  500,  500,  564,
	500,  500,  500,  500,  500,  500,  500,  500  };


/** Assign groups to the clusters */
void CLUSTER_SetClusterGroups (CLUSTER *cl, Int4 iVecNum, Int4 iCluster, Int4 iGroup)
{
	Int4 i;

	if (iCluster < iVecNum) {
		for (i = 0; i < 2; i++) {
			cl[iCluster].group [i] = iGroup;
			if (cl[iCluster].ntype [i] == NODETYPE_CLUSTER) {
				CLUSTER_SetClusterGroups(cl, iVecNum, cl[iCluster].n[i].id, iGroup);
			}
		}
	}
}

/** Process the clusters and generate postscript */
void CLUSTER_Process (CLUSTER *cl, Int4 i, CLUSTER_TREE_OPTIONS *Options)
{
	static Boolean bFirst = TRUE;
	Int4 j = 0;
	static Int4 currentgroup = -1;
	static Int4 labelcount = 0; 

    for (j = 0; j < 2; j++) {
        if (cl [i].ntype [j] == NODETYPE_CLUSTER) {
            CLUSTER_Process(cl, cl[i].n[j].id, Options);
        } else {
			if (Options->groups > 1 && currentgroup != cl[i].group[j]) {
				currentgroup = cl[i].group[j];
				if (bFirst) bFirst = FALSE;
				else fprintf (Options->fp, "nl\n"); 

				if (Options->bColorLabel || Options->bColorLink || Options->bPatterns) {
					fprintf (Options->fp, "c%ld col\n", (long) currentgroup);
				}
            
				labelcount = 0;
			}	
			if (Options->bLabels) {
				fprintf (Options->fp, "(%s) l\n", IsNullStr(cl[i].n[j].label));
			} else {
				fprintf (Options->fp, "l\n");
			}
            labelcount++;
		}
    }
    if (Options->groups > 1 && labelcount == 1 && (Options->bColorLabel || Options->bColorLink || Options->bPatterns)) {
	    fputs ("c0\n", Options->fp);
	}

    fprintf (Options->fp, "%g c\n", cl[i].diff);
    labelcount--;
}

/** Determinte the postscript width */
FloatLo CLUSTER_PS_StringWidth (CharPtr s, CLUSTER_TREE_OPTIONS *Options)
{
	Char octal [4];
	FloatLo f = 0.0;
	Int4 i = 0, c = 0;

	octal [3] = '\0';
	for (i = 0; s[i]; i++) {
		if (s[i] == '\\') {
			i++;
			if (s[i] == '\\' || s[i] == '(' || s[i] == ')') {
				c = s[i];
			} else {
				StringNCpy(octal, s + i, 3);
				c = (Int4) strtol (octal, (CharPtr *)NULL, 8);
				i += 2;
			}
		} else {
			c = s[i];
		}
		f += Options->fontwidths[c];
	}
	return f / 1000.0F * (FloatLo)Options->fontsize;
}

/** Determine the width for a particular cluster */
void CLUSTER_Process_Width (CLUSTER *cl, Int4 iCluster, FloatLo PNTR pflabelwidth1, FloatLo PNTR pflabelwidth2, CLUSTER_TREE_OPTIONS *Options)
{
    Int4 j = 0;
	static Boolean b2Col = FALSE;
    FloatLo f = 0.0;

    for (j = 0; j < 2; j++) {
        if (cl [iCluster].ntype [j] == NODETYPE_CLUSTER) {
            CLUSTER_Process_Width (cl, cl[iCluster].n[j].id, &(*pflabelwidth1),&(*pflabelwidth2), Options);
        } else {
			f = CLUSTER_PS_StringWidth (cl[iCluster].n[j].label, Options);
			if (b2Col) {
				if (f > *pflabelwidth2) *pflabelwidth2 = f;
				b2Col = FALSE;
			} else {
				if (f > *pflabelwidth1) *pflabelwidth1 = f;
				b2Col = TRUE;
			}
		}
    }
}


/** Set the dendrogram options */
SLRI_ERR CLUSTER_SetDendrogramOptions(CLUSTER_TREE_OPTIONS PNTR *ppOptions, FONTNAME fontname, Int4 fontsize,
			LINKTYPE linktype, FloatLo fexponent, Boolean bLabels, Boolean bRuler, Boolean bColorLabel,
			Boolean bColorLink, Int4 ColorGroups, Boolean bPatterns, Boolean b2Column, Int4 iMinimum, CharPtr pcFileName)
{
	CLUSTER_TREE_OPTIONS *pOptions = NULL;
	FILE *fp = NULL;

	if(ppOptions == NULL) {
		ErrPostEx(SEV_ERROR,0,0,"Invalid Options pointer");
		return SLRI_FAIL;
	}
	if(*ppOptions == NULL) {
		if((pOptions = (CLUSTER_TREE_OPTIONS *) MemNew((size_t)sizeof(CLUSTER_TREE_OPTIONS))) == NULL) return SLRI_NOMEM;
	} else {
		pOptions = *ppOptions;
	}
	if(fontname == FONTNAME_HELVETICA) {
		pOptions->fontname = "Helvetica";
		pOptions->fontwidths = helvetica;
	} else {
		pOptions->fontname = "Times";
		pOptions->fontwidths = times;
	}
	if(fontsize < 4 || fontsize > 8) {
		ErrPostEx(SEV_ERROR,0,0,"Specifiy fontsize between 4 and 8 point");
		fontsize = 8;
	}
	pOptions->fontsize = fontsize;
	pOptions->linktype = linktype;
	pOptions->exponent = fexponent;
	pOptions->bLabels = bLabels;
	pOptions->bRuler = bRuler;
	pOptions->bColorLabel = bColorLabel;
	pOptions->bColorLink = bColorLink;
	pOptions->groups = ColorGroups;
	pOptions->bPatterns = bPatterns;
	pOptions->b2Column = b2Column;
	pOptions->iMinimum = iMinimum;
	if(pcFileName == NULL) {
		ErrPostEx(SEV_ERROR,0,0,"No FileName given, will use %s",DEFAULT_DENDROGRAM_FILE);
		fp = FileOpen(DEFAULT_DENDROGRAM_FILE,"w");
	} else {
		fp = FileOpen(pcFileName,"w");
	}
	if(fp == NULL) ErrPostEx(SEV_ERROR,0,0,"Could not open tree file");
	pOptions->fp = fp;

	*ppOptions = pOptions;
	return SLRI_SUCCESS;
}
				

/** Generate a postscript dendrogram from given options and clusters */
SLRI_ERR CLUSTER_Dendrogram(CLUSTER *cl, Int4 iVecNum, CLUSTER_TREE_OPTIONS *Options)
{
	CharPtr programname = NULL;
	Int4 i = 0, j = 0, k = 0, n = 0, iTotal = 0, TopMargin = 0;
	Int4 x1 = 0, y1 = 0, x2 = 0, y2 = 0;
	FloatLo f = 0.0, step = 0.0;
	time_t tp;
	Int4 PSlevel = 1, top = 0, urx = 459, width = 300, iLeftMargin = 150, iLeftMargin2 = 0;
	Int4Ptr groups = NULL;

FloatLo
    LineSkip = -1,
    LineSkip2 = -1,
    RulerSkip = -1,
    exponent = 1.0,
    maxlabelwidth = 0.0,
    maxlabelwidth1 = 0.0,
    maxlabelwidth2 = 0.0,
    maxvalue = FLT_MIN,
    minvalue = FLT_MAX;
	exponent = Options->exponent;
   
    if (Options->bPatterns) {
		if (Options->bColorLabel == TRUE|| Options->bColorLink == TRUE)
			ErrPostEx(SEV_ERROR,0,0,"No colors with patterns");
		if (Options->groups < 2 || Options->groups > maxCOLORS)
			ErrPostEx(SEV_ERROR,0,0,"Illegal number of patterns: %i", Options->groups);
		PSlevel = 2;
		Options->bLabels = FALSE;
    }

    if ((Options->bColorLabel || Options->bColorLink) && (Options->groups < 2 || Options->groups > maxCOLORS)) ErrPostEx(SEV_ERROR,0,0,"Invalid number of groups with colored labels or links");
    if (Options->bColorLabel && !Options->bLabels) ErrPostEx(SEV_ERROR,0,0,"Color for no labels\n");
    if (Options->fontsize < 4)  ErrPostEx(SEV_ERROR,0,0,"Fontsize too small");
    if (Options->fontsize > 20) ErrPostEx(SEV_ERROR,0,0,"Fontsize too large");
    if (Options->b2Column && !Options->bLabels)	ErrPostEx(SEV_ERROR,0,0,"Placement of labels in two colums without labels");



    /* locate top node */
	top = 0;
	for(i = 0; i < iVecNum-1; i++) {
		if(cl[i].head == 0) {
			top = i;
			maxvalue = cl[i].diff;
		}
		if (cl [i].diff < minvalue) {
			minvalue = cl[i].diff;
		}
	}

    if (Options->iMinimum) {
		minvalue = (FloatLo)Options->iMinimum;
	} else {
        if (minvalue > 0.0) {
            minvalue = 0.0;
		}
    }

    for (i = 0; i < iVecNum-1; i++) {
		for(j = 0; j < 2; j++) {
			if (cl[i].n[j].label) {
				f = 0.0;
				f = CLUSTER_PS_StringWidth (cl[i].n[j].label, Options);
				f = f + iLeftMargin + (FloatLo) width / (FloatLo) pow (maxvalue - minvalue, exponent) * 
					(FloatLo) pow((cl [i].diff - minvalue), exponent) + 5.0F;
				if (f  > urx) {
					urx = (int)f;
				}
			}
		}
	}
    if (Options->groups > 1) {
		/* divide into color groups */
		j = 0;
		for (i = 0; i < iVecNum-1; i++) {
			cl [i].group [0] = cl [i].group [1] = 1;
			for (k = 0; k < 2; k++) {
				if (cl [i].ntype [k] == NODETYPE_LEAF) {
					j++;
				}
			}
		}
		if (Options->groups > j) {
			ErrPostEx(SEV_WARNING,0,0,"Too many groups, groups reduced from %ld to %ld", Options->groups, j);
			Options->groups = j;
		}
		groups = (Int4Ptr) MemNew ((size_t) sizeof(Int4) * Options->groups);
		groups[0] = top;
		for (n = 1; n < Options->groups; n++) {
			f = FLT_MIN;
			for (i = 0; i < n; i++) {
				if ((groups[i] < iVecNum-1) && (cl[groups[i]].diff > f)) {
					j = i;
					f = cl[groups[i]].diff;
				}
			}
			cl[groups[j]].group[0] = n+1;
			cl[groups[j]].group[1] = j+1;

			groups[n] = (Int4) cl[groups[j]].ntype[0] == NODETYPE_CLUSTER ? cl[groups[j]].n[0].id : (Int4)INT_MAX;
			groups[j] = (Int4) cl[groups[j]].ntype[1] == NODETYPE_CLUSTER ? cl[groups[j]].n[1].id : (Int4)INT_MAX;
			CLUSTER_SetClusterGroups(cl, iVecNum, groups [n], n + 1);
		}
    }

    if (Options->bLabels == TRUE) {
		if (LineSkip < 0) {
			LineSkip = 1.2F * (FloatLo) Options->fontsize;
			if (Options->b2Column)
				LineSkip /= 2;
		}
		if (LineSkip2 < 0) {
			LineSkip2 = 1.5F * (FloatLo)LineSkip;
			if (Options->b2Column)
				LineSkip2 *= 2;
		}
		if (RulerSkip < 0)
			RulerSkip = 1.5F * (FloatLo) LineSkip + 4.0F;
    } else {
		if (LineSkip2 < 0)
			LineSkip2 = 4.0F;
		if (LineSkip < 0) {
			LineSkip = 2.0F;
			if ((iVecNum - 1) * LineSkip + (Options->groups - 1) * LineSkip2 > 530) {
				LineSkip = (530 - (Options->groups - 1) * LineSkip2) / (iVecNum - 1);
			}
		}
		if (RulerSkip < 0)
			RulerSkip = LineSkip2 + 4;
    }

	/* Start writing file */
    fputs (
        "%!PS-Adobe-3.0 EPSF-3.0\n"
        "%%BoundingBox: ",
        Options->fp
    );

    x1 = (Int4) (Options->bLabels ? (iLeftMargin - 6.0F - maxlabelwidth) : iLeftMargin - 10);
    if (Options->b2Column) {
		CLUSTER_Process_Width(cl, top, &maxlabelwidth1, &maxlabelwidth2, Options);
		iLeftMargin2 = (Int4) ((FloatLo) iLeftMargin - 4.0F - maxlabelwidth1);
		x1 = (Int4) (iLeftMargin2 - 6.0F - maxlabelwidth2);
    }
    if (Options->bPatterns == TRUE) {
		x1 = 100;
	}
    x2 = urx;
    y1 = (Int4) (700.0F - (FloatLo)(iVecNum-1 - Options->groups + 1) * LineSkip - (FloatLo)(Options->groups - 1) * LineSkip2);
    if (Options->bRuler == TRUE) {
		y1 -= (Int4) (RulerSkip + (FloatLo) Options->fontsize + 1.0F);
    } else if (Options->bLabels == TRUE) {
		y1 -= (Int4) ((FloatLo)Options->fontsize / 2.0F);
	}
    y2 = 701;
    if (Options->bLabels == TRUE) {
		y2 += Options->fontsize / 2;
	}

    fprintf (Options->fp, "%i %i %i %i\n", x1, y1, x2, y2);
    fputs (
        "%%Creator: ",
        Options->fp
    );
    fprintf (Options->fp, "%s", IsNullStr(programname));
    fputs (
        ",SLRI TOOLKIT (c) 2002\n"
        "%%CreationDate: ",
        Options->fp
    );
    time (&tp);
    fputs (asctime (localtime (&tp)), Options->fp);

  
    fputs ("%%Title: ", Options->fp);
    fprintf (Options->fp, "%i\n", Options->groups);
   
    fputs ("%%LanguageLevel: ", Options->fp);
    fprintf (Options->fp, "%i\n", PSlevel);

    fputs (
        "%%EndComments\n"
        "\n"
        "64 dict begin\n"
        "\n",
        Options->fp
    );
    fprintf (
	Options->fp,
		"/pagestart {\n\n"
		"/EXP { %g exp } def\n"
        "/FontName /%s def\n"
        "/FontSize %i def\n"
        "/LineSkip %g def\n"
        "/LineSkip2 %g def\n"
		"\n",
		exponent,
		IsNullStr(Options->fontname),
        Options->fontsize,
		LineSkip,
		LineSkip2
    );
    fprintf (
		Options->fp,
		"/LeftMargin %i def\n",
		iLeftMargin
    );
    if (Options->b2Column)
		fprintf (
		   Options->fp,
		   "/LeftMargin2 %i def\n",
		    iLeftMargin2
		);
    fprintf (
		Options->fp,
		"\n"
        "/Width %i def\n"
        "\n",
		width
    );

    if (Options->bRuler == TRUE) {
		fprintf (
            Options->fp,
		    "/RulerSkip %g def\n"
		    "/RulerStep ",
		    RulerSkip
		);
		step = (FloatLo) pow (10, ceil (log10 (maxvalue - minvalue)) - 1);
		if ((maxvalue - minvalue) / step > 6.0) {
		    step *= 2.0;
		} else if ((maxvalue - minvalue) / step < 3.0) {
		    step *= 0.5;
		}
		fprintf (Options->fp, "%g def\n\n", step);
    }

    if (Options->bColorLink == TRUE)
		fputs (
		   "/clw { 1 setlinewidth } bind def\n"
		   "/blw { .5 setlinewidth } bind def\n",
		   Options->fp
		);
    else
		fputs (
		    ".5 setlinewidth\n",
		    Options->fp
		);

    if (Options->b2Column)
		fputs ("\n/oelinewidth .2 def\n", Options->fp);

    fputs (
        "\n"
        "%%% End of User Options %%%\n"
		"\n",
        Options->fp
    );

    if (Options->bPatterns == TRUE) {
		fputs (
            "<<\n"
            "    /PatternType 1\n"
            "    /PaintType 1\n"
            "    /TilingType 1\n"
            "    /PaintProc {\n"
            "        begin\n"
            "            XStep\n"
            "            YStep\n"
            "            1\n"
            "            [ 1 0 0 1 0 0 ]\n"
            "            data\n"
            "            image\n"
            "        end\n"
            "    }\n"
            ">>\n"
            "/pdict exch def\n"
            "\n"
            "% stack in:  /label width height patterndata\n"
            "% stack out: -\n"
            "/defpattern {\n"
            "    /pat exch def\n"
            "    /y exch def\n"
            "    /x exch def\n"
            "    pdict /BBox [ 0 0 x y ] put\n"
            "    pdict /XStep x put\n"
            "    pdict /YStep y put\n"
            "    pdict /data pat put\n"
            "    pdict [ 72 60 div 0 0\n"
            "            72 60 div 0 0 ] makepattern\n"
            "    def\n"
            "} bind def\n"
		    "\n",
		   Options->fp
		);
		for (i = 0; i < Options->groups; i++)
		   fprintf (Options->fp, "/c%-2i %s defpattern\n", i + 1, IsNullStr(pat [i]));

		fputs (
			"\n"
			"/c0 {\n"
			"    COL {\n"
			"        Y YY ne {\n"
			"            /Pattern setcolorspace P setcolor\n"
			"            100 Y moveto\n"
			"            140 Y lineto\n"
			"            140 YY LineSkip add lineto\n"
			"            100 YY LineSkip add lineto\n"
			"            closepath\n"
			"            fill\n"
			"            0 setgray\n"
			"        } if\n"
			"        /COL false def\n"
			"    } if\n"
			"} bind def\n"
			"\n"
			"/col {\n"
			"    c0\n"
			"    /Y y def\n"
			"    /YY y def\n"
			"    /P exch def\n"
			"    /COL true def\n"
			"} bind def\n"
			"\n"
			"/COL false def\n"
			"\n",
			Options->fp
		);
    }

    if (Options->bColorLink == TRUE || Options->bColorLabel == TRUE) {
		fprintf (Options->fp, "/c0 {\n    0 setgray%s\n    /COL false def\n} bind def\n", Options->bColorLink ? "\n    blw" : "");
		for (i = 0; i < Options->groups; i++)
		    fprintf (
				Options->fp,
				"/c%i { %g %g %g } def\n",
				i + 1,
				colors [i][0],
				colors [i][1],
				colors [i][2]
			);

		fputs (
		    "/col {\n"
		    "    setrgbcolor\n",
		    Options->fp
		);
		if (Options->bColorLink == TRUE)
		    fputs ("    clw\n", Options->fp);

		fputs (
			"    /COL true def\n"
			"} bind def\n"
			"\n"
			"c0\n"
			"\n",
			Options->fp
		);
    }

    fputs (
        "FontName findfont FontSize scalefont setfont\n"
        "\n",
        Options->fp
    );
    fputs (
        "gsave\n"
        "    newpath\n"
        "    0 0 moveto\n"
        "    (Ag) false charpath\n"
        "    pathbbox\n"
        "grestore\n"
        "/Up exch def\n"
		"pop\n"
		"neg /Down exch def\n"
		"pop\n"
        "gsave\n"
        "    newpath\n"
        "    0 0 moveto\n"
        "    (x) false charpath\n"
        "    pathbbox\n"
        "grestore\n"
        "2 div /Shift exch def\n"
		"pop\n"
		"pop\n"
		"pop\n"
		"\n"
        "/y TopMargin def\n"
        "\n",
        Options->fp
    );

    fprintf (Options->fp, "/Min %g def\n/Max %g def\n\n", minvalue, maxvalue);

    if (Options->b2Column)
		fputs ("/oe false def\n\n", Options->fp);

    fputs (
        "/nl {\n"
        "  /y y LineSkip add LineSkip2 sub def\n"
        "} bind def\n"
        "\n",
		Options->fp
    );

    if (Options->bPatterns == TRUE) {
		fputs (
			"/Cstroke {\n"
			"    COL { /YY y def } if\n"
			"    stroke\n"
			"} bind def\n"
			"\n",
			Options->fp
		);
    } else if (Options->bColorLabel && !Options->bColorLink) {
		fputs (
			"/Cstroke {\n"
			"    COL {\n"
			"        currentrgbcolor\n"
			"        0 setgray\n"
			"        stroke\n"
			"        setrgbcolor\n"
			"    } {\n"
			"        stroke\n"
			"    } ifelse\n"
			"} bind def\n"
			"\n",
			Options->fp
		);
    } else {
		fputs (
			"/Cstroke {\n"
			"    stroke\n"
			"} bind def\n"
			"\n",
			Options->fp
		);
	}

    if (Options->bColorLabel) {
		fputs (
			"/Cshow {\n"
			"    COL {\n"
			"        gsave\n"
			"            -1 -1 Down sub rmoveto\n"
			"            dup stringwidth pop 2 add dup 0 rlineto\n"
			"            0 Down Up add 2 add rlineto\n"
			"            neg 0 rlineto\n"
			"            closepath\n"
			"            fill\n"
			"        grestore\n"
			"        gsave\n"
			"            currentgray .4 gt { 0 } { 1 } ifelse setgray\n"
			"            show\n"
			"        grestore\n"
			"    } {\n"
			"        show\n"
			"    } ifelse\n"
			"} bind def\n"
			"\n",
			Options->fp
		);
    } else if (Options->bColorLink){
		fputs (
			"/Cshow {\n"
			"    COL {\n"
			"        currentrgbcolor\n"
			"        0 setgray\n"
			"        4 -1 roll\n"
			"        show\n"
			"        setrgbcolor\n"
			"    } {\n"
			"        show\n"
			"    } ifelse\n"
			"} bind def \n"
			"\n",
			Options->fp
		);
    } else {
		fputs (
			"/Cshow {\n"
			"    show\n"
			"} bind def\n"
			"\n",
			Options->fp
		);
	}

    if (Options->bLabels == TRUE) {
		fputs (
			"% stack in:  (label)\n"
			"% stack out: x y\n"
			"/l {\n"
			"    dup stringwidth pop\n"
			"    neg LeftMargin add 4 sub y Shift sub moveto\n",
			Options->fp
		);
		if (Options->b2Column)
			fputs (
				"    oe {\n"
			"        LeftMargin2 LeftMargin sub 0 rmoveto\n"
			"        Cshow\n"
			"        gsave\n"
			"            LeftMargin2 y moveto\n"
			"            LeftMargin 4 sub y lineto\n"
			"            0 setgray\n"
			"            oelinewidth setlinewidth\n"
			"            stroke\n"
			"        grestore\n"
			"        /oe false def\n"
			"    } {\n"
			"        Cshow\n"
			"        /oe true def\n"
			"    } ifelse\n",
			Options->fp
			);
		else
			fputs (
				"    Cshow\n",
				Options->fp
			);

		fputs (
			"    LeftMargin y\n"
			"    /y y LineSkip sub def\n"
			"} bind def\n"
			"\n",
			Options->fp
		);
    } else {
		fputs (
			"% stack in:  -\n"
			"% stack out: x y\n"
			"/l {\n"
			"    LeftMargin y\n"
			"    /y y LineSkip sub def\n"
			"} bind def\n"
			"\n",
			Options->fp
		);
	}

    fputs (
        "% stack in:  x1 y1 x2 y2 value\n"
		"% stack out: x3 y3\n"
		"/c {\n"
	    "    Min sub EXP Width mul Max Min sub EXP div LeftMargin add\n",
		Options->fp
    );
    if (Options->linktype == LINKTYPE_REC)
		fputs (
			"    5 1 roll\n"
			"    3 index 3 index moveto\n"
			"    4 index 3 index lineto\n"
			"    4 index 1 index lineto\n"
			"    1 index 1 index lineto\n"
			"    Cstroke\n"
			"    exch pop\n"
			"    add 2 div\n"
			"    exch pop\n",
			Options->fp
		);
    else if (Options->linktype == LINKTYPE_ARC)
		fputs (
			"    /x3 exch def\n"
			"    /y2 exch def\n"
			"    /x2 exch def\n"
			"    /y1 exch def\n"
			"    /x1 exch def\n"
			"    /y3 y1 y2 add 2 div def\n"
			"    x1 y1 moveto\n"
			"    x3 x1 sub .552284 mul x1 add y1\n"
			"    x3 y1 y3 sub .552284 mul y3 add\n"
			"    x3 y3 curveto\n"
			"    x3 y2 y3 sub .552284 mul y3 add\n"
			"    x3 x2 sub .552284 mul x2 add y2\n"
			"    x2 y2 curveto\n"
			"    Cstroke\n"
			"    x3 y3\n",
			Options->fp
		);
    else
		fputs (
			"    /x3 exch def\n"
			"    /y2 exch def\n"
			"    /x2 exch def\n"
			"    /y1 exch def\n"
			"    /x1 exch def\n"
			"    /y3 y1 y2 add 2 div def\n"
			"    x1 y1 moveto\n"
			"    x3 y3 lineto\n"
			"    x2 y2 lineto\n"
			"    Cstroke\n"
			"    x3 y3\n",
			Options->fp
		);

    fputs (
        "} bind def\n"
		"\n",
		Options->fp
    );

    fputs (
        "% stack in:  x y (text)\n"
        "% stack out: x y\n"
        "/n {\n"
		"    2 index 3 add\n"
		"    2 index 2 add Down add\n"
        "    moveto\n"
        "    Cshow\n"
        "} bind def\n"
        "\n"
		"} bind def\n\n",
        Options->fp
    );

	fputs ("/data {\n",Options->fp);
    CLUSTER_Process(cl, top, Options);
	fputs ("} bind def\n\n", Options->fp);
/*
    fputs (
        "pop pop\n"
        "\n",
		Options->fp
    );
*/
    if (Options->bRuler)
		fputs (
			"% This draws the ruler\n"
			"/drawruler {\n"
			" /setmark1 {\n"
			"    Min sub EXP Width mul Max Min sub EXP div LeftMargin add\n"
			"    y moveto\n"
			"    0 2 rlineto\n"
			"    stroke\n"
			" } bind def\n"
			"\n"
			" /setmark {\n"
			"    dup\n"
			"    Min sub EXP Width mul Max Min sub EXP div LeftMargin add\n"
			"    y moveto\n"
			"    gsave\n"
			"        0 4 rlineto\n"
			"        stroke\n"
			"    grestore\n"
			"    0 FontSize neg rmoveto\n"
			"    20 string cvs\n"
			"    dup stringwidth pop 2 div neg 0 rmoveto\n"
			"    show\n"
			" } bind def\n"
			"\n"
			" /y y LineSkip add RulerSkip sub def\n"
			" 0 RulerStep 5 div Max {\n"
			"    dup Min ge { setmark1 } { pop } ifelse\n"
			" } for\n"
			" 0 RulerStep Max {\n"
			"    dup Min ge { setmark } { pop } ifelse\n"
			" } for\n"
			" RulerStep neg 5 div dup Min {\n"
			"    dup Max le { setmark1 } { pop } ifelse\n"
			" } for\n"
			" RulerStep neg dup Min {\n"
			"    dup Max le { setmark } { pop } ifelse\n"
			" } for\n"
			" LeftMargin y moveto\n"
			" Width 0 rlineto stroke\n"
			" \n"
			" % This draws the vertical line for X equals 0\n"
			" Min 0 lt Max 0 gt and {\n"
			"    Min neg EXP Width mul Max Min sub EXP div LeftMargin add\n"
			"    dup\n"
			"    y\n"
			"    moveto\n"
			"    TopMargin FontSize 2 div add\n"
			"    lineto\n"
			"    [ 3 ] 0 setdash\n"
			"    stroke\n"
			" } if\n"
			"} bind def\n"
			"\n\n",
			Options->fp
		);
	/* If fits on one page */
	if(((FloatLo)iVecNum - 600.0F / (FloatLo)LineSkip) < 0.0) {
		fputs("/TopMargin 700 def\npagestart\ndata\n",Options->fp);
	} else {
		iTotal = iVecNum;
		while(iTotal > 0) {
			TopMargin += 790;
			iTotal -= (Int4)(790.0F / (FloatLo)LineSkip );
			fprintf(Options->fp,
				"/TopMargin %ld def\n"
				"pagestart\n"
				"data\n",
				(long) TopMargin
			);
			if(iTotal > 0) {
				fputs ("showpage\n\n",Options->fp);
			}
		}
	}
	
	if (Options->bRuler) {
		fputs("drawruler\n",Options->fp);
	}
	fputs(
		"end\n"
		"showpage\n"
        "%%EOF\n",
        Options->fp
    );

	FileClose (Options->fp);

    return 0;
}


/* Original arguments
void syntax (int err)
{
    fprintf (
	     err ? stderr : stdout,
        "\n"
		"Grouped Color Dendrogram Generator, Version\n"
		"(c) P. Kleiweg 1997 - 2002\n"
		"\n"
			"Usage: %s [-2] [-a] [-b float] [-c] [-C] [-e float] [-f int]\n"
			"\t\t[-L] [-n int] [-o filename] [-p] [-r int] [-R] [-s int] [-S int]\n"
			"\t\t[-t] [-T] [cluster file] > file.ps\n"
		"\n"
		"Usage: %s -P -n int [-a] [-b float] [-e float] [-f int]\n"
			"\t\t[-o filename] [-r int] [-R] [-s int] [-S int]\n"
			"\t\t[-t] [-T] [cluster file] > file.ps\n"
		"\n"
		"Usage: %s -E [-o filename]\n"
		"\n"
			"\t-2 : PostScript level 2 (default: level 1)\n"
		"\t-a : curved links (default: rectangular)\n"
			"\t-b : base offset, may be negative\n"
			"\t     (default: minimum of 0.0 and smallest value in cluster file)\n"
			"\t-c : color links (with number of groups from 2 through %i)\n"
			"\t-C : color labels (with number of groups from 2 through %i)\n"
		"\t-e : exponent (default: 1.0)\n"
		"\t-E : example cluster file\n"
		"\t-f : fontsize (default: %i)\n"
		"\t-L : no labels\n"
			"\t-n : number of groups (default: 1)\n"
		"\t-o : output file\n"
		"\t-p : placement of labels in two columns\n"
		"\t-P : patterns (implies: -2 -L, with number of groups from 2 through %i)\n"
		"\t-r : line skip for ruler\n"
		"\t-R : no ruler\n"
		"\t-s : line skip within groups\n"
		"\t-S : line skip between groups\n"
		"\t-t : triangular links (default: rectangular)\n"
		"\t-T : font Times-Roman (default: Helvetica)\n"
		"\n",
		IsNullStr(programname),
		IsNullStr(programname),
		IsNullStr(programname),
		maxCOLORS,
		maxCOLORS,
		8,
		maxCOLORS
    );
    exit (err);
}
*/

/* 
$Log: slri_cluster.c,v $
Revision 1.10  2004/03/31 16:41:54  mjdumont
minor change

Revision 1.9  2004/02/19 23:48:26  mjdumont
Fixed function name call

Revision 1.8  2004/02/02 20:53:46  mjdumont
Added read block file

Revision 1.7  2003/12/18 16:23:38  mjdumont
Fixed bug & compiler warning

Revision 1.6  2003/11/19 02:59:47  michel
Changed the cluster difference file format to be used with Neighbour i.e. spit out/read in a matrix

Revision 1.5  2003/11/18 23:44:30  michel
Added read from difference file for clustering

Revision 1.4  2003/11/06 15:40:42  michel
Removed meaningless type qualifier

Revision 1.3  2003/10/29 19:20:46  michel
Changed variables to static, conflicts with NCBI toolkit function names (imagine)

Revision 1.2  2003/10/16 20:59:50  skuznets

1. Checking for NULL pointers when print values
2. Added function Misc_TrimSpacesAroundString
3. Added macro IsNullStr

Revision 1.1.1.6  2003/03/20 09:32:34  root
Automated import from Sourceforge.net

Revision 1.7  2003/03/19 21:49:51  micheld
Removed constants from header.  Formatted comments to JavaDoc-like

Revision 1.6  2002/09/03 22:08:33  micheld
Fixed compiler warning

Revision 1.5  2002/08/15 04:05:57  micheld
Used defined variable in function

Revision 1.4  2002/07/30 19:17:35  micheld
Fixed multiple definitions

Revision 1.3  2002/07/30 14:59:38  micheld
used NCBI type max/mins

*/


