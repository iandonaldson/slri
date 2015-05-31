SRC1 = slristruc slristruc_

SRC2 = nblast nblast_

SRC3 = Seqhound

ALLSRC = $(SRC1) $(SRC2) $(SRC3)

BASEDIR = -I$(NCBICXX)/include
INCSEQHOUND_LIB_DIR = -I../include_cxx
INCSLRILIB_DIR = -I../../lib_cxx/include
INCDIR2 = $(BASEDIR)/objects/cdd
INCDIR3 = $(BASEDIR)/objects/mmdb1
INCDIR4 = $(BASEDIR)/objects/mmdb2
INCDIR5 = $(BASEDIR)/objects/mmdb3
INCDIR6 = $(BASEDIR)/objects/seqset
INCDIR7 = $(BASEDIR)/objects/seqfeat
INCDIR8 = $(BASEDIR)/objects/seq
INCDIR9 = $(BASEDIR)/objects/seqalign
INCDIR10 = $(BASEDIR)/objects/seqblock
INCDIR11 = $(BASEDIR)/objects/seqloc
INCDIR12 = $(BASEDIR)/objects/seqres
INCDIR13 = $(BASEDIR)/objects/seqcode
INCDIR14 = $(BASEDIR)/objects/general
INC_STRUC = $(INCDIR2) $(INCDIR3) $(INCDIR4) $(INCDIR5)
INC_SEQ = $(INCDIR6) $(INCDIR7) $(INCDIR8) $(INCDIR9) $(INCDIR10) $(INCDIR11) $(INCDIR12) $(INCDIR13)
INC_PUB = $(INCDIR14)
INTERNAL_INCDIR = $(INCSEQHOUND_LIB_DIR) $(INCSLRILIB_DIR) $(INC_STRUC) $(INC_SEQ) $(INC_PUB) 
