#!/usr/bin/perl

# This program will hold execution until 'n' all process in the cluster finish.
# will program will wait for the ocurrence of dummy files XREF_
use strict;

print("ARGV: @ARGV \n");


my $i =0;
my $j =0;
while()
{
#  print("scalar: " . scalar(@ARGV) ." \n");
   for($i=0; $i<scalar(@ARGV); $i++)
   {
      if (open(FP, $ARGV[$i]))
      {
         print($ARGV[$i]." file exist.\n");
         $j++;
	 $ARGV[$i] = ();   # Try remove the element from the array.
      }
      else
      {
#         print("file doesn't exist.\n");
      }
   }
   if($j == scalar(@ARGV))
   {
     exit;
   }
   close(FP);
   sleep(10);
}

print("\n\n");
