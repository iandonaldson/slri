#!/bin/sh
echo "perl dbxref_tr.pl clean TR"
perl dbxref_tr.pl clean TR
echo "cleaning done"
for i in download/TREMBL/*.dat
do
     echo "perl dbxref_tr.pl $i TR"
     perl dbxref_tr.pl $i TR
done
