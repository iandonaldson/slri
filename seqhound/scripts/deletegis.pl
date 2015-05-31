#!/usr/bin/perl  
#$Id: deletegis.pl,v 1.5 2005/03/18 15:12:29 eburgess Exp $

######################################################################################################
#
# delete gis of a division from all of the core tables 
#
######################################################################################################
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
#  along with this program; if not, write to the2
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
#           Zhe Wang, Elizabeth Burgess and Christopher W.V. Hogue hogue@mshri.on.ca
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
#  Deletes either all gis in the same division or all gis from 
#  the same file. This file is a replacement for iterateparti.
#
#  Usage: perl deletegis.pl -d division -f filename
#         where -d is the division to be deleted and -f is the 
#         filename to be deleted. Either division or filename must
#         be specified.
#
#  log files: 
#                                     
#*******************************************************************

use strict;
use DBI qw(:sql_types);
use shconfig;
use Getopt::Std;
use vars qw($opt_d $opt_f);

###############################################################2
#Did the user enter an input and output file?
if( $#ARGV != 1)
{ 
  die "You must enter either division or filename\n";
}

###############################################################
#Get command line arguments
getopts('d:f:');

my $division = $opt_d;
my $filename = $opt_f;

if ($division eq "" && $filename eq "") 
{
	print "You must provide either division name or filename.\n";
	exit(0);
}

###############################################################2
#Open a log file
open( LOG, ">deletegis.log") or die "Cannot open log file\n";
open( GIS, ">deletegis.gis") or die "Cannot open deleted gi file\n";


my $user_name = shconfig->ReadIntrez( "username" );
my $password = shconfig->ReadIntrez( "password" );
my $alias    = shconfig->ReadIntrez( "dsn" );
my $database = shconfig->ReadIntrez( "database" );

my $host     = shconfig->ReadOdbcIni( $alias,  "SERVER" );
my $port     = shconfig->ReadOdbcIni( $alias, "PORT" );

my $dsn = "DBI:mysql:" . $database . ":" . $host . ":" . $port;

my $dbh;
my $sth;

# connect to MySQL
print (LOG  "Deletegis.pl: Opening the database.\n" );
$dbh = DBI->connect ($dsn, $user_name, $password, 
					 {RaiseError => 1, AutoCommit => 0});

my $count=0;
	
############################################
# Prepare the statement handles.
############################################
print (LOG "Preparing the statement handles.\n" );

my $getAccess;
my $getKloodgeFromAccess;
my $findContigsFromTopgi;
my $findGisFromContigs;
my $editTaxgiKloodge;
my $editContigChromidUpdate;
my $findContigGiFromGICHROMID;
my $insertHISTDB;
my $getName;

my $sql;


$sql  = "SELECT rowid, access, version FROM accdb WHERE gi = ?";

$getAccess = $dbh->prepare( $sql );

$sql  = "SELECT rowid, name, version FROM accdb WHERE gi = ?";

$getName = $dbh->prepare( $sql );

$sql  = "SELECT kloodge FROM chrom WHERE access = ?";

$getKloodgeFromAccess = $dbh->prepare( $sql );

$sql  = "SELECT contiggi FROM contigchromid WHERE topgi = ?";

$findContigsFromTopgi = $dbh->prepare( $sql );

$sql  = "SELECT gi from gichromid where contiggi = ?";

$findGisFromContigs = $dbh->prepare( $sql );

$sql  = "UPDATE taxgi SET kloodge = ? WHERE gi = ?";

$editTaxgiKloodge = $dbh->prepare( $sql );

$sql  = "UPDATE contigchromid SET changed = ? WHERE contiggi = ?";

$editContigChromidUpdate = $dbh->prepare( $sql );

$sql  = "SELECT contiggi FROM gichromid where gi = ?";

$findContigGiFromGICHROMID = $dbh->prepare( $sql );

$sql  = "INSERT INTO histdb( gi, oldgi, access, version, date, ";
$sql .=                     "action, filename) VALUES "; 
$sql .=                     "( ?, 0, ?, ?, ?, 6, 'n/a' )";

my $insertHISTDB = $dbh->prepare( $sql );

############################################
# Get the gis that satisfy the input criteria.
############################################
if( $division ne "" )
{
  $sql = "select gi from parti where division='$division'";
}
elsif( $filename ne "" )
{
  $sql = "select distinct gi from histdb where filename='$filename'"
}

$sth = $dbh->prepare( $sql );	
$sth->execute();

my @ct_MS;
my (@gis);
while (@ct_MS =$sth->fetchrow()) {
	$count++;
	push(@gis, @ct_MS[0]);
}
$sth->finish();

print LOG "$count GIs to be deleted.\n";

#####################################
# delete from all tables.
#####################################
my $i=0;
my $deleted = 0;
for $i (0 .. $#gis) {
	my $seid= 0;
	my $gi  = $gis[ $i ]; 

	##############################################
	#Write the gi to the deleted gi file:
	print GIS "$gi\n";

	my @accdbInfo = GetAccdbInfoFromGi( $gi );

	##########################################
	#Log the gi into the history table.
	LogHistory( $gi, $accdbInfo[ 0 ], $accdbInfo[ 1 ] );
	
	##########################################
	#Delete from the comgen tables;
	DeleteGiFromComgenTables( $gi, $accdbInfo[ 0 ] );
	

	####################
	# find seid for gi
	####################
	$sth = $dbh->prepare ("select seid from sengi where gi=$gi");
	$sth->execute();
	while (@ct_MS =$sth->fetchrow()) {
		$seid = @ct_MS[0];
	}
	$sth->finish();

	###################################################
	# delete record of interest from all core tables
	###################################################
	$dbh->do("delete from sendb where seid=$seid");
	$dbh->do("delete from accdb where gi=$gi");
	$dbh->do("delete from asndb where gi=$gi");
	$dbh->do("delete from parti where gi=$gi");
	$dbh->do("delete from pubseq where gi=$gi");
	$dbh->do("delete from sengi where gi=$gi");
	$dbh->do("delete from taxgi where gi=$gi");
	$dbh->do("delete from nucprot where gia=$gi");
	$dbh->do("delete from nucprot where gin=$gi");
	
	$deleted++;

}
$getAccess->finish;
$getKloodgeFromAccess->finish;
$findContigsFromTopgi->finish;
$getAccess->finish;
$getAccess->finish;
$getAccess->finish;
$getAccess->finish;

$dbh->commit();
$dbh->disconnect();

print LOG "Deleted $deleted gis.\n";
exit(0); 

sub DeleteGiFromComgenTables()
{
  my ( $gi, $access ) = @_;
  my $kloodge;



  if( length( $access ) > 0 )
  {
	$kloodge = GetKloodgeFromAccess( $access );

	##################################################
	# Is this a top level contig?
	if( $kloodge  > 0 )
	{
       ProcessTopLevelContig( $gi );

	   $dbh->do("delete from chrom where kloodge = $kloodge");
	}
  }
  else
  {
	print LOG "No access for gi $gi.\n";
  }
  #################################################
  # This could still be a contig gi or a low level gi.
  ProcessContigs( $gi );

  ProcessLowLevelGi( $gi );

  #################################################
  # Delete from gichromosome and contigchromosome
  $dbh->do("delete from gichromosome where gi = $gi");
  $dbh->do("delete from contigchromosome where contiggi = $gi");
}

sub GetAccdbInfoFromGi()
{
  my $gi = $_[0];
  my $rowid;
  my $access = "n/a";
  my $version = 0;
  my $name = "/n/a";
  my @info;

  eval
  {
	$getAccess->bind_param( 1, $gi, SQL_INTEGER );
  
	$getAccess->execute;

	my @row = $getAccess->fetchrow_array;

	$rowid  = $row[ 0 ];
	$access = $row[ 1 ];
	##############################
	#Make sure we find the access if it exists.
	#Get the real access, not 'n/a'.
	while( $rowid && $access =~ /n\/a/ )
	{
	  @row = $getAccess->fetchrow_array;

	  $rowid  = $row[ 0 ];
	  $access = $row[ 1 ];	

	  if( $version == 0 )
	  {
		$version = $row[ 2 ];
	  }
	}
  };

  if( $@ )
  {
	print LOG "GetAccdbInfoFromGi() failed for gi $gi. ERROR:$@\n";
	$access = undef;
  }


   #################################################
   #If we havent's found the accession, try to get the name.
  if( $access =~ 'n/a' )
  {
	eval
	{
	  $getName->bind_param( 1, $gi, SQL_INTEGER );
  
	  $getName->execute;

	  my @row = $getName->fetchrow_array;

	  $rowid  = $row[ 0 ];
	  $name = $row[ 1 ];
	  ##############################
	  #Make sure we find the access if it exists.
	  #Get the real access, not 'n/a'.
	  while( $rowid && $name =~ /n\/a/ )
	  {
		@row = $getName->fetchrow_array;

		$rowid  = $row[ 0 ];
		$name = $row[ 1 ];	
		
		if( $version == 0 )
		{
		  $version = $row[ 2 ];
		}
	  }
	};

	$access = $name;

	if( $@ )
	{
	  print LOG "GetAccdbInfoFromGi() failed for gi $gi. ERROR:$@\n";
	  $access = undef;
	}
  }

  if( $access =~ /n\/a/ )
  {
	$access = undef;
  }

  push( @info, $access );
  push( @info, $version );

  return @info;
}

sub GetKloodgeFromAccess()
{
  my $access = $_[0];
  my $kloodge;
  my @row;

  eval
  {
	$getKloodgeFromAccess->bind_param( 1, $access );

	$getKloodgeFromAccess->execute;

	@row = $getKloodgeFromAccess->fetchrow_array;

	$kloodge = $row[ 0 ];
  };

  if( $@ )
  {
	print LOG "GetKLoodgeFromAccess() failed for $access. ERROR:$@\n";
  }

  return $kloodge;
}

sub ProcessTopLevelContig()
{
  my $gi = $_[ 0 ];
  my @row;
  my $contig;

  my $result = EditTaxgiKloodgeByGi( $gi, 0);

  if( $result <= 0 )
  {
	print LOG "Unable to update taxgi for gi $gi, " . 
	  "kloodge = 0\n";
  }

  eval
  {
	$findContigsFromTopgi->bind_param( 1, $gi, SQL_INTEGER );

	$findContigsFromTopgi->execute;

	@row = $findContigsFromTopgi->fetchrow_array;

	$contig = $row[ 0 ];

	while( $contig > 0 )
	{
	  ProcessContigs( $contig );

	  @row = $findContigsFromTopgi->fetchrow_array;

	  $contig = $row[ 0 ];
	}
  };

  if( $@ )
  {
	print LOG "ProcessTopLevelContig: failed for $gi, $contig. " .
	  "ERROR:$@\n";
  }
  $dbh->do("delete from contigchromid where topgi=$gi");
}


sub ProcessContigs()
{
  my $contig = $_[0];
  my $gi;
  my @row;

  my $result = EditTaxgiKloodgeByGi( $contig, 0);

  if( $result <= 0 )
  {
	print LOG "Unable to update taxgi for gi $contig, " . 
	  "kloodge = 0\n";
  }

  eval
  {
	$findGisFromContigs->bind_param( 1, $contig, SQL_INTEGER );

	$findGisFromContigs->execute;

	@row = $findGisFromContigs->fetchrow_array;

	$gi = $row[ 0 ];
	
	while( $gi > 0 )
	{
	  ProcessLowLevelGi( $gi );

	  @row = $findGisFromContigs->fetchrow_array;

	  $gi = $row[ 0 ];
	}
  };

  if( $@ )
  {
	print LOG "ProcessContigs() failed for $contig. ERROR:$@\n";
  }

  $dbh->do("delete from contigchromid where contiggi=$contig");
}


sub ProcessLowLevelGi()
{
  my $gi = $_[ 0 ];

  my $result = EditTaxgiKloodgeByGi( $gi, 0 );

  if( $result <= 0 )
  {
	print LOG "Unable to update taxgi for gi $gi, " . 
	  "kloodge = 0\n";
  }

  ########################################################
  # Get the contig gi and mark the contig as updated.
  my $contiggi = FindContigGiFromGICHROMID( $gi );

  if( $contiggi > 0 )
  {
	$result = EditContigChromidChanged( $contiggi, 1 );

	if( $result <= 0 )
	{
	  print LOG "Unable to mark contig $contiggi as changed\n";
	}
  }

  $dbh->do("delete from gichromid where gi=$gi");

}

sub EditTaxgiKloodgeByGi() 
{
  my ($gi, $kloodge) = @_;
  my $result;

  eval
  {
	$editTaxgiKloodge->bind_param( 1, $kloodge, SQL_INTEGER );
	$editTaxgiKloodge->bind_param( 2, $gi, SQL_INTEGER );

	$result = $editTaxgiKloodge->execute;
  };
  
  if( $@ )
  {
	print LOG "EditTaxgiKLoodgeByGi() failed for gi $gi, kloodge ".
	  "$kloodge. ERROR:$@\n";

	$result = -1;
  }
  return $result;
}

sub EditContigChromidChanged()
{
  my ($gi, $changed) = @_;
  my $result;

  eval
  {
	$editContigChromidUpdate->bind_param( 1, $changed, SQL_INTEGER );
	$editContigChromidUpdate->bind_param( 2, $gi, SQL_INTEGER );

	$result = $editContigChromidUpdate->execute;
  };

  if( $@ )
  {
	print LOG " EditContigChromidChanged() failed for gi $gi. " .
	  "ERROR:$@\n";
  }
  return $result;
}

sub FindContigGiFromGICHROMID()
{
  my $gi = $_[0];
  my $contiggi;
  my @row;

  eval
  {
	$findContigGiFromGICHROMID->bind_param( 1, $gi, SQL_INTEGER );

	$findContigGiFromGICHROMID->execute;

	@row = $findContigGiFromGICHROMID->fetchrow_array;

	$contiggi = $row[ 0 ];
  };

  if( $@ )
  {
	print LOG "FindContigGiFromGICHROMID() failed for gi $gi. " .
	  "ERROR:$@\n";
  }

  return $contiggi;
}

sub LogHistory()
{
  my ($gi, $access, $version ) = @_;
  my $date;

   my ($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst) =
	 localtime(time);

  $date = $year . "-" . $mon . "-" . "$mday"; 
  eval
  {
	$insertHISTDB->bind_param( 1, $gi, SQL_INTEGER );

	$insertHISTDB->bind_param( 2, $access );

	$insertHISTDB->bind_param( 3, $version, SQL_INTEGER );

	$insertHISTDB->bind_param( 4, $date );

	$insertHISTDB->execute;

  };

  if( $@ )
  {
		print LOG "LogHistory failed for gi $gi. " .
	  "ERROR:$@\n";
  }
  
}
# $Log: deletegis.pl,v $
# Revision 1.5  2005/03/18 15:12:29  eburgess
# Log to the history file when we delete the gis. Action = 6 for deletion by
# deletegis.pl.
#
# Revision 1.4  2005/03/16 20:36:08  eburgess
# Select gis from histdb instead of sourcefile when a filename is given.
#
# Revision 1.3  2005/02/28 16:59:37  eburgess
# Fixed typo and use distinct when selecting based on filename.
#
# Revision 1.2  2005/02/28 16:15:25  eburgess
# Modified to take either filename or division and delete all gis from either
# that filename or division. Also delete from the new comgen tables.
#
# Revision 1.1  2005/02/07 17:23:48  zwang
# To delete gis of one division from all core tables.
#
