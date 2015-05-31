# $Id: taxdb_innodb.sql,v 1.2 2005/03/31 21:51:12 zwang Exp $


#------------------------------------------------------------------------
#---
#--- in file taxdb_odbc.c
#---
#------------------------------------------------------------------------
####################################
###
### taxdb module
###
####################################
DROP TABLE IF EXISTS taxdb;
DROP TABLE IF EXISTS gcodedb;
DROP TABLE IF EXISTS divdb;
DROP TABLE IF EXISTS del;
DROP TABLE IF EXISTS merge;

CREATE TABLE taxdb (rowid INTEGER UNSIGNED NOT NULL AUTO_INCREMENT,
                    ts TIMESTAMP NOT NULL,
                    tid INTEGER NOT NULL,
                    asn MEDIUMBLOB,
                    PRIMARY KEY (tid),
                    INDEX itax_rowid (rowid),
                    INDEX itax_ts (ts),
                    INDEX itax_tid (tid)) TYPE=INNODB;
CREATE TABLE gcodedb (rowid INTEGER UNSIGNED NOT NULL AUTO_INCREMENT, 
                    ts TIMESTAMP NOT NULL,
                    gcid INTEGER NOT NULL,
                    asn MEDIUMBLOB,
                    PRIMARY KEY (gcid),
                    INDEX igcode_rowid (rowid),
                    INDEX igcode_ts (ts),
                    INDEX igcode_gcid(gcid)) TYPE=INNODB;
CREATE TABLE divdb (rowid INTEGER UNSIGNED NOT NULL AUTO_INCREMENT,
                    ts TIMESTAMP NOT NULL,
                    did INTEGER NOT NULL,
                    asn MEDIUMBLOB,
                    PRIMARY KEY (did),
                    INDEX idiv_rowid (rowid),
                    INDEX idiv_ts (ts),
                    INDEX idiv_did(did)) TYPE=INNODB;
CREATE TABLE del (rowid INTEGER UNSIGNED NOT NULL AUTO_INCREMENT,
                    ts TIMESTAMP NOT NULL,
                    tid INTEGER NOT NULL,
                    PRIMARY KEY (tid),
                    INDEX idel_rowid (rowid),
                    INDEX idel_ts (ts),
                    INDEX idel_tid(tid)) TYPE=INNODB;
CREATE TABLE merge (rowid INTEGER UNSIGNED NOT NULL AUTO_INCREMENT,
                    ts TIMESTAMP NOT NULL,
                    otid INTEGER NOT NULL,
                    ntid INTEGER NOT NULL,
                    PRIMARY KEY (otid),
                    INDEX imerge_rowid (rowid),
                    INDEX imerge_ts (ts),
                    INDEX imerge_ntid(ntid),
                    INDEX imerge_otid(otid)) TYPE=INNODB;



# $Log: taxdb_innodb.sql,v $
# Revision 1.2  2005/03/31 21:51:12  zwang
# Change field name timestamp to ts
#
# Revision 1.1  2005/02/11 19:22:24  zwang
# New files for creating tables in InnoDB.
#
# Revision 1.4  2004/10/01 19:38:21  zwang
# Change timestamp to datetime.
#
# Revision 1.3  2004/10/01 17:45:33  zwang
# Added field 'timestamp' to every table. Change table type to INNODB.
#
# Revision 1.2  2004/08/09 16:01:34  zwang
# New file to create tables.
#
