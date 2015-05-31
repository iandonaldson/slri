#! /usr/bin.perl

#--------------------------------------------------------------------------
# parser for Xref from OMIM & HUGO ==> LocusLink
# DB are : 1 OMIM, 2 HUGO
# 
# the files come from: ftp://ftp.ncbi.nih.gov/refseq/LocusLink/mim2loc
#                      ftp://ftp.ncbi.nih.gov/refseq/LocusLink/loc2UG
#
# The input file format(tab-delimited), but
# For mim2loc: Column1= OMIM | Column2=LL
# For loc2UG:  Column1= LL   | Column2=HUGO
#--------------------------------------------------------------------------
use strict;
use DBI;
use dbxref;

###############################################################
#Open a log file
# TODO
#open( LOG, ">dbxref_zfin.log") or die "Cannot open log file\n";


# mysql set up stuff
my $host      = dbxref->getProperty("dbxref.ini", "host");
my $port      = dbxref->getProperty("dbxref.ini", "port");
my $database  = dbxref->getProperty("dbxref.ini", "database");
my $table     = dbxref->getProperty("dbxref.ini", "table");
my $user_name = dbxref->getProperty("dbxref.ini", "user");
my $password  = dbxref->getProperty("dbxref.ini", "password");
my $dsn = "DBI:mysql:$database:$host:$port";

###############################################################
#  connect to MySQL-database
my $dbh = DBI->connect ($dsn, $user_name, $password, {RaiseError => 1});

###############################################################
# Prepare the statement handles.
# use functions in dbxref.pm to prepare for the update
dbxref->prepareID($dbh, $database, $table);
dbxref->prepareInsert($dbh, $database, $table);


###############################################################
#  get the filenames.
my $mim2locFile =  dbxref->getProperty( "dbxref.ini", "XREF_LL_OMIM" );
my $loc2UGFile  =  dbxref->getProperty( "dbxref.ini", "XREF_LL_UNIGENE" );
my $file2parse;

###############################################################
# get source db hash table
my %dbtable = dbxref->createDbHashTable($dbh, $database);
my $LL = $dbtable{LL}->source_id();
my $UNIGENE = $dbtable{UNIGENE}->source_id();
my $OMIM = $dbtable{OMIM}->source_id();
my $sourceID = 0;
my $db;


###############################################################
# Set up conters.
my $countLL      = 0;
my $countUNIGENE = 0;
my $countOMIM    = 0;


if ( @ARGV > 0 )
{
    $db = $ARGV[0];
}
else
{
   print "Usage : perl dbxref_LL_others OMIM\n";
   print "Usage : perl dbxref_LL_others UNIGENE\n";
   die("Please enter the right command line argument.\n");
}

# parse dbxref file for individual organism, one at a time
if($db eq "OMIM")
{
    $file2parse = $mim2locFile;
    $sourceID = $OMIM;
}
elsif($db eq "UNIGENE")
{
    $file2parse = $loc2UGFile;
    $sourceID = $UNIGENE;
}
else
{
    die("Please enter the right command line argument: OMIM or UNIGENE.\n");
}


open(FP, $file2parse) or die "can't open $file2parse: $!";

while(my $line = <FP>)
{
    my $LLid = ();
    my $xref = ();
    chomp($line);
    my @xrefs =split(/\t/, $line);

    if ($db eq "OMIM")
    {
        $xref = "MIM:" . $xrefs[0];
	$LLid = $xrefs[1];
	$countOMIM++;
    }
    elsif($db eq "UNIGENE")
    {
        $LLid = $xrefs[0];
	$xref = $xrefs[1];
	$countUNIGENE++;
    }

    @xrefs=();

#    print("LL: $LLid \t Xref: $xref\t");
    my @rowID = dbxref->selectID($LL, $LLid, 0);
#    print("rowid: $rowID[0]\n");
    my $id = $rowID[0];
    if( $rowID[0] <=0 )
    {
        # IF LL id does not exist insert it and get it's id.
#        printf("Insert LL ID because was not found\n");
	dbxref->executeInsert(0, $LL, $LLid, 0, 0, "", 0);
	$countLL++;
	@rowID = ();
	@rowID = dbxref->selectID($LL, $LLid, 0);
	$id = $rowID[0];
    }
    dbxref->executeInsert(0, $sourceID, $xref, $id, 0, "", 0);
}


#$dbxref->sth_finish();
$dbh->disconnect();
my $count = $countLL + $countUNIGENE + $countOMIM;
print($count . " records Updated into goa_xdf database.\n");

print("LL:      " . $countLL      . "\n");
print("OMIM:    " . $countOMIM    . "\n");
print("UNIGENE: " . $countUNIGENE . "\n");
print("dbxref_LL_others finish.\n\n");


exit(0);


