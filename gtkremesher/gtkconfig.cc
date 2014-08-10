#include <fstream>
#include <iostream>
#include <cmath>
#include <gtkmm/table.h>
#include <gtkmm/separator.h>
#include <gtkmm/stock.h>
#include <gtkmm/notebook.h>
#include <gtkmm/label.h>
#include <gtkmm/box.h>

#include "libremesh/defines.h"
#include "libremesh/helpers.h"
#include "gtkdefines.h"
#include "gtkconfig.h"

GtkConfigBase::GtkConfigBase (void)
  : main_box(false, 5)
{
  Gtk::HBox* save_close_box = MK_HBOX;
  save_close_box->pack_start(*MK_IMG(Gtk::Stock::SAVE, Gtk::ICON_SIZE_BUTTON));
  save_close_box->pack_start(*MK_IMG(Gtk::Stock::CLOSE, Gtk::ICON_SIZE_BUTTON));

  this->save_close_but.add(*save_close_box);
  this->close_but.set_image(*MK_IMG(Gtk::Stock::CLOSE, Gtk::ICON_SIZE_BUTTON));
  this->save_but.set_image(*MK_IMG(Gtk::Stock::SAVE, Gtk::ICON_SIZE_BUTTON));
  this->defaults_but.set_image(*MK_IMG(Gtk::Stock::REVERT_TO_SAVED,
      Gtk::ICON_SIZE_BUTTON));

  Gtk::HBox* button_box = MK_HBOX;
  button_box->pack_start(this->close_but, false, false, 0);
  button_box->pack_start(this->defaults_but, false, false, 0);
  button_box->pack_start(*MK_HSEP, true, true, 0);
  button_box->pack_start(this->save_but, false, false, 0);
  button_box->pack_start(this->save_close_but, false, false, 0);

  Gtk::VBox* layout_box = MK_VBOX;
  layout_box->set_border_width(5);
  layout_box->pack_start(this->main_box, true, true, 0);
  layout_box->pack_start(*button_box, false, false, 0);

  this->save_but.signal_clicked().connect(sigc::mem_fun
      (*this, &GtkConfigBase::on_save_clicked));
  this->save_close_but.signal_clicked().connect(sigc::mem_fun
      (*this, &GtkConfigBase::on_save_clicked));
  this->save_close_but.signal_clicked().connect(sigc::mem_fun
      (*this, &GtkConfigBase::on_close_clicked));
  this->close_but.signal_clicked().connect(sigc::mem_fun
      (*this, &GtkConfigBase::on_close_clicked));
  this->defaults_but.signal_clicked().connect(sigc::mem_fun
      (*this, &GtkConfigBase::defaults));

  this->set_title("Configuration");
  this->set_default_size(300, 200);
  this->add(*layout_box);
}

/* ---------------------------------------------------------------- */

void
GtkConfigBase::on_close_clicked (void)
{
  this->revert();
  this->hide();
}

/* ---------------------------------------------------------------- */

void
GtkConfigBase::on_save_clicked (void)
{
  this->apply();
  this->sig_config_changed.emit();
}

/* ================================================================ */

GtkSimplificationConfig::GtkSimplificationConfig (void)
  : perform_fidelity_checks("Perform fidelity checks"),
    perform_area_checks("Perform vertex area checks"),
    local_feature_detection("Check for local features"),
    keep_global_features("Don't touch global features")
{
  this->vertex_amount.set_digits(0);
  this->vertex_amount.set_increments(100, 1000);
  this->vertex_amount.set_range(3, 10000000);

  this->fidelity_check_penalty.set_increments(0.1, 1.0);
  this->fidelity_check_penalty.set_range(1.0, 10.0);
  this->fidelity_check_penalty.set_value_pos(Gtk::POS_RIGHT);

  this->area_check_penalty.set_increments(0.1, 1.0);
  this->area_check_penalty.set_range(1.0, 10.0);
  this->area_check_penalty.set_value_pos(Gtk::POS_RIGHT);

  this->thres_factor.set_increments(0.1, 0.5);
  this->thres_factor.set_range(1.1, 4.0);
  this->thres_factor.set_value_pos(Gtk::POS_RIGHT);

  this->init_threshold.set_increments(0.1, 1.0);
  this->init_threshold.set_range(1.0, 7.0);
  this->init_threshold.set_value_pos(Gtk::POS_RIGHT);

  this->max_threshold.set_increments(0.1, 1.0);
  this->max_threshold.set_range(0.0, 6.0);
  this->max_threshold.set_value_pos(Gtk::POS_RIGHT);

  this->local_feature_angle.set_increments(0.1, 1.0);
  this->local_feature_angle.set_range(5.0, 175.0);
  this->local_feature_angle.set_value_pos(Gtk::POS_RIGHT);

  Gtk::HBox* fidelity_penalty_box = MK_HBOX;
  fidelity_penalty_box->pack_start(*MK_LABEL("Penalty:"), false, false);
  fidelity_penalty_box->pack_start(this->fidelity_check_penalty, true, true);

  Gtk::HBox* area_penalty_box = MK_HBOX;
  area_penalty_box->pack_start(*MK_LABEL("Penalty:"), false, false);
  area_penalty_box->pack_start(this->area_check_penalty, true, true);

  Gtk::HBox* vertex_limit_box = MK_HBOX;
  vertex_limit_box->pack_start(*MK_LABEL("Vertex limit:"), false, false, 0);
  vertex_limit_box->pack_start(this->vertex_amount, true, true, 0);

  Gtk::VBox* basic_conf_box = MK_VBOX;
  basic_conf_box->set_border_width(5);
  basic_conf_box->pack_start(*vertex_limit_box, false, false, 0);
  basic_conf_box->pack_start(this->perform_fidelity_checks, false, false, 0);
  basic_conf_box->pack_start(*fidelity_penalty_box, false, false, 0);
  basic_conf_box->pack_start(this->perform_area_checks, false, false, 0);
  basic_conf_box->pack_start(*area_penalty_box, false, false, 0);

  Gtk::Label* iter_info_label = MK_LABEL("Iteration starts with the "
      "initial threshold, multiplied by threshold factor each pass, "
      "until the maximum threshold is reached.");
  iter_info_label->set_line_wrap(true);
  iter_info_label->set_alignment(Gtk::ALIGN_LEFT);
  iter_info_label->set_width_chars(40);

  Gtk::Label* thres_factor_label = MK_LABEL("Threshold factor:");
  thres_factor_label->set_alignment(Gtk::ALIGN_LEFT);
  Gtk::Label* init_thres_label = MK_LABEL("Initial threshold:");
  init_thres_label->set_alignment(Gtk::ALIGN_LEFT);
  Gtk::Label* max_thres_label = MK_LABEL("Maximum threshold:");
  max_thres_label->set_alignment(Gtk::ALIGN_LEFT);

  Gtk::Table* iter_table = MK_TABLE(3, 2);
  iter_table->set_col_spacings(5);
  iter_table->attach(*thres_factor_label, 0, 1, 0, 1, Gtk::FILL);
  iter_table->attach(this->thres_factor, 1, 2, 0, 1, Gtk::EXPAND | Gtk::FILL);
  iter_table->attach(*init_thres_label, 0, 1, 1, 2, Gtk::FILL);
  iter_table->attach(this->init_threshold, 1, 2, 1, 2, Gtk::EXPAND | Gtk::FILL);
  iter_table->attach(*max_thres_label, 0, 1, 2, 3, Gtk::FILL);
  iter_table->attach(this->max_threshold, 1, 2, 2, 3, Gtk::EXPAND | Gtk::FILL);

  Gtk::VBox* iteration_conf_box = MK_VBOX;
  iteration_conf_box->set_border_width(5);
  iteration_conf_box->pack_start(*iter_info_label, false, false, 0);
  iteration_conf_box->pack_start(*iter_table, false, false, 0);

  Gtk::HBox* feature_angle_conf = MK_HBOX;
  feature_angle_conf->pack_start(*MK_LABEL("Feature angle:"), false, false, 0);
  feature_angle_conf->pack_start(this->local_feature_angle, true, true, 0);

  Gtk::VBox* feature_conf_box = MK_VBOX;
  feature_conf_box->set_border_width(5);
  feature_conf_box->pack_start(this->local_feature_detection, false, false, 0);
  feature_conf_box->pack_start(*feature_angle_conf, false, false, 0);
  feature_conf_box->pack_start(this->keep_global_features, false, false, 0);

  Gtk::Notebook* notebook = MK_NOTEBOOK;
  notebook->append_page(*basic_conf_box, "Basic config");
  notebook->append_page(*iteration_conf_box, "Iteration config");
  notebook->append_page(*feature_conf_box, "Features");

  this->main_box.pack_start(*notebook, true, true, 0);

  this->defaults();
  this->set_title("Simplification Configuration");
}

/* ---------------------------------------------------------------- */

void
GtkSimplificationConfig::apply (void)
{
  this->config.vertex_limit = (unsigned int)this->vertex_amount.get_value();
  this->config.perform_fidelity_checks
      = this->perform_fidelity_checks.get_active();
  this->config.perform_area_checks = this->perform_area_checks.get_active();
  this->config.fidelity_check_penalty
      = (float)this->fidelity_check_penalty.get_value();
  this->config.area_check_penalty
      = (float)this->area_check_penalty.get_value();
  this->config.threshold_factor = (float)this->thres_factor.get_value();
  this->config.initial_threshold = (float)this->init_threshold.get_value();
  this->config.maximum_threshold = (float)this->max_threshold.get_value();

  this->config.check_local_features
      = this->local_feature_detection.get_active();
  this->config.local_feature_angle = (float)(std::cos
      (MY_DEG2RAD(this->local_feature_angle.get_value())));
  this->config.keep_global_features
      = this->keep_global_features.get_active();
}

/* ---------------------------------------------------------------- */

void
GtkSimplificationConfig::revert (void)
{
  this->vertex_amount.set_value((double)this->config.vertex_limit);

  this->perform_fidelity_checks.set_active
      (this->config.perform_fidelity_checks);
  this->perform_area_checks.set_active
      (this->config.perform_area_checks);
  this->fidelity_check_penalty.set_value(this->config.fidelity_check_penalty);
  this->area_check_penalty.set_value(this->config.area_check_penalty);

  this->thres_factor.set_value(this->config.threshold_factor);
  this->init_threshold.set_value(this->config.initial_threshold);
  this->max_threshold.set_value(this->config.maximum_threshold);

  this->local_feature_detection.set_active(this->config.check_local_features);
  this->local_feature_angle.set_value
      (MY_RAD2DEG(std::acos(this->config.local_feature_angle)));
  this->keep_global_features.set_active(this->config.keep_global_features);
}

/* ---------------------------------------------------------------- */

void
GtkSimplificationConfig::defaults (void)
{
  this->config = Remesher::SimplificationConf();
  this->revert();
}

/* ================================================================ */

GtkOversamplingConfig::GtkOversamplingConfig (void)
{
  this->grid_slicing.set_label("Use grid slicing");
  this->loop_subdiv.set_label("Use loop subdivision");
  this->linear_subdiv.set_label("Use linear subdivision");
  this->lazy_triangulation.set_label("Enable lazy triangulation");

  for (std::size_t i = 0; i < 3; ++i)
  {
    this->num_slices[i].set_increments(1.0, 10.0);
    this->num_slices[i].set_range(0.0, 100.0);
    this->num_slices[i].set_digits(0);
    this->num_slices[i].set_value_pos(Gtk::POS_RIGHT);
  }

  Gtk::Button* reduce_planes_but = MK_BUT0;
  reduce_planes_but->set_image(*MK_IMG(Gtk::Stock::GO_BACK,
      Gtk::ICON_SIZE_MENU));
  Gtk::Button* increase_planes_but = MK_BUT0;
  increase_planes_but->set_image(*MK_IMG(Gtk::Stock::GO_FORWARD,
      Gtk::ICON_SIZE_MENU));

  reduce_planes_but->signal_clicked().connect(sigc::bind(sigc::mem_fun
      (*this, &GtkOversamplingConfig::change_planes), false));
  increase_planes_but->signal_clicked().connect(sigc::bind(sigc::mem_fun
      (*this, &GtkOversamplingConfig::change_planes), true));

  Gtk::Table* slicing_tbl = MK_TABLE(2, 3);
  slicing_tbl->set_row_spacings(5);
  slicing_tbl->attach(this->grid_slicing, 0, 1, 0, 1,
      Gtk::FILL | Gtk::EXPAND);
  slicing_tbl->attach(this->lazy_triangulation, 0, 1, 1, 2,
      Gtk::FILL | Gtk::EXPAND);
  slicing_tbl->attach(*reduce_planes_but, 1, 2, 0, 2,
      Gtk::SHRINK, Gtk::SHRINK);
  slicing_tbl->attach(*increase_planes_but, 2, 3, 0, 2,
      Gtk::SHRINK, Gtk::SHRINK);

  this->main_box.pack_start(this->loop_subdiv, false, false, 0);
  this->main_box.pack_start(this->linear_subdiv, false, false, 0);
  this->main_box.pack_start(*slicing_tbl, false, false, 0);
  this->main_box.pack_start(num_slices[0], false, false, 0);
  this->main_box.pack_start(num_slices[1], false, false, 0);
  this->main_box.pack_start(num_slices[2], false, false, 0);

  this->defaults();
  this->set_title("Oversampling Configuration");
}

/* ---------------------------------------------------------------- */

void
GtkOversamplingConfig::change_planes (bool increase)
{
  std::size_t px = (std::size_t)this->num_slices[0].get_value();
  std::size_t py = (std::size_t)this->num_slices[1].get_value();
  std::size_t pz = (std::size_t)this->num_slices[2].get_value();

  if (increase)
  {
    px = (px * 2) + 1;
    py = (py * 2) + 1;
    pz = (pz * 2) + 1;
  }
  else
  {
    px = (px - 1) / 2;
    py = (py - 1) / 2;
    pz = (pz - 1) / 2;
  }

  this->num_slices[0].set_value((double)px);
  this->num_slices[1].set_value((double)py);
  this->num_slices[2].set_value((double)pz);
}

/* ---------------------------------------------------------------- */

void
GtkOversamplingConfig::apply (void)
{
  this->config.use_loop_subdiv = this->loop_subdiv.get_active();
  this->config.use_linear_subdiv = this->linear_subdiv.get_active();
  this->config.use_slicing = this->grid_slicing.get_active();
  this->config.lazy_triangulation = this->lazy_triangulation.get_active();
  this->config.slices_x = (std::size_t)this->num_slices[0].get_value();
  this->config.slices_y = (std::size_t)this->num_slices[1].get_value();
  this->config.slices_z = (std::size_t)this->num_slices[2].get_value();
}

/* ---------------------------------------------------------------- */

void
GtkOversamplingConfig::revert (void)
{
  this->loop_subdiv.set_active(this->config.use_loop_subdiv);
  this->linear_subdiv.set_active(this->config.use_linear_subdiv);
  this->grid_slicing.set_active(this->config.use_slicing);
  this->lazy_triangulation.set_active(this->config.lazy_triangulation);
  this->num_slices[0].set_value((double)this->config.slices_x);
  this->num_slices[1].set_value((double)this->config.slices_y);
  this->num_slices[2].set_value((double)this->config.slices_z);
}

/* ---------------------------------------------------------------- */

void
GtkOversamplingConfig::defaults (void)
{
  this->config = Remesher::OversamplingConf();
  this->revert();
}

/* ================================================================ */

GtkDensityFieldConfig::GtkDensityFieldConfig (void)
{
  /* Density field settings. */
  this->density_factors.set_increments(0.01, 0.1);
  this->density_factors.set_range(0.0, 1.0);
  this->density_factors.set_digits(2);
  this->density_factors.set_value_pos(Gtk::POS_RIGHT);

  this->max_curvature.set_increments(10.0, 100.0);
  this->max_curvature.set_range(10.0, 10000.0);
  this->max_curvature.set_digits(0);
  this->max_curvature.set_value_pos(Gtk::POS_RIGHT);

  this->contrast_exponent.set_increments(0.1, 0.4);
  this->contrast_exponent.set_range(0.0, 10.0);
  this->contrast_exponent.set_digits(2);
  this->contrast_exponent.set_value_pos(Gtk::POS_RIGHT);

  this->smoothing_factor.set_increments(0.01, 0.1);
  this->smoothing_factor.set_range(0.0, 1.0);
  this->smoothing_factor.set_digits(2);
  this->smoothing_factor.set_value_pos(Gtk::POS_RIGHT);

  this->smoothing_iter.set_increments(1.0, 5.0);
  this->smoothing_iter.set_range(0.0, 100.0);
  this->smoothing_iter.set_digits(0);
  this->smoothing_iter.set_value_pos(Gtk::POS_RIGHT);

  this->no_feature_density.set_label("No density for feature vertices");
  this->features_by_angle.set_label("Treat features by angle on the crease");
  this->feature_angle.set_increments(1.0, 5.0);
  this->feature_angle.set_range(0.0, 180.0);
  this->feature_angle.set_digits(0);
  this->feature_angle.set_value_pos(Gtk::POS_RIGHT);

  Gtk::Label* density_factor_label = MK_LABEL("Density factors");
  Gtk::Label* max_curvature_label = MK_LABEL("Maxiumum curvature");
  Gtk::Label* contrast_exp_label = MK_LABEL("Contrast exponent");
  Gtk::Label* smoothing_label = MK_LABEL("Smoothing factor and iterations");
  density_factor_label->set_alignment(Gtk::ALIGN_LEFT);
  max_curvature_label->set_alignment(Gtk::ALIGN_LEFT);
  contrast_exp_label->set_alignment(Gtk::ALIGN_LEFT);
  smoothing_label->set_alignment(Gtk::ALIGN_LEFT);

  Gtk::VBox* basic_conf_box = MK_VBOX;
  basic_conf_box->set_border_width(5);
  basic_conf_box->pack_start(*density_factor_label, false, false, 0);
  basic_conf_box->pack_start(this->density_factors, false, false, 0);
  basic_conf_box->pack_start(*max_curvature_label, false, false, 0);
  basic_conf_box->pack_start(this->max_curvature, false, false, 0);
  basic_conf_box->pack_start(*contrast_exp_label, false, false, 0);
  basic_conf_box->pack_start(this->contrast_exponent, false, false, 0);
  basic_conf_box->pack_start(*smoothing_label, false, false, 0);
  basic_conf_box->pack_start(this->smoothing_factor, false, false, 0);
  basic_conf_box->pack_start(this->smoothing_iter, false, false, 0);

  Gtk::VBox* feature_conf_box = MK_VBOX;
  feature_conf_box->set_border_width(5);
  feature_conf_box->pack_start(this->no_feature_density, false, false, 0);
  feature_conf_box->pack_start(this->features_by_angle, false, false, 0);
  feature_conf_box->pack_start(this->feature_angle, false, false, 0);

  Gtk::Notebook* notebook = MK_NOTEBOOK;
  notebook->append_page(*basic_conf_box, "Basic config");
  notebook->append_page(*feature_conf_box, "Feature handling");

  this->main_box.pack_start(*notebook);
  this->defaults();
  this->set_title("Density Field Configuration");
}

/* ---------------------------------------------------------------- */

void
GtkDensityFieldConfig::apply (void)
{
  this->config.alpha = (float)this->density_factors.get_value();
  this->config.beta = 1.0f - this->config.alpha;
  this->config.max_curvature = (float)this->max_curvature.get_value();
  this->config.contrast_exp = (float)this->contrast_exponent.get_value();
  this->config.smooth_factor = (float)this->smoothing_factor.get_value();
  this->config.smooth_iter = (std::size_t)this->smoothing_iter.get_value();
  this->config.features_by_angle = this->features_by_angle.get_active();
  this->config.feature_angle = (float)std::cos(MY_DEG2RAD
      (this->feature_angle.get_value()));
  this->config.no_feature_density = this->no_feature_density.get_active();
}

/* ---------------------------------------------------------------- */

void
GtkDensityFieldConfig::revert (void)
{
  this->density_factors.set_value(this->config.alpha);
  this->max_curvature.set_value(this->config.max_curvature);
  this->contrast_exponent.set_value(this->config.contrast_exp);
  this->smoothing_factor.set_value(this->config.smooth_factor);
  this->smoothing_iter.set_value((double)this->config.smooth_iter);
  this->features_by_angle.set_active(this->config.features_by_angle);
  this->feature_angle.set_value((double)MY_RAD2DEG(std::acos
      (this->config.feature_angle)));
  this->no_feature_density.set_active(this->config.no_feature_density);
}

/* ---------------------------------------------------------------- */

void
GtkDensityFieldConfig::defaults (void)
{
  this->config = Remesher::DensityFieldConf();
  this->revert();
}

/* ================================================================ */

GtkFeatureEdgesConfig::GtkFeatureEdgesConfig (void)
{
  /* Feature extraction page. */
  this->use_angle.set_label("Use dilatheral angle");
  this->border_edges.set_label("Mesh boundaries become features");
  this->complex_edges.set_label("Complex edges become features");
  this->dihedral_angle.set_increments(1.0, 10.0);
  this->dihedral_angle.set_range(10.0, 360.0);
  this->dihedral_angle.set_digits(0);
  this->dihedral_angle.set_value_pos(Gtk::POS_RIGHT);

  this->extract_contours.set_label("Extract contours");
  this->angle_theta.set_increments(MY_PI / 20.0, MY_PI / 10.0);
  this->angle_theta.set_range(0.0, MY_PI);
  this->angle_theta.set_digits(4);
  this->angle_theta.set_value_pos(Gtk::POS_RIGHT);
  this->angle_phi.set_increments(MY_PI / 20.0, MY_PI / 10.0);
  this->angle_phi.set_range(0.0, MY_PI);
  this->angle_phi.set_digits(4);
  this->angle_phi.set_value_pos(Gtk::POS_RIGHT);

  Gtk::VBox* feature_extract_box = MK_VBOX;
  feature_extract_box->set_border_width(5);
  feature_extract_box->pack_start(this->use_angle, false, false, 0);
  feature_extract_box->pack_start(this->dihedral_angle, false, false, 0);
  feature_extract_box->pack_start(this->border_edges, false, false, 0);
  feature_extract_box->pack_start(this->complex_edges, false, false, 0);
  feature_extract_box->pack_start(this->extract_contours, false, false, 0);
  feature_extract_box->pack_start(this->angle_theta, false, false, 0);
  feature_extract_box->pack_start(this->angle_phi, false, false, 0);

  /* Load / save / clear buttons. */
  Gtk::Button* load_but = MK_BUT("Load");
  Gtk::Button* save_but = MK_BUT("Save");
  Gtk::Button* clear_but = MK_BUT("Clear");

  Gtk::HBox* but_box = MK_HBOX;
  but_box->pack_start(*load_but, false, false, 0);
  but_box->pack_start(*save_but, false, false, 0);
  but_box->pack_start(*clear_but, false, false, 0);

  /* Feature manual entry page. */
  this->feature_add_v1.set_width_chars(4);
  this->feature_add_v2.set_width_chars(4);
  Gtk::Label* v1_label = MK_LABEL("V1:");
  Gtk::Label* v2_label = MK_LABEL("V2:");
  Gtk::Button* add_but = MK_BUT0;
  add_but->set_image(*MK_IMG(Gtk::Stock::ADD, Gtk::ICON_SIZE_BUTTON));

  Gtk::HBox* feature_add_box = MK_HBOX;
  feature_add_box->pack_start(*v1_label, false, false, 0);
  feature_add_box->pack_start(this->feature_add_v1, true, true, 0);
  feature_add_box->pack_start(*v2_label, false, false, 0);
  feature_add_box->pack_start(this->feature_add_v2, true, true, 0);
  feature_add_box->pack_start(*add_but, false, false, 0);

  Gtk::VBox* feature_manual_box = MK_VBOX;
  feature_manual_box->set_border_width(5);
  feature_manual_box->pack_start(*feature_add_box, false, false, 0);
  feature_manual_box->pack_start(*but_box, false, false, 0);

  /* Notebook. */
  Gtk::Notebook* notebook = MK_NOTEBOOK;
  notebook->append_page(*feature_extract_box, "Extraction");
  notebook->append_page(*feature_manual_box, "Manual");

  this->main_box.pack_start(*notebook, true, true, 0);

  add_but->signal_clicked().connect(sigc::mem_fun
      (*this, &GtkFeatureEdgesConfig::on_feature_added));
  load_but->signal_clicked().connect(sigc::bind(sigc::mem_fun
      (*this, &GtkFeatureEdgesConfig::load_from_file), "/tmp/in.features"));
  save_but->signal_clicked().connect(sigc::bind(sigc::mem_fun
      (*this, &GtkFeatureEdgesConfig::save_to_file), "/tmp/out.features"));

  this->defaults();
  this->set_title("Feature Edges Configuration");
}

/* ---------------------------------------------------------------- */

void
GtkFeatureEdgesConfig::apply (void)
{
  this->config.use_angle = this->use_angle.get_active();
  this->config.angle = (float)std::cos
      (MY_DEG2RAD(this->dihedral_angle.get_value()));
  this->config.border_edges = this->border_edges.get_active();
  this->config.complex_edges = this->complex_edges.get_active();

  this->config.extract_contours = this->extract_contours.get_active();
  this->config.angle_theta = (float)this->angle_theta.get_value();
  this->config.angle_phi = (float)this->angle_phi.get_value();
}

/* ---------------------------------------------------------------- */

void
GtkFeatureEdgesConfig::revert (void)
{
  this->use_angle.set_active(this->config.use_angle);
  this->dihedral_angle.set_value(MY_RAD2DEG(std::acos(this->config.angle)));
  this->border_edges.set_active(this->config.border_edges);
  this->complex_edges.set_active(this->config.complex_edges);

  this->extract_contours.set_active(this->config.extract_contours);
  this->angle_theta.set_value(this->config.angle_theta);
  this->angle_phi.set_value(this->config.angle_phi);
}

/* ---------------------------------------------------------------- */

void
GtkFeatureEdgesConfig::defaults (void)
{
  this->config = Remesher::FeatureEdgesConf();
  this->revert();
}

/* ---------------------------------------------------------------- */

void
GtkFeatureEdgesConfig::load_from_file (std::string const& fname)
{
    std::ifstream in(fname.c_str());
    if (!in.good())
    {
        std::cout << "Error opening file for reading" << std::endl;
        return;
    }

    while (!in.eof())
    {
        std::string line;
        std::getline(in, line);

        Remesher::StringVector sv(Remesher::Helpers::split_string(line, ' '));
        if (sv.size() < 2)
          continue;
        std::size_t idx = Remesher::Helpers::get_sizet_from_string(sv[0]);
        for (std::size_t i = 1; i < sv.size(); ++i)
        {
          std::size_t idx2 = Remesher::Helpers::get_sizet_from_string(sv[i]);
          this->features->add_feature_edge(idx, idx2);
        }
    }

    this->sig_features_changed.emit();
}

/* ---------------------------------------------------------------- */

void
GtkFeatureEdgesConfig::save_to_file (std::string const& fname)
{
    std::ofstream out(fname.c_str());
    if (!out.good())
    {
        std::cout << "Error opening file for writing" << std::endl;
        return;
    }

    for (std::size_t i = 0; i < this->features->size(); ++i)
    {
        Remesher::FeatureVertexEdges const& e = this->features[i];
        if (e.empty())
          continue;
        out << i;
        for (std::size_t j = 0; j < e.size(); ++j)
          out << " " << e[j];
        out << std::endl;
    }

    out.close();
}

/* ---------------------------------------------------------------- */

void
GtkFeatureEdgesConfig::on_feature_added (void)
{
  std::size_t v1 = Remesher::Helpers::get_sizet_from_string
      (this->feature_add_v1.get_text());
  std::size_t v2 = Remesher::Helpers::get_sizet_from_string
      (this->feature_add_v2.get_text());

  this->features->add_feature_edge(v1, v2);
  this->sig_features_changed.emit();
}

/* ================================================================ */

GtkRelaxationConfig::GtkRelaxationConfig (void)
{
  this->iterations.set_increments(1.0, 10.0);
  this->iterations.set_range(1.0, 100.0);
  this->iterations.set_digits(0);
  this->iterations.set_value_pos(Gtk::POS_RIGHT);

  this->pn_lifting.set_label("Lift vertices to PN triangle surface");

  Gtk::Label* iterations_label = MK_LABEL("Relaxation iterations");
  iterations_label->set_alignment(Gtk::ALIGN_LEFT);

  this->main_box.pack_start(*iterations_label, false, false, 0);
  this->main_box.pack_start(this->iterations, false, false, 0);
  this->main_box.pack_start(this->pn_lifting, false, false, 0);

  this->defaults();
  this->set_title("Relaxation Configuration");
}

/* ---------------------------------------------------------------- */

void
GtkRelaxationConfig::apply (void)
{
  this->config.iterations = (std::size_t)this->iterations.get_value();
  this->config.pn_lifting = this->pn_lifting.get_active();
}

/* ---------------------------------------------------------------- */

void
GtkRelaxationConfig::revert (void)
{
  this->iterations.set_value((double)this->config.iterations);
  this->pn_lifting.set_active(this->config.pn_lifting);
}

/* ---------------------------------------------------------------- */

void
GtkRelaxationConfig::defaults (void)
{
  this->config = Remesher::RelaxationConf();
  this->revert();
}

/* ================================================================ */

GtkResamplingConfig::GtkResamplingConfig (void)
{
  this->sample_amount.set_digits(0);
  this->sample_amount.set_increments(100, 1000);
  this->sample_amount.set_range(3, 1000000);

  this->decimation.set_label("Perform decimation");

  Gtk::Label* samples_label = MK_LABEL("Amount of samples");
  samples_label->set_alignment(Gtk::ALIGN_LEFT);

  this->main_box.pack_start(*samples_label, false, false, 0);
  this->main_box.pack_start(this->sample_amount, false, false, 0);
  this->main_box.pack_start(this->decimation, false, false, 0);

  this->defaults();
  this->set_title("Resampling Configuration");
}

/* ---------------------------------------------------------------- */

void
GtkResamplingConfig::apply (void)
{
  this->config.sample_amount = (std::size_t)this->sample_amount.get_value();
  this->config.perform_decimation = this->decimation.get_active();
}

/* ---------------------------------------------------------------- */

void
GtkResamplingConfig::revert (void)
{
  this->sample_amount.set_value((double)this->config.sample_amount);
  this->decimation.set_active(this->config.perform_decimation);
}

/* ---------------------------------------------------------------- */

void
GtkResamplingConfig::defaults (void)
{
  this->config = Remesher::ResamplingConf();
  this->revert();
}


