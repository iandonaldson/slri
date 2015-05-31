# $Id: rps.sql,v 1.2 2004/08/09 16:01:34 zwang Exp $

#use testsql;

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
CREATE TABLE rpsdb (rowid INTEGER NOT NULL AUTO_INCREMENT,
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
                    PRIMARY KEY (gi, cddid, rfrom, align_len),
                    INDEX irps_rowid(rowid),
                    INDEX irps_gi(gi),
                    INDEX irps_cddid(cddid),
                    INDEX irps_domid(domid),                    
                    INDEX irps_len(align_len),
                    INDEX irps_numdom(numdom),
                    INDEX irps_gi_e(gi, evalue));
CREATE TABLE domname (rowid INTEGER NOT NULL AUTO_INCREMENT,
                    accession CHAR(15) NOT NULL,
                    name CHAR(25) NOT NULL,
                    pdbid CHAR(12) NOT NULL,
                    asn1 MEDIUMBLOB NOT NULL,
                    INDEX idom_rowid(rowid),
                    INDEX idom_acc(accession),
                    INDEX idom_name(name),
                    INDEX idom_pdbid(pdbid));


# $Log: rps.sql,v $
# Revision 1.2  2004/08/09 16:01:34  zwang
# New file to create tables.
#
