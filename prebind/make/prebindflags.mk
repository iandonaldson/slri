#prebindflags.mk

CFLAGS = $(CFLAGS1) $(OPTFLAG) -I$(INCPATH) $(DEF)
LDFLAGS = -I$(INCPATH) $(OPTFLAG) -L$(LIBPATH)


## To clean out the directory without removing make
##
## This is for making libraries
##
## Implicit actions
## 
## if need a .o, compile the .c
##

.c.o :
	$(CC) $(CFLAGS) $< -o $@
#	-mkdir $(BUILDDIR)
#	mv $(@F) $(BUILDDIR)

#Comments for the make challenged.
#CFLAGS collects compiler switches (no optimization) that are defined in prebind.mk
#CFLAGS are used by the rule defined by (.c.o: ...etc) at the bottom of this file
#LDFLAGS collects compiler switches used when linking (no optimization). These are defined in prebind.mk
#LDFLAGS are used by make files (e.g. make.prebindcgi)
#.c.o: is a default rule for building object files from .c files.  Usually there would be something on the same line following .c.o:...
#but here, the default .c.o: rule provided by the make utility has been replaced by the the following two lines under .c.o:
# These lines instruct make to build the .o file from the .c file by using some compiler (defined by CC ;for e.g. -gcc) using the compile
#flags defined above by CFLAGS.
#'$<' means the current target file
#'$(@F)' means the resulting .o file  


