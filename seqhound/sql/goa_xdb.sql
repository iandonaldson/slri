-- MySQL dump 9.11
--
-- Host: skinner    Database: seqhound
-- ------------------------------------------------------
-- Server version	4.0.20-log

--
-- Table structure for table `goa_xdb`
--
use dbxrefgoa

CREATE TABLE goa_xdb (
  id INTEGER NOT NULL PRIMARY KEY AUTO_INCREMENT,
  abbreviation varchar(50) NOT NULL UNIQUE,
  name varchar(255) DEFAULT NULL,
  object varchar(255) DEFAULT NULL,
  example varchar(50) DEFAULT NULL,
  generic_url varchar(255) DEFAULT NULL,
  url_syntax varchar(255) DEFAULT NULL,
  url_example varchar(255) DEFAULT NULL,
  lastupdate TIMESTAMP NOT NULL,
  INDEX abbreviation_idx(abbreviation),
  INDEX name_idx	(name),
  INDEX lastupdate_idx	(lastupdate)
) TYPE=InnoDB;

--
-- Dumping data for table `goa_xdb`
--

INSERT INTO goa_xdb VALUES (1,'AGI_LocusCode',' Arabidopsis Genome Initiative (TAIR, TIGR, MIPS)','Locus identifier','AGI_LocusCode:At2g17950','http://www.arabidopsis.org','http://arabidopsis.org/servlets/TairObject?type=locus&name=','http://arabidopsis.org/servlets/TairObject?type=locus&name=At2g17950',NULL);
INSERT INTO goa_xdb VALUES (2,'bib',' AGRICOLA','call number','bib=0000-05160','http://www.nal.usda.gov/ag98/',NULL,NULL,NULL);
INSERT INTO goa_xdb VALUES (3,'CBS',' Center for Biological Sequence Analysis','prediction tool','CBS:NetNGlyc','http://www.cbs.dtu.dk/',NULL,'http://www.cbs.dtu.dk/services/TMHMM/',NULL);
INSERT INTO goa_xdb VALUES (4,'CGD_LOCUS',' Candida Genome Database.note: This object also includes the orf19 assembly names, eg. orf19.2475.','Gene name (gene symbol in mammalian nomenclature).','CGD_LOCUS:orf19.2475','http://www.candidagenome.org/','http://www.candidagenome.org/cgi-bin/locus.pl?locus=','http://www.candidagenome.org/cgi-bin/locus.pl?locus=orf19.2475',NULL);
INSERT INTO goa_xdb VALUES (5,'CGEN',' Compugen Gene Ontology Gene Association Data.','Identifier.','CGEN:PrID131022','!{see GO/gene-associations/readme/Compugen.README}','!{see GO/gene-associations/readme/Compugen.README}',NULL,NULL);
INSERT INTO goa_xdb VALUES (6,'CGSC',' CGSC: E.coli Genetic Stock Center.','Gene symbol.','CGSC:rbsK','http://cgsc.biology.yale.edu/',NULL,'http://cgsc.biology.yale.edu/cgi-bin/sybgw/cgsc/Site/315',NULL);
INSERT INTO goa_xdb VALUES (7,'DDB',' DictyBase','Accession ID','DDB0001836','http://dictybase.org','http://dictybase.org/db/cgi-bin/gene_page.pl?dictybaseid=','http://dictybase.org/db/cgi-bin/gene_page.pl?dictybaseid=DDB0001836',NULL);
INSERT INTO goa_xdb VALUES (8,'DDB_LOCUS',' DictyBase','Locus','mlcE','http://dictybase.org','http://dictybase.org/db/cgi-bin/dictyBase/locus.pl?locus=','http://dictybase.org/db/cgi-bin/dictyBase/locus.pl?locus=mlce',NULL);
INSERT INTO goa_xdb VALUES (9,'EC',' The Enzyme Commission.url_syntax:',NULL,'EC:1.1.1.1','http://www.chem.qmw.ac.uk/iubmb/enzyme/',NULL,'http://www.chem.qmw.ac.uk/iubmb/enzyme/EC1/1/1/1.html',NULL);
INSERT INTO goa_xdb VALUES (10,'EcoCyc',' The Encylopedia of E. coli metabolism.','Pathway identifier.','EcoCyc:P2-PWY','http://ecocyc.org/','http://malibu.ai.sri.com:1555/ECOLI/new-image?type=PATHWAY&object=?','http://malibu.ai.sri.com:1555/ECOLI/new-image?type=PATHWAY&object=P2-PWY',NULL);
INSERT INTO goa_xdb VALUES (11,'ECOGENE',' CGSC: E.coli Genetic Stock Center.','EcoGene Accession Number.','ECOGENE:EG10818','http://cgsc.biology.yale.edu/',NULL,'http://cgsc.biology.yale.edu/cgi-bin/sybgw/cgsc/Site/315',NULL);
INSERT INTO goa_xdb VALUES (12,'ECOGENE_G',' CGSC: E.coli Genetic Stock Center.','EcoGene Primary Gene Name.','ECOGENE_G:deoC','http://cgsc.biology.yale.edu/',NULL,NULL,NULL);
INSERT INTO goa_xdb VALUES (13,'EMBL',' EMBL-EBI International Nucleotide Sequence Data Library/DDBJ/GenBank.','Sequence accession number.','EMBL:AA816246','http://www.ebi.ac.uk/embl/','http://www.ncbi.nlm.nih.gov:80/entrez/query.fcgi?cmd=Retrieve&db=nucleotide&list_uids=10727410&dopt=GenBank',NULL,NULL);
INSERT INTO goa_xdb VALUES (14,'DDBJ',' EMBL-EBI International Nucleotide Sequence Data Library/DDBJ/GenBank.','Sequence accession number.','EMBL:AA816246','http://www.ebi.ac.uk/embl/','http://www.ncbi.nlm.nih.gov:80/entrez/query.fcgi?cmd=Retrieve&db=nucleotide&list_uids=10727410&dopt=GenBank',NULL,NULL);
INSERT INTO goa_xdb VALUES (15,'GB',' EMBL-EBI International Nucleotide Sequence Data Library/DDBJ/GenBank.','Sequence accession number.','EMBL:AA816246','http://www.ebi.ac.uk/embl/','http://www.ncbi.nlm.nih.gov:80/entrez/query.fcgi?cmd=Retrieve&db=nucleotide&list_uids=10727410&dopt=GenBank',NULL,NULL);
INSERT INTO goa_xdb VALUES (16,'ENSEMBL',' Database of automatically annotated genomic data.','Identifier.','ENSEMBL:ENSP00000265949','http://www.ensembl.org/','http://www.ensembl.org/perl/protview?peptide=','http://www.ensembl.org/perl/protview?peptide=ENSP00000265949',NULL);
INSERT INTO goa_xdb VALUES (17,'ENZYME',' The Swiss Institute of Bioinformatics database of Enzymes.','Identifier.','ENZYME:EC 1.1.1.1','http://www.expasy.ch/','http://www.expasy.ch/cgi-bin/nicezyme.pl?','http://www.expasy.ch/cgi-bin/nicezyme.pl?1.1.1.1',NULL);
INSERT INTO goa_xdb VALUES (18,'FB',' FlyBase.note: [NCBI use FLYBASE]','Gene identifier.','FB:FBgn0000024','http://flybase.bio.indiana.edu/','http://flybase.bio.indiana.edu/.bin/fbidq.html?','http://fly.ebi.ac.uk:7081/.bin/fbidq.html?FBgn0000055',NULL);
INSERT INTO goa_xdb VALUES (19,'FLYBASE',' FlyBase.','Gene symbol.','FLYBASE:Adh','http://flybase.bio.indiana.edu/','http://fly.ebi.ac.uk:7081/.bin/fbidq.html?',NULL,NULL);
INSERT INTO goa_xdb VALUES (20,'GDB',' Human Genome Database.','Accession number.','GDB:306600','http://www.gdb.org/','http://www.gdb.org/gdb-bin/genera/accno?accessionNum=','http://www.gdb.org/gdb-bin/genera/accno?accessionNum=GDB:306600',NULL);
INSERT INTO goa_xdb VALUES (21,'GeneDB_Spombe',' GeneDB_Spombe.','Gene identifier.','GeneDB_Spombe:SPAC890.04C','http://www.genedb.org/pombe','http://www.genedb.org/genedb/Search?organism=pombe&name=','http://www.genedb.org/genedb/Search?organism=pombe&name=SPAC890.04C',NULL);
INSERT INTO goa_xdb VALUES (22,'GeneDB_Gmorsitans',' GeneDB_Gmorsitans.','Gene identifier.','GeneDB_Gmorsitans:Gmm-0142','http://www.genedb.org/glossina','http://www.genedb.org/genedb/Search?organism=glossina&name=','http://www.genedb.org/genedb/Search?organism=glossina&name=Gmm-0142',NULL);
INSERT INTO goa_xdb VALUES (23,'GeneDB_Lmajor',' GeneDB_Lmajor.','Gene identifier.','GeneDB_Lmajor:LM5.32','http://www.genedb.org/leish','http://www.genedb.org/genedb/Search?organism=leish&name=','http://www.genedb.org/genedb/Search?organism=leish&name=LM5.32',NULL);
INSERT INTO goa_xdb VALUES (24,'GeneDB_Pfalciparum',' GeneDB_Pfalciparum.','Gene identifier.','GeneDB_Pfalciparum:PFD0755c','http://www.genedb.org/malaria','http://www.genedb.org/genedb/Search?organism=malaria&name=','http://www.genedb.org/genedb/Search?organism=malaria&name=PFD0755c',NULL);
INSERT INTO goa_xdb VALUES (25,'GeneDB_Tbrucei',' GeneDB_Tbrucei.','Gene identifier.','GeneDB_Tbrucei:Tb927.1.5250','http://www.genedb.org/tryp','http://www.genedb.org/genedb/Search?organism=tryp&name=','http://www.genedb.org/genedb/Search?organism=tryp&name=Tb927.1.5250',NULL);
INSERT INTO goa_xdb VALUES (26,'GenProtEC',' GenProtEC E. coli genome and proteome database.',NULL,NULL,'http://genprotec.mbl.edu/',NULL,NULL,NULL);
INSERT INTO goa_xdb VALUES (27,'GO',' Gene Ontology Database.','Identifier.','GO:0046703','http://godatabase.org/cgi-bin/go.cgi?query=','http://godatabase.org/cgi-bin/go.cgi?query=GO:0046703',NULL,NULL);
INSERT INTO goa_xdb VALUES (28,'GO_REF',' Gene Ontology Database references.example: GO_REF:0000001generic_url:url_example:','Accession (for reference).',NULL,NULL,NULL,NULL,NULL);
INSERT INTO goa_xdb VALUES (29,'GR',' Gramene: A Comparative Mapping Resource for Grains.','Protein','GR:P93436','http://www.gramene.org/','http://www.gramene.org/perl/protein_search?acc=','http://www.gramene.org/perl/protein_search?acc=P93436',NULL);
INSERT INTO goa_xdb VALUES (30,'GR_REF',' Gramene: A Comparative Mapping Resource for Grains.example: http://www.gramene.org/perl/pub_search?ref_id=659','Reference','GR_REF:659','http://www.gramene.org/','http://www.gramene.org/perl/pub_search?ref_id=ID number',NULL,NULL);
INSERT INTO goa_xdb VALUES (31,'HUGO',' Human Genome Organisationobject:example_id:url_syntax:',NULL,NULL,'http://www.gene.ucl.ac.uk/hugo/',NULL,NULL,NULL);
INSERT INTO goa_xdb VALUES (32,'HGNC',' HUGO Gene Nomenclature Committee.','Gene symbol.','HGNC:HNRPK','http://www.gene.ucl.ac.uk/nomenclature/',NULL,'http://www.gene.ucl.ac.uk/cgi-bin/nomenclature/searchgenes.pl?field=symbol&anchor=begins&match=HNRPK&symbol_search=Search&page_size=25&limit=1000&.cgifields=limit&.cgifields=page_size',NULL);
INSERT INTO goa_xdb VALUES (33,'IMGT_HLA',NULL,NULL,'IMGT_HLA:HLA00031','http://www.ebi.ac.uk/imgt/hla',NULL,NULL,NULL);
INSERT INTO goa_xdb VALUES (34,'IMGT_LIGM',NULL,NULL,'IMGT_LIGM:U03895','http://imgt.cines.fr',NULL,NULL,NULL);
INSERT INTO goa_xdb VALUES (35,'IND',' AGRICOLA','call number','IND84014403','http://agricola.cos.com/',NULL,NULL,NULL);
-- INSERT INTO goa_xdb VALUES (36,'InterPro',' The InterPro database of protein domains and motifs.','Identifier.','IPR000001','http://www.ebi.ac.uk/interpro/','http://www.ebi.ac.uk/interpro/DisplayIproEntry?ac=','http://www.ebi.ac.uk/interpro/DisplayIproEntry?ac=IPR000001',NULL);
INSERT INTO goa_xdb VALUES (36,'INTERPRO',' The InterPro database of protein domains and motifs.','Identifier.','IPR000001','http://www.ebi.ac.uk/interpro/','http://www.ebi.ac.uk/interpro/DisplayIproEntry?ac=','http://www.ebi.ac.uk/interpro/DisplayIproEntry?ac=IPR000001',NULL);
INSERT INTO goa_xdb VALUES (37,'IPI',' International Protein Index.','Identifier.Version','IPI:IPI00000005.1','http://www.ebi.ac.uk/IPI/IPIhelp.html',NULL,NULL,NULL);
INSERT INTO goa_xdb VALUES (38,'ISBN',' International Standard Book Number.','Identifier.','ISBN:0781702534','http://isbntools.com/','http://my.linkbaton.com/get?lbCC=q&nC=q&genre=book&item=','http://my.linkbaton.com/get?lbCC=q&nC=q&genre=book&item=0781702534',NULL);
INSERT INTO goa_xdb VALUES (39,'IUPHAR',' The IUPHAR Compendium of Receptor Characterization and Classification.','Receptor code.','IUPHAR:2.1.CBD',NULL,NULL,NULL,NULL);
INSERT INTO goa_xdb VALUES (40,'KEGG',' Kyoto Encyclopedia of Genes and Genomes.',NULL,NULL,'http://www.genome.ad.jp/kegg/',NULL,NULL,NULL);
INSERT INTO goa_xdb VALUES (41,'KEGG_PATHWAY',' KEGG Pathways Database.','Pathway.','KEGG_PATHWAY:ot00020','http://www.genome.ad.jp/kegg/docs/upd_pathway.html','http://www.genome.ad.jp/dbget-bin/www_bget?path:','http://www.genome.ad.jp/dbget-bin/www_bget?path:ot00020',NULL);
INSERT INTO goa_xdb VALUES (42,'LIGAND',' KEGG LIGAND Database.','Enzyme or Compound.','LIGAND:EC 1.1.1.1','http://www.genome.ad.jp/kegg/docs/upd_ligand.html#COMPOUND','http://www.genome.ad.jp/dbget-bin/www_bget?cpd:','http://www.genome.ad.jp/dbget-bin/www_bget?cpd:C00577',NULL);
INSERT INTO goa_xdb VALUES (43,'LocusID',' NCBI LocusLink ID.','Identifier.','LocusID:3195','http://www.ncbi.nlm.nih.gov/','http://www.ncbi.nlm.nih.gov:80/LocusLink/LocRpt.cgi?l=','http://www.ncbi.nlm.nih.gov:80/LocusLink/LocRpt.cgi?l=3195',NULL);
INSERT INTO goa_xdb VALUES (44,'MaizeGDB',' MaizeGDB','MaizeGDB Object ID Number','881225','http://www.maizegdb.org','http://www.maizegdb.org/cgi-bin/id_search.cgi?id=','http://www.maizegdb.org/cgi-bin/id_search.cgi?id=881225',NULL);
INSERT INTO goa_xdb VALUES (45,'MEDLINE',' The Medline literature database.generic_url:url_syntax:','Identifier.','MEDLINE:20572430',NULL,NULL,NULL,NULL);
INSERT INTO goa_xdb VALUES (46,'MetaCyc',' The Metabolic Encyclopedia of metabolic and other pathways.','Pathway identifier.','MetaCyc:GLUTDEG-PWY','http://metacyc.org/','http://biocyc.org:1555/META/new-image?object=','http://biocyc.org:1555/META/new-image?object=ASPARAGINE-SYN',NULL);
INSERT INTO goa_xdb VALUES (47,'MGD',' Mouse Genome Database.','Gene symbol.','MGD:Adcy9','http://www.informatics.jax.org/','http://www.informatics.jax.org/searches/marker.cgi?',NULL,NULL);
INSERT INTO goa_xdb VALUES (48,'MGI',' Mouse Genome Informatics.url_syntax:','Accession number.','MGI:80863','http://www.informatics.jax.org/','http://www.informatics.jax.org/searches/accession_report.cgi?id=',NULL,NULL);
INSERT INTO goa_xdb VALUES (49,'MO',' The MGED Ontology','ontology term','MO:Action','http://mged.sourceforge.net/ontologies/MGEDontology.php','http://mged.sourceforge.net/ontologies/MGEDontology.php#term','http://mged.sourceforge.net/ontologies/MGEDontology.php#Action',NULL);
INSERT INTO goa_xdb VALUES (50,'NAL',' AGRICOLA','call number','NAL:TP248.2 P76 v.14','http://www.nal.usda.gov/ag98/ag98.html',NULL,NULL,NULL);
INSERT INTO goa_xdb VALUES (51,'NASC_code',' Nottingham Arabidopsis Stock Centre Seeds Database','NASC code Identifier','NASC_code:N3371','http://arabidopsis.info','http://seeds.nottingham.ac.uk/NASC/stockatidb.lasso?code=','http://seeds.nottingham.ac.uk/NASC/stockatidb.lasso?code=N3371',NULL);
INSERT INTO goa_xdb VALUES (52,'NCBI',' National Center for Biotechnology Information, Bethesda.','Prefix.','NCBI_gi','http://www.ncbi.nlm.nih.gov/',NULL,NULL,NULL);
INSERT INTO goa_xdb VALUES (53,'NCBI_gi',' NCBI databases.url_syntax:','Identifier.','NCBI_gi:10727410','http://www.ncbi.nlm.nih.gov/',NULL,'http://www.ncbi.nlm.nih.gov:80/entrez/query.fcgi?cmd=Retrieve&db=nucleotide&list_uids=10727410&dopt=GenBank',NULL);
INSERT INTO goa_xdb VALUES (54,'NCBI_NM',' NCBI RefSeq.url_syntax:','mRNA identifier.','NCBI_NM:123456','http://www.ncbi.nlm.nih.gov/',NULL,NULL,NULL);
INSERT INTO goa_xdb VALUES (55,'NCBI_NP',' NCBI RefSeq.url_syntax:','Protein identifier.','NCBI_NP:123456','http://www.ncbi.nlm.nih.gov/',NULL,NULL,NULL);
INSERT INTO goa_xdb VALUES (56,'OMIM',' Mendelian Inheritance in Man.','Identifier.','OMIM:190198','http://www3.ncbi.nlm.nih.gov/Omim/searchomim.html','http://www3.ncbi.nlm.nih.gov/htbin-post/Omim/dispmim?','http://www3.ncbi.nlm.nih.gov/htbin-post/Omim/dispmim?190198',NULL);
INSERT INTO goa_xdb VALUES (57,'MIM',' Mendelian Inheritance in Man.','Identifier.','OMIM:190198','http://www3.ncbi.nlm.nih.gov/Omim/searchomim.html','http://www3.ncbi.nlm.nih.gov/htbin-post/Omim/dispmim?','http://www3.ncbi.nlm.nih.gov/htbin-post/Omim/dispmim?190198',NULL);
INSERT INTO goa_xdb VALUES (58,'PDB',' Protein Data Bank.url_syntax:','Identifier.','PDB:1A4U','http://www.rcsb.org/pdb/',NULL,'http://www.rcsb.org/pdb/cgi/explore.cgi?pid=223051005992697&pdbId=1A4U',NULL);
INSERT INTO goa_xdb VALUES (59,'Pfam',' Pfam: Protein families database of alignments and HMMs.','Accession number.','Pfam:PF00046','http://www.sanger.ac.uk/Software/Pfam/','http://www.sanger.ac.uk/cgi-bin/Pfam/getacc?','http://www.sanger.ac.uk/cgi-bin/Pfam/getacc?PF00046',NULL);
INSERT INTO goa_xdb VALUES (60,'PIR',' Protein Information Resource.','Accession number.','PIR:I49499','http://www-nbrf.georgetown.edu/pir/','http://pir.georgetown.edu/cgi-bin/pirwww/nbrfget?uid=','http://pir.georgetown.edu/cgi-bin/pirwww/nbrfget?uid=I49499',NULL);
INSERT INTO goa_xdb VALUES (61,'PMID',' PubMed.','Identifier.','PMID:4208797','http://www.ncbi.nlm.nih.gov/PubMed/',NULL,'http://www.ncbi.nlm.nih.gov:80/entrez/query.fcgi?cmd=Retrieve&db=PubMed&list_uids=4208797&dopt=Abstract',NULL);
INSERT INTO goa_xdb VALUES (62,'PUBMED',' PubMed.','Identifier.','PMID:4208797','http://www.ncbi.nlm.nih.gov/PubMed/',NULL,'http://www.ncbi.nlm.nih.gov:80/entrez/query.fcgi?cmd=Retrieve&db=PubMed&list_uids=4208797&dopt=Abstract',NULL);
INSERT INTO goa_xdb VALUES (63,'Pompep',' Schizosaccharomyces pombe protein data.url_syntax:','Gene/protein identifier.','Pompep:SPAC890.04C','ftp://ftp.sanger.ac.uk/pub/yeast/pombe/Protein_data/',NULL,NULL,NULL);
INSERT INTO goa_xdb VALUES (64,'Prosite',' Prosite. Database of protein families and domains.','Accession number.','Prosite:PS00365','http://www.expasy.ch/prosite/','http://www.expasy.ch/cgi-bin/prosite-search-ac?','http://www.expasy.ch/cgi-bin/prosite-search-ac?PS00365',NULL);
INSERT INTO goa_xdb VALUES (65,'protein_id',' The protein identifier shared by DDBJ/EMBL-bank/GenBank nucleotidesequence databases.generic_url:url_syntax:','Identifier.','protein_id:CAA71991',NULL,NULL,NULL,NULL);
INSERT INTO goa_xdb VALUES (66,'PROW',' Protein Reviews on the Web.',NULL,NULL,'http://www.ncbi.nlm.nih.gov/prow/',NULL,NULL,NULL);
INSERT INTO goa_xdb VALUES (67,'RGD',' Rat Genome Database.','Accession Number','RGD:2340','http://rgd.mcw.edu/','http://rgd.mcw.edu/query/query.cgi?id=','http://rgd.mcw.edu/query/query.cgi?id=2340',NULL);
INSERT INTO goa_xdb VALUES (68,'RESID',' RESID Database of Protein Modifications.url_syntax:','Identifier.','RESID:AA0062','ftp://ftp.ncifcrf.gov/pub/users/residues/',NULL,NULL,NULL);
INSERT INTO goa_xdb VALUES (69,'Sanger',' The Wellcome Trust Sanger Instituteurl_syntax:',NULL,NULL,'http://www.sanger.ac.uk/',NULL,NULL,NULL);
INSERT INTO goa_xdb VALUES (70,'SGD',' Saccharomyces Genome Database.','Identifier for SGD Loci','SGD:S000006169','http://www.yeastgenome.org/','http://db.yeastgenome.org/cgi-bin/locus.pl?dbid=','http://db.yeastgenome.org/cgi-bin/locus.pl?dbid=S000006169',NULL);
INSERT INTO goa_xdb VALUES (71,'SGDID',' Saccharomyces Genome Database.','Identifier for SGD Loci','SGD:S000006169','http://www.yeastgenome.org/','http://db.yeastgenome.org/cgi-bin/locus.pl?dbid=','http://db.yeastgenome.org/cgi-bin/locus.pl?dbid=S000006169',NULL);
INSERT INTO goa_xdb VALUES (72,'SGD_LOCUS',' Saccharomyces Genome Database.note: This object also includes the systematic S. cerevisiae ORF names, eg. YEL001C.','Gene name (gene symbol in mammalian nomenclature).','SGD_LOCUS:GAL4 or SGD_LOCUS:YEL001C','http://www.yeastgenome.org/','http://db.yeastgenome.org/cgi-bin/locus.pl?locus=','http://db.yeastgenome.org/cgi-bin/locus.pl?locus=YEL001C',NULL);
INSERT INTO goa_xdb VALUES (73,'SGD_REF',' Saccharomyces Genome Database.','Literature Reference Identifier.','SGD_REF:S000049602','http://www.yeastgenome.org/','http://db.yeastgenome.org/cgi-bin/reference/reference.pl?dbid=','http://db.yeastgenome.org/cgi-bin/reference/reference.pl?dbid=S000049602',NULL);
INSERT INTO goa_xdb VALUES (74,'SMD',' Stanford Microarray Databaseobject:example_id:url_syntax:',NULL,NULL,'http://genome-www.stanford.edu/microarray',NULL,NULL,NULL);
INSERT INTO goa_xdb VALUES (75,'SWISS-PROT',' Swiss-Prot protein database.','Accession number.','Swiss-Prot:P45867','http://ca.expasy.org/sprot/','http://www.expasy.ch/cgi-bin/sprot-search-ac?','http://www.expasy.ch/cgi-bin/sprot-search-ac?P45867',NULL);
INSERT INTO goa_xdb VALUES (76,'SWP',' Swiss-Prot protein database.','Accession number.','Swiss-Prot:P45867','http://ca.expasy.org/sprot/','http://www.expasy.ch/cgi-bin/sprot-search-ac?','http://www.expasy.ch/cgi-bin/sprot-search-ac?P45867',NULL);
INSERT INTO goa_xdb VALUES (77,'SP',' Swiss-Prot protein database.','Accession number.','Swiss-Prot:P45867','http://ca.expasy.org/sprot/','http://www.expasy.ch/cgi-bin/sprot-search-ac?','http://www.expasy.ch/cgi-bin/sprot-search-ac?P45867',NULL);
INSERT INTO goa_xdb VALUES (78,'SPTR',' The combination of SWISS-PROT and TrEMBL; includes TrEMBLnew.','Accession number.','SPTR:P45867','http://ca.expasy.org/sprot/','http://www.expasy.ch/cgi-bin/sprot-search-ac?','http://www.expasy.ch/cgi-bin/sprot-search-ac?P45867',NULL);
INSERT INTO goa_xdb VALUES (79,'SWALL',' The combination of SWISS-PROT and TrEMBL; includes TrEMBLnew.','Accession number.','SPTR:P45867','http://ca.expasy.org/sprot/','http://www.expasy.ch/cgi-bin/sprot-search-ac?','http://www.expasy.ch/cgi-bin/sprot-search-ac?P45867',NULL);
INSERT INTO goa_xdb VALUES (80,'SUBTILIST',' Bacillus subtilis Genome Sequence Project.url_syntax:','Accession number.','SUBTILISTG:BG11384','http://genolist.pasteur.fr/SubtiList/',NULL,NULL,NULL);
INSERT INTO goa_xdb VALUES (81,'SUBTILISTG',' Bacillus subtilis Genome Sequence Project.url_syntax:','Gene symbol.','SUBTILISTG:accC','http://genolist.pasteur.fr/SubtiList/',NULL,NULL,NULL);
INSERT INTO goa_xdb VALUES (82,'TAIR',' The Arabidopsis Information Resource.','Accession number.','TAIR:gene:2062713','http://www.arabidopsis.org/','http://www.arabidopsis.org/servlets/TairObject?accession=','http://www.arabidopsis.org/servlets/TairObject?accession=gene:2062713',NULL);
INSERT INTO goa_xdb VALUES (83,'taxon',' NCBI Taxman.url_syntax:','Identifier.','taxon:7227','http://www.ncbi.nlm.nih.gov/Taxonomy/taxonomyhome.html/',NULL,NULL,NULL);
INSERT INTO goa_xdb VALUES (84,'TC',' The Transport Protein Database.','Identifier.','TC:9.A.4.1.1','http://tcdb.ucsd.edu/tcdb/','http://tcdb.ucsd.edu/tcdb/tcprotein.php?substrate=','http://tcdb.ucsd.edu/tcdb/tcprotein.php?substrate=9.A.4.1.1',NULL);
INSERT INTO goa_xdb VALUES (85,'TIGR',' The Institute for Genomic Research.url_syntax:',NULL,NULL,'http://www.tigr.org/',NULL,NULL,NULL);
INSERT INTO goa_xdb VALUES (86,'TIGR_Ath1',' The Institute for Genomic Research, Arabidopsis thaliana database.','Accession.','TIGR_Ath1:At3g01440','http://www.tigr.org/tdb/e2k1/ath1/ath1.shtml','http://www.tigr.org/tigr-scripts/euk_manatee/shared/ORF_infopage.cgi?db=ath1&orf=','http://www.tigr.org/tigr-scripts/euk_manatee/shared/ORF_infopage.cgi?db=ath1&orf=At3g01440',NULL);
INSERT INTO goa_xdb VALUES (87,'TIGR_CMR',' The Institute for Genomic Research, Comprehensive Microbial Resource.','Locus.','TIGR_CMR:VCA0557','http://www.tigr.org/','http://www.tigr.org/tigr-scripts/CMR2/GenePage.spl?locus=','http://www.tigr.org/tigr-scripts/CMR2/GenePage.spl?locus=VCA0557',NULL);
INSERT INTO goa_xdb VALUES (88,'TIGR_EGAD',' The Institute for Genomic Research, EGAD database','Accession.','TIGR_EGAD:74462','http://www.tigr.org/','http://www.tigr.org/tigr-scripts/CMR2/ht_report.spl?prot_id=','http://www.tigr.org/tigr-scripts/CMR2/ht_report.spl?prot_id=74462',NULL);
INSERT INTO goa_xdb VALUES (89,'TIGR_GenProp',' The Institute for Genomic Research, Genome Properties','Accession.','TIGR_GenProp:GenProp0120','http://www.tigr.org/','http://www.tigr.org/tigr-scripts/CMR2/genome_property_def.spl?prop_acc=','http://www.tigr.org/tigr-scripts/CMR2/genome_property_def.spl?prop_acc=GenProp0120',NULL);
INSERT INTO goa_xdb VALUES (90,'TIGR_Pfa1',' The Institute for Genomic Research, Plasmodium falciparum database.','Accession.','TIGR_Pfa1:PFB0010w','http://www.tigr.org/tdb/e2k1/pfa1/pfa1.shtml','http://www.tigr.org/tigr-scripts/euk_manatee/shared/ORF_infopage.cgi?db=pfa1&orf=','http://www.tigr.org/tigr-scripts/euk_manatee/shared/ORF_infopage.cgi?db=pfa1&orf=PFB0010w',NULL);
INSERT INTO goa_xdb VALUES (91,'TIGR_REF',NULL,'reference locator','TIGR_REF:GO_ref','http://www.tigr.org/tdb/GO_REF/GO_REF.shtml',NULL,'http://www.tigr.org/tdb/GO_REF/GO_REF.shtml',NULL);
INSERT INTO goa_xdb VALUES (92,'TIGR_Tba1',' The Institute for Genomic Research, Trypanosoma brucei database.','Accession.','TIGR_Tba1:25N14.10','http://www.tigr.org/tdb/e2k1/tba1/','http://www.tigr.org/tigr-scripts/euk_manatee/shared/ORF_infopage.cgi?db=tba1&orf=','http://www.tigr.org/tigr-scripts/euk_manatee/shared/ORF_infopage.cgi?db=tba1&orf=25N14.10',NULL);
INSERT INTO goa_xdb VALUES (93,'TIGR_TIGRFAMS',' The Institute for Genomic Research, TIGRFAMs HMM collection.','Accession.','TIGR_TIGRFAMS:TIGR00254','http://www.tigr.org/','http://www.tigr.org/tigr-scripts/CMR2/hmm_report.spl?acc=','http://www.tigr.org/tigr-scripts/CMR2/hmm_report.spl?acc=TIGR00254',NULL);
INSERT INTO goa_xdb VALUES (94,'TIGR_TGI',' The Institute for Genomic Research, TIGR Gene Index.','TC accession.','TIGR_TGI:Cattle_TC123931','http://www.tigr.org/','http://www.tigr.org/tigr-scripts/nhgi_scripts/tc_report.pl?tc=?','http://www.tigr.org/tigr-scripts/nhgi_scripts/tc_report.pl?tc=Cattle_TC123931',NULL);
INSERT INTO goa_xdb VALUES (95,'TREMBL',' Computer-annotated protein sequence database supplementing SWISS-PROT and containing the translations of all coding sequences (CDS) present in the EMBL Nucleotide Sequence Database but not yet integrated in SWISS-PROT. Does not include TrEMBLnew.','Accession number.','TrEMBL:Q00177','http://ca.expasy.org/sprot/','http://www.expasy.ch/cgi-bin/sprot-search-ac?','http://www.expasy.ch/cgi-bin/sprot-search-ac?Q00177',NULL);
INSERT INTO goa_xdb VALUES (96,'TR',' Computer-annotated protein sequence database supplementing SWISS-PROT and containing the translations of all coding sequences (CDS) present in the EMBL Nucleotide Sequence Database but not yet integrated in SWISS-PROT. Does not include TrEMBLnew.','Accession number.','TrEMBL:Q00177','http://ca.expasy.org/sprot/','http://www.expasy.ch/cgi-bin/sprot-search-ac?','http://www.expasy.ch/cgi-bin/sprot-search-ac?Q00177',NULL);
INSERT INTO goa_xdb VALUES (97,'SPTREMBL',' Computer-annotated protein sequence database supplementing SWISS-PROT and containing the translations of all coding sequences (CDS) present in the EMBL Nucleotide Sequence Database but not yet integrated in SWISS-PROT. Does not include TrEMBLnew.','Accession number.','TrEMBL:Q00177','http://ca.expasy.org/sprot/','http://www.expasy.ch/cgi-bin/sprot-search-ac?','http://www.expasy.ch/cgi-bin/sprot-search-ac?Q00177',NULL);
INSERT INTO goa_xdb VALUES (98,'UM-BBD',' The University of Minnesota Biocatalysis/Biodegradation Database.example_id:url_syntax:','Prefix.',NULL,'http://umbbd.ahc.umn.edu/index.html',NULL,NULL,NULL);
INSERT INTO goa_xdb VALUES (99,'UM-BBD_enzymeID',' The University of Minnesota Biocatalysis/Biodegradation Database.','Enzyme identifier.','UM-BBD_enzymeID:e0413','http://umbbd.ahc.umn.edu/index.html','http://umbbd.ahc.umn.edu:8007/umbbd/servlet/pageservlet?ptype=ep&enzymeID=','http://umbbd.ahc.umn.edu:8007/umbbd/servlet/pageservlet?ptype=ep&enzymeID=e0413',NULL);
INSERT INTO goa_xdb VALUES (100,'UM-BBD_pathwayID',' The University of Minnesota Biocatalysis/Biodegradation Database.url_syntax:','Pathway identifier.','UM-BBD_pathwayID:acr','http://umbbd.ahc.umn.edu/index.html',NULL,'http://umbbd.ahc.umn.edu/acr/acr_map.html',NULL);
INSERT INTO goa_xdb VALUES (101,'UniParc',' UniProt Archive; a non-redundant archive of protein sequences extracted from Swiss-Prot, TrEMBL, PIR-PSD, EMBL, Ensembl, IPI, PDB, RefSeq, FlyBase, WormBase, European Patent Office, United States Patent and Trademark Office, and Japanese Patent Office.','Accession number.','UniParc:UPI000000000A.','http://www.ebi.ac.uk/uniparc/','http://www.ebi.ac.uk/cgi-bin/dbfetch?db=uniparc&id=','http://www.ebi.ac.uk/cgi-bin/dbfetch?db=uniparc&id=UPI000000000A',NULL);
INSERT INTO goa_xdb VALUES (102,'UniProt',' The Universal Protein Knowledgebase.','Accession number.','UniProt:P12345','http://www.uniprot.org','http://www.expasy.ch/cgi-bin/sprot-search-ac?','http://www.expasy.ch/cgi-bin/sprot-search-ac?P12345',NULL);
INSERT INTO goa_xdb VALUES (103,'VIDA',' Virus Database at University College London',NULL,NULL,'http://www.biochem.ucl.ac.uk/bsm/virus_database/VIDA.html',NULL, NULL, NULL);
INSERT INTO goa_xdb VALUES (104,'WB',' WormBase, database of nematode biology.','Gene symbol.','WB:lin-12','http://www.wormbase.org/','http://www.wormbase.org/db/gene/gene?name=','http://www.wormbase.org/db/get?class=Locus;name=lin-12',NULL);
INSERT INTO goa_xdb VALUES (105,'WB_REF',' WormBase, database of nematode biology.','Literature Reference Identifier.','WB_REF:cgc467','http://www.wormbase.org/','http://www.wormbase.org/db/misc/paper?name=','http://www.wormbase.org/db/misc/paper?name=%5Bcgc467%5D;class=Paper',NULL);
INSERT INTO goa_xdb VALUES (106,'WP',' Wormpep, database of proteins of C. elegans.','Identifier.','WP:CE25104','http://www.wormbase.org/','http://www.wormbase.org/db/get?class=Protein;name=','http://www.wormbase.org/db/get?class=Protein;name=WP%3ACE15104',NULL);
INSERT INTO goa_xdb VALUES (107,'ZFIN',' The Zebrafish Information Network','Accession ID','ZDB-GENE-990415-103','http://zfin.org/','http://zfin.org/cgi-bin/ZFIN_jump?record=','http://zfin.org/cgi-bin/ZFIN_jump?record=ZDB-GENE-990415-103',NULL);
INSERT INTO goa_xdb VALUES (108,'GOA','Gene Ontology Association','identifier','GO:123456','http://www.ebi.ac.uk/GOA/',NULL,NULL,NULL);
INSERT INTO goa_xdb VALUES (109,'PINC',NULL,NULL,NULL,NULL,NULL,NULL,NULL);
INSERT INTO goa_xdb VALUES (110,'DB','Dummy Database',NULL,NULL,NULL,NULL,NULL,NULL);
INSERT INTO goa_xdb VALUES (111,'RefSeq','NCBI RefSeq',NULL,NULL,NULL,NULL,NULL,NULL);
INSERT INTO goa_xdb VALUES (112,'GeneDB','GeneDB Wellcome Trust Sanger Institute',NULL,NULL,'http://www.genedb.org/',NULL,NULL,NULL);
INSERT INTO goa_xdb VALUES (113,'GENE','NCBI Entrez Gene',NULL,NULL,'http://www.ncbi.nlm.nih.gov/entrez/query.fcgi?db=gene','http://www.ncbi.nlm.nih.gov/entrez/query.fcgi?db=gene&cmd=Retrieve&dopt=Graphics&list_uids=',NULL,NULL);

