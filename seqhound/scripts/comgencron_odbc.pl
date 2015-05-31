#!/usr/bin/perl

# $Id: comgencron_odbc.pl,v 1.5 2004/08/11 14:08:41 zwang Exp $
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
#  cron which updates Complete Genome tables.
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
  $path_inputfiles = shconfig->ReadIntrez( "pathinputfilescomgen" );
  #current release
  $release = shconfig->ReadIntrez( "defaultrelease" );
  # flag file
  $flagfile = shconfig->ReadIntrez( "pathflags" ) . "comgen_complete.flg";

  $mymail = "| /usr/bin/mailx -s \"Comgen cron status\" " . $admail;
  @exec = ("gen.pl", "ncbi.bacteria.pl", "genftp.pl", "chrom", "iterateparti", "humouse_build.sh", "comgen");
  @fullexec;
  foreach $item (@exec) {
  	$newitem = $path . $item;
  	push(@fullexec, $newitem);
  }
  $logfile = "comgenlog";

  open(MAIL, $mymail);
  print MAIL "Comgen update.\n";

  
  ################################################
  # construct file genff 
  # using gen.pl and ncbi.bacteria.pl
  ################################################
  if ((-s $fullexec[0]) && (-s $fullexec[1])) {
     if(system ("$fullexec[0] > output") == 0)
     {
       print MAIL "gen.pl OK.\n";
       
     } 
     else 
     {
       print MAIL "ERROR: gen.pl FAILED.\n";
       open TOUCH, ">$flagfile"; close TOUCH;
       close(MAIL);
       exit(0);
     }
     
     if(system ("$fullexec[1] > output") == 0)
     {
       print MAIL "ncbi.bacteria.pl OK.\n";
       
     } 
     else 
     {
       print MAIL "ERROR: ncbi.bacteria.pl FAILED.\n";
       open TOUCH, ">$flagfile"; close TOUCH;
       close(MAIL);
       exit(0);
     }
  } else {
     print MAIL "ERROR: Cannot find $fullexec[0] or $fullexec[1]\n";
     open TOUCH, ">$flagfile"; close TOUCH;
     close(MAIL);
     exit(0);
  }
  
  ################################################
  # download *.asn files
  # using genftp.pl
  ################################################
  if (-s $fullexec[2]) {
     if(system ("$fullexec[2] > output") == 0)
     {
       print MAIL "genftp.pl OK.\n";
       
     } 
     else 
     {
       print MAIL "ERROR: genftp.pl FAILED.\n";
       open TOUCH, ">$flagfile"; close TOUCH;
       close(MAIL);
       exit(0);
     }     
  } else {
     print MAIL "ERROR: Cannot find $fullexec[2]\n";
     open TOUCH, ">$flagfile"; close TOUCH;
     close(MAIL);
     exit(0);
  }
  
  ################################################
  # drop then rebuild and populate table CHROM
  ################################################
  if ((-s $fullexec[3]) && (-s $fullexec[4]) && (-s $fullexec[5])) {
     ############
     # re-create 
     ############
     if (drop_create_table() != 1) {
        print MAIL "ERROR: Failed to remove and create tables CHROM\n";
        close(MAIL);
        exit(0);
     } else {
        print MAIL "CHROM dropped and recreated successfully\n";
     }
     
     ############
     # populate 
     # using chrom
     ############
     if(system ("$fullexec[3] -i genff -n F > output") == 0)
     {
       print MAIL "chrom OK.\n";
       
     } 
     else 
     {
       print MAIL "ERROR: chrom FAILED.\n";
       close(MAIL);
       exit(0);
     }         
  } else {
     print MAIL "Cannot find $fullexec[3] or $fullexec[4] or $fullexec[5]\n";
     close(MAIL);
     exit(0);
  } 
  
  
  ################################################
  # remove all records with division='gbchm'
  # using iterateparti
  ################################################
  if (-s $fullexec[4]) {
     if (system ("$fullexec[4] -p gbchm -d T > output") == 0) {
        print MAIL "remove records with division='gbchm' OK\n";
     } else {
        print MAIL "ERROR: Failed to remove records with division='gbchm'\n";
        close(MAIL);
        exit(0);
     }
  } else {
     print MAIL "ERROR: Cannot find $fullexec[4]\n";
     close(MAIL);
     exit(0);
  } 
  
  ################################################
  # move all AME related files to a subdir
  # We are not handling these files for now
  ################################################
  if (rename_AME_files() != 1) {
     print MAIL "ERROR: Failed to rename AME files\n";
     close(MAIL);
     exit(0);
  }
  
  ################################################
  # add records to core tables
  # using humouse_build.sh
  ################################################
  if (-s $fullexec[5]) {
     if (system ("$fullexec[5] $release > output") == 0) {
        print MAIL "humouse_build.sh OK\n";
     } else {
        print MAIL "ERROR: Failed to run humouse_build.sh\n";
        close(MAIL);
        exit(0);
     }
  } else {
     print MAIL "ERROR: Cannot find $fullexec[5]\n";
     close(MAIL);
     exit(0);
  } 
  
  ################################################
  # reset kloodge to 0 for all records in TAXGI
  ################################################
  if (reset_kloodge() != 1) {
     print MAIL "ERROR: Failed to resetkloodge to 0\n";
     close(MAIL);
     exit(0);
  }
 
  ################################################
  # update kloodge
  # using comgen
  ################################################
  if (-s $fullexec[6]) {
     if (system ("$fullexec[6] -l T -p ./ > output") == 0) {
        print MAIL "comgen OK\n";
     } else {
        print MAIL "ERROR: Failed to run comgen\n";
        close(MAIL);
        exit(0);
     }
  } else {
     print MAIL "ERROR: Cannot find $fullexec[6]\n";
     close(MAIL);
     exit(0);
  } 
  
  
  ################################################
  # move files to dir inputfiles/
  ################################################  
  if (system ("mkdir -p $path_inputfiles") == 0)
  {
       print MAIL "Dir $path_inputfiles made\n";
       if (system ("mv -f *.asn $path_inputfiles; cp -rp AME $path_inputfiles; rm -rf AME") == 0) {
          print MAIL "Move all *.asn files and AME to $path_inputfiles\n";
          print MAIL "Done\n";
       }
       else {
           print MAIL "ERROR Failed to move files.\n";
       }
  }
  else
  {
       print MAIL "ERROR Failed to make directory $path_inputfiles\n";
  }
  
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
  
  # drop table
  $dbh->do("DROP TABLE IF EXISTS chrom");
    
  # create new data tables.
  $dbh->do("CREATE TABLE chrom (rowid INTEGER NOT NULL AUTO_INCREMENT,
  				taxid INTEGER NOT NULL,
  				kloodge INTEGER NOT NULL,
  				chromflag INTEGER NOT NULL,
  				access VARCHAR(20) NOT NULL,
  				name TEXT NOT NULL,
  				PRIMARY KEY (kloodge),
  				INDEX ichrom_rowid(rowid),
  				INDEX ichrom_taxid(taxid),
  				INDEX ichrom_kl(kloodge),
  				INDEX ichrom_acc(access) );");

  $dbh->disconnect();

  return 1; 
}


#####################################################################
#
# a sub to move all AME (bee) related files to a subdir
# We are not handling these files for now
#
#####################################################################
sub rename_AME_files() {

  if (system ("mkdir -p AME") != 0) {
     print MAIL "Failed to mkdir for AME\n";
     return 0;
  }
  if (system ("mv -f ame* AME/") != 0) {
     print MAIL "Failed to mv -f ame* AME/\n";
     return 0;
  }
  if (system ("mv -f protein.asn AME/protein_b.asn") != 0) {
     print MAIL "Failed to mv -f protein.asn AME/protein_b.asn\n";
     return 0;
  }
  if (system ("mv -f rna.asn AME/rna_b.asn") != 0) {
     print MAIL "Failed to mv -f rna.asn AME/rna_b.asn\n";
     return 0;
  }

  return 1;
}

#####################################################################
#
# a sub to reset kloodge to 0 for all records in TAXGI
# before running comgen
#
#####################################################################
sub reset_kloodge() {

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
  
  # reset kloodge
  $dbh->do("UPDATE taxgi SET kloodge=0 WHERE kloodge>0");
    
  $dbh->disconnect();

  return 1;
}

# $Log: comgencron_odbc.pl,v $
# Revision 1.5  2004/08/11 14:08:41  zwang
# Fix bug in deleting AME.
#
# Revision 1.4  2004/08/09 14:49:01  zwang
# Change the way to move directory AME.
#
# Revision 1.3  2004/08/05 16:19:41  zwang
# Remove DB2 related code.
#
# Revision 1.2  2004/07/26 21:19:30  zwang
# Fixed error in specifying directory for input files.
#
# Revision 1.1  2004/07/23 16:12:45  zwang
# Scripts to update ODBC SeqHound.
#




