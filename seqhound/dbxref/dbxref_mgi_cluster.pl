# Script that parse MRK_Sequence.rpt and creates dbxref table from 
# the mapps from MGI to "GenBank Accession IDs" and "RefSeq ID"
#
#
# File: MRK_Sequence.rpt
# MGI Markers associations to GenBank sequence information (tab-delimited)
#     MGI Marker ID 
#     Symbol
#     Name
#     cM Position
#     Chromosome
#     GenBank Accession IDs   (space-separated) 
#     Unigene ID              (if any)
#     RefSeq ID               (if any)

# two choises are available - to use DBD-mysql or to use DBI to connect to
# MySQL-db.
# DBI is a most universal one (DBD-mysql is too specific;
use strict;
use DBI;
use Getopt::Long;
use dbxref;


my $host      = dbxref->getProperty("dbxref.ini", "temp_host");
my $port      = dbxref->getProperty("dbxref.ini", "temp_port");
my $database  = dbxref->getProperty("dbxref.ini", "temp_database");
my $table     = dbxref->getProperty("dbxref.ini", "temp_table");
my $user_name = dbxref->getProperty("dbxref.ini", "temp_user");
my $password  = dbxref->getProperty("dbxref.ini", "temp_password");

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
      dbxref->displayMessage('dbxref_mgi_cluster.pl');
}
if ($cluster eq 'T')
{
   if ( (! $offset) and (! $count2parse) )
   {
      dbxref->displayMessage('dbxref_mgi_cluster.pl');
   }
}
if (($db ne 'XREF_MGI_GENES') and ($db ne 'XREF_MGI_PROT'))
{
    dbxref->displayMessage('dbxref_mgi_cluster.pl');
}

#die("die test\n");

# =============================================================


my $dsn = "DBI:mysql:" . $database . ":" . $host . ":" . $port;

my $dbh;

# 2) connect to MySQL-database
$dbh = DBI->connect ($dsn, $user_name, $password, {RaiseError => 1});

# 1) process external_id Yeast mapper table. Cross Reference database.

# Database Identifier in field source_id
my %dbtable = createDbHashTable($database, $dbh);
my $MGI = $dbtable{MGI}->source_id();

#Initialize dbxref Statements.
dbxref->prepareID($dbh, $database, $table);
dbxref->prepareInsert($dbh, $database, $table);


#dbxref->executeDelete($dbh, $database, $table, 5);
#die ("End test executeDelete()\n");
#######################################
# processing MRK_Sequence.rpt         #
#######################################



my $count       = 0;                             # Count the number of records
my $countMGI    = 0;
my $countRecord = 0;
my $countRefSeq = 0;
my $countGB     = 0;
my $countGI     = 0;
my $countSP     = 0;
my $mgiid = 0;
my $dbxrefid = 0;
my @rowid = ();
my $file2parse;

if ($db eq 'XREF_MGI_GENES')
{
   $file2parse = $data_dir . dbxref->getProperty("dbxref.ini", "XREF_MGI_GENES"); #"MRK_Sequence.rpt"
   #print("Opening file: $file2parse \n");
   open(FP, $file2parse) or die "can't open $file2parse: $!";

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
#       print($line);

       my $last = "";
       chomp($line);
       $count++;
       if ("!" ne substr($line, 0, 1))
       {
           my @record = split(/\t/, $line);

	   my $MGIID      = $record[0];
	   my $SYMBOL     = $record[1];
	   my $STATUS     = $record[2];
	   my $MARKTYPE   = $record[3];
	   my $NAME       = $record[4];
	   my $CMPOSITION = $record[5];
	   my $CHROMOSOME = $record[6];
	   my $ACCESSIONS = $record[7];
	   my $UNIGENID   = $record[8];
	   my $REFSEQID   = $record[9];

	   #	print("\n");
	   #	print("MGIID      : " . $MGIID      . "\n");
	   #	print("SYMBOL     : " . $SYMBOL     . "\n");
	   #	print("NAME       : " . $NAME       . "\n");
	   #	print("CMPOSITION : " . $CMPOSITION . "\n");
	   #	print("CHROMOSOME : " . $CHROMOSOME  . "\n");
	   #	print("ACCESSIONS : " . $ACCESSIONS . "\n");
	   #	print("UNIGENID   : " . $UNIGENID   . "\n");
	   #	print("REFSEQID   : " . $REFSEQID   . "\n\n");

	   if ( length($MGIID) > 0)
	   {
	      $dbxrefid = 0;
	      if (@rowid = dbxref->selectID(5, $MGIID, 0))
	      {
	         $dbxrefid = $rowid[0];
	      }
	      #	    print("dbxrefid: " . $dbxrefid . "\n");
	      #	    print("mgi: ". $MGIID . "\n");

	      if ($dbxrefid == 0)   # not found.
	      {
	         if(dbxref->executeInsert($MGI, $MGIID, 0, 0, 'Col 1', 0) == 1)
		 {
		    $countMGI++
		 }

		 if (@rowid = dbxref->selectID(5, $MGIID, 0))
		 {
		    $dbxrefid = $rowid[0];
		 }
	      }
	   }
	   if ( length($ACCESSIONS) > 0 )
	   {
	        my @AccessionArr = split(/ /, $ACCESSIONS);
		foreach(@AccessionArr)
		{
		     if(dbxref->executeInsert(2, $_, $dbxrefid, 0, 'Col 8', 1 ) == 1)
		     {
		        $countGB++;
		     }
		}
		@AccessionArr=();
	   }

	   if (length($REFSEQID) > 0)
	   {
	      my @RefSeqArr = split(/ /, $REFSEQID);
	      foreach(@RefSeqArr)
	      {
	         if(dbxref->executeInsert(2, $_, $dbxrefid, 0, 'Col 9', 1) == 1)
		 {
		    $countRefSeq++;
		 }
	      }
	   }

	
	   $MGIID      = "";
	   $SYMBOL     = "";
	   $NAME       = "";
	   $CMPOSITION = "";
	   $CHROMOSOME = "";
	   $ACCESSIONS = "";
	   $UNIGENID   = "";
	   $REFSEQID   = "";

	   $dbxrefid = 0;
	   @rowid=();
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
   close(FP);
}


#######################################
# processing MRK_SwissProt_TrEMBL.rpt #
#######################################
if ($db eq 'XREF_MGI_PROT')
{
     $file2parse = $data_dir . dbxref->getProperty("dbxref.ini", "XREF_MGI_PROT");  #"MRK_SwissProt_TrEMBL.rpt

     open(FP, $file2parse) or die "can't open input file $file2parse: $!\n";

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
     my $count = 0;     # Count the number of records
     my $n = 0;
     while (my $line = <FP>)
     {
        chomp($line);
	$count++;
	$n++;
	my @record = split(/\t/, $line);
	# check and insert record
	#   print "#records = $#record\n";
	if ($#record==6 and $record[0]=~/(MGI:\d+)/ and $record[6] ne "")
	{
	   my $field1 = $1; # GI Marker ID
	   my $field2 = $record[6]; # SWISS-PROT/TrEMBL Protein Accession IDs (space-separated)
	   my $parent_id;

	   # to decide parent_id , check if record_id exists

	   $dbxrefid = 0;
	   my @idarr = ();
	   if (@idarr = dbxref->selectID(5, $field1, 0) )
	   {
	       $dbxrefid = $idarr[0];
	   }

	   if ( $dbxrefid == 0) # new record, create a top level id first
	   {
	      # parent_id =0
	      if (dbxref->executeInsert(5, $field1, 0, 0, 'Col 1', 0) == 1)
	      {
		 $countMGI++;
	      }
	      #get id for above new record
	      $dbxrefid = 0;
	      if (@idarr = dbxref->selectID(5, $field1, 0))
	      {
		 $dbxrefid = $idarr[0];
	      }

	      if ($dbxrefid != 0)
	      {
		 $parent_id = $dbxrefid;
	      }
	      else
	      {
		 print STDERR  "** insert error.**\n";
		 die;
	      }
	   }
	   else
	   {
	      if($idarr[0]!~/\d+/ or $#idarr>0)
	      {
                 print STDERR  "@idarr\n";
		 print STDERR  "line $n(duplicated parents): $_\n";
		 next;
	      }
	      $parent_id = $idarr[0];
	   }

	   my @ids = split(/ /, $field2);
	   if(!@ids)
	   {
	      print STDERR "line $n(no protein id): $_\n";
	      next;
	   }

	   my $id;
	   foreach $id(@ids)
	   {
	      if(length($id) ==6 and $id =~ /[OPQ]\d\w\w\w\d/)
	      {
		 if( dbxref->executeInsert(12, $id, $parent_id, 0, 'Col 7', 0) == 1)
		 {
		    $countSP++
		 }
	      }
	      else
	      {
                 # add as genbank accession to dbxref
		  if (dbxref->executeInsert(2, $id, $parent_id, 0, 'Col 8', 0) == 1)
		  {
		       $countGB++;
		  }
	      }
	   }
	}
	else
	{
	   print STDERR "line $n(invalid line): $_\n";
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

     close(FP);
}# end if

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

$printResult = $printResult . "MGI_NODE:\t$node\n";
$printResult = $printResult . "MGI_FILE2PARSE:\t$file2parse\n";
$printResult = $printResult . "MGI_MGI:\t$countMGI\n";
$printResult = $printResult . "MGI_GB:\t$countGB\n";
$printResult = $printResult . "MGI_SP:\t$countSP\n";
$printResult = $printResult . "MGI_RefSeq:\t$countRefSeq\n";
$printResult = $printResult . "MGI_$node"."_TOTAL:\t" . ($countMGI+$countGB+$countSP+$countRefSeq) . "\n";
$printResult = $printResult . "MGI_$node"."_TIME:\t" . ($t2-$t1) ."\n";
$printResult = $printResult . "Execution time:\t" . ($t2-$t1)/60 ." min.\n";
$printResult = $printResult . "\n================================================================================\n";

print("$printResult\n");

exit(0);


# Function that creats a hash table 
# with an object that represents dbabbrev. and 
# ifprimary flag with 1 or 0 if it's primary db or not.
# $_[0] database.
# $_[1] dbh.
sub createDbHashTable($, $)
{
    my $sthdb;
    my %hash_db;
    $sthdb = $_[1]->prepare("SELECT source_id, source_db, isprimary_db from " . $_[0] . ".dbxrefsourcedb");
    $sthdb->execute();
    while(my @row = $sthdb->fetchrow_array())
    {
        my $dbrec = new dbxref;
	$dbrec->setDBrecord($row[0], $row[2]);
	$hash_db{$row[1]} = $dbrec;
    }
    return %hash_db;
}
