#!/usr/bin/perl 

use lib '/home/mmatan/local/lib';
use DBI;
use strict;


######################################################################
#
# update table ll_cdd using the data from rpsdb.
#
######################################################################


# ########## for MySQL #############
my $host = "host";
my $port = "3306";
my $database = "seqhound";
my $user_name = "user";
my $password = "password";
my $dsn = "DBI:mysql:" . $database . ":" . $host . ":" . $port;
my ($dbh, $sth);
my $sql_stmt;

######################
# connect to MySQL
######################
$dbh = DBI->connect ($dsn, $user_name, $password, {RaiseError => 1, AutoCommit => 0});

$dbh->do("DELETE FROM ll_cdd");

$sql_stmt = "INSERT INTO ll_cdd (ll_id, cdd_id, evalue) ";
$sql_stmt .= "SELECT ll_llink.ll_id, rpsdb.domid, rpsdb.evalue ";
$sql_stmt .= "FROM rpsdb, ll_llink ";
$sql_stmt .= "WHERE rpsdb.gi=ll_llink.gi AND ll_llink.ll_id>0 AND evalue<=-2;";
$dbh->do($sql_stmt);

$dbh->commit();	
$dbh->disconnect();

exit(0); 
