#! /bin/sh

######################################
## Hao Lieu June 2001
## A script to run datatool generating
## slrilibstruc.asn source code
#####################################

# To use this script, an environment variable NCBICXX must be
# declared and set to point to the ncbi_cxx toolkit 
# and a SLRI environment variable must be set to point to the slri directory

# You will also need to change TOOLDIR to reflect your particular
# configuration of the ncbi_cxx toolkit. For example possible settings are:
# TOOLDIR=$NCBICXX/GCC-Debug/bin/datatool
# TOOLDIR=$NCBICXX/CC-Release/bin/datatool
# Your exact setting can be found by checking in your $NCBICXX directory.

TOOLDIR=$NCBICXX/GCC/bin/datatool
D=$NCBICXX/src/objects
M=-M 
INTERNAL=$SLRI/lib/asn
MODULE=$INTERNAL/slrilibstruc.asn

M1=$INTERNAL/slrilibstruc.asn 
M2=$D/cdd/cdd.asn
M3=$D/pub/pub.asn
M4=$D/seqset/seqset.asn
M5=$D/seqfeat/seqfeat.asn
M6=$D/seqloc/seqloc.asn
M7=$D/mmdb1/mmdb1.asn
M8=$D/mmdb3/mmdb3.asn
M9=$D/seq/seq.asn
M10=$D/seqalign/seqalign.asn


$TOOLDIR -m $MODULE -M "$M9 $M7 $M6 $M10 $M4" -of asn2slrilibstruc.log -oA -oph ../include -opc ../src -od slrilibstruc.def

