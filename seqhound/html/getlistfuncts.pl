#!/usr/bin/perl

open (FUNCTIONS, ">listoffuncts_raw.txt") || die "Cannot create file. \n";
open (LIST, "seqhoundapi.c") || die "Cannot open file. \n";

while (<LIST>) {
     $line = $_;
     if(/LIBCALL/)
       {
         chop($line);
         #print "$line\n";
         @fields = split(/\s+/,$line);
         $name = $fields[2];
         print "$name\n";
         @fields2 = split(/\(/,$name);
         $fin = $fields2[0];
         print FUNCTIONS "$fin\n";
       }
}


close (LIST);
close (FUNCTIONS);






