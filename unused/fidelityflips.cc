#include <iostream>

#include "patch3d.h"
#include "exception.h"
#include "pttdist.h"
#include "elapsedtimer.h"
#include "fidelityflips.h"

REMESHER_NAMESPACE_BEGIN

bool
FidelityFlips::check_flip_edge (std::size_t v1idx, std::size_t v2idx,
    std::size_t v0idx, std::size_t v3idx,
    std::size_t /* face1 */, std::size_t /* face2 */)
{
  MeshVertexList const& verts = this->mesh->get_vertices();
  MeshFaceList const& rfaces = this->rmesh->get_faces();
  MeshVertexList const& rverts = this->rmesh->get_vertices();

  Vec3f const& v0 = verts[v0idx];
  Vec3f const& v1 = verts[v1idx];
  Vec3f const& v2 = verts[v2idx];
  Vec3f const& v3 = verts[v3idx];

  /* Center of the current edge. */
  Vec3f c1 = (v1 + v2) / 2.0f;
  /* Center of the edge if we would flip. */
  Vec3f c2 = (v0 + v3) / 2.0f;

  /* If the distance of the centers is small, bail out. */
  float edge_qlen = (v2 - v1).qlength();
  if ((c1 - c2).qlength() < (edge_qlen / (10.0f * 10.0f)))
    return false;

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

  /* Make sure we keep good triangles. */
  if (angle <= MY_PI / 2.0f)
    return false;

  /* Create a 3D patch from the region. */
  Patch3dTrianglePtr p3d = Patch3dTriangle::create(this->rmesh, this->rvinfo);
  p3d->create_patch(this->vref[v0idx].face,
      this->vref[v1idx].face, this->vref[v2idx].face);
  Patch3d::MeshFaceSet const& pfaces = p3d->get_face_set();

  /* Iterate over all faces in the patch an measure distance. */
  float c1qdist = 1.0f;
  float c2qdist = 1.0f;
  for (Patch3d::MeshFaceSet::const_iterator iter = pfaces.begin();
      iter != pfaces.end(); ++iter)
  {
    std::size_t fidx = *iter * 3;
    std::size_t rv1idx = rfaces[fidx + 0];
    std::size_t rv2idx = rfaces[fidx + 1];
    std::size_t rv3idx = rfaces[fidx + 2];

    /* Measure distance from centers to the reference face. */
    PTTDist ptt;
    ptt.set_triangle(rverts[rv1idx], rverts[rv2idx], rverts[rv3idx]);
    float c1qdist_tmp = ptt.get_squared_dist(c1);
    float c2qdist_tmp = ptt.get_squared_dist(c2);

    c1qdist = MY_MIN(c1qdist, c1qdist_tmp);
    c2qdist = MY_MIN(c2qdist, c2qdist_tmp);
  }

  /* If the distance of center of the existing edge is better, exit. */
  if (c1qdist <= c2qdist)
    return false;

  /* If the distance is not improved by at least a fifth
   * of the edge length, we don't flip the edge. */
  float qdiff = c1qdist - c2qdist;
  if (qdiff < (edge_qlen / (10.0f * 10.0f)))
    return false;

  return true;
}

REMESHER_NAMESPACE_END
