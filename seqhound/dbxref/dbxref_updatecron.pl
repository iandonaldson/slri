#!/usr/bin/perl

##########################################################################################
#
# Update dbxref files as a CRON job using wget
#
# Commmand syntax: perl dbxref_upodatecron.pl
# 	dbxref.ini must in the same directory 
# 	cron_input_file specified in dbxref.ini file must be in place
# How it works;
#	Put this parser and other parser together in a directory called $parser_dir 
# 	For each $organism( there is a parser named dbxref_$organism.pl)
#		1. use wget to download the files to $parser_dir/download/$organism/wget/
#		2. compare the files in $parser_dir/download/$organism/wget/ to files in $parser_dir/download/$organism/
#		3. if there is new(newer) file, copy the new(newer) file over to $parser_dir/download/$organism/
#		4. unzip the new file if it is a zip file
#		5. if any file been updated, run the parser dbxref_$organism.pl
# Notes:
#		- parser will read files in $parser_dir/download/$organism/, 
# 		- the file path is also specified in dbxref.ini, And organism names should be the same as in dbxref_cron.ini
#		- if there is any change in the above logic, dbref.ini should be modified accordingly
#
##########################################################################################
use strict;
use lib '.';
use dbxref;

use File::stat;
$|=1;

my $ini="dbxref.ini";
my $cmd_opts=">> dbxref_updatecron.log 2>> dbxref_errors.log";

# open update log file
my $updatelog = dbxref->getProperty($ini, "update_log");
open (LOGFILE, ">>$updatelog") || 
    die "cannot open log file $updatelog for write: $!\n";
print LOGFILE '*'x80, "\n[ ", scalar localtime, " ] : update starts.\n",'*'x80, "\n\n";

#set up send mail command
my $myemail= dbxref->getProperty($ini, "myemail");
# RAC Waiting to install mailx under hydrogen
 my $mymail = "| /usr/bin/mailx -s \"dbxref_goa update cron status\"  ".$myemail;

 open(MAIL,$mymail);
 print LOGFILE "opening email message.\n";
 print MAIL '*'x80, "\n[ ", scalar localtime, " ] : update starts.\n",'*'x80, "\n\n";
###### End Waiting.


my $line=dbxref->getProperty($ini, "ORGANISMS");
#print("line: $line\n");
$line=~s/\s//g;
die "ORGANISM list Error in $ini !!! \n" if($line !~/((\S+;)*\S+)/);
my @ORGS= split /;/, $1;

# RAC download in the luster needs to point to /scratch/dbxrefgoa
#my $download ="download";
my $download = dbxref->getProperty($ini, "data_dir");

print ("download: $download \n");

my $wget ="wget";

#print("ORGS: @ORGS \n");
my $org;
foreach $org(@ORGS)
{
	chomp $org;
	#print("org: $org \n");
	next if $org=~/^$/;

    #print "\n========= $org =========\n";
	print LOGFILE "\n========= $org =========\n";
	print MAIL "\n========= $org =========\n";
	my $update=0;
	$line=dbxref->getProperty($ini, $org."_URL");
	$line=~s/\s//;
	#print ("line: $line\n");
	next if $line!~/(\S+;)*\S+/;
	my @URLS= split /;/,$line;

	#for DBXREF files, put them in seperated directory
	#for GOA files, put them in same directory download/GOA
	my $org_dir=$org;
   #if ($org_dir=~/^GOA/)
   #	{
   #		$org_dir = "GOA";
   #	}
	my $url;
	
	foreach $url (@URLS)
	{
		chomp $url;
		#print ("url: $url\n");
		next if $url=~/^$/;
	        # print LOGFILE "checking URL: $url\n";
		# check timestamp, tmp dir for each organism, 
		# append to log, non_verbose
		# RAC different sub-directory in cluster
		#system("/usr/sfw/bin/wget -N -nv -P $download/$org_dir/$wget -a $updatelog $url");
		system("/usr/bin/wget -N -nv -P $download/$org_dir/$wget -a $updatelog $url");
	}
	
	opendir(DIR, "$download/$org_dir/$wget");
	my @FILES= readdir(DIR);
	shift @FILES; #remove .
	shift @FILES; #remove ..
	closedir(DIR);
	
#print LOGFILE "./$download/$org_dir/$wget/", "@FILES", "\n";
	my $file;
	foreach $file(@FILES)
	{
#print LOGFILE "checking file: $file\n";
		if((not -e "$download/$org_dir/$file") || 
				(stat("$download/$org_dir/$wget/$file")->mtime > stat("$download/$org_dir/$file")->mtime))
		{
		        #print(" cp -p $download/$org_dir/$wget/$file $download/$org_dir/$file");
			# system("cp -p $download/$org_dir/$wget/$file/ $download/$org_dir/$file");
			# RAC remove this  unnecessary "/" -----------^
			system("cp -p $download/$org_dir/$wget/$file $download/$org_dir/$file");
			if($file =~/(.+)\.(gz|Z)$/i)
			{
				system("gunzip -c $download/$org_dir/$file >$download/$org_dir/$1");
			}
			print LOGFILE "File updated: $file\n";
			print MAIL "File updated: $file\n";
			$update=1;
		}
	}
	if($update)
	{
		print LOGFILE "[ ", scalar localtime, " ] : Update organism: $org.", "\n";
		print MAIL "[ ", scalar localtime, " ] : Update organism: $org.", "\n";
		$line = dbxref->getProperty($ini, $org."_CMD");
		#print "command= $line\n";
		next if $line=~/^$/;
		my $command = $line;
#print '*'x5, " $command ", '*'x5, "\n";
		# print LOGFILE "update command: $command $cmd_opts\n";
#print MAIL "update command: $command\n";
		system("$command $cmd_opts");
		print LOGFILE "[ ", scalar localtime, " ] : Finished update organism: $org.", "\n";
		print MAIL "[ ", scalar localtime, " ] : Finished update organism: $org.", "\n";
	}
	
}

# clean up and exit
print LOGFILE  '*'x80,"\n[ ", scalar localtime, " ] : update stops.\n",'*'x80, "\n\n";
print MAIL  '*'x80,"\n[ ", scalar localtime, " ] : update stops.\n",'*'x80, "\n\n";
close (MAIL);
close (LOGFILE);
exit (0);


