# # $Id: newSrchTables.sql,v 1.11 2004/11/29 16:14:48 kzheng Exp $

# ##########################################
#     SeqHound PreBIND Module tables
# ##########################################
use prebind;

CREATE TABLE IF NOT EXISTS db(
    dbid         INT NOT NULL AUTO_INCREMENT,
    name         CHAR(60) NOT NULL,
    PRIMARY KEY (dbid),
    UNIQUE(name)
) ;

INSERT into db VALUES( 1, "PubMed");
INSERT into db VALUES( 2, "PubMed Central");
INSERT into db VALUES( 3, "BioMed Central");

CREATE TABLE IF NOT EXISTS method(
    methodid         INT NOT NULL AUTO_INCREMENT,
    name             CHAR(60) NOT NULL,
    description      text,
    PRIMARY KEY (methodid),
    UNIQUE(name)
) ;
#-- docscore
INSERT into method VALUES(1, "SVM", "Support Vector Machine Text Classificaiton");
INSERT into method VALUES(11, "nrc", "judgement by nrc using SVM Classifier");
#-- resultscore
INSERT into method VALUES(2, "disambiguation", "for bionames, to be implemented");
#-- evidencescore
INSERT into method VALUES(3, "regex_score","scored regular expression patterns matching");
INSERT into method VALUES(31, "regex_#_senetences","scored regular expression patterns matching");
INSERT into method VALUES(32, "GO","scored regular expression patterns matching");

#-- pointscore
INSERT into method VALUES(4, "summary", "maximum of evidencescore" );

create table IF NOT EXISTS lexicon(
    word char(16), 
    pos char(10), #pos tag
    freq int,     # frequency per million word 
    count int,
    status char(20), # word status 'pubmed', 'stop', 'noalpha'
     
    Unique (word, pos),
    index(word), 
    index(pos), 
    index(freq),
    index(count),
    index(status)
);


CREATE TABLE IF NOT EXISTS doc(
    docid       INT NOT NULL AUTO_INCREMENT,
    dbid        INT NOT NULL,
    accession   INT NOT NULL,
    status      char(10),

    PRIMARY KEY (docid),
    INDEX (dbid),
    #FOREIGN KEY (dbid) REFERENCES db(dbid)
    #ON UPDATE CASCADE ON DELETE CASCADE,
    UNIQUE (dbid, accession), 
    index(accession),
    index(status)
) ;

CREATE TABLE IF NOT EXISTS docscore(
    docid        INT NOT NULL,
    methodid     INT NOT NULL , # SVM
    score        DOUBLE default NULL, # ISVM.iscore    

    PRIMARY KEY (docid, methodid, score),
    INDEX (docid),
    #FOREIGN KEY (docid) REFERENCES doc(docid)
    #ON UPDATE CASCADE ON DELETE CASCADE,
    INDEX (methodid),
    #FOREIGN KEY (methodid) REFERENCES method(methodid)
    #ON UPDATE CASCADE ON DELETE CASCADE,
    INDEX (score)
);


#-- redundant name group
#-- some names from bioname table are filtered out.
create table IF NOT EXISTS rng(
    rngid int auto_increment primary key, 
    name text not null, 
    taxid int not null,
    searched date,
    status char(20), # search status: 'illegal', 'stop', 'searched', 'updated'
    results int, 
    n_taxid int,
    n_bionameid int,
    n_bioentityid int, # these number are use for result scoring
    
    UNIQUE(name(60), taxid), # ignoring very long name
    index (searched),
    index(results),
    index(status),
    index(n_taxid),
    index(n_bionameid),
    index(n_bioentityid)
) ;

# Insert IGNORE into table rng(name, taxid) 
# select distinctrow a.name, c.taxid from bioname a, bioentityid b, taxgi c 
# where a.action='1' and a.nametypeid=2 and a.bioentityid=b.id and b.identifier=c.gi and ;

#update rng set status='nonalnum' where name NOT REGEXP "[[:alnum:]]+"
#update rng set status='number' where name REGEXP "^[[:digit:]]+$"; 
#update rng a, lexicon b set a.status=b.status where a.name=b.word and a.status is NULL;
#update rng  set n_taxid= count(taxid) where group by name, taxid;

create table IF NOT EXISTS rngresult(
    rngid int,
    docid int,
    pmid int,
    state        INT not null default 0, 

    index(rngid),
    #FOREIGN KEY (rngid) REFERENCES rng(rngid)
    #ON DELETE CASCADE,
    index(docid),
    index(pmid),
    index(state)
);

CREATE TABLE IF NOT EXISTS search(
    searchid        INT NOT NULL AUTO_INCREMENT,
    bioentityid     INT NOT NULL,
    bionameid       INT NOT NULL,
    name            text not null,
    taxid           INT NOT NULL,
    rngid           INT, # can be null because some names are illegal in rng
    searched        DATETIME NOT NULL default 0,
    results         INT,
    
    PRIMARY KEY (searchid),
    UNIQUE (bionameid),
    #FOREIGN KEY (bionameid) REFERENCES bioname(id)
    #ON UPDATE CASCADE ON DELETE CASCADE,
    INDEX (bioentityid),
    INDEX(name(20)),
    INDEX (taxid),
    INDEX(rngid),
    INDEX(searched),
    INDEX (results)
) ;

CREATE TABLE IF NOT EXISTS result(
    resultid      INT NOT NULL AUTO_INCREMENT,
    searchid      INT NOT NULL,
    bioentityid   INT NOT NULL,
    taxid        INT not null,
    state        INT not null default 0, 
    rngid         INT NOT NULL,
    docid         INT NOT NULL,
    positionid    INT NOT NULL default 0, # 0 means anywhere in doc
    
    PRIMARY KEY (resultid),
    UNIQUE(searchid, docid, positionid),
    INDEX (searchid),
    #FOREIGN KEY (searchid) REFERENCES search(searchid)
    #ON UPDATE CASCADE ON DELETE CASCADE,
    index (bioentityid),
    index (taxid),
    index (state),
    index (rngid),
    INDEX (docid),
    #FOREIGN KEY (docid) REFERENCES doc(docid)
    #ON UPDATE CASCADE ON DELETE CASCADE,
    INDEX (positionid)

) ;


# for name (recognition) disambiguation methods
CREATE TABLE IF NOT EXISTS resultscore(
    resultid      INT NOT NULL,
    methodid      INT NOT NULL, # name disambiguation
    score         INT default NULL,

    PRIMARY KEY(resultid, methodid, score),
   
    INDEX (resultid),
    #FOREIGN KEY (resultid) REFERENCES result(resultid)
    #ON UPDATE CASCADE ON DELETE CASCADE,
    INDEX (methodid),
    #FOREIGN KEY (methodid) REFERENCES method(methodid)
    #ON UPDATE CASCADE ON DELETE CASCADE,
    INDEX (score)

) ;


CREATE TABLE IF NOT EXISTS point(
    pointid       INT NOT NULL AUTO_INCREMENT,
    bioentityidA  INT NOT NULL,
    bioentityidB  INT NOT NULL, #check bioentityidA<bioentityidB
    
    PRIMARY KEY(pointid),
    UNIQUE(bioentityidA, bioentityidB),
    INDEX (bioentityidA),
    #FOREIGN KEY (bioentityidA) REFERENCES bioentity(id)
    #ON UPDATE CASCADE ON DELETE CASCADE,
    INDEX (bioentityidB)
    #FOREIGN KEY (bioentityidB) REFERENCES bioentity(id)
    #ON UPDATE CASCADE ON DELETE CASCADE,
) ;


CREATE TABLE IF NOT EXISTS pointscore(
    pointid       INT NOT NULL,
    methodid      INT NOT NULL, # summary:  max of evidence score 
    score         DOUBLE default NULL,

    PRIMARY KEY (pointid, methodid, score),
    INDEX (pointid),
    #FOREIGN KEY (pointid) REFERENCES point(pointid)
    #ON UPDATE CASCADE ON DELETE CASCADE,
    INDEX (methodid),
    #FOREIGN KEY (methodid) REFERENCES method(methodid)
    #ON UPDATE CASCADE ON DELETE CASCADE,
    INDEX (score)

) ;



CREATE TABLE IF NOT EXISTS evidence(
    evidenceid    INT NOT NULL AUTO_INCREMENT,
    resultidA     INT NOT NULL, 
    bioentityida  int not null,
    resultidB     INT NOT NULL,  # check  resultidA<resultidB 
    bioentityidb  int not null,
    docid         INT NOT NULL, 
    pointid       INT NOT NULL, 

    PRIMARY KEY(evidenceid),
    UNIQUE(resultidA, resultidB), 
    index (resultidA),
    #FOREIGN KEY (resultidA) REFERENCES result(resultid)
    #ON UPDATE CASCADE ON DELETE CASCADE,
    index (resultidB),
    #FOREIGN KEY (resultidB) REFERENCES result(resultid)
    #ON UPDATE CASCADE ON DELETE CASCADE,
    index (bioentityida),
    index (bioentityidb),
    index (docid),
    #FOREIGN KEY (docid) REFERENCES doc(docid)
    #ON UPDATE CASCADE ON DELETE CASCADE,
    INDEX (pointid)
    #FOREIGN KEY (pointid) REFERENCES point(pointid)
    #ON UPDATE CASCADE ON DELETE CASCADE

) ;

CREATE TABLE IF NOT EXISTS evidencescore(
    evidenceid    INT NOT NULL,
    methodid      INT NOT NULL, # regex
    score         DOUBLE default NULL,

    PRIMARY KEY (evidenceid, methodid, score),
    INDEX (evidenceid),
    #FOREIGN KEY (evidenceid) REFERENCES evidence(evidenceid)
    #ON UPDATE CASCADE ON DELETE CASCADE,
    INDEX (methodid),
    #FOREIGN KEY (methodid) REFERENCES method(methodid)
    #ON UPDATE CASCADE ON DELETE CASCADE,
    INDEX (score)
) ;

       

# $Log $

