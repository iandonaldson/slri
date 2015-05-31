#! /bin/sh

#needs an argument for release!!!!!!!!!!!!!!

flag=TRUE
for i in nc*.aso.gz
do
  gunzip $i
  j=`echo $i | sed 's/.gz//'`
  if [ "$flag" = "FALSE" ]
  then
     ./update -i $j
     ./mother -i $j -r $1 -n T -m F -u T
  else
     ./update -i $j
     ./mother -i $j -r $1 -n F -m F -u T
  fi
  gzip $j
  flag=TRUE
done

