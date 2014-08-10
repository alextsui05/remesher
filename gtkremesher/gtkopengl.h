#ifndef GTK_OPENGL_HEADER
#define GTK_OPENGL_HEADER

#include <gtkglmm.h>

#include "libglstuff/cameracontrol.h"
#include "libglstuff/gldrawable.h"

class GtkOpenGLScene : public Gtk::GL::DrawingArea
{
  protected:
    std::vector<GLDrawablePtr> elements;
    CameraControlPtr camera_control;

    sigc::connection timeout_conn;
    unsigned int buttons_pressed;
    bool need_update;
    bool is_sync;

  protected:
    virtual void on_realize (void);
    virtual bool on_configure_event (GdkEventConfigure* event);
    virtual bool on_expose_event (GdkEventExpose* event);

    //virtual bool on_map_event (GdkEventAny* event);
    //virtual bool on_unmap_event (GdkEventAny* event);
    //virtual bool on_visibility_notify_event (GdkEventVisibility* event);

    virtual bool on_button_press_event (GdkEventButton* event);
    virtual bool on_button_release_event (GdkEventButton* event);
    virtual bool on_motion_notify_event (GdkEventMotion* event);

    virtual bool on_timeout (void);

    void set_lighting (void);
    void invalidate (void);
    void update (void);

  public:
    GtkOpenGLScene (void);
    virtual ~GtkOpenGLScene (void);

    /* Refreshes the widget, rendering a new frame. */
    virtual bool refresh (void);

    /* Resets the current control to the original view. */
    virtual void reset_view (void);

    /* Sets a new camera control. */
    virtual void set_camera_control (CameraControlPtr control);

    /* Returns the camera control. */
    virtual CameraControlPtr get_camera_control (void) const;

    /* Appends a drawable to the widget.
     * Drawables are rendered in the order they are added. */
    void add_drawable (GLDrawablePtr drawable);
};

#endif /* GTK_OPENGL_HEADER */
