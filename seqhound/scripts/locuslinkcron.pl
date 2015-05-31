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
  $path = "/hibbert-local/shoundtest/5.lldb.files/";
  #$path = "/skinner-local/shoundtest/5.lldb.files/";
  #directory with the backup version of the LocusLink tables
  $dbpath = "/hibbert-local/shoundtest/datab/";
  #$dbpath = "/skinner-local/shoundtest/datab/";

  $mymail = "| /usr/bin/mailx -s \"LocusLink cron status\" " . $admail;
  $fullexec = $path."llparser";
  $logfile = "locuslinklog";
  $File = "LL_tmpl.gz";
  $fullfile = $path.$File;
  @lldblist = ("LL_CDD.*", "LL_GO.*", "LL_LLINK.*", "LL_OMIM.*");
  #Make sure parser llparser is in the path
  my $llparser = $path."llparser";
  if (!(-s $llparser)) 
  {
    die "Parser $llparser not found.\n";
  }
  my $llgoparser = $path."llgoa";
  if (!(-s $llgoparser)) 
  {
    die "Parser $llgoparser not found.\n";
  }
  # Verify presence of the locus link go annotation parser
  my $llgoa = $path."llgoa";
  if (!(-s $llgoa)) 
  {
    die "Parser $llgoa not found.\n";
  }

  unlink("/seqhound/build/flags/locuslinkcron.flg");
  #unlink("/skinner-local/shoundtest/5.lldb.files/locuslinkcron.flg");

  open(MAIL, $mymail);
  print MAIL "LocusLink update.\n";
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
  if(!($ftp->cwd("/refseq/LocusLink/")))
  {
    print MAIL "Cannot cwd refseq/LocusLink.\n";
    close (MAIL);
    die "Cannot cwd to refseq/LocusLink.";
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
    print MAIL "File LL_tmpl.gz up-to-date.\n";
  }
  if(!($ftp->quit))
  {
    print MAIL "Cannot quit ftp.\n";
    close (MAIL);
    die "Cannot quit ftp.";
  }
  if((-s $fullfile) && ($parser_run eq "TRUE"))
  {
     # backup existing locuslink tables
     my $backupdir = $dbpath."old_locuslink/";
     system("mkdir -p $backupdir");
     foreach $item (@lldblist)
     {
       my $tableEntry = $dbpath.$item;
       system ("mv $tableEntry $backupdir");
     }
     
     #uncompress the source file
     system ("gunzip -f $fullfile");
     #extract from tar ball
     $fullfile =~ s/.gz//;
     #run importtaxdb parser
     if(system ("$fullexec ") == 0)
     {
       print MAIL "New LocusLink tables built successfully.\n";
       #Copy new tables to /seqhound/build
       #my $shbuild = "/seqhound/build/datab/";
       #foreach $item (@lldblist)
       #{
       #  my $newtable = $dbpath.$item;
       #  system ("cp $newtable $shbuild");
       #}
     }
     else
     {
       # If rebuild fails, restore old tables 
       my $oldlldblist = $backupdir."*";
       system ("cp $oldlldblist $dbpath");

       #Print out log file into e-mail
       print MAIL "Rebuild LocusLink failed!\n";
       if(!open (LOG, $logfile) != 1)
       {
         print MAIL "Cannot open locuslinklog.\n";
         close (MAIL);
         die "Cannot open locuslinklog.\n";
       }
       while(<LOG>)
       {
         print MAIL $_;
       }
       close(LOG);
     }
     system ("gzip $fullfile");

  # After each rebuild of the locus link tables, run parser
  # llgoa to import GO annotation data
  my $gigosrc = "/home/rcavero/";
  my $gigozip = "gigo.zip";
  my $goDataDump = $gigosrc . $gigozip;
  # Get gi go text dump 
  if (system("cp $goDataDump .") == 0)
  {
    # Ensure text dump file exists locally
    if (-s $gigozip)
    {
      #if (system ("gunzip $gigozip") == 0)
      if (system ("unzip $gigozip") == 0)
      {
        # Run the parser llgoa
        if (system ("./llgoa") == 0)
        {
          print MAIL "GI-GO data import to locuslink successful.\n";
          #Copy locus link tables to production build
          my $llsrc = $dbpath."LL*";
          my $lldest = "/seqhound/build/datab/";
          if (system ("cp $llsrc $lldest") == 0)
          {
            print MAIL "LocusLink UPDATE OK.\n";
          }
          else
          {
            print MAIL "Copy job to seqhound production build
failed.\n";
          }
        }
        else
        {
          print MAIL "GI-GO data import to locuslink failed.\n";
        }
        #Remove the text dump files
        system ("rm *gigo*.txt");

      }
      else
      {
          print MAIL "Cannot gunzip file $gigozip.\n";
      }
  
    }
    else
    {
          print MAIL "File $gigozip not found.\n";
    }
  }
  else
  {
    print MAIL "Unable to get file $goDataDump.\n";
  }
}
  
 
  
  # Create the flag anyway as seqhound_cron needs to see the flag to
  # start backup scripts
  open TOUCH, ">/seqhound/build/flags/locuslinkcron.flg"; close TOUCH;
  #open TOUCH, ">/skinner-local/shoundtest/5.lldb.files/locuslinkcron.flg"; close
TOUCH;
  print MAIL "Done\n";
  close(MAIL);

