/* $Id: slri_stat.c,v 1.8 2003/11/05 17:47:26 michel Exp $

-- ***************************************************************************
--
--  Samuel Lunenfeld Research Institute (SLRI)
--
--  File: slri_stat.c
--
--  Description:  Statistics functions 
--	   
--
--  AUTHORS:  Michel Dumontier            micheld@mshri.on.ca
--            and Christopher W.V. Hogue  hogue@mshri.on.ca
--
--
--  Copyright Notice:
--
--  Copyright ©2001 Mount Sinai Hospital (MSH)
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
#include <slri_math.h>
#include <slri_stat.h>

#ifdef OS_UNIX_QNX
#undef FLT_MAX
#define FLT_MAX 3.402823466e+38f
#endif

static CharPtr BL_D_Bin[] = {"0-5","5-6","6-7","7-8","8-9","9-10"};
static CharPtr BL_A3_Bin[] = {"ALA","ARG","ASN","ASP","CYS","GLN","GLU","GLY","HIS","ILE","LEU","LYS","MET","PHE","PRO","SER","THR","TRP","TYR","VAL","PEP"};
/* static CharPtr BL_A1_Bin[] = {"ARNDCNEGHILKMFPSTWYV-"}; */

/* For getting a 2D, 3D OR 4D array value.  Pass in variables, and the last (Z) is an
   array with the dimensions of the multidimentional array */
#define Get2DArrayValue(A,B,Z)     (Int4) ((B)*(Z[0])+(A))
#define Get3DArrayValue(A,B,C,Z)   (Int4) ((C)*(Z[1])*(Z[0])+(B)*(Z[0])+(A))
#define Get4DArrayValue(A,B,C,D,Z) (Int4) ((D)*(Z[2])*(Z[1])*(Z[0])+(C)*(Z[1])*(Z[0])+(B)*(Z[0])+(A))

#define ASIGN(a, b) ( (b) < 0 ? -fabs(a) : fabs(a) )
#define EPS 0.005F


/* Subroutines for loglin algorithm */
static Int4 collap_(), adjust_();

/*        ALGORITHM AS 51  APPL. STATIST. (1972) VOL.21, P.218 */

/*        PERFORMS AN ITERATIVE PROPORTIONAL FIT OF THE MARGINAL */
/*        TOTALS OF A CONTINGENCY TABLE. */

/*        THIS VERSION PERMITS UP TO SEVEN VARIABLES. IF THIS LIMIT */
/*        IS TOO SMALL, CHANGE THE VALUE OF MAXVAR, AND OF THE */
/*        DIMENSION IN THE DECLARATION OF CHECK AND ICON - SEE */
/*        ALSO THE CHANGES NEEDED IN AS 51.1 AND AS 51.2 */
Int4 loglin_(Int4 *nvar, Int4 *dim, Int4 *ncon, Int4 *config, Int4 *ntab, FloatLo *table, FloatLo *fit, 
	Int4 *locmar, Int4 *nmar, FloatLo *marg, Int4 *nu, FloatLo *u, FloatLo *maxdev, Int4 *maxit, FloatLo *dev, Int4 *nlast, Int4 *ifault)
{
    /* Initialized data */

    static Int4 maxvar = 7;
    static FloatLo zero = (float)0.;
	static Int4 c__1 = 1;
    /* System generated locals */
    Int4 config_dim1, config_offset, i__1, i__2, i__3;

    /* Local variables */
    static Int4 icon[7], size;
    static FloatLo xmax;
    static Int4 i__, j, k;
    static Boolean check[7];
    static Int4 n;
    static FloatLo x, y;
    static Int4 point;

    /* Parameter adjustments */
    --dim;
    --locmar;
    config_dim1 = *nvar;
    config_offset = 1 + config_dim1 * 1;
    config -= config_offset;
    --fit;
    --table;
    --marg;
    --u;
    --dev;

    /* Function Body */

    *ifault = 0;
    *nlast = 0;

/*        CHECK VALIDITY OF NVAR, THE NUMBER OF VARIABLES, */
/*        AND OF MAXIT, THE MAXIMUM NUMBER OF ITERATIONS */

    if (*nvar > 0 && *nvar <= maxvar && *maxit > 0) {
	goto L10;
    }
L5:
    *ifault = 4;
    return 0;

/*  LOOK AT TABLE AND FIT CONSTANTS */

L10:
    size = 1;
    i__1 = *nvar;
    for (j = 1; j <= i__1; ++j) {
		if (dim[j] <= 0) {
			goto L5;
		}
		size *= dim[j];
    }
    if (size <= *ntab) {
		goto L40;
    }
L35:
    *ifault = 2;
    return 0;
L40:
    x = zero;
    y = zero;
    i__1 = size;
    for (i__ = 1; i__ <= i__1; ++i__) {
		if (table[i__] < zero || fit[i__] < zero) {
			goto L5;
		}
		x += table[i__];
		y += fit[i__];
	}

	/*  MAKE A PRELIMINARY ADJUSTMENT TO OBTAIN THE FIT */
	/*   TO AN EMPTY CONFIGURATION LIST */
	if (y == zero) {
		goto L5;
	}
	x /= y;
	i__1 = size;
	for (i__ = 1; i__ <= i__1; ++i__) {
		fit[i__] = x * fit[i__];
	}
	if (*ncon <= 0 || config[config_dim1 + 1] == 0) {
		return 0;
	}

/* ALLOCATE MARGINAL TABLES */
    point = 1;
    i__1 = *ncon;
    for (i__ = 1; i__ <= i__1; ++i__) {
/*        A ZERO BEGINNING A CONFIGURATION INDICATES */
/*        THAT THE LIST IS COMPLETED */
		if (config[i__ * config_dim1 + 1] == 0) {
			goto L160;
		}
/*        GET MARGINAL TABLE SIZE. WHILE DOING THIS */
/*        TASK, SEE IF THE CONFIGURATION LIST CONTAINS */
/*        DUPLICATIONS OR ELEMENTS OUT OF RANGE. */

		size = 1;
		i__2 = *nvar;
		for (j = 1; j <= i__2; ++j) {
			check[j - 1] = FALSE;
		}
		i__2 = *nvar;
		for (j = 1; j <= i__2; ++j) {
			k = config[j + i__ * config_dim1];
			if (k == 0) { /* A ZERO INDICATES THE END OF THE STRING */
				goto L130;
			}
			if (k >= 0 && k <= *nvar) { /*  SEE IF ELEMENT VALID */
				goto L100;
			}
L95:
			*ifault = 1;
			return 0;
L100:
			if (check[k - 1]) { /* CHECK FOR DUPLICATION */
				goto L95;
			}
			check[k - 1] = TRUE;
			size *= dim[k]; /* GET SIZE */
		}

	/*        SINCE U IS USED TO STORE FITTED MARGINALS, */
	/*        SIZE MUST NOT EXCEED NU */
	L130:
		if (size > *nu) {
			goto L35;
		}
		locmar[i__] = point; /*  LOCMAR POINTS TO MARGINAL TABLES TO BE PLACED IN MARG */
		point += size;
    }
    i__ = *ncon + 1; /* GET N, NUMBER OF VALID CONFIGURATIONS */
L160:
    n = i__ - 1;
    if (point > *nmar + 1) { /* SEE IF MARG CAN HOLD ALL MARGINAL TABLES */
		goto L35;
    }

/* OBTAIN MARGINAL TABLES */

    i__1 = n;
    for (i__ = 1; i__ <= i__1; ++i__) {
		i__2 = *nvar;
		for (j = 1; j <= i__2; ++j) {
			icon[j - 1] = config[j + i__ * config_dim1];
		}
		collap_(nvar, &table[1], &marg[1], &locmar[i__], &dim[1], icon);
    }
 
/* PERFORM ITERATIONS */

    i__1 = *maxit;
    for (k = 1; k <= i__1; ++k) {

/* XMAX IS MAXIMUM DEVIATION OBSERVED BETWEEN */
/* FITTED AND TRUE MARGINAL DURING A CYCLE */
		xmax = zero;
		i__2 = n;
		for (i__ = 1; i__ <= i__2; ++i__) {
			i__3 = *nvar;
			for (j = 1; j <= i__3; ++j) {
				icon[j - 1] = config[j + i__ * config_dim1];
			}
			collap_(nvar, &fit[1], &u[1], &c__1, &dim[1], icon);
			adjust_(nvar, &fit[1], &u[1], &marg[1], &locmar[i__], &dim[1], icon, &xmax);
		}
		dev[k] = xmax; /* TEST CONVERGENCE */
		if (xmax < *maxdev) {
			goto L240;
		}
    }
    if (*maxit > 1) {
		goto L230;
    }
    *nlast = 1;
    return 0;
L230:  /* NO CONVERGENCE */
    *ifault = 3;
    *nlast = *maxit;
    return 0;
L240:  /* NORMAL TERMINATION */
    *nlast = k;
    return 0;
} /* loglin_ */


/* Subroutine */ Int4 collap_(Int4 *nvar, FloatLo *x, FloatLo *y, Int4 *locy, Int4 *dim, Int4 *config)
{
    static FloatLo zero = (float)0.;
    Int4 i__1;
    static Int4 locu, size[8], i__, j, k, l, n, coord[7];


/*        ALGORITHM AS 51.1  APPL. STATIST. (1972) VOL.21, P.218 */

/*        COMPUTES A MARGINAL TABLE FROM A COMPLETE TABLE. */
/*        ALL PARAMETERS ARE ASSUMED VALID WITHOUT TEST. */

/*        IF THE VALUE OF NVAR IS TO BE GREATER THAN 7, THE */
/*        DIMENSIONS IN THE DECLARATIONS OF SIZE AND COORD MUST */
/*        BE INCREASED TO NVAR+1 AND NVAR RESPECTIVELY. */


/*        THE LARGER TABLE IS X AND THE SMALLER ONE IS Y */

    /* Parameter adjustments */
    --config;
    --dim;
    --x;
    --y;

    /* Function Body */

/*        INITIALISE ARRAYS */

    size[0] = 1;
    i__1 = *nvar;
    for (k = 1; k <= i__1; ++k) {
	l = config[k];
	if (l == 0) {
	    goto L20;
	}
	size[k] = size[k - 1] * dim[l];
/* L10: */
    }

/*        FIND NUMBER OF VARIABLES IN CONFIGURATION */

    k = *nvar + 1;
L20:
    n = k - 1;

/*        INITIALISE Y. FIRST CELL OF MARGINAL TABLE IS */
/*        AT Y(LOCY) AND TABLE HAS SIZE(K) ELEMENTS */

    locu = *locy + size[k - 1] - 1;
    i__1 = locu;
    for (j = *locy; j <= i__1; ++j) {
/* L30: */
	y[j] = zero;
    }

/*        INITIALISE COORDINATES */

    i__1 = *nvar;
    for (k = 1; k <= i__1; ++k) {
/* L50: */
	coord[k - 1] = 0;
    }

/*        FIND LOCATIONS IN TABLES */

    i__ = 1;
L60:
    j = *locy;
    i__1 = n;
    for (k = 1; k <= i__1; ++k) {
	l = config[k];
	j += coord[l - 1] * size[k - 1];
/* L70: */
    }
    y[j] += x[i__];

/*        UPDATE COORDINATES */

    ++i__;
    i__1 = *nvar;
    for (k = 1; k <= i__1; ++k) {
	++coord[k - 1];
	if (coord[k - 1] < dim[k]) {
	    goto L60;
	}
	coord[k - 1] = 0;
/* L80: */
    }
    return 0;
} /* collap_ */


/* Subroutine */ Int4 adjust_(Int4 *nvar, FloatLo *x, FloatLo *y, FloatLo *z__, 
Int4 *locz, Int4 *dim, Int4 *config, FloatLo *d__)
{
    /* Initialized data */
    static FloatLo zero = (float)0.;
    /* System generated locals */
    Int4 i__1;
    FloatLo r__1;
    /* Local variables */
    static Int4 size[8];
    static FloatLo e;
    static Int4 i__, j, k, l, n, coord[7];


/*        ALGORITHM AS 51.2  APPL. STATIST. (1972) VOL.21, P.218 */

/*        MAKES PROPORTIONAL ADJUSTMENT CORRESPONDING TO CONFIG. */
/*        ALL PARAMETERS ARE ASSUMED VALID WITHOUT TEST. */

/*        IF THE VALUE OF NVAR IS TO BE GREATER THAN 7, THE */
/*        DIMENSIONS IN THE DECLARATIONS OF SIZE AND COORD MUST */
/*        BE INCREASED TO NVAR+1 AND NVAR RESPECTIVELY. */


    /* Parameter adjustments */
    --config;
    --dim;
    --x;
    --y;
    --z__;

    /* Function Body */


/*        SET SIZE ARRAY */
    size[0] = 1;
    i__1 = *nvar;
    for (k = 1; k <= i__1; ++k) {
		l = config[k];
		if (l == 0) {
			goto L20;
		}
		size[k] = size[k - 1] * dim[l];
    }

/*        FIND NUMBER OF VARIABLES IN CONFIGURATION */
    k = *nvar + 1;
L20:
    n = k - 1;
/*        TEST SIZE OF DEVIATION */

    l = size[k - 1];
    j = 1;
    k = *locz;
    i__1 = l;
    for (i__ = 1; i__ <= i__1; ++i__) {
		r__1 = z__[k] - y[j];
		e = (FloatLo) fabs(r__1);
		if (e > *d__) {
			*d__ = e;
		}
		++j;
		++k;
    }

/* INITIALIZE COORDINATES */

    i__1 = *nvar;
    for (k = 1; k <= i__1; ++k) {
		coord[k - 1] = 0;
    }
    i__ = 1;

/*        PERFORM ADJUSTMENT */

L50:
    j = 0;
    i__1 = n;
    for (k = 1; k <= i__1; ++k) {
		l = config[k];
		j += coord[l - 1] * size[k - 1];
    }
    k = j + *locz;
    ++j;

/*        NOTE THAT Y(J) SHOULD BE NON-NEGATIVE */

    if (y[j] <= zero) {
		x[i__] = zero;
    }
    if (y[j] > zero) {
		x[i__] = x[i__] * z__[k] / y[j];
    }

/*        UPDATE COORDINATES */

    ++i__;
    i__1 = *nvar;
    for (k = 1; k <= i__1; ++k) {
		++coord[k - 1];
		if (coord[k - 1] < dim[k]) {
			goto L50;
		}
		coord[k - 1] = 0;
    }
    return 0;
} /* adjust_ */


/* toolkit interface to fortran loglin function */
SLRI_ERR SLRI_Log_Linear_Fit (FloatLoPtr pfTable, Int4 invar, Int4Ptr piDim, Int4Ptr piConfig,
					FloatLoPtr PNTR pfFit, Int4Ptr PNTR pilocmar, FloatLoPtr PNTR pfMarg, Int4 PNTR pinlast, Int4 PNTR pifault)
{
/* i indicates input, o output, and b for both */
/*i*/Int4 nvar = 0;                /* the number of variables in the table: i.e. r + s + d */
/*i*/Int4 *dim = NULL;             /* the number of categories in each variable: i.e. 21,21,6 with pep */
/*i*/Int4 ncon = 0;                /* the number s of marginal totals to be fit */
/*i*/Int4 *config= NULL;           /* the sets Ck k = 1..s indcating marginal totals to be fit */
/*i*/Int4 ntab = 0;                /* the number of elements in the table */  
/*i*/FloatLo *table = NULL;              /* the table to be fit */
/*b*/FloatLo *fit = NULL;                /* the fitted table */
/*o*/Int4 *locmar = NULL;          /* pointers to the tables in MARG */
/*i*/Int4 nmar = 0;                /* the dimension of MARG */
/*o*/FloatLo *marg = NULL;               /* the marginal tables to be fit */
/*i*/Int4 nu = 0;                  /* the dimension of u */
/*i*/FloatLo *u = NULL;                  /* a work area used to store fitted marginal tables */
/*i*/FloatLo maxdev = 0.0;               /* the maximum permissible difference between an observed and fitted marginal */
	                                       /* total - a reasonable value was given as 0.25 */
/*i*/Int4 maxit = 0;               /* the maximum permissible number of iterations */
/*o*/FloatLo *dev = NULL;                /* dev(i) is the maximum observed difference encountered in iteration cycle i */
	                                       /* between an observed and fitted marginal total */
/*o*/Int4 nlast = 0;               /* the number of the last iteration */
/*o*/Int4 ifault = 0;              /* an error indicator */
	Int4 i = 0;
	Int4 nconfig = 0;

	/* Get all the input ready for the fortran log line function */
	nvar = (Int4) invar;
	dim = (Int4 *) MemNew((size_t)sizeof(Int4)*nvar);
	for(i = 0; i < nvar; i++) {
		dim[i] = (Int4) piDim[i];
	}
	ncon = nvar;

	nconfig = nvar*nvar;
	config = (Int4 *) MemNew((size_t)sizeof(Int4)*nconfig);
	for(i = 0; i < nconfig; i++) {
		config[i] = (Int4) piConfig[i];
	}

	ntab = 1;
	for(i = 0; i < nvar; i++) {
		ntab *= (Int4) piDim[i];
	}
	table = (FloatLo *) MemNew((size_t)sizeof(FloatLo)*ntab);
	for(i = 0; i < ntab; i++) {
		table[i] = (FloatLo) pfTable[i];
	}

	/* The fitted table... put 1 if value exists, otherwise 0 */
	fit = (FloatLo *) MemNew((size_t)sizeof(FloatLo)*ntab);
	for(i = 0; i < ntab; i++) {
		if(table[i] > 0) {
			fit[i] = (FloatLo) 1.0;
		}
	}
 
	locmar = (Int4 *) MemNew((size_t)sizeof(Int4)*ncon);
	nmar = ntab*nvar+nvar;
	marg = (FloatLo *) MemNew((size_t)sizeof(FloatLo)*nmar);
	nu = nmar;
	u = (FloatLo *) MemNew((size_t)sizeof(FloatLo)*nu);

	maxdev = 0.25;
	maxit = 15;
	dev = (FloatLo *) MemNew((size_t)sizeof(FloatLo)*maxit);
	nlast = 0;
	ifault = 0;

	/* Call the function here */
	loglin_(&nvar, dim, &ncon, config, &ntab, table, fit,
		locmar, &nmar, marg, &nu, u, &maxdev, &maxit, dev, &nlast, &ifault);


	/* Now copy the output */
	if(pfFit) {
		*pfFit = (FloatLoPtr) MemNew((size_t)sizeof(FloatLo)*ntab);
		for(i = 0; i < ntab; i++) {
			(*pfFit)[i] = (FloatLo) fit[i];
		}
		MemFree(fit);
	}
	if(pilocmar) {
		*pilocmar = (Int4Ptr) MemNew((size_t)sizeof(Int4)*ncon);
		for(i = 0; i < ncon; i++) {
			(*pilocmar)[i] = (Int4) locmar[i];
		}
		MemFree(locmar);
	}
	if(pfMarg) {
		*pfMarg = (FloatLoPtr) MemNew((size_t)sizeof(FloatLo)*nmar);
		for(i = 0; i < nmar; i++) {
			(*pfMarg)[i] = (FloatLo) marg[i];
		}
		MemFree(marg);
	}
	if(pinlast) *pinlast = (Int4) nlast;
	if(pifault) *pifault = (Int4) ifault;
	
	MemFree(dim);
	MemFree(config);
	MemFree(table);
	MemFree(dev);

	return SLRI_SUCCESS;
}

SLRI_ERR SLRI_CalculateZScore(FloatLo score, FloatLoPtr pfTable, Int4 ntab, FloatLo PNTR ZScore)
{
	Int4 i;
	FloatLo sum = 0.0, mean = 0.0, stdev = 0.0;
	FloatHi variance = 0.0;
	/* first find the mean score */

	for(i=0; i < ntab; i++) {
		sum += pfTable[i];
	}
	mean = sum/(FloatLo) ntab;

	for(i=0; i < ntab; i++) {
		variance += (FloatHi) pow((double) (pfTable[i] - mean), 2.0);
	}
	stdev = (FloatLo) sqrt(variance / (ntab - 1.0));

	*ZScore = (score - mean) / stdev;

	return SLRI_SUCCESS;
}


SLRI_ERR SLRI_CalculateChiSquared(FloatLoPtr pfTableObs, FloatLoPtr pfTableExp, Int4 ntab, FloatLo PNTR pflX2)
{
	Int4 i;
	FloatLo X = 0.0;

	for(i = 0; i < ntab; i++) {
		if(pfTableObs[i] == 0.0 || pfTableExp[i] == 0.0) {
			ErrPostEx(SEV_ERROR,0,0,"SLRI_G2Statistic:  Invalid Table.");
			return SLRI_FAIL;
		}
		X += ( (FloatLo) pow((double) (pfTableObs[i] - pfTableExp[i]), (double)2.0) / pfTableExp[i] );
	}
	*pflX2 = X;
	return SLRI_SUCCESS;
}

/* Function to calculate the g^2 statistic from log tables */
SLRI_ERR SLRI_CalculateG2(FloatLoPtr pfTableObs, FloatLoPtr pfTableExp, Int4 ntab, Int4 PNTR pflG2)
{
	Int4 i;
	FloatHi g = 0;

	for(i = 0; i < ntab; i++) {
		if(pfTableObs[i] == 0.0 || pfTableExp[i] == 0.0) {
			ErrPostEx(SEV_ERROR,0,0,"SLRI_G2Statistic:  Invalid Table.");
			return SLRI_FAIL;
		}
		g += (FloatHi) ((double) pfTableObs[i] * log ( ((double) pfTableObs[i]) / ((double) pfTableExp[i]) ));
	}
	g = 2*g;
	*pflG2 = (Int4) g;

	return SLRI_SUCCESS;
}




SLRI_ERR SLRI_ComputeLikelyhoodRatios(FloatLoPtr pfTable, Int4 invar, Int4Ptr pidim, FILE* fp, FloatLoPtr PNTR pdg2, Int4Ptr PNTR ppf)
{
	FloatLoPtr dg2 = NULL;
	FloatLoPtr pfFit = NULL, pfMarg = NULL;
	Int4Ptr pilocmar = NULL;
	Int4 i = 0, ntab = 1;	
	Int4 pinlast = 0, pifault = 0;
	Int4 config[9];
	Int4Ptr g2 = NULL; /* g2 values */
	Int4Ptr pf = NULL; /* parameters fitted */

	CharPtr mu_desc[] = {"u1 + u2","u3 + u4 + u5","u6", "u7"};


	if(pfTable == NULL || invar <= 0 || pidim == NULL || fp == NULL) {
		ErrPostEx(SEV_ERROR,0,0,"SLRI_ComputeLikelyhoodRatios: Invalid Parameters.");
		return SLRI_FAIL;
	}
	for(i = 0; i < invar; i++) {
		ntab *= pidim[i];
	}

	g2 = (Int4Ptr) MemNew((size_t)sizeof(Int4)*5);
	pf = (Int4Ptr) MemNew((size_t)sizeof(Int4)*5);
	for(i = 0; i < 9; i++) {
		config[i] = 0;
	}

	/* Get fitted table for no components */
	config[0] = 1;
	if(SLRI_Log_Linear_Fit(pfTable,invar,pidim, config, &pfFit,&pilocmar,&pfMarg,&pinlast,&pifault)) return SLRI_FAIL;
	SLRI_CalculateG2(pfTable,pfFit,ntab,&g2[0]);
	pf[0] = 1;

	/* ur, us */
	config[0] = 1;
	config[3] = 2;
	/* config[6] = 3; */
	if(SLRI_Log_Linear_Fit(pfTable,invar,pidim, config, &pfFit,&pilocmar,&pfMarg,&pinlast,&pifault)) return SLRI_FAIL;
	SLRI_CalculateG2(pfTable,pfFit,ntab,&g2[1]);
	pf[1] = pf[0] + (pidim[0]-1) + (pidim[1]-1) /*+ (pidim[2]-1)*/;

	/* urd, usd, ud */
	config[0] = 1;
	config[1] = 3;
	config[3] = 2;
	config[4] = 3; 
	/*config[6] = 0;*/
	if(SLRI_Log_Linear_Fit(pfTable,invar,pidim, config, &pfFit,&pilocmar,&pfMarg,&pinlast,&pifault)) return SLRI_FAIL;
	SLRI_CalculateG2(pfTable,pfFit,ntab,&g2[2]);
	pf[2] = pf[1] + (pidim[0]-1)*(pidim[2]-1) + (pidim[1]-1)*(pidim[2]-1) + (pidim[2]-1);

	/* urs */
	config[0] = 1;
	config[1] = 2;
	config[3] = 0;
	config[4] = 0;
	if(SLRI_Log_Linear_Fit(pfTable,invar,pidim, config, &pfFit,&pilocmar,&pfMarg,&pinlast,&pifault)) return SLRI_FAIL;
	SLRI_CalculateG2(pfTable,pfFit,ntab,&g2[3]);
	pf[3] = pf[2] + (pidim[0]-1)*(pidim[1]-1);

	/* ursd is the remaining */
	/* This would have a value if we had 4 dimensional table */
	g2[4] = 0;
	pf[4] = ntab;

	/* Now sort the u components by G^2 */

	if(pdg2) {
		dg2 = (FloatLoPtr) MemNew((size_t)sizeof(FloatLo)*5);
	}

	/* printf out values */
	if(fp) {
		fprintf(fp,"Model\tComponents\tG^2\t\tdf\t\tdG^2\t\tddf\t\t%%dG^2\n");
		fprintf(fp,"0\tNone\t\t%-10ld\t%-10ld\n",(long) g2[0],(long) (ntab - pf[0]));
		for(i = 1; i <= 4; i++) {
			dg2[i-1] = (FloatLo) (g2[i-1]-g2[i]) / (FloatLo) g2[0] * (FloatLo) 100.0;
			fprintf(fp,"%ld\t%-14s\t%-10ld\t%-10ld\t%-10ld\t%-10ld\t%-6.2f\n",(long)i, mu_desc[i-1], (long)g2[i],(long) (ntab - pf[i]) ,(long) (g2[i-1] - g2[i]),(long) (pf[i] -1), (float) dg2[i-1]);
		}
	}
	if(pdg2) {
		*pdg2 = dg2;
	} else {
		dg2 = MemFree(dg2);
	}
	g2 = MemFree(g2);
	
	if(ppf) {
		*ppf = pf;
	} else {
		pf = MemFree(pf);
	}

	return SLRI_SUCCESS;
}


/* Should be good for 2D, 3D, 4D */
	
	/* Get sizes and assign memory to each mu           (e.g BL dimensions)
	2D,3D,4D
	mu_list[0] = mu    -> 1
	mu_list[1] = mu1   -> pidim[0]                      (21)
	mu_list[2] = mu2   -> pidim[1]                      (21)
	3D,4D
	mu_list[3] = mu3   -> pidim[2]                      (6)
	4D
	mu_list[4] = mu4   -> pidim[3]
 	
	3D,4D
	mu_list[4,5] = mu12  -> pidim[0] * pidim[1]            (441)
	mu_list[5,6] = mu13  -> pidim[0] * pidim[2]	          (126)
	mu_list[6,8] = mu14  -> pidim[0] * pidim[3]  4D
	mu_list[6,7] = mu23  -> pidim[1] * pidim[2]            (126)
	mu_list[6,9] = mu24  -> pidim[1] * pidim[3]  4D
	mu_list[6,10]= mu34  -> pidim[2] * pidim[3]  4D

	3D,4D
	mu_list[7] = mu123 -> pidim[0]* pidim[1] * pidim[2] (2646)

    
	mu_list[8] = mu124 -> pidim[0]* pidim[1] * pidim[3]  4D
	mu_list[9] = mu134 -> pidim[0]* pidim[2] * pidim[3]  4D
	mu_list[10]= mu234 -> pidim[1]* pidim[2] * pidim[3]  4D

	mu_list[11]= mu1234-> pidim[0]* pidim[1] * pidim[2] * pidim[3]
	*/

SLRI_ERR PIDimNew(Int4 nvar, Int4Ptr *pidim_list, PIDimPtr PNTR pPIDimList)
{
	Int4 i = 0, PIDimList_total = 0;
	PIDimPtr PIDimList = NULL;
	
	if((PIDimList = (PIDimPtr) MemNew((size_t)sizeof(PIDim))) == NULL) return SLRI_FAIL;

	PIDimList->pidim1 = pidim_list[++i];
	PIDimList->pidim2 = pidim_list[++i];	
	if(nvar > 2) PIDimList->pidim3 = pidim_list[++i];
	if(nvar > 3) PIDimList->pidim4 = pidim_list[++i];
	PIDimList->pidim12 = pidim_list[++i];
	if(nvar > 2) {
		PIDimList->pidim13 = pidim_list[++i];
		if(nvar > 3) PIDimList->pidim14 = pidim_list[++i];
		PIDimList->pidim23 = pidim_list[++i];
		if(nvar > 3) PIDimList->pidim24 = pidim_list[++i];
		if(nvar > 3) PIDimList->pidim34 = pidim_list[++i];
		PIDimList->pidim123 = pidim_list[++i];
		if(nvar > 3) {
			PIDimList->pidim124 = pidim_list[++i];
			PIDimList->pidim134 = pidim_list[++i];
			PIDimList->pidim234 = pidim_list[++i];
			PIDimList->pidim1234 = pidim_list[++i];
		}
	}
	PIDimList_total = (Int4) ((FloatHi) (nvar*nvar) - (FloatHi) (ceil((double)nvar/2.0)) +1 /* for mu */);
	for(i = 0; i < PIDimList_total; i++) {
		pidim_list[i] = NULL;
	}

	*pPIDimList = PIDimList;
	return SLRI_SUCCESS;
}

void PIDimFree(Int4 nvar, PIDimPtr PNTR pPIDimList)
{
	PIDimPtr PIDimList = *pPIDimList;

	if(PIDimList) {
		PIDimList->pidim1 = MemFree(PIDimList->pidim1);
		PIDimList->pidim2 = MemFree(PIDimList->pidim2);
		if(nvar > 2) PIDimList->pidim3 = MemFree(PIDimList->pidim3);
		if(nvar > 3) PIDimList->pidim4 = MemFree(PIDimList->pidim4);
		if(nvar > 2) {
			PIDimList->pidim14 = MemFree(PIDimList->pidim13);
			if(nvar > 3) PIDimList->pidim14 = MemFree(PIDimList->pidim14);
			PIDimList->pidim23 = MemFree(PIDimList->pidim23);
			if(nvar > 3) PIDimList->pidim24 = MemFree(PIDimList->pidim24);
			if(nvar > 3) PIDimList->pidim34 = MemFree(PIDimList->pidim34);
			PIDimList->pidim123 = MemFree(PIDimList->pidim123);
			if(nvar > 3) {
				PIDimList->pidim124  = MemFree(PIDimList->pidim124);
				PIDimList->pidim134  = MemFree(PIDimList->pidim134);
				PIDimList->pidim234  = MemFree(PIDimList->pidim234);
				PIDimList->pidim1234 = MemFree(PIDimList->pidim1234);
			}
		}
		PIDimList = MemFree(PIDimList);
	}
	*pPIDimList = PIDimList;
}



SLRI_ERR SLRI_MuAndPIDimListNew(Int4 nvar, Int4Ptr piDim, FloatHiPtr *PNTR ppmu_list, Int4Ptr *pmu_list_num, PIDimPtr PNTR pPIDimList) 
{
	Int4 i,j,k,l,m,mu_list_total;
	Int4Ptr mu_list_num = NULL;
	Int4Ptr *pidim_list = NULL;
	FloatHiPtr *mu_list = NULL;

	if(nvar > 4 ) {
		ErrPostEx(SEV_ERROR,0,0,"Cannot currently handle tables larger than 4 dimensions");
		return SLRI_FAIL;
	}
	/* Allocate memory for mu_list and pidim_list */
	mu_list_total = (Int4) ((FloatHi) (nvar*nvar) - (FloatHi) (ceil((double)nvar/2.0)) +1 /* for mu */);
	if((mu_list_num = (Int4Ptr) MemNew((size_t)sizeof(Int4)*(mu_list_total))) == NULL) return SLRI_FAIL;
	if((mu_list = (FloatHiPtr *) MemNew((size_t)sizeof(FloatHiPtr)*(mu_list_total))) == NULL) return SLRI_FAIL;
	if((pidim_list = (Int4Ptr *) MemNew((size_t)sizeof(Int4Ptr)*(mu_list_total))) == NULL) return SLRI_FAIL;

	for(i = 0; i <= nvar; i++) {  /* u1 - u4 */
		if((pidim_list[i] = (Int4Ptr) MemNew((size_t)sizeof(Int4)*(1))) == NULL) return SLRI_FAIL;
		if( i == 0 ) {
			mu_list_num[i] = 1;
			pidim_list[i][0] = 1;
		} else {
			mu_list_num[i] = piDim[i-1];
			pidim_list[i][0] = piDim[i-1];
		}
		if((mu_list[i] = (FloatHiPtr) MemNew((size_t)sizeof(FloatHi)*(mu_list_num[i]))) == NULL) return SLRI_FAIL;
	}
	if(nvar >= 2) { /* u12, u13, u14, u23, u24, u34 */
		for( j = 0; j < nvar; j++) {
			for ( k = j+1; k < nvar; k++) {
				mu_list_num[i] = piDim[j] * piDim[k];
				
				if((mu_list[i] = (FloatHiPtr) MemNew((size_t)sizeof(FloatHi)*(mu_list_num[i]))) == NULL) return SLRI_FAIL;
				if((pidim_list[i] = (Int4Ptr) MemNew((size_t)sizeof(Int4)*(2))) == NULL) return SLRI_FAIL;
				pidim_list[i][0] = piDim[j];
				pidim_list[i][1] = piDim[k];
				i++;
			}
		}
		if(nvar >= 3) { /* u123, u124, u134, u234 */
			for( j = 0; j < nvar; j++) {
				for ( k = j+1; k < nvar; k++) {
					for (l = k+1; l < nvar; l++) {
						mu_list_num[i] = piDim[j] * piDim[k] * piDim[l];
						if((mu_list[i] = (FloatHiPtr) MemNew((size_t)sizeof(FloatHi)*(mu_list_num[i]))) == NULL) return SLRI_FAIL;
						if((pidim_list[i] = (Int4Ptr) MemNew((size_t)sizeof(Int4)*(3))) == NULL) return SLRI_FAIL;
						pidim_list[i][0] = piDim[j];
						pidim_list[i][1] = piDim[k];
						pidim_list[i][2] = piDim[l];
						i++;
					}
				}
			}
			if(nvar>=4) { /* u1234 */
				for( j = 0; j < nvar; j++) {
					for ( k = j+1; k < nvar; k++) {
						for (l = k+1; l < nvar; l++) {
							for (m = l+1; m < nvar; m++) {
								mu_list_num[i] = piDim[j] * piDim[k] * piDim[l] * piDim[m];

								if((mu_list[i] = (FloatHiPtr) MemNew((size_t)sizeof(FloatHi)*(mu_list_num[i]))) == NULL) return SLRI_FAIL;
								if((pidim_list[i] = (Int4Ptr) MemNew((size_t)sizeof(Int4)*(3))) == NULL) return SLRI_FAIL;
								pidim_list[i][0] = piDim[j];
								pidim_list[i][1] = piDim[k];
								pidim_list[i][2] = piDim[l];
								pidim_list[i][3] = piDim[m];
								i++;
	}}}}}}}

	PIDimNew(nvar, pidim_list,&(*pPIDimList));
	pidim_list = MemFree(pidim_list);

	*pmu_list_num = mu_list_num;
	*ppmu_list = mu_list;


	return SLRI_SUCCESS;
}


SLRI_ERR SLRI_MuListFreeEx(Int4 nvar, FloatHiPtr *PNTR ppmu_list, Int4Ptr PNTR pmu_list_num, PIDimPtr PNTR pPIDimList)
{
	Int4 i;
	FloatHiPtr *mu_list = *ppmu_list;
	Int4Ptr mu_list_num = *pmu_list_num;

	Int4 mu_list_total = (Int4) ((FloatHi) (nvar*nvar) - (FloatHi) (ceil((double)nvar/2.0)) +1 /* for mu */);
	for(i = 0; i < mu_list_total; i++) {
		mu_list[i] = MemFree(mu_list[i]);
	}
	mu_list = MemFree(mu_list);
	mu_list_num = MemFree(mu_list_num);
	if(*pPIDimList) PIDimFree(nvar, &(*pPIDimList));

	*ppmu_list = mu_list;
	*pmu_list_num = mu_list_num;

	return SLRI_SUCCESS;
}

SLRI_ERR SLRI_MuListFree(Int4 nvar, FloatHiPtr *PNTR ppmu_list, Int4Ptr PNTR pmu_list_num)
{
	PIDimPtr pidim_list = NULL;
	return SLRI_MuListFreeEx(nvar, &(*ppmu_list), &(*pmu_list_num), &pidim_list);
}

SLRI_ERR SLRI_MuPIDimListFree(Int4 nvar, FloatHiPtr *PNTR ppmu_list, Int4Ptr PNTR pmu_list_num, PIDimPtr PNTR pPIDimList)
{
	return SLRI_MuListFreeEx(nvar, &(*ppmu_list), &(*pmu_list_num), &(*pPIDimList));
}


SLRI_ERR SLRI_BLDescNew(Int4 nvar, Int4Ptr mu_list_num, CharPtr **PNTR pcdesc)
{
	Int4 i = 0;
	CharPtr **desc = NULL;

	desc = (CharPtr **) MemNew((size_t)sizeof(CharPtr *)*nvar);
	for(i = 0; i < nvar; i++) {
		desc[i] = (CharPtr *) MemNew((size_t)sizeof(CharPtr)*mu_list_num[i+1]);
	}
	for(i = 0; i < mu_list_num[1]; i++) {
		desc[0][i] = StringSave(BL_A3_Bin[i]);
		desc[1][i] = StringSave(BL_A3_Bin[i]);
	}
	for(i = 0; i < mu_list_num[3]; i++) {
		desc[2][i] = StringSave(BL_D_Bin[i]);
	}
	*pcdesc = desc;

	return SLRI_SUCCESS;
}

void SLRI_BLDescFree(Int4 nvar, Int4Ptr mu_list_num, CharPtr **PNTR ppppcdesc)
{
	Int4 i = 0;
	CharPtr **desc = NULL;
	
	desc = *ppppcdesc;

	for(i = 0; i < mu_list_num[1]; i++) {
		desc[0][i] = MemFree(desc[0][i]);
		desc[1][i] = MemFree(desc[1][i]);
	}
	for(i = 0; i < mu_list_num[3]; i++) {
		desc[2][i] = MemFree(desc[2][i]);
	}
	for(i = 0; i < nvar; i++) {
		desc[i] = MemFree(desc[i]);
	}
	*ppppcdesc = desc;
}


/* This function fits a complete (no zeroes) 3D table to a log linear hierarchial model
where mu = mu1 + mu2 + mu3 + mu12 + mu13 + mu23 + mu123, and the mu components are additive
log terms
   pfTableIn    The table to be used
   nvar         The number of variables
   piDim        The dimensions of the table
   pppmu_list   The output list of mus
   pmu_list_num The number of cells for each mu
*/

SLRI_ERR SLRI_LogLinearRegression(FloatHiPtr pfTableIn, Int4 nvar, Int4Ptr piDim, FloatHiPtr *PNTR pppmu_list, Int4Ptr PNTR pmu_list_num, PIDimPtr PNTR pPIDimList)
{
	FloatHiPtr pfTable = NULL;
	FloatHiPtr *mu_list = NULL;
	Int4Ptr mu_list_num = NULL;
	PIDimPtr PIDimList = NULL;

	Int4 ntab = 1;
	Int4 i = 0, j = 0, k = 0, l = 0, m = 0;
	Int4 z = 0;
	FloatHi sum = 0.0;
	
	ntab = 1;
	for(i = 0; i < nvar; i++) {
		ntab *=  piDim[i];
	}
	
	/* Check for completeness */
	for(i = 0; i < ntab; i++) {
		if ( pfTableIn[i] == 0.0 ) {
			/* Call iterative fitting routine */
			ErrPostEx(SEV_ERROR,0,0,"SLRI_3DLogLinearRegression: You must have a complete (non-zero) table.\n"
				"Use loglin from Fortran Library to fit your table.\n");
			return SLRI_FAIL;
	}}

	/* Assing memory to mu variables */
	if(SLRI_MuAndPIDimListNew(nvar, piDim, &mu_list, &mu_list_num, &PIDimList)) return SLRI_FAIL;
	
	/* Log linear model fitting */
	if((pfTable = MemNew((size_t)sizeof(FloatHi)*ntab)) == NULL) return SLRI_FAIL;
	/* Take the log, calculate mu */
	for(i = 0; i < ntab; i++) {
		pfTable[i] = (FloatHi) log((FloatHi)pfTableIn[i]);
		mu_list[z][0] += ((FloatHi) 1.0/(FloatHi) ntab)*pfTable[i];
	}
	/* remove mu from each cell */
	for(i = 0; i < ntab; i++) {
		pfTable[i] = pfTable[i] - mu_list[0][0];
	}
	
/* Now sum along the columns/rows/depth to get the ur, us, ud*/
/* ur */
	z++; /* z = 1 */
	for(j = 0; j < mu_list_num[1]; j++) { /* for each r */
		for(k = 0; k < mu_list_num[2]; k++) {  /* along each s */
			if(nvar > 2) { /* 3D */
				for(l = 0; l < mu_list_num[3]; l++) {  /* d */
					if(nvar > 3) { /* 4D */
						for(m = 0; m <mu_list_num[4]; m++) { /* and species */
							sum += pfTable[Get4DArrayValue(j,k,l,m,PIDimList->pidim1234)];
						}
					} else { 
						sum += pfTable[Get3DArrayValue(j,k,l,PIDimList->pidim123)];
					}
				}
			} else { /* 2D */
				sum += pfTable[Get2DArrayValue(j,k,PIDimList->pidim12)];
			}
		}
		if(nvar > 2 ) {
			if (nvar > 3) {
				mu_list[z][j] = (((FloatHi) 1.0/(FloatHi)(mu_list_num[2]*mu_list_num[3]*mu_list_num[4]))*sum);
			} else {
				mu_list[z][j] = (((FloatHi) 1.0/(FloatHi)(mu_list_num[2]*mu_list_num[3]))*sum);
			}
		} else {
			mu_list[z][j] = ((FloatHi) 1.0/(FloatHi)mu_list_num[2]*sum);
		}
		sum = 0.0;
	}

/* us */
	z++; /* z = 2 */
	for(k = 0; k < mu_list_num[2]; k++) { /* for each r */
		for(j = 0; j < mu_list_num[1]; j++) {  /* along each s */
			if( nvar > 2) { /* 3D */
				for(l = 0; l < mu_list_num[3]; l++) {  /* and d */
					if( nvar > 3) { /* 4D */
						for(m = 0; m <mu_list_num[4]; m++) { /* and species */
							sum += pfTable[Get4DArrayValue(j,k,l,m,PIDimList->pidim1234)];
						}
					} else { 
						sum += pfTable[Get3DArrayValue(j,k,l,PIDimList->pidim123)];
					}
				}
			} else { /* 2D */
				sum += pfTable[Get2DArrayValue(j,k,PIDimList->pidim12)];
			}
		}
		if(nvar > 2 ) {
			if (nvar > 3) {
				mu_list[z][k] = (((FloatHi) 1.0/(FloatHi)(mu_list_num[1]*mu_list_num[3]*mu_list_num[4]))*sum);
			} else {
				mu_list[z][k] = (((FloatHi) 1.0/(FloatHi)(mu_list_num[1]*mu_list_num[3]))*sum);
			}
		} else {
			mu_list[z][k] = ((FloatHi) 1.0/(FloatHi) mu_list_num[1]*sum);
		}

		sum = 0.0;
	}

/* ud */
	if (nvar > 2 ) { 
		z++; /* z = 2,3,3 */
		for(l = 0; l < mu_list_num[3]; l++) { /* for each d */
			for(j = 0; j < mu_list_num[1]; j++) {  /* along each r */
				for(k = 0; k < mu_list_num[2]; k++) {  /* and s */
					if(nvar > 3) {
						for(m = 0; m <mu_list_num[4]; m++) { /* and species */
							sum += pfTable[Get4DArrayValue(j,k,l,m,piDim)];
						}
					} else {
						sum += pfTable[Get3DArrayValue(j,k,l,piDim)];
					}
				}
			}
			if(nvar > 3 ) {
				mu_list[z][l] = (((FloatHi) 1.0/(FloatHi)(mu_list_num[1]*mu_list_num[2]*mu_list_num[4]))*sum);
			} else {
				mu_list[z][l] = (((FloatHi) 1.0/(FloatHi)(mu_list_num[1]*mu_list_num[2]))*sum);
			} 
			sum = 0.0;
		}
	}
/* ut */
	if (nvar > 3) {
		z++; /* z = 2,3,4 */
		for(m = 0; m < mu_list_num[4]; m++) { /* for each t */
			for(j = 0; j < mu_list_num[1]; j++) {  /* along each r */
				for(k = 0; k < mu_list_num[2]; k++) {  /* and s */
					for(l = 0; l <mu_list_num[4]; l++) { /* and d */
						sum += pfTable[Get4DArrayValue(j,k,l,m,PIDimList->pidim1234)];
					}
				}
			}
			mu_list[z][m] = (((FloatHi) 1.0/(FloatHi)(mu_list_num[1]*mu_list_num[2]*mu_list_num[3]))*sum);
			sum = 0.0;
		}
	}

/* now substract ur, us and ud, usp from the table */

	for(j = 0; j < mu_list_num[1]; j++) { /* for each r */
		for(k = 0; k < mu_list_num[2]; k++) {  /* along each s */
			if (nvar > 2) {
				for(l = 0; l < mu_list_num[3]; l++) {  /* and d */
					if (nvar > 3) {
						for(m = 0; m <mu_list_num[4]; m++) { /* and taxon */
							pfTable[Get4DArrayValue(j,k,l,m,PIDimList->pidim1234)] += (	
								- mu_list[1][j] 
								- mu_list[2][k] 
								- mu_list[3][l] 
								- mu_list[4][m]);
						}
					} else {
						pfTable[Get3DArrayValue(j,k,l,PIDimList->pidim123)] += (
							- mu_list[1][j] 
							- mu_list[2][k] 
							- mu_list[3][l]);
					}
				}
			} else { /* get the final results for the 2D case */
				pfTable[Get2DArrayValue(j,k,PIDimList->pidim12)] += (
					- mu_list[1][j] 
					- mu_list[2][k]);
			}
		}
	}
	

/* urs */
	z++; /* z = 3,4,5 */
	for(j = 0; j < mu_list_num[1]; j++) { /* for each r (or s) */
		for(k = 0; k < mu_list_num[2]; k++) { 
			if (nvar > 2) {
				for (l = 0; l < mu_list_num[3]; l++) { /* across each d */
					if( nvar > 3) {
						for(m = 0; m <mu_list_num[4]; m++) { /* and taxon */
							sum += pfTable[Get4DArrayValue(j,k,l,m,PIDimList->pidim1234)];
						}
					} else {
						sum += pfTable[Get3DArrayValue(j,k,l,PIDimList->pidim123)];
					}
				}
			} else {
				mu_list[z][Get2DArrayValue(j,k,PIDimList->pidim12)] = pfTable[Get2DArrayValue(j,k,PIDimList->pidim12)];
			}
			if(nvar > 3) {
				mu_list[z][Get2DArrayValue(j,k,PIDimList->pidim12)] = ((FloatHi) 1.0/((FloatHi) mu_list_num[3]*(FloatHi) mu_list_num[4])*sum);
			} else {
				mu_list[z][Get2DArrayValue(j,k,PIDimList->pidim12)] = ((FloatHi) 1.0/(FloatHi) mu_list_num[3]*sum);
			}
			sum = 0.0;
		}
	}

	if ( nvar == 2 ) {
		/* get out now for 2D case */
		*pppmu_list = mu_list;
		*pmu_list_num = mu_list_num;
		*pPIDimList = PIDimList;
		return SLRI_SUCCESS;
	} 


/* urd */
	z++; /* z = 5,6 */
	for(j = 0; j < mu_list_num[1]; j++) { /* for each r  */
		for(l = 0; l < mu_list_num[3]; l++) { /* and d */
			for (k = 0; k < mu_list_num[2]; k++) { /* across each s */
				if (nvar > 3) {
					for(m = 0; m <mu_list_num[4]; m++) { /* and taxon */
						sum += pfTable[Get4DArrayValue(j,k,l,m,PIDimList->pidim1234)];
					}
				} else {
					sum += pfTable[Get3DArrayValue(j,k,l,PIDimList->pidim123)];
				}
			}
			if(nvar > 3) {
				mu_list[z][Get2DArrayValue(j,l,PIDimList->pidim13)] = ((FloatHi) 1.0/((FloatHi) mu_list_num[2]* (FloatHi) mu_list_num[4])*sum); 
			} else {
				mu_list[z][Get2DArrayValue(j,l,PIDimList->pidim13)] = ((FloatHi) 1.0/(FloatHi) mu_list_num[2]*sum); 
			}

			sum = 0.0;
		}
	}

/* usd */
	z++; /* z = 6,7 */
	for(k = 0; k < mu_list_num[2]; k++) { /* for each s */
		for(l = 0; l < mu_list_num[3]; l++) { /* and d */
			for (j = 0; j < mu_list_num[1]; j++) { /* across each r */
				if (nvar > 3) {
					for(m = 0; m <mu_list_num[4]; m++) { /* and taxon */
						sum += pfTable[Get4DArrayValue(j,k,l,m,PIDimList->pidim1234)];
					}
				} else {
					sum += pfTable[Get3DArrayValue(j,k,l,PIDimList->pidim123)];
				}
			}
			if(nvar > 3) {
				mu_list[z][Get2DArrayValue(k,l,PIDimList->pidim23)] = ((FloatHi) 1.0/((FloatHi) mu_list_num[1]* (FloatHi) mu_list_num[4])*sum); 
			} else {
				mu_list[z][Get2DArrayValue(k,l,PIDimList->pidim23)] = ((FloatHi) 1.0/(FloatHi) mu_list_num[1]*sum); 
			}
			sum = 0.0;
		}
	}

	if (nvar > 3 ) {
		/* u rt */ 
		z++; /* z = 6,8  */
		for(j = 0; j < mu_list_num[1]; j++) { /* for each r */
			for(m = 0; m < mu_list_num[4]; m++) { /* and t */
				for (k = 0; k < mu_list_num[2]; k++) { /* across each r */
					for(l = 0; l <mu_list_num[3]; l++) { /* and d */
						sum += pfTable[Get4DArrayValue(j,k,l,m,PIDimList->pidim1234)];
					}
				}
				mu_list[z][Get2DArrayValue(m,j,PIDimList->pidim14)] = ((FloatHi) 1.0/((FloatHi) mu_list_num[2]*(FloatHi) mu_list_num[3])*sum); 
				sum = 0.0;
			}
		}
		/* u st */ 
		z++; /* z = 6,9  */
		for(k = 0; k < mu_list_num[2]; k++) { /* for each r */
			for(m = 0; m < mu_list_num[4]; m++) { /* and t */
				for (j = 0; j < mu_list_num[1]; j++) { /* across each r */
					for(l = 0; l <mu_list_num[3]; l++) { /* and d */
						sum += pfTable[Get4DArrayValue(j,k,l,m,PIDimList->pidim1234)];
					}
				}
				mu_list[z][Get2DArrayValue(m,k,PIDimList->pidim24)] = ((FloatHi) 1.0/((FloatHi) mu_list_num[1]*(FloatHi)mu_list_num[3])*sum); 
				sum = 0.0;
			}
		}

		/* u dt */ 
		z++; /* z = 6,10 */
		for(l = 0; l < mu_list_num[3]; l++) { /* for each r */
			for(m = 0; m < mu_list_num[4]; m++) { /* and t */
				for (j = 0; j < mu_list_num[1]; j++) { /* across each r */
					for(k = 0; k <mu_list_num[2]; k++) { /* and d */
						sum += pfTable[Get4DArrayValue(j,k,l,m,PIDimList->pidim1234)];
					}
				}
				mu_list[z][Get2DArrayValue(m,l,PIDimList->pidim34)] = ((FloatHi) 1.0/((FloatHi) mu_list_num[1]*(FloatHi)mu_list_num[2])*sum); 
				sum = 0.0;
			}
		}

	}

/* NOW substract urs, urd and usd (and urt, ust, udt for 4D) to get ursd as the remaining table */
	for(j = 0; j < mu_list_num[1]; j++) { /* for each r */
		for(k = 0; k < mu_list_num[2]; k++) {  /* along each s */
			for(l = 0; l < mu_list_num[3]; l++) {  /* and d */
				if(nvar > 3) {
					for(m = 0; m < mu_list_num[4]; m++) { /* and t */
						pfTable[Get4DArrayValue(j,k,l,m,PIDimList->pidim1234)] += (
							- mu_list[5][Get2DArrayValue(j,k,PIDimList->pidim12)] 
							- mu_list[6][Get2DArrayValue(j,l,PIDimList->pidim13)] 
							- mu_list[7][Get2DArrayValue(k,l,PIDimList->pidim23)] 
							- mu_list[8][Get2DArrayValue(m,j,PIDimList->pidim14)] 
							- mu_list[9][Get2DArrayValue(m,k,PIDimList->pidim24)] 
							- mu_list[10][Get2DArrayValue(m,l,PIDimList->pidim34)]);
					}
				} else {
					pfTable[Get3DArrayValue(j,k,l,PIDimList->pidim123)] += (
						- mu_list[4][Get2DArrayValue(j,k,PIDimList->pidim12)] 
						- mu_list[5][Get2DArrayValue(j,l,PIDimList->pidim13)] 
						- mu_list[6][Get2DArrayValue(k,l,PIDimList->pidim23)]);
	}}}}

	z++; /* z = 7, 11 */
	for(j = 0; j < mu_list_num[1]; j++) { /* for each r */
		for(k = 0; k < mu_list_num[2]; k++) {  /* along each s */
			for(l = 0; l < mu_list_num[3]; l++) {  /* and d */
				if(nvar > 3) {
					for(m = 0; m < mu_list_num[4]; m++) { /* and t */
						mu_list[z][Get4DArrayValue(j,k,l,m,PIDimList->pidim1234)] = pfTable[Get4DArrayValue(j,k,l,m,PIDimList->pidim1234)];
					}
				} else {
					mu_list[z][Get3DArrayValue(j,k,l,PIDimList->pidim123)] = pfTable[Get3DArrayValue(j,k,l,PIDimList->pidim123)];
				}
	}}}

	if( nvar > 3) {
		/* u rst */
		z++; /* z = 12  */
		for(j = 0; j < mu_list_num[1]; j++) { /* for each r */
			for(k = 0; k < mu_list_num[2]; k++) { /* and t */
				for (m = 0; m < mu_list_num[4]; m++) { /* across each r */
					for(l = 0; l < mu_list_num[3]; l++) { /* and d */
						sum += pfTable[Get4DArrayValue(j,k,l,m,PIDimList->pidim1234)];
					}
					mu_list[z][Get3DArrayValue(j,k,m,PIDimList->pidim124)] = ((FloatHi) 1.0/(FloatHi)mu_list_num[3]*sum); 
					sum = 0.0;
		}}}
		
		
		/* u rdt */
		z++; /* z = 13  */
		for(j = 0; j < mu_list_num[1]; j++) { /* for each r */
			for(l = 0; l < mu_list_num[3]; l++) { /* and t */
				for (m = 0; m < mu_list_num[4]; m++) { /* across each r */
					for(k = 0; k < mu_list_num[2]; k++) { /* and d */
						sum += pfTable[Get4DArrayValue(j,k,l,m,PIDimList->pidim1234)];
					}
					mu_list[z][Get3DArrayValue(j,l,m,PIDimList->pidim134)] = ((FloatHi) 1.0/(FloatHi)mu_list_num[2]*sum); 
					sum = 0.0;
		}}}
		
		/* u sdt */
		z++; /* z = 14  */
		for(k = 0; k < mu_list_num[2]; k++) { /* for each r */
			for(l = 0; l < mu_list_num[3]; l++) { /* and t */
				for (m = 0; m < mu_list_num[4]; m++) { /* across each r */
					for(j = 0; j < mu_list_num[1]; j++) { /* and d */
						sum += pfTable[Get4DArrayValue(j,k,l,m,PIDimList->pidim1234)];
					}
					mu_list[z][Get3DArrayValue(k,l,m,PIDimList->pidim234)] = ((FloatHi) 1.0/(FloatHi)mu_list_num[1]*sum); 
					sum = 0.0;
		}}}
		
		/* substract terms */
		/* u rsdt */
		z++; /* z = 7, 11 */
		for(j = 0; j < mu_list_num[1]; j++) { /* for each r */
			for(k = 0; k < mu_list_num[2]; k++) {  /* along each s */
				for(l = 0; l < mu_list_num[3]; l++) {  /* and d */
					for(m = 0; m < mu_list_num[4]; m++) { /* and t */
						pfTable[Get4DArrayValue(j,k,l,m,PIDimList->pidim1234)] += (
							- mu_list[11][Get3DArrayValue(j,k,l,PIDimList->pidim123)] 
							- mu_list[12][Get3DArrayValue(j,k,m,PIDimList->pidim124)] 
							- mu_list[13][Get3DArrayValue(j,l,m,PIDimList->pidim134)] 
							- mu_list[14][Get3DArrayValue(k,l,m,PIDimList->pidim234)]);
					}
					mu_list[z][Get4DArrayValue(j,k,l,m,PIDimList->pidim1234)] = pfTable[Get4DArrayValue(j,k,l,m,PIDimList->pidim1234)];
		}}}
	}

	pfTable = MemFree(pfTable);
	*pppmu_list = mu_list;
	*pmu_list_num = mu_list_num;
	*pPIDimList = PIDimList;

	return SLRI_SUCCESS;
}


/* This function takes a list of selected (boolean array) mu terms and their array sizes, and prints to file */
SLRI_ERR SLRI_Dump3DLogLinearRegressionEx(Int4 nvar, FloatHiPtr *mu_list, Int4Ptr mu_list_num, PIDimPtr PIDimList, CharPtr **desc, FILE *fp, Boolean *mu_list_print, Boolean bNegateOutput)
{
	Int4 i = 0, j = 0, k = 0, l = 0;
	FloatHi value = 0.0;
	
	/* mu */
	if(mu_list_print[0] == TRUE) {
		value = mu_list[0][0];
		if(bNegateOutput) value = -value;
		fprintf(fp,"mu:\n%-12.9f\n", value);
	}

	/* mu1 */
	if(mu_list_print[1] == TRUE) {
		fprintf(fp,"\nmu1:\n");
		for(i = 0; i < mu_list_num[1]; i++) {
			value = mu_list[1][i];
			if(bNegateOutput) value = -value;
			fprintf(fp,"%-3s\t%-12.9f\n", desc[0][i],value);
		}
	}

	/* mu2 */
	if(mu_list_print[2] == TRUE) {
		fprintf(fp,"\nmu2:\n");
		for(i = 0; i < mu_list_num[2]; i++) {
			value = mu_list[2][i];
			if(bNegateOutput) value = -value;
			fprintf(fp,"%-3s\t%12.9f\n", desc[1][i],value);	
		}
	}

	/* mu3 */
	if(mu_list_print[3] == TRUE) {
		fprintf(fp,"\nmu3:\n");
		for(i = 0; i < mu_list_num[3]; i++) {
			value = mu_list[3][i];
			if(bNegateOutput) value = -value;
			fprintf(fp,"%-3s\t%12.9f\n", desc[2][i],value);
		}
	}

	/* mu12 */
	if(mu_list_print[4] == TRUE) {
		fprintf(fp,"\nmu12:\n\t");
		/* header */
		for(i = 0; i < mu_list_num[1]; i++) {
			fprintf(fp,"%-14s\t",desc[0][i]);
		}
		/* body */
		for(j = 0; j < mu_list_num[1]; j++) {
			i = 0;
			fprintf(fp,"\n%-3s\t", desc[1][j]);
			for(k = 0; k < mu_list_num[2]; k++) {
				value = mu_list[4][Get2DArrayValue(j,k,PIDimList->pidim12)];
				if(bNegateOutput) value = -value;
				fprintf(fp,"%12.9f\t ", (double) value);
			}
		}
	}

	/* mu 13 header */
	if(mu_list_print[5] == TRUE) {
		fprintf(fp,"\n\nmu13:\n\t");
		for(i = 0; i < mu_list_num[3]; i++) {
			fprintf(fp,"%-14s\t",desc[2][i]);
		}
		/* body */
		for(j = 0; j < mu_list_num[1]; j++) {
			fprintf(fp,"\n%3s\t", desc[0][j]);
			i = 0;
			for(l = 0; l < mu_list_num[3]; l++) {
				value = mu_list[5][Get2DArrayValue(j,l,PIDimList->pidim13)];
				if(bNegateOutput) value = -value;
				fprintf(fp,"%12.9f\t ", (double) value);
			}
		}
	}

	/* mu23 - header */
	if(mu_list_print[6] == TRUE) {
		fprintf(fp,"\n\nmu23:\n\t");
		for(i = 0; i < mu_list_num[3]; i++) {
			fprintf(fp,"%-14s\t",desc[2][i]);
		}
		/* mu23 - body */
		for(k = 0; k < mu_list_num[2]; k++) {
			fprintf(fp,"\n%3s\t", desc[1][k]);
			i = 0;
			for(l = 0; l < mu_list_num[3]; l++) {
				value = mu_list[6][Get2DArrayValue(k,l,PIDimList->pidim23)];
				if(bNegateOutput) value = -value;
				fprintf(fp,"%12.9f\t ", (double) value);
			}
		}
	}

	/* mu123 */
	if(mu_list_print[7] == TRUE) {
		fprintf(fp,"\n\nmu123:\n");
		for(l = 0; l < mu_list_num[3]; l++) {
			/* depth header */
			fprintf(fp,"\n%-4s\n\t", desc[2][l]);
			/* column header */
			for(j = 0; j < mu_list_num[1]; j++) {
				fprintf(fp,"%-14s\t", desc[0][j]);
			}
			/* row header */
			for(j = 0; j < mu_list_num[1]; j++) {
				fprintf(fp,"\n%-4s\t ", desc[0][j]);
				for(k = 0; k < mu_list_num[2]; k++) {
					value = mu_list[7][Get3DArrayValue(j,k,l,PIDimList->pidim123)];
					if(bNegateOutput) value = -value;
					fprintf(fp,"%12.9f\t ", (double) value);
				}
			}
			fprintf(fp,"\n");
		}
	}

	return SLRI_SUCCESS;
}

/* Dumps all terms from 3D Log linear regression and hierarchial modelling */
SLRI_ERR SLRI_Dump3DLogLinearRegressionWithHeader(Int4 nvar, FloatHiPtr *mu_list, Int4Ptr mu_list_num, PIDimPtr PIDimList, CharPtr **desc, FILE *fp)
{
	Int4 i;
	Boolean bNegateOutput = FALSE;
	Boolean *mu_list_print = (Boolean *) MemNew((size_t)sizeof(Boolean)*8);
	for(i = 0; i < 8; i++) {
		mu_list_print[i] = TRUE;
	}
		
	fprintf(fp,"Log linear Regression Output\n"
				"For a %ld variable table with ",(long)nvar);
	for(i = 1; i <= nvar; i++) {
		fprintf(fp,"%ld",(long) mu_list_num[i]);
		if(i < nvar) {
			fprintf(fp,"x");
		} else {
			fprintf(fp," dimensions.\n\n");
		}
	}
	return SLRI_Dump3DLogLinearRegressionEx(nvar, mu_list, mu_list_num, PIDimList, desc, fp, mu_list_print, bNegateOutput);
}


SLRI_ERR SLRI_Dump3DLogLinearRegression(Int4 nvar, FloatHiPtr *mu_list, Int4Ptr mu_list_num, PIDimPtr PIDimList, CharPtr **desc, FILE *fp)
{
	Int4 i;
	Boolean bNegateOutput = FALSE;
	Boolean *mu_list_print = (Boolean *) MemNew((size_t)sizeof(Boolean)*8);
	for(i = 0; i < 8; i++) {
		mu_list_print[i] = TRUE;
	}
	return SLRI_Dump3DLogLinearRegressionEx(nvar, mu_list, mu_list_num, PIDimList, desc, fp, mu_list_print, bNegateOutput);
}



/* Add terms u terms together to make the bryant-lawrence style potential */
/* Caller must free resulting list */
SLRI_ERR GenerateBLPotential(FloatHiPtr *mu_list, Int4Ptr mu_list_num, PIDimPtr PIDimList, FloatHiPtr *PNTR pppBL_mu_list)
{
	Int4 j,k,l;
	FloatHiPtr *BL_mu_list = NULL;


	/* copy to new list */
	BL_mu_list = (FloatHiPtr *) MemNew((size_t)sizeof(FloatHiPtr)*2);
	BL_mu_list[0] = (FloatHiPtr) MemNew((size_t)sizeof(FloatHi)*mu_list_num[5]);
	BL_mu_list[1] = (FloatHiPtr) MemNew((size_t)sizeof(FloatHi)*mu_list_num[7]);

	/* add u1 (ur) to u5 (urd) */
	for(l = 0; l < mu_list_num[3]; l++) {
		for(j = 0; j < mu_list_num[1]; j++) {
			BL_mu_list[0][Get2DArrayValue(j,l,PIDimList->pidim13)] = mu_list[5][Get2DArrayValue(j,l,PIDimList->pidim13)] += mu_list[1][j];
	}}

	/* add u4 (urs) to u7 (ursd) */
	for(l = 0; l < mu_list_num[3]; l++) { /* for each d */
		for(j = 0; j < mu_list_num[1]; j++) {
			for(k = 0; k < mu_list_num[2]; k++) {
				BL_mu_list[1][Get3DArrayValue(j,k,l,PIDimList->pidim123)] = mu_list[7][Get3DArrayValue(j,k,l,PIDimList->pidim123)] += mu_list[4][Get2DArrayValue(j,k,PIDimList->pidim12)];
	}}}

	*pppBL_mu_list = BL_mu_list;

	return SLRI_SUCCESS;
}


/* Dumps out the BL potential */
SLRI_ERR SLRI_DumpBLLogLinearRegression(FloatHiPtr *mu_list, Int4Ptr mu_list_num, PIDimPtr PIDimList, FILE *fp)
{
	Int4 i = 0, j = 0, k = 0, l = 0;
	FloatHi value = 0.0;
	CharPtr **desc = NULL;
	Int4 nvar = 3;

	
	SLRI_BLDescNew(nvar,mu_list_num, &desc);

	fprintf(fp,"PEP=TRUE\n");

	/* mu123 */
	for(l = 0; l < mu_list_num[3]; l++) {
		/* depth header */
		fprintf(fp,"\n%-4s\t", desc[2][l]);
		/* column header */
		for(j = 0; j < mu_list_num[1]; j++) {
			fprintf(fp,"%s\t\t", IsNullStr(desc[0][j]));
		}
		/* row header */
		for(j = 0; j < mu_list_num[1]; j++) {
			fprintf(fp,"\n%-4s\t ", desc[0][j]);
			for(k = 0; k < mu_list_num[2]; k++) {
				value = mu_list[1][Get3DArrayValue(j,k,l,PIDimList->pidim123)];
				fprintf(fp,"%12.9f\t ", (double) value);
			}
		}
		fprintf(fp,"\n");
	}

	/* mu 13 header */
	fprintf(fp,"\n\t");
	for(i = 0; i < mu_list_num[3]; i++) {
		fprintf(fp,"%-14s\t",desc[2][i]);
	}
	/* body */
	for(j = 0; j < mu_list_num[1]; j++) {
		fprintf(fp,"\n%3s\t", desc[0][j]);
		i = 0;
		for(l = 0; l < mu_list_num[3]; l++) {
			value = mu_list[0][Get2DArrayValue(j,l,PIDimList->pidim13)];
			fprintf(fp,"%12.9f\t ", (double) value);
		}
	}

	SLRI_BLDescFree(nvar,mu_list_num,&desc);

	return SLRI_SUCCESS;
}

/* Calculates a model score from one of 3 models ( ur, ur+urd, urs+ursd ), for use as a scoring function */
SLRI_ERR SLRI_CalculateMuScore(Int4Ptr piPotential, FloatHiPtr *mu_list, Int4Ptr mu_list_num, PIDimPtr PIDimList, FloatLo PNTR pscore, Int2 iModel)
{
	Int4 j,k,l;
	FloatLo score = 0.0;
	FloatHiPtr *BL_mu_list = NULL;

	/* ur */
	if(iModel == 1) {
		for(k = 0; k < mu_list_num[2]; k++) {
			for(l = 0; l < mu_list_num[3]; l++) {
				for(j = 0; j < mu_list_num[1]; j++) {
					score += (FloatLo) (mu_list[1][j] * piPotential[Get3DArrayValue(j,k,l,PIDimList->pidim123)]);
	}}}}

	/* ur + urd */
	if(iModel == 2) {
		BL_mu_list = (FloatHiPtr *) MemNew((size_t)sizeof(FloatHiPtr)*2);
		BL_mu_list[0] = (FloatHiPtr) MemNew((size_t)sizeof(FloatHi)*mu_list_num[5]);
		
		/* add u1 (ur) to u5 (urd) */
		for(l = 0; l < mu_list_num[3]; l++) {
			for(j = 0; j < mu_list_num[1]; j++) {
				BL_mu_list[0][Get2DArrayValue(j,l,PIDimList->pidim13)] = mu_list[5][Get2DArrayValue(j,l,PIDimList->pidim13)] += mu_list[1][j];
		}}

		for(k = 0; k < mu_list_num[2]; k++) {
			for(l = 0; l < mu_list_num[3]; l++) {
				for(j = 0; j < mu_list_num[1]; j++) {
					score += (FloatLo) (BL_mu_list[0][Get2DArrayValue(j,l,PIDimList->pidim13)] * piPotential[Get3DArrayValue(j,k,l,PIDimList->pidim123)]);
				}
			}
		}
		BL_mu_list[0] = MemFree(BL_mu_list[0]);
		BL_mu_list = MemFree(BL_mu_list);
	}
	if (iModel == 3 ) {
		/* add u4 (urs) to u7 (ursd) */
		for(l = 0; l < mu_list_num[3]; l++) { /* for each d */
			for(j = 0; j < mu_list_num[1]; j++) {
				for(k = 0; k < mu_list_num[2]; k++) {
					BL_mu_list[1][Get3DArrayValue(j,k,l,PIDimList->pidim123)] = mu_list[7][Get3DArrayValue(j,k,l,PIDimList->pidim123)] += mu_list[4][Get2DArrayValue(j,k,PIDimList->pidim12)];
		}}}

	}

	
	*pscore = -score;

	return SLRI_SUCCESS;
}

SLRI_ERR STAT_RowMinMax(FloatLoPtr pfData, Int4 iVecSize, FloatLo PNTR pfMin, FloatLo PNTR pfMax)
{
	Int4 i = 0;
	FloatLo fMin = FLT_MAX, fMax = FLT_MIN;

	if(pfData == NULL || (pfMin == NULL && pfMax == NULL)) {
		ErrPostEx(SEV_ERROR,0,0,"Invalid Parameters");
		return SLRI_FAIL;
	}
	for(i = 0; i < iVecSize; i++) {
		if(pfData[i] < fMin) fMin = pfData[i];
		if(pfData[i] > fMax) fMax = pfData[i];
	}
	if(pfMin) *pfMin = fMin;
	if(pfMax) *pfMax = fMax;
	return SLRI_SUCCESS;
}

/* Calculates the mean for a FloatLo array pfData of size iVecSize to pfmean*/
SLRI_ERR STAT_RowMean(FloatLoPtr pfData, Int4 iVecSize, FloatLo *pfmean)
{
	Int4 i = 0;
	if(pfData == NULL || iVecSize <= 0 || pfmean == NULL) {
		ErrPostEx(SEV_ERROR,0,0,"Invalid Parameters");
		return SLRI_FAIL;
	}
	*pfmean = 0.0;
	for(i = 0; i < iVecSize; i++) {
		*pfmean += pfData[i];
	}
	*pfmean /= (FloatLo) iVecSize;
	return SLRI_SUCCESS;
}

/* Calculates the standard deviation for data in the pfData array of size iVecSize with mean fmean to pfstddev */
SLRI_ERR STAT_RowStdDev(FloatLoPtr pfData, Int4 iVecSize, FloatLo fmean, FloatLo PNTR pfstddev)
{
	Int4 i = 0;
	FloatLo f = 0.0;

	if(pfData == NULL || iVecSize <= 0 || pfstddev == NULL) {
		ErrPostEx(SEV_ERROR,0,0,"Invalid Parameters");
		return SLRI_FAIL;
	}

	*pfstddev = 0.0;
	for (i = 0; i < iVecSize; i++) {
		f = pfData[i] - fmean;
		*pfstddev += f*f;
	}
	*pfstddev /= (FloatLo)iVecSize;
	*pfstddev = (FloatLo)sqrt(*pfstddev);
	return SLRI_SUCCESS;
}

/* rescales the vector by the minimum and range of the vector, to make all elements lie between 0 and 1 */
/* if specified, multiplied by fScaleFactor (cannot be zero) */
SLRI_ERR STAT_RowNormalize(FloatLoPtr pfData, Int4 iVecSize, FloatLo fScaleFactor)
{
	Int4 i = 0;
	FloatLo fMin = FLT_MAX, fMax = FLT_MIN;
	SLRI_ERR err;

	if(pfData == NULL || iVecSize <= 0) {
		ErrPostEx(SEV_ERROR,0,0,"Invalid Paramters");
		return SLRI_FAIL;
	}
	if((err = STAT_RowMinMax(pfData, iVecSize, &fMin, &fMax)) != SLRI_SUCCESS) return err;
	
	fMin = -fMin;
	fMax += fMin;

	if(fScaleFactor == 0.0) fScaleFactor = 1.0;
	for(i = 0; i < iVecSize; i++) {
		pfData[i] = ((pfData[i] + fMin) / fMax) * fScaleFactor;
	}
	return SLRI_SUCCESS;
}

SLRI_ERR STAT_ColumnMinMax(FloatLoPtr *ppfData, Int4 iVecNum, Int4 iVecSize, FloatLoPtr PNTR ppfMin, FloatLoPtr PNTR ppfMax)
{
	Int4 i = 0, j = 0;
	FloatLo fMin = FLT_MAX, fMax = FLT_MIN;
	FloatLoPtr pfMin = NULL, pfMax = NULL;

	if(ppfData == NULL || iVecNum <= 0 || iVecSize <= 0 || ppfMin == NULL || ppfMax == NULL) {
		ErrPostEx(SEV_ERROR,0,0,"Invalid Parameters");
		return SLRI_FAIL;
	}
	if(*ppfMin == NULL) {
		if((pfMin = (FloatLoPtr) MemNew((size_t)sizeof(FloatLo)*iVecSize)) == NULL) return SLRI_NOMEM;
		*ppfMin = pfMin;
	} else pfMin = *ppfMin;
	if(*ppfMax == NULL) {
		if((pfMax = (FloatLoPtr) MemNew((size_t)sizeof(FloatLo)*iVecSize)) == NULL) return SLRI_NOMEM;
		*ppfMax = pfMax;
	} else pfMax = *ppfMax;

	for(i = 0; i < iVecSize; i++) {
		fMin = FLT_MAX;
		fMax = FLT_MIN;
		for(j = 0; j < iVecNum; j++) {
			if(ppfData[j][i] < fMin) fMin = ppfData[j][i];
			if(ppfData[j][i] > fMax) fMax = ppfData[j][i];
		}
		if(fMin != FLT_MAX) pfMin[i] = fMin;
		else fMin = 0.0;
		if(fMax != FLT_MIN) pfMax[i] = fMax;
		else fMax = 0.0;
	}

	return SLRI_SUCCESS;
}

/* Calculates the mean for each column in the FloatLo matrix ppfData[iVecNum][iVecSize] to a new FloatLo array ppfmean */
SLRI_ERR STAT_ColumnMean(FloatLoPtr *ppfData, Int4 iVecNum, Int4 iVecSize, FloatLoPtr *ppfmean)
{
	Int4 i = 0, j = 0;
	FloatLoPtr pfmean = NULL;

	if(ppfData == NULL || iVecNum <= 0 || iVecSize <= 0 || ppfmean == NULL) {
		ErrPostEx(SEV_ERROR,0,0,"Invalid Parameters");
		return SLRI_FAIL;
	}
	if(*ppfmean == NULL) {
		if((pfmean = (FloatLoPtr) MemNew((size_t)sizeof(FloatLo)*iVecSize)) == NULL) return SLRI_NOMEM;
		*ppfmean = pfmean;
	} else pfmean = *ppfmean;

	for(j = 0; j < iVecSize; j++) {
		pfmean[j] = 0.0;
		for(i = 0; i < iVecNum; i++) {
			pfmean[j] += ppfData[i][j];
		}
		pfmean[j] /= (FloatLo) iVecNum;
	}
	return SLRI_SUCCESS;
}

/* Calculates the standard deviation for each column in the FloatLo matrix ppfData[iVecNum][iVecSize] to a new FloatLo array ppfstddev */
SLRI_ERR STAT_ColumnStdDev(FloatLoPtr *ppfData, Int4 iVecNum, Int4 iVecSize, FloatLoPtr pfmean, FloatLoPtr PNTR ppfstddev)
{
	Int4 i = 0, j = 0;
	FloatLo f = 0.0;
	FloatLoPtr pfstddev = NULL;

	if(ppfData == NULL || iVecNum <= 0 || iVecSize <= 0 || pfmean == NULL || ppfstddev == NULL) {
		ErrPostEx(SEV_ERROR,0,0,"Invalid Parameters");
		return SLRI_FAIL;
	}
	if(*ppfstddev == NULL) {
		if((pfstddev = (FloatLoPtr) MemNew((size_t)sizeof(FloatLo)*iVecSize)) == NULL) return SLRI_NOMEM;
		*ppfstddev = pfstddev;
	} else pfstddev = *ppfstddev;
	
	for (j = 0; j < iVecSize; j++) {
		pfstddev[j] = 0.0;
		for (i = 0; i < iVecNum; i++) {
			f = ppfData[i][j] - pfmean[j];
			pfstddev[j] += f*f;
		}
		pfstddev[j] /= (FloatLo)iVecNum;
		pfstddev[j] = (FloatLo)sqrt(pfstddev[j]);
	}
	return SLRI_SUCCESS;
}

/* Dump to file the mean and standard deviations */
SLRI_ERR STAT_DumpMeanAndStdDev(FloatLoPtr pfMean, FloatLoPtr pfStdDev, Int4 iVecSize, FILE *fp)
{
	Int4 i = 0;
	if(fp == NULL) {
		ErrPostEx(SEV_ERROR,0,0,"Invalid Parameters");
		return SLRI_FAIL;
	}

	if(pfMean) {
		fprintf(fp, "\nMeans:\t");
		for (i = 0; i < iVecSize; i++) {
			fprintf(fp,"%f\t",(float) pfMean[i]);  
		}
	}
	if(pfStdDev) {
		fprintf(fp,"\nstddev:\t");
		for (i = 0; i < iVecSize; i++) {
			fprintf(fp,"%f\t",(float) pfStdDev[i]);
		}
		fprintf(fp,"\n");
	}
	return SLRI_SUCCESS;
}


/* Center and reduce the column vectors. */
SLRI_ERR STAT_CenterAndReduce(FloatLoPtr *ppfData, Int4 iVecNum, Int4 iVecSize, FloatLoPtr pfmean, FloatLoPtr pfstddev, Boolean bReduce)
{
	Int4 i = 0, j = 0;
	FloatLo x = 0.0;

	if(ppfData == NULL || iVecNum <= 0 || iVecSize <= 0 || pfmean == NULL) {
		ErrPostEx(SEV_ERROR,0,0,"Invalid Parameters");
		return SLRI_FAIL;
	}

	for(i = 0; i < iVecNum; i++) {
		for (j = 0; j < iVecSize; j++) {
			ppfData[i][j] -= pfmean[j];
			if(bReduce) {
				x = (FloatLo)sqrt((FloatLo)iVecNum);
				/* handle near-zero std. dev. values, which below would cause a zero-divide. */
				if (pfstddev[j] <= EPS) pfstddev[j] = 1.0;
				x *= pfstddev[j];
				ppfData[i][j] /= x;
			}
		}
	}
	return SLRI_SUCCESS;
}

/* Centers data from the mean value */
SLRI_ERR STAT_Center(FloatLoPtr *ppfData, Int4 iVecNum, Int4 iVecSize, FloatLoPtr pfmean, FloatLoPtr pfstddev)
{
	return STAT_CenterAndReduce(ppfData,iVecNum,iVecSize,pfmean,pfstddev,FALSE);
}


/** The core function to init and retrieve histogram values */
SLRI_ERR STAT_Histogram(Boolean bInit, FloatLo *pstart, FloatLo *pend, FloatLo *pinterval, Int4 *pbins, FLoVectorPtr *pflvHist)
{
	static FloatLo start = FLT_MAX;
	static FloatLo end = FLT_MAX;
	static FloatLo interval = FLT_MAX;
	static FLoVectorPtr flvHist = NULL;
	static Int4 bins = 0;

	if(bInit) {
		if(pstart) start = *pstart;
		if(pend)   end = *pend;
		
		if(pinterval) interval = *pinterval;
		if(pbins)     bins = *pbins;
		if(interval != 0.0) {
			bins = (Int4) ceil((FloatHi)fabs((double) (end-start)) / (double) interval);
		} else {
			if(bins != 0) {
				interval = (FloatLo) fabs((double) (end-start)) / (FloatLo) (bins);
			} else {
				ErrPostEx(SEV_ERROR,0,0,"STAT_Histogram:You must specify either an interval or the number of bins");
				return SLRI_FAIL;
			}
		}

		if(flvHist) flvHist = NULL;
		if(pflvHist && *pflvHist == NULL) *pflvHist = FLoVectorNew(bins);
		return SLRI_SUCCESS;
	} 
	
	if(pstart) *pstart = start;
	if(pend) *pend = end;
	if(pinterval) *pinterval = interval;
	if(pbins) *pbins = bins;
	if(pflvHist && *pflvHist == NULL) *pflvHist = flvHist;

	return SLRI_SUCCESS;
}

/** Initialize a histogram */
SLRI_ERR STAT_InitHistogram(FloatLo start, FloatLo end, FloatLo interval, Int4 bins, FLoVectorPtr * flv)
{
	return STAT_Histogram(TRUE, &start,&end,&interval,&bins,&(*flv));
}

/** Add to a histogram */
SLRI_ERR STAT_AddToHistogram(FLoVectorPtr flvHist, FLoVectorPtr flvIn)
{
	FloatLo start = 0.0, end=0.0, interval = 0.0, val = 0.0;
	Int4 i = 0, pos = 0, newsize = 0, oldsize = 0, bins = 0;
	FLoVectorPtr flv = NULL;

	if(flvHist == NULL || flvHist->i == 0 || flvHist->v == NULL || flvIn == NULL || flvIn->i == 0 || flvIn->v == NULL) {
		ErrPostEx(SEV_ERROR,0,0,"STAT_AddToHistogram:Invalid Parameters");
		return SLRI_FAIL;
	}

	STAT_Histogram(FALSE, &start,&end,&interval,&bins,NULL);
	/** add the values to a new array only if start <= x <= end */
	newsize = 0;
	oldsize = flvIn->i;
	
	flv = FLoVectorNew(oldsize);
	for(i=0;i<oldsize;i++) {
		val = flvIn->v[i];
		/** only select numbers in the histogram range */
		if(val >= start && val <= end) {
			/** transform the numbers to between 0 and 1 */
			flv->v[newsize++] = (val-start)/(end-start);
		}
	}

	/** add to historgram */
	bins = flvHist->i - 1; /**< make sure to keep within array boundaries */
	for(i=0;i<newsize;i++) {
		pos = (Int4) floor(flv->v[i]*bins);
		if( (pos < 0) || (pos > (flvHist->i-1)) ) {
			ErrPostEx(SEV_ERROR,0,0,"STAT_AddToHistogram:Histogram value %f did not fall within bounds", flv->v[i]);
		}
		flvHist->v[pos] ++;
	}
	flv->i = oldsize;
	FLoVectorFree(flv);

	return SLRI_SUCCESS;
}

/** Simple dump to file */
SLRI_ERR STAT_PrintHistogram(FLoVectorPtr flvHist, FILE *fp)
{
	FloatLo start=0.0,end=0.0,interval=0.0;
	Int4 i=0, bins=0, itotal = 0;
	
	if(flvHist == NULL && fp == NULL) {
		ErrPostEx(SEV_ERROR,0,0,"STAT_PrintHistogram:Invalid Parameters");
		return SLRI_FAIL;
	}
	STAT_Histogram(FALSE,&start,&end,&interval,&bins,NULL);
	
	fprintf(fp,"Start: %f\tStop: %f\tBins: %ld\tInterval: %f\n", (float)start,(float)end,(long)bins,(float)interval);
	fprintf(fp,"Bin     \tFreq    \t\n");
	for(i=0; i<(flvHist->i); i++) {
		fprintf(fp,"%f\t%4.0f\t",(float) ((float)start + (float) fabs((double)interval*(double)i)), flvHist->v[i]);
		itotal += (Int4) flvHist->v[i];
		fprintf(fp,"\n");
	}
	fprintf(fp,"Total:  \t%ld\n",(long)itotal);

	return SLRI_SUCCESS;
}


/** Normalize the FloVector array between fMin and fMax */
SLRI_ERR STAT_NormalizeArray(FLoVectorPtr flv, FloatLo fMax, FloatLo fMin)
{
	FloatLoPtr fl = NULL;
	Int4 i = 0, start = 0, len = 0;

	if(flv == NULL || flv->v == NULL) {
		ErrPostEx(SEV_ERROR,0,0,"Invalid Parameters");
		return SLRI_FAIL;
	}

	fl = flv->v;
	start = 0;
	len = flv->i;

	/** Find the maximum and minimum if not specified */
	if(fMax == FLT_MIN) {
		for(i=start;i<len;i++) {
			if(fl[i] > fMax) fMax = fl[i];
		}
	}
	if(fMin == FLT_MAX) {
		for(i=start;i<len;i++) {
			if(fl[i] < fMin) fMin = fl[i];
		}
	}

	/** If the minimum is less than 0, add the minimum, else substract it */
	if(fMin <= 0.0) {
		for(i=0;i<len;i++) {fl[i] += (FloatLo) fabs(fMin);}
	} else {
		for(i=0;i<len;i++) {fl[i] -= fMin;}
	}

	/** Now add or substract the minimum to the maximum accordingly */
	if(fMin <= 0) {fMax += (FloatLo)fabs(fMin);}
	else fMax -= fMin;

	/** Finally, divide by the maximum */
	for(i=0;i<len;i++) {fl[i] /= fMax;}

	return SLRI_SUCCESS;
}

/** Normalize the distribution to a zero mean and unit standard deviation */
SLRI_ERR STAT_Normalize(FLoVectorPtr flv)
{
	Int4 i = 0, len = 0;
	double mean = 0.0, stdev = 0.0;
	FloatLoPtr fl = NULL;

	if(flv == NULL || flv->v == NULL || flv->i == 0) {
		ErrPostEx(SEV_ERROR,0,0,"STAT_Normalize:Invalid Parameters");
		return SLRI_FAIL;
	}
	fl = flv->v;
	for(i=0;i<len;i++) {
		mean += fl[i];
		stdev += sqrt( (double) fl[i]);
	}
	mean /= (double) len;
	stdev = sqrt( ((double)len*stdev - sqrt(mean)) / (double)(len*(len-1)) );

	for(i=0;i<len;i++) {
		fl[i] = (FloatLo) (((double)fl[i] - mean)/stdev);
	}
	return SLRI_SUCCESS;
}
/** Normalize the array between a high and low vallue */
SLRI_ERR STAT_NormalizeToInterval(FLoVectorPtr flv, FloatLo upperlimit, FloatLo lowerlimit)
{
	Int4 i = 0;
    double fMax = -FLT_MAX;
    double fMin = FLT_MAX;
    double scale = 0.0;
	FloatLoPtr fl = NULL;

    if ( flv == NULL || flv->i == 0 || flv->v == NULL || upperlimit <= lowerlimit ) {
		ErrPostEx(SEV_ERROR,0,0,"STAT_NormalizeRange:Invalid Parameters");
		return SLRI_FAIL;
	}
	fl = flv->v;
    for (i = 0; i < flv->i; i++) {
		fMax = MAX (fMax,fl[i]);
		fMin = MIN (fMin,fl[i]);
    }

    scale = (upperlimit - lowerlimit)/(fMax - fMin);
    for (i = 0; i < flv->i; i++) {
        fl[i] = (FloatLo) (((double) fl[i] - fMin) * scale + (double) lowerlimit);
	}

    return SLRI_SUCCESS;
}


/** Calculate the Z score from a flvIn and put the results int the flvOut */
SLRI_ERR STAT_CalculateZ(FloatLo fmean, FloatLo fstdev, FLoVectorPtr flvIn, FLoVectorPtr *flvOut)
{
	Int4 i = 0;
	FloatLoPtr fi = NULL, fo = NULL;

	if(flvIn == NULL || flvOut == NULL) {
		ErrPostEx(SEV_ERROR,0,0,"Invalid Parameters");
		return SLRI_FAIL;
	}
	if(*flvOut == NULL) *flvOut = FLoVectorNew(flvIn->i);

	fi = flvIn->v;
	fo = (*flvOut)->v;
	for(i=0;i<flvIn->i;i++) {
		fo[i] = (fi[i] - fmean) / fstdev;
	}
	return SLRI_SUCCESS;
}


/* Calculate the iVecSize*iVecSize correlation matrix */
SLRI_ERR STAT_CorrelationMatrix(FloatLoPtr *ppfData, Int4 iVecNum, Int4 iVecSize, FloatLoPtr PNTR *pppfCorr)
{
	Int4 i = 0, j = 0, k = 0;
	FloatLoPtr *ppfCorr = NULL;
	
	if(ppfData == NULL || iVecNum <= 0 || iVecSize <= 0 || pppfCorr == NULL) {
		ErrPostEx(SEV_ERROR,0,0,"Invalid Parameters");
		return SLRI_FAIL;
	}
	if(*pppfCorr == NULL) {
		if((ppfCorr = MATH_MatrixNew(iVecSize,iVecSize)) == NULL) return SLRI_NOMEM;
		*pppfCorr = ppfCorr;
	} else ppfCorr = *pppfCorr;


	for (j = 0; j < iVecSize-1; j++) {
		ppfCorr[j][j] = 1.0;
		for (k = j+1; k < iVecSize; k++) {
			ppfCorr[j][k] = 0.0;
			for (i = 0; i < iVecNum; i++) {
				ppfCorr[j][k] += (ppfData[i][j] * ppfData[i][k]);
			}
			ppfCorr[k][j] = ppfCorr[j][k];
		}
	}
	ppfCorr[iVecSize-1][iVecSize-1] = 1.0;
	return SLRI_SUCCESS;
}

/* Calculate the iVecSize*iVecSize covariance matrix. */
SLRI_ERR STAT_CovariationMatrix(FloatLoPtr *ppfData, Int4 iVecNum, Int4 iVecSize, FloatLoPtr PNTR *pppfCorr)
{
	Int4 i = 0, j = 0, k = 0;
	FloatLoPtr *ppfCorr = NULL;
	
	if(ppfData == NULL || iVecNum <= 0 || iVecSize <= 0 || pppfCorr == NULL) {
		ErrPostEx(SEV_ERROR,0,0,"Invalid Parameters");
		return SLRI_FAIL;
	}
	if(*pppfCorr == NULL) {
		if((ppfCorr = MATH_MatrixNew(iVecSize,iVecSize)) == NULL) return SLRI_NOMEM;
		*pppfCorr = ppfCorr;
	} else ppfCorr = *pppfCorr;


	for (j = 0; j < iVecSize; j++) {
		for (k = j; k < iVecSize; k++) {
			ppfCorr[j][k] = 0.0;
			for (i = 0; i < iVecNum; i++) {
				ppfCorr[j][k] += (ppfData[i][j] * ppfData[i][k]);
			}
			ppfCorr[k][j] = ppfCorr[j][k];
		}
	}
	return SLRI_SUCCESS;
}

/* Calculate the iVecSize*iVecSize sum of squares and cross product matrix. */
SLRI_ERR STAT_SSACPMatrix(FloatLoPtr *ppfData, Int4 iVecNum, Int4 iVecSize, FloatLoPtr PNTR *pppfSSACP)
{
	Int4 i = 0, j = 0, k = 0;
	FloatLoPtr *ppfSSACP = NULL;
	
	if(ppfData == NULL || iVecNum <= 0 || iVecSize <= 0 || pppfSSACP == NULL) {
		ErrPostEx(SEV_ERROR,0,0,"Invalid Parameters");
		return SLRI_FAIL;
	}
	if(*pppfSSACP == NULL) {
		if((ppfSSACP = MATH_MatrixNew(iVecSize,iVecSize)) == NULL) return SLRI_NOMEM;
		*pppfSSACP = ppfSSACP;
	} else ppfSSACP = *pppfSSACP;

	for(j = 0; j < iVecSize; j++) {
		for(k = j; k < iVecSize; k++) {
			ppfSSACP[j][k] = 0.0;
			for(i = 0; i < iVecNum; i++) {
				ppfSSACP[j][k] += (ppfData[i][j] * ppfData[i][k]);
			}
			ppfSSACP[k][j] = ppfSSACP[j][k];
		}
	}
	return SLRI_SUCCESS;
}

/* Write to file the symmetric matrix */
SLRI_ERR STAT_DumpSymmetricMatrix(FloatLoPtr *ppfMatrix, Int4 iVecSize, FILE *fp)
{
	Int4 i = 0, j = 0;

	fprintf(fp,"\nSymmetricMatrix:\n");
	for(i = 0; i < iVecSize; i++) {
		for(j = 0; j < iVecSize; j++) {
			fprintf(fp,"%f\t",(float) ppfMatrix[i][j]);
		}
		fprintf(fp,"\n");
	}
	fprintf(fp,"\n");
	return SLRI_SUCCESS;
}

/* Generates a correlation matrix */
SLRI_ERR PCA_GenerateCorrelationMatrix(FloatLoPtr *ppfData, Int4 iVecNum, Int4 iVecSize, FloatLoPtr PNTR *pppfCorr, FILE *fp)
{
	FloatLoPtr pfmean = NULL, pfstddev = NULL;
	SLRI_ERR err;

	if((err = STAT_ColumnMean(ppfData,iVecNum,iVecSize,&pfmean)) != SLRI_SUCCESS) return err;
	if((err = STAT_ColumnStdDev(ppfData,iVecNum,iVecSize,pfmean,&pfstddev)) != SLRI_SUCCESS) return err;
	if((err = STAT_CenterAndReduce(ppfData,iVecNum,iVecSize,pfmean,pfstddev,TRUE)) != SLRI_SUCCESS) return err;
	if((err = STAT_CorrelationMatrix(ppfData,iVecNum,iVecSize,&(*pppfCorr))) != SLRI_SUCCESS) return err;

	if(fp) STAT_DumpMeanAndStdDev(pfmean,pfstddev,iVecSize,fp);
	MATH_VectorFree(&pfmean);
	MATH_VectorFree(&pfstddev);
	
	return SLRI_SUCCESS;
}

/* Generates a covariance matrix */
SLRI_ERR PCA_GenerateCovarianceMatrix(FloatLoPtr *ppfData, Int4 iVecNum, Int4 iVecSize, FloatLoPtr PNTR *pppfCov, FILE *fp)
{
	FloatLoPtr pfmean = NULL;
	SLRI_ERR err;
	if((err = STAT_ColumnMean(ppfData,iVecNum,iVecSize,&pfmean)) != SLRI_SUCCESS) return err;
	if((err = STAT_Center(ppfData,iVecNum,iVecSize,pfmean,NULL)) != SLRI_SUCCESS) return err;
	if((err = STAT_CovariationMatrix(ppfData,iVecNum,iVecSize,&(*pppfCov))) != SLRI_SUCCESS) return err;
	if(fp) STAT_DumpMeanAndStdDev(pfmean,NULL,iVecSize,fp);
	MATH_VectorFree(&pfmean);
	return SLRI_SUCCESS;
}

/* Generates a Sums-of-squares-and-cross-products matrix */
SLRI_ERR PCA_GenerateSSACP(FloatLoPtr *ppfData, Int4 iVecNum, Int4 iVecSize, FloatLoPtr PNTR *pppfSSACP)
{
	return STAT_SSACPMatrix(ppfData,iVecNum,iVecSize,&(*pppfSSACP));
}



/* Reduce a real symmetric matrix to a symmetric tridiagonal matrix 
   Algorithm: Martin et al., Num. Math. 11, 181-195, 1968.
   Ref: Smith et al., Matrix Eigensystem Routines -- EISPACK Guide
        Springer-Verlag, 1976, pp. 489-494.
        W H Press et al., Numerical Recipes in C, Cambridge U P,
        1988, pp. 373-374.  
*/
void PCA_TDR(FloatLoPtr *ppfSymm, Int4 iVecSize, FloatLoPtr PNTR ppfEigen, FloatLoPtr PNTR ppfInter, FloatLoPtr PNTR *pppfSTM)
{
	Int4 l, k, j, i;
	FloatLo scale, hh, h, g, f;
	FloatLoPtr pfEigen = NULL, pfInter = NULL, *ppfSTM = NULL;

	if(*ppfEigen == NULL) {
		pfEigen = MATH_VectorNew(iVecSize);    /* Storage alloc. for vector of eigenvalues */
		*ppfEigen = pfEigen;
	} else pfEigen = *ppfEigen;
	
	if(*ppfInter == NULL) {
		pfInter = MATH_VectorNew(iVecSize);    /* Storage alloc. for 'intermediate' vector */
		*ppfInter = pfInter;
	} else pfInter = *ppfInter;
	
	if(*pppfSTM == NULL) {
		ppfSTM = MATH_MatrixNew(iVecSize, iVecSize);  /* Duplicate of correlation (etc.) matrix */
		*pppfSTM = ppfSTM;
	} else ppfSTM = *pppfSTM;

	for (i = 0; i < iVecSize; i++) {
		for (j = 0; j < iVecSize; j++) {
			ppfSTM[i][j] = ppfSymm[i][j];
		}
	}

	pfEigen = MATH_C2FVector(pfEigen);
	pfInter = MATH_C2FVector(pfInter);
	ppfSTM  = MATH_C2FMatrix(ppfSTM,iVecSize);
	
	for (i = iVecSize; i >= 2; i--) {
		l = i - 1;
		h = scale = 0.0;
		if (l > 1) {
			for (k = 1; k <= l; k++) {
				scale += (FloatLo)fabs(ppfSTM[i][k]);
			}
			if (scale == 0.0) {
				pfInter[i] = ppfSTM[i][l];
			} else {
				for (k = 1; k <= l; k++) {
					ppfSTM[i][k] /= scale;
					h += ppfSTM[i][k] * ppfSTM[i][k];
				}
				f = ppfSTM[i][l];
				g = (FloatLo) (f>0 ? -sqrt(h) : sqrt(h));
				pfInter[i] = scale * g;
				h -= f * g;
				ppfSTM[i][l] = f - g;
				f = 0.0;
				for (j = 1; j <= l; j++) {
					ppfSTM[j][i] = ppfSTM[i][j]/h;
					g = 0.0;
					for (k = 1; k <= j; k++) {
						g += ppfSTM[j][k] * ppfSTM[i][k];
					}
					for (k = j+1; k <= l; k++) {
						g += ppfSTM[k][j] * ppfSTM[i][k];
					}
					pfInter[j] = g / h;
					f += pfInter[j] * ppfSTM[i][j];
				}
				hh = f / (h + h);
				for (j = 1; j <= l; j++) {
					f = ppfSTM[i][j];
					pfInter[j] = g = pfInter[j] - hh * f;
					for (k = 1; k <= j; k++) {
						ppfSTM[j][k] -= (f * pfInter[k] + g * ppfSTM[i][k]);
					}
				}
			}
		} else {
			pfInter[i] = ppfSTM[i][l];
		}
		pfEigen[i] = h;
	}
	pfEigen[1] = 0.0;
	pfInter[1] = 0.0;
	for (i = 1; i <= iVecSize; i++) {
		l = i - 1;
		if (pfEigen[i]) {
			for (j = 1; j <= l; j++) {
				g = 0.0;
				for (k = 1; k <= l; k++) {
					g += ppfSTM[i][k] * ppfSTM[k][j];
				}
				for (k = 1; k <= l; k++) {
					ppfSTM[k][j] -= g * ppfSTM[k][i];
				}
			}
		}
		pfEigen[i] = ppfSTM[i][i];
		ppfSTM[i][i] = 1.0;
		for (j = 1; j <= l; j++) {
			ppfSTM[j][i] = ppfSTM[i][j] = 0.0;
		}
	}
	pfEigen = MATH_F2CVector(pfEigen);
	pfInter = MATH_F2CVector(pfInter);
	ppfSTM = MATH_F2CMatrix(ppfSTM,iVecSize);
}

/*  Tridiagonal QL algorithm */
void PCA_TDQL(FloatLoPtr *ppfSTM, Int4 iVecSize, FloatLoPtr pfEigen, FloatLoPtr pfInter)
{
	Int4 m, l, iter, i, k;
	FloatLo s, r, p, g, f, dd, c, b;

	
	pfEigen = MATH_C2FVector(pfEigen);
	pfInter = MATH_C2FVector(pfInter);
	ppfSTM = MATH_C2FMatrix(ppfSTM,iVecSize);

	for (i = 2; i <= iVecSize; i++) {
		pfInter[i-1] = pfInter[i];
	}
	pfInter[iVecSize] = 0.0;
	for (l = 1; l <= iVecSize; l++) {
		iter = 0;
		do {
			for (m = l; m <= iVecSize-1; m++) {
				dd = (FloatLo) (fabs(pfEigen[m]) + fabs(pfEigen[m+1]));
				if (fabs(pfInter[m]) + dd == dd) {
					break;
				}
			}
			if (m != l) {
				if (iter++ == 30) {
					ErrPostEx(SEV_ERROR,0,0,"No convergence in TLQI.");
					pfEigen = MATH_F2CVector(pfEigen);
					pfInter = MATH_F2CVector(pfInter);
					ppfSTM = MATH_F2CMatrix(ppfSTM,iVecSize);
					return;
				}
				g = (FloatLo) (pfEigen[l+1] - pfEigen[l]) / (2.0F * pfInter[l]);
				r = (FloatLo) sqrt((g * g) + 1.0F);
				g = (FloatLo) (pfEigen[m] - pfEigen[l] + pfInter[l] / (g + ASIGN(r, g)));
				s = c = 1.0F;
				p = 0.0F;
				for (i = m-1; i >= l; i--) {
					f = s * pfInter[i];
					b = c * pfInter[i];
					if (fabs(f) >= fabs(g)) {
						c = g / f;
						r = (FloatLo)sqrt((c * c) + 1.0);
						pfInter[i+1] = f * r;
						c *= (FloatLo)(s = 1.0F/r);
					} else {
						s = f / g;
						r = (FloatLo)sqrt((s * s) + 1.0F);
						pfInter[i+1] = g * r;
						s *= (c = 1.0F/r);
					}
					g = pfEigen[i+1] - p;
					r = (pfEigen[i] - g) * s + 2.0F * c * b;
					p = s * r;
					pfEigen[i+1] = g + p;
					g = c * r - b;
					for (k = 1; k <= iVecSize; k++) {
						f = ppfSTM[k][i+1];
						ppfSTM[k][i+1] = s * ppfSTM[k][i] + c * f;
						ppfSTM[k][i] = c * ppfSTM[k][i] - s * f;
					}
				}
				pfEigen[l] = pfEigen[l] - p;
				pfInter[l] = g;
				pfInter[m] = 0.0;
			}
		}  while (m != l);
	}
	pfEigen = MATH_F2CVector(pfEigen);
	pfInter = MATH_F2CVector(pfInter);
	ppfSTM = MATH_F2CMatrix(ppfSTM,iVecSize);
}

/* Dump the Eigenvalues and Eigenvectors to file */
SLRI_ERR PCA_DumpEigen(FloatLoPtr *ppfSymm, FloatLoPtr pfEigen, Int4 iVecSize, Int4 iProj, FILE *fp)
{
	Int4 i = 0, j = 0;

	if(ppfSymm == NULL || pfEigen == NULL || iVecSize <= 0 || fp == NULL) {
		ErrPostEx(SEV_ERROR,0,0,"Invalid Parameters");
		return SLRI_SUCCESS;
	}

	fprintf(fp,"\nEigenvalues:\t(%% of total)\n");
	for (j = iVecSize-1; j >= 0; j--) {
		fprintf(fp,"%7.4f\t%7.2f\n", pfEigen[j], pfEigen[j] / (float) iVecSize * 100.0F); 
	}
	fprintf(fp,"\n(Eigenvalues should be strictly positive; limited\n");
	fprintf(fp,"precision machine arithmetic may affect this.\n");
	fprintf(fp,"Eigenvalues are often expressed as cumulative\n");
	fprintf(fp,"percentages, representing the 'percentage variance\n");
	fprintf(fp,"explained' by the associated axis or principal component.)\n");

	fprintf(fp,"\nEigenvectors:\n");
	fprintf(fp,"(All their definition in terms of original vectors.)\n");
	for (j = 0; j < iVecSize; j++) {
		for (i = 0; i < iProj; i++)  {
			fprintf(fp,"%f\t", ppfSymm[j][iVecSize-i-1]);  
		}
		fprintf(fp,"\n");
	}
	return SLRI_SUCCESS;
}

/* Form projections of row-points on principal components */
SLRI_ERR PCA_FormRowProjections(FloatLoPtr *ppfData, FloatLoPtr *ppfSTM, FloatLoPtr pfInter, Int4 iVecNum, Int4 iVecSize, FloatLoPtr PNTR *pppfRowProj)
{
	Int4 i = 0, j = 0, k = 0, l = 0;
	FloatLoPtr *ppfRowProj = NULL;

	if(ppfData == NULL || ppfSTM == NULL || pfInter == NULL || pppfRowProj == NULL) {
		ErrPostEx(SEV_ERROR,0,0,"Invalid Parameters");
		return SLRI_FAIL;
	}

	if(*pppfRowProj == NULL) {
		ppfRowProj = MATH_MatrixNew(iVecNum, iVecSize);
		*pppfRowProj = ppfRowProj;
	} else ppfRowProj = *pppfRowProj;

	/* Form projections of row-points on first three prin. components. */
	/* Store in 'data', overwriting original data. */
	for (i = 0; i < iVecNum; i++) {
		for (j = 0; j < iVecSize; j++) {
			pfInter[j] = ppfData[i][j]; 
		}   /* ppfdata[i][j] will be overwritten */
		for (k = 0; k < iVecSize; k++) {
			ppfRowProj[i][k] = 0.0;
			for (l = 0; l < iVecSize; l++) {
				ppfRowProj[i][k] += pfInter[l] * ppfSTM[l][iVecSize-k-1]; 
			}
		}
	}
	return SLRI_SUCCESS;
}

/* Form projecdtions of column-points on principal components */
SLRI_ERR PCA_FormColProjections(FloatLoPtr *ppfSymm, FloatLoPtr *ppfSTM, FloatLoPtr pfEigen, FloatLoPtr pfInter, Int4 iVecNum, Int4 iVecSize, FloatLoPtr PNTR *pppfColProj)
{
	Int4 i = 0, j = 0, k = 0, l = 0;
	FloatLoPtr *ppfColProj = NULL;

	if(ppfSymm == NULL || ppfSTM == NULL || pfEigen == NULL || pfInter == NULL || iVecNum <= 0 || iVecSize <= 0 || pppfColProj == NULL) {
		ErrPostEx(SEV_ERROR,0,0,"Invalid Parameters");
		return SLRI_FAIL;
	}

	if(*pppfColProj == NULL) {
		ppfColProj = MATH_MatrixNew(iVecSize, iVecSize);
		*pppfColProj = ppfColProj;
	} else ppfColProj = *pppfColProj;


	for (j = 0; j < iVecSize; j++) {
		for (k = 0; k < iVecSize; k++) {
			pfInter[k] = ppfSymm[j][k]; 
		}
		for (i = 0; i < iVecSize; i++) {
			ppfSymm[j][i] = 0.0;
			for (l = 0; l < iVecSize;l++) {
				ppfColProj[j][i] += pfInter[l] * ppfSTM[l][iVecSize-i-1]; 
			}
			if (pfEigen[iVecSize-i-1] > 0.0005)  { /* Guard against zero eigenvalue */
				ppfColProj[j][i] /= (FloatLo)sqrt(pfEigen[iVecSize-i-1]);   /* Rescale */
			} else {
				ppfColProj[j][i] = 0.0;    /* Standard kludge */
			}
		}
	}
	return SLRI_SUCCESS;
}

/* Dumps the Projects to file */
SLRI_ERR PCA_DumpProjections(FloatLoPtr *ppfColProj, CharPtr *ppcColLabels, FloatLoPtr *ppfRowProj, CharPtr *ppcRowLabels, Int4 iVecNum, Int4 iVecSize, Int4 iProj, FILE *fp)
{
	Int4 i = 0, j = 0;

	if(ppfRowProj == NULL || ppfColProj == NULL || iVecNum <= 0 || iVecSize <= 0 || iProj <= 0 || fp == NULL) {
		ErrPostEx(SEV_ERROR,0,0,"Invalid Parameters");
		return SLRI_FAIL;
	}
	fprintf(fp,"\nProjections of row-points on %ld prin. comps.:\n",(long)iProj);
	fprintf(fp,"\t"); 
	for(i = 0; i < iProj; i++) {
		fprintf(fp,"%ld\t",(long) i);
	}
	fprintf(fp,"\n"); 
	for (i = 0; i < iVecNum; i++) {
		fprintf(fp,"%s\t",IsNullStr(ppcRowLabels[i]));
		for (j = 0; j < iProj; j++)  {
			fprintf(fp,"%f\t", (float) ppfRowProj[i][j]);  
		}
		fprintf(fp,"\n");  
	}

	fprintf(fp,"\nProjections of column-points on %ld prin. comps.:\n", (long) iProj);
	fprintf(fp,"\t"); 
	for(i = 0; i < iProj; i++) {
		fprintf(fp,"%ld\t",(long) i);
	}
	fprintf(fp,"\n"); 
	for (i = 0; i < iVecSize; i++) {
		fprintf(fp,"%s\t",IsNullStr(ppcColLabels[i]));
		for (j = 0; j < iProj; j++)  {
			fprintf(fp,"%f\t", (float) ppfColProj[i][j]);  
		}
		fprintf(fp,"\n");  
	}
	return SLRI_SUCCESS;
}

/*
$Log: slri_stat.c,v $
Revision 1.8  2003/11/05 17:47:26  michel
replaced max/min with macros MAX/MIN

Revision 1.7  2003/11/03 15:09:18  michel
minor changes

Revision 1.6  2003/10/26 15:24:54  feldman
Fixed QNX specific issue

Revision 1.5  2003/10/20 19:37:33  michel
Added 2 more functions to work with FLoVector data - to normalize & to calculate Zscore

Revision 1.4  2003/10/20 19:29:48  michel
Added histogram functionality

Revision 1.3  2003/10/16 20:59:50  skuznets

1. Checking for NULL pointers when print values
2. Added function Misc_TrimSpacesAroundString
3. Added macro IsNullStr

Revision 1.2  2003/09/22 14:42:17  feldman
Fixed typo

Revision 1.1.1.10  2003/07/16 08:54:02  root
Automated import from Sourceforge.net

Revision 1.10  2003/07/15 21:28:27  micheld
minor change

Revision 1.9  2003/03/19 21:48:13  micheld
minor change

Revision 1.8  2002/10/20 17:15:42  micheld
Added VNP<->FloatLoArray and stat functions

Revision 1.7  2002/07/18 23:22:31  micheld
Added Principal Components Analysis and supporting functions

Revision 1.6  2002/07/03 23:10:02  micheld
Added CalculateMuScore to slri lib

Revision 1.5  2001/12/07 16:56:42  micheld
added extra parameters for computelikelyhoodratios

Revision 1.4  2001/11/29 21:29:16  micheld
minor change

Revision 1.3  2001/11/20 20:35:16  micheld
fixed compiler warnings

Revision 1.2  2001/11/19 21:40:05  micheld
Added extra functions, made some progress towards 4D loglin regression
(incomplete)

Revision 1.1  2001/11/09 22:04:31  micheld
Added Log linear regression and hierarchial log linear modelling

*/

