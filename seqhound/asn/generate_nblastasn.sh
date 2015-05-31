#! /bin/sh

######################################
## Hao Lieu June 2001
## A script to run datatool generating
## NBlastasn.asn source code
#####################################

# Modifications to be made in this file:
# 1) declare an environment variable NCBICXX that points 
#    to the ncbi_cxx directory
#
# 2) declare an environment variable SLRI that points
#    to the slri directory
#
# 3) change the TOOLDIR variable (below) to the correct
#    path of the datatool. This requires changing the
#    'GCC-Release' directory to the corresponding
#    directory on this computer. Check the exact
#    directory in the ncbi_cxx directory.

TOOLDIR=$NCBICXX/GCC/bin/datatool
D=$NCBICXX/src/objects
M=-M 
INTERNAL=./
MODULE=$INTERNAL/NBlastasn.asn

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

$TOOLDIR -m $MODULE -of asn2nblastasn.log -oA -oph ../include_cxx -opc ../src_cxx -od nblast.def

