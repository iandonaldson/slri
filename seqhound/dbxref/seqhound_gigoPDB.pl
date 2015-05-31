# Process Table gigo.
#
# Update GOASEQID field 
# lookup gigo.gi
# if found
#    gigo.goaseqid <- goa_seq_dbxref.id.

use strict;

# two choises are available - to use DBD-mysql or to use DBI to connect to
# MySQL-db.
# DBI is a most universal one (DBD-mysql is too specific)
use DBI;
use SeqHound;

#use lib "/home/renan/programs/perl/bind/DBrecord";       #Class defined in dbrecord.pm
use DBrecord;
use dbxref;
#use warnings;

#my $dsn = "DBI:mysql:seqhound:localhost";
# Establish a connection to a temporary database to create DBXref, GOA, GOA_GIGO tables.
my $host      = dbxref->getProperty("dbxref.ini", "host");
my $port      = dbxref->getProperty("dbxref.ini", "port");
my $database  = dbxref->getProperty("dbxref.ini", "database");
my $tablegigo = dbxref->getProperty("dbxref.ini", "tablegigo");
my $user_name = dbxref->getProperty("dbxref.ini", "user");
my $password  = dbxref->getProperty("dbxref.ini", "password");

# Establish a connection to query seqhound.
my $host2      = dbxref->getProperty("dbxref.ini", "hostshound");
my $port2      = dbxref->getProperty("dbxref.ini", "portshound");
my $database2  = dbxref->getProperty("dbxref.ini", "databaseshound");
my $user_name2 = dbxref->getProperty("dbxref.ini", "usershound");
my $password2  = dbxref->getProperty("dbxref.ini", "passwordshound");


# //my $dsn = "DBI:mysql:swiss_prot:localhost";
my $dsn = "DBI:mysql:" . $database . ":" . $host . ":" . $port;
my $dsnII = "DBI:mysql:" . $database2 . ":" . $host2 . ":" . $port2;

#$dbh, $sthg, $sthdbx, $sthgo, $sthredund, $sthgigo, $sthgoaseqdbxref, $sthgigoUpd;

# 1) connect to MySQL-database
#print("Connect to db: " .  $database . ":" . $host . "\n");
my $dbh = DBI->connect ($dsn, $user_name, $password, {RaiseError => 1});
my $dbhII = DBI->connect ($dsnII, $user_name2, $password2, {RaiseError => 1});
#print("Connected\n");

#Initialize SeqHound.
#print("Connect to SHound\n");
#SHoundInit("TRUE", "foo");
#print("Connected\n");

# 2) process file missing_go.txt

#open(FP, "missing_go.txt");

#print("gigo_fill_all\n");

my $countGI = 0;
my $flagProt = 0;
my $access = 0;
my $flagRedund = 0;

#createDbHashTable($database, $dbh);

my %dbtable = createDbHashTable($database, $dbh);


#print("3: " . $dbtable{3}->source_db() . "\t".$dbtable{3}->isprimary_db() . "\n" );


# Create a HasTable for the db abbreviations and Ids from goa_xdb;.
# %abbr_id HasTable of Key: Abbreviation, Value: Ids.
# This hash table was created to avoid searching into goa_xdb each time
# an ID is needed from a particular abbreviation name.
##########################
my %hash_abbr = createAbbrHashTable($database, $dbh);

my $pdbid= $hash_abbr{"PDB"};
#print("pdbid: " . $pdbid . "\n");
my $sthgoaseqdbxref = $dbh->prepare("select xref_key, symbol from ". $database . ".goa_seq_dbxref where goa_xdb_id= " .$pdbid . ";");
#my $sthgigo = $dbh->prepare("select id, record_id from ". $database . ".dbxref where source_id=2;"); # limit 20;");
#my $sthgigo = $dbh->prepare("select id, record_id from ". $database . ".dbxref where source_id=2 and id=27721 limit 10;"); # Multiple Prot.
#my $sthgigo = $dbh->prepare("select id, record_id from ". $database . ".dbxref where source_id=2 and id=1166669 limit 10;");
$sthgoaseqdbxref->execute();
my $gi2search = 0; 
my $gigoid = 0;
my $prot = 0;

my @protArr = ();
my $flagProtFromDNARNA = 0;
my $numOfProt = 0;
my $protIdx = 0;
my %hash_proteins = ();
my %hash_bestproteins = ();
my %hash_sourcedb = ();
my %hash_go = ();
while (my @rowdbxref = $sthgoaseqdbxref->fetchrow_array())
{ 
#    print("==========================================================\n");
    my $name     = $rowdbxref[0];
    my $chain    = $rowdbxref[1];
    my $prot = ();
    my $retValue = ();
    my $BestProt = ();

    my $goid    = ();
    my $code    = ();
    my $xdb_id  = ();
    my $xrefkey = ();
    %hash_proteins = ();
    %hash_bestproteins = ();
    %hash_sourcedb = ();
    %hash_go = ();

#    print("id: " . $rowdbxref[0] . "\trecord_id: " . $rowdbxref[1] . "\tGI: " . $gi . "\tmoltype: " .$moltype ."\n");

    $retValue = myNAME2GI($name, $chain, \%hash_proteins);
    if ($retValue == -1)
    {
      next;
    }

#    myPrintHash(\%hash_proteins);

	myGetBestProteinsForPDB(\%hash_proteins, \%hash_bestproteins);
#	printf("\tBest NR Proteins: ");
#	myPrintHash(\%hash_bestproteins);
#    elsif($moltype eq 'protein')
#    {
#        #$BestProt = myBestProt4RedundGroup($gi);
#        myGetBestNR4Protein($gi, \%hash_bestproteins);
#	print("\tprot: " . $gi . "\tBestprot: ");  myPrintHash(\%hash_bestproteins);
#    }


# Review this if this information is comming from PDB why I putting goa_xdb_id from Swissprot.
# This shut be PDB.
# Ask Brigitte.

#        print("$key => $hash_sourcedb{$key}\n");
	my $sthgoa = $dbh->prepare("select distinct a.go_id, a.code, c.goa_xdb_id, c.xref_key from goa_association a, goa_seq_dbxref b, goa_with c where a.goa_seq_dbxref_id=b.id and c.goa_association_id=a.id  and b.xref_key='" . $name . "' AND symbol='". $chain . "';");
	$sthgoa->execute();
	while (my @goa = $sthgoa->fetchrow_array())
	{
	    $goid = $goa[0];
	    $code = $goa[1];
	    $xdb_id = $goa[2];
	    $xrefkey= $goa[3];

#	    print("GO: " . $goid ." ". $code . "\t".$xdb_id. "\t".$xrefkey. "\n");
	    if ( exists($hash_go{$goid.":".$code})){}
	    else { $hash_go{$goid.":".$code} = 1; }
	}

#	print("Searching  goa_seq_dbxref.xref_key\n");
#	my $sthgoaseqdbxref= $dbh->prepare("select id from goa_seq_dbxref where xref_key='" . $key . "';");
#	$sthgoaseqdbxref->execute();
#	while (my @goaseqdbxref = $sthgoaseqdbxref->fetchrow_array())
#	{
#	    print("gigoid:" . $gigoid . ":\t ID: " . $goaseqdbxref[0]. "\n");
#	    my $sthgigogoaseq = $dbh->prepare("INSERT INTO " . $database . "." . "gigogoaseq (gigo_id, goaseqdbxref_id) VALUES(" . $gigoid . "," . $goaseqdbxref[0] . ");");
#	    $sthgigogoaseq->execute();
#	}
#	print("\n\n");

#    print("\nGO: ");  myPrintHash(\%hash_go);


    my @keysBestProteins = keys(%hash_bestproteins);
    while(my $keyBP = pop(@keysBestProteins) )
    {
        my @keysGO = keys(%hash_go);
        while( my $keyGO = pop(@keysGO) )
	{
	      $keyGO=~ /(\d+):(\w+)/;
	      my $go = $1;
	      my $code = $2;
#	      print($keyBP . " " . $go . " " . $code. "\n");
	      if ( myIsGiGo($keyBP, $go) == 0)
	      {
		  my $sthgigogoaseq = $dbh->prepare("INSERT IGNORE INTO " . $tablegigo ." VALUES (" . $keyBP . ", " . $go . ", '" . $code ."', ".$xdb_id.", '".$xrefkey."' );");
		  $sthgigogoaseq->execute();
	      }
	      else
	      {
#		  print("\t\tAlready Exists\n");
	      }
	}
    }
}

=head Comment

=cut

#    $sthgigo = $dbh->prepare("INSERT INTO " . $database . "." . $table . " (id, gi, taxid) VALUES(0, " . $gi . "," . $taxid . ");");
#    $sthgigo->execute();

print("countGI: " . $countGI . "\n");

#$sthgigo->finish();
#$sthgoaseqdbxref2->finish();
#$sthgigoUpd->finish();
$dbh->disconnect();
exit(0);



# Search for a GI in ACCDB given a name and a Chain.
# if the chain does not exist search only for a name.
#
sub myNAME2GI($, $, $)
{
     my $sthNAME2GI = ();
     if ( $_[1] eq '@')
     {
#         printf("Name: " .$_[0] . "\n");
         $sthNAME2GI = $dbhII->prepare("select gi from " . $database2 . ".accdb where name='" . $_[0] . "';");
     }
     else
     {
#        printf("Name: " . $_[0] . " and Chain: " . $_[1] . "\n");
	 $sthNAME2GI = $dbhII->prepare("select gi from  " . $database2 . ".accdb where name='" . $_[0] . "' and chain='" . $_[1] . "';");
     }
	 $sthNAME2GI->execute();
	 my $numRows = $sthNAME2GI->rows();
	 if ($numRows == 0)
	 {
	     return -1;
	 }
	 #     print("count: " . $sthNAME2GI->rows() ."\n");
	 while ( my @rowProt = $sthNAME2GI->fetchrow_array() )
	 {
	     if ( exists(${$_[2]}{$rowProt[0]})){}
	     else { ${$_[2]}{$rowProt[0]} = 1; }
         }
     
    return 1;
}

sub myBestProt4RedundGroup($)
{
    my $sthBestProt = $dbhII->prepare("select b.gi from redund a, redund b where b.rgroup = a.rgroup and b.rordinal=1 and a.gi =" . $_[0] . ";");
    $sthBestProt->execute();
    if ( my @rowProt = $sthBestProt->fetchrow_array() )
    {
        return $rowProt[0];
    }
    return -1;
}


sub myIsChromOrCompGenome($)
{
    my $sthChrom = $dbhII->prepare("select * from chrom where access='" . $_[0] . "';");
    $sthChrom->execute();
    if ( my @rowChrom = $sthChrom->fetchrow_array() )
    {
        return 1;
    }
    return 0;
}

# Check if GI GO pair already exists in GIGO.
sub myIsGiGo($, $)
{
    my $sthGiGo = $dbh->prepare("select * from goa_gigo where gi=" . $_[0] . " and go=" . $_[1] . ";");
    $sthGiGo->execute();
    if ( my @rowChromGiGo = $sthGiGo->fetchrow_array() )
    {
        return 1;
    }
    return 0;
}

# Given two hash tables one for Proteins and the other for Best Anotated Proteins
# Return an Object_id from Compugen if any.
# Obs: May be multiple Object_ids for the list of proteins but for now we will handle just one 
#      the first found id.
# $_[0] List of proteins.
# $_[1] List of Best anotated proteins.
sub myGetCompugenObjectID($, $)
{
     my @keysProt = keys( %{$_[0]} );
     while (my $key = pop(@keysProt) )
     { #print("\t\t\tkey: " . $key . "\n");
          my $sthCompGen = $dbh->prepare("select xref_key from goa_seq_dbxref where symbol='GI" . $key . "';");
	  $sthCompGen->execute();
	  if ( my @rowXref_Key = $sthCompGen->fetchrow_array() )
	  {
	      return $rowXref_Key[0];
	  }
     }

     my @keysBestProt = keys( %{$_[1]} );
     while (my $key = pop(@keysBestProt) )
     {
          my $sthCompGen = $dbh->prepare("select xref_key from goa_seq_dbxref where symbol='GI" . $key . "';");
	  $sthCompGen->execute();
	  if ( my @rowXref_Key = $sthCompGen->fetchrow_array() )
	  {
	      return $rowXref_Key[0];
	  }
     }

     return 0;
}

#Create a hash table of Best Anotated protein from a hash table of Proteins.
# $_[0] List of proteins.
# $_[1] List of Best anotated proteins.
sub myGetBestProteins($, $)
{
    my @keys = keys( %{$_[0]} );
    my $BstPrt = ();
    while (my $key = pop(@keys) )
    {
        $BstPrt = myBestProt4RedundGroup($key);
	if ($BstPrt != -1)
	{
	    if ( exists(${$_[1]}{$BstPrt})){}
            else { ${$_[1]}{$BstPrt} = 1; }
        }
        $BstPrt = ();
    }
    return 1;
}

#Create a hash table of Best Anotated protein from a hash table of Proteins from PDB.
# The point here it's that some PDB Names are for DNA Structures.
# $_[0] List of proteins.
# $_[1] List of Best anotated proteins.
sub myGetBestProteinsForPDB($, $)
{
    my @keys = keys( %{$_[0]} );
    my $BstPrt = ();
    while (my $key = pop(@keys) )
    {
        $BstPrt = myBestProt4RedundGroup($key);
	if ($BstPrt != -1)
	{
	    if ( exists(${$_[1]}{$BstPrt})){}
            else { ${$_[1]}{$BstPrt} = 1; }
        }
        else
        {
	    if ( exists(${$_[1]}{$key})){}
            else { ${$_[1]}{$key} = 1; }
        }
        $BstPrt = ();
    }
    return 1;
}

sub myGetBestNR4Protein($, $)
{
        my $BstPrt = myBestProt4RedundGroup($_[0]);
	if ($BstPrt != -1)
	{
	    if ( exists(${$_[1]}{$BstPrt})){}
            else { ${$_[1]}{$BstPrt} = 1; }
            return 1;
        }
    return -1;
}

sub myNumOfElemInHash($)
{
  my @keys = keys( %{$_[0]});
  return scalar(@keys);
}

sub myPrintHash($)
{
    my @keys = keys( %{$_[0]});
    while (my $key = pop(@keys) )
    {
        print("\t" . $key);
    }
    print("\n");
}


sub mySHoundAccFromGi($)
{
    my $acc = SHoundAccFromGi( $_[0] );
    if ($acc eq 'n/a')
    {
        $acc = SHoundGetNameByGi( $_[0] );
	print(".");
    }
    if ($acc eq 0)
    {
        #print("NA: ");
    }
    return ($acc)
}



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
        my $dbrec = new DBrecord;
   
	$dbrec->setDBrecord($row[1], $row[2]);
	$hash_db{$row[0]} = $dbrec;
    }
    return %hash_db;
}


# Function that creates a Hast Table form the table goa_xdb
# Hash table: hash_abbr is global to the hole script.
# Key: abbreviation
# Value: goa_xdb.id
# This hash table was created to avoid searching into goa_xdb each time
# an ID is needed from a particular abbreviation name.
sub createAbbrHashTable($, $)
{
    my $sth;
    my %hash_abbr;
    $sth = $_[1]->prepare("SELECT abbreviation, id from " . $database . ".goa_xdb");
    $sth->execute();

#   print($row[0] ."\t" . $row[1] . "\n");
    
    #Crete an abreviation id hash.
    $hash_abbr{unpublished} = 0;

    while (my @row = $sth->fetchrow_array)
    {
#        print($row[0] ."\t" . $row[1] . "\n");
        $hash_abbr{$row[0]} = $row[1];
    }
    return %hash_abbr;
}
