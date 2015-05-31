#!/usr/bin/perl

# $Id: homologenecron_odbc.pl,v 1.1 2005/04/26 14:57:54 zwang Exp $
#
#*******************************************************************************************
#  Homologene Updater Script
#
#  Seqhound Bioinformatics Database
#
#  The Blueprint Initiative
#  522 University Avenue, 9th Floor, Suite 900
#  Toronto, Ontario, Canada, M5G 1W7
#
#  Hogue Lab - University of Toronto Biochemistry Department
#  Samuel Lunenfeld Research Institute, Mount Sinai Hospital
#
#  Publication to cite:
#       Michalickova K, Bader GD, Dumontier M, Lieu H, Betel D, Isserlin R, Hogue CW.
#       SeqHound: biological sequence and structure database as a platform for bioinformatics research.
#       BMC Bioinformatics. 2002 Oct 25;3(1):32.
#       PMID: 12401134
#
#  Copyright Notice:
#
#  Copyright 2003, 2004 Mount Sinai Hospital (MSH)
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
#  Authors:
#       Michael Matan, Zhe Wang
#
#
#  DESCRIPTION
#  cron which updates homologene tables.
#
#
#******************************************************************************************

use Net::FTP;
use DBI;
use shconfig;

  ##########################
  # things to customize
  ##########################
  # directory with executable
  $path = shconfig->ReadIntrez( "pathupdates" );
  # your e-mail
  $admail = shconfig->ReadIntrez( "mail" );
  # where to save input files after process is finished
  $path_inputfiles = shconfig->ReadIntrez( "pathinputfiles" );

  $File = "homologene.data";
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

  # database
  my $dbh;

  ##########################
  #set database engine type
  ##########################
  $ISAM = "myisam";
  $INNO = "innodb";
  $engine = @ARGV[0];
  if ($engine ne $ISAM && $engine ne $INNO) {
     $mail .= "$ERROR: Not provide myisam or innodb as the engine type.\n";
     $has_arg_error = "TRUE";
     send_email();
  }

  ##########################
  #set action
  ##########################
  $NEW = "new";
  $UPDATE = "update";
  $action = @ARGV[1];
  if ($action ne $NEW && $action ne $UPDATE) {
     $mail .= "$ERROR: Not provide new or update as the action.\n";
     $has_arg_error = "TRUE";
     send_email();
  }


  ##########################
  # download file
  ##########################
  if(!($ftp = Net::FTP->new("ftp.ncbi.nih.gov", Passive=>0)))
  {
    $has_ftp_error = "TRUE";
    send_email();
  }
 
  if ($ftp->login("anonymous",$admail) != 1) {
    $mail .= "$ERROR: Cannot login.\n";
    $has_ftp_error = "TRUE";
    send_email();
  }
    
  $NewCwd = "/pub/HomoloGene/current/";
  if ($ftp->cwd($NewCwd) != 1) {
    $mail .= "$ERROR: Cannot cwd to $NewCwd.\n";
    $has_ftp_error = "TRUE";
    send_email();
  }
  $Pwd = $ftp->pwd;

  if ($ftp->ascii eq "") {
    $mail .= "$ERROR: Cannot change to ascii\n";
    $has_ftp_error = "TRUE";
    send_email();
  }

  ## get file only if not exist or out of date
  if ((!(-s $File)) || ($ftp->mdtm($File) > ($^T - 86400*(-M $fullfile)))) {
    if(!($ftp->get($File, $fullfile)))
    {
      $mail .= "$ERROR: Cannot get $File.\n";
      $has_ftp_error = "TRUE";
      send_email();
    } else {
      $mail .= "Got file $File.\n";
    }
  }
  else {
    $mail .= "File up to date.\n";
    send_email();
  }
  $ftp->quit;


  ########################################################
  # connect to database
  ########################################################
  if (connect_to_db() != 1) {
    $mail .= "$ERROR: Cannot connect to database\n";
    $has_db_error = "TRUE";
    send_email();
  }
  else {
    $mail .= "Connect to database\n";
  }

  ########################################################
  # drop and recreate MyISAM tables
  ########################################################
  if ($engine eq $ISAM || $action eq $NEW) {
    # drop the existing data table then create a new one.
    if (drop_create_table() != 1) {
       $mail .= "$ERROR: Cannot remove and create tables\n";
       $has_db_error = "TRUE";
       send_email();
    }
    else {
      $mail .= "Tables recreated\n";
    }
  }
  elsif ($engine eq $INNO) {
    # delete the existing data.
    if (delete_data() != 1) {
       $mail .= "$ERROR: Cannot delete existing data\n";
       $has_db_error = "TRUE";
       send_email();
    }
    else {
      $mail .= "Data deleted\n";
    }
  }

  ###################################
  # parse in data
  ###################################
  if(-s $fullfile)
  {
    if (parse_file() != 1) {
       $mail .= "$ERROR: Failed to parse file\n";
       $has_update_error = "TRUE";
       send_email();
    }
    else {
      $mail .= "File parsed successfully\n";
    }
  }
  else {
     $mail .= "$ERROR: File missing.\n";
     $has_file_error = "TRUE";
     send_email();
  } 

  $mail .= "Done\n";

  $dbh->commit();
  $dbh->disconnect();

  send_email(); 


######################################################################
##
## a sub-routine to get database setup from two configuration files:
## ./.intrenzrc and HOME/.odbc.ini, connect to the database in
## AutoCommit off mode. 
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
  $dbh = DBI->connect ($dsn, $user_name, $password, {RaiseError => 1, AutoCommit => 0});

  return 1;
}

  
######################################################################
##
## a sub-routine to drop and recreate table
##
#####################################################################
sub drop_create_table() {
  # drop tables
  @aTable = ("hg_homologene");
  foreach $item (@aTable){
  	$dbh->do("DROP TABLE IF EXISTS $item");
  }

  # create new data tables.
  $sql_stmt = "CREATE TABLE hg_homologene(rowid INTEGER NOT NULL AUTO_INCREMENT,
  				ts TIMESTAMP NOT NULL,
  				hid INTEGER,
  				taxid INTEGER,
  				geneid INTEGER,
  				genesymbol VARCHAR(30) NOT NULL,
  				gi INTEGER,
  				accession VARCHAR(25) NOT NULL,
  				version SMALLINT,
  				INDEX ihgg_rowid(rowid),
  				INDEX ihgg_ts(ts),
  				INDEX ihgg_hid(hid),
  				INDEX ihgg_taxid(taxid),
  				INDEX ihgg_geneid(geneid),
  				INDEX ihgg_gi(gi),
  				INDEX ihgg_acc(accession)) ";

  if ($engine eq $ISAM) {
	$sql_stmt .= "TYPE=MYISAM";
  } 
  elsif ($engine eq $INNO) {
	$sql_stmt .= "TYPE=INNODB";
  } 

  $dbh->do($sql_stmt);

  return 1; 
}

######################################################################
##
## a sub-routine to delete existing data
##
#####################################################################
sub delete_data() {
  @aTable = ("hg_homologene");
  foreach $item (@aTable){
  	$dbh->do("DELETE FROM $item");
  }
  return 1; 
}


###############################################
#
# a subroutine to parse the file.
#
##############################################
sub parse_file() {
  if (!(open (INFILE, "$fullfile"))) {
	$mail .= "Cannot open file $fullfile\n";
	$has_file_error = "TRUE";
	send_email();
  }

  while (<INFILE>) {
	chomp;
	$line = $_;
	@words = split(/\t/, $line);

	$hid = $words[0];
	$taxid = $words[1];
	$geneid = $words[2];
	$genesymbol = $dbh->quote("$words[3]");
	$gi = $words[4];
	$access_all = $words[5];

	@words = split(/\./, $access_all);
	$access = $dbh->quote("$words[0]"); 
	$version = $words[1];

#	$sql_stmt = "INSERT INTO hg_homologene (hid, taxid, geneid, genesymbol, gi, accession, version) VALUES ($hid, $taxid, $geneid, $genesymbol, $gi, $access, $version";
#print "$sql_stmt\n";

	$dbh->do(qq{INSERT INTO hg_homologene (hid, taxid, geneid, genesymbol, gi, accession, version) VALUES ($hid, $taxid, $geneid, $genesymbol, $gi, $access, $version)});
  } 

  return 1;
}


###############################################
#
# a subroutine to send e-mail.
#
##############################################
sub send_email() {
  $subject = "HG update: ";

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


# $Log: homologenecron_odbc.pl,v $
# Revision 1.1  2005/04/26 14:57:54  zwang
# New script to build/update homologene table.
#

