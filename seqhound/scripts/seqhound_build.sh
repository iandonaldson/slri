#! /bin/sh

#needs an argument for release!!!!  e.g. ./seqhound_build.sh 127

flag=FALSE
for i in *.aso.gz
do
  gunzip $i
  j=`echo $i | sed 's/.gz//'`
  if [ "$flag" = "FALSE" ]
  then
     ./mother -i $j -r $1 -n T -m T
  else
     ./mother -i $j -r $1 -n T -m F
  fi
  gzip $j
  flag=TRUE
done
mkdir logs
mv *run logs


