/* $Id: readseqs.h,v 1.1.1.2 2002/02/13 09:25:24 root Exp $
*******************************************************************************************
Mount Sinai Hospital (MSH)

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
           Gary Bader and Christopher W.V. Hogue
           hogue@mshri.on.ca


  REVISION/CONTRIBUTION NOTES:

  June 1, 2000  Original MSH Revision.
                Hogue Lab - University of Toronto Biochemistry
                Department and the Samuel Lunenfeld Research
                Institute, Mount Sinai Hospital
                http://bioinfo.mshri.on.ca  hogue@mshri.on.ca
                ftp://ftp.mshri.on.ca/


  PUBLICATION TO CITE:


  DESCRIPTION
  header for FASTA sequence iterator functions



*******************************************************************************************/


#ifndef _DEVTOOLS_DBITER_
#define _DEVTOOLS_DBITER_

/*#include <ncbi.h>*/
#include <readdb.h>

#ifdef __cplusplus
extern "C" {
#endif

/*Opens a formatdb formatted protein FASTA database with a given filename.*/
ReadDBFILEPtr extern OpenProteinFastaDB(CharPtr filename);

/*Closes a previously opened protein FASTA database.*/
ReadDBFILEPtr extern CloseProteinFastaDB(ReadDBFILEPtr rdbfp);

/*Write total length and number of sequences in a database to a stream.*/
Boolean extern WriteDBStatsToStream(FILE *fpStream, ReadDBFILEPtr rdbfp);

/*Get a sequence from the database by gi number.
  NOTE: Caller must deallocate returned sequence.*/
CharPtr extern GetSeqByGI(ReadDBFILEPtr rdbfp, Int4 gi);

/*Get a sequence definition line from the database by gi number.
  NOTE: Caller must deallocate returned definition line.*/
CharPtr extern GetDeflineByGI(ReadDBFILEPtr rdbfp, Int4 gi);

/*Get a PDB code by gi from the database.
  NOTE: Caller must deallocate returned definition line.*/
CharPtr extern GetPDBCodeByGI(ReadDBFILEPtr rdbfp, Int4 gi);

/*Database iterator - returns the next GI in the database. Returns -1 if no more records, -2 if error.
  Usage note: Pass a Boolean variable as 'start'. E.g. GetNextRecordGI(rdbfp, &start);*/
Int4 extern GetNextRecordGI(ReadDBFILEPtr rdbfp, Boolean PNTR start);

/*Database iterator - returns the next sequence in the database. Returns -1 if no more records, -2 if error.
  Usage note: Pass a Boolean variable as 'start'. E.g. GetNextSequence(rdbfp, &start);
  NOTE: Caller must deallocate returned sequence.*/
CharPtr extern GetNextSequence(ReadDBFILEPtr rdbfp, Boolean PNTR start);


#ifdef __cplusplus
}
#endif

#endif /*_DEVTOOLS_DBITER_*/

/*
$Log: readseqs.h,v $
Revision 1.1.1.2  2002/02/13 09:25:24  root
Automated import from Sourceforge.net

Revision 1.2  2002/02/12 21:33:01  kaca
makefile overhaul to accommodate different database layers

Revision 1.1.1.1  2001/05/25 21:36:19  kaca
initial import

Revision 1.1.1.1  2001/05/25 20:43:00  kaca
code examples for SeqHound

Revision 1.2  2001/02/23 21:05:19  kaca
license change

Revision 1.1.1.1  2001/02/23 17:54:40  kaca
moved out of Source Forge  part of SeqHound

*/



