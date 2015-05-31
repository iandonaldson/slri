#!/usr/bin/perl

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
#  cron which updates REDUND database
#
#
#******************************************************************************************

use Net::FTP;

  # things to customize
  # your e-mail
  $admail = "seqhound.admin\@blueprint.org";
  #directory with the executable
  $path = "/hibbert-local/shoundtest/3.taxdb.files/";
  #directory with the backup version of the TAXDB tables
  $dbpath = "/hibbert-local/shoundtest/datab/";

  $mymail = "| /usr/bin/mailx -s \"TAXDB cron status\" " . $admail;
  $fullexec = $path."importtaxdb";
  $logfile = "taxdblog";
  $File = "taxdump.tar.gz";
  $fullfile = $path.$File;
  @taxdblist = ("DEL.*", "DIV.*", "GCODE.*", "MERGE.*", "TAX.*");
  #Make sure parser importtaxdb is in the path
  my $parserBinary = $path."importtaxdb";
  if (!(-s $parserBinary)) 
  {
    die "Parser importtaxdb not found.\n";
  }

  unlink("/seqhound/build/flags/taxdbcron.flg");
  open(MAIL, $mymail);
  print MAIL "TAXDB update.\n";
  if(!($ftp = Net::FTP->new("ftp.ncbi.nih.gov", Passive=>0)))
  {
    print MAIL "Cannot connect.\n";
    close (MAIL);
    die "Cannot connect.";
  }
  if(!($ftp -> login("anonymous", $admail)))
  {
    print MAIL "Cannot login.\n";
    close (MAIL);
    die "Cannot login.";
  }
  if(!($ftp->cwd("/pub/taxonomy/")))
  {
    print MAIL "Cannot cwd pub/taxonomy.\n";
    close (MAIL);
    die "Cannot cwd to pub/taxonomy.";
  }
  # Download the file taxdump.tar.gz only if the file is newer than local copy
  $parser_run = "FALSE";
  if ((!(-s $File)) || ($ftp->mdtm($File) > ($^T - 86400*(-M $fullfile))))
  {
    if (!($ftp->binary))
    {
      print MAIL "Cannot change to binary.\n";
      die "Cannot change to binary.\n";
    }
    if(!($ftp->get($File, $fullfile)))
    {
      print MAIL "Cannot get $File";
      close (MAIL);
      die "Cannot get $File.";
    }
    $parser_run = "TRUE";
  }
  else
  {
    #Log message to LOG file indicating no need to download new file
    print MAIL "File taxdump.tar.gz up-to-date.\n";
  }
  if(!($ftp->quit))
  {
    print MAIL "Cannot quit ftp.\n";
    close (MAIL);
    die "Cannot quit ftp.";
  }
  if((-s $fullfile) && ($parser_run eq "TRUE"))
  {
     # backup existing taxdb tables
     my $backupdir = $dbpath."old_taxdb/";
     system("mkdir -p $backupdir");
     foreach $item (@taxdblist)
     {
       my $tableEntry = $dbpath.$item;
       system ("mv $tableEntry $backupdir");
     }
     
     #uncompress the source file
     system ("gunzip -f $fullfile");
     #extract from tar ball
     $fullfile =~ s/.gz//;
     system ("tar -xf $fullfile");
     #run importtaxdb parser
     if(system ("$fullexec ") == 0)
     {
       print MAIL "TAXDB UPDATE OK.\n";
       #Copy new tables to /seqhound/build
       my $shbuild = "/seqhound/build/datab/";
       foreach $item (@taxdblist)
       {
         my $newtable = $dbpath.$item;
         system ("cp $newtable $shbuild");
       }
     }
     else
     {
       # If rebuild fails, restore old tables 
       my $oldtaxdblist = $backupdir."*";
       system ("cp $oldtaxdblist $dbpath");

       #Print out log file into e-mail
       print MAIL "Rebuild TAXDB failed!\n";
       if(!open (LOG, $logfile) != 1)
       {
         print MAIL "Cannot open taxdblog.\n";
         close (MAIL);
         die "Cannot open taxdblog.\n";
       }
       while(<LOG>)
       {
         print MAIL $_;
       }
       close(LOG);
     }
     # Remove the .dmp files
     my $dmpfiles = $path . "*.dmp";
     system ("rm $dmpfiles");
     system ("gzip $fullfile");
  }
  
  # Create the flag anyway as seqhound_cron needs to see the flag to
  # start backup scripts
  open TOUCH, ">/seqhound/build/flags/taxdbcron.flg"; close TOUCH;
  print MAIL "Done\n";
  close(MAIL);

# $Log: taxdbcron.pl,v $
# Revision 1.5  2004/05/17 18:25:05  vgu
# change TAXDB build location
#
# Revision 1.4  2004/05/14 17:26:48  vgu
# Add check for parser importtaxdb
#
# Revision 1.3  2004/05/13 20:23:52  vgu
# correct flag name
#
# Revision 1.2  2004/05/13 20:18:28  vgu
# Modify logic for creating job flag
#
# Revision 1.1  2004/05/13 20:04:27  vgu
# Cron job script for daily TAXDB upate
#
# Revision 1.6  2004/05/12 16:02:51  vgu
# correct error in unlink path to redund flag
#
# Revision 1.5  2004/05/11 20:15:25  vgu
# Download and run parser only if a newer version nr.gz on FTP site
#
# Revision 1.4  2004/04/06 15:40:12  vgu
# Force unzip nr.gz even if nr exists in directory see bug 1995 for details
#
# Revision 1.1.1.8  2003/01/24 09:32:58  root
# Automated import from Sourceforge.net
#
# Revision 1.9  2003/01/23 20:06:31  kaca
# added binary mode command
#
# Revision 1.8  2002/11/09 13:21:42  kaca
# consolidating log file clean up; all logs are now moved away by the last cron isshoundoncron.pl
#
# Revision 1.7  2002/09/27 18:21:59  kaca
# removed write permission command, since all CGIs open tables read only it is no longer needed
#
# Revision 1.6  2002/07/04 15:08:26  kaca
# typo fix
#
# Revision 1.5  2002/07/03 22:24:04  kaca
# added table permissions change
#
# Revision 1.3  2002/06/21 20:21:09  kaca
# fixed user comment
#
# Revision 1.2  2002/05/13 20:11:31  kaca
# changed FTP URL for NCBI
#
# Revision 1.1  2002/02/08 23:17:25  kaca
# updates REDUND table
#




