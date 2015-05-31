#! /usr/bin/perl


# $Id: shunittestcron.pl,v 1.8 2005/03/31 20:49:13 zwang Exp $
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
#  Run API test drivers for local C, remote C/Per/bioper/Java.
#  Copy the output logs to directory /www/seqhound_odbc/htdocs/testresults/
#
#******************************************************************************************
#
# How to run this script:
# 1.in a directory called "testdirvers", make subdirectories Bioperl, clocal, cremote, java and perl 
# 2.put the test driver for each language into the proper subdirectory
# 3.this script is in directory "testdrivers", i.e. at the same level of those subdirectoroies
# 4.log files are generated in subdirectories, attached with proper postfix (e.g. language
#   and date), then moved to /www/
#


  use Net::FTP;
  use Time::localtime;

  #construct today's date
  $tm = localtime;
  $today =  sprintf ("%04d%02d%02d",($tm->year)+1900, ($tm->mon)+1, $tm->mday);

  #things to customize....
  $admail = "zwang\@blueprint.org"; 
#  $admail = "seqhound.admin\@blueprint.org"; 
  $makeFullPath = "/usr/ccs/bin/make";

  #which driver to run
  $ALL = "all";
  $CLOCAL = "clocal";
  $CREMOTE = "cremote";
  $PERL = "perl";
  $BIOPERL = "bioperl";
  $JAVA = "java";
  $torun = @ARGV[0];
  if ($torun eq "") {
     $torun = $ALL;
  }

  #directory with the test driver; has to have "/" at the end!
  $pathRoot = "/shdba/shdbinnodb/testdrivers/";
  $pathCLocal = $pathRoot . "clocal/";
  $pathCRemote = $pathRoot . "cremote/";
  $pathPerl = $pathRoot . "perl/";
  $pathBioperl = $pathRoot . "bioperl/";
  $pathJava = $pathRoot . "java/";
  $pathJavaResult = $pathJava . "test_results/org/blueprint/seqhound/";
  
  #name of the test drivers;
  $exeCLocal = "shunittest_odbc_local";
  $exeCRemote = "shunittest_odbc_rem";
  
  #set up absolute paths to executables, update files and databases
  $fullexeCLocal = $pathCLocal . $exeCLocal;
  $fullexeCRemote = $pathCRemote . $exeCRemote;
  
  #name of the log for failed API;
  $failLogMaster = $today. "FailedAPI.log";
  $fullFailLogMaster = $pathRoot . $failLogMaster;


  #where to write the logs
  $logFileWritePath = "/www/seqhound/htdocs/testresults/";
  
  # end of things to customize
  
  $mymail = "| /usr/bin/mailx -s \"API test drivers status\" " . $admail;
  open(MAIL, $mymail);


  #####################################
  # clean up all exisiting log files
  #####################################
  CleanUpLogs();
  

  #####################################
  # run test driver for C Local
  #####################################
  if ($torun eq $ALL || $torun eq $CLOCAL) { 
   if (-s $fullexeCLocal) 
   {
     #run shunitteset
     if (system("cd $pathCLocal; $fullexeCLocal > driverout") != 0) {
        print MAIL "\n** $fullexeCLocal failed **\n\n";
     }
     else 
     {
        print MAIL "$fullexeCLocal completed successfully.\n"; 
     }
     if (CleanUpCLocal() == 1) {
        print MAIL "CLocal logs successfully renamed and moved to /www/ \n";
     }
   }
   else
   {
     print MAIL "\n** Cannot find executable $fullexeCLocal **\n\n";
   }
  }

  #####################################
  # run test driver for C remote
  #####################################
  if ($torun eq $ALL || $torun eq $CREMOTE) { 
   if (-s $fullexeCRemote) 
   {
     #run shunitteset
     if (system("cd $pathCRemote; $fullexeCRemote > driverout") != 0) {
        print MAIL "\n** $fullexeCRemote failed **\n\n";
     }
     else 
     {
        print MAIL "$fullexeCRemote completed successfully.\n"; 
     }
     if (CleanUpCRemote() == 1) {
        print MAIL "CRemote logs successfully renamed and moved to /www/ \n";
     }
   }
   else
   {
     print MAIL "\n** Cannot find executable $fullexeCRemote **\n\n";
   }
  }
 
  #####################################
  # run test driver for Perl remote
  #####################################
  if ($torun eq $ALL || $torun eq $PERL) {
     #run test driver for Perl
     if (system("cd $pathPerl; perl test.pl input > driverout") != 0) 
     {
        print MAIL "\n** make test for Perl failed **\n\n";
     }
     else 
     {
        print MAIL "make test for Perl completed successfully.\n";
     }
     if (CleanUpPerl() == 1) {
        print MAIL "Perl log successfully renamed and moved to /www/ \n";
     }
  }
   
  #####################################
  # run test driver for Bioperl remote
  #####################################
  if ($torun eq $ALL || $torun eq $BIOPERL) {
     #run test script
     if (system("cd $pathBioperl; $makeFullPath test > driverout") != 0) 
     {
        print MAIL "\n** make test for Bioperl test failed **\n\n";
     }
     else 
     {
        print MAIL "make test for Bioperl test completed successfully.\n";
     }
     if (CleanUpBioperl() == 1) {
     	print MAIL "Bioperl log successfully renamed and moved to /www/ \n";
     }
  }

  #####################################
  # run test driver for Java remote
  #####################################
  if ($torun eq $ALL || $torun eq $JAVA) {
     #run test driver for Java
     if (system("cd $pathJava; /software/ant/bin/ant test > driverout") != 0) 
     {
        print MAIL "\n** /software/ant/bin/ant test for Java failed **\n\n";
     }
     else 
     {
        print MAIL "/software/ant/bin/ant test for Java completed successfully.\n";
     }
     if (CleanUpJava() == 1) {
        print MAIL "Java log successfully renamed and moved to /www/ \n";
     }
  }

  #####################################
  # grep 'fail' in all log files
  #####################################
  if (GrepFail() == 0) {
     print MAIL "ERROR: Failed to grep failed API.\n";
     close MAIL;
     exit(0);
  }

  close MAIL;

  

######################################################################
#
# delete all existing log files in subdirectories
#
######################################################################
sub CleanUpLogs() {
   @logs[0] = $pathCLocal . "shreport*";
   @logs[1] = $pathCLocal . "summary*";
   @logs[2] = $pathCLocal . "shunittestlog*";
   @logs[3] = $pathCRemote . "shreport*";
   @logs[4] = $pathCRemote . "summary*";
   @logs[5] = $pathCRemote . "shunittestlog*";
   @logs[6] = $pathPerl . "test_summary.log";
   @logs[7] = $pathBioperl . "biop_test.log";
   @logs[8] = $pathJavaResult . "SeqhoundAPITest.html";
   
   foreach $item (@logs) {
      system("rm -f $item");   
   }
}

######################################################################
#
# append Date to the name logs, move logs to /www/
#
######################################################################

sub CleanUpCLocal() {
     $shreport = $pathCLocal . "shreport*";
     $shreportToday = $pathCLocal . $today . "shreportCLocal";
     $summary = $pathCLocal . "summary*";
     $summaryToday = $pathCLocal . $today . "summaryCLocal";
     $shunittestlog = $pathCLocal . "shunittestlog*";
     $shunittestlogToday = $pathCLocal . $today . "shunittestlogCLocal";
     $logsToday = $pathCLocal . $today . "*";
     if (system("mv -f $shreport $shreportToday") != 0)
     {
        print MAIL "Cannot rename $shreport to $shreportToday.\n";
        return 0;
     }
     if (system("mv -f $summary $summaryToday") != 0)
     {
        print MAIL "Cannot rename $summary to $summaryToday.\n";
        return 0;
     }
     if (system("mv -f $shunittestlog $shunittestlogToday") != 0)
     {
        print MAIL "Cannot rename $shunittestlog to $shunittestlogToday.\n";
        return 0;
     }
     if (system("mv -f $logsToday $logFileWritePath ") != 0)
     {
        print MAIL "Cannot mv $logsToday $logFileWritePath.\n";
        return 0;
     }
     return 1;     
}

sub CleanUpCRemote() {
     $shreport = $pathCRemote . "shreport*";
     $shreportToday = $pathCRemote . $today . "shreportCRemote";
     $summary = $pathCRemote . "summary*";
     $summaryToday = $pathCRemote . $today . "summaryCRemote";
     $shunittestlog = $pathCRemote . "shunittestlog*";
     $shunittestlogToday = $pathCRemote . $today . "shunittestlogCRemote";
     $logsToday = $pathCRemote . $today . "*";
     if (system("mv $shreport $shreportToday") != 0)
     {
        print MAIL "Cannot rename $shreport to $shreportToday.\n";
        return 0;
     }
     if (system("mv -f $summary $summaryToday") != 0)
     {
        print MAIL "Cannot rename $summary to $summaryToday.\n";
        return 0;
     }
     if (system("mv -f $shunittestlog $shunittestlogToday") != 0)
     {
        print MAIL "Cannot rename $shunittestlog to $shunittestlogToday.\n";
        return 0;
     }
     if (system("mv -f $logsToday $logFileWritePath ") != 0)
     {
        print MAIL "Cannot mv $logsToday $logFileWritePath.\n";
        return 0;
     }
     return 1;     
}

sub CleanUpPerl() {
     $logPerl = $pathPerl . "test_summary.log";
     $logPerlToday = $pathPerl . $today . "logPerl";
     if (system("mv -f $logPerl $logPerlToday") != 0)
     {
        print MAIL "Cannot rename $logPerl to $logPerlToday.\n";
        return 0;
     }
     if (system("mv -f $logPerlToday $logFileWritePath ") != 0)
     {
        print MAIL "Cannot mv $logPerlToday $logFileWritePath.\n";
        return 0;
     }
     return 1;     
}

sub CleanUpBioperl() {
     $logBioperl = $pathBioperl . "biop_test.log";
     $logBioperlToday = $pathBioperl . $today . "logBioperl";
     if (system("mv -f $logBioperl $logBioperlToday") != 0)
     {
        print MAIL "Cannot rename $logBioperl to $logBioperlToday.\n";
        return 0;
     }
     if (system("mv -f $logBioperlToday $logFileWritePath ") != 0)
     {
        print MAIL "Cannot mv $logBioperlToday $logFileWritePath.\n";
        return 0;
     }
     return 1;     
}

sub CleanUpJava() {
     $logJava = $pathJavaResult . "SeqhoundAPITest.html";
     $logJavaToday = $pathJava . $today . "logJava";
     if (system("mv -f $logJava $logJavaToday") != 0)
     {
        print MAIL "Cannot rename $logJava to $logJavaToday.\n";
        return 0;
     }
     if (system("mv -f $logJavaToday $logFileWritePath ") != 0)
     {
        print MAIL "Cannot mv $logJavaToday $logFileWritePath.\n";
        return 0;
     }
     return 1;     
}

######################################################################
#
# grep 'fail' in log files andn write to mail
#
######################################################################
sub GrepFail() {
     $prefix = $logFileWritePath . $today;
     @logs = ("summaryCLocal", "summaryCRemote", "logPerl", "logBioperl", "logJava");

     @faillogs = ("grepfailCLocal.log", "grepfailCRemote.log", "grepfailPerl.log", "grepfailBioperl.log", "grepfailJava.log");

     for $i (0 .. 4) {
         if ($i == 0 || $i == 1) {
            $command = "grep 'FAIL' " . $prefix . $logs[$i]. " > " . $pathRoot . $faillogs[$i];
            system($command);
         }
         elsif ($i == 4) {
            $command = "grep 'Failure' " . $prefix . $logs[$i]. " > " . $pathRoot . $faillogs[$i];
            system($command);
         }
         else {
            $command = "grep 'fail' " . $prefix . $logs[$i]. " > " . $pathRoot . $faillogs[$i];
            system($command);
         }
     }

     open(FAILLOGMASTER, ">$fullFailLogMaster") or return 0;
     for $i (0 .. 4) {
        open(FAILLOG, $faillogs[$i]);
        print FAILLOGMASTER "\n\n------------------ $logs[$i]  ---------------------------\n";
        while (<FAILLOG>) {
           $_ =~ s/$logFileWritePath//; 
           print FAILLOGMASTER $_;
        }
        close(FAILLOG);
     }     
     close(FAILLOGMASTER);

     open(FAILLOGMASTER, "$fullFailLogMaster") or return 0;
     while (<FAILLOGMASTER>) {
        print MAIL $_;
     }
     close(FAILLOGMASTER);

     for $i (0 .. 4) {
         $command = "rm " . $pathRoot . $faillogs[$i];
         system($command);
     }
     
    if (system("mv -f $fullFailLogMaster $logFileWritePath ") != 0)
    {
       print MAIL "Cannot mv $fullfaillogmaster $logFileWritePath.\n";
       return 0;
    }

    return 1;
}


# $Log: shunittestcron.pl,v $
# Revision 1.8  2005/03/31 20:49:13  zwang
# Revised after new Java API test was deployed.
#
# Revision 1.6  2004/08/13 19:02:11  zwang
# Added new sub to report failed API.
#
# Revision 1.5  2004/08/12 18:12:12  zwang
# Add a parameter to run all drivers or only one based on user's choice.
#
# Revision 1.4  2004/07/14 20:43:51  zwang
# Clean up directory even the system calls fail. Other minor changes.
#
# Revision 1.3  2004/07/05 21:02:20  zwang
# Added test driver for Java.
#
# Revision 1.2  2004/06/30 21:34:02  zwang
# Change to run test drivers for local C and remote C/Perl/Bioperl.
#
# Revision 1.1  2004/06/25 19:46:50  zwang
# New file to run shunittest and create an html page which links to the output files of shunittest.
#
