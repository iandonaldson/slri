-- This CLP file was created using DB2LOOK Version 7.1
-- Timestamp: Mon Aug 12 10:43:36 EDT 2002
-- Database Name: SEQHOUND       
-- Database Manager Version: DB2/6000 Version 7.2.0        
-- Database Codepage: 819


CONNECT TO SEQHOUND;

------------------------------------
-- DDL Statements for TABLESPACES --
------------------------------------

CREATE REGULAR TABLESPACE USERSPACE2 IN NODEGROUP IBMDEFAULTGROUP PAGESIZE 4096 MANAGED BY SYSTEM 
	 USING ('/seqh/dp1/userspace2',
		'/seqh/dp2/userspace2',
		'/seqh/dp3/userspace2',
		'/seqh/dp4/userspace2')
	 EXTENTSIZE 32
	 PREFETCHSIZE 64
	 BUFFERPOOL IBMDEFAULTBP
	 OVERHEAD 24.100000
	 TRANSFERRATE 0.900000;

CREATE REGULAR TABLESPACE SMALL_TBSP IN NODEGROUP IBMDEFAULTGROUP PAGESIZE 4096 MANAGED BY DATABASE 
	 USING (FILE '/seqh/dp2/data/small/SMALL1.data'23100)
	 EXTENTSIZE 32
	 PREFETCHSIZE 64
	 BUFFERPOOL IBMDEFAULTBP
	 OVERHEAD 24.100000
	 TRANSFERRATE 0.900000;

CREATE TEMPORARY TABLESPACE TEMPSPACE2 IN NODEGROUP IBMTEMPGROUP PAGESIZE 4096 MANAGED BY SYSTEM 
	 USING ('/seqh/dp1/tempspace2',
		'/seqh/dp2/tempspace2',
		'/seqh/dp3/tempspace2',
		'/seqh/dp4/tempspace2')
	 EXTENTSIZE 32
	 PREFETCHSIZE 64
	 BUFFERPOOL IBMDEFAULTBP
	 OVERHEAD 24.100000
	 TRANSFERRATE 0.900000;

CREATE REGULAR TABLESPACE NBLASTDB_DATA_TBSP IN NODEGROUP IBMDEFAULTGROUP PAGESIZE 4096 MANAGED BY DATABASE 
	 USING (FILE '/seqh/dp1/data/nblastdb/NBLASTDB1.data'262144,
		FILE '/seqh/dp2/data/nblastdb/NBLASTDB1.data'262144,
		FILE '/seqh/dp3/data/nblastdb/NBLASTDB1.data'262144,
		FILE '/seqh/dp4/data/nblastdb/NBLASTDB1.data'262144)
	 EXTENTSIZE 32
	 PREFETCHSIZE 64
	 BUFFERPOOL IBMDEFAULTBP
	 OVERHEAD 24.100000
	 TRANSFERRATE 0.900000;

CREATE REGULAR TABLESPACE NBLASTDB_INDX_TBSP IN NODEGROUP IBMDEFAULTGROUP PAGESIZE 4096 MANAGED BY DATABASE 
	 USING (FILE '/seqh/dp1/index/nblastdb/NBLASTDB1.indx'6400,
		FILE '/seqh/dp2/index/nblastdb/NBLASTDB1.indx'6400,
		FILE '/seqh/dp3/index/nblastdb/NBLASTDB1.indx'6400,
		FILE '/seqh/dp4/index/nblastdb/NBLASTDB1.indx'6400)
	 EXTENTSIZE 32
	 PREFETCHSIZE 64
	 BUFFERPOOL IBMDEFAULTBP
	 OVERHEAD 24.100000
	 TRANSFERRATE 0.900000;

CREATE REGULAR TABLESPACE SEQHOUND_DATA IN NODEGROUP IBMDEFAULTGROUP PAGESIZE 4096 MANAGED BY SYSTEM 
	 USING ('/seqh/dp1/data/sms',
		'/seqh/dp2/data/sms',
		'/seqh/dp3/data/sms',
		'/seqh/dp4/data/sms')
	 EXTENTSIZE 32
	 PREFETCHSIZE 64
	 BUFFERPOOL IBMDEFAULTBP
	 OVERHEAD 24.100000
	 TRANSFERRATE 0.900000;

CREATE REGULAR TABLESPACE ASNDB_DATA_TBSP IN NODEGROUP IBMDEFAULTGROUP PAGESIZE 4096 MANAGED BY DATABASE 
	 USING (FILE '/seqh/dp1/data/asndb/ASNDB1.data'462144,
		FILE '/seqh/dp1/data/asndb/ASNDB2.data'462144,
		FILE '/seqh/dp2/data/asndb/ASNDB1.data'462144,
		FILE '/seqh/dp2/data/asndb/ASNDB2.data'462144,
		FILE '/seqh/dp3/data/asndb/ASNDB1.data'462144,
		FILE '/seqh/dp3/data/asndb/ASNDB2.data'462144,
		FILE '/seqh/dp4/data/asndb/ASNDB1.data'462144,
		FILE '/seqh/dp4/data/asndb/ASNDB2.data'462144)
	 EXTENTSIZE 32
	 PREFETCHSIZE 64
	 BUFFERPOOL IBMDEFAULTBP
	 OVERHEAD 24.100000
	 TRANSFERRATE 0.900000;

CREATE REGULAR TABLESPACE ASNDB_INDX_TBSP IN NODEGROUP IBMDEFAULTGROUP PAGESIZE 4096 MANAGED BY DATABASE 
	 USING (FILE '/seqh/dp1/index/asndb/ASNDB1.indx'64000,
		FILE '/seqh/dp2/index/asndb/ASNDB1.indx'64000,
		FILE '/seqh/dp3/index/asndb/ASNDB1.indx'64000,
		FILE '/seqh/dp4/index/asndb/ASNDB1.indx'64000)
	 EXTENTSIZE 32
	 PREFETCHSIZE 64
	 BUFFERPOOL IBMDEFAULTBP
	 OVERHEAD 24.100000
	 TRANSFERRATE 0.900000;

CREATE LONG TABLESPACE ASNDB_LONG_TBSP IN NODEGROUP IBMDEFAULTGROUP PAGESIZE 4096 MANAGED BY DATABASE 
	 USING (FILE '/seqh/dp1/long/asndb/ASNDB1.long'786432,
		FILE '/seqh/dp1/long/asndb/ASNDB2.long'786432,
		FILE '/seqh/dp1/long/asndb/ASNDB3.long'786432,
		FILE '/seqh/dp1/long/asndb/ASNDB4.long'786432,
		FILE '/seqh/dp1/long/asndb/ASNDB5.long'786432,
		FILE '/seqh/dp1/long/asndb/ASNDB6.long'786432,
		FILE '/seqh/dp1/long/asndb/ASNDB7.long'786432,
		FILE '/seqh/dp1/long/asndb/ASNDB8.long'786432,
		FILE '/seqh/dp1/long/asndb/ASNDB9.long'786432,
		FILE '/seqh/dp2/long/asndb/ASNDB1.long'786432,
		FILE '/seqh/dp2/long/asndb/ASNDB2.long'786432,
		FILE '/seqh/dp2/long/asndb/ASNDB3.long'786432,
		FILE '/seqh/dp2/long/asndb/ASNDB4.long'786432,
		FILE '/seqh/dp2/long/asndb/ASNDB5.long'786432,
		FILE '/seqh/dp2/long/asndb/ASNDB6.long'786432,
		FILE '/seqh/dp2/long/asndb/ASNDB7.long'786432,
		FILE '/seqh/dp2/long/asndb/ASNDB8.long'786432,
		FILE '/seqh/dp2/long/asndb/ASNDB9.long'786432,
		FILE '/seqh/dp3/long/asndb/ASNDB1.long'786432,
		FILE '/seqh/dp3/long/asndb/ASNDB2.long'786432,
		FILE '/seqh/dp3/long/asndb/ASNDB3.long'786432,
		FILE '/seqh/dp3/long/asndb/ASNDB4.long'786432,
		FILE '/seqh/dp3/long/asndb/ASNDB5.long'786432,
		FILE '/seqh/dp3/long/asndb/ASNDB6.long'786432,
		FILE '/seqh/dp3/long/asndb/ASNDB7.long'786432,
		FILE '/seqh/dp3/long/asndb/ASNDB8.long'786432,
		FILE '/seqh/dp3/long/asndb/ASNDB9.long'786432,
		FILE '/seqh/dp4/long/asndb/ASNDB1.long'786432,
		FILE '/seqh/dp4/long/asndb/ASNDB2.long'786432,
		FILE '/seqh/dp4/long/asndb/ASNDB3.long'786432,
		FILE '/seqh/dp4/long/asndb/ASNDB4.long'786432,
		FILE '/seqh/dp4/long/asndb/ASNDB5.long'786432,
		FILE '/seqh/dp4/long/asndb/ASNDB6.long'786432,
		FILE '/seqh/dp4/long/asndb/ASNDB7.long'786432,
		FILE '/seqh/dp4/long/asndb/ASNDB8.long'786432,
		FILE '/seqh/dp4/long/asndb/ASNDB9.long'786432)
	 EXTENTSIZE 32
	 PREFETCHSIZE 64
	 BUFFERPOOL IBMDEFAULTBP
	 OVERHEAD 24.100000
	 TRANSFERRATE 0.900000;

CREATE REGULAR TABLESPACE SENDB_DATA_TBSP IN NODEGROUP IBMDEFAULTGROUP PAGESIZE 4096 MANAGED BY DATABASE 
	 USING (FILE '/seqh/dp1/data/sendb/SENDB1.data'64000,
		FILE '/seqh/dp1/data/sendb/SENDB2.data'64000,
		FILE '/seqh/dp1/data/sendb/SENDB3.data'64000,
		FILE '/seqh/dp1/data/sendb/SENDB4.data'64000,
		FILE '/seqh/dp2/data/sendb/SENDB1.data'64000,
		FILE '/seqh/dp2/data/sendb/SENDB2.data'64000,
		FILE '/seqh/dp2/data/sendb/SENDB3.data'64000,
		FILE '/seqh/dp2/data/sendb/SENDB4.data'64000,
		FILE '/seqh/dp3/data/sendb/SENDB1.data'64000,
		FILE '/seqh/dp3/data/sendb/SENDB2.data'64000,
		FILE '/seqh/dp3/data/sendb/SENDB3.data'64000,
		FILE '/seqh/dp3/data/sendb/SENDB4.data'64000,
		FILE '/seqh/dp4/data/sendb/SENDB1.data'64000,
		FILE '/seqh/dp4/data/sendb/SENDB2.data'64000,
		FILE '/seqh/dp4/data/sendb/SENDB3.data'64000,
		FILE '/seqh/dp4/data/sendb/SENDB4.data'64000)
	 EXTENTSIZE 32
	 PREFETCHSIZE 64
	 BUFFERPOOL IBMDEFAULTBP
	 OVERHEAD 24.100000
	 TRANSFERRATE 0.900000;

CREATE REGULAR TABLESPACE SENDB_INDX_TBSP IN NODEGROUP IBMDEFAULTGROUP PAGESIZE 4096 MANAGED BY DATABASE 
	 USING (FILE '/seqh/dp1/index/sendb/SENDB1.indx'25600,
		FILE '/seqh/dp1/index/sendb/SENDB2.indx'25600,
		FILE '/seqh/dp2/index/sendb/SENDB1.indx'25600,
		FILE '/seqh/dp2/index/sendb/SENDB2.indx'25600,
		FILE '/seqh/dp3/index/sendb/SENDB1.indx'25600,
		FILE '/seqh/dp3/index/sendb/SENDB2.indx'25600,
		FILE '/seqh/dp4/index/sendb/SENDB1.indx'25600,
		FILE '/seqh/dp4/index/sendb/SENDB2.indx'25600)
	 EXTENTSIZE 32
	 PREFETCHSIZE 64
	 BUFFERPOOL IBMDEFAULTBP
	 OVERHEAD 24.100000
	 TRANSFERRATE 0.900000;

CREATE LONG TABLESPACE SENDB_LONG_TBSP IN NODEGROUP IBMDEFAULTGROUP PAGESIZE 4096 MANAGED BY DATABASE 
	 USING (FILE '/seqh/dp1/long/sendb/SENDB1.long'524288,
		FILE '/seqh/dp1/long/sendb/SENDB2.long'524288,
		FILE '/seqh/dp1/long/sendb/SENDB3.long'524288,
		FILE '/seqh/dp1/long/sendb/SENDB4.long'524288,
		FILE '/seqh/dp1/long/sendb/SENDB5.long'524288,
		FILE '/seqh/dp1/long/sendb/SENDB6.long'524288,
		FILE '/seqh/dp1/long/sendb/SENDB7.long'524288,
		FILE '/seqh/dp1/long/sendb/SENDB8.long'524288,
		FILE '/seqh/dp2/long/sendb/SENDB1.long'524288,
		FILE '/seqh/dp2/long/sendb/SENDB2.long'524288,
		FILE '/seqh/dp2/long/sendb/SENDB3.long'524288,
		FILE '/seqh/dp2/long/sendb/SENDB4.long'524288,
		FILE '/seqh/dp2/long/sendb/SENDB5.long'524288,
		FILE '/seqh/dp2/long/sendb/SENDB6.long'524288,
		FILE '/seqh/dp2/long/sendb/SENDB7.long'524288,
		FILE '/seqh/dp2/long/sendb/SENDB8.long'524288,
		FILE '/seqh/dp3/long/sendb/SENDB1.long'524288,
		FILE '/seqh/dp3/long/sendb/SENDB2.long'524288,
		FILE '/seqh/dp3/long/sendb/SENDB3.long'524288,
		FILE '/seqh/dp3/long/sendb/SENDB4.long'524288,
		FILE '/seqh/dp3/long/sendb/SENDB5.long'524288,
		FILE '/seqh/dp3/long/sendb/SENDB6.long'524288,
		FILE '/seqh/dp3/long/sendb/SENDB7.long'524288,
		FILE '/seqh/dp3/long/sendb/SENDB8.long'524288,
		FILE '/seqh/dp4/long/sendb/SENDB1.long'524288,
		FILE '/seqh/dp4/long/sendb/SENDB2.long'524288,
		FILE '/seqh/dp4/long/sendb/SENDB3.long'524288,
		FILE '/seqh/dp4/long/sendb/SENDB4.long'524288,
		FILE '/seqh/dp4/long/sendb/SENDB5.long'524288,
		FILE '/seqh/dp4/long/sendb/SENDB6.long'524288,
		FILE '/seqh/dp4/long/sendb/SENDB7.long'524288,
		FILE '/seqh/dp4/long/sendb/SENDB8.long'524288)
	 EXTENTSIZE 32
	 PREFETCHSIZE 64
	 BUFFERPOOL IBMDEFAULTBP
	 OVERHEAD 24.100000
	 TRANSFERRATE 0.900000;

CREATE REGULAR TABLESPACE MMDB_DATA_TBSP IN NODEGROUP IBMDEFAULTGROUP PAGESIZE 4096 MANAGED BY DATABASE 
	 USING (FILE '/seqh/dp1/data/mmdb/MMDB1.data'64000,
		FILE '/seqh/dp1/data/mmdb/MMDB2.data'64000,
		FILE '/seqh/dp2/data/mmdb/MMDB1.data'64000,
		FILE '/seqh/dp2/data/mmdb/MMDB2.data'64000,
		FILE '/seqh/dp3/data/mmdb/MMDB1.data'64000,
		FILE '/seqh/dp3/data/mmdb/MMDB2.data'64000,
		FILE '/seqh/dp4/data/mmdb/MMDB1.data'64000,
		FILE '/seqh/dp4/data/mmdb/MMDB2.data'64000)
	 EXTENTSIZE 32
	 PREFETCHSIZE 64
	 BUFFERPOOL IBMDEFAULTBP
	 OVERHEAD 24.100000
	 TRANSFERRATE 0.900000;

CREATE REGULAR TABLESPACE MMDB_INDX_TBSP IN NODEGROUP IBMDEFAULTGROUP PAGESIZE 4096 MANAGED BY DATABASE 
	 USING (FILE '/seqh/dp1/index/mmdb/MMDB1.indx'6400,
		FILE '/seqh/dp2/index/mmdb/MMDB1.indx'6400,
		FILE '/seqh/dp3/index/mmdb/MMDB1.indx'6400,
		FILE '/seqh/dp4/index/mmdb/MMDB1.indx'6400)
	 EXTENTSIZE 32
	 PREFETCHSIZE 64
	 BUFFERPOOL IBMDEFAULTBP
	 OVERHEAD 24.100000
	 TRANSFERRATE 0.900000;

CREATE LONG TABLESPACE MMDB_LONG_TBSP IN NODEGROUP IBMDEFAULTGROUP PAGESIZE 4096 MANAGED BY DATABASE 
	 USING (FILE '/seqh/dp1/long/mmdb/MMDB1.long'524288,
		FILE '/seqh/dp2/long/mmdb/MMDB1.long'524288,
		FILE '/seqh/dp3/long/mmdb/MMDB1.long'524288,
		FILE '/seqh/dp4/long/mmdb/MMDB1.long'524288)
	 EXTENTSIZE 32
	 PREFETCHSIZE 64
	 BUFFERPOOL IBMDEFAULTBP
	 OVERHEAD 24.100000
	 TRANSFERRATE 0.900000;

CREATE LONG TABLESPACE NBLASTDB_LONG_TBSP IN NODEGROUP IBMDEFAULTGROUP PAGESIZE 4096 MANAGED BY DATABASE 
	 USING (FILE '/seqh/dp1/long/nblastdb/NBLASTDB1.long'786432,
		FILE '/seqh/dp2/long/nblastdb/NBLASTDB1.long'786432,
		FILE '/seqh/dp3/long/nblastdb/NBLASTDB1.long'786432,
		FILE '/seqh/dp4/long/nblastdb/NBLASTDB1.long'786432)
	 EXTENTSIZE 32
	 PREFETCHSIZE 64
	 BUFFERPOOL IBMDEFAULTBP
	 OVERHEAD 24.100000
	 TRANSFERRATE 0.900000;

CREATE REGULAR TABLESPACE TAXDB_DATA_TBSP IN NODEGROUP IBMDEFAULTGROUP PAGESIZE 4096 MANAGED BY DATABASE 
	 USING (FILE '/seqh/dp1/data/taxdb/TAXDB1.data'262144,
		FILE '/seqh/dp1/data/taxdb/TAXDB2.data'262144,
		FILE '/seqh/dp2/data/taxdb/TAXDB1.data'262144,
		FILE '/seqh/dp2/data/taxdb/TAXDB2.data'262144,
		FILE '/seqh/dp3/data/taxdb/TAXDB1.data'262144,
		FILE '/seqh/dp3/data/taxdb/TAXDB2.data'262144,
		FILE '/seqh/dp4/data/taxdb/TAXDB1.data'262144,
		FILE '/seqh/dp4/data/taxdb/TAXDB2.data'262144)
	 EXTENTSIZE 32
	 PREFETCHSIZE 64
	 BUFFERPOOL IBMDEFAULTBP
	 OVERHEAD 24.100000
	 TRANSFERRATE 0.900000;

CREATE REGULAR TABLESPACE TAXDB_INDX_TBSP IN NODEGROUP IBMDEFAULTGROUP PAGESIZE 4096 MANAGED BY DATABASE 
	 USING (FILE '/seqh/dp1/index/taxdb/TAXDB1.indx'6400,
		FILE '/seqh/dp2/index/taxdb/TAXDB1.indx'6400,
		FILE '/seqh/dp3/index/taxdb/TAXDB1.indx'6400,
		FILE '/seqh/dp4/index/taxdb/TAXDB1.indx'6400)
	 EXTENTSIZE 32
	 PREFETCHSIZE 64
	 BUFFERPOOL IBMDEFAULTBP
	 OVERHEAD 24.100000
	 TRANSFERRATE 0.900000;

CREATE LONG TABLESPACE TAXDB_LONG_TBSP IN NODEGROUP IBMDEFAULTGROUP PAGESIZE 4096 MANAGED BY DATABASE 
	 USING (FILE '/seqh/dp1/long/taxdb/TAXDB1.long'128000,
		FILE '/seqh/dp2/long/taxdb/TAXDB1.long'128000,
		FILE '/seqh/dp3/long/taxdb/TAXDB1.long'128000,
		FILE '/seqh/dp4/long/taxdb/TAXDB1.long'128000)
	 EXTENTSIZE 32
	 PREFETCHSIZE 64
	 BUFFERPOOL IBMDEFAULTBP
	 OVERHEAD 24.100000
	 TRANSFERRATE 0.900000;

CREATE REGULAR TABLESPACE DIVDB_DATA_TBSP IN NODEGROUP IBMDEFAULTGROUP PAGESIZE 4096 MANAGED BY DATABASE 
	 USING (FILE '/seqh/dp1/data/divdb/DIVDB1.data'64000,
		FILE '/seqh/dp1/data/divdb/DIVDB2.data'64000,
		FILE '/seqh/dp2/data/divdb/DIVDB1.data'64000,
		FILE '/seqh/dp2/data/divdb/DIVDB2.data'64000,
		FILE '/seqh/dp3/data/divdb/DIVDB1.data'64000,
		FILE '/seqh/dp3/data/divdb/DIVDB2.data'64000,
		FILE '/seqh/dp4/data/divdb/DIVDB1.data'64000,
		FILE '/seqh/dp4/data/divdb/DIVDB2.data'64000)
	 EXTENTSIZE 32
	 PREFETCHSIZE 64
	 BUFFERPOOL IBMDEFAULTBP
	 OVERHEAD 24.100000
	 TRANSFERRATE 0.900000;

CREATE REGULAR TABLESPACE DIVDB_INDX_TBSP IN NODEGROUP IBMDEFAULTGROUP PAGESIZE 4096 MANAGED BY DATABASE 
	 USING (FILE '/seqh/dp1/index/divdb/DIVDB1.indx'6400,
		FILE '/seqh/dp2/index/divdb/DIVDB1.indx'6400,
		FILE '/seqh/dp3/index/divdb/DIVDB1.indx'6400,
		FILE '/seqh/dp4/index/divdb/DIVDB1.indx'6400)
	 EXTENTSIZE 32
	 PREFETCHSIZE 64
	 BUFFERPOOL IBMDEFAULTBP
	 OVERHEAD 24.100000
	 TRANSFERRATE 0.900000;

CREATE LONG TABLESPACE DIVDB_LONG_TBSP IN NODEGROUP IBMDEFAULTGROUP PAGESIZE 4096 MANAGED BY DATABASE 
	 USING (FILE '/seqh/dp1/long/divdb/DIVDB1.long'256,
		FILE '/seqh/dp1/long/divdb/DIVDB2.long'256,
		FILE '/seqh/dp2/long/divdb/DIVDB1.long'256,
		FILE '/seqh/dp2/long/divdb/DIVDB2.long'256,
		FILE '/seqh/dp3/long/divdb/DIVDB1.long'256,
		FILE '/seqh/dp3/long/divdb/DIVDB2.long'256,
		FILE '/seqh/dp4/long/divdb/DIVDB1.long'256,
		FILE '/seqh/dp4/long/divdb/DIVDB2.long'256)
	 EXTENTSIZE 32
	 PREFETCHSIZE 64
	 BUFFERPOOL IBMDEFAULTBP
	 OVERHEAD 24.100000
	 TRANSFERRATE 0.900000;

CREATE REGULAR TABLESPACE GCODEDB_DATA_TBSP IN NODEGROUP IBMDEFAULTGROUP PAGESIZE 4096 MANAGED BY DATABASE 
	 USING (FILE '/seqh/dp1/data/gcodedb/GCODEDB1.data'64000,
		FILE '/seqh/dp1/data/gcodedb/GCODEDB2.data'64000,
		FILE '/seqh/dp1/data/gcodedb/GCODEDB3.data'64000,
		FILE '/seqh/dp1/data/gcodedb/GCODEDB4.data'64000,
		FILE '/seqh/dp2/data/gcodedb/GCODEDB1.data'64000,
		FILE '/seqh/dp2/data/gcodedb/GCODEDB2.data'64000,
		FILE '/seqh/dp2/data/gcodedb/GCODEDB3.data'64000,
		FILE '/seqh/dp2/data/gcodedb/GCODEDB4.data'64000,
		FILE '/seqh/dp3/data/gcodedb/GCODEDB1.data'64000,
		FILE '/seqh/dp3/data/gcodedb/GCODEDB2.data'64000,
		FILE '/seqh/dp3/data/gcodedb/GCODEDB3.data'64000,
		FILE '/seqh/dp3/data/gcodedb/GCODEDB4.data'64000,
		FILE '/seqh/dp4/data/gcodedb/GCODEDB1.data'64000,
		FILE '/seqh/dp4/data/gcodedb/GCODEDB2.data'64000,
		FILE '/seqh/dp4/data/gcodedb/GCODEDB3.data'64000,
		FILE '/seqh/dp4/data/gcodedb/GCODEDB4.data'64000)
	 EXTENTSIZE 32
	 PREFETCHSIZE 64
	 BUFFERPOOL IBMDEFAULTBP
	 OVERHEAD 24.100000
	 TRANSFERRATE 0.900000;

CREATE REGULAR TABLESPACE GCODEDB_INDX_TBSP IN NODEGROUP IBMDEFAULTGROUP PAGESIZE 4096 MANAGED BY DATABASE 
	 USING (FILE '/seqh/dp1/index/gcodedb/GCODEDB1.indx'6400,
		FILE '/seqh/dp2/index/gcodedb/GCODEDB1.indx'6400,
		FILE '/seqh/dp3/index/gcodedb/GCODEDB1.indx'6400,
		FILE '/seqh/dp4/index/gcodedb/GCODEDB1.indx'6400)
	 EXTENTSIZE 32
	 PREFETCHSIZE 64
	 BUFFERPOOL IBMDEFAULTBP
	 OVERHEAD 24.100000
	 TRANSFERRATE 0.900000;

CREATE LONG TABLESPACE GCODEDB_LONG_TBSP IN NODEGROUP IBMDEFAULTGROUP PAGESIZE 4096 MANAGED BY DATABASE 
	 USING (FILE '/seqh/dp1/long/gcodedb/GCODEDB1.long'256,
		FILE '/seqh/dp2/long/gcodedb/GCODEDB1.long'256,
		FILE '/seqh/dp3/long/gcodedb/GCODEDB1.long'256,
		FILE '/seqh/dp4/long/gcodedb/GCODEDB1.long'256)
	 EXTENTSIZE 32
	 PREFETCHSIZE 64
	 BUFFERPOOL IBMDEFAULTBP
	 OVERHEAD 24.100000
	 TRANSFERRATE 0.900000;

CREATE REGULAR TABLESPACE TAXGI_DATA_TBSP IN NODEGROUP IBMDEFAULTGROUP PAGESIZE 4096 MANAGED BY DATABASE 
	 USING (FILE '/seqh/dp1/data/taxgi/TAXGI1.data'128000,
		FILE '/seqh/dp1/data/taxgi/TAXGI2.data'128000,
		FILE '/seqh/dp2/data/taxgi/TAXGI1.data'128000,
		FILE '/seqh/dp2/data/taxgi/TAXGI2.data'128000,
		FILE '/seqh/dp3/data/taxgi/TAXGI1.data'128000,
		FILE '/seqh/dp3/data/taxgi/TAXGI2.data'128000,
		FILE '/seqh/dp4/data/taxgi/TAXGI1.data'128000,
		FILE '/seqh/dp4/data/taxgi/TAXGI2.data'128000)
	 EXTENTSIZE 32
	 PREFETCHSIZE 64
	 BUFFERPOOL IBMDEFAULTBP
	 OVERHEAD 24.100000
	 TRANSFERRATE 0.900000;

CREATE REGULAR TABLESPACE TAXGI_INDX_TBSP IN NODEGROUP IBMDEFAULTGROUP PAGESIZE 4096 MANAGED BY DATABASE 
	 USING (FILE '/seqh/dp1/index/taxgi/TAXGI1.indx'51200,
		FILE '/seqh/dp1/index/taxgi/TAXGI2.indx'51200,
		FILE '/seqh/dp1/index/taxgi/TAXGI3.indx'51200,
		FILE '/seqh/dp1/index/taxgi/TAXGI4.indx'51200,
		FILE '/seqh/dp2/index/taxgi/TAXGI1.indx'51200,
		FILE '/seqh/dp2/index/taxgi/TAXGI2.indx'51200,
		FILE '/seqh/dp2/index/taxgi/TAXGI3.indx'51200,
		FILE '/seqh/dp2/index/taxgi/TAXGI4.indx'51200,
		FILE '/seqh/dp3/index/taxgi/TAXGI1.indx'51200,
		FILE '/seqh/dp3/index/taxgi/TAXGI2.indx'51200,
		FILE '/seqh/dp3/index/taxgi/TAXGI3.indx'51200,
		FILE '/seqh/dp3/index/taxgi/TAXGI4.indx'51200,
		FILE '/seqh/dp4/index/taxgi/TAXGI1.indx'51200,
		FILE '/seqh/dp4/index/taxgi/TAXGI2.indx'51200,
		FILE '/seqh/dp4/index/taxgi/TAXGI3.indx'51200,
		FILE '/seqh/dp4/index/taxgi/TAXGI4.indx'51200)
	 EXTENTSIZE 32
	 PREFETCHSIZE 64
	 BUFFERPOOL IBMDEFAULTBP
	 OVERHEAD 24.100000
	 TRANSFERRATE 0.900000;

CREATE REGULAR TABLESPACE ACCDB_DATA_TBSP IN NODEGROUP IBMDEFAULTGROUP PAGESIZE 4096 MANAGED BY DATABASE 
	 USING (FILE '/seqh/dp1/data/accdb/ACCDB1.data'503000,
		FILE '/seqh/dp1/data/accdb/ACCDB2.data'503000,
		FILE '/seqh/dp2/data/accdb/ACCDB1.data'503000,
		FILE '/seqh/dp2/data/accdb/ACCDB2.data'503000,
		FILE '/seqh/dp3/data/accdb/ACCDB1.data'503000,
		FILE '/seqh/dp3/data/accdb/ACCDB2.data'503000,
		FILE '/seqh/dp4/data/accdb/ACCDB1.data'503000,
		FILE '/seqh/dp4/data/accdb/ACCDB2.data'503000)
	 EXTENTSIZE 32
	 PREFETCHSIZE 64
	 BUFFERPOOL IBMDEFAULTBP
	 OVERHEAD 24.100000
	 TRANSFERRATE 0.900000;

CREATE REGULAR TABLESPACE ACCDB_INDX_TBSP IN NODEGROUP IBMDEFAULTGROUP PAGESIZE 4096 MANAGED BY DATABASE 
	 USING (FILE '/seqh/dp1/index/accdb/ACCDB1.indx'256000,
		FILE '/seqh/dp1/index/accdb/ACCDB2.indx'256000,
		FILE '/seqh/dp2/index/accdb/ACCDB1.indx'256000,
		FILE '/seqh/dp2/index/accdb/ACCDB2.indx'256000,
		FILE '/seqh/dp3/index/accdb/ACCDB1.indx'256000,
		FILE '/seqh/dp3/index/accdb/ACCDB2.indx'256000,
		FILE '/seqh/dp4/index/accdb/ACCDB1.indx'256000,
		FILE '/seqh/dp4/index/accdb/ACCDB2.indx'256000,
		FILE '/seqh/dp1/index/accdb/ACCDB3.indx'256000,
		FILE '/seqh/dp2/index/accdb/ACCDB3.indx'256000,
		FILE '/seqh/dp3/index/accdb/ACCDB3.indx'256000)
	 EXTENTSIZE 32
	 PREFETCHSIZE 256
	 BUFFERPOOL IBMDEFAULTBP
	 OVERHEAD 24.100000
	 TRANSFERRATE 0.900000;

CREATE REGULAR TABLESPACE CDDB_INDX_TBSP IN NODEGROUP IBMDEFAULTGROUP PAGESIZE 4096 MANAGED BY DATABASE 
	 USING (FILE '/seqh/dp1/index/cddb/CDDB1.indx'6400,
		FILE '/seqh/dp2/index/cddb/CDDB1.indx'6400,
		FILE '/seqh/dp3/index/cddb/CDDB1.indx'6400,
		FILE '/seqh/dp4/index/cddb/CDDB1.indx'6400)
	 EXTENTSIZE 32
	 PREFETCHSIZE 64
	 BUFFERPOOL IBMDEFAULTBP
	 OVERHEAD 24.100000
	 TRANSFERRATE 0.900000;

CREATE LONG TABLESPACE CDDB_LONG_TBSP IN NODEGROUP IBMDEFAULTGROUP PAGESIZE 4096 MANAGED BY DATABASE 
	 USING (FILE '/seqh/dp1/long/cddb/CDDB1.long'128000,
		FILE '/seqh/dp2/long/cddb/CDDB1.long'128000,
		FILE '/seqh/dp3/long/cddb/CDDB1.long'128000,
		FILE '/seqh/dp4/long/cddb/CDDB1.long'128000)
	 EXTENTSIZE 32
	 PREFETCHSIZE 64
	 BUFFERPOOL IBMDEFAULTBP
	 OVERHEAD 24.100000
	 TRANSFERRATE 0.900000;

CREATE REGULAR TABLESPACE BIG1 IN NODEGROUP IBMDEFAULTGROUP PAGESIZE 4096 MANAGED BY SYSTEM 
	 USING ('/seqh/dp1/big1',
		'/seqh/dp2/big1',
		'/seqh/dp3/big1',
		'/seqh/dp4/big1')
	 EXTENTSIZE 32
	 PREFETCHSIZE 128
	 BUFFERPOOL IBMDEFAULTBP
	 OVERHEAD 24.100000
	 TRANSFERRATE 0.900000;


-- Mimic tablespace

ALTER TABLESPACE SYSCATSPACE
      PREFETCHSIZE 32
      OVERHEAD 24.100000
      TRANSFERRATE 0.900000;


COMMIT WORK;

CONNECT RESET;

TERMINATE;
