/* $Id: readseqs.c,v 1.2 2004/01/06 22:32:12 skuznets Exp $
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
  FASTA sequence iterator functions

  Requires that the database be formatted by "formatdb"

  formatdb use: formatdb -t PDB -i pdb.faa -o T


*******************************************************************************************/
#include <slri_misc.h>
#include <readseqs.h>

/*Opens a formatdb formatted protein FASTA database with a given filename.*/
ReadDBFILEPtr OpenProteinFastaDB(CharPtr filename)
{
	ReadDBFILEPtr rdbfp=NULL;

	if(filename==NULL) {
		ErrPostEx(SEV_ERROR,0,0,"OpenProteinFastaDB: Passed filename was NULL.");
		return(NULL);
	}

	rdbfp = readdb_new(filename, TRUE);
	if(rdbfp==NULL) {
		ErrPostEx(SEV_ERROR,0,0,"OpenProteinFastaDB: readdb_new failed.");
	}

	return(rdbfp);
}

/*Closes a previously opened protein FASTA database.*/
ReadDBFILEPtr CloseProteinFastaDB(ReadDBFILEPtr rdbfp)
{
	if(rdbfp==NULL) {
		return(NULL);
	}

	return(readdb_destruct(rdbfp));
}

/*Write total length and number of sequences in a database to a stream.*/
Boolean WriteDBStatsToStream(FILE *fpStream, ReadDBFILEPtr rdbfp)
{
	Int8 totalLen=0;
	Int4 totalSeqNum=0;
        /* SK: */
        CharPtr ptr, ptr1;

	if(fpStream==NULL) {
		ErrPostEx(SEV_ERROR,0,0,"WriteDBStatsToStream: Passed fpStream was NULL.");
		return(FALSE);
	}
	if(rdbfp==NULL) {
		ErrPostEx(SEV_ERROR,0,0,"WriteDBStatsToStream: Passed ReadDBFILEPtr was NULL.");
		return(FALSE);
	}

	if(!readdb_get_totals(rdbfp, &totalLen, &totalSeqNum)) {
		ErrPostEx(SEV_ERROR,0,0,"WriteDBStatsToStream: readdb_get_totals failed.");
		return(FALSE);
	}

        /* SK: */
	ptr  = readdb_get_filename(rdbfp);
        ptr1 = readdb_get_date(rdbfp);
	fprintf(fpStream, "Statistics for FASTA database %s created %s.\n", IsNullStr(ptr), IsNullStr(ptr1));
	fprintf(fpStream, "Total number of sequences: %ld\n", totalSeqNum);
	fprintf(fpStream, "Total length of all sequences: %ld\n", totalLen);
	fprintf(fpStream, "Longest sequence: %ld amino acids\n", readdb_get_maxlen(rdbfp));

	return(TRUE);
}


/*Free an entire linked list of SeqIdPtr*/
SeqIdPtr static SeqIdChainFree(SeqIdPtr sip)
{
	SeqIdPtr sipTmp=NULL;

	while(sip!=NULL) {
		sipTmp = sip;
		sip=sip->next;
		sipTmp = SeqIdFree(sipTmp);
	}

	return(NULL);
}

/*Get a sequence from the database by gi number.
  NOTE: Caller must deallocate returned sequence.*/
CharPtr GetSeqByGI(ReadDBFILEPtr rdbfp, Int4 gi)
{
	Int4 dbSeqNum=0, retVal=0;
	Uint1Ptr buffer=NULL;
	CharPtr sequence=NULL;
	ByteStorePtr bsp=NULL;

	if(rdbfp==NULL) {
		ErrPostEx(SEV_ERROR,0,0,"GetSeqByGI: Passed ReadDBFILEPtr was NULL.");
		return(NULL);
	}
	if(gi<0) {
		ErrPostEx(SEV_ERROR,0,0,"GetSeqByGI: Passed GI was < 0.");
		return(NULL);
	}

	dbSeqNum=readdb_gi2seq(rdbfp, gi, NULL);
	if(dbSeqNum==-1) {
		ErrPostEx(SEV_ERROR,0,0,"GetSeqByGI: GI was not found in database.");
		return(NULL);
	}
	else if(dbSeqNum<-1) {
		ErrPostEx(SEV_ERROR,0,0,"GetSeqByGI: (readdb_gi2seq failed) Database error %ld. Try re-running formatdb.", dbSeqNum);
		return(NULL);
	}

	if((retVal=readdb_get_sequence(rdbfp, dbSeqNum, &buffer))<0) {
		if(buffer) {
			buffer = MemFree(buffer);
		}
		ErrPostEx(SEV_ERROR,0,0,"GetSeqByGI: readdb_get_sequence failed. Error %ld.", retVal);
		return(NULL);
	}
	/*retVal is now the length of the sequence*/

	/*convert buffer from Seq_code_ncbistdaa to Seq_code_iupacaa*/
	bsp = BSNew(retVal);
	if(bsp==NULL) {
		buffer = MemFree(buffer);
		ErrPostEx(SEV_ERROR,0,0,"GetSeqByGI: BSNew failed.");
		return(NULL);
	}
	BSWrite(bsp, (VoidPtr) buffer, retVal);
	/*buffer is owned by rdbfp - don't have to free it here*/
	bsp = BSConvertSeq(bsp, Seq_code_iupacaa, Seq_code_ncbistdaa, retVal);
	sequence =(CharPtr)  BSStringSave(bsp);
	bsp=BSFree(bsp);

	return(sequence);
}

/*Get a sequence definition line from the database by gi number.
  NOTE: Caller must deallocate returned definition line.*/
CharPtr GetDeflineByGI(ReadDBFILEPtr rdbfp, Int4 gi)
{
	Int4 dbSeqNum=0, retVal=0;
	CharPtr defLine=NULL;

	if(rdbfp==NULL) {
		ErrPostEx(SEV_ERROR,0,0,"GetDeflineByGI: Passed ReadDBFILEPtr was NULL.");
		return(NULL);
	}
	if(gi<0) {
		ErrPostEx(SEV_ERROR,0,0,"GetDeflineByGI: Passed GI was < 0.");
		return(NULL);
	}

	dbSeqNum=readdb_gi2seq(rdbfp, gi, NULL);
	if(dbSeqNum==-1) {
		ErrPostEx(SEV_ERROR,0,0,"GetDeflineByGI: GI was not found in database.");
		return(NULL);
	}
	else if(dbSeqNum<-1) {
		ErrPostEx(SEV_ERROR,0,0,"GetDeflineByGI: (readdb_gi2seq failed) Database error %ld. Try re-running formatdb.", dbSeqNum);
		return(NULL);
	}

	if((retVal=readdb_get_defline(rdbfp, dbSeqNum, &defLine))<0) {
		if(defLine) {
			defLine = MemFree(defLine);
		}
		ErrPostEx(SEV_ERROR,0,0,"GetDeflineByGI: readdb_get_defline failed. Error %ld.", retVal);
		return(NULL);
	}

	return(defLine);
}

/*Get a PDB code by gi from the database.
  NOTE: Caller must deallocate returned definition line.*/
CharPtr GetPDBCodeByGI(ReadDBFILEPtr rdbfp, Int4 gi)
{
	CharPtr pdbCode=NULL;
	PDBSeqIdPtr psip=NULL;
	SeqIdPtr sip=NULL, sipTmp=NULL;

	if(rdbfp==NULL) {
		ErrPostEx(SEV_ERROR,0,0,"GetPDBCodeByGI: Passed ReadDBFILEPtr was NULL.");
		return(NULL);
	}
	if(gi<0) {
		ErrPostEx(SEV_ERROR,0,0,"GetPDBCodeByGI: Passed GI was < 0.");
		return(NULL);
	}

	if(!readdb_get_descriptor(rdbfp, readdb_gi2seq(rdbfp, gi, NULL), &sip, NULL)) {
		if(sip) {
			sip = SeqIdChainFree(sip);
		}
		ErrPostEx(SEV_ERROR,0,0,"GetPDBCodeByGI: readdb_get_descriptor failed.");
		return(NULL);
	}

	sipTmp = sip;
	while(sipTmp!=NULL) {
		if(sipTmp->choice==SEQID_PDB) {
			psip = sipTmp->data.ptrvalue;
			if(psip!=NULL) {
				pdbCode = StringSave(psip->mol);
			}
		}
		sipTmp=sipTmp->next;
	}
	sip = SeqIdChainFree(sip);

	return(pdbCode);
}

/*Database iterator - returns the next GI in the database. Returns -1 if no more records, -2 if error.
  Usage note: Pass a Boolean variable as 'start'. E.g. GetNextRecordGI(rdbfp, &start);*/
Int4 GetNextRecordGI(ReadDBFILEPtr rdbfp, Boolean PNTR start)
{
	static Int4 seqNum;
	SeqIdPtr sip=NULL;
	Int4 gi=0;
	CharPtr descr=NULL;

	if(rdbfp==NULL) {
		ErrPostEx(SEV_ERROR,0,0,"GetNextRecordGI: Passed ReadDBFILEPtr was NULL.");
		return(-2);
	}
	if(start==NULL) {
		ErrPostEx(SEV_ERROR,0,0,"GetNextRecordGI: Passed Boolean pointer was NULL.");
		return(-2);
	}

	if(*start) {
		seqNum=0;
		*start=FALSE;
	}
	if(seqNum >= readdb_get_num_entries(rdbfp)) {
		return(-1);
	}

	if(!readdb_get_descriptor(rdbfp, seqNum, &sip, NULL)) {
		if(sip) {
			sip = SeqIdChainFree(sip);
		}
		ErrPostEx(SEV_ERROR,0,0,"GetNextRecordGI: readdb_get_descriptor failed.");
		return(-2);
	}
	seqNum++;

	if((sip)&&(sip->choice==SEQID_GI)) {
		gi = sip->data.intvalue;
	}
	else {
		ErrPostEx(SEV_ERROR,0,0,"GetNextRecordGI: Unable to find GI in returned SeqIdPtr.");
		return(-2);
	}

	/*free SeqIdPtr chain*/
	sip = SeqIdChainFree(sip);

	return(gi);
}

/*Database iterator - returns the next sequence in the database. Returns -1 if no more records, -2 if error.
  Usage note: Pass a Boolean variable as 'start'. E.g. GetNextSequence(rdbfp, &start);
  NOTE: Caller must deallocate returned sequence.*/
CharPtr GetNextSequence(ReadDBFILEPtr rdbfp, Boolean PNTR start)
{
	CharPtr sequence=NULL;
	Int4 gi=0;

	if(rdbfp==NULL) {
		ErrPostEx(SEV_ERROR,0,0,"GetNextSequence: Passed ReadDBFILEPtr was NULL.");
		return(NULL);
	}
	if(start==NULL) {
		ErrPostEx(SEV_ERROR,0,0,"GetNextSequence: Passed Boolean pointer was NULL.");
		return(NULL);
	}

	if((gi = GetNextRecordGI(rdbfp, start)) >= 0) {
		sequence = GetSeqByGI(rdbfp, gi);
	}
	else {
		return(NULL);
	}

	return(sequence);
}


/*
   Usage example:

	ReadDBFILEPtr rdbfp=NULL;
	CharPtr seq=NULL;
	Boolean start=TRUE;
	Int4 gi=0;

	rdbfp=OpenProteinFastaDB("pdbaa.faa");

	WriteDBStatsToStream(stdout, rdbfp);

	while((seq=GetNextSequence(rdbfp, &start))!=NULL) {
		printf("%s\n",IsNullStr(seq));
		seq = MemFree(seq);
	}

	rdbfp=CloseProteinFastaDB(rdbfp);

*/

/*
$Log: readseqs.c,v $
Revision 1.2  2004/01/06 22:32:12  skuznets
Code audit has been done sucessfully.
Possible NULL pointer issues has been eliminated.

Revision 1.1.1.3  2002/02/15 09:25:34  root
Automated import from Sourceforge.net

Revision 1.3  2002/02/15 06:42:23  micheld
Modified makefile targets to include dependency building.

Revision 1.2  2002/02/12 21:33:01  kaca
makefile overhaul to accommodate different database layers

Revision 1.1.1.1  2001/05/25 21:36:19  kaca
initial import

Revision 1.1.1.1  2001/05/25 20:43:00  kaca
code examples for SeqHound

Revision 1.3  2001/03/26 19:59:59  kaca
changes in include files

Revision 1.2  2001/02/23 21:05:19  kaca
license change

Revision 1.1.1.1  2001/02/23 17:54:40  kaca
moved out of Source Forge  part of SeqHound

*/



