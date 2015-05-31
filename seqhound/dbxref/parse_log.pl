#!/usr/bin/perl

# $Id: parse_log.pl,v 1.1 2005/04/14 15:08:23 zwang Exp $


use lib '/home/rcavero/myperlmodules';
use strict;
use Time::localtime;

#####################################################################
#
# Parses log file to collect statistical info. of dbxrefgoa parsers 
#
#####################################################################

my $in_file = "xref_parser.log";
#my $stat_file = "xref_stat.log";
my $stat_file;
my $tmp_file = "xref_stat_tmp.log";

#my @organisms = ("SP", "TR", "LL", "SGD", "WB");
my @organisms = ("SP", "SGD", "WB");


my %time_nodes;		# a hash table for exec time on each node
my %count_nodes;	# a hash table for total count on each node
my %count_orgs;		# a hash table for total count for each org (entry) 

my $file_name;
my $rec_generated;
my $node;
my $count;
my $count_total;	# total count of all nodes
my $time;
my $time_avg;
my $time_total;
my $entry;		# e.g. SP_EMBL, SP_PFAM
my @words;


my $tm = localtime;
my ($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst)
= ($tm->sec, $tm->min, $tm->hour, $tm->mday, ($tm->mon)+1, ($tm->year)+1900, $tm->wday, $tm->yday, $tm->isdst);


	foreach my $org (@organisms) {
		#########################################
		# grep for each organism to a tmp file
		#########################################
		my $pattern = $org . "_";
		if (system("grep $pattern $in_file> $tmp_file") != 0) {
			print "Cannot grep $org > $tmp_file \n";
		}


		#########################################
		# open tmp file to parse
		#########################################
		open (TMPFILE, "$tmp_file") or die "Cannot open $tmp_file\n";
		while (<TMPFILE>) {
			chomp;
			my $line = $_;

			if ($line =~ m/FILE2PARSE/) {
				@words = split(/\t/, $line);
				$file_name = $words[1];
			}
			elsif ($line =~ m/NODE/) {
				@words = split(/\t/, $line);
				$node = $words[1];
			}
			elsif ($line =~ m/TOTAL/) {
				@words = split(/\t/, $line);
				$count = $words[1];
				$count_total += $count;
				$count_nodes{$node} = $count;
			}
			elsif ($line =~ m/TIME/) {
				@words = split(/\t/, $line);
				$time = $words[1];
				$time_total += $time;
				$time_nodes{$node} = $time;
			}
			else {
				##############################
				# for entries such as SP_EMBL
				##############################
				@words = split(/\t/, $line);
				$entry = $words[0]; 
				$count = $words[1];
				$count_orgs{$entry} += $count;
			} # else
			
		} # while (<TMPFILE>)


		#############################
		# print stat.
		############################
		$stat_file = "xref_stat" . $hour . $min . $sec . "log";
		open (STAT_FILE, ">>$stat_file") or die "Cannot open $stat_file for write\n";
		printf (STAT_FILE "\n==========================================================\n");
		printf (STAT_FILE "                      $org\n");
		printf (STAT_FILE "==========================================================\n");
		printf (STAT_FILE "File: $file_name\n");
		printf (STAT_FILE "Total # of records generated: %10ld\n", $count_total);
		my $number_node =  scalar(keys %count_nodes);
		printf (STAT_FILE "Total # of nodes:             %10ld\n", $number_node);
		my $number_org =  scalar(keys %count_orgs);
		printf (STAT_FILE "Total # of organisms:         %10ld\n", $number_org);

		printf (STAT_FILE "Total exec time on all nodes: %10.2f (min)\n", $time_total/60);
		printf (STAT_FILE "Avg exec time on each node:   %10.2f (min)\n", ($time_total/$number_node)/60);
		printf (STAT_FILE "-------------------------------------------------------\n"); 

		## count of each org over all nodes
		printf (STAT_FILE "\n%15s%10s\n", "organism", "count"); 
		printf (STAT_FILE "-------------------------------------------------------\n"); 
		foreach (keys %count_orgs) {
			@words = split(/_/, $_);
			printf (STAT_FILE "%15s%10ld\n", $words[1], $count_orgs{$_}); 
		}
		printf (STAT_FILE "-------------------------------------------------------\n"); 
		## exec time on each node
		printf (STAT_FILE "\n%15s%10s%15s\n", "node", "count", "time(min)"); 
		printf (STAT_FILE "-------------------------------------------------------\n"); 
		foreach (keys %count_nodes) {
			my $time = ($time_nodes{$_})/60;
			printf (STAT_FILE "%15s%10ld%15.2f\n", $_, $count_nodes{$_}, $time); 
		}
	} # foreach $org


 
exit(0);



# $Log: parse_log.pl,v $
# Revision 1.1  2005/04/14 15:08:23  zwang
# A script to parse the logs of the parsers of dbxref.
#
