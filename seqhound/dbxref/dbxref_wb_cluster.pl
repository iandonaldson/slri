# $Id: dbxref_wb.pl
# Parser to create dbxref recors from wormpep.table
# ftp://ftp.sanger.ac.uk/pub/databases/wormpep/wormpep.table
# This parser was created because SPTR have only 86.60 % of Xref to wormpep.
#
# two choices are available - to use DBD-mysql or to use DBI to connect to
# MySQL-db.
# DBI is a most universal one (DBD-mysql is too specific)
use strict;
use DBI;
use Getopt::Long;
use dbxref;

my $user_name = dbxref->getProperty("dbxref.ini", "temp_user");
my $password  = dbxref->getProperty("dbxref.ini", "temp_password");
my $host      = dbxref->getProperty("dbxref.ini", "temp_host");
my $port      = dbxref->getProperty("dbxref.ini", "temp_port");
my $database  = dbxref->getProperty("dbxref.ini", "temp_database");
my $table     = dbxref->getProperty("dbxref.ini", "temp_table");

my $data_dir  =dbxref->getProperty("dbxref.ini", "data_dir");

my $file2parse = $data_dir . dbxref->getProperty( "dbxref.ini", "XREF_WB" );

my $dsn = "DBI:mysql:" . $database . ":" . $host . ":" . $port;

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
      displayMessage('dbxref_wb_cluster');
}
if ($cluster eq 'T')
{
   if ( (! $offset) and (! $count2parse) )
   {
      displayMessage('dbxref_wb_cluster');
   }
}
#print("fileName: $file2parse \n");
#print("cluster: $cluster \n");
#print("offset: $offset \n");
#print("count2parse: $count2parse \n");
#print("dummy: $dummy \n");

#die("die test\n");

# =============================================================

my $dbh;
my $sth;
my @ary;

# 2) connect to MySQL-database
$dbh = DBI->connect ($dsn, $user_name, $password, {RaiseError => 1});

#####################################################
# Delete all existing records before adding new ones.
my %dbtable = dbxref->createDbHashTable( $dbh, $database );
my $dbId = $dbtable{"WB"}->source_id();
#dbxref->executeDelete($dbh, $database, $table, $dbId);

# Create Statements.
dbxref->prepareID($dbh, $database, $table);
dbxref->prepareInsert($dbh, $database, $table);

open(FP, $file2parse) or die "can't open given file: $!";

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


# $cv was defined to diferenciate SPTR to be SP or TR.
# SPTR = source_id = 12 but with cv=100 it's SP
#                                cv=101 it's TR

my $count     = 0;                # Count the number of records
my $countWB   = 0;
my $countGB   = 0;
my $countCE   = 0;
my $countTR   = 0;
my $countSP   = 0;
my $countSPTR = 0;
my $countTN   = 0;

my $SPTR;
my $GB;
my $WB;
my $TN;
my $cv;

my @GB;
my @SPTR;
my @TN;
my @WB;

while (my $line = <FP>)
{
#    print($line);

    my $last = "";
    chomp($line);
    $count++;
    $SPTR  = "";
    $GB = "";
    $WB = "";
    $TN = "";
    $cv = 0;
    my @record = split(/\t/, $line);

#    print("rec0: " . $record[0] . "\n");
#    print("CE: "   . $record[1] . "\n");
#    print("rec2: " . $record[2] . "\n");
#    print("rec3: " . $record[3] . "\n");
#    print("rec4: " . $record[4] . "\n");
#    print("DB: "   . $record[5] . "\n");
#    print("GB: "   . $record[6] . "\n");
#    print("\n");

    $WB   = $record[1];
    $SPTR = $record[5];
    $GB   = $record[6];

    # Process SPTR
    if ("TR:" eq substr($SPTR, 0, length("TR:")))
    {
        $SPTR = substr($SPTR, length("TR:"), length($SPTR));
	$cv = 101;
    }
    elsif ("SW:" eq substr($SPTR, 0, length("SW:")))
    {
        $SPTR = substr($SPTR, length("SW:"), length($SPTR));
	$cv = 100;
    }
    elsif ("TN:" eq substr($SPTR, 0, length("TN:")))
    {
        $TN = substr($SPTR, length("TN:"), length($SPTR));
        $SPTR= "";
	$cv = 0;
    }

    # Process GenBank Accession.

    $GB = substr($GB, 0, index($GB, "."));
#   print("CE: " . $WB . "\n");
#   print("SPTR: " . $SPTR . "\n");
#   print("GB: " . $GB . "\n");
#   print("\n");

    if(dbxref->executeInsert(13, $WB, 0, 0, 'Col 2', 0) == 1)
    {
      $countWB++;
    }

    my $PKEY;
    if (my @row = dbxref->selectID(13, $WB, 0))
    {
#       print("key: " . $row[0] . "\n");
	$PKEY = $row[0];
    }

    if ($SPTR ne "")
    {
        if(dbxref->executeInsert(12, $SPTR, $PKEY, 0, 'Col 5', $cv) == 1)
	{
	    if ($cv == 100)
	    {   $countSP++;   }
	    elsif($cv == 101)
	    {   $countTR++;   }
	}
    }
    if ($TN ne "")
    {
        if ( $TN ne $GB)   # This is because most of the time $TN it's equal to $GB.
	{
	    if(dbxef->executeInsert(2, $TN, $PKEY, 0, 'Col 6', 2) == 1)
	    {
	       $countGB++;
	    }
	}
    }
    if ($GB ne "")
    {
        if(dbxref->executeInsert(2, $GB, $PKEY, 0, 'Col 6', 2) == 1)
	{
	  $countGB++;
	}
    }

    $SPTR  = "";
    $GB = "";
    $TN = "";
    $WB = "";

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
my $t2 = time();
# =============================================================
$printResult = $printResult . "WB_NODE:\t$node\n";
$printResult = $printResult . "WB_FILE2PARSE:\t$file2parse\n";
$printResult = $printResult . "WB_WB:\t$countWB\n";
$printResult = $printResult . "WB_GB:\t$countGB\n";
$printResult = $printResult . "WB_SP:\t$countSP\n";
$printResult = $printResult . "WB_TR:\t$countTR\n";
$printResult = $printResult . "WB_SPTR:\t$countSPTR\n";
$printResult = $printResult . "WB_TN:\t$countTN\n";
my $countTotal = $countWB + $countGB + $countSP + $countTR + $countSPTR + $countTN;
$printResult = $printResult . "WB_$node"."_TOTAL:\t$countTotal\n";
$printResult = $printResult . "WB_$node"."_TIME:\t" . ($t2-$t1) ."\n";
$printResult = $printResult . "Execution time:\t" . ($t2-$t1)/60 ." min.\n";
$printResult = $printResult . "\n================================================================================\n";
print("$printResult\n");

exit(0);



# $Log: dbxref_wb_cluster.pl,v $
# Revision 1.1  2005/03/29 19:49:56  rcavero
# dbxref_wb_cluster.pl replace dbxref_wb.pl
#
# Revision 1.4  2004/06/01 18:02:36  eburgess
# Delete records before adding new ones.
#

