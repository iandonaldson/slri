#!/usr/bin/bash

# For record base flat files separated by //
# grep ID DB // AC for swissprot trembl records.
# Deploy a file to the cluster using cpcp command.
# if the sub-directory doesn't exist will be created.
# arg1: subdirectory.
# arg2: filename to deploy

grep -e '^ID   ' -e '^DR   ' -e '^//' -e '^AC   ' /scratch/dbxrefgoa/download/$1/uniprot_trembl.dat > /scratch/dbxrefgoa/download/$1/$2
dsh mkdir /scratch/dbxrefgoa/download/$1/
cpcp /scratch/dbxrefgoa/download/$1/$2 /scratch/dbxrefgoa/download/$1/$2
