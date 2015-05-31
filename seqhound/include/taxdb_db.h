/*  $Id: taxdb_db.h,v 1.2 2004/10/01 16:27:02 zwang Exp $
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


#ifndef _TAXDB_DB_
#define _TAXDB_DB_

#ifdef __cplusplus
extern "C" {
#endif


#include <asn.h>
#include <objslritax.h>
#include <slri_asn.h>


/************************/
/* Function definitions */
/************************/

/************************/
/*General Initialization*/
/************************/

/*Open the taxonomy database*/
extern Int2 OpenTaxDB(Boolean CREATE);

/*Close the taxonomy database*/
extern Int2 CloseTaxDB(void);

/**********/
/*CodeBase*/
/**********/

/*Initialize codebase for the taxonomy database*/
extern void InitCodeBaseTax(void);

extern Boolean OpentTAXDB(Boolean CREATE);
extern Boolean OpentDIVDB(Boolean CREATE);
extern Boolean OpentGCODEDB(Boolean CREATE);
extern Boolean OpentDELDB(Boolean CREATE);
extern Boolean OpentMERGEDB(Boolean CREATE);

/*Opens all taxonomy database - returns TRUE if any database was created*/
extern Boolean OpenAllTaxDBs(Boolean CREATE);

/*Close all taxonomy databases*/
extern void CloseAllTaxDBs(void);

/*Close codebase for the taxonomy database*/
extern void CloseCodeBaseTax(void);

/* start a transaction by setting auto_commit off */
extern Boolean StartTransactionTax(void);

/* end a transaction by commiting all the changes */
extern Boolean CommitTransactionTax(void);

/* end a transaction by rolling back all the changes */
extern Boolean RollBackTransactionTax(void);


/***********************/
/*Data Management Calls*/
/***********************/

/* delete all records in TAX tables */
extern Boolean DeleteAllRecordsInTAX(void);

/*main taxonomy database*/

/* Write an SLRI-taxon record to the database */
extern Int2 DB_WriteTaxRec(SLRITaxonPtr stp);

/* Replace an SLRI-Taxon record in the database */
extern Int2 DB_ReplaceTaxRec(SLRITaxonPtr stp);

/* Get an SLRI-taxon record from the database */
extern SLRITaxonPtr DB_GetTaxRec(Int4 taxId);

/* Delete an SLRI-taxon record from the database */
extern Int4 DB_DelTaxRec(Int4 taxId);

/* Undelete an SLRI-taxon record from the database */
extern Int4 DB_UnDelTaxRec(Int4 taxId);

/*taxonomy division database*/

/* Write an SLRITaxonDiv record to the database */
extern Int2 DB_WriteTaxDivRec(SLRITaxonDivPtr stdp);

/* Get an SLRITaxonDiv record from the database */
extern SLRITaxonDivPtr DB_GetTaxDivRec(Int4 divId);

/* Delete an SLRITaxonDiv record from the database */
extern Int4 DB_DelTaxDivRec(Int4 divId);

/* Undelete an SLRITaxonDiv record from the database */
extern Int4 DB_UnDelTaxDivRec(Int4 divId);

/*Genetic code database*/

/* Write an SLRITaxonGencode record to the database */
extern Int2 DB_WriteTaxGenCodeRec(SLRITaxonGencodePtr stgp);

/* Get an SLRITaxonGencode record from the database */
extern SLRITaxonGencodePtr DB_GetTaxGenCodeRec(Int4 genCodeId);

/* Delete an SLRITaxonGencode record from the database */
extern Int4 DB_DelTaxGenCodeRec(Int4 genCodeId);

/* Undelete an SLRITaxonGencode record from the database */
extern Int4 DB_UnDelTaxGenCodeRec(Int4 genCodeId);

/*Deleted nodes history database*/

/* Write a deleted node record to the database */
extern Int2 DB_WriteTaxDelNodeRec(Int4 taxId);

/* Get a deleted node record from the database */
extern Int4 DB_GetTaxDelNodeRec(Int4 taxId);

/* Delete a deleted node record from the database */
extern Int4 DB_DelTaxDelNodeRec(Int4 taxId);

/* Undelete a deleted node record from the database */
extern Int4 DB_UnDelTaxDelNodeRec(Int4 taxId);

/*Merged nodes history database*/

/* Write a merged node record to the database */
extern Int2 DB_WriteTaxMergedNodeRec(Int4 oldTaxId, Int4 newTaxId);

/* Get an old Tax ID given a new Tax ID from the merged node database */
extern Int4 DB_GetTaxMergedNodeRecOldTIDByNewTID(Int4 newTaxId);

/* Get a new Tax ID given an old Tax ID from the merged node database */
extern Int4 DB_GetTaxMergedNodeRecNewTIDByOldTID(Int4 oldTaxId);

/* Delete a merged node record from the database by new Tax ID*/
extern Int4 DB_DelTaxMergedNodeRec(Int4 newTaxId);

/* Undelete a merged node record from the database by New Tax ID*/
extern Int4 DB_UnDelTaxMergedNodeRec(Int4 newTaxId);

/*Pack the TAX DB*/
extern void DB_PackTAXDB();

/*Pack the DIV DB*/
extern void DB_PackDIVDB();

/*Pack the GCODE DB*/
extern void DB_PackGCODEDB();

/*Pack the DEL DB*/
extern void DB_PackDELDB();

/*Pack the MERGE DB*/
extern void DB_PackMERGEDB();

/* Pack all of the taxonomy databases */
extern void DB_PackAllTaxDB(void);

/*Function for usein importtaxdb*/
extern Int2 TaxIDDisk_BuildChildrenNodeList(void);

#ifdef __cplusplus
}
#endif
#endif		/* _TAXDB_DB_ */

/* 
 $Log: taxdb_db.h,v $
 Revision 1.2  2004/10/01 16:27:02  zwang
 Added transaction-related functions.

 Revision 1.1.1.4  2002/03/01 09:25:27  root
 Automated import from Sourceforge.net

 Revision 1.4  2002/02/28 17:22:59  micheld
 New MSVC project files to build cb/db2/rem apps and libraries
 Moved include order to accomodate msvc, removed #ifdef [CODEBASE|DB2]

 Revision 1.3  2002/01/03 20:56:07  risserlin
 added ifdefs for d4all.h in header file so can be used when using db2

 Revision 1.2  2001/11/21 23:50:58  kaca
 database opening controled by CREATE Boolean

 Revision 1.1.1.1  2001/05/25 21:36:20  kaca
 initial import

 Revision 1.2  2001/03/29 15:47:53  gbader
 Moved externs to be before functions

 Revision 1.1  2001/03/20 16:20:24  kaca
 migrated from the ../taxon directory

 Revision 1.6  2001/02/23 21:20:11  kaca
 license change

 Revision 1.5  2000/09/20 21:38:47  kaca
 fixed Log keyword

 Revision 1.4  2000/09/19 23:44:42  kaca
 added CVS Id and Log keywords
*/




