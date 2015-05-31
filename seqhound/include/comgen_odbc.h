/* $Id: comgen_odbc.h,v 1.3 2005/05/02 15:44:27 eburgess Exp $
*******************************************************************************************
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
  header file for comgen calls


******************************************************************************************/

#ifndef _INTREZ_COMGEN_
#define _INTREZ_COMGEN_

#include <intrez_odbc.h>
#include <seqhound.h>

#ifdef __cplusplus
extern "C" { /* } */
#endif

/*****************************************************************
  SQL variables and handles.
***************************************************************/
/* CHROM variables */
static SH_IntParam chrom_kloodge;
static SH_IntParam chrom_taxid;
static SH_IntParam chrom_chromid;
static SH_IntParam chrom_chromfl;
static SH_CharParam chrom_access;
static SH_IntParam chrom_projectid;
static SH_CharParam chrom_chromnum;

/*CONTIGCHROMID varibles */
static SH_IntParam gichromid_id;
static SH_IntParam gichromid_gi;
static SH_IntParam gichromid_contiggi;
static SH_IntParam gichromid_chromid;

/*CONTIGCHROMID varibles */
static SH_IntParam contigchromid_id;
static SH_IntParam contigchromid_contiggi;
static SH_IntParam contigchromid_topgi;
static SH_IntParam contigchromid_chromid;
static SH_IntParam contigchromid_changed;

/* GICHROMOSOME varibles */
static SH_IntParam gichrom_id;
static SH_IntParam gichrom_gi;
static SH_IntParam gichrom_contiggi;
static SH_CharParam gichrom_chromnum;

/* CONTIGCHROMOSOME varibles */
static SH_IntParam contigchrom_id;
static SH_IntParam contigchrom_contiggi;
static SH_CharParam contigchrom_chromnum;

/* CHROM fields in SQLBindParameter */
struct
{
  SQLCHAR val[1024];
   SQLINTEGER len;
}chrom_name;


/* Handles */
static SQLHANDLE HNDLChrom;
static SQLHANDLE HNDLChromSearchAccess; /* search CHROM kloodge by access */
static SQLHANDLE HNDLChromDeleteAccess; /* delete from CHROM by access */
static SQLHANDLE HNDLChromSearchChromId; /* search for kloodge by chromid */
static SQLHANDLE HNDLChromSearchChromNum; /* search for kloodge by chromnum */
static SQLHANDLE HNDLGichromidInsert;
static SQLHANDLE HNDLGichromidReplace;
static SQLHANDLE HNDLGichromDeleteGi;
static SQLHANDLE HNDLGichromidDeleteContigGi;
static SQLHANDLE HNDLGichromidDeleteChromid;
static SQLHANDLE HNDLGichromidDeleteGi;
static SQLHANDLE HNDLGichromidSearchGi;
static SQLHANDLE HNDLGichromidSearchContiggi;
static SQLHANDLE HNDLGichromoInsert;
static SQLHANDLE HNDLContigchromoInsert;

static SQLHANDLE HNDLContigchromidInsert;
static SQLHANDLE HNDLContigchromidDeleteChromid;
static SQLHANDLE HNDLContigchromidDeleteGi;
static SQLHANDLE HNDLContigchromidDeleteTopGi;
static SQLHANDLE HNDLContigchromidUpdate;
static SQLHANDLE HNDLContigchromidSearchTopGi;
static SQLHANDLE HNDLTaxgiSearchKloodgeType;
static SQLHANDLE HNDLTaxgiUnknownChromosome;

/* Function prototypes */
Boolean InitCHROM(void);
Boolean InitGICHROMID(void);
Boolean InitCONTIGCHROMID(void);
Boolean InitGICHROMOSOME(void);
Boolean InitCONTIGCHROMOSOME(void);
Boolean InitCOMGEN(void);

Boolean LIBCALL OpenCHROM(CharPtr name, Boolean Create);
Boolean LIBCALL CloseCHROM(void);

Boolean LIBCALL AppendRecordCHROM(StChromPtr pch);
Boolean LIBCALL AppendRecordGICHROMID( StGichromidPtr pRecord );
Boolean LIBCALL ReplaceRecordGICHROMID( StGichromidPtr pRecord );
Boolean LIBCALL AppendRecordGICHROMOSOME( StGichromPtr pRecord);
Boolean LIBCALL AppendRecordCONTIGCHROMOSOME( StContigchromPtr pRecord);
Boolean LIBCALL ReplaceRecordCONTIGCHROMID( StContigchromidPtr pRecord );

Boolean LIBCALL EditRecordCONTIGCHROMIDUpdate( Int4 gi, Int4 update);
Boolean LIBCALL EditRecordCONTIGCHROMIDUpdateAll();

Int2               LIBCALL SearchCHROM (StChromPtr PNTR);
Int4               LIBCALL SearchCHROMByChromId(Int4 chromid);
Int4               LIBCALL SearchCHROMByChromNum(CharPtr xmeNum, Int4* chromId, Int4 taxId); 
StGichromidPtr     LIBCALL SearchGICHROMIDByGi( Int4 gi );
StGichromidPtr     LIBCALL SearchGICHROMIDByContigGi( Int4 gi );
StContigchromidPtr LIBCALL SearchCONTIGCHROMIDByTopGi( Int4 gi );

Boolean LIBCALL DeleteAccessInCHROM( CharPtr access );
Boolean LIBCALL DeleteChromidInGICHROMID( Int4 chromid );
Boolean LIBCALL DeleteGiInGICHROMID( Int4 gi);
Boolean LIBCALL DeleteContigGiInGICHROMID( Int4 gi);
Boolean LIBCALL DeleteTopGiInCONTIGCHROMID( Int4 gi);
Boolean LIBCALL DeleteChromidInCONTIGCHROMID( Int4 chromid );
Boolean LIBCALL DeleteGiInCONTIGCHROMID( Int4 gi );
Boolean LIBCALL DeleteAllCONTIGCHROMOSOME();
Boolean LIBCALL DeleteAllGICHROMOSOME();
Boolean LIBCALL DeleteAllRecordsInCHROM();

/* Functions to remove genbank records from comgen tables */
Boolean LIBCALL RemoveGenBankRecordsFromGICHROMID();
Boolean LIBCALL RemoveGenBankRecordsFromCONTIGCHROMID();
Boolean LIBCALL RemoveGenBankRecordsFromGICHROMOSOME();
Boolean LIBCALL RemoveGenBankRecordsFromCONTIGCHROMOSOME();

StGichromidPtr     LIBCALL GetAllRecordsFromGICHROMID();
StContigchromidPtr LIBCALL GetAllRecordsFromCONTIGCHROMID();
StGichromidPtr     LIBCALL GetAllRecordsFromGICHROMOSOME();
StContigchromidPtr LIBCALL GetAllRecordsFromCONTIGCHROMOSOME();

Boolean LIBCALL AddChromIdsToGICHROMID();
Boolean LIBCALL AddChromIdsToCONTIGCHROMID();

void LIBCALL ResetStCHROM(StChromPtr PNTR ppch);
void LIBCALL ResetStGICHROMID(StGichromidPtr PNTR pRecord);
void LIBCALL ResetStCONTIGCHROMID(StContigchromidPtr PNTR pRecord);
void LIBCALL ResetStGICHROM(StGichromPtr PNTR ppch);
void LIBCALL ResetStCONTIGCHROM(StContigchromPtr PNTR ppch);

/* API functions */
ValNodePtr     LIBCALL SHoundAllGenomes(void);
ValNodePtr     LIBCALL SHoundAllGenAccess(void);
ValNodePtr     LIBCALL SHoundAllKloodge(void);
Int4           LIBCALL SHoundGetKloodgeFromAccess(CharPtr paccess);

/* Utility functions */
ValNodePtr LIBCALL GetMoleculeFromChromosome( Int4 ChromId, 
											  CharPtr type );
ValNodePtr LIBCALL GetMoleculeFromUnknownChromosome( Int4 taxid, 
													 CharPtr type );
Int4 LIBCALL TranslateChromFlag( Int4 chromflag );

#ifdef __cplusplus
 }
#endif

#endif /*  _INTREZ_COMGEN_  defined*/

/* $Log: comgen_odbc.h,v $
/* Revision 1.3  2005/05/02 15:44:27  eburgess
/* Added a field for NCBI projectid to the chrom table. Bug 5438
/*
/* Revision 1.2  2005/04/07 21:47:35  zwang
/* Add InitCOMGEN prototype.
/*
/* Revision 1.1  2005/02/01 16:51:14  eburgess
/* Moved functions from intrez_odbc.c
/* */
