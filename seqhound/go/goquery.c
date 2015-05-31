/* $Id: goquery.c,v 1.2 2004/01/06 22:32:12 skuznets Exp $
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

  March 1, 2001  Original MSH Revision.
                Hogue Lab - University of Toronto Biochemistry
                Department and the Samuel Lunenfeld Research
                Institute, Mount Sinai Hospital
                http://bioinfo.mshri.on.ca  hogue@mshri.on.ca
                ftp://ftp.mshri.on.ca/


  PUBLICATION TO CITE:

  DESCRIPTION:
  Gene Ontology database management
  Part of SeqHound

****************************************************************************/

#include <go_query.h>
#include <seqhound.h>
#include <intrez_misc.h>


Int2 Main(void)
{
  ValNodePtr vnp2 = NULL, vnp = NULL;
  CharPtr pchar = NULL;
  Int4 i = 0;
  CharPtr pi = NULL;

  ErrSetOptFlags(EO_LOG_SEVERITY | EO_LOG_CODES | EO_LOG_FILELINE | EO_LOG_USRFILE);
  ErrSetLogfile("goquerylog", ELOG_BANNER);
  ErrSetLogLevel(SEV_INFO);
  ErrSetMessageLevel(SEV_MAX);	

  /*GODB_InitCodeBase();
  GODB_OpenAllDBs();*/

  if(!SHoundInit(FALSE, "goquery"))
  {
    printf("Cannot open SeqHound.\n");
    return 1;
  }

  printf("Starting the query\n");
  /*GODB_PrintAllRecords();*/

  printf("Children of 7612\n");
  vnp = SHoundGODBGetChildrenOf(7612);
  PrintValNodeOfInt(vnp);
  ValNodeFree(vnp);

  printf("Children of 4907\n");
  vnp = SHoundGODBGetChildrenOf(4907);
  PrintValNodeOfInt(vnp);
  ValNodeFree(vnp);

  printf("Children of 5381\n");
  vnp = SHoundGODBGetChildrenOf(5381);
  PrintValNodeOfInt(vnp);
  ValNodeFree(vnp);

  printf("Parent of 5381\n");
  vnp = SHoundGODBGetParentOf(5381);
  PrintValNodeOfInt(vnp);
  ValNodeFree(vnp);

  printf("Parent of 5367\n");
  vnp = SHoundGODBGetParentOf(5367);
  PrintValNodeOfInt(vnp);
  ValNodeFree(vnp);

  printf("Parent of 16883\n");
  vnp = SHoundGODBGetParentOf(16883);
  PrintValNodeOfInt(vnp);
  ValNodeFree(vnp);

  printf("Name of 7625\n");
  pchar = SHoundGODBGetNameByID(7625);
  printf("%s\n", IsNullStr(pchar));
  MemFree(pchar);

  printf("Name of 16882\n");
  pchar = SHoundGODBGetNameByID(16882);
  printf("%s\n", IsNullStr(pchar));
  MemFree(pchar);

  printf("Name of 15412\n");
  pchar = SHoundGODBGetNameByID(15412);
  printf("%s\n", IsNullStr(pchar));
  MemFree(pchar);

  printf("All ancestors of 7627\n");
  vnp2 = SHoundGODBGetAllAncestors(7627);
  PrintValNodeOfInt(vnp2);
  ValNodeFree(vnp2);

  printf("All ancestors of 5141\n");
  vnp2 = SHoundGODBGetAllAncestors(5141);
  PrintValNodeOfInt(vnp2);
  ValNodeFree(vnp2);

  printf("All children of 7627\n");
  vnp2 = SHoundGODBGetAllChildren(7627);
  PrintValNodeOfInt(vnp2);
  ValNodeFree(vnp2);

  printf("All children of 16787\n");
  vnp2 = SHoundGODBGetAllChildren(16787);
  PrintValNodeOfInt(vnp2);
  ValNodeFree(vnp2);

  printf("Getting those records with EC\n");
  vnp2 = SHoundGODBGetRecordByReference("EC");
  PrintValNodeOfInt(vnp2);
  ValNodeFree(vnp2);

  printf("Getting those records with ISBN\n");
  vnp2 = SHoundGODBGetRecordByReference("ISBN");
  PrintValNodeOfInt(vnp2);
  ValNodeFree(vnp2);

  printf("Geting classification\n");
  pi = SHoundGODBGetClassification(5935);
  printf("%s\n", IsNullStr(pi));

  ErrPostEx(SEV_INFO, 0, 0, "Done.");
  /*GODB_InitUndoCodeBase();*/
  SHoundFini();
  return 0;

}

/*
$Log: goquery.c,v $
Revision 1.2  2004/01/06 22:32:12  skuznets
Code audit has been done sucessfully.
Possible NULL pointer issues has been eliminated.

Revision 1.1.1.3  2002/01/16 09:23:29  root
Automated import from Sourceforge.net

Revision 1.4  2002/01/15 17:38:23  kaca
disabled SHoundGODBGetHierarchyLevel due to a wrong logic

Revision 1.3  2002/01/15 16:36:23  kaca
changed include file

Revision 1.2  2001/12/11 20:12:38  kaca
added Id and Log keywords

*/
