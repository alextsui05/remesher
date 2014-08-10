#include "lloyd.h"

REMESHER_NAMESPACE_BEGIN

Lloyd::Lloyd (DelaunayPtr delaunay)
{
  this->delaunay = delaunay;
}

/* ---------------------------------------------------------------- */

DelaunayPtr
Lloyd::iteration (void)
{
  DelaunayPtr ret = Delaunay::create();

  std::vector<DPoint>& verts = this->delaunay->get_vertices();
  for (unsigned int i = 0; i < verts.size(); ++i)
  {
    if (verts[i].is_border)
    {
      ret->insert(verts[i].coord);
      continue;
    }

    Vec2f centroid(0.0f, 0.0f);
    for (unsigned int j = 0; j < verts[i].tris.size(); ++j)
      centroid += this->delaunay->calc_circumcenter(verts[i].tris[j]);
    centroid /= (float)verts[i].tris.size();

    int first, second;
    if (this->delaunay->border_interval(centroid, first, second))
    {
      ret->insert(verts[i].coord);
      continue;
    }

    ret->insert(centroid);
  }

  return ret;
}

REMESHER_NAMESPACE_END
