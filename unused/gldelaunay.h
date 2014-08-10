#ifndef GL_DELAUNAY_HEADER
#define GL_DELAUNAY_HEADER

#include "libremesh/delaunay.h"
#include "libremesh/refptr.h"
#include "gldrawable.h"

class GLDelaunay;
typedef RefPtr<GLDelaunay> GLDelaunayPtr;

class GLDelaunay : public GLDrawable
{
  private:
    Remesher::DelaunayPtr delaunay;
    int debug_vertex;
    bool draw_delaunay;
    bool draw_triangles;
    bool draw_voronoi;
    bool draw_border;
    bool draw_points;

  protected:
    GLDelaunay (void);
    void draw_impl (void);

  public:
    static GLDelaunayPtr create (void);
    void set_delaunay (Remesher::DelaunayPtr delaunay);

    void set_debug_vertex (int vertex);
    void set_draw_delaunay (bool value);
    void set_draw_voronoi (bool value);
};

/* ---------------------------------------------------------------- */

inline GLDelaunayPtr
GLDelaunay::create (void)
{
  return GLDelaunayPtr(new GLDelaunay);
}

inline void
GLDelaunay::set_delaunay (Remesher::DelaunayPtr delaunay)
{
  this->delaunay = delaunay;
}

inline void
GLDelaunay::set_debug_vertex (int vertex)
{
  this->debug_vertex = vertex;
}

inline void
GLDelaunay::set_draw_delaunay (bool value)
{
  this->draw_delaunay = value;
}

inline void
GLDelaunay::set_draw_voronoi (bool value)
{
  this->draw_voronoi = value;
}

#endif /* GL_DELAUNAY_HEADER */
