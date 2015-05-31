# $Id: Tee.pm,v 1.2 2004/12/21 20:05:50 kzheng Exp $
package Tie::Tee;
sub TIEHANDLE { 
	my $class   = shift; 
	my $handles = [@_]; 
	bless $handles, $class;
	return $handles;
}
sub PRINT {
	my $href = shift;
	my $handle;
	my $success = 0; 
	foreach $handle (@$href) { 
		$success += print $handle @_;
	} 
	return $success == @$href;
}
1;

	
