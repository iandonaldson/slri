#!/usr/bin/perl -w 
use SeqHound;
use POSIX qw(strftime);

#======================================================================
#This script is used for blueprint SeqHound data validation project.
#The purpose of this project is to validate the data in SeqHound by
#comparing the result query on SeqHound and NCBI.
#
#In order to run this script, you should also put three files in your
#current working directory:
# - eutils_efetch.pl (modified from NCBI script eutils_example.pl)
# - SeqHound.pm (SeqHound remote perl module file)
# - input (a configuration file contains the test cases)
#
#To execute the script from the command line, please follow the usage
#statement in this script. Example:
#   perl sh_efetch.pl all input    or
#   perl sh_efetch.pl SHoundGetFasta input
#
#Author: Rong Yao (ryao@blueprint.org)
#Date  : August 2004
#======================================================================

open LOG, ">>sh_efetch.log";
my $now =strftime("%a %b %e %H:%M:%S %Y", localtime);
print LOG "\nsh_efetch.pl run:  $now\n";

select (LOG);
$|++;
select (STDOUT);

#######format in the log file#########
my ($fnct, $status, $input);
format LOG_TOP =
FUNCTION					RESULT		TEST CASE
--------                    			------          ---------
.

format LOG =
@<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<	@<<<<<<	    	@<<<<<<<<<<
$fnct,                             $status,    $input
.

######logging function, writes using the above format##########
sub logger
{
	my $ret = shift;
	$fnct = shift;
	$input = shift;
	if ("TRUE" eq $ret)
	{
		$status = "diff";
	}
	else
	{
		$status = "same";
	}
	write LOG;
}


SHoundInit("TRUE", "sh_efetch.pl");
my ($retVal, $func, $diff);
my $flag = "FALSE";
my ($db, $query, $report, $action);



sub testSHoundGetGenBankff
{
    	$func = "SHoundGetGenBankff";
    	print "efetch test $func\n";
    	my $file_A = "$query"."\.sh";
    	my $file_B = "$query"."\_efetch";
    
	$retVal = &{"$func"} ($query);
	$retVal = $retVal."\n";
	writeSHoundResult($retVal, $file_A);
	system("./eutils_efetch.pl -d $db -q $query -r $report > $file_B ");
    	compare ($func, $file_A, $file_B, $query);
    	cleanup ($func, $file_A, $file_B);
}

sub testSHoundGetFasta
{
	$func = "SHoundGetFasta";
	print "efetch test $func\n";
	my $file_A = "$query"."\.sh";
	my $file_B = "$query"."\_efetch";
    
	$retVal = &{$func} ($query);
    	writeSHoundResult($retVal, $file_A);
	
	system ("./eutils_efetch.pl -d $db -q $query -r $report > out");
    	#since the format of Fasta from efetch and SeqHound has minor
    	#difference (newline, extra characters), need process efetch 
    	#result before to do file comparation.
    	open (IN, "out");
    	my $line = <IN>;
    	my $out = "";
    	while ( my $next = <IN>)
    	{
       		chomp $next;
       		$out = $out.$next;
    	}
    	my $result = $line.$out;
    	open (OUTPUT, ">$file_B");
    	print OUTPUT "$result";
    	close IN; close OUTPUT;
	system ("rm out");
    	compare ($func, $file_A, $file_B, $query);
    	cleanup ($func, $file_A, $file_B);
}

sub testSHoundAccFromGi
{
    	$func = "SHoundAccFromGi";
	print "efetch test $func\n";
	my $file_A = "$query"."\.sh";
	my $file_B = "$query"."\_efetch";
   
	$retVal = &{"$func"} ($query);
        #In SeqHound accdb, the records from PDB record do not have 
    	#accession, instead, retrieve name from accdb which is equivalent
    	#to the accession in NCBI protein database 
	if(!defined $retVal || ($retVal eq "n/a")){
		$retVal = SHoundGetNameByGi($query);
	}
    	writeSHoundResult($retVal, $file_A);
	
	system ("./eutils_efetch.pl -d $db -q $query -r $report > out");
	#Since SeqHound only stores the up-to-date access and not 
    	#includes version, so need process efetch output (remove
	#the version part) before to do file comparation.
	my $accession;
	open (IN, "out");
	my $line = <IN>;
	if ($line =~ /\.\d+/){
		my $len = length($line);
		$accession = substr ($line, 0, $len-2);
	}
	else {$accession = $line;}
	open (OUTPUT, ">$file_B");
	print OUTPUT "$accession";
	close IN; close OUTPUT;
    	system ("rm out");
	compare ($func, $file_A, $file_B, $query);
	cleanup ($func, $file_A, $file_B);
}

sub testSHoundGetTaxNameFromTaxID
{
    	$func = "SHoundGetTaxNameFromTaxID";
    	print "efetch test $func\n";
    	my $file_A = "$query"."\.sh";
    	my $file_B = "$query"."\_efetch";
    
    	$retVal = &{"$func"} ($query); 
    	writeSHoundResult($retVal, $file_A);
    	system("./eutils_efetch.pl -d $db -q $query -r $report > $file_B ");
    	compare ($func, $file_A, $file_B, $query);
    	cleanup ($func, $file_A, $file_B);
}

sub testSHoundFindAcc
{
    	$func = "SHoundFindAcc";
    	print "efetch test $func\n";
    	my $file_A = "$query"."\.sh";
    	my $file_B = "$query"."\_efetch";
    
    	$retVal = &{"$func"} ($query); 
    	writeSHoundResult($retVal, $file_A);
    	system("./eutils_efetch.pl -d $db -q $query -r $report > $file_B ");
    	compare ($func, $file_A, $file_B, $query);
    	cleanup ($func, $file_A, $file_B);
}

sub testSHoundGetSeqEntry
{
    	$func = "SHoundGetSeqEntry";
    	print "efetch test $func\n";
    	my $file_A = "$query"."\.sh";
    	my $file_B = "$query"."\_efetch";
    
    	$retVal = &{"$func"} ($query); 
	$retVal=$retVal."\n";
    	writeSHoundResult($retVal, $file_A);
    	system("./eutils_efetch.pl -d $db -q $query -r $report > $file_B ");
    	compare ($func, $file_A, $file_B, $query);
    	cleanup ($func, $file_A, $file_B);
}

#a subroutine to write SeqHound return result to a file
sub writeSHoundResult
{
	my ($ret, $file_A) = @_;
    	open (FILEOUT, ">$file_A");
	print FILEOUT "$ret";
	close (FILEOUT);
}
	

#a subroutine to organize data
#create necessary dirctories and put data files in this location
sub cleanup
{
	my ($func, $file_A, $file_B) = @_;
	if ((-d "./Data/") && (-d "./Data/$func/")){
		if (-s $diff){
			system("mv $file_A $file_B $diff ./Data/$func/");
		}
		else {
			system("mv $file_A $file_B ./Data/$func/");
		}
	}
	else{
		if (-d "./Data/")
		{
			system ("mkdir ./Data/$func");
	        	if (-s $diff) {
				system("mv $file_A $file_B $diff ./Data/$func/");
		
			}
			else {
				system("mv $file_A $file_B ./Data/$func/");
			}
		}
		else {
			system ("mkdir ./Data");
			system ("mkdir ./Data/$func");
			if (-s $diff) {
				system ("mv $file_A $file_B $diff ./Data/$func/");
			}
			else {
				system ("mv $file_A $file_B ./Data/$func/");
			}
			
		}
	}
}


#a subroutine to compare two files
#write the comparation result to the log file
sub compare
{
   	my ($func, $file_A, $file_B, $query) = @_;
   	$diff = $query."_diff";
   	if (system ("diff $file_A $file_B > $diff "))
   	{
        	#if diff file exists and has non-zero size
	    	if ((-e $diff) && (-s $diff))
	    	{ 
            		print "has difference in two files\n";	
			$flag = "TRUE";
		    	logger ($flag, $func, $query);
	    	}
	}
    	else{
		print "two files are the same\n";
		logger ($flag, $func, $query);
		system ("rm $diff");
	}
	$flag = "FALSE";
}


#a subroutine to print out usage notice
sub usage
{
	print STDOUT "Usage: \n";
	print STDOUT "\t ./sh_efetch.pl function_name|all input_file\n";
}

#a subroutine to get arguments from the a data line
#input line as : db = nucleuotide;query = 555; report = gb
#parse these information to three variables $db, $query and $report
sub get_args
{
	my ($line) = @_;
	print "$line\n";
	my @args = split(/;/, $line);
	$args[0]=~ m/(.*\=)(.*)$/;
	$db = $2;
	$args[1]=~ /(.*\=)(.*)$/;
	$query = $2;
	$args[2]=~ /(.*\=)(.*)$/;
	$report = $2;
}

#a subroutine to get test case from configuration file
sub testhandler
{
	my $funcname= shift;
        open (IF, "<$ARGV[1]");

	while (my $line = <IF>)
	{
		chomp $line;
                #find the function test case in the config file, extract db, query, report informations
		if ($line=~ /$funcname/)
		{
			my $nextline;
			while( ($nextline = <IF>) && ($nextline !~ /^$/) )
		        {
			        chomp $nextline;
			        get_args($nextline, $db, $query, $report);
					if ($report eq "-"){
						if ($funcname =~ /SHoundGetGenBankff/){
							if ($db eq "nucleotide" ){
								$report = "gb";
							}
							else {
								$report = "gp";
							}
						}elsif ($funcname =~ /SHoundGetFasta/){
							$report = "fasta";
						}elsif ($funcname =~ /SHoundAccFromGi/){
							$report = "acc";
						}elsif ($funcname =~ /SHoundGetSeqEntry/){
							$report = "native";
						}
					}
			        &{$action}();
			}
		}
	}
        close IF;
}

########main function########

if ($#ARGV == -1) 
   { usage(); exit;}
if ($#ARGV == 1) {
	if ($ARGV[0] eq "all") {
    	    	print "test all functions.\n"; 
		my @list = ("SHoundGetFasta", "SHoundFindAcc", "SHoundAccFromGi", "SHoundGetGenBankff", "SHoundGetSeqEntry", "SHoundGetTaxNameFromTaxID");
		foreach my $fnct (@list){
			$action = "test".$fnct;
			testhandler($fnct);
		}
	}
	elsif ($ARGV[0] =~ /SHound/){
		$action = "test".$ARGV[0];
		my $dir = "./Data/".$ARGV[0];
		if (-d $dir){
			system ("rm -fr $dir");
		}
		testhandler($ARGV[0]);
	}
}
else { usage(); exit;}
   

