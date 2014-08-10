include_directories( .. )
add_executable( cmdremesher main.cc )
target_link_libraries( cmdremesher remesh ${CMAKE_THREAD_LIBS_INIT} )
