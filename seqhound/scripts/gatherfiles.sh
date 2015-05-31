#! /bin/sh

# this makes executables needed to build seqhound and copies these and all script files into one directory
# argument 1 is an absolute path to seqhound directory in CVS tree
# argument 2 is an absolute path to directory where the files are copied

cd $1/parsers
make -f make.mother
make -f make.redund
make -f make.cbmmdb
cd $1/taxon
make -f make.importtaxdb
cd $1/locuslink
make -f make.llparser
make -f make.addgoid
cd $1/go
make -f make.goparser
cd $1/genomes
make -f make.chrom
make -f make.comgen
cd $1/domains
make -f make.vastblst
make -f make.pdbrep
cp $1/config/.intrezrc $2
cp $1/scripts/asnftp.pl $2
cp $1/scripts/seqhound_build.sh $2
cp $1/build/cb/mother $2
cp $1/genomes/chromff $2
cp $1/build/cb/chrom $2
cp $1/scripts/nrftp.pl $2
cp $1/build/cb/redund $2
cp $1/scripts/taxftp.pl $2
cp $1/build/cb/importtaxdb  $2
cp $1/scripts/llftp.pl $2
cp $1/build/cb/llparser $2
cp $1/scripts/goftp.pl $2
cp $1/build/cb/goparser $2
cp $1/build/cb/addgoid $2
cp $1/build/cb/comgen $2
cp $1/scripts/genftp.pl $2
cp $1/scripts/humoasn.p $2
cp $1/scripts/humouse_build.sh $2
cp $1/scripts/mmdbftp.pl $2
cp $1/build/cb/cbmmdb $2
cp $1/build/cb/vastblst $2
cp $1/build/cb/pdbrep $2



