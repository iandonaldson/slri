#!/usr/bin/bash

DBNAME=text
DUMPDIR=dump

# back up whole database
mkdir $DUMPDIR
mysqldump --tab=$DUMPDIR $DBNAME

# dump database to a zip file
mysqldump --qucik $DBNAME | gzip >$DBNAME.gz

# copy tables to another database
HOST1=skinner
HOST2=marge
DBNAME1=seqhound
DBNAME2=skinner
mysqldump -h $HOST1 -P33306 --opt $DBNAME1 bioname | mysql -h $HOST2 $DBNAME2
mysqldump -h $HOST1 -P33306 --opt $DBNAME1 bioentity | mysql -h $HOST2 $DBNAME2

