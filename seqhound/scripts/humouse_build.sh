n argument for release!!!!  e.g. ./humouse_build.sh 127


flag=FALSE
for i in *_chr*.asn
do
     if [ "$flag" = "FALSE" ]
     then
        ./mother -i $i -r $1 -n T -m F -c T -t T
     else
        ./mother -i $i -r $1 -n F -m F -c T -t T
     fi
     flag=TRUE
done

for i in rna_*.asn
do
     ./mother -i $i -r $1 -n F -m F -c T -t T
done

for i in protein_*.asn
do
     ./mother -i $i -r $1 -n F -m F -c T -t T
done

gzip *_chr*.asn;
gzip rna_*.asn;
gzip protein_*.asn;

mkdir logs
mv *run logs




