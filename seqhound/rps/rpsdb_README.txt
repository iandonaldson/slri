RPS Database
-----------------------
To identify conserved domains in a protein sequence we use CD-search to map functional domains. 
Domain profiles are taken from NCBI's Conserved Domain Database (CDD). CDD currently contains 
domains derived from two popular collections, Smart and Pfam, plus contributions from colleagues 
at NCBI (LOAD domains). The source database also provides description and links to citations as well 
as 3D structure when applicable.

Two tables were generated using the CDD and RPS-BLAST, RPSDB and DOMNAME. 
RPSDB contains precomputed RPS-BLAST domain annotation for most of the proteins 
in SeqHound REDUND table. Proteins that were not annotated in this table were the 
hypothetical ORF from RefSeq (XP_xxxxxx) and SWISS-PROT proteins. These proteins 
are not present in SeqHound although their GI's are in REDUND table. In each redundant 
group the first GI was used to for RPS-BLAST computing (ordinal 1). These proteins are 
considered to be the best representative of the redundant group. When the first protein 
in the redundant group does not have sequence information in seqhound, the rpsdb parser 
attempts to look up sequence information for the other GIs in the same redundant group.  
If such information is found, it is used as the sequence information for the first GI, 
if not, the entire redundant group is skipped.

In RPSDB, each domain identified by RPS-BLAST in a particular protein is stored 
as a separate record. The following is the RPSDB ASN.1 structure that reflects 
the table's fields:
StRpsNode ::= SEQUENCE {
 	gi INTEGER,
	dom-id VisibleString,
	from INTEGER,
	len INTEGER,
	evalue REAL,
 	n-missing INTEGER,
 	c-missing INTEGER,
 	num-of-dom INTEGER
}

DOMNAME keeps domain specific information parsed from *.csq and *.acd files 
(downloaded from ftp.ncbi.nlm.nih.gov/pub/mmdb/cdd/).

StDomNameNode ::=SEQUENCE {
	accession VisibleString,
	name VisibleString,
	mmdb-id INTEGER OPTIONAL,
	asn1 Cdd
}

RPSDB was computed on the Linux cluster using 95 machines.
The REDUND table was printed to a file using NonRedundList 
(redundlist) program that prints SeqHound's REDUND table to file. 
The list was then split into smaller files using 'splitFile.pl' perl script. 
Each file contained ~9000 GI's. 
RpsBlastToDB is the program that runs RPS-BLAST and stores the results 
in codebase tables. Each node stored the results in a separate table. 
The tables were then collected in pooled together into one MASTER table 
using MergeRpsTables. 
An E-value cutoff of 1.0 was employed, though this can be set to any value 
when one invokes the rpsdb parser.

API
A C and C++ API layer was implemented for RPSDB and DOMNAME. 
The C API is located in seqhound/include and seqhound/src directory. 
Currently (May 2002), RPSDB API is separate from seqhound.h API. This allows 
one to use RPSDB table locally in conjunction with either remote or local SeqHound. 
Alternatively, RPSDB can be incorporated as part of the SeqHound local or remote library. 
RPSDB functions are defined in both rpsdbapi.c (local) and seqhoundapi.c (remote).
TODO: To use RPSDB locally while linking to SeqHound remote library 
we wrap RPSDB function in seqhoundapi.c with #ifdef RPSDB_LOCAL defines. 
When building SeqHound remote library we can selectively remove RPSDB functions 
from the library using this define.
seqhound.h includes rpsdb.h so all RPSDB function declarations are visible regardless 
of the compilation model.


The C++ layer is implemented in seqhound/rps/
Both CodeBase and API layer were implemented in C++. 
TODO: There are a few API functions that were added to the C++ code 
and missing from the C which need to be implemented in C. 
Note that the C++ code uses a class called 'RpsDBRecord' rather than using 
the RpsDBRecordPtr declared in rpsdb_cb.hpp, similarly for DomnameDBRecord. 
A number of utility functions, mostly container printing and free call are 
written in slri_misc.hpp. This file should eventually be added to a new C++ slrilib.

There are also a number of test program to test the C/C++ API layer and 
remote calls (rpsquery). For the C API there are two programs, testapi 
and testrpsapi. 
Testrpsapi is used for testing RPSDB locally with local SeqHound while
 testapi is used to test RPSDBDB locally and SeqHound remote. 
Makefile.apitest is a makefile for the c++ API test.



 


