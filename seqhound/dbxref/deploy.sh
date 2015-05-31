#!/usr/bin/bash

# Deploy a file into the cluster using cpcp command.
# if the sub-directory doesn't exist will be created.
# arg1: subdirectory.
# arg2: filename to deploy

dsh mkdir /scratch/dbxrefgoa/download/$1/
cpcp /scratch/dbxrefgoa/download/$1/$2 /scratch/dbxrefgoa/download/$1/
