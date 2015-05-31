#!/usr/bin/perl -w

# =============================================================================
#               Documentaion
# =============================================================================
# File Description: 
#       Use in-house medline to search names for pmid.
# Command syntax: 
#       perl prebind_mymed.pl  infile outfile logfile [-t taxid]
#for example: 
#       perl prebind_mymed.pl  SRCH.dbf RESULT.txt prebind_mymed.log -t 9606
# ==============================================================================

use strict;
use Getopt::Long;
use Term::ReadKey;

#DB2 setting stuff
use lib "/export/home/db2inst1/usr/local/perl5/site_perl";
use DBI;
use POSIX qw(locale_h);
use locale;
# Must have UTF-8 locale installed on machine as well as set here
setlocale(LC_ALL, "en_US.UTF-8");

#Perl module for reading and writing the dbf files
use lib '/home/kzheng/mylib/myperlmodules/DBD-XBase-0.241/lib';
use XBase;

# --------------------------------------------------------------------        
#   define program variables
# --------------------------------------------------------------------
my $starttime = time();
my $seperator= "-"x60;
my $infile=$ARGV[0];
my $outfile=$ARGV[1];
my $logfile = $ARGV[2];
my $stopwordsfile = "pubmed-stopwords";
my $taxid0=9606; #default mesh = human
Getopt::Long::Configure ('bundling');
if ( @ARGV > 0 ) {
	GetOptions ('t=i' => \$taxid0);
}



# --------------------------------------------------------------------
#   initilize program
# --------------------------------------------------------------------
#log program start time
open LOG,"> $logfile" || die "Can't open log file for write: $!\n";
open OUT,"> $outfile" || die "Can't open output file for write: $!\n";
print LOG "="x10,"[ ", scalar localtime, " ]", "="x10, "\nprebind_mymed starts.\n";
print LOG "input file is: $infile\n";
print LOG "output file is $outfile\n";
print LOG "log file is $logfile\n";
print LOG  "$seperator\n";

# create stop words string
open STOPWORDS, "$stopwordsfile" || die "can't open pubmed stop words file: $!\n";
my $stopwords = join (' ', <STOPWORDS>);
$stopwords = " ".$stopwords. " ";
$stopwords =~ s/\s+/ /g;
print LOG "NCBI PubMED stop words:\n$seperator\n";
print LOG "$stopwords\n$seperator\n";
# -------------------------------------------------------------------
#   initialize MyMed db2 connection
# -------------------------------------------------------------------
# Get username and password
my $dbname   = 'dbi:DB2:medline';
my ($user, $password);
my ($dbh, $stmt, $sth);

print "Enter db2 user  name>";
$user = <STDIN>;
chomp($user);
print "Enter password>";
ReadMode('noecho'); # don't echo
$password = <STDIN>;
chomp($password);
ReadMode(0);        # back to normal
print "\nconnect to db2 ...\n";

# Set up a database handle
$dbh = DBI->connect($dbname,$user,$password, { AutoCommit => 1 }) 
   or die "Error connecting $DBI::errstr\n";

# LongReadLen controls the maximum length of any long data (CLOBS), this should be set to
# slightly larger than the longest field value to be fetched. 
$dbh->{LongReadLen}=20*1024;

# Allows truncation of CLOBS if they are longer than LongReadLen, if set to 0 an error will
# result instead of a warning.
$dbh->{LongTruncOk}=1;

# ------------------------------------------------------------------------------
#   Go through SRCH table,create RESULT using mymed API
# ------------------------------------------------------------------------------
print LOG "Start processing the SRCH table. \n";


# open SeaRCH table for read
my $table = new XBase "$infile" or die XBase->errstr;                 
my %taxnames=(	4932=>"Saccharomyces cerevisiae", 
				9606=>"human",#9606=>"human\" | \"hominidae",
            	10090=>"mice");
my ($deleted, $gip, $name, $taxid); #SRCH table fields needed for search

for (0 .. $table->last_record) {
	($deleted, $gip, $name, $taxid) = $table->get_record($_, "GI_PROT", "NAME", "TAXID");
	next if $deleted;
    next if $taxid != $taxid0;
    
    #if $name is in stop word string, use single quote to brace it as in medline
    if ($stopwords=~/\s$name\s/i){
    	#$name = "\\'" .$name ."\\'";
        next; #can not deal with single quote in db2 now
    }
    if(($name eq "N/A") ||($name eq "NO_PRIMARY_LOCUS_NAME")){
        next;
    }
    if($name =~ /"(.*)"/){
        $name =$1;
    }
    #deal with special symbols 
    my $mesh = $taxnames{$taxid}; 
    print LOG "$seperator \n";    
    print LOG "$gip\t$name\t$taxid\t$mesh\n";
    #print "$gip\t$name\t$taxid\t$mesh\n";
  
    # Create SQL statement to call stored procedure
	my $stmt="	SELECT pmid   
            FROM medline.citations
            WHERE contains(citation,\'section(\"mesh\") \"$mesh\" & sections(\"abstracts\", \"titles\")\"$name\"\')=1;";
            #WHERE contains(citation,\'\"$name\"& section(\"mesh\") \"$mesh\"\')=1;"; 
             

    #prepare the statement
    my $sth=$dbh->prepare($stmt) ;  #not die, skip
	if (!$sth) {
        ## print DB2 error message and exit ##
        print "Couldn't prepare statement.\n";
        #print $db->errstr,"\n";
        next;
    }
    # Execute the SQL statement 
    $sth->execute();#not die, skip
    if (!$sth) {
        ## print DB2 error message and exit ##    
        print "Couldn't execute statement: " . $sth->errstr;
        #print $db->errstr,"\n";
        next;
    }
    #print "---\n";
    # Fetch the resulting records and print them out to the output file
    my @data;
    while (@data = $sth->fetchrow_array()) {
        my $pmid_match=$data[0];
		print LOG "$pmid_match\n";
        print OUT "$gip\t$name\t$pmid_match\n"
    }
    # Close the statement handle
    print LOG "\n";
    $sth->finish;
}

# ------------------------------------------------------------------
#   clean up and calculate running time
# ------------------------------------------------------------------
$dbh->disconnect;
print "prebind_mymed stops at:", scalar localtime, "\n\n";
print LOG "="x10,"[ ", scalar localtime, " ]", "="x10, "\nprebind_mymed stops.\n";
my $stoptime = time();
printf LOG "real time \t %2.3f seconds\n", ($stoptime -$starttime);
printf LOG "User time \t %2.3f seconds\n",(times)[0];
printf LOG "System time \t %2.3f seconds\n",(times)[1];

close(LOG);
close(OUT);
$table->close;

#http://www.ncbi.nlm.nih.gov/entrez/query.fcgi?cmd=Retrieve&db=PubMed&dopt=Citation&list_uids=
# $Log: prebind_mymed.pl,v $
# Revision 1.4  2004/03/04 22:46:33  kzheng
# add exception(special symbols) handling
#


