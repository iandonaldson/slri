# two choices are available - to use DBD-mysql or to use DBI to connect to
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
      displayMessage('dbxref_AFCS_cluster.pl');
}
if ($cluster eq 'T')
{
   if ( (! $offset) and (! $count2parse) )
   {
      displayMessage('dbxref_AFCS_cluster.pl');
   }
}

#die("die test\n");
# =============================================================

my $dbh;

my $dsn = "DBI:mysql:" . $database . ":" . $host . ":" . $port;

# ) connect to MySQL-database
$dbh = DBI->connect ($dsn, $user_name, $password, {RaiseError => 1});

my %dbtable = dbxref->createDbHashTable( $dbh, $database );
my $dbId = $dbtable{"AFCS"}->source_id();

my $AFCS=$dbtable{AFCS}->source_id();
my $GB = $dbtable{GB}->source_id();

#Initialize dbxref Statements.
dbxref->prepareID($dbh, $database, $table);
dbxref->prepareInsert($dbh, $database, $table);


my $file2parse= $data_dir . dbxref->getProperty("dbxref.ini", "XREF_AFCS");

open(FP, $file2parse) or die "can't open $file2parse: $!";

# =================  CLUSTERING ================================
my $recCount = 0;
my $l;
my $record;
if ($cluster eq 'T')
{
   if($offset > 0)
   {
      while ($l = <FP>)
      {
	 $record = $l;
	 if( $record =~ /^\/\//)
	 {
	   $recCount++;
	 }
	 if ($recCount == $offset)
	 {
	    last;
	 }
      }
   }
}
# =============================================================



print("AFCS: ". $AFCS . "\n\n");
# To do: remove Records for Update.
#dbxref->executeDeleteCV($dbh, $database, $table, $SPTR, $cv);

my $count    = 0;                             # Count the number of records
my $countAFCS = 0;
my $countGB = 0;
my $countNCBIGI = 0;
my $countNotFound = 0;

while (my $line = <FP>)
{
#    print($line);
    my $last = "";
    chomp($line);

    my $afcsid = "";
    my $accession = "";
    my $ncbigi = "";

#    print($line);

    if($line =~ /(AFCSID)/)
    {
         $line =~ /(AFCSID)(\s+)(\S+)/;
	 $afcsid = $3;
#	 print("\nafcsid: $afcsid\n");
    }
    while ($line = <FP>)
    {
        if ("//" eq substr($line, 0, 2))
        {
	   # print("//\n");
	    $count++;
	    last;
	}

	chomp($line);

	if($line =~ /(AFCSID)/)
	{
  	     $line =~ /(AFCSID)(\s+)(\S+)/;
	     $afcsid = $3;
	}
	elsif($line =~ /(ACCESSION)/ )
	{
	     $line =~ /(ACCESSION)(\s+)(\S+)(.)(\d+)/;
	     $accession = $3;
	}
	elsif($line =~ /(NCBI_GI)/ )
	{
	     $line  =~ /(NCBI_GI)(\s+)(\d+)/;
	     $ncbigi = $3;
	}
    }

    my $KEYID = 0;

    my @afcsArr = dbxref->selectID( $AFCS, $afcsid, 0);
    $KEYID = $afcsArr[0];
    if ( $KEYID < 1)
    {
        if (dbxref->executeInsert($AFCS, $afcsid, 0, 0, 'AFCSID', 0) == 1)
	{
	    $countAFCS++;
	}
	if (my @arr = dbxref->selectID($AFCS, $afcsid, 0))
	{
	    $KEYID = $arr[0];
	}
    }

    if($KEYID < 1)
    {
        # Cluster sincronization problem. some times $KEYID can't be setup and get NULL.
        # for this reason will skip this record.
        next;
    }

    # 2 = GB
    if($accession ne "")
    {
	    if( dbxref->executeInsert($GB, $accession, $KEYID, 0, 'ACCESSION', 2))
	    {
	        $countGB++;
	    }
    }
    else
    {
	    $countNotFound++;
    }

    # print("//\n");
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

$printResult = $printResult . "AFCS_NODE:\t$node\n";
$printResult = $printResult . "AFCS_FILE2PARSE:\t$file2parse\n";
$printResult = $printResult . "AFCS_AFCS:\t$countAFCS\n";
$printResult = $printResult . "AFCS_GB:\t$countGB\n";
$printResult = $printResult . "AFCS_GBNOTFOUND:\t$countNotFound\n";
$printResult = $printResult . "AFCS_$node"."_TOTAL:\t" . ($countAFCS+$countGB) . "\n";
$printResult = $printResult . "AFCS_$node"."_TIME:\t" . ($t2-$t1) ."\n";
$printResult = $printResult . "Execution time:\t" . ($t2-$t1)/60 ." min.\n";
$printResult = $printResult . "\n================================================================================\n";

print("$printResult\n");

exit(0);

