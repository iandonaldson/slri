#!/usr/bin/perl

# $Id: gocron_odbc.pl,v 1.7 2005/04/21 19:38:11 zwang Exp $
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
#           Christopher W.V. Hogue hogue@mshri.on.ca
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
#  cron which updates GeneOntology tables.
#
#
#******************************************************************************************

use Net::FTP;
use DBI;
use shconfig;

  # things to customize
  # directory with executable
  $path = shconfig->ReadIntrez( "pathupdates" );
  # your e-mail
  $admail = shconfig->ReadIntrez( "mail" );
  # flag file
  $flagfile = shconfig->ReadIntrez( "pathflags" ) . "go_complete.flg";
  # where to save input files after process is finished
  $path_inputfiles = shconfig->ReadIntrez( "pathinputfiles" );

  $fullexec = $path."goparser";
  $logfile = "goparserlog";
  @aFile = ("component.ontology", "function.ontology", "process.ontology");
  $aFullfile[0] = $path.$aFile[0];
  $aFullfile[1] = $path.$aFile[1];
  $aFullfile[2] = $path.$aFile[2];

  #email content
  $mail = "";

  # error messages
  $ERROR = "-- ERROR";
  $has_arg_error = "FALSE";
  $has_ftp_error = "FALSE";
  $has_db_error = "FALSE";
  $has_file_error = "FALSE";
  $has_update_error = "FALSE";

  #set database engine type
  $ISAM = "myisam";
  $INNO = "innodb";
  $engine = @ARGV[0];
  if ($engine ne $ISAM && $engine ne $INNO) {
     $mail .= "$ERROR: Not provide myisam or innodb as the engine type.\n";
     $has_arg_error = "TRUE";
     send_email();
  }

  # download component.ontology, function.ontology, process.ontology
  if(!($ftp = Net::FTP->new("ftp.geneontology.org", Passive=>0)))
  {
    $has_ftp_error = "TRUE";
    send_email();
    open TOUCH, ">$flagfile"; close TOUCH;
  }
 
  if ($ftp->login("anonymous",$admail) != 1) {
    $mail .= "$ERROR: Cannot login.\n";
    $has_ftp_error = "TRUE";
    send_email();
    open TOUCH, ">$flagfile"; close TOUCH;
  }
    
  #get the current working directory
  if (($Pwd = $ftp->pwd) eq "") {
    $mail .= "$ERROR: Cannot get the current directory.\n";
    $has_ftp_error = "TRUE";
    send_email();
  }
    
  $NewCwd = "/pub/go/ontology";
  if ($ftp->cwd($NewCwd) != 1) {
    $mail .= "$ERROR: Cannot cwd to $NewCwd.\n";
    $has_ftp_error = "TRUE";
    send_email();
  }
  $Pwd = $ftp->pwd;
 
  for $i (0 .. 2) {
    my $File = $aFile[$i];
    if (!($ftp->get($File))) {
      $mail .= "$ERROR: Cannot get $File.\n";
      $has_ftp_error = "TRUE";
      send_email();
    }
    else {
      $mail .= "Got file $File.\n";
    }
  } 

  ########################################################
  # drop and recreate MyISAM tables
  ########################################################
  if ($engine eq $ISAM) {
    # delete the existing data table then create a new one.
    if (drop_create_table() != 1) {
       $mail .= "$ERROR: Failed to remove and create tables\n";
       $has_db_error = "TRUE";
       send_email();
    }
    else {
      $mail .= "Tables recreated\n";
    }
  }

  if((-s $aFullfile[0]) && (-s $aFullfile[1] && (-s $aFullfile[2])))
  {
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
     #run goparser
     if(system ($command) == 0)
     {
       $mail .= "Files parsed.\n";
       open TOUCH, ">$flagfile"; close TOUCH;
     }
     else
     {
       $mail .= "$ERROR: Parser failed.\n";
       $has_update_error = "TRUE";
       send_email();
     }
  }
  else {
     $mail .= "$ERROR: File missing.\n";
     $has_file_error = "TRUE";
     send_email();
  } 

  # move files to dir inputfiles/
  if (system ("mkdir -p $path_inputfiles") == 0)
  {
       if (system ("mv *.ontology $path_inputfiles") != 0) {
           $mail .= "$ERROR: Failed to move files.\n";
           $has_file_error = "TRUE";
           send_email();
       }
  }
  else
  {
       $mail .= "$ERROR: Failed to make directory $path_inputfiles\n";
       $has_file_error = "TRUE";
       send_email();
  }
 
  $mail .= "Done\n";
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
  @aTable = ("go_name", "go_parent", "go_reference", "go_synonym");
  foreach $item (@aTable){
  	$dbh->do("DROP TABLE IF EXISTS $item");
  }
    
  # create new data tables.
  $dbh->do("CREATE TABLE go_name(rowid INTEGER NOT NULL AUTO_INCREMENT,
  				go_id INTEGER NOT NULL,
  				go_name TEXT NOT NULL,
  				go_db INTEGER NOT NULL,
  				go_level INTEGER,
  				PRIMARY KEY (go_id, go_name(100), go_db, go_level),
  				INDEX igoname_rowid(rowid),
  				INDEX igoname_goid(go_id));");

  $dbh->do("CREATE TABLE go_parent(rowid INTEGER NOT NULL AUTO_INCREMENT,
  				go_id INTEGER NOT NULL,
  				parent_goid INTEGER NOT NULL,
  				PRIMARY KEY (go_id, parent_goid),
  				INDEX igoparent_rowid(rowid),
  				INDEX igoparent_goid(go_id),
  				INDEX igoparent_pid(parent_goid));");
  	
  $dbh->do("CREATE TABLE go_reference (rowid INTEGER NOT NULL AUTO_INCREMENT,
  				go_id INTEGER NOT NULL,
  				go_ref TEXT NOT NULL,
  				go_ref_db CHAR(50),
  				PRIMARY KEY(go_id, go_ref(100)),
  				INDEX igoref_rowid(rowid),
  				INDEX igoref_go_ref_db(go_ref_db));");

  $dbh->do("CREATE TABLE go_synonym (rowid INTEGER NOT NULL AUTO_INCREMENT,
  				go_id INTEGER NOT NULL,
  				go_syn TEXT NOT NULL,
  				PRIMARY KEY (go_id, go_syn(100)),
  				INDEX igosynonym_rowid(rowid),
  				INDEX igosynonym_go_id(go_id));");

  $dbh->disconnect();

  return 1; 
}

###############################################
#
# a subroutine to send e-mail.
#
##############################################
sub send_email() {
  $subject = "GO update: ";

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


# $Log: gocron_odbc.pl,v $
# Revision 1.7  2005/04/21 19:38:11  zwang
# Changed for better error message handling.
#
# Revision 1.6  2005/04/14 19:30:18  zwang
# Change inno to innodb and isam to myisam
#
# Revision 1.5  2005/02/03 22:05:40  zwang
# Add options to update in InnoDB.
#
# Revision 1.4  2004/08/05 16:19:41  zwang
# Remove DB2 related code.
#
# Revision 1.3  2004/07/23 19:37:44  zwang
# Cleaned up code. Added $ and $ where missing.
#
# Revision 1.1  2004/07/23 16:12:46  zwang
# Scripts to update ODBC SeqHound.
#




