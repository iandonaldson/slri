#! /usr/bin/sh

# need to go to the working directory when running cron job
# 
cd /skinner-local/shoundtest/dbxrefgoa 
perl dbxref_updatecron.pl
