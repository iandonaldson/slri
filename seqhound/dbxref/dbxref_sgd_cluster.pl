# $Id: dbxref_sgd.pl, v
#
# Parse SGD Xref: dbxref.tab from
# From: ftp://genome-ftp.stanford.edu/pub/yeast/data_download/chromosomal_feature/
#
# two choises are available - to use DBD-mysql or to use DBI to connect to
# MySQL-db.
# DBI is a most universal one (DBD-mysql is too specific)
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
      displayMessage('dbxref_sgd_cluster.pl');
}
if ($cluster eq 'T')
{
   if ( (! $offset) and (! $count2parse) )
   {
      displayMessage('dbxref_sgd_cluster.pl');
   }
}

#die("die test\n");
# =============================================================

my $dbh;

my $dsn = "DBI:mysql:" . $database . ":" . $host . ":" . $port;
# 2) connect to MySQL-database
$dbh = DBI->connect ($dsn, $user_name, $password, {RaiseError => 1});

my %dbtable = dbxref->createDbHashTable( $dbh, $database );
my $dbId = $dbtable{"SGD"}->source_id();
#####################################################
# Delete all existing records before adding new ones.
#dbxref->executeDelete($dbh, $database, $table, $dbId);

#Initialize dbxref Statements.
dbxref->prepareID($dbh, $database, $table);
dbxref->prepareInsert($dbh, $database, $table);

# 1) process external_id Yeast mapper table. Cross Reference database.

my $file2parse = $data_dir . dbxref->getProperty( "dbxref.ini", "XREF_SGD" );

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

my $count    = 0;                             # Count the number of records
my $countSGD = 0;
my $countSP  = 0;
my $countGB  = 0;

while (my $line = <FP>)
{
#    print($line);

    my $last = "";
    chomp($line);
    $count++;
    if ("!" ne substr($line, 0, 1))
    {
        my @record = split(/\t/, $line);

	my $externalID  = $record[0];
	my $externalSrc = $record[1];
        my $externalTyp = $record[2];
	my $ORF         = $record[3];
	my $SGD         = $record[4];
	
#	print("\n");
#	print("externalID : " . $externalID . "\n");
#	print("externalSrc: " . $externalSrc . "\n");
#	print("ORF        : " . $ORF . "\n");
#	print("SGD        : " . $SGD . "\n");

        # NCBI: GB accessions.
	# SIB : Swissprot.
	my $KEYID = 0;
	if ( ($externalSrc =~ /(NCBI)/) or ($externalSrc =~ /(SIB)/) )
	{
	    my @sgdid = dbxref->selectID(6, $SGD, 0);
	    $KEYID = $sgdid[0];
	    if ( $KEYID < 1)
	    {   # Insert SGD Record.
		# 6 = SGD
		if(dbxref->executeInsert(6, $SGD, 0, 0, 'Col4', 0) == 1)
		{
		  $countSGD++;
		}
		if (my @arr = dbxref->selectID(6, $SGD, 0))
		{
		    $KEYID = $arr[0];
		}
	    }
	    else
	    {
	        $KEYID = $sgdid[0];
	    }
	    if ($KEYID < 1)
	    {
	        # Cluster sincronization problem. some times $KEYID can't be setup and get NULL.
	        # for this reason will skip this record.
	      next;
	    }
	    if ($externalTyp =~ /(Protein version ID)/)
	    {
#	        print("externalID: " .  $externalID ."               sgdid: " . $SGD . "\n");
		my $acc =  removeVersion($externalID);
		# 2 = GB
		if(dbxref->executeInsert(2, $acc, $KEYID, 0, "Protein version ID", 2)== 1)
		{
		   $countGB++;
		}
	    }
	    elsif ($externalTyp =~ /(RefSeq protein version ID)/)
	    {
#	        print("externalID: " .  $externalID ."               sgdid: " . $SGD . "\n");
		my $acc = removeVersion($externalID);
		# 2 = GB;
		if (dbxref->executeInsert(2, $acc, $KEYID, 0, 'RefSeq protein version ID', 2) == 1)
		{
		   $countGB++;
		}
	    }
	    elsif ($externalTyp =~ /(DNA version ID)/)
	    {
#	        print("externalID: " .  $externalID ."               sgdid: " . $SGD . "\n");
		my $acc = removeVersion($externalID);
		# 2 = GB
		if (dbxref->executeInsert(2, $acc, $KEYID, 0, "GenBank DNA Version", 1) == 1)
		{
		   $countGB++;
		}
	    }
	    elsif ($externalTyp =~ /(Swiss-Prot ID)/)
	    {
#	        print("externalID: " .  $externalID ."               sgdid: " . $SGD . "\n");
		# 1 = SP
	        if(dbxref->executeInsert(12, $externalID, $KEYID, 0, "Swiss-Prot ID", 100) == 1)
		{
		  $countSP++;
		}
	    }
	}
	
	$externalID = "";
	$externalTyp = "";
	$externalSrc = "";
	$ORF = "";
	$SGD = "";

#	print("\n");
	
    }
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
#    print("Enter \n");
#    print("abbrev: " . $abbrev . "\n");
#    print("databs: " . $databs . "\n");
#    print("object: " . $object . "\n");
#    print("exampl: " . $exampl . "\n");
#    print("genurl: " . $genurl . "\n");
#    print("urlsyn: " . $urlsyn . "\n");
#    print("urlexp: " . $urlexp . "\n");



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

$printResult = $printResult . "SGD_NODE:\t$node\n";
$printResult = $printResult . "SGD_FILE2PARSE:\t$file2parse\n";
$printResult = $printResult . "SGD_SGD:\t$countSGD\n";
$printResult = $printResult . "SGD_GB:\t$countGB\n";
$printResult = $printResult . "SGD_SP:\t$countSP\n";
$printResult = $printResult . "SGD_$node"."_TOTAL:\t" . ($countSGD+$countGB+$countSP) . "\n";
$printResult = $printResult . "SGD_$node"."_TIME:\t" . ($t2-$t1) ."\n";
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

