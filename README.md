A software for isotropic surface remeshing written by Simon Fuhrmann.
Original project page can be found [here](http://www.gris.informatik.tu-darmstadt.de/~sfuhrman/remesher.html).

CMake build system introduced by Alex Tsui.

Note that the code is distributed under the GPL license. This means
that you are neither allowed to use the code or parts of the code in
non-GPL applications, nor link the libraries against non-GPL binaries.
See COPYING for details.


COMPONENTS
----------------------------------------------------------------------

The applications in this package are:

 - a GUI application (gtkremesher)
 - a console application (cmdremesher)

The applications use some or all of the following libraries:

 - a geometric library (libremesh)
 - a rendering library (libglstuff)
 - a third party matrix library (libgmm)

The console application requires libremesh only (and pthreads).
The GUI application requires both libraries and a few system libraries.
These system libraries, typically available in your distro, are:

 - GTKmm, the GUI toolkit (developer packages)
 - GtkGLextmm, the OpenGL widget for GTKmm (developer packages)
 - OpenGL


COMPILING
----------------------------------------------------------------------

This version of the project uses a CMake build system.

    mkdir build
    cd build
    cmake ..
    make

The resulting binaries are in the "gtkremesher" and "cmdremesher"
directories after successful compilation.


USAGE
----------------------------------------------------------------------

Note that this software is scientific code, i.e. may contain bugs or
cause unexpected behaviour or crashes, and may not be industrial-proof.

GUI application
---------------

Note that the GUI application is using STDOUT for debug messages
and important information. Observe the output!

 - Loading the mesh
   - Use the right pane of the window to first load the original mesh.
     The application supports most PLY and OFF models.
   - If you don't see a mesh, try scale and center
   - If your mesh contains invalid triangles, try cleaning the mesh
   - If you don't see triangles or proper shading, try to invert orientation

 - Controls
   - Right-clicking on the mesh centers position
   - Middle-clicking and moving scales the mesh
   - Left-clicking and moving rotates around the center

 - Guidance
   - A density field can be calculated which controls the
     sample density of the remeshed model.
   - Feature edges may be extracted which are specially treated
     during remeshing. Note that only a naive angle thresholding
     is implemented.

 - Conversion
   - The original mesh is converted to an initial remesh (evolving mesh).
     This can be done using either a simiplification or resampling.
   - During resampling, some error messages may appear in the console.
     Most errors can be ignored and are just for debugging purposes.

 - Remeshing
   - The initial remesh can be imporoved using Lloyd relaxation.
   - Area equalization is also implemented but is unstable
     and can lead to flipped or invalid triangles.
   - During remeshing, some error messages may appear in the console.
     Most errors can be ignored and are just for debugging purposes.

Command line application
------------------------

Start the command line application with a model file as argument.
Note that remeshing settings need to be specified in code. Edit
file 'main.cc' and tune the parameters to your needs.
