#! /usr/perl5/5.6.1/bin/perl
# $Id $


use strict;
use Text;
use Tee;
use Getopt::Long;
$|=1;

use DBI;
use lib "/export/home/db2inst1/usr/local/perl5/site_perl";
use POSIX qw(locale_h);
use locale;
setlocale(LC_ALL, "en_US.UTF-8");
my $dbh_db2= TextGetMymedHandler();
my $dbh_sh= TextGetSeqhoundHandler();
my $dbh_text= TextGetTexthandler();

my $usage= "Usage: text_update.pl [-r T/F] [-t taxid] [-u table_name]\n";
$usage .=  "\t-r\tredo all searches aginst medline based on time stamp\n";
$usage .=  "\t-t\ttaxonomy ids for the organisms to be updated,seperated by comma.\n";
$usage .=  "\t-u\tupdate target table only, default is all.\n";

my ($taxid, $taxidlist,@taxids);
my $redo="F"; #default is only search new names
my $update="all"; #default is update all tables
my $debug="F";
# command line intepretation
if ($ARGV[0] eq "-"){
    print $usage;
    exit(0);
}else{
    Getopt::Long::Configure ('bundling');
    if( @ARGV > 0){
        GetOptions ('r=s' => \$redo,
                    't=s' => \$taxidlist,
                    'u=s' => \$update,
                    'd=s' => \$debug);
    }
}

my $updatelog=TextGetProperty("update_log");
open (LOG, ">>$updatelog") || die "Can not open update log file\n";
if($debug eq "T" || $debug eq "t"){
    tie *OUT , 'Tie::Tee', *STDOUT, *LOG;
    TextSetLogFile(*OUT);
}

my $myemail=TextGetProperty("my_email");
my $time=localtime;
my $mymail="|/usr/bin/mailx -s \"[$0 @ARGV] @ [$time] \""."  ".$myemail;
open(MAIL, $mymail);
tie *TEE, 'Tie::Tee', *STDOUT, *MAIL, *LOG;
print TEE '>'x70, "\n\n";
print TEE "Output is TEEed to standout, mail, and log.\n";
print TEE "[$0 @ARGV] starts @ [", scalar localtime,"] \n";

TextSetTeeoutput(*TEE);

# update doc tables from mymed
if($update eq "all" or $update eq "text_doc"){
    print TEE "<<TextUpdateDoc starts at [", scalar localtime,"] \n";
    TextUpdateDoc($dbh_text);
    print TEE ">>TextUpdateDoc ends at [", scalar localtime,"] \n\n";
}
if($update eq "all" or $update eq "text_docscore"){
    print TEE "<<TextUpdateDocscore starts at [", scalar localtime,"] \n";
    TextUpdateDocscore($dbh_text);
    print TEE ">>TextUpdateDocscore ends at [", scalar localtime,"] \n\n";
}

# update search tables and do searches
if($update eq "all" or $update eq "text_search"){
    print TEE "<<TextUpdateSearch starts at [", scalar localtime,"] \n";
    TextUpdateSearch($dbh_sh, $dbh_text);
    print TEE ">>TextUpdateSearch ends at [", scalar localtime,"] \n\n";
}

if($update eq "all" or $update eq "text_rng"){
    print TEE "<<TextUpdateRng starts at [", scalar localtime,"] \n";
    TextUpdateRng($dbh_text);
    print TEE ">>TextUpdateRng ends at [", scalar localtime,"] \n\n";
}

if($update eq "all" or $update eq "text_rngresult"){
    print TEE "<<TextUpdateRngresult starts at [", scalar localtime,"] \n";
    TextUpdateRngresult($dbh_db2, $dbh_text);
    print TEE ">>TextUpdateRngresult ends at [", scalar localtime,"] \n\n";
}
if($update eq "all" or $update eq "text_searchscore"){
    print TEE "<<TextUpdateSearchscore starts at [", scalar localtime,"] \n";
    TextUpdateSearchscore($dbh_text);
    print TEE ">>TextUpdateSearchscore ends at [", scalar localtime,"] \n\n";
}

# update result tables for the targeted organism(s)
if($update eq "all" or $update eq "text_doctax"){
    print TEE "<<TextUpdateDoctax starts at [", scalar localtime,"] \n";
    # update result tables for the targeted organism(s)
    if(!$taxidlist){    $taxidlist=TextGetProperty("taxids"); }
    @taxids=split (/,/,$taxidlist);
    foreach $taxid(@taxids){
        print TEE  "<-- $taxid\n";
        TextUpdateDoctax($dbh_db2, $dbh_text, $taxid);
        print TEE "\n$taxid -->\n\n";
    }
    print TEE ">>TextUpdateDoctax ends at [", scalar localtime,"] \n\n";
}

if($update eq "all" or $update eq "text_result"){
    print TEE "<<TextUpdateResult starts at [", scalar localtime,"] \n";
    TextUpdateResult($dbh_text, $taxid);
    print TEE ">>TextUpdateResult ends at [", scalar localtime,"] \n\n";
}

if($update eq "all" or $update eq "text_resultscore"){
    print TEE "<<TextUpdateResultscore starts at [", scalar localtime,"] \n";
    TextUpdateResultscore($dbh_text);
    print TEE ">>TextUpdateResultscore ends at [", scalar localtime,"] \n\n";
}

# update evidence and point tables
# print TEE '='x10, "Update Evidence tables and Scoring", '='x10, "\n";
if($update eq "all" or $update eq "text_namepair"){
    print TEE "<<TextUpdateNamepair starts at [", scalar localtime,"] \n";
    TextUpdateNamepair($dbh_text);
    print TEE ">>TextUpdateNamepair ends at [", scalar localtime,"] \n\n";
}
if($update eq "all" or $update eq "text_evidence"){
    print TEE "<<TextUpdateEvidence starts at [", scalar localtime,"] \n";
    TextUpdateEvidence($dbh_text);
    print TEE ">>TextUpdateEvidence ends at [", scalar localtime,"] \n\n";
}
if($update eq "all" or $update eq "text_point"){
    print TEE "<<TextUpdatePoint starts at [", scalar localtime,"] \n";
    TextUpdatePoint($dbh_text);
    print TEE ">>TextUpdatePoint ends at [", scalar localtime,"] \n\n";
}
if($update eq "all" or $update eq "text_namepairresult"){
    print TEE "<<TextUpdateNamepairresult starts at [", scalar localtime,"] \n";
    TextUpdateNamepairresult($dbh_db2, $dbh_text);
    print TEE ">>TextUpdateNamepairresult ends at [", scalar localtime,"] \n\n";
}
if($update eq "all" or $update eq "text_namepairscore"){
    print TEE "<<TextUpdateNamepairscore starts at [", scalar localtime,"] \n";
    TextUpdateNamepairscore($dbh_text);
    print TEE ">>TextUpdateNamepairscore ends at [", scalar localtime,"] \n\n";
}
if($update eq "all" or $update eq "text_evidencescore"){
    print TEE "<<TextUpdateEvidencescore starts at [", scalar localtime,"] \n";
    TextUpdateEvidencescore($dbh_text);
    print TEE ">>TextUpdateEvidencescore ends at [", scalar localtime,"] \n\n";
}
if($update eq "all" or $update eq "text_pointscore"){
    print TEE "<<TextUpdatePointscore starts at [", scalar localtime,"] \n";
    TextUpdatePointscore($dbh_text);
    print TEE ">>TextUpdatePointscore ends at [", scalar localtime,"] \n\n";
}


$dbh_db2->disconnect;
$dbh_sh->disconnect;
$dbh_text->disconnect;

print TEE "[$0 @ARGV] ends at [", scalar localtime,"] \n";
print TEE '<'x70, "\n\n";

#clean up
close(MAIL);
close(LOG);
exit;
