#!/usr/perl5/5.6.1/bin/perl
# $Id $

use strict;
use lib "/export/home/db2inst1/usr/local/perl5/site_perl";
use DBI;
use Text;
use Tee;
use POSIX qw(locale_h);
use locale;
setlocale(LC_ALL, "en_US.UTF-8");
$|=1;

my ($min, $max)=(0,0);
if($#ARGV==2){$min=$ARGV[0]; $max=$ARGV[1];}

my $updatelog=TextGetProperty("update_log");
open (LOG, ">>$updatelog.$min.$max") || die "Can not open update log file\n";

my $myemail=TextGetProperty("my_email");
my $mymail="|/usr/bin/mailx -s \"Text update cron status\""."  ".$myemail;
open(MAIL, $mymail);
print MAIL "Program starts at [", scalar localtime,"] \n";

tie *TEE, 'Tie::Tee', *STDOUT, *MAIL, *LOG;
print TEE '>'x70, "\n\n";
print TEE "Output is TEEed to standout, mail, and log.\n";
print TEE "Program starts at [", scalar localtime,"] \n";

my $dbh_db2= TextGetMymedHandler();
my $dbh_text= TextGetTexthandler();
TextUpdateRngresultToFile($dbh_db2, $dbh_text, "F", "pmc.rngresult", $min, $max);


$dbh_db2->disconnect;
$dbh_text->disconnect;

print TEE "Program ends at [", scalar localtime,"] \n";
print TEE '<'x70, "\n\n";


#clean up
close(MAIL);
close(LOG);
exit;



sub TextUpdateRngresultToFile {
    my ($dbh_db2, $dbh_text, $redo, $file, $minid, $maxid)=@_;
    my $threshold=TextGetProperty("threshold");
    open (OUT, ">>$file.$minid.$maxid") || die "Can not open update out file\n";
    open (SUM, ">>$file.SUM.$minid.$maxid") || die "Can not open update out file\n";
    my $total=0;
    my $count=0;
    my ($rangemin, $rangemax)=("", "");
    if($minid>0){ $rangemin=" and id>=$minid "; }
    if($maxid>0){ $rangemax=" and id<=$maxid "; }
    my $range=$rangemin." ".$rangemax;
    my $stmt_count_rng="select count(id) from text_rng where results>0 and results<1000 $range";
    if($redo eq "T"){   $stmt_count_rng="select count(*) from text_rng "; }
    if(my @a=$dbh_text->selectrow_array($stmt_count_rng)){ $total = $a[0]; }
    print "----- TOTAL NAMES:$total -----\n";

    my $stmt_select_rng="select id, name from text_rng where  results>0 and results<1000 $range";
    if ($redo eq "T"){  $stmt_select_rng="select id, name from text_rng"; }
    my $sth_select_rng=$dbh_text->prepare($stmt_select_rng);
    my $sth_search_medline;

    $sth_select_rng->execute;
    while (my ($rngid, $name)=$sth_select_rng->fetchrow_array){
        $count++;
        print "== [$name] ==\n";
        my $text="\'\"$name\"\'";
        # search MyMed
        my $stmt_count="SELECT count(pmid) FROM medline.articles WHERE contains(article,$text)=1";
        if(my @data=$dbh_db2->selectrow_array($stmt_count)){
            my $n_pmid=$data[0];
        #    $sth_update_rng->execute($n_pmid, $rngid);
            $dbh_text->do("update text_rng set state=1,searched=now(),results=$n_pmid where id=$rngid");
            print SUM "$rngid\t$n_pmid\n";
            print "==[$count/$total]\t$name\t$n_pmid==\n";
            if ($n_pmid>$threshold){
                print "== [stopword?]==\n";
                if(my @row=$dbh_text->  selectrow_array("select word from text_englishdict where word='$name'") ){
                    print "ENGLISH\n";
                    next;
                }
            }
            next if length($name)<=2; # only check for numbers of results
            next if ($n_pmid==0);
        }

        my $stmt = "SELECT pmid,numberofmatches(article, $text) FROM medline.articles WHERE contains(article,$text)=1 ";
        #print "$stmt\n";
        $sth_search_medline=$dbh_db2->prepare($stmt);
        $sth_search_medline->execute;
        while(my ($pmid, $matches)=$sth_search_medline->fetchrow_array){
            print OUT "$rngid\t$pmid\t$matches\n";
            $dbh_text->do("insert into text_rngresult(rngid, pmid, matches) values($rngid, $pmid, $matches)");
        }

        $sth_search_medline->finish;
    }
    $sth_select_rng->finish;
    print "$count new text_rng names are searched\n";
    close(OUT);
    close(SUM);
}



