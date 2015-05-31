# $Id: nbr.sql,v 1.2 2004/08/09 16:01:34 zwang Exp $

#use testsql;


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

CREATE TABLE blastdb (rowid INTEGER UNSIGNED NOT NULL AUTO_INCREMENT,
                      uid BIGINT NOT NULL,
                      eval DECIMAL(12, 7) NOT NULL,
                      asnsa MEDIUMBLOB NOT NULL,
                      PRIMARY KEY (uid),
                      INDEX iblastdb_rowid (rowid),
                      INDEX iblastdb_uid (uid))
                      MAX_ROWS=700000000 AVG_ROW_LENGTH=1000;                     
CREATE TABLE nblastdb (rowid INTEGER NOT NULL AUTO_INCREMENT,
                      ord INTEGER NOT NULL,
                      gi INTEGER NOT NULL,
                      num INTEGER NOT NULL,
                      asnnbr MEDIUMBLOB NOT NULL,
                      PRIMARY KEY (ord),
                      INDEX inblastdb_rowid(rowid),
                      INDEX inblastdb_ord(ord),
                      INDEX inblastdb_gi (gi),
                      INDEX inblastdb_num(num))
                      MAX_ROWS=20000000 AVG_ROW_LENGTH=12000;


# $Log: nbr.sql,v $
# Revision 1.2  2004/08/09 16:01:34  zwang
# New file to create tables.
#
