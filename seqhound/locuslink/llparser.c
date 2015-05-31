/* $Id: llparser.c,v 1.3 2004/12/14 19:44:45 zwang Exp $
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

  DESCRIPTION
 ' Gene Ontology flat file parser
  Part of SeqHound

****************************************************************************/
#include <ll_db.h>
#include <ll_parser.h>
#include <slri_misc.h>

#define LOGING "llparserlog"
#define NUMARGS 2
Args myargs[NUMARGS] = {
        {"Input file","LL_tmpl",NULL,NULL,TRUE,'i',ARG_FILE_IN,0.0,0,NULL},
        {"Is this an update","F",NULL,NULL,TRUE,'u',ARG_BOOLEAN,0.0,0,NULL}
  };

Int2 Main(void)
{
   ErrSetOptFlags(EO_LOG_SEVERITY | EO_LOG_CODES | EO_LOG_FILELINE | EO_LOG_USRFILE);
   ErrSetLogfile(LOGING, ELOG_BANNER);
   ErrSetLogLevel(SEV_INFO);
   ErrSetMessageLevel(SEV_MAX);

   if (! GetArgs("llparser",NUMARGS,myargs))
        return 1;

   if(FileLength(myargs[0].strvalue) == 0)
   {
     ErrPostEx(SEV_ERROR,0,0,"Main: Cannot find %s file.", IsNullStr(myargs[0].strvalue));
     return 1;
   }

   if(LL_ParseFile(myargs[0].strvalue, myargs[1].intvalue) != 1)
   {
     ErrPostEx(SEV_ERROR,0,0,"Main: LL_ParseFile failed.");
     return 1;
   }
   ErrPostEx(SEV_INFO,0,0, "Main: Done!");
   return 0;
}

/*
$Log: llparser.c,v $
Revision 1.3  2004/12/14 19:44:45  zwang
Added ability of building a transactional database.
Added input parameter to specify if it's an update.
See bug 3441.

Revision 1.2  2004/01/06 22:32:13  skuznets
Code audit has been done sucessfully.
Possible NULL pointer issues has been eliminated.

Revision 1.1.1.4  2002/07/09 08:28:30  root
Automated import from Sourceforge.net

Revision 1.4  2002/07/08 18:01:11  kaca
fixed compiler varnings

Revision 1.3  2002/06/24 17:34:18  kaca
added argument default value

Revision 1.2  2002/06/06 18:54:13  kaca
added argument for input file

Revision 1.1.1.1  2001/10/10 21:53:47  kaca
initial import

Revision 1.1  2001/09/06 17:47:53  kaca
changes to accomodate new locus link official parser

*/

