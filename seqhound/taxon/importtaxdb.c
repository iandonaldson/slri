/* $Id: importtaxdb.c,v 1.4 2004/10/01 17:36:53 zwang Exp $
-- ***************************************************************************
  Copyright (C) 2000 Mount Sinai Hospital (MSH)

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
           Gary Bader and Christopher W.V. Hogue hogue@mshri.on.ca


  REVISION/CONTRIBUTION NOTES:

  June 1, 2000  Original MSH Revision.
                Hogue Lab - University of Toronto Biochemistry
                Department and the Samuel Lunenfeld Research
                Institute, Mount Sinai Hospital
                http://bioinfo.mshri.on.ca  hogue@mshri.on.ca
                ftp://ftp.mshri.on.ca/


  PUBLICATION TO CITE:


  DESCRIPTION
  taxonomy database part of SeqHound


******************************************************************************************/

#include <importtaxdb.h>
#ifndef ODBC
CODE4 cb;
#endif

#define NUMARGS 1
Args myargs[NUMARGS] = {
        {"Is this an update","F", NULL, NULL, TRUE, 'u', ARG_BOOLEAN, 0.0, 0, NULL}
  };


/*****************/
/*Parse Functions*/
/*****************/

/* Read an entire TaxDB record (up until the given end of record string) */
Int2 BSGetTaxRecordFromFile(ByteStorePtr bsp, FILE *fp, CharPtr eorString)
{
	CharPtr buffer=NULL;
	Boolean seenEOR=FALSE, seenEOF=FALSE, bspSaved=FALSE;
	ByteStorePtr bspSave=NULL;

	if(bsp==NULL) {
		ErrPostEx(SEV_INFO, 0, 0, "BSGetTaxRecordFromFile: Passed ByteStorePtr was null.");
		return(1);
	}
	if(fp==NULL) {
		ErrPostEx(SEV_ERROR, 0, 0, "BSGetTaxRecordFromFile: Passed file pointer is NULL.");
		return(1);
	}
	if(eorString==NULL) {
		ErrPostEx(SEV_INFO, 0, 0, "BSGetTaxRecordFromFile: Passed eorString was null.");
		return(1);
	}

	while(!seenEOR) {
		if(BSGetLineFromFile(bsp, fp) == EOF) {
			seenEOR=TRUE;
			seenEOF=TRUE;
		}
		/*check for end of record - put newline back after it was stripped by BSGetLineFromFile*/
		BSSeek(bsp, 0L, SEEK_END);
		BSPutByte(bsp, '\n');
		buffer = BSStringSave(bsp);
		if(strstr(buffer, eorString) != NULL) {
			seenEOR=TRUE;
		}
		buffer = MemFree(buffer);
		/*now get rid of newline that was added*/
		BSSeek(bsp, -1L, SEEK_END);
		BSDelete(bsp, 1L);
		if(bspSave==NULL) {
			if(!seenEOR) {
				bspSave = BSDup(bsp);
				bspSaved=TRUE;
			}
		}
		else {
			buffer = BSStringSave(bsp);
			BSWrite(bspSave, buffer, BSLen(bsp));
			buffer = MemFree(buffer);
		}
	}

	if(bspSaved) {
		/*overwrite destination - make sure it is clear*/
		BSSeek(bsp, 0L, SEEK_SET);
		BSDelete(bsp, BSLen(bsp));
		BSSeek(bsp, 0L, SEEK_SET);
		/*transfer bspSave to bsp*/
		buffer = BSStringSave(bspSave);
		BSWrite(bsp, buffer, BSLen(bspSave));
		buffer = MemFree(buffer);
		bspSave = BSFree(bspSave);
	}

	if(seenEOF)
		return(EOF);

	return(0);
}

/*Add a character to a string and shift existing characters over by one - leftmost char gets bumped*/
Int2 ShiftCharsInBuffer(CharPtr buffer, Int2 ch, Int4 bufferLength)
{
	Int4 i=0;

	if(buffer==NULL) {
		ErrPostEx(SEV_INFO, 0, 0, "ShiftCharsInBuffer: Passed buffer was null.");
		return(1);
	}

	for(i=0;i<(bufferLength-1);i++) {
		buffer[i]=buffer[i+1];
	}
	buffer[bufferLength-1]=(Char) ch;

	return(0);
}

/*Parse a field (with a given field delimiter) from a bytestore
Note: allocates memory - caller responsible for freeing returned string*/
CharPtr Parse_StringFromBSP(ByteStorePtr bspSource, CharPtr fieldDelimStr, Boolean *error)
{
	CharPtr fieldDelimBuf=NULL, retString=NULL;
	Int2 ch=0, fieldDelimBufLength=0;
	ByteStorePtr bspBuf=NULL;

	if(bspSource==NULL) {
		ErrPostEx(SEV_INFO, 0, 0, "Parse_StringFromBSP: Passed ByteStorePtr was null.");
		if(error!=NULL)
			*error=TRUE;
		return(NULL);
	}
	if(fieldDelimStr==NULL) {
		ErrPostEx(SEV_INFO, 0, 0, "Parse_StringFromBSP: Passed fieldDelimStr was null.");
		if(error!=NULL)
			*error=TRUE;
		return(NULL);
	}

	if(error!=NULL)
		*error=FALSE;

	bspBuf = BSNew(20);
	if(bspBuf==NULL) {
		ErrPostEx(SEV_INFO, 0, 0, "Parse_StringFromBSP: BSNew failed.");
		if(error!=NULL)
			*error=TRUE;
		return(NULL);
	}

	/*create a buffer to store the last strlen(fieldDelimStr) characters of the stream
	- this buffer is used to check if the field delim string has been hit*/
	fieldDelimBufLength = strlen(fieldDelimStr);
	fieldDelimBuf = MemNew((size_t) (fieldDelimBufLength+1)*sizeof(Char));
	if(fieldDelimBuf==NULL) {
		ErrPostEx(SEV_INFO, 0, 0, "Parse_StringFromBSP: MemNew failed for fieldDelimBuf.");
		if(error!=NULL)
			*error=TRUE;
		bspBuf=BSFree(bspBuf);
		return(NULL);
	}
	/*make sure fieldDelimBuf is null terminated*/
	fieldDelimBuf[fieldDelimBufLength]='\0';

	while((ch=BSGetByte(bspSource))!=EOF) {
		ShiftCharsInBuffer(fieldDelimBuf, ch, 3);
		if(strcmp(fieldDelimBuf, fieldDelimStr)==0) {
			break;
		}
		BSPutByte(bspBuf, ch);
	}
	fieldDelimBuf = MemFree(fieldDelimBuf);

	/*short-circuit if nothing was read*/
	if(BSLen(bspBuf)<fieldDelimBufLength) {
		bspBuf=BSFree(bspBuf);
		return(NULL);
	}

	retString = BSStringSave(bspBuf);
	/*strip off field delim chars at end of string*/
	retString[BSLen(bspBuf)-fieldDelimBufLength+1]='\0';

	bspBuf=BSFree(bspBuf);

	return(retString);
}

/*Parse an integer from a field in a bytestore (with a given field delimiter)
Returns INT4_MIN and sets the error flag in case of error*/
Int4 Parse_IntegerFromBSP(ByteStorePtr bspSource, CharPtr fieldDelimStr, Boolean *error)
{
	Int4 num=0;
	CharPtr fieldStr=NULL;

	if(bspSource==NULL) {
		ErrPostEx(SEV_INFO, 0, 0, "Parse_IntegerFromBSP: Passed ByteStorePtr was null.");
		if(error!=NULL)
			*error=TRUE;
		return(INT4_MIN);
	}
	if(fieldDelimStr==NULL) {
		ErrPostEx(SEV_INFO, 0, 0, "Parse_IntegerFromBSP: Passed fieldDelimStr was null.");
		if(error!=NULL)
			*error=TRUE;
		return(INT4_MIN);
	}

	if(error!=NULL)
		*error=FALSE;

	fieldStr = Parse_StringFromBSP(bspSource, fieldDelimStr, error);
	if(fieldStr==NULL) {
		ErrPostEx(SEV_INFO, 0, 0, "Parse_IntegerFromBSP: Parse_StringFromBSP failed.");
		if(error!=NULL)
			*error=TRUE;
		return(INT4_MIN);
	}
	if(!Misc_IsNumeric(fieldStr)) {
		ErrPostEx(SEV_INFO, 0, 0, "Parse_IntegerFromBSP: Field is not numeric.");
		if(error!=NULL)
			*error=TRUE;
		fieldStr = MemFree(fieldStr);
		return(INT4_MIN);
	}

	num = atol(fieldStr);
	fieldStr = MemFree(fieldStr);

	return(num);
}

/*Parse a record in a bytestore from the NODESFILE*/
SLRITaxonPtr Parse_TaxDBNodeRecord(ByteStorePtr bsp)
{
	SLRITaxonPtr stp=NULL;
	Int4 taxId=0, parentTaxId=0, divId=0, genCodeId=0, mitoGenCodeId=0, rankPremod=0, rankPostmod=0;
	Uint2 rankEnum=0;
	CharPtr rank=NULL, emblCode=NULL, comments=NULL, rankNoSpace=NULL;
	SLRITaxonNamePtr stnp=NULL;
	SLRITaxonRankPtr strp=NULL;
	Boolean error=FALSE, divIdInh=FALSE, genCodeIdInh=FALSE, mitoGenCodeIdInh=FALSE, gbHidden=FALSE, hiddenSubtree=FALSE;

	if(bsp==NULL) {
		ErrPostEx(SEV_INFO, 0, 0, "Parse_TaxDBNodeRecord: Passed ByteStorePtr was null.");
		return(NULL);
	}

	/*parse record*/
	taxId = Parse_IntegerFromBSP(bsp, FIELDEND, &error);
	if(error) {
		ErrPostEx(SEV_INFO, 0, 0, "Parse_TaxDBNodeRecord: taxId not parsed.");
	}
	parentTaxId = Parse_IntegerFromBSP(bsp, FIELDEND, &error);
	if(error) {
		ErrPostEx(SEV_INFO, 0, 0, "Parse_TaxDBNodeRecord: parentTaxId not parsed - TaxID: %ld.", taxId);
	}
	rank = Parse_StringFromBSP(bsp, FIELDEND, &error);
	if(error) {
		ErrPostEx(SEV_INFO, 0, 0, "Parse_TaxDBNodeRecord: rank not parsed - TaxID: %ld.", taxId);
	}
	emblCode = Parse_StringFromBSP(bsp, FIELDEND, &error);	/*optional*/
	divId = Parse_IntegerFromBSP(bsp, FIELDEND, &error);
	if(error) {
		ErrPostEx(SEV_INFO, 0, 0, "Parse_TaxDBNodeRecord: divId not parsed - TaxID: %ld.", taxId);
	}
	divIdInh = (Boolean) Parse_IntegerFromBSP(bsp, FIELDEND, &error);
	if(error) {
		ErrPostEx(SEV_INFO, 0, 0, "Parse_TaxDBNodeRecord: divIdInh not parsed - TaxID: %ld.", taxId);
	}
	genCodeId = Parse_IntegerFromBSP(bsp, FIELDEND, &error);
	if(error) {
		ErrPostEx(SEV_INFO, 0, 0, "Parse_TaxDBNodeRecord: genCodeId not parsed - TaxID: %ld.", taxId);
	}
	genCodeIdInh = (Boolean) Parse_IntegerFromBSP(bsp, FIELDEND, &error);
	if(error) {
		ErrPostEx(SEV_INFO, 0, 0, "Parse_TaxDBNodeRecord: genCodeIdInh not parsed - TaxID: %ld.", taxId);
	}
	mitoGenCodeId = Parse_IntegerFromBSP(bsp, FIELDEND, &error);
	if(error) {
		ErrPostEx(SEV_INFO, 0, 0, "Parse_TaxDBNodeRecord: mitoGenCodeId not parsed - TaxID: %ld.", taxId);
	}
	mitoGenCodeIdInh = (Boolean) Parse_IntegerFromBSP(bsp, FIELDEND, &error);
	if(error) {
		ErrPostEx(SEV_INFO, 0, 0, "Parse_TaxDBNodeRecord: mitoGenCodeIdInh not parsed - TaxID: %ld.", taxId);
	}
	gbHidden = (Boolean) Parse_IntegerFromBSP(bsp, FIELDEND, &error);
	if(error) {
		ErrPostEx(SEV_INFO, 0, 0, "Parse_TaxDBNodeRecord: gbHidden not parsed - TaxID: %ld.", taxId);
	}
	hiddenSubtree = (Boolean) Parse_IntegerFromBSP(bsp, FIELDEND, &error);
	if(error) {
		ErrPostEx(SEV_INFO, 0, 0, "Parse_TaxDBNodeRecord: hiddenSubtree not parsed - TaxID: %ld.", taxId);
	}
	comments = Parse_StringFromBSP(bsp, FIELDEND, &error);	/*optional*/

	/*create a temporary name - this will be replaced when the names database is read*/
	stnp = C_SLRITaxonName_New("temp235", SLRI_taxon_name_name_class_other);

	/*convert text rank to enumerated type*/
	if(rank==NULL) {
		ErrPostEx(SEV_INFO, 0, 0, "Parse_TaxDBNodeRecord: Rank is required and was not parsed - TaxID: %ld.", taxId);
		emblCode = MemFree(emblCode);
		comments = MemFree(comments);
		return(NULL);
	}
	/*look for post modification*/
	rankPostmod = TaxDB_GetPostModFromRankName(&rank);
	rankNoSpace = StringSave(StripSpaces(rank));
	rank = MemFree(rank);
	rank = rankNoSpace;
	/*look for pre modification*/
	rankPremod = TaxDB_GetPreModFromRankName(&rank);
	rankNoSpace = StringSave(StripSpaces(rank));
	rank = MemFree(rank);
	rank = rankNoSpace;
	/*find rank enumeration*/
	rankEnum = TaxDB_Enum_RankString2Enum(rank);
	strp = C_SLRITaxonRank_New(rankEnum);
	rank = MemFree(rank);
	if(rankPremod != -1) {
		strp->premod = rankPremod;
	}
	else {
		strp->premod = SLRI_taxon_rank_premod_none;
	}
	if(rankPostmod != -1) {
		strp->postmod = rankPostmod;
	}
	else {
		strp->postmod = SLRI_taxon_rank_postmod_none;
	}

	stp = C_SLRITaxon_New(taxId, parentTaxId, stnp, strp, divId, genCodeId, mitoGenCodeId);
	if(emblCode!=NULL) {
		C_SLRITaxon_AddEMBLCode(stp, emblCode);
		emblCode = MemFree(emblCode);
	}
	stp->inherited_div=divIdInh;
	stp->inherited_gencode=genCodeIdInh;
	stp->inherited_mito_gencode=mitoGenCodeIdInh;
	stp->genbank_hidden=gbHidden;
	stp->hidden_subtree_root=hiddenSubtree;
	if(comments!=NULL) {
		C_SLRITaxon_AddComments(stp, comments);
		comments = MemFree(comments);
	}

	return(stp);
}

/*Parse a record in a bytestore from the NAMESFILE*/
SLRITaxonNamePtr Parse_TaxDBNameRecord(ByteStorePtr bsp, Int4Ptr taxIdRet)
{
	SLRITaxonNamePtr stnp=NULL;
	Int4 taxId=0;
	CharPtr name=NULL, uniqueName=NULL, nameClass=NULL;
	Uint2 classEnum=0;
	Boolean error=FALSE;

	if(bsp==NULL) {
		ErrPostEx(SEV_INFO, 0, 0, "Parse_TaxDBNameRecord: Passed ByteStorePtr was null.");
		return(NULL);
	}

	/*parse record*/
	taxId = Parse_IntegerFromBSP(bsp, FIELDEND, &error);
	if(error) {
		ErrPostEx(SEV_INFO, 0, 0, "Parse_TaxDBNameRecord: taxId not parsed.");
	}
	name = Parse_StringFromBSP(bsp, FIELDEND, &error);
	if(error) {
		ErrPostEx(SEV_INFO, 0, 0, "Parse_TaxDBNameRecord: name not parsed - TaxID: %ld.", taxId);
	}
	uniqueName = Parse_StringFromBSP(bsp, FIELDEND, &error);
	if(error) {
		ErrPostEx(SEV_INFO, 0, 0, "Parse_TaxDBNameRecord: uniqueName not parsed - TaxID: %ld.", taxId);
	}
	nameClass = Parse_StringFromBSP(bsp, FIELDEND, &error);
	if(error) {
		ErrPostEx(SEV_INFO, 0, 0, "Parse_TaxDBNameRecord: nameClass not parsed - TaxID: %ld.", taxId);
	}

	/*build structure*/

	/*convert text name class*/
	classEnum = TaxDB_Enum_NameClassString2Enum(nameClass);
	stnp = C_SLRITaxonName_New(name, classEnum);
	name = MemFree(name);
	if(uniqueName!=NULL) {
		C_SLRITaxonName_AddUniqueName(stnp, uniqueName);
		uniqueName = MemFree(uniqueName);
	}
	if(classEnum==SLRI_taxon_name_name_class_other) {
		C_SLRITaxonName_AddOtherClass(stnp, nameClass);
	}
	nameClass = MemFree(nameClass);

	*taxIdRet = taxId;

	return(stnp);
}

/*Parse a record in a bytestore from the DIVISIONFILE*/
SLRITaxonDivPtr Parse_TaxDBDivRecord(ByteStorePtr bsp)
{
	SLRITaxonDivPtr stdp=NULL;
	Int4 divId=0;
	CharPtr divCode=NULL, divName=NULL, comments=NULL;
	Boolean error=FALSE;

	if(bsp==NULL) {
		ErrPostEx(SEV_INFO, 0, 0, "Parse_TaxDBDivRecord: Passed ByteStorePtr was null.");
		return(NULL);
	}

	/*parse record*/
	divId = Parse_IntegerFromBSP(bsp, FIELDEND, &error);
	if(error) {
		ErrPostEx(SEV_INFO, 0, 0, "Parse_TaxDBDivRecord: divId not parsed.");
	}
	divCode = Parse_StringFromBSP(bsp, FIELDEND, &error);
	if(error) {
		ErrPostEx(SEV_INFO, 0, 0, "Parse_TaxDBDivRecord: divCode not parsed - divId: %ld.", divId);
	}
	divName = Parse_StringFromBSP(bsp, FIELDEND, &error);
	if(error) {
		ErrPostEx(SEV_INFO, 0, 0, "Parse_TaxDBDivRecord: divName not parsed - divId: %ld.", divId);
	}
	comments = Parse_StringFromBSP(bsp, FIELDEND, &error);
	if(error) {
		ErrPostEx(SEV_INFO, 0, 0, "Parse_TaxDBDivRecord: comments not parsed - divId: %ld.", divId);
	}

	/*build structure*/
	stdp = C_SLRITaxonDiv_New(divId, divCode, divName);
	divCode = MemFree(divCode);
	divName = MemFree(divName);
	if(comments!=NULL) {
		C_SLRITaxonDiv_AddComments(stdp, comments);
		comments = MemFree(comments);
	}

	return(stdp);
}

/*Parse a record in a bytestore from the GENCODEFILE*/
SLRITaxonGencodePtr Parse_TaxDBGenCodeRecord(ByteStorePtr bsp)
{
	SLRITaxonGencodePtr stgp=NULL;
	Int4 genCodeId=0;
	CharPtr abbrev=NULL, name=NULL, transTable=NULL, startCodons=NULL;
	Boolean error=FALSE;

	if(bsp==NULL) {
		ErrPostEx(SEV_INFO, 0, 0, "Parse_TaxDBGenCodeRecord: Passed ByteStorePtr was null.");
		return(NULL);
	}

	/*parse record*/
	genCodeId = Parse_IntegerFromBSP(bsp, FIELDEND, &error);
	if(error) {
		ErrPostEx(SEV_INFO, 0, 0, "Parse_TaxDBGenCodeRecord: genCodeId not parsed.");
	}
	abbrev = Parse_StringFromBSP(bsp, FIELDEND, &error);	/*optional*/
	name = Parse_StringFromBSP(bsp, FIELDEND, &error);
	if(error) {
		ErrPostEx(SEV_INFO, 0, 0, "Parse_TaxDBGenCodeRecord: name not parsed - genCodeId: %ld.", genCodeId);
	}
	transTable = Parse_StringFromBSP(bsp, FIELDEND, &error);	/*optional*/
	startCodons = Parse_StringFromBSP(bsp, FIELDEND, &error);	/*optional*/

	/*build structure*/
	stgp = C_SLRITaxonGencode_New(genCodeId, name);
	name = MemFree(name);
	if(transTable!=NULL) {
		C_SLRITaxonGencode_AddTransTable(stgp, transTable);
		transTable = MemFree(transTable);
	}
	if(startCodons!=NULL) {
		C_SLRITaxonGencode_AddStartCodons(stgp, startCodons);
		startCodons = MemFree(startCodons);
	}
	if(abbrev!=NULL) {
		C_SLRITaxonGencode_AddAbbrev(stgp, abbrev);
		abbrev = MemFree(abbrev);
	}

	return(stgp);
}

#ifdef PARSE_IN_MEMORY
/*Red black tree functions*/

/*data free function for taxID tree - frees the ValNode data*/
Pointer TaxIDTree_DataFree(Pointer data)
{
	SLRITaxonPtr stpData;

	stpData = (SLRITaxonPtr) data;

	return(SLRITaxonFree(stpData));
}

/*build children node list in memory*/
Int2 TaxIDTree_BuildChildrenNodeList(RedBlackTreeNodePtr rbtnpRoot, RedBlackTreeFunctionPackagePtr funPack)
{
	RedBlackTreeNodePtr rbtnpTmp=NULL, rbtnpParent=0;
	SLRITaxonPtr stp=NULL, stpParent=NULL;
	Int4 parentTaxId=0;

	if(rbtnpRoot==NULL) {
		ErrPostEx(SEV_INFO, 0, 0, "TaxIDTree_BuildChildrenNodeList: Passed rbtnpRoot was null.");
		return(1);
	}
	if(funPack==NULL) {
		ErrPostEx(SEV_INFO, 0, 0, "TaxIDTree_BuildChildrenNodeList: Passed RedBlackTreeFunctionPackagePtr was null.");
		return(1);
	}

	rbtnpTmp = RBTree_MinKeyNode(rbtnpRoot);
	do {
		stp = (SLRITaxonPtr) rbtnpTmp->data;
		parentTaxId = stp->parent_taxId;
		if(parentTaxId==stp->taxId) {
			continue;	/*don't allow any circles in the tree*/
		}
		rbtnpParent = RBTree_Find(rbtnpRoot, &parentTaxId, funPack);
		if(rbtnpParent==NULL) {
			ErrPostEx(SEV_INFO, 0, 0, "TaxIDTree_BuildChildrenNodeList: RBTree_Find failed for Tax ID %ld.", parentTaxId);
			continue;
		}
		stpParent = (SLRITaxonPtr) rbtnpParent->data;
		Misc_AddInt2SortedList(&(stpParent->children_taxId), *((Int4Ptr) rbtnpTmp->key), 0, TRUE);
	} while((rbtnpTmp = RBTree_Successor(rbtnpTmp)) != NULL);

	return(0);
}

/*write to disk and free*/
RedBlackTreeNodePtr TaxIDTree_WriteAndFree(RedBlackTreeNodePtr rbtnpRoot, RedBlackTreeFunctionPackagePtr funPack)
{
	SLRITaxonPtr stpData=NULL;

	if(rbtnpRoot!=NULL) {
		rbtnpRoot->rightChild = TaxIDTree_WriteAndFree(rbtnpRoot->rightChild, funPack);
		rbtnpRoot->leftChild = TaxIDTree_WriteAndFree(rbtnpRoot->leftChild, funPack);
		stpData = (SLRITaxonPtr) rbtnpRoot->data;
		DB_WriteTaxRec(stpData);
		rbtnpRoot = RBTree_NodeFree(rbtnpRoot, funPack);
	}

	return(NULL);
}
#endif /*PARSE_IN_MEMORY*/

/****************/
/*    MAIN      */
/****************/

Int2 Main()
{
	FILE *fpDmpFile=NULL;
	ByteStorePtr bspBuffer=NULL;
	Int4 size=1024;	/*initial bytestore size*/
	SLRITaxonPtr stp=NULL;
	SLRITaxonNamePtr stnp=NULL;
	SLRITaxonDivPtr stdp=NULL;
	SLRITaxonGencodePtr stgp=NULL;
	Int4 taxId=0, newTaxId=0, oldTaxId=0, taxIdCount=0, nameCount=0, genCodeCount=0, divCount=0, delNodeCount=0, mergedNodeCount=0;
	Boolean error=FALSE;
	RedBlackTreeFunctionPackagePtr funPack=NULL;
	RedBlackTreeNodePtr rbtnpRoot=NULL, rbtnpTmp=NULL;

#ifdef LOG
	time_t				time_now;
	CharPtr				TimeNowStr;
#endif

	Boolean isUpdate = FALSE;

	if (! GetArgs("importtaxdb",NUMARGS,myargs))
		return 1;

#ifdef LOG
	/* Set up LogFile - uses NCBI Error logging */
	if(!ErrSetLogfile (LOG_FILE_NAME, ELOG_APPEND)) {
		ErrPostEx(SEV_INFO, 0, 0, "Unable to open log file: %s.", LOG_FILE_NAME);
		exit(1);
	}

	ErrSetOptFlags(EO_LOGTO_USRFILE);	/*log to file*/
	ErrSetOptFlags(EO_LOG_FILELINE);	/*debug only*/
	ErrSetLogLevel(SEV_INFO);		/*log most important things*/
	/*print time and date*/
	time_now = GetSecs();
	TimeNowStr = ctime(&time_now);
	TimeNowStr[24] = '\0'; 
	ErrLogPrintf("=========================================\n");
	ErrLogPrintf("Program start at %s\n", IsNullStr(TimeNowStr));
#endif
	ErrSetMessageLevel(SEV_MAX);		/*never print to stdout*/
	ErrSetFatalLevel(SEV_MAX);			/*never abort*/

	printf("Parsing the taxonomy database.\n");

	OpenTaxDB(TRUE);

	if (myargs[0].intvalue)
	isUpdate = TRUE;

	if (!StartTransactionTax()) {
		ErrPostEx(SEV_ERROR, 0, 0, "Cannot start transaction!");
		return 1;
	}

	if (isUpdate) {
		/* delete all exisiting records */
		if (!DeleteAllRecordsInTAX()) {
			ErrPostEx(SEV_ERROR,0,0, "Cannot delete all records from TAX tables!");
			if (!RollBackTransactionTax()) {
				ErrPostEx(SEV_ERROR,0,0, "Cannot roll back!");
				return 1;
			}
			CloseTaxDB();
			return 1;
		}
	}

	/*initialize bytestore*/
	bspBuffer = BSNew(size);
	if(bspBuffer==NULL) {
		ErrPostEx(SEV_INFO, 0, 0, "Main: BSNew failed.");
		return(1);
	}

#ifdef PARSE_IN_MEMORY
	/*initialize tree to store SLRITaxon objects - with TaxID's as the node*/
	funPack = RBTree_FunPack_New((RBTree_CompareFunc) RBTree_CompareIntKeyFunction, (RBTree_KeyFreeFunc) RBTree_IntKeyFree,
								   (RBTree_DataFreeFunc) TaxIDTree_DataFree);
#endif /*PARSE_IN_MEMORY*/

	/*parse nodes dump file - create SLRI-taxon objects that have dummy names*/
	if((fpDmpFile = FileOpen(NODESFILE, "r"))==NULL) {
		ErrPostEx(SEV_INFO, 0, 0, "Main: Unable to open file %s.", NODESFILE);
		return(1);
	}
	while(BSGetTaxRecordFromFile(bspBuffer, fpDmpFile, LINEEND) != EOF) {
		/*edit bspBuffer so that the LINEEND is a FIELDEND*/
		BSSeek(bspBuffer, 0L, SEEK_END);
		BSPutByte(bspBuffer, '\t');
		BSSeek(bspBuffer, 0L, SEEK_SET);
		stp = Parse_TaxDBNodeRecord(bspBuffer);
		if(stp!=NULL) {
#ifdef PARSE_IN_MEMORY
			/*insert into tree*/
			RBTree_Add(&rbtnpRoot, (Pointer) RBTree_IntSave(stp->taxId), (Pointer) stp, funPack);
#else
			DB_WriteTaxRec(stp);
			stp = SLRITaxonFree(stp);
#endif /*PARSE_IN_MEMORY*/
			taxIdCount++;
		}
	}
	FileClose(fpDmpFile);

	/*go through entire database again and build children node list*/
#ifdef PARSE_IN_MEMORY
	TaxIDTree_BuildChildrenNodeList(rbtnpRoot, funPack);
#else
	TaxIDDisk_BuildChildrenNodeList();
#endif /*PARSE_IN_MEMORY*/

	/*parse names dump file - add information to SLRI-taxon-name objects*/
	if((fpDmpFile = FileOpen(NAMESFILE, "r"))==NULL) {
		ErrPostEx(SEV_INFO, 0, 0, "Main: Unable to open file %s.", NAMESFILE);
		return(1);
	}
	while(BSGetTaxRecordFromFile(bspBuffer, fpDmpFile, LINEEND) != EOF) {
		/*edit bspBuffer so that the LINEEND is a FIELDEND*/
		BSSeek(bspBuffer, 0L, SEEK_END);
		BSPutByte(bspBuffer, '\t');
		BSSeek(bspBuffer, 0L, SEEK_SET);
		stnp = Parse_TaxDBNameRecord(bspBuffer, &taxId);
		/*replace temp name in SLRI-taxon*/
#ifndef PARSE_IN_MEMORY
		stp = DB_GetTaxRec(taxId);
#else
		rbtnpTmp = RBTree_Find(rbtnpRoot, &taxId, funPack);
		if(rbtnpTmp!=NULL) {
			stp = (SLRITaxonPtr) rbtnpTmp->data;
		}
#endif /*PARSE_IN_MEMORY*/
		if(stp!=NULL) {
			/*if this record has a 'temp' name, remove it*/
			if((stp->names != NULL)&&(StringCmp(stp->names->name, "temp235")==0)) {
				stp->names = SLRITaxonNameFree(stp->names);
			}
			C_SLRITaxon_AddTaxName(stp, stnp);
#ifndef PARSE_IN_MEMORY
			DB_ReplaceTaxRec(stp);
			stp = SLRITaxonFree(stp);
#endif /*PARSE_IN_MEMORY*/
		}
		nameCount++;
	}
	FileClose(fpDmpFile);

#ifdef PARSE_IN_MEMORY
	/*free TaxID tree and write to disk*/
	TaxIDTree_WriteAndFree(rbtnpRoot, funPack);
	funPack = RBTree_FunPack_Free(funPack);
#endif /*PARSE_IN_MEMORY*/

	/*parse division dump file - create SLRI-taxon-div objects*/
	if((fpDmpFile = FileOpen(DIVISIONFILE, "r"))==NULL) {
		ErrPostEx(SEV_INFO, 0, 0, "Main: Unable to open file %s.", DIVISIONFILE);
		return(1);
	}
	while(BSGetTaxRecordFromFile(bspBuffer, fpDmpFile, LINEEND) != EOF) {
		/*edit bspBuffer so that the LINEEND is a FIELDEND*/
		BSSeek(bspBuffer, 0L, SEEK_END);
		BSPutByte(bspBuffer, '\t');
		BSSeek(bspBuffer, 0L, SEEK_SET);
		stdp = Parse_TaxDBDivRecord(bspBuffer);
		DB_WriteTaxDivRec(stdp);
		stdp = SLRITaxonDivFree(stdp);
		divCount++;
	}
	FileClose(fpDmpFile);

	/*parse gencode dump file - create SLRI-taxon-gencode objects*/
	if((fpDmpFile = FileOpen(GENCODEFILE, "r"))==NULL) {
		ErrPostEx(SEV_INFO, 0, 0, "Main: Unable to open file %s.", GENCODEFILE);
		return(1);
	}
	while(BSGetTaxRecordFromFile(bspBuffer, fpDmpFile, LINEEND) != EOF) {
		/*edit bspBuffer so that the LINEEND is a FIELDEND*/
		BSSeek(bspBuffer, 0L, SEEK_END);
		BSPutByte(bspBuffer, '\t');
		BSSeek(bspBuffer, 0L, SEEK_SET);
		stgp = Parse_TaxDBGenCodeRecord(bspBuffer);
		DB_WriteTaxGenCodeRec(stgp);
		stgp = SLRITaxonGencodeFree(stgp);
		genCodeCount++;
	}
	FileClose(fpDmpFile);

	/*parse delnodes dump file*/
	if((fpDmpFile = FileOpen(DELNODESFILE, "r"))==NULL) {
		ErrPostEx(SEV_INFO, 0, 0, "Main: Unable to open file %s.", DELNODESFILE);
		return(1);
	}
	while(BSGetTaxRecordFromFile(bspBuffer, fpDmpFile, LINEEND) != EOF) {
		/*edit bspBuffer so that the LINEEND is a FIELDEND*/
		BSSeek(bspBuffer, 0L, SEEK_END);
		BSPutByte(bspBuffer, '\t');
		BSSeek(bspBuffer, 0L, SEEK_SET);
		taxId = Parse_IntegerFromBSP(bspBuffer, FIELDEND, &error);
		if(error) {
			ErrPostEx(SEV_INFO, 0, 0, "Main: taxId not parsed from %s.", DELNODESFILE);
		}
		DB_WriteTaxDelNodeRec(taxId);
		delNodeCount++;
	}
	FileClose(fpDmpFile);

	/*parse merged node dump file*/
	if((fpDmpFile = FileOpen(MERGEDNODESFILE, "r"))==NULL) {
		ErrPostEx(SEV_INFO, 0, 0, "Main: Unable to open file %s.", MERGEDNODESFILE);
		return(1);
	}
	while(BSGetTaxRecordFromFile(bspBuffer, fpDmpFile, LINEEND) != EOF) {
		/*edit bspBuffer so that the LINEEND is a FIELDEND*/
		BSSeek(bspBuffer, 0L, SEEK_END);
		BSPutByte(bspBuffer, '\t');
		BSSeek(bspBuffer, 0L, SEEK_SET);
		oldTaxId = Parse_IntegerFromBSP(bspBuffer, FIELDEND, &error);
		if(error) {
			ErrPostEx(SEV_INFO, 0, 0, "Main: taxId not parsed from %s.", MERGEDNODESFILE);
		}
		newTaxId = Parse_IntegerFromBSP(bspBuffer, FIELDEND, &error);
		if(error) {
			ErrPostEx(SEV_INFO, 0, 0, "Main: taxId not parsed from %s.", MERGEDNODESFILE);
		}
		DB_WriteTaxMergedNodeRec(oldTaxId, newTaxId);
		mergedNodeCount++;
	}
	FileClose(fpDmpFile);

	bspBuffer = BSFree(bspBuffer);

	if (!CommitTransactionTax()) {
		ErrPostEx(SEV_ERROR, 0, 0, "Cannot commit transaction!");
		if (!RollBackTransactionTax()) {
			ErrPostEx(SEV_ERROR,0,0, "Cannot roll back!");
			CloseTaxDB();
			return 1;
		}
		CloseTaxDB();
		return 1;
	}

	/*output parsed record count to log*/
	ErrLogPrintf("Number of Tax ID records parsed: %ld\n", taxIdCount);
	ErrLogPrintf("Number of Tax ID Name records parsed: %ld\n", nameCount);
	ErrLogPrintf("Number of Division records parsed: %ld\n", divCount);
	ErrLogPrintf("Number of Genetic Code records parsed: %ld\n", genCodeCount);
	ErrLogPrintf("Number of Deleted Node records parsed: %ld\n", delNodeCount);
	ErrLogPrintf("Number of Merged Node records parsed: %ld\n", mergedNodeCount);

#ifndef PARSE_IN_MEMORY
	/*pack db and memo*/
	DB_PackAllTaxDB();
#endif /*PARSE_IN_MEMORY*/

	CloseTaxDB();

	printf("Done.\n");

#ifdef LOG
	time_now = GetSecs();
	TimeNowStr = ctime(&time_now);
	TimeNowStr[24] = '\0';
	ErrLogPrintf("Program end at %s\n", IsNullStr(TimeNowStr));
#endif

	return(0);
}

/*
$Log: importtaxdb.c,v $
Revision 1.4  2004/10/01 17:36:53  zwang
Added ability of building a transactional database.
Added input parameter to specify if it's an update.
See bug 3441.

Revision 1.3  2004/07/09 19:27:13  vgu
make CODE4 flag conditional on codebase flag

Revision 1.2  2004/01/06 22:32:17  skuznets
Code audit has been done sucessfully.
Possible NULL pointer issues has been eliminated.

Revision 1.1.1.2  2001/11/22 09:22:10  root
Automated import from Sourceforge.net

Revision 1.2  2001/11/21 23:52:00  kaca
database opening controled by CREATE Boolean

Revision 1.1.1.1  2001/05/25 21:36:24  kaca
initial import

Revision 1.5  2001/03/20 16:25:22  kaca
added cb declaration

Revision 1.4  2001/02/23 21:20:11  kaca
license change

Revision 1.3  2000/09/19 23:44:42  kaca
added CVS Id and Log keywords

*/

