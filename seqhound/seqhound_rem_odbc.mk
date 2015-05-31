###############################################################
#  Main configuration file for seqhound remote                #
###############################################################
ODBCLIB = -lodbc

# ODBC specific options imported by seqhound.mk
EXTRAOPT_ODBC = -I$(ODBC)/include -L$(ODBC)/lib

DEF_ODBC = -DODBC

#
# Database specific source files
#

SEQH_SRC_REM_ODBC = seqhoundapi.c intrez_misc.c objslristruc.c objNBlastasn.c seq_util.c 
SEQH_OBJ_REM_ODBC = seqhoundapi.oodbc intrez_misc.oodbc objslristruc.oodbc objNBlastasn.oodbc seq_util.oodbc

