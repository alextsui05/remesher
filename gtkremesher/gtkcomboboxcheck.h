#ifndef GTK_COMBO_CHECK_HEADER
#define GTK_COMBO_CHECK_HEADER

#include <string>
#include <gtkmm/combobox.h>
#include <gtkmm/liststore.h>

struct GtkCBCColumns : public Gtk::TreeModelColumnRecord
{
  Gtk::TreeModelColumn<int> index;
  Gtk::TreeModelColumn<bool> active;
  Gtk::TreeModelColumn<Glib::ustring> name;

  GtkCBCColumns (void);
};

/* ---------------------------------------------------------------- */

class GtkComboBoxCheck : public Gtk::ComboBox
{
  private:
    GtkCBCColumns cols;
    Glib::RefPtr<Gtk::ListStore> store;

  private:
    bool row_separator_func (Glib::RefPtr<Gtk::TreeModel> const& model,
        Gtk::TreeModel::iterator const& iter);
    void on_user_selected (void);

  public:
    GtkComboBoxCheck (void);

    /* Clears all entries and sets a description item. */
    void set_description (std::string const& desc);

    /* Appends a selectable item to the end of the list.
     * Negative indices are reserved for the implementation. */
    void append_item (std::string const& name, int index, bool active);

    /* Returns a bitmask of all selected items, that is the sum of
     * all items bitshifted by its index. */
    int get_bitmask (void) const;
};

/* ---------------------------------------------------------------- */

inline
GtkCBCColumns::GtkCBCColumns (void)
{
  this->add(this->index);
  this->add(this->active);
  this->add(this->name);
}

#endif /* GTK_COMBO_CHECK_HEADER */
