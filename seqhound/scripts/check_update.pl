#!/usr/bin/perl

# $Id: check_update.pl,v 1.5 2005/04/14 19:51:45 zwang Exp $
#*******************************************************************************************
# DESCRIPTION
#
#  Check if updated data files are available on the ftp site for:
#  mmdb:    ftp://ftp.ncbi.nih.gov/mmdb/mmdbdata/
#  domname: ftp://ftp.ncbi.nih.gov/pub/mmdb/cdd/
#  wgs:     ftp://ftp.ncbi.nih.gov/ncbi-asn1/wgs/
#
#  For wgs, besides checking if any files get updated on the ftp site, this script also 
#  create a list of files and compared with the old list to check if any file is removed.
#
#  For the first time to run this script, a file timestamp.log is created with the current
#  timestamp for variables mmdb, wgs and cdd. A file wgs.list is created with the 
#  list of wgs files in wgs/ on ncbi ftp. An e-mail is sent out to inform ALL the wgs
#  files are updated. This is not true. You need to check if any of them on the ftp site 
#  have been updated since the initial build of the database. If they are,
#  manually download and parse them. 
#  For the subsequent run, the e-mail regarding updated files reflects the fact on the 
#  ftp site.  
#
#******************************************************************************************

use Net::FTP;
use shconfig;

  #############
  # e-mail
  #############
  $admail = shconfig->ReadIntrez( "mail" );

  ######################
  # get time stamp
  ######################
  my $ts_file = "timestamp.log";
  my ($ts_mmdb, $ts_domname, $ts_wgs);
  get_timestamp();
  my ($ts_mmdb_new, $ts_domname_new, $ts_wgs_new) = ($ts_mmdb, $ts_domname, $ts_wgs);

  my $ts_current = $^T;

  #####################
  # variables
  ####################
  my $output;
  my $is_updated = "FALSE";
  my $has_error = "FALSE";

  #############################
  # to check wgs file list
  ############################
  $file_wgs_old = "wgs.list.bck"; 
  $file_wgs_current = "wgs.list"; 
  $has_wgs_list = "FALSE";
  $has_wgs_removed = "FALSE";

  ###########################
  # connect to ftp
  ###########################
  if(!($ftp = Net::FTP->new("ftp.ncbi.nih.gov", Passive=>0)))
  {
    $output =  "Cannot connect.\n";
    $has_error = "TRUE";
    die "Cannot connect.";
  }
  if(!($ftp -> login("anonymous", $admail)))
  {
    $output .= "Cannot login.\n";
    $has_error = "TRUE";
    die "Cannot login.";
  }


  ###########################
  # mmdb
  ###########################
  if(!($ftp->cwd("/mmdb/mmdbdata/")))
  {
    $output .= "Cannot cwd /mmdb/mmdbdata/.\n";
    $has_error = "TRUE";
    die "Cannot cwd to /mmdb/mmdbdata/.";
  }
  $file = "new.idx";
  my $ts_mmdb_ftp = $ftp->mdtm($file); 
  if ($ts_mmdb_ftp > $ts_mmdb) {
    $is_updated = "TRUE";
    $output .= "mmdb updated on the ftp site. /mmdb/mmdbdata/\n";
    $ts_mmdb_new = $ts_current;
  }
    
  ###########################
  # domname
  ###########################
  if(!($ftp->cwd("/pub/mmdb/cdd/")))
  {
    $output .= "Cannot cwd /pub/mmdb/cdd/.\n";
    $has_error = "TRUE";
    die "Cannot cwd to /pub/mmdb/cdd/.";
  }
  $file = "acd.tar.gz";
  my $ts_domname_ftp = $ftp->mdtm($file); 
  if ($ts_domname_ftp > $ts_domname) {
    $is_updated = "TRUE";
    $output .= "acd.tar.gz updated on the ftp site. /pub/mmdb/cdd/\n";
    $ts_domname_new = $ts_current;
  }
  $file = "cdd.tar.gz";
  $ts_domname_ftp = $ftp->mdtm($file); 
  if ($ts_domname_ftp > $ts_domname) {
    $is_updated = "TRUE";
    $output .= "ccd.tar.gz updated on the ftp site. /pub/mmdb/cdd/\n";
    $ts_domname_new = $ts_current;
  }
    
  ###########################
  # wgs in ncbi-asn1
  ###########################
  if($ftp->binary eq "")
  {
    $output .= "Cannot change type to bin\n";
    $has_error = "TRUE";
    die "Cannot change type to bin.";
  }
  my @list_rswgs;
  if(!($ftp->cwd("/ncbi-asn1/wgs/")))
  {
    $output .= "Cannot cwd /ncbi-asn1/wgs/.\n";
    $has_error = "TRUE";
    die "Cannot cwd to /ncbi-asn1/wgs/.";
  }
  my @list_wgs;
  if ((@list_wgs = $ftp->ls) eq "")
  {
    $output .= "Cannot ls /ncbi-asn1/wgs/.\n";
    $has_error = "TRUE";
    die "Cannot ls /ncbi-asn1/wgs/.\n";
  }
  foreach $file(@list_wgs)
  {
    if ($file =~ m/wgs./) {
       my $ts_wgs_ftp = $ftp->mdtm($file);
       if ( $ts_wgs_ftp > $ts_wgs) {
          $is_updated = "TRUE";
          $output .= "$file updated on the ftpsite.\n";
          $ts_wgs_new = $ts_current;

          if (($ftp->get($file)) eq "")
          {
             $output .= "Cannot get $file.\n";
             $has_error = "TRUE";
             die "Cannot get $file\n";
          }
       }
    }
  }
  ###########################
  # wgs in refseq
  ###########################
  if(!($ftp->cwd("/refseq/wgs/")))
  {
    $output .= "Cannot cwd /refseq/wgs/.\n";
    $has_error = "TRUE";
    die "Cannot cwd to /refseq/wgs/.";
  }
  if ((@list_rswgs = $ftp->ls) eq "")
  {
    $output .= "Cannot ls /refseq/wgs/.\n";
    $has_error = "TRUE";
    die "Cannot ls /refseq/wgs/.\n";
  }
  foreach $file(@list_rswgs)
  {
    if ($file =~ m/.bna./) {
       my $ts_wgs_ftp = $ftp->mdtm($file);
       if ( $ts_wgs_ftp > $ts_wgs) {
          $is_updated = "TRUE";
          $output .= "$file updated on the ftpsite.\n";
          $ts_wgs_new = $ts_current;

          if (($ftp->get($file)) eq "")
          {
             $output .= "Cannot get $file.\n";
             $has_error = "TRUE";
             die "Cannot get $file\n";
          }
       }
    }
  }  
  #####################
  # write to wgs.list
  #####################
  if ((-s $file_wgs_current)) {
     system ("cp $file_wgs_current $file_wgs_old");
     $has_wgs_list ="TRUE";
  }
  if (!(open (WGSLIST, ">$file_wgs_current"))) {
    $output .= "Cannot open $file_wgs_current for write\n";
    die "Cannot open $file_wgs_current for write\n";
  }
  foreach $file(@list_wgs)
  {
    if ($file =~ m/wgs./) {
       print (WGSLIST "$file\n");
    }
  }
  foreach $file(@list_rswgs)
  {
    if ($file =~ m/.bna./) {
       print (WGSLIST "$file\n");
    }
  }
  close WGSLIST; 

  if(!($ftp->quit))
  {
    $output .= "Cannot quit.\n";
    die "Cannot quit.";
  }

  ###############################
  # check wgs files
  # check if any file is removed
  ###############################
  if ($has_wgs_list eq "TRUE") {
    check_wgs_files();
  }

  ################################
  # log new timestamp and e-mail
  ################################
  if ($is_updated eq "TRUE") {
     log_new_timestamp();
  }
  if ($is_updated eq "TRUE" || $has_error eq "TRUE" || $has_wgs_removed eq "TRUE") {
     send_mail();
  }

  exit;




######################################################################
##
## find out when the files were last ftp-ed.
##
#####################################################################
sub get_timestamp() {
  open (TSFILE, "<$ts_file");

  ## create timestamp file if it doesn't exist.
  if (!(-e TSFILE)) {
	$output .= "$ts_file didn't exist. Just created with the current timestamp\n\n";
	open (TSFILE, ">$ts_file");
  	my $now = $^T;
	print TSFILE "mmdb=$now\ndomname=$now\n$wgs=$now\n"
  }
  close TSFILE;

  open (TSFILE, "<$ts_file");
  if (!(-e TSFILE)) {
	$output .= "Cannot open file $ts_file\n";
	send_mail();
	die "Cannot open file %ts_file\n";
  }

  while (<TSFILE>) {
        my $line = $_;
        $line =~ s/\s+//g;

        my @words = split(/=/, $line);
        my $attr = $words[0];
        my $value = $words[1];

        if ($attr eq "mmdb") {
                $ts_mmdb = $value;
        }
        elsif ($attr eq "domname") {
                $ts_domname = $value;
        }
        elsif ($attr eq "wgs") {
                $ts_wgs = $value;
        }
  }
  close TSFILE;
}

######################################################################
##
## check if any wgs files are removed
##
#####################################################################
sub check_wgs_files() {
  $file_wgs_diff = "wgs.list.diff";
  @file_new;
  @file_removed;

  system ("diff $file_wgs_current $file_wgs_old > $file_wgs_diff");

  if (!(open (WGSDIFF, "$file_wgs_diff"))) {
    $output .= "Cannot open $file_wgs_diff for read\n";
    die "Cannot open $file_wgs_diff for read\n";
  }
  while (<WGSDIFF>) {
    chomp;
    $line = $_;
    if ($line =~ m/</ ) {
       @words = split(/ /, $line);
       push (@file_new, $words[1]);
    }
    if ($line =~ m/>/ ) {
       @words = split(/ /, $line);
       push (@file_removed, $words[1]);
       $has_wgs_removed = "TRUE";
    }
  }
  close WGSDIFF; 

  $output .= "\nNew files in WGS are:\n";
  foreach $file (@file_new) {
    $output .= "$file\n";
  }
  $output .= "\nRemoved files in WGS are:\n";
  foreach $file (@file_removed) {
    $output .= "$file\n";
  }
}

######################################################################
##
## log new timestamps only if a timestamp is changed.
##
#####################################################################
sub log_new_timestamp() {

  if ($is_updated eq "TRUE") {
    open (TSFILE, ">$ts_file") || die "Cannot open file $ts_file\n";
    print TSFILE "mmdb=$ts_mmdb_new\n";
    print TSFILE "domname=$ts_domname_new\n";
    print TSFILE "wgs=$ts_wgs_new\n";
    close TSFILE;
  }
} 

######################################################################
##
## send e-mail if there is updated
##
######################################################################
sub send_mail() {
   $mymail = "| /usr/bin/mailx -s \"Update alert\" " . $admail;
   open(MAIL, $mymail);
   print MAIL $output;
   close MAIL;
}



# $Log: check_update.pl,v $
# Revision 1.5  2005/04/14 19:51:45  zwang
# Download updated wgs files.
#
# Revision 1.4  2005/04/08 21:14:30  zwang
# Also check refseq/wgs
#
# Revision 1.3  2005/04/01 19:52:30  zwang
# Create timestamp.log and wgs.list for the first time run.
#
# Revision 1.2  2005/03/31 22:06:11  zwang
# Read e-mail addr. from .intrezrc
#
# Revision 1.1  2005/03/31 22:03:15  zwang
# New file to check if source files are updated for mmdb, wgs and cdd.
#
