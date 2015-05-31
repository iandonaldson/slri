#! /bin/sh


# needs an argument for release!!!!!!!!!!!!!!!

flag=TRUE
for i in rsnc*.bna.gz
do
  gunzip $i
  j=`echo $i | sed 's/.gz//'`
  k=`echo $j | sed 's/rsnc./rsnc/'`
  mv $j $k
  if [ "$flag" = "FALSE" ]
  then
     ./update -i $k
     ./mother -i $k -r $1 -n T -m F -u T
  else
     ./update -i $k
     ./mother -i $k -r $1 -n F -m F -u T
  fi
  gzip $k
  flag=TRUE
done

