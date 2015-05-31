/* $Id: y_engine.c,v 1.2 2004/01/06 22:32:18 skuznets Exp $
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
  Parser for the GO yeast annotation, adds the GO yeast annotation
  to the existing seqhound LL_GO database.

  this module requires that you first build the SGD database which
  maps yeast ORF to gi/accession; this parser will extract a
  ORF from orf_geneontology, look up the gi in SGD and then 
  appends the gi, pmid, goid and evidence code into LL_GO

  Also requires that LL_GO exists.
  
  to download orf_geneontology.all.tab, go to 
    ftp://genome-ftp.stanford.edu/pub/yeast/data_download/literature_curation/  
  
  to run parser:
    ./yst -i /path/to/orf_geneontology.all.tab

  logs everything to yparserlog, look in here for errors
    
****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif
#include <ncbi.h>
#include <y_parser.h>
#include <slri_misc.h>

#define LOGFILE "yparserlog"
#define NUMARGS 1
	
Args myargs[NUMARGS] = {
   {"Input file", "orf_geneontology.all.tab", NULL, NULL, TRUE, 'i', ARG_FILE_IN, 0.0, 0, NULL}
};

Int2 Main(void){

  ErrSetOptFlags(EO_LOG_SEVERITY | EO_LOG_CODES | EO_LOG_FILELINE | EO_LOG_USRFILE);
  ErrSetLogfile(LOGFILE, ELOG_BANNER);
  ErrSetLogLevel(SEV_INFO);
  ErrSetMessageLevel(SEV_MAX);

  if(! GetArgs("yst", NUMARGS, myargs)) return 0;
  if(FileLength(myargs[0].strvalue) == 0)
  {
    ErrPostEx(SEV_ERROR, 0, 0, "Main: Cannot find %s file.", IsNullStr(myargs[0].strvalue));
    return -1;
  }
  if(Y_ParseFile(myargs[0].strvalue) == -1){
    ErrPostEx(SEV_ERROR, 0, 0, "[YEAST_ANN]: Parse Error");
    return -1;
  }
  ErrPostEx(SEV_INFO, 0, 0, "Main: Done!");
  return 0;
}
	
#ifdef __cplusplus
}
#endif
	
/*
$Log: y_engine.c,v $
Revision 1.2  2004/01/06 22:32:18  skuznets
Code audit has been done sucessfully.
Possible NULL pointer issues has been eliminated.

Revision 1.1.1.2  2003/06/28 08:45:45  root
Automated import from Sourceforge.net

Revision 1.2  2003/06/26 19:22:07  haocl
cleaned up code, added comments and execution instructions


*/
