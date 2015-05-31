# $Id: rpsdb.mk,v 1.1.1.1 2002/02/28 09:25:44 root Exp $
########################################################################
#       Generic command-line makefile for NCBI applications
#	Adapted for compilation of FAST projects
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
# machine dependent paths,for FAST related projects
#
#######################################################################

include $(NCBI)/ncbi.mk

#NCBI  = /home/ncbi
CBDIR = /home/doron/codebase/intel-unix/source
CBINC = $(CBDIR)
CBLIB = $(CBDIR)
BZINC = ../../../bzip2
BZLIB = ../../../bzip2
SEQINC = ../include
SEQLIB = ../lib

SLRILIBDIR = ../../lib
SLRIINC = $(SLRILIBDIR)
SLRILIB = $(SLRILIBDIR)
SLRIOBJINC = $(SLRILIBDIR)/object
SLRIOBJLIB = $(SLRILIBDIR)

SYSTEM_LIB = /usr/lib/gcc-lib/i686-pld-linux/3.0.1

INCDIRS = -I$(BZINC) -I$(CBINC) -I$(SLRIINC) -I$(SLRIOBJINC) -I$(SEQINC) -I$(SYSTEM_LIB)
LIBDIRS = -L$(BZLIB) -L$(CBLIB) -L$(SLRILIB) -L$(SLRIOBJLIB) -L$(SEQLIB)\
	 -L$(SYSTEM_LIB)
#EXTRAOPT = -D_FILE_OFFSET_BITS=64
#EXTRAOPT = -g -v -print-libgcc-file-name
EXTRAOPT = -g
MISCLIBS = 
BUILDDIR = ../build

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

## NOTE- Oct 25, 2000
## SeqHound library names have changed to
## libshoundlocl.a and libshoundtax.a
LIB199 = -lcb
LIB200 = -lshoundlocl
LIB201 = -lshoundrem
LIB202 = -lslri
LIB203 = -lslriobj
LIB204 = -lfastobj
##
# For gcc3.0.X you might need to link to system lib
# 
LIB300 = -lstdc++

#######################################################################
#
# default flags for compiling and loading
#
#######################################################################

#$Log: rpsdb.mk,v $
#Revision 1.1.1.1  2002/02/28 09:25:44  root
#Automated import from Sourceforge.net
#
#Revision 1.1  2002/02/27 18:15:48  betel
#rpsdb.mk deposit
#
