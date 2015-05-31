#!/usr/bin/perl 
###########################################################################################
# Application that tests the SeqHound remote perl API functions.
# Remote Perl Functionalities are in SeqHound.pm
#
# This test script should go with a test input file, put them in the same directory.
# The input test file formatted as following:
# functionname
# key1=value1;key2=value2
# key1=value1;key2=value2
# ...
# 
# a function name followed by a list of test cases. if the function takes more one parameters
# each key-value pair separated by semicolon.
#
# Detailed test results are recorded in perl_test.log, as well as a test summary log. 
#
# Author: Rong Yao
#
# Usage: ./test_internal.pl input_file
###########################################################################################	

use POSIX qw(strftime);
use SeqHound;

### autoflushes LOG, writes to STDOUT
open LOG, ">perl_test.log";
open FILE, ">test_summary.log";

my $now =strftime("%a %b %e %H:%M:%S %Y", localtime);
print LOG "Perl test script starting:  $now\n";
print LOG "====================================================\n";

print FILE "Perl test script starting:  $now\n";
print FILE "====================================================\n";


select (LOG);
$|++;
select (STDOUT);

########### pretty print formatting ###########

my ($fnct, $status, $input);
format LOG_TOP = 
FUNCTION				       		 RESULT		INPUT
--------				        	 ------		-----
.

format LOG = 
@<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<	 @<<<<<<	@<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
$fnct,					                 $status,	$input
.

format FILE_TOP = 
FUNCTION				       		 RESULT		
--------				        	 ------		
.

format FILE = 
@<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<	 @<<<<<<	
$fnct,					                 $status
.


############ end of pretty print formatting


###### logging function, writes using the format above.
sub logger 
{
   
	$fnct = shift;
        $status = shift;
        $input = shift;
        #print "in logger: $fnct, $status,  $input\n";
        write LOG;
}


sub summary_logger
{
        $fnct = shift;
	$status = shift;
	write FILE;
}

my ($line, $nextline, $status, @inputs, @failed);

sub test_process {
    
	open (CONFIG, "<$ARGV[0]") || die "cannot open input test file for read. exit";

    	while($line = <CONFIG>)
    	{
   		chomp $line;
		if ($line =~ /SHound/) 
		{
	    		my $funcname = $line;
	   		print "$funcname...\n";
	    		my $pass_flag = "FALSE";

	    		while  (($nextline = <CONFIG>) &&  ($nextline =~ /./) )
	    		{ 
				$status = "failed";
				@inputs = get_args($nextline);
				if (scalar @inputs == 1){
					my $ret = undef;
					#print "1....get arg: $inputs[0]\n";
					if ($funcname =~ /KeyedList/){
					        my @list = split(/,/, $inputs[0]);
                            			my %hash = &{"$funcname"} (@list);
						while (my($key, $value)=each(%hash))
						{
#print "$key=> $value\n";
						}
						$ret = keys (%hash);
					}
					else {
					        if ($funcname =~ /List/){
					        	my @list = split(/,/, $inputs[0]);
		    		 			$ret = &{"$funcname"} (@list);
		    					#print "return $ret\n";
						}
						else {
							$ret = &{"$funcname"} ($inputs[0]);
#	if (defined $ret){
#print "return $ret\n";}
						}
					}
		    			if (defined $ret) {
		         			$status = "ok";
						$pass_flag="TRUE";
						logger ($funcname, $status, $inputs[0]);
		    			}
					else {
						logger ($funcname, $status, $inputs[0]);
					}
		    
				}
				if (scalar @inputs == 2){
#print "2......get arg: $inputs[0], $inputs[1]\n";
		    			my $ret = undef;
					if ($funcname =~ /KeyedList/){
						my @list = split(/,/, $inputs[0]);
                            			my %hash = &{"$funcname"} ($inputs[1], @list);
						#while (my($key, $value)=each(%hash))
						#{
							#print "$key=> $value\n";
						#}
						$ret = keys (%hash);
					}
					else {
		    				if ($funcname =~ /List/) {
		         				my @list = split(/,/, $inputs[0]);
			 				$ret = &{"$funcname"} ($inputs[1], @list); 
		    				}
		   				else {
		        				$ret = &{"$funcname"} ($inputs[0], $inputs[1]);
#print "*$ret\n";
		    				}
					}
		    			if (defined $ret) {
		        			$status = "ok";
						$pass_flag="TRUE";
						logger ($funcname, $status, $inputs[0]." & $inputs[1]");
		    			}
					else {
						logger ($funcname, $status, $inputs[0]." & $inputs[1]");
					}
				}
				if (scalar @inputs == 3){
		    			#print "3.......get arg: $inputs[0], $inputs[1], $inputs[2]\n";
					my $ret = undef;
		    			$ret = &{"$funcname"} ($inputs[0], $inputs[1], $inputs[2]);
		    			if (defined $ret) {
		        			$status = "ok";
						$pass_flag="TRUE";
						logger ($funcname, $status, $inputs[0]." & $inputs[1] & $inputs[2]");
		    			}
					else {
						logger ($funcname, $status, $inputs[0]." & $inputs[1] & $inputs[2]");
					}
				}
				
	    		}#end of while
			if ($pass_flag eq "FALSE"){
			        summary_logger ($funcname, "failed");
				push (@failed, $funcname);
			}
			else {
			        summary_logger ($funcname, "ok");
			}

        	}#end of if($line =~ /SHound/)
    	} #end of while    
}	
	

close CONFIG;


 ###########################################################################################
 #a subroutine to get arguments 
 #input line as key1=value1;key2=value2...where keys are the parametres for SeqHound function 
 #for example: gi=123;ecode=NR
 #parse values and store value in a ret array
 ############################################################################################
 sub get_args
 {
     my ($line) = @_;
     #print "$line\n";
     my @ret;
     my @args = split(/;/, $line);
     if (@args > 0){
         foreach my $i (@args) {
	     $i =~ /(.*\=)(.*)$/;
	     @ret = (@ret, $2);
	  }
     }
     return @ret;
 }  
   
   
 sub usage {
     print STDOUT "Usage: \n";
     print STDOUT "\t ./test.pl  file\n";
 }
   
 #############main function#################
 if ($#ARGV != 0)
 {
     usage();
     exit;
 }
 else
 {
     test_process ($ARGV[0]);
 }
  
 print FILE "\n###############################################\n";
 print FILE "          		TEST SUMMARY                 ";
 print FILE "\n###############################################\n";
 if ($#failed >= 0){
     	print FILE "\n function failed in all test cases: \n";
	foreach my $i (@failed){
        	print FILE "$i\n";
     	}
 }
 else {
 	print FILE "\n All API functions passed test.\n";
 }
 print "Test Completed. Please see perl_test.log for details and test_summary.log for summary.\n";
       
   
   
