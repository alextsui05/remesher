#include <GL/gl.h>

#include "gldelaunay.h"

GLDelaunay::GLDelaunay (void)
{
  this->debug_vertex = -1;
  this->draw_delaunay = true;
  this->draw_triangles = true;
  this->draw_voronoi = true;
  this->draw_border = true;
  this->draw_points = true;
}

/* ---------------------------------------------------------------- */

void
GLDelaunay::draw_impl (void)
{
  if (this->delaunay.get() == 0)
    return;

  std::vector<Remesher::DPoint> const& verts(this->delaunay->get_vertices());
  std::vector<Remesher::DTri> const& tris(this->delaunay->get_mesh());
  Remesher::DPoly const& border(this->delaunay->get_border());

  glDisable(GL_LIGHTING);
  glDepthFunc(GL_ALWAYS);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  /* Draw triangles. */
  glColor4f(0.0f, 0.5f, 1.0f, 0.2f);
  glEnable(GL_BLEND);
  glBegin(GL_TRIANGLES);
  for (unsigned int i = 0; i < tris.size() && this->draw_triangles; ++i)
  {
    if (tris[i].flagged)
      glColor4f(0.8f, 0.3f, 0.3f, 0.2f);
    else
      glColor4f(0.0f, 0.5f, 1.0f, 0.2f);

    for (unsigned int j = 0; j < 3; ++j)
    {
      Remesher::Vec2f const& p(verts[tris[i].points[j]].coord);
      glVertex3f(p[0], p[1], 0.0f);
    }
  }
  glEnd();
  glDisable(GL_BLEND);

  /* Draw debug triangle. */
  if (this->debug_vertex >= 0 && this->debug_vertex < (int)verts.size())
  {
    Remesher::DPoint const& pnt(verts[this->debug_vertex]);
    glBegin(GL_TRIANGLES);
    for (unsigned int i = 0; i < pnt.tris.size(); ++i)
    {
      Remesher::DTri const& tri(tris[pnt.tris[i]]);
      glColor3f(1.0f, (float)i / (float)pnt.tris.size(), 0.0f);
      for (unsigned int j = 0; j < 3; ++j)
      {
        Remesher::Vec2f const& p(verts[tri.points[j]].coord);
        glVertex3f(p[0], p[1], 0.0f);
      }
    }
    glEnd();

    glColor3f(0.5f, 0.5f, 1.0f);
    glPointSize(5.0f);
    glBegin(GL_POINTS);
    glVertex3f(pnt.coord[0], pnt.coord[1], 0.0f);
    glEnd();
  }

  /* Draw delaunay. */
  glColor3f(0.0f, 0.5f, 1.0f);
  glPolygonMode(GL_FRONT, GL_LINE);
  glPolygonMode(GL_BACK, GL_FILL);
  glBegin(GL_TRIANGLES);
  for (unsigned int i = 0; i < tris.size() && this->draw_delaunay; ++i)
    for (unsigned int j = 0; j < 3; ++j)
    {
      Remesher::Vec2f const& p(verts[tris[i].points[j]].coord);
      glVertex3f(p[0], p[1], 0.0f);
    }
  glEnd();
  glPolygonMode(GL_FRONT, GL_FILL);

  /* Draw voronoi. */
  glColor3f(1.0f, 0.0f, 0.0f);
  for (unsigned int i = 0; i < verts.size() && this->draw_voronoi; ++i)
  {
    if (verts[i].is_border && i >= verts.size() - 1)
      break;

    Remesher::DPoint const& point(verts[i]);
    glBegin(GL_LINES);
    for (unsigned int j = 0; j < point.tris.size(); ++j)
    {
      Remesher::Vec2f cc1 = this->delaunay->calc_circumcenter(point.tris[j]);
      Remesher::Vec2f cc2 = this->delaunay->calc_circumcenter(point.tris[(j + 1) % point.tris.size()]);
      glVertex3f(cc1[0], cc1[1], 0.0f);
      glVertex3f(cc2[0], cc2[1], 0.0f);
    }
    glEnd();
  }

  /* Draw border line. */
  glColor3f(0.0f, 1.0f, 0.0f);
  glBegin(GL_LINES);
  for (unsigned int i = 0; i < border.points.size() && this->draw_border; ++i)
  {
    unsigned int ip1 = (i + 1) % border.points.size();
    Remesher::Vec2f const& a(verts[border.points[i]].coord);
    Remesher::Vec2f const& b(verts[border.points[ip1]].coord);
    glVertex3f(a[0], a[1], 0.0f);
    glVertex3f(b[0], b[1], 0.0f);
  }
  glEnd();

  /* Draw vertices. */
  glColor3f(1.0f, 0.0f, 0.0f);
  if (verts.size() > 1500)
    glPointSize(1.0f);
  else if (verts.size() > 500)
    glPointSize(2.0f);
  else
    glPointSize(3.0f);
  glBegin(GL_POINTS);
  for (unsigned int i = 0; i < verts.size() && this->draw_points; ++i)
  {
    if (verts[i].is_border)
      glColor3f(1.0f, 1.0f, 0.0f);
    else
      glColor3f(1.0f, 0.0f, 0.0f);

    glVertex3f(verts[i].coord[0], verts[i].coord[1], 0.0f);
  }
  glEnd();

  glDepthFunc(GL_LESS); // Reset DepthFunc to default
}
