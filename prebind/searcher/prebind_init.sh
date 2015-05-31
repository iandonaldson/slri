# first import existing/precomputed tables
mysql seqhound < ./lexicon.sql
mysql seqhound < ./rng.sql
mysql seqhound < ./rngresult.sql
mysql seqhound < ./doc.sql
mysql seqhound < ./docscore.sql

#then create non-existing tables
mysql seqhound < newSrchTables.sql
#mysql seqhound < prebind.sql
