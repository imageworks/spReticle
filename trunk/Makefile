# Makefile for a Maya plugin.

##################################
# Platform specific build settings
##################################

C++           = g++

NO_TRANS_LINK =
CFLAGS        = -DLINUX -D_BOOL -DREQUIRE_IOSTREAM -DBits64_ -DLINUX_64 -fPIC -O3 
C++FLAGS      = $(CFLAGS) -Wno-deprecated -fno-gnu-keywords

LD            = $(C++) $(NO_TRANS_LINK) $(C++FLAGS) -Wl,-Bsymbolic -shared

INCLUDES      = -I. -I$(MAYA_LOCATION)/include
LIBS          = -L$(MAYA_LOCATION)/lib

.cpp.o:
	$(C++) -c $(INCLUDES) $(C++FLAGS) $<

plugins: \
    spReticuleLoc.so

clean:
	-rm -f *.o

Clean:
	-rm -f *.o *.so

##################
# Specific Rules #
##################

spReticuleLoc.so: spReticuleLoc.o
	-rm -f $@
	$(LD) -o $@ spReticuleLoc.o $(LIBS) -lOpenMaya -lOpenMayaUI -lOpenMayaAnim

