#include <iostream>
#include <GL/gl.h>

#include "libremesh/vec4.h"
#include "libremesh/quaternion.h"
#include "libremesh/defines.h"
#include "cameratrackball.h"

CameraTrackball::CameraTrackball (void)
{
  this->reset_view();
}

/* ---------------------------------------------------------------- */

void
CameraTrackball::set_mouse_move (MouseButtonMask mask, int x, int y)
{
  if (mask & MOUSE_BUTTON_1)
  {
    Remesher::Vec3f cur_vec = this->get_pointer_vector((float)x, (float)y);
    Remesher::Vec3f cur_axis = this->click_vec.cross(cur_vec).norm();
    float cur_angle = (float)::acos(this->click_vec.scalar(cur_vec));

    Remesher::Quat4f delta, orig;
    delta.set_axis_angle(cur_axis.array(), cur_angle);
    orig.set_axis_angle(this->click_axis.array(), this->click_angle);

    orig = delta * orig;
    orig.get_axis_angle(this->rot_axis.array(), this->rot_angle);
  }

  if (mask & MOUSE_BUTTON_2)
  {
    this->scale = this->click_scale
        * (float)((y - this->click_scale_y + 100)) / 100.0f;
    this->scale = MY_MAX(0.01f, this->scale);
    this->scale = MY_MIN(1000.0f, this->scale);
  }
}

/* ---------------------------------------------------------------- */

void
CameraTrackball::set_mouse_down (int button, int x, int y)
{
  if (button == 1)
  {
    this->click_vec = this->get_pointer_vector((float)x, (float)y);
    this->click_axis = this->rot_axis;
    this->click_angle = this->rot_angle;
  }

  if (button == 2)
  {
    this->click_scale_y = y;
    this->click_scale = this->scale;
  }

  if (button == 3)
  {
    Remesher::Vec3f new_center = this->get_new_center(x, y);
    Remesher::Vec3f rot_center;
    Remesher::Quat4f quat;
    quat.set_axis_angle(this->rot_axis.array(), this->rot_angle);
    quat.rotate_vector(new_center.array(), rot_center.array());

    this->move_vec = this->move_vec - rot_center / this->scale;
  }
}

/* ---------------------------------------------------------------- */

void
CameraTrackball::set_mouse_up (int button, int x, int y)
{
  button = 0;
  x = y = 0.0f;
}

/* ---------------------------------------------------------------- */

void
CameraTrackball::reset_view (void)
{
  this->move_vec = Remesher::Vec3f(0.0f, 0.0f, 0.0f);
  this->rot_axis = Remesher::Vec3f(1.0f, 0.0f, 0.0f);
  this->rot_angle = 0.0f;
  this->scale = 1.0f;

}

/* ---------------------------------------------------------------- */

Remesher::Vec3f
CameraTrackball::get_pointer_vector (float x, float y)
{
  float sphere_x = 2.0f * x / (float)this->info.width - 1.0f;
  float sphere_y = 1.0f - 2.0f * y / (float)this->info.height;

  float z2 = 1.0f - sphere_x * sphere_x - sphere_y * sphere_y;
  float sphere_z = z2 > 0.0f ? std::sqrt(z2) : 0.0f;

  return Remesher::Vec3f(sphere_x, sphere_y, sphere_z).norm();
}

/* ---------------------------------------------------------------- */

Remesher::Vec3f
CameraTrackball::get_new_center (int x, int y)
{
  /* Read depth value from OpenGL. */
  float depthvalue;
  glReadPixels(x, this->info.height - y, 1, 1,
      GL_DEPTH_COMPONENT, GL_FLOAT, &depthvalue);

  /* Return zero center if there is no depth value. */
  if (depthvalue == 1.0f)
    return Remesher::Vec3f(0.0f, 0.0f, 0.0f);

  /* Calculate distance from depth buffer. */
  float near = this->info.znear;
  float far = this->info.zfar;
  float distance = far * near / ((near - far) * depthvalue + far);

  CameraInfo const& ci = this->info;
  Remesher::Vec3f span[4];
  span[0] = Remesher::Vec3f(ci.left, ci.bottom, -ci.znear);
  span[1] = Remesher::Vec3f(ci.right, ci.bottom, -ci.znear);
  span[2] = Remesher::Vec3f(ci.right, ci.top, -ci.znear);
  span[3] = Remesher::Vec3f(ci.left, ci.top, -ci.znear);

  float fx = (float)x;
  float fy = (float)y;
  float fw = (float)ci.width;
  float fh = (float)ci.height;

  Remesher::Vec3f top_vec(span[2] * (fx / fw) + span[3] * (1.0f - fx / fw));
  Remesher::Vec3f bot_vec(span[1] * (fx / fw) + span[0] * (1.0f - fx / fw));
  Remesher::Vec3f new_center(bot_vec * (fy / fh) + top_vec * (1.0f - fy / fh));
  new_center = new_center.norm() * distance;
  new_center[2] += CAM_TRACKBALL_Z_DIST;

  return new_center;
}

/* ---------------------------------------------------------------- */

void
CameraTrackball::apply_modelview (ModelviewMask mask)
{
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  if (mask & MODELVIEW_TRANSLATE)
    glTranslatef(0.0f, 0.0f, -CAM_TRACKBALL_Z_DIST);

  if (mask & MODELVIEW_ROTATE)
    glRotatef((float)(MY_RAD2DEG(this->rot_angle)), this->rot_axis[0],
        this->rot_axis[1], this->rot_axis[2]);

  if (mask & MODELVIEW_SCALE)
    glScalef(this->scale, this->scale, this->scale);

  if (mask & MODELVIEW_TRANSLATE)
    glTranslatef(this->move_vec[0], this->move_vec[1], this->move_vec[2]);

#if 0
  Remesher::Vec3f move(move_x, move_y, move_z);
  Remesher::Vec3f newm;
  Remesher::Quat4f quat;
  quat.set_axis_angle(this->rot_axis.array(), -this->rot_angle);
  quat.rotate_vector(move.array(), newm.array());

  glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
  glBegin(GL_LINES);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(newm[0], newm[1], newm[2]);
  glEnd();
  glTranslatef(newm[0], newm[1], newm[2]);
#endif
}

/* ---------------------------------------------------------------- */

void
CameraTrackball::set_center (Remesher::Vec3f const& center)
{
    this->move_vec = -center;
}
