#!/usr/bin/perl -w
# Syntax: ./prebind_export.pl infile outfile logfile -f[field-list] -c[constrain-list]
#example: ./prebind_export.pl POINT.dbf P.txt P.log -f fields.txt -c check.txt
use strict;
use Getopt::Long;
use Term::ReadKey;

#Perl module for reading and writing the dbf files
use lib '/home/kzheng/mylib/myperlmodules/DBD-XBase-0.241/lib';
use XBase;
use IO::Handle;


my $infile=$ARGV[0];
my $outfile=$ARGV[1];
my $logfile = $ARGV[2];
my $table = new XBase "$infile" or die XBase->errstr; 
open LOG,"> $logfile" || die "Can't open log file for write: $!\n";
open OUT,"> $outfile" || die "Can't open output file for write: $!\n";
STDOUT->autoflush(1);
LOG->autoflush(1);

my $starttime = time();
print LOG "="x10,"[ ", scalar localtime, " ]", "="x10, "\nProgram starts.\n";

my ($deleted, 
    $pmid, $muid, 
    $gia, $nama, $atype, 
    $gib, $namB, $btype, 
    $opno, $opyes, 
    $piscore, $piscore2, $pitruth); #POINT table fields
my $last = $table->last_record;
my $count=0;
my $step = 0;
my $percent = 0;

$step = $last/100;
print "number of total records = $last\n";
print "1% of total records = $step\n";
for (0 .. $last) {
	($deleted, 
    $pmid, $muid, 
    $gia, $nama, $atype, 
    $gib, $namB, $btype, 
    $opno, $opyes, 
    $piscore, $piscore2, $pitruth) 
    
    = $table->get_record($_, "PMID", "MUID", 
        "GIA", "NAMA", "ATYPE", "GIB", "NAMB", "BTYPE", 
        "OPNO", "OPYES", "PISCORE", "PISCORE2", "PITRUTH");
    
    next if $deleted;
    $count++;
    if($count>=$step){
        $count=0;
        $percent++;
        #print "$percent\n";
        print "\b\b$percent";
    }
    print OUT "$pmid\t$muid\t$gia\t$nama\t$atype\t$gib\t$namB\t$btype\t$opno\t$opyes\t$piscore\t$piscore2\t$pitruth\n";
}

print LOG  "\nProgram stops.\n", "="x10,"[ ", scalar localtime, " ]", "="x10;
close(LOG);
close(OUT);
$table->close;
