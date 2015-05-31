#!/usr/bin/perl

use SeqHound;
use genbankIDConversionService;
use dbxrefService;
use Dumpvalue;
use strict;

SHoundInit ("false", "client.pl");

# call a function supported by SeqHound cgi
print "SHoundLLIDFromGi\n";
my $llid = SHoundLLIDFromGi(4557225);
print "Locuslink id = $llid for gi = 4557225.\n\n";

# call function supported by SeqHound SOAP service
# In order to call SOAP service method, create a service first
my $gb_service = new genbankIDConversionService();

print "SHoundDbNameAndIdListFromGBAcc\n";
my $result = $gb_service ->  SHoundDbNameAndIdListFromGBAcc ('X66994');
print "query GBAcc = 'X66994' and return a ShDbNameAndId object\n";
my $i = -1; 
#$result is an array of ShDbNameAndId
while (ref $result->[++$i] )
{
	print "chain=>", $result->[$i]{'chain'}, "	";
    	print "dbName=>", $result->[$i]{'dbName'}, "	";
    	print "id=>", $result->[$i]{'id'}, "\n";
}


my @acclist =('X88853', 'X66994', 'Z14089');
$result = $gb_service -> SHoundDbNameAndIdListFromGBAccKeyedList (\@acclist);
$i = -1;
print "\nSHoundDbNameAndIdListFromGBAccKeyedList\n";
while (ref $result->[++$i] )
{
    	#$value is an array of ShDbNameAndId
    	my $value = $result->[$i]{'value'};
	#use Dumpvalue;
	#Dumpvalue->new->dumpValue($value);
    	print $result->[$i]{'key'}, "=>", $value, ": ";
   	my $j = -1;
	#$value is an array of ShDbNameAndId
	#ShDbNameAndId is a hash object which has fields 'chain' 'dbName' 'id
	while (ref $value->[++$j]){
		print "chain=>", $value->[$j]->{'chain'}, "	";
    		print "dbName=>", $value->[$j]{'dbName'}, "	";
    		print "id=>", $value->[$j]{'id'}, "\n";
	}
}


my $dbxref_service = new dbxrefService();
$result = $dbxref_service -> SHoundGetDxCvTermList();
print "\nSHoundGetDxCvTermList\n";
#$result is an array of biobean ShDxCvTerm object
#Dumpvalue->new->dumpValue($result);
$i=-1;
print "Get a list of dbxref term :\n";
while (ref $result->[++$i]){
    print "description: ", $result->[$i]{'description'}, "	";
    print "dxCvId     : ", $result->[$i]{'dxCvId'}, "\n";
}


$result = $dbxref_service -> SHoundDxListFromSource("GENE", "1246501", "Col6", undef);
print "\nSHoundDxListFromSource\n";
#Dumpvalue->new->dumpValue($result);
#result is an array of ShDx
$i=-1;
while (ref $result->[++$i]){
    print "cv: ", $result->[$i]{'cv'}, "	";
    print "db: ", $result->[$i]{'db'}, "	";
    print "field: ", $result->[$i]{'field'}, "	";
    print "id: ", $result->[$i]{'id'}, "\n";
}


