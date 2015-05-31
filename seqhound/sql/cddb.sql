# $Id: cddb.sql,v 1.2 2004/08/09 16:01:33 zwang Exp $

#use testsql;

#------------------------------------------------------------------------
#---
#--- in file intrez_odbc.c
#---
#------------------------------------------------------------------------

####################################
###
### cddb module
###
####################################
DROP TABLE IF EXISTS cddb;

CREATE TABLE cddb (rowid INTEGER NOT NULL AUTO_INCREMENT,
                    gi INTEGER NOT NULL, 
                    asn1 MEDIUMBLOB NOT NULL, 
                    INDEX icddb_rowid(rowid), 
                    INDEX icddb_gi(gi));


# $Log: cddb.sql,v $
# Revision 1.2  2004/08/09 16:01:33  zwang
# New file to create tables.
#
