#!/bin/sh
for i in download/GA/gene_association.* 
do
     echo "perl goa_parser.pl $i "
     perl goa_parser.pl $i
done

