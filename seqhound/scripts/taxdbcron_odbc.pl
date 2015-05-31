#!/usr/bin/perl

# $Id: taxdbcron_odbc.pl,v 1.7 2005/04/26 19:30:40 zwang Exp $ 
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
#  cron which updates Taxonomy tables
#
#
#******************************************************************************************

use Net::FTP;
use DBI;
use shconfig;

  # things to customize
  # your e-mail
  $admail = shconfig->ReadIntrez( "mail" );
  #directory with the executable
  $path = shconfig->ReadIntrez( "pathupdates" );
  # where to save input files after process is finished
  $path_inputfiles = shconfig->ReadIntrez( "pathinputfiles" );
  
  $fullexec = $path."importtaxdb";
  $logfile = "taxdblog";
  $File = "taxdump.tar.gz";
  $fullfile = $path.$File;

  #email content
  $mail = "";

  # error messages
  $ERROR = "-- ERROR";
  $has_arg_error = "FALSE";
  $has_ftp_error = "FALSE";
  $has_db_error = "FALSE";
  $has_file_error = "FALSE";
  $has_update_error = "FALSE";

  #Make sure parser importtaxdb is in the path
  if (!(-s $fullexec)) 
  {
    $mail .= "$ERROR: Parser importtaxdb not found.\n";
    $has_file_error = "TRUE";
    send_email();
  }

  #set database engine type
  $ISAM = "myisam";
  $INNO = "innodb";
  $engine = @ARGV[0];
  if ($engine ne $ISAM && $engine ne $INNO) {
     $mail .= "$ERROR: Not provide myisam or innodb as the engine type.\n";
     $has_arg_error = "TRUE";
     send_email();
  }

  if(!($ftp = Net::FTP->new("ftp.ncbi.nih.gov", Passive=>0)))
  {
    $mail .= "$ERROR: Cannot connect to FTP.\n";
    $has_ftp_error = "TRUE";
    send_email();
  }
  if(!($ftp -> login("anonymous", $admail)))
  {
    $mail .= "$ERROR: Cannot login.\n";
    $has_ftp_error = "TRUE";
    send_email();
  }
  if(!($ftp->cwd("/pub/taxonomy/")))
  {
    $mail .= "$ERROR: Cannot cwd to /pub/taxonomy/.\n";
    $has_ftp_error = "TRUE";
    send_email();
  }

  # Download the file taxdump.tar.gz only if the file is newer than local copy
  $parser_run = "FALSE";
  if ((!(-s $File)) || ($ftp->mdtm($File) > ($^T - 86400*(-M $fullfile))))
  {
    if (!($ftp->binary))
    {
      $mail .= "$ERROR: Cannot change to binary.\n";
      $has_ftp_error = "TRUE";
      send_email();
    }
    if(!($ftp->get($File, $fullfile)))
    {
      $mail .= "$ERROR: Cannot get $File";
      $has_ftp_error = "TRUE";
      send_email();
    }
    else
    {
      $mail .= "Got $File\n";
    }
    $parser_run = "TRUE";
  }
  else
  {
    #Log message to LOG file indicating no need to download new file
    $mail .= "File taxdump.tar.gz up-to-date.\n";
    send_email();
  }
  $ftp->quit;
  
  if((-s $fullfile) && ($parser_run eq "TRUE"))
  {
     ########################################################
     # drop and recreate MyISAM tables
     ########################################################
     if ($engine eq $ISAM) {
        # delete the existing data table then create a new one.
        if (drop_create_table() != 1) {
           $mail .= "$ERROR: Cannot remove and create tables for Taxdb\n";
           $has_db_error = "TRUE";
           send_email();
        }
     } 

     #uncompress the source file
     system ("gunzip -f $fullfile");
     #extract from tar ball
     $fullfile =~ s/.gz//;
     system ("tar -xf $fullfile");

     #####################################
     # provide extra argument for InnoDB
     #####################################
     my $command;
     if ($engine eq $ISAM) {
       $command = "nohup $fullexec";
     }
     elsif ($engine eq $INNO) {
       $command = "nohup $fullexec -u T";
     }
     #run importtaxdb parser
     if(system ($command) == 0)
     {
       $mail .= "Done\n";
     }
     else
     {
       $mail .= "$ERROR: Update failed\n";
       $has_update_error = "TRUE";
       send_email();
     }
     # Remove the .dmp files
     my $dmpfiles = $path . "*.dmp";
     system ("rm $dmpfiles");
     system ("rm gc.prt");
     system ("rm readme.txt");
     system ("gzip $fullfile");
     
     # move files to dir inputfiles/
     if (system ("mkdir -p $path_inputfiles") == 0)
     {
        if (system ("mv $fullfile.gz $path_inputfiles") != 0) {
           $mail .= "$ERROR: Cannot move files.\n";
        }
     }
     else
     {
        $mail .= "$ERROR: Cannot make directory $path_inputfiles\n";
     }
  }
 
  send_email();



######################################################################
##
## a sub-routine to get database setup from two configuration files:
## ./.intrenzrc and HOME/.odbc.ini, connect to the database to 
## drop the table then create a new one.
##
#####################################################################
sub drop_create_table() {
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
  $dbh = DBI->connect ($dsn, $user_name, $password, {RaiseError => 1});
  
  # drop tables
  @aTable = ("taxdb", "gcodedb", "divdb", "del", "merge");
  foreach $item (@aTable){
  	$dbh->do("DROP TABLE IF EXISTS $item");
  }
    
  # create new data tables.
  $dbh->do("CREATE TABLE taxdb (rowid INTEGER NOT NULL AUTO_INCREMENT,
                    tid INTEGER NOT NULL,
                    asn MEDIUMBLOB,
                    PRIMARY KEY (tid),
                    INDEX itax_rowid (rowid),
                    INDEX itax_tid (tid));");

  $dbh->do("CREATE TABLE gcodedb (rowid INTEGER NOT NULL AUTO_INCREMENT, 
                    gcid INTEGER NOT NULL,
                    asn MEDIUMBLOB,
                    PRIMARY KEY (gcid),
                    INDEX igcode_rowid (rowid),
                    INDEX igcode_gcid(gcid) );");
  	
  $dbh->do("CREATE TABLE divdb (rowid INTEGER NOT NULL AUTO_INCREMENT,
                    did INTEGER NOT NULL,
                    asn MEDIUMBLOB,
                    PRIMARY KEY (did),
                    INDEX idiv_rowid (rowid),
                    INDEX idiv_did(did));");

  $dbh->do("CREATE TABLE del (rowid INTEGER NOT NULL AUTO_INCREMENT,
                    tid INTEGER NOT NULL,
                    PRIMARY KEY (tid),
                    INDEX idel_rowid (rowid),
                    INDEX idel_tid(tid) );");
                    
  $dbh->do("CREATE TABLE merge (rowid INTEGER NOT NULL AUTO_INCREMENT,
                    otid INTEGER NOT NULL,
                    ntid INTEGER NOT NULL,
                    PRIMARY KEY (otid),
                    INDEX imerge_rowid (rowid),
                    INDEX imerge_ntid(ntid),
                    INDEX imerge_otid(otid));");

  $dbh->disconnect();

  return 1; 
}

###############################################
#
# a subroutine to send e-mail.
#
##############################################
sub send_email() {
  $subject = "TAX update: ";

  if ($has_arg_error eq "TRUE"
      || $has_ftp_error eq "TRUE"
      || $has_file_error eq "TRUE"
      || $has_db_error eq "TRUE"
      || $has_update_error eq "TRUE")
  {
    $subject .= "Error: ";

    if ($has_arg_error eq "TRUE") {
       $subject .= "arg,";
    }
    if ($has_ftp_error eq "TRUE") {
       $subject .= "ftp,";
    }
    if ($has_file_error eq "TRUE") {
       $subject .= "file,";
    }
    if ($has_db_error eq "TRUE") {
       $subject .= "db,";
    }
    if ($has_update_error eq "TRUE") {
       $subject .= "update,";
    }
  }
  else
  {
    $subject .= "OK";
  }

  # set up send mail command
  $mymail = "| /usr/bin/mailx -s \"$subject\" " . $admail;

  #open mail message to log events
  open(MAIL, $mymail);
  print MAIL $mail;
  close MAIL;
  exit(0);
}


# $Log: taxdbcron_odbc.pl,v $
# Revision 1.7  2005/04/26 19:30:40  zwang
# Changed for better error message handling.
#
# Revision 1.6  2005/04/14 19:30:18  zwang
# Change inno to innodb and isam to myisam
#
# Revision 1.5  2005/02/03 22:15:05  zwang
# Add options to update in InnoDB.
#
# Revision 1.4  2004/08/05 16:19:41  zwang
# Remove DB2 related code.
#
# Revision 1.3  2004/07/23 19:37:48  zwang
# Cleaned up code. Added $ and $ where missing.
#
# Revision 1.1  2004/07/23 16:12:47  zwang
# Scripts to update ODBC SeqHound.
#
#




