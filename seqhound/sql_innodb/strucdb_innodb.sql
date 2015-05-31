# $Id: strucdb_innodb.sql,v 1.2 2005/03/31 21:51:12 zwang Exp $


#------------------------------------------------------------------------
#---
#--- in file intrez_odbc.c
#---
#------------------------------------------------------------------------

####################################
###
### strudb module
###
####################################
DROP TABLE IF EXISTS mmdb;
DROP TABLE IF EXISTS mmgi;
DROP TABLE IF EXISTS domdb;

CREATE TABLE mmdb (rowid INTEGER UNSIGNED NOT NULL AUTO_INCREMENT,
                    ts TIMESTAMP NOT NULL,
                    mmdbid INTEGER NOT NULL,
                    asn1 MEDIUMBLOB NOT NULL, 
                    pdbid VARCHAR(20) NOT NULL, 
                    bwhat INTEGER,
                    models INTEGER,
                    molecules INTEGER, 
                    size INTEGER,
                    bzsize INTEGER,
                    PRIMARY KEY (mmdbid),
                    INDEX immdb_rowid(rowid),
                    INDEX immdb_ts(ts),
                    INDEX immdb_mmdbid(mmdbid),
                    INDEX immdb_pdbid(pdbid)) TYPE=INNODB;
CREATE TABLE mmgi (rowid INTEGER UNSIGNED NOT NULL AUTO_INCREMENT,
                    ts TIMESTAMP NOT NULL,
                    mmdbid INTEGER NOT NULL, 
                    gi INTEGER NOT NULL, 
                    PRIMARY KEY (mmdbid, gi),
                    INDEX immgi_rowid(rowid),
                    INDEX immgi_ts(ts),
                    INDEX immgi_mmdbid(mmdbid), 
                    INDEX immgi_gi(gi)) TYPE=INNODB;
CREATE TABLE domdb (rowid INTEGER UNSIGNED NOT NULL AUTO_INCREMENT,
                    ts TIMESTAMP NOT NULL,
                    mmdbid INTEGER NOT NULL, 
                    asn1 MEDIUMBLOB NOT NULL, 
                    pdbid VARCHAR(20) NOT NULL, 
                    pdbchain VARCHAR(10), 
                    gi INTEGER NOT NULL, 
                    domno INTEGER, 
                    domall INTEGER NOT NULL, 
                    domid INTEGER NOT NULL, 
                    rep INTEGER, 
                    PRIMARY KEY (domid),
                    INDEX idomdb_rowid(rowid),
                    INDEX idomdb_ts(ts),
                    INDEX idomdb_mmdbid(mmdbid), 
                    INDEX idomdb_pdbid(pdbid), 
                    INDEX idomdb_gi(gi), 
                    INDEX idomdb_domall(domall), 
                    INDEX idomdb_domid(domid)) TYPE=INNODB;


# $Log: strucdb_innodb.sql,v $
# Revision 1.2  2005/03/31 21:51:12  zwang
# Change field name timestamp to ts
#
# Revision 1.1  2005/02/11 19:22:24  zwang
# New files for creating tables in InnoDB.
#
# Revision 1.2  2004/08/09 16:01:34  zwang
# New file to create tables.
#
