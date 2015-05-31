#!/usr/bin/perl -w

# =============================================================================
# Command syntax: 
#       perl prebind_diff.pl  SRCH.dbf RESULT-pubmed.dbf RESULT-mymed.dbf
# ==============================================================================

use strict;
use lib '/home/kzheng/mylib/myperlmodules/DBD-XBase-0.241/lib';
use XBase;

my $outfile = "RESULT-diff.txt";
my $diffdb = "RESULT_diff.csv";
my $infile0=$ARGV[0];
my $infile1=$ARGV[1];
my $infile2=$ARGV[2];
my $seperator= "-"x60;
open OUT ,">$outfile"||die "can't open diff file for write; $!. \n";
open DIFF ,">$diffdb"||die "can't open diff db for write; $!. \n";
print OUT "==========[ ", scalar localtime, " ]=================\n\n";

my $table0 = new XBase "$infile0" || die XBase->errstr;
my $table1 = new XBase "$infile1" || die XBase->errstr;
my $table2 = new XBase "$infile2" || die XBase->errstr;
my ($deleted0, $gi0, $name0, $taxid0, $last0);
my ($deleted1, $gi1, $name1, $pmid1, $recno1, $last1);
my ($deleted2, $gi2, $name2, $pmid2, $recno2, $last2);
my $taxid = 9606;

$last0 = $table0->last_record;
$last1 = $table1->last_record; $recno1 = 0;
$last2 = $table2->last_record; $recno2 = 0;
print OUT "$ARGV[0] [$last0]\t $ARGV[1] [$last1]\t $ARGV[2] [$last2]\n";
print OUT "gi\tname\tn-PMID-1\tnmiss\tnextra\tn-PMID-2\n";

for (0 .. $last0){
	($deleted0, $gi0, $name0, $taxid0) = $table0->get_record($_, "GI_PROT", "NAME", "TAXID");
	last unless $recno1 < $last1 && $recno2 < $last2;
    next if $deleted0;
    next if $taxid0 != 4932 && $taxid0 != 9606 && $taxid0 !=10090;
    
    my @pmids1;
    my @pmids2;
    while($recno1 <= $last1){
    	($deleted1, $gi1, $name1, $pmid1) = $table1->get_record($recno1);
    	last unless !$deleted1 && $gi1== $gi0 && $name1 eq $name0;
        push @pmids1, $pmid1;
        $recno1++;
    }
    while($recno2 <= $last2){
    	($deleted2, $gi2, $name2, $pmid2) = $table2->get_record($recno2);
    	last unless !$deleted2 && $gi2== $gi0 && $name2 eq $name0;
        push @pmids2, $pmid2;
        $recno2++;
    }
    
    next if $taxid0 != $taxid;
    
    my $npubmed= $#pmids1+1;
    my $nmymed = $#pmids2+1;
    my $nmiss =0;
    my $nextra =0;
    my $strmiss = "";
    my $strextra = "";
    
    my $strpubmed = join ' ', @pmids1; 
    $strpubmed = " ".$strpubmed ." ";
        
    my $strmymed = join ' ', @pmids2;
    $strmymed = " ".$strmymed ." ";
    
    
    my $tmppmid;
    foreach $tmppmid(@pmids2){
    	if($strpubmed!~/ $tmppmid /){
        	$nextra++;
        	$strextra.=" $tmppmid";
        }
    }
    foreach $tmppmid(@pmids1){
    	if($strmymed!~/ $tmppmid /){
        	$nmiss++;
        	$strmiss.=" $tmppmid";
        }
    }
    print OUT "\n$seperator\n";
    print OUT "$gi0\t$name0\t$npubmed\t$nmiss\t$nextra\t$nmymed\n";
    print DIFF "$gi0\t$name0\t$npubmed\t$nmiss\t$nextra\t$nmymed\n";
    #print OUT "pubmed[$npubmed]: $strpubmed\n";
    #print OUT "mymed[$nmymed]: $strmymed\n"; 
    print OUT "missing pmids : $strmiss\n";
    print OUT "extra pmids: $strextra\n"

}

close(OUT);
close(DIFF);
$table0->close;
$table1->close;
$table2->close;
