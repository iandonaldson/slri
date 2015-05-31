# 1. go to dbxrefgoa working directory, the path is useful for cron job
cd /tempdata/dbxrefgoa/
echo "Current working directory is: "
echo `pwd`

# 2. delete and rebuild dbxref database and data tables
mysql < dbxrefgoa.sql
echo "dbxrefgoa database and tables are recreated"

mysql < goa_xdb.sql
echo "dbxrefgoa.goa_xdf table fixed"

# 3. delete all data in download directory
if [ -d download ];
then 
	rm -rf download
	echo "download files directory removed"
fi

# 4  create download directory
mkdir download
echo "download files directory recreated"

# 5 Populate the database for the first time.
perl dbxref_updatecron.pl >> dbxref_updatecron.log 2>> dbxref_errors.log
