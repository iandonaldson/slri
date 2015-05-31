/* $Id: flinkset.h,v 1.1.1.1 2001/10/12 08:21:54 root Exp $
-- ***************************************************************************
--
--  Samuel Lunenfeld Research Institute (SLRI)
--
--  File: flinkset.h
--  Description:   Automatic header module from ASNTOOL
--
--  Author: Michel Dumontier
--  Date: April 2001
--
--  Hogue Lab - University of Toronto Biochemistry Department
--  Samuel Lunenfeld Research Institute, Mount Sinai Hospital
--
--  Copyright Notice:
--
--  Copyright ©2001 Mount Sinai Hospital (MSH)
--
--  This program is free software; you can redistribute it and/or 
--  modify it under the terms of the GNU General Public License as 
--  published by the Free Software Foundation; either version 2 of 
--  the License, or any later version.
--
--  This program is distributed in the hope that it will be useful, 
--  but WITHOUT ANY WARRANTY; without even the implied warranty of 
--  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
--
--  See the GNU General Public License for more details.
--
--  You should have received a copy of the GNU General Public License 
--  along with this program; if not, write to the
--         Free Software Foundation, Inc., 
--         59 Temple Place, Suite 330, Boston, MA  
--         02111-1307  USA 
--  or visit http://www.gnu.org/copyleft/gpl.html
--
--  SPECIAL EXCEPTIONS
--
--  As a special exception, Mount Sinai Hospital gives permission to 
--  link this program with the following non-GPL programs or libraries, 
--  and distribute the resulting executable, without including the source 
--  code for these in the source distribution:
--
--  a) CodeBase 6.5 or greater from Sequiter Software Inc.
--
--  b) The C or C++ interface to Oracle 8.x or greater, from
--     Oracle Corporation or IBM DB2 UDB.
--
-- ***************************************************************************
*/

#ifndef _ASNTOOL_
#include <asn.h>
#endif

static char * asnfilename = "flinkset.h";
static AsnType atx[22] = {
  {401, "FLink-set" ,1,0,0,0,0,1,0,0,NULL,&atx[9],&atx[1],0,&atx[10]} ,
  {0, "num" ,128,0,0,0,0,0,0,0,NULL,&atx[2],NULL,0,&atx[3]} ,
  {302, "INTEGER" ,0,2,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "uids" ,128,1,0,1,0,0,0,0,NULL,&atx[5],&atx[4],0,&atx[6]} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[2],NULL,0,NULL} ,
  {312, "SEQUENCE OF" ,0,16,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "weights" ,128,2,0,1,0,0,0,0,NULL,&atx[5],&atx[7],0,NULL} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[8],NULL,0,NULL} ,
  {309, "REAL" ,0,9,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {311, "SEQUENCE" ,0,16,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {402, "FLink-set-List" ,1,0,0,0,0,1,0,0,NULL,&atx[9],&atx[11],0,&atx[14]} ,
  {0, "set1" ,128,0,0,0,0,0,0,0,NULL,&atx[5],&atx[12],0,&atx[13]} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[0],NULL,0,NULL} ,
  {0, "id" ,128,1,0,0,0,0,0,0,NULL,&atx[2],NULL,0,NULL} ,
  {403, "FLink-set-List2" ,1,0,0,0,0,1,0,0,NULL,&atx[9],&atx[15],0,&atx[18]} ,
  {0, "set2" ,128,0,0,0,0,0,0,0,NULL,&atx[5],&atx[16],0,&atx[17]} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[10],NULL,0,NULL} ,
  {0, "id" ,128,1,0,0,0,0,0,0,NULL,&atx[2],NULL,0,NULL} ,
  {404, "FLink-set-List3" ,1,0,0,0,0,1,0,0,NULL,&atx[9],&atx[19],0,NULL} ,
  {0, "set3" ,128,0,0,0,0,0,0,0,NULL,&atx[5],&atx[20],0,&atx[21]} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[14],NULL,0,NULL} ,
  {0, "id" ,128,1,0,0,0,0,0,0,NULL,&atx[2],NULL,0,NULL} };

static AsnModule ampx[1] = {
  { "SLRI-FLink-set" , "flinkset.h",&atx[0],NULL,NULL,0,0} };

static AsnValxNodePtr avn = NULL;
static AsnTypePtr at = atx;
static AsnModulePtr amp = ampx;



/**************************************************
*
*    Defines for Module SLRI-FLink-set
*
**************************************************/

#define FLINK_SET &at[0]
#define FLINK_SET_num &at[1]
#define FLINK_SET_uids &at[3]
#define FLINK_SET_uids_E &at[4]
#define FLINK_SET_weights &at[6]
#define FLINK_SET_weights_E &at[7]

#define FLINK_SET_LIST &at[10]
#define FLINK_SET_LIST_set1 &at[11]
#define FLINK_SET_LIST_set1_E &at[12]
#define FLINK_SET_LIST_id &at[13]

#define FLINK_SET_LIST2 &at[14]
#define FLINK_SET_LIST2_set2 &at[15]
#define FLINK_SET_LIST2_set2_E &at[16]
#define FLINK_SET_LIST2_id &at[17]

#define FLINK_SET_LIST3 &at[18]
#define FLINK_SET_LIST3_set3 &at[19]
#define FLINK_SET_LIST3_set3_E &at[20]
#define FLINK_SET_LIST3_id &at[21]

/*
$Log: flinkset.h,v $
Revision 1.1.1.1  2001/10/12 08:21:54  root
Automated import from Sourceforge.net

Revision 1.1.1.1  2001/10/10 22:11:03  kaca
initial import

Revision 1.1  2001/09/11 21:45:10  michel
Moved the ilinkset, flinkset, stack, set and prime files into slrilib

Revision 1.1  2001/04/05 18:02:16  michel
Removed SLRILinkset from slrilib, added new to slri_math.[c/h]
Made new Generic object library, slriobj, to include FLinkSet and ILinkSet

*/

