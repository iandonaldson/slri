/* $Id: goparser.c,v 1.2 2004/10/01 17:38:00 zwang Exp $
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

#include <go_parser.h>
#include <go_db.h>

#define LOGING "goparserlog"

#define NUMARGS 1
Args myargs[NUMARGS] = {
        {"Is this an update","F", NULL, NULL, TRUE, 'u', ARG_BOOLEAN, 0.0, 0, NULL}
  };



Int2 Main(void)
{
   Boolean isUpdate = FALSE;

   if (! GetArgs("goparser",NUMARGS,myargs))
        return 1;

   ErrSetOptFlags(EO_LOG_SEVERITY | EO_LOG_CODES | EO_LOG_FILELINE | EO_LOG_USRFILE);
   ErrSetLogfile(LOGING, ELOG_BANNER);
   ErrSetLogLevel(SEV_INFO);
   ErrSetMessageLevel(SEV_MAX);

   if(FileLength("function.ontology") == 0)
   {
     ErrPostEx(SEV_ERROR,0,0,"Main: Cannot find function.ontology file in current directory.");
     return 1;
   }
   if(FileLength("process.ontology") == 0)
   {
     ErrPostEx(SEV_ERROR,0,0,"Main: Cannot find process.ontology file in current directory.");
     return 1;
   }
   if(FileLength("component.ontology") == 0)
   {
     ErrPostEx(SEV_ERROR,0,0,"Main: Cannot find component.ontology file in current directory.");
     return 1;
   }
   GO_InitCodeBase();
   GO_OpenAllDBs(TRUE);

   if (myargs[0].intvalue)
      isUpdate = TRUE;

   if (!GO_StartTransaction()) {
      ErrPostEx(SEV_ERROR, 0, 0, "Cannot start transaction!");
      GO_InitUndoCodeBase();
      return 1;
   }

   if (isUpdate) {
     /* delete all exisiting records */
     if (!DeleteAllRecordsInGO()) {
        ErrPostEx(SEV_ERROR,0,0, "Cannot delete all records from GO tables!");
        if (!GO_RollBackTransaction()) {
           ErrPostEx(SEV_ERROR,0,0, "Cannot roll back!");
           GO_InitUndoCodeBase();
           return 1;
        }
        GO_InitUndoCodeBase();
        return 1;
     }
   }

   GODB_ParseFile("function.ontology");
   GODB_ParseFile("process.ontology");
   GODB_ParseFile("component.ontology");
   /*GODB_PrintAllRecords();*/

   if (!GO_CommitTransaction()) {
      ErrPostEx(SEV_ERROR, 0, 0, "Cannot commit transaction!");
      if (!GO_RollBackTransaction()) {
         ErrPostEx(SEV_ERROR,0,0, "Cannot roll back!");
         GO_InitUndoCodeBase();
         return 1;
      }
      GO_InitUndoCodeBase();
      return 1;
   }

   GO_InitUndoCodeBase();
   ErrPostEx(SEV_INFO,0,0, "Main: Done!");
   return 0;
}

/*
$Log: goparser.c,v $
Revision 1.2  2004/10/01 17:38:00  zwang
Added ability of building a transactional database.
Added input parameter to specify if it's an update.
See bug 3441.

Revision 1.1.1.4  2003/08/21 08:48:08  root
Automated import from Sourceforge.net

Revision 1.4  2003/08/20 19:13:04  haocl
minor modifications, remove printf statements

Revision 1.3  2002/07/08 18:13:01  kaca
fixed compiler varnings

Revision 1.2  2002/03/26 16:14:39  kaca
moved table opening into goparser; it is done only once

Revision 1.1.1.1  2001/10/10 21:53:47  kaca
initial import

Revision 1.3  2001/09/21 17:38:44  kaca
removed obsolete GODBGetRecordByID

*/

