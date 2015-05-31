/* $Id: nbr_db.h,v 1.2 2004/06/10 16:57:42 zwang Exp $
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
 

  PROGRAM:  Neighbour part of SeqHound : Database header
						
  AUTHORS:  Michel Dumontier (micheld@mshri.on.ca)
            and Christopher W.V. Hogue  (hogue@mshri.on.ca)


  REVISION/CONTRIBUTION NOTES:

  March 1, 2001  Original MSH Revision.
                Hogue Lab - University of Toronto Biochemistry 
                Department and the Samuel Lunenfeld Research 
                Institute, Mount Sinai Hospital
                http://bioinfo.mshri.on.ca  hogue@mshri.on.ca
                ftp://ftp.mshri.on.ca/


  PUBLICATION TO CITE:



***************************************************************************
*/
#ifndef _NBR_DB_H
#define _NBR_DB_H

#ifdef __cplusplus
extern "C" {
#endif

#include <slri_misc.h>
#include <objslrilinkset.h>
#include <objNBlastasn.h>


#define BLASTDB 0
#define NBLASTDB 1
#define N3D 2

/************************/
/* Function definitions */
/************************/

/************************/
/*General Initialization*/
/************************/

/**********************************************************************
* OpenNPBlastDB         Open the Protein Blast database               *
* OpenNPDB              Open the Protein Neighbours database          *
* OpenN3D               Open the 3D Neighbours database
* OpenAllNbrDBs         Open Neighbour and Blast databases            *
* CloseAllNbrDBs        Close Neighbour and Blast databases           *
*
* InitCodeBaseNbr       Initialize CodeBase global data structure     *
* OpenCodeBaseNbr       Initialize CodeBase and Open All Databases    *
* CloseCodeBaseNbr      Close database and CodeBase                   *
**********************************************************************/
extern Boolean OpenNPBlastDB(Boolean CREATE);
extern Boolean OpenNPDB(Boolean CREATE);
extern Boolean OpenN3D(Boolean CREATE);
extern Boolean OpenAllNbrDBs(Boolean CREATE);
extern void CloseAllNbrDBs(Boolean bConnect);

extern void InitCodeBaseNbr(void);
extern Boolean OpenCodeBaseNbr(Boolean CREATE);
extern void CloseCodeBaseNbr(Boolean bConnect);

/***********************/
/*Data Management Calls*/
/***********************/
/**********************************************************************
* GetBlastResultsFromDB            Get the Blast Result ASN from db   *
* GetBlastSeqAlignFromDB           Get the Blast Result ASN in seqalign format *
* GetNeighboursFromDB              Get the Neighbours ASN from db     *
* Get3DNeighboursFromDB            Get the 3DNeighbours ASN from db   *

* GetNumberofNeighboursListFromDB  Get a list of the number of nbrs   *
*                                   for each db entry                 *
**********************************************************************/
extern SLRI_ERR GetBlastResultsFromDB(Int4 gi1, Int4 gi2, NBlastResultSetPtr PNTR nbrsp);
extern SLRI_ERR GetBlastSeqAlignFromDB(Int4 gi1, Int4 gi2, SeqAlignPtr PNTR psap);
extern SLRI_ERR GetNeighboursFromDB(Int4 gi, Int4 PNTR num, NBlastGiAndEvalSetPtr PNTR nbgesp);
extern SLRI_ERR Get3DNeighboursFromDB(Int4 gi, Int4 PNTR num, ILinkSetPtr PNTR p3DNeighbours);
extern SLRI_ERR GetNumberofNeighboursListFromDB(Int4 PNTR pnum, ILinkSetPtr PNTR pNumGiList, Int4 iDB);

#ifdef __cplusplus
}
#endif

#endif /*NBR_DB_H*/


/*
$Log: nbr_db.h,v $
Revision 1.2  2004/06/10 16:57:42  zwang
Add flag to determine if a database connection should be established. See bug #2324.

Revision 1.1.1.5  2002/09/04 08:29:47  root
Automated import from Sourceforge.net

Revision 1.5  2002/09/03 22:10:10  micheld
removed slri.h include from includes (executable header include)

Revision 1.4  2002/03/25 07:51:32  micheld
New genome functions. changed includes to new slrilinkset.h

Revision 1.3  2001/12/05 00:52:07  micheld
Added access to BLAST pairwise results... returns either NBLASTDB format or seqalign

Revision 1.2  2001/11/21 23:50:58  kaca
database opening controled by CREATE Boolean

Revision 1.1.1.1  2001/10/10 21:53:47  kaca
initial import

Revision 1.2  2001/08/24 20:59:44  michel
removed SEQHOUNDNBRS compile flag from library
added new functions for 3DNeighbours database retrieval
temporary fix for redundant neighbours coming from NBLAST

Revision 1.1  2001/08/02 16:13:32  michel
Added Neighbour Retrieval to SeqHound local

*/


