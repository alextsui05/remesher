include ../Makefile.inc

SOURCES = main.cc maingui.cc gtkopengl.cc arguments.cc gtkconfig.cc guiglinfo.cc imagestore.cc gtkmeshrendering.cc gtkcomboboxcheck.cc
OBJECTS = $(foreach file,$(SOURCES),$(subst .cc,.o,$(file)))
BINARY = main

GCC_INCL = -I..
GCC_LINK = #-L../libremesh -lremesh -L../libglstuff -lglstuff
GCC_LINK = -L../libglstuff -lglstuff -lpthread #-lSOIL
GTK_FLAGS = $(shell pkg-config --cflags gtkmm-2.4 gtkglextmm-1.2)
GTK_LIBS = $(shell pkg-config --libs gtkmm-2.4 gtkglextmm-1.2)

all: libraries ${OBJECTS}
	${COMPILER} -o ${BINARY} ${OBJECTS} ${GTK_LIBS} ${GCC_LINK}

multicore:
	$(MAKE) -j${CORES} all

depend:
	${COMPILER} ${GCC_INCL} -MM ${SOURCES} > Makefile.dep

%.o: %.cc
	${COMPILER} -c -o $@ $< ${GCC_FLAGS} ${GTK_FLAGS} ${GCC_INCL}

clean: FORCE
	rm -f ${BINARY} ${OBJECTS}

libraries: FORCE
	$(MAKE) -C ../libremesh
	$(MAKE) -C ../libglstuff

FORCE:

include Makefile.dep
