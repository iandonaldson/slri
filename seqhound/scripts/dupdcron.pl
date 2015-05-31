#! /usr/bin/perl


# $Id: dupdcron.pl,v 1.6 2004/04/06 15:53:27 vgu Exp $
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
#  updates SeqHound databases from ftp://ftp.ncbi.nih.gov and
#  ftp://ftp.ncbi.nih.gov/refseq/daily, runs under the cron command
#
#******************************************************************************************

  use Net::FTP;
  use Time::localtime;


  # the config file for databases points to backup version so the updates do not happen in production version
  #things to customize....
  #directory with the update executables; has to have "/" at the end!
  $path = "/seqhound/build/updates/";
  #mail address to receive update report
  $admail = "seqhound.admin\@blueprint.org";
  # in case database release is out of bounds (out of 100-200) use the default release
  $default_release = 138;
  # end of things to customize


  # set up send mail command
  $mymail = "| /usr/bin/mailx -s \"Dupd status\" " . $admail;
  #set up absolute paths to executables, update files and databases
  $fullupdate = $path."update";
  $fullupdmother = $path."mother";
# SK: Starts at background
  $fullbackup = $path."mybackup.pl hibbert &";
  $fulllog = $path."logs";
  $fullaso = $path."asofiles";

  #open mail message to log events
  open(MAIL, $mymail);
#  print MAIL "Newly modified to work with new compression.\n";
  #set release according to argument; attempt to check for its validity
  $release = @ARGV[0];
  if($release > 200)
   {
     $release = $default_release;
     print MAIL "Release out of range, using default $default_release.\n";
   }
  if($release < 100)
   {
     $release = $default_release;
     print MAIL "Release out of range, using default $default_release.\n";
   }

  #construct today's update files names
  $tm = localtime;
  $today =  sprintf ("%02d%02d",$tm->mday, ($tm->mon)+1);
  $File = sprintf ("nc%02d%02d.aso.gz",($tm->mon)+1, $tm->mday);
  $File_rs = sprintf ("rsnc.%02d%02d.%02d.bna.Z",($tm->mon)+1, $tm->mday, ($tm->year)+1900);
  $File_rs_gz = sprintf ("rsnc.%02d%02d.%02d.bna.gz",($tm->mon)+1, $tm->mday, ($tm->year)+1900);
 

  #contruct the absolute path to the update files
  $fullfile = $path.$File;
  $fullfile_rs = $path.$File_rs;
  $fullfile_rs_gz = $path.$File_rs_gz;
  $fullfilebk = $fullfile;
  $fullfile_rsbk = $fullfile_rs;
  $fullfile_rsbk_gz = $fullfile_rs_gz;

  print MAIL "SeqHound databases update for $today:\n";

  #ftp update files from NCBI
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
  if(!($ftp->cwd("ncbi-asn1/daily-nc")))
  {
    print MAIL "Cannot cwd to ncbi-asn1/daily-nc.\n";
    #close (MAIL);
    #die "Cannot cwd to ncbi-asn1/daily-nc.";
  }
  if (!($ftp->binary))
  {
    print MAIL "Cannot change to binary.\n";
    die "Cannot change to binary.\n";
  }
  if(!($ftp->get($File, $fullfile)))
  {
    print MAIL "Cannot get $File.\n";
    #close (MAIL);
    #die "Cannot get $File.";
  }
  #get today's refseq update file
  if(!($ftp->cwd("/refseq/daily")))
  {
    print MAIL "Cannot cwd to /refseq/daily.\n";
    #close (MAIL);
    #die "Cannot cwd to /refseq/daily.";
  }
  if (!($ftp->binary))
  {
    print MAIL "Cannot change to binary.\n";
    die "Cannot change to binary.\n";
  }
  if(!($ftp->get($File_rs, $fullfile_rs)))
  {
    print MAIL "Cannot get $File_rs trying $File_rs_gz.\n";
    if(!($ftp->get($File_rs_gz, $fullfile_rs_gz)))
    {
       print MAIL "Cannot get $File_rs_gz.\n";
    }
    #close (MAIL);
    #die "Cannot get $File_rs";
  }
  if(!($ftp->quit))
  {
    print MAIL "Cannot quit ftp.\n";
    close (MAIL);
    die "Cannot quit ftp.";
  }

  #check for update file and update
  if(-s $fullfile)
  {
     system("gunzip $fullfile");
     $fullfile =~ s/.gz//;
     #run update on asofile
     if (system("$fullupdate -i $fullfile") != 0)
     {
      print MAIL "Cannot update $fullfile with $fullupdate.\n";
      close (MAIL);
      die "Cannot update $fullfile with $fullupdate.";
     }
     #run mother on asofile
     if(system("$fullupdmother -i $fullfile -r $release -n F -m F -u T") != 0)
     {
      print MAIL "Cannot update datab with $fullfile with $fullupdmother.\n";
      close (MAIL);
      die "Cannot update datab with $fullfile with $fullupdmother.";
     }

     # clean up, zip up files, put logs in logs dir and binaries into asofiles dir
     system("gzip $fullfile");
     system("mv $fullfilebk $fullaso");
  }
  else
  {
     print MAIL "No update file $File!\n";
  }

  # check for refseq update file and update
  if(-s $fullfile_rs)
  {
     system("uncompress $fullfile_rs");
     $fullfile_rs =~ s/.Z//;
     $temporal = $fullfile_rs;
     $fullfile_rs =~ s/rsnc./rsnc/;
     system("mv $temporal $fullfile_rs");
     #run update on refseq file
     if (system("$fullupdate -i $fullfile_rs") != 0)
     {
      print MAIL "Cannot update with $fullfile_rs with $fullupdate.\n";
      close (MAIL);
      die "Cannot update with $fullfile_rs with $fullupdate.";
     }
     #run mother on refseq file
     if(system("$fullupdmother -i $fullfile_rs -r $release -n F -m F -u T") != 0)
     {
      print MAIL "Cannot update datab with $fullfile_rs with $fullupdmother.\n";
      #close (MAIL);
      #die "Cannot update datab with $fullfile_rs with $fullupdmother.";
     }
     # clean up, zip up files, put binaries into asofiles dir
     system("compress $fullfile_rs");
     $fullfile_rsbk = $fullfile_rs.".Z";
     system("mv $fullfile_rsbk $fullaso");

  }else{
     print MAIL "Mother could not find file $File_rs trying $File_rs_gz.\n";
     if(-s $fullfile_rs_gz)
     {
        system("gunzip $fullfile_rs_gz");
        $fullfile_rs_gz =~ s/.gz//;
        $temporal = $fullfile_rs_gz;
        $fullfile_rs_gz =~ s/rsnc./rsnc/;
        system("mv $temporal $fullfile_rs_gz");
        if ( system("$fullupdate -i $fullfile_rs_gz") != 0 )
        {
          print MAIL "Cannot update with $fullfile_rs_gz with $fullupdate.\n";
          close (MAIL);
          die "Cannot update with $fullfile_rs_gz with $fullupdate.";
        }
        if( system("$fullupdmother -i $fullfile_rs_gz -r $release -n F -m F -u T") != 0)
        {
           print MAIL "Cannot update datab with $fullfile_rs_gz with $fullupdmother.\n";
           close (MAIL);
           die "Cannot update data with $fullfile_rs_gz with $fullupdmother.";
        }
        print MAIL "update and mother successful with $fullfile_rs_gz\n";
        system("gzip $fullfile_rs_gz");
        $fullfile_rsbk = $fullfile_rs_gz.".gz";
        system("mv $fullfile_rsbk $fullaso");
     }else{
       print MAIL "Could not update with file $File_rs or $File_rs_gz!\n";
     }
  }

  # move log files into log directory
  opendir(DIR, "./");
  while($name = readdir(DIR)) {
     if($name =~ /$today/) { system("mv $name $fulllog"); }
  }
  closedir(DIR);

  print MAIL "Done.\n";
  close(MAIL);
# SK: Create success flag
  open TOUCH, ">/seqhound/build/flags/dupdcron.flg"; close TOUCH;
# SK: modified to start mybackup.pl
#  system("$fullbackup");

# $Log: dupdcron.pl,v $
# Revision 1.6  2004/04/06 15:53:27  vgu
# trun off FTP flag Passive
#
# Revision 1.1.1.6  2003/01/24 09:32:58  root
# Automated import from Sourceforge.net
#
# Revision 1.6  2003/01/23 20:06:25  kaca
# added binary mode command
#
# Revision 1.5  2002/11/09 13:21:42  kaca
# consolidating log file clean up; all logs are now moved away by the last cron isshoundoncron.pl
#
# Revision 1.4  2002/05/13 20:11:30  kaca
# changed FTP URL for NCBI
#
# Revision 1.3  2002/02/08 23:19:24  kaca
# removed backup step
#
# Revision 1.2  2002/01/17 17:35:21  kaca
# removed sendb executable
#
# Revision 1.1.1.2  2001/10/10 21:53:47  kaca
# initial import
#
# Revision 1.12  2001/06/13 14:19:39  kaca
# added backup
#
# Revision 1.10  2001/04/16 17:58:45  kaca
# more verbose
#
# Revision 1.6  2001/03/13 22:47:33  kaca
# added updates for RefSeq databases
#
# Revision 1.5  2001/02/23 20:42:25  kaca
# license change
#
# Revision 1.4  2001/02/19 20:34:43  kaca
# cosmetic change
#
# Revision 1.3  2000/10/19 16:11:18  kaca
# added CVS keywords and licenses
#
# Revision 1.2  2000/10/19 15:56:50  kaca
# minor changes
#
# Revision 1.1.1.1  2000/09/18 17:41:55  kaca
# Initial import
#
# Revision 1.1  2000/08/21 21:43:25  kaca
# initial import
#
