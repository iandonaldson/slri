#!/usr/bin/perl -w

####### creates the release package 'seqhound_perl.tar.gz'
####### for the SeqHound Perl Module

### Change this for each release
$ver = "3.0";
@default = ("seqhound-bioperl-$ver.tar", "seqhound-bioperl-$ver"
           );

### create the release file
sub release 
{
   open (TAR, "<$ARGV[1]/MANIFEST");
   system ( "mkdir $default[1]" );
   while ( <TAR> )
   {
      chomp;
      system ( "cp $_ $default[1]" );
   }
   system ("tar cf $default[0] $default[1]");
   system("gzip $default[0]");
   close TAR;
}


### delete the release file
sub clean 
{
   $default[0] =~ s/.tar/.tar.gz/;
   system("rm -fr $default[0]");
   system ( "rm -fr $default[1]" );
}

#### prints the usage notice
sub usuage 
{
   print STDOUT " Usuage: \n";
   print STDOUT " \t./release.pl (release|clean) [seqhound bioperl dir]\n";

}

###### main starts here

   if ( $#ARGV == -1 ) { usuage(); exit; }

   if ( $ARGV[0] eq "release" ) {
       if ( $#ARGV != 1 ) { usuage(); exit; }
       release ();
   } elsif ( $ARGV[0] eq "clean" ) {
       if ( $#ARGV != 0 ) { usuage(); exit; }
       clean ();
   } else { usuage(); exit; }

