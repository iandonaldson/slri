#!/usr/bin/perl

# $Id: comgencron.pl,v 1.3 2004/04/06 15:53:10 vgu Exp $
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
#  updates complete genomes in TAXGI and human and mouse complete genomes content
#
#  first ARGUMENT specifies if the human and mouse complete genomes should be reimported in
#  the system
#
#
#******************************************************************************************
use Net::FTP;


  # things to customize
  # your e-mail
  $admail = "katerina\@mshri.on.ca";
  # update directory with the comgen, genftp.pl and humouse_build.sh executables
  $path = "/home2/kaca/test2/";
  $default_release = 131;
  # end of things to customize
  # the $path directory has to contain mother, comgen, iterateparti, humoasn.pl

  $mymail = "| mail ".$admail;
  $fullexec = $path."comgen";
  $fulliter = $path."iterateparti -p gbchm -d T";
  $fullasn = $path . "humoasn.pl";
  $fullmother = $path."mother";
  $logfile = "comgenlog";
  $templist = $path . "dirlist";

  # compressing and uncompressing various files
  $fullzip = $path . "*.gz";
  $fullzip_ch = $path . "*_chr*.asn";
  $fullzip_r = $path . "rna_*.asn";
  $fullzip_p = $path . "protein_*.asn";
  
  $update_humouse = 0;
  $newrh = 0;
  $newrm = 0,
  $newph = 0;
  $newpm = 0;

  # these variables deal with non-unique naming of rna and protein files for human and mouse
  $fullrna = $path . "rna.asn.gz";
  $fullprotein = $path . "protein.asn.gz";
  $fullrna_h = $path ."rna_h.asn.gz";
  $fullrna_m = $path . "rna_m.asn.gz";
  $fullprotein_h = $path . "protein_h.asn.gz";
  $fullprotein_m = $path . "protein_m.asn.gz";



  open(MAIL, $mymail);

  $FtpHost = "ftp.ncbi.nih.gov";

  $ftp = Net::FTP->new($FtpHost, Passive=>0);

  if ($ftp->login("anonymous","katerina\@mshri.on.ca") != 1)
  {
     print MAIL "Cannot login to $FtpHost\n";
  }

  if (($Pwd = $ftp->pwd) eq "")
  {
     print MAIL "Cannot get current directory\n";
  }

  $NewCwd = "/genomes";
  if ($ftp->cwd($NewCwd) != 1)
  {
      print MAIL "Cannot cwd to $NewCwd\n";
  }
  $Pwd = $ftp->pwd;

  if (($list = $ftp->ls) eq "")
  {
      print MAIL "Nothing found in the directory $NewCwd\n";
  }
  for($item=0; $item <= $#{$list}; $item++)
  {
     $line = $list->[$item];
     $subdir = $NewCwd . "/" . $line;
     if($subdir =~ /Bacteria.OLD/)
     {
	next;
     }
     if ($ftp->cwd($subdir) != 1)
     {
	next;
     }
     if (($sublist = $ftp->ls) eq "")
     {
       print MAIL "Cannot get directory listings for $subdir.\n";
       next;
     }
     for($subitem=0; $subitem <= $#{$sublist}; $subitem++)
     {
         $subline = $sublist->[$subitem];
         if($subline =~ /.asn/)
	 {
	    if ((!(-s $fullpath)) || ($ftp->mdtm($subline) > ($^T - 86400*(-M $fullpath))))
	    {    
             if($subline =~ /.asn.gz/)
             {
                $ftp->binary;
              }
              $fullpath = $path.$subline;
	      if (($filename = $ftp->get($subline, $fullpath)) eq "")
              {
                print MAIL "Cannot get $subline\n";
              }
              else
              {
	        print MAIL "Got $subline.\n";
              }
	    }
	 }
	 else
	 {
	    #get the list and descend one level lower looking for asn extensions
            $botdir = $subdir . "/" . $subline;
            if ($ftp->cwd($botdir) != 1)
	    {
	      print MAIL "Cannot cwd to $botdir\n";
	      next;
	    }
            if (($botlist = $ftp->ls) eq "")
            {
               print MAIL "Cannot get directory listings for $botdir.\n";
               next;
            }
            for($botitem=0; $botitem <= $#{$botlist}; $botitem++)
            {
	       $botline = $botlist->[$botitem];
	       if($botline =~ /.asn/)
	       {
                 $fullpath = $path.$botline;
                 
                 # check for rna.asn and protein.asn files; which have non-unigue names 
                 if (($botline =~ /rna.asn.gz/) && ($subdir =~ /H_sapiens/))
                 {
                     if ((!(-s $fullrna_h)) ||  ($ftp->mdtm($botline) > ($^T - 86400*(-M $fullrna_h))))
                     {
                         $ftp->binary;  
                         if(($filename = $ftp->get($botline, $fullrna_h)) eq "")
                         {
                            print MAIL "Cannot get $botline or $fullrna_h\n";
                         }
                         else
                         {
                            print MAIL "Got $botline, $fullrna_h\n";
                            $update_humouse = 1;
                            $newrh = 1;
                         }
                     } 
                 }
                 elsif (($botline =~ /rna.asn.gz/) && ($subdir =~ /M_musculus/))
                 {
                   if ((!(-s $fullrna_m)) ||  ($ftp->mdtm($botline) > ($^T - 86400*(-M $fullrna_m))))
                   {
                     $ftp->binary;
                     if(($filename = $ftp->get($botline, $fullrna_m)) eq "")
                     {
                       print MAIL "Cannot get $botline, $fullrna_m\n";
                     }
                     else
                     {
                        print MAIL "Got $botline or $fullrna_m.\n";
                        $update_humouse = 1;
                        $newrm = 1;

                     }
                   }
                 }
                 elsif (($botline =~ /rna.asn.gz/) && ($subdir =~ /R_norvegicus/))
                 {
                   if ((!(-s $fullrna_r)) ||  ($ftp->mdtm($botline) > ($^T - 86400*(-M $fullrna_r))))
                   {
                     $ftp->binary;
                     if(($filename = $ftp->get($botline, $fullrna_r)) eq "")
                     {
                       print MAIL "Cannot get $botline, $fullrna_r\n";
                     }
                     else
                     {
                        print MAIL "Got $botline or $fullrna_r.\n";
                        $update_humouse = 1;
                        $newrr = 1;

                     }
                   }
                 }

                 elsif (($botline =~ /protein.asn.gz/) && ($subdir =~ /H_sapiens/))
                 {
                   if ((!(-s $fullprotein_h)) ||  ($ftp->mdtm($botline) > ($^T - 86400*(-M $fullprotein_h))))
                   {
                    $ftp->binary;    
                    if(($filename = $ftp->get($botline, $fullprotein_h)) eq "")
                     {
                        print MAIL "Cannot get $botline or $fullprotein_h\n";
                     }
                     else
                     {
                        print MAIL "Got $botline or $fullprotein_h.\n";
                        $update_humouse = 1;
                        $newph = 1;
                     }
                   }
                 }
                 elsif (($botline =~ /protein.asn.gz/) && ($subdir =~ /M_musculus/))
                 {
                   if ((!(-s $fullprotein_m)) || (($ftp->mdtm($botline) > ($^T - 86400*(-M $fullprotein_m)))))
                   {
                     $ftp->binary;
                     if(($filename = $ftp->get($botline, $fullprotein_m)) eq "")
                     {
                        print MAIL "Cannot get $botline or $fullprotein_m\n";
                     }
                     else
                     {
                        print MAIL "Got $botline or $fullprotein_m.\n";
                        $update_humouse = 1;
                        $newpm = 0;
                     }
                   }
                 }
                 elsif (($botline =~ /protein.asn.gz/) && ($subdir =~ /R_norvegicus/))
                 {
                   if ((!(-s $fullprotein_r)) || (($ftp->mdtm($botline) > ($^T - 86400*(-M $fullprotein_r)))))
                   {
                     $ftp->binary;
                     if(($filename = $ftp->get($botline, $fullprotein_r)) eq "")
                     {
                        print MAIL "Cannot get $botline or $fullprotein_r\n";
                     }
                     else
                     {
                        print MAIL "Got $botline or $fullprotein_r.\n";
                        $update_humouse = 1;
                        $newpr = 0;
                     }
                   }
                 }

                elsif ((!(-s $fullpath)) || (($ftp->mdtm($botline) > ($^T - 86400*(-M $fullpath)))))
		 {
                   $ftp->binary;
		   if(($filename = $ftp->get($botline, $fullpath)) eq "")
                   {
                     print MAIL "Cannot get $botline\n";
                   }
                   else
                   {
	              print MAIL "Got $botline.\n";
		      # flip a flag if there was an update to human or mouse chromosomes
		      if($botline =~ /.*_chr.*\.asn/)
		      {
		        $update_humouse = 1;
		      }
                   }
		 }
	       }
            }
	 }
     }
  }

  $ftp->quit;


  # gunzip anything compressed
  system("gunzip $fullzip");

  # if there were any updates on rna or protein files; run humoasn.pl 
  if($newrh == 1)
  {
    $fullrna_h =~ s/.gz//;
    system("$fullasn $fullrna_h"); 
  }
  if($newrm == 1)
  {
    $fullrna_m =~ s/.gz//;
    system("$fullasn $fullrna_m");
  }
  if($newrr == 1)
  {
    $fullrna_r =~ s/.gz//;
    system("$fullasn $fullrna_r");
  }

  if($newph == 1)
  {
    $fullprotein_h =~ s/.gz//;
    system("$fullasn $fullprotein_h");
  }
  if($newpm == 1)
  {
    $fullprotein_m =~ s/.gz//;
    system("$fullasn $fullprotein_m");
  }
  if($newpr == 1)
  {
    $fullprotein_r =~ s/.gz//;
    system("$fullasn $fullprotein_r");
  }

  

   #if the first argument is TRUE and there was an update to human or mouse files
   # run iterateparti and mother to add human and mouse into SeqHound freshly
   if ((@ARGV[0] eq "TRUE") && ($update_humouse == 1))
   {
     # this deletes sequences belonging to gbchm partition from the system
     if(system("$fulliter") != 0)
     {
        print MAIL "Iterparti failed. Cannot delete gbchm sequences from the system\n";
	close (MAIL);
	die "Iterparti failed. Cannot delete gbchm sequences from the system\n";
     }
     else
     {
        print MAIL "Iterparti OK.\n";
     }
     #this will add the human and mouse complete genomes back in the system
     #(similar functionality as humouse_build.sh)
     #it ASSUMES that the gbchm partition already exists
     system("ls -1 $path > $templist");
     if(!(open(LIST, $templist)))
     {
        print MAIL "Cannot open $templist directory listing.\n";
        close (MAIL);
        die "Cannot open $templist directory listing.\n";
     }
     while (<LIST>)
     {
        chop($_);
	if((/.*_chr.*\.asn/) || (/rna_.{1}\.asn/) || (/protein_.{1}\.asn/))
	{
           $infile = $path . $_;
	   if(system("$fullmother -i $infile -r $default_release -n F -m F -c T -t T") != 0)
	   {
	     print MAIL "Mother parser failed on $_.\n";
	     close (MAIL);
	     die "Mother parser failed on $_.\n";
	   }
           else
           {
             print MAIL "Mother OK on $_.\n";
           }
	}
     }
     close(LIST);
     system("rm -f $templist");
   }

   # this part executes comgen over all complete genome files
   if(system ("$fullexec -l T -p $path") == 0)
   {
     print MAIL "Comgen OK.\n";
   }
   else
   {
     #if there is problem print out log file into e-mail
     print MAIL "Comgen failed!\n";
     if(!open (LOG, $logfile) != 1)
     {
       print MAIL "Cannot open comgenlog.\n";
       close (MAIL);
       die "Cannot open comgenlog.\n";
     }
     while(<LOG>)
     {
       print MAIL $_;
     }
     close(LOG);
  }
 

  # zip up everything that was zipped originally
  # otherwise the FTP date comparisons will not work
  system ("gzip $fullzip_ch");
  system ("gzip $fullzip_r");
  system ("gzip $fullzip_p");
  close(MAIL);



# $Log: comgencron.pl,v $
# Revision 1.3  2004/04/06 15:53:10  vgu
# trun off FTP flag Passive
#
# Revision 1.2  2003/09/25 20:40:41  kaca
# added processing of rat data
#
# Revision 1.1.1.8  2003/01/24 09:32:57  root
# Automated import from Sourceforge.net
#
# Revision 1.10  2003/01/23 20:06:24  kaca
# added binary mode command
#
# Revision 1.9  2002/11/11 03:54:35  kaca
# many small fixes
#
# Revision 1.8  2002/11/09 13:21:42  kaca
# consolidating log file clean up; all logs are now moved away by the last cron isshoundoncron.pl
#
# Revision 1.7  2002/11/07 13:38:54  kaca
# added full paths
#
# Revision 1.6  2002/11/06 22:19:00  kaca
# attempted to resolve file date checking for human and mouse; still needs work
#
# Revision 1.4  2002/11/01 22:45:54  kaca
# added switch for adding human and mouse into the system
#
# Revision 1.2  2002/07/04 15:09:40  kaca
# added datafiles cleanup
#
# Revision 1.1  2002/07/03 22:30:07  kaca
# initial import of comgen cron
#



