#ifndef GL_FEATURE_EDGES_HEADER
#define GL_FEATURE_EDGES_HEADER

#include "libremesh/vec4.h"
#include "libremesh/refptr.h"
#include "libremesh/trianglemesh.h"
#include "libremesh/featureedges.h"
#include "gldrawable.h"

#define DRAW_CORNER_VERTS 1
#ifdef DRAW_CORNER_VERTS
# include "libremesh/meshskeleton.h"
#endif

class GLFeatureEdges;
typedef RefPtr<GLFeatureEdges> GLFeatureEdgesPtr;

class GLFeatureEdges : public GLDrawable
{
  private:
    Remesher::TriangleMeshPtr mesh;
    Remesher::FeatureEdgesPtr features;
    Remesher::Vec4f line_color;
    float line_width;

#if DRAW_CORNER_VERTS
    Remesher::MeshSkeletonPtr skel;
#endif

  protected:
    GLFeatureEdges (void);
    void draw_impl (void);

  public:
    static GLFeatureEdgesPtr create (void);

    void set_mesh (Remesher::TriangleMeshPtr mesh);
    void set_feature_edges (Remesher::FeatureEdgesPtr features);
    void clear_data (void);

    void set_line_color (float r, float g, float b, float a);
    void set_line_width (float width);
};

/* ---------------------------------------------------------------- */

inline GLFeatureEdgesPtr
GLFeatureEdges::create (void)
{
  return GLFeatureEdgesPtr(new GLFeatureEdges);
}

inline
GLFeatureEdges::GLFeatureEdges (void)
{
  this->line_color = Remesher::Vec4f(0.8f, 0.0f, 0.0f, 1.0f);
  this->line_width = 3.0f;
}

inline void
GLFeatureEdges::set_mesh (Remesher::TriangleMeshPtr mesh)
{
  this->mesh = mesh;
}
#include <iostream>

inline void
GLFeatureEdges::set_feature_edges (Remesher::FeatureEdgesPtr features)
{
  this->features = features;
#if DRAW_CORNER_VERTS
  Remesher::VertexInfoListPtr vinfo
      = Remesher::VertexInfoList::create(this->mesh);
  features->set_vertex_info(vinfo);
  this->skel = Remesher::MeshSkeleton::create();
  this->skel->set_mesh(features->get_mesh());
  this->skel->set_feature_edges(features);
  this->skel->set_vertex_info(vinfo);
  this->skel->extract();
#endif
}

inline void
GLFeatureEdges::clear_data (void)
{
  this->mesh.reset();
  this->features.reset();
}

inline void
GLFeatureEdges::set_line_color (float r, float g, float b, float a)
{
  this->line_color = Remesher::Vec4f(r, g, b, a);
}

inline void
GLFeatureEdges::set_line_width (float width)
{
  this->line_width = width;
}

#endif /* GL_FEATURE_EDGES_HEADER */
