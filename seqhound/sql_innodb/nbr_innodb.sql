# $Id: nbr_innodb.sql,v 1.2 2005/03/31 21:51:12 zwang Exp $


#------------------------------------------------------------------------
#---
#--- in file nbr_odbc.c
#---
#------------------------------------------------------------------------
####################################
###
### nbr module
###
####################################
DROP TABLE IF EXISTS blastdb;
DROP TABLE IF EXISTS nblastdb;

CREATE TABLE blastdb (rowid INTEGER UNSIGNED NOT NULL,
                      ts TIMESTAMP NOT NULL,
                      uid BIGINT DEFAULT NULL,
                      eval DECIMAL(12, 7) DEFAULT NULL,
                      asnsa MEDIUMBLOB,
                      INDEX iblastdb_rowid (rowid),
                      INDEX iblastdb_ts (ts),
                      INDEX iblastdb_uid (uid))
                      MAX_ROWS=700000000 AVG_ROW_LENGTH=1000 TYPE=INNODB;                     

CREATE TABLE nblastdb (rowid INTEGER UNSIGNED NOT NULL,
                      ts TIMESTAMP NOT NULL,
                      ord INTEGER NOT NULL DEFAULT 0,
                      gi INTEGER NOT NULL DEFAULT 0,
                      num INTEGER NOT NULL DEFAULT 0,
                      asnnbr MEDIUMBLOB NOT NULL,
                      INDEX inblastdb_rowid(rowid),
                      INDEX inblastdb_ts(ts),
                      INDEX inblastdb_ord(ord),
                      INDEX inblastdb_gi (gi),
                      INDEX inblastdb_num(num))
                      MAX_ROWS=20000000 AVG_ROW_LENGTH=12000 TYPE=INNODB;


# $Log: nbr_innodb.sql,v $
# Revision 1.2  2005/03/31 21:51:12  zwang
# Change field name timestamp to ts
#
# Revision 1.1  2005/02/11 19:22:24  zwang
# New files for creating tables in InnoDB.
#
# Revision 1.2  2004/08/09 16:01:34  zwang
# New file to create tables.
#
