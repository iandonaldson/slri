#! /bin/sh

######################################
## Hao Lieu June 2001
## A script to run datatool generating
## slrilinkset.asn source code
#####################################

# To use this script, two environment variables must be set.
# 1) NCBICXX points to the ncbi_cxx directory
# 2) SLRI points to the slri directory

# You will also need to change TOOLDIR to reflect the particular
# hierarchy of your ncbi_cxx directory. Possible settings are:
# TOOLDIR=$NCBICXX/GCC-Debug/bin/datatool
# TOOLDIR=$NCBICXX/CC-Debug/bin/datatool
# as are other possibilities depending on your native compiler and
# configurations when setting up the ncbi_cxx toolkit
# Check your ncbi_cxx for the exact path.

TOOLDIR=$NCBICXX/GCC/bin/datatool
D=$NCBICXX/src/objects
M=-M 
INTERNAL=$SLRI/lib/asn
MODULE=$INTERNAL/slrilinkset.asn

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


$TOOLDIR -m $MODULE -of asn2slrilinkset.log -oA -oph ../include -opc ../src -od slrilinkset.def

