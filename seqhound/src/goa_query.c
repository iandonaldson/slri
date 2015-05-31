/********************************************************************************
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
	   Renan A. CAVERO


  REVISION/CONTRIBUTION NOTES:

  June 1, 2000  Original MSH Revision.
                Hogue Lab - University of Toronto Biochemistry
                Department and the Samuel Lunenfeld Research
                Institute, Mount Sinai Hospital
                http://bioinfo.mshri.on.ca  hogue@mshri.on.ca
                ftp://ftp.mshri.on.ca/


  PUBLICATION TO CITE:


  DESCRIPTION
  April 2005
  SeqHound GO Assocciation Module.
  GOA Module
 

*******************************************************************************/

#include <asn.h>


#ifdef ODBC
 #include <intrez_odbc.h>
 #include <goa_odbc.h>
 #include <goa_query.h>
#else
 #include <intrez_db.h>
#endif


#include <slri_misc.h>
#include <intrez_cfg.h>
#include <seqhound.h>
#include <objslristruc.h>


/**************************************************************
                 PUBLIC INITIALIZATION FUNCTIONS
***************************************************************/
/* Connect to Database Server. This funciton must be called 
   prior to any queries to the database can be properly executed. 
   Call this at the start of your program. */
Boolean GOA_ConectToDB(Boolean CREATE)
{
    if(InitGOA() == FALSE)
    {
	ErrPostEx ( SEV_ERROR, 0, 0, "[GOA_ConectToDB]: Can't allocate Stmt Handlers." );
	return FALSE;
    }
    return TRUE;
}


/* Closes the Database Connection.
   Call this function at the end of your program. */
Boolean GOA_CloseDB(void)
{
    if(CloseGOA() == FALSE)
    {
	 ErrPostEx ( SEV_ERROR, 0, 0, "[GOA_CloseDB]: Can't free GOA Stmt Handlers." );
	 return FALSE;
    }
    return TRUE;
}

/* returns a linked list of goid of records with gi. It is the
   callers responsibility to free the vnp. Returns NULL if an
   error occured or if gi not found. */
ValNodePtr LIBCALL SHoundGOIDFromGiII(Int4 gi)
{
    ValNodePtr vnpgo = NULL;          /* our return value */

    if ( 0 >= gi )
    {
	ErrPostEx ( SEV_ERROR, 0, 0, "[SHoundGOIDFromGi]: Invalid gi." );
	return NULL;
    }

    if (GOA_SearchGOIDFromGi(gi, &vnpgo) != SH_SUCCESS)
    {
	return NULL;
    }
    return (ValNodePtr) vnpgo;
}


/* returns a linked list of goid of records given a ValNodePtr link list of gi.
   It is the  callers responsibility to free the vnp. Returns NULL if an
   error occured or if gi not found. */
ValNodePtr LIBCALL SHoundGOIDFromGiListII(ValNodePtr giList)
{
    ValNodePtr vnpgo = NULL;          /* our return value */

    if ( giList == NULL )
    {
	ErrPostEx ( SEV_ERROR, 0, 0, "[SHoundGOIDFromGiList]: Invalid gi List." );
	return NULL;
    }

    if (GOA_SearchGOIDFromGiList(giList, &vnpgo) != SH_SUCCESS)
    {
	return NULL;
    }
    return (ValNodePtr) vnpgo;
}


/* returns a linked list of goid of records with gi. It is the
   callers responsibility to free the vnp. Returns NULL if an
   error occured or if gi not found.
   Fetches all Gene Ontology (GO) term identifiers assigned to a protein 
   sequence. Uses redundancy information for the query GI identifier. 
   This means that if the query fails for the input GI identifier, 
   then the query will be retried for all other GI identifiers that belong to 
   the same redundant group. An individual sequence can be referred to by one 
   or more a process, function or component terms.
   Because a Redundant group can have GIs from different Taxonomies, 
   the taxonomy ID of the query GI is taken into consideration when searching
   the GO Terms.
*/
ValNodePtr LIBCALL SHoundGOIDFromRedundantGiII(Int4 gi)
{
    ValNodePtr vnpgo = NULL;          /* our return value */

    if ( 0 >= gi )
    {
	ErrPostEx ( SEV_ERROR, 0, 0, "[SHoundGOIDFromRedundantGi]: Invalid gi." );
	return NULL;
    }

    if (GOA_SearchGOIDFromRedundantGi(gi, &vnpgo) != SH_SUCCESS)
    {
	return NULL;
 
    }

    return (ValNodePtr) vnpgo;
}


/* returns a linked list of goid of records from gi list. It is the
   callers responsibility to free the vnp. Returns NULL if an
   error occured or if gi not found.
   Fetches all Gene Ontology (GO) term identifiers assigned to the proteins 
   sequences. Uses redundancy information for the query GI identifier. 
   This means that if the query fails for the input GI identifier, 
   then the query will be retried for all other GI identifiers that belong to 
   the same redundant group. An individual sequence can be referred to by one 
   or more a process, function or component terms.
   Because a Redundant group can have GIs from different Taxonomies, 
   the taxonomy ID of the query GI is taken into consideration when searching
   the GO Terms.
*/
ValNodePtr LIBCALL SHoundGOIDFromRedundantGiListII(ValNodePtr giList)
{
    ValNodePtr vnpgo = NULL;          /* our return value */

    if ( giList == NULL )
    {
	ErrPostEx ( SEV_ERROR, 0, 0, "[SHoundGOIDFromRedundantGiList]: Invalid gi List." );
	return NULL;
    }

    if (GOA_SearchGOIDFromRedundantGiList(giList, &vnpgo) != SH_SUCCESS)
    {
	return NULL;
    }
    return (ValNodePtr) vnpgo;
}


/* returns a linked list of gi of records with go. It is the
   callers responsibility to free the vnp. Returns NULL if an
   error occured or if gi not found.
   Fetches all protein sequences which were assigned a given Gene Ontology (GO)
   function, process or component.
 */
ValNodePtr LIBCALL SHoundGiFromGOIDII(Int4 go)
{
    ValNodePtr vnpgi = NULL;          /* our return value */
    if ( go <= 0 )
    {
	ErrPostEx ( SEV_ERROR, 0, 0, "[SHoundGiFromGOIDII]: Invalid go..." );
	return NULL;
    }

    if (GOA_SearchGiFromGOID(go, &vnpgi) != SH_SUCCESS)
    {
	return NULL;
     }

    return (ValNodePtr) vnpgi;
}


/* returns a linked list of gi's of records given a ValNodePtr link list of go.
   It is the  callers responsibility to free the vnp. Returns NULL if an
   error occured or if go not found. */
ValNodePtr LIBCALL SHoundGiFromGOIDListII(ValNodePtr goList)
{
    ValNodePtr vnpgi = NULL;          /* our return value */

    if ( goList == NULL )
    {
	ErrPostEx ( SEV_ERROR, 0, 0, "[SHoundGiFromGOIDList]: Invalid go List." );
	return NULL;
    }

    if (GOA_SearchGOIDFromGiList(goList, &vnpgi) != SH_SUCCESS)
    {
	return NULL;
    }
    return (ValNodePtr) vnpgi;
}


/* returns a linked list of gi of records from a go and Evidence Code.
   It is the   callers responsibility to free the vnp. Returns NULL if an
   error occured or if gi not found.
   Fetches all protein sequences which were assigned a given Gene Ontology (GO)
   function, process or component.
 */
ValNodePtr LIBCALL SHoundGiFromGOIDAndECodeII(Int4 go, CharPtr ECode)
{
    ValNodePtr vnpgi = NULL;          /* our return value */

    if ( go <= 0 )
    {
	ErrPostEx ( SEV_ERROR, 0, 0, "[SHoundGiFromGOIDAndECodeII]: Invalid go." );
	return NULL;
    }
    if (ECode == NULL)
    {
	ErrPostEx ( SEV_ERROR, 0, 0, "[SHoundGiFromGOIDAndECodeII]: Invalid ECode." );
	return NULL;
    }

    if (GOA_SearchGiFromGOIDAndECode(go, ECode, &vnpgi) != SH_SUCCESS)
    {
	return NULL;
    }

    return (ValNodePtr) vnpgi;
}


/* returns a linked list of gi of records with go list and Evidence Code.
   It is the callers responsibility to free the vnp. Returns NULL if an
   error occured or if gi not found.
   Fetches all protein sequences which were assigned a given Gene Ontology (GO)
   function, process or component.
 */
ValNodePtr LIBCALL SHoundGiFromGOIDListAndECodeII(ValNodePtr goList, CharPtr ECode)
{
    ValNodePtr vnpgi = NULL;          /* our return value */

    if ( goList == NULL)
    {
	ErrPostEx ( SEV_ERROR, 0, 0, "[SHoundGiFromGOIDListAndECodeII]: Invalid goList." );
	return NULL;
    }
    if (ECode == NULL)
    {
	ErrPostEx ( SEV_ERROR, 0, 0, "[SHoundGiFromGOIDListAndECodeII]: Invalid ECode." );
	return NULL;
    }

    if (GOA_SearchGiFromGOIDListAndECode(goList, ECode, &vnpgi) != SH_SUCCESS)
    {
	return NULL;
     }

    return (ValNodePtr) vnpgi;
}


ValNodePtr LIBCALL SHoundGOECodeFromGiAndGOIDII(Int4 gi, Int4 go)
{
    ValNodePtr vnpECode = NULL;          /* our return value */

    if ( gi <= 0 )
    {
	ErrPostEx ( SEV_ERROR, 0, 0, "[SHoundGOECodeFromGiAndGOID]: Invalid gi." );
	return NULL;
    }
    if (go <= 0 )
    {
	ErrPostEx ( SEV_ERROR, 0, 0, "[SHoundGOECodeFromGiAndGOID]: Invalid go." );
	return NULL;
    }

    if (GOA_SearchGOECodeFromGiAndGOID(gi, go, &vnpECode) != SH_SUCCESS)
    {
	return NULL;
    }

    return (ValNodePtr) vnpECode;
}


ValNodePtr LIBCALL SHoundGOIDFromLLIDII(Int4 ll)
{
    ValNodePtr vnpGoList = NULL;          /* our return value */
    if ( ll <= 0 )
    {
	ErrPostEx ( SEV_ERROR, 0, 0, "[SHoundGOIDFromLLIDII]: Invalid gi." );
	return NULL;
    }
    vnpGoList = SHoundGOIDFromGENEIDII(ll);
    if (vnpGoList == NULL)
    {
	return NULL;
    }
    return (ValNodePtr) vnpGoList;
}


ValNodePtr LIBCALL SHoundGOIDFromGENEIDII(Int4 gene)
{
    ValNodePtr vnpGoList = NULL;          /* our return value */

    if ( gene <= 0 )
    {
	ErrPostEx ( SEV_ERROR, 0, 0, "[SHoundGOIDFromGENEID]: Invalid gi." );
	return NULL;
    }

    if (GOA_SearchGOIDFromGENEID(gene, &vnpGoList) != SH_SUCCESS)
    {
	return NULL;
     }

    return (ValNodePtr) vnpGoList;
}

ValNodePtr LIBCALL SHoundPMIDFromGiAndGOIDII(Int4 gi, Int4 go)
{
   ValNodePtr vnpPMID = NULL;          /* our return value */

    if ( gi <= 0 )
    {
	ErrPostEx ( SEV_ERROR, 0, 0, "[SHoundPMIDFromGiAndGOID]: Invalid gi." );
	return NULL;
    }
    if (go <= 0 )
    {
	ErrPostEx ( SEV_ERROR, 0, 0, "[SHoundPMIDFromGiAndGOID]: Invalid go." );
	return NULL;
    }

    if (GOA_SearchPMIDFromGiAndGOID(gi, go, &vnpPMID) != SH_SUCCESS)
    {
	return NULL;
     }

    return (ValNodePtr) vnpPMID;  
}
