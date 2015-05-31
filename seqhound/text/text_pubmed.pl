#!/usr/bin/perl -w 
#---------------------------------------------------
# run from 9pm-5pm, 4800 names each run
# 600 names per hours (3 sec per query, 6 sec per name)
#----------------------------------------------------
use strict;
use LWP::Simple;
#use NCBI_PowerScripting;
use DBI;
use Text;
use Tee;
use  Getopt::Long;
$|=1;

my $usage= "Usage: text_pubmed.pl [-f from rngid] [-t to rngid] [-l limit runing count]\n";
my $default_limit=100;
my ($from, $to, $limit)=(0,0,$default_limit);
if ($ARGV[0] eq "-"){
    print $usage;
    exit(0);
}else{
    Getopt::Long::Configure ('bundling');
    if( @ARGV > 0){
        GetOptions ('f=i' => \$from,
                    't=i' => \$to,
                    'l=i' => \$limit);
    }
}

TextSetPropertyFile("text_pubmed.ini");
my $updatelog=TextGetProperty("update_log");
if($from>0||$to>0) {$updatelog="$updatelog.$from.$to" ;}
open (LOG, ">>$updatelog") || die "Can not open update log file\n";

my $myemail=TextGetProperty("my_email");
my $time=localtime;
my $mymail="|/usr/bin/mailx -s \"[$0 @ARGV] @ [$time] \""."  ".$myemail;
open(MAIL, $mymail);

tie *TEE, 'Tie::Tee', *STDOUT, *MAIL, *LOG;
print TEE '>'x70, "\n\n";
print TEE "Output is TEEed to standout, mail, and log.\n";
print TEE "[$0 @ARGV] starts at [", scalar localtime,"] \n";

my $dbh_text= TextGetTexthandler();
TextUpdateRngresultFromPubmed($dbh_text, "F", "pubmed.rngresult", $from, $to, $limit, $myemail);
$dbh_text->disconnect;

print TEE "[$0 @ARGV] ends at [", scalar localtime,"] \n";
print TEE '<'x70, "\n\n";


#clean up
close(MAIL);
close(LOG);
exit;





sub TextUpdateRngresultFromPubmed{
    my ($dbh_text, $redo, $file, $from, $to, $limit, $email)=@_;
    my $retmax=10000;
    my $base = "http://eutils.ncbi.nlm.nih.gov/entrez/eutils/";
    my $pubmedurl = $base."/esearch.fcgi?db=pubmed&retmax=$retmax&usehistory=y&tool=SeqHound&email=$email&term=";
    my $pmcurl= $base."/esearch.fcgi?db=pmc&retmax=$retmax&usehistory=y&tool=SeqHound&email=$email&term=";
    my $total=0;
    my $count=0;
    my ($begin, $end);
    my $delay = 0;
    my $maxdelay = 3;
    my $numberperhour=600;

    my ($rangemin, $rangemax, $rangelimit)=("", "", "");
    if($from>0){ $rangemin=" and id>=$from "; }
    if($to>0){ $rangemax=" and id<=$to "; }
    if($limit>0){ $rangelimit=" limit $limit";}
    my $range=$rangemin.$rangemax.$rangelimit;

    my $stmt_count_rng="select count(id) from text_rng where state>0 $range";
    if(my @a=$dbh_text->selectrow_array($stmt_count_rng)){ $total = $a[0]; }
    print TEE "----- TOTAL NAMES:$total -----\n";

    my $stmt_select_rng="select id, name from text_rng where state>0 $range";
    print TEE "$stmt_select_rng\n";
    my $sth_select_rng=$dbh_text->prepare($stmt_select_rng);
    my $sth_search_medline;
    $sth_select_rng->execute;
    while (my ($rngid, $name)=$sth_select_rng->fetchrow_array){
        $count++;
        if($count % $numberperhour==0){ print TEE "$count @ [", scalar localtime,"]\n";  }
        my $name2=$name;
        $name2=~s/ /\+/g;
        print TEE "[$count / $total]:[$name]\n";

        # search PubMed
        my $C1=1;
        my $term="$name2"."[TIAB]";
        for(my $retstart = 0; $retstart < $C1; $retstart +=$retmax) {
            sleep($delay);
            $begin = time;
            my $query = $pubmedurl.$term."&retstart=$retstart";
            # print "$query\n";
            my $esearch_result = get($query);
            $esearch_result =~ m|<Count>(\d+)</Count>.*<IdList>(.*)</IdList>|s;
            $C1 = $1;
            if ($C1>10000){
                print "Skip results\n";
                next;
            }
            print "$delay: $C1\n";
            if( $C1!=0){
                my $IdList = $2;
                $IdList =~s/\s+//g;
                while ($IdList =~ s/\<Id\>(\d+)\<\/Id\>//){
                    my $pmid=$1; 
                    print ">>$rngid\t$pmid\t-1";
                    $dbh_text->do("insert into text_rngresult(rngid, pmid, matches) values($rngid, $pmid, -1)");
                }
            }
            $end = time;
            $delay = $maxdelay - ($end - $begin);
            if ($delay < 0) { $delay = 0; }
            # print "$begin:$end:$delay\n";
        }

        # search pmc
        $term="$name2"."[articlebody]";
        my $C2=1;
        for(my $retstart = 0; $retstart < $C2; $retstart +=$retmax) {
            sleep($delay);
            $begin = time;
            my $query = $pmcurl.$term."&retstart=$retstart";
            # print "$query\n";
            my $esearch_result = get($query);
            $esearch_result =~ m|<Count>(\d+)</Count>.*<IdList>(.*)</IdList>|s;
            $C2 = $1;
            if ($C2>10000){
                print "Skip results\n";
                next;
            }
            print "$delay: $C2\n";
            if ($C2!=0){
                my $IdList = $2;
                $IdList =~s/\s+//g;
                while ($IdList =~ s/\<Id\>(\d+)\<\/Id\>//){
                    my $pmid=$1; 
                    print ">>$rngid\t$pmid\t-2";
                    $dbh_text->do("insert into text_rngresult(rngid, pmid, matches) values($rngid, $pmid, -2)");
                }
            }
            $end = time;
            $delay = $maxdelay - ($end - $begin);
            if ($delay < 0) { $delay = 0; }
            # print "$begin:$end:$delay\n";
        }
        my $results=$C1+$C2;
        $dbh_text->do("update text_rng set state=0,searched=now(),results=$results where id=$rngid");
    }
    $sth_select_rng->finish;
    print TEE "$count new text_rng names are searched\n";
}

