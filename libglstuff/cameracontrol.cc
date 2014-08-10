#include <cmath>
#include <iostream>
#include <GL/gl.h>

#include "cameracontrol.h"

void
CameraControl::setup_projection (ProjectionPref ppref)
{
  this->info.znear = 0.5f;
  this->info.zfar = 60.0f;

  float fov_factor;
  switch (ppref)
  {
    default:
    /* Right/left to +/- 0.1f results in FOV 22.62 degrees. */
    case PROJECTION_FOV_22: fov_factor = 0.1f; break;
    /* Right/left to +/- 0.2f results in FOV 43.60 degrees. */
    case PROJECTION_FOV_43: fov_factor = 0.2f; break;
    /* Right/left to +/- 0.3f results in FOV 61.93 degrees. */
    case PROJECTION_FOV_62: fov_factor = 0.3f; break;
  }

  float aspect = (float)this->info.width / (float)this->info.height;
  if (this->info.width > this->info.height)
  {
    this->info.left = -fov_factor * aspect;
    this->info.right = fov_factor * aspect;
    this->info.bottom = -fov_factor;
    this->info.top = fov_factor;
  }
  else
  {
    this->info.left = -fov_factor;
    this->info.right = fov_factor;
    this->info.bottom = -fov_factor / aspect;
    this->info.top = fov_factor / aspect;
  }

  glViewport(0, 0, (GLsizei)this->info.width, (GLsizei)this->info.height);
  //this->print_debug();
}

/* ---------------------------------------------------------------- */

void
CameraControl::apply_projection (void)
{
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glFrustum(this->info.left, this->info.right, this->info.bottom,
      this->info.top, this->info.znear, this->info.zfar);
}

/* ---------------------------------------------------------------- */

void
CameraControl::print_debug (void)
{
  /* FOV information. */
  float l1 = 2.0f * this->info.right;
  float l2 = std::sqrt(this->info.right * this->info.right
      + this->info.znear * this->info.znear);
  float hfov = std::acos((l1 * l1 - 2.0f * l2 * l2) / (-2.0f * l2 * l2));

  l1 = 2.0f * this->info.top;
  l2 = std::sqrt(this->info.top * this->info.top
      + this->info.znear * this->info.znear);
  float vfov = std::acos((l1 * l1 - 2.0f * l2 * l2) / (-2.0f * l2 * l2));

  std::cout << "Camera control information" << std::endl
      << "  Horizontal FOV: " << MY_RAD2DEG(hfov) << std::endl
      << "  Vertical FOV: " << MY_RAD2DEG(vfov) << std::endl;
}

