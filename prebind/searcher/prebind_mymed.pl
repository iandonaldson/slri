#!/usr/bin/perl -w

# =============================================================================
#               Documentaion
# =============================================================================
# File Description: 
#  Use in-house medline to search name/taxid for pmid.
# Command syntax: 
#  perl prebind_mymed.pl from_nameid to_nameid
#for example: 
#  /usr/perl5/5.6.1/bin/perl prebind_mymed.pl 1 100
# ==============================================================================

use strict;
my $seperator= "-"x60;
my $stopwordsfile = "pubmed-stopwords";
my $taxid; #default mesh = yeast
my %taxnames=(	4932=>"Saccharomyces cerevisiae", 
				9606=>"human",
            	10090=>"mice",
				10116=>"rats",
				7227=>"Drosophila melanogaster",
				6239=>"Caenorhabditis elegans");
open STOPWORDS, "$stopwordsfile" || die "can't open pubmed stop words file: $!\n";
my %stopwords;
my $word;
foreach $word(<STOPWORDS>){
	chomp($word);
	$stopwords{$word}=1;
}


use lib "/export/home/db2inst1/usr/local/perl5/site_perl";
use DBI;
use POSIX qw(locale_h);
use locale;
setlocale(LC_ALL, "en_US.UTF-8");
my $dbh_db2 = DBI->connect("dbi:DB2:medprod","db2inst1", "kl2es4l", 
	{RaiseError => 0, AutoCommit => 1 });
$dbh_db2->{LongReadLen}=20*1024;
$dbh_db2->{LongTruncOk}=1;
my $dbh_pb= DBI->connect ("DBI:mysql:prebind:marge:33306", "kzheng", "asdf1234", 
    {RaiseError => 0, AutoCommit => 1})

# ------------------------------------------------------------------------------
#   Go through SRCH table,create RESULT using mymed API
# ------------------------------------------------------------------------------
my ($from, $to)=(0, 10);
$from=$ARGV[0];
$to=$ARGV[1];
my ($sth_select_names, $sth_select_count, $sth_select_pmids, $sth_update_count, $sth_insert_pmid);
$sth_select_names=$dbh_pb->prepare("select name, taxid from prebind.name where id>$from  and id<$to;");
$sth_select_names->execute();
while(my ($name, $taxid)=$sth_select_names->fetchrow_array){
	

foreach $row(<IN>) {
	next if $row !~ /^(\d+)\t(\S+)$/;
	$searchid=$1;
	$name = $2;
#next if $name =~/\s/; # no space for name
	next if $name !~ /[A-Za-z]/; #if name are only a number or doesnot make sense
	next if $name =~ /'/; #can not deal with single quote in db2 now
    my $lcname=lc($name);
	next if (exists($stopwords{$lcname})); # filter out stop words 
	next if (exists($stopwords{$name})); # filter out stop words 
    
    #deal with special symbols 
    my $mesh = $taxnames{$taxid}; 
    print LOG "$seperator \n";    
    print LOG "$searchid\t$name\t$taxid\t$mesh\n";
    #print "$gip\t$name\t$taxid\t$mesh\n";
  
    # Create SQL statement to call stored procedure
	my $stmt_db2="	SELECT pmid FROM medline.citations ".
        " WHERE contains(citation,\'section(\"mesh\") \"$mesh\" & sections(\"abstracts\", \"titles\")\"$name\"\')=1;";

    #prepare the statement
    my $sth_db2=$dbh_db2->prepare($stmt_db2) ;  #not die, skip
	if (!$sth_db2) {
        ## print DB2 error message and exit ##
        print "Couldn't prepare statement.\n";
        #print $db->errstr,"\n";
        next;
    }
    # Execute the SQL statement 
    $sth_db2->execute();#not die, skip
    if (!$sth_db2) {
        ## print DB2 error message and exit ##    
        print "Couldn't execute statement: " . $sth_db2->errstr;
        #print $db->errstr,"\n";
        next;
    }
    #print "---\n";
    # Fetch the resulting records and print them out to the output file
    my @data;
    while (@data = $sth_db2->fetchrow_array()) {
        my $pmid_match=$data[0];
		print LOG "$pmid_match\n";
        print OUT "$searchid\t$name\t$pmid_match\n"
    }
    # Close the statement handle
    print LOG "\n";
    $sth_db2->finish;
}

$dbh_db2->disconnect;
$dbh_pb->disconnect;
