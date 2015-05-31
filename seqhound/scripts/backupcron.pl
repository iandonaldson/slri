#! /usr/bin/perl


# $Id: backupcron.pl,v 1.1.1.1 2002/02/09 09:25:12 root Exp $
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
#           Katerina Michalickova and Christopher W.V. Hogue hogue@mshri.on.ca
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
#  backs up the database
#
#******************************************************************************************

  use Net::FTP;
  use Time::localtime;


  #things to customize
  #directories with the production version of the database; have to have "/" at the end!
  $dbpath1 = "/seqhound/datab/";
  $sendbpath1 = "/seqhound/sendatab/";
  #directories with the backup version of the database; have to have "/" at the end!
  $dbpath2 = "/seqhound/datab2/";
  $sendbpath2 = "/seqhound/sendatab2/";
  #temporal directory names for backup process; have to have "/" at the end!
  $dbpathtemp = "/seqhound/datab_temp/";
  $sendbpathtemp = "/seqhound/sendatab_temp/";
  #mail address to receive update report
  $admail = "katerina\@mshri.on.ca";
  # end of things to customize


  # set up send mail command
  $mymail = "| mail ".$admail;
  #set up absolute paths to tables
  $datablist = $dbpath2."/databls";
  $sendatablist = $sendbpath2."/sendatabls";
  #remove last slash from paths to be able to use symlinks
  $dbsym1 = $dbpath1;
  $dbsym1 =~ s/\/$//;
  $sendbsym1 = $sendbpath1;
  $sendbsym1 =~ s/\/$//;
  $dbsym2 = $dbpath2;
  $dbsym2 =~ s/\/$//;
  $sendbsym2 = $sendbpath2;
  $sendbsym2 =~ s/\/$//;

  #open mail message to log events
  open(MAIL, $mymail);
  print MAIL "SeqHound databases backup.\n";

  #put aside the production version  of the database
  system ("mv $dbpath1 $dbpathtemp");
  system ("mv $sendbpath1 $sendbpathtemp");

  # make symbolical links to the backup copy where the update just happened
  #so it becomes production version for the time being
  system ("ln -s $dbsym2 $dbsym1");
  system ("ln -s $sendbsym2 $sendbsym1");

  #copy the backup version to production
  #create files with directory listing
  system("ls -1 $dbpath2 > $datablist");
  system("ls -1 $sendbpath2 > $sendatablist");
  if(!(open(LIST, $datablist)))
  {
   print MAIL "Cannot open $datablist directory listing.\n";
   close (MAIL);
   die "Cannot open datab directory listing.\n";
  }
  if(!(open(SENLIST, $sendatablist)))
  {
   print MAIL "Cannot open $sendatablist directory listing.\n";
   close (MAIL);
   die "Cannot open sendatab directory listing.\n";
  }
  while (<LIST>)
  {
     chop($_);
     $curfile = $dbpath2.$_;
     system("cp $curfile $dbpathtemp");
  }
  while (<SENLIST>)
  {
     chop($_);
     $curfile = $sendbpath2.$_;
     system("cp $curfile $sendbpathtemp");
  }
  close(LIST);
  close(SENLIST);

  #remove temporary files with directory listing
  system("rm -f $datablist");
  system("rm -f $sendatablist");

  #remove the symbolical link to backup version
  system ("rm -f $dbsym1");
  system ("rm -f $sendbsym1");

  #move the production version back to its normal place
  system ("mv $dbpathtemp $dbpath1");
  system ("mv $sendbpathtemp $sendbpath1");


  print MAIL "Done.\n";
  close(MAIL);

  #$Log: backupcron.pl,v $
  #Revision 1.1.1.1  2002/02/09 09:25:12  root
  #Automated import from Sourceforge.net
  #
  #Revision 1.1  2002/02/08 23:18:03  kaca
  #backup of database files
  #


