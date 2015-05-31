#!usr/bin/perl -w

#YNG.pl Yeast Name Generator
#Ian Donaldson, Mar. 2000
#
# ***************************************************************************
#
#  Pre Biomolecular Interaction Network Database (PreBIND)
#
#  File: prebind.c
#  Description: PreBIND cgi interface to PreBIND databases
#				 (see also pb_parser.c and pb_searcher.c)  
#				 
#
#  Authors: Ian Donaldson (ian.donaldson@utoronto.ca)
#			 and Christopher W.V. Hogue (hogue@mshri.on.ca)
#
#  Date: June 2002
#
#  Hogue Lab - University of Toronto Biochemistry Department
#  Samuel Lunenfeld Research Institute, Mount Sinai Hospital
#
#  REVISION/CONTRIBUTION NOTES:
#
#  June 1, 2000  Original MSH Revision.
#              Hogue Lab - University of Toronto Biochemistry Department 
#              and the Samuel Lunenfeld Research Institute, Mount Sinai Hospital
#              http://bioinfo.mshri.on.ca  hogue@mshri.on.ca
#              ftp://ftp.mshri.on.ca/
#
#
#  Copyright Notice:
#
#  Copyright ©2000 Mount Sinai Hospital (MSH)
#
#  This program is free software; you can redistribute it and/or
#  modify it under the terms of the GNU General Public License as
#  published by the Free Software Foundation; either version 2 of
#  the License, or any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
#
#  See the GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the
#         Free Software Foundation, Inc.,
#         59 Temple Place, Suite 330, Boston, MA
#         02111-1307  USA
#  or visit http://www.gnu.org/copyleft/gpl.html
#
#  SPECIAL EXCEPTIONS
#
#  As a special exception, Mount Sinai Hospital gives permission to
#  link this program with the following non-GPL programs or libraries,
#  and distribute the resulting executable, without including the source
#  code for these in the source distribution:
#
#  a) CodeBase 6.5 or greater from Sequiter Software Inc.
#
#  b) The C or C++ interface to Oracle 8.x or greater, from
#     Oracle Corporation or IBM DB2 UDB.
#
# ***************************************************************************
#
#************************************DOCUMENTATION***************************
#Given 
#INPUT files
#
#1) latest exchange file (for example "exchange_20000201_tab.txt") from SGD at 
#ftp://genome-ftp.stanford.edu/pub/yeast/SGD-YPD/exchange.20000201.tab
#Note: you should first remove the header line that describes columns and and blank
#lines at the end of the file.
#This table is of the tab-delimited form 
#SGDID   
#Gene Name (SGD) 
#Alias (SGD)     
#ORF Name        
#Protein_ID 
#SwissProt       
#PIR     
#YPDID   
#Name (YPD)      
#Alias (YPD) 
#Comments
#
#
#
#OUTPUT files
#THUMPER_log.txt records run time events and lists all other output files
#created by the run that include:
#1) a list of names and the number of times that they appear in sgd (ie >1 is
#not a unique name)
#2) name_sgdid table
#3) sgdid_name table
#4) sgdid_accession number table
#5) sgdid_aliases table
#6) searchable names table (a list of aliases suitable for searching against MedLine)
#This last list "yeast_names.txt" is the input for the PreBIND parser
#Names that were originally in brackets in the SGD list (non-official names) are
#appended with 3 stars (for example see SIT1 versus SIT1***).
#Open Reading frame names of the form YDL003C are not included here since they are
#already included by PreBIND_parser when it parses through RefSeq.
#*************************END OF DOCUMENTATION*********************************

#******************************************************************************
# starting with "exchange_20000201.txt", generate a number of lists in hashes
#name_SGDID (using the name (YPD) field)
#SGDID_name (in case some names are redundant)
#SGDID_version (version is found as protein_ID in this Table)
#SGDID_Aliases



#open a log file for recording run-time events
open (LOG, ">YNG_log.txt")|| die "Cannot open file ABS_log.txt\n";

#initialize variables
$input_filename_1="exchange_20010321_tab.txt";


$separator=("*" x 85)."\n";


#LOG stuff
print "\nYeastNameGenerator has started at: ", scalar localtime, "\n\n";
print LOG scalar localtime,": Start YeastNameGenerator \n\n\n";

#******************************************************************************
#Loop through the first input file and put it into non-redundant hashes 
open (INPUT, "$input_filename_1")||die "Cannot open file $input_filename_1: $!\n";
print LOG $separator;
print LOG scalar localtime,": input_filename: $input_filename_1\n\n";
$counter=0;
while (<INPUT>)
{
	chomp;

	#Split the line according to tabs
	($a,$b,$c,$d,$e,$f,$g,$h,$i,$j,$k)=split (/\t/);
	#these variables are for future use; only $a, $i, $e and $j are used in this program
	#a=SGDID   
	#b=Gene Name (SGD) 
	#c=Alias (SGD)     
	#d=ORF Name        
	#e=Protein_ID (this is the version number ie accesion.x))
	#f=SwissProt       
	#g=PIR     
	#h=YPDID   
	#i=Name (YPD)      
	#j=Aliases (YPD)
	#k=comments


	#convert the Protein_ID to an accession (remove .x)
	$e=~s/\.\d+//; 
	

	#Save the data into non-redundant hashes
	++$name_unique{$i}; #any non-unique name in this list will have a 2 beside it
	$name_sgdid{$i}=$a;
	$sgdid_name{$a}=$i;
	$sgdid_accession{$a}=$e;
	$sdgid_aliases{$a}=$j;

	#put the aliases into an array
	@aliases = split("/", $j);
	#add individual aliases to a hash list of names and sgdid numbers called long_name_list_sgdid
	foreach (@aliases){
		next if /^\(/; #do not include names surrounded by brackets
		$long_name_list_sgdid{$_}=$a;
		++$aliases_unique{$_ }; #any non-unique name will have a 2 beside it
	}
	#add individual aliases to a hash list of names and sgdid numbers called searchable_names_sgdid
	#these are a subset of aliases that are suitable for searching MedLine 
	#some exclusion-lines are commented out on purpose (see comments below)
	
	foreach (@aliases){
		#next if /^\(/; 				#do not include names surrounded by brackets
		if (/^\(/){					#oR include names with brackets but *** them
			$no_brackets=$_;
			$no_brackets=~s/[\),\(]//g;
			$_=$no_brackets."***";
		}
					
		#if (/^[A-Z]{1,2}[0-9]{3,9}/){			#testing area
			#print LOG "This name could be deleted: $_\n";
		#}
		
		next if /^[A-Z]{1,2}[0-9]{3,9}/;	#do not include names that are primarily numerical
		next if /^Y[A-P][LR][0-9\.\-]{3,9}/;	#do not include yeast ORF names (note 'W' or 'C' does 
							#not always appear at end of yeast ORF names
		next if /\./;				#do not include names with periods in them
		next if /^YM[0-9]+\b/; 			#do not inlude YM00000 names
		next if /^[A-Z]{1}[0-9]+\b/;		#do not inlude names with only one letter at the beginning
		$searchable_names_sgdid{$_}=$a;		#add the name to the list
							#and then also
		#for starred names do the following
		if (/\*{3}/){
			if (/^[A-Z]{3}[0-9]{1,3}\b/){	#if its a name of proper nomenclature
			s/\*//g;			#remove the stars
			$little = lc$_."p";		#change to lower case and add a 'p'
			$little = $little."***";	#add the stars back
			$Proteinnamep = ucfirst$little;	#make the first letter capital
			$searchable_names_sgdid{$Proteinnamep}=$a;#add the name to the searchable list
			}
			next;
		}


		if (/^[A-Z]{3}[0-9]{1,3}\b/){		#if its a name of proper nomenclature eg. STO1
			$little = lc$_."p";		#change to lower case and add a 'p'
			$Proteinnamep = ucfirst$little; #make the first letter capital eg. Sto1p
			$searchable_names_sgdid{$Proteinnamep}=$a;#add the name to the searchable list
			next;
		}



	}
	
	++$counter;
}

print LOG scalar localtime,": Read $counter lines from input file to 5 hashes\n\n";
close INPUT;

#print hash lists to files
&print_hash(*name_unique, "name_unique");
&print_hash(*name_sgdid, "name_sgdid");
&print_hash(*sgdid_name, "sgdid_name");
&print_hash(*sgdid_accession, "sgdid_accession");
&print_hash(*sdgid_aliases, "sdgid_aliases");
&print_hash(*long_name_list_sgdid, "long_list_of_names_sgdid");
&print_hash(*aliases_unique, "aliases_unique");
&print_hash(*searchable_names_sgdid, "yeast_names");
print LOG $separator, "\n\n\n";




#*******************************************************************************
#                             sub print_hash
#*******************************************************************************


sub print_hash
{
#a hash is passed to this subroutine for printing to a file named 'hash'.txt
local(*hash)=$_[0];
my($hash_name)=$_[1];
my(@keys_to_hash, $items, $filename, $current_key);


#get the keys to the hash and count them
@keys_to_hash=keys(%hash);
$items=@keys_to_hash;
print LOG scalar localtime, ": Number of non-redundant entries in hash: $hash_name is $items\n"; 

#print the nonredundant array to the file 'hash_name'.txt 
$filename="$hash_name".".txt";
open (HASH_FILE, ">$filename")||die "Cannot open file $filename: $!\n";
foreach $current_key (@keys_to_hash)
{
	print HASH_FILE "$current_key"."|"."$hash{$current_key}\n";
}
print LOG scalar localtime, ": Hash: $hash_name printed to $filename\n"; 

close HASH_FILE;

}




