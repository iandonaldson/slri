/* $Id: parseasn.c,v 1.8 2005/03/29 19:50:09 eburgess Exp $
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
  and distribute the resulting executable, without including the source
  code for these in the source distribution:

  a) CodeBase 6.5 or greater from Sequiter Software Inc.

  b) The C or C++ interface to Oracle 8.x or greater, from
     Oracle Corporation or IBM DB2 UDB.


  PROGRAM:


  AUTHORS:
           Katerina Michalickova and Christopher W.V. Hogue hogue@mshri.on.ca


  REVISION/CONTRIBUTION NOTES:

  June 1, 2000  Original MSH Revision.
                Hogue Lab - University of Toronto Biochemistry
                Department and the Samuel Lunenfeld Research
                Institute, Mount Sinai Hospital
                http://bioinfo.mshri.on.ca  hogue@mshri.on.ca
                ftp://ftp.mshri.on.ca/


  PUBLICATION TO CITE:


  DESCRIPTION
  November 1998
  SeqHound system API

    July 1999
    added a new field to the ACCDB database: version of accession to reflect the
    NCBI versoning system
    September 1999
    added the REDUND database and took redundant info out of TAXGIFIN and TAXGI
    TAXGIFIN taken out of the system
    October 1999
    title in ACCDB now stored in Memo field
    June 2000
    removed all remote functions, they live in ambiseqhoundapi.c only the lighweight
    (no local obtion) version is maintained

******************************************************************************************/
#include <parseasn.h>

/*====================================================================
  Functions used to parse data from bioseqs. Used by mother.c and 
  updkldge.c in genomes.
  ====================================================================*/
/***************************************************************/ 
/** GetGisFromSeqFeat
 *  
 *  Extracts gis from a SeqFeatPtr. Only extracts the gis from 
 *  SeqFeatPtrs of type cdregion or rna.
 *
 * @param pSeqFeat    The SeqFeatPtr containing the gis.
 *
 * @param ppvnGiList  A pointer to the valnode list that will
 *                    hold the gis.
 *
 */
void GetGisFromSeqFeat( SeqFeatPtr pSeqFeat, 
						ValNodePtr *ppvnGiList )
{
  SeqLocPtr  pSeqLoc  = NULL;
  Int4 gi = 0;

  if( NULL == pSeqFeat || NULL == ppvnGiList )
  {
	ErrPostEx(SEV_ERROR,0,0, "GetGisFromSeqFeat: "
			  "Null SeqFeatPtr or ValNodePtr." );
	return;
  }
  /* SEQFEAT_CDREGION holds proteins, 
	 SEQFEAT_RNA holds RNAs */
  if((pSeqFeat->data.choice == SEQFEAT_CDREGION) || 
	 (pSeqFeat->data.choice == SEQFEAT_RNA))
  {
	if (pSeqFeat->product != NULL)
	{
	  pSeqLoc = (SeqLocPtr) pSeqFeat->product;

	  gi = GetGiFromProductLocation( pSeqLoc );

	  if( gi > 0 )
	  {
		ValNodeAddInt( ppvnGiList, 0, gi );
	  }
	  else
	  {
		ErrPostEx(SEV_ERROR,0,0, "GetGisFromSeqFeat: "
                  "No gi found for CDRegion or RNA.");
		  
	  }
		
	  /*---------------------------------------------------------
		 If this is an mRNA. os there a reference to the protein?
		--------------------------------------------------------*/
	  /*(  gi = GetGiFromUserObject( pSeqFeat->ext );
	  
	  if( gi > 0 )
      {
		ValNodeAddInt( ppvnGiList, 0, gi );
		}*/
	}
  }
}


Int4 GetGiFromUserObject( UserObjectPtr pExt )
{
#ifdef MORE_BUGS
  Int4 gi = 0;
  UserObjectPtr pObject = pExt;
  UserFieldPtr  pUserField = NULL;
  UserFieldPtr  pUserFieldData = NULL;
  CharPtr pString = NULL;

  if( !pExt ) return;

  while( pObject )
  {
	if( StringCmp( pObject->_class, 
				   "CombinedFeatureUserObjects" ) == 0 )
    {
	  pUserField = pObject->data;

	  while( pUserField )
      {
		if( pUserField->data )
        {
		  pUserFieldData = pUserField->data;

		  if( StringCmp( pUserFieldData, "protein seqID" )
		  {
			
		  }
		}
	  }
	}
  }
#endif
	return 1;
}
/**************************************************************
      GetGiFromProductLocation:
      Find the gi from a SeqLocPtr.

      Parameters: psl - the input SeqLocPtr from which to extract
                        the gi. Handles SeqLocs of type Whole and
                        Int.

      Returns the gi on success, 0 on failure.
*************************************************************/
Int4 GetGiFromProductLocation( SeqLocPtr psl )
{
  SeqLocPtr pSeqLoc   = psl;
  Int4      gi        = 0;
  /* Int4      giLast    = 0; */
  SeqIdPtr  pSeqID    = NULL;
  SeqIntPtr pSeqInt   = NULL;

  while( pSeqLoc != NULL && gi == 0)
  {
	if (pSeqLoc->choice == SEQLOC_WHOLE)
    {
	  pSeqID = pSeqLoc->data.ptrvalue; /*a seq-id struct is of type valnode */
	  if (pSeqID == NULL)
      {
		ErrPostEx(SEV_ERROR,0,0, "GetGiFromLocation: NULL SeqId pointer.");
	  }
		
	  gi = GetGiFromSeqId( pSeqID );
	}
	  
	if (pSeqLoc->choice == SEQLOC_INT )
    {
	  pSeqInt = pSeqLoc->data.ptrvalue; /*a seq-interval struct is of type valnode */
	  if (pSeqInt == NULL)
      {
		ErrPostEx(SEV_ERROR,0,0, "GetGiFromLocation: NULL Seq-Int pointer.");
	  }
	  else
      {
		/*get gi of source DNA*/
		pSeqID = pSeqInt->id; /*WARNING: Seq-id used to be optional in the Seq-interval struct*/
		gi = GetGiFromSeqId( pSeqID );
	  }
	}
	
	pSeqLoc = pSeqLoc->next;
  }

  return gi;
}

/***************************************************************/ 
/** GetGiFromSeqId
 *
 *  Extract the gi from a list of SeqIds.
 *
 *  @param pSeqId The SeqIds from which to extract the gi.
 *
 *  @return       The gi.
 *
 */
Int4 GetGiFromSeqId( SeqIdPtr pSeqId )
{
  Int4       gi       = 0;

  while (pSeqId != NULL)
  {
	if (pSeqId->choice == SEQID_GI)
	{
	  gi = pSeqId->data.intvalue;
	  if (gi <= 0)
	  {
		ErrPostEx(SEV_ERROR,0,0, "GetGiFromSeqId: Invalid GI found.");
	  }
	  else
      {
		break;
	  }
	}
	pSeqId = pSeqId->next;
  }
  return gi;
}

/***************************************************************/ 
/** IsTextSeqId
 *
 *  Is the given SeqId a text seqid. i.e from genbank,embl
 *  pir, swissprot, ddbj, prf, tpe, tpg, tpd.
 *
 *  @param pSeqId The SeqId.
 *
 *  @return       true or false.
 *
 */
Boolean IsTextSeqId( SeqIdPtr pSeqId )
{
  Boolean isText = FALSE;

  if ( pSeqId->choice == SEQID_GENBANK || 
	   pSeqId->choice == SEQID_EMBL ||
	   pSeqId->choice == SEQID_PIR ||
	   pSeqId->choice == SEQID_SWISSPROT ||
	   pSeqId->choice == SEQID_DDBJ ||
	   pSeqId->choice == SEQID_PRF ||
	   pSeqId->choice == SEQID_TPE ||
	   pSeqId->choice == SEQID_TPG ||
	   pSeqId->choice == SEQID_TPD ||
	   pSeqId->choice == SEQID_OTHER )
  {
	isText = TRUE;
  }
  
  return isText;
}

/***************************************************************/ 
/** GetDbFromSeqId
 *
 *  Extract the db name from a SeqIds.
 *
 *  @param pSeqId The SeqIds from which to extract the name.
 *
 *  @return       The db name.
 *
 */
CharPtr GetDbFromSeqId( SeqIdPtr pSeqId, Boolean refseq )
{
  CharPtr      dbName = NULL;
  Int4         seqIdChoice = pSeqId->choice;

  switch( seqIdChoice )
  {

     case SEQID_GENBANK:
	   dbName = StringSave( "gb" );
	   break;
     case SEQID_EMBL:
	   dbName = StringSave( "embl" );
	   break;
     case SEQID_PIR:
	   dbName = StringSave( "pir" );
	   break;
     case SEQID_PDB:
	   dbName = StringSave( "pdb" );
	   break;
     case SEQID_SWISSPROT:
	   dbName = StringSave( "sp" );
	   break;
     case SEQID_DDBJ:
	   dbName = StringSave( "dbj" );
	   break;
     case SEQID_PRF:
	   dbName = StringSave( "prf" );
	   break;
     case SEQID_TPE:
	   dbName = StringSave( "tpe" );
	   break;
     case SEQID_TPG:
	   dbName = StringSave( "tpg" );
	   break;
     case SEQID_TPD:
	   dbName = StringSave( "tpd" );
	   break;
     case SEQID_GIBBSQ:
	   dbName = StringSave( "bbs" );
	   break;
     case SEQID_GIBBMT:
	   dbName = StringSave( "bbm" );
	   break;
     case SEQID_OTHER:
	   if( refseq )
	   {
		 dbName = StringSave( "ref" );
	   }
	   else
	   {
		 dbName = StringSave( "other" );
	   }
	   break;
     default:
	   dbName = NULL;
	   /*	   ErrPostEx(SEV_WARNING,0,0, "GetDbFromSeqId: "
			   "Unknown SeqId type %d.", seqIdChoice );*/
  }

  return dbName;
}

/***************************************************************/ 
/** GetInfoFromSeqId()
 *
 *  Extracts information from a single seqid.
 *
 *  @param pSeqId The seqid.
 *
 *  @return      An StAccdbPtr containing the information. Must
 *               be freed by the caller.
 *
 */
StAccdbPtr GetInfoFromSeqId( SeqIdPtr pSeqId, Boolean refseq )
{
  StAccdbPtr   pInfo  = StAccdbNew();
  TextSeqIdPtr pText  = NULL;
  PDBSeqIdPtr  pPDB   = NULL;
  
  if( !pInfo )
  {
	ErrPostEx(SEV_ERROR,0,0, "GetInfoFromSeqId: "
			  "Unable to allocate StAccdbPtr.");
	return NULL;
  }

  if( pSeqId == NULL )
  {
	ErrPostEx(SEV_ERROR,0,0, "GetInfoFromSeqId: "
			  "SeqId pointer is NULL.");
	return NULL;
  }

  
  if( pSeqId->choice == SEQID_GI )
  {
	pInfo->gi = pSeqId->data.intvalue;
	if( pInfo->gi == 0 )
	{
	  ErrPostEx(SEV_ERROR,0,0, "GetInfoFromSeqId: "
				"Invalid GI.");
	  pInfo = NULL;
	}		
  }
  else
  {
	/*--------------------------------------------------
	  Set the DB here for all SeqIds.
	  --------------------------------------------------*/
	pInfo->db = GetDbFromSeqId( pSeqId, refseq );
  }

  if( IsTextSeqId( pSeqId ) )
  {
	pText =  pSeqId->data.ptrvalue;
	
	if( pText )
	{
	  if( pText->accession != NULL )
	  {
		pInfo->access = StringSave( pText->accession );
	  }
		
	  if( pText->version != INT2_MIN )
	  {
		pInfo->version = pText->version;
	  }
	  else
	  {
		ErrPostEx(SEV_ERROR,0,0, "GetInfoFromSeqId: No version.");
	  }

	  if( pText->release != NULL )
         pInfo->release = StringSave( pText->release ); 

	  if( pText->name != NULL )
	  {
		pInfo->name = StringSave( pText->name );
		pInfo->namelow = StringSave( pText->name );
	  }
	}
  }

  if( pSeqId->choice == SEQID_PDB )
  {
	Char         chain[5];
	Char         date[20];

	pPDB = pSeqId->data.ptrvalue;
	
	if( pPDB )
	{
	  if( pPDB->mol != NULL )
	  {
		pInfo->name    = StringSave( pPDB->mol ); 
		pInfo->namelow = StringSave( pPDB->mol ); 
	  }

	  if( ( pPDB->chain != CHAIN_NOT_SET) && 
		  ( pPDB->chain != CHAIN_DEFAULT ) )
	  {
		sprintf( chain, "%c", (char) pPDB->chain );

		pInfo->chain = StringSave( chain );
	  }

	  if( pPDB->rel != NULL )
	  {
		if( DatePrint( pPDB->rel, date ) )
		  pInfo->release = StringSave( date );
		 else
          ErrPostEx(SEV_ERROR,0,0, "GetInfoFromSeqId: "
					"DatePrint failed.");
	  }
	}
  }
  if( pSeqId->choice == SEQID_GIBBSQ ||
	  pSeqId->choice == SEQID_GIBBMT )  
  {
	if( pInfo->db )
	{
	  Int4 id = pSeqId->data.intvalue;
		
	  Char idstr[ 10 ];

	  sprintf(idstr, "%ld", (long) id);

	  pInfo->name = StringSave( idstr );
	  pInfo->namelow = StringSave( idstr );
	  
	}
  }
  if (pSeqId->choice == SEQID_GENERAL)  /* General dbtag for other databases */
  {
	DbtagPtr pDbTag = pSeqId->data.ptrvalue;

	if( pDbTag )
	{
	  if( pDbTag->db != NULL )
		pInfo->db = StringSave( pDbTag->db );

	  if( pDbTag->tag != NULL )
	  {
		if( pDbTag->tag->str != NULL )
		
{		  pInfo->name = StringSave( pDbTag->tag->str );
		  pInfo->namelow = StringSave( pDbTag->tag->str );
		}
	  }
	  else
	  {
		Char idstr[ 10 ];

		Int4 id = pDbTag->tag->id;

		sprintf(idstr, "%ld", (long) id);

		pInfo->name = StringSave( idstr );
		pInfo->namelow = StringSave( idstr );
	  }
	}
	   
  }
  return pInfo;
}
/***************************************************************/ 
/** ParseContigs
 *
 *  Extracts contigs gis from a valnode list of SeqLocPtrs. 
 *
 *  @param pSeq  List of SeqLocPtrs.
 *
 *  @return      a list of the extracted gis.
 *
 */
ValNodePtr ParseContigs( ValNodePtr pSeq )
{
  ValNodePtr pDeltaSeq     = pSeq;
  ValNodePtr pvnContigGis  = NULL;
  SeqLocPtr  pSeqLoc       = NULL;

  while( pDeltaSeq )
  {
	if( pDeltaSeq->choice == SEQ_LOC )
    {
	  /*	  GetGisFromSeqLoc( pDeltaSeq->data.ptrvalue, &pvnGis, 
			  SEQLOC_INT );*/
	  Int4 gi 
		= GetGiFromProductLocation( pDeltaSeq->data.ptrvalue );

	  /*---------------------------------------------------------
	  only add the gi if we haven't already written to GiChromid.
	  ---------------------------------------------------------*/
	  if( gi > 0 )
	  {
		ValNodeAddInt( &pvnContigGis, 0, gi );
	  }
	  else
	  {
		ErrPostEx(SEV_ERROR,0,0, "ParseContigs: Invalid GI found.");
	  }
	}
	pDeltaSeq = pDeltaSeq->next;
  }
  return pvnContigGis;
}

void GetGisFromSeqAnnot( SeqAnnotPtr pSeqAnnot, 
						 ValNodePtr *ppvnGis)
{
  SeqFeatPtr pSeqFeat = NULL;

  while (pSeqAnnot != NULL)
  {
	if (pSeqAnnot->type == ANNOT_TABLE) /* ftable */
	{
	  pSeqFeat = (SeqFeatPtr) pSeqAnnot->data;

	  while( pSeqFeat )
	  {
		GetGisFromSeqFeat( pSeqFeat, ppvnGis );
		
		pSeqFeat = pSeqFeat->next;
	  }
	}
	pSeqAnnot = pSeqAnnot->next;
  }
}

/***************************************************************/ 
/** FillGichromid
 *
 *  Writes all gis in the ValNodePtr to gichromid along with
 *  the chromid. If the gi already exists in gichromid, then
 *  nothing is done.
 *
 * @param pvnGis      List of gis to be written to gichromid.
 *
 * @param chromid      The ncbi chromid to be written to the table.
 *
 */  
void FillGiChromid( ValNodePtr pvnGis,Int4 contigGi, Int4 chromId )
{
  ValNodePtr pGis = pvnGis;
  StGichromidPtr pRecord = NULL;

  /*------------------------------------------------------------
	Do nothing if the chromId is 0.
	----------------------------------------------------------*/
  if( chromId == 0 )
  { 
	return;
  }

  pRecord = StGichromidNew();

  if ( pRecord == NULL )
  {
	ErrPostEx(SEV_INFO,0,0, "FillGiChromid: "
			  "StGichromidNew failed.\n");
  }
  else
  {
	while( pGis != NULL )
	{
	  pRecord->gi = pGis->data.intvalue;
	  pRecord->contiggi = contigGi;
	  pRecord->chromid = chromId;

	  if( !AppendRecordGICHROMID(pRecord ) )
		ErrPostEx(SEV_INFO,0,0, "FillGiChromid: "
				  "Failed to update chromid for %d.\n", pRecord->gi);

	  ResetStGICHROMID( &pRecord );

	  pGis = pGis->next;
	}
	StGichromidFree(pRecord);
  }
}


/***************************************************************/ 
/** ReplaceGichromid
 *
 *  Writes all gis in the ValNodePtr to gichromid along with
 *  the chromid. If the gi already exists in gichromid, then
 *  that record is deleted and a new record is added.
 *
 * @param pvnGis      List of gis to be written to gichromid.
 *
 * @param chromid      The ncbi chromid to be written to the table.
 *
 */  
void ReplaceGiChromid( ValNodePtr pvnGis, Int4 contigGi, Int4 chromId )
{
  ValNodePtr pGis = pvnGis;
  StGichromidPtr pRecord = NULL;

  /*------------------------------------------------------------
	Do nothing if the chromId is 0.
	----------------------------------------------------------*/
  if( chromId == 0 )
  { 
	return;
  }

  pRecord = StGichromidNew();

  if ( pRecord == NULL )
  {
	ErrPostEx(SEV_INFO,0,0, "ReplaceGichromid: "
			  "StGichromidNew failed.\n");
  }
  else
  {
	while( pGis != NULL )
	{
	  pRecord->gi = pGis->data.intvalue;
	  pRecord->contiggi = contigGi;
	  pRecord->chromid = chromId;

	  if( !ReplaceRecordGICHROMID(pRecord ) )
		ErrPostEx(SEV_INFO,0,0, "ReplaceGichromid: "
				  "Failed to update chromid for %d.\n", pRecord->gi);

	  ResetStGICHROMID( &pRecord );

	  pGis = pGis->next;
	}
	StGichromidFree(pRecord);
  }
}

/*ValNodePtr SHValNodeFree( ValNodePtr * pHead )
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
  }*/


/*
$Log: parseasn.c,v $
Revision 1.8  2005/03/29 19:50:09  eburgess
Added pdb to the list of database names retruned by GetDbFromSeqId().

Revision 1.7  2005/03/16 20:33:17  eburgess
Use StringSave to save the db name in GetInfoFromSeqId.

Revision 1.6  2005/03/15 21:15:04  eburgess
Modified GetDbFromSeqId and GetInfoFromSeqId to handle the Refseq case.

Revision 1.5  2005/03/14 20:34:06  eburgess
Fixed typo in GetInfoFromSeqId().

Revision 1.4  2005/03/14 16:04:14  eburgess
Added functions to parse names and accessions out of seqids. Used for updates
Bugs 4939 and 5049.

Revision 1.3  2005/02/01 16:50:04  eburgess
Changed gikloodge and contigkloodge to gichromid and contigchromid.

Revision 1.2  20 05/01/10 19:28:36  eburgess
Added ReplaceGikloodge() and made other changes for comgen.

Revision 1.1  2004/11/08 20:14:38  eburgess
Initial revision.


*/
