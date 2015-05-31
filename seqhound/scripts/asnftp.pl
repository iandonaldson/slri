#!/usr/bin/perl

# $Id: asnftp.pl,v 1.10 2005/04/19 18:34:06 zwang Exp $
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
#  ftp of asn GenBank files
#
#
#******************************************************************************************

use Net::FTP;

  open(LOG, ">asnftp.log") || die "Cannot create noget.log\n";
  #flush out the output to the files.
  select(LOG);
  $| = 1;

  $FtpHost = "ftp.ncbi.nih.gov";
  print LOG "Logging in.\n";

  #set flags according to arguments
  foreach (@ARGV)
  {
    if($_ eq "-noest")
    {
      print "set to no est mode.\n";
      $NO_EST = TRUE;
    }
  }

  $ftp = Net::FTP->new($FtpHost, Passive=>0);

  if ($ftp->login("anonymous","katerina\@mshri.on.ca") != 1)
  {
     die "Can't login to $FtpHost\n";
  }

  print LOG "Logged in.\n";

  if (($Pwd = $ftp->pwd) eq "")
  {
     die "Can't get current directory\n";
  }

  print "pwd=$Pwd\n";

  $NewCwd = "/ncbi-asn1";
  if ($ftp->cwd($NewCwd) != 1)
  {
     die "Can't cwd to $NewCwd\n";
  }
  $Pwd = $ftp->pwd;
  print LOG "pwd= $Pwd\n";

  #Get file gbdel.txt, gbnew.txt and gbchg.txt
  if ($ftp->ascii eq "")
  {
    die "Can't change type to ascii\n";
  }
  $File = "gbdel.txt";
  if (($filename = $ftp->get($File)) eq "")
  {
    print LOG "Cannot get $File\n";
  }
  else
  {
    print LOG "Got $filename.\n";
  }
  $File = "gbnew.txt";
  if (($filename = $ftp->get($File)) eq "")
  {
    print LOG "Cannot get $File\n";
  }
  else
  {
    print LOG "Got $filename.\n";
  }
  $File = "gbchg.txt";
  if (($filename = $ftp->get($File)) eq "")
  {
    print LOG "Cannot get $File\n";
  }
  else
  {
    print LOG "Got $filename.\n";
  }

  #Get a directory listing of $NewCwd.  Returns a
  #reference to a list of lines returned from the server.
  if (($list = $ftp->ls) eq "")
  {
      die "Nothing found in the directory $NewCwd\n";
  }

  #after getting a listing of files type each file name into a
  #file called list.
  if(open(LIST, ">asn.list") != 1)
  {
    print LOG "could not create asn.list file.\n";
  }

  print "Directory listing for $NewCwd:\n";
  for($item=0; $item <= $#{$list}; $item++)
  {
    print LIST "$list->[$item]\n";
  }
  close (LIST);

  if ($ftp->binary eq "")
  {
    die "Can't change type to binary\n";
  }

  print LOG "Changed to binary.\n";

  open(LIST, "asn.list") || die "Cannot open asn.list.log\n";

  while (<LIST>)
  {
     $line = $_;
     if(/.aso.gz/)
     {
        @words = split(/\s+/, $line);
        $File = $words[0];
        $size = $ftp->size($File);
        print LOG "$File is $size bytes\n";

        if(/con.aso.gz/)
        {
           print LOG "Skipping $File.\n";
        }
        else
        {

           if((/est/)&&($NO_EST eq 'TRUE'))
           {
              print LOG "Skipping $File.\n";
           }
           else
           {
              if (($filename = $ftp->get($File)) eq "")
              {
                 print LOG "Cannot get $File\n";
              }
              else
              {
                 print LOG "Got $filename.\n";

              }
           }
        }
     }
  }


  ################################################
  # ftp file in directory /tpa/release/
  ################################################
  $NewCwd = "/tpa/release";
  if ($ftp->cwd($NewCwd) != 1)
  {
     die "Can't cwd to $NewCwd\n";
  }
  $Pwd = $ftp->pwd;
  print LOG "pwd= $Pwd\n";
  #Get file tpa_cu.bbs.gz
  if ($ftp->binary eq "")
  {
    die "Can't change type to binary\n";
  }
  $File = "tpa_cu.bbs.gz";
  if (($filename = $ftp->get($File)) eq "")
  {
    print LOG "Cannot get $File\n";
  }
  else
  {
    print LOG "Got $filename.\n";
  }

  ########################################################################
  #Since January 2004, NCBI no longer stores cumulative RefSeq File
  #in the directory. Need to go to refseq/release/complete to get the 
  #complete files complete*.bna.gz for initial build
  ########################################################################
  $NewCwd = "/refseq/release/complete";
  if ($ftp->cwd($NewCwd) != 1)
  {
     die "Can't cwd to $NewCwd\n";
  }
  $Pwd = $ftp->pwd;
  print LOG "pwd= $Pwd\n";
  print "pwd= $Pwd\n";
 
  # List of complete*.bna.gz files 
  my $prefix = "rscu";

  foreach $FILE ($ftp->ls)
  {
    if($FILE =~ /^complete.*\.bna\.gz/s)
    {
      my $localfile = $prefix . $FILE;
      # Rename the file from rscucomplete*.bna.gz to rscucomplete*.aso.gz for seqhound_build.sh
      $localfile =~ s/bna/aso/;
      #VG and ZW: remove text string "complete" from file name as the
      # parser for divisions PARTI and SENGI handles only 10 chars
      # (Hardcoded !!!)
      $localfile =~ s/complete//;
      $localfile =~ s/NZ_AA//;
      print LOG "Getting file $FILE and save as $localfile\n";
      if (($ftp->get($FILE, $localfile)) eq "")
      {
         print LOG "Can not get file $FILE\n";
         print "Can not get file $FILE\n";
      }
    }
  } 


  ###########################################
  # ftp files in directory ncbi-asn1/wgs
  ##########################################
  $NewCwd = "/ncbi-asn1/wgs";
  if ($ftp->cwd($NewCwd) != 1)
  {
     die "Can't cwd to $NewCwd\n";
  }
  $Pwd = $ftp->pwd;
  print LOG "pwd= $Pwd\n";

  #Get a directory listing of $NewCwd.  Returns a
  #reference to a list of lines returned from the server.
  if (($list = $ftp->ls) eq "")
  {
      die "Nothing found in the directory $NewCwd\n";
  }

  #after getting a listing of files type each file name into a
  #file called list.
  if(open(LIST, ">wgs.list") != 1)
  {
    print LOG "could not create wgs.list file.\n";
  }

  print "Directory listing for $NewCwd:\n";
  for($item=0; $item <= $#{$list}; $item++)
  {
    print LIST "$list->[$item]\n";
  }
  close (LIST);

  if ($ftp->binary eq "")
  {
    die "Can't change type to binary\n";
  }

  print LOG "Changed to binary.\n";

  open(LIST, "wgs.list") || die "Cannot open wgs.list\n";
 
  while (<LIST>)
  {
     chomp;
     $line = $_;
     if($line =~ /.bbs.gz/ && $line =~ /wgs./)
     {
        $File = $line;
        $size = $ftp->size($File);
        print LOG "$File is $size bytes\n";

        if (($filename = $ftp->get($File)) eq "")
        {
            print LOG "Cannot get $File\n";
        }
        else
        {
            print LOG "Got $filename.\n";
        }
     }
  }


  ###########################################
  # ftp files in directory refseq/wgs
  ##########################################
  $NewCwd = "/refseq/wgs";
  if ($ftp->cwd($NewCwd) != 1)
  {
     die "Can't cwd to $NewCwd\n";
  }
  $Pwd = $ftp->pwd;
  print LOG "pwd= $Pwd\n";

  #Get a directory listing of $NewCwd.  Returns a
  #reference to a list of lines returned from the server.
  if (($list = $ftp->ls) eq "")
  {
      die "Nothing found in the directory $NewCwd\n";
  }

  #after getting a listing of files type each file name into a
  #file called list.
  if(open(LIST, ">wgs.list") != 1)
  {
    print LOG "could not create wgs.list file.\n";
  }

  print "Directory listing for $NewCwd:\n";
  for($item=0; $item <= $#{$list}; $item++)
  {
    print LIST "$list->[$item]\n";
  }
  close (LIST);

  if ($ftp->binary eq "")
  {
    die "Can't change type to binary\n";
  }

  print LOG "Changed to binary.\n";

  open(LIST, "wgs.list") || die "Cannot open wgs.list\n";
 
  while (<LIST>)
  {
     chomp;
     $line = $_;
     if($line =~ /.bna.gz/ && $line =~ /rswgs./)
     {
        $File = $line;
        $size = $ftp->size($File);
        print LOG "$File is $size bytes\n";

        if (($filename = $ftp->get($File)) eq "")
        {
            print LOG "Cannot get $File\n";
        }
        else
        {
            print LOG "Got $filename.\n";
        }
     }
  }
  close (LIST);


  ############################################
  # download catalog files for refseq
  ###########################################
  $NewCwd = "/refseq/release/release-catalog";
  if ($ftp->cwd($NewCwd) != 1)
  {
     die "Can't cwd to $NewCwd\n";
  }
  $Pwd = $ftp->pwd;
  print LOG "pwd= $Pwd\n";
  print "pwd= $Pwd\n";
  $release_number_rs = 0; 
  foreach $FILE ($ftp->ls)
  {
    if($FILE =~ /RefSeq-release/)
    {
      @words = split(/\./, $FILE); 
      $release_number_rs = substr $words[0], 14;  
    } 
  }
  $catalog_rs = "RefSeq-release" . $release_number_rs . ".catalog";
  $removed_rs = "release" . $release_number_rs . ".removed-records";
  @rs_catalogs = ($catalog_rs, $removed_rs);
  if ($ftp->ascii eq "")
  {
    die "Can't change type to ascii\n";
  }
  foreach $FILE (@rs_catalogs)
  {
    if (!($ftp->get($FILE)))
    {
       print LOG "Cannot get $FILE\n";
    }
    else
    {
       print LOG "Got $FILE.\n";
    }
  }


  ##################################################
  # create files for release number.
  # file GB_Release_Number is ftp-ed from /ncbi-asn1
  # file RS_Release_Number is created
  ################################################# 
  $NewCwd = "/ncbi-asn1";
  if ($ftp->cwd($NewCwd) != 1)
  {
     die "Can't cwd to $NewCwd\n";
  }
  if ($ftp->ascii eq "")
  {
    die "Can't change type to ascii\n";
  }
  $FILE = "GB_Release_Number";
  if (!($ftp->get("GB_Release_Number")))
  {
     print LOG "Cannot get $FILE\n";
  }
  else
  {
     print LOG "Got $FILE.\n";
  }
  open(RELEASERS, ">RS_Release_Number") or die "Cannot open file RS_Release_Number for write\n";
  print RELEASERS "$release_number_rs\n";
  close RELEASERS;

  close (LOG);

  print LOG "closing connection";
  $ftp->quit;


# $Log: asnftp.pl,v $
# Revision 1.10  2005/04/19 18:34:06  zwang
# Ftp files RefSeq-release*.catalog and release*.removed-records.
# Generate a file call RS_Release_Number to hold the release number of RefSeq.
# Ftp file GB_Release_Number.
#
# Revision 1.9  2005/04/18 19:43:26  zwang
# Ftp wgs files from refseq/wgs.
#
# Revision 1.8  2005/02/11 15:59:59  zwang
# Fixed minor error.
#
# Revision 1.7  2005/01/31 19:58:37  zwang
# Add to download tpa and wgs files.
#
# Revision 1.6  2004/11/17 15:55:58  zwang
# Added to download files gbdel.txt, gbnew.txt and gbchg.txt from ncbi-asn1.
# Removed lines that are commented out.
#
# Revision 1.5  2004/05/04 20:26:19  vgu
# Fix typo in creating FTP session statement
#
# Revision 1.4  2004/04/21 15:35:17  vgu
# rename refseq files as seqhound parser hardcoded 10 chars in divsion name
#
# Revision 1.3  2004/03/24 19:34:47  zwang
# Rename refseq file to rscucomplete*.aso.gz
#
# Revision 1.2  2004/03/05 15:28:27  vgu
# Changed FTP directory to /refseq/release/complete for all RefSeq files needed for initial SeqHound build
#
# Revision 1.1.1.2  2002/05/14 08:27:37  root
# Automated import from Sourceforge.net
#
# Revision 1.2  2002/05/13 20:11:30  kaca
# changed FTP URL for NCBI
#
# Revision 1.1.1.1  2001/05/25 21:36:21  kaca
# initial import
#
# Revision 1.8  2001/03/14 16:57:49  kaca
# cosmetic changes
#
# Revision 1.7  2001/02/23 20:42:25  kaca
# license change
#
# Revision 1.6  2000/10/19 20:04:56  kaca
# download the right extention files
#
# Revision 1.5  2000/10/19 16:46:48  kaca
# minor fix
#
# Revision 1.4  2000/10/19 16:11:18  kaca
# added CVS keywords and licenses
#

