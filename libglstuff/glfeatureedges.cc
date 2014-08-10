#include <iostream>

#include <GL/gl.h>
#include "glfeatureedges.h"

void
GLFeatureEdges::draw_impl (void)
{
  if (this->mesh.get() == 0)
    return;

  if (this->features.get() == 0 || this->features->empty())
    return;

  Remesher::MeshVertexList const& verts = this->mesh->get_vertices();

  glDisable(GL_LIGHTING);
  glEnable(GL_POLYGON_OFFSET_LINE);
  glEnable(GL_BLEND);
  //glDisable(GL_DEPTH_TEST);

  glPolygonOffset(1.0f, -1.0f);
  glLineWidth(this->line_width);
  glColor4fv(&this->line_color[0]);
  glBegin(GL_LINES);
  for (std::size_t i = 0; i < this->features->size(); ++i)
  {
    Remesher::FeatureVertexEdges const& ve = this->features[i];
    for (std::size_t j = 0; j < ve.size(); ++j)
    {
      glVertex3fv(&verts[i][0]);
      glVertex3fv(&verts[ve[j]][0]);
    }
  }
  glEnd();
  glDisable(GL_POLYGON_OFFSET_LINE);

#if DRAW_CORNER_VERTS
  if (this->skel.get() == 0)
    return;

  glPolygonOffset(10.0f, -10.0f);
  glPointSize(5.0f);
  glColor4f(0.7f, 0.0f, 0.0f, this->line_color[3]);
  glEnable(GL_POLYGON_OFFSET_POINT);
  for (std::size_t i = 0; i < this->skel->size(); ++i)
  {
    Remesher::MeshBackbone const& bone(this->skel->at(i));
    if (bone.closed)
      continue;

    Remesher::Vec3f const& v1 = verts[bone.verts.front()];
    Remesher::Vec3f const& v2 = verts[bone.verts.back()];

    glBegin(GL_POINTS);
    glVertex3fv(&v1[0]);
    glVertex3fv(&v2[0]);
    glEnd();
  }
  glDisable(GL_POLYGON_OFFSET_POINT);
#endif

  glDisable(GL_BLEND);
  glLineWidth(1.0f);
}
