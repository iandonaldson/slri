# $Id: redund.sql,v 1.4 2004/10/01 19:38:21 zwang Exp $

#use testsql;


#------------------------------------------------------------------------
#---
#--- in file intrez_odbc.c
#---
#------------------------------------------------------------------------

####################################
###
### redund module
###
####################################
DROP TABLE IF EXISTS redund;

# use "rordinal" instead of "ordinal"
# use "rgroup" instead of "group"
CREATE TABLE redund(rowid INTEGER UNSIGNED NOT NULL AUTO_INCREMENT,
                     datatime DATETIME NOT NULL,
                     gi INTEGER NOT NULL, 
                     rordinal INTEGER NOT NULL, 
                     rgroup INTEGER,
                     PRIMARY KEY (gi),
                     INDEX iredund_rowid(rowid),
                     INDEX iredund_gi (gi), 
                     INDEX iredund_ordinal (rordinal), 
                     INDEX iredund_rgroup (rgroup)) TYPE=INNODB;


# $Log: redund.sql,v $
# Revision 1.4  2004/10/01 19:38:21  zwang
# Change timestamp to datetime.
#
# Revision 1.3  2004/10/01 17:45:33  zwang
# Added field 'timestamp' to every table. Change table type to INNODB.
#
# Revision 1.2  2004/08/09 16:01:33  zwang
# New file to create tables.
#
