#!/usr/bin/perl -w
use SeqHound;
use POSIX qw(strftime);

#=====================================================================
#This script is used for blueprint SeqHound data validation project.
#The purpose of this project is to validate the data in SeqHound by
#comparing the result query on SeqHound and NCBI.
#
#In order to run this script, you should also put three files in your
#current working directory:
# - binary id1_fetch (compiled from NCBI_CXX toolkit)
# - SeqHound.pm (SeqHound remote perl module file)
# - input (a configuration file contains the test cases)
#
#To execute the script from the command line, please follow the usage
#statement in this script.
#
#Author: Rong Yao (ryao@blueprint.org)
#Date  : August 2004
#=====================================================================

open LOG, ">>sh_id1fetch.log";
my $now =strftime("%a %b %e %H:%M:%S %Y", localtime);
print LOG "\nsh_id1fetch.pl run $now\n";
select (LOG);
$|++;
select (STDOUT);

#######format in the log file#########
my ($fnct, $status, $input);
format LOG_TOP =
FUNCTION									RESULT			TEST CASE
--------                    				------          ---------
.

format LOG =
@<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<	@<<<<<<	    	@<<<<<<
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


SHoundInit("TRUE", "sh_id1fetch.pl");
my ($retVal, $func);
my ($db, $query, $fmt, $action);
my $flag = "FALSE";


sub testSHoundGetGenBankff
{
    $func = "SHoundGetGenBankff";
    print "id1fetch test $func\n";
	my $file_A = "$query"."\.sh";
	my $file_B = "$query"."\_id1";
	
    $retVal = &{"$func"} ($query);
    writeSHoundResult($retVal, $file_A);
	
	getFormat();
	system ("./id1_fetch -db $db -gi $query -fmt $fmt -out $file_B");
    compare ($func, $file_A, $file_B, $query);
    cleanup ($func, $file_A, $file_B);
}


sub testSHoundGetFasta
{
	$func = "SHoundGetFasta";
	print "id1fetch test $func\n";
	my $file_A = "$query"."\.sh";
	my $file_B = "$query"."\_id1";
    
	$retVal = &{$func} ($query);
	writeSHoundResult($retVal, $file_A);
	
	system ("./id1_fetch -db $db -gi $query -fmt $fmt > out");
    #since the format of Fasta from efetch and SeqHound has minor
    #difference (newline, extra characters), need process efetch 
    #result before to do file comparation.
    open (IN, "out");
    my $line = <IN>;
    #$line =~ s/\-/ /g; 
    my $out = "";
    while ( my $next = <IN>)
    {
       chomp $next;
       $out = $out.$next;
    }
    my $result = $line.$out;
    open (OUTPUT, ">$file_B");
    print OUTPUT "$result\n";
    close IN; close OUTPUT;
	system ("rm out");
    compare ($func, $file_A, $file_B, $query);
    cleanup ($func, $file_A, $file_B);
}

#id1_fetch is not good for testint this function
sub unusedtestSHoundAccFromGi
{
    $func = "SHoundAccFromGi";
	print "id1fetch test $func\n";
	my $file_A = "$query"."\.sh";
	my $file_B = "$query"."\_id1";
   
	$retVal = &{"$func"} ($query);
    writeSHoundResult($retVal, $file_A);
	
	system ("./id1_fetch -db $db -gi $query -fmt $fmt > $file_B");
	compare ($func, $file_A, $file_B, $query);
	cleanup ($func, $file_A, $file_B);
}


#a subroutine to write SeqHound return result to a file
sub writeSHoundResult
{
	my ($ret, $file_A) = @_;
    open (FILEOUT, ">$file_A");
	print FILEOUT "$ret\n";
	close (FILEOUT);
}


#a subroutine to organize data
#create necessary directories and put data files in this location
sub cleanup
{
	my ($func, $file_A, $file_B) = @_;
    if ((-d "./Data_id1/") && (-d "./Data_id1/$func/")){
		system("mv $file_A $file_B ./Data_id1/$func/");
	}
	else{
		if (-d "./Data_id1/")
		{
			system ("mkdir ./Data_id1/$func");
		    system("mv $file_A $file_B ./Data_id1/$func/");
		}
		else {
			system ("mkdir ./Data_id1");
			system ("mkdir ./Data_id1/$func");
			system ("mv $file_A $file_B ./Data_id1/$func/");
		}
	} 
}

#a subroutine to compare two file
#write the comparation result to the log file
sub compare
{
   my ($func, $file_A, $file_B, $query) = @_;
   my $output = "output.txt";
   if (system ("diff $file_A $file_B > $output "))
   {
        #if out exists and has non-zero size
	    if ((-e $output) && (-s $output)){ 
            $flag = "TRUE";
		    logger ($flag, $func, $query); 
	        print "has difference in two files\n";
	    }
    }
    else{
	    print "two files are the same\n";
		logger ($flag, $func, $query);
	}
	system ("rm -f $output");
	$flag = "FALSE";
}


#sub getFormat a subroutine to get return format 
#-fmt <String, 'asm', 'asnb', 'docsum', 'fasta', 'genbank', 'genpept', 'quality', 'xml'>
sub getFormat
{
    if($fmt eq "gp")
    {
	    $fmt = "genpept";
    }
    if($fmt eq "gb")
    {
		$fmt = "genbank";
	}
}

#a subroutine to print out usage notice
sub usage
{
	print STDOUT "Usage: \n";
	print STDOUT "\t ./sh_id1fetch.pl SHound_function_name input_file\n";
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
	$fmt = $2;
}

#a subroutine to get test case from configuration file
sub testhandler
{
	my $funcname = $ARGV[0];
    open (IF, "<$ARGV[1]");

	while (my $line = <IF>)
	{
		chomp $line;
        #find the function test case in the config file, extract db, query, report format informations
		if ($line=~ /$funcname/)
		{
			my $nextline;
			while(( $nextline = <IF>) && ($nextline =~ /./))
			{
			   chomp $nextline;
			   get_args($nextline, $db, $query, $fmt); 
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
	if ($ARGV[0] eq "all") {  #this part has not been implemented yet
    	print "test all functions.\n";
	}
	elsif ($ARGV[0] =~ /SHound/){
		$action = "test".$ARGV[0];
		testhandler();
	}
}
else { usage(); exit;}

