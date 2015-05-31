#!/usr/bin/perl -w 
# =============================================================================
#               Documentaion
# =============================================================================
# File Description: 
#       Use in-house medline to search names for pmid.
# Command syntax: 
#       perl prebind_pubmed.pl  infile outfile [-t taxid] [-m max] 
# ==============================================================================

use strict;
use LWP::Simple;
use Getopt::Long;
use Term::ReadKey;
use lib '/home/kzheng/mylib/myperlmodules/DBD-XBase-0.241/lib';
use XBase;

my $fin = $ARGV[0];
my $fout = $ARGV[1];
my $log = "prebind_pubmed.log";
my $mesh= "";
my $taxid=0;
my $max=0;
# retmax=y  (y= number of items retrieved - default=20)
#my $retmax=10000; # The maximum number of retrieved records is 10,000.
my $retmax=500;
my $eutils = "http://www.ncbi.nlm.nih.gov/entrez/eutils";
#Requests utility to maintain results in user's environment. Used in conjunction with WebEnv. 
my $esearch= "$eutils/esearch.fcgi?db=pubmed&retmax=$retmax&usehistory=y&term=";
my $seperator = "-"x60;
#open (OUT, "> $fout") || die "Can't open output file for write: $!\n";
open (LOG,"> $log") || die "Can't open log file for write: $!\n";

Getopt::Long::Configure ('bundling');
if ( @ARGV > 0 ) {
	GetOptions ('t=i' => \$taxid,
            	'm=i' => \$max);
}

if($taxid == 4932){
	$mesh = "Saccharomyces_cerevisiae";
}
elsif($taxid == 9606){
	$mesh = "human";
}
elsif ($taxid == 10060){
	$mesh = "mice";
}
else{
	print "invalid taxid\n";
    exit(0);
}

# create RESULT table for write
my $new_table = XBase->create("name" => "$fout",
                "field_names" => [ "GI_PROT", "NAME", "PMID" ],
                "field_types" => [ "N", "C", "N" ],
                "field_lengths" => [ 20, 40, 20 ],
                "field_decimals" => [ 0, undef, 0 ]);

# ------------------------------------------------------------------------------
#   Go through SRCH table,get RESULT using pubmed
# ------------------------------------------------------------------------------
print LOG "="x10,"[ ", scalar localtime, " ]", "="x10, "\nprebind_pubmed starts.\n";
print LOG "Start processing the SRCH table. \n";

my $table = new XBase "$fin" or die XBase->errstr;

my $i = 0;
for (0 .. $table->last_record) {
	last unless $i < $max;
    my ($deleted, $gip, $name, $taxon) = $table->get_record($_, "GI_PROT", "NAME", "TAXID");
	print "$gip \t$name \t$taxon\n" unless $deleted;
    next if $taxon != $taxid;
    
    my $pmidlist=" ";# space at the beginning
    $i++;
    print LOG "$seperator\n";
    print LOG "$gip \t$name \t$taxon\n";
    my $quoted_name = "\"" . $name . "\"";
    #my $query = $quoted_name . "[word]+" . $mesh . "[mesh]";
    #Note: WebEnv is similar to the cookie
    my $query = $quoted_name . "[Title/Abstract]+ \"" . $mesh . "\"[mesh]";#quoted mesh too
    # "hominidae"[mesh] AND "human"[mesh]; #=2213,   "hominidae"[mesh], #=2214, so  "hominidae" can be ignored

    print "SRCH term=$query\n";
    print LOG "SRCH term=$query\n";
    print LOG "$esearch$query\n";
	my $esearch_result = get($esearch . $query);

    #print "\nESEARCH RESULT: $esearch_result\n";

    $esearch_result =~ m|<Count>(\d+)</Count>.*<IdList>(.*)</IdList>|s;
    
    my $Count  = $1;
    my $IdList = $2;
    #print LOG "Count = $Count; QueryKey = $QueryKey; WebEnv = $WebEnv, IdList = $IdList\n";
    print LOG "Count = $Count; IdList = $IdList\n";
    $IdList =~s/\s+//g;
    while ($IdList =~ s/\<Id\>(\d+)\<\/Id\>//){
        my $row = $new_table->last_record +1;
        $new_table->set_record($row, $gip, $name, $1) or print $new_table->errstr, 'not ';
        $pmidlist.="$1 "; 
    }
    
    my $retstart; 
    #retstart=x (x= sequential number of the first id retrieved - default=0 which will retrieve the first record) 
    for($retstart = $retmax; $retstart < $Count; $retstart +=$retmax) {
      my $query2 = $query."&retstart=$retstart";
		print LOG "$esearch$query2\n";
      my $esearch_result2 = get($esearch.$query2);
      $esearch_result2 =~ m|<Count>(\d+)</Count>.*<IdList>(.*)</IdList>|s;
		my $Count2 = $1;
        my $IdList2 = $2;
        print LOG "Count2 = $Count2; IdList2 = $IdList2\n";
        $IdList2 =~s/\s+//g;
        while ($IdList2 =~ s/\<Id\>(\d+)\<\/Id\>//){
            my $row2 = $new_table->last_record +1;
            $new_table->set_record($row2, $gip, $name, $1) or print $new_table->errstr, 'not ';
            $pmidlist.="$1 "; 
        }  
    }
    
    #print "\tCount = $Count\n\tIdList = $IdList\n\tPMID list= $pmidlist\n";
    print LOG "\tCount = $Count\n\tPMID list=$pmidlist\n\n";
 
}
print LOG "="x10,"[ ", scalar localtime, " ]", "="x10, "\nprebind_pubmed stops.\n";
close(LOG);
$table->close;
$new_table->close;

#
#Examples:
#Search in PubMed for the term cancer for the entrez date from the last 60 days and retrieve the first 100 IDs and translations using the history parameter:
#http://eutils.ncbi.nlm.nih.gov/entrez/eutils/esearch.fcgi?db=pubmed&term=cancer&reldate=60&datetype=edat&retmax=100&usehistory=y

#Search in PubMed for the journal PNAS Volume 97, and retrieve 6 IDs starting at ID 7 using a tool parameter:
#http://eutils.ncbi.nlm.nih.gov/entrez/eutils/esearch.fcgi?db=pubmed&term=PNAS[ta]+AND+97[vi]&retstart=6&retmax=6&tool=biomed3

#Search in Journals for the term obstetrics:
#http://eutils.ncbi.nlm.nih.gov/entrez/eutils/esearch.fcgi?db=journals&term=obstetrics

#Search in PubMed Central for stem cells in free fulltext articles:
#http://eutils.ncbi.nlm.nih.gov/entrez/eutils/esearch.fcgi?db=pmc&term=stem+cells+AND+free+fulltext[filter]
