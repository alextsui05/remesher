#ifndef REMESHER_EDGE_MESH_HEADER
#define REMESHER_EDGE_MESH_HEADER

#include "defines.h"
#include "refptr.h"

REMESHER_NAMESPACE_BEGIN

class EdgeMesh;
typedef RefPtr<EdgeMesh> EdgeMeshPtr;
typedef std::vector<std::pair<unsigned int, unsigned int> > EdgeList;

class EdgeMesh
{
  protected:
    std::vector<Vec3f> vertices;
    EdgeList edges;

  protected:
    EdgeMesh (void);

  public:
    static EdgeMeshPtr create (void);

    std::vector<Vec3f> const& get_vertices (void) const;
    EdgeList const& get_edges (void) const;

    std::vector<Vec3f>& get_vertices (void);
    EdgeList& get_edges (void);
};

/* ---------------------------------------------------------------- */

inline
EdgeMesh::EdgeMesh (void)
{
}

inline EdgeMeshPtr
EdgeMesh::create (void)
{
  return EdgeMeshPtr(new EdgeMesh);
}

inline std::vector<Vec3f> const&
EdgeMesh::get_vertices (void) const
{
  return this->vertices;
}

inline std::vector<Vec3f>&
EdgeMesh::get_vertices (void)
{
  return this->vertices;
}

inline EdgeList const&
EdgeMesh::get_edges (void) const
{
  return this->edges;
}

inline EdgeList&
EdgeMesh::get_edges (void)
{
  return this->edges;
}

REMESHER_NAMESPACE_END

#endif /* REMESHER_EDGE_MESH_HEADER */
