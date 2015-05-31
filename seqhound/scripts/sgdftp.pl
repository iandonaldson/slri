#!/usr/bin/perl

# $Id: sgdftp.pl,v 1.1.1.2 2003/07/30 08:49:15 root Exp $
#*******************************************************************************************
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
#  PROGRAM:
#
#
#  AUTHORS:
#           Hao Lieu 
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
#  Ftp script to down load Yeast Refseq data files  
#  And parse each of the *.faa file and populate SGD database
#  the SGD database maps ORF, gi and accession of yeast Refseq
#******************************************************************************************

# Things that need to be changed in this file are
# marked with TO CHANGE:

use Net::FTP;

##################################################################
##################################################################
# This sections contains all the things that you will need
# to change for your own system and self

# the sgd executable we want to run on the cron job
# TO CHANGE: set the path according to your file structure 
$exe = "/home/lieu/slri/seqhound/build/cb/sgd";

# path to the data files
# TO CHANGE: set the path according to your file structure 
$datapath = "/home/lieu/slri/seqhound/build/cb/data/";

# TO CHANGE: admin's email account to send cron messages to
$admail = "lieu\@mshri.on.ca";

#####################################################################
#####################################################################
# Retrieving the refseq data files from NCBI ftp site

open(LOG, ">sgdftp.log") ||  die "Cannot create sgdftp.log\n";
#flush out the output to the files.
select(LOG);
$| = 1;

$FtpHost = "ftp.ncbi.nih.gov";
$ftp = Net::FTP->new($FtpHost);

# login into the ftp site as anonymous 
if ($ftp->login("anonymous",$admail) != 1) {
    die "Can't login to $FtpHost\n";
}

print LOG "Logged in.\n";

#get the current working directory
if (($Pwd = $ftp->pwd) eq "") {
    die "Can't get current directory\n";
}

print "pwd=$Pwd\n";


# go to yeast refseq directory
$NewCwd = "/refseq/S_cerevisiae/";
if ($ftp->cwd($NewCwd) != 1) {
    die "Can't cwd to $NewCwd\n";
}
$Pwd = $ftp->pwd;
print LOG "pwd= $Pwd\n";

if ($ftp->binary eq "") {
 	 die "Can't change type to binary\n";
}

print LOG "Changed to binary.\n";
# get a listing of the directories
if( (@list = $ftp->ls("-F .")) eq "")
{
   die "Nothing found in the directory $NewCwd\n";
}

# need to go into each directory and get the .faa file
foreach $item (@list)
{
   # looking for directories eg Chr01/ ... Chr16/ 
   if($item =~ /^C(.*)\//)
   { 
      if($ftp->cwd($item) != 1)
      {
         print LOG "Could not access $item directory\n";
      }else{
         # gained entry into directory, 
	 # each directory has files: eg I.asn, I.faa,
	 # I.ffn ... I.va, we want *.faa files in 
         # each directory
         @file = $ftp->ls;
         # print STDOUT "$file[1]\n";
         # $file = "";
         $file[1] =~ /(.*)\.(.*)/;
         $file= $1.".faa";
         # download the .faa files  
         if(($download = $ftp->get($file)) eq "")
         {
            print LOG "Cannot get $file\n";
         }else {
            print LOG "Got $file\n";
         }
         $ftp->cwd("..");
      }
   }
}

print LOG "closing connection";
close(LOG);
$ftp->quit;

###################################################################
###################################################################
# Running the sgd parser

# set the mail to
$mailto = "| mail ".$admail;


# the data input files
@datainput = ("I.faa", "II.faa", "III.faa", "IV.faa", "V.faa", "VI.faa", "VII.faa",
	      "VIII.faa", "IX.faa", "X.faa", "XI.faa", "XII.faa", "XIII.faa", "XIV.faa", 
	      "XV.faa", "XVI.faa", "MT.faa");

open(MAIL, $mailto);
print MAIL "SGD Database parser\n\n";

# do we initialize the SGD database? 
# T for first file parse, then F for subsequent
$init = "T";
foreach $file (@datainput)
{
  # make the path to the data files eg /data/inputfilename
  $input = $datapath.$file;
  # print STDOUT "$exe -i $input -n $init\n";
  # execute the parser with the input file and initialization flag
  if( system("$exe -i $input -n $init") == 0 )
  {
     print MAIL "Parsed $file OK\n";
  }else{
     print MAIL "Failed on $file\n";
  }
  # change the init to not create database again
  $init = "F";
}
print MAIL "\nDone\n";
close(MAIL);

# $Log: sgdftp.pl,v $
# Revision 1.1.1.2  2003/07/30 08:49:15  root
# Automated import from Sourceforge.net
#
# Revision 1.2  2003/07/28 17:01:43  haocl
# script for getting SGD data input files and auto-running parser
# (can be used as cron job)
#
#
