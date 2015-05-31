/*   $Id: taxdb_db_p.h,v 1.1.1.1 2001/10/11 17:37:13 gbader Exp $
-- ***************************************************************************
  Copyright (C) 2000 Mount Sinai Hospital (MSH)

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


  PROGRAM:


  AUTHORS:
           Gary Bader and Christopher W.V. Hogue hogue@mshri.on.ca


  REVISION/CONTRIBUTION NOTES:

  June 1, 2000  Original MSH Revision.
                Hogue Lab - University of Toronto Biochemistry
                Department and the Samuel Lunenfeld Research
                Institute, Mount Sinai Hospital
                http://bioinfo.mshri.on.ca  hogue@mshri.on.ca
                ftp://ftp.mshri.on.ca/


  PUBLICATION TO CITE:


  DESCRIPTION
  taxonomy database part of SeqHound


******************************************************************************************/

#ifndef _TAXDB_DB_P_
#define _TAXDB_DB_P_

#ifdef __cplusplus
extern "C" {
#endif

#include <d4all.h>
#include <ncbi.h>

/*GLOBAL VARIABLES*/
extern CODE4 cb;

DATA4 *pd4tTAXdb;
DATA4 *pd4tDIVdb;
DATA4 *pd4tGCODEdb;
DATA4 *pd4tDELdb;
DATA4 *pd4tMERGEdb;

/*Taxonomy db fields*/
FIELD4 *pf4TaxTtid;
FIELD4 *pf4TaxTasn;

/*Tax division db fields*/
FIELD4 *pf4TaxDdid;
FIELD4 *pf4TaxDasn;

/*Genetic code db fields*/
FIELD4 *pf4TaxGtid;
FIELD4 *pf4TaxGasn;

/*Deleted nodes history db fields*/
FIELD4 *pf4TaxDNtid;

/*Merged nodes history db fields*/
FIELD4 *pf4TaxMNoldtid;
FIELD4 *pf4TaxMNnewtid;


/*************/
/*Taxonomy DB*/
/*************/

static FIELD4INFO f4tTAXdb[] =
{
	{ "TID",		'N', 20, 0 },	/* Taxonomy ID */
	{ "ASN",		'M', 10, 0 },	/* SLRI-taxon record */
	{ 0,0,0,0 }
};

/*data indexing tags*/
static TAG4INFO t4tTAXdb[] =
{
	{ "TID","TID", 0, e4unique, 0 },
	{ 0, 0, 0, 0, 0 }
};

/**********************/
/*Taxonomy Division DB*/
/**********************/

static FIELD4INFO f4tDIVdb[] =
{
	{ "DID",		'N', 20, 0 },	/* Taxonomy Division ID */
	{ "ASN",		'M', 10, 0 },	/* SLRI-taxon-div record */
	{ 0,0,0,0 }
};

static TAG4INFO t4tDIVdb[] =
{
	{ "DID","DID", 0, e4unique, 0 },
	{ 0, 0, 0, 0, 0 }
};

/*****************/
/*Genetic Code DB*/
/*****************/

static FIELD4INFO f4tGCODEdb[] =
{
	{ "GCID",		'N', 20, 0 },	/* Genetic code ID */
	{ "ASN",		'M', 10, 0 },	/* SLRI-taxon-gencode record */
	{ 0,0,0,0 }
};

static TAG4INFO t4tGCODEdb[] =
{
	{ "GCID","GCID", 0, e4unique, 0 },
	{ 0, 0, 0, 0, 0 }
};

/*************************/
/*Deleted node history DB*/
/*************************/

static FIELD4INFO f4tDELdb[] =
{
	{ "TID",		'N', 20, 0 },	/* Taxonomy ID */
	{ 0,0,0,0 }
};

static TAG4INFO t4tDELdb[] =
{
	{ "TID","TID", 0, e4unique, 0 },
	{ 0, 0, 0, 0, 0 }
};

/************************/
/*Merged node history DB*/
/************************/

static FIELD4INFO f4tMERGEdb[] =
{
	{ "NTID",		'N', 20, 0 },	/* New Taxonomy ID */
	{ "OTID",		'N', 20, 0 },	/* Old Taxonomy ID */
	{ 0,0,0,0 }
};

static TAG4INFO t4tMERGEdb[] =
{
	{ "NTID","NTID", 0, 0, 0 },
	{ "OTID","OTID", 0, 0, 0 },
	{ 0, 0, 0, 0, 0 }
};

#ifdef __cplusplus
}
#endif
#endif		/* _TAXDB_DB_P_ */


/* 
$Log: taxdb_db_p.h,v $
Revision 1.1.1.1  2001/10/11 17:37:13  gbader
Automated import from Sourceforge.net

Revision 1.1.1.1  2001/05/25 21:36:20  kaca
initial import

Revision 1.1  2001/03/20 16:20:24  kaca
migrated from the ../taxon directory

Revision 1.7  2001/02/23 21:20:11  kaca
license change

Revision 1.6  2000/11/02 22:32:11  kaca
made code base variable extern

Revision 1.5  2000/09/20 16:31:27  kaca
fixed Log keyword

 Revision 1.4  2000/09/19 23:44:42  kaca
 added CVS Id and Log keywords
 */


