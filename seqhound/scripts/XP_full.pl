#!/usr/bin/perl

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
#  downloads and updates all XPs
#
use Time::localtime;
use SeqHoundCommon;


$project = "XP";
$maxentries = 10000000;

$tm = localtime;
$year = ($tm->year)+1900;
$month = ($tm->mon)+1;
$day = $tm->mday;
$today = sprintf("%04d%02d%02d", $year, $month, $day);

$path = "/devseqh/home/devseqhi/seq_updates/";
$txtfile = $project.$today."_full.txt";
$htmlfile = $project.".html";
$fulldir = $path.$project;
$fullupdate = $path."update_text";
$fullupdmother = $path."mother_text";
$fulllog = $path."logs";
$logfile = $fulllog."/".$project.".log";

# Start log
open(LOG, ">>".$logfile) || die "Cannot open log '$logfile'";

# Set release according to argument; attempt to check for its validity
$release = @ARGV[0];
if($release < 100 || $release > 200)
{
	$release = $default_release;
	print LOG "Release out of range, using default $default_release.\n";
}

# Download entire XP data
system("wget -o $project.log -O $project.html -t 3 \"http://www.ncbi.nlm.nih.gov/entrez/query.fcgi?cmd=Search&db=Protein&term=srcdb+refseq+model+[Properties]&DISPLAY&doptcmdl=ASN1&dispmax=${maxentries}\"");
if( -z $htmlfile )
{
	Abort("Unable to retrieve HTML data from Entrez\n");
}

open(IN, $htmlfile) || die "Input file '$htmlfile' not found";
open(OUT, ">".$txtfile) || die "Cannot open output '$txtfile'";
$printing = 0;
$entries = 0;
# Wrap as Bioseq-set
print OUT "Bioseq-set ::= { seq-set { \n";
while( <IN> )
{
	# Remove HTML tags from data, 
	# by printing only the Seq-entry section between <pre> and </pre> tags, 
	# subject to further modifications
	$line = $_;
	if( /<pre>Seq-entry/ )
	{
		# Entering data section
		$line =~ s/.*<pre>//;
		$printing = 1;
	} else {
	if( /<\/pre>/ )	
	{
		# Exiting data section
		$printing = 0;
	} }

	if( $printing )
	{
		# Substitute possible Seq-entry with comma
		if( $line =~ s/Seq-entry ::=/,/ )
		{
			if( $entries < 1 )
			{
				# Remove initial comma (substituted Seq-entry)
				$line =~ s/,//;
			}
			$entries++;
		}
		print OUT $line;
	}
}
print OUT "\n} }\n";
close(OUT);
close(IN);
unlink $htmlfile;
if( $entries < 1 )
{
	unlink $txtfile;
	Abort("No entries found in $htmlfile \n");
}


# Update the database with the new data
print LOG "Updating $txtfile \n";
if( system("$fullupdate -i $txtfile") != 0 )
{
	Abort("Cannot update $txtfile (using $fullupdate) \n");
}
print LOG "Entries deleted - Adding $entries new entries... \n";
if( system("$fullupdmother -i $txtfile -r $release -n F -m F -u T") != 0 )
{
	Abort("Cannot update $txtfile (using $fullupdmother) \n");
}


# Clean up
# Zip up data and archive in dir
print LOG "Compressing $txtfile\n";
system("gzip $txtfile");
if( -s $txtfile.".gz" )
{
	system("rm ${fulldir}/${project}????????_full.txt*");
	system("mv $txtfile.gz $fulldir");
}
if( -e $project.$today."_fullrun" )
{
	system("mv ${project}${today}_fullrun $fulllog");
}
if( -e $project.$today."_fulllog" )
{
	system("mv ${project}${today}_fulllog $fulllog");
}
close(OUT);
close(IN);
print LOG "Full $project creation succesful\n";
close(LOG);
