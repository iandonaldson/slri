#!/usr/bin/perl -w
# $Id: generate_input.pl,v 1.1 2005/03/24 22:05:55 ryao Exp $

##############################################################################
#
# File       : generate_input.pl
# Description: This is for Blueprint SeqHound data validation project. 
#              The script generates an input file called input_test. The test
#              cases are randomly picked from SeqHound accdb and taxgi tables.
# Usage      : perl generate_input.pl
# Author     : Rong Yao (ryao@blueprint.org)
# Date       : March, 2005
###############################################################################

use DBI;
use Time::localtime;
use strict;


# for MySQL, connect to Seqhound production database
my $host = "skinner";
my $port = "6612";
my $database = "seqhound";
my $user_name = "sh";
my $password = "sh";
my $dsn = "DBI:mysql:" . $database . ":" . $host . ":" . $port;
my $dbh;
my $sth;

open OUTFILE, ">input_test";
print OUTFILE "[SHoundGetGenBankff]/[SHoundGetFasta]/[SHoundAccFromGi]/[SHoundGetSeqEntry]\n";
# connec to MySQL
$dbh = DBI->connect ($dsn, $user_name, $password, {RaiseError => 0});

my ($a,$b,$i,$input) = 0;
my (@gilist, @acclist)=();
#randomly generate a list of integers
for ($i=0;$i<3000;$i++){
	$a+=rand(10);
	$b+=rand(1000);
	$input = int($a*(++$i)+$b*(++$i/10));
	push (@gilist, $input);
}
$input = join (",", @gilist);
$sth = $dbh->prepare ("SELECT distinct a.gi, a.access FROM accdb a, taxgi t where a.gi=t.gi and a.gi in ($input) and t.type='DNA' and a.access not like 'n/a' limit 50;" );
$sth->execute();
my @ct_MS;
while (@ct_MS =$sth->fetchrow()) {
   my ($gi, $acc)= ($ct_MS[0], $ct_MS[1]);
#  print "$gi	$acc\n";
   push (@acclist, $acc);
   print OUTFILE "db=nucleotide;query=$gi;report=-\n";
}
$sth->finish();

$sth = $dbh->prepare ("SELECT distinct a.gi, a.access FROM accdb a, taxgi t where a.gi=t.gi and a.gi in ($input) and t.type='protein' and a.access not like 'n/a' limit 50;");
$sth->execute();
while (@ct_MS =$sth->fetchrow()) {
   my ($gi, $acc) = ($ct_MS[0], $ct_MS[1]);
#print "$gi	$acc\n";
   print OUTFILE "db=protein;query=$gi;report=-\n";
}
$sth->finish();

print OUTFILE "\n[SHoundFindAcc]\n";
$i=0;
foreach my $acc (@acclist){
	if ($i < 50){
		print OUTFILE "db=nucleotide;query=$acc;report=gi\n";
	    $i++;
	}
	elsif ($i<100){
		print OUTFILE "db=protein;query=$acc;report=gi\n";
	    $i++;
	}
}
        
print OUTFILE "\n[SHoundGetTaxNameFromTaxID]\n";
$sth = $dbh->prepare ("SELECT distinct taxid FROM taxgi where taxid in ($input) limit 100;");
$sth->execute();
while (@ct_MS =$sth->fetchrow()) {
   my ($taxid) = ($ct_MS[0]);
# print "$taxid\n";
   print OUTFILE "db=taxonomy;query=$taxid;report=brief\n";
}
$sth->finish();


$dbh->disconnect();
exit(0); 

