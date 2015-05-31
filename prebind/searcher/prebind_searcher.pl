#! /usr/perl5/5.6.1/bin/perl

# $Id $
#*******************************************************************************
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
#  PROGRAM: prebindRegex.pl
#
#  AUTHORS: Kai Zheng (kzheng@mshri.on.ca)
#           Ian Donaldson (ian.donaldson@mshri.on.ca)
#           and Christopher W.V. Hogue (hogue@mshri.on.ca)
#
#  REVISION/CONTRIBUTION NOTES:
#
#   July 1, 2004  Original MSH Revision.
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
#  DESCRIPTION:    Pre Biomolecular Interaction Network Database (PreBIND)
#                  PreBIND regular expression classifier
#
#******************************************************************************

use strict;
use PreBIND; 
use DBI;
use lib "/export/home/db2inst1/usr/local/perl5/site_perl";
use POSIX qw(locale_h);
use locale;
setlocale(LC_ALL, "en_US.UTF-8");
use Getopt::Long;
use Tee;

my $usage= "Usage: prebind_searcher.pl [-r T/F] [-t taxid]\n";
$usage .=  "\t-r\tredo all searches aginst medline based on time stamp\n";
$usage .=  "\t-t\ttaxonomy id for the organism to be searcher.\n";

my $taxid;
my $redo;
# command line intepretation
if ($ARGV[0] eq "-"){
	print $usage;
	exit(0);
}else{
	Getopt::Long::Configure ('bundling');
	if( @ARGV > 0){
		GetOptions ('r=s' => \$redo,
                    't=i' => \$taxid);
	}
}


# Get properties
my $ini="PreBIND.ini";
#my $ini=".prebindrc.ini";

my $updatelog=PbGetProperty($ini, "update_log");
open (LOG, ">>$updatelog") || die "Can not open update log file\n";

my $myemail=PbGetProperty($ini, "my_email");
my $mymail="|/usr/bin/mailx -s \"PreBIND searcher update cron status\""."  ".$myemail;
open(MAIL, $mymail);
print MAIL "Program starts at [", scalar localtime,"] \n"; 

tie *TEE, 'Tie::Tee', *STDOUT, *MAIL, *LOG; 
print TEE "This line goes both places.\n";

print TEE "Program starts at [", scalar localtime,"] \n";
my @taxids;
if($taxid){
	push @taxids, $taxid; 
}else{
	my $list=PbGetProperty($ini, "taxids");
	@taxids=split (/;/,$list);
}

PbInit($ini);

# update search table from name tables
PbUpdateSearch();
print TEE "PbUpdateSearch ends at [", scalar localtime,"] \n";
PbUpdateRng();
print TEE "PbUpdateRng ends at [", scalar localtime,"] \n";


# search MyMED and update result table
if($redo and ($redo eq "T" or $redo eq "t")){
	foreach $taxid(@taxids){
		PbDoSearchByDate($taxid);
		print TEE "PbDoSearchByDate for $taxid ends at [", scalar localtime,"] \n";
	}
}
else{
	foreach $taxid(@taxids){
		PbDoSearch();
		print TEE "PbDoSearch for $taxid ends at [", scalar localtime,"] \n";

	}
}

PbUpdateResult($taxid);
print TEE "PbUpdateResult for $taxid ends at [", scalar localtime,"] \n";

PbUpdateEvidence($taxid);
print TEE "PbUpdateEvidence for $taxid ends at [", scalar localtime,"] \n";

# update scores
#

print TEE "Program ends at [", scalar localtime,"] \n";
print TEE '='*70, "\n\n";
#clean up
PbFini();
close(LOG);
close(MAIL);

exit;
