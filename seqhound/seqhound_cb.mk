################################################################
#  Main configuration file for seqhound local with CODEBASE    #
################################################################
CBLIB = -lcb

INCDIR_CB = $(CBDIR)
LIBDIR_CB = $(CBDIR)
DEF_CB = -DCODEBASE

# Database specific source Files

SEQH_SRC_CB = intrez.c intrez_cb.c nbr_cb.c taxdb_cb.c ll_cb.c go_cb.c rpsdb_cb.c hist_cb.c comp_redund_cb.c
SEQH_OBJ_OCB = intrez.ocb intrez_cb.ocb nbr_cb.ocb taxdb_cb.ocb ll_cb.ocb go_cb.ocb rpsdb_cb.ocb hist_cb.ocb comp_redund_cb.ocb
