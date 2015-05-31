#!/usr/bin/perl -w
# Before `make install' is performed this script should be runnable with
# `make test'. After `make install' it should work as `perl test.pl'

#########################

# change 'tests => 1' to 'tests => last_test_to_print';

use strict;
use Test::More qw(no_plan);

use Bio::SeqHound::SeqHound;
use IO::String;
use AutoLoader qw(AUTOLOAD);
use POSIX qw(strftime);

open LOG, ">biop_test.log";

my $now =strftime("%a %b %e %H:%M:%S %Y", localtime);
print LOG "Bioperl test script starting:  $now\n";
print LOG "=======================================================\n";

### autoflushes LOG, writes to STDOUT
select (LOG);
$|++; 
select (STDOUT);
$| = 1;

########### pretty print formatting ###########

my ($fnct, $status, $input);
format LOG_TOP = 
FUNCTION				        RESULT	        INPUT
--------				        ------		-----
.

format LOG = 
@<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<	 @<<<<<<	@<<<<<<<<<<<<<<<<<<<<<<<<<
$fnct,                                           $status,	$input
.

###### logging function, writes using the format above.
sub logger 
{
   my $ret = shift;
   $fnct = shift;
   $input = shift;
   if ( "0" eq $ret ) {
      $status = "failed";
   } else {
      $status = "ok";
   }
   write LOG;
}

my (@fncts, @reply, @failed, @tmp, @tmp2, @seqlist, @gb, @list);

my ($test, $tmp, $tmp2, $i, $j, $size, $seq, $sequence, $gb);

my $seqhound = new Bio::SeqHound::SeqHound("seqhound.blueprint.org", "cgi-bin/seqrem");



if( !ok(defined $seqhound, 'new seqhound instance created') ){
   push(@failed, "new");
   print "Constructor failed, cannot continue with test, check that you have all files\n";
   exit(1);
}
if( !is($seqhound->_server(),  "http://seqhound.blueprint.org/", 'correct server') ){
   push(@failed, "_server");
}
if( !is($seqhound->_path(), "cgi-bin/seqrem" , 'correct path') ){
   push(@failed, "_path");
}


#########################
# Insert your test code below, the Test module is use()ed here so read
# its man page ( perldoc Test ) for help writing this test script.
#########################

sub InitializationFunctions
{
	
    $fnct = "SHoundInit";
    $tmp  = "TRUE";
    $tmp2 = "test.pl"; 
    $test = $seqhound->$fnct($tmp, $tmp2);
    @reply = $test->getValues();
    if(!is($reply[0], "TRUE", "$fnct: correct return value") ){
        push(@failed, $fnct);
    }
    logger ($reply[0], $fnct, $tmp." & $tmp2");
     

    $fnct = "SHoundNetEntrezInit";
    $test = $seqhound->$fnct($tmp2);
    @reply = $test->getValues();
    if(!is($reply[0], "TRUE", "$fnct: correct return value")){
        push(@failed, $fnct);
    }
    logger ($reply[0], $fnct, $tmp2);
	 
    $fnct = "SHoundIsNetEntrezOn";
    $test = $seqhound->$fnct();
    @reply = $test->getValues();
    if(!is ($reply[0], "TRUE", "$fnct: correct return value") ){
        push(@failed, $fnct);
    }
    logger ($reply[0], $fnct, "");
	 
    $fnct = "SHoundIsInited";
    $test = $seqhound->$fnct();
    if (!is ($test, "TRUE", "$fnct: correct return value")){
        push (@failed, $fnct);
    }
    logger ($test, $fnct, "");

}


sub  CoreFunctions 
{
   @fncts = ( "SHoundFindAcc", "SHoundFindName", 
              "SHoundAccFromGi", "SHoundGiFromOMIM", 
              "SHoundOMIMFromGi", "SHoundMoleculeType", 
              "SHoundIsProtein", "SHoundExists", 
              "SHoundGetDefline", "SHoundDNAFromProtein",
              "SHoundProteinFromDNA", "SHoundMuidFromGi",
              "SHoundMuidFrom3D", "SHoundGetAccFromRetiredGi",
              "SHoundSequenceLength", "SHoundGetNameByGi",
              );
   @tmp = ( "X88853", "AF060234", 230779, 103950,
            4505355, 6322015, 6322015, 6322015, 
            230779, 8698831, 2463028, 6322016, 
            11, 800, 4557225, 1235643 );
   $i = 0;
   foreach $fnct ( @fncts )
   {
	$test = $seqhound->$fnct($tmp[$i]);
    	@reply = $test->getValues();
        if ( !ok ( $reply[0] ne "0", "$fnct: is not 0." ) )
           { push ( @failed, $fnct ); }
	logger ($reply[0], $fnct, $tmp[$i++]);
   }	

   @fncts = ( "SHoundFindAccList", "SHoundFindNameList",
              "SHoundAccFromGiList", "SHoundGiFromOMIMList",
              "SHoundOMIMFromGiList", "SHoundGetDeflineList", 
              "SHoundDNAFromProteinList", "SHoundProteinFromDNAList",
              "SHoundMuidFromGiList", "SHoundMuidFrom3DList" );
   @tmp = ( ["P00951","X88853"], ["AF060234","ECJ224437","AF165032"],
            [230779,4557225], [137150,103950,108345],
            [4505355,4557225], [230779,4505355],
            [8698831,4557225], [2463028,34111], 
            [6322016,4557225], [11,12] );
   $i = 0;
   foreach $fnct ( @fncts )
   {
	$test = $seqhound->$fnct( @{$tmp[$i]} );
    	@reply = $test->getValues();
        if ( !ok ( $reply[0] ne "0", "$fnct: is not 0." ) )
           { push ( @failed, $fnct ); }
	logger ( $reply[0], $fnct, Bio::SeqHound::SLRIMisc::Array2String ( \@{$tmp[$i++]} ) );
   }

   @fncts = ( "SHoundGetReferenceIDFromGi", "SHoundGiFromReferenceID" );
   @tmp = ( 6322016, 85190481 ); 
   @tmp2 = ( 2, 2 );
   $i = 0;
   foreach $fnct ( @fncts )
   {
	$test = $seqhound->$fnct( $tmp[$i], $tmp2[$i] );
    	@reply = $test->getValues();
        if ( !ok ( $reply[0] ne "0", "$fnct: is not 0." ) )
           { push ( @failed, $fnct ); }
	logger ( $reply[0], $fnct, $tmp[$i]." & $tmp2[$i++]" );
   }

   @fncts = ( "SHoundGetFasta", "SHoundGetFastaFromRedundantGroupID" );
   @tmp = ( 230779, 11 );
   $i = 0;
   foreach $fnct ( @fncts )
   {
	$seq = $seqhound->$fnct( $tmp[$i] );
    	$sequence = $seq->seq();
#print "=====$sequence\n";
        if ( !ok ( $sequence ne "0", "$fnct: is not 0." ) )
           { push ( @failed, $fnct ); }
	logger ( $sequence, $fnct, $tmp[$i++]);
   }

   @fncts = ( "SHoundGiFromReferenceList", "SHoundGetReferenceIDFromGiList");
   @tmp = ( [97002444,85190481], [6322016,21071030] );
   @tmp2 = ( 2, 2 );
   $i = 0;
   foreach $fnct ( @fncts ) 
   {
	$test = $seqhound->$fnct( $tmp2[$i] , @{$tmp[$i]} );
    	@reply = $test->getValues();
        if ( !ok ( $reply[0] ne "0", "$fnct: is not 0." ) )
           { push ( @failed, $fnct ); }
	logger ( $reply[0], $fnct, Bio::SeqHound::SLRIMisc::Array2String(\@{$tmp[$i]})." & $tmp2[$i++]" );
   }

   @fncts = ( "SHoundGetFastaList" );
   @tmp = ( [230779,6322015] );
   $i = 0;
   foreach $fnct ( @fncts )
   {
	@seqlist = $seqhound->$fnct( @{$tmp[$i]} );
    	$sequence = $seqlist[0]->seq();
        if ( !ok ( $sequence ne "0", "$fnct: is not 0." ) )
           { push ( @failed, $fnct ); }
	logger ( $reply[0], $fnct, Bio::SeqHound::SLRIMisc::Array2String( \@{$tmp[$i++]} ));
   }
}

sub ASNFunctions
{
     @fncts = ( 
                "SHoundSeqIdFromGi", "SHoundGetBioseq",
                "SHoundXMLBioseq", "SHoundGetXMLBioseq",
                "SHoundGetSeqEntry", "SHoundGetXMLSeqEntry",
                "SHoundGetBioseqSet"
              );
     foreach $fnct ( @fncts )
     {
        $tmp = ($fnct eq "SHoundGetBioseqSet") ? 2791836 : 230779;
	$test = $seqhound->$fnct($tmp);
    	@reply = $test->getValues();
        if ( ok ( defined $reply[0], "$fnct: return value defined." ) ) 
        { 
           if ( !ok ( $reply[0] ne "0" , "$fnct: is not 0." ) )
              { push ( @failed, $fnct ); }
        }
	logger ($reply[0], $fnct, $tmp);
     }

     @fncts = ( "SHoundGetGenBankff" );
     @tmp = ( 4557225 );
     $i = 0;
     foreach $fnct ( @fncts ) 
     {
        $gb = $seqhound->$fnct ($tmp[$i]);
        #A SeqHoundGB object has an array of Bio::Seq objects
        if ( ok ( defined $gb, "$fnct: SeqHoundGB obj defined" ) )
        {
            my @keys = $gb->getKey();
	    if ( !ok ( $tmp[$i] == $keys[$i], "$fnct: keys matched" ) )
            {
	        push ( @failed, $fnct );
            }
            #$gb->print_to_file ( );
        } else { push (@failed, $fnct); }
        logger ( $gb, $fnct, $tmp[$i++] );
     }

     @fncts = ( "SHoundGetGenBankffList" );
     @tmp = ( [4557225,230779,876787] );
     $i = 0;
     foreach $fnct ( @fncts ) 
     {
        @gb = $seqhound->$fnct ( @{$tmp[$i]} );
        if ( ok ( defined $gb[$i], "$fnct: SeqHoundGB obj defined" ) )
        {
           $j = 0;
           foreach $gb ( @gb )
           {
	       my @keys = $gb->getKey();
               if ( !ok ( $tmp[$i][$j] == $keys[0], "$fnct: keys matched" ) )
               {
                   push ( @failed, $fnct );
               }
	       $j++;
	       # $gb->print_to_file ( $tmp[$i][$j] );
           }
         } else { push (@failed, $fnct ); }
	 logger ( $gb, $fnct, Bio::SeqHound::SLRIMisc::Array2String(\@{$tmp[$i++]} ));
     }

     @fncts = ("SHoundGetSeqFromGenBank");
     @tmp = (4557225);
     $i=0;
     foreach $fnct (@fncts )
     {
         $test = $seqhound -> $fnct($tmp[$i]);
	 if ( ok (defined $test, "$fnct: return value defined.") )
	 {
	     if (!ok ($test->seq() ne "", "$fnct: sequence is not null.") )
	     { push (@failed, $fnct); }
	 }
	 logger ($test, $fnct, $tmp[$i++]);
		 
     }
	 
     @fncts = (
                "SHoundSeqIdFromGiList", "SHoundGetBioseqSetList",
                "SHoundGetSeqEntryList", "SHoundGetBioseqList"
              );
     foreach $fnct ( @fncts )
     {
         @list = ($fnct eq "SHoundGetBioseqSetList") ? (2791836,93,84) : (230779,876787);
        $test = $seqhound->$fnct ( @list );
    	@reply = $test->getValues();
        if ( ok ( defined $reply[0], "$fnct: return value defined." ) ) 
        { 
           if ( !ok ( $reply[0] ne "0" , "$fnct: is not 0." ) )
              { push ( @failed, $fnct ); }
        }
	logger ($reply[0], $fnct, Bio::SeqHound::SLRIMisc::Array2String(\@list));
     }
}

sub LocusLinkFunctions
{
   @fncts = ( "SHoundGOIDFromLLID", "SHoundOMIMFromLLID", 
              "SHoundCDDIDFromLLID", "SHoundLLIDFromOMIM", 
              "SHoundLLIDFromCDDID", "SHoundGiFromGOID",
              "SHoundGOIDFromGi", "SHoundGiFromLLID", 
              "SHoundLLIDFromGi", "SHoundLocusFromGi" );
   @tmp = ( 1, 1, 1, 103950, "smart00408", 8320, 4557225, 9,
            4826760, 4826760 );
   $i = 0;
   foreach $fnct ( @fncts )
   { 
	$test = $seqhound->$fnct ( $tmp[$i] );
    	@reply = $test->getValues();
        if ( !ok ( $reply[0] ne "0" , "$fnct: is not 0." ) )
           { push ( @failed, $fnct ); }
	logger ( $reply[0], $fnct, $tmp[$i++] );
   }

   @fncts = ( "SHoundGiFromLLIDList", "SHoundGOIDFromGiList",
              "SHoundLLIDFromGiList", "SHoundGiFromGOIDList");
   @tmp = ( [9,1,2], [4501841,4557225], [4501843,4557225], [8033,4867,6419] );
   $i = 0;
   foreach $fnct ( @fncts ) 
   {
	$test = $seqhound->$fnct ( @{$tmp[$i]} );
    	@reply = $test->getValues();
        if ( !ok ( $reply[0] ne "0" , "$fnct: is not 0." ) )
           { push ( @failed, $fnct ); }
	logger ( $reply[0], $fnct, Bio::SeqHound::SLRIMisc::Array2String(\@{$tmp[$i++]}) );
   }

   @fncts = ( "SHoundLLIDFromGOIDAndECode", "SHoundGiFromGOIDAndECode",
              "SHoundGOECodeFromGiAndGOID", "SHoundGOPMIDFromGiAndGOID"
            );
   @tmp = ( 6810, 5554, 4557225, 21071030 );
   @tmp2 = ( "IEA", "ND", 17114, 5554 );
   $i = 0;
   foreach $fnct ( @fncts ) 
   {
	$test = $seqhound->$fnct( $tmp[$i], $tmp2[$i] );
    	@reply = $test->getValues();
        if ( !ok ( $reply[0] ne "0" , "$fnct: is not 0." ) )
           { push ( @failed, $fnct ); }
	logger ( $reply[0], $fnct, $tmp[$i]." & $tmp2[$i++]" );
   }
 
   $fnct = "SHoundGiFromGOIDListAndECode";
   @list = (8320,4);
   $tmp2 = "NR";
   $test = $seqhound->$fnct ( $tmp2, @list );
   @reply = $test->getValues();
   if ( !ok ( $reply[0] ne "0", "$fnct: is not 0." ) )
      { push ( @failed, $fnct ); }
   logger ($reply[0], $fnct, Bio::SeqHound::SLRIMisc::Array2String(\@list)." & $tmp2");
}

sub ConservedDomainFunctions
{
    @fncts = ( 
               "SHoundGiFromCDDID", "SHoundCDDIDFromGi",
               "SHoundGetGisByNumberOfDomains", "SHoundGetDomainIdFromLabel",
               "SHoundGetGisByDomainId", "SHoundGetDomainLabelFromDomainId",
               "SHoundGetDomainsFromGi" 
             );
    @tmp = ( "pfam00664", 4557225, 40, "COLFI", "smart00038", "smart00038",
             6322015 );
    $i = 0; 
    foreach $fnct ( @fncts )
    {
	$test = $seqhound->$fnct($tmp[$i]);
    	@reply = $test->getValues();
        if ( !ok ( $reply[0] ne "0", "$fnct: is not 0." ) )
           { push ( @failed, "$fnct" ); }
	logger ($reply[0], $fnct, $tmp[$i++]);
    }

    @fncts = ( "SHoundGiFromCDDIDList", "SHoundCDDIDFromGiList" );
    @tmp = ( 
             ["pfam00202", "pfam01835"],
             [4501841,4557225]
           );
    $i = 0;
    foreach $fnct ( @fncts )
    {
	$test = $seqhound->$fnct( @{$tmp[$i]} );
    	@reply = $test->getValues();
        if ( !ok ( $reply[0] ne "0", "$fnct: is not 0." ) )
           { push ( @failed, "$fnct" ); }
	logger ( $reply[0], $fnct, Bio::SeqHound::SLRIMisc::Array2String ( \@{$tmp[$i++]} ) );
    }

    @fncts = ( "SHoundGetGisByDomainIdAndEvalue", "SHoundGetDomainsFromGiWithEvalue",
			   "SHoundCDDScoreFromGi" );
    @tmp = ( "smart00038", 6322015, 34870013 );
    @tmp2 = ( 1.0, 1.0, "COG0606" );
    $i = 0;
    foreach $fnct ( @fncts ) 
    {
	$test = $seqhound->$fnct( $tmp[$i], $tmp2[$i] );
    	@reply = $test->getValues();
        if ( !ok ( $reply[0] ne "0", "$fnct: is not 0." ) )
           { push ( @failed, "$fnct" ); }
	logger ( $reply[0], $fnct, $tmp[$i]." & $tmp2[$i++]" );
    }
}

sub GeneOntologyFunctions
{
   @fncts = ( 
              "SHoundGODBGetRecordByReference", "SHoundGODBGetChildrenOf",
              "SHoundGODBGetParentOf", "SHoundGODBGetAllAncestors",
              "SHoundGODBGetAllChildren", "SHoundGODBGetNameByID",
              "SHoundGODBGetClassification" 
            );
   @tmp = ( "EC", 7596, 7596, 7596, 6886, 4866, 4866 );
   $i = 0;
   foreach $fnct ( @fncts )
   {
	$test = $seqhound->$fnct( $tmp[$i] );
    	@reply = $test->getValues();
        if ( !ok ( $reply[0] ne "0", "$fnct: is not 0." ) )
           { push ( @failed, $fnct ); } 
	logger ( $reply[0], $fnct, $tmp[$i++] );
   }
}

sub RedundantGiFunctions
{
   @fncts = (  
              "SHoundIsNRFirst", "SHoundRedundantGroup",
              "SHoundRedundantGroupIDFromGI", "SHoundRedundantGroupFromID",
              "SHoundFirstOfRedundantGroupFromID", "SHoundGOIDFromRedundantGi" 
            );	
   @tmp = ( 1172650, 230779, 6017926, 362477, 2, 4557225 );
   $i = 0;
   foreach $fnct ( @fncts )
   {
      $test = $seqhound->$fnct ( $tmp[$i] );
      @reply = $test->getValues();
      if ( !ok ( $reply[0] ne "0", "$fnct: is not 0." ) )
          { push ( @failed, $fnct ); }
      logger ( $reply[0], $fnct, $tmp[$i++] );
   }

   @fncts = ( "SHoundRedundantGroupIDFromGIList", "SHoundRedundantGroupFromIDList",
              "SHoundFirstOfRedundantGroupFromIDList", "SHoundGOIDFromRedundantGiList",
			  "SHoundRedundantGroupList" );
   @tmp = ( [32,6017926], [362477, 408917],
            [362477,408917],[4557225,21071030],[230779,32,4586891] );
   $i = 0;
   foreach $fnct ( @fncts )
   {
      $test = $seqhound->$fnct ( @{$tmp[$i]} );
      @reply = $test->getValues();
      if ( !ok ( $reply[0] ne "0", "$fnct: is not 0." ) )
          { push ( @failed, $fnct ); }
      logger ( $reply[0], $fnct, Bio::SeqHound::SLRIMisc::Array2String( \@{$tmp[$i++]} ));
   }
} 	

sub StructureFunctions
{
    @fncts = ( 
               "SHound3DbWhat", "SHoundGetLargestMMDBID", 
               "SHound3DExists", "SHoundGet3D",
               "SHoundGet3DfromPdbId", "SHoundGetXML3D",
               "SHoundGetPDB3D", "SHound3DFromGi" 
             );
    foreach $fnct ( @fncts )
    {
        $tmp = ( $fnct eq "SHoundGetLargestMMDBID" ) ? "" : 11;
        $tmp = ( $fnct eq "SHoundGet3DfromPdbId" ) ? "3INS" : $tmp;
        $tmp = ( $fnct eq "SHound3DFromGi" ) ? "230779" : $tmp;
	$test = $seqhound->$fnct( $tmp );
    	@reply = $test->getValues();
        if ( !ok ( $reply[0] ne "0", "$fnct : is not 0." ) )
           { push ( @failed, $fnct ); }

	logger ( $reply[0], $fnct, $tmp );
    }

    @fncts = ( "SHoundGiFrom3D", "SHoundGiFromPDBchain" );
    $tmp2 = "FALSE";
    $tmp = 11;
    foreach $fnct ( @fncts )
    {
        $tmp = ( $fnct eq "SHoundGiFromPDBchain" ) ? "9XIM" : 11;
        $tmp2 = ( $fnct eq "SHoundGiFromPDBchain" ) ? "A" : "FALSE";
	$test = $seqhound->$fnct( $tmp, $tmp2 );
    	@reply = $test->getValues();
        if ( !ok ( $reply[0] ne "0", "$fnct : is not 0." ) )
           { push ( @failed, $fnct ); }

	logger ( $reply[0], $fnct, $tmp." & $tmp2" );
    }

    $fnct = "SHoundGiFrom3DList";
    @list  = (11,23,32);
    $tmp2 = "FALSE";
    $test = $seqhound->$fnct($tmp2, @list);
    @reply = $test->getValues();
    if ( !ok ( $reply[0] ne "0", "$fnct: is not 0." ) )
       { push ( @failed, $fnct ); }
    logger ($reply[0], $fnct, Bio::SeqHound::SLRIMisc::Array2String(\@list)." & $tmp2");
	
    $fnct = "SHound3DFromGiList";
    @list  = (442560,442572);
    $test = $seqhound->$fnct(@list);
    @reply = $test->getValues();
    if ( !ok ( $reply[0] ne "0", "$fnct: is not 0." ) )
        { push ( @failed, $fnct );} 
    logger ($reply[0], $fnct, Bio::SeqHound::SLRIMisc::Array2String(\@list));
	
    $fnct = "SHound3DListFromTaxID";
    $tmp  = "56636";
    $test = $seqhound->$fnct($tmp);
    @reply = $test->getValues();
    if ( !ok ( $reply[0] ne "0", "$fnct: is not 0." ) )
       { push ( @failed, $fnct ); }
    logger ($reply[0], $fnct, $tmp);

}


sub TaxonomyFunctions
{
    @fncts = ( 
               "SHoundTaxIDFromGi", "SHoundTaxIDFrom3D",
               "SHoundProteinsFromTaxID", "SHoundProteinsFromTaxIDIII",
               "SHoundDNAFromTaxID", "SHoundDNAFromTaxIDIII",
               "SHoundProteinsFromChromosome", "SHoundAllGenomes",
               "SHoundDNAFromChromosome", "SHoundGetTaxChildNodes",
               "SHoundGetAllTaxProgeny", "SHoundGetTaxParent",
               "SHoundGetAllTaxAncestors", "SHoundIsTaxMerged",
               "SHoundIsaxDeleted", "SHoundGetTaxNameFromTaxID",
			   "SHoundGetNewTaxIDFromMergedTaxID"
             );
    @tmp   = ( 230779, 11, 56636, 3055, 10665, 3055,
               10, 139, 10, 9606, 9606, 56636,
               56636, 56636, 56636, 9606, 395 );
    $i = 0;
    foreach $fnct ( @fncts )
    {
	$test = $seqhound->$fnct($tmp[$i]);
    	@reply = $test->getValues();
        if ( !ok ( $reply[0] ne "0", "$fnct: is not 0." ) )
           { push ( @failed, $fnct ); }
	logger ($reply[0], $fnct, $tmp[$i++]);
    }	
    
	@fncts = ( "SHoundTaxIDFromGiList", "SHoundTaxIDFrom3DList",
               "SHoundProteinsFromTaxIDList", "SHoundDNAFromTaxIDList",
               "SHoundProteinsFromChromosomeList", "SHoundDNAFromChromosomeList",
               "SHoundGetTaxChildNodesList" );
    @tmp = (
            [230779,443580],
            [11,12], [10665,555],
            [10665,555], [169,10],
            [171,10,12], [9606,10066]
           );
    $i = 0;
    foreach $fnct ( @fncts )
    {
	$test = $seqhound->$fnct(@{$tmp[$i]});
    	@reply = $test->getValues();
        if ( !ok ( $reply[0] ne "0", "$fnct: is not 0." ) )
           { push ( @failed, $fnct ); }
	logger ($reply[0], $fnct, Bio::SeqHound::SLRIMisc::Array2String(\@{$tmp[$i++]}));
	}

    @fncts = ( "SHoundProteinsFromOrganismList", "SHoundChromosomeFromGenomeList",
               "SHoundDNAFromOrganismList" );
    @tmp = ( [9606,56636], [6239,562,2166],
             [9606,6239] );
    @tmp2 = ( 1, 1, 1 );
    $i = 0;
    foreach $fnct ( @fncts )
    {
        print $fnct eq "SHoundProteinsFromOrganismList" ? "takes a while...\n" : "";
	$test = $seqhound->$fnct( $tmp2[$i], @{$tmp[$i]} );
    	@reply = $test->getValues();
        if ( !ok ( $reply[0] ne "0", "$fnct: is not 0." ) )
           { push ( @failed, $fnct ); }
	logger ($reply[0], $fnct, Bio::SeqHound::SLRIMisc::Array2String(\@{$tmp[$i]})." & $tmp2[$i++]");
	}

    @fncts = ( "SHoundProteinsFromOrganism", "SHoundChromosomeFromGenome",
               "SHoundDNAFromOrganism", "SHoundGetTaxNameFromTaxIDByClass",
               "SHoundGetTaxLineageFromTaxID" );
    @tmp2 = ( 9606,6239,6239, 9606, 9606, 9606 );
    @tmp = ( 255, 255, 255, "SLRI_taxon_name_name_class_scientific_name",
             "SHoundGetTaxLineage_FULL", "SHoundGetTaxLineage_FULL" );
    $i = 0;
    
    foreach $fnct ( @fncts )
    {
	$test = $seqhound->$fnct( $tmp2[$i], $tmp[$i] );
    	@reply = $test->getValues();
        if ( !ok ( $reply[0] ne "0", "$fnct: is not 0." ) )
           { push ( @failed, $fnct ); }
	logger ($reply[0], $fnct, $tmp2[$i]." & $tmp[$i++]");
    }
   
    @fncts =("SHoundAllGenomesWithScientificNames", "SHoundAllGenomesWithNumProteins");
    foreach $fnct (@fncts)
    {
       my @list = $seqhound->$fnct();
    	   if ( !ok ( scalar(@list) ne "0", "$fnct: is not 0." ) )
           { push ( @failed, $fnct ); }
       logger ($reply[0], $fnct, "");
    }
}

sub OMIMFunctions
{
    @fncts = ( "SHoundGiFromOMIM", "SHoundOMIMFromGi" );
    @tmp = ( 137150, 4502501 );
    $i = 0;
    foreach $fnct ( @fncts )
    {
	$test = $seqhound->$fnct( $tmp[$i] );
    	@reply = $test->getValues();
        if ( !ok ( $reply[0] ne "0", "$fnct: is not 0." ) )
           { push ( @failed, $fnct ); }
	logger ( $reply[0], $fnct, $tmp[$i++] );
    }

    @fncts = ( "SHoundGiFromOMIMList", "SHoundOMIMFromGiList" );
    @tmp = ( [137150,103950,108345], [4502501,4501843] );
    $i = 0;
    foreach  $fnct ( @fncts )
    {
	$test = $seqhound->$fnct( @{$tmp[$i]} );
    	@reply = $test->getValues();
        if ( !ok ( $reply[0] ne "0", "$fnct: is not 0." ) )
           { push ( @failed, $fnct ); }
	logger ( $reply[0], $fnct, Bio::SeqHound::SLRIMisc::Array2String ( \@{$tmp[$i++]} )  );
	}
}

sub NeighboursFunctions
{
   @fncts = ( "SHoundNeighboursFromGi", "SHoundNeighboursFromTaxID",
              "SHound3DNeighboursFromGi", "SHound3DNeighboursFromTaxID" );
   @tmp = ( 4557225, 107806, 4557225, 107806 );
   @tmp2 = ( 0.5, 1, 1.0, 1.0 );
   $i = 0;
   foreach $fnct ( @fncts ) 
   {
	$test = $seqhound->$fnct($tmp[$i], $tmp2[$i], "TRUE");
    	@reply = $test->getValues();
        if ( !ok ( $reply[0] ne "0", "$fnct: is not 0." ) )
           { push ( @failed, $fnct ); }
	logger ($reply[0], $fnct, $tmp[$i]." & $tmp2[$i++]");
   }	

   @fncts = ( "SHoundNeighboursFromGiList", "SHound3DNeighboursFromGiList" );
   @tmp = ( [4557225,4501843], [4557225,6009729] );
   @tmp2 = ( 0.5, 1.0 );
   $i = 0;
   foreach $fnct ( @fncts ) 
   {
        print $fnct eq "SHound3DNeighboursFromGiList" ? "takes a while....\n" : "";
	$test = $seqhound->$fnct( $tmp2[$i], @{$tmp[$i]} );
    	@reply = $test->getValues();
        if ( !ok ( $reply[0] ne "0", "$fnct: is not 0." ) )
           { push ( @failed, $fnct ); }
	logger ($reply[0], $fnct, Bio::SeqHound::SLRIMisc::Array2String ( \@{$tmp[$i]} )." & $tmp2[$i++]");
   }
}

sub UninitializationFunctions
{
   @fncts = ( "SHoundFini" );
   @tmp = ( "NULL" );
   $i = 0;
   foreach $fnct ( @fncts )
   { 
	$test = $seqhound->$fnct($tmp[$i]);
    	@reply = $test->getValues();
        if ( !ok ( $reply[0] ne "0", "$fnct: is not 0." ) )
           { push (@failed, $fnct); }
	logger ($reply[0], $fnct, $tmp[$i++]);
   }
}


sub KeyedListFunctions
{
    @fncts = ( "SHoundRedundantGroupKeyedList", "SHoundGetFastaKeyedList", 
               "SHoundGetFastaFromRedundantGroupIDKeyedList" );
    @tmp = ( [234,4557225], [111,123,4444], [362477,123,111] );
    $i = 0;
    foreach $fnct ( @fncts )
    {
	my %hash = $seqhound->$fnct( @{$tmp[$i]} );
        if ( !ok ( keys(%hash) > 0, "$fnct: size > 0." ) )
           { push ( @failed, $fnct ); }
        #use hash size to determine if return is OK
        $size =  keys(%hash);
        logger ($size, $fnct, Bio::SeqHound::SLRIMisc::Array2String(\@{$tmp[$i++]}));
    }
}	

sub Test 
{
   @fncts = ( "SHoundGetFastaFromRedundantGroupIDKeyedList" ); 
   @list = (19, 18, 22);
   foreach $fnct (@fncts) 
   {
      my %seqio = $seqhound->$fnct(@list);
      foreach my $gi ( keys(%seqio) ) {
         print STDOUT "key: $gi\n";
         my $seq = $seqio{$gi};
		 if ($seq ne 'null')
		 {
             my $sequence = $seq->seq();
             print STDOUT "sequence: $sequence\n";
             my $length = $seq->length();
             print STDOUT "length: $length\n";
             my $id = $seq->display_id();
             print STDOUT "id: $id\n";
             my $access = $seq->accession_number();
             print STDOUT "accession: $access\n";
             my $prime = $seq->primary_id();
             print STDOUT "prime id: $prime\n";
          }
	   }
   }
}


##################
# main starts here
##################

   InitializationFunctions ();
   ASNFunctions ();
   StructureFunctions ();
   TaxonomyFunctions ();
   RedundantGiFunctions ();
   OMIMFunctions ();
   NeighboursFunctions ();
   GeneOntologyFunctions ();
   ConservedDomainFunctions ();
   LocusLinkFunctions (); 
   CoreFunctions ();
   KeyedListFunctions ();
   Test();
   UninitializationFunctions ();

   print "\n######################################\n";
   if ( $#failed >= 0 )
   {

      print "\nFailed functions\n";
      foreach $fnct ( @failed ) 
         { print "$fnct\n"; }
      print "\n######################################\n";
   }
   print "\nDone.\n";
   print "See biop_test.log logged info.\n"; 
   print "\n######################################\n";
   print "\n";
