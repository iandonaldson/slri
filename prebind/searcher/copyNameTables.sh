# bashshell>. copyNameTables.sh

# mysqldump -h skinner -P3306 --opt seqhound bioentity >~/tmp/skinner/bioentity.sql
mysqldump -h skinner -P3306 --opt seqhound bioentitytype > ~/tmp/skinner/bioentitytype.sql
mysqldump -h skinner -P3306 --opt seqhound nametype > ~/tmp/skinner/nametype.sql
mysqldump -h skinner -P3306 --opt seqhound rules > ~/tmp/skinner/rules.sql
mysqldump -h skinner -P3306 --opt seqhound bioname >~/tmp/skinner/bioname.sql
mysqldump -h skinner -P3306 --opt seqhound taxgi >~/tmp/skinner/taxgi.sql


#mysqladmin create prebind

mysql prebind <~/tmp/skinner/nametype.sql
mysql prebind <~/tmp/skinner/rules.sql
mysql prebind <~/tmp/skinner/bioentity.sql
mysql prebind <~/tmp/skinner/bioentitytype.sql
mysql prebind <~/tmp/skinner/bioname.sql
mysql prebind <~/tmp/skinner/taxgi.sql

