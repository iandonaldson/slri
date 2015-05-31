#!/usr/bin/perl -w

# $Id: prebind_regex.pl,v 1.2 2004/10/29 22:32:47 kzheng Exp $
#*******************************************************************************
#  Copyright (C) 2000 Mount Sinai Hospital (MSH)
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
#
#  PROGRAM: prebind_regex.pl
#
#  AUTHORS: Kai Zheng (kzheng@mshri.on.ca)
#           Ian Donaldson (ian.donaldson@mshri.on.ca)
#           and Christopher W.V. Hogue (hogue@mshri.on.ca)
#
#  REVISION/CONTRIBUTION NOTES:
#
#   July 1, 2004  Original MSH Revision.
#                Hogue Lab - University of Toronto Biochemistry
#                Department and the Samuel Lunenfeld Research
#                Institute, Mount Sinai Hospital
#                http://bioinfo.mshri.on.ca  hogue@mshri.on.ca
#                ftp://ftp.mshri.on.ca/
#
#
#  PUBLICATION TO CITE:
#
#
#  DESCRIPTION:    Pre Biomolecular Interaction Network Database (PreBIND)
#                  PreBIND regular expression classifier
#
#******************************************************************************

# =============================================================================
#               Documentaion
# =============================================================================
# File Description: 
#       Use regular expressions to detect protein interactions in sentences.
# Command syntax: 
#       /usr/perl5/5.6.1/bin/perl prebind_regex.pl   -i input_file 
#                               [-o outputfile]  
#                               [-l log_file] 
#                               [-m remote/local] 
#                               [-n number] 
#                               [-p precision] 
#                               [-f frequency]
# ==============================================================================

use strict;
use LWP::Simple;
use Pattern;

use Getopt::Long;
# use Term::ReadKey;
# DB2 setting stuff
use lib "/export/home/db2inst1/usr/local/perl5/site_perl";
use DBI;
use POSIX qw(locale_h);
use locale;
# Must have UTF-8 locale installed on machine as well as set here
setlocale(LC_ALL, "en_US.UTF-8");

# --------------------------------------------------------------------
# preprocess name list, special characters []()+?.*^${}|\ must be escaped
# --------------------------------------------------------------------
sub processNamelist{
    my $namelist = shift;
    $namelist =~ s/"\(([^\/]*)\)"/$1/gs; #remove outmost bracket
    $namelist =~ s/"//gs;       #remove double quota (used if name contain \W+ )
    
    # special characters []()+?.*^${}|\ must be escaped
    $namelist =~ s/\[/\\\[/gs;    # replace [ by \[
    $namelist =~ s/\]/\\\]/gs;    # replace ] by \]
    $namelist =~ s/\(/\\\(/gs;    # replace ( by \(
    $namelist =~ s/\)/\\\)/gs;    # replace ) by \)
    $namelist =~ s/\+/\\\+/gs;    # replace + by \+
    $namelist =~ s/\?/\\\?/gs;    # replace ? by \?
    $namelist =~ s/\./\\\./gs;    # replace . by \.
    $namelist =~ s/\*/\\\*/gs;    # replace * by \*
    $namelist =~ s/\{/\\\{/gs;    # replace { by \{
    $namelist =~ s/\}/\\\}/gs;    # replace } by \}
    
    $namelist =~ s/LEUKEMIA\)/(leukemia|LEUKEMIA)/gs;    # special case
    
    $namelist =~ s/\//\|/gs;      # replace / by |
	# $namelist = "\\b(p|P)*(".$namelist.")(p|P)*\\b";
	$namelist = "\\b(".$namelist.")\\b";
    return $namelist;
}



# --------------------------------------------------------------------        
#   define program variables
# --------------------------------------------------------------------
my $starttime = time();

my $pattern_file= "interaction-patterns.txt";   # file containing regex patterns
my $pattern_stat_file= "_patterns_stat.csv"; # ouput file of matching info for each pattern
my $point_file;   # input file, i.e. the POINT data files
my $log_file;     # program log file, containing running informations
my $sent_log_file ="_sent_log.txt"; # another log file for co-occur sentences
my $pair_log_file ="_pair_log.txt"; # another log file of matching info for each pair
my $default_log_file ="_log.txt";
my $result_file;                    # output file, i.e. piscore file
my $default_result_file = "_result.txt";
my $stat_file = "_stat.txt";        # file for statististical purpose

#---------------------------------------------
#  set this default precision to get different precision/recall/f-measure
my $precision; # precision threshold for regex patterns
my $default_precision = 0.55;
#-- my $default_precision = 0.61;
# set this frequency threshold to improve speed  
# not used at present
my $frequency;  # frequency threshold for patterns
my $default_frequency = 0;
#---------------------------------------------

my $seperator= "*"x75;
my @dayofweek=(qw(Sunday Monday Tuesday Wednesday Thursday Friday Saturday));
my @monthnames = (qw(January February March April May June 
                 July August September October November December));
my ($sec,$min,$hour,$mday,$mon,$year,$wday,$yday);

my $numLine =0;    # the total number of non-empty line(or pair/record) in POINT/input table
my $numLineOut =0;    # the total number of output/result line
my $numAbs=0;    # the number of abstract the program reads
my $numMatch=0;    # the number of pattern matching which returns true

my $maxtime;         #the maximum time(seconds) took to process any given abstract
my $mintime;          #the minimum time (seconds) took to process any given abstract
my $pmidMaxtime;    # pmid which takes maxtime to process
my $pmidMintime;           # pmid which takes mintime to process
my $CPUtime;               #the time took to process any given abstract
#my $begin;                  # the start time of each abstract(seconds from 1970 blah blah)
#my $end;                    # the stop time of program
my $prevtime;
my $currtime;

my $currQuery=0;     #current time for a retrival of a abstract from NCBI
my $prevQuery=0;    #previous time for a retrival of a abstract from NCBI
my $idle=0;                # sleep for [0-3]seconds for retrival from NCBI

my $foundA; # 0 if A is not in abstract, 1 if exactly matched, 2 is caseinsensitive matched
my $foundB; # same as above  

# --------------------------------------------------------------------
#   validate command line options
# --------------------------------------------------------------------
my $numArgs = $#ARGV + 1;
if( $numArgs <2 or ($numArgs%2)!=0){ 
    print "command line is not valid \n";
    print "Usage: perl prebind_regex.pl -i infile\n";
    print "                     [-p precision] \n";
	print "                     [-f frequency] \n";
    print "\t-i in:  the input point file name \n";
    print "\t-p precision: the precision threshhold for patterns \n";
    print "\t-f frequency: the frequency threshhold for patterns \n";
}

# --------------------------------------------------------------------
#   parse command line arguments
# ---------------------------------------------------------------------
Getopt::Long::Configure ('bundling');
 if ( @ARGV > 0 ) {
    GetOptions ('i=s' => \$point_file,
                'p=f' => \$precision,
                'f=i' => \$frequency);
}

if($point_file){ # this input POINT filename must be specified
    print "point file specified as $point_file\n";
}else{
    print "point file not specified !! \n";
    exit (0); 
}
if($precision){ # optional
    print "precision specified as $precision \n";
}else{
    print "precision not specified, use default precison $default_precision \n";
    $precision = $default_precision;
}

if($frequency){ # optional, not used at this time
    print "frequency specified as $frequency \n";
}else{
    print "frequency not specified, use default frequency $default_frequency \n";
    $frequency = $default_frequency;
}

$log_file= $point_file.$default_log_file;
$result_file = $point_file.$default_result_file;
$sent_log_file = $point_file.$sent_log_file;
$pair_log_file = $point_file.$pair_log_file;
$stat_file = $point_file.$stat_file;
$pattern_stat_file = $point_file.$pattern_stat_file;

# -------------------------------------------------------------------
#            open file for read and write
# -------------------------------------------------------------------
open PATTERN, $pattern_file || die "Can't open pattern file for read: $!\n";
open POINT, $point_file || die "Can't open point file for read: $!\n";
open (LOG,"> $log_file") || die "Can't open log file for write: $!\n";
open (SENTLOG,"> $sent_log_file") || die "Can't open sentence log file for write: $!\n";
open (PAIRLOG,"> $pair_log_file") || die "Can't open protein pair log file for write: $!\n";
open RESULT, "> $result_file" || die "Can't open result file for write: $!\n";
open STAT, "> $stat_file" || die "Can't open stat file for write: $!\n";
open PATTERNSTAT, "> $pattern_stat_file" || die "Can't open pattern_stat file for write: $!\n";

# --------------------------------------------------------------------
#   log program start time
# --------------------------------------------------------------------
print LOG  "$seperator\n";
print "prebind_regex starts at:", scalar localtime, "\n\n";
print LOG "prebind_regex starts at:", scalar localtime, "\n\n";

# --------------------------------------------------------------------
#   log porgram settings
# --------------------------------------------------------------------
print LOG "#--------------------------\n";
print LOG "input/POINT file is: $point_file\n";
print LOG "output/result file is $result_file\n";
print LOG "log file is $log_file\n";
print LOG "sentence log file is $sent_log_file\n";
print LOG "pair log file is $pair_log_file\n";
print LOG "statistics log file is $stat_file\n";
print LOG "----------\n";
print LOG "regex pattern file is $pattern_file\n";
print LOG "pattern statistics file is $pattern_stat_file\n";
print LOG "precision threshold is $precision\n";
if($frequency){ print LOG "pattern frequency threshold is $frequency\n";}
print LOG "#--------------------------\n";

# -------------------------------------------------------------------
#   connect to db2 local medline database
# -------------------------------------------------------------------

my ($dbh_db2, $stmt_db2, $sth_db2);
print "\nconnect to db2 medline database ...\n";
# Set up a database handle
my $dsn_db2="dbi:DB2:medline";
$dbh_db2 = DBI->connect($dsn_db2, 'db2inst1' ,'kl2es4l', 
            { AutoCommit => 1 }) 
           || die "$DBI::errstr\n";

# LongReadLen controls the maximum length of any long data (CLOBS), this should be set to
# slightly larger than the longest field value to be fetched. 
$dbh_db2->{LongReadLen}=20*1024;

# Allows truncation of CLOBS if they are longer than LongReadLen, if set to 0 an error will
# result instead of a warning.
$dbh_db2->{LongTruncOk}=1;

# Prepare SQL statement for later execution - returns reference to a statement handle object
$sth_db2=$dbh_db2->prepare("CALL medline.get_abstitle_pmid(?)") 
         || die $dbh_db2->errstr;

# -------------------------------------------------------------------
#   connect to MySQL prebind database
# -------------------------------------------------------------------
#print "\nconnect to mysql prebind database ...\n";
#my $dsn_mysql = "DBI:mysql:prebindCB:marge:33306";
#my $dbh_mysql = DBI->connect ($dsn_mysql, "kzheng", "asdf1234", 
#            {RaiseError => 1, AutoCommit => 1})
#            ||die "$DBI::errstr";
#my $sth_mysql=$dbh_mysql->prepare( "SELECT count(*) FROM Word WHERE word=?")
#            ||die $dbh_mysql->errstr;
	

# -------------------------------------------------------------------
#   parse pattern file into a pattern array
# -------------------------------------------------------------------
print "reading patterns ...\n";
my @re = <PATTERN>;
close(PATTERN);
my @patterns;
my $rule;
foreach $rule(@re){
    if($rule =~ /^\s{0,24}(-1|0|1|2)\|([^|]+)\|([^|]+)\|(\d)\|(\d)\|"(.+)"\s*/){
        my ($class, $id, $score, $style, $option, $regex) = ($1, $2, $3, $4, $5, $6);
        # use threshold to get desired precision and recall
        ## next if($score<$precision or $frequency<$frequency );
        # frequency threshold is not used because of training set size is small
        next if($score<$precision);
        my $newpattern = PatternNew($class, $id, $score, $style, $option, $regex);
        push  @patterns, $newpattern;
    }
}

# -------------------------------------------------------------------
# import dictionary 
# -------------------------------------------------------------------
my $dict="word.txt";
open DICT, $dict || die "Can't open dictionary file $dict \n"; 
my $word;
my %englishwords;
foreach $word(<DICT>){
	chomp($word);
	$englishwords{$word} = 1;
}

#####################################################################
#
#   process the point data table line by line, 
#   predict interaction using Regex patterns
#
#####################################################################
my $prev_pmid; # when porcessing a pair, this holds the pmid from last pair
my $pmid;
my $abstract;
my @list;
my $sent;
my @data;
my $cooccur;

print "process the point table\n ";
print "check result file for progress ...\n";   
while(<POINT>){
    my $line = $_;
    $cooccur = 0;
    
	# validate pair line
    next if($line !~ /^(\d+)\t(\d+)\t(\S+)\t(\d+)\t(\S+)\t(\d)/); 
    ++$numLine;
    chomp($line);    
    $pmid = $1; 
    my($gia, $nama, $gib, $namb, $piscore0) = ($2, $3, $4, $5, $6);  
    
	# piscore0 is from original algorithm described in PreBIND paper
    my $piscore=0; # this is the Predicted Interaction SCORE by new algorithm
    
	
	#======================================================
	#  
	#======================================================
	# skip if names are the same
	next if $nama eq $namb;
	
	# skip if one name is just numbers (for now)
	# before name recognition algorithm is done 
	# because it could be part other name 
	next if $nama =~/^\d+$/ or $namb=~/^\d+$/;
	
	my $A = processNamelist($nama);
    my $B = processNamelist($namb);
	my $Aword = (exists ($englishwords{lc($A)}));
	my $Bword = (exists ($englishwords{lc($A)}));
	my $Acapitalized = ($A=~/[A-Z]+/);
	my $Bcapitalized = ($B=~/[A-Z]+/);
	next if (($Aword && !$Acapitalized) or ($Bword && !$Bcapitalized));
	
	my $case=0;
	if ($Aword or $Bword )
	{
		$case=1;
	}

	$foundA=0;
    $foundB=0;

    ##----------------------------------------------
    ##  get abstract (by style)
    ##----------------------------------------------
    my $i; #sentence number
	##if (pmid eq prev_pmid){$abstract  not changed}
    if(!$prev_pmid or $prev_pmid != $pmid)
	{
        if(!$currtime){ # the first Pmid, init current time
            $currtime = (times)[0];
        }
        else{
            if(!$prevtime){ # the 2nd pmid, init max and min time, pmid
                $prevtime = $currtime;
                $currtime = (times)[0];
                $CPUtime = $currtime - $prevtime;
                $maxtime = $CPUtime;
                $pmidMaxtime = $prev_pmid;
                $mintime = $CPUtime;
                $pmidMintime = $prev_pmid;
            }else{ # the rest pmids, update pmid and min, max time
                $prevtime = $currtime;
                $currtime = (times)[0];
                $CPUtime = $currtime - $prevtime;
                if( $CPUtime > $maxtime){ $maxtime = $CPUtime; $pmidMaxtime = $prev_pmid;}
                if( $CPUtime < $mintime  ){$mintime = $CPUtime; $pmidMintime = $prev_pmid;}
            }
            #printf LOG "-- this abstract took %.2f CPU seconds to process ---\n\n", $CPUtime;
            printf LOG "Last abstract [pmid=$prev_pmid]: took %.2f CPU seconds to process ---\n", $CPUtime;
            print LOG "Current time is :", scalar localtime, "\n";
            print LOG " =================================================================\n\n";
        }
        
		$abstract = "";
		$numAbs++;
		print LOG "********************\n";    
		print LOG "$numAbs : Local feteched file\n";
		print LOG "PMID: $pmid\n";
		
		$sth_db2->execute($pmid) || die $sth_db2->errstr;
		   
		# Fetch the resulting records and print them out to the output file
				  
		while (@data = $sth_db2->fetchrow_array()) {
			my $title_match=$data[0];
			my $abs_match=$data[1];

			print LOG "TITLE: ";
			if($title_match){
				$abstract = $abstract.$title_match."\n";
				print LOG "$title_match\n";
			}
			else{
				print LOG "== NO title record ==\n";
			}
			# The abstract is a clob and comes back with tags on it - this removes them
			print LOG "ABSTRACT: ";
			if($abs_match){
				$abs_match=~s/(.*)<AbstractText>(.*?)<\/AbstractText>(.*)/$2/; 
				$abstract = $abstract.$abs_match;
				print LOG "$abs_match\n\n";                   
			}
			else{
				print LOG "== NO abstract record ==\n";
			}
			
			
		}
		# Close the statement handle
		$sth_db2->finish;
					
		# Prepare abstract - substitute all separators to comma
		$abstract =~ s/(\s|\n)+/ /gs;
		
		@list = split(/[.?!;]\s(?=[^a-z])/, $abstract); 
		# this is too simple! Ex: to Arh. It , S. Cere, 
		
		##  print all sentences in the abstract
		print LOG "#-----------------\n";
		$i=0;
		foreach $sent(@list){
			 print LOG "<$i>$sent</$i>\n";
			 $i++;
		 }
		print LOG "#-----------------\n";
    }# end of get abstract
    
    ##----------------------------------------------
    ##  preprocess(parse, normalize, tag) abstract
	##  name disambiguation
    ##----------------------------------------------
    print LOG "PROCESSING LINE [$numLine]: $line\n";
	
	next if $abstract =~(/($A|$B)(\S*\s+){0,2}\S*(kinase|gene|rna|cell)/i);
	next if $abstract =~(/(cell|kinase|gene|rna)(\S*\s+){0,2}\S*($A|$B)/i);
		
    if($abstract=~/$A/){$foundA=1;}
    elsif($abstract=~/$A/i){$foundA=2;}
    if($abstract=~/$B/){$foundB=1;}
    elsif($abstract=~/$B/i){$foundB=2;}
	
	print LOG "PROCESSED NAME LIST       : $A :$foundA  $B :$foundB\n";
    ##----------------------------------------------
    ##  do match and predcition for each pair (record)
    ##  .348 (positive) = 0.323( class 1) + 0.115(class 2);    
    ##----------------------------------------------

    my ($absScoren, $absScore0, $absScore1, $absScore2) = (0, 0, 0.438, 0.438); 
    # pair score in an abstract
    my ($absScore, $absPredict) = ($default_precision, 0);   
    
    
    my $pairinfo="MATCHING INFO:\n";
    # print LOG "SENTENCES MATCHED:\n\t<SENTENCE>: REGEX \n";
    my $matchSents=""; #-----
    
	##----------------------------------------------
	##  for each sentence in the abstract, do match and predict
	##----------------------------------------------
	$i=-1; 
	foreach $sent(@list){
		$i++;
		#print LOG "<$i>$sent</$i>\n";
		next if($sent !~ /(\S*\s){5,}/); # skip if the sentence is less than 5 words
#print"$sent\n";
		my $matched = 0;
		my $sentinfo = "";
		  
		my ($sentScoren, $sentScore0, $sentScore1, $sentScore2 )= (0, 0, 0.438, 0.438);
		# pair score in a sentence
		my ($sentScore, $sentPredict) = ($default_precision , 0);
		
		my ($class, $id, $score )  = (0, 0, 0.0);
		my ($pattern, $regex,  $regex1,  $regex2) ;
		
		##  given sentence and nameA and nameB, get piscore using regex
		# next if($sent !~ /$A/i || $sent !~ /$B/i); # -- not good because
		# there are cases where some part of nama and namb may contain the same symbol
		next if($sent!~/$A.*$B/i && $sent!~/$B.*$A/i); # skip if no co-occurance 
		$cooccur=1;

		
		$sentinfo.="\t<$i> $sent\n";
		foreach $pattern(@patterns){             
			$class=$pattern->{class};
			$id  = $pattern->{id};
			$score=$pattern->{score};   
			$regex=$pattern->{regex};

			#substitude A,B by $A, $B
			$regex1 = $regex;
			$regex1=~s/(.*)A(.*)B(.*)/$1$A$2$B$3/;

			$regex2 = $regex;
			$regex2=~s/(.*)A(.*)B(.*)/$1$B$2$A$3/;

			my $matched=($sent =~ /$regex1/i || $sent =~ /$regex2/i);
			if ($case==1){
				$matched =($sent =~ /$regex1/ || $sent =~ /$regex2/);
			}
			
			if($matched==1){ 
			#if($sent=~/$regex1/ || $sent=~/$regex2/){ 
				++$numMatch;
				 print LOG "\t<$i>: $class|$score|$regex\n";
				$sentinfo.="\t\t$class|$id|$score|$regex\n";
				PatternAddMatch($pattern);
				
				# update score for matched pattern class, order is not important here
				if($class==-1 && $score>$sentScoren) { $sentScoren=$score; }
				if($class== 0  && $score>$sentScore0) { $sentScore0=$score; }
				if($class== 2  && $score>$sentScore2) { $sentScore2=$score;  }
				if($class== 1 &&$score>$sentScore1) { $sentScore1=$score; }
			}                       
		} # end  foreach $pattern
		
		# predict sentence class, order IS important here
		if($sentScore<=$sentScore1){$sentScore=$sentScore1; $sentPredict = 1;}
		if($sentScore<=$sentScore2){$sentScore=$sentScore2; $sentPredict = 2;}
		if($sentScore<=$sentScore0){$sentScore=$sentScore0; $sentPredict = 0;}
		if($sentScore<=$sentScoren){$sentScore=$sentScoren; $sentPredict = -1;}
		
		# calculate cumulative (by # of positive sentences) piscore
		if($sentPredict==1 or $sentPredict ==2){
			$piscore ++;
		}
		
		
		# print sentence matching information , and update highest positive/negative score
		if($matched ==1){ 
			print SENTLOG "$pmid<$i>|$sentScore|$sentPredict|$gia:$nama|$gib:$namb|$sent\n"; 
			$matchSents = $matchSents."\t<$i>".$sent."</$i>\n";
			$pairinfo.=$sentinfo;
			
			# update highest positive socre for this oc-occur sentence
			if($sentPredict == -1  && $sentScore>$absScoren){$absScoren=$sentScore; }
			if($sentPredict == 2  && $sentScore>$absScore2){$absScore2=$sentScore; }
			# if($sentPredict == 0  && $sentScore>$absScore0){$absScore0=$sentScore; }
			if($sentPredict ==1 && $sentScore>$absScore1){$absScore1=$sentScore;}      
		}
		else{#not matching any pattern, but at lease A, B co-occur in this sentence
			print SENTLOG ">>$pmid<$i>|$sentScore|$sentPredict|$gia:$nama|$gib:$namb|$sent\n";               
			$matchSents = $matchSents."\t>><$i>".$sent."</$i>\n";
			$pairinfo=$pairinfo.$sentinfo;
		}
		
	}  # foreach $sent
	
	# calculate abstract predict score by highest positive/negative score
	if($absScore<=$absScore1){$absScore=$absScore1; $absPredict = 1;}
	if($absScore<=$absScore2){$absScore=$absScore2; $absPredict = 2;}
	#       if($absScore<=$absScore0){$absScore=$absScore0; $absPredict = 0;}
	if($absScore<=$absScoren){$absScore=$absScoren; $absPredict = -1;}

    print LOG "PAIR DETAIL:  $pmid|$absScore|$absPredict|$piscore|$gia:$nama|$gib:$namb|$matchSents\n"; 
    print LOG "$pairinfo";
#    print LOG "PAIR DETAIL:  $pmid|$absScore|$absPredict|$piscore|$gia:$nama|$gib:$namb\n";
#    print LOG "OUTPUT STAT: $pmid\t$gia\t$nama\t$gib\t$namb\t$foundA|$foundB|$cooccur|$piscore0|$piscore\n";
#    print LOG "OUTPUT RESULT: $pmid\t$piscore\t$gia\t$gib\n";
    print LOG "--------\n";
#    print PAIRLOG "$pmid|$absScore|$absPredict|$gia:$nama|$gib:$namb|\n$pairinfo--------\n";
#    print STAT  "$pmid\t$gia\t$nama\t$gib\t$namb\t$foundA|$foundB|$cooccur|$piscore0|$piscore\n";;
#    print RESULT  "$pmid\t$piscore\t$gia\t$gib\n";
    print RESULT  "$pmid\t$gia\t$nama\t$gib\t$namb\t$piscore\t$absPredict\t$absScore\n";
    $numLineOut++;
    $prev_pmid = $pmid;
} #end pointfile

# Disconnect from the database 
$dbh_db2->disconnect;

#$dbh_mysql->disconnect;

# -------------------------------------------------------------------
#   print statistcal result for whole process and for each pattern
# -------------------------------------------------------------------
print LOG "---- output overall result ----\n";

print LOG "$numLine input lines were processed\n";
print LOG "total predicted pairs  = $numLine\n";
print LOG "$numLineOut lines were outputted\n";

print LOG "total parsed abstracts = $numAbs\n";
print LOG "total patterns matched = $numMatch\n";

if($maxtime && $pmidMaxtime && $mintime && $pmidMintime){
    print LOG "the maximum time took to process any given abstract= $maxtime seconds";
    print LOG " (and the abstract pmid= $pmidMaxtime)\n";
    print LOG "the minimum time took to process any given abstract = $mintime seconds";
    print LOG " (and the abstract pmid= $pmidMintime)\n";
}
else{
    print LOG " less than two abstracts retrieved \n";
}

# ------------------------------------------------------------------
# print "---- sentence/pair/abstract statistics ----\n";
# no training set here
# print sum[4][4];                    
# print stat[2][2]; 
# print"precision= [$precision]; recall = [$recall]; fmeasure = [$fmeasure];"

# print "---- pattern statistics ----\n";
# print statistics to the pattern stat file
# ------------------------------------------------------------------
my $pattern;
print PATTERNSTAT "class, id, score, frenquency, style, option, regex, ";
print PATTERNSTAT "precision, recall, match, goodMatch\n";
for $pattern(@patterns){
    # $precision=$pattern->correct-match / $numLine;
    # $recall =$pattern->correct-match / $match;
    print PATTERNSTAT "$pattern->{class},$pattern->{id},$pattern->{score},$pattern->{frequency},";
    print PATTERNSTAT "$pattern->{style},$pattern->{option},\"$pattern->{regex}\",";
    printf PATTERNSTAT "%1.4f,", $pattern->{precision};
    printf PATTERNSTAT "%1.4f,",$pattern->{recall};
    print PATTERNSTAT "$pattern->{match}, $pattern->{goodMatch}\n";
}

# ------------------------------------------------------------------
#   close files
# ------------------------------------------------------------------
close(POINT);
close(RESULT);
close(PATTERNSTAT);
close(STAT);
close(SENTLOG);
close(PAIRLOG);

# ------------------------------------------------------------------
#   log program stop time
# ------------------------------------------------------------------
print LOG  "$seperator\n";
print "prebind_regex stops at:", scalar localtime, "\n\n";
print LOG "prebind_regex stops at:", scalar localtime, "\n\n";
print LOG  "$seperator\n";

#------------------------------------------------------------------
#   log system and user time
# ------------------------------------------------------------------
my $stoptime = time();
# ($user, $system, $cuser, $csystem) = times;
# printf LOG "start = $starttime; stop = $stoptime \n";
printf LOG "real time \t %2.3f seconds\n", ($stoptime -$starttime);
printf LOG "User time \t %2.3f seconds\n",(times)[0];
printf LOG "System time \t %2.3f seconds\n",(times)[1];
#printf LOG "children user time in this program is %2.3f seconds\n",(times)[2];
#printf LOG "children system time in this program is %2.3f seconds\n",(times)[3];

close(LOG);

# $Log: prebind_regex.pl,v $
# Revision 1.2  2004/10/29 22:32:47  kzheng
# initial import
#
# Revision 1.1  2004/09/02 20:35:57  kzheng
# move into searcher directory
#
# Revision 1.3  2004/07/16 20:39:24  kzheng
# add disambiguation for amino acid
#
# Revision 1.2  2004/07/08 22:11:51  kzheng
# check name boundary, add name disabiguation, add english word dictionary
# VS: ----------------------------------------------------------------------
#
# Revision 1.1  2004/01/15 21:14:15  kzheng
# prebind_regex initial import
#
