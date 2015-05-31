#################################################
# SLRILIB configuration file for db2
###############################################

#DB2DIR =

INCDIR_DB2 = $(DB2DIR)/include
LIBDIR_DB2 = $(DB2DIR)/lib
ARFLAGS = -X 64
DEF_DB2 = -DDB2

SLRILIB_SRC_DB2  = slri_db2.c 
SLRILIB_OBJ_ODB2 = slri_db2.odb2
