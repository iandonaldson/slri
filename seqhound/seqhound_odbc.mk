################################################################
#  Main configuration file for seqhound using DB2              #
################################################################

# TO CHANGE: This is the path to the ODBC libraries & includes
#ODBCDIR = /usr/lib
#ODBCDIR = /software/32/unixodbc
ODBCLIB = -lodbc

# ODBC specific options imported by seqhound.mk
EXTRAOPT_ODBC = -I$(ODBC)/include -L$(ODBC)/lib

DEF_ODBC = -DODBC

#
# Database specific source files
#


SEQH_SRC_ODBC  = odbc_func.c intrez.c intrez_odbc.c go_odbc.c ll_odbc.c taxdb_odbc.c nbr_odbc.c \
                 rpsdb_odbc.c nblast_odbc.c hist_odbc.c dbxref_odbc.c dbxref_query.c \
                 names_odbc.c polymerfix.c comgen_odbc.c gene_odbc.c
SEQH_OBJ_OODBC = odbc_func.oodbc intrez.oodbc intrez_odbc.oodbc go_odbc.oodbc ll_odbc.oodbc taxdb_odbc.oodbc nbr_odbc.oodbc \
                 rpsdb_odbc.oodbc nblast_odbc.oodbc hist_odbc.oodbc dbxref_odbc.oodbc dbxref_query.oodbc \
                 names_odbc.oodbc polymerfix.o comgen_odbc.oodbc gene_odbc.oodbc
