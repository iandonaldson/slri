/* $Id: histparser.c,v 1.3 2004/05/14 20:34:15 zwang Exp $
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
#include <hist_db.h>
#include <intrez_db.h>

#define LOGING "histparserlog"

#define NUMARGS 1
Args myargs[NUMARGS] = {
        {"Initialize HISTDB table",NULL, NULL, NULL, FALSE, 'n', ARG_BOOLEAN, 0.0, 0, NULL}
  };

Int2 Main()
{

  if (! GetArgs("histparser",NUMARGS,myargs))
	return 1;

  ErrSetOptFlags(EO_LOG_SEVERITY | EO_LOG_CODES | EO_LOG_FILELINE | EO_LOG_USRFILE);
  ErrSetLogfile(LOGING, ELOG_BANNER);
  ErrSetLogLevel(SEV_INFO);
  ErrSetMessageLevel(SEV_MAX);

  if(InitCodeBase())
     fprintf(stderr, "InitCodeBase true\n");
  /* HIST_InitCodeBase(); */
  if(!HIST_OpenAllDBs(myargs[0].intvalue))
     fprintf(stderr, "HIST_OpenAllDBs false\n");   /* set the boolean to false if want to use GetAll */
  if(!Extract_ACCDB())
     fprintf(stderr, "Extract_ACCDB false\n");
  else 
     fprintf(stderr, "Done\n");

  /* example of how to use the GetAll function,
     the boolean parameter to HIST_OpenAllDBs should be FALSE
     when using GetAll
  */
  /* NCBI_DatePtr from = DateNew();
     NCBI_DatePtr to = DateNew();
     DateWrite(to, 2002, 12, 12, NULL);
     DateWrite(from, 2001, 12, 12, NULL);
     hist = GetAll(ACTION_ADDED, from, to);
     PrintStHist(hist);
     StHistdbFree(hist);
     DateFree(from); DateFree(to);
  */
  CloseACCDB();
  CloseCodeBaseAndHISTDBs();
  return 0;
}

/*
$Log: histparser.c,v $
Revision 1.3  2004/05/14 20:34:15  zwang
Print 'Done' at the end of log.

Revision 1.2  2003/11/27 20:37:09  idonalds
correct implicit function declaration error

Revision 1.1.1.4  2002/08/20 08:29:59  root
Automated import from Sourceforge.net

Revision 1.8  2002/08/19 17:56:18  kaca
changed main into NCBI Main

Revision 1.5  2002/08/16 18:30:43  haocl
corrected error in comment

Revision 1.4  2002/08/16 17:03:41  haocl
added some comments and proper memory freeing

Revision 1.3  2002/08/15 19:44:46  kaca
removed double slash comment

Revision 1.2  2002/08/08 22:32:49  kaca
changed include hist_cb.h to hist_db.h

Revision 1.1  2002/08/08 19:01:39  haocl
initial import of hist

*/

