# $Id: SeqHound.pm,v 1.45 2005/04/26 17:55:44 ryao Exp $ 
package SeqHound;

use strict;
use LWP::UserAgent;
use vars qw($VERSION @ISA @EXPORT $AUTOLOAD %EXPORT_TAGS);
use Exporter;
use POSIX qw(strftime);

@ISA = qw(Exporter);
%EXPORT_TAGS = ('all' => [@EXPORT]);
$VERSION = '4.0';


# Items to export into the callers space for SeqHound cgi call

@EXPORT = qw(SHoundInit SHoundIsNetEntrezOn SHoundNetEntrezInit SHoundFini
SHoundIsInited SHoundFindAcc SHoundFindAccList SHoundAccFromGi
SHoundAccFromGiList SHoundSeqIdFromGi SHoundSeqIdFromGiList SHoundIsProtein SHoundMoleculeType
SHoundIsNRFirst SHoundExists SHound3DbWhat SHound3DExists SHoundGetBioseq 
SHoundGetGenBankff SHoundGetBioseqList SHoundGetSeqEntry SHoundGetSeqEntryList
SHoundGetDefline SHoundGetDeflineList SHoundGetFasta
SHoundGetFastaList SHoundGet3D SHoundGet3DfromPdbId SHoundGiFrom3D 
SHoundGiFrom3DList SHound3DFromGi SHound3DFromGiList SHoundDNAFromProtein 
SHoundDNAFromProteinList SHoundProteinFromDNA SHoundProteinFromDNAList 
SHoundGetReferenceIDFromGi SHoundMuidFromGi SHoundMuidFromGiList SHoundMuidFrom3D SHoundMuidFrom3DList 
SHoundTaxIDFromGi SHoundTaxIDFromGiList SHoundTaxIDFrom3D 
SHoundTaxIDFrom3DList SHoundProteinsFromTaxID SHoundProteinsFromTaxIDIII
SHoundProteinsFromTaxIDList SHoundDNAFromTaxID SHoundDNAFromTaxIDIII 
SHoundDNAFromTaxIDList SHoundProteinsFromOrganism 
SHoundProteinsFromOrganismList SHoundProteinsFromChromosome
SHoundProteinsFromChromosomeList SHoundAllGenomes SHoundChromosomeFromGenome
SHoundChromosomeFromGenomeList SHoundDNAFromOrganism SHoundDNAFromOrganismList
SHoundProteinsFromUnknownChromosome SHoundDNAFromUnknownChromosome 
SHoundRNAFromUnknownChromosome SHoundRNAFromOrganism
SHoundDNAFromChromosome SHoundDNAFromChromosomeList 
SHoundRNAFromChromosome SHoundRedundantGroup 
SHoundGetGenBankffList SHoundRedundantGroupList SHoundRedundantGroupIDFromGI 
SHoundRedundantGroupIDFromGIList SHoundRedundantGroupFromID 
SHoundRedundantGroupFromIDList SHoundFirstOfRedundantGroupFromID
SHoundFirstOfRedundantGroupFromIDList SHoundGetTaxChildNodes 
SHoundGetTaxChildNodesList SHoundGetAllTaxProgeny SHoundGetAllTaxParent 
SHoundGetAllTaxAncestors SHoundIsTaxMerged SHoundIsTaxDeleted
SHoundGetTaxNameFromTaxIDByClass SHoundGetTaxNameFromTaxID 
SHoundGiFromPDBchain SHoundFindName SHoundFindNameList SHoundXMLBioseq 
SHoundGetXMLSeqEntry SHoundGetTaxParent
SHoundGetXML3D SHoundGetPDB3D SHoundGetBioseqSet SHoundGetBioseqSetList
SHoundGiFromGOID SHoundGiFromGOIDList SHoundGOIDFromGi SHoundGOIDFromGiList SHoundGOECodeFromGiAndGOID
SHoundGOPMIDFromGiAndGOID SHoundGOIDFromRedundantGi SHoundGOIDFromRedundantGiList
SHoundGiFromCDDID SHoundGiFromOMIM SHoundGiFromOMIMList
SHoundOMIMFromGi SHoundOMIMFromGiList SHoundGiFromCDDIDList SHoundCDDIDFromGi
SHoundCDDIDFromGiList SHoundCDDScoreFromGi SHoundGiFromLLID
SHoundGiFromLLIDList SHoundLLIDFromGi SHoundLLIDFromGiList
SHoundLocusFromGi SHoundGODBGetRecordByReference SHoundGODBGetChildrenOf
SHoundGODBGetParentOf SHoundGODBGetAllAncestors SHoundGODBGetAllChildren SHoundGODBGetNameByID
SHoundGODBGetClassification SHoundNeighboursFromGi SHoundNeighboursFromGiList SHoundNeighboursFromTaxID
SHound3DNeighboursFromGi SHound3DNeighboursFromGiList SHound3DNeighboursFromTaxID
SHound3DListFromTaxID SHoundGetGisByNumberOfDomains SHoundGetDomainIdFromLabel SHoundGetGisByDomainId
SHoundGetDomainLabelFromDomainId SHoundGetGisByDomainIdAndEvalue SHoundGetDomainsFromGi
SHoundGetDomainsFromGiWithEvalue SHoundGiFromReferenceID SHoundGetAccFromRetiredGi SHoundGiFromGOIDAndECode
SHoundSequenceLength SHoundGiFromReferenceList 
SHoundGiFromGOIDListAndECode SHoundGetReferenceIDFromGiList SHoundGetLargestMMDBID SHoundGetNameByGi
SHoundGetXMLBioseq SHoundGetTaxLineageFromTaxID SHoundGODBGetHierarchyLevel
SHoundGetFastaKeyedList SHoundRedundantGroupKeyedList SHoundGetFastaFromRedundantGroupID 
SHoundGetFastaFromRedundantGroupIDKeyedList SHoundGOIDFromLLID  SHoundOMIMFromLLID  SHoundCDDIDFromLLID
SHoundLLIDFromGOIDAndECode  SHoundLLIDFromOMIM  SHoundLLIDFromCDDID
SHoundAllGenomesWithScientificNames SHoundAllGenomesWithNumProteins  SHoundTitleFromGi
SHoundGetNewTaxIDFromMergedTaxID
);


my $server = "http://seqhound.blueprint.org";
my $path   = "/cgi-bin/seqrem";

#for list functions
#separator between query-return pair
my $Q_R = "#~#";
#separator between groups of pair 
my $GROUP = "#;#";

#a flag to indicate if log file is needed
#you can set $logset to "FALSE" if you don't need a log
my $logset = "TRUE";

# before calling SHoundInit, $isInited is false,
# SHoundInit sets $isInited as appropriate
my $isInited = "FALSE";

# types used in subroutine SHoundGetTaxNameFromTaxID
# see subroutine below for details

my $SHoundGetTaxLineage_FULL       = 10;      # gets full lineage
my $SHoundGetTaxLineage_GENBANK    = 20;      # gets lineage as it appears in GenBank record
my $SHoundGetTaxLineage_TAXDBSHORT = 30;      # gets lineage as it would appear
                                              # on the taxonomy database home page

# nameClass used in subroutine SHoundGetTaxNameFromTacIDByClass
# see subroutine below for details

my $SLRI_taxon_name_name_class_none                  = 0;
my $SLRI_taxon_name_name_class_acronym               = 1;
my $SLRI_taxon_name_name_class_anamorph              = 2;
my $SLRI_taxon_name_name_class_blast_name            = 3;
my $SLRI_taxon_name_name_class_common_name           = 4;
my $SLRI_taxon_name_name_class_equivalent_name       = 5;
my $SLRI_taxon_name_name_class_includes              = 6;
my $SLRI_taxon_name_name_class_in_part               = 7;
my $SLRI_taxon_name_name_class_misnomer              = 8;
my $SLRI_taxon_name_name_class_misspelling           = 9;
my $SLRI_taxon_name_name_class_preferred_acronym     = 10;
my $SLRI_taxon_name_name_class_preferred_common_name = 11;
my $SLRI_taxon_name_name_class_scientific_name       = 12;
my $SLRI_taxon_name_name_class_synonym               = 13;
my $SLRI_taxon_name_name_class_teleomorph            = 14;
my $SLRI_taxon_name_name_class_other                 = 255;


sub SHoundInit
{
   my ($NetEntrezOnToo, $appname) = @_;

   my $arguments = "NetEntrezOnToo\=$NetEntrezOnToo"."\&appname\=$appname";

   my $fulladdress = &HTTPFunctionCall("SeqHoundInit", $arguments);

   my $content = get($fulladdress);

   $isInited = ProcessContent($content, "SHoundInit");

   return $isInited;
}

sub SHoundIsNetEntrezOn
{
    my $fulladdress = &HTTPFunctionCall("SeqHoundIsNetEntrezOn", "NULL");

    my $content = get($fulladdress);

    ProcessContent($content, "SHoundIsNetEntrezOn");
}

sub SHoundNetEntrezInit
{
   my ($appname) = @_;

   my $arguments = "appname\=$appname";

   my $fulladdress = &HTTPFunctionCall("SeqHoundNetEntrezInit", $arguments);

   my $content = get($fulladdress);

   ProcessContent($content, "SHoundNetEntrezInit");
}

sub SHoundFini
{
    my $fulladdress = &HTTPFunctionCall("SeqHoundFini", "NULL");

    my $content = get($fulladdress);

    ProcessContent($content, "SHoundFini");
}

sub SHoundIsInited
{
    return $isInited;
}

sub SHoundFindAcc
{
   my ($pcAcc) = @_;
   
   my $arguments = "acc\=$pcAcc";

   my $fulladdress = &HTTPFunctionCall("SeqHoundFindAcc", $arguments);

   my $content = get($fulladdress);

   ProcessContent($content, "SHoundFindAcc", @_);
}

#input: $argArr an array 
#return: return a copy array of the elements in $argArr with Max query size
#        meanwhile, $argArr shifted
sub MaxSizeQuery
{
   my $argArr = shift;
  
   my @copyArr;
   
   my $MAXQUERY = 100;
   
   my $argArrLen = scalar(@$argArr);
   
   for (my $i=0; $i< $argArrLen;)
   {
      $copyArr[$i++] = $$argArr[0];
      
      shift(@$argArr);
      
      if ($i == $MAXQUERY)
      {
        last;
      }
      
    }
    
   return @copyArr;
}

#input: $argArr an array
#return: a comma delimited string represented the input array
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

#input: $result and $tmp
#return: $result appended by comma and $tmp
sub MergeValues
{
   my ($result, $tmp) = @_;
   
   if (defined $tmp ) {
	 
	if ($tmp ne "n/a"){
	
	    return $result . $tmp . ",";
		
	}
	
   }
   return $result;
   
}


sub SHoundFindAccList
{
   my @pvnAcc = @_;
   
   my @copyArr = @pvnAcc;
   
   my $result = "";

   while ($#copyArr != -1)
   {   
      my @tmpArr = MaxSizeQuery(\@copyArr);
      
      my $str = Array2Str(\@tmpArr);

      my $arguments = "pacc\=$str";

      my $fulladdress = &HTTPFunctionCall("SeqHoundFindAccList", $arguments);

      my $content = get($fulladdress);
      
      my $tmp = ProcessContent($content, "SHoundFindAccList");
      
      $result = MergeValues($result, $tmp);
      
    }
   if ($result eq ""){

       return undef;
   }
   else {
	   
       chop ($result);
    
       return  $result;
   }
}

sub SHoundFindName
{
   my ($name) = @_;

   my $arguments = "name\=$name";

   my $fulladdress = &HTTPFunctionCall("SeqHoundFindName", $arguments);

   my $content = get($fulladdress);

   ProcessContent($content, "SHoundFindName");
}

sub SHoundFindNameList
{
   my @pname = @_;
   
   my @copyArr = @pname;
   
   my $result = "";
   
   while ($#copyArr != -1)
   {
      my @tmpArr = MaxSizeQuery(\@copyArr);
      
      my $str = Array2Str(\@tmpArr);

      my $arguments = "pname\=$str";

      my $fulladdress = &HTTPFunctionCall("SeqHoundFindNameList", $arguments);

      my $content = get($fulladdress);

      my $tmp = ProcessContent($content, "SHoundFindNameList");
      
      $result = MergeValues($result, $tmp);
      
   }
   if ($result eq ""){

       return undef;
   }
   else {
	   
       chop ($result);
    
       return  $result;
   }
    
}


sub SHoundAccFromGi
{
   my ($gi) = @_;

   my $arguments = "gi\=$gi";

   my $fulladdress = &HTTPFunctionCall("SeqHoundAccFromGi", $arguments);

   my $content = get($fulladdress);

   ProcessContent($content, "SHoundAccFromGi");
}

sub SHoundAccFromGiList
{  
   my @pvnGi = @_;

   my @copyArr = @pvnGi;
   
   my $result = "";
   
   while ($#copyArr != -1)
   {
      my @tmpArr = MaxSizeQuery(\@copyArr);
 
      my $str = Array2Str(\@tmpArr);

      my $arguments = "pgi\=$str";

      my $fulladdress = &HTTPFunctionCall("SeqHoundAccFromGiList", $arguments);
      
      my $content = get($fulladdress);

      my $tmp = ProcessContent($content, "SHoundAccFromGiList");
      
      $result = MergeValues($result, $tmp);
      
   }
   if ($result eq ""){

       return undef;
   }
   else {
	   
       chop ($result);
    
       return  $result;
   }
}


sub SHoundTitleFromGi
{
   my ($gi) = @_;

   my $arguments = "gi\=$gi";

   my $fulladdress = &HTTPFunctionCall("SeqHoundTitleFromGi", $arguments);

   my $content = get($fulladdress);

   ProcessContent($content, "SHoundTileFromGi");
}


sub SHoundSeqIdFromGi
{
   my ($gi) = @_;

   my $arguments = "gi\=$gi";

   my $fulladdress = &HTTPFunctionCall("SeqHoundSeqIdFromGiPL", $arguments);

   my $content = get($fulladdress);

   ProcessContent($content, "SHoundSeqIdFromGi");
}

sub SHoundSeqIdFromGiList
{
   my @pvnGi = @_;
   
   my @copyArr = @pvnGi;
   
   my $result = "";
   
   while ($#copyArr != -1)
   {
      my @tmpArr = MaxSizeQuery(\@copyArr);
      
      my $str = Array2Str(\@tmpArr);
      
      my $arguments = "pgi\=$str";
      
      my $fulladdress = &HTTPFunctionCall("SeqHoundSeqIdFromGiListPL", $arguments);

      my $content = get($fulladdress);

      my $tmp = ProcessContent($content, "SHoundSeqIdFromGiList");
      
      $result = MergeValues($result, $tmp);
      
   }
   if ($result eq ""){

       return undef;
   }
   else {
	   
       chop ($result);
    
       return  $result;
   }

}

## SHoundMoleculeType: return to the molecule type (protein, dna, rna or null) 
## of the given gi this deprecates SHoundIsProtein
sub SHoundMoleculeType
{
   my ($Gi) = @_;
   
   my $arguments = "gi\=$Gi";
   
   my $fulladdress = &HTTPFunctionCall("SeqHoundMoleculeType", $arguments);
   
   my $content = get($fulladdress);

   ProcessContent($content, "SHoundMoleculeType");
}


sub SHoundIsProtein
{
   my ($Gi) = @_;

   my $arguments = "gi\=$Gi";

   my $fulladdress = &HTTPFunctionCall("SeqHoundIsProtein", $arguments);

   my $content = get($fulladdress);

   ProcessContent($content, "SHoundIsProtein");
}

sub SHoundIsNRFirst
{
   my ($Gi) = @_;

   my $arguments = "gi\=$Gi";

   my $fulladdress = &HTTPFunctionCall("SeqHoundIsNRFirst", $arguments);

   my $content = get($fulladdress);

   ProcessContent($content, "SHoundIsNRFirst");
}

sub SHoundExists
{
   my ($Gi) = @_;

   my $arguments = "gi\=$Gi";

   my $fulladdress = &HTTPFunctionCall("SeqHoundExists", $arguments);

   my $content = get($fulladdress);

   ProcessContent($content, "SHoundExists");
}


sub SHoundGetLargestMMDBID
{
    my $fulladdress = &HTTPFunctionCall("SeqHoundGetLargestMMDBID", "NULL");

    my $content = get($fulladdress);

    ProcessContent($content, "SHoundGetLargestMMDBID");
}


sub SHound3DbWhat
{
   my ($mmdbid) = @_;

   my $arguments = "mmdbid\=$mmdbid";

   my $fulladdress = &HTTPFunctionCall("SeqHound3DbWhat", $arguments);

   my $content = get($fulladdress);

   ProcessContent($content, "SHound3DbWhat");
}

sub SHound3DExists
{
   my ($mmdbid) = @_;

   my $arguments = "mmdbid\=$mmdbid";

   my $fulladdress = &HTTPFunctionCall("SeqHound3DExists", $arguments);

   my $content = get($fulladdress);

   ProcessContent($content, "SHound3DExists");
}

sub SHoundGetBioseq
{
   my ($Gi) = @_;

   my $arguments = "gi\=$Gi";

   my $fulladdress = &HTTPFunctionCall("SeqHoundGetBioseqPL", $arguments);

   my $content = get($fulladdress);

   ProcessContent($content, "SHoundGetBioseq");
}

sub SHoundGetBioseqList
{
   my @pvnGi = @_;
   
   my @copyArr = @pvnGi;
   
   my $result = "";
   
   while ($#copyArr != -1)
   {
      my @tmpArr = MaxSizeQuery(\@copyArr);
      
      my $str = Array2Str(\@tmpArr);
      
      my $arguments = "pgi\=$str";

      my $fulladdress = &HTTPFunctionCall("SeqHoundGetBioseqListPL", $arguments);

      my $content = get($fulladdress);

      my $tmp = ProcessContent($content, "SHoundGetBioseqList");
      
      if (defined $tmp){
      
         $result = $result."\n".$tmp;
      }
   }
   
   if ($result eq ""){

       return undef;
   }

   return  $result;
}


sub SHoundXMLBioseq
{
   my ($Gi) = @_;

   SHoundGetXMLBioseq($Gi);
}


sub SHoundGetXMLBioseq
{
   my ($Gi) = @_;

   my $arguments = "gi\=$Gi";

   my $fulladdress = &HTTPFunctionCall("SeqHoundXMLBioseqPL", $arguments);

   my $content = get($fulladdress);

   ProcessContent($content, "SHoundGetXMLBioseq");
}

sub SHoundGetSeqEntry
{
   my ($Gi) = @_;

   my $arguments = "gi\=$Gi";

   my $fulladdress = &HTTPFunctionCall("SeqHoundGetSeqEntryPL", $arguments);

   my $content = get($fulladdress);

   ProcessContent($content, "SHoundGetSeqEntry");
}

sub SHoundGetSeqEntryList
{
   my @pvnGi = @_;
   
   my @copyArr = @pvnGi;
   
   my $result = "";
   
   while ($#copyArr != -1)
   {
      my @tmpArr = MaxSizeQuery(\@copyArr);
      
      my $str = Array2Str(\@tmpArr);
      
      my $arguments = "pgi\=$str";

      my $fulladdress = &HTTPFunctionCall("SeqHoundGetSeqEntryListPL", $arguments);

      my $content = get($fulladdress);

      my $tmp = ProcessContent($content, "SHoundGetSeqEntryList");
      
      if (defined $tmp){
      
      	$result = $result."\n".$tmp;
      
      }
   }
 
   if ($result eq ""){

       return undef;
   }

   return  $result;
}

sub SHoundGetXMLSeqEntry
{
   my ($Gi) = @_;

   my $arguments = "gi\=$Gi";

   my $fulladdress = &HTTPFunctionCall("SeqHoundGetXMLSeqEntryPL", $arguments);

   my $content = get($fulladdress);

   ProcessContent($content, "SHoundGetXMLSeqEntry");
}

sub SHoundGetBioseqSet
{
   my ($Gi) = @_;

   my $arguments = "gi\=$Gi";

   my $fulladdress = &HTTPFunctionCall("SeqHoundGetBioseqSetPL", $arguments);

   my $content = get($fulladdress);

   ProcessContent($content, "SHoundGetBioseqSet");
}

sub SHoundGetBioseqSetList
{
   my @pvnGi = @_;
   
   my @copyArr = @pvnGi;
   
   my $result = "";
   
   while ($#copyArr != -1)
   {
      my @tmpArr = MaxSizeQuery(\@copyArr);
      
      my $str = Array2Str(\@tmpArr);
      
      my $arguments = "pgi\=$str";

      my $fulladdress = &HTTPFunctionCall("SeqHoundGetBioseqSetListPL", $arguments);

      my $content = get($fulladdress);

      my $tmp = ProcessContent($content, "SHoundGetBioseqSetList");
      
      if (defined $tmp){
      
          $result = $result."\n".$tmp;
      }
      
   }
   
   if ($result eq ""){

       return undef;
   }

   return  $result;
}


sub SHoundGetDefline
{
   my ($Gi) = @_;

   my $arguments = "gi\=$Gi";

   my $fulladdress = &HTTPFunctionCall("SeqHoundGetDefline", $arguments);

   my $content = get($fulladdress);

   ProcessContent($content, "SHoundGetDefline", @_);
}


sub SHoundGetDeflineList
{
   my @pvnGi = @_;
   
   my @copyArr = @pvnGi;
   
   my $result = "";
   
   while ($#copyArr != -1)
   {
      my @tmpArr = MaxSizeQuery(\@copyArr);
      
      my $str = Array2Str(\@tmpArr);
      
      my $arguments = "pgi\=$str";

      my $fulladdress = &HTTPFunctionCall("SeqHoundGetDeflineList", $arguments);

      my $content = get($fulladdress);

      my $tmp = ProcessContent($content, "SHoundGetDeflineList");
      
      $result = MergeValues($result, $tmp);
      
   }
   if ($result eq ""){

       return undef;
   }
   else {
	   
       chop ($result);
    
       return  $result;
   }
}


sub SHoundGetFasta
{
   my ($Gi) = @_;

   my $arguments = "gi\=$Gi";

   my $fulladdress = &HTTPFunctionCall("SeqHoundGetFastaPL", $arguments);

   my $content = get($fulladdress);

   ProcessContent($content, "SHoundGetFasta");
}


#return a fasta structure associated with the redundant group id
sub SHoundGetFastaFromRedundantGroupID
{
   my ($gid) = @_;
   
   my $arguments = "gid\=$gid";
    
   my $fulladdress = &HTTPFunctionCall("SeqHoundGetFastaFromRedundantGroupIDPL", $arguments);
   
   my $content = get($fulladdress);
   
   ProcessContent($content, "SHoundGetFastaFromRedundantGroupID");

}


sub SHoundGetFastaList
{
   my @pvnGi = @_;
   
   my @copyArr = @pvnGi;
   
   my $result = "";

   while ($#copyArr != -1)
   {
      my @tmpArr = MaxSizeQuery(\@copyArr);
      
      my $str = Array2Str(\@tmpArr);
      
      my $arguments = "pgi\=$str";

      my $fulladdress = &HTTPFunctionCall("SeqHoundGetFastaListPL", $arguments);

      my $content = get($fulladdress);
      
      my $tmp = ProcessContent($content, "SHoundGetFastaList");
     
      if (defined $tmp){
      
          $result = $result."\n". $tmp;     
      }
   }
   
    if ($result eq ""){

       return undef;
   }

   return  $result;
}

sub SHoundGetGenBankff
{
   my ($Gi) = @_;

   my $arguments = "gi\=$Gi";

   my $fulladdress = &HTTPFunctionCall("SeqHoundGetGenBankff", $arguments);

   my $content = get($fulladdress);

   ProcessContent($content, "SHoundGetGenBankff");

}

sub SHoundGetGenBankffList
{
   my @pvnGi = @_;
   
   my @copyArr = @pvnGi;
   
   my $result = "";
   
   while ($#copyArr != -1)
   {
      my @tmpArr = MaxSizeQuery(\@copyArr);
      
      my $str = Array2Str(\@tmpArr);
      
      my $arguments = "pgi\=$str";

      my $fulladdress = &HTTPFunctionCall("SeqHoundGetGenBankffList", $arguments);

      my $content = get($fulladdress);

      my $tmp = ProcessContent($content, "SHoundGetGenBankffList");
      
      if (defined $tmp){
      
          $result = $result."\n".$tmp;
      }
   }
   
    if ($result eq ""){

       return undef;
   }

   return  $result;
}

sub SHoundGet3D
{
   my ($mmdbid) = @_;

   my $arguments = "mmdbid\=$mmdbid";

   my $fulladdress = &HTTPFunctionCall("SeqHoundGet3DPL", $arguments);

   my $content = get($fulladdress);

   ProcessContent($content, "SHoundGet3D");
}

sub SHoundGet3DfromPdbId
{
   my ($pdbid) = @_;

   my $arguments = "pdbid\=$pdbid";

   my $fulladdress = &HTTPFunctionCall("SeqHoundGet3DfromPdbIdPL", $arguments);

   my $content = get($fulladdress);

   ProcessContent($content, "SHoundGet3DfromPdbId");
}

sub SHoundGetXML3D
{
   my ($mmdbid) = @_;

   my $arguments = "mmdbid\=$mmdbid";

   my $fulladdress = &HTTPFunctionCall("SeqHoundGetXML3DPL", $arguments);

   my $content = get($fulladdress);

   ProcessContent($content, "SHoundGetXML3D");
}

sub SHoundGetPDB3D
{
   my ($mmdbid) = @_;

   my $arguments = "mmdbid\=$mmdbid";

   my $fulladdress = &HTTPFunctionCall("SeqHoundGetPDB3DPL", $arguments);

   my $content = get($fulladdress);

   ProcessContent($content, "SHoundGetPDB3D");
}


sub SHoundGiFrom3D
{
   my ($mmdbid, $boolProteinOnly) = @_;

   my $arguments = "mmdbid\=$mmdbid"."\&ProteinOnly\=$boolProteinOnly";

   my $fulladdress = &HTTPFunctionCall("SeqHoundGiFrom3D", $arguments);

   my $content = get($fulladdress);

   ProcessContent($content, "SHoundGiFrom3D");
}

sub SHoundGiFrom3DList
{
   my ($boolProteinOnly, @pvnMmdbid) = @_;
   
   my @copyArr = @pvnMmdbid;
  
   my $result = "";
   
   while ($#copyArr != -1)
   {
      my @tmpArr = MaxSizeQuery(\@copyArr);
      
      my $str = Array2Str(\@tmpArr);
      
      my $arguments = "pmmdbid\=$str"."\&ProteinOnly\=$boolProteinOnly";

      my $fulladdress = &HTTPFunctionCall("SeqHoundGiFrom3DList", $arguments);

      my $content = get($fulladdress);
      
      my $tmp = ProcessContent($content, "SHoundGiFrom3DList");
      
      $result = MergeValues($result, $tmp);
      
   }
   if ($result eq ""){

       return undef;
   }
   else {
   
       chop ($result);
   
       return  $result;
   }
}

sub SHoundGiFromPDBchain
{
   my ($pdbcode, $chain) = @_;

   my $arguments = "pdbcode\=$pdbcode"."\&chain\=$chain";

   my $fulladdress = &HTTPFunctionCall("SeqHoundGiFromPDBchain", $arguments);

   my $content = get($fulladdress);

   ProcessContent($content, "SHoundGiFromPDBchain");
}

sub SHound3DFromGi
{
   my ($Gi) = @_;

   my $arguments = "gi\=$Gi";

   my $fulladdress = &HTTPFunctionCall("SeqHound3DFromGi", $arguments);

   my $content = get($fulladdress);

   ProcessContent($content, "SHound3DFromGi");
}

sub SHound3DFromGiList
{
   my @pvnGi = @_;
   
   my @copyArr = @pvnGi;
   
   my $result = "";
   
   while ($#copyArr != -1)
   {
      my @tmpArr = MaxSizeQuery(\@copyArr);
      
      my $str = Array2Str(\@tmpArr);
      
      my $arguments = "pgi\=$str";

      my $fulladdress = &HTTPFunctionCall("SeqHound3DFromGiList", $arguments);

      my $content = get($fulladdress);

      my $tmp = ProcessContent($content, "SHound3DFromGiList");
      
      $result = MergeValues($result, $tmp);
      
   }
   if ($result eq ""){

       return undef;
   }
   else {
   
       chop ($result);
   
       return  $result;
   }
}

sub SHoundDNAFromProtein
{
   my ($Gi) = @_;

   my $arguments = "gi\=$Gi";

   my $fulladdress = &HTTPFunctionCall("SeqHoundDNAFromProtein", $arguments);

   my $content = get($fulladdress);

   ProcessContent($content, "SHoundDNAFromProtein");
}

sub SHoundDNAFromProteinList
{
   my @pvnGi = @_;
   
   my @copyArr = @pvnGi;
   
   my $result = "";
   
   while ($#copyArr != -1)
   {
      my @tmpArr = MaxSizeQuery(\@copyArr);
      
      my $str = Array2Str(\@tmpArr);
      
      my $arguments = "pgi\=$str";

      my $fulladdress = &HTTPFunctionCall("SeqHoundDNAFromProteinList", $arguments);

      my $content = get($fulladdress);

      my $tmp = ProcessContent($content, "SHoundDNAFromProteinList");
      
      $result = MergeValues($result, $tmp);
      
   }
   if ($result eq ""){

       return undef;
   }
   else {
   
       chop ($result);
   
       return  $result;
   }
}

sub SHoundProteinFromDNA
{
   my ($Gi) = @_;

   my $arguments = "gi\=$Gi";

   my $fulladdress = &HTTPFunctionCall("SeqHoundProteinFromDNA", $arguments);

   my $content = get($fulladdress);

   ProcessContent($content, "SHoundProteinFromDNA");
}

sub SHoundProteinFromDNAList
{
   my @pvnGi = @_;
   
   my @copyArr = @pvnGi;
   
   my $result = "";
   
   while ($#copyArr != -1)
   {
      my @tmpArr = MaxSizeQuery(\@copyArr);
      
      my $str = Array2Str(\@tmpArr);
      
      my $arguments = "pgi\=$str";

      my $fulladdress = &HTTPFunctionCall("SeqHoundProteinFromDNAList", $arguments);

      my $content = get($fulladdress);
      
      my $tmp = ProcessContent($content, "SHoundProteinFromDNAList");
      
      $result = MergeValues($result, $tmp);
      
   }
   if ($result eq ""){

       return undef;
   }
   else {
   
       chop ($result);
   
       return  $result;
   }
}


#SHoundGetReferenceIDFromGi: retrieves the pmid or muid given a gi and reference type
#if want PubMed ID, set 'ReferenceType'==1, if want Medline ID, set 'ReferenceType'==2
sub SHoundGetReferenceIDFromGi
{
 
   my ($Gi, $ReferenceType) = @_;

   my $arguments = "gi\=$Gi"."\&referencetype\=$ReferenceType";

   my $fulladdress = &HTTPFunctionCall("SeqHoundGetReferenceIDFromGi", $arguments);

   my $content = get($fulladdress);

   ProcessContent($content, "SHoundGetReferenceIDFromGi");
}


sub SHoundMuidFromGi
{
   my ($Gi) = @_;

   my $arguments = "gi\=$Gi";

   my $fulladdress = &HTTPFunctionCall("SeqHoundMuidFromGi", $arguments);

   my $content = get($fulladdress);

   ProcessContent($content, "SHoundMuidFromGi");
}

sub SHoundMuidFromGiList
{
   my @pvnGi = @_;
   
   my @copyArr = @pvnGi;
   
   my $result = "";
   
   while ($#copyArr != -1)
   {
      my @tmpArr = MaxSizeQuery(\@copyArr);
      
      my $str = Array2Str(\@tmpArr);
      
      my $arguments = "pgi\=$str";

      my $fulladdress = &HTTPFunctionCall("SeqHoundMuidFromGiList", $arguments);

      my $content = get($fulladdress);

      my $tmp = ProcessContent($content, "SHoundMuidFromGiList");
      
      $result = MergeValues($result, $tmp);
      
   }
   if ($result eq ""){

       return undef;
   }
   else {
   
       chop ($result);
   
       return  $result;
   }
}

sub SHoundMuidFrom3D
{
   my ($mmdbid) = @_;

   my $arguments = "mmdbid\=$mmdbid";

   my $fulladdress = &HTTPFunctionCall("SeqHoundMuidFrom3D", $arguments);

   my $content = get($fulladdress);

   ProcessContent($content, "SHoundMuidFrom3D");
}

sub SHoundMuidFrom3DList
{
   my @pmmdbid = @_;
   
   my @copyArr = @pmmdbid;
   
   my $result = "";
   
   while ($#copyArr != -1)
   {
      my @tmpArr = MaxSizeQuery(\@copyArr);
      
      my $str = Array2Str(\@tmpArr);
      
      my $arguments = "pmmdbid\=$str";

      my $fulladdress = &HTTPFunctionCall("SeqHoundMuidFrom3DList", $arguments);

      my $content = get($fulladdress);

      my $tmp = ProcessContent($content, "SHoundMuidFrom3DList");
      
      $result = MergeValues($result, $tmp);
      
   }
   if ($result eq ""){

       return undef;
   }
   else {
   
       chop ($result);
   
       return  $result;
   }
}

sub SHoundTaxIDFromGi
{
   my ($Gi) = @_;

   my $arguments = "gi\=$Gi";

   my $fulladdress = &HTTPFunctionCall("SeqHoundTaxIDFromGi", $arguments);

   my $content = get($fulladdress);

   ProcessContent($content, "SHoundTaxIDFromGi");
}

sub SHoundTaxIDFromGiList
{
   my @pvnGi = @_;
   
   my @copyArr = @pvnGi;
   
   my $result = "";
   
   while ($#copyArr != -1)
   {
      my @tmpArr = MaxSizeQuery(\@copyArr);
      
      my $str = Array2Str(\@tmpArr);
      
      my $arguments = "pgi\=$str";

      my $fulladdress = &HTTPFunctionCall("SeqHoundTaxIDFromGiList", $arguments);

      my $content = get($fulladdress);

      my $tmp = ProcessContent($content, "SHoundTaxIDFromGiList");
      
      $result = MergeValues($result, $tmp);
      
   }
   if ($result eq ""){

       return undef;
   }
   else {
   
       chop ($result);
   
       return  $result;
   }
}

sub SHoundTaxIDFrom3D
{
   my ($mmdbid) = @_;

   my $arguments = "mmdbid\=$mmdbid";

   my $fulladdress = &HTTPFunctionCall("SeqHoundTaxIDFrom3D", $arguments);

   my $content = get($fulladdress);

   ProcessContent($content, "SHoundTaxIDFrom3D");
}

sub SHoundTaxIDFrom3DList
{
   my @pvnMmdbid = @_;
   
   my @copyArr = @pvnMmdbid;
   
   my $result = "";
   
   while ($#copyArr != -1)
   {
      my @tmpArr = MaxSizeQuery(\@copyArr);
      
      my $str = Array2Str(\@tmpArr);
      
      my $arguments = "pmmdbid\=$str";

      my $fulladdress = &HTTPFunctionCall("SeqHoundTaxIDFrom3DList", $arguments);

      my $content = get($fulladdress);

      my $tmp = ProcessContent($content, "SHoundTaxIDFrom3DList");
      
      $result = MergeValues($result, $tmp);
      
   }
   if ($result eq ""){

       return undef;
   }
   else {
   
       chop ($result);
   
       return  $result;
   }
}

sub SHoundProteinsFromTaxID
{
   my ($taxid) = @_;

   my $arguments = "taxid\=$taxid";

   my $fulladdress = &HTTPFunctionCall("SeqHoundProteinsFromTaxID", $arguments);

   my $content = get($fulladdress);

   ProcessContent($content, "SHoundProteinsFromTaxID");
}

sub SHoundProteinsFromTaxIDIII
{
   my ($taxid) = @_;

   my $arguments = "taxid\=$taxid";

   my $fulladdress = &HTTPFunctionCall("SeqHoundProteinsFromTaxIDIII", $arguments);

   my $content = get($fulladdress);

   ProcessContent($content, "SHoundProteinsFromTaxIDIII");
}

sub SHoundProteinsFromTaxIDList
{
   my @pvntaxid = @_;
   
   my @copyArr = @pvntaxid;
   
   my $result = "";
   
   while ($#copyArr != -1)
   {
      my @tmpArr = MaxSizeQuery(\@copyArr);
      
      my $str = Array2Str(\@tmpArr);
      
      my $arguments = "ptaxid\=$str";

      my $fulladdress = &HTTPFunctionCall("SeqHoundProteinsFromTaxIDList", $arguments);

      my $content = get($fulladdress);
      
      my $tmp = ProcessContent($content, "SHoundProteinsFromTaxIDList");
      
      $result = MergeValues($result, $tmp);
      
   }
   if ($result eq ""){

       return undef;
   }
   else {
   
       chop ($result);
   
       return  $result;
   }
}

sub SHoundDNAFromTaxID
{
   my ($taxid) = @_;

   my $arguments = "taxid\=$taxid";

   my $fulladdress = &HTTPFunctionCall("SeqHoundDNAFromTaxID", $arguments);

   my $content = get($fulladdress);

   ProcessContent($content, "SHoundDNAFromTaxID");
}

sub SHoundDNAFromTaxIDIII
{
   my ($taxid) = @_;

   my $arguments = "taxid\=$taxid";

   my $fulladdress = &HTTPFunctionCall("SeqHoundDNAFromTaxIDIII", $arguments);

   my $content = get($fulladdress);

   ProcessContent($content, "SHoundDNAFromTaxIDIII");
}

sub SHoundDNAFromTaxIDList
{
   my @pvntaxid = @_;
   
   my @copyArr = @pvntaxid;
   
   my $result = "";
   
   while ($#copyArr != -1)
   {
      my @tmpArr = MaxSizeQuery(\@copyArr);
      
      my $str = Array2Str(\@tmpArr);
      
      my $arguments = "ptaxid\=$str";

      my $fulladdress = &HTTPFunctionCall("SeqHoundDNAFromTaxIDList", $arguments);

      my $content = get($fulladdress);

      my $tmp = ProcessContent($content, "SHoundDNAFromTaxIDList");
      
      $result = MergeValues($result, $tmp);
      
   }
   if ($result eq ""){

       return undef;
   }
   else {
   
       chop ($result);
   
       return  $result;
   }
}

sub SHoundProteinsFromOrganism
{
   my ($taxid, $chromflags) = @_;

   my $arguments = "taxid\=$taxid"."\&chromflags\=$chromflags";

   my $fulladdress = &HTTPFunctionCall("SeqHoundProteinsFromOrganism", $arguments);

   my $content = get($fulladdress);

   ProcessContent($content, "SHoundProteinsFromOrganism");
}

sub SHoundProteinsFromOrganismList
{
   my ($chromflags, @pvnTax) = @_;
   
   my @copyArr = @pvnTax;
   
   my $result = "";
   
   while ($#copyArr != -1)
   {
      my @tmpArr = MaxSizeQuery(\@copyArr);
      
      my $str = Array2Str(\@tmpArr);
      
      my $arguments = "ptaxid\=$str"."\&chromflags\=$chromflags";

      my $fulladdress = &HTTPFunctionCall("SeqHoundProteinsFromOrganismList", $arguments);

      my $content = get($fulladdress);

      my $tmp = ProcessContent($content, "SHoundProteinsFromOrganismList");
      
      $result = MergeValues($result, $tmp);
      
   }
   if ($result eq ""){

       return undef;
   }
   else {
   
       chop ($result);
   
       return  $result;
   }
}

sub SHoundProteinsFromChromosome
{
   my ($chromid) = @_;

   my $arguments = "chromid\=$chromid";

   my $fulladdress = &HTTPFunctionCall("SeqHoundProteinsFromChromosome", $arguments);

   my $content = get($fulladdress);

   ProcessContent($content, "SHoundProteinsFromChromosome");
}

sub SHoundProteinsFromChromosomeList
{
   my @pvnChrom = @_;
   
   my @copyArr = @pvnChrom;
   
   my $result = "";
   
   while ($#copyArr != -1)
   {
      my @tmpArr = MaxSizeQuery(\@copyArr);
      
      my $str = Array2Str(\@tmpArr);
      
      my $arguments = "pchromid\=$str";

      my $fulladdress = &HTTPFunctionCall("SeqHoundProteinsFromChromosomeList", $arguments);

      my $content = get($fulladdress);

      my $tmp = ProcessContent($content, "SHoundProteinsFromChromosomeList");
      
      $result = MergeValues($result, $tmp);
      
   }
   if ($result eq ""){

       return undef;
   }
   else {
   
       chop ($result);
   
       return  $result;
   }
}

sub SHoundAllGenomes
{

   my $fulladdress = &HTTPFunctionCall("SeqHoundAllGenomes", "NULL");

   my $content = get($fulladdress);

   ProcessContent($content, "SHoundAllGenomes");
}

sub SHoundChromosomeFromGenome
{
   my ($taxid, $chromflags) = @_;

   my $arguments = "taxid\=$taxid"."\&chromflags\=$chromflags";

   my $fulladdress = &HTTPFunctionCall("SeqHoundChromosomeFromGenome", $arguments);

   my $content = get($fulladdress);

   ProcessContent($content, "SHoundChromosomeFromGenome");
}

sub SHoundChromosomeFromGenomeList
{
   my ($chromflags, @pvnGenom) = @_;
   
   my @copyArr = @pvnGenom;
   
   my $result = "";
   
   while ($#copyArr != -1)
   {
      my @tmpArr = MaxSizeQuery(\@copyArr);
      
      my $str = Array2Str(\@tmpArr);
      
      my $arguments = "ptaxid\=$str"."\&chromflags\=$chromflags";

      my $fulladdress = &HTTPFunctionCall("SeqHoundChromosomeFromGenomeList", $arguments);

      my $content = get($fulladdress);

      my $tmp = ProcessContent($content, "SHoundChromosomeFromGenomeList");
      
      $result = MergeValues($result, $tmp);
      
   }
   if ($result eq ""){

       return undef;
   }
   else {
   
       chop ($result);
   
       return  $result;
   }
}

sub SHoundDNAFromOrganism
{
   my ($taxid, $chromflags) = @_;

   my $arguments = "taxid\=$taxid"."\&chromflags\=$chromflags";

   my $fulladdress = &HTTPFunctionCall("SeqHoundDNAFromOrganism", $arguments);

   my $content = get($fulladdress);

   ProcessContent($content, "SHoundDNAFromOrganism");
}

sub SHoundDNAFromOrganismList
{
   my ($chromflags, @pvnTax) = @_;
   
   my @copyArr = @pvnTax;
    
   my $result = "";
   
   while ($#copyArr != -1)
   {
      my @tmpArr = MaxSizeQuery(\@copyArr);
      
      my $str = Array2Str(\@tmpArr);
      
      my $arguments = "ptaxid\=$str"."\&chromflags\=$chromflags";

      my $fulladdress = &HTTPFunctionCall("SeqHoundDNAFromOrganismList", $arguments);

      my $content = get($fulladdress);

      my $tmp = ProcessContent($content, "SHoundDNAFromOrganismList");
      
      $result = MergeValues($result, $tmp);
      
   }
   if ($result eq ""){

       return undef;
   }
   else {
   
       chop ($result);
   
       return  $result;
   }
}

sub SHoundRNAFromOrganism
{
   my ($taxid, $chromflags) = @_;

   my $arguments = "taxid\=$taxid"."\&chromflags\=$chromflags";

   my $fulladdress = &HTTPFunctionCall("SeqHoundRNAFromOrganism", $arguments);

   my $content = get($fulladdress);

   ProcessContent($content, "SHoundRNAFromOrganism");
}

sub SHoundDNAFromChromosome
{
   my ($chromid) = @_;

   my $arguments = "chromid\=$chromid";

   my $fulladdress = &HTTPFunctionCall("SeqHoundDNAFromChromosome", $arguments);

   my $content = get($fulladdress);

   ProcessContent($content, "SHoundDNAFromChromosome");
}

sub SHoundDNAFromChromosomeList
{
   my @pvnChrom = @_;
   
   my @copyArr = @pvnChrom;
   
   my $result = "";
   
   while ($#copyArr != -1)
   {
      my @tmpArr = MaxSizeQuery(\@copyArr);
      
      my $str = Array2Str(\@tmpArr);
      
      my $arguments = "pchromid\=$str";

      my $fulladdress = &HTTPFunctionCall("SeqHoundDNAFromChromosomeList", $arguments);

      my $content = get($fulladdress);

      my $tmp = ProcessContent($content, "SHoundDNAFromChromosomeList");
      
      $result = MergeValues($result, $tmp);
      
   }
   if ($result eq ""){

       return undef;
   }
   else {
   
       chop ($result);
   
       return  $result;
   }
}

sub SHoundRNAFromChromosome
{
   my ($chromid) = @_;

   my $arguments = "chromid\=$chromid";

   my $fulladdress = &HTTPFunctionCall("SeqHoundRNAFromChromosome", $arguments);

   my $content = get($fulladdress);

   ProcessContent($content, "SHoundRNAFromChromosome");
}

sub SHoundDNAFromUnknownChromosome
{
   my ($taxid) = @_;

   my $arguments = "taxid\=$taxid";

   my $fulladdress = &HTTPFunctionCall("SeqHoundDNAFromUnknownChromosome", $arguments);

   my $content = get($fulladdress);

   ProcessContent($content, "SHoundDNAFromUnknownChromosome");
}

sub SHoundProteinsFromUnknownChromosome
{
   my ($taxid) = @_;

   my $arguments = "taxid\=$taxid";

   my $fulladdress = &HTTPFunctionCall("SeqHoundProteinsFromUnknownChromosome", $arguments);

   my $content = get($fulladdress);

   ProcessContent($content, "SHoundProteinsFromUnknownChromosome");
}

sub SHoundRNAFromUnknownChromosome
{
   my ($taxid) = @_;

   my $arguments = "taxid\=$taxid";

   my $fulladdress = &HTTPFunctionCall("SeqHoundRNAFromUnknownChromosome", $arguments);

   my $content = get($fulladdress);

   ProcessContent($content, "SHoundRNAFromUnknownChromosome");
}

sub SHoundRedundantGroup
{
   my ($Gi) = @_;

   my $arguments = "gi\=$Gi";

   my $fulladdress = &HTTPFunctionCall("SeqHoundRedundantGroup", $arguments);

   my $content = get($fulladdress);

   ProcessContent($content, "SHoundRedundantGroup");
}

sub SHoundRedundantGroupList
{
   my @pvnGi = @_;
   
   my @copyArr = @pvnGi;
   
   my $result = "";
   
   while ($#copyArr != -1)
   {
      my @tmpArr = MaxSizeQuery(\@copyArr);
      
      my $str = Array2Str(\@tmpArr);
      
      my $arguments = "pgi\=$str";

      my $fulladdress = &HTTPFunctionCall("SeqHoundRedundantGroupList", $arguments);

      my $content = get($fulladdress);

      my $tmp = ProcessContent($content, "SHoundRedundantGroupList");
      
      $result = MergeValues($result, $tmp);
      
   }
   if ($result eq ""){

       return undef;
   }
   else {
   
       chop ($result);
   
       return  $result;
   }
}

sub SHoundRedundantGroupIDFromGI
{
   my ($Gi) = @_;

   my $arguments = "gi\=$Gi";

   my $fulladdress = &HTTPFunctionCall("SeqHoundRedundantGroupIDFromGI", $arguments);

   my $content = get($fulladdress);

   ProcessContent($content, "SHoundRedundantGroupIDFromGI");
}

sub SHoundRedundantGroupIDFromGIList
{
   my @pvngroup = @_;
   
   my @copyArr = @pvngroup;
   
   my $result = "";
   
   while ($#copyArr != -1)
   {
      my @tmpArr = MaxSizeQuery(\@copyArr);
     
      my $str = Array2Str(\@tmpArr);
      
      my $arguments = "pgi\=$str";

      my $fulladdress = &HTTPFunctionCall("SeqHoundRedundantGroupIDFromGIList", $arguments);

      my $content = get($fulladdress);

      my $tmp = ProcessContent($content, "SHoundRedundantGroupIDFromGIList");
      
      $result = MergeValues($result, $tmp);
      
   }
   if ($result eq ""){

       return undef;
   }
   else {
   
       chop ($result);
   
       return  $result;
   }
}

sub SHoundRedundantGroupFromID
{
   my ($group) = @_;

   my $arguments = "group\=$group";

   my $fulladdress = &HTTPFunctionCall("SeqHoundRedundantGroupFromID", $arguments);

   my $content = get($fulladdress);

   ProcessContent($content, "SHoundRedundantGroupFromID");
}

sub SHoundRedundantGroupFromIDList
{
   my @pvngroup = @_;
   
   my @copyArr = @pvngroup;
   
   my $result = "";
   
   while ($#copyArr != -1)
   {
      my @tmpArr = MaxSizeQuery(\@copyArr);
      
      my $str = Array2Str(\@tmpArr);
      
      my $arguments = "pgroup\=$str";

      my $fulladdress = &HTTPFunctionCall("SeqHoundRedundantGroupFromIDList", $arguments);

      my $content = get($fulladdress);

      my $tmp = ProcessContent($content, "SHoundRedundantGroupFromIDList");
      
      $result = MergeValues($result, $tmp);
      
   }
   if ($result eq ""){

       return undef;
   }
   else {
   
       chop ($result);
   
       return  $result;
   }
}

sub SHoundFirstOfRedundantGroupFromID
{
   my ($group) = @_;

   my $arguments = "group\=$group";

   my $fulladdress = &HTTPFunctionCall("SeqHoundFirstOfRedundantGroupFromID", $arguments);

   my $content = get($fulladdress);

   ProcessContent($content, "SHoundFirstOfRedundantGroupFromID");
}

sub SHoundFirstOfRedundantGroupFromIDList
{
   my @pvngroup = @_;
   
   my @copyArr = @pvngroup;
   
   my $result = "";
   
   while ($#copyArr != -1)
   {
      my @tmpArr = MaxSizeQuery(\@copyArr);
      
      my $str = Array2Str(\@tmpArr);
      
      my $arguments = "pgroup\=$str";

      my $fulladdress = &HTTPFunctionCall("SeqHoundFirstOfRedundantGroupFromIDList", $arguments);

      my $content = get($fulladdress);

      my $tmp = ProcessContent($content, "SHoundFirstOfRedundantGroupFromIDList");
      
      $result = MergeValues($result, $tmp);
      
   }
   if ($result eq ""){

       return undef;
   }
   else {
   
       chop ($result);
   
       return  $result;
   }
}

sub SHoundGetTaxChildNodes
{
   my ($taxId) = @_;

   my $arguments = "taxid\=$taxId";

   my $fulladdress = &HTTPFunctionCall("SeqHoundGetTaxChildNodes", $arguments);

   my $content = get($fulladdress);

   ProcessContent($content, "SHoundGetTaxChildNodes");
}

sub SHoundGetTaxChildNodesList
{
   my @taxIdList = @_;
   
   my @copyArr = @taxIdList;
   
   my $result = "";
   
   while ($#copyArr != -1)
   {
      my @tmpArr = MaxSizeQuery(\@copyArr);
      
      my $str = Array2Str(\@tmpArr);
      
      my $arguments = "ptaxid\=$str";

      my $fulladdress = &HTTPFunctionCall("SeqHoundGetTaxChildNodesList", $arguments);

      my $content = get($fulladdress);

      my $tmp = ProcessContent($content, "SHoundGetTaxChildNodesList");
      
      $result = MergeValues($result, $tmp);
      
   }
   if ($result eq ""){

       return undef;
   }
   else {
   
       chop ($result);
   
       return  $result;
   }
}

sub SHoundGetAllTaxProgeny
{
   my ($taxId) = @_;

   my $arguments = "taxid\=$taxId";

   my $fulladdress = &HTTPFunctionCall("SeqHoundGetAllTaxProgeny", $arguments);

   my $content = get($fulladdress);

   ProcessContent($content, "SHoundGetAllTaxProgeny");
}

sub SHoundGetTaxParent
{
   my ($taxId) = @_;

   my $arguments = "taxid\=$taxId";

   my $fulladdress = &HTTPFunctionCall("SeqHoundGetTaxParent", $arguments);

   my $content = get($fulladdress);

   ProcessContent($content, "SHoundGetTaxParent");
}

sub SHoundGetAllTaxAncestors
{
   my ($taxId) = @_;

   my $arguments = "taxid\=$taxId";

   my $fulladdress = &HTTPFunctionCall("SeqHoundGetAllTaxAncestors", $arguments);

   my $content = get($fulladdress);

   ProcessContent($content, "SHoundGetAllTaxAncestors");
}

sub SHoundIsTaxMerged
{
   my ($taxId) = @_;

   my $arguments = "taxid\=$taxId";

   my $fulladdress = &HTTPFunctionCall("SeqHoundIsTaxMerged", $arguments);

   my $content = get($fulladdress);

   ProcessContent($content, "SHoundIsTaxMerged");
}

sub SHoundIsTaxDeleted
{
   my ($taxId) = @_;

   my $arguments = "taxid\=$taxId";

   my $fulladdress = &HTTPFunctionCall("SeqHoundIsTaxDeleted", $arguments);

   my $content = get($fulladdress);

   ProcessContent($content, "SHoundIsTaxDeleted");
}

sub SHoundGetTaxNameFromTaxIDByClass
{
    my ($taxid, $nameClass) = @_;

    if ($nameClass eq "SLRI_taxon_name_name_class_none"){
	$nameClass = $SLRI_taxon_name_name_class_none;
    }
    elsif ($nameClass eq "SLRI_taxon_name_name_class_acronym"){
	$nameClass = $SLRI_taxon_name_name_class_acronym;
    }
    elsif ($nameClass eq "SLRI_taxon_name_name_class_anamorph"){
	$nameClass = $SLRI_taxon_name_name_class_anamorph;
    }
    elsif ($nameClass eq "SLRI_taxon_name_name_class_blast_name"){
	$nameClass = $SLRI_taxon_name_name_class_blast_name;
    }
    elsif ($nameClass eq "SLRI_taxon_name_name_class_common_name"){
	$nameClass = $SLRI_taxon_name_name_class_common_name;
    }
    elsif ($nameClass eq "SLRI_taxon_name_name_class_equivalent_name"){
	$nameClass = $SLRI_taxon_name_name_class_equivalent_name;
    }
    elsif ($nameClass eq "SLRI_taxon_name_name_class_includes"){
	$nameClass = $SLRI_taxon_name_name_class_includes;
    }
    elsif ($nameClass eq "SLRI_taxon_name_name_class_in_part"){
	$nameClass = $SLRI_taxon_name_name_class_in_part;
    }
    elsif ($nameClass eq "SLRI_taxon_name_name_class_misnomer"){
	$nameClass = $SLRI_taxon_name_name_class_misnomer;
    }
    elsif ($nameClass eq "SLRI_taxon_name_name_class_misspelling"){
	$nameClass = $SLRI_taxon_name_name_class_misspelling;
    }
    elsif ($nameClass eq "SLRI_taxon_name_name_class_preferred_acronym"){
	$nameClass = $SLRI_taxon_name_name_class_preferred_acronym;
    }
    elsif ($nameClass eq "SLRI_taxon_name_name_class_preferred_common_name"){
	$nameClass = $SLRI_taxon_name_name_class_preferred_common_name;
    }
    elsif ($nameClass eq "SLRI_taxon_name_name_class_scientific_name"){
	$nameClass = $SLRI_taxon_name_name_class_scientific_name;
    }
    elsif ($nameClass eq "SLRI_taxon_name_name_class_synonym"){
	$nameClass = $SLRI_taxon_name_name_class_synonym;
    }
    elsif ($nameClass eq "SLRI_taxon_name_name_class_teleomorph"){
	$nameClass = $SLRI_taxon_name_name_class_teleomorph;
    }
    elsif ($nameClass eq "SLRI_taxon_name_name_class_other"){
	$nameClass = $SLRI_taxon_name_name_class_other;
    }
    else {                          #defaults to SLRI_taxon_name_name_class_scientific_name
	$nameClass = $SLRI_taxon_name_name_class_scientific_name;
    }

    my $arguments = "taxid\=$taxid"."\&type\=$nameClass";

    my $fulladdress = &HTTPFunctionCall("SeqHoundGetTaxNameFromTaxIDByClass", $arguments);

    my $content = get($fulladdress);

    ProcessContent($content, "SHoundGetTaxNameFromTaxIDByClass");
}

sub SHoundGetTaxNameFromTaxID
{
   my ($taxId) = @_;

   my $arguments = "taxid\=$taxId";

   my $fulladdress = &HTTPFunctionCall("SeqHoundGetTaxNameFromTaxID", $arguments);

   my $content = get($fulladdress);

   ProcessContent($content, "SHoundGetTaxNameFromTaxID");
}

sub SHoundGetNewTaxIDFromMergedTaxID
{
   my ($taxId) = @_;

   my $arguments = "taxid\=$taxId";

   my $fulladdress = &HTTPFunctionCall("SeqHoundGetNewTaxIDFromMergedTaxID", $arguments);

   my $content = get($fulladdress);

   ProcessContent($content, "SHoundGetNewTaxIDFromMergedTaxID");
}   


sub SHoundGetTaxLineageFromTaxID
{
    my ($taxid, $type) = @_;

    if ($type eq "SHoundGetTaxLineage_FULL"){
	$type = $SHoundGetTaxLineage_FULL;
    }
    elsif ($type eq "SHoundGetTaxLineage_GENBANK"){
	$type = $SHoundGetTaxLineage_GENBANK;
    }
    elsif ($type eq "SHoundGetTaxLineage_TAXDBSHORT"){
	$type = $SHoundGetTaxLineage_TAXDBSHORT;
    }
    else {                                 #defaults to SHoundGetTaxLineage_FULL
	$type = $SHoundGetTaxLineage_FULL;
    }

    my $arguments = "taxid\=$taxid"."\&type\=$type";

    my $fulladdress = &HTTPFunctionCall("SeqHoundGetTaxLineageFromTaxID", $arguments);

    my $content = get($fulladdress);

    ProcessContent($content, "SHoundGetTaxLineageFromTaxID");
}


sub SHoundGOIDFromLLID
{
  my ($llid) = @_;
  
  my $arguments = "llid\=$llid";
  
  my $fulladdress = &HTTPFunctionCall("SeqHoundGOIDFromLLID", $arguments);
  
  my $content = get($fulladdress);

  ProcessContent($content, "SHoundGOIDFromLLID");
}


sub SHoundOMIMFromLLID
{
  my ($llid) = @_;
  
  my $arguments = "llid\=$llid";
  
  my $fulladdress = &HTTPFunctionCall("SeqHoundOMIMFromLLID", $arguments);
  
  my $content = get($fulladdress);

  ProcessContent($content, "SHoundOMIMFromLLID");
}


sub SHoundCDDIDFromLLID
{
  my ($llid) = @_;
  
  my $arguments = "llid\=$llid";
  
  my $fulladdress = &HTTPFunctionCall("SeqHoundCDDIDFromLLID", $arguments);
  
  my $content = get($fulladdress);

  ProcessContent($content, "SHoundCDDIDFromLLID");
}

sub SHoundLLIDFromGOIDAndECode
{
  my ($goid, $ecode) = @_;
  
  if ( ! defined ($ecode) || $ecode eq '')
  {	
	return 0;
  } 
  
  my $arguments = "goid\=$goid"."&ecode\=$ecode";
  
  my $fulladdress = &HTTPFunctionCall("SeqHoundLLIDFromGOIDAndECode", $arguments);
  
  my $content = get($fulladdress);

  ProcessContent($content, "SHoundLLIDFromGOIDAndECode");
}


sub SHoundLLIDFromOMIM
{
   my ($omimid) = @_;

   my $arguments = "omimid\=$omimid";
   my $fulladdress = &HTTPFunctionCall("SeqHoundLLIDFromOMIM", $arguments);

   my $content = get($fulladdress);

   ProcessContent($content, "SHoundLLIDFromOMIM");
}

sub SHoundLLIDFromCDDID
{
   my ($cddid) = @_;

   my $arguments = "cddid\=$cddid";

   my $fulladdress = &HTTPFunctionCall("SeqHoundLLIDFromCDDID", $arguments);

   my $content = get($fulladdress);

   ProcessContent($content, "SHoundLLIDFromCDDID");
}


sub SHoundGiFromGOID
{
   my ($goid) = @_;

   my $arguments = "goid\=$goid";

   my $fulladdress = &HTTPFunctionCall("SeqHoundGiFromGOID", $arguments);

   my $content = get($fulladdress);

   ProcessContent($content, "SHoundGiFromGOID");
}

#returns all the gi from the given goid and evidence code
sub SHoundGiFromGOIDAndECode
{
   my ($goid, $ecode) = @_;
    
   if ( ! defined ($ecode) || $ecode eq '')
   {	
	return 0;
   } 

   my $arguments = "goid\=$goid"."&ecode\=$ecode";

   my $fulladdress = &HTTPFunctionCall("SeqHoundGiFromGOIDAndECode", $arguments);

   my $content = get($fulladdress);

   ProcessContent($content, "SHoundGiFromGOIDAndECode");
}

sub SHoundGiFromGOIDList
{
   my @goidList = @_;
   
   my @copyArr = @goidList;
   
   my $result = "";
   
   while ($#copyArr != -1)
   {
      my @tmpArr = MaxSizeQuery(\@copyArr);
      
      my $str = Array2Str(\@tmpArr);
      
      my $arguments = "vnpgo\=$str";

      my $fulladdress = &HTTPFunctionCall("SeqHoundGiFromGOIDList", $arguments);

      my $content = get($fulladdress);

      my $tmp = ProcessContent($content, "SHoundGiFromGOIDList");
      
      $result = MergeValues($result, $tmp);
      
   }
   if ($result eq ""){

       return undef;
   }
   else {
   
       chop ($result);
   
       return  $result;
   }
}

#returns all the gis from the given goid list and the evidence code
sub SHoundGiFromGOIDListAndECode
{
   my ($ecode, @goidlist) = @_;
   
   if ( ! defined ($ecode) || $ecode eq '')
   {
      return 0;
   }
   
   my @copyArr = @goidlist;
 
   my $result = "";
   
   while ($#copyArr != -1)
   {
      my @tmpArr = MaxSizeQuery(\@copyArr);
      
      my $str = Array2Str(\@tmpArr);
      
      my $arguments = "golist\=$str"."&ecode\=$ecode";

      my $fulladdress = &HTTPFunctionCall("SeqHoundGiFromGOIDListAndECode", $arguments);

      my $content = get($fulladdress);

      my $tmp = ProcessContent($content, "SHoundGiFromGOIDListAndECode");
      
      $result = MergeValues($result, $tmp);
      
   }
   if ($result eq ""){

       return undef;
   }
   else {
   
       chop ($result);
   
       return  $result;
   }
}

sub SHoundGOIDFromGi
{
   my ($gi) = @_;

   my $arguments = "gi\=$gi";

   my $fulladdress = &HTTPFunctionCall("SeqHoundGOIDFromGi", $arguments);

   my $content = get($fulladdress);

   ProcessContent($content, "SHoundGOIDFromGi");
}

sub SHoundGOIDFromGiList
{
   my @giList = @_;
   
   my @copyArr = @giList;
   
   my $result = "";
   
   while ($#copyArr != -1)
   {
      my @tmpArr = MaxSizeQuery(\@copyArr);
      
      my $str = Array2Str(\@tmpArr);
      
      my $arguments = "vnpgi\=$str";

      my $fulladdress = &HTTPFunctionCall("SeqHoundGOIDFromGiList", $arguments);

      my $content = get($fulladdress);

      my $tmp = ProcessContent($content, "SHoundGOIDFromGiList");
      
      $result = MergeValues($result, $tmp);
      
   }
   if ($result eq ""){

       return undef;
   }
   else {
   
       chop ($result);
   
       return  $result;
   }
}


sub SHoundGOIDFromRedundantGi
{
   my ($gi) = @_;

   my $arguments = "gi\=$gi";

   my $fulladdress = &HTTPFunctionCall("SeqHoundGOIDFromRedundantGi", $arguments);

   my $content = get($fulladdress);

   ProcessContent($content, "SHoundGOIDFromRedundantGi");
}

sub SHoundGOIDFromRedundantGiList
{
   my @giList = @_;
   
   my @copyArr = @giList;
   
   my $result = "";
   
   while ($#copyArr != -1)
   {
      my @tmpArr = MaxSizeQuery(\@copyArr);
      
      my $str = Array2Str(\@tmpArr);
      
      my $arguments = "vnpgi\=$str";

      my $fulladdress = &HTTPFunctionCall("SeqHoundGOIDFromRedundantGiList", $arguments);

      my $content = get($fulladdress);
      
      my $tmp = ProcessContent($content, "SHoundGOIDFromRedundantGiList");
      
      $result = MergeValues($result, $tmp);
      
   }
   if ($result eq ""){

       return undef;
   }
   else {
   
       chop ($result);
   
       return  $result;
   }
}


#returns the GO Evidence Code of given gi and GoID
sub SHoundGOECodeFromGiAndGOID
{
   my ($gi, $goid) = @_;

   my $arguments = "gi\=$gi"."&goid\=$goid";

   my $fulladdress = &HTTPFunctionCall("SeqHoundGOECodeFromGiAndGOID", $arguments);

   my $content = get($fulladdress);

   ProcessContent($content, "SHoundGOECodeFromGiAndGOID");
}

#returns the GO PMID of given gi and GOID
sub SHoundGOPMIDFromGiAndGOID
{
   my ($gi, $goid) = @_;

   my $arguments = "gi\=$gi"."&goid\=$goid";

   my $fulladdress = &HTTPFunctionCall("SeqHoundGOPMIDFromGiAndGOID", $arguments);

   my $content = get($fulladdress);

   ProcessContent($content, "SHoundGOPMIDFromGiAndGOID");
}

sub SHoundGiFromCDDID
{
   my ($cddid) = @_;

   my $arguments = "cddid\=$cddid";

   my $fulladdress = &HTTPFunctionCall("SeqHoundGiFromCDDID", $arguments);

   my $content = get($fulladdress);

   ProcessContent($content, "SHoundGiFromCDDID");
}

sub SHoundGiFromOMIM
{
   my ($omimid) = @_;

   my $arguments = "omimid\=$omimid";

   my $fulladdress = &HTTPFunctionCall("SeqHoundGiFromOMIM", $arguments);

   my $content = get($fulladdress);

   ProcessContent($content, "SHoundGiFromOMIM");
}

sub SHoundGiFromOMIMList
{
   my @omimList = @_;
   
   my @copyArr = @omimList;
   
   my $result = "";
   
   while ($#copyArr != -1)
   {
      my @tmpArr = MaxSizeQuery(\@copyArr);
      
      my $str = Array2Str(\@tmpArr);
      
      my $arguments = "vnpomim\=$str";

      my $fulladdress = &HTTPFunctionCall("SeqHoundGiFromOMIMList", $arguments);

      my $content = get($fulladdress);

      my $tmp = ProcessContent($content, "SHoundGiFromOMIMList");
      
      $result = MergeValues($result, $tmp);
      
   }
   if ($result eq ""){

       return undef;
   }
   else {
   
       chop ($result);
   
       return  $result;
   }
}

sub SHoundOMIMFromGi
{
   my ($gi) = @_;

   my $arguments = "gi\=$gi";

   my $fulladdress = &HTTPFunctionCall("SeqHoundOMIMFromGi", $arguments);

   my $content = get($fulladdress);

   ProcessContent($content, "SHoundOMIMFromGi");
}

sub SHoundOMIMFromGiList
{
   my @giList = @_;
   
   my @copyArr = @giList;
   
   my $result = "";
   
   while ($#copyArr != -1)
   {
      my @tmpArr = MaxSizeQuery(\@copyArr);
      
      my $str = Array2Str(\@tmpArr);
      
      my $arguments = "vnpgi\=$str";

      my $fulladdress = &HTTPFunctionCall("SeqHoundOMIMFromGiList", $arguments);

      my $content = get($fulladdress);

      my $tmp = ProcessContent($content, "SHoundOMIMFromGiList");
      
      $result = MergeValues($result, $tmp);
      
   }
   if ($result eq ""){

       return undef;
   }
   else {
   
       chop ($result);
   
       return  $result;
   }
}

sub SHoundGiFromCDDIDList
{
   my @cddList = @_;
   
   my @copyArr = @cddList;
   
   my $result = "";
   
   while ($#copyArr != -1)
   {
      my @tmpArr = MaxSizeQuery(\@copyArr);
      
      my $str = Array2Str(\@tmpArr);
      
      my $arguments = "vnpcdd\=$str";

      my $fulladdress = &HTTPFunctionCall("SeqHoundGiFromCDDIDList", $arguments);

      my $content = get($fulladdress);

      my $tmp = ProcessContent($content, "SHoundGiFromCDDIDList");
      
      $result = MergeValues($result, $tmp);
      
   }
   if ($result eq ""){

       return undef;
   }
   else {
   
       chop ($result);
   
       return  $result;
   }
}

sub SHoundCDDIDFromGi
{
   my ($gi) = @_;

   my $arguments = "gi\=$gi";

   my $fulladdress = &HTTPFunctionCall("SeqHoundCDDIDFromGi", $arguments);

   my $content = get($fulladdress);

   ProcessContent($content, "SHoundCDDIDFromGi");
}

sub SHoundCDDIDFromGiList
{
   my @giList = @_;
   
   my @copyArr = @giList;
   
   my $result = "";
   
   while ($#copyArr != -1)
   {
      my @tmpArr = MaxSizeQuery(\@copyArr);
      
      my $str = Array2Str(\@tmpArr);
      
      my $arguments = "vnpgi\=$str";

      my $fulladdress = &HTTPFunctionCall("SeqHoundCDDIDFromGiList", $arguments);

      my $content = get($fulladdress);

      my $tmp = ProcessContent($content, "SHoundCDDIDFromGiList");
      
      $result = MergeValues($result, $tmp);
      
   }
   if ($result eq ""){

       return undef;
   }
   else {
   
       chop ($result);
   
       return  $result;
   }
}

sub SHoundCDDScoreFromGi
{
    my ($gi, $cddid) = @_;
	
    my $arguments = "gi\=$gi"."\&cddid\=$cddid";

    my $fulladdress = &HTTPFunctionCall("SeqHoundCDDScoreFromGi", $arguments);

    my $content = get($fulladdress);

    ProcessContent($content, "SHoundCDDScoreFromGi");
}

sub SHoundGiFromLLID
{
   my ($llid) = @_;

   my $arguments = "llid\=$llid";

   my $fulladdress = &HTTPFunctionCall("SeqHoundGiFromLLID", $arguments);

   my $content = get($fulladdress);

   ProcessContent($content, "SHoundGiFromLLID");
}

sub SHoundGiFromLLIDList
{
   my @llidList = @_;
   
   my @copyArr = @llidList;
   
   my $result = "";
   
   while ($#copyArr != -1)
   {
      my @tmpArr = MaxSizeQuery(\@copyArr);
      
      my $str = Array2Str(\@tmpArr);
      
      my $arguments = "vnpll\=$str";

      my $fulladdress = &HTTPFunctionCall("SeqHoundGiFromLLIDList", $arguments);

      my $content = get($fulladdress);

      my $tmp = ProcessContent($content, "SHoundGiFromLLIDList");
      
      $result = MergeValues($result, $tmp);
      
   }
   if ($result eq ""){

       return undef;
   }
   else {
   
       chop ($result);
   
       return  $result;
   }
}

sub SHoundLLIDFromGi
{
   my ($gi) = @_;

   my $arguments = "gi\=$gi";

   my $fulladdress = &HTTPFunctionCall("SeqHoundLLIDFromGi", $arguments);

   my $content = get($fulladdress);

   ProcessContent($content, "SHoundLLIDFromGi");
}

sub SHoundLLIDFromGiList
{
   my @giList = @_;
   
   my @copyArr = @giList;
   
   my $result = "";
   
   while ($#copyArr != -1)
   {
      my @tmpArr = MaxSizeQuery(\@copyArr);
      
      my $str = Array2Str(\@tmpArr);
      
      my $arguments = "vnpgi\=$str";

      my $fulladdress = &HTTPFunctionCall("SeqHoundLLIDFromGiList", $arguments);

      my $content = get($fulladdress);

      my $tmp = ProcessContent($content, "SHoundLLIDFromGiList");
      
      $result = MergeValues($result, $tmp);
      
   }
   if ($result eq ""){

       return undef;
   }
   else {
   
       chop ($result);
   
       return  $result;
   }
}

sub SHoundLocusFromGi
{
   my ($gi) = @_;

   my $arguments = "gi\=$gi";

   my $fulladdress = &HTTPFunctionCall("SeqHoundLocusFromGi", $arguments);

   my $content = get($fulladdress);

   ProcessContent($content, "SHoundLocusFromGi");
}


sub SHoundGODBGetRecordByReference
{
   my ($goid) = @_;

   my $arguments = "refType\=$goid";

   my $fulladdress = &HTTPFunctionCall("SeqHoundGODBGetRecordByReference", $arguments);

   my $content = get($fulladdress);

   ProcessContent($content, "SHoundGODBGetRecordByReference");
}

sub SHoundGODBGetChildrenOf
{
   my ($goid) = @_;

   my $arguments = "goid\=$goid";

   my $fulladdress = &HTTPFunctionCall("SeqHoundGODBGetChildrenOf", $arguments);

   my $content = get($fulladdress);

   ProcessContent($content, "SHoundGODBGetChildrenOf");
}

sub SHoundGODBGetParentOf
{
   my ($goid) = @_;

   my $arguments = "goid\=$goid";

   my $fulladdress = &HTTPFunctionCall("SeqHoundGODBGetParentOf", $arguments);

   my $content = get($fulladdress);

   ProcessContent($content, "SHoundGODBGetParentOf");
}

sub SHoundGODBGetAllAncestors
{
   my ($goid) = @_;

   my $arguments = "goid\=$goid";

   my $fulladdress = &HTTPFunctionCall("SeqHoundGODBGetAllAncestors", $arguments);

   my $content = get($fulladdress);

   ProcessContent($content, "SHoundGODBGetAllAncestorsOf");
}

sub SHoundGODBGetAllChildren
{
   my ($goid) = @_;

   my $arguments = "goid\=$goid";

   my $fulladdress = &HTTPFunctionCall("SeqHoundGODBGetAllChildren", $arguments);

   my $content = get($fulladdress);

   ProcessContent($content, "SHoundGODBGetAllChildrenOf");
}

sub SHoundGODBGetNameByID
{
   my ($goid) = @_;

   my $arguments = "goid\=$goid";

   my $fulladdress = &HTTPFunctionCall("SeqHoundGODBGetNameByID", $arguments);

   my $content = get($fulladdress);

   ProcessContent($content, "SHoundGODBGetNameByID");
}

sub SHoundGODBGetClassification
{
   my ($goid) = @_;

   my $arguments = "goid\=$goid";

   my $fulladdress = &HTTPFunctionCall("SeqHoundGODBGetClassification", $arguments);

   my $content = get($fulladdress);

   ProcessContent($content, "SHoundGODBGetClassification");
}


sub SHoundNeighboursFromGi
{
   my ($gi, $evalue, $bLimit) = @_;

   my $arguments = "gi\=$gi"."\&evalue\=$evalue"."\&bLimit\=$bLimit";

   my $fulladdress = &HTTPFunctionCall("SeqHoundNeighboursFromGiPL", $arguments);

   my $content = get($fulladdress);

   ProcessContent($content, "SHoundNeighboursFromGi");
}

sub SHoundNeighboursFromGiList
{
   my ($evalue, @giList ) = @_;
   
   my @copyArr = @giList;
   
   my $result = "";
   
   while ($#copyArr != -1)
   {
      my @tmpArr = MaxSizeQuery(\@copyArr);
      
      my $str = Array2Str(\@tmpArr);
      
      my $arguments = "gilist\=$str"."\&evalue\=$evalue";

      my $fulladdress = &HTTPFunctionCall("SeqHoundNeighboursFromGiListPL", $arguments);

      my $content = get($fulladdress);

      my $tmp = ProcessContent($content, "SHoundNeighboursFromGiListPL");
      
      $result = MergeValues($result, $tmp);
      
   }
   if ($result eq ""){

       return undef;
   }
   else {
   
       chop ($result);
   
       return  $result;
   }
}

sub SHoundNeighboursFromTaxID
{
   my ($taxid, $evalue) = @_;

   my $arguments = "taxid\=$taxid"."\&evalue\=$evalue";

   my $fulladdress = &HTTPFunctionCall("SeqHoundNeighboursFromTaxIDPL", $arguments);

   my $content = get($fulladdress);

   ProcessContent($content, "SHoundNeighboursFromTaxID");
}

sub SHound3DNeighboursFromGi
{
   my ($gi, $evalue) = @_;

   my $arguments = "gi\=$gi"."\&evalue\=$evalue";

   my $fulladdress = &HTTPFunctionCall("SeqHound3DNeighboursFromGiPL", $arguments);

   my $content = get($fulladdress);

   ProcessContent($content, "SHound3DNeighboursFromGi");
}

sub SHound3DNeighboursFromGiList
{
   my ($evalue, @giList) = @_;
   
   my @copyArr = @giList;
  
   my $result = "";
   
   while ($#copyArr != -1)
   {
      my @tmpArr = MaxSizeQuery(\@copyArr);
      
      my $str = Array2Str(\@tmpArr);
      
      my $arguments = "gilist\=$str"."\&evalue\=$evalue";

      my $fulladdress = &HTTPFunctionCall("SeqHound3DNeighboursFromGiListPL", $arguments);

      my $content = get($fulladdress);

      my $tmp = ProcessContent($content, "SHound3DNeighboursFromGiListPL");
      
      $result = MergeValues($result, $tmp);
      
   }
   if ($result eq ""){

       return undef;
   }
   else {
   
       chop ($result);
   
       return  $result;
   }
}

sub SHound3DNeighboursFromTaxID
{
   my ($taxid, $evalue) = @_;

   my $arguments = "taxid\=$taxid"."\&evalue\=$evalue";

   my $fulladdress = &HTTPFunctionCall("SeqHound3DNeighboursFromTaxIDPL", $arguments);

   my $content = get($fulladdress);

   ProcessContent($content, "SHound3DNeighboursFromTaxID");
}

sub SHound3DListFromTaxID
{
   my ($taxid) = @_;

   my $arguments = "taxid\=$taxid";

   my $fulladdress = &HTTPFunctionCall("SeqHound3DListFromTaxIDPL", $arguments);

   my $content = get($fulladdress);

   ProcessContent($content, "SHound3DListFromTaxID");
}

# More GODB Stuff

# sub SHoundGODBGetHierarchyLevel
# {
#   my ($goid) = @_;

#   my $arguments = "goid\=$goid";

#   my $fulladdress = &HTTPFunctionCall("SeqHoundAllGODBGetHierarchyLevel", $arguments);

#   my $content = get($fulladdress);

#   ProcessContent($content, "SHoundAllGODBGetHierarchyLevel");
#}


sub SHoundAllGenomesWithScientificNames
{
    my $arguments = "";

    my $fulladdress = &HTTPFunctionCall("SeqHoundAllGenomesWithScientificNamesPL", $arguments);

    my $content = get($fulladdress);

    my $tmp = ProcessContent($content, "SHoundAllGenomesWithScientificNames");
  
    if (defined $tmp) {
    
         my @list = split(/\n/, $tmp);

         return @list;
    }
    return undef;
}

sub SHoundAllGenomesWithNumProteins
{
    my $arguments = "";

    my $fulladdress = &HTTPFunctionCall("SeqHoundAllGenomesWithNumProteinsPL", $arguments);

    my $content = get($fulladdress);

    my $tmp = ProcessContent($content, "SHoundAllGenomesWithNumProteins");

    if (defined $tmp) {
    
        my @list = split(/\n/, $tmp);

	return @list;
    }
    return undef;
}

# RPS Functions

sub SHoundGetGisByNumberOfDomains
{
   my ($numdom) = @_;

   my $arguments = "numdom\=$numdom";

   my $fulladdress = &HTTPFunctionCall("SeqHoundGetGisByNumberOfDomains", $arguments);

   my $content = get($fulladdress);

   ProcessContent($content, "SHoundGetGisByNumberOfDomains");
}

sub SHoundGetDomainIdFromLabel
{
   my ($label) = @_;

   my $arguments = "label\=$label";

   my $fulladdress = &HTTPFunctionCall("SeqHoundGetDomainIdFromLabel", $arguments);

   my $content = get($fulladdress);

   ProcessContent($content, "SHoundGetDomainIdFromLabel");
}

sub SHoundGetGisByDomainId
{
   my ($id) = @_;

   my $arguments = "domain_id\=$id";

   my $fulladdress = &HTTPFunctionCall("SeqHoundGetGisByDomainId", $arguments);

   my $content = get($fulladdress);

   ProcessContent($content, "SHoundGetGisByDomainId");
}

sub SHoundGetDomainLabelFromDomainId
{
   my ($accession) = @_;
   
   my $arguments = "accession\=$accession";

   my $fulladdress = &HTTPFunctionCall("SeqHoundGetDomainLabelFromDomainId", $arguments);

   my $content = get($fulladdress);
   
   ProcessContent($content, "SHoundGetDomainLabelFromDomainId");
}

sub SHoundGetGisByDomainIdAndEvalue
{
   my ($domain, $evalue) = @_;

   my $arguments = "domain\=$domain"."\&evalue\=$evalue";

   my $fulladdress = &HTTPFunctionCall("SeqHoundGetGisByDomainIdAndEvalue", $arguments);

   my $content = get($fulladdress);

   ProcessContent($content, "SHoundGetGisByDomainIdAndEvalue");
}

sub SHoundGetDomainsFromGi
{
   my ($gi) = @_;

   my $arguments = "gi\=$gi";

   my $fulladdress = &HTTPFunctionCall("SeqHoundGetDomainsFromGiPL", $arguments);

   my $content = get($fulladdress);

   ProcessContent($content, "SHoundGetDomainsFromGi");
}

sub SHoundGetDomainsFromGiWithEvalue
{
   my ($gi, $evalue) = @_;

   my $arguments = "gi\=$gi"."\&evalue\=$evalue";

   my $fulladdress = &HTTPFunctionCall("SeqHoundGetDomainsFromGiWithEvaluePL", $arguments);

   my $content = get($fulladdress);

   ProcessContent($content, "SHoundGetDomainsFromGiWithEvalue");
}


#returns the Gi of given referenceID, if referencetype==1, referenceid refers to PubMed id
#if referencetype==2, referenceid refers to Medline id
sub SHoundGiFromReferenceID
{
   my ($referenceid, $referencetype) = @_;

   my $arguments = "referenceid\=$referenceid"."&referencetype\=$referencetype";

   my $fulladdress = &HTTPFunctionCall("SeqHoundGiFromReferenceID", $arguments);

   my $content = get($fulladdress);

   ProcessContent($content, "SHoundGiFromReferenceID");
}


#returns the Accession number of given retired gi
sub SHoundGetAccFromRetiredGi
{
   my ($gi) = @_;

   my $arguments = "gi\=$gi";

   my $fulladdress = &HTTPFunctionCall("SeqHoundGetAccFromRetiredGi", $arguments);

   my $content = get($fulladdress);

   ProcessContent($content, "SHoundGetAccFromRetiredGi");
}

#returns the sequence length of the given gi
sub SHoundSequenceLength
{
   my ($gi) = @_;

   my $arguments = "gi\=$gi";

   my $fulladdress = &HTTPFunctionCall("SeqHoundSequenceLength", $arguments);

   my $content = get($fulladdress);

   ProcessContent($content, "SHoundSequenceLength");
}


#returns all the gis from the given referenceList
#if referencelist contains PubMed id, referencetype set to '1', if referencelist contains Medline id
#referencetype set to '2'. the return gilist may contain duplicates
sub SHoundGiFromReferenceList
{
   my ($reftype, @reflist) = @_;
   
   my @copyArr = @reflist;
   
   my $result = "";
   
   while ($#copyArr != -1)
   {
      my @tmpArr = MaxSizeQuery(\@copyArr);
      
      my $str = Array2Str(\@tmpArr);
      
      my $arguments = "reflist\=$str" . "&reftype\=$reftype";

      my $fulladdress = &HTTPFunctionCall("SeqHoundGiFromReferenceList", $arguments);

      my $content = get($fulladdress);
      
      my $tmp = ProcessContent($content, "SHoundGiFromReferenceList");
      
      $result = MergeValues($result, $tmp);
      
   }
   if ($result eq ""){

       return undef;
   }
   else {
   
       chop ($result);
   
       return  $result;
   }
}


#returns all the publication referenceID for the given gilist
#if want PubMed id, referencetype set to '1', if want Medline id
#referencetype set to '2' 
sub SHoundGetReferenceIDFromGiList
{
   my ($reftype, @gilist) = @_;
   
   my @copyArr = @gilist;
  
   my $result = "";
   
   while ($#copyArr != -1)
   {
      my @tmpArr = MaxSizeQuery(\@copyArr);
      
      my $str = Array2Str(\@tmpArr);
      
      my $arguments = "gilist\=$str"."&reftype\=$reftype";

      my $fulladdress = &HTTPFunctionCall("SeqHoundGetReferenceIDFromGiList", $arguments);

      my $content = get($fulladdress);

      my $tmp = ProcessContent($content, "SHoundGetReferenceIDFromGiList");
      
      $result = MergeValues($result, $tmp);
      
   }
   if ($result eq ""){

       return undef;
   }
   else {
   
       chop ($result);
   
       return  $result;
   }
}

#returns a PDB code from the given gi
sub SHoundGetNameByGi
{
   my ($gi) = @_;

   my $arguments = "gi\=$gi";

   my $fulladdress = &HTTPFunctionCall("SeqHoundGetNameByGi", $arguments);

   my $content = get($fulladdress);

   ProcessContent($content, "SHoundGetNameByGi");
}

#return a hash table stored fasta format data associated with the input gis
sub SHoundGetFastaKeyedList
{
   my @gilist = @_;
   
   my @copyArr = @gilist;
   
   my $result = "";
   
   my %hash;
   
   while ($#copyArr != -1)
   {
      my @tmpArr = MaxSizeQuery(\@copyArr);
      
      my $str = Array2Str(\@tmpArr);
      
      my $arguments = "pgi\=$str";

      my $fulladdress = &HTTPFunctionCall("SeqHoundGetFastaKeyedList", $arguments);

      my $content = get($fulladdress);

      %hash = ProcessContentKeyedList($content, "SHoundGetFastaKeyedList", %hash);
      
   }
   return %hash;
}

#return a hash table stored Redundant gis associated with the input gis
sub SHoundRedundantGroupKeyedList
{
   my @gilist = @_;
   
   my @copyArr = @gilist;
   
   my $result = "";
   
   my %hash;
   
   while ($#copyArr != -1)
   {
      my @tmpArr = MaxSizeQuery(\@copyArr);
      
      my $str = Array2Str(\@tmpArr);
      
      my $arguments = "pgi\=$str";

      my $fulladdress = &HTTPFunctionCall("SeqHoundRedundantGroupKeyedList", $arguments);

      my $content = get($fulladdress);

      %hash = ProcessContentKeyedList($content, "SHoundRedundantGroupKeyedList", %hash);
      
   }
   return %hash;
}


#return a hash table stored Fasta strutures associated with the input redundant group ids
sub  SHoundGetFastaFromRedundantGroupIDKeyedList
{
   my @gidList = @_;
   
   my @copyArr = @gidList;
   
   my $result = "";
   
   my %hash;
   
   while ($#copyArr != -1)
   {
      my @tmpArr = MaxSizeQuery(\@copyArr);
      
      my $str = Array2Str(\@tmpArr);
      
      my $arguments = "pgid\=$str";

      my $fulladdress = &HTTPFunctionCall("SeqHoundGetFastaFromRedundantGroupIDKeyedList", $arguments);

      my $content = get($fulladdress);

      %hash = ProcessContentKeyedList($content, "SHoundGetFastaFromRedundantGroupIDKeyedList", %hash);
      
   }
   
   return %hash;
}


sub HTTPFunctionCall
{
   my ($function, $args) = @_;

   my $functioncall = "NULL";

   if($args eq "NULL"){
       $functioncall = "\?fnct\=".$function;
   }
   else
   {
       $functioncall = "\?fnct\=".$function."\&".$args;
   }
   my $address = $server.$path.$functioncall;

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

sub ProcessContent
{
   my ($lcontent, $fname, @args) = @_;
   my $result;   
   
   if ($logset eq "TRUE")
   {
        open (LOG, ">>shoundlog");
        my $now =strftime("%a %b %e %H:%M:%S %Y", localtime);
	if ($lcontent eq "")
   	{
     		print LOG "$now	$fname No reply.";
		if (@args) {
			print LOG " Arguments were: ";
			for my $i (@args) {
				print LOG "$i ";
			}
		}
		print LOG "\n";
		return undef;
   	}
	elsif ($lcontent =~ /HTTP::Request error/){
		print LOG "$now		$fname. Http::Request error.\n";
		return undef;
	}
	else
   	{
      		chomp $lcontent;
		my @lines = split(/\n/, $lcontent, 2);
            
            	#when SEQHOUND_ERROR returned from server
      		if($lines[0] =~ /ERROR/)
      		{
        		print LOG "$now	$fname error. $lines[0] ";
			if (@args) {
				print LOG " Arguments were: ";
					for my $i (@args) {
						print LOG "$i ";
					}
			}
			print LOG "\n";
			return undef; 
		}
		
		#when SEQHOUND_NULL returned from server
      		elsif ($lines[0] =~ /NULL/)
      		{
        		print LOG "$now	$fname Value not found in the database. NULL Found. ";
			if (@args) {
				print LOG " Arguments were: ";
					for my $i (@args) {
						print LOG "$i ";
					}
				}
			print LOG "\n";
			return undef;
		}
		
		#when SEQHOUND_OK returned from server
		else {
		        #in some cases, SEQHOUND_OK (null) actually is equivalent to SEQHOUND_NULL 
      			if ($lines[1] =~ /^\(null\)/)
      			{
        			print LOG "$now	$fname Value not found in the database. (null) Found. ";
				if (@args) {
					print LOG " Arguments were: ";
					for my $i (@args) {
						print LOG "$i ";
					}
				}
				print LOG "\n";
				return undef;

      			}
			#in some cases, SEQHOUND_OK    actually return a null string and is equivalent to SEQHOUND_NULL
      			elsif ($lines[1] eq "")
      			{
        			print LOG "$now	$fname Value not found in the database. empty string returned ";
				if (@args) {
					print LOG " Arguments were: ";
					for my $i (@args) {
						print LOG "$i ";
					}
				}
				print LOG "\n";
				return undef;
			}
			#in some cases, SEQHOUND_OK -1  actually is equivalent to SEQHOUND_NULL
      			elsif($lines[1] =~ /^-1$/)
         		{
           			print LOG "$now	$fname Value not found in the database. -1 Found. ";
				if (@args) {
					print LOG " Arguments were: ";
					for my $i (@args) {
						print LOG "$i ";
					}
				}
				print LOG "\n";
				return undef;

         		}
			#SEQHOUND_OK 0 actually is equivalent to SEQHOUND_ERROR
         		elsif ($lines[1]  =~ /^0$/)
         		{
           			print LOG "$now	$fname failed. ";
				if (@args) {
					print LOG " Arguments were: ";
						for my $i (@args) {
							print LOG "$i ";
						}
					}
				print LOG "\n";
				return undef;
         		}
         		else
         		{
           			$result = $lines[1];
         		}
      		}
   	}
   	close (LOG);
   }
   #don't need log set up
   else
   {
   	if ($lcontent eq "") {return undef;}
	
   	elsif ($lcontent =~ /HTTP::Request error/){ return undef;}
	
	else
   	{
      		chomp $lcontent;
      		
		my @lines = split(/\n/, $lcontent, 2);

      		if($lines[0] =~ /ERROR/ || $lines[0] =~ /NULL/)
      		{
        		return undef;
      		}
      		else
      		{
         		if ($lines[1] =~ /^-1$/ || $lines[1]  =~ /^0$/)
         		{
           			return undef;
         		}
			elsif ($lines[1] =~ /^\(null\)/ || $lines[1] eq "")
      			{
        			return undef;
      			}
         		else
         		{
           			$result = $lines[1];
         		}
      		}
   	}
   }
   return $result;
}


#################################################################
# convert a string to a hash for list function handling
# key == query, value == return
#################################################################
sub Str2Hash
{
	my ($result, %hash) = @_;
	
	my @groups = split(/$GROUP/, $result);
	
	foreach my $kv (@groups)
	{
	   my @pairs = split(/$Q_R/, $kv, 2);
	   
	   $hash{$pairs[0]} = $pairs[1];
	   
	}
	
	return %hash;
}


############################
# list function handling
############################
sub ProcessContentKeyedList
{
   my ($lcontent, $fname, %hash) = @_;
   
   if ($logset eq "TRUE")
   {

   	open (LOG, ">>shoundlog");

   	my $now =strftime("%a %b %e %H:%M:%S %Y", localtime);
   
   	if ($lcontent eq "")
   	{
     		print LOG "$now	$fname No reply.\n";
     
     		close (LOG);
     
     		return %hash;
  
   	}
   	elsif ($lcontent =~ /ERROR/)
   	{
     		print LOG "$now	$fname fails. SEQHOUND ERROR.\n";
     
     		close (LOG);
     
     		return %hash;
     
   	}
	elsif ($lcontent =~ /HTTP::Request error/){
	
		print LOG "$now		$fname. Http::Request error.\n";
		
		close (LOG);
		
		return undef;
	}
   	else
   	{
      		chomp $lcontent;
      
      		my @lines = split(/\n/, $lcontent, 2);

     		if( ($lines[0] =~ /SEQHOUND_OK/) && !($lines[1] =~ /^-1$/) && !($lines[1]  =~ /^0$/) )
      		{
           		my $return = $lines[1];
	   
	   		%hash = Str2Hash($return, %hash);
      		}
      		else
      		{
         		print LOG "$now	$fname fails.\n";
      		}
                 close (LOG);
   
                 return %hash;	 
        }
    }
    else 
    {
    	if ($lcontent eq "") {return %hash;}
   
   	elsif ($lcontent =~ /ERROR/ || $lcontent=~ /HTTP::Request error/) {return %hash;}
   	
	else
   	{
      		chomp $lcontent;
      
      		my @lines = split(/\n/, $lcontent, 2);

     		if( ($lines[0] =~ /SEQHOUND_OK/) && !($lines[1] =~ /^-1$/) && !($lines[1]  =~ /^0$/) )
      		{
           		my $return = $lines[1];
	   
	   		%hash = Str2Hash($return, %hash);
      		}
   
                return %hash;	 
        }
    
    }

}


1;

__END__



=head1 NAME

SeqHound PERL API - interfaces with SeqHound database system of biological sequences and structures


=head1 DESCRIPTION

Perl module permitting use of the SeqHound Database System 
developed by researchers at
The Blueprint Initiative
Samuel Lunenfeld Research Institute
Mount Sinai Hospital 

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

Katerina Michalickova
E<lt>seqhound@blueprint.orgE<gt>

Rong Yao
E<lt>seqhound@blueprint.orgE<gt>



=head1 SEE ALSO

README file, README_API.pod
perl(1). 
http://www.blueprint.org/seqhound/api_help/seqhound_help_guides.html

=cut

#$Log: SeqHound.pm,v $
#Revision 1.45  2005/04/26 17:55:44  ryao
#dos2unix format convert
#
#Revision 1.44  2005/04/22 19:39:24  ryao
#removed the SOAP code from SeqHound package, the release will include the SOAP packages by stub code, this will be easy to maintain.
#
#Revision 1.43  2005/04/21 16:05:36  ryao
#Integrated genbankIDConversion soap service methods
#
#Revision 1.42  2005/04/20 20:58:23  ryao
#Modified List functions to be consisted with return type; updated for release 4.0 but before SOAP service integrated.
#
#Revision 1.41  2005/03/08 21:19:39  ryao
#added SHoundGetNewTaxIDFromMergedTaxID, bug [3474]; removed the duplicated function
#
#Revision 1.40  2005/03/02 20:33:47  ryao
#changed query and return pair symbol from '~' to '#~#', since some gis might contain '~' in defline, example gi 9971716, bug [4100].  minor changes on last revision.
#
#Revision 1.39  2005/03/02 16:59:34  eburgess
#Added API calls SHoundRNAFromChromosome, SHoundRNAFromOrganism,
#and SHoundRNAFromUnknownChromosome. Bug 4914
#
#Revision 1.38  2005/02/23 17:17:41  ryao
#LWP::UserAgent replace LWP::Simple to have a better control of HTTP::Request and HTTP::Reponse objects; Added a subroutine get() to process user agent request call; Added code to manage request error; bug [4242] related. Removed SYNOPSIS; Added CVS ID
#
