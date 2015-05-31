/*$Id: rpsdb_odbc.h,v 1.4 2004/12/14 19:49:05 zwang Exp $*/
/*
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



  REVISION/CONTRIBUTION NOTES:

  March 1, 2001  Original MSH Revision.
                Hogue Lab - University of Toronto Biochemistry
                Department and the Samuel Lunenfeld Research
                Institute, Mount Sinai Hospital
                http://bioinfo.mshri.on.ca  hogue@mshri.on.ca
                ftp://ftp.mshri.on.ca/


  PUBLICATION TO CITE:



***************************************************************************
*/

/**
** @file 	rpsdb_odbc.h
**
** @brief 	Rps database management (on ODBC layer)
**		Part of SeqHound
** @authors	Doron Betel          betel@mshri.on.ca
** 		Christopher W.V. Hogue hogue@mshri.on.ca
** 		Minzhi Lu
** @version 	2.0
** @date 	July 22, 2003
**
*/



#ifndef __RPSDB_ODBC_H__
#define __RPSDB_ODBC_H__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <ncbi.h>

#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>

#include <asn.h>
#include <mmdbapi.h>
#include <objslristruc.h>
#include <objcdd.h>

#include <slri_odbc.h>

#ifdef __cplusplus
extern "C" { /* } */
#endif

#define RPS_SUCCESS 0

  /**************************************************************/
  /*		INIT FUNCTIONS					*/
  /**************************************************************/

  extern Boolean RPS_InitCodeBase(void);
  extern Boolean RPS_CloseCodeBase(void);

  /** 
  ** forms a connection to the database and set up handles for the
  ** rps inserts.
  **
  ** @returns TRUE if connection is successful, FALSE otherwise
  */
	extern Boolean RPS_ConnectToDB(void);

  /**
  ** closes the databases and free the environment handles. 
  **
  **  This function must called at the
  **   end of your function, if you called RPS_ConnectToDB.
  **  
  ** @returns TRUE if disconnection is successful, FALSE otherwise
  **
  */
	extern Boolean RPS_DisconnectDB(void);

  /**
  ** starts transaction by setting AutoCommit off. 
  **
  **  This function must called at the
  **   beginning of your function if you want to have a transaction.
  **  
  ** @returns TRUE if setting AutoCommit off is successful, FALSE otherwise
  **
  */
	extern Boolean RPS_StartTransaction(void);

  /**
  ** commits changes. 
  **
  **  This function must called at the
  **   end of your function if you want to commit changes.
  **  
  ** @returns TRUE if commit is successful, FALSE otherwise
  **
  */
	extern Boolean RPS_CommitTransaction(void);

  /**
  **  rolls back changes. 
  **
  **  This function must called at the
  **   end of your function if you want to roll back changes.
  **  
  ** @returns TRUE if rollback is successful, FALSE otherwise
  **
  */
	extern Boolean RPS_RollBackTransaction(void);


  /**************************************************************/
  /*		APPENDING FUNCTIONS				*/
  /**************************************************************/
  /**
  ** Append record to table RPSDB
  ** @return 	RPS_APPEND_SUCCESS if append successfully
  ** 		-1 if primary key already exists
  **		-2 otherwise	 
  **/
	extern Boolean AppendRecordRPSDB(StRpsdbPtr prs);
  /**
  ** Append record to table DOMNAME
  ** @return 	RPS_APPEND_SUCCESS if append successfully
  ** 		-1 if primary key already exists
  **		-2 otherwise	 
  **/
	extern Boolean AppendRecordDOMNAME(StDomNamedbPtr dnp);

  /**************************************************************/
  /*		DELETING FUNCTIONS				*/
  /**************************************************************/
	extern Boolean DeleteGIinRPSDB(SQLINTEGER gi, Boolean destroy);
	extern Boolean DeleteAccinDOMNAME(CharPtr, Boolean destroy);
	extern Boolean DeleteAllRecordsInDOMNAME();

  /**************************************************************/
  /*		SEARCHING FUNCTIONS				*/
  /* Precondition - the search key				*/
  /* is present in the parameter struct				*/
  /* and evalue is set to some value				*/
  /**************************************************************/

  /** 
  ** Is exist calls for RPSDB
  ** @param	gi	primary key of table RPSDB
  ** @return 	TRUE if the data exists in database
  **		FALSE otherwise
  */
	extern Boolean IsExistInRPSDB(Int4 gi);
	
  /** 
  ** Is exist calls for DOMNAME 
  ** @param	accession	primary key of table DOMNAME 
  ** @return 	TRUE if the data exists in database
  **		FALSE otherwise
  */
	extern Boolean IsExistInDOMNAME(CharPtr accession);

  /**
  **  Search using a tag and Evalue cutoff in RPSDB table 
  ** @param	pprs 
  ** @return 	the result is passed in parameter, if cann't find the record, pprs is NULL
  **		and return -1; if find the data, return 0
  */
	extern Int2 SearchRPSDB(StRpsdbPtr PNTR pprs);
	
  /**
  **  Search using a tag and Evalue cutoff in DOMNAME table
  ** @param	ppdm
  ** @param	noasn 	TRUE if want to search for the ASN1 data
  **			FALSE if do NOT want to search the ASN1 data 
  ** @return 	the result is passed in parameter, if cann't find the record, pprs is NULL
  **		and return -1; if find the data, return 0
  */
	extern Int2 SearchDOMNAME(StDomNamedbPtr PNTR ppdm, Boolean noasn);

  /*Search RPSDB using a combination of Tags and Evalue 
  ** note: not implemented since this function is currently not in use
  */
	extern Int2 SearchRPSDBByTagsAndEvalue(StRpsdbPtr PNTR pprs);

  /*****************CODEBASE FUNCTIONS***************************/
  /*		not implemented in ODBC				*/
  /**************************************************************/

  /*Open tables*/
	Boolean OpenRPSDB(CharPtr name, Boolean Create);
	Boolean OpenDOMNAME(CharPtr name, Boolean Create);

  /*Print tables*/
	Boolean PrintRPSDB(void);
	Boolean PrintDOMNAME(void);


  /*packing and compression*/
	void PackRPSDB(void);
	void PackDOMNAME(void);

  /*Close tables*/
	Boolean CloseRPSDB(void);
	Boolean CloseDOMNAME(void);

  /*Reset Calls*/
	void ResetStRPSDB (StRpsdbPtr PNTR pprs);
	void ResetStDOMNAME (StDomNamedbPtr PNTR ppdn);



  /*******************
  * Utility Functions
  *******************/
	CddPtr GetCddFROMRDOMNAME(void);

	static Boolean InitBindParameterforInsert();
       
        /* search the table, used for other search functions */
        static ValNodePtr getRecord(CharPtr, CharPtr, CharPtr, SQLINTEGER, CharPtr);

  void LIBCALL PackRPSDB(void);

#ifdef __cplusplus
/* { */ }
#endif


#endif

/*
$Log: rpsdb_odbc.h,v $
Revision 1.4  2004/12/14 19:49:05  zwang
Added transaction related functions.

Revision 1.3  2004/01/22 16:00:54  zwang
Remove SHoundIsInited since it is intrez.h/c

Revision 1.2  2004/01/12 22:15:46  zwang
Changed return type to be the same as CodeBase layer. Added more functions.

Revision 1.1  2003/09/30 14:41:14  haocl
adding additional odbc layer

Revision 1.3  2002/10/30 17:47:07  betel
Added IsExist* functions

Revision 1.2  2002/10/13 22:54:03  betel
Added table packing calls

Revision 1.1  2002/09/30 17:33:48  kaca
former rpsdb_cb.h

Revision 1.5  2002/08/22 18:38:00  betel
Changed SearchDOMANE to search without returning asn1

Revision 1.4  2002/07/29 22:30:46  betel
Added GetDomainsFromGiListWithEvalue for local&remote C api (no Perl or C++ yet)

Revision 1.3  2002/05/17 15:06:10  betel
Fixed Windows MSVC build

Revision 1.2  2002/04/11 20:27:58  betel
All search most include e-value. Searching without codebase relations

Revision 1.1  2002/04/02 21:41:54  kaca
moved from ../rps

Revision 1.4  2002/03/18 21:58:38  betel
Added Domain Table CodeBase functions

Revision 1.3  2002/03/15 22:13:15  betel
Added query function based on two tags and evalue

Revision 1.2  2002/03/15 00:41:12  betel
Added more query functions-not fully tested

Revision 1.1  2002/03/12 21:12:17  betel
Moved all rpsdb relevant files to rps dir

Revision 1.2  2002/03/11 22:59:22  betel
Small changes

Revision 1.1  2002/02/27 18:14:11  betel
Codebase layer for RPS results table

*/

