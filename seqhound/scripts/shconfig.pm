package shconfig;

use strict;
use warnings;

###########################################################
# ReadIntrez:
#
#           Read a tag from .intrezrc.
# 
# @param $tag  Tag to read from the .intrezrc file.
#              
# Return: Value of that tag.
#         0 if tag not found.
sub ReadIntrez
{
  my $tag = $_[1];
  my $retVal = undef;

  my $configFile = ".intrezrc";

  open (INTREZ, "$configFile") || 
	die "Cannot open file $configFile\n";

  if( $tag ne "username" &&
	  $tag ne "password" &&
          $tag ne "database" &&
	  $tag ne "dsn" &&
	  $tag ne "pathupdates" &&
	  $tag ne "pathinputfiles" &
	  $tag ne "pathinputfilescomgen" &&
	  $tag ne "mail" &&
	  $tag ne "defaultrelease" &&
	  $tag ne "pathflags"
	)
  {
	print "Unknown tag: $tag\n";
	return $retVal;
  }

  while( my $line = <INTREZ> )#&& !$retVal )
  {
	 chomp( $line );
	 $line =~ s/\s+//g;

	 my @words = split(/=/, $line);
	 my $attr = $words[0];
	 my $value = $words[1];

	 if ($attr && $attr eq $tag) 
	 {
	   $retVal = $value;

	   print "$tag = $value\n";
	   last;
	 }
  }
  return $retVal;
}

###########################################################
# ReadOdbcIni:
#
# Reads a tag from .odbc.ini. Note: this file requires the
# alias as an input variable, obtained by calling 
# ReadIntrez( "alias" ).
#
# @param $alias Alias from which to read the tags.
#
# @param $tag  Tag to read from the .intrezrc file.
#
sub ReadOdbcIni
{
  my $alias = $_[1];
  my $tag   = $_[2];

  my $homedir    = $ENV{'HOME'};
  my $configODBC = $homedir . "/.odbc.ini";

  open (CONFIGODBC, "$configODBC") || 
	die "Cannot open file $configODBC\n";
	
  my $foundAlias = 0;
  my $gotValue   = 0;
  my $retVal = 0;

  while (<CONFIGODBC>) 
  {
	my $line = $_;
	$line =~ s/\s+//g;
	# find the correct section
	my $target = "[" . $alias . "]";
	if ($line eq $target) 
	{
	  $foundAlias=1;
	}

	# get HOST and PORT from the correct section
	if ($foundAlias == 1) 
	{
	  my @words = split(/=/, $line);
	  my $attr = $words[0];
	  my $value = $words[1];

	  if( $attr eq $tag ) 
	  {
		$retVal = $value;
		$gotValue = 1;
		
		print "$tag = $retVal\n";
	  }
	}
	# finish with the correct section
	if( $gotValue == 1 ) 
	{
	  $foundAlias = 0;
	}
  }

return $retVal;
}

return 1;
