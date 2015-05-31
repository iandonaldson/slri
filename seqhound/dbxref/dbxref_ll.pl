# Parse ll2ref from NCBI LLink
# ftp://ftp.ncbi.nih.gov/refseq/LocusLink/loc2ref

use strict;
# two choises are available - to use DBD-mysql or to use DBI to connect to
# MySQL-db.
# DBI is a most universal one (DBD-mysql is too specific)
use DBI;
use dbxref;
# Readind Property file.
my $user_name = dbxref->getProperty("dbxref.ini", "user");
my $password  = dbxref->getProperty("dbxref.ini", "password");
my $host      = dbxref->getProperty("dbxref.ini", "host");
my $port      = dbxref->getProperty("dbxref.ini", "port");
my $database  = dbxref->getProperty("dbxref.ini", "database");
my $table     = dbxref->getProperty("dbxref.ini", "table");

my $dsn = "DBI:mysql:" . $database . ":" . $host . ":" . $port;

# 2) connect to MySQL-database
my $dbh = DBI->connect ($dsn, $user_name, $password, {RaiseError => 1});
my $sth = ();
# RAC Read the file name from command line.

my $fileName = dbxref->getProperty( "dbxref.ini", "XREF_LL" );
open(FP, $fileName) or die "can't open $fileName: $!";

#####################################################
# Delete all existing records before adding new ones. 
my %dbtable = dbxref->createDbHashTable( $dbh, $database );
my $dbId = $dbtable{"LL"}->source_id();
dbxref->executeDelete($dbh, $database, $table, $dbId);


my $count = 0;                             # Count the number of records
my $count_LL = 0;
my $count_RefXM = 0;
my $count_RefXP = 0;
my $KEYID = 0;
my $t1 = time();
while (my $line = <FP>)
{
#    print($line);

    chomp($line);

    if ("!" ne substr($line, 0, 1))
    {
        my @record = split(/\t/, $line);

	my $ll    = $record[0];
	my $refXM = removeVersion($record[1]);
	my $giXM  = $record[2];
        my $review= $record[3];
	my $refXP = removeVersion($record[4]);
	my $taxid = $record[5];

#	print("\n");
#	print("ll    : " . $ll     . "\n");
#	print("refXM : " . $refXM  . "\n");
#	print("giXM  : " . $giXM   . "\n");
#	print("review: " . $review . "\n");
#	print("refXP : " . $refXP  . "\n");
#	print("taxid : " . $taxid  . "\n");
	

	$sth = $dbh->prepare ("select id from " . $database . ".dbxref where record_id='" . $ll ."' and parent_id=0");
	$sth->execute();
	my @llid = $sth->fetchrow_array();
#	{
#	print ("length lld: " .$llid[0] . " : " . length($llid[0]) , "\n");
	if ( length($llid[0]) == 0 )
	{
	    $sth = $dbh->prepare ("INSERT INTO " . $database . ".dbxref VALUES(0, 14, '" . $ll . "', " . 0 . ", " . 0 . ", '', " . 0 . ")" );
	    $sth->execute();
	    $sth = $dbh->prepare ("select max(id) from " . $database . ".dbxref");
	    $sth->execute();
	    if (my @arr = $sth->fetchrow_array())
	    {
	        $KEYID = $arr[0];
#		print("\t\t KEYID: " . $KEYID . "\n");
	    }
		$count_LL++;
	}
	else
	{
	    $KEYID = $llid[0];
	}

#	print("refDNA : " . $refXM . "\n");
#	print("refPROT: " . $refXP . "\n");
	if (length($refXM) > 7 )
	{
	    $sth = $dbh->prepare ("INSERT INTO " . $database . ".dbxref VALUES(0, 2, '" . $refXM . "', " . $KEYID . ", 0, '', 1)");
	    $sth->execute();
	    $count_RefXM++;
	    $sth = $dbh->prepare ("select max(id) from " . $database . ".dbxref");
	    $sth->execute();
	    if (my @link = $sth->fetchrow_array())
	    {
	        my $lnk = $link[0];
		if (length($refXP) > 7 )
		{
		    $sth = $dbh->prepare ("INSERT INTO " . $database . ".dbxref VALUES(0, 2, '" . $refXP . "', " . $KEYID . ", " . $lnk . ", '', " . 2 . ")" );
		    $sth->execute();
		    $count_RefXP++;
		  }
	    }
	  }
#	print("\n");
	
    }
}

my $t2 = time();

close(FP);


$sth->finish();
$dbh->disconnect();
$count = $count_LL + $count_RefXM + $count_RefXP;

print("Execution time : " . ($t2-$t1) . " sec.\n\n");
if( $t2-$t1 > 60)
{
    print("                 " . ($t2-$t1)/60 ." min.\n");
}
print($count . " records Updated into goa_xdf database.\n");
print($count_LL . " LL records\n");
print($count_RefXM . " RefXM records\n");
print($count_RefXP . " RefXP records\n");


exit(0); 



# Fuction removeVersion() remove the version number for a given Accession number.
# Argument: arg1: Accession Number.
# Return Value: Accession number with ".x" version removed.
# Ex: removeVersion(NP_012480.1);
# return NP_012480

sub removeVersion($)
{
    my $acc = $_[0];
    $acc =~ /(\S+).(\d+)/;
    return $1;

}
