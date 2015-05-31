#!/usr/bin/perl -w

my %backup = (
	"/seqhound/build/datab" => "/seqhound/data/datab",
	"/seqhound/build/mmdb" => "/seqhound/data/mmdb",
	"/seqhound/build/neighbours" => "/seqhound/data/neighbours",
	"/seqhound/build/rps" => "/seqhound/data/rps",
	"/seqhound/build/sendatab" => "/seqhound/data/sendatab",
	"/seqhound/build/precomputed" => "/seqhound/data/precomputed"
);

my %flags = (
	hibbert => "/seqhound/build/flags/krusty.flg",
	krusty => "/seqhound/build/flags/skinner.flg",
	skinner => ""
);


sub startRelink
{
        my $from = shift || die "From path is missing";
        my $to   = shift || die "To path is missing";

	# print LOG "mv $to ${to}_temp\n";
	system "mv $to ${to}_temp";
	# print LOG "ln -s $from $to\n";
	system "ln -s $from $to";
}

sub finishRelink
{
        my $from = shift || die "From path is missing";
        my $to   = shift || die "To path is missing";

	# print LOG "rm $to\n";
	system "rm $to";
	# print LOG "mv ${to}_temp $to\n";
	system "mv ${to}_temp $to";
}

sub copyData
{
        my $from = shift || die "From path is missing";
        my $to   = shift || die "To path is missing";
	my $tempDir = "${to}_temp";
 	# my $tempDir = $to;

	open LST, "ls -1 $from |" or return;

	while ( $file = <LST> )
	{
		chomp $file;
		if ( -f "$tempDir/$file" )
		{
			# print LOG "$file ",  (stat ( "$tempDir/$file"))[9], " < ",  (stat ( "$from/$file" ))[9], "\n";

			if ( (stat ( "$tempDir/$file"))[9] < (stat ( "$from/$file" ))[9] )
			{

				$size = (stat ( "$from/$file" ))[7];
				$start = time;

				if ( system ("dd if=$from/$file of=$tempDir/$file bs=1024k > /dev/null") )
				{
					print LOG " ERROR: Cannot properly copy the file $file from $from to $tempDir\n";
				}
				else
				{
					$done = time;
					$delta_time = ( $done - $start );
					$delta_time = $delta_time > 0 ? $delta_time : 1;

					print LOG "dd $from/$file $tempDir/$file ( $size in $delta_time seconds = " . $size / $delta_time . " bytes/sec\n";
				}
			}
			else
			{
				print LOG "*** File $file hasn't been changed\n";
			}
		}
		else
		{

			$size = (stat ( "$from/$file" ))[7];
			$start = time;

			if ( system ( "dd if=$from/$file of=$tempDir/$file bs=1024k >/dev/null" ) )
			{
				print LOG " ERROR: Cannot properly copy the file $file from $from to $tempDir\n";
			}
			else
			{
				$done = time;
				$delta_time = ( $done - $start );
				$delta_time = $delta_time > 0 ? $delta_time : 1;

				print LOG "*** New file:\ndd $from/$file $tempDir/$file ( $size in $delta_time seconds = " . $size / $delta_time . " bytes/sec\n";
			}
		}
	}
}

	die ("Unspecified machine" ) unless ( defined $flags{$ARGV[0]} );

	if ( $ARGV[0] ne "hibbert" )
	{
		while ( 1 )
		{
			if ( -f "/seqhound/build/flags/$ARGV[0].flg" )
			{
				last; # exit of loop
			}
			print "Going to sleep for 5 minutes...\n";
			sleep ( 300 );
		}
		unlink ( "/seqhound/build/flags/$ARGV[0].flg" );
	}
	else
	{
		unlink ( "/seqhound/build/flags/krusty.flg" );
		unlink ( "/seqhound/build/flags/skinner.flg" );
	}

	@date = localtime();

	open LOG, sprintf ( ">/seqhound/build/updates/logs/backup-%s-%04d%02d%02d.log", $ARGV[0], $date[5] + 1900, $date[4] + 1, $date[3] ) or die;

	foreach $dir ( keys %backup )
	{
		print LOG "Checking accesibility of $dir ...\n";
		unless ( -d $backup{$dir} )
		{
			system "mkdir -p $backup{$dir}";
			print LOG "$backup{$dir} has been created...\n";
		}
	}

	foreach $dir ( keys %backup )
	{
		startRelink ( $dir, $backup{$dir} );
	}

	foreach $dir ( keys %backup )
	{
		print LOG "Processing $dir ...\n";
		copyData ( $dir, $backup{$dir} );
	}

	foreach $dir ( keys %backup )
	{
		finishRelink ( $dir, $backup{$dir} );
	}

	close LOG;

	system "touch $flags{$ARGV[0]}" if $flags{$ARGV[0]};

	if ( $ARGV[0] eq "krusty" )
	{
		system ( "/seqhound/build/updates/startBackup.pl" );
	}
