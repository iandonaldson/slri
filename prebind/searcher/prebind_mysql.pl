#! /usr/bin/perl -w

# =============================================================================
# File Description: 
#       Use MySQL for prebind.
# Command syntax: 
#       perl prebind_mysql.pl [-options] <infile >outfile 2>>errfile
# ==============================================================================

use strict;
use DBI;

# process command line arguments
use Getopt::Long;
use Term::ReadKey;

#no output buffer
use IO::Handle;
STDOUT->autoflush(1);

my $opt;
Getopt::Long::Configure ('bundling');
if ( @ARGV > 0 ) {
	GetOptions ('o=s' => \$opt);
}

my $host = "10.0.0.122";
my $port = "33306";
my $db = "kzheng";
my $user_name = "kzheng";
my $password = "asdf1234";

# connect to MySQL-database
my $dsn = "DBI:mysql:$db:" . $host . ":" . $port;
my $dbh = DBI->connect ($dsn, $user_name, $password, 
			{RaiseError => 1, AutoCommit => 1});

# do the option here
if($opt eq "R")
{
    create_RESULT_table();
}
elsif($opt eq "r")
{
    insert_RESULT_table();
}
elsif($opt eq "M")
{
     create_MYMEDLINE_table();
}

elsif($opt eq "m")
{
    insert_MYMEDLINE_table();
}
elsif($opt eq "g")
{
    get_pmid_from_RESULT();
}
else
{
 print "invalid command \n";
}

# clear up
$dbh->disconnect();
print($count . " records Updated into database.\n");
exit(0); 

sub create_RESULT_table{ #$opt = R
    
    my $sth = $dbh->prepare("DROP TABLE IF EXISTS $db.RESULT;");
    $sth->execute();
    $sth = $dbh->prepare("CREATE TABLE IF NOT EXISTS $db.RESULT
	    (gi_prot INTEGER UNSIGNED NOT NULL, 
	    name VARCHAR(40) NOT NULL, 
	    pmid INTEGER UNSIGNED NOT NULL,
        PRIMARY KEY(gi_prot, name, pmid),)
        Index i_pmid (pmid),
        Index i_gi_prot(gi_prot),
        Index i_name (name)");
    $sth->execute();
}

sub insert_RESULT_table{ #$opt = r
    # using placeholder for insert to improve performance
    my $sth = $dbh->prepare(q{
        INSERT INTO $db.RESULT (GI_PROT, NAME, PMID) VALUES (?, ?, ?)
      }) or die $dbh->errstr;
    my $count = 0; # Count the number of records
    while (<>) {
        chomp;
        if (/^(\d+)\t(\S+)\t(\d+)/)
	    {
		    $count++;
		    my ($gip, $name, $pmid) = split /\t/;
		    $sth->execute($gip, $name, $pmid) or die $dbh->errstr;
	    }#if()
    }
}

sub get_pmid_from_RESULT{ 
    my $sth = $dbh->prepare(q{
        select DISTINCT PMID from $db.RESULT ORDER BY PMID
      }) or die $dbh->errstr;
    $sth->execute()
             or die "Couldn't execute statement: " . $sth->errstr;

    my $count = 0; # Count the number of records
    my @data;
    while (@data = $sth->fetchrow_array()) {
	    $count++;
	    my $pmid_match=$data[0];
	    print "$pmid_match\n";
    }

    print "---\n$count\n";
}

sub create_MYMEDLINE_table{
    my $sth = $dbh->prepare("DROP TABLE IF EXISTS $db.MYMEDLINE;");
    $sth->execute();
    my $sth = $dbh->prepare("CREATE TABLE IF NOT EXISTS $db.MYMEDLINE
	    (pmid INTEGER UNSIGNED NOT NULL,
        muid INTEGER UNSIGNED NOT NULL KEY DEFAULT 0,
	    name VARCHAR(1) NOT NULL DEFAULT 'N',
        insertdate DATE NOT NULL,
        PRIMARY KEY (pmid),
        INDEX (pmid))");
    $sth->execute();
}

sub insert_MYMEDLINE_table{
    # using placeholder for insert to improve performance
    my $sth = $dbh->prepare(q{
        INSERT INTO $db.MYMEDLINE (GI_PROT, NAME, PMID) VALUES (?, ?, ?)
      }) or die $dbh->errstr;
    my $count = 0; # Count the number of records
    while (<>) {
        chomp;
        if (/^(\d+)\t(\S+)\t(\d+)/)
	    {
		    $count++;
		    my ($gip, $name, $pmid) = split /\t/;
		    $sth->execute($gip, $name, $pmid) or die $dbh->errstr;
	    }#if()
    }
}



#

# -------------------------------------------------------------------
#   initialize mysql search table connection 
# -------------------------------------------------------------------
my $dsn_mysql = "DBI:mysql:search:marge:33306";
my $dbh_mysql = DBI->connect ($dsn_mysql, "kzheng", "asdf1234", 
			{RaiseError => 1, AutoCommit => 1});
print "\nconnect to mysql ...\n";
print "select from search table ...\n";
my $sth_mysql = $dbh_mysql->prepare(
		"select s.searchid, b.name
		from search s, bioname b
		where taxid=$taxid AND s.bionameid=b.id");
$sth_mysql->execute();

$sth_mysql->finish;
$dbh_mysql->disconnect;



