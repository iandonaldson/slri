#!/usr/bin/bash
# $Id: text_dump.sh,v 1.4 2005/04/29 16:39:53 kzheng Exp $

# dump portable tables from text
DBNAME=$1
DUMPDIR=./dumpdir
mkdir $DUMPDIR
chmod a+w $DUMPDIR
date; echo Dumping database $DBNAME to $DUMPDIR

date; echo dumping text_db
mysqldump -t --tab=$DUMPDIR $DBNAME text_db
date; echo dumping text_method
mysqldump -t --tab=$DUMPDIR $DBNAME text_method
date; echo dumping text_doc
mysqldump -t --tab=$DUMPDIR $DBNAME text_doc 
date; echo dumping text_docscore
mysqldump -t --tab=$DUMPDIR $DBNAME text_docscore
date; echo dumping text_organism
mysqldump -t --tab=$DUMPDIR $DBNAME text_organism
date; echo dumping text_doctax
mysqldump -t --tab=$DUMPDIR $DBNAME text_doctax
date; echo dumping text_englishdict
mysqldump -t --tab=$DUMPDIR $DBNAME text_englishdict
date; echo dumping text_stopword
mysqldump -t --tab=$DUMPDIR $DBNAME text_stopword
date; echo dumping text_bncorpus
mysqldump -t --tab=$DUMPDIR $DBNAME text_bncorpus
date; echo dumping text_rng
mysqldump -t --tab=$DUMPDIR $DBNAME text_rng
date; echo dumping text_rngresult
mysqldump -t --tab=$DUMPDIR $DBNAME text_rngresult
date; echo dumping text_pattern
mysqldump -t --tab=$DUMPDIR $DBNAME text_pattern
date; echo dumping text_namepair
mysqldump -t --tab=$DUMPDIR $DBNAME text_namepair
date; echo dumping text_namepairresult
mysqldump -t --tab=$DUMPDIR $DBNAME text_namepairresult

date; echo Dumping finished



