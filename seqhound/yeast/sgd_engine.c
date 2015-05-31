/* $Id: sgd_engine.c,v 1.2 2004/01/06 22:32:18 skuznets Exp $
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
  Parser for SGD_CDS_xref.txt found at 
  ftp://genome-ftp.stanford.edu/pub/yeast/data_download/chromosomal_feature/
  
  format of file:
    Accession Number
    Protein_ID of CDS
    SGDID
    S. cerevisiae ORF name
  where each field is space separated.

  to run the program:
    ./sgd -i /path/to/ChrXX.faa -n [T|F] 
      set to T if you want to create the SGD database, otherwise to
      append to and existing SGD database, use F.
  
  error and logs are recorded into sgdparserlog

****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif
#include <ncbi.h>
#include <sgd_parser.h>

#ifndef IsNullStr
#define IsNullStr(x)    (((x)==NULL)?"":(x))
#endif
	
/* the log file for the sgd parser, check here for
 * error logs when you come across any error, and 
 * if the parser completes error free, the last line
 * in this file will say 'Done!'*/
#define LOGFILE "sgdparserlog"

/* the number of arguments and the argument array
 * used by the ncbi software development toolkit */	
#define NUMARGS 2
Args myargs[NUMARGS] = {
   {"Input file", NULL, NULL, NULL, FALSE, 'i', ARG_FILE_IN, 0.0, 0, NULL},
   {"Initialize SGD Database", "F", NULL, NULL, TRUE, 'n', ARG_BOOLEAN, 0.0, 0, NULL}
};


Int2 Main(void)
{
  /* set up stuff, like the severity of errors that get log */
  ErrSetOptFlags(EO_LOG_SEVERITY | EO_LOG_CODES | EO_LOG_FILELINE | EO_LOG_USRFILE);
  /* specify the log file as above */
  ErrSetLogfile(LOGFILE, ELOG_BANNER);
  ErrSetLogLevel(SEV_INFO);
  ErrSetMessageLevel(SEV_MAX);

  /* retrieve the command line arguments and save into myargs array */
  if(! GetArgs("sgd", NUMARGS, myargs)) return 1;

  /* check that the input file specified by the command line
   * argument has length > 0, otherwise, exits with an error */ 
  if(FileLength(myargs[0].strvalue) == 0)
  {
    ErrPostEx(SEV_ERROR, 0, 0, "[SGD]: Cannot find %s file.", IsNullStr(myargs[0].strvalue));
    return -1;
  }
  /* begin to parse the file */
  if(SGD_ParseFile(myargs[0].strvalue, (Boolean)myargs[1].intvalue) == -1){
    ErrPostEx(SEV_ERROR, 0, 0, "[SGD]: Parse Error");
    return -1;
  }

  /* if you want to print out the entire SGD database,
   * uncomment these section of the code */
  /* SGD_PrintDB(); */

  
  /* all done */
  ErrPostEx(SEV_INFO, 0, 0, "[SGD]: Done!");
  return 0;
}

#ifdef __cplusplus
}
#endif
	
/*
$Log: sgd_engine.c,v $
Revision 1.2  2004/01/06 22:32:18  skuznets
Code audit has been done sucessfully.
Possible NULL pointer issues has been eliminated.

Revision 1.1.1.3  2003/06/28 08:45:45  root
Automated import from Sourceforge.net

Revision 1.3  2003/06/26 19:22:07  haocl
cleaned up code, added comments and execution instructions

Revision 1.2  2003/06/25 14:26:03  haocl
completed yeast ORF to GI/Accession database

Revision 1.1  2003/06/20 17:55:13  haocl
initial import of SGD database, used by Yeast GO annotation database

*/
