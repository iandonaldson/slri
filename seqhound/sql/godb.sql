# $Id: godb.sql,v 1.4 2004/10/01 19:38:21 zwang Exp $

#use testsql;

#------------------------------------------------------------------------
#---
#--- in file go_odbc.c
#---
#------------------------------------------------------------------------
####################################
###
### go module
###
####################################
DROP TABLE IF EXISTS go_parent;
DROP TABLE IF EXISTS go_name;
DROP TABLE IF EXISTS go_reference;
DROP TABLE IF EXISTS go_synonym;

CREATE TABLE go_parent (rowid INTEGER UNSIGNED NOT NULL AUTO_INCREMENT,
                        datetime DATETIME NOT NULL,
                        go_id INTEGER NOT NULL,
                        parent_goid INTEGER NOT NULL,
                        PRIMARY KEY (go_id, parent_goid),
                        INDEX igoparent_rowid(rowid),
                        INDEX igoparent_goid(go_id),
                        INDEX igoparent_pid(parent_goid)) TYPE=INNODB;
CREATE TABLE go_name (rowid INTEGER UNSIGNED NOT NULL AUTO_INCREMENT,
                        datetime DATETIME NOT NULL,
                        go_id INTEGER NOT NULL,
                        go_name TEXT NOT NULL,
                        go_db INTEGER NOT NULL,
                        go_level INTEGER,
                        PRIMARY KEY (go_id, go_name(100), go_db, go_level),
                        INDEX igoname_rowid(rowid),
                        INDEX igoname_goid(go_id)) TYPE=INNODB;
CREATE TABLE go_reference (rowid INTEGER UNSIGNED NOT NULL AUTO_INCREMENT,
                        datetime DATETIME NOT NULL,
                        go_id INTEGER NOT NULL,
                        go_ref TEXT NOT NULL,
                        go_ref_db CHAR(50),
                        PRIMARY KEY(go_id, go_ref(100)),
                        INDEX igoref_rowid(rowid),
                        INDEX igoref_go_ref_db(go_ref_db)) TYPE=INNODB;
CREATE TABLE go_synonym (rowid INTEGER UNSIGNED NOT NULL AUTO_INCREMENT,
                        datetime DATETIME NOT NULL,
                        go_id INTEGER NOT NULL,
                        go_syn TEXT NOT NULL,
                        PRIMARY KEY (go_id, go_syn(100)),
                        INDEX igosynonym_rowid(rowid),
                        INDEX igosynonym_go_id(go_id)) TYPE=INNODB;


# $Log: godb.sql,v $
# Revision 1.4  2004/10/01 19:38:21  zwang
# Change timestamp to datetime.
#
# Revision 1.3  2004/10/01 17:45:33  zwang
# Added field 'timestamp' to every table. Change table type to INNODB.
#
# Revision 1.2  2004/08/09 16:01:34  zwang
# New file to create tables.
#
