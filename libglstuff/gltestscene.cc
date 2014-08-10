#include <cstdlib>
#include <GL/gl.h>

#include "libremesh/averageplane.h"
#include "gltestscene.h"

#define POINTS 100

GLTestScene::GLTestScene (void)
{
}

/* ---------------------------------------------------------------- */

void
GLTestScene::glinit_impl (void)
{
  for (unsigned int i = 0; i < POINTS; ++i)
  {
    float y = (float)(::rand() % 1000) / 500.0f - 1.0f;
    float x = (float)(::rand() % 1000) / 500.0f - 1.0f;
    float z = (float)(::rand() % 1000) / 500.0f - 1.0f;
    this->points.push_back(Remesher::Vec3f(x, y, z));
    //std::cout << x << " " << y << " " << z << std::endl;
  }

  Remesher::AveragePlane ap;
  this->plane = ap.get_average(this->points);
}

/* ---------------------------------------------------------------- */

void
GLTestScene::draw_impl (void)
{
  glDisable(GL_LIGHTING);
  glEnable(GL_BLEND);
  glPointSize(3.0f);
  glColor4f(1.0f, 1.0f, 1.0f, 0.7f);

  /* Draw points. */
  glVertexPointer(3, GL_FLOAT, 0, &this->points[0]);
  glEnableClientState(GL_VERTEX_ARRAY);
  glDrawArrays(GL_POINTS, 0, (GLsizei)this->points.size());
  glDisableClientState(GL_VERTEX_ARRAY);

  /* Draw plane. */
  Remesher::Vec3f n(this->plane.n);
  Remesher::Vec3f c(this->plane.n * this->plane.d);
  Remesher::Vec3f x(0.0f, 1.0f, 0.0f);
  Remesher::Vec3f v1(n.cross(x).norm());
  Remesher::Vec3f v2(n.cross(v1));
  Remesher::Vec3f p1(c + v1);
  Remesher::Vec3f p2(c + v2);
  Remesher::Vec3f p3(c - v1);
  Remesher::Vec3f p4(c - v2);

  glColor4f(0.0f, 0.0f, 1.0f, 0.6f);
  glBegin(GL_TRIANGLES);
    glVertex3fv(&c[0]);
    glVertex3fv(&p1[0]);
    glVertex3fv(&p2[0]);

    glVertex3fv(&c[0]);
    glVertex3fv(&p2[0]);
    glVertex3fv(&p3[0]);

    glVertex3fv(&c[0]);
    glVertex3fv(&p3[0]);
    glVertex3fv(&p4[0]);

    glVertex3fv(&c[0]);
    glVertex3fv(&p4[0]);
    glVertex3fv(&p1[0]);
  glEnd();

  glDisable(GL_BLEND);
}

/* ---------------------------------------------------------------- */


