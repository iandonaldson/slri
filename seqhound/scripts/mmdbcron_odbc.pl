#!/usr/bin/perl

# $Id: mmdbcron_odbc.pl,v 1.6 2005/02/03 22:12:57 zwang Exp $
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
use DBI;
use shconfig;

my $FtpHost = "ftp.ncbi.nih.gov";
my $ftp = Net::FTP->new($FtpHost, Passive=>0);

my $path = shconfig->ReadIntrez( "pathupdates");
$admail = shconfig->ReadIntrez( "mail" );
$logfile = "mmdb.log";
# flag file
$flagfile = shconfig->ReadIntrez( "pathflags" ) . "mmdb_complete.flg";

my $idxFile = "mmdb.idx";
my $idxFileFull = $path . $idxFile;

my $killIdxFile = "kill.idx";
my $killIdxFileFull = $path . $killIdxFile;
my $newIdxFile = "new.idx";
my $newIdxFileFull = $path . $newIdxFile;

my $mmdbID, $addBioStruct, $addBioStructFull, $rmBioStruct, $rmBioStructFull;
my $kill_flag = "FALSE";
my $new_flag = "FALSE";
my $update_flag = "FALSE";

# an array to store the mmdbids which are killed
my @killmmdbids;
# an array to store the mmdbids which are new
my @newmmdbids;

# DB related
my ($dsn, $dbh);

unlink($flagfile);
open(LOG, ">$logfile") ||  die "Cannot create $logfile\n";

# Place the current date at the top of the log record
$date = `date`;
print LOG "$date\n";

if ($ftp->login("anonymous",$admail) != 1) {
    print LOG "Can't login to $FtpHost\n";
    close LOG;
    open TOUCH, ">$flagfile"; close TOUCH;
    goto CLEAN;
}

#get the current working directory
if (($Pwd = $ftp->pwd) eq "") {
    print LOG "Can't get current directory\n";
    close LOG;
    open TOUCH, ">$flagfile"; close TOUCH;
    goto CLEAN;
}

$NewCwd = "/mmdb/mmdbdata";
if ($ftp->cwd($NewCwd) != 1) {
    print LOG "Can't cwd to $NewCwd\n";
    close LOG;
    open TOUCH, ">$flagfile"; close TOUCH;
    goto CLEAN;
}
$Pwd = $ftp->pwd;
print LOG "pwd= $Pwd\n";

# download files kill.idx and new.idx only if they are newer than local copies
if ((!(-s $killIdxFile)) || ($ftp->mdtm($killIdxFile) > ($^T - 86400*(-M $killIdxFileFull))))
{

  #get the file containing the contents to be ftped in ascii mode
  if ($ftp->ascii eq "") {
      print LOG "Can't change type to ASCII\n";
      close LOG;
      open TOUCH, ">$flagfile"; close TOUCH;
      goto CLEAN;
  }
  print LOG "Changed to ASCII.\n";

  print LOG "Getting $killIdxFile.\n";
  if ($ftp->get($killIdxFile, $killIdxFileFull) eq "") {
      print LOG "Can't get $killIdxFile\n";
      close LOG;
      open TOUCH, ">$flagfile"; close TOUCH;
      goto CLEAN;
  }
  print LOG "Got $killIdxFile.\n";
  $kill_flag = "TRUE";
}
if ((!(-s $newIdxFile)) || ($ftp->mdtm($newIdxFile) > ($^T - 86400*(-M $newIdxFileFull))))
{

  #get the file containing the contents to be ftped in ascii mode
  if ($ftp->ascii eq "") {
      print LOG "Can't change type to ASCII\n";
      close LOG;
      open TOUCH, ">$flagfile"; close TOUCH;
      goto CLEAN;
  }
  print LOG "Changed to ASCII.\n";

  print LOG "Getting $newIdxFile.\n";
  if ($ftp->get($newIdxFile, $newIdxFileFull) eq "") {
      print LOG "Can't get $newIdxFile\n";
      close LOG;
      open TOUCH, ">$flagfile"; close TOUCH;
      goto CLEAN;
  }
  print LOG "Got $newIdxFile.\n";
  $new_flag = "TRUE";
}

#change mode back to binary.
if ($ftp->binary eq "") {
         print LOG "Can't change type to binary\n";
         close LOG;
         open TOUCH, ">$flagfile"; close TOUCH;
         goto CLEAN;
}
print LOG "Changed to binary.\n";


# If a new kill.idx is ftped, remove all the entries from data tables mmdb and mmgi
if($kill_flag == "TRUE") {
      print LOG "Ftped new kill.idx.\n";          
      
      open(KILLID, $killIdxFileFull) || goto CLEAN;
      while(<KILLID>) { 
            # Indicates structures removed from MMDB
            if ($_ =~ m/\s*([0-9]*)\s*.{4}/) {
                 $mmdbID = $1;
                 push(@killmmdbids, $mmdbID);
            }  
      }
      close KILLID;

      # remove the structure files
      foreach $mmdbID(@killmmdbids) {
         $rmBioStruct = "$mmdbID.val.gz";
         $rmBioStructFull = $path . $rmBioStruct;
         print LOG "Removing BioStruc $rmBioStruct ... ";
         `rm -f $rmBioStructFull`;
         if($? == 0)  {
              print LOG "Done.\n"; 
         }
         else {
              print LOG " ** FAILED **\n";
         }
      }
}

if($new_flag == "TRUE") {
      print LOG "Ftped new new.idx.\n";
      
      open(NEWID, $newIdxFileFull) || goto CLEAN;
      while(<NEWID>) { 
           # Indicates structures added to MMDB  
            if ($_ =~ m/\s*([0-9]*)\s*.{4}/) {
                 $mmdbID = $1;
                 push(@newmmdbids, $mmdbID);
            }
      }
      close NEWID;

      # download new structure files
      foreach $mmdbID (@newmmdbids) {
         print LOG "Downloading BioStruc $mmdbID ... ";
         $addBioStruct = "$mmdbID.val.gz";
         $addBioStructFull = $path . $addBioStruct;
         if(!($ftp->get($addBioStruct, $addBioStructFull))) {
              print LOG " ** FAILED **\n";
         } else {
              print LOG "Completed.\n";
         }
      } 
}

if(!($ftp->quit)) {
     print LOG "Could not close ftp connection\n";
     close LOG;
     open TOUCH, ">$flagfile"; close TOUCH;
     goto CLEAN;
}
     
# connect to DB to remove all obsolete records.
# autocommit is set to 0 in connect_to_db.
connect_to_db();
foreach $item (@killmmdbids) {
     delete_record($item);
     print LOG "mmdbid=$item deleted\n";
}
# the structures that are changed are in new.idx.
# should remove them from database as well.
foreach $item (@newmmdbids) {
     delete_record($item);
     print LOG "mmdbid=$item deleted\n";
}
# commit and disconnect.
end_transaction();
disconnect_from_db();


# since parser cbmmdb looks at file "mmdb.idx" for mmdbid,
# need to rename new.idx to mmdb.idx
if (system("cp $newIdxFileFull $idxFileFull") == 0) {
    print LOG "cp $newIdxFileFull $idxFileFull\n";
} else {
    print LOG "Unable to cp $newIdxFileFull $idxFileFull\n";
    open TOUCH, ">$flagfile"; close TOUCH;
    goto CLEAN;
}


############################ Script File Cleanup #######################################
# Change newly acquired .idx file to 'old' .idx file
CLEAN:

close (LOG);

$mymail = "| /usr/bin/mailx -s \"Mmdb cron status\" " .$admail;
open(MAIL, $mymail);

if (open (LOG, $logfile) != 1)
{
     print MAIL "Cannot open $logfile.\n";
     close(MAIL);
     die "Cannot open $logfile.\n";
}
while (<LOG>)
{
     print MAIL $_;
}

# run cbmmdb on new files
if ($new_flag == "TRUE") {
    if(system("nohup ./cbmmdb -u T") == 0)
    {
        print MAIL "cbmmdb update successful.\n";
        open TOUCH, ">$flagfile"; close TOUCH;
    }else {
        print MAIL "cbmmdb update failed.\n";
    }
}

if(system("rm -f *.val.gz") == 0)
{
     print MAIL "*.val.gz files removed successfully.\n";
}else {
     print MAIL "*.val.gz files not removed. \n";
}

close (MAIL);


######################################################################
##
## a sub-routine to connect to database
##
#####################################################################
sub connect_to_db() {

  # get configuration for user_name, password, database, alias (DSN)
  $configfile = ".intrezrc";
  open (CONFIGFILE, "$configfile") || die "Cannot open file $configfile\n";
  while (<CONFIGFILE>) {
        my $line = $_;
        $line =~ s/\s+//g;

        my @words = split(/=/, $line);
        my $attr = $words[0];
        my $value = $words[1];

        if ($attr eq "username") {
                $user_name = $value;
        }
        elsif ($attr eq "password") {
                $password = $value;
        }
        elsif ($attr eq "database") {
                $database = $value;
        }
        elsif ($attr eq "dsn") {
                $alias = $value;
        }
  }

  # get configuration for server and port number from the correct DSN
  $homedir = $ENV{'HOME'};
  $configODBC = $homedir . "/.odbc.ini";
  open (CONFIGODBC, "$configODBC") || die "Cannot open file $configODBC\n";
  $foundAlias=0;
  $gotHost=0;
  $gotPort=0;
  while (<CONFIGODBC>) {
        my $line = $_;
        $line =~ s/\s+//g;
        # find the correct section
        my $target = "[" . $alias . "]";
        if ($line eq $target) {
                $foundAlias=1;
        }

        # get HOST and PORT from the correct section
        if ($foundAlias == 1) {
                my @words = split(/=/, $line);
                my $attr = $words[0];
                my $value = $words[1];
                if ($attr eq "SERVER") {
                        $host = $value;
                        $gotHost = 1;
                }
                elsif ($attr eq "PORT") {
                        $port = $value;
                        $gotPort = 1;
                }
        }
        # finish with the correct section
        if ($gotHost==1 && $gotPort==1) {
                $foundAlias=0;
        }
  }
  $dsn = "DBI:mysql:" . $database . ":" . $host . ":" . $port;
  $table = "redund";
  $dbh = DBI->connect ($dsn, $user_name, $password, {RaiseError => 1, AutoCommit => 0});
}

######################################################################
##
## a sub-routine to disconnect from database
## input: mmdbid 
##
#####################################################################
sub disconnect_from_db() {
  $dbh->disconnect();
}

######################################################################
##
## a sub-routine to delete a record from tables MMDB and MMGI.
## input: mmdbid 
##
#####################################################################
sub delete_record($) {
   $mmdbid = shift;
   
   $dbh->do("DELETE FROM mmdb WHERE mmdbid=$mmdbid;");
   $dbh->do("DELETE FROM mmgi WHERE mmdbid=$mmdbid;");
}

######################################################################
##
## a sub-routine to end a transaction.
## commit the changes, roll back if error occurs.
#####################################################################
sub end_transaction() {
   $dbh->commit;
}


# $Log: mmdbcron_odbc.pl,v $
# Revision 1.6  2005/02/03 22:12:57  zwang
# Change system call to './cbmmdb -u T' for updating either MyISAM or InnoDB.
#
# Revision 1.5  2004/11/23 17:01:27  zwang
# Run cbmmdb if new_flag is set to TRUE.
#
# Revision 1.4  2004/08/05 16:19:41  zwang
# Remove DB2 related code.
#
# Revision 1.3  2004/07/26 21:18:03  zwang
# Run cbmmdb only there are new structures.
#
# Revision 1.2  2004/07/23 19:37:46  zwang
# Cleaned up code. Added $ and $ where missing.
#
# Revision 1.1  2004/07/23 16:12:46  zwang
# Scripts to update ODBC SeqHound.
#
#
