#! /usr/bin/perl

# $Id: testresult.pl,v 1.6 2005/04/04 15:42:04 zwang Exp $

use Time::localtime;

$TODAY = "today";
$ARCHIVE = "archive";
$NA = "N/A";
# indices for 2-dimensional array of date and logs
$INDEX_DATE = 0;
$INDEX_CLOCAL = 1;
$INDEX_CREMOTE = 2;
$INDEX_PERL = 3;
$INDEX_BIOPERL = 4;
$INDEX_JAVA = 5;


##### get pairs of name and value
%cgivars= &getcgivars ;


##### construct today's date
$tm = localtime;
$today =  sprintf ("%04d%02d%02d",($tm->year)+1900, ($tm->mon)+1, $tm->mday);

#path of the output files for URL
$filePathURL = "/testresults/";
$filePathFull = "/www/seqhound/htdocs/testresults/";

#name of the log files
$filePostfixCLocal = "summaryCLocal";
$filePostfixCRemote = "summaryCRemote";
$filePostfixPerl = "logPerl";
$filePostfixBioperl = "logBioperl";
$filePostfixJava = "logJava";

#template files for blueprint web pages
my $template1 = "seq_menu.txt";
my $template2 = "seq_footer.txt";

print "Content-type: text/html\n\n" ;

#add in header
open (TEMP1, "<$template1");
while (<TEMP1>)
{
  print $_;
}
close (TEMP1);

# add in main content
foreach (keys %cgivars) {
   $value = $cgivars{$_};

   #### display the logs for today's test
   if ($value eq $TODAY) {
      #full path of today's summary report for CLocal
      $fileToadyFullCLocal = $filePathURL . $today . $filePostfixCLocal;
      $fileToadyFullCRemote = $filePathURL . $today . $filePostfixCRemote;
      $fileToadyFullPerl = $filePathURL . $today . $filePostfixPerl;
      $fileToadyFullBioperl = $filePathURL . $today . $filePostfixBioperl;
      $fileToadyFullJava = $filePathURL . $today . $filePostfixJava;
      
      print "<br><center><H1>SeqHound API test results for $today</center></H1>\n";
     
      print "<center>\n";
 
      # get the list of all YYYYMMDDlog files
      opendir(DIR, $filePathFull);
      @filenames = readdir(DIR);
      close(DIR);
      @logsToPrint = FindInterestFiles($filenames);
      
      # files are sorted chronically
      # print today's logs.
      $i=$#logsToPrint; # index for todays log
      if ($logsToPrint[$i][$INDEX_DATE] eq $today) {
         #print links to log files
         if ($logsToPrint[$i][$INDEX_CLOCAL] ne $NA) {
            $fileToadyFullCLocal = $filePathURL . $logsToPrint[$i][$INDEX_CLOCAL];
            print "<a href=\"$fileToadyFullCLocal\" target=\"_blank\">C Local API test result</a><br>\n";      
         } else {
            print "C Local API test result<br>\n";
         }
         if ($logsToPrint[$i][$INDEX_CREMOTE] ne $NA) {
            $fileToadyFullCRemote = $filePathURL . $logsToPrint[$i][$INDEX_CREMOTE];
            print "<a href=\"$fileToadyFullCRemote\" target=\"_blank\">C Remote API test result</a><br>\n";      
         } else {
            print "C Remote API test result<br>\n";
         }
         if ($logsToPrint[$i][$INDEX_PERL] ne $NA) {
            $fileToadyFullPerl = $filePathURL . $logsToPrint[$i][$INDEX_PERL];
            print "<a href=\"$fileToadyFullPerl\" target=\"_blank\">Perl API test result</a><br>\n";      
         } else {
            print "Perl API test result<br>\n";
         }
         if ($logsToPrint[$i][$INDEX_BIOPERL] ne $NA) {
            $fileToadyFullBioperl = $filePathURL . $logsToPrint[$i][$INDEX_BIOPERL];
            print "<a href=\"$fileToadyFullBioperl\" target=\"_blank\">Bioperl API test result</a><br>\n";      
         } else {
            print "Bioperl API test result<br>\n";
         }
         if ($logsToPrint[$i][$INDEX_JAVA] ne $NA) {
            $fileToadyFullJava = $filePathURL . $logsToPrint[$i][$INDEX_JAVA];
            print "<a href=\"$fileToadyFullJava\" target=\"_blank\">Java API test result</a><br>\n";      
         } else {
            print "Java API test result<br>\n";
         }
      } else {
         print "API test for today is still in progress.<br>\n";
      }   
      print "</center>\n";
   }
   #### display a table of all archived logs
   elsif ($value eq $ARCHIVE) {
      print "<br><center><H1>Archived SeqHound API test results</H1></center>\n";

      print "\n<table border>\n";
      print "  <tr>\n";
      print "    <td></td> <td><b>C Local</b></td> <td><b>C Remote</b></td> <td><b>Perl Remote</b></td> <td><b>Bioperl Remote</b></td> <td><b>Java Remote</b></td></b>\n";
      print "  </tr>\n";
      
      # get the list of all YYYYMMDDlog files
      opendir(DIR, $filePathFull);
      @filenames = readdir(DIR);
      close(DIR);
      @logsToPrint = FindInterestFiles($filenames);
      
      # files are sorted chronically
      # print them reversed.
      for ($i=$#logsToPrint; $i>=0; $i--) {
         print "  <tr>\n";
         
         #the first element is always the date
         print "    <td><b>$logsToPrint[$i][$INDEX_DATE]</b></td>";
         
         #print links to log files
         for $j (1 .. $#{$logsToPrint[$i]}) {
            $element = $logsToPrint[$i][$j];
            $fileArchiveFull = $filePathURL . $element;
            
            if ($element ne $NA) {
               print "    <td><a href=\"$fileArchiveFull\" target=\"_blank\">view</a></td>\n";
            } else {
               print "    <td>n/a</td>\n";            
            }
         }
         print "  </tr>\n";         
      }
      print "\n</table>\n";
   }
}

#foreach (keys %cgivars) {
    #print "<li>[$_] = [$cgivars{$_}]\n" ;
#}


# add in footer
open (TEMP2, "<$template2");
while (<TEMP2>)
{
  print $_;
}
close (TEMP2);



############################################################################
# Read all CGI vars into an associative array.
# If multiple input fields have the same name, they are concatenated into
#   one array element and delimited with the \0 character (which fails if
#   the input has any \0 characters, very unlikely but conceivably possible).
# Currently only supports Content-Type of application/x-www-form-urlencoded.
############################################################################
sub getcgivars {
    local($in, %in) ;
    local($name, $value) ;


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

##############################################################################
#
# pulls out the logs files that we are interested in 
#
##############################################################################
sub FindInterestFiles(@filenames) {
   local @fileSubList;
   
   @filenames = sort(@filenames);
  
   $index=-1;
   $prevDate="";

   foreach $item (@filenames) {
      $date = substr($item, 0, 8);
     
      # ignore files without datestamp
      $_ = $date;
      next unless (m/[0-9]/);

      if ($date ne $prevDate) {
         $index++;

         $prevDate = $date;
         $fileSubList[$index][$INDEX_DATE] = $date;
         
         # initialize every element for log name
         for $i (1 .. 5) {
            $fileSubList[$index][$i]=$NA;
         }
      }
      
      $_ = $item;
      if (m/$filePostfixCLocal/) {
         $fileSubList[$index][$INDEX_CLOCAL] = $item;
      }
      elsif (m/$filePostfixCRemote/) {
         $fileSubList[$index][$INDEX_CREMOTE] = $item;
      }
      elsif (m/$filePostfixPerl/) {
         $fileSubList[$index][$INDEX_PERL] = $item;
      }
      elsif (m/$filePostfixBioperl/) {
         $fileSubList[$index][$INDEX_BIOPERL] = $item;
      }
      elsif (m/$filePostfixJava/) {
         $fileSubList[$index][$INDEX_JAVA] = $item;
      }
      
   }
      
      
   #print to check
   #for $i (0 .. $#fileSubList) {
   #   for $j (0 .. $#{$fileSubList[$i]} ) {
   #     print " *$fileSubList[$i][$j]*";
   #   }
   #   print "<br>";
   #}
   
   return @fileSubList;
}


# $Log: testresult.pl,v $
# Revision 1.6  2005/04/04 15:42:04  zwang
# Fixed minor bug, should grep by logJava fro Java result.
#
# Revision 1.5  2005/04/01 16:15:17  zwang
# Added file extension .html to the new Java test log.
#
# Revision 1.4  2005/03/30 16:26:39  zwang
# Change message when today's API result is not available.
#
# Revision 1.3  2004/07/30 16:23:19  zwang
# Added header and footer for blueprint web page.
#
