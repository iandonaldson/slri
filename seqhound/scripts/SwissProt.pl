#! /usr/bin/perl

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
#	Martin Damsbo (mdamsbo@mdsp.com)
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
#  daily Swissprot updater
#

use FindBin; use lib $FindBin::Bin; # reach modules from same dir as script
use SeqHoundCommon;


# Customize
# Database project
$project = "SwissProt";
# Directory with the update executables
$path = "./";
# Maximum number of entries to update
$maxentries = 10000000;
# /Customize

# Open logging and title it
$logpath = $path."logs/";
$logfile = $logpath.$project.".log";
open(LOG, ">".$logfile) || die "Cannot create log '$logfile'";

# Set up absolute paths to executables, update files and databases
$projectpath = $path.$project;
$updateexec = $path."update_text";
$motherexec = $path."mother_text";

# Set release according to argument; attempt to check for its validity
$release = @ARGV[0];
if($release < 100 || $release > 200)
{
	$release = $default_release;
	print LOG "Release out of range, using default $default_release.\n";
}


# Establish date specification
if( scalar(@ARGV) > 2 && $ARGV[2] =~ /(\d{4}).(\d{2}).(\d{2})/ )
{
	$todate = $ARGV[2];
}
if( scalar(@ARGV) > 1 && $ARGV[1] =~ /(\d{4}).(\d{2}).(\d{2})/ )
{
	if( defined $todate )
	{
		$fromdate = $ARGV[1];
	}
	else
	{
		# A single parameter is interpreted as a to_date
		$todate = $ARGV[1];
	}
}
if( ! defined $fromdate )
{
	# Attempt to derive from_date from previous update file
	print LOG "No from_date specified - deriving from previous update file. \n";
	$fromdate = PreviousUpdateDate($projectpath, $project);
}
if( ! defined $todate )
{
	# Make today the to_date
	print LOG "No to_date specified - using today's date. \n";
	$todate = TodayDate();
}
$datespec = sprintf("%s:%s", $fromdate, $todate);
#if( DateEpochSeconds(fromdate) > DateEpochSeconds(todate) )
#{
#	Abort("Invalid date specification: $datespec \n");
#}

# Construct update file names
$todate_noslash = $todate;
$todate_noslash =~ s/\///g;
$name = $project.$todate_noslash;
$htmlfile = $path.$name.".html";
$txtfile = $path.$name.".asn";


print LOG "SeqHound ${project} update (", scalar(localtime), ", release ${release}) for ${fromdate} - ${todate} : \n\n";
Flush(*LOG);

# Download HTML update file from NCBI repository
if( ! GetNCBIdataHTML("srcdb+swiss-prot", $datespec, $htmlfile, $logfile, "ASN1") )
{
	if( -s $htmlfile )
	{
		unlink $htmlfile;
	}
	Abort("Unable to retrieve HTML data from Entrez \n", $warning);
}

# Filter HTML update file to clean ASN1 format
$entries = HtmlFilterASN1($htmlfile, $txtfile);
if( -s $htmlfile )
{
	unlink $htmlfile;
}
if( $entries < 1 )
{
	if( -s $txtfile )
	{
		unlink $txtfile;
	}
	Abort("No entries found in $htmlfile \n");
}


# Update the database with the new data
print LOG "Updating $txtfile \n";
if( system("$updateexec -i $txtfile") != 0 )
{
	Abort("Cannot update $txtfile (using $updateexec) \n");
}
print LOG "Entries deleted - Adding $entries new entries... \n";
if( system("$motherexec -i $txtfile -r $release -n F -m F -u T ") != 0 )
{
	Abort("Cannot update $txtfile (using $motherexec) \n");
}


# Clean up
# Zip up data and archive in dir, removing previous archive
print LOG "Compressing $txtfile \n";
if( -s $txtfile )
{
	system("gzip $txtfile"); # zip data file in-place
#	system("gzip -c $txtfile > $txtfile.gz"); # leave data file unzipped for further processing (and deletion)
	if( glob($projectpath."/".$project."????????.*") )
	{
		system("rm ${projectpath}/${project}????????.*"); # remove previous archived data
	}
	if( -s $txtfile.".gz" )
	{
		system("mv ${txtfile}.gz ${projectpath}"); # move new archived data
	}
}
else
{
	Abort("No filtered file $txtfile \n");
}
# Move various log files to log dir
$updatelog = $project.$todate_noslash."log";
if( -e $updatelog )
{
	system("mv $updatelog $logpath");
}
$motherlog = $project.$todate_noslash."run";
if( -e $motherlog )
{
	system("mv $motherlog $logpath");
}
print LOG "Download and filtering succesful\n";
close(LOG);
if( $warning )
{
	MailLog(); # Mail entire log to admin in case of warnings
}
