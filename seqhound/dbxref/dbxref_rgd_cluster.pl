# $Id: dbxref_rgd_cluster.pl,v 1.2 2005/03/29 20:07:38 rcavero Exp $
# two choises are available - to use DBD-mysql or to use DBI to connect to
# MySQL-db.
# DBI is a most universal one (DBD-mysql is too specific)
use strict;
use Getopt::Long;
use DBI;
use dbxref;


# database stuff
my $user_name = dbxref->getProperty("dbxref.ini", "temp_user");
my $password  = dbxref->getProperty("dbxref.ini", "temp_password");
my $host      = dbxref->getProperty("dbxref.ini", "temp_host");
my $port      = dbxref->getProperty("dbxref.ini", "temp_port");
my $database  = dbxref->getProperty("dbxref.ini", "temp_database");
my $table     = dbxref->getProperty("dbxref.ini", "temp_table");

my $dbh;

my @xrefs;

my $dsn = "DBI:mysql:" . $database . ":" . $host . ":" . $port;
	
###############################################################
# 2) connect to MySQL-database
$dbh = DBI->connect ($dsn, $user_name, $password, {RaiseError => 1});

###############################################################
#Prepare the statement handles.
print ("Preparing the statement handles.\n" );

my %dbtable = dbxref->createDbHashTable( $dbh, $database );
dbxref->prepareID($dbh, $database, $table);
dbxref->prepareInsert($dbh, $database, $table);

###############################################################
# open the dbxref file.

my $data_dir = dbxref->getProperty("dbxref.ini", "data_dir");
my $file2parse = $data_dir . dbxref->getProperty( "dbxref.ini", "XREF_RGD" );

#print ( scalar localtime, "\nOpen the file.\n" );

open(FP, $file2parse) or die "can't open $file2parse: $!";


my $t1 = time();

# =================  CLUSTERING ================================
my $cluster;
my $offset;
my $count2parse;
my $dummy = '';

my $retValue = 0;
$retValue = GetOptions('c=s' => \$cluster, 'o=i' => \$offset,'n=i' => \$count2parse, 'f=s' => \$dummy );

if( ($cluster ne 'T') and ($cluster ne 'F') )
{
      displayMessage('dbxref_rgd_cluster.pl');
}
if ($cluster eq 'T')
{
   if ( (! $offset) and (! $count2parse) )
   {
      displayMessage('dbxref_rgd_cluster.pl');
   }
}

# =============================================================

my %dbtable = dbxref->createDbHashTable( $dbh, $database );
my $dbId = $dbtable{ "RGD"}->source_id();
my $genbank = $dbtable{ "GB"}->source_id();

###############################################################
# Delete any existing values.
#dbxref->executeDelete($dbh, $database, $table, $dbId);

###############################################################
# Begin parsing the file.

# =================  CLUSTERING ================================
my $recCount = 0;
my $l;
if ($cluster eq 'T')
{
   if($offset > 0)
   {
      while ($l = <FP>)
      {
         $recCount++;
         if ($recCount == $offset)
         {
            last;
         }
      }
   }
}
# =============================================================

my $recordCount = 0;
my $rgdCount  = 0;
my $genbankCount = 0;

while( <FP> )
{
    my $parentId = 0;
    my @rowId = 0;
    my $gbId = 0;
    my $rgdId = 0;

    chomp;
    $recordCount++;
    @xrefs = split(/\t/);

    $gbId = $xrefs[ 0 ];
    $rgdId = $xrefs[ 2 ];

    #-------------------------------------------------------------
    # Throw away the first line.
    next if ( $gbId =~ /Genbank/ );

    #-------------------------------------------------------------
    # Prefix the RGD ID with RGD so that we know it's an RGD ID.
	$rgdId = "RGD:" . $rgdId;

    #-------------------------------------------------------------
    # Do we already have a parent id for this id?
    if( length( $rgdId ) > 0 )
    {
	if( @rowId = dbxref->selectID( $dbId, $rgdId, 0 ) )
	{
	    $parentId = $rowId[0];
	}
	else
	{
	    dbxref->executeInsert($dbId, $rgdId, 0, 0, 'Col 3', 0 );

	    if( @rowId = dbxref->selectID( $dbId, $rgdId, 0 ) )
	    {
		$parentId = $rowId[0];
#		print "rgd rowId = $rowId[0]\n";
	    }
	    $rgdCount++;
	}

    }
    else
    {
	print "No rgd id for line $recordCount\n";
    }

    if( length( $gbId ) > 0 )
    {
	dbxref->executeInsert($genbank, $gbId, $parentId, 0, 'Col 1', 1 );
	$genbankCount++;
    }
    else
    {
	print "No genbank id for line $recordCount\n";
    }

# =================  CLUSTERING ================================
    if ($cluster eq 'T')
    {
          if ($recordCount == $count2parse)
          {
             last;
          }
    }
# ===============================================================

}

#print "Finished: Read $lineCount lines, found $rgdCount RGD " .
#"Ids and $genbankCount Genbank Ids\n";

close(FP);

#$dbh->disconnect();

# =================  CLUSTERING ================================
# Generate a dummy file for sincronization.
if ($cluster eq 'T')
{
   if ($dummy)
   {
      open(FPOUT, ">$dummy");
      print(FPOUT "dummy file ".$dummy);
      close(FPOUT);
   }
 }

# =============================================================


my $printResult;
my $node;
# =================  CLUSTERING ================================
if ($cluster eq 'T')
{
   if ($dummy)
   {
      $dummy =~ /(\w+)(\.)/;
      $node = $1;
   }
}
else
{
   $node = "STDALONE";
}
my $t2 = time();
# =============================================================
$printResult = $printResult . "RGD_NODE:\t$node\n";
$printResult = $printResult . "RGD_FILE2PARSE:\t$file2parse\n";
$printResult = $printResult . "RGD_RGD:\t$rgdCount\n";
$printResult = $printResult . "RGD_GB:\t$genbankCount\n";
my $countTotal = $rgdCount + $genbankCount;
$printResult = $printResult . "RGD_$node"."_TOTAL:\t$countTotal\n";
$printResult = $printResult . "RGD_$node"."_TIME:\t" . ($t2-$t1) ."\n";
$printResult = $printResult . "Execution time:\t" . ($t2-$t1)/60 ." min.\n";
$printResult = $printResult . "\n================================================================================\n";
print("$printResult\n");

exit(0);


