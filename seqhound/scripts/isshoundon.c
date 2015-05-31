/* $Id: isshoundon.c,v 1.1.1.2 2002/10/29 09:40:31 root Exp $
*******************************************************************************
$Copyright(C) 2000 Mount Sinai Hospital (MSH)

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
           Katerina Michalickova
	   Hao Lieu
	   and Christopher W.V. Hogue
           hogue@mshri.on.ca


  REVISION/CONTRIBUTION NOTES:

  June 1, 2000  Original MSH Revision.
                Hogue Lab - University of Toronto Biochemistry
                Department and the Samuel Lunenfeld Research
                Institute, Mount Sinai Hospital
                http://bioinfo.mshri.on.ca  hogue@mshri.on.ca
                ftp://ftp.mshri.on.ca/


  PUBLICATION TO CITE:


  DESCRIPTION
   tests if SeqHound system is working

*****************************************************************************************/
#include <seqhound.h>

Int2 Main()
{

   ErrSetOptFlags(EO_LOG_SEVERITY | EO_LOG_CODES | EO_LOG_FILELINE | EO_LOG_USRFILE);
   ErrSetLogfile("isshoundonlog", ELOG_BANNER);
   ErrSetLogLevel(SEV_INFO);
   ErrSetMessageLevel(SEV_MAX);

   if(!SHoundInit(FALSE, "isshoundon"))
      return 1;

   SHoundFini();
   ErrPostEx(SEV_INFO,0,0, "Done.");
   return 0;
}

/*
$Log: isshoundon.c,v $
Revision 1.1.1.2  2002/10/29 09:40:31  root
Automated import from Sourceforge.net

Revision 1.3  2002/10/28 16:26:38  kaca
added final log message; the ncbi_connect logs were difficult to read otherwise

Revision 1.1  2002/01/21 20:08:52  kaca
new cron test; tests if the SeqHound remote API is on

*/

