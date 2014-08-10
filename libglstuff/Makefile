include ../Makefile.inc

SOURCES = gltrianglemesh.cc gltestscene.cc glskybox.cc \
	  gltriangledebug.cc glfeatureedges.cc glfrustarenderer.cc \
	  cameracontrol.cc camerasimple.cc cameratrackball.cc camerarotate.cc \
	  fonttexture.cc texture.cc
OBJECTS = $(foreach file,$(SOURCES),$(subst .cc,.o,$(file)))
LIBRARY = libglstuff.a

GCC_INCL = -I..

all: ${OBJECTS}
	ar rcs ${LIBRARY} ${OBJECTS} ../libremesh/*.o
	chmod a+x ${LIBRARY}

multicore:
	$(MAKE) -j${CORES} all

depend:
	${COMPILER} ${GCC_INCL} -MM ${SOURCES} > Makefile.dep

%.o: %.cc
	${COMPILER} -c -o $@ $< ${GCC_FLAGS} ${GCC_FLAGS} ${GCC_INCL}

clean: FORCE
	rm -f ${OBJECTS} ${LIBRARY}

FORCE:

include Makefile.dep
