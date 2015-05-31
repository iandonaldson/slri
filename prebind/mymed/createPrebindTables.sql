-- $Id: createPrebindTables.sql,v 1.3 2004/06/09 20:53:29 kzheng Exp $
-- ##############################################
-- 
--  import CodeBase PreBIND tables into MySQL
--
--###############################################


--create tables from refseq 

-- DROP TABLE IF EXISTS ACC;
CREATE TABLE IF NOT EXISTS ACC(
    accession   CHAR(20) NOT NULL, 
    gi          INT NOT NULL,
    db          CHAR(20) NOT NULL,
    locus       CHAR(40)  BINARY NOT NULL,
    definition  TEXT,
    taxid       INT NOT NULL,
    pprod       TEXT,
    updated     DATE NOT NULL,
    
    PRIMARY KEY (accession),   
    INDEX   gi_tag (gi),
    INDEX   rsq_tag (db(5))
);


-- DROP TABLE IF EXISTS DB;
CREATE TABLE IF NOT EXISTS DB(
    gi_nuc      INT NOT NULL,
    begin       INT NOT NULL,
    to_loc      INT NOT NULL,
    gi_prot     INT NOT NULL,
    dbname      CHAR(20) NOT NULL,
    idint       INT,
    idstr       CHAR(20),
    
    PRIMARY KEY (gi_nuc, gi_prot, dbname, idint, idstr),
    INDEX   gin_tag (gi_nuc),
    INDEX   gip_tag (gi_prot),
    INDEX   idint_tag (idint),       
    INDEX   idstr_tag (idstr),
);


CREATE TABLE IF NOT EXISTS CDR(
    gi_nuc      INT NOT NULL, 
    from_loc    INT NOT NULL,
    to_loc      INT NOT NULL,
    gi_prot     INT NOT NULL,

    INDEX   gin_tag (gi_nuc),
    INDEX   gip_tag (gi_prot)
);

CREATE TABLE IF NOT EXISTS LOCUS(
    gi_nuc      INT NOT NULL,
    from_loc    INT NOT NULL,
    to_loc      INT NOT NULL,
    name        CHAR(40) BINARY NOT NULL,
--    gi_prot     INT NOT NULL,
    gi_prot     INT,
	
	PRIMARY KEY (gi_nuc, from_loc, name),
    INDEX   gin_tag (gi_nuc),
    INDEX   gip_tag (gi_prot),
);

CREATE TABLE IF NOT EXISTS LOSYN(
    gi_nuc      INT NOT NULL,
    from_loc    INT NOT NULL,
    to_loc      INT NOT NULL,
    syn         CHAR(40) BINARY NOT NULL,
    gi_prot     INT NOT NULL,
    
    PRIMARY KEY  (gi_nuc, from_loc, syn),
    INDEX   gin_tag (gi_nuc),
    INDEX   gip_tag (gi_prot)
);


CREATE TABLE IF NOT EXISTS YENA(
    gi_prot     INT NOT NULL,
    name        CHAR(40) BINARY NOT NULL,

    PRIMARY KEY (gi_prot, name),
    INDEX   gip_tag (gi_prot)
);


CREATE TABLE IF NOT EXISTS SRCH(
    gi_prot     INT NOT NULL,
    name        CHAR(40)  BINARY NOT NULL,
    source      INT NOT NULL,
    taxid       INT NOT NULL,
    searched    DATE,
    status      ENUM('1', '2', '3'),
    namered     TINYINT NOT NULL,
    results     INT NOT NULL,

    PRIMARY KEY (gi_prot, name),
    INDEX   gip_tag (gi_prot),
    INDEX   name_tag (name)
);


CREATE TABLE IF NOT EXISTS RESULT (
    gi_prot     INT NOT NULL, 
	name        CHAR(40) BINARY NOT NULL, 
	pmid        INT NOT NULL,
    
	PRIMARY KEY (gi_prot, name, pmid),
	INDEX (gi_prot),
	INDEX (name),
	INDEX (pmid)
	
);

CREATE TABLE IF NOT EXISTS CORE(
    pmid        INT NOT NULL,
    muid        INT NOT NULL default 0,
    numn        INT NOT NULL,
    numg        INT NOT NULL,

    PRIMARY KEY (pmid),
    INDEX   pmid_tag (pmid),
    INDEX   muid_tag (muid),
    INDEX   numn_tag (numn),
    INDEX   numg_tag (numg)
);


CREATE TABLE IF NOT EXISTS REGI(
    pmid        INT NOT NULL,
    muid        INT NOT NULL DEFAULT 0,
    gip         INT NOT NULL,

    primary key (pmid, gip),
    INDEX pmid_tag (pmid),
    INDEX gip_tag (gip)
);


CREATE TABLE IF NOT EXISTS ISVM(
    pmid        INT NOT NULL,
    muid        INT NOT NULL ,
    iscore      INT NOT NULL ,
    iscore2     INT NOT NULL ,
    opno        INT NOT NULL ,
    opyes       INT NOT NULL ,
    itruth      INT NOT NULL ,

    PRIMARY KEY (pmid),
    INDEX muid_tag (muid)
);

CREATE TABLE IF NOT EXISTS POINT(
    pmid        INT NOT NULL,
    muid        INT NOT NULL DEFAULT 0,
    gia         INT NOT NULL,
    nama        CHAR(60) BINARY NOT NULL,
    atype       ENUM('1', '2'),
    gib         INT NOT NULL,
    namb        CHAR(60) BINARY NOT NULL,
    btype       ENUM('1', '2'),
    opno        INT NOT NULL,
    opyes       INT NOT NULL,
    piscore     INT NOT NULL,
    piscore2    INT NOT NULL,
    pitruth     INT NOT NULL,
    taxid       INT NOT NULL,
    
    INDEX pmid_tag  (pmid),
    INDEX muid_tag  (muid),
    INDEX gia_tag  (gia),
    INDEX gib_tag  (gib),
    INDEX piscore_tag  (piscore),
    INDEX pitruth_tag  (pitruth)
);

CREATE TABLE IF NOT EXISTS SUBMIT(
	pmid 		INT NOT NULL,
	muid		INT NOT NULL,
	gia			INT NOT NULL,
	gib			INT NOT NULL,
	sgia		INT,
	nama		CHAR(60) NOT NULL,
	adesc		TEXT,
	atype		ENUM('1', '2'),
	aok			ENUM('1', '0'),
	afrom		INT,
	ato			INT,
	sgib		INT,
	namb		CHAR(60),
	bdesc		TEXT,
	btype		ENUM('1', '2'),
	bok 		ENUM('1', '0'),		
	bfrom		INT,
	bto			INT,
	method		INT,
	opin		ENUM('1', '0'),
	fname		CHAR(20),
	lname		CHAR(20),
	buid		INT,
	date		DATE,
	status		INT,

	INDEX (pmid),
	INDEX (muid),
	INDEX (gia),
	INDEX (gib),
	INDEX (sgia),
	INDEX (nama),
	INDEX (sgib),
	INDEX (namb),
	INDEX (fname),
	INDEX (lname),
	INDEX (date),
	INDEX (status)
);


-- $Log: createPrebindTables.sql,v $
-- Revision 1.3  2004/06/09 20:53:29  kzheng
-- clean up
--
