# Process file missing_go.txt but form mysql gigo table.
# GI's bind list with missing GO terms.
use strict;
# two choises are available - to use DBD-mysql or to use DBI to connect to
# MySQL-db.
# DBI is a most universal one (DBD-mysql is too specific)
use DBI;
use SeqHound;
use dbxref;

my $dsn = "DBI:mysql:seqhound:localhost";

# Establish a connection to a temporary database to create DBXref, GOA, GOA_GIGO tables
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


my $dsn = "DBI:mysql:" . $database . ":" . $host . ":" . $port;
my $dsnII = "DBI:mysql:" . $database2 . ":" . $host2 . ":" . $port2;


# 1) connect to MySQL-database
#print("Connect to db: " .  $database . ":" . $host . "\n");
my $dbh = DBI->connect ($dsn, $user_name, $password, {RaiseError => 1});
my $dbhII = DBI->connect ($dsnII, $user_name2, $password2, {RaiseError => 1});


my $flagRedGo = 0;

my $countGI = 0;
my $gi = 0;
# RAC read '3' from a property file. CGEN.
my $sthCGEN = $dbh->prepare("select id, symbol, goa_xdb_id, xref_key from goa_seq_dbxref WHERE goa_xdb_id = 3 and symbol like 'GI%';");
$sthCGEN->execute();

while (my @rowCGEN = $sthCGEN->fetchrow_array())
{
    my $id = $rowCGEN[0];
    my $symbol = $rowCGEN[1];
    my $xdb_id = $rowCGEN[2];
    my $xref_key = $rowCGEN[3];
    $gi = $symbol;

    $gi =~ /(GI)(\d+)/;
    $gi= $2;

    if ($gi > 0)
    {

      # Get a Redundand GI.

        my $NRgi = myBestProt4RedundGroup($gi);
	if ($NRgi == -1)
	{
	    $NRgi = $gi;
	}

	my $sthCGENGO = $dbh->prepare("select distinct go_id, code  from goa_association WHERE goa_seq_dbxref_id = " . $id);
	$sthCGENGO->execute();
	while(my @rowCGENGO = $sthCGENGO->fetchrow_array())
	{
	    my $go_id = $rowCGENGO[0];
	    my $code  = $rowCGENGO[1];
#           print($NRgi     . "\t");
#	    print($go_id    . "\t");           # GO
#	    print($code     . "\t");           # Evidence CODE
#	    print($xdb_id   . "\t");           # DataBase ID
#	    print($xref_key . "\n");           # Cross Reference ID (Object_id).

	    my $sthgigogoaseq = $dbh->prepare("INSERT IGNORE INTO " . $database . "." . $tablegigo . " (gi, go, code, xdb_id, xref_key) VALUES (" . $NRgi . ", " . $go_id . ", '" . $code ."', " . $xdb_id . ", '" .$xref_key . "');");
	    $sthgigogoaseq->execute();
	}
    }
    else
    {
      next;
    }
}
exit(0);


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
