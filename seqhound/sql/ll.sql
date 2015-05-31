# $Id: ll.sql,v 1.2 2004/08/09 16:01:34 zwang Exp $

#use testsql;


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

CREATE TABLE ll_omim (rowid INTEGER NOT NULL AUTO_INCREMENT,
                      ll_id INTEGER NOT NULL,
                      omim_id INTEGER NOT NULL,
                      PRIMARY KEY (ll_id, omim_id),
                      INDEX illomim_rowid(rowid),
                      INDEX illomim_llid(ll_id),
                      INDEX illomim_omimid(omim_id));
CREATE TABLE ll_go (rowid INTEGER NOT NULL AUTO_INCREMENT,
                      ll_id INTEGER NOT NULL,
                      go_id INTEGER,
                      pmid INTEGER,
                      evidence VARCHAR(50),
                      INDEX illgo_rowid(rowid),
                      INDEX illgo_llid(ll_id),
                      INDEX illgo_goid(go_id),
                      INDEX illgo_pmid(pmid));
CREATE TABLE ll_llink (rowid INTEGER NOT NULL AUTO_INCREMENT,
                      ll_id INTEGER NOT NULL,
                      gi INTEGER NOT NULL,
                      map TEXT,
                      PRIMARY KEY (ll_id, gi),
                      INDEX illll_rowid(rowid),
                      INDEX illll_llid(ll_id), 
                      INDEX illll_gi(gi));
CREATE TABLE ll_cdd (rowid INTEGER NOT NULL AUTO_INCREMENT,
                      ll_id INTEGER NOT NULL,
                      cdd_id VARCHAR(50),
                      evalue DECIMAL(20, 10),
                      INDEX illcdd_rowid(rowid),
                      INDEX illcdd_llid(ll_id),
                      INDEX illcdd_cddid(cdd_id));


# $Log: ll.sql,v $
# Revision 1.2  2004/08/09 16:01:34  zwang
# New file to create tables.
#
