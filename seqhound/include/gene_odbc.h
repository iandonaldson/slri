/* $Id: gene_odbc.h,v 1.1 2005/04/26 18:55:10 eburgess Exp $
  Copyright (C) 2000 Mount Sinai Hospital (MSH)

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of
  the License, or any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
xxxx
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


  AUTHORS:
           Elizabeth Burgess and Christopher W.V. Hogue hogue@mshri.on.ca


  REVISION/CONTRIBUTION NOTES:

  January 17, 2005  Original MSH Revision.
                    Hogue Lab - University of Toronto Biochemistry
                    Department and the Samuel Lunenfeld Research
                    Institute, Mount Sinai Hospital
                    http://bioinfo.mshri.on.ca  hogue@mshri.on.ca
                    ftp://ftp.mshri.on.ca/


  PUBLICATION TO CITE:


  DESCRIPTION
  header file for gene calls

******************************************************************************************/
#ifndef _GENE_ODBC_H_
#define _GENE_ODBC_H_

#include <intrez_odbc.h>
#include <intrez_cfg.h>
#include <intrez_misc.h>
#include <objslristruc.h>
#include <slri_odbc.h>

#ifdef __cplusplus
extern "C" { /* } */
#endif

/*****************************************************************
  SQL variables and handles.
***************************************************************/
/* GENEOBJECT handles */
static SH_IntParam  geneobject_id;
static SH_IntParam  geneobject_taxid;
static SH_IntParam  geneobject_geneid;
static SH_CharParam geneobject_status;

/* GENEGI handles */
static SH_IntParam  genegi_id;
  static SH_IntParam  genegi_gi;
static SH_IntParam  genegi_geneobjectid;
static SH_CharParam genegi_moleculetype;

/* GENEDBXREF handles */
static SH_IntParam  genedbxref_id;
static SH_IntParam  genedbxref_geneobjectid;
static SH_CharParam genedbxref_dbname;
static SH_CharParam genedbxref_dbaccess;

/* GENEINFO handles */
static SH_CharParam geneinfo_location;

/* Handles */
static SQLHANDLE HNDLGeneSearchDbxrefByGi;
static SQLHANDLE HNDLGeneSearchGiByDbxref;
static SQLHANDLE HNDLGeneSearchGiByGeneid;
static SQLHANDLE HNDLGeneSearchGeneidByDbxref;
static SQLHANDLE HNDLGeneSearchDbxrefByGeneid;
static SQLHANDLE HNDLGeneSearchGeneidByGi;
static SQLHANDLE HNDLGeneSearchLocusByGi;

/* Function prototypes */
Boolean  LIBCALL InitGENE(void);

ValNodePtr LIBCALL GetGENEDbxrefFromGi( Int4 gi, CharPtr name );
ValNodePtr LIBCALL GetGiFromGENEDbxref( CharPtr name, 
										CharPtr access );
ValNodePtr LIBCALL GetGiFromGeneid( Int4 geneid );
ValNodePtr LIBCALL GetGeneidFromGENEDbxref( CharPtr name, 
											CharPtr access );
ValNodePtr LIBCALL GetGENEDbxrefFromGeneid( Int4 geneid, 
											CharPtr name );
Int4       LIBCALL GetGeneidFromGi( Int4 gi );
CharPtr    LIBCALL GetLocusFromGi( Int4 gi );


#ifdef __cplusplus
 }
#endif

#endif /*  _GENE_ODBC_H_  defined*/
