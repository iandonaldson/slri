# $Id$

# ##########################################
#     SeqHound PreBIND Module tables
# ##########################################
CREATE TABLE IF NOT EXISTS text_db(
    id         INT NOT NULL AUTO_INCREMENT,
    name         CHAR(80) NOT NULL,
    PRIMARY KEY (id),
    unique u_name (name)
)TYPE=INNODB ;

CREATE TABLE IF NOT EXISTS text_method(
    id int auto_increment primary key, 
    type char(30), 
    hypothid int, 
    hypoth text,
    description text, 
    positive text,
    negative text,
    undecided text,
    assume text,
    implemented enum('YES','NO'),
    script text
)TYPE=INNODB ;

CREATE TABLE IF NOT EXISTS text_hypoth(
    id INT NOT NULL,
    hypothesis text
)TYPE=INNODB;

CREATE TABLE IF NOT EXISTS text_doc(
    id       INT NOT NULL AUTO_INCREMENT primary key,
    dbid        INT NOT NULL,
    accession   INT NOT NULL,
    state      smallint,

    INDEX (dbid),
    FOREIGN KEY (dbid) REFERENCES text_db(id) 
    ON UPDATE CASCADE ON DELETE CASCADE,
    unique u_dbid_accession (dbid, accession),
    index(accession)
)TYPE=INNODB ;

CREATE TABLE IF NOT EXISTS text_docscore(
    docid        INT NOT NULL,
    methodid     INT NOT NULL , # SVM
    score        DOUBLE default NULL, # ISVM.iscore

    PRIMARY KEY (docid, methodid),
    INDEX (docid),
    FOREIGN KEY (docid) REFERENCES text_doc(id) 
    ON UPDATE CASCADE ON DELETE CASCADE,
    INDEX (methodid),
    FOREIGN KEY (methodid) REFERENCES text_method(id) 
    ON UPDATE CASCADE ON DELETE CASCADE,
    INDEX (score)
)TYPE=INNODB;


CREATE TABLE IF NOT EXISTS text_organism(
    id              INT not null Auto_increment primary key,
    taxid           INT NOT NULL,
    mesh            text NOT NULL,
    searched        DATETIME,
    results         INT,
    bioentities     INT,
    bionames        INT,
    maxbionameid    INT,
    lastupdate      DATETIME,
    unique u_taxid (taxid)
)TYPE=INNODB;

CREATE TABLE IF NOT EXISTS text_doctax(
    id          INT not null Auto_increment primary key,
    docid       INT NOT NULL,
    taxid       INT NOT NULL,
    unique u_docid_taxid (docid, taxid),
    index(docid),
    index(taxid)
)TYPE=INNODB;

CREATE TABLE IF NOT EXISTS text_englishdict(
    id              INT not null Auto_increment primary key,
    word            CHAR(200),
    pos             CHAR(10), # pos tag
    freq            double,     # frequency per million word
    count           int,  # number of occurrenct as bionames
    status          char(200) default ' ',

    unique(word),
    index(pos),
    index(freq),
    index(count),
    index(status)
)TYPE=INNODB;

CREATE TABLE IF NOT EXISTS text_stopword(
    id              INT not null Auto_increment primary key,
    word            CHAR(16) not null,
    source          enum('pubmed','smart', 'aminoacid', 'greek','blueprint'),
    index(word)
)TYPE=INNODB;

CREATE TABLE IF NOT EXISTS text_bncorpus(
    id          INT not null Auto_increment primary key,
    word        char(160) not null, 
    freq        int not null, 
    pos         char(16) not null, 
    files       int not null, 
    index(freq),
    index(word),
    index(pos),
    index(files)
)TYPE=INNODB;


-- redundant name group
#-- some names from bioname table are filtered out.
CREATE TABLE IF NOT EXISTS text_rng(
    id INT not null auto_increment primary key,
    name CHAR(240) not null, #-- CHAR is way faster than varchar and text
    searched DATETIME,
    results int,
    state smallint,

    unique u_name (name),
    index (searched),
    index(results),
    index(state)
)TYPE=INNODB;

CREATE TABLE IF NOT EXISTS text_rngresult(
    id int not null auto_increment primary key,
    rngid int not null,
    docid int not null,
    pmid int,
    matches int,
    state smallint,

    unique u_rngid_docid(rngid, docid),
    index(rngid),
    index(docid),
    index(pmid),
    index(state)
)TYPE=INNODB;

CREATE TABLE IF NOT EXISTS text_search(
    id        INT NOT NULL AUTO_INCREMENT primary key,
    bioentityid     INT NOT NULL,
    bionameid       INT NOT NULL,
    name            CHAR(240) not null,
    taxid           INT NOT NULL,
    rngid           INT,
    method          INT,
    searched        DATETIME,
    results         INT,
    state           smallint,
    unique u_bionameid (bionameid),
    INDEX (bioentityid),
    INDEX(name(30)),
    INDEX (taxid),
    INDEX (rngid),
    INDEX(searched),
    INDEX (results),
    index (state)
)TYPE=INNODB ;

CREATE TABLE IF NOT EXISTS text_searchscore(
    searchid int not null,
    methodid int,
    score double,

    PRIMARY KEY (searchid, methodid),
    index(searchid),
    FOREIGN KEY (searchid) REFERENCES text_search(id) 
    ON UPDATE CASCADE ON DELETE CASCADE,
    index(methodid),
    index(score)
)TYPE=INNODB;

CREATE TABLE IF NOT EXISTS text_result(
    id      INT NOT NULL AUTO_INCREMENT primary key,
    searchid      INT NOT NULL,
    docid         INT NOT NULL,
    positionid    INT NOT NULL default 0, # 0 means anywhere in doc
    state         smallint,

    unique u_searchid_docid_positionid (searchid, docid, positionid),
    INDEX (searchid),
    INDEX (docid),
    FOREIGN KEY (docid) REFERENCES text_doc(id) 
    ON UPDATE CASCADE ON DELETE CASCADE,
    INDEX (positionid),
    index(state)

)TYPE=INNODB ;


# for name (recognition) disambiguation methods
CREATE TABLE IF NOT EXISTS text_resultscore(
    resultid      INT NOT NULL,
    methodid      INT NOT NULL, # name disambiguation
    score         INT default NULL,

    PRIMARY KEY(resultid, methodid),
    INDEX (resultid),
    FOREIGN KEY (resultid) REFERENCES text_result(id)
    ON UPDATE CASCADE ON DELETE CASCADE,
    INDEX (methodid),
    INDEX (score)

)TYPE=INNODB ;


CREATE TABLE IF NOT EXISTS text_point(
    id              INT NOT NULL AUTO_INCREMENT primary key,
    bioentityidA    INT NOT NULL,
    bioentityidB    INT NOT NULL,
    state       smallint,

    unique u_bioentitytidA_bioentityidB (bioentityidA, bioentityidB),
    index(state)
)TYPE=INNODB ;
#-- constraint:
#-- bioentityidA<bioentityidB

CREATE TABLE IF NOT EXISTS text_pointscore(
    pointid       INT NOT NULL,
    methodid      INT NOT NULL, 
    score         DOUBLE default NULL,

    PRIMARY KEY (pointid, methodid),
    INDEX (pointid),
    FOREIGN KEY (pointid) REFERENCES text_point(id)
    ON UPDATE CASCADE ON DELETE CASCADE,
    INDEX (methodid),
    INDEX (score)

)TYPE=INNODB ;


CREATE TABLE IF NOT EXISTS text_evidence(
    id          INT NOT NULL AUTO_INCREMENT primary key,
    docid       INT NOT NULL,
    resultidA   INT NOT NULL,
    bioentityida  INT not null,
    nama          CHAR(240) not null,
    resultidB     INT NOT NULL,
    bioentityidb  INT not null,
    namb          CHAR(240) not null,
    pointid       INT,
    namepairid    INT,
    state        smallint,

    index (docid, nama, namb),
    unique u_resultidA_resultidB (resultidA, resultidB),
    index (resultidA),
    FOREIGN KEY (resultidA) REFERENCES text_result(id) 
    ON UPDATE CASCADE ON DELETE CASCADE,index (resultidB),
    index (resultidB),
    FOREIGN KEY (resultidB) REFERENCES text_result(id) 
    ON UPDATE CASCADE ON DELETE CASCADE,
    index (bioentityida, bioentityidb),
    INDEX (pointid),
    index (namepairid),
    index(state)
)TYPE=INNODB ;
#-- constraint:
#-- resultidA<resultidB


CREATE TABLE IF NOT EXISTS text_evidencescore(
    evidenceid    INT NOT NULL,
    methodid      INT NOT NULL,
    score         DOUBLE default NULL,

    PRIMARY KEY (evidenceid, methodid),
    INDEX (evidenceid),
    # FOREIGN KEY (evidenceid) REFERENCES text_evidence(id)
    # ON UPDATE CASCADE ON DELETE CASCADE,
    INDEX (methodid),
    #FOREIGN KEY (methodid) REFERENCES text_method(id)
    #ON UPDATE CASCADE ON DELETE CASCADE,
    INDEX (score)
) TYPE=INNODB;

CREATE TABLE IF NOT EXISTS text_pattern(
    id          CHAR(8) not null primary key,
    parentid    CHAR(8),
    class INT not null,
    score DOUBLE,
    regex TEXT,

    index(id),
    index(class),
    index(score),
    index(regex(80))
)TYPE=INNODB ;



CREATE TABLE IF NOT EXISTS text_namepair(
    id INT not null auto_increment primary key,
    docid INT NOT NULL,
    pmid INT ,
    nama CHAR(240) not null,
    namb CHAR(240) not null,
    state smallint,

    unique u_docid_nama_namb(docid, nama, namb),
    index(docid),
    index(nama),
    index(namb),
    index(state)
) TYPE=INNODB;
#-- constraint:
#-- nama <namb


CREATE TABLE IF NOT EXISTS text_namepairresult(
    namepairid  INT not null,
    patternid   CHAR(8) not null,
    matches     INT,
    unique u_namepairid_patternid (namepairid, patternid),
    index(matches)
)TYPE=INNODB ;



CREATE TABLE IF NOT EXISTS text_namepairscore(
    namepairid    INT NOT NULL,
    methodid      INT NOT NULL,
    score         DOUBLE default NULL,

    PRIMARY KEY (namepairid, methodid),
    INDEX (namepairid),
    # FOREIGN KEY (namepairid) REFERENCES text_namepair(id)
    # ON UPDATE CASCADE ON DELETE CASCADE,
    INDEX (methodid),
    # FOREIGN KEY (methodid) REFERENCES text_method(id)
    # ON UPDATE CASCADE ON DELETE CASCADE,
    INDEX (score)
) TYPE=INNODB;

#-- state ENUM('ignored', 'searched', 'scored', 'updated', 'modified', 'deleted'),
