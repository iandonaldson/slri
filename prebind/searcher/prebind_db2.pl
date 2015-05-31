#!/usr/bin/perl -w

# =============================================================================
# File Description: 
#       Use in-house medline (MYMEDLINE) on db2.
# Command syntax: 
#       perl prebind_db2.pl [-options] <infile >outfile 2>>errfile
# ==============================================================================

use strict;

# process command line arguments
use Getopt::Long;
use Term::ReadKey;

# DB2 setting stuff
use lib "/export/home/db2inst1/usr/local/perl5/site_perl";
use DBI;
use POSIX qw(locale_h);
use locale;
# Must have UTF-8 locale installed on machine as well as set here
setlocale(LC_ALL, "en_US.UTF-8");

use IO::Handle;
STDOUT->autoflush(1);

# define program variables
my $starttime = time();
my $opt;
Getopt::Long::Configure ('bundling');
if ( @ARGV > 0 ) {
	GetOptions ('o=s' => \$opt);
}

# initialize MyMed db2 connection
# Get username and password
my $dbname   = 'dbi:DB2:medline';
my ($user, $password);
my $dbh;

print "program starts at:", scalar localtime, "\n\n";

print STDERR "Enter db2 user  name>";
$user = <STDIN>;
chomp($user);
print STDERR "Enter password>";
ReadMode('noecho'); # don't echo
$password = <STDIN>;
chomp($password);
ReadMode(0);        # back to normal
print STDERR "\nconnect to db2 ...\n";

# Set up a database handle
$dbh = DBI->connect($dbname,$user,$password, { AutoCommit => 1 }) 
   or die "Error connecting $DBI::errstr\n";

# LongReadLen controls the maximum length of any long data (CLOBS), this should be set to
# slightly larger than the longest field value to be fetched. 
$dbh->{LongReadLen}=20*1024;

# Allows truncation of CLOBS if they are longer than LongReadLen, if set to 0 an error will
# result instead of a warning.
$dbh->{LongTruncOk}=1;

export_citations();

# clean up
$dbh->disconnect;
print "program stops at:", scalar localtime, "\n\n";

sub export_citations
{
    # Create SQL statement to call stored procedure
    my $stmt="SELECT pmid, status, cid ".   
		    "FROM medline.citations ".
		    "ORDER BY pmid;"; 

    #prepare the statement
    my $sth;
    $sth=$dbh->prepare($stmt) or die "Error preparing statement: $DBI::errstr\n";

    # Execute the SQL statement 
    $sth->execute()or die "Error executing statement: $DBI::errstr\n";

    # Fetch the resulting records and print them  to the put file
    my @data;
    my $count =0;
    while (@data = $sth->fetchrow_array()) {
	    my $pmid=$data[0];
	    my $status = $data[1];
	    my $cid= $data[2];
	    #my $insertdate=$data[3];
        $count++;
        print  "$pmid\t$status\t$cid\n";
        #print  "$pmid\t$status\n";
    }
    $sth->finish;
    print  "-$count-\n"

}
