#!/usr/bin/perl 
use SeqHoun
use strict;

#======================================================================
#This script is used for blueprint SeqHound data validation project.
#It should put into /www/cgi-bin/ in order to get it worked.
#======================================================================

my $DataDir = "/www/seqhound/htdocs/Validation/Data/";

my %cgivars= &getcgivars ;
my $diff_flag = "FALSE";

print "Content-type: text/html\n\n" ;

foreach (keys %cgivars){
	my $value = $cgivars{$_};
	if ( $value eq "SHoundGetGenBankff" || $value eq "SHoundGetFasta" ||
		 $value eq "SHoundFindAcc" || $value eq "SHoundAccFromGi" ||
		 $value eq "SHoundGetSeqEntry" || $value eq "SHoundGetTaxNameFromTaxID" ){
		$DataDir = $DataDir.$value."/";
		opendir(DIR, $DataDir);
		my @filenames = readdir(DIR);
		my @filenames = sort (@filenames);
		foreach my $item (@filenames){
		        #print out the link for each diff file
			if ($item =~ /_diff/) {
				$diff_flag = "TRUE";
				my $result = "/Validation/Data/".$value."/".$item;
				print "<a href=$result target=\"_blank\">$item</a><br>\n";
			}
		}
		if ($diff_flag eq "FALSE"){
			print "Data retrieved from SeqHound and NCBI are identical.\n";
	
		}
		close (DIR);
	}
	$diff_flag = "FALSE";
}


# a subroutine to get CGI name/value pair 
sub getcgivars {
    my ($in, %in) ;
    my ($name, $value) ;


    # First, read entire string of CGI vars into $in
    if ( ($ENV{'REQUEST_METHOD'} eq 'GET') ||
         ($ENV{'REQUEST_METHOD'} eq 'HEAD') ) {
        $in= $ENV{'QUERY_STRING'} ;

    } elsif ($ENV{'REQUEST_METHOD'} eq 'POST') {
        if ($ENV{'CONTENT_TYPE'}=~ m#^application/x-www-form-urlencoded$#i) {
            length($ENV{'CONTENT_LENGTH'})
                || &HTMLdie("No Content-Length sent with the POST request.") ;
            read(STDIN, $in, $ENV{'CONTENT_LENGTH'}) ;

        } else { 
            &HTMLdie("Unsupported Content-Type: $ENV{'CONTENT_TYPE'}") ;
        }

    } else {
        &HTMLdie("Script was called with unsupported REQUEST_METHOD.") ;
    }
    
    # Resolve and unencode name/value pairs into %in
    foreach (split(/[&;]/, $in)) {
        s/\+/ /g ;
        ($name, $value)= split('=', $_, 2) ;
        $name=~ s/%([0-9A-Fa-f]{2})/chr(hex($1))/ge ;
        $value=~ s/%([0-9A-Fa-f]{2})/chr(hex($1))/ge ;
        $in{$name}.= "\0" if defined($in{$name}) ;  # concatenate multiple vars
        $in{$name}.= $value ;
    }

    return %in ;

}
