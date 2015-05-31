# $Id: dbxref_fb_cluster.pl
# Parse external-ddatabase.txt for FB from
#  http://flybase.bio.indiana.edu/allied-data/extdb/external-databases.txt
#
# two choises are available - to use DBD-mysql or to use DBI to connect to
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

my $data_dir  = dbxref->getProperty("dbxref.ini", "data_dir");

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
      dbxref->displayMessage('dbxref_fb_cluster.pl');
}
if ($cluster eq 'T')
{
   if ( (! $offset) and (! $count2parse) )
   {
      dbxref->displayMessage('dbxref_fb_cluster.pl');
   }
}

#die("die test\n");

# =============================================================

my $dbh;
my $sth;

my $dsn = "DBI:mysql:" . $database . ":" . $host . ":" . $port;

# 2) connect to MySQL-database
$dbh = DBI->connect ($dsn, $user_name, $password, {RaiseError => 1});

# 1) process external_id Yeast mapper table. Cross Reference database.

#####################################################
# Delete all existing records before adding new ones.
my %dbtable = dbxref->createDbHashTable( $dbh, $database );
my $dbId = $dbtable{"FB"}->source_id();

# Create Statements.
dbxref->prepareID($dbh, $database, $table);
dbxref->prepareInsert($dbh, $database, $table);

my $file2parse = $data_dir . dbxref->getProperty( "dbxref.ini", "XREF_FB" );
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

my $count = 0;                             # Count the number of records
my $count_fb = 0;
my $count_gb = 0;
my $count_cg = 0;
my $count_sp = 0;
my $count_tr = 0;

while (my $line = <FP>)
{
#    print($line);

    my $last = "";
    chomp($line);
    $count++;
    if ("!" ne substr($line, 0, 1))
    {
        my @record = split(/\|/, $line);

	my $field1 = $record[0];
	my $FB     = $record[1];
	my $field2 = $record[2];
	my $field3 = $record[3];
	my $field4 = $record[4];
	my $field5 = $record[5];

#	print("\n");
#	print("field1: " . $field1 . "\n");
#	print("fb:     " . $FB     . "\n");
#	print("field2: " . $field2 . "\n");
#	print("field3: " . $field3 . "\n");
#	print("field4: " . $field4 . "\n");
#	print("field5: " . $field5 . "\n");
	
	if (index($FB, "[") > 0 )
	{
	    $FB = substr($FB, 0, index($FB, "["));
	}
	my $KEYID;
	my @fbid = dbxref->selectID(9, $FB, 0);

	if ( length($fbid[0]) == 0 )
	{
	    if( dbxref->executeInsert(9, $FB, 0, 0, 'Col 2', 0) )
	    {
	        $count_fb++;
	    }
	    if (my @arr = dbxref->selectID(9, $FB, 0))
	    {
	        $KEYID = $arr[0];
#		print("\t\t KEYID: " . $KEYID . "\n");
	    }
		#$count_fb++;
	}
	else
	{
	    $KEYID = $fbid[0];
	}
#	print("index1: " . index($field1, ";", 0) ."\n");
#	print("index2: " . index($field1, ":", 0) ."\n");

	my $lnk;

	if ( (index($field1, ";", 0) < 0) and (index($field1, ":", 0) < 0) )
	{
#	   print("GB: " . $field1 . "\n");

	   if( dbxref->executeInsert(2, $field1, $KEYID, 0, 'Col 1a', 1))
	   {
	      $count_gb++;
	   }
	}
	elsif( (index($field1, ";", 0) > 0) and (index($field1, ":", 0) < 0) )
	{
#	   print("DNA, Prot: " . $field1 . "\n");
	   my @dnaprot = split(/\;/, $field1);
	   if ( scalar(@dnaprot) > 1)
	   {
#	       print("dna:  " . $dnaprot[0] . "\n");
#	       print("prot: " . $dnaprot[1] . "\n");
	       if(dbxref->executeInsert(2, $dnaprot[0], $KEYID, 0, 'Col 1a', 1) == 1)
	       {
		  $count_gb++;
	       }
	       if(my @link = dbxref->selectID(2, $dnaprot[0], $KEYID ))
	       {
			$lnk = $link[0];
#	 	   print("\t\t link: " . $lnk . "\n");
			 $dnaprot[1] = substr($dnaprot[1], 1, length($dnaprot[1]));
			 if (dbxref->executeInsert(2, $dnaprot[1], $KEYID, $lnk, 'Col 1b', 2))
			 {
			    $count_gb++;
			 }
	       }
	   }
	   if(scalar(@dnaprot) == 3)
	   {
 	       $dnaprot[2] = substr($dnaprot[2], 1, length($dnaprot[2]));
	       if(dbxref->executeInsert(2, $dnaprot[2], $KEYID, $lnk, 'Col 1b', 2) == 1)
	       {
		 $count_gb++;
	       }
	   }	
	}
	elsif( (index($field1, ";", 0) > 0) and (index($field1, ":", 0) > 0) )
	{
#	   print("DNA, Prot: " . $field1 . "\n");
	   my @dnaprot = split(/\;/, $field1);
	   if ( scalar(@dnaprot) == 3 )
	   {
#	       print("dna:  " . $dnaprot[0] . "\n");
#	       print("prot: " . $dnaprot[1] . "\n");
	       if(dbxref->executeInsert(2, $dnaprot[0], $KEYID, 0, 'Col 1a', 1))
	       {
		   $count_gb++;
	       }
	       if (my @link = dbxref->selectID(9, $FB, 0))
	       {
			 $lnk = $link[0];
#	 	   print("\t\t link: " . $lnk . "\n");
			 $dnaprot[1] = substr($dnaprot[1], 1, length($dnaprot[1]));
			 if(dbxref->executeInsert(2, $dnaprot[1], $KEYID, $lnk, 'Col 1b', 2))
			 {
			    $count_gb++;
			 }
	       }
	     }
	   if ( scalar(@dnaprot) == 2 )
	   {
	       if (dbxref->executeInsert(2, $dnaprot[0], $KEYID, 0, 'Col 1a', 1))
	       {
		  $count_gb++;
	       }
	   }
	}
	elsif ( index($field1, ":", 0) > 0 )
	{
	  if (substr($field1, 0, length("SWP:")) eq "SWP:")
	  {
	       $field1 = substr($field1, length("SWP:"), length($field1));
	       if(dbxerf->executeInsert(12, $field1, $KEYID, 0, 'Col 1', 100))
	       {
		  $count_sp++;
	       }
	  }
	  elsif (substr($field1, 0, length("SPTREMBL:")) eq "SPTREMBL:")
	  {
  	       $field1 = substr($field1, length("SPTREMBL:"), length($field1));
	       if (dbxref->executeInsert(12, $field1, $KEYID, 0, 'Col 1', 101))
	       {
		  $count_tr++;
	       }
	  }
	}
	if (substr($field2, 0, length("CG")) eq "CG")
	{
	   my @CG = dbxref->selectID(2, $field2, $KEYID);
	   if ( length($CG[0]) == 0 )
	   {
	      #print("CG: " . $field2 ."\n");
	      if (dbxref->executeInsert(10, $field2, $KEYID, 0, 'Col 3', 0))
	      {
		 $count_cg++;
	      }
	  }
	}
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

# ==============================================================

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

$printResult = $printResult . "FB_NODE:\t$node\n";
$printResult = $printResult . "FB_FILE2PARSE:\t$file2parse\n";
$printResult = $printResult . "FB_FB:\t$count_fb\n";
$printResult = $printResult . "FB_GB:\t$count_gb\n";
$printResult = $printResult . "FB_SP:\t$count_sp\n";
$printResult = $printResult . "FB_TR:\t$count_tr\n";
$printResult = $printResult . "FB_CG:\t$count_cg\n";
$printResult = $printResult . "FB_$node"."_TOTAL:\t" . ($count_fb + $count_gb + $count_cg + $count_sp + $count_tr) . "\n";
$printResult = $printResult . "FB_$node"."_TIME:\t" . ($t2-$t1) ."\n";
$printResult = $printResult . "Execution time:\t" . ($t2-$t1)/60 ." min.\n";
$printResult = $printResult . "\n================================================================================\n";

print("$printResult\n");
exit(0);



# Fuction removeVersion() remove the version number for a given Accession number.
# Argument: arg1: Accession Number.
# Return Value: Accession number with ".x" version removed.
# Ex: removeVersion(NP_012480.1);
# return NP_012480

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

