#!/usr/bin/perl

use lib '/home/rcavero/myperlmodules';
use DBI;

my $updatelog ="updateresult.log";
open TOUCH, ">$updatelog"; close TOUCH;

# for MySQL
my $host = "10.0.0.127";
my $port = "3306";
my $database = "seqhound";
my $user_name = "zwang";
my $password = "asdf1234";
my $dsn = "DBI:mysql:" . $database . ":" . $host . ":" . $port;
my $dbh;
my $sth;

# connec to MySQL
$dbh = DBI->connect ($dsn, $user_name, $password, {RaiseError => 0});

my $core = "accdb";
my $sp = "sp";
my $redund = "redund";
my $taxdb = "taxdb";
my $lldb = "ll_llink";
my $godb = "go_name";
my $comgen = "chrom";
my $llgo = "ll_go";
my $strucdb = "mmdb";
my $neighbours = "nblastdb";
my $rps = "rpsdb";

my @tablelist = ($core, $sp, $redund, $taxdb, $strucdb, $lldb, $llgo, $godb, $comgen, $neighbours, $rps);
my @modulelist = ("CORE", "Swiss-Prot", "REDUND", "TAXDB", "STRUCDB", "LLDB", "LLDB", "GODB", "COMGEN", "NEIGHBOURS", "RPS");

my @desc = ("Sequences", "Swiss-Prot Data", "Redundant Groups", "NCBI Tax Tree", "MMDB Structures","Locus Link Data Sets", "Go Annotations", "Gene Ontology Tree", "Complete Genomes", "Protein Sequence Neighbours", "RPS Blast Results");


my $ncbiftp = "ftp://ftp.ncbi.nlm.nih.gov/";
my $spftp = "ftp://au.expasy.org/";
my $geneontoftp = "ftp://ftp.geneontology.org/pub/go/ontology/\">";
my $coresrc = $ncbiftp."ncbi-asn1/daily-nc/\">";
my $spsrc = $spftp."databases/swiss-prot/release/\">";
my $refseqsrc = $ncbiftp. "refseq/daily/\">";
my $redundsrc = $ncbiftp."blast/db/\">";
my $taxsrc = $ncbiftp."pub/taxonomy/\">";
my $mmdbsrc = $ncbiftp."mmdb/\">";
my $lltaxdumpsrc = $ncbiftp."refseq/LocusLink/\">";
my $comgensrc = $ncbiftp."genomes/\">";
my $rpssrc = $ncbiftp."/pub/mmdb/cdd/little_endian/\">";

my $hrefhead = "<A HREF=\"";
my $hreftail = "</A>";

my $corelink = $hrefhead.$coresrc."GenBank Update".$hreftail."<BR>".$hrefhead.$refseqsrc."RefSeq Update".$hreftail;
my $splink = $hrefhead.$spsrc."SwissProt Update".$hreftail;
my $redundlink = $hrefhead.$redundsrc."nr.tar.gz".$hreftail;
my $taxlink = $hrefhead.$taxsrc."taxdump.tar.gz".$hreftail;
my $mmdblink = $hrefhead.$mmdbsrc."NCBI mmdbdata".$hreftail;
my $lllink = $hrefhead.$lltaxdumpsrc."LL_tmpl.gz".$hreftail;
my $comgenlink = $hrefhead.$comgensrc."NCBI Genomes".$hreftail;
my $rpslink = $hrefhead.$rpssrc."ALL_LE.tar.gz".$hreftail;
my $neighbourlink = $hrefhead.$redundsrc."nr.tar.gz".$hreftail;
my $geneontolink = $hrefhead.$geneontoftp."GeneOntology Release".$hreftail;

my @linklist = ($corelink, $splink, $redundlink, $taxlink, $mmdblink,
$lllink, "Compiled Internally*", $geneontolink, $comgenlink, $neighbourlink, $rpslink);

my @exa = ("ACCDB", "ACCDB", "REDUND", "TAX", "MMDB", "LL_LLINK", "LL_GO", "GO_NAME", "CHROM", "N\/A", "MASTER");

my @schedule = ("Nightly update of GenBank and RefSeq", "Manually check for Swiss-Prot new release", "Nightly check for new NR release", "Nightly check and rebuild", "Nightly check for new release", "Nightly check and rebuild", "Manual update", "Nightly check and rebuild", "Weekly", "Nightly check for new NR release, rebuild takes 24 hours", "Nightly check and rebuild");

my $count = 0;

open (UPDLOG, ">$updatelog");
foreach $table (@tablelist)
{
  my $stamp = "";
  if ($table eq $sp) {
    $stamp = "2004-08-03 16:17:45";
  }  
  else {
    $sth = $dbh->prepare ("SHOW TABLE STATUS IN seqhound LIKE '$table'");
    $sth->execute();
    my @ct_MS;
    while (@ct_MS =$sth->fetchrow()) {
  	$stamp = (@ct_MS[11]);
    }
    $sth->finish();
  }

  print UPDLOG "$modulelist[$count] \t $desc[$count] \t $exa[$count] \t $linklist[$count] \t $stamp \t $schedule[$count]\n";
  $count++;
}
close (UPDLOG);

$dbh->disconnect();
