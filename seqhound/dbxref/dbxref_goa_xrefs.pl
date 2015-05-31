# $Id: dbxref_goa_xrefs.pl
#Parser reads 3 files that map various database ids to one another. 
#We take 2 ids:
#
#1. IPI ids if we can map them to an existing SPTR parent in dbxref.
#
#2. ENSEMBLE ids as parents. We map cross references to IPI, SPTR,
#   RGD, MGI and genbank ids. 
#
#The input files are:
#
#human.xrefs    ftp.ebi.ac.uk/pub/databases/GO/goa/HUMAN
#mouse.xrefs    ftp.ebi.ac.uk/pub/databases/GO/goa/MOUSE
#rat.xrefs    ftp.ebi.ac.uk/pub/databases/GO/goa/RAT
#
#to run: perl dbxref_zfin.pl
#
#log file: dbxref_zfin.log


# two choises are available - to use DBD-mysql or to use DBI to connect to
# MySQL-db.
# DBI is a most universal one (DBD-mysql is too specific)
use strict;
use DBI;
use dbxref;

###############################################################
#Open a log file
open( LOG, ">dbxref_goa_xrefs.log") or die "Cannot open log file\n";

###############################################################
# database stuff
my $user_name = dbxref->getProperty("dbxref.ini", "user");
my $password  = dbxref->getProperty("dbxref.ini", "password");
my $host      = dbxref->getProperty("dbxref.ini", "host");
my $port      = dbxref->getProperty("dbxref.ini", "port"); 
my $database  = dbxref->getProperty("dbxref.ini", "database");
my $table     = dbxref->getProperty("dbxref.ini", "table");

my $dbh;

my @xrefs;

my $dsn = "DBI:mysql:" . $database . ":" . $host . ":" . $port;
	

###############################################################
# 2) connect to MySQL-database
$dbh = DBI->connect ($dsn, $user_name, $password, {RaiseError => 1});

###############################################################
#Prepare the statement handles.
print ("Preparing the statement handles.\n" );

dbxref->prepareID($dbh, $database, $table);
dbxref->prepareInsert($dbh, $database, $table);

###############################################################
#  get the filenames.

print (LOG scalar localtime, "\nOpen the file.\n" );

my $humanFile = 
    dbxref->getProperty( "dbxref.ini", "XREF_GOA_HUMAN" ); 

my $mouseFile = 
    dbxref->getProperty( "dbxref.ini", "XREF_GOA_MOUSE" ); 

my $ratFile =
    dbxref->getProperty( "dbxref.ini", "XREF_GOA_RAT" );

my %dbtable = dbxref->createDbHashTable( $dbh, $database );

my $sptr    = $dbtable{ "SPTR"}->source_id();
my $ipi     = $dbtable{ "IPI" }->source_id();
my $ensembl = $dbtable{ "ENSEMBL" }->source_id();
my $hugo    = -1;
my $rgd     = $dbtable{ "RGD" }->source_id();
my $mgi     = $dbtable{ "MGI" }->source_id();
my $genbank = $dbtable{ "GB" }->source_id();

###############################################################
# Delete any existing values.
dbxref->executeDelete($dbh, $database, $table, $ensembl);
dbxref->executeDelete($dbh, $database, $table, $ipi);
#die "Finished deleting\n";

###############################################################
# Begin parsing the files.
print (LOG "Parsing the file $humanFile\n" );

my $lineCount = 0;
my $ensemblCount  = 0;
my $genbankCount = 0;
my $sptrCount = 0;
my $mgiCount = 0;
my $rgdCount = 0;

###############################################################
# Parse human.xrefs
parseFile( $humanFile, $hugo );

###############################################################
# Parse mouse.txt
print (LOG "Parsing the file $mouseFile\n" );
parseFile( $mouseFile, $mgi );

###############################################################
# Parse rat.txt
print (LOG "Parsing the file $ratFile\n" );
parseFile( $ratFile, $rgd );


print LOG "Finished: Read $lineCount lines, found :\n".
  "ENSEMBL\t$ensemblCount\nGenbank\t$genbankCount\n" .
    "MGI\t$mgiCount\nRGD\t$rgdCount";

###################################################################
#  ParseFile
#
#  Parses a either a file containing refseq accessions or genbank 
#  accessions.
#
#  @param fileName   Name of the file to be opened.
#
#  @param dbInputId  Id of the genbank database. Either mouse, rat
#                     or human.
#
sub parseFile
{
    my( $fileName, $dbInputId ) = @_;

    open(XREFFILE, $fileName) or die "can't open $fileName: $!";    

    while( <XREFFILE> )
    {
	  my $parentId;
	  my @rowId;

	  chomp;
	  $lineCount++;

	  @xrefs = split(/\t/);

	  ##############################################################
	  # remove the trailing ; if one exists.
	  foreach my $field ( @xrefs )
	  {
		$field =~ s/;\s*$//;
	  }

	  my $db         = $xrefs[ 0 ];
	  my $objectId   = $xrefs[ 1 ];
	  my $ipiId      = $xrefs[ 2 ];
	  my $swissProts = $xrefs[ 3 ];
	  my $ensembls  = $xrefs[ 4 ];
	  my $NPRefseq   = $xrefs[ 5 ];
	  my $XPRefseq   = $xrefs[ 6 ];
	  my $orgId      = $xrefs[ 8 ];
	  my @Ids;

	  $lineCount++;
	 # print "Reading line $db, $objectId, $ipiId, $swissProts, " .
		#"$ensembls, $NPRefseq, $XPRefseq, $orgId\n";

	  ##############################################################
	  #if the database is swissprot or trembl and we already have
	  # a parent id, insert an ibiId child.
	  #if( $db =~ /SP/ || $db =~ /TR/ )
	  {
		my @row = dbxref->selectID( $sptr, $objectId, 0 );

		if( length( $row[ 0 ] ) > 0  )
		{
		  if( length( $ipiId) > 0 )
		  {
			dbxref->executeInsert( 0, $ipi, $ipiId, $row[ 0 ], 0, 
								   'Col 1', 0 );
		  }
		}
		else
		{
		  print LOG "SP $objectId for $orgId not in database.\n";
		  $sptrCount++;
		}
	  }
	  if( $db =~/ENSEMBL/ )
	  {
		  $ensemblCount++;
		if( length( $objectId ) > 0 )
		{
		  $parentId = 
			InsertRecord ( $ensembl, $objectId, 0, $ensemblCount );

		  if( $parentId > 0 )
		  {
			my $newRecord;
			
			if( length( $ipiId) > 0 )
			{
			  my $ipiCount;
			  InsertRecord( $ipi, $ipiId, $parentId, $ipiCount++ );
			}
			if( length( $swissProts ) > 0 )
			{
#			  print LOG "found $swissProts for ENSEMBL entry\n";
			  my $Id;
			  @Ids = split(/;/, $swissProts);

			  foreach $Id ( @Ids )
			  {
				InsertRecord( $sptr, $Id, $parentId, $sptrCount++ );
			  
			  }
			}
			if( length( $ensembls ) > 0 )
			{
			  my $Id;
			  @Ids = split(/;/,$ensembls);

			  foreach $Id ( @Ids )
			  {
				#print "Found ENSEMBL $Id for $objectId\n";
				
			  
			  }
			}
			if( length( $NPRefseq ) > 0 )
			{
				InsertRecord( $genbank, $NPRefseq, $parentId, $genbankCount++ );
			}

			if( length( $XPRefseq ) > 0 )
			{
				InsertRecord( $genbank, $XPRefseq, $parentId, $genbankCount++ );
			}

			if( $dbInputId != $hugo && ( length( $orgId ) > 0 ) )
			{
			  my @Ids = split(/\,/, $orgId);

			  my $Id = $Ids[ 0 ];

			  if( $Id =~ /MGI/ )
			  {
				InsertRecord( $mgi, $Id, $parentId, $mgiCount );
			  }
			  else
			  {
				$Id = s/^/RGD:/;
				InsertRecord( $rgd, $Id, $parentId, $rgdCount);
			  }

			}
		  }
		  else
		  {
			"print Failed to insert parent for ENSEMBL $objectId\n";
		  }
		}

	  }
    }
}

###################################################################
#  InsertRecord
#
#  Checks to see if a given Id already exists. If it
#  doesn't then add it. The Id of the record is returned.
#  
#  @param $dbId     - database id of the database to which this 
#                     object id belongs.
#
#  @param $objectId - Id to insert.
#
#  @param $parentId - parent of this record
#
#  @param $count    - counter to be incremented
#
#  @return $Id of the parent record.

sub InsertRecord
{
    my ($dbId, $objectId, $parentId, $count) = @_;
    my @rowId;
    my $newId;
    
    if( @rowId = dbxref->selectID( $dbId, $objectId, $parentId ) )
    {
	  $newId = $rowId[0];
    }
    else
    {
	  dbxref->executeInsert( 0, $dbId, $objectId, $parentId, 
			       0, 'Col 1', 0 );
	
	  if( @rowId = dbxref->selectID( $dbId, $objectId, $parentId ) )
	  {
	    $newId = $rowId[0];

		if( $newId > 0 )
		{
		  $count++;
		}
	  }
    }

    if( $newId <= 0 )
    {
	  print LOG "InsertRecord:Failed to find rowId for $dbId:$objectId with parent $parentId.\n";
    }

    return $newId;
}

# $Log: dbxref_goa_xrefs.pl,v $
# Revision 1.5  2004/07/27 17:41:58  rcavero
# There was a bug in line 252 $id instead of $Id.
#
# Revision 1.4  2004/06/17 15:59:57  rcavero
# goa updates
#
# Revision 1.3  2004/06/01 20:24:37  eburgess
# Fixed bug in entering additional swissprot Ids. Bugzilla 816.
#
















