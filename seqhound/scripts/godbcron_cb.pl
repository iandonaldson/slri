#!/usr/bin/perl

# $Id: godbcron_cb.pl,v 1.2 2004/05/28 15:29:15 vgu Exp $
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
#  ftp of Locus Link template file database
#
#******************************************************************************************


use Net::FTP;


#directore with the executable
$path = "/hibbert-local/shoundtest/4.godb.files/";
#directory with the backup version of the GO tables
$dbpath = "/hibbert-local/shoundtest/datab/";
@ontoList = ("component.ontology", "function.ontology",
"process.ontology");
@genList = ("gene_association.Compugen_GenBank.gz",
"gene_association.Compugen_UniProt");
$rebuild = "FASLE";

$fullexec = $path."goparser";
$logfile = "gocronlog";

unlink("/seqhound/build/flags/godbcron.flg");

open(LOG, ">$logfile") ||  die "Cannot create $logfile\n";
#flush out the output to the files.
select(LOG);
$| = 1;
print LOG "GO update.\n";
#Make sure parser goparser is in the path
my $goparser = $path."goparser";
if (!(-s $goparser)) 
{
  print LOG "Parser $goparser not found.\n";
  goto CLEAN;
}

$FtpHost = "ftp.geneontology.org";
$ftp = Net::FTP->new($FtpHost, Passive => 0);

if ($ftp->login("anonymous","katerina\@mshri.on.ca") != 1) {
    print LOG "Can't login to $FtpHost\n";
    goto CLEAN; 
}

#print LOG "Logged in.\n";

#get the current working directory
if (($Pwd = $ftp->pwd) eq "") {
    print LOG "Can't get current directory\n";
    goto CLEAN;
}

print "pwd=$Pwd\n";

$NewCwd = "/pub/go/ontology";
if ($ftp->cwd($NewCwd) != 1) {
    print LOG "Can't cwd to $NewCwd\n";
    goto CLEAN;
}
$Pwd = $ftp->pwd;
#print LOG "pwd= $Pwd\n";
foreach $item (@ontoList)
{
  my $localonto = $path.$item;
  if ((!(-s $localonto)) || ($ftp->mdtm($item) > ($^T - 86400*(-M $localonto))))
  {
    if (!($ftp->binary))
    {
      print LOG "Cannot change to binary.\n";
      goto CLEAN;
    }
    if (!($ftp->get($item, $localonto)))
    {
      print LOG "Cannot get $item.\n";
      die "Cannot get $item.\n";
    }
    
    $rebuild = "TRUE";
  } 
}

$NewCwd = "/pub/go/gene-associations";
if ($ftp->cwd($NewCwd) != 1) {
    print LOG "Can't cwd to $NewCwd\n";
    goto CLEAN;
}
$Pwd = $ftp->pwd;
#print LOG "pwd= $Pwd\n";

foreach $genitem (@genList)
{
  my $localgen = $path.$genitem;
  if ((!(-s $localgen)) || ($ftp->mdtm($genitem) > ($^T - 86400*(-M $localgen))))
  {
    if (!($ftp->binary))
    {
      print LOG "Cannot change to binary.\n";
      goto CLEAN;
    }
    if (!($ftp->get($genitem, $localgen)))
    {
      print LOG "Cannot get $genitem.\n";
      goto CLEAN;
    }
  } 
}

print LOG "closing connection.\n";
if (!($ftp->quit))
{
  print LOG " Cannot close FTP connection.\n";
  goto CLEAN;
}

#if rebuild flag is TRUE rebuild the GO tables
if ($rebuild eq "TRUE")
{
  #copy existing GO tables to backup directory
  my $backupdir = $dbpath."old_go/";
  my $existgo = $dbpath."GO*";
  my $backupgo = $backupdir."GO*";
  system ("mkdir -p $backupdir");
  system ("mv $existgo $backupdir"); 
  if (system (" cd $path; $fullexec") != 0)
  {
    print LOG "Rebuild GO failed.\n";
    #restore old go tables
    system ("cp $backupgo $dbpath");
    goto CLEAN;
  }
  # Copy new GO tables to production build
  $prodbuild = "/seqhound/build/datab/";
  system ("cp $existgo $prodbuild");
  print LOG "Done rebuilding GO tables.\n";
}
else
{
  print LOG " Local files up to date. No need to rebuild GO tables.\n";
}

CLEAN:

# things to customize
# your e-mail
$admail = "seqhound.admin\@blueprint.org";
$mymail = "| /usr/bin/mailx -s \"GO cron status\" " . $admail;

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
open TOUCH, ">/seqhound/build/flags/godbcron.flg"; close TOUCH;
close (LOG);
close (MAIL);

