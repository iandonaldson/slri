# parse LL_tmpl file to get mapping from LL to any other database.
# ftp://ftp.ncbi.nih.gov/refseq/LocusLink/LL_tmpl
#
# Two options are available when running this parser.
#  > goa_parser_ll LL clean
#    Where LL   : Locus Link Abbreviation.
#          clean: Option to delete all records in goa_association for LL.
# or
#  > goa_parser_ll  GOA_LL 
#    Where GOA_LL is the tag under dbxref.ini where the location and name of the
#    to parse (LL_tmpl).



# two choices are available - to use DBD-mysql or to use DBI to connect to
# MySQL-db.
# DBI is a most universal one (DBD-mysql is too specific)
use DBI;
use SeqHound;
use dbxref;

# RAC Read this parameters from a propertie files.

my $user_name = dbxref->getProperty("dbxref.ini", "user");
my $password  = dbxref->getProperty("dbxref.ini", "password");
my $host      = dbxref->getProperty("dbxref.ini", "host");
my $port      = dbxref->getProperty("dbxref.ini", "port"); 
my $database  = dbxref->getProperty("dbxref.ini", "database");
my $table     = dbxref->getProperty("dbxref.ini", "table");

my $dsn = "DBI:mysql:" . $database . ":" . $host . ":" . $port;

my $dbh, $sth;
my @ary;

# 2) connect to MySQL-database
$dbh = DBI->connect ($dsn, $user_name, $password, {RaiseError => 1});

my $t1 = time();


# Create a HasTable for the db abbreviations and Ids.
# %abbr_id HasTable of Key: Abbreviation, Value: Ids.
# This hash table was created to avoid searching into goa_xdb each time
# an ID is needed from a particular abbreviation name.
##########################
createAbbrHashTable();


my $abbr = $ARGV[0];


my $dbAbbrId = $hash_abbr{$abbr}; 

if ($ARGV[1] eq 'clean')
{
    printf("dbAbbrId" . $dbAbbrId . "\n");
    print "clean up DATABASEID = $dbAbbrId \t $abbr  ... \n";
    dbxref->executeDeleteGOA($dbh, $dbAbbrId);
    exit(0);
}


my $file2process = dbxref->getProperty("dbxref.ini", $ARGV[0]);

#This parser will parse only LL (Locus Link file LL_Tmpl).
$abbr = "LocusID";

$dbAbbrId = $hash_abbr{$abbr};


my $pmidabbrid = $hash_abbr{'PMID'};
#die("END TEST\n");

open(FP, $file2process) or die "can't open given file: $!";

$count = 0;                             # Count the number of records
$countEMBL = 0;
$countPFAM = 0;
$countNotFound = 0;
while ($line = <FP>)
{
#    print($line);

    $last = "";
    chomp($line);

    @embl;
    @pfam;
    @SMART;
    @ENSEMBL;
    @EMBL;
    @SPTR;
    @record;
    %prot;
    %acc;

    my $ID = $line;
    my $LL = "";
    my $TAXID = "";
    my $LINK = "";
    my $SeqDbxrefId = 0;
    my $SYM = "";
    while ($line = <FP>)
    {
        my $ECODE = "";
	my $GO = "";
	my @PMID = ();

        if (">>" eq substr($line, 0, 2))
        {
	   #print("//\n");
	    last;
	}

	chomp($line);
#	print($line . "\n");

#	print("line: " . $line . "\n");
	if ("LOCUSID: " eq substr($ID, 0, length("LOCUSID: ")))
	{
	    # Do I need to instert this LL? or 
	    # Insert only the ones that have GO Terms ?
	    # What about OMIN and CDD.
	    $LL = substr($ID, length("LOCUSID: "));
#	    print("LL: " . $LL . "\n");	
	    #48 = LocusID
	    $sth = $dbh->prepare("INSERT INTO " . $database .
				 ".goa_seq_dbxref VALUES(0, ". $dbAbbrId . ", '" . $LL . "', " 
				 . 0 . ", " . 0 . ", 'gene', '', '', '')");
	    $sth->execute();
	    $SeqDbxrefId = getMaxRecordsFromTable("id", "goa_seq_dbxref");
#	    print("SeqDbxrefId: " . $SeqDbxrefId . "\n");
	    my $retid = getMaxRecordsFromTable("id", "goa_seq_dbxref");
	}
	elsif ("MAP:" eq substr($line, 0, length("MAP:")))
	{
	    my $MAP = substr($line, length("MAP: "));
	    my @symbol = split(/\|/, $line);

	    $symbol[0] =~ /(\S+): (\S+)/;
	    $SYM = $2;
#	    print("symbol: " . $symbol[0] . "\n");
#	    print("SYM   : " . $SYM . "\n");
	    $sth = $dbh->prepare("UPDATE goa_seq_dbxref set symbol = '" . $SYM . "' WHERE id = " . $SeqDbxrefId . ";");
	    $sth->execute();
	}
	elsif ("LINK:" eq substr($line, 0, length("LINK:")))
	{
	    $LINK = $line;
	    $LINK =~ /(\S+)TAXID=(\d+)/;

	    if ($2 > 0)
	    {
#	        print("\t\t2: " . $2 . "\n");
	        $TAXID = $2;
#		print("\t\tTAXID: " . $TAXID . "\n");
		$sth = $dbh->prepare("UPDATE goa_seq_dbxref set taxid_1 = " . $TAXID . " WHERE id = " . $SeqDbxrefId . ";");
		$sth->execute();
	    }
	}
	elsif ("GO:" eq substr($line, 0, length("GO:")))
	{

	    my @goList = split(/\|/, $line);

	    # Get Evidence Code.
	    $ECODE = $goList[2];

	    # Get GO ID.
	    $goList[3] =~ /(\S+):(\d+)/;
	    $GO = $2;
	
	    # Get PMID
	    if( $goList[5] ne "na")
	    {
		@PMID = split(/\,/, $goList[5]);
	    }

#	    print("\t\tecode: " . $ECODE . "\n");
#	    print("\t\tgo   : " . $GO    . "\n");
#	    print("\t\tpmid : " . $PMID  . "\n");	
	
	    $sth = $dbh->prepare ("INSERT IGNORE INTO " . $database . ".goa_association " .
                                  "VALUES(0, 'F', " . $GO . ", " .
				  $SeqDbxrefId . ", " . $dbAbbrId . ", '" .
				  $ECODE . "', " .
				  "'0')");
  	    my $success = $sth->execute();
	    if( $success <= 0 )
	    {
	        #	  print "$goa_seq_dbxref_id, $goid, $evidence already in database\n";
	        next;
	    }	
		
	    my $goa_association_id = getMaxRecordsFromTable("id", "goa_association");
	
	    foreach(@PMID)
	    {
#	      print($_ . "\t");
	        if($_ > 0)
		{
		    # 66 = PMID
		    $sth = $dbh->prepare ("INSERT INTO " . $database . ".goa_reference VALUES(0, " . $goa_association_id . ", " . $pmidabbrid . ", '" . $_. "')");
		    $sth->execute();
		}
	    }
#	    print("\n");

      }
	

	$ID = $line;
    }
    $count++;


#    print(">>\n");
}

close(FP);


#$sth->finish();
#$dbh->disconnect();
my $t2 = time();

print($count . " records Updated into goa_xdf database.\n");
print($countNotFound . " records not Found in SeqHound. \n");
print("Execution time : " . ($t2-$t1) . " sec.\n\n");
if( $t2-$t1 > 60)
{
    print("                 " . ($t2-$t1)/60 ." min.\n");
}


exit(0); 





# Get the MAX number of id given a Database name.
# Return database id or -1 if not found.
sub getMaxRecordsFromTable($, $)
{
    $sth = $dbh->prepare ("select max(" . $_[0] . ") from " . $_[1] );
    $sth->execute();
    if (@ary = $sth->fetchrow_array())
    {
        return ($ary[0]);
    }
    else
    {
        return -1;
    }
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

    print($row[0] ."\t" . $row[1] . "\n");

    #Crete an abreviation id hash.
    $hash_abbr{unpublished} = 0;

    while (@row = $sth->fetchrow_array)
    {
#        print($row[0] ."\t" . $row[1] . "\n");
        $hash_abbr{$row[0]} = $row[1];
    }
}
