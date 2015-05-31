#! /usr/bin/perl

# common routines used by seqhound perl scripts

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
#   Martin Damsbo (mdamsbo@mdsp.com)
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
# 
#


package SeqHoundCommon;

use Exporter;
@ISA = qw(Exporter);
@EXPORT =
qw(
	$defaultfromdate $default_release $logfile $admail $warning 
	*LOG 
	&TodayDate &DateEpochSeconds &Sys &Flush 
	&VerifyRelease 
	&PreviousUpdateDate &GetNCBIdataHTML &HtmlFilterASN1 &UpdateDB 
	&Abort &MailLog
);


# Directory with the update executables
#$path = "./";
# Default date to update from in case of missing files
$defaultfromdate = "20020101";
# In case database release is out of bounds (out of 100-200) use the default release
$default_release = 130;
# Logging
$logfile = "SeqHound.log"; # Intermediate name, can be changed by application script
*LOG; # global log file handle
# Mail address to receive update report
$admail = "SeqHoundUpdate\@mdsp.com";
# Global warning flag
$warning = 0;


sub TodayDate
{
#	my($year, $month, $day) = @_;
	my($today, $tm);
	use Time::localtime;

	# Construct date specifiers
	$tm = localtime;
	$year = ($tm->year)+1900;
	$month = ($tm->mon)+1;
	$day = $tm->mday;
	$today = sprintf("%04d/%02d/%02d", $year, $month, $day);
	return $today;
}


sub DateEpochSeconds
{ #NWOR
	my($date) = @_;
	use Time::Local;
	$date =~ /(\d+).(\d+).(\d+)/;
	$epochseconds = timelocal(0, 0, 0, $3, ($2 - 1), ($1 - 1900));
	return $epochseconds;
}


sub VerifyRelease
{
	my($release) = @_;

	# Check for validity of release number
	if($release < 100 || $release > 200)
	{
		$release = $default_release;
		print LOG "Release out of range, using default $default_release.\n";
	}
	return $release;
}


sub Sys
{
	my($command, $warninglevel) = @_;
	$rc = system($command);
	$rc &= 0xFFFF;

	if( $rc == 0 )
	{
		return 1;
	}
	elsif( $warninglevel )
	{
		$warning = 1;
		print LOG "System command failed:\n'${command}'\n";
		if( $rc == 0xFF00 )
		{
			print LOG "Did not execute - no exit status\n";
		}
		elsif( $rc > 0x0080 )
		{
			$rc >>= 8;
			print LOG "Exit status = ${rc}\n";
		}
		else
		{
			if( $rc & 0x0080 )
			{
				$rc &= ~0x0080;
				print LOG "[Core dumped] ";
			}
			print LOG "Signal = ${rc}\n";
		}
	}
	return 0;
}


sub Flush
{ # Ugly Perl flush
	my($FH) = @_;
	my $prev_selected = select($FH);
	local $| = 1;  # disable command buffering
	local $\ = ''; # clear output record separator
	print "";      # trigger output flush
	local $| = 0;  # enale command buffering
	select($prev_selected);
}


sub PreviousUpdateDate
{
	my($fulldir, $project) = @_;
	my($prevfile, $datespec, $last_year, $last_month, $last_day);

	# Attempt to find previous update file
	if( $prevfile = glob($fulldir."/".$project."????????.*") )
	{
		$prevfile =~ /(\d{4})(\d{2})(\d{2})/;
		$last_year = $1;
		$last_month = $2;
		$last_day = $3;
	}
	else
	{
		# No previous update file - <from> date is set to last full creation
		if( $prevfile = glob($fulldir."/".$project."????????_full.*") )
		{
			$prevfile =~ /(\d{4})(\d{2})(\d{2})/;
			$last_year = $1;
			$last_month = $2;
			$last_day = $3;
		} 
		else
		{
			# No full creation file either - <from> date is set to default
			print LOG "No earlier file found to deduce previous date - defaulting to $defaultfromdate \n";
			$defaultdate = $defaultfromdate;
			if( $defaultdate =~ /(\d{4})(\d{2})(\d{2})/ )
			{
				$warning = 1;
				$last_year = $1;
				$last_month = $2;
				$last_day = $3;
			}
			else
			{
				print LOG "Bad syntax for default date: $defaultfromdate \n";
			}
		}
	}
	$datespec = sprintf("%04d/%02d/%02d", $last_year, $last_month, $last_day);
	return $datespec;
}


sub GetNCBIdataHTML
{
	my($term, $datespec, $htmlfile, $logfile, $dataformat, $batchsize, $maxentries) = @_;
	if( $dataformat == "" )
	{
		$dataformat = "ASN1";
	}
	if( $batchsize < 1 )
	{
		$batchsize = 200;
	}
	if( $maxentries < 1 )
	{
		$maxentries = 1000000;
	}

	# Download HTML update file from NCBI repository
	#ALT 	Retrieve in one single batch
	#ALT	$syssess = Sys("wget -a $logfile -O $htmlfile -t 9 \"http://www.ncbi.nlm.nih.gov/entrez/query.fcgi?cmd=Search&db=Protein&term=${term}+[Properties]%20AND%20${datespec}%20[PDAT]&DISPLAY&doptcmdl=${dataformat}&dispmax=${maxentries}\"");

	# Retrieve in several batches of size <batchsize>
#	open(HTMLOUT, ">".$htmlfile) || Abort("Cannot open output file '$htmlfile'");
#	close(HTMLOUT);
	$syssess = Sys("wget -a $logfile -O $htmlfile --ignore-length -t 20 \"http://www.ncbi.nlm.nih.gov/entrez/query.fcgi?cmd=Search&db=Protein&term=${term}+[Properties]%20AND%20${datespec}%20[PDAT]&DISPLAY&doptcmdl=${dataformat}&dispmax=${batchsize}&showndisp
max=${batchsize}\"");
	close(LOG); open(LOG, ">>".$logfile); # flush, damnit
	# Assess the total number of pages
	$npages = 0;
	if( $syssess && -s $htmlfile )
	{
		open(HTMLIN, $htmlfile) || Abort("Cannot open input file '$htmlfile'");
		while( <HTMLIN> )
		{
			if( /Page \d+ of (\d+)/ )
			{
				$npages = $1;
				print LOG "Getting ${npages} pages of size ${batchsize} : \n\n";
				Flush(*LOG);
			}
		}
		close(HTMLIN);
		if( $npages < 1 )
		{
			print LOG "No page specification found in HTML file - probably empty result \n";
			$syssess = 0;
		}
	}
	else
	{
		print LOG "NCBI wget query failed \n";
		$warning = 1;
		$syssess = 0;
	}

	$page = 1;
	$htmlfilepage = $htmlfile."page";
	while( $syssess && $page < $npages )
	{
		$syssess = Sys("wget -a $logfile -O $htmlfilepage --ignore-length -t 20 \"http://www.ncbi.nlm.nih.gov/entrez/query.fcgi?cmd=Search&db=Protein&term=${term}+[Properties]%20AND%20${datespec}%20[PDAT]&DISPLAY&doptcmdl=${dataformat}&dispmax=${batchsize}&show
ndispmax=${batchsize}&page=${page}\"");
		Sys("cat ${htmlfilepage} >> ${htmlfile}");
		$page++;
	}
	if( -s $htmlfilepage )
	{
		unlink $htmlfilepage;
	}

	if( $syssess && -s $htmlfile )
	{
		return 1;
	}
	else
	{
		return 0;
	}
}


sub HtmlFilterASN1
{
	my($htmlfile, $txtfile) = @_;
	my($line, $printing, $IN, $OUT);

	$printing = 0;
	$entries = 0;
	open(IN, $htmlfile) || Abort("Input file '$htmlfile' not found");
	open(OUT, ">".$txtfile) || Abort("Cannot open output '$txtfile'");
	# Wrap as Bioseq-set
	print OUT "Bioseq-set ::= { seq-set { \n";
	while( <IN> )
	{
		# Remove HTML tags from data, 
		# by printing only the Seq-entry section between <pre> and </pre> tags, 
		# subject to further modifications
		$line = $_;
		if( /<pre>Seq-entry/ )
		{
			# Entering data section
			$line =~ s/.*<pre>//;
			$printing = 1;
		}
		elsif( /<\/pre>/ )	
		{
			# Exiting data section
			$printing = 0;
		}
	
		if( $printing )
		{
			# Substitute possible Seq-entry with comma
			if( $line =~ s/Seq-entry ::=/,/ )
			{
				if( $entries < 1 )
				{
					# Remove initial comma (substituted Seq-entry)
					$line =~ s/,//;
				}
				$entries++;
			}
			print OUT $line;
		}
	}
	print OUT "\n} }\n";
	close(OUT);
	close(IN);
	return $entries;
}


sub UpdateDB
{
	my($fullupdate, $fullupdmother, $txtfile, $release) = @_;

	# Update database by parsing data
	print LOG "Updating $txtfile \n";
	if( system("$fullupdate -i $txtfile") != 0 )
	{
		abort("Cannot update $txtfile (using $fullupdate) \n");
	}
	print LOG "Entries deleted - Adding $entries new entries... \n";
	if( system("$fullupdmother -i $txtfile -r $release -n F -m F -u T ") != 0 )
	{
		abort("Cannot update $txtfile (using $fullupdmother) \n");
	}
}


sub Abort
{
	my($message, $severity) = @_;
	$severity = 5 unless ( defined $severity ); # severity default value

	print LOG $message."\n";
	close(LOG);
	if( $severity )
	{
		MailLog($logfile);
	}
	die $message;	
}


sub MailLog
{
	my($logfile) = @_;

	$mailpipe = "| mail ".$admail;
	open(MAIL, $mailpipe);
	open(MAILLOG, $logfile);
	while( <MAILLOG> )
	{
		print MAIL $_;
	}
	close(MAILLOG);
	close(MAIL);
}


return 1;
