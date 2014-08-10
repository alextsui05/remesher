set(gtkremesher_SOURCES main.cc maingui.cc gtkopengl.cc arguments.cc gtkconfig.cc guiglinfo.cc imagestore.cc gtkmeshrendering.cc gtkcomboboxcheck.cc)

include_directories( .. )
include_directories( ${GTK2_INCLUDE_DIRS} )
include_directories( ${GTKGLEXTMM_INCLUDE_DIRS} )
message( STATUS "${GTKGLEXTMM_INCLUDE_DIRS}" )
#/usr/include/gtkglextmm-1.2
add_executable( gtkremesher ${gtkremesher_SOURCES} )
target_link_libraries( gtkremesher remesh glstuff ${OPENGL_LIBRARIES} ${GTK2_LIBRARIES} ${GTKGLEXTMM_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} )
