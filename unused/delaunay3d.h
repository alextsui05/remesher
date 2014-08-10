#ifndef REMESHER_DELAUNAY_3D_HEADER
#define REMESHER_DELAUNAY_3D_HEADER

#include "defines.h"
#include "trianglemesh.h"
#include "vertexinfo.h"
#include "featureedges.h"

REMESHER_NAMESPACE_BEGIN

class Delaunay3d
{
  private:
    TriangleMeshPtr mesh;
    VertexInfoList vinfo;
    FeatureEdges features;

  protected:
    std::size_t single_iteration (void);
    //std::size_t handle_simple_vertex (std::size_t index);
    //std::size_t handle_border_vertex (std::size_t index);
    //bool flip_edge (std::size_t f1, std::size_t f2);
    bool flip_edge_if_lnd (std::size_t v1idx, std::size_t v2idx);

  public:
    Delaunay3d (void);
    Delaunay3d (TriangleMeshPtr mesh, VertexInfoList vinfo);

    void set_mesh (TriangleMeshPtr mesh);
    void set_vertex_info (VertexInfoList vinfo);
    void set_feature_edges (FeatureEdges features);

    void restore_delaunay (void);
};

/* ---------------------------------------------------------------- */

inline
Delaunay3d::Delaunay3d (void)
{
}

inline
Delaunay3d::Delaunay3d (TriangleMeshPtr mesh, VertexInfoList vinfo)
{
  this->set_mesh(mesh);
  this->set_vertex_info(vinfo);
}

inline void
Delaunay3d::set_mesh (TriangleMeshPtr mesh)
{
  this->mesh = mesh;
}

inline void
Delaunay3d::set_vertex_info (VertexInfoList vinfo)
{
  this->vinfo = vinfo;
}

inline void
Delaunay3d::set_feature_edges (FeatureEdges features)
{
  this->features = features;
}

REMESHER_NAMESPACE_END

#endif /* REMESHER_DELAUNAY_3D_HEADER */
