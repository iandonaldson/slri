#!/usr/bin/perl
# Gets http call string from seqhoundapi.c
# Looks for lines such as
# sprintf(fpath,"%s?fnct=SeqHoundTitleFromGi&gi=%ld", IsNullStr(slri_cgipath), (long) gi);
# Writes to a file the SeqHoundFunctionName SHoundFunctionName and 
# seqrem?fnct=SeqHoundFunction&arg=value

# If the function does not contain this path then no http call generated
# Will need to revisit for local java api



my $separator = ";"; # delimiter for records in file generated


open (FUNCTIONS, ">httpcalls.txt") || die "Cannot create file. \n";

use Cwd;
my $dir = cwd();           # get current working directory safest way
print $dir;

# assume source code file in a sub directory called sourcefiles
my $filename =$dir."/sourcefiles/seqhoundapi.c";



open (LIST, $filename ) || die "Cannot open file. \n";

while (<LIST>) {
     $line = $_;
     if(/sprintf\(fpath/)
       {
         @fields = split(/,/, $line);
         $funct = $fields[1];
         $funct=~s/\%s/seqrem/;
         $funct=~s/\%s/value/g;
         $funct=~s/\%ld/value/g;
         $funct=~s/\%u/value/g;
         $funct=~s/\%d/value/g;
         $funct=~s/\%g/value/g;
         $fields[1] =~ m/(SeqHound\w+)/;
	   $fSeqHound = $1;
         $fSHound = $fSeqHound;
         $fSHound=~ s/SeqHound/SHound/; # Get the SHound version of the function name
 
         print FUNCTIONS "$fSeqHound $separator $fSHound $separator $funct\n";
       }
}


close (LIST);
close (FUNCTIONS);






