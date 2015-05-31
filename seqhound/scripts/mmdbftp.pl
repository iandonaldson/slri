#!/usr/bin/perl

#*******************************************************************************************
#  Copyright (C) 2003 Mount Sinai Hospital (MSH)
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
#           Kevin Snyder and Christopher W.V. Hogue hogue@mshri.on.ca
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
#  Ftp of mmdb structure files *.val.gz   
#
#******************************************************************************************
use Net::FTP;

my $FtpHost = "ftp.ncbi.nih.gov";
my $ftp = Net::FTP->new($FtpHost, Passive=>0); 
my $path = "./";
my $idxFile = "mmdb.idx";
my $idxFileFull = $path . $idxFile;
my $idxFileSave = "mmdb.idx_save";
my $idxFileSaveFull = $path . $idxFileSave;
my $killIdxFile = "kill.idx";
my $killIdxFileFull = $path . $killIdxFile;
my $newIdxFile = "new.idx";
my $newIdxFileFull = $path . $newIdxFile;

my $addBioStruct;

open(LOG, ">mmdbftp.log") ||  die "Cannot create mmdbftp.log\n";

# Place the current date at the top of the log record
$date = `date`;
print LOG "$date\n";

if ($ftp->login("anonymous","seqhound.admin\@blueprint.org") != 1) {
    print LOG "Can't login to $FtpHost\n";
    close LOG;
    goto CLEAN;
}

#get the current working directory
if (($Pwd = $ftp->pwd) eq "") {
    print LOG "Can't get current directory\n";
    close LOG;
    goto CLEAN;
}

$NewCwd = "/mmdb/mmdbdata";
if ($ftp->cwd($NewCwd) != 1) {
    print LOG "Can't cwd to $NewCwd\n";
    close LOG;
    goto CLEAN;
}
$Pwd = $ftp->pwd;
print LOG "pwd= $Pwd\n";

#get the file containing the contents to be ftped in ascii mode
if ($ftp->ascii eq "") {
    print LOG "Can't change type to ASCII\n";
    close LOG;
    goto CLEAN;
}
print LOG "Changed to ASCII.\n";

print LOG "Getting $idxFile.\n";
if ($ftp->get($idxFile, $idxFileFull) eq "") {
    print LOG "Can't get $idxFile\n";
    close LOG;
    goto CLEAN;
}
print LOG "Got $idxFile.\n";

print LOG "Getting $killIdxFile.\n";
if ($ftp->get($killIdxFile, $killIdxFileFull) eq "") {
    print LOG "Can't get $killIdxFile\n";
    close LOG;
    goto CLEAN;
}
print LOG "Got $killIdxFile.\n";

print LOG "Getting $newIdxFile.\n";
if ($ftp->get($newIdxFile, $newIdxFileFull) eq "") {
    print LOG "Can't get $newIdxFile\n";
    close LOG;
    goto CLEAN;
}
print LOG "Got $newIdxFile.\n";

#change mode back to binary.
if ($ftp->binary eq "") {
         print LOG "Can't change type to binary\n";
         close LOG;
         goto CLEAN;
}

print LOG "Changed to binary.\n";

print LOG "Proceeding to download all BioStructs found in new MMDB index file.\n";
print LOG "Opening index file.\n";
open (IDX, "$idxFile") || goto CLEAN;

while (<IDX>) {
     /(\d+)/;
     if ($' != '') {
          $addBioStruct = $&;
          $addBioStruct = $addBioStruct . ".val.gz";
          $addBioStructFull = $path . $addBioStruct;
          print LOG "Getting $addBioStruct.\n";
          if (($filename = $ftp->get($addBioStruct, $addBioStructFull)) eq "") {
               print LOG "Can't get $addBioStruct\n";
          }
          else {            
               print LOG "Got $filename.\n";
          }
     }
}


if(!($ftp->quit)) {
     print LOG "Could not close ftp connection\n";
     close LOG;
     goto CLEAN;
}
     
print LOG "closing connection";
goto CLEAN;

exit(0);

############################ Script File Cleanup #######################################
# Change newly acquired .idx file to 'old' .idx file
CLEAN:

close (IDX);
close (LOG);

if (system("mv $idxFileFull $idxFileSaveFull") != 0) {
     print LOG "Could not rename .idx file to .idx_save file\n";
     close LOG;
}

# $Log: mmdbftp.pl,v $
# Revision 1.7  2004/11/17 15:53:26  vgu
# Readd script mmdbftp.pl back to CVS
#
# Revision 1.3  2004/05/14 20:13:00  vgu
# minor changes to syntax
#
# Revision 1.2  2004/05/13 14:07:02  vgu
# Make script clean up mmdb directory before a rebuild
#
# Revision 1.1  2004/05/11 20:56:32  vgu
# renamed mmdbftp.pl to mmdbcron.pl
#
# Revision 1.4  2004/03/10 16:29:49  hlieu
# disabled copying to build directory
#
# Revision 1.3  2004/01/19 15:18:21  hlieu
# improved copying to build directory
#
# Revision 1.4  2003/12/03 15:34:51  ksnyder
# Fixed typos
#
# Revision 1.3  2003/10/31 21:31:59  ksnyder
# Removed gunzip feature
#
# Revision 1.2  2003/10/31 21:01:39  ksnyder
# Added feature to unzip any BioStrucs downloaded from NCBI
#
# Revision 1.1  2003/10/28 22:17:55  ksnyder
# Update script for NCBI's mmdb database. Only downloads or deletes structures that have been added or removed from the MMDB since the last update
#
