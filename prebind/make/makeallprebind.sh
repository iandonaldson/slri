#!/usr/bin/bash

echo "== start =="

if [ "x$1" == "xdebug" ]; then
	export NCBI=/software/ncbi_debug
	export CC="cc -xildoff -g -xs -xarch=v9a -D_FILE_OFFSET_BITS=64"
	echo "\n== DEBUG CC is: $CC =="
else
	export NCBI=/software/ncbi
fi

echo "== check if this is right version of ncbi: $NCBI =="

echo; echo "== make.prebindlib =="
make -e -f make.prebindlib

echo; echo "== make.prebindparser =="
make -e -f make.prebindparser


echo; echo "== make.prebindsearcher=="
make -e -f make.prebindsearcher

echo; echo "== make.prebindcgi =="
make -e -f make.prebindcgi

echo; echo "== finished =="
