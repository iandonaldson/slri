#!/usr/perl5/5.6.1/bin/perl
# $Id $
#---------------------------------------------------
# run multiple instances to reduce update time
# text_mymed.pl minid maxid 
#----------------------------------------------------

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
if($#ARGV==1){$min=$ARGV[0]; $max=$ARGV[1];}

TextSetPropertyFile("text_mymed.ini");
my $updatelog=TextGetProperty("update_log");
if($min>0||$max>0){ $updatelog="$updatelog.$min.$max"; }
open (LOG, ">>$updatelog") || die "Can not open update log file\n";

my $myemail=TextGetProperty("my_email");
my $time=localtime;
my $mymail="|/usr/bin/mailx -s \"[$0 @ARGV] @ [$time] \""."  ".$myemail;
open(MAIL, $mymail);
tie *TEE, 'Tie::Tee', *STDOUT, *MAIL, *LOG;
print TEE '>'x70, "\n\n";
print TEE "Output is TEEed to standout, mail, and log.\n";
print TEE "[$0 @ARGV] starts at [", scalar localtime,"] \n";

my $dbh_db2= TextGetMymedHandler();
my $dbh_text= TextGetTexthandler();
TextSetTeeoutput(*TEE);
my $redo="F";
TextUpdateRngresult($dbh_db2, $dbh_text, $redo, $min, $max);

$dbh_db2->disconnect;
$dbh_text->disconnect;

print TEE "[$0 @ARGV] ends at [", scalar localtime,"] \n";
print TEE '<'x70, "\n\n";


#clean up
close(MAIL);
close(LOG);
exit;



