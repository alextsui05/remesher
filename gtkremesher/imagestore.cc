#include <glib.h>

#include "images/cam_simple.h"
#include "images/cam_trackball.h"
#include "images/invert_faces.h"
#include "images/scale_center.h"
#include "images/lookat.h"
#include "images/meshinfo.h"

#include "imagestore.h"

Glib::RefPtr<Gdk::Pixbuf> ImageStore::cam_simple;
Glib::RefPtr<Gdk::Pixbuf> ImageStore::cam_trackball;
Glib::RefPtr<Gdk::Pixbuf> ImageStore::invert_faces;
Glib::RefPtr<Gdk::Pixbuf> ImageStore::scale_center;
Glib::RefPtr<Gdk::Pixbuf> ImageStore::lookat;
Glib::RefPtr<Gdk::Pixbuf> ImageStore::meshinfo;

/* ---------------------------------------------------------------- */

void
ImageStore::init (void)
{
  ImageStore::cam_simple = ImageStore::create_from_inline(img_cam_simple);
  ImageStore::cam_trackball = ImageStore::create_from_inline(img_cam_trackball);
  ImageStore::invert_faces = ImageStore::create_from_inline(img_invert_faces);
  ImageStore::scale_center = ImageStore::create_from_inline(img_scale_center);
  ImageStore::lookat = ImageStore::create_from_inline(img_lookat);
  ImageStore::meshinfo = ImageStore::create_from_inline(img_meshinfo);
}

/* ---------------------------------------------------------------- */

void
ImageStore::unload (void)
{
}

/* ---------------------------------------------------------------- */

Glib::RefPtr<Gdk::Pixbuf>
ImageStore::create_from_inline (guint8 const* data)
{
  #ifdef GLIBMM_EXCEPTIONS_ENABLED
  return Gdk::Pixbuf::create_from_inline(-1, data, false);
  #else
  std::auto_ptr<Glib::Error> error;
  Glib::RefPtr<Gdk::Pixbuf> ret = Gdk::Pixbuf::create_from_inline
      (-1, data, false, error);
  if (error.get())
    throw error;
  return ret;
  #endif
}
