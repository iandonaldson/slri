# $Id: dbxref_goa_xrefs_cluster.pl
#Parser reads 3 files that map various database ids to one another.
#We take 2 ids:
#
#1. IPI ids if we can map them to an existing SPTR parent in dbxref.
#
#2. ENSEMBLE ids as parents. We map cross references to IPI, SPTR,
#   RGD, MGI and genbank ids.
#
#The input files are:
#
#human.xrefs    ftp.ebi.ac.uk/pub/databases/GO/goa/HUMAN
#mouse.xrefs    ftp.ebi.ac.uk/pub/databases/GO/goa/MOUSE
#rat.xrefs    ftp.ebi.ac.uk/pub/databases/GO/goa/RAT
#


# two choises are available - to use DBD-mysql or to use DBI to connect to
# MySQL-db.
# DBI is a most universal one (DBD-mysql is too specific)
use strict;
use DBI;
use Getopt::Long;
use dbxref;

###############################################################
#Open a log file
#open( LOG, ">dbxref_goa_xrefs.log") or die "Cannot open log file\n";

###############################################################
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
      dbxref->displayMessage('dbxref_goa_xrefs_cluster.pl');
}
if ($cluster eq 'T')
{
   if ( (! $offset) and (! $count2parse) )
   {
      dbxref->displayMessage('dbxref_goa_xrefs_cluster.pl');
   }
}
if ( ($db ne 'XREF_XREFGOA_HUMAN') and ($db ne 'XREF_XREFGOA_MOUSE') and ($db ne 'XREF_XREFGOA_RAT') )
{
    dbxref->displayMessage('dbxref_goa_xrefs_cluster.pl');
}
# =============================================================

my $dbh;

my @xrefs;

my $dsn = "DBI:mysql:" . $database . ":" . $host . ":" . $port;
	

###############################################################
# 2) connect to MySQL-database
$dbh = DBI->connect ($dsn, $user_name, $password, {RaiseError => 1});

###############################################################
#Prepare the statement handles.
#print ("Preparing the statement handles.\n" );

dbxref->prepareID($dbh, $database, $table);
dbxref->prepareInsert($dbh, $database, $table);

###############################################################
#  get the filenames.

#print (LOG scalar localtime, "\nOpen the file.\n" );

my $humanFile = $data_dir . dbxref->getProperty( "dbxref.ini", "XREF_XREFGOA_HUMAN" );

my $mouseFile = $data_dir . dbxref->getProperty( "dbxref.ini", "XREF_XREFGOA_MOUSE" );

my $ratFile = $data_dir . dbxref->getProperty( "dbxref.ini", "XREF_XREFGOA_RAT" );

my %dbtable = dbxref->createDbHashTable( $dbh, $database );

my $sptr    = $dbtable{ "SPTR"}->source_id();
my $ipi     = $dbtable{ "IPI" }->source_id();
my $ensembl = $dbtable{ "ENSEMBL" }->source_id();
my $hugo    = -1;
my $rgd     = $dbtable{ "RGD" }->source_id();
my $mgi     = $dbtable{ "MGI" }->source_id();
my $genbank = $dbtable{ "GB" }->source_id();

###############################################################
# Delete any existing values.
#dbxref->executeDelete($dbh, $database, $table, $ensembl);
#dbxref->executeDelete($dbh, $database, $table, $ipi);
#die "Finished deleting\n";

###############################################################
# Begin parsing the files.
#print (LOG "Parsing the file $humanFile\n" );

my $lineCount = 0;
my $ipiCount = 0;
my $ensemblCount  = 0;
my $genbankCount = 0;
my $sptrCount = 0;
my $mgiCount = 0;
my $rgdCount = 0;

###############################################################
# Parse human.xrefs

if ($db eq 'XREF_XREFGOA_HUMAN')
{
   parseFile( $humanFile, $hugo, 'XREF_XREFGOA_HUMAN' );
}

###############################################################
# Parse mouse.txt
if ($db eq 'XREF_XREFGOA_MOUSE')
{
   #print (LOG "Parsing the file $mouseFile\n" );
   parseFile( $mouseFile, $mgi, 'XREF_XREFGOA_MOUSE' );
}

###############################################################
# Parse rat.txt
if ($db eq 'XREF_XREFGOA_RAT')
{
   #print (LOG "Parsing the file $ratFile\n" );
   parseFile( $ratFile, $rgd, 'XREF_XREFGOA_RAT' );
}


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

$printResult = $printResult . "XREFGOA_NODE:\t$node\n";
if ($db eq 'XREF_GOA_HUMAN')
{
   $printResult = $printResult . "XREFGOA_FILE2PARSE:\t$humanFile\n";
}
elsif ($db eq 'XREF_GOA_MOUSE')
{
   $printResult = $printResult . "XREFGOA__FILE2PARSE:\t$mouseFile\n";
}
elsif ($db eq 'XREF_GOA_RAT')
{
   $printResult = $printResult . "XREFGOA_FILE2PARSE:\t$ratFile\n";
}

$printResult = $printResult . "XREFGOA_ENSEMBL:\t$ensemblCount\n";
$printResult = $printResult . "XREFGOA_GB:\t$genbankCount\n";
$printResult = $printResult . "XREFGOA_SP:\t$sptrCount\n";
$printResult = $printResult . "XREFGOA_IPI:\t$ipiCount\n";
$printResult = $printResult . "XREFGOA_MGI:\t$mgiCount\n";
$printResult = $printResult . "XREFGOA_RGD:\t$rgdCount\n";
my $totalCount = $ensemblCount + $genbankCount + $sptrCount + $mgiCount + $rgdCount+ $ipiCount;
$printResult = $printResult . "XREFGOA_$node"."_TOTAL:\t" . $totalCount . "\n";
$printResult = $printResult . "XREFGOA_$node"."_TIME:\t" . ($t2-$t1) ."\n";
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
#  @param dbInputId  Id of the genbank database. Either mouse, rat
#                     or human.
#
# @param  Orgainsm tag: a tag to know what organism it's been parsed.
#
sub parseFile
{
    my( $fileName, $dbInputId, $orgName ) = @_;
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

	  chomp;
	  $lineCount++;

	  @xrefs = split(/\t/);

	  ##############################################################
	  # remove the trailing ; if one exists.

	  foreach my $field ( @xrefs )
	  {
		$field =~ s/;\s*$//;
	  }

	  my $db;
	  my $objectId;
	  my $ipiId;
	 #my $swissProts;
	  my $ensembls;
	  my $NPRefseq;
	  my $XPRefseq;
	  my $orgId;

	  if ($orgName eq 'XREF_XREFGOA_HUMAN')
	  {
	      $db         = $xrefs[ 0 ];
	      $objectId   = $xrefs[ 1 ];
	      $ipiId      = $xrefs[ 2 ];
	      if ($db =~ /ENSEMBL/ )
	      {
		  $NPRefseq   = $xrefs[ 6 ];
		  $XPRefseq   = $xrefs[ 7 ];
	      }
	      else
	      {
		  $ensembls   = $xrefs[ 5 ];
		  $NPRefseq   = $xrefs[ 6 ];
		  $XPRefseq   = $xrefs[ 7 ];
	      }
	      $orgId      = $xrefs[ 8 ];
	  }
	  if ($orgName eq 'XREF_XREFGOA_MOUSE')
	  {
	      $db         = $xrefs[ 0 ];
	      $objectId   = $xrefs[ 1 ];
	      $ipiId      = $xrefs[ 2 ];
	      if ($db =~ /ENSEMBL/ )
	      {
		  $NPRefseq   = $xrefs[ 6 ];
		  $XPRefseq   = $xrefs[ 7 ];
	      }
	      else
	      {
		  $ensembls   = $xrefs[ 5 ];
		  $NPRefseq   = $xrefs[ 6 ];
		  $XPRefseq   = $xrefs[ 7 ];
	      }
	      $orgId      = $xrefs[ 8 ];
	  }
	  elsif($orgName eq 'XREF_XREFGOA_RAT')
	  {
	      $db         = $xrefs[ 0 ];
	      $objectId   = $xrefs[ 1 ];
	      $ipiId      = $xrefs[ 2 ];
	      $ensembls   = $xrefs[ 4 ];
	      if($db =~ /ENSEMBL/ )
	      {
		  $NPRefseq   = $xrefs[ 6 ];
		  $XPRefseq   = $xrefs[ 7 ];
	      }
	      else
	      {
		  $ensembls   = $xrefs[ 5 ];
		  $NPRefseq   = $xrefs[ 6 ];
		  $XPRefseq   = $xrefs[ 7 ];

	      }
	      $orgId      = $xrefs[ 8 ];
	  }
	  my @Ids;

	 # print "Reading line $db, $objectId, $ipiId, $swissProts, " .
		#"$ensembls, $NPRefseq, $XPRefseq, $orgId\n";

	  ##############################################################
	  #if the database is swissprot or trembl and we already have
	  # a parent id, insert an ibiId child.
	#  print("db: $db\n");
#	  print("objectId: $objectId\n");
#	  print("ipiId: $ipiId\n");

#	  print("xrefs[0]: $xrefs[0]\n");
#	  print("xrefs[1]: $xrefs[1]\n");
#	  print("xrefs[2]: $xrefs[2]\n");
#	  print("xrefs[3]: $xrefs[3]\n");
#	  print("xrefs[4]: $xrefs[4]\n");
#	  print("xrefs[5]: $xrefs[5]\n");
#	  print("xrefs[6]: $xrefs[6]\n");
#	  print("xrefs[7]: $xrefs[7]\n");

#	  print("ensembls: $ensembls\n");
#	  print("NPRefseq: $NPRefseq\n");
#	  print("XPRefseq: $XPRefseq\n");
#	  print("orgId: $orgId\n\n");
	  my @row=();
	  $parentId = 0;
	  if( $db =~ /SP/ || $db =~ /TR/ )
	  {
		@row = dbxref->selectID( $sptr, $objectId, 0 );
		
		if(length($row[0]) == 0)   # SP id doesn't exist.
		{
		    if(dbxref->executeInsert($sptr, $objectId, 0, 0, 'Col 2', 0) == 1)
		    {
		        $sptrCount++;
			# RAC Test
			#if ($sptrCount == 20) {die("End Test\n\n");}
		    }
		}
		$parentId = dbxref->selectID( $sptr, $objectId, 0 );

		if( length( $ensembls ) > 0 )
		{
		    my $Id;
		    @Ids = split(/;/,$ensembls);
		    foreach $Id ( @Ids )
		    {
		        if(dbxref->executeInsert($ensembl, $Id, $parentId, 0, 'Col 6', 2 ) == 1)
			{
			    $ensemblCount++;
			}
		    }
		}
	  }

	  if( $db =~ /ENSEMBL/ )
	  {
	      @row = dbxref->selectID( $ensembl, $objectId, 0 );
	      if(length($row[0] == 0))
	      {
		  if(dbxref->executeInsert($ensembl, $objectId, 0, 0, 'Col 2', 0) == 1)
		  {
		        $ensemblCount++;
		  }
	      }
	      $parentId = dbxref->selectID( $ensembl, $objectId, 0 );
	  }

	  if($parentId > 0)
	  {

	      if( length( $ipiId) > 0 )
	      {
	          if(dbxref->executeInsert($ipi, $ipiId, $parentId, 0, 'Col 3', 0 ) == 1)
		  {
		      $ipiCount++;
		  }
	      }
	      if( length( $NPRefseq ) > 0 )
	      {
		  my $Id;
		  my @Ids = ();
		  if ($NPRefseq =~ /(\*)/)
		  {
		      @Ids = split(/\*/,$NPRefseq);
		  }
		  else
		  {
		      @Ids = split(/;/,$NPRefseq);
		  }
		  foreach $Id(@Ids)
		  {
		      if(dbxref->executeInsert($genbank, $Id, $parentId, 0, 'Col 7', 2 ) == 1)
		      {
			  $genbankCount++;
		      }
		  }
	      }
	      if( length( $XPRefseq ) > 0 )
	      {
		  my $Id;
		  my @Ids = ();
		  if ($XPRefseq =~ /(\*)/)
		  {
		      @Ids = split(/\*/,$XPRefseq);
		  }
		  else
		  {
		      @Ids = split(/;/,$XPRefseq);
		  }
		  foreach $Id(@Ids)
		  {
		      if (dbxref->executeInsert($genbank, $Id, $parentId, 0, 'Col 8', 2 ) == 1)
		      {
			  $genbankCount++;
		      }
		  }
	      }
	      if(length( $orgId ) > 0 )
	      {
		  my @Ids = split(/\,/, $orgId);
		  my $Id = $Ids[ 0 ];
		  if( $Id =~ /MGI/ )
		  {
		      if(dbxref->executeInsert($mgi , $Id, $parentId, 0, 'Col 9', 0) == 1)
		      {
			  $mgiCount++;
		      }
		  }
	      }
	    }# if (parentId > 0)
			
	  # =================  CLUSTERING ================================
	  if ($cluster eq 'T')
	  {
	     if ($lineCount >= $count2parse)
	     {
	        last;
	     }
	  }
	  # ===============================================================

	}
    }




# $Log: dbxref_goa_xrefs_cluster.pl,v $
# Revision 1.6  2005/04/14 15:40:26  rcavero
# To make sure that nothing was modified.
#
# Revision 1.1  2005/03/29 19:40:16  rcavero
# dbxref_goa_xrefs_cluster.pl will replace dbxref_goa_xrefs.pl
#
# Revision 1.5  2004/07/27 17:41:58  rcavero
# There was a bug in line 252 $id instead of $Id.
#
# Revision 1.4  2004/06/17 15:59:57  rcavero
# goa updates
#
# Revision 1.3  2004/06/01 20:24:37  eburgess
# Fixed bug in entering additional swissprot Ids. Bugzilla 816.
#
















