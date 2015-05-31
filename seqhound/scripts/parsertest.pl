#!/usr/bin/perl

# Description : This perl script will test seqhound parsers.
#               In order to execute it, you should have the following files in the same directory:
#               	binary parsers
#                 	input files corresponding to each parser 
#                 	seqhound configuaration file
# Organization: blueprint.org
# Author:       Rong Yao
# Date:         January, 2004


open(LOG, ">>parser_test.log") || die "Cannot create parser log\n";

$PATH = "/bigdisk1/ryao/parsertest";
$NEIGHBOURS = "$PATH/neighbours";
$DATAB = "$PATH/datab";
$SENDATAB = "$PATH/sendatab";
$RPS = "$PATH/rps";
$MMDB ="$PATH/mmdb";
     
use POSIX qw(strftime);

#test core module
#requied files: gbhtg38.aso.gz, gbhtg40.aso.gz
sub test_mother 
{
$mother_flag = "FALSE";
$mother_first = "TRUE";
$version = 137;
@sample = ("gbhtg38.aso.gz", "gbhtg40.aso.gz");

foreach $i (@sample)
{
   if (system("gunzip $i")!= 0)
   {
       print LOG "check for: gunzip $i 								fail\n";
       $mother_flag = "TRUE";
   }
   $i =~s/.gz//;
   
   #if this is mother's first run
   if ($mother_first eq "TRUE")
   {
       print LOG "------\n";
       $mother_start=strftime("%a %b %e %H:%M:%S %Y", localtime);
       print LOG "'mother -i $i -r $version -n T -m T' starting at time:  $mother_start\n";
       if (system("./mother -i $i -r $version -n T -m T")!=0)
       {
           print LOG "check for: 'mother -i $i -r $version -n T -m T' 				fail.\n";
	   $mother_flag = "TRUE";
       }
       else
       {
           @corelist = ('PARTI', 'NUCPROT', 'ACCDB', 'PUBSEQ', 'TAXGI' );
           @sengi = ('SENGI'); 
	   #check if PARTI, NUCPROT, ACCDB, PUBSEQ, TAXGI, SENGI are built
           if (check_data2($DATAB, @corelist) == 0 && check_data2($SENDATAB, @sengi) == 0)
	   { 
	       print LOG "check for: PARTI, NUPROT, PUBDEQ, TAXGI, ACCDB, SENGI table				OK\n";
	   }
	   else
	   {  
	       print LOG "check for: PARTI, NUPROT, PUBDEQ, TAXGI, ACCDB, SENGI table				fail\n";
	       $mother_flag = "TRUE";
	   }
	   
	   $i=~s/.aso//; 
           @asndb =($i);
	   #check if gbhtg38.cdx .dbf .fpt are built
           if (check_data($DATAB, @asndb) != 0)
           {
               print LOG "check for: $i ASNDBs								fail\n";
	       $mother_flag = "TRUE";
           }
           else 
           {
               print LOG "check for: $i ASNDBs								OK\n";
           }
           $j=$i."s";
           @sendb =($j);
           if (check_data($SENDATAB, @sendb) != 0)
           {
               print LOG "check for: $j SENDBS								fail\n";
	       $mother_flag = "TRUE";
           }
           else
           {
               print LOG "check for: $j SENDBS								OK\n";
           }
	   
       }
   }
     
   #else after mother's first run
   else
   {
       $mother_second=strftime("%a %b %e %H:%M:%S %Y", localtime);
       print LOG "'mother -i $i -r $version -n T -m F' starting at time:  $mother_second\n";
       if(system("./mother -i $i -r $version -n T -m F") !=0 )
       {
            print LOG "check for :'mother -i $i -r $version -n T -m F'					fail\n";
	    $mother_flag = "TRUE";
       }
       $i=~s/.aso//; 
       @asndb =($i);
       if (check_data($DATAB, @asndb) != 0)
       {
            print LOG "check for: $i ASNDBs								fail\n";
	    $mother_flag = "TRUE";
       }
       
       else 
       {
            print LOG "check for: $i ASNDBs								OK\n";
       }
       $j=$i."s";
       @sendb =($j);
       if (check_data($SENDATAB, @sendb) != 0)
       {
            print LOG "check for: $j SENDBS.								fail\n";
	    $mother_flag = "TRUE";
       }
       else
       {
            print LOG "check for: $j SENDBS.								OK\n";
       }
    }
    
    if($i =~ /aso/)
    { 
       $k=$i;
    }
    else
    {
       $k=$i.".aso";
    }
    if (system("gzip $k") !=0 )
    {
        print LOG "check for: gzip $k					       				fail\n";
	$mother_flag = "TRUE"; 
     }
     
     #check each logfile
     $k =~s/.aso//;
     $logname = $k."run";
     if(system("mv $logname $PATH/logs") !=0)
     {
         print LOG "check for: move run files to directory logs		       				fail\n";
         $mother_flag = "TRUE";
     }
 
     if (check_log("$PATH/logs/$logname", "ERROR") ==0)
     {
        print LOG "check for: $logname 					       				fail\n";
	$mother_flag = "TRUE";
     }
     
     $mother_first = "FALSE";
 } 
 
 #final check mother test fail or not
 if ($mother_flag eq "TRUE")
 {
      print LOG "check for: mother test					      				fail\n";
 }
 else
 {
      print LOG "check for: mother test					      				OK.\n";
 }
 
 $mother_end = strftime("%a %b %e %H:%M:%S %Y", localtime);
 print LOG ("mother ending at $mother_end\n");
}   #end of test_mother 


#test redundb module
#required files: nr.gz
sub test_redund
{
$redund_flag = "FALSE";

if (system("gunzip nr.gz")!= 0)
{
   print LOG "check for:  gunzip nr.gz 										fail\n";
   $redund_flag = "TRUE";
}

$redund_start = strftime("%a %b %e %H:%M:%S %Y", localtime);
print LOG "\n--------";
print LOG "redund starting at $redund_start\n";
if(system("./redund -i nr -n T") !=0)
{
    print LOG "check for: ./redund -i nr -n T								fail\n";
    $redund_flag = "TRUE";
}
else
{
    print LOG "check for: ./redund -i nr -n T								OK\n";
    @redundlist = ('REDUND');
    if (check_data2($DATAB, @redundlist) != 0)
    {
        print LOG "check for: REDUND	table								fail\n";
	$redund_flag = "TRUE";
    }
    else
    {
    	print LOG "check for: REDUND	table								OK\n";
    }
    if (-e "$PATH/redundlog"){
    	print LOG "check for: presence of redundlog file							OK\n";
    }
    else{
    	print LOG "check for: presence of redundlog file							fail\n";
	$redund_flag = "TRUE";
    }
    if (check_log("$PATH/redundlog", "ERROR") !=0)
    {
        print LOG "check for: log file has no error messages						OK\n";
    }
    else
    {
         print lOG "check for: log file has no error messages.                                          fail\n";
	 $redund_flag = "TRUE";
    }
    if (check_log("$PATH/redundlog", "Done.") !=0 )
    {
        print LOG "check for: presence of 'done'								fail\n";
	$redund_flag = "TRUE"; 
    }
    else
    {
    	print LOG "check for: presence of 'done'								OK\n";
	
    }
}
if (system("gzip nr") !=0 )
{
    print LOG "check for: gzip nr					       				        fail\n";
    $redund_flag = "TRUE"; 
}
    
if ($redund_flag eq "TRUE")
{
   print LOG "check for redund test									fail\n";
}
else
{
    print LOG "check for redund test									OK\n"; 
}
$redund_end = strftime("%a %b %e %H:%M:%S %Y", localtime);
print LOG ("redund ending at $redund_end\n");
}   #end of test_redund 


#test taxdb module
#required files: taxdump.tar.gz
sub test_importtaxdb
{
$taxdb_flag = "FALSE";

if (system("gzip -d taxdump.tar.gz")!= 0)
{
   print LOG "check for:  gzip taxdump.tar.gz 								        fail\n";
   $taxdb_flag = "TRUE";
}


if (system("tar -xvf taxdump.tar")!= 0)
{
   print LOG "check for:  tar taxdump.tar. 								        fail\n";
   $taxdb_flag = "TRUE";
}

$taxdb_start = strftime("%a %b %e %H:%M:%S %Y", localtime);
print LOG "\n--------";
print LOG "importtaxdb starting at $taxdb_start\n";

if(system("./importtaxdb") !=0)
{
    print LOG "check for: ./importtaxdb								fail\n";
    $taxdb_flag = "TRUE";
}
else
{
    print LOG "check for: ./importtaxdb								OK\n";
    @taxdblist = ('DEL', 'DIV', 'GCODE', 'MERGE', 'TAX');
    if (check_data2($DATAB, @taxdblist) != 0)
    {
        print LOG "check for: DEL DIV GCODE MERGE TAX	table						fail\n";
	$taxdb_flag = "TRUE";
    }
    else
    {
    	print LOG "check for: DEL DIV GCODE MERGE TAX table						OK\n";
    }
    if (-e "$PATH/importtaxdb_log.txt")
    { 
    	print LOG "check for: presence of importtaxlog file						OK\n";
    }
    else{
    	print LOG "check for: presence of importtaxlog file						fail\n";
	$taxdb_flag = "TRUE";
    }
    if (check_log("$PATH/importtaxdb_log.txt", "ERROR") !=0)
    {
         print LOG "check for: log file has no error messages						OK\n";
    }
    else
    {
         print lOG "check for: log file has no error messages.                                          fail\n";
	 $taxdb_flag = "TRUE";
    }
    if (check_log("$PATH/importtaxdb_log.txt", "Program start at") !=0 ||
        check_log("$PATH/importtaxdb_log.txt", "Number of Tax ID records") !=0 ||
	check_log("$PATH/importtaxdb_log.txt", "Number of Tax ID Name records") !=0 ||
	check_log("$PATH/importtaxdb_log.txt", "Number of Division records") !=0 ||
	check_log("$PATH/importtaxdb_log.txt", "Number of Genetic Code records") !=0 ||
	check_log("$PATH/importtaxdb_log.txt", "Number of Deleted Node records") !=0 ||
	check_log("$PATH/importtaxdb_log.txt", "Number of Merged Node records") !=0 ||
	check_log("$PATH/importtaxdb_log.txt", "Program end at") !=0 )
    {
        print LOG "check for: presence of 'done'								fail\n";
	$taxdb_flag = "TRUE";
    }
    else
    {
    	print LOG "check for: presence of 'done'								OK\n"; 
    }
}
if (system("gzip taxdump.tar") !=0 )
{
    print LOG "check for: gzip taxdump					       				fail\n";
    $taxdb_flag = "TRUE"; 
}
    
if ($taxdb_flag eq "TRUE")
{
    print LOG "check for importtax test								fail\n";
}
else
{
    print LOG "check for importtax test								OK\n"; 
}
$taxdb_end = strftime("%a %b %e %H:%M:%S %Y", localtime);
print LOG ("importtaxdb ending at $taxdb_end\n");
}   #end of test_importtaxdb   


#test godb module
#required files: component.ontology, function.ontology, process.ontology
sub test_goparser
{
$godb_flag = "FALSE";

$godb_start = strftime("%a %b %e %H:%M:%S %Y", localtime);
print LOG "\n--------";
print LOG "goparser starting at $godb_start\n";

if(system("./goparser") !=0)
{
    print LOG "check for: ./goparser								        fail\n";
    $godb_flag = "TRUE";
}
else
{
    print LOG "check for: ./goparser								        OK\n";
    @godblist = ('GO_NAME', 'GO_PARENT', 'GO_REF', 'GO_SYN');
    if (check_data2($DATAB, @godblist) != 0)
    {
        print LOG "check for: GO_NAME, GO_PARENT, GO_REF, GO_SYN table					fail\n";
	$godb_flag = "TRUE";
    }
    else
    {
    	print LOG "check for: GO_NAME, GO_PARENT, GO_REF, GO_SYN table					OK\n";
    }
    if (-e "$PATH/goparserlog")
    {
    	 print LOG "check for: presence of goparserlog file							OK\n";
    }
    else{
    	 print LOG "check for: presence of goparserlog file							fail\n";
	 $godb_flag = "TRUE";
    }
    if (check_log("$PATH/goparserlog", "ERROR") !=0)
    {
         print LOG "check for: log file has no error messages						OK\n";
    }
    else
    {
         print lOG "check for: log file has no error messages.                                          fail\n";
	 $godb_flag = "TRUE";
    }
    if (check_log("$PATH/goparserlog", "Main: Done!") !=0 )
	
    {
         print LOG "check for: presence of 'done'								fail\n";
	 $godb_flag = "TRUE";
    }
    else
    {
    	print LOG "check for: presence of 'done'								OK\n"; 
    }
}

if ($godb_flag eq "TRUE")
{
    print LOG "check for goparser test									fail\n";
}
else
{
    print LOG "check for goparser test									OK\n"; 
}
$godb_end = strftime("%a %b %e %H:%M:%S %Y", localtime);
print LOG ("goparser ending at $godb_end\n");
}   #end of test_goparser 



#test LLDB module
#required files: LL_tmpl.gz
sub test_llparser
{
$lldb_flag = "FALSE";
if (system("gzip -d LL_tmpl.gz")!= 0)
{
   print LOG "check for:  gunzip LL_tmpl.gz 										fail\n";
   $lldb_flag = "TRUE";
}

$lldb_start = strftime("%a %b %e %H:%M:%S %Y", localtime);
print LOG "\n--------";
print LOG "llparser starting at $lldb_start\n";

if(system("./llparser") !=0)
{
    print LOG "check for: ./llparser								        fail\n";
    $lldb_flag = "TRUE";
}
else
{
    print LOG "check for: ./llparser								        OK\n";
    @lldblist = ('LL_CDD', 'LL_GO', 'LL_LLINK', 'LL_OMIM');
    if (check_data2($DATAB, @lldblist) != 0)
    {
        print LOG "check for: LL_CDD, LL_GO, LL_LLINK, LL_OMIM table					fail\n";
	$lldb_flag = "TRUE";
    }
    else
    {
    	print LOG "check for: LL_CDD, LL_GO, LL_LLINK, LL_OMIM	table					OK\n";
    }
    if (-e "$PATH/llparserlog")
    {
         print LOG "check for: presence of llparserlog file							OK\n";
    }
    else{
    	print LOG "check for: presence of llparserlog file							fail\n";
	$lldb_flag = "TRUE";
    }
    if (check_log("$PATH/llparserlog", "ERROR") !=0)
    {
        print LOG "check for: log file has no error messages						OK\n";
    }
    else
    {
        print lOG "check for: log file has no error messages.                                           fail\n";
	$lldb_flag = "TRUE";
    }
    if (check_log("$PATH/llparserlog", "Main: Done!") !=0 )
	
    {
        print LOG "check for: presence of 'done'								fail\n";
	$lldb_flag = "TRUE";
    }
    else
    {
    	print LOG "check for: presence of 'done'								OK\n"; 
    }
}

if ($lldb_flag eq "TRUE")
{
    print LOG "check for llparser test									fail\n";
}
else
{
    print LOG "check for llparser test									OK\n"; 
}

if (system("gzip LL_tmpl") !=0 )
{
    print LOG "check for: gzip LL_tmpl					       				fail\n";
    $lldb_flag = "TRUE"; 
}    

$lldb_end = strftime("%a %b %e %H:%M:%S %Y", localtime);
print LOG ("llparser ending at $lldb_end\n");
}   #end of test_llparser 


#test GENDB module
# test chrom parser
#required files: genff
sub test_chrom
{
$chromdb_flag = "FALSE";
$chromdb_start = strftime("%a %b %e %H:%M:%S %Y", localtime);
print LOG "\n--------";
print LOG "chrom parser starting at $chromdb_start\n";

if(system("./chrom -i genff -n T") !=0)
{
    print LOG "check for: ./chrom -i genff -n T								fail\n";
    $chromdb_flag = "TRUE";
}
else
{
     print LOG "check for: ./chrom -i genff -n T							OK\n";
     @chromdblist = ('CHROM');
     if (check_data2($DATAB, @chromdblist) != 0)
     {
        print LOG "check for: CHROM table									fail\n";
	$chromdb_flag = "TRUE";
    }
    else
    {
    	print LOG "check for: CHROM table									OK\n";
    }
    if (-e "$PATH/chromlog")
    {
         print LOG "check for: presence of chromlog file							OK\n";
    }
    else{
    	print LOG  "check for: presence of chromlog file							fail\n";
	$chromdb_flag = "TRUE";
    }
    if ( check_log("$PATH/chromlog", "ERROR") !=0)
    {
        print LOG "check for: log file has no error messages						OK\n";
    }
    else
    {
        print lOG "check for: log file has no error messages.                                           fail\n";
	$chromdb_flag = "TRUE";
    }
    if (check_log("$PATH/chromlog", "NOTE: [000.000] {chrom.c, line 136} Assigned TaxId") !=0 ||
        check_log("$PATH/chromlog", "NOTE: [000.000] {chrom.c, line 143} Assigned Kloodge") !=0 ||
	check_log("$PATH/chromlog", "NOTE: [000.000] {chrom.c, line 150} Assigned Chromfl") !=0 ||
	check_log("$PATH/chromlog", "NOTE: [000.000] {chrom.c, line 155} Assigned Access") !=0  ||
	check_log("$PATH/chromlog", "NOTE: [000.000] {chrom.c, line 158} Assigned Name") !=0 ||
	check_log("$PATH/chromlog", "NOTE: [000.000] {chrom.c, line 173} Done.") !=0 )
    {
        print LOG "check for: presence of 'done'								fail\n";
	$chromdb_flag = "TRUE";
    }
    else
    {
    	print LOG "check for: presence of 'done'								OK\n"; 
    }
}

if ($chromdb_flag eq "TRUE")
{
    print LOG "check for chrom parser test								fail\n";
}
else
{
    print LOG "check for chrom parser test								OK\n"; 
}

$chromdb_end = strftime("%a %b %e %H:%M:%S %Y", localtime);
print LOG ("chrom ending at $chromdb_end\n");
}   #end of test_chrom 
 

#test structdb module
#test cbmmdb parser
#make sure .intrezrc, .mmdbrc, .ncbirc are properly set
#need mmdb.idx, in my test, I include 2 files, 14036 and 5857
sub test_cbmmdb
{
$cbmmdb_flag = "FALSE";
$cbmmdb_start = strftime("%a %b %e %H:%M:%S %Y", localtime);
print LOG "\n--------";
print LOG "cbmmdb parser starting at $cbmmdb_start\n";

if(system("./cbmmdb -n T -m T") !=0)
{
    print LOG "check for: ./cbmmdb -n T -m T								fail\n";
    $cbmmdb_flag = "TRUE";
}
else
{
    print LOG "check for: ./cbmmdb -n T -m T							        OK\n";
    @mmdblist =('MMDB', 'MMGI');
    if (check_data2($MMDB, @mmdbdblist) != 0)
    {
        print LOG "check for: MMDB MMGI table					  	                fail\n";
	$cbmmdb_flag = "TRUE";
    }
    else
    {
    	print LOG "check for: MMDB MMGI	table						                OK\n";

    }
    if(-e "$PATH/cbmmdblog")
    {
     	print LOG "check for: presence of cbmmdblog file							OK\n";
    }
    else{
    	print LOG "check for: presence of cbmmdblog file							fail\n";
	$cbmmdb_flag = "TRUE";
    }
    if ( check_log("$PATH/cbmmdblog", "ERROR") !=0)
    {
        print LOG "check for: log file has no error messages.                                              OK\n";
    }
    else
    {
        print LOG "check for: log file has no error messages.                                              fail\n";
	$cbmmdb_flag = "TRUE";
    }
     
    if (check_log("$PATH/cbmmdblog", "Main: Done!") !=0 )
    {
        print LOG "check for: presence of 'done'								fail\n";
	$cbmmdb_flag = "TRUE";
    }
    else
    {
    	print LOG "check for: presence of 'done'								OK\n"; 
    }
}

if ($cbmmdb_flag eq "TRUE")
{
    print LOG "check for: cbmmdb parser test								fail\n";
}
else
{
    print LOG "check for: cbmmdb parser test								OK\n"; 
}

$cbmmdb_end = strftime("%a %b %e %H:%M:%S %Y", localtime);
print LOG ("cbmmdb ending at $cbmmdb_end\n");
}   #end of cbmmdb 


#build neighbour module, make sure nrB.tar.gz and nrN.tar.gz are in $PATH/neighbours
#required files: nrB.tar, nrN.tar in neighbours directory
sub test_neighbours
{
$neighbour_flag = "FALSE";
$neighbour_start = strftime("%a %b %e %H:%M:%S %Y", localtime);
print LOG "\n--------";
print LOG "build neighbours starting at $neighbour_start\n";

if (system ("tar -xvf $NEIGHBOURS/nrB.tar") !=0 || system ("tar -xvf $NEIGHBOURS/nrN.tar") !=0) 
{
    print LOG "check for: tar files									fail\n";
    $neighbour_flag = "TRUE";
} 
@neighbourlist = ('nrB', 'nrN');
if (check_data($PATH,@neighbourlist) !=0 )
{
    print LOG "check for :nrB, nrN datatable								fail\n";
    $neighbour_flag = "TRUE";
}
else
{
    print LOG "check for :nrB, nrN datatable								OK\n";
}
if ($neighbour_flag eq "FALSE")
{
    if (system("mv ./nrB.cdx $NEIGHBOURS/B.cdx") ==0 && system("mv ./nrN.cdx $NEIGHBOURS/N.cdx") ==0 &&
        system("mv ./nrB.dbf $NEIGHBOURS/B.dbf") ==0 && system("mv ./nrN.dbf $NEIGHBOURS/N.dbf") ==0 &&
	system("mv ./nrB.fpt $NEIGHBOURS/B.fpt") ==0 && system("mv ./nrN.fpt $NEIGHBOURS/N.fpt") ==0)
    {
    	print LOG "check for: rename files nrB.* to B.*, nrN.* to N.*					OK\n";
    }
    else
    {
        print LOG "check for: rename files nrB.* to B.*, nrN.* to N.*					fail\n"; 
	$neighbour_flag = "TRUE"; 	 
    }
}

if ($neighbour_flag eq "TRUE")
{
     print LOG "check for:neighbours module build 							fail\n";
}
else
{
     print LOG "check for:neighbours module build 							OK\n";
}
$neighbour_end = strftime("%a %b %e %H:%M:%S %Y", localtime);
print LOG ("neighbours module ending at $neighbour_end\n");
}   #end of test_neighbours


#build Rpsdb module
#required files: rps.tar in rps directory
sub test_rps
{
$rpsdb_flag = "FALSE";
$rpsdb_start = strftime("%a %b %e %H:%M:%S %Y", localtime);
print LOG "\n--------";
print LOG " build rpsdb starting at $rpsdb_start\n";

if(system ("tar -xvf $RPS/*.tar") !=0)
{
    print LOG "check for: unpack *.gz files								fail\n";
    $rpsdb_flag = "TRUE";
} 
@rpsdblist = ('DOMNAME', 'MASTER');
if (check_data2($PATH,@rpsdblist) !=0 )
{
    print LOG "check for :DOMNAME MASTER datatable							fail\n";
    $rpsdb_flag = "TRUE";
}
else
{
    print LOG "check for :DOMNAME MASTER datatable							OK\n";
    if (system ("mv $PATH/DOMNAME* $RPS") !=0 || system("mv $PATH/MASTER* $RPS") !=0)
    {
         print LOG "check  for: datafile moved to /rps							fail\n";
	 $rpsdb_flad = "TRUE";
    }
}  

if ($rpsdb_flag eq "TRUE")
{
     print LOG "check for:rpsdb module build 								fail\n";
}
else
{
     print LOG "check for:rpsdb module build 								OK\n";
}
$rpsdb_end = strftime("%a %b %e %H:%M:%S %Y", localtime);
print LOG ("rpsdb module ending at $rpsdb_end\n");
}  #end of test_rps

#test vastblst parser,
sub test_vastblst
{
$vastblst_flag = "FALSE";
$vastblst_start = strftime("%a %b %e %H:%M:%S %Y", localtime);
print LOG "\n--------";
print LOG "vastblst parser starting at $vastblst_start\n";

if(system("./vastblst -n T") !=0)
{
    print LOG "check for: ./vastblst -n T 								fail\n";
    $vastblst_flag = "TRUE";
}
else
{
    print LOG "check for: ./vastblst -n T 							       	OK\n";
    @vastblstlist =('DOMDB');
    if (check_data2($MMDB, @vastblstlist) != 0)
    {
        print LOG "check for: DOMDB table					  	                        fail\n";
	$vastblst_flag = "TRUE";
    }
    else
    {
    	print LOG "check for: DOMDB	table						                        OK\n";
    }
    if ( check_log("$PATH/vastblstlog", "ERROR") ==0)
    {
        print LOG "check for: vastblst log file								        fail\n";
	$vastblst_flag = "TRUE";
    }
    else
    {
    	print LOG "check for: vastblst log file								        OK\n"; 
    }
}

if ($vastblst_flag eq "TRUE")
{
    print LOG "check for: vastblst parser test								fail\n";
}
else
{
    print LOG "check for: vastblst parser test								OK\n"; 
}

$vastblst_end = strftime("%a %b %e %H:%M:%S %Y", localtime);
print LOG ("vastblst ending at $vastblst_end\n");
}   #end of test_vastblst  


#test pdbrep parser
#required files: nrpdb.040103
sub test_pdbrep
{
$pdbrep_flag = "FALSE";
$pdbrep_start = strftime("%a %b %e %H:%M:%S %Y", localtime);
print LOG "\n--------";
print LOG "pdbrep parser starting at $pdbrep_start\n";
if(system("./pdbrep -i nrpdb.040103") !=0)
{
    print LOG "check for: ./pdbrep -i nrpdb.040103							fail\n";
    $pdbrep_flag = "TRUE";
}
else
{
    print LOG "check for: ./pdbrep -i nrpdb.040103 							OK\n";
    if(-e "$PATH/pdbreplog")
    {
     	print LOG "check for: presence of pdbreplog file							OK\n";
    }
    else{
    	print LOG "check for: presence of pdbreplog file							fail\n";
	$pdbrep_flag = "TRUE";
    }
    if (check_log("$PATH/pdbreplog", "ERROR") !=0)
    {
       print LOG "check for: log file has no error messages.						OK\n";
      
    }
    else
    {
        print LOG "check for: log file has no error messages.						fail\n";
	$pdbrep_flag = "TRUE";
    }
    if ( check_log("$PATH/pdbreplog", "Done!") == 0)
    {
        print LOG "check for: presence of 'done'								OK\n";
    }
    else
    {
        print LOG "check for: presence of 'done'								fail\n";
	$pdbrep_flag = "TRUE";
    }
}

if ($pdbrep_flag eq "TRUE")
{
    print LOG "check for: pdbrep parser test								fail\n";
}
else
{
    print LOG "check for: pdbrep parser test								OK\n"; 
}

$pdbrep_end = strftime("%a %b %e %H:%M:%S %Y", localtime);
print LOG ("pdbrep ending at $pdbrep_end\n");
}   #end of test_pdbrep

#test HISTDB parser
sub test_histparser
{
$histdb_flag = "FALSE";
$histdb_start = strftime("%a %b %e %H:%M:%S %Y", localtime);
print LOG "\n--------";
print LOG "histparser starting at $histdb_start\n";
if (system ("./histparser -n T")!=0 )
{
    print LOG "check for: ./histparser -n T								fail\n";
    $histdb_flag = "TRUE";
}
else
{ 
    print LOG "check for: ./histparser -n T								OK\n";
}
@histdblist =('HISTDB');
if (check_data2($DATAB, @histdblist) !=0)
{
    print LOG "check for: HISTDB table									fail\n";
    $histdb_flag = "TRUE";
}
else
{
    print LOG "check for: HISTDB table									OK\n";
}
if (-e "$PATH/histparserlog")
{
    print LOG "check for: presence of histparser log file						OK\n";
}
else{
    print LOG "check for: presence of histparser log file						fail\n";
    $histdb_flag = "TRUE";
}   
if (check_log("$PATH/histparserlog", "ERROR") != 0)
{
    print LOG "check for: log file has no error message 						OK\n";
}
else
{
    print LOG "check for: log file has no error message 						fail\n";
    $histdb_flag = "TRUE";
}

if ($histdb_flag eq "TRUE")
{
     print LOG "check for:histdb build 									fail\n";
}
else
{
     print LOG "check for:histdb module build 								OK\n";
}
$histdb_end = strftime("%a %b %e %H:%M:%S %Y", localtime);
print LOG ("histparser ending at $histdb_end\n");
}   #end of test_histparser

#test addgoid parser
#required files: gene_association.Compugen_GenBank
sub test_addgoid
{
$addgoid_flag = "FALSE";
$addgoid_start = strftime("%a %b %e %H:%M:%S %Y", localtime);
print LOG "\n--------";
print LOG "addgoid starting at $addgoid_start\n";
if (system ("./addgoid -i gene_association.Compugen_GenBank")!=0 )
{
    print LOG "check for: ./addgoid -i gene_association.Compugen_GenBank				fail\n";
    $addgoid_flag = "TRUE";
}
else
{ 
    print LOG "check for: ./addgoid -i gene_association.Compugen_GenBank				OK\n";
}
#ignore run gene_association.compugen.Swissprot
if (-e "$PATH/addgoidlog")
{
    print LOG "check for: presence of addgoid log file							OK\n";
}
else{
    print LOG "check for: presence of addgoid log file							fail\n";
    $addgoid_flag = "TRUE";
}   
if (check_log("$PATH/addgoidlog", "ERROR") != 0)
{
    print LOG "check for: log file has no error message 						OK\n";
}
else
{
    print LOG "check for: log file has no error message 						fail\n";
    $addgoid_flag = "TRUE";
}
if (check_log("$PATH/addgoidlog", "Main: Done!") != 0)
{
    print LOG "check for: presence of 'done'		 						fail\n";
    $addgoid_flag = "TRUE";
}
else
{
    print LOG "check for: presence of 'done'		 						OK\n";
}
if ($addgoid_flag eq "TRUE")
{
     print LOG "check for:addgoid parser 								fail\n";
}
else
{
     print LOG "check for:addgoid parser 								OK\n";
}
$addgoid_end = strftime("%a %b %e %H:%M:%S %Y", localtime);
print LOG ("addgoid parser ending at $addgoid_end\n");
}    #end of test_addgoid


#test comgen parser, this will be tested until all database are built
#Note:
#1.comgen parser goes throungh all *.asn files, the comgen log file will show thousands "GI not found in ACCDB"
#  since it depends on ACCDB table, but we have only use 2 test cases (above test mother)
#2.if only test few .asn file, then comgen logfile has hundreds lines indicate that "ERROR: Cannot find input file e.g. NC_000854.asn",
#  although it presents "Main: Done"
#following tests on the few asn files
#required files: hs_chr8.asn mm_chr10.asn (these files are randomly
#                picked, so GI are not found in TAXGI databases) 
sub test_comgen
{
$comgen_flag = "FALSE";
$comgen_start = strftime("%a %b %e %H:%M:%S %Y", localtime);
print LOG "\n--------";
print LOG "comgen parser starting at $comgen_start\n";

if(system("./comgen -l T -p ./") !=0)
{
    print LOG "check for: ./comgen -l T -p ./								fail\n";
    $comgen_flag = "TRUE";
}
else
{
    print LOG "check for: ./comgen -l T -p ./							        OK\n";
    if (-e "$PATH/comgenlog")
    {
        print LOG "check for: presence of comgen log file							OK\n";
    }
    else
    {
        print LOG "check for: presence of comgen log file							fail\n";
	$comgen_flag = "TRUE";
    }
    if (check_log("./comgenlog", "ERROR") !=0)
    {
        print LOG "check for: log file has no error message						OK\n";
    }
    else
    {
    	print LOG "check for: log file has no error message						fail\n"; 
	$comgen_flag = "TRUE";
    }
    if (check_log("./comgenlog", "Main: Done") !=0)
    {
        print LOG "check for: presence of 'done'								fail\n";
	$comgen_flag = "TRUE";
    }
    else
    {
    	print LOG "check for: presence of 'done'								OK\n"; 
    }
}

if ($comgen_flag eq "TRUE")
{
    print LOG "check for: comgen parser test								fail\n";
}
else
{
    print LOG "check for: comgen parser test								OK\n"; 
}

$comgen_end = strftime("%a %b %e %H:%M:%S %Y", localtime);
print LOG ("comgen ending at $comgen_end\n");
}   #end of test_comgen


#helper funcitons start here
#check_data
#parameter: a directory, a list of file name 
#return 0 if all files names with the database extension .cdx .dbf .fpt are found in the directory
sub check_data
{
     my ($dir, @list)= @_;
     
    foreach $file (@list)
    { 
       if ((-e "$dir/$file.cdx") && (-e "$dir/$file.dbf") && (-e "$dir/$file.fpt")){
           print "$file exists\n";
       }
       else {
           return -1;
       }
    }
    return 0;
}
    

#check_data2
#parameter: a directory, a list of file name
#return 0 if all file names with extension .cdx and .dbf are found in directory, and record which filename will have .fpt extension
sub check_data2
{
     my ($dir, @list)= @_;
     
    foreach $file (@list)
    { 
       if ((-e "$dir/$file.cdx") && (-e "$dir/$file.dbf")){
           if (-e "$dir/$file.fpt")
	   {
	        print "$file has .fpt extension.\n";
           }
       }
       else {
           return -1;
       }
    }
    return 0;
}
 
       
#checklog
#parameters: a log file name and a string are looking for
#return 0 if string found in the log file 
sub check_log
{
    my ($file, $string_to_find) = @_;
    open (FILE, "$file") || die "Cannot open log file $file";
    sysread(FILE, my $file, -s FILE);
    if (index($file, $string_to_find)!= -1) {
         print "find $string_to_find\n";
    }
    else{
        close(FILE);
        return -1;
    }
    
    close(FILE);
    return 0;
}



#main starts here
$now=strftime("%a %b %e %H:%M:%S %Y", localtime);
print LOG "===============================================\n";
print LOG "Parsertest.pl starting at time:   $now\n";
print LOG "\n";

test_mother();
test_redund();
test_importtaxdb();
test_goparser();
test_llparser();
test_chrom();
test_cbmmdb();
test_neighbours();
test_rps();
test_vastblst();
test_pdbrep();
test_histparser();
test_addgoid();
test_comgen();

 
$parsertest_end =  strftime("%a %b %e %H:%M:%S %Y", localtime);
print LOG  "\n";
print LOG "===============================================\n";
print LOG "Parsertest.pl ending at time:   $parsertest_end\n";
print LOG "\n"; 
 
print "Done.\n";
print "Please see parser_test.log for more details.\n";	  
   
close(LOG);



