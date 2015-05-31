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
#  ftp of asn GenBank files
#
#
#******************************************************************************************

use Net::FTP;

  open(LOG, ">seqsync_ftp.log") || die "Cannot create seqsync_ftp.log\n";
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

  $NewCwd = "/ncbi-asn1/daily-nc/";
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

        if(/con_nc/ || (-s $File))
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

  #Since January 2004, NCBI no longer stores cumulative RefSeq File
  #in the directory. Need to go to refseq/release/complete to get the 
  #complete files complete*.bna.gz for initial build
  #download cumulative REFSEQ file from /refseq/cumulative directory
  #$NewCwd = "/refseq/cumulative";
  $NewCwd = "/refseq/cumulative/";
  if ($ftp->cwd($NewCwd) != 1)
  {
     die "Can't cwd to $NewCwd\n";
  }
  $Pwd = $ftp->pwd;
  print LOG "pwd= $Pwd\n";
  print "pwd= $Pwd\n";
  my $localfile = "rscu.aso.gz";
  $File = "rscu.bna.gz";
  $size = $ftp->size($File);
  print LOG "$File is $size bytes\n";
  if(($filename = $ftp->get($File, $localfile)) eq "")
  {
    print LOG "Can't get $File\n";
    print "Can't get $File\n";
  }
  else
  {
    print LOG "Got $File";
  }
  print LOG "closing connection";
  close (LIST);
  close (LOG);
  $ftp->quit;
