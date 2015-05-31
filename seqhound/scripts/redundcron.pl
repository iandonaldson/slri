#!/usr/bin/perl

# $Id: redundcron.pl,v 1.7 2004/05/28 18:48:17 vgu Exp $
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

  #directore with the executable
  $path = "/seqhound/build/updates/";
  #directory with the backup version of the REDUND table
  $dbpath = "/seqhound/build/datab/";

  $fullexec = $path."redund";
  $logfile = "redundlog";
  $File = "nr.gz";
  $fullfile = $path.$File;
  $rddatab = $dbpath."REDUND.*";

  open(LOG, ">$logfile") || die "Cannot create $logfile\n"; 
  unlink("/seqhound/build/flags/redundcron.flg");
  print LOG "REDUND update.\n";
  if(!($ftp = Net::FTP->new("ftp.ncbi.nih.gov", Passive=>0)))
  {
    print LOG "Cannot connect to NCBI FTP server.\n";
    goto CLEAN;
  }
  if(!($ftp -> login("anonymous", $admail)))
  {
    print LOG "Cannot login into FTP server.\n";
    goto CLEAN;
  }
  if(!($ftp->cwd("blast/db/FASTA/")))
  {
    print LOG "Cannot cwd blast/db/FASTA.\n";
    goto CLEAN;
  }
  # Download the file nr.gz only if the file is newer than local copy
  $parser_run = "FALSE";
  if ((!(-s $File)) || ($ftp->mdtm($File) > ($^T - 86400*(-M $fullfile))))
  {
    if (!($ftp->binary))
    {
      print LOG "Cannot change to binary.\n";
      goto CLEAN;
    }
    if(!($ftp->get($File, $fullfile)))
    {
      print LOG "Cannot get $File";
      goto CLEAN;
    }
    $parser_run = "TRUE";
  }
  else
  {
    #Log message to LOG file indicating no need to download new file
    print LOG "File nr.gz up-to-date.\n";
  }
  if(!($ftp->quit))
  {
    	print LOG "Cannot quit ftp.\n";
      goto CLEAN;
  }

  if((-s $fullfile) && ($parser_run eq "TRUE"))
  {
     #remove the databases, how do I know where they are
     system ("rm -f $rddatab");
     #uncompress the source file
     system ("gunzip -f $fullfile");
     $fullfile =~ s/.gz//;
     #run redund parser
     if(system ("$fullexec -i $fullfile -n T") == 0)
     {
       print LOG "Redund OK.\n";
       # Create the flag in the CLEAN step as the flag is needed anyway by the backup script
       #open TOUCH, ">/seqhound/build/flags/redundcron.flg"; close TOUCH;
       # As per request of Mike Matan
       system ("ssh hydrogen \"cd /scratch/rpsdb ; export CLUSTER=\$PWD/livenode-list_standard ; nohup ./rpsdbUpdateCron.pl >/dev/null 2>\&1 \& \" ");
     }
     else
     {
       #if there is problem print out log file into e-mail
       print LOG "Redund failed!\n";
     }
     system ("gzip $fullfile");
     goto CLEAN;
  }

  print LOG "Done\n";

CLEAN:

# things to customize
# your e-mail
$admail = "seqhound.admin\@blueprint.org";
$mymail = "| /usr/bin/mailx -s \"REDUND cron status\" " . $admail;

close (LOG);

open(MAIL, $mymail);
if (open (LOG, $logfile) != 1)
{
  print MAIL "Cannot open $logfile.\n";
  close (MAIL);
  die "Cannot open $logfile.\n";
}
while(<LOG>)
{
  print MAIL $_;

}
open TOUCH, ">/seqhound/build/flags/redundcron.flg"; close TOUCH;
close (LOG);
close (MAIL);

# $Log: redundcron.pl,v $
# Revision 1.7  2004/05/28 18:48:17  vgu
# streamline structure of script
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




