#!/usr/bin/perl -w
use Net::FTP;


#### This is a catch up script for SeqHound
#### It will download the genbank and refseq files
#### from NCBI and then run update and mother 
#### using the downloads.
#### then it gzips up and moves files to 'asofiles'
#### for storage

#### In the future, can expand on this script, but
#### Right now, have to manually add files to download
#### into @gbfiles and @rsfiles and change @ind to reflect
#### the true size of @rsfiles and @gbfiles


@info = ( "ftp.ncbi.nih.gov",
          "anonymous",
          "seqhound.admin\@blueprint.org",
          "/ncbi-asn1/daily-nc",
          "/refseq/daily"
        );

@gbfiles = (
            "nc0212.aso.gz"
           );

@rsfiles = (
            "rsnc.0212.2004.bna.gz"
           );

$release = 139;
@ind = (0);


#### does the ftping
sub ftp 
{
   $ftp = Net::FTP->new("$info[0]", Passive=>0) || die "Could not create ftp object"; 
   $ftp->login("$info[1]", "$info[2]") || die "Could not login";
   $ftp->binary || die "Could not chmod to binary";
   $ftp->cwd ("$info[3]") || die "Could now cwd into $info[3]";

   foreach $file (@gbfiles)
   {
      $ftp->get ( $file ) || die "Could not get $file";
      # print STDOUT "Getting $file\n";
   }

   $ftp->cwd ( "$info[4]" ) || die "Could not cwd into $info[4]";
   foreach $file (@rsfiles) 
   {
      $ftp->get ( $file ) || die "Could not get $file";
      # print STDOUT "Getting $file\n";
   }
   $ftp->quit || die "Could not quit ftp";
}


### unzips downlads
sub unzip 
{
   foreach $i (@ind)
   {
      system ("gunzip $gbfiles[$i]");
      # print STDOUT "gunzip $gbfiles[$i]\n";
      system ("gunzip $rsfiles[$i]");
      # print STDOUT "gunzip $rsfiles[$i]\n";
   }
}



### runs mother and update on the downloaded files
sub execute
{
   foreach $i (@ind)
   {
      $tmp = $gbfiles[$i]; 
      $tmp =~ s/.gz//; 
      if ( 0 != system ("./update -i $tmp") )
      {
         # print STDOUT "./update -i $tmp\n";
         die "Update unsuccessful on gb $tmp";
      }
      if ( 0 != system ("./mother -i $tmp -r $release -n F -m F -u T") )
      {
         # print STDOUT "./mother -i $tmp -r $release -n F -m F -u T\n";
         die "Mother unsuccessful on gb $tmp";
      }
      $tmp = $rsfiles[$i];
      $tmp =~ s/.gz//;
      $tmp2 = $tmp;
      $tmp2 =~ s/rsnc./rsnc/;
      system ("mv $tmp $tmp2"); 
      if ( 0 != system ("./update -i $tmp2") )
      {
         # print STDOUT "./update -i $tmp2\n";
          die "Update unsuccessful on rs $tmp2";
      }
      if ( 0 != system ("./mother -i $tmp2 -r $release -n F -m F -u T") )
      {
         # print STDOUT "./mother -i $tmp2 -r $release -n F -m F -u T\n";
         die "Mother unsuccessful on rs $tmp2";
      }
      # print STDOUT "$tmp\n";
   }
}



### zips files up and then moves them to asofiles 
sub cleanup
{
   foreach $i (@ind) 
   {
      $tmp = $gbfiles[$i];
      $tmp =~ s/.gz//;
      system("gzip $tmp");
      # print STDOUT "gzip $tmp\n";
      system("mv $gbfiles[$i] asofiles");
      $tmp = $rsfiles[$i];
      $tmp =~ s/.gz//;
      system("gzip $tmp");
      # print STDOUT "gzip $tmp\n";
      system("mv $rsfiles[$i] asofiles");
      # print STDOUT "mv $rsfiles[$i] asofiles\n";
   }
}

############# Main starts here

   #ftp ();
   #unzip ();
   execute ();
   #cleanup ();
