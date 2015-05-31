/* $Id: slri_stat.h,v 1.4 2003/11/03 15:09:18 michel Exp $
-- ***************************************************************************
--
--  slri_stat.h
--  
--  Statistics C library
--  
--  Authors: Michel Dumontier           micheld@mshri.on.ca
--           Christopher W.V. Hogue,    hogue@mshri.on.ca
--  Date: November 2001
--
--  Hogue Lab - University of Toronto Biochemistry Department and the
--  Samuel Lunenfeld Research Institute, Mount Sinai Hospital
--  http://bioinfo.mshri.on.ca  hogue@mshri.on.ca
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
 
#ifndef _SLRI_STAT_H
#define _SLRI_STAT_H

#include <ncbi.h>
#include <slri_misc.h>
#include <objslrilinkset.h>

#ifdef __cplusplus
extern "C" { 
#endif

/* Dimensional structure for mu lists and getting stuff out of multidimensional arrays */
typedef struct pidim
{            /* Number of array elements */
	Int4Ptr pidim1;    /* 1 */
	Int4Ptr pidim2;    /* 1 */
	Int4Ptr pidim3;    /* 1 */
	Int4Ptr pidim4;    /* 1 */
	Int4Ptr pidim12;   /* 2 */
	Int4Ptr pidim13;   /* 2 */
	Int4Ptr pidim14;   /* 2 */
	Int4Ptr pidim23;   /* 2 */
	Int4Ptr pidim24;   /* 2 */
	Int4Ptr pidim34;   /* 2 */
	Int4Ptr pidim123;  /* 3 */
	Int4Ptr pidim124;  /* 3 */
	Int4Ptr pidim134;  /* 3 */
	Int4Ptr pidim234;  /* 3 */
	Int4Ptr pidim1234; /* 4 */
} PIDim, PNTR PIDimPtr;



/*********************************************************************************************************** 
Log Linear Regression - Made for 3 variable tables of varying dimensions (you must test other variable sizes)
---------------------
  The loglin function performs iterative proportional fitting on a complete or incomplete  
table of counts based on the model that is to be fitted.  A complete table is one which has
no structural zeroes in it.  If zeroes exists, then the loglin function requires knowledge 
of them in order to account for them in the fitting.  The fitted table is used both as an input
of valid cells (indicated by a value of 1 if a nonzero number exists in the table at that position)
and an output for the fitted table. 
  The model depends on the choice of mu terms to be fitted 
  i.e. a full model has all the terms (u, u1, u2, u3, u12, u13, u23, u123).  a full model to be fitted
will return the same table (unless there were structural zeroes to begin with), but if
u12, u13, u23 are to be fitted, a different table emerges.  The differences between such tables 
can be evaluated using SLRI_ComputeLikelyhoodRatios (although the variables are statically set in this
function, you can implement a more generic one, if you like)

************************************************************************************************************
* Log Linear Regression - Table Fitting & Hierachael log linear modelling                                  *
************************************************************************************************************
* SLRI_3DLogLinearRegression          Derives mu terms for a complete model  (all mu terms)                *
*                                      This function accepts a complete (no zeroes) 3D table with the      *
*                                      number of variables and an array of their dimensions and returns a  *
*                                      list of the mu tables and an array of the number of elements in each*
* SLRI_Log_Linear_Fit (generic)       Fits a complete/incomplete table according to supplied marginals     *
* loglin_             (generic)       The fortran (in c code) function (modified)                          *
***********************************************************************************************************/
SLRI_ERR SLRI_LogLinearRegression(FloatHiPtr pfTableIn, Int4 nvar, Int4Ptr piDim, FloatHiPtr *PNTR pppmu_list, Int4Ptr PNTR pmu_list_num, PIDimPtr PNTR pPIDimList);

SLRI_ERR SLRI_Log_Linear_Fit (FloatLoPtr pfTable, Int4 invar, Int4Ptr piDim, Int4Ptr piConfig,
				FloatLoPtr PNTR pfFit, Int4Ptr PNTR pilocmar, FloatLoPtr PNTR pfMarg, Int4 PNTR pinlast, Int4 PNTR pifault);
Int4 loglin_(Int4 *nvar, Int4 *dim, Int4 *ncon, Int4 *config, Int4 *ntab, FloatLo *table, FloatLo *fit, 
	Int4 *locmar, Int4 *nmar, FloatLo *marg, Int4 *nu, FloatLo *u, FloatLo *maxdev, Int4 *maxit, FloatLo *dev, Int4 *nlast, Int4 *ifault);

SLRI_ERR SLRI_CalculateMuScore(Int4Ptr piPotential, FloatHiPtr *mu_list, Int4Ptr mu_list_num, PIDimPtr PIDimList, FloatLo PNTR pscore, Int2 iModel);
/***********************************************************************************************************
* SLRI_ComputeLikelyhoodRatios        Computes the likelyhood ratios (G^2) for a variety of loglin models  *
*                                       and writes a summary table to the file pointed by fp               *
* SLRI_CalculateChiSquared            Computes the Chi Squared between an array of invar cells             *
***********************************************************************************************************/
SLRI_ERR SLRI_ComputeLikelyhoodRatios(FloatLoPtr pfTable, Int4 invar, Int4Ptr pidim, FILE* fp, FloatLoPtr PNTR pdg2, Int4Ptr PNTR ppf);
SLRI_ERR SLRI_CalculateChiSquared(FloatLoPtr pfTableObs, FloatLoPtr pfTableExp, Int4 ntab, FloatLo PNTR pflX2); /* not tested */
SLRI_ERR SLRI_CalculateZScore(FloatLo score, FloatLoPtr pfTable, Int4 ntab, FloatLo PNTR ZScore);               

/***********************************************************************************************************
* Writing out mu terms                                                                                     *
************************************************************************************************************
* SLRI_Dump3DLogLinearRegressionWithHeader  Writes all the mu terms to file with a summary header          *
* SLRI_Dump3DLogLinearRegression      Writes all the mu terms to file without header                       *
* SLRI_Dump3DLogLinearRegressionEx    Writes indicated mu terms to file                                    *
***********************************************************************************************************/
SLRI_ERR SLRI_Dump3DLogLinearRegressionWithHeader(Int4 nvar, FloatHiPtr *mu_list, Int4Ptr mu_list_num, PIDimPtr PIDimList, CharPtr **desc, FILE *fp);
SLRI_ERR SLRI_Dump3DLogLinearRegression(Int4 nvar, FloatHiPtr *mu_list, Int4Ptr mu_list_num, PIDimPtr PIDimList, CharPtr **desc, FILE *fp);
SLRI_ERR SLRI_Dump3DLogLinearRegressionEx(Int4 nvar, FloatHiPtr *mu_list, Int4Ptr mu_list_num, PIDimPtr PIDimList, CharPtr **desc, FILE *fp, Boolean *mu_list_print, Boolean bNegateOutput);


/***********************************************************************************************************
* Miscellaneous  (tested only with 3 variable tables)                                                      *
************************************************************************************************************

* SLRI_MuListNew                      Creates an array for mu tables, the number of cells in each and the  *
*                                       dimension list of all combinations (used for getting 2D,3D,4D array*
*                                       values                                                             *
* SLRI_MuListFree                     Frees the array of tables, table dimensions and cell lists           *
* SLRI_BLDescNew                      Creates an array of variable dimension descriptions                  *
* SLRI_BLDescFree                     Frees the array of variable dimension descriptions                   *
***********************************************************************************************************/
SLRI_ERR SLRI_MuAndPIDimListNew(Int4 nvar, Int4Ptr piDim, FloatHiPtr *PNTR ppmu_list, Int4Ptr *pmu_list_num, PIDimPtr PNTR pPIDimList);
SLRI_ERR SLRI_MuPIDimListFree(Int4 nvar, FloatHiPtr *PNTR ppmu_list, Int4Ptr PNTR pmu_list_num, PIDimPtr PNTR pPIDimList);

SLRI_ERR SLRI_BLDescNew(Int4 nvar, Int4Ptr mu_list_num, CharPtr **PNTR pcdesc);
void SLRI_BLDescFree(Int4 nvar, Int4Ptr mu_list_num, CharPtr **PNTR ppppcdesc);


/***********************************************************************************************************
* Bryant Lawrence Potential                                                                                *
************************************************************************************************************
* GenerateBLPotential               Generates the BL potential by adding the mu terms r + rd, and rs + rsd *
*                                     seperately                                                           *
* SLRI_DumpBLLogLinearRegression    Writes the BL potential to file                                        *
***********************************************************************************************************/
SLRI_ERR GenerateBLPotential(FloatHiPtr *mu_list, Int4Ptr mu_list_num, PIDimPtr PIDimList, FloatHiPtr *PNTR pppBL_mu_list);
SLRI_ERR SLRI_DumpBLLogLinearRegression(FloatHiPtr *mu_list, Int4Ptr mu_list_num, PIDimPtr PIDimList, FILE *fp);


/***********************************************************************************************************
* BASIC STATISTICS                                                                                         *
***********************************************************************************************************/
/* Gets the minimum and maximum values for a vector */
SLRI_ERR STAT_RowMinMax(FloatLoPtr pfData, Int4 iVecSize, FloatLo PNTR pfMin, FloatLo PNTR pfMax);
/* Calculates the mean for a FloatLo array pfData of size iVecSize to pfmean*/
SLRI_ERR STAT_RowMean(FloatLoPtr pfData, Int4 iVecSize, FloatLo *pfmean);
/* Calculates the standard deviation for data in the pfData array of size iVecSize with mean fmean to pfstddev */
SLRI_ERR STAT_RowStdDev(FloatLoPtr pfData, Int4 iVecSize, FloatLo fmean, FloatLo PNTR pfstddev);
/* Rescales the vector by the minimum and range of the vector, to make all elements lie between 0 and 1 */
/* multiplied by non-zero fScaleFactor */
SLRI_ERR STAT_RowNormalize(FloatLoPtr pfData, Int4 iVecSize, FloatLo fScaleFactor);

/* Gets the minimum and maximum values for each column */
SLRI_ERR STAT_ColumnMinMax(FloatLoPtr *ppfData, Int4 iVecNum, Int4 iVecSize, FloatLoPtr PNTR ppfMin, FloatLoPtr PNTR ppfMax);
/* Calculates the mean for each column in the FloatLo matrix ppfData[iVecNum][iVecSize] to a new FloatLo array ppfmean */
SLRI_ERR STAT_ColumnMean(FloatLoPtr *ppfData, Int4 iVecNum, Int4 iVecSize, FloatLoPtr *ppfmean);
/* Calculates the standard deviation for each column in the FloatLo matrix ppfData[iVecNum][iVecSize] to a new FloatLo array ppfstddev */
SLRI_ERR STAT_ColumnStdDev(FloatLoPtr *ppfData, Int4 iVecNum, Int4 iVecSize, FloatLoPtr pfmean, FloatLoPtr PNTR ppfstddev);
/* Center and reduce the column vectors. */
SLRI_ERR STAT_CenterAndReduce(FloatLoPtr *ppfData, Int4 iVecNum, Int4 iVecSize, FloatLoPtr pfmean, FloatLoPtr pfstddev, Boolean bReduce);

/* Dump to file the mean and standard deviations */
SLRI_ERR STAT_DumpMeanAndStdDev(FloatLoPtr pfMean, FloatLoPtr pfStdDev, Int4 iVecSize, FILE *fp);

/** Histograms */
/** This set of functions allows one to generate a histogram based on the values in the vector compoenent of flvIn
    InitHistogram requires the lower and upper limit to count and [the interval OR the number of bins]
	AddToHistogram adds flvIn to flvHist 
	PrintHistogram prints a summary of the distribution. */
/** NOTE: These histogram functions are not thread safe */
SLRI_ERR STAT_InitHistogram (FloatLo lowerlimit, FloatLo upperlimit, FloatLo interval, Int4 bins, FLoVectorPtr * flvhist);
SLRI_ERR STAT_AddToHistogram(FLoVectorPtr flvHist, FLoVectorPtr flvIn);
SLRI_ERR STAT_PrintHistogram(FLoVectorPtr flvHist, FILE *fp);

/** Normalize the FloVector array to zero mean and unit standard deviation */
SLRI_ERR STAT_Normalize(FLoVectorPtr flv);
/** Normalize the array between a high and low vallue */
SLRI_ERR STAT_NormalizeToInterval(FLoVectorPtr flv, FloatLo fMax, FloatLo fMin);

/** Normalize the FloVector array between fMin and fMax */
SLRI_ERR STAT_NormalizeArray(FLoVectorPtr flv, FloatLo fMax, FloatLo fMin);
/** Calculate the Z score for each position in flvIn and put the results in  flvOut (memory may be allocated by function if NULL)*/
SLRI_ERR STAT_CalculateZ(FloatLo fmean, FloatLo fstdev, FLoVectorPtr flvIn, FLoVectorPtr *flvOut);


/***********************************************************************************************************
* PRINCIPAL COMPONENTS ANALYSIS                                                                            *
************************************************************************************************************
   Principal Components Analysis or the Karhunen-Loeve expansion is a
   classical method for dimensionality reduction or exploratory data
   analysis.  One reference among many is: F. Murtagh and A. Heck,
   Multivariate Data Analysis, Kluwer Academic, Dordrecht, 1987.

   Author:
   F. Murtagh
   Phone:        + 49 89 32006298 (work)
                 + 49 89 965307 (home)
   Earn/Bitnet:  fionn@dgaeso51,  fim@dgaipp1s,  murtagh@stsci
   Span:         esomc1::fionn
   email:        fmurtagh@astro.u-strasbg.fr
   web:          http://astro.u-strasbg.fr/~fmurtagh/mda-sw/
   
   F. Murtagh, Munich, 6 June 1989                                  
***********************************************************************************************************
   July 17, 2002, Adapted to the NCBI Toolkit, made modular, extended for labels 
	 compile and slripca in slri/demo/slripca for demo PCA app
	 Michel Dumontier micheld@mshri.on.ca 
*/
/* Calculate the iVecSize*iVecSize correlation matrix */
SLRI_ERR STAT_CorrelationMatrix(FloatLoPtr *ppfData, Int4 iVecNum, Int4 iVecSize, FloatLoPtr PNTR *pppfCorr);
/* Calculate the iVecSize*iVecSize covariance matrix. */
SLRI_ERR STAT_CovariationMatrix(FloatLoPtr *ppfData, Int4 iVecNum, Int4 iVecSize, FloatLoPtr PNTR *pppfCorr);
/* Calculate the iVecSize*iVecSize sum of squares and cross product matrix. */
SLRI_ERR STAT_SSACPMatrix(FloatLoPtr *ppfData, Int4 iVecNum, Int4 iVecSize, FloatLoPtr PNTR *pppfSSACP);
/* Write to file the symmetric matrix */
SLRI_ERR STAT_DumpSymmetricMatrix(FloatLoPtr *ppfMatrix, Int4 iVecSize, FILE *fp);

/* Generates a correlation matrix: finds the mean, stddev, centers and reduces data and writes to file with valid fp */
SLRI_ERR PCA_GenerateCorrelationMatrix(FloatLoPtr *ppfData, Int4 iVecNum, Int4 iVecSize, FloatLoPtr PNTR *pppfCorr, FILE *fp);
/* Generates a covariance matrix : finds the mean, centers the data and writes to file with valid fp */
SLRI_ERR PCA_GenerateCovarianceMatrix(FloatLoPtr *ppfData, Int4 iVecNum, Int4 iVecSize, FloatLoPtr PNTR *pppfCov, FILE *fp);
/* Generates a Sums-of-squares-and-cross-products matrix */
SLRI_ERR PCA_GenerateSSACP(FloatLoPtr *ppfData, Int4 iVecNum, Int4 iVecSize, FloatLoPtr PNTR *pppfSSACP);

/* Reduce a real symmetric matrix to a symmetric tridiagonal matrix */
void PCA_TDR(FloatLoPtr *ppfSymm, Int4 iVecSize, FloatLoPtr PNTR ppfEigen, FloatLoPtr PNTR ppfInter, FloatLoPtr PNTR *pppfSTM);
/* Tridiagonal QL algorithm - use in conjuction with above */
void PCA_TDQL(FloatLoPtr *ppfSTM, Int4 iVecSize, FloatLoPtr pfEigen, FloatLoPtr pfInter);
/* Dump the Eigenvalues and Eigenvectors to file */
SLRI_ERR PCA_DumpEigen(FloatLoPtr *ppfSymm, FloatLoPtr pfEigen, Int4 iVecSize, Int4 iProj, FILE *fp);

/* Form projections of row-points on principal components */
SLRI_ERR PCA_FormRowProjections(FloatLoPtr *ppfData, FloatLoPtr *ppfSTM, FloatLoPtr pfInter, Int4 iVecNum, Int4 iVecSize, FloatLoPtr PNTR *pppfRowProj);
/* Form projecdtions of column-points on principal components */
SLRI_ERR PCA_FormColProjections(FloatLoPtr *ppfSymm, FloatLoPtr *ppfSTM, FloatLoPtr pfEigen, FloatLoPtr pfInter, Int4 iVecNum, Int4 iVecSize, FloatLoPtr PNTR *pppfColProj);
/* Dumps the Projects to file */
SLRI_ERR PCA_DumpProjections(FloatLoPtr *ppfColProj, CharPtr *ppcColLabels, FloatLoPtr *ppfRowProj, CharPtr *ppcRowLabels, Int4 iVecNum, Int4 iVecSize, Int4 iProj, FILE *fp);

#ifdef __cplusplus
}
#endif
#endif

/*
$Log: slri_stat.h,v $
Revision 1.4  2003/11/03 15:09:18  michel
minor changes

Revision 1.3  2003/10/20 19:37:33  michel
Added 2 more functions to work with FLoVector data - to normalize & to calculate Zscore

Revision 1.2  2003/10/20 19:29:48  michel
Added histogram functionality

Revision 1.1.1.8  2002/10/21 08:34:08  root
Automated import from Sourceforge.net

Revision 1.8  2002/10/20 17:15:42  micheld
Added VNP<->FloatLoArray and stat functions

Revision 1.7  2002/07/22 22:10:41  micheld
removed comment within comment

Revision 1.6  2002/07/18 23:22:31  micheld
Added Principal Components Analysis and supporting functions

Revision 1.5  2002/07/03 23:10:04  micheld
Added CalculateMuScore to slri lib

Revision 1.4  2001/12/07 16:56:42  micheld
added extra parameters for computelikelyhoodratios

Revision 1.3  2001/11/29 21:29:16  micheld
minor change

Revision 1.2  2001/11/19 21:40:05  micheld
Added extra functions, made some progress towards 4D loglin regression
(incomplete)

Revision 1.1  2001/11/09 22:04:31  micheld
Added Log linear regression and hierarchial log linear modelling

*/
