#Parser reads 3 files that map ZFIN id to Genbank,Refseq or SPTR ids and
#writes the mapping to dbxref.
#
#The input files are  genebank.txt and refseq.txt downloaded
#from http://zfin.org/zf_info/downloads.html and gp2protein.zfin
#from ftp.geneontology.org/pub/go/gp2protein
#
#to run: perl dbxref_zfin.pl
#
#log file: dbxref_zfin.log


# two choises are available - to use DBD-mysql or to use DBI to connect to
# MySQL-db.
# DBI is a most universal one (DBD-mysql is too specific)
use strict;
use Getopt::Long;
use DBI;
use dbxref;


# database stuff
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
my $db;
my $offset;
my $count2parse;
my $dummy = '';

my $retValue = 0;
$retValue = GetOptions('c=s' => \$cluster, 'd=s' => \$db, 'o=i' => \$offset,'n=i' => \$count2parse, 'f=s' => \$dummy );

if( ($cluster ne 'T') and ($cluster ne 'F') )
{
      dbxref->displayMessage('dbxref_zfin_cluster.pl');
}
if ($cluster eq 'T')
{
   if ( (! $offset) and (! $count2parse) )
   {
      dbxref->displayMessage('dbxref_zfin_cluster.pl');
   }
}
if ( ($db ne 'XREF_ZFINGB') and ($db ne 'XREF_ZFINRS') and ($db ne 'XREF_ZFINGP') )
{
    dbxref->displayMessage('dbxref_zfin_cluster.pl');
}


#die("die test\n");
# =============================================================


my $dbh;

my @xrefs;

my $dsn = "DBI:mysql:" . $database . ":" . $host . ":" . $port;


###############################################################
# 2) connect to MySQL-database
$dbh = DBI->connect ($dsn, $user_name, $password, {RaiseError => 1});

###############################################################
#Prepare the statement handles.
print ("Preparing the statement handles.\n" );

dbxref->prepareID($dbh, $database, $table);
dbxref->prepareInsert($dbh, $database, $table);

###############################################################
#  get the filenames.

my $genbankFile = $data_dir . dbxref->getProperty( "dbxref.ini", "XREF_ZFINGB" );

my $refseqFile = $data_dir . dbxref->getProperty( "dbxref.ini", "XREF_ZFINRS" );

my $gp2prFile = $data_dir . dbxref->getProperty( "dbxref.ini", "XREF_ZFINGP" );

my %dbtable = dbxref->createDbHashTable( $dbh, $database );
my $dbId = $dbtable{ "ZFIN"}->source_id();
my $genbank = $dbtable{ "GB"}->source_id();
my $refseq = $dbtable{ "GB"}->source_id();
my $sptr = $dbtable{ "SPTR" }->source_id();

###############################################################
# Delete any existing values.
#dbxref->executeDelete($dbh, $database, $table, $dbId);

###############################################################
# Begin parsing the files.

my $lineCount = 0;
my $countZFIN  = 0;
my $countGB = 0;
my $countSPTR = 0;
my $count = 0;
###############################################################
# Parse genbank.txt
if ($db eq 'XREF_ZFINGB')
{
   parseFile( $genbankFile, $genbank );
}

###############################################################
# Parse refseq.txt
if ($db eq 'XREF_ZFINRS')
{
   parseFile( $refseqFile, $refseq );
}
###############################################################
# Parse gp2protein.zfin
if ($db eq 'XREF_ZFINGP')
{
   open(XREFFILE, $gp2prFile ) or die "can't open $gp2prFile: $!";
   # =================  CLUSTERING ================================
   my $recCount = 0;
   my $l;
   if ($cluster eq 'T')
   {
      if ($offset > 0)
      {
	 while ($l = <XREFFILE>)
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
   while( <XREFFILE> )
   {
      my @row;
      my $parentId;
      my @rowId;

      my $sptrId;
      my $zfinId;
      my $id;
      my $parentId;
      $count++;
      if ( !/^!/ )
      {
	 $lineCount++;
	 @row = split(/\t/);

	 $zfinId = $row[ 0 ];
	 $zfinId =~ s/ZFIN://;

	 $parentId = InsertZfinId( $zfinId );
	
	 @row = split(/;/, $row[ 1 ]);

	 foreach $id ( @row )
	 {
	    $sptrId = $id;
	    $sptrId =~ s/\s//;
	    $sptrId =~ s/SPTR://;
	    $sptrId =~ /(UniProt:)(\S+)/;
	    $sptrId = $2;

	    if( length( $sptrId ) > 0)
	    {
		@rowId = dbxref->selectID( $sptr, $sptrId, $parentId );

		if( $rowId[ 0] <= 0)
		{
		    #print "Adding $sptrId, $parentId\n";
		    dbxref->executeInsert($sptr, $sptrId, $parentId, 0, 'Col 2', 2 );
		    $countSPTR++;
		}
	    }
	 }
      }# end if( !/~!/ )
      # =================  CLUSTERING ================================
      if ($cluster eq 'T')
      {
	   if ($count == $count2parse)
	   {
	      last;
	   }
      }
      # ===============================================================

   }#end while
}#end if

dbxref->finishSelectID();
dbxref->finishInsert();
$dbh->disconnect();   # Put this into dbxref.pm
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

$printResult = $printResult . "ZFIN_NODE:\t$node\n";
if ($db eq 'XREF_ZFINGB')
{
   $printResult = $printResult . "ZFIN_FILE2PARSE:\t$genbankFile\n";
}
elsif ($db eq 'XREF_ZFINRS')
{
   $printResult = $printResult . "ZFIN_FILE2PARSE:\t$refseqFile\n";
}
elsif ($db eq 'XREF_ZFINGP')
{
   $printResult = $printResult . "ZFIN_FILE2PARSE:\t$gp2prFile\n";
}
$printResult = $printResult . "ZFIN_ZFIN:\t$countZFIN\n";
$printResult = $printResult . "ZFIN_GB:\t$countGB\n";
$printResult = $printResult . "ZFIN_SP:\t$countSPTR\n";
$printResult = $printResult . "ZFIN_$node"."_TOTAL:\t" . ($countZFIN+$countGB+$countSPTR) . "\n";
$printResult = $printResult . "ZFIN_$node"."_TIME:\t" . ($t2-$t1) ."\n";
$printResult = $printResult . "Execution time:\t" . ($t2-$t1)/60 ." min.\n";
$printResult = $printResult . "\n================================================================================\n";

print("$printResult\n");

exit(0);



###################################################################
#  ParseFile
#
#  Parses a either a file containing refseq accessions or genbank 
#  accessions.
#
#  @param fileName   Name of the file to be opened.
#
#  @param dbInputId  Id of the genbank database. Either 2 for genbank
#                    or 18 for refseq
#
sub parseFile
{
    my( $fileName, $dbInputId ) = @_;

    open(XREFFILE, $fileName) or die "can't open $fileName: $!";

    # =================  CLUSTERING ================================
    my $recCount = 0;
    my $l;
    if ($cluster eq 'T')
    {
       if ($offset > 0)
       {
	  while ($l = <XREFFILE>)
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

    while( <XREFFILE> )
    {
	my $parentId;
	my @rowId;

	my $refseqId;
	my $zfinId;

	chomp;
	$lineCount++;
	$count++;
	@xrefs = split(/\t/);

	my $zfinId  = $xrefs[0];
	my $gbId = $xrefs[2];

        #-------------------------------------------------------------
	# Do we already have a parent id for this id?
	if( length( $zfinId ) > 0 )
	{
	    $parentId = InsertZfinId( $zfinId );
	}
	else
	{
	    print("ERROR dbxrerf_zfin_cluster: No zfin id for line $lineCount\n");
	}

	if( length( $gbId ) > 0 )
	{
	    @rowId  = dbxref->selectID( $genbank, $gbId, $parentId );

	    if( $rowId[ 0 ] <= 0 )
	    {
		dbxref->executeInsert($dbInputId, $gbId, $parentId, 0, 'Col 3', 0 );
		$countGB++;
	    }
	}
	else
	{
	    print ("ERROR dbxref_zfin_cluster: No genbank id for line $lineCount\n");
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
      }#while
}

###################################################################
#  InsertZfinId
#
#  Checks to see if a given ZfinId already exists as a parent. If it
#  doesn't then add it. The Id of the parent is returned.
#
#  @param zfinId - Id to insert.
#
#  @return Id of the parent record.

sub InsertZfinId
{
    my ($zfinId) = @_;
    my @rowId;
    my $parentId;

    if( @rowId = dbxref->selectID( $dbId, $zfinId, 0 ) )
    {
	$parentId = $rowId[0];
    }
    else
    {
	dbxref->executeInsert($dbId, $zfinId, 0, 0, 'Col 1', 0 );
	
	if( @rowId = dbxref->selectID( $dbId, $zfinId, 0 ) )
	{
	    $parentId = $rowId[0];
	    $countZFIN++;
	}
    }

    if( $parentId <= 0 )
    {
	print ("ERROR dbxerf_zfin_cluster.pl: InsertZfinId:Failed to find parentId for $zfinId.\n");
    }
    return $parentId;
}


















