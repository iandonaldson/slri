# $Id: core_innodb.sql,v 1.3 2005/03/31 21:51:12 zwang Exp $


#------------------------------------------------------------------------
#---
#--- in file intrez_odbc.c
#---
#------------------------------------------------------------------------

####################################
###
### core module
###
####################################
DROP TABLE IF EXISTS accdb;
DROP TABLE IF EXISTS asndb;
DROP TABLE IF EXISTS nucprot;
DROP TABLE IF EXISTS parti;
DROP TABLE IF EXISTS pubseq;
DROP TABLE IF EXISTS sendb;
DROP TABLE IF EXISTS sengi;
DROP TABLE IF EXISTS taxgi;
DROP TABLE IF EXISTS sourcefile;
DROP TABLE IF EXISTS bioentity;
DROP TABLE IF EXISTS bioname;
DROP TABLE IF EXISTS secondrefs;
DROP TABLE IF EXISTS bioentitytype;
DROP TABLE IF EXISTS nametype;
DROP TABLE IF EXISTS rules;
DROP TABLE IF EXISTS fieldtype;



CREATE TABLE accdb (rowid INTEGER UNSIGNED NOT NULL AUTO_INCREMENT,
                     ts TIMESTAMP NOT NULL,
                     gi INTEGER NOT NULL, 
                     db VARCHAR(15) NOT NULL, 
                     name VARCHAR(30) NOT NULL, 
                     namelow VARCHAR(30), 
                     access VARCHAR(20) NOT NULL, 
                     chain VARCHAR(20), 
                     release VARCHAR(20), 
                     version INTEGER, 
                     title TEXT,
                     PRIMARY KEY (gi, db, access),
                     INDEX iaccdb_rowid(rowid),
                     INDEX iaccdb_ts(ts),
                     INDEX iaccdb_gi(gi), 
                     INDEX iaccdb_db(db), 
                     INDEX iaccdb_name(name), 
                     INDEX iaccdb_namelow(namelow), 
                     INDEX iaccdb_acc(access))
                     MAX_ROWS=1000000000 AVG_ROW_LENGTH=200 TYPE=INNODB;
CREATE TABLE asndb (rowid INTEGER UNSIGNED NOT NULL AUTO_INCREMENT,
                     ts TIMESTAMP NOT NULL,
                     gi INTEGER NOT NULL,
                     asn1 LONGBLOB, 
                     division VARCHAR(25) NOT NULL, 
                     release VARCHAR(10) NOT NULL, 
                     type VARCHAR(15) NOT NULL,
                     PRIMARY KEY (gi),
                     INDEX iasndb_rowid(rowid),
                     INDEX iasndb_ts(ts),
                     INDEX iasndb_gi(gi))
                     MAX_ROWS=1000000000 AVG_ROW_LENGTH=4000 TYPE=INNODB;
CREATE TABLE nucprot (rowid INTEGER UNSIGNED NOT NULL AUTO_INCREMENT,
                     ts TIMESTAMP NOT NULL,
                     gin INTEGER NOT NULL, 
                     gia INTEGER NOT NULL, 
                     PRIMARY KEY (gia),
                     INDEX inucprot_rowid(rowid),
                     INDEX inucprot_ts(ts),
                     INDEX inuc_gin(gin),
                     INDEX inuc_gia(gia)) TYPE=INNODB;
CREATE TABLE parti (rowid INTEGER UNSIGNED NOT NULL AUTO_INCREMENT,
                     ts TIMESTAMP NOT NULL,
                     gi INTEGER NOT NULL,
                     division CHAR(15),
                     INDEX iparti_rowid(rowid),
                     INDEX iparti_ts(ts),
                     INDEX iparti_gi(gi), 
                     INDEX iparti_div(division)) TYPE=INNODB;
CREATE TABLE pubseq (rowid INTEGER UNSIGNED NOT NULL AUTO_INCREMENT,
                     ts TIMESTAMP NOT NULL,
                     gi INTEGER NOT NULL, 
                     muid INTEGER NOT NULL, 
                     pmid INTEGER NOT NULL,
                     PRIMARY KEY (gi, pmid),
                     INDEX ipubseq_rowid(rowid),
                     INDEX ipubseq_ts(ts),
                     INDEX ipubseq_gi(gi) , 
                     INDEX ipubseq_muid(muid), 
                     INDEX ipubseq_pmid(pmid)) TYPE=INNODB;
CREATE TABLE sendb (rowid INTEGER UNSIGNED NOT NULL AUTO_INCREMENT,
                     ts TIMESTAMP NOT NULL,
                     seid INTEGER NOT NULL, 
                     asn1 MEDIUMBLOB NOT NULL,
                     PRIMARY KEY (seid),
                     INDEX isendb_rowid(rowid),
                     INDEX isendb_ts(ts),
                     INDEX isendb_seid(seid))
                     MAX_ROWS=1000000000 AVG_ROW_LENGTH=1000 TYPE=INNODB;
CREATE TABLE sengi (rowid INTEGER UNSIGNED NOT NULL AUTO_INCREMENT,
                     ts TIMESTAMP NOT NULL,
                     gi INTEGER NOT NULL, 
                     seid INTEGER NOT NULL, 
                     division CHAR(15),
                     PRIMARY KEY (gi),
                     INDEX isengi_rowid(rowid),
                     INDEX isengi_ts(ts),
                     INDEX isengi_gi(gi), 
                     INDEX isengi_seid(seid)) TYPE=INNODB;
CREATE TABLE taxgi (rowid INTEGER UNSIGNED NOT NULL AUTO_INCREMENT,
                     ts TIMESTAMP NOT NULL,
                     gi INTEGER NOT NULL, 
                     taxid INTEGER NOT NULL, 
                     kloodge INTEGER, 
                     type VARCHAR(50), 
                     PRIMARY KEY (gi), 
                     INDEX itaxgi_rowid(rowid),
                     INDEX itaxgi_ts(ts),
                     INDEX itaxgi_gi(gi), 
                     INDEX itaxgi_taxid(taxid), 
                     INDEX itaxgi_kl(kloodge),
                     INDEX itaxgi_type(type(10))) TYPE=INNODB;
CREATE TABLE sourcefile(id INTEGER NOT NULL AUTO_INCREMENT,
			ts TIMESTAMP NOT NULL,
			gi INTEGER NOT NULL,
			filename VARCHAR(80) NOT NULL,
			date DATETIME,
			INDEX isfile_id(id),
			INDEX isfile_ts(ts),
			INDEX isfile_gi(gi),
			INDEX isfile_filename(filename),
			INDEX isfile_date(date)) TYPE=INNODB;
CREATE Table bioentity(id INTEGER NOT NULL AUTO_INCREMENT,
			ts TIMESTAMP NOT NULL,
			bioentitytypeid INTEGER NOT NULL,
			db VARCHAR(15) NOT NULL,
			access VARCHAR(20) NOT NULL UNIQUE,
			identifier INTEGER UNIQUE,
			field INTEGER NOT NULL,
			INDEX ibioe_id(id),
			INDEX ibioentity_ts(ts),
			INDEX ibioe_type(bioentitytypeid),
			INDEX ibioe_access( access ),
			INDEX ibioe_identifier(identifier)) TYPE=INNODB;
CREATE Table bioname( id INTEGER NOT NULL AUTO_INCREMENT,
			ts TIMESTAMP NOT NULL,
			bioentityid INTEGER NOT NULL,
			name  TEXT NOT NULL,
			nametypeid INTEGER NOT NULL,
			db VARCHAR(15) NOT NULL,
			access VARCHAR(20) NOT NULL,
			identifier INTEGER,
			field INTEGER NOT NULL,
			official INTEGER DEFAULT 0,
			deprecated INTEGER DEFAULT 0,
			datedeprecated  TIMESTAMP,
			ruleid INTEGER,
			action VARCHAR(1) DEFAULT 'A',
			actiondate TIMESTAMP,
			INDEX ibioname_id(id),
			INDEX ibioname_ts(ts),
			INDEX ibioname_identifier(identifier),
			INDEX ibioname_access(access),
			INDEX ibioname_name(name(100)),
			INDEX ibioname_bioentityid(bioentityid),
			INDEX ibioname_nametype(nametypeid),
			INDEX ibioname_official(official),
			INDEX ibioname_deprecated(deprecated),
			INDEX ibioname_ruleid( ruleid ),
			INDEX ibioname_action( action ),
			INDEX ibioname_actiondate( actiondate )) TYPE=INNODB;
CREATE TABLE secondrefs(id INTEGER NOT NULL AUTO_INCREMENT,
			ts TIMESTAMP NOT NULL,
			bionameid INTEGER NOT NULL,
			db VARCHAR(15) NOT NULL,
			access VARCHAR(20),
			field INTEGER NOT NULL,
			INDEX isecondrefs_id(id),
			INDEX isecondrefs_ts(ts),
			INDEX isecondrefs_bionameid( bionameid ),
			INDEX isecondrefs_dbsearch( access, db ),
			INDEX ibioname_field( field ) ) TYPE=INNODB;
CREATE Table bioentitytype(
       id INTEGER NOT NULL, 
       ts TIMESTAMP NOT NULL,
       type VARCHAR(80),
       INDEX ibioentitytype_ts(ts) ) TYPE=INNODB;
CREATE Table nametype(
       id INTEGER NOT NULL, 
       ts TIMESTAMP NOT NULL,
       type VARCHAR(80),
       INDEX inametype_ts(ts) ) TYPE=INNODB;                             
CREATE Table rules(
       id INTEGER NOT NULL, 
       ts TIMESTAMP NOT NULL,
       rule VARCHAR(80),
       INDEX irules_ts(ts) ) TYPE=INNODB;
CREATE Table fieldtype(
       id INTEGER NOT NULL, 
       ts TIMESTAMP NOT NULL,
       pathtofield MEDIUMTEXT,
       INDEX ifieldtype_ts(ts) ) TYPE=INNODB;

INSERT into bioentitytype ( id, type ) VALUES( 0, "not-specified");
INSERT into bioentitytype ( id, type ) VALUES( 1, "protein" );
INSERT into bioentitytype ( id, type ) VALUES( 2, "dna" );
INSERT into bioentitytype ( id, type ) VALUES( 3, "rna" );
INSERT into bioentitytype ( id, type ) VALUES( 4, "small-molecule" );
INSERT into bioentitytype ( id, type ) VALUES( 5, "complex" );
INSERT into bioentitytype ( id, type ) VALUES( 6, "gene" );

# Define field types here. 
#
# NOTE: field types should also be defined in seqhound.h
#
#Field types defined for the refseq asn1 structure where the name is found.

INSERT into fieldtype ( id, pathtofield ) VALUES( 0, 
	"none" );
INSERT into fieldtype ( id, pathtofield ) VALUES( 1, 
	"seqentry/seqset/bioseq" );
INSERT into fieldtype ( id, pathtofield ) VALUES( 2, 
	"seqentry/seqset/bioseq/seq-annot/seqfeat-gene/locus" );
INSERT into fieldtype ( id, pathtofield ) VALUES( 3, 
	"seqentry/seqset/bioseq/seq-annot/seqfeat-prot/name" );
INSERT into fieldtype ( id, pathtofield ) VALUES( 4, 
	"seqentry/seqset/bioseq/seq-annot/seqfeat-prot/desc" );
INSERT into fieldtype ( id, pathtofield ) VALUES( 5, 
	"seqentry/seqset/bioseq/descr-title/" );
INSERT into fieldtype ( id, pathtofield ) VALUES( 6, 
	"seqentry/seqset/bioseq/descr-pbl/compound" );
INSERT into fieldtype ( id, pathtofield ) VALUES( 7, 
	"seqentry/seqset/descr-title/" );
INSERT into fieldtype ( id, pathtofield ) VALUES( 8, 
	"seqentry/seqset/descr-pbl/compound" );
INSERT into fieldtype ( id, pathtofield ) VALUES( 9, 
	"seqentry/seqset/bioseq/seq-annot/seqfeat-gene/syn" );

#Field types defined for yeast names derived from the
#SGD_features.tab flat file.

INSERT into fieldtype ( id, pathtofield ) VALUES( 100, 
	"Column4:FeatureName" );
INSERT into fieldtype ( id, pathtofield ) VALUES( 101, 
	"Column16:Description" );
INSERT into fieldtype ( id, pathtofield ) VALUES( 102, 
	"Column6:Alias" );
INSERT into fieldtype ( id, pathtofield ) VALUES( 103, 
	"Column5:Gene" );

INSERT into nametype ( id, type ) VALUES( 1, "protein" );
INSERT into nametype ( id, type ) VALUES( 2, "gene" );

INSERT into rules ( id, rule ) VALUES( 0, "none" );
INSERT into rules ( id, rule ) VALUES( 1, "Use gene name for protein." );
INSERT into rules ( id, rule ) VALUES( 2, "Use seq-set title for protein." );



#------------------------------------------------------------------------
#--- in file hist_odbc.c
#------------------------------------------------------------------------
####################################
###
### update module
###
####################################
DROP TABLE IF EXISTS histdb;

CREATE TABLE histdb (rowid INTEGER UNSIGNED NOT NULL AUTO_INCREMENT,
                     ts TIMESTAMP NOT NULL,
                     gi INTEGER NOT NULL,
                     oldgi INTEGER,	
                     access CHAR(20) NOT NULL, 
                     version INTEGER, 
                     date DATE NOT NULL, 
                     action INTEGER NOT NULL,
                     filename VARCHAR(80) NOT NULL, 
                     INDEX ihistdb_rowid(rowid),
                     INDEX ihistdb_ts(ts),
                     INDEX ihistdb_gi(gi), 
                     INDEX ihistdb_date(date), 
                     INDEX ihistdb_action(action),
                     INDEX ihistdb_filename(filename),
                     INDEX ihistdb_oldgi(oldgi)) TYPE=INNODB;



####################################
###
### complete genomes module - temporary table used by mother.
###
### gichromosome table is used by mother to record which gis
### belong to complete genomes. At the end of the build, the data
### is moved to taxgi.
##############################################################
DROP TABLE IF EXISTS chrom;
DROP TABLE IF EXISTS gichromid;
DROP TABLE IF EXISTS contigchromid;
DROP TABLE IF EXISTS gichromosome;
DROP TABLE IF EXISTS contigchromosome;

CREATE TABLE chrom (kloodge INTEGER NOT NULL AUTO_INCREMENT,
                    ts TIMESTAMP NOT NULL,
                    taxid INTEGER NOT NULL,
                    chromid INTEGER NOT NULL UNIQUE,
                    chromflag INTEGER NOT NULL,
                    access VARCHAR(20),
                    name TEXT NOT NULL,
                    chromnum varchar(10),
                    PRIMARY KEY(kloodge),
                    INDEX ichrom_chromid(chromid),
                    INDEX ichrom_ts(ts),
                    INDEX ichrom_taxid(taxid, chromnum),
                    INDEX ichrom_kl(kloodge),
                    INDEX ichrom_acc(access),
                    INDEX ichrom_chromnum( chromnum) ) TYPE=INNODB;

CREATE TABLE gichromid( id INTEGER NOT NULL AUTO_INCREMENT,
                    	ts TIMESTAMP NOT NULL,
			gi INTEGER NOT NULL UNIQUE,
			chromid INTEGER NOT NULL, PRIMARY KEY(id),
			contiggi INTEGER,
			INDEX igichromid(gi),
                    	INDEX igits(ts),
			INDEX igicontiggi(contiggi) )  TYPE=INNODB;

CREATE TABLE contigchromid( id INTEGER NOT NULL AUTO_INCREMENT,
                ts TIMESTAMP NOT NULL,
		contiggi INTEGER NOT NULL UNIQUE,
		topgi INTEGER,
		chromid INTEGER NOT NULL,
		changed INTEGER,
		PRIMARY KEY(id),
		INDEX iccontiggi(contiggi),
                INDEX icontigts(ts),
		INDEX ictopgi( topgi ) )  TYPE=INNODB;

CREATE TABLE gichromosome( id INTEGER NOT NULL AUTO_INCREMENT,
                ts TIMESTAMP NOT NULL,
		gi INTEGER NOT NULL UNIQUE,
		chromnum VARCHAR(10) NOT NULL,
		PRIMARY KEY(id),
		INDEX igichrom_gi(gi),
                INDEX ichrom_ts(ts),
		INDEX igichrom_num(chromnum) )  TYPE=INNODB;

CREATE TABLE contigchromosome( id INTEGER NOT NULL AUTO_INCREMENT,
                ts TIMESTAMP NOT NULL,
		contiggi INTEGER NOT NULL UNIQUE,
		chromnum VARCHAR(10) NOT NULL,
		PRIMARY KEY(id),
		INDEX icontigchrom_gi(contiggi),
                INDEX icontigchrom_ts(ts),
		INDEX icontigchrom_num(chromnum) )  TYPE=INNODB;


# $Log: core_innodb.sql,v $
# Revision 1.3  2005/03/31 21:51:12  zwang
# Change field name timestamp to ts
#
# Revision 1.2  2005/03/09 17:21:49  eburgess
# Added columns oldgi and filename to histdb. Changed column acc to access for
# histdb. Bug 5049
#
# Revision 1.1  2005/02/11 19:22:24  zwang
# New files for creating tables in InnoDB.
#
# Revision 1.7  2005/02/02 19:38:48  eburgess
# Moved chrom table create statement here and added other tables for the
# new comgen module. Bug 2952
#
# Revision 1.6  2004/11/10 19:10:58  eburgess
# Added sourcefile create statement. Bug 1193
#
# Revision 1.5  2004/10/15 20:22:04  eburgess
# Added create statement for contigkloodge. Bug 2952, complete genomes.
#
# Revision 1.4  2004/09/22 18:54:33  eburgess
# Added in create statement for gikloodge, a table needed by mother when
# parsing complete genome info out of refseq.
#
# Revision 1.3  2004/09/16 15:54:25  eburgess
# Changed inserts into fieldtype table to account for the new SGD_features.tab
# file
#
# Revision 1.2  2004/08/09 16:01:33  zwang
# New file to create tables.
#
