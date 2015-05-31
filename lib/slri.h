/* $Id: slri.h,v 1.1.1.5 2002/09/06 08:29:48 root Exp $

-- ***************************************************************************
--
--  Samuel Lunenfeld Research Institute (SLRI)
--
--  File: slri.h
--  Description: All Inclusive Header (except for db specific)
--
--  Author: Gary Bader
--  Date: October 1998
--
--  Hogue Lab - University of Toronto Biochemistry Department
--  Samuel Lunenfeld Research Institute, Mount Sinai Hospital
--
--  Copyright Notice:
--
--  Copyright ©2000 Mount Sinai Hospital (MSH)
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

#ifndef _SLRI_H_
#define _SLRI_H_

/*************************************************/
/* This is EXCLUSIVELY an EXECUTABLE include     */
/*************************************************/
/* DO NOT include this header for library source */
/* instead include only required headers         */
/* to support the .c/.h source code              */
/*************************************************/
#include <slri_asn.h>
#include <slri_cookie.h>
#include <slri_cluster.h>
#include <slri_endec.h>
#include <slri_mail.h>
#include <slri_math.h>
#include <slri_misc.h>
#include <slri_prime.h>
#include <slri_rbtree.h>
#include <slri_sstack.h>
#include <slri_stat.h>
#include <slri_www.h>
#include <objslrilibstruc.h>
#include <objslrilinkset.h>


#endif  /* _SLRI_H_ */

/*
  $Log: slri.h,v $
  Revision 1.1.1.5  2002/09/06 08:29:48  root
  Automated import from Sourceforge.net

  Revision 1.5  2002/09/05 18:38:06  micheld
  added comment for slri.h use in executables, not libraries

  Revision 1.4  2002/07/18 23:22:31  micheld
  Added Principal Components Analysis and supporting functions

  Revision 1.3  2002/03/25 06:44:22  micheld
  Added comprehensive suite of linksets (only clinkset, flinkset and ilinkset tested)

  Revision 1.2  2002/03/07 19:33:08  micheld
  Added Howard's web encoding to library

  Revision 1.1  2002/02/11 19:24:09  micheld
  Moved asn out of seqhound and into library (this file should generate all future slrilib asn structs)
  Added asn auto-generator script files
  Added slri.h to be all-inclusive include file (does not include database specific files)

*/

