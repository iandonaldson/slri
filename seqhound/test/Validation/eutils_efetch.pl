#!/usr/local/bin/perl -w
use Getopt::Std;

# ===========================================================================
#
#                            PUBLIC DOMAIN NOTICE
#               National Center for Biotechnology Information
#
#  This software/database is a "United States Government Work" under the
#  terms of the United States Copyright Act.  It was written as part of
#  the author's official duties as a United States Government employee and
#  thus cannot be copyrighted.  This software/database is freely available
#  to the public for use. The National Library of Medicine and the U.S.
#  Government have not placed any restriction on its use or reproduction.
#
#  Although all reasonable efforts have been taken to ensure the accuracy
#  and reliability of the software and data, the NLM and the U.S.
#  Government do not and cannot warrant the performance or results that
#  may be obtained by using this software or data. The NLM and the U.S.
#  Government disclaim all warranties, express or implied, including
#  warranties of performance, merchantability or fitness for any particular
#  purpose.
#
#  Please cite the author in any work or product based on this material.
#
# ===========================================================================
#
# Author:  Oleg Khovayko http://olegh.spedia.net
#
# File Description: eSearch/eFetch calling example
#
# Contributor: Rong Yao (ryao@blueprint.org)
#
# Contributor's notes: This file is original called eutils_example.pl. 
# http://eutils.ncbi.nlm.nih.gov/entrez/query/static/UserSystemRequirements
# I added getopts part instead of using ask_user funcion. This modification
# is for blueprint internal SeqHound data validation project use.  
#  
# ---------------------------------------------------------------------------
# Subroutine to prompt user for vari�bles in the next section

sub ask_user {
  print "$_[0] [$_[1]]: ";
  my $rc = <>;
  chomp $rc;
  if($rc eq "") { $rc = $_[1]; }
  return $rc;
}

# ---------------------------------------------------------------------------
# Define library for the 'get' function used in the next section.
# $utils contains route for the utilities.
# $db, $query, and $report may be supplied by the user when prompted; 
# if not answered, default values, will be assigned as shown below.

use LWP::Simple;
use vars qw/ %opt /;

my $utils = "http://www.ncbi.nlm.nih.gov/entrez/eutils";

#Rong added:
# -d database to use
# -q query
# -r report (e.g. fasta, acc, gb) 
getopts('d:q:r:', \%opt );
my $db = $opt{d};
my $query = $opt{q};
my $report = $opt{r};

#print "$db\n$query\n$report\n";

# ---------------------------------------------------------------------------
# $esearch cont�ins the PATH & parameters for the ESearch call
# $esearch_result containts the result of the ESearch call
# the results are displayed �nd parsed into variables 
# $Count, $QueryKey, and $WebEnv for later use and then displayed.

my $esearch = "$utils/esearch.fcgi?" .
              "db=$db&retmax=1&usehistory=y&term=";

my $esearch_result = get($esearch . $query);

#print "\nESEARCH RESULT: $esearch_result\n";

$esearch_result =~ 
  m|<Count>(\d+)</Count>.*<QueryKey>(\d+)</QueryKey>.*<WebEnv>(\S+)</WebEnv>|s;

my $Count    = $1;
my $QueryKey = $2;
my $WebEnv   = $3;

#print "Count = $Count; QueryKey = $QueryKey; WebEnv = $WebEnv\n";
my $retstart = 0;
my $retmax = 20; #can modify here
my $efetch = "$utils/efetch.fcgi?" .
               "rettype=$report&retmode=text&retstart=$retstart&retmax=$retmax&" .
               "db=$db&query_key=$QueryKey&WebEnv=$WebEnv";
	
my $efetch_result = get($efetch);
chomp $efetch_result;  
print $efetch_result;
