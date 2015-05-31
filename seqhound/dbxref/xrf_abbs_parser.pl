# two choises are available - to use DBD-mysql or to use DBI to connect to
# MySQL-db.
# DBI is a most universal one (DBD-mysql is too specific)
use DBI;
use dbxref;
# RAC Read this parameters from a propertie files.

my $user_name = dbxref->getProperty("dbxref.ini", "user");
my $password  = dbxref->getProperty("dbxref.ini", "password");
my $host      = dbxref->getProperty("dbxref.ini", "host");
my $port      = dbxref->getProperty("dbxref.ini", "port");
my $database  = dbxref->getProperty("dbxref.ini", "database");
my $file2parse= dbxref->getProperty("dbxref.ini", "abbs_goa");
my $abbs_url = dbxref->getProperty("dbxref.ini", "abbs_goa_url");

my $dsn = "DBI:mysql:$database:$host:$port";


my $dbh, $sth;
my @ary;


# 2) connect to MySQL-database
$dbh = DBI->connect ($dsn, $user_name, $password, {RaiseError => 1});

# 1) process GO.xrf_abbs Cross Reference database.

# RAC Read the file name from command line.

# Reset Tables.
resetTables();

#remove the file first
system ("rm $file2parse ");
system("wget $abbs_url");

open(FP, $file2parse) or die "can't open given file: $!";

$count = 0;                             # Count the number of records

while ($line = <FP>)
{
#    print("line: " .$line);

    if ("\n" eq substr($line, 0, 1))
    {
#        print("Enter \n");
    }
    $last = "";
    chomp($line);
    while ($line = <FP>)
    {
#        print("line: " .$line);
        if ("!" ne substr($line, 0, 1))
        {
	    if ("\n" eq substr($line, 0, 1) or " " eq substr($line, 0, 1) )
	    {
	        if (length($abbrev) == 0)
		{
		    next;
		}
#	        print("Enter \n");
#		print("abbrev: " . $abbrev . "\n");
#		print("length: " . length($abbrev) . "\n");
#		print("databs: " . $databs . "\n");
#		print("object: " . $object . "\n");
#		print("exampl: " . $exampl . "\n");
#		print("genurl: " . $genurl . "\n");
#		print("urlsyn: " . $urlsyn . "\n");
#		print("urlexp: " . $urlexp . "\n");
		$sth = $dbh->prepare ("INSERT INTO " . $database . ".goa_xdb VALUES(0, '" . $abbrev . "', ' " . $databs . "', '" . $object . "', '" . $exampl . "', '" . $genurl . "', '" . $urlsyn . "', '" . $urlexp . "')" );
		$sth->execute();
		
		$count++;
		# Append synonyms if any.
		while (@synonym)  
		{
		    $synabbr = pop(@synonym);

		    # This is becouse some synonyms abbreviations have some explanations in the same line
                    # separated by " " (space) for this reason we need to remove what is at the rigth of
                    # of the " ".
		    if (index($synabbr, " ", 0) > -1)
                    {
		        $synabbr = substr($synabbr, 0, index($synabbr, " ", 0));
		    }
		    $sth = $dbh->prepare ("INSERT INTO " . $database . ".goa_xdb VALUES(0, '" .$synabbr  . "', ' " . $databs . "', '" . $object . "', '" . $exampl . "', '" . $genurl . "', '" . $urlsyn . "', '" . $urlexp . "')" );
		    $sth->execute();
		    $count++;
#		    print("adding synonym\n");
		}
	
		$abbrev = "";
		$databs = "";
		$object = "";
		$exampl = "";
		$genurl = "";
		$urlsyn = "";
		$urlexp = "";
		$last = "";
		@synonym = ();
#		print("\n");
	    }
	    chomp($line);
	    $line =~ s/'/\\'/g;
	    if ("abbreviation: " eq substr($line, 0, length("abbreviation: ")))
	    {
	        $abbrev = substr($line, length("abbreviation: "));
	    }
	    elsif ("database: " eq substr($line, 0, length("database: ")))
	    {
	        $databs = substr($line, length("database: "));
		$last = "database";
	    }
	    elsif ("object: " eq substr($line, 0, length("object: ")))
	    {
	        $object = substr($line, length("object: "));
	    }
	    elsif ("example_id: " eq substr($line, 0, length("example_id: ")))
	    {
	        $exampl = substr($line, length("example_id: "));
	    }
	    elsif ("generic_url: " eq substr($line, 0, length("generic_url: ")))
	    {
	        $genurl = substr($line, length("generic_url: "));
	    }
	    elsif ("url_syntax: " eq substr($line, 0, length("url_syntax: ")))
	    {
	        $urlsyn = substr($line, length("url_syntax: "));
	    }
	    elsif ("url_example: " eq substr($line, 0, length("url_example: ")))
	    {
	        $urlexp = substr($line, length("url_example: "));
	    }
	    elsif ("synonym: " eq substr($line, 0, length("synonym: ")))
	    {
	       # if the abbreviation has a synonym, all synonims needs to be acumulated. For further
               # apdates into goa_xdb.
	       push(@synonym, substr($line, length("synonym: ")));
#	       print("synonym: " . substr($line, length("synonym: ")) . "\n");
	       $last = "synonym";
	    }
	    else
	    {
	        # database value is sometime in multiple lines one after the other for this reason
	        # if the program reach this else statment and $last eq "database" the database name
	        # needs to be acomulated into $databs.
	        if($last eq "database")
		{
		  $databs = $databs . $line;
		}
            }
	}
    }
#    print("Enter \n");
#    print("abbrev: " . $abbrev . "\n");
#    print("databs: " . $databs . "\n");
#    print("object: " . $object . "\n");
#    print("exampl: " . $exampl . "\n");
#    print("genurl: " . $genurl . "\n");
#    print("urlsyn: " . $urlsyn . "\n");
#    print("urlexp: " . $urlexp . "\n");

    # Insert the last record.
    $sth = $dbh->prepare ("INSERT INTO " . $database . ".goa_xdb VALUES(0, '" . $abbrev . "', ' " . $databs . "', '" . $object . "', '" . $exampl . "', '" . $genurl . "', '" . $urlsyn . "', '" . $urlexp . "')" );
    $sth->execute();
    $count++;
}
close(FP);


$sth->finish();
$dbh->disconnect();
print($count . " records Updated into goa_xdf database.\n");
exit(0);


sub resetTables()
{

   $sth = $dbh->prepare("drop table IF EXISTS goa_xdb");
   $sth->execute();
   $sth = $dbh->prepare("CREATE TABLE IF NOT EXISTS " . $database . ".goa_xdb (id INTEGER NOT NULL PRIMARY KEY AUTO_INCREMENT, abbreviation VARCHAR(50), name VARCHAR(255), object VARCHAR(255), example VARCHAR(50), generic_url VARCHAR(255), url_syntax VARCHAR(255), url_example VARCHAR(255) );");
   $sth->execute();
 }
