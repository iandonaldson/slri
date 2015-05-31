#!/usr/bin/perl

$Id: db2redundcron.pl,v 1.1.1.1 2002/08/24 08:29:37 root Exp $
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
#	Marc Dumontier (hexbinary@hotmail.com)
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
#  cron which updates REDUND database (db2 version)
#
#
#******************************************************************************************

use Net::FTP;

  # things to customize
  # your e-mail
  $admail = "mdumontier\@mdsp.com";
  #directore with the executable
  $path = "/seqh/home/prdseqhi/seqhound/updates/";
  
  # full path to db2 executable
  $db2bin = "/seqh/home/prdseqhi/sqllib/bin/db2";

  $mymail = "| mail ".$admail;
  $fullexec = $path."db2redund";
  $logfile = "db2redund.log";
  $File = "nr.Z";
  $fullfile = $path.$File;

  open(MAIL, $mymail);
  print MAIL "REDUND update.\n";
  if(!($ftp = Net::FTP->new("ftp.ncbi.nih.gov")))
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
  if(!($ftp->cwd("blast/db")))
  {
    print MAIL "Cannot cwd blast/db.\n";
    close (MAIL);
    die "Cannot cwd to blast/db.";
  }
  if(!($ftp->get($File, $fullfile)))
  {
    print MAIL "Cannot get $File";
    close (MAIL);
    die "Cannot get $File.";
  }
  if(!($ftp->quit))
  {
    print MAIL "Cannot quit ftp.\n";
    close (MAIL);
    die "Cannot quit ftp.";
  }

  if(-s $fullfile)
  {
     #uncompress the source file
     system ("uncompress $fullfile");
     $fullfile =~ s/.Z//;
     #run DB2 redund parser
     if(system ("$fullexec -i $fullfile -o redund.txt") == 0)
     {
       print MAIL "Redund OK.\n";
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

     if(system("$db2bin \"load from redund.txt of del replace into seqhound.redund\"") == 0)
     {
       print MAIL "DB2 Load OK.\n";
     }
     else
     {
       #if there is problem print out log file into e-mail
       print MAIL "DB2 Load failed!\n";  
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
     system ("rm -f redund.txt");
     system ("compress $fullfile");
  }
  else
  {
     print MAIL "Did not get the file $File.\n";
  }
system("mv $logfile $path");
print MAIL "Done\n";
close(MAIL);



$Log: db2redundcron.pl,v $
Revision 1.1.1.1  2002/08/24 08:29:37  root
Automated import from Sourceforge.net

Revision 1.1  2002/08/23 09:26:40  mdumonti
db2 version of redund cron job


