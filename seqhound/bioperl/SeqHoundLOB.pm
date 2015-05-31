# $Id: SeqHoundLOB.pm,v 1.4 2005/04/22 15:04:39 ryao Exp $
#****************************************************************************
package Bio::SeqHound::SeqHoundLOB;
use vars qw(@ISA);
use 5.006;
use strict;
use Bio::Root::Root;
use Bio::SeqHound::_SeqHoundI;

use Exporter;
use AutoLoader qw(AUTOLOAD);

@ISA = qw(Bio::Root::Root Bio::SeqHound::_SeqHoundI);

our %EXPORT_TAGS = ( 'all' => [ qw() ] );

our @EXPORT_OK = ( @{ $EXPORT_TAGS{'all'} } );
our @EXPORT = qw();
our $VERSION = '1.01';

# Preloaded methods go here.
sub new 
{
    my $class = shift;
    my $self = {};
    bless ($self, $class);
    $self->_init(@_);
    return $self;
}

sub _init
{
    my $self = shift;
    my $arr = shift;
    @{ $self->{key} } = @$arr;
    # initialize a hash of arrays
    $self->{val} = [];
}

sub getKeys
{
    my $self = shift;
    return @{$self->{key}};
}

sub mergeValues
{
   my $self = shift;
   my $val = shift;
   if (defined $val){
       # add another element to the array in our hash
       push @{ $self->{val} } , $val;
   }
}

sub getValues
{
    my $self = shift;
	my $size = @{$self->{val}};
	if ($size>0) {
        # return a reference to our array of values
        return @{$self->{val}};
	}else {return undef};
}

sub DESTROY {
    my $self = shift;
    # this method doesn't do anything, but by
    # explicitly defining it, garbage collection 
    # is cleaner
}


1;
__END__

=head1 NAME

Bio::SeqHound::SeqHoundLOB - Perl helper class for SeqHound
This module is not used by the end user, consult SeqHound.

=head1 SYNOPSIS

  This module is not intended for the end user, use SeqHound instead.
  Consult Bio::SeqHound::SeqHound for more details.

=head1 USAGE

The end user does not need to invoke this class.

=head1 DESCRIPTION

This module is a helper class for Bio::SeqHound::SeqHound. SeqHound 
methods that retrieve ASN type data return a SeqHoundLOB object 
which contains methods to retrieve the keys and data values. 

Implements Bio::SeqHound::_SeqHoundI interface

=head1 METHODS

=head2 getKey

 returns a reference to an array containing the key(s) used in the 
 SeqHound method. Usage as shown above.

=head2 getValues

 returns a reference to an array contain the values returned 
 by SeqHound, usage as shown above.

=head2 EXPORT

None by default.

=head1 FEEDBACK/BUGS

seqhound@blueprint.org

=head1 WEBSITE

For more information on SeqHound
http://www.blueprint.org/seqhound/

=head1 DISCLAIMER

This software is provided 'as is' without warranty of any kind.

=head1 AUTHOR

Hao Lieu,
The Blueprint Initiative 
Samuel Lunenfeld Research Institute
Mount Sinai Hospital
E<lt>seqhound@blueprint.orgE<gt>

=head1 SEE ALSO

 Bio::SeqHound::SeqHound
 Bio::SeqHound::_SeqHoundI
 Bio::SeqHound::SeqHoundTextOb
 perl manpage

=cut
#
# $Log: SeqHoundLOB.pm,v $
# Revision 1.4  2005/04/22 15:04:39  ryao
# modified mergeValues() and getValues() to better handle the error or null return cases. Updated before integrating SOAP service
#
# Revision 1.3  2004/07/07 18:30:04  rfarrall
# changes to documentation only
# spelling corrections, updates to contact and web information
# [bug 2533]
#
# Revision 1.2  2004/06/29 20:31:13  hlieu
# latest fixes
#
# Revision 1.1.1.4  2003/01/21 09:32:18  root
# Automated import from Sourceforge.net
#
# Revision 1.4  2003/01/20 15:50:53  haocl
# added sample code and modifications to simplify interface
#
# Revision 1.3  2003/01/07 22:24:38  haocl
# added buffering functions
#
# Revision 1.2  2002/10/22 22:13:21  haocl
# updated files and added testing functions
#
# Revision 1.1  2002/10/07 20:07:10  haocl
# removing files and adding files to reflect new design
#
#
#
