#! /usr/bin/perl


# $Id: dupdcron_odbc.pl,v 1.10 2005/04/21 18:48:10 zwang Exp $
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
  use shconfig;

  #things to customize....
  $path = shconfig->ReadIntrez( "pathupdates");
  #mail address to receive update report
  $admail = shconfig->ReadIntrez( "mail" );
  # in case database release is out of bounds (out of 100-200) use the default release
  $default_release = shconfig->ReadIntrez( "defaultrelease" );
  # flag file
  $flagfile = shconfig->ReadIntrez( "pathflags" ) . "dupd_complete.flg";
  # end of things to customize

  #set up absolute paths to executables, update files, log files and asofiles
  $fullupdate = $path."update";
  $fullupdmother = $path."mother";
  $fulliterateparti = $path."iterateparti";
  $fulllog = $path."logs";
  $fullaso = $path."asofiles";

  #email content
  $mail = "";

  # error messages
  $ERROR = "-- ERROR";
  $has_ftp_error = "FALSE";
  $has_file_error = "FALSE";
  $has_update_error = "FALSE";

  unlink($flagfile);

  #set release according to argument; attempt to check for its validity
  $release = @ARGV[0];
  if($release > 200)
   {
     $release = $default_release;
     $mail .= "WARNING: Release out of range, using default $default_release.\n";
   }
  if($release < 100)
   {
     $release = $default_release;
     $mail .= "WARNING: Release out of range, using default $default_release.\n";
   }

  #construct today's update files names
  $tm = localtime;
  $today =  sprintf ("%02d%02d", ($tm->mon)+1, ($tm->mday));
  $File = sprintf ("nc%02d%02d.aso.gz",($tm->mon)+1, ($tm->mday));
  $File_rs = sprintf ("rsnc.%02d%02d.%02d.bna.Z",($tm->mon)+1, ($tm->mday), ($tm->year)+1900);
  $File_rs_gz = sprintf ("rsnc.%02d%02d.%02d.bna.gz",($tm->mon)+1, ($tm->mday), ($tm->year)+1900);
  $File_tpa = "tpa_cu.bbs.gz"; 
  $File_release = "GB_Release_Number";
  $File_release_rs = "RS_Release_Number";

  #contruct the absolute path to the update files
  $fullfile = $path.$File;
  $fullfile_rs = $path.$File_rs;
  $fullfile_rs_gz = $path.$File_rs_gz;
  $fullfile_tpa = $path.$File_tpa;
  $fullfile_release = $path.$File_release;
  $fullfile_release_rs = $path.$File_release_rs;
  $fullfilebk = $fullfile;
  $fullfile_rsbk = $fullfile_rs;
  $fullfile_rsbk_gz = $fullfile_rs_gz;

  #if tpa should be updated
  $update_tpa = "FALSE";
  # if a new release is put up
  $is_new_release = "FALSE";
  $is_new_release_rs = "FALSE";

  $mail .= "SeqHound databases update for $today:\n";

  #ftp update files from NCBI
  if(!($ftp = Net::FTP->new("ftp.ncbi.nih.gov", Passive=>0)))
  {
    $mail .= "$ERROR: Cannot connect.\n";
    $has_ftp_error = "TRUE";
    send_email();
    open TOUCH, ">$flagfile"; close TOUCH;
  }
  if(!($ftp -> login("anonymous", $admail)))
  {
    $mail .= "$ERROR: Cannot login.\n";
    $has_ftp_error = "TRUE";
    send_email();
    open TOUCH, ">$flagfile"; close TOUCH;
  }
  if(!($ftp->cwd("ncbi-asn1/daily-nc")))
  {
    $mail .= "$ERROR: Cannot cwd to ncbi-asn1/daily-nc.\n";
    $has_ftp_error = "TRUE";
    send_email();
    open TOUCH, ">$flagfile"; close TOUCH;
  }

  ###########################################
  # get today's GenBank update file
  ###########################################
  if (!($ftp->binary))
  {
    $mail .= "$ERROR: Cannot change to binary.\n";
    $has_ftp_error = "TRUE";
    send_email();
    open TOUCH, ">$flagfile"; close TOUCH;
  }
  if(!($ftp->get($File, $fullfile)))
  {
    $mail .= "$ERROR: Cannot get $File.\n";
    $has_ftp_error = "TRUE";
    send_email();
    open TOUCH, ">$flagfile"; close TOUCH;
  } else {
    $mail .= "Got $File.\n";
  }
  ###########################################
  # get today's refseq update file
  ###########################################
  if(!($ftp->cwd("/refseq/daily")))
  {
    $mail .= "$ERROR: Cannot cwd to /refseq/daily.\n";
    $has_ftp_error = "TRUE";
    send_email();
    open TOUCH, ">$flagfile"; close TOUCH;
  }
  if (!($ftp->binary))
  {
    $mail .= "$ERROR: Cannot change to binary.\n";
    $has_ftp_error = "TRUE";
    send_email();
    open TOUCH, ">$flagfile"; close TOUCH;
  }
  if(!($ftp->get($File_rs_gz, $fullfile_rs_gz)))
  {
    $mail .= "$ERROR: Cannot get $File_rs_gz.\n";
    $has_ftp_error = "TRUE";
    send_email();
    open TOUCH, ">$flagfile"; close TOUCH;
  } else {
    $mail .= "Got $File_rs_gz.\n";
  }

  #####################################
  # get today's tpa release file
  # only if it's updated on the ftp site
  #####################################
  if(!($ftp->cwd("/tpa/release")))
  {
    $mail .= "$ERROR: Cannot cwd to /tpa/release.\n";
    $has_ftp_error = "TRUE";
    send_email();
    open TOUCH, ">$flagfile"; close TOUCH;
  }
  if ((!(-s $File_tpa)) || ($ftp->mdtm($File_tpa) > ($^T - 86400*(-M $fullfile_tpa))))
  {
    if (!($ftp->binary))
    {
      $mail .= "$ERROR: Cannot change to binary.\n";
      $has_ftp_error = "TRUE";
      send_email();
      open TOUCH, ">$flagfile"; close TOUCH;
    }
    if(!($ftp->get($File_tpa, $fullfile_tpa)))
    {
      $mail .= "$ERROR: Cannot get $File_tpa.\n";
      $has_ftp_error = "TRUE";
      send_email();
      open TOUCH, ">$flagfile"; close TOUCH;
    } else {
      $update_tpa = "TRUE";
      $mail .= "Got $File_tpa.\n";
    }
  }

  ###############################
  # for GenBank
  # get new release number
  # and files gbdel, gbnew, gbchg
  ###############################
  if(!($ftp->cwd("/ncbi-asn1")))
  {
    $mail .= "$ERROR: Cannot cwd to /ncbi-asn1.\n";
    $has_ftp_error = "TRUE";
    send_email();
    open TOUCH, ">$flagfile"; close TOUCH;
  }
  if ((!(-s $File_release)) || ($ftp->mdtm($File_release) > ($^T - 86400*(-M $fullfile_release))))
  {
    if(!($ftp->get($File_release, $fullfile_release)))
    {
      $mail .= "$ERROR: Cannot get $File_release.\n";
      $has_ftp_error = "TRUE";
      send_email();
      open TOUCH, ">$flagfile"; close TOUCH;
    } else {
      $is_new_release = "TRUE";
      $new_release_number = 0;
      $mail .= "\n**** New Release for GenBank: ";

      if (!open (RELEASE, "$fullfile_release"))
      {
         $mail .= "$ERROR: Cannot open $fullfile_release.\n";
         $has_file_error = "TRUE";
         send_email();
      } else {

         while (<RELEASE>)
         {
            chomp;
            $new_release_number = $_;
         }
         close RELEASE;
         $mail .= "$new_release_number **** \n";

         ######################
         # ftp files
         ######################
         $File_gbdel = "gbdel.txt.gz";
         $File_gbnew = "gbnew.txt.gz";
         $File_gbchg = "gbchg.txt.gz";
         $fullfile_gbdel = $path.$File_gbdel;
         $fullfile_gbnew = $path.$File_gbnew;
         $fullfile_gbchg = $path.$File_gbchg;
         $fullfile_gbdel_new = $path."gbdel".$new_release_number.".txt";
         $fullfile_gbnew_new = $path."gbnew".$new_release_number.".txt";
         $fullfile_gbchg_new = $path."gbchg".$new_release_number.".txt";
         @Files = ($File_gbdel, $File_gbnew, $File_gbchg);
         @fullfiles = ($fullfile_gbdel, $fullfile_gbnew, $fullfile_gbchg);
         @fullfiles_new = ($fullfile_gbdel_new, $fullfile_gbnew_new, $fullfile_gbchg_new);

         if (!($ftp->binary))
         {
             $mail .= "$ERROR: Cannot change to binary when ftp-ing gbdel, gbnew, gbchg.\n";
             $has_ftp_error = "TRUE";
             send_email();
         }
         for $i (0 .. 2) {
             if(!($ftp->get($Files[$i], $fullfiles[$i])))
             {
                 $mail .= " $ERROR: Cannot get $Files[$i].\n";
                 $has_ftp_error = "TRUE";
                 send_email();
             } else {
                 $mail .= " Got $Files[$i].\n";

                 system ("gunzip $fullfiles[$i]");
                 $fullfiles[$i]=~ s/.gz//;
                 system ("mv $fullfiles[$i] $fullfiles_new[$i]");
             }
         }
         $mail .= "\n";
      } # open release file
    } # get release file
  } # need to download new release file

  ###############################
  # for RefSeq
  # get new release number
  # and files Refseq-release*.catalog
  # and release*.removed-records 
  ###############################
  # get the release number in current Seqhound
  if (!(open (RELEASERS, "$fullfile_release_rs")))
  {
    $mail .= "$ERROR: Cannot open file $fullfile_release_rs\n";
    $has_file_error = "TRUE";
    send_email();
    open TOUCH, ">$flagfile"; close TOUCH;
  } 
  $release_number_rs = 0;
  while (<RELEASERS>)
  {
    chomp;
    $release_number_rs = $_;
  }
  close RELEASERS;
  $File_rs_catalog = "RefSeq-release" . $release_number_rs . ".catalog";
  $File_rs_removed = "release" . $release_number_rs . "removed-records";
  $fullfile_rs_catalog = $path . $File_rs_catalog;
  $fullfile_rs_removed = $path . $File_rs_removed;
  if(!($ftp->cwd("/refseq/release/release-catalog")))
  {
    $mail .= "$ERROR: Cannot cwd to /refseq/release/release-catalog/.\n";
    $has_ftp_error = "TRUE";
    send_email();
    open TOUCH, ">$flagfile"; close TOUCH;
  }
  # new release if unable to get files for the current release number
  if(!($ftp->get($File_rs_catalog, $fullfile_rs_catalog)) 
     && !($ftp->get($File_rs_removed, $fullfile_rs_removed)))
  {
    $is_new_release_rs = "TRUE";
    open TOUCH, ">$flagfile"; close TOUCH;
  }
  if ($is_new_release_rs eq "TRUE")
  {
    $new_release_number_rs = $release_number_rs+1;

    if (!open (RELEASERS, ">$fullfile_release_rs"))
    {
       $mail .= "$ERROR: Cannot open $fullfile_release_rs for write.\n";
       $has_file_error = "TRUE";
       send_email();
    } else {
       print RELEASERS "$new_release_number_rs\n";
    }
    close RELEASERS;
    $mail .= "\n**** New Release for RefSeq: $new_release_number_rs ****\n";

    ######################
    # ftp files
    ######################
    $File_rs_catalog = "RefSeq-release" . $new_release_number_rs . ".catalog";
    $File_rs_removed = "release" . $new_release_number_rs . ".removed-records";
    $fullfile_rs_catalog = $path . $File_rs_catalog;
    $fullfile_rs_removed = $path . $File_rs_removed;

    if (!($ftp->ascii))
    {
        $mail .= "$ERROR: Cannot change to ascii when ftp-ing Refseq catalog files.\n";
        $has_ftp_error = "TRUE";
        send_email();
    }
    if(!($ftp->get($File_rs_catalog, $fullfile_rs_catalog)))
    {
        $mail .= " $ERROR: Cannot get $File_rs_catalog.\n";
        $has_ftp_error = "TRUE";
        send_email();
    } else {
        $mail .= " Got $File_rs_catalog.\n";
    }
    if(!($ftp->get($File_rs_removed, $fullfile_rs_removed)))
    {
        $mail .= " $ERROR: Cannot get $File_rs_removed.\n";
        $has_ftp_error = "TRUE";
        send_email();
    } else {
        $mail .= " Got $File_rs_removed.\n";
    }
    $mail .= "\n";
  } # need to download new release file

  if(!($ftp->quit))
  {
    $mail .= "$ERROR: Cannot quit ftp.\n";
    $has_ftp_error = "TRUE";
    send_email();
    open TOUCH, ">$flagfile"; close TOUCH;
  }

  #######################################
  # update GenBank
  #######################################
  if(-s $fullfile)
  {
     system("gunzip $fullfile");
     $fullfile =~ s/.gz//;
     #run mother on asofile
     if(system("$fullupdmother -i $fullfile -r $release -n F -m F -u T") != 0)
     {
      $mail .= "$ERROR: Updating GenBank file failed. \n";
      $has_update_error = "TRUE";
      send_email();
     }
     $mail .= "Updating GenBank file succeeded.\n";
     # clean up, zip up files, put logs in logs dir and binaries into asofiles dir
     system("gzip $fullfile");
     system("mv $fullfilebk $fullaso");
  }
  else
  {
     $mail .= "$ERROR: No GenBank update file\n";
     $has_file_error = "TRUE";
     send_email();
  }

  #######################################
  # update RefSeq
  #######################################
  if(-s $fullfile_rs_gz)
  {
     system("gunzip $fullfile_rs_gz");
     $fullfile_rs_gz =~ s/.gz//;
     $temporal = $fullfile_rs_gz;
     $fullfile_rs_gz =~ s/rsnc./rsnc/;
     system("mv $temporal $fullfile_rs_gz");
     #run mother on refseq file
     if( system("$fullupdmother -i $fullfile_rs_gz -r $release -n F -m F -u T") != 0)
     {
        $mail .= "$ERROR: Updating RefSeq file failed.\n";
        $has_update_error = "TRUE";
        send_email();
     }
     $mail .= "Updating RefSeq file succeeded.\n";
     system("gzip $fullfile_rs_gz");
     $fullfile_rsbk = $fullfile_rs_gz.".gz";
     system("mv $fullfile_rsbk $fullaso");
  }else{
     $mail .= "$ERROR: No RefSeq update file\n";
     $has_file_error = "TRUE";
     send_email();
  }

  #############################################
  # check for tpa file for re-parsing,
  # need to remove the old data from DB
  #############################################
  if(($update_tpa eq "TRUE") && (-s $fullfile_tpa))
  {
     #######################
     # remove old data
     #######################
     if (system("$fulliterateparti -p tpa_cu -d T") != 0)
     {
      $mail .= "$ERROR: Cannot delete division tpa_cu\n";
      $has_update_error = "TRUE";
      send_email();
     }
     ######################
     # run mother on tpa file
     ######################
     system("gunzip $fullfile_tpa");
     $fullfile_tpa =~ s/.gz//;
     if(system("$fullupdmother -i $fullfile_tpa -r $release -n F -m F") != 0)
     {
      $mail .= "$ERROR: Updating TPA file failed.\n";
      $has_update_error = "TRUE";
      send_email();
     }
     $mail .= "Updating TPA file succeeded.\n";
     # clean up, zip up files, put logs in logs dir and binaries into asofiles dir
     system("gzip $fullfile_tpa");
     system("mv tpa_curun $fulllog");
  }
  else
  {
     $mail .= "$ERROR: No TPA update file\n";
     $has_file_error = "TRUE";
     send_email();
  }

  open TOUCH, ">$flagfile"; close TOUCH;


  ########################################
  # move log files into log directory
  ########################################
  $today = sprintf ("%02d%02d",($tm->mon)+1, ($tm->mday));
  opendir(DIR, "$path");
  while($name = readdir(DIR)) {
     if($name =~ /$today/) { system("mv $name $fulllog"); }
  }
  closedir(DIR);

  $mail .= "Done.\n";
  send_email();


###############################################
#
# a subroutine to send e-mail.
#
##############################################
sub send_email() {
  $subject = "Core update: ";

  if ($has_ftp_error eq "TRUE"
      || $has_file_error eq "TRUE"
      || $has_update_error eq "TRUE") 
  {
    $subject .= "Error: ";

    if ($has_ftp_error eq "TRUE") {
       $subject .= "ftp,";
    }
    if ($has_file_error eq "TRUE") {
       $subject .= "file,";
    }
    if ($has_update_error eq "TRUE") {
       $subject .= "update,";
    }
  }
  else
  {
    $subject .= "OK";
  }

  # set up send mail command
  $mymail = "| /usr/bin/mailx -s \"$subject\" " . $admail;

  #open mail message to log events
  open(MAIL, $mymail);
  print MAIL $mail;
  close MAIL;
  exit(0);
}



# $Log: dupdcron_odbc.pl,v $
# Revision 1.10  2005/04/21 18:48:10  zwang
# Removed unwanted printout.
#
# Revision 1.9  2005/04/21 18:00:00  zwang
# Changed for better error message handling.
#
# Revision 1.8  2005/04/18 19:12:31  zwang
# Added to check new release for Refseq and download files RefSeq-release*.catalog and release10.removed-records.
#
# Revision 1.7  2005/03/31 22:08:33  zwang
# Fixed minor bug in file name.
#
# Revision 1.6  2005/02/14 15:47:51  zwang
# Fixed minor bugs.
#
# Revision 1.5  2005/02/02 22:33:29  zwang
# No need to execute update since it is combined with mother.
# Download files gbdel, gbchg and gbnew for a new release.
# Update TPA file.
#
# Revision 1.4  2004/07/23 19:37:43  zwang
# Cleaned up code. Added $ and $ where missing.
#
# Revision 1.1  2004/07/23 16:12:46  zwang
# Scripts to update ODBC SeqHound.
#
#
