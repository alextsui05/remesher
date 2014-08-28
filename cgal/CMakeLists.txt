include_directories( .. )
add_executable( cgaltest test.cpp )
target_link_libraries( cgaltest remesh ${CGAL_LIBRARIES} )
