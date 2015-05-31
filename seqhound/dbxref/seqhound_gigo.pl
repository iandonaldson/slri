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

#print("gigo_fill_all\n");

my $countGI = 0;
my $flagProt = 0;
my $access = 0;
my $flagRedund = 0;

#createDbHashTable($database, $dbh);

my %dbtable = createDbHashTable($database, $dbh);

#my %dbConv = createDbConvHashTable($database, $dbh);
#myPrintHash(\%dbConv);

#print("3: " . $dbtable{3}->source_db() . "\t".$dbtable{3}->isprimary_db() . "\n" );

my $sthgigo = $dbh->prepare("SELECT id, record_id FROM ". $database . ".dbxref WHERE source_id=2 AND id>1503400 AND id<1503450;");

$sthgigo->execute();
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
my %hash_go_xref = ();

while (my @rowdbxref = $sthgigo->fetchrow_array())
{ 
#    print("==========================================================\n");
    my $id      = $rowdbxref[0];
    my $access  = $rowdbxref[1];
    my $gi      = myACC2GI($access);
    my $moltype = myGI2MOLTYPE($gi);
    my $dnarna = ();
    my $prot = ();
    my $BestProt = ();
    %hash_proteins = ();
    %hash_bestproteins = ();
    %hash_sourcedb = ();
    %hash_go = ();
    %hash_go_xref = ();

#    print("id: " . $rowdbxref[0] . "\trecord_id: " . $rowdbxref[1] . "\tGI: " . $gi . "\tmoltype: " .$moltype ."\n");

    if ($moltype eq 'DNA' or $moltype eq 'RNA')
    {
      $dnarna = $gi;
        $prot = myDNA2PROT($gi, \%hash_proteins);
#	myPrintHash(\%hash_proteins);
	if($moltype eq 'DNA')
	{
	    if (myIsChromOrCompGenome($access) == 1)
	    {
	        print("[ERROR] Chrom or Complete Genome: " . $access . "\n");
	        next;
	    }
#	    print("\tdna: " . $dnarna . "\tprot: "); 
	}
	else
	{
#	   print("\trna: " . $dnarna . "\tprot: ");
	}
#	myPrintHash(\%hash_proteins);
	myGetBestProteins(\%hash_proteins, \%hash_bestproteins);
#	printf("\tBest NR Proteins: ");
#	myPrintHash(\%hash_bestproteins);
    }
    elsif($moltype eq 'protein')
    {
        #$BestProt = myBestProt4RedundGroup($gi);
        myGetBestNR4Protein($gi, \%hash_bestproteins);
#	print("\tprot: " . $gi . "\tBestprot: ");  myPrintHash(\%hash_bestproteins);
    }

    # Check if the number of proteins is grater that 5 or 10 then abort because the DNA may be from a Contig.
#    if(myNumOfElemInHash(\%hash_proteins) == 4)
#    {
#       print("[DNA4P] DNA Encode  4 proteins: " . $gi . "\taccess: " . $access . "\n");
#     }
    if(myNumOfElemInHash(\%hash_proteins) > 1)
    {
       print("[ERROR] DNA Encode more than 1 proteins: " . $gi . "\taccess: " . $access . "\n");
       next;
    }


    my $sthdbxref = $dbh->prepare("select id, source_id, record_id, parent_id from ". $database . ".dbxref where record_id='" . $access . "';");
    $sthdbxref->execute();
    my $parent_id = 0;
    my $record_id = 0;
    my $id = 0;
    my $sthdbxref2;
    while(my @rowdbxref = $sthdbxref->fetchrow_array())
    {
#        print("\t\tid : ". $rowdbxref[0] . "\tsource_db: " . $rowdbxref[1] . "\trecord_id: " . $rowdbxref[2] . "\tparent_id: " .$rowdbxref[3] . "\n");
	$id = $rowdbxref[0];
	$record_id = $rowdbxref[2];
	$parent_id = $rowdbxref[3];
	if ( $parent_id == 0)       # it's a Parent
	{
	    if ( exists($hash_sourcedb{$record_id})){}
	    else { $hash_sourcedb{$record_id} = 1; }
	    $sthdbxref2 = $dbh->prepare("select b.* from dbxref b, dbxrefsourcedb c where c.source_id=b.source_id and c.isprimary_db=1 and b.parent_id='" . $id . "';");
	    $sthdbxref2->execute();
	}
	else                        # it's a Child
	{
	    # because I'm a child I need to grap my parent.
	    my $sthdbxref1 = $dbh->prepare("select record_id from dbxref where id='" . $parent_id . "';");
	    $sthdbxref1->execute();
	    if(my @rowParentID = $sthdbxref1->fetchrow_array())
	    {
	         if ( exists($hash_sourcedb{$rowParentID[0]})){}
		 else { $hash_sourcedb{$rowParentID[0]} = 1; }
	    }
	    # get all my brothers that are primary Databases IDs.
	    $sthdbxref2 = $dbh->prepare("select b.* from dbxref b, dbxrefsourcedb c where c.source_id=b.source_id and c.isprimary_db=1 and b.parent_id='" . $parent_id . "';");
	    $sthdbxref2->execute();
	}
	while (my @rowdbxref2 = $sthdbxref2->fetchrow_array())
	{
#	    print("\t\t\tid : ". $rowdbxref2[0] . "\tsource_db: " . $rowdbxref2[1] . "\trecord_id: " . $rowdbxref2[2] . "\tparent_id: " .$rowdbxref2[3] . "\n");
	    if ( exists($hash_sourcedb{$rowdbxref2[2]})){}
	    else { $hash_sourcedb{$rowdbxref2[2]} = 1; }
	}
    }

#      # Don't Forget to process COMPUGEN. By GI.	print("\tredund: " . $rowredund[0] . "\t" . $rowredund[1] . "\n");
#    my $compugene = myGetCompugenObjectID(\%hash_proteins, \%hash_bestproteins);
#    print("\t\t\tComputGene: " . $compugene . "\n");
#
#    if ( exists($hash_sourcedb{$compugene})){}
#    else { $hash_sourcedb{$compugene} = 1; }
#    print("\t\tPrimary DB IDs: "); myPrintHash(\%hash_sourcedb);



    #print key ==> value from %hash_sourcedb;
#    print("printing brothers and parents that are Object_ids \n");
    my @keys = keys(%hash_sourcedb);
    while(my $key = pop(@keys) )
    {
#        print("$key => $hash_sourcedb{$key}\n");
	my $sthgoa = $dbh->prepare("select distinct a.go_id, a.code, b.goa_xdb_id, b.xref_key from goa_association a, goa_seq_dbxref b where a.goa_seq_dbxref_id =b.id and b.xref_key='" . $key . "';");
	$sthgoa->execute();
	while (my @goa = $sthgoa->fetchrow_array())
	{
	    my $goid = $goa[0];
	    my $code = $goa[1];
	    my $xdb  = $goa[2];
	    my $xref = $goa[3];
#	    print("\tGO: " . $goid ." ". $code . " ". $xdb . " ". $xref . "\n");
	    if ( exists($hash_go{$goid.":".$code})){}
	    else { $hash_go{$goid.":".$code} = 1; }
	
	    if ( exists($hash_go_xref{$goid})){}
	    else { $hash_go_xref{$goid} = $xdb.";".$xref; }
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
#	print("\n");
    }
#    print("\nGO: ");  myPrintHash(\%hash_go);

    
    # Will put the DNA sequence into the hash_bestproteins to be able to generate the list of gi-go.
    if ( $moltype eq 'DNA' or $moltype eq 'RNA')
    {
        if ( exists($hash_bestproteins{$dnarna})){}
	else { $hash_bestproteins{$dnarna} = 1; }
    }
    my @keysBestProteins = keys(%hash_bestproteins);
    while(my $keyBP = pop(@keysBestProteins) )
    {
        my @keysGO = keys(%hash_go);
	
        while( my $keyGO = pop(@keysGO) )
	  {
	      $keyGO=~ /(\d+):(\w+)/;
	      my $go = $1;
	      my $code = $2;
	      my $keyxdbxref = $hash_go_xref{$go};
	      $keyxdbxref =~ /(\d+);(\S+)/;
	      my $xdb  = $1;
	      my $xref = $2;
#	      print($keyBP . " " . $go . " " . $code . " " . $xdb . " " . $xref . "\n");
	      if ( myIsGiGo($keyBP, $go) == 0)
	      {
		  my $sthgigogoaseq = $dbh->prepare("INSERT IGNORE INTO " . $database . "." . $tablegigo . " (gi, go, code, xdb_id, xref_key) VALUES (" . $keyBP . ", " . $go . ", '" . $code ."', " . $xdb . ", '" .$xref . "');");
		  $sthgigogoaseq->execute();
	      }
	      else
	      {
#		  print("\t\tAlready Exists\n");
	      }
	}
    }
}


print("countGI: " . $countGI . "\n");

$sthgigo->finish();
#$sthgoaseqdbxref2->finish();
#$sthgigoUpd->finish();
$dbh->disconnect();
exit(0);


sub myACC2GI($)
{
    my $sthAcc2Gi = $dbhII->prepare("select gi from accdb where access='" . $_[0] . "';");
    $sthAcc2Gi->execute();
    if ( my @rowGi = $sthAcc2Gi->fetchrow_array() )
    {
        return $rowGi[0];
    }
    return -1;
}

sub myGI2MOLTYPE($)
{
     my $sthGi2MolType = $dbhII->prepare("select type from taxgi where gi=" . $_[0] . ";");
    $sthGi2MolType->execute();
    if ( my @rowMolType = $sthGi2MolType->fetchrow_array() )
    {
        return $rowMolType[0];
    }
    return -1;
}
sub myDNA2PROT($, $)
{
     my $sthDna2Prot = $dbhII->prepare("select gia from nucprot where gin=" . $_[0] . ";");
    $sthDna2Prot->execute();
    while ( my @rowProt = $sthDna2Prot->fetchrow_array() )
    {
         if ( exists(${$_[1]}{$rowProt[0]})){}
	 else { ${$_[1]}{$rowProt[0]} = 1; }
    }
    return 1;
}

sub myBestProt4RedundGroup($)
{
#    my $sthBestProt = $dbhII->prepare("select b.gi from redund a, redund b where b.rgroup = a.rgroup and b.rordinal=1 and a.gi =" . $_[0] . ";");
    my $sthBestProt = $dbhII->prepare("SELECT b.gi FROM redund a, redund b, taxgi c, taxgi d WHERE b.rgroup = a.rgroup AND a.gi=c.gi AND b.gi=d.gi AND c.taxid=d.taxid AND a.gi =" . $_[0] . " limit 1;");
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
    my $sthGiGo = $dbh->prepare("select * from " . $tablegigo ." where gi=" . $_[0] . " and go=" . $_[1] . ";");
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

# Function that creats a Convertion hash table 
# between dbxrefsourcedb.source_id --> goa_xdb.id
# with an object that represents dbabbrev. and 
# ifprimary flag with 1 or 0 if it's primary db or not.
# $_[0] database.
# $_[1] dbh.
#sub createDbConvHashTable($, $)
#{
#    my $sthdb;
#    my %hash_db;
#    $sthdb = $_[1]->prepare("SELECT dbxref_source_id, xdb_id from " . $_[0] . ".goa_db_conv");
#    $sthdb->execute();
#    while(my @row = $sthdb->fetchrow_array())
#    {
#        my $dbrec = new DBrecord;
##	$dbrec->setDBrecord($row[1], $row[2]);
#	$hash_db{$row[0]} = $row[2];
#    }
#    return %hash_db;
#}
