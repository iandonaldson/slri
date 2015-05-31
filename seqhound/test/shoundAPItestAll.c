/* $Id: shoundAPItestAll.c,v 1.1 2004/03/23 20:29:25 vgu Exp $
*******************************************************************************
$Copyright (C) 2000 Mount Sinai Hospital (MSH)

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
          vgu@blueprint.org

  REVISION/CONTRIBUTION NOTES:

  March 10, 2004 Original Blueprint Revision
                 Samuel Lunenfeld Research
                 Institute, Mount Sinai Hospital
                 http://bioinfo.mshri.on.ca  hogue@mshri.on.ca
                 ftp://ftp.mshri.on.ca/


  PUBLICATION TO CITE:


  DESCRIPTION
    January 1999
    tests SeqHound remote API functions;
    October 2001
    Tests the all functions in current SeqHound but neighbours.
    The default params gave positive results at the time of creating this test.
    If functions fail; it could be caused by a function param which ceased to
    exist.
    Some of the params can be changed through input arguments; some are fixed for
    the sake of simplicity.


*****************************************************************************************/
#include <seqhound.h>
#include <CuTest.h>
// #include "CuTest.c"

#include "shoundAPItest.h"


#define LOGING "shoundAPItestlog"


/************ Utility Function for printing results to log file ***/
//void logWrite (FILE *fp, char *str1, char *str2, char *str3, char *str4, char *str5);
CuSuite* ShoundTestGetSuite ();
//extern void SetUpInputList();
void RunAllTests (void)
{

  SetUpInputList();
  CuString *output = CuStringNew();
  CuSuite* suite = CuSuiteNew();
  CuSuiteAddSuite(suite, ShoundTestGetSuite());
  CuSuiteRun(suite);
  CuSuiteSummary(suite, output);
  CuSuiteDetails(suite, output);
  printf("%s\n", output->buffer);
}

Int2 Main ()
{
  if (SHoundInitReadOnly(FALSE, "shoundAPItestAll"))
  {
    printf("SHoundInit OK.\n");
  }
  else
  {
    printf("SHoundInit FAILED.\n");
    return 1;
  }
  if (OpenMMDBAPI(0, NULL))
  {
    printf("OpenMMDBAPI OK.\n");
  }
  else
  {
    printf("OpenMMDBAPI Failed.\n");
    return 2;
  }
  RunAllTests();
  SHoundFini();
  printf ("Done with shoundAPITestAll\n");
  return 0;
}
