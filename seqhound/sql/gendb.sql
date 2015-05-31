# $Id: gendb.sql,v 1.3 2004/10/15 20:23:09 eburgess Exp $

#use testsql;

#------------------------------------------------------------------------
#---
#--- in file intrez_odbc.c
#---
#------------------------------------------------------------------------

####################################
###
### gendb module
###
####################################
DROP TABLE IF EXISTS chrom;

CREATE TABLE chrom (kloodge INTEGER NOT NULL AUTO_INCREMENT,
                    taxid INTEGER NOT NULL, 
                    chromid INTEGER NOT NULL UNIQUE, 
                    chromflag INTEGER NOT NULL, 
                    access VARCHAR(20) NOT NULL UNIQUE, 
                    name TEXT NOT NULL, 
                    PRIMARY KEY (kloodge),
                    INDEX ichrom_chromid(chromid),
                    INDEX ichrom_taxid(taxid), 
                    INDEX ichrom_kl(kloodge), 
                    INDEX ichrom_acc(access) );

# $Log: gendb.sql,v $
# Revision 1.3  2004/10/15 20:23:09  eburgess
# Changed columns in chrom. Changed rowid to kloodge and kloodge to chromid
# as this column will now hold the NCBI chrom id. Bug 2952.
#
# Revision 1.2  2004/08/09 16:01:33  zwang
# New file to create tables.
#
