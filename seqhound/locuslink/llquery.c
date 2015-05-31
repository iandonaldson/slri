/* $Id: llquery.c,v 1.4 2004/01/22 19:51:04 zwang Exp $
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
#include <intrez_misc.h>

Int2 Main(void)
{
  ValNodePtr vnp2 = NULL, vnp = NULL;
  ValNodePtr goList = NULL, omimList = NULL;
  CharPtr pchar = NULL;
  ValNodePtr i = NULL, giList = NULL, k = NULL, lllist = NULL;
  ValNodePtr cddidlist = NULL;
  ValNodePtr ecode = NULL;
  FloatHi f = 0;

  ErrSetOptFlags(EO_LOG_SEVERITY | EO_LOG_CODES | EO_LOG_FILELINE | EO_LOG_USRFILE);
  ErrSetLogfile("llquerylog", ELOG_BANNER);
  ErrSetLogLevel(SEV_INFO);
  ErrSetMessageLevel(SEV_MAX);	


  if(!SHoundInit(FALSE, "goquery"))
  {
    printf("Cannot open SeqHound.\n");
    return 1;
  }

  /*LLDB_InitCodeBase();
  LLDB_OpenAllDBs();*/

  ValNodeAddInt(&lllist, 0, 1);
  ValNodeAddInt(&lllist, 0, 2);
  ValNodeAddInt(&lllist, 0, 13);

  ValNodeAddInt(&goList, 0, 5737);
  ValNodeAddInt(&goList, 0, 8033);
  ValNodeAddInt(&goList, 0, 4867);
  ValNodeAddInt(&goList, 0, 6419);

  ValNodeAddInt(&omimList, 0, 137150);
  ValNodeAddInt(&omimList, 0, 103950);
  ValNodeAddInt(&omimList, 0, 108345);

  ValNodeCopyStr(&cddidlist, 0, "pfam00202");
  ValNodeCopyStr(&cddidlist, 0, "pfam01835");
  ValNodeCopyStr(&cddidlist, 0, "pfam00079");

  printf("Starting the query\n");

  printf("GO FUNCTIONS\nGi From GOID\n");
  i = SHoundGiFromGOID(8320);
  PrintValNodeOfInt(i);
  i = ValNodeFree(i);

  printf("Gi From GOID List\n");
  i = SHoundGiFromGOIDList(goList);
  PrintValNodeOfInt(i);
  i = ValNodeFree(i);

  printf("GOID from Gi\n");
  vnp2 = SHoundGOIDFromGi(4557225);
  PrintValNodeOfInt(vnp2);
  vnp2 = ValNodeFree(vnp2);

  printf("GOID From Gi List\n");
  ValNodeAddInt(&giList, 0, 4501841);
  ValNodeAddInt(&giList, 0, 4557225);
  k = SHoundGOIDFromGiList(giList);
  PrintValNodeOfInt(k);
  k = ValNodeFree(k);

  printf("E code From Gi\n");
  vnp = SHoundGOECodeFromGiAndGOID(4501841, 4893);
  PrintValNodeOfStr(vnp);
  vnp = ValNodeFreeData(vnp);

  printf("GO PMID From Gi\n");
  vnp = SHoundGOPMIDFromGiAndGOID(4501841, 4325);
  PrintValNodeOfInt(vnp);
  vnp = ValNodeFree(vnp);

  printf("\n\nOMIM FUNCTIONS\nGi From OMIM\n");
  vnp = SHoundGiFromOMIM(103950);
  PrintValNodeOfInt(vnp);
  vnp = ValNodeFree(vnp);

  printf("Gi From OMIM List\n");
  vnp = SHoundGiFromOMIMList(omimList);
  PrintValNodeOfInt(vnp);
  vnp = ValNodeFree(vnp);

  printf("OMIM from Gi\n");
  vnp = SHoundOMIMFromGi(4505335);
  PrintValNodeOfInt(vnp);
  vnp = ValNodeFree(vnp);

  printf("OMIM From Gi List\n");
  vnp = SHoundOMIMFromGiList(giList);
  PrintValNodeOfInt(vnp);
  vnp = ValNodeFree(vnp);

  printf("\n\nLL FUNCTIONS\nGi From LLID\n");
  vnp = SHoundGiFromLLID(9);
  PrintValNodeOfInt(vnp);
  vnp = ValNodeFree(vnp);

  printf("Gi From LLID List\n");
  vnp = SHoundGiFromLLIDList(lllist);
  PrintValNodeOfInt(vnp);
  vnp = ValNodeFree(vnp);

  printf("LLID From Gi\n");
  printf("%d\n",SHoundLLIDFromGi(4501843));

  printf("LLID From Gi List\n");
  vnp = SHoundLLIDFromGiList(giList);
  PrintValNodeOfInt(vnp);
  vnp = ValNodeFree(vnp);

  printf("Locus From Gi\n");
  pchar = SHoundLocusFromGi(4501843);
  printf("%s\n", IsNullStr(pchar));
  pchar = MemFree(pchar);

  printf("\n\nCDD FUNCTIONS\nGi From CDDID\n");
  vnp = SHoundGiFromCDDID("EPEND");
  PrintValNodeOfInt(vnp);
  vnp = ValNodeFree(vnp);

  printf("Gi From CDDID List\n");
  vnp = SHoundGiFromCDDIDList(cddidlist);
  PrintValNodeOfInt(vnp);
  vnp = ValNodeFree(vnp);

  printf("CDDID From Gi\n");
  vnp = SHoundCDDIDFromGi(4557225);
  PrintValNodeOfStr(vnp);
  vnp = ValNodeFreeData(vnp);

  printf("CDDID From Gi List\n");
  vnp = SHoundCDDIDFromGiList(giList);
  PrintValNodeOfStr(vnp);
  vnp = ValNodeFreeData(vnp);

  printf("CDD Score from Gi\n");
  printf("%f\n", SHoundCDDScoreFromGi(4557225, "pfam00207"));


  ValNodeFree(lllist);
  ValNodeFree(goList);
  ValNodeFree(omimList); 
  ValNodeFree(giList);
  ValNodeFreeData(cddidlist);

  SHoundFini();
  ErrPostEx(SEV_INFO, 0, 0, "Done.");
  printf("Done.\n");

}

/*
$Log: llquery.c,v $
Revision 1.4  2004/01/22 19:51:04  zwang
Include intrez_misc.h.

Revision 1.3  2004/01/06 22:32:13  skuznets
Code audit has been done sucessfully.
Possible NULL pointer issues has been eliminated.

Revision 1.2  2003/10/01 19:56:15  haocl
[SHoundECode|SHoundPMID]FromGi to [SHoundECode|SHoundPMID]GiAndGOID

Revision 1.1.1.3  2002/01/16 09:23:30  root
Automated import from Sourceforge.net

Revision 1.3  2002/01/15 16:36:42  kaca
changed include file

Revision 1.2  2001/12/11 20:19:51  kaca
added Log and Id keywords

*/
