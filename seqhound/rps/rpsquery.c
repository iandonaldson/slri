/* $Id: rpsquery.c,v 1.2 2004/01/06 22:32:14 skuznets Exp $
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
  Locus Link flat file parser
  Part of SeqHound

****************************************************************************/
#include <seqhound.h>

void PrintRecord(StRpsdbPtr results)
{
  Int4 count=0;
  while(results!=NULL){
    fprintf(stdout, "%ld: %s: %ld: %e: %ld: %ld: %ld: %ld:\n",
	    (long)results->gi,(char*)IsNullStr(results->dom_id),(long)results->from,
	    (double)results->evalue,(long)results->c_missing, (long)results->n_missing,
	    (long)results->len, (long)results->num_of_dom);
    results=results->next;
    count++;
  }
  ErrPostEx(SEV_INFO,0,0,"Printed %d records.",count);
}



Int2 Main(void)
{
  StRpsdbPtr head = NULL;
  ValNodePtr vnp2 = NULL,result = NULL, vnp = NULL;
  CharPtr pchar = NULL;
  ValNodePtr i = NULL;

  ErrSetOptFlags(EO_LOG_SEVERITY | EO_LOG_CODES | EO_LOG_FILELINE | EO_LOG_USRFILE);
  ErrSetLogfile("rpsquerylog", ELOG_BANNER);
  ErrSetLogLevel(SEV_INFO);
  ErrSetMessageLevel(SEV_MAX);	


  if(!SHoundInit(FALSE, "rpsquery"))
  {
    printf("Cannot open SeqHound.\n");
    return 1;
  }

  printf("Starting the query\n");

  printf("RPS FUNCTIONS\nGetGisByDomainId\n");
  i = SHoundGetGisByDomainId("smart00038");
  PrintValNodeOfInt(i);
  i = ValNodeFree(i);

  printf("GetGisByDomainAndEvalue\n");
  i = SHoundGetGisByDomainIdAndEvalue("smart00038", 1.0);
  PrintValNodeOfInt(i);
  i = ValNodeFree(i);

  printf("GetGisByNumberOfDomains\n");
  i = SHoundGetGisByNumberOfDomains(3);
  PrintValNodeOfInt(i);
  i = ValNodeFree(i);

  printf("GetDomainIdFromLabel\n");
  vnp = SHoundGetDomainIdFromLabel("SH3");
  PrintValNodeOfStr(vnp);
  vnp = ValNodeFreeData(vnp);

  printf("DomainLabelFromDomainId\n");
  pchar = SHoundGetDomainLabelFromDomainId("smart00038");
  printf("%s\n", IsNullStr(pchar));

  printf("GetDomainsFromGi\n");
  head = SHoundGetDomainsFromGi(4557225);
  PrintRecord(head);
  head = StRpsdbFree(head);

  printf("GetDomainsFromGiWithEvalue\n");
  head = SHoundGetDomainsFromGiWithEvalue(4557225, 1);
  PrintRecord(head);
  head = StRpsdbFree(head);

  SHoundFini();
  ErrPostEx(SEV_INFO, 0, 0, "Done.");
  printf("Done.\n");

}

/*
$Log: rpsquery.c,v $
Revision 1.2  2004/01/06 22:32:14  skuznets
Code audit has been done sucessfully.
Possible NULL pointer issues has been eliminated.

Revision 1.1.1.2  2002/07/09 08:28:30  root
Automated import from Sourceforge.net

Revision 1.4  2002/07/08 16:56:02  kaca
renamed two RPS-BLAST functions Accession is DomainId

Revision 1.3  2002/05/13 19:22:58  kaca
added memory freeing

Revision 1.2  2002/05/13 19:08:28  haocl
updated rps test file

Revision 1.1  2002/05/13 17:57:04  haocl
import of test for rps functions

Revision 1.3  2002/01/15 16:36:42  kaca
changed include file

Revision 1.2  2001/12/11 20:19:51  kaca
added Log and Id keywords

*/
