#! /bin/bash
# go to the update code directory
cd /databases/slri/seqhound/text
# source profile
. /etc/profile
. /export/home/db2inst1/.bash_profile

# Usage: prebind_update.pl [-r T/F] [-t taxid] [-u table_name] 
./text_update.pl  >> /databases/slri/seqhound/text/text.updatecron.log 
2>&1
