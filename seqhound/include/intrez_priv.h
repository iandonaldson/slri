/* $Id: intrez_priv.h,v 1.2 2004/01/06 22:32:12 skuznets Exp $
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

#ifndef _INTREZ_PRIV_
#define _INTREZ_PRIV_


#ifdef __cplusplus
extern "C" { /* } */
#endif


#include <d4all.h>
#include <intrez_defs.h>


#define PATENT_ORDER 110






extern CODE4 cb;

extern DATA4
     *pd4Asndb,
     *pd4Partition,
     *pd4Nucprot,
     *pd4Accdb,
     *pd4Pubseq,
     *pd4Taxgi,
     *pd4Chrom,
     *pd4Sendb,
     *pd4Sengi,
     *pd4Mmdb,
     *pd4Mmgi,
     *pd4Redund,
     *pd4Domdb,
     *pd4Vastdb,
     *pd4Nrbioseq,
     *pd4Cddb;

extern FIELD4
      /* ASNDB */
      *pf4asGi,
      *pf4asAsn1,
      *pf4asType,
      *pf4asDivision,
      *pf4asRelease,
      /* PARTI */
      *pf4paGi,
      *pf4paParti,
      /* NUCPROT */
      *pf4ncGin,
      *pf4ncGia,
      /* ACCDB */
      *pf4acGi,
      *pf4acDb,
      *pf4acName,
      *pf4acAccess,
      *pf4acChain,
      *pf4acRelease,
      *pf4acVersion,
      *pf4acTitle,
      *pf4acNamelow,
      /* PUBSEQ */
      *pf4psGi,
      *pf4psMuid,
      /* TAXGI */
      *pf4tgGi,
      *pf4tgTaxId,
      *pf4tgKloodge,
      *pf4tgType,
      /* CHROM */
      *pf4chTaxId,
      *pf4chKloodge,
      *pf4chChromfl,
      /* SENDB */
      *pf4sdSeid,
      *pf4sdAsn1,
      /* SENGI */
      *pf4sgSeid,
      *pf4sgGi,
      *pf4sgDivision,
       /* MMDB */
      *pf4mdAsn1,
      *pf4mdMmdbId,
      *pf4mdPdbId,
      *pf4mdbWhat,
      *pf4mdModels,
      *pf4mdMolecules,
      *pf4mdSize,
      *pf4mdBzSize,
      /* MMGI */
      *pf4mgMmdbId,
      *pf4mgGi,
      /* REDUND */
      *pf4rdGi,
      *pf4rdOrdinal,
      *pf4rdGroup,
      *pf4rdType,
       /* DOMDB */
       *pf4ddMmdbId,
       *pf4ddAsn1,
       *pf4ddPdbId,
       *pf4ddChain,
       *pf4ddGi,
       *pf4ddDomno,
       *pf4ddDomall,
       *pf4ddDomId,
       *pf4ddRep,
        /* VASTDB */
       *pf4vdMmdbId,
       *pf4vdPdbId,
       *pf4vdChain,
       *pf4vdAsn1,
       *pf4vdVastId1,
       *pf4vdVastId2,
       *pf4vdScore,
       *pf4vdPval,
       *pf4vdRes,
       *pf4vdIdent,
       *pf4vdRmsd,
       *pf4vdScale,
       /* NRBIOSEQ */
       *pf4nbGi,
       *pf4nbAsn1,
       /*CDDB*/
       *pf4cdGi,
       *pf4cdAsn1;


extern TAG4
      /* ASNDB */
      *tagasGi,
      /* PARTI */
      *tagpaGi,
      /* NUCPROT */
      *tagncGi,
      *tagncGin,
      *tagncGia,
      /* ACCDB */
      *tagacGi,
      *tagacAcc,
      *tagacName,
      *tagacNamelow,
      /* PUBSEQ */
      *tagpsGi,
      *tagpsMuid,
      /* TAXGI */
      *tagtgGi,
      *tagtgTaxId,
      *tagtgKloodge,
      /* CHROM */
      *tagchTaxId,
      *tagchKloodge,
      *tagchChromfl,
      /* SENDB */
      *tagsdSeid,
      /* SENGI */
      *tagsgGi,
      *tagsgSeid,
      /* MMDB */
      *tagmdMmdbId,
      *tagmdPdbId,
      /* MMGI */
      *tagmgMmdbId,
      *tagmgGi,
      /* REDUND */
      *tagrdGi,
      *tagrdOrdinal,
      *tagrdGroup,
       /* DOMDB */
       *tagddMmdbId,
       *tagddPdbId,
       *tagddGi,
       *tagddDomall,
       *tagddDomId,
       /* VASTDB */
       *tagvdMmdbId,
       *tagvdPdbId,
       *tagvdVastId1,
       /* NRBIOSEQ */
       *tagnbGi,
       /*CDDB*/
       *tagcdGi;





#ifdef __cplusplus
/* { */ }
#endif



#endif  /* _INTREZ_PRIV_ */

/*
$Log: intrez_priv.h,v $
Revision 1.2  2004/01/06 22:32:12  skuznets
Code audit has been done sucessfully.
Possible NULL pointer issues has been eliminated.

Revision 1.1.1.1  2001/10/11 17:37:13  gbader
Automated import from Sourceforge.net

Revision 1.1.1.1  2001/05/25 21:36:20  kaca
initial import

Revision 1.7  2001/03/27 21:09:10  doron
Addition of CDDB function decl. and struc

Revision 1.6  2001/03/26 19:46:02  kaca
reorganized headers to support a single header for the local and remote APIs

Revision 1.5  2001/02/23 21:03:16  kaca
license change

Revision 1.4  2000/11/06 22:17:33  kaca
removed obsolete variable

Revision 1.3  2000/11/02 22:40:11  kaca
moved some static variables to intrez.c

Revision 1.2  2000/10/16 20:11:54  kaca
minor changes

Revision 1.1  2000/10/13 21:47:30  kaca
split headers into private, public and definitions

*/


