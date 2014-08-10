#ifndef GTK_MESH_RENDERING_HEADER
#define GTK_MESH_RENDERING_HEADER

#include <gtkmm/box.h>
#include <gtkmm/label.h>
#include <gtkmm/combobox.h>
#include <gtkmm/comboboxtext.h>
#include <gtkmm/radiobutton.h>
#include <gtkmm/colorbutton.h>

#include "libglstuff/glfeatureedges.h"
#include "libglstuff/gltrianglemesh.h"
#include "libglstuff/gltriangledebug.h"

#include "gtkcomboboxcheck.h"

class GtkMeshRendering : public Gtk::VBox
{
  public:
    typedef sigc::signal<bool> SignalRedraw;

  protected:
    GLTriangleMeshPtr glmesh;
    GLTriangleDebugPtr gldebug;
    GLFeatureEdgesPtr glfeatures;
    SignalRedraw sig_redraw;

    Gtk::CheckButton draw_model_cb;
    Gtk::ComboBoxText shading_combo;
    GtkComboBoxCheck deco_combo;
    Gtk::ComboBoxText debug_combo;
    Gtk::ColorButton shade_color;
    Gtk::ColorButton deco_color;
    Gtk::ColorButton feature_color;

  protected:
    Gdk::Color get_color (float r, float g, float b);
    void on_draw_model_toggled (void);
    void on_shading_changed (void);
    void on_decorations_changed (void);
    void on_debug_changed (void);
    void on_shade_color_set (void);
    void on_deco_color_set (void);
    void on_feature_color_set (void);

  public:
    GtkMeshRendering (void);

    /* All drawables need to be set to operate. */
    void set_mesh_drawable (GLTriangleMeshPtr glmesh);
    void set_debug_drawable (GLTriangleDebugPtr gldebug);
    void set_features_drawable (GLFeatureEdgesPtr glfeatures);

    /* Set default options. */
    void set_shade_color (float r, float g, float b, float a);
    void set_deco_color (float r, float g, float b, float a);
    void set_feature_color (float r, float g, float b, float a);

    /* Control behaviour. */
    bool get_draw_mesh (void) const;
    void set_draw_mesh (bool draw);

    /* Initialize the GUI with default values. */
    void initialize (void);

    SignalRedraw& signal_redraw (void);
};

/* ---------------------------------------------------------------- */

inline void
GtkMeshRendering::set_mesh_drawable (GLTriangleMeshPtr glmesh)
{
  this->glmesh = glmesh;
}

inline void
GtkMeshRendering::set_debug_drawable (GLTriangleDebugPtr gldebug)
{
  this->gldebug = gldebug;
}

inline void
GtkMeshRendering::set_features_drawable (GLFeatureEdgesPtr glfeatures)
{
  this->glfeatures = glfeatures;
}

inline bool
GtkMeshRendering::get_draw_mesh (void) const
{
  return this->draw_model_cb.get_active();
}

inline void
GtkMeshRendering::set_draw_mesh (bool draw)
{
  this->draw_model_cb.set_active(draw);
  this->on_draw_model_toggled();
}

inline GtkMeshRendering::SignalRedraw&
GtkMeshRendering::signal_redraw (void)
{
  return this->sig_redraw;
}

#endif /* GTK_MESH_RENDERING_HEADER */
