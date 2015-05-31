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
#          Victor Gu 
#
#
#  REVISION/CONTRIBUTION NOTES:
#
#  April 7,2005  Original MSH Revision.
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
#  SeqHound download and installation script
#
#
#******************************************************************************************
############
# The script needs two command line parameters to run:
# The first one is the platform (valid parameter is either "linux" or "solaris")
# The second one is the instllation directory (e.g. /export/home/seqhound/exec)
use Net::FTP;
use File::Copy;
$tarlinux="seqhound3.02.linux_x86_32bin.tar.gz";
$tarsolaris="seqhound3.02.sparc_64bin.tar.gz";
$tarfile;

sub downloadshound
{
  my $ftphost = "ftp.blueprint.org";
  my $bindir = "pub/SeqHound/Execs";
  if ($ARGV[0] eq "linux")
  {
    $tarfile = $tarlinux;
  }
  else
  {
    $tarfile = $tarsolaris;
  }

  print "Contacting FTP host $hostname\n";
  $ftpsession = Net::FTP->new($ftphost);
  if ($ftpsession->login("anonymous", "vgu\@blueprint.org") != 1)
  {
    die "Cannot log into $ftphost\n";
  }
  
  print"Downloading $tarfile from directory $bindir\n";
  if ($ftpsession->cwd($bindir) !=1) 
  { 
    die "Can not log into directory $bindir\n";
  }

  if ($ftpsession->binary eq "")
  {
    die "Cannot change download mode to binary\n";

  }
 
  if (($downloadfile = $ftpsession->get($tarfile)) eq "")
  {
    die "Cannot get file $tarfile. Please verify $tarfile exists in $bindir on $ftphost\n";
  }
  else
  {

    print "Downloading $tarfile complete\n";
  } 

}

sub unpacktar
{
  print "Start unpacking $tarfile\n";
  $zippedfile = $tarfile;
  if (system (" gunzip $tarfile ") !=0 )
  {
    die "Fail to unzip $tarfile\n";
  }
  else
  {
    print "Done unzip $tarfile\n";
  }
  $zippedfile =~ s/.gz//;
  if (system (" tar xvf $zippedfile ") !=0 )
  {
    die "Fail to Unpack $zippedfile\n";
  }
  else
  {
    print "Done Unpack $zippedfile\n";
  }
}

sub editconfig
{
  print "Please enter database driver path\n";
  $_ = <STDIN>;
  chomp;
  $driverpath = $_;
  print "Please enter database servername\n";
  $_ = <STDIN>;
  chomp;
  $dbserver = $_;
  print "Please enter database port number\n";
  $_ = <STDIN>;
  chomp;
  $dbport = $_;
  print "Please enter database user id, default is sh\n";
  $_ = <STDIN>;
  chomp;
  $userid = $_;
  print "Please enter database password, default is sh\n";
  $_ = <STDIN>;
  chomp;
  $passwd = $_;
  print "Please enter database name, default is seqhound\n";
  $_ = <STDIN>;
  chomp;
  $dbname = $_;

  # If no installation directory is specified, then the home directory
  # will be treated as the default installation location. 
  if ( -z $ARGV[1] )
  {
    $confdir = $ENV{HOME} . "/config";
  }
  else
  {
    $confdir = $ARGV[1] . "/config";
  }
  chdir $confdir || die "Cannot change to $confdir\n";
  $odbcfile = $confdir . "/.odbc.ini";
  $odbcfilesave = $confdir . "/.odbc.ini_save";
  $odbcfilenew = $confdir . "/.odbc.ini_new";

  open ( ODBCFILE, "<$odbcfile") || die "Can not open $odbcfile\n"; 
  open ( ODBCNEW, ">$odbcfilenew") || die "Can not open $odbcfilenew\n";
  
  while (<ODBCFILE>)
  {
    my $line = $_;
    #chomp $line;
    if ( $line =~ /Driver/)
    {
      $line =~ s/=.*$/\= $driverpath/
    }
    if ( $line =~ /SERVER/)
    {
      $line =~ s/=.*$/= $dbserver/
    }
    if ( $line =~ /PORT/)
    {
      $line =~ s/=.*$/= $dbport/
    }
    if ( $line =~ /USER/)
    {
      if ( -z $userid)
      {
        $userid = "sh";
      }
      $line =~ s/=.*$/= $userid/
    }
    if ( $line =~ /PASSWORD/)
    {
      if ( -z $passwd)
      {
        $passwd = "sh";
      }
      $line =~ s/=.*$/= $passwd/
    }
    if ( $line =~ /DATABASE/)
    {
      if ( -z $dbname)
      {
        $dbname = "seqhound";
      }
      $line =~ s/=.*$/= $dbname/
    }

    print ODBCNEW $line;
  } 
  close ODBCNEW;
  close ODBCFILE;
  rename $odbcfile, $odbcfilesave;
  rename $odbcfilenew, $odbcfile;
  $entrez = $confdir . "/.intrezrc";
  $entreznew = $confdir . "/.intrezrcnew";

  open ( ENTREZ, "<$entrez");
  open ( ENTREZNEW, ">$entreznew");
  while ( <ENTREZ> )
  {
    $line = $_;
    if ( $line =~ /username/ )
    {
      $line =~ s/=.*$/= $userid/;
    }
    if ( $line =~ /password/ )
    {
      $line =~ s/=.*$/= $passwd/;
    }
    if ( $line =~ /database/ )
    {
      $line =~ s/=.*$/= $dbname/;
    }
    print ENTREZNEW $line;
  }
  
  close ENTREZ;
  close ENTREZNEW;
  rename $entreznew, $entrez;
}

sub deployconfig()
{
  if ( -z $ARGV[1] )
  {
    $installdir = $ENV{HOME};
  }
  else
  {
    $installdir = $ARGV[1];
  }

  $builddir = $installdir . "/build";
  $core = $builddir . "/core";
  $redund = $builddir. "/redund";
  $go = $builddir. "/go";
  $llink = $builddir. "/locuslink";
  $taxdb = $builddir . "/taxdb";
  $mmdb = $builddir . "/mmdb";
  $comgen = $builddir . "/comgen";

  $confdir = $installdir . "/config";
  $testdir = $installdir . "test";
  $updatedir = $installdir . "/updates";
  $wwwcgi = $installdir . "/www/seqhound/cgi-bin";

  $entrezrc = $confdir ."/.intrezrc";
  $odbcini = $confdir . "/.odbc.ini";
  copy ($entrezrc, $core) || die "Unable to copy file $entrezrc to $core";
  copy ($entrezrc, $redund) || die "Unable to copy file $entrezrc to $redund";
  copy ($entrezrc, $go) || die "Unable to copy file $entrezrc to $go";
  copy ($entrezrc, $llink) || die "Unable to copy file $entrezrc to $llink";
  copy ($entrezrc, $taxdb) || die "Unable to copy file $entrezrc to $taxdb";
  copy ($entrezrc, $mmdb) || die "Unable to copy file $entrezrc to $mmdb";
  copy ($entrezrc, $comgen) || die "Unable to copy file $entrezrc to $comgen";
  copy ($entrezrc, $testdir) || die "Unable to copy file $entrezrc to $testdir";
  copy ($entrezrc, $updatedir) || die "Unable to copy file $entrezrc to $updatedir";
  copy ($entrezrc, $wwwcgi) || die "Unable to copy file $entrezrc to $wwwcgi";
  copy ($odbcini, $ENV{HOME}) || die "Unable to copy file $entrezrc to $$ENV{HOME}";

print "Deployment of config files .intrezrc and .odbc.ini successful\n";
}

sub relocatewww
{
  if ( -z $ARGV[1] )
  {
    $wwwsrc = $ENV{HOME} . "/www/seqhound";
  }
  else
  {
    $wwwsrc = $ARGV[1] . "/www/seqhound";
  }
  
  $wwwtarget = "/www";

  if ( system (" cp -r $wwwsrc $wwwtarget ") != 0)
  {
    die " Failed to copy $wwwsrc to $wwwtarget\n";
  }
  else
  {
    print "Done setting up directory /www/seqhound\n";
  } 

}

############# main function ###################
if (-z $ARGV[1])
{
  die "Please enter the installation directory for user seqhound\n";
}
if (($ARGV[0] != "linux") || ($ARGV[0] != "solaris"))
{
  die "Please specify the platform(linux or solaris)on the command line \n";
}

downloadshound();

unpacktar();

editconfig();

deployconfig();
relocatewww();
