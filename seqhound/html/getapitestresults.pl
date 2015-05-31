#!/usr/bin/perl

# Purpose: Retrieve SHoundFunctionNames from various
#          sourcecode files copied manually to directory 
#          sourcefiles and write to apilisting.txt. 
#          Includes programming language based on file
#          extension and  filename
#          Useful as a crosscheck for major releases to see
#          if functions really exist in source code and to
#          check for typos.
# Note:    Java only gets public functions
#          If a function is commented out it will still be extracted.
#          The bioperl SeqHound.pm needs to be renamed to bioSeqHound.pm
#          otherwise there is no way to differentiate between perl 
#          and bioperl
# Todo:    may need to map to local and remote
#          list of remote files all the rest local?

my $separator = ";";  # delimiter for text file

%languagehash = (
    pm   => perl,
    java => java,
    c    => c,
    cpp  => cpp
    
);



open (FUNCTIONS, ">apilisting.txt") || die "Cannot create file. \n";

use Cwd;
my $dir = cwd();           # get current working directory safest way
print $dir;

# assume source code files are in a directory called sourcefiles
# the bioperl file needs to be renamed to bioSeqhound.pm

my $dirname =$dir."/sourcefiles";

opendir(DIR, $dirname) or die "can't opendir $dirname: $!";
while (defined(my $file = readdir(DIR))) {
 

#compare the end of the file name to the language hash

while (($key,$value) = each(%languagehash)) {
   if ($file =~ /$key$/) {
    $lang = $value;
    print "lang $lang\n";
   }
}

open (LIST, "$dirname/$file") || die "Cannot open $file. \n";
print "filename: $file\n";
if ($lang eq perl) {  # pm files
    while (<LIST>) {
        $line = $_;
        if(/^sub/)
        {
            if(/SHound*/)
	      {
                chop($line);
                #print "$line\n";
                @fields = split(/\s+/,$line);
                $name = $fields[1];
                #print "$name\n";
                @fields2 = split(/\(/,$name);
                $fin = $fields2[0];
               print FUNCTIONS "$lang $separator $file $separator $fin\n";
	      }  
       }
   }
}
elsif ($lang eq java) { # java files
    while (<LIST>) {
    $line = $_;
        if(/public/)  # only get public functions
        {
            if(/SHound*/)
	      {
                chop($line);
                #print "$line\n";
                @fields = split(/\s+/,$line);
                $name = $fields[3];
                # print "$name\n";
                @fields2 = split(/\(/,$name);
                $fin = $fields2[0];
                print FUNCTIONS "$lang $separator $file $separator $fin\n";

	      } 
        }
    }
}
elsif ($lang eq cpp) { # c++ files

    while (<LIST>) {
    $line = $_;
    
        if(/::SHound*/)
	    {
               chop($line);
               #print "$line\n";
               @fields = split(/\s+/,$line);
               $name = $fields[1];
              #  print "$name\n";
               @fields2 = split(/\(/,$name);
              $fields2[0] =~ m/(SHound\w+)/;
	         $fin = $1;
            
               
               print FUNCTIONS "$lang $separator $file $separator $fin\n";

	    } 
     
    }

}

elsif ($lang eq c) { # c files


while (<LIST>) {
     $line = $_;
     if(/LIBCALL/)
       {
        if(/SHound*/){

         chop($line);
         #print "$line\n";
         @fields = split(/\s+/,$line);
         $name = $fields[2];
         print "$name\n";
         @fields2 = split(/\(/,$name);
         $fin = $fields2[0];
         print FUNCTIONS "$lang $separator $file $separator $fin\n";
         }
       }
    }


}

else {
    print "I don't know how to parse $file\n";
}
close (LIST);

}
closedir(DIR);