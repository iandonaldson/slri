/* $Id: intrez_cfg.h,v 1.4 2005/05/02 14:20:55 rcavero Exp $
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


******************************************************************************************/

#ifndef _INTREZ_CFG_
#define _INTREZ_CFG_

#include <ncbi.h>

#ifdef __cplusplus
extern "C" { /* } */
#endif

#define Cfg_intrez_new	Cfg_shound_new


/***********************************************
*  config file function calls
***********************************************/
void Cfg_shound_new(void);
void Cfg_FreeMem(void);
CharPtr Cfg_GetCgiPath(void);
Int4 Cfg_GetMaxQueryTime(void);
CharPtr Cfg_GetPrecompuPath(void);
Int4 Cfg_GetIntValue(CharPtr section, CharPtr key);
Int4 Cfg_GetMaxQuery(void);
CharPtr Cfg_GetIndexFile(void);
Boolean Cfg_IncrementQueryCount(void);
Int4 Cfg_GetQueryCount(void);
Boolean Cfg_IncrementMaxQuery(void);
Boolean Cfg_SetQueryCount(Int4 number);
Boolean Cfg_SetMaxQuery(Int4 querycount);
Boolean Cfg_ResetQueryCount(void);
Boolean Cfg_ResetMaxQuery(void);
Boolean Cfg_exists(void);
Boolean SHoundModule(CharPtr psection);

typedef struct SHound_Config_Info {
	CharPtr Cgi_path;
	CharPtr precompu_path;
	Int4 MaxQueries;
	Int4 MaxQueryTime;
	Int4 QueryCount;
	CharPtr indexFile;
	Int4 godb;
	Int4 lldb;
	Int4 taxdb;
	Int4 neigdb;
	Int4 strucdb;
	Int4 gendb;
	Int4 redundb;
	Int4 cddb;
    Int4 rpsdb;
    Int4 dbxref;
} SHoundCfg, PNTR SHoundCfgPtr;

/*structure of the precomputed query index file*/
typedef struct Tax_index{
	Int4 tax_id;
	Int4 num_gis;
	Int4 num_DNA_gis;
	Int4 num_proteins_gis;
}Tax_index;

/*mehtods that retrieve information from the index file*/
/*checks to see a taxid is precomputed.  if it is then
if fills up an array of all the gis and returns the number of
gis it just copied over.  Must hand the function a pointer to an
Int4 and remember to free it once done.*/
Int4 Check_precomputed_index(Int4 ** gis, Int4 taxid, CharPtr TaxType);




#ifdef __cplusplus
/* { */ }
#endif



#endif  /* _INTREZ_CFG_ */

/*
$Log: intrez_cfg.h,v $
Revision 1.4  2005/05/02 14:20:55  rcavero
roll-bak version 1.3

Revision 1.2  2004/01/13 21:49:55  rcavero
nblast_odbc.h it's a draf version for nblast's ODBC port.

Revision 1.1.1.3  2002/04/04 09:27:11  root
Automated import from Sourceforge.net

Revision 1.3  2002/04/03 23:08:29  kaca
added RPSDB module

Revision 1.2  2001/11/21 23:50:58  kaca
database opening controled by CREATE Boolean

Revision 1.1.1.2  2001/10/10 21:53:47  kaca
initial import

Revision 1.3  2001/10/10 13:33:36  kaca
added SeqHound modules tracking

Revision 1.2  2001/04/06 22:41:42  kaca
added new fncts

Revision 1.1  2001/03/26 19:46:02  kaca
reorganized headers to support a single header for the local and remote APIs

*/


