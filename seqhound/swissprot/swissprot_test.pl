#!/usr/bin/perl
#
#Reads accessions from acindex.txt and queries accdb to see if
#the accession is found.
#
#Usage: perl swissprot_test.pl
#
#log file: swissprot_test.log
#
#input: acindex.txt from ftp.expasy.org

use strict;
use DBI qw(:sql_types);
use shconfig;
use Time::localtime;

###############################################################
#Open a log file
open( LOG, "> swissprot_test.log") or die "Cannot open log file\n";

my $username = shconfig->ReadIntrez( "username" );
my $password = shconfig->ReadIntrez( "password" );
my $alias    = shconfig->ReadIntrez( "dsn" );
my $database = shconfig->ReadIntrez( "database" );

my $host     = shconfig->ReadOdbcIni( $alias,  "SERVER" );
my $port     = shconfig->ReadOdbcIni( $alias, "PORT" );

print ( "$username, $password, $alias, $database, $host, $port.\n" );

my $accessfile = "acindex.txt";
#my $accessfile = "sp44_1.dat";
###############################################################
#Open the input file
print (LOG "Open the file.\n" );

open( ACCESSFILE, $accessfile ) or die "Can't open $accessfile : $!";

###############################################################
#Open the database
print (LOG  "Opening the database.\n" );

my $dbh = DBI->connect("DBI:mysql:database=$database;host=$host;port=$port",
					$username,$password) or die "Can't open database: $!";
 
###############################################################
#Prepare the statement handles.
print (LOG "Preparing the statement handles.\n" );
print (LOG "\nParent Secondary\n");
print (LOG "-------------------\n");

my $sql;
my $getGih;
my $getParentAccessh;
my $getParentidsh;

$sql  = "SELECT gi from accdb where access = ?";

$getGih = $dbh->prepare( $sql );

$sql  = "SELECT parent_id, source_id from dbxref where record_id = ?";

$getParentidsh = $dbh->prepare( $sql );

$sql  = "SELECT record_id, source_id from dbxref where id = ?";

$getParentAccessh = $dbh->prepare( $sql );

my $found = 0;
my $notFound = 0;
my $start = 0;
my $source_id = 0;

while(my $line = <ACCESSFILE> )
{
  chomp( $line );
#
#skip over header

  if( $start == 0 )
  {
	if( $line =~ /___/ )
	{
	  $start = 1;
	  next;
    }
  }

  next if ( $start == 0 );

  if ( $start == 1 && $line =~ /---/ )
  {
	last;
  }
#
#start reading data.
#
  my @fields = split(/\s+/, $line);

  my $access = $fields[ 0 ];

#
#strip spaces.
#
  $access =~ s/\s+//;

#
#Remember the original accession for logging purposes.
#
  my $oldAccess = $access;

  $access = GetParentAccess( $access );

  if( $access )
  {
	my $gi = GetGiFromAccdb( $access );

	if( $gi > 0 )
	{
	  $found++;
	}
	else
	{
	  $notFound++;
	
	  print( LOG "$access\t$oldAccess.\n" );
	  #print "Parent $access not found. Secondary access = $oldAccess.\n\n";
	}
  }
  else
  {
	$notFound++;
  }
}

$dbh->disconnect;

print "Finished testing Swiss-prot\n";

print "Found $found records. $notFound were not found\n";
print( LOG "\n\nFound $found records. $notFound were not found\n");

sub GetGiFromAccdb()
{
  my $access = $_[0];
  my $gi = 0;

  $getGih->bind_param( 1, $access, SQL_CHAR );

  $getGih->execute;

  if( my @rows = $getGih->fetchrow_array )
  {
	$gi = $rows[ 0 ];
  }

  $getGih->finish;

  return $gi;

}

sub GetParentAccess()
{
  my $access = $_[0];

  my $parentId  = 0;
  my $parentAccess = undef;

#
#Reset source_id for the new record.
#
  $source_id = 0;
#
#Get all parent ids for this record to find the primary Swiss-Prot
#record. (i.e. parent_id = 0 and source_id = 12 ).
#
  $getParentidsh->bind_param( 1, $access, SQL_CHAR );

  $getParentidsh->execute;

  while( ( my @ids = $getParentidsh->fetchrow_array ) &&
	     $source_id != 12 )
  {
	$parentId  = $ids[ 0 ];
	$source_id = $ids[ 1 ];
#
#If this is parent_id is 0 and source_id is 12, this is the primary
#record.
	if( $parentId == 0 && $source_id == 12 )
	{
	  $parentAccess = $access;
	}
	else
	{
#
#Get the accesssion for the parent_id.
#	
	  $parentAccess = 
		GetAccessFromParentId( $parentId, $source_id );
	}

  }

  if( $source_id != 12 )
  {
	$parentAccess = $access;
  }

  return $parentAccess;
}

sub GetAccessFromParentId()
{
  my $parentId = $_[ 0 ];
  my $parentAccess = undef;

  $getParentAccessh->bind_param( 1, $parentId, SQL_INTEGER );
	
  $getParentAccessh->execute;

  while ( my @rows = $getParentAccessh->fetchrow_array )
  {
	$parentAccess = $rows[ 0 ];
	$source_id    = $rows[ 1 ];

	last if( $source_id == 12 );
  }

  $getParentAccessh->finish;

  return $parentAccess;
}
