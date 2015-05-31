#!/usr/bin/perl
# Before `make install' is performed this script should be runnable with
# `make test'. After `make install' it should work as `perl test.pl'

#########################

use Test::More qw(no_plan);
use SeqHound;
use genbankIDConversionService;
use dbxrefService;
use AutoLoader qw(AUTOLOAD);
use POSIX qw(strftime);

open LOG, ">perl_test.log";

my $now =strftime("%a %b %e %H:%M:%S %Y", localtime);
print LOG "Perl test script starting:  $now\n";
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
@<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<	 @<<<<<<	@<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
$fnct,                                           $status,	$input
.

###### logging function, writes using the format above.
sub logger 
{
   my $ret = shift;
   $fnct = shift;
   $input = shift;
   if ( !defined $ret ) {
      $status = "failed";
   } else {
      $status = "ok";
   }
   write LOG;
}

my (@fncts, @reply, @failed, @tmp, @tmp2, @seqlist, @gb, @list);

my ($test, $tmp, $tmp2, $i, $j, $size, $seq, $sequence, $gb);

sub InitializationFunctions
{
	
    $fnct = "SHoundInit";
    $tmp  = "TRUE";
    $tmp2 = "test.pl"; 
    $test = &{"$fnct"}($tmp, $tmp2);
    
    if(!is($test, "TRUE", $fnct) ){
        push(@failed, $fnct);
    }
    logger ($test, $fnct, $tmp." & $tmp2");
     

    $fnct = "SHoundNetEntrezInit";
     $test = &{"$fnct"}($tmp2);
    
    if(!is($test, "TRUE", $fnct)){
        push(@failed, $fnct);
    }
    logger ($test, $fnct, $tmp2);
	 
    $fnct = "SHoundIsNetEntrezOn";
    $test = &{"$fnct"}();
    
    if(!is ($test, "TRUE", $fnct) ){
        push(@failed, $fnct);
    }
    logger ($test, $fnct, "");
	 
    $fnct = "SHoundIsInited";
     $test = &{"$fnct"}();
    if (!is ($test, "TRUE", $fnct)){
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
	 $test = &{"$fnct"}($tmp[$i]);
    	
        if ( !ok ( defined $test, $fnct ) )
           { push ( @failed, $fnct ); }
	logger ($test, $fnct, $tmp[$i++]);
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
	 $test = &{"$fnct"}( @{$tmp[$i]} );
    	
        if ( !ok ( defined $test, $fnct ) )
           { push ( @failed, $fnct ); }
	logger ( $test, $fnct, Array2Str($tmp[$i++]));
   }

   @fncts = ( "SHoundGetReferenceIDFromGi", "SHoundGiFromReferenceID" );
   @tmp = ( 6322016, 85190481 ); 
   @tmp2 = ( 2, 2 );
   $i = 0;
   foreach $fnct ( @fncts )
   {
 $test = &{"$fnct"}( $tmp[$i], $tmp2[$i] );
    	
        if ( !ok ( defined $test, $fnct ) )
           { push ( @failed, $fnct ); }
	logger ( $test, $fnct, $tmp[$i]." & $tmp2[$i++]" );
   }

   @fncts = ( "SHoundGetFasta", "SHoundGetFastaFromRedundantGroupID" );
   @tmp = ( 230779, 11 );
   $i = 0;
   foreach $fnct ( @fncts )
   {
	 $test = &{"$fnct"}( $tmp[$i] );
        if ( !ok ( defined $test, $fnct ) )
           { push ( @failed, $fnct ); }
	logger ( $test, $fnct, $tmp[$i++]);
   }

   @fncts = ( "SHoundGiFromReferenceList", "SHoundGetReferenceIDFromGiList");
   @tmp = ( [97002444,85190481], [6322016,21071030] );
   @tmp2 = ( 2, 2 );
   $i = 0;
   foreach $fnct ( @fncts ) 
   {
	 $test = &{"$fnct"}( $tmp2[$i] , @{$tmp[$i]} );
    	
        if ( !ok ( defined $test, $fnct ) )
           { push ( @failed, $fnct ); }
	logger ( $test, $fnct, Array2Str($tmp[$i])." & $tmp2[$i++]" );
   }

   @fncts = ( "SHoundGetFastaList" );
   @tmp = ( [230779,6322015] );
   $i = 0;
   foreach $fnct ( @fncts )
   {
	 $test = &{"$fnct"}( @{$tmp[$i]} );
    	
        if ( !ok ( defined $test, $fnct ) )
           { push ( @failed, $fnct ); }
	logger ( $test, $fnct, Array2Str($tmp[$i++]));
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
 	$test = &{"$fnct"}($tmp);
    	
        if ( !ok ( defined $test, $fnct ) ) 
        { 
            push ( @failed, $fnct ); 
        }
	logger ($test, $fnct, $tmp);
     }

     @fncts = ( "SHoundGetGenBankff" );
     @tmp = ( 4557225 );
     $i = 0;
     foreach $fnct ( @fncts ) 
     {
         $test = &{"$fnct"}($tmp[$i]);
         if ( !ok (defined $test, $fnct ) )
         {
	     push ( @failed, $fnct );
         }
        
        logger ( $test, $fnct, $tmp[$i++] );
     }

     @fncts = ( "SHoundGetGenBankffList" );
     @tmp = ( [4557225,230779,876787] );
     $i = 0;
     foreach $fnct ( @fncts ) 
     {
         $test = &{"$fnct"}( @{$tmp[$i]} );
         if ( !ok ( defined $test, $fnct ) )
         { 
	     push (@failed, $fnct );
	 }
	 logger ( $test, $fnct, Array2Str($tmp[$i++]) );
     }
     
	 
     @fncts = (
                "SHoundSeqIdFromGiList", "SHoundGetBioseqSetList",
                "SHoundGetSeqEntryList", "SHoundGetBioseqList"
              );
     foreach $fnct ( @fncts )
     {
         @list = ($fnct eq "SHoundGetBioseqSetList") ? (2791836,93,84) : (230779,876787);
         $test = &{"$fnct"}( @list );
    	
         if ( !ok ( defined $test, $fnct) )
         { 
	     push ( @failed, $fnct );
	 }
         logger ($test, $fnct, Array2Str(\@list));
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
	 $test = &{"$fnct"}( $tmp[$i] );
    	
        if ( !ok ( defined $test , $fnct ) )
           { push ( @failed, $fnct ); }
	logger ( $test, $fnct, $tmp[$i++] );
   }

   @fncts = ( "SHoundGiFromLLIDList", "SHoundGOIDFromGiList",
              "SHoundLLIDFromGiList", "SHoundGiFromGOIDList");
   @tmp = ( [9,1,2], [4501841,4557225], [4501843,4557225], [8033,4867,6419] );
   $i = 0;
   foreach $fnct ( @fncts ) 
   {
	 $test = &{"$fnct"}( @{$tmp[$i]} );
    	
        if ( !ok ( defined $test , $fnct ) )
           { push ( @failed, $fnct ); }
	logger ( $test, $fnct, Array2Str($tmp[$i++]) );
   }

   @fncts = ( "SHoundLLIDFromGOIDAndECode", "SHoundGiFromGOIDAndECode",
              "SHoundGOECodeFromGiAndGOID", "SHoundGOPMIDFromGiAndGOID"
            );
   @tmp = ( 6810, 5554, 4557225, 21071030 );
   @tmp2 = ( "IEA", "ND", 17114, 5554 );
   $i = 0;
   foreach $fnct ( @fncts ) 
   {
	$test = &{"$fnct"}( $tmp[$i], $tmp2[$i] );
        if ( !ok ( defined $test , $fnct ) )
           { push ( @failed, $fnct ); }
	logger ( $test, $fnct, $tmp[$i]." & $tmp2[$i++]" );
   }
 
   $fnct = "SHoundGiFromGOIDListAndECode";
   @list = (8320,4);
   $tmp2 = "NR";
   $test = &{"$fnct"}( $tmp2, @list );
   
   if ( !ok ( defined $test, $fnct) )
      { push ( @failed, $fnct ); }
   logger ($test, $fnct, @list." & $tmp2");
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
        $test = &{"$fnct"}($tmp[$i]);
    	if ( !ok ( defined $test, $fnct ) )
        {
	    push ( @failed, "$fnct" ); 
	}
	logger ($test, $fnct, $tmp[$i++]);
    }

    @fncts = ( "SHoundGiFromCDDIDList", "SHoundCDDIDFromGiList" );
    @tmp = ( 
             ["pfam00202", "pfam01835"],
             [4501841,4557225]
           );
    $i = 0;
    foreach $fnct ( @fncts )
    {
	$test = &{"$fnct"}( @{$tmp[$i]} );
    	if ( !ok ( defined $test, $fnct ) )
        {
	    push ( @failed, "$fnct" ); 
	}
	logger ( $test, $fnct, Array2Str($tmp[$i++] ));
    }

    @fncts = ( "SHoundGetGisByDomainIdAndEvalue", "SHoundGetDomainsFromGiWithEvalue",
			   "SHoundCDDScoreFromGi" );
    @tmp = ( "smart00038", 6322015, 34870013 );
    @tmp2 = ( 1.0, 1.0, "COG0606" );
    $i = 0;
    foreach $fnct ( @fncts ) 
    {
	$test = &{"$fnct"}( $tmp[$i], $tmp2[$i] );
    	if ( !ok ( defined $test, $fnct ) )
        { 
	    push ( @failed, "$fnct" ); 
	}
	logger ( $test, $fnct, $tmp[$i]." & $tmp2[$i++]" );
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
        $test = &{"$fnct"}( $tmp[$i] );
    	if ( !ok ( defined $test, $fnct ) )
        { 
	    push ( @failed, $fnct ); 
	} 
	logger ( $test, $fnct, $tmp[$i++] );
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
       $test = &{"$fnct"}( $tmp[$i] );
       if ( !ok ( defined $test, $fnct ) )
       { 
           push ( @failed, $fnct ); 
       }
       logger ( $test, $fnct, $tmp[$i++] );
   }

   @fncts = ( "SHoundRedundantGroupIDFromGIList", "SHoundRedundantGroupFromIDList",
              "SHoundFirstOfRedundantGroupFromIDList", "SHoundGOIDFromRedundantGiList",
	      "SHoundRedundantGroupList" );
   @tmp = ( [32,6017926], [362477, 408917],
            [362477,408917],[4557225,21071030],[230779,32,4586891] );
   $i = 0;
   foreach $fnct ( @fncts )
   {
       $test = &{"$fnct"}( @{$tmp[$i]} );
       if ( !ok ( defined $test, $fnct ) )
       { 
           push ( @failed, $fnct ); 
       }
       logger ( $test, $fnct, Array2Str($tmp[$i++]));
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
	$test = &{"$fnct"}( $tmp );
    	if ( !ok ( defined $test, $fnct ) )
        { 
	    push ( @failed, $fnct ); 
	}
	logger ( $test, $fnct, $tmp );
    }

    @fncts = ( "SHoundGiFrom3D", "SHoundGiFromPDBchain" );
    $tmp2 = "FALSE";
    $tmp = 11;
    foreach $fnct ( @fncts )
    {
        $tmp = ( $fnct eq "SHoundGiFromPDBchain" ) ? "9XIM" : 11;
        $tmp2 = ( $fnct eq "SHoundGiFromPDBchain" ) ? "A" : "FALSE";
	$test = &{"$fnct"}( $tmp, $tmp2 );
    	if ( !ok ( defined $test, $fnct ) )
        { 
	    push ( @failed, $fnct ); 
	}
        logger ( $test, $fnct, $tmp." & $tmp2" );
    }

    $fnct = "SHoundGiFrom3DList";
    @list  = (11,23,32);
    $tmp2 = "FALSE";
    $test = &{"$fnct"}($tmp2, @list);
    if ( !ok ( defined $test, $fnct ) )
    { 
         push ( @failed, $fnct ); 
    }
    logger ($test, $fnct, Array2Str(\@list)." & $tmp2");
	
    $fnct = "SHound3DFromGiList";
    @list  = (442560,442572);
    $test = &{"$fnct"}(@list);
    if ( !ok ( defined $test, $fnct ) )
    { 
        push ( @failed, $fnct );
    } 
    logger ($test, $fnct, Array2Str(\@list));
	
    $fnct = "SHound3DListFromTaxID";
    $tmp  = "56636";
    $test = &{"$fnct"}($tmp);
    if ( !ok ( defined $test, $fnct ) )
    { 
        push ( @failed, $fnct ); 
    }
    logger ($test, $fnct, $tmp);
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
               "SHoundIsTaxDeleted", "SHoundGetTaxNameFromTaxID",
	       "SHoundGetNewTaxIDFromMergedTaxID", "SHoundDNAFromUnknownChromosome",
	       "SHoundProteinsFromUnknownChromosome", "SHoundRNAFromChromosome",
	       "SHoundRNAFromUnknownChromosome"
             );
    @tmp   = ( 230779, 11, 56636, 3055, 10665, 3055,
               10, 139, 10, 9606, 9606, 56636,
               56636, 56636, 56636, 9606, 395, 155864,
	       155864, 169, 155864 );
    $i = 0;
    foreach $fnct ( @fncts )
    {
	$test = &{"$fnct"}($tmp[$i]);
    	if ( !ok ( defined $test, $fnct ) )
        { 
	    push ( @failed, $fnct ); 
	}
	logger ($test, $fnct, $tmp[$i++]);
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
	$test = &{"$fnct"}(@{$tmp[$i]});
    	if ( !ok ( defined $test, $fnct ) )
        {  
	    push ( @failed, $fnct ); 
	}
	logger ($test, $fnct, Array2Str($tmp[$i++]));
    }

    @fncts = ( "SHoundProteinsFromOrganismList", "SHoundChromosomeFromGenomeList",
               "SHoundDNAFromOrganismList" );
    @tmp = ( [9606,56636], [6239,562,2166],
             [9606,6239] );
    @tmp2 = ( 1, 1, 1 );
    $i = 0;
    foreach $fnct ( @fncts )
    {
        $test = &{"$fnct"}( $tmp2[$i], @{$tmp[$i]} );
    	if ( !ok ( defined $test, $fnct ) )
        { 
	    push ( @failed, $fnct ); 
	}
	logger ($test, $fnct, Array2Str($tmp[$i])." & $tmp2[$i++]");
    }

    @fncts = ( "SHoundProteinsFromOrganism", "SHoundChromosomeFromGenome",
               "SHoundDNAFromOrganism", "SHoundRNAFromOrganism",
	       "SHoundGetTaxNameFromTaxIDByClass", "SHoundGetTaxLineageFromTaxID" );
    @tmp2 = ( 9606,6239,6239, 9606, 9606, 9606, 9606 );
    @tmp = ( 255, 255, 255, 255, "SLRI_taxon_name_name_class_scientific_name",
             "SHoundGetTaxLineage_FULL", "SHoundGetTaxLineage_FULL" );
    $i = 0;
    foreach $fnct ( @fncts )
    {
	$test = &{"$fnct"}( $tmp2[$i], $tmp[$i] );
        if ( !ok ( defined $test, $fnct ) )
        { 
	    push ( @failed, $fnct ); 
	}
	logger ($test, $fnct, $tmp2[$i]." & $tmp[$i++]");
    }
   
    @fncts =("SHoundAllGenomesWithScientificNames", "SHoundAllGenomesWithNumProteins");
    foreach $fnct (@fncts)
    {
         $test = &{"$fnct"}();
    	 if ( !ok ( defined $test, $fnct ) )
         { 
	     push ( @failed, $fnct ); 
	 }
         logger ($test, $fnct, "");
    }
}

sub OMIMFunctions
{
    @fncts = ( "SHoundGiFromOMIM", "SHoundOMIMFromGi" );
    @tmp = ( 137150, 4502501 );
    $i = 0;
    foreach $fnct ( @fncts )
    {
        $test = &{"$fnct"}( $tmp[$i] );
        if ( !ok ( defined $test, $fnct ) )
        { 
	    push ( @failed, $fnct ); 
	}
	logger ( $test, $fnct, $tmp[$i++] );
    }

    @fncts = ( "SHoundGiFromOMIMList", "SHoundOMIMFromGiList" );
    @tmp = ( [137150,103950,108345], [4502501,4501843] );
    $i = 0;
    foreach  $fnct ( @fncts )
    {
	$test = &{"$fnct"}( @{$tmp[$i]} );
    	if ( !ok ( defined $test, $fnct ) )
        { 
	    push ( @failed, $fnct ); 
	}
	logger ( $test, $fnct, Array2Str($tmp[$i++]) );
    }
}

sub NeighboursFunctions
{
   @fncts = ( "SHoundNeighboursFromGi", "SHoundNeighboursFromTaxID",
              "SHound3DNeighboursFromGi", "SHound3DNeighboursFromTaxID" );
   @tmp = ( 494933, 107806, 494933, 107806);
   @tmp2 = ( 0.5, 1, 1.0, 1.0 );
   $i = 0;
   foreach $fnct ( @fncts ) 
   {
	$test = &{"$fnct"}($tmp[$i], $tmp2[$i], "TRUE");
        if ( !ok ( defined $test, $fnct ) )
        { 
	    push ( @failed, $fnct ); 
	}
	logger ($test, $fnct, $tmp[$i]." & $tmp2[$i++]");
   }	

   @fncts = ( "SHoundNeighboursFromGiList", "SHound3DNeighboursFromGiList" );
   @tmp = ( [4557225,4501843], [4557225,6009729] );
   @tmp2 = ( 0.5, 1.0 );
   $i = 0;
   foreach $fnct ( @fncts ) 
   {
	$test = &{"$fnct"}( $tmp2[$i], @{$tmp[$i]} );
    	if ( !ok ( defined $test, $fnct ) )
        { 
	    push ( @failed, $fnct ); 
	}
	logger ($test, $fnct, Array2Str($tmp[$i])." & $tmp2[$i++]");
   }
}

sub UninitializationFunctions
{
   @fncts = ( "SHoundFini" );
   foreach $fnct ( @fncts )
   { 
	$test = &{"$fnct"}();
    	if ( !ok ( defined $test, $fnct ) )
        { 
	    push (@failed, $fnct); 
	}
	logger ($test, $fnct, "");
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
	my %hash = &{"$fnct"}( @{$tmp[$i]} );
        if ( !ok ( keys(%hash) > 0, $fnct ) )
        { 
	    push ( @failed, $fnct ); 
	}
        #use hash size to determine if return is OK
        $size =  keys(%hash);
        logger ($size, $fnct, Array2Str($tmp[$i++]));
    }
}	

sub gbIDConversionFunctions(){
     my $gb_service = new genbankIDConversionService();
     
     @fncts = ("SHoundGiFromGBAcc", "SHoundGBAccFromGi");
     @tmp = ("X88853", 4557225);
     $i =0;
     foreach $fnct (@fncts)
     {
         my $result = $gb_service -> $fnct ($tmp[$i]) ;
	 if (! ok (defined $result, $fnct) ){
	     push (@failed, $fnct);
	 }
	 logger ($result, $fnct, $tmp[$i++]);
     }
     
     @fncts = ("SHoundDbNameAndIdListFromGBAcc", "SHoundDbNameAndIdListFromGi");
     @tmp = ("X88853", 555);
     $i =0;
     foreach $fnct (@fncts)
     {
         my $result = $gb_service -> $fnct ($tmp[$i]) ;
	 if (! ok (defined $result->[0], $fnct) ){
	     push (@failed, $fnct);
	 }
	 logger ($result->[0], $fnct, $tmp[$i++]);
     }
     
     @fncts = ("SHoundGBAccFromGiKeyedList", "SHoundGiFromGBAccKeyedList",
     	       "SHoundDbNameAndIdListFromGBAccKeyedList", "SHoundDbNameAndIdListFromGiKeyedList");
     @tmp = ([555,666], ['X88853', 'X66994'], ['X88853', 'X66994'], [555,666]);
     $i =0;
     foreach $fnct (@fncts)
     {
         my $result = $gb_service -> $fnct (\@{$tmp[$i]}) ;
	 if (! ok (defined $result->[0], $fnct) ){
	     push (@failed, $fnct);
	 }
	 logger ($result->[0], $fnct, Array2Str($tmp[$i++]));
     }
     
     @fncts = ("SHoundGBAccFromDbNameAndId", "SHoundGiFromDbNameAndId");
     $tmp = "embl";
     $tmp2 = "ABIPDAGEN";
     my $tmp3 = undef;
     foreach $fnct (@fncts)
     {
         my $result = $gb_service -> $fnct ($tmp, $tmp2, $tmp3) ;
	 if (! ok (defined $result, $fnct) ){
	     push (@failed, $fnct);
	 }
	 logger ($result, $fnct, $tmp." & ".$tmp2." & ".$tmp3);
     }
     
}


sub dbxrefConversionFunctions() {
    my $dbxref_service = new dbxrefService();
    
    @fncts = ("SHoundDxFieldNameListFromDbName", "SHoundDxListFromGi"); 
    @tmp = ('GENE', 131869);
    $i =0;
    foreach $fnct (@fncts)
    {
         my $result = $dbxref_service -> $fnct ($tmp[$i]) ;
	 if (! ok (defined $result->[0], $fnct) ){
	     push (@failed, $fnct);
	 }
	 logger ($result->[0], $fnct, $tmp[$i++]);
    }
    
    @fncts = ("SHoundDxListFromSource");
    @tmp = ('GENE', 1246501, 'Col6', undef);
    foreach $fnct (@fncts)
    {
         my $result = $dbxref_service -> $fnct ($tmp[0],$tmp[1], $tmp[2], $tmp[3]) ;
	 if (! ok (defined $result->[0], $fnct) ){
	     push (@failed, $fnct);
	 }
	 logger ($result->[0], $fnct, Array2Str(\@tmp));
    }

    @fncts = ("SHoundSourceListFromDx");
    @tmp = ('FB', 'FBgn0013277', undef, undef);
    foreach $fnct (@fncts)
    {
         my $result = $dbxref_service -> $fnct ($tmp[0],$tmp[1], $tmp[2], $tmp[3]) ;
	 if (! ok (defined $result->[0], $fnct) ){
	     push (@failed, $fnct);
	 }
	 logger ($result->[0], $fnct, Array2Str(\@tmp));
    }
    
    @fncts = ("SHoundGetDxDbnames");
    $tmp="TRUE";
    my $tmp2="FALSE";
    foreach $fnct (@fncts)
    {
         my $result = $dbxref_service -> $fnct ($tmp, $tmp2) ;
	 if (! ok (defined $result->[0], $fnct) ){
	     push (@failed, $fnct);
	 }
	 logger ($result->[0], $fnct, $tmp." & ".$tmp2);
    }

    @fncts = ("SHoundGetDxCvTermList");
    foreach $fnct (@fncts)
    {
         my $result = $dbxref_service -> $fnct () ;
	 if (! ok (defined $result->[0], $fnct) ){
	     push (@failed, $fnct);
	 }
	 logger ($result->[0], $fnct, $tmp[$i++]);
    }
}

#convert an array into a string, for log file use
sub Array2Str
{
   my $argArr = shift;
   
   my $retStr="";
   
   foreach my $i (@$argArr){
   
      $retStr = $retStr . $i . ","; 
   }
   
   chop ($retStr);
   
   return $retStr;
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
   gbIDConversionFunctions();
   dbxrefConversionFunctions();
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
   print "See perl_test.log logged info.\n"; 
   print "\n######################################\n";
   print "\n";
