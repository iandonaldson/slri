/* $Id: slrilinkset.h,v 1.5 2004/02/19 23:47:26 mjdumont Exp $
-- ***************************************************************************
--
--  Samuel Lunenfeld Research Institute (SLRI)
--
--  File: objslrilinkset.h
--  Description:  Modified LinkSet from NCBI's LinkSet that uses different weights
--                and can be used in a nested list
--                **** Do not use ASNTOOL to regenerate ****
--                Hard-coded LinkSet component
--
--  Author: Michel Dumontier
--  Date: April 2001 (updated Feb 2002)
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

static char * asnfilename = "slrilinkset.h";
static AsnType atx[] = {
  {401, "YLink-set" ,1,0,0,0,0,1,0,0,NULL,&atx[9],&atx[1],0,&atx[10]} ,
  {0, "num" ,128,0,0,0,0,0,0,0,NULL,&atx[2],NULL,0,&atx[3]} ,
  {302, "INTEGER" ,0,2,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "uids" ,128,1,0,1,0,0,0,0,NULL,&atx[5],&atx[4],0,&atx[6]} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[2],NULL,0,NULL} ,
  {312, "SEQUENCE OF" ,0,16,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "weights" ,128,2,0,1,0,0,0,0,NULL,&atx[5],&atx[7],0,NULL} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[8],NULL,0,NULL} ,
  {323, "VisibleString" ,0,26,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {311, "SEQUENCE" ,0,16,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {402, "YLink-set-List" ,1,0,0,0,0,1,0,0,NULL,&atx[9],&atx[11],0,&atx[14]} ,
  {0, "set1" ,128,0,0,0,0,0,0,0,NULL,&atx[5],&atx[12],0,&atx[13]} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[0],NULL,0,NULL} ,
  {0, "id" ,128,1,0,0,0,0,0,0,NULL,&atx[2],NULL,0,NULL} ,
  {403, "YLink-set-List2" ,1,0,0,0,0,1,0,0,NULL,&atx[9],&atx[15],0,&atx[18]} ,
  {0, "set2" ,128,0,0,0,0,0,0,0,NULL,&atx[5],&atx[16],0,&atx[17]} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[10],NULL,0,NULL} ,
  {0, "id" ,128,1,0,0,0,0,0,0,NULL,&atx[2],NULL,0,NULL} ,
  {404, "YLink-set-List3" ,1,0,0,0,0,1,0,0,NULL,&atx[9],&atx[19],0,&atx[22]} ,
  {0, "set3" ,128,0,0,0,0,0,0,0,NULL,&atx[5],&atx[20],0,&atx[21]} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[14],NULL,0,NULL} ,
  {0, "id" ,128,1,0,0,0,0,0,0,NULL,&atx[2],NULL,0,NULL} ,

  {405, "LLink-set" ,1,0,0,0,0,1,0,0,NULL,&atx[9],&atx[23],0,&atx[28]} ,
  {0, "num" ,128,0,0,0,0,0,0,0,NULL,&atx[2],NULL,0,&atx[24]} ,
  {0, "uids" ,128,1,0,1,0,0,0,0,NULL,&atx[5],&atx[25],0,&atx[26]} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[2],NULL,0,NULL} ,
  {0, "weights" ,128,2,0,1,0,0,0,0,NULL,&atx[5],&atx[27],0,NULL} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[8],NULL,0,NULL} ,
  {406, "LLink-set-List" ,1,0,0,0,0,1,0,0,NULL,&atx[9],&atx[29],0,&atx[32]} ,
  {0, "set1" ,128,0,0,0,0,0,0,0,NULL,&atx[5],&atx[30],0,&atx[31]} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[22],NULL,0,NULL} ,
  {0, "id" ,128,1,0,0,0,0,0,0,NULL,&atx[2],NULL,0,NULL} ,
  {407, "LLink-set-List2" ,1,0,0,0,0,1,0,0,NULL,&atx[9],&atx[33],0,&atx[36]} ,
  {0, "set2" ,128,0,0,0,0,0,0,0,NULL,&atx[5],&atx[34],0,&atx[35]} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[28],NULL,0,NULL} ,
  {0, "id" ,128,1,0,0,0,0,0,0,NULL,&atx[2],NULL,0,NULL} ,
  {408, "LLink-set-List3" ,1,0,0,0,0,1,0,0,NULL,&atx[9],&atx[37],0,&atx[40]} ,
  {0, "set3" ,128,0,0,0,0,0,0,0,NULL,&atx[5],&atx[38],0,&atx[39]} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[32],NULL,0,NULL} ,
  {0, "id" ,128,1,0,0,0,0,0,0,NULL,&atx[2],NULL,0,NULL} ,

  {409, "BLink-set" ,1,0,0,0,0,1,0,0,NULL,&atx[9],&atx[41],0,&atx[46]} ,
  {0, "num" ,128,0,0,0,0,0,0,0,NULL,&atx[2],NULL,0,&atx[42]} ,
  {0, "uids" ,128,1,0,1,0,0,0,0,NULL,&atx[5],&atx[43],0,&atx[44]} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[2],NULL,0,NULL} ,
  {0, "weights" ,128,2,0,1,0,0,0,0,NULL,&atx[5],&atx[45],0,NULL} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[8],NULL,0,NULL} ,
  {410, "BLink-set-List" ,1,0,0,0,0,1,0,0,NULL,&atx[9],&atx[47],0,&atx[50]} ,
  {0, "set1" ,128,0,0,0,0,0,0,0,NULL,&atx[5],&atx[48],0,&atx[49]} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[40],NULL,0,NULL} ,
  {0, "id" ,128,1,0,0,0,0,0,0,NULL,&atx[2],NULL,0,NULL} ,
  {411, "BLink-set-List2" ,1,0,0,0,0,1,0,0,NULL,&atx[9],&atx[51],0,&atx[54]} ,
  {0, "set2" ,128,0,0,0,0,0,0,0,NULL,&atx[5],&atx[52],0,&atx[53]} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[46],NULL,0,NULL} ,
  {0, "id" ,128,1,0,0,0,0,0,0,NULL,&atx[2],NULL,0,NULL} ,
  {412, "BLink-set-List3" ,1,0,0,0,0,1,0,0,NULL,&atx[9],&atx[55],0,&atx[58]} ,
  {0, "set3" ,128,0,0,0,0,0,0,0,NULL,&atx[5],&atx[56],0,&atx[57]} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[50],NULL,0,NULL} ,
  {0, "id" ,128,1,0,0,0,0,0,0,NULL,&atx[2],NULL,0,NULL} ,

  {413, "CLink-set" ,1,0,0,0,0,1,0,0,NULL,&atx[9],&atx[59],0,&atx[64]} ,
  {0, "num" ,128,0,0,0,0,0,0,0,NULL,&atx[2],NULL,0,&atx[60]} ,
  {0, "uids" ,128,1,0,1,0,0,0,0,NULL,&atx[5],&atx[61],0,&atx[62]} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[2],NULL,0,NULL} ,
  {0, "weights" ,128,2,0,1,0,0,0,0,NULL,&atx[5],&atx[63],0,NULL} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[8],NULL,0,NULL} ,
  {414, "CLink-set-List" ,1,0,0,0,0,1,0,0,NULL,&atx[9],&atx[65],0,&atx[68]} ,
  {0, "set1" ,128,0,0,0,0,0,0,0,NULL,&atx[5],&atx[66],0,&atx[67]} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[58],NULL,0,NULL} ,
  {0, "id" ,128,1,0,0,0,0,0,0,NULL,&atx[2],NULL,0,NULL} ,
  {415, "CLink-set-List2" ,1,0,0,0,0,1,0,0,NULL,&atx[9],&atx[69],0,&atx[72]} ,
  {0, "set2" ,128,0,0,0,0,0,0,0,NULL,&atx[5],&atx[70],0,&atx[71]} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[64],NULL,0,NULL} ,
  {0, "id" ,128,1,0,0,0,0,0,0,NULL,&atx[2],NULL,0,NULL} ,
  {416, "CLink-set-List3" ,1,0,0,0,0,1,0,0,NULL,&atx[9],&atx[73],0,&atx[76]} ,
  {0, "set3" ,128,0,0,0,0,0,0,0,NULL,&atx[5],&atx[74],0,&atx[75]} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[68],NULL,0,NULL} ,
  {0, "id" ,128,1,0,0,0,0,0,0,NULL,&atx[2],NULL,0,NULL} ,

  {417, "FLink-set" ,1,0,0,0,0,1,0,0,NULL,&atx[9],&atx[77],0,&atx[83]} ,
  {0, "num" ,128,0,0,0,0,0,0,0,NULL,&atx[2],NULL,0,&atx[78]} ,
  {0, "uids" ,128,1,0,1,0,0,0,0,NULL,&atx[5],&atx[79],0,&atx[80]} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[2],NULL,0,NULL} ,
  {0, "weights" ,128,2,0,1,0,0,0,0,NULL,&atx[5],&atx[81],0,NULL} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[82],NULL,0,NULL} ,
  {309, "REAL" ,0,9,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {418, "FLink-set-List" ,1,0,0,0,0,1,0,0,NULL,&atx[9],&atx[84],0,&atx[87]} ,
  {0, "set1" ,128,0,0,0,0,0,0,0,NULL,&atx[5],&atx[85],0,&atx[86]} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[76],NULL,0,NULL} ,
  {0, "id" ,128,1,0,0,0,0,0,0,NULL,&atx[2],NULL,0,NULL} ,
  {419, "FLink-set-List2" ,1,0,0,0,0,1,0,0,NULL,&atx[9],&atx[88],0,&atx[91]} ,
  {0, "set2" ,128,0,0,0,0,0,0,0,NULL,&atx[5],&atx[89],0,&atx[90]} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[83],NULL,0,NULL} ,
  {0, "id" ,128,1,0,0,0,0,0,0,NULL,&atx[2],NULL,0,NULL} ,
  {420, "FLink-set-List3" ,1,0,0,0,0,1,0,0,NULL,&atx[9],&atx[92],0,&atx[95]} ,
  {0, "set3" ,128,0,0,0,0,0,0,0,NULL,&atx[5],&atx[93],0,&atx[94]} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[87],NULL,0,NULL} ,
  {0, "id" ,128,1,0,0,0,0,0,0,NULL,&atx[2],NULL,0,NULL} ,

  {421, "ILink-set" ,1,0,0,0,0,1,0,0,NULL,&atx[9],&atx[96],0,&atx[101]} ,
  {0, "num" ,128,0,0,0,0,0,0,0,NULL,&atx[2],NULL,0,&atx[97]} ,
  {0, "uids" ,128,1,0,1,0,0,0,0,NULL,&atx[5],&atx[98],0,&atx[99]} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[2],NULL,0,NULL} ,
  {0, "weights" ,128,2,0,1,0,0,0,0,NULL,&atx[5],&atx[100],0,NULL} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[2],NULL,0,NULL} ,
  {422, "ILink-set-List" ,1,0,0,0,0,1,0,0,NULL,&atx[9],&atx[102],0,&atx[105]} ,
  {0, "set1" ,128,0,0,0,0,0,0,0,NULL,&atx[5],&atx[103],0,&atx[104]} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[95],NULL,0,NULL} ,
  {0, "id" ,128,1,0,0,0,0,0,0,NULL,&atx[2],NULL,0,NULL} ,
  {423, "ILink-set-List2" ,1,0,0,0,0,1,0,0,NULL,&atx[9],&atx[106],0,&atx[109]} ,
  {0, "set2" ,128,0,0,0,0,0,0,0,NULL,&atx[5],&atx[107],0,&atx[108]} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[101],NULL,0,NULL} ,
  {0, "id" ,128,1,0,0,0,0,0,0,NULL,&atx[2],NULL,0,NULL} ,
  {424, "ILink-set-List3" ,1,0,0,0,0,1,0,0,NULL,&atx[9],&atx[110],0,&atx[113]} ,
  {0, "set3" ,128,0,0,0,0,0,0,0,NULL,&atx[5],&atx[111],0,&atx[112]} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[105],NULL,0,NULL} ,
  {0, "id" ,128,1,0,0,0,0,0,0,NULL,&atx[2],NULL,0,NULL} ,

  {425, "IVector" ,1,0,0,0,0,1,0,0,NULL,&atx[9],&atx[114],0,&atx[117]} ,
  {0, "i" ,128,0,0,0,0,0,0,0,NULL,&atx[2],NULL,0,&atx[115]} ,
  {0, "v" ,128,1,0,1,0,0,0,0,NULL,&atx[5],&atx[116],0,NULL} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[2],NULL,0,NULL} ,
  {426, "IVectorList" ,1,0,0,0,0,1,0,0,NULL,&atx[9],&atx[118],0,&atx[121]} ,
  {0, "id" ,128,0,0,0,0,0,0,0,NULL,&atx[2],NULL,0,&atx[119]} ,
  {0, "iV" ,128,1,0,0,0,0,0,0,NULL,&atx[5],&atx[120],0,NULL} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[113],NULL,0,NULL} ,

  {427, "IMatrix" ,1,0,0,0,0,1,0,0,NULL,&atx[9],&atx[122],0,&atx[126]} ,
  {0, "i" ,128,0,0,0,0,0,0,0,NULL,&atx[2],NULL,0,&atx[123]} ,
  {0, "j" ,128,1,0,0,0,0,0,0,NULL,&atx[2],NULL,0,&atx[124]} ,
  {0, "m" ,128,2,0,1,0,0,0,0,NULL,&atx[5],&atx[125],0,NULL} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[2],NULL,0,NULL} ,
  {428, "IMatrixList" ,1,0,0,0,0,1,0,0,NULL,&atx[9],&atx[127],0,&atx[130]} ,
  {0, "id" ,128,0,0,0,0,0,0,0,NULL,&atx[2],NULL,0,&atx[128]} ,
  {0, "iM" ,128,1,0,0,0,0,0,0,NULL,&atx[5],&atx[129],0,NULL} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[121],NULL,0,NULL} ,
  {429, "IMatrixListSet" ,1,0,0,0,0,1,0,0,NULL,&atx[9],&atx[131],0,&atx[134]} ,
  {0, "id" ,128,0,0,0,0,0,0,0,NULL,&atx[2],NULL,0,&atx[132]} ,
  {0, "iM" ,128,1,0,0,0,0,0,0,NULL,&atx[5],&atx[133],0,NULL} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[126],NULL,0,NULL}  ,

  {430, "FHiVector" ,1,0,0,0,0,1,0,0,NULL,&atx[9],&atx[135],0,&atx[138]} ,
  {0, "i" ,128,0,0,0,0,0,0,0,NULL,&atx[2],NULL,0,&atx[136]} ,
  {0, "v" ,128,1,0,1,0,0,0,0,NULL,&atx[5],&atx[137],0,NULL} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[82],NULL,0,NULL} ,
  {431, "FHiVectorList" ,1,0,0,0,0,1,0,0,NULL,&atx[9],&atx[139],0,&atx[142]} ,
  {0, "id" ,128,0,0,0,0,0,0,0,NULL,&atx[2],NULL,0,&atx[140]} ,
  {0, "iV" ,128,1,0,0,0,0,0,0,NULL,&atx[5],&atx[141],0,NULL} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[134],NULL,0,NULL} ,

  {432, "FHiMatrix" ,1,0,0,0,0,1,0,0,NULL,&atx[9],&atx[143],0,&atx[147]} ,
  {0, "i" ,128,0,0,0,0,0,0,0,NULL,&atx[2],NULL,0,&atx[144]} ,
  {0, "j" ,128,1,0,0,0,0,0,0,NULL,&atx[2],NULL,0,&atx[145]} ,
  {0, "m" ,128,2,0,1,0,0,0,0,NULL,&atx[5],&atx[146],0,NULL} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[2],NULL,0,NULL} ,
  {433, "FHiMatrixList" ,1,0,0,0,0,1,0,0,NULL,&atx[9],&atx[148],0,&atx[151]} ,
  {0, "id" ,128,0,0,0,0,0,0,0,NULL,&atx[2],NULL,0,&atx[149]} ,
  {0, "iM" ,128,1,0,0,0,0,0,0,NULL,&atx[5],&atx[150],0,NULL} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[142],NULL,0,NULL} ,
  {434, "FHiMatrixListSet" ,1,0,0,0,0,1,0,0,NULL,&atx[9],&atx[152],0,&atx[155]} ,
  {0, "id" ,128,0,0,0,0,0,0,0,NULL,&atx[2],NULL,0,&atx[153]} ,
  {0, "iM" ,128,1,0,0,0,0,0,0,NULL,&atx[5],&atx[154],0,NULL} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[147],NULL,0,NULL}  ,

  {435, "FLoVector" ,1,0,0,0,0,1,0,0,NULL,&atx[9],&atx[156],0,&atx[159]} ,
  {0, "i" ,128,0,0,0,0,0,0,0,NULL,&atx[2],NULL,0,&atx[157]} ,
  {0, "v" ,128,1,0,1,0,0,0,0,NULL,&atx[5],&atx[158],0,NULL} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[82],NULL,0,NULL} ,
  {436, "FLoVectorList" ,1,0,0,0,0,1,0,0,NULL,&atx[9],&atx[160],0,&atx[163]} ,
  {0, "id" ,128,0,0,0,0,0,0,0,NULL,&atx[2],NULL,0,&atx[161]} ,
  {0, "iV" ,128,1,0,0,0,0,0,0,NULL,&atx[5],&atx[162],0,NULL} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[155],NULL,0,NULL} ,

  {437, "CVector" ,1,0,0,0,0,1,0,0,NULL,&atx[9],&atx[164],0,&atx[167]} ,
  {0, "i" ,128,0,0,0,0,0,0,0,NULL,&atx[2],NULL,0,&atx[165]} ,
  {0, "v" ,128,1,0,1,0,0,0,0,NULL,&atx[5],&atx[166],0,NULL} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[2],NULL,0,NULL} ,
  {438, "CVectorList" ,1,0,0,0,0,1,0,0,NULL,&atx[9],&atx[168],0,NULL} ,
  {0, "id" ,128,0,0,0,0,0,0,0,NULL,&atx[2],NULL,0,&atx[169]} ,
  {0, "iV" ,128,1,0,0,0,0,0,0,NULL,&atx[5],&atx[170],0,NULL} ,
  {0, NULL,1,-1,0,0,0,0,0,0,NULL,&atx[163],NULL,0,NULL} ,
};

static AsnModule ampx[1] = {
  { "SLRI-Link-set" , "slrilinkset.h",&atx[0],NULL,NULL,0,0} };

static AsnValxNodePtr avn = NULL;
static AsnTypePtr at = atx;
static AsnModulePtr amp = ampx;



/**************************************************
*
*    Defines for Module SLRI-Link-set
*
**************************************************/

#define YLINK_SET &at[0]
#define YLINK_SET_num &at[1]
#define YLINK_SET_uids &at[3]
#define YLINK_SET_uids_E &at[4]
#define YLINK_SET_weights &at[6]
#define YLINK_SET_weights_E &at[7]

#define YLINK_SET_LIST &at[10]
#define YLINK_SET_LIST_set1 &at[11]
#define YLINK_SET_LIST_set1_E &at[12]
#define YLINK_SET_LIST_id &at[13]

#define YLINK_SET_LIST2 &at[14]
#define YLINK_SET_LIST2_set2 &at[15]
#define YLINK_SET_LIST2_set2_E &at[16]
#define YLINK_SET_LIST2_id &at[17]

#define YLINK_SET_LIST3 &at[18]
#define YLINK_SET_LIST3_set3 &at[19]
#define YLINK_SET_LIST3_set3_E &at[20]
#define YLINK_SET_LIST3_id &at[21]

#define LLINK_SET &at[22]
#define LLINK_SET_num &at[23]
#define LLINK_SET_uids &at[24]
#define LLINK_SET_uids_E &at[25]
#define LLINK_SET_weights &at[26]
#define LLINK_SET_weights_E &at[27]

#define LLINK_SET_LIST &at[28]
#define LLINK_SET_LIST_set1 &at[29]
#define LLINK_SET_LIST_set1_E &at[30]
#define LLINK_SET_LIST_id &at[31]

#define LLINK_SET_LIST2 &at[32]
#define LLINK_SET_LIST2_set2 &at[33]
#define LLINK_SET_LIST2_set2_E &at[34]
#define LLINK_SET_LIST2_id &at[35]

#define LLINK_SET_LIST3 &at[36]
#define LLINK_SET_LIST3_set3 &at[37]
#define LLINK_SET_LIST3_set3_E &at[38]
#define LLINK_SET_LIST3_id &at[39]

#define BLINK_SET &at[40]
#define BLINK_SET_num &at[41]
#define BLINK_SET_uids &at[42]
#define BLINK_SET_uids_E &at[43]
#define BLINK_SET_weights &at[44]
#define BLINK_SET_weights_E &at[45]

#define BLINK_SET_LIST &at[46]
#define BLINK_SET_LIST_set1 &at[47]
#define BLINK_SET_LIST_set1_E &at[48]
#define BLINK_SET_LIST_id &at[49]

#define BLINK_SET_LIST2 &at[50]
#define BLINK_SET_LIST2_set2 &at[51]
#define BLINK_SET_LIST2_set2_E &at[52]
#define BLINK_SET_LIST2_id &at[53]

#define BLINK_SET_LIST3 &at[54]
#define BLINK_SET_LIST3_set3 &at[55]
#define BLINK_SET_LIST3_set3_E &at[56]
#define BLINK_SET_LIST3_id &at[57]

#define CLINK_SET &at[58]
#define CLINK_SET_num &at[59]
#define CLINK_SET_uids &at[60]
#define CLINK_SET_uids_E &at[61]
#define CLINK_SET_weights &at[62]
#define CLINK_SET_weights_E &at[63]

#define CLINK_SET_LIST &at[64]
#define CLINK_SET_LIST_set1 &at[65]
#define CLINK_SET_LIST_set1_E &at[66]
#define CLINK_SET_LIST_id &at[67]

#define CLINK_SET_LIST2 &at[68]
#define CLINK_SET_LIST2_set2 &at[69]
#define CLINK_SET_LIST2_set2_E &at[70]
#define CLINK_SET_LIST2_id &at[71]

#define CLINK_SET_LIST3 &at[72]
#define CLINK_SET_LIST3_set3 &at[73]
#define CLINK_SET_LIST3_set3_E &at[74]
#define CLINK_SET_LIST3_id &at[75]

#define FLINK_SET &at[76]
#define FLINK_SET_num &at[77]
#define FLINK_SET_uids &at[78]
#define FLINK_SET_uids_E &at[79]
#define FLINK_SET_weights &at[80]
#define FLINK_SET_weights_E &at[81]

#define FLINK_SET_LIST &at[83]
#define FLINK_SET_LIST_set1 &at[84]
#define FLINK_SET_LIST_set1_E &at[85]
#define FLINK_SET_LIST_id &at[86]

#define FLINK_SET_LIST2 &at[87]
#define FLINK_SET_LIST2_set2 &at[88]
#define FLINK_SET_LIST2_set2_E &at[89]
#define FLINK_SET_LIST2_id &at[90]

#define FLINK_SET_LIST3 &at[91]
#define FLINK_SET_LIST3_set3 &at[92]
#define FLINK_SET_LIST3_set3_E &at[93]
#define FLINK_SET_LIST3_id &at[94]

#define ILINK_SET &at[95]
#define ILINK_SET_num &at[96]
#define ILINK_SET_uids &at[97]
#define ILINK_SET_uids_E &at[98]
#define ILINK_SET_weights &at[99]
#define ILINK_SET_weights_E &at[100]

#define ILINK_SET_LIST &at[101]
#define ILINK_SET_LIST_set1 &at[102]
#define ILINK_SET_LIST_set1_E &at[103]
#define ILINK_SET_LIST_id &at[104]

#define ILINK_SET_LIST2 &at[105]
#define ILINK_SET_LIST2_set2 &at[106]
#define ILINK_SET_LIST2_set2_E &at[107]
#define ILINK_SET_LIST2_id &at[108]

#define ILINK_SET_LIST3 &at[109]
#define ILINK_SET_LIST3_set3 &at[110]
#define ILINK_SET_LIST3_set3_E &at[111]
#define ILINK_SET_LIST3_id &at[112]

#define IVECTOR &at[113]
#define IVECTOR_i &at[114]
#define IVECTOR_v &at[115]
#define IVECTOR_v_E &at[116]

#define IVECTORLIST &at[117]
#define IVECTORLIST_id &at[118]
#define IVECTORLIST_iV &at[119]
#define IVECTORLIST_iV_E &at[120]

#define IMATRIX &at[121]
#define IMATRIX_i &at[122]
#define IMATRIX_j &at[123]
#define IMATRIX_m &at[124]
#define IMATRIX_m_E &at[125]

#define IMATRIXLIST &at[126]
#define IMATRIXLIST_id &at[127]
#define IMATRIXLIST_iM &at[128]
#define IMATRIXLIST_iM_E &at[129]

#define IMATRIXLISTSET &at[130]
#define IMATRIXLISTSET_id &at[131]
#define IMATRIXLISTSET_iML &at[132]
#define IMATRIXLISTSET_iML_E &at[133]

#define FHIVECTOR &at[134]
#define FHIVECTOR_i &at[135]
#define FHIVECTOR_v &at[136]
#define FHIVECTOR_v_E &at[137]

#define FHIVECTORLIST &at[138]
#define FHIVECTORLIST_id &at[139]
#define FHIVECTORLIST_iV &at[140]
#define FHIVECTORLIST_iV_E &at[141]

#define FHIMATRIX &at[142]
#define FHIMATRIX_i &at[143]
#define FHIMATRIX_j &at[144]
#define FHIMATRIX_m &at[145]
#define FHIMATRIX_m_E &at[146]

#define FHIMATRIXLIST &at[147]
#define FHIMATRIXLIST_id &at[148]
#define FHIMATRIXLIST_iM &at[149]
#define FHIMATRIXLIST_iM_E &at[150]

#define FHIMATRIXLISTSET &at[151]
#define FHIMATRIXLISTSET_id &at[152]
#define FHIMATRIXLISTSET_iML &at[153]
#define FHIMATRIXLISTSET_iML_E &at[154]

#define FLOVECTOR &at[155]
#define FLOVECTOR_i &at[156]
#define FLOVECTOR_v &at[157]
#define FLOVECTOR_v_E &at[158]

#define FLOVECTORLIST &at[159]
#define FLOVECTORLIST_id &at[160]
#define FLOVECTORLIST_iV &at[161]
#define FLOVECTORLIST_iV_E &at[162]

#define CVECTOR &at[163]
#define CVECTOR_i &at[164]
#define CVECTOR_v &at[165]
#define CVECTOR_v_E &at[166]

#define CVECTORLIST &at[167]
#define CVECTORLIST_id &at[168]
#define CVECTORLIST_iV &at[169]
#define CVECTORLIST_iV_E &at[170]

/*
 * $Log: slrilinkset.h,v $
 * Revision 1.5  2004/02/19 23:47:26  mjdumont
 * Added FHiMatrix and lists
 *
 * Revision 1.4  2004/02/02 20:54:28  mjdumont
 * added CVector
 *
 * Revision 1.3  2003/11/05 18:41:07  michel
 * Changed FVector to FHiVector to avoid conflict with OS X's Carbon
 *
 * Revision 1.2  2003/10/20 18:38:51  michel
 * Added a FloatLo Vector ASN.1 type
 *
 * Revision 1.1.1.5  2003/08/21 08:48:06  root
 * Automated import from Sourceforge.net
 *
 * Revision 1.6  2003/08/20 14:41:30  micheld
 * Added FVector - FloatHi Vector.
 *
 */
