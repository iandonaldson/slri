#!/usr/bin/perl
#
# CLUSTER version of dbxref_ll_extra.pl
#
#--------------------------------------------------------------------------
# parser for Xref from OMIM & HUGO ==> LocusLink
# DB are : 1 OMIM, 2 HUGO
# 
# the files come from: ftp://ftp.ncbi.nih.gov/gene/DATA/mim2gene
#                      ftp://ftp.ncbi.nih.gov/gene/DATA/gene2unigene
#
# The input file format(tab-delimited), but
# For mim2gene:      Column1= OMIM | Column2=LL
# For gene2unigene:  Column1= LL   | Column2=HUGO
#--------------------------------------------------------------------------
use strict;
use DBI;
use Getopt::Long;
use dbxref;



# mysql set up stuff
my $host      = dbxref->getProperty("dbxref.ini", "temp_host");
my $port      = dbxref->getProperty("dbxref.ini", "temp_port");
my $database  = dbxref->getProperty("dbxref.ini", "temp_database");
my $table     = dbxref->getProperty("dbxref.ini", "temp_table");
my $user_name = dbxref->getProperty("dbxref.ini", "temp_user");
my $password  = dbxref->getProperty("dbxref.ini", "temp_password");

my $data_dir  = dbxref->getProperty("dbxref.ini", "data_dir");
my $dsn = "DBI:mysql:$database:$host:$port";

my $t1 = time();

# =================  CLUSTERING ================================
my $cluster;
my $db;
my $offset;
my $count2parse;
my $dummy = '';

my $retValue = 0;
$retValue = GetOptions('c=s' => \$cluster, 'd=s' => \$db, 'o=i' => \$offset,'n=i' => \$count2parse, 'f=s' => \$dummy );


if ($retValue != 1)
{
   displayMessage();
}

if( ($cluster ne 'T') and ($cluster ne 'F') )
{
      displayMessage();
}
if (! $db)
{
      displayMessage();
}
if( ($db ne 'OMIM') and ($db ne 'UNIGENE') )
{
      displayMessage();
}
if ($cluster eq 'T')
{
   if ( (! $offset) and (! $count2parse) )
   {
      displayMessage();
   }
}

#die("die test\n");
#===============================================================

###############################################################
#  connect to MySQL-database
my $dbh = DBI->connect ($dsn, $user_name, $password, {RaiseError => 1});

###############################################################
# Prepare the statement handles.
# use functions in dbxref.pm to prepare for the update
dbxref->prepareID($dbh, $database, $table);
dbxref->prepareInsert($dbh, $database, $table);


###############################################################
#  get the filenames.
my $mim2geneFile =  dbxref->getProperty( "dbxref.ini", "XREF_GENE_OMIM" );
my $gene2UGFile  =  dbxref->getProperty( "dbxref.ini", "XREF_GENE_UNIGENE" );
my $file2parse;

###############################################################
# get source db hash table
my %dbtable = dbxref->createDbHashTable($dbh, $database);
my $GENE = $dbtable{GENE}->source_id();
my $UNIGENE = $dbtable{UNIGENE}->source_id();
my $OMIM = $dbtable{OMIM}->source_id();
my $sourceID = 0;

# parse dbxref file for individual organism, one at a time
if($db eq "OMIM")
{
    $file2parse = $data_dir . $mim2geneFile;
    $sourceID = $OMIM;
}
elsif($db eq "UNIGENE")
{
    $file2parse = $data_dir . $gene2UGFile;
    $sourceID = $UNIGENE;
}
else
{
    die("Please enter the right command line argument: OMIM or UNIGENE.\n");
}

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

###############################################################
# Set up conters.
my $count = 0;
my $count_GENE      = 0;

my $count_UNIGENE = 0;
my $count_OMIM    = 0;
my $countNotIns   = 0;
while(my $line = <FP>)
{
    my $GENEid = ();
    my $xref = ();
    chomp($line);
    $count++;
    my @xrefs =split(/\t/, $line);

    if ($db eq "OMIM")
    {
        $xref = "MIM:" . $xrefs[0];
	$GENEid = $xrefs[1];
	#$countOMIM++;
    }
    elsif($db eq "UNIGENE")
    {
        $GENEid = $xrefs[0];
	$xref = $xrefs[1];
	#$countUNIGENE++;
    }

    @xrefs=();

#    print("GENE: $GENEid \t Xref: $xref\t");
    my @rowID = dbxref->selectID($GENE, $GENEid, 0);
#    print("rowid: $rowID[0]\n");
    my $id = $rowID[0];
    if( $rowID[0] <=0 )
    {
        # IF GENE id does not exist insert it and get it's id.
#        printf("Insert GENE ID because was not found\n");
	if(dbxref->executeInsert($GENE, $GENEid, 0, 0, "Col 1", 0) == 1)
	{   $count_GENE++;   }
	else
	{   $countNotIns++   }

	@rowID = ();
	@rowID = dbxref->selectID($GENE, $GENEid, 0);
	$id = $rowID[0];
    }
    if(dbxref->executeInsert($sourceID, $xref, $id, 0, "Col 2", 0) == 1)
    {
      if($db eq "OMIM")
      {   $count_OMIM++;   }
      elsif($db eq "UNIGENE")
      {   $count_UNIGENE++;   }
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

# ==============================================================

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


$printResult = $printResult . "GENE_NODE:\t$node\n";
$printResult = $printResult . "GENE_FILE2PARSE:\t$file2parse\n";
$printResult = $printResult . "GENE_GENE:\t$count_GENE\n";
$printResult = $printResult . "GENE_NotIns:\t$countNotIns\n";
$printResult = $printResult . "GENE_UNIGENE:\t$count_UNIGENE\n";
$printResult = $printResult . "GENE_OMIM:\t$count_OMIM\n";
my $totalcount = $count_GENE + $count_UNIGENE + $count_OMIM;
$printResult = $printResult . "GENE_TOTAL:\t$totalcount\n";
$printResult = $printResult . "GENE_$node"."_TIME:\t" . ($t2-$t1) ."\n";
$printResult = $printResult . "Execution time : " . ($t2-$t1)/60 . " min.\n";
$printResult = $printResult . "\n================================================================================\n";
print("$printResult\n");


exit(0);



sub displayMessage
{
      print("Usage : perl dbxref_GENE_extra -c -d -o -n -f\n");
      print("        or\n");
      print("        perl dbxref_GENE_extra -c N -d\n");
      print("\tWhere:\n");
      print("\t\t-c Cluster: T = True | F = False\n");
      print("\t\t-d Database: OMIM | UNIGENE\n");
      print("\t\t-o Offset: number of record to skip\n");
      print("\t\t-n Number: of recored to process\n");
      print("\t\t-f Cluster node that will became a Synchronization file \n\n");
      print("\tIf -c = F arguments -o -n -f are optional.\n");
      die("4 input arguments are needed: DataBase Offset Quantity SyncFileName\n");
}
