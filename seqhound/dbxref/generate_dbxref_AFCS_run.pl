#!/usr/bin/perl

# Program that counts how many records are in "AFCS" file.
# And generate the shell script to run in the cluster.

# Arg1: AFCS tag from dbxref.ini (Where is the location of
#       the file to count.
# Arg2: Number of Cluster Nodes.
# Return: The number or records in the file.
#
# Requirements dbxref.pm module.

use strict;
use dbxref;

my $data_dir   = dbxref->getProperty("dbxref.ini", "data_dir");
my $file2parse = $data_dir . dbxref->getProperty("dbxref.ini", $ARGV[0]);
my $workingdir = dbxref->getProperty("dbxref.ini", 'working_dir');

$ARGV[0] =~ /(XREF_)(\w+)/;

my $organismCMD = $2 . '_CMD2RUN';
my $command2exec = dbxref->getProperty("dbxref.ini", $organismCMD);

#Dummy variable CLUSTER: this refer to a System Variable.
my $CLUSTER = "\$CLUSTER";

# execute a system call and return the output into $liveNodes.
my $liveNodes = `echo $CLUSTER`;

open(FP, $file2parse) or die "can't open given file: $!";
open (FP1, $liveNodes) or die "can't open given file: $!";

my $numOfRec = 0;

my $numOfNodes = countNumOfLines($liveNodes);
my $record = ();
my $sep = ();
my $line = ();

while ($line = <FP>)
{
    chomp($line);
    $record = $line;

    if ( $record =~ /^\/\//)
    {
       $numOfRec++;
#       print($record . "\n");
    }
}
# print ("Num of records: " . $numOfRec . "\n");

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
print("#!/usr/bin/bash\n");
my @nodes = ();
for(my $i = 0; $i<$numOfNodes; $i++)
{
   # Read liveNode list of alive nodes in the Cluster.
   my $node = <FP1>;
   chomp($node);
   push(@nodes, $node);
   print("rsh $node \"cd ".$workingdir."; " . $command2exec . " -c T -o $RecFrom -n $C -f $node.dat >> " .$workingdir . "/xref_parser.log 2>&1 &\"\n");

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

close(FP);
close(FP1);

print ("# Num of records: " . $numOfRec . "\n");

system("chmod 754 run$ARGV[0].sh");

print("./wait.pl ");
foreach(@nodes)
{   print("$_.dat ");   }
print("\n");

print("rm ");
foreach(@nodes)
{   print("$_.dat ");   }
print("\n");




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
