/* $Id: ll_query.c,v 1.16 2005/04/26 19:07:08 eburgess Exp $
***************************************************************************
  Copyright (C) 2001 Mount Sinai Hospital (MSH)

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
            Hao Lieu


  REVISION/CONTRIBUTION NOTES:

  June 1, 2001  Original MSH Revision.
                Hogue Lab - University of Toronto Biochemistry 
                Department and the Samuel Lunenfeld Research 
                Institute, Mount Sinai Hospital
                http://bioinfo.mshri.on.ca  hogue@mshri.on.ca
                ftp://ftp.mshri.on.ca/


  PUBLICATION TO CITE:

  DESCRIPTION:
  Locus Link database query
  Part of SeqHound

****************************************************************************/
#include <asn.h>
#include <objslristruc.h>
#include <ll_query.h>
#include <ll_db.h>
#include <slri_misc.h>
#include <intrez_cfg.h>
#include <seqhound.h>

/****************************************************************
  Define database names found in the Entrez gene file gene_info.
  NOTE: only OMIM is used presently.
***************************************************************/
#define OMIM "mim"

/**************************************************************
                 PUBLIC INITIALIZATION FUNCTIONS
***************************************************************/

/* Opens all the database files needed for querying. This function 
   must be called prior to any queries to the database can be
   properly executed. Call this at the start of your program. 
   DBpath is the path to the database files. Do not include the 
   file names in the path. */
Boolean LLDB_OpenAllDBs(Boolean CREATE)
{
  return LL_OpenAllDBs(CREATE);
}

/* initializes all the database fields and tags. This funciton 
   must be called prior to any queries to the database can be
   properly executed. Call this at the start of your program. */
void LLDB_InitCodeBase(void)
{
  LL_InitCodeBase(TRUE);
}

/* closes the database files and flushes all buffers.
   at the end of your program you must call this function. */
void LLDB_InitUndoCodeBase(void)
{
  LL_InitUndoCodeBase();
}

/****************************************************************
		     PUBLIC PRINT FUNCTIONS
*****************************************************************/

/* prints all the int data in the records in vnp. */
void LLDB_PrintIntList(ValNodePtr vnp)
{
  ValNodePtr v = NULL;

  if(vnp == NULL){
    ErrPostEx(SEV_ERROR, 0, 0, "LLDB_PrintIntList: Passed ValNodePtr is NULL.");
    return;
  }
  v = vnp;
  while(v != NULL){
    printf("%d\n", v->data.intvalue);
    v = v->next;
  }
}

/* prints the ptr value data in vnp */
void LLDB_PrintECode(ValNodePtr vnp)
{
  ValNodePtr v = NULL;
  if(vnp == NULL){
    ErrPostEx(SEV_ERROR, 0, 0, "LLDB_PrintECode: Passed ValNodePtr is NULL.");
    return;
  }

  v = vnp;
  while(v != NULL){
    printf("%s\n", (CharPtr)IsNullStr(v->data.ptrvalue));
    v = v->next;
  }
}


/******************************************************************
                   PUBLIC RETRIEVAL FUNCTIONS
*******************************************************************/

ValNodePtr LIBCALL SHoundGOIDFromLLID ( Int4 llid )
{
   ValNodePtr listGOID       = NULL;
   StGOdbNodePtr   pgo       = NULL,
                 pgoHead     = NULL;

   /* if ( !SHoundModule ( "lldb" ) )
   {
     ErrPostEx ( SEV_ERROR, 0, 0, 
		 "[SHoundGOIDFromLLID]: Missing required SeqHound module." );
     return NULL;
   } */
   /* if ( llid <= 0 )
   {
      ErrPostEx ( SEV_ERROR, 0, 0, "[SHoundGOIDFromLLID]: Invalid llid.");
      return NULL;
   } */

   pgoHead = StGOdbNew ();
   pgoHead->llid = llid;
   
   if ( LL_SUCCESS == LL_SearchGODB ( &pgoHead ) )
   {
      for ( pgo = pgoHead; NULL != pgo; pgo = pgo->next )
         { Slri_ValNodeAddInt ( &listGOID, 0, pgo->goid ); }
   }

   StGOdbFree ( pgoHead );
   if ( NULL != listGOID ) Misc_IntVnpUnique ( &listGOID );
   return listGOID;
}

Int4 LIBCALL SHoundOMIMFromLLID ( Int4 llid )
{
  ValNodePtr pAccess = NULL;
  Int4 omim          = 0;

  if ( 0 >= llid )
  {
	ErrPostEx ( SEV_ERROR, 0, 0, "[SHoundOMIMFromLLID]: "
				"Invalid llid.");
	return -1;
  }
  
  pAccess = GetGENEDbxrefFromGeneid( llid, "mim" );

  if( pAccess )
  {
	omim = atoi( pAccess->data.ptrvalue );
  }

  return omim;
}

ValNodePtr LIBCALL SHoundCDDIDFromLLID ( Int4 llid )
{
   ValNodePtr listCDD           = NULL;
   StCDDdbNodePtr   pcdd         = NULL,
                  pcddHead       = NULL;
   CharPtr pc                    = NULL;

   /* if ( !SHoundModule ( "lldb" ) )
   {
     ErrPostEx ( SEV_ERROR, 0, 0, 
		 "[SHoundCDDIDFromLLID]: Missing required SeqHound module." );
     return NULL;
   } */
   /* if ( llid <= 0 )
   {
      ErrPostEx ( SEV_ERROR, 0, 0, "[SHoundCDDIDFromLLID]: Invalid llid.");
      return NULL;
   } */

   pcddHead = StCDDdbNew ();
   pcddHead->llid = llid;
   
   if ( LL_SUCCESS == LL_SearchCDDDB ( &pcddHead ) )
   {
      for ( pcdd = pcddHead; NULL != pcdd; pcdd = pcdd->next )
      {
         pc = MemNew( (StringLen(pcdd->cddid)+1)*sizeof(Char) );
         StringCpy(pc, pcdd->cddid);
         Slri_ValNodeAddStr(&listCDD, 0, pc);
      }
   }

   StCDDdbFree ( pcddHead );
   if ( NULL != listCDD ) Misc_StringVnpUniqueAndFree ( &listCDD );
   return listCDD;
}


/**
 * retrieves a list of locus link id given 
 * a gene ontology id and evidence code
 */
ValNodePtr LIBCALL SHoundLLIDFromGOIDAndECode ( Int4 goid, CharPtr evidence )
{
   StGOdbNodePtr pgo  = NULL,
              pgoHead = NULL;
   ValNodePtr vnp     = NULL;

  /* if(!SHoundModule("lldb"))
   {
     ErrPostEx(SEV_ERROR,0,0, "[SHoundLLIDFromGOIDAndECode]: Missing required SeqHound module.");
     return NULL;
   } */

   if ( 0 >= goid || NULL == evidence ) 
   {
      ErrPostEx ( SEV_ERROR, 0, 0, 
		  "[SHoundLLIDFromGOIDAndECode]: Invalid inputs.");
      return NULL;
   } 
   
   pgoHead = StGOdbNew ();
   pgoHead->goid = goid;

   if ( LL_SUCCESS == LL_SearchGODB(&pgoHead) )
   {
      for ( pgo = pgoHead; pgo != NULL; pgo = pgo->next )
      {
         if ( 0 < pgo->llid && 
	      ( 0 == strcmp ( evidence, "" ) ||
                0 == strcmp ( evidence, pgo->eviCode )) 
	    )
         {
	    Slri_ValNodeAddInt (&vnp, 0, pgo->llid);
         } 
      }
   }

   StGOdbFree ( pgoHead );
   if (NULL != vnp) Misc_IntVnpUnique (&vnp);
   return vnp;
}

ValNodePtr LIBCALL SHoundLLIDFromOMIM ( Int4 omim )
{
   ValNodePtr   vnpGeneids     = NULL;
   CharPtr      access[ 30 ]; 

   if ( 0 >= omim )
   {
      ErrPostEx ( SEV_ERROR, 0, 0, "[SHoundLLIDFromOMIM]: "
				  "Invalid llid.");
      return NULL;
   }

   sprintf( (char *)access, "%ld", omim );

   vnpGeneids = GetGeneidFromGENEDbxref( (char *)"mim", 
										 (char *)access );
   
   return vnpGeneids;
}

ValNodePtr LIBCALL SHoundLLIDFromCDDID ( CharPtr cddid )
{
  StCDDdbPtr pCDD = NULL,
       pCDDHead   = NULL;
  ValNodePtr vnp  = NULL;

  /* if(!SHoundModule("lldb"))
  {
     ErrPostEx(SEV_ERROR,0,0, "SHoundGiFromCDDID: Missing required SeqHound module.");
     return NULL;
  } */

   if ( NULL == cddid || !StrCmp(cddid, "") )
   {
      ErrPostEx(SEV_ERROR, 0, 0, "SHoundGiFromCDDID: Invalid cdd id.");
      return NULL;
   }

   pCDDHead = StCDDdbNew ();
   pCDDHead->cddid = StrSave(cddid);
   
   if ( LL_SUCCESS == LL_SearchCDDDB(&pCDDHead) )
   {
      for ( pCDD = pCDDHead; pCDD != NULL; pCDD = pCDD->next )
         { Slri_ValNodeAddInt ( &vnp, 0, pCDD->llid ); }
   }

   StCDDdbFree (pCDDHead);
   if ( NULL != vnp ) Misc_IntVnpUnique( &vnp );
   return vnp;
}

/* 
 * returns a linked list of the gi of records with GO goid. It is
 * the callers responsibility to free this link list. Returns NULL
 * if an error occurs. 
 */
ValNodePtr LIBCALL SHoundGiFromGOID(Int4 goid)
{
  /* if(!SHoundModule("lldb"))
   {
     ErrPostEx(SEV_ERROR,0,0, "[SHoundGiFromGOID]: Missing required SeqHound module.");
     return NULL;
   } */

  if (goid < 0)
  {
    ErrPostEx(SEV_ERROR,0,0, "[SHoundGiFromGOID]: Invalid goid.");
    return NULL;
  }

  return SHoundGiFromGOIDAndECode (goid, "");
}

/* returns the gi with the specified goid and evidence code. Consult
 * www.geneontology.org for valid evidence code, eg "ND", "IPI" etc. */
ValNodePtr LIBCALL SHoundGiFromGOIDAndECode(Int4 goid, CharPtr EvidenceCode)
{
   ValNodePtr vnpgi = NULL;      /* the list to be returned */
   StGOdbNodePtr pgo = NULL,
            pgoHead  = NULL;
   StLLdbNodePtr pgi = NULL,
            pgiHead  = NULL;
	      
   /* if(!SHoundModule("lldb"))
   {
      ErrPostEx ( SEV_ERROR, 0, 0, 
                 "[SHoundGiFromGOIDAndECode]: Missing required SeqHound module.");
      goto end;
    } */

    if ( goid <= 0 || EvidenceCode == NULL )
    {
      ErrPostEx(SEV_ERROR,0,0, "[SHoundGiFromGOIDAndECode]: Invalid input.");
      goto end;
    }

    pgoHead = StGOdbNew(); 
    pgoHead->goid = goid;
    if ( LL_SUCCESS == LL_SearchGODB(&pgoHead) )
    {
       /* get llid from go db */
       for ( pgo = pgoHead; pgo != NULL; pgo = pgo->next )
       {
         if ( 0 == strcmp ( EvidenceCode, "" ) ||
              0 == strcmp ( EvidenceCode, pgo->eviCode) )
         {
            /* get gi using llid */
	    pgiHead = StLLdbNew();
	    pgiHead->llid = pgo->llid;
            if ( LL_SUCCESS == LL_SearchLLDB (&pgiHead) )
	    {
               for ( pgi = pgiHead; NULL != pgi; pgi = pgi->next )
	          { Slri_ValNodeAddInt (&vnpgi, 0, pgi->npid); }
	    }
	    StLLdbFree(pgiHead);
         } 
       }

    }
    StGOdbFree(pgoHead);
    if ( NULL != vnpgi ) Misc_IntVnpUnique( &vnpgi );
    return vnpgi; 
end:
    ErrPostEx ( SEV_ERROR, 0, 0, 
	        "[SHoundGiFromGOIDAndECode]: Error with goid %d.", goid);
    return NULL;
}

/* returns a linked list of the Gi of the records with goid in vnpgo.
   It is the callers responsibility to free this link list. Returns NULL
   if an error occurs or if goids not found. */
ValNodePtr LIBCALL SHoundGiFromGOIDList(ValNodePtr vnpgo)
{
  ValNodePtr v = NULL,      /* tmp search variables */
  vnpgi = NULL,                        /* our return value */
  vgi = NULL;                          /* results of search function */


  /* if(!SHoundModule("lldb"))
  {
     ErrPostEx(SEV_ERROR,0,0, "SHoundGiFromGOIDList: Missing required SeqHound module.");
     return NULL;
  } */

  if (vnpgo == NULL){
    ErrPostEx(SEV_ERROR, 0, 0, "SHoundGiFromGOIDList: vnpgo is NULL.");
    return NULL;
  }

  for ( v = vnpgo; v != NULL; v = v->next )
  {
    /* one by one we search for each goid in vnpgo */
    vgi = SHoundGiFromGOID(v->data.intvalue);
    if ( vgi != NULL ) ValNodeLink(&vnpgi, vgi);
  }
  if (NULL != vnpgi) Misc_IntVnpUnique(&vnpgi);      /* uniques the ints we return */
  return vnpgi;
}

ValNodePtr LIBCALL SHoundGiFromGOIDListAndECode(ValNodePtr vnpgo, CharPtr EvidenceCode)
{
  ValNodePtr v = NULL,      /* tmp search variables */
  vnpgi = NULL,                        /* our return value */
  vgi = NULL;                          /* results of search function */

  /* if(!SHoundModule("lldb"))
  {
     ErrPostEx(SEV_ERROR,0,0, "SHoundGiFromGOIDList: Missing required SeqHound module.");
     return NULL;
  } */

  if ( vnpgo == NULL || EvidenceCode == NULL )
  {
    ErrPostEx(SEV_ERROR, 0, 0, "SHoundGiFromGOIDList: Invalid input.");
    return NULL;
  }

  for ( v = vnpgo; v != NULL; v = v->next )
  {
    /* one by one we search for each goid in vnpgo */
    vgi = SHoundGiFromGOIDAndECode(v->data.intvalue, EvidenceCode);
    if ( NULL != vgi ) ValNodeLink ( &vnpgi, vgi );
  }
  if (NULL != vnpgi) Misc_IntVnpUnique(&vnpgi);      /* uniques the ints we return */
  return vnpgi;
}

/* returns a linked list of goid of records with gi. It is the
   callers responsibility to free the vnp. Returns NULL if an
   error occured or if gi not found. */
ValNodePtr LIBCALL SHoundGOIDFromGi(Int4 gi)
{
  ValNodePtr vnpgo = NULL;          /* our return value */
  StLLdbPtr pll = NULL;
  /* if(!SHoundModule("lldb"))
  {
     ErrPostEx ( SEV_ERROR, 0, 0, 
                 "SHoundGiFromGOIDList: Missing required SeqHound module." );
     return NULL;
  } */

  if ( 0 >= gi )
  {
    ErrPostEx ( SEV_ERROR, 0, 0, 
		"[SHoundGOIDFromGi]: Invalid gi." );
    return NULL;
  }

  pll = StLLdbNew ();
  pll->npid = gi;
  if ( LL_SUCCESS == LL_SearchLLDB(&pll) )
  {
     vnpgo = SHoundGOIDFromLLID ( pll->llid );   
  }
  StLLdbFree ( pll );
  return (ValNodePtr) vnpgo;
}


/* obtain a group of redundant GIs and then the GO annotation */
ValNodePtr LIBCALL SHoundGOIDFromRedundantGi(Int4 gi)
{
   ValNodePtr pvngroup = NULL,pvnres = NULL;

   if((!SHoundModule("lldb")) || (!SHoundModule("redundb")))
   {
     ErrPostEx(SEV_ERROR,0,0, "SHoundGOIDFromRedundantGi: Missing required SeqHound module.");
     return NULL;
   }
   pvngroup = SHoundRedundantGroup(gi);
   if(pvngroup == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, "SHoundGOIDFromRedundantGi: SHoundRedundantGroup failed.");
      return NULL;
   }
   pvnres = SHoundGOIDFromGiList(pvngroup);
   ValNodeFree(pvngroup);
   return pvnres;
}

/* returns a linked list of goid from records with a gi in vnpgi.
   Caller has to free the return value. */
ValNodePtr LIBCALL SHoundGOIDFromGiList(ValNodePtr vnpgi)
{
  ValNodePtr vnpgo = NULL,      /* return value of each gi search */
  vgi = NULL,                 /* temp variable */
  vnpgoid = NULL;             /* our return value */

  /* if(!SHoundModule("lldb"))
  {
    ErrPostEx(SEV_ERROR,0,0, "SHoundGOIDFromGiList: Missing required SeqHound module.");
    return NULL;
  } */

  if ( NULL == vnpgi )
  {
    ErrPostEx ( SEV_ERROR, 0, 0, 
		"[SHoundGOIDFromGiList]: NULL input parameter.");
    return NULL;
  }

  for ( vgi = vnpgi; vgi != NULL; vgi = vgi->next )
  {
    vnpgo = SHoundGOIDFromGi(vgi->data.intvalue);
    if ( vnpgo != NULL ) ValNodeLink ( &vnpgoid, vnpgo );
  }
  if ( vnpgoid != NULL) Misc_IntVnpUnique(&vnpgoid);
  return vnpgoid;
}

/* obtain a group of redundant GIs and then the GO annotations */
ValNodePtr LIBCALL SHoundGOIDFromRedundantGiList(ValNodePtr vnpgi)
{
   ValNodePtr pvngroup = NULL,pvnres = NULL;

   if((!SHoundModule("lldb")) || (!SHoundModule("redundb")))
   {
     ErrPostEx(SEV_ERROR,0,0, "SHoundGOIDFromRedundantGiList: Missing required SeqHound module.");
     return NULL;
   }
   pvngroup = SHoundRedundantGroupList(vnpgi);
   if(pvngroup == NULL)
   {
      ErrPostEx(SEV_ERROR,0,0, "SHoundGOIDFromRedundantGiList: SHoundRedundantGroup failed.");
      return NULL;
   }
   pvnres = SHoundGOIDFromGiList(pvngroup);
   ValNodeFree(pvngroup);
   return pvnres;
}

/* 
 * returns list of evidence code for a given gi and goid.
 * For a given gi, the same gene ontology classification (goid)
 * can have different evidence code.
 */
ValNodePtr LIBCALL SHoundGOECodeFromGiAndGOID(Int4 gi, Int4 goid)
{
  ValNodePtr vnp    = NULL;    /* our return value */
  CharPtr cp        = NULL;
  StGOdbNodePtr pGi = NULL,
            pGiHead = NULL;
  StLLdbNodePtr pll  = NULL;

  /* if(!SHoundModule("lldb"))
   {
     ErrPostEx(SEV_ERROR,0,0, "SHoundGOECodeFromGi: Missing required SeqHound module.");
     return NULL;
   } */

  if ( 0 >= gi || 0 >= goid )
  {
    ErrPostEx ( SEV_ERROR, 0, 0, 
		"[SHoundGOECodeFromGiAndGOID]: Invalid inputs." );
    return NULL;
  }

  pll = StLLdbNew();
  pll->npid = gi;
  if ( LL_SUCCESS == LL_SearchLLDB(&pll) )
  {
     pGiHead = StGOdbNew ();
     pGiHead->llid = pll->llid;
     if ( LL_SUCCESS == LL_SearchGODB( &pGiHead ))
     {
        /* success */
        for ( pGi = pGiHead; NULL != pGi; pGi = pGi->next )
	{
           if ( pGi->goid == goid )
	   {
	      cp = MemNew ( (size_t) StringLen(pGi->eviCode)+1 );
	      StringCpy (cp, pGi->eviCode);
	      Slri_ValNodeAddStr ( &vnp, 0, cp );
	   }
	}
     }

     StGOdbFree(pGiHead);
  }
  StLLdbFree ( pll );
  return (ValNodePtr) vnp;
}

/* 
 * Returns a linked list containing the pmid of records 
 * which match the given Gi-GOID pair. 
 */
ValNodePtr LIBCALL SHoundGOPMIDFromGiAndGOID(Int4 gi, Int4 goid)
{
  StGOdbPtr     pGO = NULL, 
            pGOHead = NULL;
  StLLdbNodePtr pll  = NULL;
  ValNodePtr vnp      = NULL;

  /* if(!SHoundModule("lldb"))
   {
     ErrPostEx(SEV_ERROR,0,0, "SHoundGOPMIDFromGi: Missing required SeqHound module.");
     return NULL;
   } */

  if ( gi <= 0 || goid <= 0 )
  {
    ErrPostEx ( SEV_ERROR, 0, 0, 
		"SHoundGOPMIDFromGi: Invalid inputs." );
    return NULL;
  }

  pll = StLLdbNew ();
  pll->npid = gi;
  if ( LL_SUCCESS == LL_SearchLLDB(&pll) )
  {
     /* seach using the llid */
     pGOHead = StGOdbNew();
     pGOHead->llid = pll->llid;
     if ( LL_SUCCESS == LL_SearchGODB ( &pGOHead ))
     {
        for ( pGO = pGOHead; NULL != pGO; pGO = pGO->next )
	{
           if ( pGO->goid == goid && 0 < pGO->pmid )
	      { Slri_ValNodeAddInt (&vnp, 0, pGO->pmid); }
	}
     }
     /* Gi found and pPMIDHead has been filled in. */
     StGOdbFree (pGOHead);
     if ( NULL != vnp ) Misc_IntVnpUnique(&vnp);
  }
  StLLdbFree( pll );
  return (ValNodePtr) vnp;
}

/* 
 * returns a linked list of Gi whose omimid matches. Caller frees the
 * linked list. 
 */
ValNodePtr LIBCALL SHoundGiFromOMIM(Int4 omimid)
{
  ValNodePtr         pGis  = NULL;
  ValNodePtr         pHead = NULL;
  CharPtr            access[ 30 ];

  if ( 0 >= omimid )
  {
    ErrPostEx ( SEV_ERROR, 0, 0, 
		"[SHoundGiFromOMIM]: Invalid omim id." );
    return NULL;
  }

  sprintf( (char *)access, "%ld", omimid );
  pGis = GetGiFromGENEDbxref( (char *)"mim", (char * )access );

  return pGis;
}

/* returns a linked list of gi with omimid that matches one in vnpomim.
   Caller frees the linked list. */
ValNodePtr LIBCALL SHoundGiFromOMIMList(ValNodePtr vnpomim)
{
  ValNodePtr vgi  = NULL;       /* return value of each gi search */
  ValNodePtr vomim = NULL;              
  ValNodePtr vnp  = NULL;     /* what we are looking for */
  
  /* if(!SHoundModule("lldb"))
  {
     ErrPostEx(SEV_ERROR,0,0, "SHoundGiFromOMIMList: Missing required SeqHound module.");
     return NULL;
  } */

  if(vnpomim == NULL)
  {
    ErrPostEx(SEV_ERROR, 0, 0, 
	      "[SHoundGiFromOMIMList]: NULL input list.");
    return NULL;
  }

  for ( vomim = vnpomim; vomim != NULL; vomim = vomim->next )
  {
     vgi = SHoundGiFromOMIM (vomim->data.intvalue);
     if ( NULL != vgi ) ValNodeLink (&vnp, vgi);
  }

  if ( NULL != vnp ) Misc_IntVnpUnique(&vnp);
  return vnp;
}

/* 
 * returns a linked list of omim whose gi matches gi. Caller frees the
 * list. 
 */
ValNodePtr LIBCALL SHoundOMIMFromGi(Int4 gi)
{
  Int4 llid = 0;
  ValNodePtr pAccess = NULL;
  ValNodePtr pOMIM   = NULL;
  Int4 omimId = 0;


  if( 0 >= gi)
  {
    ErrPostEx(SEV_ERROR, 0, 0, "SHoundOMIMFromGi: Invalid gi.");
    return pAccess;
  }

  pAccess = GetGENEDbxrefFromGi( gi, "mim" );

  while( pAccess )
  {
	omimId = atoi( pAccess->data.ptrvalue );

	pOMIM = Slri_ValNodeAddInt( &pOMIM, 1, omimId );

	pAccess = pAccess->next;
  }

  return pOMIM;
}

/* 
 * returns a list of omim whose record has a gi that matches one in
 * vnpgi. Caller has to free return value 
 */
ValNodePtr LIBCALL SHoundOMIMFromGiList(ValNodePtr vnpgi)
{
  ValNodePtr vnpomim = NULL,     /* what we are looking for */
                 vgi = NULL,     /* temp variable */
                 vnp = NULL;
  /* if(!SHoundModule("lldb"))
  {
     ErrPostEx ( SEV_ERROR,0,0, 
                 "SHoundOMIMFromGiList: Missing required SeqHound module.");
     return NULL;
  } */

  if(vnpgi == NULL){
    ErrPostEx(SEV_ERROR, 0, 0, "SHoundOMIMFromGiList: NULL input.");
    return NULL;
  }

  for (vgi = vnpgi; vgi != NULL; vgi = vgi->next )
  {
    vnp = SHoundOMIMFromGi(vgi->data.intvalue);
    if ( NULL != vnp )
      ValNodeLink(&vnpomim, vnp);
  }
  if ( NULL != vnpomim ) Misc_IntVnpUnique(&vnpomim);
  return vnpomim;
}

/* 
 * returns list of gi from records whose cddid matches. Caller 
 * frees the list. 
 */
ValNodePtr LIBCALL SHoundGiFromCDDID(CharPtr cddid)
{
  ValNodePtr ll = NULL,
            vnp = NULL;

  /* if(!SHoundModule("lldb"))
  {
     ErrPostEx ( SEV_ERROR,0,0, 
                 "SHoundGiFromCDDID: Missing required SeqHound module.");
     return NULL;
  } */

  if ( cddid == NULL || !StrCmp(cddid, "") )
  {
    ErrPostEx(SEV_ERROR, 0, 0, "SHoundGiFromCDDID: Invalid cdd id.");
    return NULL;
  }

  if ( NULL != (ll = SHoundLLIDFromCDDID ( cddid )) )
  {
     vnp = SHoundGiFromLLIDList ( ll );
  }

  if ( NULL != vnp ) Misc_IntVnpUnique(&vnp);
  return (ValNodePtr) vnp;
}

/* returns a list of gi whose records match a cddid in vnpcdd. 
   Caller must free return value. */
ValNodePtr LIBCALL SHoundGiFromCDDIDList(ValNodePtr vnpcdd)
{
  ValNodePtr vgi = NULL,       /* return value of each gi search */
  vcdd = NULL,               /* temp variable */
  vnpgi = NULL;              /* what we are looking for */

  /* if(!SHoundModule("lldb"))
  {
     ErrPostEx(SEV_ERROR,0,0, "SHoundGiFromCDDIDList: Missing required SeqHound module.");
     return NULL;
  } */

  if(vnpcdd == NULL){
    ErrPostEx(SEV_ERROR, 0, 0, "SHoundGiFromCDDIDList: NULL input paramter.");
    return NULL;
  }

  for ( vcdd = vnpcdd; vcdd != NULL; vcdd = vcdd->next )
  {
    vgi = SHoundGiFromCDDID(vcdd->data.ptrvalue);
    if (NULL != vgi )ValNodeLink ( &vnpgi, vgi );
  }
  if ( NULL != vnpgi ) Misc_IntVnpUnique(&vnpgi);
  return vnpgi;
}

/* 
 * returns a list of cddid from records whose gi matches.
 * Caller must free the return value. This is a list of CharPtr. 
 */
ValNodePtr LIBCALL SHoundCDDIDFromGi(Int4 gi)
{
  ValNodePtr vnp = NULL,
               v = NULL;
  StLLdbPtr ll = NULL,
           vll = NULL;

  /* if(!SHoundModule("lldb"))
  {
     ErrPostEx(SEV_ERROR,0,0, "SHoundCDDIDFromGi: Missing required SeqHound module.");
     return NULL;
  } */

  if( 0 >= gi )
  {
    ErrPostEx(SEV_ERROR, 0, 0, "SHoundCDDIDFromGi: Invalid gi.");
    return NULL;
  }
  
  ll = StLLdbNew();
  ll->npid = gi;
  if ( LL_SUCCESS == LL_SearchLLDB(&ll) )
  {
     for ( vll = ll; NULL != vll; vll = vll->next )
     {
       v = SHoundCDDIDFromLLID ( vll->llid );
       if ( NULL != v ) ValNodeLink ( &vnp, v ); 
     }
  }

  return (ValNodePtr) vnp;
}

/* returns a list of cddid from records whose gi matches one in vnpgi.
   Caller must free the list of CharPtr. */
ValNodePtr LIBCALL SHoundCDDIDFromGiList(ValNodePtr vnpgi)
{
  ValNodePtr vcdd = NULL,       /* return value of each gi search */
  vgi = NULL;                 /* temp variable */
  ValNodePtr vnp = NULL;     /* what we are looking for */
  /* if(!SHoundModule("lldb"))
  {
     ErrPostEx(SEV_ERROR,0,0, "SHoundCDDIDFromGiList: Missing required SeqHound module.");
     return NULL;
  } */

  if ( vnpgi == NULL )
  {
    ErrPostEx ( SEV_ERROR, 0, 0, 
		"SHoundCDDIDFromGiList: NULL input parameter." );
    return NULL;
  }

  for(vgi = vnpgi; vgi != NULL; vgi = vgi->next )
  {
    vcdd = SHoundCDDIDFromGi(vgi->data.intvalue);
    if ( NULL != vcdd ) ValNodeLink ( &vnp, vcdd );
  }

  if ( NULL != vnp ) Misc_StringVnpUniqueAndFree(&vnp);
  return vnp;
}

/* 
 * returns the CDD Score for a given gi and cddid as a FloatHi.
 * To print the result out, use the notation:
 * printf ("%.8g", result);
 */
FloatHi LIBCALL SHoundCDDScoreFromGi(Int4 gi, CharPtr cddid)
{
  StCDDdbPtr pcddHead = NULL, 
                 pcdd = NULL;
  FloatHi       score = 0;
  Int4          llid = 0;

  /* if( !SHoundModule("lldb") )
  {
     ErrPostEx ( SEV_ERROR,0,0, 
                 "[SHoundCDDScoreFromGi]: Missing required SeqHound module.");
     return -1;
  } */

  if ( gi <= 0 || NULL == cddid )
  {
    ErrPostEx ( SEV_ERROR, 0, 0, 
		"[SHoundCDDScoreFromGi]: Invalid goid." );
    return -1;
  }

  llid = SHoundLLIDFromGi ( gi ); 
  if ( 0 != llid )
  {
     pcddHead = StCDDdbNew();
     pcddHead->llid = llid;
     if ( LL_SUCCESS == LL_SearchCDDDB(&pcddHead))
     { 
        /* gi found and pcddHead filled in, 
         * just need to make sure that cddid matches 
         */
        for ( pcdd = pcddHead; pcdd != NULL; pcdd = pcdd->next )
        {
           if( 0 == StringCmp(pcdd->cddid, cddid) ){
              score = pcdd->eValue;
              break;
           }
        }
        StCDDdbFree(pcddHead);
     }
  }
  return score;
}

/* 
 * returns a list of the gi of records whose locus link id matches llid.
 * Caller has to free the return value. 
 */
ValNodePtr LIBCALL SHoundGiFromLLID(Int4 llid)
{
  StLLdbPtr pGi = NULL, pGiHead = NULL;
  ValNodePtr vnpgi = NULL;

  if ( llid <= 0 )
  {
    ErrPostEx(SEV_ERROR, 0, 0, "SHoundGiFromLLID: Invalid llid.");
    return NULL;
  }
  
  vnpgi =  GetGiFromGeneid( llid );

  return (ValNodePtr) vnpgi;
}

/* returns a list of gi whose records has a locus link id that matches
   one of the llid in vnpll. Caller has to free return value. */
ValNodePtr LIBCALL SHoundGiFromLLIDList(ValNodePtr vnpll)
{
  ValNodePtr vgi = NULL,        /* return value of each gi search */
  vll = NULL;                 /* temp variable */
  ValNodePtr vnpgi = NULL;      /* what we are looking for */

  /* if(!SHoundModule("lldb"))
  {
     ErrPostEx(SEV_ERROR,0,0, "SHoundGiFromLLIDList: Missing required SeqHound module.");
     return NULL;
  } */

  if ( vnpll == NULL )
  {
    ErrPostEx(SEV_ERROR, 0, 0, "SHoundGiFromLLIDList: NULL list as input.");
    return NULL;
  }

  for ( vll = vnpll; vll != NULL; vll = vll->next )
  {
     vgi = SHoundGiFromLLID ( vll->data.intvalue );
     if (NULL != vgi ) ValNodeLink ( &vnpgi,vgi );
  }

  if ( NULL != vnpgi ) Misc_IntVnpUnique(&vnpgi);
  return vnpgi;
}

/* returns a locus link id of the record with the gi. Returns -1 if
   an error occurred, 0 if gi not found. */
Int4 LIBCALL SHoundLLIDFromGi(Int4 gi)
{
  Int4 ll             = 0;
  ValNodePtr pGeneids = NULL;

  if ( 0 >= gi )
  {
    ErrPostEx(SEV_ERROR,0,0, "SHoundLLIDFromGi: Invalid gi.");
    return 0;
  }
 
  /* pGeneids = GetGENEDbxrefFromGi( gi, "locusid" );

  if( pGeneids )
  {
	ll = pGeneids->data.intvalue;
	}*/

  ll = GetGeneidFromGi( gi );

  return ll;
}

/* returns a list of locus link id of the records whose gi matches
   one in vnpgi. Caller has to free the return value. */
ValNodePtr LIBCALL SHoundLLIDFromGiList(ValNodePtr vnpgi)
{
  Int4 vll = 0;               /* return value of each gi search */
  ValNodePtr vgi = NULL;      /* temp variable */
  ValNodePtr vnpll = NULL;    /* what we are looking for */

  /* if( !SHoundModule("lldb") )
  {
     ErrPostEx ( SEV_ERROR,0,0, 
                 "SHoundLLIDFromGiList: Missing required SeqHound module." );
     return NULL;
  } */

  if(vnpgi == NULL){
    ErrPostEx(SEV_ERROR, 0, 0, "SHoundLLIDFromGiList: NULL List as parameter.");
    return NULL;
  }
  
  for( vgi = vnpgi; vgi != NULL; vgi = vgi->next )
  {
    vll = SHoundLLIDFromGi(vgi->data.intvalue);
    if( 0 < vll ) {
      Slri_ValNodeAddInt(&vnpll, 0, vll);
    }
  }
  if ( NULL != vnpll ) Misc_IntVnpUnique(&vnpll);
  return vnpll;
}


/* 
 * allocates and returns the chromosome location of the 
 * gene that matches gi. The empty string "" indicates that there was not provided
 * map. Caller should free the return value. 
 */
CharPtr LIBCALL SHoundLocusFromGi(Int4 gi)
{
  CharPtr       pmap = NULL;
  StLLdbPtr    llPtr = NULL;
  Int4          llid = 0;

  if (0 >= gi)
  {
    ErrPostEx ( SEV_ERROR, 0, 0, 
		"[SHoundLocusFromGi]: Invalid gi." );
    return NULL;
  }

  pmap = GetLocusFromGi( gi );

  return IsNullStr(pmap);
}



/*
$Log: ll_query.c,v $
Revision 1.16  2005/04/26 19:07:08  eburgess
Change locus link apis to access GENE tables.

Revision 1.15  2004/06/10 20:48:15  zwang
Call LL_InitCodeBase with flag.

Revision 1.14  2004/05/25 16:10:34  ryao
Remove SHoundGOAnnotFromRedundantGi, it was handled by java servlet.

Revision 1.13  2004/05/25 14:23:54  ryao
removed unused variables

Revision 1.12  2004/05/19 18:20:55  hlieu
fix bugs so that will return data from GOA properly

Revision 1.11  2004/05/17 19:56:52  hlieu
fix cross comp error

Revision 1.10  2004/05/17 19:39:39  hlieu
fix SHoundOMIMFromGi to return valnodeptr

Revision 1.9  2004/05/17 19:07:28  hlieu
fix for negative locus links

Revision 1.8  2004/05/13 17:38:19  hlieu
major refactor of locus link module. primary key changed from gi to locuslink
id. Also required update of objXXX source code to reflect the change in
ASN structure

Revision 1.7  2004/04/05 15:38:35  hlieu
fixed CDDScore comment

Revision 1.6  2004/02/19 17:22:07  zwang
Check if pass in an empty string in SHoundGiFromCDDID.

Revision 1.5  2004/01/06 22:32:16  skuznets
Code audit has been done sucessfully.
Possible NULL pointer issues has been eliminated.

Revision 1.4  2003/09/29 21:33:39  haocl
merged objll.c, objshproteinreport.c into objslristruc.c,
intrez, ll_query reflect merging, NBlastasn, objslristruc and objslritax
newly generated

Revision 1.3  2003/08/26 20:54:35  haocl
changed GOPMIDFromGi to GiAndGOID

Revision 1.2  2003/08/26 20:32:53  haocl
changed SHoundGOECodeFromGi to GiAndGOID

Revision 1.1.1.9  2003/08/21 08:48:19  root
Automated import from Sourceforge.net

Revision 1.10  2003/08/20 19:47:38  haocl
check in for cvs switch over

Revision 1.9  2003/07/30 15:22:54  haocl
added functionality SHoundGiFromGOIDAndECode

Revision 1.8  2002/08/07 22:00:08  kaca
removed loging of unsuccessful searches

Revision 1.7  2002/07/25 15:11:53  kaca
reviewed ValNode sorting upon return

Revision 1.6  2001/12/19 23:31:46  kaca
added newline at EOF

Revision 1.5  2001/12/11 19:04:57  kaca
added Id keyword

Revision 1.4  2001/12/11 19:02:57  kaca
added Log keyword

*/



