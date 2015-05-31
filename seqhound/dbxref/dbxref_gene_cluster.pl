# Parse Gene Xref: gene2accession
# From: ftp://ftp.ncbi.nlm.nih.gov/gene/DATA/gene2accession.gz
#

use strict;
use DBI;
use Getopt::Long;
use dbxref;

# Read properties file dbxref.ini.
my $user_name = dbxref->getProperty("dbxref.ini", "temp_user");
my $password  = dbxref->getProperty("dbxref.ini", "temp_password");
my $host      = dbxref->getProperty("dbxref.ini", "temp_host");
my $port      = dbxref->getProperty("dbxref.ini", "temp_port");
my $database  = dbxref->getProperty("dbxref.ini", "temp_database");
my $table     = dbxref->getProperty("dbxref.ini", "temp_table");


my $user_nameshound = dbxref->getProperty("dbxref.ini", "usershound");
my $passwordshound  = dbxref->getProperty("dbxref.ini", "passwordshound");
my $hostshound      = dbxref->getProperty("dbxref.ini", "hostshound");
my $portshound      = dbxref->getProperty("dbxref.ini", "portshound");
my $databaseshound  = dbxref->getProperty("dbxref.ini", "databaseshound");
my $tableshound     = dbxref->getProperty("dbxref.ini", "tableshound");


my $data_dir = dbxref->getProperty("dbxref.ini", "data_dir");

my $t1 = time();

# =================  CLUSTERING ================================
my $cluster;
my $offset;
my $count2parse;
my $dummy = '';

my $retValue = 0;
$retValue = GetOptions('c=s' => \$cluster, 'o=i' => \$offset,'n=i' => \$count2parse, 'f=s' => \$dummy );

if( ($cluster ne 'T') and ($cluster ne 'F') )
{
      displayMessage('dbxref_gene_cluster.pl');
}
if ($cluster eq 'T')
{
   if ( (! $offset) and (! $count2parse) )
   {
      displayMessage('dbxref_gene_cluster.pl');
   }
}

#die("die test\n");
# =============================================================

my $dsn = "DBI:mysql:" . $database . ":" . $host . ":" . $port;
my $dbh = DBI->connect ($dsn, $user_name, $password, {RaiseError => 1});

my $dsnsh = "DBI:mysql:" . $databaseshound . ":" . $hostshound . ":" . $portshound;
my $dbhsh = DBI->connect ($dsnsh, $user_nameshound, $passwordshound, {RaiseError => 1});

#####################################################
# Delete all existing records before adding new ones.
my %dbtable = dbxref->createDbHashTable( $dbh, $database );
my $dbId = $dbtable{"GENE"}->source_id();
#dbxref->executeDelete($dbh, $database, $table, $dbId);

#Initialize dbxref Statements.
dbxref->prepareID($dbh, $database, $table);
dbxref->prepareInsert($dbh, $database, $table);

my $shSthIsInSHound;
prepareIsInSHound($dbhsh, $databaseshound, "accdb");

my $file2parse = $data_dir . dbxref->getProperty( "dbxref.ini", "XREF_GENE" );

open(FP, $file2parse) or die "can't open $file2parse: $!";

# =================  CLUSTERING ================================
my $recCount = 0;
my $l;
if ($cluster eq 'T')
{
   if($offset > 0)
   {
      while ($l = <FP>)
      {
	 $recCount++;
	 if ($recCount == $offset)
	 {
	    last;
	 }
      }
   }
}
# =============================================================

my $count        = 0;                             # Count the number of records
my $countGENEID  = 0;
my $countSP      = 0;
my $countGB      = 0;
my $countGB_DNA  = 0;
my $countGB_RNA  = 0;
my $countGB_PROT = 0;


while (my $line = <FP>)
{
#    print($line);

    my $last = "";
    chomp($line);
    $count++;
    if ("!" ne substr($line, 0, 1))
    {
        my @record = split(/\t/, $line);

	my $taxId   = $record[0];
	my $geneId  = $record[1];
        my $status  = $record[2];
	my $rnaAcc  = $record[3];
	my $rnaGI   = $record[4];
	my $protAcc = $record[5];
	my $protGI  = $record[6];
	my $dnaAcc  = $record[7];
	my $dnaGI   = $record[8];
	
#	print("\n");
#	print("taxId : "   . $taxId . "\n");
#	print("geneId : "  . $geneId . "\n");
#	print("status : "  . $status . "\n");
#	print("rnaAcc : "  . $rnaAcc . "\n");
#	print("rnaGI : "   . $rnaGI . "\n");
#	print("protAcc: " . $protAcc . "\n");
#	print("protGI : "  . $protGI . "\n");
#	print("dnaAcc : "  . $dnaAcc . "\n");
#	print("dnaGI : "   . $dnaGI . "\n\n");

	my $KEYID = 0;

	my @geneIndex = dbxref->selectID(32, $geneId, 0);
	$KEYID = $geneIndex[0];

	if ( length($KEYID) == 0)
	{   # Insert GENE Record.
		# 32 = GENE
	    if(dbxref->executeInsert(32, $geneId, 0, 0, 'Col2', 0) == 1)
	    {
	        $countGENEID++;
	    }
	    @geneIndex = dbxref->selectID(32, $geneId, 0);
	    $KEYID = $geneIndex[0];
	 }
	# TODO Add Link.
	my $link = 0;
	if(length($rnaAcc) > 1)
	{
	    $rnaAcc = removeVersion($rnaAcc);
	    # 2 = GB
	    if(dbxref->executeInsert(2, $rnaAcc, $KEYID, 0, "Col4", 3) == 1)
	    {
	        $countGB_RNA++;
		$link = dbxref->selectID(2, $rnaAcc, $KEYID);
	    }
	}
	if(length($protAcc) > 1)   # Be carefoul this can be a SP ID.
	{
	    my $cv = 0;
	    my $sourceId = 0;
	    if(length($protAcc) == 6)
            {
	       if ($protAcc =~ /[OPQ]\d\w\w\w\d/)
	       {
		  $sourceId = 12; # SP
		  $cv = 100;
	       }
	    }
	    else
	    {
	       $sourceId = 2;  #GB
	       $cv = 2;
	       $protAcc = removeVersion($protAcc);
	    }

	    if(dbxref->executeInsert($sourceId, $protAcc, $KEYID, $link, "Col6", $cv) == 1)
	    {
		    if( $sourceId == 2)
		    {    $countGB_PROT++;   }
		    elsif( $sourceId == 12 )
		    {    $countSP++;   }
	    }

	}
	# DNA accession are not been inserted because usualy it's from a complete genome and will
        # not make sense in DBXREF.
	#if(length($dnaAcc) > 1)   # Be carefoul this can be a SP ID.
        #{
        #    $dnaAcc = removeVersion($dnaAcc);
        #   # 2 = GB
        #   if(dbxref->executeInsert(2, $dnaAcc, $KEYID, 0, "Col8", 2) == 1)
        #   {
        #        $countGB_DNA++;
	#   }
	#}


      }
	
#	print("\n");

    # =================  CLUSTERING ================================
    if ($cluster eq 'T')
    {
	  if ($count == $count2parse)
	  {
	     last;
	  }
    }
    # ===============================================================
}

close(FP);

dbxref->finishSelectID();
dbxref->finishInsert();
$dbh->disconnect();

# =================  CLUSTERING ================================
# Generate a dummy file for sincronization.
if ($cluster eq 'T')
{
   if ($dummy)
   {
      open(FPOUT, ">$dummy");
      print(FPOUT "dummy file ".$dummy);
      close(FPOUT);
   }
 }

# =============================================================

my $t2 = time();

my $printResult;

my $node;
# =================  CLUSTERING ================================
if ($cluster eq 'T')
{
   if ($dummy)
   {
      $dummy =~ /(\w+)(\.)/;
      $node = $1;
   }
}
else
{
   $node = "STDALONE";
}
# =============================================================

$printResult = $printResult . "GENE_NODE:\t$node\n";
$printResult = $printResult . "GENE_FILE2PARSE:\t$file2parse\n";
$printResult = $printResult . "GENE_GENE:\t$countGENEID\n";
$printResult = $printResult . "GENE_GBDNA:\t$countGB_DNA\n";
$printResult = $printResult . "GENE_GBRNA:\t$countGB_RNA\n";
$printResult = $printResult . "GENE_GBPROT:\t$countGB_PROT\n";
$countGB = $countGB_DNA+$countGB_RNA+$countGB_PROT;
$printResult = $printResult . "GENE_GB:\t$countGB\n";
$printResult = $printResult . "GENE_SP:\t$countSP\n";
$printResult = $printResult . "GENE_$node"."_TOTAL:\t" . ($countGENEID+$countGB+$countSP) . "\n";
$printResult = $printResult . "GENE_$node"."_TIME:\t" . ($t2-$t1) ."\n";
$printResult = $printResult . "Execution time:\t" . ($t2-$t1)/60 ." min.\n";
$printResult = $printResult . "\n================================================================================\n";

print("$printResult\n");



exit(0);



sub removeVersion($)
{
    if ( index($_[0], ".", 0) != -1)
    {
        return (substr($_[0], 0, index($_[0], ".", 0)));
    }
    else
    {
        return($_[0]);
    }
}

sub prepareIsInSHound
{
    $shSthIsInSHound = $_[0]->prepare("SELECT access FROM ". $_[1] . ".". $_[2] . " WHERE gi=?");
}


sub isGiInSHound
{
    if($_[0] > 0)
    {
        $shSthIsInSHound->execute($_[0]);
	if(my @rowAcc = $shSthIsInSHound->fetchrow_array() )
	  {
	      return "T";
	  }
    }
    return "F";
}

