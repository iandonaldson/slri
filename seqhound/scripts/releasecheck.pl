#!/usr/bin/perl

# $Id: releasecheck.pl,v 1.1.1.2 2002/05/14 08:27:37 root Exp $
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
#           Ruth Isserlin, Katerina Michalickova and Christopher W.V. Hogue hogue@mshri.on.ca
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
#  check date on source files on NCBI FTP site
#
#
#******************************************************************************************

use Net::FTP;
use Time::localtime;

#your e-mail address
$admail = "katerina\@mshri.on.ca";

$FtpHost = "ftp.ncbi.nih.gov";
$mymail = "| mail ".$admail;
@months = ("Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec");
$myfile = "asn.list";



$ftp = Net::FTP->new($FtpHost);

if ($ftp->login("anonymous","katerina\@mshri.on.ca") != 1) {
    open(MAIL, $mymail);
    print MAIL "ReleaseCheck: Can't login to $FtpHost\n";
    close(MAIL);
    die "Can't login to $FtpHost\n";
}

$NewCwd = "/ncbi-asn1";
if ($ftp->cwd($NewCwd) != 1) {
    open(MAIL, $mymail);
    print MAIL "ReleaseCheck: Can't cwd to $NewCwd\n";
    close(MAIL);
    die "Can't cwd to $NewCwd\n";
}

if (($list = $ftp->dir) eq "") {
    open(MAIL, $mymail);
    print MAIL "ReleaseCheck: Nothing found in the directory $NewCwd\n";
    close(MAIL);
    die "Nothing found in the directory $NewCwd\n";
}

$ftp->quit;

if(open(LIST, ">$myfile") != 1){
    open(MAIL, $mymail);
    print MAIL "ReleaseCheck: Could not create file.\n";
    close(MAIL);
    die "Could not create file.\n";
}


for($item=0; $item <= $#{$list}; $item++){
		print LIST "$list->[$item]\n";
}

close (LIST);

$tm = localtime;
$today_day = $tm->mday;
$today_mon = $months[($tm->mon)];

if (open(LIST, $myfile) != 1)
{
   open(MAIL, $mymail);
   print MAIL "ReleaseCheck: Cannot open file.\n";
   close(MAIL);
   die "Cannot open file.\n";
}
while (<LIST>)
{
   $line = $_;
   @words = split(/\s+/, $line);
   $mon = $words[5];
   $day = $words[6];
}

if($mon =~ /^$today_mon$/)
{
  if($day =~ /^$today_day$/)
  {
    open(MAIL, $mymail);
    print "ReleaseCheck: New release today!!!!\n";
    print MAIL  "FTP date: ";
    print MAIL  "$mon ";
    print MAIL  "$day\n";
    print MAIL  "Today: ";
    print MAIL  "$today_mon ";
    print MAIL  "$today_day\n";
    close(MAIL);
  }
}

close (LIST);	
system("rm $myfile");


# $Log: releasecheck.pl,v $
# Revision 1.1.1.2  2002/05/14 08:27:37  root
# Automated import from Sourceforge.net
#
# Revision 1.2  2002/05/13 20:11:31  kaca
# changed FTP URL for NCBI
#
# Revision 1.1.1.1  2001/05/25 21:36:21  kaca
# initial import
#
# Revision 1.2  2001/04/18 18:49:20  kaca
# added comment
#
# Revision 1.1  2001/04/16 17:56:47  kaca
# initial import
#



