# $Id: SeqHound.pm,v 1.33 2005/04/22 15:23:26 ryao Exp $
#****************************************************************
# Bio::SeqHound::SeqHound 
# POD documentation - main docs before the code

=head1 NAME

Bio::SeqHound::SeqHound - Perl extension for SeqHound in the
Bioperl project. 

=head1 DESCRIPTION

SeqHound is intended to be a high performance server platform for
bioinformatics research. SeqHound is over 600GB (and growing) of high
quality annotated biological data that researchers can use to support
their in-house research. It is a collection of biological sequences,
taxonomy, annotation and 3-D structure database system.

SeqHound.pm is a BioPerl module to support remote access to SeqHound
through a collection of well-defined API functions. Users can develop
sophisticated applications that retrieve data using the API and then
process those raw data as appropriate.

    use Bio::SeqHound::SeqHound;

SeqHound is also available in C++, C, Java and Perl. You can also
build SeqHound locally on your internal servers. See
http://www.blueprint.org/seqhound/seqhound.html for details.

The SeqHound.pm can be converted to various formats such as html or man
pages using POD utilities such as pod2html and pod2man. See Perl
documentation for usage.
    
In SYNOPSIS, an example code for getting started is provided.

=head1 SYNOPSIS

  use strict;
  use Bio::SeqHound::SeqHound;

  my $seqhound = new Bio::SeqHound::SeqHound("seqhound.blueprint.org", "cgi-bin/seqrem");
  my $test = $seqhound->SHoundInit("TRUE", "bioperl");
  my @reply = $test->getValues();
  foreach my $i (@reply){
  	print $i, "\n";
  }

=cut

=head1 FEEDBACK/BUGS

seqhound@blueprint.org

=head1 WEBSITE

For more information on SeqHound
http://www.blueprint.org/seqhound/

=head1 DISCLAIMER

This software is provided 'as is' without warranty of any kind.

=head1 AUTHOR

Hao Lieu
E<lt>seqhound@blueprint.orgE<gt>

Rong Yao
E<lt>seqhound@blueprint.orgE<gt>

Chris Hogue
E<lt>hogue@mshri.on.caE<gt>

=head1 SEE ALSO

README file accompanying the Bio::SeqHound module.
perl manpage

=cut

package Bio::SeqHound::SeqHound;
use vars qw(@ISA);
use 5.006;
use strict;
use warnings;
use Bio::Root::Root;
use Bio::SeqIO;
use Bio::SeqHound::SeqHoundLOB;
use Bio::SeqHound::SeqHoundTextOb;
use Bio::SeqHound::SeqHoundGB;
use Bio::SeqHound::SLRIMisc;
use LWP::UserAgent;
use Exporter;
use AutoLoader qw(AUTOLOAD);
use POSIX qw(strftime);


our @ISA = qw(Bio::Root::Root Exporter AutoLoader);

our %EXPORT_TAGS = ( 'all' => [ qw(
	
) ] );

our @EXPORT_OK = ( @{ $EXPORT_TAGS{'all'} } );

our $VERSION = '1.01';


# Preloaded methods go here.

sub new {
    my $class = shift;
    my $self = {};
    bless ($self, $class);
    $self->_init(@_);
    return $self;
}

sub _init 
{
    my ($self, $server, $path) = @_;
    $self->{server} = "http://".$server."/";
    $self->{path} = $path;
    $self->{isInit} = "FALSE";
    $self->{SLRI_taxon_name_name_class_none} = 0;
    $self->{SLRI_taxon_name_name_class_acronym} = 1;
    $self->{SLRI_taxon_name_name_class_anamorph} = 2;
    $self->{SLRI_taxon_name_name_class_blast_name} = 3;
    $self->{SLRI_taxon_name_name_class_common_name} = 4;
    $self->{SLRI_taxon_name_name_class_equivalent_name} = 5;
    $self->{SLRI_taxon_name_name_class_includes} = 6;
    $self->{SLRI_taxon_name_name_class_in_part} = 7;
    $self->{SLRI_taxon_name_name_class_misnomer} = 8;
    $self->{SLRI_taxon_name_name_class_misspelling} = 9;
    $self->{SLRI_taxon_name_name_class_preferred_acronym} = 10;
    $self->{SLRI_taxon_name_name_class_preferred_common_name} = 11;
    $self->{SLRI_taxon_name_name_class_scientific_name} = 12;
    $self->{SLRI_taxon_name_name_class_synonym} = 13;
    $self->{SLRI_taxon_name_name_class_teleomorph} = 14;
    $self->{SLRI_taxon_name_name_class_other} = 255;
    
    $self->{SHoundGetTaxLineage_FULL} = 10;
    $self->{SHoundGetTaxLineage_GENBANK} = 20;
    $self->{SHoundGetTaxLineage_TAXDBSHORT} = 30;

    $self->{AM_ION} = 0x80;
    $self->{AM_RNA} = 0x40;
    $self->{AM_WAT} = 0x20;
    $self->{AM_SOL} = 0x10;
    $self->{AM_HET} = 0x08;
    $self->{AM_DNA} = 0x04;
    $self->{AM_PROT} = 0x02;
    $self->{AM_POLY} = 0x01;

    $self->{CHROM_PHAGE} = 0x40;
    $self->{CHROM_NR} = 0x20;
    $self->{CHROM_ECE} = 0x10;
    $self->{CHROM_PLMD} = 0x08;
    $self->{CHROM_CHLO} = 0x04;
    $self->{CHROM_MITO} = 0x02;
    $self->{CHROM_CHROM} = 0x01;
    $self->{CHROM_ALL} = 0xFF;
}

sub _server
{
    my $self = shift;
    return $self->{server};
}

sub _path
{
    my $self = shift;
    return $self->{path};
}

sub get
{
    my $url = shift;
    my $ua = LWP::UserAgent->new();
    my $request = HTTP::Request->new(GET => $url);
    my $response = $ua->request($request);
    if ($response->is_success){
	return $response->content;
    }
    else {
	my $result = "HTTP::Request error ".$response->status_line."\n";
	print "Alert: $url failed.\nError code: $result\n";
	return $result;
    }
}

#For list function handling
#input: a hash table with the string format query-return pair.
#return a hash table with the query (still a string) but the return value associated
#with the query is Bio::SeqIO object or null
sub parse_FastaHash
{
    my $self = shift;
    my %hash = @_;
    my $Delim = "\"";
    my %finHash;
    while (my($key,$val) = each(%hash))
    {
        if ($val ne "NULL")
	{
	    my @arr1 = split (/$Delim/,$val);
	    my $def = $arr1[1];
	    $def =~ s/\n//g;
	    
	    my @arr2 = split (/\n/, $arr1[3]);
	    my $arrSize = @arr2;
	    my $seq;
	    for (my $i =0; $i<$arrSize; $i++)
	    {
	        $seq = $seq.$arr2[$i];
	    }
	    $seq = $seq."\n";
            
            my $fasta = ">".$def."\n".$seq; 
            my $buf = IO::String->new ( $fasta );
            my $io = Bio::SeqIO->new ( -format => 'fasta', -fh => $buf );
            my $seqio = $io->next_seq();
	    $finHash{$key} = $seqio;
	 }
	 else {
	    #store null object
	    $finHash{$key} = 'null';
	 }
    }
    return %finHash;
}

=head1 System Initialization, Finishing And Testing

=head2 SHoundInit

  Title    :  SHoundInit
  Function :  Initializes the SeqHound system; checks if the server is ready for querying.
              NCBI NetEntrez service can be initialized simultaneously if the user desires to
              query obsolete (outdated) sequences.
  Usage    :  $seqhound->SHoundInit($netentrez, $myappname);
  Returns  :  SeqHoundTextOb
              value field: "TRUE" if success, "FALSE" if failure
  Args     :  initialize NetEntrez (acceptable values: "TRUE"/"FALSE")
              your application name (e.g. "myapp")
  Example  :  $test = $seqhound->SHoundInit("TRUE", "myapp");
              @reply = $test->getValues();
	      foreach $i (@reply){
	      	print $i, "\n";
	      }

=cut

sub SHoundInit
{
    my $self = shift;
    my($NetEntrezOnToo, $appname) = @_;
    my @arr = ($NetEntrezOnToo, $appname);
    my $arguments = "NetEntrezOnToo\=$NetEntrezOnToo"."\&appname\=$appname";
    my $fulladdress = $self->_HTTPFunctionCall("SeqHoundInit", $arguments);
    my $content = get($fulladdress);
    $self->{isInit} = $self->_ProcessContent($content, "SHoundInit");
    my $sh = new Bio::SeqHound::SeqHoundTextOb(\@arr); 
    $sh->mergeValues($self->{isInit});
    return $sh;
}


=head2 SHoundIsInited

  Title    :  SHoundIsInited
  Function :  Reports if SeqHound has been initialized.
  Usage    :  $seqhound->SHoundIsInited();
  Returns  :  "TRUE" if success,  "FALSE" if failure
  Args     :  none
  Example  :  $test = $seqhound->SHoundIsInited();

=cut

sub SHoundIsInited
{
    my $self = shift;
    return $self->{isInit};
}

=head2 SHoundIsNetEntrezOn

  Title    :  SHoundIsNetEntrezOn
  Function :  Finds out if NCBI NetEnrez service has been initialized. NCBI NetEntrez service
              can be used to query obsolete (outdated) sequences.
  Usage    :  $seqhound->SHoundIsNetEntrezOn();
  Returns  :  SeqHoundTextOb
              value field: "TRUE" if success,  "FALSE" if failure
  Args     :  none
  Example  :  $test = $seqhound->SHoundIsNetEntrezOn();
              @reply = $test->getValues();

=cut

sub SHoundIsNetEntrezOn
{
    my $self = shift;
    my @arr;
    my $fulladdress = $self->_HTTPFunctionCall("SeqHoundIsNetEntrezOn", "NULL");
    my $content = get($fulladdress);
    my $str = $self->_ProcessContent($content, "SHoundIsNetEntrezOn");
    my $sh = new Bio::SeqHound::SeqHoundTextOb(\@arr);
    $sh->mergeValues($str);
    return $sh;
}


=head2 SHoundNetEntrezInit

  Title    :  SHoundNetEntrezInit
  Function :  Initializes the SeqHound system together with NCBI NetEntrez service. NCBI
              NetEntrez service can be used to query obsolete (outdated) sequences.
  Usage    :  $seqhound->SHoundNetEntrezInit($myappname);
  Returns  :  SeqHoundTextOb
              value field:"TRUE" if success,  "FALSE" if failure
  Args     :  application name
  Example  :  $test = $seqhound->SHoundNetEntrezInit("myapp");
              @reply = $test->getValues();

=cut

sub SHoundNetEntrezInit
{
    my $self = shift;
    my ($appname) = @_;
    my @arr = ($appname);
    my $arguments = "appname\=$appname";
    my $fulladdress = $self->_HTTPFunctionCall("SeqHoundNetEntrezInit", $arguments);
    my $content = get($fulladdress);
    my $str = $self->_ProcessContent($content, "SHoundNetEntrezInit");
    my $sh = new Bio::SeqHound::SeqHoundTextOb(\@arr);
    $sh->mergeValues($str);
    return $sh;
}

=head2 SHoundFini

  Title    :  SHoundFini
  Function :  Closes the SeqHound system and the NCBI NetEntrez service if it has been
              initialized.
  Usage    :  $seqhound->$seqhound->SHoundFini();
  Returns  :  SeqHoundTextOb
              value field: "TRUE" if success,  "FALSE" if failure
  Args     :  none
  Example  :  $test = $seqhound->SHoundFini();
              @reply = $test->getValues();

=cut

sub SHoundFini
{
    my $self = shift;
    my @arr;
    my $fulladdress = $self->_HTTPFunctionCall("SeqHoundFini", "NULL");
    my $content = get($fulladdress);
    my $str = $self->_ProcessContent($content, "SHoundFini");
    my $sh = new Bio::SeqHound::SeqHoundTextOb(\@arr);
    $sh->mergeValues($str);
    return $sh;
}


=head1 Sequence Identifier Conversions


=head2 SHoundFindAcc

  Title    :  SHoundFindAcc
  Function :  Converts an accession number for a sequence into a GenInfo identifier - GI.
  Usage    :  $seqhound->SHoundFindAcc($accno);
  Returns  :  SeqHoundTextOb
              value field: GI identifier (e.g. 4646) or undef if failure
  Args     :  accession number
  Example  :  $test = $seqhound->SHoundFindAcc("CAA28783");
              @reply = $test->getValues();
  Note     :  This function will not accept a version number
              (e.g. NP_116609.1).  The '.1' must be removed from the
              example version number to form a valid accession.
              SeqHound only stores and returns the latest version 
              of a sequence record.

=cut

sub SHoundFindAcc
{
    my $self = shift;
    my ($pcAcc) = @_;
    my @arr = ($pcAcc);
    my $arguments = "acc\=$pcAcc";
    my $fulladdress = $self->_HTTPFunctionCall("SeqHoundFindAcc", $arguments);
    my $content = get($fulladdress);
    my $stri = $self->_ProcessContent($content, "SHoundFindAcc");
    my $sh = new Bio::SeqHound::SeqHoundTextOb(\@arr);
    $sh->mergeValues($stri);
    return $sh;
}


=head2 SHoundFindAccList

  Title    :  SHoundFindAccList
  Function :  Converts a list of accession numbers into a list of GI identifiers.
  Usage    :  $seqhound->SHoundFindAccList(@accnolist);
  Returns  :  SeqHoundTextOb
              value field: list of GIs or undef if failure
  Args     :  comma delimited list of accessions
  Example  :  $test = $seqhound->SHoundFindAccList("CAA28783","X66994");
              @reply = $test->getValues();
  Note     :  The order in the lists is not maintained
              The returning GIs are sorted and the list made unique
           :  This function will not accept a version number
              (e.g. NP_116609.1).  The '.1' must be removed from the
              example version number to form a valid accession.
              SeqHound only stores and returns the latest version 
              of a sequence record.

=cut

sub SHoundFindAccList
{
    my $self = shift;
    my (@pvnAcc) = @_;
    my @copyArr = @pvnAcc;
    my (@tempArr, $sh);
    $sh = new Bio::SeqHound::SeqHoundTextOb(\@pvnAcc);
    while($#copyArr != -1){
      @tempArr = Bio::SeqHound::SLRIMisc::MaxSizeArray(\@copyArr);
      my $Arrstr = Bio::SeqHound::SLRIMisc::Array2String(\@tempArr);
      my $arguments = "pacc\=$Arrstr";
      my $fulladdress = $self->_HTTPFunctionCall("SeqHoundFindAccList", $arguments);
      my $content = get($fulladdress);
      my $str = $self->_ProcessContent($content, "SHoundFindAccList");
      $sh->mergeValues($str);
    }
    return $sh;
}


=head2 SHoundFindName

  Title    :  SHoundFindName
  Function :  Converts PDB, EMBL, Swiss-Prot, PIR sequence identifier into a GI identifier.
  Usage    :  $seqhound->SHoundFindName($seqname);
  Returns  :  SeqHoundTextOb
              value field: GI identifier (e.g. 230940) or undef if failure
  Args     :  sequence name
  Example  :  $test = $seqhound->SHoundFindName("3TS1");
              @reply = $test->getValues();
  Note     :  Note that EMBL shares accession numbers with GenBank. EMBL GenInfo identifiers
              may be returned using these accessions; for example SHoundFindAcc(X13776) will
              return GI 45269.  EMBL also has its own identifiers (which the NCBI and SeqHound
               refer to as 'name').  So the same
              GI may be returned by SHoundFindName(PAAMIR).

=cut

sub SHoundFindName
{
   my $self = shift;
   my ($name) = @_;
   my @arr = ($name);
   my $arguments = "name\=$name";
   my $fulladdress = $self->_HTTPFunctionCall("SeqHoundFindName", $arguments);
   my $content = get($fulladdress);
   my $str = $self->_ProcessContent($content, "SHoundFindName");
   my $sh = new Bio::SeqHound::SeqHoundTextOb(\@arr);
   $sh->mergeValues($str);
   return $sh;
}


=head2 SHoundFindNameList

  Title    :  SHoundFindNameList
  Function :  Converts PDB, EMBL, Swiss-Prot, PIR sequence identifier into a GI identifier.
  Usage    :  $seqhound->SHoundFindNameList(@seqnamelist);
  Returns  :  SeqHoundTextOb
              value field: list of GIs or undef if failure
  Args     :  comma delimited list of sequence names
  Example  :  $test = $seqhound->SHoundFindNameList("PMY14796","S36449","3TS1");
              @reply = $test->getValues();
  Note     :  Note that EMBL shares accession numbers with GenBank. EMBL GenInfo identifiers
              may be returned using these accessions; for example SHoundFindAcc(X13776) will
              return GI 45269.  EMBL also has its own identifiers (which the NCBI and SeqHound
               refer to as 'name').  So the same GI may be returned by SHoundFindName(PAAMIR).
              The order in the lists is not maintained
              The returning GIs are sorted and the list made unique

=cut


sub SHoundFindNameList
{
    my $self = shift;
    my (@pname) = @_;
    my @copyArr = @pname;
    my (@tempArr, $sh);
    $sh = new Bio::SeqHound::SeqHoundTextOb(\@pname);
    while($#copyArr != -1){
      @tempArr = Bio::SeqHound::SLRIMisc::MaxSizeArray(\@copyArr); 
      my $Arrstr = Bio::SeqHound::SLRIMisc::Array2String(\@tempArr);
      my $arguments = "pname\=$Arrstr";
      my $fulladdress = $self->_HTTPFunctionCall("SeqHoundFindNameList", $arguments);
      my $content = get($fulladdress);
      my $str = $self->_ProcessContent($content, "SHoundFindNameList");
      $sh->mergeValues($str);
    }
    return $sh;
}


=head2 SHoundAccFromGi

  Title    :  SHoundAccFromGi
  Function :  Converts a GI identifier to an accession number.
  Usage    :  $seqhound->SHoundAccFromGi($gi);
  Returns  :  SeqHoundTextOb
              value field: accession number (e.g. "X08048") or undef if failure
  Args     :  GI identifier
  Example  :  $test = $seqhound->SHoundAccFromGi(435);
              @reply = $test->getValues();
  Note     :  Sequences from the PIR or PDB databases will return "n/a"
              for this function since they do not have an NCBI accession.
              Instead, use SHoundGetDefline to retrieve the accession for
              these sequences.

=cut

sub SHoundAccFromGi
{
   my $self = shift;
   my ($gi) = @_;
   my @arr = ($gi);
   my $arguments = "gi\=$gi";
   my $fulladdress = $self->_HTTPFunctionCall("SeqHoundAccFromGi", $arguments);
   my $content = get($fulladdress);
   my $str = $self->_ProcessContent($content, "SHoundAccFromGi");
   my $sh = new Bio::SeqHound::SeqHoundTextOb(\@arr);
   $sh->mergeValues($str);
   return $sh;
}


=cut

=head2 SHoundAccFromGiList

  Title    :  SHoundAccFromGiList
  Function :  Converts a list of GI identifiers to a list of accession numbers.
  Usage    :  $seqhound->SHoundAccFromGiList(@gilist);
  Returns  :  SeqHoundTextOb
              value field: list of accessions (e.g. "X08048,Y13670,AB045036")
              or undef if failure
  Args     :  comma delimited list of GI identifiers
  Example  :  $test = $seqhound->SHoundAccFromGiList("123","34634");
              @reply = $test->getValues();
  Note     :  The order in the lists is not maintained
              Sequences from the PIR or PDB databases will return "n/a"
              for this function since they do not have an NCBI accession.
              Instead, use SHoundGetDeflineList to retrieve the accessions
              for these sequences.

=cut

sub SHoundAccFromGiList
{
   my $self = shift;
   my (@pvnGi) = @_;
   my (@tempArr, $sh);
   my @copyArr = @pvnGi;
   $sh = new Bio::SeqHound::SeqHoundTextOb(\@pvnGi);
   while($#copyArr != -1){
     @tempArr = Bio::SeqHound::SLRIMisc::MaxSizeArray(\@copyArr);
     my $Arrstr = Bio::SeqHound::SLRIMisc::Array2String(\@tempArr);
     my $arguments = "pgi\=$Arrstr";
     my $fulladdress = $self->_HTTPFunctionCall("SeqHoundAccFromGiList", $arguments);
     my $content = get($fulladdress);
     my $str = $self->_ProcessContent($content, "SHoundAccFromGiList");
     $sh->mergeValues($str);
   }
    return $sh;
}

=head2 SHoundGetAccFromRetiredGi

  Title    :  SHoundGetAccFromRetiredGi
  Function :  Gets accession from a retired GI
  Usage    :  $seqhound->SHoundGetAccFromRetiredGi($gi);
  Returns  :  SeqHoundTextOb
              value field: accession number or undef if failure
  Args     :  GI identifier
  Example  :  $test = $seqhound->SHoundGetAccFromRetiredGi(4557628);
              @reply = $test->getValues();

=cut

sub SHoundGetAccFromRetiredGi
{
   my $self = shift;
   my ($gi) = @_;
   my @arr = ($gi);
   my $arguments = "gi\=$gi";
   my $fulladdress = $self->_HTTPFunctionCall("SeqHoundGetAccFromRetiredGi", $arguments);
   my $content = get($fulladdress);
   my $str = $self->_ProcessContent($content, "SHoundGetAccFromRetiredGi");
   my $sh = new Bio::SeqHound::SeqHoundTextOb(\@arr);
   $sh->mergeValues($str);
   return $sh;
}

=head2 SHoundGetNameByGi

  Title    :  SHoundGetNameByGi
  Function :  Retrieves the Protein Data Bank (PDB) code from the given GI
  Usage    :  $seqhound->SHoundGetNameByGi($gi);
  Returns  :  SeqHoundTextOb
              value field: PDB code (e.g. 3TS1) or undef if failure
  Args     :  GI identifier
  Example  :  $test = $seqhound->SHoundGetNameByGi(230940);
              @reply = $test->getValues();

=cut

sub SHoundGetNameByGi
{
   my $self = shift;
   my ($gi) = @_;
   my @arr = ($gi);
   my $arguments = "gi\=$gi";
   my $fulladdress = $self->_HTTPFunctionCall("SeqHoundGetNameByGi", $arguments);
   my $content = get($fulladdress);
   my $str = $self->_ProcessContent($content, "SHoundGetNameByGi");
   my $sh = new Bio::SeqHound::SeqHoundTextOb(\@arr);
   $sh->mergeValues($str);
   return $sh;
}

=head2 SHoundSeqIdFromGi

  Title    :  SHoundSeqIdFromGi
  Function :  Converts GI identifier into an ASN.1 structure SeqId which contains all
              available identifiers for a given sequence.
  Usage    :  $seqhound->SHoundSeqIdFromGi($gi);
  Returns  :  SeqHoundLOB
              value field: SeqId ASN.1 in text form e.g.
                       "SLRISeq-id ::= {
                               embl {
                              name "SCTRNE3A" ,
                              accession "X08048" ,
                              version 1 } ,
                               gi 4646 }"
              or undef if failure
  Args     :  GI identifier
  Example  :  $test = $seqhound->SHoundSeqIdFromGi(432);
              @reply = $test->getValues();

=cut

sub SHoundSeqIdFromGi
{
   my $self = shift;
   my ($gi) = @_;
   my @arr = ($gi);
   my $arguments = "gi\=$gi";
   my $fulladdress = $self->_HTTPFunctionCall("SeqHoundSeqIdFromGiPL", $arguments);
   my $content = get($fulladdress);
   my $str = $self->_ProcessContent($content, "SHoundSeqIdFromGi");
   my $sh = new Bio::SeqHound::SeqHoundLOB(\@arr);
   $sh->mergeValues($str);
   return $sh;
}


=head2 SHoundSeqIdFromGiList

  Title    :  SHoundSeqIdFromGiList
  Function :  Converts a list of GI identifiers into an list of ASN.1 SeqId structures which
              contain all available identifiers for a given sequence.
  Usage    :  $seqhound->SHoundSeqIdFromGiList(@gilist);
  Returns  :  SeqHoundLOB
              value field: Sequence of SeqId structures in ASN.1 text form e.g.
                           "SLRIValNode ::= {
                            seqid {
    				embl {
      				accession "CAA48484" ,
      				version 1 } ,
    				gi 432 } ,
  			    seqid {
   				embl {
      				name "BTMISATB" ,
      				accession "X65203" ,
      				version 1 } ,
			    }
			 }
              or undef if no such record
  Args     :  comma delimited list of GI identifiers
  Example  :  $test = $seqhound->SHoundSeqIdFromGiList("432","543");
              @reply = $test->getValues();

=cut

sub SHoundSeqIdFromGiList
{
   my $self = shift;
   my (@pnvGi) = @_;
   my @copyArr = @pnvGi;
   my (@tempArr, $sh);
   $sh = new Bio::SeqHound::SeqHoundLOB(\@pnvGi);
   while($#copyArr != -1){
     @tempArr = Bio::SeqHound::SLRIMisc::MaxSizeArray(\@copyArr);
     my $Arrstr = Bio::SeqHound::SLRIMisc::Array2String(\@tempArr);
     my $arguments = "pgi\=$Arrstr";
     my $fulladdress = $self->_HTTPFunctionCall("SeqHoundSeqIdFromGiListPL", $arguments);
     my $content = get($fulladdress);
     my $str = $self->_ProcessContent($content, "SHoundSeqIdFromGiList");
     $sh->mergeValues($str);
   }
   return $sh;
}


=head1 Sequence and Structure Checks

=head2 SHoundMoleculeType

  Title    :  SHoundMoleculeType
  Function :  Gets molecule type for a given GI identifier
  Usage    :  $seqhound->SHoundMoleculeType($gi);
  Returns  :  SeqHoundTextOb
              value field: molecule type (dna, rna, protein) or undef if failure
  Args     :  GI identifier
  Example  :  $test = $seqhound->SHoundMoleculeType(6322016);
              @reply = $test->getValues();
  Note     :  This function deprecates SHoundIsProtein

=cut

sub SHoundMoleculeType
{
   my $self = shift;
   my ($gi) = @_;
   my @arr = ($gi);
   my $arguments = "gi\=$gi";
   my $fulladdress = $self->_HTTPFunctionCall("SeqHoundMoleculeType", $arguments);
   my $content = get($fulladdress);
   my $str = $self->_ProcessContent($content, "SHoundMoleculeType");
   my $sh = new Bio::SeqHound::SeqHoundTextOb(\@arr);
   $sh->mergeValues($str);
   return $sh;
}

=head2 SHoundIsProtein

  Title    :  SHoundIsProtein
  Function :  Reports whether a given GI identifier denotes a protein (as opposed to DNA or
              RNA).
  Usage    :  $seqhound->SHoundIsProtein($gi);
  Returns  :  SeqHoundTextOb
              value field: "TRUE" if it is protein, "FALSE" if it is not
  Args     :  GI identifier
  Example  :  $test = $seqhound->SHoundIsProtein(354);
              @reply = $test->getValues();
  Note     :  This function will be deprecated.  Please use SHoundMoleculeType

=cut

sub SHoundIsProtein
{
   my $self = shift;
   my ($Gi) = @_;
   my $arguments = "gi\=$Gi";
   my @arr = ($Gi);
   my $fulladdress = $self->_HTTPFunctionCall("SeqHoundIsProtein", $arguments);
   my $content = get($fulladdress);
   my $str = $self->_ProcessContent($content, "SHoundIsProtein");
   my $sh = new Bio::SeqHound::SeqHoundTextOb(\@arr);
   $sh->mergeValues($str);
   return $sh;
}

=head2 SHoundSequenceLength

  Title    :  SHoundSequenceLength
  Function :  Gets sequence length from GI identifier
  Usage    :  $seqhound->SHoundSequenceLength($gi);
  Returns  :  SeqHoundTextOb
              value field: sequence length or undef if failure
  Args     :  GI identifier
  Example  :  $test = $seqhound->SHoundSequenceLength(21071030);
              @reply = $test->getValues();

=cut

sub SHoundSequenceLength
{
   my $self = shift;
   my ($gi) = @_;
   my @arr = ($gi);
   my $arguments = "gi\=$gi";
   my $fulladdress = $self->_HTTPFunctionCall("SeqHoundSequenceLength", $arguments);
   my $content = get($fulladdress);
   my $str = $self->_ProcessContent($content, "SHoundSequenceLength");
   my $sh = new Bio::SeqHound::SeqHoundTextOb(\@arr);
   $sh->mergeValues($str);
   return $sh;
}

=head2 SHoundIsNRFirst

  Title    :  SHoundIsNRFirst
  Function :  Reports if a given GI identifier represents the best annotated version from a
              group of database entries referring to the same protein sequence (so called
              redundant proteins).
  Usage    :  $seqhound->SHoundIsNRFirst($gi);
  Returns  :  SeqHoundTextOb
              value field: "TRUE" if success, "FALSE" if failure
  Args     :  GI protein identifier
  Example  :  $test = $seqhound->SHoundIsNRFirst(34543);
              @reply = $test->getValues();
  Note     :  Usable only for proteins, DNA will always return "FALSE"; generally the PDB,
              RefSeq and Swiss-Prot annotations are considered the most complete


=cut

sub SHoundIsNRFirst
{
   my $self = shift;
   my ($Gi) = @_;
   my $arguments = "gi\=$Gi";
   my @arr = ($Gi);
   my $fulladdress = $self->_HTTPFunctionCall("SeqHoundIsNRFirst", $arguments);
   my $content = get($fulladdress);
   my $str = $self->_ProcessContent($content, "SHoundIsNRFirst");
   my $sh = new Bio::SeqHound::SeqHoundTextOb(\@arr);
   $sh->mergeValues($str);
   return $sh;
}


=head2 SHoundExists

  Title    :  SHoundExists
  Function :  Reports whether a given GI identifier exists in the SeqHound system.
  Usage    :  $seqhound->SHoundExists($gi);
  Returns  :  SeqHoundTextOb
              value field: "TRUE" if success, "FALSE" if failure
  Args     :  GI identifier
  Example  :  $test = $seqhound->SHoundExists(34543);
              @reply = $test->getValues();

=cut

sub SHoundExists
{
   my $self = shift;
   my ($Gi) = @_;
   my $arguments = "gi\=$Gi";
   my @arr = ($Gi);
   my $fulladdress = $self->_HTTPFunctionCall("SeqHoundExists", $arguments);
   my $content = get($fulladdress);
   my $str = $self->_ProcessContent($content, "SHoundExists");
   my $sh = new Bio::SeqHound::SeqHoundTextOb(\@arr);
   $sh->mergeValues($str);
   return $sh;
}

=head2 SHound3DbWhat

  Title    :  SHound3DbWhat
  Function :  Finds out what kind of molecules are contained in a 3-D structure.
  Usage    :  $seqhound->SHound3DbWhat($mmdbid);
  Returns  :  SeqHoundTextOb
              value field: an integer which represents different kinds of molecules
              the return values are so called "byte flags" where each bit is reserved for one
              kind of molecule:
                  1 is polymer
                  2 is protein
                  4 is DNA
                  8 is heteroatom
                 16 is solvent
                 32 is water
                 64 is RNA
                128 is ion
              or undef if failure
  Args     :  Molecular Modeling Database identifier (MMDB ID)
  Example  :  $test = $seqhound->SHound3DbWhat(3436);
              @reply = $test->getValues();

=cut

sub SHound3DbWhat
{
   my $self =shift;
   my ($mmdbid) = @_;
   my $arguments = "mmdbid\=$mmdbid";
   my @arr = ($mmdbid);
   my $fulladdress = $self->_HTTPFunctionCall("SeqHound3DbWhat", $arguments);
   my $content = get($fulladdress);
   my $str = $self->_ProcessContent($content, "SHound3DbWhat");
   my $data = $self->_DecodeBitMask($str);
   my $sh = new Bio::SeqHound::SeqHoundTextOb(\@arr);
   $sh->mergeValues($str);
   return $sh;
}


=head2 SHoundGetLargestMMDBID

  Title    :  SHoundGetLargestMMDBID
  Function :  Gets the largest Molecular Modeling Database identifier (MMDBID) in the current
              MMDB database
  Usage    :  $seqhound->SHoundGetLargestMMDBID();
  Returns  :  SeqHoundTextOb
              value field: largest MMDB ID or undef if failure
  Args     :  none
  Example  :  $test = $seqhound->SHoundGetLargestMMDBID();
              @reply = $test->getValues();

=cut

sub SHoundGetLargestMMDBID
{
    my $self = shift;
    my @arr;
    my $fulladdress = $self->_HTTPFunctionCall("SeqHoundGetLargestMMDBID", "NULL");
    my $content = get($fulladdress);
    my $str = $self->_ProcessContent($content, "SHoundGetLargestMMDBID");
    my $sh = new Bio::SeqHound::SeqHoundTextOb(\@arr);
    $sh->mergeValues($str);
    return $sh;
}


=head2 SHound3DExists

  Title    :  SHound3DExists
  Function :  Finds out whether a given structural identifier (MMDB ID) exists in the system.
  Usage    :  $seqhound->SHound3DExists($mmdbid);
  Returns  :  SeqHoundTextOb
              value field: "TRUE" if exists, "FALSE" if it does not
  Args     :  Molecular Modeling Database identifier (MMDB ID)
  Example  :  $test = $seqhound->SHound3DExists(4534);
              @reply = $test->getValues();

=cut

sub SHound3DExists
{
   my $self = shift;
   my ($mmdbid) = @_;
   my @arr = ($mmdbid);
   my $arguments = "mmdbid\=$mmdbid";
   my $fulladdress = $self->_HTTPFunctionCall("SeqHound3DExists", $arguments);
   my $content = get($fulladdress);
   my $str = $self->_ProcessContent($content, "SHound3DExists");
   my $sh = new Bio::SeqHound::SeqHoundTextOb(\@arr);
   $sh->mergeValues($str);
   return $sh;
}

=head1 Sequence Fetch ASN.1 Bioseq

=head2 SHoundGetBioseq

  Title    :  SHoundGetBioseq
  Function :  Fetches ASN.1 representation of a sequence called Bioseq. The Bioseq is a
              structure used to represent a single sequence, it contains all available
              annotation.
  Usage    :  $seqhound->SHoundGetBioseq($gi);
  Returns  :  SeqHoundLOB
              value field: ASN.1 Bioseq in a text format or undef if failure
  Args     :  GI identifier
  Example  :  $test = $seqhound->SHoundGetBioseq(535);
              @reply = $test->getValues();

=cut

sub SHoundGetBioseq
{
   my $self = shift;
   my ($Gi) = @_;
   my $arguments = "gi\=$Gi";
   my @arr = ($Gi);
   my $fulladdress = $self->_HTTPFunctionCall("SeqHoundGetBioseqPL", $arguments);
   my $content = get($fulladdress);
   my $str = $self->_ProcessContent($content, "SHoundGetBioseq");
   my $sh = new Bio::SeqHound::SeqHoundLOB(\@arr);
   $sh->mergeValues($str);
   return $sh;
}

=head2 SHoundGetBioseqList

  Title    :  SHoundGetBioseqList
  Function :  Fetches a list of ASN.1 Bioseqs. The Bioseq is a structure used to represent a
              single sequence, it contains all available annotation.
  Usage    :  $seqhound->SHoundGetBioseqList(@gilist);
  Returns  :  SeqHoundLOB
              value field: sequence of ASN.1 Bioseqs in text format or undef if failure
  Args     :  comma delimited list of GI identifiers
  Example  :  $test = $seqhound->SHoundGetBioseqList("2645003","480117","230940");
              @reply = $test->getValues();

=cut

sub SHoundGetBioseqList
{
   my $self = shift;
   my (@pvnGi) = @_;
   my @copyArr = @pvnGi;
   my (@tempArr, $sh);
   $sh = new Bio::SeqHound::SeqHoundLOB(\@pvnGi);
   while($#copyArr != -1){ 
     @tempArr = Bio::SeqHound::SLRIMisc::MaxSizeArray(\@copyArr);
     my $Arrstr = Bio::SeqHound::SLRIMisc::Array2String(\@tempArr);
     my $arguments = "pgi\=$Arrstr";
     my $fulladdress = $self->_HTTPFunctionCall("SeqHoundGetBioseqListPL", $arguments);
     my $content = get($fulladdress);
     my $str = $self->_ProcessContent($content, "SHoundGetBioseqList");
     $sh->mergeValues($str);
   }
   return $sh;
}


=head2  SHoundXMLBioseq

  Title    :  SHoundXMLBioseq
  Function :  This is a wrapper function to call SHoundGetXMLBioseq.
  Note     :  Use SHoundGetXMLBioseq.  This is a wrapper function only.

=cut

sub SHoundXMLBioseq
{
   my $self = shift;
   my ($Gi) = @_;
   $self->SHoundGetXMLBioseq($Gi);
}


=head2 SHoundGetXMLBioseq

  Title    :  SHoundGetXMLBioseq
  Function :  Fetches XML representation of a Bioseq. The Bioseq is a structure
              used to represent a single sequence, it contains  all available annotation.
  Usage    :  $seqhound->SHoundGetXMLBioseq($gi);
  Returns  :  SeqHoundLOB
              value field: XML format of sequence record or undef if failure
  Args     :  GI identifier
  Example  :  $test = $seqhound->SHoundGetXMLBioseq(555);
              @reply = $test->getValues();

=cut

sub SHoundGetXMLBioseq
{
   my $self = shift;
   my ($Gi) = @_;
   my $arguments = "gi\=$Gi";
   my @arr = ($Gi);
   my $fulladdress = $self->_HTTPFunctionCall("SeqHoundXMLBioseqPL", $arguments);
   my $content = get($fulladdress);
   my $str = $self->_ProcessContent($content, "SHoundGetXMLBioseq");
   my $sh = new Bio::SeqHound::SeqHoundLOB(\@arr);
   $sh->mergeValues($str);
   return $sh;
}

=head1 Sequence Fetch ASN.1 SeqEntry

=head2 SHoundGetSeqEntry

  Title    :  SHoundGetSeqEntry
  Function :  Fetches ASN.1 representation of a sequence called SeqEntry. SeqEntry contains a
              single sequence or a group of sequences if they are logically related (e.g. gene
              and its protein product).
  Usage    :  SeqHoundLOB
              value field: $seqhound->SHoundGetSeqEntry($gi);
  Returns  :  ASN.1 SeqEntry in a text format or undef if failure
  Args     :  GI identifier
  Example  :  $test = $seqhound->SHoundGetSeqEntry(234);
              @reply = $test->getValues();

=cut

sub SHoundGetSeqEntry
{
   my $self = shift;
   my ($Gi) = @_;
   my $arguments = "gi\=$Gi";
   my @arr = ($Gi);
   my $fulladdress = $self->_HTTPFunctionCall("SeqHoundGetSeqEntryPL", $arguments);
   my $content = get($fulladdress);
   my $str = $self->_ProcessContent($content, "SHoundGetSeqEntry");
   my $sh = new Bio::SeqHound::SeqHoundLOB(\@arr);
   $sh->mergeValues($str);
   return $sh;
}


=head2 SHoundGetSeqEntryList

  Title    :  SHoundGetSeqEntryList
  Function :  Fetches a list of ASN.1 SeqEntries. SeqEntry contains a single sequence or a
              group of sequences if they are logically related (e.g. gene and its protein
              product).
  Usage    :  $seqhound->SHoundGetSeqEntryList(@gilist);
  Returns  :  SeqHoundLOB
              value field: sequence of ASN.1 SeqEntries in a text fomat or undef if failure
  Args     :  comma delimited list of GI identifiers
  Example  :  $test = $seqhound->SHoundGetSeqEntryList("342,555");
              @reply = $test->getValues();

=cut

sub SHoundGetSeqEntryList
{
   my $self = shift;
   my (@pvnGi) = @_;
   my @copyArr = @pvnGi;
   my (@tempArr, $sh);
   $sh = new Bio::SeqHound::SeqHoundLOB(\@pvnGi);
   while($#copyArr != -1){ 
     @tempArr = Bio::SeqHound::SLRIMisc::MaxSizeArray(\@copyArr);
     my $Arrstr = Bio::SeqHound::SLRIMisc::Array2String(\@tempArr);
     my $arguments = "pgi\=$Arrstr";
     my $fulladdress = $self->_HTTPFunctionCall("SeqHoundGetSeqEntryListPL", $arguments);
     my $content = get($fulladdress);
     my $str = $self->_ProcessContent($content, "SHoundGetSeqEntryList");
     $sh->mergeValues($str);
   }
   return $sh;
}


=head2 SHoundGetXMLSeqEntry

  Title    :  SHoundGetXMLSeqEntry
  Function :  Fetches XML representation of SeqEntry. SeqEntry contains a single
              sequence or a group of sequences if they are logically related (e.g. gene and
              its protein product).
  Usage    :  $seqhound->SHoundGetXMLSeqEntry($gi);
  Returns  :  SeqHoundLOB
              value field: XML format of a SeqEntry record of undef if failure
  Args     :  GI identifier
  Example  :  $test = $seqhound->SHoundGetXMLSeqEntry(4646);
              @reply = $test->getValues();

=cut

sub SHoundGetXMLSeqEntry
{
   my $self = shift;
   my ($Gi) = @_;
   my $arguments = "gi\=$Gi";
   my @arr = ($Gi);
   my $fulladdress = $self->_HTTPFunctionCall("SeqHoundGetXMLSeqEntryPL", $arguments);
   my $content = get($fulladdress);
   my $str = $self->_ProcessContent($content, "SHoundGetXMLSeqEntry");
   my $sh = new Bio::SeqHound::SeqHoundLOB(\@arr);
   $sh->mergeValues($str);
   return $sh;
}

=head1 Sequence Fetch ASN.1 Bioseqset

=head2 SHoundGetBioseqSet

  Title    :  SHoundGetBioseqSet
  Function :  Fetches ASN.1 representation of a sequence called BioseqSet. BioseqSet exists
              only if there is more than one sequence in logical group of sequences (e.g. gene
               and its protein product).
  Usage    :  $seqhound->SHoundGetBioseqSet($gi);
  Returns  :  SeqHoundLOB
              value field: ASN.1 BioseqSet in a text format or undef if failure
  Args     :  GI identifier
  Example  :  $test = $seqhound->SHoundGetBioseqSet(666);
              @reply = $test->getValues();

=cut

sub SHoundGetBioseqSet
{
   my $self = shift;
   my ($Gi) = @_;
   my $arguments = "gi\=$Gi";
   my @arr = ($Gi);
   my $fulladdress = $self->_HTTPFunctionCall("SeqHoundGetBioseqSetPL", $arguments);
   my $content = get($fulladdress);
   my $str = $self->_ProcessContent($content, "SHoundGetBioseqSet");
   my $sh = new Bio::SeqHound::SeqHoundLOB(\@arr);
   $sh->mergeValues($str);
   return $sh;
}


=head2 SHoundGetBioseqSetList

  Title    :  SHoundGetBioseqSetList
  Function :  Fetches a list of ASN.1 BioseqSets. BioseqSet exists only if there is more than
              one sequence in logical group of sequences (e.g. gene and its protein product).
  Usage    :  $seqhound->SHoundGetBioseqSetList(@gilist);
  Returns  :  SeqHoundLOB
              value field: sequence of ASN.1 BioseqSets in a test format of undef if failure
  Args     :  comma delimited list of GI identifiers
  Example  :  $test = $seqhound->SHoundGetBioseqSetList("11","12");
              @reply = $test->getValues();
  Note     :  The order in the lists is not maintained

=cut

sub SHoundGetBioseqSetList
{
   my $self = shift;
   my (@pvnGi) = @_;
   my @copyArr = @pvnGi;
   my (@tempArr, $sh);
   $sh = new Bio::SeqHound::SeqHoundLOB(\@pvnGi);
   while($#copyArr != -1){
     @tempArr = Bio::SeqHound::SLRIMisc::MaxSizeArray(\@copyArr);
     my $Arrstr = Bio::SeqHound::SLRIMisc::Array2String(\@tempArr);
     my $arguments = "pgi\=$Arrstr";
     my $fulladdress = $self->_HTTPFunctionCall("SeqHoundGetBioseqSetListPL", $arguments);
     my $content = get($fulladdress);
     my $str = $self->_ProcessContent($content, "SHoundGetBioseqSetList");
     $sh->mergeValues($str);
   }
   return $sh;
}

=head1 Defline Fetch

=head2 SHoundGetDefline

  Title    :  SHoundGetDefline
  Function :  Fetches definition line for a sequence.  A definition line contains the GI,
              accession number and other available identifiers together with a short
              description of the sequence.
  Usage    :  $seqhound->SHoundGetDefline($gi);
  Returns  :  SeqHoundLOB
              value field: definition line for the sequence record or undef if failure
  Args     :  GI identifier
  Example  :  $test = $seqhound->SHoundGetDefline(442545);
              @reply = $test->getValues();

=cut

sub SHoundGetDefline
{
   my $self = shift;
   my ($Gi) = @_;
   my $arguments = "gi\=$Gi";
   my @arr = ($Gi);
   my $fulladdress = $self->_HTTPFunctionCall("SeqHoundGetDefline", $arguments);
   my $content = get($fulladdress);
   my $str = $self->_ProcessContent($content, "SHoundGetDefline");
   my $sh = new Bio::SeqHound::SeqHoundLOB(\@arr);
   $sh->mergeValues($str);
   return $sh;
}


=head2 SHoundGetDeflineList

  Title    :  SHoundGetDeflineList
  Function :  Fetches multiple definition lines for a list of sequences. A definition line
              contains the GI, accession number and other available identifiers together with
              a short description of the sequence.
  Usage    :  $seqhound->SHoundGetDeflineList(@gilist);
  Returns  :  SeqHoundTextOb
              value field: list of definition line for the corresponding sequence records
              or undef if failure
  Args     :  comma delimited list of GI identifiers
  Example  :  $test = $seqhound->SHoundGetDeflineList("442545,443580");
              @reply = $test->getValues();
  Note     :  The order in the lists is not maintained

=cut

sub SHoundGetDeflineList
{
   my $self = shift;
   my (@pvnGi) = @_;
   my @copyArr = @pvnGi;
   my (@tempArr, $sh);
   $sh = new Bio::SeqHound::SeqHoundTextOb(\@pvnGi);
   while($#copyArr != -1){
     @tempArr = Bio::SeqHound::SLRIMisc::MaxSizeArray(\@copyArr);
     my $Arrstr = Bio::SeqHound::SLRIMisc::Array2String(\@tempArr);
     my $arguments = "pgi\=$Arrstr";
     my $fulladdress = $self->_HTTPFunctionCall("SeqHoundGetDeflineList", $arguments);
     my $content = get($fulladdress);
     my $str = $self->_ProcessContent($content, "SHoundGetDeflineList");
     $sh->mergeValues($str);
   }
   return $sh;
}

=head1 Sequence Fetch FASTA

=head2 SHoundGetFasta

  Title    :  SHoundGetFasta
  Function :  Fetches FASTA formatted sequence.  The FASTA format is widely used in the
              bioinformatics community; it consists of a definition line, new line followed by
               a sequence terminated by a second new line.
  Usage    :  $seqhound->SHoundGetFasta($gi);
  Returns  :  a Bio::Seq (that maps to a FASTA object) or undef if failure
  Args     :  GI identifier
  Example  :  $test = $seqhound->SHoundGetFasta(666);
              @reply = $test->getValues();

=cut

sub SHoundGetFasta
{
    my $self = shift;
    my ($Gi) = @_;
    my $arguments = "gi\=$Gi";
    my $fulladdress = $self->_HTTPFunctionCall("SeqHoundGetFastaPL", $arguments);
    my $content = get($fulladdress);
    my $str = $self->_ProcessContent($content, "SHoundGetFasta");
    if (defined $str) {
    	my $buf = IO::String->new ( $str );
    	my $io = Bio::SeqIO->new ( -format => 'fasta', -fh => $buf );
    	my $seq = $io->next_seq();
    	return $seq;
    }else {return undef};
}


=head2 SHoundGetFastaFromRedundantGroupID

  Title    :  SHoundGetFastaFromRedundantGroupID
  Function :  Fetches FASTA formatted sequence for given redundant group identifier.  The
              FASTA format is widely used in the bioinformatics community; it consists of a
              definition line, new line followed by a sequence terminated by a second new
              line.  Entrez collection of databases contains redundant entries i.e. proteins
              with the same sequence but different annotations.   The redundant group
              identifiers can only be used temporarily, they change every day.
  Usage    :  $seqhound->SHoundGetFastaFromRedundantGroupID($redundantid);
  Returns  :  a Bio::Seq (that maps to a FASTA object) or undef if failure
  Args     :  redundant group id
  Example  :  $a = $seqhound->SHoundGetFastaFromRedundantGroupID(123);
              $defline = $a->getDefline();
              $seq = $a->getSequence();

=cut

sub SHoundGetFastaFromRedundantGroupID
{
    my $self = shift;
    my ($gid) = @_;
    my $arguments = "gid\=$gid";
    my $fulladdress = $self->_HTTPFunctionCall("SeqHoundGetFastaFromRedundantGroupIDPL", $arguments);
    my $content = get($fulladdress);
    my $str = $self->_ProcessContent($content, "SHoundGetFastaFromRedundantGroupID");
    if (defined $str){
    	my $buf = IO::String->new ( $str );
    	my $io = Bio::SeqIO->new ( -format => 'fasta', -fh => $buf );
    	my $seq = $io->next_seq();
    	return $seq;
    }else {return undef;}
}


=head2 SHoundGetFastaList

  Title    :  SHoundGetFastaList
  Function :  Fetches a list of FASTA formatted sequences. The FASTA format is widely used in
              bioinformatics community; it consists of a definition line, new line followed by
               a sequence terminated by a second new line.
  Usage    :  $seqhound->SHoundGetFastaList(@gilist);
  Returns  :  Bio::SeqIO objects or undef if failure.
  Args     :  comma delimited list of GI identifiers
  Example  :  $seqio = $seqhound->SHoundGetFastaList("666","555");
              if (defined $seqio){
	           while ($seq = $seqio->next_seq()){
		       print $seq->seq(), "\n";
		    }
	      }
 
=cut

sub SHoundGetFastaList
{
   my $self = shift;
   my (@pvnGi) = @_;
   my @copyArr = @pvnGi;
   my (@tempArr, $str);
   my $finstr = "";
   while($#copyArr != -1){
     @tempArr = Bio::SeqHound::SLRIMisc::MaxSizeArray(\@copyArr);
     my $Arrstr = Bio::SeqHound::SLRIMisc::Array2String(\@tempArr);
     my $arguments = "pgi\=$Arrstr";
     my $fulladdress = $self->_HTTPFunctionCall("SeqHoundGetFastaListPL", $arguments);
      my $content = get($fulladdress);
     $str = $self->_ProcessContent($content, "GetFastaList");
     $finstr = $finstr.$str."\n";
   }
   if ($finstr ne ""){
    	my $buf = IO::String->new ( $finstr );
    	my $seqio = Bio::SeqIO->new ( -format => 'fasta', -fh => $buf );
    	return $seqio;
    }else {return undef;}
}

=head2 SHoundGetFastaKeyedList

  Title    :  SHoundGetFastaKeyedList
  Function :  Fetches GI identifiers and associated FASTA formatted sequences. The FASTA
              format is widely used in bioinformatics community; it consists of a definition
              line, new line followed by a sequence terminated by a second new line.
  Usage    :  $seqhound->SHoundGetFastaKeyedList(@gilist);
  Returns  :  hash table, each entry of the table stores a fasta stucture and its associated
              query gi
              or an empty hash table if failure
  Args     :  comma delimited list of GI identifiers
  Example  :  my %hash = $seqhound->SHoundGetFastaKeyedList("555","111,"4557225");

              foreach my $gi ( keys(%hash) ) {
                  print STDOUT "key: $gi\n";
                  my $seq = $hash{$gi};
                  if ($seq ne 'null')
                  {
                      my $sequence = $seq->seq();
                      print STDOUT "sequence: $sequence\n";
                      my $length = $seq->length();
                      print STDOUT "length: $length\n";
                      my $access = $seq->accession_number();
                      print STDOUT "accession: $access\n";
                      my $prime = $seq->primary_id();
                      print STDOUT "prime id: $prime\n";
                  }
               }

=cut

sub SHoundGetFastaKeyedList
{
    my $self = shift;
    my (@pvnGi) = @_;
    my @copyArr = @pvnGi;
    my (@tempArr, @fastaArr);
    my (%hash, %fastaHash);
    while($#copyArr != -1){
      @tempArr = Bio::SeqHound::SLRIMisc::MaxSizeArray(\@copyArr);
      my $Arrstr = Bio::SeqHound::SLRIMisc::Array2String(\@tempArr);
      my $arguments = "pgi\=$Arrstr";
      my $fulladdress = $self->_HTTPFunctionCall("SeqHoundGetFastaKeyedList", $arguments);
      my $content = get($fulladdress);
      %hash = $self->_ProcessContentKeyedList($content, "SHoundGetFastaKeyedList", %hash);
    }
    %fastaHash = $self->parse_FastaHash(%hash);
    return %fastaHash;
}

=head2 SHoundGetFastaFromRedundantGroupIDKeyedList

  Title    :  SHoundGetFastaFromRedundantGroupIDKeyedList
  Function :  Fetches FASTA formatted sequence for given redundant group identifiers.  The
              FASTA format is widely used in bioinformatics community; it consists of a
              definition line, new line followed by a sequence terminated by a second new
              line.  Entrez collection of databases contains redundant entries i.e. proteins
              with the same sequence but different annotations.   The redundant group
              identifiers can only be used temporarily, they change every day.
  Usage    :  $seqhound->
              SHoundGetFastaFromRedundantGroupIDKeyedList(@redundantidlist);
  Returns  :  hash table, each entry of the table stores  Bio::Seq objects and its associated
              query redundant groupid
              or an empty hash table if failure
  Args     :  comma delimited list of redundant group ids
  Example  :  %hash = $seqhound->
                      SHoundGetFastaFromRedundantGroupIDKeyedList("362477","123");


=cut

sub SHoundGetFastaFromRedundantGroupIDKeyedList
{
    my $self = shift;
    my (@pvnGid) = @_;
    my @copyArr = @pvnGid;
    my (@tempArr, @fastaArr);
    my (%hash, %fastaHash);
    while($#copyArr != -1){
      @tempArr = Bio::SeqHound::SLRIMisc::MaxSizeArray(\@copyArr);
      my $Arrstr = Bio::SeqHound::SLRIMisc::Array2String(\@tempArr);
      my $arguments = "pgid\=$Arrstr";
      my $fulladdress = $self->_HTTPFunctionCall("SeqHoundGetFastaFromRedundantGroupIDKeyedList", $arguments);
      my $content = get($fulladdress);
      %hash = $self->_ProcessContentKeyedList($content, "SHoundGetFastaFromRedundantGroupIDKeyedList", %hash);
    }
    %fastaHash = $self->parse_FastaHash(%hash);
    return %fastaHash;
}


=head1 Sequence Fetch GenBank Flat File

=head2 SHoundGetGenBankff

  Title    :  SHoundGetGenBankff
  Function :  Fetches GenBank formatted sequence.  GenBank flat file contains
              comprehensive full annotation of a sequence.
  Usage    :  $seqhound->SHoundGetGenBankff($gi);
  Returns  :  SeqHoundGB 
              value field: GenBank formatted sequence or undef if failure
  Args     :  GI identifier
  Example  :  $test = $seqhound->SHoundGetGenBankff(32);
              @reply = $test->getValues();

=cut

sub SHoundGetGenBankff
{
    my $self = shift;
    my ($Gi) = @_;
    my $arguments = "gi\=$Gi";
    my @arr = ($Gi);
    my $fulladdress = $self->_HTTPFunctionCall("SeqHoundGetGenBankff", $arguments);
    my $content = get($fulladdress);
    my $str = $self->_ProcessContent($content, "SHoundGetGenBankff");
    my $gb = new Bio::SeqHound::SeqHoundGB($Gi);
    $gb->mergeValues($str);
    return $gb;
}

=head2 SHoundGetGenBankffList

  Title    :  SHoundGetGenBankffList
  Function :  Fetches a list of GenBank formatted sequences. GenBank flat file
              contains comprehensive full annotation of a sequence.
  Usage    :  $seqhound->SHoundGetGenBankffList(@gilist);
  Returns  :  Array of SeqHoundGB
  Args     :  comma delimited list of GI identifiers
  Example  :  @gb = $seqhound->SHoundGetGenBankffList("32","666");
              foreach $gb (@gb){
	          # $gb is SeqHoundGB, see example in SHoundGetGenBankff
		  ...
	      } 
  Note     :  The order in the lists is not maintained

=cut

sub SHoundGetGenBankffList
{
    my $self = shift;
    my @Gi = @_;
    my @gbArr;
    foreach my $gi ( @Gi )
    {
      my $gb = $self->SHoundGetGenBankff ( $gi );
      @gbArr = (@gbArr, $gb);
    }
    return @gbArr;
}


=head2 SHoundGetSeqFromGenBank


  Title    :  SHoundGetSeqFromGenBank
  Function :  Fetches a GenBank sequence.
  Usage    :  $seqhound->SHoundGetSeqFromGenBank($gi);
  Returns  :  Bio::Seq object or undef if failure
  Args     :  GI identifier
  Example  :  $test = $seqhound->SHoundGetSeqFromGenBank(32);
              print "ACC: ", $test->display_id(), "\n";
              print "SEQ: ", $test->seq(), "\n";
              print "TYP: ", $test->alphabet(), "\n";
              print "ACC: ", $test->accession_number(), "\n";
              print "VER: ", $test->seq_version(), "\n";
              print "DES: ", $test->desc(), "\n";
              print "PRI: ", $test->primary_id(), "\n";
              print "\n";
  Note     :  Consult the documentation for Bio::Seq for more information on the Bio::Seq
              object

=cut

sub SHoundGetSeqFromGenBank
{
    my $self = shift;
    my ($Gi) = @_;
    my $arguments = "gi\=$Gi";
    my $fulladdress = $self->_HTTPFunctionCall("SeqHoundGetGenBankff", $arguments);
    my $content = get($fulladdress);
    my $str = $self->_ProcessContent($content, "SHoundGetGenBankff");
    if (defined $str) {
    	my $buf = IO::String->new ( $str );
    	my $io = Bio::SeqIO->new ( -format => 'genbank', -fh => $buf );
    	my $seq = $io->next_seq(); 
    	return $seq;
    }else {return undef;}
}

=head1 Structure Fetch

=head2 SHoundGet3D

  Title    :  SHoundGet3D
  Function :  Fetches ASN.1 representation of a 3-D structure called Biostruc.  Biostrucs
              represent data model in the Molecular Modeling database.
  Usage    :  $seqhound->SHoundGet3D($mmdbid);
  Returns  :  SeqHoundLOB
  	      value field: ASN.1 Biostruc in a text format or undef if failure
  Args     :  Molecular Modeling Database identifier (MMDB ID)
  Example  :  $test = $seqhound->SHoundGet3D(32);
              @reply = $test->getValues();

=cut

sub SHoundGet3D
{
    my $self = shift;
    my ($mmdbid) = @_;
    my $arguments = "mmdbid\=$mmdbid";
    my @arr = ($mmdbid);
    my $fulladdress = $self->_HTTPFunctionCall("SeqHoundGet3DPL", $arguments);
    my $content = get($fulladdress);
    my $str = $self->_ProcessContent($content, "SHoundGet3D");
    my $sh = new Bio::SeqHound::SeqHoundLOB(\@arr);
    $sh->mergeValues($str);
    return $sh;
}

=head2 SHoundGet3DfromPdbId

  Title    :  SHoundGet3DfromPdbId
  Function :  Fetches ASN.1 representation of a 3-D structure called Biostruc. Biostrucs
              represent data model in the Molecular Modeling database.
  Usage    :  $seqhound->SHoundGet3DfromPdbId($pdbid);
  Returns  :  SeqHoundLOB
  	      value field: ASN.1 Biostruc in a text format or undef if failure
  Args     :  PDB code
  Example  :  $test = $seqhound->SHoundGet3DfromPdbId("3INS");
              @reply = $test->getValues();

=cut

sub SHoundGet3DfromPdbId
{
   my $self = shift;
   my ($pdbid, $file) = @_;
   my $arguments = "pdbid\=$pdbid";
   my @arr = ($pdbid);
   my $fulladdress = $self->_HTTPFunctionCall("SeqHoundGet3DfromPdbIdPL", $arguments);
   my $content = get($fulladdress);
   my $str = $self->_ProcessContent($content, "SHoundGet3DfromPdbId");
   my $sh = new Bio::SeqHound::SeqHoundLOB(\@arr);
   $sh->mergeValues($str);
   if($file){
     open (OUT, ">$file");
     print OUT $str; 
     close OUT;
   }
   return $sh;
}


=head2 SHoundGetXML3D

  Title    :  SHoundGetXML3D
  Function :  Fetches XML representation of a 3-D structure.  XML is a converted
              Biostruc which represent data model in Molecular Modeling database.
  Usage    :  $seqhound->SHoundGetXML3D($mmdbid);
  Returns  :  SeqHoundLOB
  	      value field: XML structural record or undef if failure
  Args     :  Molecular Modeling Database identifier (MMDB ID)
  Example  :  $test = $seqhound->SHoundGetXML3D(11);
              @reply = $test->getValues();

=cut

sub SHoundGetXML3D
{
   my $self = shift;
   my ($mmdbid, $file) = @_;
   my $arguments = "mmdbid\=$mmdbid";
   my @arr = ($mmdbid);
   my $fulladdress = $self->_HTTPFunctionCall("SeqHoundGetXML3DPL", $arguments);
   my $content = get($fulladdress);
   my $str = $self->_ProcessContent($content, "SHoundGetXML3D");
   my $sh = new Bio::SeqHound::SeqHoundLOB(\@arr);
   $sh->mergeValues($str);
   if($file){
     open (OUT, ">$file");
     print OUT $str;
     close OUT;
   }
   return $sh;
}


=head2 SHoundGetPDB3D

  Title    :  SHoundGetPDB3D
  Function :  Fetches a 3-D structure in Protein Data Bank (PDB) format.  PDB text
              format is a format used by Protein Data Bank.
  Usage    :  $seqhound->SHoundGetPDB3D($mmdbid);
  Returns  :  SeqHoundLOB
  	      value field: PDB formatted structural record or undef if failure
  Args     :  Molecular Modeling Database identifier (MMDB ID)
  Example  :  $test = $seqhound->SHoundGetPDB3D(11);
              @reply = $test->getValues();

=cut

sub SHoundGetPDB3D
{
    my $self = shift;
    my ($mmdbid, $file) = @_;
    my @arr = ($mmdbid);
    my $arguments = "mmdbid\=$mmdbid";
    my $fulladdress = $self->_HTTPFunctionCall("SeqHoundGetPDB3DPL", $arguments);
    my $content = get($fulladdress);
    my $str = $self->_ProcessContent($content, "SHoundGetPDB3D");
    my $sh = new Bio::SeqHound::SeqHoundLOB(\@arr);
    $sh->mergeValues($str);
    if($file){
      open (OUT, ">$file");
      print OUT $str;
      close OUT;
    } 
    return $sh;
}


=head1 SeqHound Links

=head2 SHoundGiFrom3D

  Title    :  SHoundGiFrom3D
  Function :  Gets GI identifiers for sequence or sequences contained in a 3-D structure.
  Usage    :  $seqhound->SHoundGiFrom3D($gi, $proteinonly);
  Returns  :  SeqHoundTextOb
  	      value field: list of GIs or one GI or undef if failure
  Args     :  Molecular Modeling Database identifier (MMDB ID)
              "TRUE" or "FALSE" for proteins only
  Example  :  $test = $seqhound->SHoundGiFrom3D(23, "FALSE");
              @reply = $test->getValues();

=cut

sub SHoundGiFrom3D
{
   my $self = shift;
   my ($mmdbid, $boolProteinOnly) = @_;
   my @arr = ($mmdbid, $boolProteinOnly);
   my $arguments = "mmdbid\=$mmdbid"."\&ProteinOnly\=$boolProteinOnly";
   my $fulladdress = $self->_HTTPFunctionCall("SeqHoundGiFrom3D", $arguments);
   my $content = get($fulladdress);
   my $str = $self->_ProcessContent($content, "SHoundGiFrom3D");
   my $sh = new Bio::SeqHound::SeqHoundTextOb(\@arr);
   $sh->mergeValues($str);
   return $sh;
}

=head2 SHoundGiFrom3DList

  Title    :  SHoundGiFrom3DList
  Function :  Gets GI identifiers contained in a list of 3-D identifiers.
  Usage    :  $seqhound->SHoundGiFrom3DList($mmdbidlist, $proteinonly);
  Returns  :  SeqHoundTextOb
  	      value field: list of GIs or undef if failure
  Args     :  comma delimited list of Molecular Modeling Database identifiers (MMDB IDs)
              "TRUE" or "FALSE" for proteins only
  Example  :  $test = $seqhound->SHoundGiFrom3DList("FALSE", "32", "23");
              @reply = $test->getValues();

=cut

sub SHoundGiFrom3DList
{
   my $self = shift;
   my ($boolProteinOnly, @pvnMmdbid) = @_;
   my @copyArr = @pvnMmdbid;
   my (@tempArr, $sh);
   $sh = new Bio::SeqHound::SeqHoundTextOb(\@copyArr);
   while($#copyArr != -1){
     @tempArr = Bio::SeqHound::SLRIMisc::MaxSizeArray(\@copyArr);
     my $Arrstr = Bio::SeqHound::SLRIMisc::Array2String(\@tempArr);
     my $arguments = "pmmdbid\=$Arrstr"."\&ProteinOnly\=$boolProteinOnly";
     my $fulladdress = $self->_HTTPFunctionCall("SeqHoundGiFrom3DList", $arguments);
     my $content = get($fulladdress);
     my $str = $self->_ProcessContent($content, "SHoundGiFrom3DList");
     $sh->mergeValues($str);
   }
   return $sh;
}

=head2 SHoundGiFromPDBchain

  Title    :  SHoundGiFromPDBchain
  Function :  Gets a GI  identifier for a Protein Data Bank (PDB) structural chain.  PDB code
              supplemented with a chain always refers to a single sequence.
  Usage    :  $seqhound->SHoundGiFromPDBchain($pdbid, $pdbchain);
  Returns  :  SeqHoundTextOb
  	      value field: GI identifier for the PDB chain (e.g. 443580) or undef if failure
  Args     :  PDB code
              PDB chain (e.g. "A")
  Example  :  $test = $seqhound->SHoundGiFromPDBchain("9XIM", "A");
              @reply = $test->getValues();

=cut

sub SHoundGiFromPDBchain
{
   my $self = shift;
   my ($pdbcode, $chain ) = @_;
   my @arr = ($pdbcode, $chain);
   if($chain =~ /NULL/i){
     $chain = "";
   }
   my $arguments = "pdbcode\=$pdbcode"."\&chain\=$chain";
   my $fulladdress = $self->_HTTPFunctionCall("SeqHoundGiFromPDBchain", $arguments);
   my $content = get($fulladdress);
   my $str = $self->_ProcessContent($content, "SHoundGiFromPDBchain");
   my $sh = new Bio::SeqHound::SeqHoundTextOb(\@arr);
   $sh->mergeValues($str);
   return $sh;
}


=head2 SHound3DFromGi

  Title    :  SHound3DFromGi
  Function :  Converts a GI sequence identifier to a structural MMDB identifier. Reports in
              which 3-D structure a particular sequence can be found.
  Usage    :  $seqhound->SHound3DFromGi($gi);
  Returns  :  SeqHoundTextOb
  	      value field: MMDB ID (e.g. 3446) or undef if failure
  Args     :  GI identifier
  Example  :  $test = $seqhound->SHound3DFromGi(442560);
              @reply = $test->getValues();

=cut

sub SHound3DFromGi
{
   my $self = shift;
   my ($Gi) = @_;
   my @arr = ($Gi);
   my $arguments = "gi\=$Gi";
   my $fulladdress = $self->_HTTPFunctionCall("SeqHound3DFromGi", $arguments);
   my $content = get($fulladdress);
   my $str = $self->_ProcessContent($content, "SHound3DFromGi");
   my $sh = new Bio::SeqHound::SeqHoundTextOb(\@arr);
   $sh->mergeValues($str);
   return $sh;
}


=head2 SHound3DFromGiList

  Title    :  SHound3DFromGiList
  Function :  Converts a list of sequence GI identifiers to a list of structural MMDB
              identifiers.
  Usage    :  $seqhound->SHound3DFromGiList(@gilist);
  Returns  :  SeqHoundTextOb
  	      value field: list of MMDB IDs (e.g. "11,12") or undef if failure
  Args     :  comma delimited list of GI identifiers
  Example  :  $test = $seqhound->SHound3DFromGiList("442560","442572");
              @reply = $test->getValues();
  Note     :  The order in the lists is not maintained

=cut

sub SHound3DFromGiList
{
   my $self = shift;
   my (@pvnGi) = @_;
   my @copyArr = @pvnGi;
   my (@tempArr, $sh);
   $sh = new Bio::SeqHound::SeqHoundTextOb(\@pvnGi);
   while($#copyArr != -1){
     @tempArr = Bio::SeqHound::SLRIMisc::MaxSizeArray(\@copyArr);
     my $Arrstr = Bio::SeqHound::SLRIMisc::Array2String(\@tempArr);
     my $arguments = "pgi\=$Arrstr";
     my $fulladdress = $self->_HTTPFunctionCall("SeqHound3DFromGiList", $arguments);
     my $content = get($fulladdress);
     my $str = $self->_ProcessContent($content, "SHound3DFromGiList");
     $sh->mergeValues($str);
   }
   return $sh;
}


=head2 SHoundDNAFromProtein

  Title    :  SHoundDNAFromProtein
  Function :  Converts protein (gene product)  identifier to a nucleic acid (gene) identifier.
  Usage    :  $seqhound->SHoundDNAFromProtein($gi);
  Returns  :  SeqHoundTextOb
  	      value field: DNA GI identifier (e.g. 152776) or undef if failure
  Args     :  protein GI identifier
  Example  :  $test = $seqhound->SHoundDNAFromProtein(32);
              @reply = $test->getValues();

=cut

sub SHoundDNAFromProtein
{
   my $self = shift;
   my ($Gi) = @_;
   my @arr = ($Gi);
   my $arguments = "gi\=$Gi";
   my $fulladdress = $self->_HTTPFunctionCall("SeqHoundDNAFromProtein", $arguments);
   my $content = get($fulladdress);
   my $str = $self->_ProcessContent($content, "SHoundDNAFromProtein");
   my $sh = new Bio::SeqHound::SeqHoundTextOb(\@arr);
   $sh->mergeValues($str);
   return $sh;
}

=head2 SHoundDNAFromProteinList

  Title    :  SHoundDNAFromProteinList
  Function :  Converts a list of protein (gene product) identifiers to a list of nucleic acids
               (genes) identifiers.
  Usage    :  $seqhound->SHoundDNAFromProteinList(@gilist);
  Returns  :  SeqHoundTextOb
  	      value field: list of DNA identifiers (e.g. "152776,961496") or undef if failure
  Args     :  comma delimited list  of protein GI identifiers
  Example  :  $test = $seqhound->SHoundDNAFromProteinList("325,4546");
              @reply = $test->getValues();
  Note     :  The order in the lists is not maintained

=cut

sub SHoundDNAFromProteinList
{
   my $self = shift;
   my (@pvnGi) = @_;
   my @copyArr = @pvnGi;
   my (@tempArr, $sh);
   $sh = new Bio::SeqHound::SeqHoundTextOb(\@pvnGi);
   while($#copyArr != -1){
     @tempArr = Bio::SeqHound::SLRIMisc::MaxSizeArray(\@copyArr);
     my $Arrstr = Bio::SeqHound::SLRIMisc::Array2String(\@tempArr);
     my $arguments = "pgi\=$Arrstr";
     my $fulladdress = $self->_HTTPFunctionCall("SeqHoundDNAFromProteinList", $arguments);
     my $content = get($fulladdress);
     my $str = $self->_ProcessContent($content, "SHoundDNAFromProteinList");
     $sh->mergeValues($str);
   }
   return $sh;
}


=head2 SHoundProteinFromDNA

  Title    :  SHoundProteinFromDNA
  Function :  Converts a nucleic acid (gene) identifier to one or more  protein (gene product)
                identifiers.
  Usage    :  $seqhound->SHoundProteinFromDNA($gi);
  Returns  :  SeqHoundTextOb
  	      value field: list of protein GI identifiers
  Args     :  DNA GI identifier
  Example  :  $test = $seqhound->SHoundProteinFromDNA(31);
              @reply = $test->getValues();

=cut

sub SHoundProteinFromDNA
{
   my $self = shift;
   my ($Gi) = @_;
   my @arr = ($Gi);
   my $arguments = "gi\=$Gi";
   my $fulladdress = $self->_HTTPFunctionCall("SeqHoundProteinFromDNA", $arguments);
   my $content = get($fulladdress);
   my $str = $self->_ProcessContent($content, "SHoundProteinFromDNA");
   my $sh = new Bio::SeqHound::SeqHoundTextOb(\@arr);
   $sh->mergeValues($str);
   return $sh;
}


=head2 SHoundProteinFromDNAList

  Title    :  SHoundProteinFromDNAList
  Function :  Converts a list of nucleic acid (gene) identifiers to a list of protein (gene
              product) identifiers.
  Usage    :  $seqhound->SHoundProteinFromDNAList(@gilist);
  Returns  :  SeqHoundTextOb
  	      value field: comma delimitedlist of protein GI identifiers or undef if failure
  Args     :  comma delimited list of DNA GI identifiers
  Example  :  $test = $seqhound->SHoundProteinFromDNAList("324","4545");
              @reply = $test->getValues();
  Note     :  The order in the lists is not maintained

=cut

sub SHoundProteinFromDNAList
{
   my $self = shift;
   my (@pvnGi) = @_;
   my @copyArr = @pvnGi;
   my (@tempArr, $sh);
   $sh = new Bio::SeqHound::SeqHoundTextOb(\@pvnGi);
   while($#copyArr != -1){
     @tempArr = Bio::SeqHound::SLRIMisc::MaxSizeArray(\@copyArr);
     my $Arrstr = Bio::SeqHound::SLRIMisc::Array2String(\@tempArr);
     my $arguments = "pgi\=$Arrstr";
     my $fulladdress = $self->_HTTPFunctionCall("SeqHoundProteinFromDNAList", $arguments);
     my $content = get($fulladdress);
     my $str = $self->_ProcessContent($content, "SHoundProteinFromDNAList");
     $sh->mergeValues($str);
   }
   return $sh;
}

=head2 SHoundGetReferenceIDFromGi

  Title    :  SHoundGetReferenceIDFromGi
  Function :  Retrieves publication reference ID from a GI and reference type.
  Usage    :  $seqhound->SHoundGetReferenceIDFormGi($gi,$referencetype);
  Returns  :  SeqHoundTextOb
  	      value field: list of reference IDs or undef if failure
  Args     :  GI identifier
              reference type
                  1 for PubMed ID
                  2 for MedLine ID
  Example  :  $test = $seqhound->SHoundGetReferenceIDFormGi(6322016,  2);
              @reply = $test->getValues();
  Note     :  This function deprecates SHoundMuidFromGi.

=cut

sub SHoundGetReferenceIDFromGi
{
   my $self = shift;
   my ($gi, $reftype) = @_;
   my @arr = ($gi, $reftype);
   my $arguments = "gi\=$gi"."\&referencetype\=$reftype";
   my $fulladdress = $self->_HTTPFunctionCall("SeqHoundGetReferenceIDFromGi", $arguments);
   my $content = get($fulladdress);
   my $str = $self->_ProcessContent($content, "SHoundGetReferenceIDFromGi");
   my $sh = new Bio::SeqHound::SeqHoundTextOb(\@arr);
   $sh->mergeValues($str);
   return $sh;
}

=head2 SHoundGetReferenceIDFromGiList

  Title    :  SHoundGetReferenceIDFromGiList
  Function :  Retrieves publication reference ID from a GI list and reference type.
  Usage    :  $seqhound->SHoundGetReferenceIDFormGiList(@gilist,$referencetype);
              duplicates in list?
  Returns  :  SeqHoundTextOb
  	      value field: list of reference IDs or undef if failure
  Args     :  comma delimited list of GI identifiers
              reference type
                  1 for PubMed ID
                  2 for MedLine ID
  Example  :  $test = $seqhound->SHoundGetReferenceIDFormGiList("4557225","21071030", 2);
              @reply = $test->getValues();
  Note     :  The order in the lists is not maintained
              This function deprecates SHoundMuidFromGiList.

=cut

sub SHoundGetReferenceIDFromGiList
{
   my $self = shift;
   my ($reftype, @gilist) = @_;
   my (@tempArr, $sh);
   my @copyArr = @gilist;
   #my @arr = (@gilist, $reftype);
   $sh = new Bio::SeqHound::SeqHoundTextOb(\@gilist);
   while($#copyArr != -1){
     @tempArr = Bio::SeqHound::SLRIMisc::MaxSizeArray(\@copyArr);
     my $Arrstr = Bio::SeqHound::SLRIMisc::Array2String(\@tempArr);
     my $arguments = "gilist\=$Arrstr"."\&reftype=$reftype";
     my $fulladdress = $self->_HTTPFunctionCall("SeqHoundGetReferenceIDFromGiList", $arguments);
     my $content = get($fulladdress);
     my $str = $self->_ProcessContent($content, "SHoundGetReferenceIDFromGiList");
     $sh->mergeValues($str);
   }
    return $sh;
}

=head2 SHoundGiFromReferenceID

  Title    :  SHoundGiFromReferenceID
  Function :  Gets GI identifiers from publication reference ID.
  Usage    :  $seqhound->SHoundGiFromReferenceID($referenceid,$referencetype);
  Returns  :  SeqHoundTextOb
  	      value field: list of GIs or undef if failure
  Args     :  reference ID
              reference type
                  1 for PubMed ID
                  2 for MedLine ID
  Example  :  $test = $seqhound->SHoundGiFromReferenceID(85190481,  2);
              @reply = $test->getValues();
  Note     :  Return value may be very long for some publications, eg for a high throughput
              publication.
              The return list may contain duplicates

=cut

sub SHoundGiFromReferenceID
{
   my $self = shift;
   my ($refid, $reftype) = @_;
   my @arr = ($refid, $reftype);
   my $arguments = "referenceid\=$refid"."\&referencetype\=$reftype";
   my $fulladdress = $self->_HTTPFunctionCall("SeqHoundGiFromReferenceID", $arguments);
   my $content = get($fulladdress);
   my $str = $self->_ProcessContent($content, "SHoundGiFromReferenceID");
   my $sh = new Bio::SeqHound::SeqHoundTextOb(\@arr);
   $sh->mergeValues($str);
   return $sh;
}


=head2 SHoundGiFromReferenceList

  Title    :  SHoundGiFromReferenceList
  Function :  Gets GI identifiers from publication reference ID list.
  Usage    :  $seqhound->SHoundGiFromReferenceList(@referenceidlist,$referencetype);
  Returns  :  SeqHoundTextOb
  	      value field: a list of comma delimited GIs or undef if failure
  Args     :  referenceID list
              reference type
                  1 for PubMed ID
                  2 for MedLine ID
  Example  :  $test = $seqhound->SHoundGiFromReferenceList("97002444,85190481",  2);
              @reply = $test->getValues();
  Note     :  Return value may be very long for some publications, eg for a high throughput
              publication.
              The order in the lists is not maintained
              The return list may contain duplicates

=cut

sub SHoundGiFromReferenceList
{
   my $self = shift;
   my ($reftype, @reflist) = @_;
   my (@tempArr, $sh);
   my @copyArr = @reflist;
   #my @arr = ($reflist, $reftype);
   $sh = new Bio::SeqHound::SeqHoundTextOb(\@reflist);
   while($#copyArr != -1){
     @tempArr = Bio::SeqHound::SLRIMisc::MaxSizeArray(\@copyArr);
     my $Arrstr = Bio::SeqHound::SLRIMisc::Array2String(\@tempArr);
     my $arguments = "reflist\=$Arrstr"."\&reftype=$reftype";
     my $fulladdress = $self->_HTTPFunctionCall("SeqHoundGiFromReferenceList", $arguments);
     my $content = get($fulladdress);
     my $str = $self->_ProcessContent($content, "SHoundGiFromReferenceList");
     $sh->mergeValues($str);
   }
    return $sh;
}

=head2 SHoundMuidFromGi

  Title    :  SHoundMuidFromGi
  Function :  Gets MEDLINE identifier(s) (MUID) for a given GI identifier.  Fetches
              identifier(s) of the original paper(s) discussing a given sequence.
  Usage    :  $seqhound->SHoundMuidFromGi($gi);
  Returns  :  SeqHoundTextOb
  	      value field: list of MUID identifiers or undef if failure
  Args     :  GI identifier
  Example  :  $test = $seqhound->SHoundMuidFromGi(32);
              @reply = $test->getValues();
  Note     :  This function will be deprecated.
              Please use SHoundGetReferenceIDFromGi

=cut

sub SHoundMuidFromGi
{
   my $self = shift;
   my ($Gi) = @_;
   my @arr = ($Gi);
   my $arguments = "gi\=$Gi";
   my $fulladdress = $self->_HTTPFunctionCall("SeqHoundMuidFromGi", $arguments);
   my $content = get($fulladdress);
   my $str = $self->_ProcessContent($content, "SHoundMuidFromGi");
   my $sh = new Bio::SeqHound::SeqHoundTextOb(\@arr);
   $sh->mergeValues($str);
   return $sh;
}


=head2 SHoundMuidFromGiList

  Title    :  SHoundMuidFromGiList
  Function :  Gets MEDLINE identifiers (MUID) for a list of GI identifiers. Fetches
              identifiers of  the original papers discussing given sequences.
  Usage    :  $seqhound->SHoundMuidFromGiList(@gilist);
  Returns  :  SeqHoundTextOb
  	      value field: list of MUID identifiers or undef if failure
  Args     :  comma delimited list of GI identifiers
  Example  :  $test = $seqhound->SHoundMuidFromGiList("324,32");
              @reply = $test->getValues();
  Note     :  The order in the lists is not maintained
              This function will be deprecated.
              Please use SHoundGetReferenceIDFromGiList

=cut

sub SHoundMuidFromGiList
{
   my $self = shift;
   my (@pvnGi) = @_;
   my @copyArr = @pvnGi;
   my (@tempArr, $sh);
   $sh = new Bio::SeqHound::SeqHoundTextOb(\@pvnGi);
   while($#copyArr != -1){
     @tempArr = Bio::SeqHound::SLRIMisc::MaxSizeArray(\@copyArr);
     my $Arrstr = Bio::SeqHound::SLRIMisc::Array2String(\@tempArr);
     my $arguments = "pgi\=$Arrstr";
     my $fulladdress = $self->_HTTPFunctionCall("SeqHoundMuidFromGiList", $arguments);
     my $content = get($fulladdress);
     my $str = $self->_ProcessContent($content, "SHoundMuidFromGiList");
     $sh->mergeValues($str);
   }
   return $sh;
}

=head2 SHoundMuidFrom3D

  Title    :  SHoundMuidFrom3D
  Function :  Converts a Molecular Modeling Database structural identifier (MMDBID) to one or
              more MEDLINE identifiers (MUID). Fetches  identifier of the original paper
              discussing a given 3-D structure.
  Usage    :  $seqhound->SHoundMuidFrom3D($mmdbid);
  Returns  :  SeqHoundTextOb
  	      value field: list of MUID identifiers or undef if failure
  Args     :  Molecular Modeling Database identifier (MMDB ID)
  Example  :  $test = $seqhound->SHoundMuidFrom3D(11);
              @reply = $test->getValues();

=cut

sub SHoundMuidFrom3D
{
   my $self = shift;
   my ($mmdbid) = @_;
   my @arr = ($mmdbid);
   my $arguments = "mmdbid\=$mmdbid";
   my $fulladdress = $self->_HTTPFunctionCall("SeqHoundMuidFrom3D", $arguments);
   my $content = get($fulladdress);
   my $str = $self->_ProcessContent($content, "SHoundMuidFrom3D");
   my $sh = new Bio::SeqHound::SeqHoundTextOb(\@arr);
   $sh->mergeValues($str);
   return $sh;
}

=head2 SHoundMuidFrom3DList

  Title    :  SHoundMuidFrom3DList
  Function :  Converts a list of Molecular Modeling Database structural identifiers (MMDBID)
              to a list of MEDLINE identifiers (MUID). Fetches identifiers of the original
              papers discussing given 3-D structures.
  Usage    :  $seqhound->SHoundMuidFrom3DList(@mmdbidlist);
  Returns  :  SeqHoundTextOb
  	      value field: list of MUIDs or undef if failure
  Args     :  comma delimited list of structural identifiers
  Example  :  $test = $seqhound->SHoundMuidFrom3DList("11,12");
              @reply = $test->getValues();
  Note     :  The order in the lists is not maintained

=cut

sub SHoundMuidFrom3DList
{
   my $self = shift;
   my (@pmmdbid) = @_;
   my @copyArr = @pmmdbid;
   my (@tempArr, $sh);
   $sh = new Bio::SeqHound::SeqHoundTextOb(\@pmmdbid);
   while($#copyArr != -1){
     @tempArr = Bio::SeqHound::SLRIMisc::MaxSizeArray(\@copyArr);
     my $Arrstr = Bio::SeqHound::SLRIMisc::Array2String(\@tempArr);
     my $arguments = "pmmdbid\=$Arrstr";
     my $fulladdress = $self->_HTTPFunctionCall("SeqHoundMuidFrom3DList", $arguments);
     my $content = get($fulladdress);
     my $str = $self->_ProcessContent($content, "SHoundMuidFrom3DList");
     $sh->mergeValues($str);
   }
   return $sh;
}

=head2 SHoundTaxIDFromGi

  Title    :  SHoundTaxIDFromGi
  Function :  Converts a GI identifier to an NCBI taxonomy identifier. Reports which organism
              a given sequence is from.
  Usage    :  $seqhound->SHoundTaxIDFromGi($gi);
  Returns  :  SeqHoundTextOb
  	      value field: taxonomy identifier (e.g. 9913) or undef if failure
  Args     :  GI identifier
  Example  :  $test = $seqhound->SHoundTaxIDFromGi(32);
              @reply = $test->getValues();

=cut

sub SHoundTaxIDFromGi
{
   my $self = shift;
   my ($Gi) = @_;
   my @arr = ($Gi);
   my $arguments = "gi\=$Gi";
   my $fulladdress = $self->_HTTPFunctionCall("SeqHoundTaxIDFromGi", $arguments);
   my $content = get($fulladdress);
   my $str = $self->_ProcessContent($content, "SHoundTaxIDFromGi");
   my $sh = new Bio::SeqHound::SeqHoundTextOb(\@arr);
   $sh->mergeValues($str);
   return $sh;
}


=head2 SHoundTaxIDFromGiList

  Title    :  SHoundTaxIDFromGiList
  Function :  Converts a list of GI identifiers to a list of NCBI taxonomy identifier  Reports
               which organisms given sequences are from.
  Usage    :  $seqhound->SHoundTaxIDFromGiList(@gilist);
  Returns  :  SeqHoundTextOb
  	      value field: list of taxonomy identifiers or undef if failure
  Args     :  comma delimited list of GI identifiers
  Example  :  $test = $seqhound->SHoundTaxIDFromGiList("324,32");
              @reply = $test->getValues();
  Note     :  The order in the lists is not maintained

=cut

sub SHoundTaxIDFromGiList
{
   my $self = shift;
   my (@pvnGi) = @_;
   my @copyArr = @pvnGi;
   my (@tempArr, $sh);
   $sh = new Bio::SeqHound::SeqHoundTextOb(\@pvnGi);
   while($#copyArr != -1){
     @tempArr = Bio::SeqHound::SLRIMisc::MaxSizeArray(\@copyArr);
     my $Arrstr = Bio::SeqHound::SLRIMisc::Array2String(\@tempArr);
     my $arguments = "pgi\=$Arrstr";
     my $fulladdress = $self->_HTTPFunctionCall("SeqHoundTaxIDFromGiList", $arguments);
     my $content = get($fulladdress);
     my $str = $self->_ProcessContent($content, "SHoundTaxIDFromGiList");
     $sh->mergeValues($str);
   }
   return $sh;
}

=head2 SHoundTaxIDFrom3D

  Title    :  SHoundTaxIDFrom3D
  Function :  Converts a structural identifier to one or more taxonomy identifiers.   Reports
              which organism(s) the 3-D structure comes from.
  Usage    :  $seqhound->SHoundTaxIDFrom3D($mmdbid);
  Returns  :  SeqHoundTextOb
  	      value field: list of taxonomy identifiers or undef if failure
  Args     :  Molecular Modeling Database identifier (MMDB ID)
  Example  :  $test = $seqhound->SHoundTaxIDFrom3D(11);
              @reply = $test->getValues();

=cut

sub SHoundTaxIDFrom3D
{
   my $self = shift;
   my ($mmdbid) = @_;
   my @arr = ($mmdbid);
   my $arguments = "mmdbid\=$mmdbid";
   my $fulladdress = $self->_HTTPFunctionCall("SeqHoundTaxIDFrom3D", $arguments);
   my $content = get($fulladdress);
   my $str = $self->_ProcessContent($content, "SHoundTaxIDFrom3D");
   my $sh = new Bio::SeqHound::SeqHoundTextOb(\@arr);
   $sh->mergeValues($str);
   return $sh;
}


=head2 SHoundTaxIDFrom3DList

  Title    :  SHoundTaxIDFrom3DList
  Function :  Converts a list of structural identifiers to a list of taxonomy identifiers.
              Reports which organisms the 3-D structures come from.
  Usage    :  $seqhound->SHoundTaxIDFrom3DList(@mmdbidlist);
  Returns  :  SeqHoundTextOb
  	      value field: list of taxonomy identifiers or undef if failure
  Args     :  comma delimited list of structural identifiers
  Example  :  $test = $seqhound->SHoundTaxIDFrom3DList("11,12");
              @reply = $test->getValues();
  Note     :  The order in the lists is not maintained

=cut

sub SHoundTaxIDFrom3DList
{
   my $self = shift;
   my (@pvnMmdbid) = @_;
   my @copyArr = @pvnMmdbid;
   my (@tempArr, $sh);
   $sh = new Bio::SeqHound::SeqHoundTextOb(\@pvnMmdbid);
   while($#copyArr != -1){
     @tempArr = Bio::SeqHound::SLRIMisc::MaxSizeArray(\@copyArr);
     my $Arrstr = Bio::SeqHound::SLRIMisc::Array2String(\@tempArr);
     my $arguments = "pmmdbid\=$Arrstr";
     my $fulladdress = $self->_HTTPFunctionCall("SeqHoundTaxIDFrom3DList", $arguments);
     my $content = get($fulladdress);
     my $str = $self->_ProcessContent($content, "SHoundTaxIDFrom3DList");
     $sh->mergeValues($str);
   }
   return $sh;
}


=head2 SHoundProteinsFromTaxID

  Title    :  SHoundProteinsFromTaxID
  Function :  Gets all protein GI identifiers from one organism.
  Usage    :  $seqhound->SHoundProteinsFromTaxID($taxid);
  Returns  :  SeqHoundTextOb
  	      value field: list of protein GI identifiers or undef if failure
  Args     :  taxonomy identifier
  Example  :  $test = $seqhound->SHoundProteinsFromTaxID(10665);
              @reply = $test->getValues();

=cut

sub SHoundProteinsFromTaxID
{
   my $self = shift;
   my ($taxid) = @_;
   my @arr = ($taxid);
   my $arguments = "taxid\=$taxid";
   my $fulladdress = $self->_HTTPFunctionCall("SeqHoundProteinsFromTaxID", $arguments);
   my $content = get($fulladdress);
   my $str = $self->_ProcessContent($content, "SHoundProteinsFromTaxID");
   my $sh = new Bio::SeqHound::SeqHoundTextOb(\@arr);
   $sh->mergeValues($str);
   return $sh;
}


=head2 SHoundProteinsFromTaxIDList

  Title    :  SHoundProteinsFromTaxIDList
  Function :  Gets all protein GI identifiers from a list of organisms.
  Usage    :  $seqhound->SHoundProteinsFromTaxIDList(@taxidlist);
  Returns  :  SeqHoundTextOb
  	      value field: list of protein GI identifiers or undef if failure
  Args     :  comma delimited list of taxonomy identifiers
  Example  :  $test = $seqhound->SHoundProteinsFromTaxIDList("10665,555");
              @reply = $test->getValues();
  Note     :  The order in the lists is not maintained

=cut

sub SHoundProteinsFromTaxIDList
{
   my $self = shift;
   my (@pvntaxid) = @_;
   my @copyArr = @pvntaxid;
   my (@tempArr, $sh);
   $sh = new Bio::SeqHound::SeqHoundTextOb(\@pvntaxid);
   while($#copyArr != -1){
     @tempArr = Bio::SeqHound::SLRIMisc::MaxSizeArray(\@copyArr);
     my $Arrstr = Bio::SeqHound::SLRIMisc::Array2String(\@tempArr);
     my $arguments = "ptaxid\=$Arrstr";
     my $fulladdress = $self->_HTTPFunctionCall("SeqHoundProteinsFromTaxIDList", $arguments);
     my $content = get($fulladdress);
     my $str = $self->_ProcessContent($content, "SHoundProteinsFromTaxIDList");
     $sh->mergeValues($str);
   }
   return $sh;
}


=head2 SHoundDNAFromTaxID

  Title    :  SHoundDNAFromTaxID
  Function :  Gets all nucleic acid sequence identifiers from one organism.
  Usage    :  $seqhound->SHoundDNAFromTaxID($taxid);
  Returns  :  SeqHoundTextOb
  	      value field: list of protein GI identifiers or undef if failure
  Args     :  taxonomy identifier
  Example  :  $test = $seqhound->SHoundDNAFromTaxID(10665);
              @reply = $test->getValues();

=cut

sub SHoundDNAFromTaxID
{
   my $self = shift;
   my ($taxid) = @_;
   my @arr = ($taxid);
   my $arguments = "taxid\=$taxid";
   my $fulladdress = $self->_HTTPFunctionCall("SeqHoundDNAFromTaxID", $arguments);
   my $content = get($fulladdress);
   my $str = $self->_ProcessContent($content, "SHoundDNAFromTaxID");
   my $sh = new Bio::SeqHound::SeqHoundTextOb(\@arr);
   $sh->mergeValues($str);
   return $sh;
}


=head2 SHoundDNAFromTaxIDList

  Title    :  SHoundDNAFromTaxIDList
  Function :  Gets all nucleic acid sequence identifiers from a list of organisms.
  Usage    :  $seqhound->SHoundDNAFromTaxIDList(@taxidlist);
  Returns  :  SeqHoundTextOb
  	      value field: list of protein GI identifiers or undef if failure
  Args     :  comma delimited list of taxonomy identifiers
  Example  :  $test = $seqhound->SHoundDNAFromTaxIDList("10665","555");
              @reply = $test->getValues();
  Note     :  The order in the lists is not maintained

=cut

sub SHoundDNAFromTaxIDList
{
   my $self = shift;
   my (@pvntaxid) = @_;
   my @copyArr = @pvntaxid;
   my (@tempArr, $sh);
   $sh = new Bio::SeqHound::SeqHoundTextOb(\@pvntaxid);
   while($#copyArr != -1){
     @tempArr = Bio::SeqHound::SLRIMisc::MaxSizeArray(\@copyArr);
     my $Arrstr = Bio::SeqHound::SLRIMisc::Array2String(\@tempArr);
     my $arguments = "ptaxid\=$Arrstr";
     my $fulladdress = $self->_HTTPFunctionCall("SeqHoundDNAFromTaxIDList", $arguments);
     my $content = get($fulladdress);
     my $str = $self->_ProcessContent($content, "SHoundDNAFromTaxIDList");
     $sh->mergeValues($str);
   }
   return $sh;
}


=head2 SHoundProteinsFromTaxIDIII

  Title    :  SHoundProteinsFromTaxIDIII
  Function :  Gets all protein GI identifiers from one organism. The function takes advantage
              of precomputed searches if the appropriate one is available.
  Usage    :  $seqhound->SHoundProteinsFromTaxIDIII($taxid);
  Returns  :  SeqHoundTextOb
  	      value field: list of protein GI identifiers or undef if failure
  Args     :  taxonomy identifier
  Example  :  $test = $seqhound->SHoundProteinsFromTaxIDIII(10665);
              @reply = $test->getValues();

=cut

sub SHoundProteinsFromTaxIDIII
{
   my $self = shift;
   my ($taxid) = @_;
   my $arguments = "taxid\=$taxid";
   my @arr = ($taxid);
   my $fulladdress = $self->_HTTPFunctionCall("SeqHoundProteinsFromTaxIDIII", $arguments);
   my $content = get($fulladdress);
   my $str = $self->_ProcessContent($content, "SHoundProteinsFromTaxIDIII");
   my $sh = new Bio::SeqHound::SeqHoundTextOb(\@arr);
   $sh->mergeValues($str);
   return $sh;
}

=head2 SHoundDNAFromTaxIDIII

  Title    :  SHoundDNAFromTaxIDIII
  Function :  Gets all nucleic acid sequence identifiers from one organism.  The function
              takes advantage of precomputed searches if the appropriate one is available.
  Usage    :  $seqhound->SHoundDNAFromTaxIDIII($taxid);
  Returns  :  SeqHoundTextOb
  	      value field: a comma delimited list of protein GI identifiers or undef if failure
  Args     :  taxonomy identifier
  Example  :  $test = $seqhound->SHoundDNAFromTaxIDIII(10665);
              @reply = $test->getValues();

=cut

sub SHoundDNAFromTaxIDIII
{
   my $self = shift;
   my ($taxid) = @_;
   my @arr = ($taxid);
   my $arguments = "taxid\=$taxid";
   my $fulladdress = $self->_HTTPFunctionCall("SeqHoundDNAFromTaxIDIII", $arguments);
   my $content = get($fulladdress);
   my $str = $self->_ProcessContent($content, "SHoundDNAFromTaxIDIII");
   my $sh = new Bio::SeqHound::SeqHoundTextOb(\@arr);
   $sh->mergeValues($str);
   return $sh;
}


=head1 Complete Genome Iterators



=head2 SHoundProteinsFromOrganism

  Title    :  SHoundProteinsFromOrganism
  Function :  Gets all protein GI identifiers from a complete genome annotation. Complete
              genome annotations are available from NCBI.
  Usage    :  $seqhound->SHoundProteinsFromOrganism($taxid, $filter);
  Returns  :  SeqHoundTextOb
  	      value field: list of protein GI identifiers or undef if failure
  Args     :  taxonomy identifier
              filter integer which represents different kinds of DNA molecule (chromosome,
              plasmid etc.)
  Example  :  $test = $seqhound->SHoundProteinsFromOrganism(155864, 1);
              @reply = $test->getValues();
  Note     :  The arguments concerning DNA molecule type are so called "byte flags" where each
               bit is reserved for one kind of DNA molecule:
                           1 is chromosome
                           2 is mitochondrion
                           4 is chloroplast
                           8 is plasmid
                           16 is extra-chromosomal element
                           32 is plastid
                           64 is phage
                           128 is miscellaneous
              the flags are additive; for example 255 means that all kinds of DNA molecules in
               a given organism are requested

=cut

sub SHoundProteinsFromOrganism
{
   my $self = shift;
   my ($taxid, $chromflag) = @_;
   my @arr = ($taxid, $chromflag);
   $chromflag = $self->_DecodeChromFlag($chromflag);
   my $arguments = "taxid\=$taxid"."\&chromflags\=$chromflag";
   my $fulladdress = $self->_HTTPFunctionCall("SeqHoundProteinsFromOrganism", $arguments);
   my $content = get($fulladdress);
   my $str = $self->_ProcessContent($content, "SHoundProteinsFromOrganism");
   my $sh = new Bio::SeqHound::SeqHoundTextOb(\@arr);
   $sh->mergeValues($str);
   return $sh;
}

=head2 SHoundProteinsFromOrganismList

  Title    :  SHoundProteinsFromOrganismList
  Function :  Gets all protein GI identifiers from a complete genomes list. Complete genome
              annotations are available from NCBI.
  Usage    :  $seqhound->SHoundProteinsFromOrganismList(@taxidlist, $filter);
  Returns  :  SeqHoundTextOb
  	      value field: list of protein GI identifiers or undef if failure
  Args     :  comma delimited list of taxonomy identifiers
              a filter integer which represents different kinds of DNA molecule (chromosome,
              plasmid etc.)
  Example  :  $test = $seqhound->SHoundProteinsFromOrganismList("155864,139", 1);
              @reply = $test->getValues();
  Note     :  The order in the lists is not maintained
           :  The arguments concerning DNA molecule type are so called "byte flags" where each
               bit is reserved for one kind of DNA molecule:
                           1 is chromosome
                           2 is mitochondrion
                           4 is chloroplast
                           8 is plasmid
                           16 is extra-chromosomal element
                           32 is plastid
                           64 is phage
                           128 is miscellaneous
              the flags are additive; for example 255 means that all kinds of DNA molecules in
               a given organism are requested

=cut

sub SHoundProteinsFromOrganismList
{
   my $self = shift;
   my ($chromflag, @pvnTax) = @_;
   $chromflag = $self->_DecodeChromFlag($chromflag);
   my @copyArr = @pvnTax;
   my (@tempArr, $sh);
   $sh = new Bio::SeqHound::SeqHoundTextOb(\@pvnTax);
   while($#copyArr != -1){
     @tempArr = Bio::SeqHound::SLRIMisc::MaxSizeArray(\@copyArr);
     my $Arrstr = Bio::SeqHound::SLRIMisc::Array2String(\@tempArr);
     my $arguments = "ptaxid\=$Arrstr"."\&chromflags\=$chromflag";
     my $fulladdress = $self->_HTTPFunctionCall("SeqHoundProteinsFromOrganismList", $arguments);
     my $content = get($fulladdress);
     my $str = $self->_ProcessContent($content, "SHoundProteinsFromOrganismList");
     $sh->mergeValues($str);
   }  
   return $sh;
}

=head2 SHoundProteinsFromChromosome

  Title    :  SHoundProteinsFromChromosome
  Function :  Gets all protein GI identifiers from one DNA molecule such as chromosome,
              plasmid etc.  The resulting protein GI identifiers constitute a subset of
              complete genome annotation available from NCBI.
  Usage    :  $seqhound->SHoundProteinsFromChromosome($chromosomalid);
  Returns  :  SeqHoundTextOb
  	      value field: list of protein GI identifiers or undef if failure
  Args     :  chromosomal ID which refers to a chromosome or plasmid etc.
  Example  :  $test = $seqhound->SHoundProteinsFromChromosome(169);
              @reply = $test->getValues();
  Note     :  Chromosomal identifier which refers to any DNA molecule can be obtained from
              chromff file in seqhound development tree in slri/seqhound/genomes/ directory

=cut

sub SHoundProteinsFromChromosome
{
   my $self = shift;
   my ($chromid) = @_;
   my $arguments = "chromid\=$chromid";
   my @arr = ($chromid);
   my $fulladdress = $self->_HTTPFunctionCall("SeqHoundProteinsFromChromosome", $arguments);
   my $content = get($fulladdress);
   my $str = $self->_ProcessContent($content, "SHoundProteinsFromChromosome");
   my $sh = new Bio::SeqHound::SeqHoundTextOb(\@arr);
   $sh->mergeValues($str);
   return $sh;
}

=head2 SHoundProteinsFromUnknownChromosome

  Title    :  SHoundProteinsFromUnknownChromosome
  Function :  Gets all proteins identifiers which have not been assigned to a specific chromosome
              from a complete genome.
  Usage    :  $seqhound->SHoundProteinsFromChromosome($taxid);
  Returns  :  SeqHoundTextOb
  	      value field: list of protein GI identifiers or undef if failure
  Args     :  taxonomy identifier
  Example  :  $test = $seqhound->SHoundProteinsFromChromosome(155864);
              @reply = $test->getValues();
  Note     :  Chromosomal identifier which refers to any DNA molecule can be obtained from
              chromff file in seqhound development tree in slri/seqhound/genomes/ directory

=cut

sub SHoundProteinsFromUnknownChromosome
{
   my $self = shift;
   my ($taxid) = @_;
   my $arguments = "taxid\=$taxid";
   my @arr = ($taxid);
   my $fulladdress = $self->_HTTPFunctionCall("SeqHoundProteinsFromUnknownChromosome", $arguments);
   my $content = get($fulladdress);
   my $str = $self->_ProcessContent($content, "SHoundProteinsFromUnknownChromosome");
   my $sh = new Bio::SeqHound::SeqHoundTextOb(\@arr);
   $sh->mergeValues($str);
   return $sh;
}

=head2 SHoundProteinsFromChromosomeList

  Title    :  SHoundProteinsFromChromosomeList
  Function :  Gets all protein GI identifiers from a list DNA molecules such as chromosomes,
              plasmids etc. The resulting protein GI identifiers constitute a subset of
              complete genome annotation available from NCBI.
  Usage    :  $seqhound->SHoundProteinsFromChromosomeList(@chromosomalidlist);
  Returns  :  SeqHoundTextOb
  	      value field: list of protein GI identifiers or undef if failure
  Args     :  comma delimited list of chromosomal IDs or undef if failure
  Example  :  $test = $seqhound->SHoundProteinsFromChromosomeList("169,132");
              @reply = $test->getValues();
  Note     :  The order in the lists is not maintained
           :  Chromosomal identifier which refers to any DNA molecule can be obtained from
              chromff file in seqhound development tree in slri/seqhound/genomes/ directory

=cut

sub SHoundProteinsFromChromosomeList
{
   my $self = shift;
   my (@pvnChrom) = @_;
   my @copyArr = @pvnChrom;
   my (@tempArr, $sh);
   $sh = new Bio::SeqHound::SeqHoundTextOb(\@pvnChrom);
   while($#copyArr != -1){
     @tempArr = Bio::SeqHound::SLRIMisc::MaxSizeArray(\@copyArr);
     my $Arrstr = Bio::SeqHound::SLRIMisc::Array2String(\@tempArr);
     my $arguments = "pchromid\=$Arrstr";
     my $fulladdress = $self->_HTTPFunctionCall("SeqHoundProteinsFromChromosomeList", $arguments);
	 my $content = get($fulladdress);
     my $str = $self->_ProcessContent($content, "SHoundProteinsFromChromosomeList");
     $sh->mergeValues($str);
   }
   return $sh;
}


=head2 SHoundRNAFromChromosome

  Title    :  SHoundRNAFromChromosome
  Function :  Gets all RNA identifiers from a NCBI chromosome identifier.
  Usage    :  $seqhound->SHoundRNAFromChromosome($chromid)
  Returns  :  SeqHoundTextOb
  	      value field: list of RNA GI identifiers or undef
  Args     :  NCBI chromosomal identifier
  Example  :  $test = $seqhound->SHoundRNAFromChromosome(169);
              @reply = $test->getValues();

=cut

sub SHoundRNAFromChromosome
{
   my $self = shift;
   my ($chromid) = @_;
   my $arguments = "chromid\=$chromid";
   my @arr = ($chromid);
   my $fulladdress = $self->_HTTPFunctionCall("SeqHoundRNAFromChromosome", $arguments);
   my $content = get($fulladdress);
   my $str = $self->_ProcessContent($content, "SHoundRNAFromChromosome");
   my $sh = new Bio::SeqHound::SeqHoundTextOb(\@arr);
   $sh->mergeValues($str);
   return $sh;
}

=head2 SHoundRNAFromUnknownChromosome

  Title    :  SHoundRNAFromUnknownChromosome
  Function :  Gets all RNA identifiers which have not been assigned to a specific chromosome
              from a complete genome.
  Usage    :  $seqhound->SHoundRNAFromUnknownChromosome($taxid);
  Returns  :  SeqHoundTextOb
  	      value field: list of RNA GI identifiers or undef
  Args     :  taxonomy identifier
  Example  :  $test = $seqhound->SHoundRNAFromUnknownChromosome(155864);
              @reply = $test->getValues();

=cut

sub SHoundRNAFromUnknownChromosome
{
   my $self = shift;
   my ($taxid) = @_;
   my $arguments = "taxid\=$taxid";
   my @arr = ($taxid);
   my $fulladdress = $self->_HTTPFunctionCall("SeqHoundRNAFromUnknownChromosome", $arguments);
   my $content = get($fulladdress);
   my $str = $self->_ProcessContent($content, "SHoundRNAFromUnknownChromosome");
   my $sh = new Bio::SeqHound::SeqHoundTextOb(\@arr);
   $sh->mergeValues($str);
   return $sh;
}


=head2 SHoundRNAFromOrganism

  Title    :  SHoundRNAFromOrganism
  Function :  Gets all RNA identifiers from a complete genome annotation. Complete genome
              annotations are available from NCBI.
  Returns  :  SeqHoundTextOb
  	      value field: list of RNA GI identifiers or undef
  Usage    :  $seqhound->SHoundRNAFromOrganism($taxid, $chromflag);
  Args     :  taxonomy identifier and a filter integer representing different kinds of RNA
              molecule (see comment)
  Example  :  SHoundRNAFromOrganism(155864, 1);
  Note     :  The arguments concerning RNA molecule type are so called "byte flags" where each
              bit is reserved for one kind of RNA molecule:
                           1 is chromosome
                           2 is mitochondrion
                           4 is chloroplast
                           8 is plasmid
                           16 is extra-chromosomal element
                           32 is plastid
                           64 is phage
                           128 is miscellaneous
              the flags are additive; for example 255 means that all kinds of RNA molecules in
              a given organism are requested

=cut

sub SHoundRNAFromOrganism
{
   my $self = shift;
   my ($taxid, $chromflag) = @_;
   my @arr; $arr[0] = $taxid; $arr[1] = $chromflag;
   $chromflag = $self->_DecodeChromFlag($chromflag);
   my $arguments = "taxid\=$taxid"."\&chromflags\=$chromflag";
   my $fulladdress = $self->_HTTPFunctionCall("SeqHoundRNAFromOrganism", $arguments);
   my $content = get($fulladdress);
   my $str = $self->_ProcessContent($content, "SHoundRNAFromOrganism");
   my $sh = new Bio::SeqHound::SeqHoundTextOb(\@arr);
   $sh->mergeValues($str);
   return $sh;
}

=head2 SHoundAllGenomes

  Title    :  SHoundAllGenomes
  Function :  Gets a list of taxonomy identifiers for all complete genomes present in the
              SeqHound system. Complete genome annotations are available from NCBI.
  Usage    :  $seqhound->SHoundAllGenomes();
  Returns  :  SeqHoundTextOb
  	      value field: list of taxonomy identifiers or undef if failure
  Args     :  none
  Example  :  $test = $seqhound->SHoundAllGenomes();
              @reply = $test->getValues();

=cut

sub SHoundAllGenomes
{
   my $self =shift;
   my @arr = ("");
   my $fulladdress = $self->_HTTPFunctionCall("SeqHoundAllGenomes", "");
   my $content = get($fulladdress);
   my $str = $self->_ProcessContent($content, "SHoundAllGenomes");
   my $sh = new Bio::SeqHound::SeqHoundTextOb(\@arr);
   $sh->mergeValues($str);
   return $sh;
}

=head2 SHoundChromosomeFromGenome

  Title    :  SHoundChromosomeFromGenome
  Function :  Gets a list of DNA molecules from a complete genome.  The desired return types
              of DNA molecules can be filtered.
  Usage    :  $seqhound->SHoundChromosomeFromGenome($taxid, $filter);
  Returns  :  SeqHoundTextOb
  	      value field: list of chromosomal IDs (e.g. "115") or undef if failure
  Args     :  taxonomy identifier
              filter integer which represents different kinds of DNA molecule (chromosome,
              plasmid etc.)
  Example  :  $test = $seqhound->SHoundChromosomeFromGenome(155864, 1);
              @reply = $test->getValues();
  Note     :  The arguments concerning DNA molecule type are so called "byte  flags" where
              each bit is reserved for one kind of DNA molecule:
                           1 is chromosome
                           2 is mitochondrion
                           4 is chloroplast
                           8 is plasmid
                           16 is extra-chromosomal element
                           32 is plastid
                           64 is phage
                           128 is miscellaneous
              the flags are additive; for example 255 means that all kinds of DNA molecules in
               a given organism are requested

=cut

sub SHoundChromosomeFromGenome
{
   my $self = shift;
   my ($taxid, $chromflag) = @_;
   my @arr = ($taxid, $chromflag);
   $chromflag = $self->_DecodeChromFlag($chromflag);
   my $arguments = "taxid\=$taxid"."\&chromflags\=$chromflag";
   my $fulladdress = $self->_HTTPFunctionCall("SeqHoundChromosomeFromGenome", $arguments);
   my $content = get($fulladdress);
   my $str = $self->_ProcessContent($content, "SHoundChromosomeFromGenome");
   my $sh = new Bio::SeqHound::SeqHoundTextOb(\@arr);
   $sh->mergeValues($str);
   return $sh;
}


=head2 SHoundChromosomeFromGenomeList

  Title    :  SHoundChromosomeFromGenomeList
  Function :  Gets a list of DNA molecules from a complete genome list.  The desired return
              types of DNA molecules can be filtered.
  Usage    :  $seqhound->SHoundChromosomeFromGenomeList(@taxidlist, $filter);
  Returns  :  SeqHoundTextOb
  	      value field: list of chromosomal IDs (e.g. "115,130") or undef if failure
  Args     :  comma delimited list of taxonomy identifiers
              filter integer which represents different kinds of DNA molecule (chromosome,
              plasmid etc.)
  Example  :  $test = $seqhound->SHoundChromosomeFromGenomeList("155864", 1);
              @reply = $test->getValues();
  Note     :  The arguments concerning DNA molecule type are so called "byte  flags" where
              each bit is reserved for one kind of DNA molecule:
                           1 is chromosome
                           2 is mitochondrion
                           4 is chloroplast
                           8 is plasmid
                           16 is extra-chromosomal element
                           32 is plastid
                           64 is phage
                           128 is miscellaneous
              the flags are additive; for example 255 means that all kinds of DNA molecules in
               a given organism are requested
              The order in the lists is not maintained

=cut

sub SHoundChromosomeFromGenomeList
{
   my $self = shift;
   my ($chromflag, @pvnGenom) = @_;
   my @copyArr = @pvnGenom;
   my (@tempArr, $sh);
   push (@pvnGenom, $chromflag);
   $sh = new Bio::SeqHound::SeqHoundTextOb(\@pvnGenom);
   while($#copyArr != -1){
     @tempArr = Bio::SeqHound::SLRIMisc::MaxSizeArray(\@copyArr);
     $chromflag = $self->_DecodeChromFlag($chromflag);
     my $Arrstr = Bio::SeqHound::SLRIMisc::Array2String(\@tempArr);
     my $arguments = "ptaxid\=$Arrstr"."\&chromflags\=$chromflag";
     my $fulladdress = $self->_HTTPFunctionCall("SeqHoundChromosomeFromGenomeList", $arguments);
     my $content = get($fulladdress);
     my $str = $self->_ProcessContent($content, "SHoundChromosomeFromGenomeList");
     $sh->mergeValues($str);
   }
   return $sh;
}


=head2 SHoundDNAFromOrganism

  Title    :  SHoundDNAFromOrganism
  Function :  Gets all DNA identifiers from a complete genome annotation. Complete genome
              annotations are available from NCBI.
  Usage    :  $seqhound->SHoundDNAFromOrganism(6239, 1);
  Returns  :  SeqHoundTextOb
  	      value field: list of DNA GI identifiers or undef if failure
  Args     :  taxonomy identifier
              filter integer which represents different kinds of DNA molecule (chromosome,
              plasmid etc.)
  Example  :  $test = $seqhound->SHoundDNAFromOrganism(6239, 1);
              @reply = $test->getValues();
  Note     :  The arguments concerning DNA molecule type are so called "byte flags" where each
               bit is reserved for one kind of DNA molecule:
                           1 is chromosome
                           2 is mitochondrion
                           4 is chloroplast
                           8 is plasmid
                           16 is extra-chromosomal element
                           32 is plastid
                           64 is phage
                           128 is miscellaneous
              the flags are additive; for example 255 means that all kinds of DNA molecules in
               a given organism are requested

=cut

sub SHoundDNAFromOrganism
{
   my $self = shift;
   my ($taxid, $chromflag) = @_;
   my @arr; $arr[0] = $taxid; $arr[1] = $chromflag;
   $chromflag = $self->_DecodeChromFlag($chromflag);
   my $arguments = "taxid\=$taxid"."\&chromflags\=$chromflag";
   my $fulladdress = $self->_HTTPFunctionCall("SeqHoundDNAFromOrganism", $arguments);
   my $content = get($fulladdress);
   my $str = $self->_ProcessContent($content, "SHoundDNAFromOrganism");
   my $sh = new Bio::SeqHound::SeqHoundTextOb(\@arr);
   $sh->mergeValues($str);
   return $sh;
}

=head2 SHoundDNAFromOrganismList

  Title    :  SHoundDNAFromOrganismList
  Function :  Gets all DNA identifiers from a complete genomes list. Complete genome
              annotations are available from NCBI.
  Usage    :  $seqhound->SHoundDNAFromOrganismList(@taxidlist, $filter);
  Returns  :  SeqHoundTextOb
  	      value field: list of DNA GI identifiers  or undef if failure
  Args     :  comma delimited list of taxonomy identifiers
              filter integer which represents different kinds of DNA molecule (chromosome,
              plasmid etc.)
  Example  :  $test = $seqhound->SHoundDNAFromOrganismList("6239", 255);
              @reply = $test->getValues();
  Note     :  The arguments concerning DNA molecule type are so called "byte flags" where each
               bit is reserved for one kind of DNA molecule:
                           1 is chromosome
                           2 is mitochondrion
                           4 is chloroplast
                           8 is plasmid
                           16 is extra-chromosomal element
                           32 is plastid
                           64 is phage
                           128 is miscellaneous
              the flags are additive; for example 255 means that all kinds of DNA molecules in
               a given organism are requested
              The order in the lists is not maintained

=cut

sub SHoundDNAFromOrganismList
{
   my $self = shift;
   my ($chromflag, @pvnTax) = @_;
   my @copyArr = @pvnTax;
   push(@pvnTax, $chromflag);
   my (@tempArr, $sh);
   $sh = new Bio::SeqHound::SeqHoundTextOb(\@pvnTax);
   while($#copyArr != -1){
     @tempArr = Bio::SeqHound::SLRIMisc::MaxSizeArray(\@copyArr);
     my $Arrstr = Bio::SeqHound::SLRIMisc::Array2String(\@tempArr);
     $chromflag = $self->_DecodeChromFlag($chromflag);
     my $arguments = "ptaxid\=$Arrstr"."\&chromflags\=$chromflag";
     my $fulladdress = $self->_HTTPFunctionCall("SeqHoundDNAFromOrganismList", $arguments);
     my $content = get($fulladdress);
     my $str = $self->_ProcessContent($content, "SHoundDNAFromOrganismList");
     $sh->mergeValues($str);
   } 
   return $sh;
}


=head2 SHoundDNAFromChromosome

  Title    :  SHoundDNAFromChromosome
  Function :  Gets all DNA identifiers from one DNA molecule such as chromosome, plasmid etc.
              The resulting DNA identifiers constitute a subset of complete genome annotation
              available from NCBI.
  Usage    :  $seqhound->SHoundDNAFromChromosome($chromosomalid);
  Returns  :  SeqHoundTextOb
  	      value field: list of DNA GI identifiers or undef if failure
  Args     :  chromosomal ID which refers to a chromosome or plasmid etc.
  Example  :  $test = $seqhound->SHoundDNAFromChromosome(145);
              @reply = $test->getValues();

=cut

sub SHoundDNAFromChromosome
{
   my $self = shift;
   my ($chromid) = @_;
   my @arr = ($chromid);
   my $arguments = "chromid\=$chromid";
   my $fulladdress = $self->_HTTPFunctionCall("SeqHoundDNAFromChromosome", $arguments);
   my $content = get($fulladdress);
   my $str = $self->_ProcessContent($content, "SHoundDNAFromChromosome");
   my $sh = new Bio::SeqHound::SeqHoundTextOb(\@arr);
   $sh->mergeValues($str);
   return $sh;
}

=head2 SHoundDNAFromUnknownChromosome

  Title    :  SHoundDNAFromUnknownChromosome
  Function :  Gets all DNA identifiers which have not been assigned to a specific chromosome
              from a complete genome.
  Usage    :  $seqhound->SHoundDNAFromChromosome($taxid);
  Returns  :  SeqHoundTextOb
  	      value field: list of DNA GI identifiers or undef
  Args     :  taxonomy identifier
  Example  :  $test = $seqhound->SHoundDNAFromChromosome(155864);
              @reply = $test->getValues();

=cut

sub SHoundDNAFromUnknownChromosome
{
   my $self = shift;
   my ($taxid) = @_;
   my @arr = ($taxid);
   my $arguments = "taxid\=$taxid";
   my $fulladdress = $self->_HTTPFunctionCall("SeqHoundDNAFromUnknownChromosome", $arguments);
   my $content = get($fulladdress);
   my $str = $self->_ProcessContent($content, "SHoundDNAFromUnknownChromosome");
   my $sh = new Bio::SeqHound::SeqHoundTextOb(\@arr);
   $sh->mergeValues($str);
   return $sh;
}


=head2 SHoundDNAFromChromosomeList

  Title    :  SHoundDNAFromChromosomeList
  Function :  Gets all DNA identifiers from a list DNA molecules such as chromosomes, plasmids
               etc. The resulting DNA identifiers constitute a subset of complete genome
              annotation available from NCBI.
  Usage    :  $seqhound->SHoundDNAFromChromosomeList(@chromosomalidlist);
  Returns  :  SeqHoundTextOb
  	      value field: list of DNA GI identifiers or undef if failure
  Args     :  comma delimited list of chromosomal IDs
  Example  :  $test = $seqhound->SHoundDNAFromChromosomeList("143,145");
              @reply = $test->getValues();
  Note     :  The order in the lists is not maintained

=cut



sub SHoundDNAFromChromosomeList
{
   my $self = shift;
   my (@pvnChrom) = @_;
   my @copyArr = @pvnChrom;
   my (@tempArr, $sh);
   $sh = new Bio::SeqHound::SeqHoundTextOb(\@pvnChrom);
   while($#copyArr != -1){
     @tempArr = Bio::SeqHound::SLRIMisc::MaxSizeArray(\@copyArr);
     my $Arrstr = Bio::SeqHound::SLRIMisc::Array2String(\@tempArr);
     my $arguments = "pchromid\=$Arrstr";
     my $fulladdress = $self->_HTTPFunctionCall("SeqHoundDNAFromChromosomeList", $arguments);
     my $content = get($fulladdress);
     my $str = $self->_ProcessContent($content, "SHoundDNAFromChromosomeList");
     $sh->mergeValues($str);
   }
   return $sh;
}

=head1 Redundant (Equivalent) Sequences

=head2 SHoundRedundantGroup

  Title    :  SHoundRedundantGroup
  Function :  Gets all protein entries with the same sequence from the system.
              Entrez collection of databases contains redundant entries i.e. proteins with the
               same sequence but different annotations.
  Usage    :  $seqhound->SHoundRedundantGroup($gi);
  Returns  :  SeqHoundTextOb
  	      value field: list of protein GI identifiers or undef if failure
  Args     :  GI protein identifier
  Example  :  $test = $seqhound->SHoundRedundantGroup(32);
              @reply = $test->getValues();

=cut

sub SHoundRedundantGroup
{
   my $self = shift;
   my ($Gi) = @_;
   my @arr = ($Gi);
   my $arguments = "gi\=$Gi";
   my $fulladdress = $self->_HTTPFunctionCall("SeqHoundRedundantGroup", $arguments);
   my $content = get($fulladdress);
   my $str = $self->_ProcessContent($content, "SHoundRedundantGroup");
   my $sh = new Bio::SeqHound::SeqHoundTextOb(\@arr);
   $sh->mergeValues($str);
   return $sh;
}

=head2 SHoundRedundantGroupList

  Title    :  SHoundRedundantGroupList
  Function :  Gets all protein entries with the same sequence from the system for a list of
              protein sequences.
              Entrez collection of databases contains redundant entries i.e. proteins with the
               same sequence but different annotations.
  Usage    :  $seqhound->SHoundRedundantGroupList(@gilist);
  Returns  :  SeqHoundTextOb
  	      value field: list of protein GI identifiers or undef if failure
  Args     :  comma delimited list of protein GI identifiers
  Example  :  $test = $seqhound->SHoundRedundantGroupList("32,4586891");
              @reply = $test->getValues();
  Note     :  The order in the lists is not maintained

=cut

sub SHoundRedundantGroupList
{
   my $self = shift;
   my (@pvnGi) = @_;
   my @copyArr = @pvnGi;
   my (@tempArr, $sh);
   $sh = new Bio::SeqHound::SeqHoundTextOb(\@pvnGi);
   while($#copyArr != -1){
     @tempArr = Bio::SeqHound::SLRIMisc::MaxSizeArray(\@copyArr);
     my $Arrstr = Bio::SeqHound::SLRIMisc::Array2String(\@tempArr);
     my $arguments = "pgi\=$Arrstr";
     my $fulladdress = $self->_HTTPFunctionCall("SeqHoundRedundantGroupList", $arguments);
     my $content = get($fulladdress);
     my $str = $self->_ProcessContent($content, "SHoundRedundantGroupList");
     $sh->mergeValues($str);
   }
   return $sh;
}

=head2 SHoundRedundantGroupKeyedList

  Title    :  SHoundRedundantGroupKeyedList
  Function :  Gets all protein GI identifiers with the same sequence as the query protein GI
              identifier.
              Entrez collection of databases contains redundant entries i.e. proteins with the
               same sequence but different annotations.
  Usage    :  $seqhound->SHoundRedundantGroupKeyedList(@gilist);
  Returns  :  a hash table, each entry of the table stores a query protein GI identifier and
              its associated list of redundant group protein GI identifiers.
              or an empty hash table if failure
  Args     :  comma delimited list of protein GI identifiers
  Example  :  %hash = $seqhound->SHoundRedundantGroupKeyedList("4557225,234,123");


=cut

sub SHoundRedundantGroupKeyedList
{
   
   my $self = shift;
   my (@pvnGi) = @_;
   my @copyArr = @pvnGi;
   my (@tempArr, $sh);
   my %hash;
   $sh = new Bio::SeqHound::SeqHoundTextOb(\@pvnGi);
   while($#copyArr != -1){
     @tempArr = Bio::SeqHound::SLRIMisc::MaxSizeArray(\@copyArr);
     my $Arrstr = Bio::SeqHound::SLRIMisc::Array2String(\@tempArr);
     my $arguments = "pgi\=$Arrstr";
     my $fulladdress = $self->_HTTPFunctionCall("SeqHoundRedundantGroupKeyedList", $arguments);
     my $content = get($fulladdress);
     %hash = $self->_ProcessContentKeyedList($content, "SHoundRedundantGroupKeyedList", %hash);
   }
   return %hash;
}

=head2 SHoundRedundantGroupIDFromGI

  Title    :  SHoundRedundantGroupIDFromGI
  Function :  Gets an arbitrary redundant group identifier for a protein sequence.
              Entrez collection of databases contains redundant entries i.e. proteins with the
               same sequence but different annotations.  The redundant group identifiers can
              only be used temporarily, they change every day.
  Usage    :  $seqhound->SHoundRedundantGroupIDFromGI($gi);
  Returns  :  SeqHoundTextOb
  	      value field: redundant group identifier (e.g. 362477)
  Args     :  GI identifier
  Example  :  $test = $seqhound->SHoundRedundantGroupIDFromGI(32);
              @reply = $test->getValues();


=cut

sub SHoundRedundantGroupIDFromGI
{
   my $self = shift;
   my ($Gi) = @_;
   my @arr = ($Gi); 
   my $arguments = "gi\=$Gi";
   my $fulladdress = $self->_HTTPFunctionCall("SeqHoundRedundantGroupIDFromGI", $arguments);
   my $content = get($fulladdress);
   my $str = $self->_ProcessContent($content, "SHoundRedundantGroupIDFromGI");
   my $sh = new Bio::SeqHound::SeqHoundTextOb(\@arr);
   $sh->mergeValues($str);
   return $sh;
}


=head2 SHoundRedundantGroupIDFromGIList

  Title    :  SHoundRedundantGroupIDFromGIList
  Function :  Gets an arbitrary redundant group identifier for a list of protein sequences.
              Entrez collection of databases contains redundant entries i.e. proteins with the
               same sequence but different annotations.  The redundant group identifiers can
              only be used temporarily, they change every day.
  Usage    :  $seqhound->SHoundRedundantGroupIDFromGIList(@gilist);
  Returns  :  SeqHoundTextOb
  	      value field: list of group identifiers or undef if failure
  Args     :  comma delimited list of protein GI identifiers
  Example  :  $test = $seqhound->SHoundRedundantGroupIDFromGIList("32","6017926");
              @reply = $test->getValues();
  Note     :  The order in the lists is not maintained.

=cut

sub SHoundRedundantGroupIDFromGIList
{
   my $self = shift;
   my (@pvngroup) = @_;
   my @copyArr = @pvngroup;
   my (@tempArr, $sh);
   $sh = new Bio::SeqHound::SeqHoundTextOb(\@pvngroup);
   while($#copyArr != -1){
     @tempArr = Bio::SeqHound::SLRIMisc::MaxSizeArray(\@copyArr);
     my $Arrstr = Bio::SeqHound::SLRIMisc::Array2String(\@tempArr);
     my $arguments = "pgi\=$Arrstr";
     my $fulladdress = $self->_HTTPFunctionCall("SeqHoundRedundantGroupIDFromGIList", $arguments);
     my $content = get($fulladdress);
     my $str = $self->_ProcessContent($content, "SHoundRedundantGroupIDFromGIList");
     $sh->mergeValues($str);
   }
   return $sh;
}


=head2 SHoundRedundantGroupFromID

  Title    :  SHoundRedundantGroupFromID
  Function :  Gets a list of redundant protein sequences from a redundant group identifier.
              Entrez collection of databases contains redundant entries i.e. proteins with the
               same sequence but different annotations.  The redundant group identifiers can
              only be used temporarily, they change every day.
  Usage    :  $seqhound->SHoundRedundantGroupFromID($redundantid);
  Returns  :  SeqHoundTextOb
  	      value field: list of protein GI identifiers or undef if failure
  Args     :  redundant group identifier
  Example  :  $test = $seqhound->SHoundRedundantGroupFromID(362477);
              @reply = $test->getValues();


=cut

sub SHoundRedundantGroupFromID
{
   my $self = shift;
   my ($group) = @_;
   my @arr = ($group);
   my $arguments = "group\=$group";
   my $fulladdress = $self->_HTTPFunctionCall("SeqHoundRedundantGroupFromID", $arguments);
   my $content = get($fulladdress);
   my $str = $self->_ProcessContent($content, "SHoundRedundantGroupFromID");
   my $sh = new Bio::SeqHound::SeqHoundTextOb(\@arr);
   $sh->mergeValues($str);
   return $sh;
}

=head2 SHoundRedundantGroupFromIDList

  Title    :  SHoundRedundantGroupFromIDList
  Function :  Gets a list of redundant protein sequences from a list of redundant group
              identifiers.
              Entrez collection of databases contains redundant entries i.e. proteins with the
               same sequence but different annotations. The redundant group identifiers can
              only be used temporarily, they change every day.
  Usage    :  $seqhound->SHoundRedundantGroupFromIDList(@redundantidlist);
  Returns  :  SeqHoundTextOb
  	      value field: list of protein GI identifiers or undef if failure
  Args     :  comma delimited list of redundant group identifiers
  Example  :  $test = $seqhound->SHoundRedundantGroupFromIDList("32","362477");
              @reply = $test->getValues();
  Note     :  The order in the lists is not maintained

=cut

sub SHoundRedundantGroupFromIDList
{
   my $self = shift;
   my (@pvngroup) = @_;
   my @copyArr = @pvngroup;
   my (@tempArr, $sh);
   $sh = new Bio::SeqHound::SeqHoundTextOb(\@pvngroup);
   while($#copyArr != -1){
     @tempArr = Bio::SeqHound::SLRIMisc::MaxSizeArray(\@copyArr);
     my $Arrstr = Bio::SeqHound::SLRIMisc::Array2String(\@tempArr);
     my $arguments = "pgroup\=$Arrstr";
     my $fulladdress = $self->_HTTPFunctionCall("SeqHoundRedundantGroupFromIDList", $arguments);
     my $content = get($fulladdress);
     my $str = $self->_ProcessContent($content, "SHoundRedundantGroupFromIDList");
     $sh->mergeValues($str);
   }
   return $sh;
}


=head2 SHoundFirstOfRedundantGroupFromID

  Title    :  SHoundFirstOfRedundantGroupFromID
  Function :  Entrez collection of databases contains redundant entries i.e. proteins with the
               same sequence but different annotations. The function gets a representative
              entry (generally the one with best annotation such as PDB, RefSeq or SwissProt
              entry) given a redundant group identifier. The redundant group identifiers can
              only be used temporarily, they change every day.
  Usage    :  $seqhound->SHoundFirstOfRedundantGroupFromID($redundantid);
  Returns  :  SeqHoundTextOb
  	      value field: protein GI identifier or undef if failure
  Args     :  redundant group identifier
  Example  :  $test = $seqhound->SHoundFirstOfRedundantGroupFromID(362477);
              @reply = $test->getValues();

=cut

sub SHoundFirstOfRedundantGroupFromID
{
   my $self = shift;
   my ($group) = @_;
   my @arr = ($group);
   my $arguments = "group\=$group";
   my $fulladdress = $self->_HTTPFunctionCall("SeqHoundFirstOfRedundantGroupFromID", $arguments);
   my $content = get($fulladdress);
   my $str = $self->_ProcessContent($content, "SHoundFirstOfRedundantGroupFromID");
   my $sh = new Bio::SeqHound::SeqHoundTextOb(\@arr);
   $sh->mergeValues($str);
   return $sh;
}



=head2 SHoundFirstOfRedundantGroupFromIDList

  Title    :  SHoundFirstOfRedundantGroupFromIDList
  Function :  Entrez collection of databases contains redundant entries i.e. proteins with the
               same sequence but different annotations. The function gets a list of
              representative entries (generally the one with best annotation such as PDB,
              RefSeq or SwissProt entry) given a list of redundant group identifiers.  The
              redundant group identifiers can only be used temporarily, they change every day.
  Usage    :  $seqhound->SHoundFirstOfRedundantGroupFromIDList(@redundantidlist);
  Returns  :  SeqHoundTextOb
  	      value field: list of protein GIs or undef if failure
  Args     :  comma delimited list of redundant group identifiers
  Example  :  $test = $seqhound->SHoundFirstOfRedundantGroupFromIDList("362477,408917");
              @reply = $test->getValues();
  Note     :  The order in the lists is not maintained.

=cut

sub SHoundFirstOfRedundantGroupFromIDList
{
   my $self = shift;
   my (@group) = @_;
   my @copyArr = @group;
   my (@tempArr, $sh);
   $sh = new Bio::SeqHound::SeqHoundTextOb(\@group);
   while($#copyArr != -1){
     @tempArr = Bio::SeqHound::SLRIMisc::MaxSizeArray(\@copyArr);
     my $Arrstr = Bio::SeqHound::SLRIMisc::Array2String(\@tempArr);
     my $arguments = "pgroup\=$Arrstr";
     my $fulladdress = $self->_HTTPFunctionCall("SeqHoundFirstOfRedundantGroupFromIDList", $arguments);
     my $content = get($fulladdress);
     my $str = $self->_ProcessContent($content, "SHoundFirstOfRedundantGroupFromIDList");
     $sh->mergeValues($str);
   }
   return $sh;
}


=head1 Taxonomy

=head2 SHoundGetTaxChildNodes

  Title    :  SHoundGetTaxChildNodes
  Function :  Operates on NCBI's taxonomy tree.  Retrieves all taxons situated directly below
              a given taxon in the taxonomy tree.
  Usage    :  $seqhound->SHoundGetTaxChildNodes($taxid);
  Returns  :  SeqHoundTextOb
  	      value field: list of taxonomy IDs or undef if failure
  Args     :  taxonomy identifier
  Example  :  $test = $seqhound->SHoundGetTaxChildNodes(10665);
              @reply = $test->getValues();

=cut

sub SHoundGetTaxChildNodes
{
   my $self = shift;
   my ($taxId) = @_;
   my @arr = ($taxId);
   my $arguments = "taxid\=$taxId";
   my $fulladdress = $self->_HTTPFunctionCall("SeqHoundGetTaxChildNodes", $arguments);
   my $content = get($fulladdress);
   my $str = $self->_ProcessContent($content, "SHoundGetTaxChildNodes");
   my $sh = new Bio::SeqHound::SeqHoundTextOb(\@arr);
   $sh->mergeValues($str);
   return $sh;
}

=head2 SHoundGetTaxChildNodesList

  Title    :  SHoundGetTaxChildNodesList
  Function :  Operates on NCBI's taxonomy tree.  Retrieves all taxons situated directly below
              a list of taxons in the taxonomy tree.
  Usage    :  $seqhound->SHoundGetTaxChildNodesList($taxidlist);
  Returns  :  SeqHoundTextOb
  	      value field: list of taxonomy identifiers or undef if failure 
  Args     :  comma delimited list of taxonomy identifiers
  Example  :  $test = $seqhound->SHoundGetTaxChildNodesList("10665,10066");
              @reply = $test->getValues();
  Note     :  The order in the lists is not maintained

=cut

sub SHoundGetTaxChildNodesList
{
   my $self = shift;
   my (@taxIdList) = @_;
   my @copyArr = @taxIdList;
   my (@tempArr, $sh);
   $sh = new Bio::SeqHound::SeqHoundTextOb(\@taxIdList);
   while($#copyArr != -1){
     @tempArr = Bio::SeqHound::SLRIMisc::MaxSizeArray(\@copyArr);
     my $Arrstr = Bio::SeqHound::SLRIMisc::Array2String(\@tempArr);
     my $arguments = "ptaxid\=$Arrstr";
     my $fulladdress = $self->_HTTPFunctionCall("SeqHoundGetTaxChildNodesList", $arguments);
     my $content = get($fulladdress);
     my $str = $self->_ProcessContent($content, "SHoundGetTaxChildNodesList");
     $sh->mergeValues($str);
   } 
   return $sh;
}

=head2 SHoundGetAllTaxProgeny

  Title    :  SHoundGetAllTaxProgeny
  Function :  Operates on NCBI's taxonomy tree.  It retrieves all taxons (the whole subtree)
              situated below a given taxon in the taxonomy tree.
  Usage    :  $seqhound->SHoundGetAllTaxProgeny($taxid);
  Returns  :  SeqHoundTextOb
  	      value field: list of taxonomy identifiers or undef if failure
  Args     :  taxonomy identifier
  Example  :  $test = $seqhound->SHoundGetAllTaxProgeny(10665);
              @reply = $test->getValues();

=cut

sub SHoundGetAllTaxProgeny
{
   my $self = shift;
   my ($taxId) = @_;
   my @arr = ($taxId);
   my $arguments = "taxid\=$taxId";
   my $fulladdress = $self->_HTTPFunctionCall("SeqHoundGetAllTaxProgeny", $arguments);
   my $content = get($fulladdress);
   my $str = $self->_ProcessContent($content, "SHoundGetAllTaxProgeny");
   my $sh = new Bio::SeqHound::SeqHoundTextOb(\@arr);
   $sh->mergeValues($str);
   return $sh;
}


=head2 SHoundGetTaxParent

  Title    :  SHoundGetTaxParent
  Function :  Operates on NCBI's taxonomy tree.   Retrieves the taxon situated directly above
              a given taxon in the taxonomy tree.
  Usage    :  $seqhound->SHoundGetTaxParent($taxid);
  Returns  :  SeqHoundTextOb
  	      value field: taxonomy ID (e.g. 10663) or undef if failure
  Args     :  taxonomy identifier
  Example  :  $test = $seqhound->SHoundGetTaxParent(10665);
              @reply = $test->getValues();

=cut

sub SHoundGetTaxParent
{
   my $self = shift;
   my ($taxId) = @_;
   my @arr = ($taxId);
   my $arguments = "taxid\=$taxId";
   my $fulladdress = $self->_HTTPFunctionCall("SeqHoundGetTaxParent", $arguments);
   my $content = get($fulladdress);
   my $str = $self->_ProcessContent($content, "SHoundGetTaxParent");
   my $sh = new Bio::SeqHound::SeqHoundTextOb(\@arr);
   $sh->mergeValues($str);
   return $sh;
}

=head2 SHoundGetAllTaxAncestors

  Title    :  SHoundGetAllTaxAncestors
  Function :  Operates on NCBI's taxonomy tree.  It retrieves all taxons (the whole subtree)
              situated above a given taxon in the taxonomy tree.
  Usage    :  $seqhound->SHoundGetAllTaxAncestors($taxid);
  Returns  :  SeqHoundTextOb
  	      value field: list of taxonomy identifiers or undef if failure
  Args     :  taxonomy identifier
  Example  :  $test = $seqhound->SHoundGetAllTaxAncestors(10665);
              @reply = $test->getValues();

=cut

sub SHoundGetAllTaxAncestors
{
   my $self = shift;
   my ($taxId) = @_;
   my @arr = ($taxId);
   my $arguments = "taxid\=$taxId";
   my $fulladdress = $self->_HTTPFunctionCall("SeqHoundGetAllTaxAncestors", $arguments);
   my $content = get($fulladdress);
   my $str = $self->_ProcessContent($content, "SHoundGetAllTaxAncestors");
   my $sh = new Bio::SeqHound::SeqHoundTextOb(\@arr);
   $sh->mergeValues($str);
   return $sh;
}

=head2 SHoundIsTaxMerged

  Title    :  SHoundIsTaxMerged
  Function :  Operates on NCBI's taxonomy tree. Reports if a given taxon has been renamed.
  Usage    :  $seqhound->SHoundIsTaxMerged($gi);
  Returns  :  SeqHoundTextOb
  	      value field: "TRUE" if yes "FALSE" if no
  Args     :  taxonomy identifier
  Example  :  $test = $seqhound->SHoundIsTaxMerged(10665);
              @reply = $test->getValues();

=cut

sub SHoundIsTaxMerged
{
   my $self = shift;
   my ($taxId) = @_;
   my @arr = ($taxId);
   my $arguments = "taxid\=$taxId";
   my $fulladdress = $self->_HTTPFunctionCall("SeqHoundIsTaxMerged", $arguments);
   my $content = get($fulladdress);
   my $str = $self->_ProcessContent($content, "SHoundIsTaxMerged");
   my $sh = new Bio::SeqHound::SeqHoundTextOb(\@arr);
   $sh->mergeValues($str);
   return $sh;
}

=head2 SHoundIsTaxDeleted

  Title    :  SHoundIsTaxDeleted
  Function :  Operates on NCBI's taxonomy tree. Reports if a given taxon has been deleted.
  Usage    :  $seqhound->SHoundIsTaxDeleted($gi);
  Returns  :  SeqHoundTextOb
  	      value field: "TRUE" if yes "FALSE" if no
  Args     :  taxonomy identifier
  Example  :  $test = $seqhound->SHoundIsTaxDeleted(10665);
              @reply = $test->getValues();

=cut

sub SHoundIsTaxDeleted
{
   my $self = shift;
   my ($taxId) = @_;
   my @arr = ($taxId);
   my $arguments = "taxid\=$taxId";
   my $fulladdress = $self->_HTTPFunctionCall("SeqHoundIsTaxDeleted", $arguments);
   my $content = get($fulladdress);
   my $str = $self->_ProcessContent($content, "SHoundIsTaxDeleted");
   my $sh = new Bio::SeqHound::SeqHoundTextOb(\@arr);
   $sh->mergeValues($str);
   return $sh;
}

=head2 SHoundGetTaxNameFromTaxIDByClass

  Title    :  SHoundGetTaxNameFromTaxIDByClass
  Function :  Operates on NCBI's taxonomy tree.   Retrieves taxonomy name in specified style.
  Usage    :  $seqhound->SHoundGetTaxNameFromTaxIDByClass
                           ($taxid, $filter);
  Returns  :  SeqHoundTextOb
  	      value field: taxonomy name (e.g. "human") or undef if failure
  Args     :  taxonomy identifier
              class filter (e.g. "SLRI_taxon_name_name_class_preferred_common_name")
  Example  :  $test = $seqhound->SHoundGetTaxNameFromTaxIDByClass
                           (9606, "SLRI_taxon_name_name_class_preferred_common_name");
              @reply = $test->getValues();
  Note     :  Class filters usable:
              SLRI_taxon_name_name_class_none
              SLRI_taxon_name_name_class_acronym
              SLRI_taxon_name_name_class_anamorph
              SLRI_taxon_name_name_class_blast_name
              SLRI_taxon_name_name_class_common_name
              SLRI_taxon_name_name_class_equivalent_name
              SLRI_taxon_name_name_class_includes
              SLRI_taxon_name_name_class_in_part
              SLRI_taxon_name_name_class_misnomer
              SLRI_taxon_name_name_class_misspelling
              SLRI_taxon_name_name_class_preferred_acronym
              SLRI_taxon_name_name_class_preferred_common_name
              SLRI_taxon_name_name_class_scientific_name
              SLRI_taxon_name_name_class_synonym
              SLRI_taxon_name_name_class_teleomorph
              SLRI_taxon_name_name_class_other

=cut

sub SHoundGetTaxNameFromTaxIDByClass
{
    my $self = shift;
    my ($taxid, $nameClass) = @_;
    my @arr; @arr = ($taxid, $nameClass);
    if ($nameClass eq "SLRI_taxon_name_name_class_none"){
	$nameClass = $self->{SLRI_taxon_name_name_class_none};
    }
    elsif ($nameClass eq "SLRI_taxon_name_name_class_acronym"){
	$nameClass = $self->{SLRI_taxon_name_name_class_acronym};
    }
    elsif ($nameClass eq "SLRI_taxon_name_name_class_anamorph"){
	$nameClass = $self->{SLRI_taxon_name_name_class_anamorph};
    }
    elsif ($nameClass eq "SLRI_taxon_name_name_class_blast_name"){
	$nameClass = $self->{SLRI_taxon_name_name_class_blast_name};
    }
    elsif ($nameClass eq "SLRI_taxon_name_name_class_common_name"){
	$nameClass = $self->{SLRI_taxon_name_name_class_common_name};
    }
    elsif ($nameClass eq "SLRI_taxon_name_name_class_equivalent_name"){
	$nameClass = $self->{SLRI_taxon_name_name_class_equivalent_name};
    }
    elsif ($nameClass eq "SLRI_taxon_name_name_class_includes"){
	$nameClass = $self->{SLRI_taxon_name_name_class_includes};
    }
    elsif ($nameClass eq "SLRI_taxon_name_name_class_in_part"){
	$nameClass = $self->{SLRI_taxon_name_name_class_in_part};
    }
    elsif ($nameClass eq "SLRI_taxon_name_name_class_misnomer"){
	$nameClass = $self->{SLRI_taxon_name_name_class_misnomer};
    }
    elsif ($nameClass eq "SLRI_taxon_name_name_class_misspelling"){
	$nameClass = $self->{SLRI_taxon_name_name_class_misspelling};
    }
    elsif ($nameClass eq "SLRI_taxon_name_name_class_preferred_acronym"){
	$nameClass = $self->{SLRI_taxon_name_name_class_preferred_acronym};
    }
    elsif ($nameClass eq "SLRI_taxon_name_name_class_preferred_common_name"){
	$nameClass = $self->{SLRI_taxon_name_name_class_preferred_common_name};
    }
    elsif ($nameClass eq "SLRI_taxon_name_name_class_scientific_name"){
	$nameClass = $self->{SLRI_taxon_name_name_class_scientific_name};
    }
    elsif ($nameClass eq "SLRI_taxon_name_name_class_synonym"){
	$nameClass = $self->{SLRI_taxon_name_name_class_synonym};
    }
    elsif ($nameClass eq "SLRI_taxon_name_name_class_teleomorph"){
	$nameClass = $self->{SLRI_taxon_name_name_class_teleomorph}; 
    }
    elsif ($nameClass eq "SLRI_taxon_name_name_class_other"){
	$nameClass = $self->{SLRI_taxon_name_name_class_other};
    }
    else {                          #defaults to SLRI_taxon_name_name_class_scientific_name
	$nameClass = $self->{SLRI_taxon_name_name_class_scientific_name};
    }
    my $arguments = "taxid\=$taxid"."\&type\=$nameClass";
    my $fulladdress = $self->_HTTPFunctionCall("SeqHoundGetTaxNameFromTaxIDByClass", $arguments);
    my $content = get($fulladdress);
    my $str = $self->_ProcessContent($content, "SHoundGetTaxNameFromTaxIDByClass");
    my $sh = new Bio::SeqHound::SeqHoundTextOb(\@arr);
    $sh->mergeValues($str);
    return $sh;
}


=head2 SHoundGetTaxNameFromTaxID

  Title    :  SHoundGetTaxNameFromTaxID
  Function :  Operates on NCBI's taxonomy tree.   Retrieves taxonomy name in scientific form.
  Usage    :  $seqhound->SHoundGetTaxNameFromTaxID($taxid);
  Returns  :  SeqHoundTextOb
  	      value field: scientific name (e.g. "Homo sapiens") or undef if failure
  Args     :  taxonomy identifier
  Example  :  $test = $seqhound->SHoundGetTaxNameFromTaxID(9606);
              @reply = $test->getValues();

=cut

sub SHoundGetTaxNameFromTaxID
{
   my $self = shift;
   my ($taxId) = @_;
   my @arr = ($taxId);
   my $arguments = "taxid\=$taxId";
   my $fulladdress = $self->_HTTPFunctionCall("SeqHoundGetTaxNameFromTaxID", $arguments);
   my $content = get($fulladdress);
   my $str = $self->_ProcessContent($content, "SHoundGetTaxNameFromTaxID");
   my $sh =new Bio::SeqHound::SeqHoundTextOb(\@arr);
   $sh->mergeValues($str);
   return $sh;
}


=head2 SHoundGetNewTaxIDFromMergedTaxID

  Title    :  SHoundGetNewTaxIDFromMergedTaxID
  Function :  Operates on NCBI's taxonomy tree.   Retrieves taxonomy identifier.
  Usage    :  $seqhound->SHoundGetNewTaxIDFromMergedTaxID($taxid);
  Returns  :  SeqHoundTextOb
  	      value field: a valid taxonomy identifier if the given taxid is merged or
              undef if failure
  Args     :  taxonomy identifier
  Example  :  $test = $seqhound->GetNewTaxIDFromMergedTaxID(395);
              @reply = $test->getValues();

=cut

sub SHoundGetNewTaxIDFromMergedTaxID
{
   my $self = shift;
   my ($taxId) = @_;
   my @arr = ($taxId);
   my $arguments = "taxid\=$taxId";
   my $fulladdress = $self->_HTTPFunctionCall("SeqHoundGetNewTaxIDFromMergedTaxID", $arguments);
   my $content = get($fulladdress);
   my $str = $self->_ProcessContent($content, "SHoundGetNewTaxIDFromMergedTaxID");
   my $sh =new Bio::SeqHound::SeqHoundTextOb(\@arr);
   $sh->mergeValues($str);
   return $sh;
}

=head2 SHoundGetTaxLineageFromTaxID

  Title    :  SHoundGetTaxLineageFromTaxID
  Function :  Operates on NCBI's taxonomy tree.   Retrieves lineage information in a specified
               style.
  Usage    :  $seqhound->SHoundGetTaxLineageFromTaxID($taxid,$filter);
  Returns  :  SeqHoundTextOb
  	      value field: lineage (e.g."Eukaryota; Metazoa; Chordata; Craniata; Vertebrata; Euteleostomi;
              Mammalia; Eutheria; Primates; Catarrhini; Hominidae; Homo") or undef if failure
  Args     :  taxonomy identifier
              lineage filter (e.g. "SHoundGetTaxLineage_GENBANK")
  Example  :  $test =
              $seqhound->SHoundGetTaxLineageFromTaxID(9606,"SHoundGetTaxLineage_FULL");
              @reply = $test->getValues();
  Note     :  Lineage filters:
              SHoundGetTaxLineage_FULL --   gets full lineage
              SHoundGetTaxLineage_GENBANK -- gets lineage as it appears in GenBank record
              SHoundGetTaxLineage_TAXDBSHORT - gets lineage as it would appear on the taxonomy
               database home page

=cut


sub SHoundGetTaxLineageFromTaxID
{
    my $self = shift;
    my ($taxid, $type) = @_;
    my @arr = ($taxid, $type);
    if ($type eq "SHoundGetTaxLineage_FULL"){
	$type = $self->{SHoundGetTaxLineage_FULL};
    }
    elsif ($type eq "SHoundGetTaxLineage_GENBANK"){
	$type = $self->{SHoundGetTaxLineage_GENBANK};
    }
    elsif ($type eq "SHoundGetTaxLineage_TAXDBSHORT"){
	$type = $self->{SHoundGetTaxLineage_TAXDBSHORT};
    }
    else {                                 #defaults to SHoundGetTaxLineage_FULL
	$type = $self->{SHoundGetTaxLineage_FULL};
    }
    my $arguments = "taxid\=$taxid"."\&type\=$type";
    my $fulladdress = $self->_HTTPFunctionCall("SeqHoundGetTaxLineageFromTaxID", $arguments);
    my $content = get($fulladdress);
    my $str = $self->_ProcessContent($content, "SHoundGetTaxLineageFromTaxID");
    my $sh = new Bio::SeqHound::SeqHoundTextOb(\@arr);
    $sh->mergeValues($str);
    return $sh;
}


=head1 Functional Annotation

=head2 SHoundGOIDFromLLID

  Title    :  SHoundGOIDFromLLID
  Function :  Fetches all Gene Ontology (GO) IDs for a given LocusLink ID (LLID)
  Usage    :  $seqhound->SHoundGOIDFromLLID($llid);
  Returns  :  SeqHoundTextOb
  	      value field: list of GOIDs or undef if failure
  Args     :  Locus Link ID (LLID, e.g. 1)
  Example  :  $test = $seqhound->SHoundGOIDFromLLID(1);
              @reply = $test->getValues();

=cut

sub SHoundGOIDFromLLID
{
   my $self = shift;
   my ($llid) = @_;
   my @arr = ($llid);
   my $arguments = "llid\=$llid";
   my $fulladdress = $self->_HTTPFunctionCall("SeqHoundGOIDFromLLID", $arguments);
   my $content = get($fulladdress);
   my $str = $self->_ProcessContent($content, "SHoundGOIDFromLLID");
   my $sh = new Bio::SeqHound::SeqHoundTextOb(\@arr);
   $sh->mergeValues($str);
   return $sh;
}

=head2 SHoundOMIMFromLLID

  Title    :  SHoundOMIMFromLLID
  Function :  Fetches Online Mendelian Inheritance in Man (OMIM) IDs for a given LocusLink ID
              (LLID)
  Usage    :  $seqhound->SHoundOMIMFromLLID($llid);
  Returns  :  SeqHoundTextOb
  	      value field: OMIM ID or undef if failure
  Args     :  Locus Link ID (LLID)
  Example  :  $test = $seqhound->SHoundOMIMFromLLID(1);
              @reply = $test->getValues();


=cut


sub SHoundOMIMFromLLID
{
   my $self = shift;
   my ($llid) = @_;
   my @arr = ($llid);
   my $arguments = "llid\=$llid";
   my $fulladdress = $self->_HTTPFunctionCall("SeqHoundOMIMFromLLID", $arguments);
   my $content = get($fulladdress);
   my $str = $self->_ProcessContent($content, "SHoundOMIMFromLLID");
   my $sh = new Bio::SeqHound::SeqHoundTextOb(\@arr);
   $sh->mergeValues($str);
   return $sh;
}


=head2 SHoundCDDIDFromLLID

  Title    :  SHoundCDDIDFromLLID
  Function :  Fetches all Conserved Domain Database (CDD) IDs for a given LocusLink ID (LLID)
  Usage    :  $seqhound->SHoundCDDIDFromLLID($llid);
  Returns  :  SeqHoundTextOb
  	      value field: list of CDDIDs or undef if failure
  Args     :  Locus Link ID (LLID)
  Example  :  $test = $seqhound->SHoundCDDIDFromLLID(1);
              @reply = $test->getValues();

=cut

sub SHoundCDDIDFromLLID
{
   my $self = shift;
   my ($llid) = @_;
   my @arr = ($llid);
   my $arguments = "llid\=$llid";
   my $fulladdress = $self->_HTTPFunctionCall("SeqHoundCDDIDFromLLID", $arguments);
   my $content = get($fulladdress);
   my $str = $self->_ProcessContent($content, "SHoundCDDIDFromLLID");
   my $sh = new Bio::SeqHound::SeqHoundTextOb(\@arr);
   $sh->mergeValues($str);
   return $sh;
}

=head2 SHoundLLIDFromGOIDAndECode

  Title    :  SHoundLLIDFromGOIDAndECode
  Function :  Fetches LocusLink IDs (LLID) for a given Gene Ontology ID (GOID) and evidence
              code (ECode)
  Usage    :  $seqhound->SHoundLLIDFromGOIDAndECode($goid, $ecode);
  Returns  :  SeqHoundTextOb
  	      value field: list of LLIDs or undef if failure
  Args     :  GOID
              GO evidence code (e.g. "IEA")
  Example  :  $test = $seqhound->SHoundLLIDFromGOIDAndECode(6810, "IEA");
              @reply = $test->getValues();

=cut

sub SHoundLLIDFromGOIDAndECode
{
   my $self = shift;
   my ($goid, $ecode) = @_;
   my @arr = ($goid, $ecode);
   my $arguments = "goid\=$goid"."&ecode\=$ecode";
   my $fulladdress = $self->_HTTPFunctionCall("SeqHoundLLIDFromGOIDAndECode", $arguments);
   my $content = get($fulladdress);
   my $str = $self->_ProcessContent($content, "SHoundLLIDFromGOIDAndECode");
   my $sh = new Bio::SeqHound::SeqHoundTextOb(\@arr);
   $sh->mergeValues($str);
   return $sh;
}

=head2 SHoundLLIDFromOMIM

  Title    :  SHoundLLIDFromOMIM
  Function :  Fetches LocusLink IDs (LLID) for a given Online Mendelian Inheritance in Man
              (OMIM) ID.
  Usage    :  $seqhound->SHoundLLIDFromOMIM($omimid);
  Returns  :  SeqHoundTextOb
  	      value field: list of LLIDs or undef if failure
  Args     :  OMIM ID
  Example  :  $test = $seqhound->SHoundLLIDFromOMIM(103950);
              @reply = $test->getValues();

=cut

sub SHoundLLIDFromOMIM
{
   my $self = shift;
   my ($omimid) = @_;
   my @arr = ($omimid);
   my $arguments = "omimid\=$omimid";
   my $fulladdress = $self->_HTTPFunctionCall("SeqHoundLLIDFromOMIM", $arguments);
   my $content = get($fulladdress);
   my $str = $self->_ProcessContent($content, "SHoundLLIDFromOMIM");
   my $sh = new Bio::SeqHound::SeqHoundTextOb(\@arr);
   $sh->mergeValues($str);
   return $sh;
}

=head2 SHoundLLIDFromCDDID

  Title    :  SHoundLLIDFromCDDID
  Function :  Fetches LocusLink IDs (LLID) for a given Conserved Domain Database (CDD) ID
  Usage    :  $seqhound->SHoundLLIDFromCDDID($cddid);
  Returns  :  SeqHoundTextOb
  	      value field: list of LLIDs or undef if failure
  Args     :  Conserved Domain ID
  Example  :  $test = $seqhound->SHoundLLIDFromCDDID("smart00408");
              @reply = $test->getValues();

=cut


sub SHoundLLIDFromCDDID
{
   my $self = shift;
   my ($cddid) = @_;
   my @arr = ($cddid);
   my $arguments = "cddid\=$cddid";
   my $fulladdress = $self->_HTTPFunctionCall("SeqHoundLLIDFromCDDID", $arguments);
   my $content = get($fulladdress);
   my $str = $self->_ProcessContent($content, "SHoundLLIDFromCDDID");
   my $sh = new Bio::SeqHound::SeqHoundTextOb(\@arr);
   $sh->mergeValues($str);
   return $sh;
}


=head2 SHoundGiFromGOID

  Title    :  SHoundGiFromGOID
  Function :  Fetches all protein sequences which were assigned a given Gene Ontology (GO)
              function, process or component.
  Usage    :  $seqhound->SHoundGiFromGOID($goid);
  Returns  :  SeqHoundTextOb
  	      value field: list of GIs or undef if failure
  Args     :  GOID
  Example  :  $test = $seqhound->SHoundGiFromGOID(5554);
              @reply = $test->getValues();
  Note     :  Return list has unique value

=cut

sub SHoundGiFromGOID
{
   my $self = shift;
   my ($goid) = @_;
   my @arr = ($goid);
   my $arguments = "goid\=$goid";
   my $fulladdress = $self->_HTTPFunctionCall("SeqHoundGiFromGOID", $arguments);
   my $content = get($fulladdress);
   my $str = $self->_ProcessContent($content, "SHoundGiFromGOID");
   my $sh = new Bio::SeqHound::SeqHoundTextOb(\@arr);
   $sh->mergeValues($str);
   return $sh;
}

=head2 SHoundGiFromGOIDList

  Title    :  SHoundGiFromGOIDList
  Function :  Fetches all protein sequences which were assigned a list of Gene Ontology (GO)
              functions, processes or components.
  Usage    :  $seqhound->SHoundGiFromGOIDList(@goidlist);
  Returns  :  SeqHoundTextOb
  	      value field: list of GIs or undef if failure
  Args     :  comma delimited list of GOIDs
  Example  :  $test = $seqhound->SHoundGiFromGOIDList("5554,4");
              @reply = $test->getValues();
  Note     :  The order in the lists is not maintained
              Return list has unique value

=cut

sub SHoundGiFromGOIDList
{
   my $self = shift;
   my (@goidList) = @_;
   my @copyArr = @goidList;
   my (@tempArr, $sh);
   $sh = new Bio::SeqHound::SeqHoundTextOb(\@goidList);
   while($#copyArr != -1){
     @tempArr = Bio::SeqHound::SLRIMisc::MaxSizeArray(\@copyArr);
     my $Arrstr = Bio::SeqHound::SLRIMisc::Array2String(\@tempArr);
     my $arguments = "vnpgo\=$Arrstr";
     my $fulladdress = $self->_HTTPFunctionCall("SeqHoundGiFromGOIDList", $arguments);
     my $content = get($fulladdress);
     my $str = $self->_ProcessContent($content, "SHoundGiFromGOIDList");
     $sh->mergeValues($str);
   } 
   return $sh;
}

=head2 SHoundGOIDFromGi

  Title    :  SHoundGOIDFromGi
  Function :  Fetches all Gene Ontology (GO) terms assigned to a protein sequence.  An
              individual sequence can be referred to by one or more process, function or
              component terms.
  Usage    :  $seqhound->SHoundGOIDFromGi($gi);
  Returns  :  SeqHoundTextOb
  	      value field: list of GO identifiers or undef
  Args     :  GI identifier
  Example  :  $test = $seqhound->SHoundGOIDFromGi(15149484);
              @reply = $test->getValues();

=cut

sub SHoundGOIDFromGi
{
   my $self = shift;
   my ($gi) = @_;
   my @arr = ($gi);
   my $arguments = "gi\=$gi";
   my $fulladdress = $self->_HTTPFunctionCall("SeqHoundGOIDFromGi", $arguments);
   my $content = get($fulladdress);
   my $str = $self->_ProcessContent($content, "SHoundGOIDFromGi");
   my $sh = new Bio::SeqHound::SeqHoundTextOb(\@arr);
   $sh->mergeValues($str);
   return $sh;
}

=head2 SHoundGOIDFromGiList

  Title    :  SHoundGOIDFromGiList
  Function :  Fetches all Gene Ontology (GO) terms assigned to a list of protein sequences.
              An individual sequence can be referred to by one or more process, function or
              component terms.
  Usage    :  $seqhound->SHoundGOIDFromGiList(@gilist);
  Returns  :  SeqHoundTextOb
  	      value field: list of GO identifiers or undef
  Args     :  comma delimited list of GI identifiers
  Example  :  $test = $seqhound->SHoundGOIDFromGiList("15149484,6912624");
              @reply = $test->getValues();
  Note     :  The order in the lists is not maintained

=cut

sub SHoundGOIDFromGiList
{
   my $self = shift;
   my (@giList) = @_;
   my @copyArr = @giList;
   my (@tempArr, $sh);
   $sh = new Bio::SeqHound::SeqHoundTextOb(\@giList);
   while($#copyArr != -1){
     @tempArr = Bio::SeqHound::SLRIMisc::MaxSizeArray(\@copyArr);
     my $Arrstr = Bio::SeqHound::SLRIMisc::Array2String(\@tempArr);
     my $arguments = "vnpgi\=$Arrstr";
     my $fulladdress = $self->_HTTPFunctionCall("SeqHoundGOIDFromGiList", $arguments);
     my $content = get($fulladdress);
     my $str = $self->_ProcessContent($content, "SHoundGOIDFromGiList");
     $sh->mergeValues($str);
   } 
   return $sh;
}

=head2 SHoundGOIDFromRedundantGi

  Title    :  SHoundGOIDFromRedundantGi
  Function :  Fetches all Gene Ontology (GO) term identifiers assigned to a protein sequence.
               Uses redundancy information for the query GI identifier.  This means that if
              the query fails for the input GI identifier, then the query will be retried for
              all other GI identifiers that belong to the same redundant group.  An individual
               sequence can be referred to by one or more a process, function or component
              terms.
  Usage    :  $seqhound->SHoundGOIDFromRedundantGi($gi);
  Returns  :  SeqHoundTextOb
  	      value field: list of GOIDs or undef if failure
  Args     :  GI identifier (e.g. 21071030)
  Example  :  $test = $seqhound->SHoundGOIDFromRedundantGi(21071030);
              @reply = $test->getValues();


=cut

sub SHoundGOIDFromRedundantGi
{
   my $self = shift;
   my ($gi) = @_;
   my @arr = ($gi);
   my $arguments = "gi\=$gi";
   my $fulladdress = $self->_HTTPFunctionCall("SeqHoundGOIDFromRedundantGi", $arguments);
   my $content = get($fulladdress);
   my $str = $self->_ProcessContent($content, "SHoundGOIDFromRedundantGi");
   my $sh = new Bio::SeqHound::SeqHoundTextOb(\@arr);
   $sh->mergeValues($str);
   return $sh;
}

=head2 SHoundGOIDFromRedundantGiList

  Title    :  SHoundGOIDFromRedundantGiList
  Function :  Fetches all Gene Ontology (GO) term identifiers assigned to a list of protein
              sequences.  Uses redundancy information for the query GI identifier.  This means
               that if the query fails for the input GI identifier, then the query will be
              retried for all other GI identifiers that belong to the same redundant group.
              An individual sequence can be referred to by one or more a process, function or
              component terms.
  Usage    :  $seqhound->SHoundGOIDFromRedundantGiList(@gilist);
  Returns  :  SeqHoundTextOb
  	      value field: list of GOIDs or undef if failure
  Args     :  comma delimited list of GI identifiers
  Example  :  $test = $seqhound->SHoundGOIDFromRedundantGiList("21071030,4586891");
              @reply = $test->getValues();
  Note     :  Return list is sorted and unique
              The order in the lists is not maintained

=cut

sub SHoundGOIDFromRedundantGiList
{
   my $self = shift;
   my (@giList) = @_;
   my @copyArr = @giList;
   my (@tempArr, $sh);
   $sh = new Bio::SeqHound::SeqHoundTextOb(\@giList);
   while($#copyArr != -1){
     @tempArr = Bio::SeqHound::SLRIMisc::MaxSizeArray(\@copyArr);     
     my $Arrstr = Bio::SeqHound::SLRIMisc::Array2String(\@tempArr);
     my $arguments = "vnpgi\=$Arrstr";
     my $fulladdress = $self->_HTTPFunctionCall("SeqHoundGOIDFromRedundantGiList", $arguments);
     my $content = get($fulladdress);
     my $str = $self->_ProcessContent($content, "SHoundGOIDFromRedundantGiList");
     $sh->mergeValues($str);
   } 
   return $sh;
}

=head2 SHoundGiFromOMIM

  Title    :  SHoundGiFromOMIM
  Function :  Fetches all protein GI identifiers associated with an entry in Online Mendelian
              Inheritance in Man (OMIM) database.
  Usage    :  $seqhound->SHoundGiFromOMIM($omimid);
  Returns  :  SeqHoundTextOb
  	      value field: list of protein GI identifiers (e.g. 4501847) or undef if failure
  Args     :  OMIM ID
  Example  :  $test = $seqhound->SHoundGiFromOMIM(137150);
              @reply = $test->getValues();

=cut

sub SHoundGiFromOMIM
{
   my $self = shift;
   my ($omimid) = @_;
   my @arr = ($omimid);
   my $arguments = "omimid\=$omimid";
   my $fulladdress = $self->_HTTPFunctionCall("SeqHoundGiFromOMIM", $arguments);
   my $content = get($fulladdress);
   my $str = $self->_ProcessContent($content, "SHoundGiFromOMIM");
   my $sh = new Bio::SeqHound::SeqHoundTextOb(\@arr);
   $sh->mergeValues($str);
   return $sh;
}

=head2 SHoundGiFromOMIMList

  Title    :  SHoundGiFromOMIMList
  Function :  Fetches all protein GI identifiers associated with a list of entries in Online
              Mendelian Inheritance in Man (OMIM) database.
  Usage    :  $seqhound->SHoundGiFromOMIMList(@omimidlist);
  Returns  :  SeqHoundTextOb
  	      value field: list of protein GI identifiers (e.g.4501847,4557225) or undef if
              failure
  Args     :  comma delimited OMIM IDs
  Example  :  $test = $seqhound->SHoundGiFromOMIMList("137150","103950","108345");
              @reply = $test->getValues();
  Note     :  Return list is sorted and unique
              The order in the lists is not maintained

=cut

sub SHoundGiFromOMIMList
{
   my $self = shift;
   my (@omimList) = @_;
   my @copyArr = @omimList;
   my (@tempArr, $sh);
   $sh = new Bio::SeqHound::SeqHoundTextOb(\@omimList);
   while($#copyArr != -1){
     @tempArr = Bio::SeqHound::SLRIMisc::MaxSizeArray(\@copyArr);
     my $Arrstr = Bio::SeqHound::SLRIMisc::Array2String(\@tempArr);
     my $arguments = "vnpomim\=$Arrstr";
     my $fulladdress = $self->_HTTPFunctionCall("SeqHoundGiFromOMIMList", $arguments);
     my $content = get($fulladdress);
     my $str = $self->_ProcessContent($content, "SHoundGiFromOMIMList");
     $sh->mergeValues($str);
   } 
   return $sh;
}

=head2 SHoundOMIMFromGi

  Title    :  SHoundOMIMFromGi
  Function :  Fetches Online Mendelian Inheritance in Man (OMIM) database identifiers
              associated with a protein GI identifier.
  Usage    :  $seqhound->SHoundOMIMFromGi($gi);
  Returns  :  SeqHoundTextOb
  	      value field: list of OMIM IDs (e.g. 120820) or undef if failure
  Args     :  GI identifier  (must be protein GI identifier)
  Example  :  $test = $seqhound->SHoundOMIMFromGi(4502501);
              @reply = $test->getValues();

=cut

sub SHoundOMIMFromGi
{
   my $self = shift;
   my ($gi) = @_;
   my @arr = ($gi);
   my $arguments = "gi\=$gi";
   my $fulladdress = $self->_HTTPFunctionCall("SeqHoundOMIMFromGi", $arguments);
   my $content = get($fulladdress);
   my $str = $self->_ProcessContent($content, "SHoundOMIMFromGi");
   my $sh = new Bio::SeqHound::SeqHoundTextOb(\@arr);
   $sh->mergeValues($str);
   return $sh;
}


=head2 SHoundOMIMFromGiList

  Title    :  SHoundOMIMFromGiList
  Function :  Fetches Online Mendelian Inheritance in Man (OMIM) database identifiers
              associated with a list of protein GI identifiers.
  Usage    :  $seqhound->SHoundOMIMFromGiList(@gilist);
  Returns  :  SeqHoundTextOb
  	      value field: list of OMIM IDs (e.g. 107280,120820) or undef if failure
  Args     :  comma delimited list of GI identifiers (must be protein GI identifiers)
  Example  :  $test = $seqhound->SHoundOMIMFromGiList("4502501,4501843");
              @reply = $test->getValues();
  Note     :  The order in the lists is not maintained
              Return list is sorted and unique

=cut

sub SHoundOMIMFromGiList
{
   my $self = shift;
   my (@giList) = @_;
   my @copyArr = @giList;
   my (@tempArr, $sh);
   $sh = new Bio::SeqHound::SeqHoundTextOb(\@giList);
   while($#copyArr != -1){
     @tempArr = Bio::SeqHound::SLRIMisc::MaxSizeArray(\@copyArr);
     my $Arrstr = Bio::SeqHound::SLRIMisc::Array2String(\@tempArr);
     my $arguments = "vnpgi\=$Arrstr";
     my $fulladdress = $self->_HTTPFunctionCall("SeqHoundOMIMFromGiList", $arguments);
     my $content = get($fulladdress);
     my $str = $self->_ProcessContent($content, "SHoundOMIMFromGiList");
     $sh->mergeValues($str);
   }
   return $sh;
}

=head2 SHoundGiFromCDDID

  Title    :  SHoundGiFromCDDID
  Function :  Fetches all protein sequences carrying a domain from Conserved Domain Database
              (CDD) as determined by Reverse Position Specific BLAST.  Uses information from
              NCBI's LocusLink database.  If the query CDD identifier is not used by LocusLink
               then no information will be returned.
  Usage    :  $seqhound->SHoundGiFromCDDID($cddid);
  Returns  :  SeqHoundTextOb
  	      value field: list of protein GI identifiers or undef if failure
  Args     :  Conserved Domain Database ID (CDDID) (either Pfam or SMART)
  Example  :  $test = $seqhound->SHoundGiFromCDDID("pfam00202");
              @reply = $test->getValues();

=cut

sub SHoundGiFromCDDID
{
   my $self = shift;
   my ($cddid) = @_;
   my @arr = ($cddid);
   my $arguments = "cddid\=$cddid";
   my $fulladdress = $self->_HTTPFunctionCall("SeqHoundGiFromCDDID", $arguments);
   my $content = get($fulladdress);
   my $str = $self->_ProcessContent($content, "SHoundGiFromCDDID");
   my $sh = new Bio::SeqHound::SeqHoundTextOb(\@arr);
   $sh->mergeValues($str);
   return $sh;
}

=head2 SHoundGiFromCDDIDList

  Title    :  SHoundGiFromCDDIDList
  Function :  Fetches all protein sequences carrying a list of domains from Conserved Domain
              Database (CDD) as determined by Reverse Position Specific BLAST.  Uses
              information from NCBI's LocusLink database.  If the query CDD identifier is not
              used by LocusLink then no information will be returned.
  Usage    :  $seqhound->SHoundGiFromCDDIDList(@cddidlist);
  Returns  :  SeqHoundTextOb
  	      value field: list of protein GI identifiers or undef if failure
  Args     :  comma delimited Conserved Domain Database identifier (CDDID) list
  Example  :  $test = $seqhound->SHoundGiFromCDDIDList("pfam00202,pfam01835,pfam00079");
              @reply = $test->getValues();
  Note     :  Return list is sorted and unique
              The order in the lists is not maintained

=cut

sub SHoundGiFromCDDIDList
{
   my $self = shift;
   my (@giList) = @_;
   my @copyArr = @giList;
   my (@tempArr, $sh);
   $sh = new Bio::SeqHound::SeqHoundTextOb(\@giList);
   while($#copyArr != -1){
     @tempArr = Bio::SeqHound::SLRIMisc::MaxSizeArray(\@copyArr);
     my $Arrstr = Bio::SeqHound::SLRIMisc::Array2String(\@tempArr);
     my $arguments = "vnpcdd\=$Arrstr";
     my $fulladdress = $self->_HTTPFunctionCall("SeqHoundGiFromCDDIDList", $arguments);
     my $content = get($fulladdress);
     my $str = $self->_ProcessContent($content, "SHoundGiFromCDDIDList");
     $sh->mergeValues($str);
   } 
   return $sh;
}


=head2 SHoundCDDIDFromGi

  Title    :  SHoundCDDIDFromGi
  Function :  Fetches domains from Conserved Domain Database (CDD) as determined by Reverse
              Position Specific BLAST on a given sequence.
  Usage    :  $seqhound->SHoundCDDIDFromGi($gi);
  Returns  :  SeqHoundTextOb
  	      value field: list of CDDIDs (e.g.COG2373, pfam00207,pfam01821,pfam01835) or
              undef if failure
  Args     :  GI identifier (must be protein GI identifier)
  Example  :  $test = $seqhound->SHoundCDDIDFromGi(4502501);
              @reply = $test->getValues();
  Note     :  Domain information returned by this function was originally retrieved for
              proteins found in NCBI's LocusLink database.  If the query protein GI identifier
               is not used by the LocusLink record then no information will be returned.  The
              query protein GI identifier could be used to first retrieve a list of redundant
              (identical) sequence GI identifiers using SHoundRedundantGroup and then
               using these returned protein GI identifiers to query the LocusLink data.
              For more conserved domain information (for all proteins found in NCBI's nr
              dataset) see API functions under  RPS BLAST Domains.

=cut

sub SHoundCDDIDFromGi
{
   my $self = shift;
   my ($gi) = @_;
   my @arr = ($gi);
   my $arguments = "gi\=$gi";
   my $fulladdress = $self->_HTTPFunctionCall("SeqHoundCDDIDFromGi", $arguments);
   my $content = get($fulladdress);
   my $str = $self->_ProcessContent($content, "SHoundCDDIDFromGi");
   my $sh = new Bio::SeqHound::SeqHoundTextOb(\@arr);
   $sh->mergeValues($str);
   return $sh;
}

=head2 SHoundCDDIDFromGiList

  Title    :  SHoundCDDIDFromGiList
  Function :  Fetches domains from Conserved Domain Database (CDD) as determined by Reverse
              Position Specific BLAST on a list of sequences.
  Usage    :  $seqhound->SHoundCDDIDFromGiList(@gilist);
  Returns  :  SeqHoundTextOb
  	      value field: list of CDDIDs or undef if failure
  Args     :  comma delimited list of GI identifiers (must be protein GI identifiers)
  Example  :  $test = $seqhound->SHoundCDDIDFromGiList("4502501,4501843");
              @reply = $test->getValues();
  Note     :  The order in the lists is not maintained
              Domain information returned by this function was originally retrieved for
              proteins found in NCBI's LocusLink database.  If the query protein GI identifier
               is not used by the LocusLink record then no information will be returned.  The
              query protein GI identifier could be used to first retrieve a list of redundant
              (identical) sequence GI identifiers using SHoundRedundantGroup and then
               using these returned protein GI identifiers to query the LocusLink data.
              For more conserved domain information (for all proteins found in NCBI's nr
              dataset) see API functions under  RPS BLAST Domains.

=cut

sub SHoundCDDIDFromGiList
{
   my $self = shift;
   my (@giList) = @_;
   my @copyArr = @giList;
   my (@tempArr, $sh);
   $sh = new Bio::SeqHound::SeqHoundTextOb(\@giList);
   while($#copyArr != -1){
     @tempArr = Bio::SeqHound::SLRIMisc::MaxSizeArray(\@copyArr);
     my $Arrstr = Bio::SeqHound::SLRIMisc::Array2String(\@tempArr);
     my $arguments = "vnpgi\=$Arrstr";
     my $fulladdress = $self->_HTTPFunctionCall("SeqHoundCDDIDFromGiList", $arguments);
     my $content = get($fulladdress);
     my $str = $self->_ProcessContent($content, "SHoundCDDIDFromGiList");
     $sh->mergeValues($str);
   }
   return $sh;
}


=head2 SHoundCDDScoreFromGi

  Title    :  SHoundCDDScoreFromGi
  Function :  Fetches a score for Conserved Domain Database (CDD) domain assignment as
              determined by Reverse Position Specific BLAST.
  Usage    :  $seqhound->SHoundCDDScoreFromGi($gi, $cddid);
  Returns  :  SeqHoundTextOb
  	      value field: evalue for corresponding protein GI identifier and CDDID or undef if failure
  Args     :  GI identifier (must be protein GI identifier)
              Conserved Domain Database ID (CDDID)
  Example  :  $test = $seqhound->SHoundCDDScoreFromGi(4501841, "pfam01411");
              @reply = $test->getValues();
  Note     :  Currently, this function does not return meaningful results because there are no
               evalues associated with the records.
              Information returned by this function was originally retrieved for proteins
              found in NCBI's LocusLink database.  If the query protein GI identifier is not
              used by the LocusLink record then no information will be returned.  The query
              protein GI identifier could be used to first retrieve a list of redundant
              (identical) sequence GI identifiers using SHoundRedundantGroup and then
               using these returned protein GI identifiers to query the LocusLink data.
              For more conserved domain information (for all proteins found in NCBI's nr
              dataset) see API functions under RPS BLAST Domains.

=cut

sub SHoundCDDScoreFromGi
{
   my $self = shift;
   my ($gi, $cddid) = @_;
   my @arr = ($gi, $cddid);
   my $arguments = "gi\=$gi"."\&cddid\=$cddid";
   my $fulladdress = $self->_HTTPFunctionCall("SeqHoundCDDScoreFromGi", $arguments);
   my $content = get($fulladdress);
   my $str = $self->_ProcessContent($content, "SHoundCDDScoreFromGi");
   my $sh = new Bio::SeqHound::SeqHoundTextOb(\@arr);
   $sh->mergeValues($str);
   return $sh;
}


=head2 SHoundGiFromLLID

  Title    :  SHoundGiFromLLID
  Function :  Fetches protein GI identifiers associated with NCBI's LocusLink ID (LLID)
  Usage    :  $seqhound->SHoundGiFromLLID($llid);
  Returns  :  SeqHoundTextOb
  	      value field: list of protein GI identifiers or undef if failure
  Args     :  LocusLink ID (LLID)
  Example  :  $test = $seqhound->SHoundGiFromLLID(9);
              @reply = $test->getValues();

=cut

sub SHoundGiFromLLID
{
   my $self = shift;
   my ($llid) = @_;
   my @arr = ($llid);
   my $arguments = "llid\=$llid";
   my $fulladdress = $self->_HTTPFunctionCall("SeqHoundGiFromLLID", $arguments);
   my $content = get($fulladdress);
   my $str = $self->_ProcessContent($content, "SHoundGiFromLLID");
   my $sh = new Bio::SeqHound::SeqHoundTextOb(\@arr);
   $sh->mergeValues($str);
   return $sh;
}

=cut

=head2 SHoundGiFromLLIDList

  Title    :  SHoundGiFromLLIDList
  Function :  Fetches protein GI identifiers associated with a list of NCBI's LocusLink IDs
              (LLID).
  Usage    :  $seqhound->SHoundGiFromLLIDList(@llidlist);
  Returns  :  SeqHoundTextOb
  	      value field: list of protein GI identifiers or undef if failure
  Args     :  comma delimited list of LLIDs
  Example  :  $test = $seqhound->SHoundGiFromLLIDList("2,9,13");
              @reply = $test->getValues();
  Note     :  Return list is sorted and unique
              The order in the lists is not maintained

=cut

sub SHoundGiFromLLIDList
{
   my $self = shift;
   my (@giList) = @_;
   my @copyArr = @giList;
   my (@tempArr, $sh);
   $sh = new Bio::SeqHound::SeqHoundTextOb(\@giList);
   while($#copyArr != -1){
     @tempArr = Bio::SeqHound::SLRIMisc::MaxSizeArray(\@copyArr);
     my $Arrstr = Bio::SeqHound::SLRIMisc::Array2String(\@tempArr);
     my $arguments = "vnpll\=$Arrstr";
     my $fulladdress = $self->_HTTPFunctionCall("SeqHoundGiFromLLIDList", $arguments);
     my $content = get($fulladdress);
     my $str = $self->_ProcessContent($content, "SHoundGiFromLLIDList");
     $sh->mergeValues($str);
   }
   return $sh;
}

=head2 SHoundLLIDFromGi

  Title    :  SHoundLLIDFromGi
  Function :  Fetches LocusLink IDs (LLID) for a given protein sequence.
  Usage    :  $seqhound->SHoundLLIDFromGi($gi);
  Returns  :  SeqHoundTextOb
  	      value field: LLID (e.g. 12) or undef if failure
  Args     :  GI identifier (must be protein GI identifier)
  Example  :  $test = $seqhound->SHoundLLIDFromGi(4501843);
              @reply = $test->getValues();

=cut

sub SHoundLLIDFromGi
{
   my $self = shift;
   my ($gi) = @_;
   my @arr = ($gi);
   my $arguments = "gi\=$gi";
   my $fulladdress = $self->_HTTPFunctionCall("SeqHoundLLIDFromGi", $arguments);
   my $content = get($fulladdress);
   my $str = $self->_ProcessContent($content, "SHoundLLIDFromGi");
   my $sh = new Bio::SeqHound::SeqHoundTextOb(\@arr);
   $sh->mergeValues($str);
   return $sh;
}


=head2 SHoundLLIDFromGiList

  Title    :  SHoundLLIDFromGiList
  Function :  Fetches LocusLink IDs (LLID) associated with a list of protein GI identifiers.
  Usage    :  $seqhound->SHoundLLIDFromGiList(@gilist);
  Returns  :  SeqHoundTextOb
  	      value field: list of LLIDs (e.g. 2, 16) or undef if failure
  Args     :  comma delimited list of GI identifiers (must be protein GI identifiers)
  Example  :  $test = $seqhound->SHoundLLIDFromGiList("4501841","4557225");
              @reply = $test->getValues();
  Note     :  Return list is sorted and unique
              The order in the lists is not maintained

=cut

sub SHoundLLIDFromGiList
{
   my $self =shift;
   my (@giList) = @_;
   my @copyArr = @giList;
   my (@tempArr, $sh);
   $sh = new Bio::SeqHound::SeqHoundTextOb(\@giList);
   while($#copyArr != -1){
     @tempArr = Bio::SeqHound::SLRIMisc::MaxSizeArray(\@copyArr);
     my $Arrstr = Bio::SeqHound::SLRIMisc::Array2String(\@tempArr);
     my $arguments = "vnpgi\=$Arrstr";
     my $fulladdress = $self->_HTTPFunctionCall("SeqHoundLLIDFromGiList", $arguments);
     my $content = get($fulladdress);
     my $str = $self->_ProcessContent($content, "SHoundLLIDFromGiList");
     $sh->mergeValues($str);
   }
   return $sh;
}

=head2 SHoundLocusFromGi

  Title    :  SHoundLocusFromGi
  Function :  Fetches a locus assignment for a protein sequence as found in NCBI's LocusLink
              database.
  Usage    :  $seqhound->SHoundLocusFromGi($gi);
  Returns  :  SeqHoundTextOb
  	      value field: locus (e.g. 14q32.1) or undef if failure
  Args     :  GI identifier (must be protein GI identifier)
  Example  :  $test = $seqhound->SHoundLocusFromGi(51036689);
              @reply = $test->getValues();

=cut

sub SHoundLocusFromGi
{
   my $self = shift;
   my ($gi) = @_;
   my @arr = ($gi);
   my $arguments = "gi\=$gi";
   my $fulladdress = $self->_HTTPFunctionCall("SeqHoundLocusFromGi", $arguments);
   my $content = get($fulladdress);
   my $str = $self->_ProcessContent($content, "SHoundLocusFromGi");
   my $sh = new Bio::SeqHound::SeqHoundTextOb(\@arr);
   $sh->mergeValues($str);
   return $sh;
}

=head2 SHoundGiFromGOIDAndECode

  Title    :  SHoundGiFromGOIDAndECode
  Function :  Gets GI identifiers from Gene Ontology ID (GOID) and evidence code (ECode)
  Usage    :  $seqhound->SHoundGiFromGOIDAndECode($goid, $ecode);
  Returns  :  SeqHoundTextOb
  	      value field: list of GIs or undef if failure
  Args     :  GOID
              GO evidence code  (e.g. ND)
  Example  :  $test = $seqhound->SHoundGiFromGOIDAndECode(5554, ND);
               @reply = $test->getValues();
  Note     :  Return list has unique value

=cut

sub SHoundGiFromGOIDAndECode 
{
   my $self = shift;
   my ($goid, $ecode) = @_;
   my $sh = 0;
   if ($ecode eq "")
   {
      return $sh;
   } 
   my @arr = ($goid, $ecode);
   my $arguments = "goid\=$goid"."\&ecode\=$ecode";
   my $fulladdress = $self->_HTTPFunctionCall("SeqHoundGiFromGOIDAndECode", $arguments);
   my $content = get($fulladdress);
   my $str = $self->_ProcessContent($content, "SHoundGiFromGOIDAndECode");
   $sh = new Bio::SeqHound::SeqHoundTextOb(\@arr);
   $sh->mergeValues($str);
   return $sh;
}

=head2 SHoundGiFromGOIDListAndECode

  Title    :  SHoundGiFromGOIDListAndECode
  Function :  Gets GI identifiers from Gene Ontology ID (GOID) list and evidence code (ECode)
  Usage    :  $seqhound->SHoundGiFromGOIDListAndECode(@goidlist, $ecode);
  Returns  :  SeqHoundTextOb
  	      value field: list of GIs or undef if failure
  Args     :  GOIDList
              GO evidence code
  Example  :  $test = $seqhound->SHoundGiFromGOIDListAndECode("5554,4", ND);
              @reply = $test->getValues();
  Note     :  Return list has unique value
           :  The order in the lists is not maintained

=cut

sub SHoundGiFromGOIDListAndECode
{
   my $sh = 0;
   my $self = shift;
   my ($ecode, @goidList) = @_;
   if ($ecode eq "")
   {
      return $sh;
   } 
   my @tempArr;
   my @copyArr = @goidList;
   #my @arr = (@goidList, $ecode);
   $sh = new Bio::SeqHound::SeqHoundTextOb(\@goidList);
   while($#copyArr != -1){
     @tempArr = Bio::SeqHound::SLRIMisc::MaxSizeArray(\@copyArr);
     my $Arrstr = Bio::SeqHound::SLRIMisc::Array2String(\@tempArr);
     my $arguments = "golist\=$Arrstr"."\&ecode=$ecode";
     my $fulladdress = $self->_HTTPFunctionCall("SeqHoundGiFromGOIDListAndECode", $arguments);
     my $content = get($fulladdress);
     my $str = $self->_ProcessContent($content, "SHoundGiFromGOIDListAndECode");
     $sh->mergeValues($str);
   }
    return $sh;
}


=head2 SHoundGOPMIDFromGiAndGOID

  Title    :  SHoundGOPMIDFromGiAndGOID
  Function :  Gets PMID list from GI identifier and Gene Ontology ID (GOID)
  Usage    :  $seqhound->SHoundGOPMIDFromGiAndGOID($gi, $goid);
  Returns  :  SeqHoundTextOb
  	      value field: list of PMIDs or undef if failure
  Args     :  GI identifier
              GOID
  Example  :  $test = $seqhound->SHoundGOPMIDFromGiAndGOID(21071030, 5554);
              @reply = $test->getValues();
  Note     :  Return list has unique value

=cut

sub SHoundGOPMIDFromGiAndGOID
{
   my $self = shift;
   my ($gi, $goid) = @_;
   my @arr = ($gi, $goid);
   my $arguments = "gi\=$gi"."\&goid\=$goid";
   my $fulladdress = $self->_HTTPFunctionCall("SeqHoundGOPMIDFromGiAndGOID", $arguments);
   my $content = get($fulladdress);
   my $str = $self->_ProcessContent($content, "SHoundGOPMIDFromGiAndGOID");
   my $sh = new Bio::SeqHound::SeqHoundTextOb(\@arr);
   $sh->mergeValues($str);
   return $sh;
}


=head2 SHoundGOECodeFromGiAndGOID

  Title    :  SHoundGOECodeFromGiAndGOID
  Function :  Gets evidence code from GI identifier and Gene Ontology (GO) ID.
  Usage    :  $seqhound->SHoundGOECodeFromGiAndGOID($gi, $goid);
  Returns  :  SeqHoundTextOb
  	      value field: list of evidence codes or undef if failure
  Args     :  GI identifier
              GOID
  Example  :  $test = $seqhound->SHoundGOECodeFromGiAndGOID(21071030, 5554);
              @reply = $test->getValues();
  Note     :  Return list has unique value

=cut

sub SHoundGOECodeFromGiAndGOID
{
   my $self = shift;
   my ($gi, $goid) = @_;
   my @arr = ($gi, $goid);
   my $arguments = "gi\=$gi"."\&goid\=$goid";
   my $fulladdress = $self->_HTTPFunctionCall("SeqHoundGOECodeFromGiAndGOID", $arguments);
   my $content = get($fulladdress);
   my $str = $self->_ProcessContent($content, "SHoundGOECodeFromGiAndGOID");
   my $sh = new Bio::SeqHound::SeqHoundTextOb(\@arr);
   $sh->mergeValues($str);
   return $sh;
}

=head1 GO Hierarchy

=head2 SHoundGODBGetRecordByReference

  Title    :  SHoundGODBGetRecordByReference
  Function :  Gets a list of  Gene Ontology (GO) IDs with a specified GO reference type.
  Usage    :  $seqhound->SHoundGODBGetRecordByReference($goreferencetype);
  Returns  :  SeqHoundTextOb
  	      value field: list of GOIDs
              or undef if failure
  Args     :  reference type for GO module (e.g. EC)
  Example  :  $test = $seqhound->SHoundGODBGetRecordByReference("EC");
              @reply = $test->getValues();
  Note     :  Return list is sorted and unique

=cut

sub SHoundGODBGetRecordByReference
{
   my $self = shift;
   my ($goid) = @_;
   my @arr = ($goid);
   my $arguments = "refType\=$goid";
   my $fulladdress = $self->_HTTPFunctionCall("SeqHoundGODBGetRecordByReference", $arguments);
   my $content = get($fulladdress);
   my $str = $self->_ProcessContent($content, "SHoundGODBGetRecordByReference");
   my $sh = new Bio::SeqHound::SeqHoundTextOb(\@arr);
   $sh->mergeValues($str);
   return $sh;
}

=head2 SHoundGODBGetChildrenOf

  Title    :  SHoundGODBGetChildrenOf
  Function :  Operates on Gene Ontology (GO) vocabulary graphs.  Retrieves a list of
              processes, functions or components situated directly below a given GO identifier
               in the graphs.
  Usage    :  $seqhound->SHoundGODBGetChildrenOf(@goidlist)
  Returns  :  SeqHoundTextOb
  	      value field: list of GOIDs or undef if failure
  Args     :  GOID
  Example  :  $test = $seqhound->SHoundGODBGetChildrenOf(7596);
              @reply = $test->getValues();
  Note     :  Return list is sorted and unique


=cut

sub SHoundGODBGetChildrenOf
{
   my $self = shift;
   my ($goid) = @_;
   my @arr = ($goid);
   my $arguments = "goid\=$goid";
   my $fulladdress = $self->_HTTPFunctionCall("SeqHoundGODBGetChildrenOf", $arguments);
   my $content = get($fulladdress);
   my $str = $self->_ProcessContent($content, "SHoundGODBGetChildrenOf");
   my $sh = new Bio::SeqHound::SeqHoundTextOb(\@arr);
   $sh->mergeValues($str);
   return $sh;
}

=head2 SHoundGODBGetParentOf

  Title    :  SHoundGODBGetParentOf
  Function :  Operates on Gene Ontology (GO) vocabulary graphs.  Retrieves a list of
              processes, functions or components situated directly above a given GO identifier
               in the graphs.
  Usage    :  $seqhound->SHoundGODBGetParentOf($goid);
  Returns  :  SeqHoundTextOb
  	      value field: list of GOIDs or undef if failure
  Args     :  GOID
  Example  :  $test = $seqhound->SHoundGODBGetParentOf(7596);
              @reply = $test->getValues();
  Note     :  due to the logic of the GO hierarchy graphs, it is possible that a GO identifier
               will have more than one parent
              Return list is sorted and unique

=cut

sub SHoundGODBGetParentOf
{
   my $self = shift;
   my ($goid) = @_;
   my @arr = ($goid);
   my $arguments = "goid\=$goid";
   my $fulladdress = $self->_HTTPFunctionCall("SeqHoundGODBGetParentOf", $arguments);
   my $content = get($fulladdress);
   my $str = $self->_ProcessContent($content, "SHoundGODBGetParentOf");
   my $sh = new Bio::SeqHound::SeqHoundTextOb(\@arr);
   $sh->mergeValues($str);
   return $sh;
}

=head2 SHoundGODBGetAllAncestors

  Title    :  SHoundGODBGetAllAncestors
  Function :  This function operates on Gene Ontology (GO) vocabulary graphs.  It retrieves a
              list of all processes, functions or components situated above a given GO
              identifier in the graphs.
  Usage    :  $seqhound->SHoundGODBGetAllAncestors($goid);
  Returns  :  SeqHoundTextOb
  	      value field: list of GOIDs or undef if failure
  Args     :  GOID
  Example  :  $test = $seqhound->SHoundGODBGetAllAncestors(7596);
              @reply = $test->getValues();
  Note     :  Return list is sorted and unique

=cut

sub SHoundGODBGetAllAncestors
{
   my $self = shift;
   my ($goid) = @_;
   my @arr = ($goid);
   my $arguments = "goid\=$goid";
   my $fulladdress = $self->_HTTPFunctionCall("SeqHoundGODBGetAllAncestors", $arguments);
   my $content = get($fulladdress);
   my $str = $self->_ProcessContent($content, "SHoundGODBGetAllAncestorsOf");
   my $sh = new Bio::SeqHound::SeqHoundTextOb(\@arr);
   $sh->mergeValues($str);
   return $sh;
}

=head2 SHoundGODBGetAllChildren

  Title    :  SHoundGODBGetAllChildren
  Function :  Operates on Gene Ontology (GO) vocabulary graphs.  Retrieves a list all of
              processes, functions or components situated below a given GO identifier in the
              graphs.
  Usage    :  $seqhound->SHoundGODBGetAllChildren($goid);
  Returns  :  SeqHoundTextOb
  	      value field: list of GOIDs or undef if failure
  Args     :  GOID
  Example  :  $test = $seqhound->SHoundGODBGetAllChildren(7596);
              @reply = $test->getValues();
  Note     :  Return list is sorted and unique


=cut

sub SHoundGODBGetAllChildren
{
   my $self = shift;
   my ($goid) = @_;
   my @arr = ($goid);
   my $arguments = "goid\=$goid";
   my $fulladdress = $self->_HTTPFunctionCall("SeqHoundGODBGetAllChildren", $arguments);
   my $content = get($fulladdress);
   my $str = $self->_ProcessContent($content, "SHoundGODBGetAllChildrenOf");
   my $sh = new Bio::SeqHound::SeqHoundTextOb(\@arr);
   $sh->mergeValues($str);
   return $sh;
}

=head2 SHoundGODBGetNameByID

  Title    :  SHoundGODBGetNameByID
  Function :  Operates on Gene Ontology (GO) vocabulary graphs.  Retrieves a name of a
              process, a function or a component for a given GO identifier.
  Usage    :  $seqhound->SHoundGODBGetNameByID($goid);
  Returns  :  SeqHoundTextOb
  	      value field: name representing the GOID (e.g. blood coagulation) or undef if failure
  Args     :  GOID
  Example  :  $test = $seqhound->SHoundGODBGetNameByID(7596);
              @reply = $test->getValues();

=cut

sub SHoundGODBGetNameByID
{
   my $self = shift;
   my ($goid) = @_;
   my @arr = ($goid);
   my $arguments = "goid\=$goid";
   my $fulladdress = $self->_HTTPFunctionCall("SeqHoundGODBGetNameByID", $arguments);
   my $content = get($fulladdress);
   my $str = $self->_ProcessContent($content, "SHoundGODBGetNameByID");
   my $sh = new Bio::SeqHound::SeqHoundTextOb(\@arr);
   $sh->mergeValues($str);
   return $sh;
}

=head2 SHoundGODBGetClassification

  Title    :  SHoundGODBGetClassification
  Function :  Operates on Gene Ontology (GO) vocabulary graphs.  Reports if a given GO
              identifier describes a cellular process, a cellular function or a cellular
              component.
  Usage    :  $seqhound->SHoundGODBGetClassification($goid);
  Returns  :  SeqHoundTextOb
  	      value field: GODB classification (e.g. biological_process) or undef if failure
  Args     :  GOID
  Example  :  test = $seqhound->SHoundGODBGetClassification(7596);
              @reply = $test->getValues();
  Note     :  GO_MOL_FUN refers to a function
              GO_BIO_PRO refers to a process
              GO_CELL_COM refers to a component

=cut

sub SHoundGODBGetClassification
{
   my $self = shift;
   my ($goid) = @_;
   my @arr = ($goid);
   my $arguments = "goid\=$goid";
   my $fulladdress = $self->_HTTPFunctionCall("SeqHoundGODBGetClassification", $arguments);
   my $content = get($fulladdress);
   my $str = $self->_ProcessContent($content, "SHoundGODBGetClassification");
   my $sh = new Bio::SeqHound::SeqHoundTextOb(\@arr);
   $sh->mergeValues($str);
   return $sh;
}


=head1 Sequence Neighbours

=head2 SHoundNeighboursFromGi

  Title    :  SHoundNeighboursFromGi
  Function :  Retrieves a list of protein BLAST neighbours.  Uses redundancy information for
              the query protein.  This means that if the query fails for the input GI
              identifier, then the query will be retried for all other GI identifiers that
              belong to the same redundant group.
              The BLAST protein neighbours were calculated using 0.01 maximum E-value cutoff.
  Usage    :  $seqhound->SHoundNeighboursFromGi($gi,$evalue,$restrictreturn);
  Returns  :  SeqHoundTextOb
  	      value field: sequence neighbours as a comma delimited list of Gi identifiers or undef if
              failure
  Args     :  protein GI identifier
              evalue cutoff  (e.g. 0.5)
              "TRUE" to restrict return list to 100, otherwise set limit "FALSE"
  Example  :  $test = $seqhound->SHoundNeighboursFromGi(4557225,0.5,"TRUE");
              @reply = $test->getValues();
  Note     :  The return list is not ordered

=cut

sub SHoundNeighboursFromGi
{
   my $self = shift;
   my ($gi, $evalue, $bLimit) = @_;
   my @arr = ($gi, $evalue, $bLimit);
   my $arguments = "gi\=$gi"."\&evalue\=$evalue"."\&bLimit\=$bLimit";
   my $fulladdress = $self->_HTTPFunctionCall("SeqHoundNeighboursFromGiPL", $arguments);
   my $content = get($fulladdress);
   my $str = $self->_ProcessContent($content, "SHoundNeighboursFromGi");
   my $sh = new Bio::SeqHound::SeqHoundTextOb(\@arr);
   $sh->mergeValues($str);
   return $sh;
}

=head2 SHoundNeighboursFromGiList

  Title    :  SHoundNeighboursFromGiList
  Function :  Retrieves a list of protein BLAST neighbours using a list of proteins as a
              query.  Uses redundancy information for the query proteins.  This means that if
              the query fails for the input GI identifier, then the query will be retried for
              all other GI identifiers that belong to the same redundant group.
              The BLAST protein neighbours were calculated using 0.01 maximum E-value cutoff.
  Usage    :  $seqhound->SHoundNeighboursFromGiList(@gilist,$evalue);
  Returns  :  SeqHoundTextOb
  	      value field: list of GIs or undef if failure
  Args     :  comma delimited list of protein GI identifiers
              evalue cutoff  (e.g. 0.5)
  Example  :  $test = $seqhound->SHoundNeighboursFromGiList("4557225,4501843",0.5);
              @reply = $test->getValues();
  Note     :  The return list is not ordered

=cut

sub SHoundNeighboursFromGiList
{
   my $self = shift;
   my ($evalue, @giList) = @_;
   my @copyArr = @giList;
   my (@tempArr, $sh);
   $sh = new Bio::SeqHound::SeqHoundTextOb(\@giList);
   while($#copyArr != -1){
     @tempArr = Bio::SeqHound::SLRIMisc::MaxSizeArray(\@copyArr);
     my $Arrstr = Bio::SeqHound::SLRIMisc::Array2String(\@tempArr);
     my $arguments = "gilist\=$Arrstr"."\&evalue\=$evalue";
     my $fulladdress = $self->_HTTPFunctionCall("SeqHoundNeighboursFromGiListPL", $arguments);
     my $content = get($fulladdress);
     my $str = $self->_ProcessContent($content, "SHoundNeighboursFromGiList");
     $sh->mergeValues($str);
   }
   return $sh;
}

=head2 SHoundNeighboursFromTaxID

  Title    :  SHoundNeighboursFromTaxID
  Function :  Retrieves a list of protein BLAST neighbours using a list of proteins belonging
              to a complete genome as a query.  Uses redundancy information for the query
              proteins.
              The BLAST protein neighbours were calculated using 0.01 maximum E-value cutoff.
  Usage    :  $seqhound->SHoundNeighboursFromTaxID($taxid, $evalue);
  Returns  :  SeqHoundTextOb
  	      value field: list of GIs or undef if failure
  Args     :  taxonomy identifier
              evalue cutoff
  Example  :  $test = $seqhound->SHoundNeighboursFromTaxID(9606, 1.0);
              @reply = $test->getValues();

=cut

sub SHoundNeighboursFromTaxID
{
   my $self = shift;
   my ($taxid, $evalue) = @_;
   my @arr = ($taxid, $evalue);
   my $arguments = "taxid\=$taxid"."\&evalue\=$evalue";
   my $fulladdress = $self->_HTTPFunctionCall("SeqHoundNeighboursFromTaxIDPL", $arguments);
   my $content = get($fulladdress);
   my $str = $self->_ProcessContent($content, "SHoundNeighboursFromTaxID");
   my $sh = new Bio::SeqHound::SeqHoundTextOb(\@arr);
   $sh->mergeValues($str);
   return $sh;
}

=head2 SHound3DNeighboursFromGi

  Title    :  SHound3DNeighboursFromGi
  Function :  Retrieves a list of protein BLAST neighbours possessing 3-D structure.  Uses
              redundancy information for the query protein.  This means that if the query
              fails for the input GI identifier, then the query will be retried for all other
              GI identifiers that belong to the same redundant group.
               The BLAST protein neighbours were calculated using 0.01 maximum E-value cutoff.
  Usage    :  $seqhound->SHound3DNeighoursFromGi($gi, $evalue);
  Returns  :  SeqHoundTextOb
  	      value field: list of GI identifiers or undef if failure
  Args     :  protein GI identifier
              evalue cutoff  (e.g. 1.0)
  Example  :  $test = $seqhound->SHound3DNeighoursFromGi(4557225, 1.0);
              @reply = $test->getValues();
  Note     :  The return list is not ordered

=cut

sub SHound3DNeighboursFromGi
{
   my $self = shift;
   my ($gi, $evalue) = @_;
   my @arr = ($gi, $evalue);
   my $arguments = "gi\=$gi"."\&evalue\=$evalue";
   my $fulladdress = $self->_HTTPFunctionCall("SeqHound3DNeighboursFromGiPL", $arguments);
   my $content = get($fulladdress);
   my $str = $self->_ProcessContent($content, "SHound3DNeighboursFromGi");
   my $sh = new Bio::SeqHound::SeqHoundTextOb(\@arr);
   $sh->mergeValues($str);
   return $sh;
}

=head2 SHound3DNeighboursFromGiList

  Title    :  SHound3DNeighboursFromGiList
  Function :  Retrieves a list of protein BLAST neighbours possessing 3-D structure using a
              list of proteins as a query.  Uses redundancy information for the query protein.
               This means that if the query fails for the input GI identifier, then the query
              will be retried for all other GI identifiers that belong to the same redundant
              group.
              The BLAST protein neighbours were calculated using 0.01 maximum E-value cutoff.
  Usage    :  $seqhound->SHound3DNeighoursFromGiList(@gilist, $evalue);
  Returns  :  SeqHoundTextOb
  	      value field: list of GI identifiers or undef if failure
  Args     :  comma delimited list of protein GI identifiers
              evalue cutoff (e.g. 1.0)
  Example  :  $test = $seqhound->SHound3DNeighoursFromGiList("4557225, 6009729", 1.0);
              @reply = $test->getValues();
  Note     :  The order in the lists is not maintained.

=cut

sub SHound3DNeighboursFromGiList
{
   my $self = shift;
   my ($evalue, @giList) = @_;
   my @copyArr = @giList;
   my (@tempArr, $sh);
   $sh = new Bio::SeqHound::SeqHoundTextOb(\@giList);
   while($#copyArr != -1){
     @tempArr = Bio::SeqHound::SLRIMisc::MaxSizeArray(\@copyArr);
     my $Arrstr = Bio::SeqHound::SLRIMisc::Array2String(\@tempArr);
     my $arguments = "gilist\=$Arrstr"."\&evalue\=$evalue";
     my $fulladdress = $self->_HTTPFunctionCall("SeqHound3DNeighboursFromGiListPL", $arguments);
     my $content = get($fulladdress);
     my $str = $self->_ProcessContent($content, "SHound3DNeighboursFromGiList");
     $sh->mergeValues($str);
   }
   return $sh;
}

=head2 SHound3DNeighboursFromTaxID

  Title    :  SHound3DNeighboursFromTaxID
  Function :  Retrieves a list of protein BLAST neighbours possessing 3-D structure using a
              list of proteins belonging to a complete genome as a query. Uses redundancy
              information for the query protein.
              The BLAST protein neighbours were calculated using 0.01 maximum E-value cutoff.
  Usage    :  $seqhound->SHound3DNeighboursFromTaxID($taxid, $evalue);
  Returns  :  SeqHoundTextOb
  	      value field: list of GIs or undef if failure
  Args     :  taxonomy identifier
              evalue cutoff (e.g. 1.0)
  Example  :  $test = $seqhound->SHound3DNeighboursFromTaxID(9606, 1.0);
              @reply = $test->getValues();

=cut

sub SHound3DNeighboursFromTaxID
{
   my $self = shift;
   my ($taxid, $evalue) = @_;
   my @arr = ($taxid, $evalue);
   my $arguments = "taxid\=$taxid"."\&evalue\=$evalue";
   my $fulladdress = $self->_HTTPFunctionCall("SeqHound3DNeighboursFromTaxIDPL", $arguments);
   my $content = get($fulladdress);
   my $str = $self->_ProcessContent($content, "SHound3DNeighboursFromTaxID");
   my $sh = new Bio::SeqHound::SeqHoundTextOb(\@arr);
   $sh->mergeValues($str);
   return $sh;
}

=head1 Complete Genome Analysis

=head2 SHound3DListFromTaxID

  Title    :  SHound3DListFromTaxID
  Function :  Returns a list of proteins with 3-D structure for a complete organism.  The
              function uses redundancy information but no neighbour information
  Usage    :  $seqhound->SHound3DListFromTaxID($taxid);
  Returns  :  SeqHoundTextOb
  	      value field: list of protein GIs or undef if failure
  Args     :  taxonomy identifier
  Example  :  $test = $seqhound->SHound3DListFromTaxID(9913);
              @reply = $test->getValues();
  Note     :  Return list is not sorted

=cut

sub SHound3DListFromTaxID
{
   my $self =shift;
   my ($taxid) = @_;
   my @arr = ($taxid);
   my $arguments = "taxid\=$taxid";
   my $fulladdress = $self->_HTTPFunctionCall("SeqHound3DListFromTaxIDPL", $arguments);
   my $content = get($fulladdress);
   my $str = $self->_ProcessContent($content, "SHound3DListFromTaxID");
   my $sh = new Bio::SeqHound::SeqHoundTextOb(\@arr);
   $sh->mergeValues($str);
   return $sh;
}

# More GODB Stuff

#This function is disabled since the local API function has a wrong logic and is also disabled.
#sub SHoundGODBGetHierarchyLevel
#{
#   my $self = shift;
#   my ($goid) = @_;
#   my @arr = ($goid);
#   my $arguments = "goid\=$goid";
#   my $fulladdress = $self->_HTTPFunctionCall("SeqHoundAllGODBGetHierarchyLevel", $arguments);
#   my $content = get($fulladdress);
#   my $str = $self->_ProcessContent($content, "SHoundAllGODBGetHierarchyLevel");
#   my $sh = new Bio::SeqHound::SeqHoundTextOb(\@arr);
#   $sh->mergeValues($str);
#   return $sh;
#}


=head2 SHoundAllGenomesWithScientificNames

  Title    :  SHoundAllGenomesWithScientificNames
  Function :  Retrieves all complete genomes from SeqHound and their scientific names.
  Usage    :  $seqhound->SHoundAllGenomesWithScientificNames();
  Returns  :  a list of taxonomy identifiers and their associated scientific names 
  Args     :  none
  Example  :  @list = $seqhound->SHoundAllGenomesWithScientificNames();
              print join("\n", @list);

=cut

sub SHoundAllGenomesWithScientificNames
{
   my $self =shift;
   my @arr;
   my $fulladdress = $self->_HTTPFunctionCall("SeqHoundAllGenomesWithScientificNamesPL", "");
   my $content = get($fulladdress);
   my $str = $self->_ProcessContent($content, "SHoundAllGenomesWithScientificNamesPL");
   my @list = split(/\n/, $str);
   return @list;
}


=head2 SHoundAllGenomesWithNumProteins

  Title    :  SHoundAllGenomesWithNumProteins
  Function :  Retrieves summary for complete genomes and their protein count.
  Usage    :  $seqhound->SHoundAllGenomesWithNumProteins();
  Returns  :  a list of taxonomy identifiers and their associated number of proteins 
  Args     :  none
  Example  :  @list = $seqhound->SHoundAllGenomesWithNumProteins();
              print join("\n", @list);

=cut

sub SHoundAllGenomesWithNumProteins
{
   my $self =shift;
   my @arr;
   my $fulladdress = $self->_HTTPFunctionCall("SeqHoundAllGenomesWithNumProteinsPL", "");
   my $content = get($fulladdress);
   my $str = $self->_ProcessContent($content, "SHoundAllGenomesWithNumProteinsPL");
   my @list = split(/\n/, $str);
   return @list;
}

=head1 RPS Blast Domains

=head2 SHoundGetGisByNumberOfDomains

  Title    :  SHoundGetGisByNumberOfDomains
  Function :  Fetches all sequences with a specified number of domains identified by Reverse
              Position Specific BLAST using Conserved Domain Database set of domains.
  Usage    :  $seqhound->SHoundGetGisByNumberOfDomains($domainno);
  Returns  :  SeqHoundTextOb
  	      value field: list of GIs or undef if failure
  Args     :  number of domains
  Example  :  $test = $seqhound->SHoundGetGisByNumberOfDomains(4);
              @reply = $test->getValues();

=cut


sub SHoundGetGisByNumberOfDomains
{
   my $self = shift;
   my ($numdom) = @_;
   my @arr = ($numdom);
   my $arguments = "numdom\=$numdom";
   my $fulladdress = $self->_HTTPFunctionCall("SeqHoundGetGisByNumberOfDomains", $arguments);
   my $content = get($fulladdress);
   my $str = $self->_ProcessContent($content, "SHoundGetGisByNumberOfDomains");
   my $sh = new Bio::SeqHound::SeqHoundTextOb(\@arr);
   $sh->mergeValues($str);
   return $sh;
}

=head2 SHoundGetDomainIdFromLabel

  Title    :  SHoundGetDomainIdFromLabel
  Function :  Returns one or more Conserved Domain Database (CDD) identifiers given a short
              non-unique label. The  labels are preferred amongst the scientific community.
  Usage    :  $seqhound->SHoundGetDomainIdFromLabel($label);
  Returns  :  SeqHoundTextOb
  	      value field: list of CDDIDs (e.g. smart00038,pfam01410) or undef if failure
  Args     :  short label (e.g. COLFI)
  Example  :  $test = $seqhound->SHoundGetDomainIdFromLabel("COLFI");
              @reply = $test->getValues();

=cut

sub SHoundGetDomainIdFromLabel
{
   my $self = shift;
   my ($label) = @_;
   my @arr = ($label);
   my $arguments = "label\=$label";
   my $fulladdress = $self->_HTTPFunctionCall("SeqHoundGetDomainIdFromLabel", $arguments);
   my $content = get($fulladdress);
   my $str = $self->_ProcessContent($content, "SHoundGetDomainIdFromLabel");
   my $sh = new Bio::SeqHound::SeqHoundTextOb(\@arr);
   $sh->mergeValues($str);
   return $sh;
}

=head2 SHoundGetGisByDomainId

  Title    :  SHoundGetGisByDomainId
  Function :  Fetches protein sequences on which a domain from the Conserved Domain Database
              (CDD) was identified by Reverse Position Specific BLAST.
  Usage    :  $seqhound->SHoundGetGisByDomainId($cddid);
  Returns  :  SeqHoundTextOb
  	      value field: list of GIs or undef if failure
  Args     :  CDDID
  Example  :  $test = $seqhound->SHoundGetGisByDomainId("pfam01410");
              @reply = $test->getValues();

=cut

sub SHoundGetGisByDomainId
{
   my $self = shift;
   my ($id) = @_;
   my @arr = ($id);
   my $arguments = "domain_id\=$id";
   my $fulladdress = $self->_HTTPFunctionCall("SeqHoundGetGisByDomainId", $arguments);
   my $content = get($fulladdress);
   my $str = $self->_ProcessContent($content, "SHoundGetGisByDomainId");
   my $sh = new Bio::SeqHound::SeqHoundTextOb(\@arr);
   $sh->mergeValues($str);
   return $sh;
}

=head2 SHoundGetDomainLabelFromDomainId

  Title    :  SHoundGetDomainLabelFromDomainId
  Function :  Returns a short non-unique label given a domain for a Conserved Domain Database
              (CDD). The  labels are preferred amongst the scientific community.
  Usage    :  $seqhound->SHoundGetDomainLabelFromDomainId($cddid);
  Returns  :  SeqHoundTextOb
  	      value field: short label (e.g. COLFI ) or undef if failure
  Args     :  CDDID
  Example  :  $test = $seqhound->SHoundGetDomainLabelFromDomainId("pfam01410");
              @reply = $test->getValues();

=cut

sub SHoundGetDomainLabelFromDomainId
{
   my $self = shift;
   my ($accession) = @_;
   my @arr = ($accession);
   my $arguments = "accession\=$accession";
   my $fulladdress = $self->_HTTPFunctionCall("SeqHoundGetDomainLabelFromDomainId", $arguments);
   my $content = get($fulladdress);
   my $str = $self->_ProcessContent($content, "SHoundGetDomainLabelFromDomainId");
   my $sh = new Bio::SeqHound::SeqHoundTextOb(\@arr);
   $sh->mergeValues($str);
   return $sh;
}

=head2 SHoundGetGisByDomainIdAndEvalue

  Title    :  SHoundGetGisByDomainIdAndEvalue
  Function :  Fetches protein sequences on which a domain from the Conserved Domain Database
              (CDD) was identified by Reverse Position Specific BLAST. The function uses an
              E-value cutoff for reporting results.
  Usage    :  $seqhound->SHoundGetGisByDomainIdAndEvalue($cddid,$evalue);
  Returns  :  SeqHoundTextOb
  	      value field: list of GIs or undef if failure
  Args     :  CDDID
              evalue cutoff (e.g. 0.5)
  Example  :  $test = $seqhound->SHoundGetGisByDomainIdAndEvalue("pfam01410",0.5);
              @reply = $test->getValues();

=cut

sub SHoundGetGisByDomainIdAndEvalue
{
   my $self = shift;
   my ($domain, $evalue) = @_;
   my @arr = ($domain, $evalue);
   my $arguments = "domain\=$domain"."\&evalue\=$evalue";
   my $fulladdress = $self->_HTTPFunctionCall("SeqHoundGetGisByDomainIdAndEvalue", $arguments);
   my $content = get($fulladdress);
   my $str = $self->_ProcessContent($content, "SHoundGetGisByDomainIdAndEvalue");
   my $sh = new Bio::SeqHound::SeqHoundTextOb(\@arr);
   $sh->mergeValues($str);
   return $sh;
}

=head2 SHoundGetDomainsFromGi

  Title    :  SHoundGetDomainsFromGi
  Function :  Fetches information about domains from the Conserved Domain Database (CDD) as
              assigned by Reverse Position Specific BLAST to a given protein sequence.  Uses
              redundancy information for the query proteins.  This means that if the query
              fails for the input GI identifier, then the query will be retried for all other
              GI identifiers that belong to the same redundant group.
  Usage    :  $seqhound->SHoundGetDomainsFromGi($gi);
  Returns  :  SeqHoundTextOb
  	      value field: a RPS strucrure
              StRpsdb ::= {
              {
                 gi 4557225 ,
                 dom-id "pfam01835" ,
                 cdd-id 6592 ,
                 from 10 ,
                 len 616 ,
                 score 1953 ,
                 evalue { 0, 10, 0 } ,
                 bitscore { 287872, 10, -5 } ,
                 n-missing 0 ,
                 c-missing 3 ,
                 num-of-dom 2 } ,
                 {
                    gi 4557225 ,
                    dom-id "pfam00207" ,
                    cdd-id 5952 ,
                    from 724 ,
                    len 737 ,
                    score 2361 ,
                    evalue { 0, 10, 0 } ,
                    bitscore { 2960741, 10, -6 } ,
                    n-missing 0 ,
                    c-missing 1 ,
                       num-of-dom 2 } }
              or undef if failure
  Args     :  protein GI identifier
  Example  :  $test = $seqhound->SHoundGetDomainsFromGi(4557225);
              @reply = $test->getValues();
  Note     :  The returning structure contains CDD domain identifier, E-value, position,
              length of the alignment, N and C terminal residues missing when compared to the
              consensus domain and the total number of domains on a protein.

=cut

sub SHoundGetDomainsFromGi
{
   my $self = shift;
   my ($gi) = @_;
   my @arr = ($gi);
   my $arguments = "gi\=$gi";
   my $fulladdress = $self->_HTTPFunctionCall("SeqHoundGetDomainsFromGiPL", $arguments);
   my $content = get($fulladdress);
   my $str = $self->_ProcessContent($content, "SHoundGetDomainsFromGi");
   my $sh = new Bio::SeqHound::SeqHoundTextOb(\@arr);
   $sh->mergeValues($str);
   return $sh;
}

=head2 SHoundGetDomainsFromGiWithEvalue

  Title    :  SHoundGetDomainsFromGiWithEvalue
  Function :  Fetches information about domains from the Conserved Domain Database (CDD) as
              assigned by Reverse Position Specific BLAST to a given protein sequence.  Uses
              redundancy information for the query proteins.  This means that if the query
              fails for the input GI identifier, then the query will be retried for all other
              GI identifiers that belong to the same redundant group.  The function uses an
              E-value cutoff for reporting results.
  Usage    :  $seqhound->SHoundGetDomainsFromGiWithEvalue($gi, $evalue);
  Returns  :  SeqHoundTextOb
  	      value field: RPS strucrure or undef if failure
  Args     :  protein GI identifier
              evalue cutoff  (e.g. 0.5)
  Example  :  $test = $seqhound->SHoundGetDomainsFromGiWithEvalue(6322015, 0.5);
              @reply = $test->getValues();
  Note     :  The returning structure contains CDD domain identifier, E-value, position,
              length of the alignment, N and C terminal residues missing when compared to the
              consensus domain and the total number of domains on a protein

=cut

sub SHoundGetDomainsFromGiWithEvalue
{
   my $self = shift;
   my ($gi, $evalue) = @_;
   my @arr = ($gi, $evalue);
   my $arguments = "gi\=$gi"."\&evalue\=$evalue";
   my $fulladdress = $self->_HTTPFunctionCall("SeqHoundGetDomainsFromGiWithEvaluePL", $arguments);
   my $content = get($fulladdress);
   my $str = $self->_ProcessContent($content, "SHoundGetDomainsFromGiWithEvalue");
   my $sh = new Bio::SeqHound::SeqHoundTextOb(\@arr);
   $sh->mergeValues($str);
   return $sh;
}












sub _DecodeChromFlag  
{
    my $self = shift;
    my $chromflag = shift;
    if($chromflag eq "CHROM_PHAGE"){
        return $self->{CHROM_PHAGE};
    }elsif($chromflag eq "CHROM_NR"){
        return $self->{CHROM_NR};
    }elsif($chromflag eq "CHROM_ECE"){
        return $self->{CHROM_ECE};
    }elsif($chromflag eq "CHROM_PLMD"){
        return $self->{CHROM_PLMD};
    }elsif($chromflag eq "CHROM_CHLO"){
 	return $self->{CHROM_CHLO};
    }elsif($chromflag eq "CHROM_MITO"){
	return $self->{CHROM_MITO};
    }elsif($chromflag eq "CHROM_CHROM"){
	return $self->{CHROM_CHROM};
    }elsif($chromflag eq "CHROM_ALL"){
 	return $self->{CHROM_ALL};
    }else{
 	return $self->{CHROM_ALL};
    }
}

sub _DecodeBitMask
{
    my $self = shift;
    my $str = shift;
    my $data = '';
    if($str & $self->{AM_ION}){
	$data = "AM_ION";
    }
    if($str & $self->{AM_RNA}){
        if($data ne ''){ $data .= ','; }
	$data .= "AM_RNA";
    }
    if($str & $self->{AM_WAT}){
        if($data ne ''){ $data .= ','; }
        $data .= "AM_WAT";
    }
    if($str & $self->{AM_SOL}){
 	if($data ne ''){ $data .= ','; }
        $data .= "AM_SOL";
    }
    if($str & $self->{AM_HET}){
  	if($data ne ''){ $data .= ','; }
        $data .= "AM_HET";
    }
    if($str & $self->{AM_DNA}){
	if($data ne ''){ $data .= ','; }
        $data .= "AM_DNA";
    }
    if($str & $self->{AM_PROT}){
	if($data ne ''){ $data .= ','; }
 	$data .= "AM_PROT";
    }
    if($str & $self->{AM_POLY}){
 	if($data ne ''){ $data .= ','; }
  	$data .= "AM_POLY";
    }
    return $data;
}

sub _HTTPFunctionCall
{
   my $self = shift;
   
   (my $function, my $args) = @_;

   my $functioncall = "NULL";

   if($args eq "NULL"){
      
       $functioncall = "\?fnct\=".$function;
   }
   else
   {
       $functioncall = "\?fnct\=".$function."\&".$args;
   }
   
   my $address = $self->{server}.$self->{path}.$functioncall;

   return $address;
}


sub _ProcessContent
{
   my $self = shift;
   
   (my $lcontent, my $fname) = @_;
   
   my $result;
   
   open (LOG, ">>shoundlog");
   
   my $now =strftime("%a %b %e %H:%M:%S %Y", localtime);

   if ($lcontent eq "")
   {
     print LOG "$now	$fname. No reply.\n";
     $result = undef;
   }
   else
   {
      chomp $lcontent;
      
      my @lines = split(/\n/, $lcontent, 2);

      if($lines[0] =~ /ERROR/)
      {
        print LOG "$now	$fname error. $lines[0]\n";
        $result = undef;
      }
      elsif ($lines[0] =~ /NULL/)
      {
        print LOG "$now	$fname Value not found in the database.\n";
        
        $result = undef;
      }
      elsif ($lines[1] =~ /(null)/)
      {
        print LOG "$now	$fname Value not found in the database.\n";
        
        $result = undef;
      }
      elsif($lines[1] eq "")
      {
	 print LOG "$now	$fname Value not found in the database.\n";
         
         $result = undef;
	 
    }else
    {
         if ($lines[1] =~ /^-1$/)
         {
           print LOG "$now	$fname Value not found in the database.\n";
           
           $result = undef;
         }
         elsif ($lines[1]  =~ /^0$/)
         {
           print LOG "$now	$fname failed.\n";
           
           $result = undef;
         }
         else
         {
           
           $result = $lines[1];
         }
      }
   }
   close (LOG);
   
   return $result;
}



############################
#list function handling
############################
sub _ProcessContentKeyedList
{
   my $self = shift;
   
   my ($lcontent, $fname, %hash) = @_;

   open (LOG, ">>shoundlog");
   
   my $now =strftime("%a %b %e %H:%M:%S %Y", localtime);

   if ($lcontent eq "")
   {
     print LOG "$now	$fname No reply.\n";
     
     close(LOG);
     
     return %hash;
  
   }
   
   if ($lcontent =~ /ERROR/)
   {
     print LOG "$now	$fname fails.\n";
     
     close(LOG);
     
     return %hash;
     
   }
   else
   {
      
      chomp $lcontent;
      
      my @lines = split(/\n/, $lcontent, 2);

      if( ($lines[0] =~ /SEQHOUND_OK/) && !($lines[1] =~ /^-1$/) && !($lines[1]  =~ /^0$/) )
      {
           my $return = $lines[1];
	   
	   %hash = $self->Bio::SeqHound::SLRIMisc::Str2HashTable($return, %hash);
      }
      else
      {
         print LOG "$now	$fname fails.\n";
	 
	 close(LOG);
	
      }
   }
    
    return %hash;

}




sub DESTROY{
   my $self = shift;
}

1;
__END__

#
# $Log: SeqHound.pm,v $
# Revision 1.33  2005/04/22 15:23:26  ryao
# Updated for release 4.0: _ProcessContent() changed the return value to undef if functions calls fail as SeqHound_ERROR or SeqHound_NULL; LWP::UserAgent replaces LWP::Simple to have a better control of HTTP::Request and HTTP::Response; Added five new functions to adapt new comgen module; Modified pod documentation for each function to correct the return value. These changes are before integrating SOAP service to bioperl remote
#
# Revision 1.32  2005/03/11 16:57:07  ryao
# Added SHoundGetNewTaxIDFromMergedTaxID bug [3474]; updated comments since PubMed ID is available now
#
# Revision 1.31  2004/09/20 20:02:14  rfarrall
# minor change to documentation [bug 3039]
#
# Revision 1.30  2004/09/01 15:53:00  ryao
# updates
#
# Revision 1.29  2004/07/22 14:44:37  rfarrall
# added note to SHoundAllGenomesWithScientificNames and
# SHoundAllGenomesWithNumProteins as seqrem will not be
# updated in time for release.
# [bug 2785]
#
# Revision 1.28  2004/07/20 15:54:31  rfarrall
# minor changes to documentation
# [bug 2785]
#
# Revision 1.27  2004/07/16 20:47:31  ryao
# add code to show time stamp in log file. bug [2353] related.
#
# Revision 1.26  2004/07/16 14:30:25  rfarrall
# updated docs for
# SHoundAccFromGi
# SHoundAccFromGiList
# [bug 2490]
#
# SHoundFindAcc
# SHoundFindAccList
# [bug 2567]
#
# Revision 1.25  2004/07/15 18:40:00  ryao
# Modified SHoundAllGenomesWithScientificNames and SHoundAllGenomesWithNumProteins: added pod documentation, changed return value to a list.
#
# Revision 1.24  2004/07/14 21:38:33  ryao
# Disabled SHoundAllGODBGetHierarchyLevel since it is also disabled in local API due to logic error; Added SHoundAllGenomAllGenomesWithNumProteins and SHoundAllGenomesWithScientificNames.
#
# Revision 1.23  2004/07/14 19:32:44  rfarrall
# added POD docs for SHoundGetSeqFromGenBank
# [bug 2533]
#
# Revision 1.22  2004/07/13 19:23:12  rfarrall
# changes to docs
# removed reference to chromff file for
# SHoundChromosomeFromGenome
# SHoundChromosomeFromGenomeList
# SHoundDNAFromChromosome
# SHoundDNAFromChromosomeList
# SHoundProteinsFromChromosome
# SHoundProteinsFromChromosomeList
#
# [bug 2533]
#
# Revision 1.21  2004/07/13 16:05:51  rfarrall
# minor changes to docs for SHoundGetGenBankff, SHoundGetGenBankffList, SHoundGetPDB3D, SHoundGetXML3D,  SHoundGetXMLBioseq, SHoundGetXMLSeqEntry
# [bug 2533]
#
# Revision 1.20  2004/07/12 20:52:15  rfarrall
# inserted POD documentation for all API functions except SHoundGetSeqFromGenBank
# moved some functions within the file to be included in the correct function section
# updated existing documentation (website, contact etc.)
# checked with podchecker and test.pl
# [bug 2533]
#
# Revision 1.19  2004/07/09 21:31:00  ryao
# minor fix
#
# Revision 1.18  2004/07/06 20:42:07  ryao
# modified parse_FastaHash() to store null object
#
# Revision 1.17  2004/06/30 21:25:07  hlieu
# fix test.pl to work on Seq objects
#
# Revision 1.16  2004/06/30 21:00:14  hlieu
# major refactor, pre-release, remove objects for Fasta, ASN, GB2Seq, _SeqHoundParser, replace with Bio::Seq code and object
#
# Revision 1.15  2004/06/29 20:31:12  hlieu
# latest fixes
#
# Revision 1.14  2004/06/24 18:16:52  hlieu
# update for BioPerl release, bug 2533
#
# Revision 1.13  2004/06/22 19:35:46  ryao
# Modified SHoundGetFastaFromRedundantGroupID in order to get correct defline object. bug 2571 related.
#
# Revision 1.12  2004/06/01 14:07:15  ryao
# added new locus link functions
#
# Revision 1.11  2004/05/28 20:59:54  ryao
# correct parameter passing in a few functions; fix bug in _ProcessContent
#
# Revision 1.10  2004/05/14 17:31:03  ryao
# added SHoundGetFastaFromRedundantGroupID(List)
#
# Revision 1.9  2004/04/05 18:21:23  ryao
# minor change
#
# Revision 1.8  2004/04/05 18:14:24  ryao
# Added SHoundGetFastaKeyedList SHoundRedundantGroupKeyedList _ProcessContentKeyedList
#
# Revision 1.7  2004/02/26 15:42:09  ryao
# several bugs fixed
#
# Revision 1.5  2004/02/16 17:29:44  ryao
# remove SHoundGOECodeFromGi and SHoundGOPMIDFromGi, fix bug 1564.
#
# Revision 1.4  2004/01/16 20:48:36  ryao
# added SHoundGetXMLBioseq, changed SHoundXMLBioseq as a wrapper function. This change is consist with other remote APIsSeqHound.pm
#
# Revision 1.3  2004/01/13 16:45:55  ryao
# change SHoundIsInit to SHoundIsInited to consist with other remote API calls
#
# Revision 1.2  2004/01/09 22:13:00  ryao
# add SHoundGetLargestMMDBID
#
# Revision 1.1.1.10  2003/06/18 08:41:39  root
# Automated import from Sourceforge.net
#
# Revision 1.11  2003/06/16 21:46:06  haocl
# updated test.pl and minor fixes in SeqHound.pm, sample.pl
#
# Revision 1.10  2003/01/20 15:50:52  haocl
# added sample code and modifications to simplify interface
#
# Revision 1.9  2003/01/07 22:24:31  haocl
# added buffering functions
#
# Revision 1.8  2003/01/02 19:29:27  haocl
# made minor change to dependency
#
# Revision 1.7  2002/10/31 18:35:21  haocl
# added SHoundGetSeqFromGenBank
#
# Revision 1.6  2002/10/30 17:45:19  haocl
# added _DecodeChromFlag; better reporting in test
#
# Revision 1.4  2002/10/22 22:13:21  haocl
# updated files and added testing functions
#
# Revision 1.3  2002/10/07 20:07:08  haocl
# removing files and adding files to reflect new design
#
# Revision 1.2  2002/09/27 21:47:23  haocl
# added more documentation and Id and Log
#
#
