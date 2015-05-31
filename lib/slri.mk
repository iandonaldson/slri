#################################################
# SLRILIB make dependencies
###############################################
include $(NCBI)/ncbi.mk
include slri_db2.mk
include slri_cb.mk
include slri_odbc.mk

##################
## COMMON PATHS ##
##################
SLRIINC = ./
SLRIDIR = ./
SLRILIB     = -lslri
SLRILIB_CB  = -lslri_cb
SLRILIB_DB2 = -lslri_db2
SLRILIB_ODBC = -lslri_odbc
SLRIMAKE = make.slrilib

# unix library names
LIB     = libslri.a
LIB_CB  = libslri_cb.a
LIB_DB2 = libslri_db2.a
LIB_ODBC = libslri_odbc.a
#
#BZDIR for cross compilation build
BZDIR = ../../bzip2
#BZDIR for seqhound developer compilation
#BZDIR = /software/64/bzip2
BZINC = $(BZDIR)
BZLIB = -lbz
BZMAKE = make.bzlib

SLRI_INCDIR = $(SLRIINC) -I$(BZINC)
BUILDDIR = ../build

EXTRAOPT = -D_FILE_OFFSET_BITS=64
OTHERLIBS = $(NCBI_OTHERLIBS)
ARFLAGS =

##################
## MAKE SECTION ##
##################   
.SUFFIXES: .c .h .o .ocb .odb2 .oodbc
## default and new database-specific implicit actions
.c.o :
	$(CC) $(CFLAGS) $*.c -o $*.o
.c.ocb :
	$(CC) $(CFLAGS_CB) $<
	mv $*.o $*.ocb
.c.odb2 :
	$(CC) $(CFLAGS_DB2) $<
	mv $*.o $*.odb2
.c.oodbc :
	$(CC) $(CFLAGS_ODBC) $<
	mv $*.o $*.oodbc

SLRILIB_SRC_COM = slri_asn.c slri_cluster.c slri_cookie.c slri_dagtree.c slri_endec.c slri_mail.c slri_math.c slri_misc.c slri_prime.c \
	slri_rbtree.c slri_set.c slri_sstack.c slri_stat.c slri_www.c \
	objslrilinkset.c objslrilibstruc.c 
SLRILIB_OBJ_COM = slri_asn.o slri_cluster.o slri_cookie.o slri_dagtree.o slri_endec.o slri_mail.o slri_math.o slri_misc.o slri_prime.o \
	slri_rbtree.o slri_set.o slri_sstack.o slri_stat.o slri_www.o \
	objslrilinkset.o objslrilibstruc.o
SLRILIB_SRC_ODBC = slri_odbc.c
SLRILIB_OBJ_ODBC = slri_odbc.oodbc

CC = $(NCBI_CC)
OPTFLAG = -O $(NCBI_OPTFLAG)
INCPATH = $(NCBI_INCDIR) -I$(SLRI_INCDIR) $(CUSTOM_INCDIR) -I$(INCDIR_CB) -I$(INCDIR_DB2)
LIBPATH = $(NCBI_LIBDIR) -L$(SLRI_LIBDIR) $(CUSTOM_LIBDIR) -L$(LIBDIR_CB) -L$(LIBDIR_DB2)
CFLAGS  = $(NCBI_CFLAGS1) $(CFLAGS1) $(OPTFLAG) $(EXTRAOPT) -I$(INCPATH) $(VIBFLAG) $(DEF)
LDFLAGS  = $(NCBI_LDFLAGS1) -I$(INCPATH) $(OPTFLAG) -L$(LIBPATH) $(VIBFLAG)
VIBLIBS = $(NCBI_VIBLIBS)
VIBFLAG = $(NCBI_VIBFLAG)


##################################################
## DATABASE SPECIFIC FLAGS  FOR SEQHOUNDLOCLLIB ##
##################################################
# INCLUDE THESE IN YOUR MAKEFILES to target dbs  #
# Remember to add the database library           #
##################################################

# CODEBASE
CFLAGS_CB         = $(CFLAGS) $(DEF_CB) $(EXTRAOPT_CB)
LDFLAGS_CB        = $(LDFLAGS) $(DEF_CB)

# DB2
CFLAGS_DB2        = $(CFLAGS) $(DEF_DB2) $(EXTRAOPT_DB2)
LDFLAGS_DB2       = $(LDFLAGS) $(DEF_DB2)

# ODBC
# EXTRAOPT_ODBC = -I/software/32/unixodbc/include -L/software/32/unixodbc/lib
INCPATH_ODBC       = $(NCBI_INCDIR) -I$(BZDIR) -I. 
CFLAGSODBC         = $(NCBI_FLAGS1) $(CFLAGS1) $(EXTRAOPT_ODBC) -I$(INCPATH_ODBC) -I$(INCPATH) $(DEF)
#CFLAGS_ODBC        = $(CFLAGSODBC) $(DEF_ODBC) $(EXTRAOPT_ODBC)
CFLAGS_ODBC        = $(CFLAGS) $(DEF_ODBC) $(EXTRAOPT_ODBC)
LDFLAGS_ODBC       = $(LDFLAGS) $(DEF_ODBC)
