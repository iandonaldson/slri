/* $Id: sgd_query.c,v 1.2 2004/01/06 22:32:18 skuznets Exp $
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
  Example of how to use the db layer to query & retrieve records from the 
  SGD database.
  
****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif
#include <ncbi.h>
#include <sgd_db.h>


Int2 Main(void)
{
  /* set up stuff, like the severity of errors that get log */
  /* ErrSetOptFlags(EO_LOG_SEVERITY | EO_LOG_CODES | EO_LOG_FILELINE | EO_LOG_USRFILE); */
  /* specify the log file as above */
  /* ErrSetLogfile(LOGFILE, ELOG_BANNER);
  ErrSetLogLevel(SEV_INFO);
  ErrSetMessageLevel(SEV_MAX); */
  CharPtr key;
  SGD_ST sgd = {0, NULL, NULL};
  /* sgd.iGI = 6226527; */
  key = malloc(sizeof(Char) * ACC_SIZE);
  sgd.pORF = key;
  StringCpy(key, "BI3");
  SGD_InitDataBase();
  if(!SGD_OpenDB(FALSE))
  {
    ErrPostEx(SEV_ERROR, 0, 0, "[SGD]: Could not open databases");
    return -1;
  }
  /* SGD_PrintDataBase(); */
  SearchSGD(&sgd);
  printf("%s %d %s\n", IsNullStr(sgd.pAcc), sgd.iGI, IsNullStr(sgd.pORF));
  /* all done */
  SGD_CloseDataBase();
  ErrPostEx(SEV_INFO, 0, 0, "[SGD]: Done!");
  return 0;
}

#ifdef __cplusplus
}
#endif
	
/*
$Log: sgd_query.c,v $
Revision 1.2  2004/01/06 22:32:18  skuznets
Code audit has been done sucessfully.
Possible NULL pointer issues has been eliminated.

Revision 1.1.1.2  2003/06/28 08:45:45  root
Automated import from Sourceforge.net

Revision 1.2  2003/06/26 19:23:57  haocl
fixed // style comments

Revision 1.1  2003/06/25 14:31:36  haocl
adding sgd_query yeast GO annotation parser, renamed yeast module

*/
