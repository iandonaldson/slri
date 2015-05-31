-- choose database
use dbxrefcron;

######################################################################
#
# XREF tables
#
######################################################################
-- create tables for the above database
DROP TABLE IF EXISTS dbxref;
DROP TABLE IF EXISTS dbxrefsourcedb;

CREATE TABLE IF NOT EXISTS dbxref ( 
    id          INT NOT NULL AUTO_INCREMENT, 
    source_id   INT NOT NULL, 
    record_id   CHAR(30) NOT NULL, 
    parent_id   INT NOT NULL, 
    link        INT, 
    field       CHAR(20), 
    cv          INT, 
    PRIMARY KEY (id),
    Index i_source_id   (source_id), 
    Index i_dbxref_id   (record_id), 
    Index i_parent_id   (parent_id),
    Index i_link        (link) 
);


CREATE TABLE IF NOT EXISTS dbxrefsourcedb( 
    source_id INT NOT NULL, 
    source_db CHAR(50) NOT NULL, 
    isprimary_db TINYINT  NOT NULL,
    PRIMARY KEY(source_id),
    Index i_source_id2 (source_id), 
    Index i_source_db (source_db) 
);


-- insert db abbreviations into dbxrefsourcedb line by line
-- /* to do multi-line comment, need to remove ";" for the following insert commands first
insert into dbxrefsourcedb values(1, 'SP', 1);
insert into dbxrefsourcedb values(2, 'GB', 0);
insert into dbxrefsourcedb values(3, 'PFAM', 0);
insert into dbxrefsourcedb values(4, 'INTERPRO', 0);
insert into dbxrefsourcedb values(5, 'MGI', 1);
insert into dbxrefsourcedb values(6, 'SGD', 1);
insert into dbxrefsourcedb values(7, 'SMART', 0);
insert into dbxrefsourcedb values(8, 'ZFIN', 1);
insert into dbxrefsourcedb values(9, 'FB', 1);
insert into dbxrefsourcedb values(10, 'CG', 0);
insert into dbxrefsourcedb values(11, 'TR', 1);
insert into dbxrefsourcedb values(12, 'SPTR', 1);
insert into dbxrefsourcedb values(13, 'WB', 1);
insert into dbxrefsourcedb values(14, 'LL', 0);
insert into dbxrefsourcedb values(15, 'CGEN', 0);
insert into dbxrefsourcedb values(16, 'TIGR_ATH', 1 );
insert into dbxrefsourcedb values(18, 'GENEDB_SPOMBE', 1);
insert into dbxrefsourcedb values(17, 'REFSEQ', 0 );
insert into dbxrefsourcedb values(19, 'DDB', 1 );
insert into dbxrefsourcedb values(20, 'GR', 1 ); #gramene
insert into dbxrefsourcedb values(21, 'TAIR', 1 );
insert into dbxrefsourcedb values(22, 'TIGR_CMR', 1 );
insert into dbxrefsourcedb values(23, 'UNIGENE', 1 ); 
insert into dbxrefsourcedb values(24, 'VIDA', 1 ); 
insert into dbxrefsourcedb values(25, 'RGD', 1 );
insert into dbxrefsourcedb values(26, 'IPI', 1);
insert into dbxrefsourcedb values(27, 'ENSEMBL', 1);
-- */ 

-- or use batch import
-- LOAD DATA INFILE "./dbxrefsourcedb.txt" INTO TABLE dbxrefsourcedb;

####################################################################### 
#
# GENE ASSOCIATION tables
#
####################################################################### 
CREATE TABLE goa_xdb (
		id INTEGER NOT NULL PRIMARY KEY AUTO_INCREMENT, 
		abbreviation VARCHAR(50), name VARCHAR(255), 
		object VARCHAR(255), 
		example VARCHAR(50), 
		generic_url VARCHAR(255), 
		url_syntax VARCHAR(255), 
		url_example VARCHAR(255) 
);
CREATE TABLE goa_seq_dbxref ( 
		id INTEGER UNSIGNED NOT NULL PRIMARY KEY AUTO_INCREMENT, 
		goa_xdb_id INTEGER UNSIGNED NOT NULL, 
		xref_key VARCHAR(30),
		taxid_1 INTEGER NOT NULL,
		taxid_2 INTEGER, 
		type_id VARCHAR(20) NOT NULL, 
		symbol VARCHAR(30) NOT NULL, 
		full_name VARCHAR(255), 
		synonym VARCHAR(50), 
		Index i_xref_key(xref_key), 
		Index symbol_idx(symbol)
);
CREATE TABLE goa_association (
		id INTEGER UNSIGNED NOT NULL PRIMARY KEY AUTO_INCREMENT, 
		is_not CHAR(1), 
		go_id INTEGER UNSIGNED NOT NULL, 
		goa_seq_dbxref_id INTEGER UNSIGNED NOT NULL, 
		assigned_by INTEGER UNSIGNED NOT NULL,
		code CHAR(4) NOT NULL,
		date CHAR(8), 
		CONSTRAINT UNIQUE (go_id, goa_seq_dbxref_id, code ),
		Index goa_seq_dbxref_id_idx(goa_seq_dbxref_id),
		Index assigned_by_idx(assigned_by)
);
CREATE TABLE goa_reference (
		id INTEGER UNSIGNED NOT NULL PRIMARY KEY AUTO_INCREMENT, 
		goa_association_id INTEGER UNSIGNED NOT NULL,
		goa_xdb_id INTEGER UNSIGNED NOT NULL,
		xref_key VARCHAR(20) NOT NULL, 
		Index goa_association_id_idx(goa_association_id),
		Index goa_xdb_id_idx(goa_xdb_id)
);
CREATE TABLE goa_with (
		id INTEGER UNSIGNED NOT NULL PRIMARY KEY AUTO_INCREMENT,
		goa_association_id INTEGER UNSIGNED NOT NULL, 
		goa_xdb_id INTEGER UNSIGNED NOT NULL,
		xref_key VARCHAR(20) NOT NULL, 
		key_type INTEGER UNSIGNED NOT NULL,
		goa_seq_dbxref_id INTEGER UNSIGNED NOT NULL, 
		Index goa_association_id_idx(goa_association_id), 
		Index goa_xdb_id_idx(goa_xdb_id) 
);

############################################################################
#Temporaray insert statements for goa_xdb. May be removed later if we find
#a better way to handle them
INSERT INTO goa_xdb VALUES(NULL, "GOA", "Gene Ontology Association", "identifier", "GO:123456", "http://www.ebi.ac.uk/GOA/", NULL, NULL );						   
INSERT INTO goa_xdb VALUES(NULL, "PINC", NULL, NULL, NULL, NULL, NULL, NULL );          
INSERT INTO goa_xdb VALUES(NULL, "DB", "Dummy Database", NULL, NULL, NULL, NULL, NULL );          
INSERT INTO goa_xdb VALUES(NULL, "RefSeq", "NCBI RefSeq", NULL, NULL, NULL, NULL, NULL );          
							

#Load data infile '/home/kzheng/ll_go/goa_xdb.txt' into table goa_xdb;



