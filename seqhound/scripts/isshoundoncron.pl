#!/usr/bin/perl

# $Id: isshoundoncron.pl,v 1.2 2004/03/30 20:32:03 vgu Exp $
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
#  Checks the SeqHound system; runs isshoundon as a cron job
#
#
#******************************************************************************************

# things to customize
# your e-mail
$admail = "seqhound.admin\@blueprint.org";
# update directory with the isshoundon executable
$path = "/seqhound/build/updates/";
#end of things to customize


$mymail = "| /usr/bin/mailx -s \" Isshoundon cron status\" " . $admail;
$fullexe = $path."isshoundon";
$fulllogpath = $path . "logs";

open(MAIL, $mymail);

if(system ("$fullexe") == 1)
{
  print MAIL "SHound is not on!\n";
}
else
{
  print MAIL "SeqHound is OK.\n";
}

close(MAIL);

# clean up logs from all the crons from the same night
system("mv *log $fulllogpath");
system("mv *run $fulllogpath");
system("mv *gis $fulllogpath");


# $Log: isshoundoncron.pl,v $
# Revision 1.2  2004/03/30 20:32:03  vgu
# Commit changes on production server
#
# Revision 1.1.1.3  2002/11/10 09:39:24  root
# Automated import from Sourceforge.net
#
# Revision 1.3  2002/11/09 13:21:42  kaca
# consolidating log file clean up; all logs are now moved away by the last cron isshoundoncron.pl
#
# Revision 1.2  2002/07/04 15:07:17  kaca
# minor comment changes
#
# Revision 1.1  2002/01/21 20:08:52  kaca
# new cron test; tests if the SeqHound remote API is on
#



