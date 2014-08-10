#ifndef IMAGE_STORE_HEADER
#define IMAGE_STORE_HEADER

#include <gdkmm/pixbuf.h>

class ImageStore
{
  private:
    static Glib::RefPtr<Gdk::Pixbuf> create_from_inline (guint8 const* data);

  public:
    static Glib::RefPtr<Gdk::Pixbuf> cam_simple;
    static Glib::RefPtr<Gdk::Pixbuf> cam_trackball;
    static Glib::RefPtr<Gdk::Pixbuf> invert_faces;
    static Glib::RefPtr<Gdk::Pixbuf> scale_center;
    static Glib::RefPtr<Gdk::Pixbuf> lookat;
    static Glib::RefPtr<Gdk::Pixbuf> meshinfo;

    static void init (void);
    static void unload (void);
};

#endif /* IMAGE_STORE_HEADER */
