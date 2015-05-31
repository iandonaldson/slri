/* $Id: reskloodge.c,v 1.2 2004/01/06 22:32:12 skuznets Exp $
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
  1) tests complete genomes
  2) resets complete genomes in SeqHound
******************************************************************************************/

#include <intrez_cb.h>
#include <seqhound.h>


#define LOGING "reskloodgelog"

#define NUMARGS 1
Args myargs[NUMARGS] = {
       {"Reset all kloodge assignments?", "FALSE", NULL, NULL, TRUE, 'o', ARG_BOOLEAN, 0.0, 0, NULL}
	};


Int2 Main()
{
    int rc;
    Int4 count = 0;
    ValNodePtr pvntaxid = NULL, pvntaxidhead = NULL, pvntemp = NULL;
    ILinkSetPtr pil = NULL;
    CharPtr ptaxname = NULL;


    ErrSetOptFlags(EO_LOG_SEVERITY | EO_LOG_CODES | EO_LOG_FILELINE | EO_LOG_USRFILE);
    ErrSetLogfile(LOGING, ELOG_BANNER);
    ErrSetLogLevel(SEV_INFO);
    ErrSetMessageLevel(SEV_MAX);

    if ( ! GetArgs("reskloodge",NUMARGS, myargs))
           return 1;


    if(!SHoundInit(FALSE, "reskloodge"))
    {
      ErrPostEx(SEV_ERROR,0,0, "SHoundInit failed.\n");
      return 1;
    }
    else
      printf("SHound inited.\n");


    pil = ILinkSetNew();
    if(myargs[0].intvalue)  /* used to reset all kloodge fields in TAXGI */
    {
       printf("\nReseting...\n");
       for (rc = d4top(pd4Taxgi); rc == r4success; rc = d4skip(pd4Taxgi, (long) 1))
       {
          f4assignLong(pf4tgKloodge, 0);
       }
    }

    /*test all genomic assignments */
    printf("Testing...\n");
    pvntaxidhead = pvntaxid = SHoundAllGenomes();
    while(pvntaxid != NULL)
    {
      pvntemp = SHoundProteinsFromOrganism(pvntaxid->data.intvalue, CHROM_ALL);
      ptaxname = SHoundGetTaxNameFromTaxID(pvntaxid->data.intvalue);
      if (ptaxname)
      {
        printf("Organism %53s  proteins %ld\t", ptaxname, ValNodeLen(pvntemp));
        pvntemp = SHoundDNAFromOrganism(pvntaxid->data.intvalue, CHROM_ALL);
        printf("DNA %ld\n",ValNodeLen(pvntemp));
	pvntemp = ValNodeFree(pvntemp);
	ptaxname = MemFree(ptaxname);
      }
      else
        printf("No tax name for taxid %ld.\n", (long) pvntaxid->data.intvalue);
      pvntaxid = pvntaxid->next;

      /*pvntemp = SHoundProteinsFromOrganism(pvntaxid->data.intvalue, CHROM_ALL);
      if ((SHound3DListFromTaxID(pvntaxid->data.intvalue, &pil) == 0) && (pil != NULL))*/
          /*printf("Organism %s\ttaxid %ld\t %ld structures\n", IsNullStr(SHoundGetTaxNameFromTaxID(pvntaxid->data.intvalue)), pvntaxid->data.intvalue, pil->num);*/
          /*printf("%ld\t%ld\t%s\t%ld\n", pvntaxid->data.intvalue, pil->num, IsNullStr(SHoundGetTaxNameFromTaxID(pvntaxid->data.intvalue)), ValNodeLen(pvntemp));
      else
          printf("%ld\t0\t%s\t%ld\n", pvntaxid->data.intvalue, IsNullStr(SHoundGetTaxNameFromTaxID(pvntaxid->data.intvalue)), ValNodeLen(pvntemp));
      pil = ILinkSetFree(pil);
      pvntemp = ValNodeFree(pvntemp);
      pvntaxid = pvntaxid->next;*/
    }
    pvntaxidhead = ValNodeFree(pvntaxidhead);

    SHoundFini();
    printf("Done.\n");
    return 0;
}
/*
$Log: reskloodge.c,v $
Revision 1.2  2004/01/06 22:32:12  skuznets
Code audit has been done sucessfully.
Possible NULL pointer issues has been eliminated.

Revision 1.1.1.3  2002/08/07 08:29:41  root
Automated import from Sourceforge.net

Revision 1.4  2002/08/06 19:48:03  kaca
null printing fix

Revision 1.3  2002/05/13 15:26:11  kaca
formatting changes

Revision 1.2  2002/03/04 18:37:35  kaca
variable name typo fix

Revision 1.1  2002/03/04 18:34:40  kaca
complete genomes test

*/

