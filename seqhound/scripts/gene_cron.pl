#!/usr/bin/perl

# $Id: gene_cron.pl,v 1.6 2005/04/14 19:30:18 zwang Exp $
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
#2
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
#           Katerina Michalickova and Christopher W.V. Hogue hogue@mshri.on.ca
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
#  ftps gene flat files from NCBI (ftp.ncbi.nih.gov/gene/DATA) and 
#  runs parse_gene_files.pl. The following files are processed:
#      refseq2gene.gz
#      gene_info.gz
#      gene_history.gz
#      gene2pubmed.gz
#  If arg provided as "update", will delete all existing records,
#  otherwise, do initial build
#
#******************************************************************************************

use strict;
use Net::FTP;
use shconfig;

if (@ARGV[0] ne "innodb" && @ARGV[0] ne "myisam") {
  print "ERROR. Please provide innodb or myisam as the first parameter.\n";
  exit;
}


# things to customize
# your e-mail
my $admail = shconfig->ReadIntrez( "mail" );
# directory with the executable
my $path = shconfig->ReadIntrez( "pathupdates" );
# flag file
my $flagfile = shconfig->ReadIntrez( "pathflags" ) . "gene_complete.flg";
my $mymail = "| /usr/bin/mailx -s \"Gene update status\" ".$admail;

open(MAIL, $mymail);

print MAIL "Gene files update.\n";

open(LOG, ">gene_files_ftp.log") ||  die "Cannot create gene_files_ftp.log\n";
#flush out the output to the files.
select(LOG);
$| = 1;

my $FtpHost = "ftp.ncbi.nih.gov";
my $ftp = Net::FTP->new($FtpHost);

if ($ftp->login("anonymous",$admail) != 1) {
  print MAIL "Cannot connect.\n";
  close (MAIL);
  open TOUCH, ">$flagfile"; close TOUCH;
  die "Can't login to $FtpHost\n";
}

print LOG "Logged in.\n";

my $Pwd;
#get the current working directory
if (($Pwd = $ftp->pwd) eq "") {
    open TOUCH, ">$flagfile"; close TOUCH;
    die "Can't get current directory\n";
}

print "pwd=$Pwd\n";

my $NewCwd = "/gene/DATA/";
if ($ftp->cwd($NewCwd) != 1) {
  print MAIL "Cannot cwd to $NewCwd.\n";
  open TOUCH, ">$flagfile"; close TOUCH;
  close (MAIL);
  die "Can't cwd to $NewCwd\n";
}
$Pwd = $ftp->pwd;
print LOG "pwd= $Pwd\n";

if ($ftp->binary eq "") {
  print MAIL "Cannot change to binary.\n";
  open TOUCH, ">$flagfile"; close TOUCH;
  close (MAIL);
  die "Can't change type to binary\n";
}

print LOG "Changed to binary.\n";

my $runParser = "FALSE";

my $File = "gene2pubmed.gz";
my $pubmedUpdated = GetFile( $File );

$File = "gene2refseq.gz";
my $refseqUpdated = GetFile( $File );


$File = "gene_history.gz";
my $historyUpdated = GetFile( $File );

$File = "gene_info.gz";
my $infoUpdated = GetFile( $File );

print LOG "closing connection";

close(LOG);
if(!($ftp->quit))
{
  print MAIL "Cannot quit ftp.\n";
  print LOG "Cannot quit ftp.\n";
  open TOUCH, ">$flagfile"; close TOUCH;
  close (MAIL);
  die "Cannot quit ftp.";
}

if( $pubmedUpdated eq "TRUE" &&
	$refseqUpdated eq "TRUE" &&
    $historyUpdated eq "TRUE" &&
    $infoUpdated eq "TRUE" )
{
  my $command = "perl parse_gene_files.pl @ARGV[0] @ARGV[1]";

  if(system($command))
  {
	print LOG "ERROR: parse_gene_files.pl \n";
	print MAIL "ERROR: parse_gene_files.pl \n";
  }
  else
  {
        open TOUCH, ">$flagfile"; close TOUCH;
	print LOG "DONE! \n";
	print MAIL "DONE! \n";
  }
}
else
{
        open TOUCH, ">$flagfile"; close TOUCH;
	print LOG "DONE. Files do not need updating. \n";
	print MAIL "DONE/ Files do not need updating. \n";
}

system("mv gene2* inputfiles/");
system("mv gene_history* inputfiles/");
system("mv gene_info* inputfiles/");
system("mv gene_files_ftp.log logs/");
system("mv parse_gene_files.log logs/");


close (MAIL);
close (LOG);
exit (0);

###############################################################
# GetFile
#
# FTPs the given file name, unzips the file and makes sure that
# it only contains uniq lines.
sub GetFile()
{
  my $File = $_[0];
  my $updated = "FALSE";
  my $fullFile = $path.$File;

  print LOG "Getting $File\n";
  print "Getting $File\n";

  if ((!(-s $File)) || 
	  ($ftp->mdtm($File) > ($^T - 86400*(-M $fullFile))))
  {  
	if ( !( $ftp->get($File, $fullFile) ) ) 
	{
	  print MAIL "Cannot get $File";
	  print LOG "Cannot get $File";
          open TOUCH, ">$flagfile"; close TOUCH;
	  close (MAIL);

	  die "Can't get $File\n";
	}

	if( system( "gunzip -f $File" ) )
	{
	  print MAIL "Cannot unzip $File";
	  print LOG "Cannot unzip $File";

          open TOUCH, ">$flagfile"; close TOUCH;
	  close (MAIL);

	  die "Can't unzip $File\n";
	}

	my $newFile = $File;
	$newFile =~ s/.gz//;
	my $uniqFile = $newFile . "Uniq";

	print "working on $newFile to $uniqFile\n";
###############################################################
#Make sure the files do not contain duplicate lines.
	if( system( "uniq $newFile > $uniqFile" ) )
	{
	  print LOG "Cannot make $newFile unique. \n";
	  print MAIL "Cannot make $newFile unique. \n";  

	  close (MAIL);
	  die "Cannot make $File unique.\n";
	}

###############################################################
#Zip the old file.
	if( system( "gzip $newFile" ) )
	{
	  print LOG "Cannot zip $newFile. \n";
	  print MAIL "Cannot zip $newFile. \n";  
	}
	
	$updated = "TRUE";
  }
 
  return $updated;
}


# $Log: gene_cron.pl,v $
# Revision 1.6  2005/04/14 19:30:18  zwang
# Change inno to innodb and isam to myisam
#
# Revision 1.5  2005/02/17 17:27:34  zwang
# Fixed error. Check for cmd line parameter.
#
# Revision 1.4  2005/02/02 22:58:43  zwang
# Take isupdate as a paramter.
#
# Revision 1.3  2005/02/02 22:37:00  zwang
# Take storage engine as a paramter.
#
