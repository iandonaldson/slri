#! /usr/bin.perl
# $Id: dbxref_DBs_SPTR_cluster.pl,v 1.3 2005/03/29 20:15:23 rcavero Exp $
#--------------------------------------------------------------------------
# parser for DB - SWISSPROT cross-referenece file
# DB are : 1 spombe, 2 gramene 3 tair 4 tigr_cmr 5 unigene 6 vida
# 
# the files come from: ftp://ftp.geneontology.org/pub/go/gp2protein/
# The input file format(tab-delimited):  
#   column 1:  GeneDB_Spombe Db_Object_Id
#   Column 2: Swissprot id
#--------------------------------------------------------------------------
use strict;
use DBI;
use dbxref;
use Getopt::Long;
use IO::Handle;

# mysql set up stuff
my $host      = dbxref->getProperty("dbxref.ini", "temp_host");
my $port      = dbxref->getProperty("dbxref.ini", "temp_port");
my $database  = dbxref->getProperty("dbxref.ini", "temp_database");
my $table     = dbxref->getProperty("dbxref.ini", "temp_table");
my $user_name = dbxref->getProperty("dbxref.ini", "temp_user");
my $password  = dbxref->getProperty("dbxref.ini", "temp_password");
my $dsn = "DBI:mysql:$database:$host:$port";

#  connect to MySQL-database
my $dbh = DBI->connect ($dsn, $user_name, $password, {RaiseError => 1});
# use functions in dbxref.pm to prepare for the update

dbxref->prepareID($dbh, $database, $table);
dbxref->prepareInsert($dbh, $database, $table);

# get source db hash table 
my %dbtable = dbxref->createDbHashTable($dbh, $database);
my $SPTR=$dbtable{SPTR}->source_id();

my ($infile, $current_db, $format);
my $organism;

Getopt::Long::Configure ('bundling');

# =================  CLUSTERING ================================

    my $cluster;
    my $offset;
    my $count2parse;
    my $dummy = '';

    my $retValue = 0;
    $retValue = GetOptions('d=s' => \$organism, 'c=s' => \$cluster, 'o=i' => \$offset,'n=i' => \$count2parse, 'f=s' => \$dummy );

    if( ($cluster ne 'T') and ($cluster ne 'F') )
    {
       dbxref->displayMessage('dbxref_DBs_SPTR_cluster');
    }
    if ($cluster eq 'T')
    {
       if ( (! $offset) and (! $count2parse) )
       {
          displayMessage('dbxref_DBs_SPTR_cluster');
       }
    }
# =============================================================


my $data_dir = dbxref->getProperty("dbxref.ini", "data_dir");


# parse dbxref file for individual organism, one at a time
if($organism eq "spombe")
{
	# 1. parse spombe file
	$infile = dbxref->getProperty("dbxref.ini", "XREF_GENEDB_SPOMBE");
	$current_db = $dbtable{GENEDB_SPOMBE}->source_id();
	$format = "^(SP\\S+)\\t(?:SPTR:)+([OPQ]\\d\\w\\w\\w\\d)";
	parse_DB($infile, $current_db, $format);
}
elsif($organism eq "gramene")
{
	# 2. parse gramene file
	$infile = dbxref->getProperty("dbxref.ini", "XREF_GRAMENE");
	$current_db = $dbtable{GR}->source_id();
	$format = "^GR:(\\S+)\\t(?:SWALL:)+([OPQ]\\d\\w\\w\\w\\d)";
	parse_DB($infile, $current_db, $format);
}
elsif($organism eq "tair")
{
	#3 . parse tair file
	$infile = dbxref->getProperty("dbxref.ini", "XREF_TAIR");
	$current_db = $dbtable{TAIR}->source_id();
	$format = "^TAIR:(\\S+)\\t(?:Swiss-Prot:)+([OPQ]\\d\\w\\w\\w\\d)";
	parse_DB($infile, $current_db, $format);
}
elsif($organism eq "tigr_cmr")
{
	# 4. tigr_cmr file
	$infile = dbxref->getProperty("dbxref.ini", "XREF_TIGR_CMR");
	$current_db = $dbtable{TIGR_CMR}->source_id();
	$format = "^TIGR_CMR:(\\S+)\\t(?:SWISS-PROT:|SPTR:)+([OPQ]\\d\\w\\w\\w\\d)";
	parse_DB($infile, $current_db, $format);
}
elsif($organism eq "unigene")
{
	# 5. parse unigene file
	$infile = dbxref->getProperty("dbxref.ini", "XREF_UNIGENE");
	$current_db = $dbtable{UNIGENE}->source_id();
	$format = "^UniGene:(\\S+)\\t(?:UniProt:)+([OPQ]\\d\\w\\w\\w\\d)";
	parse_DB($infile, $current_db, $format);
}
elsif($organism eq "vida")
{
	#6. parse vida file
	$infile = dbxref->getProperty("dbxref.ini", "XREF_VIDA");
	$current_db = $dbtable{VIDA}->source_id();
	$format = "^VIDA:(\\S+)\\t(?:Swiss-Prot:|TrEMBL:)+([OPQ]\\d\\w\\w\\w\\d)";
	parse_DB($infile, $current_db, $format);
}

dbxref->finishSelectID();
dbxref->finishInsert();
$dbh->disconnect();

exit(0);

sub parse_DB
{
#    my $SPTR = $dbtable{SPTR}->source_id();
#    print(RESULT "SPTR: $SPTR\n");
    my $infile=$_[0];   # filename
    my $sourcedb_id = $_[1];     # db id
    my $regex = $_[2];     # line format

    #dbxref->executeDelete($dbh, $database, $table, $sourcedb_id);
    # die ("End test executeDelete()\n");

    $infile = $data_dir . $infile;

    open(FP, $infile) or die "can't open given file $infile: $!\n";

    my $t1 = time();

    my $n=0;           # keep track of line number
    my $count = 0;     # Count the number of records
    my $countSPTR = 0;
    my $countOrg = 0;
    my $err=0;

    my @A = <FP>;
    my $total;		# total number  of lines to process
    if ($cluster eq 'T') { $total = $count2parse; }
    else { $total = $#A; }

    close (FP);
    open (FP, $infile) or die "can't open given file $infile: $!\n";

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

    my $step = int($total/100);
    my $percent =0;     #the percentage of line been processed
#    print ERR "== $infile == total lines: $total\n";
#    print RESULT "Parse file: $infile == \nTotal lines: $total\n";
    #print "total lines: $total \none percent : $step \n";
    my $line;
    while (<FP>)
    {
        $line = $_;
        chomp($line);
        $n++;

        next if $line=~/(^$)|(^!)/;

        if( $step> 0 and  $n%$step == 0)
        {
            $percent++;
           # print "\b\b\b$percent";
        }

        # check and insert record
        # sample line; SPAC644.07	SPTR:Q9P6Q3
        if ($line =~ /$regex/)
        {
	    my $db_object_id = $1; # DB_Object_Id of current db
	    my $SPTR_id = $2; # swiss_prot id
            my $parent_id;

            # to decide parent_id , check if current swiss_prot id exists as a top level record
            my @rowid = dbxref->selectID($SPTR, $SPTR_id, 0);
            if (!@rowid) # new GO record, create a top level id first
            {
                # parent_id =0
                if(dbxref->executeInsert($SPTR, $SPTR_id , 0, 0, 'Col 2', 0))
		{
		   $countSPTR++;
		}
                #get id for above new record
                my @rowid2 = dbxref->selectID($SPTR, $SPTR_id, 0);
                if (length($rowid2[0]) != 0)
	            {
	                $parent_id = $rowid2[0];
	            }
                else
                {
                   print "line $n ( insertion error): $line\n";
                   print "== ", scalar localtime, " ==: die ...\n"; ; 
                   die;
                }
            }
            else #  This SPTR_id already exists as a top level record
            {
                $parent_id = $rowid[0];
            }

            # and then add the swissprot_id to go_xref as the child of this Go_id
            if (dbxref->executeInsert($sourcedb_id, $db_object_id, $parent_id, 0, 'Col 1', 0 ))
	    {
	       $countOrg++;
	    }
	}	
        else #format is incorrect:example: SPAC19B12.13	SPTR:CAD89537
        {
            print "line $n: $line\n";
            $err++;
        }
# =================  CLUSTERING ================================
        if ($cluster eq 'T')
        {
   	    if ($n == $count2parse)
	    {
	       last;
	    }
        }
# ===============================================================
    }#endwhile

    close(FP);
    #print "="x10,"[ ", scalar localtime, " ]", "="x10, "\n";

# =================  CLUSTERING ================================
    # Generate a dummy file for synchronization.
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
    $printResult = $printResult . "DB2SPTR_NODE:\t$node\n";
    $printResult = $printResult . "DB2SPTR_FILE2PARSE:\t$infile\n";
    $printResult = $printResult . "DB2SPTR_SPTR:\t$countSPTR\n";

    if ($organism eq "spombe") {
        $printResult = $printResult . "DB2SPTR_SPOMBE:\t";
    } elsif ($organism eq "gramene") {
        $printResult = $printResult . "DB2SPTR_GRAMENE:\t";
    } elsif ($organism eq "tair") {
        $printResult = $printResult . "DB2SPTR_TAIR:\t";
    } elsif ($organism eq "tigr_cmr") {
        $printResult = $printResult . "DB2SPTR_TIGR_CMR:\t";
    } elsif ($organism eq "unigene") {
        $printResult = $printResult . "DB2SPTR_UNIGENE:\t";
    } elsif ($organism eq "vida") {
        $printResult = $printResult . "DB2SPTR_VIDA:\t";
    } 
    $printResult .= "$countOrg\n";

    my $countTotal = $countSPTR + $countOrg;
    $printResult = $printResult . "DB2SPTR_$node"."_TOTAL:\t$countTotal\n";
    $printResult = $printResult . "DB2SPTR_$node"."_TIME:\t" . ($t2-$t1) ."\n";
    $printResult = $printResult . "Execution time:\t" . ($t2-$t1)/60 ." min.\n";
    $printResult = $printResult . "\n================================================================================\n";
    print("$printResult\n");

#    print("\b\b\b$err error lines!\n$count records Updated into dbxdf database.\n");

}


# $Log: dbxref_DBs_SPTR_cluster.pl,v $
# Revision 1.3  2005/03/29 20:15:23  rcavero
# dbxref_DBs_SPTR_cluster.pl replace dbxref_DBs_SPTR.pl
#
# Revision 1.1  2005/01/27 21:29:51  zwang
# Make compliant with parallel computing.
#
# Revision 1.9  2004/09/29 15:33:55  rcavero
# Fix Fix spelling mistake in variable $SPRT instead of $SPTR
# remove Porcentage Counter.
#
# Revision 1.8  2004/09/28 20:56:09  kzheng
# use swiss_prot id as parent instead of organism db_object_id
#

