CREATE DATABASE bind_module;
USE bind_module;

/*bmd_source, bmd_record_types, bmd_dbases*/
CREATE TABLE bmd_source (uid INTEGER PRIMARY KEY NOT NULL AUTO_INCREMENT, db SMALLINT NOT NULL, acc VARCHAR(10), id INTEGER, type SMALLINT NOT NULL, data_blob LONGBLOB, data_clob LONGTEXT); 
CREATE TABLE bmd_record_types (type SMALLINT PRIMARY KEY NOT NULL AUTO_INCREMENT, type_name VARCHAR(20) NOT NULL);
CREATE TABLE bmd_dbases (db SMALLINT PRIMARY KEY NOT NULL AUTO_INCREMENT, db_name VARCHAR(30) NOT NULL);

INSERT INTO bmd_dbases (db_name) VALUES('BIND');
INSERT INTO bmd_record_types (type_name) VALUES('interaction');
INSERT INTO bmd_record_types (type_name) VALUES('complex');

/*bmd_ints*/
CREATE TABLE bmd_ints (uid INTEGER PRIMARY KEY NOT NULL, objAid INTEGER NOT NULL, objBid INTEGER NOT NULL, rig INTEGER NOT NULL, descr TEXT);

/*bmd_objects, bmd_mol_type, bmd_obj_dbases*/
CREATE TABLE bmd_objects (objid INTEGER PRIMARY KEY NOT NULL AUTO_INCREMENT, type SMALLINT NOT NULL, db SMALLINT NOT NULL, id INTEGER, tax INTEGER, acc VARCHAR(20) NOT NULL, label TEXT, rog INTEGER NOT NULL);
CREATE TABLE bmd_mol_type (type SMALLINT PRIMARY KEY NOT NULL AUTO_INCREMENT, type_name VARCHAR(15) NOT NULL);
CREATE TABLE bmd_obj_dbases (db SMALLINT PRIMARY KEY NOT NULL AUTO_INCREMENT, db_name VARCHAR(30) NOT NULL);

INSERT INTO bmd_mol_type (type_name) VALUES('unknown');
INSERT INTO bmd_mol_type (type_name) VALUES('protein');
INSERT INTO bmd_mol_type (type_name) VALUES('DNA');
INSERT INTO bmd_mol_type (type_name) VALUES('RNA');
INSERT INTO bmd_mol_type (type_name) VALUES('small-molecule');
INSERT INTO bmd_mol_type (type_name) VALUES('complex');
INSERT INTO bmd_mol_type (type_name) VALUES('gene');
INSERT INTO bmd_mol_type (type_name) VALUES('photon');

INSERT INTO bmd_obj_dbases (db_name) VALUES('GenBank');
INSERT INTO bmd_obj_dbases (db_name) VALUES('BIND');
INSERT INTO bmd_obj_dbases (db_name) VALUES('LIGAND');
INSERT INTO bmd_obj_dbases (db_name) VALUES('CHEMICAL ABSTRACTS SERVICE');
INSERT INTO bmd_obj_dbases (db_name) VALUES('ICCB');
INSERT INTO bmd_obj_dbases (db_name) VALUES('KLOTHO');
INSERT INTO bmd_obj_dbases (db_name) VALUES('LIGAND, CAS');
INSERT INTO bmd_obj_dbases (db_name) VALUES('HTTP://ICCB.MED.HARVARD.EDU.');
INSERT INTO bmd_obj_dbases (db_name) VALUES('CAS');
INSERT INTO bmd_obj_dbases (db_name) VALUES('EMBL-EBI MSD');

INSERT INTO bmd_objects (type, db, acc, id, tax, label, rog) SELECT type, db, '-', 0, 0, '', 0 FROM bmd_mol_type a, bmd_obj_dbases b WHERE a.type_name = 'unknown' AND b.db_name = "BIND";

/*bmd_refs, bmd_exp_methods, bmd_refs_db*/
CREATE TABLE bmd_refs (uid INTEGER PRIMARY KEY NOT NULL, db SMALLINT NOT NULL, acc VARCHAR(15), id INTEGER NOT NULL, method SMALLINT NOT NULL);
CREATE TABLE bmd_exp_methods (method SMALLINT PRIMARY KEY NOT NULL AUTO_INCREMENT, method_descr VARCHAR(40));
CREATE TABLE bmd_refs_db (db SMALLINT PRIMARY KEY NOT NULL AUTO_INCREMENT, db_name VARCHAR(10));

INSERT INTO bmd_refs_db (db_name) VALUES('PubMed');

INSERT INTO bmd_exp_methods (method_descr) VALUES('not-specified');
INSERT INTO bmd_exp_methods (method_descr) VALUES('alanine-scanning');
INSERT INTO bmd_exp_methods (method_descr) VALUES('affinity-chromatography');
INSERT INTO bmd_exp_methods (method_descr) VALUES('atomic-force-microscopy');
INSERT INTO bmd_exp_methods (method_descr) VALUES('autoradiography');
INSERT INTO bmd_exp_methods (method_descr) VALUES('competition-binding');
INSERT INTO bmd_exp_methods (method_descr) VALUES('cross-linking');
INSERT INTO bmd_exp_methods (method_descr) VALUES('deuterium-hydrogen-exchange');
INSERT INTO bmd_exp_methods (method_descr) VALUES('electron-microscopy');
INSERT INTO bmd_exp_methods (method_descr) VALUES('electron-spin-resonance');
INSERT INTO bmd_exp_methods (method_descr) VALUES('elisa');
INSERT INTO bmd_exp_methods (method_descr) VALUES('equilibrium-dialysis');
INSERT INTO bmd_exp_methods (method_descr) VALUES('fluorescence-anisotropy');
INSERT INTO bmd_exp_methods (method_descr) VALUES('footprinting');
INSERT INTO bmd_exp_methods (method_descr) VALUES('gel-retardation-assays');
INSERT INTO bmd_exp_methods (method_descr) VALUES('gel-filtration-chromatography');
INSERT INTO bmd_exp_methods (method_descr) VALUES('hybridization');
INSERT INTO bmd_exp_methods (method_descr) VALUES('immunoblotting');
INSERT INTO bmd_exp_methods (method_descr) VALUES('immunoprecipitation');
INSERT INTO bmd_exp_methods (method_descr) VALUES('immunostaining');
INSERT INTO bmd_exp_methods (method_descr) VALUES('interaction-adhesion-assay');
INSERT INTO bmd_exp_methods (method_descr) VALUES('light-scattering');
INSERT INTO bmd_exp_methods (method_descr) VALUES('mass-spectrometry');
INSERT INTO bmd_exp_methods (method_descr) VALUES('membrane-filtration');
INSERT INTO bmd_exp_methods (method_descr) VALUES('monoclonal-antibody-blockade');
INSERT INTO bmd_exp_methods (method_descr) VALUES('nuclear-translocation-assay');
INSERT INTO bmd_exp_methods (method_descr) VALUES('phage-display');
INSERT INTO bmd_exp_methods (method_descr) VALUES('reconstitution');
INSERT INTO bmd_exp_methods (method_descr) VALUES('resonance-energy-transfer');
INSERT INTO bmd_exp_methods (method_descr) VALUES('site-directed-mutagenesis');
INSERT INTO bmd_exp_methods (method_descr) VALUES('sucrose-gradient-sedimentation');
INSERT INTO bmd_exp_methods (method_descr) VALUES('surface-plasmon-resonance-chip');
INSERT INTO bmd_exp_methods (method_descr) VALUES('transient-coexpression');
INSERT INTO bmd_exp_methods (method_descr) VALUES('three-dimensional-structure');
INSERT INTO bmd_exp_methods (method_descr) VALUES('two-hybrid-test');
INSERT INTO bmd_exp_methods (method_descr) VALUES('allele-specific-complementation');
INSERT INTO bmd_exp_methods (method_descr) VALUES('far-western');
INSERT INTO bmd_exp_methods (method_descr) VALUES('colocalization');
INSERT INTO bmd_exp_methods (method_descr) VALUES('other');

/*bmd_complexes, bmd_complex2ints, bmd_complex2subunits*/
CREATE TABLE bmd_complexes (uid INTEGER PRIMARY KEY NOT NULL, numsubunits INTEGER NOT NULL, descr TEXT);
CREATE TABLE bmd_complex2ints (uid_complex INTEGER NOT NULL, uid_ints INTEGER NOT NULL);
CREATE TABLE bmd_complex2subunits (uid INTEGER PRIMARY KEY NOT NULL, objid INTEGER NOT NULL);




