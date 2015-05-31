#!/usr/bin/perl

# $Id: ystcron.pl,v 1.1.1.1 2003/06/26 08:53:44 root Exp $
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
$exe = "/home/lieu/slri/seqhound/build/cb/yst";

# path to the data files
# TO CHANGE: set the path according to your file structure 
$datapath = "/home/lieu/slri/seqhound/build/cb/data/";

# TO CHANGE: admin's email account to send cron messages to
$admail = "lieu\@mshri.on.ca";

# The ftp site 
$FtpHost = "genome-ftp.stanford.edu";

# The file you want to retrieve
$getfile = "orf_geneontology.all.tab";
#####################################################################
#####################################################################
# Retrieving the refseq data files from Stanford ftp site

$mailto = "| mail ".$admail;
open(MAIL, $mailto) ||  die "Cannot open mail\n";
# flush out the output to MAIL.
select(MAIL);
$| = 1;

$ftp = Net::FTP->new($FtpHost);

# login into the ftp site as anonymous 
if ($ftp->login("anonymous",$admail) != 1) {
   die "Can't login to $FtpHost\n";
}
print "Logged in to $FtpHost.\n";

# go to yeast refseq directory
$NewCwd = "pub/yeast/data_download/literature_curation/";
if ($ftp->cwd($NewCwd) != 1) {
   die "Can't cwd to $NewCwd\n";
}
$Pwd = $ftp->pwd;
print "pwd=$Pwd\n";

if ($ftp->binary eq "") {
   die "Can't change type to binary\n";
}
$local = $datapath.$getfile;
if(($download = $ftp->get($getfile, $local)) eq "")
{
   die "Cannot get $getfile\n";
}
print "Got $local\n";

$ftp->quit;

###################################################################
###################################################################
# Running the yst parser

print "\nRunning the YST parser\n";

# execute the parser with the input file
if( system("$exe -i $local") == 0 )
{
  print "Parsed $local OK\n";
}else{
  print "Failed parsing of $local\n";
}
print MAIL "\nDone\n";
close(MAIL);

# $Log: ystcron.pl,v $
# Revision 1.1.1.1  2003/06/26 08:53:44  root
# Automated import from Sourceforge.net
#
# Revision 1.1  2003/06/25 18:16:01  haocl
# cron job to retrieve GO annotation and run executable
#
# Revision 1.1  2003/06/25 17:02:14  haocl
# adding script that retrieves data file and runs the parser for SGD DB
#
#
