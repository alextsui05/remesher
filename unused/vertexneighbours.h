#ifndef VERTEX_NEIGHBOURS_HEADER
#define VERTEX_NEIGHBOURS_HEADER

#include <vector>
#include "trianglemesh.h"
#include "defines.h"

REMESHER_NAMESPACE_BEGIN

class VertexNeighbours;
typedef RefPtr<VertexNeighbours> VertexNeighboursPtr;

class VertexNeighbours : public std::vector<std::vector<int> >
{
  protected:
    VertexNeighbours (TriangleMeshPtr mesh);
    void build_vertex_list (TriangleMeshPtr mesh, std::size_t vertex);

  public:
    static VertexNeighboursPtr create (TriangleMeshPtr mesh);
};

/* ---------------------------------------------------------------- */

inline VertexNeighboursPtr
VertexNeighbours::create (TriangleMeshPtr mesh)
{
  return VertexNeighboursPtr(new VertexNeighbours(mesh));
}

REMESHER_NAMESPACE_END

#endif /* VERTEX_NEIGHBOURS_HEADER */
