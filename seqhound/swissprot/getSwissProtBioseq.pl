#!/usr/bin/perl -w

###############################################
#
#  This takes as input the a newline delimited list of swissprot accessions, and outputs an asn.1 text file which contains the NCBI conversions of 
#  swissprot records which those accessions reference.  It relies upon the ID1_fetch program to access the asn.1 records from NCBI.  
#  The output is an asn.1 text file containing a bioseq-set which contains all the swissprot records that id1_fetch was able to download.  However, 
#  seqsets are not properly handled, so the script dealwithseqsets.pl must be used to postprocess the records before they can be parsed by seqhound's 
#  mother parser. 
#
#  Author:  Michael Matan, mmatan@blueprint.org
#
#  Useage:  ./getSwissProtBioseq.pl accessionListFileName asn.1OutputFileName
#  
###############################################

$infile=$ARGV[0];
$bioseqoutfile=$ARGV[1];

open (FH_MISSED_SWAL, ">>missed_swal.dat");

open (FH_SWAL, "$infile");  #open the swiss prot accession list file

open (FH_BIOSEQ, ">>${bioseqoutfile}");
print FH_BIOSEQ "Bioseq-set ::= { seq-set { \n";
#print FH_BIOSEQ "Seq-entry ::= set {\n seq-set {\n";

$isfirstentry=1;

while ($current_swa = <FH_SWAL> ) {

  print "$current_swa";
  system("id1_fetch -fmt asn -flat \'8:${current_swa}\' > ${bioseqoutfile}entry");

#  open (FH_BIOSEQ, ">>${bioseqoutfile}");

  open (FH_ENTRY, "${bioseqoutfile}entry");
  if (-s FH_ENTRY == 0) {  #if the entry was not populated, skip it
    close(FH_ENTRY);
    print FH_MISSED_SWAL "$current_swa\n";
    next;
  }

  if ($isfirstentry != 1) {
    print FH_BIOSEQ ", ";
  }
  else {
    $isfirstentry=0;
  }

  $entry_line = <FH_ENTRY>;  #get rid of the first line
  print FH_BIOSEQ "seq { \n";

  while ( $entry_line = <FH_ENTRY> ) {
    print FH_BIOSEQ "$entry_line";
  }

#  print FH_BIOSEQ ", \n";

  close(FH_ENTRY);

  sleep 3;
}

print FH_BIOSEQ "} }\n";

close(FH_BIOSEQ);

