#!/usr/bin/perl -w
use Time::localtime;

my $tm = localtime;
my $today = sprintf ("%02d/%02d/%04d", ($tm->mon)+1, $tm->mday, ($tm->year)+1900);

my $h1 = "<center><h1>SeqHound Update Status for $today </h1></center>";

my $reportHtml = "report.html";
my $template1 = "t1.txt";
my $template2 = "t2.txt";
my $updatelog = "updateresult.log";
my $srcdir = "/bigdisk1/zwang/updatestatus/";
my $destdir = "/www/seqhound_odbc/htdocs/";


open (REPORT, ">$reportHtml");
open (TEMP1, "<$template1");
while (<TEMP1>)
{
  print REPORT $_;
}
close (TEMP1);

print REPORT "$h1 \n";

print REPORT "<center><table border=\"1\" cellspacing =\"0\" cellpadding=\"3\"> ";
open (UPDATE, "<$updatelog");
print REPORT "<TR>";
print REPORT "<TH>Module</TH>";
print REPORT "<TH>Description</TH>";
print REPORT "<TH>Example Table</TH>";
print REPORT "<TH>Data Source</TH>";
print REPORT "<TH>Last Updated</TH>";
print REPORT "<TH>Update Frequency</TH>";
print REPORT "</TR>";
while (<UPDATE>)
{
    print REPORT "<tr>";
  my @list = split (/\t/, $_);
  foreach $item (@list)
  {
    print REPORT "<td width=\"20%\">";
    print REPORT "$item";
    print REPORT "</td>";
  }
    print REPORT "<tr>";
}
  print REPORT "</table></center>";
  print REPORT "<BR>*All GOID pairs in the table are compiled from two sources: Gene ontology annotation files from the ";
  print REPORT "<A HREF=\"ftp://ftp.geneontology.org/pub/go/gene-associations/\">";
  print REPORT "gene ontology web site</A>";
  print REPORT " and database cross references collected from multiple 
sources";
  print REPORT " including Swiss-Prot, Tremble, Flybase, Wormbase, MGI,
SGD and others.<BR>"; 
close (UPDATE);
open (TEMP2, "<$template2");
while (<TEMP2>)
{
  print REPORT $_;
}
close (TEMP2);


close (REPORT);
# Copy the new html report to the htdocs directory
my $destfile = $destdir . $reportHtml;
my $oldfile = $destdir."report.html_save";
if (-s $destfile)
{
  system ("mv $destfile $oldfile"); 
}
my $srcfile = $srcdir . $reportHtml;

system ("cp $srcfile $destfile");
# Create a directory to store the daily report html files
my $statusdir = $srcdir."reportlog/";
my $backupfile = "report".$today.".html";
$backupfile =~ s/\///g;
my $archfile = $statusdir.$backupfile;

system ("mkdir -p $statusdir");
system ("cp $reportHtml $archfile");

