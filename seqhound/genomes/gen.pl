#!/usr/bin/perl -w
# $Id: gen.pl,v 1.2 2004/02/05 19:59:31 hlieu Exp $
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
#           gen.pl
#
#
#  AUTHORS:
#           Hao Lieu (hlieu@blueprint.org)
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
#     Rebuilds genff from scratch. You may want to update the
#     hash %toget below. The general format of %toget is:
#        "organism_name" => ["subdir1", "subdir2"]
#  
#     where subdirn are subdirectories of organims_name in the NCBI
#     ftp site ie genomes/organism_name/subdirn 
#
#     gen.pl is used in conjunction with ncbi.bacteria.pl to 
#     completely rebuild genff. After you run gen.pl, run ncbi.bacteria.pl
#    
#     search for TODO: to see any changes you need to make
#***************************************************************************

############################################################################
#                               USAGE
# to execute script use:
#
# ./gen.pl
#
############################################################################

use Net::FTP;

### ftp information for where to remotely connect, ...
### TODO: change the login password
@ftp_info = 
(
   "ftp.ncbi.nih.gov",                  # ftp site
   "anonymous",                         # login name
   "seqhound.admin\@blueprint.org",     # login password
   "/genomes"                           # remote working directory
);

# default information, you can change this if you want 
@default = 
(   
   "./genff",                           # location of genff
   "comff",                             # new comgens to add to genff
   "asn",                               # directory to download asn files to
   "gen.log",                           # log file
);

### a hash of the eukaryotes and directories where their complete genome are 
### TODO: you may want to check for new complete genomes
%toget = 
(
   "Arabidopsis_thaliana" => ["CHR_I", "CHR_II", "CHR_III", "CHR_IV", "CHR_V"],
   "Caenorhabditis_elegans" => ["CHR_I", "CHR_II", "CHR_III", "CHR_IV", "CHR_V", "CHR_X"],
   "Drosophila_melanogaster" => ["CHR_2", "CHR_3", "CHR_4", "CHR_X"],
   "Encephalitozoon_cuniculi" => ["."],
   "H_sapiens" => ["CHR_01", "CHR_02", "CHR_03", "CHR_04", "CHR_05", "CHR_06", "CHR_07", "CHR_08",
                   "CHR_09", "CHR_10", "CHR_11", "CHR_12", "CHR_13", "CHR_14", "CHR_15", "CHR_16",
                   "CHR_17", "CHR_18", "CHR_19", "CHR_20", "CHR_21", "CHR_22", "CHR_Un", "CHR_X", "CHR_Y" ],
   "M_musculus" => ["CHR_01", "CHR_02", "CHR_03", "CHR_04", "CHR_05", "CHR_06", "CHR_07", "CHR_08",
                    "CHR_09", "CHR_10", "CHR_11", "CHR_12", "CHR_13", "CHR_14", "CHR_15", "CHR_16",
                    "CHR_17", "CHR_18", "CHR_19", "CHR_Un", "CHR_X", "CHR_Y"],
   "Plasmodium_falciparum"=> ["CHR1", "CHR2", "CHR3", "CHR4", "CHR5", "CHR6", "CHR7", "CHR8", "CHR9",
                              "CHR10", "CHR11", "CHR12", "CHR13", "CHR14"],
   "R_norvegicus" => ["CHR_01", "CHR_02", "CHR_03", "CHR_04", "CHR_05", "CHR_06", "CHR_07", "CHR_08",
                      "CHR_09", "CHR_10", "CHR_11", "CHR_12", "CHR_13", "CHR_14", "CHR_15", "CHR_16",
                      "CHR_17", "CHR_18", "CHR_19", "CHR_20", "CHR_Un", "CHR_X", "CHR_Y"],
   "Saccharomyces_cerevisiae" => ["CHR_I", "CHR_II", "CHR_II", "CHR_IV", "CHR_V", "CHR_VI", 
                                  "CHR_VII", "CHR_VIII", "CHR_IX", "CHR_X", "CHR_XI", "CHR_XII",
                                  "CHR_XIII", "CHR_XIV", "CHR_XV", "CHR_XVI"],
   "Schizosaccharomyces_pombe" => ["CHR_I", "CHR_II", "CHR_III"]
);



## ftp to NCBI and get the asn files in the directores specified
## in the %toget. Creates a file comff that holds the organism name
## and the asn files downloaded from the organims. comff is passed to parser 
sub ftp
{
   my $ftp = Net::FTP->new($ftp_info[0]) || die "Could not create ftp object";

   $ftp->login($ftp_info[1], $ftp_info[2]) || die "Could not login";
   $ftp->binary || die "Could not switch to binary";
   print LOG "Logged in successful\n";


   open ( COMFF , ">$default[1]" );


   foreach my $org ( keys %toget )
   {
      $chr = $toget{$org};      # get the array for each organism
      $tmp = $org;
      $tmp =~ tr/_/ /;
      print COMFF "$tmp\n";     # write organism name to comff
      foreach $dir (@$chr) 
      {
         $ftp->cwd("$ftp_info[3]/$org/$dir") || die "Could not cwd to genomes/$org/$dir";
         @files = $ftp->ls("-F");
         foreach $file (@files) {               # find all asn files in directory
            if( $file =~ /\.asn/ ) {
                $ftp->get ( "$file", "$default[2]/$file" ) || die "Could not get $org/$dir/$file\n";
                if ( $file =~ /\.gz/ ) {
                   $file =~ s/\.gz//;
                   print COMFF "\t$file\n"; 
                } else {
                   print COMFF "\t$file\n";
                }
            }
         }
      }
   }
   close ( COMFF );

   print LOG "Got files complete genome\n";


   $ftp->quit;
}

### unzip all the asn files
sub unzip 
{
   open LIST, "ls -F $default[2] |";
   while ( $file = <LIST> )
   {
      chomp $file;
      if ($file =~ /\.gz/ ) {
          # print STDOUT "Unzipping $default[2]/$file\n";
          system ("gunzip $default[2]/$file");
      }
   } 
   print LOG "Unzipped file.\n";
}

### cleanup all files downloaded, all temps created
sub cleanup 
{
   system("rm -fr $default[2]" );
   unlink("$default[1]");
   print LOG "Removing $default[2]\nDeleting $default[1]\n";
}



   #### main starts here

   #### makes sure pregen.pl is also in same directory
   if ( ! (-f "pregen.pl" ) ) { die "pregen.pl must also be in this directory."; }
   if ( ! (-f "gen_cxx" ) ) { die "gen_cxx must also be in this directory."; }
      
   #### creates a director to download asn files into 
   if ( ! (-d $default[2]) ) {
      system("mkdir $default[2]");
   }

   #### back up old genff if found
   if ( -f $default[0] ){
      $backup = $default[0] . ".backup";
      system("mv $default[0] $backup");
   }


   open(LOG, ">$default[3]") || die "No log file";

   #### download all asn files and unzip
   ftp();
   unzip ();

   #### execute the preparser on the downloaded files
   system ("./pregen.pl $default[1] $default[2]");
   print LOG "Executed: ./pregen.pl $default[1] $default[2]\n";


   #### parse preparser-generated files and save data into genff
   system ("./gen_cxx -kloodge 0 -infile $default[1] -outfile $default[0] -dir $default[2]");
   print LOG "Executed: ./gen_cxx -kloodge 0 -infile $default[1] -outfile $default[0] -dir $default[2]\n";
   

   cleanup();

   close(LOG);
   print STDOUT "See $default[3] for results\n";

# $Log: gen.pl,v $
# Revision 1.2  2004/02/05 19:59:31  hlieu
# added checks for robustness
#
# Revision 1.1  2004/02/03 20:30:06  hlieu
# Used to rebuild genff from scatch
#
#
#
