#include <iostream>
#include <GL/gl.h>

#include "camerasimple.h"

CameraSimple::CameraSimple (void)
{
  this->reset_view();
}

/* ---------------------------------------------------------------- */

void
CameraSimple::set_mouse_move (MouseButtonMask mask, int x, int y)
{
  if (mask & MOUSE_BUTTON_1)
  {
    this->motion_dist_x = x - this->motion_begin_x;
    this->motion_dist_y = y - this->motion_begin_y;
  }

  if (mask & MOUSE_BUTTON_2)
  {
    this->scale = this->click_scale
        * (float)((y - this->click_scale_y + 100)) / 100.0f;
    this->scale = MY_MAX(0.1f, this->scale);
    this->scale = MY_MIN(50.0f, this->scale);
  }

  while (this->motion_dist_x < 0)
    this->motion_dist_x += 360;
  while (this->motion_dist_x > 360)
    this->motion_dist_x -= 360;

  if (this->motion_dist_y > 90)
    this->motion_dist_y = 90;
  else if (this->motion_dist_y < -90)
    this->motion_dist_y = -90;
}

/* ---------------------------------------------------------------- */

void
CameraSimple::set_mouse_down (int button, int x, int y)
{
  if (button == 1)
  {
    this->motion_begin_x = x - this->motion_dist_x;
    this->motion_begin_y = y - this->motion_dist_y;
  }
  else if (button == 2)
  {
    this->click_scale_y = y;
    this->click_scale = this->scale;
  }
}

/* ---------------------------------------------------------------- */

void
CameraSimple::set_mouse_up (int button, int x, int y)
{
  button = x = y = 0;
}

/* ---------------------------------------------------------------- */

void
CameraSimple::reset_view (void)
{
  this->motion_dist_x = 0;
  this->motion_dist_y = 0;
  this->scale = 1.0f;
}

/* ---------------------------------------------------------------- */

void
CameraSimple::apply_modelview (ModelviewMask mmask)
{
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  if (mmask & MODELVIEW_TRANSLATE)
    glTranslatef(0.0f, 0.0f, -CAM_SIMPLE_Z_DIST);

  if (mmask & MODELVIEW_ROTATE)
  {
    glRotatef((float)this->motion_dist_y, 1.0f, 0.0f, 0.0f);
    glRotatef((float)this->motion_dist_x, 0.0f, 1.0f, 0.0f);
  }

  if (mmask & MODELVIEW_SCALE)
    glScalef(this->scale, this->scale, this->scale);
}
