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
#  Ftp of mmdb structure files. Also allows for 'smart' updates when changes occur in the ncbi mmdb  
#
#******************************************************************************************
use Net::FTP;

my $FtpHost = "ftp.ncbi.nih.gov";
my $ftp = Net::FTP->new($FtpHost); 
my $path = "./";
my $idxFile = "mmdb.idx";
my $idxFileFull = $path . $idxFile;
my $oldIdxFile = "oldmmdb.idx";
my $oldIdxFileFull = $path . $oldIdxFile;
my $mmdbID, $addBioStruct, $addBioStructFull, $rmBioStruct, $rmBioStructFull;

# where the mmdb tables will be copies to after they are built
my $to   = "/seqhound/build/mmdb";
my $from = "/seqhound/build/updates/mmdb";
my @backup = ("MMDB.cdx", "MMDB.dbf", "MMDB.fpt", "MMDB.txt", "MMGI.cdx", "MMGI.dbf");

open(LOG, ">mmdb.log") ||  die "Cannot create mmdb.log\n";

# Place the current date at the top of the log record
$date = `date`;
print LOG "$date\n";

if ($ftp->login("anonymous","ksnyder\@mshri.on.ca") != 1) {
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

#change mode back to binary.
if ($ftp->binary eq "") {
         print LOG "Can't change type to binary\n";
         close LOG;
         goto CLEAN;
}

print LOG "Changed to binary.\n";

# If an old mmdb.idx file exists, compare its contents to the newly acquired mmdb.idx file

if(-e $oldIdxFileFull) {
     print LOG "Found old MMDB index file.\n";
     print LOG "Proceeding to identify differences between new and old MMDB.\n";  
     `diff "$idxFile" "$oldIdxFile" > mmdbDiff`;
      if(-z "mmdbDiff") {
           print LOG "No MMDB update is required.\n";
           close LOG;
           goto CLEAN;
      }
      open(INDEX, "mmdbDiff") || goto CLEAN;
      while(<INDEX>) { 
            # Indicates structures added to MMDB  
            if ($_ =~ m/<\s*([0-9]*)\s*.{4}/) {
                 $mmdbID = $1;
                 print LOG "Downloading BioStruc $mmdbID.\n";
                 $addBioStruct = "$mmdbID.val.gz";
                 $addBioStructFull = $path . $addBioStruct;
                 if(!($ftp->get($addBioStruct, $addBioStructFull))) {
                       print LOG "Cannot get $addBioStruct";
                  }
                 print LOG "Download Complete.\n";
            }
            # Indicates structures removed from MMDB
            if ($_ =~ m/>\s*([0-9]*)\s*.{4}/) {
                 $mmdbID = $1;
                 $rmBioStruct = "$mmdbID.val.gz";
                 $rmBioStructFull = $path . $rmBioStruct;
                 print LOG "Removing BioStruc $rmBioStruct.\n";
                 `rm $rmBioStructFull`;
                  if($? == 0)  {
                       print LOG "Removal Complete.\n"; 
                  }
                  else {
                       print LOG "Could not remove BioStruc $rmBioStruc.\n";
                  }
            }     
      }
      close INDEX;
}

# Otherwise, download all files currently housed in the MMDB
else {
     print LOG "Old MMDB index file does not exist.\n";
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
}

if(!($ftp->quit)) {
     print LOG "Could not close ftp connection\n";
     close LOG;
     goto CLEAN;
}
     
print LOG "closing connection";



############################ Script File Cleanup #######################################
# Change newly acquired .idx file to 'old' .idx file
CLEAN:

unlink mmdbDiff;
close (IDX);
close (LOG);

$admail = "vgu\@blueprint.org";
$mymail = "| /usr/bin/mailx -s \"Mmdb status\" " .$admail;
open(MAIL, $mymail);
print MAIL "cbmmdb update runs on the 10th of each month.\n";

# directory where the new MMDB tables are created

# .intrezrc has been
if(system("./cbmmdb -n T -m T") == 0)
{
   print MAIL "cbmmdb update successful.\n";
}else {
   print MAIL "cbmmdb update failed.\n";
}

# system("mv $to ${to}_tmp");
# system("ln -s $from $to");

# moves the new files to the build directory
my $i;
# foreach my $file (@backup) 
# {
#    if ( -f "${to}_tmp/$file" ){
#      $i = system("cp $from/$file ${to}_tmp/$file");
#   }
# }

# if(0 == $i){
#   print MAIL "moved data tables to $to successfully.\n";
# }else {
#    print MAIL "move to $to unsuccessful.\n";
# }

# system("rm $to");
# system("mv ${to}_tmp $to");


close (MAIL);

`mv $idxFileFull $oldIdxFileFull`;
if($? != 0) {
     print LOG "Could not rename .idx file to 'old' .idx file\n";
     close LOG;
}

# $Log: mmdb_build.pl,v $
# Revision 1.1  2004/07/14 16:15:47  vgu
# Script for ftp and build MMDB module
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
