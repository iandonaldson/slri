/* $Id: postcomgen.c,v 1.3 2005/04/22 17:40:02 eburgess Exp $
*******************************************************************************************
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
  and distribute the resultsing executable, without including the source
  code for these in the source distribution:

  a) CodeBase 6.5 or greater from Sequiter Software Inc.

  b) The C or C++ interface to Oracle 8.x or greater, from
     Oracle Corporation or IBM DB2 UDB.


  PROGRAM:


  AUTHORS:
           Elizabeth Burgess and Christopher W.V. Hogue hogue@mshri.on.ca


  REVISION/CONTRIBUTION NOTES:

  June 1, 2000  Original MSH Revision.
                Hogue Lab - University of Toronto Biochemistry
                Department and the Samuel Lunenfeld Research
                Institute, Mount Sinai Hospital
                http://bioinfo.mshri.on.ca  hogue@mshri.on.ca
                ftp://ftp.mshri.on.ca/


cd   PUBLICATION TO CITE:


  DESCRIPTION
  August 2004
  Reads the chrom database and for each record, gets either
  the bioseqset or bioseq and assigns Kloodge ids in taxgi
  for all protein and rna gis derived from that record.

  - requires 1) a full refseq build with a full chrom table.


******************************************************************************************/

#include <intrez_db.h>
#include <parseasn.h>
#include <seqport.h>
#include <intrez_misc.h>
#include <intrez.h>

#define NUMARGS 1
#define MAXINSERTS 10000

Args myargs[ NUMARGS ] =
{
  { "Run in update mode", "F", NULL, NULL, TRUE, 'u', ARG_BOOLEAN, 0.0, 0, NULL}
};

Boolean UPDATE = FALSE;

ValNodePtr SHValNodeFree( ValNodePtr * pHead )
{
  ValNodePtr next;
  ValNodePtr pNode = *pHead;

  while (pNode != NULL)
  {
	next = pNode->next;
	Nlm_MemFree(pNode);
	pNode = next;
  }
  *pHead = NULL;
}

Boolean UpdateTaxgiWithKloodge( StGichromidPtr pGichromid)
{
  StGichromidPtr pRecord = pGichromid;
  Int4 insertCount = 0;
  Boolean success = TRUE;

  while( pRecord != NULL )
  {
	if( !EditTAXGIKloodgeByGi( pRecord->gi, 
							   pRecord->chromid ) )
	{
	  ErrPostEx(SEV_INFO,0,0, "UpdateTaxgiWithKloodge: "
				"Failed to update kloodge for %d.\n", 
				pRecord->gi);
	}

	if( !UPDATE && insertCount > MAXINSERTS )
	{
	  success = CommitAndRollBack();

	  if( !success )
	  {
		ErrPostEx(SEV_ERROR, 0, 0, " UpdateTaxgiWithKloodge: "
				  "Unable to commit transaction." );
		return success;
	  }
		
	  insertCount = 0;
	}
	pRecord = pRecord->next;
  }
  return success; 
}

Boolean ExtractGisFromContigBioseqs( StContigchromidPtr pRecords )
{
  StContigchromidPtr pContigs = pRecords;
  Int4 count = 0;
  Boolean success = TRUE;

  while( pContigs )
  {
	Int2 retValue = 0;
	Int4 gi = pContigs->contiggi;
	Int4 chromid = pContigs->chromid;

	printf( "ExtractGisFromContigBioseqs: extracting %ld\n", gi );

	/*-------------------------------------------------------------
	   Get the bioseq for this gi.
	   ------------------------------------------------------------*/
	BioseqPtr pBioseq = SHoundGetBioseq( gi );

	if( pBioseq && pBioseq->annot )
	{
	  ValNodePtr  pvnGis    = NULL;
	  SeqAnnotPtr pSeqAnnot = pBioseq->annot;

	  /*-------------------------------------------------------------
		Get all gis that belong to this contig and write these to
		gichromid.
		-------------------------------------------------------------*/
	  GetGisFromSeqAnnot( pSeqAnnot, &pvnGis );

	  /*-----------------------------------------------------------
		If the chromid unknown (<0 ), we do not replace any existing
		record that already has a chromid. TODO: maybe introduce
		an update flag and change the value if we're updating.
		----------------------------------------------------------*/
	  if( chromid < 0 )
	  {
		FillGiChromid( pvnGis, gi, chromid );
	  }
	  else
	  {
		ReplaceGiChromid( pvnGis, gi, chromid );
	  }
	  count++;
	  //ValNodeFree( pvnGis );
	  SHValNodeFree( &pvnGis );

	  /*-------------------------------------------------------------
		If this gi has references to other contigs (probably the 
		sequence itself), get those and write those to gichromid. 
		-------------------------------------------------------------*/
	  if( pBioseq->seq_ext_type == DELTA_SEQ )
	  {
		ValNodePtr pvnContigGis = ParseContigs( pBioseq->seq_ext );

		FillGiChromid( pvnContigGis, gi, chromid );
		 
		//ValNodeFree( pvnContigGis );
		SHValNodeFree( &pvnContigGis );
	  }
	  BioseqFree( pBioseq );
	  count++;

	  if( !UPDATE && count > MAXINSERTS )
	  {
		success = CommitAndRollBack();

		if( !success )
		{
		  ErrPostEx(SEV_ERROR, 0, 0, " ExtractGisFromContigBioseqs: "
					"Unable to commit transaction." );
		  return success;
		}

		count = 0;
	  }
	}
	/*----------------------------------------------------------------
	  Mark the contig record as read, so that we don't have to read
	  it again, if we are successful to this point.
	  ------------------------------------------------------------*/
	success = EditRecordCONTIGCHROMIDUpdate( gi, 2 );

	if( !success )
	{
	  ErrPostEx(SEV_ERROR, 0, 0, " ExtractGisFromContigBioseqs: "
					"Unable to update CONTIGCHROMID record" );
	  return success;
	}


	pContigs = pContigs->next;
  }
  return success;
}

Int2 Main()
{

  StContigchromidPtr pContigs = NULL;
  StGichromidPtr     pRecords     = NULL;

  Boolean success = FALSE;

   /* ------------------------------------------------------------
	  check command line arguments 
   ---------------------------------------------------------------*/
   if (!GetArgs("postcomgen",1, myargs))
          return 1;

   if ( myargs[0].intvalue )
	 UPDATE = TRUE;

  if( !InitCodeBase() ) return 1;
  if( !InitNAMES() ) return 1; 
  if( !InitCOMGEN() ) return 1;  

  if (!StartTransaction())
  {
	ErrPostEx(SEV_ERROR, 0, 0, "Main:Cannot start transaction!");
	return 1;
  }

   ErrSetOptFlags(EO_LOG_SEVERITY | EO_LOG_CODES | EO_LOG_FILELINE | EO_LOG_USRFILE);
   ErrSetLogfile("postcomgen.log", ELOG_BANNER);
   ErrSetLogLevel(SEV_INFO);
   ErrSetMessageLevel(SEV_MAX);

   /*--------------------------------------------------------------
	 First add all GICHROMOSOME gis to GICHROMID with their ChromIds.
	 Only add the gi if it does not already exist in GICHROMID.
	 --------------------------------------------------------------*/
   success = AddChromIdsToGICHROMID();

   if( !success )
   {
	 ErrPostEx(SEV_ERROR, 0, 0, "Main: Unable to update gichromid "
			   "with records from gichromosome." );

	 return 1;
   }

   if( !UPDATE )
   {
	 success = CommitAndRollBack();

	 if( !success )
	 {
	   ErrPostEx(SEV_ERROR, 0, 0, "Main: "
					"Unable to commit transaction." );
	   return 1;
	 }
   }

   printf( "GICHROMOSOME records added to GICHROMID\n" );
  /*-----------------------------------------------------------
	 We no longer need GICHROMOSOME
	 ----------------------------------------------------------*/
   DeleteAllGICHROMOSOME();

   /*-----------------------------------------------
	 Remove all genbank records from contig bioseqs.
     --------------------------------------------------------*/
   success = RemoveGenBankRecordsFromCONTIGCHROMID();

   if( !success )
   {
	 ErrPostEx(SEV_ERROR, 0, 0, "Main: Unable2 to remove records "
			   "from contigchromid." );

	 return 1;
   }

   printf( "Genbank records removed from CONTIGCHROMID\n" );

   success = RemoveGenBankRecordsFromCONTIGCHROMOSOME();

   if( !success )
   {
	 ErrPostEx(SEV_ERROR, 0, 0, "Main: Unable to remove records "
			   "from contigchromosome." );

	 return 1;
   }

   if( !UPDATE )
   {
	 success = CommitAndRollBack();

	 if( !success )
	 {
	   ErrPostEx(SEV_ERROR, 0, 0, "Main: "
					"Unable to commit transaction." );
	   return 1;
	 }
   }

   printf( "Genbank records removed from CONTIGCHROMOSOME\n" );

/*--------------------------------------------------------------
	 Add all CONTIGCHROMOSOME gis to CONTIGCHROMID with their 
	 ChromIds. Only add gi if it is not already in CONTIGCHROMID.
	 --------------------------------------------------------------*/
   success = AddChromIdsToCONTIGCHROMID();
 
   if( !success )
   {
	 ErrPostEx(SEV_ERROR, 0, 0, "Main: Unable to add congtigchromosome "
			   "records to contigchromid." );

	 return 1;
   }

   if( !UPDATE )
   {
	 success = CommitAndRollBack();

	 if( !success )
	 {
	   ErrPostEx(SEV_ERROR, 0, 0, "Main: "
					"Unable to commit transaction." );
	   return 1;
	 }
   }

   printf( "CONTIGCHROMOSOME records added to CONTIGCHROMID\n" );
 /*-----------------------------------------------------------
	 We no longer need CONTIGCHROMOSOME
	 ----------------------------------------------------------*/
   DeleteAllCONTIGCHROMOSOME();

   /*----------------------------------------------------------
	 Extract Gis from the contigs and write to GICHROMID.
	 ---------------------------------------------------------*/
   pContigs = GetAllRecordsFromCONTIGCHROMID();   

   if( !UPDATE )
   {
	 success = CommitAndRollBack();

	 if( !success )
	 {
	   ErrPostEx(SEV_ERROR, 0, 0, "Main: "
					"Unable to commit transaction." );
	   return 1;
	 }
   }
   printf( "Got all records from CONTIGCHROMID\n" );

   ExtractGisFromContigBioseqs( pContigs );

   StContigchromidFree( pContigs );

   printf( "Gis extracted from bioseqs.\n" );
   /*------------------------------------------------------------
	 Mark all contigs as read (changed = 2) so that we don't 
	 have to read them again when we update.
	 ----------------------------------------------------------*/
   /*   success = EditRecordCONTIGCHROMIDUpdateAll(); 
   if( !success )
   {
	 ErrPostEx(SEV_ERROR, 0, 0, "Main: Unable to update "
			   "contigchromid." );

	 return 1;
	 }*/

      printf( "CONTIGCHROMID updated.\n" );
   /*----------------------------------------------------------
	 Remove all genbank records from gichromid. Do this how 
	 because the previous steps (reading the bioseqs) could
	 add genbank records to GICHROMID.
	 ---------------------------------------------------------*/
   success = RemoveGenBankRecordsFromGICHROMID();

   printf( "Genbank records removed from GICHROMID\n" );
   /*----------------------------------------------------------
	 Update taxgi.
   ---------------------------------------------------------*/
   pRecords = GetAllRecordsFromGICHROMID();

   if( !UPDATE )
   {
	 success = CommitAndRollBack();

	 if( !success )
	 {
	   ErrPostEx(SEV_ERROR, 0, 0, "Main: "
					"Unable to commit transaction." );
	   return 1;
	 }
   }

   UpdateTaxgiWithKloodge( pRecords );


   /*-------------------------------------------------------------
	 Commit everything at the end, whether we're updating or not.
	 -----------------------------------------------------------*/
   success = CommitAndRollBack();

   if( !success )
   {
	 ErrPostEx(SEV_ERROR, 0, 0, "Main: "
			   "Unable to commit transaction." );
	 return 1;
   }
   StGichromidFree( pRecords );

   printf( "DONE: TAXGI updated.\n" );
   ErrPostEx(SEV_INFO,0,0, "Postcomgen: Done!");
  return 0;
}

/*
$Log: postcomgen.c,v $
Revision 1.3  2005/04/22 17:40:02  eburgess
Mark each contigchromid record as read as its read instead of marking the
entire table as read after everything is done.

Revision 1.2  2005/03/02 16:54:00  eburgess
Added transactional support.

Revision 1.1  2005/02/01 18:32:01  eburgess
Initial revision: new name for updkldge.c

*/
