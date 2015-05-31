/* $Id: seqhoundapi.h,v 1.2 2004/01/13 21:49:56 rcavero Exp $
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
  January 2000
  header file for SeqHound API light-weight version

******************************************************************************************/

#ifndef _SEQHOUNDAPI_
#define _SEQHOUNDAPI_



#include <asn.h>
#include <ncbi.h>
#include <mmdbapi.h>
#include <connect/ncbi_connection.h>
#include <connect/ncbi_connutil.h>
#include <connect/ncbi_http_connector.h>
#include <objslristruc.h>
#include <objslrilinkset.h>
#include <slri_misc.h>
#include <seqhound.h>


#ifdef __cplusplus
extern "C" { /* } */
#endif


ValNodePtr My_Misc_CommaStrString2ValNodeListEx(CharPtr string, CharPtr seps, Boolean maintainOrder);
ValNodePtr My_Misc_CommaStrStringWithNull2ValNodeListEx(CharPtr string, CharPtr seps, Boolean maintainOrder);
ValNodePtr LIBCALL My_ValNodeAddStr (ValNodePtr PNTR head, Nlm_Int2 choice, Nlm_CharPtr str);
CharPtr getSubString(CharPtr auxch, Int4 i, Int4 j);


#ifdef __cplusplus
/* { */ }
#endif


#endif /* _SEQHOUNDAPI_ */

/*
$Log: seqhoundapi.h,v $
Revision 1.2  2004/01/13 21:49:56  rcavero
nblast_odbc.h it's a draf version for nblast's ODBC port.

Revision 1.1.1.3  2002/09/06 08:29:53  root
Automated import from Sourceforge.net

Revision 1.3  2002/09/05 18:24:13  micheld
replaced executable include slri.h with slri_misc.h and objslrilinkset.h

Revision 1.2  2002/03/25 07:51:32  micheld
New genome functions. changed includes to new slrilinkset.h

Revision 1.1.1.2  2001/10/10 21:53:47  kaca
initial import

Revision 1.12  2001/08/03 19:02:55  michel
Added remote API SHoundNeighboursFromGiEx (untested) and CGI call

Revision 1.11  2001/04/16 20:41:46  kaca
reflect changes in toolkit connect stuff

Revision 1.10  2001/03/27 21:23:26  kaca
removed function declaration duplicates

Revision 1.9  2001/03/26 22:33:13  kaca
created intrez_misc file and moved some API functions into it

Revision 1.8  2001/03/26 19:46:02  kaca
reorganized headers to support a single header for the local and remote APIs

Revision 1.7  2001/02/23 21:03:16  kaca
license change

Revision 1.6  2000/10/19 19:55:34  kaca
minor changes

Revision 1.5  2000/10/16 20:11:54  kaca
minor changes

Revision 1.4  2000/10/13 22:59:42  kaca
the server name is read from the config file into a global variable

Revision 1.3  2000/10/13 21:47:30  kaca
split headers into private, public and definitions

Revision 1.1.1.1  2000/09/18 17:41:55  kaca
Initial import

Revision 1.4  2000/08/14 20:43:41  gbader
Removed traces of bad programming practice where a CharPtr is passed to a variable
argument function such as printf as the format string.  This prevents user from
crashing program by entering e.g. %s in an input string.

Revision 1.3  2000/07/12 17:31:23  kaca
added includes for libslri

*/
