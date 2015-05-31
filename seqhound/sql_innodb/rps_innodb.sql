# $Id: rps_innodb.sql,v 1.2 2005/03/31 21:51:12 zwang Exp $


#------------------------------------------------------------------------
#---
#--- in file rps_odbc.c
#---
#------------------------------------------------------------------------
####################################
###
### rpsdb module
###
####################################
DROP TABLE IF EXISTS rpsdb;
DROP TABLE IF EXISTS domname;

# use 'rfrom' instead of 'from'
CREATE TABLE rpsdb (rowid INTEGER UNSIGNED NOT NULL,
                    ts TIMESTAMP NOT NULL,
                    gi INTEGER NOT NULL,
                    cddid INTEGER NOT NULL,
                    domid CHAR(12) NOT NULL,
                    rfrom INTEGER NOT NULL,
                    align_len INTEGER NOT NULL,
                    score INTEGER NOT NULL,
                    evalue DECIMAL(15, 8),
                    bitscore DECIMAL(15, 8),
                    missing_n INTEGER NOT NULL,
                    missing_c INTEGER NOT NULL,
                    numdom INTEGER NOT NULL,
                    INDEX irps_rowid(rowid),
                    INDEX irps_ts(ts),
                    INDEX irps_gi(gi),
                    INDEX irps_cddid(cddid),
                    INDEX irps_domid(domid),                    
                    INDEX irps_len(align_len),
                    INDEX irps_numdom(numdom),
                    INDEX irps_e(evalue),
                    INDEX irps_gi_e(gi, evalue)) TYPE=INNODB;

CREATE TABLE domname (rowid INTEGER UNSIGNED NOT NULL AUTO_INCREMENT,
                    ts TIMESTAMP NOT NULL,
                    accession CHAR(15) NOT NULL,
                    name CHAR(25) NOT NULL,
                    pdbid CHAR(12) NOT NULL,
                    asn1 MEDIUMBLOB NOT NULL,
                    INDEX idom_rowid(rowid),
                    INDEX idom_ts(ts),
                    INDEX idom_acc(accession),
                    INDEX idom_name(name),
                    INDEX idom_pdbid(pdbid)) TYPE=INNODB;


# $Log: rps_innodb.sql,v $
# Revision 1.2  2005/03/31 21:51:12  zwang
# Change field name timestamp to ts
#
# Revision 1.1  2005/02/11 19:22:24  zwang
# New files for creating tables in InnoDB.
#
# Revision 1.2  2004/08/09 16:01:34  zwang
# New file to create tables.
#
