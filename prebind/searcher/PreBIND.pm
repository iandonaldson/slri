# $Id: PreBIND.pm,v 1.5 2004/11/29 16:14:48 kzheng Exp $
##############################################################
# LICENSE 
##############################################################
# Errors are output to standout, use redirection to save to logfile
# Error syntax: ERR:function-name:error-message
# <<Programming Perl>> 1.6.1 What Is Truth? 
#     1. Any string is true except for""and"0".
#     2. Any number is true except for 0.
#     3. Any reference is true.
#     4. Any undefined value is false.
##############################################################

package PreBIND;

use strict;
use vars qw($VERSION @ISA @EXPORT @EXPORT_OK);

use Exporter;

@ISA = qw(Exporter);
# Items to export into the callers space
# Public functions: get, set, insert, delete ; init, updates
# Private functions:  
@EXPORT = qw(
    PbInit PbFini
    PbInitMyMED PbInitSeqhound PbInitPrebind
    PbCloseMyMED PbCloseSeqhound PbClosePrebind
    PbPrepareSth PbFinishSth 
    PbGetProperty 
    PbUpdateSearch PbSearch PbSearchByDate 
    PbUpdateResult PbUpdateEvidence
   );
######################### VARIALBES #########################
my $dbh_pb;
my $dbh_sh;
my $dbh_db2;

my $sth_insert_doc;
my $sth_get_docid;
my $sth_insert_search;
my $sth_get_searchid;
my $sth_insert_result;
my $sth_get_resultid;
my $sth_insert_evidence;
my $sth_get_evidenceid;
my $sth_insert_point;
my $sth_get_pointid;
my $sth_get_cooccurrence;
my $sth_select_search;
my $sth_update_searched;
my $sth_select_search_by_date;
my $sth_search_medline_count;
my $sth_search_medline_by_date_count;
my $sth_search_medline;
my $sth_search_medline_by_date;

my $ini; 
my $rc;
my %stopwords;
# insert counter
my $n_doc=0;
my $n_rng=0;
my $n_search=0;
my $n_result=0;
my $n_evidence=0;
my $n_point=0;

# taxid and mesh term hashtable
my %taxmesh=(   
    4932=>"Saccharomyces cerevisiae", 
    9606=>"human",
    10090=>"mice",
    10116=>"rats",
    7227=>"Drosophila melanogaster",
    6239=>"Caenorhabditis elegans");

my %englishwords;
######################### FUNCTIONS #########################

# or use create a table to hold this hashtable like this
# @return hashtable for taxnomy id and mesh terms
sub createTaxmeshHashTable
{
    my %hashtable;
    my $sth_pb = $dbh_pb->prepare("SELECT taxid, meshterm from taxmesh");
    $sth_pb->execute();
    while(my @row = $sth_pb->fetchrow_array())
    {
         $hashtable{$row[0]} = $row[1];
    }
    $sth_pb->finish;
    return %hashtable;
}

# Read a tag from a proterty file. If more than one value,
# Return all values concated by semi-column
# @param:
#     $_[0] FileName: Property File.
#     $_[1] Tag:      Tag to read from the property file.
# @Return: Value of that tag.
#         0 if tag not found.
sub PbGetProperty{
    open (INI, $_[0]) or 
        die "ERR:PbGetProperty:CAN NOT OPEN INI FILE: $_[0] \n";
    my $line;
    my $value=();
    my $tag=$_[1];
    my $found=0;
    while($line = <INI>){
        chomp($line);
        if($line=~/^\s*$tag\s*=\s*(\S+).*$/){
            if($found==0){
                $value=$1;
                $found=1;
            }else{
                $value=$value.";".$1;
            }
        }
    }
   close(INI);
   if($found==0){
        print "ERR:PbGetProperty:$tag value not found\n";
    }else {
        $value=~s/\"//g;
#print "$tag=$value\n";
    }
    return $value;
}

sub createStopwordsHashTable{
    my $stopwordsfile=PbGetProperty($ini, "stop_words");
    open STOPWORDS, "$stopwordsfile" 
        || die "can't open pubmed stop words file: $!\n";
    my $word;
    foreach $word(<STOPWORDS>){
        chomp($word);
        $stopwords{$word}=1;
    }
    close(STOPWORDS);
    print "Stop word hash table generated. \n";
    print %stopwords, "\n";    
}
sub createDictionary{
    my $dict=PbGetProperty($ini, "english_words");
    open DICT, $dict || die "Can't open dictionary file $dict \n"; 
    my $word;
    foreach $word(<DICT>){
        chomp($word);
        $englishwords{$word}=1;
    }
    close(DICT);
    print "English dictioary created \n";
    print %englishwords, "\n";
}

#====================== database handler ==========================
sub PbInitMyMED{
    my $db2_user=PbGetProperty($ini, "db2_user");
    my $db2_password=PbGetProperty($ini, "db2_password");
    my $db2_database=PbGetProperty($ini, "db2_database");
    my $db2_dsn = "DBI:DB2:$db2_database";
    $dbh_db2 = DBI->connect($db2_dsn,$db2_user,$db2_password, 
        {RaiseError => 0,  AutoCommit => 1 })  
       or die "ERR:PbInitMyMED:$DBI::errstr\n";
    $dbh_db2->{LongReadLen}=20*1024;
    $dbh_db2->{LongTruncOk}=1;
    print "connect to db2 mymed successful\n";
}

sub PbInitSeqhound{
    my $sh_host=PbGetProperty($ini, "sh_host");
    my $sh_port=PbGetProperty($ini, "sh_port");
    my $sh_user=PbGetProperty($ini, "sh_user");
    my $sh_password=PbGetProperty($ini, "sh_password");
    my $sh_database=PbGetProperty($ini, "sh_database");
    my $sh_dsn="DBI:mysql:$sh_database:$sh_host:$sh_port";
    $dbh_sh= DBI->connect ($sh_dsn, $sh_user, $sh_password, 
       {RaiseError => 0, AutoCommit => 1})
       or die "ERR:PbInitSeqhound:$DBI::errstr\n";
    print "connect to mysql seqhound successful\n";
}

sub PbInitPrebind{
    my $pb_host=PbGetProperty($ini, "pb_host");
    my $pb_port=PbGetProperty($ini, "pb_port");
    my $pb_user=PbGetProperty($ini, "pb_user");
    my $pb_password=PbGetProperty($ini, "pb_password");
    my $pb_database=PbGetProperty($ini, "pb_database");
    my $pb_dsn= "DBI:mysql:$pb_database:$pb_host:$pb_port";
    $dbh_pb= DBI->connect ($pb_dsn, $pb_user, $pb_password, 
    {RaiseError => 0, AutoCommit => 1})
    or die "ERR:PbInitPrebind:$DBI::errstr\n";
    print "connect to mysql prebind successful\n";
}

sub PbCloseMyMED{
    $dbh_db2->close;
}
sub PbCloseSeqhound{
    $dbh_sh->close;
}
sub PbClosePrebind{
    $dbh_pb->close;
}

sub PbPrepareSth{
# insert and getid stament handlers
#    $sth_insert_doc=$dbh_pb->prepare("insert into doc(dbid, accession) values(?,?)")
#        or die "ERR:PbPrepareSth:sth_insert_doc\n";
#    $sth_get_docid=$dbh_pb->prepare("select docid from doc where dbid=? and accession=?")
#        or die "ERR:PbPrepareSth:sth_get_docid\n";
#    $sth_insert_result=$dbh_pb->prepare("insert into result(searchid, docid) values(?,?)")
#        or die "ERR:PbPrepareSth:sth_insert_result\n";
#    $sth_get_resultid=$dbh_pb->prepare("select resultid from result where searchid=? and docid=?")
#        or die "ERR:PbPrepareSth:sth_get_result_id\n";
#    $sth_insert_evidence=$dbh_pb->prepare("insert into evidence(resultidA, resultidB, docid, pointid) values(?,?,?,?)")
#        or die "ERR:PbPrepareSth:sth_insert_evidence\n";
#    $sth_get_evidenceid=$dbh_pb->prepare("select evidenceid from evidence where resultida=? and resultidb=?")
#        or die "ERR:PbPrepareSth:sth_get_evidenceid\n";
#    $sth_insert_point=$dbh_pb->prepare("insert into point(bioentityida, bioentityidb) values(?,?)")
#        or die "ERR:PbPrepareSth:sth_insert_point\n";
#    $sth_get_pointid=$dbh_pb->prepare("select pointid from point where bioentityida=? and bioentityidb=?")
#        or die "ERR:PbPrepareSth:sth_get_pointid\n";
#    $sth_get_cooccurrence=$dbh_pb->prepare("select resultid, bioentityid 
#        from result r,search s, bioname b
#        where r.docid=? and r.searchid = s.searchid and taxid=? and s.bioentityid!=? and s.bionameid=b.bionameid and not(b.name like ?)")
#         or die "ERR:PbPrepareSth:sth_get_cooccurrence\n";
}

sub PbFinishSth{
#    $sth_insert_doc->finish;
#    $sth_get_docid->finish;
#    $sth_insert_result->finish;
#    $sth_get_resultid->finish;
#    $sth_insert_evidence->finish;
#    $sth_get_evidenceid->finish;
#    $sth_insert_point->finish;
#    $sth_get_pointid->finish;
#    $sth_select_search->finish;
#    $sth_select_search_by_date->finish;
#    $sth_update_searched->finish;
#    $sth_search_medline_count->finish;
#    $sth_search_medline->finish;
#    $sth_search_medline_by_date->finish;
#    $sth_search_medline_by_date_count->finish;
    
}
sub PbInit{
    $ini=$_[0];
    PbInitMyMED();
    PbInitSeqhound();
    PbInitPrebind();
}

sub PbFini{
    print "================== summary ===================";
    print "$n_rng inserts to RNG\n";
    print "$n_search inserts to SEARCH\n";
    print "$n_doc inserts to DOC\n";
    print "$n_result inserts to RESULT\n";
    print "$n_evidence inserts to EVIDENCE:\n";
    print "$n_point inserts to POINT\n";
    print "==============================================";
    PbCloseMyMED();
    PbClosePrebind();
    PbCloseSeqhound();
}

# ================== UpDATE procedures ===========================
#
# if support subquery, should check any new bionameid not in search
# now use bionameid
sub PbUpdateSearch{
    # synchronize search with bioname, Trigger function is better
    # check new bioname ids in bioname table
    my $last_max_id=0; # last update bioname id    
    my @row_ary= $dbh_pb->selectrow_array("select max(bionameid) from search")
        or die "ERR:PbUpdateSearch";
    print "row=",@row_ary, "\n";
    if(@row_ary&&$row_ary[0]>=1){ 
        $last_max_id=$row_ary[0];
    }
    print "last_max_bionameid=$last_max_id\n";
    
    # update rng (redundant name group) table from bioname
    my $sth_get_nametax=$dbh_sh->prepare("select distinctrow a.name c.taxid 
     from bioname a, bioentity b, taxgi c
     where a.id>$last_max_id and a.action like '1' and a.bioentityid=b.id and b.identifier=c.gi");
    $sth_get_nametax->execute;
    
    my $sth_insert_rng=$dbh_pb->prepare("Insert IGNORE into table rng(name,taxid) values(?,?)");
    while (my ($name, $taxid)=$sth_get_nametax->fetchrow_array){
        $sth_insert_rng->execute($name, $taxid);
        $n_rng++;
    }
    $sth_get_nametax->finish;
    $sth_insert_rng->finish;
    
    # update search table from bioname
    my $query= "select a.id, a.bioentityid, a.name, c.taxid
     from bioname a,  bioentity b, taxgi c
     where  a.id>$last_max_id and 
       a.action like '1'  and a.nametypeid=2 and 
       a.bioentityid=b.id and b.identifier=c.gi";
    #print "QUERY=$query\n";
    my $sth_get_names=$dbh_sh->prepare($query)
        or die "ERR:PbUpdateSearch:sth_get_names";
    $sth_get_names->execute()
        or die "ERR:PbUpdateSearch:sth_get_names";
    
    my $sth_insert_search=$dbh_pb->prepare("INSERT  INTO search
       (bionameid, bioentityid, name,taxid) VALUES(?,?,?,?)")
        or die "ERR:PbUpdateSearch:sth_insert_search";
    while(my ($bionameid, $bioentityid, $name, $taxid)=$sth_get_names->fetchrow_array) {
        $sth_insert_search->execute($bionameid, $bioentityid, $name, $taxid);
        $n_search++;
    }
    $sth_get_names->finish;
    $sth_insert_search->finish;
    
    # update rngid in search table
    my $rows=$dbh_pb->do("update search s,rng r set s.rngid=r.rngid where s.name=r.name and s.taxid=r.taxid");
    print "$rows/$n_search rngid have been updated\n"; 
}


PbUpdateRng{
    my $row=$dbh_pb->do("update rng set status='illegal' where status is null and name NOT REGEXP \"[[:alpha:]]+\"");
    print "$row new names has no alpha\n";
    #my $row=$dbh_pb->do("update rng set status='illegal' where status is null and name REGEXP \"[[:cntrl:]]+\"");
    #print "$row new names has cntrl\n";
    my $row=$dbh_pb->do("update rng set status='illegal' where status is null and name REGEXP \"[[:'\":]]+\"");
    print "$row new names has quotas\n";
    my $row=$dbh_pb->do("update rng a, lexicon b set a.status='stop'
        where a.status is null and a.name=b.word and (b.status='pubmed' or b.status='stop')");
    print "$row new names are stopwords\n";

}

# to do: define store procedure for this function
# db2 "call medline.prebind_search($mesh, $name)"
sub PbDoSearch
{
    my $taxid= $_[0];
    my $mesh = $taxmesh{$taxid};
    my $total=0;
    my $count=0;

    my $stmt_count_rng="select count(*) from rng 
        where taxid=$taxid and searched=0 and status is NULL";
    if(my @a=$dbh_pb->selectrow_array($stmt_count_rng)){
        $total = $a[0];
    }
    print "---------- TAXID:$taxid\tMESH:$mesh\tTOTAL:$total ---------\n";
    
    my $sth_select_rng=$dbh_pb->prepare("select $rngid, $name from rng where taxid=$taxid and searched=0 and status is NULL");
    $sth_update_rng=$dbh_pb->prepare("update rng set searched=now(), status='searched', results=? where rngid=?") ;    
    $sth_insert_rngresult=$dbh_pb->prepare("Insert into rngresult(rngid, pmid) values(?,?)");
    
    $sth_select_rng->execute;
    while (my ($rngid, $name)=$sth_select_rng->fetchrow_array){
        $count++;
                
        # search MyMed
         my $stmt_count = "SELECT count(pmid) FROM medline.citations ".
              "WHERE contains(citation,\'section(\"mesh\") \"$mesh\" ".
               "& sections(\"abstracts\", \"titles\")\"$name\"\')=1";
        if(my @data=$dbh_db2->selectrow_array($stmt_count)){
            my $n_pmid=$data[0];
            $sth_update_rng->execute($n_pmid, $rngid);
            print "==[$count/$total]\t$name\t$taxid\t$n_pmid==\n";
            if ($n_pmid>10000){
                if(my @row=$dbh_pb->selectrow_array("select word from lexicon where word=$name") ){
                    $dbh_pb->do("update rng set status='stop' where name=$name");
                    print "== [stopword]\t$name\t$taxid\t$n_pmid==\n";        
                    next;
                }
            }
            next if $n_pmid==0;                
        }
        
        my $stmt = "SELECT pmid FROM medline.citations ".
              "WHERE contains(citation,\'section(\"mesh\") \"$mesh\" ".
               "& sections(\"abstracts\", \"titles\")\"$name\"\')=1";
        #print "$stmt\n";
        $sth_search_medline=$dbh_db2->prepare($stmt);
        $sth_search_medline->execute;
        while(my @ary=$sth_search_medline->fetchrow_array){
            $sth_insert_rng_result->execute($rngid, $ary[0]);
        }
        $sth_search_medline->finish;
 
    }
    $sth_select_rng->finish;
    $sth_update_rng->finish;
    $sth_insert_rng_result->finish;
    
    $dbh_pb->do("update rngresult a, doc b set a.docid=b.docid 
        where a.docid is null and a.pmid=b.accession and b.dbid=1");
        
    $dbh_pb->do("update search a, rng b set set a.rngid=b.rngid
        where  a.rngid is null and a.taxid=$taxid and a.name=b.name)";   
        
    $dbh_pb->do("insert into result(searchid, bioentityid, taxid, rngid, docid) 
        select searchid, bioentityid, taxid, a.rngid, b.docid 
        from search a, rngresult b, docscore c
        where a.searchid=0 and a.taxid=$taxid and a.rngid=b.rngid 
            and b.docid=c.docid and docscore=1 and method=2");

    $dbh_pb->do("update search a, rng b set set a.searched=b.searched,a.results=b.results 
        where  a.taxid=$taxid and a.searched=0)";  
}

# redo search with doc insert date later than search date
sub PbDoSearchByDate
{
    my $taxid=$_[0];
    my $mesh= $taxmesh{$taxid};
    my $total;
    my $count=0;
    
    my $stmt_count_rng="select count(*) from rng where taxid=$taxid and status='searched'";
    if(my @a=$dbh_pb->selectrow_array){
        $total = $a[0];
    }
    print "---------- TAXID:$taxid\tMESH:$mesh\tTOTAL:$total ---------\n";
    
    my $sth_select_rng=$dbh_pb->prepare("select $rngid, $name,$searched from rng 
        where taxid=$taxid and and status='searched'";
    $sth_select_rng->execute;
    $sth_update_rng=$dbh_pb->prepare("update search set searched=now(), status='updated', results=results+? where rngid=?") ;
    $sth_insert_rngresult=$dbh_pb->prepare("Insert into rngresult(rngid, pmid) values(?,?)");
    while(my ($rngid, $name, $searched)=$sth_select_rng->fetchrow_array){
        $count++;
        # next if PbFilter($name);
        
        # search MyMed
         my $stmt_count = "SELECT count(pmid) FROM medline.citations ".
              "WHERE contains(citation,\'section(\"mesh\") \"$mesh\" ".
               "& sections(\"abstracts\", \"titles\")\"$name\"\')=1 and date>$searched";
        if(my @data=$dbh_db2->selectrow_array($stmt_count)){
            my $n_pmid=$data[0];
            $sth_update_rng->execute($n_pmid, $rngid);
            print " ==[$count/$total]$name\t$taxid\t$n_pmid==\n";
            next if $n_pmid==0;    
        }
        
        my $stmt="SELECT pmid FROM medline.citations ".
             "WHERE contains(citation,\'section(\"mesh\") \"$mesh\" ".
             " & sections(\"abstracts\", \"titles\")\"$name\"\')=1 and date>$searched;";
        $sth_search_medline=$dbh_db2->prepare($stmt);
        $sth_search_medline->execute;
        while( my @ary=$sth_search_medline->fetchrow_arry){
            $sth_insert_rng_result->execute($rngid, $ary[0]);
        }
        $sth_search_medline->finish;
    }
    $sth_select_rng->finish;
    $sth_update_rng->finish;
    $sth_insert_rng_result->finish;
    
    $dbh_pb->do("update rngresult a, doc b set a.docid=b.docid 
        where a.docid is null and a.pmid=b.accession and b.dbid=1");
        
    $dbh_pb->do("update search a, rng b set a.searched=b.searched, a.results=b.results 
        where b.taxid=$taxid and b.status='updated' and b.rngid=b.rngid)";   
        
    $dbh_pb->do("insert into result(searchid, bioentityid, taxid, rngid, docid) 
        select searchid, bioentityid, taxid, a.rngid, b.docid 
        from search a, rngresult b
        where b.state=0 and b.rngid= a.rngid and a.taxid=$taxid");
    
    $dbh_pb->do("update rngresult set state=1 where state =0");
    
    $dbh_pb->do("update rng set status='searched' where status='updated'");
}

sub PbUpdateResult{
    my $taxid=$_[0];
    #insert point table 
    $dbh_pb->do("Insert IGNORE into point(bioentityida, bioentityidb) 
        select distinctrow a.bioentityid, b.bioentityid 
        from result a, result b 
        where a.status=0 and a.taxid=$taxid and b.status=0 and b.taxid=$taxid and a.bioentityid<b.bioentityid");
    $dbh_pb->do("Insert IGNORE into point(bioentityida, bioentityidb) 
        select a.bioentityid, b.bioentityid 
        from result a, result b 
        where a.status=0 and a.taxid=$taxid and b.status!=0 and b.taxid=$taxid and a.bioentityid<b.bioentityid");

    #insert evidence table 
    $dbh_pb->do("insert into evidence(resultida, bioentityida, resultidb, bioentityidb,docid)
         select a.resultid, a.bioentityid, b.resultid, b.bioentityid, a.docid 
         from result a, result b
        where a.status=0 and a.taxid=$taxid and b.status=0 and b.taxid=$taxid 
            and a.resultid<b.resultid and a.rngid!=b.rngid ");
    $dbh_pb->do("insert into evidence(resultida, bioentityida,resultidb, bioentityidb,docid) 
        select a.resultid, b.resultid, a.docid 
        from result a, result b 
        where a.status=0 and a.taxid=$taxid and b.status!=0 and b.taxid=$taxid 
            and a.resultid<b.resultid and a.rngid!=b.rngid");

    $dbh_pb->do("update result set status=1 where status=0");
}


sub PbUpdateEvidence{
     $dbh_pb->do("update evidence a, point b set a.pointid=b.pointid
        where a.pointid is NULL and 
        ((a.bioentityida<a.bioentityidb and a.bioentityida=b.bioentityida and a.bioentityidb=b.bioentityidb) or
        (a.bioentityida>a.bioentityidb and a.bioentityida=b.bioentityidb and a.bioentityidb=b.bioentityida))")
}

#================= to be impremented =========================
sub PbUpdateResultscore{} # new name disambiguation method
sub PbUpdateEvidencescore{} #---------> prebind_regex
sub PbUpdatePointscore{} # select max evidence score
#================================================================

1;
__END__

=head1 NAME

PreBIND PERL API - interfaces for SeqHound prebind module

=head1 SYNOPSIS 

Please consult the SeqHound manual prebind module.

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

Kai Zheng
E<lt>seqhound@blueprint.orgE<gt>


=head1 SEE ALSO

README file, README_API.pod
perl(1). 
http://www.blueprint.org/seqhound/api_help/seqhound_help_guides.html

=cut

