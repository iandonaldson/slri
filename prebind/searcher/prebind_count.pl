# syntax:  perl prebind_count.pl outputfile database_table
# example: perl prebind_count.pl point.10116 count10116

use DBI;
use strict;

open (OUT,"> $ARGV[0]") || die " can't open $ARGV[0]\n";
my $table = $ARGV[1];


my $dsn_mysql = "DBI:mysql:search:marge:33306";
my $dbh_mysql = DBI->connect ($dsn_mysql, "kzheng", "asdf1234", 
	{RaiseError => 1, AutoCommit => 1});
my $sth1_mysql = $dbh_mysql->prepare(
	"select distinctrow pmid, count(bioentityid) 
	from $table group by pmid");
my $sth2_mysql = $dbh_mysql->prepare(
    "select bioentityid from $table where pmid=?");
my $sth3_mysql = $dbh_mysql->prepare(
    "select name from $table where pmid=? AND bioentityid=?");


$sth1_mysql->execute();
my (@row1, $pmid, $count);

while(@row1 = $sth1_mysql->fetchrow_array()){
	$pmid = $row1[0];
	$count= $row1[1];
	if ($count>1){
		$sth2_mysql->execute($pmid);
		my (@row2, @ids, @names);
		while(@row2=$sth2_mysql->fetchrow_array() ){
			push @ids, $row2[0];
		}

		my (@row3, $i, $j);
		my $namelist="";
		for($i=0; $i<$count; $i++){
			$sth3_mysql->execute($pmid, $ids[$i]);
			@row3=$sth3_mysql->fetchrow_array();
			my $namelist=$row3[0];
			while(@row3=$sth3_mysql->fetchrow_array() ){
				$namelist.="/$row3[0]";
			}
			push @names,$namelist;
		}
		for ($i=0;$i<$count-1; $i++){
			for ($j=$i; $j<$count; $j++){
				next if lc($names[$i]) eq lc($names[$j]); # if name is the same ignoring cases, skip
				print OUT "$pmid\t$ids[$i]\t$names[$i]\t$ids[$j]\t$names[$j]\t0\n";
			}
		}

	}
}
			

$sth1_mysql->finish;
$sth2_mysql->finish;
$sth3_mysql->finish;

$dbh_mysql->disconnect;

close(OUT);

	




