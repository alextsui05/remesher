#ifndef GTK_WIDGETS_HEADER
#define GTK_WIDGETS_HEADER

#include <iomanip>
#include <sstream>
#include <gtkmm/scale.h>

class GtkLogHScale : public Gtk::HScale
{
  protected:
    Glib::ustring on_format_value (double value);

  public:
    double get_value (void) const;
    void set_value (double value);
};

/* ---------------------------------------------------------------- */

inline Glib::ustring
GtkLogHScale::on_format_value (double value)
{
  value = ::pow10(-value);
  std::stringstream s;
  s << std::fixed << std::setprecision(7) << value;
  return s.str();
}

inline double
GtkLogHScale::get_value (void) const
{
  return ::pow10(-this->Gtk::HScale::get_value());
}

inline void
GtkLogHScale::set_value (double value)
{
  this->Gtk::HScale::set_value(-::log10(value));
}

#endif /* GTK_WIDGETS_HEADER */
