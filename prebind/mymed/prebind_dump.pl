#!/usr/bin/perl -w

use lib '/home/kzheng/mylib/myperlmodules/DBD-XBase-0.241/lib';
use XBase;

$infile=$ARGV[0];
$logfile = $ARGV[1];

$table = new XBase "$infile" or die XBase->errstr;
open LOG,"> $logfile" || die "Can't open log file for write: $!\n";
print LOG "="x10,"[ ", scalar localtime, " ]", "="x10, "\nProgram starts.\n";
$table->dump_records("fs" => "\t", "rs" => "\n");
print LOG "="x10,"[ ", scalar localtime, " ]", "="x10, "\nProgram stops.\n";
$table->close;
close(LOG);
