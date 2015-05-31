#use strict;

# Parser for goa_association files.ftp://ftp.geneontology.org/pub/go/gene-associations/
# Arguments:
#           ARGV[0] Tag from dbxref.ini that specify the file name.
#                   or 'clean'
#           ARGV[1] if ARGV[0] is 'clean' then ARGV[1] will be the Database Abbreviation from goa_xdb.
# test scipt 
# 1) connects to MySQL-database
# 2) processes accession number from file
# 3) extracts GO-annotations that belong to a given acc-number
# 4) spits out the result
# 5) disconnect

# two choises are available - to use DBD-mysql or to use DBI to connect to
# MySQL-db.
# DBI is a most universal one (DBD-mysql is too specific)
use strict;
use DBI;
use Getopt::Long;
use dbxref;

my $gNumOfFields = 15;

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
      dbxref->displayMessage('goa_parser_cluster.pl');
}
if ($cluster eq 'T')
{
   if ( (! $offset) and (! $count2parse) )
   {
      dbxref->displayMessage('goa_parser_cluster.pl');
   }
}
if (length($db) <= 0)
{
    dbxref->displayMessage('goa_parser_cluster.pl');
}

# =============================================================

my $user_name = dbxref->getProperty("dbxref.ini", "temp_user");
my $password  = dbxref->getProperty("dbxref.ini", "temp_password");
my $host      = dbxref->getProperty("dbxref.ini", "temp_host");
my $port      = dbxref->getProperty("dbxref.ini", "temp_port");
my $database  = dbxref->getProperty("dbxref.ini", "temp_database");

my $data_dir  = dbxref->getProperty("dbxref.ini", "data_dir");

my $file2parse = $data_dir . dbxref->getProperty("dbxref.ini", $db);

my $dbname_goa_seq_dbxref = dbxref->getProperty("dbxref.ini", "temp_goa_seq_dbxref");
my $dbname_goa_association= dbxref->getProperty("dbxref.ini", "temp_goa_association");
my $dbname_goa_reference  = dbxref->getProperty("dbxref.ini", "temp_goa_reference");
my $dbname_goa_with       = dbxref->getProperty("dbxref.ini", "temp_goa_with");


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

my %hash_abbr=();

createAbbrHashTable();

my $pdbAbbrId = $hash_abbr{"PDB"};
#print ("PDB: " .  $pdbAbbrId . "\n");

#
# Strip out prefix GOA_ from argument, to get the abbreviation name
# of the file to begin processing.


#print "argv[0]= $ARGV[0]  ;  argv[1]=$ARGV[1]\n";
my $abbr = $db;

my $sthInsGoaAss;
prepareInsertGoaAssociation($dbh, $database);

my $sthInsGoaRef;
prepareInsertGoaReference($dbh, $database);

my $sthInsGoaSeqDbx;
prepareInsertGoaSeqDbx($dbh, $database);

my $sthInsGoaWith;
prepareInsertGoaWith($dbh, $database);

#Check File integrity.
##########################
# -Count number of files per line (14)or(15) tab delimited.
#checkFileIntegrity($file2parse);
#die("End CheckFileIntegrity\n");

# 1) process GO.xrf_abbs Cross Reference database.

# RAC Read the file name from command line.

if(open(FP, $file2parse))
{
  # TODO: generate Dummy file.
  print("File OK\n");
}
else
{
    die "can't open given file: $!";
}
#or
#die "can't open given file: $!";

my $gMessage;                              # Global message to return Errors.
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
#    print($line);
    $count++;
    if ("!" ne substr($line, 0, 1))
    {
        chomp($line);
	my @record = split(/\t/, $line);
	my $db               = $record[0];     #    print("db: "                . $record[0] . "\n");
	my $db_object_id     = $record[1];     #    print("db_object_id: "      . $record[1] . "\n");
	my $db_object_symbol = $record[2];     #    print("db_object_symbol: "  . $record[2] . "\n");
	my $db_not           = $record[3];     #    print("not: "               . $record[3] . "\n");
	my $goid             = $record[4];     #    print("goid: "              . $record[4] . "\n");
	my $db_reference     = $record[5];     #    print("db_reference: "      . $record[5] . "\n");
	my $evidence         = $record[6];     #    print("evidence: "          . $record[6] . "\n");
	my $with_from        = $record[7];     #    print("with_from: "         . $record[7] . "\n");
	my $aspect           = $record[8];     #    print("aspect: "            . $record[8] . "\n");
	my $db_object_name   = $record[9];     #    print("db_object_name: "    . $record[9] . "\n");
	my $synonym          = $record[10];    #    print("synonym: "           . $record[10]. "\n");
	my $db_object_type   = $record[11];    #    print("db_object_type: "    . $record[11]. "\n");
	my $taxon            = $record[12];    #    print("taxon: "             . $record[12]. "\n");
	my $date             = $record[13];    #    print("date: "              . $record[13]. "\n");
	my $assigned_by      = $record[14];    #    print("assigned_by: "       . $record[14]. "\n");

	#do nothing if we do not have the essential fields (or the line is empty).	
	next if( length( $db ) <= 0 || length( $db_object_id ) <= 0 || 
		length( $goid )	<= 0 || length( $aspect ) <= 0 );

	# Because some records are missing the Reference field, an unpublished reference was created
	# and all record with out reference is givin an "unpublished" refrence.
	# Key:unpublished     value:0.
	if (length($db_reference) == 0)
	{
	    $db_reference = "unpublished:unpublished";
	}

	# Process db
	my $id = getDbNameIdFromHash($db);
        # if db abbreviation doesn't exist.
	if ($id == -1)
	{
	    $errorCount++;
	    print("Could not found db name: " . $db . " abbreviation id into goa_xdb. Line " .$count . "\n");
	    if ($errorCount>5)
	    {
	      # TODO generate a dummy file an Error message and die.
	      # =================  CLUSTERING ================================
	      # Generate a dummy file for sincronization.
	      if ($cluster eq 'T')
	      {    if ($dummy)
		   {     open(FPOUT, ">$dummy");
			 print(FPOUT "dummy file ".$dummy);
			 close(FPOUT);
		   }
	      }
	      # =============================================================
	      generateDummyFile();
	      die("To many db names: " . $db . " abbreviation missing in goa_xdb\n");
	    }
	    next;
	}
	
	# Process db_object_symbol.
	# . If $db_object_symbol contain "'" this need to be encoded to "\'".
	if (index($db_object_symbol, "\'", 0) > 0)
	{
	    $db_object_symbol =~ s/\'/\\\'/g;                            # Change "'" by "\'"
	}
	
	# Process assigned_by If $assigned_by is missing use default $db
	if ($assigned_by eq "")
	{
	    $assigned_by = $db;
	}

	my $assigned_by_id  = getDbNameIdFromHash($assigned_by);

	if ($assigned_by_id == -1)
	  {
	    generateDummyFile();
	    die("Could not found assigned_by name: " . $assigned_by . " abbreviation id into goa_xdb\nLine " .$count . "\n");
	}

	# Process db_not
	if ($db_not eq "")
	{
	    $db_not = "F";
	}
	else
	{
	    $db_not = "T";
	}

	# Process goid
	#$goid = substr($goid, length("go:"));
	$goid =~ /(GO:)(\d+)/;
	$goid = $2;
	#RAC
#	print("GO: " . $goid . "\n");
	if ($goid <= 0)
	{
	  print("Wrong GO: " . $goid . "\n");
	  next;
	}

	#Process with_from
	my $with_from_id = getDbNameIdFromHash(substr($with_from, 0, length("TAIR")));
	my $with_from_key = substr($with_from, length("TAIR")+1);

#   	print("    reference_by_id: " . $reference_by_id . "\n");
#	print("    reference_by_key: " . $reference_by_key . "\n");

#	print("\nEnter\n");
#	print("db: "                . $db               . "\n");
#	print("db_id: "             . $db_id            . "\n");
#	print("db_object_id: "      . $db_object_id     . "\n");
#	print("db_object_symbol: "  . $db_object_symbol . "\n");
#	print("db_not: "            . $db_not           . "\n");
#	print("goid: "              . $goid             . "\n");
#	print("db_reference: "      . $db_reference     . "\n");
#	print("evidence: "          . $evidence         . "\n");
#	print("with_from: "         . $with_from        . "\n");
#	print("aspect: "            . $aspect           . "\n");
#	print("db_object_name: "    . $db_object_name   . "\n");
#	print("synonym: "           . $synonym          . "\n");
#	print("db_object_type: "    . $db_object_type   . "\n");
#	print("taxon: "             . $taxon            . "\n");
#	print("date: "              . $date             . "\n");
#	print("assigned_by: "       . $assigned_by      . "\n");
#	print("assigned_by_id: "    . $assigned_by_id   . "\n");

# }

#print "$count. $db_id, $db_object, $db_object_id, $taxon, $db_object_type, $db_object_symbol\n";
	# Insert Recort into goa_seq_dbxref.
	my $goa_seq_dbxref_id = processGOA_seq_dbxref($id, $db_object_id, $taxon, $db_object_type, $db_object_symbol, $db_object_name, $synonym);


#	print("goa_seq_dbxref_id : " . $goa_seq_dbxref_id . "\n\n");
#######
#	print("\ndb_no: " .  $db_not . "\n goid: ". $goid . "\n goa_seq_dbxref_id: " . $goa_seq_dbxref_id);
#	print(" \n assigned_by_id: " .$assigned_by_id . "\n  evidence: " . $evidence . "\n date: " .$date ."\n");

	
	if($sthInsGoaAss->execute($db_not, $goid, $goa_seq_dbxref_id, $assigned_by_id, $evidence, $date) != 1)
	{
	  # TODO: if accession already exist in database then insert it's refernce and with.
	  #print "$goa_seq_dbxref_id, $goid, $evidence already in database\n";
	  next;
	}
	else
	{   $countASS++;   }
	# TODO this can't be getMaxId need to bet getIDFromGoaAssociation(database, goa_seq_dbxref_id, go_id)
	my $goa_association_id = getMaxIdFromGoaAssociation($database, $goa_seq_dbxref_id, $goid);
#######


#	print("evidence.length()" . length($evidence) . "\n");
#	print("evidence: " . $evidence . "\n");

	#process Reference
	#print("goa_association_id: $goa_association_id\n");
	#print("db_reference: " . $db_reference . "\n");
	processReference($goa_association_id, $db_reference);

	#process WithFrom
	#print("with_from: " . $with_from . "\n");
	processWithFrom($goa_association_id, $with_from);

    } #if()
    # =================  CLUSTERING ================================
    if ($cluster eq 'T')
    {
	   if ($count == $count2parse)
	   {
	      last;
	   }
    }
    # ===============================================================

} #while()

close(FP);

$sth->finish();
$sthInsGoaSeqDbx->finish();
$sthInsGoaAss->finish();
$sthInsGoaRef->finish();
$sthInsGoaWith->finish();

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

$db =~ /(\w+)(_)(\w+)/;
my $tag = $1 . $3;
#print("tag: $tag\n");

# =============================================================
$printResult = $printResult . $tag . "_NODE:\t$node\n";
$printResult = $printResult . $tag . "_FILE2PARSE:\t$file2parse\n";
$printResult = $printResult . $tag . "_SEQDBXREF:\t$countSEQDBX\n";
$printResult = $printResult . $tag . "_ASSOCIATION:\t$countASS\n";
$printResult = $printResult . $tag . "_REFERENCE:\t$countREF\n";
$printResult = $printResult . $tag . "_WITH:\t$countWITH\n";
$printResult = $printResult . $tag . "_$node"."_TOTAL:\t" . ($countSEQDBX+$countASS+$countREF+$countWITH) . "\n";
$printResult = $printResult . $tag . "_$node"."_TIME:\t" . ($t2-$t1) . "\n";
$printResult = $printResult . "Execution time:\t" . ($t2-$t1)/60 . " min.\n";
$printResult = $printResult . "\n================================================================================\n";

print("$printResult\n");

exit(0);




# Get Database id given a Database name.
# Search under seqhound.goa_xdb table.
# Return database id or -1 if not found.
sub getDbNameId($)
{
#    print("dbName: " . $_[0] . "\n");

    $sth = $dbh->prepare ("select id from goa_xdb where abbreviation='" . $_[0] . "'" );
    $sth->execute();
    if (@ary = $sth->fetchrow_array())
    {
#       print("        dbid: " . $ary[0] . "\n");
        return ($ary[0]);
    }
    else
    {
#        print("        dbid: " . $ary[0] . "\n");
        return -1;
    }
}


# Get Database id From Hash.
# Search under hash_abbr hash table that contain Key Values of abbreviations and db_ids..
# Argument: abbreviation.
# Return:   db_id (DataBase_id).
sub getDbNameIdFromHash($)
{
  my $tag = $_[0];

  #Make sure that the case is correct:
  $tag =~ s/UNIPROT/UniProt/;
  $tag =~ s/GENEDB_SPOMBE/GeneDB_Spombe/;
  $tag =~ s/Tair/TAIR/;

  #Make sure that GOC:unpublished is handled correctly
  $tag =~ s/GOC/GOC:unpublished/;

  #If they've used AGRICOLA as an abbreviation, change it to
  #to the correct abbreviation /bib/
  $tag =~ s/AGRICOLA/bib/;

#  print "getDbNameIdFromHash: $tag $hash_abbr{$tag}\n";

    if(exists($hash_abbr{$tag}))
    {
        return $hash_abbr{$tag};
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

#    print($row[0] ."\t" . $row[1] . "\n");

    #Crete an abreviation id hash.
    $hash_abbr{unpublished} = 0;

    while (my @row = $sth->fetchrow_array)
    {
#        print($row[0] ."\t" . $row[1] . "\n");
        $hash_abbr{$row[0]} = $row[1];
    }
}


# Process Reference take a Reference String and the association ID.
# -Split the reference by "|" and for each reference split it by ":" to get the
#  DbName and DbKey.
# -Search into goa_xdb for the id of the DbName.
# -Append record to goa_reference.
# Arguments:
#           arg1: associationId
#           arg2: referenceString.
sub processReference($, $)
{
    my @refRecord = split(/\|/, $_[1]);
    foreach(@refRecord)
    {
       
	# By pass exception for "GOA:". GOA can be of one of the form "interpro", "spec", "spkw".
	# but the name of the DB is GOA.xxxxxxx. For this reason the reference_by_id need to be
        # GOA.xxxxxx.
	#    Here need to be proccesed pipes of References.
        my @ref = ();
        if (substr($_, 0, length("GOA:")) eq "GOA:")
	{
	    $ref[0] = $_;
	    $ref[1] = substr($_, index($_, ":", 0)+1);
	}
	else
	{
	    $ref[0] = substr($_, 0, index($_, ":", 0));
	    $ref[1] = substr($_, index($_, ":", 0)+1);
	}
	my $dbid = getDbNameIdFromHash($ref[0]);
	# Insert the record into goa_reference
	if($sthInsGoaRef->execute($_[0], $dbid, $ref[1]) == 1)
	{
	    $countREF++;
	}
    }
}


# Process WithFrom take an association ID  and a With_From String and.
# -Split the reference by "|" and for each reference split it by ":" to get the
#  DbName and DbKey.
# -Search into goa_xdb for the id of the DbName.
# -Append record to goa_reference.
# Arguments:
#           arg1: associationId
#           arg2: $with_from String.
# Not Implemented Yet:
#                      keyType: dond't know yet.
#                      goa_seq_dbxref_id: don't implemented yet.
#                      insert 0 into the database.
sub processWithFrom($, $)
{
    my @refRecord = split(/\|/, $_[1]);
    foreach(@refRecord)
    {
#        print( $_  . "\n");
        my @ref = ();
        $ref[0] = substr($_, 0, index($_, ":", 0));
	$ref[0] =~ s/\s//;
	$ref[1] = substr($_, index($_, ":", 0)+1);
#	print("   ref: " . $ref[0]  . "\n");
#	print("   key: " . $ref[1]  . "\n");

	# gene_association.mgi file has the "with" field with "InterPro" in lower letters.
	# All DataBase abbreviations need to be in upper Case.
	if ($ref[0] eq "InterPro")
	{
	    $ref[0] = "INTERPRO";
        }
	my $dbid = getDbNameIdFromHash($ref[0]);

	if($sthInsGoaWith->execute($_[0], $dbid, $ref[1], 0, 0) == 1)
	{
	    $countWITH++;
	}
    }
}

# Function processGOA_dbxref, append a Record to seqhound.goa_seq_dbxref.
# Arguments: arg1: goa_xdb_id:
#            arg2: xref_key:
#            arg3: taxid:
#            arg4: type_id
#            arg5: symbol
#            arg6: full_name
#            arg7: synonym
# Return Value: goa_seq_dbxref.id.
sub processGOA_seq_dbxref($, $, $, $, $, $, $)
{
    my $goa_xdb_id = $_[0];
    my $xref_key   = $_[1];
    my $gi         = 0;
    my $accession  = 0;
    my $taxid_1    =  substr($_[2], length("taxon:"));
    my $taxid_2    = 0;
    my $type_id    = $_[3];
    my $symbol     = $_[4];
    my $full_name  = $_[5];
    my $synonym    = $_[6];

#   if we have no taxid, set taxid_1 to 0
	if( length( $taxid_1 ) <= 0 )
	{
	  $taxid_1 = 0;
	}
    # print "processGOA_seq_dbxref: full name: $full_name, synonym: $synonym\n";
    # process full_name.
    # If $full_name contain "'" this need to be encoded to "\'".
    if ( $full_name =~ /\\/)                                # Change "\" by "\\"
    {
     	 $full_name =~ s/\\/\\\\/g;
    }
    if ( $full_name =~ /\'/)                                # Change "'" by "\'"
    {
     	 $full_name =~ s/\'/\\\'/g;
    }

    # process synonym.
    # If $synonym contain "'" this need to be encoded to "\'".

    if ( $synonym =~ /\\/)                                  # Change "\" by "\\"
    {
     	 $synonym =~ s/\\/\\\\/g;
    }
    if ( $synonym =~ /\'/)                                # Change "'" by "\'"
    {
     	 $synonym =~ s/\'/\\\'/g;
    }


#	print "processGOA_seq_dbxref: processed full name: $full_name, synonym $synonym\n";

    # Search xref_key from goa_seq_dbxref.
    # if $goa_xdb_id is a PDB record will be necessary to match PDB Chain ($symbol).
    # print("goa_xdb_id: $goa_xdb_id\t$pdbAbbrId\t $symbol \n");
    if ($goa_xdb_id == $pdbAbbrId)
    {
        $sth = $dbh->prepare("SELECT id FROM " . $database . ".".$dbname_goa_seq_dbxref." WHERE xref_key='" . $xref_key . "' AND symbol='" . $symbol . "'");
    }
    else
    {
        $sth = $dbh->prepare("SELECT id FROM " . $database . ".".$dbname_goa_seq_dbxref." WHERE xref_key='" . $xref_key . "'");
    }
    $sth->execute();
    if (@ary = $sth->fetchrow_array())
    {
        #print("id: " . $xref_key . "\t gi: " . $ary[0] . "\n");
	return ($ary[0]);
    }
    else
    {
        #print("id: " . $xref_key . " gi not found \n");
	#print("     Trying to search in SeqHound...\n");
	
	# SeqHound Find GI & ACCESION to fill GOA_seq_dbxref

        if( $sthInsGoaSeqDbx->execute($goa_xdb_id, $xref_key, $taxid_1, $taxid_2, $type_id, $symbol, $full_name, $synonym) == 1)
	{
	    $countSEQDBX++;
	}
	
	my $retid = getIdFromGoaSeqDbx($database, $goa_xdb_id, $xref_key, $symbol );

	return $retid;
    }
    # SHoundFini() will be called outside this function.
}


sub checkFileIntegrity($)
{
    open(FP1, $_[0]) or die "can't open given file: $!";
    my $lineCount = 0;
    my $flagExit = 0;

#    SHoundInit("TRUE", "foo");

    # Fields 1, 2, 3, 5, 6, 7, 9, 12, 13, 14, 15. needs to be present.
    # Field 5 Can't be NULL but since some Records are Missing Reference
    #         Will by pass this rule and put Field5= PMID:unpublushed
    my @fieldToCheck = (0, 1, 2, 4, 5, 6, 8, 11, 12, 13);
    my $line1;
    while ($line1 = <FP1>)
    {
	  my $lineCount++;
	  if ("!" ne substr($line1, 0, 1))          # Check if the line it's not a comment.
	  {
		chomp($line1);
		my @field = split(/\t/, $line1);
		if (scalar(@field) == $gNumOfFields or scalar(@field) == $gNumOfFields-1)
		{
		  my $db               = $field[0];    #     print("db: "                . $field[0] . "\n");
		  my $db_object_id     = $field[1];    #     print("db_object_id: "      . $field[1] . "\n");
		  my $db_object_symbol = $field[2];    #     print("db_object_symbol: "  . $field[2] . "\n");
		  my $db_not           = $field[3];    #     print("not: "               . $field[3] . "\n");
		  my $goid             = $field[4];    #     print("goid: "              . $field[4] . "\n");
		  my $db_reference     = $field[5];    #     print("db_reference: "      . $field[5] . "\n");
		  my $evidence         = $field[6];    #     print("evidence: "          . $field[6] . "\n");
		  my $with_from        = $field[7];    #     print("with_from: "         . $field[7] . "\n");
		  my $aspect           = $field[8];    #     print("aspect: "            . $field[8] . "\n");
		  my $db_object_name   = $field[9];    #     print("db_object_name: "    . $field[9] . "\n");
		  my $synonym          = $field[10];   #     print("synonym: "           . $field[10]. "\n");
		  my $db_object_type   = $field[11];   #     print("db_object_type: "    . $field[11]. "\n");
		  my $taxon            = $field[12];   #     print("taxon: "             . $field[12]. "\n");
		  my $date             = $field[13];   #     print("date: "              . $field[13]. "\n");
		  my $assigned_by      = $field[14];   #     print("assigned_by: "       . $field[14]. "\n");
		#	    print("-------------------------------------------------------\n\n");
		
		  foreach(@fieldToCheck)
		  {
			if (length($field[$_]) == 0)
			{
			  $flagExit = 1;
			  print("Field # " .$_ . " can't be NULL\n");
			  last;
			}
		  }

		  if ($flagExit == 1)
		  {
			last;
		  }

		  # Field 1 DB need to have an entry in hash_abbr copy from goa_xdb.
	
		  my $id = getDbNameIdFromHash($field[0]);

		  if ($id == -1)
		  {
			$flagExit = 1;
			print("Field # 1 " . $field[0] . " doesn't have an entry into goa_xdb.\n");
			last;
		  }
		  if ($flagExit == 1)
		  {
			last;
		  }
		  # Field 5 goid need to have the format GO:xxxxxxx
			
		  if (checkGOConsistency($field[4]) eq "F")
		  {
			$flagExit = 1;
			print("Field # 5 " . $field[4] . " doesn't complain with the format GO:xxxxxxx \n");
			last;
		  }
	
		  # Field 6 need to have the format xxxxx:xxxxx and it's tags need to have and entry
		  # into goa_xdb.
		  if (checkReferenceConsistency($field[5]) eq "F")
		  {
			$flagExit = 1;
			print("Field # 5 " . $field[5] . " " . $gMessage . "\n"); # " doesn't complain with the format xxxx:xxxxxxx \n");
			last;
		  }

		# Field 8 need to have the format xxxxx:xxxxx and it's tags need to have and entry
                # into goa_xdb. This Test is equal to the one in Field 6 Reference..
		# Not check with consintency if Evidence: IGI, IPI, ISS.
		  if (length($field[7]) > 0)
		  {
		    if (checkReferenceConsistency($field[7]) eq "F")
			{
			  #		        To bypass with in gene_association.tair for evidence IGI, IPI, ISS
			  #                       but not make to much sence.
			  if ($field[6] ne "IGI" && $field[6] ne "IPI" && $field[6] ne "ISS")
			  {
				$flagExit = 1;
				print("evidence: " . $field[6] . "\n");
				print("Field # 8 " . $field[7] . " " . $gMessage . "\n");
				last;
			  }
			}
		  }

		  # Field 9 Aspect need's to have only one (1) character.
		  if (length($field[8]) != 1)
		  {
			print("Field # 9 need's to have only one (1) character\n");
			$flagExit = 1;
			last;
		  }
	
		# Field 9 Aspect must be P, F, or C.
		  if ( ($field[8] ne "P") && ($field[8] ne "F") && ($field[8] ne "C"))
		  {
			print("Field # 9 " . $field[8] . " need's to one of the following Char: P, F, C.\n");
			$flagExit = 1;
			last;
		  }


		# Field 13 taxon need to have the format xxxx:xxxx
		  if (checkTaxonConsistency($field[12]) eq "F")
		  {
			$flagExit = 1;
			print("Field # 13 " . $field[12] . " doesn't complain with the format xxx:xxx \n");
			last;
		  }

		# Check if Date have only Numbers.

		  if ($field[13] =~ /\D/)
		  {
			die("Date field can have only Numbers: " . $field[13] . " Line " . $lineCount);
		  }
		
		# Check integrity test Assigned_by only if Assigned_by exists.
		# according to GO Spec. Assigned_by needs to exists but some field are missing it.
		  if (length($field[14]) > 0)
		  {
			if(getDbNameIdFromHash($field[14]) == -1)
			{
			    generateDummyFile();
			    die("Could not found assigned_by name: " . $field[14] . " abbreviation id into goa_xdb. Line " .$lineCount . "\n");
			}
		  }

		  if ($flagExit == 1)
		  {
			last;
		  }
		# Create more Checkings for different fields.

		}
		else
		{
		    generateDummyFile("Can't process line# " . $lineCount . " wrong number of fields\n" . $line1 . "\nHave: " . scalar(@field) . " fileds. Only 15 fields are allow.");
		    die ("Can't process line# " . $lineCount . " wrong number of fields\n" . $line1 . "\nHave: " . scalar(@field) . " fileds. Only 15 fields are allow.\n\n\n\n\n\n");
		}
		if ($flagExit == 1)
		{
		  last;
		}
	  }
	}
#    SHoundFini();
    if ($flagExit == 1)
    {
	  print ("Can't process line# " . $lineCount . " flag = 1\n" . $line1);
    }
    print("File Ok, " . $lineCount . " to process.\n");
    close(FP1);
    print("End Integrity Test ......................\n");
	#    die("End Integrity Test ......................\n");

}

sub writeFixFile($)
{
    open(OUTFILE, ">file.dat") or die ("Can't find file.dat\n");
    open(FP1, $_[0]) or die "can't open given file: $!";
    my $lineCount = 0;
    my $flagExit = 0;

#    SHoundInit("TRUE", "foo");

    # Fields 1, 2, 3, 5, 6, 7, 9, 12, 13, 14, 15. needs to be present.
    # Field 5 Can't be NULL but since some Records are Missing Reference
    #         Will by pass this rule and put Field5= PMID:unpublushed
    my @fieldToCheck = (0, 1, 2, 4, 6, 8, 11, 12, 13);
    my $lineCount = 0;
    my @field = ();
    my $line1;
    while ($line1 = <FP1>)
    {
        $lineCount++;
        if ("!" ne substr($line1, 0, 1))          # Check if the line it's not a comment.
	{
	    chomp($line1);
	    @field = split(/\t/, $line1);
	    print("lineCount: " . $lineCount . "\n");
	    print("gNumOfFields: " . $gNumOfFields . "\n");
	    print("scalar.field: " . scalar(@field) . "\n");
	    if ( (scalar(@field) == $gNumOfFields) || (scalar(@field) == $gNumOfFields-1) || (scalar(@field) > 14) )
	    {
	        my $db               = $field[0];    #     print("db: "                . $field[0] . "\n");
		my $db_object_id     = $field[1];    #     print("db_object_id: "      . $field[1] . "\n");
		my $db_object_symbol = $field[2];    #     print("db_object_symbol: "  . $field[2] . "\n");
		my $db_not           = $field[3];    #     print("not: "               . $field[3] . "\n");
		my $goid             = $field[4];    #     print("goid: "              . $field[4] . "\n");
		my $db_reference     = $field[5];    #     print("db_reference: "      . $field[5] . "\n");
		my $evidence         = $field[6];    #     print("evidence: "          . $field[6] . "\n");
		my $with_from        = $field[7];    #     print("with_from: "         . $field[7] . "\n");
		my $aspect           = $field[8];    #     print("aspect: "            . $field[8] . "\n");
		my $db_object_name   = $field[9];    #     print("db_object_name: "    . $field[9] . "\n");
		my $synonym          = $field[10];   #     print("synonym: "           . $field[10]. "\n");
		my $db_object_type   = $field[11];   #     print("db_object_type: "    . $field[11]. "\n");
		my $taxon            = $field[12];   #     print("taxon: "             . $field[12]. "\n");
		my $date             = $field[13];   #     print("date: "              . $field[13]. "\n");
		my $assigned_by      = $field[14];   #     print("assigned_by: "       . $field[14]. "\n");
			    print("-------------------------------------------------------\n\n");
		
		foreach(@fieldToCheck)
		{
		    if (length($field[$_]) == 0)
		    {
		        $flagExit = 1;
			print("Field # " .$_ . " can't be NULL\n");
			last;
		    }
		}

		if ($flagExit == 1)
		{
		    last;
		}

		# Field 1 DB need to have an entry in hash_abbr copy from goa_xdb.
		if (!$hash_abbr{$field[0]})
		{
		    $flagExit = 1;
		    print("Field # 1 " . $field[0] . " doesn't have an entry into goa_xdb.\n");
		    last;
		}
		
		# Field 2 db_object_id need to be in the form xxx:xxx 
#		if( checkSemiColonFieldFormat($field[1]) eq "F")
#		{
#		    $flagExit = 1;
#		    print("Field # 2 " . $field[1] . " doesn't complain with the format xxx:xxx \n");
#		    last;
#		}

		# Field 5 goid need to have the format GO:xxxxxxx

		if (checkGOConsistency($field[4]) eq "F")
		{
		    $flagExit = 1;
		    print("Field # 5 " . $field[4] . " doesn't complain with the format GO:xxxxxxx \n");
		    last;
		}
	
		# Field 6 need to have the format xxxxx:xxxxx and it's tags need to have and entry
                # into goa_xdb.
		if (checkReferenceConsistency($field[5]) eq "F")
		{
		    $flagExit = 1;
		    print("Field # 5 " . $field[5] . " doesn't complain with the format xxxx:xxxxxxx \n");
		    last;
		}

		# Field 8 need to have the format xxxxx:xxxxx and it's tags need to have and entry
                # into goa_xdb. This Test is equal to the one in Field 6 Reference..
		if (length($field[7]) > 0)
		{
		    if (checkReferenceConsistency($field[7]) eq "F")
		    {
		        $flagExit = 1;
			print("Field # 8 " . $field[7] . " " . $gMessage . "\n");
#			      " doesn't complain with the format xxx:xxx \n");
			last;
		    }
		}

		# Field 9 Aspect need's to have only one (1) character.
		if (length($field[8]) != 1)
		{
		    print("Field # 9 need's to have only one (1) character\n");
		    $flagExit = 1;
		    last;
		}
	
		# Field 9 Aspect must be P, F, or C.
		if ( ($field[8] ne "P") && ($field[8] ne "F") && ($field[8] ne "C"))
		{
		    print("Field # 9 " . $field[8] . " need's to one of the following Char: P, F, C.\n");
		    $flagExit = 1;
		    last;
		}


		# Field 13 taxon need to have the format xxxx:xxxx
		if (checkTaxonConsistency($field[12]) eq "F")
		{
		    $flagExit = 1;
		    print("Field # 13 " . $field[12] . " doesn't complain with the format xxx:xxx \n");
		    last;
		}

		if ($flagExit == 1)
		{
		    last;
		}
		# Create more Checkings for different fields.
	    }
	    else
	    {
	        print("gNumOfFields: " . $gNumOfFields . "\n");
		print("scalar.field: " . scalar(@field) . "\n");

	        die("Can't process line# " . $lineCount . "\n" . $line1 . "\n\n\n\n\n\n\n");
	    }
	    if ($flagExit == 1)
	    {
		last;
	    }
	}
    }
#    SHoundFini();
    if ($flagExit == 1)
    {
        print("gNumOfFields: " . $gNumOfFields . "\n");
	print("scalar.field: " . scalar(@field) . "\n");
        die("Can't process line# " . $lineCount . "\n" . $line1);
    }
    print("File Ok, " . $lineCount . " to process.\n");
    close(FP1);
    print("End Integrity Test ......................\n");
#    die("End Integrity Test ......................\n");

}


# Implementation of all "GO" consistency checks.
# * Can't be more that 1 (one) GO tag per Record.
# * The tag need to be "GO:".
# * Each GO tag need to have a Numeric value.
# * The value need to be numerical?
# Argument: $_[0] String to check.
sub checkGOConsistency($)
{
    if( checkGOFormat($_[0], "GO:", "N") eq "F")
    {
        return "F";
    }
    else
    {
        return "T";
    }
}


# Check GO format
# * Split the "|" separated list
# * Check if each pair (tag:value) )have the format XXXX:XXXX  and the Value is Numeric.
# * Check if does'n have more than 1 pairs.
# Arguments: $_[0] String to check.
#            $_[1] String tag name
#            $_[2] String Value Type "N" for Numeric [0-9], 
#                                    "A" for Alpha Numerica Characters.
sub checkGOFormat($, $, $)
{
     my $exit = 0;
#     print("line: " . $_[0] . "\n");
#     print("element " . $_. "\n");
     if (checkSemiColonAndValueType($_[0], $_[2]) eq "F")
     {
         return "F";
     }
     if (substr($_[0], 0, length($_[1])) ne $_[1] )
     {
         print("Wrong " . $_[1] . " " . $_ . "  tag \n");
	 return "F";
     }
     return "T";
}


# Implementation of all "taxon" related consistency checks.
# * Can't be more that 2 (two) taxon tags per Record.
# * The tag need to be "taxon:".
# * Each taxon tag need to have a value.
# * The value need to be numerical?
sub checkTaxonConsistency($)
{
    if( checkTaxonFormat($_[0], "taxon:", "N") eq "F")
    {
        return "F";
    }
    else
    {
        return "T";
    }
}

# Check GO format
# * Split the "|" separated list
# * Check if each pair (tag:value) )have the format XXXX:XXXX  and the Value is Numeric.
# * Check if does'n have more than 2 pairs.
# Arguments: $_[0] String to check.
#            $_[1] String tag name
#            $_[2] String Value Type "N" for Numeric [0-9], 
#                                    "A" for Alpha Numerica Characters.
sub checkTaxonFormat($, $, $)
{
     my $exit = 0;
     my @subRecord = split(/\|/, $_[0]);
#     print("line: " . $_[0] . "\n");
     foreach(@subRecord)
     {
#         print("element " . $_. "\n");
	 if (checkSemiColonAndValueType($_, $_[2]) eq "F")
	 {
	     return "F";
	 }
	 if (substr($_, 0, length($_[1])) ne $_[1] )
	 {
#	     print("Wrong " . $_[1] . " " . $_ . "  tag \n");
	     return "F";
	 }
     }
     if(scalar(@subRecord) > 2)
     {
#          print("Wrong " . $_[1] . " tags can't be more than 2 elements.\n");
	  return "F";
     }
     return "T";
}


sub checkSemiColonAndTag($, $)
{
     my $exit = 0;
     my @subRecord = split(/\|/, $_[0]);
#     print("line: " . $_[0] . "\n");
     foreach(@subRecord)
     {
#         print("element " . $_. "\n");
	 if (checkSemiColonFieldFormat($_) eq "F")
	 {
#	     print("wrong pair1 \n");
	     return "F";
	 }
	 if (substr($_, 0, length($_[1])) ne $_[1] )
	 {
#	     print("Wrong " . $_[1] . " " . $_ . "  tag \n");
	     return "F";
	 }
     }
     if(scalar(@subRecord) > 2)
     {
#          print("Wrong " . $_[1] . " tags can't be more than 2 \n");
	  return "F";
     }
     return "T";
}


# Implementation of all "GO" consistency checks.
# * Can't be more that 1 (one) GO tag per Record.
# * The tag need to be "GO:".
# * Each GO tag need to have a Numeric value.
# * The value need to be numerical?
# Argument: $_[0] String to check.
sub checkReferenceConsistency($)
{
#     print("line: " . $_[0] . "\n");
#     print("element " . $_. "\n");
     my @subRecord = split(/\|/, $_[0]);
     foreach(@subRecord)
     {
#         print("element " . $_. "\n");
	 if (checkSemiColonFieldFormatAndTagIntoDB($_) eq "F")
	 {
	     return "F";
	 }
     }
     return "T";
}


sub checkSemiColonFieldFormatAndTagIntoDB($)
{
    my $tag = substr($_[0], 0, index($_[0], ":", 0));
    $tag =~ s/\s//;
    $tag =~ s/" "//;
#    $tag =~ s/\s//;
#    print("   tag: " . $tag . "\n");
#    print("   substr: " . substr($_[0], index($_[0], ":", 0)+1) . "\n");

    if (index($_[0], ":") > 0)
    {
        if ( (length(substr($_[0], 0, index($_[0], ":", 0))) < 1) || (length(substr($_[0], index($_[0], ":", 0)+1)) < 1) )
	{
	    $gMessage = "Tag: " . $tag . "|" . substr($_[0], index($_[0], ":", 0)+1) . " doesn't have the format xxxx:xxxxx\n";
	    return "F";
	}
	if (!$hash_abbr{$tag})
	{
#	    print("    Tag: " . $tag . " doesn't have an entry into goa_xdb.\n");
	    $gMessage = "Tag: " . $tag . " doesn't have an entry into goa_xdb.\n";
	    return "F";
	}
	return "T";
    }
    else
    {
        $gMessage = "Missing Tag:Value Pair\n";
        return "F";
    }
}

sub checkSemiColonPipeFieldFormat($)
{
     my $exit = 0;
     my @subRecord = split(/\|/, $_[0]);
#     print("line: " . $_[0] . "\n");
     foreach(@subRecord)
     {
#         print("element " . $_. "\n");
	 if (checkSemiColonFieldFormat($_) eq "F")
	 {
#	     print("wrong pair1 \n");
	     return "F";
	 }
     }
     return "T";
}


# Check if the string have the format xxxx:xxxx
sub checkSemiColonFieldFormat($)
{
#    print("   substr: " . substr($_[0], 0, index($_[0], ":", 0)) . "\n");
#    print("   substr: " . substr($_[0], index($_[0], ":", 0)+1) . "\n");
    if (index($_[0], ":") > 0)
    {
        if ( (length(substr($_[0], 0, index($_[0], ":", 0))) < 1) || (length(substr($_[0], index($_[0], ":", 0)+1)) < 1) )
	{
	    return "F";
	}
	return "T";
    }
    else
    {
        return "F";
    }
}


# Check if the string have the format xxxx:xxxx
# and if the value it's numeric [0-9]
#
sub checkSemiColonAndValueType($, $)
{
#    print("   substr: " . substr($_[0], 0, index($_[0], ":", 0)) . "\n");

    my $value = substr($_[0], index($_[0], ":", 0)+1);
#    print("value1: " . $value . "\n");

    if ($_[1] eq "N")
    {
        if ( $value =~ /\D/ )
#	{print("Value " . $value . " it's Digits [0-9] \n");}
#	else
        {
#            print("Value " . $value . " need to be only Digits [0-9] \n");
	    return "F";
	}
    }
    if (index($_[0], ":") > 0)
    {
        if ( (length(substr($_[0], 0, index($_[0], ":", 0))) < 1) || (length(substr($_[0], index($_[0], ":", 0)+1)) < 1)  )
	{
	    return "F";
	}
	return "T";
    }
    else
    {
        return "F";
    }
}


sub getIdFromGoaSeqDbx($, $, $, $)
{
  my $sth;
 
   if($_[1] == $pdbAbbrId)
   {
       $sth = $dbh->prepare("SELECT id FROM " . $_[0] . ".".$dbname_goa_seq_dbxref." WHERE xref_key='" . $_[2] . "' AND symbol='" . $_[3] . "'");
   }
   else
   {
       $sth = $dbh->prepare("SELECT id FROM " . $_[0] . ".".$dbname_goa_seq_dbxref." WHERE xref_key='" . $_[2] ."'");
   }
   $sth->execute();
   if (my @ary = $sth->fetchrow_array())
   {
        return ($ary[0]);
   }
   else
   {
        return -1;
   }
}

# $_[0]: Database Name
# $_[1]: goa_seq_dbxref_id
# $_[2]: go_id
sub getMaxIdFromGoaAssociation($, $, $)
{
   #$sth = $dbh->prepare("SELECT max(id) FROM " . $_[0] . ".".$dbname_goa_association." WHERE goa_seq_dbxref_id=" . $_[1]);
   # Cluster safe.
  $sth = $dbh->prepare("SELECT id FROM " . $_[0] . ".".$dbname_goa_association." WHERE goa_seq_dbxref_id= $_[1] and go_id=$_[2]");
   $sth->execute();
   if (my @ary = $sth->fetchrow_array())
   {
        return ($ary[0]);
   }
   else
   {
        return -1;
   }
}

# Reset tables: goa_seq_dbxref, goa_association, goa_reference, goa_with.
# Check if Table exist before trying to create it.
sub resetTables()
{

   $sth = $dbh->prepare("DROP TABLE IF EXISTS $dbname_goa_seq_dbxref");
   $sth->execute();
   $sth = $dbh->prepare("CREATE TABLE " . $database . ".".$dbname_goa_seq_dbxref." ( id INTEGER UNSIGNED NOT NULL PRIMARY KEY AUTO_INCREMENT, goa_xdb_id INTEGER UNSIGNED NOT NULL, xref_key VARCHAR(30), taxid_1 INTEGER NOT NULL, taxid_2 INTEGER, type_id VARCHAR(20) NOT NULL, symbol VARCHAR(30) NOT NULL, full_name VARCHAR(255), synonym VARCHAR(50), Index goa_xdb_id_idx(goa_xdb_id), Index xref_key_idx(xref_key), Index symbol_idx(symbol) )");
   $sth->execute();

   $sth = $dbh->prepare("DROP TABLE IF EXISTS $dbname_goa_association");
   $sth->execute();
   $sth = $dbh->prepare("CREATE TABLE " . $database . ".".$dbname_goa_association." ( id INTEGER UNSIGNED NOT NULL PRIMARY KEY AUTO_INCREMENT, is_not CHAR(1), go_id INTEGER UNSIGNED NOT NULL, goa_seq_dbxref_id INTEGER UNSIGNED NOT NULL, assigned_by INTEGER UNSIGNED NOT NULL,code CHAR(4) NOT NULL, date CHAR(8),CONSTRAINT UNIQUE (go_id, goa_seq_dbxref_id, code ), Index goa_id_idx(go_id),Index goa_seq_dbxref_id_idx(goa_seq_dbxref_id), Index assigned_by_idx(assigned_by) )");
   $sth->execute();	

   $sth = $dbh->prepare("DROP TABLE IF EXISTS $dbname_goa_reference");
   $sth->execute();
   $sth = $dbh->prepare("CREATE TABLE " . $database . ".".$dbname_goa_reference." ( id INTEGER UNSIGNED NOT NULL PRIMARY KEY AUTO_INCREMENT, goa_association_id INTEGER UNSIGNED NOT NULL, goa_xdb_id INTEGER UNSIGNED NOT NULL, xref_key VARCHAR(20) NOT NULL, Index goa_association_id_idx(goa_association_id), Index goa_xdb_id_idx(goa_xdb_id) )");
   $sth->execute();

   $sth = $dbh->prepare("DROP TABLE IF EXISTS $dbname_goa_with");
   $sth->execute();
   $sth = $dbh->prepare("CREATE TABLE " . $database . ".".$dbname_goa_with." ( id INTEGER UNSIGNED NOT NULL PRIMARY KEY AUTO_INCREMENT, goa_association_id INTEGER UNSIGNED NOT NULL, goa_xdb_id INTEGER UNSIGNED NOT NULL, xref_key VARCHAR(20) NOT NULL, key_type INTEGER UNSIGNED NOT NULL, goa_seq_dbxref_id INTEGER UNSIGNED NOT NULL, Index goa_association_id_idx(goa_association_id), Index goa_xdb_id_idx(goa_xdb_id), Index goa_seq_dbxref_id_idx(goa_seq_dbxref_id) )");
   $sth->execute();

}




sub prepareInsertGoaSeqDbx($, $)
{
    $sthInsGoaSeqDbx = $_[0]->prepare("INSERT INTO " . $_[1] . "." . $dbname_goa_seq_dbxref .
				      "(goa_xdb_id, xref_key, taxid_1, taxid_2, type_id, symbol, full_name, synonym) " .
				      "VALUES(?, ?, ?, ?, ?, ?, ?, ?)");
}

sub prepareInsertGoaAssociation($, $)
{
     $sthInsGoaAss = $_[0]->prepare("INSERT IGNORE INTO " . $_[1] . "." . $dbname_goa_association .
				    "(is_not, go_id, goa_seq_dbxref_id, assigned_by, code, date) " .
				    "VALUES(?, ?, ?, ?, ?, ?)");
}

sub prepareInsertGoaReference($, $)
{
    $sthInsGoaRef = $_[0]->prepare("INSERT INTO " . $_[1] . "." . $dbname_goa_reference .
				   "(goa_association_id, goa_xdb_id, xref_key) " .
				   "VALUES(?, ?, ?)");
}

sub prepareInsertGoaWith($, $)
{
    $sthInsGoaWith = $_[0]->prepare("INSERT INTO " . $_[1] . "." . $dbname_goa_with .
				    "(goa_association_id, goa_xdb_id, xref_key, key_type, goa_seq_dbxref_id) " .
				    "VALUES(?, ?, ?, ?, ?)");
}


sub generateDummyFile($)
{
    # =================  CLUSTERING ================================
    # Generate a dummy file for sincronization.
    if ($cluster eq 'T')
    {
	if ($dummy)
	{
	    $dummy =~ /(\w+)(\.)/;
	    $node = $1;
	    print("$node ERROR Ends with problems.\n");
	    print("\n================================================================================\n");
	}
	
        if ($dummy)
        {
	    open(FPOUT, ">$dummy");
	    print(FPOUT "dummy file ".$dummy);
	    close(FPOUT);
	}
    }
    # =============================================================
}
