# use strict;

# test scipt
# 1) connects to MySQL-database
# 2) processes accession number from file
# 3) extracts GO-annotations that belong to a given acc-number
# 4) spits out the result
# 5) disconnect

# two choises are available - to use DBD-mysql or to use DBI to connect to
# MySQL-db.
# DBI is a most universal one (DBD-mysql is too specific)
use DBI;

$gNumOfFields = 15;
$gFileToProcess = $ARGV[0];
#$gFileToProcess = "gene_association.fb";

my $host = "10.0.0.122";
# my $port = "13306";
# my $database = "seqhoundtest";

#my $host = "10.0.0.37";
my $port = "33306";
my $database = "ebxref";

# //my $dsn = "DBI:mysql:swiss_prot:localhost";
my $dsn = "DBI:mysql:" . $database . ":" . $host . ":" . $port;

# RAC Read this parameters from a propertie files.
my $user_name = "eburgess";
my $password = "asdf1234";
my $dbh, $sth;



my @ary;

print("time: " . time() . "\n");
$t1 = time();

# 2) connect to MySQL-database
$dbh = DBI->connect ($dsn, $user_name, $password, {RaiseError => 1});


#Reset table
##########################
#resetTables();
#die("End. Table Reset.\n");


# Create a HasTable for the db abbreviations and Ids.
# %abbr_id HasTable of Key: Abbreviation, Value: Ids.
# This hash table was created to avoid searching into goa_xdb each time
# an ID is needed from a particular abbreviation name.
##########################
createAbbrHashTable();

# Write Fix File
# if the file have an but try to repair it.
#writeFixFile($gFileToProcess);
#die("End. writeFixFile()\n");



#Check File integrity.
##########################
# -Count number of files per line (14)or(15) tab delimited.
checkFileIntegrity($gFileToProcess);
die("End CheckFileIntegrity\n");

# 1) process GO.xrf_abbs Cross Reference database.

# RAC Read the file name from command line.

open(FP, $gFileToProcess) or die "can't open given file: $!";


$gMessage;                              # Global message to return Errors.

$count = 0;                             # Count the number of records


while ($line = <FP>)
{
#    print($line);
    $count++;
    if ("!" ne substr($line, 0, 1))
    { 
        chomp($line);
	@record = split(/\t/, $line);
	$db               = $record[0];     #    print("db: "                . $record[0] . "\n");
	$db_object_id     = $record[1];     #    print("db_object_id: "      . $record[1] . "\n");
	$db_object_symbol = $record[2];     #    print("db_object_symbol: "  . $record[2] . "\n");
	$db_not           = $record[3];     #    print("not: "               . $record[3] . "\n");
	$goid             = $record[4];     #    print("goid: "              . $record[4] . "\n");
	$db_reference     = $record[5];     #    print("db_reference: "      . $record[5] . "\n");
	$evidence         = $record[6];     #    print("evidence: "          . $record[6] . "\n");
	$with_from        = $record[7];     #    print("with_from: "         . $record[7] . "\n");
	$aspect           = $record[8];     #    print("aspect: "            . $record[8] . "\n");
	$db_object_name   = $record[9];     #    print("db_object_name: "    . $record[9] . "\n");
	$synonym          = $record[10];    #    print("synonym: "           . $record[10]. "\n");
	$db_object_type   = $record[11];    #    print("db_object_type: "    . $record[11]. "\n");
	$taxon            = $record[12];    #    print("taxon: "             . $record[12]. "\n");
	$date             = $record[13];    #    print("date: "              . $record[13]. "\n");
	$assigned_by      = $record[14];    #    print("assigned_by: "       . $record[14]. "\n");


	# Because some records are missing it's Reference, an unpublished reference was created
	# and all record with out reference is givin an "unpublished" refrence.
	# Key:unpublished     value:0.
	if (length($db_reference) == 0)
	{
	    $db_reference = "unpublished:unpublished";
	}

	# Process db
#	$db_id = getDbNameId($db);
	$id = getDbNameIdFromHash($db);
#	$db_id = $hash_abbr{$db};
#	print($db .": " . $db_id ."\n");

#	if (!$db_id)                    # if db abbreviation doesn't exist.
	if ($id == -1)
	{
	    die("Could not find db name: " . $db . " abbreviation id into goa_xdb. Line " .$count . "\n");
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
#	print("assigned_by: " . $assigned_by . "\n");
	$assigned_by_id  = getDbNameIdFromHash($assigned_by);
#	print("assigned_by_id: " . $assigned_by_id . "\n");
	if ($assigned_by_id == -1)
	{
	    die("Could not find assigned_by name: " . $assigned_by . " abbreviation id into goa_xdb\nLine " .$count . "\n");
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
	$goid = substr($goid, length("go:"));

	# Process DB_Reference

#	if(substr($db_reference, 0, length("GOA:")) eq "GOA:")
#	{
#	    $reference_by_id = getDbNameIdFromHash($db_reference);
#	    $reference_by_key = substr($db_reference, index($db_reference, ":", 0)+1);
#	    print("reference_by_id 1: " . $reference_by_id . "\n");
#	    print("reference_by_key 1: " . $reference_by_key  . "\n");
#	}
#	else
#	{
#	    $reference_by_id = getDbNameIdFromHash(substr($db_reference, 0, index($db_reference, ":", 0)));
#	    $reference_by_key = substr($db_reference, index($db_reference, ":", 0)+1);
#	    print("reference_by_id 2: " . $reference_by_id . "\n");
#	    print("reference_by_key 2: " . $reference_by_key  . "\n");
#	}


	#Process with_from
	$with_from_id = getDbNameIdFromHash(substr($with_from, 0, length("TAIR")));
	$with_from_key = substr($with_from, length("TAIR")+1);

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


	# Insert Recort into goa_seq_dbxref.
	$goa_seq_dbxref_id = processGOA_seq_dbxref($db_id, $db_object_id, $taxon, $db_object_type, $db_object_symbol, $db_object_name, $synonym);


#	print("goa_seq_dbxref_id : " . $goa_seq_dbxref_id . "\n\n");
	# Insert the record into goa_association
	# e.g.: INSERT INTO seqhound.goa_association VALUES(0, 'F', 4872, 0, 15, 'ISS', '20030329')
	##$sth = $dbh->prepare("INSERT INTO seqhound.goa_association VALUES(0, 'F', 0, 15, 'ISS', '20030329')"); # . $goid . ", 0, " . $assigned_by_id . ", '" . $evidence . "', '" . $date . "')");

#######
	print("\ndb_no: " .  $db_not . "\n goid: ". $goid . "\n goa_seq_dbxref_id: " . $goa_seq_dbxref_id);
	print(" \n assigned_by_id: " .$assigned_by_id . "\n  evidence: " . $evidence . "\n date: " .$date ."\n");

	$sth = $dbh->prepare ("INSERT INTO " . $database . ".goa_association VALUES(0, '" . $db_not . "', " . $goid . ", " .  $goa_seq_dbxref_id . ", " . $assigned_by_id . ", '" . $evidence . "', '" . $date . "')");
	$sth->execute();
	$goa_association_id = getMaxRecordsFromTable("id", "goa_association");
#######

#	print("Max goa_association_id: " . $goa_association_id);
#	print("evidence.length()" . length($evidence) . "\n");
#	print("evidence: " . $evidence . "\n");

	#process Reference
#	print("reference: " . $reference . "\n");
	processReference($goa_association_id, $db_reference);

	#process WithFrom
	#print("with_from: " . $with_from . "\n");
	processWithFrom($goa_association_id, $with_from);

#	print("---> " . $count . "<---");


    } #if()

} #while()


close(FP);


$sth->finish();
$dbh->disconnect();
print($count . " records Updated into goa_xdf database.\n");
$t2 = time();
print("time: " . $t2 . "\n");
print("Execution time : " . ($t2-$t1) . " sec.\n\n");
if( $t2-$t1 > 60)
{
    print("                 " . ($t2-$t1)/60 ." min.\n");
}
exit(0);  



# Get GI from mapper
sub getGIfromObjectID($)
{
    print("object_id: " . $_[0] . "\n");

    $sth = $dbh->prepare ("select refseq_gi from " . $database . ".mapper where object_id='" . $_[0] . "'" );
    $sth->execute();
    if (@ary = $sth->fetchrow_array())
    {
       print("        dbid: " . $ary[0] . "\n");
        return ($ary[0]);
    }
    else
    {
        print("        dbid: " . $ary[0] . "\n");
        return -1;
    }
}

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
    my $abbr = $_[0];
	my $abbr_id = -1;
	
	#Make sure that the case is correct:
	$tag =~ s/UNIPROT/UniProt/;
	$tag =~ s/GENEDB_SPOMBE/GeneDB_Spombe/;

	#Make sure that GOC:unpublished is handled correctly
	$tag =~ s/GOC/GOC:unpublished/;

	#If they've used AGRICOLA as an abbreviation, change it to
	#to the correct abbreviation /bib/
	$tag =~ s/AGRICOLA/bib/;

	if( $abbr =~ /GOC/ )
	{
	  
	  $abbr_id = $hash_abbr{"GOC:unpublished"};

	}
    elsif(exists($hash_abbr{$_[0]}))
    {

		  $abbr_id = $hash_abbr{$_[0]};

    }
    else
    {
        $abbr_id = -1;
    }

	return $abbr_id;
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

#    my @row = $sth->fetchrow_array;
#    print($row[0] ."\t" . $row[1] . "\n");
 
    #Crete an abreviation id hash.
    $hash_abbr{unpublished} = 0;
#   print("hash_abbr . CGEN.key: " . $hash_abbr{CGEN} . "\n");
    while (@row = $sth->fetchrow_array)
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
    @refRecord = split(/\|/, $_[1]);
    foreach(@refRecord)
    {
#        print( $_  . "\n");


	# By pass exception for "GOA:". GOA can be of one of the form "interpro", "spec", "spkw".
	# but the name of the DB is GOA.xxxxxxx. For this reason the reference_by_id need to be
        # GOA.xxxxxx.
	#    Here need to be proccesed pipes of References.

        if (substr($_, 0, length("GOA:")) eq "GOA:")
	{
	    $ref[0] = $_;
	    $ref[1] = substr($_, index($_, ":", 0)+1);
	}
	else
	{
	    $ref[0] = substr($_, 0, index($_, ":", 0));
	    $ref[1] = substr($_, index($_, ":", 0)+1);
#	    print("   ref: " . $ref[0]  . "\n");
#	    print("   key: " . $ref[1]  . "\n");
	}
	$dbid = getDbNameIdFromHash($ref[0]);
#	print("dbid: " . $dbid . "\n");
	# Insert the record into goa_reference
	#e.g.: INSERT INTO seqhound.goa_reference VALUES(0, 1, 53, 'unpublished')
	$sth = $dbh->prepare ("INSERT INTO " . $database . ".goa_reference VALUES(0, " . $_[0] . ", " . $dbid . ", '" . $ref[1]. "')");
	$sth->execute();
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
    @refRecord = split(/\|/, $_[1]);
    foreach(@refRecord)
    {
#        print( $_  . "\n");
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
	$dbid = getDbNameIdFromHash($ref[0]);
	# Insert the record into goa_with
	#e.g.: INSERT INTO seqhound.goa_with VALUES(0, 1, 53, 'unpublished', 0, 0);
	$sth = $dbh->prepare ("INSERT INTO " . $database . ".goa_with VALUES(0, " . $_[0] . ", " . $dbid . ", '" . $ref[1]. "', " . 0 . ", " . 0 . ")");
	$sth->execute();
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


    # process full_name.
    # If $full_name contain "'" this need to be encoded to "\'".
    if (index($full_name, "\'", 0) > 0)
    {
        $full_name =~ s/\'/\\\'/g;                            # Change "'" by "\'"
    }

    # process synonym. 
    # If $full_name contain "'" this need to be encoded to "\'".
    if (index($synonym, "\'", 0) > 0)
    {
        $synonym =~ s/\'/\\\'/g;                            # Change "'" by "\'"
    }

    # Search xref_key from goa_seq_dbxref.

    $sth = $dbh->prepare("SELECT id FROM " . $database . ".goa_seq_dbxref WHERE xref_key='" . $xref_key . "'");
    $sth->execute();
    if (@ary = $sth->fetchrow_array())
    {
        #print("id: " . $xref_key . " gi: " . $ary[1] . "\n");
	return ($ary[0]);
    }
    else
    {
        #print("id: " . $xref_key . " gi not found \n");
	#print("     Trying to search in SeqHound...\n");
	
	# SeqHound Find GI & ACCESION to fill GOA_seq_dbxref

#  Disable for now
#        $gi = getGIfromObjectID($xref_key);
#	if ($gi == -1)
#	{
#	    $gi = SHoundFindName($xref_key);
#	
#	    if ($gi == 0)
#	    {
#	        #print("          Gi not found for id: " . $xref_key . "\n");
#	    }
#	    else
#	    {
#	        #print("          Gi:" . $gi . " found for id: " . xref_key . "\n");
#	    }
#	 }
	#print("     Inserting GI in GOA_seq_dbxref\n");
	
	#print("xref_key: " .  $xref_key . "\n gi: ". $gi . "\n accession: " . $accession);
	#print(" \n taxid_1: " .$taxid_1 . "\n taxid_2: " .$taxid_2 . "\n type_id " .$type_id);
	#print(" \n symbol: " . $symbol . "\n full_name: " . $full_name . "\n synonym: ".$synonym);

	$sth = $dbh->prepare("INSERT INTO " . $database . ".goa_seq_dbxref VALUES(0, 0, '" . $xref_key . "', " .$taxid_1 . ", " .$taxid_2 . ", '" .$type_id. "', '" .$symbol. "', '" .$full_name . "', '" .$synonym. "')");
	$sth->execute();
	my $retid = getMaxRecordsFromTable("id", "goa_seq_dbxref");
	return $retid ;
    }
    # SHoundFini() will be called outside this function.
}


sub checkFileIntegrity($)
{
    open(FP1, $_[0]) or die "can't open given file: $!";
    $lineCount = 0;
    $flagExit = 0;
    $flagError = 0;
    $errorCount = 0;
#    SHoundInit("TRUE", "foo");

    # Fields 1, 2, 3, 5, 6, 7, 9, 12, 13, 14, 15. needs to be present.
    # Field 5 Can't be NULL but since some Records are Missing Reference
    #         Will by pass this rule and put Field5= PMID:unpublushed
    @fieldToCheck = (0, 1, 2, 4, 5, 6, 8, 11, 12, 13);

    while ($line1 = <FP1>)
    {
        $lineCount++;
        if ("!" ne substr($line1, 0, 1))          # Check if the line it's not a comment.
	{
	    chomp($line1);
	    @field = split(/\t/, $line1);
	    if (scalar(@field) == $gNumOfFields or scalar(@field) == $gNumOfFields-1)
	    {
	        $db               = $field[0];    #     print("db: "                . $field[0] . "\n");
		$db_object_id     = $field[1];    #     print("db_object_id: "      . $field[1] . "\n");
		$db_object_symbol = $field[2];    #     print("db_object_symbol: "  . $field[2] . "\n");
		$db_not           = $field[3];    #     print("not: "               . $field[3] . "\n");
		$goid             = $field[4];    #     print("goid: "              . $field[4] . "\n");
		$db_reference     = $field[5];    #     print("db_reference: "      . $field[5] . "\n");
		$evidence         = $field[6];    #     print("evidence: "          . $field[6] . "\n");
		$with_from        = $field[7];    #     print("with_from: "         . $field[7] . "\n");
		$aspect           = $field[8];    #     print("aspect: "            . $field[8] . "\n");
		$db_object_name   = $field[9];    #     print("db_object_name: "    . $field[9] . "\n");
		$synonym          = $field[10];   #     print("synonym: "           . $field[10]. "\n");
		$db_object_type   = $field[11];   #     print("db_object_type: "    . $field[11]. "\n");
		$taxon            = $field[12];   #     print("taxon: "             . $field[12]. "\n");
		$date             = $field[13];   #     print("date: "              . $field[13]. "\n");
		$assigned_by      = $field[14];   #     print("assigned_by: "       . $field[14]. "\n");
		#	    print("-------------------------------------------------------\n\n");
		
		foreach(@fieldToCheck)
		{
		    if (length($field[$_]) == 0)
		    {
		        $flagExit = 1;
			$flagError = 1;
			print("Field # " . ($_+1) . " can't be NULL\n");
		        #last;
		    }
		}

		if ($flagExit == 1)
		{
		    # last;
		}

		# Field 1 DB need to have an entry in hash_abbr copy from goa_xdb.

		$id = getDbNameIdFromHash($field[0]);

		if ($id == -1)
		{
		    $flagExit = 1; $flagError = 1;
		    print("Field # 1 " . $field[0] . " doesn't have an entry into goa_xdb.\n");
		    # last;
		}
		if ($flagExit == 1)
		{
		    # last;
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
		    $flagExit = 1; $flagError = 1;
		    print("Field # 5 " . $field[4] . " doesn't complain with the format GO:xxxxxxx \n");
		    # last;
		}
	
		# Field 6 need to have the format xxxxx:xxxxx and it's tags need to have and entry
                # into goa_xdb.

		if (checkReferenceConsistency($field[5]) eq "F")
		{
		    $flagExit = 1; $flagError = 1;
		    print("Field # 6 " . $field[5] . " " . $gMessage . "\n"); # " doesn't complain with the format xxxx:xxxxxxx \n");
		    # last;
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
			    $flagExit = 1; $flagError = 1;
#			    print("evidence: " . $field[6] . "\n");
			    print("Field # 8 " . $field[7] . " " . $gMessage ."\n");
			#    last;
			}
		    }
		}

		# Field 9 Aspect need's to have only one (1) character.
		if (length($field[8]) != 1)
		{
		    print("Field # 9 need's to have only one (1) character\n");
		    $flagExit = 1;  $flagError = 1;
		    # last;
		}
	
		# Field 9 Aspect must be P, F, or C.
		if ( ($field[8] ne "P") && ($field[8] ne "F") && ($field[8] ne "C"))
		{
		    print("Field # 9 " . $field[8] . " need's to one of the following Char: P, F, C.\n");
		    $flagExit = 1; $flagError = 1;
		    # last;
		}


		# Field 13 taxon need to have the format xxxx:xxxx
		if (checkTaxonConsistency($field[12]) eq "F")
		{
		    $flagExit = 1; $flagError = 1;
		    print("Field # 13 " . $field[12] . " doesn't complain with the format xxx:xxx \n");
		    # last;
		}

		# Check if Date have only Numbers.

		if ($field[13] =~ /\D/)
		{ 
		  $flagExit = 1; $flagError = 1;
		    #die("Date field can have only Numbers: " . $field[13] . " Line " . $lineCount);
		  print("Field # 14 Date field can have only Numbers: " . $field[13] . " Line " . $lineCount);
		}
		
		# Check integrity test Assigned_by only if Assigned_by exists.
		# according to GO Spec. Assigned_by needs to exists but some field are missing it.
		if (length($field[14]) > 0)
		{
		    if(getDbNameIdFromHash($field[14]) == -1)
		    {
			  print "field#15 is bad: $field[14]\n";
		      $flagExit = 1; $flagError = 1;
		        #die("Could not found assigned_by name: " . $field[14] . " abbreviation id into goa_xdb. Line " .$lineCount . "\n");
		      print("Field # 15 Could not find assigned_by name: " . $field[14] . " abbreviation id into goa_xdb. Line " .$lineCount . "\n");
		    }
		}


		#if ($flagExit == 1)
		#{
		#    last;
		#}
		# Create more Checkings for different fields.

	    }
	   # else
	   # {
	    if ($flagExit == 1)
	    {
	        #die("Can't process line# " . $lineCount . "\n" . $line1 . "\nHave: " . scalar(@field) . " fileds. Only 15 fields are allow.\n\n\n\n\n\n");
	      print("Can't process line# " . $lineCount . "\n" . $line1 . "\n\n"); #Have: " . scalar(@field) . " fileds. Only 15 fields are allow.\n\n");
	      $errorCount++;
	      $flagExit = 0;
	    }
	   # }
	   # if ($flagExit == 1)
	   # {
	   #	last;
	   # }
	}
    }
#    SHoundFini();
    if ($flagError == 1)
    {
#       die("Can't process " . $errorCount . " records, from " . $lineCount . "\n");
	print("Can't process " . $errorCount . " records, from " . $lineCount . "\n");
	
    }
    print("File Ok, " . $lineCount . " to process.\n");
    close(FP1);
    print("End Integrity Test ......................\n");
    die("End Integrity Test ......................\n");

}

sub writeFixFile($)
{
    open(OUTFILE, ">file.dat") or die ("Can't find file.dat\n");
    open(FP1, $_[0]) or die "can't open given file: $!";
    $lineCount = 0;
    $flagExit = 0;

#    SHoundInit("TRUE", "foo");

    # Fields 1, 2, 3, 5, 6, 7, 9, 12, 13, 14, 15. needs to be present.
    # Field 5 Can't be NULL but since some Records are Missing Reference
    #         Will by pass this rule and put Field5= PMID:unpublushed
    @fieldToCheck = (0, 1, 2, 4, 6, 8, 11, 12, 13);

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
	        $db               = $field[0];    #     print("db: "                . $field[0] . "\n");
		$db_object_id     = $field[1];    #     print("db_object_id: "      . $field[1] . "\n");
		$db_object_symbol = $field[2];    #     print("db_object_symbol: "  . $field[2] . "\n");
		$db_not           = $field[3];    #     print("not: "               . $field[3] . "\n");
		$goid             = $field[4];    #     print("goid: "              . $field[4] . "\n");
		$db_reference     = $field[5];    #     print("db_reference: "      . $field[5] . "\n");
		$evidence         = $field[6];    #     print("evidence: "          . $field[6] . "\n");
		$with_from        = $field[7];    #     print("with_from: "         . $field[7] . "\n");
		$aspect           = $field[8];    #     print("aspect: "            . $field[8] . "\n");
		$db_object_name   = $field[9];    #     print("db_object_name: "    . $field[9] . "\n");
		$synonym          = $field[10];   #     print("synonym: "           . $field[10]. "\n");
		$db_object_type   = $field[11];   #     print("db_object_type: "    . $field[11]. "\n");
		$taxon            = $field[12];   #     print("taxon: "             . $field[12]. "\n");
		$date             = $field[13];   #     print("date: "              . $field[13]. "\n");
		$assigned_by      = $field[14];   #     print("assigned_by: "       . $field[14]. "\n");
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
	
#Make sure that the case is correct:

	$tag =~ s/UNIPROT/UniProt/;
	$tag =~ s/GENEDB_SPOMBE/GeneDB_Spombe/;
	$tag =~ s/AGRICOLA/bib/;
	$tag =~ s/Tair/TAIR/;

    if (index($_[0], ":") > 0)
    {
        if ( (length(substr($_[0], 0, index($_[0], ":", 0))) < 1) || (length(substr($_[0], index($_[0], ":", 0)+1)) < 1) )
	{
	    $gMessage = "Tag: " . $tag . "|" . substr($_[0], index($_[0], ":", 0)+1) . " doesn't have the format xxxx:xxxxx";
	    return "F";
	}

	if (!$hash_abbr{$tag} && !($tag =~ /GOC/) )
	{
#	    print("    Tag: " . $tag . " doesn't have an entry into goa_xdb.\n");
	    $gMessage = "Tag: " . $tag . " doesn't have an entry into goa_xdb.";
	    return "F";
	}
	elsif ($tag =~ /GOC/ )
	{
	  if( !$hash_abbr{"GOC:unpublished"} )
	  {
		return "F";
	  }
	}	
	return "T";
    }
    else
    {
        $gMessage = "Missing Tag:Value Pair $tag";
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

    $value = substr($_[0], index($_[0], ":", 0)+1);
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





# Reset tables: goa_seq_dbxref, goa_association, goa_reference, goa_with.
# Check if Table exist before trying to create it.
sub resetTables()
{

  $sth = $dbh->prepare("drop table goa_seq_dbxref");
  $sth->execute();

   $sth = $dbh->prepare("CREATE TABLE " . $database . ".goa_seq_dbxref ( id INTEGER UNSIGNED NOT NULL PRIMARY KEY AUTO_INCREMENT, goa_xdb_id INTEGER UNSIGNED NOT NULL, xref_key VARCHAR(30), taxid_1 INTEGER NOT NULL, taxid_2 INTEGER, type_id VARCHAR(20) NOT NULL, symbol VARCHAR(30) NOT NULL, full_name VARCHAR(255), synonym VARCHAR(50), Index i_xref_key(xref_key) )");
   $sth->execute();

  $sth = $dbh->prepare("drop table goa_association");
  $sth->execute();
   $sth = $dbh->prepare("CREATE TABLE " . $database . ".goa_association ( id INTEGER UNSIGNED NOT NULL PRIMARY KEY AUTO_INCREMENT, is_not CHAR(1), go_id INTEGER UNSIGNED NOT NULL, goa_seq_dbxref_id INTEGER UNSIGNED NOT NULL, assigned_by INTEGER UNSIGNED NOT NULL,code CHAR(4) NOT NULL, date CHAR(8) )");
   $sth->execute();	

  $sth = $dbh->prepare("drop table goa_reference");
  $sth->execute();
   $sth = $dbh->prepare("CREATE TABLE " . $database . ".goa_reference ( id INTEGER UNSIGNED NOT NULL PRIMARY KEY AUTO_INCREMENT, goa_association_id INTEGER UNSIGNED NOT NULL, goa_xdb_id INTEGER UNSIGNED NOT NULL, xref_key VARCHAR(20) NOT NULL)");
   $sth->execute();

  $sth = $dbh->prepare("drop table goa_with");
  $sth->execute();
   $sth = $dbh->prepare("CREATE TABLE " . $database . ".goa_with ( id INTEGER UNSIGNED NOT NULL PRIMARY KEY AUTO_INCREMENT, goa_association_id INTEGER UNSIGNED NOT NULL, goa_xdb_id INTEGER UNSIGNED NOT NULL, xref_key VARCHAR(20) NOT NULL, key_type INTEGER UNSIGNED NOT NULL, goa_seq_dbxref_id INTEGER UNSIGNED NOT NULL )");
   $sth->execute();

}
