# $Id: SLRIMisc.pm,v 1.5 2005/04/22 16:04:34 ryao Exp $
#****************************************************************************
package Bio::SeqHound::SLRIMisc;
use vars qw(@ISA);
use 5.006;
use strict;

use Exporter;
use AutoLoader qw(AUTOLOAD);

@ISA = qw();

our %EXPORT_TAGS = ( 'all' => [ qw() ] );

our @EXPORT_OK = ( @{ $EXPORT_TAGS{'all'} } );
our @EXPORT = qw();
our $VERSION = '1.01';


#for list functions
#separator between query-return pair
my $Q_R = "#~#";
#separator between groups of pair 
my $GROUP = "#;#";

sub Array2String 
{
  my $argArr = shift;
  my $retStr = "";
  foreach my $tmp (@$argArr){
    $retStr = $retStr . $tmp . ",";
    
  }
  # remove the trailing ','
  chop($retStr); 
  return $retStr; 
}

sub MaxSizeArray 
{
  my $argArr = shift;
  my @copyArr;
  my $MAXQ = 5;
  my $len = scalar(@$argArr);
  for(my $i = 0; $i < $len;){
    $copyArr[$i++] = $$argArr[0]; 
    shift(@$argArr);
    if($i == $MAXQ) 
    {
       last;
    }
  }
  return @copyArr;
}

#################################################################
# RY: convert a string to a hashtable for list function handling
# key == query, value == return
#################################################################
sub Str2HashTable
{
    my $self = shift;
    
    my ($result, %hash) = @_;
	
    my @groups = split(/$GROUP/, $result);
	
    foreach my $kv (@groups)
    {
        my @pairs = split(/$Q_R/, $kv, 2);
	   
        $hash{$pairs[0]} = $pairs[1];
	   
    }
	
    return %hash;
}



# Preloaded methods go here.
#sub new 
#{
#    my $class = shift;
#    my $self = {};
#    bless ($self, $class);
#    $self->_init(@_);
#    return $self;
#}

#sub _init
#{
#    my $self = shift;
#    $self->{key} = shift;
#    $self->{ret} = shift;
#}

#sub getKey
#{
#    my $self = shift;
#    my @keys = split(/,/, $self->{key});
#    return \@keys;
#}

#sub getValues
#{
#    my $self = shift;
#    my @arr;
#    push(@arr, $self->{ret});
#    return \@arr;
#}

#sub DESTROY {
#    my $self = shift;
    # this method doesn't do anything, but by
    # explicitly defining it, garbage collection 
    # is cleaner
#}


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
# $Log: SLRIMisc.pm,v $
# Revision 1.5  2005/04/22 16:04:34  ryao
# correction
#
# Revision 1.4  2004/11/02 16:36:07  ryao
# changed the GROUP separator from ';' to '#;#'. bug 3787 related.
#
# Revision 1.3  2004/07/07 18:30:05  rfarrall
# changes to documentation only
# spelling corrections, updates to contact and web information
# [bug 2533]
#
# Revision 1.2  2004/04/05 18:20:39  ryao
# Added helper function for processing keyedlist. Add Str2HashTable.
#
# Revision 1.1.1.2  2003/01/21 09:32:18  root
# Automated import from Sourceforge.net
#
# Revision 1.2  2003/01/20 15:50:52  haocl
# added sample code and modifications to simplify interface
#
# Revision 1.1  2003/01/07 22:24:25  haocl
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
