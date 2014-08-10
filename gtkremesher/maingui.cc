#include <iostream>

#include <gtkmm/main.h>
#include <gtkmm/notebook.h>
#include <gtkmm/frame.h>
#include <gtkmm/stock.h>
#include <gtkmm/messagedialog.h>
#include <gtkmm/separator.h>
#include <gtkmm/box.h>
#include <gtkmm/image.h>
#include <gtkmm/table.h>
#include <gtkmm/label.h>

#include "libremesh/modelwriter.h"
#include "libremesh/exception.h"
#include "libremesh/meshstats.h"
#include "libremesh/helpers.h"
#include "libremesh/cvtstats.h"

#include "libremesh/delaunayflips.h"
#include "libremesh/densityfield.h"

#include "libglstuff/camerasimple.h"
#include "libglstuff/cameratrackball.h"
#include "libglstuff/glfrustarenderer.h"

#include "imagestore.h"
#include "guiglinfo.h"
#include "arguments.h"
#include "gtkdefines.h"
#include "maingui.h"

MainGui::MainGui (void)
{
  /* Init class members. */
  this->reference_save.set_image(*MK_IMG
      (Gtk::Stock::SAVE_AS, Gtk::ICON_SIZE_MENU));
  this->reference_refresh.set_image(*MK_IMG
      (Gtk::Stock::REFRESH, Gtk::ICON_SIZE_MENU));
  this->reference_clean.set_image(*MK_IMG
      (Gtk::Stock::CLEAR, Gtk::ICON_SIZE_MENU));
  this->reference_copy.set_image(*MK_IMG
      (Gtk::Stock::COPY, Gtk::ICON_SIZE_MENU));
  this->evolving_save.set_image(*MK_IMG
      (Gtk::Stock::SAVE_AS, Gtk::ICON_SIZE_MENU));
  this->evolving_copy.set_image(*MK_IMG
      (Gtk::Stock::COPY, Gtk::ICON_SIZE_MENU));

  this->reference_save.set_tooltip_text("Save the reference mesh to file");
  this->reference_refresh.set_tooltip_text("Reload the reference mesh from file");
  this->reference_clean.set_tooltip_text("Clean invalid faces from the reference mesh");
  this->reference_copy.set_tooltip_text("Copy reference mesh as evolving mesh");
  this->evolving_save.set_tooltip_text("Save the evolving mesh to file");
  this->evolving_copy.set_tooltip_text("Copy the evolving mesh as reference mesh");

  this->test1_but.set_label("Test1");
  this->test2_but.set_label("Cell Stats");
  this->test3_but.set_label("Rm dups");

  //this->stats_refmesh_but.set_label("Mesh stats");
  //this->stats_evomesh_but.set_label("Mesh stats");
  //this->lookat_refmesh_but.set_label("Look at...");
  //this->lookat_evomesh_but.set_label("Look at...");
  this->stats_refmesh_but.set_image(*MK_IMG_PB(ImageStore::meshinfo));
  this->stats_evomesh_but.set_image(*MK_IMG_PB(ImageStore::meshinfo));
  this->lookat_refmesh_but.set_image(*MK_IMG_PB(ImageStore::lookat));
  this->lookat_evomesh_but.set_image(*MK_IMG_PB(ImageStore::lookat));
  this->stats_refmesh_but.set_tooltip_text("Mesh statistics");
  this->stats_evomesh_but.set_tooltip_text("Mesh statistics");
  this->lookat_refmesh_but.set_tooltip_text("Lookat vertex / face");
  this->lookat_evomesh_but.set_tooltip_text("Lookat vertex / face");

  this->auto_preprocess_cb.set_label("Auto-preprocess on load");
  this->mesh_visibility_cb.set_label("Toggle mesh visibility");
  this->auto_preprocess_cb.set_active(false);

  this->reference_vertices.set_text("n/a");
  this->reference_triangles.set_text("n/a");
  this->reference_vertices.set_alignment(Gtk::ALIGN_LEFT);
  this->reference_triangles.set_alignment(Gtk::ALIGN_LEFT);

  this->evolving_vertices.set_text("n/a");
  this->evolving_triangles.set_text("n/a");
  this->evolving_vertices.set_alignment(Gtk::ALIGN_LEFT);
  this->evolving_triangles.set_alignment(Gtk::ALIGN_LEFT);

  this->render_skybox.set_label("Render Skybox");

  /* Reference model info. */
  Gtk::Label* label_vertices = MK_LABEL("V:");
  Gtk::Label* label_triangles = MK_LABEL("F:");
  Gtk::HBox* refmesh_info_box = MK_HBOX;
  refmesh_info_box->pack_start(*label_vertices, false, false, 5);
  refmesh_info_box->pack_start(this->reference_vertices, false, false, 0);
  refmesh_info_box->pack_start(*label_triangles, false, false, 5);
  refmesh_info_box->pack_start(this->reference_triangles, false, false, 0);

  Gtk::Button* inv_faces_but = MK_BUT0;
  inv_faces_but->set_image(*MK_IMG_PB(ImageStore::invert_faces));
  inv_faces_but->property_xalign() = 0.0f;
  inv_faces_but->set_tooltip_text("Invert face orientation");

  Gtk::Button* scale_and_center_but = MK_BUT0;
  scale_and_center_but->set_image(*MK_IMG_PB(ImageStore::scale_center));
  scale_and_center_but->property_xalign() = 0.0f;
  scale_and_center_but->set_tooltip_text("Scale and center model");

  Gtk::HBox* reference_opts_box = MK_HBOX0;
  reference_opts_box->set_spacing(1);
  reference_opts_box->pack_start(*scale_and_center_but, false, false, 0);
  reference_opts_box->pack_start(*inv_faces_but, false, false, 0);
  reference_opts_box->pack_start(this->reference_clean, false, false, 0);
  reference_opts_box->pack_end(this->reference_copy, false, false, 0);
  reference_opts_box->pack_end(this->lookat_refmesh_but, false, false, 0);
  reference_opts_box->pack_end(this->stats_refmesh_but, false, false, 0);

  Gtk::HBox* filechooser_box = MK_HBOX;
  filechooser_box->set_spacing(1);
  filechooser_box->pack_start(this->fcb, true, true, 0);
  filechooser_box->pack_start(this->reference_refresh, false, false, 0);
  filechooser_box->pack_start(this->reference_save, false, false, 0);

  Gtk::VBox* reference_fcb_box = MK_VBOX;
  reference_fcb_box->set_spacing(1);
  reference_fcb_box->pack_start(*refmesh_info_box, false, false, 0);
  reference_fcb_box->pack_start(*filechooser_box, false, false, 0);
  reference_fcb_box->pack_start(*reference_opts_box, false, false, 0);
  reference_fcb_box->pack_start(this->auto_preprocess_cb, false, false, 0);

  /* Evolving mesh. */
  Gtk::Label* ev_label_vertices = MK_LABEL("V:");
  Gtk::Label* ev_label_triangles = MK_LABEL("T:");
  Gtk::HBox* evolving_info_box = MK_HBOX;
  evolving_info_box->pack_start(*ev_label_vertices, false, false, 5);
  evolving_info_box->pack_start(this->evolving_vertices, false, false, 0);
  evolving_info_box->pack_start(*ev_label_triangles, false, false, 5);
  evolving_info_box->pack_start(this->evolving_triangles, false, false, 0);

  Gtk::HBox* evolving_opts_box = MK_HBOX;
  evolving_opts_box->set_spacing(1);
  evolving_opts_box->pack_end(this->evolving_save, false, false, 0);
  evolving_opts_box->pack_end(this->evolving_copy, false, false, 0);
  evolving_opts_box->pack_end(this->lookat_evomesh_but, false, false, 0);
  evolving_opts_box->pack_end(this->stats_evomesh_but, false, false, 0);

  Gtk::VBox* evo_model_vbox = MK_VBOX;
  evo_model_vbox->pack_start(*evolving_info_box, false, false, 0);
  evo_model_vbox->pack_start(*evolving_opts_box, false, false, 0);

  /* Algorithm steps. */
  Gtk::Button* step_density_exec_but = MK_BUT0;
  step_density_exec_but->set_label("Density field");
  step_density_exec_but->set_alignment(0.0f, 0.5f);
  Gtk::Button* step_density_clear_but = MK_BUT0;
  step_density_clear_but->set_image(*MK_IMG(Gtk::Stock::CLEAR,
      Gtk::ICON_SIZE_MENU));
  Gtk::Button* step_density_conf_but = MK_BUT0;
  step_density_conf_but->set_image(*MK_IMG(Gtk::Stock::PREFERENCES,
      Gtk::ICON_SIZE_MENU));

  Gtk::Button* step_feature_extract_but = MK_BUT0;
  step_feature_extract_but->set_label("Feature edges");
  step_feature_extract_but->set_alignment(0.0f, 0.5f);
  Gtk::Button* step_feature_clear_but = MK_BUT0;
  step_feature_clear_but->set_image(*MK_IMG(Gtk::Stock::CLEAR,
      Gtk::ICON_SIZE_MENU));
  Gtk::Button* step_feature_conf_but = MK_BUT0;
  step_feature_conf_but->set_image(*MK_IMG(Gtk::Stock::PREFERENCES,
      Gtk::ICON_SIZE_MENU));

  Gtk::Button* step_oversampling_but = MK_BUT0;
  step_oversampling_but->set_label("Oversampling");
  step_oversampling_but->set_alignment(0.0f, 0.5f);
  Gtk::Button* step_oversampling_conf_but = MK_BUT0;
  step_oversampling_conf_but->set_image(*MK_IMG(Gtk::Stock::PREFERENCES,
      Gtk::ICON_SIZE_MENU));

  Gtk::Button* step_simpl_exec_but = MK_BUT0;
  step_simpl_exec_but->set_label("Simplification");
  step_simpl_exec_but->set_alignment(0.0f, 0.5f);
  Gtk::Button* step_simpl_conf_but = MK_BUT0;
  step_simpl_conf_but->set_image(*MK_IMG(Gtk::Stock::PREFERENCES,
      Gtk::ICON_SIZE_MENU));

  Gtk::Button* step_resampling_exec_but = MK_BUT0;
  step_resampling_exec_but->set_label("Resampling");
  step_resampling_exec_but->set_alignment(0.0f, 0.5f);
  Gtk::Button* step_resampling_conf_but = MK_BUT0;
  step_resampling_conf_but->set_image(*MK_IMG(Gtk::Stock::PREFERENCES,
      Gtk::ICON_SIZE_MENU));

  Gtk::Button* step_area_equal_exec_but = MK_BUT0;
  step_area_equal_exec_but->set_label("Area equalization");
  step_area_equal_exec_but->set_alignment(0.0f, 0.5f);
  Gtk::Button* step_area_equal_conf_but = MK_BUT0;
  step_area_equal_conf_but->set_image(*MK_IMG(Gtk::Stock::PREFERENCES,
      Gtk::ICON_SIZE_MENU));

  Gtk::Button* step_lloyd_exec_but = MK_BUT0;
  step_lloyd_exec_but->set_label("Lloyd relaxation");
  step_lloyd_exec_but->set_alignment(0.0f, 0.5f);
  Gtk::Button* step_lloyd_conf_but = MK_BUT0;
  step_lloyd_conf_but->set_image(*MK_IMG(Gtk::Stock::PREFERENCES,
      Gtk::ICON_SIZE_MENU));

  /* Reference mesh table. */
  Gtk::Table* algo_refmesh_table = MK_TABLE(4, 3);
  algo_refmesh_table->set_col_spacings(1);
  algo_refmesh_table->set_row_spacings(1);

  algo_refmesh_table->attach(*step_density_exec_but, 0, 1, 0, 1,
      Gtk::EXPAND | Gtk::FILL);
  algo_refmesh_table->attach(*step_density_clear_but, 1, 2, 0, 1, Gtk::SHRINK);
  algo_refmesh_table->attach(*step_density_conf_but, 2, 3, 0, 1, Gtk::SHRINK);

  algo_refmesh_table->attach(*step_feature_extract_but, 0, 1, 1, 2,
      Gtk::EXPAND | Gtk::FILL);
  algo_refmesh_table->attach(*step_feature_clear_but, 1, 2, 1, 2, Gtk::SHRINK);
  algo_refmesh_table->attach(*step_feature_conf_but, 2, 3, 1, 2, Gtk::SHRINK);

  algo_refmesh_table->attach(*step_oversampling_but, 0, 2, 2, 3,
      Gtk::EXPAND | Gtk::FILL);
  algo_refmesh_table->attach(*step_oversampling_conf_but, 2, 3, 2, 3,
      Gtk::SHRINK);

  /* Algorithm table. */
  Gtk::Table* algo_conv_table = MK_TABLE(2, 2);
  algo_conv_table->set_col_spacings(1);
  algo_conv_table->set_row_spacings(1);
  algo_conv_table->attach(*step_simpl_exec_but, 0, 1, 0, 1,
      Gtk::EXPAND | Gtk::FILL);
  algo_conv_table->attach(*step_simpl_conf_but, 1, 2, 0, 1, Gtk::SHRINK);
  algo_conv_table->attach(*step_resampling_exec_but, 0, 1, 1, 2,
      Gtk::EXPAND | Gtk::FILL);
  algo_conv_table->attach(*step_resampling_conf_but, 1, 2, 1, 2, Gtk::SHRINK);

  /* Evolving mesh table. */
  Gtk::Table* algo_evomesh_table = MK_TABLE(2, 1);
  algo_evomesh_table->set_col_spacings(1);
  algo_evomesh_table->set_row_spacings(1);
  algo_evomesh_table->attach(*step_lloyd_exec_but, 0, 1, 1, 2,
      Gtk::EXPAND | Gtk::FILL);
  algo_evomesh_table->attach(*step_lloyd_conf_but, 1, 2, 1, 2,
      Gtk::SHRINK);
  algo_evomesh_table->attach(*step_area_equal_exec_but, 0, 1, 0, 1,
      Gtk::EXPAND | Gtk::FILL);
  algo_evomesh_table->attach(*step_area_equal_conf_but, 1, 2, 0, 1,
      Gtk::SHRINK);

  /* Algorithm. */
  Gtk::HBox* algorithm_box = MK_HBOX;
  algorithm_box->pack_start(this->test1_but, true, true, 0);
  algorithm_box->pack_start(this->test2_but, true, true, 0);
  algorithm_box->pack_start(this->test3_but, true, true, 0);

  Gtk::VBox* algo_control_vbox = MK_VBOX;
  algo_control_vbox->pack_start(*algorithm_box, false, false, 0);

  /* Misc rendering configuration. */
  Gtk::HBox* glconfig_hbox = MK_HBOX;
  glconfig_hbox->pack_start(*MK_LABEL("Background color:"), false, false, 0);
  glconfig_hbox->pack_start(this->bgcolor, false, false, 0);

  /* Main GUI setup. */
  Gtk::Widget* header_reference_model = this->create_header("Original mesh");
  Gtk::Widget* header_evolving_model = this->create_header("Evolving mesh");
  Gtk::Widget* header_conversion = this->create_header("Conversion");
  Gtk::Widget* header_source_model = this->create_header("Original mesh");
  Gtk::Widget* header_triangulation = this->create_header("Evolving mesh");
  Gtk::Widget* header_misc_rendering = this->create_header("Misc rendering");

  /* Algorithm settings. */
  Gtk::VBox* algorithm_vbox = MK_VBOX;
  algorithm_vbox->set_border_width(5);
  algorithm_vbox->pack_start(*header_reference_model, false, false, 0);
  algorithm_vbox->pack_start(*reference_fcb_box, false, false, 0);
  algorithm_vbox->pack_start(*algo_refmesh_table, false, false, 0);
  algorithm_vbox->pack_start(*header_conversion, false, false, 0);
  algorithm_vbox->pack_start(*algo_conv_table, false, false, 0);
  algorithm_vbox->pack_start(*header_evolving_model, false, false, 0);
  algorithm_vbox->pack_start(*evo_model_vbox, false, false, 0);
  algorithm_vbox->pack_start(*algo_evomesh_table, false, false, 0);

  // Misc processing
  //Gtk::Widget* header_misc_model = this->create_header("Misc processing");
  //algorithm_vbox->pack_start(*header_misc_model, false, false, 0);
  //algorithm_vbox->pack_start(*algo_control_vbox, true, true, 0);

  algorithm_vbox->pack_end(this->mesh_visibility_cb, false, false, 0);

  /* Rendering settings. */
  Gtk::VBox* rendering_vbox = MK_VBOX;
  rendering_vbox->set_border_width(5);
  rendering_vbox->pack_start(*header_source_model, false, false, 0);
  rendering_vbox->pack_start(this->refmesh_opts, false, false, 0);
  rendering_vbox->pack_start(*header_triangulation, false, false, 0);
  rendering_vbox->pack_start(this->evomesh_opts, false, false, 0);
  rendering_vbox->pack_start(*header_misc_rendering, false, false, 0);
  rendering_vbox->pack_start(*glconfig_hbox, false, false, 0);
  rendering_vbox->pack_start(this->render_skybox, false, false, 0);

  Gtk::Notebook* settings_nb = MK_NOTEBOOK;
  settings_nb->append_page(*algorithm_vbox, "Algorithm");
  settings_nb->append_page(*rendering_vbox, "Rendering");

  /* Controls box. */
  Gtk::Button* cam_simple_but = MK_BUT0;
  cam_simple_but->set_image(*MK_IMG_PB(ImageStore::cam_simple));
  Gtk::Button* cam_trackball_but = MK_BUT0;
  cam_trackball_but->set_image(*MK_IMG_PB(ImageStore::cam_trackball));
  Gtk::Button* app_exit_but = MK_BUT0;
  app_exit_but->set_image(*MK_IMG(Gtk::Stock::QUIT, Gtk::ICON_SIZE_MENU));
  Gtk::Button* gl_info_but = MK_BUT0;
  gl_info_but->set_image(*MK_IMG(Gtk::Stock::INFO, Gtk::ICON_SIZE_MENU));

  Gtk::HBox* controls_box = MK_HBOX0;
  controls_box->set_spacing(1);
  controls_box->pack_start(*cam_simple_but, false, false, 0);
  controls_box->pack_start(*cam_trackball_but, false, false, 0);
  controls_box->pack_end(*app_exit_but, false, false, 0);
  controls_box->pack_end(*gl_info_but, false, false, 0);

  Gtk::VBox* notebook_box = MK_VBOX;
  notebook_box->pack_start(*settings_nb, true, true, 0);
  notebook_box->pack_start(*controls_box, false, false, 0);

  /* GL Area. */
  Gtk::Frame* glframe = MK_FRAME0;
  glframe->set_shadow_type(Gtk::SHADOW_IN);
  glframe->add(this->glarea);

  Gtk::VBox* glarea_vbox = MK_VBOX;
  glarea_vbox->pack_start(*glframe, true, true, 0);

  Gtk::HBox* main_hbox = MK_HBOX;
  main_hbox->set_border_width(5);
  main_hbox->pack_start(*glarea_vbox, true, true, 0);
  main_hbox->pack_start(*notebook_box, false, false, 0);


  /* Drawable creation. */
  this->skybox = GLSkyBox::create();
  this->skybox->set_visible(false);
  this->reference_mesh = GLTriangleMesh::create();
  this->evolving_mesh = GLTriangleMesh::create();
  this->reference_debug = GLTriangleDebug::create();
  this->evolving_debug = GLTriangleDebug::create();
  this->reference_features = GLFeatureEdges::create();
  this->evolving_features = GLFeatureEdges::create();

  this->refmesh_opts.set_mesh_drawable(this->reference_mesh);
  this->refmesh_opts.set_debug_drawable(this->reference_debug);
  this->refmesh_opts.set_features_drawable(this->reference_features);
  this->evomesh_opts.set_mesh_drawable(this->evolving_mesh);
  this->evomesh_opts.set_debug_drawable(this->evolving_debug);
  this->evomesh_opts.set_features_drawable(this->evolving_features);
  this->refmesh_opts.set_shade_color(0.6f, 0.6f, 0.6f, 1.0f);
  this->refmesh_opts.set_deco_color(0.0f, 0.0f, 0.0f, 0.5f);
  this->refmesh_opts.set_feature_color(0.2f, 0.2f, 0.2f, 1.0f);
  //this->refmesh_opts.set_feature_color(0.1f, 1.0f, 0.2f, 1.0f);
  this->evomesh_opts.set_shade_color(0.5f, 0.5f, 0.66f, 1.0f); //#8080aa
  //this->evomesh_opts.set_shade_color(0.6f, 0.4f, 0.4f, 1.0f);
  this->evomesh_opts.set_deco_color(0.0f, 0.0f, 0.0f, 0.5f);
  this->evomesh_opts.set_feature_color(0.2f, 0.3f, 1.0f, 1.0f);
  //this->evomesh_opts.set_feature_color(1.0f, 0.7f, 0.4f, 1.0f);
  this->refmesh_opts.initialize();
  this->evomesh_opts.initialize();

  this->on_background_color_set();

  /* Scene configuration. */
  this->evolving_mesh->set_mesh(this->iface.get_evolving_mesh());
  this->glarea.add_drawable(this->skybox);
  this->glarea.add_drawable(this->reference_mesh);
  this->glarea.add_drawable(this->evolving_mesh);
  this->glarea.add_drawable(this->reference_debug);
  this->glarea.add_drawable(this->evolving_debug);
  this->glarea.add_drawable(this->reference_features);
  this->glarea.add_drawable(this->evolving_features);
  //this->glarea.add_drawable(GLTestScene::create());
  //this->glarea.add_drawable(GLFrustaRenderer::create());

  this->glarea.refresh();

  /* Register signals. */
  this->fcb.signal_selection_changed().connect(sigc::mem_fun
      (*this, &MainGui::on_model_selected));

  this->reference_save.signal_clicked().connect(sigc::bind(sigc::mem_fun
      (*this, &MainGui::on_mesh_save_request), true));
  this->reference_refresh.signal_clicked().connect(sigc::mem_fun
      (*this, &MainGui::on_model_selected));
  this->reference_clean.signal_clicked().connect(sigc::mem_fun
      (*this, &MainGui::on_reference_clean));
  this->reference_copy.signal_clicked().connect(sigc::mem_fun
      (*this, &MainGui::on_reference_copy));

  this->evolving_save.signal_clicked().connect(sigc::bind(sigc::mem_fun
      (*this, &MainGui::on_mesh_save_request), false));
  this->evolving_copy.signal_clicked().connect(sigc::mem_fun
      (*this, &MainGui::on_evolving_mesh_copy));

  this->test1_but.signal_clicked().connect(sigc::mem_fun
      (*this, &MainGui::on_test_1_clicked));
  this->test2_but.signal_clicked().connect(sigc::mem_fun
      (*this, &MainGui::on_test_2_clicked));
  this->test3_but.signal_clicked().connect(sigc::mem_fun
      (*this, &MainGui::on_test_3_clicked));

  this->stats_refmesh_but.signal_clicked().connect(sigc::bind(sigc::mem_fun
      (*this, &MainGui::on_meshstats_clicked), true));
  this->stats_evomesh_but.signal_clicked().connect(sigc::bind(sigc::mem_fun
      (*this, &MainGui::on_meshstats_clicked), false));
  this->lookat_refmesh_but.signal_clicked().connect(sigc::bind(sigc::mem_fun
      (*this, &MainGui::on_lookat_clicked), true));
  this->lookat_evomesh_but.signal_clicked().connect(sigc::bind(sigc::mem_fun
      (*this, &MainGui::on_lookat_clicked), false));

  scale_and_center_but->signal_clicked().connect(sigc::mem_fun
      (*this, &MainGui::on_scale_and_center_clicked));
  inv_faces_but->signal_clicked().connect(sigc::mem_fun
      (*this, &MainGui::on_invert_faces_clicked));
  gl_info_but->signal_clicked().connect(sigc::mem_fun
      (*this, &MainGui::on_gl_info_clicked));
  app_exit_but->signal_clicked().connect(sigc::mem_fun
      (*this, &Gtk::Window::hide));
  this->mesh_visibility_cb.signal_clicked().connect(sigc::mem_fun
      (*this, &MainGui::on_mesh_visibility_changed));
  cam_simple_but->signal_clicked().connect(sigc::bind(sigc::mem_fun
      (*this, &MainGui::on_camera_changed), 0));
  cam_trackball_but->signal_clicked().connect(sigc::bind(sigc::mem_fun
      (*this, &MainGui::on_camera_changed), 1));

  /* Algorithm configuration and execution. */
  step_density_exec_but->signal_clicked().connect(sigc::mem_fun
      (*this, &MainGui::on_density_field_execute));
  step_density_clear_but->signal_clicked().connect(sigc::mem_fun
      (*this, &MainGui::on_density_field_clear));
  step_density_conf_but->signal_clicked().connect(sigc::mem_fun
      (*this, &MainGui::on_density_field_settings));

  step_feature_extract_but->signal_clicked().connect(sigc::mem_fun
      (*this, &MainGui::on_feature_extract_execute));
  step_feature_clear_but->signal_clicked().connect(sigc::mem_fun
      (*this, &MainGui::on_feature_extract_clear));
  step_feature_conf_but->signal_clicked().connect(sigc::mem_fun
      (*this, &MainGui::on_feature_extract_settings));

  step_oversampling_but->signal_clicked().connect(sigc::mem_fun
      (*this, &MainGui::on_oversampling_execute));
  step_oversampling_conf_but->signal_clicked().connect(sigc::mem_fun
      (*this, &MainGui::on_oversampling_settings));

  step_resampling_exec_but->signal_clicked().connect(sigc::mem_fun
      (*this, &MainGui::on_resampling_execute));
  step_resampling_conf_but->signal_clicked().connect(sigc::mem_fun
      (*this, &MainGui::on_resampling_settings));

  step_simpl_exec_but->signal_clicked().connect(sigc::mem_fun
      (*this, &MainGui::on_simplification_execute));
  step_simpl_conf_but->signal_clicked().connect(sigc::mem_fun
      (*this, &MainGui::on_simplification_settings));

  step_area_equal_exec_but->signal_clicked().connect(sigc::mem_fun
      (*this, &MainGui::on_area_equal_execute));
  step_area_equal_conf_but->signal_clicked().connect(sigc::mem_fun
      (*this, &MainGui::on_area_equal_settings));

  step_lloyd_exec_but->signal_clicked().connect(sigc::mem_fun
      (*this, &MainGui::on_lloyd_execute));
  step_lloyd_conf_but->signal_clicked().connect(sigc::mem_fun
      (*this, &MainGui::on_lloyd_settings));

  this->density_config.signal_config_changed().connect(sigc::mem_fun
      (*this, &MainGui::on_config_changed));
  this->simpl_config.signal_config_changed().connect(sigc::mem_fun
      (*this, &MainGui::on_config_changed));
  this->oversampling_config.signal_config_changed().connect(sigc::mem_fun
      (*this, &MainGui::on_config_changed));
  this->resampling_config.signal_config_changed().connect(sigc::mem_fun
      (*this, &MainGui::on_config_changed));
  this->feature_config.signal_config_changed().connect(sigc::mem_fun
      (*this, &MainGui::on_config_changed));
  this->feature_config.signal_features_changed().connect(sigc::mem_fun
      (*this, &MainGui::update_glarea));
  this->lloyd_config.signal_config_changed().connect(sigc::mem_fun
      (*this, &MainGui::on_config_changed));

  /* Rendering. */
  this->bgcolor.signal_color_set().connect(sigc::mem_fun
      (*this, &MainGui::on_background_color_set));
  this->render_skybox.signal_clicked().connect(sigc::mem_fun
      (*this, &MainGui::on_skybox_toggled));

  this->refmesh_opts.signal_redraw().connect(sigc::mem_fun
      (this->glarea, &GtkOpenGLScene::refresh));
  this->evomesh_opts.signal_redraw().connect(sigc::mem_fun
      (this->glarea, &GtkOpenGLScene::refresh));

  /* Window configuration. */
  this->add(*main_hbox);
  this->set_default_size(800, 480);
  this->set_title("GTK Remesher Frontend");
  this->show_all();

  this->bgcolor.set_color(Gdk::Color("white"));

  if (!Arguments::load_model.empty())
    this->fcb.set_filename(Arguments::load_model);
}

/* ---------------------------------------------------------------- */

Gtk::Widget*
MainGui::create_header (Glib::ustring const& label)
{
  Gtk::HBox* hbox = MK_HBOX;
  hbox->pack_start(*MK_HSEP, true, true, 0);
  hbox->pack_start(*MK_LABEL(label), false, false, 0);
  hbox->pack_start(*MK_HSEP, true, true, 0);
  return hbox;
}

/* ---------------------------------------------------------------- */

void
MainGui::on_model_selected (void)
{
  std::string filename = this->fcb.get_filename();
  if (filename.empty())
    return;

  /* Try to load the model using the remesher interface. */
  try
  {
    this->iface.load_model(filename);
  }
  catch (Remesher::Exception& e)
  {
    Gtk::MessageDialog md("Error reading model data!",
        false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK);
    md.set_secondary_text("The model data could not be loaded. "
        "This either means the model data is not understood, "
        "or the file could not been opened for some reason.\n\n"
        "Error message: " + e);
    md.set_title("Error loading model");
    md.set_transient_for(*this);
    md.run();
    return;
  }

  /* If auto-preprocessing is enabled, scale & center, clean and optimize the mesh. */
  if (this->auto_preprocess_cb.get_active())
  {
    std::cout << "Auto-preprocessing the reference mesh..." << std::endl;

    try
    {
      this->iface.get_reference_mesh()->scale_and_center();
      this->iface.clean_reference_mesh();
      this->iface.optimize_reference_mesh();
    }
    catch (Remesher::Exception& e)
    {
      this->on_error(e);
      return;
    }
  }

  /* Update GUI and display the shiny new mesh. */
  this->on_reference_mesh_changed();
  this->glarea.refresh();
}

/* ---------------------------------------------------------------- */

void
MainGui::on_reference_mesh_changed (void)
{
  Remesher::TriangleMeshPtr mesh = this->iface.get_reference_mesh();
  Remesher::FeatureEdgesPtr features = this->iface.get_reference_features();

  this->reference_mesh->set_mesh(mesh);
  this->reference_debug->set_mesh(mesh);
  this->reference_features->set_mesh(mesh);
  this->reference_features->set_feature_edges(features);

  this->reference_vertices.set_text(Remesher::Helpers::get_dotted_str
      (this->iface.get_reference_mesh()->get_vertices().size()));
  this->reference_triangles.set_text(Remesher::Helpers::get_dotted_str
      (this->iface.get_reference_mesh()->get_faces().size() / 3));
}

/* ---------------------------------------------------------------- */

void
MainGui::on_evolving_mesh_changed (void)
{
  this->evolving_mesh->set_mesh(this->iface.get_evolving_mesh());
  this->evolving_features->set_mesh(this->iface.get_evolving_mesh());
  this->evolving_features->set_feature_edges
      (this->iface.get_evolving_features());

  this->evolving_vertices.set_text(Remesher::Helpers::get_dotted_str
      (this->evolving_mesh->get_mesh()->get_vertices().size()));
  this->evolving_triangles.set_text(Remesher::Helpers::get_dotted_str
      (this->evolving_mesh->get_mesh()->get_faces().size() / 3));
  this->evolving_debug->set_mesh(this->evolving_mesh->get_mesh());
}

/* ---------------------------------------------------------------- */

void
MainGui::on_mesh_save_request (bool reference)
{
  char const* wintitle = 0;
  Remesher::TriangleMeshPtr mesh;

  if (reference)
  {
    wintitle = "Save reference mesh...";
    mesh = this->iface.get_reference_mesh();
  }
  else
  {
    wintitle = "Save evolving mesh...";
    mesh = this->iface.get_evolving_mesh();
  }

  if (mesh.get() == 0 || mesh->get_vertices().empty())
  {
    Gtk::MessageDialog md("Cannot save the mesh!",
        false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK);
    md.set_secondary_text("The mesh cannot be saved since it is not\n"
        "initialized or does not contain data to be saved.");
    md.set_title("Error saving mesh");
    md.set_transient_for(*this);
    md.run();
    return;
  }

  std::string input_filename = this->fcb.get_filename();

  Gtk::FileChooserDialog fcd(*this, wintitle, Gtk::FILE_CHOOSER_ACTION_SAVE);
  fcd.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
  fcd.add_button(Gtk::Stock::SAVE, Gtk::RESPONSE_OK);
  fcd.set_do_overwrite_confirmation(true);
  fcd.set_filename(input_filename);

#define FILE_FILTER_OFF "OFF model format (*.off)"
#define FILE_FILTER_PLY "PLY model format (*.ply)"
#define FILE_FILTER_SVG "PLY model format (*.svg)"

  Gtk::FileFilter off_filter;
  off_filter.add_pattern("*.off");
  off_filter.set_name(FILE_FILTER_OFF);

  Gtk::FileFilter ply_filter;
  ply_filter.add_pattern("*.ply");
  ply_filter.set_name(FILE_FILTER_PLY);

  Gtk::FileFilter svg_filter;
  ply_filter.add_pattern("*.svg");
  ply_filter.set_name(FILE_FILTER_SVG);

  fcd.add_filter(off_filter);
  fcd.add_filter(ply_filter);
  fcd.add_filter(svg_filter);

  std::string filename;
  while (true)
  {
    int ret = fcd.run();
    if (ret != Gtk::RESPONSE_OK)
      return;

    filename = fcd.get_filename();
    std::string extension(".off");
    bool filename_changed = false;

    /* Define the desired extension according to the filter. */
    Gtk::FileFilter* ffilter = fcd.get_filter();
    if (ffilter == &off_filter)
      extension = ".off";
    else if (ffilter == &ply_filter)
      extension = ".ply";
    else if (ffilter == &svg_filter)
      extension = ".svg";

    /* Append extension to filename there is no extension yet. */
    if (filename.size() < 4)
    {
      filename += extension;
      filename_changed = true;
    }
    else
    {
      std::string file_ext = filename.substr(filename.size() - 4);
      if (file_ext != ".off" && file_ext != ".ply" && file_ext != ".svg")
      {
        filename += extension;
        filename_changed = true;
      }
    }

    if (filename_changed)
    {
      fcd.set_current_name(filename);
      fcd.response(Gtk::RESPONSE_OK);
    }
    else
      break;
  }

  fcd.hide();

  try
  {
    Remesher::ModelWriter::save_model(filename, mesh);
  }
  catch (Remesher::Exception& e)
  {
    Gtk::MessageDialog md("Error saving mesh to file!",
        false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK);
    md.set_secondary_text("The mesh could not be saved, because the "
        "model exporter reported an error.\n\n"
        "The error message is: " + e);
    md.set_title("Error saving mesh to file");
    md.set_transient_for(*this);
    md.run();
    return;
  }
}

/* ---------------------------------------------------------------- */

void
MainGui::on_test_1_clicked (void)
{
  #if 0
  Remesher::TriangleMeshPtr mesh = this->iface.get_reference_mesh();
  Remesher::FeatureEdgesPtr features = this->iface.get_reference_features();

  std::vector<bool> dlist;
  dlist.resize(mesh->get_vertices().size());
  for (std::size_t i = 0; i < dlist.size(); ++i)
    dlist[i] = (::rand() % 2 == 0);

  Remesher::MeshDecimation md;
  md.set_mesh(mesh);
  md.set_features(features);
  md.set_delete_list(dlist);
  md.start_decimation();
  
  mesh->recalc_normals();
  this->on_reference_mesh_changed();
  this->reference_mesh->update_mesh();
  this->glarea.refresh();
  #endif

  #if 0
  /* Remove large triangles test. */
  Remesher::TriangleMeshPtr mesh =  this->iface.get_reference_mesh();
  Remesher::MeshVertexList& verts = mesh->get_vertices();
  Remesher::MeshFaceList& faces = mesh->get_faces();
  Remesher::Vec3f min(INFINITY, INFINITY, INFINITY);
  Remesher::Vec3f max(-INFINITY, -INFINITY, -INFINITY);
  for (std::size_t i = 0; i < verts.size(); ++i)
    for (std::size_t j = 0; j < 3; ++j)
    {
      if (verts[i][j] < min[j])
        min[j] = verts[i][j];
      if (verts[i][j] > max[j])
        max[j] = verts[i][j];
    }
  float diag = (max - min).length();
  float thres = diag * 0.01f;

  std::cout << "Diagonal length: " << diag << ", threshold: " << thres << std::endl;

  std::size_t face_amount = faces.size() / 3;
  std::size_t deleted = 0;
  for (std::size_t i = 0; i < face_amount; ++i)
  {
    Remesher::Vec3f const& v1 = verts[faces[i * 3 + 0]];
    Remesher::Vec3f const& v2 = verts[faces[i * 3 + 1]];
    Remesher::Vec3f const& v3 = verts[faces[i * 3 + 2]];
    float l1 = (v2 - v1).length();
    float l2 = (v3 - v1).length();
    float l3 = (v2 - v3).length();
    float maxlen = std::max(l1, std::max(l2, l3));
    if (maxlen > thres)
    {
      faces[i * 3 + 0] = 0;
      faces[i * 3 + 1] = 0;
      faces[i * 3 + 2] = 0;
      deleted += 1;
    }
  }

  std::cout << "Deleted " << deleted << " faces of the mesh" << std::endl;
  mesh->recalc_normals();
  this->on_reference_mesh_changed();
  this->reference_mesh->update_mesh();
  this->glarea.refresh();
  #endif
  

  #if 1
  Remesher::TriangleMeshPtr mesh = this->iface.get_evolving_mesh();
  Remesher::VertexInfoListPtr vinfo = Remesher::VertexInfoList::create(mesh);

  Remesher::MeshStats ms(mesh, vinfo);
  ms.print_stats();
  #endif


  #if 0
  Remesher::TriangleMeshPtr mesh =  this->iface.get_reference_mesh();
  Remesher::MeshFaceList& faces = mesh->get_faces();
  for (std::size_t i = 0; i < faces.size(); ++i)
    faces[i] = faces[i] - 1;
  #endif

  #if 0
  #endif

  #if 0
  this->iface.exec_delaunay_flips();
  this->evolving_mesh->get_mesh()->recalc_normals();
  this->evolving_mesh->regen_face_shading();
  this->glarea.refresh();
  return;
  #endif

  /*
  Remesher::VertexInfoList vinfo(this->iface.get_evolving_mesh());
  Remesher::DelaunayFlips d3d;
  d3d.set_mesh(this->iface.get_evolving_mesh());
  d3d.set_vertex_info(vinfo);
  d3d.set_feature_edges(this->iface.get_evolving_features());
  d3d.flip_edges();

  this->evolving_mesh->get_mesh()->recalc_normals();
  this->evolving_mesh->regen_face_shading();
  this->glarea.refresh();
  */
}

/* ---------------------------------------------------------------- */

void
MainGui::on_test_2_clicked (void)
{
  /* Voronoi cell stats. */
  Remesher::VertexRefListPtr vref = this->iface.get_evolving_vertex_refs();
  Remesher::VertexInfoListPtr vinfo = this->iface.get_evolving_vertex_info();
  Remesher::DensityFieldPtr dens = this->iface.get_density_field();
  Remesher::TriangleMeshPtr evomesh = this->iface.get_evolving_mesh();

  Remesher::CvtStats stats;
  //stats.set_ref_mesh(...);
  stats.set_ref_density(dens);
  stats.set_evo_mesh(evomesh);
  stats.set_evo_vinfo(vinfo);
  stats.set_evo_vref(vref);

  stats.calc_stats("/tmp/cvtstats.txt");
}

/* ---------------------------------------------------------------- */

void
MainGui::on_test_3_clicked (void)
{
  /* Duplicated vertex cleaning test. */
  this->iface.clean_duplicated_vertices();
  this->on_reference_mesh_changed();
  this->reference_mesh->update_mesh();
  this->glarea.refresh();
}

/* ---------------------------------------------------------------- */

void
MainGui::on_meshstats_clicked (bool refmesh)
{
  Remesher::TriangleMeshPtr mesh;
  if (refmesh)
    mesh = this->reference_mesh->get_mesh();
  else
    mesh = this->evolving_mesh->get_mesh();

  if (!mesh.get())
  {
    std::cout << "Mesh not initialized!" << std::endl;
    return;
  }

  Remesher::VertexInfoListPtr vinfo = Remesher::VertexInfoList::create(mesh);
  Remesher::MeshStats ms(mesh, vinfo);
  ms.print_stats();
}

/* ---------------------------------------------------------------- */

void
MainGui::on_lookat_clicked (bool refmesh)
{
  Remesher::TriangleMeshPtr mesh;
  if (refmesh)
    mesh = this->reference_mesh->get_mesh();
  else
    mesh = this->evolving_mesh->get_mesh();

  if (!mesh.get())
  {
    std::cout << "Mesh not initialized!" << std::endl;
    return;
  }

  Gtk::MessageDialog dialog(*this, "Enter element ID to look at...",
      false, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_OK_CANCEL);
  dialog.set_secondary_text("Please enter the element ID of a vertex "
      "or face of the reference mesh to look at.");
  dialog.set_title("Look at...");
  dialog.set_default_response(Gtk::RESPONSE_OK);

  /* Create widgets to enter the vertex ID. */
  Gtk::Label* label_vid = MK_LABEL_LEFT("Vertex ID:");
  Gtk::Entry* entry_vid = MK_ENTRY;
  Gtk::Label* label_fid = MK_LABEL_LEFT("Face ID:");
  Gtk::Entry* entry_fid = MK_ENTRY;

  Gtk::Table* entry_tbl = MK_TABLE(2,2);
  entry_tbl->attach(*label_vid, 0, 1, 0, 1, Gtk::FILL | Gtk::SHRINK);
  entry_tbl->attach(*entry_vid, 1, 2, 0, 1);
  entry_tbl->attach(*label_fid, 0, 1, 1, 2, Gtk::FILL | Gtk::SHRINK);
  entry_tbl->attach(*entry_fid, 1, 2, 1, 2);

  Gtk::VBox* dialog_box = dialog.get_vbox();
  dialog_box->pack_start(*entry_tbl, false, false, 0);
  dialog_box->show_all();

  /* Run the dialog. */
  int ret = dialog.run();
  dialog.hide();
  std::string vid_text = entry_vid->get_text();
  std::string fid_text = entry_fid->get_text();
  if (ret != Gtk::RESPONSE_OK || (vid_text.empty() && fid_text.empty()))
    return;

  bool lookup_vertex = !vid_text.empty();
  std::size_t id = 0;
  if (lookup_vertex)
    id = Remesher::Helpers::get_sizet_from_string(vid_text);
  else
    id = Remesher::Helpers::get_sizet_from_string(fid_text);

  /* Get mesh faces/vertices. */
  Remesher::MeshVertexList const& verts = mesh->get_vertices();
  Remesher::MeshFaceList const& faces = mesh->get_faces();

  if (lookup_vertex && verts.size() <= id)
  {
    std::cout << "Vertex ID out of bounds!" << std::endl;
    return;
  }
  else if (!lookup_vertex && faces.size() / 3 <= id)
  {
    std::cout << "Face ID out of bounds!" << std::endl;
    return;
  }

  /* Find vertex and update camera control. */
  CameraControl* cc = this->glarea.get_camera_control().get();
  CameraTrackball* ct = dynamic_cast<CameraTrackball*>(cc);
  if (ct == 0)
  {
    std::cout << "Invalid camera control" << std::endl;
    return;
  }

  /* Determine position to look at. */
  Remesher::Vec3f center(0.0f, 0.0f, 0.0f);

  if (lookup_vertex)
    center = verts[id];
  else
    center = (verts[faces[id * 3]] + verts[faces[id * 3 + 1]]
        + verts[faces[id * 3 + 2]]) / 3.0f;
  ct->set_center(center);
  
  this->glarea.refresh();
}

/* ---------------------------------------------------------------- */

void
MainGui::on_scale_and_center_clicked (void)
{
  this->reference_mesh->get_mesh()->scale_and_center();
  this->reference_mesh->update_mesh();
  this->glarea.refresh();
}

/* ---------------------------------------------------------------- */

void
MainGui::on_invert_faces_clicked (void)
{
  this->reference_mesh->get_mesh()->invert_faces();
  this->reference_mesh->update_mesh();
  this->glarea.refresh();
}

/* ---------------------------------------------------------------- */

void
MainGui::on_simplification_execute (void)
{
  try
  {
    this->iface.exec_simplification();
  }
  catch (Remesher::Exception& e)
  {
    this->on_error(e);
    return;
  }

  this->on_evolving_mesh_changed();
  this->evolving_features->set_feature_edges
      (this->iface.get_evolving_features());

  this->glarea.refresh();
}

/* ---------------------------------------------------------------- */

void
MainGui::on_simplification_settings (void)
{
  this->simpl_config.set_transient_for(*this);
  this->simpl_config.show_all();
}

/* ---------------------------------------------------------------- */

void
MainGui::on_density_field_execute (void)
{
  try
  {
    this->iface.exec_density_calculation();
  }
  catch (Remesher::Exception& e)
  {
    this->on_error(e);
    return;
  }

  Remesher::DensityFieldPtr df = this->iface.get_density_field();
  float max_density = df->get_max_density();

  GLColorArray& colors = this->reference_mesh->get_color_array();
  colors.clear();
  colors.reserve(df->size());
  for (std::size_t i = 0; i < df->size(); ++i)
  {
    Remesher::VertexDensity const& vd = df[i];
    if (vd.valid)
    {
      float density = vd.density / max_density;
#define DENSITY_COLOR 2
#if DENSITY_COLOR == 1
      /* Classic density field visualization. */
      float red = density;
      float green = 0.0f;
      float blue = (1.0f - density) / 2.0f;
#elif DENSITY_COLOR == 2
      /* New visualization as used in the thesis. */
      float white_level = 153.0f / 255.0f;
      float red = white_level + density * (1.0f - white_level);
      float green = white_level - density * white_level;
      float blue = white_level - density * white_level;
#endif
      colors.push_back(Remesher::Vec3f(red, green, blue));
    }
    else
    {
      colors.push_back(Remesher::Vec3f(0.0f, 1.0f, 0.0f));
    }
  }

  this->reference_mesh->update_mesh();
  this->glarea.refresh();
}

/* ---------------------------------------------------------------- */

void
MainGui::on_density_field_clear (void)
{
  this->iface.clear_density_field();
  this->reference_mesh->get_color_array().clear();
  this->reference_mesh->update_mesh();
  this->glarea.refresh();
}

/* ---------------------------------------------------------------- */

void
MainGui::on_feature_extract_execute (void)
{
  try
  {
    this->iface.exec_feature_extraction();
  }
  catch (Remesher::Exception& e)
  {
    this->on_error(e);
    return;
  }

  this->on_reference_mesh_changed();
  this->glarea.refresh();
}

/* ---------------------------------------------------------------- */

void
MainGui::on_feature_extract_clear (void)
{
  this->iface.clear_feature_edges();
  this->on_reference_mesh_changed();
  this->glarea.refresh();
}

/* ---------------------------------------------------------------- */

void
MainGui::on_feature_extract_settings (void)
{
  this->feature_config.set_features(this->iface.get_reference_features());
  this->feature_config.set_transient_for(*this);
  this->feature_config.show_all();
}

/* ---------------------------------------------------------------- */

void
MainGui::on_density_field_settings (void)
{
  this->density_config.set_transient_for(*this);
  this->density_config.show_all();
}

/* ---------------------------------------------------------------- */

void
MainGui::on_oversampling_execute (void)
{
  try
  {
    this->iface.exec_oversampling();
  }
  catch (Remesher::Exception& e)
  {
    this->on_error(e);
    return;
  }

  this->on_reference_mesh_changed();
  this->glarea.refresh();
}

/* ---------------------------------------------------------------- */

void
MainGui::on_oversampling_settings (void)
{
  this->oversampling_config.set_transient_for(*this);
  this->oversampling_config.show_all();
}

/* ---------------------------------------------------------------- */

void
MainGui::on_resampling_execute (void)
{
  this->iface.exec_resampling();
  this->on_evolving_mesh_changed();
  this->evolving_mesh->regen_face_shading();
  this->glarea.refresh();
}

/* ---------------------------------------------------------------- */

void
MainGui::on_resampling_settings (void)
{
  this->resampling_config.set_transient_for(*this);
  this->resampling_config.show_all();
}

/* ---------------------------------------------------------------- */

void
MainGui::on_lloyd_execute (void)
{
  try
  {
    this->iface.exec_lloyd();
  }
  catch (Remesher::Exception& e)
  {
    this->on_error(e);
    return;
  }

  this->evolving_mesh->regen_face_shading();
  this->glarea.refresh();
}

/* ---------------------------------------------------------------- */

void
MainGui::on_lloyd_settings (void)
{
  this->lloyd_config.set_transient_for(*this);
  this->lloyd_config.show_all();
}

/* ---------------------------------------------------------------- */

void
MainGui::on_area_equal_execute (void)
{
  try
  {
    this->iface.exec_area_equalization();
  }
  catch (Remesher::Exception& e)
  {
    this->on_error(e);
    return;
  }

  this->evolving_mesh->regen_face_shading();
  this->glarea.refresh();
}

/* ---------------------------------------------------------------- */

void
MainGui::on_area_equal_settings (void)
{
  this->area_equal_config.set_transient_for(*this);
  this->area_equal_config.show_all();
}

/* ---------------------------------------------------------------- */

void
MainGui::on_config_changed (void)
{
  this->iface.set_oversampling_conf(this->oversampling_config.get_config());
  this->iface.set_density_field_conf(this->density_config.get_config());
  this->iface.set_feature_edges_conf(this->feature_config.get_config());
  this->iface.set_simplification_conf(this->simpl_config.get_config());
  this->iface.set_resampling_conf(this->resampling_config.get_config());
  this->iface.set_area_equal_conf(this->area_equal_config.get_config());
  this->iface.set_lloyd_conf(this->lloyd_config.get_config());
}

/* ---------------------------------------------------------------- */

void
MainGui::on_background_color_set (void)
{
  Gdk::Color color = this->bgcolor.get_color();
  glClearColor((float)color.get_red() / 65535.0f,
      (float)color.get_green() / 65535.0f,
      (float)color.get_blue() / 65535.0f, 1.0f);

  this->glarea.refresh();
}

/* ---------------------------------------------------------------- */

void
MainGui::on_gl_info_clicked (void)
{
  Gtk::Window* win = new GuiGLInfo;
  win->set_transient_for(*this);
}

/* ---------------------------------------------------------------- */

void
MainGui::on_camera_changed (int type)
{
  switch (type)
  {
    case 0:
      this->glarea.set_camera_control(CameraSimple::create());
      break;
    default:
    case 1:
      this->glarea.set_camera_control(CameraTrackball::create());
      break;
  }
  this->glarea.refresh();
}

/* ---------------------------------------------------------------- */

void
MainGui::on_reference_clean (void)
{
  try
  {
    this->iface.clean_reference_mesh();
    this->on_reference_mesh_changed();
    this->reference_mesh->regen_face_shading();
  }
  catch (Remesher::Exception& e)
  {
    this->on_error(e);
    return;
  }

  this->glarea.refresh();
}

/* ---------------------------------------------------------------- */

void
MainGui::on_reference_copy (void)
{
  try
  {
    this->iface.copy_reference_mesh();
  }
  catch (Remesher::Exception& e)
  {
    this->on_error(e);
    return;
  }

  this->on_evolving_mesh_changed();
  this->glarea.refresh();
}

/* ---------------------------------------------------------------- */

void
MainGui::on_evolving_mesh_copy (void)
{
  try
  {
    this->iface.copy_evolving_mesh();
  }
  catch (Remesher::Exception& e)
  {
    this->on_error(e);
    return;
  }

  this->on_reference_mesh_changed();
  this->glarea.refresh();
}

/* ---------------------------------------------------------------- */

void
MainGui::on_skybox_toggled (void)
{
  this->skybox->set_visible(this->render_skybox.get_active());
  this->glarea.refresh();
}

/* ---------------------------------------------------------------- */

void
MainGui::on_mesh_visibility_changed (void)
{
  if (refmesh_opts.get_draw_mesh() == evomesh_opts.get_draw_mesh())
  {
    this->refmesh_opts.set_draw_mesh(true);
    this->evomesh_opts.set_draw_mesh(false);
    return;
  }

  if (refmesh_opts.get_draw_mesh())
  {
    this->refmesh_opts.set_draw_mesh(false);
    this->evomesh_opts.set_draw_mesh(true);
  }
  else
  {
    this->refmesh_opts.set_draw_mesh(true);
    this->evomesh_opts.set_draw_mesh(false);
  }
}

/* ---------------------------------------------------------------- */

void
MainGui::on_error (std::string const& message)
{
  Gtk::MessageDialog md(message, false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK);
  md.set_title("Error");
  md.set_transient_for(*this);
  md.run();
}

/* ---------------------------------------------------------------- */

void
MainGui::update_glarea (void)
{
  this->on_reference_mesh_changed();
  this->glarea.refresh();
}
