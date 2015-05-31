# $Id: dbxref_ddb_cluster.pl,v 1.2 2005/03/29 20:04:38 rcavero Exp $
# two choises are available - to use DBD-mysql or to use DBI to connect to
# MySQL-db.
# DBI is a most universal one (DBD-mysql is too specific)
use strict;
use DBI;
use Getopt::Long;
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
dbxref->prepareID($dbh, $database, $table);
dbxref->prepareInsert($dbh, $database, $table);

###############################################################
# open the dbxref file.

my $data_dir = dbxref->getProperty("dbxref.ini", "data_dir");
my $file2parse = $data_dir . dbxref->getProperty( "dbxref.ini", "XREF_DDB" );

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
      dbxref->displayMessage('dbxref_ddb_cluster.pl');
}
if ($cluster eq 'T')
{
   if ( (! $offset) and (! $count2parse) )
   {
      dbxref->displayMessage('dbxref_ddb_cluster.pl');
   }
}
#print("fileName: $file2parse \n");
#print("cluster: $cluster \n");
#print("offset: $offset \n");
#print("count2parse: $count2parse \n");
#print("dummy: $dummy \n");

#die("die test\n");

# =============================================================


my %dbtable = dbxref->createDbHashTable( $dbh, $database );
my $dbId = $dbtable{ "DDB"}->source_id();
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


my $lineCount = 0;
my $ddbCount  = 0;
my $genbankCount = 0;

#print ("Parsing the file\n" );


while( <FP> )
{
    my $parentId;
    my $rowId;
    my $gbId;
    my $ddbId;

    chomp;

    $lineCount++;

    @xrefs = split(/\t/);

    $ddbId = $xrefs[ 0 ];
    $gbId = $xrefs[ 1 ];

    #-------------------------------------------------------------
    # Do we already have a parent id for this id?
    if( length( $ddbId ) > 0 )
    {
	if( $rowId = dbxref->selectID( $dbId, $ddbId, 0 ) )
	{
	    $parentId = $rowId;
	}
	else
	{
	    dbxref->executeInsert($dbId, $ddbId, 0, 0, 'Col 1', 0 );

	    if( $rowId = dbxref->selectID( $dbId, $ddbId, 0 ) )
	    {
		$parentId = $rowId;
	    }
	    $ddbCount++;
	}
    }
    else
    {
	print "No ddb id for line $lineCount\n";
    }

    if( length( $gbId ) > 0 )
    {
	dbxref->executeInsert($genbank, $gbId, $parentId, 0, 'Col 2', 0 );
	$genbankCount++;
    }
    else
    {
	print "No genbank id for line $lineCount\n";
    }

# =================  CLUSTERING ================================
    if ($cluster eq 'T')
    {
	  if ($lineCount == $count2parse)
	  {
	     last;
	  }
    }
# ===============================================================

}

close(FP);
dbxref->finishSelectID();
dbxref->finishInsert();
$dbh->disconnect();

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
$printResult = $printResult . "DDB_NODE:\t$node\n";
$printResult = $printResult . "DDB_FILE2PARSE:\t$file2parse\n";
$printResult = $printResult . "DDB_DDB:\t$ddbCount\n";
$printResult = $printResult . "DDB_GB:\t$genbankCount\n";
my $countTotal = $ddbCount + $genbankCount;
$printResult = $printResult . "DDB_$node"."_TOTAL:\t$countTotal\n";
$printResult = $printResult . "DDB_$node"."_TIME:\t" . ($t2-$t1) ."\n";
$printResult = $printResult . "Execution time:\t" . ($t2-$t1)/60 ." min.\n";
$printResult = $printResult . "\n================================================================================\n";
print("$printResult\n");

exit(0);


# $Log: dbxref_ddb_cluster.pl,v $
# Revision 1.2  2005/03/29 20:04:38  rcavero
# Add command for update procedures.
#
# Revision 1.1  2005/01/27 21:29:51  zwang
# Make compliant with parallel computing.
#
