#include <GL/gl.h>

#include "libremesh/vec3.h"
#include "gledgemesh.h"

void
GLEdgeMesh::draw_impl (void)
{
  if (this->mesh.get() == 0 || this->edges.get() == 0)
    return;

  Remesher::EdgeList& el = this->edges->get_edges();
  Remesher::MeshVertexList& vertices = this->mesh->get_vertices();

  glColor3f(0.0f, 1.0f, 0.0f);
  glBegin(GL_LINES);
  for (unsigned int i = 0; i < el.size(); ++i)
  {
    glVertex3fv(&vertices[el[i].first][0]);
    glVertex3fv(&vertices[el[i].second][0]);
  }
  glEnd();
}
