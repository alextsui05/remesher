#include <algorithm>
#include <iostream>

#include "exception.h"
#include "delaunay3d.h"

REMESHER_NAMESPACE_BEGIN

void
Delaunay3d::restore_delaunay (void)
{
  if (this->mesh.get() == 0)
    throw Exception("Delaunay3d: No mesh has been set");

  if (this->mesh->get_faces().empty())
    return;

  if (this->mesh->get_vertices().size() != this->vinfo.size())
    throw Exception("Vertex list does not match vertex info");

  std::size_t iteration = 0;
  std::size_t total_flips = 0;
  while (true)
  {
    std::size_t flips = this->single_iteration();
    total_flips += flips;
    std::cout << "Delaunay3d: Performed " << flips
        << " edge flips." << std::endl;

    iteration += 1;

    if (flips == 0)
      break;

    if (iteration >= 30)
      break;
  }

  std::cout << "Delaunay3d: Performed " << total_flips << " edge flips in "
      << iteration << " iterations." << std::endl;
}

/* ---------------------------------------------------------------- */

std::size_t
Delaunay3d::single_iteration (void)
{
  /*
   * Iterate over all faces and all three edges per face. Only edges
   * v1 -> v2 are taken if v1 < v2 (v1 and v2 are vertex indices).
   * An actual edge flip is performed if the minimum angle is increased.
   */

  MeshFaceList const& faces = this->mesh->get_faces();

  std::size_t edge_flips = 0;
  std::size_t face_amount = faces.size() / 3;
  for (std::size_t fi = 0; fi < face_amount; ++fi)
  {
    for (std::size_t i = 0; i < 3; ++i)
    {
      std::size_t ip1 = (i + 1) % 3;
      std::size_t v1idx = faces[fi * 3 + i];
      std::size_t v2idx = faces[fi * 3 + ip1];

      /* Only try to flip for vertex indices v1, v2 with v1 < v2. */
      if (v1idx > v2idx)
        continue;

      /* Only flip if we don't have a feature edge. This leads
       * to a constrained delaunay triangulation. */
      if (!this->features.empty()
          && this->features.is_feature_edge(v1idx, v2idx))
        continue;

      /* Try to flip. This will return true if flip has been performed. */
      if (this->flip_edge_if_lnd(v1idx, v2idx))
        edge_flips += 1;
    }
  }

  return edge_flips;
}

/* ---------------------------------------------------------------- */

bool
Delaunay3d::flip_edge_if_lnd (std::size_t v1idx, std::size_t v2idx)
{
  MeshFaceList& faces = this->mesh->get_faces();
  MeshVertexList const& verts = this->mesh->get_vertices();

  /* Get the two faces belonging to the edge.
   * We use the vertex info to do this job. */
  std::size_t face1, face2, v0idx, v3idx;
  bool unique = this->vinfo.get_edge_info(v1idx, v2idx,
      face1, face2, v0idx, v3idx);

  /* If the edge is used by more than two faces, skip it. */
  if (!unique)
  {
    //std::cout << "Skipping non-unique face constellation for "
    //    << v1idx << " and " << v2idx << std::endl;
    return false;
  }

  /* If we cannot find two adjacent faces, the edge is a border. */
  if (face1 == MAX_SIZE_T || face2 == MAX_SIZE_T)
    return false;

  /* If we found an opposive vertex but it's equal to v0idx, we have
   * two faces with the same vertices (in opposite direction). */
  if (v0idx == v3idx)
    return false;

  /* Compute the angle beween (v1 - v0) and (v2 - v0) and
   * the angle between (v1 - v3) and (v2 - v3). The sum of these
   * angles must be <= 180 degrees, otherwise we flip the edge. */
  float angle;
  {
    Vec3f e1 = (verts[v1idx] - verts[v0idx]).norm();
    Vec3f e2 = (verts[v2idx] - verts[v0idx]).norm();
    float scalar = e1.scalar(e2);
    scalar = MY_MIN(1.0f, scalar);
    scalar = MY_MAX(-1.0f, scalar);
    angle = std::acos(scalar);

    e1 = (verts[v1idx] - verts[v3idx]).norm();
    e2 = (verts[v2idx] - verts[v3idx]).norm();
    scalar = e1.scalar(e2);
    scalar = MY_MIN(1.0f, scalar);
    scalar = MY_MAX(-1.0f, scalar);
    angle += std::acos(scalar);
  }

#if REMESHER_NAN_CHECKS
  if (std::isnan(angle))
  {
    std::cout << "Delaunay3d: NAN angle detected!" << std::endl;
    ::exit(1);
  }
#endif

  /* No need to flip. */
  if (angle <= MY_PI)
    return false;
  else if (FLOAT_EQ(angle, MY_PI))
    return false;

  /* Flip edge. */
  //std::cout << "Flippig edge " << face1 << ", " << face2
  //    << ", got vertices: " << v1idx << ", " << v2idx << std::endl;

  /* Update vertex info. */
  this->vinfo[v0idx].adj_faces.push_back(face2);
  this->vinfo[v3idx].adj_faces.push_back(face1);

  if (!this->vinfo.remove_adjacent_face(v1idx, face2))
    throw Exception("Cannot find face2 in vertex v1");
  if (!this->vinfo.remove_adjacent_face(v2idx, face1))
    throw Exception("Cannot find face1 in vertex v2");

  /* Perform the edge flip. */
  faces[face1 * 3 + 0] = v0idx;
  faces[face1 * 3 + 1] = v1idx;
  faces[face1 * 3 + 2] = v3idx;
  faces[face2 * 3 + 0] = v0idx;
  faces[face2 * 3 + 1] = v3idx;
  faces[face2 * 3 + 2] = v2idx;

  /* Fix vertex info. */
  this->vinfo.order_and_classify(v0idx);
  this->vinfo.order_and_classify(v1idx);
  this->vinfo.order_and_classify(v2idx);
  this->vinfo.order_and_classify(v3idx);

  return true;
}

REMESHER_NAMESPACE_END
