/* $Id: cddb.c,v 1.3 2004/01/13 21:43:33 zwang Exp $*/
/*
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


  PROGRAM: Human Coding region parser

  AUTHORS: Doron Betel betel@mshri.on.ca
            and Christopher W.V. Hogue hogue@mshri.on.ca


  REVISION/CONTRIBUTION NOTES:

  March 1, 2001  Original MSH Revision.
                Hogue Lab - University of Toronto Biochemistry
                Department and the Samuel Lunenfeld Research
                Institute, Mount Sinai Hospital
                http://bioinfo.mshri.on.ca  hogue@mshri.on.ca
                ftp://ftp.mshri.on.ca/


  PUBLICATION TO CITE:
*/
/***************************************
* Human coding region takes the a list of
* gi list, generates a SLRIFasta file for each
* gi  and dumps it to file
***************************************/
#include <ncbi.h>
#include <objall.h>
#include <stdio.h>
#include <stdlib.h>
#include "codRegion.h"
#include <string.h>
#include <errno.h>
#include <intrez_db.h>
#include <objslristruc.h>
#include <seqhound.h>

#define NUMARGS 2
#define LOGING "taxCD_logfile"
#define HUMAN_TAXID  9606

 /*The list of flags for humanCD*/
 Args myargs[NUMARGS]={
		{"Taxonomy", NULL ,NULL,NULL,FALSE, 't',ARG_INT, 0.0, 0, NULL},
		{"Initialize database file","F", NULL, NULL, TRUE, 'n', ARG_BOOLEAN, 0.0,0, NULL}
  		};
		
/*These taxon enumeration type*/
enum TAXON_TYPE {
		E_coli = 562,
		Methanoc = 2190,
		Yeast = 4932,
		C_elegans = 6239,
		Drosophila = 7227,
		Frog = 8355,
		Human = 9606,
		Mouse = 10090,
		Heptatis = 11103,
		HIV = 12721} TAXON;

/*function prototypes*/
ValNodePtr GetGis(Int4 taxonomy);
void MakeCodeRegionRecords(ValNodePtr );
void MakeCodeRegionRecordsII(ValNodePtr, CharPtr );
Int8 ValNodeSize(ValNodePtr);

Int2 Main()
{
	/*Variables*/
	/*FILE* fp=NULL; */
	ValNodePtr head=NULL;
	ValNodePtr gi=NULL;
	Int2 er=0;
	Int4 last = 0;
	Int8 numOfGi=0;
	Char buf[20];

	ErrSetOptFlags(EO_LOG_SEVERITY | EO_LOG_CODES | EO_LOG_FILELINE | EO_LOG_USRFILE);
  	/*use ByteStore error log function so multiple cgi do not write to the
  	same log_file. No interleaved log lines*/
  	BSErrLogOpen(LOGING, ELOG_BANNER);
   	ErrSetLogLevel(SEV_WARNING);
  	ErrSetMessageLevel(SEV_WARNING);

  	/*get the arguments from the cgi call or command line*/
	if(!GetArgs("cddb", NUMARGS, myargs)) {

		fprintf(stdout, "cddb.c: Can not process the arguments\n");
		return 1; /*failure*/
	}

	/*Initialize TAXON variable*/
	TAXON= myargs[0].intvalue;

	/*open SeqHound database*/
	if(!SHoundInit(FALSE, "cddb")){
		exit(1);
	}
	/*open database and assign pointers to fileds and tags*/
	if( !OpenCDDB ( "CDDB" , (Boolean)myargs[1].intvalue))
	{
		ErrPostEx(SEV_ERROR, 0, 0, "cannot open CDDB database");
		return 1;
	}

	/*open precompute*/
/*	Cfg_shound_new();*/

	/*************************************
	*Call seqhound to get the proteins
	*ValNode gi's
	*************************************/
	gi=(ValNodePtr)GetGis(TAXON);

	if(gi==NULL){
		ErrPostEx(SEV_FATAL, 0, 0, "Can not get gi list from SeqHound\n");
		exit(1);

	}
	head=gi;
	numOfGi=ValNodeSize(gi);
	Int8toStr(numOfGi, buf);
	fprintf(stdout," Number of Gi's in this list= %s\n",IsNullStr(buf));
	/*call to the CD function - pass it the gi list and the outputfile*/
	/*There are two CodingRegion functions. The second one collects only
	* those that are of a specific partition
	* Nov. 2001 -currently, we collect all the coding regions of humans
	* precompute has two human gi lists: one is GenBank and the other is RefSeq
	* RefSeq is a subset of GenBank, that's why we collect the CD of all humans*/
	MakeCodeRegionRecords(gi);
	/*MakeCodeRegionRecordsII(gi, "gbrscu");*/
	/*testing functions*/
	/*PrintCDDB();*/

	if(head!=NULL)
		ValNodeFree(head);
         CloseCDDB();
         SHoundFini();
/*	Cfg_FreeMem();*/
	/*close BSError log file*/
	BSErrLogClose();
	return 0;
}/* End of Main  */

/*retrieves a gilist of a taxonomy*/
ValNodePtr GetGis(Int4 taxonomy)
{
	if(taxonomy==HUMAN_TAXID)
		/*get the human precomputed list from primate devision only*/
/*		return (ValNodePtr)SHoundProteinsFromTaxIDIII((-1)*taxonomy);*/
		/*Get all the human protein sequences- big list*/
		return (ValNodePtr)SHoundProteinsFromTaxIDIII(taxonomy);
	else
		/*all other gi list. If the taxon has a precomputed list it will get that
		* otherwise it will collect the list from SeqHoud (see ...FromTaxIDII)*/
		return (ValNodePtr)SHoundProteinsFromTaxIDIII(taxonomy);
}

/*Create the CDDB records*/
/*Extracts the coding region from a gi into an SLRIFasta and assign to CDDB fields
**/
void MakeCodeRegionRecords(ValNodePtr gilist)
{
	StCddbPtr cdp;
	SLRIFastaPtr fasta_ptr=NULL;
	Int4 numOfRec=0;
	cdp= StCddbNew();

	/*trarverse through the gi list*/
	while(gilist !=NULL){
		fasta_ptr=codingRegion(gilist->data.intvalue); /*get SLRIFastaCD for gi*/
		if(fasta_ptr != NULL && fasta_ptr->sequence !=NULL){
			cdp->gi = gilist->data.intvalue;
			if(fasta_ptr->defline==NULL){
				fasta_ptr->defline= "No definition\n";
			}
			cdp->asn1=fasta_ptr;
			if(!AppendRecordCDDB(cdp))
			{
				ErrPostEx(SEV_ERROR, 0 , 0, "MakeCodeRegionRecords: Cannot append record\n");
				BSErrLogClose();
				exit(1);
			}
			numOfRec++;
			fasta_ptr=SLRIFastaFree(fasta_ptr);
			/*reset StCddbPtr*/
			ResetStCDDB(&cdp);
		}
		gilist=gilist->next;
	}
	fprintf(stdout, "Number of records build= %d\n",numOfRec);
	StCddbFree(cdp);
}

/*Second version of MakeCodeRegionRecords
* this version checks the gi's devision
* so we use only the gi from that devision*/
void MakeCodeRegionRecordsII(ValNodePtr gilist, CharPtr division)
{
	StCddbPtr cdp;
	SLRIFastaPtr fasta_ptr=NULL;
	Int4 numOfRec=0;
	cdp= StCddbNew();

	/*trarverse through the gi list*/
	while(gilist !=NULL){
		/*check if this gi is part of RefSeq division*/
		if(!IsThisDivision(gilist->data.intvalue, division))
		{
			gilist=gilist->next;
		}
		else
		{
			fasta_ptr=codingRegion(gilist->data.intvalue); /*get SLRIFastaCD for gi*/
			if(fasta_ptr != NULL && fasta_ptr->sequence !=NULL){
				cdp->gi = gilist->data.intvalue;
				if(fasta_ptr->defline==NULL){
					fasta_ptr->defline= "No definition\n";
				}
				cdp->asn1=fasta_ptr;
				if(!AppendRecordCDDB(cdp))
				{
					ErrPostEx(SEV_ERROR, 0 , 0, "MakeCodeRegionRecords: Cannot append record\n");
					BSErrLogClose();
					exit(1);
				}
				numOfRec++;
				fasta_ptr=SLRIFastaFree(fasta_ptr);
				/*reset StCddbPtr*/
				ResetStCDDB(&cdp);
			}
			gilist=gilist->next;
		}
	}
	ErrPostEx(SEV_WARNING, 0,0," Number of records build=%d\n", numOfRec);
	fprintf(stdout, "Number of records build= %d\n",numOfRec);
	StCddbFree(cdp);
}

Int8 ValNodeSize(ValNodePtr head)
{
	Int8 size=0;
	if( head==NULL)
		return size;
	while(head!=NULL)
	{
		size++;
		head=head->next;
	}
	return size;
}

/*
$Log: cddb.c,v $
Revision 1.3  2004/01/13 21:43:33  zwang
Commented out PrintCDDB.

Revision 1.2  2004/01/06 22:32:13  skuznets
Code audit has been done sucessfully.
Possible NULL pointer issues has been eliminated.

Revision 1.1.1.7  2002/09/25 08:30:25  root
Automated import from Sourceforge.net

Revision 1.8  2002/09/24 23:15:46  betel
Small fixes

Revision 1.7  2002/04/05 16:26:31  kaca
moved back from ../rps

Revision 1.1  2002/04/03 18:06:59  kaca
moved from ../parsers

Revision 1.5  2002/02/22 00:03:17  betel
Change of license. Rpsdb still not tested.

Revision 1.4  2001/12/03 16:31:40  betel
Correction to RefSeq precompute and cddb parser

Revision 1.1.1.1  2001/05/25 21:36:20  kaca
initial import

Revision 1.2  2001/04/05 14:27:19  doron
Added seqhound.h

Revision 1.1  2001/03/27 21:11:47  doron
Addition of cddb for generating CDDB database

*/


