# Makefile for a Maya plugin.

##################################
# Platform specific build settings
##################################

ARCH          = $(shell uname -m)
C++           = g++
BUILDDIR      = Build/$(ARCH)

NO_TRANS_LINK =
CFLAGS        = -DLINUX -D_BOOL -DREQUIRE_IOSTREAM -DBits64_ -DLINUX_64 -fPIC

C++FLAGS      = $(CFLAGS) -Wno-deprecated -fno-gnu-keywords

LD            = $(C++) $(NO_TRANS_LINK) $(C++FLAGS) -Wl,-Bsymbolic -shared

INCLUDES      = -I. -I$(MAYA_LOCATION)/include -I/opt/X11/include
LIBS          = -L$(MAYA_LOCATION)/lib

debug: spReticleLoc.o spReticleLoc.so
opt: spReticleLoc.o spReticleLoc.so

debug: BUILDDIR = Build/$(ARCH)-debug
debug: CFLAGS += -g -gstabs+

opt: BUILDDIR = Build/$(ARCH)-opt
opt: CFLAGS += -O3

.cpp.o:
	-mkdir -p $(BUILDDIR)
	$(C++) -c $(INCLUDES) $(C++FLAGS) -o $(BUILDDIR)/$@ $<

plugins: \
    spReticleLoc.so

clean:
	-rm -f Build/*/*.o

Clean:
	-rm -rf Build

##################
# Specific Rules #
##################
GPURenderer.o : util.h GPURenderer.h GPURenderer.cpp
OpenGLRenderer.o : font.h OpenGLRenderer.h OpenGLRenderer.cpp
V2Renderer.o : V2Renderer.h V2Renderer.cpp
spReticleLoc.o : defines.h util.h spReticleLoc.h spReticleLoc.cpp

spReticleLoc.so: GPURenderer.o OpenGLRenderer.o V2Renderer.o spReticleLoc.o
	-@mkdir -p $(BUILDDIR)
	-@rm -f $@
	$(LD) -o $(BUILDDIR)/$@ $(BUILDDIR)/GPURenderer.o $(BUILDDIR)/OpenGLRenderer.o $(BUILDDIR)/V2Renderer.o $(BUILDDIR)/spReticleLoc.o $(LIBS) -lOpenMaya -lOpenMayaRender -lOpenMayaUI
	@echo ""
	@echo "###################################################"
	@echo successfully compiled $@ into $(BUILDDIR)
	@echo $(CURDIR)/$(BUILDDIR)/$@
	@echo ""

