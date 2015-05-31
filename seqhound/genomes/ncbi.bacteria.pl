#!/usr/bin/perl -w
# $Id: ncbi.bacteria.pl,v 1.5 2004/02/05 19:59:31 hlieu Exp $
#***************************************************************************
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
#  PROGRAM: Perl script
#           ncbi.bacteria.pl
#
#
#  AUTHORS:
#           Hao Lieu
#           Katerina Michalickova
#           Christopher W.V. Hogue hogue@mshri.on.ca
#
#
#  REVISION/CONTRIBUTION NOTES:
#
#  June 1, 2000  Original MSH Revision.
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
#  DESCRIPTION
#  -connect to ncbi.nlm.nih.gov/genbank/genomes/Bacteria and
#     compare directory listing to genff contents
#  -results saved to bacteria.results
#  -downloads the necessary files and then runs the preparser
#   and parser on downloaded files and adds new records to genff
#
#  search for TODO: for any changes you need to make
#
#  to execute:
#     ./ncbi.bacteria.pl [location of genff]
#        if [location of genff] not supplied, defaults to ./genff
#***************************************************************************

use Net::FTP;
use strict;

### ftp information
### TODO: change the password
my @ftp_info = 
(
   "ftp.ncbi.nih.gov",                  # ftp site
   "anonymous",                         # login name
   "seqhound.admin\@blueprint.org",     # login password
   "/genomes/Bacteria"                  # remote working directory
);

#### file to hold ncbi's listing of bacteria
my @default = 
(   
   "./genff",                           # location of genff
   "bacteria.results",                  # comparison of ncbi and genff
   "bacteria.log",                      # log file
   "comff",                             # temp file passed to parser 
   "asn"                                # download directory, gets deleted at cleanup
);


#### hash of ncbi's bacteria on their ftp site
my %bact_hash;

#### hash of genff bacteria
my %local_hash;

#### for %match & %semi the hashes are organized as:
#### %semi = ( "ncbi bacteria" => "best genff match" );
#### %match = ( "bacteria" => "bacteria" );
my (%match, %semi, @unmatch);

#### current highest kloodge in genff, new records increment on this
my $max_klge = 0;



############################################################################
#
#                                 PERL FORMAT
#
# format is a handy little perl built-in utility that allows pretty
# picture printing. Simply specify the setup of the pretty print, then
# use 'write FILEHANDLE' to print using the pretty setup.
# Simple as a charm, pretty as a rainbow.
#
###########################################################################

# in this format to RESULT, I have set up 2 columns, one for the genff
# bacteria, and the other for the ncbi-obtained bacteria.
# in the first column I print the value of $nc (as in ncbi), and in the second
# column, I print the value of $loc (as in local).
# then to print the values, I just call 'write RESULT'.
# The first time I call 'write RESULT', perl automatically
# calls 'write RESULT_TOP' to display my table headings

my ($loc, $nc);
format RESULT_TOP = 


*******************************SEMI MATCHED BACTERIA****************************
NCBI BACTERIA                                    GENFF
-------------                                    -----
.

format RESULT = 
@<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<                @<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
$nc,                                            $loc
.

################## END OF FORMAT SPECS


#### ftp to NCBI and get a list of their complete genome for bacteria
#### store info into %bact_hash for later use
sub ftp
{
   my $ftp = Net::FTP->new($ftp_info[0]) || die "Could not create ftp object";

   $ftp->login($ftp_info[1], $ftp_info[2]) || die "Could not login";
   $ftp->cwd($ftp_info[3]) || die "Could not cwd to $ftp_info[3]";

   print LOG "Logged in successful\n";

   # open(LIST, ">$default[1]") || die "Could not open $default[1]";

   my $bact;
   foreach my $file ($ftp->ls("-F"))
   {
      if( $file =~ /(.*)\// ) {
         # print LIST "$1\n";
         ($bact = $1) =~ tr/_/ /;
         $bact_hash{$bact} = 1; 
      }
   }

   # close LIST; 
   print LOG "Got NCBI's bacteria complete genome\n";


   $ftp->quit;
}


#### store genff info into %local_hash 
sub comgen 
{

   my $gen_source; 
   if( $ARGV[0] ) { $gen_source = $ARGV[0];    }
   else           { $gen_source = $default[0]; }  
                           
   open(GENOME, "<$gen_source") || die "Could not open $gen_source";

   my ($entry, $bacteria);
   while ( defined ($entry = <GENOME>) ) 
   {
                           
      # extract the bacteria name  and kloodge
      chomp $entry;
      $entry =~ /^(\d+)[\s|\t]+(\d+)[\s|\t]+(\d+)[\s|\t]+([\w|\d|_]*)[\s|\t]+(\w.*)$/;
      if ( $2 > $max_klge ){
         $max_klge = $2;
      }
      $bacteria = $5;
      
      # get rid of extra details won't need
      $bacteria =~ s/(chromosome|plasmid|megaplasmid|extrachromosomal|small|large|mitochondrion).*//;
      if ( $bacteria =~ / $/ ) { chop $bacteria; }

      $local_hash{$bacteria} = 1;
   }
   print LOG "Got $gen_source\n";
   close GENOME;
}


#### compares %bact_hast to %local_hash
#### and store results into @unmatched, @semimatch, @match
sub compare
{
   foreach my $ncbi ( keys %bact_hash )
   {
      $ncbi =~ tr/_/ /;
      if( exists ( $local_hash{$ncbi} ) )
      {

         # $ncbi in genff already
         $match{$ncbi} = $ncbi;
      }else {

         # $ncbi is not in genff
         my @nword = split(/ /, $ncbi);
         my ($segs, $best) = (0, 0); #matched tokens, best match
          
         foreach my $lbact ( keys %local_hash )
         {
            # if 1st ncbi token does not match local bacteria
            if( ! ($lbact =~ /$nword[0]/) ) {
                next;
            }else {
                $segs++; # the Genus matches
                # compare the rest of the name
                for(my $n = 1; $n <= $#nword; $n++)
                {
                   if( $lbact =~ /$nword[$n]/i ){ $segs++; }
                } # end for
                if ( $segs > $best ) {
                   $best = $segs;
                   $semi{$ncbi} = $lbact;
                }
            } # end inner else
         } # end foreach

         if( !exists($match{$ncbi}) && !exists($semi{$ncbi}) ){
             $unmatch[$#unmatch + 1] = $ncbi;
         }
      } # end outer else
   }
}



#### generate a report 
sub report
{
   open(RESULT, ">$default[1]") || die "Could not open $default[1]";

   print RESULT "***********PERFECT MATCH***********\n";
   foreach my $bact ( sort keys (%match) ){
      print RESULT "$bact\n";
   }

   foreach my $bact ( sort keys %semi) {
      $nc = $bact; $loc = $semi{$bact};
      write RESULT;
   }

   print RESULT "\n\n***********UNMATCHED NCBI BACTERIA*************\n";
   foreach my $bact (sort @unmatch) {
      print RESULT "$bact\n";
   }

   print LOG "Results saved to $default[1]\n";
   close RESULT;
}

#### downloads the .asn files for the semi matched and unmatched
sub download
{
   my $ftp = Net::FTP->new( $ftp_info[0] ) || die "Could not create ftp object";

   open(COMFF, ">$default[3]") || die "Could not open $default[3]";
   
   $ftp->login( $ftp_info[1], $ftp_info[2] ) || die "Could not login";
   $ftp->binary;
   my @toget = (keys(%semi), @unmatch);

   foreach my $bact ( sort @toget )
   {
      print COMFF "$bact\n";
      $bact =~ tr/ /_/;
      $ftp->cwd( "$ftp_info[3]/$bact" ) || die "Could not cwd to $ftp_info[3]/$bact";

      foreach my $file ( $ftp->ls("-F") ) {
         if ( $file =~ /\.asn/ )
         {
            $ftp->get( $file, "$default[4]/$file" ) || die "Could not get $file";
            print LOG "New Entry to genff $file\n";
            print COMFF "\t$file\n";
         }
      }
   }
   close(COMFF);
}


#### main starts here
 
   ### make sure pregen.pl is in same directory
   if ( !(-f "pregen.pl") ) { die "pregen.pl must also be in this directory."; }
   if ( !(-f "gen_cxx") ) { die "gen_cxx must also be in this directory."; }


   ### create the download directory 
   if( !(-d $default[4]) ) {
      system("mkdir $default[4]");
   }

   open(LOG, ">$default[2]") || die "No log file";

   ### get ncbi's complete genome
   ftp();

   ### get genff complete genome
   comgen();


   ### compare ncbi to genff
   compare();

   ### generate a report
   report();


   ### download new asn from ncbi
   download();

   ### run preparser on downloads
   system("./pregen.pl $default[3] $default[4]");
   print LOG "./pregen.pl $default[3] $default[4]\n";

   ### run preparser-generated files and add records into genff
   system ("./gen_cxx -kloodge $max_klge -infile $default[3] -outfile $default[0] -dir $default[4]");
   print LOG "./gen_cxx -kloodge $max_klge -infile $default[3] -outfile $default[0] -dir $default[4]\n";

   ### delete temp files and downloaded files
   unlink ( $default[3] );
   system("rm -fr $default[4]");
   print LOG "deleteing $default[4]\n";
   print STDOUT "See $default[2] for log\n";
   print LOG "See $default[1] for changes to $default[0]\n";
   close(LOG);

# $Log: ncbi.bacteria.pl,v $
# Revision 1.5  2004/02/05 19:59:31  hlieu
# added checks for robustness
#
# Revision 1.4  2004/02/03 23:00:04  hlieu
# minor modifictaions. incorporates pregen.pl, cleanup asn directory, use comff
#
# Revision 1.3  2004/02/03 14:59:01  hlieu
# preparser for gen.pl used to slim down NCBI .asn files to only the Seq_descr
# objects
#
#
