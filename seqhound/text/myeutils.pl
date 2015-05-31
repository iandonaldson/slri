#!/usr/bin/perl -w 
# =============================================================================
#  perl myetils.pl pubmed canada[mh]+canada[ad] >canada.mhad.pubmed
# ==============================================================================

use strict;
use LWP::Simple;

my $db=$ARGV[0];
my $query=$ARGV[1];
my $pmidlist="";
# retmax=y  (y= number of items retrieved - default=20)
my $retmax=10000; # The maximum number of retrieved records is 10,000.
#my $retmax=500;
my $eutils = "http://www.ncbi.nlm.nih.gov/entrez/eutils";
#Requests utility to maintain results in user's environment. Used in conjunction with WebEnv. 
my $esearch= "$eutils/esearch.fcgi?db=$db&retmax=$retmax&usehistory=y&term=";
my $sep = "-"x6;

#my $query = $quoted_name . "[word]+" . $mesh . "[mesh]";
#Note: WebEnv is similar to the cookie
#my $query = "breast cancer"; #quoted mesh too
print "$esearch$query\n";
my $esearch_result = get($esearch . $query);

#print "\nESEARCH RESULT: $esearch_result\n";
$esearch_result =~ m|<Count>(\d+)</Count>.*<IdList>(.*)</IdList>|s;

my $Count  = $1;
my $IdList = $2;
#print "Count = $Count; QueryKey = $QueryKey; WebEnv = $WebEnv, IdList = $IdList\n";
#print "Count = $Count; IdList = $IdList\n";
$IdList =~s/\s+//g;
while ($IdList =~ s/\<Id\>(\d+)\<\/Id\>//){
    $pmidlist.="$1\n"; 
}

my $retstart; 
#retstart=x (x= sequential number of the first id retrieved - default=0 which will retrieve the first record) 
for($retstart = $retmax; $retstart < $Count; $retstart +=$retmax) {
  my $query2 = $query."&retstart=$retstart";
    #  print  "$esearch$query2\n";
  my $esearch_result2 = get($esearch.$query2);
  $esearch_result2 =~ m|<Count>(\d+)</Count>.*<IdList>(.*)</IdList>|s;
    my $Count2 = $1;
    my $IdList2 = $2;
    # print  "Count2 = $Count2; IdList2 = $IdList2\n";
    $IdList2 =~s/\s+//g;
    while ($IdList2 =~ s/\<Id\>(\d+)\<\/Id\>//){
        $pmidlist.="$1\n"; 
    }
}

#print "\tCount = $Count\n\tIdList = $IdList\n\tPMID list= $pmidlist\n";
print "PMID\n$sep\n$pmidlist$sep\n$Count rows in set\n";


