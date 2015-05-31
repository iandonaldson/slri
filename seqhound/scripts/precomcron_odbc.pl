#!/usr/bin/perl

# $Id: precomcron_odbc.pl,v 1.3 2004/07/23 19:37:47 zwang Exp $
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
#  runs precompute as a cron job
#
#
#******************************************************************************************

use shconfig;


# things to customize
# your e-mail
$admail = shconfig->ReadIntrez( "mail" );
#directory with precompute executable
$path = shconfig->ReadIntrez( "pathupdates");
# flag file
$flagfile = shconfig->ReadIntrez( "pathflags" ) . "precomp_complete.flg";
#end of things to customize


$mymail = "| /usr/bin/mailx -s \"Precompute status\" ".$admail;
$fullprec = $path."precompute";
$logfile = "precomputelog";

open(MAIL, $mymail);

if(system ("$fullprec -a update > output") == 0)
{
  #Create success flag
  open TOUCH, ">$flagfile"; close TOUCH;
  print MAIL "Precompute OK.\n";
}
else
{
  print MAIL "Precompute failed!\n";
  if(!open (LOG, $logfile))
  {
    print MAIL "Cannot open precomputelog.\n";
    close (MAIL);
    die "Cannot open precomputelog.\n";
  }
  print MAIL "Done.\n";
  close(LOG);
}

close(MAIL);



# $Log: precomcron_odbc.pl,v $
# Revision 1.3  2004/07/23 19:37:47  zwang
# Cleaned up code. Added $ and $ where missing.
#
# Revision 1.1  2004/07/23 16:12:46  zwang
# Scripts to update ODBC SeqHound.
#
#



