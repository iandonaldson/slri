/*  $Id: taxdb_cb.c,v 1.2 2003/11/03 15:50:27 haocl Exp $
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
#include <ncbi.h>
#include <taxdb_db_p.h>
#include <taxdb_db.h>
#include <slri_cb.h>
#include <slri_misc.h>

/*******************************/
/* Taxonomy database functions */
/*******************************/

/*field initializers - must call these before any DB access*/
static void InittTAXdbFields(void)
{
	/* assign field pointers to the fields by name */
	pf4TaxTtid = d4field(pd4tTAXdb,"TID");
	pf4TaxTasn = d4field(pd4tTAXdb,"ASN");
}

static void InittDIVdbFields(void)
{
	/* assign field pointers to the fields by name */
	pf4TaxDdid = d4field(pd4tDIVdb,"DID");
	pf4TaxDasn = d4field(pd4tDIVdb,"ASN");
}

static void InittGCODEdbFields(void)
{
	/* assign field pointers to the fields by name */
	pf4TaxGtid = d4field(pd4tGCODEdb,"GCID");
	pf4TaxGasn = d4field(pd4tGCODEdb,"ASN");
}

static void InittDELdbFields(void)
{
	/* assign field pointers to the fields by name */
	pf4TaxDNtid = d4field(pd4tDELdb,"TID");
}

static void InittMERGEdbFields(void)
{
	/* assign field pointers to the fields by name */
	pf4TaxMNoldtid = d4field(pd4tMERGEdb,"OTID");
	pf4TaxMNnewtid = d4field(pd4tMERGEdb,"NTID");
}

Boolean OpentTAXDB(Boolean CREATE)
{
	Char pathdatab[PATH_MAX];
	
	GetAppParam("intrez", "datab", "path", NULL, pathdatab, (size_t)PATH_MAX * (sizeof(Char)));
	StringCat(pathdatab, "TAX");
	return(OpenDBII(pathdatab, &pd4tTAXdb, f4tTAXdb, t4tTAXdb, (InitFieldFunc) InittTAXdbFields, &cb, CREATE));
}

Boolean OpentDIVDB(Boolean CREATE)
{
	
	Char pathdatab[PATH_MAX];	

	GetAppParam("intrez", "datab", "path", NULL, pathdatab, (size_t)PATH_MAX * (sizeof(Char)));
	StringCat(pathdatab, "DIV");
	return(OpenDBII(pathdatab, &pd4tDIVdb, f4tDIVdb, t4tDIVdb, (InitFieldFunc) InittDIVdbFields, &cb, CREATE));
}

Boolean OpentGCODEDB(Boolean CREATE)
{
	Char pathdatab[PATH_MAX];
	
	GetAppParam("intrez", "datab", "path", NULL, pathdatab, (size_t)PATH_MAX * (sizeof(Char)));
	StringCat(pathdatab, "GCODE");
	return(OpenDBII(pathdatab, &pd4tGCODEdb, f4tGCODEdb, t4tGCODEdb, (InitFieldFunc) InittGCODEdbFields, &cb, CREATE));
}

Boolean OpentDELDB(Boolean CREATE)
{
	Char pathdatab[PATH_MAX];
	
	GetAppParam("intrez", "datab", "path", NULL, pathdatab, (size_t)PATH_MAX * (sizeof(Char)));
	StringCat(pathdatab, "DEL");
	return(OpenDBII(pathdatab, &pd4tDELdb, f4tDELdb, t4tDELdb, (InitFieldFunc) InittDELdbFields, &cb, CREATE));
}

Boolean OpentMERGEDB(Boolean CREATE)
{
	
	Char pathdatab[PATH_MAX];
	
	GetAppParam("intrez", "datab", "path", NULL, pathdatab, (size_t)PATH_MAX * (sizeof(Char)));
	StringCat(pathdatab, "MERGE");
	return(OpenDBII(pathdatab, &pd4tMERGEdb, f4tMERGEdb, t4tMERGEdb, (InitFieldFunc) InittMERGEdbFields, &cb, CREATE));
}

/*Opens all taxonomy database - returns TRUE if any database was created*/
Boolean OpenAllTaxDBs(Boolean CREATE)
{
	Boolean c1=FALSE, c2=FALSE, c3=FALSE, c4=FALSE, c5=FALSE;

	c1 = OpentTAXDB(CREATE);
	c2 = OpentDIVDB(CREATE);
	c3 = OpentGCODEDB(CREATE);
	c4 = OpentDELDB(CREATE);
	c5 = OpentMERGEDB(CREATE);

	if(c1 && c2 && c3 && c4 && c5)
		return(TRUE);

	return(FALSE);
}

/*Initialize codebase for the taxonomy database*/
void InitCodeBaseTax(void)
{
	code4init(&cb);
	cb.errOpen = 0;
	cb.safety = 0;
	/*adding a duplicate accession number creates an error*/
	cb.errDefaultUnique = e4unique;
}

/*Close codebase for the taxonomy database*/
void CloseCodeBaseTax(void)
{
	/*explicitly flush all DB's*/
	d4flush(pd4tTAXdb);
	d4flush(pd4tDIVdb);
	d4flush(pd4tGCODEdb);
	d4flush(pd4tDELdb);
	d4flush(pd4tMERGEdb);
	/*close Codebase*/
	code4close(&cb);
	code4initUndo(&cb);
}

/*Close all taxonomy databases*/
void CloseAllTaxDBs(void)
{
	/*explicitly close all DB's*/
	d4close(pd4tTAXdb);
	d4close(pd4tDIVdb);
	d4close(pd4tGCODEdb);
	d4close(pd4tDELdb);
	d4close(pd4tMERGEdb);
}


/*Open the taxonomy database*/
Int2 OpenTaxDB(Boolean CREATE)
{
	InitCodeBaseTax();
	OpenAllTaxDBs(CREATE);

	return(0);
}

/*Close the taxonomy database*/
Int2 CloseTaxDB(void)
{
	CloseCodeBaseTax();

	return(0);
}

/*Pack the TAX DB*/
void DB_PackTAXDB()
{
	d4pack(pd4tTAXdb);
	d4memoCompress(pd4tTAXdb);
}

/*Pack the DIV DB*/
void DB_PackDIVDB()
{
	d4pack(pd4tDIVdb);
	d4memoCompress(pd4tDIVdb);
}

/*Pack the GCODE DB*/
void DB_PackGCODEDB()
{
	d4pack(pd4tGCODEdb);
	d4memoCompress(pd4tGCODEdb);
}

/*Pack the DEL DB*/
void DB_PackDELDB()
{
	d4pack(pd4tDELdb);
	d4memoCompress(pd4tDELdb);
}

/*Pack the MERGE DB*/
void DB_PackMERGEDB()
{
	d4pack(pd4tMERGEdb);
	d4memoCompress(pd4tMERGEdb);
}

/* Pack all of the taxonomy databases */
void DB_PackAllTaxDB(void)
{
	DB_PackTAXDB();
	DB_PackDIVDB();
	DB_PackGCODEDB();
	DB_PackDELDB();
	DB_PackMERGEDB();
}

/***************************/
/*Data Management Functions*/
/***************************/

/*main taxonomy database*/

/* Write an SLRI-taxon record to the database */
Int2 DB_WriteTaxRec(SLRITaxonPtr stp)
{
	if(stp == NULL) {
		ErrPostEx(SEV_INFO, 3, 0, "DB_WriteTaxRec: Passed ASN.1 pointer is null.");
		return(-1);
	}

	/*add the asn.1 record*/
	d4appendStart(pd4tTAXdb,0);
	f4assignLong(pf4TaxTtid, stp->taxId);

	if(!AssignASNMemMemo((Pointer) stp, (AsnWriteFunc) SLRITaxonAsnWrite, pf4TaxTasn)) {
		ErrPostEx(SEV_INFO, 3, 0, "DB_WriteTaxRec: Call to AssignASNMemMemo failed - Tax ID %ld.", stp->taxId);
		return(-1);
	}
	/*end record append*/
	d4append(pd4tTAXdb);

	return(0);
}

/* Replace an SLRI-Taxon record in the database */
Int2 DB_ReplaceTaxRec(SLRITaxonPtr stp)
{
	Int4 taxId=0;

	if(stp == NULL) {
		ErrPostEx(SEV_INFO, 3, 0, "DB_ReplaceTaxRec: Passed ASN.1 pointer is null.");
		return(-1);
	}

	taxId = stp->taxId;
	d4tagSelect(pd4tTAXdb, d4tag(pd4tTAXdb, "TID"));
	if(d4seekDouble(pd4tTAXdb, taxId) != r4success) {
		ErrPostEx(SEV_INFO, 0, 0, "Taxonomy ID: %ld not found in database.", taxId);
		return(-1);
	}

	if(d4deleted(pd4tTAXdb)) {	/*record has been deleted, but database not packed*/
		ErrPostEx(SEV_INFO, 0, 0, "Taxonomy ID: %ld has been deleted.  Must undelete to access.", taxId);
		return(-1);
	}

	if(!AssignASNMemMemo((Pointer) stp, (AsnWriteFunc) SLRITaxonAsnWrite, pf4TaxTasn)) {
		ErrPostEx(SEV_INFO, 3, 0, "DB_ReplaceTaxRec: Call to AssignASNMemMemo failed - Tax ID %ld.", taxId);
		return(-1);
	}

	return(0);
}

/* Get an SLRI-taxon record from the database */
SLRITaxonPtr DB_GetTaxRec(Int4 taxId)
{
	SLRITaxonPtr stp=NULL;
	Int4 newTaxId=0;

	d4tagSelect(pd4tTAXdb, d4tag(pd4tTAXdb, "TID"));
	if(d4seekDouble(pd4tTAXdb, taxId) != r4success) {
		/*try looking in merged database before giving up*/
		newTaxId = DB_GetTaxMergedNodeRecNewTIDByOldTID(taxId);
		if(newTaxId<0) {
			ErrPostEx(SEV_INFO, 0, 0, "Taxonomy ID: %ld not found in database.", taxId);
			return(NULL);
		}
		else {
			if(d4seekDouble(pd4tTAXdb, taxId) != r4success) {
				ErrPostEx(SEV_INFO, 0, 0, "Taxonomy ID: %ld not found in database.", taxId);
				return(NULL);
			}
		}
	}

	if(d4deleted(pd4tTAXdb)) {	/*record has been deleted, but database not packed*/
		ErrPostEx(SEV_INFO, 0, 0, "Taxonomy ID: %ld has been deleted.  Must undelete to access.", taxId);
		return(NULL);
	}

	stp = (SLRITaxonPtr) GetASNMemMemo(pf4TaxTasn, (AsnReadFunc) SLRITaxonAsnRead);
	if(stp==NULL) {
		ErrPostEx(SEV_INFO, 3, 0, "DB_GetTaxRec: Call to GetASNMemMemo failed Taxonomy ID %ld.", taxId);
		return(NULL);
	}

	return(stp);
}

/* Delete an SLRI-taxon record from the database */
Int4 DB_DelTaxRec(Int4 taxId)
{
	d4tagSelect(pd4tTAXdb, d4tag(pd4tTAXdb, "TID"));
	if(d4seekDouble(pd4tTAXdb, taxId) != r4success) {
		ErrPostEx(SEV_INFO, 0, 0, "Can't delete Taxonomy ID: %ld - not found in database.", taxId);
		return(-1);
	}

	d4delete(pd4tTAXdb);
	return(0);
}

/* Undelete an SLRI-taxon record from the database */
Int4 DB_UnDelTaxRec(Int4 taxId)
{
	d4tagSelect(pd4tTAXdb, d4tag(pd4tTAXdb, "TID"));
	if(d4seekDouble(pd4tTAXdb, taxId) != r4success) {
		ErrPostEx(SEV_INFO, 0, 0, "Can't undelete Taxonomy ID: %ld - not found in database.", taxId);
		return(-1);
	}

	if(!d4deleted(pd4tTAXdb)) {	/*record has not been deleted*/
		ErrPostEx(SEV_INFO, 0, 0, "Taxonomy ID %ld has not been deleted.", taxId);
		return(-1);
	}

	d4recall(pd4tTAXdb);
	return(0);
}

/*taxonomy division database*/

/* Write an SLRITaxonDiv record to the database */
Int2 DB_WriteTaxDivRec(SLRITaxonDivPtr stdp)
{
	if(stdp == NULL) {
		ErrPostEx(SEV_INFO, 3, 0, "DB_WriteTaxDivRec: Passed ASN.1 pointer is null.");
		return(-1);
	}

	/*add the asn.1 record*/
	d4appendStart(pd4tDIVdb,0);
	f4assignLong(pf4TaxDdid, stdp->div_id);

	if(!AssignASNMemMemo((Pointer) stdp, (AsnWriteFunc) SLRITaxonDivAsnWrite, pf4TaxDasn)) {
		ErrPostEx(SEV_INFO, 3, 0, "DB_WriteTaxDivRec: Call to AssignASNMemMemo failed - Tax Division ID %ld.", stdp->div_id);
		return(-1);
	}
	/*end record append*/
	d4append(pd4tDIVdb);

	return(0);
}

/* Get an SLRITaxonDiv record from the database */
SLRITaxonDivPtr DB_GetTaxDivRec(Int4 divId)
{
	SLRITaxonDivPtr stdp=NULL;

	d4tagSelect(pd4tDIVdb, d4tag(pd4tDIVdb, "DID"));
	if(d4seekDouble(pd4tDIVdb, divId) != r4success) {
		ErrPostEx(SEV_INFO, 0, 0, "Taxonomy Division ID: %ld not found in database.", divId);
		return(NULL);
	}

	if(d4deleted(pd4tDIVdb)) {	/*record has been deleted, but database not packed*/
		ErrPostEx(SEV_INFO, 0, 0, "Taxonomy Division ID: %ld has been deleted.  Must undelete to access.", divId);
		return(NULL);
	}

	stdp = (SLRITaxonDivPtr) GetASNMemMemo(pf4TaxDasn, (AsnReadFunc) SLRITaxonDivAsnRead);
	if(stdp==NULL) {
		ErrPostEx(SEV_INFO, 3, 0, "DB_GetTaxDivRec: Call to GetASNMemMemo failed Taxonomy Division ID %ld.", divId);
		return(NULL);
	}

	return(stdp);
}

/* Delete an SLRITaxonDiv record from the database */
Int4 DB_DelTaxDivRec(Int4 divId)
{
	d4tagSelect(pd4tDIVdb, d4tag(pd4tDIVdb, "DID"));
	if(d4seekDouble(pd4tDIVdb, divId) != r4success) {
		ErrPostEx(SEV_INFO, 0, 0, "Can't delete Taxonomy Division ID: %ld - not found in database.", divId);
		return(-1);
	}

	d4delete(pd4tDIVdb);
	return(0);
}

/* Undelete an SLRITaxonDiv record from the database */
Int4 DB_UnDelTaxDivRec(Int4 divId)
{
	d4tagSelect(pd4tDIVdb, d4tag(pd4tDIVdb, "DID"));
	if(d4seekDouble(pd4tDIVdb, divId) != r4success) {
		ErrPostEx(SEV_INFO, 0, 0, "Can't undelete Taxonomy Division ID: %ld - not found in database.", divId);
		return(-1);
	}

	if(!d4deleted(pd4tDIVdb)) {	/*record has not been deleted*/
		ErrPostEx(SEV_INFO, 0, 0, "Taxonomy Division ID %ld has not been deleted.", divId);
		return(-1);
	}

	d4recall(pd4tDIVdb);
	return(0);
}

/*Genetic code database*/

/* Write an SLRITaxonGencode record to the database */
Int2 DB_WriteTaxGenCodeRec(SLRITaxonGencodePtr stgp)
{
	if(stgp == NULL) {
		ErrPostEx(SEV_INFO, 3, 0, "DB_WriteTaxGenCodeRec: Passed ASN.1 pointer is null.");
		return(-1);
	}

	/*add the asn.1 record*/
	d4appendStart(pd4tGCODEdb,0);
	f4assignLong(pf4TaxGtid, stgp->gencode_id);

	if(!AssignASNMemMemo((Pointer) stgp, (AsnWriteFunc) SLRITaxonGencodeAsnWrite, pf4TaxGasn)) {
		ErrPostEx(SEV_INFO, 3, 0, "DB_WriteTaxGenCodeRec: Call to AssignASNMemMemo failed - Tax Genetic Code ID %ld.", stgp->gencode_id);
		return(-1);
	}
	/*end record append*/
	d4append(pd4tGCODEdb);

	return(0);
}

/* Get an SLRITaxonGencode record from the database */
SLRITaxonGencodePtr DB_GetTaxGenCodeRec(Int4 genCodeId)
{
	SLRITaxonGencodePtr stgp=NULL;

	d4tagSelect(pd4tGCODEdb, d4tag(pd4tGCODEdb, "GCID"));
	if(d4seekDouble(pd4tGCODEdb, genCodeId) != r4success) {
		ErrPostEx(SEV_INFO, 0, 0, "Taxonomy Genetic Code ID: %ld not found in database.", genCodeId);
		return(NULL);
	}

	if(d4deleted(pd4tGCODEdb)) {	/*record has been deleted, but database not packed*/
		ErrPostEx(SEV_INFO, 0, 0, "Taxonomy Genetic Code ID: %ld has been deleted.  Must undelete to access.", genCodeId);
		return(NULL);
	}

	stgp = (SLRITaxonGencodePtr) GetASNMemMemo(pf4TaxGasn, (AsnReadFunc) SLRITaxonGencodeAsnRead);
	if(stgp==NULL) {
		ErrPostEx(SEV_INFO, 3, 0, "DB_GetTaxGenCodeRec: Call to GetASNMemMemo failed Taxonomy Genetic Code ID %ld.", genCodeId);
		return(NULL);
	}

	return(stgp);
}

/* Delete an SLRITaxonGencode record from the database */
Int4 DB_DelTaxGenCodeRec(Int4 genCodeId)
{
	d4tagSelect(pd4tGCODEdb, d4tag(pd4tGCODEdb, "GCID"));
	if(d4seekDouble(pd4tGCODEdb, genCodeId) != r4success) {
		ErrPostEx(SEV_INFO, 0, 0, "Can't delete Taxonomy Genetic Code ID: %ld - not found in database.", genCodeId);
		return(-1);
	}

	d4delete(pd4tGCODEdb);
	return(0);
}

/* Undelete an SLRITaxonGencode record from the database */
Int4 DB_UnDelTaxGenCodeRec(Int4 genCodeId)
{
	d4tagSelect(pd4tGCODEdb, d4tag(pd4tGCODEdb, "GCID"));
	if(d4seekDouble(pd4tGCODEdb, genCodeId) != r4success) {
		ErrPostEx(SEV_INFO, 0, 0, "Can't undelete Taxonomy Genetic Code ID: %ld - not found in database.", genCodeId);
		return(-1);
	}

	if(!d4deleted(pd4tGCODEdb)) {	/*record has not been deleted*/
		ErrPostEx(SEV_INFO, 0, 0, "Taxonomy Genetic Code ID %ld has not been deleted.", genCodeId);
		return(-1);
	}

	d4recall(pd4tGCODEdb);
	return(0);
}

/*Deleted nodes history database*/

/* Write a deleted node record to the database */
Int2 DB_WriteTaxDelNodeRec(Int4 taxId)
{
	if(taxId < 0) {
		ErrPostEx(SEV_INFO, 3, 0, "DB_WriteTaxDelNodeRec: Passed taxId  < 0.");
		return(-1);
	}

	/*add the record*/
	d4appendStart(pd4tDELdb,0);
	f4assignLong(pf4TaxDNtid, taxId);
	/*end record append*/
	d4append(pd4tDELdb);

	return(0);
}

/* Get a deleted node record from the database */
Int4 DB_GetTaxDelNodeRec(Int4 taxId)
{
	d4tagSelect(pd4tDELdb, d4tag(pd4tDELdb, "TID"));
	if(d4seekDouble(pd4tDELdb, taxId) != r4success) {
		ErrPostEx(SEV_INFO, 0, 0, "Taxonomy ID: %ld not found in deleted node database.", taxId);
		return(-1);
	}

	if(d4deleted(pd4tDELdb)) {	/*record has been deleted, but database not packed*/
		ErrPostEx(SEV_INFO, 0, 0, "Taxonomy ID: %ld has been deleted from deleted node database.  Must undelete to access.", taxId);
		return(-1);
	}

	return(f4long(pf4TaxDNtid));
}

/* Delete a deleted node record from the database */
Int4 DB_DelTaxDelNodeRec(Int4 taxId)
{
	d4tagSelect(pd4tDELdb, d4tag(pd4tDELdb, "TID"));
	if(d4seekDouble(pd4tDELdb, taxId) != r4success) {
		ErrPostEx(SEV_INFO, 0, 0, "Can't delete Taxonomy ID: %ld - not found in deleted node database.", taxId);
		return(-1);
	}

	d4delete(pd4tDELdb);
	return(0);
}

/* Undelete a deleted node record from the database */
Int4 DB_UnDelTaxDelNodeRec(Int4 taxId)
{
	d4tagSelect(pd4tDELdb, d4tag(pd4tDELdb, "TID"));
	if(d4seekDouble(pd4tDELdb, taxId) != r4success) {
		ErrPostEx(SEV_INFO, 0, 0, "Can't undelete Taxonomy ID: %ld - not found in deleted node database.", taxId);
		return(-1);
	}

	if(!d4deleted(pd4tDELdb)) {	/*record has not been deleted*/
		ErrPostEx(SEV_INFO, 0, 0, "Taxonomy ID %ld has not been deleted from deleted node database.", taxId);
		return(-1);
	}

	d4recall(pd4tDELdb);
	return(0);
}

/*Merged nodes history database*/

/* Write a merged node record to the database */
Int2 DB_WriteTaxMergedNodeRec(Int4 oldTaxId, Int4 newTaxId)
{
	if(oldTaxId < 0) {
		ErrPostEx(SEV_INFO, 3, 0, "DB_WriteTaxMergedNodeRec: Passed oldTaxId  < 0.");
		return(-1);
	}
	if(newTaxId < 0) {
		ErrPostEx(SEV_INFO, 3, 0, "DB_WriteTaxMergedNodeRec: Passed newTaxId  < 0.");
		return(-1);
	}

	/*add the record*/
	d4appendStart(pd4tMERGEdb,0);
	f4assignLong(pf4TaxMNoldtid, oldTaxId);
	f4assignLong(pf4TaxMNnewtid, newTaxId);
	/*end record append*/
	d4append(pd4tMERGEdb);

	return(0);
}

/* Get an old Tax ID given a new Tax ID from the merged node database */
Int4 DB_GetTaxMergedNodeRecOldTIDByNewTID(Int4 newTaxId)
{
	d4tagSelect(pd4tMERGEdb, d4tag(pd4tMERGEdb, "NTID"));
	if(d4seekDouble(pd4tMERGEdb, newTaxId) != r4success) {
		ErrPostEx(SEV_INFO, 0, 0, "New Taxonomy ID: %ld not found in merged node database.", newTaxId);
		return(-1);
	}

	if(d4deleted(pd4tMERGEdb)) {	/*record has been deleted, but database not packed*/
		ErrPostEx(SEV_INFO, 0, 0, "New Taxonomy ID: %ld has been deleted from merged node database.  Must undelete to access.", newTaxId);
		return(-1);
	}

	return(f4long(pf4TaxMNoldtid));
}

/* Get a new Tax ID given an old Tax ID from the merged node database */
Int4 DB_GetTaxMergedNodeRecNewTIDByOldTID(Int4 oldTaxId)
{
	d4tagSelect(pd4tMERGEdb, d4tag(pd4tMERGEdb, "OTID"));
	if(d4seekDouble(pd4tMERGEdb, oldTaxId) != r4success) {
		ErrPostEx(SEV_INFO, 0, 0, "Old Taxonomy ID: %ld not found in merged node database.", oldTaxId);
		return(-1);
	}

	if(d4deleted(pd4tMERGEdb)) {	/*record has been deleted, but database not packed*/
		ErrPostEx(SEV_INFO, 0, 0, "Old Taxonomy ID: %ld has been deleted from merged node database.  Must undelete to access.", oldTaxId);
		return(-1);
	}

	return(f4long(pf4TaxMNnewtid));
}

/* Delete a merged node record from the database by new Tax ID*/
Int4 DB_DelTaxMergedNodeRec(Int4 newTaxId)
{
	d4tagSelect(pd4tMERGEdb, d4tag(pd4tMERGEdb, "NTID"));
	if(d4seekDouble(pd4tMERGEdb, newTaxId) != r4success) {
		ErrPostEx(SEV_INFO, 0, 0, "Can't delete new Taxonomy ID: %ld - not found in merged node database.", newTaxId);
		return(-1);
	}

	d4delete(pd4tMERGEdb);
	return(0);
}

/* Undelete a merged node record from the database by New Tax ID*/
Int4 DB_UnDelTaxMergedNodeRec(Int4 newTaxId)
{
	d4tagSelect(pd4tMERGEdb, d4tag(pd4tMERGEdb, "NTID"));
	if(d4seekDouble(pd4tMERGEdb, newTaxId) != r4success) {
		ErrPostEx(SEV_INFO, 0, 0, "Can't undelete new Taxonomy ID: %ld - not found in merged node database.", newTaxId);
		return(-1);
	}

	if(!d4deleted(pd4tMERGEdb)) {	/*record has not been deleted*/
		ErrPostEx(SEV_INFO, 0, 0, "New Taxonomy ID %ld has not been deleted from merged node database.", newTaxId);
		return(-1);
	}

	d4recall(pd4tMERGEdb);
	return(0);
}

/*Function for usein importtaxdb*/
Int2 TaxIDDisk_BuildChildrenNodeList(void)
{
	Int4 rc=0;
	SLRITaxonPtr stp=NULL, stpParent=NULL;
	Int4 parentTaxId=0;

	d4tagSelect(pd4tTAXdb, d4tag(pd4tTAXdb, "TID"));

	for(rc=d4top(pd4tTAXdb);rc==r4success;rc=d4skip(pd4tTAXdb, 1L)) {
		stp = (SLRITaxonPtr) GetASNMemMemo(pf4TaxTasn, (AsnReadFunc) SLRITaxonAsnRead);
		if(stp==NULL) {
			ErrPostEx(SEV_INFO, 3, 0, "DB_GetTaxRec: Call to GetASNMemMemo failed for record after Taxonomy ID %ld.", parentTaxId);
			return(1);
		}
		parentTaxId = stp->parent_taxId;
		if(parentTaxId==stp->taxId) {
			stp = SLRITaxonFree(stp);
			continue;	/*don't allow any circles in the tree*/
		}
		stpParent = DB_GetTaxRec(parentTaxId);
		if(stpParent==NULL) {
			ErrPostEx(SEV_INFO, 0, 0, "TaxIDDisk_BuildChildrenNodeList: DB_GetTaxRec failed for Tax ID %ld.", parentTaxId);
			stp = SLRITaxonFree(stp);
			return(1);
		}
		Misc_AddInt2SortedList(&(stpParent->children_taxId), stp->taxId, 0, TRUE);
		DB_ReplaceTaxRec(stpParent);
		d4seekDouble(pd4tTAXdb, stp->taxId);	/*reposition database for d4skip call*/
		stp = SLRITaxonFree(stp);
		stpParent = SLRITaxonFree(stpParent);
	}

	return(0);
}

/* 
   $Log: taxdb_cb.c,v $
   Revision 1.2  2003/11/03 15:50:27  haocl
   changed order of includes to avoid _LARGE_FILE_SOURCE redefinition warnings

   Revision 1.1.1.2  2002/01/15 09:23:35  root
   Automated import from Sourceforge.net

   Revision 1.2  2002/01/14 22:33:01  kaca
   switched includes generated nlm_beep error

   Revision 1.1  2001/12/19 23:59:52  kaca
   renamed to support database layer naming convention

   Revision 1.2  2001/11/21 23:51:16  kaca
   database opening controled by CREATE Boolean

   Revision 1.1.1.1  2001/05/25 21:36:24  kaca
   initial import

   Revision 1.2  2001/03/29 15:34:07  gbader
   Added slri_misc.h include

   Revision 1.1  2001/03/20 16:22:28  kaca
   migrated from ../taxon directory; library shoundtaxlib merged with shoundlocllib

   Revision 1.5  2001/02/23 21:20:11  kaca
   license change

   Revision 1.4  2000/09/20 21:38:47  kaca
   fixed Log keyword

   Revision 1.3  2000/09/19 23:44:42  kaca
   added CVS Id and Log keywords
*/



