#! /bin/sh

######################################
## Hao Lieu June 2001
## A script to run datatool generating
## NBlastasn.asn source code
#####################################

# Modifications to be made in this file
# 1) declare environment variable NCBICXX to point to the ncbi_cxx directory.
#
# 2) declare environment variable SLRI to point to the slri directory.
#
# 3) change TOOLDIR path to reflect your location of datatool. 
#    The exact path can be found in ncbi_cxx directory.

TOOLDIR=$NCBICXX/GCC/bin/datatool
D=$NCBICXX/src/objects
M=-M 
INTERNAL=./
SLRILIBSTRUC=../../lib/asn
MODULE=$INTERNAL/slristruc.asn

M1=$SLRILIBSTRUC/slrilibstruc.asn 
M2=$D/cdd/cdd.asn
M3=$D/pub/pub.asn
M4=$D/seqset/seqset.asn
M5=$D/seqfeat/seqfeat.asn
M6=$D/seqloc/seqloc.asn
M7=$D/mmdb1/mmdb1.asn
M8=$D/mmdb3/mmdb3.asn
M9=$D/seq/seq.asn
M10=$D/seqalign/seqalign.asn
M11=$SLRILIBSTRUC/slrilinkset.asn

$TOOLDIR -m $MODULE -M "$M9 $M7 $M4 $M1 $M2 $M11" -of asn2slristruc.log -oA -oph ../include_cxx -opc ../src_cxx -od slristruc.def
