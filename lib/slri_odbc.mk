#################################################
# SLRILIB configuration file for odbc
###############################################

# TO CHANGE: Set this path to the unix odbc path
ODBC_LIB = $(ODBC)/lib
ODBC_INC = $(ODBC)/include
#ODBC_LIB = /software/32/unixodbc/lib
#ODBC_INC = /software/32/unixodbc/include
#ODBC_LIB = /usr/local/lib
#ODBC_INC = /usr/local/include

EXTRAOPT_ODBC = -I$(ODBC_INC) -L$(ODBC_LIB)

ARFLAGS =
DEF_ODBC = -DODBC

