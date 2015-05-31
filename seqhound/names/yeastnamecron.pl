#!/usr/bin/perl

# $Id: yeastnamecron.pl,v 1.3 2004/09/27 15:40:25 eburgess Exp $
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
#           Elizabeth Burgess and Christopher W.V. Hogue hogue@mshri.on.ca
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
#  ftps chromosomal_feature.tab file from SGD and runs the
#  yeast name parser.
#
#******************************************************************************************

use strict;
use Net::FTP;
use shconfig;

# things to customize
# your e-mail
my $admail =  shconfig->ReadIntrez( "mail" );

my $mymail = "| /usr/bin/mailx -s \"YeastNames Status\" ".$admail;

my $path = shconfig->ReadIntrez( "pathupdates" );;

open(MAIL, $mymail);

print MAIL "Yeast Name update.\n";

open(LOG, ">yeastnameftp.log") ||  die "Cannot create yeastnameftp.log\n";
#flush out the output to the files.
select(LOG);
$| = 1;

my $FtpHost = "genome-ftp.stanford.edu";
my $ftp = Net::FTP->new($FtpHost);

if ($ftp->login("anonymous",$admail) != 1) {
  print MAIL "Cannot connect.\n";
  close (MAIL);
  die "Can't login to $FtpHost\n";
}

print LOG "Logged in.\n";

#get the current working directory
my $Pwd;
if (($Pwd = $ftp->pwd) eq "") {
    die "Can't get current directory\n";
}

print "pwd=$Pwd\n";

my $NewCwd = "/pub/yeast/data_download/chromosomal_feature/";
if ($ftp->cwd($NewCwd) != 1) {
  print MAIL "Cannot cwd to $NewCwd.\n";
  close (MAIL);
  die "Can't cwd to $NewCwd\n";
}
$Pwd = $ftp->pwd;
print LOG "pwd= $Pwd\n";

if ($ftp->binary eq "") {
  print MAIL "Cannot change to binary.\n";
  close (MAIL);
  die "Can't change type to binary\n";
}

print LOG "Changed to binary.\n";

my $File = "SGD_features.tab";
print LOG "Checking to see if file needs update.\n";

my $fullFile = $path.$File;

if ((!(-s $File)) || 
	  ($ftp->mdtm($File) > ($^T - 86400*(-M $fullFile))))
{  
  print LOG "Getting $File\n";
  if ($ftp->get($File) eq "") 
  {
	print MAIL "Cannot get $File";
	print LOG "Cannot get $File";
    close (MAIL);

    die "Can't get $File\n";
  }

  print LOG "Running parser\n";
  if(system("perl yeastnameparser.pl"))
  {
	print LOG "ERROR: yeastnameparser.pl \n";
	print MAIL "ERROR: yeastnameparser.pl \n";
  }
  else
  {
	print LOG "Done parsing. \n";
	print MAIL "Done parsing. \n";
  }
}
else
{
  print LOG "File does not need update.\n";
  print MAIL "File does not need update.\n";
}

print LOG "closing connection.\n";
print MAIL "closing connection.\n";

close(LOG);
if(!($ftp->quit))
{
  print MAIL "Cannot quit ftp.\n";
  print LOG "Cannot quit ftp.\n";
  close (MAIL);
  die "Cannot quit ftp.";
}

close (MAIL);
close (LOG);
exit (0);
# $Log: yeastnamecron.pl,v $
# Revision 1.3  2004/09/27 15:40:25  eburgess
# Modified to use SGD_features.tab instead of chromosomal_features.tab.
#
# Revision 1.2  2004/08/09 13:17:04  eburgess
# Fixed typo at top of file.
#
# Revision 1.1  2004/08/03 18:30:48  eburgess
# Initial revision.
#
