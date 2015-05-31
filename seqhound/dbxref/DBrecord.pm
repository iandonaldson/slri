package DBrecord;

use strict;
use warnings;

sub new
{
    my $dbrecord = {the_source_db => "xxx",
		    the_isprimary_db => 0, };
    bless( $dbrecord );
    return $dbrecord;
}

sub source_db
{
    my $self = shift();
    $self->{the_source_db} = shift() if (@_);
    return $self->{the_source_db};
}

sub isprimary_db
{
    my $self = shift();
    $self->{the_isprimary_db} = shift() if (@_);
    return $self->{the_isprimary_db};
}

sub setDBrecord
{
    if ( @_ == 3)
    {
        my $self = shift();
	$self->source_db($_[0]);
	$self->isprimary_db($_[1]);
    }
    else
    {
        print("Method setDBrecord requires 2 (two) arguments.\n");
    }
}

sub print
{
    my $self = shift();
    print($self->source_db);
    print("\t");
    print($self->isprimary_db);
    print("\n");
}

return 1;
