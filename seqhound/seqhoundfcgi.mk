SEQH_SLRIDIR = 
SEQH_SLRIINC = ../../lib
SEQH_TAXINC = ../taxon
SEQH_SHDIR = 
SEQH_SHINC = ../include
SEQH_BZDIR = ../../../bzip2
SEQH_BZINC = ../../../bzip2
MOBIDIR = ../../mobidick/lib
MOBIINC = ../../mobidick/include
GOINC = ../go
LLINC = ../locuslink
FCGI = /home3/people/kaca/apache_1.3.12/devkit_2.2.0/include
SEQH_INCDIR = $(CBDIR) -I$(SEQH_SLRIINC) -I$(SEQH_TAXINC) -I$(SEQH_SHINC) -I$(SEQH_BZINC) -I./ -I$(FCGI) -I$(GOINC) -I$(LLINC)
SEQH_LIBDIR = $(CBDIR) -L$(SEQH_SLRIDIR) -L$(SEQH_SHDIR) -L$(SEQH_BZDIR) -L$(MOBIDIR)
BUILDDIR = ../build
SEQH_SLRILIB = -lshoundlocl -lslri
EXTRAOPT = -DSHFCGI -D_FILE_OFFSET_BITS=64
