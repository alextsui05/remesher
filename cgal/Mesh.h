#ifndef REMESHER_CGAL_MESH_H
#define REMESHER_CGAL_MESH_H

#include <CGAL/Cartesian.h>
#include <CGAL/Polyhedron_3.h>
#include "libremesh/defines.h"

REMESHER_NAMESPACE_BEGIN
namespace CGAL {

typedef ::CGAL::Cartesian< double > Kernel;

template < class Refs, class Point >
class Vertex : public ::CGAL::HalfedgeDS_vertex_base< Refs, ::CGAL::Tag_true, Point >
{
public:
    typedef ::CGAL::HalfedgeDS_vertex_base< Refs, ::CGAL::Tag_true, Point > Base;

};

template < class Refs >
class Halfedge : public ::CGAL::HalfedgeDS_halfedge_base< Refs >
{
public:
    typedef ::CGAL::HalfedgeDS_halfedge_base< Refs > Base;

};

template < class Refs, class Plane >
class Facet : public ::CGAL::HalfedgeDS_face_base< Refs, ::CGAL::Tag_true, Plane >
{
public:
    typedef ::CGAL::HalfedgeDS_face_base< Refs, ::CGAL::Tag_true, Plane > Base;

};

struct PolyhedronItems : public ::CGAL::Polyhedron_items_3
{
    template < class Refs, class Traits >
    struct Vertex_wrapper
    {
        typedef Vertex< Refs, typename Traits::Point_3 > Vertex;
    };

    template < class Refs, class Traits>
    struct Halfedge_wrapper
    {
        typedef Halfedge< Refs > Halfedge;
    };

    template < class Refs, class Traits>
    struct Face_wrapper
    {
        typedef Facet< Refs, typename Traits::Plane_3 > Face;
    };
};

class Mesh : public ::CGAL::Polyhedron_3< Kernel, PolyhedronItems >
{

// TODO: Create a static factory method that takes Remesher::TriangleMesh and makes a Remesher::CGAL::Mesh.
//TriangleMesh::create_copy (bool with_normals) const
//{
//  TriangleMeshPtr ret(new TriangleMesh);
//  ret->get_vertices() = this->vertices;
//  ret->get_faces() = this->faces;
//  if (with_normals)
//  {
//    ret->get_vertex_normals() = this->vertex_normals;
//    ret->get_face_normals() = this->face_normals;
//  }
//  return ret;
//}

};

} // namespace CGAL
REMESHER_NAMESPACE_END


#endif // REMESHER_CGAL_MESH_H
