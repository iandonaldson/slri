#File that converts from TIGR_ath Id's to Refseq Ids. Takes 2 input
#files: release5.genbank_accessions.txt from 
#ftp.tigr.org/pub/data/a_thaliana/ath1 and gp2accession.tigr_ath from
#
#to run: perl dbxref_tigr_ath_cluster.pl.
#


# two choices are available - to use DBD-mysql or to use DBI to connect to
# MySQL-db.
# DBI is a most universal one (DBD-mysql is too specific)
use strict;
use Getopt::Long;
use DBI qw(:sql_types);
use dbxref;

# database stuff
my $user_name = dbxref->getProperty("dbxref.ini", "temp_user");
my $password  = dbxref->getProperty("dbxref.ini", "temp_password");
my $host      = dbxref->getProperty("dbxref.ini", "temp_host");
my $port      = dbxref->getProperty("dbxref.ini", "temp_port");
my $database  = dbxref->getProperty("dbxref.ini", "temp_database");
my $table     = dbxref->getProperty("dbxref.ini", "temp_table");


my $data_dir  =dbxref->getProperty("dbxref.ini", "data_dir");

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
      dbxref->displayMessage('dbxref_tigr_ath_cluster.pl');
}
if ($cluster eq 'T')
{
   if ( (! $offset) and (! $count2parse) )
   {
      dbxref->displayMessage('dbxref_tigr_ath_cluster.pl');
   }
}
if (($db ne 'XREF_TIGR_ATH') and ($db ne 'XREF_TIGR_ATH_GP'))
{
    dbxref->displayMessage('dbxref_tigr_ath_cluster.pl');
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
dbxref->prepareID($dbh, $database, $table);
dbxref->prepareInsert($dbh, $database, $table);

my %dbtable = dbxref->createDbHashTable( $dbh, $database );
my $dbId = $dbtable{ "TIGR_ATH"}->source_id();
my $genbank = $dbtable{ "GB"}->source_id();
my $file2parse;
# open the dbxref file.
my $countATH = 0;
my $countGB = 0;
my $count = 0;

if ($db eq 'XREF_TIGR_ATH')
{
   $file2parse = $data_dir . dbxref->getProperty( "dbxref.ini", "XREF_TIGR_ATH" );

   open(XREFFILE, $file2parse) or die "can't open $file2parse: $!";

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

   # Begin parsing the file.
   while( <XREFFILE> )
   {
      my $parentId = 0;
      my @rowId;
      my $linkId   = 0;

      chomp;

      $count++;
      @xrefs = split(/\t/);

      my $athId  = $xrefs[ 0 ];
      my $protId = $xrefs[ 1 ];
      my $geneId = $xrefs[ 2 ];

      #-------------------------------------------------------------
      # Do we already have a parent id for this id?
      if( length( $athId ) > 0 )
      {
	 if( @rowId = dbxref->selectID( $dbId, $athId, 0 ) )
	 {
	    $parentId = $rowId[0];
	 }
	 else
	 {
	     if(dbxref->executeInsert($dbId, $athId, 0, 0, 'Col 1', 0 ))
	     {
	        $countATH++;
	     }

	     if( @rowId = dbxref->selectID( $dbId, $athId, 0 ) )
	     {
	        $parentId = $rowId[0];
	     }
	  }
      }
	

      if( length( $geneId ) > 0)
      {
	 @rowId  = dbxref->selectID( $genbank, $geneId, $parentId );

	 if( $rowId[ 0 ] <= 0 )
	 {
	    if(dbxref->executeInsert($genbank, $geneId, $parentId, 0, 'Col 3', 1 ))
	    {
	       $countGB++;
	    }

	    if( @rowId = dbxref->selectID( $genbank, $geneId, $parentId )  )
	    {
		$linkId = $rowId[0];
	    }
	 }
      }
      if( length( $protId ) > 0)
      {
	 @rowId  = dbxref->selectID( $genbank, $protId, $parentId );
	 if( $rowId[ 0 ] <= 0 )
	 {
	    if(dbxref->executeInsert($genbank, $protId, $parentId, $linkId, 'Col 2', 2 ) )
	    {
	      $countGB++;
	    }
	 }
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
   }# end while
   close(XREFFILE);
}# end if($db eq 'XREF_TIGR_ATH')


###############################################################
# open the gp2protein.tigr_ath file.

if ($db eq 'XREF_TIGR_ATH_GP')
{
   $file2parse = $data_dir . dbxref->getProperty( "dbxref.ini", "XREF_TIGR_ATH_GP" );
   open(XREFFILE, $file2parse) or die "can't open $file2parse: $!";

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
   my $dbId = $dbtable{ "TIGR_ATH"}->source_id();
   my $genbank = $dbtable{ "GB"}->source_id();

   ###############################################################
   # Begin parsing the file.

   my $newCount = 0;
   my $newProt = 0;
   while( <XREFFILE> )
   {
      my $parentId = 0;
      my @rowId;
      my $linkId   = 0;

      chomp;
      $count++;

      @xrefs = split(/\t/);

      my $athId  = $xrefs[ 0 ];
      my $protId = $xrefs[ 1 ];

      my @xrefs = split( /:/, $athId );
      my $athId = $xrefs[ 1 ];

      my @xrefs = split( /:/, $protId );
      my $protId = $xrefs[ 1 ];
      $parentId = 0;
      #-------------------------------------------------------------
      # Do we already have a parent id for this id?
      if( length( $athId ) > 0 )
      {
	 if( @rowId = dbxref->selectID( $dbId, $athId, 0 ) )
	 {
	    $parentId = $rowId[0];
	 }
	 else
	 {
	    if( dbxref->executeInsert($dbId, $athId, 0, 0, 'Col 1', 0 ) )
	    {
	       $countATH++;
	    }

	    if( @rowId = dbxref->selectID( $dbId, $athId, 0 ) )
	    {
		$parentId = $rowId[0];
	    }
	 }
      }
      if( length( $protId ) > 0)
      {
         @rowId  = dbxref->selectID( $genbank, $protId, $parentId );
	 if( $rowId[ 0 ] <= 0 )
	 {
	    if(dbxref->executeInsert($genbank, $protId, $parentId, $linkId, 'Col 2', 2 ))
	    {
	       $countGB++;
	    }
	 }
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
    }#end while
    close(XREFFILE);
 }#end if($db eq 'XREF_TIGR_ATH_GP')


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

$printResult = $printResult . "TIGR_ATH_NODE:\t$node\n";
$printResult = $printResult . "TIGR_ATH_FILE2PARSE:\t$file2parse\n";
$printResult = $printResult . "TIGR_ATH_TIGR_ATH:\t$countATH\n";
$printResult = $printResult . "TIGR_ATH_GB:\t$countGB\n";
$printResult = $printResult . "TIGR_ATH_$node"."_TOTAL:\t" . ($countATH+$countGB) . "\n";
$printResult = $printResult . "TIGR_ATH_$node"."_TIME:\t" . ($t2-$t1) ."\n";
$printResult = $printResult . "Execution time:\t" . ($t2-$t1)/60 ." min.\n";
$printResult = $printResult . "\n================================================================================\n";

print("$printResult\n");

exit(0);






