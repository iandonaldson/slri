#!/usr/bin/perl

# $Id: llcron.pl,v 1.2 2004/04/06 15:53:34 vgu Exp $
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
#          Katerina Michalickova and Christopher W.V. Hogue hogue@mshri.on.ca
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
#  makes new locus link tables, expects the gene_association files for 
#  GenBank and Swissprot in the "path" directory 
#
#******************************************************************************************

use Net::FTP;

  # things to customize
  # your e-mail
  $admail = "katerina\@mshri.on.ca";
  # update directory with the llparser and addgoid executables
  $path = "/seqhound/update/";
  #directory with the backup version of the locus link database
  $dbpath = "/seqhound/datab2/";
  # end of things to customize

  $mymail = "| mail ".$admail;
  $fullexec = $path."llparser";
  $logfile = "llparserlog";
  $fullexec2 = $path."addgoid";
  $logfile2 = "addgoidlog";
  $File = "LL_tmpl.gz";
  $fullfile = $path.$File;
  $lldatab = $dbpath."LL_*";
  $genbank = "gene_association.compugen.Genbank.gz";
  $swissprot = "gene_association.compugen.Swissprot.gz";
  $fullgenbank = $path.$genbank;
  $fullswisspr = $path.$swissprot;

  open(MAIL, $mymail);
  print MAIL "Locus link update.\n";
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
  if(!($ftp->cwd("refseq/LocusLink")))
  {
    print MAIL "Cannot cwd refseq/LocusLink.\n";
    close (MAIL);
    die "Cannot cwd to refseq/LocusLink.";
  }
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
  if(!($ftp->quit))
  {
    print MAIL "Cannot quit ftp.\n";
    close (MAIL);
    die "Cannot quit ftp.";
  }

  if(-s $fullfile)
  {
     #remove the databases
     system ("rm -f $lldatab");
     #gunzip the source file
     system ("gunzip $fullfile");
     $fullfile =~ s/.gz//;
     #run llparser
     if(system ("$fullexec -i $fullfile") == 0)
     {
       print MAIL "Locus Link parser OK.\n";
     }
     else
     {
       #if there is problem print out log file into e-mail
       print MAIL "Locus Link parser failed!\n";
       if(!open (LOG, $logfile) != 1)
       {
         print MAIL "Cannot open llparserlog.\n";
         close (MAIL);
         die "Cannot open llparserlog.\n";
       }
       while(<LOG>)
       {
         print MAIL $_;
       }
       close(LOG);
     }
     system ("gzip $fullfile");
  }
  else
  {
     print MAIL "Did not get the file $File.\n";
  }


  if(!($ftp = Net::FTP->new("ftp.geneontology.org", Passive=>0)))
  {
    print MAIL "Cannot connect to geneontology.\n";
    close (MAIL);
    die "Cannot connect.";
  }
  if(!($ftp -> login("anonymous", $admail)))
  {
    print MAIL "Cannot login to geneontology.\n";
    close (MAIL);
    die "Cannot login.";
  }
  if(!($ftp->cwd("/pub/go/gene-associations")))
  {
    print MAIL "Cannot cwd refseq/LocusLink.\n";
    close (MAIL);
    die "Cannot cwd to refseq/LocusLink.";
  }
  # check the date stamp on the remote file and download if the local file is older or missing
  if((!(-s $fullgenbank)) || ($ftp->mdtm($genbank) > ($^T - 86400*(-M $fullgenbank))))
  {

	  print MAIL "Downloading new Genbank association file.\n";
	  if(!($ftp->get($genbank, $fullgenbank)))
	  {
	    print MAIL "Cannot get $genbank";
	    close (MAIL);
	    die "Cannot get $genbank.";
	  }
  }
  if((!(-s $fullswisspr)) ||  ($ftp->mdtm($swissprot) > ($^T - 86400*(-M $fullswisspr))))
  {
 	  print MAIL "Downloading new Swissprot association file.\n";
	  if(!($ftp->get($swissprot, $fullswisspr)))
	  {
	    print MAIL "Cannot get $swissprot";
	    close (MAIL);
	    die "Cannot get $swissprot.";
	  }
  }
  if(!($ftp->quit))
  {
    print MAIL "Cannot quit ftp to geneontology.\n";
    close (MAIL);
    die "Cannot quit ftp.";
  }


  if(-s $fullgenbank)
  {
     #run addgoid
     system ("gunzip $fullgenbank");
     $fullgenbank =~ s/.gz//;
     if(system ("$fullexec2 -i $fullgenbank") == 0)
     {
       print MAIL "Addgoid parser OK.\n";
     }
     else
     {
       #if there is problem print out log file into e-mail
       print MAIL "Addgoid parser failed!\n";
       if(!open (LOG, $logfile2) != 1)
       {
         print MAIL "Cannot open addgoidlog.\n";
         close (MAIL);
         die "Cannot open addgoidlog.\n";
       }
       while(<LOG>)
       {
         print MAIL $_;
       }
       close(LOG);
     }
     system ("gzip $fullgenbank");
  }
  else
  {
     print MAIL "Did not get the file $genbank.\n";
  }

  if(-s $fullswisspr)
  {
     system ("gunzip $fullswisspr");
     $fullswisspr =~ s/.gz//;
     if(system ("$fullexec2 -i $fullswisspr") == 0)
     {
       print MAIL "Addgoid parser OK.\n";
     }
     else
     {
       #if there is problem print out log file into e-mail
       print MAIL "Addgoid parser failed!\n";
       if(!open (LOG, $logfile2) != 1)
       {
         print MAIL "Cannot open addgoidlog.\n";
         close (MAIL);
         die "Cannot open addgoidlog.\n";
       }
       while(<LOG>)
       {
         print MAIL $_;
       }
       close(LOG);
     }
     system ("gzip $fullswisspr");
  }
  else
  {
     print MAIL "Did not get the file $swissprot.\n";
  }

  print MAIL "Done\n";
  close(MAIL);



# $Log: llcron.pl,v $
# Revision 1.2  2004/04/06 15:53:34  vgu
# trun off FTP flag Passive
#
# Revision 1.1.1.11  2003/01/24 09:32:58  root
# Automated import from Sourceforge.net
#
# Revision 1.12  2003/01/23 20:06:29  kaca
# added binary mode command
#
# Revision 1.11  2002/11/09 13:21:42  kaca
# consolidating log file clean up; all logs are now moved away by the last cron isshoundoncron.pl
#
# Revision 1.10  2002/09/27 18:21:59  kaca
# removed write permission command, since all CGIs open tables read only it is no longer needed
#
# Revision 1.9  2002/09/26 16:13:47  kaca
# moved up log file clean up
#
# Revision 1.8  2002/08/09 21:23:17  kaca
# added date check for association files; remains untested
#
# Revision 1.7  2002/08/02 18:47:10  kaca
# gene association files are now downloaded compressed
#
# Revision 1.6  2002/07/04 15:08:55  kaca
# added FTP for gene-association files
#
# Revision 1.5  2002/07/03 22:18:35  kaca
# made the script less verbose
#
# Revision 1.4  2002/06/13 18:07:07  kaca
# fixed gene association files names
#
# Revision 1.2  2002/06/06 19:07:45  kaca
# permission change
#
# Revision 1.1  2002/06/06 18:58:06  kaca
# initial import of locus link tables update cron
#





