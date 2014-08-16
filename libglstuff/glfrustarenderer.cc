#if defined(__APPLE__) && defined(__MACH__)
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include <iostream>
#include <fstream>
#include <cstring>
#include <cerrno>

#include "libremesh/matrix3.h"
#include "libremesh/helpers.h"
#include "glfrustarenderer.h"

struct BundleCamera
{
  float focalLength;
  float distort1;
  float distort2;
  float rot[9];
  Remesher::Vec3f trans;
};

/* ---------------------------------------------------------------- */

void
GLFrustaRenderer::init (std::string const& bundle_file)
{
  std::ifstream  in(bundle_file.c_str());
  if (!in.good())
  {
    std::cout << "Cannot open file for reading: "
      << ::strerror(errno) << std::endl;
    return;
  }

  std::string version;
  std::getline(in, version);
  Remesher::Helpers::chop_string(version);

  std::cout << "Reading bundle file " << bundle_file
    << ", version: " << version << "..." << std::endl;

  std::size_t numCameras, numPoints;
  in >> numCameras >> numPoints;

  std::cout << "Parsing " << numCameras << " cameras" << std::endl;

  this->frusta.clear();

  /* Read all cameras. */
  for (std::size_t i = 0; i < numCameras; ++i)
  {
    BundleCamera cam;
    in >> cam.focalLength >> cam.distort1 >> cam.distort2;
    for (int j = 0; j < 9; ++j)
      in >> cam.rot[j];
    for (int j = 0; j < 3; ++j)
      in >> cam.trans[j];

    Remesher::Vec3f la(0.0f, 0.0f, 1.0f);
    Remesher::Vec3f uv(0.0f, 1.0f, 0.0f);

    Remesher::Matrix3f mat;
    mat[0] = cam.rot[0]; mat[1] = cam.rot[1]; mat[2] = cam.rot[2];
    mat[3] = cam.rot[3]; mat[4] = cam.rot[4]; mat[5] = cam.rot[5];
    mat[6] = cam.rot[6]; mat[7] = cam.rot[7]; mat[8] = cam.rot[8];

    GLFrustaParams frustum;
    frustum.pos = mat.transpose().neg() * cam.trans;
    frustum.lookat = mat.transpose() * la;
    frustum.upvec = mat.transpose() * uv;

    this->frusta.push_back(frustum);
  }

  in.close();
}

/* ---------------------------------------------------------------- */

void
GLFrustaRenderer::draw_impl (void)
{
  glPointSize(5.0f);
  glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
  glBegin(GL_LINES);
  float scale = 0.05f;
  for (std::size_t i = 0; i < this->frusta.size(); ++i)
  {
    GLFrustaParams const& f(this->frusta[i]);

    if (i % 2) continue; // Drop some cameras
    if (f.pos[2] > 0.1f) continue;

    Remesher::Vec3f right = f.upvec.cross(f.lookat) / 3.0f;
    Remesher::Vec3f up = f.upvec / 3.0f;
    Remesher::Vec3f base(f.pos + f.lookat * scale);

    Remesher::Vec3f v1(base - right * scale - up * scale);
    Remesher::Vec3f v2(base + right * scale - up * scale);
    Remesher::Vec3f v3(base + right * scale + up * scale);
    Remesher::Vec3f v4(base - right * scale + up * scale);

    glVertex3fv(&f.pos[0]);
    glVertex3fv(&v1[0]);
    glVertex3fv(&f.pos[0]);
    glVertex3fv(&v2[0]);
    glVertex3fv(&f.pos[0]);
    glVertex3fv(&v3[0]);
    glVertex3fv(&f.pos[0]);
    glVertex3fv(&v4[0]);

    glVertex3fv(&v1[0]);
    glVertex3fv(&v2[0]);
    glVertex3fv(&v2[0]);
    glVertex3fv(&v3[0]);
    glVertex3fv(&v3[0]);
    glVertex3fv(&v4[0]);
    glVertex3fv(&v4[0]);
    glVertex3fv(&v1[0]);
  }
  glEnd();
}
