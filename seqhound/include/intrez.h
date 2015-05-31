/* $Id: intrez.h,v 1.12 2005/01/06 19:17:24 zwang Exp $
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
  header file for SeqHound database parsers and wwwseekgi cgi-bin,
  and SeqHoundAPI

******************************************************************************************/

#ifndef _INTREZ_
#define _INTREZ_


#include <asn.h>
#include <ncbi.h>
#ifdef ODBC
#include <asn2gnbk.h>
#else
#include <asn2gnbk.h>
/* include <asn2ff.h> */
#endif
#include <mmdbapi.h>
#include <mmdblocl.h>
#include <tofasta.h>
#include <objslristruc.h>
#include <taxdb_db.h>
#include <nbr_db.h>
#include <objslritax.h>
#include <slri_misc.h>
#include <slri_www.h>
#include <seqhound.h>
#include <hist_db.h>


#ifdef __cplusplus
extern "C" { /* } */
#endif

/*
#define REP            0x80
#define REP            0x40
#define REP            0x20
#define REP            0x10 */
#define REP_NONREDUND  0x08
#define REP_MIN_EIGHTY 0x04
#define REP_MIN_FORTY  0x02
#define REP_MIN_SEVEN  0x01
#define REP_ALL        0x0F



/* redefine Intrez functions to SHound equivalents for legacy code */
#define IntrezInit							SHoundInit
#define IntrezIsNetEntrezOn						SHoundIsNetEntrezOn
#define IntrezNetEntrezInit						SHoundNetEntrezInit
#define IntrezFini							SHoundFini
#define IntrezStartTransaction						SHoundStartTransaction
#define IntrezEndTransaction						SHoundEndTransaction
#define IntrezIsInited							SHoundIsInited
#define IntrezReconnect							SHoundReconnect
#define IntrezFindSeqId							SHoundFindSeqId
#define IntrezFindSeqIdList						SHoundFindSeqIdList
#define IntrezFindAcc							SHoundFindAcc
#define IntrezFindAccList						SHoundFindAccList
#define IntrezFindName							SHoundFindName
#define IntrezFindNameList						SHoundFindNameList
#define IntrezAccFromGi							SHoundAccFromGi
#define IntrezAccFromGiList						SHoundAccFromGiList
#define IntrezTitleFromGi						SHoundTitleFromGi
#define IntrezTitleFromGiList						SHoundTitleFromGiList
#define IntrezSeqIdFromGi						SHoundSeqIdFromGi
#define IntrezSeqIdFromGiList						SHoundSeqIdFromGiList
#define IntrezIsProtein							SHoundIsProtein
#define IntrezIsNRFirst							SHoundIsNRFirst
#define IntrezExists							SHoundExists
#define Intrez3DbWhat							SHound3DbWhat
#define Intrez3DExists							SHound3DExists
#define IntrezGetBioseq							SHoundGetBioseq
#define IntrezGetBioseqList						SHoundGetBioseqList
#define IntrezGetBioseqSet						SHoundGetBioseqSet
#define IntrezGetBioseqSetList						SHoundGetBioseqSetList
#define IntrezGetSeqEntry						SHoundGetSeqEntry
#define IntrezGetSeqEntryList						SHoundGetSeqEntryList
#define IntrezGetFasta							SHoundGetFasta
#define IntrezGetFastaList						SHoundGetFastaList
#define IntrezGetGenBankff						SHoundGetGenBankff
#define IntrezGetGenBankffList						SHoundGetGenBankffList
#define IntrezGetXMLBioseq						SHoundGetXMLBioseq
#define IntrezGetXMLSeqEntry						SHoundGetXMLSeqEntry
#define IntrezGetXML3D							SHoundGetXML3D
#define IntrezGiFrom3D							SHoundGiFrom3D
#define IntrezGiFrom3DList						SHoundGiFrom3DList
#define Intrez3DFromGi							SHound3DFromGi
#define Intrez3DFromGiList						SHound3DFromGiList
#define IntrezDNAFromProtein						SHoundDNAFromProtein
#define IntrezDNAFromProteinList					SHoundDNAFromProteinList
#define IntrezProteinFromDNA						SHoundProteinFromDNA
#define IntrezProteinFromDNAList					SHoundProteinFromDNAList
#define IntrezMuidFromGi						SHoundMuidFromGi
#define IntrezMuidFromGiList						SHoundMuidFromGiList
#define IntrezMuidFrom3D						SHoundMuidFrom3D
#define IntrezMuidFrom3DList						SHoundMuidFrom3DList
#define IntrezTaxIDFromGi						SHoundTaxIDFromGi
#define IntrezTaxIDFromGiList						SHoundTaxIDFromGiList
#define IntrezTaxIDFrom3D						SHoundTaxIDFrom3D
#define IntrezTaxIDFrom3DList						SHoundTaxIDFrom3DList
#define IntrezProteinsFromTaxID						SHoundProteinsFromTaxID
#define IntrezProteinsFromTaxIDII					SHoundProteinsFromTaxIDII
#define IntrezProteinsFromTaxIDList					SHoundProteinsFromTaxIDList
#define IntrezDNAFromTaxID						SHoundDNAFromTaxID
#define IntrezDNAFromTaxIDII						SHoundDNAFromTaxIDII
#define IntrezDNAFromTaxIDList						SHoundDNAFromTaxIDList
#define IntrezDNAFromTaxIDIII						SHoundDNAFromTaxIDIII
#define IntrezProteinsFromTaxIDIII					SHoundProteinsFromTaxIDIII
#define IntrezRedundantGroup						SHoundRedundantGroup
#define IntrezRedundantGroupList					SHoundRedundantGroupList
#define IntrezRedundantGroupIDFromGI    				SHoundRedundantGroupIDFromGI
#define IntrezRedundantGroupIDFromGIList				SHoundRedundantGroupIDFromGIList
#define IntrezRedundantGroupFromID      				SHoundRedundantGroupFromID
#define IntrezRedundantGroupFromIDList  				SHoundRedundantGroupFromIDList
#define IntrezFirstOfRedundantGroupFromID				SHoundFirstOfRedundantGroupFromID
#define IntrezFirstOfRedundantGroupFromIDList				SHoundFirstOfRedundantGroupFromIDList
#define IntrezProteinsFromOrganism					SHoundProteinsFromOrganism
#define IntrezProteinsFromOrganismList					SHoundProteinsFromOrganismList
#define IntrezProteinsFromChromosome					SHoundProteinsFromChromosome
#define IntrezProteinsFromChromosomeList				SHoundProteinsFromChromosomeList
#define IntrezAllGenomes						SHoundAllGenomes
#define IntrezAllKloodge						SHoundAllKloodge
#define IntrezAllMMDBID							SHoundAllMMDBID
#define IntrezChromosomeFromGenome					SHoundChromosomeFromGenome
#define IntrezChromosomeFromGenomeList					SHoundChromosomeFromGenomeList
#define IntrezDNAFromOrganism						SHoundDNAFromOrganism
#define IntrezDNAFromOrganismList					SHoundDNAFromOrganismList
#define IntrezDNAFromChromosome						SHoundDNAFromChromosome
#define IntrezDNAFromChromosomeList					SHoundDNAFromChromosomeList
#define IntrezGet3D							SHoundGet3D
#define IntrezGet3DfromPdbId						SHoundGet3DfromPdbId
#define IntrezGet3DEx							SHoundGet3DEx
#define IntrezGet3DfromPdbIdEx						SHoundGet3DfromPdbIdEx
#define IntrezGetPDB3D							SHoundGetPDB3D
#define IntrezGetDomain							SHoundGetDomain
#define IntrezPDBFromMMDB						SHoundPDBFromMMDB


/***********************************************/
/* Domains; remote versions not implemented    */
/***********************************************/
SLRIValNodePtr        LIBCALL SHoundGetDomain(Int4 gi, Int2 domno);
SLRIFastaPtr          LIBCALL SHoundGetFastaDomain(Int4 domid, Int4 tolerance);
SLRIFastaPtr   LIBCALL SHoundMakeFastaDomain(StDomdbPtr pdd, Int4 tolerance, Int4 minilen);

/**********************************************/
/* get coding region FASTA from CDDB          */
/**********************************************/
SLRIFastaPtr LIBCALL IntrezGetCDSLRIFasta(Int4 Gi);

/*************************************************/
/* specialized SeqEntry retrieval for Text Index */
/*************************************************/
SeqEntryPtr  LIBCALL SHoundGetSeqEntryforTindex(Int4 Gi);



#ifdef __cplusplus
/* { */ }
#endif


#endif  /* _INTREZ_ */

/*
$Log: intrez.h,v $
Revision 1.12  2005/01/06 19:17:24  zwang
Added two new APIs - SHoundStartTransaction and SHoundEndTransaction - for users to put their process into a transaction.

Revision 1.11  2004/08/12 17:55:28  zwang
New API added to get title by gi from ACCDB

Revision 1.10  2004/06/18 22:19:23  hlieu
change includes for new GenBankff, bug 1162

Revision 1.9  2004/06/09 14:40:04  eburgess
Removed CHROM defines as these are defined in seqhound.h. Seqhound.h is
included in intrez.h.

Revision 1.8  2004/05/31 21:08:10  hlieu
minor compiler warnings, need to be fixed, bug 2185

Revision 1.7  2004/05/20 18:18:07  vgu
added conditional compilation for GenBankff on ODBC and CB

Revision 1.6  2004/05/07 17:16:42  rcavero
Replace asn2ff.h with asn2gnbk.h to be able to use
NCBI's function SeqEntryToGnbk

Revision 1.5  2004/01/13 21:49:55  rcavero
nblast_odbc.h it's a draf version for nblast's ODBC port.

Revision 1.4  2004/01/12 21:58:02  zwang
Include hist_db.h

Revision 1.3  2003/11/03 20:40:22  ryao
added SHoundPDBFromMMDB allow user to query pdbid from given mmdbid

Revision 1.2  2003/10/24 17:53:12  feldman
Added SHoundGet3DEx function to allow user to choose model level and num models when fetching biostruc

Revision 1.1.1.5  2003/07/26 08:41:52  root
Automated import from Sourceforge.net

Revision 1.5  2003/07/25 19:54:51  haocl
prototypes for new functionality in PUBSEQ.
SHoundGetReferenceIDFromGi[List], SHoundGiFromReferenceID[List]

Revision 1.4  2002/01/15 16:39:31  kaca
added SHoundMakeFastaDomain declaration

Revision 1.3  2001/11/19 21:22:43  kaca
removed domain declarations, they moved to intrez.c as static

Revision 1.2  2001/11/13 21:52:22  kaca
changed scope of some domain functions

Revision 1.1.1.2  2001/10/10 21:53:47  kaca
initial import

Revision 1.19  2001/08/02 16:13:32  michel
Added Neighbour Retrieval to SeqHound local

Revision 1.18  2001/07/24 19:55:36  kaca
added specialized SeqEntry retrieval for text index

Revision 1.17  2001/04/27 23:11:37  kaca
added SHoundFindName and SHoundFindNameList functions

Revision 1.16  2001/04/27 22:01:45  kaca
added minimal domain length check

Revision 1.15  2001/04/16 20:41:46  kaca
reflect changes in toolkit connect stuff

Revision 1.14  2001/04/12 21:01:42  kaca
moved function declaration from intrez.h to seqhound.h

Revision 1.13  2001/03/29 16:31:24  kaca
added slri_www include

Revision 1.12  2001/03/29 16:27:06  kaca
added function declaration for GetBioseqSet and GetBioseqSetList

Revision 1.11  2001/03/28 21:21:03  kaca
removed conflict

Revision 1.10  2001/03/27 21:23:26  kaca
removed function declaration duplicates

Revision 1.9  2001/03/27 21:09:10  doron
Addition of CDDB function decl. and struc

Revision 1.5  2000/10/16 20:11:54  kaca
minor changes

Revision 1.4  2000/10/13 21:46:11  kaca
split headers into private, public and definitions

Revision 1.2  2000/09/18 23:18:31  kaca
newline added at the end

Revision 1.1.1.1  2000/09/18 17:41:55  kaca
Initial import

Revision 1.6  2000/08/31 14:23:11  ruth
added precomputed searches to DNAfromTaxid and proteinfortaxid.  Added precomputed index functions

Revision 1.5  2000/08/30 15:58:33  ruth
added config function declarations

Revision 1.4  2000/08/21 21:08:34  kaca
removed TAXSP and parts of TAXGI
*/
