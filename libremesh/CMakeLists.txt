set( remesh_SOURCES interface.cc
    modelwriter.cc modelloader.cc trianglemesh.cc
    helpers.cc averageplane.cc micropatch.cc microdelaunay.cc
    relocation.cc polygon2.cc triangulator.cc 
    simplification.cc meshcleanup.cc meshoptimize.cc 
    oversampling.cc meshslicing.cc relaxation.cc relaxanglesmooth.cc 
    subdivbase.cc subdivlinear.cc subdivloop.cc meshdecimation.cc
    edgeflips.cc delaunayflips.cc resampling.cc incdelaunay.cc
    vertexinfo.cc vertexref.cc patch3d.cc patch2d_fb.cc patch2d.cc
    densityfield.cc featureedges.cc meshskeleton.cc patchcache.cc
    pntriangle.cc meshstats.cc cvtstats.cc gslpolysolve.cc
)

add_library( remesh ${remesh_SOURCES} )
