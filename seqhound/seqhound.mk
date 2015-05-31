########################################################################
#       Generic command-line makefile for NCBI applications
#
#   This assumes the following variables are set in a file called "ncbi.mk"
#     which is included by this one. In this case it resides in a directory
#     set by the environment variable "NCBI"
#
#    NCBI_CC = default compiler to use
#    NCBI_CFLAGS1 = default compiler switches (not optimization)
#    NCBI_LDFLAGS1 = default compiler switches when linking (not optimization)
#    NCBI_OPTFLAG = default optimization (-O)
#    NCBI_INCDIR = default toolkit include paths
#    NCBI_LIBDIR = default toolkit library paths
#    NCBI_ALTLIB  = debugging toolkit library paths
#    NCBI_VIBFLAG = additional include paths/defines for Vibrant apps
#    NCBI_VIBLIBS  = additional libraries needed to link Vibrant apps
#    NCBI_OTHERLIBS = additional libraries needed to link the system
#
#   This enables platform independent makefiles for ncbi toolkit apps. You
#   can change any default behavior by setting the variable on the command
#   line.
#
#   Use gcc complier.   "make NCBI_CC=gcc" or "make CC=gcc"
#   Change optimization to debug
#                "make NCBI_OPTFLAG="-g" NCBI_LIBPATH=$NCBI_ALTLIB"
#                or
#                "make OPTFLAG="-g" LIBPATH=$NCBI_ALTLIB"
#
#   You can also change the behavior permanently in the makefile by changing
#    the assignment of the internal variable to the one from the include
#    file:
#
#   Use gcc compiler.
#    Change   CC=$(NCBI_CC)
#    To       CC=gcc
#    in the makefile itself
#
#   Make debugging version
#    OPTFLAG=-g
#    LIBPATH=$(NCBI_ALTDIR)
#
#   You need to specify the EXE and SRC symbols
#
#######################################################################
#
# default flags for compiling and loading   
#
#######################################################################
include $(NCBI)/ncbi.mk
# For cross-platform compatibility, SEQHDIR can be overwritten (as in case for compaq alpha)
#SEQHDIR=$(SLRI)/seqhound
SEQHDIR=..
include $(SEQHDIR)/seqhound_cb.mk
include $(SEQHDIR)/seqhound_db2.mk
include $(SEQHDIR)/seqhound_odbc.mk
include $(SEQHDIR)/seqhound_rem.mk
include $(SEQHDIR)/seqhound_rem_odbc.mk

##################
## FCGI HOME    ##
## CUSTOMIZE!!  ##
##################
FCGIDIR=

##################
## COMMON PATHS ##
##################
SLRIINC = ../../lib
SLRIDIR = ../../lib
SLRILIBMAKE = make.slrilib
SLRILIB     = -lslri
SLRILIBMAKE_CB = make.slrilib_cb
SLRILIB_CB  = -lslri_cb
SLRILIBMAKE_DB2 = make.slrilib_db2
SLRILIB_ODBC = -lslri_odbc
# SLRILIBMAKE_ODBC = make.slrilib_odbc
SLRILIB_DB2 = -lslri_db2
BZDIR = ../../../bzip2
#BZDIR = /software/64/bzip2
BZINC = $(BZDIR) 
BZMAKE = make.bzlib
BZLIB = -lbz

SEQH_SRC = ../src
SEQH_INC = ../include
SEQH_LIB = ../lib
SEQH_EXAMPLES = ../examples

SEQH_REM_MAKE  = make.shoundremlib
#SEQH_LIB_CB_REM   = libshoundremlib_cb.a
#SEQH_REM_LIB_CB   = -lshoundremlib_cb
SEQH_REM_LIB = -lshoundremlib
SEQH_LIB_CB_REM   = libshoundrem.a
SEQH_REM_LIB_CB   = -lshoundrem
SEQH_LIB_ODBC_REM   = libshoundremlib_odbc.a
SEQH_REM_LIB_ODBC   = -lshoundremlib_odbc

SEQH_LOCL_MAKE    = make.shoundlocllib
SEQH_LIB_CB       = libshoundlocllib_cb.a
SEQH_LOCL_LIB_CB  = -lshoundlocllib_cb
SEQH_LIB_DB2      = libshoundlocllib_db2.a
SEQH_LOCL_LIB_DB2 = -lshoundlocllib_db2
SEQH_LIB_ODBC = libshoundlocllib_odbc.a
SEQH_LOCL_LIB_ODBC = -lshoundlocllib_odbc


SEQH_TAXINC = ../taxon
SEQH_LLINC = ../locuslink
SEQH_GOINC = ../go
SEQH_TIDIR = ../../textindex
SEQH_TIOBJ = $(SEQH_TIDIR)/object
SEQH_TILIB = -lslriti
SEQH_TIMAKE = make.slritilib

############ NOT INCLUDED IN COMMON PATH #############
## ADD other paths 'CUSTOM_INCDIR' or 'CUSTOM_LIBDIR'
MOBIDIR    = ../../mobidick/lib
MOBIINC    = ../../mobidick/include
KANGDIR    = ../../kangaroo
#FCGIDIR    = /home3/people/kaca/apache_1.3.12/devkit_2.2.0/include
#FCGILIB    = -lfcgi
TRAJDIR    = ../../trades/lib
TRAJINC    = ../../trades/include
CUSTOM_INCDIR =
CUSTOM_LIBDIR =
SYSTEMLIBDIR = /usr/lib/gcc-lib/i686-pld-linux/3.0.1
SYSTEMLIB  = -lstdc++

########################################################

SEQH_INCDIR = $(SLRIINC) -I$(SEQH_TAXINC) -I$(SEQH_INC) -I$(BZINC) -I$(SEQH_LLINC) -I$(SEQH_GOINC) -I$(KANGDIR) -I$(SEQH_TIDIR) -I$(SEQH_TIOBJ) -I./
SEQH_LIBDIR = $(SLRIDIR) -L$(SEQH_LIB) -L$(BZDIR) -L$(SEQH_TIDIR) 
BUILDDIR = ../build
BUILDDIR_REM = $(BUILDDIR)/rem
BUILDDIR_DB2 = $(BUILDDIR)/db2
BUILDDIR_CB  = $(BUILDDIR)/cb
BUILDDIR_ODBC  = $(BUILDDIR)/odbc

EXTRAOPT = -D_FILE_OFFSET_BITS=64
OTHERLIBS = $(NCBI_OTHERLIBS) 
ARFLAGS =

##################
## MAKE SECTION ##
##################
.SUFFIXES:
.SUFFIXES: .c .h .o .ocb .odb2 .oodbc
## default and new database-specific implicit actions
.c.o :
	$(CC) $(CFLAGS) $<
.c.ocb :
	$(CC) $(CFLAGS_CB) $<
	mv $*.o $*.ocb
.c.odb2 :
	$(CC) $(CFLAGS_DB2) $<
	mv $*.o $*.odb2
.c.oodbc :
	$(CC) $(CFLAGS_ODBC) $<
	mv $*.o $*.oodbc

SEQH_SRC_COM = intrez_cfg.c intrez_misc.c objslristruc.c nbr.c objNBlastasn.c \
	objslritax.c taxdb_cons.c seq_util.c slri_post.c ll_query.c go_query.c \
	taxdb_enum.c rpsdbapi.c
SEQH_OBJ_COM = intrez_cfg.o intrez_misc.o objslristruc.o nbr.o objNBlastasn.o \
	objslritax.o taxdb_cons.o seq_util.o slri_post.o ll_query.o go_query.o \
	taxdb_enum.o rpsdbapi.o
SEQH_ODBC_COM = intrez_cfg.o intrez_misc.o objslristruc.o nbr.o objNBlastasn.o \
        objslritax.o taxdb_cons.o seq_util.o slri_post.o ll_query.o go_query.o \
         taxdb_enum.o rpsdbapi.o parseasn.o


CC = $(NCBI_CC)
OPTFLAG = $(NCBI_OPTFLAG)
INCPATH = $(NCBI_INCDIR) -I$(SEQH_INCDIR) -I$(INCDIR_CB)
LIBPATH = $(NCBI_LIBDIR) -L$(SEQH_LIBDIR) -L$(LIBDIR_CB)
CFLAGS  = $(NCBI_CFLAGS1) $(CFLAGS1) $(OPTFLAG) $(EXTRAOPT) -I$(INCPATH) $(VIBFLAG) $(DEF)
#CFLAGS  = $(NCBI_CFLAGS1) $(CFLAGS1) -I$(INCPATH) $(VIBFLAG) $(DEF)
LDFLAGS  = $(NCBI_LDFLAGS1) -I$(INCPATH) $(OPTFLAG) -L$(LIBPATH) $(VIBFLAG)
#LDFLAGS  = -I$(INCPATH) -L$(LIBPATH) $(VIBFLAG)
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

# ODBC: DEF_ODBC is the start of odbc specific libraries and includes
CFLAGS_ODBC        = $(CFLAGS) $(DEF_ODBC) $(EXTRAOPT_ODBC)
LDFLAGS_ODBC      = $(LDFLAGS) $(DEF_ODBC) $(EXTRAOPT_ODBC)

# library Dependencies
SEQH_LOCL_DEPS  = $(LIB22) $(LIB23) $(LIB8) $(LIB7) $(LIB6) $(LIB3) $(LIB2) $(LIB1) $(OTHERLIBS)
SEQH_REM_DEPS   = $(LIB22) $(LIB23) $(LIB2) $(LIB1) $(OTHERLIBS)

#################################
# APPLICATION LINKING VARIABLES #
#################################

SEQH_REM_LIBS   = $(SEQH_REM_LIB) $(SLRILIB) $(SEQH_REM_DEPS)
SEQH_REM_LIBS_CB   = $(SEQH_REM_LIB_CB) $(SLRILIB) $(SEQH_REM_DEPS)
SEQH_REM_LIBS_ODBC = $(SEQH_REM_LIB_ODBC) $(SLRILIB_ODBC) $(SLRILIB) $(SEQH_REM_DEPS)
SEQH_LOCL_LIBS_CB  = $(SEQH_LOCL_LIB_CB) $(SLRILIB_CB) $(SLRILIB) $(BZLIB) $(CBLIB) $(SEQH_LOCL_DEPS)
SEQH_LOCL_LIBS_DB2 = $(SEQH_LOCL_LIB_DB2) $(SLRILIB_DB2) $(SLRILIB) $(BZLIB) $(DB2LIB) $(SEQH_LOCL_DEPS)
SEQH_LOCL_LIBS_ODBC = $(SEQH_LOCL_LIB_ODBC) $(SLRILIB_ODBC) $(SLRILIB) $(BZLIB) $(ODBCLIB) $(SEQH_LOCL_DEPS)
SEQH_FCGI_LIBS_CB  = $(SEQH_LOCL_LIBS_CB) $(FCGILIB)
SEQH_FCGI_LIBS_DB2 = $(SEQH_LOCL_LIBS_DB2) $(FCGILIB)
SEQH_FCGI_LIBS_ODBC = $(SEQH_LOCL_LIBS_ODBC) $(FCGILIB)
SEQH_TI_LIBS_CB    = $(SEQH_TILIB) $(SEQH_LOCL_LIBS_CB)
SEQH_TI_LIBS_DB2   = $(SEQH_TILIB) $(SEQH_LOCL_LIBS_DB2)
SEQH_TI_LIBS_ODBC   = $(SEQH_TILIB) $(SEQH_LOCL_LIBS_ODBC)
SEQH_TRAJ_LIBS_CB = -lmmdbtraj $(SEQH_LOCL_LIBS_CB)


###############################################################
#Aliased NCBI libraries - WARNING, these numbers may change!!!#
###############################################################
LIB1 = -lncbi
LIB2 = -lncbiobj
LIB3 = -lncbicdr
LIB4 = -lvibrant
LIB5 = -lncbiacc
LIB6 = -lnetcli
LIB7 = -lnetentr
LIB8 = -lncbiNacc
LIB9 = -lncbiCacc
# LIB10 is reserved for NCSA socket library
LIB10 =
LIB11 = -lncbimla
LIB12 = -lncbitax
LIB13 = -lncbiid0
#LIB14 = -lncbibls

LIB15 = -lnetcliE
LIB16 = -lnetcliS
LIB17 = -lnetcliES
LIB19 = -lncbispel
# LIB20 is for the NCBI desktop utilities
LIB20 = -lncbidesk
LIB21 = -lncbibls2
LIB22 = -lncbimmdb
LIB23 = -lncbitool
LIB24 = -lncbisugg
LIB25 = -lncbiwww
LIB26 = -lncbitax1
LIB27 = -lncbimsc1
LIB29 = -lncbitxc1
LIB30 = -lncbicn3d
LIB31 = -lvibnet
LIB33 = -lsmartnet
LIB35 = -lssparse
LIB36 = -lnetblast
LIB38 = -lncbiprsn
