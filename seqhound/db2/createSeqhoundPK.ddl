-- -----------------------------------------------------------
-- To run (command line): db2 -tvf createSeqHoundPK 
-- Database Name	: SEQHOUND         
-- Database Schema	: SEQHOUND          
-- Creator		: Ruth Isserlin
-- Date   		: June 27, 2001
-- -----------------------------------------------------------
-- Script to add primary keys to the tables in seqhound created
-- by the mother parser
-- -----------------------------------------------------------

CONNECT TO SEQHOUND;

------------------------------------------------
-- Add the Paraemeters that need to be increased to 
-- allow for the additional indexes
------------------------------------------------

------------------------------------------------
-- DDL Statements for table SEQHOUND .NUCPROT
------------------------------------------------
 
 ALTER TABLE SEQHOUND.NUCPROT ADD PRIMARY KEY (GIA, GIN);

------------------------------------------------
-- PK Statements for table SEQHOUND .PARTITION
------------------------------------------------
 
 ALTER TABLE SEQHOUND.PARTI ADD PRIMARY KEY (GI); 

------------------------------------------------
-- PK Statements for table SEQHOUND.ACCDB
------------------------------------------------
 
 ALTER TABLE SEQHOUND.ACCDB ADD PRIMARY KEY(GI, DB, ACCESS);

------------------------------------------------
-- PK Statements for table SEQHOUND.TAXGI
------------------------------------------------

 ALTER TABLE SEQHOUND.TAXGI ADD PRIMARY KEY (GI,TAXID);

------------------------------------------------
-- PK Statements for table SEQHOUND.ASNDB
------------------------------------------------
 
 ALTER TABLE SEQHOUND.ASNDB ADD PRIMARY KEY(GI);

------------------------------------------------
-- PK Statements for table SEQHOUND.PUBSEQ
------------------------------------------------

--no primary keys

------------------------------------------------
-- PK Statements for table SEQHOUND.MMGI
------------------------------------------------

 ALTER TABLE SEQHOUND.SENGI ADD PRIMARY KEY(GI,SEID);
 
------------------------------------------------
-- PK Statements for table SEQHOUND.SENDB
------------------------------------------------
 
 ALTER TABLE SEQHOUND.SENDB ADD PRIMARY KEY (SEID);

------------------------------------------------
-- Add additional indexes for fast retrieval 
------------------------------------------------
CREATE INDEX seqhound.pubseq_gi_ind ON SEQHOUND.PUBSEQ(GI);
CREATE INDEX seqhound.pubseq_muid_ind ON SEQHOUND.PUBSEQ(MUID);

CREATE INDEX seqhound.taxgi_taxid_ind ON SEQHOUND.TAXGI(taxid);
CREATE INDEX seqhound.taxgi_gi_ind ON SEQHOUND.TAXGI(gi);

CREATE INDEX seqhound.accdb_access_ind ON SEQHOUND.ACCDB(access);
CREATE INDEX seqhound.accdb_gi_ind ON SEQHOUND.ACCDB(gi);
CREATE INDEX seqhound.accdb_name_ind ON seqhound.accdb(name);

CREATE INDEX  seqhound.sengi_gi_ind ON SEQHOUND.SENGI(gi);
CREATE INDEX  seqhound.sengi_seid_ind ON SEQHOUND.SENGI(seid);

CREATE INDEX  seqhound.nucprot_gia_ind ON SEQHOUND.NUCPROT(gia);
CREATE INDEX  seqhound.nucprot_gin_ind ON SEQHOUND.NUCPROT(gin);

--CREATE INDEX  ON SEQHOUND.CHROM(taxonid);
--CREATE INDEX  ON SEQHOUND.CHROM(chromid);

Create index seqhound.redund_gi on seqhound.redund(gi);
Create index seqhound.redund_ord on seqhound.redund(ordinal);
Create index seqhound.redund_group on seqhound.redund(group);

Create index seqhound.godb_gi on seqhound.godb(gi);
Create index seqhound.godb_goid on seqhound.godb(go_id);
Create index seqhound.godb_goid on seqhound.godb(go_id);

Create index seqhound.lldb_gi on seqhound.lldb(gi);
Create index seqhound.lldb_llid on seqhound.lldb(locuslink_id);

Create index seqhound.omimdb_gi on seqhound.omimdb(gi);
Create index seqhound.omimdb_omimid on seqhound.godb(omim_id);

Create index seqhound.goname_goid on seqhound.go_name(go_id);
Create index seqhound.goname_level on seqhound.go_name(level);
Create index seqhound.goname_db on seqhound.go_name(db);

Create index seqhound.gosyn_goid on seqhound.go_synonym(go_id);

Create index seqhound.goref_goid on seqhound.go_reference(go_id);

Create index seqhound.goparent_goid on seqhound.go_parent(go_id);
Create index seqhound.goparent_goid on seqhound.go_parent(go_id);


COMMIT WORK;

-- CONNECT RESET;

-- TERMINATE;
