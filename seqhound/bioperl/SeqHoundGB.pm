# $Id: SeqHoundGB.pm,v 1.6 2005/04/22 15:04:39 ryao Exp $
#**************************************************************************
package Bio::SeqHound::SeqHoundGB;
use strict;
use warnings;
use Bio::SeqHound::_SeqHoundI;
use Bio::SeqIO;
use vars qw($VERSION @ISA @EXPORTER @EXPORTER_OK);
use Exporter;
use AutoLoader qw(AUTOLOAD);

our @ISA = qw(Exporter);

our %EXPORT_TAGS = ( 'all' => [ qw() ] );

our @EXPORT_OK = ( @{ $EXPORT_TAGS{'all'} } );

our @EXPORT = qw();
our $VERSION = '1.01';

# This object implement stores the information parsed from a genbank
# flat file.  

# Preloaded methods go here.

sub new {
    my $class = shift;
    my $self = {};
    bless ($self, $class);
    $self->_init(@_);
    return $self;
}

sub _init{
    my $self = shift;
    my $arr = shift;
    @{ $self->{key} } = $arr;
    $self->{ret} = [];
}

sub setValues
{
    my $self = shift;
    my $arr = shift;
    $self->{values} = [];
    @{ $self->{values} } = @$arr;
}

sub mergeValues
{
    my $self = shift;
	my $val = shift;
	if (defined $val){
        push @{$self->{ret}} , $val;
	}
}

sub print_to_file{

    # this write_seq does not reproduce the original contents of
    # a genbank flat file, so until that is the case, I've
    # commented out the use of the SeqIO object and simply
    # wrote what SeqHound returned to the file
    my $self = shift;
    my $write_file = shift;
    # my $arr = shift;
    # my $out = Bio::SeqIO->new('-file' => ">$write_file", '-format' => 'genbank');
    # foreach my $i (@$arr){
    #     $out->write_seq($i);
    # }
    open (OUT, ">$write_file");
    my $i = 0;
    foreach my $val (@{$self->{ret}}){
      print OUT $val; 
    }
}

sub getValues 
{
   my $self = shift;
   my $size = @{$self->{ret}};
   if ($size>0){
       return @{ $self->{ret} };
   }else {return undef;}
}

sub getKey
{
    my $self = shift;
    return @{ $self->{key} };
}

sub DESTROY
{
    my $self = shift;
    # this method does nothing, but by explicitly
    # coding it, garbage collection is cleaner
}

1;
__END__

=head1 NAME

Bio::SeqHound::SeqHoundGB - Perl extension for SeqHound
This module is not used by the end user, consult SeqHound instead.

=head1 SYNOPSIS

  This module is not intended for the end user, use SeqHound instead.
  Consult Bio::SeqHound::SeqHound for more details.

=head1 USAGE

  Does not apply. See the accompanying documentation.

=head1 DESCRIPTION

This module is a variant of the Bio::SeqIO::genbank module written by
Elia Stupka, with modifications to work in memory, instead of on a 
genbank formatted file.

Only some of the functions in the original Bio::SeqIO::genbank are included in
here because this module only contains the ability to convert a genbank
stream to a Seq object, not vice versa. 

Consult Bio::Seq for Seq functionalities. 

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
 Bio::Seq
 perl manpage


=cut
#
# $Log: SeqHoundGB.pm,v $
# Revision 1.6  2005/04/22 15:04:39  ryao
# modified mergeValues() and getValues() to better handle the error or null return cases. Updated before integrating SOAP service
#
# Revision 1.5  2004/07/07 18:30:04  rfarrall
# changes to documentation only
# spelling corrections, updates to contact and web information
# [bug 2533]
#
# Revision 1.4  2004/06/30 21:00:15  hlieu
# major refactor, pre-release, remove objects for Fasta, ASN, GB2Seq, _SeqHoundParser, replace with Bio::Seq code and object
#
# Revision 1.3  2004/06/29 20:31:13  hlieu
# latest fixes
#
# Revision 1.2  2004/06/24 18:16:52  hlieu
# update for BioPerl release, bug 2533
#
# Revision 1.1.1.3  2003/01/21 09:32:18  root
# Automated import from Sourceforge.net
#
# Revision 1.3  2003/01/20 15:50:52  haocl
# added sample code and modifications to simplify interface
#
# Revision 1.2  2002/10/22 22:13:21  haocl
# updated files and added testing functions
#
# Revision 1.1  2002/10/07 20:07:10  haocl
# removing files and adding files to reflect new design
#
# Revision 1.2  2002/09/27 21:47:23  haocl
# added more documentation and $Id: SeqHoundGB.pm,v 1.6 2005/04/22 15:04:39 ryao Exp $ and $Log: SeqHoundGB.pm,v $
# added more documentation and $Id: SeqHoundGB.pm,v 1.5 2004/07/07 18:30:04 rfarrall Exp $ and Revision 1.6  2005/04/22 15:04:39  ryao
# added more documentation and $Id: SeqHoundGB.pm,v 1.5 2004/07/07 18:30:04 rfarrall Exp $ and modified mergeValues() and getValues() to better handle the error or null return cases. Updated before integrating SOAP service
# added more documentation and $Id: SeqHoundGB.pm,v 1.5 2004/07/07 18:30:04 rfarrall Exp $ and
# added more documentation and $Id: SeqHoundGB.pm,v 1.6 2005/04/22 15:04:39 ryao Exp $ and Revision 1.5  2004/07/07 18:30:04  rfarrall
# added more documentation and $Id: SeqHoundGB.pm,v 1.6 2005/04/22 15:04:39 ryao Exp $ and changes to documentation only
# added more documentation and $Id: SeqHoundGB.pm,v 1.6 2005/04/22 15:04:39 ryao Exp $ and spelling corrections, updates to contact and web information
# added more documentation and $Id: SeqHoundGB.pm,v 1.6 2005/04/22 15:04:39 ryao Exp $ and [bug 2533]
# added more documentation and $Id: SeqHoundGB.pm,v 1.6 2005/04/22 15:04:39 ryao Exp $ and
# added more documentation and $Id: SeqHoundGB.pm,v 1.6 2005/04/22 15:04:39 ryao Exp $ and Revision 1.4  2004/06/30 21:00:15  hlieu
# added more documentation and $Id: SeqHoundGB.pm,v 1.6 2005/04/22 15:04:39 ryao Exp $ and major refactor, pre-release, remove objects for Fasta, ASN, GB2Seq, _SeqHoundParser, replace with Bio::Seq code and object
# added more documentation and $Id: SeqHoundGB.pm,v 1.6 2005/04/22 15:04:39 ryao Exp $ and
# added more documentation and $Id: SeqHoundGB.pm,v 1.6 2005/04/22 15:04:39 ryao Exp $ and Revision 1.3  2004/06/29 20:31:13  hlieu
# added more documentation and $Id: SeqHoundGB.pm,v 1.6 2005/04/22 15:04:39 ryao Exp $ and latest fixes
# added more documentation and $Id: SeqHoundGB.pm,v 1.6 2005/04/22 15:04:39 ryao Exp $ and
# added more documentation and $Id: SeqHoundGB.pm,v 1.6 2005/04/22 15:04:39 ryao Exp $ and Revision 1.2  2004/06/24 18:16:52  hlieu
# added more documentation and $Id: SeqHoundGB.pm,v 1.6 2005/04/22 15:04:39 ryao Exp $ and update for BioPerl release, bug 2533
# added more documentation and $Id: SeqHoundGB.pm,v 1.6 2005/04/22 15:04:39 ryao Exp $ and
# added more documentation and $Id: SeqHoundGB.pm,v 1.6 2005/04/22 15:04:39 ryao Exp $ and Revision 1.1.1.3  2003/01/21 09:32:18  root
# added more documentation and $Id: SeqHoundGB.pm,v 1.6 2005/04/22 15:04:39 ryao Exp $ and Automated import from Sourceforge.net
# added more documentation and $Id: SeqHoundGB.pm,v 1.6 2005/04/22 15:04:39 ryao Exp $ and
# added more documentation and $Id: SeqHoundGB.pm,v 1.6 2005/04/22 15:04:39 ryao Exp $ and Revision 1.3  2003/01/20 15:50:52  haocl
# added more documentation and $Id: SeqHoundGB.pm,v 1.6 2005/04/22 15:04:39 ryao Exp $ and added sample code and modifications to simplify interface
# added more documentation and $Id: SeqHoundGB.pm,v 1.6 2005/04/22 15:04:39 ryao Exp $ and
# added more documentation and $Id: SeqHoundGB.pm,v 1.6 2005/04/22 15:04:39 ryao Exp $ and Revision 1.2  2002/10/22 22:13:21  haocl
# added more documentation and $Id: SeqHoundGB.pm,v 1.6 2005/04/22 15:04:39 ryao Exp $ and updated files and added testing functions
# added more documentation and $Id: SeqHoundGB.pm,v 1.6 2005/04/22 15:04:39 ryao Exp $ and
# added more documentation and $Id: SeqHoundGB.pm,v 1.6 2005/04/22 15:04:39 ryao Exp $ and Revision 1.1  2002/10/07 20:07:10  haocl
# added more documentation and $Id: SeqHoundGB.pm,v 1.6 2005/04/22 15:04:39 ryao Exp $ and removing files and adding files to reflect new design
# added more documentation and $Id: SeqHoundGB.pm,v 1.6 2005/04/22 15:04:39 ryao Exp $ and
#
#
