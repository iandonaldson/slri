# This is an extensive test driver for functions implemented in SeqHound
use Bio::DB::SeqHound;
use Bio::SeqIO;
use Bio::Root::Root;
use Bio::DB::WebDBSeqI;
use Bio::Root::IO;
use IO::String;
use Bio::DB::Query::GenBank;
use strict;

my $sh = new Bio::DB::SeqHound();

#####################
#test get_Seq_by_acc
#####################

my $what_to_test = "get_Seq_by_acc";
my @genbank_acc = ('M34830', 'tttx', 'M98777',  'S43921');
my @refseq_acc = ('NP_443717', 'NM_131213', 'NM_194279', 'XP_341637');
my @embl_acc = ('X17276', 'X53814', 'Z18633', 'X59781' );
my @swiss_acc = ('P80438', 'Q04616', 'P80385', 'P31441');
test_result ($what_to_test, @genbank_acc);
test_result ($what_to_test, @refseq_acc);
test_result ($what_to_test, @embl_acc);
test_result ($what_to_test, @swiss_acc);

#-------------------
#test get_Seq_by_gi
#-------------------

$what_to_test = "get_Seq_by_gi";
my @genbank_gi = (141740, 161966, 255064);
my @refseq_gi = (34996479, 4557284, 45185482 );
my @swiss_gi = (1168186, 1168195, 2507558);
my @embl_gi =(5, 11002, 313523);
test_result ($what_to_test, @genbank_gi);
test_result ($what_to_test, @refseq_gi);
test_result ($what_to_test, @swiss_gi);
test_result ($what_to_test, @embl_gi);


#--------------------
#test get_Seq_by_id
#--------------------

$what_to_test = "get_Seq_by_id";
my @genbank_id = ('J05128', 'M34830', 'S43442', 141740, 255092, 'ABCAARAA', 'TRBANTRPB' );
my @refseq_id = ('NP_443717', 34996479, 'NM_006732', 'NP_005251');
my @swiss_id = ('Q04616', 'P80438', 'P41932', 1168186, '12KD_MYCSM', 'ACTO_ACACA');
my @embl_id = (5, 'tttx', 'X63723', 'AMSAT1D', 'HIVAV3AAU', 'BBBETA2GP', 0 );
test_result ($what_to_test, @genbank_id);
test_result ($what_to_test, @refseq_id);
test_result ($what_to_test, @swiss_id);
test_result ($what_to_test, @embl_id);

#----------------------
#test get_Stream_by_id
#----------------------

$what_to_test = "get_Stream_by_id";
#can pass above @genbank_id, @refseq_id, @embl_id and swiss_id into this test function
test_result_stream($what_to_test, \@genbank_id);
test_result_stream($what_to_test, \@refseq_id);
test_result_stream($what_to_test, \@swiss_id);
test_result_stream($what_to_test, \@embl_id);


#---------------------
#test get_Stream_by_gi
#---------------------

$what_to_test = "get_Stream_by_gi";
#can pass above @genbank_gi, @embl_gi, @refseq_gi and @swiss_gi into this test function
test_result_stream($what_to_test, \@genbank_gi);
test_result_stream($what_to_test, \@refseq_gi);
test_result_stream($what_to_test, \@swiss_gi);
test_result_stream($what_to_test, \@embl_gi);

#----------------------
#test get_Stream_by_acc
#----------------------

$what_to_test = "get_Stream_by_acc";
#can pass above @genbank_acc, @embl_acc, @refseq_acc and @swiss_acc into this test functio
test_result_stream($what_to_test, \@genbank_acc);
test_result_stream($what_to_test, \@refseq_acc);
test_result_stream($what_to_test, \@swiss_acc);
test_result_stream($what_to_test, \@embl_acc);

#------------------------
#test get_Stream_by_query
#------------------------

$what_to_test = "get_Stream_by_query";
my $query = Bio::DB::Query::GenBank->new(-db=>'nucleotide',
					 -ids=>['X02597', 'X63732', 11002, 4557284]);

my $query_string = "Candida maltosa 26S ribosomal RNA gene";
my $query2 = Bio::DB::Query::GenBank->new (-db=>'nucleotide',
		                          -query=>$query_string);
test_result_stream($what_to_test, $query);
test_result_stream($what_to_test, $query2);


####################
#subroutines
####################
sub test_result {
my ($what_to_test, @test_list) = @_;
foreach my $i (@test_list){
	my $seq_a = $sh->$what_to_test ($i);
	if (defined $seq_a){
		print "======From SeqHound: $what_to_test: $i\n";
		print "sequence:  ", $seq_a->seq, "\n";
		print "accession:   ", $seq_a->accession_number, "\n";
    		print "display_id:  ", $seq_a->display_id, "\n";
    		print "primary_id:  ", $seq_a->primary_id, "\n";
   		print "length:      ", $seq_a->length, "\n";
    		print "desc:        ", $seq_a->desc, "\n";
    		print "alphabet:    ", $seq_a->alphabet, "\n";
    		print "Seqfeature:  ", $seq_a->get_SeqFeatures, "\n";
		foreach my $feat ($seq_a->get_SeqFeatures())
		{
			print "start:        ", $feat->start, "\n";
			print "primary_tag:  ", $feat->primary_tag, "\n";
			print "source_tag:   ", $feat->source_tag, "\n";
		}
		print "obj_id:      ", $seq_a->object_id, "\n";
   		print "namespace:   ", $seq_a->namespace, "\n";
    		print "dis_name:    ", $seq_a->display_name, "\n";
    		print "description: ", $seq_a->description, "\n";
    		print "annotation:  ", $seq_a->annotation, "\n";
   		print "featurecount ", $seq_a->feature_count, "\n";
    		print "primary seq: ", $seq_a->primary_seq, "\n";
   		print "species:     ", $seq_a->species, "\n";
		my $species = $seq_a->species;
		if (defined $species){
  			print "species_commonname:      ", $species->common_name, "\n";
	   		print "species_ncbitaxid:       ", $species->ncbi_taxid, "\n";
		}
       	 	if (($seq_a->alphabet ne 'protein') && defined $seq_a->seq){
			print "reverse seq: ", $seq_a->revcom->seq, "\n";
        		print "translate seq: ", $seq_a->translate->seq, "\n\n";
		}
	}
	else{
		print "can not retrieve Bio::seq object $what_to_test: $i\n";
	}
}
}

sub test_result_stream{
	my ($what_to_test, @list)=@_;
	my $seqio = $sh->$what_to_test(@list);
	if (defined $seqio){
	while (my $seq = $seqio->next_seq){
		if (defined $seq){
		print "========From SeqHound: $what_to_test: ", $seq->primary_id, "\n";
		print "sequence:    ", $seq->seq, "\n";
    		print "accession:   ", $seq->accession_number, "\n";
    		print "display_id:  ", $seq->display_id, "\n";
    		print "primary_id:  ", $seq->primary_id, "\n";
    		print "length:      ", $seq->length, "\n";
    		print "desc:        ", $seq->desc, "\n";
    		print "alphabet:    ", $seq->alphabet, "\n";
    		print "feature:     ", $seq->get_SeqFeatures, "\n";
		foreach my $feat ($seq->get_SeqFeatures())
		{
			print "start:        ", $feat->start, "\n";
			print "primary_tag:  ", $feat->primary_tag, "\n";
			print "source_tag:   ", $feat->source_tag, "\n";
		}
		print "obj_id:      ", $seq->object_id, "\n";
    		print "namespace:   ", $seq->namespace, "\n";
    		print "dis_name:    ", $seq->display_name, "\n";
    		print "description: ", $seq->description, "\n";
    		print "annotation:  ", $seq->annotation, "\n";
    		print "featurecount ", $seq->feature_count, "\n";
    		print "primary seq: ", $seq->primary_seq, "\n";
    		print "species:     ", $seq->species, "\n\n";
		my $species = $seq->species;
  		if (defined $species){
			print "species_commonname:      ", $species->common_name, "\n";
  			print "species_ncbitaxid:       ", $species->ncbi_taxid, "\n";
		}
		if ($seq->alphabet ne "protein" && defined $seq->seq){
			print "reverse seq: ", $seq->revcom->seq, "\n";
			print "translate seq: ", $seq->translate->seq, "\n\n";
		}
		}
		else {
			print "can not retrieve Bio::Seq object by id: ", $seq->primary_id, "\n";
		}
	}
	}
	else {
		print "can not retrieve Bio::SeqIO stream object\n";
	}
}
