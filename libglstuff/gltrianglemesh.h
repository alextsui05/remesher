#ifndef GL_TRIANGLE_MESH_HEADER
#define GL_TRIANGLE_MESH_HEADER

#include <vector>

#include "libremesh/refptr.h"
#include "libremesh/vec4.h"
#include "libremesh/trianglemesh.h"
#include "gldrawable.h"

enum GLMeshShading
{
  MESH_SHADING_NONE,
  MESH_SHADING_FACE,
  MESH_SHADING_VERTEX
};

enum GLMeshDecoration
{
  MESH_DECO_NONE = 0,
  MESH_DECO_POINTS = 1 << 0,
  MESH_DECO_LINES = 1 << 1,
  MESH_DECO_VORONOI = 1 << 2,
  MESH_DECO_VNORMALS = 1 << 3,
  MESH_DECO_FNORMALS = 1 << 4,
  MESH_DECO_FACE_ORDER = 1 << 5,
  MESH_DECO_REGULARITY = 1 << 6
};

/* ---------------------------------------------------------------- */

class GLTriangleMesh;
typedef RefPtr<GLTriangleMesh> GLTriangleMeshPtr;
typedef std::vector<Remesher::Vec3f> GLColorArray;

class GLTriangleMesh : public GLDrawable
{
  private:
    Remesher::TriangleMeshPtr mesh;

    GLMeshShading shading;
    int decoration;
    Remesher::Vec4f shade_color;
    Remesher::Vec4f deco_color;
    GLColorArray color_array;
    int dlist_id;

  protected:
    GLTriangleMesh (void);
    void draw_impl (void);
    void draw_points (bool allow_color = true);
    void draw_lines (void);
    void draw_with_vertex_normals (bool allow_color = true);
    void draw_with_face_normals (void);
    void draw_vertex_normals (void);
    void draw_face_normals (void);
    void draw_voronoi_cells (void);
    void draw_face_connections (void);
    void draw_vertex_regularity (void);

  public:
    static GLTriangleMeshPtr create (void);

    void set_mesh (Remesher::TriangleMeshPtr mesh);
    Remesher::TriangleMeshPtr get_mesh (void) const;
    void release_mesh (void);
    void update_mesh (void);

    void set_shade_color (float r, float g, float b, float a);
    void set_decoration_color (float r, float g, float b, float a);

    void set_mesh_shading (GLMeshShading shading);
    void set_mesh_decoration (int decoration);

    GLMeshShading get_mesh_shading (void) const;
    int get_mesh_decoration (void) const;

    GLColorArray& get_color_array (void);
    GLColorArray const& get_color_array (void) const;
    void regen_face_shading (void);
};

/* ---------------------------------------------------------------- */

inline
GLTriangleMesh::GLTriangleMesh (void)
{
  this->shading = MESH_SHADING_FACE;
  this->decoration = MESH_DECO_LINES | MESH_DECO_POINTS;
  this->dlist_id = 0;
  this->set_shade_color(0.4f, 0.4f, 0.6f, 1.0f);
  this->set_decoration_color(0.3f, 0.3f, 0.5f, 0.4f);
}

inline GLTriangleMeshPtr
GLTriangleMesh::create (void)
{
  return GLTriangleMeshPtr(new GLTriangleMesh);
}

inline Remesher::TriangleMeshPtr
GLTriangleMesh::get_mesh (void) const
{
  return this->mesh;
}

inline void
GLTriangleMesh::release_mesh (void)
{
  this->mesh.reset();
}

inline void
GLTriangleMesh::set_mesh_shading (GLMeshShading shading)
{
  this->shading = shading;
}

inline void
GLTriangleMesh::set_mesh_decoration (int decoration)
{
  this->decoration = decoration;
}

inline GLMeshShading
GLTriangleMesh::get_mesh_shading (void) const
{
  return this->shading;
}

inline int
GLTriangleMesh::get_mesh_decoration (void) const
{
  return this->decoration;
}

inline void
GLTriangleMesh::set_shade_color (float r, float g, float b, float a)
{
  this->shade_color = Remesher::Vec4f(r, g, b, a);
  this->regen_face_shading();
}

inline void
GLTriangleMesh::set_decoration_color (float r, float g, float b, float a)
{
  this->deco_color = Remesher::Vec4f(r, g, b, a);
}

inline GLColorArray&
GLTriangleMesh::get_color_array (void)
{
  return this->color_array;
}

inline GLColorArray const&
GLTriangleMesh::get_color_array (void) const
{
  return this->color_array;
}

#endif /* GL_TRIANGLE_MESH_HEADER */
