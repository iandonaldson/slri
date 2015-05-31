/* $Id: shoundtypes.hpp,v 1.2 2003/11/07 18:51:02 haocl Exp $ */ 
/***************************************************************************
  Copyright (C) 2001 Mount Sinai Hospital (MSH)
    
  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public License
  as published by the Free Software Foundation; either version 2.1 of
  the License, or any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

  See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with this program; if not, write to the
         Free Software Foundation, Inc.,
         59 Temple Place, Suite 330, Boston, MA
         02111-1307  USA
  or visit http://www.gnu.org/licenses/lgpl.txt

  SPECIAL EXCEPTIONS

  As a special exception, Mount Sinai Hospital gives permission to
  link this program with the following non-GPL programs or libraries,
  and distribute the resulting executable, without including the source
  code for these in the source distribution:

  a) CodeBase 6.5 or greater from Sequiter Software Inc.

  b) The C or C++ interface to Oracle 8.x or greater, from
     Oracle Corporation or IBM DB2 UDB.


  PROGRAM: SLRI C++ Seqhound WebAPI search functions


  AUTHORS: Hao Lieu


  REVISION/CONTRIBUTION NOTES:

  September 1, 2002  Original MSH Revision.
                Hogue Lab - University of Toronto Biochemistry
                Department and the Samuel Lunenfeld Research
                Institute, Mount Sinai Hospital
                http://bioinfo.mshri.on.ca  hogue@mshri.on.ca
                ftp://ftp.mshri.on.ca/


  PUBLICATION TO CITE:

************************************************************************/

#ifndef SH_TYPES
#define SH_TYPES

BEGIN_SCOPE(shoundquery)

  /* I put these enumerations in their own file because intrez.cpp and
   * query.cpp both need these definitions, and they conflict with
   * each other when the definitions are in query.cpp. */

	
  /* types of identifiers that can be specified.
   * To use these, eg specify x as taxid:
   *
   * using namespace shoundquery;
   *
   * ... 
   *   eQueryType x = eQuery_TaxID; 
   */
  enum eQueryType {
    eQuery_TaxID,                /* taxid */
    eQuery_PubMedID,             /* pubmed id */
    eQuery_MMDBID,               /* mmdb id */
    eQuery_ChromosomeID,         /* chrom id */
    eQuery_GOID,                 /* goid */
    eQuery_OMIMID,               /* omimid */
    eQuery_CDDID,                /* cdd id */
    eQuery_LocusLinkID           /* locus link id */
  };

  /* two types of molecules 
   * To use, eg set y as protein: 
   * 
   *  using namespace shoundquery;
   *
   * 
   *    eMolType y = eProtein; 
   *
   * The defintions can be expanded to include RNA, plasmids etc.
   */
  enum eMolType { eProtein = 0, eDNA ,eRNA/* , ePlasmid */};

  /* the display format, to use
   *
   *   using namespace shoundquery;
   *
   *
   *   eFormatType f = eDefline;
   */
  enum eFormatType
  {
    eDefline,                  /* Defline format */
    eFASTA,                    /* Fasta format */
    eASN,                      /* ASN format */
    eXML,                      /* XML format */
    eGenBank,                  /* NCBI's GenBank format */
    eSeqReport,                /* the comprehensive SeqHound Report */
    eUndef                     /* undefined type */
  };
END_SCOPE(shoundquery)


#endif

/*
 * $Log: shoundtypes.hpp,v $
 * Revision 1.2  2003/11/07 18:51:02  haocl
 * adjusted to change in SHoundMoleculeType
 *
 * Revision 1.1.1.2  2003/08/09 08:44:32  root
 * Automated import from Sourceforge.net
 *
 * Revision 1.2  2003/08/08 16:22:43  haocl
 * new files shoundrecords and changes to others
 *
 * Revision 1.1  2003/07/31 21:16:39  haocl
 * checking in portion of the new web interface
 *
 *
 */
