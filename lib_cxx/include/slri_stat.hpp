/*$Id: slri_stat.hpp,v 1.1.1.1 2002/08/12 08:29:51 root Exp $*/
/***************************************************************************
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


  PROGRAM: General statistics functions


  AUTHORS:	Doron Betel	betel@mshri.on.ca
            and Christopher W.V. Hogue hogue@mshri.on.ca


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
#ifndef _H_SLRI_STAT_HPP
#define _H_SLRI_STAT_HPP
#include <corelib/ncbistd.hpp>
#include <math.h>

USING_NCBI_SCOPE;

/*Compute the mean over a container
* Precondition- Elements in container must be numeric types
* (i.e. -int, Uint4, double, etc.) and "Cnt" can be list,vector, set, multiset */
template <class Cnt> double SampleMean(Cnt& container){
	typename Cnt::const_iterator itr;
	double sum=0.0;
	unsigned int n=0;
	if(container.empty() or container.size()==0){
		ERR_POST(Warning<<"[SampleMean] empty container");
		return 0.0;
	}
	for(itr =container.begin(); itr!=container.end(); itr++){
		sum +=(*itr);
		n++;
	}
	return sum/n;
}

/*Compute variance over a container type.
* Precondition- Elements in container must be numeric types
* (i.e. -int, Uint4, double, etc.) and "Cnt" can be list,vector, set, multiset
*Sample Variance s^2= (nSum(x^2) -(Sum(x))^2 )/ n(n-1)
*/
template <class Cnt> double SampleVariance(Cnt& container){
	typename Cnt::const_iterator itr;
	double sumSquare=0.0;
	double sum=0.0;
	unsigned int n=0;
	if(container.empty() or container.size()==0){
		ERR_POST(Warning<<"[SampleVariance] empty container");
		return 0.0;
	}
	for(itr=container.begin(); itr!=container.end(); itr++){
		sumSquare +=pow((*itr),2);
		sum +=(*itr);
		n++;
	}
	return ( ( (n*sumSquare)- pow(sum,2) )/(n*(n-1)) );
}

#endif
/*
$Log: slri_stat.hpp,v $
Revision 1.1.1.1  2002/08/12 08:29:51  root
Automated import from Sourceforge.net

Revision 1.1  2002/08/11 19:07:45  betel
Initial deposit

*/
