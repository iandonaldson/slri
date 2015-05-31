################################################################
#  Main configuration file for seqhound using DB2              #
################################################################
DB2LIB = -ldb2

DB2DIR = /usr/lpp/db2_07_01

INCDIR_DB2 = $(DB2DIR)/include
LIBDIR_DB2 = $(DB2DIR)/lib
DEF_DB2 = -DDB2

#
# Database specific source files
#
SEQH_SRC_DB2  = intrez_db2.c nbr_db2.c taxdb_db2.c ll_db2.c go_db2.c rpsdb_db2.c
SEQH_OBJ_ODB2 = intrez_db2.odb2 nbr_db2.odb2 taxdb_db2.odb2 ll_db2.odb2 go_db2.odb2 rpsdb_db2.odb2
