# parse gene2go file from gene
# ftp://ftp.ncbi.nih.gov/gene/DATA/gene2go.gz
#
# two choices are available - to use DBD-mysql or to use DBI to connect to
# MySQL-db.
# DBI is a most universal one (DBD-mysql is too specific)
use strict;
use DBI;
use Getopt::Long;
use dbxref;


# =================  CLUSTERING ================================
my $cluster;
my $db;
my $offset;
my $count2parse;
my $dummy = '';

my $retValue = 0;
$retValue = GetOptions('c=s' => \$cluster, 'd=s' => \$db, 'o=i' => \$offset,'n=i' => \$count2parse, 'f=s' => \$dummy );

if( ($cluster ne 'T') and ($cluster ne 'F') )
{
      dbxref->displayMessage('go_geneparser_cluster.pl');
}
if ($cluster eq 'T')
{
   if ( (! $offset) and (! $count2parse) )
   {
      dbxref->displayMessage('go_geneparser_cluster');
   }
}
if (length($db) <= 0)
{
    dbxref->displayMessage('go_geneparser_cluster');
}

# =============================================================


my $user_name = dbxref->getProperty("dbxref.ini", "temp_user");
my $password  = dbxref->getProperty("dbxref.ini", "temp_password");
my $host      = dbxref->getProperty("dbxref.ini", "temp_host");
my $port      = dbxref->getProperty("dbxref.ini", "temp_port");
my $database  = dbxref->getProperty("dbxref.ini", "temp_database");

my $data_dir  = dbxref->getProperty("dbxref.ini", "data_dir");

my $dbname_goa_seq_dbxref = dbxref->getProperty("dbxref.ini", "temp_goa_seq_dbxref");
my $dbname_goa_association= dbxref->getProperty("dbxref.ini", "temp_goa_association");
my $dbname_goa_reference  = dbxref->getProperty("dbxref.ini", "temp_goa_reference");
my $dbname_goa_with       = dbxref->getProperty("dbxref.ini", "temp_goa_with");

my $file2parse = $data_dir . dbxref->getProperty("dbxref.ini", $db);

my $dsn = "DBI:mysql:" . $database . ":" . $host . ":" . $port;

my $dbh;
my $sth;
my $sthSeqDbxref;
my $sthAssociation;
my $sthReference;
my $sthWith;

my @ary;

my $t1 = time();

# 2) connect to MySQL-database
$dbh = DBI->connect ($dsn, $user_name, $password, {RaiseError => 1});

#Reset table
#########################
# resetTables();
# die("End. Table Reset.\n");


# Create a HasTable for the db abbreviations and Ids.
# %abbr_id HasTable of Key: Abbreviation, Value: Ids.
# This hash table was created to avoid searching into goa_xdb each time
# an ID is needed from a particular abbreviation name.
##########################

my %hash_abbr = ();

createAbbrHashTable();

my $geneAbbrId = $hash_abbr{"GENE"};
#print ("PDB: " .  $pdbAbbrId . "\n");

#
# Strip out prefix GOA_ from argument, to get the abbreviation name
# of the file to begin processing.

my $abbr = $db;

my $sthGoaSeqDbxrefID;
prepareSelectGoaSeqDbxrefID($dbh, $database);

my $sthInsGoaSeqDbx;
prepareInsertGoaSeqDbx($dbh, $database);

my $sthInsGoaAss;
prepareInsertGoaAssociation($dbh, $database);


if(open(FP, $file2parse))
{
  # TODO: generate Dummy file.
  print("File OK\n");
}
else
{
    print("I'm dieing\n");
    die "can't open given file: $!";
}my $gMessage;                              # Global message to return Errors.
my $errorCount = 0;
my $count = 0;                             # Count the number of records

my $countSEQDBX;
my $countASS;
my $countREF;
my $countWITH;

#print "Starting to read file\n";

# =================  CLUSTERING ================================
   my $recCount = 0;
   my $l;
   if ($cluster eq 'T')
   {
      if ($offset > 0)
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

while (my $line = <FP>)
{
     chomp($line);
     if("#" ne substr($line, 0, 1))
     {
         $count++;
	 my @record = split(/\t/, $line);
	 my $tax_id= $record[0];     #    print("tax_id: " . $record[0] . "\n");
	 my $gene  = $record[1];     #    print("gene: "   . $record[0] . "\n");
	 my $go    = $record[2];     #    print("go:    "  . $record[1] . "\n");
	 my $ecode = $record[3];     #    print("ecode: "  . $record[2] . "\n");

	 $go =~ /(GO:)(\d+)/;
	 my $go_id = $2;
	 #print("gene: $gene\n");
	 $sthGoaSeqDbxrefID->execute($geneAbbrId , $gene);
	 my @goaSeqDbxrefID = $sthGoaSeqDbxrefID->fetchrow_array();
	 my $seqDbxrefID = $goaSeqDbxrefID[0];

	 if ($seqDbxrefID < 1)
	 {
	     #print("seqDbxrefID: $seqDbxrefID \n");
	     if($sthInsGoaSeqDbx->execute($geneAbbrId, $gene, $tax_id, 0, "", "", "", "") == 1)
	     {
	         $countSEQDBX++;
		 $sthGoaSeqDbxrefID->execute($geneAbbrId , $gene);
		 if(@goaSeqDbxrefID = $sthGoaSeqDbxrefID->fetchrow_array())
		 {
		     $seqDbxrefID = $goaSeqDbxrefID[0];
		     #print("seqDbxrefID: $seqDbxrefID\n");
		 }
		 else
		 {
		     next;
		 }
	     }
	  }
	 if($sthInsGoaAss->execute("F", $go_id, $seqDbxrefID, $geneAbbrId, $ecode, "") == 1)
	 {
	     $countASS++;
	 }
	 else
	 {
	     # TODO: if accession already exist in database then insert it's refernce and with.
	     #print "$goa_seq_dbxref_id, $goid, $evidence already in database\n";
	     next;
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
}


close(FP);
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

$printResult = $printResult . "GOA_NODE:\t$node\n";
$printResult = $printResult . "GOA_FILE2PARSE:\t$file2parse\n";
$printResult = $printResult . "GOA_SEQDBXREF:\t$countSEQDBX\n";
$printResult = $printResult . "GOA_ASSOCIATION:\t$countASS\n";
$printResult = $printResult . "GOA_$node"."_TOTAL:\t" . ($countSEQDBX+$countASS) . "\n";
$printResult = $printResult . "GOA_$node"."_TIME:\t" . ($t2-$t1) ."\n";
$printResult = $printResult . "Execution time:\t" . ($t2-$t1)/60 ." min.\n";
$printResult = $printResult . "\n================================================================================\n";

print("$printResult\n");



exit(0);



sub prepareSelectGoaSeqDbxrefID
{
    $sthGoaSeqDbxrefID = $_[0]->prepare("SELECT id FROM ".$_[1].".".$dbname_goa_seq_dbxref .
					" WHERE goa_xdb_id =? AND xref_key=?");
}

sub prepareInsertGoaSeqDbx
{
    $sthInsGoaSeqDbx = $_[0]->prepare("INSERT INTO " . $_[1] . ".".$dbname_goa_seq_dbxref .
				      "(goa_xdb_id, xref_key, taxid_1, taxid_2, type_id, symbol, full_name, synonym) " .
				      "VALUES(?, ?, ?, ?, ?, ?, ?, ?)");
}

sub prepareInsertGoaAssociation($, $)
{
    $sthInsGoaAss = $_[0]->prepare("INSERT IGNORE INTO " . $_[1] . "." . $dbname_goa_association .
				   "(is_not, go_id, goa_seq_dbxref_id, assigned_by, code, date) " .
				   "VALUES(?, ?, ?, ?, ?, ?)");
}

# Function that creates a Hast Table form the table goa_xdb
# Hash table: hash_abbr is global to the hole script.
# Key: abbreviation
# Value: goa_xdb.id
# This hash table was created to avoid searching into goa_xdb each time
# an ID is needed from a particular abbreviation name.
sub createAbbrHashTable()
{
    $sth = $dbh->prepare("SELECT abbreviation, id from " . $database . ".goa_xdb");
    $sth->execute();

#    print($row[0] ."\t" . $row[1] . "\n");

    #Crete an abreviation id hash.
    $hash_abbr{unpublished} = 0;

    while (my @row = $sth->fetchrow_array)
    {
#        print($row[0] ."\t" . $row[1] . "\n");
        $hash_abbr{$row[0]} = $row[1];
    }
}
