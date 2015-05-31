#!/usr/bin/perl
# Program that counts how many records are in a line base flat file.
# And generate the shell script to run in the cluster.

# ARGV[0]: XREF_{DATABAE} ej. XREF_LL that point to the LocusLink flat file loc2ref.
#
# Return:
# Requirements dbxref.pm module.

use strict;
use DBI;
use Getopt::Long;
use dbxref;

my $data_dir   = dbxref->getProperty("dbxref.ini", "data_dir");

# Establish a connection a database where is DBXref, GOA, GOA_GIGO tables.
my $host      = dbxref->getProperty("dbxref.ini", "host");
my $port      = dbxref->getProperty("dbxref.ini", "port");
my $database  = dbxref->getProperty("dbxref.ini", "database");
my $tabledbxref= dbxref->getProperty("dbxref.ini", "table");
my $tablegigo = dbxref->getProperty("dbxref.ini", "tablegigo");
my $user_name = dbxref->getProperty("dbxref.ini", "user");
my $password  = dbxref->getProperty("dbxref.ini", "password");

my $cmd = $ARGV[0] . "_CMD2RUN";
$cmd = "GIGO_" . $cmd;

my $command2exec = dbxref->getProperty("dbxref.ini", $cmd);



my $dsn = "DBI:mysql:" . $database . ":" . $host . ":" . $port;
my $dbh = DBI->connect ($dsn, $user_name, $password, {RaiseError => 1});


my $workingdir = dbxref->getProperty("dbxref.ini", 'working_dir');
#print("file2parse: $file2parse\n");


#Dummy variable CLUSTER: this refer to a System Variable.
my $CLUSTER = "\$CLUSTER";

# execute a system call and return the output into $liveNodes.
my $liveNodes = `echo $CLUSTER`;


my $db = $ARGV[0];
my $retValue = 0;
$retValue = GetOptions('d=s' => \$db );

if( ($db ne 'GB') and ($db ne 'SP') )
{
      print("Usage : perl generate_gigo_run.pl -d\n");
      print("\tWhere:\n");
      print("\t\t-d Datbase: SP = Swissprot | GB = Genbank\n");
      die("1 input arguments is needed: Database\n");
}

my $sthdbxref;
if ($db eq "GB")
{
   $sthdbxref = $dbh->prepare("SELECT count(id) FROM ". $database . "." . $tabledbxref. " WHERE source_id=2;");
}
if ($db eq "SP")
{
   $sthdbxref = $dbh->prepare("SELECT count(id) FROM ". $database . "." . $tabledbxref. " WHERE source_id=12 AND parent_id=0;");
}

$sthdbxref->execute();

my $numOfRec = 0;

if (my @countRec = $sthdbxref->fetchrow_array())
{
    $numOfRec = $countRec[0];
}
else
{
    die "Database $db needs to have records.\n";
}
if ($numOfRec <= 0)
{
    die "Database $db needs to have records.\n";
}
open(FP1, $liveNodes) or die "can't open given file: $!";


#my $numOfNodes = `wc -l ${CLUSTER} | awk '{print $1}'`;

my $record = ();
my $sep = ();
my $line = ();


my $numOfNodes = countNumOfLines($liveNodes);

$line  = ();

# A: Num. of Records.
# B: Num. of Nodes.
#
# A/B = C
# C*B = D
# A-D = F   if (F == 0) use C * B
#           if (F > 0 ) use C * (B-1) + (C+F)
# C+F = G
#

my $useCtimesB;
my $intNode = 90;
my $A = $numOfRec;
my $B = $numOfNodes;
my $C = int($A / $B);
#print("C: $C\n");

my $D = $C * $B;
#print("D: $D\n");

my $F = $A - $D;
#print("F: $F\n");

if ( $F == 0)
{
#  print("F = 0\n");
#  print("useCtimesB = 1\n");
  $useCtimesB = 1;
}
elsif ( $F > 0 )
{
#   print("F > 0\n");
#   print("useCtimesB = 0\n");
   $useCtimesB = 0;
}
else
{
  print("F: $F\n");
  die("Error F can't be negative");
}

# Debug Info.
#print("A: $A\n");
#print("B: $B\n");
#print("C: $C\n");
#print("D: $D\n");
#print("F: $F\n");
#print("useCtimesB = $useCtimesB\n");

my $RecFrom = 0;
print("#! /usr/bin/bash\n");
my @nodes = ();
for(my $i = 0; $i<$numOfNodes; $i++)
{
    my $node = <FP1>;
    chomp($node);
    push(@nodes, $node);
    print("rsh $node \"cd ".$workingdir. "; " . $command2exec . " -d $db -c T -o $RecFrom -n $C -f $node.dat >> " . $workingdir . "/xref_parser.log 2>&1 &\"\n");

    $RecFrom = $RecFrom + $C;
    if( $i < $numOfNodes-2 )
    {
       $intNode++;
    }
    elsif($useCtimesB == 0)
    {
       $intNode++;
       $C = $C + $F;
    }
    elsif($useCtimesB == 1)
    {
       $intNode++;
    }
}
print ("# Num of records: " . $numOfRec . "\n");

system("chmod 754 runSeqhound_gigo_$ARGV[0].sh");

print("./wait.pl ");
foreach(@nodes)
{   print("$_.dat ");   }
print("\n");

print("rm ");
foreach(@nodes)
{   print("$_.dat ");   }
print("\n");


close(FP1);

sub countNumOfLines($)
{
  my $line = ();
  my $numOfRec = 0;
  open(FPLiveNodes, $_[0]) or die "can't open given file: $!";
  while ($line = <FPLiveNodes>)
  {
      chomp($line);
      $numOfRec++;
  }
  close(FPLiveNodes);
  return $numOfRec;
}
