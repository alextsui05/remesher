# Isotropic surface remesher
# http://www.gris.informatik.tu-darmstadt.de/~sfuhrman/files/remesher-2010-11-01.tar.gz
cmake_minimum_required( VERSION 2.8 )

project( remesher )

# Project-specific settings
set( CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}" )
include_directories( ${CMAKE_CURRENT_BINARY_DIR} )
#add_definitions( -DBUILD_TESTS )

# (UNIX only) Configure build targets to look for libraries relative to their
# install path.
if ( ${CMAKE_SYSTEM_NAME} MATCHES "Linux" ) # This is how to check for Linux
    set( CMAKE_SKIP_BUILD_RPATH FALSE )
    set( CMAKE_BUILD_WITH_INSTALL_RPATH FALSE )
    set( CMAKE_INSTALL_RPATH "\$ORIGIN/../lib" )
    set( CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE )
endif ( ${CMAKE_SYSTEM_NAME} MATCHES "Linux" )

# Let the user set some options
#option( BUILD_TESTS "Build unit tests" OFF )
#option( BUILD_EXAMPLES "Build examples" OFF )
#if ( APPLE )
#    option( BUILD_OSX_BUNDLE "Build OSX bundle" OFF )
#endif( )

# Define necessary packages
# sudo apt-get install libgmm++-dev
find_package( OpenGL REQUIRED )
find_package( Threads REQUIRED )
find_package( GMM REQUIRED )

# sudo apt-get install libgtkmm-2.4-dev libgtkglextmm-x11-1.2-dev
find_package( GTK2 2.4 REQUIRED gtk gtkmm )
find_package( GTKGLEXTMM REQUIRED )

#find_package( CGAL REQUIRED )
#include( ${CGAL_USE_FILE} )

# define build targets

#add_library( Foo SHARED Foo.cpp )
#add_library( FooStatic STATIC Foo.cpp )
#if (UNIX)
#    set_target_properties( FooStatic PROPERTIES OUTPUT_NAME Foo )
#endif(UNIX)

add_subdirectory( libremesh )
add_subdirectory( libglstuff )
add_subdirectory( cmdremesher )
add_subdirectory( gtkremesher )

#install( TARGETS test
#    RUNTIME DESTINATION bin 
#    LIBRARY DESTINATION lib
#)
