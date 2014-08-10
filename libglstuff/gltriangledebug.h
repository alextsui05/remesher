#ifndef GL_TRIANGLE_DEBUG_HEADER
#define GL_TRIANGLE_DEBUG_HEADER

#include <vector>

#include "libremesh/refptr.h"
#include "libremesh/trianglemesh.h"

#include "gldrawable.h"
#include "fonttexture.h"

enum GLDebugRenderType
{
  DEBUG_RENDER_TYPE_NOTHING,
  DEBUG_RENDER_TYPE_VERTICES,
  DEBUG_RENDER_TYPE_FACES
};

/* ---------------------------------------------------------------- */

class GLTriangleDebug;
typedef RefPtr<GLTriangleDebug> GLTriangleDebugPtr;

class GLTriangleDebug : public GLDrawable
{
  private:
    typedef std::vector<FontTexturePtr> FontTextureList;

  private:
    GLDebugRenderType rtype;
    Remesher::TriangleMeshPtr mesh;
    FontTextureList tex_list;

  protected:
    GLTriangleDebug (void);
    void draw_impl (void);
    void generate_textures (std::size_t amount);
    void draw_vertex_textures (void);
    void draw_face_textures (void);

  public:
    static GLTriangleDebugPtr create (void);

    void set_mesh (Remesher::TriangleMeshPtr mesh);
    void set_render_type (GLDebugRenderType rtype);
    void clear_data (void);
};

/* ---------------------------------------------------------------- */

inline GLTriangleDebugPtr
GLTriangleDebug::create (void)
{
  return GLTriangleDebugPtr(new GLTriangleDebug);
}

inline void
GLTriangleDebug::set_mesh (Remesher::TriangleMeshPtr mesh)
{
  this->mesh = mesh;
}

inline void
GLTriangleDebug::set_render_type (GLDebugRenderType rtype)
{
  this->rtype = rtype;
  if (this->rtype == DEBUG_RENDER_TYPE_NOTHING)
     this->clear_data();
}

inline void
GLTriangleDebug::clear_data (void)
{
  this->tex_list.clear();
}

#endif /* GL_TRIANGLE_DEBUG_HEADER */
