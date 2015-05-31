/* $Id: slri_cluster.h,v 1.4 2004/02/02 20:53:47 mjdumont Exp $
-- ***************************************************************************
--
--  Samuel Lunenfeld Research Institute (SLRI)
--
--  @file slri_cluster.h Set of Data Clustering Functions
--
--  Original concept and detail coding obtained From P. Kleiweg ©1997-2002
--  http://odur.let.rug.nl/~kleiweg/indexs.html (all code released as GNU GPL)
--  
--  Functions were changed to be modular and suitable for library use 
--    Changed Data structures, modified dendrogram output
--
--  @author Michel Dumontier <micheld@mshri.on.ca>
--  @date July 2002, December 2003
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

#ifndef _SLRI_CLUSTER_
#define _SLRI_CLUSTER_

#include <slri_misc.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup slri_cluster */

/***********************/
/* Vector FILE reading */
/***********************/
/** Read in a vector file to generate differences/clusters/tree */
SLRI_ERR CLUSTER_ReadFile(CharPtr pcFileName, FloatLoPtr **pf, CharPtr **pcColLabels, CharPtr **pcRowLabels, Int4 *iRowNum, Int4 *iColNum);
/** Read in a labeled matrix file */
SLRI_ERR CLUSTER_ReadBlockFile(CharPtr pcFileName, FloatLoPtr **pf, CharPtr **pcColLabels, CharPtr **pcRowLabels, Int4 *iRowNum, Int4 *iColNum);

/** Free the read file */
void CLUSTER_ReadFileFree(FloatLoPtr **pf, CharPtr **pcColLabels, CharPtr **pcRowLabels, Int4 iRowNum, Int4 iColNum);

/**************************/
/* Difference Calculation */
/**************************/
/** Difference Methods */
typedef enum  {
	CLUSTER_METRIC_SQUAREDIFF, /**< Sum of the square of the difference */
	CLUSTER_METRIC_EUCLID,     /**< Square root of the Sum of the square of the difference */
	CLUSTER_METRIC_MANHATTAN,  /**< Absolute difference */
	CLUSTER_METRIC_PEARSON,    /**< Pearson correlation coefficient */
	CLUSTER_METRIC_BRAYCURTIS, /**< Bray-Curtis method */
	CLUSTER_METRIC_CANBERRA,   /**< Canberra method */
  	/* pseudo-binary comparisons */
	CLUSTER_METRIC_SIMPLEMATCHING,
	CLUSTER_METRIC_JACCARD,
	CLUSTER_METRIC_SORENSEN,
	CLUSTER_METRIC_BARONIURBANI
} CLUSTER_METRIC;

/* defined in .c file
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
*/

/** Get the metric name - do not free */
CharPtr CLUSTER_GetMetricName(CLUSTER_METRIC metric);



/** Calculates the difference by given metric in 2D array ppfMatrix 
 *  Given an FloatLo Array Array of iVecNum size with iVecSize arrays 
 */
SLRI_ERR CLUSTER_Diff (CLUSTER_METRIC metric, FloatLoPtr *ppf, Int4 iVecNum, Int4 iVecSize, FloatLoPtr PNTR *ppfMatrix);
/** Read/Dump the difference calculation to file */
SLRI_ERR CLUSTER_ReadDiff(CharPtr pcFileName, FloatLoPtr **pppfMatrix, CharPtr **pppcLabels, Int4 *piVecNum);
SLRI_ERR CLUSTER_DumpDiff(CLUSTER_METRIC metric, FloatLoPtr *ppfMatrix, CharPtr *ppcLabels, Int4 iVecNum, CharPtr pcFileName);


/************************/
/* CLUSTERING           */
/************************/

/** Clustering Methods */
typedef enum {
	CLUSTER_METHOD_SL, /**< Single Linkage (nearest neighbour) */
	CLUSTER_METHOD_CL, /**< Complete Linkage (furthest neighbour) */
	CLUSTER_METHOD_GA, /**< Group Average */
	CLUSTER_METHOD_WA, /**< Weighted Average */
	CLUSTER_METHOD_UC, /**< Unweighted Centroid */
	CLUSTER_METHOD_WC, /**< Weighted Centroid */
	CLUSTER_METHOD_WM  /**< Ward's Method (Minimum Variance) */
} CLUSTER_METHOD;

/* defined in .c file
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
*/

/** Get the Method name */
CharPtr CLUSTER_GetMethodName(CLUSTER_METHOD method);

/** Node Type */
typedef enum {  
	NODETYPE_CLUSTER,  /**< cluster */
	NODETYPE_LEAF      /**< leaf */
} NODETYPE;

/** labels only given to nodes */
typedef struct
{
	Int4 id;           /**< node/cluster id */
	CharPtr label;     /**< node label */
} NODEID;

/** Cluster data structure */
typedef struct
{
	Int4 id;           /**< node/cluster id */
	Int4 head;         /**< node for which cluster */
	Int4 nodes;        /**< number of nodes clustered */
	Int4 group[2];     /**< color group */
	NODETYPE ntype[2]; /**< type of node */
	NODEID n[2];       /**< node info */
	FloatLo diff;      /**< cluster difference */
} CLUSTER, *CLUSTERPtr;

/** Generates a cluster 'tree'
 *  Generates a cluster 'tree' of size iVecNum-1 that incorporates ppfMatrix and ppcLabel values given a FloatLoPtr 2D Array of iVecNumxiVecNum size 
 */
SLRI_ERR CLUSTER_Cluster(CLUSTER_METHOD method, FloatLoPtr *ppfMatrix, CharPtr *ppcLabels, Int4 iVecNum, CLUSTER **pcl);
/** Dump the cluster to file */
SLRI_ERR CLUSTER_DumpCluster(CLUSTER_METHOD method, CLUSTER *cl, Int4 iVecNum, CharPtr pcFileName);


/************************/
/* FORMATING            */
/************************/
/** Tree format */
typedef enum {
	CLUSTER_FORMAT_PS,   /**< PostScript format */
	CLUSTER_FORMAT_HTML  /**< HTML Format - not yet implemented */
} CLUSTER_FORMAT;

/* Link type */
typedef enum { 
	LINKTYPE_REC,        /**< Rectangle link */
	LINKTYPE_TRI,        /**< Triangle link */
	LINKTYPE_ARC         /**< Arc link */
} LINKTYPE;

/** Font type */
typedef enum {
	FONTNAME_HELVETICA,  /**< Helvetica font */
	FONTNAME_TIMES       /**< Times font */
} FONTNAME;

/** Cluster tree options */
typedef struct
{
	CharPtr fontname;    /**< font used */
	Int4Ptr fontwidths;  /**< Font widths */
	Int4 fontsize;       /**< Font Size */
	LINKTYPE linktype;   /**< link type */
	FloatLo exponent;    /**< exponent */
	Boolean bLabels;     /**< Draw Labels */
	Boolean bRuler;      /**< Draw Ruler  */
	Boolean bColorLabel; /**< Color the labels */
	Boolean bColorLink;  /**< Color the cluster lines */
	Int4 groups;         /**< Number of color groups */
	Boolean bPatterns;   /**< */
	Boolean b2Column;    /**< place labels in 2 columns */
	FILE *fp;            /**< File Pointer */
	Int4 iMinimum;       /**< minimum */
} CLUSTER_TREE_OPTIONS;

/** Set the tree options before calling CLUSTER_Tree */
SLRI_ERR CLUSTER_SetDendrogramOptions(CLUSTER_TREE_OPTIONS PNTR *ppOptions, FONTNAME fontname, Int4 fontsize,
			LINKTYPE linktype, FloatLo fexponent, Boolean bLabels, Boolean bRuler, Boolean bColorLabel,
			Boolean bColorLink, Int4 ColorGroups, Boolean bPatterns, Boolean b2Column, Int4 iMinimum,
			CharPtr pcFileName);

#define DEFAULT_DENDROGRAM_FILE "mytree.ps"
/** Dendrogram output */
SLRI_ERR CLUSTER_Dendrogram(CLUSTER *cl, Int4 iVecNum, CLUSTER_TREE_OPTIONS *pOptions);


#ifdef __cplusplus
}
#endif
#endif		/* _SLRI_CLUSTER_ */

/*
$Log: slri_cluster.h,v $
Revision 1.4  2004/02/02 20:53:47  mjdumont
Added read block file

Revision 1.3  2003/11/18 23:44:30  michel
Added read from difference file for clustering

Revision 1.2  2003/11/06 15:40:42  michel
Removed meaningless type qualifier

Revision 1.1.1.7  2003/03/22 09:31:41  root
Automated import from Sourceforge.net

Revision 1.8  2003/03/22 05:43:30  micheld
removed comment within comment

Revision 1.7  2003/03/19 21:49:51  micheld
Removed constants from header.  Formatted comments to JavaDoc-like

Revision 1.6  2002/09/03 22:08:33  micheld
Fixed compiler warning

Revision 1.5  2002/08/15 04:05:58  micheld
Used defined variable in function

Revision 1.4  2002/07/30 19:17:35  micheld
Fixed multiple definitions

Revision 1.3  2002/07/30 15:19:50  micheld
added function to return metric and method names

Revision 1.2  2002/07/18 23:22:31  micheld
Added Principal Components Analysis and supporting functions

Revision 1.1  2002/07/04 17:56:22  micheld
Initial import of clustering library

*/



