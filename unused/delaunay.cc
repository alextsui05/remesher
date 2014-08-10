#include <iomanip>
#include <iostream>

#include "vec3.h"
#include "delaunay.h"

REMESHER_NAMESPACE_BEGIN

void
Delaunay::insert (Vec2f const& coord)
{
  /* Insert new vertex into the triangulation. */
  if (this->vertices.empty())
  {
    /* This is the first vertex and part of the border. */
    this->vertices.push_back(DPoint(coord));
    this->vertices.back().is_border = true;
    this->border.points.push_back(0);
    return;
  }

  if (this->vertices.size() == 1)
  {
    /* This is the second vertex and part of the border. */
    this->vertices.push_back(DPoint(coord));
    this->vertices.back().is_border = true;
    this->border.points.push_back(1);
    return;
  }

  /*
   * Insertion has two cases now.
   * 1. The new vertex is inside a triangle, subdividing a triangle
   * 2. The new vertex is outside the border, extending the convex hull
   */

  this->vertices.push_back(coord);
  unsigned int newidx = this->vertices.size() - 1;

  int first, second;
  if (this->border_interval(coord, first, second))
  {
    /*
     * Case 1: The new vertex is right of at least one edge of the border.
     * This means the new vertex is outside of the convex hull.
     */
    std::cout << "Inserting new border vertex ("
        << newidx << "): " << coord << " Affected: "
        << first << " " << second << std::endl;
    this->insert_border_vertex(newidx, first, second);
  }
  else
  {
    /*
     * Case 2: The new vertex is inside the triangulation, thus
     * in one of the triangles. Find the triangle and subdivide.
     */
    std::cout << "Inserting interior vertex: ("
        << newidx << "): " << coord << std::endl;
    this->insert_interior_vertex(newidx);
  }

  //std::cout << "Info: Amount of tris: " << this->mesh.size() << std::endl;
  //this->consistency_check();
}

/* ---------------------------------------------------------------- */

bool
Delaunay::border_interval (Vec2f const& v, int& first, int& second)
{
  first = -1;
  second = -1;
  bool first_matched = false;
  bool prev_matched = false;

  for (unsigned int i = 0; i < this->border.points.size(); ++i)
  {
    unsigned int ip1 = (i + 1) % this->border.points.size();
    Vec2f p(this->vertices[this->border.points[i]].coord);
    Vec2f m(this->vertices[this->border.points[ip1]].coord - p);

    /* Check if new point v is right of a border segment. */
    if (this->ceq(p, m, v) > 0)
    {
      if (first == -1) first = i;
      if (i == 0) first_matched = true;
      if (i > 0 && first_matched & !prev_matched && first != -1)
      {
        first = i;
        return true;
      }
      prev_matched = true;
    }
    else if (prev_matched)
    {
      second = i;
      prev_matched = false;
      if (!first_matched) return true;
    }
  }

  if (second == -1)
    second = 0;

  return first != -1 && second != -1;
}

/* ---------------------------------------------------------------- */

void
Delaunay::insert_border_vertex (unsigned int newidx, int first, int second)
{
  /* The new vertex is outside. Connect to existing border. */
  DPoint& pnt = this->vertices[newidx];
  pnt.is_border = true;
  unsigned int num_triangles = 0;
  int i = first;
  while (i != second)
  {
    /* Create new triangles. */
    unsigned int ip1 = (i + 1) % this->border.points.size();
    unsigned int v2idx = this->border.points[ip1];
    unsigned int v3idx = this->border.points[i];
    DTri tri(newidx, v2idx, v3idx);
    this->mesh.push_back(tri);
    unsigned int fidx = this->mesh.size() - 1;

    /* Notify vertices about the new triangles. */
    pnt.tris.insert(pnt.tris.begin(), fidx);
    this->vertices[v2idx].tris.push_back(fidx);
    this->vertices[v3idx].tris.insert
        (this->vertices[v3idx].tris.begin(), fidx);

    num_triangles += 1;

    /* Delete border vertices between first and second. */
    if (i != first)
    {
      this->vertices[this->border.points[i]].is_border = false;
      this->border.points.erase(this->border.points.begin() + i);
      if (second > i) second = second - 1;
      i = i % this->border.points.size();
    }
    else
    {
      i = (i + 1) % this->border.points.size();
    }
  }

  /* Insert the new vertex to the border. */
  if (second == 0)
    this->border.points.push_back(newidx);
  else
    this->border.points.insert(this->border.points.begin() + second, newidx);

  /* Restore delaunay property for inserted triangles. */
  for (unsigned int i = 0; i < num_triangles; ++i)
    this->restore_delaunay(newidx, this->mesh.size() - 1 - i);
}

/* ---------------------------------------------------------------- */

void
Delaunay::insert_interior_vertex (unsigned int newidx)
{
  Vec2f const& coord = this->vertices[newidx].coord;

  /* The new vervex is inside some triangle. Search it. */
  for (unsigned int i = 0; i < this->mesh.size(); ++i)
  {
    bool inside = true;
    int on_edge = -1;
    for (unsigned int j = 0; j < 3; ++j)
    {
      unsigned int jp1 = (j + 1) % 3;
      Vec2f p(this->vertices[this->mesh[i].points[j]].coord);
      Vec2f m(this->vertices[this->mesh[i].points[jp1]].coord - p);

      float ceq_value = this->ceq(p, m, coord);
      if (FLOAT_EQ(ceq_value, 0.0f))
      {
        /* Once the point is directly on an edge, consider that triangle. */
        on_edge = j;
      }
      else if (ceq_value > 0.0f)
      {
        /* Once the point is on the wrong side, skip triangle. */
        inside = false;
        break;
      }
    }

    if (!inside)
      continue;

    if (on_edge < 0)
    {
      //std::cout << "Info: Inside triangle " << i << std::endl;
      this->refine_face(i, newidx);
      return;
    }
    else
    {
      //std::cout << "Info: On edge of triangle " << i
      //    << " (edge " << this->mesh[i].points[on_edge] << " -> "
      //    << this->mesh[i].points[(on_edge + 1) % 3] << ")" << std::endl;
      this->refine_edge(i, on_edge, newidx);
      return;
    }
  }

  std::cerr << "Warning: New vertex was not inserted!" << std::endl;
}

/* ---------------------------------------------------------------- */

void
Delaunay::refine_face (unsigned int tri_idx, unsigned int pnt_idx)
{
  /* Create shorthands. tri MUST be a copy! */
  DTri tri(this->mesh[tri_idx]);
  DPoint& pnt = this->vertices[pnt_idx];
  DPoint& pnt1 = this->vertices[tri.points[0]];
  DPoint& pnt2 = this->vertices[tri.points[1]];
  DPoint& pnt3 = this->vertices[tri.points[2]];

  /* Create new triangles and add it to the mesh. */
  DTri new1 = DTri(tri.points[0], tri.points[1], pnt_idx);
  DTri new2 = DTri(tri.points[1], tri.points[2], pnt_idx);
  DTri new3 = DTri(tri.points[2], tri.points[0], pnt_idx);
  this->mesh[tri_idx] = new1;
  this->mesh.push_back(new2);
  this->mesh.push_back(new3);

  /* Notify vertices about changed triangles. */
  pnt.tris.push_back(tri_idx);
  pnt.tris.push_back(this->mesh.size() - 2);
  pnt.tris.push_back(this->mesh.size() - 1);
  pnt1.replace_tri(tri_idx, tri_idx, this->mesh.size() - 1);
  pnt2.replace_tri(tri_idx, this->mesh.size() - 2, tri_idx);
  pnt3.replace_tri(tri_idx, this->mesh.size() - 1, this->mesh.size() - 2);

  /* Restore delaunay property. */
  this->restore_delaunay(pnt_idx, tri_idx);
  this->restore_delaunay(pnt_idx, this->mesh.size() - 1);
  this->restore_delaunay(pnt_idx, this->mesh.size() - 2);
}

/* ---------------------------------------------------------------- */

void
Delaunay::refine_edge (unsigned int tri_idx, unsigned int edge_idx,
    unsigned int pnt_idx)
{
  Vec2f const& coord = this->vertices[pnt_idx].coord;
  DTri const& tri(this->mesh[tri_idx]);
  unsigned int e1 = edge_idx;
  unsigned int e2 = (edge_idx + 1) % 3;

  /* Check if we have a similar point already. */
  if (this->vertices[tri.points[e1]].coord.eps_equals(coord)
      || this->vertices[tri.points[e2]].coord.eps_equals(coord))
  {
    std::cerr << "Warning: Point already present!" << std::endl;
    this->vertices.erase(this->vertices.begin() + pnt_idx);
    return;
  }

  unsigned int ev1 = this->mesh[tri_idx].points[e1];
  unsigned int ev2 = this->mesh[tri_idx].points[e2];
  unsigned int ev3 = this->mesh[tri_idx].points[(e2 + 1) % 3];

  /* We need the opposite triangle. If this does not exist,
   * we refine the border. */
  int atri_idx = this->find_adjacent_tri(ev1, ev2, tri_idx);
  if (atri_idx < 0)
  {
    /* It's a border edge. Insert new border vertex. */
    DTri new1(ev3, ev1, pnt_idx);
    DTri new2(ev2, ev3, pnt_idx);
    this->mesh[tri_idx] = new1;
    this->mesh.push_back(new2);

    /* Update triangles. */
    this->vertices[ev2].replace_tri(tri_idx, this->mesh.size() - 1);
    this->vertices[ev3].replace_tri(tri_idx, tri_idx, this->mesh.size() - 1);
    this->vertices[pnt_idx].tris.push_back(tri_idx);
    this->vertices[pnt_idx].tris.push_back(this->mesh.size() - 1);

    for (unsigned int i = 0; i < this->border.points.size(); ++i)
      if (this->border.points[i] == ev2)
      {
        this->border.points.insert(this->border.points.begin() + i, pnt_idx);
        break;
      }

    this->restore_delaunay(pnt_idx, tri_idx);
    this->restore_delaunay(pnt_idx, this->mesh.size() - 1);
  }
  else
  {
    /* It's a common edge. Subdivide the edge. */
    DTri const& atri = this->mesh[atri_idx];
    unsigned int av = this->vertices.size();
    for (unsigned int i = 0; i < 3; ++i)
      if (atri.points[i] != ev1 && atri.points[i] != ev2)
      {
        av = atri.points[i];
        break;
      }

    if (av == this->vertices.size())
    {
      std::cerr << "Error: Cannot find third vertex" << std::endl;
      return;
    }

    /* Create new triangles. */
    DTri new1(ev3, ev1, pnt_idx);
    DTri new2(ev1, av, pnt_idx);
    DTri new3(av, ev2, pnt_idx);
    DTri new4(ev2, ev3, pnt_idx);
    this->mesh[tri_idx] = new1;
    this->mesh[atri_idx] = new2;
    this->mesh.push_back(new3);
    this->mesh.push_back(new4);

    /* Update triangle references for the vertices. */
    this->vertices[ev2].replace_tri(tri_idx, this->mesh.size() - 1);
    this->vertices[ev2].replace_tri(atri_idx, this->mesh.size() - 2);
    this->vertices[ev3].replace_tri(tri_idx, tri_idx, this->mesh.size() - 1);
    this->vertices[av].replace_tri(atri_idx, this->mesh.size() - 2, atri_idx);
    this->vertices[pnt_idx].tris.push_back(tri_idx);
    this->vertices[pnt_idx].tris.push_back(atri_idx);
    this->vertices[pnt_idx].tris.push_back(this->mesh.size() - 2);
    this->vertices[pnt_idx].tris.push_back(this->mesh.size() - 1);

    /* Update delaunay trianglulation. */
    this->restore_delaunay(pnt_idx, tri_idx);
    this->restore_delaunay(pnt_idx, atri_idx);
    this->restore_delaunay(pnt_idx, this->mesh.size() - 1);
    this->restore_delaunay(pnt_idx, this->mesh.size() - 2);
  }
}

/* ---------------------------------------------------------------- */

int
Delaunay::find_adjacent_tri (unsigned int v1, unsigned int v2,
    unsigned int tri_idx)
{
  DPoint const& ep1 = this->vertices[v1];
  DPoint const& ep2 = this->vertices[v2];

  /* Find the triangle adjacent to the edge. */
  for (unsigned int i = 0; i < ep1.tris.size(); ++i)
    for (unsigned int j = 0; j < ep2.tris.size(); ++j)
      if (ep1.tris[i] != tri_idx && ep1.tris[i] == ep2.tris[j])
        return ep1.tris[i];

  return -1;
}

/* ---------------------------------------------------------------- */

void
Delaunay::restore_delaunay (unsigned int pnt_idx, unsigned int tri_idx)
{
  return;
  /* Find the edge opposite of the given point. */
  DTri& tri = this->mesh[tri_idx];

  unsigned int ev1, ev2;
  for (unsigned int i = 0; i < 3; ++i)
    if (tri.points[i] == pnt_idx)
    {
      ev1 = tri.points[(i + 1) % 3];
      ev2 = tri.points[(i + 2) % 3];
    }

  /* Find the triangle adjacent to the edge. */
  int atri_idx = this->find_adjacent_tri(ev1, ev2, tri_idx);

  /* Check if the adjacent triangle exists. If not, delaunay is valid. */
  if (atri_idx < 0)
  {
    //std::cerr << "Info: There is no adjacent triangle." << std::endl;
    return;
  }

  //std::cout << "Checking delaunay for point " << pnt_idx << " ("
  //    << tri_idx << ") and triangle " << atri_idx << std::endl;

  /* Check if the given point is inside the circumcenter of the triangle. */
  if (!this->point_in_circumcenter(pnt_idx, atri_idx))
    return;

  /* Point is inside circumcenter. Flip edge. */
  DTri& atri = this->mesh[atri_idx];
  unsigned int av1 = this->vertices.size();
  for (unsigned int i = 0; i < 3; ++i)
    if (atri.points[i] != ev1 && atri.points[i] != ev2)
    {
      av1 = atri.points[i];
      break;
    }

  if (av1 == this->vertices.size())
  {
    std::cerr << "Error: Could not find third vertex." << std::endl;
    return;
  }

  //std::cout << "Flipping edge for triangles "
  //    << tri_idx << " and " << atri_idx << std::endl;

  /* Current edge is ev1 -> ev2. We flip it to pnt_idx -> av1. */
  DTri new1(pnt_idx, ev1, av1);
  DTri new2(av1, ev2, pnt_idx);
  this->mesh[tri_idx] = new1;
  this->mesh[atri_idx] = new2;

  /* Updating vertices. */
  this->vertices[pnt_idx].replace_tri(tri_idx, tri_idx, atri_idx);
  this->vertices[ev1].remove_tri(atri_idx);
  this->vertices[av1].replace_tri(atri_idx, atri_idx, tri_idx);
  this->vertices[ev2].remove_tri(tri_idx);

  /*
  this->vertices[pnt_idx].tris.push_back(atri_idx);
  this->vertices[ev1].remove_tri(atri_idx);
  this->vertices[av1].tris.push_back(tri_idx);
  this->vertices[ev2].remove_tri(tri_idx);
  */

  /* Some edges are now not guaranteed to be delaunay. Fix them. */
  this->restore_delaunay(pnt_idx, tri_idx);
  this->restore_delaunay(pnt_idx, atri_idx);
  //this->restore_delaunay(av1, tri_idx);
  //this->restore_delaunay(av1, atri_idx);
}

/* ---------------------------------------------------------------- */

bool
Delaunay::point_in_circumcenter (unsigned int pnt_idx, unsigned int tri_idx)
{
  DTri const& tri = this->mesh[tri_idx];
  Vec2f const& a(this->vertices[tri.points[0]].coord);
  Vec2f const& b(this->vertices[tri.points[1]].coord);
  Vec2f const& c(this->vertices[tri.points[2]].coord);
  Vec2f const& d(this->vertices[pnt_idx].coord);

  float axdx = a[0] - d[0];
  float bxdx = b[0] - d[0];
  float cxdx = c[0] - d[0];
  float aydy = a[1] - d[1];
  float bydy = b[1] - d[1];
  float cydy = c[1] - d[1];
  float axdxaydy = axdx * axdx + aydy * aydy;
  float bxdxbydy = bxdx * bxdx + bydy * bydy;
  float cxdxcydy = cxdx * cxdx + cydy * cydy;

  float det = axdx * bydy * cxdxcydy
      + bxdx * cydy * axdxaydy
      + cxdx * aydy * bxdxbydy
      - cxdx * bydy * axdxaydy
      - bxdx * aydy * cxdxcydy
      - axdx * cydy * bxdxbydy;

  /* If determinant is positive, given point is inside circumcenter. */
  return det > 0.0f;
}

/* ---------------------------------------------------------------- */

Vec2f
Delaunay::calc_circumcenter (unsigned int tri_idx)
{
  DTri const& tri = this->mesh[tri_idx];
  DPoint const& p1 = this->vertices[tri.points[0]];
  DPoint const& p2 = this->vertices[tri.points[1]];
  DPoint const& p3 = this->vertices[tri.points[2]];

  Vec3f bs1, bs2;
  {
    Vec2f d = p1.coord - p2.coord;
    Vec2f s = p1.coord + p2.coord;
    bs1 = Vec3f(d[0], d[1], d.scalar(s) / -2.0f);

    d = p2.coord - p3.coord;
    s = p2.coord + p3.coord;
    bs2 = Vec3f(d[0], d[1], d.scalar(s) / -2.0f);
  }

  Vec3f c = bs1.cross(bs2);
  return Vec2f(c[0] / c[2], c[1] / c[2]);
}

/* ---------------------------------------------------------------- */

void
Delaunay::consistency_check (void)
{
  for (unsigned int i = 0; i < this->mesh.size(); ++i)
  {
    DTri& tri = this->mesh[i];

    /* Check if points for the triangle have the triangle as ref. */
    for (unsigned int j = 0; j < 3; ++j)
    {
      unsigned int pidx = tri.points[j];
      if (pidx >= this->vertices.size())
        std::cerr << "Warning: Point index out of bounds." << std::endl;

      DPoint& p = this->vertices[pidx];
      bool found = false;
      for (unsigned int k = 0; k < p.tris.size(); ++k)
        if (p.tris[k] == i)
          found = true;

      if (!found)
      {
        std::cerr << "Warning: Triangle vertex (" << pidx
            << ") misses triangle (" << i << ")." << std::endl;
        tri.flagged = true;
      }
    }

    /* Check if the triangle is free of points in the circumcenter. */

    #if 0
    /* Variation that checks against neighboring points. */
    for (unsigned int j = 0; j < 3 && !tri.flagged; ++j)
    {
      DPoint const& p = this->vertices[tri.points[j]];
      for (unsigned int k = 0; k < p.tris.size() && !tri.flagged; ++k)
        for (unsigned int l = 0; l < 3 && !tri.flagged; ++l)
        {
          unsigned int pidx = this->mesh[p.tris[k]].points[l];
          if (tri.points[0] != pidx && tri.points[1] != pidx
              && tri.points[2] != pidx && this->point_in_circumcenter(pidx, i))
          {
            #if 0
            std::cout << "Warning: Point in circumcenter! "
                << this->vertices[j].coord << std::endl;
            std::cout << "  Triangle " << i << ": " << tri.points[0]
                << " " << tri.points[1] << " " << tri.points[2] << std::endl;
            #endif
            tri.flagged = true;
          }
      }
    }
    #endif

    /* Variation to check circumcenter for all vertices. */
    for (unsigned int j = 0; j < this->vertices.size(); ++j)
    {
      if (tri.points[0] != j && tri.points[1] != j
              && tri.points[2] != j && this->point_in_circumcenter(j, i))
      {
        tri.flagged = true;
        std::cout << "Warning: Triangle is non-delaunay: " << i << std::endl;
      }

    }

  }

  for (unsigned int i = 0; i < this->vertices.size(); ++i)
    if (this->vertices[i].tris.size() == 0)
      std::cout << "Warning: Vertex without triangles: "
          << i << ", " << this->vertices[i].coord << std::endl;

  #if 0
  std::cout << "Info: Current border is: ";
  for (unsigned int i = 0; i < this->border.points.size(); ++i)
    std::cout << this->border.points[i] << " ";
  std::cout << std::endl;
  #endif
}

REMESHER_NAMESPACE_END
