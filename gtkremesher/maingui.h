#ifndef MAIN_GUI_HEADER
#define MAIN_GUI_HEADER

#include <gtkmm/window.h>
#include <gtkmm/filechooserbutton.h>
#include <gtkmm/button.h>
#include <gtkmm/label.h>
#include <gtkmm/box.h>
#include <gtkmm/colorbutton.h>
#include <gtkmm/checkbutton.h>
#include <gtkmm/togglebutton.h>
#include <gtkmm/progressbar.h>
#include <gtkmm/comboboxtext.h>
#include <gtkmm/scale.h>

#include "libremesh/interface.h"
#include "libglstuff/gltrianglemesh.h"
#include "libglstuff/gltriangledebug.h"
#include "libglstuff/glfeatureedges.h"
#include "libglstuff/glskybox.h"
#include "gtkmeshrendering.h"
#include "gtkconfig.h"
#include "gtkopengl.h"

class MainGui : public Gtk::Window
{
  private:
    Remesher::Interface iface;

    /* Mesh info/action elements. */
    Gtk::FileChooserButton fcb;

    Gtk::Button reference_save;
    Gtk::Button reference_refresh;
    Gtk::Button reference_clean;
    Gtk::Button reference_copy;
    Gtk::Label reference_vertices;
    Gtk::Label reference_triangles;

    Gtk::Button evolving_save;
    Gtk::Button evolving_copy;
    Gtk::Label evolving_vertices;
    Gtk::Label evolving_triangles;

    /* Test buttons. */
    Gtk::Button test1_but;
    Gtk::Button test2_but;
    Gtk::Button test3_but;

    Gtk::Button stats_refmesh_but;
    Gtk::Button stats_evomesh_but;
    Gtk::Button lookat_refmesh_but;
    Gtk::Button lookat_evomesh_but;

    Gtk::CheckButton auto_preprocess_cb;
    Gtk::CheckButton mesh_visibility_cb;

    /* Algorithm configuration. */
    GtkSimplificationConfig simpl_config;
    GtkOversamplingConfig oversampling_config;
    GtkDensityFieldConfig density_config;
    GtkFeatureEdgesConfig feature_config;
    GtkResamplingConfig resampling_config;
    GtkRelaxationConfig lloyd_config;
    GtkRelaxationConfig area_equal_config;

    /* Rendering settings. */
    Gtk::ColorButton bgcolor;
    Gtk::CheckButton render_skybox;

    GtkMeshRendering refmesh_opts;
    GtkMeshRendering evomesh_opts;

    /* OpenGL members. */
    GLSkyBoxPtr skybox;
    GLTriangleMeshPtr reference_mesh;
    GLTriangleDebugPtr reference_debug;
    GLFeatureEdgesPtr reference_features;
    GLTriangleMeshPtr evolving_mesh;
    GLTriangleDebugPtr evolving_debug;
    GLFeatureEdgesPtr evolving_features;
    GtkOpenGLScene glarea;

  protected:
    Gtk::Widget* create_header (Glib::ustring const& label);
    void on_model_selected (void);
    void on_reference_mesh_changed (void);
    void on_reference_clean (void);
    void on_reference_copy (void);
    void on_evolving_mesh_changed (void);
    void on_evolving_mesh_copy (void);
    void on_mesh_save_request (bool reference);
    void on_scale_and_center_clicked (void);
    void on_invert_faces_clicked (void);
    void on_gl_info_clicked (void);
    void on_camera_changed (int type);
    void on_mesh_visibility_changed (void);
    void on_error (std::string const& message);
    void on_lookat_clicked (bool refmesh); // ref or evo mesh
    void on_meshstats_clicked (bool refmesh); // ref or evo mesh

    void on_density_field_execute (void);
    void on_density_field_settings (void);
    void on_density_field_clear (void);
    void on_feature_extract_execute (void);
    void on_feature_extract_clear (void);
    void on_feature_extract_settings (void);
    void on_oversampling_execute (void);
    void on_oversampling_settings (void);
    void on_simplification_execute (void);
    void on_simplification_settings (void);
    void on_resampling_execute (void);
    void on_resampling_settings (void);
    void on_lloyd_execute (void);
    void on_lloyd_settings (void);
    void on_area_equal_execute (void);
    void on_area_equal_settings (void);

    void on_config_changed (void);
    void on_render_type_changed (void);
    void on_visibility_changed (void);
    void on_test_1_clicked (void);
    void on_test_2_clicked (void);
    void on_test_3_clicked (void);

    void on_background_color_set (void);
    void on_skybox_toggled (void);

    void update_glarea (void);

  public:
    MainGui (void);
};

#endif /* MAIN_GUI_HEADER */
