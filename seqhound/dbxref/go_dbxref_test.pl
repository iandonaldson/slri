#selects database Ids from from a gene_association file
# and searches the dbxref table for a genbank id.
#
#Usage: perl go_dbxref_test.pl gene_association.file
#

use strict;
use DBI  qw(:sql_types);
use dbxref;
use Getopt::Std;
use SeqHound;

###############################################################
#Open a log file
open( LOG, ">go_dbxref_test.log") or die "Cannot open log file\n";

#open( GIFILE, ">gi_go.txt") or die "Cannot open gi_go.txt\n";

###############################################################
#Open the gene association file
#getopts("i:");

my $gFileToProcess  = $ARGV[0]; # = $opt_i;

open( GAFILE, $gFileToProcess ) or die 
    "Cannot open gene association file" . $gFileToProcess;

print LOG "\n\nFILE " . $gFileToProcess . "\n\n";

###############################################################
# database stuff

my $host = "10.0.0.122";
my $port = "43306";
my $database = "dbxrefnew";   # seqhound;
my $table = "dbxref";
my $user_name = "eburgess";
my $password = "asdf1234";

my $dbh;

my $dsn = "DBI:mysql:" . $database . ":" . $host . ":" . $port;
	
#connect to MySQL-database
$dbh = DBI->connect ($dsn, $user_name, $password, {RaiseError => 1});

my %dbtable = dbxref->createDbHashTable( $dbh, $database );
my $genbank = $dbtable{ "GB" }->source_id();
my $sptr    = $dbtable{ "SPTR" }->source_id();
###############################################################
#Prepare the statement handles.

print LOG scalar localtime, "\n Preparing statement handles\n";
print "\n Preparing statement handles\n";

dbxref->prepareID( $dbh, $database, $table );

my $sql;
my $getChildren;
my $getAllRecords;
my $getGBId;

$sql  = "SELECT * FROM dbxref WHERE parent_id = ?";

$getChildren = $dbh->prepare( $sql );

$sql  = "SELECT * FROM dbxref WHERE record_id = ?";

$getAllRecords = $dbh->prepare( $sql );

$sql  = "SELECT record_id FROM dbxref WHERE ";
$sql .=          "source_id = $genbank AND ";
$sql .=          "parent_id = ?";

$getGBId = $dbh->prepare( $sql );

###############################################################
#Read the file
print "Reading the file\n";

my $objectCount    = 0;
my $notFoundCount  = 0;
my $lineCount      = 0;
my $genbankCount   = 0;
my $noGenbankCount = 0;

my $oldObjectId;
my $gi;
while( <GAFILE> )
{
    my $db;
    my $objectId;

    my $source_db;
    my $source_id;
    my $parent_id;
    my $genbank_id;

    my $evidenceCode;
    my $goid;

    my @line;

    ############################################################
    #   skip comment lines
    if (  /^!/ )
    {
	next;
    }

    $lineCount++;
    chomp;
    
    @line = split( /\t/ );

    $db = $line[0];
    $objectId = $line[1];
    
    $goid = $line[4];
    $goid =~ s/GO://;
    $evidenceCode = $line[6];

	print "Reading line: $db, $objectId, $goid, $evidenceCode\n";
    ############################################################
    # If we've already examined this objectId, go to the next line
    if( $objectId eq $oldObjectId )
    {
		print GIFILE $gi . "\t" . $goid . "\t" . 
	    $evidenceCode . "\n";
		next;
    }
 
    $oldObjectId = $objectId;
    $objectCount++;

    ##########################################################
    # Assign the source id.
    if( $db =~ /UniProt|Swiss-Prot|SWISS-PROT|SWP|SP|SPTR|SWALL/)
    {
	$source_id = $dbtable{ "SPTR" }->source_id();
    }
    elsif( $db =~ /FB/ )
    {
	$source_id = $dbtable{ "FB" }->source_id();
    }
    elsif( $db =~ /MGI/ )
    {
	$source_id = $dbtable{ "MGI" }->source_id();
    }
    elsif( $db =~ /SGD/ )
    {
	$source_id = $dbtable{ "SGD" }->source_id();
    }
    elsif( $db =~ /WB/ )
    {
	$source_id = $dbtable{ "WB" }->source_id();
    }
	elsif( $db =~ /DDB/i )
	{
		$source_id = $dbtable{ "DDB" }->source_id();
	}
	elsif( $db =~ /ZFIN/i )
	{
		$source_id = $dbtable{ "ZFIN" }->source_id();
	}
	elsif( $db =~ /TIGR_CMR/i)
	{
		$source_id = $dbtable{ "TIGR_CMR" }->source_id();
	}
	elsif( $db =~ /TIGR_ATH/i)
	{
		$source_id = $dbtable{ "TIGR_ATH" }->source_id();
	}
	elsif( $db =~ /RGD/i )
	{
		$source_id = $dbtable{ "RGD" }->source_id();
	}
	elsif( $db =~ /ENSEMBL/ )
	{
		$source_id = $dbtable{ "ENSEMBL" }->source_id();
#		 print "Database is $db $source_id\n";
	}
	elsif( $db =~ /RefSeq/ )
	{
		$source_id = $dbtable{ "REFSEQ" }->source_id();
	}
	elsif( $db =~ /GeneDB_Spombe/i )
	{
	 
		$source_id = $dbtable{ "GENEDB_SPOMBE" }->source_id();

	}
	elsif( $db =~ /TIGR_CMR/i )
	{
		$source_id = $dbtable{ "TIGR_CMR" }->source_id();
	}
	elsif( $db =~ /GR/ )
    {
	  $source_id = $dbtable{ "SPTR" }->source_id();
#		 print "Database is $db $source_id\n";
    }
	elsif( $db =~ /VIDA/i )
	{
		$source_id = $dbtable{ "VIDA" }->source_id();
	}
	elsif( $db =~ /TAIR/i )
	{
		$source_id = $dbtable{ "TAIR" }->source_id();
	}
    else
    {
	  print "Database abbreviation $db not recognized  for $objectId.\n";
	  print LOG "Database abbreviation $db not recognized for $objectId.\n";

    }

#   print "Source $source_id, $objectId\n";
    ###################################################
	#Is the ID already a Genbank ID?
	if( $source_id == 2 )
	{
	  $genbankCount++;
	  next;
	}

	###################################################
    #Get all records.
	$getAllRecords->bind_param( 1, $objectId );
	$getAllRecords->execute();

	my $gbId = 0;

	my @records = $getAllRecords->fetchrow_array();

	if( @records )
	{
	  my $recordCount = 0;

	  foreach( @records )
	  {
		my $id         = $records[ 0 ];
		my $source_id  = $records[ 1 ];
		my $record_id  = $records[ 2 ];
		my $parent_id  = $records[ 3 ];

		$recordCount++;

		###################################################
		# If this record is a parent, see if any of the 
		# children are from genbank.
		if( $parent_id == 0 )
		{
		  $gbId = GetGenbankIdFromChildren( $id );
		}
		else
		{
		  ###################################################
		  #Try to get the genbank id from the parent.
		  $gbId = GetGenbankIdFromChildren( $parent_id );
		}
		
		if( length( $gbId ) > 0 )
		{
		  $genbankCount++;
		  last;
		}
	  }
	}
	else
	{
	  $notFoundCount++;
	  print LOG "$objectId is not found in DBXREF.\n";
	}

	if( length( $gbId ) <= 0 )
	{
	  print LOG "Genbank ID not found for $db $objectId\n";
	  $noGenbankCount++;
	}
	
#    print "$lineCount: ObjectIds - $objectCount, " .
#	"GenbankIds - $genbankCount, " .
#	    "not found = $noGenbankCount, no parent - " .
#	       "$notFoundCount\n";
  }

print LOG "\n\nRead $lineCount lines\n";
print LOG "Found $objectCount object ids\n";
print LOG "Found $genbankCount genbank records\n";
print LOG "Unable to find any records for $notFoundCount ids\n";
print LOG "Unable to find $noGenbankCount genbank records\n";
my $percent = ( $genbankCount/$objectCount ) * 100;

print LOG "Found genbank ids for $percent of object ids\n\n";
print LOG  scalar localtime, "\n";

sub GetGenbankIdFromChildren()
{
  my( $parent_id ) = @_;
  my $gbId = 0;

  $getChildren->bind_param( 1, $parent_id, SQL_INTEGER );
  $getChildren->execute();

  my @children = $getChildren->fetchrow_array();

  foreach( @children )
  {

	my $childSource = $children[ 1 ];
	my $childId     = $children[ 2 ];
	 
	if( my $childSource == $genbank )
	{
	  $gbId = $childId;
	  last;
	}
	else
	{
	  ############################################################
      #See if this child has any other parents who might have
	  #children with a genbank id.
	  my @parent = dbxref->selectID( $childSource, $childId, 0 );
		
	  if( $parent[ 0 ] > 0 )
	  {
		$getGBId->bind_param( 1, $parent_id, SQL_INTEGER );
		$getGBId->execute();

		my @rowId = $getGBId->fetchrow_array();

		$gbId = $rowId[0];
	  }
	}
	if( length( $gbId ) > 0 )
	{
	  last;
	}
  }

  return $gbId;
}
















