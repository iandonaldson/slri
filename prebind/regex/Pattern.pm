# $Id $

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
#          Kai Zheng 
#
#
#  REVISION/CONTRIBUTION NOTES:
#
#  June 1, 2004  Original MSH Revision.
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
#  DESCRIPTION:
#                potential protein-protein interaction regex pattern
#
#******************************************************************************************

package Pattern;

use strict;
use vars qw($VERSION @ISA @EXPORT @EXPORT_OK);

use Exporter;

@ISA = qw(Exporter);
# Items to export into the callers space

@EXPORT = qw(
	PatternNew PatternAddMatch PatternAddGoodMatch
);

# Optional exports
@EXPORT_OK = qw($server $path);
$VERSION = '2.00';


sub PatternNew { 
    # 11 fields here, but some are not in use yet
	my ($class, $id, $score, $precision, $frequency, $style, $option, $regex);
	my($match, $goodMatch, $recall ) ;
    ($class, $id, $score, $style, $option, $regex) = @_;
    $frequency = 0.0;
    $match = 0;
    $goodMatch=0;
    $precision = 0.0;
    $recall= 0.0; 
    my %pattern = (
        "class"  => $class,
        "id" => $id,
        "score" => $score,
        "precision" => $precision,
        "frequency" => $frequency,
        "style" => $style,
        "option" => $option,
        "regex" => $regex,
        "match" => $match,
        "goodMatch" => $goodMatch,
        "recall" => $recall
    );
    return \%pattern;  # return a reference to a local object
}

# a pattern is matched in a sentence
sub PatternAddMatch{
   my $pattern = shift;     # shift is pop()
   $pattern->{"match"}++ ;
}

# used when input data are curated
# if prediction matches curated result, increment true match
sub PatternAddGoodMatch{ 
	my $pattern = shift;
   $pattern->{"goodMatch"}++ ;
}

__END__

=head1 NAME

	REGEX Pattern for potential protein-protein interaction

=head1 SYNOPSIS

  	The subroutines that are available in this module are (in no particular order):
	PatternNew
	PatternAddMatch
	PatternAddGoodMatch

=head1 DESCRIPTION

=head2 A Usage Example

The subroutine SHoundAccFromGi requires one parameter a GI identifier; it returns an accession number.
In your perl script you should have:

    #!/usr/bin/perl
    use Pattern;

    # Other stuff may go here
    # Some more stuff
    # Then to make the function call...
    $pattern = PatternNew(1, 1101, 0.6, 0.7, 0, 0, "A interacts B");
	PatternAddMatch($pattern);
    PatternAddGoodMatch($pattern); 
   
    # Other stuff in script


	=head1 RETURN VALUE

If the parameters are valid for the subroutine, each subroutine returns an
appropriate value.  If there is not result return value is 0 and error message
is logged.

=head1 ERROR

If the parameter(s) are invalid for the subroutine, each subroutine will print 
to screen an error message giving some description of the error.

=head1 AUTHORS

Kai Zheng

=head1 SEE ALSO

perl(1). 
http://bioinfo.mshri.on.ca/seqhound/perlapi.html

=cut

	



