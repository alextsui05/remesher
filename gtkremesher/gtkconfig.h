#ifndef GTK_CONFIG_HEADER
#define GTK_CONFIG_HEADER

#include <gtkmm/window.h>
#include <gtkmm/button.h>
#include <gtkmm/spinbutton.h>
#include <gtkmm/scale.h>
#include <gtkmm/checkbutton.h>

#include "libremesh/oversampling.h"
#include "libremesh/densityfield.h"
#include "libremesh/simplification.h"
#include "libremesh/featureedges.h"
#include "libremesh/relaxlloyd.h"
#include "libremesh/resampling.h"
#include "gtkwidgets.h"

class GtkConfigBase : public Gtk::Window
{
  protected:
    Gtk::Button close_but;
    Gtk::Button save_but;
    Gtk::Button save_close_but;
    Gtk::Button defaults_but;
    Gtk::VBox main_box;

    sigc::signal<void> sig_config_changed;

  protected:
    void on_close_clicked (void);
    void on_save_clicked (void);

    virtual void revert (void) = 0;
    virtual void apply (void) = 0;
    virtual void defaults (void) = 0;

  public:
    GtkConfigBase (void);
    virtual ~GtkConfigBase (void) {}

    sigc::signal<void>& signal_config_changed (void);
};

/* ---------------------------------------------------------------- */

class GtkSimplificationConfig : public GtkConfigBase
{
  private:
    Remesher::SimplificationConf config;

    Gtk::SpinButton vertex_amount;
    Gtk::CheckButton perform_fidelity_checks;
    Gtk::CheckButton perform_area_checks;
    Gtk::HScale fidelity_check_penalty;
    Gtk::HScale area_check_penalty;
    GtkLogHScale init_threshold;
    GtkLogHScale max_threshold;
    Gtk::CheckButton local_feature_detection;
    Gtk::HScale local_feature_angle;
    Gtk::CheckButton keep_global_features;

    Gtk::HScale thres_factor;

  protected:
    void apply (void);
    void revert (void);
    void defaults (void);

  public:
    GtkSimplificationConfig (void);

    Remesher::SimplificationConf const& get_config (void) const;
};

/* ---------------------------------------------------------------- */

class GtkOversamplingConfig : public GtkConfigBase
{
  private:
    Remesher::OversamplingConf config;

    Gtk::CheckButton grid_slicing;
    Gtk::CheckButton lazy_triangulation;
    Gtk::HScale num_slices[3];
    Gtk::CheckButton loop_subdiv;
    Gtk::CheckButton linear_subdiv;

  protected:
    void apply (void);
    void revert (void);
    void defaults (void);
    void change_planes (bool increase);

  public:
    GtkOversamplingConfig (void);
    Remesher::OversamplingConf const& get_config (void) const;
};

/* ---------------------------------------------------------------- */

class GtkDensityFieldConfig : public GtkConfigBase
{
  private:
    Remesher::DensityFieldConf config;

    Gtk::HScale density_factors;
    Gtk::HScale max_curvature;
    Gtk::HScale contrast_exponent;
    Gtk::HScale smoothing_factor;
    Gtk::HScale smoothing_iter;
    Gtk::CheckButton no_feature_density;
    Gtk::CheckButton features_by_angle;
    Gtk::HScale feature_angle;

  protected:
    void apply (void);
    void revert (void);
    void defaults (void);

  public:
    GtkDensityFieldConfig (void);

    Remesher::DensityFieldConf const& get_config (void) const;
};

/* ---------------------------------------------------------------- */

class GtkFeatureEdgesConfig : public GtkConfigBase
{
  public:
    typedef sigc::signal<void> SignalFeaturesChanged;

  private:
    Remesher::FeatureEdgesConf config;
    Remesher::FeatureEdgesPtr features;

    Gtk::CheckButton use_angle;
    Gtk::HScale dihedral_angle;
    Gtk::CheckButton border_edges;
    Gtk::CheckButton complex_edges;

    Gtk::CheckButton extract_contours;
    Gtk::HScale angle_theta;
    Gtk::HScale angle_phi;

    Gtk::Entry feature_add_v1;
    Gtk::Entry feature_add_v2;

    SignalFeaturesChanged sig_features_changed;

  protected:
    void apply (void);
    void revert (void);
    void defaults (void);

    void on_feature_added (void);

  public:
    GtkFeatureEdgesConfig (void);

    Remesher::FeatureEdgesConf const& get_config (void) const;
    void set_features (Remesher::FeatureEdgesPtr features);
    SignalFeaturesChanged& signal_features_changed (void);

    void load_from_file (std::string const& fname);
    void save_to_file (std::string const& fname);
};

/* ---------------------------------------------------------------- */

class GtkRelaxationConfig : public GtkConfigBase
{
  private:
    Remesher::RelaxationConf config;
    Gtk::HScale iterations;
    Gtk::CheckButton pn_lifting;

  protected:
    void apply (void);
    void revert (void);
    void defaults (void);

  public:
    GtkRelaxationConfig (void);

    Remesher::RelaxationConf const& get_config (void) const;
};

/* ---------------------------------------------------------------- */

class GtkResamplingConfig : public GtkConfigBase
{
  private:
    Remesher::ResamplingConf config;
    Gtk::SpinButton sample_amount;
    Gtk::CheckButton decimation;

  protected:
    void apply (void);
    void revert (void);
    void defaults (void);

  public:
    GtkResamplingConfig (void);

    Remesher::ResamplingConf const& get_config (void) const;
};

/* ---------------------------------------------------------------- */

inline sigc::signal<void>&
GtkConfigBase::signal_config_changed (void)
{
  return this->sig_config_changed;
}

inline Remesher::SimplificationConf const&
GtkSimplificationConfig::get_config (void) const
{
  return this->config;
}

inline Remesher::OversamplingConf const&
GtkOversamplingConfig::get_config (void) const
{
  return this->config;
}

inline Remesher::DensityFieldConf const&
GtkDensityFieldConfig::get_config (void) const
{
  return this->config;
}

inline Remesher::FeatureEdgesConf const&
GtkFeatureEdgesConfig::get_config (void) const
{
  return this->config;
}

inline void
GtkFeatureEdgesConfig::set_features (Remesher::FeatureEdgesPtr features)
{
  this->features = features;
}

inline GtkFeatureEdgesConfig::SignalFeaturesChanged&
GtkFeatureEdgesConfig::signal_features_changed (void)
{
  return this->sig_features_changed;
}

inline Remesher::RelaxationConf const&
GtkRelaxationConfig::get_config (void) const
{
  return this->config;
}

inline Remesher::ResamplingConf const&
GtkResamplingConfig::get_config (void) const
{
  return this->config;
}

#endif /* GTK_CONFIG_HEADER */
