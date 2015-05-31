#!/usr/bin/bash

echo "== start =="

MAKEDIR=`pwd`
echo; echo "== make directory is: $MAKEDIR =="


if [ "x$1" == "xdebug" ]; then
        export NCBI=/software/ncbi_debug
        export CC="cc -xildoff -g -xs -xarch=v9a -D_FILE_OFFSET_BITS=64"
        echo "== DEBUG CC is: $CC =="
else
        export NCBI=/software/ncbi
fi

echo; echo "== check this is right version of ncbi: $NCBI =="

echo; echo "== make.bzlib =="
cd ../../../bzip2; make -e -f make.bzlib
if [ "x$?" != "x0" ]; then
	echo "----- MAKE has failed with error code $? -----"
fi
cd $MAKEDIR

echo; echo "== make.deslib =="
cd ../../../deslib; make -e -f make.deslib
if [ "x$?" != "x0" ]; then
	echo "----- MAKE has failed with error code $? -----"
fi
cd $MAKEDIR

echo; echo "== make.rsareflib =="
cd ../../../rsaref; make -e -f make.rsareflib
if [ "x$?" != "x0" ]; then
	echo "----- MAKE has failed with error code $? -----"
fi
cd $MAKEDIR

echo; echo "== make.slrilib_cb =="
cd ../../../slri/lib; make -e -f make.slrilib_cb
if [ "x$?" != "x0" ]; then
	echo "----- MAKE has failed with error code $? -----"
fi
cd $MAKEDIR

echo; echo "== make.slrilib =="
cd ../../../slri/lib; make -e -f make.slrilib
if [ "x$?" != "x0" ]; then
	echo "----- MAKE has failed with error code $? -----"
fi
cd $MAKEDIR

echo; echo "== make.taskapi =="
cd ../../../slri/mobidick/build/taskapi; make -e -f make.taskapi
if [ "x$?" != "x0" ]; then
	echo "----- MAKE has failed with error code $? -----"
fi
cd $MAKEDIR

echo; echo "== make.slritilib =="
cd ../../../slri/textindex; make -e -f make.slritilib
if [ "x$?" != "x0" ]; then
	echo "----- MAKE has failed with error code $? -----"
fi
cd $MAKEDIR

echo; echo "== make.shoundlocllib =="
cd ../../../slri/seqhound/src; make -e -f make.shoundlocllib
if [ "x$?" != "x0" ]; then
	echo "----- MAKE has failed with error code $? -----"
fi
cd $MAKEDIR

echo; echo "== make.shoundremlib =="
cd ../../../slri/seqhound/src; make -e -f make.shoundremlib
if [ "x$?" != "x0" ]; then
	echo "----- MAKE has failed with error code $? -----"
fi
cd $MAKEDIR

#-------------------------------------------------------------
#
#   NOTE: make.bindlib and make.bindobjlib will not compile
#         with the -e option and when compiled without -e,
#         the -xarch flag is not set. This causes errors
#         when compiling prebind. Therefore set CC in the
#         make command.
#-------------------------------------------------------------
echo; echo "== make.bindlib =="
cd ../../../slri/bind/make; 
make -f make.bindlib CC="cc -xildoff -g -xs -xarch=v9a -D_FILE_OFFSET_BITS=64"
if [ "x$?" != "x0" ]; then
	echo "----- MAKE has failed with error code $? -----"
fi
cd $MAKEDIR

echo; echo "== make.bindobjlib =="
cd ../../../slri/bind/make; 
make -f make.bindobjlib CC="cc -xildoff -g -xs -xarch=v9a -D_FILE_OFFSET_BITS=64"
if [ "x$?" != "x0" ]; then
	echo "----- MAKE has failed with error code $? -----"
fi
cd $MAKEDIR

echo; echo "== finished =="
