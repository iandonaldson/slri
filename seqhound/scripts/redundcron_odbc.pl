#!/usr/bin/perl

# $Id: redundcron_odbc.pl,v 1.6 2005/04/14 19:30:18 zwang Exp $
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
#  cron which updates REDUND table
#
#
#******************************************************************************************

use Net::FTP;
use DBI;
use shconfig;

  # things to customize
  # your e-mail
  $admail = shconfig->ReadIntrez( "mail" );
  #directore with the executable
  $path = shconfig->ReadIntrez( "pathupdates" );
  $flagfile = shconfig->ReadIntrez( "pathflags" ) . "redund_complete.flg";

  $mymail = "| /usr/bin/mailx -s \"Redund cron status\" " . $admail;
  $fullexec = $path."redund";
  $logfile = "redundlog";
  $File = "nr.gz";
  $fullfile = $path.$File;

  open(MAIL, $mymail);
  print MAIL "REDUND update.\n";

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
  if(!($ftp->cwd("blast/db/FASTA/")))
  {
    print MAIL "Cannot cwd blast/db/FASTA.\n";
    close (MAIL);
    open TOUCH, ">$flagfile"; close TOUCH;
    die "Cannot cwd to blast/db.";
  }
  # Download the file nr.gz only if the file is newer than local copy
  $parser_run = "FALSE";
  if ((!(-s $File)) || ($ftp->mdtm($File) > ($^T - 86400*(-M $fullfile))))
  {
    if (!($ftp->binary))
    {
      print MAIL "Cannot change to binary.\n";
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
    $parser_run = "TRUE";
  }
  else
  {
    #Log message to LOG file indicating no need to download new file
    print MAIL "File nr.gz up-to-date.\n";
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
       if (drop_create_table() == 1) {
          print MAIL "Table REDUND was successfully removed and created again.\n"
       } else {
          print MAIL "Failed to remove and create table REDUND\n";
          close (MAIL);
          die "Cannot recreate table.";
       }
     }

     #uncompress the source file
     system ("gunzip -f $fullfile");
     $fullfile =~ s/.gz//;

     #####################################
     # provide extra argument for InnoDB
     #####################################
     my $command;
     if ($engine eq $ISAM) {
       $command = "nohup $fullexec -i $fullfile";
     }
     elsif ($engine eq $INNO) {
       $command = "nohup $fullexec -i $fullfile -u T";
     }

     #run redund parser
     if(system ($command) == 0)
     {
       print MAIL "Redund OK.\n";
       open TOUCH, ">$flagfile"; close TOUCH;
     }
     else
     {
       #if there is problem print out log file into e-mail
       print MAIL "Redund failed!\n";
       if(!open (LOG, $logfile) != 1)
       {
         print MAIL "Cannot open redundlog.\n";
         close (MAIL);
         die "Cannot open redundlog.\n";
       }
       while(<LOG>)
       {
         print MAIL $_;
       }
       close(LOG);
     }
     system ("gzip $fullfile");
  }

  print MAIL "Done\n";
  close(MAIL);


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
  $dbh->do("DROP TABLE IF EXISTS $table");
  # create a new data table.
  $dbh->do("CREATE TABLE $table(rowid INTEGER NOT NULL AUTO_INCREMENT,
                                            gi INTEGER NOT NULL,
                                            rordinal INTEGER NOT NULL,
                                            rgroup INTEGER,
                                            PRIMARY KEY(gi),
                                            INDEX iredund_rowid(rowid),
                                            INDEX iredund_gi(gi),
                                            INDEX iredund_ordinal(rordinal),
                                            INDEX iredund_rgroup(rgroup));");

  $dbh->disconnect();
  
  return 1; 
}


# $Log: redundcron_odbc.pl,v $
# Revision 1.6  2005/04/14 19:30:18  zwang
# Change inno to innodb and isam to myisam
#
# Revision 1.5  2005/02/03 22:14:31  zwang
# Add options to update in InnoDB.
#
# Revision 1.4  2004/08/05 16:19:41  zwang
# Remove DB2 related code.
#
# Revision 1.3  2004/07/23 19:37:47  zwang
# Cleaned up code. Added $ and $ where missing.
#
# Revision 1.1  2004/07/23 16:12:47  zwang
# Scripts to update ODBC SeqHound.
#
#




