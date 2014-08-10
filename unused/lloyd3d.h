#ifndef REMESHER_LLOYD_3D_HEADER
#define REMESHER_LLOYD_3D_HEADER

#include "defines.h"
#include "thread.h"
#include "triangle3.h"
#include "trianglemesh.h"
#include "vertexinfo.h"
#include "densityfield.h"
#include "relocation.h"
#include "featureedges.h"

REMESHER_NAMESPACE_BEGIN

/*
 * Ths class implements a Lloyd relaxation performed directly on
 * the triangle mesh. It basically walks over every vertex on
 * the evolving mesh and moves the vertex to the centroid of it's
 * voronoi cell. The method is described in:
 *
 *   Vitaly Surazhsky, Pierre Alliez and Craig Gotsman
 *   Isotropic remeshing of surfaces: a local parameterization approach
 *
 * Relocation of the vertex is externally handled through a common
 * interface for all remeshing algorithms.
 */

/* ---------------------------------------------------------------- */

struct Lloyd3dConf
{
  /* Amount of lloyd iterations. */
  std::size_t iterations;

  Lloyd3dConf (void);
};

/* ---------------------------------------------------------------- */

class LloydThread;
class Lloyd3d
{
  friend class LloydThread;
  private:
    Lloyd3dConf config;
    TriangleMeshPtr rmesh;
    TriangleMeshPtr emesh;
    VertexInfoListPtr rvinfo;
    VertexInfoListPtr evinfo;
    DensityFieldPtr rdens;
    FeatureEdgesPtr features;
    Relocation reloc;

    VertexRefListPtr reflist;
    VertexRefListPtr reflist_new;

  protected:
    /* Process all vertices. */
    void handle_all_vertices (void);
    /* Process all vertices in several threads. */
    void handle_all_vertices_parallel (void);
    /* Processes vertices including start until excluding end. */
    void handle_vertices (std::size_t start, std::size_t end);
    /* Processes a single vertex. */
    void handle_vertex (std::size_t index);

  public:
    Lloyd3d (void);

    /* Set algorithm configuration. */
    void set_config (Lloyd3dConf const& config);

    /* Set algorithm data (all required). */
    void set_reference_mesh (TriangleMeshPtr rmesh);
    void set_reference_info (VertexInfoListPtr rvinfo);
    void set_evolving_mesh (TriangleMeshPtr emesh);
    void set_evolving_info (VertexInfoListPtr evinfo);
    void set_evolving_features (FeatureEdgesPtr features);
    void set_vertex_reflist (VertexRefListPtr reflist);
    void set_reference_density (DensityFieldPtr refdens);

    /* Start algorithm; perform iterations of Lloyd relaxation. */
    void start_lloyd_relaxation (void);
};

/* ---------------------------------------------------------------- */

class LloydThread : public Thread
{
  private:
    Lloyd3d* lloyd;
    std::size_t start;
    std::size_t end;

  private:
    void* run (void);

  public:
    LloydThread (Lloyd3d* lloyd, std::size_t start, std::size_t end);
};

/* ---------------------------------------------------------------- */

inline
Lloyd3dConf::Lloyd3dConf (void)
{
  this->iterations = 1;
}

inline
Lloyd3d::Lloyd3d (void)
{
}

inline void
Lloyd3d::set_config (Lloyd3dConf const& config)
{
  this->config = config;
}

inline void
Lloyd3d::set_reference_mesh (TriangleMeshPtr rmesh)
{
  this->rmesh = rmesh;
}

inline void
Lloyd3d::set_evolving_mesh (TriangleMeshPtr emesh)
{
  this->emesh = emesh;
}

inline void
Lloyd3d::set_reference_info (VertexInfoListPtr rvinfo)
{
  this->rvinfo = rvinfo;
}

inline void
Lloyd3d::set_evolving_info (VertexInfoListPtr evinfo)
{
  this->evinfo = evinfo;
}

inline void
Lloyd3d::set_evolving_features (FeatureEdgesPtr features)
{
  this->features = features;
}

inline void
Lloyd3d::set_vertex_reflist (VertexRefListPtr reflist)
{
  this->reflist = reflist;
}

inline void
Lloyd3d::set_reference_density (DensityFieldPtr refdens)
{
  this->rdens = refdens;
}

REMESHER_NAMESPACE_END

#endif /* REMESHER_LLOYD_3D_HEADER */
