#!/usr/bin/bash

# script to clean cluster sub-directories after a parser finish.

dsh rm /scratch/dbxrefgoa/download/$1/*
dsh rmdir /scratch/dbxrefgoa/download/$1