-- -----------------------------------------------------------
-- To run (command line): db2 -tvf createSeqHoundTables.ddl 
-- Database Name	: SEQHOUND         
-- Database Schema	: SEQHOUND          
-- Creator		: Ruth Isserlin
-- Date   		: August 17,2001 
-- -----------------------------------------------------------

CONNECT TO SEQHOUND ;

---------------------------------
-- DDL statements for schema SEQHOUND
---------------------------------

DROP TABLE SEQHOUND.ASNDB;
DROP TABLE SEQHOUND.PARTI;
DROP TABLE SEQHOUND.TAXGI;
DROP TABLE SEQHOUND.NUCPROT;
DROP TABLE SEQHOUND.ACCDB;
DROP TABLE SEQHOUND.PUBSEQ;
DROP TABLE SEQHOUND.SENGI;
DROP TABLE SEQHOUND.SENDB;
DROP TABLE SEQHOUND.GO_PARENT;
DROP TABLE SEQHOUND.GO_REFERENCE;
DROP TABLE SEQHOUND.GO_SYNONYM;
DROP TABLE SEQHOUND.GO_NAME;
DROP TABLE SEQHOUND.OMIMDB;
DROP TABLE SEQHOUND.LLDB;
DROP TABLE SEQHOUND.CDDDB;
DROP TABLE SEQHOUND.GODB;
DROP TABLE SEQHOUND.gcodedb;
DROP TABLE SEQHOUND.mergedtaxdb;
DROP TABLE SEQHOUND.deltaxdb;
DROP TABLE SEQHOUND.MMDB;
DROP TABLE SEQHOUND.MMGI;
DROP TABLE SEQHOUND.TAXDB;
DROP TABLE SEQHOUND.DIVDB;
DROP TABLE SEQHOUND.CHROM;
DROP TABLE SEQHOUND.REDUND;

DROP SCHEMA SEQHOUND RESTRICT;

CREATE SCHEMA SEQHOUND;

SET CURRENT SCHEMA SEQHOUND;

------------------------------------------------
-- DDL Statements for table SEQHOUND.mergedtaxDB
------------------------------------------------
 
 CREATE TABLE SEQHOUND.mergedtaxdb  
	(
		  old_taxid INTEGER NOT NULL PRIMARY KEY, 
		  new_taxid INTEGER NOT NULL
        )
        IN SEQHOUND_DATA
	; 
------------------------------------------------
-- DDL Statements for table SEQHOUND.deltaxDB
------------------------------------------------

 CREATE TABLE SEQHOUND.deltaxdb  
	(
		  del_taxid INTEGER NOT NULL PRIMARY KEY
	)
        IN SEQHOUND_DATA
	; 
------------------------------------------------
-- DDL Statements for table SEQHOUND.taxDB
------------------------------------------------
 
 CREATE TABLE SEQHOUND.taxdb  
	(
		  taxid INTEGER NOT NULL Primary KEY,
                  ASN1 BLOB(200K) NOT LOGGED 
	)   
        IN TAXDB_DATA_TBSP
	LONG IN TAXDB_LONG_TBSP
	INDEX IN TAXDB_INDX_TBSP
	; 

------------------------------------------------
-- DDL Statements for table SEQHOUND.divDB
------------------------------------------------
  
 CREATE TABLE SEQHOUND.divdb  
	(
		  divid INTEGER NOT NULL PRIMARY KEY,
                  ASN1 BLOB(200K) NOT LOGGED 
	)   
	IN DIVDB_DATA_TBSP
	LONG IN DIVDB_LONG_TBSP
	INDEX IN DIVDB_INDX_TBSP
	; 
 
------------------------------------------------
-- DDL Statements for table SEQHOUND.gcodeDB
------------------------------------------------
 
 CREATE TABLE SEQHOUND.gcodedb  
	(
		  gcodeid INTEGER NOT NULL PRIMARY KEY,
                  ASN1 BLOB(200K) NOT LOGGED 
	)   
	IN GCODEDB_DATA_TBSP
	LONG IN GCODEDB_LONG_TBSP
	INDEX IN GCODEDB_INDX_TBSP
	; 
------------------------------------------------
-- DDL Statements for table SEQHOUND.OMIMDB
------------------------------------------------
CREATE TABLE SEQHOUND.OMIMDB
	(
		GI INTEGER NOT NULL,
		OMIM_ID INTEGER NOT NULL
	)
	IN SEQHOUND_DATA
	;


------------------------------------------------
-- DDL Statements for table SEQHOUND.LLDB
------------------------------------------------
CREATE TABLE SEQHOUND.LLDB
	(
		GI INTEGER NOT NULL,
		LOCUSLINK_ID INTEGER NOT NULL,
		MAP VARCHAR(1024) 
	)
	IN SEQHOUND_DATA
	;


------------------------------------------------
-- DDL Statements for table SEQHOUND.CDDDB
------------------------------------------------
CREATE TABLE SEQHOUND.CDDDB
	(
		GI INTEGER NOT NULL,
		CDD_ID VARCHAR(50) NOT NULL,
		EVALUE DECIMAL(20,10) 
	)
	IN SEQHOUND_DATA
	;


------------------------------------------------
-- DDL Statements for table SEQHOUND.GODB
------------------------------------------------
CREATE TABLE SEQHOUND.GODB
	(
		GI INTEGER NOT NULL,
		GO_ID INTEGER NOT NULL,
		PMID INTEGER NOT NULL,
		EVIDENCE VARCHAR(50)
	)
	IN SEQHOUND_DATA
	;

------------------------------------------------
-- DDL Statements for table SEQHOUND.GO_PARENT
------------------------------------------------
CREATE TABLE SEQHOUND.GO_PARENT
	(
		GO_ID INTEGER NOT NULL,
		PARENT_GOID INTEGER NOT NULL
	)
	IN SEQHOUND_DATA
	;

------------------------------------------------
-- DDL Statements for table SEQHOUND.GO_SYNONYM
------------------------------------------------
CREATE TABLE SEQHOUND.GO_SYNONYM
	(
		GO_ID INTEGER NOT NULL,
		GO_SYN VARCHAR(1024)
	)
	IN SEQHOUND_DATA
	;

------------------------------------------------
-- DDL Statements for table SEQHOUND.GO_REFERENCE
------------------------------------------------
CREATE TABLE SEQHOUND.GO_REFERENCE
	(
		GO_ID INTEGER NOT NULL,
		GO_REF VARCHAR(1024),
		GO_REF_DB VARCHAR(10)
        )
	IN SEQHOUND_DATA
	;

------------------------------------------------
-- DDL Statements for table SEQHOUND.GO_NAME
------------------------------------------------
CREATE TABLE SEQHOUND.GO_NAME
	(
		GO_ID INTEGER NOT NULL ,
		GO_NAME VARCHAR(1024) NOT NULL,
		GO_DB   INTEGER,
		GO_LEVEL INTEGER
	)
	IN SEQHOUND_DATA
	;

------------------------------------------------
-- DDL Statements for table SEQHOUND .NUCPROT
------------------------------------------------
 
 CREATE TABLE SEQHOUND.NUCPROT  
	(
		  GIA INTEGER NOT NULL, 
		  GIN INTEGER NOT NULL
	 )   
  	IN SEQHOUND_DATA
	; 

COMMENT ON COLUMN SEQHOUND.NUCPROT.GIA IS 'DNA GI';
COMMENT ON COLUMN SEQHOUND.NUCPROT.GIN IS 'Protein GI';

------------------------------------------------
-- DDL Statements for table SEQHOUND .PARTI
------------------------------------------------
 
 CREATE TABLE SEQHOUND.PARTI  
	(
		  GI INTEGER NOT NULL, 
		  DIVISION CHAR(10) 
	)   
	IN SEQHOUND_DATA
	; 

COMMENT ON COLUMN SEQHOUND.PARTI.DIVISION IS 'Which Database is this GI stored in';
 

------------------------------------------------
-- DDL Statements for table SEQHOUND.ACCDB
------------------------------------------------
 
 CREATE TABLE SEQHOUND.ACCDB  
	(
		  GI        INTEGER NOT NULL, 
		  DB        CHAR(15) NOT NULL, 
		  NAME      CHAR(30),
	          ACCESS    CHAR(20) NOT NULL,
	          CHAIN     CHAR(10),
		  RELEASE   CHAR(20),
		  VERSION   INTEGER,
		  TITLE     VARCHAR(1024)
	)
	IN ACCDB_DATA_TBSP
	INDEX IN ACCDB_INDX_TBSP
        ; 

COMMENT ON COLUMN SEQHOUND.ACCDB.GI IS 		'';
COMMENT ON COLUMN SEQHOUND.ACCDB.DB IS 		'Database Name';
COMMENT ON COLUMN SEQHOUND.ACCDB.NAME IS 	'Seq name';
COMMENT ON COLUMN SEQHOUND.ACCDB.ACCESS IS 	'GenBank Accession';
COMMENT ON COLUMN SEQHOUND.ACCDB.CHAIN IS 	'PDB Chain';
COMMENT ON COLUMN SEQHOUND.ACCDB.RELEASE IS 	'release in other database';
COMMENT ON COLUMN SEQHOUND.ACCDB.VERSION IS 	'Accession Version';
COMMENT ON COLUMN SEQHOUND.ACCDB.TITLE IS 	'Title in seqdescr';

------------------------------------------------
-- DDL Statements for table SEQHOUND.TAXGI
------------------------------------------------
 
 CREATE TABLE SEQHOUND.TAXGI  
	(
		  GI       INTEGER NOT NULL, 
		  TAXID   INTEGER NOT NULL,
		  KLOODGE INTEGER,
	  	  TYPE     CHAR(50)
	)   
        IN TAXGI_DATA_TBSP
	INDEX IN TAXGI_INDX_TBSP
	; 

------------------------------------------------
-- DDL Statements for table SEQHOUND.ASNDB
------------------------------------------------
 
 CREATE TABLE SEQHOUND.ASNDB  
	(
		  GI            INTEGER NOT NULL,
                  ASN1          BLOB(15M) NOT LOGGED, 
		  DIVISION      CHAR(10),
		  RELEASE       CHAR(10),
	          TYPE          CHAR(15)
	)   
	IN ASNDB_DATA_TBSP
	LONG IN ASNDB_LONG_TBSP
	INDEX IN ASNDB_INDX_TBSP
	; 

COMMENT ON COLUMN SEQHOUND.ASNDB.GI IS '';
COMMENT ON COLUMN SEQHOUND.ASNDB.DIVISION IS 'Genbank Division';
COMMENT ON COLUMN SEQHOUND.ASNDB.RELEASE IS '';
COMMENT ON COLUMN SEQHOUND.ASNDB.TYPE IS 'Nucleic Acid or Protein';

------------------------------------------------
-- DDL Statements for table SEQHOUND.PUBSEQ
------------------------------------------------
 
 CREATE TABLE SEQHOUND.PUBSEQ 
	(
		  GI        INTEGER NOT NULL, 
		  MUID      INTEGER NOT NULL
	)
	IN SEQHOUND_DATA
        ; 

------------------------------------------------
-- DDL Statements for table SEQHOUND.SENGI
------------------------------------------------
 
 CREATE TABLE SEQHOUND.SENGI  
	(
		  GI     INTEGER NOT NULL ,
                  SEID INTEGER NOT NULL,
                  DIVISION CHAR(10)
	)
	IN SEQHOUND_DATA
	; 
------------------------------------------------
-- DDL Statements for table SEQHOUND.SENDB
------------------------------------------------
 
 CREATE TABLE SEQHOUND.SENDB  
	(
		  SEID          INTEGER NOT NULL, 
		  ASN1          BLOB(15M) NOT LOGGED
	)  
	IN SENDB_DATA_TBSP
	LONG IN SENDB_LONG_TBSP
	INDEX IN SENDB_INDX_TBSP
	;  

------------------------------------------------
-- DDL Statements for table SEQHOUND.MMDB
------------------------------------------------

create table seqhound.mmdb (
    	mmdbid INTEGER NOT NULL PRIMARY KEY, 
	asn1 BLOB(10M) NOT LOGGED, 
        pdbid VARCHAR(20),
	bwhat INTEGER,
	models INTEGER,
	molecules INTEGER,
	size INTEGER,
	bzsize INTEGER
	)
	IN MMDB_DATA_TBSP
	LONG IN MMDB_LONG_TBSP
        INDEX IN MMDB_INDX_TBSP
	;

------------------------------------------------
-- DDL Statements for table SEQHOUND.MMGI
------------------------------------------------
 create table seqhound.mmgi (
    	gi INTEGER NOT NULL, 
    	mmdbid INTEGER NOT NULL,
	Primary KEY (gi, mmdbid)  
	)
	IN SEQHOUND_DATA
	;    
-------------------------------------------------
-- DDL Statements for table SEQHOUND.CHROM
-------------------------------------------------
 CREATE TABLE SEQHOUND.CHROM (
	TAXONID INTEGER NOT NULL PRIMARY KEY,
	CHROMID INTEGER,
	CHROMFLAG INTEGER,
	MOLECULETYPE INTEGER 
	)
	IN SEQHOUND_DATA
	;
--------------------------------------------------
-- DDL Statements for table SEQHOUND.REDUND
--------------------------------------------------
  CREATE TABLE SEQHOUND.REDUND (
	GI INTEGER NOT NULL ,
	ODINAL INTEGER,
	GROUP INTEGER
	)
	IN SEQHOUND_DATA
	;
--------------------------------------------------

COMMIT WORK;

-- CONNECT RESET;

-- TERMINATE;
