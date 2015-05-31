#!/usr/bin/perl -w

####### creates the release package 'seqhound_perl.tar.gz'
####### for the SeqHound Perl Module



@default = ("seqhound_perl.tar"
           );

### create the release file
sub release 
{
   open (TAR, "<$ARGV[1]/MANIFEST");

   $_ = <TAR>;
   system ("tar cf $default[0] $ARGV[1]/$_");
   while ( <TAR> )
   {
      system("tar rf $default[0] $ARGV[1]/$_ ");
   }
   system("gzip $default[0]");
   close TAR;
}


### delete the release file
sub clean 
{
   $default[0] =~ s/.tar/.tar.gz/;
   system("rm $default[0]");
}

#### prints the usage notice
sub usuage 
{
   print STDOUT " Usuage: \n";
   print STDOUT " \t./release.pl (release|clean) [perl_dir]\n";

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

