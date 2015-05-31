# -- $id$
# --------------------------------------------------------------------
# -- combine ll_go tables from different sources
# -- see README.ll_go and combine_ll_go.sh for more detail
# -- mysql -h marge -P43306 -u rcavero -p <ll_go.sql
# --------------------------------------------------------------------
use dbxrefnew;

-- export new gigo tables to text files
SELECT * INTO OUTFILE '/home/kzheng/ll_go/gigoII.txt' FROM gigoII;
SELECT * INTO OUTFILE '/home/kzheng/ll_go/gigoIII.txt' FROM gigoIII;
#--  also run: "perl prebind_dump.pl LL_GO.dbf dump.log > gigo_cb.txt"

-- create temporary tables to combine all tables
DROP TABLE IF EXISTS gigo_temp;
CREATE TABLE gigo_temp(
    gi INT  NOT NULL,
    go_id  INT  NOT NULL,
    pmid INT  NOT NULL DEFAULT 0,
    evidence CHAR(50) NOT NULL
);

LOAD DATA INFILE '/home/kzheng/ll_go/gigoII.txt' INTO TABLE gigo_temp(gi, go_id, evidence);
LOAD DATA INFILE '/home/kzheng/ll_go/gigoIII.txt' INTO TABLE gigo_temp(gi, go_id, evidence);
LOAD DATA INFILE '/home/kzheng/ll_go/gigo_cb.txt' INTO TABLE gigo_temp;

-- create a non-redundent record list
SELECT DISTINCT * INTO OUTFILE '/home/kzheng/ll_go/gigo_new.txt' FROM gigo_temp;
#-- Query OK, 8885877 rows affected (28 min 52.98 sec);

-- now the new ll_go table
DROP TABLE IF EXISTS ll_go; 
CREATE TABLE ll_go (
	rowid INTEGER NOT NULL AUTO_INCREMENT,
  	gi INTEGER NOT NULL,
  	go_id INTEGER,
  	pmid INTEGER,
  	evidence VARCHAR(50),
  	INDEX illgo_rowid(rowid),
  	INDEX illgo_gi(gi),
  	INDEX illgo_goid(go_id),
  	INDEX illgo_pmid(pmid)
);

LOAD DATA INFILE '/home/kzheng/ll_go/gigo_new.txt' INTO TABLE ll_go(gi, go_id,pmid, evidence);
#-- Query OK, 8885877 rows affected (5 min 17.87 sec);

DROP TABLE gigo_temp;
#-- $log$

