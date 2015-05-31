#!/usr/bin/perl -w
#see http://search.cpan.org/~janpaz/DBD-XBase-0.241/lib/XBase.pm

# =============================================================================
# Command syntax: 
#       perl prebind_dump.pl  infile outfile
# Command Example:
#     perl prebind_dump.pl  SRCH.dbf Srch.out
# ==============================================================================

use lib '/home/kzheng/mylib/myperlmodules/DBD-XBase-0.241/lib';
use XBase;

$infile=$ARGV[0];
$outfile = $ARGV[1];

$table = new XBase "$infile" or die XBase->errstr;
open OUT,"> $outfile" || die "Can't open out file for write: $!\n";
print OUT "="x10,"[ ", scalar localtime, " ]", "="x10, "\nProgram starts.\n";

$table->dump_records("fs" => "\t", "rs" => "\n");

print OUT "="x10,"[ ", scalar localtime, " ]", "="x10, "\nProgram stops.\n";
$table->close;
close(OUT);
