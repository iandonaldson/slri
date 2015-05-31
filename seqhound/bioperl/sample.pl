#!/usr/bin/perl -w

# This file contains code to show you how to use each of the SeqHound functions.
# Simply search for the function you wish to use and copy the code provided here.
# If you want to run this script, you may have to change the path above, eg to
# /usr/perl or /bin/perl, based on your system.

use strict;
use Bio::SeqHound::SeqHound;
use Bio::SeqIO;

my $seqhound = new Bio::SeqHound::SeqHound("seqhound.blueprint.org",
					   "cgi-bin/seqrem");

my ($test, @reply, @data, $param, $val, $tmp, @param);


### SHoundInit takes 2 parameters
### 1) boolean InitializeNetEntrez
### 2) String applicationName
### initializes SeqHound
$test = $seqhound->SHoundInit("TRUE", "sample.pl");
# get back the reply values
@reply = $test->getValues();
print("VAL: $reply[0]\n");
@data = $test->getKeys();
print("KEY: $data[0]\n"); 
       
       
       
#### SHoundGetFastaList
#### param1: array of gi
#### returns an array of fasta objects that allow access
#### to the sequence and defline string
my @query = (4557225, 555); 
my @fasta = $seqhound->SHoundGetFastaList(@query);
foreach $val (@fasta){
 print "SEQ: ", $val->getSequence(), "\n";
 print "DEF: ", $val->getDefline(), "\n";
 print "\n";
}
print "\n";
       
       
 sub A{      

### SHoundIsNetEntrezOn takes no parameters
### returns boolean success of initialization 
$test = $seqhound->SHoundIsNetEntrezOn();
@reply = $test->getValues();
# get back the reply values
@reply = $test->getValues();
print("VAL: $reply[0]\n");	
	

### SHoundNetEntrezInit takes 1 parameter
### 1) the name of the application
### returns the success of initialization of the NetEntrez
$test = $seqhound->SHoundNetEntrezInit("sample.pl");
@reply = $test->getValues();
print("VAL: $reply[0]\n");
@data = $test->getKeys();
print("KEY: $data[0]\n"); 	


### SHoundFindAcc takes 1 parameter
### 1) the accession you wish to get
### returns an object that holds the GI of the accession
$test = $seqhound->SHoundFindAcc("P00952");
@reply = $test->getValues();
print("VAL: $reply[0]\n");
@data = $test->getKeys();
print("KEY: $data[0]\n");


@param = ("P00952", "P00352");


### SHoundFindAccList takes 1 parameter
### 1) an array of Strings of accession
### returns an object that has the GI of the accession
$test = $seqhound->SHoundFindAccList(@param);
@reply = $test->getValues();
print "VAL: ";
foreach $val (@reply){
   print "$val ";
}
print "\nKEY: ";
@data = $test->getKeys();
foreach $val (@data){
   print "$val ";
}


### SHoundFindName takes 1 parameter
### 1) the name 
### returns an object that has the GI
$test = $seqhound->SHoundFindName("BtMIsATn");
@reply = $test->getValues();
print "VAL: ";
foreach $val (@reply){
   print "$val ";
}
print "\nKEY: ";
@data = $test->getKeys();
foreach $val (@data){
   print "$val ";
}


### SHoundFindNameList takes 1 parameter
### 1) array of String name
### returns an object with the GIs
@param = ("BtMIsATn", "ECJ224437");
$test = $seqhound->SHoundFindNameList(@param);
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
}
print "\nKEY: ";
@data = $test->getKeys();
foreach $val (@data){
   print "$val ";
}


### SHoundAccFromGi takes 1 parameter
### 1) a GI value
### returns an object with the accession string
$test = $seqhound->SHoundAccFromGi(4557225);
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
}
print "\nKEY: ";
@data = $test->getKeys();
foreach $val (@data){
   print "$val ";
}


@param = (4557225, 4557235);


### SHoundAccFromGiList takes 1 parameter
### 1) an array of GI
### returns an object with the accession strings
$test = $seqhound->SHoundAccFromGiList(@param);
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
}
print "\nKEY: ";
@data = $test->getKeys();
foreach $val (@data){
   print "$val ";
}


### SHoundSeqIdFromGi
### 1) a GI value
### returns an object with the SeqId
$test = $seqhound->SHoundSeqIdFromGi(4557225);
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
}
print "\nKEY: ";
@data = $test->getKeys();
foreach $val (@data){
   print "$val ";
}
print "\n";


@param = (4557225, 4557255);


### SHoundSeqIdFromGiList
### 1) a GI array
### returns an object with the SeqId
$test = $seqhound->SHoundSeqIdFromGiList(@param);
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
}
print "\nKEY: ";
@data = $test->getKeys();
foreach $val (@data){
   print "$val ";
}
print "\n";

 
### SHoundIsProtein
### 1) a GI value
### returns an object with the boolean is GI a protein
$test = $seqhound->SHoundIsProtein(4557225);
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
}
print "\nKEY: ";
@data = $test->getKeys();
foreach $val (@data){
   print "$val ";
}
print "\n";


### SHoundIsNRFirst takes 1 parameter
### 1) a GI value
### returns object with boolean is GI NRFirst
$test = $seqhound->SHoundIsNRFirst(4557225);
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
}
print "\nKEY: ";
@data = $test->getKeys();
foreach $val (@data){
   print "$val ";
}
print "\n";


### SHoundExists
### param 1: GI value
### returns object with boolean is GI exist
$test = $seqhound->SHoundExists(4557225);
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
}
print "\nKEY: ";
@data = $test->getKeys();
foreach $val (@data){
   print "$val ";
}
print "\n";


### SHound3DbWhat
### param 1: GI value
### returns object with Gi is 3DbWhat 
$test = $seqhound->SHound3DbWhat(4557225);
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
}
print "\nKEY : ";
@data = $test->getKeys();
foreach $val (@data){
   print "$val ";
}
print "\n";


### SHoundGetLargestMMDBID
### returns the largest MMDB id
$test = $seqhound->SHoundGetLargestMMDBID();
@reply = $test->getValues();
print("VAL: $reply[0]\n");
# can look at the parameters sent to see
@data = $test->getKeys();
print("KEY: $data[0]\n");
print "\n";


### SHound3DExists
### param 1: gi value
### returns object with boolean gi 3D exists?
$test = $seqhound->SHound3DExists(4557225);
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
}
print "\nKEY: ";
@data = $test->getKeys();
foreach $val (@data){
   print "$val ";
}
print "\n";


### SHoundGetBioseq
### parameter 1: gi
### returns object with bioseq value
$test = $seqhound->SHoundGetBioseq(4557225);
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
}
print "\nKEY: ";
@data = $test->getKeys();
foreach $val (@data){
   print "$val ";
}
print "\n";


### SHoundGetBioseqList
### param 1: gi array
### returns object with bioseq values
$test = $seqhound->SHoundGetBioseqList(@param);
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
}
print "\nKEY: ";
@data = $test->getKeys();
foreach $val (@data){
   print "$val ";
}
print "\n";


### SHoundXMLBioseq
### param 1: gi value
### returns object with bioseq with XML
$test = $seqhound->SHoundXMLBioseq(4557225);
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
}
print "\nKEY: ";
@data = $test->getKeys();
foreach $val (@data){
   print "$val ";
}
print "\n";


### SHoundGetSeqEntry
### param1: gi
### returns object with seqentry value
$test = $seqhound->SHoundGetSeqEntry(4557225);
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
}
print "\nKEY: ";
@data = $test->getKeys();
foreach $val (@data){
   print "$val ";
}
print "\n";


### SHoundGetSeqEntryList
### param 1: gi
### returns object with seqentry values
$test = $seqhound->SHoundGetSeqEntryList(@param);
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
}
print "\nKEY: ";
@data = $test->getKeys();
foreach $val (@data){
   print "$val ";
}
print "\n";


### SHoundGetXMLSeqEntry
### param 1: gi
### returns an object with seqentrys in XML
$test = $seqhound->SHoundGetXMLSeqEntry(4557225);
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
}
print "\nKEY: ";
@data = $test->getKeys();
foreach $val (@data){
   print "$val ";
}
print "\n";


### SHoundGetBioseqSet
### param1: gi
### returns an object with bioseqset
$test = $seqhound->SHoundGetBioseqSet(4557225);
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
}
print "\nKEY: ";
@data = $test->getKeys();
foreach $val (@data){
   print "$val ";
}
print "\n";


### SHoundGetBioseqSetList
### param 1: gi array
### returns object with bioseqset values
$test = $seqhound->SHoundGetBioseqSetList(@param);
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
}
print "\nKEY: ";
@data = $test->getKeys();
foreach $val (@data){
   print "$val ";
}
print "\n";


### SHoundGetDefline: 
### param 1: gi value
### returns object with defline value
$test = $seqhound->SHoundGetDefline(4557225);
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
}
print "\nKEY: ";
@data = $test->getKeys();
foreach $val (@data){
   print "$val ";
}
print "\n";


### SHoundGetDeflineList:
### param1: gi array
### returns object with defline values
$test = $seqhound->SHoundGetDeflineList(@param);
@reply = $test->getValues();
print "\nVAL:\n";
foreach $val (@reply){
   print "$val\n";
}
print "\nKEY: ";
@data = $test->getKeys();
foreach $val (@data){
   print "$val ";
}
print "\n";


### SHoundGetFasta
### param1: gi value
### returns a SeqHoundFasta object that allows us to access
### the sequence and defline strings
my $fasta = $seqhound->SHoundGetFasta(4557225);
print "SEQ: ", $fasta->getSequence(), "\n";
print "DEF: ", $fasta->getDefline(), "\n";
print "\n";


#### SHoundGetFastaList
#### param1: array of gi
#### returns an array of fasta objects that allow access
#### to the sequence and defline string
my @param1 = (4557225, 4557245, 4557235, 4557265, 4557295, 4557285, 10); 
my @fasta = $seqhound->SHoundGetFastaList(@param1);
foreach $val (@fasta){
 print "SEQ: ", $val->getSequence(), "\n";
 print "DEF: ", $val->getDefline(), "\n";
 print "\n";
}
print "\n";


##### SHoundGetSeqFromGenBank
##### param1: gi
##### return a Bio::Seq object.
##### To see what a Bio::Seq object is and how it works
##### consult the documentation for the object using the command:
##### perldoc Bio::Seq
my $seq = $seqhound->SHoundGetSeqFromGenBank(4557225);
print "ACC: ", $seq->display_id(), "\n";
print "SEQ: ", $seq->seq(), "\n";
print "TYP: ", $seq->alphabet(), "\n";
print "ACC: ", $seq->accession_number(), "\n";
print "VER: ", $seq->seq_version(), "\n";
print "DES: ", $seq->desc(), "\n";
print "PRI: ", $seq->primary_id(), "\n";
print "\n";


### SHoundGetGenBankff
### param 1: gi value
### This method returns a SeqHoundGB object
### A SeqHoundGB object has an array of Bio::Seq objects, accessing it here
my $seqgb = $seqhound->SHoundGetGenBankff(4557225);
my @seqArr =  $seqgb->getSeqArray();
### accessing each Bio::Seq object in the array
foreach my $seq (@seqArr){
   print $seq->display_id(), "\n";
   print $seq->desc(), "\n";
   print $seq->seq_version(), "\n";
   ### etc ... see Bio::Seq documentation
}
### Can also print the contents of the GenBankff to a file
### print_to_file takes a single parameter, the file name to write to
$seqgb->print_to_file("gb_file.txt");
my @keys = $seqgb->getKeys();
foreach my $key (@keys){
   print "$key ";
}
print "\n";


### SHoundGetGenBankffList
### param1: gi value
### This method returns a SeqHoundGB object
### A SeqHoundGB object has an array of Bio::Seq objects, accessing it here
$seqgb = $seqhound->SHoundGetGenBankffList(@param1);
@seqArr =  $seqgb->getSeqArray();
### accessing each Bio::Seq object in the array
foreach my $seq (@seqArr){
   print $seq->display_id(), "\n";
   print $seq->desc(), "\n";
   print $seq->seq_version(), "\n";
   print "\n";
   ### etc ... see Bio::Seq documentation
}
### Can also print the contents of the GenBankff to a file
### print_to_file takes a single parameter, the file name to write to
$seqgb->print_to_file("gb_file.txt");
@keys = $seqgb->getKeys();
foreach my $key (@keys){
   print "$key ";
}
print "\n";


### SHoundGet3D
### parameter 1: mmdbid
### returns an object with the 3D information in binary
#$test = $seqhound->SHoundGet3D(11);
#@reply = $test->getValues();
#print "\nVAL: ";
#foreach $val (@reply){
#   print "$val ";
#} 
#@data = $test->getKeys();
#print "\nKEY: ";
#foreach $val (@data){
#    print "$val ";
#}
#print "\n";

### SHoundGet3DfromPdbId can be used in two ways:
### 1) if you want the 3D, simply provide a pdbid
###    eg. $seqhound->SHoundGet3DfromPdbId("1GIX");
### 2) if you want the data printed to a file, provide
###    a second parameter, a file name, 
###    eg. $seqhound->SHoundGet3DfromPdbId("1GIX", "3d.txt");
###

### SHoundGet3DfromPdbId(String pdbid, optional String filename);
$test = $seqhound->SHoundGet3DfromPdbId("1GIX", "3d.txt");
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
} 
@data = $test->getKeys();
print "\nKEY: ";
foreach $val (@data){
    print "$val ";
}
print "\n";


### SHoundGetXML3D can be used in two ways:
### 1) if you want the 3D in xml, simply provide a mmdbid
###    eg. $seqhound->SHoundGetXML3D(54);
### 2) if you want the data printed to a file, provide
###    a second parameter, a file name, 
###    eg. $seqhound->SHoundGetXML3D(54, "xml3d.txt");
###


### SHoundGetXML3D(String mmdbid, optional String filename);
$test = $seqhound->SHoundGetXML3D(54, "xml3d.txt");
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
} 
@data = $test->getKeys();
print "\nKEY: ";
foreach $val (@data){
    print "$val ";
}
print "\n";


### SHoundGetPDB3D can be used in two ways:
### 1) if you want the pdb 3D, simply provide a mmdbid
###    eg. $seqhound->SHoundGetPDB3D(11);
### 2) if you want the data printed to a file, provide
###    a second parameter, a file name, 
###    eg. $seqhound->SHoundGetPDB3D(11, "pdb3d.txt");
###


### SHoundGetPDB3D(String pdbid, optional String filename);
$test = $seqhound->SHoundGetPDB3D(11, "pdb3d.txt");
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
} 
@data = $test->getKeys();
print "\nKEY: ";
foreach $val (@data){
    print "$val ";
}
print "\n";


### SHoundGiFrom3D
### parameter 1: "TRUE" "FALSE"
### parameter 2: mmdbid 
### returns object with gi values
$test = $seqhound->SHoundGiFrom3D("TRUE", 11);
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
} 
@data = $test->getKeys();
print "\nKEY: ";
foreach $val (@data){
    print "$val ";
}
print "\n";
my @mmdbid = (11, 12, 13);


### SHoundGiFrom3DList
### Paramter 1: "TRUE" "FALSE"
### parameter 2: mmdbid array
$test = $seqhound->SHoundGiFrom3DList("TRUE", @mmdbid);
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
} 
@data = $test->getKeys();
print "\nKEY: ";
foreach $val (@data){
    print "$val ";
}
print "\n";


### SHoundGiFromPDBchain takes two parameters:
### parameter 1: the pdbcode
### parameter 2: the chain
### eg SHoundGiFromPDBchain("1VII", "NULL");
### eg SHoundGiFromPDBchain("1AY1", "L");
$test = $seqhound->SHoundGiFromPDBchain("1VII", "null");
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
} 
@data = $test->getKeys();
print "\nKEY: ";
foreach $val (@data){
    print "$val ";
}
print "\n";


#### SHound3DFromGi
#### param1: gi value
#### returns object with access to 3d data
$test = $seqhound->SHound3DFromGi(2392601);
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
} 
@data = $test->getKeys();
print "\nKEY: ";
foreach $val (@data){
   print "$val ";
}
print "\n";


my @gi = (2392601, 2392602, 2395421);


#### SHound3DFromGiList
#### param1: gi array
#### returns object with 3d data
$test = $seqhound->SHound3DFromGiList(@gi);
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
} 
@data = $test->getKeys();
print "\nKEY: ";
foreach $val (@data){
   print "$val ";
}
print "\n";


#### SHoundDNAFromProtein
#### parameter 1: protein ID
#### returns object with DNA id data for protein
$test = $seqhound->SHoundDNAFromProtein(94);
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
} 
@data = $test->getKeys();
print "\nKEY: ";
foreach $val (@data){
   print "$val ";
}
print "\n";


### SHoundDNAFromProteinList
### parameter 1: protein array
### return object with dna data
my @pro = (94, 99, 104, 110, 117);
$test = $seqhound->SHoundDNAFromProteinList(@pro);
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
} 
@data = $test->getKeys();
print "\nKEY: ";
foreach $val (@data){
   print "$val ";
}
print "\n";


### SHoundProteinFromDNA
### param1: dna id
### returns object with protein data
$test = $seqhound->SHoundProteinFromDNA(109);
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
} 
@data = $test->getKeys();
print "\nKEY: ";
foreach $val (@data){
   print "$val ";
}
print "\n";
my @dna = (93, 98, 103, 109);


#### SHoundProteinFromDNAList
#### parameter: dna id array
#### returns object with access to protein data
$test = $seqhound->SHoundProteinFromDNAList(@dna);
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
} 
@data = $test->getKeys();
print "\nKEY: ";
foreach $val (@data){
   print "$val ";
}
print "\n";

my @gi2 = (4557225, 45573454, 4557455);

### SHoundMuidFromGi
### parameter: gi
### returns object with access to muid
$test = $seqhound->SHoundMuidFromGi($gi2[0]);
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
} 
@data = $test->getKeys();
print "\nKEY: ";
foreach $val (@data){
   print "$val ";
}
print "\n";


#### SHoundMuidFromGiList
#### param: gi array
#### returns object with access to muid data
$test = $seqhound->SHoundMuidFromGiList(@gi2);
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
} 
@data = $test->getKeys();
print "\nKEY: ";
foreach $val (@data){
   print "$val ";
}
print "\n";
my @dlist = (11, 12, 13);


#### SHoundMuidFrom3D
#### parameter: 3D id
#### returns object with access to muid data
$test = $seqhound->SHoundMuidFrom3D($dlist[0]);
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
} 
@data = $test->getKeys();
print "\nKEY: ";
foreach $val (@data){
   print "$val ";
}
print "\n";


#### SHoundMuidFrom3DList
#### parameter1: 3d array
#### returns object with muid data
$test = $seqhound->SHoundMuidFrom3DList(@dlist);
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
} 
@data = $test->getKeys();
print "\nKEY: ";
foreach $val (@data){
   print "$val ";
}
print "\n";


#### SHoundTaxIDFromGi
#### parameter1: gi array
#### returns object with taxid data
$test = $seqhound->SHoundTaxIDFromGi($gi2[0]);
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
} 
@data = $test->getKeys();
print "\nKEY: ";
foreach $val (@data){
  print "$val ";
}
print "\n";


#### SHoundTaxIDFromGiList
#### parameter: gi array
#### returns object with access to taxid data
$test = $seqhound->SHoundTaxIDFromGiList(@gi2);
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
} 
@data = $test->getKeys();
print "\nKEY: ";
foreach $val (@data){
   print "$val ";
}
print "\n";


### SHoundTaxIDFrom3D
### parameter: 3d id
### returns object with access to taxid data
$test = $seqhound->SHoundTaxIDFrom3D($dlist[0]);
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
} 
@data = $test->getKeys();
print "\nKEY: ";
foreach $val (@data){
   print "$val ";
}
print "\n";


### SHoundTaxIDFrom3DList
### param: 3d array
### returns object with taxid
$test = $seqhound->SHoundTaxIDFrom3DList(@dlist);
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
} 
@data = $test->getKeys();
print "\nKEY: ";
foreach $val (@data){
   print "$val ";
}
print "\n";



### SHoundProteinsFromTaxId
### param: taxid value
### returns object with protein
$test = $seqhound->SHoundProteinsFromTaxID($dlist[0]);
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
} 
@data = $test->getKeys();
print "\nKEY: ";
foreach $val (@data){
   print "$val ";
}
print "\n";


### SHoundProteinsFromTaxIDList
### param: taxid array 
### returns object with protein
$test = $seqhound->SHoundProteinsFromTaxIDList(@dlist);
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
} 
@data = $test->getKeys();
print "\nKEY: ";
foreach $val (@data){
   print "$val ";
}
print "\n";


### SHoundProteinsFromTaxIDIII
### param: taxid  
### returns object with protein
$test = $seqhound->SHoundProteinsFromTaxIDIII($dlist[0]);
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
} 
@data = $test->getKeys();
print "\nKEY: ";
foreach $val (@data){
   print "$val ";
}
print "\n";


### SHoundDNAFromTaxID
### param: taxid 
### returns object with dna 
$test = $seqhound->SHoundDNAFromTaxID($dlist[0]);
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
} 
@data = $test->getKeys();
print "\nKEY: ";
foreach $val (@data){
   print "$val ";
}
print "\n";


### SHoundDNAFromTaxIDList
### param: taxid 
### returns object with dna 
$test = $seqhound->SHoundDNAFromTaxIDList(@dlist);
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
} 
@data = $test->getKeys();
print "\nKEY: ";
foreach $val (@data){
   print "$val ";
}
print "\n";


### SHoundDNAFromTaxIDIII
### param: taxid 
### returns object with dna 
$test = $seqhound->SHoundDNAFromTaxIDIII($dlist[0]);
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
} 
@data = $test->getKeys();
print "\nKEY: ";
foreach $val (@data){
   print "$val ";
}
print "\n";


### SHoundProteinsFromOrganims takes 2 parameters:
### 1) param 1 is a chromflag which can be:
###    "CHROM_PHAGE", "CHROM_NR", "CHROM_ECE", "CHROM_PLMD",
###    "CHROM_CHLO", "CHROM_MITO", "CHROM_CHROM", "CHROM_ALL"
### 2) param 2 is the taxid
$test = $seqhound->SHoundProteinsFromOrganism("CHROM_ALL", 10090);
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
} 
@data = $test->getKeys();
print "\nKEY: ";
foreach $val (@data){
   print "$val ";
}
print "\n";


my @org = (10090, 9606, 4932, 6035);


### SHoundProteinsFromOrganims takes 2 parameters:
### 1) param 1 is a chromflag which can be:
###    "CHROM_PHAGE", "CHROM_NR", "CHROM_ECE", "CHROM_PLMD",
###    "CHROM_CHLO", "CHROM_MITO", "CHROM_CHROM", "CHROM_ALL"
### 2) param 2 is the taxid
$test = $seqhound->SHoundProteinsFromOrganismList("CHROM_ALL", @org);
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
} 
@data = $test->getKeys();
print "\nKEY: ";
foreach $val (@data){
   print "$val ";
}
print "\n";


#### SHoundProteinsFromChromosome
#### param 1: chromid
#### returns object with proteins
$test = $seqhound->SHoundProteinsFromChromosome(143);
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
} 
@data = $test->getKeys();
print "\nKEY: ";
foreach $val (@data){
   print "$val ";
}
print "\n";


my @chrom = (143, 13, 84);


#### SHoundProteinsFromChromosomeList
#### param 1: chromid array
#### returns object with proteins
$test = $seqhound->SHoundProteinsFromChromosomeList(@chrom);
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
} 
@data = $test->getKeys();
print "\nKEY: ";
foreach $val (@data){
   print "$val ";
}
print "\n";


#### SHoundAllGenomes
#### returns object with all genome 
$test = $seqhound->SHoundAllGenomes();
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
} 
@data = $test->getKeys();
print "\nKEY: ";
foreach $val (@data){
   print "$val ";
}
print "\n";


### SHoundProteinsFromOrganims takes 2 parameters:
### 1) param 1 is a chromflag which can be:
###    "CHROM_PHAGE", "CHROM_NR", "CHROM_ECE", "CHROM_PLMD",
###    "CHROM_CHLO", "CHROM_MITO", "CHROM_CHROM", "CHROM_ALL"
### 2) param 2 is the taxid
$test = $seqhound->SHoundChromosomeFromGenome("CHROM_ALL", 10090);
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
} 
@data = $test->getKeys();
print "\nKEY: ";
foreach $val (@data){
   print "$val ";
}
print "\n";


my @gen = (10090, 9606);


### SHoundProteinsFromOrganims takes 2 parameters:
### 1) param 1 is a chromflag which can be:
###    "CHROM_PHAGE", "CHROM_NR", "CHROM_ECE", "CHROM_PLMD",
###    "CHROM_CHLO", "CHROM_MITO", "CHROM_CHROM", "CHROM_ALL"
### 2) param 2 is the taxid
$test = $seqhound->SHoundChromosomeFromGenomeList("CHROM_ALL", @gen);
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
} 
@data = $test->getKeys();
print "\nKEY: ";
foreach $val (@data){
   print "$val ";
}
print "\n";


### SHoundProteinsFromOrganims takes 2 parameters:
### 1) param 1 is a chromflag which can be:
###    "CHROM_PHAGE", "CHROM_NR", "CHROM_ECE", "CHROM_PLMD",
###    "CHROM_CHLO", "CHROM_MITO", "CHROM_CHROM", "CHROM_ALL"
### 2) param 2 is the taxid
$test = $seqhound->SHoundDNAFromOrganism("CHROM_ALL", 10090);
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
} 
@data = $test->getKeys();
print "\nKEY: ";
foreach $val (@data){
   print "$val ";
}
print "\n";


### SHoundProteinsFromOrganims takes 2 parameters:
### 1) param 1 is a chromflag which can be:
###    "CHROM_PHAGE", "CHROM_NR", "CHROM_ECE", "CHROM_PLMD",
###    "CHROM_CHLO", "CHROM_MITO", "CHROM_CHROM", "CHROM_ALL"
### 2) param 2 is the taxid
$test = $seqhound->SHoundDNAFromOrganismList("CHROM_ALL", @gen);
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
} 
@data = $test->getKeys();
print "\nKEY: ";
foreach $val (@data){
   print "$val ";
}
print "\n";


### SHoundDNAFromChromosome
### param 1: chromosome
### returns object with dna
$test = $seqhound->SHoundDNAFromChromosome(213);
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
} 
@data = $test->getKeys();
print "\nKEY: ";
foreach $val (@data){
   print "$val ";
}
print "\n";

my @chrom1 = (213, 215, 143);


### SHoundDNAFromChromosomeList
### param 1: chromosome
### returns object with dna
$test = $seqhound->SHoundDNAFromChromosomeList(@chrom1);
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
} 
@data = $test->getKeys();
print "\nKEY: ";
foreach $val (@data){
   print "$val ";
}
print "\n";


### SHoundRedundantGroup
### parameter 1: gi value
### returns object with redundant information
$test = $seqhound->SHoundRedundantGroup(4557225);
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
} 
@data = $test->getKeys();
print "\nKEY: ";
foreach $val (@data){
   print "$val ";
}
print "\n";


### SHoundRedundantGroupList
### parameter 1: gi values
### returns object with redundant information
$test = $seqhound->SHoundRedundantGroupList(@gi2);
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
} 
@data = $test->getKeys();
print "\nKEY: ";
foreach $val (@data){
   print "$val ";
}
print "\n";


### SHoundRedundantGroupIDFromGI
### parameter 1: gi values
### returns object with redundant information
$test = $seqhound->SHoundRedundantGroupIDFromGI(4557225);
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
} 
@data = $test->getKeys();
print "\nKEY: ";
foreach $val (@data){
   print "$val ";
}
print "\n";


### SHoundRedundantGroupIDFromGIList
### parameter 1: gi values
### returns object with redundant information
$test = $seqhound->SHoundRedundantGroupIDFromGIList(@gi2);
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
} 
@data = $test->getKeys();
print "\nKEY: ";
foreach $val (@data){
   print "$val ";
}
print "\n";


### SHoundRedundantGroupFromID
### parameter 1: group id value
### returns object with redundant group 
$test = $seqhound->SHoundRedundantGroupFromID(666683);
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
} 
@data = $test->getKeys();
print "\nKEY: ";
foreach $val (@data){
   print "$val ";
}
print "\n";


my @group = (666683, 666615);


### SHoundFirstOfRedundantGroupFromID
### parameter 1: group id value
### returns object with first of redundant group 
$test = $seqhound->SHoundFirstOfRedundantGroupFromID(666683);
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
} 
@data = $test->getKeys();
print "\nKEY: ";
foreach $val (@data){
   print "$val ";
}
print "\n";


### SHoundFirstOfRedundantGroupFromIDList
### parameter 1: group id value
### returns object with first of redundant group 
$test = $seqhound->SHoundFirstOfRedundantGroupFromIDList(@group);
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
} 
@data = $test->getKeys();
print "\nKEY: ";
foreach $val (@data){
   print "$val ";
}
print "\n";


### SHoundGetTaxChildNodes
### parameter 1: taxid value
### returns object with tax children 
$test = $seqhound->SHoundGetTaxChildNodes(10090);
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
} 
@data = $test->getKeys();
print "\nKEY: ";
foreach $val (@data){
   print "$val ";
}
print "\n";


### SHoundGetTaxChildNodesList
### parameter 1: taxid value
### returns object with tax children 
$test = $seqhound->SHoundGetTaxChildNodesList(@gen);
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
} 
@data = $test->getKeys();
print "\nKEY: ";
foreach $val (@data){
   print "$val ";
}
print "\n";


### SHoundGetAllTaxProgeny
### parameter 1: taxid value
### returns object with tax children 
$test = $seqhound->SHoundGetAllTaxProgeny($gen[0]);
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
} 
@data = $test->getKeys();
print "\nKEY: ";
foreach $val (@data){
   print "$val ";
}
print "\n";


### SHoundGetTaxParent
### parameter 1: taxid value
### returns object with tax parent 
$test = $seqhound->SHoundGetTaxParent($gen[0]);
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
} 
@data = $test->getKeys();
print "\nKEY: ";
foreach $val (@data){
   print "$val ";
}
print "\n";


### SHoundGetAllTaxAncestors
### parameter 1: taxid value
### returns object with ancestor 
$test = $seqhound->SHoundGetAllTaxAncestors($gen[0]);
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
} 
@data = $test->getKeys();
print "\nKEY: ";
foreach $val (@data){
   print "$val ";
}
print "\n";


### SHoundIsTaxMerged
### parameter 1: taxid value
### returns object with boolean 
$test = $seqhound->SHoundIsTaxMerged($gen[0]);
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
} 
@data = $test->getKeys();
print "\nKEY: ";
foreach $val (@data){
   print "$val ";
}
print "\n";


### SHoundIsTaxDeleted
### parameter 1: taxid value
### returns object with boolean 
$test = $seqhound->SHoundIsTaxDeleted($gen[0]);
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
} 
@data = $test->getKeys();
print "\nKEY: ";
foreach $val (@data){
   print "$val ";
}
print "\n";


### SHoundGetTaxNameFromTaxIDByClass
### parameter 1: taxid value
### parameter 2: "SLRI_taxon_name_name_class_scientific_name"
###              "SLRI_taxon_name_name_class_none"
###              "SLRI_taxon_name_name_class_acronym"
###              "SLRI_taxon_name_name_class_anamorph"
###              "SLRI_taxon_name_name_class_blast_name"
###              "SLRI_taxon_name_name_class_common_name"
###              "SLRI_taxon_name_name_class_equivalent_name"
###              "SLRI_taxon_name_name_class_include"
###              "SLRI_taxon_name_name_class_in_part"
###              "SLRI_taxon_name_name_class_misnomer"
###              "SLRI_taxon_name_name_class_misspelling"
###              "SLRI_taxon_name_name_class_preferred_acronym"
###              "SLRI_taxon_name_name_class_preferred_common_name"
###              "SLRI_taxon_name_name_class_synonym"
###              "SLRI_taxon_name_name_class_teleomorph"
###              "SLRI_taxon_name_name_class_other"
### returns object with tax name
$test = $seqhound->SHoundGetTaxNameFromTaxIDByClass($gen[0], "SLRI_taxon_name_name_class_scientific_name");
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
} 
@data = $test->getKeys();
print "\nKEY: ";
foreach $val (@data){
   print "$val ";
}
print "\n";


### SHoundGetTaxNameFromTaxID
### parameter 1: taxid
### returns object with tax name
$test = $seqhound->SHoundGetTaxNameFromTaxID($gen[0]);
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
} 
@data = $test->getKeys();
print "\nKEY: ";
foreach $val (@data){
   print "$val ";
}
print "\n";


### SHoundGetTaxNameFromTaxIDByClass
### parameter 1: taxid value
### parameter 2: "SHoundGetTaxLineage_FULL"
###              "SHoundGetTaxLineage_GENBANK"
###              "SHoundGetTaxLineage_TAXDBSHORT"
### returns object with tax lineage 
$test = $seqhound->SHoundGetTaxLineageFromTaxID($gen[0], "SHoundGetTaxLineage_FULL");
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
} 
@data = $test->getKeys();
print "\nKEY: ";
foreach $val (@data){
   print "$val ";
}
print "\n";


### SHoundGiFromGOID
### paramter: goid
### return object with gi data
$test = $seqhound->SHoundGiFromGOID(8532);
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
} 
@data = $test->getKeys();
print "\nKEY: ";
foreach $val (@data){
   print "$val ";
}
print "\n";


my @goid = (8532, 8332, 8645, 8212);


### SHoundGiFromGOIDList
### paramter: goid array
### return object with gi data
$test = $seqhound->SHoundGiFromGOIDList(@goid);
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
} 
@data = $test->getKeys();
print "\nKEY: ";
foreach $val (@data){
   print "$val ";
}
print "\n";


### SHoundGOIDFromGi
### paramter: gi 
### return object with goid data
$test = $seqhound->SHoundGOIDFromGi(4279);
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
} 
@data = $test->getKeys();
print "\nKEY: ";
foreach $val (@data){
   print "$val ";
}
print "\n";


### SHoundGOIDFromRedundantGi
### paramter: gi 
### return object with goid data
$test = $seqhound->SHoundGOIDFromRedundantGi($gi2[0]);
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
} 
@data = $test->getKeys();
print "\nKEY: ";
foreach $val (@data){
   print "$val ";
}
print "\n";


### SHoundGiFromCDDID
### paramter: cddid 
### return object with gi 
$test = $seqhound->SHoundGiFromCDDID("pfam00202");
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
} 
@data = $test->getKeys();
print "\nKEY: ";
foreach $val (@data){
   print "$val ";
}
print "\n";


### SHoundGiFromOMIM
### paramter: omimid 
### return object with gi 
$test = $seqhound->SHoundGiFromOMIM(103950);
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
} 
@data = $test->getKeys();
print "\nKEY: ";
foreach $val (@data){
   print "$val ";
}
print "\n";


my @omim = (103950);


### SHoundGiFromOMIM
### paramter: omimid array 
### return object with gi 
$test = $seqhound->SHoundGiFromOMIMList(@omim);
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
} 
@data = $test->getKeys();
print "\nKEY: ";
foreach $val (@data){
   print "$val ";
}
print "\n";


### SHoundOMIMFromGi
### paramter: gi 
### return object with omimid 
$test = $seqhound->SHoundOMIMFromGi($gi2[0]);
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
} 
@data = $test->getKeys();
print "\nKEY: ";
foreach $val (@data){
   print "$val ";
}
print "\n";


### SHoundGiFromCDDIDList
### paramter: cddid array
### return object with gi 
my @cddid = ("pfam00202", "pfam00432");
$test = $seqhound->SHoundGiFromCDDIDList(@cddid);
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
} 
@data = $test->getKeys();
print "\nKEY: ";
foreach $val (@data){
   print "$val ";
}
print "\n";


### SHoundCDDIDFromGi
### paramter: gi 
### return object with cddid 
$test = $seqhound->SHoundCDDIDFromGi($gi2[0]);
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
} 
@data = $test->getKeys();
print "\nKEY: ";
foreach $val (@data){
   print "$val ";
}
print "\n";


### SHoundCDDIDFromGiList
### paramter: gi 
### return object with cddid 
$test = $seqhound->SHoundCDDIDFromGiList(@gi2);
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
} 
@data = $test->getKeys();
print "\nKEY: ";
foreach $val (@data){
   print "$val ";
}
print "\n";


### SHoundCDDScoreFromGi
### paramter: gi 
### return object with cddscore 
$test = $seqhound->SHoundCDDScoreFromGi(4557225, "pfam00202");
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
} 
@data = $test->getKeys();
print "\nKEY: ";
foreach $val (@data){
   print "$val ";
}
print "\n";


### SHoundGiFromLLID
### paramter: llid 
### return object with gi 
$test = $seqhound->SHoundGiFromLLID(1234);
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
} 
@data = $test->getKeys();
print "\nKEY: ";
foreach $val (@data){
   print "$val ";
}
print "\n";


my @llid = (1234, 1245, 3421);


### SHoundGiFromLLIDList
### paramter: llid 
### return object with gi 
$test = $seqhound->SHoundGiFromLLIDList(@llid);
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
} 
@data = $test->getKeys();
print "\nKEY: ";
foreach $val (@data){
   print "$val ";
}
print "\n";


### SHoundLLIDFromGi
### paramter: gi 
### return object with llid 
$test = $seqhound->SHoundLLIDFromGi($gi2[0]);
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
} 
@data = $test->getKeys();
print "\nKEY: ";
foreach $val (@data){
   print "$val ";
}
print "\n";


### SHoundLLIDFromGiList
### paramter: gi 
### return object with llid 
$test = $seqhound->SHoundLLIDFromGiList(@gi2);
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
} 
@data = $test->getKeys();
print "\nKEY: ";
foreach $val (@data){
   print "$val ";
}
print "\n";


### SHoundLocusFromGi
### paramter: gi 
### return object with locus 
$test = $seqhound->SHoundLocusFromGi($gi2[0]);
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
} 
@data = $test->getKeys();
print "\nKEY: ";
foreach $val (@data){
   print "$val ";
}
print "\n";


### SHoundGODBGetRecordByReference
### paramter: reference string 
### return object with go 
$test = $seqhound->SHoundGODBGetRecordByReference("EC");
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
} 
@data = $test->getKeys();
print "\nKEY: ";
foreach $val (@data){
   print "$val ";
}
print "\n";


### SHoundGODBGetChildrenOf
### paramter: goid 
### return object with go 
$test = $seqhound->SHoundGODBGetChildrenOf(6754);
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
} 
@data = $test->getKeys();
print "\nKEY: ";
foreach $val (@data){
   print "$val ";
}
print "\n";


### SHoundGODBGetParentOf
### paramter: goid 
### return object with go 
$test = $seqhound->SHoundGODBGetParentOf(6754);
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
} 
@data = $test->getKeys();
print "\nKEY: ";
foreach $val (@data){
   print "$val ";
}
print "\n";


### SHoundGODBGetAllAncestors
### paramter: goid 
### return object with go 
$test = $seqhound->SHoundGODBGetAllAncestors(6754);
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
} 
@data = $test->getKeys();
print "\nKEY: ";
foreach $val (@data){
   print "$val ";
}
print "\n";


### SHoundGODBGetNameByID
### paramter: goid 
### return object with go 
$test = $seqhound->SHoundGODBGetNameByID(6754);
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
} 
@data = $test->getKeys();
print "\nKEY: ";
foreach $val (@data){
   print "$val ";
}
print "\n";


### SHoundGODBGetClassification
### paramter: goid 
### return object with go 
$test = $seqhound->SHoundGODBGetClassification(6754);
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
} 
@data = $test->getKeys();
print "\nKEY: ";
foreach $val (@data){
   print "$val ";
}
print "\n";


### SHoundNeighboursFromGi
### paramter: gi 
### parameter 2: evalue
### parameter 3: blimit boolean string
### return object with neighbours 
$test = $seqhound->SHoundNeighboursFromGi(2392601, 1.00, "TRUE");
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
} 
@data = $test->getKeys();
print "\nKEY: ";
foreach $val (@data){
   print "$val ";
}
print "\n";


### SHoundNeighboursFromGi
### parameter 1: evalue
### parameter 2: gi 
### return object with neighbours 
$test = $seqhound->SHoundNeighboursFromGiList(1.00, @gi2);
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
} 
@data = $test->getKeys();
print "\nKEY: ";
foreach $val (@data){
   print "$val ";
}
print "\n";


### SHoundNeighboursFromGi
### parameter 1: evalue
### parameter 2: gi 
### return object with neighbours 
$test = $seqhound->SHoundNeighboursFromTaxID(1, 56636);
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
} 
@data = $test->getKeys();
print "\nKEY: ";
foreach $val (@data){
   print "$val ";
}
print "\n";


### SHound3DNeighboursFromGi
### parameter 1: evalue
### parameter 2: gi 
### return object with neighbours 
$test = $seqhound->SHound3DNeighboursFromGi(1, $gi2[0]);
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
} 
@data = $test->getKeys();
print "\nKEY: ";
foreach $val (@data){
   print "$val ";
}
print "\n";


### SHound3DNeighboursFromGi
### parameter 1: evalue
### parameter 2: gi 
### return object with neighbours 
$test = $seqhound->SHound3DNeighboursFromGiList(1, @gi2);
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
} 
@data = $test->getKeys();
print "\nKEY: ";
foreach $val (@data){
   print "$val ";
}
print "\n";


### SHound3DNeighboursFromGi
### parameter 1: evalue
### parameter 2: taxid 
### return object with neighbours 
$test = $seqhound->SHound3DNeighboursFromTaxID(1, 56636);
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
} 
@data = $test->getKeys();
print "\nKEY: ";
foreach $val (@data){
   print "$val ";
}
print "\n";


### SHound3DNeighboursFromGi
### parameter 1: evalue
### parameter 2: taxid 
### return object with neighbours 
$test = $seqhound->SHound3DListFromTaxID(196414);
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
} 
@data = $test->getKeys();
print "\nKEY: ";
foreach $val (@data){
   print "$val ";
}
print "\n";


### SHoundGODBGetHierarchyLevel
### parameter 1: taxid 
### return object with neighbours 
$test = $seqhound->SHoundGODBGetHierarchyLevel(6414);
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
} 
@data = $test->getKeys();
print "\nKEY: ";
foreach $val (@data){
   print "$val ";
}
print "\n";


### SHoundGetGisByNumberOfDomains
### parameter 1: number of domains 
### return object gi 
$test = $seqhound->SHoundGetGisByNumberOfDomains(4);
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
} 
@data = $test->getKeys();
print "\nKEY: ";
foreach $val (@data){
   print "$val ";
}
print "\n";


### SHoundGetDomainIdFromLabel
### parameter 1: domain label 
### return object domain id
$test = $seqhound->SHoundGetDomainIdFromLabel("SH3");
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
} 
@data = $test->getKeys();
print "\nKEY: ";
foreach $val (@data){
   print "$val ";
}
print "\n";


### SHoundGetGisByDomainId
### parameter 1: domain id 
### return object with gi 
$test = $seqhound->SHoundGetGisByDomainId("smart00038");
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
} 
@data = $test->getKeys();
print "\nKEY: ";
foreach $val (@data){
   print "$val ";
}
print "\n";


### SHoundGetDomainLabelFromDoaminId
### parameter 1: domain id 
### return object with domain Label 
$test = $seqhound->SHoundGetDomainLabelFromDomainId("smart00038");
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
} 
@data = $test->getKeys();
print "\nKEY: ";
foreach $val (@data){
   print "$val ";
}
print "\n";


### SHoundGetDomainLabelFromDoaminId
### parameter 1: domain id 
### parameter 2: evalue 
### return object with gi 
$test = $seqhound->SHoundGetGisByDomainIdAndEvalue("smart00038", 1);
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
} 
@data = $test->getKeys();
print "\nKEY: ";
foreach $val (@data){
   print "$val ";
}
print "\n";


### SHoundGetDomainsFromGi
### parameter 1: gi 
### return object with domain 
$test = $seqhound->SHoundGetDomainsFromGi(4557225);
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
} 
@data = $test->getKeys();
print "\nKEY: ";
foreach $val (@data){
   print "$val ";
}
print "\n";


### SHoundGetDomainsFromGiWithEvalue
### parameter 1: gi 
### parameter 1: evalue 
### return object with domain 
$test = $seqhound->SHoundGetDomainsFromGiWithEvalue(4557225, 1);
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
} 
@data = $test->getKeys();
print "\nKEY: ";
foreach $val (@data){
   print "$val ";
}
print "\n";


### SHoundMoleculeType
### parameter 1: gi 
### return object with molecule type (dna, rna, protein or 0)
$test = $seqhound->SHoundMoleculeType(-1);
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
} 
@data = $test->getKeys();
print "\nKEY: ";
foreach $val (@data){
   print "$val ";
}
print "\n";


### SHoundGOPMIDFromGiAndGOID
### parameter 1: gi 
### parameter 2: goid
### return object with go pmid
$test = $seqhound->SHoundGOPMIDFromGiAndGOID(21071030, 5554);
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
} 
@data = $test->getKeys();
print "\nKEY: ";
foreach $val (@data){
   print "$val ";
}
print "\n";


### SHoundGOECodeFromGiAndGOID
### parameter 1: gi 
### parameter 2: goid
### return object with go evidence code
$test = $seqhound->SHoundGOECodeFromGiAndGOID(21071030, 5554);
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
} 
@data = $test->getKeys();
print "\nKEY: ";
foreach $val (@data){
   print "$val ";
}
print "\n";


### SHoundGetReferenceIDFromGi
### parameter 1: gi 
### parameter 2: referencetype
### return object with publication id, is PubMed id, reftype='1', if Medline id , reftype='2'
$test = $seqhound->SHoundGetReferenceIDFromGi(4557225, 2);
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
} 
@data = $test->getKeys();
print "\nKEY: ";
foreach $val (@data){
   print "$val ";
}
print "\n";


### SHoundSequenceLength
### parameter 1: gi 
### return object with sequence length
$test = $seqhound->SHoundSequenceLength(4557225);
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
} 
@data = $test->getKeys();
print "\nKEY: ";
foreach $val (@data){
   print "$val ";
}
print "\n";


### SHoundGiFromReferenceID
### parameter 1: reference id
### parameter 2: reference type
### return object with gi, if reference id contains PubMed id, reftype='1', if Medline id , reftype='2'
$test = $seqhound->SHoundGiFromReferenceID(85190481,2);
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
} 
@data = $test->getKeys();
print "\nKEY: ";
foreach $val (@data){
   print "$val ";
}
print "\n";


### SHoundGiFromGOIDAndECode
### parameter 1: goid
### parameter 2: go evidence code
### return object with gi
$test = $seqhound->SHoundGiFromGOIDAndECode(4,undef);
if ($test != 0){
   @reply = $test->getValues();
   print "\nVAL: ";
   foreach $val (@reply){
     print "$val ";
   } 
   @data = $test->getKeys();
   print "\nKEY: ";
   foreach $val (@data){
      print "$val ";
   }
}
else {
   print "no return value\n";
}
print "\n";


### SHoundGetAccFromRetiredGi
### parameter 1: gi
### return object with accession
$test = $seqhound->SHoundGetAccFromRetiredGi(2466452);
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
} 
@data = $test->getKeys();
print "\nKEY: ";
foreach $val (@data){
   print "$val ";
}
print "\n";


$param = "21071030";
my $reftype = 2;
### SHoundGiFromReferenceList
### parameter 1: reflist
### parameter 2: reftype
### return object with gi, if reflist contains PubMed id, set reftype='1', if reflist contains Medline id, set reftype='2'
$test = $seqhound->SHoundGiFromReferenceList($param, $reftype);
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
} 
@data = $test->getKeys();
print "\nKEY: ";
foreach $val (@data){
   print "$val ";
}
print "\n";


$param = "6322016,4557225,21071030";
### SHoundGetReferenceIDFromGiList
### parameter 1: gilist
### parameter 2: reftype
### return object with referenceid, if referenceid is PubMed id, set reftype='1', if referenceid is Medline id, set reftype='2'
$test = $seqhound->SHoundGetReferenceIDFromGiList($param, $reftype);
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
} 
@data = $test->getKeys();
print "\nKEY: ";
foreach $val (@data){
   print "$val ";
}
print "\n";


$param = "5554,4";
my $ecode = undef;
### SHoundGiFromGOIDListAndECode
### parameter 1: goid list
### parameter 2: ecode
### return object with gi
$test = $seqhound->SHoundGiFromGOIDListAndECode($param, $ecode);
if ($test != 0)
{
   @reply = $test->getValues();
   print "\nVAL: ";
   foreach $val (@reply){
      print "$val ";
   } 
   @data = $test->getKeys();
   print "\nKEY: ";
   foreach $val (@data){
      print "$val ";
   }
}
print "\n";

### SHoundXMLBioseq
### param 1: gi value
### returns object with bioseq with XML
$test = $seqhound->SHoundGetXMLBioseq(555);
@reply = $test->getValues();
print "\nVAL: ";
foreach $val (@reply){
   print "$val ";
}
print "\nKEY: ";
@data = $test->getKeys();
foreach $val (@data){
   print "$val ";
}
print "\n";

}

   
   
   
   
   
    
