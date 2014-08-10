#include <iostream>
#include <GL/gl.h>

#include "libglstuff/camerasimple.h"
#include "libglstuff/cameratrackball.h"
#include "libglstuff/camerarotate.h"
#include "gtkopengl.h"

#if 0 // Off-screen renderer hack
#   include <fstream>
#   include "libremesh/helpers.h"
#endif

GtkOpenGLScene::GtkOpenGLScene (void)
  : Gtk::GL::DrawingArea(Gdk::GL::Config::create
        (Gdk::GL::MODE_RGB | Gdk::GL::MODE_DEPTH | Gdk::GL::MODE_DOUBLE))
{
  this->buttons_pressed = 0;
  this->need_update = false;
  this->is_sync = false;

  //this->add_events(Gdk::VISIBILITY_NOTIFY_MASK);
  this->add_events(Gdk::POINTER_MOTION_MASK);
  this->add_events(Gdk::BUTTON_PRESS_MASK);
  this->add_events(Gdk::BUTTON_RELEASE_MASK);

  this->signal_button_press_event().connect
      (sigc::mem_fun(*this, &GtkOpenGLScene::on_button_press_event));
  this->signal_button_release_event().connect
      (sigc::mem_fun(*this, &GtkOpenGLScene::on_button_release_event));
  this->signal_motion_notify_event().connect
      (sigc::mem_fun(*this, &GtkOpenGLScene::on_motion_notify_event));

  this->camera_control = CameraTrackball::create();
  //this->camera_control = CameraRotate::create();
}

/* ---------------------------------------------------------------- */

GtkOpenGLScene::~GtkOpenGLScene (void)
{
}

/* ---------------------------------------------------------------- */

void
GtkOpenGLScene::on_realize (void)
{
  Gtk::GL::DrawingArea::on_realize();

  Glib::RefPtr<Gdk::GL::Window> glwindow = get_gl_window();
  if (!glwindow->gl_begin(get_gl_context()))
    return;

  this->set_lighting();

  /* Needed since zooming works with scaling. */
  glEnable(GL_RESCALE_NORMAL);

  glEnable(GL_DEPTH_TEST);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClearDepth(1.0f);

  glwindow->gl_end();
}

/* ---------------------------------------------------------------- */

bool
GtkOpenGLScene::on_configure_event (GdkEventConfigure* /* event */)
{
  Glib::RefPtr<Gdk::GL::Window> glwindow = get_gl_window();
  if (!glwindow->gl_begin(get_gl_context()))
    return false;

  CameraInfo ci;
  ci.width = this->get_width();
  ci.height = this->get_height();
  this->camera_control->set_camera_info(ci);
  this->camera_control->setup_projection();

  //std::cout << "GL area dimensions: " << ci.width
  //    << "x" << ci.height << std::endl;

  glwindow->gl_end();

  return true;
}

/* ---------------------------------------------------------------- */

bool
GtkOpenGLScene::on_expose_event (GdkEventExpose* /* event */)
{
  Glib::RefPtr<Gdk::GL::Window> glwindow = get_gl_window();
  if (!glwindow->gl_begin(get_gl_context()))
    return false;

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  for (unsigned int i = 0; i < this->elements.size(); ++i)
  {
    this->camera_control->setup_projection(this->elements[i]->get_projection());
    this->camera_control->apply_projection();
    this->camera_control->apply_modelview(this->elements[i]->get_modelview());
    this->elements[i]->draw();
  }

  /* Swap buffers. */
  if (glwindow->is_double_buffered())
    glwindow->swap_buffers();
  else
    glFlush();

  glwindow->gl_end();

  return true;
}

/* ---------------------------------------------------------------- */

//#include "../goesele-proj/mve/libs/util/string.h"

bool
GtkOpenGLScene::on_button_press_event (GdkEventButton* event)
{
  Glib::RefPtr<Gdk::GL::Window> glwindow = get_gl_window();
  if (!glwindow->gl_begin(get_gl_context()))
    return false;

  /* Forward event to the camera control. */
  this->camera_control->set_mouse_down(event->button,
      (int)event->x, (int)event->y);


#if 0
  /* Offscreen rendering hacks. */
  /* Current frame not rendered yet, outputting previous frame! */

  static int frame_cnt = 0;
  static int last_time = 0;

  if ((int)event->time != last_time)
  {
    last_time = event->time;
    frame_cnt += 1;

    std::cout << "Event: " << (int)event->send_event
        << ", State: " << (int)event->state
        << ", time: " << (int)event->time << std::endl;


    ((CameraRotate*)this->camera_control.get())->next_frame();

    int width = this->get_width();
    int height = this->get_height();
    std::cout << "Writing frame with " << width << "x" << height << std::endl;

    std::size_t buf_size(width * height * 4);
    char* buf = new char[buf_size];
    glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, buf);
    //std::cout << glGetError() << std::endl;

    //std::ofstream out(("/tmp/frame_" + Remesher::Helpers::get_string(frame_cnt) + ".ppm").c_str(), std::ios::binary);

    std::ofstream out(("/tmp/frame_" + util::string::get_filled(frame_cnt, 3) + ".ppm").c_str(), std::ios::binary);
    out << "P6" << std::endl;
    out << width << " " << height << std::endl << "255" << std::endl;
    for (std::size_t i = 0; i < buf_size; ++i)
    {
        if (i % 4 == 3)
            continue;
        out.write(buf + i, 1);
    }
    //out.write(buf, buf_size);
    out.close();

    delete [] buf;
  }
#endif


  glwindow->gl_end();

  /* Assume we need a refresh. */
  this->need_update = true;

  /* Set periodically refresh during button press. */
  this->buttons_pressed += 1;
  this->timeout_conn = Glib::signal_timeout().connect
      (sigc::mem_fun(*this, &GtkOpenGLScene::on_timeout), 24);

  return true;
}

/* ---------------------------------------------------------------- */

bool
GtkOpenGLScene::on_button_release_event (GdkEventButton* event)
{
  Glib::RefPtr<Gdk::GL::Window> glwindow = get_gl_window();
  if (!glwindow->gl_begin(get_gl_context()))
    return false;

  /* Forward event to the camera control. */
  this->camera_control->set_mouse_up(event->button,
      (int)event->x, (int)event->y);

  glwindow->gl_end();

  /* Assume we need a refresh. */
  this->need_update = true;

  /* Stop periodic update if no buttons are pressed. */
  this->buttons_pressed -= 1;
  if (this->buttons_pressed <= 0 && this->timeout_conn.connected())
  {
    this->buttons_pressed = 0;
    this->timeout_conn.disconnect();
    this->refresh();
    this->need_update = false;
  }

  return true;
}

/* ---------------------------------------------------------------- */

bool
GtkOpenGLScene::on_motion_notify_event (GdkEventMotion* event)
{
  Glib::RefPtr<Gdk::GL::Window> glwindow = get_gl_window();
  if (!glwindow->gl_begin(get_gl_context()))
    return false;

  /* Forward event to the camera control. */
  this->camera_control->set_mouse_move((MouseButtonMask)event->state,
      (int)event->x, (int)event->y);

  glwindow->gl_end();

  /* Assume we're needing a refresh. */
  this->need_update = true;

  return false;
}

/* ---------------------------------------------------------------- */

void
GtkOpenGLScene::set_lighting (void)
{
  {
    /* Light 0. White light from the left. */
    GLfloat ambient[]  = { 0.5f, 0.5f, 0.5f, 1.0f };
    GLfloat diffuse[]  = { 0.6f, 0.6f, 0.6f, 1.0f };
    GLfloat position[] = { -2.0f, 1.0f, 1.0f, 0.0f };

    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_POSITION, position);
    glEnable(GL_LIGHT0);
  }

  {
    /* Light 1. Bluish light from the right.*/
    //GLfloat ambient[]  = { 0.1f, 0.1f, 0.2f, 1.0f };
    //GLfloat diffuse[]  = { 0.3f, 0.3f, 1.0f, 1.0f };

    /* Light 1. Greyish light from the right.*/
    GLfloat ambient[]  = { 0.2f, 0.2f, 0.2f, 1.0f };
    GLfloat diffuse[]  = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat position[] = { 2.0f, 1.0f, 1.0f, 0.0f };

    glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT1, GL_POSITION, position);
    glEnable(GL_LIGHT1);
  }

  //GLfloat mat_specular[] = { 0.5f, 0.5f, 0.5f, 1.0f };
  GLfloat mat_specular[] = { 0.2f, 0.2f, 0.2f, 1.0f };
  GLfloat mat_shininess[] = { 50.0f };

  glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
  glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

  /*
  static GLfloat lmodel_ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
  static GLfloat local_view[] = { 1.0f };
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
  glLightModelfv(GL_LIGHT_MODEL_LOCAL_VIEWER, local_view);
  */

  glEnable(GL_LIGHTING);
}

/* ---------------------------------------------------------------- */

bool
GtkOpenGLScene::on_timeout (void)
{
  if (this->need_update)
  {
    this->refresh();
    this->need_update = false;
  }
  return true;
}

/* ---------------------------------------------------------------- */

bool
GtkOpenGLScene::refresh (void)
{
  this->invalidate();

  /* Only update if sync mode is enabled. */
  if (this->is_sync)
    this->update();

  return true;
}

/* ---------------------------------------------------------------- */

void
GtkOpenGLScene::invalidate (void)
{
  if (this->get_window())
    this->get_window()->invalidate_rect(get_allocation(), false);
}

/* ---------------------------------------------------------------- */

void
GtkOpenGLScene::update (void)
{
  if (this->get_window())
    this->get_window()->process_updates(false);
}

/* ---------------------------------------------------------------- */

void
GtkOpenGLScene::reset_view (void)
{
  this->camera_control->reset_view();
  this->refresh();
}

/* ---------------------------------------------------------------- */

void
GtkOpenGLScene::set_camera_control (CameraControlPtr control)
{
  this->camera_control = control;
  this->on_configure_event(0);
}

/* ---------------------------------------------------------------- */

CameraControlPtr
GtkOpenGLScene::get_camera_control (void) const
{
    return this->camera_control;
}

/* ---------------------------------------------------------------- */

void
GtkOpenGLScene::add_drawable (GLDrawablePtr drawable)
{
  this->elements.push_back(drawable);
}
