set( glstuff_SOURCES gltrianglemesh.cc gltestscene.cc glskybox.cc
    gltriangledebug.cc glfeatureedges.cc glfrustarenderer.cc
    cameracontrol.cc camerasimple.cc cameratrackball.cc camerarotate.cc
    fonttexture.cc texture.cc
)

include_directories( BEFORE .. )

add_library( glstuff ${glstuff_SOURCES} )
