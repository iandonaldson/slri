########################################################################
#	Generic command-line makefile for NCBI applications
#	Adapted for compilation of prebind executables and libraries
#
#   	This assumes the following variables are set in a file called "ncbi.mk"
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
# machine dependent paths,for prebind
#
#######################################################################

include $(NCBI)/ncbi.mk

PREBINDDIR = ..
PREBINDOBJS = $(PREBINDDIR)/lib/object
PREBINDINC = $(PREBINDDIR)/include
PREBINDLIB = $(PREBINDDIR)/lib

#Codebase includes and libraires are defined by an environment variable called CBDIR
CBINC = $(CBDIR)
CBLIB = $(CBDIR)

MODIR = $(PREBINDDIR)/../mobidick
MOINC = $(MODIR)/include
MOLIB = $(MODIR)/lib

RSADIR = $(PREBINDDIR)/../../rsaref
DESDIR = $(PREBINDDIR)/../../deslib

SLRILIBDIR = $(PREBINDDIR)/../lib
SLRIINC = $(SLRILIBDIR)
SLRILIB = $(SLRILIBDIR)

BINDLIB = $(PREBINDDIR)/../bind/lib
BINDINC = $(PREBINDDIR)/../bind/lib
BINDOBJS = $(PREBINDDIR)/../bind/lib/object

SEQHOUNDINC = $(PREBINDDIR)/../seqhound/include
SEQHOUNDLIB = $(PREBINDDIR)/../seqhound/lib

BZIPINC = $(PREBINDDIR)/../../bzip2
BZIPLIBDIR = $(PREBINDDIR)/../../bzip2
BZIPLIBNAME = $(BZIPLIBDIR)/libbz.a
BZIPLIBMAKE = make.bzlib

INCDIRS = -I$(PREBINDINC) -I$(PREBINDOBJS) -I$(SLRIINC) -I$(MOINC) -I$(CBINC) -I$(SEQHOUNDINC) -I$(BINDINC) -I$(BINDOBJS) -I$(BZIPINC) -I$(RSADIR) -I$(DESDIR) 

LIBDIRS = -L$(CBLIB) -L$(SLRILIB) -L$(MOLIB) -L$(SEQHOUNDLIB) -L$(PREBINDLIB) -L$(BINDLIB) -L$(BINDOBJS) -L$(BZIPLIBDIR) -L$(RSADIR) -L$(DESDIR) 

EXTRAOPT = -D_FILE_OFFSET_BITS=64

MISCLIBS = 
BUILDDIR = $(PREBINDDIR)/build


#Aliased NCBI libraries - WARNING, these numbers may change!!!
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
#LIB14 = -lncbibls0
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
LIB28 = -lvibgif
LIB29 = -lncbitxc1
LIB30 = -lncbicn3d
LIB31 = -lvibnet
LIB33 = -lsmartnet
LIB35 = -lssparse
LIB36 = -lnetblast
LIB38 = -lncbiprsn
LIB40 = -lncbitxc2
LIB41 = -lncbiid1
LIB42 = -lctutils
LIB43 = -losutils
LIB44 = -lidload
LIB45 = -lddvlib
LIB400 = -lvibrantOGL
LIB3000 = -lncbicn3dOGL

# TRADES and/or SLRI libraries
LIB95 = -ldssp
LIB96 = -lmdoutapi
LIB97 = -ltaskapi
LIB98 = -lbz
LIB99 = -lmmdbtraj
LIB100 = -lfoldtraj
LIB101 = -lrsaref
LIB102 = -lDES

## NOTE- Oct 25, 2000
## SeqHound library names have changed to
## libshoundlocl.a and libshoundtax.a
## Note that the slri lib has been split in two:
## -lslri is the core lib and 
## -lslri_db2 OR -lslri_cb is the database backend specific functions ## of the slri lib 
## If you are compiling the shound local api (lshoundlocllib_db2)
## then you will need the corresponding db specific slri lib
LIB199 = -lcb
LIB200 = -lshoundlocllib_db2
LIB201 = -lshoundrem
LIB202 = -lslri
LIB203 = -lslriobj
LIB204 = -lfastobj
LIB205 = -lslri_db2
LIB206 = -lprebind
LIB207 = -lbind
LIB208 = -lshoundlocllib_cb
LIB209 = -lslri_cb
LIB210 = -lbindobj

#######################################################################
#
# default flags for compiling and loading
#
#######################################################################

CC = $(NCBI_CC) $(DEF)
CFLAGS1 = $(NCBI_CFLAGS1)
LDFLAGS1 = $(NCBI_LDFLAGS1)
OPTFLAG =  $(EXTRAOPT)
OTHERLIBS = $(NCBI_OTHERLIBS)
VIBLIBS = $(NCBI_VIBLIBS)
VIBFLAG = $(NCBI_VIBFLAG)
INCPATH = $(NCBI_INCDIR) $(INCDIRS)
LIBPATH = $(NCBI_LIBDIR) $(LIBDIRS)
DEF =
