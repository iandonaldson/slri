#!/usr/bin/perl 
#Parses 4 files from ftp://ftp.ncbi.nih.gov/gene/DATA
#    1.gene2pubmed.gz
#    2.gene2refseq.gz
#    3.gene_history.gz
#    4.gene_info.gz
#
#Usage: perl parse_gene_files.pl arg
#If arg provided as "update", will delete all existing records, 
#otherwise, do initial build
#
#log file: gene_files.log
#

use strict;
use DBI qw(:sql_types);

my $INNO = "innodb";
my $ISAM = "myisam";

###############################################################
#Take in argument,  
#different "create table" for different storage engines
my $engine = @ARGV[0];
if ($engine ne $INNO && $engine ne $ISAM) {
	print "Please provide $INNO or $ISAM as the first parameter\n";
	exit(0);
} 

#If provided as "update", will delete all existing records, 
#otherwise, do initial build
my $is_update = "FALSE";
if (@ARGV[1] eq "update") {
	$is_update = "TRUE";
} 

###############################################################
#Open a log file
open( LOG, "> parse_gene_files.log") or die "Cannot open log file\n";

###############################################################
# database stuff
my $user_name;
my $password;
my $host;
my $port;
my $alias;
my $database;

ReadConfigFile();

print "$user_name using $database on $host:$port.\n";

my $dbh;

my $dsn = "DBI:mysql:" . $database . ":" . $host . ":" . $port;

###############################################################
# 2) connect to MySQL-database
print (LOG scalar localtime,  "\nOpening the database.\n" );
$dbh = DBI->connect ($dsn, $user_name, $password, {RaiseError => 1, AutoCommit => 0});

print "Connected\n";
###############################################################
#delete all existing records in the tables if this is an update.
#
if ($engine eq $INNO && $is_update eq "TRUE") {
	DeleteAllRecords();
}
else {
	ResetTables();
}

###############################################################
#Open the input files
print (LOG "Open the files.\n" );

my $pubmedFile = "gene2pubmedUniq";
my $refseqFile = "gene2refseqUniq";
my $infoFile = "gene_infoUniq";
my $historyFile = "gene_historyUniq";

open( REFSEQFILE, $refseqFile ) or die "Can't open $refseqFile : $!";
open( INFOFILE, $infoFile ) or die "Can't open $infoFile : $!";
open( HISTORYFILE, $historyFile ) or die "Can't open $historyFile : $!";
open( PUBMEDFILE, $pubmedFile ) or die "Can't open $pubmedFile : $!";

###############################################################
#Prepare the statement handles.
print (LOG "Preparing the statement handles.\n" );
print ("Preparing the statement handles.\n" );

my $sql;
my $geneObjectInserth;
my $genomicGiInserth;
my $productGiInserth;
my $pubmedInserth;
my $infoInserth;
my $historyInserth;
my $synonymInserth;
my $dbxrefInserth;
my $getObjectIdh;
my $getInfoIdh;
my $getGenomicIdh;

$sql  = "INSERT IGNORE INTO gene_object( id, geneid, status ) ";
$sql .=                          "VALUES( ?,?,? )";

$geneObjectInserth = $dbh->prepare( $sql );

$sql  = "INSERT IGNORE INTO gene_genomicgi( id, geneobjectid, gi, ";
$sql .=                        "start, end, orientation ) ";
$sql .=                        "VALUES( ?,?,?,?,?,? )";

$genomicGiInserth = $dbh->prepare( $sql );

$sql  = "INSERT IGNORE INTO gene_productgi( id, geneobjectid, gi )"; 
$sql .=                          "VALUES( ?,?,? )"; 

$productGiInserth = $dbh->prepare( $sql );

$sql  = "INSERT INTO gene_info( id, geneobjectid, symbol, ";
$sql .=                       "locustag, chromosome, ";
$sql .=                       "maplocation, description ) ";
$sql .=                       "VALUES( ?,?,?,?,?,?,? )";

$infoInserth = $dbh->prepare( $sql );

$sql  = "INSERT INTO gene_history( id,taxid, currentgeneid, ";
$sql .=                            "oldgeneid, oldsymbol )";
$sql .=                            "VALUES( ?,?,?,?,? )";

$historyInserth = $dbh->prepare( $sql );

$sql  = "INSERT INTO gene_pubmed( id, geneobjectid , pmid ) ";
$sql .=                         "VALUES( ?,?,? )";

$pubmedInserth = $dbh->prepare( $sql );

$sql  = "INSERT INTO gene_dbxrefs( id, geneinfoid, DBName, DBAccess )";
$sql .=                            "VALUES( ?,?,?,? )";

$dbxrefInserth = $dbh->prepare( $sql );

$sql  = "INSERT INTO gene_synonyms( id, geneinfoid, synonym )";
$sql .=                            "VALUES( ?,?,? )";

$synonymInserth = $dbh->prepare( $sql );

$sql  = "SELECT id FROM gene_info WHERE geneobjectid = ?";

$getInfoIdh = $dbh->prepare( $sql );

$sql  = "SELECT id from gene_object WHERE geneid = ?";

$getObjectIdh = $dbh->prepare( $sql );

$sql  = "SELECT id from gene_genomicid WHERE geneid = ? ";
$sql .=                               "AND geneobjectid = ?";
$sql .=                               "AND gi = ?";
$sql .=                               "AND start = ?";
$sql .=                               "AND end = ?";
$sql .=                               "AND orientation = ?";


$getGenomicIdh = $dbh->prepare( $sql );

###############################################################
#Parse gene2refseq
print (LOG "Parsing gene2refseq\n" );
print ( "Parsing gene2refseq\n" );
my $lineCount = 0;
my $recordCount = 0;

while( <REFSEQFILE> )
{
  chomp;

  my @fields = split(/\t/);

###############################################################
# if certain fields are represented by a dash set them to undef.
# Note that we do not want to replace the "-" for
# the orientation field.
  my @newFields;
  for( my $i = 0; $i < 11; $i ++ )
  {
	push( @newFields, $fields[ $i ] );
  }

  RemoveDashes( @newFields );

  my $tax_id         = $newFields[ 0 ];
  my $gene_id        = $newFields[ 1 ];
  my $status         = $newFields[ 2 ];
  my $rna_access     = $newFields[ 3 ];
  my $rna_gi         = $newFields[ 4 ];
  my $protein_access = $newFields[ 5 ];
  my $protein_gi     = $newFields[ 6 ];
  my $genomic_access = $newFields[ 7 ];
  my $genomic_gi     = $newFields[ 8 ];
  my $genomic_start  = $newFields[ 9 ];
  my $genomic_end    = $newFields[ 10 ];
  my $orientation    = $fields[ 11 ];

  my $geneObjectId;

  $lineCount++;

#  print "line $tax_id, $gene_id, $status, $rna_access, $rna_gi " .
#	    "$protein_access, $protein_gi, $genomic_access, " .
#		"$genomic_gi, $genomic_start, $genomic_end, $orientation\n";
  if( length( $gene_id ) > 0 )
  {
	$geneObjectId = InsertGeneObject( $gene_id, $status );

	if( $geneObjectId > 0 )
	{
	  $recordCount++;

	  if( length( $rna_gi ) > 0 )
	  {
		my $insert = InsertProductGi( $geneObjectId, $rna_gi );

		if( $insert <= 0 )
		{
		  print LOG "Could not insert RNA gi $rna_gi for " .
			        "$gene_id\n";
		}
	  }

      #print "Inserting '$protein_gi' for '$geneObjectId'\n";
	  if( length( $protein_gi ) > 0 )
	  {
		
		my $insert = InsertProductGi( $geneObjectId, $protein_gi );

		if( $insert <= 0 )
		{
		  print LOG "Could not insert protein gi $protein_gi " .
			        "for $gene_id\n";
		}
	  }
	  if( length( $genomic_gi ) > 0 )
		{
		  my $insert = 
			InsertGenomicGi( $geneObjectId, $genomic_gi, 
							 $genomic_start, $genomic_end, 
						     $orientation );

		if( $insert <= 0 )
		{
		  print LOG "Could not insert genomic gi $genomic_gi " .
			        "for $gene_id\n";
		}
	  }
		
	}
	else
    {
	  print LOG "Failed to insert record $tax_id, $gene_id\n";
    }
  }
}

print LOG scalar localtime, " Read $lineCount lines and " .
   "found $recordCount records from $refseqFile.\n";

############################################################### 
print (LOG "Parsing gene_info\n" );
print ("Parsing gene_info\n" );

$lineCount = 0;
$recordCount = 0;
my $synonymCount = 0;
my $dbxrefCount = 0;
while( <INFOFILE> )
{
  chomp;

  my @fields = split(/\t/);

  RemoveDashes( @fields );

#  print "line: @fields\n";

  my $tax_id         = $fields[ 0 ];
  my $gene_id        = $fields[ 1 ];
  my $symbol         = $fields[ 2 ];
  my $locus_tag      = $fields[ 3 ];
  my $synField       = $fields[ 4 ];
  my $dbField       = $fields[ 5 ];
  my $chr            = $fields[ 6 ];
  my $map            = $fields[ 7 ];
  my $descr          = $fields[ 8 ];

###############################################################
# Do nothing if the record is bad.
  next if( length( $tax_id ) <= 0 && length( $gene_id ) <= 0 );

  my $gene_info_id;
  my $gene_object_id = GetGeneObjectId( $gene_id );

###############################################################
# Create a new GeneObject Id if one does not exist.
  if( $gene_object_id <= 0 )
  {
	print LOG "Adding $gene_id to gene_object.\n";

	$gene_object_id = InsertGeneObject( $gene_id );

	if( $gene_object_id <= 0 )
	{
	   print LOG "Unable to add $gene_id to gene_object.\n";
	}
  }

###############################################################
# Insert gene_info record
  $infoInserth->bind_param( 1, undef );
  $infoInserth->bind_param( 2, $gene_object_id, SQL_INTEGER );
  $infoInserth->bind_param( 3, $symbol );
  $infoInserth->bind_param( 4, $locus_tag );
  $infoInserth->bind_param( 5, $chr );
  $infoInserth->bind_param( 6, $map );
  $infoInserth->bind_param( 7, $descr );

  my $rows = $infoInserth->execute;

  if( $rows > 0 )
  {
	$recordCount++;

	$getInfoIdh->bind_param( 1, $gene_object_id, SQL_INTEGER );

	$getInfoIdh->execute;
	  
	my @result = $getInfoIdh->fetchrow_array;

	$gene_info_id = $result[ 0 ];
	if( $gene_info_id <= 0 )
	{
	  print LOG "Failed to retrieve GeneInfo id for $tax_id, $gene_id\n";
	  next;
	}
  }
  else
  {
	print LOG "Failed to insert gene_info record $tax_id, $gene_id\n";
	next;
  }
###############################################################
# Insert any synonyms.
  if( length( $synField ) > 0 )
  {
	my @synonyms = split(/\|/, $synField);

	foreach my $syn ( @synonyms )
	{
	  $synonymInserth->bind_param( 1, undef );
	  $synonymInserth->bind_param( 2, $gene_info_id, SQL_INTEGER );
	  $synonymInserth->bind_param( 3, $syn);

	  $rows = $synonymInserth->execute;

	  if( $rows <= 0 )
	  {
		print LOG "Failed to insert synonym $syn " .
		  "for $tax_id, $gene_id\n";
	  }
	  else
	  {
		$synonymCount++;
	  }
	}
  }

###############################################################
# Insert any dbxrefs.
  if( length( $dbField ) > 0 )
  {
	my @dbxrefs = split(/\|/, $dbField );

	foreach my $xref( @dbxrefs )
	{
	  my@names = split(/\:/, $xref );

	  $dbxrefInserth->bind_param( 1, undef );
	  $dbxrefInserth->bind_param( 2, $gene_info_id, SQL_INTEGER );
	  $dbxrefInserth->bind_param( 3, $names[ 0 ] );
	  $dbxrefInserth->bind_param( 4, $names[ 1 ] );

	  $rows = $dbxrefInserth->execute;

	  if( $rows <= 0 )
	  {
		print LOG "Failed to insert dbxref $xref " .
		  "for $tax_id, $gene_id\n";
	  }
	  else
	  {
		$dbxrefCount++;
	  }
	}
  }
}

print LOG scalar localtime, " Read $lineCount lines and " .
   "found $recordCount records from $infoFile.\n" .
   "\tsynonyms:\t$synonymCount\tDBXrefs:\t$dbxrefCount\n";

###############################################################
#Parse gene_history
print (LOG "Parsing gene_history\n" );
print ("Parsing gene_history\n" );

$lineCount = 0;
$recordCount = 0;

while( <HISTORYFILE> )
{
  chomp;

  my @fields = split(/\t/);

  $lineCount++;

###############################################################
# if certain fields are represented by a dash set them to undef.
  RemoveDashes( @fields );

  my $tax_id         = $fields[ 0 ];
  my $gene_id        = $fields[ 1 ];
  my $old_gene_id    = $fields[ 2 ];
  my $old_symbol     = $fields[ 3 ];

###############################################################
# Note that gene_id can be NULL in this file.
  if( length( $tax_id ) > 0 )
  {
	$historyInserth->bind_param( 1, undef );
	$historyInserth->bind_param( 2, $tax_id, SQL_INTEGER );
	$historyInserth->bind_param( 3, $gene_id, SQL_INTEGER );
	$historyInserth->bind_param( 4, $old_gene_id, SQL_INTEGER );
	$historyInserth->bind_param( 5, $old_symbol );
	
	my $rows = $historyInserth->execute;

	if( $rows > 0 )
	{
	  $recordCount++;
	}
	else
    {
	  print LOG "Failed to insert History record $tax_id, $gene_id\n";
    }

  }
}

print LOG scalar localtime, " Read $lineCount lines and " .
   "found $recordCount records from $historyFile.\n";

###############################################################
#Parse gene2pubmed
print (LOG "Parsing gene_pubmed\n" );
print ("Parsing gene_pubmed\n" );

$lineCount = 0;
$recordCount = 0;
while( <PUBMEDFILE> )
{
  chomp;

  my @fields = split(/\t/);

  $lineCount++;

  my $gene_id  = $fields[ 0 ];
  my $pmid     = $fields[ 1 ];
  my $gene_object_id;

  if( length( $gene_id ) > 0 && length( $pmid ) > 0 )
  {
	$gene_object_id = GetGeneObjectId( $gene_id );

###############################################################
# Create a new GeneObject Id if one does not exist.
    if( $gene_object_id <= 0 )
    {
      print LOG "Adding $gene_id to gene_object.\n";

	  $gene_object_id = InsertGeneObject( $gene_id );

	  if( $gene_object_id <= 0 )
	  {
		print LOG "Unable to add $gene_id to gene_object.\n";
	  }
	}



	$pubmedInserth->bind_param( 1, undef );
	$pubmedInserth->bind_param( 2, $gene_object_id, SQL_INTEGER );
	$pubmedInserth->bind_param( 3, $pmid, SQL_INTEGER );
	
	my $rows = $pubmedInserth->execute;

	if( $rows > 0 )
	{
	  $recordCount++;
	}
	else
    {
	  print LOG "Failed to insert record $gene_id, $pmid\n";
    }

  }
}

print LOG scalar localtime, " Read $lineCount lines and " .
   "found $recordCount records from $pubmedFile.\n";

$geneObjectInserth->finish;
$genomicGiInserth->finish;
$productGiInserth->finish;
$pubmedInserth->finish;
$infoInserth->finish;
$historyInserth->finish;
$synonymInserth->finish;
$dbxrefInserth->finish;
$getObjectIdh->finish;
$getInfoIdh->finish;

$dbh->commit;
$dbh->disconnect;

close REFSEQFILE;
close INFOFILE;
close HISTORYFILE;
close PUBMEDFILE;

exit;

######################################################################
# DeleteAllRecords()
#
# Delete all existing records in the tables.
sub DeleteAllRecords()
{
  print LOG "Deleting records.\n";
   print "Deleting records.\n";

  $dbh->do( q{DELETE FROM gene_dbxrefs} ) 
    or die "Unable to delete from gene_dbxrefs.\n";
  $dbh->do( q{DELETE FROM gene_genomicgi} ) 
    or die "Unable to delete from gene_genomicgi.\n";
  $dbh->do( q{DELETE FROM gene_history} ) 
    or die "Unable to delete from gene_history.\n";
  $dbh->do( q{DELETE FROM gene_info} ) 
    or die "Unable to delete from gene_info.\n";
  $dbh->do( q{DELETE FROM gene_object} ) 
    or die "Unable to delete from gene_object.\n";
  $dbh->do( q{DELETE FROM gene_productgi} ) 
    or die "Unable to delete from gene_productgi.\n";
  $dbh->do( q{DELETE FROM gene_pubmed} ) 
    or die "Unable to delete from gene_pubmed.\n";
  $dbh->do( q{DELETE FROM gene_synonyms} ) 
    or die "Unable to delete from gene_synonyms.\n";

}


######################################################################
# ResetTables()
#
# Drops tables if they exist and recreates them.
sub ResetTables()
{
  print LOG "Dropping tables.\n";
   print "Dropping tables.\n";

  $dbh->do( q{DROP TABLE IF EXISTS gene_object} ) 
    or die "Unable to delete from gene_object.\n";
  $dbh->do( q{DROP TABLE IF EXISTS gene_genomicgi} ) 
    or die "Unable to delete from gene_genomicgi.\n";
  $dbh->do( q{DROP TABLE IF EXISTS gene_productgi} ) 
    or die "Unable to delete from gene_productgi.\n";
  $dbh->do( q{DROP TABLE IF EXISTS gene_history} ) 
    or die "Unable to delete from gene_history.\n";
  $dbh->do( q{DROP TABLE IF EXISTS gene_info} ) 
    or die "Unable to delete from gene_info.\n";
  $dbh->do( q{DROP TABLE IF EXISTS gene_synonyms} ) 
    or die "Unable to delete from gene_synonyms.\n";

 $dbh->do( q{DROP TABLE IF EXISTS gene_dbxrefs} ) 
    or die "Unable to delete from gene_dbxrefs.\n";
 $dbh->do( q{DROP TABLE IF EXISTS gene_pubmed} ) 
    or die "Unable to delete from gene_pubmed.\n";

  print LOG "Creating tables.\n";
  print "Creating tables.\n";

  my $sql;

  $sql  = "CREATE TABLE `gene_object` ( ";
  $sql  .= 	      "`id` INTEGER UNSIGNED NOT NULL AUTO_INCREMENT, ";
  $sql  .= 	      "`timestamp` TIMESTAMP NOT NULL, ";
  $sql  .= 	      "`geneid` int(11) NOT NULL, ";
  $sql  .= 	      "`status` varchar(64), ";
  $sql  .= 	      "CONSTRAINT UNIQUE( geneid, status ),	";
  $sql  .= 	      "INDEX igeneobject_id( `id` ), ";	
  $sql  .= 	      "INDEX igeneobject_ts( `timestamp` ), ";	
  $sql  .= 	      "INDEX `igeneobject_geneid` (`geneid`), ";
if ($engine eq $ISAM) {
  $sql  .= 	      "INDEX `igeneobject_status` (`status`) )";
} elsif ($engine eq $INNO) {
  $sql  .= 	      "INDEX `igeneobject_status` (`status`) ) TYPE=INNODB";
}
  $dbh->do( $sql )
	or die "Unable to create gene_object table.";

  print "Created gene_object\n";
  $sql   = "CREATE TABLE gene_genomicgi( ";
  $sql  .= 	      "`id` INTEGER UNSIGNED NOT NULL AUTO_INCREMENT, ";
  $sql  .= 	      "`timestamp` TIMESTAMP NOT NULL, ";
  $sql  .= 	      "`geneobjectid` INTEGER NOT NULL, ";
  $sql  .= 	      "`gi` INTEGER NOT NULL, ";
  $sql  .= 	      "`start` INTEGER, ";
  $sql  .= 	      "`end` INTEGER, ";
  $sql  .= 	      "`orientation` VARCHAR( 1 ), ";
  $sql  .= 	      "CONSTRAINT UNIQUE( geneobjectid, gi, start, end, orientation ), ";		
  $sql  .= 	      "INDEX igenomic_id( `id` ), ";		
  $sql  .= 	      "INDEX igenomic_ts( `timestamp` ), ";		
  $sql  .= 	      "INDEX igenomic_objectid( `geneobjectid` ), ";
if ($engine eq $ISAM) {
  $sql  .= 	      "INDEX igenomic_gi( `gi` ) )";
} elsif ($engine eq $INNO) {
  $sql  .= 	      "INDEX igenomic_gi( `gi` ) )  TYPE=INNODB";
}

 $dbh->do( $sql )
	or die "Unable to create gene_genomicgi table.";

  $sql   = "CREATE TABLE gene_productgi( ";
  $sql  .= 	      "`id` INTEGER UNSIGNED NOT NULL AUTO_INCREMENT, ";
  $sql  .= 	      "`timestamp` TIMESTAMP NOT NULL, ";
  $sql  .= 	      "`geneobjectid` INTEGER NOT NULL, ";
  $sql  .= 	      "`gi` INTEGER NOT NULL, ";
  $sql  .= 	      "CONSTRAINT UNIQUE( geneobjectid, gi ), ";	
  $sql  .= 	      "INDEX iproduct_id( `id` ), ";		
  $sql  .= 	      "INDEX iproduct_ts( `timestamp` ), ";		
  $sql  .= 	      "INDEX iproduct_objectid( `geneobjectid` ), ";
if ($engine eq $ISAM) {
  $sql  .= 	      "INDEX iprodict_gi( `gi` ) )";
} elsif ($engine eq $INNO) {
  $sql  .= 	      "INDEX iprodict_gi( `gi` ) ) TYPE=INNODB";
}
  
  $dbh->do( $sql )
	or die "Unable to create gene_productgi table.";

  $sql   = "CREATE TABLE `gene_history` ( ";
  $sql  .= "`id` INTEGER UNSIGNED NOT NULL AUTO_INCREMENT, ";
  $sql  .= "`timestamp` TIMESTAMP NOT NULL, ";
  $sql  .= "`taxid` INTEGER, ";	
  $sql  .= "`currentgeneid` INTEGER, ";
  $sql  .= "`oldgeneid` INTEGER, ";
  $sql  .= "`oldsymbol` varchar(20), ";
  $sql  .= "INDEX igenehistory_id( `id` ), ";	
  $sql  .= "INDEX igenehistory_ts( `timetamp` ), ";	
  $sql  .= "KEY `current_gene_id` (`currentgeneid`), ";
  $sql  .= "KEY `discont_gene_id` (`oldgeneid`), ";
if ($engine eq $ISAM) {
  $sql  .= "KEY `discont_symbol` (`oldsymbol`) )";
} elsif ($engine eq $INNO) {
  $sql  .= "KEY `discont_symbol` (`oldsymbol`) )  TYPE=INNODB";
}

  $dbh->do( $sql )
	or die "Unable to create gene_history table.";

  $sql   = "CREATE TABLE `gene_info` ( ";
  $sql  .= "`id` INTEGER UNSIGNED NOT NULL AUTO_INCREMENT, ";
  $sql  .= "`timestamp` TIMESTAMP NOT NULL, ";
  $sql  .= "`geneobjectid` int(11) NOT NULL , ";
  $sql  .= "`symbol` VARCHAR(255), ";
  $sql  .= "`locustag` VARCHAR(255), ";
  $sql  .= "`chromosome` VARCHAR(32), ";
  $sql  .= "`maplocation` VARCHAR(255), ";
  $sql  .= "`description` MEDIUMTEXT, ";
  $sql  .= "INDEX igeneinfo_id( `id` ),	";
  $sql  .= "INDEX igeneinfo_ts( `timestamp` ),	";
  $sql  .= "KEY `object_id` (`geneobjectid`), ";
  $sql  .= "KEY `symbol` (`symbol`), ";
  $sql  .= "KEY `locus_tag` (`locustag`), ";
if ($engine eq $ISAM) {
  $sql  .= "KEY `chr` (`chromosome`) )";
} elsif ($engine eq $INNO) {
  $sql  .= "KEY `chr` (`chromosome`) )  TYPE=INNODB";
}

  $dbh->do( $sql )
	or die "Unable to create gene_info table.";

  $sql   = "CREATE TABLE `gene_synonyms` ( ";
  $sql  .= "`id` INTEGER UNSIGNED NOT NULL AUTO_INCREMENT, ";
  $sql  .= "`timestamp` TIMESTAMP NOT NULL, ";
  $sql  .= "`geneinfoid` INTEGER NOT NULL, ";
  $sql  .= "`synonym` TEXT NOT NULL, ";
  $sql  .= "INDEX igenesyn_id( `id` ), ";
  $sql  .= "INDEX igenesyn_ts( `timestamp` ), ";
  $sql  .= "INDEX igeneinfoid( `geneinfoid` ), ";		
if ($engine eq $ISAM) {
  $sql  .= "INDEX isynonym( `synonym`(100) ) )";	
} elsif ($engine eq $INNO) {
  $sql  .= "INDEX isynonym( `synonym`(100) ) ) TYPE=INNODB";	
}

  $dbh->do( $sql )
	or die "Unable to create gene_synonyms table.";

  $sql   = "CREATE TABLE `gene_dbxrefs` ( ";
  $sql  .= "`id` INTEGER UNSIGNED NOT NULL AUTO_INCREMENT, ";
  $sql  .= "`timestamp` TIMESTAMP NOT NULL, ";
  $sql  .= "`geneinfoid` INTEGER NOT NULL, ";
  $sql  .= "`dbname` VARCHAR( 255 ) NOT NULL, ";
  $sql  .= "`dbaccess` VARCHAR( 30 ) NOT NULL,";
  $sql  .= "INDEX igenedbxrefs_id( `id` ), ";		
  $sql  .= "INDEX igenedbxrefs_ts( `timestamp` ), ";		
  $sql  .= "INDEX igenedbxrefs_infoid( `geneinfoid` ), ";		
  $sql  .= "INDEX igenedbxrefs_db_name( `dbname` ), ";
if ($engine eq $ISAM) {
  $sql  .= "INDEX igenedbxrefs_db_access( `dbaccess` ) )";
} elsif ($engine eq $INNO) {
  $sql  .= "INDEX igenedbxrefs_db_access( `dbaccess` ) ) TYPE=INNODB";
}

  $dbh->do( $sql )
	or die "Unable to create gene_dbxrefs table.";

  $sql   = "CREATE TABLE `gene_pubmed` ( ";
  $sql  .= "`id` INTEGER UNSIGNED NOT NULL AUTO_INCREMENT, ";
  $sql  .= "`timestamp` TIMESTAMP NOT NULL, ";
  $sql  .= "`geneobjectid` INTEGER NOT NULL, ";
  $sql  .= "`pmid` INTEGER NOT NULL, ";
  $sql  .= "INDEX ipubmed_id( `id` ), ";
  $sql  .= "INDEX ipubmed_ts( `timestamp` ), ";
  $sql  .= "INDEX ipubmed_geneid( `geneobjectid` ), ";
if ($engine eq $ISAM) {
  $sql  .= "INDEX ipmid( `pmid` ) )";
} elsif ($engine eq $INNO) {
  $sql  .= "INDEX ipmid( `pmid` ) ) TYPE=INNODB";
}

$dbh->do( $sql )
	or die "Unable to create gene_pubmed table.";
 	
}

#########################################################
# RemoveDashes
#
# sets fields that consist of only "-" to undefined.
#
# @fields list of fields to be checked.
#

sub RemoveDashes()
{
  my @fields;
  
  foreach (@_)
  {
	if( /^-$/ )
	{
	  $_ = undef;
	}
	push ( @fields, $_ );
  }

  return wantarray ? @fields : $fields[ 0 ];
}

#########################################################
# InsertGeneObject()
#
# Insert the given GeneId and Status into the GeneObject 
# table.
#
# @param $geneId  GeneId to insert.
#
# @param $status  Status to insert. 
sub InsertGeneObject()
{
  my( $geneId, $status ) = @_;
  my $id;
  my @rows;
  
#########################################################
# First try to retrieve a record for this gene id.
  $id = GetGeneObjectId( $geneId );

  if( $id <= 0 )
  {
	$geneObjectInserth->bind_param( 1, undef );
	$geneObjectInserth->bind_param( 2, $geneId, SQL_INTEGER );
	$geneObjectInserth->bind_param( 3, $status );

	@rows = $geneObjectInserth->execute;

	if( $rows[ 0 ] > 0 )
	{

	  $id = GetGeneObjectId( $geneId );
	}
  }

  return $id;
}

sub GetGeneObjectId()
{
  my $geneId = $_[ 0 ];
  my @rows;
  my $id;

  $getObjectIdh->bind_param( 1, $geneId, SQL_INTEGER );

  $getObjectIdh->execute;

  @rows = $getObjectIdh->fetchrow_array;

  if( $rows[ 0 ] <= 0 ) 
  {
	#print LOG "GetGeneObjectId():Failed to retrieve " .
	 #          "geneobjectid for $geneId\n";
	$id = -1;
  }
  else
  {
	$id = $rows[ 0 ];
    #print "Found $id for $geneId \n";
  }
  
  return $id;
}

sub GetGenomicId()
{
  my( $geneObjectId, $gi, $start, $end, $orientation ) = @_;
  my @rows;
  my $id;

  $getGenomicIdh->bind_param( 1, $geneObjectId, SQL_INTEGER );
  $getGenomicIdh->bind_param( 2, $gi, SQL_INTEGER );
  $getGenomicIdh->bind_param( 3, $start, SQL_INTEGER );
  $getGenomicIdh->bind_param( 4, $end, SQL_INTEGER );
  $getGenomicIdh->bind_param( 5, $orientation, SQL_INTEGER );

  $getGenomicIdh->execute;

  @rows = $getGenomicIdh->fetchrow_array;

  if( $rows[ 0 ] <= 0 ) 
  {
	print LOG "GetGenomicId():Failed to retrieve id for $geneObjectId, $gi\n";
	           
	$id = -1;
  }
  else
  {
	$id = $rows[ 0 ];
  }
  
  return $id;
}

sub InsertProductGi()
{
  my( $geneObjectId, $gi ) = @_;
  my $rows;
  my $success;

  $productGiInserth->bind_param( 1, undef);
  $productGiInserth->bind_param( 2, $geneObjectId, SQL_INTEGER);
  $productGiInserth->bind_param( 3, $gi, SQL_INTEGER );	

  $rows = $productGiInserth->execute;

  if( $rows <= 0 )
  {
	  print LOG "InsertProductGi():Failed to insert $gi " .
		        "for $geneObjectId\n";

	  $success = -1;
  }
  else
  {
	$success = 1;
  }
  return $success;
}

sub InsertGenomicGi()
{
  my( $geneObjectId, $gi, $start, $end, $orient ) = @_;
  my @rows;
  my $genomicId = -1;

  #$genomicId = GetGenomicId($geneObjectId, $gi, $start,
	#						$end, $orient);

  if( $genomicId <= 0 )
  {
	#print "Inserting $geneObjectId, $gi, $start, $end, $orient\n";
	$genomicGiInserth->bind_param( 1, undef);
	$genomicGiInserth->bind_param( 2, $geneObjectId, SQL_INTEGER);
	$genomicGiInserth->bind_param( 3, $gi, SQL_INTEGER );	
	$genomicGiInserth->bind_param( 4, $start, SQL_INTEGER);
	$genomicGiInserth->bind_param( 5, $end, SQL_INTEGER);
	$genomicGiInserth->bind_param( 6, $orient );

	@rows = $genomicGiInserth->execute;

	if( $rows[ 0 ] <= 0 )
	{
	  print LOG "InsertGenomicGi():Failed to insert $gi " .
		"for $geneObjectId\n";

	  $genomicId = -1;
	}
	else
	{
	  $genomicId = 1;
	}
  }
  return $genomicId;
}

sub ReadConfigFile()
{
  my $configFile = ".intrezrc";

  open (CONFIGFILE, "$configFile") || die "Cannot open file $configFile\n";
  while (<CONFIGFILE>) {
        my $line = $_;
        $line =~ s/\s+//g;

        my @words = split(/=/, $line);
        my $attr = $words[0];
        my $value = $words[1];

        if ($attr eq "username") {
                $user_name = $value;
        }
        elsif ($attr eq "password") {
                $password = $value;
        }
        elsif ($attr eq "database") {
                $database = $value;
        }
        elsif ($attr eq "dsn") {
                $alias = $value;
        }
  }

  # get configuration for server and port number from the correct DSN
  my $homedir = $ENV{'HOME'};
  my $configODBC = $homedir . "/.odbc.ini";
  open (CONFIGODBC, "$configODBC") || die "Cannot open file $configODBC\n";
  my $foundAlias=0;
  my $gotHost=0;
  my $gotPort=0;
  while (<CONFIGODBC>) {
        my $line = $_;
        $line =~ s/\s+//g;
        # find the correct section
        my $target = "[" . $alias . "]";
        if ($line eq $target) {
                $foundAlias=1;
        }

        # get HOST and PORT from the correct section
        if ($foundAlias == 1) {
                my @words = split(/=/, $line);
                my $attr = $words[0];
                my $value = $words[1];
                if ($attr eq "SERVER") {
                        $host = $value;
                        $gotHost = 1;
                }
                elsif ($attr eq "PORT") {
                        $port = $value;
                        $gotPort = 1;
                }
        }
        # finish with the correct section
        if ($gotHost==1 && $gotPort==1) {
                $foundAlias=0;
        }
  }
}
