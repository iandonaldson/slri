#!/usr/bin/bash
# $Id$

# specify current database name and dumped data path
DBNAME=$1
DUMPDIR=./dumpdir

# create database
mysql -e "CREATE DATABASE IF NOT EXISTS $DBNAME"

# create tables
mysql $DBNAME < text.sql

# import existing/precomputed tables
date; echo loading text_db
mysqlimport --local $DBNAME $DUMPDIR/text_db.txt
date; echo loading text_method
mysqlimport --local $DBNAME $DUMPDIR/text_method.txt
date; echo loading text_doc
mysqlimport --local $DBNAME $DUMPDIR/text_doc.txt
date; echo loading text_docscore
mysqlimport --local $DBNAME $DUMPDIR/text_docscore.txt
date; echo loading text_orgranism
mysqlimport --local $DBNAME $DUMPDIR/text_organism.txt
date; echo loading text_doctax
mysqlimport --local $DBNAME $DUMPDIR/text_doctax.txt
date; echo loading text_englishdict
mysqlimport --local $DBNAME $DUMPDIR/text_englishdict.txt
date; echo loading text_stopword
mysqlimport --local $DBNAME $DUMPDIR/text_stopword.txt
date; echo loading text_bncorpus
mysqlimport --local $DBNAME $DUMPDIR/text_bncorpus.txt
date; echo loading text_rng
mysqlimport --local $DBNAME $DUMPDIR/text_rng.txt
date; echo loading text_rngresult
mysqlimport --local $DBNAME $DUMPDIR/text_rngresult.txt
date; echo loading text_pattern
mysqlimport --local $DBNAME $DUMPDIR/text_pattern.txt
date; echo loading text_namepair
mysqlimport --local $DBNAME $DUMPDIR/text_namepair.txt
date; echo loading text_namepairresult
mysqlimport --local $DBNAME $DUMPDIR/text_namepairresult.txt
date
mysql -e "SHOW TABLE STATUS FROM $DBNAME\G"

