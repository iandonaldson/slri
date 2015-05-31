#!/usr/bin/perl -w
# Split file- splits a text file every 1000
# lines. Places them in seperate files"
# Two arguments: <filename> to split and number of lines per file.
# Note that the last file will contain a trailing 1000...
# that need to be removed manually.
 

use IO::Handle;
$USAGE="splits <file> to smaller files by <number> of lines\n <filename> <number>\n";
unless (@ARGV){
	print "$USAGE";
	exit;
}
 
$file= $ARGV[0];
$split_size=$ARGV[1];

open (LIST, $file) || die "Cannot open $file.\n";
$count = 0;
$fileNo=1;

while ($line=<LIST>) {
	$filename=$file."split".$fileNo;
	open(TEMP, ">$filename")|| die "Cannot create $filename.\n";

	for($count=0; $count<$split_size; $count++)
	{
		#print to file
		print TEMP "$line";
		#begin comment
		#perl does not handle end-of-file too well
		#(at least on linux) so the last line read is
		#assigned to $line which kills the program.
		#to avoid that, we flush out the buffer at the end.
		#Note that this flushing inserts many "011111..." at the
		#end of the last file. That needs to be removed manually.
		#end comment
		$line = <LIST> || TEMP->autoflush(1);

	}
	print TEMP "$line";
	close(TEMP);
	$fileNo++;
}
close(LIST);

