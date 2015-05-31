#!/usr/bin/perl
# dbxref_sptr_cluster.pl
# Program thats paralelize the parsing of "Swissprot" or "Trembl" files.
#
# Arg1: tag: XREF_SP or XREF_TREMBL tag from dbxref.ini (Where is the file it's located).
# Arg2: offset: initial offset in recors of the file.
# Arg3: count2parse number of records to process from the offset.
#
use strict;
use DBI;
use Getopt::Long;
#use lib '/home/rcavero/dbxrefgoa';
use dbxref;

my $host      = dbxref->getProperty("dbxref.ini", "temp_host");     #"localhost";
my $port      = dbxref->getProperty("dbxref.ini", "temp_port");     #"3306";
my $database  = dbxref->getProperty("dbxref.ini", "temp_database"); #"dbxrefgo";
my $table     = dbxref->getProperty("dbxref.ini", "temp_table");    #"dbxref";
my $user_name = dbxref->getProperty("dbxref.ini", "temp_user");     #"root";
my $password  = dbxref->getProperty("dbxref.ini", "temp_password"); #"asdf1234";

my $data_dir  = dbxref->getProperty("dbxref.ini", "data_dir");

# =================  CLUSTERING ================================
# $filetype can be: XREF_SP or XREF_TREMBL.
my $cluster;
my $db;
my $offset;
my $count2parse;
my $dummy = '';

my $retValue = 0;
$retValue = GetOptions('c=s' => \$cluster, 'd=s' => \$db, 'o=i' => \$offset,'n=i' => \$count2parse, 'f=s' => \$dummy );

if( ($cluster ne 'T') and ($cluster ne 'F') )
{
      displayMessage();
}
if (! $db)
{
      displayMessage();
}
if( ($db ne 'XREF_SP') and ($db ne 'XREF_TR') )
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

my $filetype = $db;

#print("cluster: $cluster \n");
#print("db: $db \n");
#print("offset: $offset \n");
#print("count2parse: $count2parse \n");
#print("dummy: $dummy \n");
#===============================================================
#die("die Test\n");

my $file2parse= $data_dir . dbxref->getProperty("dbxref.ini", $filetype);

my $dsn = "DBI:mysql:" . $database . ":" . $host . ":" . $port;

my $dbh;
my $sth;
my @ary;

# 2) connect to MySQL-database
$dbh = DBI->connect ($dsn, $user_name, $password, {RaiseError => 1});

# Create Statements.
dbxref->prepareID($dbh, $database, $table);
dbxref->prepareInsert($dbh, $database, $table);

# $cv was defined to diferenciate SPTR to be SP or TR.
# SPTR = source_id = 12 but with cv=100 it's SP
#                                cv=101 it's TR
#

my $cv = 0;
my $cv2= 0;
my $file;
if ($filetype eq 'XREF_SP')
{
  $cv = 100;
  $cv2 = 110;                    # Used for secondary SP IDs.
  $file = "SP";
}
elsif ($filetype eq 'XREF_TR')
{
  $cv = 101;
  $cv2 = 111;                    # Used for secondary TR IDs.
  $file = "TR"
}

# get source db hash table, delete related SPTR records
my %dbtable = dbxref->createDbHashTable($dbh, $database);
my $SPTR=$dbtable{SPTR}->source_id();
# To do: remove secondary SP or TR IDs. (110 & 111). !!!
#dbxref->executeDeleteCV($dbh, $database, $table, $SPTR, $cv);

my $t1 = time();
my $count         = 0;                # Count the number of records
my $countNotIns   = 0;
my $countSPTR     = 0;
my $countSPTRsec  = 0;
my $countEMBL     = 0;
my $countPFAM     = 0;
my $countINTERPRO = 0;
my $countMGD      = 0;
my $countSGD      = 0;
my $countZFIN     = 0;
my $countSMART    = 0;
my $countFB       = 0;
my $countWB       = 0;
my $countMIM      = 0;
my $countHUGO     = 0;
my $countPIR      = 0;
my $countTIGR     = 0;
my $countSPOMBE   = 0;


# =================  CLUSTERING ================================
open(FP, $file2parse) or displayDieMessage($!, $file2parse);
my $recCount = 0;
my $line;
my $record;
if ($cluster eq 'T')
{
   if($offset > 0)
   {
      while ($line = <FP>)
      {
	 $record = $line;
	 if ( $record =~ /^\/\//)
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

while ($line = <FP>)
{
    #print($line);
    chomp($line);

    my @embl =();
    my @pfam = ();
    my @SMART = ();
    my @ENSEMBL = ();
    my @EMBL = ();
    my @SPTR = ();
    my @record = ();
    my @NUCT = ();
    my @PROT = ();
    my @FB = ();
    my @WB = ();
    my @PIR = ();
    my @IDList = ();
    my @SPTRList = ();
    my %accesion;
    my %interpro;
    my %pfam;
    my %mgd;
    my %sgd;
    my %smart;
    my %zfin;
    my %fb;
    my %wb;
    my %mim;
    my %hugo;
    my %pir;
    my %accession;
    my %tigr;
    my %spombe;
    my $TREMBLID   = "";
    my $EMBL       = "";
    my $SWISSPROT  = "";
    my $REFSEQ_NP  = "";	
    my $REFSEQ_XP  = "";
    my $ZFIN = "";
    my $GI         = 0;
    my $PFAMID     = 0;
    my $INTERPROID = 0;
    my $FB = 0;
    my $WB = 0;
    my $PIR = 0;
    my $MIM = 0;
    my $ACCount = 0;
    while ($line = <FP>)
    {
        if ("//" eq substr($line, 0, 2))
        {
	   # print("//\n");
	    last;
	}

	chomp($line);
	my $ID = $line;
#	print($line . "\n");

	if($line =~ /(AC   )/)
	{
	    $ACCount++;

	    $ID = substr($ID, length("AC   "));
	    if ($ACCount == 1)
	    {
	        $SPTR = substr($ID, 0, index($ID, ";"));
#		print("ID: " . $ID ."\n");
		chop($ID);                        # Remove last ;
		@SPTRList =  split(/; /, $ID);
		splice(@SPTRList, 0, 1);
	    }
	    else
	    {
	      chop ($ID);                         # Remove last ;
	      @IDList = split(/; /, $ID);
	      foreach(@IDList)
	      {
		  push(@SPTRList, $_)
	      }

	    }
	}
   	elsif($line =~ /(DR)/)
	{
	    if ($line =~ /(DR   EMBL; )/)
	    {
	        grapEMBL($line, \@NUCT, \@PROT );
	    }
	    elsif ($line =~ /(DR   Pfam; )/)
	    {
	        grapPfam($line, \%pfam);
	    }
	    elsif ($line =~ /(DR   InterPro; )/)
	    {
	        grapInterPro($line, \%interpro);
	    }
	    elsif ($line =~ /(DR   MGD; )/)
	    {
	        grapMGD($line, \%mgd);
	    }
	    elsif ($line =~ /(DR   SGD; )/)
	    {
	        grapSGD($line, \%sgd);
	    }
	    elsif ($line =~ /(DR   SMART; )/)
	    {
	        grapSMART($line, \%smart);
	    }
	    elsif ($line =~ /(DR   ZFIN; )/)
	    {
	        grapZFIN($line, \%zfin);
	    }
	    elsif ($line =~ /(DR   FlyBase; )/)
	    {
	        grapFB($line, \%fb);
	    }
	    elsif ($line =~ /(DR   WormPep; )/)
	    {
	        grapWB($line, \%wb);
	    }
	    elsif ($line =~ /(DR   MIM; )/)
	    {
	        grapOMIM($line, \%mim);
	    }
	    elsif ($line =~ /(DR   Genew; )/)
	    {
	        grapHUGO($line, \%hugo);
	    }
	    elsif ($line =~ /(DR   PIR;)/)
	    {
	        grapPIR($line,\%pir );
	    }
	    elsif ($line =~ /(DR   TIGR;)/)
	    {
	        grapTIGR($line,\%tigr );
	    }
	    elsif ($line =~ /(DR   GeneDB_Spombe;)/)
	    {
	        grapSPOMBE($line,\%spombe );
	    }
	}
    }
    $count++;

#    print($count . " " . "SPTR: " . $SPTR . "\n");

    # 12 = SP; cv = 100 : SP;  cv = 101 : TR
    if(dbxref->executeInsert('12', $SPTR, '0', '0', 'AC', $cv) == 1)
    {   $countSPTR++;    }
    else
    {   $countNotIns++;  }

    #$sthSelectSPID->execute('12', $SPTR);
    my $PKEY;
    if (my @row = dbxref->selectID(12, $SPTR, 0))
    {
#       print("key: " . $row[0] . "\n");
	$PKEY = $row[0];
    }
    # insert Secondary SP IDs.
    # 12 = SP; cv = 100 = SP;  cv = 101 = TR;  cv = 110 = SP Secondary ID; cv = 111 = TR Secondary ID;
    foreach (@SPTRList)
    {
         if(dbxref->executeInsert('12', $_, $PKEY, '0', 'AC', $cv2) == 1)
	 {   $countSPTRsec++;   }
	 else
	 {   $countNotIns++;  }
    }

    for (my $i = 0; $i<scalar(@NUCT); $i++)
    {

#        print("\t\t" . $SPTR . "\tACC:" . $NUCT[$i] . "\tPROT: " . $PROT[$i] . "\n");
        # 2 = GB
        if(dbxref->executeInsert('2', $NUCT[$i], $PKEY, '0', 'DR', '1') == 1)
	{   $countEMBL++;   }
	else
	{   $countNotIns++;  }

	my $link = getLinkId($NUCT[$i], $PKEY);
	if (index($PROT[$i], "-") == -1)
	{
	    # 2 = GB
	    if(dbxref->executeInsert('2', $PROT[$i], $PKEY, $link, 'DR', '2') == 1)
	    {   $countEMBL++;    }
	    else
	    {   $countNotIns++;  }
	}
    }

    my @keys = ();

# Process pfam id's.
        @keys = keys(%pfam);
        for (my $i = 0; $i<scalar(@keys); $i++)
	{
#	    print("\t\tpfam: ". $keys[$i] . "\n");
#	    print("\t\t" . $SPTR . "\tPFAM:" . $keys[$i] . "\n");
	    # 3 = PFAM
	    if(dbxref->executeInsert('3', $keys[$i], $PKEY, '0', 'DR', '0')==1)
	    {   $countPFAM++;   }
	    else
	    {   $countNotIns++;  }
	}
        @keys = ();

# Process interpro id's.
        @keys = keys(%interpro);
        for (my $i = 0; $i<scalar(@keys); $i++)
	{
#	    print("\t\tinterpro: ". $keys[$i] . "\n");
	    # 4 = INTERPRO
	    if (dbxref->executeInsert('4', $keys[$i], $PKEY, '0', 'DR', '0')==1)
	    {   $countINTERPRO++;   }
	    else
	    {   $countNotIns++;  }
	}

# Process mgd id's.
        @keys = ();
        @keys = keys(%mgd);
        for (my $i = 0; $i<scalar(@keys); $i++)
	{
#	    print("\t\tmgd: ". $keys[$i] . "\n");
	    # 5 = MGI
	    if(dbxref->executeInsert('5', $keys[$i], $PKEY, '0', 'DR', '0') == 1)
	    {   $countMGD++;   }
	    else
	    {   $countNotIns++;  }
	}

# Process sgd id's.
        @keys = ();
        @keys = keys(%sgd);
        for (my $i = 0; $i<scalar(@keys); $i++)
	{
#	    print("\t\tsgd: ". $keys[$i] . "\n");
	    # 6 = SGD
	    if(dbxref->executeInsert('6', $keys[$i], $PKEY, '0', 'DR', '0')==1)
	    {   $countSGD++;   }
	    else
	    {   $countNotIns++;  }
	}
	
# Process smart id's.
        @keys = ();
        @keys = keys(%smart);
        for (my $i = 0; $i<scalar(@keys); $i++)
	{
#	    print("\t\tsmart: ". $keys[$i] . "\n");
	    # 7 = SMART
	    if(dbxref->executeInsert('7', $keys[$i], $PKEY, '0', 'DR', '0') == 1)
            {   $countSMART++;   }
	    else
	    {   $countNotIns++;  }
	}

# Process zfin id's.
        @keys = ();
        @keys = keys(%zfin);
        for (my $i = 0; $i<scalar(@keys); $i++)
	{
#	    print("\t\tzfin: ". $keys[$i] . "\n");
	    # 8 = ZFIN
	    if(dbxref->executeInsert('8', $keys[$i], $PKEY, '0', 'DR', '0') == 1)
	    {   $countZFIN++;   }
	    else
	    {   $countNotIns++;  }
	}

# Process fb id's.
        @keys = ();
        @keys = keys(%fb);
        for (my $i = 0; $i<scalar(@keys); $i++)
	{
#	    print("\t\tfb: ". $keys[$i] . "\n");
	    # 8 = FB
	    if(dbxref->executeInsert('9', $keys[$i], $PKEY, '0', 'DR', '0') == 1)
	    {   $countFB++;   }
	    else
	    {   $countNotIns++;  }
	}

# Process wb id's.
        @keys = ();
        @keys = keys(%wb);
        for (my $i = 0; $i<scalar(@keys); $i++)
	{
#	    print("\t\tfb: ". $keys[$i] . "\n");
	    # 13 = WB
	    if(dbxref->executeInsert('13', $keys[$i], $PKEY, '0', 'DR', '0') == 1)
            {   $countWB++;   }
            else
	    {   $countNotIns++;  }
	}

# Process mim id's
        @keys = ();
        @keys = keys(%mim);
        for (my $i = 0; $i<scalar(@keys); $i++)
	{
#	    print("\t\tmim: ". $keys[$i] . "\n");
	    # 30 = OMIM
	    if(dbxref->executeInsert('30', $keys[$i], $PKEY, '0', 'DR', '0') ==1)
	    {   $countMIM++;   }
	    else
	    {   $countNotIns++;  }
	}

# Process hugo id's
        @keys = ();
        @keys = keys(%hugo);
        for (my $i = 0; $i<scalar(@keys); $i++)
	{
#	    print("\t\tmim: ". $keys[$i] . "\n");
	    # 29 = HUGO
	    if(dbxref->executeInsert('29', $keys[$i], $PKEY, '0', 'DR', '0') ==1)
	    {   $countHUGO++;   }
	    else
	    {   $countNotIns++;  }
	}

# Process pir id's.
        @keys = ();
        @keys = keys(%pir);
        for (my $i = 0; $i<scalar(@keys); $i++)
	{
#	    print("\t\tpir: ". $keys[$i] . "\n");
	    # 31 = PIR
	    if(dbxref->executeInsert('31', $keys[$i], $PKEY, '0', 'DR', '0') == 1)
	    {   $countPIR++;   }
	    else
	    {   $countNotIns++;  }
	}

# Process tigr_cmr id's.
        @keys = ();
        @keys = keys(%tigr);
        for (my $i = 0; $i<scalar(@keys); $i++)
	{
#	    print("\t\ttigr: ". $keys[$i] . "\n");
	    # 22 = TIGR
	    if(dbxref->executeInsert('22', $keys[$i], $PKEY, '0', 'DR', '0') == 1)
	    {   $countTIGR++;   }
	    else
	    {   $countNotIns++;  }
	}

# Process spombe id's.
        @keys = ();
        @keys = keys(%spombe);
        for (my $i = 0; $i<scalar(@keys); $i++)
	{
#	    print("\t\tspombe: ". $keys[$i] . "\n");
	    # 17 = spombe
	    if(dbxref->executeInsert('17', $keys[$i], $PKEY, '0', 'DR', '0') == 1)
	    {   $countSPOMBE++;   }
	    else
	    {   $countNotIns++;  }
	}
	# Clean Hash.
	%accession = ();
        @NUCT = ();
        @PROT = ();
        %pfam = ();
        %interpro = ();
        %mgd = ();
        %sgd = ();
        %zfin = ();
        %smart = ();
        %fb = ();
        %wb = ();
        %mim = ();
        %hugo =();
        %pir = ();
        %tigr = ();
        %spombe = ();
        $PKEY = "";
        @SPTRList = ();
        @IDList = ();
        @EMBL = ();

# =================  CLUSTERING ================================
    if ( $cluster eq 'T')
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
my $t2 = time();

$printResult = $printResult .$file . "_NODE:\t$node\n";
$printResult = $printResult .$file . "_FILE2PARSE:\t$file2parse\n";
$printResult = $printResult .$file . "_NotIns:\t" . $countNotIns    . "\n";

$printResult = $printResult .$file . "_SPTR:\t" . $countSPTR      . "\n";
$printResult = $printResult .$file . "_SPTRsec\t" . $countSPTRsec   . "\n";
$printResult = $printResult .$file . "_EMBL:\t" . $countEMBL      . "\n";
$printResult = $printResult .$file . "_PFAM:\t" . $countPFAM      . "\n";
$printResult = $printResult .$file . "_INTERPRO:\t" . $countINTERPRO  . "\n";
$printResult = $printResult .$file . "_MGD:\t" . $countMGD       . "\n";
$printResult = $printResult .$file . "_SGD:\t" . $countSGD       . "\n";
$printResult = $printResult .$file . "_ZFIN:\t" . $countZFIN      . "\n";
$printResult = $printResult .$file . "_SMART:\t" . $countSMART     . "\n";
$printResult = $printResult .$file . "_FB:\t" . $countFB        . "\n";
$printResult = $printResult .$file . "_WB:\t" . $countWB        . "\n";
$printResult = $printResult .$file . "_MIM:\t" . $countMIM       . "\n";
$printResult = $printResult .$file . "_HUGO:\t" . $countHUGO      . "\n";
$printResult = $printResult .$file . "_PIR:\t" . $countPIR       . "\n";
$printResult = $printResult .$file . "_TIGR:\t" . $countTIGR       . "\n";
$printResult = $printResult .$file . "_SPOMBE:\t" . $countSPOMBE       . "\n";
my $total = $countSPTR + $countSPTRsec + $countEMBL + $countPFAM + $countINTERPRO +
            $countMGD + $countSGD + $countZFIN + $countSMART + $countFB + $countWB +
            $countMIM + $countHUGO + $countPIR + $countTIGR + $countSPOMBE;
$printResult = $printResult .$file . "_TOTAL:\t" . $total          . "\n";
$printResult = $printResult .$file. "_$node"."_TIME:\t" . ($t2-$t1) ."\n";
$printResult = $printResult . "Execution time:\t" . ($t2-$t1)/60 ." min.\n";
$printResult = $printResult . "\n================================================================================\n";
print($printResult);


exit(0);



# Function grapEMBL() process the line "DR   EMBL; " from SP or TR file.
# Argument: arg1: the complite "DR   EMBL" line from SP or TR file
# Return Value: No return value.
#               The values will be part of the global hash table %accession.
sub grapEMBL($, $, $)
{
    my $emblid = substr($_[0], length("DR   EMBL; "), length($_[0]));

    my @record = split(/; /, $emblid);

    my $i = 1;  # this is the protein.
#    printf("i0: " . $record[0] . "\n");
#    printf("i1: " . $record[1] . "\n");


    push(@{$_[1]}, $record[0]);

    if ( index($record[1], ".", 0) != -1)
    {
        push(@{$_[2]}, substr($record[1], 0, index($record[1], ".", 0)));
    }
    else
    {
        push(@{$_[2]}, $record[1]);
    }
}

# Function grapPfam() process the line "DR   Pfam; " from SP or TR file.
# Argument: arg1: the complite "DR   Pfam" line from SP or TR file
# Return Value: No return value.
#               The values will be part of the global hash table %pfam.
sub grapPfam($, $)
{
#    $PFAMID = substr($_[0], length("DR   Pfam; "));
#    $PFAMID = substr($PFAMID, 0, index($PFAMID, ";"));
    $_[0] =~ /(DR   Pfam;)(\s+)(\S+)(;)/;
    my $PFAMID = $3;

#    if ( not defined($pfam{$PFAMID} ))
    if ( not defined($_[1]{$PFAMID} ))
    {
#        $pfam{$PFAMID} = 1;
      $_[1]{$PFAMID} = 1;
    }
}


# Function grapInterPro() process the line "DR   InterPro; " from SP or TR file.
# Argument: arg1: the complite "DR   InterPro;" line from SP or TR file
# Return Value: No return value.
#               The values will be part of the global hash table %interpro.
sub grapInterPro($, $)
{
    my $INTERPROID = substr($_[0], length("DR   InterPro; "));
    $INTERPROID = substr($INTERPROID, 0, index($INTERPROID, ";"));

    if ( not defined($_[1]{$INTERPROID} ))
    {
        $_[1]{$INTERPROID} = 1;
    }
}

# Function grapMGD() process the line "DR   MGD; " from SP or TR file.
# Argument: arg1: the complite "DR   MGD;" line from SP or TR file
# Return Value: No return value.
#               The values will be part of the global hash table %mgd.
sub grapMGD($, $)
{
    my $MGDID = substr($_[0], length("DR   MGD; "));
    $MGDID = substr($MGDID, 0, index($MGDID, ";"));

    if ( not defined($_[1]{$MGDID} ))
    {
        $_[1]{$MGDID} = 1;
	$countMGD++;
    }
}


# Function grapSGD() process the line "DR   SGD; " from SP or TR file.
# Argument: arg1: the complite "DR   SGD;" line from SP or TR file
# Return Value: No return value.
#               The values will be part of the global hash table %sgd.
sub grapSGD($, $)
{
    my $SGDID = substr($_[0], length("DR   SGD; "));
    $SGDID = substr($SGDID, 0, index($SGDID, ";"));

    if ( not defined($_[1]{$SGDID} ))
    {
        $_[1]{$SGDID} = 1;
    }
}


# Function grapZFIN() process the line "ZFIN; " from SP or TR file.
# Argument: arg1: the complite "ZFIN;" line from SP or TR file
# Return Value: No return value.
#               The values will be part of the global hash table %zfin.
sub grapZFIN($, $)
{
    my $ZFIN = substr($_[0], length("DR   ZFIN; "));
    $ZFIN = substr($ZFIN, 0, index($ZFIN, ";"));

    if ( not defined($_[1]{$ZFIN} ))
    {
        $_[1]{$ZFIN} = 1;
    }
}


# Function grapFB() process the line "FlayBase; " from SP or TR file.
# Argument: arg1: the complite "FlyBase;" line from SP or TR file
# Return Value: No return value.
#               The values will be part of the global hash table %fb.
sub grapFB($, $)
{
    my $FB = substr($_[0], length("DR   FlyBase; "));
    $FB = substr($FB, 0, index($FB, ";"));

    if ( not defined($_[1]{$FB} ))
    {
        $_[1]{$FB} = 1;
    }
}


# Function grapWB() process the line "WormPep; " from SP or TR file.
# Argument: arg1: the complite "WormPep;" line from SP or TR file
# Return Value: No return value.
#               The values will be part of the global hash table %wb.
sub grapWB($, $)
{
    my $WB = substr($_[0], length("DR   WormPep; "), length($_[0]));
    my @record = split(/; /, $WB);
#     printf("i0: " . $record[0] . "\n");
#     printf("i1: " . $record[1] . "\n");

    $WB = substr($record[1], 0, index($record[1], "."));

    if ( not defined($_[1]{$WB} ))
    {
        $_[1]{$WB} = 1;
    }
}
# Function grapSMART() process the line "DR   SMART; " from SP or TR file.
# Argument: arg1: the complite "DR   SGD;" line from SP or TR file
# Return Value: No return value.
#               The values will be part of the global hash table %smart.
sub grapSMART($, $)
{
    my $SMARTID = substr($_[0], length("DR   SMART; "));
    $SMARTID = substr($SMARTID, 0, index($SMARTID, ";"));

    if ( not defined($_[1]{$SMARTID} ))
    {
        $_[1]{$SMARTID} = 1;
    }
}
# Function grapOMIM() process the line "MIM; " from SP or TR file.
# Argument: arg1: the complite "MIM;" line from SP or TR file
# Return Value: No return value.
#               The values will be part of the global hash table %mim
sub grapOMIM($)
{
    # my $MIM = $_[0];
    $_[0] =~ /(DR   MIM;)(\s+)(\S+)(;)/;
    my $MIM = "MIM:" . $3;

    if ( not defined($_[1]{$MIM} ))
    {
        $_[1]{$MIM} = 1;
    }
}

# Function grapHUGO() process the line "Genew; " from SP or TR file.
# Argument: arg1: the complite "Genew;" line from SP or TR file
# Return Value: No return value.
#               The values will be part of the global hash table %hugo
sub grapHUGO($, $)
{
    $_[0] =~ /(DR   Genew;)(\s+)(\S+)(;)/;
    my $HGNC = $3;

    if ( not defined($_[1]{$HGNC} ))
    {
        $_[1]{$HGNC} = 1;
    }
}


# Function grapPIR() process the line "DR   PIR; " from SP or TR file.
# Argument: arg1: the complite "DR   PIR;" line from SP or TR file
# Return Value: No return value.
#               The values will be part of the global hash table %pir.
sub grapPIR($, $)
{
    $_[0] =~ /(DR   PIR;)(\s+)(\S+)(;)/;
    my $PIRID = $3;
#    printf("PIRID: " . $PIRID . "\n");

    if ( not defined($_[1]{$PIRID} ))
    {
        $_[1]{$PIRID} = 1;
    }
}

# Function grapTIGR() process the line "DR   TIGR; " from SP or TR file.
# Argument: arg1: the complite "DR   TIGR;" line from SP or TR file
# Return Value: No return value.
#               The values will be part of the global hash table %tigr.
sub grapTIGR($, $)
{
    $_[0] =~ /(DR   TIGR;)(\s+)(\S+)(;)/;
    my $TIGRID = $3;
#    printf("TIGR: " . $TIGR . "\n");

    if ( not defined($_[1]{$TIGRID} ))
    {
        $_[1]{$TIGRID} = 1;
    }
}


# Function grapTIGR() process the line "DR   SPOMBE; " from SP or TR file.
# Argument: arg1: the complite "DR   GeneDB_Spombe;" line from SP or TR file
# Return Value: No return value.
#               The values will be part of the global hash table %spombe.
sub grapSPOMBE($, $)
{
    $_[0] =~ /(DR   GeneDB_Spombe;)(\s+)(\S+)(;)/;
    my $SPOMBEID = $3;
#    printf("SPOMBE: " . $SPOMBEID . "\n");

    if ( not defined($_[1]{$SPOMBEID} ))
    {
        $_[1]{$SPOMBEID} = 1;
    }
}


# Get the MAX number of id given a Database name.
# Return database id or 0 if not found.
sub getMaxRecordFromTable($, $)
{
    $sth = $dbh->prepare ("select max(" . $_[0] . ") from ". $database . "." . $_[1] );
    $sth->execute();
    if (@ary = $sth->fetchrow_array())
    {
        return ($ary[0]);
    }
    else
    {
        return 0;
    }
}

# Get the id of the Nucleotide for a given record_id and parent_id.
# Arg1: record_id
# Arg2: parent_id.
# Return: id ( 
sub getLinkId($, $)
{
    if( @ary = dbxref->selectID(2, $_[0], $_[1]))
    {
        return ($ary[0]);
    }
    else
    {
        return 0;
    }
}


sub displayMessage()
{
      my $message = displayNode();
      $message = $message .  "Usage : perl dbxref_sptr_cluster -c -d -o -n -f\n";
      $message = $message . "        or\n";
      $message = $message . "        perl dbxref_LL_others -c N -d\n";
      $message = $message . "\tWhere:\n";
      $message = $message . "\t\t-c Cluster: T = True | F = False\n";
      $message = $message . "\t\t-d Database: XREF_SP | XREF_TR\n";
      $message = $message . "\t\t-o Offset: number of record to skip\n";
      $message = $message . "\t\t-n Number: of records to process\n";
      $message = $message . "\t\t-f Cluster node that will became a Synchronization file \n";
      $message = $message . "\tIf -c = F arguments -o -n -f are optional.\n";
      if ($cluster eq 'T')
      {
	 if ($dummy)
	 {
	    open(FPOUT, ">$dummy");
	    print(FPOUT "dummy file ".$dummy);
	    close(FPOUT);
	 }
      }
      $message = $message . "5 input arguments are needed: Cluster DataBase Offset Quantity SyncFileName\n";
      $message = $message . "================================================================================\n";

      die($message);
}

sub displayNode()
{
   if ($cluster eq 'T')
   {
      if ($dummy)
      {
	 return "Node: $dummy\n";
      }
   }
}

sub displayDieMessage($, $)
{
    my $message = displayNode();
    $message = $message . " can't open given file $_[0] $_[1]\n";
    $message = $message . "================================================================================\n";
    die($message);
}
