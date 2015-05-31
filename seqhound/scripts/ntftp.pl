#!/usr/bin/perl

# $Id: ntftp.pl,v 1.2 2004/01/06 16:52:30 hlieu Exp $
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
#  ftp of nt blast database
#
#******************************************************************************************

use Net::FTP;

open(LOG, ">ntftp.log") ||  die "Cannot create ntftp.log\n";
#flush out the output to the files.
select(LOG);
$| = 1;

$FtpHost = "ftp.ncbi.nih.gov";
print LOG "Logging in.\n";
$ftp = Net::FTP->new($FtpHost); 

if ($ftp->login("anonymous","katerina\@mshri.on.ca") != 1) {
    die "Can't login to $FtpHost\n";
}

print LOG "Logged in.\n";

#get the current working directory
if (($Pwd = $ftp->pwd) eq "") {
    die "Can't get current directory\n";
}

print "pwd=$Pwd\n";

$NewCwd = "/blast/db/FASTA/";
if ($ftp->cwd($NewCwd) != 1) {
    die "Can't cwd to $NewCwd\n";
}
$Pwd = $ftp->pwd;
print LOG "pwd= $Pwd\n";

if ($ftp->binary eq "") {
 	 die "Can't change type to binary\n";
}

print LOG "Changed to binary.\n";

$File = "nt.gz";

print LOG "Getting $File\n";
if ($ftp->get($File) eq "") {
    die "Can't get $File\n";
}

print LOG "closing connection";
close(LOG);
$ftp->quit;

# $Log: ntftp.pl,v $
# Revision 1.2  2004/01/06 16:52:30  hlieu
# fixed ftp directory to FASTA, fixed compression to .gz
#
# Revision 1.1.1.2  2002/05/14 08:27:37  root
# Automated import from Sourceforge.net
#
# Revision 1.2  2002/05/13 20:11:31  kaca
# changed FTP URL for NCBI
#
# Revision 1.1.1.1  2001/05/25 21:36:21  kaca
# initial import
#
# Revision 1.4  2001/02/23 20:42:25  kaca
# license change
#
# Revision 1.3  2000/10/19 16:11:18  kaca
# added CVS keywords and licenses
#
