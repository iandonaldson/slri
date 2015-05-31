#!/usr/bin/perl
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
#          Victor Gu 
#
#
#  REVISION/CONTRIBUTION NOTES:
#
#  April 7,2005  Original MSH Revision.
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
#  SeqHound download and installation script
#
#
#******************************************************************************************
############
# The script needs two command line parameters to run:
# The first one is the platform (valid parameter is either "linux" or "solaris")
# The second one is the instllation directory (e.g. /export/home/seqhound/exec)
use Net::FTP;
$srctar="seqhound_3.0_src.tar.gz";

sub downloadsrc
{
  my $ftphost = "ftp.blueprint.org";
  my $srcdir = "pub/SeqHound/Code";

  print "Contacting FTP host $hostname\n";
  $ftpsession = Net::FTP->new($ftphost);
  if ($ftpsession->login("anonymous", "vgu\@blueprint.org") != 1)
  {
    die "Cannot log into $ftphost\n";
  }
  
  print"Downloading $srctar from directory $srcdir\n";
  if ($ftpsession->cwd($srcdir) !=1) 
  { 
    die "Can not log into directory $srcdir\n";
  }

  if ($ftpsession->binary eq "")
  {
    die "Cannot change download mode to binary\n";

  }
 
  if (($downloadfile = $ftpsession->get($srctar)) eq "")
  {
    die "Cannot get file $srctar. Please verify $srctar exists in $srcdir on $ftphost\n";
  }
  else
  {

    print "Downloading $srctar complete\n";
  } 

}

sub unpacktar
{
  print "Start unpacking $srctar\n";
  $zippedfile = $srctar;
  if (system (" gunzip $srctar ") !=0 )
  {
    die "Fail to unzip $srctar\n";
  }
  else
  {
    print "Done unzip $srctar\n";
  }
  $zippedfile =~ s/.gz//;
  if (system (" tar xvf $zippedfile ") !=0 )
  {
    die "Fail to Unpack $zippedfile\n";
  }
  else
  {
    print "Done Unpack $zippedfile\n";
  }
}

################# main function ##################

downloadsrc();
unpacktar();
