--###################################################################
-- 
-- dbxref_goa updated script, run MONTHLY
-- create database and datatables for dbxrefgoa cron job
-- run this manually first before run cron job
-- run xrf_abbs_parser.pl after this to populate goa_xdb table
--
--###################################################################


DROP DATABASE IF EXISTS dbxrefgoa;
create database dbxrefgoa;

-- choose database
use dbxrefgoa;



######################################################################
#
# XREF tables
#`
######################################################################
DROP TABLE IF EXISTS dbxref;
CREATE TABLE dbxref ( 
    id          INTEGER NOT NULL AUTO_INCREMENT, 
    source_id   INTEGER NOT NULL DEFAULT 0, 
    record_id   CHAR(30) NOT NULL, 
    parent_id   INTEGER UNSIGNED NOT NULL DEFAULT 0, 
    link        INTEGER DEFAULT NULL, 
    field       CHAR(20) DEFAULT NULL, 
    cv          INTEGER DEFAULT NULL, 
    lastupdate	TIMESTAMP NOT NULL,
    PRIMARY KEY (source_id, record_id, parent_id),
    INDEX id_idx 	(id),
    INDEX source_id_idx	(source_id),
    INDEX dbxref_id_idx	(record_id), 
    INDEX parent_id_idx	(parent_id),
    INDEX link_idx	(link),
    INDEX field_idx	(field),
    INDEX cv_idx	(cv),
    INDEX lastupdate_idx(lastupdate)
);
# DROP TABLE IF EXISTS dbxref;
#CREATE TABLE dbxref ( id INTEGER NOT NULL AUTO_INCREMENT, source_id   INTEGER NOT NULL DEFAULT 0, record_id CHAR(30) NOT NULL, parent_id INTEGER UNSIGNED NOT NULL DEFAULT 0, link INTEGER DEFAULT NULL, field CHAR(20) DEFAULT NULL, cv INTEGER DEFAULT NULL, lastupdate TIMESTAMP NOT NULL, PRIMARY KEY (source_id, record_id, parent_id), INDEX id_idx (id), INDEX source_id_idx (source_id), INDEX dbxref_id_idx (record_id), INDEX parent_id_idx (parent_id), INDEX link_idx (link), INDEX field_idx (field), INDEX cv_idx (cv), INDEX lastupdate_idx (lastupdate) );


DROP TABLE IF EXISTS temp_dbxref;
CREATE TABLE temp_dbxref ( 
    id          INTEGER NOT NULL AUTO_INCREMENT, 
    source_id   INTEGER NOT NULL DEFAULT 0, 
    record_id   CHAR(30) NOT NULL, 
    parent_id   INTEGER UNSIGNED NOT NULL DEFAULT 0, 
    link        INTEGER DEFAULT NULL, 
    field       CHAR(20) DEFAULT NULL, 
    cv          INTEGER DEFAULT NULL, 
    lastupdate	TIMESTAMP NOT NULL,
    PRIMARY KEY (source_id, record_id, parent_id),
    INDEX id_idx 	(id),
    INDEX source_id_idx	(source_id), 
    INDEX dbxref_id_idx	(record_id), 
    INDEX parent_id_idx	(parent_id),
    INDEX link_idx	(link),
    INDEX field_idx	(field),
    INDEX cv_idx	(cv),
    INDEX lastupdate_idx(lastupdate)
);
# DROP TABLE IF EXISTS temp_dbxref;
#CREATE TABLE temp_dbxref ( id INTEGER NOT NULL AUTO_INCREMENT, source_id   INTEGER NOT NULL DEFAULT 0, record_id CHAR(30) NOT NULL, parent_id INTEGER UNSIGNED NOT NULL DEFAULT 0, link INTEGER DEFAULT NULL, field CHAR(20) DEFAULT NULL, cv INTEGER DEFAULT NULL, lastupdate TIMESTAMP NOT NULL, PRIMARY KEY (source_id, record_id, parent_id), INDEX id_idx (id), INDEX source_id_idx (source_id), INDEX dbxref_id_idx (record_id), INDEX parent_id_idx (parent_id), INDEX link_idx (link), INDEX field_idx (field), INDEX cv_idx (cv), INDEX lastupdate_idx (lastupdate) );




DROP TABLE IF EXISTS dbxrefsourcedb;
CREATE TABLE dbxrefsourcedb( 
    source_id INT NOT NULL, 
    source_db CHAR(50) NOT NULL, 
    isprimary_db TINYINT  NOT NULL,
	lastupdate TIMESTAMP NOT NULL,
    PRIMARY KEY(source_id),
	CONSTRAINT UNIQUE (source_db),
    INDEX source_id_idx (source_id), 
    INDEX source_db_idx (source_db),
	INDEX lastupdate_idx (lastupdate)
);

#CREATE TABLE dbxrefsourcedb( source_id INT NOT NULL, source_db CHAR(50) NOT NULL, isprimary_db TINYINT  NOT NULL, lastupdate TIMESTAMP NOT NULL, PRIMARY KEY(source_id), CONSTRAINT UNIQUE (source_db), INDEX source_id_idx (source_id), INDEX source_db_idx (source_db), INDEX lastupdate_idx (lastupdate) );


-- insert db abbreviations into dbxrefsourcedb line by line
-- /* to do multi-line comment, need to remove ";" for the following insert commands first
insert into dbxrefsourcedb values(1, 'SP', 1, NULL);
insert into dbxrefsourcedb values(2, 'GB', 0, NULL);
insert into dbxrefsourcedb values(3, 'PFAM', 0, NULL);
insert into dbxrefsourcedb values(4, 'INTERPRO', 0, NULL);
insert into dbxrefsourcedb values(5, 'MGI', 1, NULL);
insert into dbxrefsourcedb values(6, 'SGD', 1, NULL);
insert into dbxrefsourcedb values(7, 'SMART', 0, NULL);
insert into dbxrefsourcedb values(8, 'ZFIN', 1, NULL);
insert into dbxrefsourcedb values(9, 'FB', 1, NULL);
insert into dbxrefsourcedb values(10, 'CG', 0, NULL);
insert into dbxrefsourcedb values(11, 'TR', 1, NULL);
insert into dbxrefsourcedb values(12, 'SPTR', 1, NULL);
insert into dbxrefsourcedb values(13, 'WB', 1, NULL);
insert into dbxrefsourcedb values(14, 'LL', 0, NULL);
insert into dbxrefsourcedb values(15, 'CGEN', 0, NULL);
insert into dbxrefsourcedb values(16, 'TIGR_ATH', 1, NULL);
insert into dbxrefsourcedb values(18, 'GENEDB_SPOMBE', 1, NULL);
insert into dbxrefsourcedb values(17, 'REFSEQ', 0, NULL);
insert into dbxrefsourcedb values(19, 'DDB', 1, NULL);
insert into dbxrefsourcedb values(20, 'GR', 1, NULL); #gramene
insert into dbxrefsourcedb values(21, 'TAIR', 1, NULL);
insert into dbxrefsourcedb values(22, 'TIGR_CMR', 1, NULL);
insert into dbxrefsourcedb values(23, 'UNIGENE', 1, NULL); 
insert into dbxrefsourcedb values(24, 'VIDA', 1, NULL); 
insert into dbxrefsourcedb values(25, 'RGD', 1, NULL);
insert into dbxrefsourcedb values(26, 'IPI', 1, NULL);
insert into dbxrefsourcedb values(27, 'ENSEMBL', 1, NULL);
insert into dbxrefsourcedb values(28, 'AFCS', 1, NULL);
insert into dbxrefsourcedb values(29, 'HUGO', 0, NULL);
insert into dbxrefsourcedb values(30, 'OMIM', 0, NULL);
insert into dbxrefsourcedb values(31, 'PIR', 0, NULL);
insert into dbxrefsourcedb values(32, 'GENE', 1, NULL);
-- */ 

-- or use batch import
-- LOAD DATA INFILE "./dbxrefsourcedb.txt" INTO TABLE dbxrefsourcedb;

####################################################################### 
#
# GENE ASSOCIATION tables
#
####################################################################### 


DROP TABLE IF EXISTS goa_seq_dbxref;
CREATE TABLE goa_seq_dbxref ( 
        id INTEGER UNSIGNED NOT NULL AUTO_INCREMENT, 
        goa_xdb_id INTEGER UNSIGNED NOT NULL, 
        xref_key VARCHAR(30) NOT NULL, 
        taxid_1 INTEGER NOT NULL, 
        taxid_2 INTEGER DEFAULT 0, 
        type_id VARCHAR(20) NOT NULL, 
        symbol VARCHAR(30) NOT NULL, 
        full_name VARCHAR(255) DEFAULT NULL, 
        synonym VARCHAR(50) DEFAULT NULL, 
        lastupdate TIMESTAMP NOT NULL,
	INDEX id_idx(id),
	PRIMARY KEY (goa_xdb_id, xref_key), 
        Index goa_xdb_id_idx	(goa_xdb_id),
        Index xref_key_idx	(xref_key), 
	Index taxid_1_idx	(taxid_1),
        Index symbol_idx	(symbol),
	Index synonym_idx	(synonym),
        Index lastupdate_idx	(lastupdate)
);

#CREATE TABLE goa_seq_dbxref ( id INTEGER UNSIGNED NOT NULL AUTO_INCREMENT, goa_xdb_id INTEGER UNSIGNED NOT NULL, xref_key VARCHAR(30) NOT NULL, taxid_1 INTEGER NOT NULL, taxid_2 INTEGER DEFAULT 0, type_id VARCHAR(20) NOT NULL, symbol VARCHAR(30) NOT NULL, full_name VARCHAR(255) DEFAULT NULL, synonym VARCHAR(50) DEFAULT NULL, lastupdate TIMESTAMP NOT NULL, INDEX id_idx(id), PRIMARY KEY (goa_xdb_id, xref_key), Index goa_xdb_id_idx(goa_xdb_id), Index xref_key_idx(xref_key), Index taxid_1_idx(taxid_1), Index symbol_idx(symbol), Index synonym_idx(synonym), Index lastupdate_idx(lastupdate) );



DROP TABLE IF EXISTS goa_association;
CREATE TABLE goa_association (
		id INTEGER UNSIGNED NOT NULL PRIMARY KEY AUTO_INCREMENT, 
		is_not CHAR(1) DEFAULT NULL,
		go_id INTEGER UNSIGNED NOT NULL, 
		goa_seq_dbxref_id INTEGER UNSIGNED NOT NULL, 
		assigned_by INTEGER UNSIGNED NOT NULL,
		code CHAR(4) NOT NULL,
		date CHAR(8) DEFAULT NULL,
	        lastupdate TIMESTAMP NOT NULL,
		CONSTRAINT UNIQUE (go_id, goa_seq_dbxref_id, code ),
       		INDEX goa_id_idx	(go_id),
		INDEX goa_seq_dbxref_id_idx(goa_seq_dbxref_id),
		INDEX assigned_by_idx	(assigned_by),
	        INDEX code_idx		(code),
		INDEX lastupdate_idx	(lastupdate)
);

#CREATE TABLE goa_association (id INTEGER UNSIGNED NOT NULL PRIMARY KEY AUTO_INCREMENT, is_not CHAR(1) DEFAULT NULL, go_id INTEGER UNSIGNED NOT NULL, goa_seq_dbxref_id INTEGER UNSIGNED NOT NULL, assigned_by INTEGER UNSIGNED NOT NULL, code CHAR(4) NOT NULL, date CHAR(8) DEFAULT NULL, lastupdate TIMESTAMP NOT NULL, CONSTRAINT UNIQUE (go_id, goa_seq_dbxref_id, code ), INDEX goa_id_idx(go_id), INDEX goa_seq_dbxref_id_idx(goa_seq_dbxref_id), INDEX assigned_by_idx(assigned_by), INDEX code_idx(code), INDEX lastupdate_idx(lastupdate) );


DROP TABLE IF EXISTS goa_reference;
CREATE TABLE goa_reference (
		id INTEGER UNSIGNED NOT NULL PRIMARY KEY AUTO_INCREMENT, 
		goa_association_id INTEGER UNSIGNED NOT NULL,
		goa_xdb_id INTEGER UNSIGNED NOT NULL,
		xref_key VARCHAR(20) NOT NULL, 
	    	lastupdate TIMESTAMP NOT NULL,
		Index goa_association_id_idx(goa_association_id),
		Index goa_xdb_id_idx	(goa_xdb_id),
		Index xref_key_idx	(xref_key),
	    	Index lastupdate_idx	(lastupdate)

);
#CREATE TABLE goa_reference ( id INTEGER UNSIGNED NOT NULL PRIMARY KEY AUTO_INCREMENT, goa_association_id INTEGER UNSIGNED NOT NULL, goa_xdb_id INTEGER UNSIGNED NOT NULL, xref_key VARCHAR(20) NOT NULL, lastupdate TIMESTAMP NOT NULL, Index goa_association_id_idx(goa_association_id), Index goa_xdb_id_idx(goa_xdb_id), Index xref_key_idx(xref_key), Index lastupdate_idx(lastupdate) );

DROP TABLE IF EXISTS goa_with;
CREATE TABLE goa_with (
		id INTEGER UNSIGNED NOT NULL PRIMARY KEY AUTO_INCREMENT,
		goa_association_id INTEGER UNSIGNED NOT NULL, 
		goa_xdb_id INTEGER UNSIGNED NOT NULL,
		xref_key VARCHAR(20) NOT NULL, 
		key_type INTEGER UNSIGNED NOT NULL,
		goa_seq_dbxref_id INTEGER UNSIGNED NOT NULL, 
		lastupdate TIMESTAMP NOT NULL,
		INDEX goa_association_id_idx(goa_association_id), 
		INDEX goa_xdb_id_idx	(goa_xdb_id), 
		INDEX xref_key_idx	(xref_key), 
	   	INDEX key_type_idx	(key_type), 
		INDEX goa_seq_dbxref_id_idx(goa_seq_dbxref_id), 
		INDEX lastupdate_idx	(lastupdate)
);

#CREATE TABLE goa_with ( id INTEGER UNSIGNED NOT NULL PRIMARY KEY AUTO_INCREMENT, goa_association_id INTEGER UNSIGNED NOT NULL, goa_xdb_id INTEGER UNSIGNED NOT NULL, xref_key VARCHAR(20) NOT NULL, key_type INTEGER UNSIGNED NOT NULL, goa_seq_dbxref_id INTEGER UNSIGNED NOT NULL, lastupdate TIMESTAMP NOT NULL, INDEX goa_association_id_idx(goa_association_id), INDEX goa_xdb_id_idx(goa_xdb_id), INDEX xref_key_idx(xref_key), INDEX key_type_idx(key_type), INDEX goa_seq_dbxref_id_idx(goa_seq_dbxref_id), INDEX lastupdate_idx(lastupdate) );


DROP TABLE IF EXISTS goa_xdb;
CREATE TABLE goa_xdb (
		id INTEGER NOT NULL PRIMARY KEY AUTO_INCREMENT,
		abbreviation VARCHAR(50) NOT NULL UNIQUE,
		name VARCHAR(255) DEFAULT NULL,
		object VARCHAR(255) DEFAULT NULL,
		example VARCHAR(50) DEFAULT NULL,
		generic_url VARCHAR(255) DEFAULT NULL,
		url_syntax VARCHAR(255) DEFAULT NULL,
		url_example VARCHAR(255) DEFAULT NULL,
		lastupdate TIMESTAMP NOT NULL,
		INDEX abbreviation_idx	(abbreviation),
		INDEX name_idx		(name),
		INDEX lastupdate_idx	(lastupdate)
);

#CREATE TABLE goa_xdb ( id INTEGER NOT NULL PRIMARY KEY AUTO_INCREMENT, abbreviation VARCHAR(50) NOT NULL UNIQUE, name VARCHAR(255) DEFAULT NULL, object VARCHAR(255) DEFAULT NULL, example VARCHAR(50) DEFAULT NULL, generic_url VARCHAR(255) DEFAULT NULL, url_syntax VARCHAR(255) DEFAULT NULL, url_example VARCHAR(255) DEFAULT NULL, lastupdate TIMESTAMP NOT NULL, INDEX abbreviation_idx (abbreviation), INDEX name_idx (name), INDEX lastupdate_idx (lastupdate));

############################################################################
#Temporaray insert statements for goa_xdb. May be removed later if we find
#a better way to handle them

--Load data infile @workdir'/goa_xdb.txt' into table goa_xdb;


############################################################################
# goa_gigo table that contain gi-go pairs.

DROP TABLE IF EXISTS goa_gigo;
CREATE TABLE goa_gigo ( 
	gi INTEGER NOT NULL, 
	go INTEGER NOT NULL, 
	code CHAR(4) DEFAULT NULL, 
	xdb_id INTEGER DEFAULT 0, 
	xref_key VARCHAR(30) DEFAULT NULL,
	lastupdate TIMESTAMP NOT NULL,
	PRIMARY KEY		(gi, go, code), 
	INDEX gi_idx		(gi),
	INDEX go_idx		(go),
	INDEX code_idx          (code),
	INDEX xdb_id_idx	(xdb_id),
	INDEX xref_key_idx	(xref_key),
	INDEX lastupdate_idx	(lastupdate)
);

#CREATE TABLE goa_gigo ( gi INTEGER NOT NULL, go INTEGER NOT NULL, code CHAR(4) DEFAULT NULL, xdb_id INTEGER DEFAULT 0, xref_key VARCHAR(30) DEFAULT NULL, lastupdate TIMESTAMP NOT NULL, PRIMARY KEY (gi, go, code), INDEX gi_idx (gi), INDEX go_idx (go), INDEX xdb_id_idx (xdb_id), INDEX lastupdate_idx (lastupdate), INDEX xref_key_idx (xref_key) );
