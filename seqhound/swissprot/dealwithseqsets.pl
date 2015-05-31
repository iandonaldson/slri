#!/usr/bin/perl -w

###############################################
#
#  This takes as input the asn.1 output of the getSwissProtBioseq.pl script.  It goes through the asn.1 text bioseq file and 
#  properly formats the seqsets so that a valid Bioseq is produced, which can then be parsed by the mother parser.
#
#  Author:  Michael Matan, mmatan@blueprint.org
#
#  Useage:  ./dealwithseqsets inputfilename outputfilename
#  
###############################################


$inputfilename=$ARGV[0];
$outputfilename=$ARGV[1];

open(FH_inputfile, "<$inputfilename");
open(FH_outputfile, ">$outputfilename");

while ($firstline=<FH_inputfile>) {
  if ($firstline =~ /, seq \{/) {
    $secondline = <FH_inputfile>;
    if ($secondline =~ /class segset/) {
      print FH_outputfile ", set {\n";
      print FH_outputfile "$secondline";
    }
    else {
      print FH_outputfile "$firstline";
      print FH_outputfile "$secondline";
    }
  }
  else {
    print FH_outputfile "$firstline";
  }
}

close(FH_inputfile);
close(FH_outputfile);

