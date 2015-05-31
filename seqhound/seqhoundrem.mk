SEQH_SLRIDIR = ../../lib
SEQH_SLRIINC =  ../../lib
SEQH_TAXINC = ../taxon
SEQH_SHDIR = ../lib
SEQH_SHINC = ../include
SEQH_BZDIR = ../../../bzip2
SEQH_BZINC = ../../../bzip2
LLINC = ../locuslink
GOINC = ../go
SEQH_INCDIR = $(SEQH_SLRIINC) -I$(SEQH_TAXINC) -I$(SEQH_SHINC) -I$(SEQH_BZINC) -I./ -I$(LLINC) -I$(GOINC)
SEQH_LIBDIR = $(SEQH_SLRIDIR) -L$(SEQH_SHDIR) -L$(SEQH_BZDIR)
BUILDDIR = ../build
SEQH_SLRILIB = -lshoundrem -lslri
ARFLAGS = 
EXTRAOPT = -D_FILE_OFFSET_BITS=64
