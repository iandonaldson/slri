perl prebind_dump.pl LL_GO.dbf dump.log > gigo_cb.txt
mysql -h marge -u rcavero -P43306 -p < ll_go.sql
./gi_go -cT
./gi_go -z0 -m 10000000 -r T -R gigo_new.txt
