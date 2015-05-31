#!/usr/bin/perl 
#E-utility Perl module
#NCBI PowerScripting
#EW Sayers 
#version 1.1, 10/26/04: added sub link_history, deleted link_post
#version 1.2, 1/21/04: revised esummary and print_summary to handle homologene docsum XML


# ===========================================================================
#
#                            PUBLIC DOMAIN NOTICE
#               National Center for Biotechnology Information
#
#  This software/database is a "United States Government Work" under the
#  terms of the United States Copyright Act.  It was written as part of
#  the author's official duties as a United States Government employee and
#  thus cannot be copyrighted.  This software/database is freely available
#  to the public for use. The National Library of Medicine and the U.S.
#  Government have not placed any restriction on its use or reproduction.
#
#  Although all reasonable efforts have been taken to ensure the accuracy
#  and reliability of the software and data, the NLM and the U.S.
#  Government do not and cannot warrant the performance or results that
#  may be obtained by using this software or data. The NLM and the U.S.
#  Government disclaim all warranties, express or implied, including
#  warranties of performance, merchantability or fitness for any particular
#  purpose.
#
#  Please cite the author in any work or product based on this material.
#
# ===========================================================================
#
# Author:  Eric W. Sayers  sayers@ncbi.nlm.nih.gov
# http://www.ncbi.nlm.nih.gov/Class/PowerTools/eutils/course.html
#
#  
# ---------------------------------------------------------------------------


#Contains the following subroutines:
#read_params
#egquery
#esearch
#esummary
#efetch
#efetch_batch
#elink
#elink_history
#epost_uids
#epost_file
#epost_set
#print_summary
#print_links
#print_link_summaries

package NCBI_PowerScripting;
use LWP::Simple;
use LWP::UserAgent;
use vars qw ($VERSION @ISA @EXPORT @EXPORT_OK);
require "ctime.pl";
use Exporter;

@ISA = qw (Exporter);
@EXPORT = qw (read_params egquery esearch esummary efetch efetch_batch elink
                 epost_uids epost_file elink_history epost_set print_summary 
                 print_links print_link_summaries );
		 
$VERSION = '1.1';

my $delay = 0;
my $maxdelay = 3;

#*************************************************************

sub read_params {

# Reads input parameters from file supplied on command line
# Input file must have lines of the following format:
#   parameter|value
# where parameter is the URL parameter name and value is the
# value to be assigned to parameter
# For ELink, the parameter "dbfrom" must be on a line before 
# the id parameters. This allows multiple &id parameters
# Input: file named on command line
# Output: %params; keys are parameter names, values are values
# Example: $params{'db'} = 'nucleotide'
# $params{'id'} is an array if "dbfrom" parameter is in input file

my ($param, $value);
my (@keys, @test);
my %mark;
my $dbfrom;

#check for correct command line syntax
if ($#ARGV != 0) { die "Usage: [eutil].pl input_file\n"; }

#read input parameter file
open(INPUT, "<$ARGV[0]") || die "Aborting. Can't open $ARGV[0]\n";

while (<INPUT>) {

   chomp;
   ($param, $value) = split(/\|/);
   if ($param eq 'dbfrom') { $dbfrom = 1; }
   if (($param eq 'id') && ($dbfrom)) {
      push (@{$params{$param}}, $value);
   }
   else {
      $params{$param} = $value;
   }
}

close INPUT;

return (%params);

}

#************************************************************************

sub egquery {
# Performs EGQuery.
# Input: %params:
# $params{'term'} - Entrez query
# $params{'tool'} - tool name
# $params{'email'} - e-mail address
# Output = %results; keys are databases, values are UID counts

my %params = @_;
my $base = "http://eutils.ncbi.nlm.nih.gov/entrez/eutils/";
my ($url, $raw);
my @out;
my $database;
my %results;
my ($begin, $end);

sleep($delay);

$url = $base . "egquery.fcgi?term=$params{'term'}";
$url .= "&tool=$params{'tool'}&email=$params{'email'}";

$begin = time;
$raw = get($url);

@out = split(/^/, $raw);

foreach (@out) {

   if (/<DbName>(.*)<\/DbName>/) { $database = $1; }
   if (/<Count>(\d+)<\/Count>/) { $results{$database} = $1; }

}

$end = time;
$delay = $maxdelay - ($end - $begin);
if ($delay < 0) { $delay = 0; }

return(%results);

}

#*********************************************************************

sub esearch {
# Performs ESearch. 
# Input: %params
# $params{'db'} - database
# $params{'term'} - Entrez query
# $params{'usehistory'} (y/n) - flag for using the Entrez history server
# $params{'retstart'} - first item in results list to display (default = 0)
# $params{'retmax'} - number of items in results list to display (default = 20)
# $params{'WebEnv'} - Web Environment for accessing existing data sets
# $params{'tool'} - tool name
# $params{'email'} - e-mail address
#
# Output: %results: keys are 'count', 'query_key', 'WebEnv', 'uids'
# $results{'uids'} is an array

my %params = @_;
my $base = "http://eutils.ncbi.nlm.nih.gov/entrez/eutils/";
my ($url, $raw);
my @out;
my %results;
my ($begin, $end);

sleep($delay);

$url = $base . "esearch.fcgi?db=$params{'db'}&term=$params{'term'}";
$url .= "&usehistory=$params{'usehistory'}&WebEnv=$params{'WebEnv'}";
$url .= "&retstart=$params{'retstart'}&retmax=$params{'retmax'}";
$url .= "&tool=$params{'tool'}&email=$params{'email'}";

$begin = time;
$raw = get($url);

$raw =~ /<Count>(\d+)<\/Count>/s;
$results{'count'} = $1; 
$raw =~ /<QueryKey>(\d+)<\/QueryKey>.*<WebEnv>(\S+)<\/WebEnv>/s;
$results{'query_key'} = $1 if ($params{'usehistory'} eq 'y');
$results{'WebEnv'} = $2;
@out = split(/^/, $raw);

foreach (@out) {
   if (/<Id>(\d+)<\/Id>/) { push (@{$results{'uids'}}, $1); }
}   

$end = time;
$delay = $maxdelay - ($end - $begin);
if ($delay < 0) { $delay = 0; }

return(%results);

}

#****************************************************************

sub esummary {
# Performs ESummary. 
# Input: %params:
# $params{'db'} - database
# $params{'id'} - UID list 
# $params{'query_key'} - query_key
# $params{'WebEnv'} - web environment
# $params{'retstart'} - first DocSum to retrieve
# $params{'retmax'} - number of DocSums to retrieve
# $params{'tool'} - tool name
# $params{'email'} - e-mail address
#
# Output: %results: $results{id}{item} = value
# where id = UID, item = Item Name

my %params = @_;
my $base = "http://eutils.ncbi.nlm.nih.gov/entrez/eutils/";
my ($url, $raw);
my @out;
my $id;
my %results;
my ($begin, $end);

sleep($delay);

$url = $base . "esummary.fcgi?db=$params{'db'}&retstart=$params{'retstart'}";
$url .= "&retmax=$params{'retmax'}";

if ($params{'query_key'}) {
   $url .= "&query_key=$params{'query_key'}&WebEnv=$params{'WebEnv'}";   
}
else {
   $url .= "&id=$params{'id'}";   
}

$url .= "&tool=$params{'tool'}&email=$params{'email'}";

$begin = time;
$raw = get($url);

@out = split(/^/, $raw);

if ($params{'db'} eq 'homologene') {

$results{'homologene'} = 'y';

 foreach (@out) {

   $id = $1 if (/<Id>(\d+)<\/Id>/);
   if (/<Item Name="(.+)" Type=.*>(.+)<\/Item>/) {
      push(@{$results{$id}{$1}}, $2);
   }   
 }
}

else {

 foreach (@out) {

   $id = $1 if (/<Id>(\d+)<\/Id>/);
   if (/<Item Name="(.+)" Type=.*>(.+)<\/Item>/) {
      $results{$id}{$1} = $2;
   }   

 }
}

$end = time;
$delay = $maxdelay - ($end - $begin);
if ($delay < 0) { $delay = 0; }

return(%results);

}

#****************************************************************

sub efetch {
# Performs EFetch. 
# Input: %params:
# $params{'db'} - database
# $params{'id'} - UID list 
# $params{'query_key'} - query key 
# $params{'WebEnv'} - web environment
# $params{'retmode'} - output data format
# $params{'rettype'} - output data record type
# $params{'retstart'} - first record in set to retrieve
# $params{'retmax'} - number of records to retrieve
# $params{'seq_start'} - retrieve sequence starting at this position
# $params{'seq_stop'} - retrieve sequence until this position
# $params{'strand'} - which DNA strand to retrieve (1=plus, 2=minus)
# $params{'complexity'} - determines what data object to retrieve
# $params{'tool'} - tool name
# $params{'email'} - e-mail address
#
# Output: $raw; raw EFetch output

my %params = @_;
my $base = "http://eutils.ncbi.nlm.nih.gov/entrez/eutils/";
my ($url, $raw);
my ($begin, $end);

sleep($delay);

$url = $base . "efetch.fcgi?db=$params{'db'}";

if ($params{'query_key'}) {
   $url .= "&query_key=$params{'query_key'}&WebEnv=$params{'WebEnv'}";   
}
else {
   $url .= "&id=$params{'id'}";   
}

$url .= "&retmode=$params{'retmode'}&rettype=$params{'rettype'}";
$url .= "&retstart=$params{'retstart'}&retmax=$params{'retmax'}";
$url .= "&seq_start=$params{'seq_start'}&seq_stop=$params{'seq_stop'}";
$url .= "&strand=$params{'strand'}&complexity=$params{'complexity'}";
$url .= "&tool=$params{'tool'}&email=$params{'email'}";

$begin = time;
$raw = get($url);

$end = time;
$delay = $maxdelay - ($end - $begin);
if ($delay < 0) { $delay = 0; }

return($raw);

}

#****************************************************************

sub efetch_batch {

# Uses efetch to download a large data set in 500 record batches
# The data set must be stored on the History server
# The output is sent to a file named $params{'outfile'}
# Input: %params:
# $params{'db'} - link to database
# $params{'query_key'} - query key
# $params{'WebEnv'} - web environment
# $params{'retmode'} - output data format
# $params{'rettype'} - output data record type
# $params{'seq_start'} - retrieve sequence starting at this position
# $params{'seq_stop'} - retrieve sequence until this position
# $params{'strand'} - which DNA strand to retrieve (1=plus, 2=minus)
# $params{'complexity'} - determines what data object to retrieve
# $params{'tool'} - tool name
# $params{'email'} - e-mail address
# $params{'outfile'} - name of output file
#
# Output: nothing returned; raw EFetch output sent to $params{'outfile'}
# Other output: periodic status messages sent to standard output

my %params = @_;
my $base = "http://eutils.ncbi.nlm.nih.gov/entrez/eutils/";
my ($url, $raw);
my ($begin, $end);
my %results;
my ($count, $limit);
my ($retstart, $retmax);

$retmax = 500;

#first use ESearch to determine the size of the dataset

$params{'term'} = "%23" . "$params{'query_key'}";
$params{'usehistory'} = 'y';

%results = esearch(%params);

$count = $results{'count'};
$params{'retmax'} = $retmax;

print "Retrieving $count records...\n";

open (OUT, ">$params{'outfile'}") || die "Aborting. Can't open $params{'outfile'}\n";

for ($retstart = 0; $retstart < $count; $retstart += $retmax) {

   sleep($delay);
   $params{'retstart'} = $retstart;
   $begin = time;
   $raw = efetch(%params);
   
   print OUT $raw;

   if ($retstart + $retmax > $count) { $limit = $count; }
   else { $limit = $retstart + $retmax; }
   
   print "Received records $retstart - $limit.\n";
   $end = time;
   $delay = $maxdelay - ($end - $begin);
   if ($delay < 0) { $delay = 0; }
}

close OUT;

}

#****************************************************************

sub elink {
# Performs ELink. 
# Input: %params:
# $params{'dbfrom'} - link from database
# $params{'db'} - link to database
# $params{'id'} - array of UID lists 
# $params{'query_key'} - query key
# $params{'WebEnv'} - web environment)
# $params{'term'} - Entrez term used to limit link results
# $params{'tool'} - tool name
# $params{'email'} - e-mail address
#
# Output: %links: 
# @{$links{'from'}{$set}} = array of input UIDs in set $set
# @{$links{'to'}{$db}{$set}} = array of linked UIDs in $db in set $set
# where $set = integer corresponding to one &id parameter
# value in the ELink URL

my %params = @_;
my $base = "http://eutils.ncbi.nlm.nih.gov/entrez/eutils/";
my ($url, $raw);
my ($line, $getdata, $getid, $link, $id, $set);
my @out;
my @link_ids;
my $ids;
my %results;
my $db;
my ($begin, $end);

sleep($delay);

$set = 0;

$url = $base . "elink.fcgi?dbfrom=$params{'dbfrom'}&db=$params{'db'}";
$url .= "&term=$params{'term'}";

if ($params{'query_key'}) {
   
   $url .= "&query_key=$params{'query_key'}&WebEnv=$params{'WebEnv'}";
      
}
else {

   foreach $ids (@{$params{'id'}}) {      
      $url .= "&id=$ids";
   }      
}
$url .= "&tool=$params{'tool'}&email=$params{'email'}";

$begin = time;
$raw = get($url);

@out = split(/^/,$raw);

$getdata = 0;

foreach $line (@out) {

#check for input UIDs
   $getid = 1 if ($line =~ /<IdList>/);
   if ($getid) {
      
      push (@{$results{'from'}{$set}}, $1) if ($line =~ /<Id>(\d+)<\/Id>/);
  
   }   
   $getid = 0 if ($line =~ /<\/IdList>/);

#check for linked UIDs   
   if ($line =~ /<DbTo>(\S+)<\/DbTo>/) {
      $db = $1;
      $getdata = 1;
   }
   $getdata = 0 if ($line =~ /<\/LinkSetDb>/);

   if ($getdata) {
          push (@{$results{'to'}{$db}{$set}}, $1) if ($line =~ /<Id>(\d+)<\/Id>/);
   }

   if ($line =~ /<\/LinkSet>/) {
      $getdata = 0;
      $set++;
   }	
		
}

$end = time;
$delay = $maxdelay - ($end - $begin);
if ($delay < 0) { $delay = 0; }

return(%results);

}

#************************************************************

sub elink_history {

# Uses ELink with &cmd=neighbor_history to post ELink results
# on the History server

# Input: %params:
# $params{'dbfrom'} - link from database
# $params{'db'} - link to database
# $params{'id'} - array of UID lists 
# $params{'query_key'} - query key
# $params{'WebEnv'} - web environment
# $params{'term'} - Entrez term used to limit link results
# $params{'tool'} - tool name
# $params{'email'} - e-mail address
#
# Output: %links: 
# @{$links{'from'}{$set}} = array of input UIDs in set $set
# $links{'to'}{$set}{$db}{'query_key'} = query_key of linked UIDs in $db in set $set
# $links{'WebEnv'} = Web Environment of linked UID sets
# where $set = integer corresponding to one &id parameter
# value in the ELink URL

my %params = @_;
my $base = "http://eutils.ncbi.nlm.nih.gov/entrez/eutils/";
my ($url, $raw);
my ($line, $getdata, $getid, $link, $id, $set, $name);
my @out;
my @link_ids;
my $ids;
my %results;
my $db;
my ($begin, $end);

sleep($delay);
$set = 0;

$url = $base . "elink.fcgi?dbfrom=$params{'dbfrom'}&db=$params{'db'}";
$url .= "&cmd=neighbor_history&term=$params{'term'}";

if ($params{'query_key'}) {
   
   $url .= "&query_key=$params{'query_key'}&WebEnv=$params{'WebEnv'}";
      
}
else {

   foreach $ids (@{$params{'id'}}) {      
      $url .= "&id=$ids";
   }      
}
$url .= "&tool=$params{'tool'}&email=$params{'email'}";

$begin = time;
$raw = get($url);

@out = split(/^/,$raw);

$getdata = 0;

foreach $line (@out) {

#check for input UIDs
   $getid = 1 if ($line =~ /<IdList>/);
   if ($getid) {
      
      push (@{$results{'from'}{$set}}, $1) if ($line =~ /<Id>(\d+)<\/Id>/);
  
   }   
   $getid = 0 if ($line =~ /<\/IdList>/);

#check for linked UIDs   
   if ($line =~ /<DbTo>(\S+)<\/DbTo>/) {
      $db = $1;
      $getdata = 1;
      $name = $params{'dbfrom'} . '_' . $db;
   }
   $getdata = 0 if ($line =~ /<\/LinkSetDbHistory>/);
   
   if ($line =~ /<LinkName>(\S+)<\/LinkName>/) {
      $getdata = 0 unless ($name eq $1);
   }   

   if ($line =~ /<QueryKey>(\d+)<\/QueryKey>/) {
      $results{'to'}{$set}{$db}{'query_key'} = $1 if ($getdata);
   }
   
   if ($line =~ /<WebEnv>(\S+)<\/WebEnv>/) {
      $results{'WebEnv'} = $1;
   }      

   if ($line =~ /<\/LinkSet>/) {
      $getdata = 0;
      $set++;
   }	
		
}

$end = time;
$delay = $maxdelay - ($end - $begin);
if ($delay < 0) { $delay = 0; }

return(%results);

}

#*********************************************************************

sub epost_uids {
# Performs EPost, placing UIDs in the URL. 
# Input: %params:
# $params{'db'} - database
# $params{'id'} - list of UIDs
# $params{'tool'} - tool name
# $params{'email'} - e-mail address
#
#Output: %results: keys are 'WebEnv' and 'query_key'

my %params = @_;
my $base = "http://eutils.ncbi.nlm.nih.gov/entrez/eutils/";
my ($url, $raw);
my ($begin, $end);

sleep($delay);

$url = $base . "epost.fcgi?db=$params{'db'}&id=$params{'id'}";
$url .= "&tool=$params{'tool'}&email=$params{'email'}";

$begin = time;
$raw = get($url);

$raw =~ /<QueryKey>(\d+)<\/QueryKey>.*<WebEnv>(\S+)<\/WebEnv>/s;
$results{'query_key'} = $1;
$results{'WebEnv'} = $2;

$end = time;
$delay = $maxdelay - ($end - $begin);
if ($delay < 0) { $delay = 0; }

return(%results);

}

#*********************************************************************

sub epost_file {
# Performs EPost, accepts input from file. 
# Input file must have one UID per line.
# Input: %params:
# $params{'db'} - database
# $params{'id'} - filename containing a list of UIDs
# $params{'tool'} - tool name
# $params{'email'} - e-mail address
#
# Output: %results: keys are 'WebEnv' and 'query_key'

my %params = @_;
my $base = "http://eutils.ncbi.nlm.nih.gov/entrez/eutils/";
my $uids;
my @list;
my ($begin, $end, $count);
my %results;

sleep($delay);

#read input file of UIDs, one per line
open (INPUT, "$params{'id'}") || die "Can't open $params{'id'}\n";

while (<INPUT>) {

   chomp;
   push (@list, $_);

}

$params{'id'} = join (',', @list);

%results = epost_set(%params);

$count = $#list;
print "Posted $count records to $params{'db'}.\n";

$end = time;
$delay = $maxdelay - ($end - $begin);
if ($delay < 0) { $delay = 0; }

return(%results);

}

#***********************************************************

sub epost_set {

# Uses EPost to post a set of UIDs using the POST method
# Useful for large sets of UIDs not from a disk file
# Accepts a comma-delimited list of UIDs in $params{'id'}
# Output: $results{'query_key'}, $results{'WebEnv'}

my (%params) = @_;
my $base = "http://eutils.ncbi.nlm.nih.gov/entrez/eutils/";
my ($url_params, $raw, $url);
my %results;

$url_params = "db=$params{'db'}&id=$params{'id'}";
$url_params .= "&tool=$params{'tool'}&email=$params{'email'}";

$url = $base . "epost.fcgi";

#create user agent
my $ua = new LWP::UserAgent;
$ua->agent("epost_file/1.0 " . $ua->agent);

#create HTTP request object
my $req = new HTTP::Request POST => "$url";
$req->content_type('application/x-www-form-urlencoded');
$req->content("$url_params");

$begin = time;
#post the HTTP request
$raw = $ua->request($req);

$raw->content =~ /<QueryKey>(\d+)<\/QueryKey>.*<WebEnv>(\S+)<\/WebEnv>/s;
$results{'query_key'} = $1;
$results{'WebEnv'} = $2;

return (%results);

}

#***********************************************************

sub print_summary {

# Input: %results output from sub esummary

my %results = @_;
my ($id, $count, $i);
my (@a, @b, @c);

$count = 0;

if ($results{'homologene'} eq 'y') {

 @a = sort keys %results;
 @b = sort keys %{$results{$a[0]}};
 foreach (@b) {
   @c = @{$results{$a[0]}{$_}};
   $count = $#c if ($count < $#c);
 }
 
  foreach $id (sort keys %results) {

   unless ($id eq 'homologene') {
    print "\nID $id:\n";
    for ($i=0; $i <= $count; $i++) {
     foreach (sort keys %{$results{$id}}) {
       print "$_: $results{$id}{$_}[$i]\n" if ($results{$id}{$_}[$i]);
     }
     print "\n";
    }
   } 
 }
}

else {

 foreach $id (sort keys %results) {

   print "\nID $id:\n";
   foreach (sort keys %{$results{$id}}) {
      print "$_: $results{$id}{$_}\n";
   }
 } 
}
}

#***********************************************************

sub print_links {

# Input: %results output from sub elink

my %results = @_;
my ($key, $db);

foreach $key (sort keys %{$results{'from'}}) {
   print "Links from: ";
   foreach (@{$results{'from'}{$key}}) {
      print "$_ ";
   }
   foreach $db (keys %{$results{'to'}}) {
    print "\nto $db:";
    foreach (@{$results{'to'}{$db}{$key}}) {
      print "$_ ";
    } 
   }
   print "\n***\n";   
}

}

#**********************************************************

sub print_link_summaries {

# Input: %results output from sub link_history
# Output: Docsums for linked records arranged by input UID 
# set and linked database

my %results = @_;
my (%params,%docsums);
my ($db, $set);

foreach $set ( sort keys %{$results{'to'}} ) {

   print "Links from set $set\n";
   foreach $db (keys %{$results{'to'}{$set}} ) {

   $params{'db'} = $db;
   $params{'WebEnv'} = $results{'WebEnv'};
   $params{'query_key'} = $results{'to'}{$set}{$db}{'query_key'};
   %docsums = esummary(%params);
   print "$db\n\n";
   print_summary(%docsums);
   print "\n";
   }
}    

}

#**********************************************************

1; #End of module
