#!/usr/bin/perl -w




sub parser 
{
   $file = shift;
   open ( NCBI, "<$ARGV[1]/$file" ) || die "Could not open $file";
   open ( DESC, ">$ARGV[1]/$file".".src" ) || die "Could not open $file.src";

   # the start of the descr object 
   print DESC "Seq-descr ::= {\n";

   $i = 0;     # keeps track of the brackets
   # look for the right line
   while ( $data = <NCBI> )
   {
      if ( $data =~ /descr {/ ) 
      { 
         #print STDOUT "found it $data\n"; 
         last; 
      }
   }

   $i++;
   # start copying over to DESC
   # while ( $data = getc(NCBI) ) 
   while ( $i > 0 ) 
   {
       $data = getc(NCBI); 
       if( $data =~ /\{/ ) { $i++; }
       elsif ( $data =~ /\}/ ) { $i--; }
       print DESC $data;

   }

 
   # the end of the descr object
   # print DESC "}";

   close ( DESC );
   close ( NCBI );

}




sub process 
{

   open (COMFF, "<$ARGV[0]") || die "Could not open comff";
   
   while ( $bact = <COMFF> )
   {
      chomp $bact; 
      if ( ! ($bact =~ /\t/) )
      {
         next;
      } else {
         $bact =~ /\t(.*)/;
         parser ( $1 );
      }

   } 
   
   close COMFF;

}




sub usuage
{
   print STDOUT "Usuage: ./pregenff <COMFF> <ASN DIR>\n";
   print STDOUT "\t <COMFF> Location of file comff\n";
   print STDOUT "\t <ASN DIR> Directory where asn kept\n";
   print STDOUT "This program is intended to be auto run.\n";
}

##########    main starts here 

   if ( 1 != $#ARGV ) {
      usuage();
      exit;
   }


   process ();
