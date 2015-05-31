# $Id: _SeqHoundI.pm,v 1.1.1.3 2002/10/23 08:33:46 root Exp $
#*************************************************************************
package Bio::SeqHound::_SeqHoundI;
use vars qw(@ISA);
use 5.006;
use strict;
use Bio::Root::RootI;
#use warnings;

use Exporter;
use AutoLoader qw(AUTOLOAD);

@ISA = qw(Bio::Root::RootI);

our %EXPORT_TAGS = ( 'all' => [ qw() ] );

our @EXPORT_OK = ( @{ $EXPORT_TAGS{'all'} } );
our @EXPORT = qw();
our $VERSION = '1.01';

sub getKey
{
    my $self = shift;
    $self->throw_not_implemented();
}

sub getValues
{
    my $self = shift;
    $self->throw_not_implemented();
}

1;
__END__

=head1 NAME

Bio::SeqHound::_SeqHoundI - Perl interface to be implemented by SeqHoundLOB
and SeqHoundTextOb. This module is not used by the end user. Consult
Bio::SeqHound::SeqHound

=head1 SYNOPSIS

  This module is not intended for the end user, use SeqHound instead.
  Consult Bio::SeqHound::SeqHound for more details.

=head1 USAGE

The end user does not need to use this object. See Bio::SeqHound::SeqHound.

=head1 DESCRIPTION

This interface is a helper class for Bio::SeqHound::SeqHound. 
SeqHoundLOB and SeqHoundTextOb objects must implement this interface. 
SeqHound methods return either a SeqHoundLOB or SeqHoundTextOb object.
Since they both have the same interface, end users can treat the 
objects the same way.

=head2 EXPORT

None by default.

=head1 FEEDBACK/BUGS

lieu@mshri.on.ca
hogue@mshri.on.ca

=head1 WEBSITE

For more information on SeqHound and other bioinformatics software
http://bioinfo.mshri.on.ca

=head1 DISCLAIMER

This software is provided 'as is' without warranty of any kind.

=head1 AUTHOR

Hao Lieu
Samuel Luenfeld Research Institute
Mount Sinai Hospital
E<lt>lieu@mshri.on.caE<gt>

=head1 SEE ALSO

 Bio::SeqHound::SeqHound
 Bio::SeqHound::SeqHoundLOB
 Bio::SeqHound::SeqHoundTextOb
 perl manpage

=cut
#
# $Log: _SeqHoundI.pm,v $
# Revision 1.1.1.3  2002/10/23 08:33:46  root
# Automated import from Sourceforge.net
#
# Revision 1.4  2002/10/22 22:13:21  haocl
# updated files and added testing functions
#
# Revision 1.3  2002/10/07 20:07:10  haocl
# removing files and adding files to reflect new design
#
# Revision 1.2  2002/09/27 21:47:23  haocl
# added more documentation and $Id: _SeqHoundI.pm,v 1.1.1.3 2002/10/23 08:33:46 root Exp $ and $Log: _SeqHoundI.pm,v $
# added more documentation and $Id: _SeqHoundI.pm,v 1.4 2002/10/22 22:13:21 haocl Exp $ and Revision 1.1.1.3  2002/10/23 08:33:46  root
# added more documentation and $Id: _SeqHoundI.pm,v 1.4 2002/10/22 22:13:21 haocl Exp $ and Automated import from Sourceforge.net
# added more documentation and $Id: _SeqHoundI.pm,v 1.4 2002/10/22 22:13:21 haocl Exp $ and
# added more documentation and $Id: _SeqHoundI.pm,v 1.1.1.3 2002/10/23 08:33:46 root Exp $ and Revision 1.4  2002/10/22 22:13:21  haocl
# added more documentation and $Id: _SeqHoundI.pm,v 1.1.1.3 2002/10/23 08:33:46 root Exp $ and updated files and added testing functions
# added more documentation and $Id: _SeqHoundI.pm,v 1.1.1.3 2002/10/23 08:33:46 root Exp $ and
# added more documentation and $Id: _SeqHoundI.pm,v 1.1.1.3 2002/10/23 08:33:46 root Exp $ and Revision 1.3  2002/10/07 20:07:10  haocl
# added more documentation and $Id: _SeqHoundI.pm,v 1.1.1.3 2002/10/23 08:33:46 root Exp $ and removing files and adding files to reflect new design
# added more documentation and $Id: _SeqHoundI.pm,v 1.1.1.3 2002/10/23 08:33:46 root Exp $ and
#
#
