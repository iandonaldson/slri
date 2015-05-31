#!/usr/bin/perl

$fullpath = "/seqhound/build/updates";
$flagpath = "/seqhound/build/flags";
$llinkpath = "/hibbert-local/shoundtest/5.lldb.files";
$gopath = "/hibbert-local/shoundtest/4.godb.files";

# Create a log file for all cron update scripts to log results
my $updatelog =$fullpath . "/updateresult.log";

if (-f $updatelog)
{
  system ("rm $updatelog");
}

open TOUCH, ">$updatelog"; close TOUCH;
 

	system ( "cd $fullpath" );
	system ( "$fullpath/dupdcron.pl" );
	system ( "$fullpath/redundcron.pl" );
	system ( "$fullpath/precomcron.pl" );
	system ( "$fullpath/taxdbcron.pl" );
	#system ( "$llinkpath/locuslinkcron.pl" );
	system ( "$gopath/godbcron_cb.pl" );


# SK: In case of disabling of redundcron.pl you need to decomment next line
#     and comment second next line
#	if ( -f "$flagpath/dupdcron.flg" )
	#if ( -f "$flagpath/dupdcron.flg" && -f "$flagpath/redundcron.flg" && -f "$flagpath/taxdbcron.flg" && -f "$flagpath/locuslinkcron.flg" && -f "$flagpath/godbcron.flg")
	if ( -f "$flagpath/dupdcron.flg" && -f "$flagpath/redundcron.flg" && -f "$flagpath/taxdbcron.flg" && -f "$flagpath/godbcron.flg")
	{
		system ( "$fullpath/rpsdbTransferCron.pl ");
		system ( "$fullpath/mybackup.pl hibbert &" );
		unlink ( "$flagpath/dupdcron.flg" );
		unlink ( "$flagpath/redundcron.flg" );
		unlink ( "$flagpath/taxdbcron.flg" );
		unlink ( "$flagpath/locuslinkcron.flg" );
		unlink ( "$flagpath/godbcron.flg" );
	}
	if ( -f "$flagpath/precompute.flg" )
        {
          	unlink ("$flagpath/precompute.flg" );
    	}
#VG: The cron job isshoundon.pl can be run after everything else is done
	system ( "$fullpath/isshoundoncron.pl" );
