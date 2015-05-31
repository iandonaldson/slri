#!/usr/bin/perl

# $Id: locuslinkcron_odbc.pl,v 1.6 2005/04/14 19:30:18 zwang Exp $
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
#  cron which updates LocusLink tables
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
  $flagfile = shconfig->ReadIntrez( "pathflags" ) . "ll_complete.flg";
  $path_inputfiles = shconfig->ReadIntrez( "pathinputfiles" );

  # DB related
  my ($dsn, $dbh);
  
  $mymail = "| /usr/bin/mailx -s \"LocusLink cron status\" " . $admail;
  $fullexec = $path."llparser";
  $logfile = "locuslinklog";
  $File = "LL_tmpl.gz";
  $fullfile = $path.$File;
  #Make sure parser llparser is in the path
  my $parserBinary = $path."llparser";
  if (!(-s $parserBinary)) 
  {
    die "Parser $parserBinary not found.\n";
  }

  open(MAIL, $mymail);
  print MAIL "LocusLink update.\n";

  #set database engine type
  $ISAM = "myisam";
  $INNO = "innodb";
  $engine = @ARGV[0];
  if ($engine ne $ISAM && $engine ne $INNO) {
     print MAIL "Please provide myisam or innodb as the engine type.\n";
     close (MAIL);
     die "Cannot get DB engine type.";
  }

  if(!($ftp = Net::FTP->new("ftp.ncbi.nih.gov", Passive=>0)))
  {
    print MAIL "Cannot connect.\n";
    close (MAIL);
    open TOUCH, ">$flagfile"; close TOUCH;
    die "Cannot connect.";
  }
  if(!($ftp -> login("anonymous", $admail)))
  {
    print MAIL "Cannot login.\n";
    close (MAIL);
    open TOUCH, ">$flagfile"; close TOUCH;
    die "Cannot login.";
  }
  if(!($ftp->cwd("/refseq/LocusLink/")))
  {
    print MAIL "Cannot cwd refseq/LocusLink.\n";
    close (MAIL);
    open TOUCH, ">$flagfile"; close TOUCH;
    die "Cannot cwd to refseq/LocusLink.";
  }
  # Download the file taxdump.tar.gz only if the file is newer than local copy
  $parser_run = "FALSE";
  if ((!(-s $File)) || ($ftp->mdtm($File) > ($^T - 86400*(-M $fullfile))))
  {
    if (!($ftp->binary))
    {
      print MAIL "Cannot change to binary.\n";
      close(MAIL);
      open TOUCH, ">$flagfile"; close TOUCH;
      die "Cannot change to binary.\n";
    }
    if(!($ftp->get($File, $fullfile)))
    {
      print MAIL "Cannot get $File";
      close (MAIL);
      open TOUCH, ">$flagfile"; close TOUCH;
      die "Cannot get $File.";
    }
    print MAIL "Got $File";
    $parser_run = "TRUE";
  }
  else
  {
    #Log message to LOG file indicating no need to download new file
    print MAIL "File LL_tmpl.gz up-to-date.\n";
    open TOUCH, ">$flagfile"; close TOUCH;
  }
  if(!($ftp->quit))
  {
    print MAIL "Cannot quit ftp.\n";
    close (MAIL);
    open TOUCH, ">$flagfile"; close TOUCH;
    die "Cannot quit ftp.";
  }
  if((-s $fullfile) && ($parser_run eq "TRUE"))
  {
     ########################################################
     # drop and recreate MyISAM tables
     ########################################################
     if ($engine eq $ISAM) {
       # delete the existing data table then create a new one.
       if (connect_to_db() != 1) {
         print MAIL "Unable to connect to DB\n";
         close(MAIL);
         die "Cannot connect to DB.";
       }
     
       # remove all recrods generated by llparser
       if (delete_records() != 1) {
       #if (drop_create_table() != 1) {
         print MAIL "Unable to remove all recrods generated by llparser\n";
         close(MAIL);
         die "Cannot remove records generated by llparser.";
       }
     
       # disconnect
       disconnect_from_db();
     }


     #uncompress the source file
     system ("gunzip -f $fullfile");
     #extract from tar ball
     $fullfile =~ s/.gz//;

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
     #run llparser
     if(system ($command) == 0)
     {
       print MAIL "LocusLink UPDATE OK.\n";
       open TOUCH, ">$flagfile"; close TOUCH;
     }
     else
     {
       print MAIL "Rebuild LocusLink failed!\n";
     }
     system ("gzip $fullfile");

     # move files to dir inputfiles/
     if (system ("mkdir -p $path_inputfiles") == 0)
     {
        print MAIL "Dir $path_inputfiles made\n";
        if (system ("mv $fullfile.gz $path_inputfiles") == 0) {
           print MAIL "Move all data file to $path_inputfiles\n";
           print MAIL "Done\n";
        }
        else {
           print MAIL "Failed to move files.\n";
        }
     }
     else
     {
        print MAIL "Failed to make directory $path_inputfiles\n";
     }
  }
  
  close(MAIL);



######################################################################
##
## a sub-routine to get database setup from two configuration files:
## ./.intrenzrc and HOME/.odbc.ini, connect to the database to 
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
  $dbh = DBI->connect ($dsn, $user_name, $password, {RaiseError => 1});
  
  return 1;
}
 
######################################################################
##
## a sub-routine to remove all records inserted by llparser
## i.e. records with ll_id>0 
##
#####################################################################
sub delete_records() {

  # remove all records in ll_cdd and ll_omim
  #$dbh->do("DELETE FROM ll_cdd;");
  $dbh->do("DELETE FROM ll_omim;");
  
  # remove records in ll_go and ll_llink with ll_id>0
  $dbh->do("DELETE FROM ll_llink where ll_id>0;");
  $dbh->do("DELETE FROM ll_go where ll_id>0;");
  
  return 1;
}

######################################################################
##
## a sub-routine to drop tables and recreate them 
##
#####################################################################
sub drop_create_table() {

  # drop tables
  #@aTable = ("ll_omim", "ll_go", "ll_llink", "ll_cdd");
  @aTable = ("ll_omim", "ll_go", "ll_llink");
  foreach $item (@aTable){
  	$dbh->do("DROP TABLE IF EXISTS $item");
  }
    
  # create new data tables.
  $dbh->do("CREATE TABLE ll_omim (rowid INTEGER NOT NULL AUTO_INCREMENT,
                      ll_id INTEGER NOT NULL,
                      omim_id INTEGER NOT NULL,
                      PRIMARY KEY (ll_id, omim_id),
                      INDEX illomim_rowid(rowid),
                      INDEX illomim_llid(ll_id),
                      INDEX illomim_omimid(omim_id));");

  $dbh->do("CREATE TABLE ll_go (rowid INTEGER NOT NULL AUTO_INCREMENT,
                      ll_id INTEGER NOT NULL,
                      go_id INTEGER,
                      pmid INTEGER,
                      evidence VARCHAR(50),
                      INDEX illgo_rowid(rowid),
                      INDEX illgo_llid(ll_id),
                      INDEX illgo_goid(go_id),
                      INDEX illgo_pmid(pmid));");
  	
  $dbh->do("CREATE TABLE ll_llink (rowid INTEGER NOT NULL AUTO_INCREMENT,
                      ll_id INTEGER NOT NULL,
                      gi INTEGER NOT NULL,
                      map TEXT,
                      PRIMARY KEY (ll_id, gi),
                      INDEX illll_rowid(rowid),
                      INDEX illll_llid(ll_id), 
                      INDEX illll_gi(gi));");

  #$dbh->do("CREATE TABLE ll_cdd (rowid INTEGER NOT NULL AUTO_INCREMENT,
  #                    ll_id INTEGER NOT NULL,
  #                    cdd_id VARCHAR(50),
  #                    evalue DECIMAL(20, 10),
  #                    INDEX illcdd_rowid(rowid),
  #                    INDEX illcdd_llid(ll_id),
  #                    INDEX illcdd_cddid(cdd_id));");

  $dbh->disconnect();

  return 1; 
}

######################################################################
##
## a sub-routine to drop tables and recreate them 
##
#####################################################################
sub disconnect_from_db() {
  $dbh->disconnect();
}




# $Log: locuslinkcron_odbc.pl,v $
# Revision 1.6  2005/04/14 19:30:18  zwang
# Change inno to innodb and isam to myisam
#
# Revision 1.5  2005/02/03 22:07:39  zwang
# Add options to update in InnoDB.
# Stop updating ll_cdd since it is imported from RPSDB.
# Added update of GO files.
#
# Revision 1.4  2004/08/05 16:19:41  zwang
# Remove DB2 related code.
#
# Revision 1.3  2004/07/23 19:37:46  zwang
# Cleaned up code. Added $ and $ where missing.
#
# Revision 1.1  2004/07/23 16:12:46  zwang
# Scripts to update ODBC SeqHound.
#

