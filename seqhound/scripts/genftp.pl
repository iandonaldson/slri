#!/usr/bin/perl

# $Id: genftp.pl,v 1.3 2004/04/06 15:55:27 vgu Exp $
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
#           Katerina Michalickova, Ruth Isserlin and Christopher W.V. Hogue hogue@mshri.on.ca
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
#  FTP all complete genome ASN.1 files available in /genomes subdirectories
#
#  first ARGUMENT is path to directory where the download files go but it is optional
#
#
#******************************************************************************************

use Net::FTP;
  $path = @ARGV[0];
  if($path eq "")
  {
    $path = "./";
  }
  $FtpHost = "ftp.ncbi.nih.gov";
  print "Logging in.\n";

  $ftp = Net::FTP->new($FtpHost, Passive=>0);

  if ($ftp->login("anonymous","katerina\@mshri.on.ca") != 1)
  {
     die "Cannot login to $FtpHost\n";
  }
  print "Logged in.\n";

  if (($Pwd = $ftp->pwd) eq "")
  {
     die "Cannot get current directory\n";
  }
  print "pwd=$Pwd\n";

  $NewCwd = "/genomes";
  if ($ftp->cwd($NewCwd) != 1)
  {
     die "Cannot cwd to $NewCwd\n";
  }
  $Pwd = $ftp->pwd;
  print "pwd= $Pwd\n";

  if (($list = $ftp->ls) eq "")
  {
      die "Nothing found in the directory $NewCwd\n";
  }
  for($item=0; $item <= $#{$list}; $item++)
  {
     $line = $list->[$item];
     print "\n\n$line\n";
     $subdir = $NewCwd . "/" . $line;
     if($subdir =~ /Bacteria.OLD/)
     {
        print"Skipping Bacteria.OLD.\n";
	next;
     }
     if ($ftp->cwd($subdir) != 1)
     {
        print "Cannot cwd to $subdir\n";
	next;
     }
     if (($sublist = $ftp->ls) eq "")
     {
       print "Cannot get directory listings for $subdir.\n";
       next;
     }
     for($subitem=0; $subitem <= $#{$sublist}; $subitem++)
     {
         $subline = $sublist->[$subitem];
	 print "$subline\n";
         if($subline =~ /.asn/)
	 {
	    $fullpath = $path.$subline;
	    if ((!(-s $subline)) || ($ftp->mdtm($subline) > ($^T - 86400*(-M $fullpath))))
	    {
              if($subline =~ /.asn.gz/)
              {
                $ftp->binary;
              }
	      if (($filename = $ftp->get($subline, $fullpath)) eq "")
              {
                print "Cannot get $subline\n";
              }
              else
              {
	        print "Got $subline.\n";
              }
	    }
	 }
	 else
	 {
	    #get the list and descend one level lower looking for asn extensions
            $botdir = $subdir . "/" . $subline;
            if ($ftp->cwd($botdir) != 1)
	    {
	      print "Cannot cwd to $botdir\n";
	      next;
	    }
            if (($botlist = $ftp->ls) eq "")
            {
               print "Cannot get directory listings for $botdir.\n";
               next;
            }
            for($botitem=0; $botitem <= $#{$botlist}; $botitem++)
            {
	       $botline = $botlist->[$botitem];
	       if($botline =~ /.asn/)
	       {
                 $fullpath = $path.$botline;
                 if ((!(-s $botline)) ||  ($ftp->mdtm($botline) > ($^T - 86400*(-M $fullpath))))
		 {
                   if($botline =~ /.asn.gz/)
                   {
                      $ftp->binary;
                   }
		   if(($filename = $ftp->get($botline, $fullpath)) eq "")
                   {
                     print "Cannot get $botline\n";
                   }
                   else
                   {
	              print "Got $botline.\n";
                   }
		 }

		 #take care of protein.aso and rna.aso files which have non-unique names
		 if (($botline =~ /rna.asn.gz/) && ($subdir =~ /H_sapiens/))
		 {
		   system("mv rna.asn.gz rna_h.asn.gz");
		 }
		 if (($botline =~ /rna.asn.gz/) && ($subdir =~ /M_musculus/))
		 {
		   system("mv rna.asn.gz rna_m.asn.gz");
		 }
             if (($botline =~ /rna.asn.gz/) && ($subdir =~ /R_norvegicus/))
		 {
		   system("mv rna.asn.gz rna_r.asn.gz");
		 }
             if (($botline =~ /protein.asn.gz/) && ($subdir =~ /H_sapiens/))
		 {
		   system("mv protein.asn.gz protein_h.asn.gz");
		 }
		 if (($botline =~ /protein.asn.gz/) && ($subdir =~ /M_musculus/))
		 {
		   system("mv protein.asn.gz protein_m.asn.gz");
		 }
             if (($botline =~ /protein.asn.gz/) && ($subdir =~ /R_norvegicus/))
		 {
		   system("mv protein.asn.gz protein_r.asn.gz");
		 }
	       }
            }
	 }
     }
  }

  print "Closing connection\n";
  $ftp->quit;

  system("gunzip *.gz");
  system("./humoasn.pl protein_h.asn");
  system("./humoasn.pl protein_m.asn");
  system("./humoasn.pl protein_r.asn");
  system("./humoasn.pl rna_h.asn");
  system("./humoasn.pl rna_m.asn");
  system("./humoasn.pl rna_r.asn");







# $Log: genftp.pl,v $
# Revision 1.3  2004/04/06 15:55:27  vgu
# trun off FTP flag Passive
#
# Revision 1.2  2003/09/25 20:40:41  kaca
# added processing of rat data
#
# Revision 1.1.1.7  2003/01/24 09:32:58  root
# Automated import from Sourceforge.net
#
# Revision 1.8  2003/01/23 20:06:25  kaca
# added binary mode command
#
# Revision 1.7  2002/11/07 13:39:30  kaca
# reflecting changes in humoasn.pl
#
# Revision 1.6  2002/10/31 22:28:23  kaca
# added argument directy where the download files go
#
# Revision 1.5  2002/10/31 22:02:29  kaca
# added processing of rna and protein files
#
# Revision 1.4  2002/10/29 22:39:50  kaca
# dealing with files of the same name
#
# Revision 1.3  2002/10/24 21:38:16  kaca
# script now downloads files for human and mouse
#
# Revision 1.2  2002/10/18 20:31:19  kaca
# the script now avoids Bacteria.old directory; data files from there were overwriting the fresh ones
#
# Revision 1.1  2002/10/11 22:03:54  kaca
# initial import of complete genome files import script; still a prototype
#


