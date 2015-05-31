# $Id: ll_innodb.sql,v 1.2 2005/03/31 21:51:12 zwang Exp $


#------------------------------------------------------------------------
#---
#--- in file ll_odbc.c
#---
#------------------------------------------------------------------------
####################################
###
### ll module
###
####################################
DROP TABLE IF EXISTS ll_omim;
DROP TABLE IF EXISTS ll_go;
DROP TABLE IF EXISTS ll_llink;
DROP TABLE IF EXISTS ll_cdd;

CREATE TABLE ll_omim (rowid INTEGER UNSIGNED NOT NULL AUTO_INCREMENT,
                      ts TIMESTAMP NOT NULL,
                      ll_id INTEGER NOT NULL,
                      omim_id INTEGER NOT NULL,
                      PRIMARY KEY (ll_id, omim_id),
                      INDEX illomim_rowid(rowid),
                      INDEX illomim_ts(ts),
                      INDEX illomim_llid(ll_id),
                      INDEX illomim_omimid(omim_id)) TYPE=INNODB;
CREATE TABLE ll_go (rowid INTEGER UNSIGNED NOT NULL AUTO_INCREMENT,
                      ts TIMESTAMP NOT NULL,
                      ll_id INTEGER NOT NULL,
                      go_id INTEGER,
                      pmid INTEGER,
                      evidence VARCHAR(50),
                      INDEX illgo_rowid(rowid),
                      INDEX illgo_ts(ts),
                      INDEX illgo_llid(ll_id),
                      INDEX illgo_goid(go_id),
                      INDEX illgo_pmid(pmid)) TYPE=INNODB;
CREATE TABLE ll_llink (rowid INTEGER UNSIGNED NOT NULL AUTO_INCREMENT,
                      ts TIMESTAMP NOT NULL,
                      ll_id INTEGER NOT NULL,
                      gi INTEGER NOT NULL,
                      map TEXT,
                      PRIMARY KEY (ll_id, gi),
                      INDEX illll_rowid(rowid),
                      INDEX illll_ts(ts),
                      INDEX illll_llid(ll_id), 
                      INDEX illll_gi(gi)) TYPE=INNODB;
CREATE TABLE ll_cdd (rowid INTEGER UNSIGNED NOT NULL AUTO_INCREMENT,
                      ts TIMESTAMP NOT NULL,
                      ll_id INTEGER NOT NULL,
                      cdd_id VARCHAR(50),
                      evalue DECIMAL(20, 10),
                      INDEX illcdd_rowid(rowid),
                      INDEX illcdd_ts(ts),
                      INDEX illcdd_llid(ll_id),
                      INDEX illcdd_cddid(cdd_id)) TYPE=INNODB;


# $Log: ll_innodb.sql,v $
# Revision 1.2  2005/03/31 21:51:12  zwang
# Change field name timestamp to ts
#
# Revision 1.1  2005/02/11 19:22:24  zwang
# New files for creating tables in InnoDB.
#
# Revision 1.2  2004/08/09 16:01:34  zwang
# New file to create tables.
#
