#ifndef GL_EDGE_MESH_HEADER
#define GL_EDGE_MESH_HEADER

#include "libremesh/refptr.h"
#include "libremesh/trianglemesh.h"
#include "libremesh/edgemesh.h"
#include "gldrawable.h"

class GLEdgeMesh;
typedef RefPtr<GLEdgeMesh> GLEdgeMeshPtr;

class GLEdgeMesh : public GLDrawable
{
  private:
    Remesher::EdgeMeshPtr edges;
    Remesher::TriangleMeshPtr mesh;

  protected:
    GLEdgeMesh (void);
    void draw_impl (void);

  public:
    static GLEdgeMeshPtr create (void);

    void set_edge_mesh (Remesher::EdgeMeshPtr edges);
    void set_triangle_mesh (Remesher::TriangleMeshPtr mesh);

    void release_meshes (void);
};

/* ---------------------------------------------------------------- */

inline GLEdgeMeshPtr
GLEdgeMesh::create (void)
{
  return GLEdgeMeshPtr(new GLEdgeMesh);
}

inline
GLEdgeMesh::GLEdgeMesh (void)
{
}

inline void
GLEdgeMesh::set_edge_mesh (Remesher::EdgeMeshPtr edges)
{
  this->edges = edges;
}

inline void
GLEdgeMesh::set_triangle_mesh (Remesher::TriangleMeshPtr mesh)
{
  this->mesh = mesh;
}

inline void
GLEdgeMesh::release_meshes (void)
{
  this->edges.reset();
  this->mesh.reset();
}

#endif /* GL_EDGE_MESH_HEADER */
