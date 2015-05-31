#!/usr/bin/perl 
#
# $Id: yeastnameparser.pl,v 1.5 2004/10/22 18:40:00 eburgess Exp $
#*******************************************************************************************
#  Copyright (C) 2000 Mount Sinai Hospital (MSH)
#
#  This program is free software; you can redistribute it and/or
#  modify it under the terms of the GNU General Public License as
#  published by the Free Software Foundation; either version 2 of
#  the License, or any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
#
#  See the GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the
#         Free Software Foundation, Inc.,
#         59 Temple Place, Suite 330, Boston, MA
#         02111-1307  USA
#  or visit http://www.gnu.org/copyleft/gpl.html
#
#  SPECIAL EXCEPTIONS
#
#  As a special exception, Mount Sinai Hospital gives permission to
#  link this program with the following non-GPL programs or libraries,
#  and distribute the resulting executable, without including the source
#  code for these in the source distribution:
#
#  a) CodeBase 6.5 or greater from Sequiter Software Inc.
#
#  b) The C or C++ interface to Oracle 8.x or greater, from
#     Oracle Corporation or IBM DB2 UDB.
#
#
#  PROGRAM:
#
#
#  AUTHORS:
#           Elizabeth Burgess and Christopher W.V. Hogue hogue@mshri.on.ca
#
#
#  REVISION/CONTRIBUTION NOTES:
#
#  June 1, 2000  Original MSH Revision.
#                Hogue Lab - University of Toronto Biochemistry
#                Department and the Samuel Lunenfeld Research
#                Institute, Mount Sinai Hospital
#                http://bioinfo.mshri.on.ca  hogue@mshri.on.ca
#                ftp://ftp.mshri.on.ca/
#
#
#  PUBLICATION TO CITE:
#
#
#  DESCRIPTION
#  Imports yeast names and info from a tab delimited file originating 
#  from SGD, into a MySql database.
#
#  Input: SGD_features.tab from genome-ftp.stanford.edu/pub/yeast/data_download
#
#  Usage: perl yeastnameparser.pl
#
#  log files: yeast name.log   -general errors
#             yeastadd.log     -list of added names, sgdids, bionameid and update field..
#             yeastupdate.log  -list of updated names, sgdids, bioentityid, field.
#*******************************************************************

use strict;

use DBI qw(:sql_types);
use shconfig;
use dbxref;
use Time::localtime;

my $namefile = "SGD_features.tab";

my @nameinfo;

my $username = shconfig->ReadIntrez( "username" );
my $password = shconfig->ReadIntrez( "password" );
my $alias    = shconfig->ReadIntrez( "dsn" );
my $database = shconfig->ReadIntrez( "database" );

my $host     = shconfig->ReadOdbcIni( $alias,  "SERVER" );
my $port     = shconfig->ReadOdbcIni( $alias, "PORT" );

#print "$username, $password, $alias, $database, $host, $port\n";

my $dbh;

my $bioentityInserth;
my $bionameInserth;
my $dbxrefh;
my $getBioentityIdByAccessh;
my $getNamesByBioentityh;
my $updateActionh;
my $updateStdh;
my $getSecondRefh;
my $secondRefInserth;
my $sql;

###############################################################2
# Global variables used as constant values.
my $UNCHANGED = 0;
my $ADD       = 1;
my $DELETE    = 2;

my $FEATURE     = 100;
my $DESCRIPTION = 101;
my $ALIAS       = 102;
my $GENE        = 103;
###############################################################2
#Open a log file
open( LOG, ">yeastname.log") or die "Cannot open log file\n";
open( UPDATE, ">yeastupdate.log") or die "Cannot open update file\n";
open( ADDFILE, ">yeastadd.log" ) or die "Cannot open add file\n";

###############################################################
#Open the input file
print (LOG "Open the file.\n" );

open( NAMEFILE, $namefile ) or die "Can't open $namefile : $!";

###############################################################
#Open the database
print (LOG  "Opening the database.\n" );

#$dbh = DBI->connect("DBI:mysql:database=ebref;host=10.0.0.122;port=33306","eburgess","asdf1234") 
$dbh = DBI->connect("DBI:mysql:database=$database;host=$host;port=$port",
					$username,$password) or die
  "Unable to connect to connect to database: $dbh->errstr\n"; 


my %dbtable = dbxref->createDbHashTable( $dbh, $database );
my $dbId = $dbtable{ "SGD"}->source_id();

###############################################################
#Delete yeast records from the tables.
#print LOG "Deleting yeast records from the tables.\n";

#$dbh->do( q{DELETE FROM bioentity WHERE db = "sgd"} ) 
#    or die "Unable to delete from bioentity.\n";

###############################################################
#Prepare the statement handles.
print (LOG "Preparing the statement handles.\n" );

$sql  = "INSERT INTO bioentity(id, bioentitytypeid, ";
$sql .=                        "db, access, ";
$sql .=                        "identifier,field ) ";
$sql .=         "VALUES(?, ?, ?, ?, ?, ? )";

$bioentityInserth = $dbh->prepare( $sql );

$sql  = "INSERT INTO bioname(id, bioentityid, name, ";
$sql .=                      "nametypeid, db, access, identifier, ";
$sql .=                      "field, official, deprecated, ";
$sql .=                      "datedeprecated, ruleid, action, ";
$sql .=                      "actiondate ) ";
$sql .=                      "VALUES( ?,?,?,?,?,?,?,?,?,?,?,?,?, NOW() )";

$bionameInserth = $dbh->prepare( $sql );

$sql  = "SELECT * from dbxref a LEFT JOIN dbxref b ON ";
$sql .=                         "a.parent_id = b.id WHERE ";
$sql .=                         "b.record_id = ? AND ";
$sql .=                         "b.parent_id = 0 AND ";
$sql .=                         "a.record_id LIKE '_P\_%'";

$dbxrefh = $dbh->prepare( $sql );

$sql  = "SELECT * from bioentity WHERE access = ?";

$getBioentityIdByAccessh = $dbh->prepare( $sql );

$sql  = "SELECT * FROM bioname WHERE bioentityid = ?";

$getNamesByBioentityh = $dbh->prepare( $sql );

$sql  = "UPDATE bioname SET action = ?, actiondate = NOW() ";
$sql .=                 "WHERE id = ? ";

$updateActionh = $dbh->prepare( $sql );

$sql = "UPDATE bioname SET official = ? WHERE id = ?";

$updateStdh = $dbh->prepare( $sql );

$sql = "SELECT id, bionameid, db, access, field from ";
$sql .=           "secondrefs WHERE bionameid = ? and ";
$sql .=           "db = ? and access = ?";

$getSecondRefh = $dbh->prepare( $sql );

$sql  =  "INSERT into secondrefs( id, bionameid, db, access, ";
$sql .=                          "field ) ";
$sql .=                          "VALUES( ?,?,?,?,? ) ";

$secondRefInserth = $dbh->prepare( $sql );

###############################################################
# Begin parsing the names.
print (LOG "Parsing the names\n" );

my $nameCount = 0;

while(<NAMEFILE> )
{
  chomp;

  my $goodName = 0;

  #found an ORF
  if( /Verified/ || /Uncharacterized/  || /Dubious/ )
  {
    @nameinfo = split(/\t/);

    my $sgdid       = $nameinfo[0];
    my $feature     = $nameinfo[ 3 ];
    my $gene        = $nameinfo[ 4 ];
    my @aliases     = split(/\|/,$nameinfo[5]);

    my $description = $nameinfo[15];

	my @fileNames;

	#verify that SGDID is good.
	
	#print ("$sgdid, $feature, $gene, $description\n");
###############################################################
# Place all good names into an array.
    #Checking the feature field
    $goodName = CheckName( $feature );

   if( $feature ne $gene && $goodName )
   {
	 push @fileNames, [ $feature, $FEATURE, $ADD, 0 ];
   }

    #Checking the gene field
   $goodName = CheckName( $gene );
   if( $goodName )
   {
	 my $std = 0;

	
	 push @fileNames, [ $gene, $GENE, $ADD, 1 ];
   }

   #Checking each alias
   foreach my $test( @aliases )
   {
     $goodName = CheckName( $test );
     if( $goodName )
     {
	   push @fileNames, [ $test, $ALIAS, $ADD, 0 ];
     }	
   }
	
   #Checking the description field
	$goodName = CheckName( $description );
	if( $goodName )
	{
	   push @fileNames, [ $description, $DESCRIPTION, $ADD, 0 ];
	 }	

	if( @fileNames )
	{
###############################################################
# Get the bioentity id
	  my $entityId = FindBioentityID( $sgdid );

###############################################################
# For now, do nothing if the bioentity id is not found. Later
# we might want to add in a new bioentity id.
	  next if( $entityId <= 0 );

###############################################################
# Get all names for this bioentity from the table.
	  $getNamesByBioentityh->bind_param( 1, $entityId, SQL_INTEGER );
	  $getNamesByBioentityh->execute;

	  my $tbl_ref =  
		$getNamesByBioentityh->fetchall_arrayref;

	  my @update;
	  my $found = "FALSE";
###############################################################
# Compare each name in the table with those found in the file.
	  if( $tbl_ref )
	  {
		for my $row( @$tbl_ref )
		{
		  my $bionameId    = $row->[0];
		  my $nameFromDb   = $row->[2];
		  my $db           = $row->[4];
		  my $fieldFromDb  = $row->[7];
		  my $stdFromDb    = $row->[8];
		  my $actionFromDb = $row->[12];

		  for my $fileRow( @fileNames )
		  {
			my $nameFromFile  = $fileRow->[0];
			my $fieldFromFile = $fileRow->[1];
			my $stdFromFile   = $fileRow->[3];

			if ( $nameFromFile eq $nameFromDb )
			{
			  $found = "TRUE";
			  $fileRow->[2] = $UNCHANGED;

#####################################################################
# If the name has been deleted and now is readded, update the record.
			  if( $actionFromDb == $DELETE && $db eq 'sgd' )
			  {
				print UPDATE "$nameFromFile\t$sgdid\t$bionameId\t action=$ADD\n";
				UpdateAction( $bionameId, $ADD  );
			  }

#####################################################################
# Is this an official name?
			  if( $stdFromDb != $stdFromFile )
			  {
				print UPDATE "$nameFromFile\t$sgdid\t$bionameId\tofficial=$stdFromFile\n";
				#print "Marking $nameFromDb, $sgdid as official: $stdFromFile\n";
				UpdateStandard( $bionameId, $stdFromFile );
			  }
			
			  AddSecondRef($bionameId, "sgd", $sgdid, 
						   $fieldFromFile );
			  last;
			}
		  }

		  if( $found eq "FALSE" &&
			  $db eq 'sgd' &&
			  $actionFromDb == $ADD )
		  {
			push @update, $bionameId;
			print UPDATE "$nameFromDb\t$sgdid\t$bionameId\tfield=$fieldFromDb\n";
		  }
		}
	  }	

	  for my $updateId ( @update )
	  {
		UpdateAction(  $updateId, $DELETE );
	  }
		
	  for my $fileRow( @fileNames )
	  {
		my $name   = $fileRow->[0];
		my $field  = $fileRow->[1];
		my $std    = $fileRow->[3];

		if( $fileRow->[2] == $ADD )
		{
		  AddName( $entityId, $name, 1, $sgdid, $field, 
				   $std );

			print ADDFILE "$name\t$sgdid\t$entityId\tfield=$field\n";
		}
	  }
	}
 }
}

print LOG "\n\nFound $nameCount names.\n";

my $rc = $dbh->disconnect;

close NAMEFILE;
close IDFILE;

exit; 

#########################################################
#  CheckName
#
#  return 0 if the name is not correct in some way, 1 if
#  it's ok.
sub CheckName
{
    my $retval = 0;

    if (/^\(/){	
	print "FOUND BRACKETS!!!";
    }

    if( !defined $_[0]                    || #name is undefined
        $_[0] eq ""                     )  #|| #name is an empty string          
#	$_[0] =~ /^[A-Z]{1,2}[0-9]{3,9}/      || #name is primarily numerical
#	$_[0] =~ /^Y[A-P][LR][0-9\.\-]{3,9}/  || #yeast orf name
#	$_[0] =~ /\./                         || #name has a period in it
#	$_[0] =~ /^YM[0-9]+\b/                || #name is a YM0000 name.
#	$_[0] =~ /^[A-Z]{1}[0-9]+\b/ )           #name has only 1 letter at the beginning.
    {
	$retval = 0;
    }
    else 
    {
       $retval = 1;       
    }
    
    return $retval;
}

#########################################################
#  AddEntity
#
#  Adds a new bioentity to the bioentity table. Note that
#  all bioentities added from this script will have "sgd"
#  as their database and featuretype as their field.
#
#  param access   SGDID for this bioentity.
#
#  param field    Field for this bioentity. Always 
#                 featuretype for this script.
#
#  returns  The id for the new bioentity if successful.
#            -1 if there was a problem.
sub AddEntity
{
  my( $access, $field ) = @_;

  my $id = 0;

  $bioentityInserth->bind_param( 1, undef );
  $bioentityInserth->bind_param( 2, 3, SQL_INTEGER );
  $bioentityInserth->bind_param( 3, "sgd" );
  $bioentityInserth->bind_param( 4, $access );
  $bioentityInserth->bind_param( 5, undef );
  $bioentityInserth->bind_param( 6, $field );  

  if( !$bioentityInserth->execute() )
  {
    print LOG "AddEntity: Insert into bioentity failed for $access\n"; 
    $id = -1;
  }
  else
  {
	$getBioentityIdByAccessh->bind_param( 1, $access, 
										 SQL_CHAR );

	$getBioentityIdByAccessh->execute;	

    if( my @rows = $getBioentityIdByAccessh->fetchrow_array )
    {
	    $id = $rows[ 0 ];
	}
	else
	{
	  print LOG "AddEntity: Can not get bioentityId for $access\n";
	  $id = -1
	}
  }

  $bioentityInserth->finish;
  return $id;
}

#########################################################
#  AddName
#
#  Adds a name to the database. If the passed in bioentityid
#  is less that 0, then it will add a new bioentity. After
#  the name has been added, it checks to see if the name is 
#  proper nomenclature. If so, then it adds a p and 
#  capitilizes the first letter and adds that name too.
#
#  NOTE: The actiondate will be today's date and time as we are 
#        using the MySQL function NOW() to get the date.
#
#  param bioentityId   The bioentityId for that name. -1 if
#                      a bioentity does not exist for that name.
#
#  param name          The name to add.
#
#  param type          The type of name being added. e.g. locus.
#
#  param access        The SGDID for that name.
#  
#  param field         The field from which this name was extracted.
#
#  param std           Is this a standard name? 'Y' or 'N'
#
#  returns             the bioentityID for this name.

sub AddName
{
  my( $bioentityId, $name, $type, $access, 
	  $field, $std ) = @_;

  my $id = $bioentityId;
  my $retval = -1;

  $bionameInserth->bind_param( 1, undef ); 
  $bionameInserth->bind_param( 2, $id, SQL_INTEGER ); 
  $bionameInserth->bind_param( 3, $name ); 
  $bionameInserth->bind_param( 4, $type, SQL_INTEGER ); 
  $bionameInserth->bind_param( 5, "sgd" ); 
  $bionameInserth->bind_param( 6, $access ); 
  $bionameInserth->bind_param( 7, undef ); 
  $bionameInserth->bind_param( 8, $field, SQL_INTEGER ); 
  $bionameInserth->bind_param( 9, $std, SQL_INTEGER ); 
  $bionameInserth->bind_param( 10, 0 ); 
  $bionameInserth->bind_param( 11, "0000-00-00 00:00:00" ); 
  $bionameInserth->bind_param( 12, 0 ); 
  $bionameInserth->bind_param( 13, $ADD, SQL_INTEGER );
#  $bionameInserth->bind_param( 14, $actionDate );
  

  if( !$bionameInserth->execute() )
  {
    print LOG "AddName: Insert into bioname failed for $name, $access\n";
  }

  $nameCount++;
  #if its a name of proper nomenclature eg. STO1
  #add a p and capitalize the first letter.
#  if ( $name =~ /^[A-Z]{3}[0-9]{1,3}/)
#  {
#    my $new_name = lc$name."p";
#    $new_name = ucfirst$new_name;
      
#    $bionameInserth->bind_param( 3, $new_name );    
    
#    if( !$bionameInserth->execute() )
#    {
#	print("AddName: Insert into bioname failed for " . 
#	      $new_name . ", " . $access . "\n"); 
#    }
#    $nameCount++;
#  }
  $bionameInserth->finish;

  if( $bioentityId <= 0 )
  {
    $retval = $id;
  }
  else
  {
    $retval = $bioentityId;  
  }
  
  return $retval;
}

#########################################################
# FindBioentityID()
#
# Finds a bioentity for a given SGDID. Note that if there is
# no refseq accession for a given SGDID, then we do not add
# an entity.
#
# @param sgdid  The SGDID for which to find a refseq accession.
sub FindBioentityID()
{
  my $sgdid = $_[0];

  $dbxrefh->bind_param( 1, $sgdid, SQL_CHAR );

  $dbxrefh->execute;

  my @dbxrefs;

  my $access;
  my $bioentityId = -1;

  if( @dbxrefs = $dbxrefh->fetchrow_array )
  {
	$access = $dbxrefs[ 2 ];
  }
  else
  {
	$access = $sgdid;
	print LOG "FindBioentityId: Could not find a refseq record for $sgdid\n";
  }

  $getBioentityIdByAccessh->bind_param( 1, $access, SQL_CHAR );

  $getBioentityIdByAccessh->execute;
  my @rows = $getBioentityIdByAccessh->fetchrow_array;

  if( $rows[ 0 ] > 0 )
  {
	$bioentityId = $rows[ 0 ];
  }
  else
  {
	print LOG "FindBioentityID: No bioentity for $sgdid\n";
  }

  $getBioentityIdByAccessh->finish;
  $dbxrefh->finish;

  return $bioentityId;
}

#########################################################
# UpdateAction
#
# Updates the action field of a given record in the bioname table.
#
# NOTE: The actiondate will be today's date and time.
#
# @param id        Bioname Id of the record to be updated.
#
# @param action    Action taken.
#
sub UpdateAction()
{
  my ( $id, $action ) = @_;
  my $retval = 1;

#  print "Updating $id, $action\n";

  $updateActionh->bind_param( 1, $action, SQL_INTEGER );
#  $updateActionh->bind_param( 2, $actionDate );
  $updateActionh->bind_param( 2, $id, SQL_INTEGER );

  if( !$updateActionh->execute() )
  {
	print LOG "UpdateAction: Failed to update bionameid $id\n"; 
    $retval = -1;
  }

  $updateActionh->finish;
  return $retval;
}

#########################################################
# UpdateStandard
#
# Updates the official field of a given record in the bioname table.
#
# @param id       bionameid of the record to be updated.
#
# @param standard Value to write to the official field.
#
sub UpdateStandard()
{
  my( $id, $standard ) = @_;
  my $retval = 1;

  print "UpdateStandard - marking $id as $standard\n";
  $updateStdh->bind_param( 1, $standard, SQL_INTEGER );
  $updateStdh->bind_param( 2, $id, SQL_INTEGER );


  if( !$updateStdh->execute() )
  {
	print LOG "UpdateStandard: Failed to update bionameid $id\n"; 
    $retval = -1;
  }

  $updateStdh->finish;

  return $retval;
}

sub AddSecondRef()
{
  my( $bionameid, $db, $access, $field ) = @_;

#########################################################
# Do we already have this record?
  $getSecondRefh->bind_param( 1, $bionameid, SQL_INTEGER );
  $getSecondRefh->bind_param( 2, $db );
  $getSecondRefh->bind_param( 3, $access );

  my @rows = $getSecondRefh->execute;

  if( $rows[ 0 ] <= 0 )
  {
	$secondRefInserth->bind_param( 1, undef );
	$secondRefInserth->bind_param( 2, $bionameid, SQL_INTEGER );
	$secondRefInserth->bind_param( 3, $db );
	$secondRefInserth->bind_param( 4, $access );
	$secondRefInserth->bind_param( 5, $field, SQL_INTEGER );

	if( !$secondRefInserth->execute() )
	{
	  print LOG "AddSecondRef: Insert into bioname failed for " .
		                "$bionameid, $db, $access, $field\n";
	}
  }
 
  $getSecondRefh->finish;
  $secondRefInserth->finish;
}
# $Log: yeastnameparser.pl,v $
# Revision 1.5  2004/10/22 18:40:00  eburgess
# Removed a print statement.
#
# Revision 1.4  2004/09/16 15:53:18  eburgess
# Modified to use SGD_features.tab from SGD, instead of chromosomal_feature.tab.
#
# Revision 1.3  2004/08/31 19:49:48  zwang
# Changed DB2 related variable names to ODBC. Got rid of hard-coded database name.
#
# Revision 1.2  2004/08/09 13:17:43  eburgess
# Fixed typo at top of file.
#
# Revision 1.1  2004/08/03 18:30:50  eburgess
# Initial revision.
#
