/* $Id: comp_redund.c,v 1.1.1.4 2002/10/17 08:34:34 root Exp $
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
  Gene Ontology flat file parser
  Part of SeqHound

****************************************************************************/
#include <ncbi.h>
#include <comp_redund_db.h>

Args arg[2] = {
  {"path to old REDUND table", "./redund_old", NULL, NULL, FALSE, 'o', ARG_STRING, 0.0, 0, NULL},
  {"path to new REDUND table", "./redund_new", NULL, NULL, FALSE, 'n', ARG_STRING, 0.0, 0, NULL}
};

Int2 Main()
{
  CharPtr strdate = NULL, filebase = NULL, filetobl = NULL, filetodel = NULL;
  FILE *fptobl = NULL, *fptodel = NULL;
  NCBI_DatePtr date = NULL;
  RedundDB pRed_old = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};
  RedundDB pRed_new = {NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL};

   ErrSetOptFlags(EO_LOG_SEVERITY | EO_LOG_CODES | EO_LOG_FILELINE | EO_LOG_USRFILE);
   ErrSetLogfile("comp_redundlog", ELOG_BANNER);
   ErrSetLogLevel(SEV_INFO);
   ErrSetMessageLevel(SEV_MAX);

  if(!GetArgs("comp_redund", 2, arg)){
    return 1;
  }
  date = DateCurr();
  strdate = (CharPtr) GetStringDate(date);
  DateFree(date);

  filebase = (CharPtr) MemNew(16*sizeof(Char));
  strcpy(filebase, strdate);
  filetodel = (CharPtr) MemNew(20*sizeof(Char));
  filetobl = (CharPtr) MemNew(20*sizeof(Char));
  strcpy(filetobl, filebase);
  strcpy(filetodel, filebase);
  strcat(filetobl, "toblast");
  strcat(filetodel, "todelete");
  fptobl = FileOpen(filetobl, "w");
  if(fptobl == NULL)
  {
    ErrPostEx(SEV_ERROR,0,0, "Could not create file.");
    MemFree(strdate);
    MemFree(filebase);
    MemFree(filetodel);
    MemFree(filetobl);
    return 1;
  }
  fptodel = FileOpen(filetodel, "w");
  if(fptodel == NULL)
  {
    ErrPostEx(SEV_ERROR,0,0, "Could not create file.");
    MemFree(strdate);
    MemFree(filebase);
    MemFree(filetodel);
    MemFree(filetobl);
    return 1;
  }


  MemFree(strdate);
  MemFree(filebase);
  MemFree(filetodel);
  MemFree(filetobl);

  REDUND_InitCodeBase();


  pRed_new.next = &pRed_old;
  REDUND_OpenAllDBs(FALSE, &pRed_new, arg[0].strvalue, arg[1].strvalue);
  RedundantCheck(&pRed_new, &pRed_old, fptobl);
  RedundantCheck(&pRed_old, &pRed_new, fptodel);

  FileClose(fptodel);
  FileClose(fptobl);
  return 0;
}

/*
$Log: comp_redund.c,v $
Revision 1.1.1.4  2002/10/17 08:34:34  root
Automated import from Sourceforge.net

Revision 1.5  2002/10/15 20:22:48  kaca
added logging

Revision 1.4  2002/09/25 18:25:24  kaca
produces a list to blast and list to delete

Revision 1.3  2002/08/22 18:31:58  haocl
modified to use ncbi command line parsing

Revision 1.2  2002/08/22 17:33:40  kaca
changed include from redund_db.h comp_redund_db.h

Revision 1.1  2002/08/21 17:46:44  haocl
renamed files from redund.c to comp_redund.c, make.redund to make.comp_red

Revision 1.2  2002/08/19 17:37:32  haocl
modified to have path to redund_old and redund_new
passed in command line

Revision 1.1  2002/08/19 14:57:14  haocl
initial import of redund


*/

